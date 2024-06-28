#include "byte_stream.hh"
#include <chrono>
#include <iostream>
#include <thread>

using namespace std;

ByteStream::ByteStream( uint64_t capacity ) : capacity_( capacity ), buffer_( capacity, '\0' ) {}

bool Writer::is_closed() const
{
  // Your code here.
  return closed_;
}

void Writer::push( string data )
{
  // Your code here.
  for ( char byte : data ) {

    if ( size_ == capacity_ ) { // buffer is full
      return;
    }

    buffer_[write_pos_] = byte;
    write_pos_ = ( write_pos_ + 1 ) % capacity_;
    ++size_;
    ++bytes_pushed_;
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
  return capacity_ - size_;
}

uint64_t Writer::bytes_pushed() const
{
  // Your code here.
  return bytes_pushed_;
}

bool Reader::is_finished() const
{
  // Your code here.
  return closed_ && ( bytes_pushed_ == bytes_poped_ );
}

uint64_t Reader::bytes_popped() const
{
  // Your code here.
  return bytes_poped_;
}

string_view Reader::peek() const
{
  // Your code here.
  if ( size_ == 0 ) { // buffer is empty
    return string_view();
  } else {
    return string_view( &buffer_[read_pos_], 1 );
  }
}

void Reader::pop( uint64_t len )
{
  // Your code here.
  for ( uint64_t i = 0; i < len; ++i ) {
    if ( size_ == 0 ) { // buffer is empty.
      return;
    }
    --size_;
    ++bytes_poped_;
    read_pos_ = ( read_pos_ + 1 ) % capacity_;
  }
}

uint64_t Reader::bytes_buffered() const
{
  // Your code here.
  return size_;
}
