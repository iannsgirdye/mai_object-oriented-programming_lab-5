#ifndef CUSTOM_MEMORY_RESOURCE_HPP
#define CUSTOM_MEMORY_RESOURCE_HPP

#include <memory_resource>
#include <list>

class CustomMemoryResource: public std::pmr::memory_resource {
  private:
    // блок в буфере
    struct block {
      void *pointer_;  // указатель на начало
      size_t size_;    // размер
      bool is_free_;   // статус

      block(void *pointer, size_t size, bool is_free = false): pointer_(pointer), size_(size), is_free_(is_free) {}
    };
    
    void *memory_;             // указатель буфер
    size_t size_;              // размер буфера
    size_t offset_;            // количество распределённых байтов
    std::list<block> blocks_;  // список блоков
  
    void mergeFreeBlocks();

    void insertFreeBlockBefore(auto iter, size_t aligned_gap) {
      blocks_.insert(iter, block(iter->pointer_, aligned_gap, true));
    }

    void pushFreeBlockBefore(void *remaining_pointer, size_t aligned_gap) {
      blocks_.push_back(block(remaining_pointer, aligned_gap, true));
    }

    void updateCurrentBlock(auto iter, size_t bytes, void *aligned_pointer) {
      iter->pointer_ = aligned_pointer;
      iter->size_ = bytes;
      iter->is_free_ = false;
    }

    void pushCurrentBlock(void *aligned_pointer, size_t bytes) {
      blocks_.push_back(block(aligned_pointer, bytes, false));
    }

    void insertFreeBlockAfter(auto iter, void *aligned_pointer, size_t bytes, size_t aligned_size) {
      const size_t remaining_size = aligned_size - bytes;
      const void *remaining_pointer = static_cast<char *>(aligned_pointer) + bytes;
      blocks_.insert(std::next(iter), block(remaining_pointer, remaining_size, true));
    }

  public:
    // Выделение памяти с заданным выравниванием
    void *do_allocate(size_t bytes, size_t alignment) override {
      if (bytes == 0) {
        return nullptr;
      }
      if (alignment == 0 || (alignment & (alignment - 1)) != 0) {
        throw std::bad_alloc();
      }

      void *aligned_pointer;
      size_t aligned_gap, aligned_size;

      for (auto iter = blocks_.begin(); iter != blocks_.end(); ++iter) {
        if (iter->is_free_ && iter->size_ >= bytes) {
          aligned_pointer = std::align(alignment, bytes, iter->pointer_, iter->size_);
          if (aligned_pointer) {
            aligned_gap = static_cast<char *>(aligned_pointer) - static_cast<char *>(iter->pointer_);
            if (aligned_gap > 0) {
              insertFreeBlockBefore(iter, aligned_gap);
            }
            
            updateCurrentBlock(iter, bytes, aligned_pointer);

            aligned_size = iter->size_ - aligned_gap;
            if (aligned_size > bytes) {
              insertFreeBlockAfter(iter, aligned_pointer, bytes, aligned_size);
            }

            return aligned_pointer;
          }
        }
      }

      size_t remaining_size = size_ - offset_;
      if (remaining_size >= bytes) {
        void *remaining_pointer = static_cast<void *>(static_cast<char *>(memory_) + offset_);
        aligned_pointer = std::align(alignment, bytes, remaining_pointer, remaining_size);
        if (aligned_pointer) {
          aligned_gap = static_cast<char *>(aligned_pointer) - static_cast<char *>(remaining_pointer);
          if (aligned_gap > 0) {
            pushFreeBlockBefore(remaining_pointer, aligned_gap);
          }

          pushCurrentBlock(aligned_pointer, bytes);

          offset_ += aligned_gap + bytes;
          return aligned_pointer;
        }
      }
      
      throw std::bad_alloc();
    }

    // Освобождение памяти
    void do_deallocate(void *pointer, size_t bytes, size_t alignment) override;

    // Сравнение с другим memory_resource
    bool do_is_equal(const std::pmr::memory_resource &other) const noexcept override;
};

#endif
