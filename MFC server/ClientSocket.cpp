// ClientSocket.cpp: 구현 파일
//

#include "pch.h"
#include "asyncServertest.h"
#include "ClientSocket.h"
#include "ListenSocket.h"
#include "asyncServertestDlg.h"
#include <afxsock.h>
#include "stdafx.h"
#include "mysql.h"

// CClientSocket
CClientSocket::CClientSocket()
{
	m_pListenSocket = NULL;
}

CClientSocket::~CClientSocket()
{
}


// CClientSocket 멤버 함수
void CClientSocket::SetListenSocket(CAsyncSocket* pSocket)
{
	m_pListenSocket = pSocket;
}


void CClientSocket::OnClose(int nErrorCode) // 함수의 연결이 종료되는 시점에서 호출
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	CSocket::OnClose(nErrorCode);

	CListenSocket* pServerSocket = (CListenSocket*)m_pListenSocket;
	pServerSocket->CloseClientSocket(this);
	
	CasyncServertestDlg* pMain = (CasyncServertestDlg*)AfxGetMainWnd();
	pMain->clntlist.AddString(_T("클라이언트 접속 종료")); // 진행 리스트에 띄우고싶음..
	pMain->clntlist.SetCurSel(pMain->clntlist.GetCount() - 1);
}


void CClientSocket::OnReceive(int nErrorCode)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	CString strTmp = _T("");
	TCHAR szBuffer[30000];
	
	int str_len;
	str_len = Receive(szBuffer, sizeof(szBuffer));

	szBuffer[str_len] = 0;

	strTmp.Format(_T("%s"), szBuffer);

	CString tmp = _T("");
	CString tmsg = _T("");
	CString tmsgs = _T("");
	CString tmsgss = _T("");
	CString resulttmp1 = _T("");
	CString resulttmp = _T("");
	AfxExtractSubString(tmp, strTmp, 0, '^'); // 가장 처음에 있는 ^ 나누는 것
	AfxExtractSubString(tmsg, strTmp, 1, '^'); // 2
	AfxExtractSubString(tmsgs, strTmp, 2, '^'); // 제품코드
	AfxExtractSubString(tmsgss, strTmp, 3, '^'); // 마지막꺼
	AfxExtractSubString(resulttmp1, strTmp, 4, '^'); // 이게 분석결과
	
	if (strTmp.Find(_T("1^"))==0) // mfc 클라이언트
	{
		a++;
		CString fnstr = _T("");
		fnstr.Format(_T("%d"), a); 
		
		CString savePath = _T("C:\\Users\\iot\\Desktop\\servertest\\"+fnstr+".jpg"); // 파일 이름

		CFile targetFile;
		targetFile.Open(savePath, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);
		int filesize = targetFile.GetLength();
		Receive(&filesize, sizeof(int)); //파일 사이즈 받는것
		byte* data = new byte[filesize];
		DWORD dwRead;

		int nReceiveLen = 0;
		do
		{
			dwRead = Receive(data, filesize);
			targetFile.Write(data, dwRead);
			nReceiveLen += dwRead;
			if (nReceiveLen >= filesize)
			{
				break;
			}
		} while (dwRead > 0);
		CasyncServertestDlg* pMain = (CasyncServertestDlg*)AfxGetMainWnd();
		pMain->clntlist.AddString(_T("이미지 파일 수신 완료")); // 진행 리스트에 띄우고싶음..
		pMain->clntlist.SetCurSel(pMain->clntlist.GetCount() - 1);

		delete data;
		targetFile.Close();

		// 받은 이미지 파이썬 서버로 보내기
		CListenSocket* pServerSocket = (CListenSocket*)m_pListenSocket;
		pServerSocket->SendImageData();
	}
	else if (tmsg.Find(_T("2")) == 0) // 파이썬 서버(클라이언트)
	{
		char sql[1024];
		CasyncServertestDlg* pMain = (CasyncServertestDlg*)AfxGetMainWnd();

		int nItemNum = pMain->resultlist.GetItemCount();
		pMain->resultlist.InsertItem(nItemNum, tmsgs); // 제품코드
		pMain->resultlist.SetItemText(nItemNum, 1, tmsgss); // 분석결과
		pMain->resultlist.SetItemText(nItemNum, 2, resulttmp1); // 불량종류

		CListenSocket* pServerSocket = (CListenSocket*)m_pListenSocket;
		pServerSocket->SendResultData((TCHAR*)(LPCTSTR)strTmp); // 실시간 결과 바로 보내기
		CString invertquery = _T("INSERT INTO testresult.result (code, baby, type) VALUES ('")+tmsgs+_T("', '")+tmsgss+_T("', '")+resulttmp1+_T("')");
		CStringA queryBuffer = (CStringA)invertquery;
		const char* query;
		query = queryBuffer.GetBuffer();
		mysql_query(&connection, query);
	}
	else if (strTmp.Find(_T("3^")) == 0) // 전체결과 전송
	{
		CString invertquery = _T("SELECT * FROM testresult.result");
		CStringA queryBuffer = (CStringA)invertquery;
		const char* query;
		query = queryBuffer.GetBuffer();
		int field; 
		mysql_query(&connection, "set Names euckr"); // 한국어 사용
		mysql_query(&connection, query); // 이게 쿼리 실행문
		Sql_Result = mysql_store_result(&connection);
		field = mysql_num_fields(Sql_Result);
		CString row = _T("3^");
		while ((Sql_Row = mysql_fetch_row(Sql_Result)) != NULL)
		{
			for (int i = 0; i < field; i++)
			{
				row += Sql_Row[i];
				row += "^";
			}
		}
		CListenSocket* pServerSocket = (CListenSocket*)m_pListenSocket;
		pServerSocket->SendResultData((TCHAR*)(LPCTSTR)row);
	}
	CSocket::OnReceive(nErrorCode);
}

