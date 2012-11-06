#pragma once

typedef FILE *FileHandle;
const FileHandle InvalidFileHandle = 0x0;

bool zb_load_file(const char *filename, void **dataOut, u32 *sizeOut, bool nullTerminate = false);
void zb_free_file_buffer(void *buffer);

u32 zb_read_file_size(FileHandle handle);
bool zb_read_file(FileHandle handle, void *dataOut, u32 dataSize);

FileHandle zb_open_file(const char *filename);
void zb_close_file(FileHandle handle);
