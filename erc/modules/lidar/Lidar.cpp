/**
 * Module wrapper for lidar LMS200
 */

#include "Lidar.h"
#include "LidarDataPacket.h"
#include "../../common/define.h"
#include "../../common/timedrecv.h"
#include "../../common/Point.h"
#include "LidarData.h"
#include <cmath>

#define LIDAR_AOI_START 89
#define LIDAR_AOI_END -89

using namespace std;
using namespace erc;
using namespace LibSerial;

Lidar::Lidar(DebugStream& debug, libconfig::Setting &setting)
  : LocalModule(debug,setting),stream(0),good(false),currentData(new LidarData)
{
	//trying baud rate 9600
	stream = new SerialStream((string)setting["device"]);
	debug<<"Lidar: trying to open " << (string)setting["device"] << endl;
	if(!stream) return;
	if(!(*stream)) return;
	if(!(stream->IsOpen())) return;
	debug<<"Lidar: trying baud rate 9600." << endl;
	stream->SetBaudRate( SerialStreamBuf::BAUD_9600);
	stream->SetCharSize( SerialStreamBuf::CHAR_SIZE_8 );
	stream->SetNumOfStopBits( 1 );
	stream->SetParity( SerialStreamBuf::PARITY_NONE );
	stream->SetFlowControl( SerialStreamBuf::FLOW_CONTROL_NONE );
	
	//send out the change rate command
	if(isConnected())
	{
		debug<<"Lidar: running at baud rate 9600. switching over to 38400." << endl;
		//running on 9600bps now switch to 38400bps
		LidarDataPacket cmd;
		cmd.addr = 0x00;
		cmd.data = new unsigned char[2];
		cmd.data[0] = 0x20; //command
		cmd.data[1] = 0x40;
		cmd.length = 2;
		*stream << cmd;
		(*stream).flush();
		//rate change command sent out
		//now try to receive response
		if(timedrecv(1,0,stream->getFileDescriptor())<=0) return;
		*stream >> cmd;
		if(cmd.length!=3) return;
		if(cmd.data[0]!=0xA0) return;
		if(cmd.data[1]!=0x00) return;

		//wait till the baud rate change
		while(isConnected()) Thread::sleep(500);

		//reconnecting
		stream->Close();
		delete stream;
		stream = 0;

		Thread::sleep(500);
	}
	else
	{
		//reconnecting
		stream->Close();
		delete stream;
		stream = 0;

		//trying for baud rate 19200
		debug<<"Lidar: trying baud rate 19200." << endl;
		stream = new SerialStream((string)setting["device"]);
		if(!stream) return;
		if(!(*stream)) return;
		if(!(stream->IsOpen())) return;
	
		stream->SetBaudRate( SerialStreamBuf::BAUD_19200);
		stream->SetCharSize( SerialStreamBuf::CHAR_SIZE_8 );
		stream->SetNumOfStopBits( 1 );
		stream->SetParity( SerialStreamBuf::PARITY_NONE );
		stream->SetFlowControl( SerialStreamBuf::FLOW_CONTROL_NONE );
	
		//send out the change rate command
		if(isConnected())
		{
			debug<<"Lidar: running at baud rate 19200. switching over to 38400." << endl;
			LidarDataPacket cmd;
			cmd.addr = 0x00;
			cmd.data = new unsigned char[2];
			cmd.data[0] = 0x20; //command
			cmd.data[1] = 0x40;
			cmd.length = 2;
			*stream << cmd;
			(*stream).flush();
			//rate change command sent out
			//now try to receive response
			if(timedrecv(1,0,stream->getFileDescriptor())<=0) return;
			*stream >> cmd;
			if(cmd.length!=3) return;
			if(cmd.data[0]!=0xA0) return;
			if(cmd.data[1]!=0x00) return;

			//wait till the baud rate change
			while(isConnected()) Thread::sleep(500);

			//reconnecting
			stream->Close();
			delete stream;
			stream = 0;

			Thread::sleep(500);
		}
		else
		{
			//reconnecting
			stream->Close();
			delete stream;
			stream = 0;
		}
	}

	//connect using new speed 38400
	debug<<"Lidar: trying baud rate 38400." << endl;
	stream = new SerialStream((string)setting["device"]);
	if(!(*stream)) return;
	if(!(stream->IsOpen())) return;
	stream->SetBaudRate( SerialStreamBuf::BAUD_38400 );
	stream->SetCharSize( SerialStreamBuf::CHAR_SIZE_8 );
	stream->SetNumOfStopBits( 1 );
	stream->SetParity( SerialStreamBuf::PARITY_NONE );
	stream->SetFlowControl( SerialStreamBuf::FLOW_CONTROL_NONE );
	if(!isConnected()) return;
	good = (*stream);
	debug<<"Lidar: successfully connected at 38400." << endl;
}

Lidar::~Lidar()
{
}

bool Lidar::isGood() const
{
	return good && stream && (*stream) && stream->IsOpen();
}

bool Lidar::isConnected()
{
	if(!stream||!*stream) return false;
	LidarDataPacket cmd;
	cmd.addr = 0x00;
	cmd.data = new unsigned char[1];
	cmd.length = 1;
	cmd.data[0] = 0x31; //command	
	*stream << cmd;
	(*stream).flush();

	if(timedrecv(1,0,stream->getFileDescriptor())>0)
	{
		*stream >> cmd;
		return true;
	}
	return false;
}

void Lidar::run()
{
	while(isGood())
	{
		mutex.enterMutex();
		//wrap everything so that we wont create a dead lock
		do
		{
			//start scanning
			LidarDataPacket cmd;
			cmd.addr = 0x00;
			cmd.data = new unsigned char[2];
			cmd.length = 2;
			cmd.data[0] = 0x30; //command	
			cmd.data[1] = 0x01;
			*stream << cmd;
			(*stream).flush();
			if(!isGood()) break;

			LidarDataPacket response;
			if(!(timedrecv(1,0,stream->getFileDescriptor())>0)) break;
			if(!(*stream>>response)) break; //get response
			if(!isGood()) break;
	
			//if(!response.valid) continue;  //check for crc16
			//if(response.addr!=0x80) break;
			if(response.length!=(4+2*LIDAR_DATA_LENGTH)) break;
			if(response.data[0]!=0xB0) break;

			LidarData *data = new LidarData;
			data->min = 65535;
			//bool gap_started = false;
			//short int start_angle = 180; //right 90 degrees

			for(int i=0;i<LIDAR_DATA_LENGTH;i++)
			{
				data->distance[i] = WORD_TO_SHORT(response.data[3+2*i], response.data[3+2*i+1]);

				//find out the sharp point
				if(LIDAR_DATA_LENGTH/2-i<LIDAR_AOI_START*2&&LIDAR_DATA_LENGTH/2-i>LIDAR_AOI_END*2)
				{
					if((data->distance[i])<(data->min))
						data->min = data->distance[i];
				}

			}
			currentData = DataPointer(data); //create a new lidar data struct
		}
		while(false);
		mutex.leaveMutex();
		Thread::sleep(10);
	}
}

void Lidar::close()
{
	good = false;
	Thread::sleep(1000);
	mutex.enterMutex();
	if(stream) stream->Close();
	delete stream;
	mutex.leaveMutex();
}


DataPointer Lidar::getData(const unsigned char arg) const
{
	return currentData;
}
