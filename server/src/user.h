/*
 * user.h
 *
 *  Created on: Jul 4, 2023
 *      Author: yeob
 */

#ifndef USER_H_
#define USER_H_

#include <vector>
#include <string>

#include <string.h>

using namespace std;

class User
{
private:
	char m_name[16];
	int m_sockfd;

public:

	User(char* name,int sockfd);
	~User();

	char* get_name();
};







#endif /* USER_H_ */
