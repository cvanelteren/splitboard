#ifndef types_hpp
#define types_hpp
#include <array>
#include <cstddef>
#include <inttypes.h>
#include <stdint.h>
#include <unordered_map>
#include <vector>
struct hash_pair {
  template <class T1, class T2>
  size_t operator()(const std::pair<T1, T2> &p) const {
    auto hash1 = std::hash<T1>{}(p.first);
    auto hash2 = std::hash<T2>{}(p.second);
    return hash1 ^ hash2;
  }
};

typedef struct {
  size_t time;
  bool active;
  uint16_t buffer;
  uint8_t source;
  uint8_t sinc;
  uint8_t col;
  uint8_t row;
  // size_t activation_time;
} keyswitch_t;

// typedef std::pair<size_t, size_t> switch_t;
// typedef std::unordered_map<size_t, std::unordered_map<uint8_t, std::string>>
//     layer_t;

// typedef std::unordered_map<uint8_t, layer_t> layers_t;
//
typedef std::vector<std::vector<uint8_t>> layer_t;
typedef std::vector<std::vector<std::vector<uint8_t>>> layers_t;

typedef std::unordered_map<uint8_t, std::unordered_map<uint8_t, keyswitch_t>>
    matrix_t;

typedef struct {
  std::vector<keyswitch_t> active_keys;
} KeyData;

typedef std::array<keyswitch_t, 6> buffer_t;

#endif
