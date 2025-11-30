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
    std::list<block> blocks_;  // список блоков
  
    void mergeFreeBlocks();

  public:
    // Выделение памяти с заданным выравниванием
    void *do_allocate(size_t bytes, size_t alignment) override;

    // Освобождение памяти
    void do_deallocate(void *pointer, size_t bytes, size_t alignment) override;

    // Сравнение с другим memory_resource
    bool do_is_equal(const std::pmr::memory_resource &other) const noexcept override;
};

#endif
