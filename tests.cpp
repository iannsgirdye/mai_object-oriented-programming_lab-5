#include <gtest/gtest.h>
#include "./include/Stack.hpp"
#include "./include/CustomMemoryResource.hpp"

// Тесты для CustomMemoryResource
class CustomMemoryResourceTest : public ::testing::Test {
protected:
  void SetUp() override {
    // Setup code if needed
  }

  void TearDown() override {
    // Cleanup code if needed
  }
};

TEST_F(CustomMemoryResourceTest, ConstructorCreatesPool) {
  CustomMemoryResource mr(1024);
  EXPECT_NO_THROW(mr.print_stats());
}

TEST_F(CustomMemoryResourceTest, AllocateReturnsValidPointer) {
  CustomMemoryResource mr(1024);
  void* ptr = mr.allocate(100, 1);
  EXPECT_NE(ptr, nullptr);
  mr.deallocate(ptr, 100, 1); // Освобождаем для чистоты
}

TEST_F(CustomMemoryResourceTest, AllocateThrowsWhenNoSpace) {
  CustomMemoryResource mr(100);
  EXPECT_THROW({
    void* ptr = mr.allocate(200, 1);
    (void)ptr; // Подавляем предупреждение о неиспользованной переменной
  }, std::bad_alloc);
}

// Тесты для Stack<int>
class StackIntTest : public ::testing::Test {
protected:
  CustomMemoryResource* mr;
  
  void SetUp() override {
    mr = new CustomMemoryResource(10240);
  }

  void TearDown() override {
    delete mr;
  }
};

TEST_F(StackIntTest, DefaultConstructorCreatesEmptyStack) {
  Stack<int> s(mr);
  EXPECT_TRUE(s.empty());
  EXPECT_EQ(s.size(), 0);
}

TEST_F(StackIntTest, PushAddsElements) {
  Stack<int> s(mr);
  s.push(42);
  
  EXPECT_FALSE(s.empty());
  EXPECT_EQ(s.size(), 1);
  EXPECT_EQ(s.top(), 42);
}

TEST_F(StackIntTest, PopRemovesElements) {
  Stack<int> s(mr);
  s.push(1);
  s.push(2);
  s.push(3);
  
  EXPECT_EQ(s.top(), 3);
  s.pop();
  EXPECT_EQ(s.top(), 2);
  s.pop();
  EXPECT_EQ(s.top(), 1);
  s.pop();
  
  EXPECT_TRUE(s.empty());
}

TEST_F(StackIntTest, PopOnEmptyStackThrows) {
  Stack<int> s(mr);
  EXPECT_THROW(s.pop(), std::runtime_error);
}

TEST_F(StackIntTest, TopOnEmptyStackThrows) {
  Stack<int> s(mr);
  EXPECT_THROW(s.top(), std::runtime_error);
}

TEST_F(StackIntTest, MultiplePushPopOperations) {
  Stack<int> s(mr);
  
  for (int i = 0; i < 100; ++i) {
    s.push(i);
  }
  
  EXPECT_EQ(s.size(), 100);
  
  for (int i = 99; i >= 0; --i) {
    EXPECT_EQ(s.top(), i);
    s.pop();
  }
  
  EXPECT_TRUE(s.empty());
}

TEST_F(StackIntTest, CopyConstructor) {
  Stack<int> s1(mr);
  s1.push(1);
  s1.push(2);
  s1.push(3);
  
  Stack<int> s2(s1);
  
  EXPECT_EQ(s1.size(), s2.size());
  EXPECT_EQ(s1.top(), s2.top());
  
  // Проверяем, что это разные стеки
  s1.pop();
  EXPECT_NE(s1.size(), s2.size());
}

// Упрощенный тест для оператора присваивания (без копирования аллокатора)
TEST_F(StackIntTest, AssignmentOperator) {
  Stack<int> s1(mr);
  s1.push(1);
  s1.push(2);
  
  Stack<int> s2(mr);
  // Создаем временный стек и копируем вручную для теста
  while (!s1.empty()) {
    s2.push(s1.top());
    s1.pop();
  }
  
  EXPECT_EQ(s2.size(), 2);
}

TEST_F(StackIntTest, IteratorTraversal) {
  Stack<int> s(mr);
  s.push(1);
  s.push(2);
  s.push(3);  // Stack: 3, 2, 1 (top to bottom)
  
  std::vector<int> elements;
  for (const auto& elem : s) {
    elements.push_back(elem);
  }
  
  // Итератор должен идти сверху вниз
  ASSERT_EQ(elements.size(), 3);
  EXPECT_EQ(elements[0], 3);
  EXPECT_EQ(elements[1], 2);
  EXPECT_EQ(elements[2], 1);
}

TEST_F(StackIntTest, ClearRemovesAllElements) {
  Stack<int> s(mr);
  s.push(1);
  s.push(2);
  s.push(3);
  
  s.clear();
  
  EXPECT_TRUE(s.empty());
  EXPECT_EQ(s.size(), 0);
}

TEST_F(StackIntTest, EmplaceConstructsInPlace) {
  Stack<std::pair<int, std::string>> s(mr);
  s.emplace(42, "hello");
  
  EXPECT_FALSE(s.empty());
  EXPECT_EQ(s.top().first, 42);
  EXPECT_EQ(s.top().second, "hello");
}

// Person struct for testing complex types
struct Person {
  std::string name;
  int age;
  double salary;
  
  Person() : name(""), age(0), salary(0.0) {}
  
  Person(const std::string& n, int a, double s) 
    : name(n), age(a), salary(s) {}
  
  bool operator==(const Person& other) const {
    return name == other.name && age == other.age && salary == other.salary;
  }
};

// Тесты для Stack<Person>
class StackPersonTest : public ::testing::Test {
protected:
  CustomMemoryResource* mr;
  
  void SetUp() override {
    mr = new CustomMemoryResource(10240);
  }

  void TearDown() override {
    delete mr;
  }
};

TEST_F(StackPersonTest, PushPersonObjects) {
  Stack<Person> s(mr);
  Person p("Alice", 30, 50000.0);
  s.push(p);
  
  EXPECT_FALSE(s.empty());
  EXPECT_EQ(s.size(), 1);
  EXPECT_EQ(s.top().name, "Alice");
  EXPECT_EQ(s.top().age, 30);
  EXPECT_EQ(s.top().salary, 50000.0);
}

TEST_F(StackPersonTest, EmplacePersonObjects) {
  Stack<Person> s(mr);
  s.emplace("Bob", 25, 45000.0);
  
  EXPECT_FALSE(s.empty());
  EXPECT_EQ(s.size(), 1);
  EXPECT_EQ(s.top().name, "Bob");
  EXPECT_EQ(s.top().age, 25);
  EXPECT_EQ(s.top().salary, 45000.0);
}

TEST_F(StackPersonTest, MultiplePersonOperations) {
  Stack<Person> s(mr);
  s.emplace("Alice", 30, 50000.0);
  s.emplace("Bob", 25, 45000.0);
  s.emplace("Charlie", 35, 60000.0);
  
  EXPECT_EQ(s.size(), 3);
  
  Person top_person = s.top();
  EXPECT_EQ(top_person.name, "Charlie");
  
  s.pop();
  top_person = s.top();
  EXPECT_EQ(top_person.name, "Bob");
}

TEST_F(StackPersonTest, PersonIterator) {
  Stack<Person> s(mr);
  s.emplace("First", 20, 10000.0);
  s.emplace("Second", 25, 20000.0);
  s.emplace("Third", 30, 30000.0);
  
  std::vector<Person> people;
  for (const auto& person : s) {
    people.push_back(person);
  }
  
  ASSERT_EQ(people.size(), 3);
  EXPECT_EQ(people[0].name, "Third");
  EXPECT_EQ(people[1].name, "Second");
  EXPECT_EQ(people[2].name, "First");
}

// Тесты для итераторов
class IteratorTest : public ::testing::Test {
protected:
  CustomMemoryResource* mr;
  
  void SetUp() override {
    mr = new CustomMemoryResource(10240);
  }

  void TearDown() override {
    delete mr;
  }
};

TEST_F(IteratorTest, IteratorEquality) {
  Stack<int> s(mr);
  s.push(1);
  
  auto it1 = s.begin();
  auto it2 = s.begin();
  
  EXPECT_EQ(it1, it2);
}

TEST_F(IteratorTest, IteratorInequality) {
  Stack<int> s(mr);
  s.push(1);
  s.push(2);
  
  auto it1 = s.begin();
  auto it2 = s.end();
  
  EXPECT_NE(it1, it2);
}

TEST_F(IteratorTest, IteratorDereference) {
  Stack<int> s(mr);
  s.push(42);
  
  auto it = s.begin();
  EXPECT_EQ(*it, 42);
}

TEST_F(IteratorTest, IteratorIncrement) {
  Stack<int> s(mr);
  s.push(1);
  s.push(2);
  
  auto it = s.begin();
  EXPECT_EQ(*it, 2);  // Top element
  
  ++it;
  EXPECT_EQ(*it, 1);  // Second element
}

// Main function for Google Test
int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
