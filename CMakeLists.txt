cmake_minimum_required(VERSION 3.13)
# set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "") # works
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# Setup Toolchain, Platform, Board
set(CMAKE_CONFIG_DIR "${CMAKE_SOURCE_DIR}/cmake")
include("${CMAKE_CONFIG_DIR}/Board.cmake")
include("${CMAKE_CONFIG_DIR}/Platform.cmake")
include("${CMAKE_CONFIG_DIR}/Toolchain.cmake")

# Print info
message("Board: ${BOARD}")
message("MCU: ${MCU}")
message("Platform: ${PLATFORM}")
message("Toolchain: ${TOOLCHAIN}")

# Start Project
project(Black_Delay)

# Add subdirectories
add_subdirectory(3rd_party)
add_subdirectory(src)