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

  double vec[3];

  size_t i = 0;
  while (i < CDP.numnp) {
    d3_buffer_read_vec3(&plot_file->buffer, vec);

    i++;
  }

  if (CDP.nel8 > 0) {
    uint8_t *ix8 = malloc(9 * CDP.nel8 * plot_file->buffer.word_size);
    d3_buffer_read_words(&plot_file->buffer, ix8, 9 * CDP.nel8);

    d3_word value[9];
    uint32_t value32[9];

    size_t offset = 0;
    size_t i = 0;
    while (i < CDP.nel8) {
      if (plot_file->buffer.word_size == 4) {
        memcpy(value32, &ix8[offset], plot_file->buffer.word_size * 9);
        size_t j = 0;
        while (j < 9) {
          value[j + 0] = value32[j + 0];
          value[j + 1] = value32[j + 1];
          value[j + 2] = value32[j + 2];

          j += 3;
        }
      } else {
        memcpy(value, &ix8[offset], plot_file->buffer.word_size * 9);
      }
      offset += plot_file->buffer.word_size * 9;

      i++;
    }

    free(ix8);
  } else if (CDP.nel8 < 0) {
    const int64_t nel8 = CDP.nel8 * -1;

    uint8_t *ix10 = malloc(2 * nel8 * plot_file->buffer.word_size);
    d3_buffer_read_words(&plot_file->buffer, ix10, 2 * nel8);

    d3_word value[2];
    uint32_t value32[2];

    size_t offset = 0;
    size_t i = 0;
    while (i < nel8) {
      if (plot_file->buffer.word_size == 4) {
        memcpy(value32, &ix10[offset], plot_file->buffer.word_size * 2);
        value[0] = value32[0];
        value[1] = value32[1];
      } else {
        memcpy(value, &ix10[offset], plot_file->buffer.word_size * 2);
      }
      offset += plot_file->buffer.word_size * 2;

      i++;
    }

    free(ix10);
  }

  if (CDP.nelt > 0) {
    uint8_t *ixt = malloc(9 * CDP.nelt * plot_file->buffer.word_size);
    d3_buffer_read_words(&plot_file->buffer, ixt, 9 * CDP.nelt);

    d3_word value[9];
    uint32_t value32[9];

    size_t offset = 0;
    size_t i = 0;
    while (i < CDP.nelt) {
      if (plot_file->buffer.word_size == 4) {
        memcpy(value32, &ixt[offset], plot_file->buffer.word_size * 9);
        size_t j = 0;
        while (j < 9) {
          value[j + 0] = value32[j + 0];
          value[j + 1] = value32[j + 1];
          value[j + 2] = value32[j + 2];

          j += 3;
        }
      } else {
        memcpy(value, &ixt[offset], plot_file->buffer.word_size * 9);
      }
      offset += plot_file->buffer.word_size * 9;

      i++;
    }

    free(ixt);
  }

  if (CDP.nel2 > 0) {
    uint8_t *ix2 = malloc(6 * CDP.nel2 * plot_file->buffer.word_size);
    d3_buffer_read_words(&plot_file->buffer, ix2, 6 * CDP.nel2);

    d3_word value[6];
    uint32_t value32[6];

    size_t offset = 0;
    size_t i = 0;
    while (i < CDP.nel2) {
      if (plot_file->buffer.word_size == 4) {
        memcpy(value32, &ix2[offset], plot_file->buffer.word_size * 6);
        size_t j = 0;
        while (j < 6) {
          value[j + 0] = value32[j + 0];
          value[j + 1] = value32[j + 1];
          value[j + 2] = value32[j + 2];

          j += 3;
        }
      } else {
        memcpy(value, &ix2[offset], plot_file->buffer.word_size * 6);
      }
      offset += plot_file->buffer.word_size * 6;

      i++;
    }

    free(ix2);
  }

  if (CDP.nel4 > 0) {
    uint8_t *ix4 = malloc(5 * CDP.nel4 * plot_file->buffer.word_size);
    d3_buffer_read_words(&plot_file->buffer, ix4, 5 * CDP.nel4);

    d3_word value[5];
    uint32_t value32[5];

    size_t offset = 0;
    size_t i = 0;
    while (i < CDP.nel4) {
      if (plot_file->buffer.word_size == 4) {
        memcpy(value32, &ix4[offset], plot_file->buffer.word_size * 5);
        size_t j = 0;
        while (j < 5) {
          value[j + 0] = value32[j + 0];

          j++;
        }
      } else {
        memcpy(value, &ix4[offset], plot_file->buffer.word_size * 5);
      }
      offset += plot_file->buffer.word_size * 5;

      i++;
    }

    free(ix4);
  }

  return 1;
}

int _d3plot_read_extra_node_connectivity(d3plot_file *plot_file) {
  if (CDP.nel8 < 0) {
    const int64_t nel8 = CDP.nel8 * -1;

    uint8_t *ix10 = malloc(2 * nel8 * plot_file->buffer.word_size);
    d3_buffer_read_words(&plot_file->buffer, ix10, 2 * nel8);

    d3_word value[2];
    uint32_t value32[2];

    size_t offset = 0;
    size_t i = 0;
    while (i < nel8) {
      if (plot_file->buffer.word_size == 4) {
        memcpy(value32, &ix10[offset], plot_file->buffer.word_size * 2);
        value[0] = value32[0];
        value[1] = value32[1];
      } else {
        memcpy(value, &ix10[offset], plot_file->buffer.word_size * 2);
      }
      offset += plot_file->buffer.word_size * 2;

      i++;
    }

    free(ix10);
  }

  if (CDP.nel48 > 0) {
    uint8_t *ix48 = malloc(5 * CDP.nel48 * plot_file->buffer.word_size);
    d3_buffer_read_words(&plot_file->buffer, ix48, 5 * CDP.nel48);

    d3_word value[5];
    uint32_t value32[5];

    size_t offset = 0;
    size_t i = 0;
    while (i < CDP.nel48) {
      if (plot_file->buffer.word_size == 4) {
        memcpy(value32, &ix48[offset], plot_file->buffer.word_size * 5);
        value[0] = value32[0];
        value[1] = value32[1];
        value[2] = value32[2];
        value[3] = value32[3];
        value[4] = value32[4];
      } else {
        memcpy(value, &ix48[offset], plot_file->buffer.word_size * 5);
      }
      offset += plot_file->buffer.word_size * 5;

      i++;
    }

    free(ix48);
  }

  if (CDP.extra > 0 && CDP.nel20 > 0) {
    uint8_t *ix20 = malloc(13 * CDP.nel20 * plot_file->buffer.word_size);
    d3_buffer_read_words(&plot_file->buffer, ix20, 13 * CDP.nel20);

    d3_word value[13];
    uint32_t value32[13];

    size_t offset = 0;
    size_t i = 0;
    while (i < CDP.nel20) {
      if (plot_file->buffer.word_size == 4) {
        memcpy(value32, &ix20[offset], plot_file->buffer.word_size * 13);
        size_t j = 0;
        while (j < 13) {
          value[j] = value32[j];

          j++;
        }
      } else {
        memcpy(value, &ix20[offset], plot_file->buffer.word_size * 13);
      }
      offset += plot_file->buffer.word_size * 13;

      i++;
    }

    free(ix20);
  }

  return 1;
}

int _d3plot_read_adapted_element_parent_list(d3plot_file *plot_file) {
  if (CDP.nadapt == 0) {
    return 1;
  }

  uint8_t *aepl = malloc(2 * CDP.nadapt * plot_file->buffer.word_size);
  d3_buffer_read_words(&plot_file->buffer, aepl, 2 * CDP.nadapt);

  d3_word value[2];
  uint32_t value32[2];

  size_t offset = 0;
  size_t i = 0;
  while (i < CDP.nadapt) {
    if (plot_file->buffer.word_size == 4) {
      memcpy(value32, &aepl[offset], 2 * plot_file->buffer.word_size);
      value[0] = value32[0];
      value[1] = value32[1];
    } else {
      memcpy(value, &aepl[offset], 2 * plot_file->buffer.word_size);
    }
    offset += 2 * plot_file->buffer.word_size;

    i++;
  }

  free(aepl);

  return 1;
}

int _d3plot_read_header(d3plot_file *plot_file) {

  while (1) {
    d3_word ntype = 0;
    d3_buffer_read_words(&plot_file->buffer, &ntype, 1);

    if (ntype == 90001) {
      d3_word numprop = 0;
      d3_buffer_read_words(&plot_file->buffer, &numprop, 1);
      size_t i = 0;
      while (i < numprop) {
        d3_word idp = 0;
        d3_buffer_read_words(&plot_file->buffer, &idp, 1);
        char *ptitle = malloc(18 * plot_file->buffer.word_size + 1);
        d3_buffer_read_words(&plot_file->buffer, ptitle, 18);
        ptitle[18 * plot_file->buffer.word_size] = '\0';

        free(ptitle);

        i++;
      }
    } else if (ntype == 90000) {
      plot_file->header.head = malloc(18 * plot_file->buffer.word_size + 1);
      d3_buffer_read_words(&plot_file->buffer, plot_file->header.head, 18);
      plot_file->header.head[18 * plot_file->buffer.word_size] = '\0';

    } else if (ntype == 90002) {
      d3_word numcon = 0;
      d3_buffer_read_words(&plot_file->buffer, &numcon, 1);

      size_t i = 0;
      while (i < numcon) {
        d3_word idc = 0;
        d3_buffer_read_words(&plot_file->buffer, &idc, 1);

        char *ctitle = malloc(18 * plot_file->buffer.word_size);
        d3_buffer_read_words(&plot_file->buffer, ctitle, 18);
        ctitle[18 * plot_file->buffer.word_size] = '\0';

        free(ctitle);

        i++;
      }
    } else if (ntype == 900100) {
      d3_word nline = 0;
      d3_buffer_read_words(&plot_file->buffer, &nline, 1);

      size_t i = 0;
      while (i < nline) {
        char *keyword = malloc(20 * plot_file->buffer.word_size + 1);
        d3_buffer_read_words(&plot_file->buffer, keyword, 20);
        keyword[20 * plot_file->buffer.word_size] = '\0';

        free(keyword);

        i++;
      }
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

int _d3plot_read_user_identification_numbers(d3plot_file *plot_file) {
  if (CDP.narbs == 0) {
    CDP.numrbs = 0;

    return 1;
  }

  int64_t nsort;
  d3_word nsrh = 0, nsrb = 0, nsrs = 0, nsrt = 0, nsortd = 0, nsrhd = 0,
          nsrbd = 0, nsrsd = 0, nsrtd = 0,
          nmmat = plot_file->control_data.nmmat;
  if (plot_file->buffer.word_size == 4) {
    int32_t nsort32;
    d3_buffer_read_words(&plot_file->buffer, &nsort32, 1);
    nsort = nsort32;
  } else {
    d3_buffer_read_words(&plot_file->buffer, &nsort, 1);
  }
  d3_buffer_read_words(&plot_file->buffer, &nsrh, 1);
  d3_buffer_read_words(&plot_file->buffer, &nsrb, 1);
  d3_buffer_read_words(&plot_file->buffer, &nsrs, 1);
  d3_buffer_read_words(&plot_file->buffer, &nsrt, 1);
  d3_buffer_read_words(&plot_file->buffer, &nsortd, 1);
  d3_buffer_read_words(&plot_file->buffer, &nsrhd, 1);
  d3_buffer_read_words(&plot_file->buffer, &nsrbd, 1);
  d3_buffer_read_words(&plot_file->buffer, &nsrsd, 1);
  d3_buffer_read_words(&plot_file->buffer, &nsrtd, 1);

  CDP.numrbs = 0;

  if (nsort < 0) {
    d3_word nsrma = 0, nsrmu = 0, nsrmp = 0, nsrtm = 0;

    d3_buffer_read_words(&plot_file->buffer, &nsrma, 1);
    d3_buffer_read_words(&plot_file->buffer, &nsrmu, 1);
    d3_buffer_read_words(&plot_file->buffer, &nsrmp, 1);
    d3_buffer_read_words(&plot_file->buffer, &nsrtm, 1);
    d3_buffer_read_words(&plot_file->buffer, &CDP.numrbs, 1);
    d3_buffer_read_words(&plot_file->buffer, &nmmat, 1);
  } else {
    plot_file->error_string = malloc(39 + 20);
    sprintf(plot_file->error_string, "Non negative nsort (%d) is not supported",
            nsort);
    return 0;
  }

  uint8_t *nusern, *nuserh, *nuserb, *nusers, *nusert, *norder, *nsrmu_a,
      *nsrmp_a;

  nusern = malloc(nsortd * plot_file->buffer.word_size);
  nuserh = malloc(nsrhd * plot_file->buffer.word_size);
  nuserb = malloc(nsrbd * plot_file->buffer.word_size);
  nusers = malloc(nsrsd * plot_file->buffer.word_size);
  nusert = malloc(nsrtd * plot_file->buffer.word_size);
  norder = malloc(nmmat * plot_file->buffer.word_size);
  nsrmu_a = malloc(nmmat * plot_file->buffer.word_size);
  nsrmp_a = malloc(nmmat * plot_file->buffer.word_size);

  DT_PTR_SET(D3PLT_PTR_NODE_IDS);

  d3_buffer_read_words(&plot_file->buffer, nusern, nsortd);
  d3_buffer_read_words(&plot_file->buffer, nuserh, nsrhd);
  d3_buffer_read_words(&plot_file->buffer, nuserb, nsrbd);
  d3_buffer_read_words(&plot_file->buffer, nusers, nsrsd);
  d3_buffer_read_words(&plot_file->buffer, nusert, nsrtd);
  d3_buffer_read_words(&plot_file->buffer, norder, nmmat);
  d3_buffer_read_words(&plot_file->buffer, nsrmu_a, nmmat);
  d3_buffer_read_words(&plot_file->buffer, nsrmp_a, nmmat);

  free(nusern);
  free(nuserh);
  free(nuserb);
  free(nusers);
  free(nusert);
  free(norder);
  free(nsrmu_a);
  free(nsrmp_a);

  return 1;
}
