class Note{
  public:
    Note(double begin_time, double end_time, int key_num): begin_time(begin_time), end_time(end_time), key_num(key_num){}
    const double begin_time;
    const double end_time;
    // char timbre;
    const int key_num;
    // float velocity;
};
