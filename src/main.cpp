#include <functional>
#include <iterator>
#include <ostream>
#include <stdio.h>
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
#include "MidiEvent.h"
#include "MidiEventList.h"
#include "MidiFile.h"
#include "./play_sound.cpp"
#include "./object/note.cpp"

using namespace std;

/*
   int track_count;
   int time_delta; // 変数名 時間単位

   int get_data(vector<byte> *data, int begin, int count){
   int result;
   if(count == 1) result = to_integer<int>((*data)[0]);
   else result = 0x100 * get_data(data, begin, count - 1) +to_integer<int>((*data)[begin + count - 1]);
   (*data).erase((*data).begin() + begin, (*data).begin() + count);
   return result;
   }
   int get_data(vector<byte> *data, int count){
   return get_data(data, 0, count);
   }
   int get_data(vector<byte> *data){
   return get_data(data, (*data).size());
   }
   vector<byte> string_to_byte(string data) {
   vector<byte> result;
   for (char c : data) {
   result.push_back((byte) c);
   }
   return result;
   }

   int load(string file_path, vector<string> *raw_score)
   {
   ifstream ifs(file_path, ios::binary);

   ifs.seekg(0, ios::end);
   long long int size = ifs.tellg();
   ifs.seekg(0);

   vector<byte> data;
// data.insert(data.begin(), istream_iterator<byte>(ifs), istream_iterator<byte>());
ifs.read(reinterpret_cast<char*>(data.data()), size);
vector<byte> MThd_byte = string_to_byte("MThd");
if(get_data(&data, 4) != get_data(&MThd_byte)) return 1;
if(get_data(&data, 4, 2) != 0) return 1;

track_count = get_data(&data, 2);
time_delta = get_data(&data, 2);

vector<byte> MTrk_byte = string_to_byte("MTrk");
while (!data.empty()) {
if(get_data(&data, 4) != get_data(&MTrk_byte)) break;
int event_data_size = get_data(&data, 4);

vector<byte> event_data;
copy(data.begin(), data.begin() + event_data_size, back_inserter(event_data));

// statements
}
return 0;
}
*/
int main(int argc, char**argv) {
  // const int SCORE_LENGTH = 100;
  const int KEYBOARD_LENGTH = 9;
  const int BLACK_LENGTH = 5;
  const int C4_OFFSET = 30;
  const int SCROLL_SPEED = 10;

  PlaySound audio_out = PlaySound();

  // string raw_score[SCORE_LENGTH];
  struct winsize w;
  ioctl(0, TIOCGWINSZ, &w);
  printf("\033[2J");
  printf("\033[%d;%dH" ,0, 0);

  string current_display[w.ws_row];

  // Load
  vector<Note> notes;
  vector<string> raw_score;
  smf::MidiFile midifile;
  midifile.read(argv[1]);
  midifile.doTimeAnalysis();
  double score_length_sec = 0;
  for (int i = 0; i < midifile.size(); i++) {
    smf::MidiEventList events = midifile[i];
    events.linkEventPairs();
    events.linkNotePairs();
    score_length_sec = max(score_length_sec, events[events.size() - 1].seconds);
    map<int, double> draft_notes;
    for (int event_count = 0; event_count < events.size(); event_count++) {
      smf::MidiEvent event = events[event_count];
      if(event.isNoteOn()){
        draft_notes[event.getKeyNumber()] = event.seconds;
      }
      else if(event.isNoteOff()){
        if(draft_notes.contains(event.getKeyNumber())){
          double start_time = draft_notes[event.getKeyNumber()];
          draft_notes.erase(event.getKeyNumber());
          Note note = Note(start_time, event.seconds, event.getKeyNumber());
          notes.push_back(note);
        }
      }
      // else if(event.isPressure()){event.}
    }
  }
  const int SCORE_LENGTH = ceil(score_length_sec * SCROLL_SPEED);
  const int score_width = w.ws_col / 2;
  for(int i = 0; i < SCORE_LENGTH; i++){
    if(raw_score.size() <= i){
      raw_score.push_back(string(score_width, '9'));
    }
    double time = (double)i / SCROLL_SPEED;
    for (Note note : notes) {
      if(note.begin_time <= time && time <= note.end_time){
        int note_loc = note.key_num - 60 + C4_OFFSET;
        if(note_loc < 0) note_loc = 0;
        if(score_width - 1 < note_loc) note_loc = score_width - 1;

        raw_score[i][note_loc] = '1';
      }
    }
  }

  /*
     for(int l = 0; l < SCORE_LENGTH; l++){
     for (int i = 0; i < w.ws_col / 2; i++) {
     if(30 <= l && (i == 20 || i == 24 || i == 27)) raw_score[l] += '1';
     else if(20 <= l && l < 30 && i == 27) raw_score[l] += '1';
     else if(10 <= l && l < 20 && i == 24) raw_score[l] += '1';
     else if(l < 5 && i == 20) raw_score[l] += '1';
     else raw_score[l] += '9';
     }
     }
     */

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


  usleep(1000000);


  // Show
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
    audio_out.play(freqs, 100000);
    usleep(100000);
  }
  audio_out.end();
  return 0;
}
