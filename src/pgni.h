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

int _PGNI_LOAD(void ***member, size_t **num,
               void *(func)(d3plot_file *, size_t *), void **pointer_buffer,
               size_t *current_pointer, size_t *size_buffer,
               size_t *current_size, d3plot_file *plot_file) {
  if (!*member) {
    *member = &pointer_buffer[(*current_pointer)++];
    **member = NULL;
  }
  if (!**member) {
    if (!*num) {
      *num = &size_buffer[(*current_size)++];
    }
    **member = func(plot_file, *num);
    if (plot_file->error_string) {
      return 0;
    }
  }

  return 1;
}

#define PGNI_LOAD(member, num, func)                                           \
  _PGNI_LOAD((void ***)&p->member, &p->num, func, pointer_buffer,              \
             current_pointer, size_buffer, current_size, plot_file)

void pgni_add_element_solids(d3plot_file *plot_file, const d3plot_part *part,
                             d3_word *part_node_ids, size_t *num_part_node_ids,
                             d3plot_part_get_node_ids_params *p,
                             d3plot_part_get_node_ids_params *params,
                             void **pointer_buffer, size_t *current_pointer,
                             size_t *size_buffer, size_t *current_size) {
  if (part->num_solids != 0) {
    if (!PGNI_LOAD(solid_ids, num_solids, d3plot_read_solid_element_ids)) {
      /* Ignore these elements*/
      CLEAR_ERROR_STRING();
    } else {
      if (!PGNI_LOAD(solid_cons, num_solids, d3plot_read_solid_elements)) {
        /* Ignore these elements*/
        /*unload solid_ids*/
        if (!params || !params->solid_ids)
          free(*p->solid_ids);
        CLEAR_ERROR_STRING();
      } else {
        size_t i = 0;
        while (i < part->num_solids) {
          const size_t el_index = d3plot_index_for_id(
              part->solid_ids[i], *p->solid_ids, *p->num_solids);
          const d3plot_solid_con *el_con = &(*p->solid_cons)[el_index];
          size_t j = 0;
          while (j < (sizeof(el_con->node_indices) /
                      sizeof(*el_con->node_indices))) {
            const d3_word node_index = el_con->node_indices[j];
            const d3_word node_id = (*p->node_ids)[node_index];
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
        /*unload solid_cons*/
        if (!params || !params->solid_cons)
          free(*p->solid_cons);
        /*unload solid_ids*/
        if (!params || !params->solid_ids)
          free(*p->solid_ids);
      }
    }
  }
}

void pgni_add_element_beams(d3plot_file *plot_file, const d3plot_part *part,
                            d3_word *part_node_ids, size_t *num_part_node_ids,
                            d3plot_part_get_node_ids_params *p,
                            d3plot_part_get_node_ids_params *params,
                            void **pointer_buffer, size_t *current_pointer,
                            size_t *size_buffer, size_t *current_size) {
  if (part->num_beams != 0) {
    if (!PGNI_LOAD(beam_ids, num_beams, d3plot_read_beam_element_ids)) {
      /* Ignore these elements*/
      CLEAR_ERROR_STRING();
    } else {
      if (!PGNI_LOAD(beam_cons, num_beams, d3plot_read_beam_elements)) {
        /* Ignore these elements*/
        /*unload beam_ids*/
        if (!params || !params->beam_ids)
          free(*p->beam_ids);
        CLEAR_ERROR_STRING();
      } else {
        size_t i = 0;
        while (i < part->num_beams) {
          const size_t el_index = d3plot_index_for_id(
              part->beam_ids[i], *p->beam_ids, *p->num_beams);
          const d3plot_beam_con *el_con = &(*p->beam_cons)[el_index];
          size_t j = 0;
          while (j < (sizeof(el_con->node_indices) /
                      sizeof(*el_con->node_indices))) {
            const d3_word node_index = el_con->node_indices[j];
            const d3_word node_id = (*p->node_ids)[node_index];
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
        /*unload beam_cons*/
        if (!params || !params->beam_cons)
          free(*p->beam_cons);
        /*unload beam_ids*/
        if (!params || !params->beam_ids)
          free(*p->beam_ids);
      }
    }
  }
}

void pgni_add_element_shells(d3plot_file *plot_file, const d3plot_part *part,
                             d3_word *part_node_ids, size_t *num_part_node_ids,
                             d3plot_part_get_node_ids_params *p,
                             d3plot_part_get_node_ids_params *params,
                             void **pointer_buffer, size_t *current_pointer,
                             size_t *size_buffer, size_t *current_size) {
  if (part->num_shells != 0) {
    if (!PGNI_LOAD(shell_ids, num_shells, d3plot_read_shell_element_ids)) {
      /* Ignore these elements*/
      CLEAR_ERROR_STRING();
    } else {
      if (!PGNI_LOAD(shell_cons, num_shells, d3plot_read_shell_elements)) {
        /* Ignore these elements*/
        /*unload shell_ids*/
        if (!params || !params->shell_ids)
          free(*p->shell_ids);
        CLEAR_ERROR_STRING();
      } else {
        size_t i = 0;
        while (i < part->num_shells) {
          const size_t el_index = d3plot_index_for_id(
              part->shell_ids[i], *p->shell_ids, *p->num_shells);
          const d3plot_shell_con *el_con = &(*p->shell_cons)[el_index];
          size_t j = 0;
          while (j < (sizeof(el_con->node_indices) /
                      sizeof(*el_con->node_indices))) {
            const d3_word node_index = el_con->node_indices[j];
            const d3_word node_id = (*p->node_ids)[node_index];
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
        /*unload shell_cons*/
        if (!params || !params->shell_cons)
          free(*p->shell_cons);
        /*unload shell_ids*/
        if (!params || !params->shell_ids)
          free(*p->shell_ids);
      }
    }
  }
}

void pgni_add_element_thick_shells(
    d3plot_file *plot_file, const d3plot_part *part, d3_word *part_node_ids,
    size_t *num_part_node_ids, d3plot_part_get_node_ids_params *p,
    d3plot_part_get_node_ids_params *params, void **pointer_buffer,
    size_t *current_pointer, size_t *size_buffer, size_t *current_size) {
  if (part->num_thick_shells != 0) {
    if (!PGNI_LOAD(thick_shell_ids, num_thick_shells,
                   d3plot_read_thick_shell_element_ids)) {
      /* Ignore these elements*/
      CLEAR_ERROR_STRING();
    } else {
      if (!PGNI_LOAD(thick_shell_cons, num_thick_shells,
                     d3plot_read_thick_shell_elements)) {
        /* Ignore these elements*/
        /*unload thick_shell_ids*/
        if (!params || !params->thick_shell_ids)
          free(*p->thick_shell_ids);
        CLEAR_ERROR_STRING();
      } else {
        size_t i = 0;
        while (i < part->num_thick_shells) {
          const size_t el_index =
              d3plot_index_for_id(part->thick_shell_ids[i], *p->thick_shell_ids,
                                  *p->num_thick_shells);
          const d3plot_thick_shell_con *el_con =
              &(*p->thick_shell_cons)[el_index];
          size_t j = 0;
          while (j < (sizeof(el_con->node_indices) /
                      sizeof(*el_con->node_indices))) {
            const d3_word node_index = el_con->node_indices[j];
            const d3_word node_id = (*p->node_ids)[node_index];
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
        /*unload thick_shell_cons*/
        if (!params || !params->thick_shell_cons)
          free(*p->thick_shell_cons);
        /*unload thick_shell_ids*/
        if (!params || !params->thick_shell_ids)
          free(*p->thick_shell_ids);
      }
    }
  }
}

void pgnind_add_element_solids(d3plot_file *plot_file, const d3plot_part *part,
                               d3_word *part_node_indices,
                               size_t *num_part_node_indices,
                               d3plot_part_get_node_ids_params *p,
                               d3plot_part_get_node_ids_params *params,
                               void **pointer_buffer, size_t *current_pointer,
                               size_t *size_buffer, size_t *current_size) {
  if (part->num_solids != 0) {
    if (!PGNI_LOAD(solid_ids, num_solids, d3plot_read_solid_element_ids)) {
      /* Ignore these elements*/
      CLEAR_ERROR_STRING();
    } else {
      if (!PGNI_LOAD(solid_cons, num_solids, d3plot_read_solid_elements)) {
        /* Ignore these elements*/
        /*unload solid_ids*/
        if (!params || !params->solid_ids)
          free(*p->solid_ids);
        CLEAR_ERROR_STRING();
      } else {
        size_t i = 0;
        while (i < part->num_solids) {
          const size_t el_index = d3plot_index_for_id(
              part->solid_ids[i], *p->solid_ids, *p->num_solids);
          const d3plot_solid_con *el_con = &(*p->solid_cons)[el_index];
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
            /*********************************************/
            j++;
          }
          i++;
        }
        /*unload solid_cons*/
        if (!params || !params->solid_cons)
          free(*p->solid_cons);
        /*unload solid_ids*/
        if (!params || !params->solid_ids)
          free(*p->solid_ids);
      }
    }
  }
}

void pgnind_add_element_beams(d3plot_file *plot_file, const d3plot_part *part,
                              d3_word *part_node_indices,
                              size_t *num_part_node_indices,
                              d3plot_part_get_node_ids_params *p,
                              d3plot_part_get_node_ids_params *params,
                              void **pointer_buffer, size_t *current_pointer,
                              size_t *size_buffer, size_t *current_size) {
  if (part->num_beams != 0) {
    if (!PGNI_LOAD(beam_ids, num_beams, d3plot_read_beam_element_ids)) {
      /* Ignore these elements*/
      CLEAR_ERROR_STRING();
    } else {
      if (!PGNI_LOAD(beam_cons, num_beams, d3plot_read_beam_elements)) {
        /* Ignore these elements*/
        /*unload beam_ids*/
        if (!params || !params->beam_ids)
          free(*p->beam_ids);
        CLEAR_ERROR_STRING();
      } else {
        size_t i = 0;
        while (i < part->num_beams) {
          const size_t el_index = d3plot_index_for_id(
              part->beam_ids[i], *p->beam_ids, *p->num_beams);
          const d3plot_beam_con *el_con = &(*p->beam_cons)[el_index];
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
            /*********************************************/
            j++;
          }
          i++;
        }
        /*unload beam_cons*/
        if (!params || !params->beam_cons)
          free(*p->beam_cons);
        /*unload beam_ids*/
        if (!params || !params->beam_ids)
          free(*p->beam_ids);
      }
    }
  }
}

void pgnind_add_element_shells(d3plot_file *plot_file, const d3plot_part *part,
                               d3_word *part_node_indices,
                               size_t *num_part_node_indices,
                               d3plot_part_get_node_ids_params *p,
                               d3plot_part_get_node_ids_params *params,
                               void **pointer_buffer, size_t *current_pointer,
                               size_t *size_buffer, size_t *current_size) {
  if (part->num_shells != 0) {
    if (!PGNI_LOAD(shell_ids, num_shells, d3plot_read_shell_element_ids)) {
      /* Ignore these elements*/
      CLEAR_ERROR_STRING();
    } else {
      if (!PGNI_LOAD(shell_cons, num_shells, d3plot_read_shell_elements)) {
        /* Ignore these elements*/
        /*unload shell_ids*/
        if (!params || !params->shell_ids)
          free(*p->shell_ids);
        CLEAR_ERROR_STRING();
      } else {
        size_t i = 0;
        while (i < part->num_shells) {
          const size_t el_index = d3plot_index_for_id(
              part->shell_ids[i], *p->shell_ids, *p->num_shells);
          const d3plot_shell_con *el_con = &(*p->shell_cons)[el_index];
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
            /*********************************************/
            j++;
          }
          i++;
        }
        /*unload shell_cons*/
        if (!params || !params->shell_cons)
          free(*p->shell_cons);
        /*unload shell_ids*/
        if (!params || !params->shell_ids)
          free(*p->shell_ids);
      }
    }
  }
}

void pgnind_add_element_thick_shells(
    d3plot_file *plot_file, const d3plot_part *part, d3_word *part_node_indices,
    size_t *num_part_node_indices, d3plot_part_get_node_ids_params *p,
    d3plot_part_get_node_ids_params *params, void **pointer_buffer,
    size_t *current_pointer, size_t *size_buffer, size_t *current_size) {
  if (part->num_thick_shells != 0) {
    if (!PGNI_LOAD(thick_shell_ids, num_thick_shells,
                   d3plot_read_thick_shell_element_ids)) {
      /* Ignore these elements*/
      CLEAR_ERROR_STRING();
    } else {
      if (!PGNI_LOAD(thick_shell_cons, num_thick_shells,
                     d3plot_read_thick_shell_elements)) {
        /* Ignore these elements*/
        /*unload thick_shell_ids*/
        if (!params || !params->thick_shell_ids)
          free(*p->thick_shell_ids);
        CLEAR_ERROR_STRING();
      } else {
        size_t i = 0;
        while (i < part->num_thick_shells) {
          const size_t el_index =
              d3plot_index_for_id(part->thick_shell_ids[i], *p->thick_shell_ids,
                                  *p->num_thick_shells);
          const d3plot_thick_shell_con *el_con =
              &(*p->thick_shell_cons)[el_index];
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
            /*********************************************/
            j++;
          }
          i++;
        }
        /*unload thick_shell_cons*/
        if (!params || !params->thick_shell_cons)
          free(*p->thick_shell_cons);
        /*unload thick_shell_ids*/
        if (!params || !params->thick_shell_ids)
          free(*p->thick_shell_ids);
      }
    }
  }
}

#define PGNI_ADD_SOLIDS()                                                      \
  pgni_add_element_solids(plot_file, part, part_node_ids, num_part_node_ids,   \
                          p, params, pointer_buffer, &current_pointer,         \
                          size_buffer, &current_size);
#define PGNI_ADD_BEAMS()                                                       \
  pgni_add_element_beams(plot_file, part, part_node_ids, num_part_node_ids, p, \
                         params, pointer_buffer, &current_pointer,             \
                         size_buffer, &current_size);
#define PGNI_ADD_SHELLS()                                                      \
  pgni_add_element_shells(plot_file, part, part_node_ids, num_part_node_ids,   \
                          p, params, pointer_buffer, &current_pointer,         \
                          size_buffer, &current_size);
#define PGNI_ADD_THICK_SHELLS()                                                \
  pgni_add_element_thick_shells(plot_file, part, part_node_ids,                \
                                num_part_node_ids, p, params, pointer_buffer,  \
                                &current_pointer, size_buffer, &current_size);

#define PGNIND_ADD_SOLIDS()                                                    \
  pgnind_add_element_solids(plot_file, part, part_node_indices,                \
                            num_part_node_indices, p, params, pointer_buffer,  \
                            &current_pointer, size_buffer, &current_size);
#define PGNIND_ADD_BEAMS()                                                     \
  pgnind_add_element_beams(plot_file, part, part_node_indices,                 \
                           num_part_node_indices, p, params, pointer_buffer,   \
                           &current_pointer, size_buffer, &current_size);
#define PGNIND_ADD_SHELLS()                                                    \
  pgnind_add_element_shells(plot_file, part, part_node_indices,                \
                            num_part_node_indices, p, params, pointer_buffer,  \
                            &current_pointer, size_buffer, &current_size);
#define PGNIND_ADD_THICK_SHELLS()                                              \
  pgnind_add_element_thick_shells(                                             \
      plot_file, part, part_node_indices, num_part_node_indices, p, params,    \
      pointer_buffer, &current_pointer, size_buffer, &current_size);

#endif