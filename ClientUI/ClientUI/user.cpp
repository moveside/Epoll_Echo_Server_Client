#include"user.h"




User::~User()
{

}

bool User::start()
{
	return client.start_client();
}

bool User::send_name(string name)
{
	strcpy(m_name, name.c_str());
	return client.send_name(name.c_str());
}

void User::set_name(char* str)
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
	return client.recv_data();
}

void User::user_cmd()
{
	while (1)
	{
		char msg;
		cout << "============COMMAND============" << endl;
		cout << "send data : 1" << endl;
		cout << "log       : 2" << endl;
		cout << "delete    : 3" << endl;
		cout << "exit      : 0" << endl;
		cout << "============COMMAND============" << endl;
		cout << "input cmd : ";
		cin >> msg;

		switch (msg)
		{
		case '1':
		{
			client.send_data();
			break;
		}
		case '2':
		{
			client.send_log();
			break;
		}
		case '3':
		{
			client.send_delete();
			break;
		}
		case '0':
		{
			client.socket_close();
			break;
		}
		default:
		{
			break;
		}
		}
		client.recv_data();
	}
}