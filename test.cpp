
#include <cstdint>
#include <cstdio>
#include <stdint.h>
#include <stdio.h>
#include <typeinfo>
#include <vector>

#include <map>
#include <string>

class A {
public:
  int a;
  virtual void print() const = 0;

protected:
  void default_print() const {
    printf("%s \n", typeid(this).name());
    printf("in A\n");
  }
};

class B : public A {
public:
  void print() const override {
    printf("%s \n", typeid(this).name());
    printf("in B\n");
  }
};

class C : public A {
public:
  void print() const override {
    printf("%s \n", typeid(this).name());
    printf("in C\n");
  }
};

typedef struct {
  int x;
  int y;
} test;

typedef struct {
  int *x;
  int *y;
} rtest;

typedef uint8_t bla[2];
const bla D = {0, 1};

class Test {
public:
  void func() { printf("Hello\n"); }
};

uint8_t downcast(size_t x) { return x; };
int main() {

  Test *tester = new Test();
  void (Test::*fptr)() = &Test::func;

  (tester->*fptr)();
  // printf("test %d  \n", b);

  uint16_t x = 256;

  auto a = downcast(x);

  printf("%d %d \n", x, a);

  // std::vector<const bla *> E;
  // E.push_back(&D);

  // static std::map<std::string, std::map<std::string, int>> defaults = {
  //     {std::string("Definitely a string"),
  //      {{std::string("This too"), 0}, {"test", 1}}},
  // };

  // t.x = -1;
  // printf(">> test %d \n", *rt.x);
  // C c = C();
  // B b = B();

  // std::vector<A *> as;
  // as.push_back(&b);
  // as.push_back(&c);

  // printf("%d\n", as.size());
  // printf("%d %d %d \n", sizeof(as), sizeof(c), sizeof(b));

  // printf("%d\n", sizeof((uint8_t *)&as));

  // auto x = (uint8_t *)&as;
  // auto z = (std::vector<A *> *)x;
  // for (auto elem : *z) {
  //   elem->print();
  // }
  // for (auto &elem : as) {
  //   elem->print();
  // }
}
