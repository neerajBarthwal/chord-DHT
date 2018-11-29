#include <iostream>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <thread>

#include "init.h"
#include "connection.h"
#include "chordfunctions.h"
#include "util.h"
#include "nodedetails.h"
#include "chord.h"

using namespace std;

void initialize(NodeDetails &nodeDetails)
{

	/*
 	Open a socket to listen to other nodes
*/
	nodeDetails.sp.assignAndBindToIpAndPort();

	cout << "Started at port number: " << nodeDetails.sp.getPortNumber() << endl;

	cout << "Type help to know more\n";
}