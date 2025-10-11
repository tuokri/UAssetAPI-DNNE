# Just for testing and verification.

cmake_minimum_required(VERSION 3.31)
project(UAssetAPI-DNNE-result)

set(CMAKE_C_STANDARD 23)

add_library(
    UAssetAPI-DNNE-result
    STATIC
    uassetapi_dnne/result.c
)

target_include_directories(
    UAssetAPI-DNNE-result
    PUBLIC
    ./
)

if (MSVC)
    set(COMPILE_OPTIONS "/W4")
else ()
    set(COMPILE_OPTIONS "-Wall;-pedantic;-Wpedantic;-Wextra;-Wcast-align;-Wcast-qual;-Wctor-dtor-privacy;-Wdisabled-optimization;-Wformat=2;-Winit-self;-Wlogical-op;-Wmissing-declarations;-Wmissing-include-dirs;-Wnoexcept;-Wold-style-cast;-Woverloaded-virtual;-Wredundant-decls;-Wshadow;-Wsign-conversion;-Wsign-promo;-Wstrict-null-sentinel;-Wswitch-default;-Wundef;-Wno-unused;-fimplicit-constexpr;-fconcepts-diagnostics-depth=5")
endif ()

target_compile_options(
    UAssetAPI-DNNE-result
    PRIVATE
    ${COMPILE_OPTIONS}
)
