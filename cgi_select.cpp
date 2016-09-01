#include "sql_api.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

sql_api sql;

void cgi_select()
{
   sql.my_connect();
   string head[3],data1[1024][3];
   int cur_row;
   sql.my_select(head,data1,cur_row);
   for(int i=0;i<cur_row;i++)
   {
     for(int j=0;j<3;j++)
     {
       cout<<"<h1>"<<data1[i][j]<<"<h1>"<<"\t";
     }
     cout<<"<br>"<<endl;
   }
}
int main()
{
  char method[255]={0};
  char query[1024]={0};
  char post_data[1024]={0};
  int length=0;
  cout<<"<html>"<<endl;
  cout<<"<head><title>you are in my list now!</title></head>"<<endl;
  cout<<"<body>"<<endl;
  strcpy(method,getenv("REQUEST_MOTHED"));
  if(strcasecmp("GET",method==0))
  {
    strcpy(query,getenv("QUERY_STRING"));
    cout<<"<h1>get query:"<<query<<"</h1></br>"<<endl;
    //cgi_select();
  }
  else if(strcasecmp("POST",method)==0)
  {
    length=atoi(getenv("CONTENT_LENGTH"));
    cout<<length<<endl;
    int i=0;
    for(i;i<length;i++)
    {
      read(0,post_data+i,1);
    }
    post_data[i]='\0';
    cout<<"<h1>"<<post_data<<"</h1>"<<endl;
    //cgi_select();
  }
  cout<<"</body>"<<endl;
  cout<<"</html>"<<endl;
}
