#include <catch2/catch_test_macros.hpp>

#include <highfive/highfive.hpp>

TEST_CASE("EmptyCollectiveHDF5", "[runme]") {
    int mpi_rank = -1;
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);

    std::string filename = "tests/data/test_data.h5";

    bool collective_metadata = true;
    bool collective_transfer = true;

    HighFive::FileAccessProps fapl;
    fapl.add(HighFive::MPIOFileAccess{MPI_COMM_WORLD, MPI_INFO_NULL});
    fapl.add(HighFive::MPIOCollectiveMetadata{collective_metadata});

    auto file = HighFive::File(filename, HighFive::File::ReadOnly, fapl);

    auto grp = file.getGroup("one_dim");
    auto dset = grp.getDataSet("float64");

    HighFive::DataTransferProps dxpl;
    dxpl.add(HighFive::UseCollectiveIO{collective_transfer});

    hid_t space_id = H5Dget_space(dset.getId());

    // std::cout << "mpi_rank = " << mpi_rank << std::endl;
    // H5Sselect_none(space_id);
    // if(mpi_rank == 0) {
    //     hsize_t offset[] = {0};
    //     hsize_t count[] = {2};
    //     H5Sselect_hyperslab(space_id, H5S_SELECT_OR, offset, nullptr, count, nullptr);
    // }

    // double * buf = (double*) malloc(1024 * sizeof(double));
    // H5Dread(dset.getId(), H5T_NATIVE_DOUBLE, H5S_ALL, space_id, dxpl.getId(), buf);

    auto hyperslab = HighFive::HyperSlab();
    hid_t mem_space_id;

    if(mpi_rank == 0) {
        hyperslab |= HighFive::RegularHyperSlab({0ul}, {2ul});

        hsize_t dims[] = {2ul};
        mem_space_id = H5Screate_simple(1, dims, nullptr);
    }
    else {
        hsize_t dims[] = {0ul};
        mem_space_id = H5Screate_simple(1, dims, nullptr);
    }

    auto dataspace = hyperslab.apply(dset.getSpace());
    // double * buf = (double*) malloc(1024 * sizeof(double));
    // H5Dread(dset.getId(), H5T_NATIVE_DOUBLE, mem_space_id, dataspace.getId(), dxpl.getId(), buf);

    dset.select(hyperslab).template read<std::vector<double>>(dxpl);

    H5Sclose(space_id);
    H5Sclose(mem_space_id);
}
