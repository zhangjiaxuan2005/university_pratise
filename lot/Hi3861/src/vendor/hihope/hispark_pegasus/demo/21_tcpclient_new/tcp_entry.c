
#include <stdio.h>
#include <unistd.h>

#include "ohos_init.h"
#include "cmsis_os2.h"

#include "hi_wifi_api.h"
#include "lwip/ip_addr.h"
#include "lwip/netifapi.h"
#include "lwip/sockets.h"

static unsigned short int servport = 3861;
static char *servIP = "192.168.43.142";

static int sockfd = -1;
static int connfd = -1;

void TcpClientTask(void)
{
    int ret;
    unsigned char recv_buff[64] = { 0 };
    unsigned char send_buff[] = "Hello OpenHarmony!";

    while (1) {
        sockfd = socket(AF_INET, SOCK_STREAM, 0); // TCP socket

        struct sockaddr_in serverAddr = {0};
        serverAddr.sin_family = AF_INET;        // AF_INET表示IPv4协议
        serverAddr.sin_port = htons(servport);  // 端口号，从主机字节序转为网络字节序
        // 将主机IP地址从“点分十进制”字符串 转化为 标准格式（32位整数）
        if (inet_pton(AF_INET, servIP, &serverAddr.sin_addr) <= 0) {
            printf("%s: inet_pton failed!\n", __func__);
            close(sockfd);
            return;
        }

        // 尝试和目标主机建立连接，连接成功会返回0 ，失败返回 -1
        connfd = connect(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
        if (connfd != 0) {
            printf("%s: connect to server [%s][%d] failed!\n", __func__, servIP, servport);
            close(sockfd);
            sleep(1);
            continue;
        }
        printf("%s: connect to server [%s][%d] success!\n", __func__, servIP, servport);
        // 建立连接成功之后，这个TCP socket描述符 —— sockfd 就具有了 “连接状态”，
        // 发送、接收 对端都是 connect 参数指定的目标主机和端口

        while (connfd == 0) {
            ret = send(sockfd, send_buff, strlen(send_buff), 0);
            if (ret > 0) {
                printf("%s: send: [%d] %s\n", __func__, ret, send_buff);
            } else {
                printf("%s: send failed! client disconnect\n", __func__);
                connfd = -1;
                break;
            }

            memset(recv_buff, 0, sizeof(recv_buff));
            ret = recv(sockfd, &recv_buff, sizeof(recv_buff), 0);
            if (ret > 0) {
                printf("%s: recv: [%d] %s\n", __func__, ret, recv_buff);
            } else {
                printf("%s: recv failed! client disconnect\n", __func__);
                connfd = -1;
                break;
            }
        }
    }
}

void TcpClientEntry(void)
{
    osThreadAttr_t attr1 = {"TcpClientTask", 0, NULL, 0, NULL, 1024*4, 32, 0, 0};
    if (osThreadNew((osThreadFunc_t)TcpClientTask, NULL, &attr1) == NULL) {
        printf("[TcpServerEntry] Falied to create TcpClientTask!\n");
    }
}