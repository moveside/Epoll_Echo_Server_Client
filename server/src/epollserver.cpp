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
	set_threadPool();
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

void Epollserver::set_threadPool()
{
	for(int i=0;i<Count_Readthread;i++)
	{
		threadPool.push_back(thread(&Epollserver::thread_read,this));
	}
	for(int i=0;i<Count_Writethread;i++)
	{
		threadPool.push_back(thread(&Epollserver::thread_write,this));
	}
	for(int i=0;i<2;i++)
	{
		threadPool[i].detach();
	}
}

void Epollserver::client_connect()
{
	struct sockaddr_in client_addr;
	socklen_t client_addr_len = sizeof(client_addr);
	int client_fd;
	if((client_fd = accept(m_sockfd,(struct sockaddr *)&client_addr,&client_addr_len)) < 0)
	{
		perror("client connect");
		return;
	}
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
		if(m_user_name.find(packet->name) != m_user_name.end()) // 로그인 실패
		{
			m_sendPacket.data[0]='0';
		}
		else // 성공
		{
			User *user = new User(packet->name,client_fd);
			m_users.insert(make_pair(client_fd,user));
			m_user_name.insert(packet->name);
			cout << user->get_name() << " Login" << endl;
			m_sendPacket.data[0]='1';
		}
		m_sendPacket.cmd = CMD_USER_LOGIN_RECV;

		break;
	}
	case CMD_USER_DATA_SEND:
	{
		cout << packet->data << " recv by " <<  packet->name <<endl;

		string h = packet->name;
		m_clients_log.push_back(make_pair(packet->name,packet->data));

		if(m_clients_log.size()>=40)
		{
			m_clients_log.pop_front();
		}
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
				{ return (el.first.compare(n)==0);}
		);

		m_sendPacket.cmd = CMD_USER_DEL_RECV;
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
	if((write(client_fd,(char*)&m_sendPacket,sizeof(m_sendPacket))) < 0)
	{
		perror("write");
	}
}

void Epollserver::thread_read()
{
	while(1)
	{
		if(!m_requestPool.empty())
		{

			request_mutex.lock();

			pair<Packet,int> el = m_requestPool.front();
			m_requestPool.pop();

			request_mutex.unlock();

			Packet* packet = new Packet();
			switch(el.first.cmd)
			{
			case CMD_USER_LOGIN_SEND:
			{
				if(m_user_name.find(el.first.name) != m_user_name.end()) // 로그인 실패
				{
					packet->data[0]='0';
				}
				else // 성공
				{
					User *user = new User(el.first.name,el.second);
					static mutex login_mutex;
					login_mutex.lock();

					m_users.insert(make_pair(el.second,user));
					m_user_name.insert(el.first.name);

					login_mutex.unlock();
					cout << user->get_name() << " Login thread" << endl;
					packet->data[0]='1';
				}
				packet->cmd = CMD_USER_LOGIN_RECV;
				strcpy(packet->name,el.first.name);
				break;
			}
			case CMD_USER_DATA_SEND:
			{
				cout << el.first.data << " recv by " <<  el.first.name <<endl;

				string h = el.first.name;
				static mutex logPush_mutex;
				logPush_mutex.lock();

				m_clients_log.push_back(make_pair(el.first.name,el.first.data));
				if(m_clients_log.size()>=40)
				{
					m_clients_log.pop_front();
				}

				logPush_mutex.unlock();
				packet->cmd = CMD_USER_DATA_RECV;
				strcpy(packet->data,el.first.data);
				strcpy(packet->name,el.first.name);
				break;
			}
			case CMD_USER_DEL_SEND:
			{
				cout  <<  el.first.name << " delete request" << endl;
				string n = el.first.name;
				static mutex logRemove_mutex;
				logRemove_mutex.lock();

				m_clients_log.remove_if([n](pair<string,string> el)
						{ return (el.first.compare(n)==0);}
				);

				logRemove_mutex.unlock();
				packet->cmd = CMD_USER_DEL_RECV;
				strcpy(packet->name,el.first.name);
				break;
			}
			case CMD_USER_LOG_SEND:
			{
				cout  <<  el.first.name << " log request" << endl;
				string log;
				for(auto el : m_clients_log)
				{
					log += el.first;
					log += " : ";
					log += el.second;
					log += "\n";
				}
				packet->cmd = CMD_USER_LOG_RECV;
				strcpy(packet->data,log.c_str());
				strcpy(packet->name,el.first.name);
				break;
			}
			default:
			{
				break;
			}
			}
			m_sendPool.push(make_pair(packet,el.second));
			cout << "push" << endl;
		}
	}
}

void Epollserver::thread_write()
{
	while(1)
	{
		if(!m_sendPool.empty())
		{
			cout << "write" << endl;
			send_mutex.lock();

			pair<Packet*,int> el = m_sendPool.front();
			m_sendPool.pop();

			send_mutex.unlock();
			if((write(el.second,(char*)el.first,sizeof(Packet))) < 0)
			{
				perror("write");
			}
		}
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
				char buffer[sizeof(Packet)];
				int n = read(client,buffer,sizeof(buffer));
				if(n < 0) // 에러
				{
					break;
				}
				else if(n==0) // 연결 해제
				{
					auto findIter = m_users.find(client);
					if(findIter != m_users.end())
					{
					cout << findIter->second->get_name() << " disconnect" << endl;
					m_user_name.erase(findIter->second->get_name());
					m_users.erase(findIter);
					epoll_ctl(m_epfd,EPOLL_CTL_DEL,client,&m_ev);
					close(client);
					}
				}
				else
				{
					packet = (Packet*)buffer;
					m_requestPool.push(make_pair(*packet,client));
				}
			}
		}
	}
}


