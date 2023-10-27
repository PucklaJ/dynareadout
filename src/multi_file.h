/***********************************************************************************
 *                         This file is part of dynareadout
 *                    https://github.com/PucklaJ/dynareadout
 ***********************************************************************************
 * Copyright (c) 2022 Jonas Pucher
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from the
 * use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not claim
 * that you wrote the original software. If you use this software in a product,
 * an acknowledgment in the product documentation would be appreciated but is
 * not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 ************************************************************************************/

#ifndef MULTI_FILE_H
#define MULTI_FILE_H

#include "sync.h"
#include <stdio.h>

#ifndef NO_THREAD_SAFETY
typedef struct {
  FILE *file_handle;
  size_t index;
} multi_file_index_t;

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

#else
typedef size_t multi_file_index_t;
typedef FILE *multi_file_t;

#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Initialize the multi file for path. Needs to be closed by multi_file_close*/
multi_file_t multi_file_open(const char *path);

/* Close the multi file and deallocate all resources. Non thread-safe*/
void multi_file_close(multi_file_t *f);

/* Returns an index for a file handle of which the calling threads takes
 * ownership. Returns ULONG_MAX if it fails to open a new file. Needs to be
 * returned with multi_file_return*/
multi_file_index_t multi_file_access(multi_file_t *f);

/* Returns the file accessed by multi_file_access and releases it to be used by
 * other threads*/
void multi_file_return(multi_file_t *f, multi_file_index_t *index);

/* Same as fseek, but for a file of multi file*/
int multi_file_seek(multi_file_t *f, multi_file_index_t *index, long offset,
                    int whence);
/* Same as ftell, but for a file of multi file*/
long multi_file_tell(multi_file_t *f, multi_file_index_t *index);
/* Similar to fread, but it just uses size to specify the number of bytes
 * instead of size and nmemb*/
size_t multi_file_read(multi_file_t *f, multi_file_index_t *index, void *ptr,
                       size_t size, size_t nmemb);

#ifdef __cplusplus
}
#endif

#endif