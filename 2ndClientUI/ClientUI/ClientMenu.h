#pragma once
#include "afxdialogex.h"
#include "user.h"
#include "packet.h"
#include "client.h"

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
public:
	CStatic user_name;
	CStatic Notice;
	CListBox Log_Data;
	CEdit Send_Data;

	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedCancel();

	void recv_data();
	afx_msg void OnEnChangeEdit1();
	CString Input_Data;
	afx_msg void OnStnClickedStaticName();
};
