/*
 * EchoServer.cpp
 *
 *  Created on: Jun 30, 2023
 *      Author: yeob
 */

#include "epollserver.h"
#include "packet.h"

int main()
{
	Epollserver epollserver;
	epollserver.start();
	return 0;
}
