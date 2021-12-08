set(TOOLCHAIN "armgcc" CACHE INTERNAL "")

# --- STM32F405xx ---
if(MCU STREQUAL "stm32f429xx")
    set(CPU "cortex-m4")
        
    set(PLATFORM_DEFINES
      "-DSTM32F429xx"
      "-DARM_MATH_CM4"
      "-DUSE_HAL_DRIVER"
      "-DUSE_FULL_LL_DRIVER"
      )
  
    set(PLATFORM_BUILD_FLAGS
      -mcpu=${CPU}
      -mthumb
      -mfpu=fpv4-sp-d16
      -mfloat-abi=hard
      )
  
  
    set(PLATFORM_LINK_FLAGS
      -mcpu=${CPU}
      -mthumb
      -mfpu=fpv4-sp-d16
      -mfloat-abi=hard
      --specs=nosys.specs
      -u _printf_float
      )
else()
    message(FATAL_ERROR "Invalid MCU: ${MCU}")
endif()
