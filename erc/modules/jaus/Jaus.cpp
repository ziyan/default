/**
 * Jaus command receiver
 */

#include "Jaus.h"
#include "JausData.h"
#include "../../common/define.h"

#define UDP_HEADER "JAUS01.0"
#define UDP_HEADER_SIZE_BYTES 8
#define HEADER_SIZE_BYTES 16
#define UDP_BUFFER 8*1024

#define DST_INSTANCE_ID 1
#define DST_NODE_ID 1
#define DST_COMPONENT_ID 33

#define SRC_INSTANCE_ID 1
#define SRC_NODE_ID 1
#define SRC_COMPONENT_ID 40
#define SRC_SUBSYS_ID 1

#define CMD_STOP 0x0003
#define CMD_RESUME 0x0004
#define CMD_SET_DISCRETE_DEVICES 0x0406


using namespace erc;
using namespace std;
using namespace ost;

Jaus::Jaus(DebugStream& debug, libconfig::Setting &setting) :
	LocalModule(debug,setting),
	socket(IPV4Address(""),(int)setting["port"]),
	currentData(new JausData),
	good(false)
{
	((JausData&)(*currentData)).mobility = false;
	((JausData&)(*currentData)).warning = false;
	good = true;
}

Jaus::~Jaus()
{
	terminate();
}

bool Jaus::isGood() const
{
	return good;
}

DataPointer Jaus::getData(const unsigned char arg) const
{
	return currentData;
}

void Jaus::run()
{
	char udp_header[UDP_HEADER_SIZE_BYTES+1];
	unsigned char buffer[UDP_BUFFER];
	unsigned short int len = 0;
	while(isGood())
	{
		//sleep
		Thread::sleep(10);
		
		//peek
		if(socket.peek(buffer,UDP_BUFFER)<=0) continue;
		
		//receive a packet
		len = socket.receive(buffer,UDP_BUFFER);
		debug << "Jaus: received a udp packet" << endl;

		//check for length
		if(len<UDP_HEADER_SIZE_BYTES+HEADER_SIZE_BYTES)
			continue;

		//compare jaus header
		if(UDP_HEADER_SIZE_BYTES>0)
		{
			copy(buffer, buffer+UDP_HEADER_SIZE_BYTES, udp_header);
			udp_header[UDP_HEADER_SIZE_BYTES] = 0;
			if(strcmp(udp_header,UDP_HEADER)!=0) continue;
		}
		
		//get length
		unsigned short int length =  (buffer[UDP_HEADER_SIZE_BYTES+12] & 0xff) | (buffer[UDP_HEADER_SIZE_BYTES+11] >> 4);
		if(len != UDP_HEADER_SIZE_BYTES + HEADER_SIZE_BYTES + length) continue;
		
		//check for ids
		//subsystem id
		//unsigned char DST_SUBSYS_ID = ((unsigned long int)socket.getLocal().getAddress().s_addr) >> 24 & 0xFF;
		//if(((unsigned long int)socket.getLocal().getAddress().s_addr) >> 24 & 0xFF) != SRC_SUBSYS_ID) continue;
		
		//dst
		if(buffer[UDP_HEADER_SIZE_BYTES+4]!=DST_INSTANCE_ID) continue;
		if(buffer[UDP_HEADER_SIZE_BYTES+5]!=DST_COMPONENT_ID) continue;
		if(buffer[UDP_HEADER_SIZE_BYTES+6]!=DST_NODE_ID) continue;
		//if(buffer[UDP_HEADER_SIZE_BYTES+7]!=DST_SUBSYS_ID) continue;
		
		if(buffer[UDP_HEADER_SIZE_BYTES+8]!=SRC_INSTANCE_ID) continue;
		if(buffer[UDP_HEADER_SIZE_BYTES+9]!=SRC_COMPONENT_ID) continue;
		if(buffer[UDP_HEADER_SIZE_BYTES+10]!=SRC_NODE_ID) continue;
		if(buffer[UDP_HEADER_SIZE_BYTES+11]!=SRC_SUBSYS_ID) continue;
		
		//get command
		unsigned short int cmd = WORD_TO_SHORT(buffer[UDP_HEADER_SIZE_BYTES+2],buffer[UDP_HEADER_SIZE_BYTES+3]);
		debug << "Jaus: received valid command " << cmd << endl;
		
		if(cmd==CMD_STOP)
		{
			((JausData&)(*currentData)).mobility = false;
			debug << "Jaus: mobility set to false" << endl;
		}
		else if(cmd==CMD_RESUME)
		{
			((JausData&)(*currentData)).mobility = true;
			debug << "Jaus: mobility set to true" << endl;
		}
		else if(cmd==CMD_SET_DISCRETE_DEVICES)
		{
			if(length > 0)
			{
				int field = 1;
				if(!(buffer[UDP_HEADER_SIZE_BYTES+HEADER_SIZE_BYTES] & 2)) continue;
				if(buffer[UDP_HEADER_SIZE_BYTES+HEADER_SIZE_BYTES] & 1) field++;
				
				if(buffer[UDP_HEADER_SIZE_BYTES+HEADER_SIZE_BYTES+field] & 2)
				{
					((JausData&)(*currentData)).warning = true;
					debug << "Jaus: warning set to true" << endl;
				}
				else
				{
					((JausData&)(*currentData)).warning = false;
					debug << "Jaus: warning set to false" << endl;
				}
			}
		}
	}
	good = false;
}

void Jaus::close()
{
	good = false;
	Thread::sleep(20);
}

