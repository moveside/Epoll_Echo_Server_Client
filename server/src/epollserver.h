/*
 * server.h
 *
 *  Created on: Jun 30, 2023
 *      Author: yeob
 */

#ifndef EPOLLSERVER_H_
#define EPOLLSERVER_H_


#include "packet.h"
#include "user.h"

#include<iostream>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <vector>
#include <list>
#include <unordered_map>
#include <string>
#include <set>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <errno.h>



using namespace std;

class Epollserver
{
private:
	const static int Maxevent = 16;
	const int Port = 10007;

	int m_sockfd;
	struct sockaddr_in m_addr;
	int m_epfd;
	struct epoll_event m_ev;
	struct epoll_event m_events[Maxevent];
	Packet m_sendPacket;

	unordered_map<int,User*> m_users;
	list<pair<string,string>> m_clients_log;
	set<string> m_user_name;
public:

	~Epollserver();

	void start();
	void socket_setup();
	void socket_bind();
	void socket_ctl();
	void client_connect();
	void socket_wait();
	void server_read(Packet* packet,int client_fd);
	void server_write(int clinet_fd);
};





#endif /* EPOLLSERVER_H_ */
