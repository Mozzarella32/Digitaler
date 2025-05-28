#!/bin/sh

if [ ! -d "../build" ]; then
  mkdir ../build
  cd ../build
  cmake ../
  cd ../app
fi
cmake --build ../build -j$(nproc) || cmkae -S ../ -B ../build && cmake --build ../build -j$(nproc) 
./Digitaler
