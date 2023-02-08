#ifndef D3PLOT_PART_NODES_GEN_H
#define D3PLOT_PART_NODES_GEN_H

#include <stdio.h>

void pgni_load_function_and_macro(FILE *file);

void pgni_unload_macro(FILE *file);

void pgni_add_element_function(FILE *file, const char *element_name,
                               const char *el_ids, const char *el_cons,
                               const char *num_els, const char *ids_func,
                               const char *cons_func, const char *con_type);

void pgnind_add_element_function(FILE *file, const char *element_name,
                                 const char *el_ids, const char *el_cons,
                                 const char *num_els, const char *ids_func,
                                 const char *cons_func, const char *con_type);

void pgni_add_element_macro(FILE *file, const char *element_name,
                            const char *element_lower, const char *el_ids,
                            const char *el_cons, const char *num_els);

void pgnind_add_element_macro(FILE *file, const char *element_name,
                              const char *element_lower, const char *el_ids,
                              const char *el_cons, const char *num_els);

#endif