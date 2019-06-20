#pragma once
#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_DEPRECATE
#include <windows.h>
#include <objbase.h>
#include <stdlib.h>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <string>
#include "DuiLib\UIlib.h"
#include "sqlite3.h"
# pragma comment(lib,"lib\\sqlite3.lib")
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
using namespace DuiLib;

constexpr auto ID_Timer = 1;
struct recordRow {
	string num;
	string timestr;
	recordRow(string _num, string _timestr) :num(_num), timestr(_timestr) {}
};

extern HWND hWnd;
extern sqlite3* recHistory;
extern bool isTableExisted;
extern bool isWindowClosed;
extern vector<shared_ptr<recordRow>> records, newRecords;
extern int recordsIdx;
extern int recordsSize;
extern string tableName;
extern std::queue<string> tasks;