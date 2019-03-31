/*
 * config.h
 *
 *  Created on: Apr 12, 2017
 *      Author: Super
 */

#ifndef CONFIG_H_
#define CONFIG_H_

/*Data Structure
 *
 *Packtet: | Node_ID | Print_NO | Local_IRCF | Local_time || Transmitter_ID | Packet_ID | Transmitted_IRCF | transmitter_time | CFO ||
 *Bytes:		1		2			2			2				1				2				2					2			1
 *
 *
 *
 */
#define  Node_ID			12
#define  Protocol_On    1
#define  Protocol_Off   0

#define bit_Node_ID  0
#define bit_packetID_1 1
#define bit_packetID_2 2
#define bit_ircf_1 3
#define bit_ircf_2 4
#define bit_localtime_1 5
#define bit_localtime_2 6
#define bit_localtime_3 7
#define bit_localtime_4 8
#define bit_SYNC 9




#define Packet_transmission_period 10
#define SYNC_Period 2

#define transmit_power_0dbm 0x50
#define transmit_power_10dbm 0xc6
#define transmit_power_maxdbm 0xc0



typedef struct
{
//	int root_id;
	int nodeid;

	unsigned int synt_seq;
	unsigned int IRCF_value;
	unsigned int long local_time_value;
	unsigned int long global_time_value;

}syntpacket;

typedef struct
{
//	int root_id;
	int nodeid;

	unsigned int synt_seq;
	unsigned int IRCF_value;
	unsigned long local_time_value;
	unsigned long global_time_value;
	int CFO;
	unsigned long recv_timestamp;
	unsigned int sync_bit;


}syntreceive;

typedef struct
{
    int stk[10-1];
    int top;
}stack;


#endif /* CONFIG_H_ */

