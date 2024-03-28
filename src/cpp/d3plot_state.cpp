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

#include "d3plot_state.hpp"
#include "d3plot.hpp"
#include <sstream>

namespace dro {

D3plotSurface::~D3plotSurface() noexcept {
  d3plot_free_surface(*static_cast<d3plot_surface *>(this));
}

D3plotSurface D3plotShell::get_mean() const noexcept {
  d3plot_surface ip =
      d3plot_get_shell_mean(static_cast<const d3plot_shell *>(this));
  return *static_cast<D3plotSurface *>(&ip);
}

template <> Array<D3plotShell>::~Array<D3plotShell>() noexcept {
  if (m_delete_data && m_data) {
    d3plot_free_shells_state(static_cast<d3plot_shell *>(m_data));
    m_data = nullptr;
  }
}

const Array<double> D3plotShell::get_mean_history_variables() const noexcept {
  d3plot_surface ip =
      d3plot_get_shell_mean(static_cast<const d3plot_shell *>(this));
  return Array<double>(ip.history_variables, num_history_variables);
}

const Array<double>
D3plotShell::get_add_ip_history_variables(size_t add_idx) const {
  if (add_idx >= num_additional_integration_points) {
    std::stringstream stream;
    stream << add_idx
           << " is an invalid index for additional integration points ("
           << add_idx << " >= " << num_additional_integration_points << ")";
    const auto str(stream.str());
    throw D3plot::Exception(
        D3plot::Exception::ErrorString(strdup(str.c_str())));
  }

  return Array<double>(add_ips[add_idx].history_variables,
                       num_history_variables, false);
}

D3plotSurface D3plotThickShell::get_mean() const noexcept {
  d3plot_surface ip = d3plot_get_thick_shell_mean(
      static_cast<const d3plot_thick_shell *>(this));
  return *static_cast<D3plotSurface *>(&ip);
}

template <> Array<D3plotThickShell>::~Array<D3plotThickShell>() noexcept {
  if (m_delete_data && m_data) {
    d3plot_free_thick_shells_state(
        reinterpret_cast<d3plot_thick_shell *>(m_data));
    m_data = nullptr;
  }
}

const Array<double>
D3plotThickShell::get_mean_history_variables() const noexcept {
  d3plot_surface ip = d3plot_get_thick_shell_mean(
      reinterpret_cast<const d3plot_thick_shell *>(this));
  return Array<double>(ip.history_variables, num_history_variables);
}

const Array<double>
D3plotThickShell::get_add_ip_history_variables(uint8_t add_idx) const {
  if (add_idx >= num_additional_integration_points) {
    std::stringstream stream;
    stream << (int)add_idx
           << " is an invalid index for additional integration points ("
           << (int)add_idx << " >= " << (int)num_additional_integration_points
           << ")";
    const auto str(stream.str());
    throw D3plot::Exception(
        D3plot::Exception::ErrorString(strdup(str.c_str())));
  }

  return Array<double>(add_ips[add_idx].history_variables,
                       num_history_variables, false);
}

const Array<double> D3plotBeam::get_history_variables(uint8_t ip_idx) const {
  if (ip_idx >= num_integration_points) {
    std::stringstream stream;
    stream << (int)ip_idx << " is an invalid index for integration points ("
           << (int)ip_idx << " >= " << (int)num_integration_points << ")";
    const auto str(stream.str());
    throw D3plot::Exception(
        D3plot::Exception::ErrorString(strdup(str.c_str())));
  }

  return Array<double>(ips[ip_idx].history_variables, num_history_variables,
                       false);
}

template <> Array<D3plotBeam>::~Array<D3plotBeam>() noexcept {
  if (m_delete_data && m_data) {
    d3plot_free_beams_state(static_cast<d3plot_beam *>(m_data));
    m_data = nullptr;
  }
}

} // namespace dro
