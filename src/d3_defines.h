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

#ifndef D3_DEFINES_H
#define D3_DEFINES_H
#include <stdint.h>
#ifdef __cplusplus
#include <array>
#endif

typedef uint64_t d3_word;

typedef struct {
#ifdef __cplusplus
  std::array<d3_word, 8> node_ids;
#else
  d3_word node_ids[8];
#endif
  d3_word material_id;
} d3plot_solid;

typedef d3plot_solid d3plot_thick_shell;

typedef struct {
#ifdef __cplusplus
  std::array<d3_word, 2> node_ids;
#else
  d3_word node_ids[2];
#endif
  d3_word orientation_node_id;
  d3_word _null[2];
  d3_word material_id;
} d3plot_beam;

typedef struct {
#ifdef __cplusplus
  std::array<d3_word, 4> node_ids;
#else
  d3_word node_ids[4];
#endif
  d3_word material_id;
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
#define D3PLT_PTR_EL48_IDS (D3PLT_PTR_EL4_IDS + 1)
#define D3PLT_PTR_EL8_CONNECT (D3PLT_PTR_EL48_IDS + 1)
#define D3PLT_PTR_ELT_CONNECT (D3PLT_PTR_EL8_CONNECT + 1)
#define D3PLT_PTR_EL2_CONNECT (D3PLT_PTR_ELT_CONNECT + 1)
#define D3PLT_PTR_EL4_CONNECT (D3PLT_PTR_EL2_CONNECT + 1)
#define D3PLT_PTR_STATE_TIME (D3PLT_PTR_EL4_CONNECT + 1)
#define D3PLT_PTR_STATE_NODE_COORDS (D3PLT_PTR_STATE_TIME + 1)
#define D3PLT_PTR_STATE_NODE_VEL (D3PLT_PTR_STATE_NODE_COORDS + 1)
#define D3PLT_PTR_STATE_NODE_ACC (D3PLT_PTR_STATE_NODE_VEL + 1)
#define D3PLT_PTR_STATES (D3PLT_PTR_STATE_NODE_ACC + 1)
#define D3PLT_PTR_COUNT D3PLT_PTR_STATES

#endif