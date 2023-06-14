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

#include "d3plot.h"
#include "profiling.h"
#include <stdlib.h>
#include <string.h>

#ifndef CDP
#define CDP plot_file->control_data
#endif
#ifdef DT_PTR_SET
#undef DT_PTR_SET
#endif
#define DT_PTR_SET(index) plot_file->data_pointers[index] = d3_ptr->cur_word
#define DT_PTR_SET_DPTR(index) plot_file->data_pointers[index] = data_pointer

#include "d3plot_error_macros.h"

int _d3plot_read_geometry_data(d3plot_file *plot_file, d3_pointer *d3_ptr) {
  BEGIN_PROFILE_FUNC();

  /* We can skip the entire GEOMETRY DATA section since we already know
   * how big it is and where to find the data*/
  const size_t geometry_start_word = d3_ptr->cur_word;
  size_t data_pointer = geometry_start_word;

  if (CDP.element_connectivity_packed) {
    ERROR_AND_NO_RETURN_PTR("Packed Element Connectivity is not supported");
    END_PROFILE_FUNC();
    return 0;
  }

  /* Here are the node coordinates*/
  DT_PTR_SET_DPTR(D3PLT_PTR_NODE_COORDS);

  data_pointer += CDP.numnp * CDP.ndim;

  DT_PTR_SET_DPTR(D3PLT_PTR_EL8_CONNECT);
  int64_t nel8;
  if (CDP.nel8 > 0) {
    nel8 = CDP.nel8;
  } else {
    nel8 = -CDP.nel8;
    data_pointer += 2 * nel8;
    /* TODO: read function for -nel8 data*/
  }
  data_pointer += 9 * nel8;

  if (CDP.nelt > 0) {
    DT_PTR_SET_DPTR(D3PLT_PTR_ELT_CONNECT);
    data_pointer += 9 * CDP.nelt;
  }

  if (CDP.nel2 > 0) {
    DT_PTR_SET_DPTR(D3PLT_PTR_EL2_CONNECT);
    data_pointer += 6 * CDP.nel2;
  }

  if (CDP.nel4 > 0) {
    DT_PTR_SET_DPTR(D3PLT_PTR_EL4_CONNECT);
    data_pointer += 5 * CDP.nel4;
  }

  /* Skip the entire geometry section*/
  d3_buffer_skip_words(&plot_file->buffer, d3_ptr,
                       data_pointer - geometry_start_word);

  if (plot_file->buffer.error_string) {
    ERROR_AND_NO_RETURN_F_PTR("Failed to skip words: %s",
                              plot_file->buffer.error_string);
    END_PROFILE_FUNC();
    return 0;
  }

  END_PROFILE_FUNC();
  return 1;
}

int _d3plot_read_user_identification_numbers(d3plot_file *plot_file,
                                             d3_pointer *d3_ptr) {
  BEGIN_PROFILE_FUNC();

  if (CDP.narbs == 0) {
    CDP.numrbs = 0;

    END_PROFILE_FUNC();
    return 1;
  }

  const size_t user_ids_start = d3_ptr->cur_word;

  int64_t nsort;
  d3_word nsortd = 0, nsrhd = 0, nsrbd = 0, nsrsd = 0, nsrtd = 0,
          nmmat = plot_file->control_data.nmmat;
  if (plot_file->buffer.word_size == 4) {
    int32_t nsort32;
    d3_buffer_read_words(&plot_file->buffer, d3_ptr, &nsort32, 1);
    nsort = nsort32;
  } else {
    d3_buffer_read_words(&plot_file->buffer, d3_ptr, &nsort, 1);
  }
  if (plot_file->buffer.error_string) {
    ERROR_AND_NO_RETURN_F_PTR("Failed to read NSORT: %s",
                              plot_file->buffer.error_string);

    END_PROFILE_FUNC();
    return 0;
  }

  d3_buffer_skip_words(&plot_file->buffer, d3_ptr, 4);
  /* TODO: Find out what NSRH, NSRB, NSRS and NSRT is for*/
  d3_buffer_read_words(&plot_file->buffer, d3_ptr, &nsortd, 1);
  d3_buffer_read_words(&plot_file->buffer, d3_ptr, &nsrhd, 1);
  d3_buffer_read_words(&plot_file->buffer, d3_ptr, &nsrbd, 1);
  d3_buffer_read_words(&plot_file->buffer, d3_ptr, &nsrsd, 1);
  d3_buffer_read_words(&plot_file->buffer, d3_ptr, &nsrtd, 1);

  if (plot_file->buffer.error_string) {
    ERROR_AND_NO_RETURN_F_PTR(
        "Failed to read NSORTD, NSRHD, NSRBD, NSRSD and NSRTD: %s",
        plot_file->buffer.error_string);

    END_PROFILE_FUNC();
    return 0;
  }

  CDP.numrbs = 0;

  if (nsort < 0) {
    d3_buffer_skip_words(&plot_file->buffer, d3_ptr, 4);
    /* TODO: Find out what NSRMA, NSRMU, NSRMP and NSRTM is for*/
    d3_buffer_read_words(&plot_file->buffer, d3_ptr, &CDP.numrbs, 1);
    d3_buffer_read_words(&plot_file->buffer, d3_ptr, &nmmat, 1);

    if (plot_file->buffer.error_string) {
      ERROR_AND_NO_RETURN_F_PTR("Failed to read NUMRBS and NMMAT: %s",
                                plot_file->buffer.error_string);

      END_PROFILE_FUNC();
      return 0;
    }
  }

  const size_t data_pointer_start = d3_ptr->cur_word;
  size_t data_pointer = d3_ptr->cur_word;
  DT_PTR_SET_DPTR(D3PLT_PTR_NODE_IDS);
  data_pointer += nsortd; /* nusern*/
  DT_PTR_SET_DPTR(D3PLT_PTR_EL8_IDS);
  data_pointer += nsrhd; /* nuserh*/
  DT_PTR_SET_DPTR(D3PLT_PTR_EL2_IDS);
  data_pointer += nsrbd; /* nuserb*/
  DT_PTR_SET_DPTR(D3PLT_PTR_EL4_IDS);
  data_pointer += nsrsd; /* nusers*/
  DT_PTR_SET_DPTR(D3PLT_PTR_ELT_IDS);
  data_pointer += nsrtd; /* nusert*/
  if (nsort < 0) {
    DT_PTR_SET_DPTR(D3PLT_PTR_PART_IDS);
    data_pointer += 3 * nmmat; /* norder + ?*/
    /* TODO: Find out what NSRMU and NSRMP is for*/
  } else {
    /* These values are not used when nsort >= 0*/
    data_pointer += 3 * nmmat;
  }

  /* Skip multiple values at once*/
  d3_buffer_skip_words(&plot_file->buffer, d3_ptr,
                       data_pointer - data_pointer_start);

  if (plot_file->buffer.error_string) {
    ERROR_AND_NO_RETURN_F_PTR("Failed to skip words: %s",
                              plot_file->buffer.error_string);

    END_PROFILE_FUNC();
    return 0;
  }

  const size_t user_ids_end = d3_ptr->cur_word;
  const size_t user_ids_size = user_ids_end - user_ids_start;
  if (user_ids_size != CDP.narbs) {
    ERROR_AND_NO_RETURN_F_PTR(
        "The USER IDENTIFICATION NUMBERS section is false (%zu != %llu)",
        user_ids_size, CDP.narbs);

    END_PROFILE_FUNC();
    return 0;
  }

  END_PROFILE_FUNC();
  return 1;
}

int _d3plot_read_extra_node_connectivity(d3plot_file *plot_file,
                                         d3_pointer *d3_ptr) {
  BEGIN_PROFILE_FUNC();

  const size_t data_pointer_start = d3_ptr->cur_word;
  size_t data_pointer = data_pointer_start;
  if (CDP.nel8 < 0) {
    const int64_t nel8 = CDP.nel8 * -1;
    data_pointer += 2 * nel8;
    /* TODO: read function for -nel8 data*/
  }

  if (CDP.nel48 > 0) {
    data_pointer += 5 * CDP.nel48;
    /* TODO: read function for nel48 data*/
  }

  if (CDP.nel20 > 0) {
    data_pointer += 13 * CDP.nel20;
    /* TODO: read function for nel20 data*/
  }

  /* Skip everything at once*/
  d3_buffer_skip_words(&plot_file->buffer, d3_ptr,
                       data_pointer - data_pointer_start);

  if (plot_file->buffer.error_string) {
    ERROR_AND_NO_RETURN_F_PTR("Failed to skip words: %s",
                              plot_file->buffer.error_string);

    END_PROFILE_FUNC();
    return 0;
  }

  END_PROFILE_FUNC();
  return 1;
}

int _d3plot_read_adapted_element_parent_list(d3plot_file *plot_file,
                                             d3_pointer *d3_ptr) {
  BEGIN_PROFILE_FUNC();

  if (CDP.nadapt == 0) {
    END_PROFILE_FUNC();
    return 1;
  }

  d3_buffer_skip_words(&plot_file->buffer, d3_ptr, 2 * CDP.nadapt);
  /* TODO: read function for aepl*/

  if (plot_file->buffer.error_string) {
    ERROR_AND_NO_RETURN_F_PTR("Failed to skip words: %s",
                              plot_file->buffer.error_string);
    END_PROFILE_FUNC();
    return 0;
  }

  END_PROFILE_FUNC();
  return 1;
}

int _d3plot_read_header(d3plot_file *plot_file, d3_pointer *d3_ptr) {
  BEGIN_PROFILE_FUNC();

  while (1) {
    d3_word ntype = 0;
    d3_buffer_read_words(&plot_file->buffer, d3_ptr, &ntype, 1);
    if (plot_file->buffer.error_string) {
      ERROR_AND_NO_RETURN_F_PTR("Failed to read NTYPE: %s",
                                plot_file->buffer.error_string);
      END_PROFILE_FUNC();
      return 0;
    }

    if (ntype == 90000) {
      /* HEAD is always 72 bytes*/
      d3_buffer_skip_bytes(&plot_file->buffer, d3_ptr, 72);
      /* TODO: read function for head*/
      if (plot_file->buffer.error_string) {
        ERROR_AND_NO_RETURN_F_PTR("Failed to skip words: %s",
                                  plot_file->buffer.error_string);
        END_PROFILE_FUNC();
        return 0;
      }

    } else if (ntype == 90001) {
      d3_word numprop = 0;
      d3_buffer_read_words(&plot_file->buffer, d3_ptr, &numprop, 1);
      if (plot_file->buffer.error_string) {
        ERROR_AND_NO_RETURN_F_PTR("Failed to read NUMPROP: %s",
                                  plot_file->buffer.error_string);
        END_PROFILE_FUNC();
        return 0;
      }
      DT_PTR_SET(D3PLT_PTR_PART_TITLES);
      /* PTITLE is always 72 bytes*/
      d3_buffer_skip_bytes(&plot_file->buffer, d3_ptr,
                           (1 * plot_file->buffer.word_size + 72) * numprop);
      if (plot_file->buffer.error_string) {
        ERROR_AND_NO_RETURN_F_PTR("Failed to skip words: %s",
                                  plot_file->buffer.error_string);
        END_PROFILE_FUNC();
        return 0;
      }
    } else if (ntype == 90002) {
      d3_word numcon = 0;
      d3_buffer_read_words(&plot_file->buffer, d3_ptr, &numcon, 1);
      if (plot_file->buffer.error_string) {
        ERROR_AND_NO_RETURN_F_PTR("Failed to read NUMCON: %s",
                                  plot_file->buffer.error_string);
        END_PROFILE_FUNC();
        return 0;
      }
      /* CTITLE is always 72 bytes*/
      d3_buffer_skip_bytes(&plot_file->buffer, d3_ptr,
                           (1 * plot_file->buffer.word_size + 72) * numcon);
      if (plot_file->buffer.error_string) {
        ERROR_AND_NO_RETURN_F_PTR("Failed to skip words: %s",
                                  plot_file->buffer.error_string);
        END_PROFILE_FUNC();
        return 0;
      }
      /* TODO: read function for contact titles*/

    } else if (ntype == 900100) {
      d3_word nline = 0;
      d3_buffer_read_words(&plot_file->buffer, d3_ptr, &nline, 1);
      if (plot_file->buffer.error_string) {
        ERROR_AND_NO_RETURN_F_PTR("Failed to read NLINE: %s",
                                  plot_file->buffer.error_string);
        END_PROFILE_FUNC();
        return 0;
      }
      /* KEYWORD is always 80 bytes*/
      d3_buffer_skip_bytes(&plot_file->buffer, d3_ptr, 80 * nline);
      if (plot_file->buffer.error_string) {
        ERROR_AND_NO_RETURN_F_PTR("Failed to skip words: %s",
                                  plot_file->buffer.error_string);
        END_PROFILE_FUNC();
        return 0;
      }
      /* TODO: read function for extra keyword lines?*/

    } else {
      double eof_marker;
      if (plot_file->buffer.word_size == 4) {
        float eof_marker32;
        memcpy(&eof_marker32, &ntype, plot_file->buffer.word_size);
        eof_marker = eof_marker32;
      } else {
        memcpy(&eof_marker, &ntype, plot_file->buffer.word_size);
      }

      if (eof_marker != D3_EOF) {
        ERROR_AND_NO_RETURN_F_PTR(
            "Here (after header) 'd3plot':(%zu) should be "
            "the EOF marker (%f != %f)",
            d3_ptr->cur_word - 1, eof_marker, D3_EOF);
        END_PROFILE_FUNC();
        return 0;
      }

      break;
    }
  }

  END_PROFILE_FUNC();
  return 1;
}
