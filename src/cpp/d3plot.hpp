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
#include "d3plot_part.hpp"
#include "d3plot_state.hpp"
#include "vec.hpp"
#include <chrono>
#include <d3plot.h>
#include <exception>
#include <filesystem>
#include <vector>

namespace dro {

// This holds all data needed to read d3plot files
class D3plot {
public:
  class Exception : public std::exception {
  public:
#ifdef _WIN32
    // On windows python scripts crash when deallocated the memory of a thrown
    // exception
    class ErrorString : public std::string {
    public:
      ErrorString(char *str, bool delete_data = true) noexcept
          : std::string(str) {
        if (delete_data)
          free(str);
      }
    };
#else
    using ErrorString = String;
#endif

    Exception(ErrorString error_str) noexcept;

    const char *what() const noexcept override;

  private:
    const ErrorString m_error_str;
  };

  static size_t index_for_id(const Array<d3_word> &ids, d3_word id);

  D3plot(D3plot &&rhs) noexcept;
  // Open a d3plot file family by giving the root file name
  // Example: d3plot of d3plot01, d3plot02, d3plot03, etc.
  D3plot(const std::filesystem::path &root_file_name);
  ~D3plot() noexcept;

  D3plot &operator=(D3plot &&rhs) noexcept;

  // Read all ids of the nodes
  Array<d3_word> read_node_ids();
  // Read all ids of the solid elements
  Array<d3_word> read_solid_element_ids();
  // Read all ids of the beam elements
  Array<d3_word> read_beam_element_ids();
  // Read all ids of the shell elements
  Array<d3_word> read_shell_element_ids();
  // Read all ids of the solid shell elements
  Array<d3_word> read_thick_shell_element_ids();
  // Read all ids of the solid, beam, shell and thick shell elements
  Array<d3_word> read_all_element_ids();
  // Read all ids of the parts
  Array<d3_word> read_part_ids();
  // Returns a vector containing all part titles as sized strings
  std::vector<SizedString> read_part_titles();

  // Read the node coordinates of all nodes of a given state (time step)
  Array<dVec3> read_node_coordinates(size_t state = 0);
  // Reads all node coordinates of all time steps and returns it as one big
  // array
  std::vector<Array<dVec3>> read_all_node_coordinates();
  // Read the node velocity of all nodes of a given state (time step)
  Array<dVec3> read_node_velocity(size_t state);
  std::vector<Array<dVec3>> read_all_node_velocity();
  // Read the node acceleration of all nodes of a given state (time step)
  Array<dVec3> read_node_acceleration(size_t state);
  std::vector<Array<dVec3>> read_all_node_acceleration();
  // The same as read_node_coordinates but with floats instead of doubles
  Array<fVec3> read_node_coordinates_32(size_t state);
  // Reads all node coordinates of all time steps and returns it as one big
  // array
  std::vector<Array<fVec3>> read_all_node_coordinates_32();
  // The same as read_node_velocity but with floats instead of doubles
  Array<fVec3> read_node_velocity_32(size_t state);
  std::vector<Array<fVec3>> read_all_node_velocity_32();
  // The same as read_node_acceleration but with floats instead of doubles
  Array<fVec3> read_node_acceleration_32(size_t state);
  std::vector<Array<fVec3>> read_all_node_acceleration_32();
  // Read the time of a given state (time step) in milliseconds
  double read_time(size_t state);
  // Reads all time of every state (time step) in milliseconds
  Array<double> read_all_time();
  // Read the time of a given state (time step) in milliseconds
  float read_time_32(size_t state);
  // Reads all time of every state (time step) in milliseconds
  Array<float> read_all_time_32();
  // Returns stress, strain (if NEIPH >= 6) for a given state
  Array<d3plot_solid> read_solids_state(size_t state);
  // Returns stress, strain (if ISTRN == 1) for a given state
  D3plotThickShellsState read_thick_shells_state(size_t state);
  // Returns Axial Force, S shear resultant, T shear resultant, S bending
  // moment, T bending moment and Torsional resultant of all beams for a given
  // state
  Array<d3plot_beam> read_beams_state(size_t state);
  // Returns stress, strain (if ISTRN == 1) and some other variables (see docs
  // pg. 36) of all shells for a given state
  D3plotShellsState read_shells_state(size_t state);

  // Returns the node connectivity + material number of all 8 node solid
  // elements
  Array<d3plot_solid_con> read_solid_elements();
  // Returns the node connectivity + material number of all 8 node thick shell
  // elements
  Array<d3plot_thick_shell_con> read_thick_shell_elements();
  // Returns the node connectivity + orientation node + material number of all
  // beam elements
  Array<d3plot_beam_con> read_beam_elements();
  // Returns the node connectivity + material number of all shell
  // elements
  Array<d3plot_shell_con> read_shell_elements();
  // Returns a string holding the Title of the d3plot file
  SizedString read_title();
  // Returns the time at which the simulation has been run as calender time
  std::chrono::system_clock::time_point read_run_time();
  // Returns all elements of a part. The part_index can retrieved by iterating
  // over the array returned by read_part_ids
  D3plotPart read_part(size_t part_index);
  // The same as read_part, but instead of an index into the parts, this
  // functions takes an id. You can supply this function with the part ids
  // returned by read_part_ids. If you do not do this, they will be read in this
  // function call. Which means that if you intend to call this function
  // multiple times, it is best to preload the part ids.
  D3plotPart read_part_by_id(size_t part_id,
                             const Array<d3_word> &part_ids = Array<d3_word>());

  // Returns the number of states (time steps)
  inline size_t num_time_steps() const { return m_handle.num_states; }

  inline d3plot_file &get_handle() { return m_handle; }
  inline const d3plot_file &get_handle() const { return m_handle; }

private:
  // The underlying C handle of the d3plot file
  d3plot_file m_handle;
};

} // namespace dro

// Stream output operators for structs
std::ostream &operator<<(std::ostream &stream, const d3plot_tensor &t);
std::ostream &operator<<(std::ostream &stream, const d3plot_x_y &x);
std::ostream &operator<<(std::ostream &stream, const d3plot_x_y_xy &x);
std::ostream &operator<<(std::ostream &stream, const d3plot_surface &s);
std::ostream &operator<<(std::ostream &stream, const d3plot_solid &s);
std::ostream &operator<<(std::ostream &stream, const d3plot_thick_shell &ts);
std::ostream &operator<<(std::ostream &stream, const d3plot_beam &b);
std::ostream &operator<<(std::ostream &stream, const d3plot_shell &s);

std::ostream &operator<<(std::ostream &stream, const d3plot_solid_con &s);
std::ostream &operator<<(std::ostream &stream, const d3plot_beam_con &b);
std::ostream &operator<<(std::ostream &stream, const d3plot_shell_con &s);