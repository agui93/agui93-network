#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#define BUFFLEN 1024
#define SERVER_PORT 8888
#define BACKLOG 5

static void handle_request(int s, struct sockaddr_in* from, char *buff)
{
	time_t now;		/*时间*/
	int len = sizeof(*from);
	memset(buff, 0, BUFFLEN);/*清零*/
	now = time(NULL);/*当前时间*/
	sprintf(buff, "%24s\r\n",ctime(&now));/*将时间拷贝入缓冲区*/
	sendto(s, buff, strlen(buff),0, (struct sockaddr*)from, len);/*发送数据*/
}

static void handle_connect(int s_s)
{	
	struct sockaddr_in from;	/*客户端地址*/
	int len = sizeof(from);
	int n = 0;
	char buff[BUFFLEN];
	/*主处理过程*/
	while(1)
	{
		memset(buff, 0, BUFFLEN);/*清零*/
		/*接收客户端连接*/
		n = recvfrom(s_s, buff, BUFFLEN,0,(struct sockaddr*)&from, &len);/*接收发送方数据*/
		if(n > 0 && !strncmp(buff, "TIME", 4))/*判断是否合法接收数据*/
		{
			/*创建进程进行数据处理*/
			if(!fork() ){
				handle_request(s_s, &from, buff);/*处理连接请求*/
				return ;	
			}
		}
	}		
}


int main(int argc, char *argv[])
{
	int s_s;	/*服务器套接字文件描述符*/
	struct sockaddr_in local;	/*本地地址*/	
	
	/*建立TCP套接字*/
	s_s = socket(AF_INET, SOCK_STREAM, 0);
	
	/*初始化地址接哦股*/
	memset(&local, 0, sizeof(local));/*清零*/
	local.sin_family = AF_INET;/*AF_INET协议族*/
	local.sin_addr.s_addr = htonl(INADDR_ANY);/*任意本地地址*/
	local.sin_port = htons(SERVER_PORT);/*服务器端口*/
	
	/*将套接字文件描述符绑定到本地地址和端口*/
	int err = bind(s_s, (struct sockaddr*)&local, sizeof(local));
		
	/*处理客户端连接*/
	handle_connect(s_s);
	
	close(s_s);
	
	return 0;		
}
