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

#ifndef D3PLOT_H
#define D3PLOT_H
#include "d3_buffer.h"
#include "d3_defines.h"

struct tm;

/* This holds all data needed to read d3plot files*/
typedef struct {
  struct {
    /* These are all the values inside the CONTROL DATA section of the first
     * d3plot file (root file)*/
    d3_word ndim, numnp, icode, nglbv, it, iu, iv, ia, nummat8, nv3d, nel2,
        nummat2, nv1d, nel4, nummat4, nv2d, neiph, neips, nmsph, narbs, nelt,
        nummatt, nv3dt, ioshl[4], ialemat, ncfdv1, nadapt, nmmat, npefg, nel48,
        idtdt, nel20, nt3d;
    /* This will be calculated*/
    d3_word numrbs;
    /* These variables can by negative*/
    int64_t nel8, maxint;
    /* These values will also be calculated*/
    uint8_t mdlopt, mattyp, istrn;

    /* These are some values also being calculated, but are not part of the
     * documentation*/
    uint8_t plastic_strain_tensor_written, thermal_strain_tensor_written,
        element_connectivity_packed;
  } control_data;

  /* This array holds the word locations of different data*/
  size_t *data_pointers;
  size_t num_states;

  d3_buffer buffer;
  /* This holds an error after calling some functions*/
  char *error_string;
} d3plot_file;

#ifdef __cplusplus
extern "C" {
#endif

/* Open a d3plot file family by giving the root file name
 * Example: d3plot of d3plot01, d3plot02, d3plot03, etc.*/
d3plot_file d3plot_open(const char *root_file_name);
/* Close a d3plot_file and deallocate all the memory*/
void d3plot_close(d3plot_file *plot_file);
/* Read all ids of the nodes. The return value needs to be deallocated by free*/
d3_word *d3plot_read_node_ids(d3plot_file *plot_file, size_t *num_ids);
/* Read all ids of the solid elements. The return value needs to be deallocated
 * by free*/
d3_word *d3plot_read_solid_element_ids(d3plot_file *plot_file, size_t *num_ids);
/* Read all ids of the beam elements. The return value needs to be deallocated
 * by free*/
d3_word *d3plot_read_beam_element_ids(d3plot_file *plot_file, size_t *num_ids);
/* Read all ids of the shell elements. The return value needs to be deallocated
 * by free*/
d3_word *d3plot_read_shell_element_ids(d3plot_file *plot_file, size_t *num_ids);
/* Read all ids of the thick shell elements. The return value needs to be
 * deallocated by free*/
d3_word *d3plot_read_thick_shell_element_ids(d3plot_file *plot_file,
                                             size_t *num_ids);
/* Read all ids of the parts. The return value needs to be deallocated by free*/
d3_word *d3plot_read_part_ids(d3plot_file *plot_file, size_t *num_parts);
/* Returns an array containing null terminated strings for the part titles. Each
 * element of the array needs to be deallocated by free and the array itself
 * also needs to deallocated by free*/
char **d3plot_read_part_titles(d3plot_file *plot_file, size_t *num_parts);
/* Returns an array containing all axes of all nodes at a given state. See:
 * XYZXYZXYZXYZ...*/
/* Read the node coordinates of all nodes of a given state (time step). The
 * return value needs to deallocated by free. Example: X,Y and Z values of node
 * with index 20: rv[20*3+0], rv[20*3+1], rv[20*3+2]*/
double *d3plot_read_node_coordinates(d3plot_file *plot_file, size_t state,
                                     size_t *num_nodes);
/* Read the node velocity of all nodes of a given state (time step). The
 * return value needs to deallocated by free. Example: X,Y and Z values of node
 * with index 20: rv[20*3+0], rv[20*3+1], rv[20*3+2]*/
double *d3plot_read_node_velocity(d3plot_file *plot_file, size_t state,
                                  size_t *num_nodes);
/* Read the node acceleration of all nodes of a given state (time step). The
 * return value needs to deallocated by free. Example: X,Y and Z values of node
 * with index 20: rv[20*3+0], rv[20*3+1], rv[20*3+2]*/
double *d3plot_read_node_acceleration(d3plot_file *plot_file, size_t state,
                                      size_t *num_nodes);
/* The same as d3plot_read_node_coordinates but it does not convert floats to
 * double. It does the opposite if the word size is 8*/
float *d3plot_read_node_coordinates_32(d3plot_file *plot_file, size_t state,
                                       size_t *num_nodes);
/* The same as d3plot_read_node_velocity but it does not convert floats to
 * double. It does the opposite if the word size is 8*/
float *d3plot_read_node_velocity_32(d3plot_file *plot_file, size_t state,
                                    size_t *num_nodes);
/* The same as d3plot_read_node_acceleration but it does not convert floats to
 * double. It does the opposite if the word size is 8*/
float *d3plot_read_node_acceleration_32(d3plot_file *plot_file, size_t state,
                                        size_t *num_nodes);
/* Read the time of a given state (time step) in milliseconds*/
double d3plot_read_time(d3plot_file *plot_file, size_t state);
/* Returns stress, strain (if NEIPH >= 6) for a given state. The return value
 * needs to be deallocated by free.*/
d3plot_solid *d3plot_read_solids_state(d3plot_file *plot_file, size_t state,
                                       size_t *num_solids);
/* Returns stress, strain (if ISTRN == 1) for a given state. The return value
 * needs to be deallocated by free.*/
d3plot_thick_shell *d3plot_read_thick_shells_state(d3plot_file *plot_file,
                                                   size_t state,
                                                   size_t *num_thick_shells);
/* Returns Axial Force, S shear resultant, T shear resultant, S bending moment,
 * T bending moment and Torsional resultant of all beams for a given state. The
 * return value needs to be deallocated by free.*/
d3plot_beam *d3plot_read_beams_state(d3plot_file *plot_file, size_t state,
                                     size_t *num_beams);
/* Returns stress, strain (if ISTRN == 1) and some other variables (see docs pg.
 * 36) of all shells for a given state. The return value needs to be deallocated
 * by free.*/
d3plot_shell *d3plot_read_shells_state(d3plot_file *plot_file, size_t state,
                                       size_t *num_shells);
/* Returns the node connectivity + material number of all 8 node solid
 * elements. The return value needs to be deallocated by free*/
d3plot_solid_con *d3plot_read_solid_elements(d3plot_file *plot_file,
                                             size_t *num_solids);
/* Returns the node connectivity + material number of all 8 node thick shell
 * elements. The return value needs to be deallocated by free*/
d3plot_thick_shell_con *
d3plot_read_thick_shell_elements(d3plot_file *plot_file,
                                 size_t *num_thick_shells);
/* Returns the node connectivity + orientation node + material number of all
 * beam elements. The return value needs to be deallocated by free*/
d3plot_beam_con *d3plot_read_beam_elements(d3plot_file *plot_file,
                                           size_t *num_beams);
/* Returns the node connectivity +  material number of all
 * shell elements. The return value needs to be deallocated by free*/
d3plot_shell_con *d3plot_read_shell_elements(d3plot_file *plot_file,
                                             size_t *num_shells);
/* Returns a null terminated string holding the Title of the d3plot file. The
 * return value needs to be deallocated by free.*/
char *d3plot_read_title(d3plot_file *plot_file);
/* Returns the time at which the simulation has been run as calender time*/
struct tm *d3plot_read_run_time(d3plot_file *plot_file);
/* Returns all elements of a part. The part_index can retrieved by iterating
 * over the array returned by d3plot_read_part_ids. The return value needs to be
 * deallocated by _d3plot_free_part*/
d3plot_part d3plot_read_part(d3plot_file *plot_file, size_t part_index);
/* The same as d3plot_read_part, but instead of an index into the parts, this
 * function takes an id. You can supply this function with the part ids returned
 * by d3plot_read_part_ids. If you set part_ids to NULL they will be read in
 * this function call. Which means that if you intend to call this function
 * multiple times, it is best to preload the part ids. The return value needs to
 * be deallocated by _d3plot_free_part.*/
d3plot_part d3plot_read_part_by_id(d3plot_file *plot_file, d3_word part_id,
                                   const d3_word *part_ids, size_t num_parts);

/***** Data sections *******/
/* GEOMETRY DATA pg. 17*/
int _d3plot_read_geometry_data(d3plot_file *plot_file);
/* USER MATERIAL, NODE, AND ELEMENT IDENTIFICATION NUMBERS pg. 18*/
int _d3plot_read_user_identification_numbers(d3plot_file *plot_file);
/* EXTRA 2, 4, 12 NODE CONNECTIVITY FOR 10, 8, 20 NODE TETRAHEDRON, SHELL, SOLID
 * ELEMENTS pg. 19*/
int _d3plot_read_extra_node_connectivity(d3plot_file *plot_file);
/* ADAPTED ELEMENT PARENT LIST (not implemented) pg. 19*/
int _d3plot_read_adapted_element_parent_list(d3plot_file *plot_file);
/* HEADER, PART & CONTACT INTERFACE TITLES pg. 22*/
int _d3plot_read_header(d3plot_file *plot_file);
/* STATE DATA pg. 31*/
int _d3plot_read_state_data(d3plot_file *plot_file);
/***************************/

/***** Private Functions ********/
/* Return a string representing the given file type*/
const char *_d3plot_get_file_type_name(d3_word file_type);
/* Return the nth digit of an integer as an integer.
 * Example: value=32, n=0 -> rv=2; value=32, n=1 -> rv=3;
 * value=82376345, n=5 -> rv=3*/
int _get_nth_digit(d3_word value, int n);
/* A nice function to read node coordinates, velocity and acceleration.
 * data_type is one of the D3PLT_PTR values*/
double *_d3plot_read_node_data(d3plot_file *plot_file, size_t state,
                               size_t *num_nodes, size_t data_type);
/* The same as _d3plot_read_node_data but it does not convert floats to
 * double. It does the opposite if the word size is 8*/
float *_d3plot_read_node_data_32(d3plot_file *plot_file, size_t state,
                                 size_t *num_nodes, size_t data_type);
/* A nice function to read node and element ids*/
d3_word *_d3plot_read_ids(d3plot_file *plot_file, size_t *num_ids,
                          size_t data_type, size_t num_ids_value);
/* Insert a sorted (ascending) array (src) into a sorted array (dst)*/
d3_word *_insert_sorted(d3_word *dst, size_t dst_size, const d3_word *src,
                        size_t src_size);
/* Deallocates all memory of a d3plot_part*/
void d3plot_free_part(d3plot_part *part);
/********************************/

/*********** Utility Functions ***************/
/* These functions do some more computations *
 * to reach their outputs, so be aware of    *
 * that if you use them                      */

/* Read all ids of the solid, beam, shell and solid shell elements. The return
 * value needs to be deallocated by free*/
d3_word *d3plot_read_all_element_ids(d3plot_file *plot_file, size_t *num_ids);
/* Returns the index of id in the array ids. If it cannot be found then
 * UINT64_MAX will be returned.*/
size_t d3plot_index_for_id(d3_word id, const d3_word *ids, size_t num_ids);
/*********************************************/

#ifdef __cplusplus
}
#endif

#include "d3plot_part_nodes.h"

#endif
