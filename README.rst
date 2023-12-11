MPI-IO backend for libsonata
============================

This provides an ``Hdf5Reader`` plugin that reads files using
collective MPI-IO.

Installation
------------

Python
~~~~~~

Since ``libsonata_mpi`` depends on MPI, it can’t be built as a wheel and must
be installed from source. Therefore, a reasonably modern C++ compiler and MPI
must already be installed.

Typically this package is installed as an optional dependency of ``libsonata``:

::

   pip install libsonata[mpi]

To directly install ``libsonata_mpi`` without ``libsonata`` (which is utterly
useless), perform:

::

   pip install libsonata_mpi

C++
~~~

Ensure a reasonably modern C++ compiler and MPI are installed/loaded.
Then perform:

::

   cmake -DSONATA_MPI_FROM_SUBMODULES=On -DCMAKE_BUILD_TYPE=Release -B build
   cmake --build build --parallel

customize by using:

* ``CMAKE_INSTALL_PREFIX`` to pick an installation directory.
* ``SONATA_MPI_FROM_SUBMODULES`` to configure if dependencies are picked up
  from Git submodules.

Usage
-----

Python
~~~~~~

::

   from libsonata import make_collective_reader

   hdf5_reader = libsonata.make_collective_reader(
       comm,
       collective_metadata,
       collective_transfer,
   )
   edges = libsonata.EdgeStorage(edge_filename, hdf5_reader)

If available, this will use MPI-IO with collective I/O when reading datasets
``collective_transfer == True`` or HDF5 metadata ``collective_metadata ==
True``. The communicator must be an MPI4Py communicator. If ``libsonata`` fails
to import ``libsonata_mpi``, it’ll return the default reader. To check
if collective I/O is working use Darshan or set the environment variable
``ROMIO_PRINT_HINTS=1``.


License
-------

libsonata is distributed under the terms of the GNU Lesser General Public License version 3,
unless noted otherwise, for example, for external dependencies.
Refer to `COPYING.LESSER` and `COPYING` files for details.

Copyright (c) 2023 Blue Brain Project/EPFL

libsonata is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License version 3
as published by the Free Software Foundation.

libsonata is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with libsonata.  If not, see <https://www.gnu.org/licenses/>.
