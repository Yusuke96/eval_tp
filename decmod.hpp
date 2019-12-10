#include "main.hpp"

class Decmod{
public:
  //	PARAMETER
  queue<Packet> q;
  bool status;
  double recent_time;
  //	FUNCTION
  Decmod();
  ~Decmod();
  void Enqueue(Packet*);
  void Dequeue(Packet*);
  void Update(Packet p);
};
