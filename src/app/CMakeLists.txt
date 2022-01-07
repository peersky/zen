set(available_projects "zen_mk2")

if(NOT PROJECT IN_LIST available_projects)
  message(FATAL_ERROR "Invalid project ${PROJECT} selected! PROJECT must be on of ${available_projects}")
else()
  add_subdirectory("${PROJECT}")
endif()