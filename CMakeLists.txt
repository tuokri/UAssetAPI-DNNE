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
if (WIN32)
    set(
        UASSETAPI_DNNE_FORCED_RUNTIME_IDENTIFIER "win-x64"
        CACHE STRING "UAssetAPI-DNNE build RuntimeIdentifier.")
else ()
    set(
        UASSETAPI_DNNE_FORCED_RUNTIME_IDENTIFIER "linux-x64"
        CACHE STRING "UAssetAPI-DNNE build RuntimeIdentifier.")
endif ()

# TODO: this should really be passed in from a CMake preset!
# TODO: that would cross-compiling the dotnet project.
if (WIN32)
    set(DNNE_ASSEMBLY_NAME DNNE_WINDOWS)
elseif (LINUX)
    set(DNNE_ASSEMBLY_NAME DNNE_LINUX)
endif ()

set(UASSETAPI_DNNE_BINARY_DIR "${CMAKE_BINARY_DIR}/generated")

set(
    UASSETAPI_DNNE_DOTNET_CMD
    dotnet build --verbosity normal
    --configuration ${UASSETAPI_DNNE_DOTNET_CONFIGURATION}
    -p:ForcedRuntimeIdentifier=${UASSETAPI_DNNE_FORCED_RUNTIME_IDENTIFIER}
    -p:DnneBuildExports=${DNNE_BUILD_EXPORTS}
    -p:EmitDnnePlatformSourceToOutDir=${UASSETAPI_DNNE_EMIT_DNNE_PLATFORM_SOURCE_TO_OUT_DIR}
    -p:CMakeOutDir="${UASSETAPI_DNNE_BINARY_DIR}"
    --output "${UASSETAPI_DNNE_BINARY_DIR}"
)

file(
    GLOB_RECURSE
    UASSETAPI_DNNE_PROJECT_SOURCES
    ${PROJECT_SOURCE_DIR}/UAssetAPI-DNNE/**.cs
    ${PROJECT_SOURCE_DIR}/UAssetAPI-DNNE/**.csproj
    ${PROJECT_SOURCE_DIR}/UAssetAPI-DNNE/**.props
    ${PROJECT_SOURCE_DIR}/UAssetAPI-DNNE/**.c
    ${PROJECT_SOURCE_DIR}/UAssetAPI-DNNE/**.h
    ${PROJECT_SOURCE_DIR}/UAssetAPI-DNNE/**.packages.lock.json
    ${PROJECT_SOURCE_DIR}/UAssetAPI-DNNE/submodules/UAssetAPI/**.cs
    ${PROJECT_SOURCE_DIR}/UAssetAPI-DNNE/submodules/UAssetAPI/**.csproj
    ${PROJECT_SOURCE_DIR}/UAssetAPI-DNNE/submodules/UAssetAPI/**.props
    ${PROJECT_SOURCE_DIR}/UAssetAPI-DNNE.CodeGen/**.cs
    ${PROJECT_SOURCE_DIR}/UAssetAPI-DNNE.CodeGen/**.csproj
    ${PROJECT_SOURCE_DIR}/UAssetAPI-DNNE.CodeGen/**.props
)

cmake_print_variables(UASSETAPI_DNNE_DOTNET_CONFIGURATION)
cmake_print_variables(UASSETAPI_DNNE_BUILD_EXPORTS)
cmake_print_variables(UASSETAPI_DNNE_FORCED_RUNTIME_IDENTIFIER)
cmake_print_variables(UASSETAPI_DNNE_DOTNET_CMD)
cmake_print_variables(UASSETAPI_DNNE_BINARY_DIR)
cmake_print_variables(UASSETAPI_DNNE_EMIT_DNNE_PLATFORM_SOURCE_TO_OUT_DIR)
cmake_print_variables(UASSETAPI_DNNE_PROJECT_SOURCES)
cmake_print_variables(DNNE_ASSEMBLY_NAME)

set(
    UASSETAPI_DNNE_C_SOURCES
    ${PROJECT_SOURCE_DIR}/UAssetAPI-DNNE/src/c/uassetapi_dnne/result.c
)

set(
    UASSETAPI_DNNE_GENERATED_SOURCES
    ${UASSETAPI_DNNE_BINARY_DIR}/uassetapi_dnne/platform.c
)

set(
    UASSETAPI_DNNE_INCLUDE_DIRS
    ${PROJECT_SOURCE_DIR}/UAssetAPI-DNNE/src/c
    ${UASSETAPI_DNNE_BINARY_DIR}
)

set(
    UASSETAPI_DNNE_GENERATED_HEADERS
    ${UASSETAPI_DNNE_BINARY_DIR}/uassetapi_dnne/dnne.h
    ${UASSETAPI_DNNE_BINARY_DIR}/uassetapi_dnne/engineversion.h
    ${UASSETAPI_DNNE_BINARY_DIR}/uassetapi_dnne/uassetapi_dnne.h
)

add_custom_target(
    uassetapi_dnne_codegen
    ALL
    DEPENDS
    ${UASSETAPI_DNNE_GENERATED_SOURCES}
    ${UASSETAPI_DNNE_GENERATED_HEADERS}
)
add_custom_command(
    OUTPUT
    ${UASSETAPI_DNNE_GENERATED_SOURCES}
    ${UASSETAPI_DNNE_GENERATED_HEADERS}
    DEPENDS ${UASSETAPI_DNNE_PROJECT_SOURCES} ${UASSETAPI_DNNE_BINARY_DIR}/DnneNetHostDir.txt
    COMMAND ${UASSETAPI_DNNE_DOTNET_CMD}
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/UAssetAPI-DNNE
    COMMENT "UASSETAPI_DNNE_NETHOST_DIR=${UASSETAPI_DNNE_NETHOST_DIR}"
)

add_custom_target(
    uassetapi_dnne_read_nethost_dir_from_file
    ALL
    DEPENDS
    ${UASSETAPI_DNNE_BINARY_DIR}/DnneNetHostDir.txt
)
add_custom_command(
    OUTPUT ${UASSETAPI_DNNE_BINARY_DIR}/DnneNetHostDir.txt
    COMMAND msbuild -target:WriteDnneNetHostDirToFile -p:CMakeOutDir="${UASSETAPI_DNNE_BINARY_DIR}"
    # TODO: instead of using read_and_store.cmake here, we should probably use
    #       execute_process and read the file contents into an OUTPUT_VARIABLE!
    COMMAND ${CMAKE_COMMAND} -DINPUT_FILE="${UASSETAPI_DNNE_BINARY_DIR}/DnneNetHostDir.txt"
    -DDEST_VARIABLE=UASSETAPI_DNNE_NETHOST_DIR -P ${CMAKE_SOURCE_DIR}/cmake/read_and_store.cmake
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/UAssetAPI-DNNE
)

add_library(
    uassetapi_dnne
    STATIC
    ${UASSETAPI_DNNE_C_SOURCES}
    ${UASSETAPI_DNNE_GENERATED_SOURCES}
)

target_compile_definitions(
    uassetapi_dnne
    PRIVATE
    DNNE_ASSEMBLY_NAME=${DNNE_ASSEMBLY_NAME}
)

target_include_directories(
    uassetapi_dnne
    PUBLIC
    ${UASSETAPI_DNNE_INCLUDE_DIRS}
    ${UASSETAPI_DNNE_NETHOST_DIR}
)

# TODO: this is not right.
target_link_libraries(
    uassetapi_dnne
    PUBLIC
    UAssetAPI-DNNE
    PRIVATE
    UAssetAPI
    Newtonsoft.Json
    ZstdSharp
    nethost
)

add_dependencies(
    uassetapi_dnne
    uassetapi_dnne_read_nethost_dir_from_file
    uassetapi_dnne_codegen
)

set_target_properties(
    uassetapi_dnne
    PROPERTIES ADDITIONAL_CLEAN_FILES ${UASSETAPI_DNNE_BINARY_DIR}
)
