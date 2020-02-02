#include "main.hpp"

class Global{
public:
  //arg
  vector<string> arg_str;
  vector<string> option;
  //  PARAMETER
  //  Simulation time
  double time;
  double end_time;
  double start_time;
  double proc_size;
  double table_stall_time_total;
  vector<double> table_stall_time;
  vector<long> table_stall_count;
  double dram_stall_time;
  long dram_stall_count;
  long dram_access_try;
  long dram_wait;
  double cache_stall_time_total;
  long cache_access_count;
  vector<double> cache_stall_time;
  vector<long> cache_stall_count;
  vector<long> cache_try_count;
  vector<long> cache_hit_count;
  map<string,int> num_of_flow;
  //decode procedure counter
  double table_hit_cache_hit;
  double table_hit_cache_miss;
  double table_miss_decode_first_packet;
  double dont_cache_decode_first_packet;
  double dont_cache_decode_following_packet;
  //  Config parameter
  string tracefile;
  ifstream input;
  string resfile;
  ofstream output;
  int num_prediction_miss;
  int num_dontcache;
  u_int num_cache;
  vector<u_int> conf_cache;
  map<u_int, pair<u_int, u_int> > num_eachcache; //<targetsize(32,16,8,4,2), lastcachenum(0~7), amount(0~)>
  u_int num_decmod;
  u_int size_queue;
  double delay_cache;
  double delay_decode;
  double delay_makehuff;
  double delay_table;
  double delay_dram;
  //  Module
  Decmod* mod;
  Cache* cache;
  Table* table;
  Dram* dram;
  //  Event handler
  typedef void (Global::*Func)(Packet);
  typedef pair<Func, Packet> Event;
  multimap<double, Event> event_handler; // <time, function>
  //  Statistics
  u_int num_packet;
  u_int num_drop;
  u_int num_hit;
  double num_of_through; //読み飛ばしたヘッダパケット数
  double num_of_ReadedPacket;
  u_int res_sec;
  u_int nt;
  u_int np;
  u_int nd;
  u_int nh;
  u_int eof=0;
  //  Others
  Packet next_p;
  unsigned long pcount;
  //  FUNCTION
  Global();
  ~Global();
  void readConf(int argc, char* argv[]);
  void showConf();
  void initSim();
  void showResult();
  void reportResult();
  void inputFirstpacket();
  void inputPacket(Packet);
  void inputQueue(Packet);
  void outputQueue(Packet);
  void accessTable(Packet);
  void accessCache(Packet);
  void accessDram(Packet);
  void decodeFirstPacket(Packet);
  void decodePacket(Packet);
  void updateFirstPacket(Packet);
  void updatePacket(Packet);
  void processEvent();
  void through(Packet);
  double getReadedPacket();
};
