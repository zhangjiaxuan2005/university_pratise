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
#include "hi_stdlib.h"

#include "nfc_config.h"

T4T_FILE current_file;

unsigned char capability_container[15] =
{   0x00, 0x0F,        //CCLEN  
    0x20,              //Mapping Version 
    0x00, 0xF6,        //MLe 必须是F6  写成FF超过256字节就会分帧  但是写成F6就不会分帧
    0x00, 0xF6,        //MLc 必须是F6  写成FF超过256字节就会分帧  但是写成F6就不会分帧
    0x04,              //NDEF消息格式 05的话就是私有
    0x06,              //NDEF消息长度
    0xE1, 0x04,        //NDEF FILE ID       NDEF的文件标识符
    0x03, 0x84,        //NDEF最大长度
    0x00,              //Read Access           可读
    0x00               //Write Access          可写
};             

#define HUAWEI  (0)
#define TAOBAO  (1)
#define WECHAT  (2)
#define NFC_TAG_NAME  WECHAT
unsigned char ndef_file[1024] = {
#if (NFC_TAG_NAME == HUAWEI)
    /* http://wwww.huawei.com */
    0x00, 0x0F,
    0xD1, 0x01, 0x0B, 0x55, 0x01,
    0x68, 0x75, 0x61, 0x77, 0x65, 0x69, 0x2E, 0x63, 0x6F, 0x6D, //huawei.com
#elif (NFC_TAG_NAME == TAOBAO)
    0x00, 0x23,
    0xd4, 0x0f, 0x11,
    0x61, 0x6e, 0x64, 0x72, 0x6f, 0x69, 0x64, 0x2e, 0x63, 0x6f, 0x6d, 0x3a, //android.com:
    0x70, 0x6b, 0x67, 0x63, 0x6f, 0x6d, 0x2e,  //pkgcom.
    0x74, 0x61, 0x6f, 0x62, 0x61, 0x6f, 0x2e,  //taobao.
    0x74, 0x61, 0x6f, 0x62, 0x61, 0x6f,      //taobao
#else
    /* wechat */
    0x00, 0x20,
    0xd4, 0x0f, 0x0e,
    0x61, 0x6e, 0x64, 0x72, 0x6f, 0x69, 0x64, 0x2e, 0x63, 0x6f, 0x6d, 0x3a, //android.com:
    0x70, 0x6b, 0x67, 0x63, 0x6f, 0x6d, 0x2e,                //pkgcom.
    0x74, 0x65, 0x6e, 0x63, 0x65, 0x6e, 0x74, 0x2e, 0x6d, 0x6d, //tencent.mm
#endif
};
unsigned char fm327_fifo[1024];
unsigned char irq_data_in = 0;//非接数据接收终端标识
unsigned char irq_txdone = 0;
unsigned char rfLen;
unsigned char irq_rxdone = 0;
unsigned char irq_data_wl = 0;
unsigned char FlagFirstFrame = 0; //卡片首帧标识

unsigned char fm365SakConfig = 0x20;//只有在通道模式下才需要配置，14443-3通道配置成0x00,14443-4通道配置成0x20.目前路由器走的是14443-4通道
unsigned char Protocol_4[4];     //-4 通道
unsigned char Protocol_3[4];     //-3 通道
unsigned char Protocol_Tag[4];   //tag模式
unsigned char SAK2_3[1] = {0x00};
unsigned char SAK2_4[1] = {0x20};
unsigned char Tag_mode = 0;//定义走Tag芯片，Tag_mode和AFE_mode 不能同时为1
unsigned char AFE_mode = 1;//定义走通道芯片，Tag_mode和AFE_mode 不能同时为1
unsigned char nfc_mode = 0;//mode为0时，芯片为通道模式，mode为1时，芯片为Tag模式

void Protocol_Config(void)
{
    Protocol_InitTypeDef  Protocol_InitStructure;
    if(nfc_mode == 0) {
        AFE_mode = 1;
        Tag_mode = 0;
    } else {
        Tag_mode = 1;
        AFE_mode = 0;
    }
    printf("Tag_mode is %d, AFE_mode is %d\r\n", Tag_mode, AFE_mode);
    if (AFE_mode) { //配置成通道模式
        if (fm365SakConfig == 0x20) {  /*14443-4通道模式*/
            Protocol_InitStructure.USER_CF0G = AFE_VoutEnsable;
            Protocol_InitStructure.USER_CF1G = AFE_InventoryEnable_14443_4_RFONDisable;
            Protocol_InitStructure.USER_CF2G = AFE_Tag_WIP_ArbitrateCT;
            Protocol_InitStructure.USER_CHECK = ~(Protocol_InitStructure.USER_CF0G ^ Protocol_InitStructure.USER_CF1G ^ Protocol_InitStructure.USER_CF2G);
            Protocol_4[0] = Protocol_InitStructure.USER_CF0G;
            Protocol_4[1] = Protocol_InitStructure.USER_CF1G;
            Protocol_4[2] = Protocol_InitStructure.USER_CF2G;
            Protocol_4[3] = Protocol_InitStructure.USER_CHECK;
        } else if (fm365SakConfig == 0x00) {  /*14443-3通道模式*/
            Protocol_InitStructure.USER_CF0G = AFE_VoutEnsable;
            Protocol_InitStructure.USER_CF1G = AFE_InventoryEnable_14443_3_RFONEnable;//RFON要打开，要不初始化来不及做
            Protocol_InitStructure.USER_CF2G = AFE_Tag_WIP_ArbitrateCT;	
            Protocol_InitStructure.USER_CHECK = ~(Protocol_InitStructure.USER_CF0G ^ Protocol_InitStructure.USER_CF1G ^ Protocol_InitStructure.USER_CF2G);
            Protocol_3[0] = Protocol_InitStructure.USER_CF0G;
            Protocol_3[1] = Protocol_InitStructure.USER_CF1G;
            Protocol_3[2] = Protocol_InitStructure.USER_CF2G;
            Protocol_3[3] = Protocol_InitStructure.USER_CHECK;
        }
    } else if (Tag_mode) {  /*tag SD模式*/
        Protocol_InitStructure.USER_CF0G = Tag_VoutEnable_OD_FD;
        Protocol_InitStructure.USER_CF1G = Tag_InventoryEnable_AFE_InventoryEnable_14443_4_RFONEnsable;
        Protocol_InitStructure.USER_CF2G = Tag_SD_ArbitrateNone;
        Protocol_InitStructure.USER_CHECK = ~(Protocol_InitStructure.USER_CF0G ^ Protocol_InitStructure.USER_CF1G ^ Protocol_InitStructure.USER_CF2G);
        Protocol_Tag[0] = Protocol_InitStructure.USER_CF0G;
        Protocol_Tag[1] = Protocol_InitStructure.USER_CF1G;
        Protocol_Tag[2] = Protocol_InitStructure.USER_CF2G;
        Protocol_Tag[3] = Protocol_InitStructure.USER_CHECK;
    }
}
// 修改SAK必须要计算CRC8,这个是CRC8的计算函数
unsigned char crc8(unsigned char *data,unsigned char data_length)
{
	int i=0;
	int j=0;
    int Crc8 = 0xff;
    for (i = 0; i < data_length; i++)
	{
        Crc8 ^= data[i];
	    for (j = 0; j < 8; j++)
		{
			if ((Crc8 & 0x01) == 0x01)
				Crc8 = (Crc8>>1) ^ 0xb8;
			else
				Crc8 >>= 1;
			Crc8 &= 0xff;
		}
	}
    return Crc8 & 0xff;
}
void FM11_Init(void)
{ 
    unsigned char ATS[5]={0x05,0x72,0xF7,0x60,0x02};   //芯片默认配置,走无链接通道
    unsigned char ATS_change[5]={0x05,0x78,0xF7,0x90,0x02};   //走带链接通道,修改ATS配置
    unsigned char serial_number[9]={0x00};
    unsigned char ATQA_SAK[4]={0x00};
    unsigned char CRC8[13]={0x00};
    unsigned char CRC8_data=0x00;
    unsigned char status1,status2,status3,status4;
    unsigned char SAK2_4[1]={0x20};   //14443-4  通道模式
    unsigned char SAK2_3[1]={0x00};   //14443-3  通道模式
    unsigned char rbuf[16]={0x00};   //读取测试buf
    unsigned char wbuf[16]={0x1d,0x42,0x27,0xf0,0x98,0xd1,0x18,0x2b,0x7a,0x44,0x00,0x04,0x00};   //写入测试buf

	fm11_read_eep(serial_number, FM441_Serial_number_EEaddress, 9);
	if(serial_number[7]==0x10)//FM11NT082C UID[6]为0x10，FM11NC08和FM11NT081DI为0x00，如果想做到一套代码兼容FM11NT081和FM11NT082的话这段代码是关键
	{
		if(AFE_mode) // 通道模式	1
		{
            printf("FM11_Init: AFE_mode[1]\n");
			if(fm365SakConfig == 0x20)//14443-4  通道模式
			{
			    printf("FM11_Init: fm365SakConfig[0x20]\n");
                fm11_write_eep(FM441_SAK_Control_EEaddress, 1, &SAK2_4[0]); //修改SAK，SAK一旦修改，必须修改CRC8
				fm11_write_eep(FM441_Protocol_Control_EEaddress, 4, Protocol_4);	
			}
			if(fm365SakConfig == 0x00)//14443-3  通道模式
			{
			    printf("FM11_Init: fm365SakConfig[0x00]\n");
                fm11_write_eep(FM441_SAK_Control_EEaddress, 1, &SAK2_3[0]);//修改SAK，SAK一旦修改，必须修改CRC8
				fm11_write_eep(FM441_Protocol_Control_EEaddress, 4, Protocol_3);	
			}
			status2=fm11_read_eep(serial_number, FM441_Serial_number_EEaddress, 9);//读取serial_number ,为计算CRC8做准备
			status3=fm11_read_eep(ATQA_SAK, FM441_ATQA_EEaddress, 4);//读取ATQA_SAK ,为计算CRC8做准备
            if((status2!=1)&&(status3!=1))//注意我status是成功返回1，客户需要注意是不是0。防止读eeprom的过程中没读成功，例如发送器件地址之后就没响应，导致UID和ATQA等值出错，CRC8不对
			{
			    printf("FM11_Init[A]: read serial_number[0x%02X][0x%02X][0x%02X][0x%02X][...]\n",
                        serial_number[0], serial_number[1], serial_number[2], serial_number[3]);
			    printf("FM11_Init[A]: read      ATQA_SAK[0x%02X][0x%02X][0x%02X][0x%02X]\n",
                        ATQA_SAK[0], ATQA_SAK[1], ATQA_SAK[2], ATQA_SAK[3]);
				memcpy(CRC8, serial_number, 9);
				memcpy(&CRC8[9], ATQA_SAK, 4);

				CRC8_data = crc8(CRC8, 13);
                // CRC8_data = 0x25;

				fm11_write_eep(FM441_CRC8_EEaddress, 1, &CRC8_data);
                unsigned char data2[1] = {0x00};
                fm11_read_eep(data2, FM441_CRC8_EEaddress, 1);
			    printf("FM11_Init[A]: read      data2[0x%02X]\n", data2[0]);
			}
			fm11_write_reg(FM441_RF_TXCTL_REG, 0x77);//让切换模式字立马生效
			fm11_write_reg(RESET_SILENCE, 0x55);//让切换模式字立马生效
            hi_udelay(1000);//很重要，复位时间
		}
		else if(Tag_mode)//tag模式 0
		{
            printf("FM11_Init: Tag_mode[1]\n");
            fm11_write_eep(FM441_SAK_Control_EEaddress,1,&SAK2_3[0]);//修改SAK，SAK一旦修改，必须修改CRC8
			fm11_write_eep(FM441_Protocol_Control_EEaddress,4,Protocol_Tag);
			status2=fm11_read_eep(serial_number,FM441_Serial_number_EEaddress ,9);//读取serial_number ,为计算CRC8做准备
			status3=fm11_read_eep(ATQA_SAK,FM441_ATQA_EEaddress,4);//读取ATQA_SAK ,为计算CRC8做准备
			if((status2!=1)&&(status3!=1))//注意我status是成功返回1，客户需要注意是不是0。防止读eeprom的过程中没读成功，例如发送器件地址之后就没响应，导致UID和ATQA等值出错，CRC8不对
            {
			    printf("FM11_Init[T]: read serial_number[0x%02X][0x%02X][0x%02X][0x%02X][...]\n",
                        serial_number[0], serial_number[1], serial_number[2], serial_number[3]);
			    printf("FM11_Init[T]: read      ATQA_SAK[0x%02X][0x%02X][0x%02X][0x%02X]\n",
                        ATQA_SAK[0], ATQA_SAK[1], ATQA_SAK[2], ATQA_SAK[3]);
				memcpy(CRC8,serial_number,9);
				memcpy(&CRC8[9],ATQA_SAK,4);
				CRC8_data=crc8(CRC8,13); 
				fm11_write_eep(FM441_CRC8_EEaddress,1,&CRC8_data);
			}
			fm11_write_reg(FM441_RF_TXCTL_REG,0x77);//让切换模式字立马生效
			fm11_write_reg(RESET_SILENCE,0x55);//让切换模式字立马生效
            hi_udelay(1000);//很重要，复位时间

            fm11_read_eep(rbuf,FM441_Protocol_Control_EEaddress, 4);//测试下写成功没有
            fm11_read_eep(rbuf,FM441_SAK_Control_EEaddress, 1); //测试下写成功没有
		}
    }
}
/*co8i 写命令: 该接口写eeprom 更改芯片配置*/
unsigned int c08i_nfc_i2c_write(unsigned char reg_high_8bit_cmd,
                                unsigned char reg_low_8bit_cmd,
                                unsigned char* data_buff,
                                unsigned char len)
{
    IotI2cIdx id = IOT_I2C_IDX_0;
    IotI2cData c081nfc_i2c_write_cmd_addr ={0};
    unsigned char _send_user_cmd[64] = {reg_high_8bit_cmd, reg_low_8bit_cmd};

    c081nfc_i2c_write_cmd_addr.sendBuf = _send_user_cmd;
    c081nfc_i2c_write_cmd_addr.sendLen = 2+len;
    for (int i = 0; i < len; i++) {
        _send_user_cmd[2+i] = *(data_buff+i);
    }
    int ret = IoTI2cWrite(id, C081_NFC_ADDR&0xFE, c081nfc_i2c_write_cmd_addr.sendBuf, c081nfc_i2c_write_cmd_addr.sendLen);
    printf("----- c08i_nfc_i2c_write: write %d: %s\n", len, (ret==0)?"OK":"NG");

    return 0;
}

/* 写寄存器*/
unsigned int write_fifo_reg( unsigned char reg_high_8bit_cmd, unsigned char reg_low_8bit_cmd, unsigned char data_buff)
{
    IotI2cIdx id = IOT_I2C_IDX_0;
    IotI2cData c081nfc_i2c_write_cmd_addr ={0};
    unsigned char _send_user_cmd[3] = {reg_high_8bit_cmd, reg_low_8bit_cmd, data_buff};

    c081nfc_i2c_write_cmd_addr.sendBuf = _send_user_cmd;
    c081nfc_i2c_write_cmd_addr.sendLen = 3;

    int ret = IoTI2cWrite(id, C081_NFC_ADDR&0xFE, c081nfc_i2c_write_cmd_addr.sendBuf, c081nfc_i2c_write_cmd_addr.sendLen);

    return 0;
}

/*写fifo data*/
unsigned int write_fifo_data( unsigned char* data_buff, unsigned char len)
{
    IotI2cIdx id = IOT_I2C_IDX_0;
    IotI2cData c081nfc_i2c_write_cmd_addr ={0};
    unsigned char _send_user_cmd[128] = {0};

    memset(_send_user_cmd, 0x0, sizeof(_send_user_cmd));

    _send_user_cmd[0] = 0xff;
    _send_user_cmd[1] = 0xf0;

    for (int i=0; i<len; i++) {
        _send_user_cmd[2+i] = *(data_buff+i);
    }
    c081nfc_i2c_write_cmd_addr.sendBuf = _send_user_cmd;
    c081nfc_i2c_write_cmd_addr.sendLen = 2 + len;
    int ret = IoTI2cWrite(id, C081_NFC_ADDR&0xFE, c081nfc_i2c_write_cmd_addr.sendBuf, c081nfc_i2c_write_cmd_addr.sendLen);

    return 0;
}

/*EEPROM page write*/
void eep_write_page(unsigned char *pBuffer, unsigned short WriteAddr, unsigned char datalen)
{
    printf("----- eep_write_page :: WriteAddr[0x%02X],len[%d] -----\n", WriteAddr, datalen);
    c08i_nfc_i2c_write((unsigned char)((WriteAddr & 0xFF00) >> 8), (unsigned char)(WriteAddr & 0x00FF), pBuffer, datalen);
	hi_udelay(10000);//必须延时10ms
}

/*写EEPROM*/
void fm11_write_eep(unsigned short addr,unsigned int len,unsigned char *wbuf)
{
	unsigned char offset;

    if (addr < FM11_E2_USER_ADDR || addr >= FM11_E2_MANUF_ADDR) {
        return;
    }
    if (addr % FM11_E2_BLOCK_SIZE) {
        offset = FM11_E2_BLOCK_SIZE - (addr % FM11_E2_BLOCK_SIZE);
        if (len > offset) {
            eep_write_page(wbuf,addr,offset);
            
            addr += offset;
            wbuf += offset;
            len -= offset;
        } else {
            eep_write_page(wbuf,addr,len);
            len = 0;
        }
     }
    while (len) {
        if (len >= FM11_E2_BLOCK_SIZE) {
            eep_write_page(wbuf,addr,FM11_E2_BLOCK_SIZE);
            addr += FM11_E2_BLOCK_SIZE;
            wbuf += FM11_E2_BLOCK_SIZE;
            len -= FM11_E2_BLOCK_SIZE;
        } else {
            eep_write_page(wbuf,addr,len);
            len = 0;
        }
    }
}

/*读EEPROM*/
unsigned int fm11_read_eep(unsigned char *dataBuff, unsigned short ReadAddr, unsigned short len)
{	
    write_read((unsigned char)((ReadAddr & 0xFF00)>>8), (unsigned char)(ReadAddr & 0x00FF), dataBuff, 2, len);

    return  0;
}
/*读NFC寄存器*/
unsigned char fm11_read_reg(unsigned short addr)
{
	unsigned char pdata[10] ={0};
	unsigned char a =0;

    if (fm11_read_eep(pdata, addr, 1) == 0) {	
		a = pdata[0];
	    return a;
	} else {
        printf("fm11_read_eep failed \r\n");  
      return -1;
    }   
}
/*写NFC寄存器*/
unsigned char fm11_write_reg(unsigned short addr, unsigned char data)
{
    unsigned int status =0;

    status = write_fifo_reg((unsigned char)((addr & 0xFF00) >> 8), (unsigned char)(addr & 0x00FF), data);
    if (status != 0) {
        return -1;
    }
    return 0;
}
/*读取FIFO*/
unsigned char  fm11_read_fifo(unsigned char NumByteToRead, unsigned char *pbuf)
{
    unsigned char read_fifo_len = NumByteToRead;

    if (fm11_read_eep(pbuf, FM327_FIFO, read_fifo_len) == 0) {
        return 0;
    } else {
        return -1;
    } 
}
/*写FIFO*/
unsigned char fm11_write_fifo(unsigned char *pbuf, unsigned char len)
{
    unsigned char status =0;

    if (pbuf == NULL) {
        return -1;
    }
    status = write_fifo_data(pbuf, len);
    if (status != 0) {
        return -1;
    }
    return 0;
}

/*数据回发*/
void fm11_data_send(unsigned int ilen, unsigned char *ibuf)
{
	unsigned int slen = 0;
	unsigned char *sbuf = NULL;

    if (ibuf == NULL) {
        return;
    }
	slen = ilen;
	sbuf = &ibuf[0];

	if (slen <= 32) {
		fm11_write_fifo(sbuf,slen);//write fifo	有多少发多少
		slen = 0;
        fm11_write_reg(RF_TXEN_REG,0x55);	//写0x55时触发非接触口回发数据
	} else {
		fm11_write_fifo(sbuf,32);//write fifo    先发32字节进fifo
        fm11_write_reg(RF_TXEN_REG, 0x55);	//写0x55时触发非接触口回发数据
		slen = slen - 32;//待发长度－32
		sbuf = sbuf + 32;//待发数据指针+32

        while (slen > 0) {
            if ((fm11_read_reg(FIFO_WORDCNT_REG) & 0x3F ) <= 8) {
                if (slen<=24) {
                    fm11_write_fifo(sbuf,slen); //write fifo	先发32字节进fifo
                    slen = 0;
                } else {
                    fm11_write_fifo(sbuf,24);	//write fifo	先发32字节进fifo
                    slen = slen - 24; 	//待发长度－24
                    sbuf = sbuf + 24; 	//待发数据指针+24
                }
            }
        }
        irq_txdone = 0;
    }
}

/*读取RF数据*/
unsigned int fm11_data_recv(unsigned char *rbuf)
{
	unsigned char irq = 0;
    unsigned char ret = 0;
	unsigned char irq_data_wl = 0;
	unsigned char irq_data_in = 0;
	unsigned int rlen = 0;
    unsigned int temp = 0;

#ifdef CHECK
    /*查询方式*/
	while (1) {
	    irq_data_wl = 0;
	    irq = fm11_read_reg(MAIN_IRQ);//查询中断标志
	    if (irq & MAIN_IRQ_FIFO) {
            ret=fm11_read_reg(FIFO_IRQ);
            if(ret & FIFO_IRQ_WL)
            irq_data_wl = 1;
		}
        if (irq & MAIN_IRQ_AUX) {
            fm11_read_reg(AUX_IRQ);
            fm11_write_reg(FIFO_FLUSH,0xFF);
        }
	    if (irq& MAIN_IRQ_RX_START) {
            irq_data_in = 1;
		}
        if (irq_data_in && irq_data_wl) {
            irq_data_wl =0;
            fm11_read_fifo(24,&rbuf[rlen]);//渐满之后读取24字节
            rlen += 24;
        }
        if (irq & MAIN_IRQ_RX_DONE) {
            temp =(unsigned int)( fm11_read_reg(FIFO_WORDCNT) & 0x3F);	//接收完全之后，查fifo有多少字节
            fm11_read_fifo(temp,&rbuf[rlen]);		//读最后的数据
            rlen += temp;
            irq_data_in = 0;
            break;
        }
        hi_sleep(10);
	}
#endif 

#ifdef INTERRUPT
    while (1) {
	    irq_data_wl = 0;
	    irq = fm11_read_reg(MAIN_IRQ);//查询中断标志

	    if (irq & MAIN_IRQ_FIFO) {
            ret=fm11_read_reg(FIFO_IRQ);
            if(ret & FIFO_IRQ_WL)  
            irq_data_wl = 1;
		}
        if (irq & MAIN_IRQ_AUX) {
            fm11_read_reg(AUX_IRQ);
            fm11_write_reg(FIFO_FLUSH,0xFF);
        }
		
	    if (irq& MAIN_IRQ_RX_START) {

          irq_data_in = 1;
		}

         if (irq_data_in && irq_data_wl) {
            irq_data_wl =0;
            fm11_read_fifo(24,&rbuf[rlen]);//渐满之后读取24字节
            rlen += 24;
        }

        if (irq & MAIN_IRQ_RX_DONE) {
            temp =(unsigned int)( fm11_read_reg(FIFO_WORDCNT) & 0x3F);	//接收完全之后，查fifo有多少字节
            fm11_read_fifo(temp,&rbuf[rlen]);		//读最后的数据
            rlen += temp;
            irq_data_in = 0;
            break;
        }
		hi_sleep(1);
	}
#endif
	if (rlen <= 2) {
        return 0;
    }  
	rlen -= 2;//2字节crc校验
	return rlen;
}
/* 写fifo 和 写寄存器*/
void fm11_t4t(hi_void)
{
    unsigned char ret =0;
    unsigned char nak_crc_err = 0x05;
    unsigned char status_ok[3] = { 0x02, 0x90, 0x00 };
    unsigned char status_word[3] = { 0x02, 0x6A, 0x82 };
    unsigned char status_word2[3] = { 0x02, 0x6A, 0x00 };
    unsigned char crc_err = 0;
    const unsigned char ndef_capability_container[2] = { 0xE1, 0x03 };
    const unsigned char ndef_id[2] = { 0xE1, 0x04 };
    unsigned char  i =0;
    unsigned char xlen =0;
    unsigned char xbuf[256] = {0};
    
    if (crc_err) {
        printf("fm11_t4t: crc_err[1]\n");
        fm11_write_fifo(&nak_crc_err, 1);
	    fm11_write_reg(RF_TXEN_REG, 0x55);
	    crc_err = 0;
    } else {
        status_ok[0]    = fm327_fifo[0];
        status_word[0]  = fm327_fifo[0];
        status_word2[0] = fm327_fifo[0];
        // select apdu
        if (fm327_fifo[INS] == 0xA4) {     // INS=2
            if (fm327_fifo[P1] == 0x00) {  // P1=3
                if ((fm327_fifo[LC] == sizeof(ndef_capability_container)) &&  // LC=5
                    (0 == memcmp(ndef_capability_container, fm327_fifo + DATA, fm327_fifo[LC]))) {
                    fm11_write_fifo(status_ok, 3);
                    fm11_write_reg(RF_TXEN_REG, 0x55);
                    current_file = CC_FILE;
                    printf("fm11_t4t: current_file=CC_FILE\n");
                } else if ((fm327_fifo[LC] == sizeof(ndef_id)) &&
                           (0 == memcmp(ndef_id, fm327_fifo + DATA, fm327_fifo[LC]))) {
                    fm11_write_fifo(status_ok, 3);
                    fm11_write_reg(RF_TXEN_REG, 0x55);
                    current_file = NDEF_FILE;
                    printf("fm11_t4t: current_file=NDEF_FILE\n");
                } else {
                    fm11_write_fifo(status_word2, 3);
                    fm11_write_reg(RF_TXEN_REG, 0x55);
                    current_file = NONE;
                    printf("fm11_t4t: current_file=NONE\n");
                }
            } else if (fm327_fifo[P1] == 0x04) {  // P1=3
                ret = fm11_write_fifo(status_ok, 3);
                if (ret != 0) {
                    printf("fm11_write_reg failed\r\n");
                }
                ret = fm11_write_reg(RF_TXEN_REG, 0x55);
                if (ret != 0) {
                    printf("fm11_write_reg failed\r\n");
                }
            } else {
                fm11_write_fifo(status_ok, 3);
                fm11_write_reg(RF_TXEN_REG, 0x55);
            }
        } else if (fm327_fifo[INS] == 0xB0) {  // INS=2
            if (current_file == CC_FILE) {
                printf("fm11_t4t: CC_FILE: fm11_write_fifo...\n");
                fm11_write_fifo(status_ok, 1);
                fm11_write_fifo(capability_container + (fm327_fifo[P1] << 8) + fm327_fifo[P2], fm327_fifo[LC]);
                fm11_write_fifo(&status_ok[1], 2);
                fm11_write_reg(RF_TXEN_REG, 0x55);
            } else if (current_file == NDEF_FILE) {
                memset(&xbuf[0], 0, sizeof(xbuf));
                // 1-byte from fm327_fifo[0]
                memcpy(&xbuf[0], &status_ok[0], 1);
                // fm327_fifo[LC]-bytes offset from fm327_fifo[0+0xP1P2],
                // kind of 2-bytes from ndef_file[0] and ndef_file[1]
                memcpy(&xbuf[1], &ndef_file[0] + (fm327_fifo[P1] << 8) + fm327_fifo[P2], fm327_fifo[LC]);
                // status_ok[1] status_ok[2]
                memcpy(&xbuf[0] + fm327_fifo[LC] + 1, status_ok + 1, 2);
                xlen = fm327_fifo[LC]+3;  //1-is Head, 2-Tail

                printf("fm11_t4t: NDEF_FILE: fm11_data_send: xlen[%d]\n", xlen);
                if (xlen == 5) {
                printf("          xbuf[0x%02X] [0x%02X][0x%02X] [0x%02X][0x%02X]\n",
                            xbuf[0], xbuf[1], xbuf[2], xbuf[3], xbuf[4]);
                } else {
                printf("          xbuf[0x%02X] [0x%02X][0x%02X][0x%02X][0x%02X]...[0x%02X][0x%02X][0x%02X]\n",
                            xbuf[0], xbuf[1], xbuf[2], xbuf[3], xbuf[4], xbuf[xlen-3], xbuf[xlen-2], xbuf[xlen-1]);
                }
                fm11_data_send(xlen, xbuf);
            } else {
                fm11_write_fifo(status_word, 3);
                fm11_write_reg(RF_TXEN_REG, 0x55);
            }
        } else if (fm327_fifo[INS] ==  0xD6) { // UPDATE_BINARY
            printf("fm11_t4t: UPDATE_BINARY: rfLen[%d]\n", rfLen);
            for (i=0; i < rfLen; i++) {
                printf("0x%02x ",fm327_fifo[i]);	
            }
            printf("\r\n");
            memcpy(ndef_file + (fm327_fifo[P1] << 8) + fm327_fifo[P2], fm327_fifo + DATA, fm327_fifo[LC]);
            fm11_write_fifo(status_ok, 3);
            fm11_write_reg(RF_TXEN_REG, 0x55);
        } else {
            fm11_data_send(rfLen, fm327_fifo);
        }
    }
}
/*app nfc demo*/
void nfcread(void* param)
{
#ifdef CHECK
    while (1) {
        rfLen = fm11_data_recv(fm327_fifo);		//读取rf数据(一帧)			
        if(rfLen > 0) {
            printf("--------------------------------------------------------------\n");
            printf("nfcread : fm327_fifo: [0x%02X]/CLA[0x%02X]/INS[0x%02X]/P1[0x%02X]/P2[0x%02X]/LC[0x%02X]...\n",
                fm327_fifo[0], fm327_fifo[1], fm327_fifo[2], fm327_fifo[3], fm327_fifo[4], fm327_fifo[5]);
            fm11_t4t();
            irq_data_in = 0;
        }
        usleep(1000);
    }
#endif

#ifdef INTERRUPT
    while (1) {
        if (FlagFirstFrame) {	
			rfLen = fm11_data_recv(fm327_fifo);		//读取rf数据(一帧)			
			if (rfLen > 0) {
			    fm11_t4t();
		    }
            irq_data_in = 0;
		}
        hi_sleep(1);
	}
#endif
}
