#include "wrapping_integers.hh"
#include <algorithm>

using namespace std;

Wrap32 Wrap32::wrap( uint64_t n, Wrap32 zero_point )
{
  // Your code here.
  return  Wrap32 {static_cast<uint32_t>((zero_point.raw_value_ + n) %  (1ULL << 32))};
}

uint64_t Wrap32::unwrap(Wrap32 zero_point, uint64_t checkpoint) const
{
    // 计算相对差异
    uint64_t differ = static_cast<uint64_t>(raw_value_ < zero_point.raw_value_ ?
       (1ULL << 32) + raw_value_ - zero_point.raw_value_ : raw_value_ - zero_point.raw_value_);

    // 计算基础绝对序号
    uint64_t base = (checkpoint / (1ULL << 32)) * (1ULL << 32);

    // 计算三个候选值
    uint64_t candidate1 = base + differ;
    uint64_t candidate2 = candidate1 >= (1ULL << 32) ? candidate1 - (1ULL << 32) : candidate1;
    uint64_t candidate3 = candidate1 + (1ULL << 32);

    // 选择最接近checkpoint的候选值
    uint64_t closest = candidate1;
    if (std::abs(static_cast<int64_t>(checkpoint - candidate2)) < std::abs(static_cast<int64_t>(checkpoint - closest))) {
        closest = candidate2;
    }
    if (std::abs(static_cast<int64_t>(checkpoint - candidate3)) < std::abs(static_cast<int64_t>(checkpoint - closest))) {
        closest = candidate3;
    }

    return closest;
}