#include"user.h"




User::~User()
{

}

bool User::start()
{
	return client.start_client();
}



void User::set_name(const char* str)
{
	strcpy(m_name, str);
}
char* User::get_name()
{
	return m_name;
}

bool User::send_data(CMD cmd, string data)
{
	return client.send_packet(cmd, data);
}

Packet User::recv_data()
{
	return client.recv_packet();
}

void User::disconnect()
{
	client.socket_close();
}