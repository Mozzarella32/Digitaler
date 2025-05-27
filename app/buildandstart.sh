#!/bin/sh

if [ ! -d "../build" ]; then
  mkdir ../build
  cd ../build
  cmake ../
  cd ../app
fi
cmake --build ../build -j8
./Digitaler
