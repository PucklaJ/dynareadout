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
#include <time.h>

#define READ_CONTROL_DATA_PLOT_FILE_WORD(value)                                \
  plot_file.control_data.value = 0;                                            \
  d3_buffer_read_words(&plot_file.buffer, &plot_file.control_data.value, 1)
#define READ_CONTROL_DATA_PLOT_FILE_SIGNED_WORD(value)                         \
  if (plot_file.buffer.word_size == 4) {                                       \
    int32_t value32;                                                           \
    d3_buffer_read_words(&plot_file.buffer, &value32, 1);                      \
    CDA.value = value32;                                                       \
  } else {                                                                     \
    d3_buffer_read_words(&plot_file.buffer, &CDA.value, 1);                    \
  }
#define READ_CONTROL_DATA_WORD(value)                                          \
  d3_word value = 0;                                                           \
  d3_buffer_read_words(&plot_file.buffer, &value, 1)
#define CDA plot_file.control_data

d3plot_file d3plot_open(const char *root_file_name) {
  d3plot_file plot_file;
  plot_file.error_string = NULL;
  plot_file.data_pointers = NULL;
  plot_file.num_states = 0;

  plot_file.buffer = d3_buffer_open(root_file_name);
  if (plot_file.buffer.error_string) {
    /* Swaperoo*/
    plot_file.error_string = plot_file.buffer.error_string;
    plot_file.buffer.error_string = NULL;
    return plot_file;
  }

  /* Allocate the first data pointers*/
  plot_file.data_pointers = malloc(D3PLT_PTR_COUNT * sizeof(size_t));
  size_t i = 0;
  while (i < D3PLT_PTR_COUNT) {
    plot_file.data_pointers[i] = 0;

    i++;
  }

  d3_buffer_skip_words(&plot_file.buffer, 10); /* Title*/
  plot_file.data_pointers[D3PLT_PTR_RUN_TIME] = plot_file.buffer.cur_word;
  d3_buffer_skip_words(&plot_file.buffer, 1); /* Run time*/

  READ_CONTROL_DATA_WORD(file_type);
  if (file_type > 1000) {
    file_type -= 1000;
    /* TODO: all external(users) numbers (Node, Element, Material and Rigid
  Surface Nodes) will be written in I8 format.*/
  }
  /* Quit immediately if this is not a d3plot file*/
  if (file_type != D3_FILE_TYPE_D3PLOT) {
    plot_file.error_string = malloc(50);
    sprintf(plot_file.error_string, "Wrong file type: %s",
            _d3plot_get_file_type_name(file_type));
    return plot_file;
  }

  d3_buffer_skip_words(&plot_file.buffer, 1); /* TODO: Source version*/
  d3_buffer_skip_words(&plot_file.buffer, 1); /* TODO: Release version*/
  d3_buffer_skip_words(&plot_file.buffer, 1); /* TODO: Version*/
  READ_CONTROL_DATA_PLOT_FILE_WORD(ndim);
  READ_CONTROL_DATA_PLOT_FILE_WORD(numnp);
  READ_CONTROL_DATA_PLOT_FILE_WORD(icode);
  READ_CONTROL_DATA_PLOT_FILE_WORD(nglbv);
  READ_CONTROL_DATA_PLOT_FILE_WORD(it);
  READ_CONTROL_DATA_PLOT_FILE_WORD(iu);
  READ_CONTROL_DATA_PLOT_FILE_WORD(iv);
  READ_CONTROL_DATA_PLOT_FILE_WORD(ia);
  READ_CONTROL_DATA_PLOT_FILE_SIGNED_WORD(nel8);
  READ_CONTROL_DATA_PLOT_FILE_WORD(nummat8);
  d3_buffer_skip_words(&plot_file.buffer, 1); /* TODO: NUMDS*/
  d3_buffer_skip_words(&plot_file.buffer, 1); /* TODO: NUMST*/
  READ_CONTROL_DATA_PLOT_FILE_WORD(nv3d);
  READ_CONTROL_DATA_PLOT_FILE_WORD(nel2);
  READ_CONTROL_DATA_PLOT_FILE_WORD(nummat2);
  READ_CONTROL_DATA_PLOT_FILE_WORD(nv1d);
  READ_CONTROL_DATA_PLOT_FILE_WORD(nel4);
  READ_CONTROL_DATA_PLOT_FILE_WORD(nummat4);
  READ_CONTROL_DATA_PLOT_FILE_WORD(nv2d);
  READ_CONTROL_DATA_PLOT_FILE_WORD(neiph);
  READ_CONTROL_DATA_PLOT_FILE_WORD(neips);
  READ_CONTROL_DATA_PLOT_FILE_SIGNED_WORD(maxint);
  /*READ_CONTROL_DATA_PLOT_FILE_WORD(edlopt); Not used in LS-Dyna?*/
  READ_CONTROL_DATA_PLOT_FILE_WORD(nmsph);
  d3_buffer_skip_words(&plot_file.buffer, 1); /* TODO: NGPSPH*/
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
  d3_buffer_skip_words(&plot_file.buffer, 1); /* TODO: NCFDV2*/
  READ_CONTROL_DATA_PLOT_FILE_WORD(nadapt);
  READ_CONTROL_DATA_PLOT_FILE_WORD(nmmat);
  d3_buffer_skip_words(&plot_file.buffer, 1); /* TODO: NUMFLUID*/
  d3_buffer_skip_words(&plot_file.buffer, 1); /* TODO: INN*/
  READ_CONTROL_DATA_PLOT_FILE_WORD(npefg);
  READ_CONTROL_DATA_PLOT_FILE_WORD(nel48);
  READ_CONTROL_DATA_PLOT_FILE_WORD(idtdt);
  READ_CONTROL_DATA_WORD(extra);
  d3_buffer_skip_words(&plot_file.buffer, 6); /* TODO: WORDS*/

  if (extra > 0) {
    READ_CONTROL_DATA_PLOT_FILE_WORD(nel20);
    READ_CONTROL_DATA_PLOT_FILE_WORD(nt3d);
  } else {
    CDA.nel20 = 0;
    CDA.nt3d = 0;
  }

  if (CDA.ndim == 5 || CDA.ndim == 7) {
    CDA.mattyp = 1;
    CDA.ndim = 3;
  } else {
    CDA.mattyp = 0;
    if (CDA.ndim == 3) {
      CDA.element_connectivity_packed = 1;
    } else {
      CDA.element_connectivity_packed = 0;
      if (CDA.ndim == 4) {
        CDA.ndim = 3;
      }
    }
  }

  /* Quit immediately if NDIM is not supported*/
  if (CDA.ndim != 3) {
    plot_file.error_string = malloc(50);
    sprintf(plot_file.error_string, "A ndim value of %d is not supported",
            CDA.ndim);
    return plot_file;
  }

  i = 0;
  while (i < 4) {
    CDA.ioshl[i] -= 999 * (CDA.ioshl[i] == 1000);

    i++;
  }

  if (_get_nth_digit(CDA.idtdt, 0) == 1) {
    /* TODO: An array of dT/dt values of
             length NUMNP. Array is
             written after node temperature
             arrays.*/
  }
  if (_get_nth_digit(CDA.idtdt, 1) == 1) {
    /* TODO: An array of residual forces of
             length 3*NUMNP followed by
             residual moments of length
             3*NUMNP. This data is written
             after node temperatures or
             dT/dt values if there are output.*/
  }
  if (_get_nth_digit(CDA.idtdt, 2) == 1) {
    /* TODO: Plastic strain tensor is written
             for each solid and shell after
             standard element data. For
             solids (6 values) and for shells
             (6 x 3 = 18 values), at the
             lower, middle and upper
             integration location.*/
    CDA.plastic_strain_tensor_written = 1;
  } else {
    CDA.plastic_strain_tensor_written = 0;
  }
  if (_get_nth_digit(CDA.idtdt, 3) == 1) {
    /* TODO: Thermal strain tensor is written
             after standard element data. For
             solid (6 values) and shell (6
             values) and after the plastic
             strain tensor if output.*/
    CDA.thermal_strain_tensor_written = 1;
  } else {
    CDA.thermal_strain_tensor_written = 0;
  }

  if (CDA.plastic_strain_tensor_written || CDA.thermal_strain_tensor_written) {
    CDA.istrn = _get_nth_digit(CDA.idtdt, 4);
  }

  /* Compute MDLOPT*/
  if (CDA.maxint >= 0) {
    CDA.mdlopt = 0;
  } else if (CDA.maxint < -10000) {
    CDA.mdlopt = 2;
    CDA.maxint = CDA.maxint * -1 - 10000;
  } else if (CDA.maxint < 0) {
    CDA.mdlopt = 1;
    CDA.maxint *= -1;
  } else {
    plot_file.error_string = malloc(40);
    sprintf(plot_file.error_string, "Invalid value for MAXINT: %d", CDA.maxint);
    return plot_file;
  }

  if (CDA.idtdt < 100) {
    /* We need to compute ISTRN*/
    /*ISTRN can only be computed as follows and if NV2D > 0.
      If NV2D-MAXINT*(6*IOSHL(1)+IOSHL(2)+NEIPS)+8*IOSHL(3)+4*IOSHL(4) > 1
      Then ISTRN = 1, else ISTRN = 0

      If ISTRN=1, and NEIPH>=6, last the 6 additional values are the six strain
      components.

      Or NELT > 0
      If NV3DT-MAXINT*(6*IOSHL(1)+IOSHL(2)+NEIPS) > 1
      Then ISTRN = 1, else ISTRN = 0*/
    const d3_word rhs =
        CDA.maxint * (6 * CDA.ioshl[0] + CDA.ioshl[1] + CDA.neips) +
        8 * CDA.ioshl[2] + 4 * CDA.ioshl[3];
    if (CDA.nv2d > rhs + 1) {
      CDA.istrn = 1;
    } else {
      CDA.istrn = 0;
    }

    if (CDA.istrn == 1 && CDA.neiph >= 6) {
      /* TODO: last the 6 additional values are the six strain*/
    }

    if (CDA.nelt > 0) {
      if ((CDA.nv3dt -
           CDA.maxint * (6 * CDA.ioshl[0] + CDA.ioshl[1] + CDA.neips)) > 1) {
        CDA.istrn = 1;
      } else {
        CDA.istrn = 0;
      }
    }
  }

  /* We are done with CONTROL DATA now comes the real data*/

  if (CDA.mattyp) {
    plot_file.error_string = malloc(38);
    sprintf(plot_file.error_string, "MATERIAL TYPE DATA is not implemented");
    return plot_file;
  }
  if (CDA.ialemat) {
    plot_file.error_string = malloc(42);
    sprintf(plot_file.error_string,
            "FLUID MATERIAL ID DATA is not implemented");
    return plot_file;
  }
  if (CDA.nmsph) {
    plot_file.error_string = malloc(68);
    sprintf(
        plot_file.error_string,
        "SMOOTH PARTICLE HYDRODYNAMICS ELEMENT DATA FLAGS is not implemented");
    return plot_file;
  }
  if (CDA.npefg) {
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

  if (CDA.nmsph > 0) {
    plot_file.error_string = malloc(72);
    sprintf(plot_file.error_string, "SMOOTH PARTICLE HYDRODYNAMICS NODE AND "
                                    "MATERIAL LIST is not implemented");
    return plot_file;
  }

  if (CDA.npefg > 0) {
    plot_file.error_string = malloc(42);
    sprintf(plot_file.error_string,
            "PARTICLE GEOMETRY DATA is not implemented");
    return plot_file;
  }

  if (CDA.ndim > 5) {
    plot_file.error_string = malloc(43);
    sprintf(plot_file.error_string,
            "RIGID ROAD SURFACE DATA is not implemented");
    return plot_file;
  }

  /* Read EOF marker*/
  double eof_marker;
  d3_buffer_read_double_word(&plot_file.buffer, &eof_marker);

  if (eof_marker != D3_EOF) {
    plot_file.error_string = malloc(50);
    sprintf(plot_file.error_string, "Here (%d) should be the EOF marker",
            plot_file.buffer.cur_word - 1);
    return plot_file;
  }

  if (!_d3plot_read_header(&plot_file)) {
    return plot_file;
  }

  if (CDA.ncfdv1 == 67108864) {
    plot_file.error_string = malloc(36);
    sprintf(plot_file.error_string, "EXTRA DATA TYPES is not implemented");
    return plot_file;
  }

  if (!d3_buffer_next_file(&plot_file.buffer)) {
    plot_file.error_string = malloc(14);
    sprintf(plot_file.error_string, "Too few files");
    return plot_file;
  }

  /* Here comes the STATE DATA*/

  int result = 1;
  while (result) {
    result = _d3plot_read_state_data(&plot_file);
    if (result == 2) {
      if (!d3_buffer_next_file(&plot_file.buffer)) {
        break;
      }
    }
  }

  return plot_file;
}

void d3plot_close(d3plot_file *plot_file) {
  d3_buffer_close(&plot_file->buffer);

  free(plot_file->data_pointers);
  free(plot_file->error_string);

  plot_file->num_states = 0;
}

d3_word *d3plot_read_node_ids(d3plot_file *plot_file, size_t *num_ids) {
  return _d3plot_read_ids(plot_file, num_ids, D3PLT_PTR_NODE_IDS,
                          plot_file->control_data.numnp);
}

d3_word *d3plot_read_solid_element_ids(d3plot_file *plot_file,
                                       size_t *num_ids) {
  return _d3plot_read_ids(plot_file, num_ids, D3PLT_PTR_EL8_IDS,
                          plot_file->control_data.nel8);
}

d3_word *d3plot_read_beam_element_ids(d3plot_file *plot_file, size_t *num_ids) {
  return _d3plot_read_ids(plot_file, num_ids, D3PLT_PTR_EL2_IDS,
                          plot_file->control_data.nel2);
}

d3_word *d3plot_read_shell_element_ids(d3plot_file *plot_file,
                                       size_t *num_ids) {
  return _d3plot_read_ids(plot_file, num_ids, D3PLT_PTR_EL4_IDS,
                          plot_file->control_data.nel4);
}

d3_word *d3plot_read_thick_shell_element_ids(d3plot_file *plot_file,
                                             size_t *num_ids) {
  return _d3plot_read_ids(plot_file, num_ids, D3PLT_PTR_ELT_IDS,
                          plot_file->control_data.nelt);
}

d3_word *d3plot_read_all_element_ids(d3plot_file *plot_file, size_t *num_ids) {
  d3_word *all_ids = NULL;
  *num_ids = 0;

  size_t num_ids_buffer;
  d3_word *ids_buffer =
      d3plot_read_solid_element_ids(plot_file, &num_ids_buffer);
  if (num_ids_buffer > 0) {
    all_ids = _insert_sorted(all_ids, *num_ids, ids_buffer, num_ids_buffer);
    *num_ids += num_ids_buffer;
    free(ids_buffer);
  }

  ids_buffer = d3plot_read_beam_element_ids(plot_file, &num_ids_buffer);
  if (num_ids_buffer > 0) {
    all_ids = _insert_sorted(all_ids, *num_ids, ids_buffer, num_ids_buffer);
    *num_ids += num_ids_buffer;
    free(ids_buffer);
  }

  ids_buffer = d3plot_read_shell_element_ids(plot_file, &num_ids_buffer);
  if (num_ids_buffer > 0) {
    all_ids = _insert_sorted(all_ids, *num_ids, ids_buffer, num_ids_buffer);
    *num_ids += num_ids_buffer;
    free(ids_buffer);
  }

  ids_buffer = d3plot_read_thick_shell_element_ids(plot_file, &num_ids_buffer);
  if (num_ids_buffer > 0) {
    all_ids = _insert_sorted(all_ids, *num_ids, ids_buffer, num_ids_buffer);
    *num_ids += num_ids_buffer;
    free(ids_buffer);
  }

  return all_ids;
}

d3_word *d3plot_read_part_ids(d3plot_file *plot_file, size_t *num_parts) {
  return _d3plot_read_ids(plot_file, num_parts, D3PLT_PTR_PART_IDS,
                          plot_file->control_data.nmmat);
}

char **d3plot_read_part_titles(d3plot_file *plot_file, size_t *num_parts) {
  *num_parts = plot_file->control_data.nmmat;
  char **part_titles = malloc(*num_parts * sizeof(char *));

  size_t i = 0;
  while (i < *num_parts) {
    part_titles[i] = malloc(18 * plot_file->buffer.word_size + 1);
    if (i == 0)
      d3_buffer_read_words_at(&plot_file->buffer, part_titles[i], 18,
                              plot_file->data_pointers[D3PLT_PTR_PART_TITLES] +
                                  1);
    else {
      d3_buffer_skip_words(&plot_file->buffer, 1);
      d3_buffer_read_words(&plot_file->buffer, part_titles[i], 18);
    }

    part_titles[i][18 * plot_file->buffer.word_size] = '\0';

    i++;
  }

  return part_titles;
}

double *d3plot_read_node_coordinates(d3plot_file *plot_file, size_t state,
                                     size_t *num_nodes) {
  return _d3plot_read_node_data(plot_file, state, num_nodes,
                                D3PLT_PTR_STATE_NODE_COORDS);
}

double *d3plot_read_node_velocity(d3plot_file *plot_file, size_t state,
                                  size_t *num_nodes) {
  return _d3plot_read_node_data(plot_file, state, num_nodes,
                                D3PLT_PTR_STATE_NODE_VEL);
}

double *d3plot_read_node_acceleration(d3plot_file *plot_file, size_t state,
                                      size_t *num_nodes) {
  return _d3plot_read_node_data(plot_file, state, num_nodes,
                                D3PLT_PTR_STATE_NODE_ACC);
}

double d3plot_read_time(d3plot_file *plot_file, size_t state) {
  if (state >= plot_file->num_states) {
    plot_file->error_string = malloc(70);
    sprintf(plot_file->error_string, "%d is out of bounds for the states");
    return -1.0;
  }

  double time;
  if (plot_file->buffer.word_size == 4) {
    float time32;
    d3_buffer_read_words_at(&plot_file->buffer, &time32, 1,
                            plot_file->data_pointers[D3PLT_PTR_STATES + state] +
                                plot_file->data_pointers[D3PLT_PTR_STATE_TIME]);
    time = time32;
  } else {
    d3_buffer_read_words_at(&plot_file->buffer, &time, 1,
                            plot_file->data_pointers[D3PLT_PTR_STATES + state] +
                                plot_file->data_pointers[D3PLT_PTR_STATE_TIME]);
  }

  return time;
}

d3plot_solid_con *d3plot_read_solid_elements(d3plot_file *plot_file,
                                             size_t *num_solids) {
  if (plot_file->control_data.nel8 <= 0) {
    /* nel8 represents the number of extra nodes for ten node solids*/
    *num_solids = 0;
    return NULL;
  }

  *num_solids = plot_file->control_data.nel8;
  d3plot_solid_con *solids = malloc(*num_solids * sizeof(d3plot_solid_con));
  if (plot_file->buffer.word_size == 4) {
    uint32_t *solids32 = malloc(*num_solids * 9 * sizeof(uint32_t));
    d3_buffer_read_words_at(&plot_file->buffer, solids32, 9 * *num_solids,
                            plot_file->data_pointers[D3PLT_PTR_EL8_CONNECT]);

    size_t i = 0;
    while (i < *num_solids) {
      size_t j = 0;
      while (j < 8) {
        solids[i].node_ids[j + 0] = solids32[i * 9 + j + 0];
        solids[i].node_ids[j + 1] = solids32[i * 9 + j + 1];
        solids[i].node_ids[j + 2] = solids32[i * 9 + j + 2];
        solids[i].node_ids[j + 3] = solids32[i * 9 + j + 3];

        j += 4;
      }
      solids[i].material_id = solids32[i * 9 + 8];

      i++;
    }

    free(solids32);
  } else {
    d3_buffer_read_words_at(&plot_file->buffer, solids, 9 * *num_solids,
                            plot_file->data_pointers[D3PLT_PTR_EL8_CONNECT]);
  }

  return solids;
}

d3plot_thick_shell_con *
d3plot_read_thick_shell_elements(d3plot_file *plot_file,
                                 size_t *num_thick_shells) {
  if (plot_file->control_data.nelt == 0) {
    *num_thick_shells = 0;
    return NULL;
  }

  *num_thick_shells = plot_file->control_data.nelt;
  d3plot_thick_shell_con *thick_shells =
      malloc(*num_thick_shells * sizeof(d3plot_thick_shell_con));
  if (plot_file->buffer.word_size == 4) {
    uint32_t *thick_shells32 = malloc(*num_thick_shells * 9 * sizeof(uint32_t));
    d3_buffer_read_words_at(&plot_file->buffer, thick_shells32,
                            9 * *num_thick_shells,
                            plot_file->data_pointers[D3PLT_PTR_ELT_CONNECT]);

    size_t i = 0;
    while (i < *num_thick_shells) {
      size_t j = 0;
      while (j < 8) {
        thick_shells[i].node_ids[j + 0] = thick_shells32[i * 9 + j + 0];
        thick_shells[i].node_ids[j + 1] = thick_shells32[i * 9 + j + 1];
        thick_shells[i].node_ids[j + 2] = thick_shells32[i * 9 + j + 2];
        thick_shells[i].node_ids[j + 3] = thick_shells32[i * 9 + j + 3];

        j += 4;
      }
      thick_shells[i].material_id = thick_shells32[i * 9 + 8];

      i++;
    }

    free(thick_shells32);
  } else {
    d3_buffer_read_words_at(&plot_file->buffer, thick_shells,
                            9 * *num_thick_shells,
                            plot_file->data_pointers[D3PLT_PTR_ELT_CONNECT]);
  }

  return thick_shells;
}

d3plot_beam_con *d3plot_read_beam_elements(d3plot_file *plot_file,
                                           size_t *num_beams) {
  if (plot_file->control_data.nel2 == 0) {
    *num_beams = 0;
    return NULL;
  }

  *num_beams = plot_file->control_data.nel2;
  d3plot_beam_con *beams = malloc(*num_beams * sizeof(d3plot_beam_con));
  if (plot_file->buffer.word_size == 4) {
    uint32_t *beams32 = malloc(*num_beams * 6 * sizeof(uint32_t));
    d3_buffer_read_words_at(&plot_file->buffer, beams32, 6 * *num_beams,
                            plot_file->data_pointers[D3PLT_PTR_EL2_CONNECT]);

    size_t i = 0;
    while (i < *num_beams) {
      beams[i].node_ids[0] = beams32[i * 6 + 0];
      beams[i].node_ids[1] = beams32[i * 6 + 1];
      beams[i].orientation_node_id = beams32[i * 6 + 2];
      beams[i]._null[0] = beams32[i * 6 + 3];
      beams[i]._null[1] = beams32[i * 6 + 4];
      beams[i].material_id = beams32[i * 6 + 5];

      i++;
    }

    free(beams32);
  } else {
    d3_buffer_read_words_at(&plot_file->buffer, beams, 6 * *num_beams,
                            plot_file->data_pointers[D3PLT_PTR_EL2_CONNECT]);
  }

  return beams;
}

d3plot_shell_con *d3plot_read_shell_elements(d3plot_file *plot_file,
                                             size_t *num_shells) {
  if (plot_file->control_data.nel4 == 0) {
    *num_shells = 0;
    return NULL;
  }

  *num_shells = plot_file->control_data.nel4;
  d3plot_shell_con *shells = malloc(*num_shells * sizeof(d3plot_shell_con));
  if (plot_file->buffer.word_size == 4) {
    uint32_t *shells32 = malloc(*num_shells * 5 * sizeof(uint32_t));
    d3_buffer_read_words_at(&plot_file->buffer, shells32, 5 * *num_shells,
                            plot_file->data_pointers[D3PLT_PTR_EL4_CONNECT]);

    size_t i = 0;
    while (i < *num_shells) {
      shells[i].node_ids[0] = shells32[i * 5 + 0];
      shells[i].node_ids[1] = shells32[i * 5 + 1];
      shells[i].node_ids[2] = shells32[i * 5 + 2];
      shells[i].node_ids[3] = shells32[i * 5 + 3];
      shells[i].material_id = shells32[i * 5 + 4];

      i++;
    }

    free(shells32);
  } else {
    d3_buffer_read_words_at(&plot_file->buffer, shells, 5 * *num_shells,
                            plot_file->data_pointers[D3PLT_PTR_EL4_CONNECT]);
  }

  return shells;
}

char *d3plot_read_title(d3plot_file *plot_file) {
  char *title = malloc(10 * plot_file->buffer.word_size + 1);
  /* We never set D3PLT_PTR_TITLE, but because the Title is at position 0 we
   * don't need to*/
  d3_buffer_read_words_at(&plot_file->buffer, title, 10,
                          plot_file->data_pointers[D3PLT_PTR_TITLE]);
  title[10 * plot_file->buffer.word_size] = '\0';
  return title;
}

struct tm *d3plot_read_run_time(d3plot_file *plot_file) {
  d3_word run_time = 0;
  d3_buffer_read_words_at(&plot_file->buffer, &run_time, 1,
                          plot_file->data_pointers[D3PLT_PTR_RUN_TIME]);
  const time_t epoch_time = run_time;

  return localtime(&epoch_time);
}

#define ADD_ELEMENTS_TO_PART(id_func, el_func, el_type, part_num, part_ids)    \
  ids = id_func(plot_file, &num_elements);                                     \
  if (num_elements > 0) {                                                      \
    el_type *els = el_func(plot_file, &num_elements);                          \
                                                                               \
    size_t i = 0;                                                              \
    while (i < num_elements) {                                                 \
      /* For some reason materials in d3plot are parts -_('_')_-*/             \
      if (els[i].material_id == part_index) {                                  \
        part.part_num++;                                                       \
        part.part_ids =                                                        \
            realloc(part.part_ids, part.part_num * sizeof(d3_word));           \
        part.part_ids[part.part_num - 1] = ids[i];                             \
      }                                                                        \
                                                                               \
      i++;                                                                     \
    }                                                                          \
                                                                               \
    free(ids);                                                                 \
    free(els);                                                                 \
  }

d3plot_part d3plot_read_part(d3plot_file *plot_file, size_t part_index) {
  /* Use LS-Dyna's internal index system (Fortran starts by 1)*/
  part_index++;
  d3plot_part part;
  part.solid_ids = NULL;
  part.thick_shell_ids = NULL;
  part.beam_ids = NULL;
  part.shell_ids = NULL;
  part.num_solids = 0;
  part.num_thick_shells = 0;
  part.num_beams = 0;
  part.num_shells = 0;

  size_t num_elements;
  d3_word *ids;

  ADD_ELEMENTS_TO_PART(d3plot_read_solid_element_ids,
                       d3plot_read_solid_elements, d3plot_solid_con, num_solids,
                       solid_ids);
  ADD_ELEMENTS_TO_PART(d3plot_read_thick_shell_element_ids,
                       d3plot_read_thick_shell_elements, d3plot_thick_shell_con,
                       num_thick_shells, thick_shell_ids);
  ADD_ELEMENTS_TO_PART(d3plot_read_beam_element_ids, d3plot_read_beam_elements,
                       d3plot_beam_con, num_beams, beam_ids);
  ADD_ELEMENTS_TO_PART(d3plot_read_shell_element_ids,
                       d3plot_read_shell_elements, d3plot_shell_con, num_shells,
                       shell_ids);

  return part;
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

double *_d3plot_read_node_data(d3plot_file *plot_file, size_t state,
                               size_t *num_nodes, size_t data_type) {
  if (state >= plot_file->num_states) {
    plot_file->error_string = malloc(70);
    sprintf(plot_file->error_string, "%d is out of bounds for the states");
    return NULL;
  }

  *num_nodes = plot_file->control_data.numnp;
  double *coords = malloc(*num_nodes * 3 * sizeof(double));

  if (plot_file->buffer.word_size == 4) {
    float *coords32 = malloc(*num_nodes * 3 * sizeof(float));
    d3_buffer_read_words_at(&plot_file->buffer, coords32, *num_nodes * 3,
                            plot_file->data_pointers[D3PLT_PTR_STATES + state] +
                                plot_file->data_pointers[data_type]);
    size_t i = 0;
    while (i < *num_nodes) {
      coords[i * 3 + 0] = coords32[i * 3 + 0];
      coords[i * 3 + 1] = coords32[i * 3 + 1];
      coords[i * 3 + 2] = coords32[i * 3 + 2];

      i++;
    }

    free(coords32);
  } else {
    d3_buffer_read_words_at(&plot_file->buffer, coords, *num_nodes * 3,
                            plot_file->data_pointers[D3PLT_PTR_STATES + state] +
                                plot_file->data_pointers[data_type]);
  }

  return coords;
}

d3_word *_d3plot_read_ids(d3plot_file *plot_file, size_t *num_ids,
                          size_t data_type, size_t num_ids_value) {
  *num_ids = num_ids_value;
  if (num_ids_value == 0) {
    return NULL;
  }

  d3_word *ids = malloc(*num_ids * sizeof(d3_word));
  if (plot_file->buffer.word_size == 4) {
    uint32_t *ids32 = malloc(*num_ids * plot_file->buffer.word_size);
    d3_buffer_read_words_at(&plot_file->buffer, ids32, *num_ids,
                            plot_file->data_pointers[data_type]);
    size_t i = 0;
    while (i < *num_ids) {
      ids[i + 0] = ids32[i + 0];
      if (i < *num_ids - 1)
        ids[i + 1] = ids32[i + 1];
      if (i < *num_ids - 2)
        ids[i + 2] = ids32[i + 2];
      if (i < *num_ids - 3)
        ids[i + 3] = ids32[i + 3];

      i += 4;
    }

    free(ids32);
  } else {
    d3_buffer_read_words_at(&plot_file->buffer, ids, *num_ids,
                            plot_file->data_pointers[data_type]);
  }

  return ids;
}

#define SWAP(lhs, rhs)                                                         \
  d3_word temp = lhs;                                                          \
  lhs = rhs;                                                                   \
  rhs = temp

d3_word *_insert_sorted(d3_word *dst, size_t dst_size, const d3_word *src,
                        size_t src_size) {
  if (!dst) {
    dst = malloc(src_size * sizeof(d3_word));
    memcpy(dst, src, src_size * sizeof(d3_word));
    return dst;
  }

  const size_t dst_min = dst[0];
  const size_t dst_max = dst[dst_size - 1];
  const size_t src_min = src[0];
  const size_t src_max = src[src_size - 1];

  dst = realloc(dst, (dst_size + src_size) * sizeof(d3_word));

  if (src_max <= dst_min) {
    /* Insert before dst*/
    memcpy(&dst[src_size], dst, dst_size * sizeof(d3_word));
    memcpy(dst, src, src_size * sizeof(d3_word));
  } else if (src_min >= dst_max) {
    /* Insert after dst*/
    memcpy(&dst[dst_size], src, src_size * sizeof(d3_word));
  } else {
    /* Insert inside dst*/
    /* Search for index where i-1 is lesser than min and i is greater than min
     */
    size_t i = 1;
    while (i < dst_size && !(dst[i - 1] < src_min && dst[i] > src_min)) {
      i++;
    }

    memcpy(&dst[i + src_size], &dst[i], (dst_size - i) * sizeof(d3_word));
    memcpy(&dst[i], src, src_size * sizeof(d3_word));
  }

  return dst;
}

void d3plot_free_part(d3plot_part *part) {
  free(part->solid_ids);
  free(part->thick_shell_ids);
  free(part->beam_ids);
  free(part->shell_ids);

  part->solid_ids = NULL;
  part->thick_shell_ids = NULL;
  part->beam_ids = NULL;
  part->shell_ids = NULL;
  part->num_solids = 0;
  part->num_thick_shells = 0;
  part->num_beams = 0;
  part->num_shells = 0;
}