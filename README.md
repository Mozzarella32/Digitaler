# Digitaler
## Description
A logic Simulator

## Prerequisites
You must install cmake, a c++ compiler and have accas to a stl implementation.

## Building and Downloading
### Linux
```bash
git clone --recurse-submodules --shallow-submodules --jobs 8 https://github.com/Mozzarella32/Digitaler.git
cd Digitaler
cd app
./buildandstart.sh
```
### Windows
```bash
git clone --recurse-submodules --shallow-submodules --jobs 8 https://github.com/Mozzarella32/Digitaler.git
cd Digitaler
cd app
./buildandstart.bat
```

## Running
The executable is moved to the app directory

## State of development
This builds on Windows and Linux(testet with Ubuntu and Arch) and runes normaly on Windows. But on Arch it fails with GLEW_Fehler: MissingGL version and on Ubuntu it starts but there are still many bugs.

### Windows
![image](assets/README/DemoWindows.png)
### Ubuntu
![image](assets/README/DemoUbuntu.png)
