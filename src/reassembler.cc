#include "reassembler.hh"
#include <algorithm>
#include <iostream>

using namespace std;

void Reassembler::insert( uint64_t first_index, string data, bool is_last_substring ) {
  if (is_last_substring) {
    eof_index_ = first_index + data.size();  
  }

  uint64_t available_capacity =  output_.writer().available_capacity();
  if (first_index > expected_index_) {
    if ((first_index - expected_index_) >= available_capacity)  {
      return;
    } else {
      uint64_t sub_data_size = min(data.size(), available_capacity - (first_index - expected_index_)); 
      data = data.substr(0, sub_data_size);
      for (uint64_t i = 0; i < sub_data_size; ++i) {
        uint64_t current_index = first_index + i;
        if (internal_storage_.find(current_index) == internal_storage_.end()) {
          internal_storage_.insert({current_index, data[i]});
          ++bytes_pending_;
        }
      }
    } 
  } else if (expected_index_ < first_index + data.size()) {
    for (uint64_t i = 0; i < data.size(); ++i) {
      uint64_t current_index = first_index + i;
      if ((expected_index_ == current_index) && available_capacity) {
        if (internal_storage_.find(expected_index_) == internal_storage_.end()) {
          output_.writer().push(string(1, data[i]));
          ++expected_index_;
          --available_capacity;
        } else {
          internal_storage_.erase(expected_index_);
          output_.writer().push(string(1, data[i]));
          ++expected_index_;
          --bytes_pending_;
          --available_capacity;
        }
      }
    }
  } 

  for (auto it = internal_storage_.begin(); it != internal_storage_.end(); ) {
    if (available_capacity && it->first == expected_index_) {
        output_.writer().push(string(1, it->second));
        it = internal_storage_.erase(it);
        ++expected_index_;
        --bytes_pending_;
        --available_capacity;
    } else {
        ++it;
    }
  }

  if (eof_index_ == expected_index_) {
    output_.writer().close();
  }
 }

uint64_t Reassembler::bytes_pending() const
{
  // Your code here.
  return bytes_pending_;
}
