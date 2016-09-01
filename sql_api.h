#pragma once

#include <iostream>
#include "mysql.h"
using namespace std;

class sql_api
{
public:
  sql_api(const string &_host="127.0.0.1",\
          const string &_user="root",\
          const string &_passwd="",\
          const string &_db="momo");
  int my_connect();
  int my_insert(const string &data);
  void my_select(string field_name[],string data[][3],int &row);
  //int my_update();
  //int my_delete();
  ~sql_api();
private:
  MYSQL *conn;
  string host;
  string user;
  string passwd;
  string db;
  MYSQL_RES* result;
};
