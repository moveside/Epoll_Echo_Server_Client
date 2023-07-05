//============================================================================
// Name        : server.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "epollserver.h"


Epollserver::~Epollserver()
{
	cout << "server end" << endl;
}


void Epollserver::start()
{
	socket_setup();
	socket_bind();
	socket_ctl();
	socket_wait();
}


void Epollserver::socket_setup()
{
	if((m_sockfd = socket(PF_INET,SOCK_STREAM,0)) < 0)
	{
		perror("create socket");
	}
	// 논블로킹
	int flags = fcntl(m_sockfd,F_GETFL);
	fcntl(m_sockfd,F_SETFL,flags | O_NONBLOCK);

}
void Epollserver::socket_bind()
{
	memset(&m_addr,0,sizeof(m_addr));
	m_addr.sin_family = AF_INET;
	m_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	m_addr.sin_port = htons(Port);

	if(bind(m_sockfd,(struct sockaddr *)&m_addr,sizeof(m_addr)) < 0)
	{
		close(m_sockfd);
		perror("socket bind");
	}
	if(listen(m_sockfd,10) < 0)
	{
		close(m_sockfd);
		perror("socket listen");
	}
}
void Epollserver::socket_ctl()
{
	if((m_epfd = epoll_create(5))<0)
	{
		close(m_sockfd);
		perror("epoll_create");
	}
	m_ev.events = EPOLLIN;
	m_ev.data.fd = m_sockfd;
	if(epoll_ctl(m_epfd,EPOLL_CTL_ADD,m_sockfd,&m_ev) < 0)
	{
		perror("epoll_ctl");
		close(m_sockfd);
		close(m_epfd);
	}
}

void Epollserver::client_connect()
{
	cout << "connect start" << endl;
	struct sockaddr_in client_addr;
	socklen_t client_addr_len = sizeof(client_addr);
	int client_fd;
	if((client_fd = accept(m_sockfd,(struct sockaddr *)&client_addr,&client_addr_len)) < 0)
	{
		perror("client connect");
		return;
	}
	//논블로킹
	int flag = fcntl(client_fd,F_GETFL,0);
	fcntl(client_fd,F_SETFL,flag|O_NONBLOCK);

	struct epoll_event event;
	memset(&event,0,sizeof(event));
	event.events = EPOLLIN;
	event.data.fd = client_fd;
	if(epoll_ctl(m_epfd,EPOLL_CTL_ADD,client_fd,&event) < 0)
	{
		perror("client epoll_ctl");
		return;
	}
	cout << "connect success" << endl;
}


void Epollserver::server_read(Packet* packet,int client_fd)
{
	memset(&m_sendPacket,0,sizeof(m_sendPacket));
	switch(packet->cmd)
	{
	case CMD_USER_LOGIN_SEND:
	{
		User *user = new User(packet->name,client_fd);
		m_users.insert(make_pair(client_fd,user));
		cout << user->get_name() << " connect" << endl;

		m_sendPacket.cmd = CMD_USER_LOGIN_RECV;
		break;
	}
	case CMD_USER_DATA_SEND:
	{
		cout << packet->data << " recv by " <<  packet->name <<endl;

		auto findIter = m_users.find(client_fd);
		findIter->second->add_log(packet->data); // 유저별 개
		std::string h = packet->name;
		m_clients_log.push_back(make_pair(packet->name,packet->data));

		m_sendPacket.cmd = CMD_USER_DATA_RECV;
		strcpy(m_sendPacket.data,packet->data);
		strcpy(m_sendPacket.name,packet->name);
		break;
	}
	case CMD_USER_DEL_SEND:
	{
		cout  <<  packet->name << " delete request" << endl;
		string n = packet->name;
		m_clients_log.remove_if([n](pair<string,string> el)
				{ return (el.first.compare(n)==0);
				}
		);

		break;
	}
	case CMD_USER_LOG_SEND:
	{
		cout  <<  packet->name << " log request" << endl;
		string log;
		for(auto el : m_clients_log)
		{
			log += el.first;
			log += " : ";
			log += el.second;
			log += "\n";
		}

		m_sendPacket.cmd = CMD_USER_LOG_RECV;
		strcpy(m_sendPacket.data,log.c_str());
		strcpy(m_sendPacket.name,packet->name);
		break;
	}
	default:
	{
		break;
	}
	}
}

void Epollserver::server_write(int client_fd)
{
	if((write(client_fd,(char*)&m_sendPacket,1050)) < 0)
	{
		perror("write");
	}


}
void Epollserver::socket_wait()
{
	while(1)
	{
		int nfd = epoll_wait(m_epfd,m_events,10,-1);
		if(nfd < 0)
		{
			perror("epoll_wait");
		}
		for(int i=0;i<nfd;i++)
		{
			Packet* packet;
			if(m_events[i].data.fd == m_sockfd) // 새로운 client 연결
			{
				client_connect();
			}
			else
			{
				int client = m_events[i].data.fd;
				char buffer[1050];
				int n = read(client,buffer,1050);
				if(n < 0)
				{
					break;
						/*
						perror("read error");
						epoll_ctl(m_epfd,EPOLL_CTL_DEL,client,&m_ev);
						close(client);
						*/
				}
				else if(n==0)
				{
					auto findIter = m_users.find(client);
					cout << findIter->second->get_name() << " disconnect" << endl;
					m_users.erase(findIter);
					epoll_ctl(m_epfd,EPOLL_CTL_DEL,client,&m_ev);
					close(client);
				}
				else
				{
					packet = (Packet*)buffer;
					server_read(packet,client);
					server_write(client);
				}


			}
		}
	}
}


