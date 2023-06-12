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

class D3plotShellsState : public Array<d3plot_shell> {
public:
  D3plotShellsState(d3plot_shell *data, size_t size,
                    size_t num_history_variables,
                    bool delete_data = true) noexcept;
  D3plotShellsState(D3plotShellsState &&rhs) noexcept;
  ~D3plotShellsState() noexcept override;

  inline size_t get_num_history_variables() const noexcept {
    return m_num_history_variables;
  }

  const Array<double> get_mid_history_variables(size_t index) const;
  const Array<double> get_inner_history_variables(size_t index) const;
  const Array<double> get_outer_history_variables(size_t index) const;

private:
  size_t m_num_history_variables;
};

class D3plotThickShellsState : public Array<d3plot_thick_shell> {
public:
  D3plotThickShellsState(d3plot_thick_shell *data, size_t size,
                         size_t num_history_variables,
                         bool delete_data = true) noexcept;
  D3plotThickShellsState(D3plotThickShellsState &&rhs) noexcept;
  ~D3plotThickShellsState() noexcept override;

  inline size_t get_num_history_variables() const noexcept {
    return m_num_history_variables;
  }

  const Array<double> get_mid_history_variables(size_t index) const;
  const Array<double> get_inner_history_variables(size_t index) const;
  const Array<double> get_outer_history_variables(size_t index) const;

private:
  size_t m_num_history_variables;
};

} // namespace dro
