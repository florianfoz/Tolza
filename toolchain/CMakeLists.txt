cmake_minimum_required(VERSION 3.28)

project(tolza
    VERSION 2026.10
    LANGUAGES C CXX
)

# ----------------------------
# Language standards
# ----------------------------
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_EXPORT_COMPILE_COMMANDS ON)

# ----------------------------
# Project metadata
# ----------------------------
set(SOFTWARE_NAME "tolza")
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
# Toolchain core library
# ----------------------------
add_library(tolza-core STATIC
    ${TOLZA_SOURCES}
)

target_include_directories(tolza-core
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
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
)


# ----------------------------
# Main executable
# ----------------------------
add_executable(tolza
    "${TOLZA_MAIN}"
)

# PCH generated once for tolza-core
target_precompile_headers(tolza-core
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/pch.hpp"
)

# Reuse the same PCH in the main executable
target_precompile_headers(tolza
    REUSE_FROM tolza-core
)

# Root for project includes
target_include_directories(tolza
    PUBLIC
        tolza-core
        tolza-common
)

target_link_libraries(tolza
    PUBLIC
        tolza-core
        tolza-common
)

target_include_directories(tolza
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

set_target_properties(tolza PROPERTIES
    OUTPUT_NAME "${SOFTWARE_NAME}"
)


# ----------------------------
# Installation
# ----------------------------
include(GNUInstallDirs)

install(
    TARGETS tolza
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
