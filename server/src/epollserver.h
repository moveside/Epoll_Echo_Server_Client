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
#include "ringbuffer.h"

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
#include <queue>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <errno.h>
#include <thread>
#include <mutex>
#include <pthread.h>
#include <memory>
#include <condition_variable>
#include <time.h>
using namespace std;

class Epollserver
{
private:
	const static int Maxevent = 16;
	const int Port = 10007;
	const int Count_Readthread = 5;
	const int Count_Writethread = 5;

	int m_sockfd;
	struct sockaddr_in m_addr;
	int m_epfd;
	struct epoll_event m_ev;
	struct epoll_event m_events[Maxevent];
	Packet m_sendPacket;

	unordered_map<int,User*> m_users;
	set<string> m_user_name;
	list<pair<string,string>> m_clients_log;


	vector<thread> threadPool;

	queue <pair<RECVPacket*,int>> m_requestPool;
	queue <pair<Packet*,int>> m_sendPool;

	// thread_read 관련 변수
	condition_variable cv_request;
	mutex request_mutex;
	mutex user_mutex;
	mutex log_mutex;
	mutex username_mutex;
	// thread_send 관련 변수
	condition_variable cv_send;
	mutex send_mutex;

	// RingBuffer 관련 변수
	RingBuffer ringbuffer;
	condition_variable cv_ringbuffer;
	mutex ringbuffer_mutex;

public:

	~Epollserver();

	void start();
	void socket_setup();
	void socket_bind();
	void socket_ctl();
	void client_connect();
	void socket_wait();
	void set_threadPool();
	void server_read(Packet* packet,int client_fd);
	void thread_read();
	void thread_write();
	void server_write(int clinet_fd);
	void thread_buffer();
};





#endif /* EPOLLSERVER_H_ */
