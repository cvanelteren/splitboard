#include <cstdio>
#include <iostream>
#include <vector>

using namespace std;

int main() {
  vector<vector<vector<int>>> vec = {{{1, 2, 3}, {4, 5, 6}},
                                     {{7, 8, 9}, {10, 11, 12}}};

  vector<vector<int>> *b = &vec[0];
  vector<int> *c = &vec[0][0];
  b++;
  c++;
  // b--;
  printf("%d\n", (*b)[0][0]);
  printf("%d\n", (*c)[0]);
};
