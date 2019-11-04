// ServerDlg.h : header file
//

#if !defined(AFX_SERVERDLG_H__F9FC6DE6_63FE_11D4_BD47_00AA00BBF353__INCLUDED_)
#define AFX_SERVERDLG_H__F9FC6DE6_63FE_11D4_BD47_00AA00BBF353__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CServerDlg dialog

typedef struct _SOCKET_STREAM_FILE_INFO {

    TCHAR       szFileTitle[128];                   //�ļ��ı�����
    DWORD       dwFileAttributes;                   //�ļ�������
    FILETIME    ftCreationTime;                     //�ļ��Ĵ���ʱ��
    FILETIME    ftLastAccessTime;                   //�ļ���������ʱ��
    FILETIME    ftLastWriteTime;                    //�ļ�������޸�ʱ��
    DWORD       nFileSizeHigh;                      //�ļ���С�ĸ�λ˫��
    DWORD       nFileSizeLow;                       //�ļ���С�ĵ�λ˫��
    DWORD       dwReserved0;                        //������Ϊ0
    DWORD       dwReserved1;                        //������Ϊ0

} SOCKET_STREAM_FILE_INFO, * PSOCKET_STREAM_FILE_INFO;

class CServerDlg : public CDialog
{
// Construction
public:
	CServerDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CServerDlg)
	enum { IDD = IDD_SERVER_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CServerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CServerDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButtonListen();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SERVERDLG_H__F9FC6DE6_63FE_11D4_BD47_00AA00BBF353__INCLUDED_)
