#include <iterator>
#include <ostream>
#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string>
#include <iostream>
#include <array>
#include <cmath>
#include "./play_sound.cpp"

int main(int argc, char**argv) {
  const int SCORE_LENGTH = 100;
  const int KEYBOARD_LENGTH = 9;
  const int BLACK_LENGTH = 5;
  const int C4_OFFSET = 20;
  
  PlaySound audio_out = PlaySound();

  std::string raw_score[SCORE_LENGTH];
  struct winsize w;
  ioctl(0, TIOCGWINSZ, &w);
  printf("\033[2J");
  printf("\033[%d;%dH" ,0, 0);

  std::string current_display[w.ws_row];

  // Load
  for(int l = 0; l < SCORE_LENGTH; l++){
    for (int i = 0; i < w.ws_col / 2; i++) {
      if(30 <= l && (i == 20 || i == 24 || i == 27)) raw_score[l] += '1';
      else if(20 <= l && l < 30 && i == 27) raw_score[l] += '1';
      else if(10 <= l && l < 20 && i == 24) raw_score[l] += '1';
      else if(l < 10 && i == 20) raw_score[l] += '1';
      else raw_score[l] += '9';
    }
  }

  // make piano
  std::string piano_lane;
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
  std::cout << "\033[" << w.ws_row - KEYBOARD_LENGTH + 1 << ";" << 0 << "H";
  // std::cout << "\033[47m";
  // std::cout << std::string(w.ws_col * KEYBOARD_LENGTH, ' ');
  for(int i = 0; i < BLACK_LENGTH; i++) std::cout << piano_lane << "\033[49m" << std::endl;
  std::cout << "\033[47m" << std::string(w.ws_col * (KEYBOARD_LENGTH - BLACK_LENGTH), ' ') << "\033[49m";
  // std::cout << "\033[49m";
  std::cout << std::flush;


  usleep(1000000);


  // Show
  for(int current_loc = - w.ws_row; current_loc < SCORE_LENGTH - KEYBOARD_LENGTH; current_loc++){
    for(int line = 0; line < w.ws_row - KEYBOARD_LENGTH; line++){
      const int index = w.ws_row - line + current_loc;
      std::string out_text;
      if(0 <= index && index < SCORE_LENGTH) out_text = raw_score[index];
      else out_text = std::string(w.ws_col / 2, '9');
      for(int letter = 0; letter < w.ws_col / 2; letter++){
        if(current_display[line][letter] != out_text[letter]){
          std::cout << "\033[" << line + 1 << ";" << letter * 2 + 1 << "H"; // << std::flush;
          std::cout << "\033[4" << out_text[letter] << "m"; // << std::flush;
          std::cout << "  "; // << std::flush;
          std::cout << "\033[49m";
        }
      }
      current_display[line] = out_text;
    }
    std::cout << std::flush;
    std::string last_line = current_display[w.ws_row - KEYBOARD_LENGTH - 1];
    std::set<float> freqs = {};
    for(int i=0; i < w.ws_row; i++){
      if(last_line[i] != '9'){
        freqs.insert(440 * std::pow(2.0, (i - (C4_OFFSET + 9)) / 12.0));
      }
    }
    // for(float f : freqs) std::cout << f << std::endl;
    audio_out.play(freqs, 100000);
    usleep(100000);
  }
  audio_out.end();
  return 0;
}
