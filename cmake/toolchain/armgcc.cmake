message(STATUS "Using ARM GCC")


if(NOT DEFINED ARM_NONE_EABI_TOOLCHAIN_BIN_PATH)
  # Try to find arm-none-eabi-gcc

  # Apple
  if(APPLE)
    find_program(arm_gcc "/Applications/ARM/gcc-arm-none-eabi-10.3-2021.10/bin/arm-none-eabi-gcc")

    if(NOT arm_gcc)
      message(FATAL_ERROR "No arm-none-eabi-gcc toolchain found! Fix cmake/toolchain/armgcc.make!")
    endif()

    get_filename_component(arm_gcc_path ${arm_gcc} DIRECTORY)

    set(ARM_NONE_EABI_TOOLCHAIN_BIN_PATH ${arm_gcc_path})

  # Unix
  elseif(UNIX AND NOT APPLE)
    find_program(arm_gcc "/usr/bin/arm-none-eabi-gcc")

    if(NOT arm_gcc)
      find_program(arm_gcc "/opt/gcc-arm-none-eabi-9-2020-q2-update/bin/arm-none-eabi-gcc")
    endif()

    if(NOT arm_gcc)
      message(FATAL_ERROR "No arm-none-eabi-gcc toolchain found! Fix cmake/toolchain/armgcc.make!")
    endif()

    get_filename_component(arm_gcc_path ${arm_gcc} DIRECTORY)

    set(ARM_NONE_EABI_TOOLCHAIN_BIN_PATH ${arm_gcc_path})
  endif()
endif()

set(TOOLCHAIN_PREFIX "${ARM_NONE_EABI_TOOLCHAIN_BIN_PATH}/arm-none-eabi-")

# Find all compilers and tools
find_program(arm_cc ${TOOLCHAIN_PREFIX}gcc REQUIRED)
find_program(arm_cxx ${TOOLCHAIN_PREFIX}g++ REQUIRED)
find_program(arm_objcopy ${TOOLCHAIN_PREFIX}objcopy REQUIRED)
find_program(arm_size ${TOOLCHAIN_PREFIX}size REQUIRED)
find_program(arm_objdump ${TOOLCHAIN_PREFIX}objdump REQUIRED)
find_program(arm_ar ${TOOLCHAIN_PREFIX}ar REQUIRED)
find_program(arm_ranlib ${TOOLCHAIN_PREFIX}ranlib REQUIRED)
find_program(arm_ld ${TOOLCHAIN_PREFIX}ld REQUIRED)


# fix on macOS: prevent cmake from adding implicit parameters to Xcode
set(CMAKE_OSX_SYSROOT "/")
set(CMAKE_OSX_DEPLOYMENT_TARGET "")

# Tell that we are building for ARM
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR ARM)

# Set tools
set(CMAKE_C_COMPILER ${arm_cc})
set(CMAKE_ASM_COMPILER ${arm_cc})
set(CMAKE_CXX_COMPILER ${arm_cxx})
set(CMAKE_AR ${arm_ar})
set(CMAKE_RANLIB ${arm_ranlib})
set(CMAKE_OBJCOPY ${arm_objcopy})
set(CMAKE_SIZE_UTIL ${arm_size})

# Perform compiler test without linker, since we are cross-compiling
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# Set language standard/version settings
enable_language(C ASM)
# set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)


# Set build type flags
set(CMAKE_C_FLAGS_DEBUG "-g3 -O0")
set(CMAKE_C_FLAGS_RELWITHDEBINFO "-g -O3")
set(CMAKE_C_FLAGS_RELEASE "-g0 -O3")
set(CMAKE_C_FLAGS_MINSIZEREL "-g0 -Os")

set(CMAKE_CXX_FLAGS_DEBUG "-g3 -O0")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-g -O3")
set(CMAKE_CXX_FLAGS_RELEASE "-g0 -O3")
set(CMAKE_CXX_FLAGS_MINSIZEREL "-g0 -Os")


# Do not allow shared libs
set(BUILD_SHARED_LIBS OFF)
set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")


set(data_flags "-ffunction-sections -fdata-sections") # -fno-builtin -fshort-enums
set(warning_flags "-Wall") # -Wno-attributes -Wno-format -fno-strict-aliasing

# C compile flags ----------------------------------------------------------------------------------

set(CMAKE_C_FLAGS "--std=gnu99 ${warning_flags} ${data_flags}")

# CMake has some default linkinkg flags, we need to clean them!
# First option is to clear the CMAKE_C_LINK_FLAGS
# Other option is to change the CMake linker executable
set(CMAKE_C_LINK_FLAGS "")
# set(CMAKE_C_LINK_EXECUTABLE "<CMAKE_C_COMPILER> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")

# Add colors
add_compile_options (-fdiagnostics-color=always)

# enable_language(C ASM)

# Export compilation commands to .json file (used by clang-complete backends)
# set(CMAKE_EXPORT_COMPILE_COMMANDS ON)



function(add_platform_executable target)

  message("Run ARM platform specific linking on ${target}")

  # message("DEFINES:" ${PLATFORM_DEFINES})

  set_target_properties(${target} PROPERTIES OUTPUT_NAME ${target}.elf)
  # set(CMAKE_EXECUTABLE_SUFFIX_C ".elf")

  set(hex_file "${target}.hex")
  set(elf_file "${target}.elf")
  set(bin_file "${target}.bin")

  target_sources(${target} PRIVATE
    ${PLATFORM_SOURCE_FILES}
    )

  target_include_directories(${target} PRIVATE
    ${PLATFORM_INCLUDE_DIRS}
    )

  target_compile_definitions(${target} PRIVATE
    ${PLATFORM_DEFINES}
    )

  # Definitions based on Debug or Release
  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${target} PRIVATE
      ${PLATFORM_DEFINES_DEBUG}
    )
  else()
    target_compile_definitions(${target} PRIVATE
      ${PLATFORM_DEFINES_RELEASE}
    )
  endif()


  target_compile_options(${target} PRIVATE
    ${PLATFORM_BUILD_FLAGS}
    )

  target_link_options(${target} PRIVATE
    ${PLATFORM_LINK_FLAGS}
    --specs=nano.specs
    # -lc
    # -lm
    -lnosys
    -Wl,-Map=${target}.map,--cref
    -Wl,--print-memory-usage
    -Wl,--gc-sections
    )

  # Print executable size
  add_custom_command(TARGET ${target}
    POST_BUILD
    COMMAND ${arm_size} -A ${elf_file}
    COMMAND ${arm_size} ${elf_file}
    )

  # Create hex file
  add_custom_command(TARGET ${target}
    POST_BUILD
    COMMAND ${arm_objcopy} -O ihex ${elf_file} ${hex_file}
    COMMAND ${arm_objcopy} -O binary ${elf_file} ${bin_file}
    )

endfunction()


function(add_platform_linker_file target linker_file)

  target_link_options(${target} PRIVATE
    -T${linker_file}
    )

endfunction()


function(add_platform_library target)

  target_compile_definitions(${target} PRIVATE
    ${PLATFORM_DEFINES}
    )

  target_compile_options(${target} PRIVATE
    ${PLATFORM_BUILD_FLAGS}
    )

endfunction()




function (set_target_link_options target_name linker_file)
  set(link_flags
    ${${ARCH}_DEFINES}
    "-Wl,--gc-sections --specs=nano.specs -L\"${NRF5_SDK_PATH}/modules/nrfx/mdk\""
    "-Xlinker -Map=\"${CMAKE_CURRENT_BINARY_DIR}/${target_name}.map\""
    "-T\"${linker_file}\"")

  string(REGEX REPLACE ";" " " link_flags "${link_flags}")
  set_target_properties(${target_name} PROPERTIES LINK_FLAGS ${link_flags})
endfunction (set_target_link_options)

function (create_hex executable)
  add_custom_command(
    TARGET ${executable}
    POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex ${CMAKE_CURRENT_BINARY_DIR}/${executable}.elf ${CMAKE_CURRENT_BINARY_DIR}/${executable}.hex
    BYPRODUCTS ${CMAKE_CURRENT_BINARY_DIR}/${executable}.hex)

  add_custom_command(
    TARGET ${executable}
    POST_BUILD
    COMMAND ${CMAKE_SIZE_UTIL} ${CMAKE_CURRENT_BINARY_DIR}/${executable}.elf)
endfunction(create_hex)


if (CMAKE_INTERPROCEDURAL_OPTIMIZATION)
  message(WARNING
    "CMAKE_INTERPROCEDURAL_OPTIMIZATION enables -flto with GCC which can lead to unexpected behavior. "
    "One particular problem is that the interrupt vector table can be messed up if the startup file "
    "isn't the first source file of the target. In general weak symbols tend to cause problems.\n"
    "More information https://gcc.gnu.org/bugzilla/show_bug.cgi?id=83967. "
    )
endif (CMAKE_INTERPROCEDURAL_OPTIMIZATION)
