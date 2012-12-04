//
// See copyright notice in zbd.h.
//

ZB_INLINE void WordAlignedBuffer::write(const void *data, i32 bytes) {
  const u32 writeIndex = commands.size();
  const u32 expansion = (bytes + sizeof(u32) - 1) / sizeof(u32); // Round up the number of u32's to allocate.
  commands.resize(writeIndex + expansion);
  memcpy(&commands[writeIndex], data, bytes);
}

ZB_INLINE const void *WordAlignedBuffer::read(i32 bytes) {
  defend ((u32)readIndex < commands.size());
  const u32 readCount = (bytes + sizeof(u32) - 1) / sizeof(u32);
  void *data = &commands[readIndex];
  readIndex += readCount;
  return data;
}

ZB_INLINE void WordAlignedBuffer::writeArray(const void *data, i32 bytes) {
  write(bytes);
  write(data, bytes);
}

ZB_INLINE const void *WordAlignedBuffer::readArray(i32 *sizeOut) {
  const i32 *bytes = read<i32>();
  *sizeOut = *bytes;
  return read(*bytes);
}

ZB_INLINE void WordAlignedBuffer::writeString(const char *str) {
  writeArray(str, strlen(str) + 1);
}

ZB_INLINE const char *WordAlignedBuffer::readString(void) {
  i32 dummy = 0;
  return readArray<char>(&dummy); 
}

ZB_INLINE void WordAlignedBuffer::reset(void) {
  readIndex = 0;
  commands.clear();
}

ZB_INLINE bool WordAlignedBuffer::reachedEnd(void) {
   return (u32)readIndex == commands.size();
}
