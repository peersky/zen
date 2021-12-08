
if(SIM)
  add_subdirectory(3rd_party/JUCE)
else()
  include(SeggerRTT.cmake)
  include(STM32Cube.cmake)
endif()

