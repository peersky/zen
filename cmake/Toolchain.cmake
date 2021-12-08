set(available_toolchains "armgcc" "gcc" "native")
set(TOOLCHAIN "armgcc" CACHE STRING "Toolchain for building")
set_property(CACHE TOOLCHAIN PROPERTY STRINGS ${available_toolchains})


if(NOT TOOLCHAIN IN_LIST available_toolchains)
  message(FATAL_ERROR "Invalid platform selected! PLATFORM must be on of ${available_toolchains}")
else()
  include("${CMAKE_CONFIG_DIR}/toolchain/${TOOLCHAIN}.cmake")
endif()
