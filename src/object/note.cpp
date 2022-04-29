#include <cmath>
#include <iostream>

using namespace std;
class Note{
  public:
    Note(double begin_time, double end_time, int key_num): begin_time(begin_time), end_time(end_time), key_num(key_num){}
    const double begin_time;
    const double end_time;
    // char timbre;
    const int key_num;
    // float velocity;
    double freq() {
      return 440.0 * pow(2.0, (key_num - 69) / 12.0);
    }
};
