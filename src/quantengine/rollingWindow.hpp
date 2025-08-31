#pragma once

#include <vector>
#include <type_traits>
#include <stdexcept>
#include <cstddef>

namespace daniel {

// Keep the last N items in a circular buffer (N must be power of two).
template <typename T>
class RollingWindow {

public:
  explicit RollingWindow(std::size_t capacity_pow2)
    : buf_(capacity_pow2),
      mask_(capacity_pow2 - 1)
  {
    if (capacity_pow2 == 0 || (capacity_pow2 & mask_) != 0) {
      throw std::invalid_argument("capacity must be a power of two (>0)");
    }
  }

  RollingWindow(const RollingWindow&)            = delete;
  RollingWindow& operator=(const RollingWindow&) = delete;
  RollingWindow(RollingWindow&&)                 = delete;
  RollingWindow& operator=(RollingWindow&&)      = delete;

  // Insert a new item (overwrites the oldest once full).
  void push(const T& v) {
    buf_[write_idx_ & mask_] = v;
    ++write_idx_;
    if (count_ < capacity()) ++count_;
  }

  // Window stats
  std::size_t capacity() const { return mask_ + 1; }
  std::size_t count()    const { return count_; }
  bool        empty()    const { return count_ == 0; }
  bool        full()     const { return count_ == capacity(); }
  void        clear()          { write_idx_ = 0; count_ = 0; }

  // Access by "age"
  // Oldest = age 0, Newest = age 0 (from the end)
  const T& at_oldest(std::size_t k = 0) const {
    if (k >= count_) throw std::out_of_range("at_oldest: k >= count()");
    std::size_t oldest_logical = write_idx_ - count_;
    return buf_[(oldest_logical + k) & mask_];
  }
  const T& at_newest(std::size_t k = 0) const {
    if (k >= count_) throw std::out_of_range("at_newest: k >= count()");
    std::size_t newest_logical = write_idx_ - 1;
    return buf_[(newest_logical - k) & mask_];
  }

  // Iterate current window in chronological order (oldest → newest)
  template <class F>
  void for_each(F&& f) const {
    std::size_t oldest_logical = write_idx_ - count_;
    for (std::size_t i = 0; i < count_; ++i) {
      f(buf_[(oldest_logical + i) & mask_]);
    }
  }

private:
  std::vector<T> buf_;
  std::size_t mask_;          // capacity - 1
  std::size_t write_idx_ = 0; // total pushes (monotonic)
  std::size_t count_ = 0; // valid items in window (<= capacity)
};

} 