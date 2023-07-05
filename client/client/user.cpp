#include"user.h"




User::~User()
{

}

void User::start()
{
	client.start_client();
	input_name();
	cout << "your name : " << get_name() << endl;
	
	client.send_name(m_name);
	
	user_cmd();
}

void User::input_name()
{
	char inp[16];
	cout << "input name : ";
	cin >> inp;
	set_name(inp);
}
void User::set_name(char* str)
{
	strcpy(m_name, str);
}
char* User::get_name()
{
	return m_name;
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
		client.send_packet();
		client.recv_data();
	}
}