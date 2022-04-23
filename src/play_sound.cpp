#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>
// #include <bits/stdint-intn.h>
#include <iostream>
#include <math.h>
#include <limits.h>
#include <stdio.h>
#include <unistd.h>
#include <set>

class PlaySound {
  const int TONE_SAMPLINGRATE = 44100;
  ALCdevice *device;
  ALCcontext *context;
  int16_t* data;
  ALuint source;
  public:
  PlaySound() {
    device = alcOpenDevice(NULL);
    if (!device) return;
    context = alcCreateContext(device, NULL);
    if (!context) return;
    alcMakeContextCurrent(context);
  }
  void play(std::set<float> freqs, int duration) {

    ALuint buffer;
    alGenBuffers(1, &buffer);
    alGenSources(1, &source);

    int tone_size;
    data = crateTone(freqs, duration, &tone_size);

    int data_size =  tone_size * sizeof(int16_t);
    alBufferData(buffer, AL_FORMAT_MONO16, &data[0], data_size, TONE_SAMPLINGRATE);

    alSourceStop(source);
    // alSourceRewind(source);
    alSourcei(source, AL_BUFFER, buffer);

    alSourcePlay(source);
    // usleep(duration);
    // alSourcePause(source);
    alDeleteBuffers(1, &buffer);
    // alDeleteSources(1, &source);

  }
  int16_t* crateTone(std::set<float> freqs, int duration, int *tone_size)
  {
    int bufsize = round(TONE_SAMPLINGRATE * duration / 1000.0);
    int16_t* buff = new int16_t[bufsize];

    for(float freq : freqs){
      double delta = (2 * M_PI * freq ) / TONE_SAMPLINGRATE;

      for (int i = 0; i < bufsize; i++) {
        buff[i] += (int16_t) round(SHRT_MAX * sin(delta * i) / 10);
      }
    }

    *tone_size = bufsize;
    return buff;
  }
  void end() {

    alcDestroyContext(context);
    alcCloseDevice(device);
    delete[] data;
  }
};
