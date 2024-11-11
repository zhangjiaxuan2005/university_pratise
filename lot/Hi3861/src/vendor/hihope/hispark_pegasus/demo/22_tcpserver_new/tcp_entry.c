
#include <stdio.h>
#include <unistd.h>

#include "ohos_init.h"
#include "cmsis_os2.h"

#include "hi_wifi_api.h"
#include "lwip/ip_addr.h"
#include "lwip/netifapi.h"
#include "lwip/sockets.h"

static unsigned short int servport = 3861;
static int sockfd = -1;
static int connfd = -1;

void TcpRecvTask(void)
{
    int ret;
    unsigned char recv_buff[64] = { 0 };

    int backlog = 1;
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);  // TCP socket

    struct sockaddr_in serverAddr = {0};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(servport);          // 端口号，从主机字节序转为网络字节序
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY); // 允许任意主机接入， 0.0.0.0

    ret = bind(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)); // 绑定端口
    if (ret < 0) {
        printf("%s: bind failed, ret[%d]\n", __func__, ret);
        close(sockfd);
        return;
    }

    ret = listen(sockfd, backlog); // 开始监听
    if (ret < 0) {
        printf("%s: listen failed, ret[%d]\n", __func__, ret);
        close(sockfd);
        return;
    }

    while (1) {
        // 接受客户端连接，成功会返回一个表示连接的 socket ， clientAddr 参数将会携带客户端主机和端口信息 ；失败返回 -1
        // 此后的 收、发 都在 表示连接的 socket 上进行；之后 sockfd 依然可以继续接受其他客户端的连接，
        // UNIX系统上经典的并发模型是“每个连接一个进程”——创建子进程处理连接，父进程继续接受其他客户端的连接
        // 鸿蒙liteos-a内核之上，可以使用UNIX的“每个连接一个进程”的并发模型
        //     liteos-m内核之上，可以使用“每个连接一个线程”的并发模型
        struct sockaddr_in clientAddr = {0};
        socklen_t clientAddrLen = sizeof(clientAddr);
        connfd = accept(sockfd, (struct sockaddr *)&clientAddr, &clientAddrLen);
        if (connfd < 0) {
            printf("%s: accept failed, connfd[%d], errno[%d]\n", __func__, connfd, errno);
            //close(sockfd);
            //return;
            continue;
        }
        printf("%s: accept success, connfd[%d]\n", __func__, connfd);
        printf("%s: client info: addr[%s], port[%hu]\n", __func__,
                inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

        while (connfd > 0) {
            memset(recv_buff, 0, sizeof(recv_buff));
            ret = recv(connfd, recv_buff, sizeof(recv_buff), 0);
            if (ret > 0) {
                printf("%s: recv: [%d] %s\n", __func__, ret, recv_buff);
                
                //TODO
            }
        }
    }
}

void TcpSendTask(void)
{
    ssize_t ret = 0;
    unsigned char send_buff[] = "Hello TCP Client!";

    while (1) {
        sleep(1);
        if (connfd > 0) {
            ret = send(connfd, send_buff, strlen(send_buff), 0);
            if (ret < 0) {
                printf("%s: send failed! client disconnect\n", __func__);
                close(connfd);
                connfd = -1;
            } else {
                printf("%s: send: [%d] %s\n", __func__, ret, send_buff);
            }
        }
    }
}

void TcpServerEntry(void)
{
    //TcpSocketInit();

    osThreadAttr_t attr1 = {"TcpRecvTask", 0, NULL, 0, NULL, 1024*4, 32, 0, 0};
    if (osThreadNew((osThreadFunc_t)TcpRecvTask, NULL, &attr1) == NULL) {
        printf("[TcpServerEntry] Falied to create TcpRecvTask!\n");
    }

#if 1
    osThreadAttr_t attr2 = {"TcpSendTask", 0, NULL, 0, NULL, 1024*4, 32, 0, 0};
    if (osThreadNew((osThreadFunc_t)TcpSendTask, NULL, &attr2) == NULL) {
        printf("[TcpServerEntry] Falied to create TcpSendTask!\n");
    }
#endif
}