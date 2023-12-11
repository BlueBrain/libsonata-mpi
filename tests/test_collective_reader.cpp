/*************************************************************************
 * Copyright (C) 2023 Blue Brain Project
 *
 * This file is part of 'libsonata-mpi', distributed under the terms
 * of the GNU Lesser General Public License version 3.
 *
 * See top-level COPYING.LESSER and COPYING files for details.
 *************************************************************************/

#include <catch2/catch_test_macros.hpp>

#include <bbp/sonata/hdf5_collective_reader.h>

#include <mpi.h>

namespace mpi {
int comm_rank(MPI_Comm comm) {
  int rank = -1;
  MPI_Comm_rank(comm, &rank);

  return rank;
}

int comm_size(MPI_Comm comm) {
  int size = -1;
  MPI_Comm_size(comm, &size);

  return size;
}
}

template<class T>
std::string deduce_dataset_name();

template<>
std::string deduce_dataset_name<float>() {
  return "float32";
}

template<>
std::string deduce_dataset_name<double>() {
  return "float64";
}

template<>
std::string deduce_dataset_name<uint64_t>() {
  return "uint64";
}


template<class T>
void check_one_dimensional(const bbp::sonata::Hdf5Reader& reader,
                           const bbp::sonata::Selection& selection) {

  std::string filename = "tests/data/test_data.h5";
  auto file = reader.openFile(filename);

  auto grp = file.getGroup("one_dim");
  auto dset_name = deduce_dataset_name<T>();

  auto flat_selection = selection.flatten();

  SECTION("One dimensional: " + dset_name) {
    auto dset = grp.getDataSet(dset_name);
    auto x = reader.readSelection<T>(dset, selection);
    for(size_t i = 0; i < flat_selection.size(); ++i) {
        REQUIRE(x[i] == flat_selection[i]);
    }
  }
}

template<class T>
void check_two_dimensional(const bbp::sonata::Hdf5Reader& reader,
                           const bbp::sonata::Selection& xsel) {

  std::string filename = "tests/data/test_data.h5";
  auto file = reader.openFile(filename);

  auto dset = file.getDataSet("two_dim");

  auto flat_xsel = xsel.flatten();
  auto ysel = bbp::sonata::Selection({{0, 2}});

  SECTION("Two dimensional.") {
    auto dims = dset.getDimensions();

    auto x = reader.readSelection<T>(dset, xsel, ysel);
    for(size_t i = 0; i < flat_xsel .size(); ++i) {
        REQUIRE(x[i][0] == flat_xsel[i] * dims[1]);
        REQUIRE(x[i][1] == flat_xsel[i] * dims[1] + 1);
    }
  }
}

TEST_CASE("ReadDataSet") {
  auto comm = MPI_COMM_WORLD;
  auto rank = mpi::comm_rank(comm);

  auto reader = bbp::sonata::make_collective_reader(comm, true, true);

  auto selections = std::vector<bbp::sonata::Selection>{
    bbp::sonata::Selection({{0, 3}, {5, 6}, {8, 10}}),
    bbp::sonata::Selection({{2, 8}, {10, 32}})
  };

  check_one_dimensional<float>(reader, selections[rank]);
  check_one_dimensional<double>(reader, selections[rank]);
  check_one_dimensional<uint64_t>(reader, selections[rank]);

  check_two_dimensional<std::array<uint64_t, 2>>(reader, selections[rank]);
}
