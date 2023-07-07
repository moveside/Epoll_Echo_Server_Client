#pragma once
#ifndef  CLIENT_H_
#define CLIENT_H_



#include "packet.h"

#include<iostream>

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include<thread>
#include<fcntl.h>

#include<WinSock2.h>
#include<sys/types.h>



using namespace std;

class Client
{
private:
	const char* SERVER_IP = "192.168.8.36";
	const unsigned int Port = 10007;

	WSADATA wsadata;
	SOCKET m_sockfd;
	SOCKADDR_IN m_addr;
	char m_name[16];
	Packet m_sendPacket;
public:
	~Client();
	SOCKET get_sockfd();

	bool start_client();
	void socket_setup();
	bool socket_connect();
	void socket_close();




	bool send_name(const char* name);
	void send_data();
	void send_delete();
	void send_log();



	bool send_packet(CMD cmd,string data);
	Packet recv_data();


};



#endif // ! CLIENT_H_