cmake_minimum_required(VERSION 3.16)
cmake_policy(VERSION 3.16)

if(NOT SONATA_MPI_VERSION)
    execute_process(COMMAND git describe --tags
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                    RESULT_VARIABLE GIT_VERSION_FAILED
                    OUTPUT_VARIABLE GIT_PKG_VERSION_FULL
                    ERROR_VARIABLE GIT_VERSION_ERROR
                    OUTPUT_STRIP_TRAILING_WHITESPACE)
    if(GIT_VERSION_FAILED)
      message(
        FATAL_ERROR
          "Could not retrieve version from command 'git describe --tags'\n"
          ${GIT_VERSION_ERROR})
    endif()

# keep last line of command output
    string(REPLACE "\n"
                   ";"
                   GIT_PKG_VERSION_FULL
                   "${GIT_PKG_VERSION_FULL}")
   list(GET GIT_PKG_VERSION_FULL -1 SONATA_MPI_VERSION)
endif()

# keep MAJOR.MINOR.PATCH (PATCH being optional)
string(REGEX
       REPLACE "v?([0-9]+\\.[0-9]+(\\.[0-9]+)?).*"
               "\\1"
               SONATA_MPI_VERSION
               "${SONATA_MPI_VERSION}")

# Get "major.minor" from string "major.minor.version"
string(REGEX MATCH "^(.*)\\.[^.]*$" dummy ${SONATA_MPI_VERSION})
set(SONATA_MPI_VERSION_ABI ${CMAKE_MATCH_1})

project(sonata_mpi VERSION ${SONATA_MPI_VERSION})

option(SONATA_MPI_FROM_SUBMODULES "Use Git submodules for header-only dependencies" OFF)
option(SONATA_MPI_PYTHON "Build Python extensions" OFF)
option(SONATA_MPI_TESTS "Build tests" ON)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMake)
set(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY true)

set(SONATA_MPI_CXX_MINIMUM_STANDARD 14)
if(NOT "${CMAKE_CXX_STANDARD}")
  set(CMAKE_CXX_STANDARD ${SONATA_MPI_CXX_MINIMUM_STANDARD})
elseif("${CMAKE_CXX_STANDARD}" LESS SONATA_MPI_CXX_MINIMUM_STANDARD)
  message(FATAL_ERROR
    "SONATA_MPI requires at 'CMAKE_CXX_STANDARD=${SONATA_CXX_MINIMUM_STANDARD}' or newer.")
endif()
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SONATA_MPI_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)

# =============================================================================
# Dependencies
# =============================================================================

if (SONATA_MPI_FROM_SUBMODULES)
    add_subdirectory(extlib EXCLUDE_FROM_ALL)
else()
    if (SONATA_MPI_TESTS)
        find_package(Catch2 REQUIRED)
    endif()
    find_package(HighFive REQUIRED)
    find_package(sonata REQUIRED)
endif()

find_package(MPI REQUIRED)

# =============================================================================
# Targets
# =============================================================================

set(SONATA_MPI_SRC
    src/hdf5_collective_reader.cpp
)

add_library(sonata_mpi_shared SHARED ${SONATA_MPI_SRC})
add_library(sonata_mpi_static STATIC ${SONATA_MPI_SRC})

foreach(TARGET sonata_mpi_shared sonata_mpi_static)
    set_target_properties(${TARGET}
        PROPERTIES
            POSITION_INDEPENDENT_CODE ON
            CXX_VISIBILITY_PRESET hidden
            OUTPUT_NAME "sonata_mpi"
    )
    target_include_directories(${TARGET}
        PUBLIC
            $<BUILD_INTERFACE:${SONATA_MPI_INCLUDE_DIR}>
            $<INSTALL_INTERFACE:include>
    )
    target_link_libraries(${TARGET}
        PRIVATE HighFive
        PUBLIC MPI::MPI_C MPI::MPI_CXX
    )
endforeach(TARGET)

set_target_properties(sonata_mpi_shared
    PROPERTIES
        VERSION ${SONATA_MPI_VERSION}
        SOVERSION ${SONATA_MPI_VERSION_ABI}
)

target_link_libraries(sonata_mpi_shared
    PUBLIC sonata::sonata_shared
)

target_compile_definitions(sonata_mpi_shared
    PUBLIC SONATA_DLL
    PRIVATE SONATA_DLL_EXPORTS
)

target_link_libraries(sonata_mpi_static
    PUBLIC sonata::sonata_static
)

# =============================================================================
# Install
# =============================================================================

install(TARGETS sonata_mpi_shared
    EXPORT sonata_mpi-targets
    LIBRARY
        DESTINATION lib
    ARCHIVE
        DESTINATION lib
)

install(DIRECTORY ${SONATA_MPI_INCLUDE_DIR}/bbp
    DESTINATION include
)

install(FILES CMake/sonata_mpi-config.cmake
    DESTINATION share/sonata_mpi/CMake
)

install(EXPORT sonata_mpi-targets
    DESTINATION share/sonata_mpi/CMake
    NAMESPACE sonata_mpi::
)

# =============================================================================
# Testing
# =============================================================================

if (SONATA_MPI_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# =============================================================================
# Python bindings
# =============================================================================

if (SONATA_MPI_PYTHON)
    add_subdirectory(python)
endif()
