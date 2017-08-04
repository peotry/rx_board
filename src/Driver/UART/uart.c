/******************************************************************************
* Copyright (c) 2017, WELLAV Technology Co.,Ltd.
* All rights reserved.
*
* FileName uart.c
* Description : UART module
* Author    : ruibin.zhang
* Modified  :
* Reviewer  :
* Date      : 2017-02-28
* Record    :
*
******************************************************************************/
#include <semaphore.h>
#include <fcntl.h>
#include <termios.h>
#include "uart.h"
#include "WV_log.h"

volatile int g_UART0Fd = -1;
volatile int g_UART1Fd = -1;

static sem_t  g_stUartSem;


/*****************************************************************************
  Function:     uart1_Flush
  Description:  flush uart1 in and out buff
  Input:        none
  Output:       none
  Return:       none
  Author:       huada.huang
*****************************************************************************/
void uart1_Flush()
{
    tcflush(g_UART1Fd, TCIOFLUSH);
}


/*****************************************************************************
  Function:     uart_LockInit
  Description:  none
  Input:        none
  Output:       none
  Return:       none
  Author:       ruibin.zhang
*****************************************************************************/
static void uart_LockInit(void)
{
    sem_init(&g_stUartSem, 0, 1);
}


/*****************************************************************************
  Function:     uart_Lock
  Description:  none
  Input:        none
  Output:       none
  Return:       none
  Author:       ruibin.zhang
*****************************************************************************/
static void uart_Lock(void)
{
    sem_wait(&g_stUartSem);
}


/*****************************************************************************
  Function:     uart_UnLock
  Description:  none
  Input:        none
  Output:       none
  Return:       none
  Author:       ruibin.zhang
*****************************************************************************/
static void uart_UnLock(void)
{
    sem_post(&g_stUartSem);
}


/*****************************************************************************
  Function:     uart_Open
  Description:  uart dev open
  Input:        const char *dev_name       device name,such as "/dev/ttyPS0"
  Output:       none
  Return:       UARTErrCode
  Author:       ruibin.zhang
*****************************************************************************/
int uart_OpenDev(const char *dev_name)
{
    if(NULL == dev_name)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_UART, "Param error!");
        return -1;
    }

    int fd = -1;

    uart_LockInit();

    fd = open(dev_name, O_RDWR | O_NOCTTY | O_NDELAY);// 璇诲啓 | 闈炵粓绔� | 闈為樆濉�
    if(fd < 0)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_UART, "%s: open error!", dev_name);
        return -1;
    }
    else
    {
        LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_UART, "%s: open success!", dev_name);
        return fd;
    }

}


/*****************************************************************************
  Function:     uart_Set
  Description:  set uart
  Input:        int fd              -       涓插彛鎵撳紑鍙ユ焺
                int data_bits       -       鏁版嵁浣嶅 7bit/8bit
                int stop_bits       -       鍋滄浣� 1 / 2
                char parity         -       濂囧伓鏍￠獙
  Output:       none
  Return:       UARTErrCode
  Author:       ruibin.zhang
*****************************************************************************/
UARTErrCode uart_Set(int fd, int speed, int data_bits, int stop_bits, char parity)
{
    struct termios newtio, oldtio;

    /* get the default setting for the uart */
    if((fd < 0) || (tcgetattr(fd, &oldtio) != 0))
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_UART, "The uart is not open!");
        return WV_UART_GET_OLDCONFIG_ERR;
    }

    /* CLOCAL : no modem;
     * CREAD  : can read from the uart
     */
    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag |= CLOCAL | CREAD;

#ifdef URAT_HARDWARE_CONTROL
    switch(flow_ctrl)
    {
        /* no hardware flow control */
        case 0:
            newtio.c_cflag &= ~CRTSCTS;
            printf(" no hardware flow control!\n");
            break;
        /* with hardware flow control */
        case 1:
            newtio.c_cflag |= CRTSCTS;
            printf(" with hardware flow control!\n");
            break;
        /* software flow control */
        case 2:
            newtio.c_cflag |= IXON|IXOFF|IXANY;
            printf(" software flow control!\n");
            break;
        default:
            newtio.c_cflag &= ~(CRTSCTS|IXON|IXOFF|IXANY);
            printf("no any flow control!");
    }
#endif

    /* data width */
    switch(data_bits)
    {
        case 7:
            newtio.c_cflag |= CS7;
            break;
        case 8:
            newtio.c_cflag |= CS8;
            break;
    }

    /* set one bit stop, or it is two bits */
    if(stop_bits == 1)
        newtio.c_cflag &=~CSTOPB;
    else
        newtio.c_cflag |=CSTOPB;

    /* parity bit setting */
    switch(parity)
    {
        // No parity
        case 'n':
        case 'N':
            newtio.c_cflag &= ~PARENB;          /* Clear parity enable */
            newtio.c_iflag &= ~INPCK;           /* Enable parity checking */
            break;
        // Odd parity
        case 'o':
        case 'O':
            newtio.c_cflag |= (PARODD | PARENB); /* 璁剧疆涓哄鏁堥獙*/
            newtio.c_iflag |= INPCK;             /* Disable parity checking */
            break;
        // Even parity
        case 'e':
        case 'E':
            newtio.c_cflag |= PARENB;           /* Enable parity */
            newtio.c_cflag &= ~PARODD;          /* 杞崲涓哄伓鏁堥獙*/
            newtio.c_iflag |= INPCK;            /* Disable parity checking */
            break;
        // Space parity is setup the same as no parity
        case 'S':
        case 's':                               /*as no parity*/
            newtio.c_cflag &= ~PARENB;
            break;
        default:
            newtio.c_cflag &= ~PARENB;          /* Clear parity enable */
            newtio.c_iflag &= ~INPCK;           /* Enable parity checking */
            break;
    }

    /* speed set*/
    switch(speed)
    {
        case 2400:
            cfsetispeed(&newtio, B2400);
            cfsetospeed(&newtio, B2400);
            break;
        case 4800:
            cfsetispeed(&newtio, B4800);
            cfsetospeed(&newtio, B4800);
            break;
        case 9600:
            cfsetispeed(&newtio, B9600);
            cfsetospeed(&newtio, B9600);
            break;
        case 115200:
            cfsetispeed(&newtio, B115200);
            cfsetospeed(&newtio, B115200);
            break;
        default:
            cfsetispeed(&newtio, B115200);
            cfsetospeed(&newtio, B115200);
            break;
    }

    //闈為樆濉� 妯″紡 TODO 濡傛灉闇�瑕佷慨鏀� 鍙傝�僪ttp://blog.csdn.net/mj5742356/article/details/7879345
    newtio.c_cc[VTIME] = 0;
    newtio.c_cc[VMIN]  = 0;
    tcflush(fd, TCIFLUSH);

    if(tcsetattr(fd, TCSANOW, &newtio) != 0)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_UART, "uart set failed!");
        return WV_UART_SET_CONFIG_ERR;
    }

    LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_UART, "uart set success!");
    return WV_UART_SUCCESS;
}


/*****************************************************************************
  Function:     uart_Read
  Description:  read uart
  Input:        int fd              -       涓插彛鎵撳紑鍙ユ焺
  Output:       U8 *pReadBuf        -       璇诲嚭鏉ョ殑鍐呭
                S32 * ps32Readlen   -       璇诲嚭鏉ョ殑闀垮害
  Return:       UARTErrCode
  Author:       ruibin.zhang
  Example:      U8 pReadBuf[UART_RD_LENGTH];
                S32 s32Readlen;
                uart_Read(g_UART0Fd, pReadBuf, &s32Readlen);
                if(s32Readlen > 0)
                    printf("[%s]", pReadBuf);
*****************************************************************************/
UARTErrCode uart_Read(int fd, U8 *pReadBuf, S32 * ps32Readlen)
{
    if(fd < 0)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_UART, "The uart is not open!");
        return WV_UART_PARAM_ERR;
    }

    if((NULL == pReadBuf) || (NULL == ps32Readlen))
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_UART, "Param is NULL!");
        return WV_UART_PARAM_ERR;
    }

    uart_Lock();

    memset(pReadBuf, 0, UART_RD_LENGTH);
    *ps32Readlen = read(fd, pReadBuf, UART_RD_LENGTH);

    if(*ps32Readlen > 0)
    {
        LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_UART, "uart read len %d!", *ps32Readlen);
        LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_UART, "[%s]", pReadBuf);
    }
    uart_UnLock();
    return WV_UART_SUCCESS;
}


/*****************************************************************************
  Function:     uart_Write
  Description:  write uart
  Input:        int fd              -       涓插彛鎵撳紑鍙ユ焺
                U8 *pWriteBuf       -       鍐欒繘鍘荤殑鍐呭
                S32 s32Writelen     -       鍐欒繘鍘荤殑闀垮害
  Output:       none
  Return:       UARTErrCode
  Author:       ruibin.zhang
*****************************************************************************/
UARTErrCode uart_Write(int fd, U8 *pWriteBuf, S32 s32Writelen)
{
    if(fd < 0)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_UART, "The uart is not open!");
        return WV_UART_PARAM_ERR;
    }

    if(NULL == pWriteBuf)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_UART, "Param is NULL!");
        return WV_UART_PARAM_ERR;
    }
    uart_Lock();
    S32 s32Writtenlen = 0;
    s32Writtenlen = write(fd, pWriteBuf, s32Writelen);
    if(s32Writtenlen != s32Writelen)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_UART, "uart written is't match len %d/%d!", s32Writtenlen, s32Writelen);
    }
    uart_UnLock();
    return WV_UART_SUCCESS;
}

/*****************************************************************************
  Function:     uart0_Init
  Description:  uart0 dev init
  Input:        none
  Output:       none
  Return:       UARTErrCode
  Author:       ruibin.zhang
*****************************************************************************/
UARTErrCode uart0_Init()
{
    UARTErrCode ret = WV_UART_SUCCESS;
    if(g_UART0Fd > 0)
    {
       LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_UART, "%s: already open!", UART0_DEV_NAME);
       return WV_UART_ALREADY_OPEN_ERR;
    }

    g_UART0Fd = uart_OpenDev(UART0_DEV_NAME);
    if(g_UART0Fd < 0)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_UART, "%s: open error!", UART0_DEV_NAME);
        return WV_UART_OPEN_ERR;
    }
    else
    {
        LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_UART, "%s: open success!", UART0_DEV_NAME);
    }

    ret = uart_Set(g_UART0Fd, 115200, 8, 1, 'n');

    return ret;
}


/*****************************************************************************
  Function:     uart_Read
  Description:  read uart0
  Input:        none
  Output:       U8 *pReadBuf        -       璇诲嚭鏉ョ殑鍐呭
                S32 * ps32Readlen   -       璇诲嚭鏉ョ殑闀垮害
  Return:       UARTErrCode
  Author:       ruibin.zhang
  Example:      U8 pReadBuf[UART_RD_LENGTH];
                S32 s32Readlen;
                uart_Read(g_UART0Fd, pReadBuf, &s32Readlen);
                if(s32Readlen > 0)
                    printf("[%s]", pReadBuf);
*****************************************************************************/
UARTErrCode uart0_Read(U8 *pReadBuf, S32 * ps32Readlen)
{
    return  uart_Read(g_UART0Fd, pReadBuf, ps32Readlen);
}


/*****************************************************************************
  Function:     uart0_Write
  Description:  write uart0
  Input:        U8 *pWriteBuf       -       鍐欒繘鍘荤殑鍐呭
                S32 s32Writelen     -       鍐欒繘鍘荤殑闀垮害
  Output:       none
  Return:       UARTErrCode
  Author:       ruibin.zhang
*****************************************************************************/
UARTErrCode uart0_Write(U8 *pWriteBuf, S32 s32Writelen)
{
    return  uart_Write(g_UART0Fd, pWriteBuf, s32Writelen);
}


/*****************************************************************************
  Function:     uart1_Init
  Description:  uart1 dev init
  Input:        none
  Output:       none
  Return:       UARTErrCode
  Author:       ruibin.zhang
*****************************************************************************/
UARTErrCode uart1_Init()
{
    UARTErrCode ret = WV_UART_SUCCESS;
    if(g_UART1Fd > 0)
    {
       LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_UART, "%s: already open!", UART1_DEV_NAME);
       return WV_UART_ALREADY_OPEN_ERR;
    }

    g_UART1Fd = uart_OpenDev(UART1_DEV_NAME);
    if(g_UART1Fd < 0)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_UART, "%s: open error!", UART1_DEV_NAME);
        return WV_UART_OPEN_ERR;
    }
    else
    {
        LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_UART, "%s: open success!", UART1_DEV_NAME);
    }

    ret = uart_Set(g_UART1Fd, 115200, 8, 1, 'n');

    return ret;
}


/*****************************************************************************
  Function:     uart_Read
  Description:  read uart1
  Input:        none
  Output:       U8 *pReadBuf        -       璇诲嚭鏉ョ殑鍐呭
                S32 * ps32Readlen   -       璇诲嚭鏉ョ殑闀垮害
  Return:       UARTErrCode
  Author:       ruibin.zhang
  Example:      U8 pReadBuf[UART_RD_LENGTH];
                S32 s32Readlen;
                uart1_Read(pReadBuf, &s32Readlen);
                if(s32Readlen > 0)
                    printf("[%s]", pReadBuf);
*****************************************************************************/
UARTErrCode uart1_Read(U8 *pReadBuf, S32 * ps32Readlen)
{
    return  uart_Read(g_UART1Fd, pReadBuf, ps32Readlen);
}


/*****************************************************************************
  Function:     uart1_Write
  Description:  write uart1
  Input:        U8 *pWriteBuf       -       鍐欒繘鍘荤殑鍐呭
                S32 s32Writelen     -       鍐欒繘鍘荤殑闀垮害
  Output:       none
  Return:       UARTErrCode
  Author:       ruibin.zhang
*****************************************************************************/
UARTErrCode uart1_Write(U8 *pWriteBuf, S32 s32Writelen)
{
    return  uart_Write(g_UART1Fd, pWriteBuf, s32Writelen);
}

#if 0
//Example uart0
void uart0_ReadExample()
{
    U8 pReadBuf[UART_RD_LENGTH];
    S32 s32Readlen;

    uart0_Init();

    while(1)
    {
        uart0_Read(pReadBuf, &s32Readlen);
        usleep(1000000);
    }

}

void uart0_WriteExample()
{
    U8 Writebuf[21] = "uart0 write success!\n";

    uart0_Init();

    while(1)
    {
        uart0_Write(Writebuf, 21);
        usleep(1000000);
    }

}

void uart1_ReadExample()
{
    U8 pReadBuf[UART_RD_LENGTH];
    S32 s32Readlen;

    uart1_Init();

    while(1)
    {
        uart1_Read(pReadBuf, &s32Readlen);
        usleep(1000000);
    }

}


void uart1_WriteExample()
{
    U8 Writebuf[21] = "uart1 write success!\n";
/*    U8 pReadBuf[UART_RD_LENGTH];
    S32 s32Readlen;*/

    uart1_Init();

    while(1)
    {
        uart1_Write(Writebuf, 21);
        //uart_Read(g_UART1Fd, pReadBuf, &s32Readlen);
        usleep(2000000);
    }

}

#endif
