// Not used file finally .

#ifndef helper_h
#define helper_h
#include "chord.h"
class Helper
{

  public:
	void setServerDetails(struct sockaddr_in &server, std::string ip, int port);
	void setTimer(struct timeval &timer);
	ll getHash(std::string key);
	void assignIpAndPortToNode();
	std::vector<std::string> splitCommand(std::string);

	pair<string, int> getIpAndPort(string address);
};

#endif
