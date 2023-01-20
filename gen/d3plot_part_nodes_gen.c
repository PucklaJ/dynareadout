#include "d3plot_part_nodes_gen.h"

void pgni_load_function_and_macro(FILE *file) {
  fprintf(file,
          "int _PGNI_LOAD(void ***member, size_t **num,\n"
          "void *(func)(d3plot_file *, size_t *), void "
          "**pointer_buffer,\n"
          "size_t *current_pointer, size_t *size_buffer,\n"
          "size_t *current_size, d3plot_file *plot_file) {\n"
          "if (!*member) {\n"
          "*member = &pointer_buffer[(*current_pointer)++];\n"
          "**member = NULL;\n"
          "}\n"
          "if (!**member) {\n"
          "if (!*num) {\n"
          "*num = &size_buffer[(*current_size)++];\n"
          "}\n"
          "**member = func(plot_file, *num);\n"
          "if (plot_file->error_string) {\n"
          "return 0;\n"
          "}\n"
          "}\n"
          "\n"
          "return 1;\n"
          "}\n"
          "\n"
          "#define PGNI_LOAD(member, num, func)"
          "\\\n"
          "_PGNI_LOAD((void ***)&p->member, &p->num, func, pointer_buffer,"
          "\\\n"
          "current_pointer, size_buffer, current_size, plot_file)\n"
          "\n");
}

void pgni_unload_macro(FILE *file) {
  fprintf(file, "#define PGNI_UNLOAD(member)\\\n"
                "if (!params || !params->member)\\\n"
                "free(*p->member);\n");
}

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

char *pgni_unload(const char *member) {
  char *buffer = malloc(1024);
#ifdef _WIN32
  _sprintf_p(
#else
  sprintf(
#endif
      buffer,
      "/*unload %1$s*/\nif (!params || !params->%1$s) free(*p->%1$s);\n",
      member);
  return buffer;
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
      "part, d3_word* part_node_ids, size_t* num_part_node_ids, "
      "d3plot_part_get_node_ids_params* p, "
      "d3plot_part_get_node_ids_params* params, void** pointer_buffer, size_t* "
      "current_pointer, size_t* size_buffer, size_t* current_size) {\n"
      "if (part->%4$s != 0) {"
      "if (!PGNI_LOAD(%2$s, %4$s, %5$s)) {\n"
      "/* Ignore these elements*/\n"
      "CLEAR_ERROR_STRING();"
      "} else {"
      "if (!PGNI_LOAD(%3$s, %4$s, %6$s)) {\n"
      "/* Ignore these elements*/\n"
      "%10$s"
      " CLEAR_ERROR_STRING();"
      "} else {"
      "size_t i = 0;"
      "while (i < part->%4$s) {"
      "const size_t el_index ="
      "d3plot_index_for_id(part->%2$s[i], *p->%2$s, *p->%4$s);\n"
      "const %7$s *el_con = &(*p->%3$s)[el_index];\n"
      "size_t j = 0;"
      "while (j < (sizeof(el_con->node_indices) /"
      "sizeof(*el_con->node_indices))) {"
      "const d3_word node_index = el_con->node_indices[j];"
      "const d3_word node_id = (*p->node_ids)[node_index];\n"
      "/* Push node id onto the array of node ids*/\n"
      "%8$s"
      "/******************************************/\n"
      "j++;"
      "}"
      "i++;"
      "}\n"
      "%9$s"
      "%10$s"
      "}"
      "}"
      "}"
      "}\n",
      element_name, el_ids, el_cons, num_els, ids_func, cons_func, con_type,
      pgni_push_snippet(), pgni_unload(el_cons), pgni_unload(el_ids));
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
      "part, d3_word* part_node_indices, size_t* num_part_node_indices, "
      "d3plot_part_get_node_ids_params* p, "
      "d3plot_part_get_node_ids_params* params, void** pointer_buffer, size_t* "
      "current_pointer, size_t* size_buffer, size_t* current_size) {"
      "if (part->%4$s != 0) {"
      " if (!PGNI_LOAD(%2$s, %4$s,"
      "%5$s)) {\n"
      "/* Ignore these elements*/\n"
      "CLEAR_ERROR_STRING();"
      "} else {"
      "if (!PGNI_LOAD(%3$s, %4$s,"
      "%6$s)) {\n"
      "/* Ignore these elements*/\n"
      "%10$s"
      "CLEAR_ERROR_STRING();"
      "} else {"
      "size_t i = 0;"
      "while (i < part->%4$s) {"
      "const size_t el_index = d3plot_index_for_id("
      "part->%2$s[i], *p->%2$s, *p->%4$s);"
      "const %7$s *el_con = &(*p->%3$s)[el_index];"
      "size_t j = 0;"
      "while (j < (sizeof(el_con->node_indices) /"
      "sizeof(*el_con->node_indices))) {"
      "const d3_word node_index = el_con->node_indices[j];\n"
      "/* Push node index onto the array of node indices*/\n"
      "%8$s"
      "/*********************************************/\n"
      "j++;"
      "}"
      "i++;"
      "}\n"
      "%9$s"
      "%10$s"
      "}"
      "}"
      "}"
      "}\n",
      element_name, el_ids, el_cons, num_els, ids_func, cons_func, con_type,
      pgnind_push_snippet(), pgni_unload(el_cons), pgni_unload(el_ids));
}

void pgni_add_element_macro(FILE *file, const char *element_name,
                            const char *element_lower) {
  fprintf(file,
          "#define PGNI_ADD_%s() pgni_add_element_%s(plot_file, part, "
          "part_node_ids, num_part_node_ids, p, params, pointer_buffer, "
          "&current_pointer, size_buffer, &current_size);\n",
          element_name, element_lower);
}

void pgnind_add_element_macro(FILE *file, const char *element_name,
                              const char *element_lower) {
  fprintf(
      file,
      "#define PGNIND_ADD_%s() pgnind_add_element_%s(plot_file, part, "
      "part_node_indices, num_part_node_indices, p, params, pointer_buffer, "
      "&current_pointer, size_buffer, &current_size);\n",
      element_name, element_lower);
}