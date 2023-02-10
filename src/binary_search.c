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

/* This file has been automatically generated*/

#include "binary_search.h"
#include "profiling.h"
#include <string.h>

int d3_word_cmp(const d3_word a, const d3_word b) {
  return a > b ? 1 : (a < b ? -1 : 0);
}

size_t binout_directory_binary_search_folder(const binout_folder_t *arr,
                                             size_t start_index,
                                             size_t end_index,
                                             const path_view_t *value) {
  BEGIN_PROFILE_FUNC();

  if (start_index == end_index) {
    if (path_view_strcmp(value, arr[start_index].name) == 0) {
      END_PROFILE_FUNC();
      return start_index;
    }

    END_PROFILE_FUNC();
    return ~0;
  }

  const size_t half_index = start_index + (end_index - start_index) / 2;
  const int cmp_val = path_view_strcmp(value, arr[half_index].name);

  if (cmp_val < 0) {
    const size_t index = binout_directory_binary_search_folder(
        arr, start_index, half_index, value);
    END_PROFILE_FUNC();
    return index;
  } else if (cmp_val > 0) {
    if (half_index == end_index - 1) {
      const size_t index = binout_directory_binary_search_folder(
          arr, end_index, end_index, value);
      END_PROFILE_FUNC();
      return index;
    }
    const size_t index = binout_directory_binary_search_folder(
        arr, half_index, end_index, value);
    END_PROFILE_FUNC();
    return index;
  }

  END_PROFILE_FUNC();
  return half_index;
}

size_t binout_directory_binary_search_folder_insert(const binout_folder_t *arr,
                                                    size_t start_index,
                                                    size_t end_index,
                                                    const path_view_t *value,
                                                    int *found) {
  BEGIN_PROFILE_FUNC();

  if (start_index == end_index) {
    const int cmp_value = path_view_strcmp(value, arr[start_index].name);

    if (cmp_value == 0) {
      *found = 1;
      END_PROFILE_FUNC();
      return start_index;
    }
    if (cmp_value > 0) {
      *found = 0;
      END_PROFILE_FUNC();
      return start_index + 1;
    }

    *found = 0;
    END_PROFILE_FUNC();
    return start_index;
  }

  const size_t half_index = start_index + (end_index - start_index) / 2;
  const int cmp_val = path_view_strcmp(value, arr[half_index].name);

  if (cmp_val < 0) {
    const size_t index = binout_directory_binary_search_folder_insert(
        arr, start_index, half_index, value, found);
    END_PROFILE_FUNC();
    return index;
  } else if (cmp_val > 0) {
    if (half_index == end_index - 1) {
      const size_t index = binout_directory_binary_search_folder_insert(
          arr, end_index, end_index, value, found);
      END_PROFILE_FUNC();
      return index;
    }
    const size_t index = binout_directory_binary_search_folder_insert(
        arr, half_index, end_index, value, found);
    END_PROFILE_FUNC();
    return index;
  }

  *found = 1;
  END_PROFILE_FUNC();
  return half_index;
}

size_t binout_directory_binary_search_file(const binout_file_t *arr,
                                           size_t start_index, size_t end_index,
                                           const path_view_t *value) {
  BEGIN_PROFILE_FUNC();

  if (start_index == end_index) {
    if (path_view_strcmp(value, arr[start_index].name) == 0) {
      END_PROFILE_FUNC();
      return start_index;
    }

    END_PROFILE_FUNC();
    return ~0;
  }

  const size_t half_index = start_index + (end_index - start_index) / 2;
  const int cmp_val = path_view_strcmp(value, arr[half_index].name);

  if (cmp_val < 0) {
    const size_t index = binout_directory_binary_search_file(arr, start_index,
                                                             half_index, value);
    END_PROFILE_FUNC();
    return index;
  } else if (cmp_val > 0) {
    if (half_index == end_index - 1) {
      const size_t index =
          binout_directory_binary_search_file(arr, end_index, end_index, value);
      END_PROFILE_FUNC();
      return index;
    }
    const size_t index =
        binout_directory_binary_search_file(arr, half_index, end_index, value);
    END_PROFILE_FUNC();
    return index;
  }

  END_PROFILE_FUNC();
  return half_index;
}

size_t binout_directory_binary_search_file_insert(const binout_file_t *arr,
                                                  size_t start_index,
                                                  size_t end_index,
                                                  const char *value,
                                                  int *found) {
  BEGIN_PROFILE_FUNC();

  if (start_index == end_index) {
    const int cmp_value = strcmp(value, arr[start_index].name);

    if (cmp_value == 0) {
      *found = 1;
      END_PROFILE_FUNC();
      return start_index;
    }
    if (cmp_value > 0) {
      *found = 0;
      END_PROFILE_FUNC();
      return start_index + 1;
    }

    *found = 0;
    END_PROFILE_FUNC();
    return start_index;
  }

  const size_t half_index = start_index + (end_index - start_index) / 2;
  const int cmp_val = strcmp(value, arr[half_index].name);

  if (cmp_val < 0) {
    const size_t index = binout_directory_binary_search_file_insert(
        arr, start_index, half_index, value, found);
    END_PROFILE_FUNC();
    return index;
  } else if (cmp_val > 0) {
    if (half_index == end_index - 1) {
      const size_t index = binout_directory_binary_search_file_insert(
          arr, end_index, end_index, value, found);
      END_PROFILE_FUNC();
      return index;
    }
    const size_t index = binout_directory_binary_search_file_insert(
        arr, half_index, end_index, value, found);
    END_PROFILE_FUNC();
    return index;
  }

  *found = 1;
  END_PROFILE_FUNC();
  return half_index;
}

size_t d3_word_binary_search(const d3_word *arr, size_t start_index,
                             size_t end_index, d3_word value) {
  BEGIN_PROFILE_FUNC();

  if (start_index == end_index) {
    if (d3_word_cmp(value, arr[start_index]) == 0) {
      END_PROFILE_FUNC();
      return start_index;
    }

    END_PROFILE_FUNC();
    return ~0;
  }

  const size_t half_index = start_index + (end_index - start_index) / 2;
  const int cmp_val = d3_word_cmp(value, arr[half_index]);

  if (cmp_val < 0) {
    const size_t index =
        d3_word_binary_search(arr, start_index, half_index, value);
    END_PROFILE_FUNC();
    return index;
  } else if (cmp_val > 0) {
    if (half_index == end_index - 1) {
      const size_t index =
          d3_word_binary_search(arr, end_index, end_index, value);
      END_PROFILE_FUNC();
      return index;
    }
    const size_t index =
        d3_word_binary_search(arr, half_index, end_index, value);
    END_PROFILE_FUNC();
    return index;
  }

  END_PROFILE_FUNC();
  return half_index;
}

size_t d3_word_binary_search_insert(const d3_word *arr, size_t start_index,
                                    size_t end_index, d3_word value,
                                    int *found) {
  BEGIN_PROFILE_FUNC();

  if (start_index == end_index) {
    const int cmp_value = d3_word_cmp(value, arr[start_index]);

    if (cmp_value == 0) {
      *found = 1;
      END_PROFILE_FUNC();
      return start_index;
    }
    if (cmp_value > 0) {
      *found = 0;
      END_PROFILE_FUNC();
      return start_index + 1;
    }

    *found = 0;
    END_PROFILE_FUNC();
    return start_index;
  }

  const size_t half_index = start_index + (end_index - start_index) / 2;
  const int cmp_val = d3_word_cmp(value, arr[half_index]);

  if (cmp_val < 0) {
    const size_t index = d3_word_binary_search_insert(arr, start_index,
                                                      half_index, value, found);
    END_PROFILE_FUNC();
    return index;
  } else if (cmp_val > 0) {
    if (half_index == end_index - 1) {
      const size_t index =
          d3_word_binary_search_insert(arr, end_index, end_index, value, found);
      END_PROFILE_FUNC();
      return index;
    }
    const size_t index =
        d3_word_binary_search_insert(arr, half_index, end_index, value, found);
    END_PROFILE_FUNC();
    return index;
  }

  *found = 1;
  END_PROFILE_FUNC();
  return half_index;
}

size_t key_file_binary_search_insert(const keyword_t *arr, size_t start_index,
                                     size_t end_index, const char *value,
                                     int *found) {
  BEGIN_PROFILE_FUNC();

  if (start_index == end_index) {
    const int cmp_value = strcmp(value, arr[start_index].name);

    if (cmp_value == 0) {
      *found = 1;
      END_PROFILE_FUNC();
      return start_index;
    }
    if (cmp_value > 0) {
      *found = 0;
      END_PROFILE_FUNC();
      return start_index + 1;
    }

    *found = 0;
    END_PROFILE_FUNC();
    return start_index;
  }

  const size_t half_index = start_index + (end_index - start_index) / 2;
  const int cmp_val = strcmp(value, arr[half_index].name);

  if (cmp_val < 0) {
    const size_t index = key_file_binary_search_insert(
        arr, start_index, half_index, value, found);
    END_PROFILE_FUNC();
    return index;
  } else if (cmp_val > 0) {
    if (half_index == end_index - 1) {
      const size_t index = key_file_binary_search_insert(
          arr, end_index, end_index, value, found);
      END_PROFILE_FUNC();
      return index;
    }
    const size_t index =
        key_file_binary_search_insert(arr, half_index, end_index, value, found);
    END_PROFILE_FUNC();
    return index;
  }

  *found = 1;
  END_PROFILE_FUNC();
  return half_index;
}

size_t key_file_binary_search(const keyword_t *arr, size_t start_index,
                              size_t end_index, const char *value) {
  BEGIN_PROFILE_FUNC();

  if (start_index == end_index) {
    if (strcmp(value, arr[start_index].name) == 0) {
      END_PROFILE_FUNC();
      return start_index;
    }

    END_PROFILE_FUNC();
    return ~0;
  }

  const size_t half_index = start_index + (end_index - start_index) / 2;
  const int cmp_val = strcmp(value, arr[half_index].name);

  if (cmp_val < 0) {
    const size_t index =
        key_file_binary_search(arr, start_index, half_index, value);
    END_PROFILE_FUNC();
    return index;
  } else if (cmp_val > 0) {
    if (half_index == end_index - 1) {
      const size_t index =
          key_file_binary_search(arr, end_index, end_index, value);
      END_PROFILE_FUNC();
      return index;
    }
    const size_t index =
        key_file_binary_search(arr, half_index, end_index, value);
    END_PROFILE_FUNC();
    return index;
  }

  END_PROFILE_FUNC();
  return half_index;
}

#ifdef PROFILING
size_t string_binary_search_insert(char const **arr, size_t start_index,
                                   size_t end_index, const char *value,
                                   int *found) {
  BEGIN_PROFILE_FUNC();

  if (start_index == end_index) {
    const int cmp_value = strcmp(value, arr[start_index]);

    if (cmp_value == 0) {
      *found = 1;
      END_PROFILE_FUNC();
      return start_index;
    }
    if (cmp_value > 0) {
      *found = 0;
      END_PROFILE_FUNC();
      return start_index + 1;
    }

    *found = 0;
    END_PROFILE_FUNC();
    return start_index;
  }

  const size_t half_index = start_index + (end_index - start_index) / 2;
  const int cmp_val = strcmp(value, arr[half_index]);

  if (cmp_val < 0) {
    const size_t index =
        string_binary_search_insert(arr, start_index, half_index, value, found);
    END_PROFILE_FUNC();
    return index;
  } else if (cmp_val > 0) {
    if (half_index == end_index - 1) {
      const size_t index =
          string_binary_search_insert(arr, end_index, end_index, value, found);
      END_PROFILE_FUNC();
      return index;
    }
    const size_t index =
        string_binary_search_insert(arr, half_index, end_index, value, found);
    END_PROFILE_FUNC();
    return index;
  }

  *found = 1;
  END_PROFILE_FUNC();
  return half_index;
}

size_t profiling_stack_binary_search_insert(const profiling_stack_t *arr,
                                            size_t start_index,
                                            size_t end_index, const char *value,
                                            int *found) {
  BEGIN_PROFILE_FUNC();

  if (start_index == end_index) {
    const int cmp_value = strcmp(value, arr[start_index].execution_name);

    if (cmp_value == 0) {
      *found = 1;
      END_PROFILE_FUNC();
      return start_index;
    }
    if (cmp_value > 0) {
      *found = 0;
      END_PROFILE_FUNC();
      return start_index + 1;
    }

    *found = 0;
    END_PROFILE_FUNC();
    return start_index;
  }

  const size_t half_index = start_index + (end_index - start_index) / 2;
  const int cmp_val = strcmp(value, arr[half_index].execution_name);

  if (cmp_val < 0) {
    const size_t index = profiling_stack_binary_search_insert(
        arr, start_index, half_index, value, found);
    END_PROFILE_FUNC();
    return index;
  } else if (cmp_val > 0) {
    if (half_index == end_index - 1) {
      const size_t index = profiling_stack_binary_search_insert(
          arr, end_index, end_index, value, found);
      END_PROFILE_FUNC();
      return index;
    }
    const size_t index = profiling_stack_binary_search_insert(
        arr, half_index, end_index, value, found);
    END_PROFILE_FUNC();
    return index;
  }

  *found = 1;
  END_PROFILE_FUNC();
  return half_index;
}

#endif
