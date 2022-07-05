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

#ifndef D3PLOT_H
#define D3PLOT_H
#include "d3_buffer.h"
#include "d3_defines.h"

typedef struct {
  struct {
    char *title;
    d3_word run_time, source_version, release_version, version, ndim, numnp,
        icode, nglbv, it, iu, iv, ia, nummat8, numds, numst, nv3d, nel2,
        nummat2, nv1d, nel4, nummat4, nv2d, neiph, neips, maxint, edlopt, nmsph,
        ngpsph, narbs, nelt, nummatt, nv3dt, ioshl[4], ialemat, ncfdv1, ncfdv2,
        nadapt, nmmat, numfluid, inn, npefg, nel48, idtdt, extra, words[6],
        nel20, nt3d;
    int64_t nel8;
    int mattyp, istrn, plastic_strain_tensor_written,
        thermal_strain_tensor_written, element_connectivity_packed;
  } control_data;

  d3_buffer buffer;
  char *error_string;
} d3plot_file;

#ifdef __cplusplus
extern "C" {
#endif

d3plot_file d3plot_open(const char *root_file_name);
void d3plot_close(d3plot_file *plot_file);

/***** Data sections *******/
int _d3plot_read_geometry_data(d3plot_file *plot_file);
/***************************/

const char *_d3plot_get_file_type_name(d3_word file_type);
int _get_nth_digit(d3_word value, int n);

#ifdef __cplusplus
}
#endif

#endif