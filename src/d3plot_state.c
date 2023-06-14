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

#ifndef CDP
#define CDP plot_file->control_data
#endif
#ifdef DT_PTR_SET
#undef DT_PTR_SET
#endif
#define DT_PTR_SET(value)                                                      \
  if (plot_file->num_states == 1)                                              \
  plot_file->data_pointers[value] = d3_ptr->cur_word - state_start

#include "d3plot_error_macros.h"

int _d3plot_read_state_data(d3plot_file *plot_file, d3_pointer *d3_ptr) {
  BEGIN_PROFILE_FUNC();

  const size_t state_start = d3_ptr->cur_word;

  double time;
  d3_buffer_read_double_word(&plot_file->buffer, d3_ptr, &time);
  if (plot_file->buffer.error_string) {
    ERROR_AND_NO_RETURN_F_PTR("Failed to read time: %s",
                              plot_file->buffer.error_string);
    END_PROFILE_FUNC();
    return 0;
  }

  if (time == D3_EOF) {
    END_PROFILE_FUNC();
    return 2;
  }

  plot_file->num_states++;
  plot_file->data_pointers =
      realloc(plot_file->data_pointers,
              (D3PLT_PTR_COUNT + plot_file->num_states) * sizeof(size_t));
  plot_file->data_pointers[D3PLT_PTR_STATES + plot_file->num_states - 1] =
      state_start;

  /* GLOBAL*/
  const size_t global_start = d3_ptr->cur_word;

  d3_buffer_skip_words(&plot_file->buffer, d3_ptr, 6);
  /* TODO: read functions for KE, IE, TE, X, Y and Z*/

  d3_buffer_skip_words(&plot_file->buffer, d3_ptr, CDP.nummat8);
  /* TODO: read function for MAT8 IE*/
  d3_buffer_skip_words(&plot_file->buffer, d3_ptr, CDP.nummat2);
  /* TODO: read function for MAT2 IE*/
  d3_buffer_skip_words(&plot_file->buffer, d3_ptr, CDP.nummat4);
  /* TODO: read function for MAT4 IE*/
  d3_buffer_skip_words(&plot_file->buffer, d3_ptr, CDP.nummatt);
  /* TODO: read function for MATT IE*/
  d3_buffer_skip_words(&plot_file->buffer, d3_ptr, CDP.numrbs);
  /* TODO: read function for RBS IE*/

  d3_buffer_skip_words(&plot_file->buffer, d3_ptr, CDP.nummat8);
  /* TODO: read function for MAT8 KE*/
  d3_buffer_skip_words(&plot_file->buffer, d3_ptr, CDP.nummat2);
  /* TODO: read function for MAT2 KE*/
  d3_buffer_skip_words(&plot_file->buffer, d3_ptr, CDP.nummat4);
  /* TODO: read function for MAT4 KE*/
  d3_buffer_skip_words(&plot_file->buffer, d3_ptr, CDP.nummatt);
  /* TODO: read function for MATT KE*/
  d3_buffer_skip_words(&plot_file->buffer, d3_ptr, CDP.numrbs);
  /* TODO: read function for RBS KE*/

  d3_buffer_skip_words(&plot_file->buffer, d3_ptr, CDP.nummat8);
  /* TODO: read function for MAT8 X*/
  d3_buffer_skip_words(&plot_file->buffer, d3_ptr, CDP.nummat2);
  /* TODO: read function for MAT2 X*/
  d3_buffer_skip_words(&plot_file->buffer, d3_ptr, CDP.nummat4);
  /* TODO: read function for MAT4 X*/
  d3_buffer_skip_words(&plot_file->buffer, d3_ptr, CDP.nummatt);
  /* TODO: read function for MATT X*/
  d3_buffer_skip_words(&plot_file->buffer, d3_ptr, CDP.numrbs);
  /* TODO: read function for RBS X*/

  d3_buffer_skip_words(&plot_file->buffer, d3_ptr, CDP.nummat8);
  /* TODO: read function for MAT8 Y*/
  d3_buffer_skip_words(&plot_file->buffer, d3_ptr, CDP.nummat2);
  /* TODO: read function for MAT2 Y*/
  d3_buffer_skip_words(&plot_file->buffer, d3_ptr, CDP.nummat4);
  /* TODO: read function for MAT4 Y*/
  d3_buffer_skip_words(&plot_file->buffer, d3_ptr, CDP.nummatt);
  /* TODO: read function for MATT Y*/
  d3_buffer_skip_words(&plot_file->buffer, d3_ptr, CDP.numrbs);
  /* TODO: read function for RBS Y*/

  d3_buffer_skip_words(&plot_file->buffer, d3_ptr, CDP.nummat8);
  /* TODO: read function for MAT8 Z*/
  d3_buffer_skip_words(&plot_file->buffer, d3_ptr, CDP.nummat2);
  /* TODO: read function for MAT2 Z*/
  d3_buffer_skip_words(&plot_file->buffer, d3_ptr, CDP.nummat4);
  /* TODO: read function for MAT4 Z*/
  d3_buffer_skip_words(&plot_file->buffer, d3_ptr, CDP.nummatt);
  /* TODO: read function for MATT Z*/
  d3_buffer_skip_words(&plot_file->buffer, d3_ptr, CDP.numrbs);
  /* TODO: read function for RBS Z*/

  d3_buffer_skip_words(&plot_file->buffer, d3_ptr, CDP.nummat8);
  /* TODO: read function for MAT8 MASS*/
  d3_buffer_skip_words(&plot_file->buffer, d3_ptr, CDP.nummat2);
  /* TODO: read function for MAT2 MASS*/
  d3_buffer_skip_words(&plot_file->buffer, d3_ptr, CDP.nummat4);
  /* TODO: read function for MAT4 MASS*/
  d3_buffer_skip_words(&plot_file->buffer, d3_ptr, CDP.nummatt);
  /* TODO: read function for MATT MASS*/
  d3_buffer_skip_words(&plot_file->buffer, d3_ptr, CDP.numrbs);
  /* TODO: read function for RBS MASS*/

  d3_buffer_skip_words(&plot_file->buffer, d3_ptr, CDP.nummat8);
  /* TODO: read function for MAT8 FORCE*/
  d3_buffer_skip_words(&plot_file->buffer, d3_ptr, CDP.nummat2);
  /* TODO: read function for MAT2 FORCE*/
  d3_buffer_skip_words(&plot_file->buffer, d3_ptr, CDP.nummat4);
  /* TODO: read function for MAT4 FORCE*/
  d3_buffer_skip_words(&plot_file->buffer, d3_ptr, CDP.nummatt);
  /* TODO: read function for MATT FORCE*/
  d3_buffer_skip_words(&plot_file->buffer, d3_ptr, CDP.numrbs);
  /* TODO: read function for RBS FORCE*/

  if (plot_file->buffer.error_string) {
    ERROR_AND_NO_RETURN_F_PTR("Failed to skip words: %s",
                              plot_file->buffer.error_string);
    END_PROFILE_FUNC();
    return 0;
  }

  /* Assume that N is one*/
  const size_t RWN = 1;
  size_t numrw;
  {
    numrw = (CDP.nglbv - 6 -
             7 * (CDP.nummat8 + CDP.nummat2 + CDP.nummat4 + CDP.nummatt +
                  CDP.numrbs)) /
            RWN;
  }

  d3_buffer_skip_words(&plot_file->buffer, d3_ptr, numrw);
  /* TODO: read function for RW_FORCE*/

  if (RWN == 4) {
    d3_buffer_skip_words(&plot_file->buffer, d3_ptr, numrw * 3);
    /* TODO: read function for RW_POS*/
  }

  if (plot_file->buffer.error_string) {
    ERROR_AND_NO_RETURN_F_PTR("Failed to skip RW: %s",
                              plot_file->buffer.error_string);
    END_PROFILE_FUNC();
    return 0;
  }

  const size_t global_end = d3_ptr->cur_word;
  const size_t global_size = global_end - global_start;

  if (global_size != CDP.nglbv) {
    ERROR_AND_NO_RETURN_F_PTR("Size of GLOBAL is %zu instead of %llu",
                              global_size, CDP.nglbv);
    END_PROFILE_FUNC();
    return 0;
  }

  /* NODEDATA*/
  /**** Order of Node Data ******
   * IT, U, Mass Scaling, V, A
   ******************************/

  const size_t node_data_start = d3_ptr->cur_word;

  uint8_t it = _get_nth_digit(CDP.it, 0);
  uint8_t N = it * (it > 1);
  if (N == 2) {
    it = 1;
    N = 3;
  }
  const uint8_t mass_N = _get_nth_digit(CDP.it, 1) == 1;

  const size_t NND =
      ((it + N + mass_N) + CDP.ndim * (CDP.iu + CDP.iv + CDP.ia)) * CDP.numnp;

  if (it > 0) {
    d3_buffer_skip_words(&plot_file->buffer, d3_ptr, it * CDP.numnp);
    /* TODO: read function for IT data*/
  }

  if (N > 0) {
    d3_buffer_skip_words(&plot_file->buffer, d3_ptr, N * CDP.numnp);
    /* TODO: read function for NODE FLUX data*/
  }

  if (mass_N) {
    d3_buffer_skip_words(&plot_file->buffer, d3_ptr, CDP.numnp);
    /* TODO: read function for MASS SCALING*/
  }

  if (CDP.iu) {
    DT_PTR_SET(D3PLT_PTR_STATE_NODE_COORDS);
    d3_buffer_skip_words(&plot_file->buffer, d3_ptr, 3 * CDP.numnp);
  }

  if (CDP.iv) {
    DT_PTR_SET(D3PLT_PTR_STATE_NODE_VEL);
    d3_buffer_skip_words(&plot_file->buffer, d3_ptr, 3 * CDP.numnp);
  }

  if (CDP.ia) {
    DT_PTR_SET(D3PLT_PTR_STATE_NODE_ACC);
    d3_buffer_skip_words(&plot_file->buffer, d3_ptr, 3 * CDP.numnp);
  }

  if (plot_file->buffer.error_string) {
    ERROR_AND_NO_RETURN_F_PTR("Failed to skip words: %s",
                              plot_file->buffer.error_string);
    END_PROFILE_FUNC();
    return 0;
  }

  const size_t node_data_end = d3_ptr->cur_word;
  const size_t node_data_size = node_data_end - node_data_start;
  if (node_data_size != NND) {
    ERROR_AND_NO_RETURN_F_PTR("NODEDATA should be %zu instead of %zu", NND,
                              node_data_size);
    END_PROFILE_FUNC();
    return 0;
  }

  /* THERMDATA*/
  d3_buffer_skip_words(&plot_file->buffer, d3_ptr, CDP.nt3d * CDP.nel8);
  /* TODO: read function for nt3d data*/

  if (plot_file->buffer.error_string) {
    ERROR_AND_NO_RETURN_F_PTR("Failed to skip THERMDATA: %s",
                              plot_file->buffer.error_string);
    END_PROFILE_FUNC();
    return 0;
  }

  /* CFDDATA is no longer output*/

  /* ELEMDATA*/
  const size_t ENN =
      CDP.nel8 * CDP.nv3d + CDP.nelt * CDP.nv3dt + CDP.nel2 * CDP.nv1d +
      CDP.nel4 * CDP.nv2d +
      CDP.nmsph * 0; /* We don't support SMOOTH PARTICLE HYDRODYNAMICS*/
  const size_t elem_data_start = d3_ptr->cur_word;

  DT_PTR_SET(D3PLT_PTR_STATE_ELEMENT_SOLID);
  d3_buffer_skip_words(&plot_file->buffer, d3_ptr, CDP.nv3d * CDP.nel8);

  DT_PTR_SET(D3PLT_PTR_STATE_ELEMENT_BEAM);
  d3_buffer_skip_words(&plot_file->buffer, d3_ptr, CDP.nv1d * CDP.nel2);

  DT_PTR_SET(D3PLT_PTR_STATE_ELEMENT_SHELL);
  d3_buffer_skip_words(&plot_file->buffer, d3_ptr, CDP.nv2d * CDP.nel4);

  /* Then follows who knows what -_(′_′)_-*/
  /* But because we don't support NMSPH, we can assume that NELT follows*/
  DT_PTR_SET(D3PLT_PTR_STATE_ELEMENT_THICK_SHELL);
  d3_buffer_skip_words(&plot_file->buffer, d3_ptr, CDP.nv3dt * CDP.nelt);

  if (plot_file->buffer.error_string) {
    ERROR_AND_NO_RETURN_F_PTR("Failed to skip ELEMDATA: %s",
                              plot_file->buffer.error_string);
    END_PROFILE_FUNC();
    return 0;
  }

  const size_t elem_data_end = d3_ptr->cur_word;
  const size_t elem_data_size = elem_data_end - elem_data_start;
  if (elem_data_size < ENN) {
    ERROR_AND_NO_RETURN_F_PTR("ELEMDATA should be %zu instead of %zu", ENN,
                              elem_data_size);
    END_PROFILE_FUNC();
    return 0;
  }

  /* Element Deletion Option*/
  size_t skip_words;
  if (CDP.mdlopt == 0) {
    skip_words = 0;
  } else if (CDP.mdlopt == 1) {
    skip_words = CDP.numnp;
  } else if (CDP.mdlopt == 2) {
    skip_words = CDP.nel8 + CDP.nelt + CDP.nel4 + CDP.nel2;
  } else {
    ERROR_AND_NO_RETURN_F_PTR("The value of MDLOPT is invalid: %d", CDP.mdlopt);
    END_PROFILE_FUNC();
    return 0;
  }

  if (skip_words > 0) {
    d3_buffer_skip_words(&plot_file->buffer, d3_ptr, skip_words);
    if (plot_file->buffer.error_string) {
      ERROR_AND_NO_RETURN_F_PTR("Failed to skip Element Deletion Option: %s",
                                plot_file->buffer.error_string);
      END_PROFILE_FUNC();
      return 0;
    }
  }

  const size_t state_end = d3_ptr->cur_word;
  const size_t state_size =
      (state_end - state_start) * plot_file->buffer.word_size;

  END_PROFILE_FUNC();
  return 1;
}
