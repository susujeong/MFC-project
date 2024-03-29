
// ChatClientDlg.h: 헤더 파일
//

#include "SocCom.h"
#include "opencv2/opencv.hpp"
#pragma once
using namespace cv;

// CChatClientDlg 대화 상자
class CChatClientDlg : public CDialogEx
{
// 생성입니다.
private:
	CRect m_image_rect;
	CImage m_image;

public:
	CChatClientDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

	CSocCom m_socCom;			// 통신용 소켓

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CHATCLIENT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LPARAM OnReceive(UINT wParam, LPARAM IParam);
	DECLARE_MESSAGE_MAP()
public:
	CString m_strIP;
	CListBox m_list;
	afx_msg void OnClickedButtonConnect();
	
	CStatic testwebcam;

	VideoCapture* capture;
	Mat mat_frame;
	CImage cimage_mfc;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedButtonPic();
	CString m_strStatus;
	CString m_strStatus2;


	// m_pThread를 통해 스레드 시작, hThread에 해당 핸들 넣음
	// m_bRun, m_bStop으로 스레드 시작하고 종료하는 구문 넣음
	CWinThread* m_pThread;
	HANDLE hThread;
	bool m_bRun, m_bStop;
	static UINT RunCap(LPVOID pParam);

	CWinThread* m_pThread2;
	HANDLE hThread2;
	bool m_bRun2, m_bStop2;
	static UINT Result(LPVOID pParam);



	CListCtrl m_List;
	afx_msg void OnBnClickedResult();
	CStatic m_PIC;
};
