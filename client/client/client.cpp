#include "client.h"
#include "packet.h"





Client::~Client()
{

}





void Client::start_client()
{
	WSAStartup(MAKEWORD(2, 0), &wsadata);


	socket_setup();
	socket_connect();

}


void Client::socket_setup()
{
	m_sockfd = socket(PF_INET, SOCK_STREAM, 0);

	memset(&m_addr, 0, sizeof(m_addr));
	m_addr.sin_family = AF_INET;
	m_addr.sin_addr.S_un.S_addr = inet_addr(SERVER_IP);
	m_addr.sin_port = htons(Port);
}

void Client::socket_connect()
{
	if (connect(m_sockfd, (sockaddr*)&m_addr, sizeof(m_addr)) != 0)
	{
		cout << "socket_connect error" << endl;
		exit(0);
	}

	cout << "connect success" << endl;
}

void Client::send_name(char* name)
{
	Packet packet;
	memset(&packet, 0, sizeof(packet));
	packet.cmd = CMD_USER_LOGIN_SEND;
	packet.data_size = 800;
	strcpy(packet.data, name);
	char* buffer = (char*)&packet;
	int n = send(m_sockfd, buffer, sizeof(packet),0);
	if (n < 0)
	{
		cout << "send error" << endl;
	}
}





void Client::send_data()
{
	char msg[1024];
	cout << "input send data : ";
	cin >> msg;
	Packet packet;
	packet.cmd = CMD_USER_DATA_SEND;
	strcpy(packet.data, msg);
	packet.data_size = 5;
	if ((send(m_sockfd, (char*)&packet, sizeof(packet), 0)) < 0)
	{
		cout << "send error" << endl;
	}
	else cout << packet.data << " send" << endl;
}

void Client::socket_loop()
{
	while (1)
	{
		cout << "LOOP" << endl;
		send_data();
		Sleep(1000);
		recv_data();
	}
	
}
void Client::recv_data()
{
	char buffer[1032];
	if (recv(m_sockfd, buffer, 1032, 0) < 0)
	{
		cout << "recv error" << endl;
	}
	Packet* pa = (Packet*)buffer;
	cout << pa->data << " recv" << endl;
}

