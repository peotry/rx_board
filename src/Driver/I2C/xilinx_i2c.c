/******************************************************************************
* Copyright (c) 2017, WELLAV Technology Co.,Ltd.
* All rights reserved.
*
* FileName xilinx_i2c.c
* Description :xlinx i2c controller driver
* Author    : guanghui.chen
* Modified  : ruibin.zhang
* Reviewer  :
* Date      : 2017-02-26
* Record    :
*
******************************************************************************/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <semaphore.h>
#include "xilinx_i2c.h"
#include "WV_log.h"

volatile int g_I2C0Fd = -1;
volatile int g_I2C1Fd = -1;

static sem_t  g_stI2C0Sem;
static sem_t  g_stI2C1Sem;

/*****************************************************************************
  Function:     i2c_LockInit
  Description:  none
  Input:        none
  Output:       none
  Return:       none
  Author:       ruibin.zhang
*****************************************************************************/
static void i2c_LockInit(sem_t *sem)
{
    sem_init(sem, 0, 1);
}

/*****************************************************************************
  Function:     i2c_Lock
  Description:  none
  Input:        none
  Output:       none
  Return:       none
  Author:       ruibin.zhang
*****************************************************************************/
static void i2c_Lock(sem_t *sem)
{
    sem_wait(sem);
}

/*****************************************************************************
  Function:     i2c_UnLock
  Description:  none
  Input:        none
  Output:       none
  Return:       none
  Author:       ruibin.zhang
*****************************************************************************/
static void i2c_UnLock(sem_t *sem)
{
    sem_post(sem);
}

/*****************************************************************************
  Function:     i2c_smbus_access
  Description:  i2c access func
  Input:        int file
                char read_write
                unsigned char command
                int size
                union i2c_smbus_data *data
  Output:       none
  Return:       SUCCESS 0;FAILURE -1
  Author:       guanghui.chen
*****************************************************************************/
static int i2c_smbus_access(int file, char read_write, unsigned char command,
                                     int size, union i2c_smbus_data *data)
{
    struct i2c_smbus_ioctl_data args;

    args.read_write = read_write;
    args.command = command;
    args.size = size;
    args.data = data;
    return ioctl(file,I2C_SMBUS,&args);
}

/*****************************************************************************
  Function:     i2c_smbus_access
  Description:  i2c access func
  Input:        int file
                char read_write
                unsigned char command
                int size
                union i2c_smbus_data *data
  Output:       none
  Return:       SUCCESS 0;FAILURE -1
  Author:       huada.huang
*****************************************************************************/
static int i2c_rdwr_access(int file, unsigned char address, unsigned short read_write,
                                    int size, unsigned char *data)
{
    int ret;
    struct i2c_rdwr_ioctl_data args;

    if (size > 0)
    {
        struct i2c_msg msgs =
        {
             .addr = address,
             .flags = read_write, //read or write
             .len = size,
             .buf = data
        };

        args.nmsgs = 1;
        args.msgs = &msgs;

        ret = ioctl(file, I2C_RDWR,&args);
    }
    return ret;
}

/*****************************************************************************
  Function:     i2c_smbus_write_byte_data
  Description:  i2c write by one byte
  Input:        int file
                unsigned char command
                unsigned char value
  Output:       none
  Return:       SUCCESS 0;FAILURE -1
  Author:       guanghui.chen
*****************************************************************************/
static int i2c_smbus_write_byte_data(int file, unsigned char command, unsigned char value)
{
    union i2c_smbus_data data;

    data.byte = value;
    return i2c_smbus_access(file,I2C_SMBUS_WRITE,command,
                            I2C_SMBUS_BYTE_DATA, &data);
}


/*****************************************************************************
  Function:     i2c_smbus_read_byte_data
  Description:  i2c smbus read by one byte
  Input:        int file
                unsigned char command
                unsigned char *value
  Output:       none
  Return:       SUCCESS 0;FAILURE -1
  Author:       guanghui.chen
*****************************************************************************/
static int i2c_smbus_read_byte_data(int file, unsigned char command, unsigned char *value)
{
    int ret = I2C_FAILURE;
    union i2c_smbus_data data;

    ret = i2c_smbus_access(file,I2C_SMBUS_READ,command,I2C_SMBUS_BYTE_DATA,&data);
    if (I2C_SUCCESS == ret)
    {
        *value = data.byte;
        return I2C_SUCCESS;
    }
    else
    {
        return ret;
    }
}

/*****************************************************************************
  Function:     i2c_dev_open
  Description:  i2c dev open
  Input:        const char *dev_name       device name,such as "/dev/i2c-0"
                unsigned char device_addr  slave device address,such as 0x4B(ADT7410)
  Output:       none
  Return:       SUCCESS 0;FAILURE -1
  Author:       guanghui.chen
*****************************************************************************/
static int i2c_dev_open(const char *dev_name, int *pfd)
{
    int fd = -1;
    //int ret = I2C_FAILURE;

    fd = open(dev_name, O_RDWR);
    if(0 > fd)
    {
        printf("i2c_dev_open:open error.\n");
        return I2C_FAILURE;
    }
    *pfd = fd;
   // ret = i2c_set_slave_addr(fd,device_addr,0);
    return I2C_SUCCESS;

}


/*****************************************************************************
  Function:     i2c_set_slave_addr
  Description:  set device's slave addrs
  Input:        int file
                int address
                int force
  Output:       none
  Return:       SUCCESS 0;FAILURE -1
  Author:       guanghui.chen
*****************************************************************************/
static inline int i2c_set_slave_addr(int file, int address, int force)
{
    int ret = I2C_FAILURE;

    /* With force, let the user read from/write to the registers
       even when a driver is also running */
    ret = ioctl(file, force ? I2C_SLAVE_FORCE : I2C_SLAVE, address);
    if (I2C_SUCCESS > ret)
    {
        printf("Could not set address to 0x%02x,ret = %d.\n",address,ret);
        return I2C_FAILURE;
    }

    return I2C_SUCCESS;
}


/*****************************************************************************
  Function:     i2c_set_addr
  Description:  set device's slave addrs
  Input:        int index
                int address
                int force
  Output:       none
  Return:       SUCCESS 0;FAILURE -1
  Author:       huada.huang
*****************************************************************************/
int i2c_set_addr(int index, int address, int force)
{
    int nRet = 0;
    switch(index)
    {
        case I2C_0:
        {    
            i2c_Lock(&g_stI2C0Sem);
            nRet = i2c_set_slave_addr(g_I2C0Fd, address, 0);
            i2c_UnLock(&g_stI2C0Sem);

            break;
        }
        case I2C_1:
        {
            i2c_Lock(&g_stI2C1Sem);
            nRet = i2c_set_slave_addr(g_I2C1Fd, address, 0);
            i2c_UnLock(&g_stI2C1Sem);
            break;
        }
        default:
        {
            break;
        }
    }
    return nRet;
}
/*****************************************************************************
  Function:     i2c_receive_msg
  Description:  i2c receive msg
  Input:        int index
                unsigned char address
                unsigned char *value
                unsigned char len
  Output:       none
  Return:       SUCCESS 0;FAILURE -1
  Author:       huada.huang
*****************************************************************************/
int i2c_receive_msg(int index, unsigned char address, unsigned char *value, unsigned char len)
{

    int ret = I2C_FAILURE;
       
    switch(index)
    {
        case I2C_0:
        {    
            i2c_Lock(&g_stI2C0Sem);
            ret = i2c_rdwr_access(g_I2C0Fd, address, I2C_M_RD, len, value);
            i2c_UnLock(&g_stI2C0Sem);
 
            break;
        }
        case I2C_1:
        {
            i2c_Lock(&g_stI2C1Sem);
            ret = i2c_rdwr_access(g_I2C1Fd, address, I2C_M_RD, len, value);  
            i2c_UnLock(&g_stI2C1Sem);
            break;
        }
        default:
        {
            break;
        }
    }
    
    if(I2C_SUCCESS > ret)
    {
        printf("%s_read_msg: error,addr = 0x%x, ret = %x.\n",(index == 1)?"i2c1":"i2c0", address, ret);
        return I2C_FAILURE;
    }
    
    return I2C_SUCCESS;
}


/*****************************************************************************
  Function:     i2c_write_msg
  Description:  i2c write msg
  Input:        int index
                unsigned char address
                unsigned char *value
                unsigned char len
  Output:       none
  Return:       SUCCESS 0;FAILURE -1
  Author:       huada.huang
*****************************************************************************/
int i2c_write_msg(int index, unsigned char address, unsigned char *value, unsigned char len)
{
    int ret = I2C_FAILURE;
    
    switch(index)
    {
        case I2C_0:
        {    
            i2c_Lock(&g_stI2C0Sem);
            ret = i2c_rdwr_access(g_I2C0Fd, address, 0, len, value);
            i2c_UnLock(&g_stI2C0Sem);

            break;
        }
        case I2C_1:
        {
            i2c_Lock(&g_stI2C1Sem);
            ret = i2c_rdwr_access(g_I2C1Fd, address, 0, len, value);  
            i2c_UnLock(&g_stI2C1Sem);
            break;
        }
        default:
        {
            break;
        }
    }
    
    if(I2C_SUCCESS > ret)
    {
        printf("%s_write_msg: error, addr = 0x%x ,ret = %x.\n",(index == 1)?"i2c1":"i2c0", address, ret);
        return I2C_FAILURE;
    }
 
    return I2C_SUCCESS;
}


/*****************************************************************************
  Function:     i2c_dev_close
  Description:  close i2c device
  Input:        int file  the open device file descriptor
  Output:       none
  Return:       SUCCESS 0;FAILURE -1
  Author:       guanghui.chen
*****************************************************************************/
int i2c_dev_close(int file)
{
    int ret = I2C_FAILURE;

    ret = close(file);

    return ret;
}

/*****************************************************************************
  Function:     i2c_write
  Description:  i2c write by one byte
  Input:        int file              the open device file descriptor
                unsigned char offset  address of the register
                unsigned char value   the value you want to set
  Output:       none
  Return:       SUCCESS 0;FAILURE -1
  Author:       guanghui.chen
*****************************************************************************/
int i2c_write(int index, unsigned char offset,unsigned char value)
{
    int ret = I2C_FAILURE;

    switch(index)
    {
        case I2C_0:
        {
            i2c_Lock(&g_stI2C0Sem);
            ret = i2c_smbus_write_byte_data(g_I2C0Fd, offset, value);
            i2c_UnLock(&g_stI2C0Sem);
            break;
        }
        case I2C_1:
        {   
            i2c_Lock(&g_stI2C1Sem);
            ret = i2c_smbus_write_byte_data(g_I2C1Fd, offset, value);
            i2c_UnLock(&g_stI2C1Sem);
            break;
        }
        default:
        {
            break;
        }
    }

    if(I2C_SUCCESS > ret)
    {
        printf("%s_write:write offset %d error,ret = %x.\n",(index==1)?"i2c1":"i2c0",offset,ret);
        return I2C_FAILURE;
    }


    return I2C_SUCCESS;
}

/*****************************************************************************
  Function:     i2c_read
  Description:  i2c read by one byte
  Input:        int file              the open device file descriptor
                unsigned char offset  address of the register
                unsigned char *value  the buf you want to save data
  Output:       none
  Return:       SUCCESS 0;FAILURE -1
  Author:       guanghui.chen
*****************************************************************************/
int i2c_read(int index ,unsigned char offset,unsigned char *value)
{
    unsigned char data = 0;
    int ret = I2C_FAILURE;

    switch(index)
    {
        case I2C_0:
        {
            i2c_Lock(&g_stI2C0Sem);
            ret = i2c_smbus_read_byte_data(g_I2C0Fd, offset, &data);
            i2c_UnLock(&g_stI2C0Sem);
            break;
        }
        case I2C_1:
        {   
            i2c_Lock(&g_stI2C1Sem);
            ret = i2c_smbus_read_byte_data(g_I2C1Fd, offset, &data);
            i2c_UnLock(&g_stI2C1Sem);
            break;
        }
        default:
        {
            break;
        }
    }
    
    if(I2C_SUCCESS > ret)
    {
        printf("%s_read:read offset %d error,ret = %x \r\n",(index==1)?"i2c1":"i2c0",offset,ret);
        return I2C_FAILURE;
    }    
    *value = data;
    
    return I2C_SUCCESS;
}


/*****************************************************************************
  Function:     i2c_OpenBus
  Description:  OPEN I2C bus
  Input:        int index : i2c0 or i2c1
  Output:       none
  Return:       SUCCESS 0;FAILURE -1
  Author:       huada.huang
*****************************************************************************/
int i2c_OpenBus(int index)
{
    int fd;
    int nRet = 0;
    
    switch(index)
    {
        case I2C_0:
        {
            nRet = i2c_dev_open(I2C0_DEV_NAME, &fd);
            if(I2C_SUCCESS == nRet)
            {
                g_I2C0Fd = fd;
                i2c_LockInit(&g_stI2C0Sem);
                LOG_PRINTF(LOG_LEVEL_DEBUG,LOG_MODULE_DRIVERS, "Open I2C_0 SUCCESS.");
            }
            else
            {
                LOG_PRINTF(LOG_LEVEL_ERROR,LOG_MODULE_DRIVERS, "Error: Open I2C_0 FAILED!");
            }
            break;
        }
        case I2C_1:
        {
            nRet = i2c_dev_open(I2C1_DEV_NAME, &fd);
            if(I2C_SUCCESS == nRet)
            {
                g_I2C1Fd = fd;
                i2c_LockInit(&g_stI2C1Sem);
                LOG_PRINTF(LOG_LEVEL_DEBUG,LOG_MODULE_DRIVERS, "Open I2C_1 SUCCESS.");
            }
            else
            {
                LOG_PRINTF(LOG_LEVEL_ERROR,LOG_MODULE_DRIVERS, "Error: Open I2C_1 FAILED!");
            }
            break;
        }
        default:
        {
            LOG_PRINTF(LOG_LEVEL_ERROR,LOG_MODULE_DRIVERS, "Error: Unsupport I2C port!");
            break;   
        }
    }
    return nRet;
}

/*****************************************************************************
  Function:     i2c_CloseBus
  Description:  close I2C bus
  Input:        int index : i2c0 or i2c1
  Output:       none
  Return:       SUCCESS 0;FAILURE -1
  Author:       huada.huang
*****************************************************************************/
int i2c_CloseBus(int index)
{
  
    switch(index)
    {
        case I2C_0:
        {
            close(g_I2C0Fd);
            g_I2C1Fd = -1;
            break;
        }
        case I2C_1:
        {
            close(g_I2C1Fd);
            g_I2C1Fd = -1;
            break;
        }
        default:
        {
            break;   
        }
    }
    return I2C_SUCCESS;
}


