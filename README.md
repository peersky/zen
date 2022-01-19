# ZEN - Signal processing as form of meditation

This repository contains highly optimized, sharp Occama blade for audio signal processing
It's multiplatform. You can build it as VST/Desktop/Mobile/Whatever via JUCE
or go embedded world trough STM32 target

Feel free to add your own target as PR

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



#prepare build
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
