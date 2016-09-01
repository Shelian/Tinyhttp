#include <stdio.h>
#include <string.h>
#include <stdlib.h>
void add(const char * arg)
{
  int buf[2]={0};
  int index=0;
  char * end=arg;
  printf("%s\n",arg);
  *end=0;
  //data1=1000&data2=2000
  if(arg==NULL||*arg==0)
  { 
    exit(1);
  }
  //参数解析
 while((end>=arg)&&(index<2))
 {
  printf("%c\n",*end);
  printf("%s\n",arg);
   if(*end=='=')
   {
    buf[index]=atoi(end+1);
    printf("v1=%d\n",buf[index]);
    index++;
    }
   if(*end=='&')
    {
	//当外面传入的arg是字符串数组才可以将*end=0 不然是字符串*end=0会出现
      *end='\0';
    }
   end--;
  }
  printf("v1=%d\n",buf[0]);
  printf("v2=%d\n",buf[1]);
  int ret=buf[0]+buf[1];
  printf("v1+v2=%d\n",ret);
}

int main()
{
	char str[]="data1=1000&data2=2000";
        add(str);
}
