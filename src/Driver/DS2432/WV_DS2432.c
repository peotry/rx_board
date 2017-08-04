/******************************************************************************
 * WV_DS2432.c
 *
 * Copyright (c) 2010,WellAV Technologies Ltd. All rights reserved. .
 *
 * DESCRIPTION: -
 *      Contains drivers for a DS2432 1k-bit Protected 1-Wire EEPROM with SHA-1 Engine
 * ----------------------------------
 * 22 Oct 2010, Chengbing.Shi written
 * ----------------------------------
 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include "DS2432.h"
#include "WV_DS2432.h"
#include "FPGA.h"

static DS2432_DataStatus_t         s_tChip;                       /* Chip data status */
static BusMaster_DataStatus_t      s_tBusMaster;                  /* Bus Master data status */

extern U8      g_DS2432_Index;


/*************************************************
  Function:       NLF - INTERNAL FUNTION
  Description:    To perform a minor calculation of
                  the SHA-1 algorithm
  Input:          Three U32 which are numbers for
                  the calculation, and an U8 which
                  is used to tell which calculation
                  to perform
  Output:         NONE
  Return:         An U32 which is the result of
                  the performed calculation
  Others:
  Create:         2010/10/25
*************************************************/
static U32 NLF(U32 b, U32 c, U32 d, U8 num)
{
    U32 u32Tmp = 0;

    if(num < 20)
        u32Tmp = ((b & c) | ((~b) & d));
    else if(num < 40)
        u32Tmp = (b ^ c ^ d);
    else if(num < 60)
        u32Tmp = ((b & c) | (b & d) | (c & d));
    else
        u32Tmp = (b ^ c ^ d);

    return u32Tmp;
}

/*************************************************
  Function:       KTN - INTERNAL FUNTION
  Description:    To return a contant based on the
                  input of the function, used for
                  the SHA-1 algorithm
  Input:          An U8 which is used to decide what
                  number to return
  Output:         NONE
  Return:         An U32 which is one of four constants
  Others:
  Create:         2010/10/25
*************************************************/
static U32 KTN(U8 u8Num)
{
    U32 u32Tmp = 0;

    if(u8Num < 20)
        u32Tmp = 0x5A827999;
    else if(u8Num < 40)
        u32Tmp = 0x6ED9EBA1;
    else if(u8Num < 60)
        u32Tmp = 0x8F1BBCDC;
    else
        u32Tmp = 0xCA62C1D6;

    return u32Tmp;
}

/*************************************************
  Function:       make8
  Description:    make 8 bit data
  Input:          32 bit hash data and a bit index
  Output:         NONE
  Return:         8 bit use bit have been managed
  Others:
  Create:         2010/10/25
*************************************************/
static U8 make8(U32 u32HashData, U8 u8Index)
{
    U8  u8UseBit = 0;
    U32 u32Temp = 0;

    u32HashData = (u32HashData >> (u8Index * 8));
    u32Temp = u32HashData & 0x000000FF;
    u8UseBit = u32Temp;

    return u8UseBit;
}

/*************************************************
  Function:       make16
  Description:    make 16 bit data
  Input:          half of the 16 bit data, another half
                  data
  Output:         NONE
  Return:         16 bit data
  Others:
  Create:         2010/10/25
*************************************************/
static U16 make16(U8 u8Data1, U16 u8Data2)
{
    U16 u16Crc = 0;
    U16 u8Temp = 0;

    u8Temp = u8Data1;
    u8Temp = u8Temp << 8;
    u16Crc = (u8Temp | u8Data2);

    return u16Crc;
}

/*************************************************
  Function:       make32
  Description:    make 32 bit data
  Input:          four part of 8 bit data of 32 bit data
  Output:         NONE
  Return:         32 bit full data
  Others:
  Create:         2010/10/25
*************************************************/
static U32 make32(U8 u8Data1, U8 u8Data2, U8 u8Data3, U8 u8Data4)
{
    U32 u32Crc = 0;
    U32 u32Temp = 0;

    u32Temp = u8Data1;
    u32Crc |= u32Temp;
    u32Crc = (u32Crc << 8);

    u32Temp = u8Data2;
    u32Crc |= u32Temp;
    u32Crc = (u32Crc << 8);

    u32Temp = u8Data3;
    u32Crc |= u32Temp;
    u32Crc = (u32Crc << 8);

    u32Temp = u8Data4;
    u32Crc |= u32Temp;

    return u32Crc;
}

/*************************************************
  Function:       verify_CRC
  Description:    To check a CRC against a test CRC
  Input:          One int that is a CRC sent from
                  the DS2432 chip and one int that
                  is calculated by the MCU
  Output:         NONE
  Return:         An bool that signifies if the crc
                  and test matched (1, true) or d
                  idn't match (0, false)
  Others:
  Create:         2010/10/25
*************************************************/
static bool verify_CRC(U8 u8Crc, U8 u8Test)
{
    bool bState = false;

    if(u8Crc == u8Test)
        bState =  true;
    else
        bState =  false;

    return bState;
}

/*************************************************
  Function:       verify_CRC16
  Description:    To check a CRC against a test CRC
  Input:          One int that is a CRC sent from
                  the DS2432 chip and one int that
                  is calculated by the MCU
  Output:         NONE
  Return:         An int1 that signifies if the crc
                  and complement of the test matched
                  (1, true) or didn't match (0, false)
  Others:
  Create:         2010/10/25
*************************************************/
static bool verify_CRC16(U16 u8Crc, U16 u16Test)
{
    bool bState = false;

    u16Test = (~u16Test);

    if(u8Crc == u16Test)
        bState = true;
    else
        bState = false;

    return bState;
}

/*************************************************
  Function:       calc_CRC
  Description:    To calculate an 8-bit CRC based
                  on a polynomial and the series
                  of data bytes
  Input:          A pointer to an array of the data
                  bytes and an int saying how many
                  bytes there are in the data array
  Output:         NONE
  Return:         An 8 bit data which is the calculated CRC
  Others:         Polynomial used x^8 + x^5 + x^4 + 1 = 10001100
  Create:         2010/10/25
*************************************************/
static U8 calc_CRC(U8 *pu8Data, U8 u8Bytes)
{
    U32    i = 0;
    U8     u8ShiftRegister = 0;
    U8     u8Bits = 0;
    U8     u8DataB;

    for(i = 0; i < u8Bytes; i++)
    {
        u8DataB = *(pu8Data + i);

        for(u8Bits = 0; u8Bits < 8; ++u8Bits)
        {
            if((u8ShiftRegister ^ u8DataB) & 0x01)
            {
                u8ShiftRegister = u8ShiftRegister >> 1;
                u8ShiftRegister ^= CRC_POLY;
            }
            else
            {
                u8ShiftRegister = u8ShiftRegister >> 1;
            }

            u8DataB = u8DataB >> 1;
        }
    }

    return u8ShiftRegister;
}

/*************************************************
  Function:       calc_CRC16
  Description:    To calculate a 16-bit CRC based
                  on a polynomial and the series
                  of data bytes
  Input:          A pointer to an array of the data
                  bytes and an int saying how many
                  bytes there are in the data array
  Output:         NONE
  Return:         An 16 bit data which is the calculated CRC
  Others:         Polyinomial used x^16 + x^15 + x^2 + 1 = 1010000000000001
  Create:         2010/10/25
*************************************************/
static U16 calc_CRC16(U8 *pu8Data, U8 u8Bytes)
{
    U32    i = 0;
    U16    u8ShiftRegister = 0;
    U8     u8Bits = 0;
    U8     u8DataB = 0;

    for(i = 0; i < u8Bytes; ++ i)
    {
        u8DataB = *(pu8Data + i);

        for(u8Bits = 0; u8Bits < 8; ++u8Bits)
        {
            if((u8ShiftRegister ^ (U16)u8DataB) & 0x0001)
            {
                u8ShiftRegister = u8ShiftRegister >> 1;
                u8ShiftRegister ^= CRC_16_POLY;
            }

            else
            {
                u8ShiftRegister = u8ShiftRegister >> 1;
            }

            u8DataB = u8DataB >> 1;
        }
    }

    return u8ShiftRegister;
}

/*************************************************
  Function:       gen_MAC
  Description:    To use SHA-1 calculation to generate
                  a message authentication code (MAC)
  Input:          A pointer to an array containing
                  the 128-byte input data, and a pointer
                  to an array in which the 20 byte
                  MAC will be placed
  Output:         NONE
  Return:         NONE
  Others:
  Create:         2010/10/25
*************************************************/
static void gen_MAC(U8 *pu8Data, U8 *paru8MacArray)
{
    U32    aru32Hash[5] = {0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476, 0xC3D2E1F0};
    U32    i = 0;

    U32    aru32MTword[80];
    U32    u32ShftTmp = 0;
    U32    u32Temp = 0;

    memset(aru32MTword,0x00,sizeof(aru32MTword));

    for(i = 0; i < 16; i++)
    {
        aru32MTword[i] = (make32(*(pu8Data + (i * 4)),
            *(pu8Data + (i * 4) + 1),
            *(pu8Data + (i * 4) + 2),
            *(pu8Data + (i * 4) + 3)));
    }

    for(;i<80;i++)
    {
        u32ShftTmp = (aru32MTword[i-3] ^ aru32MTword[i-8] ^ aru32MTword[i-14] ^ aru32MTword[i-16]);
        aru32MTword[i] = (((u32ShftTmp << 1) & 0xFFFFFFFE) | ((u32ShftTmp >> 31) & 0x00000001));
    }

    for(i=0;i<80;i++)
    {
        u32ShftTmp = (((aru32Hash[0] << 5) & 0xFFFFFFE0) | ((aru32Hash[0] >> 27) & 0x0000001F));
        u32Temp = (NLF(aru32Hash[1],aru32Hash[2],aru32Hash[3],i) + aru32Hash[4] + KTN(i) + aru32MTword[i] + u32ShftTmp);
        aru32Hash[4] = aru32Hash[3];
        aru32Hash[3] = aru32Hash[2];
        aru32Hash[2] = (((aru32Hash[1] << 30) & 0xC0000000) | ((aru32Hash[1] >> 2) & 0x3FFFFFFF));
        aru32Hash[1] = aru32Hash[0];
        aru32Hash[0] = u32Temp;
    }

    for(i = 5; i > 0; i--)
    {
        *paru8MacArray = make8(aru32Hash[i - 1], 0);
        paru8MacArray++;
        *paru8MacArray = make8(aru32Hash[i - 1], 1);
        paru8MacArray++;
        *paru8MacArray = make8(aru32Hash[i - 1], 2);
        paru8MacArray++;
        *paru8MacArray = make8(aru32Hash[i - 1], 3);
        paru8MacArray++;
    }
}

/*************************************************
  Function:       Touch_Read_Byte
  Description:    To read a byte of data from a
                  touch device
  Input:          NONE
  Output:         NONE
  Return:         U8 data
  Others:
  Create:         2010/10/25
*************************************************/
static U8 Touch_Read_Byte()
{
    U32     u32tempValue = 0;
    U32     u32ReadCommand      = 0x40000000;
    U32     u32ReleaseCommand   = 0x00000000;

    switch (g_DS2432_Index)
    {
        case SLOT_MAINBOARD:
        {
            u32ReadCommand      = 0x40000001;
            u32ReleaseCommand   = 0x00000001;
            break;
        }
        case SLOT0:
        {
            u32ReadCommand      = 0x40000002;
            u32ReleaseCommand   = 0x00000002;
            break;
        }
        case SLOT1:
        {
            u32ReadCommand      = 0x40000004;
            u32ReleaseCommand   = 0x00000004;
            break;
        }
        case SLOT2:
        {
            u32ReadCommand      = 0x40000008;
            u32ReleaseCommand   = 0x00000008;
            break;
        }
        default:
        {
            printf ("[Touch_Read_Byte] Bad params \r\n");
            return false; // params error
            //break;
        }
    }

    (void)FPGA_REG_Write(DS2432_WR_ADDR, u32ReadCommand);         // read cmm
    // read busy bit until zero
    do
    {
        (void)FPGA_REG_Read(DS2432_RW_BUSY, &u32tempValue);
    } while (u32tempValue != 0);
    (void)FPGA_REG_Read(DS2432_RD_DATA, &u32tempValue);              // read cmm
    (void)FPGA_REG_Write(DS2432_WR_ADDR, u32ReleaseCommand);      // release

    //printf ("RX: %02X \r\n", tempValue);
    return u32tempValue & 0xFF;
}

/*************************************************
  Function:       Touch_Write_Byte
  Description:    To write a byte of data to a touch
                  device
  Input:          A byte representing the data to
                  be written
  Output:         NONE
  Return:         bool true         
                  bool false         
  Others:
  Create:         2010/10/25
*************************************************/
static bool Touch_Write_Byte(U8 u8Command)
{
    U32     u32TempValue = 0;
    U32     u32WriteCommand     = 0x80000000;
    U32     u32ReleaseCommand   = 0x00000000;

    switch (g_DS2432_Index)
    {
        case SLOT_MAINBOARD:
        {
            u32WriteCommand     = 0x80000001;
            u32ReleaseCommand   = 0x00000001;
            break;
        }
        case SLOT0:
        {
            u32WriteCommand     = 0x80000002;
            u32ReleaseCommand   = 0x00000002;
            break;
        }
        case SLOT1:
        {
            u32WriteCommand     = 0x80000004;
            u32ReleaseCommand   = 0x00000004;
            break;
        }
        case SLOT2:
        {
            u32WriteCommand     = 0x80000008;
            u32ReleaseCommand   = 0x00000008;
            break;
        }
        default:
        {
            printf ("[Touch_Write_Byte] Bad params \r\n");
            return false; // params error
            //break;
        }
    }

    (void)FPGA_REG_Write(DS2432_WR_DATA, u8Command);          // write data
    (void)FPGA_REG_Write(DS2432_WR_ADDR, u32WriteCommand);    // write address cmm
    // read busy bit until zero
    do
    {
        (void)FPGA_REG_Read(DS2432_RW_BUSY, &u32TempValue);
    } while (u32TempValue != 0);
    (void)FPGA_REG_Write(DS2432_WR_ADDR, u32ReleaseCommand);  // write address cmm

     //printf ("TX: %02X \r\n", ucCommand);
     return(true);
}

/*************************************************
  Function:       Touch_Present
  Description:    To detect if a touch device in present
  Input:          NONE
  Output:         NONE
  Return:         true               
                  false               
  Others:
  Create:         2010/10/25
*************************************************/
static bool Touch_Present(void)
{
    U32     u32TempValue = 0;
    S32     s32MaxTryTimes = MAX_TRY_TIMES;

    U32     u32ResetCommand     = 0x20000000;
    U32     u32ReleaseCommand   = 0x00000000;

    switch (g_DS2432_Index)
    {
        case SLOT_MAINBOARD:
        {
            u32ResetCommand     = 0x20000001;
            u32ReleaseCommand   = 0x00000001;
            break;
        }
        case SLOT0:
        {
            u32ResetCommand     = 0x20000002;
            u32ReleaseCommand   = 0x00000002;
            break;
        }
        case SLOT1:
        {
            u32ResetCommand     = 0x20000004;
            u32ReleaseCommand   = 0x00000004;
            break;
        }
        case SLOT2:
        {
            u32ResetCommand     = 0x20000008;
            u32ReleaseCommand   = 0x00000008;
            break;
        }
        default:
        {
            printf ("[Touch_Present] Bad params \r\n");
            return false; // params error
            //break;
        }
    }

    TTTT:
    // write address
    (void)FPGA_REG_Write(DS2432_WR_ADDR, u32ResetCommand);
    // read busy bit until zero
    do
    {
        (void)FPGA_REG_Read(DS2432_RW_BUSY, &u32TempValue);
    } while (u32TempValue != 0);

    // get data
    (void)FPGA_REG_Read(DS2432_RD_DATA, &u32TempValue);
    (void)FPGA_REG_Write(DS2432_WR_ADDR, u32ReleaseCommand);
    if ((u32TempValue & 0x01) == 0x00)
    {
        return true;    // success
    }
    else
    {
        s32MaxTryTimes--;
        if (s32MaxTryTimes < 0)
        {
            printf ("[Touch_Present] Error timeout \r\n");
            return false;
        }
        // wait for 1 sec
        printf ("^");
        goto TTTT;
    }
}

 /*************************************************
  Function:       Match_Rom
  Description:    To activate a single DS2432 on a bus
  Input:          NONE
  Output:         NONE
  Return:         emErrType
  Others:
  Create:         2010/10/25
*************************************************/
static enmWVDS2432ErrorType Match_Rom(void)
{
    enmWVDS2432ErrorType    emErrType = ERR_OK;
    U32   i = 0;

    if(Touch_Present())
    {
        if(!emErrType)
        {
            if(!Touch_Write_Byte(MATCH_ROM_CMD))
            {
                emErrType = ERR_COLLISION;
            }
        }

        if(!emErrType)
        {
            for(i = 0; i < 8; ++i)
            {
                if(!Touch_Write_Byte(s_tChip.m_ucarChipID[i]))
                {
                    emErrType = ERR_COLLISION;
                }
            }
        }
    }
    else
        emErrType = ERR_NODEVICE;

    return emErrType;
}

/*************************************************
  Function:       Resume
  Description:    Resume giving memory commands to
                  DS2432 that last successfully used
                  match_rom with
  Input:          NONE
  Output:         NONE
  Return:         emErrType
  Others:
  Create:         2010/10/25
*************************************************/
static enmWVDS2432ErrorType Resume(void)
{
    enmWVDS2432ErrorType    emErrType = ERR_OK;

    if(Touch_Present())
    {
        if(!Touch_Write_Byte(RESUME_CMD))
        {
            emErrType = ERR_COLLISION;
        }
    }

    else
        emErrType = ERR_NODEVICE;

    return emErrType;
}

/*************************************************
  Function:       Read_SP
  Description:    To read the data in the TA1, TA2,
                  ES, and scratchpad
  Input:          A pointer to an array of bytes to
                  hold the 11 bytes of data received
                  from the chip
  Output:         a series of 8 bytes scratch pad data
  Return:         emErrType
  Others:
  Create:         2010/10/25
*************************************************/
static enmWVDS2432ErrorType Read_SP(U8 *pu8ScratchpadData)
{
    enmWVDS2432ErrorType    emErrType = ERR_OK;
    U32   i = 0;
    U8    u8Temp = 0;
    U16   u16Crc = 0;
    U8    aru8Scratch[12];

    memset(aru8Scratch,0x00,sizeof(aru8Scratch));

    aru8Scratch[0] = READ_SP_CMD;

    emErrType = Resume();
    if(!emErrType)
    {
        if(!Touch_Write_Byte(READ_SP_CMD))
        {
            emErrType = ERR_COLLISION;
        }
    }

    if(!emErrType)
    {
        for(i = 1; i < 12; i++)
            aru8Scratch[i] = Touch_Read_Byte();

        u16Crc = Touch_Read_Byte();
        u8Temp = Touch_Read_Byte();

        u16Crc = make16(u8Temp, u16Crc);

        if(!verify_CRC16(u16Crc, calc_CRC16(aru8Scratch, 12)))
            emErrType = ERR_COMM;
        else
            for(i = 0; i < 11; i++)
                *(pu8ScratchpadData + i) = aru8Scratch[i + 1];
    }

    (void)Touch_Present();

    return emErrType;
}

/*************************************************
  Function:       Write_SP
  Description:    Writes data to the scratch pad
  Input:          An int for the address of where
                  the data is intended to go, a
                  pointer to an array of ints which
                  contains the data to be written
                  (which may be no larger than 8
                  bytes), and an int that represents
                  how many bytes will be written
  Output:         NONE
  Return:         emErrType
  Others:
  Create:         2010/10/25
*************************************************/
static enmWVDS2432ErrorType Write_SP(U8 ucSPAddress, U8* pucWriteData, S32 s32Length)
{
    enmWVDS2432ErrorType    emErrType = ERR_OK;
    S32    i = 0;
    U8     u8Temp = ucSPAddress & 0xF8;
    U16    u16Crc = 0;
    U8     aru8Scratch[11] = {0};

    emErrType = Resume();

    if(!emErrType)
        emErrType = WVDS2432ReadMemory(u8Temp, (aru8Scratch + 3), 8);

    if(!emErrType)
    {
        for(i = 0; i < s32Length; i++)
            aru8Scratch[((ucSPAddress - u8Temp + i) + 3)] = *(pucWriteData + i);

        aru8Scratch[0] = WRITE_SP_CMD;
        aru8Scratch[1] = ucSPAddress;
        aru8Scratch[2] = ADDRESS_MSB;

        emErrType = Resume();

        if(!emErrType)
        {
            //Send write scratchpad command
            if(!Touch_Write_Byte(WRITE_SP_CMD))
                emErrType = ERR_COLLISION;

            //Send lower byte of the address
            if(!emErrType)
                if(!Touch_Write_Byte(ucSPAddress))
                    emErrType = ERR_COLLISION;

            //Send upper byte of the address
            if(!emErrType)
                if(!Touch_Write_Byte(ADDRESS_MSB))
                    emErrType = ERR_COLLISION;

            if(!emErrType)
            {
                for(i = 3; i < 11; i++)
                {
                    if(!Touch_Write_Byte(aru8Scratch[i]) && !emErrType)
                    {
                        emErrType = ERR_COLLISION;
                    }
                }
            }

            if(!emErrType)
            {
                u16Crc = Touch_Read_Byte();
                u8Temp = Touch_Read_Byte();

                u16Crc = make16(u8Temp, u16Crc);

                if(!verify_CRC16(u16Crc, calc_CRC16(aru8Scratch, 11)))
                    emErrType = ERR_COMM;
            }
        }
    }

    (void)Touch_Present();

    return emErrType;
}

/*************************************************
  Function:       Write_Byte
  Description:    To write a byte to the DS2432
  Input:          A byte containing the address to
                  be written to, and an int containing
                  the byte of data to be written
  Output:         NONE
  Return:         emErrType
  Others:
  Create:         2010/10/25
*************************************************/
static enmWVDS2432ErrorType Write_Byte(U8 u8SpecialAddress, U8 u8Data)
{
    enmWVDS2432ErrorType    emErrType = ERR_OK;
    U8   aru8arSHAInput[64];
    U8   aru8MacArray[20];
    U8   aru8Scratch[12];
    int  i = 0;
    U8   u8Page= u8SpecialAddress & 0xE0;
    U8   u8Temp = 0;

    //init
    memset(aru8arSHAInput,0x00,sizeof(aru8arSHAInput));
    memset(aru8MacArray,0x00,sizeof(aru8MacArray));
    memset(aru8Scratch,0x00,sizeof(aru8Scratch));

    //don't check famliy code
    emErrType = Match_Rom();
    if(!emErrType)
    {
        //Build array needed for MAC calculation
        //See DS2432 datasheet for details
        //M0
        for(i = 0; i < 4; ++i)
            aru8arSHAInput[i] = s_tBusMaster.m_ucarSecret[i];

        //M1 - M7
        for(i = 0; i < 8; i++)
            aru8arSHAInput[i + 4] = s_tBusMaster.m_ucarSecret[i];

        if(!emErrType)
            emErrType = Resume();

        if(!emErrType)
            emErrType = WVDS2432ReadMemory((u8Page + 8), (aru8arSHAInput + 12), 20);

        aru8arSHAInput[28] = 0xFF;
        aru8arSHAInput[29] = 0xFF;
        aru8arSHAInput[30] = 0xFF;
        aru8arSHAInput[31] = 0xFF;

        if(!emErrType)
            emErrType = Resume();

        //Write data to the scratch pad
        if(!emErrType)
            emErrType = Write_SP(u8SpecialAddress, &u8Data, 1);

        if(!emErrType)
            emErrType = Resume();

        //M8 - M9
        if(!emErrType)
        {
            emErrType = Read_SP(aru8Scratch);

            if(!emErrType)
            {
                for(i = 3; i < 11; i++)
                    aru8arSHAInput[i + 29] = aru8Scratch[i];

                //sha_input[address - page] = data;

                //M10[31:24]
                aru8arSHAInput[40] = 0x00 | (u8SpecialAddress >> 5);

                //M10[23:0] - M11
                for(i = 0; i < 7; i++)
                    aru8arSHAInput[i + 41] = s_tChip.m_ucarChipID[i];

                //M12
                for(i = 4; i < 8; i++)
                    aru8arSHAInput[i + 44] = s_tBusMaster.m_ucarSecret[i];

                //M13-M15
                aru8arSHAInput[52] = 0xFF;
                aru8arSHAInput[53] = 0xFF;
                aru8arSHAInput[54] = 0xFF;
                aru8arSHAInput[55] = 0x80;
                aru8arSHAInput[56] = 0x00;
                aru8arSHAInput[57] = 0x00;
                aru8arSHAInput[58] = 0x00;
                aru8arSHAInput[59] = 0x00;
                aru8arSHAInput[60] = 0x00;
                aru8arSHAInput[61] = 0x00;
                aru8arSHAInput[62] = 0x01;
                aru8arSHAInput[63] = 0xB8;

                //Generate the MAC using SHA-1 algorithm
                gen_MAC(aru8arSHAInput, aru8MacArray);

                emErrType = Resume();

                if(!emErrType)
                {
                    //Copy the scratchpad to memory
                    if(!Touch_Write_Byte(COPY_SP_CMD))
                        emErrType = ERR_COLLISION;

                    if(!emErrType)
                    {
                        for(i = 0; i < 3; i++)
                        {
                            if(!Touch_Write_Byte(aru8Scratch[i]))
                            {
                                emErrType = ERR_COLLISION;
                            }
                        }
                    }

                    usleep(2000);

                    if(!emErrType)
                    {
                        for(i = 0; i < 20; i++)
                        {
                            if(!Touch_Write_Byte(aru8MacArray[i]))
                            {
                                emErrType = ERR_COLLISION;
                            }
                        }
                    }

                    (void)Delay_ms(10);

                    u8Temp = Touch_Read_Byte();
                    //If all 0's are read then the MAC address did not match
                    //the one the chip calculated
                    if(!emErrType)
                    {
                        if(u8Temp == 0)
                            emErrType = ERR_NO_WRITE;
                        else if(u8Temp == 0xFF)
                            emErrType = ERR_BAD_AUTH;
                    }
                }
            }
        }
    }

    (void)Touch_Present();

    return emErrType;
}

/*************************************************
  Function:       Get_Auth
  Description:    To get the TA1, TA2, and E/S bytes
                  for authentication
  Input:          A pointer to an array where the
                  authentication bytes will be stored
  Output:         3 bytes of data contains th TA1, TA2,
                  and E/S bytes for authentication
  Return:         emErrType
  Others:
  Create:         2010/10/25
*************************************************/
static enmWVDS2432ErrorType Get_Auth(U8 *pu8Bytes)
{
    enmWVDS2432ErrorType    emErrType = ERR_OK;
    U8 aru8Scratch[11];

    memset(aru8Scratch,0x00,sizeof(aru8Scratch));

    emErrType = Read_SP(aru8Scratch);
    /* TA1, TA2, E/S */
    if(!emErrType)
    {
        *pu8Bytes = aru8Scratch[0];
        *(pu8Bytes + 1) = aru8Scratch[1];
        *(pu8Bytes + 2) = aru8Scratch[2];
    }

    return emErrType;
}

/*************************************************
  Function:       Copy_Scratchpad
  Description:    Copy scratch pad data to eeprom register
  Input:          A byte containing the address to
                  be written to, and a pointer to an
                  array containing 8 byte of data to be written
  Output:         NONE
  Return:         emErrType
  Others:
  Create:         2010/10/25
*************************************************/
static enmWVDS2432ErrorType Copy_Scratchpad(U8 u8EepromAddress, U8 *pu8Data)
{
    enmWVDS2432ErrorType    emErrType = ERR_OK;
    U8   aru8SHAInput[64];
    U8   aru8MacArray[20];
    U8   aru8Scratch[12];
    U8   aru8AuthBytes[3];
    U32  i = 0;
    U8   u8Page = u8EepromAddress & 0xE0;
    U8   u8Temp = 0;

    //init
    memset(aru8SHAInput,0x00,sizeof(aru8SHAInput));
    memset(aru8MacArray,0x00,sizeof(aru8MacArray));
    memset(aru8Scratch,0x00,sizeof(aru8Scratch));
    memset(aru8AuthBytes,0x00,sizeof(aru8AuthBytes));

    if(u8EepromAddress >= 0x80)
        emErrType = ERR_RANGE;

    if(!emErrType)
    {
        //don't check famliy code
        emErrType = Match_Rom();
        if(!emErrType)
        {
            //Build array needed for MAC calculation
            //See DS2432 datasheet for details
            //M0
            for(i = 0; i < 4; i++)
                aru8SHAInput[i] = s_tBusMaster.m_ucarSecret[i];

            //M1 - M7
            if(!emErrType)
                emErrType = Resume();

            if(!emErrType)
                emErrType = WVDS2432ReadMemory(u8Page, (aru8SHAInput + 4), 28);

            if(!emErrType)
                emErrType = Resume();

            //Write data to the scratch pad
            if(!emErrType)
                emErrType = Write_SP(u8EepromAddress, pu8Data, 8);

            if(!emErrType)
                emErrType = Resume();

            //M8 - M9
            if(!emErrType)
            {
                emErrType = Read_SP(aru8Scratch);

                if(!emErrType)
                {
                    for(i = 3; i < 11; i++)
                    {
                        aru8SHAInput[i + 29] = aru8Scratch[i];
                    }

                    //sha_input[address - page] = data;

                    //M10[31:24]
                    aru8SHAInput[40] = 0x00 | (u8EepromAddress >> 5);

                    //M10[23:0] - M11
                    for(i = 0; i < 7; i++)
                    {
                        aru8SHAInput[i + 41] = s_tChip.m_ucarChipID[i];
                    }

                    //M12
                    for(i = 4; i < 8; i++)
                        aru8SHAInput[i + 44] = s_tBusMaster.m_ucarSecret[i];

                    //M13-M15
                    aru8SHAInput[52] = 0xFF;
                    aru8SHAInput[53] = 0xFF;
                    aru8SHAInput[54] = 0xFF;
                    aru8SHAInput[55] = 0x80;
                    aru8SHAInput[56] = 0x00;
                    aru8SHAInput[57] = 0x00;
                    aru8SHAInput[58] = 0x00;
                    aru8SHAInput[59] = 0x00;
                    aru8SHAInput[60] = 0x00;
                    aru8SHAInput[61] = 0x00;
                    aru8SHAInput[62] = 0x01;
                    aru8SHAInput[63] = 0xB8;

                    //Generate the MAC using SHA-1 algorithm
                    gen_MAC(aru8SHAInput, aru8MacArray);

                    emErrType = Resume();

                    (void)Get_Auth(aru8AuthBytes);

                    emErrType = Resume();

                    if(!emErrType)
                    {
                        //Copy the scratchpad to memory
                        if(!Touch_Write_Byte(COPY_SP_CMD))
                            emErrType = ERR_COLLISION;

                        if(!emErrType)
                        {
                            for(i = 0; i < 3; i++)
                            {
                                if(!Touch_Write_Byte(aru8AuthBytes[i]))
                                {
                                    emErrType = ERR_COLLISION;
                                }
                            }

                        }

                        (void)Delay_ms(2);

                        if(!emErrType)
                        {
                            for(i = 0; i < 20; i++)
                            {
                                if(!Touch_Write_Byte(aru8MacArray[i]))
                                {
                                    emErrType = ERR_COLLISION;
                                }
                            }
                        }

                        (void)Delay_ms(10);

                        u8Temp = Touch_Read_Byte();

                        //If all 0's are read then the MAC address did not match
                        //the one the chip calculated
                        if(!emErrType)
                        {
                            if(u8Temp == 0)
                                emErrType = ERR_NO_WRITE;

                            else if(u8Temp == 0xFF)
                                emErrType = ERR_BAD_AUTH;
                        }
                    }
                }
            }
        }
    }

    (void)Touch_Present();

    return emErrType;
}

/*************************************************
  Function:       WVDS2432Initialize
  Description:    Reset the ds2432 chip(s) by depriving
                  them of power
                  Initialize several global parameters
  Input:          A page number which the bus master
                  assigned,
                  A pointer to an array contains 8 bytes
                  secret from bus master,
                  The length of secret array(8 bytes),
                  The pio pin number.
  Output:         NONE
  Return:         emErrType
  Others:
  Create:         2010/10/25
*************************************************/
enmWVDS2432ErrorType WVDS2432Initialize(S32 s32Page, U8 *pu8Secret, U32 u32Len)
{
    enmWVDS2432ErrorType    emErrType = ERR_OK;

    /* check param */
    if(pu8Secret == NULL ||
       s32Page < PAGE_MIN || s32Page > PAGE_MAX ||
       u32Len > sizeof(s_tBusMaster.m_ucarSecret))
    {
        WVDS2432_ErrorPrint(("[WVDS2432Initialize]Check Param Failed!\r\n"));
        return ERR_RANGE;
    }

    /* check and reset the device */
    //printf ("##debug## Touch_Present(): %d \r\n", Touch_Present());
    if(Touch_Present() == false)
    {
        WVDS2432_ErrorPrint(("[WVDS2432Initialize]Touch_Present() Failed!\r\n"));
        return ERR_NODEVICE;
    }

    s_tBusMaster.m_nEepromPage = s32Page;                           /* initialize eeprom page */
    s_tBusMaster.m_ucPageAddress = s32Page * 0x20;                  /* initialize eeprom page address */
    memcpy(s_tBusMaster.m_ucarSecret, pu8Secret,                /* initialize secret */
           sizeof(s_tBusMaster.m_ucarSecret));

    WVDS2432_DebugPrint(("[WVDS2432Initialize]Wellav DS2432 Initialize Performed!\r\n"));
    return emErrType;
}

/*************************************************
  Function:       WVDS2432ReadRom
  Description:    To read the laser ROM contained the
                  family code and chip serial number
  Input:          A pointer to an array of U8 8 (must
                  contain at least 7 elements)
                  The length of the chip id and crc code
  Output:         A series bytes of Family ID Byte,
                  Serial Byte 0, Serial Byte 1, Serial
                  Byte 2, Serial Byte 3, Serial Byte 4,
                  Serial Byte 5, CRC Byte
  Return:         emErrType
  Others:
  Create:         2010/10/25
*************************************************/
enmWVDS2432ErrorType WVDS2432ReadRom(U8 *pu8IDBuffer, U32 u32Len)
{
    enmWVDS2432ErrorType    emErrType = ERR_OK;
    U32   i = 0;
    U8    *pu8CarStr = pu8IDBuffer;

    /* check param */
    if(pu8IDBuffer == NULL || u32Len > sizeof(s_tChip.m_ucarChipID))
    {
        WVDS2432_ErrorPrint(("[WVDS2432ReadRom]Check Param Failed!\r\n"));
        return ERR_RANGE;
    }

    /* read rom */
    if(Touch_Present())
    {
        /* write command */
        if(!Touch_Write_Byte(READ_ROM_CMD))
        {
            WVDS2432_ErrorPrint(("[WVDS2432ReadRom]Touch_Write_Byte(READ_ROM_CMD) Failed!\r\n"));
            emErrType = ERR_COLLISION;
        }

        /* read 8 bytes chip id */
        if(!emErrType)
        {
            for(i = 0; i < u32Len; i ++)
            {
                pu8IDBuffer[i] = Touch_Read_Byte();
            }
        }

        for(i = 0; i < u32Len; i ++)
        {
            printf("chip id[%d]: %x\n",i, pu8IDBuffer[i]);
        }

        /* 8 bit crc verify */
        if(!verify_CRC(*(pu8CarStr + (u32Len - 1)), calc_CRC(pu8CarStr, u32Len - 1)) && !emErrType)
        {
          /*
            WVDS2432_ErrorPrint(("##DEBUG## %02X%02X%02X%02X%02X%02X%02X%02X \r\n",
                    pu8CarStr[0],
                    pu8CarStr[1],
                    pu8CarStr[2],
                    pu8CarStr[3],
                    pu8CarStr[4],
                    pu8CarStr[5],
                    pu8CarStr[6],
                    pu8CarStr[7]));
            */
            WVDS2432_ErrorPrint(("[WVDS2432ReadRom]verify_CRC() Failed!\r\n"));
            emErrType = ERR_COMM;
        }
    }
    else
    {
        WVDS2432_ErrorPrint(("[WVDS2432ReadRom]Touch_Present() No Device Touch Present Failed!\r\n"));
        emErrType = ERR_NODEVICE;
    }

    /* copy chip id */
    memcpy(s_tChip.m_ucarChipID, pu8IDBuffer, sizeof(s_tChip.m_ucarChipID));

    return emErrType;
}

/*************************************************
  Function:       WVDS2432LoadSecret
  Description:    To load a secret into the DS2432 chip
  Input:          NONE
  Output:         NONE
  Return:         emErrType
  Others:
  Create:         2010/10/25
*************************************************/
enmWVDS2432ErrorType WVDS2432LoadSecret(void)
{
    enmWVDS2432ErrorType    emErrType = ERR_OK;
    U32   i = 0;
    U8    u8Temp = 0;
    U8    aru8AuthBytes[3] = {0};

    /* check chip id */
    //if(strtoul((const char*)s_tChip.m_ucarChipID,NULL,0) == 0)
    if(s_tChip.m_ucarChipID == 0)
    {
        emErrType = Resume();
    }
    else
    {
        //don't check famliy code
        emErrType = Match_Rom();
    }

    /* write secret to scratchpad */
    if(!emErrType)
    {
        emErrType = Write_SP(0x80, s_tBusMaster.m_ucarSecret, sizeof(s_tBusMaster.m_ucarSecret));
    }

    /* load secret */
    if(!emErrType)
    {
        emErrType = Resume();

        /* get TA1, TA2, E/S */
        if(!emErrType)
        {
            emErrType = Get_Auth(aru8AuthBytes);
        }

        if(!emErrType)
        {
            emErrType = Resume();

            if(!emErrType)
            {
                if(!Touch_Write_Byte(LOAD_SECRET_CMD))
                {
                    WVDS2432_ErrorPrint(("[WVDS2432LoadSecret]Touch_Write_Byte(LOAD_SECRET_CMD) Failed!\r\n"));
                    emErrType = ERR_COLLISION;
                }

                if(!emErrType)
                {
                    /* write TA1, TA2, E/S */
                    for(i = 0; i < AUTH_BYTES; i++)
                    {
                        if(!Touch_Write_Byte(aru8AuthBytes[i]))
                        {
                            WVDS2432_ErrorPrint(("[WVDS2432LoadSecret]Touch_Write_Byte() Write Auth Bytes Failed!\r\n"));
                            emErrType = ERR_COLLISION;
                        }
                    }

                    /* if TA1, TA2, E/S match, then copy secret */
                    if(!emErrType)
                    {
                        (void)Delay_ms(11);

                        u8Temp = Touch_Read_Byte();
                        if(u8Temp == 0xFF)
                        {
                            WVDS2432_ErrorPrint(("[WVDS2432LoadSecret]The End Byte is 0xFF Failed!\r\n"));
                            emErrType = ERR_BAD_AUTH;
                        }
                    }
                }
            }
        }
    }

    (void)Touch_Present();

    if(!emErrType)
        WVDS2432_DebugPrint(("[WVDS2432LoadSecret]Wellav DS2432 Load Secret Performed!\r\n"));
    else
        WVDS2432_DebugPrint(("[WVDS2432LoadSecret]Wellav DS2432 Load Secret Failed!\r\n"));

    return emErrType;
}

/*************************************************
  Function:       WVDS2432WriteAuthPage
  Description:    To write a verified page of data
  Input:          A pointer to an array of 32 bytes
                  data write to eeprom page
                  The length of the data will be written
                  to eeprom
  Output:         NONE
  Return:         emErrType
  Others:
  Create:         2010/10/25
*************************************************/
enmWVDS2432ErrorType WVDS2432WriteAuthPage(S32 s32EEPROMPage, U8 *pu8EepromWriteData, U32 u32Len)
{
    enmWVDS2432ErrorType    emErrType = ERR_OK;
    U8   u8EepromAddress = 0;
    U32  i = 0;

    /* check param */
    if(pu8EepromWriteData == NULL || u32Len > PAGE_DATA_LENGTH)
    {
        WVDS2432_ErrorPrint(("[WVDS2432WriteAuthPage]Check Param Failed!\r\n"));
        return ERR_RANGE;
    }

    /* wirte four times, each time 8 bytes */
    for(i = 0; i < 4; i++)
    {
        /* accord to the page calculate the address */
        u8EepromAddress = (U8)(i * SCRATCHPAD_LENGTH + s32EEPROMPage * PAGE_DATA_LENGTH);
        emErrType = Copy_Scratchpad(u8EepromAddress, pu8EepromWriteData + i * SCRATCHPAD_LENGTH);
    }

    if(!emErrType)
        WVDS2432_DebugPrint(("[WVDS2432WriteAuthPage]Wellav DS2432 Write Auth Page Performed!\r\n"));
    else
        WVDS2432_DebugPrint(("[WVDS2432WriteAuthPage]Wellav DS2432 Write Auth Page Failed!\r\n"));

    return emErrType;
}

/*************************************************
  Function:       WVDS2432ReadAuthPage
  Description:    To read a verified page of memory
                  after finish the authentication answer
  Input:          An array of 3 bytes random challenge
                  from bus master
  Output:         NONE
  Return:         emErrType
  Others:
  Create:         2010/10/25
*************************************************/
enmWVDS2432ErrorType WVDS2432ReadAuthPage(S32 s32EEPROMPage, U8 *pu8RandomChallenge, U32 u32Len)
{
    enmWVDS2432ErrorType    emErrType = ERR_OK;
    U32   i = 0;
    U8    aru8ChipMac[20];
    U8    aru8Scratch[36];
    U8    aru8SHAInput[64];
    U8    aru8MacArray[20];
    U8    u8Temp = 0;
    U16   u16Crc = 0;

    U8    u8PageAddress = s32EEPROMPage * 0x20;

    //init
    memset(aru8ChipMac,0x00,sizeof(aru8ChipMac));
    memset(aru8Scratch,0x00,sizeof(aru8Scratch));
    memset(aru8SHAInput,0x00,sizeof(aru8SHAInput));
    memset(aru8MacArray,0x00,sizeof(aru8MacArray));

    /* check param */
    if(pu8RandomChallenge == NULL || u32Len > AUTH_BYTES)
    {
        WVDS2432_ErrorPrint(("[WVDS2432ReadAuthPage]Check Param Failed!\r\n"));
        return ERR_RANGE;
    }

    /* authentication answer */
    if(s32EEPROMPage <= PAGE_MAX)
    {
        //don't check famliy code
        emErrType = Match_Rom();
        if(!emErrType)
        {
            /* 1-4 write 0 */
            for(i = 0; i < 4; i++)
            {
                aru8Scratch[i] = 0x00;
            }

            /* three bytes random data */
            aru8Scratch[4] = pu8RandomChallenge[0];
            aru8Scratch[5] = pu8RandomChallenge[1];
            aru8Scratch[6] = pu8RandomChallenge[2];
            aru8Scratch[7] = 0x00;

            /* write three bytes random data to scratch pad */
            emErrType = Write_SP(u8PageAddress, aru8Scratch, 8);

            if(!emErrType)
            {
                emErrType = Resume();

                if(!emErrType)
                {
                    /* write command */
                    if(!Touch_Write_Byte(READ_AUTH_PAGE_CMD))
                        emErrType = ERR_COLLISION;

                    if(!emErrType)
                        if(!Touch_Write_Byte(u8PageAddress))
                            emErrType = ERR_COLLISION;

                    if(!emErrType)
                        if(!Touch_Write_Byte(ADDRESS_MSB))
                            emErrType = ERR_COLLISION;

                    /* authentication answer */
                    if(!emErrType)
                    {
                        for(i = 0; i < 33; i++)
                        {
                            aru8Scratch[i + 3] = Touch_Read_Byte();
                        }

                        if(aru8Scratch[35] == 0xff)
                        {
                            u8Temp = Touch_Read_Byte();
                            u16Crc = make16(Touch_Read_Byte(), u8Temp);

                            aru8Scratch[0] = READ_AUTH_PAGE_CMD;
                            aru8Scratch[1] = u8PageAddress;
                            aru8Scratch[2] = ADDRESS_MSB;

                            if(!verify_CRC16(u16Crc, calc_CRC16(aru8Scratch, 36)))
                            {
                                WVDS2432_ErrorPrint(("[WVDS2432ReadAuthPage]verify_CRC16(PageData) Failed!\r\n"));
                                emErrType = ERR_COMM;
                            }

                            /* get sha input data */
                            if(!emErrType)
                            {
                                //M0
                                for(i = 0; i < 4; i++)
                                {
                                    aru8SHAInput[i] = s_tBusMaster.m_ucarSecret[i];
                                }

                                //M1 - M8
                                for(i = 3; i < 35; i++)
                                {
                                    aru8SHAInput[i + 1] = aru8Scratch[i];
                                }

                                //M9
                                aru8SHAInput[36] = 0xFF;
                                aru8SHAInput[37] = 0xFF;
                                aru8SHAInput[38] = 0xFF;
                                aru8SHAInput[39] = 0xFF;

                                //M10 - M11
                                aru8SHAInput[40] = s32EEPROMPage | 0x40;

                                for(i = 0; i < 7; i++)
                                {
                                    aru8SHAInput[i + 41] = s_tChip.m_ucarChipID[i];
                                }

                                //M12
                                for(i = 4; i < 8; i++)
                                {
                                    aru8SHAInput[i + 44] = s_tBusMaster.m_ucarSecret[i];
                                }

                                //M13 challenge
                                aru8SHAInput[52] = pu8RandomChallenge[0];
                                aru8SHAInput[53] = pu8RandomChallenge[1];
                                aru8SHAInput[54] = pu8RandomChallenge[2];

                                //M14 - M15
                                aru8SHAInput[55] = 0x80;
                                aru8SHAInput[56] = 0x00;
                                aru8SHAInput[57] = 0x00;
                                aru8SHAInput[58] = 0x00;
                                aru8SHAInput[59] = 0x00;
                                aru8SHAInput[60] = 0x00;
                                aru8SHAInput[61] = 0x00;
                                aru8SHAInput[62] = 0x01;
                                aru8SHAInput[63] = 0xB8;

                                /* SHA-1 arithmetic */
                                gen_MAC(aru8SHAInput, aru8MacArray);

                                (void)Delay_ms(2);

                                /* read 20 bytes MAC */
                                for(i = 0; i < 20; i++)
                                {
                                    aru8ChipMac[i] = Touch_Read_Byte();
                                }

                                u8Temp = Touch_Read_Byte();
                                u16Crc = make16(Touch_Read_Byte(), u8Temp);

                                if(!verify_CRC16(u16Crc, calc_CRC16(aru8ChipMac, 20)))
                                {
                                    WVDS2432_ErrorPrint(("[WVDS2432ReadAuthPage]verify_CRC16(ChipMac) Failed!\r\n"));
                                    emErrType = ERR_COMM;
                                }

                                if(!emErrType)
                                    if(!verify_CRC16(u16Crc, calc_CRC16(aru8MacArray, 20)))
                                    {
                                        WVDS2432_ErrorPrint(("[WVDS2432ReadAuthPage]verify_CRC16(HostMac) Failed!\r\n"));
                                        emErrType = ERR_COMM;
                                    }

                                if(!emErrType)
                                {
                                    for(i = 3; i < 35; i++)
                                    s_tChip.m_ucarEepromPageData[i-3] = aru8Scratch[i];
                                }
                            }
                            else
                            {
                                WVDS2432_ErrorPrint(("[WVDS2432ReadAuthPage]Write Command Failed!\r\n"));
                                emErrType = ERR_BAD_AUTH;
                            }
                        }
                    }
                }
            }
        }
    }
    else
    {
        WVDS2432_ErrorPrint(("[WVDS2432ReadAuthPage]Error Page Range Failed!\r\n"));
        emErrType = ERR_PAGE_RANGE;
    }

    (void)Touch_Present();

    if(!emErrType)
        WVDS2432_DebugPrint(("[WVDS2432ReadAuthPage]Wellav DS2432 Read Auth Page Performed!\r\n"));
    else
        WVDS2432_DebugPrint(("[WVDS2432ReadAuthPage]Wellav DS2432 Read Auth Page Failed!emErrType(%d)\r\n", emErrType));

    return emErrType;
}

/*************************************************
  Function:       WVDS2432ProtectSecret
  Description:    Write protects secret of the specified chip
  Input:          NONE
  Output:         NONE
  Return:         emErrType
  Others:
  Create:         2010/10/25
*************************************************/
enmWVDS2432ErrorType WVDS2432ProtectSecret(void)
{
    enmWVDS2432ErrorType    emErrType = ERR_OK;

    emErrType = Write_Byte(0x88, 0xAA);

    if(!emErrType)
    {
        WVDS2432_InfoPrint(("[WVDS2432ProtectSecret]Wellav DS2432 Protect Secret Performed!\r\n"));
    }
    else
    {
        WVDS2432_ErrorPrint(("[WVDS2432ProtectSecret]Wellav DS2432 Protect Secret Failed!\r\n"));
    }

    return emErrType;
}

/*************************************************
  Function:       WVDS2432ProtectPageZero
  Description:    Activate page 0 write protect function
  Input:          NONE
  Output:         NONE
  Return:         emErrType
  Others:
  Create:         2010/10/25
*************************************************/
enmWVDS2432ErrorType WVDS2432ProtectPageZero(void)
{
    enmWVDS2432ErrorType    emErrType = ERR_OK;

    emErrType = Write_Byte(0x8D, 0xAA);

    if(!emErrType)
        WVDS2432_DebugPrint(("[WVDS2432ProtectPageZero]Wellav DS2432 Protect Page Zero Performed!\r\n"));
    else
        WVDS2432_DebugPrint(("[WVDS2432ProtectPageZero]Wellav DS2432 Protect Page Zero Failed!\r\n"));

    return emErrType;
}

/*************************************************
  Function:       WVDS2432ProtectAllPages
  Description:    Activate all pages write protect function
  Input:          NONE
  Output:         NONE
  Return:         emErrType
  Others:
  Create:         2010/10/25
*************************************************/
enmWVDS2432ErrorType WVDS2432ProtectAllPages(void)
{
    enmWVDS2432ErrorType    emErrType = ERR_OK;

    emErrType = Write_Byte(0x89, 0xAA);

    if(!emErrType)
        WVDS2432_DebugPrint(("[WVDS2432ProtectPageZero]Wellav DS2432 Protect All Page Performed!\r\n"));
    else
        WVDS2432_DebugPrint(("[WVDS2432ProtectPageZero]Wellav DS2432 Protect All Page Failed!\r\n"));

    return emErrType;
}

/*************************************************
  Function:       WVDS2432ReadMemory
  Description:    Reads a number of user specified
                  bytes
  Input:          An int representing the address
                  of the byte to start reading from,
                  A pointer to an array where the
                  read bytes will be stored, and an
                  int holding the number of bytes to
                  be read in
                  The length of the data read
  Output:         The data read from chip
  Return:         emErrType
  Others:
  Create:         2010/10/25
*************************************************/
enmWVDS2432ErrorType WVDS2432ReadMemory(U8 u8ReadMemoryAddress, U8 *pu8DataBuffer, U32 u32Length)
{
    enmWVDS2432ErrorType    emErrType = ERR_OK;
    U32    i = 0;

    /* check param */
    if(u8ReadMemoryAddress > MAX_ADDRESS ||
       pu8DataBuffer == NULL ||
       u32Length > MAX_DATA_LENGTH)
    {
        WVDS2432_ErrorPrint(("[WVDS2432ReadMemory]Check Param Failed!\r\n"));
        return ERR_RANGE;
    }

    /* Send read memory command */
    if(!Touch_Write_Byte(READ_MEMORY_CMD))
        emErrType = ERR_COLLISION;

    /* Send lower byte of the address */
    if(!emErrType)
        if(!Touch_Write_Byte(u8ReadMemoryAddress))
            emErrType = ERR_COLLISION;

    /* Send upper byte of the address */
    if(!emErrType)
        if(!Touch_Write_Byte(ADDRESS_MSB))
            emErrType = ERR_COLLISION;

    if(!emErrType)
    {
        for(i = 0; i < u32Length; i++, pu8DataBuffer++)
        {
            *pu8DataBuffer = Touch_Read_Byte();
        }
    }

    (void)Touch_Present();

    if(!emErrType)
        WVDS2432_DebugPrint(("[WVDS2432ReadMemory]Wellav DS2432 Read Memory Performed!\r\n"));
    else
        WVDS2432_DebugPrint(("[WVDS2432ReadMemory]Wellav DS2432 Read Memory Failed!\r\n"));

    return emErrType;
}

