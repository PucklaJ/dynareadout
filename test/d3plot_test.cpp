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

#define DOCTEST_CONFIG_TREAT_CHAR_STAR_AS_STRING
#include <binary_search.h>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <ctime>
#include <d3plot.h>
#include <doctest/doctest.h>
#ifdef D3PLOT_CPP
#include "main_test.hpp"
#include <d3plot.hpp>
#endif

#define CHECK_APPROX(lhs, rhs) CHECK_LT(fabs(lhs - rhs), 10e-6)

TEST_CASE("d3_buffer") {

  d3_buffer buffer = d3_buffer_open("test_data/d3plot");
  if (buffer.error_string) {
    FAIL(buffer.error_string);
    d3_buffer_close(&buffer);
    return;
  }

  CHECK((buffer.word_size == 4));
  CHECK(buffer.num_file_handles == 28);

  char title[10 * 4 + 1];
  title[10 * 4] = '\0';
  d3_buffer_read_words(&buffer, title, 10);

  CHECK(title == "Pouch_macro_37Ah                        ");

  uint8_t *probe = new uint8_t[40 * 1000 * 1000 * 4];
  d3_buffer_read_words_at(&buffer, probe, 40 * 1000 * 1000, 0);

  // d3plot
  CHECK(probe[0x00000000] == 0x50);
  CHECK(probe[0x0057EFFF] == 0x00);
  // d3plot01
  CHECK(probe[0x0057F000] == 0x00);
  CHECK(probe[0x0057F008] == 0x08);
  CHECK(probe[0x048A8FFF] == 0x00);
  // d3plot02
  CHECK(probe[0x048A9000] == 0x94);
  CHECK(probe[0x048A9008] == 0x00);
  CHECK(probe[0x08BD2FFF] == 0x00);

  delete[] probe;

  d3_buffer_close(&buffer);
}

TEST_CASE("d3plot") {
  d3plot_file plot_file = d3plot_open("test_data/d3plot");
  if (plot_file.error_string) {
    FAIL(plot_file.error_string);
    d3plot_close(&plot_file);
    return;
  }

  char *title = d3plot_read_title(&plot_file);
  CHECK(title == "Pouch_macro_37Ah                        ");
  free(title);

  struct tm *run_time = d3plot_read_run_time(&plot_file);
  REQUIRE(run_time != NULL);
  CHECK(run_time->tm_wday == 3);
  CHECK(run_time->tm_mday == 10);
  CHECK(run_time->tm_mon == 10);
  CHECK(run_time->tm_year == 121);
  CHECK(run_time->tm_hour == 12);
  CHECK(run_time->tm_min == 1);
  CHECK(run_time->tm_sec == 42);

  REQUIRE(plot_file.num_states == 102);

  size_t num_nodes;
  d3_word *node_ids = d3plot_read_node_ids(&plot_file, &num_nodes);

  REQUIRE(num_nodes == 114893);
  CHECK(node_ids[59530] == 84285019);
  CHECK(node_ids[0] == 10);
  CHECK(node_ids[114892] == 84340381);
  CHECK(node_ids[2458] == 2852);

  free(node_ids);

  size_t num_elements;
  d3_word *element_ids = d3plot_read_all_element_ids(&plot_file, &num_elements);

  REQUIRE(num_elements == 133456);
  CHECK(element_ids[0] == 1);
  CHECK(element_ids[1] == 2);
  CHECK(element_ids[2] == 3);
  CHECK(element_ids[3] == 4);
  CHECK(element_ids[133318] == 72044862);

  free(element_ids);

  CHECK_APPROX(d3plot_read_time(&plot_file, 0), 0.0);
  CHECK_APPROX(d3plot_read_time(&plot_file, 1), 0.0999492854);
  CHECK_APPROX(d3plot_read_time(&plot_file, 2), 0.1998985708);
  CHECK_APPROX(d3plot_read_time(&plot_file, 3), 0.2998797894);
  CHECK_APPROX(d3plot_read_time(&plot_file, 4), 0.399907);
  CHECK_APPROX(d3plot_read_time(&plot_file, 5), 0.499967);
  CHECK_APPROX(d3plot_read_time(&plot_file, 6), 0.599917);
  CHECK_APPROX(d3plot_read_time(&plot_file, 7), 0.699972);
  CHECK_APPROX(d3plot_read_time(&plot_file, 8), 0.799929);
  CHECK_APPROX(d3plot_read_time(&plot_file, 9), 0.899985);
  CHECK_APPROX(d3plot_read_time(&plot_file, 10), 0.999915);
  CHECK_APPROX(d3plot_read_time(&plot_file, 11), 1.099944);
  CHECK_APPROX(d3plot_read_time(&plot_file, 12), 1.199965);
  CHECK_APPROX(d3plot_read_time(&plot_file, 13), 1.299985);
  CHECK_APPROX(d3plot_read_time(&plot_file, 14), 1.399986);
  CHECK_APPROX(d3plot_read_time(&plot_file, 15), 1.499983);
  CHECK_APPROX(d3plot_read_time(&plot_file, 16), 1.599941);
  CHECK_APPROX(d3plot_read_time(&plot_file, 17), 1.699989);
  CHECK_APPROX(d3plot_read_time(&plot_file, 18), 1.799880);
  CHECK_APPROX(d3plot_read_time(&plot_file, 19), 1.899986);

  {
    double *node_data = d3plot_read_node_coordinates(&plot_file, 0, &num_nodes);
    REQUIRE(num_nodes == 114893);
    CHECK_APPROX(node_data[0], 0.031293001);
    CHECK_APPROX(node_data[1], -0.075000003);
    CHECK_APPROX(node_data[2], 69.1887664795);

    free(node_data);

    node_data = d3plot_read_node_velocity(&plot_file, 0, &num_nodes);
    REQUIRE(num_nodes == 114893);
    CHECK_APPROX(node_data[0], 0.0);
    CHECK_APPROX(node_data[1], 0.0);
    CHECK_APPROX(node_data[2], 0.0);

    free(node_data);

    node_data = d3plot_read_node_acceleration(&plot_file, 0, &num_nodes);
    REQUIRE(num_nodes == 114893);
    CHECK_APPROX(node_data[0], 0.0);
    CHECK_APPROX(node_data[1], 0.0);
    CHECK_APPROX(node_data[2], 0.0);

    free(node_data);
  }

  {
    float *node_data =
        d3plot_read_node_coordinates_32(&plot_file, 0, &num_nodes);
    REQUIRE(num_nodes == 114893);
    CHECK_APPROX(node_data[0], 0.031293001f);
    CHECK_APPROX(node_data[1], -0.075000003f);
    CHECK_APPROX(node_data[2], 69.1887664795f);

    free(node_data);

    node_data = d3plot_read_node_velocity_32(&plot_file, 0, &num_nodes);
    REQUIRE(num_nodes == 114893);
    CHECK_APPROX(node_data[0], 0.0f);
    CHECK_APPROX(node_data[1], 0.0f);
    CHECK_APPROX(node_data[2], 0.0f);

    free(node_data);

    node_data = d3plot_read_node_acceleration_32(&plot_file, 0, &num_nodes);
    REQUIRE(num_nodes == 114893);
    CHECK_APPROX(node_data[0], 0.0f);
    CHECK_APPROX(node_data[1], 0.0f);
    CHECK_APPROX(node_data[2], 0.0f);

    free(node_data);
  }

  d3plot_solid_con *solids_con =
      d3plot_read_solid_elements(&plot_file, &num_elements);
  REQUIRE(num_elements == 45000);

  CHECK(solids_con[43988].node_indices[0] == 32228);
  CHECK(solids_con[43988].node_indices[1] == 32229);
  CHECK(solids_con[43988].node_indices[2] == 32305);
  CHECK(solids_con[43988].node_indices[3] == 32304);
  CHECK(solids_con[43988].node_indices[4] == 33064);
  CHECK(solids_con[43988].node_indices[5] == 33065);
  CHECK(solids_con[43988].node_indices[6] == 33141);
  CHECK(solids_con[43988].node_indices[7] == 33140);

  CHECK(solids_con[44086].node_indices[0] == 32327);
  CHECK(solids_con[44086].node_indices[1] == 32328);
  CHECK(solids_con[44086].node_indices[2] == 32404);
  CHECK(solids_con[44086].node_indices[3] == 32403);
  CHECK(solids_con[44086].node_indices[4] == 33163);
  CHECK(solids_con[44086].node_indices[5] == 33164);
  CHECK(solids_con[44086].node_indices[6] == 33240);
  CHECK(solids_con[44086].node_indices[7] == 33239);

  CHECK(solids_con[43985].node_indices[0] == 32225);
  CHECK(solids_con[43985].node_indices[1] == 32226);
  CHECK(solids_con[43985].node_indices[2] == 32302);
  CHECK(solids_con[43985].node_indices[3] == 32301);
  CHECK(solids_con[43985].node_indices[4] == 33061);
  CHECK(solids_con[43985].node_indices[5] == 33062);
  CHECK(solids_con[43985].node_indices[6] == 33138);
  CHECK(solids_con[43985].node_indices[7] == 33137);

  size_t i = 0;
  while (i < num_elements) {
    CHECK(solids_con[i].material_index == 8);
    i++;
  }

  free(solids_con);

  d3plot_thick_shell_con *thick_shells_con =
      d3plot_read_thick_shell_elements(&plot_file, &num_elements);
  REQUIRE(num_elements == 0);

  d3plot_beam_con *beams_con =
      d3plot_read_beam_elements(&plot_file, &num_elements);
  REQUIRE(num_elements == 0);

  d3plot_shell_con *shells_con =
      d3plot_read_shell_elements(&plot_file, &num_elements);
  REQUIRE(num_elements == 88456);

  /* EL4 87441: (113858, 113859, 113808, 113807) 8*/
  CHECK(shells_con[87441].node_indices[0] == 113857);
  CHECK(shells_con[87441].node_indices[1] == 113858);
  CHECK(shells_con[87441].node_indices[2] == 113807);
  CHECK(shells_con[87441].node_indices[3] == 113806);
  CHECK(shells_con[87441].material_index == 7);
  /* EL4 88455: (114892, 114893, 114842, 114841) 8*/
  CHECK(shells_con[88455].node_indices[0] == 114891);
  CHECK(shells_con[88455].node_indices[1] == 114892);
  CHECK(shells_con[88455].node_indices[2] == 114841);
  CHECK(shells_con[88455].node_indices[3] == 114840);
  CHECK(shells_con[88455].material_index == 7);
  /* EL4 87806: (114231, 114232, 114181, 114180) 8*/
  CHECK(shells_con[87806].node_indices[0] == 114230);
  CHECK(shells_con[87806].node_indices[1] == 114231);
  CHECK(shells_con[87806].node_indices[2] == 114180);
  CHECK(shells_con[87806].node_indices[3] == 114179);
  CHECK(shells_con[87806].material_index == 7);

  i = 0;
  while (i < num_elements) {
    CHECK(shells_con[i].material_index >= 0);
    CHECK(shells_con[i].material_index <= 7);

    i++;
  }

  free(shells_con);

  size_t num_parts;
  d3_word *part_ids = d3plot_read_part_ids(&plot_file, &num_parts);
  REQUIRE(num_parts == 9);
  CHECK(part_ids[0] == 67000063);
  CHECK(part_ids[1] == 67000064);
  CHECK(part_ids[2] == 70000063);
  CHECK(part_ids[3] == 70000064);
  CHECK(part_ids[4] == 71000063);
  CHECK(part_ids[5] == 72000063);
  CHECK(part_ids[6] == 72000064);
  CHECK(part_ids[7] == 72000065);
  CHECK(part_ids[8] == 73000001);

  free(part_ids);

  char **part_titles = d3plot_read_part_titles(&plot_file, &num_parts);
  REQUIRE(num_parts == 9);
  CHECK(part_titles[0] == "Negative_Terminal_Copper_Anode                      "
                          "                    ");
  CHECK(part_titles[1] == "Negative_Terminal_Copper_Anode_Minus                "
                          "                    ");
  CHECK(part_titles[2] == "Positive_Terminal_Aluminum_Cathode                  "
                          "                    ");
  CHECK(part_titles[3] == "Positive_Terminal_Aluminum_Cathode_Plus             "
                          "                    ");
  CHECK(part_titles[4] == "Pouch                                               "
                          "                    ");
  CHECK(part_titles[5] == "Pouch_Fold                                          "
                          "                    ");
  CHECK(part_titles[6] == "Impactor                                            "
                          "                    ");
  CHECK(part_titles[7] == "Ground                                              "
                          "                    ");
  CHECK(part_titles[8] == "Jellyroll                                           "
                          "                    ");

  i = 0;
  while (i < num_parts) {
    free(part_titles[i]);

    i++;
  }
  free(part_titles);

  d3plot_part part = d3plot_read_part(&plot_file, 0);
  CHECK(part.num_shells == 120);
  d3plot_free_part(&part);

  part = d3plot_read_part(&plot_file, 1);
  CHECK(part.num_shells == 10);
  d3plot_free_part(&part);

  part = d3plot_read_part(&plot_file, 2);
  CHECK(part.num_shells == 118);
  d3plot_free_part(&part);

  part = d3plot_read_part(&plot_file, 3);
  CHECK(part.num_shells == 12);
  d3plot_free_part(&part);

  part = d3plot_read_part(&plot_file, 4);
  CHECK(part.num_shells == 7368);

  size_t num_shell_ids;
  d3_word *shell_ids =
      d3plot_read_shell_element_ids(&plot_file, &num_shell_ids);
  d3plot_shell_con *shell_cons =
      d3plot_read_shell_elements(&plot_file, &num_shell_ids);

  d3plot_part_get_node_ids_params pgni_params;
  pgni_params.solid_ids = NULL;
  pgni_params.beam_ids = NULL;
  pgni_params.shell_ids = &shell_ids;
  pgni_params.thick_shell_ids = NULL;
  pgni_params.node_ids = NULL;
  pgni_params.solid_cons = NULL;
  pgni_params.beam_cons = NULL;
  pgni_params.shell_cons = &shell_cons;
  pgni_params.thick_shell_cons = NULL;
  pgni_params.num_solids = NULL;
  pgni_params.num_beams = NULL;
  pgni_params.num_shells = &num_shell_ids;
  pgni_params.num_thick_shells = NULL;
  pgni_params.num_node_ids = NULL;

  size_t num_part_node_ids;
  d3_word *part_node_ids = d3plot_part_get_node_ids(
      &plot_file, &part, &num_part_node_ids, &pgni_params);

  free(shell_ids);
  free(shell_cons);

  CHECK(num_part_node_ids == 7370);

  free(part_node_ids);
  d3plot_free_part(&part);

  part = d3plot_read_part(&plot_file, 5);
  CHECK(part.num_shells == 828);
  d3plot_free_part(&part);

  part = d3plot_read_part(&plot_file, 6);
  CHECK(part.num_shells == 75000);
  d3plot_free_part(&part);

  part = d3plot_read_part(&plot_file, 7);
  CHECK(part.num_shells == 5000);
  d3plot_free_part(&part);

  part = d3plot_read_part(&plot_file, 8);
  CHECK(part.num_solids == 45000);
  d3plot_free_part(&part);

  d3plot_solid *solids =
      d3plot_read_solids_state(&plot_file, 101, &num_elements);
  REQUIRE(num_elements == 45000);
  free(solids);

  d3plot_thick_shell *thick_shells =
      d3plot_read_thick_shells_state(&plot_file, 101, &num_elements);
  REQUIRE(num_elements == 0);
  free(thick_shells);

  d3plot_beam *beams = d3plot_read_beams_state(&plot_file, 101, &num_elements);
  REQUIRE(num_elements == 0);
  free(beams);

  d3plot_shell *shells =
      d3plot_read_shells_state(&plot_file, 101, &num_elements);
  REQUIRE(num_elements == 88456);
  free(shells);

  d3plot_close(&plot_file);
}

#ifdef D3PLOT_CPP
TEST_CASE("d3plot C++") {
  try {
    dro::D3plot plot_file("i_dont_exist");
    FAIL("No exception was thrown");
  } catch (const dro::D3plot::Exception &e) {
    CHECK(strlen(e.what()) > 0);
  }

  dro::D3plot plot_file("test_data/d3plot");

  {
    const auto title(plot_file.read_title());
    CHECK(title == "Pouch_macro_37Ah                        ");
  }

  struct tm *run_time = plot_file.read_run_time();
  REQUIRE(run_time != NULL);
  CHECK(run_time->tm_wday == 3);
  CHECK(run_time->tm_mday == 10);
  CHECK(run_time->tm_mon == 10);
  CHECK(run_time->tm_year == 121);
  CHECK(run_time->tm_hour == 12);
  CHECK(run_time->tm_min == 1);
  CHECK(run_time->tm_sec == 42);

  REQUIRE(plot_file.num_time_steps() == 102);

  {
    const auto node_ids(plot_file.read_node_ids());

    REQUIRE(node_ids.size() == 114893);
    CHECK(node_ids[59530] == 84285019);
    CHECK(node_ids[0] == 10);
    CHECK(node_ids[114892] == 84340381);
    CHECK(node_ids[2458] == 2852);
  }

  {
    const auto element_ids(plot_file.read_all_element_ids());

    REQUIRE(element_ids.size() == 133456);
    CHECK(element_ids[0] == 1);
    CHECK(element_ids[1] == 2);
    CHECK(element_ids[2] == 3);
    CHECK(element_ids[3] == 4);
    CHECK(element_ids[133318] == 72044862);
  }

  CHECK_APPROX(plot_file.read_time(0), 0.0);
  CHECK_APPROX(plot_file.read_time(1), 0.0999492854);
  CHECK_APPROX(plot_file.read_time(2), 0.1998985708);
  CHECK_APPROX(plot_file.read_time(3), 0.2998797894);
  CHECK_APPROX(plot_file.read_time(4), 0.399907);
  CHECK_APPROX(plot_file.read_time(5), 0.499967);
  CHECK_APPROX(plot_file.read_time(6), 0.599917);
  CHECK_APPROX(plot_file.read_time(7), 0.699972);
  CHECK_APPROX(plot_file.read_time(8), 0.799929);
  CHECK_APPROX(plot_file.read_time(9), 0.899985);
  CHECK_APPROX(plot_file.read_time(10), 0.999915);
  CHECK_APPROX(plot_file.read_time(11), 1.099944);
  CHECK_APPROX(plot_file.read_time(12), 1.199965);
  CHECK_APPROX(plot_file.read_time(13), 1.299985);
  CHECK_APPROX(plot_file.read_time(14), 1.399986);
  CHECK_APPROX(plot_file.read_time(15), 1.499983);
  CHECK_APPROX(plot_file.read_time(16), 1.599941);
  CHECK_APPROX(plot_file.read_time(17), 1.699989);
  CHECK_APPROX(plot_file.read_time(18), 1.799880);
  CHECK_APPROX(plot_file.read_time(19), 1.899986);

  {
    const auto node_data(plot_file.read_node_coordinates(0));
    REQUIRE(node_data.size() == 114893);
    CHECK_APPROX(node_data[0][0], 0.031293001);
    CHECK_APPROX(node_data[0][1], -0.075000003);
    CHECK_APPROX(node_data[0][2], 69.1887664795);
  }

  {
    const auto node_data(plot_file.read_node_velocity(0));
    REQUIRE(node_data.size() == 114893);
    CHECK_APPROX(node_data[0][0], 0.0);
    CHECK_APPROX(node_data[0][1], 0.0);
    CHECK_APPROX(node_data[0][2], 0.0);
  }

  {
    const auto node_data(plot_file.read_node_acceleration(0));
    REQUIRE(node_data.size() == 114893);
    CHECK_APPROX(node_data[0][0], 0.0);
    CHECK_APPROX(node_data[0][1], 0.0);
    CHECK_APPROX(node_data[0][2], 0.0);
  }

  {
    const auto solids(plot_file.read_solid_elements());
    REQUIRE(solids.size() == 45000);

    CHECK(solids[43988].node_indices[0] == 32228);
    CHECK(solids[43988].node_indices[1] == 32229);
    CHECK(solids[43988].node_indices[2] == 32305);
    CHECK(solids[43988].node_indices[3] == 32304);
    CHECK(solids[43988].node_indices[4] == 33064);
    CHECK(solids[43988].node_indices[5] == 33065);
    CHECK(solids[43988].node_indices[6] == 33141);
    CHECK(solids[43988].node_indices[7] == 33140);

    CHECK(solids[44086].node_indices[0] == 32327);
    CHECK(solids[44086].node_indices[1] == 32328);
    CHECK(solids[44086].node_indices[2] == 32404);
    CHECK(solids[44086].node_indices[3] == 32403);
    CHECK(solids[44086].node_indices[4] == 33163);
    CHECK(solids[44086].node_indices[5] == 33164);
    CHECK(solids[44086].node_indices[6] == 33240);
    CHECK(solids[44086].node_indices[7] == 33239);

    CHECK(solids[43985].node_indices[0] == 32225);
    CHECK(solids[43985].node_indices[1] == 32226);
    CHECK(solids[43985].node_indices[2] == 32302);
    CHECK(solids[43985].node_indices[3] == 32301);
    CHECK(solids[43985].node_indices[4] == 33061);
    CHECK(solids[43985].node_indices[5] == 33062);
    CHECK(solids[43985].node_indices[6] == 33138);
    CHECK(solids[43985].node_indices[7] == 33137);

    for (const auto &solid : solids) {
      CHECK(solid.material_index == 8);
    }
  }

  {
    const auto thick_shells(plot_file.read_thick_shell_elements());
    REQUIRE(thick_shells.empty());
  }

  {
    const auto beams(plot_file.read_beam_elements());
    REQUIRE(beams.empty());
  }

  {
    const auto shells(plot_file.read_shell_elements());
    REQUIRE(shells.size() == 88456);

    /* EL4 87441: (113858, 113859, 113808, 113807) 8*/
    CHECK(shells[87441].node_indices[0] == 113857);
    CHECK(shells[87441].node_indices[1] == 113858);
    CHECK(shells[87441].node_indices[2] == 113807);
    CHECK(shells[87441].node_indices[3] == 113806);
    CHECK(shells[87441].material_index == 7);
    /* EL4 88455: (114892, 114893, 114842, 114841) 8*/
    CHECK(shells[88455].node_indices[0] == 114891);
    CHECK(shells[88455].node_indices[1] == 114892);
    CHECK(shells[88455].node_indices[2] == 114841);
    CHECK(shells[88455].node_indices[3] == 114840);
    CHECK(shells[88455].material_index == 7);
    /* EL4 87806: (114231, 114232, 114181, 114180) 8*/
    CHECK(shells[87806].node_indices[0] == 114230);
    CHECK(shells[87806].node_indices[1] == 114231);
    CHECK(shells[87806].node_indices[2] == 114180);
    CHECK(shells[87806].node_indices[3] == 114179);
    CHECK(shells[87806].material_index == 7);

    for (const auto &shell : shells) {
      CHECK(shell.material_index >= 0);
      CHECK(shell.material_index <= 7);
    }
  }

  {
    const auto part_ids(plot_file.read_part_ids());
    REQUIRE(part_ids.size() == 9);
    CHECK(part_ids[0] == 67000063);
    CHECK(part_ids[1] == 67000064);
    CHECK(part_ids[2] == 70000063);
    CHECK(part_ids[3] == 70000064);
    CHECK(part_ids[4] == 71000063);
    CHECK(part_ids[5] == 72000063);
    CHECK(part_ids[6] == 72000064);
    CHECK(part_ids[7] == 72000065);
    CHECK(part_ids[8] == 73000001);

    const auto part_titles = plot_file.read_part_titles();
    REQUIRE(part_titles.size() == 9);
    CHECK(part_titles[0] ==
          "Negative_Terminal_Copper_Anode                      "
          "                    ");
    CHECK(part_titles[1] ==
          "Negative_Terminal_Copper_Anode_Minus                "
          "                    ");
    CHECK(part_titles[2] ==
          "Positive_Terminal_Aluminum_Cathode                  "
          "                    ");
    CHECK(part_titles[3] ==
          "Positive_Terminal_Aluminum_Cathode_Plus             "
          "                    ");
    CHECK(part_titles[4] ==
          "Pouch                                               "
          "                    ");
    CHECK(part_titles[5] ==
          "Pouch_Fold                                          "
          "                    ");
    CHECK(part_titles[6] ==
          "Impactor                                            "
          "                    ");
    CHECK(part_titles[7] ==
          "Ground                                              "
          "                    ");
    CHECK(part_titles[8] ==
          "Jellyroll                                           "
          "                    ");
  }

  auto part(plot_file.read_part(0));
  CHECK(part.get_shell_elements().size() == 120);

  part = plot_file.read_part(1);
  CHECK(part.get_shell_elements().size() == 10);

  part = plot_file.read_part(2);
  CHECK(part.get_shell_elements().size() == 118);

  part = plot_file.read_part(3);
  CHECK(part.get_shell_elements().size() == 12);

  part = plot_file.read_part(4);
  CHECK(part.get_shell_elements().size() == 7368);

  part = plot_file.read_part(5);
  CHECK(part.get_shell_elements().size() == 828);

  part = plot_file.read_part(6);
  CHECK(part.get_shell_elements().size() == 75000);

  part = plot_file.read_part(7);
  CHECK(part.get_shell_elements().size() == 5000);

  part = plot_file.read_part(8);
  CHECK(part.get_solid_elements().size() == 45000);

  {
    const auto solids = plot_file.read_solids_state(101);
    REQUIRE(solids.size() == 45000);
  }

  {
    const auto thick_shells = plot_file.read_thick_shells_state(101);
    REQUIRE(thick_shells.empty());
  }

  {
    const auto beams = plot_file.read_beams_state(101);
    REQUIRE(beams.empty());
  }

  {
    const auto shells = plot_file.read_shells_state(101);
    REQUIRE(shells.size() == 88456);
  }
}
#endif

TEST_CASE("basic01") {
  d3plot_file plot_file = d3plot_open("test_data/basic01/d3plot");
  if (plot_file.error_string) {
    FAIL(plot_file.error_string);
    d3plot_close(&plot_file);
    return;
  }

  char *title = d3plot_read_title(&plot_file);
  CHECK(title == "implicit tensile test                                        "
                 "                   ");
  free(title);

  size_t num_parts;
  d3_word *part_ids = d3plot_read_part_ids(&plot_file, &num_parts);
  REQUIRE(num_parts == 1);
  CHECK(part_ids[0] == 1);

  free(part_ids);

  char **part_titles = d3plot_read_part_titles(&plot_file, &num_parts);
  REQUIRE(num_parts == 1);
  CHECK(part_titles[0] == "shell tensile strip                                 "
                          "                    ");

  free(part_titles[0]);
  free(part_titles);

  size_t num_nodes;
  d3_word *node_ids = d3plot_read_node_ids(&plot_file, &num_nodes);
  REQUIRE(num_nodes == 186);

  free(node_ids);

  d3plot_part part1 = d3plot_read_part(&plot_file, 0);
  CHECK(part1.num_shells == 150);
  CHECK(part1.num_solids == 0);
  CHECK(part1.num_beams == 0);
  CHECK(part1.num_thick_shells == 0);

  d3plot_free_part(&part1);

  size_t num_elements;
  d3_word *element_ids = d3plot_read_all_element_ids(&plot_file, &num_elements);
  REQUIRE(num_elements == 150);

  free(element_ids);

  REQUIRE(plot_file.num_states == 2);

  double *node_coords = d3plot_read_node_coordinates(&plot_file, 1, &num_nodes);
  REQUIRE(num_nodes == 186);

  free(node_coords);

  d3plot_close(&plot_file);
}

TEST_CASE("basic02") {
  d3plot_file plot_file = d3plot_open("test_data/basic02/d3plot");
  if (plot_file.error_string) {
    FAIL(plot_file.error_string);
    d3plot_close(&plot_file);
    return;
  }

  char *title = d3plot_read_title(&plot_file);
  CHECK(title == "implicit tensile test, elastic-plastic material              "
                 "                   ");
  free(title);

  size_t num_parts;
  d3_word *part_ids = d3plot_read_part_ids(&plot_file, &num_parts);
  REQUIRE(num_parts == 1);
  CHECK(part_ids[0] == 1);

  free(part_ids);

  char **part_titles = d3plot_read_part_titles(&plot_file, &num_parts);
  REQUIRE(num_parts == 1);
  CHECK(part_titles[0] == "shell tensile strip                                 "
                          "                    ");

  free(part_titles[0]);
  free(part_titles);

  size_t num_nodes;
  d3_word *node_ids = d3plot_read_node_ids(&plot_file, &num_nodes);
  REQUIRE(num_nodes == 186);

  free(node_ids);

  d3plot_part part1 = d3plot_read_part(&plot_file, 0);
  CHECK(part1.num_shells == 150);
  CHECK(part1.num_solids == 0);
  CHECK(part1.num_beams == 0);
  CHECK(part1.num_thick_shells == 0);

  d3plot_free_part(&part1);

  size_t num_elements;
  d3_word *element_ids = d3plot_read_all_element_ids(&plot_file, &num_elements);
  REQUIRE(num_elements == 150);

  free(element_ids);

  REQUIRE(plot_file.num_states == 101);

  double *node_coords = d3plot_read_node_coordinates(&plot_file, 1, &num_nodes);
  REQUIRE(num_nodes == 186);

  free(node_coords);

  d3plot_close(&plot_file);
}

TEST_CASE("_get_nth_digit") {
  const d3_word value = 123456;
  CHECK(_get_nth_digit(value, 0) == 6);
  CHECK(_get_nth_digit(value, 1) == 5);
  CHECK(_get_nth_digit(value, 2) == 4);
  CHECK(_get_nth_digit(value, 3) == 3);
  CHECK(_get_nth_digit(value, 4) == 2);
  CHECK(_get_nth_digit(value, 5) == 1);

  const d3_word value1 = 10;
  CHECK(_get_nth_digit(value1, 0) == 0);
  CHECK(_get_nth_digit(value1, 1) == 1);
  CHECK(_get_nth_digit(value1, 2) == 0);
  CHECK(_get_nth_digit(value1, 3) == 0);
  CHECK(_get_nth_digit(value1, 4) == 0);
}

TEST_CASE("_insert_sorted") {
  {
    const d3_word dst[7] = {1, 2, 3, 10, 11, 12, 13};
    const d3_word src[6] = {4, 5, 6, 7, 8, 9};
    d3_word *_dst = (d3_word *)malloc(7 * sizeof(d3_word));
    memcpy(_dst, dst, sizeof(dst));

    _dst = _insert_sorted(_dst, 7, src, 6);
    size_t i = 1;
    while (i <= 13) {
      CHECK(_dst[i - 1] == i);
      i++;
    }

    free(_dst);
  }
  {
    const d3_word dst[4] = {10, 11, 12, 13};
    const d3_word src[6] = {4, 5, 6, 7, 8, 9};
    d3_word *_dst = (d3_word *)malloc(4 * sizeof(d3_word));
    memcpy(_dst, dst, sizeof(dst));

    _dst = _insert_sorted(_dst, 4, src, 6);
    size_t i = 4;
    while (i <= 13) {
      CHECK(_dst[i - 4] == i);
      i++;
    }

    free(_dst);
  }
  {
    const d3_word dst[6] = {4, 5, 6, 7, 8, 9};
    const d3_word src[4] = {10, 11, 12, 13};
    d3_word *_dst = (d3_word *)malloc(6 * sizeof(d3_word));
    memcpy(_dst, dst, sizeof(dst));

    _dst = _insert_sorted(_dst, 6, src, 4);
    size_t i = 4;
    while (i <= 13) {
      CHECK(_dst[i - 4] == i);
      i++;
    }

    free(_dst);
  }
  {
    const d3_word dst[1] = {4};
    const d3_word src[4] = {10, 11, 12, 13};
    d3_word *_dst = (d3_word *)malloc(1 * sizeof(d3_word));
    memcpy(_dst, dst, sizeof(dst));

    _dst = _insert_sorted(_dst, 1, src, 4);
    CHECK(_dst[0] == 4);
    CHECK(_dst[1] == 10);
    CHECK(_dst[2] == 11);
    CHECK(_dst[3] == 12);
    CHECK(_dst[4] == 13);

    free(_dst);
  }
  {
    const d3_word dst[1] = {14};
    const d3_word src[4] = {10, 11, 12, 13};
    d3_word *_dst = (d3_word *)malloc(1 * sizeof(d3_word));
    memcpy(_dst, dst, sizeof(dst));

    _dst = _insert_sorted(_dst, 1, src, 4);
    CHECK(_dst[0] == 10);
    CHECK(_dst[1] == 11);
    CHECK(_dst[2] == 12);
    CHECK(_dst[3] == 13);
    CHECK(_dst[4] == 14);

    free(_dst);
  }
}

TEST_CASE("binary_search") {
  constexpr d3_word arr[] = {1, 2, 3, 4, 6, 7, 8, 9, 10, 11};
  constexpr size_t arr_size = sizeof(arr) / sizeof(*arr);
  size_t idx = d3_word_binary_search(arr, 0, arr_size - 1, 6);
  CHECK(idx == 4);
  idx = d3_word_binary_search(arr, 0, arr_size - 1, 1);
  CHECK(idx == 0);
  idx = d3_word_binary_search(arr, 0, arr_size - 1, 10);
  CHECK(idx == 8);
  idx = d3_word_binary_search(arr, 0, arr_size - 1, 11);
  CHECK(idx == 9);
  idx = d3_word_binary_search(arr, 0, arr_size - 1, 12);
  CHECK(idx == UINT64_MAX);
  idx = d3_word_binary_search(arr, 0, arr_size - 1, 0);
  CHECK(idx == UINT64_MAX);
  idx = d3_word_binary_search(arr, 0, arr_size - 1, 5);
  CHECK(idx == UINT64_MAX);
}

// #ifdef PROFILING
// int main(int args, char *argv[]) {
// doctest::Context ctx;
//
// ctx.addFilter("test-case", "d3plot");
// ctx.applyCommandLine(args, argv);
//
// const int res = ctx.run();
//
// if (ctx.shouldExit()) {
// return res;
// }
//
// END_PROFILING("test_data/d3plot_profiling.txt");
// }
// #endif
