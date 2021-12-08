set(available_platforms "stm32" "native")
set(PLATFORM "stm32" CACHE STRING "Platform of the system")
set_property(CACHE PLATFORM PROPERTY STRINGS ${available_platforms})


if(NOT PLATFORM IN_LIST available_platforms)
  message(FATAL_ERROR "Invalid platform ${PLATFORM} selected! PLATFORM must be on of ${available_platforms}")
else()
  string(TOUPPER ${PLATFORM} platform_uppercase)
  add_definitions(-DPLATFORM_${platform_uppercase})
  include("${CMAKE_CONFIG_DIR}/platform/${PLATFORM}.cmake")
endif()
