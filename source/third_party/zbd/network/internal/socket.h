#pragma once

//
// See copyright notice in zbd.h.
//

ZB_NAMESPACE_BEGIN

class Address {
public:
  Address(void);
  
  Address(u8 a, u8 b, u8 c, u8 d, u16 port);
  Address(u32 address, u16 port);
  u32 GetAddress(void) const;
  u8 GetA(void) const;
  u8 GetB(void) const;
  u8 GetC(void) const;
  u8 GetD(void) const;
  u16 GetPort(void) const;
  bool operator==(const Address &rhs) const;
  bool operator!=(const Address &rhs) const;
  bool operator<(const Address &rhs) const;

  operator u32(void) const;

private:
  u32 address;
  u16 port;
};

class Socket {
public:
  Socket(void);
  
  bool Open(u16 port);
  void Close(void);
  
  bool IsOpen(void) const;
  
  bool Send(const Address &destination, const void *data, bytes dataSize);
  i32 Receive(Address *senderOut, void *dataOut, bytes dataOutSize);

private:
  i32 s;
};

ZB_NAMESPACE_END
