#include "client.h"





Client::~Client()
{
}
SOCKET Client::get_sockfd()
{
	return m_sockfd;
}




bool Client::start_client()
{
	WSAStartup(MAKEWORD(2, 0), &wsadata);


	socket_setup();
	if (!socket_connect()) return false;

}
void Client::socket_setup()
{
	m_sockfd = socket(PF_INET, SOCK_STREAM, 0);

	memset(&m_addr, 0, sizeof(m_addr));
	m_addr.sin_family = AF_INET;
	m_addr.sin_addr.S_un.S_addr = inet_addr(SERVER_IP);
	m_addr.sin_port = htons(Port);


}
bool Client::socket_connect()
{
	if (connect(m_sockfd, (sockaddr*)&m_addr, sizeof(m_addr)) != 0)
	{
		return false;
	}
	return true;
}


void Client::socket_close()
{
	closesocket(m_sockfd);
	WSACleanup();
	exit(0);
}

bool Client::send_packet(CMD cmd,string data)
{
	Packet packet;
	strcpy(packet.head.head, "Aa");
	strcpy(packet.tail.tail, "zZ");
	switch (cmd)
	{
	case LOGIN:
	{
		packet.body.cmd = CMD_USER_LOGIN_SEND;
		strcpy(m_name, data.c_str());
		strcpy(packet.body.data, m_name);
		strcpy(packet.body.name, m_name);
		break;
	}
	case SEND:
	{
		packet.body.cmd = CMD_USER_DATA_SEND;
		strcpy(packet.body.data, data.c_str());
		strcpy(packet.body.name, m_name);
		break;
	}
	case LOG:
	{
		packet.body.cmd = CMD_USER_LOG_SEND;
		strcpy(packet.body.name, m_name);
		break;
	}
	case DEL:
	{
		packet.body.cmd = CMD_USER_DEL_SEND;
		strcpy(packet.body.name, m_name);
		break;
	}

	default:
	{
		break;
	}
	}

	if ((send(m_sockfd, (char*)&packet, sizeof(packet), 0)) < 0)
	{
		return false;
	}
	return true;
}

RECVPacket Client::recv_packet()
{
	string buff;
	char buffer[sizeof(RECVPacket)];
	if (recv(m_sockfd, buffer, sizeof(buffer), 0) <0)
	{
		cout << "recv error" << endl;
	}
	RECVPacket* packet = (RECVPacket*)buffer;
	return *packet;
}
