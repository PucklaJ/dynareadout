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

class D3plot;

// A class used to access data of a certain part in a d3plot file (family)
class D3plotPart {
public:
  D3plotPart(D3plotPart &&rhs) noexcept;
  D3plotPart(const D3plotPart &rhs) noexcept;
  D3plotPart(const d3plot_part &part) noexcept;
  ~D3plotPart() noexcept;

  D3plotPart &operator=(D3plotPart &&rhs) noexcept;
  D3plotPart &operator=(const D3plotPart &rhs) noexcept;

  // Returns all solid element ids of the part
  Array<d3_word> get_solid_elements();
  // Returns all thick shell element ids of the part
  Array<d3_word> get_thick_shell_elements();
  // Returns all beam element ids of the part
  Array<d3_word> get_beam_elements();
  // Returns all shell element ids of the part
  Array<d3_word> get_shell_elements();

  // Returns all solid element indices of the part
  Array<size_t> get_solid_element_indices();
  // Returns all thick shell element indices of the part
  Array<size_t> get_thick_shell_element_indices();
  // Returns all beam element indices of the part
  Array<size_t> get_beam_element_indices();
  // Returns all shell element indices of the part
  Array<size_t> get_shell_element_indices();

  // Returns all node ids of the part. All ids and connectivities of all
  // different elements can be provided to improve performance, since they would
  // not need to be loaded.
  Array<d3_word> get_node_ids(
      D3plot &plot_file, const Array<d3_word> *solid_ids = nullptr,
      const Array<d3_word> *beam_ids = nullptr,
      const Array<d3_word> *shell_ids = nullptr,
      const Array<d3_word> *thick_shell_ids = nullptr,
      const Array<d3_word> *node_ids = nullptr,
      const Array<d3plot_solid_con> *solid_cons = nullptr,
      const Array<d3plot_beam_con> *beam_cons = nullptr,
      const Array<d3plot_shell_con> *shell_cons = nullptr,
      const Array<d3plot_thick_shell_con> *thick_shell_cons = nullptr) const;
  // Returns all node indices of the part. All ids and connectivities of all
  // different elements can be provided to improve performance, since they would
  // not need to be loaded. This returns indices into the node ids array
  // returned by dro::D3plot::read_node_ids
  Array<d3_word> get_node_indices(
      D3plot &plot_file, const Array<d3_word> *solid_ids = nullptr,
      const Array<d3_word> *beam_ids = nullptr,
      const Array<d3_word> *shell_ids = nullptr,
      const Array<d3_word> *thick_shell_ids = nullptr,
      const Array<d3plot_solid_con> *solid_cons = nullptr,
      const Array<d3plot_beam_con> *beam_cons = nullptr,
      const Array<d3plot_shell_con> *shell_cons = nullptr,
      const Array<d3plot_thick_shell_con> *thick_shell_cons = nullptr) const;

  // Returns the number of nodes of a part. Internally it just calls
  // get_node_indices and returns the number of node indices.
  size_t get_num_nodes(
      D3plot &plot_file, const Array<d3_word> *solid_ids = nullptr,
      const Array<d3_word> *beam_ids = nullptr,
      const Array<d3_word> *shell_ids = nullptr,
      const Array<d3_word> *thick_shell_ids = nullptr,
      const Array<d3plot_solid_con> *solid_cons = nullptr,
      const Array<d3plot_beam_con> *beam_cons = nullptr,
      const Array<d3plot_shell_con> *shell_cons = nullptr,
      const Array<d3plot_thick_shell_con> *thick_shell_cons = nullptr) const;

  // Returns the number of all elements of a part
  size_t get_num_elements() const;

  // Returns an array containing all element ids
  Array<d3_word> get_all_element_ids() const;

private:
  d3plot_part m_part;
};

} // namespace dro
