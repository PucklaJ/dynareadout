#include "d3plot_part_nodes_gen.h"
#include <stdlib.h>

const char *pgni_push_snippet() {
  return "if (*num_part_node_ids == 0) {"
         "*num_part_node_ids = 1;"
         "part_node_ids[0] = node_id;"
         "} else {"
         "int found;"
         "const size_t insert_index = d3_word_binary_search_insert("
         "part_node_ids, 0, *num_part_node_ids - 1, node_id, &found);"
         "if (!found) {"
         "(*num_part_node_ids)++;\n"
         "/* Move everything to the right*/\n"
         "size_t i = *num_part_node_ids - 1;"
         "while (i > insert_index) {"
         "part_node_ids[i] = part_node_ids[i - 1];"
         "i--;"
         "}"
         "part_node_ids[insert_index] = node_id;"
         "}"
         "}\n";
}

const char *pgnind_push_snippet() {
  return "if (*num_part_node_indices == 0) {"
         "*num_part_node_indices = 1;"
         "part_node_indices[0] = node_index;"
         "} else {"
         "int found;"
         "const size_t insert_index = d3_word_binary_search_insert("
         "part_node_indices, 0, *num_part_node_indices - 1, node_index, "
         "&found);"
         "if (!found) {"
         "(*num_part_node_indices)++;\n"
         "/* Move everything to the right*/\n"
         "size_t i = *num_part_node_indices - 1;"
         "while (i > insert_index) {"
         "part_node_indices[i] = part_node_indices[i - 1];"
         "i--;"
         "}"
         "part_node_indices[insert_index] = node_index;"
         "}"
         "}\n";
}

void pgni_add_element_function(FILE *file, const char *element_name /*1*/,
                               const char *el_ids /*2*/,
                               const char *el_cons /*3*/,
                               const char *num_els /*4*/,
                               const char *ids_func /*5*/,
                               const char *cons_func /*6*/,
                               const char *con_type /*7*/) {
#ifdef _WIN32
  _fprintf_p(
#else
  fprintf(
#endif
      file,
      "void pgni_add_element_%1$s(d3plot_file* plot_file, const d3plot_part* "
      "part, d3_word* part_node_ids, size_t* num_part_node_ids, const d3_word* "
      "node_ids, const d3_word* "
      "%2$s, size_t %4$s, const %7$s* %3$s) {\n"
      "if (part->%4$s != 0) {\n"
      "uint8_t loaded_ids = 0;\n"
      "uint8_t loaded_cons = 0;\n"
      "if (!%2$s) {\n"
      "loaded_ids = 1;\n"
      "%2$s = %5$s(plot_file, &%4$s);\n"
      "if (plot_file->error_string) {\n"
      "/*Ignore these elements*/\n"
      "CLEAR_ERROR_STRING();\n"
      "}\n"
      "}\n"
      "if (%2$s) {\n"
      "if (!%3$s) {\n"
      "%3$s = %6$s(plot_file, &%4$s);\n"
      "if (plot_file->error_string) {\n"
      "/* Ignore these elements */\n"
      "/* Unload solid_ids*/\n"
      "if (loaded_ids) {\n"
      "free((d3_word*)%2$s);\n"
      "}\n"
      "CLEAR_ERROR_STRING();\n"
      "}\n"
      "}\n"
      "if (%3$s) {\n"
      "size_t i = 0;"
      "while (i < part->%4$s) {"
      "const size_t el_index ="
      "d3plot_index_for_id(part->%2$s[i], %2$s, %4$s);\n"
      "const %7$s *el_con = &%3$s[el_index];\n"
      "size_t j = 0;"
      "while (j < (sizeof(el_con->node_indices) /"
      "sizeof(*el_con->node_indices))) {"
      "const d3_word node_index = el_con->node_indices[j];"
      "const d3_word node_id = node_ids[node_index];\n"
      "/* Push node id onto the array of node ids*/\n"
      "%8$s"
      "/******************************************/\n"
      "j++;"
      "}"
      "i++;"
      "}\n"
      "if (loaded_ids) {\n"
      "free((d3_word*)%2$s);\n"
      "}\n"
      "if (loaded_cons) {\n"
      "free((%7$s*)%3$s);\n"
      "}\n"
      "}"
      "}"
      "}"
      "}\n",
      element_name, el_ids, el_cons, num_els, ids_func, cons_func, con_type,
      pgni_push_snippet());
}

void pgnind_add_element_function(FILE *file, const char *element_name /*1*/,
                                 const char *el_ids /*2*/,
                                 const char *el_cons /*3*/,
                                 const char *num_els /*4*/,
                                 const char *ids_func /*5*/,
                                 const char *cons_func /*6*/,
                                 const char *con_type /*7*/) {
#ifdef _WIN32
  _fprintf_p(
#else
  fprintf(
#endif
      file,
      "void pgnind_add_element_%1$s(d3plot_file* plot_file, const d3plot_part* "
      "part, d3_word* part_node_indices, size_t* num_part_node_indices, const "
      "d3_word* "
      "%2$s, size_t %4$s, const %7$s* %3$s) {\n"
      "if (part->%4$s != 0) {\n"
      "uint8_t loaded_ids = 0;\n"
      "uint8_t loaded_cons = 0;\n"
      "if (!%2$s) {\n"
      "loaded_ids = 1;\n"
      "%2$s = %5$s(plot_file, &%4$s);\n"
      "if (plot_file->error_string) {\n"
      "/*Ignore these elements*/\n"
      "CLEAR_ERROR_STRING();\n"
      "}\n"
      "}\n"
      "if (%2$s) {\n"
      "if (!%3$s) {\n"
      "%3$s = %6$s(plot_file, &%4$s);\n"
      "if (plot_file->error_string) {\n"
      "/* Ignore these elements */\n"
      "/* Unload solid_ids*/\n"
      "if (loaded_ids) {\n"
      "free((d3_word*)%2$s);\n"
      "}\n"
      "CLEAR_ERROR_STRING();\n"
      "}\n"
      "}\n"
      "if (%3$s) {\n"
      "size_t i = 0;"
      "while (i < part->%4$s) {"
      "const size_t el_index ="
      "d3plot_index_for_id(part->%2$s[i], %2$s, %4$s);\n"
      "const %7$s *el_con = &%3$s[el_index];\n"
      "size_t j = 0;"
      "while (j < (sizeof(el_con->node_indices) /"
      "sizeof(*el_con->node_indices))) {"
      "const d3_word node_index = el_con->node_indices[j];\n"
      "/* Push node index onto the array of node indices*/\n"
      "%8$s"
      "/******************************************/\n"
      "j++;"
      "}"
      "i++;"
      "}\n"
      "if (loaded_ids) {\n"
      "free((d3_word*)%2$s);\n"
      "}\n"
      "if (loaded_cons) {\n"
      "free((%7$s*)%3$s);\n"
      "}\n"
      "}"
      "}"
      "}"
      "}\n",
      element_name, el_ids, el_cons, num_els, ids_func, cons_func, con_type,
      pgnind_push_snippet());
}

void pgni_add_element_macro(FILE *file, const char *element_name,
                            const char *element_lower, const char *el_ids,
                            const char *el_cons, const char *num_els) {
  fprintf(file,
          "#define PGNI_ADD_%s() pgni_add_element_%s(plot_file, part, "
          "part_node_ids, num_part_node_ids, node_ids, %s, %s, %s);\n",
          element_name, element_lower, el_ids, num_els, el_cons);
}

void pgnind_add_element_macro(FILE *file, const char *element_name,
                              const char *element_lower, const char *el_ids,
                              const char *el_cons, const char *num_els) {
  fprintf(file,
          "#define PGNIND_ADD_%s() pgnind_add_element_%s(plot_file, part, "
          "part_node_indices, num_part_node_indices, %s, %s, %s);\n",
          element_name, element_lower, el_ids, num_els, el_cons);
}