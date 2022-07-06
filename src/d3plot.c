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

#define READ_CONTROL_DATA_PLOT_FILE_WORD(value)                                \
  plot_file.control_data.value = 0;                                            \
  d3_buffer_read_words(&plot_file.buffer, &plot_file.control_data.value, 1)
#define READ_CONTROL_DATA_WORD(value)                                          \
  d3_word value = 0;                                                           \
  d3_buffer_read_words(&plot_file.buffer, &value, 1)
#define CDATA plot_file.control_data
#define CDATAP plot_file->control_data

d3plot_file d3plot_open(const char *root_file_name) {
  d3plot_file plot_file;
  CDATA.title = NULL;
  plot_file.error_string = NULL;

  plot_file.buffer = d3_buffer_open(root_file_name);
  if (plot_file.buffer.error_string) {
    plot_file.error_string = malloc(strlen(plot_file.buffer.error_string) + 1);
    memcpy(plot_file.error_string, plot_file.buffer.error_string,
           strlen(plot_file.buffer.error_string) + 1);
    return plot_file;
  }

  /* Read Title*/
  CDATA.title = malloc(10 * plot_file.buffer.word_size + 1);
  d3_buffer_read_words(&plot_file.buffer, CDATA.title, 10);
  CDATA.title[10 * plot_file.buffer.word_size] = '\0';

  printf("Title: %s\n", CDATA.title);

  READ_CONTROL_DATA_PLOT_FILE_WORD(run_time);

  printf("Runtime: %d\n", CDATA.run_time);

  READ_CONTROL_DATA_WORD(file_type);
  if (file_type > 1000) {
    file_type -= 1000;
    /* TODO: all external(users) numbers (Node, Element, Material and Rigid
  Surface Nodes) will be written in I8 format.*/
  }
  if (file_type != D3_FILE_TYPE_D3PLOT) {
    plot_file.error_string = malloc(50);
    sprintf(plot_file.error_string, "Wrong file type: %s",
            _d3plot_get_file_type_name(file_type));
    return plot_file;
  }

  READ_CONTROL_DATA_PLOT_FILE_WORD(source_version);
  READ_CONTROL_DATA_PLOT_FILE_WORD(release_version);
  READ_CONTROL_DATA_PLOT_FILE_WORD(version);
  READ_CONTROL_DATA_PLOT_FILE_WORD(ndim);
  READ_CONTROL_DATA_PLOT_FILE_WORD(numnp);
  READ_CONTROL_DATA_PLOT_FILE_WORD(icode);
  READ_CONTROL_DATA_PLOT_FILE_WORD(nglbv);
  READ_CONTROL_DATA_PLOT_FILE_WORD(it);
  READ_CONTROL_DATA_PLOT_FILE_WORD(iu);
  READ_CONTROL_DATA_PLOT_FILE_WORD(iv);
  READ_CONTROL_DATA_PLOT_FILE_WORD(ia);
  READ_CONTROL_DATA_PLOT_FILE_WORD(nel8);
  READ_CONTROL_DATA_PLOT_FILE_WORD(nummat8);
  READ_CONTROL_DATA_PLOT_FILE_WORD(numds);
  READ_CONTROL_DATA_PLOT_FILE_WORD(numst);
  READ_CONTROL_DATA_PLOT_FILE_WORD(nv3d);
  READ_CONTROL_DATA_PLOT_FILE_WORD(nel2);
  READ_CONTROL_DATA_PLOT_FILE_WORD(nummat2);
  READ_CONTROL_DATA_PLOT_FILE_WORD(nv1d);
  READ_CONTROL_DATA_PLOT_FILE_WORD(nel4);
  READ_CONTROL_DATA_PLOT_FILE_WORD(nummat4);
  READ_CONTROL_DATA_PLOT_FILE_WORD(nv2d);
  READ_CONTROL_DATA_PLOT_FILE_WORD(neiph);
  READ_CONTROL_DATA_PLOT_FILE_WORD(neips);

  if (plot_file.buffer.word_size == 4) {
    int32_t maxint32;
    d3_buffer_read_words(&plot_file.buffer, &maxint32, 1);
    CDATA.maxint = maxint32;
  } else {
    d3_buffer_read_words(&plot_file.buffer, &CDATA.maxint, 1);
  }

  /*READ_CONTROL_DATA_PLOT_FILE_WORD(edlopt); Not used in LS-Dyna?*/
  READ_CONTROL_DATA_PLOT_FILE_WORD(nmsph);
  READ_CONTROL_DATA_PLOT_FILE_WORD(ngpsph);
  READ_CONTROL_DATA_PLOT_FILE_WORD(narbs);
  READ_CONTROL_DATA_PLOT_FILE_WORD(nelt);
  READ_CONTROL_DATA_PLOT_FILE_WORD(nummatt);
  READ_CONTROL_DATA_PLOT_FILE_WORD(nv3dt);
  READ_CONTROL_DATA_PLOT_FILE_WORD(ioshl[0]);
  READ_CONTROL_DATA_PLOT_FILE_WORD(ioshl[1]);
  READ_CONTROL_DATA_PLOT_FILE_WORD(ioshl[2]);
  READ_CONTROL_DATA_PLOT_FILE_WORD(ioshl[3]);
  READ_CONTROL_DATA_PLOT_FILE_WORD(ialemat);
  READ_CONTROL_DATA_PLOT_FILE_WORD(ncfdv1);
  READ_CONTROL_DATA_PLOT_FILE_WORD(ncfdv2);
  READ_CONTROL_DATA_PLOT_FILE_WORD(nadapt);
  READ_CONTROL_DATA_PLOT_FILE_WORD(nmmat);
  READ_CONTROL_DATA_PLOT_FILE_WORD(numfluid);
  READ_CONTROL_DATA_PLOT_FILE_WORD(inn);
  READ_CONTROL_DATA_PLOT_FILE_WORD(npefg);
  READ_CONTROL_DATA_PLOT_FILE_WORD(nel48);
  READ_CONTROL_DATA_PLOT_FILE_WORD(idtdt);
  READ_CONTROL_DATA_PLOT_FILE_WORD(extra);
  READ_CONTROL_DATA_PLOT_FILE_WORD(words[0]);
  READ_CONTROL_DATA_PLOT_FILE_WORD(words[1]);
  READ_CONTROL_DATA_PLOT_FILE_WORD(words[2]);
  READ_CONTROL_DATA_PLOT_FILE_WORD(words[3]);
  READ_CONTROL_DATA_PLOT_FILE_WORD(words[4]);
  READ_CONTROL_DATA_PLOT_FILE_WORD(words[5]);

  printf("Done with CONTROL DATA at %d\n", plot_file.buffer.cur_word);

  if (CDATA.extra > 0) {
    READ_CONTROL_DATA_PLOT_FILE_WORD(nel20);
    READ_CONTROL_DATA_PLOT_FILE_WORD(nt3d);
  } else {
    CDATA.nel20 = 0;
    CDATA.nt3d = 0;
  }

  if (CDATA.ndim == 5 || CDATA.ndim == 7) {
    CDATA.mattyp = 1;
    CDATA.ndim = 3;
  } else {
    CDATA.mattyp = 0;
    if (CDATA.ndim == 3) {
      CDATA.element_connectivity_packed = 1;
    } else {
      CDATA.element_connectivity_packed = 0;
      if (CDATA.ndim == 4) {
        CDATA.ndim = 3;
      }
    }
  }

  size_t i = 0;
  while (i < 4) {
    CDATA.ioshl[i] -= 999 * (CDATA.ioshl[i] == 1000);

    i++;
  }

  if (_get_nth_digit(CDATA.idtdt, 0) == 1) {
    /* TODO: An array of dT/dt values of
             length NUMNP. Array is
             written after node temperature
             arrays.*/
  }
  if (_get_nth_digit(CDATA.idtdt, 1) == 1) {
    /* TODO: An array of residual forces of
             length 3*NUMNP followed by
             residual moments of length
             3*NUMNP. This data is written
             after node temperatures or
             dT/dt values if there are output.*/
  }
  if (_get_nth_digit(CDATA.idtdt, 2) == 1) {
    /* TODO: Plastic strain tensor is written
             for each solid and shell after
             standard element data. For
             solids (6 values) and for shells
             (6 x 3 = 18 values), at the
             lower, middle and upper
             integration location.*/
    CDATA.plastic_strain_tensor_written = 1;
  } else {
    CDATA.plastic_strain_tensor_written = 0;
  }
  if (_get_nth_digit(CDATA.idtdt, 3) == 1) {
    /* TODO: Thermal strain tensor is written
             after standard element data. For
             solid (6 values) and shell (6
             values) and after the plastic
             strain tensor if output.*/
    CDATA.thermal_strain_tensor_written = 1;
  } else {
    CDATA.thermal_strain_tensor_written = 0;
  }

  if (CDATA.plastic_strain_tensor_written ||
      CDATA.thermal_strain_tensor_written) {
    CDATA.istrn = _get_nth_digit(CDATA.idtdt, 4);
  }

  if (CDATA.maxint >= 0) {
    CDATA.mdlopt = 0;
  } else if (CDATA.maxint < -10000) {
    CDATA.mdlopt = 2;
    CDATA.maxint = CDATA.maxint * -1 - 10000;
  } else if (CDATA.maxint < 0) {
    CDATA.mdlopt = 1;
    CDATA.maxint *= -1;
  } else {
    plot_file.error_string = malloc(40);
    sprintf(plot_file.error_string, "Invalid value for MAXINT: %d",
            CDATA.maxint);
    return plot_file;
  }

  if (CDATA.idtdt < 100) {
    /* We need to compute istrn*/
    /*ISTRN can only be computed as follows and if NV2D > 0.
      If NV2D-MAXINT*(6*IOSHL(1)+IOSHL(2)+NEIPS)+8*IOSHL(3)+4*IOSHL(4) > 1
      Then ISTRN = 1, else ISTRN = 0

      If ISTRN=1, and NEIPH>=6, last the 6 additional values are the six strain
      components.

      Or NELT > 0
      If NV3DT-MAXINT*(6*IOSHL(1)+IOSHL(2)+NEIPS) > 1
      Then ISTRN = 1, else ISTRN = 0*/
    const d3_word rhs =
        CDATA.maxint * (6 * CDATA.ioshl[0] + CDATA.ioshl[1] + CDATA.neips) +
        8 * CDATA.ioshl[2] + 4 * CDATA.ioshl[3];
    if (CDATA.nv2d > rhs + 1) {
      CDATA.istrn = 1;
    } else {
      CDATA.istrn = 0;
    }

    if (CDATA.istrn == 1 && CDATA.neiph >= 6) {
      /* TODO: last the 6 additional values are the six strain*/
    }

    if (CDATA.nelt > 0) {
      if ((CDATA.nv3dt - CDATA.maxint * (6 * CDATA.ioshl[0] + CDATA.ioshl[1] +
                                         CDATA.neips)) > 1) {
        CDATA.istrn = 1;
      } else {
        CDATA.istrn = 0;
      }
    }
  }

  char release_version_str[5];
  memcpy(release_version_str, &CDATA.release_version, 4);
  release_version_str[4] = '\0';
  printf("Release number: %s\n", release_version_str);

  /* We are done with CONTROL DATA now comes the real data*/

  if (CDATA.mattyp) {
    plot_file.error_string = malloc(38);
    sprintf(plot_file.error_string, "MATERIAL TYPE DATA is not implemented");
    return plot_file;
  }
  if (CDATA.ialemat) {
    plot_file.error_string = malloc(42);
    sprintf(plot_file.error_string,
            "FLUID MATERIAL ID DATA is not implemented");
    return plot_file;
  }
  if (CDATA.nmsph) {
    plot_file.error_string = malloc(68);
    sprintf(
        plot_file.error_string,
        "SMOOTH PARTICLE HYDRODYNAMICS ELEMENT DATA FLAGS is not implemented");
    return plot_file;
  }
  if (CDATA.npefg) {
    plot_file.error_string = malloc(33);
    sprintf(plot_file.error_string, "PARTICLE DATA is not implemented");
    return plot_file;
  }

  if (!_d3plot_read_geometry_data(&plot_file)) {
    return plot_file;
  }

  if (!_d3plot_read_user_identification_numbers(&plot_file)) {
    return plot_file;
  }

  if (!_d3plot_read_extra_node_connectivity(&plot_file)) {
    return plot_file;
  }

  if (!_d3plot_read_adapted_element_parent_list(&plot_file)) {
    return plot_file;
  }

  if (CDATA.nmsph > 0) {
    plot_file.error_string = malloc(72);
    sprintf(plot_file.error_string, "SMOOTH PARTICLE HYDRODYNAMICS NODE AND "
                                    "MATERIAL LIST is not implemented");
    return plot_file;
  }

  if (CDATA.npefg > 0) {
    plot_file.error_string = malloc(42);
    sprintf(plot_file.error_string,
            "PARTICLE GEOMETRY DATA is not implemented");
    return plot_file;
  }

  if (CDATA.ndim > 5) {
    plot_file.error_string = malloc(43);
    sprintf(plot_file.error_string,
            "RIGID ROAD SURFACE DATA is not implemented");
    return plot_file;
  }

  /* Read EOF marker (D3_EOF)*/
  double eof_marker;
  d3_buffer_read_double_word(&plot_file.buffer, &eof_marker);

  if (eof_marker != D3_EOF) {
    plot_file.error_string = malloc(30);
    sprintf(plot_file.error_string, "Here should be the EOF marker");
    return plot_file;
  }

  if (!_d3plot_read_header(&plot_file)) {
    return plot_file;
  }

  if (CDATA.ncfdv1 == 67108864) {
    plot_file.error_string = malloc(36);
    sprintf(plot_file.error_string, "EXTRA DATA TYPES is not implemented");
    return plot_file;
  }

  d3_buffer_next_file(&plot_file.buffer);

  /* Here comes the STATE DATA*/

  /* Limit to 50 iterations to stop it from infinitely looping and creating a
   * lot of data*/
  size_t state_count = 0;
  while (state_count < 3 && _d3plot_read_state_data(&plot_file)) {
    state_count++;
  }

  return plot_file;
}

void d3plot_close(d3plot_file *plot_file) {
  d3_buffer_close(&plot_file->buffer);

  free(plot_file->control_data.title);
  free(plot_file->header.head);
  free(plot_file->error_string);

  plot_file->control_data.title = NULL;
  plot_file->header.head = NULL;
}

int _d3plot_read_geometry_data(d3plot_file *plot_file) {
  if (CDATAP.element_connectivity_packed) {
    plot_file->error_string = malloc(45);
    sprintf(plot_file->error_string,
            "Packed Element Connectivity is not supported");
    return 0;
  }

  if (CDATAP.icode != D3_CODE_OLD_DYNA3D &&
      CDATAP.icode != D3_CODE_NIKE3D_LS_DYNA3D_LS_NIKE3D) {
    plot_file->error_string = malloc(49);
    sprintf(plot_file->error_string,
            "The given order of the elements is not supported");
    return 0;
  }

  /* Print X*/
  double vec64[3];
  float vec32[3];

  size_t i = 0;
  while (i < CDATAP.numnp) {
    if (plot_file->buffer.word_size == 4) {
      d3_buffer_read_words(&plot_file->buffer, vec32, 3);
      vec64[0] = (double)vec32[0];
      vec64[1] = (double)vec32[1];
      vec64[2] = (double)vec32[2];
    } else {
      d3_buffer_read_words(&plot_file->buffer, vec64, 3);
    }

    i++;
  }

  if (CDATAP.nel8 > 0) {
    uint8_t *ix8 = malloc(9 * CDATAP.nel8 * plot_file->buffer.word_size);
    d3_buffer_read_words(&plot_file->buffer, ix8, 9 * CDATAP.nel8);

    d3_word value[9];
    uint32_t value32[9];

    size_t offset = 0;
    size_t i = 0;
    while (i < CDATAP.nel8) {
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
  } else if (CDATAP.nel8 < 0) {
    const int64_t nel8 = CDATAP.nel8 * -1;

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

  if (CDATAP.nelt > 0) {
    uint8_t *ixt = malloc(9 * CDATAP.nelt * plot_file->buffer.word_size);
    d3_buffer_read_words(&plot_file->buffer, ixt, 9 * CDATAP.nelt);

    d3_word value[9];
    uint32_t value32[9];

    size_t offset = 0;
    size_t i = 0;
    while (i < CDATAP.nelt) {
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

  if (CDATAP.nel2 > 0) {
    uint8_t *ix2 = malloc(6 * CDATAP.nel2 * plot_file->buffer.word_size);
    d3_buffer_read_words(&plot_file->buffer, ix2, 6 * CDATAP.nel2);

    d3_word value[6];
    uint32_t value32[6];

    size_t offset = 0;
    size_t i = 0;
    while (i < CDATAP.nel2) {
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

  if (CDATAP.nel4 > 0) {
    uint8_t *ix4 = malloc(5 * CDATAP.nel4 * plot_file->buffer.word_size);
    d3_buffer_read_words(&plot_file->buffer, ix4, 5 * CDATAP.nel4);

    d3_word value[5];
    uint32_t value32[5];

    size_t offset = 0;
    size_t i = 0;
    while (i < CDATAP.nel4) {
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
  if (CDATAP.nel8 < 0) {
    const int64_t nel8 = CDATAP.nel8 * -1;

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

  if (CDATAP.nel48 > 0) {
    uint8_t *ix48 = malloc(5 * CDATAP.nel48 * plot_file->buffer.word_size);
    d3_buffer_read_words(&plot_file->buffer, ix48, 5 * CDATAP.nel48);

    d3_word value[5];
    uint32_t value32[5];

    size_t offset = 0;
    size_t i = 0;
    while (i < CDATAP.nel48) {
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

  if (CDATAP.extra > 0 && CDATAP.nel20 > 0) {
    uint8_t *ix20 = malloc(13 * CDATAP.nel20 * plot_file->buffer.word_size);
    d3_buffer_read_words(&plot_file->buffer, ix20, 13 * CDATAP.nel20);

    d3_word value[13];
    uint32_t value32[13];

    size_t offset = 0;
    size_t i = 0;
    while (i < CDATAP.nel20) {
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
  if (CDATAP.nadapt == 0) {
    return 1;
  }

  uint8_t *aepl = malloc(2 * CDATAP.nadapt * plot_file->buffer.word_size);
  d3_buffer_read_words(&plot_file->buffer, aepl, 2 * CDATAP.nadapt);

  d3_word value[2];
  uint32_t value32[2];

  size_t offset = 0;
  size_t i = 0;
  while (i < CDATAP.nadapt) {
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
  if (CDATAP.narbs == 0) {
    CDATAP.numrbs = 0;

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

  if (nsort < 0) {
    d3_word nsrma = 0, nsrmu = 0, nsrmp = 0, nsrtm = 0;

    d3_buffer_read_words(&plot_file->buffer, &nsrma, 1);
    d3_buffer_read_words(&plot_file->buffer, &nsrmu, 1);
    d3_buffer_read_words(&plot_file->buffer, &nsrmp, 1);
    d3_buffer_read_words(&plot_file->buffer, &nsrtm, 1);
    d3_buffer_read_words(&plot_file->buffer, &CDATAP.numrbs, 1);
    d3_buffer_read_words(&plot_file->buffer, &nmmat, 1);
  } else {
    CDATAP.numrbs = 0;

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

  d3_buffer_read_words(&plot_file->buffer, nusern, nsortd);
  d3_buffer_read_words(&plot_file->buffer, nuserh, nsrhd);
  d3_buffer_read_words(&plot_file->buffer, nuserb, nsrbd);
  d3_buffer_read_words(&plot_file->buffer, nusers, nsrsd);
  d3_buffer_read_words(&plot_file->buffer, nusert, nsrtd);
  d3_buffer_read_words(&plot_file->buffer, norder, nmmat);
  d3_buffer_read_words(&plot_file->buffer, nsrmu_a, nmmat);
  d3_buffer_read_words(&plot_file->buffer, nsrmp_a, nmmat);

  size_t i = 0;
  size_t offset = 0;
  while (i < nsortd) {
    d3_word nid = 0;
    memcpy(&nid, &nusern[offset], plot_file->buffer.word_size);
    offset += plot_file->buffer.word_size;

    i++;
  }

  i = 0;
  while (i < nmmat) {
    d3_word value[3];
    memcpy(&value[0], &norder[i * plot_file->buffer.word_size],
           plot_file->buffer.word_size);
    memcpy(&value[1], &nsrmu_a[i * plot_file->buffer.word_size],
           plot_file->buffer.word_size);
    memcpy(&value[2], &nsrmp_a[i * plot_file->buffer.word_size],
           plot_file->buffer.word_size);

    i++;
  }

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

#define READ_MAT_VALUE(num, name, variable, variable_name)                     \
  i = 0;                                                                       \
  while (i < CDATAP.num) {                                                     \
    d3_buffer_read_double_word(&plot_file->buffer, &variable);                 \
    printf(name " %d " variable_name ": %f\n", i, variable);                   \
                                                                               \
    i++;                                                                       \
  }

int _d3plot_read_state_data(d3plot_file *plot_file) {
  const size_t state_start = plot_file->buffer.cur_word;

  double time;
  d3_buffer_read_double_word(&plot_file->buffer, &time);
  printf("TIME: %f\n", time);

  /* GLOBAL*/
  const size_t global_start = plot_file->buffer.cur_word;

  double ke, ie, te, x, y, z, mass, force;
  d3_buffer_read_double_word(&plot_file->buffer, &ke);
  d3_buffer_read_double_word(&plot_file->buffer, &ie);
  d3_buffer_read_double_word(&plot_file->buffer, &te);
  d3_buffer_read_double_word(&plot_file->buffer, &x);
  d3_buffer_read_double_word(&plot_file->buffer, &y);
  d3_buffer_read_double_word(&plot_file->buffer, &z);
  printf("KE: %f\nIE: %f\nTE: %f\nX: %f\nY: %f\nZ: %f\n", ke, ie, te, x, y, z);

  size_t i = 0;
  while (i < CDATAP.nummat8) {
    d3_buffer_read_double_word(&plot_file->buffer, &ie);
    printf("MAT8 %d IE: %f\n", i, ie);

    i++;
  }

  READ_MAT_VALUE(nummat2, "MAT2", ie, "IE");
  READ_MAT_VALUE(nummat4, "MAT4", ie, "IE");
  READ_MAT_VALUE(nummatt, "MATT", ie, "IE");
  READ_MAT_VALUE(numrbs, "RBS", ie, "IE");

  READ_MAT_VALUE(nummat8, "MAT8", ke, "KE");
  READ_MAT_VALUE(nummat2, "MAT2", ke, "KE");
  READ_MAT_VALUE(nummat4, "MAT4", ke, "KE");
  READ_MAT_VALUE(nummatt, "MATT", ke, "KE");
  READ_MAT_VALUE(numrbs, "RBS", ke, "KE");

  READ_MAT_VALUE(nummat8, "MAT8", x, "X");
  READ_MAT_VALUE(nummat2, "MAT2", x, "X");
  READ_MAT_VALUE(nummat4, "MAT4", x, "X");
  READ_MAT_VALUE(nummatt, "MATT", x, "X");
  READ_MAT_VALUE(numrbs, "RBS", x, "X");

  READ_MAT_VALUE(nummat8, "MAT8", y, "Y");
  READ_MAT_VALUE(nummat2, "MAT2", y, "Y");
  READ_MAT_VALUE(nummat4, "MAT4", y, "Y");
  READ_MAT_VALUE(nummatt, "MATT", y, "Y");
  READ_MAT_VALUE(numrbs, "RBS", y, "Y");

  READ_MAT_VALUE(nummat8, "MAT8", z, "Z");
  READ_MAT_VALUE(nummat2, "MAT2", z, "Z");
  READ_MAT_VALUE(nummat4, "MAT4", z, "Z");
  READ_MAT_VALUE(nummatt, "MATT", z, "Z");
  READ_MAT_VALUE(numrbs, "RBS", z, "Z");

  READ_MAT_VALUE(nummat8, "MAT8", mass, "MASS");
  READ_MAT_VALUE(nummat2, "MAT2", mass, "MASS");
  READ_MAT_VALUE(nummat4, "MAT4", mass, "MASS");
  READ_MAT_VALUE(nummatt, "MATT", mass, "MASS");
  READ_MAT_VALUE(numrbs, "RBS", mass, "MASS");

  READ_MAT_VALUE(nummat8, "MAT8", force, "FORCE");
  READ_MAT_VALUE(nummat2, "MAT2", force, "FORCE");
  READ_MAT_VALUE(nummat4, "MAT4", force, "FORCE");
  READ_MAT_VALUE(nummatt, "MATT", force, "FORCE");
  READ_MAT_VALUE(numrbs, "RBS", force, "FORCE");

  /* Assume that N is one*/
  const size_t RWN = 1;
  size_t numrw;
  {
    numrw = (CDATAP.nglbv - 6 -
             7 * (CDATAP.nummat8 + CDATAP.nummat2 + CDATAP.nummat4 +
                  CDATAP.nummatt + CDATAP.numrbs)) /
            RWN;
  }
  printf("NUMRW: %d\n", numrw);

  i = 0;
  while (i < numrw) {
    d3_buffer_read_double_word(&plot_file->buffer, &force);
    printf("RW_FORCE %d: %f\n", i, force);

    i++;
  }

  if (RWN == 4) {
    i = 0;
    double pos[3];
    while (i < numrw) {
      d3_buffer_read_double_word(&plot_file->buffer, &pos[0]);
      d3_buffer_read_double_word(&plot_file->buffer, &pos[1]);
      d3_buffer_read_double_word(&plot_file->buffer, &pos[2]);

      printf("RW_POS %d: (%f, %f, %f)\n", i, pos[0], pos[1], pos[2]);

      i++;
    }
  }

  const size_t global_end = plot_file->buffer.cur_word;
  const size_t global_size = global_end - global_start;

  if (global_size != CDATAP.nglbv) {
    plot_file->error_string = malloc(70);
    sprintf(plot_file->error_string, "Size of GLOBAL is %d instead of %d",
            global_size, CDATAP.nglbv);
    return 0;
  }

  /* NODEDATA*/
  /**** Order of Node Data ******
   * IT, U, Mass Scaling, V, A
   ******************************/

  printf("Read node data at 0x%x\n",
         plot_file->buffer.cur_word * plot_file->buffer.word_size);

  const size_t node_data_start = plot_file->buffer.cur_word;

  size_t it = _get_nth_digit(CDATAP.it, 0);
  size_t N = it * (it > 1);
  if (N == 2) {
    it = 1;
    N = 3;
  }
  const size_t mass_N = _get_nth_digit(CDATAP.it, 1) == 1;

  const size_t NND =
      ((it + N + mass_N) + CDATAP.ndim * (CDATAP.iu + CDATAP.iv + CDATAP.ia)) *
      CDATAP.numnp;

  double temp[3], node_flux[3], mass_scaling, u[3], v[3], a[3];

  size_t n = 0;
  while (n < CDATAP.numnp) {
    printf("NODE %d\n", n);

    size_t j = 0;
    while (j < it) {
      d3_buffer_read_double_word(&plot_file->buffer, &temp[j]);

      j++;
    }

    j = 0;
    while (j < N) {
      d3_buffer_read_double_word(&plot_file->buffer, &node_flux[j]);
    }

    if (mass_N == 1) {
      d3_buffer_read_double_word(&plot_file->buffer, &mass_scaling);
    }

    if (it == 1) {
      printf("TEMP: %f\n", temp[0]);
    } else if (it == 3) {
      printf("TEMP: (%f, %f, %f)\n", temp[0], temp[1], temp[2]);
    }

    if (N == 3) {
      printf("FLUX: (%f, %f, %f)\n", node_flux[0], node_flux[1], node_flux[2]);
    }

    if (mass_N == 1) {
      printf("MASS SCALING: %f\n", mass_scaling);
    }

    j = 0;
    while (j < CDATAP.ndim) {
      if (CDATAP.iu) {
        d3_buffer_read_double_word(&plot_file->buffer, &u[j]);
      }
      if (CDATAP.iv) {
        d3_buffer_read_double_word(&plot_file->buffer, &v[j]);
      }
      if (CDATAP.ia) {
        d3_buffer_read_double_word(&plot_file->buffer, &a[j]);
      }

      j++;
    }

    if (CDATAP.iu) {
      printf("DISPLACEMENT: (%f, %f, %f)\n", u[0], u[1], u[2]);
    }
    if (CDATAP.iv) {
      printf("VELOCITY: (%f, %f, %f)\n", v[0], v[1], v[2]);
    }
    if (CDATAP.ia) {
      printf("ACCELERATION: (%f, %f, %f)\n", a[0], a[1], a[2]);
    }

    n++;
  }

  const size_t node_data_end = plot_file->buffer.cur_word;
  const size_t node_data_size = node_data_end - node_data_start;
  if (node_data_size != NND) {
    plot_file->error_string = malloc(70);
    sprintf(plot_file->error_string, "NODEDATA should be %d instead of %d", NND,
            node_data_size);
    fflush(stdout);
    return 0;
  }

  /* THERMDATA*/
  double nt3d;

  n = 0;
  while (n < CDATAP.nt3d) {
    size_t e = 0;
    while (e < CDATAP.nel8) {
      d3_buffer_read_double_word(&plot_file->buffer, &nt3d);
      printf("NT3D %d: EL8 %d: %f\n", n, e, nt3d);

      e++;
    }

    n++;
  }

  /* CFDDATA is no longer output*/

  /* ELEMDATA*/
  const size_t ENN =
      CDATAP.nel8 * CDATAP.nv3d + CDATAP.nelt * CDATAP.nv3dt +
      CDATAP.nel2 * CDATAP.nv1d + CDATAP.nel4 * CDATAP.nv2d +
      CDATAP.nmsph * 0; /* We don't support SMOOTH PATICLE HYDRODYNAMICS*/
  const size_t elem_data_start = plot_file->buffer.cur_word;

  size_t e = 0;
  while (e < CDATAP.nel8) {
    double *e_data = malloc(CDATAP.nv3d * sizeof(double));
    if (plot_file->buffer.word_size == 4) {
      float *e_data32 = malloc(CDATAP.nv3d * sizeof(float));
      d3_buffer_read_words(&plot_file->buffer, e_data32, CDATAP.nv3d);
      size_t j = 0;
      while (j < CDATAP.nv3d) {
        e_data[j] = e_data32[j];

        j++;
      }

      free(e_data32);
    } else {
      d3_buffer_read_words(&plot_file->buffer, e_data, CDATAP.nv3d);
    }

    printf("EL8 %d: (", e);
    size_t j = 0;
    while (j < CDATAP.nv3d) {
      printf("%f, ", e_data[j]);

      j++;
    }
    printf(")\n");

    free(e_data);

    e++;
  }

  e = 0;
  while (e < CDATAP.nel2) {
    double *e_data = malloc(CDATAP.nv1d * sizeof(double));
    if (plot_file->buffer.word_size == 4) {
      float *e_data32 = malloc(CDATAP.nv1d * sizeof(float));
      d3_buffer_read_words(&plot_file->buffer, e_data32, CDATAP.nv1d);
      size_t j = 0;
      while (j < CDATAP.nv1d) {
        e_data[j] = e_data32[j];

        j++;
      }

      free(e_data32);
    } else {
      d3_buffer_read_words(&plot_file->buffer, e_data, CDATAP.nv1d);
    }

    printf("EL2 %d: (", e);
    size_t j = 0;
    while (j < CDATAP.nv1d) {
      printf("%f, ", e_data[j]);

      j++;
    }
    printf(")\n");

    free(e_data);

    e++;
  }

  e = 0;
  while (e < CDATAP.nel4) {
    double *e_data = malloc(CDATAP.nv2d * sizeof(double));
    if (plot_file->buffer.word_size == 4) {
      float *e_data32 = malloc(CDATAP.nv2d * sizeof(float));
      d3_buffer_read_words(&plot_file->buffer, e_data32, CDATAP.nv2d);
      size_t j = 0;
      while (j < CDATAP.nv2d) {
        e_data[j] = e_data32[j];

        j++;
      }

      free(e_data32);
    } else {
      d3_buffer_read_words(&plot_file->buffer, e_data, CDATAP.nv2d);
    }

    printf("EL4 %d: (", e);
    size_t j = 0;
    while (j < CDATAP.nv2d) {
      printf("%f, ", e_data[j]);

      j++;
    }
    printf(")\n");

    free(e_data);

    e++;
  }

  /* Then follows who knows what -_(′_′)_-*/
  /* But because we don't support NMSPH, we can assume that NELT follows*/
  e = 0;
  while (e < CDATAP.nelt) {
    double *e_data = malloc(CDATAP.nv3dt * sizeof(double));
    if (plot_file->buffer.word_size == 4) {
      float *e_data32 = malloc(CDATAP.nv3dt * sizeof(float));
      d3_buffer_read_words(&plot_file->buffer, e_data32, CDATAP.nv3dt);
      size_t j = 0;
      while (j < CDATAP.nv3dt) {
        e_data[j] = e_data32[j];

        j++;
      }

      free(e_data32);
    } else {
      d3_buffer_read_words(&plot_file->buffer, e_data, CDATAP.nv3dt);
    }

    printf("ELT %d: (", e);
    size_t j = 0;
    while (j < CDATAP.nv3dt) {
      printf("%f, ", e_data[j]);

      j++;
    }
    printf(")\n");

    free(e_data);

    e++;
  }

  const size_t elem_data_end = plot_file->buffer.cur_word;
  const size_t elem_data_size = elem_data_end - elem_data_start;
  if (elem_data_size < ENN) {
    plot_file->error_string = malloc(70);
    sprintf(plot_file->error_string, "ELEMDATA should be %d instead of %d", ENN,
            elem_data_size);
    fflush(stdout);
    return 0;
  }

  /* Element Deletion Option*/
  size_t skip_words;
  if (CDATAP.mdlopt == 0) {
    skip_words = 0;
  } else if (CDATAP.mdlopt == 1) {
    skip_words = CDATAP.numnp;
  } else if (CDATAP.mdlopt == 2) {
    skip_words = CDATAP.nel8 + CDATAP.nelt + CDATAP.nel4 + CDATAP.nel2;
  } else {
    plot_file->error_string = malloc(50);
    sprintf(plot_file->error_string, "The value of MDLOPT is invalid: %d",
            CDATAP.mdlopt);
    return 0;
  }

  if (skip_words > 0) {
    d3_buffer_skip_words(&plot_file->buffer, skip_words);
  }

  fflush(stdout);

  const size_t state_end = plot_file->buffer.cur_word;
  const size_t state_size =
      (state_end - state_start) * plot_file->buffer.word_size;
  printf("STATE SIZE: %d\n", state_size);

  return 1;
}

const char *_d3plot_get_file_type_name(d3_word file_type) {
  switch (file_type) {
  case D3_FILE_TYPE_D3PLOT:
    return "d3plot";
  case D3_FILE_TYPE_D3DRLF:
    return "d3drlf";
  case D3_FILE_TYPE_D3THDT:
    return "d3thdt";
  case D3_FILE_TYPE_INTFOR:
    return "intfor";
  case D3_FILE_TYPE_D3PART:
    return "d3part";
  case D3_FILE_TYPE_BLSTFOR:
    return "blstfor";
  case D3_FILE_TYPE_D3CPM:
    return "d3cpm";
  case D3_FILE_TYPE_D3ALE:
    return "d3ale";
  case D3_FILE_TYPE_D3EIGV:
    return "d3eigv";
  case D3_FILE_TYPE_D3MODE:
    return "d3mode";
  case D3_FILE_TYPE_D3ITER:
    return "d3iter";
  case D3_FILE_TYPE_D3SSD:
    return "d3ssd";
  case D3_FILE_TYPE_D3SPCM:
    return "d3spcm";
  case D3_FILE_TYPE_D3PSD:
    return "d3psd";
  case D3_FILE_TYPE_D3RMS:
    return "d3rms";
  case D3_FILE_TYPE_D3FTG:
    return "d3ftg";
  case D3_FILE_TYPE_D3ACS:
    return "d3acs";
  default:
    return "invalid";
  };
}

int _get_nth_digit(d3_word value, int n) {
  int i = 0;
  while (1) {
    d3_word last_value = value;
    value /= 10;
    if (i == n) {
      value *= 10;
      return last_value - value;
    }

    i++;
  }
}