#include "../include/Stack.hpp"
#include "../include/Person.hpp"
#include <iostream>

#define TEN_KB 10240

void testDefault(CustomMemoryResource &mr) {
  std::cout << "\n=====> Test of Stack <=====\n";
  
  {
    Stack<int> intStack(&mr);
    
    for (int i = 1; i != 10; ++i) {
      intStack.push(i * 10);
    }
    
    std::cout << "Size of Stack: " << intStack.size() << "\n";
    std::cout << "Top of Stack: " << intStack.top() << "\n";
    
    std::cout << "Elements of Stack (from up to down): ";
    for (const auto& item : intStack) {
      std::cout << item << " ";
    }
    std::cout << "\n";
    
    for (int i = 0; i < 3; ++i) {
      std::cout << "Delete: " << intStack.top() << "\n";
      intStack.pop();
    }
    
    std::cout << "New size: " << intStack.size() << "\n";
  }
  
  mr.print_stats();
}

void testForPerson(CustomMemoryResource &mr) {
  std::cout << "\n=====> Test of Stack for Person <=====\n";
  
  {
    Stack<Person> personStack(&mr);
    
    personStack.emplace("Alice", 30, 50000.0);
    personStack.emplace("Bob", 25, 45000.0);
    personStack.emplace("Charlie", 35, 60000.0);
    
    std::cout << "Size of Stack: " << personStack.size() << "\n";
    std::cout << "Top element of Stack: " << personStack.top() << "\n";
    
    std::cout << "Element of Stack (from up to down):\n";
    for (const auto& person : personStack) {
      std::cout << "  " << person << "\n";
    }
    
    std::cout << "Delete: " << personStack.top() << "\n";
    personStack.pop();
    std::cout << "New size: " << personStack.size() << "\n";
  }

  mr.print_stats();
}

void testRepeatedUsingOfMemory(CustomMemoryResource &mr) {
  std::cout << "\n=====> Test repeated using of memory <=====\n";
    
  {
    Stack<int> Stack1(&mr);
    for (int i = 1; i != 5; ++i) {
      Stack1.push(i);
    }
    std::cout << "First Stack created, size: " << Stack1.size() << "\n";
  }
  mr.print_stats();
    
  {
    Stack<int> Stack2(&mr);
    for (int i = 10; i != 20; ++i) {
      Stack2.push(i);
    }
    std::cout << "Second Stack created, size: " << Stack2.size() << "\n";
  } 
  mr.print_stats();
}

void testFinish(CustomMemoryResource &mr) {
  std::cout << "[End of the program]\n";
  mr.print_stats();
}

int main() {
  try {
    CustomMemoryResource mr(TEN_KB);
    testDefault(mr);
    testForPerson(mr);
    testRepeatedUsingOfMemory(mr);
    testFinish(mr);
  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }
  return 0;
}
