# dynareadout

An Ansi C library for parsing binary output files of LS Dyna (d3plot, binout)

## Bindings

This library also supports bindings to other languages

| Language | Implemented |
| -------- | :---------: |
| C++      |      ✅      |
| Python   |      ❌      |
| Go       |      ❌      |

## Build

You just need any C compiler and [xmake](https://xmake.io) as the build system. Just call `xmake` and the library will build. You can find the built binaries inside the build folder. To also build some of the bindings you need to call `xmake f` with one or more of the following flags. And then `xmake` again.

| Language | Flag            | Default |
| -------- | --------------- | :-----: |
| C++      | `--build_cpp=y` |    ✅    |
| Python   | -               |    ❌    |
| Go       | -               |    ❌    |

## Example

### C

```c

#include <binout.h>
#include <stdio.h>

int main(int args, char* argv[]) {
  /* This library also supports opening multiple binout files at once by globing them*/
  binout_file bin_file = binout_open("simulation/binout*");
  char* open_error = binout_open_error(&bin_file);
  if (open_error) {
    fprintf(stderr, "Failed to open binout: %s\n", open_error);
    free(open_error);
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
  binout_free_children(children, num_children);

  /* Read some data. The library implements read functions for multiple types*/
  size_t num_node_ids;
  int32_t* node_ids = binout_read_int32_t(&bin_file, "/nodout/metadata/ids", &num_node_ids);
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

### C++

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

    const dro::Vector<int32_t> node_ids = bin_file.read<int32_t>("/nodout/metadata/ids");
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