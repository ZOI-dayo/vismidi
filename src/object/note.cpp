class Note{
  public: 
    Note(char timbre, int note_num, float velocity) : timbre(timbre), note_num(note_num), velocity(velocity){
    }
  private:
    const char timbre;
    const int note_num;
    const float velocity;
};
