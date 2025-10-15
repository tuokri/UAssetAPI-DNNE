cmake_minimum_required(VERSION 4.0)
project(UAssetAPI-DNNE CXX)

include(CMakePrintHelpers)

set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CMAKE_COMMAND} -E time")

# To make things simpler, instruct .exe, .dll, and .pdb files to be placed
# in the same directory. Also get rid of Visual Studio generator's default
# Debug and Release directories to make sure all of our executable and library
# files get placed in the same directory.
if (MSVC)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
    set(CMAKE_PDB_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
    cmake_print_variables(CMAKE_LIBRARY_OUTPUT_DIRECTORY)
    cmake_print_variables(CMAKE_RUNTIME_OUTPUT_DIRECTORY)
    cmake_print_variables(CMAKE_PDB_OUTPUT_DIRECTORY)
    cmake_print_variables(CMAKE_ARCHIVE_OUTPUT_DIRECTORY)

    foreach (CONFIG_TYPE ${CMAKE_CONFIGURATION_TYPES})
        string(TOUPPER ${CONFIG_TYPE} CONFIG_TYPE)
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONFIG_TYPE} ${CMAKE_BINARY_DIR})
        set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONFIG_TYPE} ${CMAKE_BINARY_DIR})
        set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CONFIG_TYPE} ${CMAKE_BINARY_DIR})
    endforeach ()
endif ()

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(UASSETAPI_DNNE_DOTNET_CONFIGURATION Debug)
else ()
    set(UASSETAPI_DNNE_DOTNET_CONFIGURATION Release)
endif ()

option(UASSETAPI_DNNE_BUILD_EXPORTS "Run dotnet build step for DNNE exports." OFF)
# TODO: these 2 names are misleading.
# See: https://github.com/AaronRobinsonMSFT/DNNE/blob/363a27a004c5e46b66cb39f6eaba756eed5edb27/src/msbuild/DNNE.BuildTasks/CreateCompileCommand.cs#L81-L82
option(UASSETAPI_DNNE_EMIT_DNNE_PLATFORM_SOURCE_C_TO_OUT_DIR "Emit DNNE's platform.c (C) to build dir." ON)
option(UASSETAPI_DNNE_EMIT_DNNE_PLATFORM_SOURCE_CPP_TO_OUT_DIR "Emit DNNE's platform_4.cpp (C++) to build dir." OFF)
option(UASSETAPI_DNNE_GENERATE_CPP "If set, generate CPP files with CodeGen tool, else C." OFF)

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

set(UASSETAPI_DNNE_BINARY_DIR "${CMAKE_BINARY_DIR}/generated")

set(
    UASSETAPI_DNNE_DOTNET_CMD
    dotnet build --verbosity normal
    --configuration ${UASSETAPI_DNNE_DOTNET_CONFIGURATION}
    -p:ForcedRuntimeIdentifier=${UASSETAPI_DNNE_FORCED_RUNTIME_IDENTIFIER}
    -p:DnneBuildExports=${DNNE_BUILD_EXPORTS}
    -p:EmitDnnePlatformSourceCToOutDir=${UASSETAPI_DNNE_EMIT_DNNE_PLATFORM_SOURCE_C_TO_OUT_DIR}
    -p:EmitDnnePlatformSourceCPPToOutDir=${UASSETAPI_DNNE_EMIT_DNNE_PLATFORM_SOURCE_CPP_TO_OUT_DIR}
    -p:GenerateCPP=${UASSETAPI_DNNE_GENERATE_CPP}
    -p:CMakeOutDir="${UASSETAPI_DNNE_BINARY_DIR}"
    --output "${UASSETAPI_DNNE_BINARY_DIR}"
)

file(
    GLOB_RECURSE
    UASSETAPI_DNNE_PROJECT_SOURCES
    ${PROJECT_SOURCE_DIR}/UAssetAPI-DNNE/*.cs
    ${PROJECT_SOURCE_DIR}/UAssetAPI-DNNE/*.csproj
    ${PROJECT_SOURCE_DIR}/UAssetAPI-DNNE/*.props
    ${PROJECT_SOURCE_DIR}/UAssetAPI-DNNE/*.c
    ${PROJECT_SOURCE_DIR}/UAssetAPI-DNNE/*.h
    ${PROJECT_SOURCE_DIR}/UAssetAPI-DNNE/*.packages.lock.json
    ${PROJECT_SOURCE_DIR}/UAssetAPI-DNNE/submodules/UAssetAPI/*.cs
    ${PROJECT_SOURCE_DIR}/UAssetAPI-DNNE/submodules/UAssetAPI/*.csproj
    ${PROJECT_SOURCE_DIR}/UAssetAPI-DNNE/submodules/UAssetAPI/*.props
    ${PROJECT_SOURCE_DIR}/UAssetAPI-DNNE.CodeGen/*.cs
    ${PROJECT_SOURCE_DIR}/UAssetAPI-DNNE.CodeGen/*.csproj
    ${PROJECT_SOURCE_DIR}/UAssetAPI-DNNE.CodeGen/*.props
)

cmake_print_variables(UASSETAPI_DNNE_DOTNET_CONFIGURATION)
cmake_print_variables(UASSETAPI_DNNE_BUILD_EXPORTS)
cmake_print_variables(UASSETAPI_DNNE_FORCED_RUNTIME_IDENTIFIER)
cmake_print_variables(UASSETAPI_DNNE_DOTNET_CMD)
cmake_print_variables(UASSETAPI_DNNE_BINARY_DIR)
cmake_print_variables(UASSETAPI_DNNE_EMIT_DNNE_PLATFORM_SOURCE_C_TO_OUT_DIR)
cmake_print_variables(UASSETAPI_DNNE_EMIT_DNNE_PLATFORM_SOURCE_CPP_TO_OUT_DIR)
cmake_print_variables(UASSETAPI_DNNE_PROJECT_SOURCES)
cmake_print_variables(UASSETAPI_DNNE_GENERATE_CPP)

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
    # NOTE: this is needed because uassetapi_dnne.h fails to find dnne.h
    #       when DNNE_COMPILE_AS_SOURCE is defined. It would be nice to avoid,
    #       but it is the best workaround currently.
    ${UASSETAPI_DNNE_BINARY_DIR}/uassetapi_dnne
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
    # TODO: ideally we would add all the .dlls, .pdbs, etc. generated by dotnet build!
    # TODO: run dotnet build at configure time, using execute_process?
    DEPENDS
    ${UASSETAPI_DNNE_PROJECT_SOURCES}
    COMMAND ${UASSETAPI_DNNE_DOTNET_CMD}
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/UAssetAPI-DNNE
)
# TODO: can we leverage the *.csproj.FileListAbsolute.txt files somehow?

execute_process(
    COMMAND dotnet msbuild
    -target:WriteDnneNetHostDirToFile
    -p:CMakeOutDir=${UASSETAPI_DNNE_BINARY_DIR}
    -p:DnneRuntimeIdentifier=${UASSETAPI_DNNE_FORCED_RUNTIME_IDENTIFIER}
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/UAssetAPI-DNNE
    COMMAND_ECHO STDOUT
    COMMAND_ERROR_IS_FATAL ANY
    TIMEOUT 30
)
file(STRINGS "${UASSETAPI_DNNE_BINARY_DIR}/DnneNetHostDir.txt" UASSETAPI_DNNE_NETHOST_DIR)
cmake_path(SET UASSETAPI_DNNE_NETHOST_DIR NORMALIZE ${UASSETAPI_DNNE_NETHOST_DIR})
message(STATUS "UASSETAPI_DNNE_NETHOST_DIR='${UASSETAPI_DNNE_NETHOST_DIR}'")

set_source_files_properties(
    ${UASSETAPI_DNNE_BINARY_DIR}/uassetapi_dnne/platform.c
    PROPERTIES LANGUAGE CXX
)

add_library(
    uassetapi_dnne
    STATIC
    ${UASSETAPI_DNNE_C_SOURCES}
    ${UASSETAPI_DNNE_GENERATED_SOURCES}
)

target_compile_definitions(
    uassetapi_dnne
    PUBLIC
    DNNE_ASSEMBLY_NAME=UAssetAPI-DNNE
    DNNE_COMPILE_AS_SOURCE
)

target_include_directories(
    uassetapi_dnne
    PUBLIC
    ${UASSETAPI_DNNE_INCLUDE_DIRS}
    PRIVATE
    ${UASSETAPI_DNNE_NETHOST_DIR}
)

cmake_path(
    SET
    UASSETAPI_DNNE_NETHOSTLIB_PATH
    NORMALIZE
    ${UASSETAPI_DNNE_NETHOST_DIR}/libnethost.lib
)

target_link_libraries(
    uassetapi_dnne
    PRIVATE
    ${UASSETAPI_DNNE_NETHOSTLIB_PATH}
)

add_dependencies(
    uassetapi_dnne
    uassetapi_dnne_codegen
)

set_target_properties(
    uassetapi_dnne
    PROPERTIES ADDITIONAL_CLEAN_FILES ${UASSETAPI_DNNE_BINARY_DIR}
)

# TODO: these run currently even if nothing was built.
# TODO: try to make these only run if something was actually built!
add_custom_command(
    TARGET uassetapi_dnne_codegen POST_BUILD
    COMMAND "${CMAKE_COMMAND}"
    -DDLL_SOURCE_DIR=${UASSETAPI_DNNE_BINARY_DIR}
    -DDLL_DEST_DIR=${CMAKE_BINARY_DIR}
    -DINPUT_JSON_FILE_PATH="${UASSETAPI_DNNE_BINARY_DIR}/UAssetAPI.deps.json"
    -P "${PROJECT_SOURCE_DIR}/cmake/copy_deps_dlls.cmake"
)
add_custom_command(
    TARGET uassetapi_dnne_codegen POST_BUILD
    COMMAND "${CMAKE_COMMAND}"
    -DDLL_SOURCE_DIR=${UASSETAPI_DNNE_BINARY_DIR}
    -DDLL_DEST_DIR=${CMAKE_BINARY_DIR}
    -DINPUT_JSON_FILE_PATH="${UASSETAPI_DNNE_BINARY_DIR}/UAssetAPI-DNNE.deps.json"
    -P "${PROJECT_SOURCE_DIR}/cmake/copy_deps_dlls.cmake"
)

add_custom_command(
    TARGET uassetapi_dnne_codegen POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy
    ${UASSETAPI_DNNE_BINARY_DIR}/UAssetAPI-DNNE.dll
    ${CMAKE_BINARY_DIR}/UAssetAPI-DNNE.dll
    COMMAND "${CMAKE_COMMAND}" -E copy
    ${UASSETAPI_DNNE_BINARY_DIR}/UAssetAPI-DNNE.runtimeconfig.json
    ${CMAKE_BINARY_DIR}/UAssetAPI-DNNE.runtimeconfig.json
)

# NOTE: libnethost is only provided with release CRT.
#       See: https://github.com/dotnet/runtime/issues/64087
# TODO: Fix later if libnethost debug variant is made available!
if (MSVC)
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options(
            uassetapi_dnne
            PUBLIC
            "/MT"
        )
    endif ()
endif ()
