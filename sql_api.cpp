#include "sql_api.h"


sql_api::sql_api(const string &_host,\
                 const string &_user,\
                 const string &_passwd,\
                 const string &_db)
{
  //init a structure MYSQL  conn;
  //conn is a handler to operate mysql;
  //mysql_init will return a handler;
  conn=mysql_init(NULL);
  result=NULL;
  this->host=_host;
  this->user=_user;
  this->passwd=_passwd;
  this->db=_db;
}
sql_api::~sql_api()
{
  mysql_close(conn);
  if(result)
  {
    free(result);
  }
}
int sql_api::my_connect()
{
  //to build a real link to mysql 
  if(mysql_real_connect(conn,host.c_str(),user.c_str(),passwd.c_str(),db.c_str(),3306,NULL,0)<0)
  {
    cout<<"connect error"<<endl;
    return -1;
  }
  else
  {
    //cout<<"connect done..."<<endl;
    return 0;
  }
}
int sql_api::my_insert(const string &data)
{
  string sql="insert into friend (name,birth) values";
  sql+="(";
  sql+=data;
  sql+=");";
  if(mysql_query(conn,sql.c_str())==0)
  {
    //cout<<"insert success"<<endl;
    return 0;
  }
  else
  {
    //cout<<"insert failed"<<endl;
    return -1;
  }
}
void sql_api::my_select(string field_name[],string data[][3],int &row)
{
  MYSQL_FIELD *fd;
  string sql="select * from friend";
  //judge query result
  if(mysql_query(conn,sql.c_str())==0)
  {
    //cout<<"query success"<<endl;
  }
  else
  {
    //cout<<"query failed"<<endl;
  }
  //result keep the result of select
  //result will use malloc a space to keep datas
  //so it need to be freed when no need of it
  result=mysql_store_result(conn);

  //get the row_num of tables
  int row_num=mysql_num_rows(result);
  //cout<<"row"<<row_num<<endl;
  //get the column_num of tables
  int field_num=mysql_num_fields(result);
  row=row_num;
  //cout<<"column"<<field_num<<endl;

  //get the cloumn name  of column
  for(int i=0;fd=mysql_fetch_field(result);i++)
  {
   // cout<<fd->name<<" ";
    field_name[i]=fd->name;
  }
  //cout<<endl;

  //get the info of each cloumn
  for(int i=0;i<row;i++)
  {
    //rowinfo likes char**
    MYSQL_ROW rowinfo=mysql_fetch_row(result);
    if(rowinfo)
    {
      for(int j=0;j<field_num;j++)
      {
        data[i][j]=rowinfo[j];
       // cout<<data[i][j]<<"\t";
      }
     // cout<<endl;
    }
  }
}

#ifdef _DEBUG_
int main()
{
  string sql_data[100][3];
  string head[3];
  int cur_row=-1;
  string host="127.0.0.1";
  string user="root";
  string passwd="";
  string db="momo";
  const string data="'haha','1.29'";
  sql_api sql(host,user,passwd,db);
  sql.my_connect();
  sql.my_select(head,sql_data,cur_row);
  sleep(1);
  for(int i=0;i<cur_row;i++)
  {
    for(int j=0;j<3;j++)
    {
      cout<<sql_data[i][j]<<" ";
    }
    cout<<endl;
  }
  sql.my_insert(data);
  return 0;
}
#endif




