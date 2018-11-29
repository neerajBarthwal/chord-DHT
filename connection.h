/**
    * Team: Fargo
    * Members: Amit , Ritik , Suchi , Neeraj
    * Subject: Operating System
    * Project: Chord
**/

#ifndef connection_h
#define connection_h

#include <iostream>
#include <netinet/in.h>
#include "chord.h"
using namespace std;

class SocketAndPort
{
  private:
	int portNoServer;
	int sock;
	struct sockaddr_in current;

  public:
	void assignAndBindToIpAndPort();
	void changePortNumber(int portNo);
	void closeSocket();
	bool isPortInUse(int portNo);
	string getIpAddress();
	int getPortNumber();
	int getSocketFd();
	int connect_socket(string, string);
};

#endif