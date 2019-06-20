#pragma once
#pragma warning(disable:4996)
#include "sqliteFunc.h"

std::mutex mtx;
std::condition_variable cv;

int getRecordSize(void* notuse, int argc, char** argv, char** zaColNmae) {
	sscanf(argv[0], "%d", &recordsSize);
	records.resize(recordsSize);
	return 0;
}
int getRecords(void* notuse, int argc, char** argv, char** zaColNmae) {
	records[recordsIdx++] = make_shared<recordRow>(argv[0], argv[1]);
	return 0;
}
int verifyTableExist(void* nouse, int argc, char** argv, char** zaColName) {
	if (argv[0][0] == '0')
		isTableExisted = false;
	else
		isTableExisted = true;
	return 0;
}

void openDatabaseAndTable(sqlite3* &db, string dbName, string tblName)
{
	ASSERT(sqlite3_open(dbName.c_str(), &db) == SQLITE_OK);
	char sqlstr[200];
	sprintf(sqlstr, "select count(*) from sqlite_master where type='table' and name='%s';", tableName.c_str());
	ASSERT(sqlite3_exec(db, sqlstr, verifyTableExist, nullptr, 0) == SQLITE_OK); 
	if (!isTableExisted) {
		sprintf(sqlstr, "create table %s (id integer primary key autoincrement, num text not null, timestr text not null);", tableName.c_str());
		ASSERT(sqlite3_exec(db, sqlstr, 0, nullptr, 0) == SQLITE_OK);
	}
}

void writeRecord(sqlite3 * &db, std::string tblName, int recordNum, int recordmin, int recordsec, int recordmsec)
{
	char sqlstr[200];
	sprintf(sqlstr, "insert into %s (num, timestr) values ('No.%d', '%02d:%02d.%02d');",
		tblName.c_str(), recordNum, recordmin, recordsec, recordmsec / 10);
	string sqlstr_t = sqlstr;

	////another approch
	//std::thread thd(writeRecord_t, std::ref(db), sqlstr_t);
	//thd.detach();
	
	tasks.push(sqlstr_t);
	cv.notify_one();
}
void writeRecord_t(sqlite3 * &db, std::string sqlstr)
{
	ASSERT(sqlite3_exec(db, sqlstr.c_str(), 0, nullptr, 0) == SQLITE_OK);
}

void writeRecord_T(sqlite3* &db, queue<string> &tasks)
{
	std::unique_lock<std::mutex> lck(mtx);
	while (!isWindowClosed) {
		while (tasks.empty()) cv.wait(lck);
		while (!tasks.empty()) {
			string sqlstr = tasks.front();
			tasks.pop();
			ASSERT(sqlite3_exec(db, sqlstr.c_str(), 0, nullptr, 0) == SQLITE_OK);
		}
	}
}
