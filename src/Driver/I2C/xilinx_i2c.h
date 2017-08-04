/******************************************************************************

  Copyright (C), 2013, HuiZhou WELLAV Technology Co., Ltd.

 ******************************************************************************
  File Name     : xilinx_i2c.h
  Version       : Initial Draft
  Author        : guanghui.chen
  Created       : 2013/12/12
  Last Modified :
  Description   : xlinx_i2c.c header file
  Function List :
  History       :
  1.Date        : 2013/12/12
    Author      : guanghui.chen
    Modification: Created file

******************************************************************************/

#ifndef __XLINX_I2C_H__
#define __XLINX_I2C_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#define I2C_SUCCESS           (0)
#define I2C_FAILURE           (-1)
/* smbus_access read or write markers */
#define I2C_SMBUS_READ  1
#define I2C_SMBUS_WRITE 0

/* SMBus transaction types (size parameter in the above functions) 
   Note: these no longer correspond to the (arbitrary) PIIX4 internal codes! */
#define I2C_SMBUS_QUICK         0
#define I2C_SMBUS_BYTE          1
#define I2C_SMBUS_BYTE_DATA     2 
#define I2C_SMBUS_WORD_DATA     3
#define I2C_SMBUS_PROC_CALL     4
#define I2C_SMBUS_BLOCK_DATA        5
#define I2C_SMBUS_I2C_BLOCK_BROKEN  6
#define I2C_SMBUS_BLOCK_PROC_CALL   7       /* SMBus 2.0 */
#define I2C_SMBUS_I2C_BLOCK_DATA    8

#define I2C0_DEV_NAME "/dev/i2c-0"
#define I2C1_DEV_NAME "/dev/i2c-1"

typedef enum
{
    I2C_0,
    I2C_1,

    I2C_NUM
}I2C_PortIndex_t;


extern int i2c_set_addr(int index, int address, int force);
extern int i2c_receive_msg(int file, unsigned char address, unsigned char *value, unsigned char len);
extern int i2c_write_msg(int file, unsigned char address, unsigned char *value, unsigned char len);
//extern int i2c_dev_close(int file);
extern int i2c_OpenBus(int index);
extern int i2c_CloseBus(int index);
//extern int i2c_dev_open(const char *dev_name,unsigned char device_addr);
extern int i2c_read(int file,unsigned char offset,unsigned char *value);
extern int i2c_write(int file,unsigned char offset,unsigned char value);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __XLINX_I2C_H__ */
