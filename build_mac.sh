cd `dirname $0`
mkdir -p ./out
g++ -std=gnu++20 -I/opt/homebrew/opt/openal-soft/include/ -I../_lib/midifile/include -L../_lib/midifile/lib -lmidifile -Wall ./src/main.cpp -o ./out/main.out -framework OpenAL
