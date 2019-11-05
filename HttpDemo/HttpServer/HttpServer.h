#pragma once
#include <string>
#include <functional>
#include <unordered_map>
#include "../common/mongoose.h"
using namespace std;

// ����http����callback
typedef void OnRspCallback(mg_connection* c, string);
// ����http����handler
using ReqHandler = function<bool(string, string,
	mg_connection * c, OnRspCallback)>;

class HttpServer
{
public:
	HttpServer() {}
	~HttpServer() {}
	void Init(const string& port); // ��ʼ������
	bool Start(); // ����httpserver
	bool Close(); // �ر�
	// ע���¼�������
	void AddHandler(const string& url, ReqHandler req_handler);
	void RemoveHandler(const string& url); // �Ƴ�ʱ�䴦����
	static string s_web_dir; // ��ҳ��Ŀ¼ 
	static mg_serve_http_opts s_server_option; // web������ѡ��
	// �ص�����ӳ���
	static unordered_map<string, ReqHandler> s_handler_map;

private:
	// ��̬�¼���Ӧ����
	static void OnHttpEvent(mg_connection* connection,
		int event_type, void* event_data);
	static void HandleEvent(mg_connection* connection,
		http_message* http_req);
	static void SendRsp(mg_connection* connection,
		string rsp);

	string m_port;    // �˿�
	mg_mgr m_mgr;          // ���ӹ�����
};