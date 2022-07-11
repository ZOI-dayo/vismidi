cd `dirname $0`
mkdir -p ./out
clang++ -std=gnu++17 -I./libraries/openal-soft/include/ -I./libraries/midifile/include -L./libraries/midifile/lib -lmidifile -I/usr/local/include -I/usr/local/include/AL -L/usr/local/lib -lopenal ./src/main.cpp -o ./out/main.out 

