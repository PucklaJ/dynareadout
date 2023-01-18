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
    D3plot &plot_file, Array<d3_word> *solid_ids, Array<d3_word> *beam_ids,
    Array<d3_word> *shell_ids, Array<d3_word> *thick_shell_ids,
    Array<d3_word> *node_ids, Array<d3plot_solid_con> *solid_cons,
    Array<d3plot_beam_con> *beam_cons, Array<d3plot_shell_con> *shell_cons,
    Array<d3plot_thick_shell_con> *thick_shell_cons) const {
  d3plot_part_get_node_ids_params p;
  memset(&p, 0, sizeof(p));
  if (solid_ids) {
    auto [p_solid_ids, p_num_solids] = solid_ids->access();
    p.solid_ids = p_solid_ids;
    p.num_solids = p_num_solids;
  }
  if (beam_ids) {
    auto [p_beam_ids, p_num_beams] = beam_ids->access();
    p.beam_ids = p_beam_ids;
    p.num_beams = p_num_beams;
  }
  if (shell_ids) {
    auto [p_shell_ids, p_num_shells] = shell_ids->access();
    p.shell_ids = p_shell_ids;
    p.num_shells = p_num_shells;
  }
  if (thick_shell_ids) {
    auto [p_thick_shell_ids, p_num_thick_shells] = thick_shell_ids->access();
    p.thick_shell_ids = p_thick_shell_ids;
    p.num_thick_shells = p_num_thick_shells;
  }
  if (node_ids) {
    auto [p_node_ids, p_num_node_ids] = node_ids->access();
    p.node_ids = p_node_ids;
    p.num_node_ids = p_num_node_ids;
  }
  if (solid_cons) {
    auto [p_solid_cons, p_num_solids] = solid_cons->access();
    p.solid_cons = p_solid_cons;
    p.num_solids = p_num_solids;
  }
  if (beam_cons) {
    auto [p_beam_cons, p_num_beams] = beam_cons->access();
    p.beam_cons = p_beam_cons;
    p.num_beams = p_num_beams;
  }
  if (shell_cons) {
    auto [p_shell_cons, p_num_shells] = shell_cons->access();
    p.shell_cons = p_shell_cons;
    p.num_shells = p_num_shells;
  }
  if (thick_shell_cons) {
    auto [p_thick_shell_cons, p_num_thick_shells] = thick_shell_cons->access();
    p.thick_shell_cons = p_thick_shell_cons;
    p.num_thick_shells = p_num_thick_shells;
  }

  size_t num_part_node_ids;
  auto *part_node_ids = d3plot_part_get_node_ids(
      &plot_file.get_handle(), &m_part, &num_part_node_ids, &p);
  // TODO: Check for errors

  if (solid_cons && solid_ids) {
    auto [_, num] = solid_ids->access();
    *num = solid_cons->size();
  }
  if (beam_cons && beam_ids) {
    auto [_, num] = beam_ids->access();
    *num = beam_cons->size();
  }
  if (shell_cons && shell_ids) {
    auto [_, num] = shell_ids->access();
    *num = shell_cons->size();
  }
  if (thick_shell_cons && thick_shell_ids) {
    auto [_, num] = thick_shell_ids->access();
    *num = thick_shell_cons->size();
  }

  return Array<d3_word>(part_node_ids, num_part_node_ids);
}

Array<d3_word> D3plotPart::get_node_indices(
    D3plot &plot_file, Array<d3_word> *solid_ids, Array<d3_word> *beam_ids,
    Array<d3_word> *shell_ids, Array<d3_word> *thick_shell_ids,
    Array<d3plot_solid_con> *solid_cons, Array<d3plot_beam_con> *beam_cons,
    Array<d3plot_shell_con> *shell_cons,
    Array<d3plot_thick_shell_con> *thick_shell_cons) const {
  d3plot_part_get_node_ids_params p;
  memset(&p, 0, sizeof(p));
  if (solid_ids) {
    auto [p_solid_ids, p_num_solids] = solid_ids->access();
    p.solid_ids = p_solid_ids;
    p.num_solids = p_num_solids;
  }
  if (beam_ids) {
    auto [p_beam_ids, p_num_beams] = beam_ids->access();
    p.beam_ids = p_beam_ids;
    p.num_beams = p_num_beams;
  }
  if (shell_ids) {
    auto [p_shell_ids, p_num_shells] = shell_ids->access();
    p.shell_ids = p_shell_ids;
    p.num_shells = p_num_shells;
  }
  if (thick_shell_ids) {
    auto [p_thick_shell_ids, p_num_thick_shells] = thick_shell_ids->access();
    p.thick_shell_ids = p_thick_shell_ids;
    p.num_thick_shells = p_num_thick_shells;
  }
  if (solid_cons) {
    auto [p_solid_cons, p_num_solids] = solid_cons->access();
    p.solid_cons = p_solid_cons;
    p.num_solids = p_num_solids;
  }
  if (beam_cons) {
    auto [p_beam_cons, p_num_beams] = beam_cons->access();
    p.beam_cons = p_beam_cons;
    p.num_beams = p_num_beams;
  }
  if (shell_cons) {
    auto [p_shell_cons, p_num_shells] = shell_cons->access();
    p.shell_cons = p_shell_cons;
    p.num_shells = p_num_shells;
  }
  if (thick_shell_cons) {
    auto [p_thick_shell_cons, p_num_thick_shells] = thick_shell_cons->access();
    p.thick_shell_cons = p_thick_shell_cons;
    p.num_thick_shells = p_num_thick_shells;
  }

  size_t num_part_node_indices;
  auto *part_node_indices = d3plot_part_get_node_indices(
      &plot_file.get_handle(), &m_part, &num_part_node_indices, &p);
  // TODO: Check for errors

  if (solid_cons && solid_ids) {
    auto [_, num] = solid_ids->access();
    *num = solid_cons->size();
  }
  if (beam_cons && beam_ids) {
    auto [_, num] = beam_ids->access();
    *num = beam_cons->size();
  }
  if (shell_cons && shell_ids) {
    auto [_, num] = shell_ids->access();
    *num = shell_cons->size();
  }
  if (thick_shell_cons && thick_shell_ids) {
    auto [_, num] = thick_shell_ids->access();
    *num = thick_shell_cons->size();
  }

  return Array<d3_word>(part_node_indices, num_part_node_indices);
}

} // namespace dro