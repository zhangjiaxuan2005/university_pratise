#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ohos_init.h"
#include "cmsis_os2.h"

#include "MQTTClient.h"

#define dbg_print         printf
#define MQTT_SERVER_IP    "192.168.1.3"
#define MQTT_SERVER_PORT  (1883)
#define MQTT_QOS          (QOS2)  //QOS0:At most once; QOS1:At least once; QOS2:Exactly once
#define BUFF_SIZE         (1024)

void TopicMsgCb(MessageData* data)
{
	dbg_print("[mqtt_client] TopicMsgCb received: topic[%.*s] msg[%.*s]\n",
        data->topicName->lenstring.len, data->topicName->lenstring.data,
		data->message->payloadlen, data->message->payload);
}

void MQTTClientTask(void)
{
	int ret, count = 0;
	dbg_print("[mqtt_client] MQTTClientTask Begin:\n");

    Network netCfg;
	NetworkInit(&netCfg);

	ret = NetworkConnect(&netCfg, MQTT_SERVER_IP, MQTT_SERVER_PORT);
	dbg_print("[mqtt_client] MQTTClientTask NetworkInit + NetworkConnect: ret[%d](0-OK)\n", ret);
    if (ret != 0) {
        return;
    }

	MQTTClient client;
    unsigned char sendBuf[BUFF_SIZE];
    unsigned char readBuf[BUFF_SIZE];
	MQTTClientInit(&client, &netCfg, 2000, sendBuf, sizeof(sendBuf), readBuf, sizeof(readBuf));

	MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
	data.willFlag          = 0;
	data.MQTTVersion       = 3;
	data.keepAliveInterval = 10;
	data.cleansession      = 1;
  	data.clientID.cstring  = "Hi3861";
	data.username.cstring = NULL;
	data.password.cstring = NULL;

	ret = MQTTConnect(&client, &data);
	dbg_print("[mqtt_client] MQTTClientTask MQTTConnect: ret[%d](0-OK)\n", ret);
	if (ret != 0) {
        goto MQTT_EXIT;
	}

    char *subTopic = "aaaa";
	ret = MQTTSubscribe(&client, subTopic, MQTT_QOS, TopicMsgCb);
	dbg_print("[mqtt_client] MQTTClientTask MQTTSubscribe: topic[%s] ret[%d](0-OK)\n", subTopic, ret);
	if (ret != 0) {
        goto MQTT_EXIT;
	}

    char *pubTopic = "bbbb";
    char msgPayload[32];
    MQTTMessage pubMsg;
	while (++count) {
		pubMsg.qos      = MQTT_QOS;
		pubMsg.retained = 0;
		pubMsg.payload  = msgPayload;
		sprintf(msgPayload, "message number %d", count);
		pubMsg.payloadlen = strlen(msgPayload);

		if ((ret = MQTTPublish(&client, pubTopic, &pubMsg)) != 0) {
            dbg_print("[mqtt_client] MQTTClientTask MQTTPublish(topic[%s], msg[%s]) ==>> NG\n",
                pubTopic, pubMsg.payload);
			goto MQTT_EXIT;
		}
		osDelay(200);
	}

MQTT_EXIT:
    NetworkDisconnect(&netCfg);
    MQTTDisconnect(&client);
	dbg_print("[mqtt_client] MQTTClientTask End.\n");
}

#if 0
static void MQTTClientEntry(void)
{
    osThreadAttr_t attr = {"MQTTClientTask", 0, NULL, 0, NULL, 1024*4, 28, 0, 0};

    if (osThreadNew((osThreadFunc_t)MQTTClientTask, NULL, &attr) == NULL) {
        printf("[MQTTClientEntry] create MQTTClientTask Failed\n");
    }
}
SYS_RUN(MQTTClientEntry);
#endif