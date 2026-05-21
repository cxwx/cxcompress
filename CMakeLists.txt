cmake_minimum_required(VERSION 3.20 FATAL_ERROR)
set(CX_VER_MAJ 0) # major
set(CX_VER_MIN 5) # minor
set(CX_VER_PAT 0) # patch
set(CX_VER_SS ${CX_VER_MAJ}.${CX_VER_MIN}.${CX_VER_PAT})
message("Version: ${CX_VER_SS}")

project(cxfileio VERSION ${CX_VER_SS} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Dependencies
find_package(Boost REQUIRED COMPONENTS iostreams)
find_package(ROOT QUIET)

file(GLOB sources ${PROJECT_SOURCE_DIR}/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/*.hh)

if(ROOT_FOUND)
    include(${ROOT_USE_FILE})
    root_generate_dictionary(G__cxfileio cxFileIO.hh LINKDEF LinkDef.h)
    add_library(
        cxfileio
        SHARED
        ${sources}
        ${CMAKE_CURRENT_BINARY_DIR}/G__cxfileio.cxx
    )
else()
    add_library(cxfileio SHARED ${sources})
endif()

target_include_directories(cxfileio PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(cxfileio PUBLIC Boost::iostreams ${ROOT_LIBRARIES})
set_target_properties(
    cxfileio
    PROPERTIES VERSION ${CX_VER_SS} SOVERSION ${CX_VER_MAJ}
)

# Example
add_executable(example example.cc)
target_link_libraries(example PRIVATE cxfileio)

set(CMAKE_INSTALL_PREFIX $ENV{HOME}/software/install/)
install(FILES ${headers} DESTINATION include/cxfunc/)

if(ROOT_FOUND)
    install(
        FILES
            ${CMAKE_BINARY_DIR}/libcxfileio_rdict.pcm
            ${CMAKE_BINARY_DIR}/libcxfileio.rootmap
        DESTINATION lib/
    )
endif()
install(
    TARGETS cxfileio
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)
