#pragma once
#include <string>
#include <functional>
#include "../common/mongoose.h"
using namespace std;

// �˴�������function�࣬typedef�ٺ��溯��ָ�븳ֵ��Ч
using ReqCallback = function<void(string)>;

class HttpClient
{
public:
	HttpClient() {}
	~HttpClient() {}

	static void SendReq(const string& url, ReqCallback req_callback);
	static void OnHttpEvent(mg_connection* connection,
		int event_type, void* event_data);
	static int s_exit_flag;
	static ReqCallback s_req_callback;
};