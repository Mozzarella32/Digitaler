# Digitaler
## Description
A logic Simulator

## Prerequisites
Cmake, c++23, a c++/c compiler
### Ubuntu
```bash
sudo apt-get update
sudo apt-get install -y libgtk-3-dev libglu1-mesa-dev build-essential
```
### Arch
```bash
sudo pacman -S gtk3 glu base-devel
```
Probebly some xorg stuff to, not sure
```bash
sudo pacman -S xorgproto libx11 libxext libxau libxmu libxt libxpm libxrender libice libsm libxcb libxfixes libxrandr libxi libxinerama libxcursor libxdamage libxdmcp libxcomposite libxft libxfont2

```
## Building and Downloading
### Linux
```bash
git clone --recurse-submodules --shallow-submodules --jobs 8 https://github.com/Mozzarella32/Digitaler.git
cd Digitaler
make release
make run
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
Builds on Windows and Linux(Tested on Ubuntu and Arch see ci). Currently refactoring shaders so graphics are broken.

### Windows
![image](assets/README/DemoWindows.png)
### Ubuntu
![image](assets/README/DemoUbuntu.png)
