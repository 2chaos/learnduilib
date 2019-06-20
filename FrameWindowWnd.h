#pragma once
#include "stdafx.h"

using namespace DuiLib;

// 窗口实例及消息响应部分
class CFrameWindowWnd : public WindowImplBase //public CWindowWnd, public INotifyUI
{
public:
	CFrameWindowWnd() { };
	virtual LPCTSTR GetWindowClassName() const { return _T("UIMainFrame"); };
	virtual CDuiString GetSkinFile() { return _T("..//stopwatch.xml"); }
	virtual CDuiString GetSkinFolder() { return _T(""); }
	UINT GetClassStyle() const { return UI_CLASSSTYLE_FRAME | CS_DBLCLKS; };
	void OnFinalMessage(HWND /*hWnd*/) { delete this; };
	void Reset();
	void Count();
	void Start();
	void Stop();
	void correctTime(int& min, int& sec, int& msec);
	void InitWindow();
	void Notify(TNotifyUI& msg);
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
public:
	CTextUI* pText;
	CButtonUI *pBtnLeft, *pBtnRight;
	CListUI* pList;
	CListTextElementUI* pListElement;
	SYSTEMTIME time_exist, time_beg, time_now;
	struct cusTime {
		int min, sec, msec;
	}show, lastShow, delta;
	enum button_state {
		up = 1,
		down = 2,
		enable = 4,
		disable = 8
	}btnLeft_state, btnRight_state;
	int recordNum = 0;
};
