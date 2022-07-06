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

#define READ_MAT_VALUE(num, name, variable, variable_name)                     \
  i = 0;                                                                       \
  while (i < CDP.num) {                                                        \
    d3_buffer_read_double_word(&plot_file->buffer, &variable);                 \
                                                                               \
    i++;                                                                       \
  }
#define CDP plot_file->control_data

int _d3plot_read_state_data(d3plot_file *plot_file) {
  const size_t state_start = plot_file->buffer.cur_word;

  double time;
  d3_buffer_read_double_word(&plot_file->buffer, &time);

  if (time == D3_EOF) {
    printf("EOF read at %d\n", plot_file->buffer.cur_word);
    return 2;
  }

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

  size_t i = 0;
  while (i < CDP.nummat8) {
    d3_buffer_read_double_word(&plot_file->buffer, &ie);

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
    numrw = (CDP.nglbv - 6 -
             7 * (CDP.nummat8 + CDP.nummat2 + CDP.nummat4 + CDP.nummatt +
                  CDP.numrbs)) /
            RWN;
  }

  i = 0;
  while (i < numrw) {
    d3_buffer_read_double_word(&plot_file->buffer, &force);

    i++;
  }

  if (RWN == 4) {
    i = 0;
    double pos[3];
    while (i < numrw) {
      d3_buffer_read_double_word(&plot_file->buffer, &pos[0]);
      d3_buffer_read_double_word(&plot_file->buffer, &pos[1]);
      d3_buffer_read_double_word(&plot_file->buffer, &pos[2]);

      i++;
    }
  }

  const size_t global_end = plot_file->buffer.cur_word;
  const size_t global_size = global_end - global_start;

  if (global_size != CDP.nglbv) {
    plot_file->error_string = malloc(70);
    sprintf(plot_file->error_string, "Size of GLOBAL is %d instead of %d",
            global_size, CDP.nglbv);
    fflush(stdout);
    return 0;
  }

  /* NODEDATA*/
  /**** Order of Node Data ******
   * IT, U, Mass Scaling, V, A
   ******************************/

  printf("Read node data at 0x%x\n",
         plot_file->buffer.cur_word * plot_file->buffer.word_size);

  const size_t node_data_start = plot_file->buffer.cur_word;

  size_t it = _get_nth_digit(CDP.it, 0);
  size_t N = it * (it > 1);
  if (N == 2) {
    it = 1;
    N = 3;
  }
  const size_t mass_N = _get_nth_digit(CDP.it, 1) == 1;

  const size_t NND =
      ((it + N + mass_N) + CDP.ndim * (CDP.iu + CDP.iv + CDP.ia)) * CDP.numnp;

  double temp[3], node_flux[3], mass_scaling, u[3], v[3], a[3];

  size_t n = 0;
  if (it > 0) {
    while (n < CDP.numnp) {
      size_t j = 0;
      while (j < it) {
        d3_buffer_read_double_word(&plot_file->buffer, &temp[j]);

        j++;
      }

      n++;
    }
  }

  if (N > 0) {
    n = 0;
    while (n < CDP.numnp) {
      size_t j = 0;
      while (j < N) {
        d3_buffer_read_double_word(&plot_file->buffer, &node_flux[j]);

        j++;
      }

      n++;
    }
  }

  if (mass_N) {
    n = 0;
    while (n < CDP.numnp) {
      d3_buffer_read_double_word(&plot_file->buffer, &mass_scaling);

      n++;
    }
  }

  if (CDP.iu) {
    n = 0;
    while (n < CDP.numnp) {
      d3_buffer_read_vec3(&plot_file->buffer, u);
      if (n < 5)
        printf("NODE %d DISPLACEMENT: (%f, %f, %f)\n", n, u[0], u[1], u[2]);

      n++;
    }
  }

  if (CDP.iv) {
    n = 0;
    while (n < CDP.numnp) {
      d3_buffer_read_vec3(&plot_file->buffer, v);
      if (n < 5)
        printf("NODE %d VELOCITY: (%f, %f, %f)\n", n, v[0], v[1], v[2]);

      n++;
    }
  }

  if (CDP.ia) {
    n = 0;
    while (n < CDP.numnp) {
      d3_buffer_read_vec3(&plot_file->buffer, a);
      if (n < 5)
        printf("NODE %d ACCELERATION: (%f, %f, %f)\n", n, a[0], a[1], a[2]);

      n++;
    }
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
  while (n < CDP.nt3d) {
    size_t e = 0;
    while (e < CDP.nel8) {
      d3_buffer_read_double_word(&plot_file->buffer, &nt3d);

      e++;
    }

    n++;
  }

  /* CFDDATA is no longer output*/

  /* ELEMDATA*/
  const size_t ENN =
      CDP.nel8 * CDP.nv3d + CDP.nelt * CDP.nv3dt + CDP.nel2 * CDP.nv1d +
      CDP.nel4 * CDP.nv2d +
      CDP.nmsph * 0; /* We don't support SMOOTH PATICLE HYDRODYNAMICS*/
  const size_t elem_data_start = plot_file->buffer.cur_word;

  size_t e = 0;
  while (e < CDP.nel8) {
    double *e_data = malloc(CDP.nv3d * sizeof(double));
    if (plot_file->buffer.word_size == 4) {
      float *e_data32 = malloc(CDP.nv3d * sizeof(float));
      d3_buffer_read_words(&plot_file->buffer, e_data32, CDP.nv3d);
      size_t j = 0;
      while (j < CDP.nv3d) {
        e_data[j] = e_data32[j];

        j++;
      }

      free(e_data32);
    } else {
      d3_buffer_read_words(&plot_file->buffer, e_data, CDP.nv3d);
    }

    free(e_data);

    e++;
  }

  e = 0;
  while (e < CDP.nel2) {
    double *e_data = malloc(CDP.nv1d * sizeof(double));
    if (plot_file->buffer.word_size == 4) {
      float *e_data32 = malloc(CDP.nv1d * sizeof(float));
      d3_buffer_read_words(&plot_file->buffer, e_data32, CDP.nv1d);
      size_t j = 0;
      while (j < CDP.nv1d) {
        e_data[j] = e_data32[j];

        j++;
      }

      free(e_data32);
    } else {
      d3_buffer_read_words(&plot_file->buffer, e_data, CDP.nv1d);
    }

    free(e_data);

    e++;
  }

  e = 0;
  while (e < CDP.nel4) {
    double *e_data = malloc(CDP.nv2d * sizeof(double));
    if (plot_file->buffer.word_size == 4) {
      float *e_data32 = malloc(CDP.nv2d * sizeof(float));
      d3_buffer_read_words(&plot_file->buffer, e_data32, CDP.nv2d);
      size_t j = 0;
      while (j < CDP.nv2d) {
        e_data[j] = e_data32[j];

        j++;
      }

      free(e_data32);
    } else {
      d3_buffer_read_words(&plot_file->buffer, e_data, CDP.nv2d);
    }

    free(e_data);

    e++;
  }

  /* Then follows who knows what -_(′_′)_-*/
  /* But because we don't support NMSPH, we can assume that NELT follows*/
  e = 0;
  while (e < CDP.nelt) {
    double *e_data = malloc(CDP.nv3dt * sizeof(double));
    if (plot_file->buffer.word_size == 4) {
      float *e_data32 = malloc(CDP.nv3dt * sizeof(float));
      d3_buffer_read_words(&plot_file->buffer, e_data32, CDP.nv3dt);
      size_t j = 0;
      while (j < CDP.nv3dt) {
        e_data[j] = e_data32[j];

        j++;
      }

      free(e_data32);
    } else {
      d3_buffer_read_words(&plot_file->buffer, e_data, CDP.nv3dt);
    }

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

  fflush(stdout);

  return 1;
}