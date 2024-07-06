#include "reassembler.hh"
#include <algorithm>
#include <iostream>

using namespace std;

void Reassembler::insert( uint64_t first_index, string data, bool is_last_substring ) {
  if (data.empty()) {
    if (is_last_substring) {
      output_.writer().close();
    }
    return;
  }
  
  auto last_index = first_index + data.size();                  // data: [first_index, last_index)
  auto end_index = expected_index_ + output_.writer().available_capacity(); // 可用范围: [expected_index_, end_index)
  if ( last_index < expected_index_ || first_index >= end_index ) { 
    return; 
  }

  // 调整data的大小，丢掉last_index之后的字节
  if ( end_index < last_index ) {
    last_index = end_index;
    data.resize(last_index - first_index);
    is_last_substring = false;
  }

  // 丢掉expected_index_之前的字节
  if ( first_index < expected_index_ ) {
    data = data.substr( expected_index_ - first_index );  // 从expected_index - first_index处开始截取子串
    first_index = expected_index_;
  }
  
  if (first_index == expected_index_ && buffer_.empty()) {
    insert_stream_from_expected_data(data);
  } else if (first_index == expected_index_ && 
             last_index <= buffer_.front().last_index + 1) {
    data.resize(min(last_index, buffer_.front().first_index) - first_index); // 计算调整data的大小, 丢掉data后边与buffer_中的第一个子串的重叠部分
    insert_stream_from_expected_data(data);
  } else {      // 1.first_index != expected_index_ 
                // 2.first_index == expected_index_ and 当前data跨越buffer_中的1个以上的区间 
    insert_buffer( first_index, last_index - 1, data );
  }

  last_substring_ |= is_last_substring;
  
  // 将buffer中的字符串写入stream
  insert_stream_from_buffer();
}

void Reassembler::insert_buffer(uint64_t first_index, uint64_t last_index,
                                std::string data) {
  auto l = first_index, r = last_index;
  auto buffer_begin = buffer_.begin(), buffer_end = buffer_.end();
  auto left = lower_bound(buffer_begin, buffer_end, l, [] (const Block &a, uint64_t b) { return a.last_index < b;});
  auto right = upper_bound(buffer_begin, buffer_end, r, [] (uint64_t b, const Block &a) { return a.first_index > b;});
  if (left != buffer_end) {
    l = min(l, left->first_index);
  }
  if (right != buffer_begin) {
    r = max(r, prev(right)->last_index);
  } 
  // data 已经在buffer_中
  if (left != buffer_end && left->first_index == l && left->last_index == r) {
    return; 
  }

  uint64_t str_size = r - l + 1;
  buffer_size_ += str_size;
  // data与buffer_无重叠
  if (data.size() == str_size && left == right) { 
    buffer_.emplace(left, Block{l, r, data});
    return;
  }

  //data与buffer_有重叠
  string str(str_size, 0);
  // 合并重叠的字符串
  for (auto it = left; it != right; ++it) {
    buffer_size_ -= it->data.size();
    std::copy(it->data.begin(), it->data.end(), str.begin() + it->first_index - l);
  }
  // 将新数据data拷贝并合并到字符串中
  std::copy(data.begin(), data.end(), str.begin() + first_index - l);
  buffer_.emplace(buffer_.erase(left, right), Block{l, r, str});
}

// 将buffer_中的数据插入到stream中
void Reassembler::insert_stream_from_buffer() {
  while (!buffer_.empty() && expected_index_ == buffer_.front().first_index) {
    Block &front = buffer_.front();
    buffer_size_ -= front.data.size();
    insert_stream_from_expected_data(front.data);
    buffer_.pop_front();
  }

  if (last_substring_ && buffer_.empty()) {   // 最后一个插入的子串被写入到stream中, 关闭stream
    output_.writer().close();
  }
}
void Reassembler::insert_stream_from_expected_data(const std::string &data) {
    expected_index_ += data.size();
    output_.writer().push(data);
}

uint64_t Reassembler::bytes_pending() const
{
  // Your code here.
  return buffer_size_;
}