#pragma once

#include "byte_stream.hh"
#include <list>
#include <vector>



class Reassembler
{
public:
  // Construct Reassembler to write into given ByteStream.
  explicit Reassembler( ByteStream&& output ) : output_( std::move( output ) ){}

  /*
   * Insert a new substring to be reassembled into a ByteStream.
   *   `first_index`: the index of the first byte of the substring
   *   `data`: the substring itself
   *   `is_last_substring`: this substring represents the end of the stream
   *   `output`: a mutable reference to the Writer
   *
   * The Reassembler's job is to reassemble the indexed substrings (possibly out-of-order
   * and possibly overlapping) back into the original ByteStream. As soon as the Reassembler
   * learns the next byte in the stream, it should write it to the output.
   *
   * If the Reassembler learns about bytes that fit within the stream's available capacity
   * but can't yet be written (because earlier bytes remain unknown), it should store them
   * internally until the gaps are filled in.
   *
   * The Reassembler should discard any bytes that lie beyond the stream's available capacity
   * (i.e., bytes that couldn't be written even if earlier gaps get filled in).
   *
   * The Reassembler should close the stream after writing the last byte.
   */
  void insert( uint64_t first_index, std::string data, bool is_last_substring );

  // How many bytes are stored in the Reassembler itself?
  uint64_t bytes_pending() const;

  // Access output stream reader
  Reader& reader() { return output_.reader(); }
  const Reader& reader() const { return output_.reader(); }

  // Access output stream writer, but const-only (can't write from outside)
  const Writer& writer() const { return output_.writer(); }
  // Writer& writer() { return output_.writer(); }
  
  // return the expected index, i.e. the first unassembled index
  const uint64_t & expected_index() const { return expected_index_; };

private:
  void insert_buffer(uint64_t first_index, uint64_t last_index, std::string data);
  void insert_stream_from_buffer();
  void insert_stream_from_expected_data(const std::string &data);

private:
  struct Block {
    uint64_t first_index;   // buffer_ 中区间的左端点
    uint64_t last_index;    // 区间右端点
    std::string data;       // 区间内的字节
  };

  uint64_t expected_index_ {};  // ByteStream中期望的下一个字节 
  uint64_t buffer_size_ {};     //  buffer_的大小，和ByteStream公用一个capacity
  bool last_substring_ {};      // 是否是最后一个子串？
  ByteStream output_; // the Reassembler writes to this ByteStream
  std::list<Block> buffer_ {};
};
