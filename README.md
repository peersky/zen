# CMake STM32 & JUCE template

This is template for embedded projects with STM32 and JUCE based simulators

## Compile

### Prepare build directories
```sh
mkdir -p build/stm32
mkdir -p build/sim
```

### Compile STM32 application 
```sh

# Enter stm32 build directory
cd build/stm32

# Prepare build
cmake ../.. -GNinja

# Compile
ninja
```
- if using other build system e.g. Make, remove -GNinja
- by defdault BOARD=black_delay

### Compile JUCE Simulator
```sh

# Enter simulator build directory
cd build/sim

# Prepare build
cmake ../.. -GNinja -DBOARD=simulator

# Compile
ninja
```


