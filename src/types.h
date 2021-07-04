#ifndef types_h
#define types_h
#include <cstddef>
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
  size_t source;
  size_t sinc;
} keyswitch_t;

// typedef std::pair<size_t, size_t> switch_t;
typedef std::unordered_map<size_t, std::unordered_map<size_t, std::string>>
    layer_t;

typedef std::unordered_map<size_t, layer_t> layers_t;

typedef std::unordered_map<size_t, std::unordered_map<size_t, keyswitch_t>>
    matrix_t;

typedef struct {
  std::vector<keyswitch_t> active_keys;

} KeyData;

#endif
