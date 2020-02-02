#include "main.hpp"

Global global;

int main(int argc, char *argv[])
{
  Packet p;
    global.readConf(argc, argv);
    global.showConf();
    global.initSim();
    //for(int i=0; i<100000; i++){
    global.inputPacket(p);
    //global.inputFirstpacket();
    //    int i = 0;
    for(;;){
      //     cout << i << endl;
      global.processEvent();
      //      i++;
    }
    
    return 0;
}
