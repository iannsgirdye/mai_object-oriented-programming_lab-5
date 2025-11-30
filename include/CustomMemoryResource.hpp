#ifndef CUSTOM_MEMORY_RESOURCE_HPP
#define CUSTOM_MEMORY_RESOURCE_HPP

#include <memory_resource>
#include <list>

class CustomMemoryResource: public std::pmr::memory_resource {
  private:
    // блок в буфере
    struct block {
      void *ptr_;  // указатель на начало
      size_t size_;    // размер
      bool is_free_;   // статус

      block(void *ptr, size_t size, bool is_free = false): ptr_(ptr), size_(size), is_free_(is_free) {}
    };
    
    void *memory_;             // указатель буфер
    size_t size_;              // размер буфера
    size_t offset_ = 0;        // количество распределённых байтов
    std::list<block> blocks_;  // список блоков
  
    void mergeFreeBlocks();

    void insertFreeBlockBefore(auto iter, size_t aligned_gap) {
      blocks_.insert(iter, block(iter->ptr_, aligned_gap, true));
    }

    void pushFreeBlockBefore(void *remaining_ptr, size_t aligned_gap) {
      blocks_.push_back(block(remaining_ptr, aligned_gap, true));
    }

    void updateCurrentBlock(auto iter, size_t bytes, void *aligned_ptr) {
      iter->ptr_ = aligned_ptr;
      iter->size_ = bytes;
      iter->is_free_ = false;
    }

    void pushCurrentBlock(void *aligned_ptr, size_t bytes) {
      blocks_.push_back(block(aligned_ptr, bytes, false));
    }

    void insertFreeBlockAfter(auto iter, void *aligned_ptr, size_t bytes, size_t aligned_size) {
      const size_t remaining_size = aligned_size - bytes;
      const void *remaining_ptr = static_cast<char *>(aligned_ptr) + bytes;
      blocks_.insert(std::next(iter), block(remaining_ptr, remaining_size, true));
    }

  public:
    CustomMemoryResource(size_t size): size_(size), memory_(static_cast<void *>(new char[size])) {}

    // Выделение памяти с заданным выравниванием
    void *do_allocate(size_t bytes, size_t alignment) override {
      if (bytes == 0) {
        return nullptr;
      }
      if (alignment == 0 || (alignment & (alignment - 1)) != 0) {
        throw std::bad_alloc();
      }

      void *aligned_ptr;
      size_t aligned_gap, aligned_size;

      for (auto iter = blocks_.begin(); iter != blocks_.end(); ++iter) {
        if (iter->is_free_ && iter->size_ >= bytes) {
          aligned_ptr = std::align(alignment, bytes, iter->ptr_, iter->size_);
          if (aligned_ptr) {
            aligned_gap = static_cast<char *>(aligned_ptr) - static_cast<char *>(iter->ptr_);
            if (aligned_gap > 0) {
              insertFreeBlockBefore(iter, aligned_gap);
            }
            
            updateCurrentBlock(iter, bytes, aligned_ptr);

            aligned_size = iter->size_ - aligned_gap;
            if (aligned_size > bytes) {
              insertFreeBlockAfter(iter, aligned_ptr, bytes, aligned_size);
            }

            return aligned_ptr;
          }
        }
      }

      size_t remaining_size = size_ - offset_;
      if (remaining_size >= bytes) {
        void *remaining_ptr = static_cast<void *>(static_cast<char *>(memory_) + offset_);
        aligned_ptr = std::align(alignment, bytes, remaining_ptr, remaining_size);
        if (aligned_ptr) {
          aligned_gap = static_cast<char *>(aligned_ptr) - static_cast<char *>(remaining_ptr);
          if (aligned_gap > 0) {
            pushFreeBlockBefore(remaining_ptr, aligned_gap);
          }

          pushCurrentBlock(aligned_ptr, bytes);

          offset_ += aligned_gap + bytes;
          return aligned_ptr;
        }
      }
      
      throw std::bad_alloc();
    }

    // Освобождение памяти
    void do_deallocate(void *ptr, size_t bytes, size_t alignment) override;

    // Сравнение с другим memory_resource
    bool do_is_equal(const std::pmr::memory_resource &other) const noexcept override;
};

#endif
