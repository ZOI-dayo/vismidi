#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>
#include <iostream>
#include <math.h>
#include <limits.h>
#include <stdio.h>
#include <unistd.h>

class PlaySound {
  const int TONE_SAMPLINGRATE = 44100;
  public:
  void play(int freq, int duration) {
    ALCdevice *device = alcOpenDevice(NULL);
    if (!device) return;
    ALCcontext *context = alcCreateContext(device, NULL);
    if (!context) return;
    alcMakeContextCurrent(context);


    ALuint buffer;
    ALuint source;
    alGenBuffers(1, &buffer);
    alGenSources(1, &source);

    int tone_size;
    int16_t* data = crateTone(freq, duration, &tone_size);

    int data_size =  tone_size * sizeof(int16_t);
    alBufferData(buffer, AL_FORMAT_MONO16, &data[0], data_size, TONE_SAMPLINGRATE);

    alSourcei(source, AL_BUFFER, buffer);

    alSourcePlay(source);
    usleep(duration);
    alSourcePause(source);

    alDeleteBuffers(1, &buffer);
    alDeleteSources(1, &source);
    alcDestroyContext(context);
    alcCloseDevice(device);
    delete[] data;
  }
  int16_t* crateTone(int freq, int duration, int *tone_size)
  {
    int bufsize = round(TONE_SAMPLINGRATE * duration / 10000.0);
    int16_t* buff = new int16_t[bufsize];

    double delta = (2 * M_PI * freq ) / TONE_SAMPLINGRATE;

    for (int i = 0; i < bufsize; i++) {
      buff[i] = SHRT_MAX * sin(delta * i);
    }

    *tone_size = bufsize;
    return buff;
  }
};
