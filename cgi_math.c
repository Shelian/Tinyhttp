#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
void add(const char *arg)
{
  
  int buf[2]={0};
  int index=0;
  int i=strlen(arg)-1;
  char end[1024]={0};
  //data1=1000&data2=2000
  if(arg==NULL||*arg==0)
  { 
    exit(1);
  }
  strcpy(end,arg);
  //参数解析
  while(i>=0&&index<2)
  {
    printf("%c\n",end[i]);
    printf("%s\n",end);
    if(end[i]=='=')
    {
     buf[index]=atoi(end+i+1);
     printf("<br><h1>v=%d</br></h1>\n",buf[index]);
     index++;
    }
    if(end[i]=='&')
    {
      end[i]='\0';
    }
    i--;
  }
  printf("<br><h1>v1=%d</br></h1>\n",buf[0]);
  printf("<br><h1>v2=%d</br></h1>\n",buf[1]);
  int ret=buf[0]+buf[1];
  printf("<br><h1>v1+v2=%d</br></h1>\n",ret);
}
int main()
{
  char method[255]={0};
  char query[1024]={0};
  char post_data[1024]={0};
  int length=0;
  printf("<html>\n");
  printf("<head><title>victory!</title><head>\n");
  printf("<boby>\n");
  strcpy(method,getenv("REQUEST_METHOD"));
   if(strcasecmp("GET",method)==0)
  {
    strcpy(query,getenv("QUERY_STRING"));
    printf("<h1>query:%s</h1>\n",query);
    add(query);
  }
  else if(strcasecmp("POST",method)==0) 
  {
    length=atoi(getenv("CONTENT_LENGTH"));
    int i;
    for(i=0;i<length;i++)
   {
       read(0,post_data+i,1);
    }
     post_data[i]='\0';
     printf("<h1>post_data:%s</h1>\n",post_data);
     char *str="v1=1&v2=2";
      
     add(post_data);
   }
  printf("</boby>\n");
  printf("</html>\n");
}
