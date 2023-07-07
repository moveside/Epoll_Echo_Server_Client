#include "user.h"






User::User(char* name,int sockfd)
{
	strcpy(m_name,name);
	m_sockfd = sockfd;
}

User::~User()
{

}

char* User::get_name()
{
	return m_name;
}

