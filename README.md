# Digitaler
## Description
A logic Simulator

## Downloading 
Don't forget the submodules
```bash
git clone git@github.com:Mozzarella32/Digitaler.git
git submodule update --recursive --remote
cd Digitaler
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
