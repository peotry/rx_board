 
/* local to stlnb */

#include "appGlobal.h"
#include "xilinx_i2c.h"

#include "lnbh24.h"      /* header for this file */


#define WAIT_N_MS(ms)  usleep(ms*1000)


U8 LNB_I2C_ADDR[4] = {0x14, 0x10, 0x14, 0x10};



#define IBSP_I2C_TIMEOUT 1000

#define LNB_I2C_BUS I2C_0

extern int i2c_write_msg(int index, unsigned char address, unsigned char *value, unsigned char len);
extern int i2c_receive_msg(int index, unsigned char address, unsigned char *value, unsigned char len);


/* register field type */
typedef enum
{
    CHIP_UNSIGNED = 0,
    CHIP_SIGNED = 1
}
STCHIP_FieldType_t;




/* error codes */
typedef enum
{
    CHIPERR_NO_ERROR = 0,       /* No error encountered */
    CHIPERR_PARAMETER_ERR,
    CHIPERR_INVALID_HANDLE,     /* Using of an invalid chip handle */
    CHIPERR_I2C_NO_ACK,         /* No acknowledge from the chip */
    CHIPERR_I2C_BURST           /* Two many registers accessed in burst mode */
}
STCHIP_Error_t;


typedef struct
{
    U8  LNBH24_OLF ;     /* Over current protection Indicator */
    U8  LNBH24_OTF ;        /* Over temperature protection Indicator */
    U8  LNBH24_EN  ;        /* Port Enable */
    U8  LNBH24_VSEL;       /* Voltage Select 13/18V */
    U8  LNBH24_LLC ;        /* Coax Cable Loss Compensation */
    U8  LNBH24_TEN  ;       /* Tone Enable/Disable */
    U8  LNBH24_ISEL ;      /*  LNB mode selection (Rx or Tx) */
    U8  LNBH24_PCL  ;    /* Short Circuit Protection, Dynamic/Static Mode */    
}LNBH24_FieldVal_t;



/*****************************************************
**FUNCTION  ::  ChipGetFieldMask
**ACTION    ::  get the mask of a field in the chip
**PARAMS IN ::  
**PARAMS OUT::  mask
**RETURN    ::  mask
*****************************************************/
S32 ChipGetFieldMask(U32 FieldId)
{
    S32 mask;
    mask = FieldId & 0xFF; /*FieldId is [reg address][reg address][sign][mask] --- 4 bytes */ 
    
    return mask;
}

/*****************************************************
**FUNCTION  ::  ChipGetFieldSign
**ACTION    ::  get the sign of a field in the chip
**PARAMS IN ::  
**PARAMS OUT::  sign
**RETURN    ::  sign
*****************************************************/
S32 ChipGetFieldSign(U32 FieldId)
{
    S32 sign;
    sign = (FieldId>>8) & 0x01; /*FieldId is [reg address][reg address][sign][mask] --- 4 bytes */ 
    
    return sign;
}

/*****************************************************
**FUNCTION  ::  ChipGetFieldPosition
**ACTION    ::  get the position of a field in the chip
**PARAMS IN ::  
**PARAMS OUT::  position
**RETURN    ::  position
*****************************************************/
S32 ChipGetFieldPosition(U8 Mask)
{
    volatile S32 position=0, i=0;

    while((position == 0)&&(i < 8))
    {
        position = (Mask >> i) & 0x01;
        i++;
    }
  
    return (i-1);
}


/*****************************************************
**FUNCTION  ::  ChipGetFieldBits
**ACTION    ::  get the number of bits of a field in the chip
**PARAMS IN ::  
**PARAMS OUT::  number of bits
**RETURN    ::  number of bits
*****************************************************/
S32 ChipGetFieldBits(S32 mask, S32 Position)
{
    volatile S32 bits,bit;
    volatile S32 i =0;
 
    bits = mask >> Position;
    bit = bits ;
    while ((bit > 0)&&(i<8))
    {
        i++;
        bit = bits >> i;
    }
    return i;
}


/*****************************************************
**FUNCTION  ::  ChipGetOneRegister
**ACTION    ::  Get the value of one register 
**PARAMS IN ::  hChip   ==> Handle to the chip
**              reg_id  ==> Id of the register
**PARAMS OUT::  NONE
**RETURN    ::  Register's value
*****************************************************/
STCHIP_Error_t  ChipGetOneRegister(S32 *s32Err, U32 portIndex, U8 I2cAddr, U16 RegAddr)         
{
    U8  ucdata[2];
    U8  ucRAddr[1]; 


    ucRAddr[0] = (I2cAddr >> 1);
    
    if ((0 == portIndex) || (1 == portIndex))
    {
        *s32Err = i2c_receive_msg(I2C_0, ucRAddr[0], ucdata, 1);
    }
    else
    {
        *s32Err = i2c_receive_msg(I2C_1, ucRAddr[0], ucdata, 1);
    }
    

    return ucdata[0];

}



/*****************************************************
**FUNCTION  ::  ChipSetOneRegister
**ACTION    ::  Set the value of one register 
**PARAMS IN ::  hChip   ==> Handle to the chip
**              reg_id  ==> Id of the register
**              Data    ==> Data to store in the register
**PARAMS OUT::  NONE
**RETURN    ::  Error
*****************************************************/
STCHIP_Error_t ChipSetOneRegister(S32 *s32Err, U32 portIndex, U8 I2cAddr, U16 RegAddr,U8 Data)   
{
    U8  ucdata[3];
    U8  ucWAddr[1];

    ucdata[0] = Data;
    ucWAddr[0] = I2cAddr >> 1;
    


    if ((0 == portIndex) || (1 == portIndex))
    {
        *s32Err = i2c_write_msg(I2C_0, ucWAddr[0], ucdata, 1);
    }
    else
    {
        *s32Err = i2c_write_msg(I2C_1, ucWAddr[0], ucdata, 1);
    }
    
    

    return CHIPERR_NO_ERROR;

}



STCHIP_Error_t ChipSetField(U32 portIndex, U32 FieldId,S32 Value)
{
   
    S32 ERR = 0; 
    S32 regValue,
        mask,
        sign,
        bits,
        pos;
        
    U8 I2cAddr = LNB_I2C_ADDR[portIndex];


    regValue=ChipGetOneRegister(&ERR, portIndex, I2cAddr, (FieldId >> 16)&0xFFFF);      /*  Read the register   */

    if (ERR != CHIPERR_NO_ERROR)
    {
        printf("ChipSetField  ChipGetOneRegister fail!\r\n ");
    }
    
//  printf("[ChipSetField] regValue = %d!\r\n ", regValue);
    sign = ChipGetFieldSign(FieldId);
    mask = ChipGetFieldMask(FieldId);
    pos = ChipGetFieldPosition(mask);
    bits = ChipGetFieldBits(mask,pos);
            
    if(sign == CHIP_SIGNED)
        Value = (Value > 0 ) ? Value : Value + (bits);  /*  compute signed fieldval */

    Value = mask & (Value << pos);                      /*  Shift and mask value    */

    regValue=(regValue & (~mask)) + Value;      /*  Concat register value and fieldval  */
    ChipSetOneRegister(&ERR, portIndex, I2cAddr, (FieldId >> 16)&0xFFFF, regValue);      /*  Write the register */

    return ERR;
}



LNB_Error_t LNBH24_Init(U32 portIndex)
{
    S32 s32Err = CHIPERR_NO_ERROR;

    U8 I2cAddr = LNB_I2C_ADDR[portIndex];
    printf("***addr 0x%x\r\n", I2cAddr);
    ChipSetOneRegister(&s32Err, portIndex, I2cAddr, (U16) RLNBH24_REGS, 0xC8);
    
    if (s32Err != CHIPERR_NO_ERROR)
    {
        //printf("LNBH24_Init fail!\r\n ");
        return LNB_ERR;
    }

    WAIT_N_MS(25);

    return LNB_NO_ERR;
}

/*----------------------------------------------------
 FUNCTION      LNBH24_SetLnb
 ACTION        set the Lnb
 PARAMS IN     Lnb -> true for LnbHigh, false for LnbLow
 PARAMS OUT    NONE
 RETURN        NONE
------------------------------------------------------*/
void LNBH24_SetLnb(U32 portIndex, LNB_ToneState_t LnbTone)
{
    S32 s32Err = 0;
printf("**LNBH24_SetLnb**   port %d,tone %d \r\n", portIndex, LnbTone);    
    /* Low band -> no 22KHz tone */
    if (LnbTone == LNB_TONE_OFF)
    {
        // set ttx = 0 [shouquan.tang-20120918]:修复22K幅度(400mv-->650mv)
       s32Err |=  ChipSetField(portIndex, 0x000040, 0);
       s32Err |=  ChipSetField(portIndex, FLNBH24_TEN, 0);
    }
    
    /* High band -> 22KHz tone */
    if (LnbTone == LNB_TONE_22KHZ)
    {
        s32Err |= ChipSetField(portIndex, FLNBH24_TEN, 1);
        // set ttx = 1 [shouquan.tang-20120918]:修复22K幅度(400mv-->650mv)
        s32Err |= ChipSetField(portIndex, 0x000040, 1);
    }

    if (s32Err != CHIPERR_NO_ERROR)
    {
        printf("LNBH24_SetLnb fail!\r\n ");
    }
    else
    {
        printf("LNBH24_SetLnb succ!\r\n ");
    }
    return;
}


/*----------------------------------------------------
 FUNCTION      LNBH24_SetPolarization
 ACTION        set the polarization
 PARAMS IN     Polarization -> Polarization
 PARAMS OUT    NONE
 RETURN        NONE
------------------------------------------------------*/
void   LNBH24_SetPolarization(U32 portIndex, LNB_Polarization_t Polarization)
{
    S32 sReg;
    S32 Err = 0;
    
    U8 I2cAddr = LNB_I2C_ADDR[portIndex];
    
    if(Polarization == VERTICAL)/* Set 13V for Vertical Pol*/
    {
        ChipSetField(portIndex, FLNBH24_EN, 1);
        // set llc = 1 [shouquan.tang-20120920]:加1V，配合硬件LNB修改
        ChipSetField(portIndex, 0x000010, 1);
        ChipSetField(portIndex, FLNBH24_VSEL, 0);
    }
    else if(Polarization == HORIZONTAL)/* Set 18v for Horizontal Pol*/
    {
        ChipSetField(portIndex, FLNBH24_EN, 1);
        // set llc = 1 [shouquan.tang-20120920]:加1V，配合硬件LNB修改
        ChipSetField(portIndex, 0x000010, 1);
        ChipSetField(portIndex, FLNBH24_VSEL, 1);
    }
    else if(Polarization == NOPOLARIZATION)/* Set None*/
    {
        ChipSetField(portIndex, FLNBH24_EN, 0);
    }


    sReg = ChipGetOneRegister(&Err, portIndex, I2cAddr, (U16) RLNBH24_REGS);


    if (Err != CHIPERR_NO_ERROR)
    {
        printf("LNBH24_SetPolarization fail!\r\n ");
    }
    else
    {
        printf("LNBH24_SetPolarization succ!\r\n ");
    }       
}



/*----------------------------------------------------
 FUNCTION      LNBH24_SetCurrentThreshold
 ACTION        set the Current for LNB
 PARAMS IN     CurrentSelection 
 PARAMS OUT    NONE
 RETURN        NONE
------------------------------------------------------*/
/*
void LNBH24_SetCurrentThreshold( LNB_CurrentThresholdSelection_t CurrentThresholdSelection)
{
    if(CurrentThresholdSelection == CURRENT_THRESHOLD_HIGH)*//* Set ISEL=0 to set high(500-650mA)*/
        /*STTUNER_IOREG_FieldSetVal(DeviceMap, FLNB21_ISEL, 0);
        if(CurrentThresholdSelection == CURRENT_THRESHOLD_LOW)*//* Set ISEL=1 to set low(400-550mA)*/
       /* STTUNER_IOREG_FieldSetVal(DeviceMap, FLNB21_ISEL, 1);
}*/

#if 0
/*----------------------------------------------------
 FUNCTION      LNBH24_SetProtectionMode
 ACTION        set the short circuit for LNB
 PARAMS IN     ShortCircuitProtection 
 PARAMS OUT    NONE
 RETURN        NONE
------------------------------------------------------*/    
void LNBH24_SetProtectionMode(LNBH24_InstanceData_t     *Instance, LNB_ShortCircuitProtectionMode_t ShortCircuitProtectionMode)
{
    if(ShortCircuitProtectionMode == LNB_PROTECTION_DYNAMIC)/* dynamic short circuit protection*/
     ChipSetFieldImage(FLNBH24_PCL, 0,&(Instance->RegVal) );    
        
    if(ShortCircuitProtectionMode == LNB_PROTECTION_STATIC)/* static short circuit protection*/
    ChipSetFieldImage(FLNBH24_PCL, 1,&(Instance->RegVal)  );
}


/*----------------------------------------------------
 FUNCTION      LNBH24_SetLossCompensation
 ACTION        compensation for coaxial cable loss 
 PARAMS IN     CoaxCableLossCompensation 
 PARAMS OUT    NONE
 RETURN        NONE
------------------------------------------------------*/    
void LNBH24_SetLossCompensation(LNBH24_InstanceData_t     *Instance, BOOL CoaxCableLossCompensation)
{
    if(CoaxCableLossCompensation == TRUE)/* add +1 in 13/18V*/
    ChipSetFieldImage(FLNBH24_LLC, 1,&(Instance->RegVal)  );
    
    
    if(CoaxCableLossCompensation == FALSE)/* normal 13/18V */
    ChipSetFieldImage(FLNBH24_LLC, 0,&(Instance->RegVal)  );
}
#endif

/*----------------------------------------------------
 FUNCTION      LNBH24_GetPower
 ACTION        get lnb power status
 PARAMS IN     
 PARAMS OUT    
 RETURN        
------------------------------------------------------*/
LNB_Error_t LNBH24_GetPower(U32 portIndex, LNB_Power_t *PowerStatus)
{
    U8 I2cAddr = 0;  
    S32 s32Err = 0;
    U8 powerstatus;

    I2cAddr = LNB_I2C_ADDR[portIndex];

    powerstatus = ChipGetOneRegister(&s32Err, portIndex, I2cAddr, (U16) RLNBH24_REGS);
    if((powerstatus & 0x04) == 0x04)
    {
        *PowerStatus = LNB_STATUS_ON;
    }
    else
    {
        *PowerStatus = LNB_STATUS_OFF;
    }

    return LNB_NO_ERR;
}

/*----------------------------------------------------
 FUNCTION     LNBH24_SetPower

 ACTION        set lnb power
 PARAMS IN     
 PARAMS OUT    
 RETURN        
------------------------------------------------------*/
LNB_Error_t LNBH24_SetPower(U32 portIndex,  LNB_Power_t Status)
{
    S32 Error = CHIPERR_NO_ERROR;

    switch (Status)
    {
        case LNB_STATUS_ON:
        Error |= ChipSetField(portIndex, FLNBH24_EN, 1);
        break;
        
        case LNB_STATUS_OFF:
        Error |= ChipSetField(portIndex, FLNBH24_EN, 0);
        break;

        default:
        break;
    }

    return(Error);
}

/*----------------------------------------------------
 FUNCTION     LNBH24_GetStatus

 ACTION        To read the diagnostic flag register and identify the following cases
 
 1) Short circuit
 2) Over temperature
 3) Both Shortcircuit and over temperature
 4) Outut voltage range
 5) LNB present or not
 6) 22khz tone present or not
 
           
 PARAMS IN     
 PARAMS OUT    
 RETURN        
------------------------------------------------------*/
LNB_Error_t LNBH24_GetStatus(U32 portIndex, LNB_Status_t *Status)
{
    S32 s32Err  = 0;

    U8 DIAG_FLAG = LNB_STATUS_NORMAL;

    U8 I2cAddr = 0;  

    
    I2cAddr = LNB_I2C_ADDR[portIndex];


    DIAG_FLAG = ChipGetOneRegister(&s32Err, portIndex, I2cAddr, (U16)RLNBH24_REGS);
    if (s32Err != CHIPERR_NO_ERROR)
    {
        return LNB_ERR;
    }

    *Status = LNB_STATUS_NORMAL;
    
    if((DIAG_FLAG & 0x01)== 0x01)
        (*Status) = LNB_STATUS_SHORT_CIRCUIT;
    
    if((DIAG_FLAG & 0x02) == 0x02)
        (*Status) = LNB_STATUS_OVER_TEMPERATURE;
        
    if((DIAG_FLAG & 0x03) == 0x03)
        (*Status) = LNB_STATUS_SHORTCIRCUIT_OVERTEMPERATURE;

    
    return LNB_NO_ERR;      
}


