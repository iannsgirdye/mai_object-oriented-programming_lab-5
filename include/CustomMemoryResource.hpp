#ifndef FIXED_BLOCK_MEMORY_RESOURCE
#define FIXED_BLOCK_MEMORY_RESOURCE

#include <memory_resource>
#include <list>
#include <cstddef>
#include <new>
#include <iostream>
#include <string>
#include <iterator>

class CustomMemoryResource: public std::pmr::memory_resource {
  private:
    void *memory_pool_;
    size_t pool_size_;
    size_t offset_;
    
    struct block_info {
      void *ptr_;
      size_t size_;
      bool is_free_;
      
      block_info(void *ptr, size_t size, bool is_free = false): ptr_(ptr), size_(size), is_free_(is_free) {}
    };
    
    std::list<block_info> allocated_blocks_;

  protected:
    void *do_allocate(size_t bytes, size_t alignment) override {
      for (auto iter = allocated_blocks_.begin(); iter != allocated_blocks_.end(); ++iter) {
        if (iter->is_free_ && iter->size_ >= bytes) {
          size_t aligned_ptr = reinterpret_cast<size_t>(iter->ptr_);
          size_t aligned_offset = (alignment - (aligned_ptr % alignment)) % alignment;
          size_t total_size = bytes + aligned_offset;
          
          if (total_size <= iter->size_) {
            void *result = static_cast<char*>(iter->ptr_) + aligned_offset;
            iter->is_free_ = false;
            
            if (total_size < iter->size_) {
              allocated_blocks_.emplace(
                std::next(iter), 
                static_cast<char*>(iter->ptr_) + total_size, 
                iter->size_ - total_size, 
                true
              );
              iter->size_ = total_size;
            }
            
            return result;
          }
        }
      }
      
      size_t aligned_offset = (alignment - (offset_ % alignment)) % alignment;
      size_t new_offset = offset_ + aligned_offset;
      
      if (new_offset + bytes > pool_size_) {
        throw std::bad_alloc();
      }
      
      void *result = static_cast<char*>(memory_pool_) + new_offset;
      offset_ = new_offset + bytes;
      
      allocated_blocks_.emplace_back(result, bytes, false);
      return result;
    }
    
    void do_deallocate(void *ptr, size_t bytes, size_t alignment) override {
      for (auto &block : allocated_blocks_) {
        if (block.ptr_ == ptr && block.size_ == bytes) {
          block.is_free_ = true;
          return;
        }
      }
    }
    
    bool do_is_equal(const std::pmr::memory_resource &other) const noexcept override {
      return this == &other;
    }

  public:
    explicit CustomMemoryResource(size_t size): pool_size_(size), offset_(0) {
      memory_pool_ = std::malloc(pool_size_);
      if (!memory_pool_) {
        throw std::bad_alloc();
      }
    }
    
    ~CustomMemoryResource() {
      std::free(memory_pool_);
    }
    
    void print_stats() const {
      std::cout << "\nMemory pool stats:\n";
      std::cout << "-> Total size: " << pool_size_ << " bytes\n";
      std::cout << "-> Used offset: " << offset_ << " bytes\n";
      std::cout << "-> Allocated blocks: " << allocated_blocks_.size() << "\n";
      
      size_t free_blocks = 0;
      size_t used_memory = 0;
      for (const auto &block : allocated_blocks_) {
        if (block.is_free_) {
          free_blocks++;
        } else {
          used_memory += block.size_;
        }
      }
      std::cout << "-> Free blocks: " << free_blocks << "\n";
      std::cout << "-> Actually used memory: " << used_memory << " bytes\n";
    }
};

#endif
