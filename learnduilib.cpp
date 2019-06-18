// Duilib 使用设置部分
#pragma once
#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_DEPRECATE
#include <windows.h>
#include <objbase.h>
#include "DuiLib\UIlib.h"
using namespace DuiLib;
#ifdef _DEBUG
# ifdef _UNICODE
# pragma comment(lib, "lib\\DuiLib_ud.lib")
# else
# pragma comment(lib, "lib\\DuiLib_d.lib")
# endif
#else
# ifdef _UNICODE
# pragma comment(lib, "lib\\DuiLib_u.lib")
# else
# pragma comment(lib, "lib\\DuiLib.lib")
# endif
#endif


HWND hWnd;
#define ID_Timer 1
// 窗口实例及消息响应部分
class CFrameWindowWnd : public WindowImplBase //public CWindowWnd, public INotifyUI
{
public:
	CFrameWindowWnd() { };
	virtual LPCTSTR GetWindowClassName() const { return _T("UIMainFrame"); };
	virtual CDuiString GetSkinFile() { return _T("..//stopwatch.xml"); }
	virtual CDuiString GetSkinFolder() { return _T(""); }
	void InitTextBtnTimeListNum() {
		pText->SetText(_T("00:00.00"));
		btnLeft_state = (button_state)(up | disable);
		pBtnLeft->SetEnabled(false);
		pBtnLeft->SetText(_T("计次"));
		btnRight_state = up;
		time_exist.wMinute	= time_exist.wSecond	= time_exist.wMilliseconds	= 0;
		time_beg.wMinute	= time_beg.wSecond		= time_beg.wMilliseconds	= 0;
		time_now.wMinute	= time_now.wSecond		= time_now.wMilliseconds	= 0;
		lastShow_min		= lastShow_sec			= lastShow_msec				= 0;
		pList->RemoveAll();
		num = 0;
	}
	inline void correctTime(int& min, int& sec, int& msec) {
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
	void InitWindow() {
		pText		= static_cast<CTextUI*>(m_PaintManager.FindControl(_T("timeNow")));
		pBtnLeft	= static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btnLeft")));
		pBtnRight	= static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btnRight")));
		pList		= static_cast<CListUI*>(m_PaintManager.FindControl(_T("timeHistory")));
		InitTextBtnTimeListNum();
	}
	UINT GetClassStyle() const { return UI_CLASSSTYLE_FRAME | CS_DBLCLKS; };
	void OnFinalMessage(HWND /*hWnd*/) { delete this; };
	void Notify(TNotifyUI& msg)
	{
		if (msg.sType == _T("click")) {
			if (msg.pSender->GetName() == _T("closebtn")) {
				::PostQuitMessage(0); 
			}
			else if (msg.pSender->GetName() == _T("btnLeft")) {
				if (btnLeft_state & down) {							//复位
					InitTextBtnTimeListNum();
				}
				else if (btnLeft_state == (up | enable)) {			//计次		
					lastShow_min	= show_min;
					lastShow_sec	= show_sec;
					lastShow_msec	= show_msec;
					pListElement	= new CListTextElementUI();
					pList->Add(pListElement);
					++num;
				}				
			}
			else if (msg.pSender->GetName() == _T("btnRight")) {	
				if (btnRight_state & up) {							//启动
					GetLocalTime(&time_beg);
					if (btnLeft_state & disable)
						pBtnLeft->SetEnabled(true);				
					btnLeft_state = (button_state )(up | enable);
					pBtnLeft->SetText(_T("计次"));
					btnRight_state = down;
					pBtnRight->SetText(_T("停止"));
					if (pList->GetCount() == 0) {
						pListElement = new CListTextElementUI();
						pList->Add(pListElement);
						++num;
					}
					SetTimer(hWnd, ID_Timer, 10, nullptr);
				}
				else if (btnRight_state & down) {					//停止
					btnRight_state = up;
					KillTimer(hWnd, ID_Timer);
					time_exist.wMinute			= show_min;
					time_exist.wSecond			= show_sec;
					time_exist.wMilliseconds	= show_msec;
					pBtnLeft->SetText(_T("复位"));
					btnLeft_state = down;
					pBtnRight->SetText(_T("启动"));
				}
			}
		}
	}
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
		if (uMsg == WM_TIMER) {
			if (btnRight_state & down) {
				GetLocalTime(&time_now);
				show_min	= time_exist.wMinute + time_now.wMinute - time_beg.wMinute;
				show_sec	= time_exist.wSecond + time_now.wSecond - time_beg.wSecond;
				show_msec	= time_exist.wMilliseconds + time_now.wMilliseconds - time_beg.wMilliseconds;
				correctTime(show_min, show_sec, show_msec);

				deltaMin	= show_min - lastShow_min;
				deltaSec	= show_sec - lastShow_sec;
				deltaMsec	= show_msec - lastShow_msec;
				correctTime(deltaMin, deltaSec, deltaMsec);

				TCHAR recLeft[5], recRight[10];
				wsprintf(recLeft, _T("计次%d"), num);
				wsprintf(recRight, _T("%02d:%02d.%02d"), deltaMin, deltaSec, deltaMsec / 10);
				pListElement->SetText(0, recLeft);
				pListElement->SetText(1, recRight);		
				
				TCHAR time_str[10];
				wsprintf(time_str, _T("%02d:%02d.%02d"), show_min, show_sec, show_msec / 10);
				pText->SetText(time_str);
			}
			return 0;
		}
		else return __super::HandleMessage(uMsg, wParam, lParam);
	}
public:
	CPaintManagerUI m_pm;

	CTextUI* pText;
	CButtonUI *pBtnLeft, *pBtnRight;
	CListUI* pList;
	CListTextElementUI* pListElement;
	SYSTEMTIME time_exist, time_beg, time_now;
	int show_min, show_sec, show_msec;
	int lastShow_min, lastShow_sec, lastShow_msec;
	int deltaMin, deltaSec, deltaMsec;
	int num = 0;
	enum button_state {
		up = 1,
		down = 2,
		enable = 4,
		disable = 8
	}btnLeft_state, btnRight_state;
};
// 程序入口及 Duilib 初始化部分
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR
	/*lpCmdLine*/, int nCmdShow)
{
	CPaintManagerUI::SetInstance(hInstance);
	CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath());

	HRESULT Hr = ::CoInitialize(NULL);
	if (FAILED(Hr)) return 0;

	CFrameWindowWnd* pFrame = new CFrameWindowWnd();
	if (pFrame == NULL) return 0;
	hWnd = pFrame->Create(NULL, _T("StopWatch"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
	pFrame->CenterWindow();
	pFrame->ShowWindow(true);
	CPaintManagerUI::MessageLoop();

	::CoUninitialize();
	return 0;
}