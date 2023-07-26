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
	shutdown(m_sockfd, SD_SEND);
	//closesocket(m_sockfd);
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
	char buffer[sizeof(RECVPacket)];
	char make_buff[sizeof(RECVPacket)];
	RECVPacket* packet = nullptr;
	int len = 0;
	int n = recv(m_sockfd, buffer, sizeof(buffer), 0);
	if (n < 0)
	{
		cout << "recv error" << endl;
		return *packet;
	}
	memcpy(make_buff, buffer, n);
	int recv_len = n;
	while (recv_len < sizeof(RECVPacket))
	{
		cout << "can't recv all Packet" << endl;
		char tmp_buffer[sizeof(RECVPacket)];
		n = recv(m_sockfd, tmp_buffer, sizeof(RECVPacket)-recv_len, 0);
		memcpy(make_buff + recv_len, tmp_buffer, n);
		recv_len += n;
		cout << recv_len << endl;
	}
	if (recv_len != sizeof(RECVPacket)) cout << "===========ERROR==========" << endl;
	packet = (RECVPacket*)make_buff;
	return *packet;
}
