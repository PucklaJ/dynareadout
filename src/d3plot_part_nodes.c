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

#include "pgni.h"

d3_word *d3plot_part_get_node_ids2(
    d3plot_file *plot_file, const d3plot_part *part, size_t *num_part_node_ids,
    const d3_word *node_ids, size_t num_nodes, const d3_word *solid_ids,
    size_t num_solids, const d3_word *beam_ids, size_t num_beams,
    const d3_word *shell_ids, size_t num_shells, const d3_word *thick_shell_ids,
    size_t num_thick_shells, const d3plot_solid_con *solid_cons,
    const d3plot_beam_con *beam_cons, const d3plot_shell_con *shell_cons,
    const d3plot_thick_shell_con *thick_shell_cons) {
  BEGIN_PROFILE_FUNC();
  CLEAR_ERROR_STRING();

  uint8_t node_ids_loaded = 0;

  /* Load node ids*/
  if (!node_ids) {
    node_ids_loaded = 1;
    node_ids = d3plot_read_node_ids(plot_file, &num_nodes);
    if (plot_file->error_string) {
      ERROR_AND_NO_RETURN_F_PTR("Failed to load node ids: %s",
                                plot_file->error_string);
      END_PROFILE_FUNC();
      return NULL;
    }
  }

  const size_t part_node_ids_cap = part->num_solids * 8 + part->num_beams * 2 +
                                   part->num_shells * 4 +
                                   part->num_thick_shells * 8;
  *num_part_node_ids = 0;
  d3_word *part_node_ids = malloc(part_node_ids_cap * sizeof(d3_word));

  PGNI_ADD_SOLIDS();
  PGNI_ADD_BEAMS();
  PGNI_ADD_SHELLS();
  PGNI_ADD_THICK_SHELLS();

  /*unload node_ids*/
  if (node_ids_loaded) {
    free((d3_word *)node_ids);
  }

  /* Shrink to fit*/
  if (part_node_ids_cap != *num_part_node_ids) {
    part_node_ids =
        realloc(part_node_ids, *num_part_node_ids * sizeof(d3_word));
  }

  END_PROFILE_FUNC();
  return part_node_ids;
}

d3_word *d3plot_part_get_node_indices2(
    d3plot_file *plot_file, const d3plot_part *part,
    size_t *num_part_node_indices, const d3_word *solid_ids, size_t num_solids,
    const d3_word *beam_ids, size_t num_beams, const d3_word *shell_ids,
    size_t num_shells, const d3_word *thick_shell_ids, size_t num_thick_shells,
    const d3plot_solid_con *solid_cons, const d3plot_beam_con *beam_cons,
    const d3plot_shell_con *shell_cons,
    const d3plot_thick_shell_con *thick_shell_cons) {
  BEGIN_PROFILE_FUNC();
  CLEAR_ERROR_STRING();

  const size_t part_node_indices_cap =
      part->num_solids * 8 + part->num_beams * 2 + part->num_shells * 4 +
      part->num_thick_shells * 8;
  *num_part_node_indices = 0;
  d3_word *part_node_indices = malloc(part_node_indices_cap * sizeof(d3_word));

  PGNIND_ADD_SOLIDS();
  PGNIND_ADD_BEAMS();
  PGNIND_ADD_SHELLS();
  PGNIND_ADD_THICK_SHELLS();

  /* Shrink to fit*/
  if (part_node_indices_cap != *num_part_node_indices) {
    part_node_indices =
        realloc(part_node_indices, *num_part_node_indices * sizeof(d3_word));
  }

  END_PROFILE_FUNC();
  return part_node_indices;
}