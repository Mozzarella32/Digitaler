# Digitaler
## Description
A logic Simulator

## Prerequisites
You must install cmake, a c++ compiler and have accas to a stl implementation.

## Downloading 
Don't forget the submodules
```bash
git clone https://github.com/Mozzarella32/Digitaler.git
cd Digitaler
git submodule update --recursive --init --remote
```

## Building
Go to the root of this Project and run these commands the -j8 flag is for faster compilation and can be omitted
```bash
mkdir build
cd build
cmake ../ && cmake --build . -j8
```

## Running
The executable is moved to the app directory

## State of development
This (should) compile on Windows however ther is no gui and simulation yet, you can play around with the DataResourceManager.cpp to insert blocks. You also have the ability to place wires.
