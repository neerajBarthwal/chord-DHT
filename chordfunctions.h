/**
    * Team: Fargo
    * Members: Amit , Ritik , Suchi , Neeraj
    * Subject: Operating System
    * Project: Chord
**/

#ifndef chordfunctions_h
#define chordfunctions_h

#include <iostream>
#include "chord.h"
#include "connection.h"
#include "nodedetails.h"

using namespace std;

typedef long long int lli;

void put(string key,string value,NodeDetails &nodeDetails);
void get(string key,NodeDetails nodeDetails);
void download(NodeDetails &nodeDetails,string key);
void create(NodeDetails &nodeDetails);
void join(NodeDetails &nodeDetails,string ip,string port);
void printdetails(NodeDetails nodeDetails);
void listenTo(NodeDetails &nodeDetails);
void doStabilize(NodeDetails &nodeDetails);
void callNotify(NodeDetails &nodeDetails,string ipAndPort);
void callFixFingers(NodeDetails &nodeDetails);
void doTask(NodeDetails &nodeDetails,int newSock,struct sockaddr_in client,string nodeIdString);
void leave(NodeDetails &nodeDetails);
void showMenuDriven();

void requestDownload(NodeDetails nodeDetails,string ip,int port,string fileName);

#endif