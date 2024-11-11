#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include "ohos_init.h"
#include "cmsis_os2.h"

void MainTask(void)
{
    hi_watchdog_disable();
    WifiTask();
    MQTTClientTask();
}

void MainEntry(void)
{
    osThreadAttr_t attr = {"MainTask", 0, NULL, 0, NULL, 1024*4, 24, 0, 0};

    if (osThreadNew((osThreadFunc_t)MainTask, NULL, &attr) == NULL) {
        printf("[MainEntry] create MainTask Failed!\n");
    }
}
SYS_RUN(MainEntry);