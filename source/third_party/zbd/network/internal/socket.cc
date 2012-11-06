//
// See copyright notice in zbd.h.
//

#if defined(ZB_PLATFORM_WINDOWS)
#include <winsock.h>
#else
#error Missing header include for this platform
#endif

ZB_NAMESPACE_BEGIN

Address::Address(void)
: address(0)
, port(0) {
  // Do nothing.
}

Address::Address(u8 a, u8 b, u8 c, u8 d, u16 port) {
  address = (a << 24) | (b << 16) | (c << 8) | d;
  this->port = port;
}

Address::Address(u32 address, u16 port) {
  this->address = address;
  this->port = port;
}

u32 Address::GetAddress(void) const {
  return address;
}

u8 Address::GetA(void) const {
  return static_cast<u8>(address >> 24);
}

u8 Address::GetB(void) const {
  return static_cast<u8>(address >> 16);
}

u8 Address::GetC(void) const {
  return static_cast<u8>(address >> 8);
}

u8 Address::GetD(void) const {
  return static_cast<u8>(address);
}

u16 Address::GetPort(void) const {
  return port;
}

bool Address::operator==(const Address &rhs) const {
  return address == rhs.address && port == rhs.port;
}

bool Address::operator!=(const Address &rhs) const {
  return !(*this == rhs);
}

bool Address::operator<(const Address &rhs) const {
  return address < rhs.address && port < rhs.port;
}

Address::operator u32(void) const {
  return address;
}


Socket::Socket(void)
: s(0) {
  // Do nothing.
}

bool Socket::Open(u16 port) {
  s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (s <= 0) {
    return false;
  }

  if (port != 0) {
    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(s, reinterpret_cast<const sockaddr*>(&address), sizeof(sockaddr_in)) < 0) {
      Close();
      zb_udp_log("Failed to bind port %u.", port);
      return false;
    }

    zb_udp_log("Bound socket to port %u.", port);
  }
  
#if defined(ZB_PLATFORM_MAC) || defined(ZB_PLATFORM_LINUX)
  i32 nonBlocking = 1;
  if (fcntl(s, F_SETFL, O_NONBLOCK, nonBlocking) == -1) {
#elif defined (ZB_PLATFORM_WINDOWS)
  DWORD nonBlocking = 1;
  if (ioctlsocket(s, FIONBIO, &nonBlocking) != 0) {
#else
#error Compilation for this platform is not supported.
#endif
    zb_udp_log("Failed to set socket to non blocking mode.");
    Close();
    return false;
  }

  return true;
}

void Socket::Close(void) {
  if (socket != 0) {
#if defined(ZB_PLATFORM_MAC) || defined(ZB_PLATFORM_LINUX)
    close(s);
#elif defined(ZB_PLATFORM_WINDOWS)
    closesocket(s);
#else
#error Compilation for this platform is not supported.
#endif
    s = 0;
  }
}

bool Socket::IsOpen(void) const {
  return s != 0;
}

bool Socket::Send(const Address &destination, const void *data, bytes dataSize) {
  defend (data != 0x0);
  defend (dataSize > 0);

  if (s == 0) {
    return false;
  }

  sockaddr_in address;
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = htonl(destination.GetAddress());
  address.sin_port = htons(destination.GetPort());

  bytes sentCount = sendto(s, reinterpret_cast<const char*>(data), dataSize, 0, reinterpret_cast<sockaddr*>(&address), sizeof(sockaddr_in));

  return sentCount == dataSize;
}

i32 Socket::Receive(Address *senderOut, void *dataOut, bytes dataOutSize){
  defend (dataOut != 0x0);
  defend (dataOutSize > 0);

  if (s == 0) {
    return 0;
  }

#if defined(ZB_PLATFORM_WINDOWS)
  typedef i32 socklen_t;
#endif

  sockaddr_in from = {0};
  socklen_t fromLength = sizeof(from);

  i32 receivedCount = recvfrom(s, reinterpret_cast<char*>(dataOut), dataOutSize, 0, reinterpret_cast<sockaddr*>(&from), &fromLength);

  if (receivedCount <= 0) {
    return 0;
  }

  const u32 address = ntohl(from.sin_addr.s_addr);
  const u16 port = ntohs(from.sin_port);

  *senderOut = Address(address, port);

  return receivedCount;
}

ZB_NAMESPACE_END
