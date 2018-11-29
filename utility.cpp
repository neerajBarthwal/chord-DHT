#include "chord.h"
#include "util.h"
#include "nodedetails.h"
#include "connection.h"

mutex mt;

/*utility function to split command by spaces*/
vector<string> Utility::split_string(string command){  // improve for multiple spaces
      char space_delimiter=' ';
      vector<string> split_command;
      stringstream ss(command);
      string tokens;
     
      while(getline(ss, tokens, space_delimiter)) {
        split_command.push_back(tokens);
      }
      return split_command;
}

/* generating SHA1*/
lli Utility::getHash(string key){
    unsigned char outputbuf[41];
    char finalHash[41];
    string keyHash = "";
    int i;
    lli mod = pow(2,M);
    unsigned char unsigned_key[key.length()+1];
    for(i=0;i<key.length();i++){
        unsigned_key[i] = key[i];
    }
    unsigned_key[i] = '\0';


    SHA1(unsigned_key,sizeof(unsigned_key),outputbuf);
    for (i = 0; i < SHA224_DIGEST_LENGTH/8; i++) {
        sprintf(finalHash,"%d",outputbuf[i]);
        keyHash += finalHash;
    }

    lli hashToReturn = stoll(keyHash) % mod;
    return hashToReturn;
}

/* split string on basis of :*/
pair<string,int> Utility::getIpAndPort(string key){

    int pos = key.find(':');
    string ip = key.substr(0,pos);
    string port = key.substr(pos+1);

    pair<string,int> ipAndPortPair;
    ipAndPortPair.first = ip;
    ipAndPortPair.second = atoi(port.c_str());

    return ipAndPortPair;
}

/* split string for keyvalue pair when get and put function used*/
pair<lli,string> Utility::getKeyAndVal(string keyAndVal){

    int pos = keyAndVal.find(':');
    string key = keyAndVal.substr(0,pos);
    string val = keyAndVal.substr(pos+1);

    pair<lli,string> keyAndValPair;
    keyAndValPair.first = stoll(key);
    keyAndValPair.second = val;

    return keyAndValPair;
}

/* will decide if id is in form of key:value or not */
bool Utility::isKeyValue(string id){

    int pos = id.find(":");

    if(pos == -1)
        return false;

    for(int i=0;i<pos;i++){
        if( !(id[i] >= 48 && id[i] <= 57) )
            return false;
    }

    return true;
}

/* Used in get function to obtain value for a key*/
string Utility::getKeyFromNode(pair< pair<string,int> , lli > node,string keyHash){
    string ip = node.first.first;
    int port = node.first.second;

    SocketAndPort sp;
    int sock = sp.connect_socket(ip,to_string(port));

    keyHash += "k";

    char keyHashChar[40];
    strcpy(keyHashChar,keyHash.c_str());

    send(sock,keyHashChar,strlen(keyHashChar),0);
// cout<<"sent key hash to server";
    char valChar[100];
    int len = recv(sock,valChar,1024,0);

    valChar[len] = '\0';

    string val = valChar;
//  cout<<"checking value"<<val;
    close(sock);

    return val;
}

void Utility::setServerDetails(struct sockaddr_in &server,string ip,int port){
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(ip.c_str());
    server.sin_port = htons(port);
}

/*Used in put funtion */
void Utility::sendKeyToNode(pair< pair<string,int> , lli > node,lli keyHash,string value){
    string ip = node.first.first;
    int port = node.first.second;

    SocketAndPort sp;
    int sock = sp.connect_socket(ip,to_string(port));

    string keyAndVal = combineIpAndPort(to_string(keyHash),value);

    char keyAndValChar[100];
    strcpy(keyAndValChar,keyAndVal.c_str());

    send(sock,keyAndValChar,strlen(keyAndValChar),0);
// cout<<"sent her";
    close(sock);
}


/* used in join by new node*/
void Utility::getKeysFromSuccessor(NodeDetails &nodeDetails,string ip,int port){

    SocketAndPort sp;
   // cout<<"requesting for keys from successor now";
    int sock = sp.connect_socket(ip,to_string(port));

    string id = to_string(nodeDetails.getId());


    string msg = "getKeys:" + id;

    char msgChar[40];
    strcpy(msgChar,msg.c_str());

    send(sock,msgChar,strlen(msgChar),0);
    cout<<"message sent to server"<<msgChar<<"on socket"<<sock<<"\n";

    char keysAndValuesChar[2000];
    int len = recv(sock,keysAndValuesChar,2000,0);

    keysAndValuesChar[len] = '\0';

    close(sock);

    string keysAndValues = keysAndValuesChar;

    vector< pair<lli,string> > keysAndValuesVector = seperateKeysAndValues(keysAndValues);

/*     cout << "Size is " << keysAndValuesVector.size() << "\n";
    exit(1); */
    for(int i=0;i<keysAndValuesVector.size();i++){
        nodeDetails.storeKey(keysAndValuesVector[i].first , keysAndValuesVector[i].second);
    }

}

/* used to store all keys from predecessor when it leaves*/
vector< pair<lli,string> > Utility::seperateKeysAndValues(string keysAndValues){
    int size = keysAndValues.size();
    int i = 0;
    vector< pair<lli,string> > res;

    while(i < size){
        string key = "";
        while(i < size && keysAndValues[i] != ':'){
            key += keysAndValues[i];
            i++;
        }
        i++;

        string val = "";
        while(i < size && keysAndValues[i] != ';'){
            val += keysAndValues[i];
            i++;
        }
        i++;

        res.push_back(make_pair(stoll(key),val));
    }

    return res;
}


vector< pair<string,int> > Utility::seperateSuccessorList(string succList){
    int size = succList.size();
    int i = 0;
    vector< pair<string,int> > res;

    while(i < size){
        string ip = "";
        while(i < size && succList[i] != ':'){
            ip += succList[i];
            i++;
        }
        i++;

        string port = "";
        while(i < size && succList[i] != ';'){
            port += succList[i];
            i++;
        }
        i++;

        res.push_back(make_pair(ip,stoi(port)));
    }

    return res;
}

/* combine ip and port with : */
string Utility::combineIpAndPort(string ip,string port){
    string ipAndPort = "";
    int i=0;

    for(i=0;i<ip.size();i++){
        ipAndPort += ip[i];
    }

    ipAndPort += ':';

    for(i=0;i<port.size();i++){
        ipAndPort += port[i];
    }

    return ipAndPort;
}


void Utility::storeAllKeys(NodeDetails &nodeDetails,string keysAndValues){
    int pos = keysAndValues.find("storeKeys");

    vector< pair<lli,string> > res = seperateKeysAndValues(keysAndValues.substr(0,pos));

    for(int i=0;i<res.size();i++){
        nodeDetails.storeKey(res[i].first,res[i].second);
    }
}

/* send all keys to the newly joined node which belong to it now */
void Utility::sendNeccessaryKeys(NodeDetails &nodeDetails,int newSock,struct sockaddr_in client,string nodeIdString){
    socklen_t l = sizeof(client);

    int pos = nodeIdString.find(':');
/*     cout << "Amit\n";
    exit(1); */
    lli nodeId = stoll(nodeIdString.substr(pos+1));

    vector< pair<lli , string> > keysAndValuesVector = nodeDetails.getKeysForPredecessor(nodeId);

    string keysAndValues = "";
/*     cout << "Amit\n";
    exit(1); */
    /* will arrange all keys and val in form of key1:val1;key2:val2; */
    for(int i=0;i<keysAndValuesVector.size();i++){
        keysAndValues += to_string(keysAndValuesVector[i].first) + ":" + keysAndValuesVector[i].second;
        keysAndValues += ";";
//        cout<<"inside loop"<<keysAndValues<<"\n";
    }
    cout<<keysAndValues<<"\n";
    char keysAndValuesChar[2000];
    strcpy(keysAndValuesChar,keysAndValues.c_str()); 

    if(send(newSock,keysAndValuesChar,strlen(keysAndValuesChar),0)<0)
        cout << "error in sending" << "\n";
    else
//        cout<<"sent"<<keysAndValuesChar<<"\n"; 
    {}

    
}

/* after get command called from client */
void Utility::sendValToNode(NodeDetails nodeDetails,int newSock,struct sockaddr_in client,string nodeIdString){
    nodeIdString.pop_back();
    lli key = stoll(nodeIdString);
    string val = nodeDetails.getValue(key);

    socklen_t l = sizeof(client);

    char valChar[100];
    strcpy(valChar,val.c_str());

    send(newSock,valChar,strlen(valChar),0);
}

/* send successor id of current node to the contacting node */
void Utility::sendSuccessorId(NodeDetails nodeDetails,int newSock,struct sockaddr_in client){

    pair< pair<string,int> , lli > succ = nodeDetails.getSuccessor();
    string succId = to_string(succ.second);
    char succIdChar[40];

    socklen_t l = sizeof(client);

    strcpy(succIdChar,succId.c_str());

    send(newSock,succIdChar,strlen(succIdChar),0);
// cout<<"sent";
}

/* find successor of contacting node and send it's ip:port to it */
void Utility::sendSuccessor(NodeDetails nodeDetails,string nodeIdString,int newSock,struct sockaddr_in client){
    
    lli nodeId = stoll(nodeIdString);

    socklen_t l = sizeof(client);
    
    /* find successor of the joining node */
    pair< pair<string,int> , lli > succNode;
    succNode = nodeDetails.findSuccessor(nodeId);

    /* get Ip and port of successor as ip:port in char array to send */
    char ipAndPort[40];
    string succIp = succNode.first.first;
    string succPort = to_string(succNode.first.second);
    strcpy(ipAndPort,combineIpAndPort(succIp,succPort).c_str());

    /* send ip and port info to the respective node */
    send(newSock, ipAndPort, strlen(ipAndPort), 0);

}

/* send ip:port of predecessor of current node to contacting node */
void Utility::sendPredecessor(NodeDetails nodeDetails,int newSock,struct sockaddr_in client){
    
    pair< pair<string,int> , lli > predecessor = nodeDetails.getPredecessor();
    
    string ip = predecessor.first.first;
    string port = to_string(predecessor.first.second);

    socklen_t l = sizeof(client);

    /* if predecessor is nil */
    if(ip == ""){
        send(newSock, "", 0, 0);       
    }

    else{
        string ipAndPort = combineIpAndPort(ip,port);

        char ipAndPortChar[40];
        strcpy(ipAndPortChar,ipAndPort.c_str());

        send(newSock, ipAndPortChar, strlen(ipAndPortChar), 0);

    }
}


lli Utility::getSuccessorId(string ip,int port){
    
    SocketAndPort sp;
    int sock = sp.connect_socket(ip,to_string(port));

/*     setsockopt(sock,SOL_SOCKET,SO_RCVTIMEO,(char*)&timer,sizeof(struct timeval)); */

    char msg[] = "finger";

    if (send(sock, msg, strlen(msg) , 0) == -1){
        cout<<"in getsuccesorid"<<endl;
       // perror("error");
        //exit(-1);
    }

    char succIdChar[40];

    int length = recv(sock,succIdChar,1024,0);

    close(sock);

    if(length < 0){
        return -1;
    }

    succIdChar[length] = '\0';

    return atoll(succIdChar);

}

void Utility::setTimer(struct timeval &timer){
    timer.tv_sec = 0;
    timer.tv_usec = 100000;
}

/* get predecessor node (ip:port) of the node having ip and port */
pair< pair<string,int> , lli > Utility::getPredecessorNode(string ip,int port,string ipClient,int portClient,bool forStabilize){

    /* set timer for socket */
    struct timeval timer;
    setTimer(timer);

    SocketAndPort sp;
    int sock = sp.connect_socket(ip,to_string(port));

    setsockopt(sock,SOL_SOCKET,SO_RCVTIMEO,(char*)&timer,sizeof(struct timeval));

    string msg = "";

    /* p2 means that just send predecessor of node ip:port , do not call notify */
    /* p1 means that this is for stabilize so notify node as well */

    if(forStabilize == true){
        msg = combineIpAndPort(ipClient,to_string(portClient));
        msg += "p1";
    }

    else
        msg = "p2";


    char ipAndPortChar[40];
    strcpy(ipAndPortChar,msg.c_str());

    if (send(sock, ipAndPortChar, strlen(ipAndPortChar), 0) < 0){
        perror("error in predecessor node\n");
        cout<<"123\n";
        exit(-1);
    }

  //cout << "Ritik here\n";

    int len = recv(sock, ipAndPortChar, 1024, 0);
    close(sock);

    if(len < 0){
        pair< pair<string,int> , lli > node;
        node.first.first = "";
        node.first.second = -1;
        node.second = -1;
        return node;
    }

    ipAndPortChar[len] = '\0';

    

    string ipAndPort = ipAndPortChar;
    lli hash;
    pair<string,int> ipAndPortPair;

    pair< pair<string,int> , lli > node;

    if(ipAndPort == ""){
        node.first.first = "";
        node.first.second = -1;
        node.second = -1;
    }

    else{
        ipAndPortPair = getIpAndPort(ipAndPort);
        node.first.first = ipAndPortPair.first;
        node.first.second = ipAndPortPair.second;
        node.second = getHash(ipAndPort);
    }
    //cout << "Ritik not here\n";
    return node;
}

/* get successor list from node having ip and port */
vector< pair<string,int> > Utility::getSuccessorListFromNode(string ip,int port){


    struct timeval timer;
    setTimer(timer);
    
    SocketAndPort sp;
    int sock = sp.connect_socket(ip,to_string(port));

    setsockopt(sock,SOL_SOCKET,SO_RCVTIMEO,(char*)&timer,sizeof(struct timeval));

    char msg[] = "sendSuccList";

    send(sock,msg,strlen(msg),0);

    char succListChar[1000];
    int len = recv(sock,succListChar,1000,0);

    close(sock);


    if(len < 0){
        vector< pair<string,int> > list;
        return list;
    }

    succListChar[len] = '\0';

    string succList = succListChar;

    vector< pair<string,int> > list = seperateSuccessorList(succList);

    return list;

}

/* send node's successor list to the contacting node */
void Utility::sendSuccessorList(NodeDetails &nodeDetails,int sock,struct sockaddr_in client){
    socklen_t l = sizeof(client);

    vector< pair< pair<string,int> , lli > > list = nodeDetails.getSuccessorList();

    string successorList = splitSuccessorList(list);

    char successorListChar[1000];
    strcpy(successorListChar,successorList.c_str());

    send(sock,successorListChar,strlen(successorListChar),0);
//    close(sock);

}

string Utility::splitSuccessorList(vector< pair< pair<string,int> , lli > > list){
    string res = "";

    for(int i=1;i<=R;i++){

        res = res + list[i].first.first + ":" + to_string(list[i].first.second) + ";";
    }

    return res;
}


void Utility::sendAcknowledgement(int newSock,struct sockaddr_in client){
    socklen_t l = sizeof(client);

    send(newSock,"1",1,0);
}

/* check if node alive or not */
bool Utility::isNodeAlive(string ip,int port){


    /* set timer for socket */
    struct timeval timer;
    setTimer(timer);

    SocketAndPort sp;
    int sock = sp.connect_socket(ip,to_string(port));
    if(sock < 0){
 //       cout<<"node is dead\n";
        return false;
    }
    /* set timer on this socket */
    setsockopt(sock,SOL_SOCKET,SO_RCVTIMEO,(char*)&timer,sizeof(struct timeval));

    char msg[] = "alive";
    send(sock,msg,strlen(msg),0);

    char response[5];
    int len = recv(sock,response,2,0);

    close(sock);
    /* node is still active */
    if(len >= 0){
        return true;
    }
    else
        return false;
}

