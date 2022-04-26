#include <functional>
#include <iterator>
#include <ostream>
#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string>
#include <iostream>
#include <array>
#include <vector>
#include <cmath>
#include <fstream>
#include <cstddef>
#include <iterator>
#include "./play_sound.cpp"

using namespace std;


int track_count;

int byte_sum(vector<byte> data, int begin, int count){
  if(count == 1) return to_integer<int>(data[0]);
  else return 0x100 * byte_sum(data, begin, count - 1) +to_integer<int>(data[size(data)]);
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
  if(byte_sum(data, 0, 4) == byte_sum(string_to_byte("MThd"), 0, 4)) return 1;
  if(byte_sum(data, 8, 2) != 0) return 1;

  // track_count = 0x100 * data[]
  return 0;
}

int main(int argc, char**argv) {
  // const int SCORE_LENGTH = 100;
  const int KEYBOARD_LENGTH = 9;
  const int BLACK_LENGTH = 5;
  const int C4_OFFSET = 20;

  PlaySound audio_out = PlaySound();

  // string raw_score[SCORE_LENGTH];
  struct winsize w;
  ioctl(0, TIOCGWINSZ, &w);
  printf("\033[2J");
  printf("\033[%d;%dH" ,0, 0);

  string current_display[w.ws_row];

  // Load
  vector<string> raw_score;
  const int SCORE_LENGTH = load("/path/to/midi", &raw_score);
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
