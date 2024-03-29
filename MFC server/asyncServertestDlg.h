
// asyncServertestDlg.h: 헤더 파일
//

#pragma once
#include "afxwin.h"
#include "ListenSocket.h"

// CasyncServertestDlg 대화 상자
class CasyncServertestDlg : public CDialogEx
{
private:
	CFont m_font;
// 생성입니다.
public:
	CasyncServertestDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.
	CListenSocket m_ListenSocket; // 서버용 소켓

	//쓰레드용
	CWinThread* m_pThread;
	HANDLE hThread;
	bool m_bRun, m_bStop;
	static UINT RunThread(LPVOID pParam);


// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ASYNCSERVERTEST_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CListBox clntlist;
	afx_msg void OnDestroy();
	CListCtrl resultlist;
};
