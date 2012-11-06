
namespace zb_thread {
  ThreadHandle start(ThreadEntryFunction entry, void *userdata) {
    return reinterpret_cast<ThreadHandle>(_beginthreadex(0, 0, entry, userdata, 0, 0));
  }

  void exit(u32 returnValue) {
    _endthreadex(returnValue);
  }

  void stop(ThreadHandle th, seconds countdown) {
    defend (th != InvalidHandle);
    WaitForSingleObjectEx(th, static_cast<DWORD>(countdown == ThreadInfiniteWait ? ThreadInfiniteWait : countdown * 1000), FALSE);
    CloseHandle(th);
  }
}
