# Makefile for roboerc - the Extensible Robot Core

# compiler
CC = g++
# flag
FLAG = -ggdb
# include header files path
INC = -I/usr/local/include/opencv -I/usr/include/opencv -I/usr/include/cc++2
# subdir make options (no need to modify)
CXX = $(CC) $(FLAG) $(INC)
# library paths
LIBS = -lccgnu2 -ldl -lserial -lsqlite3 -lgps -lpthread -lphidget21 -lm -lcv -lhighgui -lcvaux -lconfig++ -L /usr/lib -L /usr/local/lib
# start script
START_SCRIPT = script/sh/erc

# targets
all: erc

# dependency
MODULE_DEP = modules/Module.o modules/LocalModule.o modules/RemoteModule.o modules/gps/Gps.o modules/lidar/Lidar.o modules/lidar/LidarDataPacket.o modules/camera/Camera.o modules/motor/Motor.o modules/motor/PhidgetMC.o modules/motor/SerialMC.o modules/compass/Compass.o modules/joystick/JoyStick.o modules/jaus/Jaus.o modules/parallel/ParallelPort.o
DATA_DEP = modules/Data.o modules/DataPointer.o modules/gps/LatLong-UTMconversion.o modules/gps/UTMConversion.o modules/gps/GpsData.o modules/lidar/LidarData.o modules/camera/CameraFrame.o modules/camera/CameraData.o modules/motor/MotorData.o modules/compass/CompassData.o modules/joystick/JoyStickData.o modules/jaus/JausData.o modules/parallel/ParallelPortData.o

# CONTROL object
CONTROL_DEP = control/Control.o control/PID.o control/DiGraph/Edge.o control/DiGraph/Vertex.o

# network object
NETWORK_DEP = network/NetworkClient.o network/NetworkDataPacket.o
SERVER_DEP = network/NetworkServer.o network/NetworkSession.o

# common object
COMMON_DEP = common/crc16.o common/timedrecv.o common/Point.o common/Line.o common/DebugStream.o

# daemon object
DAEMON_DEP = daemon/Daemon.o

# add up dependency
DEP = $(MODULE_DEP) $(DATA_DEP) $(CONTROL_DEP) $(SERVER_DEP) $(NETWORK_DEP) $(COMMON_DEP) $(DAEMON_DEP)



# making erc
erc: erc.o $(DEP)
	$(CC) $(FLAG) -o erc erc.o $(DEP) $(LIBS)

# making erctool
erctool: erctool.o $(NETWORK_DEP) $(DATA_DEP) $(COMMON_DEP)
	$(CC) $(FLAG) -o erctool erctool.o $(NETWORK_DEP) $(DATA_DEP) $(COMMON_DEP) $(LIBS)

# clean up object files
clean:
	find . -iname \*.o -exec rm -f {} \;

# realclean
realclean: clean clear
	rm -f erc erctool

# clear temp files
clear:
	find . -iname \*~ -exec rm -f {} \;
	find . -iname \*.tmp -exec rm -f {} \;

# install erc into sbin
install: erc erctool
	cp erc /usr/sbin/erc
	cp erctool /usr/bin/erctool
	chmod 755 /usr/sbin/erc
	mkdir -p /etc/erc/
	cp erc.config /etc/erc/erc.config
	chmod -R 775 /etc/erc
	cp $(START_SCRIPT) /etc/init.d/erc
	chmod 755 /etc/init.d/erc

# uninstall erc
uninstall:
	rm -f /usr/sbin/erc
	rm -f /usr/bin/erctool
	rm -f /etc/init.d/erc
	rm -Rf /etc/erc
