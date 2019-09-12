#include "main.hpp"

class Table{
    public:
    //  PARAMETER
	map<string, Packet> tag;
  map<string, unsigned long> flow_size;
  map<string,pair<u_int,u_int>> cache_info; // (add) <flow_id,<cache_size,cache_num>>
  map<string,int>upflag;
  map<string,unsigned long>readed_size;
        double next_time;
    //  FUNCTION
        Table();
        ~Table();
        int Access(Packet);
        void Update(Packet);
  unsigned long GetCurrentSize(Packet);
  pair<u_int,u_int> GetCacheInfo(Packet);
  void UpdateReadedSize(Packet);
  unsigned long GetReadedSize(Packet);
};
