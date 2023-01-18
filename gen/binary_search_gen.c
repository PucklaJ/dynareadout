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

void binary_search_cmp(FILE *file, const char *type_name) {
  fprintf(file,
          "int %s_cmp (const %s a, const %s b) { return a > b ? 1 : (a < b ? "
          "-1 : 0); }\n",
          type_name, type_name, type_name);
}

void binary_search_prototype(FILE *file, const char *func_name,
                             const char *array_type, const char *search_type) {
  fprintf(file,
          "size_t %s(%s arr, size_t start_index, size_t end_index, %s value)",
          func_name, array_type, search_type);
}

void binary_search_body(FILE *file, const char *func_name, const char *cmp_func,
                        const char *cmp_suf) {
  fprintf(
      file,
      "{\n"
      "BEGIN_PROFILE_FUNC();\n"
      "\n"
      "if (start_index == end_index) {\n"
      "if (%s(value, arr[start_index]%s) == 0) {\n"
      "END_PROFILE_FUNC();\n"
      "return start_index;\n"
      "}\n"
      "\n"
      "END_PROFILE_FUNC();\n"
      "return ~0;\n"
      "}\n"
      "\n"
      "const size_t half_index = start_index + (end_index - start_index) / 2;\n"
      "const int cmp_val = %s(value, arr[half_index]%s);\n"
      "\n"
      "if (cmp_val < 0) {\n"
      "const size_t index = %s(arr, start_index, half_index, value);\n"
      "END_PROFILE_FUNC();\n"
      "return index;\n"
      "} else if (cmp_val > 0) {\n"
      "if (half_index == end_index - 1) {\n"
      "const size_t index = %s(arr, end_index, end_index, value);\n"
      "END_PROFILE_FUNC();\n"
      "return index;\n"
      "}\n"
      "const size_t index = %s(arr, half_index, end_index, value);\n"
      "END_PROFILE_FUNC();\n"
      "return index;\n"
      "}\n"
      "\n"
      "END_PROFILE_FUNC();\n"
      "return half_index;\n"
      "}\n",
      cmp_func, cmp_suf, cmp_func, cmp_suf, func_name, func_name, func_name);
}

void binary_search_insert_prototype(FILE *file, const char *func_name,
                                    const char *array_type,
                                    const char *search_type) {
  fprintf(file,
          "size_t %s(%s arr, size_t start_index, size_t end_index, %s value, "
          "int *found)",
          func_name, array_type, search_type);
}

void binary_search_insert_body(FILE *file, const char *func_name,
                               const char *cmp_func, const char *cmp_suf) {
  fprintf(
      file,
      "{\n"
      "BEGIN_PROFILE_FUNC();\n"
      "\n"
      "if (start_index == end_index) {\n"
      "const int cmp_value = %s(value, arr[start_index]%s);\n"
      "\n"
      "if (cmp_value == 0) {\n"
      "*found = 1;\n"
      "END_PROFILE_FUNC();\n"
      "return start_index;\n"
      "}\n"
      "if (cmp_value > 0) {\n"
      "*found = 0;\n"
      "END_PROFILE_FUNC();\n"
      "return start_index + 1;\n"
      "}\n"
      "\n"
      "*found = 0;\n"
      "END_PROFILE_FUNC();\n"
      "return start_index;\n"
      "}\n"
      "\n"
      "const size_t half_index = start_index + (end_index - start_index) / 2;\n"
      "const int cmp_val = %s(value, arr[half_index]%s);\n"
      "\n"
      "if (cmp_val < 0) {\n"
      "const size_t index = %s(arr, start_index, half_index, value, found);\n"
      "END_PROFILE_FUNC();\n"
      "return index;\n"
      "} else if (cmp_val > 0) {\n"
      "if (half_index == end_index - 1) {\n"
      "const size_t index = %s(arr, end_index, end_index, value, found);\n"
      "END_PROFILE_FUNC();\n"
      "return index;\n"
      "}\n"
      "const size_t index = %s(arr, half_index, end_index, value, found);\n"
      "END_PROFILE_FUNC();\n"
      "return index;\n"
      "}\n"
      "\n"
      "*found = 1;\n"
      "END_PROFILE_FUNC();\n"
      "return half_index;\n"
      "}\n",
      cmp_func, cmp_suf, cmp_func, cmp_suf, func_name, func_name, func_name);
}