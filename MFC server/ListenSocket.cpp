// ListenSocket.cpp: 구현 파일
//

#include "pch.h"
#include "asyncServertest.h"
#include "ListenSocket.h"
#include "ClientSocket.h"
#include "asyncServertestDlg.h"


// CListenSocket

CListenSocket::CListenSocket()
{
}

CListenSocket::~CListenSocket()
{
}


// CListenSocket 멤버 함수


void CListenSocket::OnAccept(int nErrorCode)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	CClientSocket* pClient = new CClientSocket;
	if (Accept(*pClient))
	{
		pClient->SetListenSocket(this);
		m_ptrClientSocketList.AddTail(pClient);
	
		CasyncServertestDlg* pMain = (CasyncServertestDlg*)AfxGetMainWnd();
		pMain->clntlist.AddString(_T("클라이언트 접속")); // 클라이언트 접속 진행상황 출력
		pMain->clntlist.SetCurSel(pMain->clntlist.GetCount() - 1);
	}
	else
	{
		delete pClient;
		AfxMessageBox(_T("ERROR: Failed to accept new client!"));
	}

	CAsyncSocket::OnAccept(nErrorCode);
} 

void CListenSocket::CloseClientSocket(CSocket* pClient) // 특정 클라이언트 접속이 끊겼을때 객체 소멸시키기
{
	POSITION pos;
	pos = m_ptrClientSocketList.Find(pClient);
	if (pos != NULL)
	{
		if (pClient != NULL)
		{
			pClient->ShutDown();
			pClient->Close();
		}

		m_ptrClientSocketList.RemoveAt(pos);
		delete pClient;
	}
}

void CListenSocket::SendResultData(TCHAR* pszMessage) // mfc 클라로 결과 전송하는 코드
{
	POSITION pos;
	pos = m_ptrClientSocketList.GetHeadPosition();
	CClientSocket* pClient = NULL;
	pClient = (CClientSocket*)m_ptrClientSocketList.GetNext(pos); // 전해진 클라이언트한테 메세지 전송하게?

	pClient->Send(pszMessage, lstrlen(pszMessage) * 2);

}

void CListenSocket::SendImageData() // 파이썬으로 이미지 전송하는 코드...
{
	a++;
	CString fnstr = _T("");
	fnstr.Format(_T("%d"), a);
	CString sendPath = _T("C:\\Users\\iot\\Desktop\\servertest\\"+fnstr+".jpg");
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CFile sourceFile;
	sourceFile.Open(sendPath, CFile::modeRead | CFile::typeBinary);

	POSITION pos2;

	pos2 = m_ptrClientSocketList.GetTailPosition(); // 마지막 요소의 주소값 가지고오기


	CClientSocket* pClient = NULL;

	pClient = (CClientSocket*)m_ptrClientSocketList.GetNext(pos2); // 마지막 요소의 주소값에 있는 애 할당

	int fileSize = sourceFile.GetLength();
	CString str;
	str.Format(_T("%d"), fileSize);
	
	pClient->Send(str, 16);

	byte* data = new byte[fileSize];
	DWORD dwRead;

	dwRead = sourceFile.Read(data, fileSize);
	pClient->Send(data, dwRead);

	delete data;
	sourceFile.Close();
}