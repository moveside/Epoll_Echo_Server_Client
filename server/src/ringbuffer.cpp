/*
 * ringbuffer.cpp
 *
 *  Created on: Jul 13, 2023
 *      Author: yeob
 */


#include "ringbuffer.h"





RingBuffer::RingBuffer()
{
	front = 0;
	rear = 0;
}

bool RingBuffer::is_empty()
{
	if(front==rear) return true;
	return false;
}
bool RingBuffer::is_full()
{
	if(rear==(front+1)%500) return true;
	return false;
}



void RingBuffer::enqueue_buffer(char* data,int n,int client)
{
	memcpy(get<0>(ringbuffer[front]),data,n);
	get<1>(ringbuffer[front]) = n;
	get<2>(ringbuffer[front]) = client;
	front++;
	front %= 500;
}
char* RingBuffer::dequeue_buffer(int start,int end)
{
	tuple<char[60],int,int> el = ringbuffer[rear];
	rear++;
	rear %=500;
	char* result = new char[60];
	memcpy(result,get<0>(el)+start,end-start+1);
	return result;
}

int RingBuffer::find_str(char* data)
{
	tuple<char[60],int,int> el = ringbuffer[rear];
	for(int i=0;i<=	get<1>(el)-strlen(data);i++)
	{
		bool is_find = true;
		for(int j=0;j<strlen(data);j++)
		{
			if(	get<0>(el)[i+j]!=data[j])
			{
				is_find = false;
				break;
			}
		}
		if(is_find) return i;
	}
	return -1;
}

int RingBuffer::get_size()
{
	return get<1>(ringbuffer[rear]);
}

int RingBuffer::get_client()
{
	return get<2>(ringbuffer[rear]);
}
