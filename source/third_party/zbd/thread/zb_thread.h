#pragma once

#include "zbd.h"

#if defined(ZB_PLATFORM_WINDOWS)
#include <windows.h>
#include <process.h>
#include "internal/threads_win32.h"
#endif

namespace zb_thread {
  typedef  u32 (THREAD_ENTRY_DECORATION *ThreadEntryFunction)(void*);
  ThreadHandle start(ThreadEntryFunction entry, void *userdata);
  void exit(u32 returnValue);
  void stop(ThreadHandle th, u32 countdown);
}

#include "internal/thread_context.h"
