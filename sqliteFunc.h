#pragma once
#include "stdafx.h"

// sqlite»Øµ÷º¯Êý
int getRecordSize(void* notuse, int argc, char** argv, char** zaColNmae);
int getRecords(void* notuse, int argc, char** argv, char** zaColNmae);
int verifyTableExist(void* nouse, int argc, char** argv, char** zaColName);

void openDatabaseAndTable(sqlite3*& db, std::string dbName, std::string tblName);
void writeRecord(sqlite3* &db, std::string tblNmae, int recordNum, int recordmin, int recordsec, int recordmsec);
void writeRecord_t(sqlite3 * &db, std::string sqlstr);
void writeRecord_T(sqlite3* &db, std::queue<string> &tasks);