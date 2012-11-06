#pragma once

/*--------------------------------------------------------------------------------------------------
Designed to turn any class that inherits from it into a separate thread.  To use, inherit from this 
class then create the virtual main function.

Notes and Limitations:
	- ThreadContext::main() should be called within the overloaded main to ensure
	proper shutdown behavior.
	- stop should not be called from inside the main function.
	- The thread terminates itself when main returns.
--------------------------------------------------------------------------------------------------*/
class ThreadContext {
public:

	ThreadContext(void);
	virtual ~ThreadContext(void);

  // Main thread
  bool start(void);
  void request_stop(void);
  void stop(seconds countdown = zb_thread::ThreadInfiniteWait);
  bool running(void);

  // Second thread
	virtual u32 main(void) = 0;

private:

	static u32 THREAD_ENTRY_DECORATION startup_callback(void *caller);
  zb_thread::ThreadHandle threadId;
	bool stopThread;
};
