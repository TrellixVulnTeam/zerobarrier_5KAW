
FileHandle zb_open_file(const char *filename) {
  defend (filename != 0x0);
  return _wfopen(zb_widen(filename).c_str(), L"rb");
}

void zb_close_file(FileHandle handle) {
  defend (handle != 0x0);
  fclose(handle);
}

bool zb_load_file(const char *filename, void **dataOut, u32 *sizeOut, bool nullTerminate) {
  FileHandle handle = zb_open_file(filename);
  bool success = false;

  if (handle != 0x0) {
    const u32 fileSize = zb_read_file_size(handle);
    char *buffer = znew("zb_filebuffer") char[fileSize + (nullTerminate ? 1 : 0)];
    
    success = zb_read_file(handle, buffer, fileSize);

    if (success) {
      if (nullTerminate) {
        buffer[fileSize] = 0x0;
      }

      *dataOut = buffer;
      *sizeOut = fileSize;
    }
    else {
      *dataOut = 0x0;
      *sizeOut = 0;
      zb_free_file_buffer(buffer);
    }

    zb_close_file(handle);
  }
  
  return success;
}

void zb_free_file_buffer(void *buffer) {
  zarray_delete(buffer);
}

u32 zb_read_file_size(FileHandle handle) {
  defend (handle != InvalidFileHandle);
  fseek(handle, 0, SEEK_END);
  const u32 size = ftell(handle);
  fseek(handle, 0, SEEK_SET);

  return size;
}

bool zb_read_file(FileHandle handle, void *dataOut, const u32 readSize) {
  defend (handle != 0x0);
  return fread(dataOut, readSize, 1, handle) == 1;
}
