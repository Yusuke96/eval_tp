#include "main.hpp"

Global global;

int main(int argc, char *argv[])
{
  Packet p;
    global.readConf(argc, argv);
    global.showConf();
    global.initSim();
    global.inputPacket(p);

    for(;;){
      global.processEvent();
      }
    
    return 0;
}
