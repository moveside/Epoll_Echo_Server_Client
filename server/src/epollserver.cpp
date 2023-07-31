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
	for(auto el : m_users)
	{
		epoll_ctl(m_epfd,EPOLL_CTL_DEL,el.first,&m_ev);
		close(el.first);
	}
	cout << "server end" << endl;
}


void Epollserver::start()
{
	socket_setup();
	socket_bind();
	socket_ctl();
	set_threadPool();
	//socket_wait();
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

void Epollserver::server_write(int client_fd)
{
	if((write(client_fd,(char*)&m_sendPacket,sizeof(m_sendPacket))) < 0)
	{
		perror("write");
	}
}
*/

void Epollserver::thread_read()
{
	while (1)
	{
		unique_lock<mutex> lock_pop(request_mutex);
		cv_request.wait(lock_pop, [this]() {return !this->m_requestPool.empty();});


		pair<RECVPacket*, int> el = move(m_requestPool.front());
		m_requestPool.pop();
		lock_pop.unlock();

		Packet *packet = new Packet;
		switch (el.first->body.cmd)
		{
		case CMD_USER_LOGIN_SEND:
		{
			lock_guard<mutex> lock(user_mutex);
			if (m_user_name.find(el.first->body.name) != m_user_name.end()) // 로그인 실패
			{
				packet->body.data[0] = '0';
			}
			else // 성공
			{
				User *user = new User(el.first->body.name, el.second);

				m_users.insert(make_pair(el.second, user));
				m_user_name.insert(el.first->body.name);

				cout << user->get_name() << " Login thread" << endl;
				packet->body.data[0] = '1';
			}
			packet->body.cmd = CMD_USER_LOGIN_RECV;
			strcpy(packet->body.name, el.first->body.name);
			break;
		}
		case CMD_USER_DATA_SEND:
		{
			{
				lock_guard<mutex> lock(log_mutex);
				m_clients_log.push_back(make_pair(el.first->body.name, el.first->body.data));
				if (m_clients_log.size() > 40)
				{
					m_clients_log.pop_front();
				}
			}
			packet->body.cmd = CMD_USER_DATA_RECV;
			strcpy(packet->body.data, el.first->body.data);
			strcpy(packet->body.name, el.first->body.name);
			break;
		}
		case CMD_USER_DEL_SEND:
		{
			{
				lock_guard<mutex> lock(log_mutex);
				cout << el.first->body.name << " delete request" << endl;
				string n = el.first->body.name;

				m_clients_log.remove_if([n](pair<string, string> el)
						{return (el.first.compare(n) == 0);});
			}
			packet->body.cmd = CMD_USER_DEL_RECV;
			strcpy(packet->body.name, el.first->body.name);
			break;
		}
		case CMD_USER_LOG_SEND:
		{
			{
				lock_guard<mutex> lock(log_mutex);
				cout << el.first->body.name << " log request" << endl;
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
				strcpy(packet->body.name, el.first->body.name);
			}
			{
				Packet *namePacket = new Packet;
				lock_guard<mutex> lock(user_mutex);
				string name;
				for(auto el : m_user_name)
				{
					name += el;
					name += "\n";
				}
				namePacket->body.cmd = CMD_USER_NAME_RECV;
				strcpy(namePacket->body.data, name.c_str());
				strcpy(namePacket->body.name, el.first->body.name);
				strcpy(namePacket->head.head,"Aa");
				strcpy(namePacket->tail.tail,"zZ");
				{
					lock_guard<mutex> lock_push(send_mutex);
					m_sendPool.push(make_pair(namePacket, el.second));
				}
				cv_send.notify_one();
			}
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
		free(el.first);
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
		lock.unlock();
		int send_len = 0;
		while(send_len < sizeof(Packet))
		{
			int n = (write(el.second,(char*)el.first+send_len,sizeof(Packet)-send_len));
			if(n<0)
			{
				if(errno == EAGAIN) // buffer가 가득차서 못보냄. 기다려야함
				{
					cout << "EAGAIN : " << el.first->body.data << " send" << endl;
					sleep(1);
				}
				else // write error
				{
					perror("write");
				}
			}
			else
			{
				send_len +=n;
			}
			if(send_len !=sizeof(Packet) && n > 0) // 보냈지만 일부분만 보냈음. 다시보내야함
			{
				cout << "can't send all Packet!!" << " send : " << n << "size" << endl;
			}
		}
		if(send_len != sizeof(Packet))
		{
			cout << "================ERROR=============" << endl;
		}

		free(el.first);
		usleep(1000);
	}
}

void Epollserver::thread_buffer()
{
	while(1)
	{
		unique_lock<mutex> lock_ringbuffer(ringbuffer_mutex);
		cv_ringbuffer.wait(lock_ringbuffer
				,[this](){return !this->ringbuffer.is_empty();});
		int st_index;
		int client;
		char* make_buff = new char[sizeof(RECVPacket)];
		while(1)
		{
			if(ringbuffer.is_empty()) break;
			if((st_index = (ringbuffer.find_str("Aa"))) >=0)
			{
				client = ringbuffer.get_client();
				break;
			}
			else
			{
				ringbuffer.dequeue_buffer(0, ringbuffer.get_size());
			}
		}
		while(1)
		{
			if(ringbuffer.is_empty()) break;
			int buff_len = sizeof(RECVPacket);
			int size = ringbuffer.get_size() - st_index;
			char* deq_buffer;

			if(buff_len > size)
			{
				deq_buffer = ringbuffer.dequeue_buffer(st_index, size);
				memcpy(make_buff+(sizeof(RECVPacket)-buff_len),deq_buffer,size);
				buff_len -= (size);
			}
			else
			{
				deq_buffer = ringbuffer.dequeue_buffer(st_index, buff_len);
				memcpy(make_buff+(sizeof(RECVPacket)-buff_len),deq_buffer,buff_len);
				buff_len = 0;
			}
			st_index = 0;
			if(buff_len <=0) break;
		}
		lock_ringbuffer.unlock();
		cv_ringbuffer.notify_one(); // ringbuffer에서 하나이상 deq했기 때문에 is_full = false
		RECVPacket* t = (RECVPacket*)make_buff;
		if(strcmp(t->head.head,"Aa")!=0 && strcmp(t->tail.tail,"zZ")!=0)
		{
			cout << "error Packet!!" << endl;
		}
		else
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
				while(can_read)
				{
					char* buffer = new char[sizeof(RECVPacket)];
					int n = read(client,buffer,sizeof(RECVPacket));
					if(n <= 0)
					{
						if(n==-1 && errno == EAGAIN) can_read = false;
						else // 연결 해제
						{
							if(n==-1) perror("read");
							lock_guard<mutex> lock(user_mutex);
							auto findIter = m_users.find(client);
							if(findIter != m_users.end())
							{
								cout << findIter->second->get_name() << " disconnect" << endl;
								m_user_name.erase(findIter->second->get_name());
								m_users.erase(findIter);
								free(findIter->second);
							}
							epoll_ctl(m_epfd,EPOLL_CTL_DEL,client,&m_ev);
							shutdown(client,SHUT_WR);
							//close(client);
							can_read = false;
						}
					}
					else // read 성공
					{
						unique_lock<mutex> lock_ringbuffer(ringbuffer_mutex);
						cv_ringbuffer.wait(lock_ringbuffer
								,[this](){return !this->ringbuffer.is_full();}); // ringbuffer가 가득차면 기다린다
						ringbuffer.enqueue_buffer(buffer,n,client);
						free(buffer);
						cv_ringbuffer.notify_one();
					}
					usleep(2000);
				} // while(can_read)
			} // else
		}// for(int i=0;i<nfd;i++)
	} // while(1)
}


