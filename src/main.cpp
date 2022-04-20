#include <ostream>
#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string>
#include <iostream>
#include <array>
int main(int argc, char**argv) {
  const int SCORE_LENGTH = 5;
  std::string raw_score[SCORE_LENGTH];
  struct winsize w;
  ioctl(0, TIOCGWINSZ, &w);
  printf("\033[2J");
  printf("\033[%d;%dH" ,0, 0);

  for(int l = 0; l < SCORE_LENGTH; l++){
    for (int i = 0; i < w.ws_col; i++) {
      if(i == 5) raw_score[l] += "\033[41m";
      raw_score[l] += " ";
      if(i - 1 == 5) raw_score[l] += "\033[49m";
    }
  }
  for(int current_loc = - w.ws_row; current_loc < SCORE_LENGTH; current_loc++){
    printf("\033[2J");
    printf("\033[%d;%dH" ,0, 0);
    for(int line = 0; line < w.ws_row; line++){
      const int index = w.ws_row - line + current_loc;
      if(0 <= index && index < SCORE_LENGTH) std::cout << raw_score[index] << std::endl;
      else std::cout << std::string(w.ws_col, ' ') << std::endl;
    }
    usleep(100000);
  }
  return 0;
}
