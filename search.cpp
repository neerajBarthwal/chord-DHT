// Not used file finally .

#include "chord.h"
#include "node.h"
#include "finger.h"
#include "helper.h"

typedef long long ll;

pair<ll,pair<string,int>> find_successor(ll identifier){ //return pair of address and id of successor
	//pair<pair<string,int>,ll> predecessor_details=find_predecessor(id);
	//pair<pair<string,int>,ll> successor_details=get_successor_of(predecessor_details);
	pair <ll, pair<string,int>> my_details;
	my_details.first = node.identifier;
	my_details.second.first = node.getIp();
	my_details.second.second = node.getPort();

	//my succesor is the succesor of given identifier
	if(identifier > node.identifier && identifier <= node.successor.first){
		return node.successor;
	}	

	// i am the succesor of given identifier
	else if(node.identifier == node.successor.first || identifier == node.identifier){
		return my_details;
	}

	//only two nodes exist in ring
	else if(node.successor.first == node.predecessor.first){
		if(node.successor.first >= node.identifier){
			if(identifier > node.successor.first || identifier < node.identifier)
				return my_details;
		}
		else{
			if((identifier > node.identifier && identifier > node.successor.first) || (identifier < node.identifier && identifier < node.successor.first))
				return node.successor;
			else
				return my_details;
		}
	}

	else{
		pair < ll, pair<string,int>> preceding_node = closest_preceding_node(identifier);
		if(preceding_node.first == node.identifier){  
			return node.successor;
		}
		else{
			Helper help;
			//int sockT=get_connection(preciding_node.second.first,preciding_node.second.second);
			/* connect to node which will now find the successor */
			

			/* if this node couldn't find closest preciding node for given node id then now ask it's successor to do so */
			if(preceding_node.first == -1){
				preceding_node = node.successor;
			}

			string ip=preceding_node.second.first;
			int port=preceding_node.second.second;

			struct sockaddr_in server;
		    socklen_t addr_len = sizeof(server);    
		    server.sin_family = AF_INET;
		    server.sin_addr.s_addr = inet_addr(ip.c_str());
		    server.sin_port = htons(port);   
		    int sock_id = socket(AF_INET,SOCK_DGRAM,0);
		    if(sock_id < 0){
		       perror("error");
		       exit(-1);
		    }
		    struct timeval timer;
    		help.setTimer(timer);

    		setsockopt(sock_id,SOL_SOCKET,SO_RCVTIMEO,(char*)&timer,sizeof(struct timeval));
		    //sendto(__fd, __buf, __n, __flags, __addr, __addr_len)
			

			/* send the  identifier to the other node */
			char IdentifierChar[40];
			strcpy(IdentifierChar,to_string(identifier).c_str());
			sendto(sock_id, IdentifierChar, strlen(IdentifierChar), 0, (struct sockaddr*) &server, addr_len);

			/* receive ip and port of node's successor as ip:port*/
			char ipAndPort[40];

			int l = recvfrom(sock_id, ipAndPort, 1024, 0, (struct sockaddr *) &server, &addr_len);

			close(sock_id);

			if(l < 0){
				pair < ll, pair<string,int>> successor_details;
				successor_details.first = -1;
				node.second.first = "";
				node.second.second = -1;
				return successor_details;
			}

			pair < ll, pair<string,int>> successor_details;
			ipAndPort[l] = '\0';

			/* set ip,port and hash for this node and return it */
			string key = ipAndPort;
    		ll hash = help.getHash(ipAndPort);
    		pair<string,int> ipAndPortPair = help.getIpAndPort(key);
    		successor_details.first = hash;
    		successor_details.second.first = ipAndPortPair.first;
    		successor_details.second.second = ipAndPortPair.second;

    		return successor_details;
		}
	}
}
//edited till here
pair< ll, pair<string,int> > closest_preceding_node(ll nodeId){
	Helper help;

	for(int i=M;i>=1;i--){
		if(node.finger_table[i].ip == "" || node.finger_table[i].port == -1 || node.finger_table[i].identifier == -1){
			continue;
		}

		if(node.finger_table[i].identifier > node.identifier && node.finger_table[i].identifier < nodeId){
			pair< ll, pair<string,int> > details;
			details.first=node.finger_table[i].identifier;
			details.second.first=node.finger_table[i].ip;
			details.second.second=node.finger_table[i].port;
			return details;
		}
		else{

			lli successorId = help.getSuccessorId(node.finger_table[i].first.first,node.finger_table[i].first.second);

			if(successorId == -1)
				continue;

			if(node.finger_table[i].second > successorId){
				if((nodeId <= node.finger_table[i].second && nodeId <= successorId) || (nodeId >= node.finger_table[i].second && nodeId >= successorId)){
					return node.finger_table[i];
				}
			}
			else if(node.finger_table[i].second < successorId && nodeId > node.finger_table[i].second && nodeId < successorId){
				return node.finger_table[i];
			}
			
			pair< pair<string,int> , lli > predNode = help.getPredecessorNode(node.finger_table[i].first.first,node.finger_table[i].first.second,"",-1,false);
			lli predecessorId = predNode.second;

			if(predecessorId != -1 && node.finger_table[i].second < predecessorId){
				if((nodeId <= node.finger_table[i].second && nodeId <= predecessorId) || (nodeId >= node.finger_table[i].second && nodeId >= predecessorId)){
					return predNode;
				}		
			}
			if(predecessorId != -1 && node.finger_table[i].second > predecessorId && nodeId >= predecessorId && nodeId <= node.finger_table[i].second){
				return predNode;
			}
		}
	}

	/* */
	pair< pair<string,int> , lli > node;
	node.first.first = "";
	node.first.second = -1;
	node.second = -1;
	return node;
}

/*string find_predecessor(ll id){
	Node current_node=node;
	while()
	if(id > current_node.identifier && id <=current_node.successor.first){
		string message=current_node.address+" "+to_string(current_node.identifier);
	}
	else{
		string message=closest_preceding_finger(id);
	}
	return message;
}*/






