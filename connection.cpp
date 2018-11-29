#include <iostream>

#include "chord.h"
#include "connection.h"
#include "util.h"

/* 
    Common function to connect socket.
*/
int SocketAndPort::connect_socket(string ip,string port)
{
	Utility util;
	struct sockaddr_in serverToConnectTo;
	socklen_t l = sizeof(serverToConnectTo);
	util.setServerDetails(serverToConnectTo,ip,stoi(port));
	int sock = socket(AF_INET,SOCK_STREAM,0);
	if(sock < 0){
		perror("error");
		return -1;
	}
	int ret = connect(sock, (struct sockaddr *)&serverToConnectTo, sizeof(serverToConnectTo));
	if (ret < 0)
	{
//		printf("Error In Connection Connection.cpp\n");
		return -1;
	}
	return sock;
}
 
/* 
    Assign and bind to port.
*/

void SocketAndPort::assignAndBindToIpAndPort(){

	/* generating a port number between 1024 and 65535 */
	srand(time(0));
	portNoServer = rand() % 65536;
	if(portNoServer < 1024)
		portNoServer += 1024;

	socklen_t len = sizeof(current);

	sock = socket(AF_INET,SOCK_STREAM,0);
	current.sin_family = AF_INET;
	current.sin_port = htons(portNoServer);
	current.sin_addr.s_addr = inet_addr("10.42.0.1");

	if( bind(sock,(struct sockaddr *)&current,len) < 0){
		perror("error");
		exit(-1);
	}

}

/* 
    Change port number.
*/
void SocketAndPort::changePortNumber(int newPortNumber){
	if(newPortNumber < 1024 || newPortNumber > 65535){
		cout<<"Please enter a valid port number\n";
	}
	else{
		if( isPortInUse(newPortNumber) ){
			cout<<"Sorry but port number is already in use\n";
		}
		else{
			close(sock);
			socklen_t len = sizeof(current);
			sock = socket(AF_INET,SOCK_STREAM,0); 
			current.sin_port = htons(newPortNumber);
			if( bind(sock,(struct sockaddr *)&current,len) < 0){
				perror("error");
				current.sin_port = htons(portNoServer);
			}
			else{
				portNoServer = newPortNumber;
				cout<<"Port number changed to : "<<portNoServer<<endl;
			}
		}
	}
}

/* check if a port number is already in use */
bool SocketAndPort::isPortInUse(int portNo){
	int newSock = socket(AF_INET,SOCK_STREAM,0);

	struct sockaddr_in newCurr;
	socklen_t len = sizeof(newCurr);
	newCurr.sin_port = htons(portNo);
	newCurr.sin_family = AF_INET;
	newCurr.sin_addr.s_addr = inet_addr("10.42.0.1");
	
	if( bind(newSock,(struct sockaddr *)&newCurr,len) < 0){
		perror("error");
		return true;
	}
	else{
		close(newSock);
		return false;
	}
}

/* get IP Address */
string SocketAndPort::getIpAddress(){
	string ip = inet_ntoa(current.sin_addr);
	return ip;
}

/* get port number on which it is listening */
int SocketAndPort::getPortNumber(){
	return portNoServer;
}

int SocketAndPort::getSocketFd(){
	return sock;
}

/* close socket */
void SocketAndPort::closeSocket(){
	close(sock);
}
