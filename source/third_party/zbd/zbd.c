//
// See copyright notice in zbd.h.
//

#include "zbd.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

ZB_NAMESPACE_BEGIN

#if !defined(ZB_DISABLE_MEMORY)
const char *zb_anonymous_new_name = "Anonymous New";
const char *zb_anonymous_delete_name = "Anonymous Delete";
const char *zb_function_unknown_name = "Unknown";
const char *zb_file_unknown_name = "Unknown";

#if defined(ZB_PLATFORM_WINDOWS)
#include <windows.h>
#endif

void *zb_heap_alloc(size_t size) {
#if defined(ZB_PLATFORM_WINDOWS)
#if defined(ZB_MEMORY_DEBUG)
  const u32 flags = HEAP_ZERO_MEMORY;
#else
  const u32 flags = 0;
#endif
  return HeapAlloc(GetProcessHeap(), flags, size);
#else
#error Not implemented for this platform
#endif
}

void zb_heap_free(void *p) {
#if defined(ZB_PLATFORM_WINDOWS)
  HeapFree(GetProcessHeap(), 0, p);
#else
#error Not implemented for this platform
#endif
}

#if !defined(ZB_MEMORY_OVERRIDE_MALLOC)
void *zb_malloc(size_t size, i32 line, const char *function, const char *file, u32 id, const char *name) {
  void *p = zb_heap_alloc(size);
  zrecord_malloc(p, size, line, function, file, id, name);
  return p;
}

//void *zb_malloc_aligned(u32 size, u32 alignment, i32 line, const char *function, const char *file, const char *name) {
//
//}

void zb_free(void *p, i32 line, const char *function, const char *file, u32 id) {
  zrecord_free(p, line, function, file, id);
  zb_heap_free(p);
}

void zb_array_free(void *p, i32 line, const char *function, const char *file, u32 id) {
  zrecord_array_free(p, line, function, file, id);
}

#if defined(ZB_MEMORY_RECORD) && !defined(ZB_MEMORY_OVERRIDE_RECORD)
#if !defined(ZB_MEMORY_NETWORK_DISABLE)
// Data arranged as pointer, size, id
// Cases
// 4-4-4 (unlikely) 16
// 4-4-2 (unlikely) 10
// 4-2-4 (unlikely) 10
// 4-2-2 (most likely) 8
// 8-4-4 (unlikely) 16
// 8-4-2 (unlikely) 14
// 8-2-4 (unlikely) 14 
// 8-2-2 (likely?) 12
void zb_record_malloc(void *p, size_t size, i32 line, const char *function, const char *file, u32 id, const char *name) {
  silence_unused(p);
  silence_unused(size);
  silence_unused(line);
  silence_unused(function);
  silence_unused(file);
  silence_unused(id); 
  silence_unused(name);
  zb_record_malloc_network_update(p, size, id);
}

void zb_record_free(void *p, i32 line, const char *function, const char *file, u32 id) {
  silence_unused(p);
  silence_unused(line);
  silence_unused(function);
  silence_unused(file);
  silence_unused(id);
  zb_record_free_network_update(p, id);
}

void zb_record_array_free(void *p, i32 line, const char *function, const char *file, u32 id) {
  silence_unused(p);
  silence_unused(line);
  silence_unused(function);
  silence_unused(file);
  silence_unused(id);
  zb_record_free_network_update(p, id);
}
#else // Local recording
  // TODO
#endif
#endif
#endif
#endif

#if !defined(ZB_DISABLE_ASSERT)
i32 zb_null_assertion_handler(const char *condition, const char *message, const char *function, const char *file, i32 line, zb_stack_trace *trace) { 
  silence_unused(condition);
  silence_unused(message);
  silence_unused(function);
  silence_unused(file);
  silence_unused(line);
  silence_unused(trace);

  return 0;
}

i32 zb_stderr_assertion_handler(const char *condition, const char *message, const char *function, const char *file, i32 line, zb_stack_trace *trace) {
  silence_unused(function);
  silence_unused(trace);

  fprintf(stderr, " --");

  if (message != 0x0) {
    fprintf(stderr, "%s", message);
  }

  if (condition != 0x0) {
    fprintf(stderr, " [%s] failed on", condition);
  }

  fprintf(stderr, " (%s:%i)", file, line);

  return 1;
}

#if defined(ZB_PLATFORM_WINDOWS)
i32 zb_ods_assertion_handler(const char *condition, const char *message, const char *function, const char *file, i32 line, zb_stack_trace *trace) {
  char lineNumber[33] = {0};
  
  silence_unused(function);
  silence_unused(trace);

  OutputDebugStringA(" --");

  if (message != 0x0) {
    OutputDebugStringA(message);
  }

  if (condition != 0x0) {
    OutputDebugStringA(" [");
    OutputDebugStringA(condition);
    OutputDebugStringA("] failed on");
  }

  OutputDebugStringA(" (");
  OutputDebugStringA(file);
  OutputDebugStringA(":");

  _itoa(line, lineNumber, 10);
  OutputDebugStringA(lineNumber);

  OutputDebugStringA(")\n");

  return 1;
}
#endif

static zb_assertion_handler zb_assert_handler = zb_stderr_assertion_handler;
zb_assertion_handler zb_get_assertion_handler(void) { 
  return zb_assert_handler; 
}

zb_assertion_handler zb_set_assertion_handler(zb_assertion_handler new_handler) {
  zb_assertion_handler old_handler = zb_assert_handler;
  zb_assert_handler = new_handler;
  return old_handler;
}

static i32 zb_assert_activated = 0;
i32 zb_assertion_failure(const char *condition, const char *function,  const char *file, i32 line, const char *message, zb_stack_trace *trace) {
  i32 halt = 0;
  if (zb_assert_activated) {
    return 1;
  }

  zb_assert_activated = 1;
  halt = zb_assert_handler(condition, message, function, file, line, trace);
  zfree(trace);
  zb_assert_activated = 0;

  return halt;
}

i32 zb_assertion_failure_formatted(const char *condition, const char *function,  const char *file, i32 line, const char *message, zb_stack_trace *trace, ...) {
  i32 halt = 0;

  va_list args;
  char formatted_message[2048];
  formatted_message[0] = 0;

  if (zb_assert_activated) {
    return 1;
  }

  zb_assert_activated = 1;

  va_start(args, trace);
#if defined (_MSC_VER)
  vsprintf_s(formatted_message, sizeof(formatted_message), message, args);
#else
  vsprintf(formatted_message, message, args);
#endif
  va_end(args);

  halt = zb_assert_handler(condition, formatted_message, function, file, line, trace);
  zfree(trace);
  zb_assert_activated = 0;

  return halt;
}
#endif

#if !defined(ZB_DISABLE_LOG)
void zb_null_log_handler(const char *message, const char *function, const char *file, i32 line, i32 channel, i32 newline) {
  silence_unused(message);
  silence_unused(function);
  silence_unused(file);
  silence_unused(line);
  silence_unused(channel);
  silence_unused(newline);
}

void zb_debug_log_handler(const char *message, const char *function, const char *file, i32 line, i32 channel, i32 newline) {
  silence_unused(function);
  silence_unused(file);
  silence_unused(line);
  silence_unused(channel);

  OutputDebugStringA(message);
  if (newline) {
    OutputDebugStringA("\n");  
  }
}

static zb_log_handler zb_logging_handler = zb_debug_log_handler;
zb_log_handler zb_get_log_handler(void) {
  return zb_logging_handler;
}

zb_log_handler zb_set_log_handler(zb_log_handler new_handler) {
  zb_log_handler old_handler = zb_logging_handler;
  zb_logging_handler = new_handler;
  return old_handler;
}

void zb_log_message(const char *message, const char *function, const char *file, i32 line, i32 channel, i32 newline, ...) {
  va_list args;
  char formatted_message[2048];

  formatted_message[0] = 0;
  va_start(args, newline);
#if defined (_MSC_VER)
  vsprintf_s(formatted_message, sizeof(formatted_message), message, args);
#else
  vsprintf(formatted_message, message, args);
#endif
  va_end(args);

  zb_logging_handler(formatted_message, function, file, line, channel, newline);
}
#endif


#if !defined(ZB_DISABLE_STACK_TRACE)
#if defined(ZB_PLATFORM_WINDOWS)
#include <windows.h>
#include <DbgHelp.h>

static i32 zb_stack_trace_initialized = 0;
static CRITICAL_SECTION zb_dbghelp_lock;

void zb_initialize_stack_trace(void) {
  if (!zb_stack_trace_initialized) {
    InitializeCriticalSection(&zb_dbghelp_lock);
    EnterCriticalSection(&zb_dbghelp_lock);

    SymSetOptions(SYMOPT_LOAD_LINES|SYMOPT_UNDNAME|SYMOPT_DEFERRED_LOADS|SymGetOptions());
    SymInitialize(GetCurrentProcess(), 0x0, TRUE);

    zb_stack_trace_initialized = 1;
    LeaveCriticalSection(&zb_dbghelp_lock);  
  }
}

void zb_shutdown_stack_trace(void) {
  EnterCriticalSection(&zb_dbghelp_lock);
  SymCleanup(GetCurrentProcess());
  LeaveCriticalSection(&zb_dbghelp_lock);  
  DeleteCriticalSection(&zb_dbghelp_lock);
  zb_stack_trace_initialized = 0;
}

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4740 4748)
#endif
void zb_capture_stack_trace(zb_stack_trace *trace, u32 maxDepth) {
  DWORD machineType;
  CONTEXT context;
  STACKFRAME64 stackFrame;
  char symbolBuffer[sizeof(SYMBOL_INFO) + ZB_TRACE_NAME_MAX * sizeof(TCHAR)];
  PSYMBOL_INFO symbolInfo = (PSYMBOL_INFO)symbolBuffer;
  IMAGEHLP_LINE64 lineInfo = {0};
  i32 frameIndex;
  ptrdiff_t copyCount;
  i32 skipFrame = 1;
  DWORD dummyOffset = 0;

  trace->frame_count = 0;

  if (!zb_stack_trace_initialized) {
    return;
  }

  symbolInfo->SizeOfStruct = sizeof(SYMBOL_INFO);
  symbolInfo->MaxNameLen = ZB_TRACE_NAME_MAX;

  lineInfo.SizeOfStruct = sizeof(IMAGEHLP_LINE64);

#ifdef _M_IX86
  ZeroMemory(&context, sizeof(context));
  context.ContextFlags = CONTEXT_CONTROL;
  __asm {
Label:
    mov [context.Ebp], ebp;
    mov [context.Esp], esp;
    mov eax, [Label];
    mov [context.Eip], eax;
  }
#else
  RtlCaptureContext(&context);
#endif

  ZeroMemory(&stackFrame, sizeof(stackFrame));
#ifdef _M_IX86
  machineType                 = IMAGE_FILE_MACHINE_I386;
  stackFrame.AddrPC.Offset    = context.Eip;
  stackFrame.AddrPC.Mode      = AddrModeFlat;
  stackFrame.AddrFrame.Offset = context.Ebp;
  stackFrame.AddrFrame.Mode   = AddrModeFlat;
  stackFrame.AddrStack.Offset = context.Esp;
  stackFrame.AddrStack.Mode   = AddrModeFlat;
#elif _M_X64
  machineType                 = IMAGE_FILE_MACHINE_AMD64;
  stackFrame.AddrPC.Offset    = context.Rip;
  stackFrame.AddrPC.Mode      = AddrModeFlat;
  stackFrame.AddrFrame.Offset = context.Rsp;
  stackFrame.AddrFrame.Mode   = AddrModeFlat;
  stackFrame.AddrStack.Offset = context.Rsp;
  stackFrame.AddrStack.Mode   = AddrModeFlat;
#elif _M_IA64
  machineType                 = IMAGE_FILE_MACHINE_IA64;
  stackFrame.AddrPC.Offset    = context.StIIP;
  stackFrame.AddrPC.Mode      = AddrModeFlat;
  stackFrame.AddrFrame.Offset = context.IntSp;
  stackFrame.AddrFrame.Mode   = AddrModeFlat;
  stackFrame.AddrBStore.Offset= context.RsBSP;
  stackFrame.AddrBStore.Mode  = AddrModeFlat;
  stackFrame.AddrStack.Offset = context.IntSp;
  stackFrame.AddrStack.Mode   = AddrModeFlat;
#else
#error Architecture not supported.
#endif

  EnterCriticalSection(&zb_dbghelp_lock);
  while (trace->frame_count < maxDepth) {
    HANDLE process = GetCurrentProcess();
    if (!StackWalk64(machineType, process, GetCurrentThread(), &stackFrame, &context, 0x0, SymFunctionTableAccess64, SymGetModuleBase64, 0x0)) {
      break;
    }

    // Skip the first entry since it's this function.
    if (skipFrame) {
      skipFrame = 0;
      continue;
    }

    if (stackFrame.AddrPC.Offset == 0) {
      break;
    }

    frameIndex = trace->frame_count++;
    if (SymFromAddr(process, stackFrame.AddrPC.Offset, 0x0, symbolInfo)) {
      copyCount = symbolInfo->NameLen < ZB_TRACE_NAME_MAX - 1 ? symbolInfo->NameLen : ZB_TRACE_NAME_MAX - 1;
      memcpy(trace->names[frameIndex], symbolInfo->Name, copyCount);
      trace->names[frameIndex][copyCount] = 0;

      if (SymGetLineFromAddr64(process, stackFrame.AddrPC.Offset, &dummyOffset, &lineInfo)) {
        PCHAR end = lineInfo.FileName;
        PCHAR start = end;

        while (*end++ != 0) ;
        --end;

        start = end;
        while (*start != '\\' && *start != '/') { --start; }
        ++start;

        copyCount = end - start < ZB_TRACE_FILENAME_MAX - 1 ? end - start : ZB_TRACE_FILENAME_MAX - 1;
        memcpy(trace->filenames[frameIndex], start, copyCount);
        trace->filenames[frameIndex][copyCount] = 0;

        trace->lines[frameIndex] = lineInfo.LineNumber;
      }
      else {
        trace->lines[frameIndex] = 0;
        trace->filenames[frameIndex][0] = 0;
      }

      trace->addresses[frameIndex] = (u32)stackFrame.AddrPC.Offset;
    }
    else {
      trace->names[frameIndex][0] = 0;
    }
  }
  LeaveCriticalSection(&zb_dbghelp_lock);
}
#if defined(_MSC_VER)
#pragma warning(pop)
#endif

#endif
#endif

ZB_NAMESPACE_END
