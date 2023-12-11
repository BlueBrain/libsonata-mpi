set(SONATA_MPI_TEST_SOURCES
  unit_tests.cpp
  test_collective_reader.cpp
)

add_executable(unit_tests ${SONATA_MPI_TEST_SOURCES})
target_link_libraries(unit_tests
    PRIVATE
    sonata_mpi_shared
    sonata_shared
    HighFive
    Catch2::Catch2
)

add_test(NAME unit_tests COMMAND mpirun -n 2 ${CMAKE_CURRENT_BINARY_DIR}/unit_tests WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
