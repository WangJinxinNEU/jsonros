#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <typeinfo>
#include <unistd.h>
#include <arpa/inet.h>
#include <json/json.h>
#include <sys/socket.h>
#include<errno.h>
#include <boost/concept_check.hpp>
#include "socket.h"
#include "listener.h"

using namespace std;

Socket::Socket()
{
    listenfd = 0;
    connfd = 0;   //related with the server
    listenfd = socket ( AF_INET, SOCK_STREAM, 0 );
    if(listenfd == -1)
    {
      cout<<"Error  socket,because "<<strerror(errno)<<endl;  
	  exit(1);  
    }
    cout<<"socket established"<<endl; 
    
}
Socket::~Socket()
{

}

Client::Client(string addr, int port)
{
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(addr.c_str());
    serv_addr.sin_port = htons(port);
       if(connect(listenfd,(struct sockaddr*) &serv_addr,sizeof(serv_addr)) == -1)
    {
      cout<<"connect error"<<endl;
      exit(1);
    }
}
Client::~Client()
{
    close(listenfd);
    close(connfd);
}
json_object  * Client::Rev()
{
      int rec;
      rec= recv ( listenfd, revbuff, MAX_SIZE,0 );
    if ( rec == -1 )
    {
      cout<<"read error"<<endl;
      exit(1);
    }
    json_object *callobj;
    callobj = json_tokener_parse ( revbuff );
//     json_object *re =  json_object_object_get ( callobj,"mapping" );
//     return json_object_get_string(re);
    return callobj;
}
void Client::SendTo(json_object * jobj)
{
   
    if ( strcpy ( sendbuff, json_object_to_json_string ( jobj ) ) == NULL )
        {
            perror ( "strcpy" );
            exit(1);
        }

    if ( send ( listenfd, sendbuff, strlen ( sendbuff ),0 ) == -1 )
    {
	perror ( "write" );
	exit(1);
    }
}

Server::Server(uint32_t addr, int port)
{
    memset(&serv_addr,0,sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl ( addr );
    serv_addr.sin_port = htons ( port );
       if(bind ( listenfd, ( struct sockaddr* ) &serv_addr, sizeof ( serv_addr ) ) == -1)
    {
	cout <<"error bind it, because "<< strerror(errno)<<endl;  
        exit(1);  
    }

    if(listen ( listenfd, 5 ) == -1)
    {
	cout <<"error listen to, bacause "<< strerror(errno)<<endl;  
        exit(1);  
    }
      
}
Server::~Server()
{
    close(listenfd);
}
void Server::Con()
{
    connfd = accept ( listenfd, ( struct sockaddr* ) NULL, NULL );
}
void Server::ShutCon()
{
    close(connfd);
}

json_object * Server::Rev()
{
    int r;
    string tmp;
    json_object *jobj = json_object_new_object();
  /*read command from client*/
    r= recv ( connfd, revbuff, MAX_SIZE,0 );
    if ( r == -1 )
    {
      cout<<"read error"<<endl;
      exit(1);
    }

    jobj = json_tokener_parse ( revbuff );
    
    return jobj;
}
void Server::SendTo(json_object * jobj)
{
    
    if ( strcpy ( sendbuff, json_object_to_json_string ( jobj ) ) == NULL )
	{
	    perror ( "strcpy" );
	    exit(1);
	}

    if ( send ( connfd, sendbuff, strlen ( sendbuff ),0 ) == -1 )
    {
	perror ( "write" );
	exit(1);
    }
}
