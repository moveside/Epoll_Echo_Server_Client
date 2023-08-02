/*
 * ring_buffer.h
 *
 *  Created on: Jul 13, 2023
 *      Author: yeob
 */

#ifndef RINGBUFFER_H_
#define RINGBUFFER_H_


#include "packet.h"


#include <iostream>
#include <string.h>
#include <vector>
#include <tuple>

using namespace std;


class RingBuffer
{
private:
	tuple<char[sizeof(Packet)],int,int> ringbuffer[500];
	int front;
	int rear;

public:
	RingBuffer();



	bool is_empty();
	bool is_full();

	void enqueue_buffer(char* data,int size,int client);
	char* dequeue_buffer(int& start,int size);

	int find_str(char* data);
	pair<int,int> find_head();
	COMBODY* find_body(int start,int size);

	COMBODY* combine_Packet(int &client);
	int get_size();
	int get_client();
};



#endif /* RINGBUFFER_H_ */
