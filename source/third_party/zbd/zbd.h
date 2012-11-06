#pragma once

//------------------------------------------------------------------------------
// Copyright (c) 2011 David Siems
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Macros and functions for assertion, stack traces, and logging.  A lot
// of this functionality can (and should) be disabled for shipping builds. See
// CONFIG for details.
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// ZB_COMPILER
// Defines compiler specific functionality to generic names.
// 
// ZB_CPP - Defined if the code is being compiled using a c++ compiler.
// ZB_FUNCTION_NAME - The name of the current function being compiled.
// ZB_FILE_NAME - The name of the current file being compiled.
// ZB_LINE_NUMBER - The current line number being compiled.
// ZB_RESTRICT - Specifies that a variable is not aliased.
// ZB_PREFETCH - Loads the specified address into the CPU cache.
// ZB_INLINE - Forces the compiler to inline. (To the extent that this is possible)
// ZB_ALIGNOF - Gets the alignment of the specified type. (Use like sizeof)
// ZB_USING_EXCEPTIONS - Defined if c++ exceptions are enabled.
// ZB_MULTI_LINE_MACRO_BEGIN/END - Safely wrap a multi-line macro.
// ZB_UNICODE - The code is being compiled using unicode.
//------------------------------------------------------------------------------
#if defined(_MSC_VER)
#define ZB_CONSTANT_WARNING_WORKAROUND ZB_LINE_NUMBER==-1,0
#else
#define ZB_CONSTANT_WARNING_WORKAROUND ZB_LINE_NUMBER==-1
#endif

#define ZB_MULTI_LINE_MACRO_BEGIN do {
#define ZB_MULTI_LINE_MACRO_END } while(ZB_CONSTANT_WARNING_WORKAROUND)

#if defined(__cplusplus)
#define ZB_CPP
#define ZB_C_SECTION_BEGIN extern "C" {
#define ZB_C_SECTION_END }
#else
#define ZB_C_SECTION_BEGIN
#define ZB_C_SECTION_END
#endif

#if defined(_MSC_VER)
#if _MSC_VER == 1600 && defined(ZB_DISABLE_ASSERT)  // VS2010
// I hate doing this but VS2010 broke suppressing these warnings using sizeof or &.
#pragma warning(disable:4100) // Unused formal parameter
#pragma warning(disable:4189) // Unused local variable
#endif

#define ZB_FUNCTION_NAME __FUNCTION__
#define ZB_FILE_NAME __FILE__
#define ZB_LINE_NUMBER __LINE__
#define ZB_RESTRICT __restrict
#define ZB_PREFETCH(address) _mm_prefetch(address, 3)
#define ZB_INLINE __forceinline
#define ZB_ALIGNOF __alignof
#define ZB_PP_COUNTER __COUNTER__

#ifdef _PREFAST_
#define ZB_ASSUME(condition) ZB_MULTI_LINE_MACRO_BEGIN \
  __assume(condition);  \
  __analysis_assume(!!(condition)); \
ZB_MULTI_LINE_MACRO_END
#else
#define ZB_ASSUME(condition) ZB_MULTI_LINE_MACRO_BEGIN \
  __assume(condition); \
ZB_MULTI_LINE_MACRO_END
#endif

#if defined(_CPPUNWIND)
#define ZB_USING_EXCEPTIONS
#endif

#if defined(UNICODE)
#define ZB_UNICODE
#endif

#else
#define ZB_FUNCTION_NAME __func__
#define ZB_FILE_NAME __FILE__
#define ZB_LINE_NUMBER __LINE__
#define ZB_RESTRICT __restrict__
#define ZB_PREFETCH(address) __builtin_prefetch(address)
#define ZB_INLINE inline
#define ZB_ALIGNOF __alignof__
#define ZB_PP_COUNTER __COUNTER__

#if defined(__EXCEPTIONS)
#define ZB_USING_EXCEPTIONS
#endif

#define __debugbreak() ZB_MULTI_LINE_MACRO_BEGIN \
  asm { int 3 } \
ZB_MULTI_LINE_MACRO_END

#define ZB_ASSUME(condition) ZB_MULTI_LINE_MACRO_BEGIN \
  if (!(condition)) { \
    __builtin_unreachable(); \
  } \
ZB_MULTI_LINE_MACRO_END
#endif

#if defined(ZB_USING_EXCEPTIONS)
#include <exception> // for std::bad_alloc
#endif

//------------------------------------------------------------------------------
// ZB_PLATFORM
// Determines what platform is compiling.
//------------------------------------------------------------------------------
#if defined(_WIN32) || defined(__CYGWIN32__)
#define ZB_PLATFORM_WINDOWS
#elif defined(__APPLE__)
#define ZB_PLATFORM_MAC
#elif defined(__linux__)
#define ZB_PLATFORM_LINUX
#else
#error Compilation for this platform is not supported.
#endif


//------------------------------------------------------------------------------
// ZB_ARCH -- TODO Processor specific #defines...
//------------------------------------------------------------------------------
#if !defined(ZB_CPP)
#if defined(_WIN64)
typedef unsigned __int64 size_t;
#else
typedef unsigned int size_t;
#endif
#endif


//------------------------------------------------------------------------------
// ZB_NAMESPACE
// Wraps all Zero Barrier code in the namespace specified by the 
// ZB_NAMESPACE_NAME define if it exists.
//------------------------------------------------------------------------------
#if defined(ZB_CPP) && defined(ZB_NAMESPACE_NAME)
#define ZB_USING_NAMESPACE using namespace ZB_NAMESPACE_NAME
#define ZB_NAMESPACE_BEGIN namespace ZB_NAMESPACE_NAME {
#define ZB_NAMESPACE_END }
#else
#define ZB_NAMESPACE_NAME 
#define ZB_USING_NAMESPACE 
#define ZB_NAMESPACE_BEGIN 
#define ZB_NAMESPACE_END 
#endif

#if defined(ZB_DISABLE_MEMORY)
#include <malloc.h>
#endif

#if defined(ZB_CPP)
// Used later, needs to be outside the ZB namespace.
namespace std {
  struct nothrow_t;
}
#endif

// Open the namespace for the remainder of the file.
ZB_NAMESPACE_BEGIN

//------------------------------------------------------------------------------
// ZB_TYPES
// Basic type definitions. If you wanted to change precision across the board 
// you'd do it here.
//------------------------------------------------------------------------------
typedef double f64;
typedef float f32;

typedef int i32;
typedef short i16;
typedef char i8;

typedef unsigned u32;
typedef unsigned short u16;
typedef unsigned char u8;

typedef u32 string_hash;
typedef u32 bytes;

//------------------------------------------------------------------------------
// ZB_HELPERS
// Miscellaneous helper functions and macros.
// 
// offset_of - Calculate the offset of a field in a structure or class.
// silence_unused - Silence the warning caused by an unused declared variable.
// zb_hash_combine - Combine two string hashes into a single hash.
// zb_hash_string - Compute the hash for a string.
//------------------------------------------------------------------------------
#define offset_of(type, field) (&((type*)(0x0))->field)
#define banish(var) ((void)sizeof(var))

// Workaround for VS2010 being stupid.
#if _MSC_VER == 1600
#define silence_unused(var) ZB_MULTI_LINE_MACRO_BEGIN \
  &var; \
  ZB_MULTI_LINE_MACRO_END
#else
#define silence_unused(var) ZB_MULTI_LINE_MACRO_BEGIN \
 banish(var); \
ZB_MULTI_LINE_MACRO_END
#endif

// String hashing
ZB_INLINE string_hash zb_hash_combine(string_hash a, string_hash b) {
  return a ^ b  + 0x9e3779b9 + (a << 6) + (a >> 2);
}

ZB_INLINE string_hash zb_hash_string(const char *s) {
  string_hash seed = 0;
  char c = *s++;
  
  while (c != '\0') {
    seed = zb_hash_combine(seed, (string_hash)c);
    c = *s++;
  }

  return seed;
}


//------------------------------------------------------------------------------
// ZB_MEMORY
// zmalloc - Allocate a block of memory.
// zmalloc_aligned - Allocate an aligned block of memory.
// zfree - Free a block of memory.
// znew - Allocate a block of memory calling constructors.
// znew_aligned - Allocate an aligned block of memory calling constructors.
// zdelete - Delete a block of memory calling destructors.
// zarray_delete - Delete an allocated array calling destructors.
//
// Whenever you do a thing, act as if all the world were watching. 
// -- Thomas Jefferson
//------------------------------------------------------------------------------
#if !defined(ZB_DISABLE_MEMORY)

ZB_C_SECTION_BEGIN
// Low level functions for interacting with the OS's heap.
// You should use these if you decide to override zb_malloc.
void *zb_heap_alloc(size_t size);
void *zb_heap_alloc_aligned(size_t size, u32 alignment);
void zb_heap_free(void *p);

// You can #define ZB_MEMORY_OVERRIDE_MALLOC to keep these functions from being defined
// so you can define them yourself.
// You shouldn't call these functions directly, use the zmalloc/zfree macros.
void *zb_malloc(size_t size, i32 line, const char *function, const char *file, u32 id, const char *name);
void *zb_malloc_aligned(size_t size, u32 alignment, i32 line, const char *function, const char *file, u32 id, const char *name);
void zb_free(void *p, i32 line, const char *function, const char *file, u32 id);

// This function cannot be overridden and should never be called by the user.
// It's used for interacting with delete[] in c++.
void zb_array_free(void *p, i32 line, const char *function, const char *file, u32 id);

// You can #define ZB_MEMORY_OVERRIDE_RECORD to keep these functions from being
// defined so you can define them yourself.
// You probably shouldn't call these functions directly unless you know what you're
// doing.
void zb_record_malloc(void *p, size_t size, i32 line, const char *function, const char *file, u32 id, const char *name);
void zb_record_free(void *p, i32 line, const char *function, const char *file, u32 id);
void zb_record_array_free(void *p, i32 line, const char *function, const char *file, u32 id);
ZB_C_SECTION_END

#if defined(ZB_MEMORY_RECORD)
#define zmalloc(size, ...) zb_malloc(size, ZB_LINE_NUMBER, ZB_FUNCTION_NAME, ZB_FILE_NAME, ZB_PP_COUNTER, ##__VA_ARGS__)
#define zmalloc_aligned(size, alignment, ...) zb_malloc_aligned(size, alignment, ZB_LINE_NUMBER, ZB_FUNCTION_NAME, ZB_FILE_NAME, ZB_PP_COUNTER, ##__VA_ARGS__)
#define zfree(p) zb_free(p, ZB_LINE_NUMBER, ZB_FUNCTION_NAME, ZB_FILE_NAME, ZB_PP_COUNTER)

#define zrecord_malloc(p, size, line, function, file, id, name) ZB_MULTI_LINE_MACRO_BEGIN \
  zb_record_malloc(p, size, line, function, file, id, name); \
ZB_MULTI_LINE_MACRO_END

#define zrecord_free(p, line, function, file, id) ZB_MULTI_LINE_MACRO_BEGIN \
  zb_record_free(p, line, function, file, id); \
ZB_MULTI_LINE_MACRO_END

#define zrecord_array_free(p, line, function, file, id) ZB_MULTI_LINE_MACRO_BEGIN \
  zb_record_array_free(p, line, function, file, id); \
ZB_MULTI_LINE_MACRO_END

#else
#define zmalloc(size, ...) zb_malloc(size, -1, 0x0, 0x0, 0, 0x0)
#define zmalloc_aligned(size, alignment, ...) zb_malloc_aligned(size, alignment, -1, 0x0, 0x0, 0, 0x0)
#define zfree(p) zb_free(p, -1, 0x0, 0x0, 0)

#define zrecord_malloc(p, size, line, function, file, id, name) ZB_MULTI_LINE_MACRO_BEGIN \
  silence_unused(p); \
  silence_unused(size); \
  silence_unused(line); \
  silence_unused(function); \
  silence_unused(file); \
  silence_unused(id); \
  silence_unused(name); \
ZB_MULTI_LINE_MACRO_END

#define zrecord_free(p, line, function, file, id) ZB_MULTI_LINE_MACRO_BEGIN \
  silence_unused(p); \
  silence_unused(line); \
  silence_unused(function); \
  silence_unused(file); \
  silence_unused(id); \
ZB_MULTI_LINE_MACRO_END

#define zrecord_array_free(p, line, function, file, id) ZB_MULTI_LINE_MACRO_BEGIN \
  silence_unused(p); \
  silence_unused(line); \
  silence_unused(function); \
  silence_unused(file); \
  silence_unused(id); \
ZB_MULTI_LINE_MACRO_END
#endif

// Cover new/delete as well if this is c++.
#if defined(ZB_CPP)
ZB_C_SECTION_BEGIN
extern const char *zb_anonymous_new_name;
extern const char *zb_anonymous_delete_name;
extern const char *zb_function_unknown_name;
extern const char *zb_file_unknown_name;
ZB_C_SECTION_END

#if defined(ZB_USING_EXCEPTIONS)
#define ZB_CHECK_THROW(x) ZB_MULTI_LINE_MACRO_BEGIN \
  void *p = (x); \
  if (p != 0x0) { \
    return p; \
  } \
  throw std::bad_alloc(); \
ZB_MULTI_LINE_MACRO_END
#else
#define ZB_CHECK_THROW(x) return (x);
#endif

// Custom new / delete
ZB_INLINE void *operator new(size_t size, i32 line, const char *function, const char *file, u32 id, const char *name = zb_anonymous_new_name) {
  ZB_CHECK_THROW(zb_malloc(size, line, function, file, id, name));
}

ZB_INLINE void *operator new(size_t size, u32 alignment, i32 line, const char *function, const char *file, u32 id, const char *name = zb_anonymous_new_name) {
  ZB_CHECK_THROW(zb_malloc_aligned(size, alignment, line, function, file, id, name));
}

// These functions should never actually be called, but could be if exceptions get turned on and
// a constructor throws.
ZB_INLINE void operator delete(void *p, i32 line, const char *function, const char *file, u32 id, const char *name) {
  if (p != 0x0) {
    silence_unused(name);
    zb_free(p, line, function, file, id);
  }
}

ZB_INLINE void operator delete(void *p, u32 alignment, i32 line, const char *function, const char *file, u32 id, const char *name) {
  if (p != 0x0) {
    silence_unused(alignment);
    silence_unused(name);
    zb_free(p, line, function, file, id);
  }
}

#if !defined(ZB_MEMORY_DISABLE_GLOBAL_NEW_DELETE)
// Global new / delete - Various compilers use different combinations of these.
ZB_INLINE void *operator new(size_t size, std::nothrow_t &) {
  return zb_malloc(size, -1, zb_function_unknown_name, zb_file_unknown_name, (u32)-1, zb_anonymous_new_name);
}

#if defined(_MSC_VER)
#include "sal.h"
_Ret_opt_bytecap_(_Size)
#endif
ZB_INLINE void *operator new(size_t _Size, const std::nothrow_t&) {
  return zb_malloc(_Size, -1, zb_function_unknown_name, zb_file_unknown_name, (u32)-1, zb_anonymous_new_name);
}

ZB_INLINE void *operator new(size_t size) {
  ZB_CHECK_THROW(zb_malloc(size, -1, zb_function_unknown_name, zb_file_unknown_name, (u32)-1, zb_anonymous_new_name));
}

ZB_INLINE void *operator new(size_t size, size_t alignment, std::nothrow_t &) {
  return zb_malloc_aligned(size, (u32)alignment, -1, zb_function_unknown_name, zb_file_unknown_name, (u32)-1, zb_anonymous_new_name);
}

ZB_INLINE void *operator new(size_t size, size_t alignment, const std::nothrow_t &) {
  return zb_malloc_aligned(size, (u32)alignment, -1, zb_function_unknown_name, zb_file_unknown_name, (u32)-1, zb_anonymous_new_name);
}

ZB_INLINE void *operator new(size_t size, size_t alignment) {
  ZB_CHECK_THROW(zb_malloc_aligned(size, (u32)alignment, -1, zb_function_unknown_name, zb_file_unknown_name, (u32)-1, zb_anonymous_new_name));
}

ZB_INLINE void operator delete(void *p, std::nothrow_t &) {
  if (p != 0x0) {
    zb_free(p, -1, zb_function_unknown_name, zb_file_unknown_name, (u32)-1);
  }
}

ZB_INLINE void operator delete(void *p, const std::nothrow_t &) {
  if (p != 0x0) {
    zb_free(p, -1, zb_function_unknown_name, zb_file_unknown_name, (u32)-1);
  }
}

ZB_INLINE void operator delete(void *p) throw() {
  if (p != 0x0) {
    zb_free(p, -1, zb_function_unknown_name, zb_file_unknown_name, (u32)-1);
  }
}

ZB_INLINE void operator delete(void *p, size_t alignment, std::nothrow_t &) {
  silence_unused(alignment);

  if (p != 0x0) {
    zb_free(p, -1, zb_function_unknown_name, zb_file_unknown_name, (u32)-1);
  }
}

ZB_INLINE void operator delete(void *p, size_t alignment, const std::nothrow_t &) {
  silence_unused(alignment);

  if (p != 0x0) {
    zb_free(p, -1, zb_function_unknown_name, zb_file_unknown_name, (u32)-1);
  }
}

ZB_INLINE void operator delete(void *p, size_t alignment) throw() {
  silence_unused(alignment);

  if (p != 0x0) {
    zb_free(p, -1, zb_function_unknown_name, zb_file_unknown_name, (u32)-1);
  }
}
#endif

// Template the delete operator that gets used so that it can call the appropriate destructor
// Technically this doesn't match with any of the above new operators but that doesn't matter.
template <typename T> ZB_INLINE void operator delete(void *p, T *type, i32 line, const char *function, const char *file, u32 id) {
  if (type != 0x0) {
    type->~T();
    zb_free(p, line, function, file, id);
  }
}

template <typename T> ZB_INLINE void operator delete[](void *p, T *type, i32 line, const char *function, const char *file, u32 id) {
  if (type != 0x0) {
    // This function does not actually free any memory, it's just for tracking purposes.
    zb_array_free(p, line, function, file, id);

    // Operator delete[] must be called because there is no standardized way to call
    // object destructors or know how many objects were allocated. So, we have to leave
    // the work up to the compiler.
    delete[] type;
  }
}

// All calls to znew should be paired with zdelete calls. It's not the end of the world if this
// doesn't happen but the memory tracking won't be as effective.
#define znew(...) new (ZB_LINE_NUMBER, ZB_FUNCTION_NAME, ZB_FILE_NAME, ZB_PP_COUNTER, ##__VA_ARGS__)
#define znew_aligned(alignment, ...) new(alignment, ZB_LINE_NUMBER, ZB_FUNCTION_NAME, ZB_FILE_NAME, ZB_PP_COUNTER, ##__VA_ARGS__)
#define zdelete(p) operator delete(p, p, ZB_LINE_NUMBER, ZB_FUNCTION_NAME, ZB_FILE_NAME, ZB_PP_COUNTER)
#define zarray_delete(p) operator delete[](p, p, ZB_LINE_NUMBER, ZB_FUNCTION_NAME, ZB_FILE_NAME, ZB_PP_COUNTER)
#endif
#else
#define zmalloc(size, ...) malloc(size)
#define zmalloc_aligned(size, alignment) // TODO: Fix this.
#define zfree(p) free(p)

#if defined(ZB_CPP)
#define znew(...) new
#define znew_aligned(alignment, ...) new  // TODO: Fix this.
#define zdelete(p) delete p
#define zarray_delete(p) delete[] p
#endif
#endif


//------------------------------------------------------------------------------
// ZB_TRACE
// ZB_INITIALIZE_STACK_TRACING - Initialize stack tracing. This must be called 
// at startup.
// 
// ZB_CAPTURE_STACK_TRACE - Capture a stack trace.
// ZB_SHUTDOWN_STACK_TRACING - Shutdown stack tracing. This must be called 
// at shutdown.
// 
// "Moria! Moria! Wonder of the Northern world! Too deep we delved there, and woke the nameless fear."
//------------------------------------------------------------------------------
#if !defined(ZB_DISABLE_STACK_TRACE)
#if !defined(ZB_TRACE_CONFIGURE)
#define ZB_TRACE_MAX 50
#define ZB_TRACE_NAME_MAX 75
#define ZB_TRACE_FILENAME_MAX 100
#endif
typedef struct zb_stack_trace {
  u32 frame_count;
  char names[ZB_TRACE_MAX][ZB_TRACE_NAME_MAX];
  char filenames[ZB_TRACE_MAX][ZB_TRACE_FILENAME_MAX];
  u32 lines[ZB_TRACE_MAX];
  u32 addresses[ZB_TRACE_MAX];
} zb_stack_trace;

ZB_C_SECTION_BEGIN
void zb_initialize_stack_trace(void);
void zb_capture_stack_trace(zb_stack_trace *trace, u32 maxDepth);
void zb_shutdown_stack_trace(void);
ZB_C_SECTION_END

#define ZB_TRACE_HELPER(traceName) \
  zb_stack_trace *traceName = (zb_stack_trace*)zmalloc(sizeof(zb_stack_trace), "ZB_StackTraceHelper"); \
  zb_capture_stack_trace(traceName, ZB_TRACE_MAX)

#define ZB_CAPTURE_STACK_TRACE(trace) ZB_MULTI_LINE_MACRO_BEGIN \
  zb_capture_stack_trace(&trace, ZB_TRACE_MAX); \
ZB_MULTI_LINE_MACRO_END

#define ZB_INITIALIZE_STACK_TRACING() ZB_MULTI_LINE_MACRO_BEGIN \
  zb_initialize_stack_trace(); \
ZB_MULTI_LINE_MACRO_END

#define ZB_SHUTDOWN_STACK_TRACING() ZB_MULTI_LINE_MACRO_BEGIN \
  zb_shutdown_stack_trace(); \
ZB_MULTI_LINE_MACRO_END
#else
typedef struct _zb_stack_trace {
  char dummy; // Structs must have a member in C.
} zb_stack_trace;

#define ZB_TRACE_HELPER(traceName) zb_stack_trace *traceName = 0x0
#define ZB_CAPTURE_STACK_TRACE(trace) silence_unused(trace)

#define ZB_INITIALIZE_STACK_TRACING() ZB_MULTI_LINE_MACRO_BEGIN \
ZB_MULTI_LINE_MACRO_END

#define ZB_SHUTDOWN_STACK_TRACING() ZB_MULTI_LINE_MACRO_BEGIN \
ZB_MULTI_LINE_MACRO_END
#endif


//------------------------------------------------------------------------------
// ZB_ASSERT
// 
// ZB_SET_ASSERTION_HANDLER - Set your own assertion handler.
// defend - Basic assertion macro.
// defend_msg - Basic assertion macro, lets you specify a reason for failure.
// static_defend - Compile time assertion.
// crash - Crashes the application with a message.
// not_implemented - Crashes the application with a 'not implemented' message.
//
// Prepare thyself!
//------------------------------------------------------------------------------
typedef i32 (*zb_assertion_handler)(const char *condition, const char *message, const char *function, const char *file, i32 line, zb_stack_trace *trace);

#if !defined(ZB_DISABLE_ASSERT)  
ZB_C_SECTION_BEGIN
zb_assertion_handler zb_get_assertion_handler(void);
zb_assertion_handler zb_set_assertion_handler(zb_assertion_handler);

i32 zb_assertion_failure(const char *condition, const char *function, const char *file, i32 line, const char *message, zb_stack_trace *trace);
i32 zb_assertion_failure_formatted(const char *condition, const char *function, const char *file, i32 line, const char *message, zb_stack_trace *trace, ...);
i32 zb_null_assertion_handler(const char*, const char*, const char*, const char*, i32, zb_stack_trace*);
i32 zb_stderr_assertion_handler(const char *condition, const char *message, const char *function, const char *file, i32 line, zb_stack_trace *trace);
i32 zb_ods_assertion_handler(const char *condition, const char *message, const char *function, const char *file, i32 line, zb_stack_trace *trace);

ZB_C_SECTION_END

#define ZB_SET_ASSERTION_HANDLER(handler) zb_set_assertion_handler(handler)
#define ZB_GET_ASSERTION_HANDLER() zb_get_assertion_handler()

#define defend(condition) ZB_MULTI_LINE_MACRO_BEGIN \
  if (!(condition)) { \
    ZB_TRACE_HELPER(trace); \
    if (zb_assertion_failure(#condition, ZB_FUNCTION_NAME, ZB_FILE_NAME, ZB_LINE_NUMBER, 0x0, trace)) { \
      __debugbreak(); \
    } \
  } \
  ZB_ASSUME(condition); \
ZB_MULTI_LINE_MACRO_END

#define defend_msg(condition, message, ...) ZB_MULTI_LINE_MACRO_BEGIN \
  if (!(condition)) { \
    ZB_TRACE_HELPER(trace); \
    if (zb_assertion_failure_formatted(#condition, ZB_FUNCTION_NAME, ZB_FILE_NAME, ZB_LINE_NUMBER, message, trace, ##__VA_ARGS__)) { \
      __debugbreak(); \
    } \
  } \
  ZB_ASSUME(condition); \
ZB_MULTI_LINE_MACRO_END

#define crash(message, ...) ZB_MULTI_LINE_MACRO_BEGIN \
  ZB_TRACE_HELPER(trace); \
  zb_assertion_failure_formatted(0x0, ZB_FUNCTION_NAME, ZB_FILE_NAME, ZB_LINE_NUMBER, message, trace, ##__VA_ARGS__); \
  __debugbreak(); \
ZB_MULTI_LINE_MACRO_END

#define not_implemented() crash("%s:%u has not been implemented.", ZB_FUNCTION_NAME, ZB_LINE_NUMBER)
#else
// #define to null because the original functions return pointers.
#define ZB_SET_ASSERTION_HANDLER(handler) 0x0
#define ZB_GET_ASSERTION_HANDLER() 0x0

#define defend(condition) ZB_MULTI_LINE_MACRO_BEGIN \
  ZB_ASSUME(condition); \
  banish(condition); \
ZB_MULTI_LINE_MACRO_END

#define defend_msg(condition, message, ...) ZB_MULTI_LINE_MACRO_BEGIN \
  ZB_ASSUME(condition); \
  banish(condition); \
  banish(message); \
ZB_MULTI_LINE_MACRO_END

#define crash(message, ...) ZB_MULTI_LINE_MACRO_BEGIN \
  banish(message); \
ZB_MULTI_LINE_MACRO_END

#define not_implemented() crash("")
#endif

#define static_defend(condition, message) \
  extern char static_defend_##message[1]; \
  extern char static_defend_##message[(condition) ? 1 : 2]

#define sentinel() ZB_MULTI_LINE_MACRO_BEGIN \
  __debugbreak(); \
ZB_MULTI_LINE_MACRO_END


//------------------------------------------------------------------------------
// ZB_LOG
// These logging macros only cover the logging frontend. You should specify
// your own log handler to do the actual filtering etc. Channel 0 corresponds to
// 'always print'.
//
// ZB_SET_LOG_HANDLER - Set the log handler. 
// zb_log_piece - Log a message filtered by channel specifying a newline or not.
// zb_log - Log a message filtered by channel with a newline.
// zb_log_unfiltered - Log a message without filtering with a newline.
//------------------------------------------------------------------------------
typedef void (*zb_log_handler)(const char *message, const char *function, const char *file, i32 line, i32 channel, i32 newline);

#if !defined(ZB_DISABLE_LOG)
ZB_C_SECTION_BEGIN
zb_log_handler zb_get_log_handler(void);
zb_log_handler zb_set_log_handler(zb_log_handler);

void zb_log_message(const char *message, const char *function, const char *file, i32 line, i32 channel, i32 newline, ...);
void zb_null_log_handler(const char *message, const char *function, const char *file, i32 line, i32 channel, i32 newline);
void zb_debug_log_handler(const char *message, const char *function, const char *file, i32 line, i32 channel, i32 newline);
ZB_C_SECTION_END

#define ZB_SET_LOG_HANDLER(handler) zb_set_log_handler(handler)
#define ZB_GET_LOG_HANDLER() zb_get_log_handler()

#define zb_log_piece(channel, message, newline, ...) ZB_MULTI_LINE_MACRO_BEGIN \
  zb_log_message(message, ZB_FUNCTION_NAME, ZB_FILE_NAME, ZB_LINE_NUMBER, channel, newline, ##__VA_ARGS__); \
ZB_MULTI_LINE_MACRO_END
#else
// #define to 0x0 because the original functions return pointers.
#define ZB_SET_LOG_HANDLER(handler) 0x0
#define ZB_GET_LOG_HANDLER() 0x0
#define zb_log_piece(channel, message, newline, ...) ZB_MULTI_LINE_MACRO_BEGIN \
  silence_unused(channel); \
  silence_unused(message); \
ZB_MULTI_LINE_MACRO_END
#endif

#define zb_log(channel, message, ...) zb_log_piece(channel, message, 1, ##__VA_ARGS__)
#define zb_spam(message, ...) zb_log(0, message, ##__VA_ARGS__)

ZB_NAMESPACE_END
