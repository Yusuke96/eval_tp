#include "main.hpp"

CRC::CRC(){input = (unsigned char*)malloc(sizeof(char) * 13);}
CRC::~CRC(){free(input);}
void CRC::Align(Packet p){
    memcpy(&input[0], &(p.sip[0]), 4);
    memcpy(&input[4], &(p.dip[0]), 4);
    memcpy(&input[8], &(p.sport), 2);
    memcpy(&input[10], &(p.dport), 2);
    memcpy(&input[12], &(p.protocol), 1);
}
unsigned int CRC::Calc(int bit, unsigned long num){
    switch(bit){
      /*        case 1:
			crc_1.reset();
			crc_1.process_bytes(input, num);
			return crc_1.checksum();
		case 2:
			crc_2.reset();
			crc_2.process_bytes(input, num);
			return crc_2.checksum();
		case 3:
			crc_3.reset();
			crc_3.process_bytes(input, num);
			return crc_3.checksum();
		case 4:
			crc_4.reset();
			crc_4.process_bytes(input, num);
			return crc_4.checksum();
		case 5:
			crc_5.reset();
			crc_5.process_bytes(input, num);
			return crc_5.checksum();
		case 6:
			crc_6.reset();
			crc_6.process_bytes(input, num);
			return crc_6.checksum();
		case 7:
			crc_7.reset();
			crc_7.process_bytes(input, num);
			return crc_7.checksum();
*/		case 8:
			crc_8.reset();
			crc_8.process_bytes(input, num);
			return crc_8.checksum();
/*		case 9:
			crc_9.reset();
			crc_9.process_bytes(input, num);
			return crc_9.checksum();
		case 10:
			crc_10.reset();
			crc_10.process_bytes(input, num);
			return crc_10.checksum();
		case 11:
			crc_11.reset();
			crc_11.process_bytes(input, num);
			return crc_11.checksum();
		case 12:
			crc_12.reset();
			crc_12.process_bytes(input, num);
			return crc_12.checksum();
		case 13:
			crc_13.reset();
			crc_13.process_bytes(input, num);
			return crc_13.checksum();
		case 14:
			crc_14.reset();
			crc_14.process_bytes(input, num);
			return crc_14.checksum();
		case 15:
			crc_15.reset();
			crc_15.process_bytes(input, num);
			return crc_15.checksum();
*/		case 16:
			crc_16.reset();
			crc_16.process_bytes(input, num);
			return crc_16.checksum();
		case 17:
			crc_17.reset();
			crc_17.process_bytes(input, num);
			return crc_17.checksum();
		case 18:
			crc_18.reset();
			crc_18.process_bytes(input, num);
			return crc_18.checksum();
		default:
			cout << "Error: This CRC calculation is not appropiate." << endl;
			exit(-1);
    }
}
