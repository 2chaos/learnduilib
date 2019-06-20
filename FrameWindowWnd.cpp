#pragma once
#include "FrameWindowWnd.h"
#include "stdafx.h"
#include "sqliteFunc.h"

// 窗口实例及消息响应部分
void CFrameWindowWnd::Reset() {
	pText->SetText(_T("00:00.00"));
	btnLeft_state = (button_state)(up | disable);
	pBtnLeft->SetEnabled(false);
	pBtnLeft->SetText(_T("计次"));
	btnRight_state = up;
	time_exist.wMinute = time_exist.wSecond = time_exist.wMilliseconds = 0;
	time_beg.wMinute = time_beg.wSecond = time_beg.wMilliseconds = 0;
	time_now.wMinute = time_now.wSecond = time_now.wMilliseconds = 0;
	lastShow.min = lastShow.sec = lastShow.msec = 0;
	pList->RemoveAll();
	recordNum = 0;
	recordsSize = 0;
}
void CFrameWindowWnd::Count() {
	cusTime recordTime;
	recordTime.min = show.min - lastShow.min;
	recordTime.sec = show.sec - lastShow.sec;
	recordTime.msec = show.msec - lastShow.msec;
	correctTime(recordTime.min, recordTime.sec, recordTime.msec);
	
	//子线程执行数据写入
	writeRecord(recHistory, tableName, recordNum, recordTime.min, recordTime.sec, recordTime.msec);

	lastShow.min	= show.min;
	lastShow.sec	= show.sec;
	lastShow.msec	= show.msec;
	pListElement	= new CListTextElementUI();
	pList->Add(pListElement);
	++recordNum;
}
void CFrameWindowWnd::Start() {
	GetLocalTime(&time_beg);
	if (btnLeft_state & disable)
		pBtnLeft->SetEnabled(true);
	btnLeft_state = (button_state)(up | enable);
	pBtnLeft->SetText(_T("计次"));
	btnRight_state = down;
	pBtnRight->SetText(_T("停止"));
	if (pList->GetCount() == recordsSize) {
		pListElement = new CListTextElementUI();
		pList->Add(pListElement);
		++recordNum;
	}
	SetTimer(hWnd, ID_Timer, 10, nullptr);
}
void CFrameWindowWnd::Stop() {
	btnRight_state = up;
	KillTimer(hWnd, ID_Timer);
	time_exist.wMinute = show.min;
	time_exist.wSecond = show.sec;
	time_exist.wMilliseconds = show.msec;
	pBtnLeft->SetText(_T("复位"));
	btnLeft_state = down;
	pBtnRight->SetText(_T("启动"));
}
void CFrameWindowWnd::correctTime(int& min, int& sec, int& msec) {
	if (msec < 0) {
		msec += 1000;
		--sec;
	}
	if (msec >= 1000) {
		msec -= 1000;
		++sec;
	}
	if (sec < 0) {
		sec += 60;
		--min;
	}
	if (sec >= 60) {
		sec -= 60;
		++min;
	}
}
void CFrameWindowWnd::InitWindow() {
	pText = static_cast<CTextUI*>(m_PaintManager.FindControl(_T("timeNow")));
	pBtnLeft = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btnLeft")));
	pBtnRight = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btnRight")));
	pList = static_cast<CListUI*>(m_PaintManager.FindControl(_T("timeHistory")));
	Reset();

	// 从database中读取记录存到records中
	char sqlstr[200];

	sprintf(sqlstr, "select count(*) from %s;", tableName.c_str());					// get total numbers
	int rc = sqlite3_exec(recHistory, sqlstr, getRecordSize, nullptr, 0);
	ASSERT(rc == SQLITE_OK);
	sprintf(sqlstr, "select num, timestr from %s;", tableName.c_str());				// records.push_back()
	rc = sqlite3_exec(recHistory, sqlstr, getRecords, nullptr, 0);
	ASSERT(rc == SQLITE_OK);

	for (int i = 0; i < records.size(); ++i) {
		pListElement = new CListTextElementUI();
		pList->Add(pListElement);

		size_t len = records[i]->num.size() + 1;
		TCHAR* tstr = new TCHAR[len];
		mbstowcs(tstr, records[i]->num.c_str(), len);
		pListElement->SetText(0, tstr);
		delete[] tstr;

		len = records[i]->timestr.size() + 1;
		tstr = new TCHAR[len];
		mbstowcs(tstr, records[i]->timestr.c_str(), len);
		pListElement->SetText(1, tstr);
		delete[] tstr;
	}
}
void CFrameWindowWnd::Notify(TNotifyUI& msg) {
	if (msg.sType == _T("click")) {
		if (msg.pSender->GetName() == _T("closebtn"))
			::PostQuitMessage(0);
		else if (msg.pSender->GetName() == _T("btnLeft")) {
			if (btnLeft_state & down)							//复位
				Reset();
			else if (btnLeft_state == (up | enable)) 			//计次		
				Count();
		}
		else if (msg.pSender->GetName() == _T("btnRight")) {
			if (btnRight_state & up) 							//启动
				Start();
			else if (btnRight_state & down) 					//停止
				Stop();
		}
	}
}
LRESULT CFrameWindowWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (uMsg == WM_TIMER) {
		if (btnRight_state & down) {
			GetLocalTime(&time_now);
			//backup
			show.min = time_exist.wMinute + time_now.wMinute - time_beg.wMinute;
			show.sec = time_exist.wSecond + time_now.wSecond - time_beg.wSecond;
			show.msec = time_exist.wMilliseconds + time_now.wMilliseconds - time_beg.wMilliseconds;
			correctTime(show.min, show.sec, show.msec);

			delta.min = show.min - lastShow.min;
			delta.sec = show.sec - lastShow.sec;
			delta.msec = show.msec - lastShow.msec;
			correctTime(delta.min, delta.sec, delta.msec);

			TCHAR showStr[10];
			wsprintf(showStr, _T("No.%d"), recordNum);
			pListElement->SetText(0, showStr);
			wsprintf(showStr, _T("%02d:%02d.%02d"), delta.min, delta.sec, delta.msec / 10);
			pListElement->SetText(1, showStr);
			wsprintf(showStr, _T("%02d:%02d.%02d"), show.min, show.sec, show.msec / 10);
			pText->SetText(showStr);
		}
		return 0;
	}
	else return __super::HandleMessage(uMsg, wParam, lParam);
}
