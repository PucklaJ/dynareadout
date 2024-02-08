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

class D3plotShell : public d3plot_shell {
public:
  inline const Array<double> get_mid_history_variables() const noexcept {
    return Array<double>(mid.history_variables, num_history_variables, false);
  }
  inline const Array<double> get_inner_history_variables() const noexcept {
    return Array<double>(inner.history_variables, num_history_variables, false);
  }
  inline const Array<double> get_outer_history_variables() const noexcept {
    return Array<double>(outer.history_variables, num_history_variables, false);
  }
  const Array<double> get_add_ip_history_variables(size_t add_idx) const;

  inline const Array<d3plot_surface> get_add_ips() const noexcept {
    return Array<d3plot_surface>(add_ips, num_additional_integration_points,
                                 false);
  }
};

template <> Array<D3plotShell>::~Array<D3plotShell>() noexcept;

class D3plotThickShell : public d3plot_thick_shell {
public:
  inline const Array<double> get_mid_history_variables() const noexcept {
    return Array<double>(mid.history_variables, num_history_variables, false);
  }
  inline const Array<double> get_inner_history_variables() const noexcept {
    return Array<double>(inner.history_variables, num_history_variables, false);
  }
  inline const Array<double> get_outer_history_variables() const noexcept {
    return Array<double>(outer.history_variables, num_history_variables, false);
  }
};

template <> Array<D3plotThickShell>::~Array<D3plotThickShell>() noexcept;

} // namespace dro
