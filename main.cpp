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

int main()
{

	NodeDetails nodeDetails;

	initialize(nodeDetails);

	string command;
	showMenuDriven();
	while (1)
	{
		printf("Enter Command\n");
		getline(cin, command);

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

	if (arg == "get")
	{
		if (nodeDetails.getStatus() == false)
		{
			cout << "This node is not in the ring.\n";
		}
		else
			get(arguments[1], nodeDetails);
	}

	else
	{
		cout << "#------------------- Invalid Command ---------------------#\n";
	}
}

void processThreeLengthCommands(NodeDetails &nodeDetails, string arg, vector<string> &arguments)
{
	if (arg == "join")
	{
		if (nodeDetails.getStatus() == true)
		{
			cout << "This node is already on the ring.\n";
		}
		else
		{
			join(ref(nodeDetails), arguments[1], arguments[2]);
			/*
	thread one(join,ref(nodeDetails),arguments[1],arguments[2]);
	one.detach();
*/
		}
	}

	else if (arg == "put")
	{
		if (nodeDetails.getStatus() == false)
		{
			cout << "This node is not in the ring.\n";
		}
		else
			put(arguments[1], arguments[2], nodeDetails);
	}

	else
	{
		cout << "#------------------- Invalid Command ---------------------#\n";
	}
}
