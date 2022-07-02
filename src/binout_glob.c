#include "binout_glob.h"
#include "path.h"

#ifdef _WIN32

#else
#include <glob.h>

char **binout_glob(const char *pattern, size_t *num_files) {
  glob_t glob_buffer;
  const int error_code = glob(pattern, GLOB_TILDE, NULL, &glob_buffer);

  if (error_code != 0) {
    *num_files = 0;
    globfree(&glob_buffer);
    return NULL;
  }

  /* Directly use the values from glob_buffer. Why not?*/
  *num_files = glob_buffer.gl_pathc;
  return glob_buffer.gl_pathv;
}

#endif

void binout_free_glob(char **globed_files, size_t num_files) {
  path_free_elements(globed_files, num_files);
}