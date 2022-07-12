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

#define CDP plot_file->control_data

int _d3plot_read_state_data(d3plot_file *plot_file) {
  const size_t state_start = plot_file->buffer.cur_word;

  double time;
  d3_buffer_read_double_word(&plot_file->buffer, &time);

  if (time == D3_EOF) {
    return 2;
  }

  plot_file->num_states++;
  plot_file->data_pointers =
      realloc(plot_file->data_pointers,
              (D3PLT_PTR_COUNT + plot_file->num_states) * sizeof(size_t));
  plot_file->data_pointers[D3PLT_PTR_STATES + plot_file->num_states - 1] =
      state_start;

  /* GLOBAL*/
  const size_t global_start = plot_file->buffer.cur_word;

  double ke, ie, te, x, y, z, mass, force;
  d3_buffer_skip_words(&plot_file->buffer, 6);
  /* TODO: read functions for KE, IE, TE, X, Y and Z*/

  d3_buffer_skip_words(&plot_file->buffer, CDP.nummat8);
  /* TODO: read function for MAT8 IE*/
  d3_buffer_skip_words(&plot_file->buffer, CDP.nummat2);
  /* TODO: read function for MAT2 IE*/
  d3_buffer_skip_words(&plot_file->buffer, CDP.nummat4);
  /* TODO: read function for MAT4 IE*/
  d3_buffer_skip_words(&plot_file->buffer, CDP.nummatt);
  /* TODO: read function for MATT IE*/
  d3_buffer_skip_words(&plot_file->buffer, CDP.numrbs);
  /* TODO: read function for RBS IE*/

  d3_buffer_skip_words(&plot_file->buffer, CDP.nummat8);
  /* TODO: read function for MAT8 KE*/
  d3_buffer_skip_words(&plot_file->buffer, CDP.nummat2);
  /* TODO: read function for MAT2 KE*/
  d3_buffer_skip_words(&plot_file->buffer, CDP.nummat4);
  /* TODO: read function for MAT4 KE*/
  d3_buffer_skip_words(&plot_file->buffer, CDP.nummatt);
  /* TODO: read function for MATT KE*/
  d3_buffer_skip_words(&plot_file->buffer, CDP.numrbs);
  /* TODO: read function for RBS KE*/

  d3_buffer_skip_words(&plot_file->buffer, CDP.nummat8);
  /* TODO: read function for MAT8 X*/
  d3_buffer_skip_words(&plot_file->buffer, CDP.nummat2);
  /* TODO: read function for MAT2 X*/
  d3_buffer_skip_words(&plot_file->buffer, CDP.nummat4);
  /* TODO: read function for MAT4 X*/
  d3_buffer_skip_words(&plot_file->buffer, CDP.nummatt);
  /* TODO: read function for MATT X*/
  d3_buffer_skip_words(&plot_file->buffer, CDP.numrbs);
  /* TODO: read function for RBS X*/

  d3_buffer_skip_words(&plot_file->buffer, CDP.nummat8);
  /* TODO: read function for MAT8 Y*/
  d3_buffer_skip_words(&plot_file->buffer, CDP.nummat2);
  /* TODO: read function for MAT2 Y*/
  d3_buffer_skip_words(&plot_file->buffer, CDP.nummat4);
  /* TODO: read function for MAT4 Y*/
  d3_buffer_skip_words(&plot_file->buffer, CDP.nummatt);
  /* TODO: read function for MATT Y*/
  d3_buffer_skip_words(&plot_file->buffer, CDP.numrbs);
  /* TODO: read function for RBS Y*/

  d3_buffer_skip_words(&plot_file->buffer, CDP.nummat8);
  /* TODO: read function for MAT8 Z*/
  d3_buffer_skip_words(&plot_file->buffer, CDP.nummat2);
  /* TODO: read function for MAT2 Z*/
  d3_buffer_skip_words(&plot_file->buffer, CDP.nummat4);
  /* TODO: read function for MAT4 Z*/
  d3_buffer_skip_words(&plot_file->buffer, CDP.nummatt);
  /* TODO: read function for MATT Z*/
  d3_buffer_skip_words(&plot_file->buffer, CDP.numrbs);
  /* TODO: read function for RBS Z*/

  d3_buffer_skip_words(&plot_file->buffer, CDP.nummat8);
  /* TODO: read function for MAT8 MASS*/
  d3_buffer_skip_words(&plot_file->buffer, CDP.nummat2);
  /* TODO: read function for MAT2 MASS*/
  d3_buffer_skip_words(&plot_file->buffer, CDP.nummat4);
  /* TODO: read function for MAT4 MASS*/
  d3_buffer_skip_words(&plot_file->buffer, CDP.nummatt);
  /* TODO: read function for MATT MASS*/
  d3_buffer_skip_words(&plot_file->buffer, CDP.numrbs);
  /* TODO: read function for RBS MASS*/

  d3_buffer_skip_words(&plot_file->buffer, CDP.nummat8);
  /* TODO: read function for MAT8 FORCE*/
  d3_buffer_skip_words(&plot_file->buffer, CDP.nummat2);
  /* TODO: read function for MAT2 FORCE*/
  d3_buffer_skip_words(&plot_file->buffer, CDP.nummat4);
  /* TODO: read function for MAT4 FORCE*/
  d3_buffer_skip_words(&plot_file->buffer, CDP.nummatt);
  /* TODO: read function for MATT FORCE*/
  d3_buffer_skip_words(&plot_file->buffer, CDP.numrbs);
  /* TODO: read function for RBS FORCE*/

  /* Assume that N is one*/
  const size_t RWN = 1;
  size_t numrw;
  {
    numrw = (CDP.nglbv - 6 -
             7 * (CDP.nummat8 + CDP.nummat2 + CDP.nummat4 + CDP.nummatt +
                  CDP.numrbs)) /
            RWN;
  }

  d3_buffer_skip_words(&plot_file->buffer, numrw);
  /* TODO: read function for RW_FORCE*/

  if (RWN == 4) {
    d3_buffer_skip_words(&plot_file->buffer, numrw * 3);
    /* TODO: read function for RW_POS*/
  }

  const size_t global_end = plot_file->buffer.cur_word;
  const size_t global_size = global_end - global_start;

  if (global_size != CDP.nglbv) {
    plot_file->error_string = malloc(70);
    sprintf(plot_file->error_string, "Size of GLOBAL is %d instead of %d",
            global_size, CDP.nglbv);
    return 0;
  }

  /* NODEDATA*/
  /**** Order of Node Data ******
   * IT, U, Mass Scaling, V, A
   ******************************/

  const size_t node_data_start = plot_file->buffer.cur_word;

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
    d3_buffer_skip_words(&plot_file->buffer, it * CDP.numnp);
    /* TODO: read function for IT data*/
  }

  if (N > 0) {
    d3_buffer_skip_words(&plot_file->buffer, N * CDP.numnp);
    /* TODO: read function for NODE FLUX data*/
  }

  if (mass_N) {
    d3_buffer_skip_words(&plot_file->buffer, CDP.numnp);
    /* TODO: read function for MASS SCALING*/
  }

  if (CDP.iu) {
    if (plot_file->num_states == 1)
      plot_file->data_pointers[D3PLT_PTR_STATE_NODE_COORDS] =
          plot_file->buffer.cur_word - state_start;

    d3_buffer_skip_words(&plot_file->buffer, 3 * CDP.numnp);
  }

  if (CDP.iv) {
    if (plot_file->num_states == 1)
      plot_file->data_pointers[D3PLT_PTR_STATE_NODE_VEL] =
          plot_file->buffer.cur_word - state_start;

    d3_buffer_skip_words(&plot_file->buffer, 3 * CDP.numnp);
  }

  if (CDP.ia) {
    if (plot_file->num_states == 1)
      plot_file->data_pointers[D3PLT_PTR_STATE_NODE_ACC] =
          plot_file->buffer.cur_word - state_start;

    d3_buffer_skip_words(&plot_file->buffer, 3 * CDP.numnp);
  }

  const size_t node_data_end = plot_file->buffer.cur_word;
  const size_t node_data_size = node_data_end - node_data_start;
  if (node_data_size != NND) {
    plot_file->error_string = malloc(70);
    sprintf(plot_file->error_string, "NODEDATA should be %d instead of %d", NND,
            node_data_size);
    return 0;
  }

  /* THERMDATA*/
  d3_buffer_skip_words(&plot_file->buffer, CDP.nt3d * CDP.nel8);
  /* TODO: read function for nt3d data*/

  /* CFDDATA is no longer output*/

  /* ELEMDATA*/
  const size_t ENN =
      CDP.nel8 * CDP.nv3d + CDP.nelt * CDP.nv3dt + CDP.nel2 * CDP.nv1d +
      CDP.nel4 * CDP.nv2d +
      CDP.nmsph * 0; /* We don't support SMOOTH PARTICLE HYDRODYNAMICS*/
  const size_t elem_data_start = plot_file->buffer.cur_word;

  if (plot_file->num_states == 1)
    plot_file->data_pointers[D3PLT_PTR_STATE_ELEMENT_SOLID] =
        plot_file->buffer.cur_word - state_start;
  d3_buffer_skip_words(&plot_file->buffer, CDP.nv3d * CDP.nel8);

  d3_buffer_skip_words(&plot_file->buffer, CDP.nv1d * CDP.nel2);
  /* TODO: read function for nel2 data*/

  d3_buffer_skip_words(&plot_file->buffer, CDP.nv2d * CDP.nel4);
  /* TODO: read function for nel4 data*/

  /* Then follows who knows what -_(′_′)_-*/
  /* But because we don't support NMSPH, we can assume that NELT follows*/
  d3_buffer_skip_words(&plot_file->buffer, CDP.nv3dt * CDP.nelt);
  /* TODO: read function for nelt data*/

  const size_t elem_data_end = plot_file->buffer.cur_word;
  const size_t elem_data_size = elem_data_end - elem_data_start;
  if (elem_data_size < ENN) {
    plot_file->error_string = malloc(70);
    sprintf(plot_file->error_string, "ELEMDATA should be %d instead of %d", ENN,
            elem_data_size);
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
    plot_file->error_string = malloc(50);
    sprintf(plot_file->error_string, "The value of MDLOPT is invalid: %d",
            CDP.mdlopt);
    return 0;
  }

  if (skip_words > 0) {
    d3_buffer_skip_words(&plot_file->buffer, skip_words);
  }

  const size_t state_end = plot_file->buffer.cur_word;
  const size_t state_size =
      (state_end - state_start) * plot_file->buffer.word_size;

  return 1;
}