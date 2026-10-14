cmake_minimum_required(VERSION 3.28)


# ----------------------------
# Language standards
# ----------------------------
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(external/enumlite)
add_subdirectory(external/tomlplusplus)
add_subdirectory(external/cli11)


# Automatically collect implementation files.
file(GLOB_RECURSE TOLZA_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

# ----------------------------
# common core library
# ----------------------------
add_library(tolza-common STATIC
    ${TOLZA_SOURCES}
)

target_include_directories(tolza-common
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(tolza-common
    PUBLIC
        CLI11::CLI11
        tomlplusplus::tomlplusplus
        enumlite::enumlite
)


target_precompile_headers(tolza-common
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/pch.hpp"
)

