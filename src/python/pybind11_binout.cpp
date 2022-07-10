#include <binout.h>
#include <pybind11/pybind11.h>

PYBIND11_MODULE(libbinout, m) {
  m.doc() = "Read in LS-Dyna Binout files";

  m.def("binout_open", &binout_open, "Opens a binout");
  m.def("binout_close", &binout_close, "Closes a binout");
}