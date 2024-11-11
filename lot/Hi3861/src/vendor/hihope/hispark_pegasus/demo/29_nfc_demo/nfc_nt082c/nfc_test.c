/*
 * Copyright (C) 2023 HiHope Open Source Organization .
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 *
 * limitations under the License.
 */
#include <stdio.h>
#include <unistd.h>
#include "ohos_init.h"
#include "cmsis_os2.h"
#include "iot_errno.h"
#include "iot_gpio.h"
#include "iot_i2c.h"
#include "hi_io.h"
#include "hi_gpio.h"

#include "nfc_config.h"
extern unsigned char nfc_mode;

#define HI_I2C_IDX_BAUDRATE 400000 // 400k
/*i2c read*/
unsigned int write_read(unsigned char reg_high_8bit_cmd,
                        unsigned char reg_low_8bit_cmd,
                        unsigned char* recv_data,
                        unsigned char send_len,
                        unsigned char read_len)
{
    IotI2cIdx id = IOT_I2C_IDX_0;
    IotI2cData co8i_nfc_i2c_read_data ={0};
    IotI2cData c081nfc_i2c_write_cmd_addr ={0};

    unsigned char _send_user_cmd[2] = {reg_high_8bit_cmd, reg_low_8bit_cmd};

    memset(recv_data, 0x0, sizeof(recv_data));
    memset(&co8i_nfc_i2c_read_data, 0x0, sizeof(IotI2cData));

    c081nfc_i2c_write_cmd_addr.sendBuf = _send_user_cmd;
    c081nfc_i2c_write_cmd_addr.sendLen = send_len;

    co8i_nfc_i2c_read_data.receiveBuf = recv_data;
    co8i_nfc_i2c_read_data.receiveLen = read_len;

    IoTI2cWrite(id, C081_NFC_ADDR&0xFE, c081nfc_i2c_write_cmd_addr.sendBuf, c081nfc_i2c_write_cmd_addr.sendLen);
    IoTI2cRead(id, C081_NFC_ADDR|I2C_RD, co8i_nfc_i2c_read_data.receiveBuf, co8i_nfc_i2c_read_data.receiveLen);

    return 0;
}

void nfc_init(void)
{
    nfc_mode = 0;
 	Protocol_Config();
    FM11_Init();
}

void NfcTask(void)
{
    printf("[NfcTask] NfcTask is running ...\n");
    nfc_init();
    while (1) {
        nfcread();
        usleep(10000);
    }
}

static int I2C0Init(void)
{
    IoTGpioInit(HI_IO_NAME_GPIO_13);
    IoTGpioInit(HI_IO_NAME_GPIO_14);
    hi_io_set_func(HI_IO_NAME_GPIO_13, HI_IO_FUNC_GPIO_13_I2C0_SDA);
    hi_io_set_func(HI_IO_NAME_GPIO_14, HI_IO_FUNC_GPIO_14_I2C0_SCL);

    return IoTI2cInit(0, (400*1000));
}

osThreadId_t NfcTestEntry(void)
{
    I2C0Init();

    IoTGpioInit(HI_IO_NAME_GPIO_9);  //CSN
    hi_io_set_func(HI_IO_NAME_GPIO_9, HI_IO_FUNC_GPIO_11_GPIO);
    IoTGpioSetDir(HI_IO_NAME_GPIO_9, HI_GPIO_DIR_OUT);
    hi_io_set_pull(HI_IO_NAME_GPIO_9, 0);
    hi_udelay(250);

    IoTGpioInit(HI_IO_NAME_GPIO_10);    //IRQ_N
    hi_io_set_func(HI_IO_NAME_GPIO_10, HI_IO_FUNC_GPIO_10_GPIO);
    IoTGpioSetDir(HI_IO_NAME_GPIO_10, HI_GPIO_DIR_OUT);
    IoTGpioSetOutputVal(HI_IO_NAME_GPIO_10, 1);

    //osPriorityAboveNormal[32], osPriorityNormal[24]
    //{.name, .attr_bits, .cb_mem, .cb_size, .stack_mem, .stack_size, .priority, .tz_module, .reserved}
    osThreadAttr_t attr = {"NfcTask", 0, NULL, 0, NULL, 1024*4, 24, 0, 0};
    osThreadId_t taskId = osThreadNew((osThreadFunc_t)NfcTask, NULL, &attr);
    if (taskId == NULL) {
        printf("[NfcTestEntry] Falied to create %s!\n", attr.name);
    }
    return taskId;
}
SYS_RUN(NfcTestEntry);
