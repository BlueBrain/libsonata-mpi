#!/usr/bin/env python3

# Copyright (C) 2023 Blue Brain Project
#
# This file is part of 'libsonata-mpi', distributed under the terms
# of the GNU Lesser General Public License version 3.
#
# See top-level COPYING.LESSER and COPYING files for details.

import numpy as np
import h5py

filename = "test_data.h5"

dtypes = [("float32", np.float32), ("float64", np.float64), ("uint64", np.uint64)]

with h5py.File(filename, "w") as h5:
    grp = h5.create_group("one_dim")
    for dtype_name, dtype in dtypes:
        x = np.arange(32, dtype=dtype)
        grp[dtype_name] = x

    x = np.arange(32*2, dtype=np.uint64).reshape((32, 2))
    h5["two_dim"] = x
