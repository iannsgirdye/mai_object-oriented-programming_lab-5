#ifndef PERSON_HPP
#define PERSON_HPP

#include <string>
#include <iostream>

struct Person {
  std::string name_;
  int age_;
  double salary_;
  
  Person() : name_(""), age_(0), salary_(0.0) {}
  
  Person(const std::string &name, int age, double salary) : name_(name), age_(age), salary_(salary) {}
  
  friend std::ostream& operator<<(std::ostream& out, const Person& person) {
    out << "Person{name: " << person.name_ << ", age: " << person.age_ << ", salary: " << person.salary_ << "}";
    return out;
  }
};

#endif
