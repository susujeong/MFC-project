
// ChatClientDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "ChatClient.h"
#include "ChatClientDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CChatClientDlg 대화 상자


CChatClientDlg::CChatClientDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CHATCLIENT_DIALOG, pParent)
	, m_strIP(_T(""))
	, m_strStatus(_T(""))
	, m_strStatus2(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CChatClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_IP, m_strIP);
	DDX_Control(pDX, IDC_LIST1, m_list);
	DDX_Control(pDX, IDC_PICTURE, testwebcam);
	DDX_Text(pDX, IDC_STATIC_MSG, m_strStatus);
	DDX_Text(pDX, IDC_STATIC_MSG2, m_strStatus2);
	DDX_Control(pDX, IDC_TOTAL, m_List);
	DDX_Control(pDX, IDC_PIC_RESULT, m_PIC);
}

BEGIN_MESSAGE_MAP(CChatClientDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE_VOID(UM_RECEIVE, OnReceive)
	ON_BN_CLICKED(IDC_BUTTON_CONNECT, &CChatClientDlg::OnClickedButtonConnect)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_PIC, &CChatClientDlg::OnBnClickedButtonPic)
	ON_BN_CLICKED(IDC_RESULT, &CChatClientDlg::OnBnClickedResult)
END_MESSAGE_MAP()


// CChatClientDlg 메시지 처리기

BOOL CChatClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	//m_strIP.SetString(_T("10.10.20.103"));

	// 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
	capture = new VideoCapture(0);
	if (!capture->isOpened())
	{
		MessageBox(_T("웹캠안열림!!\n"));
	}
	capture->set(CAP_PROP_FRAME_WIDTH, 320);
	capture->set(CAP_PROP_FRAME_HEIGHT, 240);

	SetTimer(1000, 30, NULL);


	// 리스트컨트롤 초기화
	CRect rect;										// 리스트 컨트롤 크기를 가져올 변수
	m_List.GetClientRect(&rect);
	m_List.SetExtendedStyle(LVS_EX_GRIDLINES);		// 리스트컨트롤 그리드 표시

	// 리스트 컨트롤 컬럼 추가
	m_List.InsertColumn(0, _T("제품번호"), LVCFMT_LEFT, rect.Width()*0.33);		// 첫번째
	m_List.InsertColumn(1, _T("분석결과"), LVCFMT_LEFT, rect.Width() * 0.33);	// 두번째
	m_List.InsertColumn(2, _T("불량종류"), LVCFMT_LEFT, rect.Width() * 0.33);	// 세번째


	GetDlgItem(IDC_PIC_RESULT)->GetWindowRect(m_image_rect);
	ScreenToClient(m_image_rect);


	return TRUE;
}


// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CChatClientDlg::OnPaint()
{
	CPaintDC dc(this);

	if (IsIconic())
	{
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		if (!m_image.IsNull()) { // 그림이 로딩되었는지 체크한다.
			// 이미지가 원본 크기와 다르게 출력될때 어떤 방식으로 이미지를
			// 확대하거나 축소할 것인지를 결정한다.
			dc.SetStretchBltMode(COLORONCOLOR);
			// 그림을 Picture Control 크기로 화면에 출력한다.
			m_image.Draw(dc, m_image_rect);
		}
		//CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CChatClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// 데이터를 보내는 것은 소켓 클래스의 멤버 함수인 Send를 이용
// 데이터를 받을 때는 통신 소켓 클래스에 오버라이딩한 OnReceive 메시지 함수를 사용
LPARAM CChatClientDlg::OnReceive(UINT wParam, LPARAM lParam) 
{
	UpdateData(TRUE);

	// 접속된 곳에서 데이터가 도착했을 때
	char pTmp[2048];
	CString strTmp;
	memset(pTmp, '\0', 2048);

	// 데이터를 pTmp에 받는다.
	m_socCom.Receive(pTmp, 2048);
	strTmp.Format(_T("%s"), pTmp);

	CString result = (_T(""));
	AfxExtractSubString(result, strTmp, 0, '^');


	CString product = (_T(""));
	AfxExtractSubString(product, strTmp, 2, '^');			// 제품번호

	CString color = (_T(""));
	AfxExtractSubString(color, strTmp, 3, '^');			// 양/불


	int resultNum = _ttoi(result);

	if (resultNum == 3)
	{
		std::vector<CString> vecText;
		int nP = 0;
		CString strText(_T(""));
		while (FALSE != AfxExtractSubString(strText, strTmp, nP++, '^'))
		{
			vecText.push_back(strText);
		}

		//int x = vecText.size();
		//CString ss;
		//ss.Format(_T("%d"), x);
		//MessageBox(ss);

		//MessageBox(vecText[0]);
		//MessageBox(vecText[1]);
		//MessageBox(vecText[2]);
		//MessageBox(vecText[3]);
		//MessageBox(vecText[4]);


		int nItemNum = m_List.GetItemCount();
		int j = nItemNum;

		for (int i = 0; i < vecText.size() - 1; i += 3)
		{
			j = m_List.GetItemCount();
			if (vecText.size() - 2 <= i)
			{
				break;
			}
			m_List.InsertItem(j, vecText[i + 1]);				// 제품코드

			m_List.SetItemText(j, 1, vecText[i + 2]);		// 분석결과
			m_List.SetItemText(j, 2, vecText[i + 3]);		// 분석종류
		}
	}
	else
	{
		if (color == _T("불량"))
		{
			m_image.Destroy();
			m_image.Load(L"red.png");
			InvalidateRect(m_image_rect, false);
		}
		else
		{
			m_image.Destroy();
			m_image.Load(L"green.png");
			InvalidateRect(m_image_rect, false);
		}

		// 메세지 갈아치우기
		m_strStatus = product;
		m_strStatus2 = color;

		// 리스트박스에 보여준다.
		int i = m_list.GetCount();
		m_list.InsertString(i, strTmp);
	}

	UpdateData(FALSE);
	return TRUE;
}



// 서버에 접속 요청
// 소켓 클라이언트의 Create함수에서 인자가 있으면 서버로 인식하므로, 인자를 넘기지 않는다. 그러면 클라이언트로 판단해서 동작.
// Create로 소켓을 생성했다면 실제 접속은 Connect 함수를 이용
// 첫번째 인자를 이용해(m_strIP는 에디터 컨트롤과 연결된 멤버 변수) 접속할 곳의 IP 주소를 넘기며 두번째 인자를 통해 접속할 포트 번호를 지정
void CChatClientDlg::OnClickedButtonConnect()
{
	// 접속
	UpdateData(TRUE);
	m_socCom.Create();

	// IP 주소와 포트 번호를 지정
	m_socCom.Connect(m_strIP, 9190);
	m_socCom.Init(this->m_hWnd);
}

// 웹캠
void CChatClientDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	capture->read(mat_frame);

	int bpp = 8 * mat_frame.elemSize();
	assert((bpp == 8 || bpp == 24 || bpp == 32));

	int padding = 0;
	if (bpp < 32)
		padding = 4 - (mat_frame.cols % 4);

	if (padding == 4)
		padding = 0;

	int border = 0;
	if (bpp < 32)
	{
		border = 4 - (mat_frame.cols % 4);
	}



	Mat mat_temp;
	if (border > 0 || mat_frame.isContinuous() == false)
	{
		cv::copyMakeBorder(mat_frame, mat_temp, 0, 0, 0, border, cv::BORDER_CONSTANT, 0);
	}
	else
	{
		mat_temp = mat_frame;
	}


	RECT r;
	testwebcam.GetClientRect(&r);
	Size winSize(r.right, r.bottom);

	cimage_mfc.Create(winSize.width, winSize.height, 24);


	BITMAPINFO* bitInfo = (BITMAPINFO*)malloc(sizeof(BITMAPINFO) + 256 * sizeof(RGBQUAD));
	bitInfo->bmiHeader.biBitCount = bpp;
	bitInfo->bmiHeader.biWidth = mat_temp.cols;
	bitInfo->bmiHeader.biHeight = -mat_temp.rows;
	bitInfo->bmiHeader.biPlanes = 1;
	bitInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bitInfo->bmiHeader.biCompression = BI_RGB;
	bitInfo->bmiHeader.biClrImportant = 0;
	bitInfo->bmiHeader.biClrUsed = 0;
	bitInfo->bmiHeader.biSizeImage = 0;
	bitInfo->bmiHeader.biXPelsPerMeter = 0;
	bitInfo->bmiHeader.biYPelsPerMeter = 0;

	if (bpp == 8)
	{
		RGBQUAD* palette = bitInfo->bmiColors;
		for (int i = 0; i < 256; i++)
		{
			palette[i].rgbBlue = palette[i].rgbGreen = palette[i].rgbRed = (BYTE)i;
			palette[i].rgbReserved = 0;
		}
	}


	if (mat_temp.cols == winSize.width && mat_temp.rows == winSize.height)
	{

		SetDIBitsToDevice(cimage_mfc.GetDC(),
			0, 0, winSize.width, winSize.height,
			0, 0, 0, mat_temp.rows,
			mat_temp.data, bitInfo, DIB_RGB_COLORS);
	}
	else
	{
		int destx = 0, desty = 0;
		int destw = winSize.width;
		int desth = winSize.height;
		int imgx = 0, imgy = 0;
		int imgWidth = mat_temp.cols - border;
		int imgHeight = mat_temp.rows;

		StretchDIBits(cimage_mfc.GetDC(),
			destx, desty, destw, desth,
			imgx, imgy, imgWidth, imgHeight,
			mat_temp.data, bitInfo, DIB_RGB_COLORS, SRCCOPY);
	}


	HDC dc = ::GetDC(testwebcam.m_hWnd);
	cimage_mfc.BitBlt(dc, 0, 0);


	::ReleaseDC(testwebcam.m_hWnd, dc);

	cimage_mfc.ReleaseDC();
	cimage_mfc.Destroy();
	CDialogEx::OnTimer(nIDEvent);
}


void CChatClientDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}


UINT CChatClientDlg::RunCap(LPVOID pParam)
{
	CChatClientDlg* g_pDlg = (CChatClientDlg*)pParam;
	g_pDlg->m_bRun = true;

	while (1)
	{
		Sleep(3000);
		if (g_pDlg->m_bRun == true)
		{
			CString splitMsg = _T("1^");
			g_pDlg->m_socCom.Send(splitMsg, 256);
			Mat send;
			g_pDlg->capture->read(send);

			imwrite("C:\\Users\\iot\\PycharmProjects\\pythonProject\\1.jpg", send);

			CString sendPath = _T("C:\\Users\\iot\\PycharmProjects\\pythonProject\\1.jpg");
			CFile sourceFile;
			sourceFile.Open(sendPath, CFile::modeRead | CFile::typeBinary);

			int fileSize = sourceFile.GetLength();
			g_pDlg->m_socCom.Send(&fileSize, sizeof(fileSize)); // 파일 사이즈 전송

			byte* data = new byte[fileSize];
			DWORD dwRead;
			dwRead = sourceFile.Read(data, fileSize);
			g_pDlg->m_socCom.Send(data, dwRead);

			delete data;
			sourceFile.Close();
		}
		if (g_pDlg->m_bStop == true)
		{
			return 0;
		}
	}
	return 0;
}

void CChatClientDlg::OnBnClickedButtonPic()
{
	// 스레드를 시작하는 루틴을 넣음(스레드의 핸들, 변수 초기화 등등)
	// AfxBeginThread로 스레드 시작, 파라미터로 스레드 함수와 메인클래스 주소값 넘김
	if (m_pThread == NULL)
	{
		m_pThread = AfxBeginThread(RunCap, this);
		hThread = m_pThread->m_hThread;
		m_bStop = false;
		m_bRun = false;
		if (!m_pThread)
		{
			AfxMessageBox(_T("Thread could not be created,"));
			return;
		}
	}

	//if (m_pThread != NULL)
	//{
	//	m_bStop = true;
	//	m_pThread = NULL;
	//}
}


// 전체 결과 송신
UINT CChatClientDlg::Result(LPVOID pParam)
{
	CChatClientDlg* g_pDlg = (CChatClientDlg*)pParam;
	g_pDlg->m_bRun2 = true;
	//g_pDlg->m_bStop2 = false;

	while (1)
	{
		if (g_pDlg->m_bRun2 == true)
		{
			CString splitMsg = _T("3^");
			g_pDlg->m_socCom.Send(splitMsg, 256);

			g_pDlg->m_bRun2 == false;
			g_pDlg->m_bStop2 == true;
		}
		if (g_pDlg->m_bStop2 == true)
		{
			break;
		}
	}
	//AfxMessageBox(_T("꺼져"));
	return 0;
}


// 전체결과 요청
void CChatClientDlg::OnBnClickedResult()
{
	m_List.DeleteAllItems();


	if (m_pThread2 == NULL)
	{
		m_pThread2 = AfxBeginThread(Result, this);
		hThread2 = m_pThread2->m_hThread;
		m_bStop2 = true;
		m_bRun2 = false;
		if (!m_pThread2)
		{
			AfxMessageBox(_T("Thread could not be created,"));
			return;
		}
		//AfxMessageBox(_T("꺼져2"));
	}

	if (m_pThread2 != NULL)
	{
		m_bStop2 = true;
		m_pThread2 = NULL;
	}
	//AfxMessageBox(_T("꺼져3"));
}
