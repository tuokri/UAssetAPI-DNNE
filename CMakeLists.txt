cmake_minimum_required(VERSION 4.0)
project(UAssetAPI-DNNE C CXX)

include(CMakePrintHelpers)

set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CMAKE_COMMAND} -E time")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(UASSETAPI_DNNE_DOTNET_CONFIGURATION Debug)
else ()
    set(UASSETAPI_DNNE_DOTNET_CONFIGURATION Release)
endif ()

option(UASSETAPI_DNNE_BUILD_EXPORTS "Run dotnet build step for DNNE exports." OFF)
option(UASSETAPI_DNNE_EMIT_DNNE_PLATFORM_SOURCE_TO_OUT_DIR "Emit DNNE's platform.c to build dir." ON)

# TODO: do we need these to be configurable?
# TODO: if yes, maybe add option that skips this block?
if(WIN32)
    set(UASSETAPI_DNNE_FORCED_RUNTIME_IDENTIFIER "win-x64" CACHE STRING "UAssetAPI-DNNE build RuntimeIdentifier.")
else()
    set(UASSETAPI_DNNE_FORCED_RUNTIME_IDENTIFIER "linux-x64" CACHE STRING "UAssetAPI-DNNE build RuntimeIdentifier.")
endif()

set(UASSETAPI_DNNE_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")

set(
    UASSETAPI_DNNE_DOTNET_CMD
    dotnet build --verbosity detailed
    --configuration ${UASSETAPI_DNNE_DOTNET_CONFIGURATION}
    -p:ForcedRuntimeIdentifier=${UASSETAPI_DNNE_FORCED_RUNTIME_IDENTIFIER}
    -p:DnneBuildExports=${DNNE_BUILD_EXPORTS}
    -p:EmitDnnePlatformSourceToOutDir=${UASSETAPI_DNNE_EMIT_DNNE_PLATFORM_SOURCE_TO_OUT_DIR}
    --output ${UASSETAPI_DNNE_BINARY_DIR}
)

cmake_print_variables(UASSETAPI_DNNE_DOTNET_CONFIGURATION)
cmake_print_variables(UASSETAPI_DNNE_BUILD_EXPORTS)
cmake_print_variables(UASSETAPI_DNNE_FORCED_RUNTIME_IDENTIFIER)
cmake_print_variables(UASSETAPI_DNNE_DOTNET_CMD)

# TODO: after dotnet build, restructure generated .c and .h files.
#   --> Copy dnne.h, etc. to uassetapi_dnne subdir.

set(
    UASSETAPI_DNNE_C_SOURCES
    UAssetAPI-DNNE/src/c/uassetapi_dnne/result.c
    ${UASSETAPI_DNNE_BINARY_DIR}/ # TODO?
)

set(
    UASSETAPI_DNNE_INCLUDE_DIRS
    UAssetAPI-DNNE/src/c
    ${UASSETAPI_DNNE_BINARY_DIR}
)

add_library(
    uassetapi_dnne
    STATIC
    ${UASSETAPI_DNNE_C_SOURCES}
)

target_include_directories(
    uassetapi_dnne
    PUBLIC
    ${UASSETAPI_DNNE_INCLUDE_DIRS}
)
