#include "main.hpp"
#include<iomanip>
Decmod::Decmod(){
  status=false;//free
}

Decmod::~Decmod(){;}

void Decmod::Enqueue(Packet *p){
    if(q.size() >= global.size_queue){
        global.num_drop++;
	cout << "debug:error" << endl;
        return;
    }else{
        q.push(*p);
	//cout << "debug:EQP1" << global.num_packet << " status==" << status << " hash==" << p->hash << endl;
        if(q.size() >= 1 && status == 0){
	  //cout << "debug:EQP2" << global.num_packet << endl;
            Global::Event e = pair<Global::Func, Packet>(&Global::outputQueue, *p);
            global.event_handler.insert(pair<double, Global::Event>(p->timestamp, e));
        }
    }
}

void Decmod::Dequeue(Packet *tmp_p){
  status = 1;//busy
    if(q.empty()){
      return;
    }else{
      Packet p;
      p = q.front();
      //cout << "debug:[pop]" << p.hash << endl;
      q.pop();
      p.timestamp = tmp_p->timestamp;
      Global::Event e = pair<Global::Func, Packet>(&Global::accessTable, p);
      global.event_handler.insert(pair<double, Global::Event>(p.timestamp, e));
    }
}
