#include <iostream>
#include <string>
#include "../include/CustomMemoryResource.hpp"
#include "../include/CustomStack.hpp"

struct CustomType {
  int digit_;
  double number_;
  char letter_;
  std::string word_;

  CustomType() = default;
  CustomType(int digit, double number, char letter, std::string word):
    digit_(digit), number_(number), letter_(letter), word_(word) {}
};

void testSimpleType() {
  std::cout << "\n===== test simple type =====\n";

  const size_t memory_size = 1024;
  CustomMemoryResource memory_resource(memory_size);
  CustomStack<int> stack(&memory_resource);

  std::cout << "use stack.push():" << std::endl;
  for (size_t i = 0; i != 10; ++i) {
    std::cout << "  stack.push(" << i << ")" << std::endl;
    stack.push(i);
  }

  std::cout << "stack.size(): " << stack.size() << std::endl;
  std::cout << "stack.top(): " << stack.top() << std::endl;

  std::cout << "elements:";
  for (const auto &item : stack) {
    std::cout << " " << item;
  }
  std::cout << std::endl;

  std::cout << "use stack.pop():" << std::endl;
  while (!stack.empty()) {
    std::cout << "  stack.top(): " << stack.top() << std::endl;
    stack.pop();
  }
    
  std::cout << "stack.empty(): " << (stack.empty() ? "true" : "false") << std::endl;
}

void printCustomType(CustomType &item) {
  std::cout << "CustomType(" << item.digit_ << ", " << item.number_ << ", '" << item.letter_ << "', \"" << item.word_ << "\")";
}

void testCustomType() {
  std::cout << "\n===== test custom type =====\n";

  const size_t memory_size = 8192;
  CustomMemoryResource memory_resource(memory_size);
  CustomStack<CustomType> stack(&memory_resource);
  CustomType item;

  std::cout << "use stack.push():" << std::endl;
  for (size_t i = 0; i != 10; ++i) {
    item.digit_ = i;
    item.number_ = i + 123.456;
    item.letter_ = 'a' + i;
    item.word_ = "some_word";

    std::cout << "  stack.push(";
    printCustomType(item);
    std::cout << ")\n";

    stack.push(item);
  }

  std::cout << "stack.size(): " << stack.size() << std::endl;
  
  item = stack.top();
  std::cout << "stack.top(): ";
  printCustomType(item);
  std::cout << std::endl;

  std::cout << "elements:" << std::endl;
  for (auto item : stack) {
    std::cout << "  ";
    printCustomType(item);
    std::cout << std::endl;
  }

  std::cout << "use stack.pop():" << std::endl;
  while (!stack.empty()) {
    std::cout << "  stack.top(): ";
    item = stack.top();
    printCustomType(item);
    std::cout << std::endl;

    stack.pop();
  }
}

int main() {
  testSimpleType();
  testCustomType();

  return 0;
}
