#ifndef H_CLASS_PKT_QUE___
#define H_CLASS_PKT_QUE___

#pragma once

#include "windows.h"
#include <cassert>
#include "util.h"

class CPacketQueue;

type_packet * NewPacket();
void DeletePacket(type_packet* packet);
// TopÇÊÇËêÊÇÉNÉäÉA
void ClearQueue(type_queue* pqTop);
BOOL AddPacket(CPacketQueue *pQueue, ptype_session sess, BYTE* data, WORD size);
BOOL AddPacketTeamUser(CPacketQueue *pQueue,ptype_session ignore_sess, t_sessionInfo* sess_info, BYTE* data, WORD size);
BOOL AddPacketAllUser(CPacketQueue *pQueue,ptype_session ignore_sess, t_sessionInfo* sess_info, BYTE* data, WORD size);

void EnqueuePacket(type_queue* pq, type_packet* packet);

#define QUEUE_MUTEX	(L"Q_MUTEX")

class CPacketQueue{
public:
	CPacketQueue()
	{
		m_hMyEvent = NULL;
		m_nCount = 0;
		m_pQueue = (type_queue**)malloc(sizeof(type_queue**));
//		void *pMemBlock;
//		pMemBlock = VirtualAlloc(NULL,sizeof(ptype_queue),MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
//		assert(pMemBlock);
//		m_pQueue=reinterpret_cast<ptype_queue*>(pMemBlock);
		*m_pQueue = NULL;
	};

	virtual ~CPacketQueue()
	{
		ResetGrowEvent();
		CloseHandle(m_hMyEvent);
		m_nCount = 0;
		SafeFree(m_pQueue);
//		VirtualFree((void*)m_pQueue,0,MEM_RELEASE);
	}

	type_packet*	Dequeue();
	int EnqueueRaw(type_packet * packet);
	int Enqueue(type_queue * pq);
	void ClearQueue();

	inline int GetCount()
	{
		return m_nCount;
	};

	void DoGrowEvent()
	{
		::SetEvent(m_hMyEvent);
	}
	void ResetGrowEvent()
	{
		::ResetEvent(m_hMyEvent);
	}
	HANDLE CreateGrowEventHandle()
	{
		return m_hMyEvent = CreateEvent(NULL, TRUE, FALSE, QUEUE_MUTEX);
	}
	HANDLE OpenMyEvent()
	{
		return ::OpenEvent(EVENT_ALL_ACCESS, FALSE, QUEUE_MUTEX);
	}

private:
	type_queue**	m_pQueue;
	int				m_nCount;

	HANDLE			m_hMyEvent;

};

#endif