cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

file(READ "VERSION.md" ver)
string(REGEX MATCH "MAJ ([0-9]*)" _ ${ver})
set(CX_VER_MAJ ${CMAKE_MATCH_1}) # major
string(REGEX MATCH "MIN ([0-9]*)" _ ${ver})
set(CX_VER_MIN ${CMAKE_MATCH_1}) # minor
string(REGEX MATCH "PAT ([0-9]*)" _ ${ver})
set(CX_VER_PAT ${CMAKE_MATCH_1}) # patch
set(CX_VER_SS ${CX_VER_MAJ}.${CX_VER_MIN}.${CX_VER_PAT})
message("Version: ${CX_VER_SS}")

project(
    cxfunc
    VERSION ${CX_VER_SS}
    DESCRIPTION "a list of tools made by cx"
    LANGUAGES CXX
)

set(CMAKE_BUILD_TYPE Release)
if(APPLE)
    set(CMAKE_CXX_STANDARD 23)
    message("apple")
    set(CMAKE_BUILD_TYPE Debug)
    set(CMAKE_VERBOSE_MAKEFILE OFF)
    set(CMAKE_MACOSX_RPATH 1)
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

    # cmake-lint: disable=C0103,C0307
    if(CMAKE_C_COMPILER_ID MATCHES "Clang\$")
        set(OpenMP_C_FLAGS "-Xclang -fopenmp")
        set(OpenMP_C_LIB_NAMES "omp")
        set(OpenMP_omp_LIBRARY omp)
    endif()

    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang\$")
        set(OpenMP_CXX_FLAGS "-Xclang -fopenmp")
        set(OpenMP_CXX_LIB_NAMES "omp")
        set(OpenMP_omp_LIBRARY omp)
    endif()
else()
    set(CMAKE_CXX_STANDARD 23)
endif()

# doxygen
find_package(Doxygen OPTIONAL_COMPONENTS dot mscgen dia)
if(DOXYGEN_FOUND)
    if(NOT DOXYGEN_DOT_EXECUTABLE)
        message(
            FATAL_ERROR
            "doxygen with no 'dot', install graphviz or set DOXYGEN_DOT_EXECUTABLE"
        )
    endif()
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
    set(DOXYGEN_GENERATE_HTML YES)
    set(DOXYGEN_GENERATE_MAN YES)
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
    message("Use doxygen with \"make doxygen\"")
    add_custom_target(
        doxygen
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating documentation"
        VERBATIM
    )
    add_custom_command(
        TARGET doxygen
        POST_BUILD
        COMMAND cd ${CMAKE_CURRENT_BINARY_DIR}/latex; pdflatex refman.tex
        COMMENT "cxcomment"
    )
    install(
        DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html
        DESTINATION share/doc/cxfunc
        OPTIONAL
    )
    install(
        FILES ${CMAKE_CURRENT_BINARY_DIR}/latex/refman.pdf
        DESTINATION share/doc/cxfunc
        OPTIONAL
    )
else()
    message("no Doxygen found")
endif()

# test for CPM
file(
    DOWNLOAD
        https://github.com/cpm-cmake/CPM.cmake/releases/download/v0.38.3/CPM.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake
    EXPECTED_HASH
        SHA256=cc155ce02e7945e7b8967ddfaff0b050e958a723ef7aad3766d368940cb15494
)
include(${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake)

if(APPLE)
    cmake_policy(SET CMP0167 NEW)
endif()
find_package(Boost COMPONENTS program_options iostreams filesystem REQUIRED)

find_package(spdlog REQUIRED)
if(APPLE)
    find_package(termcolor REQUIRED)
endif()
find_package(nlohmann_json REQUIRED)
find_package(yaml-cpp REQUIRED)
find_package(ryml REQUIRED)
find_package(tomlplusplus REQUIRED)

find_package(
    VTK
    COMPONENTS
        CommonColor
        CommonCore
        FiltersSources
        InteractionStyle
        RenderingContextOpenGL2
        RenderingCore
        RenderingFreeType
        RenderingGL2PSOpenGL2
        RenderingOpenGL2
)
if(NOT VTK_FOUND)
    message(FATAL_ERROR "not find VTK.")
endif()

list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
find_package(
    ROOT
    REQUIRED
        Core
        Imt
        RIO
        Net
        Hist
        Graf
        Graf3d
        Gpad
        ROOTVecOps
        Tree
        TreePlayer
        Rint
        Postscript
        Matrix
        Physics
        MathCore
        Thread
        MultiProc
        ROOTDataFrame
        FITSIO
)
if(NOT ROOT_FOUND)
    message(STATUS "ROOT Cern not found")
endif()

find_library(CXCORSIKAIO CorsikaIODict REQUIRED)

find_package(PkgConfig)
pkg_check_modules(Healpix_cxx REQUIRED healpix_cxx>=3.60)
pkg_check_modules(gumbo REQUIRED Gumbo>=0.13.0)

find_package(CURL REQUIRED)

option(CXG4PLOT "geant4 vis" ON)
if(CXG4PLOT)
    find_package(Geant4 11.2 REQUIRED ui_all vis_all)
else()
    find_package(Geant4 11.2 REQUIRED)
endif()

find_package(glaze)

set(CMAKE_INSTALL_PREFIX $ENV{HOME}/software/install/)
set(CX_INSTALL_HEADERS include/cxfunc/)
set(CX_INSTALL_HEADERS2 include/cxfunc/)
set(CX_INSTALL_LIBS lib)

include_directories(
    ${PROJECT_SOURCE_DIR}/
    ${PROJECT_SOURCE_DIR}/config/
    ${PROJECT_SOURCE_DIR}/base/ # for clangd
    ${PROJECT_SOURCE_DIR}/random/
    ${PROJECT_SOURCE_DIR}/rootio/
    ${Boost_INCLUDE_DIRS}/
    ${ROOT_INCLUDE_DIRS}/
    ${Geant4_INCLUDE_DIR}/
    ${YAML_CPP_INCLUDE_DIR}
    # ${CPR_INCLUDE_DIRS}/
    /opt/homebrew/include
    /usr/include/
    /usr/local/include/
    $ENV{HOME}/software/install/include
)
link_directories(
    /usr/lib/
    /usr/local/lib
    /opt/homebrew/lib
    $ENV{HOME}/software/install/lib
)

set(CMAKE_REQUIRED_INCLUDES "/opt/homebrew/include")
# BUG: CMake Error at
# /opt/homebrew/share/cmake/Modules/CheckIncludeFile.cmake:99 (try_compile):
# Unknown extension ".c" for file include(CheckIncludeFile)
# check_include_file("dbg.h" HAVE_DBG_H) if(NOT HAVE_DBG_H) # TODO: not found?
# why? message(STATUS "dbg.h not found") endif()

check_cxx_source_compiles(
    "
  #include <dbg.h>
  int main() { return 0; }
"
    HAVE_DBG_H2
)
if(NOT HAVE_DBG_H2)
    message(STATUS "dbg.h not found2")
endif()

add_subdirectory(tools/mergefile)
# add_subdirectory(tools/tevcat)
add_subdirectory(tools/cp)
add_subdirectory(tools/readhead)
# add_subdirectory(tools/rw)
# add_subdirectory(tools/opencv)  # TODO: c++23 new ROOT

file(
    GLOB sources
    base/*.cc
    ${PROJECT_SOURCE_DIR}/*.cc
    ${PROJECT_SOURCE_DIR}/*/*.cc
)
file(
    GLOB headers
    base/*.hh
    ${PROJECT_SOURCE_DIR}/*.hh
    ${PROJECT_SOURCE_DIR}/*/*.hh
)

set(LINK_LIBS
    ${ROOT_LIBRARIES}
    cxthetaphi
    CorsikaIODict
    CCfits
    healpix_cxx
    sharp
    # ${CPR_LIBRARY}} cpr
    spdlog::spdlog
    ${Boost_LIBRARIES}
    # ${YAML_CPP_LIBRARIES} # not worked yet
    yaml-cpp::yaml-cpp
    ryml::ryml
    tomlplusplus
    CURL::libcurl
    # ${VTK_LIBRARIES}
    gumbo
)

# file(GLOB g4_sources g4/*.cc)
# file(GLOB g4_headers g4/*.hh)
# list(APPEND sources ${g4_sources})
# list(APPEND headers ${g4_headers})
list(APPEND LINK_LIBS ${Geant4_LIBRARIES})

file(COPY ${PROJECT_SOURCE_DIR}/usedfile DESTINATION ./)

add_library(cxfunc SHARED ${sources} ${headers})
target_link_libraries(cxfunc ${LINK_LIBS})
set_target_properties(
    cxfunc
    PROPERTIES VERSION ${CX_VER_SS} SOVERSION ${CX_VER_MAJ}
)

add_executable(p1_test setROOT/p1_test.cpp)
target_link_libraries(p1_test cxfunc ${LINK_LIBS})

add_executable(cxFITStoROOT fitsToROOT/cxFitsToRoot.cpp)
target_link_libraries(cxFITStoROOT cxfunc)

add_executable(cximg2png cximg2png.cpp)
target_link_libraries(cximg2png cxfunc)

add_executable(cxRandTest timer/cxRandCompare.cpp)
target_link_libraries(cxRandTest cxfunc ${LINK_LIBS})
add_executable(cxHeaderTest timer/cxHeaderTest.cpp)
target_link_libraries(cxHeaderTest cxfunc ${LINK_LIBS})

add_executable(cxMerge merge/cxMerge.cpp)
target_link_libraries(cxMerge cxfunc ${LINK_LIBS})

# TODO: remove G4

install(FILES ${headers} DESTINATION ${CX_INSTALL_HEADERS})
install(FILES ${headers} DESTINATION ${CX_INSTALL_HEADERS2})
install(
    TARGETS cxMerge cxHeaderTest cxfunc cximg2png cxFITStoROOT cxRandTest
    EXPORT ${PROJECT_NAME}Targets
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION ${CX_INSTALL_LIBS}
    ARCHIVE DESTINATION ${CX_INSTALL_LIBS}
)
