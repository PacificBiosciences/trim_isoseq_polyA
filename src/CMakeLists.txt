include_directories(
        .
        ${Boost_INCLUDE_DIRS}
        ${ZLIB_INCLUDE_DIRS}
)

# source files
set(LIB_SOURCE_FILES
        char_traits.hpp
        fasta.hpp
        fastq.hpp
        format.hpp
        hmm_model.cpp
        hmm_model.hpp
        hmm_utilities.h
        kernel_color.h
        matrix.hpp
        polyA_hmm_model.cpp
        polyA_hmm_model.hpp
        quality.hpp
        sequence.hpp
        type_policy.h
        thread.hpp
        )

set(EXE_SOURCE_FILES
        ${LIB_SOURCE_FILES}
        main.cpp
        )

# CXX and LINKER FLAGS
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TrimIsoseqPolyA_CXX_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${TrimIsoseqPolyA_EXE_LINKER_FLAGS}")

add_library(trima ${LIB_SOURCE_FILES})

target_include_directories(trima PUBLIC ${TrimIsoseqPolyA_SourceDir})

set_target_properties(trima PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${TrimIsoseqPolyA_LibDir}
        RUNTIME_OUTPUT_DIRECTORY ${TrimIsoseqPolyA_LibDir}
        LIBRARY_OUTPUT_DIRECTORY ${TrimIsoseqPolyA_LibDir}
        )

if (TrimIsoseqPolyA_build_bin)
        add_executable(trim_isoseq_polyA ${EXE_SOURCE_FILES})
        set_target_properties(trim_isoseq_polyA PROPERTIES
                RUNTIME_OUTPUT_DIRECTORY ${TrimIsoseqPolyA_BinDir}
                )

        if (NOT APPLE)
        set(MY_LIBRT -lrt)
        else ()
        endif ()

        message(STATUS "Boost_LIBRARIES: " ${Boost_LIBRARIES})

        target_link_libraries(trim_isoseq_polyA
                ${Boost_LIBRARIES}
                ${ZLIB_LIBRARIES}
                ${BZIP2_LIBRARIES}
                ${CMAKE_THREAD_LIBS_INIT}
                #${MY_LIBRT}
                )
endif()