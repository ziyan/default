/**
 * Main file for erctool, The Remote Tool for Extensible Robot Core
 */

#include <iostream>

#include "common/define.h"
#include "modules/Modules.h"
#include "network/NetworkClient.h"
#include "network/NetworkDefine.h"

using namespace std;
using namespace erc;


void display(DataPointer pointer)
{
	if(pointer.isNull())
	{
		cerr << "erctool: Remote server returned a null pointer."<<endl;
		return;
	}
	switch(pointer->getType())
	{
		case DATATYPE_GPSDATA:
		{
			GpsData &data = (GpsData&)(*pointer);
			cout<<"Gps:"<<endl
			<<"    Latitude  = "<<data.latitude<<endl
			<<"    Longitude = "<<data.longitude<<endl
			<<"    Altitude  = "<<data.altitude<<endl
			<<"    Speed     = "<<data.speed<<endl
			<<"    Climb     = "<<data.climb<<endl
			<<"    X         = "<<data.x<<endl
			<<"    Y         = "<<data.y<<endl
			<<"    Zone      = "<<data.zone<<endl<<endl;
			break;
		}
		case DATATYPE_LIDARDATA:
		{
			LidarData &data = (LidarData&)(*pointer);
			cout<<"Lidar Data:"<<endl;
			for(int i=0;i<361;i++)
			{
				cout<<(double)i/2<<"\t"<<data.distance[i]<<"\t";
				if(i%9==8) cout<<endl;
			}
			cout<<endl<<endl;
			break;
		}
		case DATATYPE_CAMERADATA:
		{
			CameraData &data = (CameraData&)(*pointer);
			cout<<"Camera Data:"<<endl;
			cout<<"\tSize:\t"<<data.width<<"x"<<data.height<<endl;
			cout<<"\tStatus:\t"<<(int)data.state<<endl;
			cout<<"\tLeft:\t("<<data.left.p1.x<<","<<data.left.p1.y<<")\t("<<data.left.p2.x<<","<<data.left.p2.y<<")"<<endl;
			cout<<"\tCenter:\t("<<data.center.p1.x<<","<<data.center.p1.y<<")\t("<<data.center.p2.x<<","<<data.center.p2.y<<")"<<endl;
			cout<<"\tRight:\t("<<data.right.p1.x<<","<<data.right.p1.y<<")\t("<<data.right.p2.x<<","<<data.right.p2.y<<")"<<endl<<endl;
			break;
		}
		case DATATYPE_COMPASSDATA:
		{
			CompassData &data = (CompassData&)(*pointer);
			cout<<"Compass Data:"<<endl;
			cout<<"\t"<<((double)data.direction/10.0)<<endl<<endl;
			break;
		}
		case DATATYPE_MOTORDATA:
		{
			MotorData &data = (MotorData&)(*pointer);
			cout<<"Motor Data:"<<endl;
			cout<<"\tNumber of motors: "<<(int)data.num<<endl;
			for(int i=0;i<data.num;i++)
			{
				cout<<"\t  Motor "<<i<<" current speed = "<<(int)data.value[i]<<endl;
			}
			cout<<endl;
			break;
		}
		case DATATYPE_JAUSDATA:
		{
			JausData &data = (JausData&)(*pointer);
			cout<<"Jaus Data:"<<endl;
			cout<<"\tMobility: "<<(data.mobility?"true":"false")<<endl;
			cout<<"\tWarning:  "<<(data.warning?"true":"false")<<endl;
			cout<<endl;
			break;
		}
		case DATATYPE_PARALLELPORTDATA:
		{
			ParallelPortData &data = (ParallelPortData&)(*pointer);
			cout<<"Parallel Port Data:"<<endl;
			cout<<"\tData ports: ";
			for(int i=0;i<8;i++) cout<<data.data[i];
			cout<<endl;
			cout<<"\tStatus Ports: ";
			for(int i=0;i<5;i++) cout<<data.status[i];
			cout<<endl;
			cout<<endl;
			break;
		}
		default:
			cerr<<"erctool: Unkown datatype "<<(int)pointer->getType()<<"."<<endl;
	}
}



int main(int argc, char *argv[])
{
	if(argc<4||(argc>=4&&string(argv[3])=="help"))
	{
		cout<<"Usage: "<<argv[0]<<" hostname port action [additional arguments]"<<endl<<endl;
		cout<<"   Avaliable actions:"<<endl;
		cout<<"   mod                           - List currently running modules."<<endl;
		cout<<"   modall                        - List all available modules."<<endl;
		cout<<"   modload module-name           - Load a module on remote server."<<endl;
		cout<<"   modunload module-name         - Unload a module on remote server."<<endl;
		cout<<"   modget module-name arg [msec] - Get current data from module."<<endl;
		cout<<"   modstat module-name           - Fetch the status of a module."<<endl;
		cout<<"   controlstart                  - Start the control loop."<<endl;
		cout<<"   controlstop                   - Stop the control loop."<<endl;
		cout<<"   controlstat                   - Fetch the status of control loop."<<endl;
		cout<<"   configget key                 - Fetch the configuration value."<<endl;
		cout<<"   configset key [setting]       - Set or remove the configuration."<<endl;
		cout<<"   debug                         - Fetch the debug stream."<<endl;
		cout<<"   halt                          - Halt remote ERC."<<endl;
		cout<<endl;
		return 1;
	}

	try
	{
		NetworkClient client(argv[1],atoi(argv[2]));

		string action = argv[3];
		if(action=="modload")
		{
			if(argc!=4+1)
			{
				cerr<<"Usage: "<<argv[0]<<" hostname port modload module-name"<<endl;
				return -1;
			}
			if(client.loadModule(argv[4]))
			{
				cout<<"erctool: Module "<<argv[4]<<" loaded."<<endl;
			}
			else
			{
				cerr<<"erctool: Failed to load Module."<<endl;
			}
		}
		else if(action=="modunload")
		{
			if(argc!=4+1)
			{
				cerr<<"Usage: "<<argv[0]<<" hostname port modunload module-name"<<endl;
				return -1;
			}
			if(client.unloadModule(argv[4]))
			{
				cout<<"erctool: Module "<<argv[4]<<" unloaded."<<endl;
			}
			else
			{
				cerr<<"erctool: Failed to unload Module."<<endl;
			}
		}
		else if(action=="controlstart")
		{
			if(argc!=4)
			{
				cerr<<"Usage: "<<argv[0]<<" hostname port controlstart"<<endl;
				return -1;
			}
			if(client.startControl())
			{
				cout<<"erctool: Control loop started."<<endl;
			}
			else
			{
				cerr<<"erctool: Failed to start control loop."<<endl;
			}
		}
		else if(action=="controlstop")
		{
			if(argc!=4)
			{
				cerr<<"Usage: "<<argv[0]<<" hostname port controlstop"<<endl;
				return -1;
			}
			if(client.stopControl())
			{
				cout<<"erctool: Control loop stopped."<<endl;
			}
			else
			{
				cerr<<"erctool: Failed to stop control loop."<<endl;
			}
		}
		else if(action=="controlstat")
		{
			if(argc!=4)
			{
				cerr<<"Usage: "<<argv[0]<<" hostname port controlstat"<<endl;
				return -1;
			}
			unsigned char status = client.getControlStatus();
			cout<<"erctool: Control loop status:\t";
			switch(status)
			{
				case NETWORK_STATUS_RUNNING:
					cout<<"Running"<<endl;
					break;
				case NETWORK_STATUS_ERROR:
					cout<<"Error"<<endl;
					break;
				case NETWORK_STATUS_STOP:
					cout<<"Stopped"<<endl;
					break;
				case NETWORK_STATUS_NOT_STARTED:
					cout<<"Not started"<<endl;
					break;
				default:
					cout<<"Failed to query"<<endl;
			}
		}
		else if(action=="modstat")
		{
			if(argc!=5)
			{
				cerr<<"Usage: "<<argv[0]<<" hostname port modstat module-name"<<endl;
				return -1;
			}
			unsigned char status = client.getModuleStatus(argv[4]);
			cout<<"erctool: Module "<<argv[4]<<" status:\t";
			switch(status)
			{
				case NETWORK_STATUS_RUNNING:
					cout<<"Running"<<endl;
					break;
				case NETWORK_STATUS_ERROR:
					cout<<"Error"<<endl;
					break;
				case NETWORK_STATUS_STOP:
					cout<<"Stopped"<<endl;
					break;
				case NETWORK_STATUS_NOT_STARTED:
					cout<<"Not started"<<endl;
					break;
				default:
					cout<<"Failed to query"<<endl;
			}
		}
		else if(action=="mod")
		{
			if(argc!=4)
			{
				cerr<<"Usage: "<<argv[0]<<" hostname port mod"<<endl;
				return -1;
			}
			map<string,unsigned char>::const_iterator i;
			unsigned char status;
			map<string,unsigned char> list = client.getLoadedModules();
			cout<<"erctool: Loaded modules:"<<endl;
			cout<<"\tModule-name\tModule-Type\tModule-Status"<<endl;
			for(i = list.begin(); i != list.end(); ++i)
			{
				status = client.getModuleStatus(i->first);
				cout<<"\t"<<i->first<<"\t\t"<<(int)(i->second)<<"\t\t";
				switch(status)
				{
					case NETWORK_STATUS_RUNNING:
						cout<<"Running"<<endl;
						break;
					case NETWORK_STATUS_ERROR:
						cout<<"Error"<<endl;
						break;
					case NETWORK_STATUS_STOP:
						cout<<"Stopped"<<endl;
						break;
					case NETWORK_STATUS_NOT_STARTED:
						cout<<"Not started"<<endl;
						break;
					default:
						cout<<"Failed to query"<<endl;
				}
			}
		}
		else if(action=="modall")
		{
			if(argc!=4)
			{
				cerr<<"Usage: "<<argv[0]<<" hostname port modall"<<endl;
				return -1;
			}
			map<string,unsigned char>::const_iterator i;
			unsigned char status;
			map<string,unsigned char> list = client.getAvailableModules();
			cout<<"erctool: Available modules:"<<endl;
			cout<<"\tModule-name\tModule-Type\tModule-Status"<<endl;
			for(i = list.begin(); i != list.end(); ++i)
			{
				status = client.getModuleStatus(i->first);
				cout<<"\t"<<i->first<<"\t\t"<<(int)(i->second)<<"\t\t";
				switch(status)
				{
					case NETWORK_STATUS_RUNNING:
						cout<<"Running"<<endl;
						break;
					case NETWORK_STATUS_ERROR:
						cout<<"Error"<<endl;
						break;
					case NETWORK_STATUS_STOP:
						cout<<"Stopped"<<endl;
						break;
					case NETWORK_STATUS_NOT_STARTED:
						cout<<"Not started"<<endl;
						break;
					default:
						cout<<"Failed to query"<<endl;
				}
			}
		}
		else if(action=="modget")
		{
			if(argc!=4+2&&argc!=4+3)
			{
				cerr<<"Usage: "<<argv[0]<<" hostname port modget module-name arg [msec]"<<endl;
				return -1;
			}
			int interval = 0;
			if(argc==4+3) interval = atoi(argv[6]);
			do
			{
				DataPointer ptr = client.getModuleData(argv[4],atoi(argv[5]));
				if(ptr.isNull())
				{
					cerr<<"erctool: Failed to get data."<<endl;
					return -1;
				}
				display(ptr);
				if(interval>0) usleep(1000*interval);
			}
			while(interval>0);
		}
		
		else if(action=="configgets")
		{
			if(argc!=5)
			{
				cerr<<"Usage: "<<argv[0]<<" hostname port configgets key"<<endl;
				return -1;
			}
			cout<<"erctool: "<<argv[4]<<" = "<<client.getStringConfig(argv[4])<<endl;
		}
		else if(action=="configsets")
		{
			if(argc!=5&&argc!=6)
			{
				cerr<<"Usage: "<<argv[0]<<" hostname port configsets key [setting]"<<endl;
				return -1;
			}
			string value = "";
			if(argc==6) value = argv[5];
			if(client.setStringConfig(argv[4],value))
			{
				cout<<"erctool: "<<argv[4]<<" = "<<client.getStringConfig(argv[4])<<endl;
			}
			else
			{
				cout<<"erctool: Failed to set value."<<endl;
			}
		}
		
		else if(action=="configgeti")
		{
			if(argc!=5)
			{
				cerr<<"Usage: "<<argv[0]<<" hostname port configgeti key"<<endl;
				return -1;
			}
			cout<<"erctool: "<<argv[4]<<" = "<<client.getIntConfig(argv[4])<<endl;
		}
		else if(action=="configseti")
		{
			if(argc!=5&&argc!=6)
			{
				cerr<<"Usage: "<<argv[0]<<" hostname port configseti key [setting]"<<endl;
				return -1;
			}
			int value = 0;
			if(argc==6) value = atoi(argv[5]);
			if(client.setIntConfig(argv[4],value))
			{
				cout<<"erctool: "<<argv[4]<<" = "<<client.getIntConfig(argv[4])<<endl;
			}
			else
			{
				cout<<"erctool: Failed to set value."<<endl;
			}
		}
		else if(action=="configgetb")
		{
			if(argc!=5)
			{
				cerr<<"Usage: "<<argv[0]<<" hostname port configgetb key"<<endl;
				return -1;
			}
			cout<<"erctool: "<<argv[4]<<" = "<<client.getBoolConfig(argv[4])<<endl;
		}
		else if(action=="configsetb")
		{
			if(argc!=5&&argc!=6)
			{
				cerr<<"Usage: "<<argv[0]<<" hostname port configsetb key [setting]"<<endl;
				return -1;
			}
			bool value = false;
			if(argc==6) value = (string(argv[5])=="true");
			if(client.setBoolConfig(argv[4],value))
			{
				cout<<"erctool: "<<argv[4]<<" = "<<client.getBoolConfig(argv[4])<<endl;
			}
			else
			{
				cout<<"erctool: Failed to set value."<<endl;
			}
		}
		else if(action=="configgetd")
		{
			if(argc!=5)
			{
				cerr<<"Usage: "<<argv[0]<<" hostname port configgetd key"<<endl;
				return -1;
			}
			cout<<"erctool: "<<argv[4]<<" = "<<client.getDoubleConfig(argv[4])<<endl;
		}
		else if(action=="configsetd")
		{
			if(argc!=5&&argc!=6)
			{
				cerr<<"Usage: "<<argv[0]<<" hostname port configsetd key [setting]"<<endl;
				return -1;
			}
			double value = 0.0;
			if(argc==6)
			{
				stringstream ss;
				ss << argv[5];
				ss >> value;
			}
			if(client.setDoubleConfig(argv[4],value))
			{
				cout<<"erctool: "<<argv[4]<<" = "<<client.getDoubleConfig(argv[4])<<endl;
			}
			else
			{
				cout<<"erctool: Failed to set value."<<endl;
			}
		}
		else if(action=="debug")
		{
			if(argc!=4)
			{
				cerr<<"Usage: "<<argv[0]<<" hostname port debug"<<endl;
				return -1;
			}
			istream *debug = client.getDebugStream();
			if(debug)
			{
				cout<<"erctool: going into debug stream output mode."<<endl;
				char c;
				while(*debug)
				{
					(*debug).get(c);
					cout<<c;
				}
			}
			else
			{
				cerr<<"erctool: Failed to get debug stream."<<endl;
			}
		}
		else if(action=="online")
		{
			if(argc!=4)
			{
				cerr<<"Usage: "<<argv[0]<<" hostname port online"<<endl;
				return -1;
			}
			vector<string> v = client.getActiveClients();
			cout<<"erctool: online active client connections:"<<endl;
			for(vector<string>::const_iterator i = v.begin();i!=v.end();++i)
			{
				cout<< "\t" << (*i) <<endl;
			}
		}
		else if(action=="halt")
		{
			if(argc!=4)
			{
				cerr<<"Usage: "<<argv[0]<<" hostname port halt"<<endl;
				return -1;
			}
			client.halt();
		}
		else
		{
			cerr<<"erctool: Action not recognized."<<endl;
		}
	}
	catch(...)
	{
		cerr<<"erctool: Error, cannot connnect to remote server."<<endl;
	}
	cout<<endl;
}

