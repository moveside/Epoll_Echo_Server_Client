﻿
// ClientUIDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "ClientUI.h"
#include "ClientUIDlg.h"
#include "afxdialogex.h"
#include "ClientMenu.h"
#include "user.h"
#include "client.h"
#include "packet.h"
#include <thread>
#include <vector>
#include <string>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

User user;
User test_user[5];

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnChangeEdit1();
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	ON_EN_CHANGE(IDC_EDIT1, &CAboutDlg::OnEnChangeEdit1)
END_MESSAGE_MAP()


// CClientUIDlg 대화 상자



CClientUIDlg::CClientUIDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CLIENTUI_DIALOG, pParent)
	, input_ID(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CClientUIDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT1, input_ID);
	DDV_MaxChars(pDX, input_ID, 8);
}

BEGIN_MESSAGE_MAP(CClientUIDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CClientUIDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDCANCEL, &CClientUIDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CClientUIDlg 메시지 처리기

BOOL CClientUIDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.

	if (!user.start())
	{
		MessageBox(_T("서버 연결 실패"), _T("Connect"), MB_ICONINFORMATION);
		EndDialog(0);
	}
	// 테스트 유저
	/*
	for (int i = 0; i < 5; i++)
	{
		if (!test_user[i].start())
		{
			MessageBox(_T("서버 연결 실패"), _T("Connect"), MB_ICONINFORMATION);
			EndDialog(0);
		}
	}
	*/
	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CClientUIDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CClientUIDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CClientUIDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CClientUIDlg::OnBnClickedButton1()
{
	CEdit* p = (CEdit*)GetDlgItem(IDC_EDIT1);
	if (p->GetWindowTextLengthW() > 0)
	{
		CString cinput_ID;
		p->GetWindowTextW(cinput_ID);
		string sinput_ID = CT2CA(cinput_ID);
		user.send_data(LOGIN, sinput_ID);
		Packet* packet = user.recv_data();
		if (packet == nullptr)
		{
			MessageBox(_T("응답 실패"), _T("Login"), MB_ICONINFORMATION);
			user.disconnect();
		}
		if (packet->body.cmd == CMD_USER_LOGIN_RECV && packet->body.data[0]=='1')
		{
			//테스트 유저
			/*
			for (int i = 0; i < 5; i++)
			{
				string id_data = "testuser" + to_string(i);
				test_user[i].send_data(LOGIN, id_data);
				RECVPacket dummy_packet = test_user[i].recv_data();
			}
			*/
			MessageBox(_T("로그인 성공"), _T("Login"), MB_ICONINFORMATION);
			user.set_name(sinput_ID.c_str());
			EndDialog(0);
			ClientMenu menu;
			menu.DoModal();
		}
		else
		{
			MessageBox(_T("로그인 실패"), _T("Login"), MB_ICONINFORMATION);
		}
	}
	else
	{
		MessageBox(_T("아이디를 입력하세요"), _T("Login"), MB_ICONINFORMATION);
	}
}


void CClientUIDlg::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	user.disconnect();
	CDialogEx::OnCancel();
}




BOOL CClientUIDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_ESCAPE)
		{
			// ESC 키 이벤트에 대한 처리 추가
			return TRUE;
		}
		else if (pMsg->wParam == VK_RETURN)
		{
			OnBnClickedButton1();
			// Enter 키 이벤트에 대한 처리 추가
			return TRUE;
		}
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}


void CAboutDlg::OnEnChangeEdit1()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialogEx::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
}
