/*
 * packet.h
 *
 *  Created on: Jul 4, 2023
 *      Author: yeob
 */

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
  CMD_USER_LOG_RECV,
  CMD_USER_NAME_RECV
};

#pragma pack(push,1)

struct HEAD
{
	char head[3];
	int dataSize;
};
struct RECVBODY
{
	packetCommand cmd;
	char data[30];
	char name[16];
};
struct BODY
{
	packetCommand cmd;
	char data[2048];
};
struct COMBODY
{
	packetCommand cmd;
	char* data;
};
struct TAIL
{
	char tail[3];
};

struct Packet
{
	HEAD head;
	BODY body;
	TAIL tail;
};
struct RECVPacket
{
	HEAD head;
	RECVBODY body;
	TAIL tail;
};

#pragma pack(pop)

#endif /* PACKET_H_ */
