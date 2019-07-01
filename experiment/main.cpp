// 一些C++知识的整理

#include <iostream>

using std::cout;
using std::endl;

// 多态(polymorphically)的使用 与 虚函数，JAVA中默认为虚。
class Animal {
 protected:
  int weight;
 public:
  virtual void walk() {
    std::cout << "work in Animal" << std::endl;
  }
};

class Bird : public Animal {
 public:
  void walk() {
    std::cout << "work in Bird" << std::endl;
  }
};

void test1() {
  Bird a;
  Animal *p = &a;
  p->walk();
}

// const_case 莫名其妙
void test2() {
  const int a = 0;
  const int *p = &a;
  int *q = const_cast<int *>(p);
  *q = 1;
  cout << "const a: " << a << endl;
  cout << "const *p: " << *p << endl;
}

int main(int, char **) {
  cout << "Hello, World!" << endl;
  return 0;
}
