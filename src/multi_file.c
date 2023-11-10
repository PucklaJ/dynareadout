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

#include "multi_file.h"
#include "path.h"
#include "profiling.h"
#include "string_builder.h"

#ifndef NO_THREAD_SAFETY
#include <limits.h>
#include <stdlib.h>
#include <string.h>

multi_file_t multi_file_open(const char *path) {
  BEGIN_PROFILE_FUNC();

  multi_file_t f;

  if (!path_is_abs(path)) {
    char *wd = path_working_directory();
    f.file_path = path_join_real(wd, path);
    free(wd);
  } else {
    f.file_path = string_clone(path);
  }

  f.file_handles = NULL;
  f.num_file_handles = 0;
  f.file_handles_mutex = sync_create();

  END_PROFILE_FUNC();
  return f;
}

void multi_file_close(multi_file_t *f) {
  BEGIN_PROFILE_FUNC();

  free(f->file_path);

  size_t i = 0;
  while (i < f->num_file_handles) {
    if (f->file_handles[i].file_handle)
      fclose(f->file_handles[i].file_handle);

    i++;
  }

  free(f->file_handles);
  sync_destroy(&f->file_handles_mutex);

  f->file_path = NULL;
  f->file_handles = NULL;
  f->num_file_handles = 0;

  END_PROFILE_FUNC();
}

multi_file_index_t multi_file_access(multi_file_t *f) {
  BEGIN_PROFILE_FUNC();
  sync_lock(&f->file_handles_mutex);

  /* Search for a free sync file*/
  size_t i = 0;
  while (i < f->num_file_handles) {
    if (sync_trylock(&f->file_handles[i].mutex) == 0) {
      if (!f->file_handles[i].file_handle) {
        /* If the file is not yet open*/
        f->file_handles[i].file_handle = fopen(f->file_path, "rb");
        if (!f->file_handles[i].file_handle) {
          sync_unlock(&f->file_handles[i].mutex);
        } else {
          multi_file_index_t index;
          index.file_handle = f->file_handles[i].file_handle;
          index.index = i;

          sync_unlock(&f->file_handles_mutex);
          END_PROFILE_FUNC();
          return index;
        }
      } else {
        /* If the file is already open*/
        multi_file_index_t index;
        index.file_handle = f->file_handles[i].file_handle;
        index.index = i;

        sync_unlock(&f->file_handles_mutex);
        END_PROFILE_FUNC();
        return index;
      }
    }

    i++;
  }

  /* Create a new sync file, because no free file has been found*/
  f->num_file_handles++;
  f->file_handles =
      realloc(f->file_handles, f->num_file_handles * sizeof(sync_file_t));

  sync_file_t *new_file = &f->file_handles[f->num_file_handles - 1];

  new_file->mutex = sync_create();
  new_file->file_handle = fopen(f->file_path, "rb");
  if (!new_file->file_handle) {
    multi_file_index_t index;
    index.file_handle = NULL;
    index.index = ULONG_MAX;

    sync_unlock(&f->file_handles_mutex);
    END_PROFILE_FUNC();
    return index;
  }

  sync_lock(&new_file->mutex);

  multi_file_index_t index;
  index.file_handle = new_file->file_handle;
  index.index = f->num_file_handles - 1;

  sync_unlock(&f->file_handles_mutex);
  END_PROFILE_FUNC();
  return index;
}

void multi_file_return(multi_file_t *f, multi_file_index_t *index) {
  BEGIN_PROFILE_FUNC();
  sync_lock(&f->file_handles_mutex);

  sync_unlock(&f->file_handles[index->index].mutex);

  sync_unlock(&f->file_handles_mutex);
  END_PROFILE_FUNC();
}

int multi_file_seek(multi_file_t *f, multi_file_index_t *index, long offset,
                    int whence) {
  BEGIN_PROFILE_FUNC();

  const int rv = fseek(index->file_handle, offset, whence);

  END_PROFILE_FUNC();
  return rv;
}

long multi_file_tell(multi_file_t *f, multi_file_index_t *index) {
  BEGIN_PROFILE_FUNC();

  const long rv = ftell(index->file_handle);

  END_PROFILE_FUNC();
  return rv;
}

size_t multi_file_read(multi_file_t *f, multi_file_index_t *index, void *ptr,
                       size_t size, size_t nmemb) {
  BEGIN_PROFILE_FUNC();

  const size_t rv = fread(ptr, size, nmemb, index->file_handle);

  END_PROFILE_FUNC();
  return rv;
}

#else

multi_file_t multi_file_open(const char *path) {
  BEGIN_PROFILE_FUNC();

  multi_file_t f = (multi_file_t)fopen(path, "rb");

  END_PROFILE_FUNC();
  return f;
}

void multi_file_close(multi_file_t *f) {
  BEGIN_PROFILE_FUNC();

  if (*f) {
    fclose(*((FILE **)f));
    *f = NULL;
  }

  END_PROFILE_FUNC();
}

size_t multi_file_access(multi_file_t *f) {
  BEGIN_PROFILE_FUNC();
  END_PROFILE_FUNC();
  return 0;
}

void multi_file_return(multi_file_t *f, multi_file_index_t *index) {
  BEGIN_PROFILE_FUNC();
  END_PROFILE_FUNC();
}

int multi_file_seek(multi_file_t *f, multi_file_index_t *index, long offset,
                    int whence) {
  BEGIN_PROFILE_FUNC();

  const int rv = fseek(*((FILE **)f), offset, whence);

  END_PROFILE_FUNC();
  return rv;
}

long multi_file_tell(multi_file_t *f, multi_file_index_t *index) {
  BEGIN_PROFILE_FUNC();

  const long rv = ftell(*((FILE **)f));

  END_PROFILE_FUNC();
  return rv;
}

size_t multi_file_read(multi_file_t *f, multi_file_index_t *index, void *ptr,
                       size_t size, size_t nmemb) {
  BEGIN_PROFILE_FUNC();

  const size_t rv = fread(ptr, size, nmemb, *((FILE **)f));

  END_PROFILE_FUNC();
  return rv;
}

#endif