#ifndef CUSTOM_STACK_HPP
#define CUSTOM_STACK_HPP

#include <memory_resource>
#include <stdexcept>

template <class T>
class CustomStack {
  private:
    struct Node {
      T data_;
      Node *next_ = nullptr;

      template<typename ...Args>
      Node(Args &&...args): data_(std::forward<Args>(args)...) {}
    };
  
    Node *head_;
    std::pmr::polymorphic_allocator<Node> allocator_;
    size_t size_;
    
  public:
    class iterator {
      private:
        Node *current_;

      public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T *;
        using reference = T &;
      
        explicit iterator(Node *node): current_(node) {}

        reference operator*() const {
          return current_->data_;
        }

        pointer operator->() const {
          return &(current_->data_);
        }

        iterator &operator++() {
          if (current_) {
            current_ = current_->next_;
          }
          return *this;
        }

        iterator operator++(int) {
          iterator temp = *this;
          ++(*this);
          return temp;
        }

        bool operator==(const iterator &other) const {
          return current_ == other.current_;
        }
        
        bool operator!=(const iterator &other) const {
          return current_ != other.current_;
        }
    };

    explicit CustomStack(std::pmr::memory_resource *mr): head_(nullptr), allocator_(mr), size_(0) {}
    CustomStack(): head_(nullptr), allocator_(std::pmr::get_default_resource()), size_(0) {}

    CustomStack(const CustomStack &) = delete;             // Запрет копирования
    CustomStack &operator=(const CustomStack &) = delete;  //

    // Перемещение
    CustomStack(CustomStack &&other) noexcept: head_(other.head_), allocator_(other.allocator_), size_(other.size_) {
      other.head_ = nullptr;
      other.size_ = 0;
    }

    CustomStack &operator=(CustomStack &&other) noexcept {
      if (this != &other) {
        clear();
        head_ = other.head_;
        allocator_ = other.allocator_;
        size_ = other.size_;
        other.head_ = nullptr;
        other.size_ = 0;
      }
      return *this;
    }

    ~CustomStack() {
      clear();
    }

    template<typename ...Args>
    void emplace(Args &&...args) {
      Node *new_node = allocator_.allocate(1);
      try {
        allocator_.construct(new_node, std::forward<Args>(args)...);
        new_node->next_ = head_;
        head_ = new_node;
        ++size_;
      } catch (...) {
        allocator_.deallocate(new_node, 1);
        throw;
      }
    }

    void push(const T &value) {
      emplace(value);
    }

    void push(T &&value) {
      emplace(std::move(value));
    }

    void pop() {
      if (empty()) {
        throw std::runtime_error("Stack is empty");
      }
      Node *node_to_delete = head_;
      head_ = head_->next_;
      allocator_.destroy(node_to_delete);
      allocator_.deallocate(node_to_delete, 1);
      --size_;
    }

    T &top() {
      if (empty()) {
        throw std::runtime_error("Stack is empty");
      }
      return head_->data_;
    }

    const T &top() const {
      if (empty()) {
        throw std::runtime_error("Stack is empty");
      }
      return head_->data_;
    }

    bool empty() const {
      return head_ == nullptr;
    }
    
    size_t size() const {
      return size_;
    }

    void clear() {
      while (!empty()) {
        pop();
      }
    }

    iterator begin() {
      return iterator(head_);
    }

    iterator end() {
      return iterator(nullptr);
    }
};

#endif
