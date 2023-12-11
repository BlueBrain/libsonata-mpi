set(SONATA_PYTHON_BUILD ${CMAKE_CURRENT_BINARY_DIR}/libsonata)

if (SONATA_MPI_FROM_SUBMODULES)
    add_subdirectory(pybind11 EXCLUDE_FROM_ALL)
else()
    find_package(pybind11)
endif()

pybind11_add_module(sonata_mpi_python SYSTEM bindings.cpp)

set_target_properties(sonata_mpi_python PROPERTIES
    OUTPUT_NAME "_libsonata_mpi"
)

target_include_directories(sonata_mpi_python PRIVATE ${SONATA_MPI4PY_INCLUDE_DIR})

target_link_libraries(sonata_mpi_python
    PRIVATE sonata_mpi_static
    PRIVATE HighFive
    PRIVATE pybind11::module
    PRIVATE MPI::MPI_C MPI::MPI_CXX
)
