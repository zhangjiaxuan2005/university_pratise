#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "ohos_init.h"
#include "ohos_types.h"

//device/hisilicon/hispark_pegasus/hi3861_adapter/kal/cmsis
#include "cmsis_os2.h"

//device/hisilicon/hispark_pegasus/sdk_liteos/include
#include "hi_wifi_api.h"
//#include "wifi_sta.h"

//device/hisilicon/hispark_pegasus/sdk_liteos/third_party/lwip_sack/include
#include "lwip/ip_addr.h"
#include "lwip/netifapi.h"

#include "wifi_entry.h"

#define IP_LEN (16)

static BOOL fgWifiConnected = FALSE;
static BOOL fgWifiIPChecked = FALSE;

void PingTest(void)
{
    const char* argv[] = {"www.baidu.com"};
    u32_t ret = os_shell_ping(1, argv);
    printf("[wifilink] os_shell_ping(%s) ret = %d\n",argv[0], ret);
}

void CheckWifiState(void)
{
    if(fgWifiIPChecked)
        return;

    struct netif* p_netif = netifapi_netif_find("wlan0");
    if(NULL == p_netif) {
        printf("[wifilink] CheckWifiState netifapi_netif_find fail\n");
        return;
    }

    ip4_addr_t gwaddr  = {0};
    ip4_addr_t ipaddr  = {0};
    ip4_addr_t netmask = {0};
    if (HISI_OK != netifapi_netif_get_addr(p_netif, &ipaddr, &netmask, &gwaddr)) {
        printf("[wifilink] CheckWifiState netifapi_netif_get_addr fail\n");
        return;
    }

    char ip[IP_LEN] = {0};
    char gw[IP_LEN] = {0};
    inet_ntop(AF_INET, &ipaddr, ip, IP_LEN);
    inet_ntop(AF_INET, &gwaddr, gw, IP_LEN);
    printf("[wifilink] CheckWifiState fgWifiConnected[T]: IP[%s]/GW[%s]\n", ip, gw);

    if(ipaddr.addr && gwaddr.addr) {
        fgWifiIPChecked = TRUE;
    }

    return;
}

void WifiLink(void)
{
    if(fgWifiConnected)   //防止重复连接WiFi
        return;

    printf("[wifilink] WifiLink Begin: fgWifiConnected[F]\n");

    //step 1: AT+STARTSTA
    // #启动STA模式
    char ifname[WIFI_IFNAME_MAX_SIZE] = {0};  //“wlan0”
    int  len = WIFI_IFNAME_MAX_SIZE;

    if (HISI_OK != hi_wifi_sta_start(ifname, &len)) {
        printf("[wifilink] WifiLink hi_wifi_sta_start fail\n");
        return;
    }

    //step 2: AT+CONN="SSID",,2,"PASSWORD"
    //# 连接指定AP，其中SSID/PASSWORD为待连接的热点名称和密码
    hi_wifi_assoc_request request = {0};
    request.auth = HI_WIFI_SECURITY_WPA2PSK; //2

    memcpy(request.ssid, WIFI_SSID, strlen(WIFI_SSID));
    memcpy(request.key, WIFI_PASSWD, strlen(WIFI_PASSWD));

    if (HISI_OK != hi_wifi_sta_connect(&request)) {
        printf("[wifilink] WifiLink hi_wifi_sta_connect fail\n");
        return;
    }

    //step 3: AT+DHCP=wlan0,1
    //# 通过DHCP向AP请求wlan0的IP地址
    struct netif* p_netif = netifapi_netif_find(ifname);
    if(NULL == p_netif) {
        printf("[wifilink] WifiLink netifapi_netif_find fail\n");
        return;
    }

    #if 1  //DHCP 自动分配IP
    if(HISI_OK != netifapi_dhcp_start(p_netif)) {
        printf("[wifilink] WifiLink netifapi_dhcp_start fail\n");
        return;
    }
    #else  //设置固定 IP
    ip4_addr_t gw;
    ip4_addr_t ipaddr;
    ip4_addr_t netmask;

    IP4_ADDR(&gw,      192, 168,  1, 1);
    IP4_ADDR(&ipaddr,  192, 168,  1, 200);   //固定到这个 IP
    IP4_ADDR(&netmask, 255, 255, 255, 0);

    if (HISI_OK != netifapi_netif_set_addr(p_netif, &ipaddr, &netmask, &gw)) {
        printf("[wifilink] WifiLink netifapi_netif_set_addr fail\n");
        return;
    }

    if (HISI_OK != hi_wifi_sta_connect(&request)) {
        printf("[wifilink] WifiLink hi_wifi_sta_connect fail\n");
        return;
    }
    #endif

    fgWifiConnected = TRUE;
    printf("[wifilink] WifiLink End.   fgWifiConnected[T]\n");

    return;
}

void WifiTask(void)
{
    printf("\n[wifilink] WifiTask Begin:\n");

    while (1) {
        usleep(1000000);  //sleep 1000ms

        if(!fgWifiConnected) {
            WifiLink();
        }
        else if(fgWifiConnected && !fgWifiIPChecked) {
            CheckWifiState();
        }
        else if(fgWifiIPChecked) {
            //PingTest();
            break;
        }
    }

    printf("[wifilink] WifiTask End.\n\n");

    return NULL;
}

#if 0
void WifiTasktEntry(void)
{
    printf("[wifilink] SYS_RUN(WifiTasktEntry)\n");

    osThreadAttr_t attr = {"WifiTask", 0, NULL, 0, NULL, 4096, 25, 0, 0};

    if(NULL == osThreadNew((osThreadFunc_t)WifiTask, NULL, &attr)) {
        printf("[wifilink] WifiTasktEntry: Failed to create WifiTask!\n");
    }
}
SYS_RUN(WifiTasktEntry);
#endif