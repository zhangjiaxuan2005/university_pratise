#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include "ohos_init.h"
#include "cmsis_os2.h"
#include "iot_i2c.h"
#include "iot_gpio.h"
#include "iot_pwm.h"
#include "iot_uart.h"
#include "iot_errno.h"

#include <hi_pwm.h>
#include <hi_gpio.h>
#include <hi_io.h>
#include <hi_adc.h>
#include <hi_watchdog.h>

#include "aht20.h"

float g_Temp = 0.0;    // air temperature
float g_Humi = 0.0;    // air humidity

#define I2C0_IDX      (0)
#define I2C0_BAUDRATE (400*1000)
static int I2C0Init(void)
{
    IoTGpioInit(HI_IO_NAME_GPIO_13);
    IoTGpioInit(HI_IO_NAME_GPIO_14);
    hi_io_set_func(HI_IO_NAME_GPIO_13, HI_IO_FUNC_GPIO_13_I2C0_SDA);
    hi_io_set_func(HI_IO_NAME_GPIO_14, HI_IO_FUNC_GPIO_14_I2C0_SCL);

    return IoTI2cInit(I2C0_IDX, I2C0_BAUDRATE);
}
static void I2C0DeInit(void)
{
    IoTI2cDeinit(I2C0_IDX);
}

void MainTask(void)
{
    int ret = -1;
    hi_watchdog_disable();

    if (I2C0Init() != 0) {
        return;
    }

    WifiTask();
    huawei_cloud_mqtt_init();

    ret = AHT20_Calibrate();
    printf("[MainTask] AHT20_Calibrate: ret[%d]\n", ret);

    while (1) {
        sleep(2);
        ret = AHT20_StartMeasure();
        printf("[MainTask] AHT20_StartMeasure: ret[%d]\n", ret);

        ret = AHT20_GetMeasureResult(&g_Temp, &g_Humi);
        printf("[MainTask] AHT20_GetMeasureResult: ret[%d], Temp[%.2f], Humi[%.2f]\n",
                   ret, g_Temp, g_Humi);

        deal_report_msg();  // send to huawei cloud
    }

    I2C0DeInit();
}

void MainEntry(void)
{
    osThreadAttr_t attr = {"MainTask", 0, NULL, 0, NULL, 1024*10, 24, 0, 0};

    if (osThreadNew((osThreadFunc_t)MainTask, NULL, &attr) == NULL) {
        printf("[MainEntry] create MainTask Failed!\n");
    }
}
SYS_RUN(MainEntry);