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

/* This file has been automatically generated*/

#ifndef PGNI_H
#define PGNI_H
#include "binary_search.h"
#include "d3plot.h"
#include "d3plot_error_macros.h"
#include "profiling.h"
#include <stdlib.h>
#include <string.h>

void pgni_add_element_solids(d3plot_file *plot_file, const d3plot_part *part,
                             d3_word *part_node_ids, size_t *num_part_node_ids,
                             const d3_word *node_ids, const d3_word *solid_ids,
                             size_t num_solids,
                             const d3plot_solid_con *solid_cons) {
  if (part->num_solids != 0) {
    uint8_t loaded_ids = 0;
    uint8_t loaded_cons = 0;
    if (!solid_ids) {
      loaded_ids = 1;
      solid_ids = d3plot_read_solid_element_ids(plot_file, &num_solids);
      if (plot_file->error_string) {
        /*Ignore these elements*/
        D3PLOT_CLEAR_ERROR_STRING();
      }
    }
    if (solid_ids) {
      if (!solid_cons) {
        solid_cons = d3plot_read_solid_elements(plot_file, &num_solids);
        if (plot_file->error_string) {
          /* Ignore these elements */
          /* Unload solid_ids*/
          if (loaded_ids) {
            free((d3_word *)solid_ids);
          }
          D3PLOT_CLEAR_ERROR_STRING();
        }
      }
      if (solid_cons) {
        size_t i = 0;
        while (i < part->num_solids) {
          const size_t el_index =
              d3plot_index_for_id(part->solid_ids[i], solid_ids, num_solids);
          const d3plot_solid_con *el_con = &solid_cons[el_index];
          size_t j = 0;
          while (j < (sizeof(el_con->node_indices) /
                      sizeof(*el_con->node_indices))) {
            const d3_word node_index = el_con->node_indices[j];
            const d3_word node_id = node_ids[node_index];
            /* Push node id onto the array of node ids*/
            if (*num_part_node_ids == 0) {
              *num_part_node_ids = 1;
              part_node_ids[0] = node_id;
            } else {
              int found;
              const size_t insert_index = d3_word_binary_search_insert(
                  part_node_ids, 0, *num_part_node_ids - 1, node_id, &found);
              if (!found) {
                (*num_part_node_ids)++;
                /* Move everything to the right*/
                size_t i = *num_part_node_ids - 1;
                while (i > insert_index) {
                  part_node_ids[i] = part_node_ids[i - 1];
                  i--;
                }
                part_node_ids[insert_index] = node_id;
              }
            }
            /******************************************/
            j++;
          }
          i++;
        }
        if (loaded_ids) {
          free((d3_word *)solid_ids);
        }
        if (loaded_cons) {
          free((d3plot_solid_con *)solid_cons);
        }
      }
    }
  }
}

void pgni_add_element_beams(d3plot_file *plot_file, const d3plot_part *part,
                            d3_word *part_node_ids, size_t *num_part_node_ids,
                            const d3_word *node_ids, const d3_word *beam_ids,
                            size_t num_beams,
                            const d3plot_beam_con *beam_cons) {
  if (part->num_beams != 0) {
    uint8_t loaded_ids = 0;
    uint8_t loaded_cons = 0;
    if (!beam_ids) {
      loaded_ids = 1;
      beam_ids = d3plot_read_beam_element_ids(plot_file, &num_beams);
      if (plot_file->error_string) {
        /*Ignore these elements*/
        D3PLOT_CLEAR_ERROR_STRING();
      }
    }
    if (beam_ids) {
      if (!beam_cons) {
        beam_cons = d3plot_read_beam_elements(plot_file, &num_beams);
        if (plot_file->error_string) {
          /* Ignore these elements */
          /* Unload solid_ids*/
          if (loaded_ids) {
            free((d3_word *)beam_ids);
          }
          D3PLOT_CLEAR_ERROR_STRING();
        }
      }
      if (beam_cons) {
        size_t i = 0;
        while (i < part->num_beams) {
          const size_t el_index =
              d3plot_index_for_id(part->beam_ids[i], beam_ids, num_beams);
          const d3plot_beam_con *el_con = &beam_cons[el_index];
          size_t j = 0;
          while (j < (sizeof(el_con->node_indices) /
                      sizeof(*el_con->node_indices))) {
            const d3_word node_index = el_con->node_indices[j];
            const d3_word node_id = node_ids[node_index];
            /* Push node id onto the array of node ids*/
            if (*num_part_node_ids == 0) {
              *num_part_node_ids = 1;
              part_node_ids[0] = node_id;
            } else {
              int found;
              const size_t insert_index = d3_word_binary_search_insert(
                  part_node_ids, 0, *num_part_node_ids - 1, node_id, &found);
              if (!found) {
                (*num_part_node_ids)++;
                /* Move everything to the right*/
                size_t i = *num_part_node_ids - 1;
                while (i > insert_index) {
                  part_node_ids[i] = part_node_ids[i - 1];
                  i--;
                }
                part_node_ids[insert_index] = node_id;
              }
            }
            /******************************************/
            j++;
          }
          i++;
        }
        if (loaded_ids) {
          free((d3_word *)beam_ids);
        }
        if (loaded_cons) {
          free((d3plot_beam_con *)beam_cons);
        }
      }
    }
  }
}

void pgni_add_element_shells(d3plot_file *plot_file, const d3plot_part *part,
                             d3_word *part_node_ids, size_t *num_part_node_ids,
                             const d3_word *node_ids, const d3_word *shell_ids,
                             size_t num_shells,
                             const d3plot_shell_con *shell_cons) {
  if (part->num_shells != 0) {
    uint8_t loaded_ids = 0;
    uint8_t loaded_cons = 0;
    if (!shell_ids) {
      loaded_ids = 1;
      shell_ids = d3plot_read_shell_element_ids(plot_file, &num_shells);
      if (plot_file->error_string) {
        /*Ignore these elements*/
        D3PLOT_CLEAR_ERROR_STRING();
      }
    }
    if (shell_ids) {
      if (!shell_cons) {
        shell_cons = d3plot_read_shell_elements(plot_file, &num_shells);
        if (plot_file->error_string) {
          /* Ignore these elements */
          /* Unload solid_ids*/
          if (loaded_ids) {
            free((d3_word *)shell_ids);
          }
          D3PLOT_CLEAR_ERROR_STRING();
        }
      }
      if (shell_cons) {
        size_t i = 0;
        while (i < part->num_shells) {
          const size_t el_index =
              d3plot_index_for_id(part->shell_ids[i], shell_ids, num_shells);
          const d3plot_shell_con *el_con = &shell_cons[el_index];
          size_t j = 0;
          while (j < (sizeof(el_con->node_indices) /
                      sizeof(*el_con->node_indices))) {
            const d3_word node_index = el_con->node_indices[j];
            const d3_word node_id = node_ids[node_index];
            /* Push node id onto the array of node ids*/
            if (*num_part_node_ids == 0) {
              *num_part_node_ids = 1;
              part_node_ids[0] = node_id;
            } else {
              int found;
              const size_t insert_index = d3_word_binary_search_insert(
                  part_node_ids, 0, *num_part_node_ids - 1, node_id, &found);
              if (!found) {
                (*num_part_node_ids)++;
                /* Move everything to the right*/
                size_t i = *num_part_node_ids - 1;
                while (i > insert_index) {
                  part_node_ids[i] = part_node_ids[i - 1];
                  i--;
                }
                part_node_ids[insert_index] = node_id;
              }
            }
            /******************************************/
            j++;
          }
          i++;
        }
        if (loaded_ids) {
          free((d3_word *)shell_ids);
        }
        if (loaded_cons) {
          free((d3plot_shell_con *)shell_cons);
        }
      }
    }
  }
}

void pgni_add_element_thick_shells(
    d3plot_file *plot_file, const d3plot_part *part, d3_word *part_node_ids,
    size_t *num_part_node_ids, const d3_word *node_ids,
    const d3_word *thick_shell_ids, size_t num_thick_shells,
    const d3plot_thick_shell_con *thick_shell_cons) {
  if (part->num_thick_shells != 0) {
    uint8_t loaded_ids = 0;
    uint8_t loaded_cons = 0;
    if (!thick_shell_ids) {
      loaded_ids = 1;
      thick_shell_ids =
          d3plot_read_thick_shell_element_ids(plot_file, &num_thick_shells);
      if (plot_file->error_string) {
        /*Ignore these elements*/
        D3PLOT_CLEAR_ERROR_STRING();
      }
    }
    if (thick_shell_ids) {
      if (!thick_shell_cons) {
        thick_shell_cons =
            d3plot_read_thick_shell_elements(plot_file, &num_thick_shells);
        if (plot_file->error_string) {
          /* Ignore these elements */
          /* Unload solid_ids*/
          if (loaded_ids) {
            free((d3_word *)thick_shell_ids);
          }
          D3PLOT_CLEAR_ERROR_STRING();
        }
      }
      if (thick_shell_cons) {
        size_t i = 0;
        while (i < part->num_thick_shells) {
          const size_t el_index = d3plot_index_for_id(
              part->thick_shell_ids[i], thick_shell_ids, num_thick_shells);
          const d3plot_thick_shell_con *el_con = &thick_shell_cons[el_index];
          size_t j = 0;
          while (j < (sizeof(el_con->node_indices) /
                      sizeof(*el_con->node_indices))) {
            const d3_word node_index = el_con->node_indices[j];
            const d3_word node_id = node_ids[node_index];
            /* Push node id onto the array of node ids*/
            if (*num_part_node_ids == 0) {
              *num_part_node_ids = 1;
              part_node_ids[0] = node_id;
            } else {
              int found;
              const size_t insert_index = d3_word_binary_search_insert(
                  part_node_ids, 0, *num_part_node_ids - 1, node_id, &found);
              if (!found) {
                (*num_part_node_ids)++;
                /* Move everything to the right*/
                size_t i = *num_part_node_ids - 1;
                while (i > insert_index) {
                  part_node_ids[i] = part_node_ids[i - 1];
                  i--;
                }
                part_node_ids[insert_index] = node_id;
              }
            }
            /******************************************/
            j++;
          }
          i++;
        }
        if (loaded_ids) {
          free((d3_word *)thick_shell_ids);
        }
        if (loaded_cons) {
          free((d3plot_thick_shell_con *)thick_shell_cons);
        }
      }
    }
  }
}

void pgnind_add_element_solids(d3plot_file *plot_file, const d3plot_part *part,
                               d3_word *part_node_indices,
                               size_t *num_part_node_indices,
                               const d3_word *solid_ids, size_t num_solids,
                               const d3plot_solid_con *solid_cons) {
  if (part->num_solids != 0) {
    uint8_t loaded_ids = 0;
    uint8_t loaded_cons = 0;
    if (!solid_ids) {
      loaded_ids = 1;
      solid_ids = d3plot_read_solid_element_ids(plot_file, &num_solids);
      if (plot_file->error_string) {
        /*Ignore these elements*/
        D3PLOT_CLEAR_ERROR_STRING();
      }
    }
    if (solid_ids) {
      if (!solid_cons) {
        solid_cons = d3plot_read_solid_elements(plot_file, &num_solids);
        if (plot_file->error_string) {
          /* Ignore these elements */
          /* Unload solid_ids*/
          if (loaded_ids) {
            free((d3_word *)solid_ids);
          }
          D3PLOT_CLEAR_ERROR_STRING();
        }
      }
      if (solid_cons) {
        size_t i = 0;
        while (i < part->num_solids) {
          const size_t el_index =
              d3plot_index_for_id(part->solid_ids[i], solid_ids, num_solids);
          const d3plot_solid_con *el_con = &solid_cons[el_index];
          size_t j = 0;
          while (j < (sizeof(el_con->node_indices) /
                      sizeof(*el_con->node_indices))) {
            const d3_word node_index = el_con->node_indices[j];
            /* Push node index onto the array of node indices*/
            if (*num_part_node_indices == 0) {
              *num_part_node_indices = 1;
              part_node_indices[0] = node_index;
            } else {
              int found;
              const size_t insert_index = d3_word_binary_search_insert(
                  part_node_indices, 0, *num_part_node_indices - 1, node_index,
                  &found);
              if (!found) {
                (*num_part_node_indices)++;
                /* Move everything to the right*/
                size_t i = *num_part_node_indices - 1;
                while (i > insert_index) {
                  part_node_indices[i] = part_node_indices[i - 1];
                  i--;
                }
                part_node_indices[insert_index] = node_index;
              }
            }
            /******************************************/
            j++;
          }
          i++;
        }
        if (loaded_ids) {
          free((d3_word *)solid_ids);
        }
        if (loaded_cons) {
          free((d3plot_solid_con *)solid_cons);
        }
      }
    }
  }
}

void pgnind_add_element_beams(d3plot_file *plot_file, const d3plot_part *part,
                              d3_word *part_node_indices,
                              size_t *num_part_node_indices,
                              const d3_word *beam_ids, size_t num_beams,
                              const d3plot_beam_con *beam_cons) {
  if (part->num_beams != 0) {
    uint8_t loaded_ids = 0;
    uint8_t loaded_cons = 0;
    if (!beam_ids) {
      loaded_ids = 1;
      beam_ids = d3plot_read_beam_element_ids(plot_file, &num_beams);
      if (plot_file->error_string) {
        /*Ignore these elements*/
        D3PLOT_CLEAR_ERROR_STRING();
      }
    }
    if (beam_ids) {
      if (!beam_cons) {
        beam_cons = d3plot_read_beam_elements(plot_file, &num_beams);
        if (plot_file->error_string) {
          /* Ignore these elements */
          /* Unload solid_ids*/
          if (loaded_ids) {
            free((d3_word *)beam_ids);
          }
          D3PLOT_CLEAR_ERROR_STRING();
        }
      }
      if (beam_cons) {
        size_t i = 0;
        while (i < part->num_beams) {
          const size_t el_index =
              d3plot_index_for_id(part->beam_ids[i], beam_ids, num_beams);
          const d3plot_beam_con *el_con = &beam_cons[el_index];
          size_t j = 0;
          while (j < (sizeof(el_con->node_indices) /
                      sizeof(*el_con->node_indices))) {
            const d3_word node_index = el_con->node_indices[j];
            /* Push node index onto the array of node indices*/
            if (*num_part_node_indices == 0) {
              *num_part_node_indices = 1;
              part_node_indices[0] = node_index;
            } else {
              int found;
              const size_t insert_index = d3_word_binary_search_insert(
                  part_node_indices, 0, *num_part_node_indices - 1, node_index,
                  &found);
              if (!found) {
                (*num_part_node_indices)++;
                /* Move everything to the right*/
                size_t i = *num_part_node_indices - 1;
                while (i > insert_index) {
                  part_node_indices[i] = part_node_indices[i - 1];
                  i--;
                }
                part_node_indices[insert_index] = node_index;
              }
            }
            /******************************************/
            j++;
          }
          i++;
        }
        if (loaded_ids) {
          free((d3_word *)beam_ids);
        }
        if (loaded_cons) {
          free((d3plot_beam_con *)beam_cons);
        }
      }
    }
  }
}

void pgnind_add_element_shells(d3plot_file *plot_file, const d3plot_part *part,
                               d3_word *part_node_indices,
                               size_t *num_part_node_indices,
                               const d3_word *shell_ids, size_t num_shells,
                               const d3plot_shell_con *shell_cons) {
  if (part->num_shells != 0) {
    uint8_t loaded_ids = 0;
    uint8_t loaded_cons = 0;
    if (!shell_ids) {
      loaded_ids = 1;
      shell_ids = d3plot_read_shell_element_ids(plot_file, &num_shells);
      if (plot_file->error_string) {
        /*Ignore these elements*/
        D3PLOT_CLEAR_ERROR_STRING();
      }
    }
    if (shell_ids) {
      if (!shell_cons) {
        shell_cons = d3plot_read_shell_elements(plot_file, &num_shells);
        if (plot_file->error_string) {
          /* Ignore these elements */
          /* Unload solid_ids*/
          if (loaded_ids) {
            free((d3_word *)shell_ids);
          }
          D3PLOT_CLEAR_ERROR_STRING();
        }
      }
      if (shell_cons) {
        size_t i = 0;
        while (i < part->num_shells) {
          const size_t el_index =
              d3plot_index_for_id(part->shell_ids[i], shell_ids, num_shells);
          const d3plot_shell_con *el_con = &shell_cons[el_index];
          size_t j = 0;
          while (j < (sizeof(el_con->node_indices) /
                      sizeof(*el_con->node_indices))) {
            const d3_word node_index = el_con->node_indices[j];
            /* Push node index onto the array of node indices*/
            if (*num_part_node_indices == 0) {
              *num_part_node_indices = 1;
              part_node_indices[0] = node_index;
            } else {
              int found;
              const size_t insert_index = d3_word_binary_search_insert(
                  part_node_indices, 0, *num_part_node_indices - 1, node_index,
                  &found);
              if (!found) {
                (*num_part_node_indices)++;
                /* Move everything to the right*/
                size_t i = *num_part_node_indices - 1;
                while (i > insert_index) {
                  part_node_indices[i] = part_node_indices[i - 1];
                  i--;
                }
                part_node_indices[insert_index] = node_index;
              }
            }
            /******************************************/
            j++;
          }
          i++;
        }
        if (loaded_ids) {
          free((d3_word *)shell_ids);
        }
        if (loaded_cons) {
          free((d3plot_shell_con *)shell_cons);
        }
      }
    }
  }
}

void pgnind_add_element_thick_shells(
    d3plot_file *plot_file, const d3plot_part *part, d3_word *part_node_indices,
    size_t *num_part_node_indices, const d3_word *thick_shell_ids,
    size_t num_thick_shells, const d3plot_thick_shell_con *thick_shell_cons) {
  if (part->num_thick_shells != 0) {
    uint8_t loaded_ids = 0;
    uint8_t loaded_cons = 0;
    if (!thick_shell_ids) {
      loaded_ids = 1;
      thick_shell_ids =
          d3plot_read_thick_shell_element_ids(plot_file, &num_thick_shells);
      if (plot_file->error_string) {
        /*Ignore these elements*/
        D3PLOT_CLEAR_ERROR_STRING();
      }
    }
    if (thick_shell_ids) {
      if (!thick_shell_cons) {
        thick_shell_cons =
            d3plot_read_thick_shell_elements(plot_file, &num_thick_shells);
        if (plot_file->error_string) {
          /* Ignore these elements */
          /* Unload solid_ids*/
          if (loaded_ids) {
            free((d3_word *)thick_shell_ids);
          }
          D3PLOT_CLEAR_ERROR_STRING();
        }
      }
      if (thick_shell_cons) {
        size_t i = 0;
        while (i < part->num_thick_shells) {
          const size_t el_index = d3plot_index_for_id(
              part->thick_shell_ids[i], thick_shell_ids, num_thick_shells);
          const d3plot_thick_shell_con *el_con = &thick_shell_cons[el_index];
          size_t j = 0;
          while (j < (sizeof(el_con->node_indices) /
                      sizeof(*el_con->node_indices))) {
            const d3_word node_index = el_con->node_indices[j];
            /* Push node index onto the array of node indices*/
            if (*num_part_node_indices == 0) {
              *num_part_node_indices = 1;
              part_node_indices[0] = node_index;
            } else {
              int found;
              const size_t insert_index = d3_word_binary_search_insert(
                  part_node_indices, 0, *num_part_node_indices - 1, node_index,
                  &found);
              if (!found) {
                (*num_part_node_indices)++;
                /* Move everything to the right*/
                size_t i = *num_part_node_indices - 1;
                while (i > insert_index) {
                  part_node_indices[i] = part_node_indices[i - 1];
                  i--;
                }
                part_node_indices[insert_index] = node_index;
              }
            }
            /******************************************/
            j++;
          }
          i++;
        }
        if (loaded_ids) {
          free((d3_word *)thick_shell_ids);
        }
        if (loaded_cons) {
          free((d3plot_thick_shell_con *)thick_shell_cons);
        }
      }
    }
  }
}

#define PGNI_ADD_SOLIDS()                                                      \
  pgni_add_element_solids(plot_file, part, part_node_ids, num_part_node_ids,   \
                          node_ids, solid_ids, num_solids, solid_cons);
#define PGNI_ADD_BEAMS()                                                       \
  pgni_add_element_beams(plot_file, part, part_node_ids, num_part_node_ids,    \
                         node_ids, beam_ids, num_beams, beam_cons);
#define PGNI_ADD_SHELLS()                                                      \
  pgni_add_element_shells(plot_file, part, part_node_ids, num_part_node_ids,   \
                          node_ids, shell_ids, num_shells, shell_cons);
#define PGNI_ADD_THICK_SHELLS()                                                \
  pgni_add_element_thick_shells(plot_file, part, part_node_ids,                \
                                num_part_node_ids, node_ids, thick_shell_ids,  \
                                num_thick_shells, thick_shell_cons);

#define PGNIND_ADD_SOLIDS()                                                    \
  pgnind_add_element_solids(plot_file, part, part_node_indices,                \
                            num_part_node_indices, solid_ids, num_solids,      \
                            solid_cons);
#define PGNIND_ADD_BEAMS()                                                     \
  pgnind_add_element_beams(plot_file, part, part_node_indices,                 \
                           num_part_node_indices, beam_ids, num_beams,         \
                           beam_cons);
#define PGNIND_ADD_SHELLS()                                                    \
  pgnind_add_element_shells(plot_file, part, part_node_indices,                \
                            num_part_node_indices, shell_ids, num_shells,      \
                            shell_cons);
#define PGNIND_ADD_THICK_SHELLS()                                              \
  pgnind_add_element_thick_shells(plot_file, part, part_node_indices,          \
                                  num_part_node_indices, thick_shell_ids,      \
                                  num_thick_shells, thick_shell_cons);

#endif