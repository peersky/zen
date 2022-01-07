
if(SIM OR VST)
  message("adding JUCE")
  add_subdirectory(JUCE)
else()
  include(SeggerRTT.cmake)
  include(STM32Cube.cmake)
endif()

