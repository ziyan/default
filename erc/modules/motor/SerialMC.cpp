#include "SerialMC.h"

#define NUM_OF_MOTOR 4
#include <iostream>

using ost::Mutex;
using namespace erc;
using namespace std;
using namespace LibSerial;

SerialMC::SerialMC(DebugStream& debug, libconfig::Setting &setting)
 : Motor(debug,setting), servo(0)
{
	good = false;
	num_of_motor = NUM_OF_MOTOR;
	servo = new SerialStream((string)setting["device"]);
	if(!(*servo)) return;
	if(!(servo->IsOpen())) return;
	servo->SetBaudRate( LibSerial::SerialStreamBuf::BAUD_9600 );
	servo->SetCharSize( LibSerial::SerialStreamBuf::CHAR_SIZE_8 );
	servo->SetParity( LibSerial::SerialStreamBuf::PARITY_NONE );
	servo->SetNumOfStopBits( 1 );
	servo->SetFlowControl( LibSerial::SerialStreamBuf::FLOW_CONTROL_NONE );
	good = (*servo);
}

SerialMC::~SerialMC()
{
	servo->Close();
	delete servo;
}

bool SerialMC::setValue(const unsigned char motor, const unsigned char value)
{
	if (!good) return false;
	if (motor >= NUM_OF_MOTOR) return false;
	mutex.enterMutex();

	if(motor == 0)
		*servo << "L" << value << flush;
	else if(motor == 1)
		*servo << "R" << value << flush;
	good = (*servo);
	
	mutex.leaveMutex();
	return good;
}


