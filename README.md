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
# Linux
Go to the app directory and run buildandstart.sh
```bash
cd app
./buildandstart.sh
```
# Windows
Go to the app directory and run buildandstart.bat
```bash
cd app
./buildandstart.bat
```

## Running
The executable is moved to the app directory

## State of development
This builds on Windows and Linux(testet with Ubuntu and Arch) and runes normaly on Windows. But on Arch it fails with GLEW_Fehler: MissingGL version and on Ubuntu it starts but there are still many bugs.
