cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Compiler options
set(STM32_MCU_FLAGS  "-mcpu=cortex-m55 -mfpu=fpv5-d16 -mfloat-abi=hard -mcmse " )

#Linker options
set(STM32_LINKER_SCRIPT STM32N657XX_ROMxspi2_RAMxspi1_custom.ld)
set(STM32_LINKER_OPTION  )

# Set the project name
set(CMAKE_PROJECT_NAME DroneStack_Appli)
project(${CMAKE_PROJECT_NAME})

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Include mx-generated
include("mx-generated.cmake")

# Remove lwIP subdirectory includes that shadow standard C/C++ headers.
# CubeMX adds paths like LwIP/src/include/lwip and LwIP/src/include/compat/stdc
# which contain errno.h that shadows picolibc's errno.h, breaking both C code
# (missing errno macros) and C++ code (libc++'s #include_next chain for <cerrno>).
# The parent path LwIP/src/include is sufficient — lwIP sources use
# #include "lwip/errno.h" etc.
# This must be re-applied after each CubeMX regeneration of mx-generated.cmake.
get_target_property(_cubemx_incs stm32cubemx INTERFACE_INCLUDE_DIRECTORIES)
list(FILTER _cubemx_incs EXCLUDE REGEX "LwIP/src/include/(compat|lwip)")
set_property(TARGET stm32cubemx PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${_cubemx_incs}")

# Make Application/Inc visible to all targets (Connectivity needs lwipopts.h).
target_include_directories(stm32cubemx INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/Application/Inc)

# Infineon middleware fixes for starm-clang:
# - __WEAK: PAL uses it without including the CMSIS header that defines it.
# - CY_USING_HAL: cyhal.h guards all HAL driver includes behind this define.
#   Without it, SDIO types/enums (cyhal_sdio_cfg_t, CYHAL_SDIO_CMD_*, etc.)
#   are never declared.
target_compile_definitions(Connectivity PRIVATE
    "__WEAK=__attribute__((weak))"
    CY_USING_HAL
    MBEDTLS_USER_CONFIG_FILE="mbedtls_user_config.h"
)

# Exclude LPA source files that require cy_secure_sockets (not available) and
# assume dual-stack IP (u_addr union). LPA handles WiFi power-saving features
# and is not required for basic WiFi operation.
get_target_property(_conn_srcs Connectivity SOURCES)
list(FILTER _conn_srcs EXCLUDE REGEX "lpa/")
set_property(TARGET Connectivity PROPERTY SOURCES "${_conn_srcs}")


# In order to use CMake for cross-compiling
set(CMAKE_CXX_COMPILER_FORCED "true")

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
)

# Enable CMake support for ASM, C and C++ languages
enable_language(C CXX ASM)

# Build drone_core autonomy library
add_subdirectory(DroneAutonomyStack/core)
# Fix ETL include path for the submodule layout
target_include_directories(drone_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../Libraries/etl/include)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths
    ${CMAKE_CURRENT_SOURCE_DIR}/Application/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/Utilities/lcd
    ${CMAKE_CURRENT_SOURCE_DIR}/Utilities/Fonts
    ${CMAKE_CURRENT_SOURCE_DIR}/../Drivers/BSP
    ${CMAKE_CURRENT_SOURCE_DIR}/../Drivers/BSP/Components/common
    ${CMAKE_CURRENT_SOURCE_DIR}/../Middlewares/ST/Camera_Middleware
    ${CMAKE_CURRENT_SOURCE_DIR}/../Middlewares/ST/Camera_Middleware/STM32_ISP_Library/isp/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../Middlewares/ST/Camera_Middleware/STM32_ISP_Library/evision/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../Middlewares/ST/Camera_Middleware/sensors
    ${CMAKE_CURRENT_SOURCE_DIR}/../Middlewares/ST/Camera_Middleware/sensors/imx335
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user sources here
    ${CMAKE_CURRENT_SOURCE_DIR}/Application/Src/hal_exti_notifier.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Application/Src/px4_uart_comm_link.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Application/Src/stm32_debug_log.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Application/Src/stm32_core_app.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Application/Src/lidar_uart_receiver.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Application/Src/camerapipeline.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Utilities/lcd/stm32_lcd.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Utilities/lcd/stm32_lcd_ex.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../Drivers/BSP/stm32n6570_discovery_lcd.c

    ${CMAKE_CURRENT_SOURCE_DIR}/../Middlewares/ST/Camera_Middleware/STM32_ISP_Library/isp/Src/isp_core.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../Middlewares/ST/Camera_Middleware/STM32_ISP_Library/isp/Src/isp_services.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../Middlewares/ST/Camera_Middleware/STM32_ISP_Library/isp/Src/isp_algo.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../Middlewares/ST/Camera_Middleware/cmw_camera.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../Middlewares/ST/Camera_Middleware/cmw_utils.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../Middlewares/ST/Camera_Middleware/sensors/cmw_imx335.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../Middlewares/ST/Camera_Middleware/sensors/imx335/imx335.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../Middlewares/ST/Camera_Middleware/sensors/imx335/imx335_reg.c
)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    # Add user defined libraries
    drone_core
    ${CMAKE_CURRENT_SOURCE_DIR}/../Middlewares/ST/Camera_Middleware/STM32_ISP_Library/evision/Lib/libn6-evision-awb_gcc.a
    ${CMAKE_CURRENT_SOURCE_DIR}/../Middlewares/ST/Camera_Middleware/STM32_ISP_Library/evision/Lib/libn6-evision-st-ae_gcc.a
)

# Remove wrong libob.a library dependency when using cpp files
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)

# Validate that STM32CubeMX code is compatible with C standard
if((CMAKE_C_STANDARD EQUAL 90) OR (CMAKE_C_STANDARD EQUAL 99))
    message(ERROR "Generated code requires C11 or higher")
endif()

# Specify the output ELF file
set(TARGET_ELF "${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}.elf")
# Command to generate .bin file
add_custom_command(
    TARGET ${CMAKE_PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary ${TARGET_ELF} ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}.bin
    COMMENT "Generating .bin file"
)

# Define the paths to the STM32 Signing Tool and the input/output files
set(SIGNING_TOOL /usr/local/STMicroelectronics/STM32Cube/STM32CubeProgrammer/bin/STM32_SigningTool_CLI)
# Define the input and output files for Appli
set(APPLI_INPUT "${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}.bin")
set(APPLI_OUTPUT "${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}-trusted.bin")

# Add a post-build step to sign the Appli binary
add_custom_command(
    TARGET ${CMAKE_PROJECT_NAME}
    POST_BUILD
    # Step 1: Create a file with 'y'
    COMMAND ${CMAKE_COMMAND} -E echo y > "${CMAKE_BINARY_DIR}/input.txt"
    # Step 2: Run the signing tool, redirecting input from the file
    COMMAND "${SIGNING_TOOL}" -bin "${APPLI_INPUT}" -nk -of 0x80000000 -t fsbl -o "${APPLI_OUTPUT}" -hv 2.3 -dump "${APPLI_OUTPUT}" -align < "${CMAKE_BINARY_DIR}/input.txt"
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
    COMMENT "Signing Appli binary: ${APPLI_INPUT} -> ${APPLI_OUTPUT}"
)

# Convert the signed Appli .bin back to .elf
add_custom_command(
  TARGET ${CMAKE_PROJECT_NAME}
  POST_BUILD
  COMMAND arm-none-eabi-objcopy -I binary -O elf32-littlearm -B arm --change-addresses=0x70100000 ${APPLI_OUTPUT} ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}-trusted.elf
  COMMENT "Converting signed .bin back to .elf in address [0x70100000]"
)