#include <memory.h>

const size_t InvalidHandleIndex = static_cast<size_t>(-1);

class Handle {
public:
  class DefragmentingAllocator *owner;
  size_t index;
};

class DefragmentingAllocator {
public:
  DefragmentingAllocator(void);

  void initialize(void *pool, size_t size, size_t handleCount);

  Handle allocate(int size);
  Handle allocate_handle(void *p);
  void deallocate(Handle h);

  void *resolve_handle(Handle h);

  void defragment(int maxBlocks);

private:
  void *allocate_chunk(int size);
  void deallocate_chunk(void *p);
  void adjust_handles_in_range(void *start, void *end, int adjustment);

  void *pool_start;
  void *pool_end;

  size_t next_handle_index;
  void *allocations_start;
  void *next_allocation;
  void *defrag_location;
};

DefragmentingAllocator::DefragmentingAllocator(void)
: pool_start(0x0)
, pool_end(0x0)
, next_handle_index(0)
, allocations_start(0x0)
, next_allocation(0x0)
, defrag_location(0x0)
{}

void DefragmentingAllocator::initialize(void *pool, size_t size, size_t handleCount) {
  //defend (size > 0);
  //defend (handleCount > 0);

  pool_start = pool;
  pool_end = static_cast<char*>(pool) + size;

  next_handle_index = 0;
  allocations_start = static_cast<void**>(pool_start) + handleCount;
  next_allocation = allocations_start;
  defrag_location = allocations_start;

  memset(pool_start, 0, handleCount * sizeof(void*));
}

Handle DefragmentingAllocator::allocate(int size) {
  return allocate_handle(allocate_chunk(size));
}

void DefragmentingAllocator::deallocate(Handle h) {
  void **handlePool = static_cast<void**>(pool_start);
  deallocate_chunk(resolve_handle(h));
  handlePool[h.index] = 0x0;
}

void *DefragmentingAllocator::resolve_handle(Handle h) {
  //defend (h.owner == this);
  //defend (h.index != InvalidHandleIndex);
  return static_cast<void**>(pool_start)[h.index];
}

Handle DefragmentingAllocator::allocate_handle(void *p) {
  void **handlePool = static_cast<void**>(pool_start);
  const size_t slotCount = static_cast<void**>(allocations_start) - handlePool;

  // Search from where we are to the end.
  size_t handleIndex = next_handle_index;
  while (handleIndex < slotCount) {
    if (handlePool[handleIndex] == 0x0) {
      break;
    }

    ++handleIndex;
  }

  // Wrap to the beginning and search to where we started.
  if (handlePool[handleIndex] != 0x0) {
    handleIndex = 0;
    const size_t wrapPoint = next_handle_index;
    while (handleIndex < wrapPoint) {
      if (handlePool[handleIndex] == 0x0) {
        break;
      }

      ++handleIndex;
    }
  }

  Handle newHandle;
  newHandle.owner = this;

  if (handlePool[handleIndex] == 0x0) {
    handlePool[handleIndex] = p;
    newHandle.index = handleIndex;
  }
  else {
    newHandle.index = InvalidHandleIndex;
  }

  next_handle_index = handleIndex + 1;
  return newHandle;
}

namespace DefragmentingAllocatorHelpers {
  size_t AlignmentAdjustment64(void *address) {
    const size_t alignment = 8;
    const size_t misalignment = reinterpret_cast<size_t>(address) & (alignment - 1);
    return alignment - misalignment;
  }
}

void *DefragmentingAllocator::allocate_chunk(int size) {
  char *allocationPoint = static_cast<char*>(next_allocation);
  size_t adjustment = DefragmentingAllocatorHelpers::AlignmentAdjustment64(allocationPoint + sizeof(int));

  const int allocationSize = size + sizeof(int) + adjustment;
  void *chunk = 0x0;

  if (allocationPoint + allocationSize <= pool_end)
  {
    *reinterpret_cast<int*>(allocationPoint) = allocationSize;
    chunk = allocationPoint + sizeof(int) + adjustment;
    unsigned char *tag = static_cast<unsigned char*>(chunk) - 1;
    *tag = static_cast<unsigned char>(adjustment);
    next_allocation = allocationPoint + allocationSize;
  }

  return chunk;
}

void DefragmentingAllocator::deallocate_chunk(void *p) {
  unsigned char *alignmentTag = static_cast<unsigned char*>(p) - 1;
  const unsigned char adjustment = *alignmentTag;

  int *allocationTag = reinterpret_cast<int*>(reinterpret_cast<unsigned char*>(p) - adjustment) - 1;
  const int allocationSize = *allocationTag;
  //defend (allocationSize > sizeof(int));

  *allocationTag = -allocationSize;
}

void DefragmentingAllocator::defragment(int blocksToScan) {
  void *firstBlockBase = defrag_location;
  void *allocationsEnd = next_allocation;

  if (firstBlockBase == allocationsEnd) {
    return;
  }

  int firstBlockSize = *static_cast<int*>(firstBlockBase);
  while (blocksToScan > 0) {
    if (firstBlockSize > 0) {
      firstBlockBase = static_cast<char*>(firstBlockBase) + firstBlockSize;
      firstBlockSize = *static_cast<int*>(firstBlockBase);
      --blocksToScan;
      continue;
    }

    // firstBlockSize < 0 at this point, means we've found a gap.
    void *secondBlockBase = static_cast<char*>(firstBlockBase) - firstBlockSize;
    if (secondBlockBase == allocationsEnd) {
      // Free space until the end, move the allocation marker up and reset
      // the defrag location to the front for next time.
      allocationsEnd = firstBlockBase;
      firstBlockBase = allocations_start;
      firstBlockSize = *static_cast<int*>(firstBlockBase);
      --blocksToScan;
      continue;
    }

    int secondBlockSize = *static_cast<int*>(secondBlockBase);
    if (secondBlockSize < 0) {
      firstBlockSize += secondBlockSize;
      continue;
    }

    // secondBlockSize > 0 at this point, means we've found a non-empty block.
    const size_t readAdjustment = DefragmentingAllocatorHelpers::AlignmentAdjustment64(static_cast<char*>(secondBlockBase) + sizeof(int));
    const void *readBase = static_cast<char*>(secondBlockBase) + sizeof(int) + readAdjustment;
    const int readSize = secondBlockSize - sizeof(int) - readAdjustment;

    const size_t writeAdjustment = DefragmentingAllocatorHelpers::AlignmentAdjustment64(static_cast<char*>(firstBlockBase) + sizeof(int));
    void *writeBase = static_cast<char*>(firstBlockBase) + sizeof(int) + writeAdjustment;
    const int writeSize = readSize + sizeof(int) + writeAdjustment;

    *static_cast<int*>(firstBlockBase) = writeSize;
    unsigned char *tag = static_cast<unsigned char*>(writeBase) - 1;
    *tag = static_cast<unsigned char>(writeAdjustment);
    memcpy(writeBase, readBase, readSize);

    firstBlockBase = static_cast<char*>(firstBlockBase) + writeSize;
    firstBlockSize = firstBlockSize - secondBlockSize + writeSize;
    --blocksToScan;

    adjust_handles_in_range(secondBlockBase, static_cast<char*>(secondBlockBase) + secondBlockSize, static_cast<char*>(writeBase) - reinterpret_cast<const char*>(readBase));
  }

  *static_cast<int*>(firstBlockBase) = firstBlockSize;
  defrag_location = firstBlockBase;
  next_allocation = allocationsEnd;
}

void DefragmentingAllocator::adjust_handles_in_range(void *start, void *end, int adjustment) {
  char **handlePool = static_cast<char**>(pool_start);
  const size_t slotCount = static_cast<char**>(allocations_start) - handlePool;

  for (size_t i = 0; i < slotCount; ++i) {
    const void *address = handlePool[i];
    if (address > start && address < end) {
      handlePool[i] += adjustment;
    }
  }
}
