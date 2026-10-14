cmake_minimum_required(VERSION 3.28)

project(tolza-compiler
    VERSION 2026.10
    LANGUAGES C CXX
)

# ----------------------------
# Global options
# ----------------------------
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ----------------------------
# Project metadata
# ----------------------------
set(SOFTWARE_NAME "tolza-compiler")
string(TIMESTAMP SOFTWARE_VERSION "%Y-%m-%d")
string(TIMESTAMP TOLZA_VERSION "%Y-%m")

set(SOFTWARE_VENDOR "CLOVIS")

# ----------------------------
# Dependencies
# ----------------------------
add_subdirectory(
    "${CMAKE_CURRENT_SOURCE_DIR}/../common"
    "${CMAKE_CURRENT_BINARY_DIR}/tolza-common"
)

add_subdirectory(external/json)

find_package(LLVM REQUIRED CONFIG)
find_package(Clang REQUIRED CONFIG)


# ----------------------------
# Sources
# ----------------------------

# Entry point: only in the executable.
set(TOLZA_MAIN
    "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp"
)

# Automatically collect implementation files.
# main.cpp must not be compiled into the library.
file(GLOB_RECURSE TOLZA_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

list(FILTER TOLZA_SOURCES EXCLUDE REGEX "/main\\.cpp$")


# ----------------------------
# Compiler core library
# ----------------------------
add_library(tolza-core STATIC
    ${TOLZA_SOURCES}
)

target_include_directories(tolza-core
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
    PRIVATE
        ${LLVM_INCLUDE_DIRS}
        ${CLANG_INCLUDE_DIRS}
)

target_compile_definitions(tolza-core
    PUBLIC
        SOFTWARE_NAME="${SOFTWARE_NAME}"
        SOFTWARE_VERSION="${SOFTWARE_VERSION}"
        TOLZA_VERSION="${TOLZA_VERSION}"
    PRIVATE
        ${LLVM_DEFINITIONS}
)

# Dependencies used by the core implementation.
target_link_libraries(tolza-core
    PUBLIC
        tolza-common
        nlohmann_json::nlohmann_json
        LLVM-19
        clang-19
)


# ----------------------------
# Main executable
# ----------------------------
add_executable(tolza-compiler
    "${TOLZA_MAIN}"
)

# PCH generated once for tolza-core
target_precompile_headers(tolza-core
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/pch.hpp"
)

# Reuse the same PCH in the main executable
target_precompile_headers(tolza-compiler
    REUSE_FROM tolza-core
)

target_link_libraries(tolza-compiler
    PUBLIC
        tolza-core
        tolza-common
        nlohmann_json::nlohmann_json
        LLVM-19
        clang-19
)

target_include_directories(tolza-compiler
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
        ${LLVM_INCLUDE_DIRS}
        ${CLANG_INCLUDE_DIRS}
)

target_compile_definitions(tolza-compiler
    PRIVATE
        ${LLVM_DEFINITIONS}
)

set_target_properties(tolza-compiler PROPERTIES
    OUTPUT_NAME "${SOFTWARE_NAME}"
)


# ----------------------------
# Installation
# ----------------------------
include(GNUInstallDirs)

install(TARGETS tolza-compiler
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
)


# ----------------------------
# Tests
# ----------------------------
option(TOLZA_TESTS "Build Tolza tests" OFF)

if(TOLZA_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
