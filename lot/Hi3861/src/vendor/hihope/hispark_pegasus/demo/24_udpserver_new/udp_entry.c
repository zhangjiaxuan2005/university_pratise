
#include <stdio.h>
#include <unistd.h>

#include "ohos_init.h"
#include "cmsis_os2.h"

#include "hi_wifi_api.h"
#include "lwip/ip_addr.h"
#include "lwip/netifapi.h"
#include "lwip/sockets.h"

static struct sockaddr_in clntAddr;
static int sockfd;

void UdpSocketInit(void)
{
    sockfd = socket(PF_INET, SOCK_DGRAM, 0);
 
    static unsigned short int servPort = 3861;  //服务器的监听端口
    struct sockaddr_in addrServ;                //服务器的地址信息
    bzero(&addrServ, sizeof(struct sockaddr_in));
    addrServ.sin_family = AF_INET;
    addrServ.sin_port = htons(servPort);
    addrServ.sin_addr.s_addr = htonl(INADDR_ANY);  //即inet_addr("0.0.0.0")  //即服务器自己的的IP地址

    bind(sockfd, (struct sockaddr *)&addrServ, sizeof(struct sockaddr_in));
    printf("UdpSocketInit: sockfd[%d] \r\n", sockfd);

    //开启广播功能
    int opt = 1;
    int ret = setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &opt, sizeof(opt)); 
    printf("UdpSocketInit: setsockopt ret[%d] \r\n", ret);
}

void UdpRecvTask(void)
{
    int ret;
    unsigned char recv_buff[64] = { 0 };
    int lenAddr = sizeof(struct sockaddr_in);
    unsigned char reply_buff[] = "Reply Message!";

    while (1) {
        memset(recv_buff, 0, sizeof(recv_buff));
        ret = recvfrom(sockfd, recv_buff, sizeof(recv_buff), 0, (struct sockaddr*)&clntAddr, (socklen_t*)&lenAddr);
        if (ret > 0) {
            printf("UdpRecvTask: recvFrom: %s-%d(%d): [%d] %s\n",
                inet_ntoa(clntAddr.sin_addr), ntohs(clntAddr.sin_port), clntAddr.sin_port, ret, recv_buff);

            #if 1
            //clntAddr记录了向server发送过消息的最后一个client的身份信息(包含了IP和port)
            //这里只对该clntAddr单播即reply
            ret = sendto(sockfd, reply_buff, sizeof(reply_buff), 0, (struct sockaddr *)&clntAddr, sizeof(struct sockaddr_in));
            if (ret < 0) {
                printf("UdpRecvTask: sendto failed!\r\n");
            } else {
                printf("UdpRecvTask: sendto %s-%d(%d): %s\n",
                    inet_ntoa(clntAddr.sin_addr), ntohs(clntAddr.sin_port), clntAddr.sin_port, reply_buff);
            }
            #endif
        }
    }
}

void UdpSendTask(void)
{
    ssize_t ret = 0;
    unsigned char send_buff[] = "Hello OpenHarmony!";

    while (1) {
        sleep(1);
        //clntAddr记录了向server发送过消息的最后一个client的身份信息(包含了IP和port)
        //不打开下面两句语句，即只对clntAddr单播
        //打开下面两句语句，即对局域网内所有的client(它们的IP不一样，但都监听了clntPort)进行广播
        #if 0
        clntAddr.sin_addr.s_addr = inet_addr("255.255.255.255");
        clntAddr.sin_port = htons(10000);   // clntPort
        #endif

        ret = sendto(sockfd, send_buff, sizeof(send_buff), 0, (struct sockaddr *)&clntAddr, sizeof(struct sockaddr_in));
        if (ret < 0) {
            printf("UdpSendTask sendto failed!\r\n");
        } else {
            printf("UdpRecvTask: sendto %s-%d(%d): %s\n",
                inet_ntoa(clntAddr.sin_addr), ntohs(clntAddr.sin_port), clntAddr.sin_port, send_buff);
        }
    }
}

void UdpServerEntry(void)
{
    UdpSocketInit();

    osThreadAttr_t attr1 = {"UdpRecvTask", 0, NULL, 0, NULL, 1024*2, 32, 0, 0};
    if (osThreadNew((osThreadFunc_t)UdpRecvTask, NULL, &attr1) == NULL) {
        printf("[UdpServerEntry] Falied to create UdpRecvTask!\n");
    }

#if 1
    osThreadAttr_t attr2 = {"UdpSendTask", 0, NULL, 0, NULL, 1024*2, 32, 0, 0};
    if (osThreadNew((osThreadFunc_t)UdpSendTask, NULL, &attr2) == NULL) {
        printf("[UdpServerEntry] Falied to create UdpSendTask!\n");
    }
#endif
}