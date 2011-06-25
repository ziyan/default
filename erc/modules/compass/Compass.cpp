/**
 * Module wrapper for compass CMPS303
 */

#include "Compass.h"
#include "CompassData.h"
#include "../../common/define.h"

#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <linux/i2c-dev.h>

using namespace erc;
using namespace std;

#define CMPS03_ADDR 0x60
#define CMPS03_SOFTWARE_REVISION 0x0
#define CMPS03_BEARING_BYTE 0x1
#define CMPS03_BEARING_WORD_HIGH 0x2
#define CMPS03_BEARING_WORD_LOW 0x3
#define CMPS03_CALIBRATE_CMD 0xF

Compass::Compass(DebugStream& debug, libconfig::Setting &setting)
  : LocalModule(debug,setting),good(false),currentData(new CompassData)
{
	fd = open(setting["device"],O_RDWR);
	if(fd<0) return;
	long funcs;
	if(ioctl(fd,I2C_FUNCS,&funcs)<0) return;
	if(!(funcs & I2C_FUNC_SMBUS_QUICK)) return;
	if(ioctl(fd,I2C_SLAVE,CMPS03_ADDR)<0) return;
	if(i2c_smbus_read_byte_data(fd, CMPS03_SOFTWARE_REVISION)<0) return;
	good = true;
}

Compass::~Compass()
{
}

bool Compass::isGood() const
{
	return good && (fd>=0);
}

void Compass::run()
{
	if(!isGood()) return;
	int h,l;
	while(isGood())
	{
		sleep(10);
		if(!isGood()) break;
		mutex.enterMutex();
		h = i2c_smbus_read_byte_data(fd, CMPS03_BEARING_WORD_HIGH);
		if (h < 0) continue;
		l = i2c_smbus_read_byte_data(fd, CMPS03_BEARING_WORD_LOW);
		if (l < 0) continue;
		
		((CompassData&)(*currentData)).direction = WORD_TO_SHORT(l,h);
		//cout<<WORD_TO_SHORT(l,h)<<endl;
		mutex.leaveMutex();
		sleep(10);
	}
	good = false;
}

DataPointer Compass::getData(const unsigned char arg) const
{
	return currentData;
}

void Compass::close()
{
	good = false;
	sleep(100);
	mutex.enterMutex();
	if(fd>=0) ::close(fd);
	mutex.leaveMutex();
}


