// ClientMenu.cpp: 구현 파일
//

#include "pch.h"
#include "ClientUI.h"
#include "afxdialogex.h"
#include "ClientMenu.h"


// ClientMenu 대화 상자

extern User user;

IMPLEMENT_DYNAMIC(ClientMenu, CDialogEx)

ClientMenu::ClientMenu(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG1, pParent)
	, Input_Data(_T(""))
{

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

END_MESSAGE_MAP()


// ClientMenu 메시지 처리기


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
		recv_data();
	}
	else
	{
		Notice.SetWindowText(L"Input Send Data");
	}
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void ClientMenu::OnBnClickedButton2() // Log Button
{
	user.send_data(LOG, "");
	recv_data();
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void ClientMenu::OnBnClickedButton3() // Delete Button
{
	user.send_data(DEL, "");
	recv_data();
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void ClientMenu::OnBnClickedCancel() // Exit Button
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialogEx::OnCancel();
}



void ClientMenu::recv_data()
{
	Packet packet = user.recv_data();
	switch (packet.cmd)
	{
	case CMD_USER_DATA_RECV:
	{
		Notice.SetWindowText(L"Send data Success");
		break;
	}
	case CMD_USER_LOG_RECV:
	{
		Notice.SetWindowText(L"LOG Success");
		Log_Data.ResetContent();
		string data = packet.data;
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
		Notice.SetWindowText(L"DEL Success");
		break;
	}
	default:
	{
		break;
	}
	}
}

