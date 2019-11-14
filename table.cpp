#include "main.hpp"

Table::Table(){
    tag.clear();
    next_time = 0.0;
}

Table::~Table(){;}

int Table::Access(Packet p){
  next_time = p.timestamp;
    if(tag.count(p.flow_id) == 0){
        return -1;    // TABLE MISS
    }else{
      //  TABLE HIT
      return tag[p.flow_id].cache_num;
    }
}

void Table::Update(Packet p){
  next_time = p.timestamp;//(add)0830
  upflag[p.flow_id] += 1;
  p.current_len += p.length;//正しい？
  flow_size[p.flow_id] += p.length / p.comp_ratio;
  cache_info[p.flow_id].first = p.info_current_cache.first; //(add)
  cache_info[p.flow_id].second = p.info_current_cache.second; //(add)
  tag[p.flow_id] = p;
}

unsigned long Table::GetCurrentSize(Packet p){
  return flow_size[p.flow_id];
}

pair<u_int,u_int> Table::GetCacheInfo(Packet p){
  return cache_info[p.flow_id];
}

unsigned long Table::GetReadedSize(Packet p){
  return readed_size[p.flow_id];
}

void Table::UpdateReadedSize(Packet p){
  readed_size[p.flow_id] += p.length;
}
