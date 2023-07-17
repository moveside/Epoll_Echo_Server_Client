#pragma once
#include "afxdialogex.h"
#include "user.h"
#include "packet.h"
#include "client.h"
#include <thread>
#include <vector>
#include <queue>


// ClientMenu 대화 상자

class ClientMenu : public CDialogEx
{
	DECLARE_DYNAMIC(ClientMenu)

public:
	ClientMenu(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~ClientMenu();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
private:
	vector<thread> threads;
	int m_request;
	queue <pair<CMD, string>> m_requestpool;
	CMD m_cmd;
	string m_data;
	thread m_send_thread;
	thread m_recv_thread;
public:
	CStatic user_name;
	CStatic Notice;
	CListBox Log_Data;
	CEdit Send_Data;

	void send_data();
	void recv_data();
	void set_threads();

	void send_dummydata();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedCancel();


	CString Input_Data;
	afx_msg void OnBnClickedButton4();
};
