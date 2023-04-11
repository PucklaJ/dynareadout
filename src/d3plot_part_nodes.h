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

#ifndef D3PLOT_PART_NODES_H
#define D3PLOT_PART_NODES_H

#include "d3_defines.h"
#include <stddef.h>

#ifndef D3PLOT_H
struct d3plot_file;
#endif

#define d3plot_part_get_node_ids(plot_file, part, num_part_node_ids)           \
  d3plot_part_get_node_ids2(plot_file, part, num_part_node_ids, NULL, 0, NULL, \
                            0, NULL, 0, NULL, 0, NULL, 0, NULL, NULL, NULL,    \
                            NULL)
#define d3plot_part_get_node_indices(plot_file, part, num_part_node_indices)   \
  d3plot_part_get_node_indices2(plot_file, part, num_part_node_indices, NULL,  \
                                0, NULL, 0, NULL, 0, NULL, 0, NULL, NULL,      \
                                NULL, NULL)

#define d3plot_part_get_num_nodes(plot_file, part)                             \
  d3plot_part_get_num_nodes2(plot_file, part, NULL, 0, NULL, 0, NULL, 0, NULL, \
                             0, NULL, NULL, NULL, NULL)

#ifdef __cplusplus
extern "C" {
#endif

/* Returns an array containing all node ids that are inside of the part.
 * The return value needs to be deallocated by free. This functions takes a
 * d3plot_part_get_node_ids_params struct. You can set the values of the struct
 * to optimize the functions performance. If you set params to NULL all data
 * will be retrieved, allocated and deallocated inside this one function call*/
d3_word *d3plot_part_get_node_ids2(
    d3plot_file *plot_file, const d3plot_part *part, size_t *num_part_node_ids,
    const d3_word *node_ids, size_t num_nodes, const d3_word *solid_ids,
    size_t num_solids, const d3_word *beam_ids, size_t num_beams,
    const d3_word *shell_ids, size_t num_shells, const d3_word *thick_shell_ids,
    size_t num_thick_shells, const d3plot_solid_con *solid_cons,
    const d3plot_beam_con *beam_cons, const d3plot_shell_con *shell_cons,
    const d3plot_thick_shell_con *thick_shell_cons);
/* The same as d3plot_part_get_node_ids, but it returns indices instead of ids.
 * Those indices can be used to index into the node_ids array returned by
 * d3plot_read_node_ids. If you set params to NULL all data
 * will be retrieved, allocated and deallocated inside this one function call*/
d3_word *d3plot_part_get_node_indices2(
    d3plot_file *plot_file, const d3plot_part *part,
    size_t *num_part_node_indices, const d3_word *solid_ids, size_t num_solids,
    const d3_word *beam_ids, size_t num_beams, const d3_word *shell_ids,
    size_t num_shells, const d3_word *thick_shell_ids, size_t num_thick_shells,
    const d3plot_solid_con *solid_cons, const d3plot_beam_con *beam_cons,
    const d3plot_shell_con *shell_cons,
    const d3plot_thick_shell_con *thick_shell_cons);

/* Returns the number of nodes of a part. Internally it just calls
 * d3plot_part_get_node_indices2 and returns the number of node indices.*/
size_t d3plot_part_get_num_nodes2(
    d3plot_file *plot_file, const d3plot_part *part, const d3_word *solid_ids,
    size_t num_solids, const d3_word *beam_ids, size_t num_beams,
    const d3_word *shell_ids, size_t num_shells, const d3_word *thick_shell_ids,
    size_t num_thick_shells, const d3plot_solid_con *solid_cons,
    const d3plot_beam_con *beam_cons, const d3plot_shell_con *shell_cons,
    const d3plot_thick_shell_con *thick_shell_cons);

/* Returns the number of all elements of a part*/
size_t d3plot_part_get_num_elements(const d3plot_part *part);

/* Returns an array containing all element ids and writes the size into num_ids.
 * Needs to be deallocated by free.*/
d3_word *d3plot_part_get_all_element_ids(const d3plot_part *part,
                                         size_t *num_ids);

#ifdef __cplusplus
}
#endif

#endif