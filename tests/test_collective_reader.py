from mpi4py import MPI
import libsonata

def test_make_collective_reader():
    reader = libsonata.make_collective_reader(MPI.COMM_WORLD, True, True)
    assert type(reader) == libsonata.Hdf5Reader
