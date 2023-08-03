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
	if((rear+1)%500==front) return true;
	return false;
}
int RingBuffer::get_size()
{
	return get<1>(ringbuffer[rear]);
}

int RingBuffer::get_client()
{
	return get<2>(ringbuffer[rear]);
}
void RingBuffer::enqueue_buffer(char* data,int size,int client)
{
	memcpy(get<0>(ringbuffer[front]),data,size);
	get<1>(ringbuffer[front]) = size;
	get<2>(ringbuffer[front]) = client;
	front++;
	front %= 500;
}
char* RingBuffer::dequeue_buffer(int& start,int size)
{
	char* result = new char[size+1];
	if(start >= get_size()) start %= get_size();
	int index = start;
	int deq_size = size;
	while(deq_size > 0)
	{
		auto el = ringbuffer[rear];
		int max_deq = get_size() - index;
		if(deq_size > max_deq)
		{
			memcpy(result+(size - deq_size),get<0>(el)+index,max_deq);
			deq_size -= max_deq;
			index = 0;
			rear++;
			rear %=500;
		}
		else
		{
			memcpy(result+(size - deq_size),get<0>(el)+index,deq_size+1);
			if(deq_size == max_deq)
			{
				rear++;
				rear %=500;
			}
			deq_size = 0;
		}
	}
	return result;
}

int RingBuffer::find_str(char* data)
{
	auto el = ringbuffer[rear];
	for(int i=0;i<=get<1>(el)-strlen(data);i++)
	{
		bool is_find = true;
		for(int j=0;j<strlen(data);j++)
		{
			if(get<0>(el)[i+j]!=data[j])
			{
				is_find = false;
				break;
			}
		}
		if(is_find) return i;
	}
	return -1;
}

pair<int,int> RingBuffer::find_head()
{
	auto el = ringbuffer[rear];
	char* data = "Aa";
	for(int i=0;i<=get<1>(el)-sizeof(data);i++)
	{
		bool is_find = true;
		for(int j=0;j<strlen(data);j++)
		{
			if(get<0>(el)[i+j]!=data[j])
			{
				is_find = false;
				break;
			}
		}
		if(is_find)
		{
			return make_pair(i,get<0>(el)[i+3]);
		}

	}
	return make_pair(-1,-1);
}

COMBODY* RingBuffer::find_body(int start,int size)
{
	// cmd 추출
	COMBODY* body = new COMBODY;
	int index = start;
	auto el = ringbuffer[rear];
	body->cmd = static_cast<packetCommand>(get<0>(el)[index]);
	// data 추출
	index += sizeof(packetCommand);
	int deq_size = size;
	char* bodyData = dequeue_buffer(index,deq_size);
	body->data = bodyData;
	return body;
}


COMBODY* RingBuffer::combine_Packet(int &client)
{
	int index = -1;
	while(1)
	{
		if(is_empty()) break;
		if((index = find_str("Aa")) >= 0 )
		{
			client = get_client();
			break;
		}
		else
		{
			int tmp_index = 0;
			dequeue_buffer(tmp_index,get_size());
		}
	}
	if(index == -1) return nullptr;
	HEAD* head = (HEAD*)dequeue_buffer(index,sizeof(HEAD));
	index += sizeof(HEAD);
	COMBODY* body = find_body(index,head->dataSize);
	index += sizeof(BODY);
	TAIL* tail = (TAIL*)dequeue_buffer(index,sizeof(TAIL));
	if(strcmp(head->head,"Aa") == 0 && strcmp(tail->tail,"zZ")==0)
	{
		delete head;
		delete tail;
		return body;
	}
	else
	{
		delete head;
		delete tail;
		delete body;
		return nullptr;
	}
}



