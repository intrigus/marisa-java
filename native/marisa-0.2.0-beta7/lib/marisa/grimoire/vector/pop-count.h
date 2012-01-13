#ifndef MARISA_GRIMOIRE_VECTOR_POP_COUNT_H_
#define MARISA_GRIMOIRE_VECTOR_POP_COUNT_H_

#include "../../base.h"

namespace marisa {
namespace grimoire {
namespace vector {

template <std::size_t T>
class PopCount;

template <>
class PopCount<32> {
 public:
  explicit PopCount(UInt32 x) : value_() {
    x = (x & 0x55555555U) + ((x & 0xAAAAAAAAU) >> 1);
    x = (x & 0x33333333U) + ((x & 0xCCCCCCCCU) >> 2);
    x = (x + (x >> 4)) & 0x0F0F0F0FU;
    x += x << 8;
    x += x << 16;
    value_ = x;
  }

  std::size_t lo8() const {
    return value_ & 0xFFU;
  }
  std::size_t lo16() const {
    return (value_ >> 8) & 0xFFU;
  }
  std::size_t lo24() const {
    return (value_ >> 16) & 0xFFU;
  }
  std::size_t lo32() const {
    return (value_ >> 24) & 0xFFU;
  }

 private:
  UInt32 value_;
};

template <>
class PopCount<64> {
 public:
  explicit PopCount(UInt64 x) : value_() {
    x = (x & 0x5555555555555555ULL) + ((x & 0xAAAAAAAAAAAAAAAAULL) >> 1);
    x = (x & 0x3333333333333333ULL) + ((x & 0xCCCCCCCCCCCCCCCCULL) >> 2);
    x = (x + (x >> 4)) & 0x0F0F0F0F0F0F0F0FULL;
    x += x << 8;
    x += x << 16;
    x += x << 32;
    value_ = x;
  }

  std::size_t lo8() const {
    return (std::size_t)(value_ & 0xFFU);
  }
  std::size_t lo16() const {
    return (std::size_t)((value_ >> 8) & 0xFFU);
  }
  std::size_t lo24() const {
    return (std::size_t)((value_ >> 16) & 0xFFU);
  }
  std::size_t lo32() const {
    return (std::size_t)((value_ >> 24) & 0xFFU);
  }
  std::size_t lo40() const {
    return (std::size_t)((value_ >> 32) & 0xFFU);
  }
  std::size_t lo48() const {
    return (std::size_t)((value_ >> 40) & 0xFFU);
  }
  std::size_t lo56() const {
    return (std::size_t)((value_ >> 48) & 0xFFU);
  }
  std::size_t lo64() const {
    return (std::size_t)((value_ >> 56) & 0xFFU);
  }

 private:
  UInt64 value_;
};

}  // namespace vector
}  // namespace grimoire
}  // namespace marisa

#endif  // MARISA_GRIMOIRE_VECTOR_POP_COUNT_H_
