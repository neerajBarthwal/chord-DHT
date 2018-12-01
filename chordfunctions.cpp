#include "chord.h"
#include "chordfunctions.h"
#include "util.h"

typedef long long int lli;

using namespace std;

Utility util = Utility();

/*
    Put the key to the required Node
*/
void put(string key, string value, NodeDetails &nodeDetails)
{
    if (key == "" || value == "")
    {
        cout << "Key or Value field is empty\n";
        return;
    }

    else
    {
        string file_path=string(my_files_path)+"/"+key;

    //string file_path="sultan.mp3";

    // ofstream file (file_path);
    struct stat st;
      if(stat(file_path.c_str(), &st) != 0) {
         cout<<"file does not exist in "<<string(my_files_path)<<"\n";
         return;
      }
      else if(S_ISDIR(st.st_mode)){
        cout<<"This is a directory cannot share \n";
        return;
      }

        lli keyHash = util.getHash(key);
        cout << "Key is " << key << " and hash is " << keyHash << endl;

        pair<pair<string, int>, lli> node = nodeDetails.findSuccessor(keyHash);

        util.sendKeyToNode(node, keyHash, value);

        cout << "key Entered Successfully\n";
    }
}

/* 
    Get key for the required Node
*/
void get(string key, NodeDetails nodeDetails)
{

    if (key == "")
    {
        cout << "Key field is empty\n";
        return;
    }
    else
    {

        lli keyHash = util.getHash(key);

        pair<pair<string, int>, lli> node = nodeDetails.findSuccessor(keyHash);

        string val = util.getKeyFromNode(node, to_string(keyHash));

        if (val == "")
            cout << "Key Not found\n";

        else
            cout << "Found key " << key << " : value " << val << endl;
    }
}


// find the node which has required file and download if it has it.
void download(NodeDetails &nodeDetails,string key)
{

    if (key == "")
    {
        cout << "Key field is empty\n";
        return;
    }
    else
    {

        lli keyHash = util.getHash(key);

        pair<pair<string, int>, lli> node = nodeDetails.findSuccessor(keyHash);

        string val = util.getKeyFromNode(node, to_string(keyHash));

        if (val == ""){  // file not found
            cout << "No one in the ring has this file\n";
        }

        else{ //file found
            string uploader_address=val;
            Utility util;
            pair<string,int> ipAndPort = util.getIpAndPort(uploader_address);

            string ip = ipAndPort.first;
            int port = ipAndPort.second;
            thread th(requestDownload,nodeDetails,ip,port,key);
            th.detach();
        }
    }
}

void requestDownload(NodeDetails nodeDetails,string ip,int port,string fileName){
    string file_path=string(my_files_path)+"/"+fileName;
    if (util.isNodeAlive(ip, port) == false)
    {
        cout << "Sorry but this node is currently down.Try after some time.\n";
        return;
    }

    int sock = nodeDetails.sp.connect_socket(ip, to_string(port));
    string message="download "+fileName;
    char message_char[41];
    strcpy(message_char, message.c_str());

/* 
   Node send id to successor.
*/
    if (send(sock, message_char, strlen(message_char), 0) == -1)
    {
        //cout << "In JOIN1\n";
    }

    char response[40];
    int len;
    if ((len = recv(sock, response, 1024, 0)) == -1)
    {
        //cout << "In JOIN2\n";
    }
    response[len] = '\0';
//    cout << "reveived from server" << ipAndPort << "\n";
    string reply=response;
    if(reply == "yes"){ // file is present download
        message="ok";
        strcpy(message_char, message.c_str());

    /* 
       Node send id to successor.
    */
        if (send(sock, message_char, strlen(message_char), 0) == -1)
        {
            //cout << "In JOIN1\n";
        }
        size_t datasize;
        FILE* fd = fopen(file_path.c_str(), "wb");
        char buffer[256];
        int BUFFER_SIZE=256;
        while (true){
            datasize = recv(sock, buffer, BUFFER_SIZE, 0);
            //cout<<datasize<<"\n";
            if(datasize == 0){
                break;
            }
            fwrite(&buffer, 1, datasize, fd);
            //print_on_screen(to_string(datasize));
        }
        fclose(fd);
        cout<<"file downloaded\n";
    }
    else{
        cout<<"File is not present. It may have been deleted\n";
    }
    close(sock);
}

/* 
   Creates A New Ring
*/
void create(NodeDetails &nodeDetails)
{

    string ip = nodeDetails.sp.getIpAddress();
    int port = nodeDetails.sp.getPortNumber();
    string key = ip + ":" + (to_string(port));

    lli hash = util.getHash(key);
    cout << "#----------------------------------------------------------------------#\n";
    cout << "Ring Created\n";
    cout << "identifier is: " << hash << "\n";
    cout << "#------------------------------------------------------------------------------#\n\n";

    nodeDetails.setId(hash);
    nodeDetails.setSuccessor(ip, port, hash);
    nodeDetails.setSuccessorList(ip, port, hash);
    nodeDetails.setPredecessor("", -1, -1);
    nodeDetails.setFingerTable(ip, port, hash);
    nodeDetails.setStatus();

/* 
    Threads
    Listen thread to act as server
    Stabilize algo.
*/
    thread second(listenTo, ref(nodeDetails));
    second.detach();

    thread fifth(doStabilize, ref(nodeDetails));
    fifth.detach();
}

/* 
    Node joines in a DHT Ring
*/
void join(NodeDetails &nodeDetails, string ip, string port)
{

    if (util.isNodeAlive(ip, atoi(port.c_str())) == false)
    {
        cout << "Sorry but no node is active on this IP or Port.Try with new details.\n";
        return;
    }

    int sock = nodeDetails.sp.connect_socket(ip, port);

    string currIp = nodeDetails.sp.getIpAddress();
    string currPort = to_string(nodeDetails.sp.getPortNumber());

    lli nodeId = util.getHash(currIp + ":" + currPort);

    char charNodeId[41];
    strcpy(charNodeId, to_string(nodeId).c_str());

/* 
   Node send id to successor.
*/
    if (send(sock, charNodeId, strlen(charNodeId), 0) == -1)
    {
        cout << "In JOIN1\n";
    }

    char ipAndPort[40];
    int len;
    if ((len = recv(sock, ipAndPort, 1024, 0)) == -1)
    {
        cout << "In JOIN2\n";
    }
    ipAndPort[len] = '\0';
//    cout << "reveived from server" << ipAndPort << "\n";

    close(sock);
    cout << "#--------------------------------------------------------------------------------#\n";
    cout << "Successfully joined the ring.\n";
    cout << "#--------------------------------------------------------------------------------#\n\n";
    string key = ipAndPort;
    lli hash = util.getHash(key);
    cout << "Identifier is: " << nodeId << "\n";
    pair<string, int> ipAndPortPair = util.getIpAndPort(key);

    nodeDetails.setId(nodeId);
    nodeDetails.setSuccessor(ipAndPortPair.first, ipAndPortPair.second, hash);
    nodeDetails.setSuccessorList(ipAndPortPair.first, ipAndPortPair.second, hash);
    nodeDetails.setPredecessor("", -1, -1);
    nodeDetails.setFingerTable(ipAndPortPair.first, ipAndPortPair.second, hash);
    nodeDetails.setStatus();

    util.getKeysFromSuccessor(nodeDetails, ipAndPortPair.first, ipAndPortPair.second);
    cout << "Got Necessary Keys\n";


/* 
    Threads
    Listen thread to act as server
    Stabilize algo.
*/

    thread fourth(listenTo, ref(nodeDetails));
    fourth.detach();

    thread third(doStabilize, ref(nodeDetails));
    third.detach();
}

/* 
    Prints successor,predecessor,successor list and finger table of Node
*/


void printdetails(NodeDetails nodeDetails)
{
    string ip = nodeDetails.sp.getIpAddress();
    lli id = nodeDetails.getId();
    int port = nodeDetails.sp.getPortNumber();
    vector<pair<pair<string, int>, lli>> fingerTable = nodeDetails.getFingerTable();
    cout << "Self " << ip << " " << port << " " << id << endl;
    pair<pair<string, int>, lli> succ = nodeDetails.getSuccessor();
    pair<pair<string, int>, lli> pre = nodeDetails.getPredecessor();
    vector<pair<pair<string, int>, lli>> succList = nodeDetails.getSuccessorList();
    cout << "Successor " << succ.first.first << " " << succ.first.second << " " << succ.second << endl;
    cout << "Predecessor " << pre.first.first << " " << pre.first.second << " " << pre.second << endl;
    for (int i = 1; i <= M; i++)
    {
        ip = fingerTable[i].first.first;
        port = fingerTable[i].first.second;
        id = fingerTable[i].second;
        cout << "Finger[" << i << "] " << id << " " << ip << " " << port << endl;
    }
    for (int i = 1; i <= R; i++)
    {
        ip = succList[i].first.first;
        port = succList[i].first.second;
        id = succList[i].second;
        cout << "Successor[" << i << "] " << id << " " << ip << " " << port << endl;
    }
}

/* 
    Node leaves the Ring. 
*/

void leave(NodeDetails &nodeDetails)
{
    pair<pair<string, int>, lli> succ = nodeDetails.getSuccessor();
    lli id = nodeDetails.getId();

    if (id == succ.second)
        return;

    vector<pair<lli, string>> keysAndValuesVector = nodeDetails.getAllKeysForSuccessor();

    if (keysAndValuesVector.size() == 0)
        return;

    string keysAndValues = "";

    for (int i = 0; i < keysAndValuesVector.size(); i++)
    {
        keysAndValues += to_string(keysAndValuesVector[i].first) + ":" + keysAndValuesVector[i].second;
        keysAndValues += ";";
    }

    keysAndValues += "storeKeys";

    int sock = nodeDetails.sp.connect_socket(succ.first.first, to_string(succ.first.second));
    char keysAndValuesChar[2000];
    strcpy(keysAndValuesChar, keysAndValues.c_str());

    send(sock, keysAndValuesChar, strlen(keysAndValuesChar), 0);
    //cout << "Ritik\n";
    close(sock);
}

/* 
    Does Task based on message received.
*/

void upload(string message,int sock,struct sockaddr_in client){
    socklen_t l = sizeof(client);
    vector<string> processed_message=util.split_string(message);
    string file_path=string(my_files_path)+"/"+processed_message[1];

    //string file_path="sultan.mp3";

    // ofstream file (file_path);
    string reply;
    struct stat st;
  if(stat(file_path.c_str(), &st) != 0) {
    reply="no";
  }
  else if(S_ISDIR(st.st_mode)){
    reply="no";
  }
  else{
    reply="yes";
  }
    
    // if (file.is_open()){ // file exists
    //     reply="yes";
    //     file.close();
    // }
    // else{
    //     reply="no";
    // }


    char response[1000];
    strcpy(response,reply.c_str());

    send(sock,response,strlen(response),0);

    message="";
    if(reply=="no"){
        cout<<processed_message[1]<<" file has been removed from my_files folder. \n";
        close(sock);
        return;
    }
    char response_from_requester[1000];
    int len=recv(sock,response_from_requester,strlen(response_from_requester),0);
    response_from_requester[len]='\0';

     char buffer[256];
     int BUFFER_SIZE=256;
    cout<<file_path<<"\n";


  //   FILE* fp = fopen(file_path.c_str(), "rb"); 
  // long int res;
  //   // checking if the file exist or not 
  //   if (fp == NULL) { 
  //       printf("File Not Found!\n"); 
  //       res=0; 
  //   } 
  
  //   fseek(fp, 0L, SEEK_END); 
  
  //   // calculating the size of the file 
  //   res = ftell(fp); 
  
  //   // closing the file 
  //   fclose(fp); 
  
  //   //cout<<"file size is 1: "<<res << endl; 


    FILE *fd = fopen(file_path.c_str(), "rb");
    size_t rret, wret;
    int bytes_read;

    while (!feof(fd)) {
        //cout<<"here\n";
        //perror("error: ");
        if ((bytes_read = fread(&buffer, 1,BUFFER_SIZE, fd)) > 0){
            //cout<<bytes_read<<"\n";
            send(sock, buffer, bytes_read, 0);
            //print_on_screen("sent "+to_string(bytes_read));
        }
        else
            break;
    }
    fclose(fd);
    close(sock);
}
void doTask(NodeDetails &nodeDetails, int newSock, struct sockaddr_in client, string nodeIdString)
{
    bool do_it=true;
    if(nodeIdString.find("download") != -1){
        do_it=false;
        /*string file_path="sultan.mp3";
        FILE* fp = fopen(file_path.c_str(), "rb"); 
          long int res;
            // checking if the file exist or not 
            if (fp == NULL) { 
                printf("File Not Found!\n"); 
                res=0; 
            } 
          
            fseek(fp, 0L, SEEK_END); 
          
            // calculating the size of the file 
            res = ftell(fp); 
          
            // closing the file 
            fclose(fp); 
          
            cout<<"file size is"<<res;*/



        thread th(upload,nodeIdString,newSock,client);
        th.detach();
    }

    else if (nodeIdString.find("storeKeys") != -1)
    {
        util.storeAllKeys(nodeDetails, nodeIdString);
    }

    else if (util.isKeyValue(nodeIdString))
    {
        pair<lli, string> keyAndVal = util.getKeyAndVal(nodeIdString);
        nodeDetails.storeKey(keyAndVal.first, keyAndVal.second);
    }

    else if (nodeIdString.find("alive") != -1)
    {
        util.sendAcknowledgement(newSock, client);
    }

    else if (nodeIdString.find("sendSuccList") != -1)
    {
        util.sendSuccessorList(nodeDetails, newSock, client);
    }
    else if (nodeIdString.find("getKeys") != -1)
    {

        util.sendNeccessaryKeys(nodeDetails, newSock, client, nodeIdString);
//      cout<<"amit tiw\n";
    }

    else if (nodeIdString.find("k") != -1)
    {
        util.sendValToNode(nodeDetails, newSock, client, nodeIdString);
    }

    /* contacting node wants the predecessor of this node */
    else if (nodeIdString.find("p") != -1)
    {
        util.sendPredecessor(nodeDetails, newSock, client);

        /* p1 in msg means that notify the current node about this contacting node */
        if (nodeIdString.find("p1") != -1)
        {
//            cout << "Working here 1\n";
            callNotify(nodeDetails, nodeIdString);
        }
    }

    /* contacting node wants successor Id of this node for util in finger table */
    else if (nodeIdString.find("finger") != -1)
    {
        util.sendSuccessorId(nodeDetails, newSock, client);
    }

    /* contacting node wants current node to find successor for it */
    else
    {
        util.sendSuccessor(nodeDetails, nodeIdString, newSock, client);
    }
    if(do_it){
        close(newSock);
    }
    else{
        //cout<<"fuck\n";
    }
//  cout<<"closed after sending neccessary keys\n";
}

/* 
    Listen to contacting Node.
*/
void listenTo(NodeDetails &nodeDetails)
{
    /*     struct sockaddr_in client;
    socklen_t l = sizeof(client); */
    int newSocket;
    struct sockaddr_in new_addr;
    socklen_t addr_size = sizeof(new_addr);

    int sock = nodeDetails.sp.getSocketFd();
    
/*
    cout<<"listen thread launched by "<<nodeDetails.sp.getIpAddress()<<" "<<nodeDetails.sp.getPortNumber()<<"on socjet id"<<sock;
*/

    while (1)
    {

        if (listen(sock, 10) == 0)
        {
        }
        /* 		    printf("Listening in listentoFN\n"); */
        else
        /* 		    printf("Error in listening ListenFN\n"); */ {
        }
        newSocket = accept(sock, (struct sockaddr *)&new_addr, &addr_size);

        if (newSocket < 0)
        {
            /* 			printf("Connection not accepted \n"); */
            exit(1);
        }
        else
        /* 			printf("connection accepted in ListenFN. \n"); */ {
        }

        char charNodeId[40];
        int len = recv(newSocket, charNodeId, 1024, 0);
        charNodeId[len] = '\0';
        string nodeIdString = charNodeId;

        /* launch a thread that will perform diff tasks acc to received msg */
        thread f(doTask, ref(nodeDetails), newSocket, new_addr, nodeIdString);
        f.detach();
    }
}

/* 
    Does Stabilize
*/
void doStabilize(NodeDetails &nodeDetails)
{

    /* do stabilize tasks */
    while (1)
    {
        nodeDetails.checkPredecessor();
        nodeDetails.checkSuccessor();
        nodeDetails.stabilize();
        nodeDetails.updateSuccessorList();
        nodeDetails.fixFingers();
        this_thread::sleep_for(chrono::milliseconds(200));
    }
}

/* 
    Call Notify for predecessor successor.
*/
void callNotify(NodeDetails &nodeDetails, string ipAndPort)
{

    ipAndPort.pop_back();
    ipAndPort.pop_back();

    pair<string, int> ipAndPortPair = util.getIpAndPort(ipAndPort);
    string ip = ipAndPortPair.first;
    int port = ipAndPortPair.second;
    lli hash = util.getHash(ipAndPort);

    pair<pair<string, int>, lli> node;
    node.first.first = ip;
    node.first.second = port;
    node.second = hash;

    nodeDetails.notify(node);
}

/* 
    Menu Driven Interface using help
 */
void showMenuDriven()
{  
    cout << "################################################################################\n";
    cout<<"Files to be shared and downloaded are kept in my_files folder\n\n";
    cout << "1) create : Creates a DHT ring.\n";
    cout << "2) join <ip>:<port> : Join the existing ring.\n";
    cout << "3) printdetails : Print predecessor, successor, FingerTable and Successor list\n";
    cout << "4) print : Print all keys and IP:PORT present in the corresponding node\n";
    cout << "5) put <key>: put key into the chord network. Key is the filename to be shared.\n";
    cout << "6) get <key> : get IP:PORT of provided key\n";
    cout << "7) download <key> : download the file with name \"key\" into my_files folder. \n";
    cout << "################################################################################\n\n";
}
