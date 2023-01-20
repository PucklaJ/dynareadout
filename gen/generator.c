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

#include "binary_search_gen.h"
#include "d3plot_part_nodes_gen.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

const char license_comment[] =
    "/*************************************************************************"
    "**********\n"
    " *                         This file is part of dynareadout\n"
    " *                    https://github.com/PucklaJ/dynareadout\n"
    " *************************************************************************"
    "**********\n"
    " * Copyright (c) 2022 Jonas Pucher\n"
    " *\n"
    " * This software is provided 'as-is', without any express or implied "
    "warranty.\n"
    " * In no event will the authors be held liable for any damages arising "
    "from the\n"
    " * use of this software.\n"
    " *\n"
    " * Permission is granted to anyone to use this software for any purpose,\n"
    " * including commercial applications, and to alter it and redistribute "
    "it\n"
    " * freely, subject to the following restrictions:\n"
    " *\n"
    " * 1. The origin of this software must not be misrepresented; you must "
    "not claim\n"
    " * that you wrote the original software. If you use this software in a "
    "product,\n"
    " * an acknowledgment in the product documentation would be appreciated "
    "but is\n"
    " * not required.\n"
    " *\n"
    " * 2. Altered source versions must be plainly marked as such, and must "
    "not be\n"
    " * misrepresented as being the original software.\n"
    " *\n"
    " * 3. This notice may not be removed or altered from any source "
    "distribution.\n"
    " *************************************************************************"
    "***********/\n\n";

void format(const char *filename) {
  char buffer[1024];
  sprintf(buffer, "clang-format -i %s", filename);
  system(buffer);
}

void semicolon(FILE *file) { fprintf(file, ";\n"); }
void newline(FILE *file) { fprintf(file, "\n"); }
void header_begin(FILE *file, const char *macro) {
  fprintf(file, "#ifndef %s\n#define %s\n", macro, macro);
}
void header_end(FILE *file) { fprintf(file, "#endif"); }
void license(FILE *file) { fprintf(file, license_comment); }
void include_rel(FILE *file, const char *filename) {
  fprintf(file, "#include \"%s\"\n", filename);
}
void include_abs(FILE *file, const char *filename) {
  fprintf(file, "#include <%s>\n", filename);
}
void cpp_start(FILE *file) {
  fprintf(file, "#ifdef __cplusplus\nextern \"C\" {\n#endif\n\n");
}
void cpp_end(FILE *file) { fprintf(file, "#ifdef __cplusplus\n}\n#endif\n\n"); }
void generated_notice(FILE *file) {
  fprintf(file, "/* This file has been automatically generated*/\n\n");
}
void ifdef_start(FILE *file, const char *macro) {
  fprintf(file, "#ifdef %s\n", macro);
}
void ifdef_end(FILE *file) { fprintf(file, "#endif\n"); }

int main(int args, char *argv[]) {
  FILE *binary_search_h = fopen("src/binary_search.h", "w");
  FILE *binary_search_c = fopen("src/binary_search.c", "w");
  FILE *pgni_h = fopen("src/pgni.h", "w");

  assert(binary_search_h != NULL);
  assert(binary_search_c != NULL);
  assert(pgni_h != NULL);

  license(binary_search_h);
  generated_notice(binary_search_h);
  header_begin(binary_search_h, "BINARY_SEARCH_H");
  include_rel(binary_search_h, "binout_directory.h");
  include_rel(binary_search_h, "d3_defines.h");
  include_abs(binary_search_h, "stddef.h");
  newline(binary_search_h);
  cpp_start(binary_search_h);
  binary_search_prototype(binary_search_h,
                          "binout_directory_binary_search_folder",
                          "const binout_folder_t *", "const path_view_t *");
  semicolon(binary_search_h);
  newline(binary_search_h);
  binary_search_insert_prototype(
      binary_search_h, "binout_directory_binary_search_folder_insert",
      "const binout_folder_t *", "const path_view_t *");
  semicolon(binary_search_h);
  newline(binary_search_h);
  binary_search_prototype(binary_search_h,
                          "binout_directory_binary_search_file",
                          "const binout_file_t *", "const path_view_t *");
  semicolon(binary_search_h);
  newline(binary_search_h);
  binary_search_insert_prototype(binary_search_h,
                                 "binout_directory_binary_search_file_insert",
                                 "const binout_file_t *", "const char *");
  semicolon(binary_search_h);
  newline(binary_search_h);
  binary_search_prototype(binary_search_h, "d3_word_binary_search",
                          "const d3_word *", "d3_word");
  semicolon(binary_search_h);
  newline(binary_search_h);
  binary_search_insert_prototype(binary_search_h,
                                 "d3_word_binary_search_insert",
                                 "const d3_word *", "d3_word");
  semicolon(binary_search_h);
  newline(binary_search_h);
  newline(binary_search_h);
  ifdef_start(binary_search_h, "PROFILING");
  binary_search_insert_prototype(binary_search_h, "string_binary_search_insert",
                                 "char const **", "const char *");
  semicolon(binary_search_h);
  ifdef_end(binary_search_h);
  newline(binary_search_h);
  cpp_end(binary_search_h);
  header_end(binary_search_h);

  license(binary_search_c);
  generated_notice(binary_search_c);
  include_rel(binary_search_c, "binary_search.h");
  include_rel(binary_search_c, "profiling.h");
  include_abs(binary_search_c, "string.h");
  newline(binary_search_c);
  binary_search_cmp(binary_search_c, "d3_word");
  newline(binary_search_c);

  binary_search_prototype(binary_search_c,
                          "binout_directory_binary_search_folder",
                          "const binout_folder_t *", "const path_view_t *");
  newline(binary_search_c);
  binary_search_body(binary_search_c, "binout_directory_binary_search_folder",
                     "path_view_strcmp", ".name");
  newline(binary_search_c);
  binary_search_insert_prototype(
      binary_search_c, "binout_directory_binary_search_folder_insert",
      "const binout_folder_t *", "const path_view_t *");
  newline(binary_search_c);
  binary_search_insert_body(binary_search_c,
                            "binout_directory_binary_search_folder_insert",
                            "path_view_strcmp", ".name");
  newline(binary_search_c);
  binary_search_prototype(binary_search_c,
                          "binout_directory_binary_search_file",
                          "const binout_file_t *", "const path_view_t *");
  newline(binary_search_c);
  binary_search_body(binary_search_c, "binout_directory_binary_search_file",
                     "path_view_strcmp", ".name");
  newline(binary_search_c);
  binary_search_insert_prototype(binary_search_c,
                                 "binout_directory_binary_search_file_insert",
                                 "const binout_file_t *", "const char *");
  newline(binary_search_c);
  binary_search_insert_body(binary_search_c,
                            "binout_directory_binary_search_file_insert",
                            "strcmp", ".name");
  newline(binary_search_c);
  binary_search_prototype(binary_search_c, "d3_word_binary_search",
                          "const d3_word *", "d3_word");
  newline(binary_search_c);
  binary_search_body(binary_search_c, "d3_word_binary_search", "d3_word_cmp",
                     "");
  newline(binary_search_c);
  binary_search_insert_prototype(binary_search_c,
                                 "d3_word_binary_search_insert",
                                 "const d3_word *", "d3_word");
  newline(binary_search_c);
  binary_search_insert_body(binary_search_c, "d3_word_binary_search_insert",
                            "d3_word_cmp", "");
  newline(binary_search_c);
  ifdef_start(binary_search_c, "PROFILING");
  binary_search_insert_prototype(binary_search_c, "string_binary_search_insert",
                                 "char const **", "const char*");
  newline(binary_search_c);
  binary_search_insert_body(binary_search_c, "string_binary_search_insert",
                            "strcmp", "");
  ifdef_end(binary_search_c);

  license(pgni_h);
  generated_notice(pgni_h);
  newline(pgni_h);
  header_begin(pgni_h, "PGNI_H");
  include_rel(pgni_h, "d3plot.h");
  include_rel(pgni_h, "d3plot_error_macros.h");
  include_rel(pgni_h, "profiling.h");
  include_abs(pgni_h, "stdlib.h");
  include_abs(pgni_h, "string.h");
  newline(pgni_h);
  pgni_load_function_and_macro(pgni_h);
  newline(pgni_h);
  pgni_add_element_function(pgni_h, "solids", "solid_ids", "solid_cons",
                            "num_solids", "d3plot_read_solid_element_ids",
                            "d3plot_read_solid_elements", "d3plot_solid_con");
  newline(pgni_h);
  pgni_add_element_function(pgni_h, "beams", "beam_ids", "beam_cons",
                            "num_beams", "d3plot_read_beam_element_ids",
                            "d3plot_read_beam_elements", "d3plot_beam_con");
  newline(pgni_h);
  pgni_add_element_function(pgni_h, "shells", "shell_ids", "shell_cons",
                            "num_shells", "d3plot_read_shell_element_ids",
                            "d3plot_read_shell_elements", "d3plot_shell_con");
  newline(pgni_h);
  pgni_add_element_function(
      pgni_h, "thick_shells", "thick_shell_ids", "thick_shell_cons",
      "num_thick_shells", "d3plot_read_thick_shell_element_ids",
      "d3plot_read_thick_shell_elements", "d3plot_thick_shell_con");
  newline(pgni_h);
  pgnind_add_element_function(pgni_h, "solids", "solid_ids", "solid_cons",
                              "num_solids", "d3plot_read_solid_element_ids",
                              "d3plot_read_solid_elements", "d3plot_solid_con");
  newline(pgni_h);
  pgnind_add_element_function(pgni_h, "beams", "beam_ids", "beam_cons",
                              "num_beams", "d3plot_read_beam_element_ids",
                              "d3plot_read_beam_elements", "d3plot_beam_con");
  newline(pgni_h);
  pgnind_add_element_function(pgni_h, "shells", "shell_ids", "shell_cons",
                              "num_shells", "d3plot_read_shell_element_ids",
                              "d3plot_read_shell_elements", "d3plot_shell_con");
  newline(pgni_h);
  pgnind_add_element_function(
      pgni_h, "thick_shells", "thick_shell_ids", "thick_shell_cons",
      "num_thick_shells", "d3plot_read_thick_shell_element_ids",
      "d3plot_read_thick_shell_elements", "d3plot_thick_shell_con");
  newline(pgni_h);
  pgni_add_element_macro(pgni_h, "SOLIDS", "solids");
  pgni_add_element_macro(pgni_h, "BEAMS", "beams");
  pgni_add_element_macro(pgni_h, "SHELLS", "shells");
  pgni_add_element_macro(pgni_h, "THICK_SHELLS", "thick_shells");
  newline(pgni_h);
  pgnind_add_element_macro(pgni_h, "SOLIDS", "solids");
  pgnind_add_element_macro(pgni_h, "BEAMS", "beams");
  pgnind_add_element_macro(pgni_h, "SHELLS", "shells");
  pgnind_add_element_macro(pgni_h, "THICK_SHELLS", "thick_shells");
  newline(pgni_h);
  header_end(pgni_h);

  fclose(binary_search_h);
  fclose(binary_search_c);
  fclose(pgni_h);

  format("src/binary_search.h");
  format("src/binary_search.c");
  format("src/pgni.h");

  return 0;
}