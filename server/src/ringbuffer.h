/*
 * ring_buffer.h
 *
 *  Created on: Jul 13, 2023
 *      Author: yeob
 */

#ifndef RINGBUFFER_H_
#define RINGBUFFER_H_

#include <iostream>
#include "packet.h"

#include <string.h>
#include <vector>
#include <tuple>

using namespace std;


class RingBuffer
{
private:
	tuple<char[sizeof(RECVPacket)],int,int> ringbuffer[500];
	int front;
	int rear;

public:
	RingBuffer();



	bool is_empty();
	bool is_full();

	void enqueue_buffer(char* data,int n,int client);
	char* dequeue_buffer(int start,int end);

	int find_str(char* data);

	int get_size();
	int get_client();
};



#endif /* RINGBUFFER_H_ */
