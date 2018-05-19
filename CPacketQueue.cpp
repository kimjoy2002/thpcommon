#include "CPacketQueue.h"
#include "common.h"

#define VIRTUAL_ALLOC	0

type_packet * NewPacket()
{
#if VIRTUAL_ALLOC
	void *pMemBlock = NULL;
	type_packet * temp;
	pMemBlock = VirtualAlloc(NULL,sizeof(type_packet),MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	assert(pMemBlock);
	temp=reinterpret_cast<ptype_packet>(pMemBlock);
	return temp;
#else
	type_packet* pMemBlock = (type_packet*)malloc( sizeof(type_packet));
	assert(pMemBlock);
//	temp=reinterpret_cast<ptype_packet>(pMemBlock);
	return pMemBlock;
#endif
}

void DeletePacket(type_packet * packet)
{
	if (packet)
	{
#if VIRTUAL_ALLOC
		VirtualFree((void*)packet,0,MEM_RELEASE);
		packet = NULL;
#else
//		void* p = (void*)packet;
		free(packet);
		packet = NULL;
#endif
	}
}

//> �Z�b�V�����֑���p�P�b�g�쐬
BOOL AddPacket(CPacketQueue *pQueue, ptype_session sess, BYTE* data, WORD size)
{
	if (!sess || !data || size > MAX_PACKET_SIZE)
		return FALSE;

	ptype_packet ppkt = NewPacket();
	if (!ppkt) return FALSE;

	ppkt->cli_sock = sess->sock;
	ppkt->session = sess;
	ppkt->size = size;

	ZeroMemory(ppkt->data,sizeof(char)*MAX_PACKET_SIZE);
	CopyMemory(ppkt->data,data,size);

	pQueue->EnqueueRaw(ppkt);
	return TRUE;
};
//< �Z�b�V�����֑���p�P�b�g�쐬

BOOL AddPacketTeamUser(CPacketQueue *pQueue,int team_no, t_sessionInfo* sess_info, BYTE* data, WORD size)
{
	if (!sess_info || !data || size > MAX_PACKET_SIZE)
		return FALSE;
	BOOL ret = FALSE;
	
	//> �ڑ��ς݃��[�U�Ƃ̏���
	for(int i=0;i<g_nMaxLoginNum;i++)
	{
		ptype_session sess = &(sess_info[i].s);
		// �F�؍ς݂�
		if (sess->connect_state != CONN_STATE_AUTHED)	continue;
		if (sess->team_no != GALLERY_TEAM_NO			// �ϐ�
		&& sess->team_no != team_no)	continue;			// ��΂��Z�b�V������
		ret |=AddPacket(pQueue, sess, data, size);						// �L���[�ǉ�
		if (!ret)	break;
	}
	return TRUE;
}

//> �Z�b�V�����֑���p�P�b�g�쐬
BOOL AddPacketAllUser(CPacketQueue *pQueue,ptype_session ignore_sess, t_sessionInfo* sess_info, BYTE* data, WORD size)
{
	if (!sess_info || !data || size > MAX_PACKET_SIZE)
		return FALSE;
	BOOL ret = FALSE;
	//> �ڑ��ς݃��[�U�Ƃ̏���
	for(int i=0;i<g_nMaxLoginNum;i++)
	{
		ptype_session sess = &(sess_info[i].s);
		// �F�؍ς݂�
		if (sess->connect_state != CONN_STATE_AUTHED)	continue;
		if ( sess == ignore_sess)	continue;						// ��΂��Z�b�V������
		ret |=AddPacket(pQueue, sess, data, size);						// �L���[�ǉ�
		if (!ret)	break;
	}
	return TRUE;
};
//< �Z�b�V�����֑���p�P�b�g�쐬

void EnqueuePacket(type_queue* pq, type_packet* packet)
{
	type_queue* temp = pq;
	// �ŏI�L���[�܂ňړ�
	for (; temp->next; temp=temp->next);

	temp->next = (type_queue*)malloc(sizeof(type_queue));
//	void *pMemBlock;
//	pMemBlock = VirtualAlloc(NULL,sizeof(type_queue),MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
//	assert(pMemBlock);
//	temp->next=reinterpret_cast<ptype_queue>(pMemBlock);
	if (!temp->next)
	{
		MessageBox(NULL, L"packet�쐬�������s��", L"error", MB_OK);
		return;
	}
	temp = temp->next;
	temp->next = NULL;
	temp->packet = packet;
}

// Top������N���A
void ClearQueue(type_queue* pq)
{
	type_queue* temp = pq;
	type_queue* next = NULL;
	while (temp)
	{
		DeletePacket(temp->packet);
		next = temp->next;
		free(temp);
//		VirtualFree((void*)temp,0,MEM_RELEASE);
		temp = next;
	}
}

//////////////////////////////////////////////CLASS
//> �p�P�b�gDequeue
type_packet * CPacketQueue::Dequeue()
{
	type_queue *  temp;
	type_packet * packet;

	if (!m_pQueue)
		return NULL;
	if (!*m_pQueue)
		return NULL;
	temp = *m_pQueue;
	*m_pQueue = (*m_pQueue)->next;
	
	packet = temp->packet;
	SafeFree(temp);
//	VirtualFree((void*)temp,0,MEM_RELEASE);

	if (!packet)
		return NULL;
	m_nCount--;
	return packet;
}
//> �p�P�b�gDequeue

//> �p�P�b�gEnqueue
int CPacketQueue::EnqueueRaw(type_packet * packet)
{
	if (!m_pQueue)	return 0;
	if (!packet)		return 0;
	type_queue * temp = *m_pQueue;

	if (!temp)
	{
//		void *pMemBlock;
//		pMemBlock = VirtualAlloc(NULL,sizeof(type_queue),MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
//		assert(pMemBlock);
//		temp=reinterpret_cast<ptype_queue>(pMemBlock);
//		if (!(temp))
		if (!(temp = (type_queue*)malloc(sizeof(type_queue))))
		{
			MessageBox(NULL, L"packet�쐬�������s��", L"error", MB_OK);
			return 0;
		}
		temp->next = NULL;
		*m_pQueue = temp;
	}
	else
	{
		// �ŏI�L���[�܂ňړ�
		for (; temp->next; temp=temp->next);

		temp->next = (type_queue*)malloc(sizeof(type_queue));
//		void *pMemBlock;
//		pMemBlock = VirtualAlloc(NULL,sizeof(type_queue),MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
//		assert(pMemBlock);
//		temp->next=reinterpret_cast<ptype_queue>(pMemBlock);
		temp->next->next = NULL;
		temp = temp->next;
	}
	temp->packet = packet;

	m_nCount++;
	// �L���[���������ʒm
	DoGrowEvent();

	return m_nCount;
}
//> �p�P�b�gEnqueue

int CPacketQueue::Enqueue(type_queue * pq)
{
	int ret =0;
	if (!m_pQueue)		return 0;
	if (!pq)			return 0;
	type_queue * temp = *m_pQueue;

	if (!temp)
	{
		temp = pq;
		*m_pQueue = temp;
	}
	else
	{
		// �ŏI�L���[�܂ňړ�
		for (; temp->next; temp=temp->next);
		temp->next = pq;
	}
	
	for (type_queue * pcount=pq; pcount; pcount=pcount->next) ret++;
	m_nCount += ret;
	// �L���[���������ʒm
	DoGrowEvent();
	return m_nCount;
}

void CPacketQueue::ClearQueue()
{
	type_queue *  temp = *m_pQueue;
	type_queue *  next=NULL;
	while (temp)
	{
		DeletePacket(temp->packet);
		next = temp->next;
		SafeFree(temp);
//		VirtualFree((void*)temp,0,MEM_RELEASE);
		temp = next;
	}
//	m_pQueue = NULL;
	m_nCount = 0;
}
