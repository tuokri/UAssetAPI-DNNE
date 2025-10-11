# Just for testing and verification.

cmake_minimum_required(VERSION 4.0)
project(UAssetAPI-DNNE-result)

set(CMAKE_C_STANDARD 23)

add_library(
    UAssetAPI-DNNE-result
    STATIC
    uassetapi_dnne/result/result.c
)

target_include_directories(
    UAssetAPI-DNNE-result
    PUBLIC
    ./
)
