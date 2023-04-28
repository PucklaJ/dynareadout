#include "d3plot_state.hpp"
#include <d3plot.h>

namespace dro {

D3plotShellsState::D3plotShellsState(d3plot_shell *data, size_t size,
                                     size_t num_history_variables,
                                     bool delete_data) noexcept
    : Array(data, size, delete_data),
      m_num_history_variables(num_history_variables) {}

D3plotShellsState::~D3plotShellsState() noexcept {
  if (m_data && m_delete_data) {
    d3plot_free_shells_state(reinterpret_cast<d3plot_shell *>(m_data));
    m_data = NULL;
  }
}

Array<double> D3plotShellsState::get_mid_history_variables(size_t index) {
  return Array<double>(Array::operator[](index).mid.history_variables,
                       m_num_history_variables, false);
}

Array<double> D3plotShellsState::get_inner_history_variables(size_t index) {
  return Array<double>(Array::operator[](index).inner.history_variables,
                       m_num_history_variables, false);
}

Array<double> D3plotShellsState::get_outer_history_variables(size_t index) {
  return Array<double>(Array::operator[](index).outer.history_variables,
                       m_num_history_variables, false);
}

const Array<double>
D3plotShellsState::get_mid_history_variables(size_t index) const {
  return Array<double>(Array::operator[](index).mid.history_variables,
                       m_num_history_variables, false);
}

const Array<double>
D3plotShellsState::get_inner_history_variables(size_t index) const {
  return Array<double>(Array::operator[](index).inner.history_variables,
                       m_num_history_variables, false);
}

const Array<double>
D3plotShellsState::get_outer_history_variables(size_t index) const {
  return Array<double>(Array::operator[](index).outer.history_variables,
                       m_num_history_variables, false);
}

D3plotThickShellsState::D3plotThickShellsState(d3plot_thick_shell *data,
                                               size_t size,
                                               size_t num_history_variables,
                                               bool delete_data) noexcept
    : Array(data, size, delete_data),
      m_num_history_variables(num_history_variables) {}

D3plotThickShellsState::~D3plotThickShellsState() noexcept {
  if (m_data && m_delete_data) {
    d3plot_free_thick_shells_state(
        reinterpret_cast<d3plot_thick_shell *>(m_data));
    m_data = NULL;
  }
}

Array<double> D3plotThickShellsState::get_mid_history_variables(size_t index) {
  return Array<double>(Array::operator[](index).mid.history_variables,
                       m_num_history_variables, false);
}

Array<double>
D3plotThickShellsState::get_inner_history_variables(size_t index) {
  return Array<double>(Array::operator[](index).inner.history_variables,
                       m_num_history_variables, false);
}

Array<double>
D3plotThickShellsState::get_outer_history_variables(size_t index) {
  return Array<double>(Array::operator[](index).outer.history_variables,
                       m_num_history_variables, false);
}

const Array<double>
D3plotThickShellsState::get_mid_history_variables(size_t index) const {
  return Array<double>(Array::operator[](index).mid.history_variables,
                       m_num_history_variables, false);
}

const Array<double>
D3plotThickShellsState::get_inner_history_variables(size_t index) const {
  return Array<double>(Array::operator[](index).inner.history_variables,
                       m_num_history_variables, false);
}

const Array<double>
D3plotThickShellsState::get_outer_history_variables(size_t index) const {
  return Array<double>(Array::operator[](index).outer.history_variables,
                       m_num_history_variables, false);
}

} // namespace dro
