///////////////////////////////////////
// P2PServer.cpp�ļ�
#include <stdio.h>
#include "../common/InitSock.h"
#include "../comm.h"

CInitSock theSock; //WSAStartup/WSACleanup
CPeerList g_PeerList; // �ͻ��б�
CRITICAL_SECTION g_PeerListLock; // ͬ���Կͻ��б�ķ���
SOCKET g_s; // UDP�׽���

DWORD WINAPI IOThreadProc(LPVOID lpParam)
{
	char buff[MAX_PACKET_SIZE] = { 0 };
	CP2PMessage* pMsg = (CP2PMessage*)buff;
	sockaddr_in remoteAddr = { 0 };
	int nRecv, nAddrLen = sizeof(remoteAddr);
	while (TRUE)
	{
		nRecv = ::recvfrom(g_s, buff, MAX_PACKET_SIZE, 0,
			(sockaddr*)&remoteAddr, &nAddrLen);
		if (nRecv == SOCKET_ERROR)
		{
			printf(" recvfrom() failed \n");
			continue;
		}
		if (nRecv < sizeof(CP2PMessage))
		{
			continue;
		}

		// ��ֹ�û����ʹ�����û���
		pMsg->peer.szUserName[MAX_USER_NAME] = '\0';
		switch (pMsg->nMessageType)
		{
		case USER_LOG_IN: // ���û���¼			
		{
			// ��¼�û��Ļʱ�䣬�����û��Ĺ����ն���Ϣ
			pMsg->peer.dwLastActiveTime = ::GetTickCount64();
			pMsg->peer.addr[pMsg->peer.AddrNum].dwIp = remoteAddr.sin_addr.S_un.S_addr;
			pMsg->peer.addr[pMsg->peer.AddrNum].nPort = ntohs(remoteAddr.sin_port);
			pMsg->peer.AddrNum++;

			// ���û���Ϣ���浽�û��б���
			::EnterCriticalSection(&g_PeerListLock);
			BOOL bOK = g_PeerList.AddAPeer(&pMsg->peer);
			::LeaveCriticalSection(&g_PeerListLock);
			if (bOK)
			{
				// ����ȷ����Ϣ�����û��Ĺ�����ַ���ݹ�ȥ
				pMsg->nMessageType = USER_LOG_ACK;
				::sendto(g_s, (char*)pMsg, sizeof(CP2PMessage), 0,
					(sockaddr*)&remoteAddr, sizeof(remoteAddr));
				printf(" has a user login : %s (%s:%ld) \n",
					pMsg->peer.szUserName, ::inet_ntoa(remoteAddr.sin_addr),
					ntohs(remoteAddr.sin_port));
			}
		}
		break;

		case USER_LOG_OUT: // ���û��ǳ�
		{
			::EnterCriticalSection(&g_PeerListLock);
			g_PeerList.DeleteAPeer(pMsg->peer.szUserName);
			::LeaveCriticalSection(&g_PeerListLock);
			printf(" has a user logout : %s (%s:%ld) \n",
				pMsg->peer.szUserName, ::inet_ntoa(remoteAddr.sin_addr),
				ntohs(remoteAddr.sin_port));
		}
		break;

		case GET_USER_LIST:	// ���û��������û��б�
		{
			printf(" sending user list information to %s (%s:%ld)... \n",
				pMsg->peer.szUserName, ::inet_ntoa(remoteAddr.sin_addr),
				ntohs(remoteAddr.sin_port));
			CP2PMessage peerMsg;
			peerMsg.nMessageType = GET_USER_LIST;
			for (int i = 0; i < g_PeerList.m_nCurrentSize; i++)
			{
				memcpy(&peerMsg.peer, &g_PeerList.m_pPeer[i], sizeof(PEER_INFO));
				::sendto(g_s, (char*)&peerMsg, sizeof(CP2PMessage), 0,
					(sockaddr*)&remoteAddr, sizeof(remoteAddr));
			}
			// ���ͽ������
			peerMsg.nMessageType = USER_LIST_CMP; //complete
			::sendto(g_s, (char*)&peerMsg, sizeof(CP2PMessage), 0,
				(sockaddr*)&remoteAddr, sizeof(remoteAddr));
		}
		break;

		case P2P_CONNECT: // ���û���������һ���û��������ʹ���Ϣ
		{
			char* pszUser = (char*)(pMsg + 1);
			printf(" %s wants to connect to %s \n", pMsg->peer.szUserName, pszUser);
			::EnterCriticalSection(&g_PeerListLock);
			PEER_INFO* pInfo = g_PeerList.GetAPeer(pszUser);
			::LeaveCriticalSection(&g_PeerListLock);
			if (pInfo != NULL)
			{
				remoteAddr.sin_addr.S_un.S_addr = pInfo->addr[pInfo->AddrNum - 1].dwIp;
				remoteAddr.sin_port = htons(pInfo->addr[pInfo->AddrNum - 1].nPort);
				::sendto(g_s, (char*)pMsg, sizeof(CP2PMessage), 0,
					 (sockaddr*)&remoteAddr, sizeof(remoteAddr));
			}
		}
		break;

		case USER_ACTIVE_QUERY_ACK:	// �û��ԡ�ѯ�ʡ���Ϣ��Ӧ��	
		{
			printf(" recv active ack message from %s (%s:%ld) \n",
				pMsg->peer.szUserName, ::inet_ntoa(remoteAddr.sin_addr),
				ntohs(remoteAddr.sin_port));
			::EnterCriticalSection(&g_PeerListLock);
			PEER_INFO* pInfo = g_PeerList.GetAPeer(pMsg->peer.szUserName);
			if (pInfo != NULL)
			{
				pInfo->dwLastActiveTime = ::GetTickCount64();
			}
			::LeaveCriticalSection(&g_PeerListLock);
		}
		break;
		}
	}
	return 0;
}

int main()
{
	// �����׽��֣��󶨵����ض˿�
	g_s = ::WSASocket(AF_INET, SOCK_DGRAM, IPPROTO_UDP,
		NULL, 0, WSA_FLAG_OVERLAPPED);
	if (g_s == 0 || g_s == INVALID_SOCKET)
	{
		printf(" WSASocket() failed %d \n", ::WSAGetLastError());
		return 1;
	}
	sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(SERVER_PORT);
	sin.sin_addr.S_un.S_addr = INADDR_ANY;
	if (::bind(g_s, (LPSOCKADDR)&sin, sizeof(sin)) == SOCKET_ERROR)
	{
		printf(" bind() failed %d \n", ::WSAGetLastError());
		return 2;
	}

	///////////////////////////////////////////////////////
	// ������δ���������ʾ�������󶨵��ն�
	char szHost[256];
	::gethostname(szHost, 256);
	hostent* pHost = ::gethostbyname(szHost);
	in_addr addr;
	for (int i = 0; ; i++)
	{
		char* p = pHost->h_addr_list[i];
		if (p == NULL)
			break;

		memcpy(&addr.S_un.S_addr, p, pHost->h_length);
		printf(" bind to local address -> %s:%ld \n",
			::inet_ntoa(addr), SERVER_PORT);
	}
	// ��������	
	printf(" P2P Server starting... \n\n");
	::InitializeCriticalSection(&g_PeerListLock);
	HANDLE hThread = ::CreateThread(NULL, 0, IOThreadProc, NULL, 0, NULL);
	if (hThread == 0 || hThread == INVALID_HANDLE_VALUE)
	{
		printf(" CreateThread() failed %d \n", ::GetLastError());
		return 3;
	}
	// ��ʱ��ͻ������͡�ѯ�ʡ���Ϣ��ɾ������Ӧ���û�
	while (TRUE)
	{
		int nRet = ::WaitForSingleObject(hThread, 15 * 1000);
		if (nRet == WAIT_TIMEOUT) //�ȴ�15��
		{
			CP2PMessage queryMsg;
			queryMsg.nMessageType = USER_ACTIVE_QUERY;
			ULONGLONG dwTick = ::GetTickCount64();
			for (int i = 0; i < g_PeerList.m_nCurrentSize; i++)
			{
				if (dwTick - g_PeerList.m_pPeer[i].dwLastActiveTime
					>= 2 * 15 * 1000 + 600)
				{ //����30�룬ɾ���û�
					printf(" delete a non-active user: %s \n",
						g_PeerList.m_pPeer[i].szUserName);
					::EnterCriticalSection(&g_PeerListLock);
					g_PeerList.DeleteAPeer(g_PeerList.m_pPeer[i].szUserName);
					::LeaveCriticalSection(&g_PeerListLock);
					// ��Ϊɾ�˵�ǰ���������û�������iֵ�Ͳ�Ӧ�ü�1��
					i--;
				}
				else
				{
					// ע�⣬��ַ�б��е����һ����ַ�ǿͻ��Ĺ�����ַ��
					//		ѯ����ϢӦ�÷��������ַ
					sockaddr_in peerAddr = { 0 };
					peerAddr.sin_family = AF_INET;
					peerAddr.sin_addr.S_un.S_addr = g_PeerList.m_pPeer[i]						
						.addr[g_PeerList.m_pPeer[i].AddrNum - 1].dwIp;
					peerAddr.sin_port = htons(g_PeerList.m_pPeer[i]
						.addr[g_PeerList.m_pPeer[i].AddrNum - 1].nPort);
					::sendto(g_s, (char*)&queryMsg, sizeof(queryMsg), 0,
						(sockaddr*)&peerAddr, sizeof(peerAddr));
				}
			}
		}
		else
		{
			//WAIT_OBJECT_0	The state of the specified object is signaled.
			//WAIT_FAILED indicates failure. 
			break;
		}
	}

	printf(" P2P Server shutdown. \n");
	::DeleteCriticalSection(&g_PeerListLock);
	::CloseHandle(hThread);
	::closesocket(g_s);
	return 0;
}