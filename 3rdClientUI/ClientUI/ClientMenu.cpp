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
extern User test_user[5];
clock_t st, ed;
int recv_cnt;
bool ss[6000];
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
	DDV_MaxChars(pDX, Input_Data, 30);


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
	//thread send_thread(&ClientMenu::send_data,this);
	thread recv_thread(&ClientMenu::recv_data,this);

	//send_thread.detach();
	recv_thread.detach();
}


void ClientMenu::OnBnClickedButton1() // Send Button
{
	recv_cnt = 0;
	st = clock();
	CEdit* p = (CEdit*)GetDlgItem(IDC_EDIT1);
	if (p->GetWindowTextLengthW() > 0)
	{
		CString data;
		p->GetWindowTextW(data);
		string sdata = CT2CA(data);
		user.send_data(SEND, sdata);
		
		for (int i = 1; i <= 5000; i++)
		{
			string s = to_string(i);
			user.send_data(SEND, s);
			for (int i = 10; i < 5; i++)
			{
				test_user[i].send_data(SEND, s);
			}
		}
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
	//m_requestpool.push(make_pair(LOG, ""));
}

void ClientMenu::OnBnClickedButton3() // Delete Button
{
	user.send_data(DEL, "");
	//m_requestpool.push(make_pair(DEL, ""));
}


void ClientMenu::OnBnClickedCancel() // Exit Button
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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
			ss[_ttoi(recv_data)] = true;
			if (recv_data == "10000")
			{
				ed = clock();
				double res = double(ed - st);
				notice_text = to_string(res).c_str();
				Notice.SetWindowText((notice_text));
			}
			recv_cnt++;
			notice_text = to_string(recv_cnt).c_str();
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
			string result = data.substr(pos);
			CString cresult(result.c_str());
			Log_Data.AddString((LPCTSTR)cresult);
			break;
		}
		case CMD_USER_DEL_RECV:
		{
			Notice.SetWindowText(L"내 로그 삭제");
			break;
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