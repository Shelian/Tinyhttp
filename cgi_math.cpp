#include<iostream>
using namespace std;
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<stdio.h>

void calc_add(const char* arg)
{
	if(arg==NULL || *arg=='\0')
		{
		cout<<"<p>error args<br>"<<endl;
		exit(1);
}
	int v1,v2;
	sscanf(arg,"v1=%d&v2=%d",&v1,&v2);
	cout<<"<p>Value:"<<v1+v2<<"<br>"<<endl;
}

int main()
{
	char method[255]={0};
	char query[1024]={0};
	char post_data[1024]={0};
	int length=0;
	cout<<"<html>"<<endl;
	cout<<"<h1>CALC <br/>"<<endl;
	
	strcpy(method,getenv("REQUEST_METHOD"));
	//cout<<"<p> method "<<method<<"<br/>"<<endl;
	if(strcasecmp("GET",method)==0)
	{
		strcpy(query,getenv("QUERY_STRING"));
		//cout<<"args= "<<query<<"<br/>"<<endl;
		calc_add(query);
	}
	else if(strcasecmp("POST",method)==0)
	{
		length=atoi(getenv("CONTENT_LENGTH"));
		int i;
		for(i=0;i<length;i++)
			read(0,post_data+i,1);
		post_data[i]='\0';
		//cout<<"args= "<<post_data<<"<br/>"<<endl;
		calc_add(post_data);
	}

	cout<<"</html>"<<endl;
	return 0;
}

