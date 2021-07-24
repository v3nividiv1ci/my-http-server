#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <cerrno>
#include <sys/socket.h>
#include <netinet/in.h>
#include <csignal>
#define BUFFSIZE 2048
#define DEFAULT_PORT 23333
#define MAXLINK 10
int sock_fd, conn_fd;
void stopServerRunning(int p) {
    close(sock_fd);
    printf("Close Server\n");
    exit(0);

}

int main() {
    struct sockaddr_in serv_addr; // 存放ip和端口
    char buff[BUFFSIZE]; // 用于收发数据
    sock_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);    // 创建套接字
    if(-1 == sock_fd) {
        printf("Create socket error(%d): %s\n", errno, strerror(errno));
        return -1;
    }

    // 将套接字和IP、端口绑定
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(DEFAULT_PORT);
    if (-1 == bind(sock_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr))) {
        printf("Bind error(%d): %s\n", errno, strerror(errno));
        return -1;
    }

    // 进入监听状态
    if (-1 == listen(sock_fd, MAXLINK)) {
        printf("Listen error(%d): %s\n", errno, strerror(errno));
        return -1;
    }
    while (true) {
        signal(SIGINT, stopServerRunning);
        conn_fd = accept(sock_fd, nullptr, nullptr);
        if (-1 == conn_fd) {
            printf("Accept error(%d): %s\n", errno, strerror(errno));
            return -1;
        }
        bzero(buff, BUFFSIZE);
        // 接收客户端请求
        recv(conn_fd, buff, BUFFSIZE - 1, 0);
        printf("Recv: %s\n", buff);
        if (strcmp(buff, "ping") == 0) {
            strcpy(buff, "pong");
            printf("Send: %s\n", buff);
            // 向客户端发送数据
            send(conn_fd, buff, strlen(buff), 0);
        }

        // 关闭套接字
        close(conn_fd);
    }
    return 0;
}

