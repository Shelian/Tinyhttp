#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <stdlib.h>
#define _SIZE_ 1024

//提示命令行需要输入的参数
static void usage(const char *proc)
{
  printf("%s [ip] [port]\n",proc);
}

//建立连接
static int startup(const char *ip,int port)
{
  printf("port:%d\n",port);
  //创建套接字
  int sock=socket(AF_INET,SOCK_STREAM,0);
  if(sock<0)
  {
    perror("socket");
    return 2;
  }
  //端口复用
  int opt=1;
  setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
  //绑定容器local与套接字sock
  struct sockaddr_in local;
  local.sin_family=AF_INET;
  local.sin_port=htons(port);
  local.sin_addr.s_addr=inet_addr(ip);
  if(bind(sock,(struct sockaddr *)&local,sizeof(local))<0)
  {
    perror("bind");
    return 3;
  }
  //监听套接字
  if(listen(sock,5)<0)
  {
    perror("listen");
    return 4;
  }
  return sock;
}
//按照行的方式读取一行
int get_line(int sock,char buf[],int len)
{

 if(buf==NULL||sock<0||len<=0)
 {
   return  -1;
 }
 int j=0;
 char c=0;
 while(j<len-1&& c!='\n')
 {

   int n=recv(sock,&c,1,0);
   //没有读到东西
   if(n<0)
   {
     c='\n';
   }
   else
   {
      //读到/r/n处理成\n，读到\r处理成\n
      if(c=='\r')
      {
        n=recv(sock,&c,1,MSG_PEEK);
        if(c=='\n')
         n=recv(sock,&c,1,0);
        else
          c='\n';
      }
   }
   buf[j++]=c;
  }
 buf[j]='\0';
 return j;
}
static void echo_errno(int sock,int err)
{
 // char buf[1024];
 // switch(err)
 // {
 //   case 400:
 //     sprintf(buf,"HTTP/1.1 400 Bad Request\r\n\r\n");
 //     break;
 //   case 404:
 //     sprintf(buf,"HTTP/1.1 400 Not Found\r\n\r\n");
 //     break;
 //   case 500:
 //     sprintf(buf,"HTTP/1.1 500 Internal Server Error\r\n\r\n");
 //     break;
 // }
 // send(sock, buf, strlen(buf), 0);
 // sprintf(buf, "Content-Type: text/html\r\n\r\n");
 // send(sock, buf, strlen(buf), 0);
 // switch(err)
 // {
 // case 400:
 // sprintf(buf,"<HTML><TITLE>Bad Request</TITLE>\n<BODY><h1>bad request</h1></BODY>\n");
 //   break;
 // case 404:
 // sprintf(buf,"<HTML><TITLE>Not Found</TITLE>\n                                   \n");
 //   break;
 // case 500:
 // sprintf(buf,"<HTML><TITLE>Internal Server Error</TITLE>\n");
 //   break;
 // }
 // send(sock,buf, strlen(buf), 0);
 // sprintf(buf,"</HTML>\r\n");
 // send(sock,buf, strlen(buf), 0);
 return;

}
static void echo_www(int sock,const char *path,int size)
{
  char buf[_SIZE_];
  if(sock<0||path==NULL||size<=0)
  {
    return;
  }
  int fd=open(path,O_RDONLY);
  if(fd<0)
  {
    //echo_errno(sock,404);
    return;
  }
  printf("server path :%s\n",path);
  memset(buf,'\0',sizeof(buf));  
  sprintf(buf,"HTTP/1.1 200 0K\r\n\r\n");//普通http请求，get方法过来直接将服务器的文件返回到浏览器中
  send(sock,buf,strlen(buf),0);
  //把fd中的发给sock
  if(sendfile(sock,fd,NULL,size)<0)
  {
    // echo_errno(sock,404);
     return;
  }
  close(fd);
}

static void clear(int sock)
{
  char buf[_SIZE_];
  while((get_line(sock,buf,sizeof(buf))>0)&&(strcmp(buf,"\n")!=0));
  printf("DONE!\n");
}

int exec_cgi(int sock,const char *path,const char *method,const char *query)
{
  //printf("path:%s\n",path);
  //printf("query:%s\n",query);
  char buf[_SIZE_]={0};
  int num=-1;
  int length=0;
  int cgi_in[2];
  int cgi_out[2];
  if(strcasecmp("GET",method)==0)
  {
    //printf("cgi1---get\n");
    //清空接受缓存区中的内容
    clear(sock);
  }
  else
  {
    //post方法解析content_length
    num=get_line(sock,buf,sizeof(buf));
    while(num>0&&strcmp("\n",buf)!=0)
    {
         //strcasecmp只能忽略大小写
	   //Content-Length: 9要变成Content-Length:'\0'9
       buf[15]='\0';
      if(strcasecmp(buf,"Content-Length:")==0)
        {
          length=atoi(buf+16);
          printf("content_length:%d\n",length);
        }
        num=get_line(sock,buf,sizeof(buf));
    }
    printf("length=%d\n",length);
    if(length==0)
    {
      return -1;
    }
  }
  memset(buf,'\0',sizeof(buf));
  strcpy(buf,"HTTP/1.1 200 OK\r\n\r\n");
  send(sock,buf,strlen(buf),0);
  if(pipe(cgi_in)==-1)
  {
    return -1;
  }
  if(pipe(cgi_out)==-1)
  {
    return -1;
  }
  //创建子进程
  int pid=fork();
  if(pid<0)
  {
    return -1;
  }
  else if(pid==0)//child process
  {
      
      //cgi端
      //stdin------->in[0]
      //stdout------>out[1]
      close(cgi_in[1]);
      close(cgi_out[0]);
      dup2(cgi_in[0],0);
      dup2(cgi_out[1],1);
      char env_method[1024]={0};
      char env_query[1024]={0};
      char env_length[1024]={0};
      sprintf(env_method,"REQUEST_METHOD=%s",method);
      putenv(env_method);
      if(strcasecmp("GET",method)==0)
      {
        sprintf(env_query,"QUERY_STRING=%s",query);
        putenv(env_query);
      }
      else
      {
        sprintf(env_length,"CONTENT_LENGTH=%d",length);
        putenv(env_length);
      }
     execl(path,path,NULL);
     exit(0);
  }
  else
  {
    //服务器端
    close(cgi_in[0]);
    close(cgi_out[1]);
    int i=0;
    char ch=0;
    if(strcasecmp("POST",method)==0)
    {
        //从消息正文中获得参数写入cgi_in[1]----stdin
        for(i;i<length;i++)
        {
          recv(sock,&ch,1,0);
          write(cgi_in[1],&ch,1);
        }
    }
    //从stdout读数据发送给sock
    while(read(cgi_out[0],&ch,1)>0)
    {
        send(sock,&ch,1,0);
    }
   close(cgi_in[1]);
   close(cgi_out[0]);
    wait(pid,NULL,0);
  }
  return 0;
}
//收到请求
void *accept_request(void *arg)
{
   int sock=(int)arg;
   char buf[_SIZE_];
   char method[_SIZE_];
   char url[_SIZE_];
   char path[_SIZE_];
   char *query=url;
   int cgi=0;
   int ret= -1;
   int i=0;   //buf下标
   int j=0;   //其他解析段下标
//调试
#ifdef _DEBUG_
   do{
     ret=get_line(sock,buf,sizeof(buf));
     printf("%s",buf);
     fflush(stdout);
   }while((ret>0)&&(strcmp(buf,"\n")!=0));
#endif
   //读取请求行存储在buf中
   if(get_line(sock,buf,sizeof(buf))<0)
   {
      //echo_errno(sock,500);
      return;
   }
   printf("request line:%s\n",buf);
   //解析方法
   while(!isspace(buf[i])&&(i<sizeof(method)-1))
   {
      method[j++]=buf[i++];
   }
   method[j]='\0';

   //去除空格
   while(isspace(buf[i])&&(i<sizeof(buf)))
   {
     i++;
   }
  // printf("AAAA%c\n",buf[i]);
   //读取url
   j=0;
   while(!isspace(buf[i])&&(i<sizeof(buf))&&(j<sizeof(url)-1))
   {
      url[j++]=buf[i++];
   }

   url[j]='\0';
   //如果不是post也不是get方法-就不处理直接返回
   if(strcasecmp(method,"GET")&&strcasecmp(method,"POST"))
   {
      //echo_errno(sock,400);
      return;
   }
   //是post，cgi处理
   if(strcasecmp(method,"POST")==0)
   {
      cgi=1;
   }
   //是get
   //如果是get，并且url中有？,那么就是path+参数的形式---cgi
   //否则就是一般模式
   if(strcasecmp(method,"GET")==0)
   {
     //从url中判断path，并且看有没有'?',确定有没有参数，是不是cgi
     while(*query!='\0')
     {
        if(*query=='?')
        {
           cgi=1;
           *query='\0';
           query++;
           break;
        }
        query++;
      }
    }
  // printf("AAAAA:%s\n",url);
    //解析到path
    //将需要访问的网页放入htdoc文件夹中，如果路径是/--->htdoc/index.html
    sprintf(path,"htdoc%s",url);
    //path="htdoc/"
    // printf("AAAAA:%s\n",path);
    if(strlen(path)==1+strlen("htdoc"))
    {
       strcat(path,"index.html");
    }
    //printf("path:%s\n",path);
    //获取文件的结构体
    struct stat st;
    if(stat(path,&st)<0)
    {
      clear(sock);
      return;
    }
    else
    {
        //判断是不是非/目录的其他目录，让其指向根目录
        //S_IFMT:文件类型的位遮罩
        //S_IXUSR:文件所有者具有可执行权限
        //S_IXOTH:其他用户具有可执行权限
        //S_IXGRP:用户组具有可执行权限
      if(S_ISDIR(st.st_mode))
        {
            strcat(path,"/");
		        printf("path:%s\n",path);
            strcat(path,"index.html");
	        	printf("path:%s\n",path);
        }
        else if(st.st_mode&S_IXUSR||st.st_mode&S_IXOTH||st.st_mode&S_IXGRP)
        {
            cgi=1;
        }
        else
        {
          //echo_errno(sock,400);
        }
        if(cgi)
        {
          printf("exec_cgi\n");
          exec_cgi(sock,path,method,query);
          printf("exec_cgi_end\n");
        }
        else
        {
          clear(sock);
          echo_www(sock,path,st.st_size);
        }
    }
    close(sock);
    return (void *)0;
}

int main(int argc,char *argv[])
{
  if(argc!=3)
  {
    usage(argv[0]);
    return 1;
  }
  int listen_sock=startup(argv[1],atoi(argv[2]));
  struct sockaddr_in peer;
  socklen_t len=sizeof(peer);
  while(1)
  {
    printf("AAAAAA\n");
    int new_sock=accept(listen_sock,(struct sockaddr *)&peer,&len);
    printf("%d\n",new_sock);
    if(new_sock<0)
    {
      perror("accept");
      return 5;
    }
    else 
    {
      printf("debug:client socket: %s :%d \n",inet_ntoa(peer.sin_addr),ntohs(peer.sin_port));
      printf("new_sock:%d\n",new_sock);
      pthread_t tid;
      pthread_create(&tid,NULL,accept_request,(void *)new_sock);
      pthread_detach(tid);
    }
  }
  
}
