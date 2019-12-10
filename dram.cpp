#include "main.hpp"

Dram::Dram(){
  next_time_read = 0.0;
  next_time_write = 0.0;
}

Dram::~Dram(){;}

void Dram::Read(Packet p){
  next_time_read = p.timestamp;
}

void Dram::Write(Packet p){
  next_time_write = p.timestamp;
}
