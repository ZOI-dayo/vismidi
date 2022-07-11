#include <chrono>
#include <ctime>
#include <functional>
#include <iterator>
#include <ostream>
#include <stdio.h>
#include <sys/_types/_time_t.h>
#include <sys/ttycom.h>
#include <thread>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string>
#include <iostream>
#include <array>
#include <utility>
#include <vector>
#include <cmath>
#include <fstream>
#include <cstddef>
#include <iterator>
#include <map>
#include <getopt.h>
#include "MidiEvent.h"
#include "MidiEventList.h"
#include "MidiFile.h"
#include "./play_sound.cpp"

using namespace std;

void play(PlaySound audio_out, struct winsize w, int KEYBOARD_LENGTH, int scroll_speed){
  audio_out.play(1000000 * (w.ws_row - KEYBOARD_LENGTH) / scroll_speed);
}

void abrt_handler(int sig) {
  cout << "\e[?12;25h" << flush;
  abort();
}

int main(int argc, char**argv) {
  
  signal(SIGINT, abrt_handler);

  // const int SCORE_LENGTH = 100;
  const int KEYBOARD_LENGTH = 9;
  const int BLACK_LENGTH = 5;
  const int C4_OFFSET = 45;
  int scroll_speed = 30;
  int volume = 10;
  const struct option longopts[] = {
    { "volume", required_argument, 0, 'v' },
    { "speed" , required_argument, 0, 's' },
    { "input" , required_argument, 0, 'i' },
    { 0       , 0                , 0,  0  }
  };
  int longindex = 0;
  // opterr = 0;
  int option_result;
  const char* optstring = "v:s:i:";
  string input_file;
  while ((option_result = getopt_long(argc, argv, optstring, longopts, &longindex)) != -1){
    if(option_result == 'v') {
      volume = atoi(optarg);
    }else if(option_result == 's') {
      scroll_speed = atoi(optarg);
    }else if(option_result == 'i') {
      input_file = optarg;
    }

  }

  PlaySound audio_out = PlaySound((double) volume / 100.0);

  // string raw_score[SCORE_LENGTH];
  struct winsize w;
  ioctl(0, TIOCGWINSZ, &w);
  cout << "\033[2J" << "\033[0;0H" << "\e[?25l" << flush;

  string current_display[w.ws_row];

  // Load
  vector<Note> notes;
  vector<string> raw_score;
  smf::MidiFile midifile;
  midifile.read(input_file);
  midifile.doTimeAnalysis();
  double score_length_sec = 0;
  for (int i = 0; i < midifile.size(); i++) {
    smf::MidiEventList events = midifile[i];
    events.linkEventPairs();
    events.linkNotePairs();
    score_length_sec = max(score_length_sec, events[events.size() - 1].seconds);
    struct draft_note {
      double start_second;
      float volume;
    };
    map<int, draft_note> draft_notes;
    for (int event_count = 0; event_count < events.size(); event_count++) {
      smf::MidiEvent event = events[event_count];
      if(event.isNoteOn()){
        draft_notes[event.getKeyNumber()].start_second = event.seconds;
        draft_notes[event.getKeyNumber()].volume = event.getVelocity() / 127.0;
      }
      else if(event.isNoteOff()){
        if(draft_notes.count(event.getKeyNumber()) == 0){
          double start_second = draft_notes[event.getKeyNumber()].start_second;
          float volume = draft_notes[event.getKeyNumber()].volume;
          draft_notes.erase(event.getKeyNumber());
          Note note = Note(start_second, event.seconds, event.getKeyNumber(), volume);
          notes.push_back(note);
        }
      }
      // else if(event.isPressure()){event.}
    }
  }
  const int SCORE_LENGTH = ceil(score_length_sec * scroll_speed);
  const int score_width = w.ws_col / 2;
  for(int i = 0; i < SCORE_LENGTH; i++){
    if(raw_score.size() <= i){
      raw_score.push_back(string(score_width, '9'));
    }
    double time = (double)i / scroll_speed;
    for (Note note : notes) {
      if(note.begin_time <= time && time <= note.end_time){
        int note_loc = note.key_num - 60 + C4_OFFSET;
        if(note_loc < 0) note_loc = 0;
        if(score_width - 1 < note_loc) note_loc = score_width - 1;

        raw_score[i][note_loc] = '1';
      }
    }
  }

  // make piano
  string piano_lane;
  for(int i = 0; i < w.ws_col / 2; i++){
    switch((i + 12000 - C4_OFFSET) % 12){
      case 0:
      case 2:
      case 4:
      case 5:
      case 7:
      case 9:
      case 11:
        piano_lane += "\033[47m  ";
        break;
      case 1:
      case 3:
      case 6:
      case 8:
      case 10:
        piano_lane += "\033[40m  ";
        break;
    }
  }
  cout << "\033[" << w.ws_row - KEYBOARD_LENGTH + 1 << ";" << 0 << "H";
  // cout << "\033[47m";
  // cout << string(w.ws_col * KEYBOARD_LENGTH, ' ');
  for(int i = 0; i < BLACK_LENGTH; i++) cout << piano_lane << "\033[49m" << endl;
  cout << "\033[47m" << string(w.ws_col * (KEYBOARD_LENGTH - BLACK_LENGTH), ' ') << "\033[49m";
  // cout << "\033[49m";
  cout << flush;

  audio_out.setup(notes);

  // usleep(1000000);
  thread bgm(play, audio_out, w, KEYBOARD_LENGTH, scroll_speed);

  // Show
  chrono::time_point start_time = chrono::steady_clock::now();
  for(int current_loc = - w.ws_row; current_loc < SCORE_LENGTH - KEYBOARD_LENGTH; current_loc++){
    for(int line = 0; line < w.ws_row - KEYBOARD_LENGTH; line++){
      const int index = w.ws_row - line + current_loc;
      string out_text;
      if(0 <= index && index < SCORE_LENGTH) out_text = raw_score[index];
      else out_text = string(w.ws_col / 2, '9');
      for(int letter = 0; letter < w.ws_col / 2; letter++){
        if(current_display[line][letter] != out_text[letter]){
          cout << "\033[" << line + 1 << ";" << letter * 2 + 1 << "H"; // << flush;
          cout << "\033[4" << out_text[letter] << "m"; // << flush;
          cout << "  "; // << flush;
          cout << "\033[49m";
        }
      }
      current_display[line] = out_text;
    }
    cout << flush;
    string last_line = current_display[w.ws_row - KEYBOARD_LENGTH - 1];
    set<float> freqs = {};
    for(int i=0; i < w.ws_row; i++){
      if(last_line[i] != '9'){
        freqs.insert(440 * pow(2.0, (i - (C4_OFFSET + 9)) / 12.0));
      }
    }
    // for(float f : freqs) cout << f << endl;
    // audio_out.play(freqs, 1000000 / scroll_speed);
    
    chrono::time_point end_time = start_time + chrono::microseconds((1000000 / scroll_speed) * (current_loc + w.ws_row));
    this_thread::sleep_until(end_time);
    // usleep(1000000 / scroll_speed);
  }
  bgm.join();
  // audio_out.end();
  cout << "\e[?12;25h" << endl;;
  return 0;
}
