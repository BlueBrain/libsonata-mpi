/*************************************************************************
 * Copyright (C) 2023 Blue Brain Project
 *
 * This file is part of 'libsonata-mpi', distributed under the terms
 * of the GNU Lesser General Public License version 3.
 *
 * See top-level COPYING.LESSER and COPYING files for details.
 *************************************************************************/

#include <catch2/catch_session.hpp>

#include <mpi.h>

int main(int argc, char* argv[]) {
  MPI_Init(&argc, &argv);

  int result = Catch::Session().run( argc, argv );


  MPI_Finalize();
  return result;
}
