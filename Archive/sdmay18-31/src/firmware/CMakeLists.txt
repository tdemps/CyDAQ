project( firmware )
cmake_minimum_required( VERSION 2.8 )
set( CMAKE_SYSTEM_NAME Generic  )
set( CMAKE_SYSTEM_PROCESSOR arm )

# Toolchain Settings
set( TOOLCHAIN_PREFIX   arm-none-eabi-              )
set( CMAKE_C_COMPILER   ${TOOLCHAIN_PREFIX}gcc      )
set( CMAKE_ASM_COMPILER ${TOOLCHAIN_PREFIX}as       )
set( CMAKE_AR           ${TOOLCHAIN_PREFIX}ar       )
set( CMAKE_OBJCOPY      ${TOOLCHAIN_PREFIX}objcopy  )
set( CMAKE_OBJDUMP      ${TOOLCHAIN_PREFIX}objdump  )

# Compiler Flags
set( CPU "-mcpu=cortex-m4" )
set( FPU "-mfpu=fpv4-sp-d16 -mfloat-abi=hard" )
set( CMAKE_ASM_FLAGS "-mthumb ${CPU} ${FPU} -MD" )
set( CMAKE_C_FLAGS "-mthumb ${CPU} ${FPU} -ffunction-sections -fdata-sections -MD -std=c99 -Wall -pedantic -g" )

set ( CMAKE_C_LINK_FLAGS "-Wl,--undefined=uxTopUsedPriorityy" )

if ( APPLE )
    string ( REPLACE "-Wl,-search_paths_first" "" CMAKE_C_LINK_FLAGS ${CMAKE_C_LINK_FLAGS} )
endif ()

# Linker Flags
set( CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "" )
set( CMAKE_EXE_LINKER_FLAGS "-T${PROJECT_SOURCE_DIR}/firmware.ld -specs=${PROJECT_SOURCE_DIR}/firmware.specs" )

# Processor Definitions
add_definitions( -DPART_TM4C123GH6PM        )
add_definitions( -DTARGET_IS_TM4C123_RB1    )
add_definitions( -Dgcc                      )

# Enable Assembler Support
enable_language( ASM )

# Paths
set( BUILD_PATH             "${PROJECT_SOURCE_DIR}/build"                   )
set( TIVAWARE_PATH          "${PROJECT_SOURCE_DIR}/libs"                    )
set( SOURCE_PATH            "${PROJECT_SOURCE_DIR}/src"                     )
set( FREERTOS_PATH          "${TIVAWARE_PATH}/third_party/FreeRTOS/Source"  )

# Include directories for headers
include_directories( ${TIVAWARE_PATH}                       )
include_directories( ${FREERTOS_PATH}/include               )
include_directories( ${FREERTOS_PATH}/portable/GCC/ARM_CM4F )
include_directories( ${SOURCE_PATH}                         )
include_directories( ${SOURCE_PATH}/drivers                 )

# Add sources from subdirectories
add_subdirectory( "${PROJECT_SOURCE_DIR}/libs/driverlib"                    "${BUILD_PATH}/libs/driverlib"                      )
add_subdirectory( "${PROJECT_SOURCE_DIR}/libs/third_party/FreeRTOS/Source"  "${BUILD_PATH}/libs/third_party/FreeRTOS/Source"    )
add_subdirectory( "${PROJECT_SOURCE_DIR}/libs/utils"                        "${BUILD_PATH}/libs/utils"                          )
add_subdirectory( "${PROJECT_SOURCE_DIR}/src"                               "${BUILD_PATH}/src"                                 )

# Build Executable
add_executable( ${PROJECT_NAME}.elf ${SRC_LIST} )

# Objcopy executable into a .bin
macro( OBJCOPY_FILE EXE_NAME )
    set( FO ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.bin )
    set( FI ${CMAKE_CURRENT_BINARY_DIR}/${EXE_NAME}         )
    message( STATUS ${FO} )
    add_custom_command(
        OUTPUT ${FO}
        COMMAND ${CMAKE_OBJCOPY}
        ARGS -O binary ${FI} ${FO}
        DEPENDS ${FI}
    )
    get_filename_component (TGT "${EXE_NAME}" NAME )
    add_custom_target( "target-objcopy_${TGT}" ALL DEPENDS ${FO} VERBATIM )
    get_directory_property( extra_clean_files ADDITIONAL_MAKE_CLEAN_FILES )
    set_directory_properties(
        PROPERTIES
        ADDITIONAL_MAKE_CLEAN_FILES "${extra_clean_files};${FO}"
    )
    set_source_files_properties( "${FO}" PROPERTIES GENERATED TRUE )
endmacro( OBJCOPY_FILE )

objcopy_file( ${PROJECT_NAME}.elf )