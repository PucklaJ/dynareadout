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

#pragma once
#include "array.hpp"
#include <d3_defines.h>

namespace dro {

class D3plotPart {
public:
  D3plotPart(const d3plot_part &part);
  D3plotPart(D3plotPart &&rhs);
  ~D3plotPart();

  D3plotPart &operator=(D3plotPart &&rhs);

  Array<d3_word> get_solid_elements();
  Array<d3_word> get_thick_shell_elements();
  Array<d3_word> get_beam_elements();
  Array<d3_word> get_shell_elements();

private:
  d3plot_part m_part;
};

} // namespace dro
