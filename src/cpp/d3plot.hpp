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

  inline size_t num_time_steps() const { return m_handle.num_states; }

private:
  d3plot_file m_handle;
};

} // namespace dro
