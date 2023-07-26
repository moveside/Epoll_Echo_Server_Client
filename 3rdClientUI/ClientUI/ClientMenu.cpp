// ClientMenu.cpp: 구현 파일
//

#include "pch.h"
#include "ClientUI.h"
#include "afxdialogex.h"
#include "ClientMenu.h"
#include <string>
#include <condition_variable>
#include <time.h>
// ClientMenu 대화 상자

extern User user;

IMPLEMENT_DYNAMIC(ClientMenu, CDialogEx)

ClientMenu::ClientMenu(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG1, pParent)
	, Input_Data(_T(""))
{
	set_threads();
}

ClientMenu::~ClientMenu()
{
}

void ClientMenu::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_STATIC_NAME, user_name);
	CString cUser_Name = CA2T(user.get_name());
	user_name.SetWindowTextW((LPCTSTR)cUser_Name);

	DDX_Control(pDX, IDC_STATIC_NOTICE, Notice);
	DDX_Control(pDX, IDC_LIST1, Log_Data);

	DDX_Text(pDX, IDC_EDIT1, Input_Data);
	DDV_MaxChars(pDX, Input_Data, 29);


	DDX_Control(pDX, IDC_LIST2, users_name);
}


BEGIN_MESSAGE_MAP(ClientMenu, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &ClientMenu::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &ClientMenu::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &ClientMenu::OnBnClickedButton3)
	ON_BN_CLICKED(IDCANCEL, &ClientMenu::OnBnClickedCancel)

	
	ON_BN_CLICKED(IDC_BUTTON4, &ClientMenu::OnBnClickedButton4)
END_MESSAGE_MAP()


// ClientMenu 메시지 처리기


void ClientMenu::set_threads()
{
	thread recv_thread(&ClientMenu::recv_data,this);
	recv_thread.detach();
}


void ClientMenu::OnBnClickedButton1() // Send Button
{
	CEdit* p = (CEdit*)GetDlgItem(IDC_EDIT1);
	if (p->GetWindowTextLengthW() > 0)
	{
		CString data;
		p->GetWindowTextW(data);
		string sdata = CT2CA(data);
		user.send_data(SEND, sdata);
		p->SetWindowTextW(L"");
	}
	else
	{
		Notice.SetWindowText(L"Input Send Data");
	}
}


void ClientMenu::OnBnClickedButton2() // Log Button
{
	user.send_data(LOG, "");
}

void ClientMenu::OnBnClickedButton3() // Delete Button
{
	user.send_data(DEL, "");
}


void ClientMenu::OnBnClickedCancel() // Exit Button
{
	user.disconnect();
	CDialogEx::OnCancel();
}

void ClientMenu::send_data()
{
	while (1)
	{
		if (!m_requestpool.empty())
		{
			auto el = m_requestpool.front();
			m_requestpool.pop();
			user.send_data(el.first, el.second);
		}
	}
}

void ClientMenu::recv_data()
{
	while (1)
	{
		RECVPacket packet = user.recv_data();
		switch (packet.body.cmd)
		{
		case CMD_USER_DATA_RECV:
		{
			CString recv_data(packet.body.data);
			CString notice_text = recv_data + (CString)" 보냄";
			Notice.SetWindowText((notice_text));
			break;
		}
		case CMD_USER_LOG_RECV:
		{
			Notice.SetWindowText(L"로그 출력");
			Log_Data.ResetContent();
			string data = packet.body.data;
			int pos = 0;
			for (int i = 0; i < data.size(); i++)
			{
				if (data[i] == '\n')
				{
					int len = i - pos;
					string result = data.substr(pos, len);
					CString cresult(result.c_str());
					Log_Data.AddString((LPCTSTR)cresult);
					pos = i + 1;
				}
			}
			break;
		}
		case CMD_USER_DEL_RECV:
		{
			Notice.SetWindowText(L"내 로그 삭제");
			break;
		}
		case CMD_USER_NAME_RECV:
		{
			users_name.ResetContent();
			string data = packet.body.data;
			int pos = 0;
			for (int i = 0; i < data.size(); i++)
			{
				if (data[i] == '\n')
				{
					int len = i - pos;
					string result = data.substr(pos, len);
					CString cresult(result.c_str());
					users_name.AddString((LPCTSTR)cresult);
					pos = i + 1;
				}
			}
		}
		default:
		{
			break;
		}
		}
	}
}






void ClientMenu::OnBnClickedButton4()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void ClientMenu::send_dummydata()
{

}

BOOL ClientMenu::PreTranslateMessage(MSG* pMsg)
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
