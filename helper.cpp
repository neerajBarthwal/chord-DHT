// Not used file finally .

#include "helper.h"
#include "node.h"
using namespace std;

typedef long long ll;

void Helper::setServerDetails(struct sockaddr_in &server, string ip, int port)
{
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(ip.c_str());
    server.sin_port = htons(port);
}
ll Helper::getHash(string key)
{
    unsigned char obuf[41];
    char finalHash[41];
    string keyHash = "";
    unsigned char unsigned_key[key.length() + 1];
    int i;
    for (i = 0; i < key.length(); i++)
    {
        unsigned_key[i] = key[i];
    }
    unsigned_key[i] = '\0';
    SHA1(unsigned_key, sizeof(unsigned_key), obuf);
    for (i = 0; i < SHA_DIGEST_LENGTH / 4; i++)
    {
        sprintf(finalHash, "%d", obuf[i]);
        keyHash += finalHash;
    }

    ll modulo = pow(2, M);
    cout << "Key hash: " << keyHash << endl;
    ll hash = fmod(stoll(keyHash), modulo);
    cout << "sha hash generated is " << hash << "\n";
    return hash;
}

void Helper::assignIpAndPortToNode()
{

    /* generating a port number between 1024 and 65535 */
    srand(time(0));
    int port_number = rand() % 65536;
    if (port_number < 1024)
        port_number += 1024;
    sockaddr_in sockaddr;
    socklen_t len = sizeof(struct sockaddr);

    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_port = htons(port_number);
    sockaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    /*     node.setIp("127.0.0.1");
    node.setPort(port_number); */

    if (bind(sock, (struct sockaddr *)&sockaddr, len) < 0)
    {
        perror("error");
        exit(-1);
    }
}

vector<string> splitCommand(string command)
{
    vector<string> arguments;
    int pos = 0;
    do
    {
        pos = command.find(' ');
        string arg = command.substr(0, pos);
        arguments.push_back(arg);
        command = command.substr(pos + 1);
    } while (pos != -1);
    return arguments;
}

pair<string, int> Helper::getIpAndPort(string address)
{
    int ind = address.find(":");
    string ip = address.substr(0, ind);
    int port = stoi(address.substr(ind + 1));
    pair<string, int> ip_port;
    ip_port.first = ip;
    ip_port.second = port;
    return ip_port;
}

int getConnectionSocket(string ip, int port)
{
}
