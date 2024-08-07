# dynareadout

High-Performance and Thread-Safe C/C++ library for parsing binary output files and key files of LS Dyna (d3plot, binout, input deck). The core is written in Ansi C to ensure the highest compatibility and portability to other programming languages.

## Bindings

This library also supports bindings to other languages

| Language | Repository                                                          |
| -------- | ------------------------------------------------------------------- |
| C++      | [dynareadout](https://github.com/PucklaJ/dynareadout)               |
| Python   | [dynareadout_python](https://github.com/PucklaJ/dynareadout_python) |
| Go       | [dynareadout_go](https://github.com/PucklaJ/dynareadout_go)         |
| Odin     | [dynareadout_odin](https://github.com/PucklaJ/dynareadout_odin)     |

## Wiki

This library has a [wiki](https://github.com/PucklaJ/dynareadout/wiki) which includes the API documentation for all languages.

## Build

You just need any C compiler and [xmake](https://xmake.io) as the build system. Just call `xmake` and the library will build. You can find the built binaries inside the build folder. To also build some of the bindings you need to call `xmake f` with one or more of the following flags. And then `xmake` again.

| Language | Flag               | Default |
| -------- | ------------------ | :-----: |
| C++      | `--build_cpp=y`    |    ✅    |
| Python   | `--build_python=y` |    ❌    |

The repository also contains a Makefile. This file can be used to build the C and C++ libraries of dynareadout. By default calling `make` will only compile the C library with **gcc** for **x86_64**, but when executing `make all` it will also build the C++ library. Note that the C++ library requires a gcc installation supporting C++17. The built binaries can be found inside the `build/linux/x86_64/release` folder.

## xmake repo

This library has been added to the [xmake repo](https://github.com/xmake-io/xmake-repo). Therefore it can be installed using `xrepo install dynareadout`. To use it inside an xmake project use the following code inside your xmake.lua.

```lua
add_requires("dynareadout")
-- enable or disable C++ bindings
-- add_requires("dynareadout", {configs = {cpp = false}})
-- shared build instead of static
-- add_requires("dynareadout", {shared = true})
-- no thread safety
-- add_requires("dynareadout", {configs = {thread_safe = false}})

target("example")
  set_kind("binary")
  add_packages("dynareadout")
  add_files("src/example.c")
```

## Python

The python version of this library can by installed using pip:

```console
python -m pip install dynareadout
```

Examples for the python version can be found on the [repository](https://github.com/PucklaJ/dynareadout_python)

## Examples

### C - Binout

```c

#include <binout.h>
#include <stdio.h>

int main(int args, char* argv[]) {
  /* This library also supports opening multiple binout files at once by globing them*/
  binout_file bin_file = binout_open("simulation/binout*");
  if (bin_file.error_string) {
    fprintf(stderr, "Failed to open binout: %s\n", bin_file.error_string);
    binout_close(&bin_file);
    /* You could continue after an open error, since it just means that one file failed to open, but in this example we quit*/
    return 1;
  }

  /* Print the children of the binout*/
  size_t num_children;
  char** children = binout_get_children(&bin_file, "/", &num_children);
  size_t i = 0;
  while(i < 0) {
    printf("Child %d: %s\n", i, children[i]);

    i++;
  }
  binout_free_children(children);

  /* Read some data. The library implements read functions for multiple types*/
  size_t num_node_ids;
  int32_t* node_ids = binout_read_i32(&bin_file, "/nodout/metadata/ids", &num_node_ids);
  /* After any read you should check error_string*/
  if (bin_file.error_string) {
    fprintf(stderr, "Failed to read node ids: %s\n", bin_file.error_string);
    /* You can just continue with the program*/
  } else {
    i = 0;
    while(i < num_node_ids) {
      printf("Node ID %d: %d\n", i, node_ids[i]);

      i++;
    }

    /* Always make sure to free the data*/
    free(node_ids);
  }


  binout_close(&bin_file);

  return 0;
}
```

### C - D3plot

```c
#include <d3plot.h>
#include <stdio.h>

int main(int args, char* argv[]) {
  /* Just give it the first d3plot file and it opens all of them*/
  d3plot_file plot_file = d3plot_open("simulation/d3plot");
  /* Always make sure to check for errors*/
  if (plot_file.error_string) {
    fprintf(stderr, "Failed to open: %s\n", plot_file.error_string);
    d3plot_close(&plot_file);
    return 1;
  }

  /* Read the title and run time*/
  char* title = d3plot_read_title(&plot_file);
  printf("Title: %s\n", title);
  /* Always make sure to check if the return value needs to be deallocated*/
  free(title);

  struct tm* run_time = d3plot_read_run_time(&plot_file);
  printf("Date: %d.%d.%d\n", run_time.tm_mday, run_time.tm_mon, run_time.tm_year);

  /* Read node ids*/
  size_t num_nodes;
  d3_word* node_ids = d3plot_read_node_ids(&plot_file, &num_nodes);

  printf("Nodes: %d\n", num_nodes);
  for (size_t i = 0; i < num_nodes; i++) {
    printf("Node %d: %d\n", i, node_ids[i]);
  }

  free(node_ids);

  /* Read node coordinates of time step 10*/
  double* node_coords = d3plot_read_node_coordinates(&plot_file, 10, &num_nodes);

  for (size_t i = 0; i < num_nodes; i++) {
    printf("Node Coords %d: (%.2f, %.2f, %.2f)\n", i, node_coords[i * 3 + 0], node_coords[i * 3 + 1], node_coords[i * 3 + 2]);
  }

  free(node_coords);

  /* Close the d3plot file at the end*/
  d3plot_close(&plot_file);

  return 0;
}

```

### C - KeyFile

```c
#include <errno.h>
#include <key.h>
#include <stdio.h>
#include <stdlib.h>

int main(int args, char *argv[]) {
  size_t num_keywords;
  char *error_string;
  keyword_t *keywords =
      key_file_parse("simulation/input.k", &num_keywords, 1, &error_string);
  /* Always make sure to check for errors */
  if (error_string) {
    fprintf(stderr, "Failed to parse key file: %s\n", error_string);
    free(error_string);
    return 1;
  }

  /* Print all nodes */
  size_t node_index = 0;
  keyword_t *node_keyword =
      key_file_get(keywords, num_keywords, "NODE", node_index++);
  while (node_keyword) {
    size_t card_index = 0;
    while (card_index < node_keyword->num_cards) {
      card_t *card = &node_keyword->cards[card_index];

      /* Parse the values of a card by iterating over them*/
      card_parse_begin(card, DEFAULT_VALUE_WIDTH);
      const int64_t nid = card_parse_int_width(card, 8);
      card_parse_next_width(card, 8);
      const double x = card_parse_float64_width(card, 16);
      card_parse_next_width(card, 16);
      const double y = card_parse_float64_width(card, 16);
      card_parse_next_width(card, 16);
      const double z = card_parse_float64_width(card, 16);

      /* You can check for parsing errors using errno */
      if (errno != 0) {
        fprintf(stderr, "An error occurred while parsing NODE\n");
      } else {
        printf("NODE %ld: (%.3f; %.3f; %.3f)\n", nid, x, y, z);
      }

      card_index++;
    }

    node_keyword = key_file_get(keywords, num_keywords, "NODE", node_index++);
  }

  /* Always make sure to deallocate the memory */
  key_file_free(keywords, num_keywords);

  return 0;
}
```

### C++ - Binout

```C++
#include <iostream>
#include <binout.hpp>

int main(int args, char* argv[]) {
  // The library throws an exception on an error
  try {
    dro::Binout bin_file("simulation/binout*");

    const std::vector<dro::String> children = bin_file.get_children("/");
    for (const auto& child : children) {
      std::cout << "Child: " << child << std::endl;
    }

    const dro::Array<int32_t> node_ids = bin_file.read<int32_t>("/nodout/metadata/ids");
    for (const auto id : node_ids) {
      std::cout << "Node ID: " << id << std::endl;
    }
  } catch (const dro::Binout::Exception& e) {
    std::cerr << "An error occurred: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}

```

### C++ - D3plot

```C++
#include <iostream>
#include <d3plot.hpp>

int main(int args, char* argv[]) {
  try {
    dro::D3plot plot_file("simulation/d3plot");

    const dro::String title(plot_file.read_title());
    std::cout << "Title: " << title << std::endl;

    struct tm* run_time = plot_file.read_run_time();
    std::cout << "Date: " << run_time.tm_mday << "." << run_time.tm_mon << "." << run_time.tm_year << std::endl;

    const dro::Array<d3_word> node_ids(plot_file.read_node_ids());
    std::cout << "Nodes: " << node_ids.size() << std::endl;

    for (const auto& nid : node_ids) {
      std::cout << "Node ID: " << nid << std::endl;
    }

    const dro::Array<dro::dVec3> node_coords(plot_file.read_node_coordinates(10));
    for (const auto& coords : node_coords) {
      std::cout << "Node Coords: (" << coords[0] << ", " << coords[1] << ", " << coords[2] << ")" << std::endl;
    }
  } catch (const dro::D3plot::Exception& e) {
    std::cerr << "An error occurred: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}

```

### C++ - KeyFile

```C++
#include <iomanip>
#include <iostream>
#include <key.hpp>

int main(int args, char *argv[]) {
  try {
    dro::Keywords keywords = dro::KeyFile::parse("simulation/input.k");

    // Parse all nodes
    for (auto node_keyword : keywords["NODE"]) {
      for (auto node_card : node_keyword) {
        auto [nid, x, y, z] =
            node_card.parse_whole<int, float, float, float>({8, 16, 16, 16});

        std::cout << std::setprecision(3) << std::fixed;
        std::cout << "NODE " << nid << ": (" << x << "; " << y << "; " << z
                  << ")" << std::endl;
      }
    }

  } catch (const dro::KeyFile::Exception &e) {
    std::cerr << "An error occurred: " << e.what() << std::endl;
  }

  return 0;
}
```
