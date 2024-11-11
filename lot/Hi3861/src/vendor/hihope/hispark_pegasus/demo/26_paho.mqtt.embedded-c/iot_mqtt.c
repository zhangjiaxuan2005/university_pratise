#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cmsis_os2.h"
#include "ohos_init.h"

#include "MQTTClient.h"
//#include "wifi_connect.h"

#define MQTT_SERVERIP "192.168.1.4"
#define MQTT_SERVERPORT 1883
#define MQTT_CMD_TIMEOUT_MS 2000
#define MQTT_KEEP_ALIVE_MS 2000
#define MQTT_DELAY_2S 200
#define MQTT_DELAY_500_MS 50
#define MQTT_VERSION 3
#define MQTT_QOS 2
#define MQTT_TASK_STACK_SIZE (1024 * 10)

static unsigned char sendBuf[1000];
static unsigned char readBuf[1000];

Network network;

void messageArrived(MessageData *data)
{
    printf("Message arrived on topic[%.*s]: msg[%.*s]\n", data->topicName->lenstring.len,
        data->topicName->lenstring.data, data->message->payloadlen, data->message->payload);
}

void MQTTDemoTask(void)
{
    //WifiConnect("BearPi", "123456789");
    printf("[iot_mqtt] MQTTDemoTask: Starting ...\n");
    int rc, count = 0;
    MQTTClient client;

    NetworkInit(&network);
    printf("[iot_mqtt] MQTTDemoTask: NetworkConnect ...\n");

    NetworkConnect(&network, MQTT_SERVERIP, MQTT_SERVERPORT);
    printf("[iot_mqtt] MQTTDemoTask: MQTTClientInit ...\n");
    MQTTClientInit(&client, &network, MQTT_CMD_TIMEOUT_MS, sendBuf, sizeof(sendBuf), readBuf, sizeof(readBuf));

    MQTTString clientId = MQTTString_initializer;
    clientId.cstring = "bearpi";

    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
    data.clientID = clientId;
    data.willFlag = 0;
    data.MQTTVersion = MQTT_VERSION;
    data.keepAliveInterval = MQTT_KEEP_ALIVE_MS;
    data.cleansession = 1;

    printf("[iot_mqtt] MQTTDemoTask: MQTTConnect ...\n");
    rc = MQTTConnect(&client, &data);
    if (rc != 0) {
        printf("[iot_mqtt] MQTTDemoTask: MQTTConnect: rc[%d]\n", rc);
        NetworkDisconnect(&network);
        MQTTDisconnect(&client);
        osDelay(MQTT_DELAY_2S);
    }

    printf("[iot_mqtt] MQTTDemoTask: MQTTSubscribe ...\n");
    rc = MQTTSubscribe(&client, "aaaa", MQTT_QOS, messageArrived);
    if (rc != 0) {
        printf("[iot_mqtt] MQTTDemoTask: MQTTSubscribe: rc[%d]\n", rc);
        osDelay(MQTT_DELAY_2S);
    }
    while (++count) {
        MQTTMessage message;
        char payload[30];

        message.qos = MQTT_QOS;
        message.retained = 0;
        message.payload = payload;
        (void)sprintf_s(payload, sizeof(payload), "message number %d", count);
        message.payloadlen = strlen(payload);

        if ((rc = MQTTPublish(&client, "bbbb", &message)) != 0) {
            printf("[iot_mqtt] MQTTDemoTask: Return code from MQTT publish is rc[%d]\n", rc);
            NetworkDisconnect(&network);
            MQTTDisconnect(&client);
        }
        osDelay(MQTT_DELAY_500_MS);
    }
}
#if 0
static void MQTTDemoEntry(void)
{
    //{.name, .attr_bits, .cb_mem, .cb_size, .stack_mem, .stack_size, .priority, .tz_module, .reserved}
    osThreadAttr_t attr = {"MQTTDemoTask", 0, NULL, 0, NULL, 1024*5, 25, 0, 0};

    if (osThreadNew((osThreadFunc_t)MQTTDemoTask, NULL, &attr) == NULL) {
        printf("[MQTTDemoEntry] create MQTTDemoTask Failed!\n");
    }
}
SYS_RUN(MQTTDemoEntry);
#endif