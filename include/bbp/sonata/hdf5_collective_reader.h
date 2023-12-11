/*************************************************************************
 * Copyright (C) 2023 Blue Brain Project
 *
 * This file is part of 'libsonata-mpi', distributed under the terms
 * of the GNU Lesser General Public License version 3.
 *
 * See top-level COPYING.LESSER and COPYING files for details.
 *************************************************************************/

#pragma once

#include <mpi.h>

#include <bbp/sonata/common.h>
#include <bbp/sonata/hdf5_reader.h>

namespace bbp {
namespace sonata {

Hdf5Reader SONATA_API make_collective_reader(MPI_Comm comm,
                                  bool collective_metadata,
                                  bool collective_transfer);

}
}  // namespace bbp
