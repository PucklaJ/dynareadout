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

#ifndef D3PLOT_H
#define D3PLOT_H
#include "d3_buffer.h"
#include "d3_defines.h"

#define D3PLT_PTR_TITLE 0
#define D3PLT_PTR_NODE_COORDS (D3PLT_PTR_TITLE + 1)
#define D3PLT_PTR_NODE_IDS (D3PLT_PTR_NODE_COORDS + 1)
#define D3PLT_PTR_EL8_IDS (D3PLT_PTR_NODE_IDS + 1)
#define D3PLT_PTR_EL2_IDS (D3PLT_PTR_EL8_IDS + 1)
#define D3PLT_PTR_EL4_IDS (D3PLT_PTR_EL2_IDS + 1)
#define D3PLT_PTR_EL48_IDS (D3PLT_PTR_EL4_IDS + 1)
#define D3PLT_PTR_EL8_CONNECT (D3PLT_PTR_EL48_IDS + 1)
#define D3PLT_PTR_ELT_CONNECT (D3PLT_PTR_EL8_CONNECT + 1)
#define D3PLT_PTR_EL2_CONNECT (D3PLT_PTR_ELT_CONNECT + 1)
#define D3PLT_PTR_EL4_CONNECT (D3PLT_PTR_EL2_CONNECT + 1)
#define D3PLT_PTR_STATE_TIME (D3PLT_PTR_EL4_CONNECT + 1)
#define D3PLT_PTR_STATE_NODE_COORDS (D3PLT_PTR_STATE_TIME + 1)
#define D3PLT_PTR_STATE_NODE_VEL (D3PLT_PTR_STATE_NODE_COORDS + 1)
#define D3PLT_PTR_STATE_NODE_ACC (D3PLT_PTR_STATE_NODE_VEL + 1)
#define D3PLT_PTR_STATES (D3PLT_PTR_STATE_NODE_ACC + 1)
#define D3PLT_PTR_COUNT D3PLT_PTR_STATES

/* This holds all data needed to read d3plot files*/
typedef struct {
  struct {
    /* These are all the values inside the CONTROL DATA section of the first
     * d3plot file (root file)*/
    d3_word run_time, source_version, release_version, version, ndim, numnp,
        icode, nglbv, it, iu, iv, ia, nummat8, numds, numst, nv3d, nel2,
        nummat2, nv1d, nel4, nummat4, nv2d, neiph, neips, edlopt, nmsph, ngpsph,
        narbs, nelt, nummatt, nv3dt, ioshl[4], ialemat, ncfdv1, ncfdv2, nadapt,
        nmmat, numfluid, inn, npefg, nel48, idtdt, extra, words[6], nel20, nt3d;
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
/* Read all ids of the solid shell elements. The return value needs to be
 * deallocated by free*/
d3_word *d3plot_read_solid_shell_element_ids(d3plot_file *plot_file,
                                             size_t *num_ids);
/* Read all ids of the solid, beam, shell and solid shell elements. The return
 * value needs to be deallocated by free*/
d3_word *d3plot_read_all_element_ids(d3plot_file *plot_file, size_t *num_ids);
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
/* Read the time of a given state (time step) in milliseconds*/
double d3plot_read_time(d3plot_file *plot_file, size_t state);
/* Returns the node connectivity + material number of all 8 node solid
 * elements. The return value needs to be deallocated by free*/
d3plot_solid *d3plot_read_solid_elements(d3plot_file *plot_file,
                                         size_t *num_solids);
/* Returns the node connectivity + material number of all 8 node thick shell
 * elements. The return value needs to be deallocated by free*/
d3plot_thick_shell *d3plot_read_thick_shell_elements(d3plot_file *plot_file,
                                                     size_t *num_thick_shells);
/* Returns the node connectivity + orientation node + material number of all
 * beam elements. The return value needs to be deallocated by free*/
d3plot_beam *d3plot_read_beam_elements(d3plot_file *plot_file,
                                       size_t *num_beams);
/* Returns the node connectivity +  material number of all
 * shell elements. The return value needs to be deallocated by free*/
d3plot_shell *d3plot_read_shell_elements(d3plot_file *plot_file,
                                         size_t *num_shells);
/* Returns a null terminated string holding the Title of the d3plot file. The
 * return value needs to be deallocated by free.*/
char *d3plot_read_title(d3plot_file *plot_file);

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
/* A nice function to read node and element ids*/
d3_word *_d3plot_read_ids(d3plot_file *plot_file, size_t *num_ids,
                          size_t data_type, size_t num_ids_value);
/* Insert a sorted (ascending) array (src) into a sorted array (dst)*/
d3_word *_insert_sorted(d3_word *dst, size_t dst_size, const d3_word *src,
                        size_t src_size);
/********************************/

#ifdef __cplusplus
}
#endif

#endif