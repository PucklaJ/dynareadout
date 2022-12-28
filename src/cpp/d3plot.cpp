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

#include "d3plot.hpp"
#include <ctime>

namespace dro {

D3plot::Exception::Exception(String error_str) noexcept
    : m_error_str(std::move(error_str)) {}

const char *D3plot::Exception::what() const noexcept {
  return m_error_str.data();
}

size_t D3plot::index_for_id(const Array<d3_word> &ids, d3_word id) {
  return d3plot_index_for_id(id, ids.data(), ids.size());
}

D3plot::D3plot(const std::filesystem::path &root_file_name) {
  m_handle = d3plot_open(root_file_name.string().c_str());
  if (m_handle.error_string) {
    // Copy the error string and call d3plot_close since the destructor is not
    // getting called
    char *error_string = m_handle.error_string;
    m_handle.error_string = NULL;
    d3plot_close(&m_handle);

    throw Exception(String(error_string));
  }
}

D3plot::~D3plot() noexcept { d3plot_close(&m_handle); }

Array<d3_word> D3plot::read_node_ids() {
  size_t num_ids;
  d3_word *ids = d3plot_read_node_ids(&m_handle, &num_ids);
  if (m_handle.error_string) {
    throw Exception(String(m_handle.error_string, false));
  }

  return Array<d3_word>(ids, num_ids);
}

Array<d3_word> D3plot::read_solid_element_ids() {
  size_t num_ids;
  d3_word *ids = d3plot_read_solid_element_ids(&m_handle, &num_ids);
  if (m_handle.error_string) {
    throw Exception(String(m_handle.error_string, false));
  }

  return Array<d3_word>(ids, num_ids);
}

Array<d3_word> D3plot::read_beam_element_ids() {
  size_t num_ids;
  d3_word *ids = d3plot_read_beam_element_ids(&m_handle, &num_ids);
  if (m_handle.error_string) {
    throw Exception(String(m_handle.error_string, false));
  }

  return Array<d3_word>(ids, num_ids);
}

Array<d3_word> D3plot::read_shell_element_ids() {
  size_t num_ids;
  d3_word *ids = d3plot_read_shell_element_ids(&m_handle, &num_ids);
  if (m_handle.error_string) {
    throw Exception(String(m_handle.error_string, false));
  }

  return Array<d3_word>(ids, num_ids);
}

Array<d3_word> D3plot::read_solid_shell_element_ids() {
  size_t num_ids;
  d3_word *ids = d3plot_read_thick_shell_element_ids(&m_handle, &num_ids);
  if (m_handle.error_string) {
    throw Exception(String(m_handle.error_string, false));
  }

  return Array<d3_word>(ids, num_ids);
}

Array<d3_word> D3plot::read_all_element_ids() {
  size_t num_ids;
  d3_word *ids = d3plot_read_all_element_ids(&m_handle, &num_ids);
  if (m_handle.error_string) {
    throw Exception(String(m_handle.error_string, false));
  }

  return Array<d3_word>(ids, num_ids);
}

Array<d3_word> D3plot::read_part_ids() {
  size_t num_ids;
  d3_word *ids = d3plot_read_part_ids(&m_handle, &num_ids);
  if (m_handle.error_string) {
    throw Exception(String(m_handle.error_string, false));
  }

  return Array<d3_word>(ids, num_ids);
}

std::vector<String> D3plot::read_part_titles() {
  size_t num_parts;
  char **part_titles = d3plot_read_part_titles(&m_handle, &num_parts);
  if (m_handle.error_string) {
    throw Exception(String(m_handle.error_string, false));
  }

  std::vector<String> vec;
  vec.reserve(num_parts);
  for (size_t i = 0; i < num_parts; i++) {
    vec.emplace_back(part_titles[i]);
  }

  free(part_titles);
  return vec;
}

Array<dVec3> D3plot::read_node_coordinates(size_t state) {
  size_t num_nodes;
  dVec3 *nodes = reinterpret_cast<dVec3 *>(
      d3plot_read_node_coordinates(&m_handle, state, &num_nodes));
  if (m_handle.error_string) {
    throw Exception(String(m_handle.error_string, false));
  }

  return Array<dVec3>(nodes, num_nodes);
}

Array<dVec3> D3plot::read_node_velocity(size_t state) {
  size_t num_nodes;
  dVec3 *nodes = reinterpret_cast<dVec3 *>(
      d3plot_read_node_velocity(&m_handle, state, &num_nodes));
  if (m_handle.error_string) {
    throw Exception(String(m_handle.error_string, false));
  }

  return Array<dVec3>(nodes, num_nodes);
}

Array<dVec3> D3plot::read_node_acceleration(size_t state) {
  size_t num_nodes;
  dVec3 *nodes = reinterpret_cast<dVec3 *>(
      d3plot_read_node_acceleration(&m_handle, state, &num_nodes));
  if (m_handle.error_string) {
    throw Exception(String(m_handle.error_string, false));
  }

  return Array<dVec3>(nodes, num_nodes);
}

Array<fVec3> D3plot::read_node_coordinates_32(size_t state) {
  size_t num_nodes;
  fVec3 *nodes = reinterpret_cast<fVec3 *>(
      d3plot_read_node_coordinates_32(&m_handle, state, &num_nodes));
  if (m_handle.error_string) {
    throw Exception(String(m_handle.error_string, false));
  }

  return Array<fVec3>(nodes, num_nodes);
}

Array<fVec3> D3plot::read_node_velocity_32(size_t state) {
  size_t num_nodes;
  fVec3 *nodes = reinterpret_cast<fVec3 *>(
      d3plot_read_node_velocity_32(&m_handle, state, &num_nodes));
  if (m_handle.error_string) {
    throw Exception(String(m_handle.error_string, false));
  }

  return Array<fVec3>(nodes, num_nodes);
}

Array<fVec3> D3plot::read_node_acceleration_32(size_t state) {
  size_t num_nodes;
  fVec3 *nodes = reinterpret_cast<fVec3 *>(
      d3plot_read_node_acceleration_32(&m_handle, state, &num_nodes));
  if (m_handle.error_string) {
    throw Exception(String(m_handle.error_string, false));
  }

  return Array<fVec3>(nodes, num_nodes);
}

double D3plot::read_time(size_t state) {
  double time{d3plot_read_time(&m_handle, state)};
  if (m_handle.error_string) {
    throw Exception(String(m_handle.error_string, false));
  }
  return time;
}

Array<d3plot_solid> D3plot::read_solids_state(size_t state) {
  size_t num_elements;
  d3plot_solid *elements =
      d3plot_read_solids_state(&m_handle, state, &num_elements);
  if (m_handle.error_string) {
    throw Exception(String(m_handle.error_string, false));
  }

  return Array<d3plot_solid>(elements, num_elements);
}

Array<d3plot_thick_shell> D3plot::read_thick_shells_state(size_t state) {
  size_t num_elements;
  d3plot_thick_shell *elements =
      d3plot_read_thick_shells_state(&m_handle, state, &num_elements);
  if (m_handle.error_string) {
    throw Exception(String(m_handle.error_string, false));
  }

  return Array<d3plot_thick_shell>(elements, num_elements);
}

Array<d3plot_beam> D3plot::read_beams_state(size_t state) {
  size_t num_elements;
  d3plot_beam *elements =
      d3plot_read_beams_state(&m_handle, state, &num_elements);
  if (m_handle.error_string) {
    throw Exception(String(m_handle.error_string, false));
  }

  return Array<d3plot_beam>(elements, num_elements);
}

Array<d3plot_shell> D3plot::read_shells_state(size_t state) {
  size_t num_elements;
  d3plot_shell *elements =
      d3plot_read_shells_state(&m_handle, state, &num_elements);
  if (m_handle.error_string) {
    throw Exception(String(m_handle.error_string, false));
  }

  return Array<d3plot_shell>(elements, num_elements);
}

Array<d3plot_solid_con> D3plot::read_solid_elements() {
  size_t num_elements;
  d3plot_solid_con *elements =
      d3plot_read_solid_elements(&m_handle, &num_elements);
  if (m_handle.error_string) {
    throw Exception(String(m_handle.error_string, false));
  }

  return Array<d3plot_solid_con>(elements, num_elements);
}

Array<d3plot_thick_shell_con> D3plot::read_thick_shell_elements() {
  size_t num_elements;
  d3plot_thick_shell_con *elements =
      d3plot_read_thick_shell_elements(&m_handle, &num_elements);
  if (m_handle.error_string) {
    throw Exception(String(m_handle.error_string, false));
  }

  return Array<d3plot_thick_shell_con>(elements, num_elements);
}

Array<d3plot_beam_con> D3plot::read_beam_elements() {
  size_t num_elements;
  d3plot_beam_con *elements =
      d3plot_read_beam_elements(&m_handle, &num_elements);
  if (m_handle.error_string) {
    throw Exception(String(m_handle.error_string, false));
  }

  return Array<d3plot_beam_con>(elements, num_elements);
}

Array<d3plot_shell_con> D3plot::read_shell_elements() {
  size_t num_elements;
  d3plot_shell_con *elements =
      d3plot_read_shell_elements(&m_handle, &num_elements);
  if (m_handle.error_string) {
    throw Exception(String(m_handle.error_string, false));
  }

  return Array<d3plot_shell_con>(elements, num_elements);
}

String D3plot::read_title() {
  char *title = d3plot_read_title(&m_handle);
  if (m_handle.error_string) {
    throw Exception(String(m_handle.error_string, false));
  }
  return String(title);
}

struct tm *D3plot::read_run_time() {
  struct tm *run_time = d3plot_read_run_time(&m_handle);
  if (m_handle.error_string) {
    throw Exception(String(m_handle.error_string, false));
  }
  return run_time;
}

D3plotPart D3plot::read_part(size_t part_index) {
  d3plot_part part = d3plot_read_part(&m_handle, part_index);
  if (m_handle.error_string) {
    throw Exception(String(m_handle.error_string, false));
  }
  return D3plotPart(part);
}

D3plotPart D3plot::read_part_by_id(size_t part_id,
                                   const Array<d3_word> &part_ids) {
  d3plot_part part = d3plot_read_part_by_id(&m_handle, part_id, part_ids.data(),
                                            part_ids.size());
  if (m_handle.error_string) {
    throw Exception(String(m_handle.error_string, false));
  }
  return D3plotPart(part);
}

} // namespace dro
