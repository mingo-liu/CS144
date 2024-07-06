#include "tcp_receiver.hh"
#include <algorithm>

using namespace std;

void TCPReceiver::receive(TCPSenderMessage message) {
    if (message.RST) {
      reassembler_.reader().set_error();
      return;
    }  

    if (message.SYN) {
      isn_ = message.seqno;
      started_ = true;
    }

    // 如果没有收到SYN包，那么后面到来的报文段都要被丢弃
    if (!started_) {  
      return;    
    }

    uint64_t abs_seqno = message.seqno.unwrap(isn_, 
                                              reassembler_.expected_index());
    // 携带数据的报文段与SYN报文段的序号一样
    if (!abs_seqno && !message.SYN) {     
      return;
    }
    // abs_seqno == 0 对应的一定是SYN报文段(有可能携带数据)
    uint64_t stream_index = abs_seqno ? abs_seqno - 1 : 0;    
    reassembler_.insert(stream_index, message.payload, message.FIN);
}

TCPReceiverMessage TCPReceiver::send() const
{
  // Your code here.
  std::optional<Wrap32> ackno = Wrap32::wrap(
      // + 1？stream_index -> absolute sequence number 
      reassembler_.expected_index() + 1 + reassembler_.writer().is_closed(), 
      isn_); 

  uint16_t window_size = static_cast<uint16_t>(
      std::min(reassembler_.writer().available_capacity(), 
               static_cast<uint64_t>(UINT16_MAX)));
  return TCPReceiverMessage {started_ ? ackno : std::nullopt, window_size, 
                             reassembler_.writer().has_error()}; 
}
