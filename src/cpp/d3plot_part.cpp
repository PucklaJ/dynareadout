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

#include "d3plot_part.hpp"
#include "d3plot.hpp"
#include <d3plot.h>

namespace dro {

D3plotPart::D3plotPart(const d3plot_part &part) : m_part(part) {}

D3plotPart::D3plotPart(D3plotPart &&rhs) : m_part(rhs.m_part) {
  rhs.m_part.solid_ids = NULL;
  rhs.m_part.thick_shell_ids = NULL;
  rhs.m_part.beam_ids = NULL;
  rhs.m_part.shell_ids = NULL;
}

D3plotPart::~D3plotPart() { d3plot_free_part(&m_part); }

D3plotPart &D3plotPart::operator=(D3plotPart &&rhs) {
  m_part = rhs.m_part;
  rhs.m_part.solid_ids = NULL;
  rhs.m_part.thick_shell_ids = NULL;
  rhs.m_part.beam_ids = NULL;
  rhs.m_part.shell_ids = NULL;

  return *this;
}

Array<d3_word> D3plotPart::get_solid_elements() {
  return Array<d3_word>(m_part.solid_ids, m_part.num_solids, false);
}

Array<d3_word> D3plotPart::get_thick_shell_elements() {
  return Array<d3_word>(m_part.thick_shell_ids, m_part.num_thick_shells, false);
}

Array<d3_word> D3plotPart::get_beam_elements() {
  return Array<d3_word>(m_part.beam_ids, m_part.num_beams, false);
}

Array<d3_word> D3plotPart::get_shell_elements() {
  return Array<d3_word>(m_part.shell_ids, m_part.num_shells, false);
}

Array<d3_word> D3plotPart::get_node_ids(
    D3plot &plot_file, const Array<d3_word> *solid_ids,
    const Array<d3_word> *beam_ids, const Array<d3_word> *shell_ids,
    const Array<d3_word> *thick_shell_ids, const Array<d3_word> *node_ids,
    const Array<d3plot_solid_con> *solid_cons,
    const Array<d3plot_beam_con> *beam_cons,
    const Array<d3plot_shell_con> *shell_cons,
    const Array<d3plot_thick_shell_con> *thick_shell_cons) const {
  size_t num_part_node_ids;
  auto *part_node_ids = d3plot_part_get_node_ids2(
      &plot_file.get_handle(), &m_part, &num_part_node_ids,
      node_ids ? node_ids->data() : NULL, node_ids ? node_ids->size() : 0,
      solid_ids ? solid_ids->data() : NULL, solid_ids ? solid_ids->size() : 0,
      beam_ids ? beam_ids->data() : NULL, beam_ids ? beam_ids->size() : 0,
      shell_ids ? shell_ids->data() : NULL, shell_ids ? shell_ids->size() : 0,
      thick_shell_ids ? thick_shell_ids->data() : NULL,
      thick_shell_ids ? thick_shell_ids->size() : 0,
      thick_shell_cons ? thick_shell_cons->data() : NULL,
      beam_cons ? beam_cons->data() : NULL,
      shell_cons ? shell_cons->data() : NULL,
      thick_shell_cons ? thick_shell_cons->data() : NULL);
  // TODO: Check for errors

  return Array<d3_word>(part_node_ids, num_part_node_ids);
}

Array<d3_word> D3plotPart::get_node_indices(
    D3plot &plot_file, const Array<d3_word> *solid_ids,
    const Array<d3_word> *beam_ids, const Array<d3_word> *shell_ids,
    const Array<d3_word> *thick_shell_ids,
    const Array<d3plot_solid_con> *solid_cons,
    const Array<d3plot_beam_con> *beam_cons,
    const Array<d3plot_shell_con> *shell_cons,
    const Array<d3plot_thick_shell_con> *thick_shell_cons) const {
  size_t num_part_node_indices;
  auto *part_node_indices = d3plot_part_get_node_indices2(
      &plot_file.get_handle(), &m_part, &num_part_node_indices,
      solid_ids ? solid_ids->data() : NULL, solid_ids ? solid_ids->size() : 0,
      beam_ids ? beam_ids->data() : NULL, beam_ids ? beam_ids->size() : 0,
      shell_ids ? shell_ids->data() : NULL, shell_ids ? shell_ids->size() : 0,
      thick_shell_ids ? thick_shell_ids->data() : NULL,
      thick_shell_ids ? thick_shell_ids->size() : 0,
      thick_shell_cons ? thick_shell_cons->data() : NULL,
      beam_cons ? beam_cons->data() : NULL,
      shell_cons ? shell_cons->data() : NULL,
      thick_shell_cons ? thick_shell_cons->data() : NULL);
  // TODO: Check for errors

  return Array<d3_word>(part_node_indices, num_part_node_indices);
}

} // namespace dro