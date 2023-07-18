#pragma once
#ifndef PACKET_H_
#define PACKET_H_





enum packetCommand
{
	CMD_USER_LOGIN_SEND = 1,
	CMD_USER_LOGIN_RECV,
	CMD_USER_DATA_SEND,
	CMD_USER_DATA_RECV,
	CMD_USER_DEL_SEND,
	CMD_USER_DEL_RECV,
	CMD_USER_LOG_SEND,
	CMD_USER_LOG_RECV
};

struct Packet
{
	packetCommand cmd;
	char data[2048];
	char name[16];
};


enum CMD
{
	SEND = 1,
	LOG,
	DEL,
	LOGIN
};


#endif // !PACKET_H_