if (SONATA_MPI_TESTS)
    add_subdirectory(Catch2)
endif()

set(EXTLIB_FROM_SUBMODULES On)
set(SONATA_TESTS Off)
set(SONATA_CXX_WARNINGS Off)
add_subdirectory(libsonata)
