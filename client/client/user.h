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

	void start();

	void input_name();
	void set_name(char* str);
	char* get_name();

	void user_cmd();
};

#endif