#!/bin/sh

if [ ! -d "../build" ]; then
  mkdir ../build
  cd ../build
  cmake ../
  cd ../app
fi
cmake --build ../build -j$(nproc) || cmake -S ../ -B ../build && cmake --build ../build -j$(nproc) 
./Digitaler
