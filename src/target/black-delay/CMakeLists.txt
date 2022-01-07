# Set target name
set(target "black-delay")

# Use STM32Cube library
# find_package(STM32Cube)
use_STM32Cube(F4 ${CMAKE_CURRENT_SOURCE_DIR})

# Use Segger RTT
# find_package(SeggerRTT)
use_SeggerRTT()


# Target Source files
set(source_files
  main.cpp

  startup_stm32f429xx.s
  system_stm32f4xx.c
  stm32f4xx_it.c
  )

# Target Include directories
set(include_dirs
  ${CMAKE_CURRENT_SOURCE_DIR}
  )


# Add an executable target
add_executable(${target} ${source_files})

# Specify target include directories
target_include_directories(${target} PRIVATE ${include_dirs})

target_link_libraries(${target} PRIVATE
  m
  stm32cubef4
  segger_rtt
  )

#
add_platform_linker_file(${target} ${CMAKE_CURRENT_SOURCE_DIR}/STM32F429ZITx_FLASH.ld)
add_platform_executable(${target})