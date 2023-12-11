/*************************************************************************
 * Copyright (C) 2023 Blue Brain Project
 *
 * This file is part of 'libsonata-mpi', distributed under the terms
 * of the GNU Lesser General Public License version 3.
 *
 * See top-level COPYING.LESSER and COPYING files for details.
 *************************************************************************/

#include <pybind11/pybind11.h>

#include <bbp/sonata/hdf5_collective_reader.h>
#include <mpi4py/mpi4py.h>

namespace py = pybind11;

using namespace pybind11::literals;

using namespace bbp::sonata;


PYBIND11_MODULE(_libsonata_mpi, m) {
    if (import_mpi4py() < 0) {
        throw std::runtime_error("Could not load mpi4py API.");
    }

    m.def("make_collective_reader",
          [](py::object comm, bool collective_metadata, bool collective_transfer) {
              PyObject* py_comm = comm.ptr();
              // Check that we have been passed an mpi4py communicator
              if (PyObject_TypeCheck(py_comm, &PyMPIComm_Type)) {
                  return make_collective_reader(*PyMPIComm_Get(py_comm),
                                                collective_metadata,
                                                collective_transfer);
              }
              throw std::runtime_error("Not an MPI Comm!");
          });
}
