
#include <stdio.h>
#include <unistd.h>

#include "ohos_init.h"
#include "cmsis_os2.h"

#include "hi_wifi_api.h"
#include "lwip/ip_addr.h"
#include "lwip/netifapi.h"
#include "lwip/sockets.h"

static unsigned short int servPort = 3861;
static char *servIP = "192.168.1.3";

void UdpClientTask(void)
{
    ssize_t ret;
    unsigned char recv_buff[64] = { 0 };
    unsigned char send_buff[] = "Hello OpenHarmony!";

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0); // UDP socket

    struct sockaddr_in toAddr = {0};
    toAddr.sin_family = AF_INET;
    toAddr.sin_port = htons(servPort);       // 端口号，从主机字节序转为网络字节序
    if (inet_pton(AF_INET, servIP, &toAddr.sin_addr) <= 0) { // 将主机IP地址从“点分十进制”字符串 转化为 标准格式（32位整数）
        printf("UdpClientTask: inet_pton failed!\n");
        return;
    }

    while (1) {
        // UDP socket 是 “无连接的” ，因此每次发送都必须先指定目标主机和端口，主机可以是多播地址
        ret = sendto(sockfd, send_buff, sizeof(send_buff), 0, (struct sockaddr *)&toAddr, sizeof(toAddr));
        if (ret < 0) {
            printf("UdpClientTask: sendto failed!\n");
            continue;
        }
        printf("UdpClientTask: sendto: %s \r\n", send_buff);
  
        struct sockaddr_in fromAddr = {0};
        socklen_t fromLen = sizeof(fromAddr);
        // UDP socket 是 “无连接的” ，因此每次接收时前并不知道消息来自何处，通过 fromAddr 参数可以得到发送方的信息（主机、端口号）
        ret = recvfrom(sockfd, &recv_buff, sizeof(recv_buff), 0, (struct sockaddr *)&fromAddr, &fromLen);
        if (ret > 0) {
            recv_buff[ret] = '\0';
            char* clientIP = inet_ntoa(fromAddr.sin_addr);
            printf("UdpClientTask: recvFrom: %s-%d(%d): [%d] %s\n",
                clientIP, ntohs(fromAddr.sin_port), fromAddr.sin_port, ret, recv_buff);
        }
    }
}

void UdpClientEntry(void)
{
    osThreadAttr_t attr1 = {"UdpClientTask", 0, NULL, 0, NULL, 1024*4, 32, 0, 0};
    if (osThreadNew((osThreadFunc_t)UdpClientTask, NULL, &attr1) == NULL) {
        printf("[UdpClientEntry] Falied to create UdpClientTask!\n");
    }
}