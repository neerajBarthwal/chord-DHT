#include <iostream>
#include <vector>
#include "init.h"
#include "connection.h"
#include "chordfunctions.h"
#include "util.h"
#include "nodedetails.h"
#include "chord.h"

using namespace std;
void processOneLengthCommands(NodeDetails &nodeDetails, string arg, vector<string> &arguments);
void processTwoLengthCommands(NodeDetails &nodeDetails, string arg, vector<string> &arguments);
void processThreeLengthCommands(NodeDetails &nodeDetails, string arg, vector<string> &arguments);

string my_ip;
int my_port;
int main(int argc, char*argv[])
{

	NodeDetails nodeDetails;
	if(argc==1){
		cout<<"Enter IP:PORT\n"<<endl;
		exit(0);
	}

	if (mkdir(my_files_path, 0777) == -1) 
        cerr << "Error in creating files Directory:  " << strerror(errno) << endl; 
  
    else
        cout << "Directory created"; 

	string address = argv[1];

	Utility util;
	pair<string,int> ipAndPort = util.getIpAndPort(address);

	my_ip = ipAndPort.first;
	my_port = ipAndPort.second;

	initialize(nodeDetails);

	string command;
	showMenuDriven();
	while (1)
	{
		printf("Enter Command\n");
		getline(cin, command);

		if(command=="")
			continue;

		Utility util = Utility();
		vector<string> arguments = util.split_string(command);

		string arg = arguments[0];
		if (arguments.size() == 1)
		{
			/* Creates */
			processOneLengthCommands(nodeDetails, arg, arguments);
		}

		else if (arguments.size() == 2)
		{

			processTwoLengthCommands(nodeDetails, arg, arguments);
			cout << " Executed basic command successfully\n";
		}

		else if (arguments.size() == 3)
		{

			processThreeLengthCommands(nodeDetails, arg, arguments);
		}

		else
		{
			cout << "#------------------- Invalid Command ---------------------#\n";
		}
	}

	return 0;
}

void processOneLengthCommands(NodeDetails &nodeDetails, string arg, vector<string> &arguments)
{
	if (arg == "create")
	{
		if (nodeDetails.getStatus() == true)
		{
			cout << "Could not initialize new ring. This node is already on the ring.\n";
		}
		else
		{
			thread first(create, ref(nodeDetails));
			first.detach();
			// 					create(nodeDetails);
		}
	}

	else if (arg == "printdetails")
	{
		if (nodeDetails.getStatus() == false)
		{
			cout << "This node is not in the ring.\n";
		}
		else
			printdetails(nodeDetails);
	}

	else if (arg == "leave")
	{
		leave(nodeDetails);
		nodeDetails.sp.closeSocket();
		exit(0);
	}

	else if (arg == "port")
	{
		cout << nodeDetails.sp.getPortNumber() << endl;
	}

	else if (arg == "print")
	{
		if (nodeDetails.getStatus() == false)
		{
			cout << "This node is not in the ring.\n";
		}
		else
			nodeDetails.printKeys();
	}

	else
	{
		cout << "#------------------- Invalid Command ---------------------#\n";
	}
}

void processTwoLengthCommands(NodeDetails &nodeDetails, string arg, vector<string> &arguments)
{
	if(arg == "join"){
				if(nodeDetails.getStatus() == true){
					cout<<"This node is already on the ring.\n";
				}
				else{
					Utility util;
					string address=arguments[1];
					pair<string,int> ipAndPort = util.getIpAndPort(address);

					string ip = ipAndPort.first;
					int port = ipAndPort.second;
					//cout<<"joining "<<ip<<":"<<port<<"\n";
					join(ref(nodeDetails),ip,to_string(port));
/* 					thread one(join,ref(nodeDetails),arguments[1],arguments[2]);
					one.detach(); */
				}

		}

	else if (arg == "get")
	{
		if (nodeDetails.getStatus() == false)
		{
			cout << "This node is not in the ring.\n";
		}
		else
			get(arguments[1], nodeDetails);
	}

	else if (arg == "download")
	{
		if (nodeDetails.getStatus() == false)
		{
			cout << "This node is not in the ring.\n";
		}
		else
			download(nodeDetails,arguments[1]);
	}

	else if(arg == "put"){
		if(nodeDetails.getStatus() == false){
			cout<<"This node is not in the ring.\n";
		}
		else{
			string address=my_ip+":"+to_string(my_port);
			put(arguments[1],address,nodeDetails);
		}
	}

	else
	{
		cout << "#------------------- Invalid Command ---------------------#\n";
	}
}

void processThreeLengthCommands(NodeDetails &nodeDetails, string arg, vector<string> &arguments)
{
	if(arg == ""){

	}

	else
	{
		cout << "#------------------- Invalid Command ---------------------#\n";
	}
}
