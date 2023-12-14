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

Because both ``libsonata`` and ``libsonata-mpi`` depend on HDF5 they need to be
linked against the same version of HDF5. Hence, effectively both ``libsonata``
and ``libsonata-mpi`` need to be built from source.

Manually
^^^^^^^^

Clone both repositories (with ``--recursive``) and then use:

::

   pip install .

in both repositories.

Spack
^^^^^

There's a spack recipe which allows the following, preferably inside
a spack environment.

::

   spack add libsonata-mpi
   spack install

C++
~~~

Ensure a reasonably modern C++ compiler and MPI are installed/loaded.
Then perform:

::

   cmake -DSONATA_MPI_FROM_SUBMODULES=On -DCMAKE_BUILD_TYPE=Release -B build
   cmake --build build --parallel

customize by using:

* ``CMAKE_INSTALL_PREFIX`` to pick an installation directory.
* ``CMAKE_PREFIX_PATH`` to help CMake find locally installed dependencies.
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

Acknowledgements
----------------
The development of this software was supported by funding to the Blue Brain Project, a research center of the École polytechnique fédérale de Lausanne (EPFL), from the Swiss government’s ETH Board of the Swiss Federal Institutes of Technology.

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
