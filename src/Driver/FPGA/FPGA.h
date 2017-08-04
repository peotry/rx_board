/**********************************************************************
* Copyright (c) 2017,HuiZhou WELLAV Technology Co.,Ltd.
* All rights reserved.
*
* FileName FPGA.h
* Description : set of other moudles refer to REG moudle
* Author    : ruibin.zhang
* Modified  : huada.huang
* Reviewer  :
* Date      : 2017-02-20
* Record    :
*
**********************************************************************/
#ifndef FPGA_H_
#define FPGA_H_

#include "appGlobal.h"
#include "WV_CI.h"
//逻辑寄存器地址


#define CNT_135_M       0x80befc0  // 135M = 135*10^6 = 0x80befc0
#define TUNER_TS_INDEX  0xfc

#define PACKET_2_BIT(PacketNum) ((PacketNum) * 8 * 188)

#define IO_READ      0x04  // b'100
#define IO_WRITE     0x05  // b'101
#define MEM_READ     0x06  // b'110
#define MEM_WRITE    0x07  // b'111

#define CI_POW_ON     1
#define CI_POW_OFF    0



//test moudle addr                  // R/W?     bits    default value
#define VER_NUM             0x000   // R        32      0x201
#define VER_DATE            0x001   // R        32      0x17020916
#define TEST_REG1           0x002   // RW      12       0x0
#define TEST_REG2           0x003   // RW      32       0xa5a5a5a5

//slot id
#define REG_SLOT            0x10    // R        4

//DS2432 moudle addr                // R/W?     bits    default value
#define DS2432_WR_ADDR      0x360   // RW      32      0x0
#define DS2432_WR_DATA      0x361   // RW      8       0x0
#define DS2432_RD_DATA      0x362   // R        8
#define DS2432_RW_BUSY      0x363   // R        1

//tuner moudle addr                 // R/W?     bits    default value
#define TUNER_RESET_REG     0x11    //RW        4       0X0
#define TUNER_TYPE_REG      0x12    //R         3


//TSIP                               // R/W?  bits   default value
#define PHY_RESET_N         0x100    //R       1
#define TSIP_AUTO_DET       0x101    //RW      1      0x1
#define TSIP_SPEED_CONFIG   0x102    //RW      1      0x1
#define TSIP_SPEED_MODE     0x103    //R       1
#define TSIP_RX_ERR_CLR     0x104    //RW      1      0x0
#define TSIP_RX_ERR_CNT     0x105    //R       8
#define TSIP_CRC_ERR_CLR    0x106    //RW      1      0x0
#define TSIP_CRC_ERR_CNT    0x107    //R       8


#define TSIP_COM_SEL       0x108    //RW    6         0x0
#define TSIP_MAC_ADDH      0x109    //RW    24        0x0
#define TSIP_MAC_ADDL      0x10a    //RW    24        0x0
#define TSIP_IP_ADDR       0x10b    //RW    32        0x0
#define TSIP_UDP_PORT      0x10c    //RW    16        0x0
#define TSIP_FLT_IP0       0x10d    //RW    32        0x0
#define TSIP_FLT_IP1       0x10e    //RW    32        0x0
#define TSIP_FLT_IP2       0x10f    //RW    32        0x0
#define TSIP_FLT_IP3       0x110    //RW    32        0x0
#define TSIP_MAC_CHAN      0x111    //RW    8         249


#define TSIP_ARP_TIME      0x112    //RW    32        125000000
#define TSIP_CPU_WREN      0x120    //RW    1         0 
#define TSIP_CPU_ADDR      0x121    //RW    9         0 
#define TSIP_DAT_H         0x122    //RW    32        0 
#define TSIP_DAT_L         0x123    //RW    32        0 
#define TSIP_CPU_RBK_H     0x124    //R     32
#define TSIP_CPU_RBK_L     0x125    //RW    32

#define TSIP_HDR_MODE      0x130   //RW     1         1 
#define TSIP_OWN_HDRI      0x131   //RW     1         0 
#define TSIP_RTP_USEI      0x132   //RW     1         0 
#define TSIP_LEN_SELI      0x133   //RW     1         0 
#define TSIP_PAY_NUMI      0x134   //RW     3         7
#define TSIP_OWN_HDRO      0x135   //RW     4         4'b1111
#define TSIP_RTP_USEO      0x136   //RW     4         0 
#define TSIP_LEN_SELO      0x137   //RW     4         0
#define TSIP_PAY_NUMO      0x138   //RW     12        0xfff  
//TS add head
#define ADD_HEAD_WR_EN     0x160   //RW     1         0
#define ADD_HEAD_ADDR      0x161   //RW     8         0
#define ADD_HEAD_DATA      0x162   //RW     2         0
#define ADD_HEAD_RDBK      0x163   //R      2

//Scan TS moudle addr                         // R/W?     bits    default value
//逻辑有4个流搜索模块，可以同时进行4个流的搜索
#define CHANNEL_CONFIG_0              0x200   // R/W      8       0x0 :要搜的流的通道号
#define CHANNEL_CONFIG_1              0x201   // R/W      8       0x0
#define CHANNEL_CONFIG_2              0x202   // R/W      8       0x0
#define CHANNEL_CONFIG_3              0x203   // R/W      8       0x0

#define PID_CONFIG_0                  0x204   // R/W      13      0x0
#define PID_CONFIG_1                  0x205   // R/W      13      0x0
#define PID_CONFIG_2                  0x206   // R/W      13      0x0
#define PID_CONFIG_3                  0x207   // R/W      13      0x0

#define TID_CONFIG_0                  0x208   // R/W      8       0x0
#define TID_CONFIG_1                  0x209   // R/W      8       0x0
#define TID_CONFIG_2                  0x20A   // R/W      8       0x0
#define TID_CONFIG_3                  0x20B   // R/W      8       0x0

#define PID_EXIST_FLAG_CLEAR_EN_0     0x20C   // R/W      1       0x0
#define PID_EXIST_FLAG_CLEAR_EN_1     0x20D   // R/W      1       0x0
#define PID_EXIST_FLAG_CLEAR_EN_2     0x20E   // R/W      1       0x0
#define PID_EXIST_FLAG_CLEAR_EN_3     0x20F   // R/W      1       0x0

#define PID_EXIST_FLAG_ADDR_0         0x210   // R/W      8       0x0
#define PID_EXIST_FLAG_ADDR_1         0x211   // R/W      8       0x0
#define PID_EXIST_FLAG_ADDR_2         0x212   // R/W      8       0x0
#define PID_EXIST_FLAG_ADDR_3         0x213   // R/W      8       0x0

#define PID_EXIST_FLAG_READ_BACK_0    0x214   // R        32      
#define PID_EXIST_FLAG_READ_BACK_1    0x215   // R        32      
#define PID_EXIST_FLAG_READ_BACK_2    0x216   // R        32      
#define PID_EXIST_FLAG_READ_BACK_3    0x217   // R        32      

#define CONFIG_CHANGED_FLAG_0         0x218   // R/W      1       0x0
#define CONFIG_CHANGED_FLAG_1         0x219   // R/W      1       0x0
#define CONFIG_CHANGED_FLAG_2         0x21A   // R/W      1       0x0
#define CONFIG_CHANGED_FLAG_3         0x21B   // R/W      1       0x0

#define READ_PACKET_DONE_FLAG_0       0x21C   // R/W      1       0x0
#define READ_PACKET_DONE_FLAG_1       0x21D   // R/W      1       0x0
#define READ_PACKET_DONE_FLAG_2       0x21E   // R/W      1       0x0
#define READ_PACKET_DONE_FLAG_3       0x21F   // R/W      1       0x0

#define PACKET_ADDR_0                 0x220   // R/W      6       0x0
#define PACKET_ADDR_1                 0x221   // R/W      6       0x0
#define PACKET_ADDR_2                 0x222   // R/W      6       0x0
#define PACKET_ADDR_3                 0x223   // R/W      6       0x0

#define PACKET_NUM_0                  0x224   // R        4       
#define PACKET_NUM_1                  0x225   // R        4       
#define PACKET_NUM_2                  0x226   // R        4       
#define PACKET_NUM_3                  0x227   // R        4       

#define PACKET_READ_BACK_0            0x228   // R        32      0x0
#define PACKET_READ_BACK_1            0x229   // R        32      0x0
#define PACKET_READ_BACK_2            0x22A   // R        32      0x0
#define PACKET_READ_BACK_3            0x22B   // R        32      0x0

//TS monitor                           // R/W?     bits    default value
#define STR_MMNT_POINT         0x240
#define STR_MMNT_RSID          0x241   //R/W        8        0x0
#define STR_MMNT_TIMER         0x242   //R/W        32       0x0
#define STR_MMNT_CLR_CC        0x243   //R/W        1        0X0
#define STR_MMNT_ADDR0         0x244   //R/W        13       0X0
#define STR_MMNT_PID_RATE      0x245   //R          16
#define STR_MMNT_PID_CC        0x246   //R          8
#define STR_MMNT_ADDR1         0x247   //R/W        8
#define STR_MMNT_TOTLR         0x248   //R          18
#define STR_MMNT_EFFER         0x249   //R          18


//CI modules                           // R/W?     bits    default value
#define CI_INT                 0x300   //R          1
#define CI_CDD                 0x301   //R          2
#define CI_POW                 0x302   //RW         2         0
#define CI_RST                 0x303   //RW         2         0
#define CI_SEL                 0x304   //RW         2         0
#define CI_CMD                 0x305   //RW         3         0
#define CI_RDEN                0x306   //RW         1         0
#define CI_WREN                0x307   //RW         1         0
#define CI_ADDR                0x308   //RW         15        0
#define CI_DATA                0x309   //RW         8         0
#define CI_RDBK                0x30a   //R          8
#define CI_CLK_PAR             0x30b   //RW         8         0
#define CI_BYPASS              0x30c
#define CI_SID_SEL             0x30d
#define CI_ERR_CLR             0x30e
#define CI_ERR_CNT             0x30f
#define CI_PES_CLR             0x310
#define CI_PES_STS             0x311

#define CI0_BYPASS             0x30c   //RW         1         1
#define CI1_BYPASS             0x30d   //RW         1         1
#define CI0_SID_SEL            0x30e   //RW         8         0
#define CI1_SID_SEL            0x30f   //RW         8         0
#define CI0_PES_CLR            0X310
#define CI1_PES_CLR            0X311

#define CI_PID_DETECT0_A       0x312   //RW         13        0
#define CI_PID_DETECT1_A       0x313   //RW         13        0
#define CI_PID_DETECT2_A       0x314   //RW         13        0
#define CI_PID_DETECT3_A       0x315   //RW         13        0
#define CI_PID_DETECT4_A       0x316   //RW         13        0
#define CI_PID_DETECT5_A       0x317   //RW         13        0
#define CI_PID_DETECT6_A       0x318   //RW         13        0
#define CI_PID_DETECT7_A       0x319   //RW         13        0

#define CI_PID_DETECT0_B       0x31a   //RW         13        0
#define CI_PID_DETECT1_B       0x31b   //RW         13        0
#define CI_PID_DETECT2_B       0x31c   //RW         13        0
#define CI_PID_DETECT3_B       0x31d   //RW         13        0
#define CI_PID_DETECT4_B       0x31e   //RW         13        0
#define CI_PID_DETECT5_B       0x31f   //RW         13        0
#define CI_PID_DETECT6_B       0x320   //RW         13        0
#define CI_PID_DETECT7_B       0x321   //RW         13        0

#define CI2_PID_DETECT0_A       0x322   //RW         13        0
#define CI2_PID_DETECT1_A       0x323   //RW         13        0
#define CI2_PID_DETECT2_A       0x324   //RW         13        0
#define CI2_PID_DETECT3_A       0x325   //RW         13        0
#define CI2_PID_DETECT4_A       0x326   //RW         13        0
#define CI2_PID_DETECT5_A       0x327   //RW         13        0
#define CI2_PID_DETECT6_A       0x328   //RW         13        0
#define CI2_PID_DETECT7_A       0x329   //RW         13        0

#define CI2_PID_DETECT0_B       0x32a   //RW         13        0
#define CI2_PID_DETECT1_B       0x32b   //RW         13        0
#define CI2_PID_DETECT2_B       0x32c   //RW         13        0
#define CI2_PID_DETECT3_B       0x32d   //RW         13        0
#define CI2_PID_DETECT4_B       0x32e   //RW         13        0
#define CI2_PID_DETECT5_B       0x32f   //RW         13        0
#define CI2_PID_DETECT6_B       0x330   //RW         13        0
#define CI2_PID_DETECT7_B       0x331   //RW         13        0


#define CI0_PES_STS            0x322   //R          8         0
#define CI1_PES_STS            0x323   //R          8         0

//Input Lookup Table
#define LUT_SWAP               0x340   // R/W      1       0x0 :0-1-0切换，所有entry更新完后才切换
#define LUT_WRITE_EN           0x341   // R/W      1       0x0 :0-1-0使能
#define LUT_ADDR               0x342   // R/W      13      0x0 :[13:12]：0~3代表4个通道
                                                              //[11]:0表示配置复用查找表；1表示配置包头B/M信息
                                                              //[10:0]：当[11]=0时，0~2047个entry表示配置复用查找表；
                                                              //        当[11]=1时，0~255表示配置包头B/M信息位
#define LUT_DATA               0x343   // R/W      24      0x0 :当地址[11]=0时：
                                                              //[23]：en，如果本entry有效，置1，否则置0
                                                              //[22:13]：Src_Stream_id索引号
                                                              //[12:0]：Src_PID索引号
                                                              //软件需根据[22:0]数据从小到大进行排序
                                                              //当地址[11]=1时： 
                                                              //[23:2]：保留
                                                              //[1:0]：表示B/M信息位
#define LUT_READ_BACK          0x344   // R        24      0x0 //同LUT_DATA
#define LUT_PINGPONG_FLAG      0x345   // R        4       0x0 :0/1分别表示不同的存储地址空间
                                                                     //[0]：对应输入查找表0
                                                                     //[1]：对应输入查找表1
                                                                     //[2]：对应输入查找表2
                                                                     //[3]：对应输入查找表3

//Output Lookup Table for CI MUX
#define OUTPUT_LUT_SWAP               0x400   // R/W      1       0x0 :0-1-0切换，所有entry更新完后才切换
#define OUTPUT_LUT_WRITE_EN           0x401   // R/W      1       0x0 :0-1-0使能
#define OUTPUT_LUT_ADDR               0x402   // R/W      12      //[11]:0表示配置直通查找表；1表示配置复用查找表
#define OUTPUT_LUT_DATA_H             0x403   // R/W      29
#define OUTPUT_LUT_DATA_L             0x404   // R/W      32  0x0 :当地址[11]=0时：
                                                                      //[34]：en，如果本entry有效，置1，否则置0
                                                                      //[33:26]:Num
                                                                      //[25:20]:Src_Slot
                                                                      //[19:10]:Src_Stream_id
                                                                      //[9:0]：Dest_Stream_id
                                                                  //当地址[11]=1时： 
                                                                      //[60]：en，如果本entry有效，置1，否则置0
                                                                      //[59:52]:Num
                                                                      //[51:46]:Src_Slot
                                                                      //[45:36]:Src_Stream_id
                                                                      //[35:23]:Src_pid
                                                                      //[22:13]:Dest_Stream_id
                                                                      //[12:0]:Dest_pid
                                                                  //都需要根据Src来排序(从小到大，Slot-->Stream-->pid)
#define OUTPUT_LUT_READ_BACK_H        0x405   // R        29      0x0 //同OUTPUT_LUT_DATA_H
#define OUTPUT_LUT_READ_BACK_L        0x406   // R        32      0x0 //同OUTPUT_LUT_DATA_L
#define OUTPUT_LUT_PINGPONG_FLAG      0x407   // R        4       0x0 :0/1分别表示不同的存储地址空间

//SI table ADDR
#define PSI_DBG_MODE         0x410       
#define PSI_CPU_OVER         0x411
#define PSI_CPU_WREN         0x412
#define PSI_CPU_ADDR         0x413
#define PSI_CPU_DATA         0x414
#define PSI_CPU_RDBK         0x415


typedef enum
{
    TUNER_BOARD_C_OR_DTMB = 0,
    TUNER_BOARD_S2,
    
    TUNER_BOARD_TYPE_CNT,
    TUNER_BOARD_TYPE_UNKNOW = 0xFF
}TUNER_BOARD_TYPE_enum;

void FPGA_REG_Map();
void FPGA_Init(void);
void FPGA_REG_Write(U32 u32Address, U32 u32Data);
void FPGA_REG_Read(U32 u32Address, U32 *pu32Data);

void FPGA_REG_Write_Without_Delay(U32 u32Address, U32 u32Data);
void FPGA_REG_Read_Without_Delay(U32 u32Address, U32 *pu32Data);

U32 FPGA_GetLogicVersion(void);
U32 FPGA_GetSlotID(void);

/* Tuner modules*/
U32 FPGA_GetTunerType(void);
void FPGA_SetTunerReset(U8 u8port);
void FPGA_SetMacAddr(U8 *u8MacAddr);
void FPGA_GetMacAddr(U8 *u8MacAddr);
void FPGA_setTsipAddr(U8 *u8TsipAddr);


/* CI modules*/
void FPGA_readCiInterrupt(U32 *u32IntStatus);
void FPGA_checkCiRegister(U32 *u32RegStatus);
void FPGA_setCiPower(U8 slot, U8 u8Status);
void FPGA_CiReset(U8 u8Slot);
void FPGA_setCiSelect(U32 u32sel);
void FPGA_clearCiSelect(U32 u32sel);
void FPGA_CiReadCmd(U32 u32Command);
void FPGA_CiReadEnable();
void FPGA_CiReadDisable(void);
void FPGA_CiWriteAddr(U32 u32Addr);
void FPGA_CiWriteEnable(U32 u32Command);
void FPGA_CiWriteDisable(void);
void FPGA_CiWrite(U32 u32Addr, U32 u32data);
void FPGA_CiRead(U32 *u32data);
void FPGA_setCiClk(U32 u32ClockValue);
void FPGA_setCAMbypassOn(U8 u8Slot);
void FPGA_setCAMbypassOff(U8 u8Slot);
void FPGA_setCAM0bypassOn();
void FPGA_setCAM0bypassOff();
void FPGA_setCAM1bypassOn();
void FPGA_setCAM1bypassOff();

void FPGA_setCISid(U8 u8Slot, U32 u32StreamID);
void FPGA_setCI0Sid(U32 u32StreamID);
void FPGA_setCI1Sid(U32 u32StreamID);

void FPGA_getCiPidDetect0_A(U32 *u32Pid);
void FPGA_getCiPidDetect1_A(U32 *u32Pid);
void FPGA_getCiPidDetect2_A(U32 *u32Pid);
void FPGA_getCiPidDetect3_A(U32 *u32Pid);
void FPGA_getCiPidDetect4_A(U32 *u32Pid);
void FPGA_getCiPidDetect5_A(U32 *u32Pid);
void FPGA_getCiPidDetect6_A(U32 *u32Pid);
void FPGA_getCiPidDetect7_A(U32 *u32Pid);
void FPGA_getCiPidDetect0_B(U32 *u32Pid);
void FPGA_getCiPidDetect1_B(U32 *u32Pid);
void FPGA_getCiPidDetect2_B(U32 *u32Pid);
void FPGA_getCiPidDetect3_B(U32 *u32Pid);
void FPGA_getCiPidDetect4_B(U32 *u32Pid);
void FPGA_getCiPidDetect5_B(U32 *u32Pid);
void FPGA_getCiPidDetect6_B(U32 *u32Pid);
void FPGA_getCiPidDetect7_B(U32 *u32Pid);
void FPGA_setCiPidDetect0_A(U32 *u32Pid);
void FPGA_setCiPidDetect1_A(U32 *u32Pid);
void FPGA_setCiPidDetect2_A(U32 *u32Pid);
void FPGA_setCiPidDetect3_A(U32 *u32Pid);
void FPGA_setCiPidDetect4_A(U32 *u32Pid);
void FPGA_setCiPidDetect5_A(U32 *u32Pid);
void FPGA_setCiPidDetect6_A(U32 *u32Pid);
void FPGA_setCiPidDetect7_A(U32 *u32Pid);
void FPGA_setCiPidDetect0_B(U32 *u32Pid);
void FPGA_setCiPidDetect1_B(U32 *u32Pid);
void FPGA_setCiPidDetect2_B(U32 *u32Pid);
void FPGA_setCiPidDetect3_B(U32 *u32Pid);
void FPGA_setCiPidDetect4_B(U32 *u32Pid);
void FPGA_setCiPidDetect5_B(U32 *u32Pid);
void FPGA_setCiPidDetect6_B(U32 *u32Pid);
void FPGA_setCiPidDetect7_B(U32 *u32Pid);


void FPGA_getCi2PidDetect0_A(U32 *u32Pid);
void FPGA_getCi2PidDetect1_A(U32 *u32Pid);
void FPGA_getCi2PidDetect2_A(U32 *u32Pid);
void FPGA_getCi2PidDetect3_A(U32 *u32Pid);
void FPGA_getCi2PidDetect4_A(U32 *u32Pid);
void FPGA_getCi2PidDetect5_A(U32 *u32Pid);
void FPGA_getCi2PidDetect6_A(U32 *u32Pid);
void FPGA_getCi2PidDetect7_A(U32 *u32Pid);
void FPGA_getCi2PidDetect0_B(U32 *u32Pid);
void FPGA_getCi2PidDetect1_B(U32 *u32Pid);
void FPGA_getCi2PidDetect2_B(U32 *u32Pid);
void FPGA_getCi2PidDetect3_B(U32 *u32Pid);
void FPGA_getCi2PidDetect4_B(U32 *u32Pid);
void FPGA_getCi2PidDetect5_B(U32 *u32Pid);
void FPGA_getCi2PidDetect6_B(U32 *u32Pid);
void FPGA_getCi2PidDetect7_B(U32 *u32Pid);
void FPGA_setCi2PidDetect0_A(U32 *u32Pid);
void FPGA_setCi2PidDetect1_A(U32 *u32Pid);
void FPGA_setCi2PidDetect2_A(U32 *u32Pid);
void FPGA_setCi2PidDetect3_A(U32 *u32Pid);
void FPGA_setCi2PidDetect4_A(U32 *u32Pid);
void FPGA_setCi2PidDetect5_A(U32 *u32Pid);
void FPGA_setCi2PidDetect6_A(U32 *u32Pid);
void FPGA_setCi2PidDetect7_A(U32 *u32Pid);
void FPGA_setCi2PidDetect0_B(U32 *u32Pid);
void FPGA_setCi2PidDetect1_B(U32 *u32Pid);
void FPGA_setCi2PidDetect2_B(U32 *u32Pid);
void FPGA_setCi2PidDetect3_B(U32 *u32Pid);
void FPGA_setCi2PidDetect4_B(U32 *u32Pid);
void FPGA_setCi2PidDetect5_B(U32 *u32Pid);
void FPGA_setCi2PidDetect6_B(U32 *u32Pid);
void FPGA_setCi2PidDetect7_B(U32 *u32Pid);


//bitrate
void FPGA_getTunerTotalBitRate(U8 u8port, U32 *u32totalBitrate);
void FPGA_getTunerEffectBitRate(U8 u8port, U32 *u32EffectBitrate);
U32 FPGA_GetPIDBitRate(U16 u16PID);

//PSI/SI
void FPGA_SendSIPacket2Logic(U32 *pu32Data, U32 u32DataLen);

#endif
