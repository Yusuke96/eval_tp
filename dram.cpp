#include "main.hpp"

Dram::Dram(){
    next_time = 0.0;
}

Dram::~Dram(){;}

void Dram::Access(Packet p){
    next_time = p.timestamp;
}