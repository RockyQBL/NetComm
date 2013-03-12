#include "NC_Server.h"

using namespace std;

NC_Server::NC_Server()
{

}

NC_Server::NC_Server(string address, unsigned int port) 
                : NetComm(true, address, port)
{

}

NC_Server::~NC_Server()
{

}

void NC_Server::Listen()
{
//{{{
    //listen on master socket
    listen(this->master_socket, this->backlog_length);

    #if DEBUG
        cout<<"Queue Length "<<this->backlog_length<<", Listening..."<<endl;
    #endif
//}}}
}

void NC_Server::Listen(int connBacklog)
{
//{{{
    //make sure queue length is up to date
    this->backlog_length = connBacklog;

    //listen on master socket
    listen(this->master_socket,this->backlog_length);

    #if DEBUG
        cout<<"Queue Length "<<this->queueLength<<", Listening..."<<endl;
    #endif
//}}}
}

int NC_Server::Accept()
{
//{{{
    //need size of socket
    this->socket_length = sizeof(this->socket_address);

     #if DEBUG
        cout<<"Blocking on accept..."<<endl;
    #endif

    //accept a single connectiong ans store reutrned socket to temporary client struct
    temp_client.socket_fd = accept(master_socket, 
             (struct sockaddr *) &socket_address,
                                  &socket_length);

    #if DEBUG
        cout<<"Accepted Connection..."<<endl;
    #endif

     //push updated temp client struct to vector of connected clients
     this->connected_clients.push_back(temp_client);

     //return the integer id of the now connected client
     return connected_clients.size() - 1;
//}}}
}

void NC_Server::Send(unsigned char *data, int bytes)
{
    send(this->connected_clients.back().socket_fd, data, bytes, 0);
}

void NC_Server::Send(unsigned char *data, int bytes, int client_id)
{
    send(this->connected_clients[client_id].socket_fd, data, bytes, 0);
}

void NC_Server::Send(unsigned char *data, int bytes, string client_name)
{
//{{{
    int client_id = FindClient(client_name);
    if(client_id >= 0)
        send(this->connected_clients[client_id].socket_fd, data, bytes, 0);
    else
        cout<<"Unable to send, not such client..."<<endl;
//}}}
}


int NC_Server::Receive(unsigned char *buffer, int bytes)
{
    read(this->connected_clients.back().socket_fd, buffer, bytes);
}

int NC_Server::Receive(unsigned char *buffer, int bytes, int client_id)
{
    read(this->connected_clients[client_id].socket_fd, buffer, bytes);
}

int NC_Server::Receive(unsigned char *buffer, int bytes, string client_name)
{
//{{{
int client_id = FindClient(client_name);

if(client_id >= 0)
    read(this->connected_clients.back().socket_fd, buffer, bytes);
else
    cout<<"Cannot receive from a non-existent client..."<<endl;
//}}}
}


bool NC_Server::SetClientName(int client_id, string client_name)
{
//{{{
if(client_id < 0 || client_id > this->connected_clients.size() - 1)
{
    cout<<"No client with that id exists..."<<endl;
    return false;
}

this->connected_clients[client_id].name = client_name;
return true;
//}}}
}

int NC_Server::FindClient(std::string client_name)
{
//{{{
for(int i = 0; i < this->connected_clients.size(); i++)
{
    if(connected_clients[i].name == client_name)
        return i;
}

printf("Client not found in connected clients listing...\n");
return -1;
//}}}
}