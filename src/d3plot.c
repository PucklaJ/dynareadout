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
#include "binary_search.h"
#include "profiling.h"
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#ifndef inline
#define inline
#endif
#include <time.h>

#define READ_CONTROL_DATA_PLOT_FILE_WORD(value)                                \
  plot_file.control_data.value = 0;                                            \
  d3_buffer_read_words(&plot_file.buffer, &d3_ptr,                             \
                       &plot_file.control_data.value, 1)
#define READ_CONTROL_DATA_PLOT_FILE_SIGNED_WORD(value)                         \
  if (plot_file.buffer.word_size == 4) {                                       \
    int32_t value32;                                                           \
    d3_buffer_read_words(&plot_file.buffer, &d3_ptr, &value32, 1);             \
    CDA.value = value32;                                                       \
  } else {                                                                     \
    d3_buffer_read_words(&plot_file.buffer, &d3_ptr, &CDA.value, 1);           \
  }
#define READ_CONTROL_DATA_WORD(value)                                          \
  d3_word value = 0;                                                           \
  d3_buffer_read_words(&plot_file.buffer, &d3_ptr, &value, 1)
#define CDA plot_file.control_data

#include "d3plot_error_macros.h"

d3plot_file d3plot_open(const char *root_file_name) {
  BEGIN_PROFILE_FUNC();

  d3plot_file plot_file;
  plot_file.error_string = NULL;
  plot_file.data_pointers = NULL;
  plot_file.num_states = 0;
  plot_file.initial_node_coords = NULL;
  plot_file.initial_node_coords_32 = NULL;

  plot_file.buffer = d3_buffer_open(root_file_name);
  if (plot_file.buffer.error_string) {
    /* Swaperoo*/
    plot_file.error_string = plot_file.buffer.error_string;
    plot_file.buffer.error_string = NULL;

    END_PROFILE_FUNC();
    return plot_file;
  }

  /* Allocate the first data pointers*/
  plot_file.data_pointers = malloc(D3PLT_PTR_COUNT * sizeof(size_t));
  memset(plot_file.data_pointers, 0, D3PLT_PTR_COUNT * sizeof(size_t));

  d3_pointer d3_ptr = d3_buffer_seek(&plot_file.buffer, 0);

  d3_buffer_skip_words(&plot_file.buffer, &d3_ptr, 10); /* Title*/
  plot_file.data_pointers[D3PLT_PTR_RUN_TIME] = d3_ptr.cur_word;
  d3_buffer_skip_words(&plot_file.buffer, &d3_ptr, 1); /* Run time*/

  READ_CONTROL_DATA_WORD(file_type);
  if (file_type > 1000) {
    file_type -= 1000;
    /* TODO: all external(users) numbers (Node, Element, Material and Rigid
  Surface Nodes) will be written in I8 format.*/
  }
  /* Quit immediately if this is not a d3plot file*/
  if (file_type != D3_FILE_TYPE_D3PLOT) {
    d3_pointer_close(&plot_file.buffer, &d3_ptr);
    plot_file.error_string = malloc(50);
    sprintf(plot_file.error_string, "Wrong file type: %s",
            _d3plot_get_file_type_name(file_type));

    END_PROFILE_FUNC();
    return plot_file;
  }

  d3_buffer_skip_words(&plot_file.buffer, &d3_ptr, 1); /* TODO: Source version*/
  d3_buffer_skip_words(&plot_file.buffer, &d3_ptr,
                       1); /* TODO: Release version*/
  d3_buffer_skip_words(&plot_file.buffer, &d3_ptr, 1); /* TODO: Version*/
  READ_CONTROL_DATA_PLOT_FILE_WORD(ndim);
  READ_CONTROL_DATA_PLOT_FILE_WORD(numnp);
  READ_CONTROL_DATA_WORD(icode);
  READ_CONTROL_DATA_PLOT_FILE_WORD(nglbv);
  READ_CONTROL_DATA_PLOT_FILE_WORD(it);
  READ_CONTROL_DATA_PLOT_FILE_WORD(iu);
  READ_CONTROL_DATA_PLOT_FILE_WORD(iv);
  READ_CONTROL_DATA_PLOT_FILE_WORD(ia);
  READ_CONTROL_DATA_PLOT_FILE_SIGNED_WORD(nel8);
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
  READ_CONTROL_DATA_PLOT_FILE_SIGNED_WORD(maxint);
  /*READ_CONTROL_DATA_PLOT_FILE_WORD(edlopt); Not used in LS-Dyna?*/
  READ_CONTROL_DATA_PLOT_FILE_WORD(nmsph);
  d3_buffer_skip_words(&plot_file.buffer, &d3_ptr, 1); /* TODO: NGPSPH*/
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
  d3_buffer_skip_words(&plot_file.buffer, &d3_ptr, 1); /* TODO: NCFDV2*/
  READ_CONTROL_DATA_PLOT_FILE_WORD(nadapt);
  READ_CONTROL_DATA_PLOT_FILE_WORD(nmmat);
  d3_buffer_skip_words(&plot_file.buffer, &d3_ptr, 1); /* TODO: NUMFLUID*/
  d3_buffer_skip_words(&plot_file.buffer, &d3_ptr, 1); /* TODO: INN*/
  READ_CONTROL_DATA_WORD(npefg);
  READ_CONTROL_DATA_PLOT_FILE_WORD(nel48);
  READ_CONTROL_DATA_WORD(idtdt);
  READ_CONTROL_DATA_WORD(extra);
  d3_buffer_skip_words(&plot_file.buffer, &d3_ptr, 6); /* TODO: WORDS*/

  uint8_t mattyp;

  if (extra > 0) {
    READ_CONTROL_DATA_PLOT_FILE_WORD(nel20);
    READ_CONTROL_DATA_PLOT_FILE_WORD(nt3d);
    READ_CONTROL_DATA_PLOT_FILE_WORD(nel27);
    READ_CONTROL_DATA_PLOT_FILE_WORD(neipb);
    READ_CONTROL_DATA_PLOT_FILE_WORD(nel21p);
    READ_CONTROL_DATA_PLOT_FILE_WORD(nel15t);
    READ_CONTROL_DATA_PLOT_FILE_WORD(
        soleng); /* NOTE: I don't know what SOLENG > 0 does -_(°_°)_-*/
    READ_CONTROL_DATA_PLOT_FILE_WORD(nel20t);
    READ_CONTROL_DATA_PLOT_FILE_WORD(nel40p);
    READ_CONTROL_DATA_PLOT_FILE_WORD(nel64);
    READ_CONTROL_DATA_PLOT_FILE_WORD(quadr);
    READ_CONTROL_DATA_PLOT_FILE_WORD(cubic);
    READ_CONTROL_DATA_PLOT_FILE_WORD(tsheng);
    READ_CONTROL_DATA_PLOT_FILE_WORD(nbranch);
    READ_CONTROL_DATA_PLOT_FILE_WORD(penout);
    READ_CONTROL_DATA_PLOT_FILE_WORD(engout);
    READ_CONTROL_DATA_PLOT_FILE_WORD(bemeng);
    READ_CONTROL_DATA_PLOT_FILE_WORD(kineng);

    d3_buffer_skip_words(&plot_file.buffer, &d3_ptr, extra - 18);

    if (CDA.quadr > 0 || CDA.cubic > 0) {
      READ_CONTROL_DATA_PLOT_FILE_WORD(npart);
      size_t i = 0;
      while (i < CDA.npart) {
        /* TODO: data of higher order element parts*/
        d3_buffer_skip_words(&plot_file.buffer, &d3_ptr, 9);

        i++;
      }
    } else {
      CDA.npart = 0;
    }
  } else {
    CDA.nel20 = 0;
    CDA.nt3d = 0;
    CDA.nel27 = 0;
    CDA.neipb = 0;
    CDA.nel21p = 0;
    CDA.nel15t = 0;
    CDA.soleng = 0;
    CDA.nel20t = 0;
    CDA.nel40p = 0;
    CDA.nel64 = 0;
    CDA.quadr = 0;
    CDA.cubic = 0;
    CDA.tsheng = 0;
    CDA.nbranch = 0;
    CDA.penout = 0;
    CDA.engout = 0;
    CDA.bemeng = 0;
    CDA.kineng = 0;
    CDA.npart = 0;
  }

  /* Check if an error ocurred somewhere while reading the control data*/
  if (plot_file.buffer.error_string) {
    ERROR_AND_RETURN_F("Failed to read the CONTROL DATA: %s",
                       plot_file.buffer.error_string);
  }

  /* Calculate BEAMIP*/
  /* NV1D = 6 + 5*BEAMIP + NEIPB*(3+BEAMIP) */
  CDA.beamip = (CDA.nv1d - 6 - CDA.neipb * 3) / (5 + CDA.neipb);

  if (CDA.ndim == 5 || CDA.ndim == 7) {
    mattyp = 1;
    CDA.ndim = 3;
    CDA.element_connectivity_packed = 0;
  } else {
    mattyp = 0;
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
    ERROR_AND_RETURN_F("A ndim value of %llu is not supported", CDA.ndim);
  }

  /* Compute IOSHL and IOSOL */
  size_t i = 0;
  while (i < 4) {
    switch (CDA.ioshl[i]) {
    case 1000:
      CDA.ioshl[i] = 1;
      if (i < 2)
        CDA.iosol[i] = 1;
      break;
    case 999:
      CDA.ioshl[i] = 0;
      if (i < 2)
        CDA.iosol[i] = 1;
      break;
    default:
      CDA.ioshl[i] = 0;
      if (i < 2)
        CDA.iosol[i] = 0;
      break;
    }

    i++;
  }

  if (_get_nth_digit(idtdt, 0) == 1) {
    /* TODO*/
    ERROR_AND_RETURN_F("IDTDT (%llu) value is not supported. Docs p12: An "
                       "array of dT/dt values of length NUMNP.",
                       idtdt);
  }
  if (_get_nth_digit(idtdt, 1) == 1) {
    /* TODO*/
    ERROR_AND_RETURN_F(
        "IDTDT (%llu) value is not supported. Docs p12: An array of residual "
        "forces length 3*NUMNP followed by residual moments of length 3*NUMNP.",
        idtdt);
  }
  if (_get_nth_digit(idtdt, 2) == 1) {
    /* TODO*/
    ERROR_AND_RETURN_F(
        "IDTDT (%llu) value is not supported. Docs p12: Plastic strain tensor "
        "is written for each solid and shell after standard element data.",
        idtdt)
  }
  if (_get_nth_digit(idtdt, 3) == 1) {
    /* TODO*/
    ERROR_AND_RETURN_F("IDTDT (%llu) value is not supported. Docs p12: Thermal "
                       "strain tensor is written after standard element data.",
                       idtdt);
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
    ERROR_AND_RETURN_F("Invalid value for MAXINT: %lld", CDA.maxint);
  }

  if (idtdt < 100) {
    /* We need to compute ISTRN*/
    /*ISTRN can only be computed as follows and if NV2D > 0.
      If NV2D-MAXINT*(6*IOSHL(1)+IOSHL(2)+NEIPS)+8*IOSHL(3)+4*IOSHL(4) > 1
      Then ISTRN = 1, else ISTRN = 0

      If ISTRN=1, and NEIPH>=6, last the 6 additional values are the six strain
      components.

      Or NELT > 0
      If NV3DT-MAXINT*(6*IOSHL(1)+IOSHL(2)+NEIPS) > 1
      Then ISTRN = 1, else ISTRN = 0*/
    if (CDA.nv2d > 0) {
      const d3_word rhs =
          CDA.maxint * (6 * CDA.ioshl[0] + CDA.ioshl[1] + CDA.neips) +
          8 * CDA.ioshl[2] + 4 * CDA.ioshl[3];
      if (CDA.nv2d > rhs + 1) {
        CDA.istrn = 1;
      } else {
        CDA.istrn = 0;
      }
    } else if (CDA.nelt > 0) {
      if ((CDA.nv3dt -
           CDA.maxint * (6 * CDA.ioshl[0] + CDA.ioshl[1] + CDA.neips)) > 1) {
        CDA.istrn = 1;
      } else {
        CDA.istrn = 0;
      }
    }
  } else {
    CDA.istrn = _get_nth_digit(idtdt, 4);
  }

  if (icode != D3_CODE_OLD_DYNA3D &&
      icode != D3_CODE_NIKE3D_LS_DYNA3D_LS_NIKE3D) {
    ERROR_AND_RETURN("The given order of the elements is not supported");
  }

  if (CDA.neips > UCHAR_MAX) {
    ERROR_AND_RETURN_F("A value of NEIPS (history variables) greater than %u "
                       "is not supported (%llu > %u)",
                       UCHAR_MAX, CDA.neips, UCHAR_MAX);
  }

  if (CDA.maxint > UCHAR_MAX) {
    ERROR_AND_RETURN_F("A value of MAXINT (number of integration points) "
                       "greater than %u is not supported (%ld > %u)",
                       UCHAR_MAX, CDA.maxint, UCHAR_MAX);
  }

  /* We are done with CONTROL DATA now comes the real data, but first let's say
   * bye bye if the d3plot contains unsupported data*/

  if (mattyp) {
    ERROR_AND_RETURN("MATERIAL TYPE DATA is not supported");
  }
  if (CDA.ialemat) {
    ERROR_AND_RETURN("FLUID MATERIAL ID DATA is not implemented");
  }
  if (CDA.nmsph) {
    ERROR_AND_RETURN(
        "SMOOTH PARTICLE HYDRODYNAMICS ELEMENT DATA FLAGS is not implemented");
  }
  if (npefg) {
    ERROR_AND_RETURN("PARTICLE DATA is not implemented");
  }
  if (CDA.numds != 0) {
    /* TODO: NUMDS*/
    ERROR_AND_RETURN_F(
        "NUMDS (%llu) with a different value than 0 is not supported",
        CDA.numds);
  }
  if (CDA.numst != 0) {
    ERROR_AND_RETURN_F("NUMST (%llu) should be 0", CDA.numst);
  }
  if (CDA.it != 0) {
    /* TODO: IT*/
    ERROR_AND_RETURN_F(
        "IT (%llu) with a different value than 0 is not supported", CDA.it);
  }

  if (!_d3plot_read_geometry_data(&plot_file, &d3_ptr)) {
    END_PROFILE_FUNC();
    return plot_file;
  }

  if (!_d3plot_read_user_identification_numbers(&plot_file, &d3_ptr)) {
    END_PROFILE_FUNC();
    return plot_file;
  }

  if (!_d3plot_read_extra_node_connectivity(&plot_file, &d3_ptr)) {
    END_PROFILE_FUNC();
    return plot_file;
  }

  if (!_d3plot_read_adapted_element_parent_list(&plot_file, &d3_ptr)) {
    END_PROFILE_FUNC();
    return plot_file;
  }

  if (CDA.nmsph > 0) {
    ERROR_AND_RETURN("SMOOTH PARTICLE HYDRODYNAMICS NODE AND "
                     "MATERIAL LIST is not implemented");
  }

  if (npefg > 0) {
    ERROR_AND_RETURN("PARTICLE GEOMETRY DATA is not implemented");
  }

  if (CDA.ndim > 5) {
    ERROR_AND_RETURN("RIGID ROAD SURFACE DATA is not implemented");
  }

  /* Read EOF marker*/
  double eof_marker;
  d3_buffer_read_double_word(&plot_file.buffer, &d3_ptr, &eof_marker);

  if (eof_marker != D3_EOF) {
    ERROR_AND_RETURN_F(
        "Here (before header) 'd3plot':(%zu) should be the EOF marker",
        d3_ptr.cur_word - 1);
  }

  if (!_d3plot_read_header(&plot_file, &d3_ptr)) {
    END_PROFILE_FUNC();
    return plot_file;
  }

  if (CDA.ncfdv1 == 67108864) {
    ERROR_AND_RETURN("EXTRA DATA TYPES is not implemented");
  }

  if (!d3_buffer_next_file(&plot_file.buffer, &d3_ptr)) {
    ERROR_AND_RETURN("Too few files");
  }
  if (plot_file.buffer.error_string) {
    ERROR_AND_RETURN_F("Failed to switch to the next file: %s",
                       plot_file.buffer.error_string);
  }

  /* Here comes the STATE DATA*/

  int result = 1;
  while (result) {
    result = _d3plot_read_state_data(&plot_file, &d3_ptr);
    if (result == 2) {
      if (!d3_buffer_next_file(&plot_file.buffer, &d3_ptr)) {
        break;
      }
      if (plot_file.buffer.error_string) {
        ERROR_AND_RETURN_F("Failed to switch to the next file: %s",
                           plot_file.buffer.error_string);
      }
    }
  }

  END_PROFILE_FUNC();
  return plot_file;
}

void d3plot_close(d3plot_file *plot_file) {
  BEGIN_PROFILE_FUNC();

  d3_buffer_close(&plot_file->buffer);

  free(plot_file->data_pointers);
  free(plot_file->error_string);
  free(plot_file->initial_node_coords);
  free(plot_file->initial_node_coords_32);

  plot_file->num_states = 0;
  plot_file->error_string = NULL;

  END_PROFILE_FUNC();
}

d3_word *d3plot_read_node_ids(d3plot_file *plot_file, size_t *num_ids) {
  BEGIN_PROFILE_FUNC();

  d3_word *ids = _d3plot_read_ids(plot_file, num_ids, D3PLT_PTR_NODE_IDS,
                                  plot_file->control_data.numnp);

  END_PROFILE_FUNC();
  return ids;
}

d3_word *d3plot_read_solid_element_ids(d3plot_file *plot_file,
                                       size_t *num_ids) {
  BEGIN_PROFILE_FUNC();

  d3_word *ids = _d3plot_read_ids(plot_file, num_ids, D3PLT_PTR_EL8_IDS,
                                  plot_file->control_data.nel8);

  END_PROFILE_FUNC();
  return ids;
}

d3_word *d3plot_read_beam_element_ids(d3plot_file *plot_file, size_t *num_ids) {
  BEGIN_PROFILE_FUNC();

  d3_word *ids = _d3plot_read_ids(plot_file, num_ids, D3PLT_PTR_EL2_IDS,
                                  plot_file->control_data.nel2);

  END_PROFILE_FUNC();
  return ids;
}

d3_word *d3plot_read_shell_element_ids(d3plot_file *plot_file,
                                       size_t *num_ids) {
  BEGIN_PROFILE_FUNC();

  d3_word *ids = _d3plot_read_ids(plot_file, num_ids, D3PLT_PTR_EL4_IDS,
                                  plot_file->control_data.nel4);

  END_PROFILE_FUNC();
  return ids;
}

d3_word *d3plot_read_thick_shell_element_ids(d3plot_file *plot_file,
                                             size_t *num_ids) {
  BEGIN_PROFILE_FUNC();

  d3_word *ids = _d3plot_read_ids(plot_file, num_ids, D3PLT_PTR_ELT_IDS,
                                  plot_file->control_data.nelt);

  END_PROFILE_FUNC();
  return ids;
}

d3_word *d3plot_read_all_element_ids(d3plot_file *plot_file, size_t *num_ids) {
  BEGIN_PROFILE_FUNC();

  d3_word *all_ids = NULL;
  *num_ids = 0;

  size_t num_ids_buffer;
  d3_word *ids_buffer =
      d3plot_read_solid_element_ids(plot_file, &num_ids_buffer);
  /* If an error occurs when trying to read the element ids, those specific
   * elements will be ignored*/
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

  END_PROFILE_FUNC();
  return all_ids;
}

d3_word *d3plot_read_part_ids(d3plot_file *plot_file, size_t *num_parts) {
  BEGIN_PROFILE_FUNC();
  D3PLOT_CLEAR_ERROR_STRING();

  if (plot_file->data_pointers[D3PLT_PTR_PART_IDS] == 0) {
    if (plot_file->data_pointers[D3PLT_PTR_PART_TITLES] == 0) {
      ERROR_AND_NO_RETURN_PTR("Could not retrieve part ids");
      *num_parts = 0;

      END_PROFILE_FUNC();
      return NULL;
    }

    /* Read the part ids from the header if NSORT >= 0*/
    *num_parts = plot_file->control_data.nmmat;
    d3_word *part_ids = malloc(*num_parts * sizeof(d3_word));

    d3_pointer d3_ptr;

    size_t i = 0;
    while (i < *num_parts) {
      part_ids[i] = 0;

      if (i == 0) {
        d3_ptr = d3_buffer_read_words_at(
            &plot_file->buffer, &part_ids[i], 1,
            plot_file->data_pointers[D3PLT_PTR_PART_TITLES]);
      } else {
        d3_buffer_read_words(&plot_file->buffer, &d3_ptr, &part_ids[i], 1);
      }
      d3_buffer_skip_bytes(&plot_file->buffer, &d3_ptr, 72);

      i++;
    }

    d3_pointer_close(&plot_file->buffer, &d3_ptr);
    END_PROFILE_FUNC();
    return part_ids;
  }

  d3_word *ids = _d3plot_read_ids(plot_file, num_parts, D3PLT_PTR_PART_IDS,
                                  plot_file->control_data.nmmat);

  END_PROFILE_FUNC();
  return ids;
}

char **d3plot_read_part_titles(d3plot_file *plot_file, size_t *num_parts) {
  BEGIN_PROFILE_FUNC();
  D3PLOT_CLEAR_ERROR_STRING();

  *num_parts = plot_file->control_data.nmmat;
  char **part_titles = malloc(*num_parts * sizeof(char *));

  d3_pointer d3_ptr;

  size_t i = 0;
  while (i < *num_parts) {
    part_titles[i] = malloc(72 + 1);
    if (i == 0)
      /* PTITLE is always 72 bytes. So we need to divide by to get the correct
       * number of words*/
      d3_ptr = d3_buffer_read_words_at(
          &plot_file->buffer, part_titles[i],
          18 / (plot_file->buffer.word_size == 8 ? 2 : 1),
          plot_file->data_pointers[D3PLT_PTR_PART_TITLES] + 1);
    else {
      d3_buffer_skip_words(&plot_file->buffer, &d3_ptr, 1);
      d3_buffer_read_words(&plot_file->buffer, &d3_ptr, part_titles[i],
                           18 / (plot_file->buffer.word_size == 8 ? 2 : 1));
    }

    if (plot_file->buffer.error_string) {
      d3_pointer_close(&plot_file->buffer, &d3_ptr);
      ERROR_AND_NO_RETURN_F_PTR("Failed to read words: %s",
                                plot_file->buffer.error_string);
      size_t j = 0;
      while (j <= i) {
        free(part_titles[j]);
        j++;
      }
      free(part_titles);
      *num_parts = 0;

      END_PROFILE_FUNC();
      return NULL;
    }

    part_titles[i][72] = '\0';

    i++;
  }

  d3_pointer_close(&plot_file->buffer, &d3_ptr);
  END_PROFILE_FUNC();
  return part_titles;
}

double *d3plot_read_node_coordinates(d3plot_file *plot_file, size_t state,
                                     size_t *num_nodes) {
  BEGIN_PROFILE_FUNC();

  double *data = _d3plot_read_node_data(plot_file, state, num_nodes,
                                        D3PLT_PTR_STATE_NODE_COORDS);
  if (plot_file->error_string) {
    END_PROFILE_FUNC();
    return data;
  }

  if (plot_file->control_data.iu == 2) {
    if (!plot_file->initial_node_coords) {
      *num_nodes = plot_file->control_data.numnp;
      plot_file->initial_node_coords = malloc(*num_nodes * 3 * sizeof(double));

      if (plot_file->buffer.word_size == 8) {
        d3_pointer d3_ptr = d3_buffer_read_words_at(
            &plot_file->buffer, plot_file->initial_node_coords, *num_nodes * 3,
            plot_file->data_pointers[D3PLT_PTR_NODE_COORDS]);
        d3_pointer_close(&plot_file->buffer, &d3_ptr);

        if (plot_file->buffer.error_string) {
          free(data);
          free(plot_file->initial_node_coords);
          plot_file->initial_node_coords = NULL;

          ERROR_AND_NO_RETURN_F_PTR("failed to read initial node coords: %s",
                                    plot_file->buffer.error_string);
          END_PROFILE_FUNC();
          return NULL;
        }
      } else {
        if (!plot_file->initial_node_coords_32) {
          plot_file->initial_node_coords_32 =
              malloc(*num_nodes * 3 * sizeof(float));

          d3_pointer d3_ptr = d3_buffer_read_words_at(
              &plot_file->buffer, plot_file->initial_node_coords_32,
              *num_nodes * 3, plot_file->data_pointers[D3PLT_PTR_NODE_COORDS]);
          d3_pointer_close(&plot_file->buffer, &d3_ptr);

          if (plot_file->buffer.error_string) {
            free(data);
            free(plot_file->initial_node_coords);
            free(plot_file->initial_node_coords_32);
            plot_file->initial_node_coords = NULL;
            plot_file->initial_node_coords_32 = NULL;

            ERROR_AND_NO_RETURN_F_PTR("failed to read initial node coords: %s",
                                      plot_file->buffer.error_string);
            END_PROFILE_FUNC();
            return NULL;
          }
        }

        size_t i = 0;
        while (i < *num_nodes * 3) {
          plot_file->initial_node_coords[i + 0] =
              plot_file->initial_node_coords_32[i + 0];
          plot_file->initial_node_coords[i + 1] =
              plot_file->initial_node_coords_32[i + 1];
          plot_file->initial_node_coords[i + 2] =
              plot_file->initial_node_coords_32[i + 2];

          i += 3;
        }
      }

      size_t i = 0;
      while (i < *num_nodes) {
        data[i + 0] += plot_file->initial_node_coords[i + 0];
        data[i + 1] += plot_file->initial_node_coords[i + 1];
        data[i + 2] += plot_file->initial_node_coords[i + 2];

        i += 3;
      }
    }
  }

  END_PROFILE_FUNC();
  return data;
}

double *d3plot_read_all_node_coordinates(d3plot_file *plot_file,
                                         size_t *num_nodes,
                                         size_t *num_time_steps) {
  BEGIN_PROFILE_FUNC();

  if (plot_file->buffer.word_size == 4) {
    float *big_data32 = d3plot_read_all_node_coordinates_32(
        plot_file, num_nodes, num_time_steps);
    if (plot_file->error_string) {
      END_PROFILE_FUNC();
      return NULL;
    }

    const size_t num_values = *num_nodes * *num_time_steps * 3;

    double *big_data = malloc(num_values * sizeof(double));

    size_t i = 0;
    while (i < num_values) {
      big_data[i + 0] = big_data32[i + 0];
      big_data[i + 1] = big_data32[i + 1];
      big_data[i + 2] = big_data32[i + 2];

      i += 3;
    }
    free(big_data32);

    END_PROFILE_FUNC();
    return big_data;
  }

  D3PLOT_CLEAR_ERROR_STRING();

  *num_time_steps = plot_file->num_states;
  *num_nodes = (size_t)plot_file->control_data.numnp;

  double *big_data = malloc(*num_nodes * *num_time_steps * 3 * sizeof(double));

  size_t current_pointer = 0;
  size_t t = 0;
  while (t < *num_time_steps) {
    d3_pointer d3_ptr = d3_buffer_read_words_at(
        &plot_file->buffer, &big_data[current_pointer], *num_nodes * 3,
        plot_file->data_pointers[D3PLT_PTR_STATES + t] +
            plot_file->data_pointers[D3PLT_PTR_STATE_NODE_COORDS]);
    d3_pointer_close(&plot_file->buffer, &d3_ptr);
    if (plot_file->buffer.error_string) {
      ERROR_AND_NO_RETURN_F_PTR("Failed to read words: %s",
                                plot_file->buffer.error_string);
      *num_nodes = 0;
      *num_time_steps = 0;
      free(big_data);
      END_PROFILE_FUNC();
      return NULL;
    }

    current_pointer += *num_nodes * 3;
    t++;
  }

  END_PROFILE_FUNC();
  return big_data;
}

double *d3plot_read_node_velocity(d3plot_file *plot_file, size_t state,
                                  size_t *num_nodes) {
  BEGIN_PROFILE_FUNC();

  double *data = _d3plot_read_node_data(plot_file, state, num_nodes,
                                        D3PLT_PTR_STATE_NODE_VEL);

  END_PROFILE_FUNC();
  return data;
}

double *d3plot_read_all_node_velocity(d3plot_file *plot_file, size_t *num_nodes,
                                      size_t *num_time_steps) {
  BEGIN_PROFILE_FUNC();

  if (plot_file->buffer.word_size == 4) {
    float *big_data32 =
        d3plot_read_all_node_velocity_32(plot_file, num_nodes, num_time_steps);
    if (plot_file->error_string) {
      END_PROFILE_FUNC();
      return NULL;
    }

    const size_t num_values = *num_nodes * *num_time_steps * 3;

    double *big_data = malloc(num_values * sizeof(double));

    size_t i = 0;
    while (i < num_values) {
      big_data[i + 0] = big_data32[i + 0];
      big_data[i + 1] = big_data32[i + 1];
      big_data[i + 2] = big_data32[i + 2];

      i += 3;
    }
    free(big_data32);

    END_PROFILE_FUNC();
    return big_data;
  }

  D3PLOT_CLEAR_ERROR_STRING();

  *num_time_steps = plot_file->num_states;
  *num_nodes = (size_t)plot_file->control_data.numnp;

  double *big_data = malloc(*num_nodes * *num_time_steps * 3 * sizeof(double));

  size_t current_pointer = 0;
  size_t t = 0;
  while (t < *num_time_steps) {
    d3_pointer d3_ptr = d3_buffer_read_words_at(
        &plot_file->buffer, &big_data[current_pointer], *num_nodes * 3,
        plot_file->data_pointers[D3PLT_PTR_STATES + t] +
            plot_file->data_pointers[D3PLT_PTR_STATE_NODE_VEL]);
    d3_pointer_close(&plot_file->buffer, &d3_ptr);
    if (plot_file->buffer.error_string) {
      ERROR_AND_NO_RETURN_F_PTR("Failed to read words: %s",
                                plot_file->buffer.error_string);
      *num_nodes = 0;
      *num_time_steps = 0;
      free(big_data);
      END_PROFILE_FUNC();
      return NULL;
    }

    current_pointer += *num_nodes * 3;
    t++;
  }

  END_PROFILE_FUNC();
  return big_data;
}

double *d3plot_read_node_acceleration(d3plot_file *plot_file, size_t state,
                                      size_t *num_nodes) {
  BEGIN_PROFILE_FUNC();

  double *data = _d3plot_read_node_data(plot_file, state, num_nodes,
                                        D3PLT_PTR_STATE_NODE_ACC);

  END_PROFILE_FUNC();
  return data;
}

double *d3plot_read_all_node_acceleration(d3plot_file *plot_file,
                                          size_t *num_nodes,
                                          size_t *num_time_steps) {
  BEGIN_PROFILE_FUNC();

  if (plot_file->buffer.word_size == 4) {
    float *big_data32 = d3plot_read_all_node_acceleration_32(
        plot_file, num_nodes, num_time_steps);
    if (plot_file->error_string) {
      END_PROFILE_FUNC();
      return NULL;
    }

    const size_t num_values = *num_nodes * *num_time_steps * 3;

    double *big_data = malloc(num_values * sizeof(double));

    size_t i = 0;
    while (i < num_values) {
      big_data[i + 0] = big_data32[i + 0];
      big_data[i + 1] = big_data32[i + 1];
      big_data[i + 2] = big_data32[i + 2];

      i += 3;
    }
    free(big_data32);

    END_PROFILE_FUNC();
    return big_data;
  }

  D3PLOT_CLEAR_ERROR_STRING();

  *num_time_steps = plot_file->num_states;
  *num_nodes = (size_t)plot_file->control_data.numnp;

  double *big_data = malloc(*num_nodes * *num_time_steps * 3 * sizeof(double));

  size_t current_pointer = 0;
  size_t t = 0;
  while (t < *num_time_steps) {
    d3_pointer d3_ptr = d3_buffer_read_words_at(
        &plot_file->buffer, &big_data[current_pointer], *num_nodes * 3,
        plot_file->data_pointers[D3PLT_PTR_STATES + t] +
            plot_file->data_pointers[D3PLT_PTR_STATE_NODE_ACC]);
    d3_pointer_close(&plot_file->buffer, &d3_ptr);
    if (plot_file->buffer.error_string) {
      ERROR_AND_NO_RETURN_F_PTR("Failed to read words: %s",
                                plot_file->buffer.error_string);
      *num_nodes = 0;
      *num_time_steps = 0;
      free(big_data);
      END_PROFILE_FUNC();
      return NULL;
    }

    current_pointer += *num_nodes * 3;
    t++;
  }

  END_PROFILE_FUNC();
  return big_data;
}

float *d3plot_read_node_coordinates_32(d3plot_file *plot_file, size_t state,
                                       size_t *num_nodes) {
  BEGIN_PROFILE_FUNC();

  float *data = _d3plot_read_node_data_32(plot_file, state, num_nodes,
                                          D3PLT_PTR_STATE_NODE_COORDS);
  if (plot_file->error_string) {
    END_PROFILE_FUNC();
    return data;
  }

  if (plot_file->control_data.iu == 2) {
    if (!plot_file->initial_node_coords_32) {
      *num_nodes = plot_file->control_data.numnp;
      plot_file->initial_node_coords_32 =
          malloc(*num_nodes * 3 * sizeof(float));

      if (plot_file->buffer.word_size == 4) {
        d3_pointer d3_ptr = d3_buffer_read_words_at(
            &plot_file->buffer, plot_file->initial_node_coords_32,
            *num_nodes * 3, plot_file->data_pointers[D3PLT_PTR_NODE_COORDS]);
        d3_pointer_close(&plot_file->buffer, &d3_ptr);

        if (plot_file->buffer.error_string) {
          free(data);
          free(plot_file->initial_node_coords_32);
          plot_file->initial_node_coords_32 = NULL;

          ERROR_AND_NO_RETURN_F_PTR("failed to read initial node coords: %s",
                                    plot_file->buffer.error_string);
          END_PROFILE_FUNC();
          return NULL;
        }
      } else {
        if (!plot_file->initial_node_coords) {
          plot_file->initial_node_coords =
              malloc(*num_nodes * 3 * sizeof(double));

          d3_pointer d3_ptr = d3_buffer_read_words_at(
              &plot_file->buffer, plot_file->initial_node_coords,
              *num_nodes * 3, plot_file->data_pointers[D3PLT_PTR_NODE_COORDS]);
          d3_pointer_close(&plot_file->buffer, &d3_ptr);

          if (plot_file->buffer.error_string) {
            free(data);
            free(plot_file->initial_node_coords_32);
            free(plot_file->initial_node_coords);
            plot_file->initial_node_coords_32 = NULL;
            plot_file->initial_node_coords = NULL;

            ERROR_AND_NO_RETURN_F_PTR("failed to read initial node coords: %s",
                                      plot_file->buffer.error_string);
            END_PROFILE_FUNC();
            return NULL;
          }
        }

        size_t i = 0;
        while (i < *num_nodes * 3) {
          plot_file->initial_node_coords_32[i + 0] =
              plot_file->initial_node_coords[i + 0];
          plot_file->initial_node_coords_32[i + 1] =
              plot_file->initial_node_coords[i + 1];
          plot_file->initial_node_coords_32[i + 2] =
              plot_file->initial_node_coords[i + 2];

          i += 3;
        }
      }

      size_t i = 0;
      while (i < *num_nodes) {
        data[i + 0] += plot_file->initial_node_coords_32[i + 0];
        data[i + 1] += plot_file->initial_node_coords_32[i + 1];
        data[i + 2] += plot_file->initial_node_coords_32[i + 2];

        i += 3;
      }
    }
  }

  END_PROFILE_FUNC();
  return data;
}

float *d3plot_read_all_node_coordinates_32(d3plot_file *plot_file,
                                           size_t *num_nodes,
                                           size_t *num_time_steps) {
  BEGIN_PROFILE_FUNC();

  if (plot_file->buffer.word_size == 8) {
    double *big_data64 =
        d3plot_read_all_node_coordinates(plot_file, num_nodes, num_time_steps);
    if (plot_file->error_string) {
      END_PROFILE_FUNC();
      return NULL;
    }

    const size_t num_values = *num_nodes * *num_time_steps * 3;

    float *big_data = malloc(num_values * sizeof(float));

    size_t i = 0;
    while (i < num_values) {
      big_data[i + 0] = big_data64[i + 0];
      big_data[i + 1] = big_data64[i + 1];
      big_data[i + 2] = big_data64[i + 2];

      i += 3;
    }
    free(big_data64);

    END_PROFILE_FUNC();
    return big_data;
  }

  D3PLOT_CLEAR_ERROR_STRING();

  *num_time_steps = plot_file->num_states;
  *num_nodes = (size_t)plot_file->control_data.numnp;

  float *big_data = malloc(*num_nodes * *num_time_steps * 3 * sizeof(float));

  size_t current_pointer = 0;
  size_t t = 0;
  while (t < *num_time_steps) {
    d3_pointer d3_ptr = d3_buffer_read_words_at(
        &plot_file->buffer, &big_data[current_pointer], *num_nodes * 3,
        plot_file->data_pointers[D3PLT_PTR_STATES + t] +
            plot_file->data_pointers[D3PLT_PTR_STATE_NODE_COORDS]);
    d3_pointer_close(&plot_file->buffer, &d3_ptr);
    if (plot_file->buffer.error_string) {
      ERROR_AND_NO_RETURN_F_PTR("Failed to read words: %s",
                                plot_file->buffer.error_string);
      *num_nodes = 0;
      *num_time_steps = 0;
      free(big_data);
      END_PROFILE_FUNC();
      return NULL;
    }

    current_pointer += *num_nodes * 3;
    t++;
  }

  END_PROFILE_FUNC();
  return big_data;
}

float *d3plot_read_node_velocity_32(d3plot_file *plot_file, size_t state,
                                    size_t *num_nodes) {
  BEGIN_PROFILE_FUNC();

  float *data = _d3plot_read_node_data_32(plot_file, state, num_nodes,
                                          D3PLT_PTR_STATE_NODE_VEL);

  END_PROFILE_FUNC();
  return data;
}

float *d3plot_read_all_node_velocity_32(d3plot_file *plot_file,
                                        size_t *num_nodes,
                                        size_t *num_time_steps) {
  BEGIN_PROFILE_FUNC();

  if (plot_file->buffer.word_size == 8) {
    double *big_data64 =
        d3plot_read_all_node_velocity(plot_file, num_nodes, num_time_steps);
    if (plot_file->error_string) {
      END_PROFILE_FUNC();
      return NULL;
    }

    const size_t num_values = *num_nodes * *num_time_steps * 3;

    float *big_data = malloc(num_values * sizeof(float));

    size_t i = 0;
    while (i < num_values) {
      big_data[i + 0] = big_data64[i + 0];
      big_data[i + 1] = big_data64[i + 1];
      big_data[i + 2] = big_data64[i + 2];

      i += 3;
    }
    free(big_data64);

    END_PROFILE_FUNC();
    return big_data;
  }

  D3PLOT_CLEAR_ERROR_STRING();

  *num_time_steps = plot_file->num_states;
  *num_nodes = (size_t)plot_file->control_data.numnp;

  float *big_data = malloc(*num_nodes * *num_time_steps * 3 * sizeof(float));

  size_t current_pointer = 0;
  size_t t = 0;
  while (t < *num_time_steps) {
    d3_pointer d3_ptr = d3_buffer_read_words_at(
        &plot_file->buffer, &big_data[current_pointer], *num_nodes * 3,
        plot_file->data_pointers[D3PLT_PTR_STATES + t] +
            plot_file->data_pointers[D3PLT_PTR_STATE_NODE_VEL]);
    d3_pointer_close(&plot_file->buffer, &d3_ptr);
    if (plot_file->buffer.error_string) {
      ERROR_AND_NO_RETURN_F_PTR("Failed to read words: %s",
                                plot_file->buffer.error_string);
      *num_nodes = 0;
      *num_time_steps = 0;
      free(big_data);
      END_PROFILE_FUNC();
      return NULL;
    }

    current_pointer += *num_nodes * 3;
    t++;
  }

  END_PROFILE_FUNC();
  return big_data;
}

float *d3plot_read_node_acceleration_32(d3plot_file *plot_file, size_t state,
                                        size_t *num_nodes) {
  BEGIN_PROFILE_FUNC();

  float *data = _d3plot_read_node_data_32(plot_file, state, num_nodes,
                                          D3PLT_PTR_STATE_NODE_ACC);

  END_PROFILE_FUNC();
  return data;
}

float *d3plot_read_all_node_acceleration_32(d3plot_file *plot_file,
                                            size_t *num_nodes,
                                            size_t *num_time_steps) {
  BEGIN_PROFILE_FUNC();

  if (plot_file->buffer.word_size == 8) {
    double *big_data64 =
        d3plot_read_all_node_acceleration(plot_file, num_nodes, num_time_steps);
    if (plot_file->error_string) {
      END_PROFILE_FUNC();
      return NULL;
    }

    const size_t num_values = *num_nodes * *num_time_steps * 3;

    float *big_data = malloc(num_values * sizeof(float));

    size_t i = 0;
    while (i < num_values) {
      big_data[i + 0] = big_data64[i + 0];
      big_data[i + 1] = big_data64[i + 1];
      big_data[i + 2] = big_data64[i + 2];

      i += 3;
    }
    free(big_data64);

    END_PROFILE_FUNC();
    return big_data;
  }

  D3PLOT_CLEAR_ERROR_STRING();

  *num_time_steps = plot_file->num_states;
  *num_nodes = (size_t)plot_file->control_data.numnp;

  float *big_data = malloc(*num_nodes * *num_time_steps * 3 * sizeof(float));

  size_t current_pointer = 0;
  size_t t = 0;
  while (t < *num_time_steps) {
    d3_pointer d3_ptr = d3_buffer_read_words_at(
        &plot_file->buffer, &big_data[current_pointer], *num_nodes * 3,
        plot_file->data_pointers[D3PLT_PTR_STATES + t] +
            plot_file->data_pointers[D3PLT_PTR_STATE_NODE_ACC]);
    d3_pointer_close(&plot_file->buffer, &d3_ptr);
    if (plot_file->buffer.error_string) {
      ERROR_AND_NO_RETURN_F_PTR("Failed to read words: %s",
                                plot_file->buffer.error_string);
      *num_nodes = 0;
      *num_time_steps = 0;
      free(big_data);
      END_PROFILE_FUNC();
      return NULL;
    }

    current_pointer += *num_nodes * 3;
    t++;
  }

  END_PROFILE_FUNC();
  return big_data;
}

double d3plot_read_time(d3plot_file *plot_file, size_t state) {
  BEGIN_PROFILE_FUNC();
  D3PLOT_CLEAR_ERROR_STRING();

  if (state >= plot_file->num_states) {
    ERROR_AND_NO_RETURN_F_PTR("%zu is out of bounds for the states", state);

    END_PROFILE_FUNC();
    return -1.0;
  }

  double time;
  if (plot_file->buffer.word_size == 4) {
    float time32;
    d3_pointer d3_ptr = d3_buffer_read_words_at(
        &plot_file->buffer, &time32, 1,
        plot_file->data_pointers[D3PLT_PTR_STATES + state] +
            plot_file->data_pointers[D3PLT_PTR_STATE_TIME]);
    d3_pointer_close(&plot_file->buffer, &d3_ptr);
    time = (double)time32;
  } else {
    d3_pointer d3_ptr = d3_buffer_read_words_at(
        &plot_file->buffer, &time, 1,
        plot_file->data_pointers[D3PLT_PTR_STATES + state] +
            plot_file->data_pointers[D3PLT_PTR_STATE_TIME]);
    d3_pointer_close(&plot_file->buffer, &d3_ptr);
  }

  if (plot_file->buffer.error_string) {
    ERROR_AND_NO_RETURN_F_PTR("Failed to read words: %s",
                              plot_file->buffer.error_string);

    END_PROFILE_FUNC();
    return -1.0;
  }

  END_PROFILE_FUNC();
  return time;
}

double *d3plot_read_all_time(d3plot_file *plot_file, size_t *num_states) {
  BEGIN_PROFILE_FUNC();
  D3PLOT_CLEAR_ERROR_STRING();

  *num_states = plot_file->num_states;
  double *times = malloc(plot_file->num_states * sizeof(double));

  if (plot_file->buffer.word_size == 4) {
    float time32;

    size_t i = 0;
    while (i < plot_file->num_states) {
      d3_pointer d3_ptr = d3_buffer_read_words_at(
          &plot_file->buffer, &time32, 1,
          plot_file->data_pointers[D3PLT_PTR_STATES + i] +
              plot_file->data_pointers[D3PLT_PTR_STATE_TIME]);
      d3_pointer_close(&plot_file->buffer, &d3_ptr);
      if (plot_file->buffer.error_string) {
        ERROR_AND_NO_RETURN_F_PTR("Failed to read words: %s",
                                  plot_file->buffer.error_string);
        *num_states = 0;
        free(times);
        times = NULL;
        break;
      }

      times[i] = (double)time32;

      i++;
    }
  } else {
    size_t i = 0;
    while (i < plot_file->num_states) {
      d3_pointer d3_ptr = d3_buffer_read_words_at(
          &plot_file->buffer, &times[i], 1,
          plot_file->data_pointers[D3PLT_PTR_STATES + i] +
              plot_file->data_pointers[D3PLT_PTR_STATE_TIME]);
      d3_pointer_close(&plot_file->buffer, &d3_ptr);
      if (plot_file->buffer.error_string) {
        ERROR_AND_NO_RETURN_F_PTR("Failed to read words: %s",
                                  plot_file->buffer.error_string);
        *num_states = 0;
        free(times);
        times = NULL;
        break;
      }

      i++;
    }
  }

  END_PROFILE_FUNC();
  return times;
}

float d3plot_read_time_32(d3plot_file *plot_file, size_t state) {
  BEGIN_PROFILE_FUNC();
  D3PLOT_CLEAR_ERROR_STRING();

  if (state >= plot_file->num_states) {
    ERROR_AND_NO_RETURN_F_PTR("%zu is out of bounds for the states", state);

    END_PROFILE_FUNC();
    return -1.0f;
  }

  float time;
  if (plot_file->buffer.word_size == 8) {
    double time64;
    d3_pointer d3_ptr = d3_buffer_read_words_at(
        &plot_file->buffer, &time64, 1,
        plot_file->data_pointers[D3PLT_PTR_STATES + state] +
            plot_file->data_pointers[D3PLT_PTR_STATE_TIME]);
    d3_pointer_close(&plot_file->buffer, &d3_ptr);
    time = (float)time64;
  } else {
    d3_pointer d3_ptr = d3_buffer_read_words_at(
        &plot_file->buffer, &time, 1,
        plot_file->data_pointers[D3PLT_PTR_STATES + state] +
            plot_file->data_pointers[D3PLT_PTR_STATE_TIME]);
    d3_pointer_close(&plot_file->buffer, &d3_ptr);
  }

  if (plot_file->buffer.error_string) {
    ERROR_AND_NO_RETURN_F_PTR("Failed to read words: %s",
                              plot_file->buffer.error_string);

    END_PROFILE_FUNC();
    return -1.0f;
  }

  END_PROFILE_FUNC();
  return time;
}

float *d3plot_read_all_time_32(d3plot_file *plot_file, size_t *num_states) {
  BEGIN_PROFILE_FUNC();
  D3PLOT_CLEAR_ERROR_STRING();

  *num_states = plot_file->num_states;
  float *times = malloc(plot_file->num_states * sizeof(float));

  if (plot_file->buffer.word_size == 8) {
    double time64;

    size_t i = 0;
    while (i < plot_file->num_states) {
      d3_pointer d3_ptr = d3_buffer_read_words_at(
          &plot_file->buffer, &time64, 1,
          plot_file->data_pointers[D3PLT_PTR_STATES + i] +
              plot_file->data_pointers[D3PLT_PTR_STATE_TIME]);
      d3_pointer_close(&plot_file->buffer, &d3_ptr);
      if (plot_file->buffer.error_string) {
        ERROR_AND_NO_RETURN_F_PTR("Failed to read words: %s",
                                  plot_file->buffer.error_string);
        *num_states = 0;
        free(times);
        times = NULL;
        break;
      }

      times[i] = (float)time64;

      i++;
    }
  } else {
    size_t i = 0;
    while (i < plot_file->num_states) {
      d3_pointer d3_ptr = d3_buffer_read_words_at(
          &plot_file->buffer, &times[i], 1,
          plot_file->data_pointers[D3PLT_PTR_STATES + i] +
              plot_file->data_pointers[D3PLT_PTR_STATE_TIME]);
      d3_pointer_close(&plot_file->buffer, &d3_ptr);
      if (plot_file->buffer.error_string) {
        ERROR_AND_NO_RETURN_F_PTR("Failed to read words: %s",
                                  plot_file->buffer.error_string);
        *num_states = 0;
        free(times);
        times = NULL;
        break;
      }

      i++;
    }
  }

  END_PROFILE_FUNC();
  return times;
}

d3plot_solid *d3plot_read_solids_state(d3plot_file *plot_file, size_t state,
                                       size_t *num_solids) {
  BEGIN_PROFILE_FUNC();
  D3PLOT_CLEAR_ERROR_STRING();

  *num_solids = plot_file->control_data.nel8;
  if (*num_solids == 0) {
    END_PROFILE_FUNC();
    return NULL;
  }

  if (state >= plot_file->num_states) {
    ERROR_AND_NO_RETURN_F_PTR("%zu is out of bounds for the states", state);
    *num_solids = 0;

    END_PROFILE_FUNC();
    return NULL;
  }

  d3plot_solid *solids = malloc(*num_solids * sizeof(d3plot_solid));
  if (plot_file->buffer.word_size == 4) {
    float *data =
        malloc((plot_file->control_data.nel8 * plot_file->control_data.nv3d) *
               sizeof(float));

    d3_pointer d3_ptr = d3_buffer_read_words_at(
        &plot_file->buffer, data,
        plot_file->control_data.nel8 * plot_file->control_data.nv3d,
        plot_file->data_pointers[D3PLT_PTR_STATES + state] +
            plot_file->data_pointers[D3PLT_PTR_STATE_ELEMENT_SOLID]);
    d3_pointer_close(&plot_file->buffer, &d3_ptr);
    if (plot_file->buffer.error_string) {
      ERROR_AND_NO_RETURN_F_PTR("Failed to read words: %s",
                                plot_file->buffer.error_string);
      *num_solids = 0;
      free(data);
      free(solids);

      END_PROFILE_FUNC();
      return NULL;
    }

    size_t i = 0;
    size_t o = 0;
    while (i < *num_solids) {
      /* Docs: page 33*/
      if (plot_file->control_data.iosol[0]) {
        /* 1. Sigma-x (true stress in the global system)*/
        solids[i].sigma.x = data[o++];
        /* 2. Sigma-y*/
        solids[i].sigma.y = data[o++];
        /* 3. Sigma-z*/
        solids[i].sigma.z = data[o++];
        /* 4. Sigma-xy*/
        solids[i].sigma.xy = data[o++];
        /* 5. Sigma-yz*/
        solids[i].sigma.yz = data[o++];
        /* 6. Sigma-zx*/
        solids[i].sigma.zx = data[o++];
      } else {
        memset(&solids[i].sigma, 0, sizeof(d3plot_tensor));
      }

      if (plot_file->control_data.iosol[1]) {
        /* 7. Effective plastic strain or material dependent variable*/
        solids[i].effective_plastic_strain = data[o++];
      } else {
        solids[i].effective_plastic_strain = 0.0;
      }

      /* Docs p12: If ISTRN=1, and NEIPH>=6, last the 6 additional values are
       * the six strain components. */
      if (plot_file->control_data.istrn == 1 &&
          plot_file->control_data.neiph >= 6) {
        /* We need -1 since we start by 0 and in the docs they start with
         * 1*/
        /* 7+NEIPH-5. Epsilon-x*/
        solids[i].epsilon.x = data[o + plot_file->control_data.neiph - 5 - 1];
        /* 7+NEIPH-4. Epsilon-y*/
        solids[i].epsilon.y = data[o + plot_file->control_data.neiph - 4 - 1];
        /* 7+NEIPH-3. Epsilon-z*/
        solids[i].epsilon.z = data[o + plot_file->control_data.neiph - 3 - 1];
        /* 7+NEIPH-2. Epsilon-xy*/
        solids[i].epsilon.xy = data[o + plot_file->control_data.neiph - 2 - 1];
        /* 7+NEIPH-1. Epsilon-yz*/
        solids[i].epsilon.yz = data[o + plot_file->control_data.neiph - 1 - 1];
        /* 7+NEIPH. Epsilon-zx*/
        solids[i].epsilon.zx = data[o + plot_file->control_data.neiph - 0 - 1];
      } else {
        memset(&solids[i].epsilon, 0, sizeof(d3plot_tensor));
      }

      o += plot_file->control_data.neiph;

      i++;
    }

    free(data);
    if (o != plot_file->control_data.nel8 * plot_file->control_data.nv3d) {
      ERROR_AND_NO_RETURN_F_PTR(
          "Sanity Check: Did not read all data from solids state. o=%zu NEL8 "
          "(%llu) * NV3D (%llu) = %llu",
          o, plot_file->control_data.nel8, plot_file->control_data.nv3d,
          plot_file->control_data.nel8 * plot_file->control_data.nv3d);
      *num_solids = 0;
      free(solids);

      END_PROFILE_FUNC();
      return NULL;
    }
  } else {
    double *data =
        malloc((plot_file->control_data.nel8 * plot_file->control_data.nv3d) *
               sizeof(double));

    d3_pointer d3_ptr = d3_buffer_read_words_at(
        &plot_file->buffer, data,
        plot_file->control_data.nel8 * plot_file->control_data.nv3d,
        plot_file->data_pointers[D3PLT_PTR_STATES + state] +
            plot_file->data_pointers[D3PLT_PTR_STATE_ELEMENT_SOLID]);
    d3_pointer_close(&plot_file->buffer, &d3_ptr);
    if (plot_file->buffer.error_string) {
      ERROR_AND_NO_RETURN_F_PTR("Failed to read words: %s",
                                plot_file->buffer.error_string);
      *num_solids = 0;
      free(data);
      free(solids);

      END_PROFILE_FUNC();
      return NULL;
    }

    size_t i = 0;
    size_t o = 0;
    while (i < *num_solids) {
      /* Docs: page 33*/
      if (plot_file->control_data.iosol[0]) {
        /* Read the six stresses*/
        memcpy(&solids[i].sigma, &data[o], sizeof(d3plot_tensor));
        o += 6;
      } else {
        memset(&solids[i].sigma, 0, sizeof(d3plot_tensor));
      }

      if (plot_file->control_data.iosol[1]) {
        solids[i].effective_plastic_strain = data[o++];
      } else {
        solids[i].effective_plastic_strain = 0.0;
      }

      /* Docs p12: If ISTRN=1, and NEIPH>=6, last the 6 additional values are
       * the six strain components. */
      if (plot_file->control_data.istrn == 1 &&
          plot_file->control_data.neiph >= 6) {
        /* We need -1 since we start by 0 and in the docs they start with
         * 1*/
        memcpy(&solids[i].epsilon,
               &data[o + plot_file->control_data.neiph - 5 - 1],
               sizeof(d3plot_tensor));
      } else {
        memset(&solids[i].epsilon, 0, sizeof(d3plot_tensor));
      }

      o += plot_file->control_data.neiph;

      i++;
    }

    free(data);
    if (o != plot_file->control_data.nel8 * plot_file->control_data.nv3d) {
      ERROR_AND_NO_RETURN_F_PTR(
          "Sanity Check: Did not read all data from solids state. o=%zu NEL8 "
          "(%llu) * NV3D (%llu) = %llu",
          o, plot_file->control_data.nel8, plot_file->control_data.nv3d,
          plot_file->control_data.nel8 * plot_file->control_data.nv3d);
      *num_solids = 0;
      free(solids);

      END_PROFILE_FUNC();
      return NULL;
    }
  }

  END_PROFILE_FUNC();
  return solids;
}

d3plot_thick_shell *d3plot_read_thick_shells_state(d3plot_file *plot_file,
                                                   size_t state,
                                                   size_t *num_thick_shells) {
  BEGIN_PROFILE_FUNC();
  D3PLOT_CLEAR_ERROR_STRING();

  *num_thick_shells = plot_file->control_data.nelt;
  if (*num_thick_shells == 0) {
    END_PROFILE_FUNC();
    return NULL;
  }

  if (state >= plot_file->num_states) {
    ERROR_AND_NO_RETURN_F_PTR("%zu is out of bounds for the states", state);
    *num_thick_shells = 0;

    END_PROFILE_FUNC();
    return NULL;
  }

  const uint8_t num_integration_points =
      (uint8_t)plot_file->control_data.maxint;
  const uint8_t num_history_variables = (uint8_t)plot_file->control_data.neips;
  const uint8_t stress_written = plot_file->control_data.ioshl[0];
  const uint8_t plastic_strain_written = plot_file->control_data.ioshl[1];
  const uint8_t inner_outer_strain_written = plot_file->control_data.istrn == 1;

  /* Allocate memory for all history variables of all thick shells*/
  double *history_variables =
      malloc(*num_thick_shells * 3 * num_history_variables * sizeof(double));

  d3plot_surface *additional_integration_points =
      malloc(sizeof(d3plot_surface) * (num_integration_points - 3) *
             *num_thick_shells);

  d3plot_thick_shell *thick_shells =
      malloc(*num_thick_shells * sizeof(d3plot_thick_shell));
  if (plot_file->buffer.word_size == 4) {
    float *data = malloc(plot_file->control_data.nelt *
                         plot_file->control_data.nv3dt * sizeof(float));

    d3_pointer d3_ptr = d3_buffer_read_words_at(
        &plot_file->buffer, data,
        plot_file->control_data.nelt * plot_file->control_data.nv3dt,
        plot_file->data_pointers[D3PLT_PTR_STATES + state] +
            plot_file->data_pointers[D3PLT_PTR_STATE_ELEMENT_THICK_SHELL]);
    d3_pointer_close(&plot_file->buffer, &d3_ptr);
    if (plot_file->buffer.error_string) {
      ERROR_AND_NO_RETURN_F_PTR("Failed to read words: %s",
                                plot_file->buffer.error_string);
      *num_thick_shells = 0;
      free(data);
      free(thick_shells);
      free(history_variables);
      free(additional_integration_points);

      END_PROFILE_FUNC();
      return NULL;
    }

    size_t i = 0;
    size_t o = 0;
    while (i < *num_thick_shells) {
      thick_shells[i].add_ips = NULL;

      /* Loop over all integration points */
      size_t j = 0;
      while (j < num_integration_points) {
        d3plot_surface *ip;
        switch (j) {
        case 0:
          ip = &thick_shells[i].mid;
          break;
        case 1:
          ip = &thick_shells[i].inner;
          break;
        case 2:
          ip = &thick_shells[i].outer;
          break;
        default: /* If MAXINT > 3 there are more integration points */
          if (!thick_shells[i].add_ips) {
            thick_shells[i].add_ips =
                &additional_integration_points[i *
                                               (num_integration_points - 3)];
          }
          ip = &thick_shells[i].add_ips[j - 3];
        }

        if (stress_written) {
          ip->sigma.x = data[o++];
          ip->sigma.y = data[o++];
          ip->sigma.z = data[o++];
          ip->sigma.xy = data[o++];
          ip->sigma.yz = data[o++];
          ip->sigma.zx = data[o++];
        } else {
          memset(&ip->sigma, 0, sizeof(d3plot_tensor));
        }

        if (plastic_strain_written) {
          ip->effective_plastic_strain = data[o++];
        } else {
          ip->effective_plastic_strain = 0.0;
        }

        /* Define NEIPS additional history values here for surface*/
        if (plot_file->control_data.neips != 0) {
          ip->history_variables =
              &history_variables[i * num_integration_points *
                                     num_history_variables +
                                 j * num_history_variables];
          size_t k = 0;
          while (k < num_history_variables) {
            ip->history_variables[k++] = data[o++];
          }
        } else {
          ip->history_variables = NULL;
        }

        if (j == 2) {
          /* If ISTRN=1, define inner and outer strain after outer surface */
          if (inner_outer_strain_written) {
            thick_shells[i].inner_epsilon.x = data[o++];
            thick_shells[i].inner_epsilon.y = data[o++];
            thick_shells[i].inner_epsilon.z = data[o++];
            thick_shells[i].inner_epsilon.xy = data[o++];
            thick_shells[i].inner_epsilon.yz = data[o++];
            thick_shells[i].inner_epsilon.zx = data[o++];

            thick_shells[i].outer_epsilon.x = data[o++];
            thick_shells[i].outer_epsilon.y = data[o++];
            thick_shells[i].outer_epsilon.z = data[o++];
            thick_shells[i].outer_epsilon.xy = data[o++];
            thick_shells[i].outer_epsilon.yz = data[o++];
            thick_shells[i].outer_epsilon.zx = data[o++];
          } else {
            memset(&thick_shells[i].inner_epsilon, 0,
                   2 * sizeof(d3plot_tensor));
          }
        }

        j++;
      }

      thick_shells[i].num_history_variables = num_history_variables;
      thick_shells[i].num_additional_integration_points =
          num_integration_points - 3;

      i++;
    }

    free(data);
    if (o != plot_file->control_data.nelt * plot_file->control_data.nv3dt) {
      ERROR_AND_NO_RETURN_F_PTR(
          "Sanity Check: Did not read all data from thick shells state. o=%zu "
          "NELT (%llu) * NV3DT (%llu) = %llu",
          o, plot_file->control_data.nelt, plot_file->control_data.nv3dt,
          plot_file->control_data.nelt * plot_file->control_data.nv3dt);
      *num_thick_shells = 0;
      free(thick_shells);
      free(history_variables);
      free(additional_integration_points);

      END_PROFILE_FUNC();
      return NULL;
    }
  } else {
    double *data = malloc(plot_file->control_data.nelt *
                          plot_file->control_data.nv3dt * sizeof(double));

    d3_pointer d3_ptr = d3_buffer_read_words_at(
        &plot_file->buffer, data,
        plot_file->control_data.nelt * plot_file->control_data.nv3dt,
        plot_file->data_pointers[D3PLT_PTR_STATES + state] +
            plot_file->data_pointers[D3PLT_PTR_STATE_ELEMENT_THICK_SHELL]);
    d3_pointer_close(&plot_file->buffer, &d3_ptr);
    if (plot_file->buffer.error_string) {
      ERROR_AND_NO_RETURN_F_PTR("Failed to read words: %s",
                                plot_file->buffer.error_string);
      *num_thick_shells = 0;
      free(data);
      free(thick_shells);
      free(history_variables);
      free(additional_integration_points);

      END_PROFILE_FUNC();
      return NULL;
    }

    size_t i = 0;
    size_t o = 0;
    while (i < *num_thick_shells) {
      thick_shells[i].add_ips = NULL;

      size_t j = 0;
      while (j < num_integration_points) {
        d3plot_surface *ip;
        switch (j) {
        case 0:
          ip = &thick_shells[i].mid;
          break;
        case 1:
          ip = &thick_shells[i].inner;
          break;
        case 2:
          ip = &thick_shells[i].outer;
          break;
        default:
          if (!thick_shells[i].add_ips) {
            thick_shells[i].add_ips =
                &additional_integration_points[i *
                                               (num_integration_points - 3)];
          }
          ip = &thick_shells[i].add_ips[j - 3];
          break;
        }

        if (stress_written) {
          memcpy(&ip->sigma, &data[o], sizeof(d3plot_tensor));
          o += sizeof(d3plot_tensor) / sizeof(double);
        } else {
          memset(&ip->sigma, 0, sizeof(d3plot_tensor));
        }

        if (plastic_strain_written) {
          ip->effective_plastic_strain = data[o++];
        } else {
          ip->effective_plastic_strain = 0.0;
        }

        /* Define NEIPS additional history variables here */
        if (plot_file->control_data.neips != 0) {
          ip->history_variables =
              &history_variables[i * num_integration_points *
                                     num_history_variables +
                                 j * num_history_variables];
          memcpy(ip->history_variables, &data[o],
                 num_history_variables * sizeof(double));
          o += plot_file->control_data.neips;
        } else {
          ip->history_variables = NULL;
        }

        if (j == 2) {
          /* If ISTRN=1, define inner and outer strain after outer surface */
          if (inner_outer_strain_written) {
            memcpy(&thick_shells[i].inner_epsilon, &data[o],
                   2 * sizeof(d3plot_tensor));
            o += 2 * sizeof(d3plot_tensor) / sizeof(double);
          } else {
            memset(&thick_shells[i].inner_epsilon, 0,
                   2 * sizeof(d3plot_tensor));
          }
        }

        j++;
      }

      thick_shells[i].num_history_variables = num_history_variables;
      thick_shells[i].num_additional_integration_points =
          num_integration_points - 3;

      i++;
    }

    free(data);
    if (o != plot_file->control_data.nelt * plot_file->control_data.nv3dt) {
      ERROR_AND_NO_RETURN_F_PTR(
          "Sanity Check: Did not read all data from thick shells state. o=%zu "
          "NELT (%llu) * NV3DT (%llu) = %llu",
          o, plot_file->control_data.nelt, plot_file->control_data.nv3dt,
          plot_file->control_data.nelt * plot_file->control_data.nv3dt);
      *num_thick_shells = 0;
      free(thick_shells);
      free(history_variables);
      free(additional_integration_points);

      END_PROFILE_FUNC();
      return NULL;
    }
  }

  END_PROFILE_FUNC();
  return thick_shells;
}

d3plot_beam *d3plot_read_beams_state(d3plot_file *plot_file, size_t state,
                                     size_t *num_beams) {
  BEGIN_PROFILE_FUNC();
  D3PLOT_CLEAR_ERROR_STRING();

  *num_beams = plot_file->control_data.nel2;
  if (*num_beams == 0) {
    END_PROFILE_FUNC();
    return NULL;
  }

  if (state >= plot_file->num_states) {
    ERROR_AND_NO_RETURN_F_PTR("%zu is out of bounds for the states", state);
    *num_beams = 0;

    END_PROFILE_FUNC();
    return NULL;
  }

  if (plot_file->control_data.neipb > 255 ||
      plot_file->control_data.beamip > 255) {
    ERROR_AND_NO_RETURN_F_PTR(
        "Only up to 255 history variables and beam integration points are "
        "supported NEIPB=%llu BEAMIP=%llu",
        plot_file->control_data.neipb, plot_file->control_data.beamip);
    *num_beams = 0;

    END_PROFILE_FUNC();
    return NULL;
  }

  const uint8_t num_integration_points =
      (uint8_t)plot_file->control_data.beamip;
  const uint8_t num_history_variables = (uint8_t)plot_file->control_data.neipb;
  const size_t his_avg_min_max_offset = *num_beams *
                                        (size_t)num_integration_points *
                                        (size_t)num_history_variables;

  d3plot_beam_ip *integration_points =
      malloc(*num_beams * num_integration_points * sizeof(d3plot_beam_ip));
  double *history_variables =
      malloc(*num_beams * num_history_variables * (3 + num_integration_points) *
             sizeof(double));

  d3plot_beam *beams = malloc(*num_beams * sizeof(d3plot_beam));
  if (plot_file->buffer.word_size == 4) {
    float *data = malloc(plot_file->control_data.nel2 *
                         plot_file->control_data.nv1d * sizeof(float));

    d3_pointer d3_ptr = d3_buffer_read_words_at(
        &plot_file->buffer, data,
        plot_file->control_data.nel2 * plot_file->control_data.nv1d,
        plot_file->data_pointers[D3PLT_PTR_STATES + state] +
            plot_file->data_pointers[D3PLT_PTR_STATE_ELEMENT_BEAM]);
    d3_pointer_close(&plot_file->buffer, &d3_ptr);
    if (plot_file->buffer.error_string) {
      ERROR_AND_NO_RETURN_F_PTR("Failed to read words: %s",
                                plot_file->buffer.error_string);
      *num_beams = 0;
      free(data);
      free(beams);

      END_PROFILE_FUNC();
      return NULL;
    }

    size_t i = 0;
    size_t o = 0;
    while (i < *num_beams) {
      d3plot_beam *beam = &beams[i];
      beam->num_history_variables = num_history_variables;
      beam->num_integration_points = num_integration_points;
      beam->ips = &integration_points[i * (size_t)num_integration_points];
      if (history_variables) {
        beam->history_average =
            &history_variables[his_avg_min_max_offset +
                               i * 3 * (size_t)num_history_variables +
                               0 * (size_t)num_history_variables];
        beam->history_min =
            &history_variables[his_avg_min_max_offset +
                               i * 3 * (size_t)num_history_variables +
                               1 * (size_t)num_history_variables];
        beam->history_max =
            &history_variables[his_avg_min_max_offset +
                               i * 3 * (size_t)num_history_variables +
                               2 * (size_t)num_history_variables];
      } else {
        memset(&beam->history_average, 0, sizeof(double *) * 3);
      }

      beam->axial_force = data[o++];
      beam->s_shear_resultant = data[o++];
      beam->t_shear_resultant = data[o++];
      beam->s_bending_moment = data[o++];
      beam->t_bending_moment = data[o++];
      beam->torsional_resultant = data[o++];

      uint8_t j = 0;
      while (j < num_integration_points) {
        d3plot_beam_ip *ip = &beam->ips[j];

        ip->rs_shear_stress = data[o++];
        ip->tr_shear_stress = data[o++];
        ip->axial_stress = data[o++];
        ip->plastic_strain = data[o++];
        ip->axial_strain = data[o++];

        j++;
      }

      if (history_variables) {
        j = 0;
        while (j < num_history_variables) {
          /* Average per integration point */
          beam->history_average[j] = data[o++];

          j++;
        }

        j = 0;
        while (j < num_history_variables) {
          /* Minimum per integration point */
          beam->history_min[j] = data[o++];

          j++;
        }

        j = 0;
        while (j < num_history_variables) {
          /* Maximum per integration point */
          beam->history_max[j] = data[o++];

          j++;
        }

        j = 0;
        while (j < num_integration_points) {
          d3plot_beam_ip *ip = &beam->ips[j];
          ip->history_variables =
              &history_variables[i * (size_t)num_integration_points *
                                     (size_t)num_history_variables +
                                 (size_t)j * (size_t)num_history_variables];

          uint8_t k = 0;
          while (k < num_history_variables) {
            /* History variable of integration point */
            ip->history_variables[k] = data[o++];

            k++;
          }

          j++;
        }
      } else {
        j = 0;
        while (j < num_integration_points) {
          beam->ips[j].history_variables = NULL;

          j++;
        }
      }

      i++;
    }

    free(data);
    if (o != plot_file->control_data.nv1d * plot_file->control_data.nel2) {
      ERROR_AND_NO_RETURN_F_PTR(
          "Sanity Check: Did not read all data from beams state. o=%zu "
          "BEAMIP=%u NEIPB=%u NEL2 "
          "(%llu) * NV1D (%llu) = %llu",
          o, num_integration_points, num_history_variables,
          plot_file->control_data.nel2, plot_file->control_data.nv1d,
          plot_file->control_data.nel2 * plot_file->control_data.nv1d);
      *num_beams = 0;
      free(beams);

      END_PROFILE_FUNC();
      return NULL;
    }
  } else {
    double *data = malloc(plot_file->control_data.nel2 *
                          plot_file->control_data.nv1d * sizeof(double));

    d3_pointer d3_ptr = d3_buffer_read_words_at(
        &plot_file->buffer, data,
        plot_file->control_data.nel2 * plot_file->control_data.nv1d,
        plot_file->data_pointers[D3PLT_PTR_STATES + state] +
            plot_file->data_pointers[D3PLT_PTR_STATE_ELEMENT_BEAM]);
    d3_pointer_close(&plot_file->buffer, &d3_ptr);
    if (plot_file->buffer.error_string) {
      ERROR_AND_NO_RETURN_F_PTR("Failed to read words: %s",
                                plot_file->buffer.error_string);
      *num_beams = 0;
      free(data);
      free(beams);

      END_PROFILE_FUNC();
      return NULL;
    }

    size_t i = 0;
    size_t o = 0;
    while (i < *num_beams) {
      d3plot_beam *beam = &beams[i];
      beam->num_history_variables = num_history_variables;
      beam->num_integration_points = num_integration_points;
      beam->ips = &integration_points[i * (size_t)num_integration_points];
      if (history_variables) {
        beam->history_average =
            &history_variables[his_avg_min_max_offset +
                               i * 3 * (size_t)num_history_variables +
                               0 * (size_t)num_history_variables];
        beam->history_min =
            &history_variables[his_avg_min_max_offset +
                               i * 3 * (size_t)num_history_variables +
                               1 * (size_t)num_history_variables];
        beam->history_max =
            &history_variables[his_avg_min_max_offset +
                               i * 3 * (size_t)num_history_variables +
                               2 * (size_t)num_history_variables];
      } else {
        memset(&beam->history_average, 0, sizeof(double *) * 3);
      }

      memcpy(&beam->axial_force, &data[o], sizeof(double) * 6);
      o += 6;

      uint8_t j = 0;
      while (j < num_integration_points) {
        d3plot_beam_ip *ip = &beam->ips[j];

        /* RS shear stress */
        /* TR shear stress */
        /* Axial stress */
        /* Plastic strain */
        /* Axial strain */
        memcpy(&ip->rs_shear_stress, &data[o], sizeof(double) * 5);
        o += 5;

        j++;
      }

      if (history_variables) {
        /* Average per integration point */
        memcpy(beam->history_average, &data[o],
               sizeof(double) * (size_t)num_history_variables);
        o += (size_t)num_history_variables;

        /* Minimum per integration point */
        memcpy(beam->history_min, &data[o],
               sizeof(double) * (size_t)num_history_variables);
        o += (size_t)num_history_variables;

        /* Maximum per integration point */
        memcpy(beam->history_max, &data[o],
               sizeof(double) * (size_t)num_history_variables);
        o += (size_t)num_history_variables;

        j = 0;
        while (j < num_integration_points) {
          d3plot_beam_ip *ip = &beam->ips[j];
          ip->history_variables =
              &history_variables[i * (size_t)num_integration_points *
                                     (size_t)num_history_variables +
                                 (size_t)j * (size_t)num_history_variables];

          /* History variables of integration point */
          memcpy(ip->history_variables, &data[o],
                 sizeof(double) * (size_t)num_history_variables);
          o += (size_t)num_history_variables;

          j++;
        }
      } else {
        j = 0;
        while (j < num_integration_points) {
          beam->ips[j].history_variables = NULL;

          j++;
        }
      }

      i++;
    }

    free(data);
    if (o != plot_file->control_data.nv1d * plot_file->control_data.nel2) {
      ERROR_AND_NO_RETURN_F_PTR(
          "Sanity Check: Did not read all data from beams state. o=%zu "
          "BEAMIP=%llu NEIPB=%llu NEL2 "
          "(%llu) * NV1D (%llu) = %llu",
          o, plot_file->control_data.beamip, plot_file->control_data.neipb,
          plot_file->control_data.nel2, plot_file->control_data.nv1d,
          plot_file->control_data.nel2 * plot_file->control_data.nv1d);
      *num_beams = 0;
      free(beams);

      END_PROFILE_FUNC();
      return NULL;
    }
  }

  END_PROFILE_FUNC();
  return beams;
}

d3plot_shell *d3plot_read_shells_state(d3plot_file *plot_file, size_t state,
                                       size_t *num_shells) {
  BEGIN_PROFILE_FUNC();
  D3PLOT_CLEAR_ERROR_STRING();

  *num_shells = plot_file->control_data.nel4;
  if (*num_shells == 0) {
    END_PROFILE_FUNC();
    return NULL;
  }

  if (state >= plot_file->num_states) {
    ERROR_AND_NO_RETURN_F_PTR("%zu is out of bounds for the states", state);
    *num_shells = 0;

    END_PROFILE_FUNC();
    return NULL;
  }

  const uint8_t num_integration_points =
      (uint8_t)plot_file->control_data.maxint;
  const uint8_t num_history_variables = (uint8_t)plot_file->control_data.neips;
  const uint8_t stress_written = plot_file->control_data.ioshl[0];
  const uint8_t plastic_strain_written = plot_file->control_data.ioshl[1];
  const uint8_t force_resultant_written = plot_file->control_data.ioshl[2];
  const uint8_t thickness_energy_written = plot_file->control_data.ioshl[3];

  /* Allocate memory for all history variables of all shells*/
  double *history_variables =
      malloc(num_history_variables * num_integration_points * *num_shells *
             sizeof(double));

  d3plot_surface *additional_integration_points = malloc(
      sizeof(d3plot_surface) * (num_integration_points - 3) * *num_shells);

  d3plot_shell *shells = malloc(*num_shells * sizeof(d3plot_shell));
  if (plot_file->buffer.word_size == 4) {
    float *data = malloc(plot_file->control_data.nel4 *
                         plot_file->control_data.nv2d * sizeof(float));

    d3_pointer d3_ptr = d3_buffer_read_words_at(
        &plot_file->buffer, data,
        plot_file->control_data.nel4 * plot_file->control_data.nv2d,
        plot_file->data_pointers[D3PLT_PTR_STATES + state] +
            plot_file->data_pointers[D3PLT_PTR_STATE_ELEMENT_SHELL]);
    d3_pointer_close(&plot_file->buffer, &d3_ptr);
    if (plot_file->buffer.error_string) {
      ERROR_AND_NO_RETURN_F_PTR("Failed to read words: %s",
                                plot_file->buffer.error_string);
      *num_shells = 0;
      free(data);
      free(shells);

      END_PROFILE_FUNC();
      return NULL;
    }

    size_t i = 0;
    size_t o = 0;
    while (i < *num_shells) {
      shells[i].add_ips = NULL;

      /* Loop over all integration points */
      size_t j = 0;
      while (j < num_integration_points) {
        d3plot_surface *ip;
        switch (j) {
        case 0:
          ip = &shells[i].mid; /* The first IP is mid */
          break;
        case 1:
          ip = &shells[i].inner; /* The second IP is inner */
          break;
        case 2:
          ip = &shells[i].outer; /* The third IP is outer */
          break;
        default: /* If MAXINT > 3 there are more integration points */
          if (!shells[i].add_ips) {
            shells[i].add_ips =
                &additional_integration_points[i *
                                               (num_integration_points - 3)];
          }
          ip = &shells[i].add_ips[j - 3];
          break;
        }

        if (stress_written) {
          ip->sigma.x = data[o++];
          ip->sigma.y = data[o++];
          ip->sigma.z = data[o++];
          ip->sigma.xy = data[o++];
          ip->sigma.yz = data[o++];
          ip->sigma.zx = data[o++];
        } else {
          memset(&ip->sigma, 0, sizeof(d3plot_tensor));
        }

        if (plastic_strain_written) {
          ip->effective_plastic_strain = data[o++];
        } else {
          ip->effective_plastic_strain = 0.0;
        }

        if (plot_file->control_data.neips != 0) {
          /* Define NEIPS additional history values here*/
          ip->history_variables =
              &history_variables[i * num_integration_points *
                                     num_history_variables +
                                 j * num_history_variables];
          size_t k = 0;
          while (k < num_history_variables) {
            ip->history_variables[k++] = data[o++];
          }
        } else {
          ip->history_variables = NULL;
        }

        j++;
      }

      if (force_resultant_written) {
        shells[i].bending_moment.x = data[o++];
        shells[i].bending_moment.y = data[o++];
        shells[i].bending_moment.xy = data[o++];
        shells[i].shear_resultant.x = data[o++];
        shells[i].shear_resultant.y = data[o++];
        shells[i].normal_resultant.x = data[o++];
        shells[i].normal_resultant.y = data[o++];
        shells[i].normal_resultant.xy = data[o++];
      } else {
        memset(&shells[i].bending_moment, 0, sizeof(double) * 8);
      }

      if (thickness_energy_written) {
        shells[i].thickness = data[o++];
        shells[i].element_dependent_variables[0] = data[o++];
        shells[i].element_dependent_variables[1] = data[o++];
        shells[i].internal_energy = data[o++];
      } else {
        memset(&shells[i].thickness, 0, sizeof(double) * 4);
      }

      if (plot_file->control_data.istrn == 0) {
        memset(&shells[i].inner_epsilon, 0, 2 * sizeof(d3plot_tensor));
      } else if (plot_file->control_data.istrn == 1) {
        shells[i].inner_epsilon.x = data[o++];
        shells[i].inner_epsilon.y = data[o++];
        shells[i].inner_epsilon.z = data[o++];
        shells[i].inner_epsilon.xy = data[o++];
        shells[i].inner_epsilon.yz = data[o++];
        shells[i].inner_epsilon.zx = data[o++];

        shells[i].outer_epsilon.x = data[o++];
        shells[i].outer_epsilon.y = data[o++];
        shells[i].outer_epsilon.z = data[o++];
        shells[i].outer_epsilon.xy = data[o++];
        shells[i].outer_epsilon.yz = data[o++];
        shells[i].outer_epsilon.zx = data[o++];
      }

      shells[i].num_history_variables = num_history_variables;
      shells[i].num_additional_integration_points = num_integration_points - 3;

      i++;
    }

    free(data);
    if (o != plot_file->control_data.nel4 * plot_file->control_data.nv2d) {
      ERROR_AND_NO_RETURN_F_PTR(
          "Sanity Check: Did not read all data from shells state. o=%zu NEL4 "
          "(%llu) * NV2D (%llu) = %llu",
          o, plot_file->control_data.nel4, plot_file->control_data.nv2d,
          plot_file->control_data.nel4 * plot_file->control_data.nv2d);
      *num_shells = 0;
      free(shells);
      free(history_variables);
      free(additional_integration_points);

      END_PROFILE_FUNC();
      return NULL;
    }
  } else {
    double *data = malloc(plot_file->control_data.nel4 *
                          plot_file->control_data.nv2d * sizeof(double));

    d3_pointer d3_ptr = d3_buffer_read_words_at(
        &plot_file->buffer, data,
        plot_file->control_data.nel4 * plot_file->control_data.nv2d,
        plot_file->data_pointers[D3PLT_PTR_STATES + state] +
            plot_file->data_pointers[D3PLT_PTR_STATE_ELEMENT_SHELL]);
    d3_pointer_close(&plot_file->buffer, &d3_ptr);
    if (plot_file->buffer.error_string) {
      ERROR_AND_NO_RETURN_F_PTR("Failed to read words: %s",
                                plot_file->buffer.error_string);
      *num_shells = 0;
      free(data);
      free(shells);
      free(history_variables);
      free(additional_integration_points);

      END_PROFILE_FUNC();
      return NULL;
    }

    size_t i = 0;
    size_t o = 0;
    while (i < *num_shells) {
      shells[i].add_ips = NULL;

      size_t j = 0;
      while (j < num_integration_points) {
        d3plot_surface *ip;
        switch (j) {
        case 0:
          ip = &shells[i].mid;
          break;
        case 1:
          ip = &shells[i].inner;
          break;
        case 2:
          ip = &shells[i].outer;
          break;
        default:
          if (!shells[i].add_ips) {
            shells[i].add_ips =
                &additional_integration_points[i *
                                               (num_integration_points - 3)];
          }
          ip = &shells[i].add_ips[j - 3];
          break;
        }

        if (stress_written) {
          memcpy(&ip->sigma, &data[o], sizeof(d3plot_tensor));
          o += sizeof(d3plot_tensor) / sizeof(double);
        } else {
          memset(&ip->sigma, 0, sizeof(d3plot_tensor));
        }

        if (plastic_strain_written) {
          ip->effective_plastic_strain = data[o++];
        } else {
          ip->effective_plastic_strain = 0.0;
        }

        /* Define NEIPS additional history variables here */
        if (plot_file->control_data.neips != 0) {
          ip->history_variables =
              &history_variables[i * num_integration_points *
                                     num_history_variables +
                                 j * num_history_variables];
          memcpy(ip->history_variables, &data[o],
                 num_history_variables * sizeof(double));
          o += plot_file->control_data.neips;
        } else {
          ip->history_variables = NULL;
        }

        j++;
      }

      if (force_resultant_written) {
        memcpy(&shells[i].bending_moment, &data[o], sizeof(double) * 8);
        o += 8;
      } else {
        memset(&shells[i].bending_moment, 0, sizeof(double) * 8);
      }

      if (thickness_energy_written) {
        memcpy(&shells[i].thickness, &data[o], sizeof(double) * 4);
        o += 4;
        /* On page 49 the docs say:
           "33. Internal energy (if and only if ISTRN=0)"
         * which suggests that Internal energy is only written if ISTRN is 0,
         * but the docs also say on page 48:
           "NV2D=MAXINT* (6*IOSHL(1) + 1*IOSHL(2) + NEIPS) +8*IOSHL(3) +
         4*IOSHL(4)" The 4*IOSHL(4) part suggests that the following values
         always appear together in the database:

           30. Thickness
           31. Element dependent variable
           32. Element dependent variable
           33. Internal energy

           Which means that the docs are contradicting themselves -_(°_°)_-
           The basic01 example also suggests that the latter is correct.
           */
      } else {
        memset(&shells[i].thickness, 0, sizeof(double) * 4);
      }

      if (plot_file->control_data.istrn == 0) {
        memset(&shells[i].inner_epsilon, 0, 2 * sizeof(d3plot_tensor));
      } else if (plot_file->control_data.istrn == 1) {
        memcpy(&shells[i].inner_epsilon, &data[o], 2 * sizeof(d3plot_tensor));
        o += 2 * sizeof(d3plot_tensor) / sizeof(double);
      }

      shells[i].num_history_variables = num_history_variables;
      shells[i].num_additional_integration_points = num_integration_points - 3;

      i++;
    }

    free(data);
    if (o != plot_file->control_data.nel4 * plot_file->control_data.nv2d) {
      ERROR_AND_NO_RETURN_F_PTR(
          "Sanity Check: Did not read all data from shells state. o=%zu NEL4 "
          "(%llu) * NV2D (%llu) = %llu",
          o, plot_file->control_data.nel4, plot_file->control_data.nv2d,
          plot_file->control_data.nel4 * plot_file->control_data.nv2d);
      *num_shells = 0;
      free(shells);
      free(history_variables);
      free(additional_integration_points);

      END_PROFILE_FUNC();
      return NULL;
    }
  }

  END_PROFILE_FUNC();
  return shells;
}

d3plot_solid_con *d3plot_read_solid_elements(d3plot_file *plot_file,
                                             size_t *num_solids) {
  BEGIN_PROFILE_FUNC();
  D3PLOT_CLEAR_ERROR_STRING();

  if (plot_file->control_data.nel8 <= 0) {
    /* nel8 represents the number of extra nodes for ten node solids*/
    *num_solids = 0;
    END_PROFILE_FUNC();
    return NULL;
  }

  *num_solids = plot_file->control_data.nel8;
  d3plot_solid_con *solids = malloc(*num_solids * sizeof(d3plot_solid_con));
  if (plot_file->buffer.word_size == 4) {
    uint32_t *solids32 = malloc(*num_solids * 9 * sizeof(uint32_t));
    d3_pointer d3_ptr = d3_buffer_read_words_at(
        &plot_file->buffer, solids32, 9 * *num_solids,
        plot_file->data_pointers[D3PLT_PTR_EL8_CONNECT]);
    d3_pointer_close(&plot_file->buffer, &d3_ptr);
    if (plot_file->buffer.error_string) {
      ERROR_AND_NO_RETURN_F_PTR("Failed to read words: %s",
                                plot_file->buffer.error_string);
      *num_solids = 0;
      free(solids32);
      free(solids);

      END_PROFILE_FUNC();
      return NULL;
    }

    size_t i = 0;
    while (i < *num_solids) {
      /* Subtract 1 because Fortran starts by 1 and C starts by 0*/
      solids[i].node_indices[0] = solids32[i * 9 + 0] - 1;
      solids[i].node_indices[1] = solids32[i * 9 + 1] - 1;
      solids[i].node_indices[2] = solids32[i * 9 + 2] - 1;
      solids[i].node_indices[3] = solids32[i * 9 + 3] - 1;
      solids[i].node_indices[4] = solids32[i * 9 + 4] - 1;
      solids[i].node_indices[5] = solids32[i * 9 + 5] - 1;
      solids[i].node_indices[6] = solids32[i * 9 + 6] - 1;
      solids[i].node_indices[7] = solids32[i * 9 + 7] - 1;
      solids[i].material_index = solids32[i * 9 + 8] - 1;

      i++;
    }

    free(solids32);
  } else {
    d3_pointer d3_ptr = d3_buffer_read_words_at(
        &plot_file->buffer, solids, 9 * *num_solids,
        plot_file->data_pointers[D3PLT_PTR_EL8_CONNECT]);
    d3_pointer_close(&plot_file->buffer, &d3_ptr);
    if (plot_file->buffer.error_string) {
      ERROR_AND_NO_RETURN_F_PTR("Failed to read words: %s",
                                plot_file->buffer.error_string);
      *num_solids = 0;
      free(solids);

      END_PROFILE_FUNC();
      return NULL;
    }

    size_t i = 0;
    while (i < *num_solids) {
      /* Subtract 1 because Fortran starts by 1 and C starts by 0*/
      solids[i].node_indices[0] -= 1;
      solids[i].node_indices[1] -= 1;
      solids[i].node_indices[2] -= 1;
      solids[i].node_indices[3] -= 1;
      solids[i].node_indices[4] -= 1;
      solids[i].node_indices[5] -= 1;
      solids[i].node_indices[6] -= 1;
      solids[i].node_indices[7] -= 1;
      solids[i].material_index -= 1;

      i++;
    }
  }

  END_PROFILE_FUNC();
  return solids;
}

d3plot_thick_shell_con *
d3plot_read_thick_shell_elements(d3plot_file *plot_file,
                                 size_t *num_thick_shells) {
  BEGIN_PROFILE_FUNC();
  D3PLOT_CLEAR_ERROR_STRING();

  if (plot_file->control_data.nelt == 0) {
    *num_thick_shells = 0;
    END_PROFILE_FUNC();
    return NULL;
  }

  *num_thick_shells = plot_file->control_data.nelt;
  d3plot_thick_shell_con *thick_shells =
      malloc(*num_thick_shells * sizeof(d3plot_thick_shell_con));
  if (plot_file->buffer.word_size == 4) {
    uint32_t *thick_shells32 = malloc(*num_thick_shells * 9 * sizeof(uint32_t));
    d3_pointer d3_ptr = d3_buffer_read_words_at(
        &plot_file->buffer, thick_shells32, 9 * *num_thick_shells,
        plot_file->data_pointers[D3PLT_PTR_ELT_CONNECT]);
    d3_pointer_close(&plot_file->buffer, &d3_ptr);
    if (plot_file->buffer.error_string) {
      ERROR_AND_NO_RETURN_F_PTR("Failed to read words: %s",
                                plot_file->buffer.error_string);
      *num_thick_shells = 0;
      free(thick_shells32);
      free(thick_shells);

      END_PROFILE_FUNC();
      return NULL;
    }

    size_t i = 0;
    while (i < *num_thick_shells) {
      size_t j = 0;
      while (j < 8) {
        /* Subtract 1 because Fortran starts by 1 and C starts by 0*/
        thick_shells[i].node_indices[j + 0] = thick_shells32[i * 9 + j + 0] - 1;
        thick_shells[i].node_indices[j + 1] = thick_shells32[i * 9 + j + 1] - 1;
        thick_shells[i].node_indices[j + 2] = thick_shells32[i * 9 + j + 2] - 1;
        thick_shells[i].node_indices[j + 3] = thick_shells32[i * 9 + j + 3] - 1;

        j += 4;
      }
      thick_shells[i].material_index = thick_shells32[i * 9 + 8] - 1;

      i++;
    }

    free(thick_shells32);
  } else {
    d3_pointer d3_ptr = d3_buffer_read_words_at(
        &plot_file->buffer, thick_shells, 9 * *num_thick_shells,
        plot_file->data_pointers[D3PLT_PTR_ELT_CONNECT]);
    d3_pointer_close(&plot_file->buffer, &d3_ptr);
    if (plot_file->buffer.error_string) {
      ERROR_AND_NO_RETURN_F_PTR("Failed to read words: %s",
                                plot_file->buffer.error_string);
      *num_thick_shells = 0;
      free(thick_shells);

      END_PROFILE_FUNC();
      return NULL;
    }

    size_t i = 0;
    while (i < *num_thick_shells) {
      size_t j = 0;
      while (j < 8) {
        /* Subtract 1 because Fortran starts by 1 and C starts by 0*/
        thick_shells[i].node_indices[j + 0] -= 1;
        thick_shells[i].node_indices[j + 1] -= 1;
        thick_shells[i].node_indices[j + 2] -= 1;
        thick_shells[i].node_indices[j + 3] -= 1;

        j += 4;
      }
      thick_shells[i].material_index -= 1;

      i++;
    }
  }

  END_PROFILE_FUNC();
  return thick_shells;
}

d3plot_beam_con *d3plot_read_beam_elements(d3plot_file *plot_file,
                                           size_t *num_beams) {
  BEGIN_PROFILE_FUNC();
  D3PLOT_CLEAR_ERROR_STRING();

  if (plot_file->control_data.nel2 == 0) {
    *num_beams = 0;
    END_PROFILE_FUNC();
    return NULL;
  }

  *num_beams = plot_file->control_data.nel2;
  d3plot_beam_con *beams = malloc(*num_beams * sizeof(d3plot_beam_con));
  if (plot_file->buffer.word_size == 4) {
    uint32_t *beams32 = malloc(*num_beams * 6 * sizeof(uint32_t));
    d3_pointer d3_ptr = d3_buffer_read_words_at(
        &plot_file->buffer, beams32, 6 * *num_beams,
        plot_file->data_pointers[D3PLT_PTR_EL2_CONNECT]);
    d3_pointer_close(&plot_file->buffer, &d3_ptr);
    if (plot_file->buffer.error_string) {
      ERROR_AND_NO_RETURN_F_PTR("Failed to read words: %s",
                                plot_file->buffer.error_string);
      *num_beams = 0;
      free(beams32);
      free(beams);

      END_PROFILE_FUNC();
      return NULL;
    }

    size_t i = 0;
    while (i < *num_beams) {
      /* Subtract 1 because Fortran starts by 1 and C starts by 0*/
      beams[i].node_indices[0] = beams32[i * 6 + 0] - 1;
      beams[i].node_indices[1] = beams32[i * 6 + 1] - 1;
      beams[i].orientation_node_index = beams32[i * 6 + 2] - 1;
      beams[i]._null[0] = beams32[i * 6 + 3];
      beams[i]._null[1] = beams32[i * 6 + 4];
      beams[i].material_index = beams32[i * 6 + 5] - 1;

      i++;
    }

    free(beams32);
  } else {
    d3_pointer d3_ptr = d3_buffer_read_words_at(
        &plot_file->buffer, beams, 6 * *num_beams,
        plot_file->data_pointers[D3PLT_PTR_EL2_CONNECT]);
    d3_pointer_close(&plot_file->buffer, &d3_ptr);
    if (plot_file->buffer.error_string) {
      ERROR_AND_NO_RETURN_F_PTR("Failed to read words: %s",
                                plot_file->buffer.error_string);
      *num_beams = 0;
      free(beams);

      END_PROFILE_FUNC();
      return NULL;
    }

    size_t i = 0;
    while (i < *num_beams) {
      /* Subtract 1 because Fortran starts by 1 and C starts by 0*/
      beams[i].node_indices[0] -= 1;
      beams[i].node_indices[1] -= 1;
      beams[i].orientation_node_index -= 1;
      beams[i].material_index -= 1;

      i++;
    }
  }

  END_PROFILE_FUNC();
  return beams;
}

d3plot_shell_con *d3plot_read_shell_elements(d3plot_file *plot_file,
                                             size_t *num_shells) {
  BEGIN_PROFILE_FUNC();
  D3PLOT_CLEAR_ERROR_STRING();

  if (plot_file->control_data.nel4 == 0) {
    *num_shells = 0;
    END_PROFILE_FUNC();
    return NULL;
  }

  *num_shells = plot_file->control_data.nel4;
  d3plot_shell_con *shells = malloc(*num_shells * sizeof(d3plot_shell_con));
  if (plot_file->buffer.word_size == 4) {
    uint32_t *shells32 = malloc(*num_shells * 5 * sizeof(uint32_t));
    d3_pointer d3_ptr = d3_buffer_read_words_at(
        &plot_file->buffer, shells32, 5 * *num_shells,
        plot_file->data_pointers[D3PLT_PTR_EL4_CONNECT]);
    d3_pointer_close(&plot_file->buffer, &d3_ptr);
    if (plot_file->buffer.error_string) {
      ERROR_AND_NO_RETURN_F_PTR("Failed to read words: %s",
                                plot_file->buffer.error_string);
      *num_shells = 0;
      free(shells32);
      free(shells);

      END_PROFILE_FUNC();
      return NULL;
    }

    size_t i = 0;
    while (i < *num_shells) {
      /* Subtract 1 because Fortran starts by 1 and C starts by 0*/
      shells[i].node_indices[0] = shells32[i * 5 + 0] - 1;
      shells[i].node_indices[1] = shells32[i * 5 + 1] - 1;
      shells[i].node_indices[2] = shells32[i * 5 + 2] - 1;
      shells[i].node_indices[3] = shells32[i * 5 + 3] - 1;
      shells[i].material_index = shells32[i * 5 + 4] - 1;

      i++;
    }

    free(shells32);
  } else {
    d3_pointer d3_ptr = d3_buffer_read_words_at(
        &plot_file->buffer, shells, 5 * *num_shells,
        plot_file->data_pointers[D3PLT_PTR_EL4_CONNECT]);
    d3_pointer_close(&plot_file->buffer, &d3_ptr);
    if (plot_file->buffer.error_string) {
      ERROR_AND_NO_RETURN_F_PTR("Failed to read words: %s",
                                plot_file->buffer.error_string);
      *num_shells = 0;
      free(shells);

      END_PROFILE_FUNC();
      return NULL;
    }

    size_t i = 0;
    while (i < *num_shells) {
      /* Subtract 1 because Fortran starts by 1 and C starts by 0*/
      shells[i].node_indices[0] -= 1;
      shells[i].node_indices[1] -= 1;
      shells[i].node_indices[2] -= 1;
      shells[i].node_indices[3] -= 1;
      shells[i].material_index -= 1;

      i++;
    }
  }

  END_PROFILE_FUNC();
  return shells;
}

char *d3plot_read_title(d3plot_file *plot_file) {
  BEGIN_PROFILE_FUNC();
  D3PLOT_CLEAR_ERROR_STRING();

  char *title = malloc(10 * plot_file->buffer.word_size + 1);
  /* We never set D3PLT_PTR_TITLE, but because the Title is at position 0 we
   * don't need to*/
  d3_pointer d3_ptr = d3_buffer_read_words_at(
      &plot_file->buffer, title, 10, plot_file->data_pointers[D3PLT_PTR_TITLE]);
  d3_pointer_close(&plot_file->buffer, &d3_ptr);
  if (plot_file->buffer.error_string) {
    ERROR_AND_NO_RETURN_F_PTR("Failed to read words: %s",
                              plot_file->buffer.error_string);
    free(title);

    END_PROFILE_FUNC();
    return NULL;
  }
  title[10 * plot_file->buffer.word_size] = '\0';

  END_PROFILE_FUNC();
  return title;
}

struct tm *d3plot_read_run_time(d3plot_file *plot_file) {
  BEGIN_PROFILE_FUNC();
  D3PLOT_CLEAR_ERROR_STRING();

  d3_word run_time = 0;
  d3_pointer d3_ptr =
      d3_buffer_read_words_at(&plot_file->buffer, &run_time, 1,
                              plot_file->data_pointers[D3PLT_PTR_RUN_TIME]);
  d3_pointer_close(&plot_file->buffer, &d3_ptr);
  if (plot_file->buffer.error_string) {
    ERROR_AND_NO_RETURN_F_PTR("Failed to read words: %s",
                              plot_file->buffer.error_string);
    return NULL;
  }
  const time_t epoch_time = run_time;
  struct tm *time_value = localtime(&epoch_time);

  END_PROFILE_FUNC();
  return time_value;
}

time_t d3plot_read_epoch_run_time(d3plot_file *plot_file) {
  BEGIN_PROFILE_FUNC();
  D3PLOT_CLEAR_ERROR_STRING();

  d3_word run_time = 0;
  d3_pointer d3_ptr =
      d3_buffer_read_words_at(&plot_file->buffer, &run_time, 1,
                              plot_file->data_pointers[D3PLT_PTR_RUN_TIME]);
  d3_pointer_close(&plot_file->buffer, &d3_ptr);
  if (plot_file->buffer.error_string) {
    ERROR_AND_NO_RETURN_F_PTR("Failed to read words: %s",
                              plot_file->buffer.error_string);
    return (time_t)0;
  }
  const time_t epoch_time = run_time;

  END_PROFILE_FUNC();
  return epoch_time;
}

#define ADD_ELEMENTS_TO_PART(id_func, el_func, el_type, part_num, part_ids,    \
                             part_indices)                                     \
  ids = id_func(plot_file, &num_elements);                                     \
  if (plot_file->error_string) {                                               \
    /* Just ignore those elements*/                                            \
    D3PLOT_CLEAR_ERROR_STRING();                                               \
  } else if (num_elements > 0) {                                               \
    el_type *els = el_func(plot_file, &num_elements);                          \
    if (plot_file->error_string) {                                             \
      /* Just ignore those elements*/                                          \
      D3PLOT_CLEAR_ERROR_STRING();                                             \
    } else {                                                                   \
      size_t i = 0;                                                            \
      while (i < num_elements) {                                               \
        /* For some reason materials in d3plot are parts -_('_')_-*/           \
        if (els[i].material_index == part_index) {                             \
          /* TODO: Insert these element ids sorted*/                           \
          part.part_num++;                                                     \
          part.part_ids =                                                      \
              realloc(part.part_ids, part.part_num * sizeof(d3_word));         \
          part.part_indices =                                                  \
              realloc(part.part_indices, part.part_num * sizeof(size_t));      \
          part.part_ids[part.part_num - 1] = ids[i];                           \
          part.part_indices[part.part_num - 1] = i;                            \
        }                                                                      \
                                                                               \
        i++;                                                                   \
      }                                                                        \
    }                                                                          \
                                                                               \
    free(ids);                                                                 \
    free(els);                                                                 \
  }

d3plot_part d3plot_read_part(d3plot_file *plot_file, size_t part_index) {
  BEGIN_PROFILE_FUNC();
  D3PLOT_CLEAR_ERROR_STRING();

  d3plot_part part;
  part.solid_ids = NULL;
  part.thick_shell_ids = NULL;
  part.beam_ids = NULL;
  part.shell_ids = NULL;
  part.solid_indices = NULL;
  part.thick_shell_indices = NULL;
  part.beam_indices = NULL;
  part.shell_indices = NULL;
  part.num_solids = 0;
  part.num_thick_shells = 0;
  part.num_beams = 0;
  part.num_shells = 0;

  size_t num_elements;
  d3_word *ids;

  ADD_ELEMENTS_TO_PART(d3plot_read_solid_element_ids,
                       d3plot_read_solid_elements, d3plot_solid_con, num_solids,
                       solid_ids, solid_indices);
  ADD_ELEMENTS_TO_PART(d3plot_read_thick_shell_element_ids,
                       d3plot_read_thick_shell_elements, d3plot_thick_shell_con,
                       num_thick_shells, thick_shell_ids, thick_shell_indices);
  ADD_ELEMENTS_TO_PART(d3plot_read_beam_element_ids, d3plot_read_beam_elements,
                       d3plot_beam_con, num_beams, beam_ids, beam_indices);
  ADD_ELEMENTS_TO_PART(d3plot_read_shell_element_ids,
                       d3plot_read_shell_elements, d3plot_shell_con, num_shells,
                       shell_ids, shell_indices);

  /* If no elements have been found, this means that the part with the given
   * index does not exist*/
  if (part.num_solids == 0 && part.num_thick_shells == 0 &&
      part.num_beams == 0 && part.num_shells == 0) {
    ERROR_AND_NO_RETURN_F_PTR("The part with index %zu does not exist",
                              part_index);
    END_PROFILE_FUNC();
    return part;
  }

  END_PROFILE_FUNC();
  return part;
}

d3plot_part d3plot_read_part_by_id(d3plot_file *plot_file, d3_word part_id,
                                   const d3_word *part_ids, size_t num_parts) {
  BEGIN_PROFILE_FUNC();
  D3PLOT_CLEAR_ERROR_STRING();

  d3plot_part part = {0};

  d3_word *p_part_ids = part_ids ? (d3_word *)part_ids
                                 : d3plot_read_part_ids(plot_file, &num_parts);
  if (plot_file->error_string) {
    END_PROFILE_FUNC();
    return part;
  }

  if (num_parts == 0) {
    ERROR_AND_NO_RETURN_PTR("This d3plot does not have any parts");
    END_PROFILE_FUNC();
    return part;
  }

  const size_t index =
      d3_word_binary_search(p_part_ids, 0, num_parts - 1, part_id);
  if (!part_ids) {
    free(p_part_ids);
  }

  if (index == ~0) {
    ERROR_AND_NO_RETURN_F_PTR("The part id %llu has not been found", part_id);
    END_PROFILE_FUNC();
    return part;
  }

  part = d3plot_read_part(plot_file, index);

  END_PROFILE_FUNC();
  return part;
}

size_t d3plot_index_for_id(d3_word id, const d3_word *ids, size_t num_ids) {
  BEGIN_PROFILE_FUNC();

  const size_t index = d3_word_binary_search(ids, 0, num_ids - 1, id);

  END_PROFILE_FUNC();
  return index;
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
  BEGIN_PROFILE_FUNC();

  int i = 0;
  while (1) {
    d3_word last_value = value;
    value /= 10;
    if (i == n) {
      value = last_value - value * 10;

      END_PROFILE_FUNC();
      return value;
    }

    i++;
  }
}

double *_d3plot_read_node_data(d3plot_file *plot_file, size_t state,
                               size_t *num_nodes, size_t data_type) {
  D3PLOT_CLEAR_ERROR_STRING();

  if (plot_file->data_pointers[data_type] == 0) {
    ERROR_AND_NO_RETURN_F_PTR(
        "This node data is not present IU=%llu IV=%llu IA=%llu",
        plot_file->control_data.iu, plot_file->control_data.iv,
        plot_file->control_data.ia);

    return NULL;
  }

  if (plot_file->buffer.word_size == 4) {
    float *coords32 =
        _d3plot_read_node_data_32(plot_file, state, num_nodes, data_type);
    if (!coords32) {
      return NULL;
    }

    double *coords = malloc(*num_nodes * 3 * sizeof(double));
    size_t i = 0;
    while (i < *num_nodes) {
      coords[i * 3 + 0] = coords32[i * 3 + 0];
      coords[i * 3 + 1] = coords32[i * 3 + 1];
      coords[i * 3 + 2] = coords32[i * 3 + 2];

      i++;
    }

    free(coords32);

    return coords;
  }

  if (state >= plot_file->num_states) {
    ERROR_AND_NO_RETURN_F_PTR("%zu is out of bounds for the states", state);
    return NULL;
  }

  *num_nodes = plot_file->control_data.numnp;
  double *coords = malloc(*num_nodes * 3 * sizeof(double));

  d3_pointer d3_ptr = d3_buffer_read_words_at(
      &plot_file->buffer, coords, *num_nodes * 3,
      plot_file->data_pointers[D3PLT_PTR_STATES + state] +
          plot_file->data_pointers[data_type]);
  d3_pointer_close(&plot_file->buffer, &d3_ptr);
  if (plot_file->buffer.error_string) {
    ERROR_AND_NO_RETURN_F_PTR("Failed to read words: %s",
                              plot_file->buffer.error_string);
    *num_nodes = 0;
    free(coords);
    return NULL;
  }

  return coords;
}

float *_d3plot_read_node_data_32(d3plot_file *plot_file, size_t state,
                                 size_t *num_nodes, size_t data_type) {
  D3PLOT_CLEAR_ERROR_STRING();

  if (plot_file->data_pointers[data_type] == 0) {
    ERROR_AND_NO_RETURN_F_PTR(
        "This node data is not present IU=%llu IV=%llu IA=%llu",
        plot_file->control_data.iu, plot_file->control_data.iv,
        plot_file->control_data.ia);

    return NULL;
  }

  if (plot_file->buffer.word_size == 8) {
    double *coords64 =
        _d3plot_read_node_data(plot_file, state, num_nodes, data_type);
    if (!coords64) {
      return NULL;
    }

    float *coords = malloc(*num_nodes * 3 * sizeof(float));
    size_t i = 0;
    while (i < *num_nodes) {
      coords[i * 3 + 0] = coords64[i * 3 + 0];
      coords[i * 3 + 1] = coords64[i * 3 + 1];
      coords[i * 3 + 2] = coords64[i * 3 + 2];

      i++;
    }

    free(coords64);

    return coords;
  }

  if (state >= plot_file->num_states) {
    ERROR_AND_NO_RETURN_F_PTR("%zu is out of bounds for the states", state);
    return NULL;
  }

  *num_nodes = plot_file->control_data.numnp;
  float *coords = malloc(*num_nodes * 3 * sizeof(float));

  d3_pointer d3_ptr = d3_buffer_read_words_at(
      &plot_file->buffer, coords, *num_nodes * 3,
      plot_file->data_pointers[D3PLT_PTR_STATES + state] +
          plot_file->data_pointers[data_type]);
  d3_pointer_close(&plot_file->buffer, &d3_ptr);
  if (plot_file->buffer.error_string) {
    ERROR_AND_NO_RETURN_F_PTR("Failed to read words: %s",
                              plot_file->buffer.error_string);
    *num_nodes = 0;
    free(coords);
    return NULL;
  }

  return coords;
}

d3_word *_d3plot_read_ids(d3plot_file *plot_file, size_t *num_ids,
                          size_t data_type, size_t num_ids_value) {
  D3PLOT_CLEAR_ERROR_STRING();

  *num_ids = num_ids_value;
  if (num_ids_value == 0) {
    return NULL;
  }

  d3_word *ids = malloc(*num_ids * sizeof(d3_word));
  if (plot_file->buffer.word_size == 4) {
    uint32_t *ids32 = malloc(*num_ids * plot_file->buffer.word_size);
    d3_pointer d3_ptr =
        d3_buffer_read_words_at(&plot_file->buffer, ids32, *num_ids,
                                plot_file->data_pointers[data_type]);
    d3_pointer_close(&plot_file->buffer, &d3_ptr);
    if (plot_file->buffer.error_string) {
      ERROR_AND_NO_RETURN_F_PTR("Failed to read words: %s",
                                plot_file->buffer.error_string);
      *num_ids = 0;
      free(ids32);
      free(ids);
      return NULL;
    }

    size_t i = 0;
    while (i < *num_ids) {
      ids[i] = ids32[i];

      i++;
    }

    free(ids32);
  } else {
    d3_pointer d3_ptr = d3_buffer_read_words_at(
        &plot_file->buffer, ids, *num_ids, plot_file->data_pointers[data_type]);
    d3_pointer_close(&plot_file->buffer, &d3_ptr);
    if (plot_file->buffer.error_string) {
      ERROR_AND_NO_RETURN_F_PTR("Failed to read words: %s",
                                plot_file->buffer.error_string);
      *num_ids = 0;
      free(ids);
      return NULL;
    }
  }

  return ids;
}

#define SWAP(lhs, rhs)                                                         \
  d3_word temp = lhs;                                                          \
  lhs = rhs;                                                                   \
  rhs = temp

d3_word *_insert_sorted(d3_word *dst, size_t dst_size, const d3_word *src,
                        size_t src_size) {
  BEGIN_PROFILE_FUNC();

  if (!dst) {
    dst = malloc(src_size * sizeof(d3_word));
    memcpy(dst, src, src_size * sizeof(d3_word));

    END_PROFILE_FUNC();
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

  END_PROFILE_FUNC();
  return dst;
}

void d3plot_free_part(d3plot_part *part) {
  BEGIN_PROFILE_FUNC();

  free(part->solid_ids);
  free(part->thick_shell_ids);
  free(part->beam_ids);
  free(part->shell_ids);

  free(part->solid_indices);
  free(part->thick_shell_indices);
  free(part->beam_indices);
  free(part->shell_indices);

  part->solid_ids = NULL;
  part->thick_shell_ids = NULL;
  part->beam_ids = NULL;
  part->shell_ids = NULL;
  part->solid_indices = NULL;
  part->thick_shell_indices = NULL;
  part->beam_indices = NULL;
  part->shell_indices = NULL;
  part->num_solids = 0;
  part->num_thick_shells = 0;
  part->num_beams = 0;
  part->num_shells = 0;

  END_PROFILE_FUNC();
}

void d3plot_free_shells_state(d3plot_shell *shells) {
  BEGIN_PROFILE_FUNC();

  if (shells) {
    free(shells->mid.history_variables);
    free(shells->add_ips);
    free(shells);
  }

  END_PROFILE_FUNC();
}

void d3plot_free_thick_shells_state(d3plot_thick_shell *thick_shells) {
  BEGIN_PROFILE_FUNC();

  if (thick_shells) {
    free(thick_shells->mid.history_variables);
    free(thick_shells->add_ips);
    free(thick_shells);
  }

  END_PROFILE_FUNC();
}

void d3plot_free_beams_state(d3plot_beam *beams) {
  BEGIN_PROFILE_FUNC();

  if (beams) {
    if (beams->ips) {
      free(beams->ips->history_variables);
      free(beams->ips);
    }
    free(beams);
  }

  END_PROFILE_FUNC();
}