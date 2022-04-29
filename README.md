# VisMIDI: Visual MIDI Player

Play MIDI file in console

## Requirement

- OpenAL Soft (Original OpenAL also usable)
- [Midifile](https://github.com/craigsapp/midifile)

## Installation

- OpenAL Soft
Forrow [this](https://openal-soft.org/#installing)

- Midifile
Forrow [this](https://github.com/craigsapp/midifile#downloading)

```bash
cd somewhere
git clone git@github.com:ZOI-dayo/vismidi.git
cd vismidi

# You might edit the path of library in build.sh or build_mac.sh

./build_mac.sh # or ./build_sh
```

## Usage

```bash
cd somewhere/vismidi
./out/main.out -i /path/to/file.midi -v 10 -s 30
```

## Option

- speed
  `-s (val)` or `--speed (val)`
  1 <= `val` (int, [char/s], Default: 30)
- volume
  `-v (val)` or `--volume (val)`
  0 <= `val` <= 100 (int, [%], Default: 10)

## Author

ZOI_dayo

- Japanese high school student
- [GitHub](https://github.com/ZOI-dayo)
- [Twitter](https://twitter.com/ZOI_dayo)

## License

"VisMIDI" is under [MIT license](https://en.wikipedia.org/wiki/MIT_License).
