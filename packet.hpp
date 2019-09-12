#include "main.hpp"

class Packet{
    public:
    //  PARAMETER
        double timestamp;
	unsigned long id;
        int length;
        u_char sip[4];
        u_char dip[4];
        unsigned int sport;
        unsigned int dport;
        u_char protocol;
		string flow_id;
        unsigned long comp_len;
        unsigned long decomp_len;
        double comp_ratio;
  string stream_num;
  double flow_size;
  int line_num;
		unsigned long current_len;
        unsigned int hash;
		int cache_num;
        bool cache_miss;
  unsigned long current_dictsize; //(add)
  pair<u_int, u_int> info_current_cache; //(add)現在のキャッシュ情報<size,amount>
    //  FUNCTION
        Packet();
        ~Packet();
        void Show();
};
