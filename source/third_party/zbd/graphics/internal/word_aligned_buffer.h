#pragma once

//
// See copyright notice in zbd.h.
//

// Read/Write buffer that aligns the writes to the word boundary.
// It's important that the number of read calls, and the size
// passed to the read calls matches the write calls since padding
// is being done on a per-write basis.
// 
// For example, this will break horribly:
// char *foo = "rawr";
// write(foo, strlen(foo));
// read<char>();
// read<char>();
// ...
// 
// Instead you must do this:
// write(foo, strlen(foo));
// read(strlen(foo));
// 
// Or, since strings are a pain since their length is not known at
// compile time do this:
// writeString(foo);
// readString();
// 
// These are just helper functions that write/read the length of the string
// into the buffer first, then the string.
class WordAlignedBuffer {
public:
  WordAlignedBuffer(void)
  : readIndex(0) {
    // Do nothing.
  }

  ZB_INLINE void write(const void *data, i32 bytes);
  ZB_INLINE const void *read(i32 bytes);

  // Write/Read the size of the data into the buffer
  // as well as the data itself. Useful for arrays and anything else 
  // that has an unknown compile time size.
  ZB_INLINE void writeArray(const void *data, i32 bytes);
  ZB_INLINE const void *readArray(i32 *sizeOut);

  ZB_INLINE void reset(void); // Resets both reading and writing.
  ZB_INLINE bool reachedEnd(void);  // Indicates reading has reached the end.

  ZB_INLINE void freeMemory(void);

  //
  // Convenience
  // 
  template <typename T> ZB_INLINE void write(const T &pod) {
    write(&pod, sizeof(pod));
  }

  template <typename T> ZB_INLINE const T *read(void) {
    return (const T*)read(sizeof(T));
  }

  template <typename T> ZB_INLINE const T *readArray(i32 *sizeOut) {
    return (const T*)readArray(sizeOut);
  }

  ZB_INLINE void writeString(const char *str);
  ZB_INLINE const char *readString(void);

private:
  zbvector(u32) commands;
  i32 readIndex;
};
