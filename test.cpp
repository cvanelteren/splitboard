
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

#define layer_tap 4096
#define LT(key, layer) (key | layer_tap | (layer & 0xf) << 8)
// Function to toggle bits in the given range
int toggleBitsFromLToR(int n, int l, int r) {
  // calculating a number 'num' having 'r' number of bits
  // and bits in the range l to r are the only set bits
  int num = ((1 << r) - 1) ^ ((1 << (l - 1)) - 1);

  // toggle the bits in the range l to r in 'n'
  // and return the number
  return (n ^ num);
}

// Function to unset bits in the given range
int unsetBitsInGivenRange(int n, int l, int r) {
  // 'num' is the highest positive integer number
  // all the bits of 'num' are set
  long num = (1ll << (4 * 8 - 1)) - 1;

  // toggle the bits in the range l to r in 'num'
  num = toggleBitsFromLToR(num, l, r);

  // unset the bits in the range l to r in 'n'
  // and return the number
  return (n & num);
}

int main() {

  uint16_t x = LT(2, 3);
  // x = 0x01;
  // uint16_t x = 0x01;

  // unset first 8 bits
  uint16_t bitmask = ((1 << 8) - 1) ^ ((1 << (17 - 1)) - 1);

  printf("%d %d \n", x, x & bitmask);
  size_t tmp;
  switch (unsetBitsInGivenRange(x, 12, 0)) {
  case (layer_tap):
    printf("layer tap\n");
    tmp = unsetBitsInGivenRange(unsetBitsInGivenRange(x, 16, 12), 8, 0) >> 8;
    printf("%d\n", tmp);
    switch (tmp) {
    case 3:
      printf("Layer 3 \n");
      break;
    default:
      printf("No layer\n");
    }

    break;
  default:
    printf("Default \n!");
    break;
  }

  // printf("test %d  \n", b);

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
