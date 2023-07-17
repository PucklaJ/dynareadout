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

#ifndef D3_DEFINES_H
#define D3_DEFINES_H
#include <stddef.h>
#include <stdint.h>
#ifdef __cplusplus
#include <array>
#endif

typedef uint64_t d3_word;

typedef struct {
  /* Stores indices into the node_ids, node_coords, etc. arrays*/
#ifdef __cplusplus
  std::array<d3_word, 8> node_indices;
#else
  d3_word node_indices[8];
#endif
  /* Index into the parts (this those indeed refer to parts even though the
   * documentation does not say so)*/
  d3_word material_index;
} d3plot_solid_con;

typedef d3plot_solid_con d3plot_thick_shell_con;

typedef struct {
  /* Stores indices into the node_ids, node_coords, etc. arrays*/
#ifdef __cplusplus
  std::array<d3_word, 2> node_indices;
#else
  d3_word node_indices[2];
#endif
  d3_word orientation_node_index;
  d3_word _null[2];
  /* Index into the parts (this those indeed refer to parts even though the
   * documentation does not say so)*/
  d3_word material_index;
} d3plot_beam_con;

typedef struct {
  /* Stores indices into the node_ids, node_coords, etc. arrays*/
#ifdef __cplusplus
  std::array<d3_word, 4> node_indices;
#else
  d3_word node_indices[4];
#endif
  /* Index into the parts (this those indeed refer to parts even though the
   * documentation does not say so)*/
  d3_word material_index;
} d3plot_shell_con;

typedef struct {
  d3_word *solid_ids;
  d3_word *thick_shell_ids;
  d3_word *beam_ids;
  d3_word *shell_ids;

  size_t *solid_indices;
  size_t *thick_shell_indices;
  size_t *beam_indices;
  size_t *shell_indices;

  size_t num_solids;
  size_t num_thick_shells;
  size_t num_beams;
  size_t num_shells;
} d3plot_part;

typedef struct {
  double x;
  double y;
  double z;
  union {
    double xy;
    double yx;
  };
  union {
    double yz;
    double zy;
  };
  union {
    double zx;
    double xz;
  };
} d3plot_tensor;

typedef struct {
  double x;
  double y;
} d3plot_x_y;

typedef struct {
  double x;
  double y;
  double xy;
} d3plot_x_y_xy;

typedef struct {
  union {
    d3plot_tensor sigma;
    d3plot_tensor stress;
  };
  union {
    double effective_plastic_strain;
    double material_dependent_value;
  };
  union {
    d3plot_tensor epsilon;
    d3plot_tensor strain;
  };
} d3plot_solid;

/* Only used for d3plot_thick_shell and d3plot_shell*/
typedef struct {
  union {
    d3plot_tensor sigma;
    d3plot_tensor stress;
  };
  union {
    double effective_plastic_strain;
    double material_dependent_value;
  };

  /* All history variables of all elements are allocated in one big array and
   * this is a pointer somewhere into said array*/
  double *history_variables;
} d3plot_surface;

typedef struct {
  d3plot_surface mid;
  d3plot_surface inner;
  d3plot_surface outer;
  union {
    d3plot_tensor inner_epsilon;
    d3plot_tensor inner_strain;
  };
  union {
    d3plot_tensor outer_epsilon;
    d3plot_tensor outer_strain;
  };
} d3plot_thick_shell;

typedef struct {
  double axial_force;
  double s_shear_resultant;
  double t_shear_resultant;
  double s_bending_moment;
  double t_bending_moment;
  double torsional_resultant;
} d3plot_beam;

typedef struct {
  d3plot_surface mid;
  d3plot_surface inner;
  d3plot_surface outer;
  union {
    d3plot_tensor inner_epsilon;
    d3plot_tensor inner_strain;
  };
  union {
    d3plot_tensor outer_epsilon;
    d3plot_tensor outer_strain;
  };

  d3plot_x_y_xy bending_moment;
  d3plot_x_y shear_resultant;
  d3plot_x_y_xy normal_resultant;
  double thickness;
#ifdef __cplusplus
  std::array<double, 2> element_dependent_variables;
#else
  double element_dependent_variables[2];
#endif
  double internal_energy;
} d3plot_shell;

#define D3_FILE_TYPE_D3PLOT 1
#define D3_FILE_TYPE_D3DRLF 2
#define D3_FILE_TYPE_D3THDT 3
#define D3_FILE_TYPE_INTFOR 4
#define D3_FILE_TYPE_D3PART 5
#define D3_FILE_TYPE_BLSTFOR 6
#define D3_FILE_TYPE_D3CPM 7
#define D3_FILE_TYPE_D3ALE 8
#define D3_FILE_TYPE_D3EIGV 11
#define D3_FILE_TYPE_D3MODE 12
#define D3_FILE_TYPE_D3ITER 13
#define D3_FILE_TYPE_D3SSD 21
#define D3_FILE_TYPE_D3SPCM 22
#define D3_FILE_TYPE_D3PSD 23
#define D3_FILE_TYPE_D3RMS 24
#define D3_FILE_TYPE_D3FTG 25
#define D3_FILE_TYPE_D3ACS 26

#define D3_CODE_OLD_DYNA3D 2
#define D3_CODE_NIKE3D_LS_DYNA3D_LS_NIKE3D 6

#define D3_EOF -999999.0

#define D3PLT_PTR_TITLE 0
#define D3PLT_PTR_RUN_TIME (D3PLT_PTR_TITLE + 1)
#define D3PLT_PTR_NODE_COORDS (D3PLT_PTR_RUN_TIME + 1)
#define D3PLT_PTR_NODE_IDS (D3PLT_PTR_NODE_COORDS + 1)
#define D3PLT_PTR_EL8_IDS (D3PLT_PTR_NODE_IDS + 1)
#define D3PLT_PTR_EL2_IDS (D3PLT_PTR_EL8_IDS + 1)
#define D3PLT_PTR_EL4_IDS (D3PLT_PTR_EL2_IDS + 1)
#define D3PLT_PTR_ELT_IDS (D3PLT_PTR_EL4_IDS + 1)
#define D3PLT_PTR_PART_IDS (D3PLT_PTR_ELT_IDS + 1)
#define D3PLT_PTR_EL8_CONNECT (D3PLT_PTR_PART_IDS + 1)
#define D3PLT_PTR_ELT_CONNECT (D3PLT_PTR_EL8_CONNECT + 1)
#define D3PLT_PTR_EL2_CONNECT (D3PLT_PTR_ELT_CONNECT + 1)
#define D3PLT_PTR_EL4_CONNECT (D3PLT_PTR_EL2_CONNECT + 1)
#define D3PLT_PTR_PART_TITLES (D3PLT_PTR_EL4_CONNECT + 1)
#define D3PLT_PTR_STATE_TIME (D3PLT_PTR_PART_TITLES + 1)
#define D3PLT_PTR_STATE_NODE_COORDS (D3PLT_PTR_STATE_TIME + 1)
#define D3PLT_PTR_STATE_NODE_VEL (D3PLT_PTR_STATE_NODE_COORDS + 1)
#define D3PLT_PTR_STATE_NODE_ACC (D3PLT_PTR_STATE_NODE_VEL + 1)
#define D3PLT_PTR_STATE_ELEMENT_SOLID (D3PLT_PTR_STATE_NODE_ACC + 1)
#define D3PLT_PTR_STATE_ELEMENT_THICK_SHELL (D3PLT_PTR_STATE_ELEMENT_SOLID + 1)
#define D3PLT_PTR_STATE_ELEMENT_BEAM (D3PLT_PTR_STATE_ELEMENT_THICK_SHELL + 1)
#define D3PLT_PTR_STATE_ELEMENT_SHELL (D3PLT_PTR_STATE_ELEMENT_BEAM + 1)
#define D3PLT_PTR_STATES (D3PLT_PTR_STATE_ELEMENT_SHELL + 1)
#define D3PLT_PTR_COUNT D3PLT_PTR_STATES

#endif