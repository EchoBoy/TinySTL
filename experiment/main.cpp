// 一些C++知识的整理

#include <iostream>
#include <vector>

using std::cout;
using std::endl;

/// 多态(polymorphically)的使用 与 虚函数，JAVA中默认为虚。
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

/// const_case 莫名其妙
void test2() {
  const int a = 0;
  const int *p = &a;
  int *q = const_cast<int *>(p);
  *q = 1;
  cout << "const a: " << a << endl;
  cout << "const *p: " << *p << endl;
}

/// reference vector faild
void test3() {
  std::vector<int *> vec;
  int tmp = 0;
  int *a = &tmp;

  vec.push_back(a);
  vec.push_back(a);
  cout << vec[0] << " " << vec[1] << endl;
  *a = 1;
  cout << vec[0] << " " << vec[1] << endl;
}

int main(int, char **) {
  test3();
  return 0;
}
