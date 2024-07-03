#include "byte_stream.hh"
#include <chrono>
#include <iostream>
#include <thread>

using namespace std;

ByteStream::ByteStream( uint64_t capacity ) : capacity_( capacity ) {}

bool Writer::is_closed() const
{
  // Your code here.
  return closed_;
}

void Writer::push( string data )
{
  // Your code here.
  for (char byte : data) {
    if (buffer_.size() == capacity_) {
      return;
    }
    buffer_.push_back(byte);
    ++bytes_pushed_size_;
  }
}

void Writer::close()
{
  // Your code here.
  closed_ = true;
}

uint64_t Writer::available_capacity() const
{
  // Your code here.
  return capacity_ - buffer_.size();
}

uint64_t Writer::bytes_pushed() const
{
  // Your code here.
  return bytes_pushed_size_;
}

bool Reader::is_finished() const
{
  // Your code here.
  return closed_ && (buffer_.empty());
}

uint64_t Reader::bytes_popped() const
{
  // Your code here.
  return bytes_poped_size_;
}

string_view Reader::peek() const
{
  // Your code here.
  if (buffer_.empty()) {
    return string_view();
  } 
  return string_view(&buffer_.front(), 1);  
}

void Reader::pop( uint64_t len )
{
  // Your code here.
  for ( uint64_t i = 0; i < len; ++i ) {
    if (buffer_.empty()) {
      return;
    }
    ++bytes_poped_size_;
    buffer_.pop_front();
  }
}

uint64_t Reader::bytes_buffered() const
{
  // Your code here.
  return buffer_.size();
}
