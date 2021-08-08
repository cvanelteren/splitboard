#include <cstdio>
#include <iostream>
#include <vector>

using namespace std;

namespace test {
static vector<vector<vector<uint8_t>>> vec = {{{1, 2, 3}, {4, 5, 6}},
                                              {{7, 8, 9}, {10, 11, 12}}};
}

int main() {

  vector<vector<uint8_t>> *b = &test::vec[0];
  vector<uint8_t> *c = &test::vec[0][0];
  b++;
  c++;
  // b--;
  printf("%d\n", (*b)[0][0]);
  printf("%d\n", (*c)[0]);
};
