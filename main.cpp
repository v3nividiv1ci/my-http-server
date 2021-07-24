#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <cerrno>
#include <sys/socket.h>
#include <netinet/in.h>
#include <csignal>
#include <sys/stat.h>

#define BUFFSIZE 2048
#define DEFAULT_PORT 23333
#define MAXLINK 10
int sock_fd, conn_fd;
void stopServerRunning(int p) {
    close(sock_fd);
    printf("Close Server\n");
    exit(0);

}

int getLine(int sock, char *buf, size_t size) {
    int i = 0;
    char c = '\0';
    int n;

    while ((i < size - 1) && (c != '\n')) {
        n = recv(sock, &c, 1, 0); // 每次向c中读一个字节并在tcp缓冲区中删除
        if (n > 0) {
            if (c == '\r') {
                n = recv(sock, &c, 1, MSG_PEEK);
                if((n > 0) && (c == '\n')) {
                    recv(sock, &c, 1, 0);
                } else {
                    c = '\n';
                }
            }
            buf[i] = c;
            i ++;
        } else {
            c = '\n';
        }
    }
    buf[i] = '\0';
    return i;
}

void acceptRequest(int client_socket) {
    char buf[1024] = {0}; // 读取请求行
    int numchars = 0;
    char method[255]; // 请求方法
    char url[255] = {0}; // 请求url
    char path[512] = {0};
    size_t i = 0, j = 0;
//    stat 函数用来获取指定文件夹的信息
    struct stat st;
    char *query_string = nullptr;
//    打印报文
    while(1)
    {
        numchars = getLine(client_socket, buf, sizeof(buf));
        if (numchars == 0) {
            break;
        }
            printf("%s\n", buf);
            memset(buf, 0, sizeof(buf));
    }


}

int main() {
    struct sockaddr_in serv_addr, client_addr; // 存放ip和端口
    char buff[BUFFSIZE]; // 用于收发数据
    sock_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);    // 创建套接字
    int clen = sizeof(client_addr);
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
//        signal(SIGINT, stopServerRunning);
        conn_fd = accept(sock_fd, (struct sockaddr*)&client_addr, (unsigned int *)&clen);
        if (-1 == conn_fd) {
            printf("Accept error(%d): %s\n", errno, strerror(errno));
            return -1;
        }
        acceptRequest(conn_fd);
//      关闭套接字
//      close(conn_fd);
    }
    close(sock_fd);
    return 0;
}

