#include "main.hpp"

class Dram{
public:
  //  PARAMETER
  double next_time_read;
  double next_time_write;
  //FUNCTION
  Dram();
  ~Dram();
  void Read(Packet);
  void Write(Packet);
};
