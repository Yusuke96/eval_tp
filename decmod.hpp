#include "main.hpp"

class Decmod{
public:
  //	PARAMETER
  queue<Packet> q;
  bool status;
  //	FUNCTION
  Decmod();
  ~Decmod();
  void Enqueue(Packet*);
  void Dequeue(Packet*);
};
