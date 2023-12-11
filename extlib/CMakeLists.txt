if (SONATA_MPI_TESTS)
    add_subdirectory(Catch2)
endif()

set(EXTLIB_FROM_SUBMODULES On)
set(SONATA_TESTS Off)
add_subdirectory(libsonata)
