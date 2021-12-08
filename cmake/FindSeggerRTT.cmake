message("Adding Segger RTT library...")

# Try to find Segger RTT library source
# file(GLOB_RECURSE segger_rtt_path "${CMAKE_SOURCE_DIR}/SEGGER_RTT.h") # It is not recommended to use file GLOB
find_path(segger_rtt_path SEGGER_RTT.h PATHS ${CMAKE_SOURCE_DIR}/*/* PATH_SUFFIXES RTT)

if(NOT segger_rtt_path)
  message(FATAL_ERROR "Failed to find Segger RTT library!")
endif()

# Get library root path
get_filename_component(segger_rtt_path "${segger_rtt_path}/.." ABSOLUTE)

message(STATUS "Segger RTT library found!")

  # Segger RTT
set(segger_rtt_src
  ${segger_rtt_path}/RTT/SEGGER_RTT_ASM_ARMv7M.S
  ${segger_rtt_path}/RTT/SEGGER_RTT.c
  ${segger_rtt_path}/RTT/SEGGER_RTT_printf.c
  ${segger_rtt_path}/Syscalls/SEGGER_RTT_Syscalls_GCC.c
)

set(segger_rtt_inc
  ${segger_rtt_path}/RTT
)

# set(rtt_lib "segger_rtt")
# add_library(${rtt_lib} INTERFACE IMPORTED)
# target_include_directories(${rtt_lib} INTERFACE ${segger_rtt_inc})
# target_sources(${rtt_lib} INTERFACE ${segger_rtt_src})

set(rtt_lib "segger_rtt")
add_library(${rtt_lib} STATIC ${segger_rtt_src})
target_include_directories(${rtt_lib} PUBLIC ${segger_rtt_inc})
add_platform_library(${rtt_lib})