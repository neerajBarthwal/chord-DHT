/**
    * Team: Fargo
    * Members: Amit , Ritik , Suchi , Neeraj
    * Subject: Operating System
    * Project: Chord
**/

#ifndef FINGER_H
#define FINGER_H

#include "chord.h"
using namespace std;
class Finger
{

  public:
	ll identifier; //chord identifier
	string ip;
	int port;
	string address;		   //ip:port
	ll start;			   // start of interval
	pair<ll, ll> interval; // semi-open [ll,ll)

	Finger()
	{
	}

	Finger(ll identifier, string ip, int port, pair<ll, ll> interval)
	{
		this->identifier = identifier;
		this->ip = ip;
		this->port = port;
		this->interval = interval;
		this->start = 0;
		this->address = ip + ":" + to_string(port); // address is of the form ==> IP:PORT
	}

	const string &getAddress() const
	{
		return address;
	}

	void setAddress(const string &address)
	{
		this->address = this->ip + ":" + to_string(this->port);
		;
	}

	ll getIdentifier() const
	{
		return identifier;
	}

	void setIdentifier(ll identifier)
	{
		this->identifier = identifier;
	}

	const pair<ll, ll> &getInterval() const
	{
		return interval;
	}

	void setInterval(const pair<ll, ll> &interval)
	{
		this->interval = interval;
	}

	const string &getIp() const
	{
		return ip;
	}

	void setIp(const string &ip)
	{
		this->ip = ip;
	}

	int getPort() const
	{
		return port;
	}

	void setPort(int port)
	{
		this->port = port;
	}
};
#endif
