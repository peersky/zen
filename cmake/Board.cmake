set(available_boards "black_delay" "simulator")
set(BOARD "black_delay" CACHE STRING "Board type")
set_property(CACHE BOARD PROPERTY STRINGS ${available_boards})


if(NOT BOARD IN_LIST available_boards)
  message(FATAL_ERROR "Invalid board selected! BOARD must be on of ${available_boards}")
else()
  string(TOUPPER ${BOARD} board_uppercase)
  add_definitions(-DBOARD_${board_uppercase})
  include("${CMAKE_CONFIG_DIR}/board/${BOARD}.cmake")
endif()




function(add_board_source_file list source)
  # TODO: Check if file exists
  get_filename_component(dir ${source} DIRECTORY)
  get_filename_component(filename ${source} NAME_WE)

  set(${list}
    ${${list}}
    "${CMAKE_CURRENT_SOURCE_DIR}/${dir}/${filename}_${BOARD}.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/${source}"
    PARENT_SCOPE
    )
  
endfunction(add_board_source_file)
