// Duilib 使用设置部分
#pragma once
#include "stdafx.h"
#include "sqliteFunc.h"
#include "FrameWindowWnd.h"

// 全局变量
HWND hWnd;
sqlite3* recHistory;
bool isTableExisted = false;
bool isWindowClosed = false;
string databaseName = "Database/RecHistory.db", tableName = "record";
vector<shared_ptr<recordRow>> records, newRecords;
int recordsIdx = 0;
int recordsSize;
std::queue<string> tasks;

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR
	/*lpCmdLine*/, int nCmdShow)
{
	HRESULT Hr = ::CoInitialize(NULL);
	if (FAILED(Hr)) return 0;

	//打开数据库，若记录表不存在则创建
	openDatabaseAndTable(recHistory, databaseName, tableName);
	std::thread thd(writeRecord_T, std::ref(recHistory), std::ref(tasks));
	thd.detach();

	CPaintManagerUI::SetInstance(hInstance);
	CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath());
	CFrameWindowWnd* pFrame = new CFrameWindowWnd();
	if (pFrame == NULL) return 0;
	hWnd = pFrame->Create(NULL, _T("StopWatch"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
	pFrame->CenterWindow();
	pFrame->ShowWindow(true);
	CPaintManagerUI::MessageLoop();
	isWindowClosed = true;

	sqlite3_close(recHistory);
	::CoUninitialize();
	return 0;
}

/*
raw pointer
1. 智能指针
2. 使用子线程来写
3. 如何分模块
*/