#include "client.h"
#include "packet.h"





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


bool Client::send_name(const char* name)
{
	Packet packet;
	memset(&packet, 0, sizeof(packet));
	packet.cmd = CMD_USER_LOGIN_SEND;
	strcpy(packet.name, name);
	char* buffer = (char*)&packet;
	int n = send(m_sockfd, buffer, sizeof(packet), 0);
	if (n < 0)
	{
		return false;
	}
	strcpy(m_name, name);
	return true;
}


void Client::send_data()
{
	char msg[1024];
	cout << "input data : ";
	cin >> msg;

	m_sendPacket.cmd = CMD_USER_DATA_SEND;
	strcpy(m_sendPacket.data, msg);
	strcpy(m_sendPacket.name, m_name);
	// else cout << packet.data << " send" << endl;
}

void Client::send_delete()
{
	m_sendPacket.cmd = CMD_USER_DEL_SEND;
	strcpy(m_sendPacket.name, m_name);
}

void Client::send_log()
{
	cout << "send log" << endl;
	m_sendPacket.cmd = CMD_USER_LOG_SEND;
	strcpy(m_sendPacket.name, m_name);
}

bool Client::send_packet(CMD cmd,string data)
{
	memset(&m_sendPacket, 0, sizeof(m_sendPacket));
	switch (cmd)
	{
	case SEND:
	{
		m_sendPacket.cmd = CMD_USER_DATA_SEND;
		strcpy(m_sendPacket.data, data.c_str());
		strcpy(m_sendPacket.name, m_name);
		break;
	}
	case LOG:
	{
		m_sendPacket.cmd = CMD_USER_LOG_SEND;
		strcpy(m_sendPacket.name, m_name);
		break;
	}
	case DEL:
	{
		m_sendPacket.cmd = CMD_USER_DEL_SEND;
		strcpy(m_sendPacket.name, m_name);
		break;
	}
	case LOGIN:
	{
		m_sendPacket.cmd = CMD_USER_LOGIN_SEND;
		strcpy(m_name, data.c_str());
		strcpy(m_sendPacket.name, m_name);
	}
	default:
	{
		break;
	}
	}
	if ((send(m_sockfd, (char*)&m_sendPacket, sizeof(m_sendPacket), 0)) < 0)
	{
		return false;
	}
	return true;
}

Packet Client::recv_data()
{
	char buffer[1050];
	if (recv(m_sockfd, buffer, 1050, 0) < 0)
	{
		cout << "recv error" << endl;
	}
	Packet* packet = (Packet*)buffer;
	switch (packet->cmd)
	{
	case CMD_USER_LOGIN_RECV:
	{
		cout << "LOGIN SUCCESS" << endl;
		break;
	}
	case CMD_USER_DATA_RECV:
	{
		cout << packet->name << " : " << packet->data << endl;
		break;
	}
	case CMD_USER_DEL_RECV:
	{
		cout << "DELETE SUCCESS" << endl;
		break;
	}
	case CMD_USER_LOG_RECV:
	{
		cout << "============LOG============" << endl;
		cout << packet->data;
		cout << "============LOG============" << endl;
		break;
	}
	default:
	{
		cout << "error packet" << endl;
		break;
	}
	}
	return *packet;
}
