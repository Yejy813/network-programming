#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

#define BUF_SIZE 1024

void error_handling(char* message);

int main(int argc, char* argv[])
{
    int sock;
    char message[BUF_SIZE];
    int str_len;
    struct sockaddr_in serv_addr;

    if(argc != 3)
    {
        printf("Usage : %s <IP> <port>\n", argv[0]);
        exit(1);        
    }

    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        error_handling("socket() error");       
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));

    if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
    {
        error_handling("connect() error");        
    }
    else
    {
        puts("connected.........");
    }

    while(1)
    {
        printf("Input message(Q to quit): ");
        fgets(message, BUF_SIZE, stdin);

        if(!strcmp(message, "q\n") || !strcmp(message, "Q\n"))
        {
            break;
        }

        // 调用完 write，然后立即调用 read，有可能只读取到部分数据
        // 服务器调用 1 次 write 函数传输数据，如果数据太大，操作系统就有可能把数据
        // 分成多个数据包发送到客户端。另外在此过程中，客户端有可能在尚未收到全部数据
        // 时就调用 read 函数， ch5 中有优化代码，这里没有发生错误是因为数据量小，且在
        // 同一台设备或者相邻设备运行
        write(sock, message, strlen(message));
        str_len = read(sock, message, BUF_SIZE - 1);

        message[str_len] = 0;
        printf("Message from server: %s", message);
    }

    close(sock);
    return 0;
}

void error_handling(char* message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1); 
}