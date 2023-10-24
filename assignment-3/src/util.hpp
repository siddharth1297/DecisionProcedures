#ifndef __UTIL_H__
#define __UTIL_H__

#include <algorithm>
#include <unordered_set>
#include <vector>

/*
 * https://stackoverflow.com/questions/4077609/overloading-output-stream-operator-for-vectort
 */
template <class T>
std::ostream &operator<<(std::ostream &os, const std::unordered_set<T> &s) {
  size_t i = 0;
  os << "[";
  for (typename std::unordered_set<T>::const_iterator ii = s.begin();
       ii != s.end(); ++ii) {
    os << *ii;
    if (i + 1 < s.size()) {
      os << ", ";
    }
    i++;
  }
  os << "]";
  return os;
}

template <class T>
std::ostream &operator<<(std::ostream &os, const std::vector<T> &s) {
  size_t i = 0;
  os << "[";
  for (typename std::vector<T>::const_iterator ii = s.begin(); ii != s.end();
       ++ii) {
    os << *ii;
    if (i + 1 < s.size()) {
      os << ", ";
    }
    i++;
  }
  os << "]";
  return os;
}

template <class T>
std::ostream &operator<<(std::ostream &os, const std::pair<T, T> &p) {
  os << "[" << p.first << ", " << p.second << "]";
  return os;
}

template <class T>
std::vector<int> VectorSetUnion(const std::vector<T> &v1,
                                const std::vector<T> &v2) {
  std::vector<int> out(v1.size() + v2.size());
  auto it =
      std::set_union(v1.begin(), v1.end(), v2.begin(), v2.end(), out.begin());
  out.resize(it - out.begin());
  return out;
}
#endif
