#pragma once

#include "mysql.h"
#include <WinSock2.h>
#pragma comment(lib, "libmysql.lib")
#pragma comment(lib, "ws2_32.lib")

#define MY_IP "10.10.20.108"
#define DB_USER "sujeong"
#define DB_PASS ""
#define DB_NAME "testresult"

extern MYSQL_ROW Sql_Row; // mysql 의 각행의 인덱스값들 불러온다.
extern MYSQL_RES* Sql_Result; // mysql의 결과
extern MYSQL connection; // mysql의 연결을 담당


