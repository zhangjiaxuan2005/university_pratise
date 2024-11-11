#include <stdio.h>
#include <unistd.h>
#include "ohos_init.h"
#include "cmsis_os2.h"

//#include "hi_wifi_api.h"
//#include "lwip/sockets.h"
#include "lwip/ip_addr.h"
#include "lwip/netifapi.h"
#include "lwip/api_shell.h"

#include "wifi_device.h"

//#define WIFI_SSID      "openharmony"
//#define WIFI_PASSWD    "12345678"
#define WIFI_SSID      "STALKZ"
#define WIFI_PASSWD    "88888888"

static void PrintLinkedInfo(WifiLinkedInfo* info)
{
    if (!info) return;

    static char macAddress[32] = {0};
    unsigned char* mac = info->bssid;
    snprintf(macAddress, sizeof(macAddress), "%02X:%02X:%02X:%02X:%02X:%02X",
        mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    printf("bssid: %s, rssi: %d, connState: %d, reason: %d, ssid: %s\r\n",
        macAddress, info->rssi, info->connState, info->disconnectedReason, info->ssid);
}

static int g_connected = 0;
static void OnWifiConnectionChanged(int state, WifiLinkedInfo* info)
{
    if (!info) return;

    printf("%s %d, state = %d, info = \r\n", __FUNCTION__, __LINE__, state);
    PrintLinkedInfo(info);

    if (state == 1) {
        g_connected = 1;
    } else {
        g_connected = 0;
    }
}

static void OnWifiScanStateChanged(int state, int size)
{
    printf("%s %d, state = %X, size = %d\r\n", __FUNCTION__, __LINE__, state, size);
}

void WifiTask(void)
{
    WifiErrorCode errCode;
    printf("WifiTask: Begin:\r\n");

    //监听
    WifiEvent eventListener = {
        //链接状态发生变化
        .OnWifiConnectionChanged = OnWifiConnectionChanged,
        //扫描结果发生变化
        .OnWifiScanStateChanged = OnWifiScanStateChanged
    };
    WifiDeviceConfig apConfig = {};
    int netId = -1;

    osDelay(10);
    errCode = RegisterWifiEvent(&eventListener);
    printf("WifiTask: RegisterWifiEvent: ret = %d\r\n", errCode);

    strcpy(apConfig.ssid, WIFI_SSID);
    strcpy(apConfig.preSharedKey, WIFI_PASSWD);
    apConfig.securityType = WIFI_SEC_TYPE_PSK;

    errCode = EnableWifi();
    printf("WifiTask: EnableWifi: ret = %d\r\n", errCode);
    osDelay(10);

    errCode = AddDeviceConfig(&apConfig, &netId);
    printf("WifiTask: AddDeviceConfig: ret = %d\r\n", errCode);

    g_connected = 0;
    errCode = ConnectTo(netId);
    printf("WifiTask: ConnectTo(%d): ret = %d\r\n", netId, errCode);

    while (!g_connected) {
        osDelay(10);
    }
    printf("WifiTask: g_connected: %d\r\n", g_connected);
    osDelay(50);

    struct netif* iface = netifapi_netif_find("wlan0");
    if (iface) {
        err_t ret = netifapi_dhcp_start(iface);
        printf("WifiTask: netifapi_dhcp_start: ret = %d\r\n", ret);

        osDelay(200); // wait DHCP server give me IP
        ret = netifapi_netif_common(iface, dhcp_clients_info_show, NULL);
        printf("WifiTask: netifapi_netif_common: ret = %d\r\n", ret);
    }
    printf("WifiTask: End.\r\n");
}

#if 0
void WifiTasktEntry(void)
{
    //{.name, .attr_bits, .cb_mem, .cb_size, .stack_mem, .stack_size, .priority, .tz_module, .reserved}
    osThreadAttr_t attr = {"WifiTask", 0, NULL, 0, NULL, 1024*2, 24, 0, 0};

    if (osThreadNew((osThreadFunc_t)WifiTask, NULL, &attr) == NULL) {
        printf("[WifiTasktEntry] create WifiTask Failed!\n");
    }
}
SYS_RUN(WifiTasktEntry);
#endif