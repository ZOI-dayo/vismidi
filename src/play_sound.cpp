#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>
#include <array>
#include <climits>
#include <iostream>
#include <math.h>
#include <limits.h>
#include <stdio.h>
#include <sys/_types/_int16_t.h>
#include <typeinfo>
#include <unistd.h>
#include <set>
#include <vector>
#include "object/note.cpp"

using namespace std;

class PlaySound {
  const int TONE_SAMPLINGRATE = 44100;
  const double MASTER_VOLUME = 0.1;

  public:
  int get_data_size(vector<Note> notes){
    double max_time = 0;
    for (Note note : notes) {
      if(max_time < note.end_time) max_time = note.end_time;
    }
    return ceil(max_time * TONE_SAMPLINGRATE);
  }
  vector<int16_t> calc_audio(vector<Note> notes) {
    int size = get_data_size(notes);
    vector<int16_t> data(size);
    for(Note note : notes){
      int start_time = note.begin_time * TONE_SAMPLINGRATE;
      int end_time = note.end_time * TONE_SAMPLINGRATE;
      double freq = note.freq();
      double volume = note.volume * MASTER_VOLUME * 440 / freq;
      double delta = (2 * M_PI * freq ) / TONE_SAMPLINGRATE;
      int length = end_time - start_time;
      int fixed_length = ceil((double)length * freq / TONE_SAMPLINGRATE) / freq * TONE_SAMPLINGRATE;
      for (int i = start_time; i <= start_time + fixed_length; i++) {
        // data[i] +=
        int addition = round(SHRT_MAX * sin(delta * (i - start_time)) * volume);
        int added = data[i] + addition;
        if(added < SHRT_MIN) added = SHRT_MIN;
        if(SHRT_MAX < added) added = SHRT_MAX;
        data[i] = added;
      }
    }
    return data;
  }

  ALuint source;
  int size;
  void setup(vector<Note> notes) {
    ALCdevice *device= alcOpenDevice(NULL);
    ALCcontext *context = alcCreateContext(device, NULL);
    ALuint buffer;
    alcMakeContextCurrent(context);
    vector<int16_t> data = calc_audio(notes);
    size = data.size();
    alGenBuffers(1, &buffer);
    alGenSources(1, &source);

    // int tone_size;
    // data = crateTone(freqs, duration, &tone_size, 0.1);

    alBufferData(buffer, AL_FORMAT_MONO16, data.data(), size * 2, TONE_SAMPLINGRATE); // なぜか2...

    alSourcei(source, AL_BUFFER, buffer);
  }
  void play(int offset){    
    usleep(offset);

    alSourcePlay(source);

    usleep(((long long int)1000000) * size / TONE_SAMPLINGRATE);
    alSourceStop(source);

    // alSourceStop(source);
    // alDeleteBuffers(1, &buffer);
    // alcDestroyContext(context);
    // alcCloseDevice(device);
    // delete[] data;
  }
  /*
     void play(std::set<float> freqs, int duration) {

     ALuint buffer;
     alGenBuffers(1, &buffer);
     alGenSources(1, &source);

     int tone_size;
     data = crateTone(freqs, duration, &tone_size, 0.1);

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
int16_t* crateTone(std::set<float> freqs, int duration, int *tone_size, float volume)
{
int bufsize = round(TONE_SAMPLINGRATE * (duration / 250000.0)); // なぜ250000なのかは不明 100000ではなく?
int16_t* buff = new int16_t[bufsize];

for(float freq : freqs){
double delta = (2 * M_PI * freq ) / TONE_SAMPLINGRATE;

for (int i = 0; i < bufsize; i++) {
buff[i] += (int16_t) round(SHRT_MAX * sin(delta * i) * volume);
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
   */
};
