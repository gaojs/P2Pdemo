////////////////////////////////////
// comm.h�ļ�
#ifndef __COMM_H__
#define __COMM_H__

#define MAX_USER_NAME 15
#define MAX_TRY_NUMBER 5
#define MAX_ADDR_NUMBER 5
#define MAX_PACKET_SIZE 1024
#define SERVER_PORT 4567

// һ���ն���Ϣ
struct ADDR_INFO
{
	DWORD dwIp;
	u_short nPort;
};

// һ���ڵ���Ϣ
struct PEER_INFO
{
	char szUserName[MAX_USER_NAME + 1];	// �û���
	ADDR_INFO addr[MAX_ADDR_NUMBER + 1]; // �ɽڵ��˽���ն˺͹����ն���ɵ�����
	u_char AddrNum;					// addr����Ԫ������
	ADDR_INFO p2pAddr;				// P2Pͨ��ʱӦʹ�õĵ�ַ���ͻ���ʹ�ã�
	ULONGLONG dwLastActiveTime;		// ��¼���û��Ļʱ�䣨������ʹ�ã�
};

// ͨ����Ϣ��ʽ
struct CP2PMessage
{
	int nMessageType;	// ��Ϣ����
	PEER_INFO peer;		// �ڵ���Ϣ
};

// �û�ֱ���������֮�䷢�͵���Ϣ// ��Ϣ����
#define USER_LOG_IN	 101	// �û���¼������
#define USER_LOG_OUT 102	// �û��ǳ�������
#define USER_LOG_ACK 103

#define GET_USER_LIST 104	// �����û��б�
#define USER_LIST_CMP 105	// �б������

#define USER_ACTIVE_QUERY	106			// ������ѯ��һ���û��Ƿ���Ȼ���
#define USER_ACTIVE_QUERY_ACK	107		// ������ѯ��Ӧ��

// ͨ����������ת���û����û�֮�䷢�͵���Ϣ
#define P2P_CONNECT	108			// ������һ���û���������
#define P2P_CONNECT_ACK	109		// ����Ӧ�𣬴���Ϣ���ڴ�

// �û�ֱ�����û�֮�䷢�͵���Ϣ
#define P2P_MESSAGE		110		// ������Ϣ
#define P2P_MESSAGE_ACK	111		// �յ���Ϣ��Ӧ��

class CPeerList
{
public:
	CPeerList();
	~CPeerList();

	// ���б������һ���ڵ�
	BOOL AddAPeer(PEER_INFO* pPeer);
	// ����ָ���û�����Ӧ�Ľڵ�
	PEER_INFO* GetAPeer(char* pszUserName);
	// ���б���ɾ��һ���ڵ�
	void DeleteAPeer(char* pszUserName);
	// ɾ�����нڵ�
	void DeleteAllPeers();

	// ��ͷָ��ͱ�Ĵ�С
	PEER_INFO* m_pPeer;
	int m_nCurrentSize;

protected:
	int m_nTotalSize;
};

#endif // __COMM_H__
