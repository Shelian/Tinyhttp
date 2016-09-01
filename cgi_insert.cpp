#include "sql_api.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

sql_api *sql=new sql_api();

void cgi_select()
{
   string head[3],data1[1024][3];
   int cur_row;
   sql->my_select(head,data1,cur_row);
   for(int i=0;i<cur_row;i++)
   {
     for(int j=0;j<3;j++)
     {
       cout<<data1[i][j]<<"&nbsp;"<<endl;
     }
     cout<<"<br>"<<endl;
   }
}
void cgi_insert(char *in)
{
  for(int i=0;i<strlen(in);i++)
	if(in[i]=='&')
	  in[i]=' ';
  cout<<"<br>"<<in<<"<br>"<<endl;
  char name[100]={0};
  char birth[100]={0};
  sscanf(in,"v1=%s v2=%s",name,birth);
  cout<<"name"<<name<<" "<<"birth"<<birth<<endl;
  string data="'";
  data+=name;
  data+="\',\'";
  data+=birth;
  data+="\'";
  //cout<<data<<endl;
  sql->my_connect();
  cout<<"<h1> your insert data:"<<data<<"</h1></br>";
  sql->my_insert(data);
  cgi_select();
}
int main()
{
  char method[255]={0};
  char query[1024]={0};
  char post_data[1024]={0};
  int length=0;
  cout<<"<html>"<<endl;
 // cout<<"hahahahahahaha1"<<endl;
  cout<<"<head><title>you are in my list now!</title></head>"<<endl;
  cout<<"<boby>"<<endl;
  
  strcpy(method,getenv("REQUEST_METHOD"));
  //cout<<method<<endl;
  //cout<<"hahahahahahaha3"<<endl;
  if(strcasecmp("GET",method)==0)
  {
    strcpy(query,getenv("QUERY_STRING"));
    cout<<"get query:"<<query<<endl;
    cgi_insert(query);
  }
  else if(strcasecmp("POST",method)==0)
  {
    //cout<<"hahahahahahaha4"<<endl;
    length=atoi(getenv("CONTENT_LENGTH"));
    int i=0;
    for(i;i<length;i++)
    {
      read(0,post_data+i,1);
      //cout<<post_data[i]<<" ";
    }
    //cout<<endl;
    post_data[i]='\0';
    cout<<"post data:"<<post_data<<endl;
    cgi_insert(post_data);
  }
  cout<<"</boby>"<<endl;
  cout<<"</html>"<<endl;
}
