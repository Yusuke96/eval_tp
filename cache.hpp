#include "main.hpp"

class Cache{
public:
  //  PARAMETER
  typedef vector< pair<string, bool> > Tag;   //  boolはhit->updateまでに追い出されることを防ぐフラグ
  map<u_int, Tag> tag; // <index, 5tuple, 置換防止フラグ>
  u_int size;
  u_int num_entry;
  u_int num_line;
  map<string,int> flow; 
  double next_time_read;
  double next_time_write;
  //FUNCTION
  Cache();
  ~Cache();
  Packet Insert(Packet&);
  bool Access(Packet);
  void Update(Packet&);
};
