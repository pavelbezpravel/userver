#pragma once

#include <array>
#include <atomic>
#include <memory>

namespace utils {

// This class helps to store pointer. Pointer stored as shared_ptr.
// Whet you call Get, you will get copy of this shared pointer under read lock.
// When you call Set pointer, it will be stored under writelock.
//
// It works slower than SwappingPtr in 5 times, but readers will always have
// shared_ptr that they obtained, so it is impossible that data will be free'd.
template <typename T>
class SwappingSmart {
 public:
  SwappingSmart() : current_(0), write_lock_ ATOMIC_FLAG_INIT {}

  explicit SwappingSmart(const std::shared_ptr<T>& ptr) : SwappingSmart() {
    Set(ptr);
  }

  std::shared_ptr<T> Get() const {
    const short index = current_.load(std::memory_order_relaxed);
    return ptrs_[index];
  }
  void Set(const std::shared_ptr<T>& ptr) {
    std::shared_ptr<T> buf = ptr;
    // wait for write lock
    while (write_lock_.test_and_set(std::memory_order_acquire)) {
    }
    // read current index
    const short index = current_.load(std::memory_order_relaxed);
    // get new index
    const short new_index = 1 - index;
    // store data in new index
    std::swap(ptrs_[new_index], buf);
    // enable new index
    current_.store(new_index, std::memory_order_relaxed);
    // unlock write lock
    write_lock_.clear(std::memory_order_release);
    // old value will be cleaned here, after lock
  }
  void Set(T&& obj) { Set(std::make_shared<T>(std::move(obj))); }
  void Clear() {
    Set(std::make_shared<T>());
    Set(std::make_shared<T>());
  }

 private:
  std::atomic<short> current_;
  std::atomic_flag write_lock_;
  std::array<std::shared_ptr<T>, 2> ptrs_;
};

}  // namespace utils
