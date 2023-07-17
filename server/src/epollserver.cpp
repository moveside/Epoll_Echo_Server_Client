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
	if(fcntl(m_sockfd,F_SETFL,flags | O_NONBLOCK) < 0)
	{
		cout << "nonblock fail" << endl;
	}

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
	threadPool.push_back(thread(&Epollserver::socket_wait,this));
	threadPool.push_back(thread(&Epollserver::thread_buffer,this));
	for(int i=0;i<Count_Readthread;i++)
	{
		threadPool.push_back(thread(&Epollserver::thread_read,this));
	}
	for(int i=0;i<Count_Writethread;i++)
	{
		threadPool.push_back(thread(&Epollserver::thread_write,this));
	}
	int num_threads = Count_Readthread + Count_Writethread + 2;
	for(int i=0;i<num_threads;i++)
	{
		threadPool[i].join();
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

/*
void Epollserver::server_read(Packet* packet,int client_fd)
{
	memset(&m_sendPacket,0,sizeof(m_sendPacket));
	switch(packet->body.cmd)
	{
	case CMD_USER_LOGIN_SEND:
	{
		if(m_user_name.find(packet->body.name) != m_user_name.end()) // 로그인 실패
		{
			m_sendPacket.body.data[0]='0';
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
*/
void Epollserver::server_write(int client_fd)
{
	if((write(client_fd,(char*)&m_sendPacket,sizeof(m_sendPacket))) < 0)
	{
		perror("write");
	}
}

void Epollserver::thread_read()
{
	while (1)
	{
		unique_lock<mutex> lock_pop(request_mutex);
		cv_request.wait(lock_pop, [this]() {return !this->m_requestPool.empty();});


		pair<RECVBODY*, int> el = move(m_requestPool.front());
		m_requestPool.pop();

		lock_pop.unlock();

		Packet *packet = new Packet();
		switch (el.first->cmd)
		{
		case CMD_USER_LOGIN_SEND:
		{
			lock_guard<mutex> lock(user_mutex);
			if (m_user_name.find(el.first->name) != m_user_name.end()) // 로그인 실패
			{
				packet->body.data[0] = '0';
			} else // 성공
			{
				User *user = new User(el.first->name, el.second);

				m_users.insert(make_pair(el.second, user));
				m_user_name.insert(el.first->name);

				cout << user->get_name() << " Login thread" << endl;
				packet->body.data[0] = '1';
			}
			packet->body.cmd = CMD_USER_LOGIN_RECV;
			strcpy(packet->body.name, el.first->name);
			break;
		}
		case CMD_USER_DATA_SEND:
		{
			lock_guard<mutex> lock(log_mutex);
			//cout << el.first->data << " recv by " << el.first->name << endl;
			m_clients_log.push_back(make_pair(el.first->name, el.first->data));
			if (m_clients_log.size() >= 40)
			{
				m_clients_log.pop_front();
			}
			packet->body.cmd = CMD_USER_DATA_RECV;
			strcpy(packet->body.data, el.first->data);
			strcpy(packet->body.name, el.first->name);
			break;
		}
		case CMD_USER_DEL_SEND:
		{
			lock_guard<mutex> lock(log_mutex);
			cout << el.first->name << " delete request" << endl;
			string n = el.first->name;

			m_clients_log.remove_if([n](pair<string, string> el)
					{return (el.first.compare(n) == 0);});
			packet->body.cmd = CMD_USER_DEL_RECV;
			strcpy(packet->body.name, el.first->name);
			break;
		}
		case CMD_USER_LOG_SEND:
		{
			lock_guard<mutex> lock(log_mutex);
			cout << el.first->name << " log request" << endl;
			string log;
			for (auto el : m_clients_log)
			{
				log += el.first;
				log += " : ";
				log += el.second;
				log += "\n";
			}
			packet->body.cmd = CMD_USER_LOG_RECV;
			strcpy(packet->body.data, log.c_str());
			strcpy(packet->body.name, el.first->name);
			break;
		}
		default:
		{
			cout << "packet error" << endl;
			break;
		}
		}
		strcpy(packet->head.head,"Aa");
		strcpy(packet->tail.tail,"zZ");
		{
			lock_guard<mutex> lock_push(send_mutex);
			m_sendPool.push(make_pair(packet, el.second));
		}
		cv_send.notify_one();
		usleep(1000);
	}// while(1)
}

void Epollserver::thread_write()
{
	while(1)
	{
		unique_lock<mutex> lock(send_mutex);
		cv_send.wait(lock,[this](){return !this->m_sendPool.empty();});

		pair<Packet*,int> el = m_sendPool.front();
		m_sendPool.pop();
		int n = (write(el.second,(char*)el.first,sizeof(Packet)));
		cout << n;
		while(n < 0)
		{
			if(errno == EAGAIN)
			{
				cout << "EAGAIN : " << el.first->body.data << " send" << endl;
				usleep(1000000);
				n = (write(el.second,(char*)el.first,sizeof(Packet)));
			}
			else
			{
				perror("write");
			}

		}
		cout << " send : " << el.first->body.data << endl;
		lock.unlock();
		usleep(1000);
	}
}

void Epollserver::thread_buffer()
{
	while(1)
	{
		unique_lock<mutex> lock_ringbuffer(ringbuffer_mutex);
		cv_ringbuffer.wait(lock_ringbuffer,[this](){return !this->ringbuffer.is_empty();});
		int st_index;
		int ed_index;
		int client;
		char make_buff[sizeof(RECVPacket)];
		int buff_len = 0;
		while(1)
		{
			if((st_index = (ringbuffer.find_str("Aa"))) >=0)
			{
				st_index += sizeof(HEAD)+1;
				client = ringbuffer.get_client();
				break;
			}
			else
			{
				ringbuffer.dequeue_buffer(0, ringbuffer.get_size());
			}
			if(ringbuffer.is_empty()) break;
		}
		while(1)
		{
			if((ed_index = (ringbuffer.find_str("zZ"))) >=0)
			{
				ed_index -= 1;
				char* deq_buffer = ringbuffer.dequeue_buffer(st_index,ed_index);
				memcpy(make_buff+buff_len,deq_buffer,ed_index-st_index);
				break;
			}
			else
			{
				ed_index = ringbuffer.get_size();
				char* deq_buffer = ringbuffer.dequeue_buffer(0, ed_index);
				memcpy(make_buff+buff_len,deq_buffer,ed_index-st_index);
				buff_len+=ed_index;
				st_index = 0;
			}
		}
		lock_ringbuffer.unlock();
		//cv_ringbuffer.notify_one();
		RECVBODY* t = (RECVBODY*)make_buff;
		{
			lock_guard<mutex> lock(request_mutex);
			m_requestPool.push(make_pair(t,client));
		}
		cv_request.notify_one(); // 스레드 하나를 깨운다.
		usleep(1000);
	}// while(1)
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
			if(m_events[i].data.fd == m_sockfd) // 새로운 client 연결
			{
				client_connect();
			}
			else
			{
				int client = m_events[i].data.fd;
				bool can_read = true;
				string st;
				while(can_read)
				{
					char buffer[60];
					int n = read(client,buffer,sizeof(RECVPacket));
					if(n == -1)
					{
						if(errno != EAGAIN) // read 에러
						{
							perror("read");
						}
						can_read = false;
					}
					else if(n==0) // 연결 해제
					{
						lock_guard<mutex> lock(user_mutex);
						auto findIter = m_users.find(client);
						if(findIter != m_users.end())
						{
							cout << findIter->second->get_name() << " disconnect" << endl;
							m_user_name.erase(findIter->second->get_name());
							m_users.erase(findIter);
							epoll_ctl(m_epfd,EPOLL_CTL_DEL,client,&m_ev);
							close(client);
						}
						can_read = false;
					}
					else
					{
						unique_lock<mutex> lock_ringbuffer(ringbuffer_mutex);
						cv_ringbuffer.wait(lock_ringbuffer,[this]()
								{return !this->ringbuffer.is_full();}); // ringbuffer가 가득차면 기다린다
						ringbuffer.enqueue_buffer(buffer,n,client);
						cv_ringbuffer.notify_one();
					}
					usleep(2000);
				} // while(can_read)
			} // else
		}// for(int i=0;i<nfd;i++)
	} // while(1)
}


