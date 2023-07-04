#include"user.h"




User::~User()
{

}

void User::start()
{
	client.start_client();
	input_name();
	cout << "your name : " << get_name() << endl;
	
	client.send_name(get_name());
	client.socket_loop();
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
		cout << "input command : ";

	}
}