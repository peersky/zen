message("Adding STM32Cube HAL library...")

# Try to find STM32Cube library source
# file(GLOB_RECURSE cube_path "${CMAKE_SOURCE_DIR}/stm32*xx_hal.h") # It is not recommended to use file GLOB
find_path(cube_path stm32f4xx_hal.h PATHS ${CMAKE_SOURCE_DIR}/*/*/*/*/*) # A default STM32Cube directory tree is assumed inside additional library directory (e.g. 3rd_party)

if(NOT cube_path)
  message(FATAL_ERROR "Failed to find STM32Cube library!")
endif()

# Get library root path
get_filename_component(cube_path "${cube_path}/../../.." ABSOLUTE)

message(STATUS "STM32Cube library found!")

# HAL Drivers for F4 familiy
set(f4_hal_drivers
  adc can cec cortex crc cryp dac dcmi dfsdm dma dma2d dsi exti flash
  flash_ramfunc fmpi2c gpio hash hcd i2c i2s irda iwdg lptim ltdc mmc nand nor
  pccard pcd pwr qspi rcc rng rtc sai sd sdram smartcard smbus spdifrx spi
  sram tim uart usart wwdg

  adc_ex cryp_ex dac_ex dcmi_ex dma_ex flash_ex fmpi2c_ex hash_ex i2c_ex
  i2s_ex ltdc_ex pcd_ex pwr_ex rcc_ex rtc_ex sai_ex tim_ex
)

# LL Drivers for F4 familiy
set(f4_ll_drivers
  adc crc dac dma dma2d exti fmc fsmc gpio i2c lptim pwr rcc rng rtc sdmmc spi
  tim usart usb utils
)



# Get Familiy
find_file(PACKAGE_FILE NAMES package.xml PATHS ${cube_path})
if(PACKAGE_FILE)
  file(READ ${PACKAGE_FILE} PACKAGE_FILE_CONTENT)

  string(REGEX MATCH "PackDescription Release=\"FW.F([0-9])." FAMILY_SYM ${PACKAGE_FILE_CONTENT})
  if(CMAKE_MATCH_1)
    set(FAMILY "F${CMAKE_MATCH_1}")
  else()
  message(FATAL_ERROR "Could not extract STM32 Family from package.xml!")
  endif()
else()
  message(FATAL_ERROR "Could not open STM32Cube package.xml!")
endif()

message(STATUS "Found STM32${FAMILY}Cube")

# Lower case familiy name
string(TOLOWER ${FAMILY} FAMILY_L)

# Get Paths
set(drivers_path "${cube_path}/Drivers")
set(hal_path "${drivers_path}/STM32${FAMILY}xx_HAL_Driver")
set(cmsis_path "${drivers_path}/CMSIS")

# --- Collect sources ---
set(stm32cube_src "${hal_path}/Src/stm32${FAMILY_L}xx_hal.c")

if(NOT DEFINED ${FAMILY_L}_hal_drivers OR NOT DEFINED ${FAMILY_L}_ll_drivers)
  message(FATAL_ERROR "Drivers for ${FAMILY} not defined")
endif()

# Collect HAL Drivers
foreach(driver ${${FAMILY_L}_hal_drivers})
  list(APPEND stm32cube_src "${hal_path}/Src/stm32${FAMILY_L}xx_hal_${driver}.c")
endforeach()

# Collect LL Drivers
foreach(driver ${${FAMILY_L}_ll_drivers})
  list(APPEND stm32cube_src "${hal_path}/Src/stm32${FAMILY_L}xx_ll_${driver}.c")
endforeach()


# --- Collect include paths ---
set(stm32cube_inc
  # HAL
  ${hal_path}/Inc
  ${hal_path}/Inc/Legacy

  # CMSIS
  ${cmsis_path}/Device/ST/STM32${FAMILY}xx/Include
  ${cmsis_path}/Include
  ${cmsis_path}/DSP/Include
  )


# --- Create Library ---
set(hal_lib "stm32cube${FAMILY_L}")
add_library(${hal_lib} INTERFACE IMPORTED)
target_link_libraries(${hal_lib} INTERFACE ${cmsis_path}/Lib/GCC/libarm_cortexM4lf_math.a)
target_include_directories(${hal_lib} INTERFACE ${stm32cube_inc})
target_sources(${hal_lib} INTERFACE ${stm32cube_src})

