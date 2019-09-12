#include "main.hpp"

class Dram{
    public:
    //  PARAMETER
        double next_time;
    //FUNCTION
        Dram();
        ~Dram();
        void Access(Packet);
};