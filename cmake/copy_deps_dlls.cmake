# include(CMakePrintHelpers)

set(DLL_NAMES "")

cmake_path(SET INPUT_JSON_FILE_PATH NORMALIZE ${INPUT_JSON_FILE_PATH})
message(STATUS "copy_deps_dlls: INPUT_JSON_FILE_PATH=${INPUT_JSON_FILE_PATH}")
file(READ ${INPUT_JSON_FILE_PATH} JSON_CONTENT)

# Extract the top-level "targets" object.
string(JSON TARGETS_OBJ GET ${JSON_CONTENT} "targets")
# # cmake_print_variables(TARGETS_OBJ)

# Get all keys (targets) under "targets".
set(TARGET_KEYS "")
string(JSON TARGETS_OBJ_LENGTH LENGTH ${TARGETS_OBJ})
math(EXPR TARGETS_OBJ_LENGTH "${TARGETS_OBJ_LENGTH}-1")
foreach (i RANGE ${TARGETS_OBJ_LENGTH})
    string(JSON TARGET_KEY MEMBER ${TARGETS_OBJ} ${i})
    # cmake_print_variables(TARGET_KEY)
    list(APPEND TARGET_KEYS ${TARGET_KEY})
endforeach ()
# cmake_print_variables(TARGET_KEYS)

foreach (TARGET_KEY ${TARGET_KEYS})
    # Extract each target object.
    string(JSON TARGET_OBJ GET "${TARGETS_OBJ}" "${TARGET_KEY}")

    # Get all packages under this target.
    set(PACKAGE_KEYS "")
    string(JSON TARGET_OBJ_LENGTH LENGTH ${TARGET_OBJ})
    if (TARGET_OBJ_LENGTH LESS_EQUAL 0)
        continue()
    endif ()
    math(EXPR TARGET_OBJ_LENGTH "${TARGET_OBJ_LENGTH}-1")
    foreach (i RANGE ${TARGET_OBJ_LENGTH})
        string(JSON PACKAGE_KEY MEMBER ${TARGET_OBJ} ${i})
        # cmake_print_variables(PACKAGE_KEY)
        list(APPEND PACKAGE_KEYS ${PACKAGE_KEY})
    endforeach ()

    foreach (PKG ${PACKAGE_KEYS})
        string(JSON PACKAGE_OBJ GET "${TARGET_OBJ}" "${PKG}")

        # If it has a "runtime" field.
        string(JSON RUNTIME_OBJ ERROR_VARIABLE NO_RUNTIME GET "${PACKAGE_OBJ}" "runtime")
        # cmake_print_variables(RUNTIME_OBJ)
        if (NOT NO_RUNTIME)
            string(JSON DLL_PATH MEMBER "${RUNTIME_OBJ}" 0)

            if (DLL_PATH MATCHES ".*.dll$")
                # Strip path prefixes, keep only filename.
                get_filename_component(DLL_FILE "${DLL_PATH}" NAME)
                list(APPEND DLL_NAMES "${DLL_FILE}")
            endif ()
        endif ()
    endforeach ()
endforeach ()

list(REMOVE_DUPLICATES DLL_NAMES)
# message(STATUS "copy_deps_dlls: DLL_NAMES=${DLL_NAMES}")

# TODO: these do not get cleaned.
foreach (DLL_NAME ${DLL_NAMES})
    set(DLL_SRC "${DLL_SOURCE_DIR}/${DLL_NAME}")
    set(DLL_DST "${DLL_DEST_DIR}/${DLL_NAME}")
    if (EXISTS ${DLL_SRC})
        message(STATUS "copy: '${DLL_SRC}' -> '${DLL_DST}'")
        file(COPY_FILE ${DLL_SRC} ${DLL_DST} ONLY_IF_DIFFERENT)
        configure_file(${DLL_SRC} ${DLL_DST})
    else ()
        message(WARNING "DLL file '${DLL_SRC}' does not exist!")
    endif ()

    get_filename_component(DLL_WLE "${DLL_NAME}" NAME_WLE)
    # message(STATUS DLL_WLE=${DLL_WLE})
    set(DLL_PDB "${DLL_SOURCE_DIR}/${DLL_WLE}.pdb")
    # message(STATUS DLL_PDB=${DLL_PDB})
    if (EXISTS ${DLL_PDB})
        get_filename_component(DLL_PDB_NAME ${DLL_PDB} NAME)
        message(STATUS "copy: '${DLL_PDB}' -> '${DLL_DEST_DIR}/${DLL_PDB_NAME}'")
        file(COPY_FILE ${DLL_PDB} ${DLL_DEST_DIR}/${DLL_PDB_NAME} ONLY_IF_DIFFERENT)
        configure_file(${DLL_PDB} ${DLL_DEST_DIR}/${DLL_PDB_NAME})
    endif ()
endforeach ()
