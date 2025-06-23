#!/bin/sh
rm Digitaler
rm Error.log
if [ ! -d "../build" ]; then
  mkdir ../build
  cd ../build
  cd ../app
fi


cmake -DCMAKE_BUILD_TYPE=Debug -S ../ -B ../build
cmake --build ../build -j$(nproc)
./Digitaler
