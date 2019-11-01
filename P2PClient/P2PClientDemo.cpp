//////////////////////////////////////////////////////////
// P2PClient.cpp�ļ�
#include <winsock2.h>
#include <iostream>
using namespace std;

#include "p2pclient.h"
class CMyP2P : public CP2PClient
{
public:
	void OnRecv(char* pszUserName, char* pszData, int nDataLen)
	{
		pszData[nDataLen] = '\0';
		printf(" Recv a Message from %s: %s \n", pszUserName, pszData);
	}
};

int main()
{
	printf(" main() ProcessId = %d \n", GetCurrentProcessId());
	printf(" main() ThreadId = %d \n", GetCurrentThreadId());

	CMyP2P client;
	if (!client.Init(0))
	{
		printf(" CP2PClient::Init() failed \n");
		return 1;
	}

	// ��ȡ������IP��ַ���û���
	char szServerIp[20] = { 0 };
	char szUserName[MAX_USER_NAME] = { 0 };
	printf(" Please input server ip: ");
	cin >> szServerIp;
	printf(" Please input your name: ");
	cin >> szUserName;
	// ��½������
	if (!client.Login(szUserName, szServerIp))
	{
		printf(" CP2PClient::Login() failed \n");
		return 2;
	}
	// ��һ�ε�½�����ȸ����û��б�
	client.GetUserList();
	// ����ǰ״̬�ͱ�������÷�������û�
	printf(" %s has successfully logined server \n", szUserName);
	printf("\n Commands are: \"getu\", \"send\", \"exit\" \n");
	// ѭ�������û�����
	char szCommandLine[256] = { 0 };
	while (TRUE)
	{
		cin >> szCommandLine;
		if (strlen(szCommandLine) < 4)
		{
			continue;
		}
		// ����������
		char szCommand[10] = { 0 };
		strncpy(szCommand, szCommandLine, 4);
		szCommand[4] = '\0';
		if (_stricmp(szCommand, "getu") == 0)
		{
			// ��ȡ�û��б�
			if (client.GetUserList())
			{
				printf(" Have %d users logined server: \n",
					client.m_PeerList.m_nCurrentSize);
				for (int i = 0; i < client.m_PeerList.m_nCurrentSize; i++)
				{
					PEER_INFO* pInfo = &client.m_PeerList.m_pPeer[i];
					printf(" Username: %s(%s:%ld) \n", pInfo->szUserName,
						::inet_ntoa(*((in_addr*)&pInfo->addr[pInfo->AddrNum - 1].dwIp)),
						pInfo->addr[pInfo->AddrNum - 1].nPort);
				}
			}
			else
			{
				printf(" Get User List Failure !\n");
			}
		}

		else if (_stricmp(szCommand, "send") == 0)
		{
			// �������Է��û���
			char szPeer[MAX_USER_NAME] = { 0 };
			int i = 0;
			for (i = 5;; i++)
			{
				if (szCommandLine[i] != ' ')
				{
					szPeer[i - 5] = szCommandLine[i];
				}
				else
				{
					szPeer[i - 5] = '\0';
					break;
				}
			}

			// ������Ҫ���͵���Ϣ
			char szMsg[56] = { 0 };
			strcpy(szMsg, &szCommandLine[i + 1]);

			// ������Ϣ
			if (client.SendText(szPeer, szMsg, strlen(szMsg)))
			{
				printf(" Send OK! \n");
			}
			else
			{
				printf(" Send Failure! \n");
			}

		}
		else if (_stricmp(szCommand, "exit") == 0)
		{
			break;
		}
	}
	return 0;
}