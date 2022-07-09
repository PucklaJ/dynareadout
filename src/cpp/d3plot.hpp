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

#pragma once
#include "array.hpp"
#include "vec.hpp"
#include <d3plot.h>
#include <exception>
#include <filesystem>

namespace dro {

class D3plot {
public:
  class Exception : public std::exception {
  public:
    Exception(String error_str) noexcept;

    const char *what() const noexcept override;

  private:
    const String m_error_str;
  };

  D3plot(const std::filesystem::path &root_file_name);
  ~D3plot() noexcept;

  Array<d3_word> read_node_ids();
  Array<d3_word> read_solid_element_ids();
  Array<d3_word> read_beam_element_ids();
  Array<d3_word> read_shell_element_ids();
  Array<d3_word> read_solid_shell_element_ids();
  Array<d3_word> read_all_element_ids();

  Array<dVec3> read_node_coordinates(size_t state);
  Array<dVec3> read_node_velocity(size_t state);
  Array<dVec3> read_node_acceleration(size_t state);
  double read_time(size_t state);

  Array<d3plot_solid> read_solid_elements();
  Array<d3plot_thick_shell> read_thick_shell_elements();
  Array<d3plot_beam> read_beam_elements();
  Array<d3plot_shell> read_shell_elements();
  String read_title();

  inline size_t num_time_steps() const { return m_handle.num_states; }

private:
  d3plot_file m_handle;
};

} // namespace dro
