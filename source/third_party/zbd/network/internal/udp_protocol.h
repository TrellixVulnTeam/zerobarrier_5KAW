#pragma once

//
// See copyright notice in zbd.h.
//

#pragma pack(push, 1)
struct UdpMessageHeader {
  u32 protocol;
  u16 id;
  u16 ack_base;
  u32 acks;
};

struct UdpReliableHeader {
  u16 id;
  u8 size;
};
#pragma pack(pop)

struct UdpConnection {
  Address address;
  seconds last_packet_time;

};

class UdpProtocol {
public:
  bool Initialize(u32 protocol, u16 port);  // Pass 0 to auto-assign the port. (Usually used for clients)
  void Shutdown(void);

  void Connect(const Address &address);
  void Disconnect(const Address &address);

  
};
