#pragma once

#define THREAD_ENTRY_DECORATION __stdcall

namespace zb_thread {
  typedef HANDLE ThreadHandle;
  const ThreadHandle InvalidHandle = INVALID_HANDLE_VALUE;
  const u32 ThreadInfiniteWait = INFINITE;
}
