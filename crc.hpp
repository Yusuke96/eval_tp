#include "main.hpp"

class CRC{
	public:
        //  PARAMETER
		unsigned char *input;
		crc_optimal< 1, 0x01, 0, 0, false, false> crc_1;
		crc_optimal< 2, 0x03, 0, 0, false, false> crc_2;
		crc_optimal< 3, 0x03, 0, 0, false, false> crc_3;
		crc_optimal< 4, 0x03, 0, 0, false, false> crc_4;
		crc_optimal< 5, 0x17, 0, 0, false, false> crc_5;
		crc_optimal< 6, 0x03, 0, 0, false, false> crc_6;
		crc_optimal< 7, 0x09, 0, 0, false, false> crc_7;
		crc_optimal< 8, 0x4D, 0, 0, false, false> crc_8;
		crc_optimal< 9, 0x21, 0, 0, false, false> crc_9;
		crc_optimal< 10, 0x81, 0, 0, false, false> crc_10;
		crc_optimal< 11, 0x05, 0, 0, false, false> crc_11;
		crc_optimal< 12, 0x09, 0, 0, false, false> crc_12;
		crc_optimal< 13, 0x1B, 0, 0, false, false> crc_13;
		crc_optimal< 14, 0x21, 0, 0, false, false> crc_14;
		crc_optimal< 15, 0x03, 0, 0, false, false> crc_15;
		crc_optimal< 16, 0x2B, 0, 0, false, false> crc_16;
		crc_optimal< 17, 0x09, 0, 0, false, false> crc_17;
		crc_optimal< 18, 0x09, 0, 0, false, false> crc_18;
		crc_optimal< 32, 0x04C11DB7, 0xFFFFFFFF, 0xFFFFFFFF, true, true> crc_32;
        //  FUNCTION
		CRC();
		~CRC();
		void Align(Packet);
 		unsigned int Calc(int bit, unsigned long num);
};
