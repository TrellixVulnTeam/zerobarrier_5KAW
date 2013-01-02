//
// See Copyright notice in zbd.h.
//

#pragma once

#include <algorithm>
#include <string>
#include <vector>
#include <list>
#include <iostream>
#include <cstdarg>
#include <algorithm>
#include <hash_map>
#include <ctime>
#include <limits>
#include <map>
#include <set>
#include <cstdlib>

#ifdef ZB_PLATFORM_WINDOWS
#include <windows.h>
#endif

ZB_NAMESPACE_BEGIN

#define zbvector(...) std::vector<__VA_ARGS__>
#define zblist(...) std::list<__VA_ARGS__>
#define zbmap(...) std::map<__VA_ARGS__>
#define zbset(...) std::set<__VA_ARGS__>
#define zbhashmap(...) stdext::hash_map<__VA_ARGS__>
#define zbmin(...) std::min(__VA_ARGS__)
#define zbmax(...) std::max(__VA_ARGS__)
#define zbswap(...) std::swap(__VA_ARGS__)
#define zbstrcmp(...) std::strcmp(__VA_ARGS__)

typedef std::string zbstring;
typedef std::wstring zbwstring;


// Casting
// Casts from U to T making sure nothing is truncated.
template <typename T, typename U> ZB_INLINE T NoTruncate(const U n) {
  defend (n >= static_cast<U>(std::numeric_limits<T>::min()));
  defend (n <= static_cast<U>(std::numeric_limits<T>::max()));
  return static_cast<T>(n);
}

// STL Helpers
template <typename T, typename U, typename V> ZB_INLINE void MapInsertUnique(T &map, const U &key, const V &value) {
  defend (map.find(key) == map.end());
  map[key] = value;
}

template <typename T, typename U> ZB_INLINE typename T::mapped_type MapFindGuaranteed(T &map, const U &key) {
  T::iterator i = map.find(key);
  defend (i != map.end());
  return i->second;
}

// TODO: These are win32 specific. Move them.
ZB_INLINE zbstring zb_narrow(const wchar_t *s) {
  const i32 requiredCharacters = WideCharToMultiByte(CP_UTF8, 0, s, -1, 0x0, 0, 0x0, 0x0);
  char *buffer = znew("zb_narrow_buffer") char[requiredCharacters];
  WideCharToMultiByte(CP_UTF8, 0, s, -1, buffer, requiredCharacters, 0x0, 0x0);
  zbstring string(buffer);
  zarray_delete(buffer);

  return string;
}

ZB_INLINE zbwstring zb_widen(const char *s) {
  const i32 requiredCharacters = MultiByteToWideChar(CP_UTF8, 0, s, -1, 0x0, 0);
  wchar_t *buffer = znew("zb_widen_buffer") wchar_t[requiredCharacters];
  MultiByteToWideChar(CP_UTF8, 0, s, -1, buffer, requiredCharacters);
  zbwstring wideString(buffer);
  zarray_delete(buffer);

  return wideString;
}

ZB_INLINE zbstring zb_narrow(const zbwstring &s) {
  return zb_narrow(s.c_str());
}

ZB_INLINE zbwstring zb_widen(const zbstring &s) {
  return zb_widen(s.c_str());
}

ZB_NAMESPACE_END
