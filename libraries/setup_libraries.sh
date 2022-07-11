#!/bin/bash

cd `dirname $0`

git submodule update

# midifile
cd ./midifile
make library
cd ../

# openal-soft
cd openal-soft/build/
cmake ..
make
make install
cd ../../

