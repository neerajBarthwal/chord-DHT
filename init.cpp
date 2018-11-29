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

	cout << "\n";
	cout << "Port number assigned: " << nodeDetails.sp.getPortNumber() << endl;
	cout << "Welcome to Fargo Chord\n\n";
}