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

#ifndef BINOUT_READ_H
#define BINOUT_READ_H

#include <stddef.h>
#include <stdint.h>

#ifndef BINOUT_H
struct binout_file;
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Read data from the file as int8_t. The type id of the data has to match. The
 * return value needs to be deallocated by free.*/
int8_t *binout_read_i8(binout_file *bin_file, const char *path_to_variable,
                       size_t *data_size);
/* Read data from the file as int16_t. The type id of the data has to match. The
 * return value needs to be deallocated by free.*/
int16_t *binout_read_i16(binout_file *bin_file, const char *path_to_variable,
                         size_t *data_size);
/* Read data from the file as int32_t. The type id of the data has to match. The
 * return value needs to be deallocated by free.*/
int32_t *binout_read_i32(binout_file *bin_file, const char *path_to_variable,
                         size_t *data_size);
/* Read data from the file as int64_t. The type id of the data has to match. The
 * return value needs to be deallocated by free.*/
int64_t *binout_read_i64(binout_file *bin_file, const char *path_to_variable,
                         size_t *data_size);
/* Read data from the file as uint8_t. The type id of the data has to match. The
 * return value needs to be deallocated by free.*/
uint8_t *binout_read_u8(binout_file *bin_file, const char *path_to_variable,
                        size_t *data_size);
/* Read data from the file as uint16_t. The type id of the data has to match.
 * The return value needs to be deallocated by free.*/
uint16_t *binout_read_u16(binout_file *bin_file, const char *path_to_variable,
                          size_t *data_size);
/* Read data from the file as uint32_t. The type id of the data has to match.
 * The return value needs to be deallocated by free.*/
uint32_t *binout_read_u32(binout_file *bin_file, const char *path_to_variable,
                          size_t *data_size);
/* Read data from the file as uint64_t. The type id of the data has to match.
 * The return value needs to be deallocated by free.*/
uint64_t *binout_read_u64(binout_file *bin_file, const char *path_to_variable,
                          size_t *data_size);
/* Read data from the file as float. The type id of the data has to match. The
 * return value needs to be deallocated by free.*/
float *binout_read_f32(binout_file *bin_file, const char *path_to_variable,
                       size_t *data_size);
/* Read data from the file as double. The type id of the data has to match. The
 * return value needs to be deallocated by free.*/
double *binout_read_f64(binout_file *bin_file, const char *path_to_variable,
                        size_t *data_size);

/* Read a variable under the dxxxxxx folders. This functions reads all timesteps
 * of a variable at once. If the actual variable is something like
 * "/nodout/d000000/x_displacement" then variable has to be
 * "/nodout/x_displacement". The shape of the array is num_timesteps *
 * num_values which means to get a value you need to index like [timestep *
 * num_values + value]*/
int8_t *binout_read_timed_i8(binout_file *bin_file, const char *variable,
                             size_t *num_values, size_t *num_timesteps);
int16_t *binout_read_timed_i16(binout_file *bin_file, const char *variable,
                               size_t *num_values, size_t *num_timesteps);
int32_t *binout_read_timed_i32(binout_file *bin_file, const char *variable,
                               size_t *num_values, size_t *num_timesteps);
int64_t *binout_read_timed_i64(binout_file *bin_file, const char *variable,
                               size_t *num_values, size_t *num_timesteps);
uint8_t *binout_read_timed_u8(binout_file *bin_file, const char *variable,
                              size_t *num_values, size_t *num_timesteps);
uint16_t *binout_read_timed_u16(binout_file *bin_file, const char *variable,
                                size_t *num_values, size_t *num_timesteps);
uint32_t *binout_read_timed_u32(binout_file *bin_file, const char *variable,
                                size_t *num_values, size_t *num_timesteps);
uint64_t *binout_read_timed_u64(binout_file *bin_file, const char *variable,
                                size_t *num_values, size_t *num_timesteps);
float *binout_read_timed_f32(binout_file *bin_file, const char *variable,
                             size_t *num_values, size_t *num_timesteps);
double *binout_read_timed_f64(binout_file *bin_file, const char *variable,
                              size_t *num_values, size_t *num_timesteps);

#ifdef __cplusplus
}
#endif

#endif