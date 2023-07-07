#pragma once
#ifndef  user_H_
#define user_H_

#include "client.h"

class User
{
private:
	char m_name[16];

	Client client;

public:
	~User();

	bool start();
	bool send_name(string name);

	void set_name(char* str);
	char* get_name();

	bool send_data(CMD cmd,string data);
	Packet recv_data();
	

	void user_cmd();
};

#endif