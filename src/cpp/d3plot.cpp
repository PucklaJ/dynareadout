#include "d3plot.hpp"

namespace dro {

D3plot::Exception::Exception(String error_str) noexcept
    : m_error_str(std::move(error_str)) {}

const char *D3plot::Exception::what() const noexcept {
  return m_error_str.data();
}

D3plot::D3plot(const std::filesystem::path &root_file_name) {
  m_handle = d3plot_open(root_file_name.string().c_str());
  if (m_handle.error_string) {
    // Copy the error string and call d3plot_close since the destructor is not
    // getting called
    char *error_string = m_handle.error_string;
    m_handle.error_string = NULL;
    d3plot_close(&m_handle);

    throw Exception(String(error_string, strlen(error_string) + 1));
  }
}

D3plot::~D3plot() noexcept { d3plot_close(&m_handle); }

Array<d3_word> D3plot::read_node_ids() {
  size_t num_ids;
  d3_word *ids = d3plot_read_node_ids(&m_handle, &num_ids);

  return Array<d3_word>(ids, num_ids);
}

Array<d3_word> D3plot::read_solid_element_ids() {
  size_t num_ids;
  d3_word *ids = d3plot_read_solid_element_ids(&m_handle, &num_ids);

  return Array<d3_word>(ids, num_ids);
}

Array<d3_word> D3plot::read_beam_element_ids() {
  size_t num_ids;
  d3_word *ids = d3plot_read_beam_element_ids(&m_handle, &num_ids);

  return Array<d3_word>(ids, num_ids);
}

Array<d3_word> D3plot::read_shell_element_ids() {
  size_t num_ids;
  d3_word *ids = d3plot_read_shell_element_ids(&m_handle, &num_ids);

  return Array<d3_word>(ids, num_ids);
}

Array<d3_word> D3plot::read_solid_shell_element_ids() {
  size_t num_ids;
  d3_word *ids = d3plot_read_solid_shell_element_ids(&m_handle, &num_ids);

  return Array<d3_word>(ids, num_ids);
}

Array<d3_word> D3plot::read_all_element_ids() {
  size_t num_ids;
  d3_word *ids = d3plot_read_all_element_ids(&m_handle, &num_ids);

  return Array<d3_word>(ids, num_ids);
}

Array<dVec3> D3plot::read_node_coordinates(size_t state) {
  size_t num_nodes;
  dVec3 *nodes = reinterpret_cast<dVec3 *>(
      d3plot_read_node_coordinates(&m_handle, state, &num_nodes));

  return Array<dVec3>(nodes, num_nodes);
}

Array<dVec3> D3plot::read_node_velocity(size_t state) {
  size_t num_nodes;
  dVec3 *nodes = reinterpret_cast<dVec3 *>(
      d3plot_read_node_velocity(&m_handle, state, &num_nodes));

  return Array<dVec3>(nodes, num_nodes);
}

Array<dVec3> D3plot::read_node_acceleration(size_t state) {
  size_t num_nodes;
  dVec3 *nodes = reinterpret_cast<dVec3 *>(
      d3plot_read_node_acceleration(&m_handle, state, &num_nodes));

  return Array<dVec3>(nodes, num_nodes);
}

double D3plot::read_time(size_t state) {
  return d3plot_read_time(&m_handle, state);
}

Array<d3plot_solid> D3plot::read_solid_elements() {
  size_t num_elements;
  d3plot_solid *elements = d3plot_read_solid_elements(&m_handle, &num_elements);

  return Array<d3plot_solid>(elements, num_elements);
}

Array<d3plot_thick_shell> D3plot::read_thick_shell_elements() {
  size_t num_elements;
  d3plot_thick_shell *elements =
      d3plot_read_thick_shell_elements(&m_handle, &num_elements);

  return Array<d3plot_thick_shell>(elements, num_elements);
}

Array<d3plot_beam> D3plot::read_beam_elements() {
  size_t num_elements;
  d3plot_beam *elements = d3plot_read_beam_elements(&m_handle, &num_elements);

  return Array<d3plot_beam>(elements, num_elements);
}

Array<d3plot_shell> D3plot::read_shell_elements() {
  size_t num_elements;
  d3plot_shell *elements = d3plot_read_shell_elements(&m_handle, &num_elements);

  return Array<d3plot_shell>(elements, num_elements);
}

} // namespace dro
