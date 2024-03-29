
// asyncServertestDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "asyncServertest.h"
#include "asyncServertestDlg.h"
#include "afxdialogex.h"
#include "ClientSocket.h"
#include "mysql.h"
#include "stdafx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

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
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CasyncServertestDlg 대화 상자
CasyncServertestDlg::CasyncServertestDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_ASYNCSERVERTEST_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CasyncServertestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, clntlist);
	DDX_Control(pDX, IDC_LIST2, resultlist);
}

BEGIN_MESSAGE_MAP(CasyncServertestDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CasyncServertestDlg 메시지 처리기

BOOL CasyncServertestDlg::OnInitDialog()
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

	// 쓰레드
	if (m_pThread == NULL)
	{
		m_pThread = AfxBeginThread(RunThread, this);
		hThread = m_pThread->m_hThread;
		m_bStop = false;
		m_bRun = false;
		if (!m_pThread)
		{
			AfxMessageBox(_T("Thread could not be created."));
			return TRUE;
		}
	}

	// 리스트컨트롤
	CRect rect;
	resultlist.GetClientRect(&rect);
	resultlist.DeleteAllItems(); //초기화
	resultlist.SetExtendedStyle(LVS_EX_GRIDLINES);
	resultlist.InsertColumn(0, _T("제품코드"), LVCFMT_CENTER, 272);
	resultlist.InsertColumn(1, _T("분석결과"), LVCFMT_CENTER, 272);
	resultlist.InsertColumn(2, _T("불량종류"), LVCFMT_CENTER, 272);

	// 리스트 박스 폰트 설정
	m_font.CreatePointFont(96, L"맑은 고딕");
	clntlist.SetFont(&m_font);
	resultlist.SetFont(&m_font);

	//DB연결
	mysql_init(&connection);
	if (!mysql_real_connect(&connection, MY_IP, DB_USER, DB_PASS, DB_NAME, 3306, NULL, 0))
	{
		CString s = _T("");
		s = mysql_error(&connection);
		AfxMessageBox(s, MB_OK);
	}
	else
	{
		mysql_query(&connection, "set names euckr"); //한글 인식

	}

	// 소켓 생성하는 역할
	if (m_ListenSocket.Create(9190, SOCK_STREAM)) 
	{
		if (!m_ListenSocket.Listen())
		{
			AfxMessageBox(L"ERROR: Listen() return FALSE");
		}
	}
	else
	{
		AfxMessageBox(L"ERROR: Failed to create server socket!");
	}

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CasyncServertestDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CasyncServertestDlg::OnPaint()
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
HCURSOR CasyncServertestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CasyncServertestDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	if (m_pThread != NULL)
	{
		m_bStop = true;
		m_pThread = NULL;
	}

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	POSITION pos;
	pos = m_ListenSocket.m_ptrClientSocketList.GetHeadPosition();
	CClientSocket* pClient = NULL;

	while (pos != NULL)
	{
		pClient = (CClientSocket*)m_ListenSocket.m_ptrClientSocketList.GetNext(pos);

		if (pClient != NULL)
		{
			pClient->ShutDown();
			pClient->Close();

			delete pClient;
		}
	}

	m_ListenSocket.ShutDown();
	m_ListenSocket.Close();
}

UINT CasyncServertestDlg::RunThread(LPVOID pParam)
{
	CasyncServertestDlg* g_pDlg = (CasyncServertestDlg*)pParam;
	g_pDlg->m_bRun = true;
	while (1)
	{
		if (g_pDlg->m_bRun == true)
		{
			CClientSocket* pMain = (CClientSocket*)AfxGetApp();
			pMain->OnReceive(0);
		}
		if (g_pDlg->m_bStop == true)
		{
			return 0;
		}
	}
	return 0;
}