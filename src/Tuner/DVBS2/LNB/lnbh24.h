/*****************************************************************************/
/* COPYRIGHT (C) 2009 STMicroelectronics - All Rights Reserved               */
/* ST makes no warranty express or implied including but not limited to,     */
/* any warranty of                                                           */
/*                                                                           */
/*   (i)  merchantability or fitness for a particular purpose and/or         */
/*   (ii) requirements, for a particular purpose in relation to the LICENSED */
/*        MATERIALS, which is provided 揂S IS�, WITH ALL FAULTS. ST does not */
/*        represent or warrant that the LICENSED MATERIALS provided here     */
/*        under is free of infringement of any third party patents,          */
/*        copyrights,trade secrets or other intellectual property rights.    */
/*        ALL WARRANTIES, CONDITIONS OR OTHER TERMS IMPLIED BY LAW ARE       */
/*        EXCLUDED TO THE FULLEST EXTENT PERMITTED BY LAW                    */
/*                                                                           */
/*****************************************************************************/
                                                       

#ifndef __ST_SAT_LNBH24_H
#define __ST_SAT_LNBH24_H

#ifdef __cplusplus
extern "C"
{
#endif                          /* __cplusplus */



/* map lnb register controls of lnb */
#define LNBH24_NBREGS   1
#define LNBH24_NBFIELDS 8

/* IOCFG register */
#define  RLNBH24_REGS         0x00

/* IOCFG fields */
#define  FLNBH24_OLF        0x000001/* Over current protection Indicator */
#define  FLNBH24_OTF        0x000002 /* Over temperature protection Indicator */
#define  FLNBH24_EN         0x000004 /* Port Enable */
#define  FLNBH24_VSEL       0x000008 /* Voltage Select 13/18V */
#define  FLNBH24_LLC        0x000010 /* Coax Cable Loss Compensation */
#define  FLNBH24_TEN        0x000020 /* Tone Enable/Disable */
#define  FLNBH24_ISEL       0x000040 /*  LNB mode selection (Rx or Tx) */
#define  FLNBH24_PCL        0x000080/* Short Circuit Protection, Dynamic/Static Mode */

 typedef enum
 {
  LNB_IOCTL_PROTECTION_NOCHANGE = 0 ,   
  LNB_IOCTL_PROTECTION_STATIC,
  LNB_IOCTL_PROTECTION_DYNAMIC  
 }STFRONTEND_IOCTL_LNBShortCircuitProtectionMode_t;
 
typedef enum
{
 LNB_IOCTL_POWERBLCOKS_NOCHANGE = 0,
 LNB_IOCTL_POWERBLCOKS_ENABLED ,
 LNB_IOCTL_POWERBLCOKS_DISABLED 
}STFRONTEND_IOCTL_PowerControl_t;

typedef enum
{
 LNB_IOCTL_TTXMODE_NOCHANGE = 0,
 LNB_IOCTL_TTXMODE_RX,
 LNB_IOCTL_TTXMODE_TX  
}STFRONTEND_IOCTL_TTX_Mode_t;

typedef enum
{
 LNB_IOCTL_LLCMODE_NOCHANGE = 0,
 LNB_IOCTL_LLCMODE_ENABLED,
 LNB_IOCTL_LLCMODE_DISABLED  
}STFRONTEND_IOCTL_LLC_Mode_t;

typedef enum
{
    LNB_TONE_OFF = 0,       /* LNB disabled */
    LNB_TONE_22KHZ      /* LNB set to 22kHz */
} LNB_ToneState_t;

typedef enum
{
    LNB_STATUS_ON,
    LNB_STATUS_OFF
}LNB_Power_t;

typedef enum
{
    LNB_STATUS_NORMAL,
    LNB_STATUS_SHORT_CIRCUIT,
    LNB_STATUS_OVER_TEMPERATURE,
    LNB_STATUS_SHORTCIRCUIT_OVERTEMPERATURE   
} LNB_Status_t;

typedef enum
{
    VERTICAL = 0,
    HORIZONTAL,
    NOPOLARIZATION
}LNB_Polarization_t;

typedef enum
{
    LNB_NO_ERR = 0,
    LNB_ERR
}LNB_Error_t;

typedef struct
{
    char *pLNBName;                     /* LNB name */
    U8    LNBI2cAddress;                /* LNB I2C address */
}SAT_LNB_Params_t; 

/* API */
LNB_Error_t LNBH24_Init(U32 portIndex);
void        LNBH24_SetLnb(U32 portIndex, LNB_ToneState_t LnbTone);
void LNBH24_SetPolarization(U32 portIndex, LNB_Polarization_t Polarization);
#if 0
void        LNBH24_SetProtectionMode(LNBH24_InstanceData_t     *Instance, LNB_ShortCircuitProtectionMode_t ShortCircuitProtectionMode);
void        LNBH24_SetLossCompensation(LNBH24_InstanceData_t     *Instance, BOOL CoaxCableLossCompensation);
#endif
LNB_Error_t LNBH24_GetPower(U32 portIndex, LNB_Power_t *PowerStatus);
LNB_Error_t LNBH24_SetPower(U32 portIndex,  LNB_Power_t Status);
LNB_Error_t LNBH24_GetStatus(U32 portIndex, LNB_Status_t *Status);


#ifdef __cplusplus
}
#endif                          /* __cplusplus */

#endif


/* End of lnbh24.h */
