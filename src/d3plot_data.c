/***********************************************************************************
 *                         This file is part of dynareadout
 *                    https://github.com/PucklaMotzer09/dynareadout
 ***********************************************************************************
 * Copyright (c) 2022 PucklaMotzer09
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
#include <stdlib.h>
#include <string.h>

#define CDP plot_file->control_data
#define DT_PTR_SET(index)                                                      \
  plot_file->data_pointers[index] = plot_file->buffer.cur_word

int _d3plot_read_geometry_data(d3plot_file *plot_file) {
  /* TODO: We can skip the entire GEOMETRY DATA section since we already know
   * how big it is and where to find the data*/

  if (CDP.element_connectivity_packed) {
    plot_file->error_string = malloc(45);
    sprintf(plot_file->error_string,
            "Packed Element Connectivity is not supported");
    return 0;
  }

  if (CDP.icode != D3_CODE_OLD_DYNA3D &&
      CDP.icode != D3_CODE_NIKE3D_LS_DYNA3D_LS_NIKE3D) {
    plot_file->error_string = malloc(49);
    sprintf(plot_file->error_string,
            "The given order of the elements is not supported");
    return 0;
  }

  /* Here are the node coordinates*/
  DT_PTR_SET(D3PLT_PTR_NODE_COORDS);

  d3_buffer_skip_words(&plot_file->buffer, CDP.numnp * CDP.ndim);

  if (CDP.nel8 > 0) {
    DT_PTR_SET(D3PLT_PTR_EL8_CONNECT);
    d3_buffer_skip_words(&plot_file->buffer, 9 * CDP.nel8);
  } else if (CDP.nel8 < 0) {
    const int64_t nel8 = CDP.nel8 * -1;
    d3_buffer_skip_words(&plot_file->buffer, 2 * nel8);
    /* TODO: read function for -nel8 data*/
  }

  if (CDP.nelt > 0) {
    DT_PTR_SET(D3PLT_PTR_ELT_CONNECT);
    d3_buffer_skip_words(&plot_file->buffer, 9 * CDP.nelt);
  }

  if (CDP.nel2 > 0) {
    DT_PTR_SET(D3PLT_PTR_EL2_CONNECT);
    d3_buffer_skip_words(&plot_file->buffer, 6 * CDP.nel2);
  }

  if (CDP.nel4 > 0) {
    DT_PTR_SET(D3PLT_PTR_EL4_CONNECT);
    d3_buffer_skip_words(&plot_file->buffer, 5 * CDP.nel4);
  }

  return 1;
}

int _d3plot_read_user_identification_numbers(d3plot_file *plot_file) {
  if (CDP.narbs == 0) {
    CDP.numrbs = 0;

    return 1;
  }

  int64_t nsort;
  d3_word nsortd = 0, nsrhd = 0, nsrbd = 0, nsrsd = 0, nsrtd = 0,
          nmmat = plot_file->control_data.nmmat;
  if (plot_file->buffer.word_size == 4) {
    int32_t nsort32;
    d3_buffer_read_words(&plot_file->buffer, &nsort32, 1);
    nsort = nsort32;
  } else {
    d3_buffer_read_words(&plot_file->buffer, &nsort, 1);
  }
  d3_buffer_skip_words(&plot_file->buffer, 4);
  /* TODO: Find out what NSRH, NSRB, NSRS and NSRT is for*/
  d3_buffer_read_words(&plot_file->buffer, &nsortd, 1);
  d3_buffer_read_words(&plot_file->buffer, &nsrhd, 1);
  d3_buffer_read_words(&plot_file->buffer, &nsrbd, 1);
  d3_buffer_read_words(&plot_file->buffer, &nsrsd, 1);
  d3_buffer_read_words(&plot_file->buffer, &nsrtd, 1);

  CDP.numrbs = 0;

  if (nsort < 0) {
    d3_buffer_skip_words(&plot_file->buffer, 4);
    /* TODO: Find out what NSRMA, NSRMU, NSRMP and NSRTM is for*/

    d3_buffer_read_words(&plot_file->buffer, &CDP.numrbs, 1);
    d3_buffer_read_words(&plot_file->buffer, &nmmat, 1);
  } else {
    plot_file->error_string = malloc(39 + 20);
    sprintf(plot_file->error_string, "Non negative nsort (%d) is not supported",
            nsort);
    return 0;
  }

  DT_PTR_SET(D3PLT_PTR_NODE_IDS);
  d3_buffer_skip_words(&plot_file->buffer, nsortd); /* nusern*/
  DT_PTR_SET(D3PLT_PTR_EL8_IDS);
  d3_buffer_skip_words(&plot_file->buffer, nsrhd); /* nuserh*/
  DT_PTR_SET(D3PLT_PTR_EL2_IDS);
  d3_buffer_skip_words(&plot_file->buffer, nsrbd); /* nuserb*/
  DT_PTR_SET(D3PLT_PTR_EL4_IDS);
  d3_buffer_skip_words(&plot_file->buffer, nsrsd); /* nusers*/
  DT_PTR_SET(D3PLT_PTR_EL48_IDS);
  d3_buffer_skip_words(&plot_file->buffer, nsrtd); /* nusert*/
  DT_PTR_SET(D3PLT_PTR_PART_IDS);
  d3_buffer_skip_words(&plot_file->buffer, nmmat); /* norder*/
  d3_buffer_skip_words(&plot_file->buffer, 2 * nmmat);
  /* TODO: Find out what NSRMU and NSRMP is for*/

  return 1;
}

int _d3plot_read_extra_node_connectivity(d3plot_file *plot_file) {
  if (CDP.nel8 < 0) {
    const int64_t nel8 = CDP.nel8 * -1;
    d3_buffer_skip_words(&plot_file->buffer, 2 * nel8);
    /* TODO: read function for -nel8 data*/
  }

  if (CDP.nel48 > 0) {
    d3_buffer_skip_words(&plot_file->buffer, 5 * CDP.nel48);
    /* TODO: read function for nel48 data*/
  }

  if (CDP.nel20 > 0) {
    d3_buffer_skip_words(&plot_file->buffer, 13 * CDP.nel20);
    /* TODO: read function for nel20 data*/
  }

  return 1;
}

int _d3plot_read_adapted_element_parent_list(d3plot_file *plot_file) {
  if (CDP.nadapt == 0) {
    return 1;
  }

  d3_buffer_skip_words(&plot_file->buffer, 2 * CDP.nadapt);
  /* TODO: read function for aepl*/

  return 1;
}

int _d3plot_read_header(d3plot_file *plot_file) {
  while (1) {
    d3_word ntype = 0;
    d3_buffer_read_words(&plot_file->buffer, &ntype, 1);

    if (ntype == 90000) {
      d3_buffer_skip_words(&plot_file->buffer, 18);
      /* TODO: read function for head*/

    } else if (ntype == 90001) {
      d3_word numprop = 0;
      d3_buffer_read_words(&plot_file->buffer, &numprop, 1);
      DT_PTR_SET(D3PLT_PTR_PART_TITLES);
      d3_buffer_skip_words(&plot_file->buffer, (1 + 18) * numprop);

    } else if (ntype == 90002) {
      d3_word numcon = 0;
      d3_buffer_read_words(&plot_file->buffer, &numcon, 1);
      d3_buffer_skip_words(&plot_file->buffer, (1 + 18) * numcon);
      /* TODO: read function for contact titles*/

    } else if (ntype == 900100) {
      d3_word nline = 0;
      d3_buffer_read_words(&plot_file->buffer, &nline, 1);
      d3_buffer_skip_words(&plot_file->buffer, 20 * nline);
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
        plot_file->error_string = malloc(70);
        sprintf(plot_file->error_string,
                "Here (%d) should be the EOF marker (%f != %f)\n",
                plot_file->buffer.cur_word - 1, eof_marker, D3_EOF);
        return 0;
      }

      break;
    }
  }

  return 1;
}
