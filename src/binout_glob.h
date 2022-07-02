#ifndef BINOUT_GLOB_H
#define BINOUT_GLOB_H
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Takes in a glob pattern and returns all globed files and writes the number
 * of files into num_files The return value needs to be deallocated using
 * binout_free_glob*/
char **binout_glob(const char *pattern, size_t *num_files);
/* Free the allocated memory*/
void binout_free_glob(char **globed_files, size_t num_files);

#ifdef __cplusplus
}
#endif

#endif