#pragma once

#include "mysql.h"
#include <WinSock2.h>
#pragma comment(lib, "libmysql.lib")
#pragma comment(lib, "ws2_32.lib")

#define MY_IP "10.10.20.108"
#define DB_USER "sujeong"
#define DB_PASS ""
#define DB_NAME "testresult"

extern MYSQL_ROW Sql_Row; // mysql �� ������ �ε������� �ҷ��´�.
extern MYSQL_RES* Sql_Result; // mysql�� ���
extern MYSQL connection; // mysql�� ������ ���


