/**
 * Main file for erc, The Extensible Robot Core
 * The start procedure is simple:
 *	1. create daemon object
 *	2. start up sql connection
 *	3. start up network server
 */

#include "common/define.h"
#include "common/DebugStream.h"
#include "daemon/Daemon.h"
#include <iostream>
#include <vector>
#include <string>
#include <getopt.h>
#include <libconfig.h++>

#define CONFIG_FILE "/etc/erc/erc.config"
#define DEFAULT_PORT 8888

using namespace erc;
using namespace std;
using namespace libconfig;


bool killed = false;
void kill(int i = 0)
{
	killed = true;
}

int main(int argc, char *argv[])
{
	//reset random number
	srand(time(0));

	//register kill signals
	signal(SIGINT, kill);
	signal(SIGTERM, kill);
	signal(SIGABRT, kill);

	//settings
	char *config_path = CONFIG_FILE;
	unsigned short int port = DEFAULT_PORT;
	bool port_specified = false;
	bool verbose = false;
	bool start_control = false;
	bool no_clean = false;
	vector<std::string> modules;

	//set up short commands
	char options[]="c:m:p:vhan";

	//set up long commands
	static struct option long_options[] =
	{
		{"verbose",	no_argument,		0,	'v'},
		{"help",	no_argument,		0,	'h'},
		{"auto",	no_argument,		0,	'a'},
		{"module",	required_argument,	0,	'm'},
		{"config",	required_argument,	0,	'c'},
		{"port",	required_argument,	0,	'p'},
		{"no-clean",no_argument,		0,	'n'},
		{0, 0, 0, 0}
	};
	int flag, long_index;	

	while((flag=getopt_long(argc, argv, options, long_options,&long_index)) != -1)
	{
		switch(flag)
		{
			case 'h':
				cout<<"Usage: "<<argv[0]<<" [options]"<<endl;
				cout<<endl;
				cout<<"  -a, --auto"<<endl;
				cout<<"\tAuto start up control system after loading modules."<<endl<<endl;

				cout<<"  -h, --help"<<endl;
				cout<<"\tPrint help message."<<endl<<endl;

				cout<<"  -v, --verbose"<<endl;
				cout<<"\tUse verbose mode."<<endl<<endl;
				
				cout<<"  -n, --no-clean"<<endl;
				cout<<"\tDo not change configuration file."<<endl<<endl;

				cout<<"  -c config-path, --config config-path"<<endl;
				cout<<"\tOverwrite default config file path."<<endl<<endl;

				cout<<"  -m module-name, --module module-name"<<endl;
				cout<<"\tSchedule module start up."<<endl<<endl;
				
				cout<<"  -p port, --port port"<<endl;
				cout<<"\tOverwrite port number in configuration file."<<endl<<endl;

				exit(0);
			case 'c':
				config_path = optarg;
				break;
			case 'm':
				modules.push_back(optarg);
				break;
			case 'p':
				port = atoi(optarg);
				port_specified = true;
				break;
			case 'a':
				start_control = true;
				break;
			case 'v':
				verbose = true;
				break;
			case 'n':
				no_clean = true;
				break;
			case '?':
				cerr<<"Type in '"<<argv[0]<<" --help' to find out more."<<endl<<endl;
				exit(1);
			default:
				cerr<<"Type in '"<<argv[0]<<" --help' to find out more."<<endl<<endl;
				exit(1);
		}
	}
	//setup Debug stream
	DebugStream debug;
	if(verbose) debug.reg(&cout);
		
	//start daemon
	try
	{
		//setup Configuration cache
		Config config;
		config.readFile(config_path);
		if(!port_specified)
		{
			try
			{
				port = (int)config.lookup("network.port") & 0xFFFF;
			}
			catch(...)
			{
				debug << "erc: port setting does not exist in configuration file."<<endl;
				debug << "erc: using default port "<<port<<"."<<endl;
			}
		} 
		{
			Daemon daemon(debug,port,config);
			for(int i=0;i<modules.size();i++)
				daemon.loadModule(modules[i]);

			if(start_control)
			{
				sleep(3);
				daemon.startControl();
			}
			while(!killed && daemon.isGood())
			{
				usleep(100000);
			}
		}
		if(!no_clean)
		{
			debug << "erc: saving configuration into \""<<config_path<<"\"."<<endl;
			config.writeFile(config_path);
			debug << "erc: configuration saved."<<endl;
		}
		else
		{
			debug << "erc: no clean specified, leave the configuration file alone."<<endl;
		}
	}
	catch(ParseException &e)
	{
		debug << "erc: error when parsing configuration file \""<<config_path<<"\"."<<endl;
		debug << "erc: line "<<e.getLine()<<" : "<<e.getError()<<"."<<endl;
		return -1;
	}
	catch(FileIOException &e)
	{
		debug << "erc: error when trying to access configuration file \""<<config_path<<"\"."<< endl;
		debug << "erc: please specify another configuration file using -c option."<<endl;
		return -1;
	}
	catch(...)
	{
		debug << "erc: error when trying to start network server at port "<<port<<"."<< endl;
		debug << "erc: please specify another port using -p option."<<endl;
		return -1;
	}
}
