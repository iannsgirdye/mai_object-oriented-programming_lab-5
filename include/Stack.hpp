#ifndef STACK_HPP
#define STACK_HPP

#include "./CustomMemoryResource.hpp"

template<typename T>
class Stack {
  private:
    struct node {
      T data;
      node *next;
      
      template<typename ...Args>
      node(Args &&...args): data(std::forward<Args>(args)...), next(nullptr) {}
    };
    
    node *head_;
    std::pmr::polymorphic_allocator<node> alloc_;
    size_t size_;

  public:
    class iterator {
      private:
        node *current_;
        
      public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T *;
        using reference = T &;
        
        explicit iterator(node *ptr): current_(ptr) {}
        
        reference operator*() const { 
          return current_->data; 
        }

        pointer operator->() const {
          return &current_->data; 
        }
        
        iterator &operator++() {
          current_ = current_->next;
          return *this;
        }
        
        iterator operator++(int) {
          iterator tmp = *this;
          ++(*this);
          return tmp;
        }
        
        bool operator==(const iterator &other) const {
          return current_ == other.current_;
        }
        
        bool operator!=(const iterator &other) const {
          return current_ != other.current_;
        }
    };
    
    explicit Stack(std::pmr::memory_resource *mr = std::pmr::get_default_resource())
      : head_(nullptr), alloc_(mr), size_(0) {}

    ~Stack() {
      clear();
    }
    
    Stack(const Stack &other) : head_(nullptr), alloc_(other.alloc_), size_(0) {
      Stack<T> tempStack(alloc_.resource());
      for (const auto &item : other) {
        tempStack.push(item);
      }
      
      while (!tempStack.empty()) {
        push(tempStack.top());
        tempStack.pop();
      }
    }
    
    Stack &operator=(const Stack &other) {
      if (this != &other) {
        clear();
        alloc_ = other.alloc_;
        
        Stack<T> tempStack(alloc_.resource());
        for (const auto &item : other) {
          tempStack.push(item);
        }
        
        while (!tempStack.empty()) {
          push(tempStack.top());
          tempStack.pop();
        }
      }
      return *this;
    }
    
    void push(const T &value) {
      node *new_node = alloc_.allocate(1);
      alloc_.construct(new_node, value);
      new_node->next = head_;
      head_ = new_node;
      size_++;
    }
    
    template<typename ...Args>
    void emplace(Args &&...args) {
      node *new_node = alloc_.allocate(1);
      alloc_.construct(new_node, std::forward<Args>(args)...);
      new_node->next = head_;
      head_ = new_node;
      size_++;
    }
    
    void pop() {
      if (empty()) {
        throw std::runtime_error("Stack is empty");
      }
      node *temp = head_;
      head_ = head_->next;
      alloc_.destroy(temp);
      alloc_.deallocate(temp, 1);
      size_--;
    }
    
    T &top() {
      if (empty()) {
        throw std::runtime_error("Stack is empty");
      }
      return head_->data;
    }
    
    const T &top() const {
      if (empty()) {
        throw std::runtime_error("Stack is empty");
      }
      return head_->data;
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
    
    iterator begin() const {
      return iterator(head_);
    }
    
    iterator end() const {
      return iterator(nullptr);
    }
};

#endif
