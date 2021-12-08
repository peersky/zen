message("Generating STM32Cube HAL library...")

set(cube_path "${CMAKE_CURRENT_SOURCE_DIR}/STM32CubeF4" CACHE INTERNAL "")



function(use_STM32Cube FAMILY hal_conf_path)

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

    # HAL Configuration file path
    ${hal_conf_path}
    )

  # --- Create Library ---
  set(hal_lib "stm32cube${FAMILY_L}")
  add_library(${hal_lib} STATIC ${stm32cube_src})
  target_link_libraries(${hal_lib} PRIVATE ${cmsis_path}/Lib/GCC/libarm_cortexM4lf_math.a)
  target_include_directories(${hal_lib} PUBLIC ${stm32cube_inc})
  add_platform_library(${hal_lib})

  endfunction()
