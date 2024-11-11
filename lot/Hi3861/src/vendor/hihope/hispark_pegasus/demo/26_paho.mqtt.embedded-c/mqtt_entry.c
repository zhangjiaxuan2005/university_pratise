#include <stdio.h>
#include <unistd.h>
#include "ohos_init.h"
#include "cmsis_os2.h"

#include "hi_wifi_api.h"
#include "lwip/ip_addr.h"
#include "lwip/netifapi.h"
#include "lwip/sockets.h"

#include "MQTTClient.h"


static MQTTClient mq_client;

 unsigned char *onenet_mqtt_buf;
 unsigned char *onenet_mqtt_readbuf;
 int buf_size;

Network n;
MQTTPacket_connectData data = MQTTPacket_connectData_initializer;  

//��Ϣ�ص�����
void mqtt_callback(MessageData *msg_data)
{
    size_t res_len = 0;
    uint8_t *response_buf = NULL;
    char topicname[45] = { "$crsp/" };

    LOS_ASSERT(msg_data);

    printf("topic %.*s receive a message\r\n", msg_data->topicName->lenstring.len, msg_data->topicName->lenstring.data);

    printf("message is %.*s\r\n", msg_data->message->payloadlen, msg_data->message->payload);

}

int MQTTDemoTask(void)
{
	int rc = -1;
    
	NetworkInit(&n);
	rc = NetworkConnect(&n, "192.168.1.4", 1883);
    //rc = NetworkConnect(&n, "5.196.95.208", 1883);
    printf("[MQTTDemoTask] NetworkConnect: rc = %d\n", rc);

    buf_size  = 128;
    onenet_mqtt_buf = (unsigned char *) malloc(buf_size);
    onenet_mqtt_readbuf = (unsigned char *) malloc(buf_size);
    if (!(onenet_mqtt_buf && onenet_mqtt_readbuf)) {
        printf("[MQTTDemoTask] No memory for MQTT client buffer!");
        return -2;
    }

	MQTTClientInit(&mq_client, &n, 1000, onenet_mqtt_buf, buf_size, onenet_mqtt_readbuf, buf_size);
	
    //rc = MQTTStartTask(&mq_client);
    //printf("[MQTTDemoTask] MQTTStartTask: rc = %d\n", rc);

    data.keepAliveInterval = 30;
    data.cleansession = 1;
	data.clientID.cstring = "ohos_hi3861";
	data.username.cstring = "123456";
	data.password.cstring = "222222";

	data.keepAliveInterval = 10;
	data.cleansession = 1;
	
    mq_client.defaultMessageHandler = mqtt_callback;

	//���ӷ�����
	rc = MQTTConnect(&mq_client, &data);
    printf("[MQTTDemoTask] MQTTConnect: rc = %d\n", rc);

	//������Ϣ�������ûص�����
	rc = MQTTSubscribe(&mq_client, "aaaa", 0, mqtt_callback);
    printf("[MQTTDemoTask] MQTTSubscribe: rc = %d\n", rc);

	while(1) {
		MQTTMessage message;

		message.qos = QOS1;
		message.retained = 0;
		message.payload = (void *)"openharmony";
		message.payloadlen = strlen("openharmony");

		//������Ϣ
        rc = MQTTPublish(&mq_client, "bbbb", &message);
		if (rc < 0) {
            printf("[MQTTDemoTask] MQTTPublish: rc = %d\n", rc);
			return -1;
		}
        sleep(1);
	}

	return 0;
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