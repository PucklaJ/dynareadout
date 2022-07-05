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

  READ_CONTROL_DATA_PLOT_FILE_WORD(run_time);

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
  READ_CONTROL_DATA_PLOT_FILE_WORD(maxint);
  READ_CONTROL_DATA_PLOT_FILE_WORD(edlopt);
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
  READ_CONTROL_DATA_PLOT_FILE_WORD(idtdt);
  READ_CONTROL_DATA_PLOT_FILE_WORD(extra);
  READ_CONTROL_DATA_PLOT_FILE_WORD(words[0]);
  READ_CONTROL_DATA_PLOT_FILE_WORD(words[1]);
  READ_CONTROL_DATA_PLOT_FILE_WORD(words[2]);
  READ_CONTROL_DATA_PLOT_FILE_WORD(words[3]);
  READ_CONTROL_DATA_PLOT_FILE_WORD(words[4]);
  READ_CONTROL_DATA_PLOT_FILE_WORD(words[5]);

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

  return plot_file;
}

void d3plot_close(d3plot_file *plot_file) {
  d3_buffer_close(&plot_file->buffer);

  free(plot_file->control_data.title);
  free(plot_file->error_string);

  plot_file->control_data.title = NULL;
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

    printf("Node %d: (%f, %f, %f)\n", i, vec64[0], vec64[1], vec64[2]);

    i++;
  }

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