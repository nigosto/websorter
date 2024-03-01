#include "thread_pool.hpp"
#include <algorithm>
#include <compare>
#include <concepts>
#include <cstddef>
#include <cstdlib>
#include <ctime>
#include <execution>
#include <future>
#include <iterator>
#include <optional>

namespace quicksort_utils {
template <typename T>
constexpr std::iterator_traits<T *>::difference_type THRESHOLD = 10'000;

template <typename T> T choose_pivot(const T *begin, const T *end) {
  std::srand(std::time(nullptr));

  return *(begin + rand() / (RAND_MAX + 1u) / (end - begin - 1));
}

template <typename T>
  requires std::totally_ordered<T>
void quicksort_sequential(T *begin, T *end) {
  if (begin + 1 >= end) {
    return;
  }

  T pivot = choose_pivot(begin, end);
  T *middle1 = std::partition(std::execution::par_unseq, begin, end,
                              [pivot](const T &em) { return em < pivot; });
  T *middle2 = std::partition(std::execution::par_unseq, middle1, end,
                              [pivot](const T &em) { return pivot >= em; });

  quicksort_sequential(begin, middle1);
  quicksort_sequential(middle2, end);
}

template <typename T>
  requires std::totally_ordered<T>
void quicksort(T *begin, T *end) {
  if (begin + 1 >= end) {
    return;
  }

  T pivot = choose_pivot(begin, end);
  T *middle1 = std::partition(std::execution::par_unseq, begin, end,
                              [pivot](const T &em) { return em < pivot; });
  T *middle2 = std::partition(std::execution::par_unseq, middle1, end,
                              [pivot](const T &em) { return pivot >= em; });

  std::optional<std::future<void>> left;
  if (std::distance(begin, middle1) >= THRESHOLD<T>) {
    left = std::async(std::launch::async, [&] { quicksort(begin, middle1); });
  } else {
    quicksort_sequential(begin, middle1);
  }

  std::optional<std::future<void>> right;
  if (std::distance(middle2, end) >= THRESHOLD<T>) {
    right = std::async(std::launch::async, [&] { quicksort(middle2, end); });
  } else {
    quicksort_sequential(middle2, end);
  }

  if (left.has_value()) {
    left.value().wait();
  }

  if (right.has_value()) {
    right.value().wait();
  }
}
} // namespace quicksort_utils