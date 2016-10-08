#ifndef SOCKET_H_H
#define SOCKET_H_H
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <unistd.h>
#include <arpa/inet.h>
#include <json/json.h>
#include <sys/socket.h>
#include <errno.h>
#include <boost/concept_check.hpp>
#include "listener.h"

using namespace std;
#ifndef MAX_SIZE
#define MAX_SIZE 1024
#endif

class Socket
{
public:
	   Socket();
	   virtual ~Socket();
	   virtual json_object  * Rev()=0;
	   virtual void SendTo(json_object * jobj)=0;
protected:
	  struct sockaddr_in serv_addr;
	  int listenfd;
	  int connfd;
	  char sendbuff[MAX_SIZE];
	  char revbuff[MAX_SIZE];
};

class Client:public Socket
{
public:
	    Client(string addr, int port);
	    virtual ~Client();
	    json_object * Rev();
	    void SendTo(json_object * jobj);
};

class Server:public Socket
{
public:
	    Server(uint32_t addr, int port);
	    virtual ~Server();
	    json_object  * Rev();
	    void Con();
	    void ShutCon();
	    void SendTo(json_object * jobj);
};

#endif
