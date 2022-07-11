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

#include "d3plot_part.hpp"
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

} // namespace dro