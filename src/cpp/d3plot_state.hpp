#pragma once

#include "array.hpp"
#include <d3_defines.h>

namespace dro {

class D3plotShellsState : public Array<d3plot_shell> {
public:
  D3plotShellsState(d3plot_shell *data, size_t size,
                    size_t num_history_variables,
                    bool delete_data = true) noexcept;
  ~D3plotShellsState() noexcept override;

  Array<double> get_mid_history_variables(size_t index);
  Array<double> get_inner_history_variables(size_t index);
  Array<double> get_outer_history_variables(size_t index);

  const Array<double> get_mid_history_variables(size_t index) const;
  const Array<double> get_inner_history_variables(size_t index) const;
  const Array<double> get_outer_history_variables(size_t index) const;

private:
  size_t m_num_history_variables;
};

} // namespace dro
