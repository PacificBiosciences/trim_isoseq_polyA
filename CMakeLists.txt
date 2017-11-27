######################################################
# CMake build script for TrimIsoseqPolyA executable.
######################################################

project(TrimIsoseqPolyA CXX C)
cmake_minimum_required(VERSION 2.8)

# project version
set(TrimIsoseqPolyA_MAJOR_VERSION 0)
set(TrimIsoseqPolyA_MINOR_VERSION 0)
set(TrimIsoseqPolyA_PATCH_VERSION 2)
set(TrimIsoseqPolyA_VERSION
        "${TrimIsoseqPolyA_MAJOR_VERSION}.${TrimIsoseqPolyA_MINOR_VERSION}.${TrimIsoseqPolyA_PATCH_VERSION}"
        )

# build time options
option(TrimIsoseqPolyA_build_tests "Build trim isoseq polyA unit tests." OFF)

# main project paths
set(TrimIsoseqPolyA_RootDir ${TrimIsoseqPolyA_SOURCE_DIR})
set(TrimIsoseqPolyA_SourceDir ${TrimIsoseqPolyA_RootDir}/src)
set(TrimIsoseqPolyA_TestsDir ${TrimIsoseqPolyA_RootDir}/tests)
set(TrimIsoseqPolyA_LibDir ${TrimIsoseqPolyA_RootDir}/lib)
set(TrimIsoseqPolyA_BinDir ${TrimIsoseqPolyA_RootDir}/bin)
file(MAKE_DIRECTORY ${TrimIsoseqPolyA_BinDir})

if (NOT TrimIsoseqPolyA_OutputDir)
    set(TrimIsoseqPolyA_OutputDir ${TrimIsoseqPolyA_RootDir})
endif ()

# shared and third-party paths
find_package(Threads)
find_package(Boost COMPONENTS system program_options iostreams REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

option(SUPPORT_COMPRESSED_INPUT "To support gzip and bzip2 compressed input" OFF)
if (SUPPORT_COMPRESSED_INPUT)
    add_definitions(-DTO_SUPPORT_COMPRESSED_INPUT)
    find_package(ZLIB REQUIRED)
    find_package(BZip2 REQUIRED)
endif ()

# shared CXX flags for src & tests
include(CheckCXXCompilerFlag)
set(TrimIsoseqPolyA_CXX_FLAGS " -g -std=c++11 -Wall")

# quash warnings
# quash clang warnings Boost
check_cxx_compiler_flag("-Wno-unused-local-typedefs" HAS_NO_UNUSED_LOCAL_TYPEDEF)
if (HAS_NO_UNUSED_LOCAL_TYPEDEF)
    set(TrimIsoseqPolyA_CXX_FLAGS "${TrimIsoseqPolyA_CXX_FLAGS} -Wno-unused-local-typedefs")
endif ()

check_cxx_compiler_flag("-Wdeprecated-declarations" HAS_DEPRECATED_DECLARATIONS)
if (HAS_DEPRECATED_DECLARATIONS)
    set(TrimIsoseqPolyA_CXX_FLAGS "${TrimIsoseqPolyA_CXX_FLAGS} -Wdeprecated-declarations")
endif ()

# CXX and LINKER FLAGS
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TrimIsoseqPolyA_CXX_FLAGS}")
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${TrimIsoseqPolyA_LINKER_FLAGS}")

# main exe src
add_subdirectory(src)

# testing
if (TrimIsoseqPolyA_build_tests)
    enable_testing()

    # Add dependencies including gtest and gmock as external projects.
    include(ExternalProject)

    # download and build gtest
    ExternalProject_Add(
            gtest
            URL http://googletest.googlecode.com/files/gtest-1.7.0.zip
            PREFIX ${CMAKE_BINARY_DIR}/gtest
            # Disable install step
            INSTALL_COMMAND ""
    )

    # Create a libgtest target to be used as a dpendency by test programs
    add_library(libgtest IMPORTED STATIC GLOBAL)
    add_dependencies(libgtest gtest)

    # Set gtest properties
    ExternalProject_Get_Property(gtest source_dir binary_dir)
    set_target_properties(libgtest PROPERTIES
            "IMPORTED_LOCATION" "${binary_dir}/libgtest.a"
            "IMPORTED_LOCATION" "${binary_dir}/libgtest_main.a"
            "IMPORTED_LINK_INTERFACE_LIBRARIES" ${CMAKE_THREAD_LIBS_INIT}
            )

    message(STATUS " gtest/include = " ${source_dir}/include)
    include_directories(
            "${source_dir}/include"
            "${source_dir}/src"
    )
    #add_subdirectory(${source_dir})

    #set(GTEST_SourceDir ${source_dir}) 
    #message (status " GTEST_SourceDir = " ${source_dir})

    # Download and build gmock
    ExternalProject_Add(
            gmock
            URL http://googlemock.googlecode.com/files/gmock-1.7.0.zip
            PREFIX ${CMAKE_CURRENT_BINARY_DIR}/gmock
            # Disable install step
            INSTALL_COMMAND ""
    )

    # create a libgmock target to be used as a dependency by test programs
    add_library(libgmock IMPORTED STATIC GLOBAL)
    add_dependencies(libgmock gmock)

    # set gmock properties
    ExternalProject_Get_Property(gmock source_dir binary_dir)
    set_target_properties(libgmock PROPERTIES
            "IMPORTED_LOCATION" "${binary_dir}/libgmock.a"
            "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
            )

    message(STATUS " gmock/include = " ${source_dir}/include)
    include_directories("${source_dir}/include")

    add_subdirectory(tests)
endif ()
