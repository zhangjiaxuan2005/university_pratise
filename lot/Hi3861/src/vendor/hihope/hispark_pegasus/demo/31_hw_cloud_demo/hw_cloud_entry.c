//线程和进程的部分
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "cmsis_os2.h"
#include "ohos_init.h"
#include <cJSON.h>

/********************************************************************/
#include <dtls_al.h>
#include <mqtt_al.h>
#include <oc_mqtt_al.h>
#include <oc_mqtt_profile.h>

//连接华为云的部分，注意区分标准版和基础版的对接地址
#define CONFIG_APP_SERVERIP   "117.78.5.125"  //华为云设备接入标准版对接地址
//#define CONFIG_APP_SERVERIP   "121.36.42.100" //华为云设备接入基础版对接地址
#define CONFIG_APP_SERVERPORT "1883"          //不能变,MQTT接入端口为1883

#define CONFIG_APP_DEVICEID  "64c6147bb84c1334befc56ca_ZHJJ0000" // 替换为注册设备后生成的deviceid
#define CONFIG_APP_DEVICEPWD "ZHJJ0000" // 替换为注册设备后生成的密钥

#define CONFIG_APP_LIFETIME  (60) // seconds
#define CONFIG_QUEUE_TIMEOUT (5 * 1000)

#define MSGQUEUE_COUNT        16
#define MSGQUEUE_SIZE         10
#define CLOUD_TASK_STACK_SIZE (1024 * 10)
#define CLOUD_TASK_PRIO       24
#define BEEP_DELAY            5

/********************************************************************/
//服务和属性的部分
typedef enum {
    en_msg_cmd = 0, //华为云向设备发送命令
    en_msg_report,  //向华为云上传信息
} en_msg_type_t;

typedef struct {
    char *request_id;
    char *payload;
} cmd_t;

typedef struct {
    int lum;
} report_t;

typedef struct {
    en_msg_type_t msg_type;
    union {
        cmd_t cmd;
        report_t report;
    } msg;
} app_msg_t;

typedef struct {
    osMessageQueueId_t app_msg;
    int connected;
    int led;
} app_cb_t;
static app_cb_t g_app_cb;

extern float g_Temp;    // air temperature
extern float g_Humi;    // air humidity

/********************************************************************/
// 使用这个函数将所有消息推送到缓冲区中
static int msg_rcv_callback(oc_mqtt_profile_msgrcv_t *msg)
{
    int ret = 0;
    char *buf;
    int buf_len;
    app_msg_t *app_msg;

    if ((msg == NULL) || (msg->request_id == NULL) || (msg->type != EN_OC_MQTT_PROFILE_MSG_TYPE_DOWN_COMMANDS)) {
        return ret;
    }

    buf_len = sizeof(app_msg_t) + strlen(msg->request_id) + 1 + msg->msg_len + 1;
    buf = malloc(buf_len);
    if (buf == NULL) {
        return ret;
    }
    app_msg = (app_msg_t *)buf;
    buf += sizeof(app_msg_t);

    app_msg->msg_type = en_msg_cmd;
    app_msg->msg.cmd.request_id = buf;
    buf_len = strlen(msg->request_id);
    buf += buf_len + 1;
    memcpy_s(app_msg->msg.cmd.request_id, buf_len, msg->request_id, buf_len);
    app_msg->msg.cmd.request_id[buf_len] = '\0';

    buf_len = msg->msg_len;
    app_msg->msg.cmd.payload = buf;
    memcpy_s(app_msg->msg.cmd.payload, buf_len, msg->msg, buf_len);
    app_msg->msg.cmd.payload[buf_len] = '\0';

    ret = osMessageQueuePut(g_app_cb.app_msg, &app_msg, 0U, CONFIG_QUEUE_TIMEOUT);
    if (ret != 0) {
        free(app_msg);
    }

    return ret;
}

/********************************************************************/
//向华为云推送数据
//推送数据到华为云，当需要上传数据时，需要先拼装数据，让后通过oc_mqtt_profile_propertyreport上报数据。
void deal_report_msg(void)
{
    oc_mqtt_profile_service_t service;

    oc_mqtt_profile_kv_t Temp;  //空气温度
    oc_mqtt_profile_kv_t Humi;  //空气湿度

    if (g_app_cb.connected != 1) {
        return;
    }
    printf("report sensor info to huawei cloud ...\n");

    service.event_time = NULL;
    service.service_id = "zhinengjiaju0000";   //服务名
    service.service_property = &Temp;
    service.nxt = NULL;         //下一个服务

    Temp.key = "Temp";          //服务的属性名
    Temp.value = &g_Temp ;      //上报的数据为"Intrude"或者是 "Safe"
    Temp.type = EN_OC_MQTT_PROFILE_VALUE_FLOAT;//属性是float类型，华为云平台上创建属性选择decimal(小数)
    Temp.nxt = &Humi;

    Humi.key = "Humi";          //服务的属性名
    Humi.value = &g_Humi ;      //上报的数据为"Intrude"或者是 "Safe"
    Humi.type = EN_OC_MQTT_PROFILE_VALUE_FLOAT;//属性是float类型，华为云平台上创建属性选择decimal(小数)
    Humi.nxt = NULL;

    //设备上报属性数据,用于设备按产品模型中定义的格式将属性数据上报给平台.
    oc_mqtt_profile_propertyreport(NULL, &service);
    return;
}

/********************************************************************/
static void oc_cmdresp(cmd_t *cmd, int cmdret)
{
    oc_mqtt_profile_cmdresp_t cmdresp;
    cmdresp.paras = NULL;
    cmdresp.request_id = cmd->request_id;   
    cmdresp.ret_code = cmdret;
    cmdresp.ret_name = NULL;
    (void)oc_mqtt_profile_cmdresp(NULL, &cmdresp);
}

static void deal_cmd_msg(cmd_t *cmd)
{
    cJSON *obj_root;
    cJSON *obj_cmdname;
    cJSON *obj_paras;
    cJSON *obj_para;
    int cmdret = 1;

    obj_root = cJSON_Parse(cmd->payload);
    if (obj_root == NULL) {
        oc_cmdresp(cmd, cmdret);
    }

    obj_cmdname = cJSON_GetObjectItem(obj_root, "command_name");
    if (obj_cmdname == NULL) {
        cJSON_Delete(obj_root);
    }

    //命令名称：DoSomthing
    if (strcmp(cJSON_GetStringValue(obj_cmdname), "DoSomthing") == 0) {
        obj_paras = cJSON_GetObjectItem(obj_root, "paras");
        if (obj_paras == NULL) {
            cJSON_Delete(obj_root);
        }
        obj_para = cJSON_GetObjectItem(obj_paras, "args");//参数名称：args
        if (obj_para == NULL) {
            cJSON_Delete(obj_root);
        }
        ///< operate the LED here
        if (strcmp(cJSON_GetStringValue(obj_para), "DO") == 0) {//数据类型：string
            g_app_cb.led = 1;
            //jd_switch(1, 0);
            printf("huawei_cloud_mqtt: get cmd: DoSomthing(DO)\n");
        } else {
            g_app_cb.led = 0;
            //jd_switch(1, 1);
            printf("huawei_cloud_mqtt: get cmd: DoSomthing(UNDO)\n");
        }
        cmdret = 0;
        oc_cmdresp(cmd, cmdret);
    }
    return;
}

/********************************************************************/
static int CloudCmdTask(void)
{
    app_msg_t *app_msg;
    printf("[CloudCmdTaskEntry] create CloudCmdTask OK!\n");

    while (1) {
        app_msg = NULL;
        (void)osMessageQueueGet(g_app_cb.app_msg, (void **)&app_msg, NULL, 0xFFFFFFFF);
        if (app_msg != NULL) {
            switch (app_msg->msg_type) {
                case en_msg_cmd:
                    deal_cmd_msg(&app_msg->msg.cmd);
                    break;
                default:
                    break;
            }
            free(app_msg);
        }
    }
    return 0;
}

static void CloudCmdTaskEntry(void)
{
    osThreadAttr_t attr;

    attr.name = "CloudCmdTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = CLOUD_TASK_STACK_SIZE;
    attr.priority = CLOUD_TASK_PRIO;

    if (osThreadNew((osThreadFunc_t)CloudCmdTask, NULL, &attr) == NULL) {
        printf("[CloudCmdTaskEntry] create CloudCmdTask Failed!\n");
    }
}

int huawei_cloud_mqtt_init(void)
{
    /***********************************************************/
    // 连接华为云的部分
    uint32_t ret;
    dtls_al_init();
    mqtt_al_init();
    oc_mqtt_init();

    g_app_cb.app_msg = osMessageQueueNew(MSGQUEUE_COUNT, MSGQUEUE_SIZE, NULL);
    if (g_app_cb.app_msg == NULL) {
        printf("huawei_cloud_mqtt_init: Create receive msg queue failed\r\n");
    }
    oc_mqtt_profile_connect_t connect_para;
    (void)memset_s(&connect_para, sizeof(connect_para), 0, sizeof(connect_para));

    connect_para.boostrap = 0;
    connect_para.device_id = CONFIG_APP_DEVICEID;
    connect_para.device_passwd = CONFIG_APP_DEVICEPWD;
    connect_para.server_addr = CONFIG_APP_SERVERIP;
    connect_para.server_port = CONFIG_APP_SERVERPORT;
    connect_para.life_time = CONFIG_APP_LIFETIME;
    connect_para.rcvfunc = msg_rcv_callback;
    connect_para.security.type = EN_DTLS_AL_SECURITY_TYPE_NONE;
    ret = oc_mqtt_profile_connect(&connect_para);
    if ((ret == (int)en_oc_mqtt_err_ok)) {
        g_app_cb.connected = 1;
        printf("huawei_cloud_mqtt_init: oc_mqtt_profile_connect succed!\r\n");
    } else {
        printf("huawei_cloud_mqtt_init: oc_mqtt_profile_connect failed!\r\n");
    }

    /**********************************************************/
    // 另起一个线程用于接收和处理华为云发送的命令
    CloudCmdTaskEntry();

    return 0;
}