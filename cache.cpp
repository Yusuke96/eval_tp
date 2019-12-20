#include "main.hpp"

Cache::Cache(){
  tag.clear();
  size=0;
  num_entry=0;
  num_line=0;
  next_time_read=0.0;
  next_time_write=0.0;
}

Cache::~Cache(){;}

Packet Cache::Insert(Packet& p){
  //insert
  CRC crc;
  crc.Align(p);
  flow[p.flow_id] = 1;
  u_int line;
  next_time_write = p.timestamp;//add0808
  if(num_line==1){
    line = 0;
  }else{
    line = crc.Calc(16, 13) % num_line;//  キャッシュのインデックス用にハッシュ値を別に計算
  }
  if(tag[line].size() == num_entry || tag[line].size() == 4){
    if(tag[line].back().second == true){       //  この場合，hit->update途中のパケットが処理中なのでキャッシュ登録しない(稀？)
      return p;
    }

    //dram access(write back)
    while(true){
      global.dram_access_try ++;
      if(p.timestamp >= global.dram->next_time_write){//success
	p.timestamp += global.delay_dram;
	global.dram->Write(p);
	break;
      }else{//failure
	if(global.num_packet >= 10001){
	  global.dram_stall_count += 1;
	  if(global.dram->next_time_write - p.timestamp > global.delay_dram){
	    global.dram_stall_time += global.delay_dram;
	    // p.timestamp += global.delay_dram;
	  }else{
	    global.dram_stall_time += (global.dram->next_time_write - p.timestamp);
	    // p.timestamp += (global.dram->next_time_write - p.timestamp);
	  }
	}
	p.timestamp = global.dram->next_time_write;
      }
    }

    //簡易LRU
    tag[line].pop_back();
  }
  p.line_num = line;
  tag[line].insert(tag[line].begin(), pair<string,bool>(p.flow_id, false));
  
  return p;
}

bool Cache::Access(Packet p){
  next_time_read = p.timestamp;
  global.cache_access_count += 1;
  flow[p.flow_id] = 1;
  CRC crc;
  crc.Align(p);
  u_int line;
  if(num_line==1){
    line = 0;
  }else{
    line = crc.Calc(16, 13) % num_line;  //  キャッシュのインデックス用にハッシュ値を別に計算
  }
  vector< pair<string, bool> >::iterator it = tag[line].begin();
  while(it != tag[line].end()){
    if(it->first == p.flow_id){
      //  LRU ... hitしたエントリを先頭に
      tag[line].erase(it);
      tag[line].insert(tag[line].begin(), pair<string, bool>(p.flow_id, true));   //  hitした場合，hit->updateの間にエントリが追い出されるのを防ぐフラグをつける
      global.num_hit++;
      return true;    //  HIT
    }
    it++;
  }
  return false;   //  MISS
}

void Cache::Update(Packet& p){
  CRC crc;
  crc.Align(p);
  u_int line;
  next_time_write = p.timestamp;
  if(num_line==1){
    line = 0;
  }else{
    line = crc.Calc(16, 13) % num_line;  //  キャッシュのインデックス用にハッシュ値を別に計算
  }
  p.line_num = line;
  vector< pair<string, bool> >::iterator it = tag[line].begin();
  while(it != tag[line].end()){
    if(it->first == p.flow_id){
      it->second = false;     //  置換防止フラグを解除
      return;
    }
    it++;
  }    
}
