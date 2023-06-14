#ifndef MULTI_FILE_H
#define MULTI_FILE_H

#include "sync.h"
#include <stdio.h>

typedef struct {
  FILE *file_handle;
  sync_t mutex;
} sync_file_t;

/* A file that can be opened (read-only) on multiple threads and read in a
 * thread safe way*/
typedef struct {
  char *file_path;

  sync_file_t *file_handles;
  size_t num_file_handles;

  sync_t file_handles_mutex;
} multi_file_t;

/* Initialize the multi file for path. Needs to be closed by multi_file_close*/
multi_file_t multi_file_open(const char *path);

/* Close the multi file and deallocate all resources. Non thread-safe*/
void multi_file_close(multi_file_t *f);

/* Returns an index for a file handle of which the calling threads takes
 * ownership. Returns ULONG_MAX if it fails to open a new file. Needs to be
 * returned with multi_file_return*/
size_t multi_file_access(multi_file_t *f);

/* Returns the file accessed by multi_file_access and releases it to be used by
 * other threads*/
void multi_file_return(multi_file_t *f, size_t index);

/* Same as fseek, but for a file of multi file*/
int multi_file_seek(multi_file_t *f, size_t index, long offset, int whence);
/* Same as ftell, but for a file of multi file*/
long multi_file_tell(multi_file_t *f, size_t index);
/* Similar to fread, but it just uses size to specify the number of bytes
 * instead of size and nmemb*/
size_t multi_file_read(multi_file_t *f, size_t index, void *ptr, size_t size);

#endif