
ThreadContext::ThreadContext(void) 
: threadId(zb_thread::InvalidHandle),
  stopThread(false) {
}

ThreadContext::~ThreadContext(void) {
  stop(0);
}

u32 ThreadContext::main(void) {
	return stopThread == true ? 0 : 1;
}

bool ThreadContext::start(void) {
  defend_msg (threadId == zb_thread::InvalidHandle, "Thread attempted to start twice.");

  stopThread = false;
  threadId = zb_thread::start(&startup_callback, this);
  return threadId != zb_thread::InvalidHandle;
}

void ThreadContext::stop(u32 countdown) {
  if(threadId == zb_thread::InvalidHandle) {
    return;
  }

  request_stop();
  zb_thread::stop(threadId, countdown);
  threadId = zb_thread::InvalidHandle;
}

void ThreadContext::request_stop(void) {
  stopThread = true;
}

bool ThreadContext::running(void) {
  return threadId != zb_thread::InvalidHandle;
}

u32 ThreadContext::startup_callback(void *caller) {
	ThreadContext *callingInstance = reinterpret_cast<ThreadContext*>(caller);

	u32 retval = callingInstance->main();
  callingInstance->threadId = zb_thread::InvalidHandle;

	return retval;
}
