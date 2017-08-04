/*
Filename:       RDA5815M_1.X.c
Description:    RDA5815M Digital Satellite Tuner IC driver.
version 1.0     The primary version,  created by hongxin wang  2012-8-28
version 1.1     Modify the register 0x38 from 0x93 to 0x 9B for increasing the amplitude of XOUT
version 1.2     Add configuration for Xtal = 30MHz. 2013-04-26
version 1.3     Add configuration for Xtal = 24MHz. 2013-06-09
version 1.4     AGC optimized to enhance max gain. 2013-06-27
version 1.4.1   Code simplified to avoid bugs for modes -- Xtal_24M and Xtal_30M. 2013-07-25
version 1.5     Optimize bandwidth calculation. 2013-8-19
version 1.6     Correct some functions' return value. Add APIs -- "RDA5815_LockStatus", "RDA5815_Sleep", "RDA5815_Wakeup", "RDA5815_RunningStatus". 2013-11-20
*/

#include "appGlobal.h"
#include "RDA5815M.h"
#include "xilinx_i2c.h"

#include "II2C.h"
#include "Avl_dvbsx_globals.h"


#define Xtal_27M
//#define Xtal_30M
//#define Xtal_24M

#define     RDA_Tuner_Sleeping      0x02
#define     RDA_Tuner_Waking        0x03

#define RDA5815Sleep(x)   usleep(x*1000)



extern int i2c_write_msg(int index, unsigned char address, unsigned char *value, unsigned char len);


#if 0
static AVL_DVBSx_ErrorCode AV2011_I2C_write(U8 reg_start,U8* buff,U8 len, struct AVL_DVBSx_Chip * pAVLChip)
{
    AVL_DVBSx_ErrorCode r=0;
    AVL_uint16 uiTimeOut = 0;
    AVL_uchar ucTemp[50];
    int i;
    AVL_DVBSx_IBSP_Delay(5);
    ucTemp[0] = reg_start;
    r = AVL_DVBSx_II2CRepeater_GetOPStatus( pAVLChip );
    while( r != AVL_DVBSx_EC_OK)
    {
#ifdef AV2011_ENABLE_TIMEOUT
            if( uiTimeOut++>AV2011_TIMEOUT ) 
            {
                return(AVL_DVBSx_EC_TimeOut);
            }
#endif
            AVL_DVBSx_IBSP_Delay(1);
            r = AVL_DVBSx_II2CRepeater_GetOPStatus( pAVLChip );
    }
    
    for(i=1;i<len+1;i++)
    {
       // ucTemp[i++]=AVL_uchar(reg_start+i);
        ucTemp[i]=*(buff+i-1);
    }
    r = AVL_DVBSx_II2CRepeater_SendData((AVL_uchar) TUNER_ADDRESS,ucTemp, len+1, pAVLChip );
    if(r != AVL_DVBSx_EC_OK){
        return(r);
    }
    AVL_DVBSx_IBSP_Delay(1);
      return(r);
}
#endif


#if 0
static void RDA5815WriteReg(U8 Register, U8 Value, struct AVL_DVBSx_Chip * pAVLChip)
{
    U8 u8Data[2] = {0};
    
    u8Data[0] = Register;
    u8Data[1] = Value;
    i2c_write_msg(I2C_0, TUNER_ADDRESS >> 1, u8Data, 2);
}

#else

static void RDA5815WriteReg(AVL_uint32  u32Port, U8 Register, U8 Value, struct AVL_DVBSx_Chip * pAVLChip)
{
    AVL_DVBSx_ErrorCode r=0;
    //AVL_uint16 uiTimeOut = 0;
    //AVL_uchar ucTemp[50];
    AVL_DVBSx_IBSP_Delay(5);
    //ucTemp[0] = reg_start;

    r = AVL_DVBSx_II2CRepeater_GetOPStatus(u32Port, pAVLChip );
    while( r != AVL_DVBSx_EC_OK)
    {
#ifdef AV2011_ENABLE_TIMEOUT
            if( uiTimeOut++>AV2011_TIMEOUT ) 
            {
                return(AVL_DVBSx_EC_TimeOut);
            }
#endif
            AVL_DVBSx_IBSP_Delay(1);
            r = AVL_DVBSx_II2CRepeater_GetOPStatus(u32Port, pAVLChip );
    }
    

    U8 u8Data[2] = {0};
    
    u8Data[0] = Register;
    u8Data[1] = Value;
    r = AVL_DVBSx_II2CRepeater_SendData(u32Port,(AVL_uchar) TUNER_ADDRESS, u8Data, 2, pAVLChip);
   // i2c_write_msg(I2C_0, TUNER_ADDRESS >> 1, u8Data, 2);
}

#endif


void RDA5815Initial(AVL_uint32 u32Port, struct AVL_DVBSx_Chip * pAVLChip)
{    
    RDA5815Sleep(1);//Wait 1ms. 
     // Chip register soft reset    
    RDA5815WriteReg(u32Port, 0x04,0x04, pAVLChip);
    RDA5815WriteReg(u32Port, 0x04,0x05, pAVLChip); 

    // Initial configuration start

    //pll setting 
    RDA5815WriteReg(u32Port, 0x1a,0x13, pAVLChip);
    RDA5815WriteReg(u32Port, 0x41,0x53, pAVLChip);
    RDA5815WriteReg(u32Port, 0x38,0x9B, pAVLChip);
    RDA5815WriteReg(u32Port, 0x39,0x15, pAVLChip);
    RDA5815WriteReg(u32Port, 0x3A,0x00, pAVLChip);
    RDA5815WriteReg(u32Port, 0x3B,0x00, pAVLChip);
    RDA5815WriteReg(u32Port, 0x3C,0x0c, pAVLChip);
    RDA5815WriteReg(u32Port, 0x0c,0xE2, pAVLChip);
    RDA5815WriteReg(u32Port, 0x2e,0x6F, pAVLChip);

#ifdef Xtal_27M
    RDA5815WriteReg(u32Port, 0x72,0x07, pAVLChip); // v1.1, 1538~1539
    RDA5815WriteReg(u32Port, 0x73,0x10, pAVLChip);
    RDA5815WriteReg(u32Port, 0x74,0x71, pAVLChip);
    RDA5815WriteReg(u32Port, 0x75,0x06, pAVLChip); // v1.1, 1363~1364, 1862~1863
    RDA5815WriteReg(u32Port, 0x76,0x40, pAVLChip);
    RDA5815WriteReg(u32Port, 0x77,0x89, pAVLChip);
    RDA5815WriteReg(u32Port, 0x79,0x04, pAVLChip); // v1.1, 900
    RDA5815WriteReg(u32Port, 0x7A,0x2A, pAVLChip);
    RDA5815WriteReg(u32Port, 0x7B,0xAA, pAVLChip);
    RDA5815WriteReg(u32Port, 0x7C,0xAB, pAVLChip);
#elif defined(Xtal_30M)
    RDA5815WriteReg(u32Port, 0x72,0x06, pAVLChip); // v1.2, 1544~1545
    RDA5815WriteReg(u32Port, 0x73,0x60, pAVLChip);
    RDA5815WriteReg(u32Port, 0x74,0x66, pAVLChip);
    RDA5815WriteReg(u32Port, 0x75,0x05, pAVLChip); // v1.2, 1364~1365, 1859~1860
    RDA5815WriteReg(u32Port, 0x76,0xA0, pAVLChip);
    RDA5815WriteReg(u32Port, 0x77,0x7B, pAVLChip);
    RDA5815WriteReg(u32Port, 0x79,0x03, pAVLChip); // v1.2, 901
    RDA5815WriteReg(u32Port, 0x7A,0xC0, pAVLChip);
    RDA5815WriteReg(u32Port, 0x7B,0x00, pAVLChip);
    RDA5815WriteReg(u32Port, 0x7C,0x00, pAVLChip);
#elif defined(Xtal_24M)
    RDA5815WriteReg(u32Port, 0x72,0x08, pAVLChip); // v1.3, 1547~1548
    RDA5815WriteReg(u32Port, 0x73,0x00, pAVLChip);
    RDA5815WriteReg(u32Port, 0x74,0x80, pAVLChip);
    RDA5815WriteReg(u32Port, 0x75,0x07, pAVLChip); // v1.3, 1367~1368, 1859~1860
    RDA5815WriteReg(u32Port, 0x76,0x10, pAVLChip);
    RDA5815WriteReg(u32Port, 0x77,0x9A, pAVLChip);
    RDA5815WriteReg(u32Port, 0x79,0x04, pAVLChip); // v1.3, 901
    RDA5815WriteReg(u32Port, 0x7A,0xB0, pAVLChip);
    RDA5815WriteReg(u32Port, 0x7B,0x00, pAVLChip);
    RDA5815WriteReg(u32Port, 0x7C,0x00, pAVLChip);
#endif

    RDA5815WriteReg(u32Port, 0x2f,0x57, pAVLChip);
    RDA5815WriteReg(u32Port, 0x0d,0x70, pAVLChip);
    RDA5815WriteReg(u32Port, 0x18,0x4B, pAVLChip);
    RDA5815WriteReg(u32Port, 0x30,0xFF, pAVLChip);
    RDA5815WriteReg(u32Port, 0x5c,0xFF, pAVLChip);
    RDA5815WriteReg(u32Port, 0x65,0x00, pAVLChip);
    RDA5815WriteReg(u32Port, 0x70,0x3F, pAVLChip);
    RDA5815WriteReg(u32Port, 0x71,0x3F, pAVLChip);
    RDA5815WriteReg(u32Port, 0x53,0xA8, pAVLChip);
    RDA5815WriteReg(u32Port, 0x46,0x21, pAVLChip);
    RDA5815WriteReg(u32Port, 0x47,0x84, pAVLChip);
    RDA5815WriteReg(u32Port, 0x48,0x10, pAVLChip);
    RDA5815WriteReg(u32Port, 0x49,0x08, pAVLChip);
    RDA5815WriteReg(u32Port, 0x60,0x80, pAVLChip);
    RDA5815WriteReg(u32Port, 0x61,0x80, pAVLChip);
    RDA5815WriteReg(u32Port, 0x6A,0x08, pAVLChip);
    RDA5815WriteReg(u32Port, 0x6B,0x63, pAVLChip);
    RDA5815WriteReg(u32Port, 0x69,0xF8, pAVLChip);
    RDA5815WriteReg(u32Port, 0x57,0x64, pAVLChip);
    RDA5815WriteReg(u32Port, 0x05,0xaa, pAVLChip);
    RDA5815WriteReg(u32Port, 0x06,0xaa, pAVLChip);
    RDA5815WriteReg(u32Port, 0x15,0xAE, pAVLChip);
    RDA5815WriteReg(u32Port, 0x4a,0x67, pAVLChip);
    RDA5815WriteReg(u32Port, 0x4b,0x77, pAVLChip);

    //agc setting

    RDA5815WriteReg(u32Port, 0x4f,0x40, pAVLChip);
    RDA5815WriteReg(u32Port, 0x5b,0x20, pAVLChip);

    RDA5815WriteReg(u32Port, 0x16,0x0C, pAVLChip);
    RDA5815WriteReg(u32Port, 0x18,0x0C, pAVLChip);            
    RDA5815WriteReg(u32Port, 0x30,0x1C, pAVLChip);            
    RDA5815WriteReg(u32Port, 0x5c,0x2C, pAVLChip);            
    RDA5815WriteReg(u32Port, 0x6c,0x3C, pAVLChip);            
    RDA5815WriteReg(u32Port, 0x6e,0x3C, pAVLChip);            
    RDA5815WriteReg(u32Port, 0x1b,0x7C, pAVLChip);            
    RDA5815WriteReg(u32Port, 0x1d,0xBD, pAVLChip);            
    RDA5815WriteReg(u32Port, 0x1f,0xBD, pAVLChip);            
    RDA5815WriteReg(u32Port, 0x21,0xBE, pAVLChip);            
    RDA5815WriteReg(u32Port, 0x23,0xBE, pAVLChip);            
    RDA5815WriteReg(u32Port, 0x25,0xFE, pAVLChip);            
    RDA5815WriteReg(u32Port, 0x27,0xFF, pAVLChip);            
    RDA5815WriteReg(u32Port, 0x29,0xFF, pAVLChip);            
    RDA5815WriteReg(u32Port, 0xb3,0xFF, pAVLChip);            
    RDA5815WriteReg(u32Port, 0xb5,0xFF, pAVLChip);            

    RDA5815WriteReg(u32Port, 0x17,0xF0, pAVLChip);            
    RDA5815WriteReg(u32Port, 0x19,0xF0, pAVLChip);            
    RDA5815WriteReg(u32Port, 0x31,0xF0, pAVLChip);            
    RDA5815WriteReg(u32Port, 0x5d,0xF0, pAVLChip);            
    RDA5815WriteReg(u32Port, 0x6d,0xF0, pAVLChip);            
    RDA5815WriteReg(u32Port, 0x6f,0xF1, pAVLChip);            
    RDA5815WriteReg(u32Port, 0x1c,0xF5, pAVLChip);            
    RDA5815WriteReg(u32Port, 0x1e,0x35, pAVLChip);            
    RDA5815WriteReg(u32Port, 0x20,0x79, pAVLChip);            
    RDA5815WriteReg(u32Port, 0x22,0x9D, pAVLChip);            
    RDA5815WriteReg(u32Port, 0x24,0xBE, pAVLChip);            
    RDA5815WriteReg(u32Port, 0x26,0xBE, pAVLChip);            
    RDA5815WriteReg(u32Port, 0x28,0xBE, pAVLChip);            
    RDA5815WriteReg(u32Port, 0x2a,0xCF, pAVLChip);            
    RDA5815WriteReg(u32Port, 0xb4,0xDF, pAVLChip);            
    RDA5815WriteReg(u32Port, 0xb6,0x0F, pAVLChip);            

    RDA5815WriteReg(u32Port, 0xb7,0x15, pAVLChip); //start    
    RDA5815WriteReg(u32Port, 0xb9,0x6c, pAVLChip);            
    RDA5815WriteReg(u32Port, 0xbb,0x63, pAVLChip);            
    RDA5815WriteReg(u32Port, 0xbd,0x5a, pAVLChip);            
    RDA5815WriteReg(u32Port, 0xbf,0x5a, pAVLChip);            
    RDA5815WriteReg(u32Port, 0xc1,0x55, pAVLChip);            
    RDA5815WriteReg(u32Port, 0xc3,0x55, pAVLChip);            
    RDA5815WriteReg(u32Port, 0xc5,0x47, pAVLChip);            
    RDA5815WriteReg(u32Port, 0xa3,0x53, pAVLChip);            
    RDA5815WriteReg(u32Port, 0xa5,0x4f, pAVLChip);            
    RDA5815WriteReg(u32Port, 0xa7,0x4e, pAVLChip);            
    RDA5815WriteReg(u32Port, 0xa9,0x4e, pAVLChip);            
    RDA5815WriteReg(u32Port, 0xab,0x54, pAVLChip);            
    RDA5815WriteReg(u32Port, 0xad,0x31, pAVLChip);            
    RDA5815WriteReg(u32Port, 0xaf,0x43, pAVLChip);            
    RDA5815WriteReg(u32Port, 0xb1,0x9f, pAVLChip);               

    RDA5815WriteReg(u32Port, 0xb8,0x6c, pAVLChip); //end      
    RDA5815WriteReg(u32Port, 0xba,0x92, pAVLChip);            
    RDA5815WriteReg(u32Port, 0xbc,0x8a, pAVLChip);            
    RDA5815WriteReg(u32Port, 0xbe,0x8a, pAVLChip);            
    RDA5815WriteReg(u32Port, 0xc0,0x82, pAVLChip);            
    RDA5815WriteReg(u32Port, 0xc2,0x93, pAVLChip);            
    RDA5815WriteReg(u32Port, 0xc4,0x85, pAVLChip);            
    RDA5815WriteReg(u32Port, 0xc6,0x77, pAVLChip);            
    RDA5815WriteReg(u32Port, 0xa4,0x82, pAVLChip);            
    RDA5815WriteReg(u32Port, 0xa6,0x7e, pAVLChip);            
    RDA5815WriteReg(u32Port, 0xa8,0x7d, pAVLChip);            
    RDA5815WriteReg(u32Port, 0xaa,0x6f, pAVLChip);            
    RDA5815WriteReg(u32Port, 0xac,0x65, pAVLChip);            
    RDA5815WriteReg(u32Port, 0xae,0x43, pAVLChip);            
    RDA5815WriteReg(u32Port, 0xb0,0x9f, pAVLChip);             
    RDA5815WriteReg(u32Port, 0xb2,0xf0, pAVLChip);             

    RDA5815WriteReg(u32Port, 0x81,0x92, pAVLChip); //rise     
    RDA5815WriteReg(u32Port, 0x82,0xb4, pAVLChip);            
    RDA5815WriteReg(u32Port, 0x83,0xb3, pAVLChip);            
    RDA5815WriteReg(u32Port, 0x84,0xac, pAVLChip);            
    RDA5815WriteReg(u32Port, 0x85,0xba, pAVLChip);            
    RDA5815WriteReg(u32Port, 0x86,0xbc, pAVLChip);            
    RDA5815WriteReg(u32Port, 0x87,0xaf, pAVLChip);            
    RDA5815WriteReg(u32Port, 0x88,0xa2, pAVLChip);            
    RDA5815WriteReg(u32Port, 0x89,0xac, pAVLChip);            
    RDA5815WriteReg(u32Port, 0x8a,0xa9, pAVLChip);            
    RDA5815WriteReg(u32Port, 0x8b,0x9b, pAVLChip);            
    RDA5815WriteReg(u32Port, 0x8c,0x7d, pAVLChip);            
    RDA5815WriteReg(u32Port, 0x8d,0x74, pAVLChip);            
    RDA5815WriteReg(u32Port, 0x8e,0x9f, pAVLChip);           
    RDA5815WriteReg(u32Port, 0x8f,0xf0, pAVLChip);               
                                     
    RDA5815WriteReg(u32Port, 0x90,0x15, pAVLChip); //fall     
    RDA5815WriteReg(u32Port, 0x91,0x39, pAVLChip);            
    RDA5815WriteReg(u32Port, 0x92,0x30, pAVLChip);            
    RDA5815WriteReg(u32Port, 0x93,0x27, pAVLChip);            
    RDA5815WriteReg(u32Port, 0x94,0x29, pAVLChip);            
    RDA5815WriteReg(u32Port, 0x95,0x0d, pAVLChip);            
    RDA5815WriteReg(u32Port, 0x96,0x10, pAVLChip);            
    RDA5815WriteReg(u32Port, 0x97,0x1e, pAVLChip);            
    RDA5815WriteReg(u32Port, 0x98,0x1a, pAVLChip);            
    RDA5815WriteReg(u32Port, 0x99,0x19, pAVLChip);            
    RDA5815WriteReg(u32Port, 0x9a,0x19, pAVLChip);            
    RDA5815WriteReg(u32Port, 0x9b,0x32, pAVLChip);            
    RDA5815WriteReg(u32Port, 0x9c,0x1f, pAVLChip);            
    RDA5815WriteReg(u32Port, 0x9d,0x31, pAVLChip);            
    RDA5815WriteReg(u32Port, 0x9e,0x43, pAVLChip);            

    RDA5815Sleep(10);//Wait 10ms; 

    // Initial configuration end
}                      
    
/********************************************************************************/
//  Function to Set the RDA5815                       
//  fPLL:       Frequency           unit: MHz from 250 to 2300 
//  fSym:   SymbolRate          unit: KSps from 1000 to 60000 
/********************************************************************************/
                         
unsigned char RDA5815Set(AVL_uint32 u32Port, unsigned long fPLL, unsigned long fSym, struct AVL_DVBSx_Chip * pAVLChip)
{                              
    unsigned char buffer = 0; 
    unsigned long temp_value = 0;
    unsigned long bw = 0;/*,temp_value1 = 0,temp_value2=0 ;*/
    unsigned char Filter_bw_control_bit = 0;    

    RDA5815WriteReg(u32Port, 0x04,0xc1, pAVLChip); //add by rda 2011.8.9,RXON = 0 , change normal working state to idle state
    RDA5815WriteReg(u32Port, 0x2b,0x95, pAVLChip);//clk_interface_27m=0  add by rda 2012.1.12

    //set frequency start
#ifdef Xtal_27M     // v1.1
    temp_value = (unsigned long)fPLL* 77672;//((2^21) / RDA5815_XTALFREQ);
#elif defined(Xtal_30M)     // v1.2
    temp_value = (unsigned long)fPLL* 69905;//((2^21) / RDA5815_XTALFREQ);
#elif defined(Xtal_24M)     // v1.3
    temp_value = (unsigned long)fPLL* 87381;//((2^21) / RDA5815_XTALFREQ);
#endif

    buffer = ((unsigned char)((temp_value>>24)&0xff));
    RDA5815WriteReg(u32Port, 0x07,buffer, pAVLChip);
    buffer = ((unsigned char)((temp_value>>16)&0xff));  
    RDA5815WriteReg(u32Port, 0x08,buffer, pAVLChip);   
    buffer = ((unsigned char)((temp_value>>8)&0xff));
    RDA5815WriteReg(u32Port, 0x09,buffer, pAVLChip);   
    buffer = ((unsigned char)( temp_value&0xff));
    RDA5815WriteReg(u32Port, 0x0a,buffer, pAVLChip);
    //set frequency end
    
    // set Filter bandwidth start
    bw=fSym;        //kHz

    //135 means 1+rolloff, 4000 means offset, they can be modified to make filter narrower.  20140625
    Filter_bw_control_bit = (unsigned char)((bw*135/200+4000)/1000);   

    if(Filter_bw_control_bit<4)
        Filter_bw_control_bit = 4;    // MHz
    else if(Filter_bw_control_bit>40)
        Filter_bw_control_bit = 40;   // MHz
    
    Filter_bw_control_bit&=0x3f;
    Filter_bw_control_bit|=0x40;        //v1.5

    RDA5815WriteReg(u32Port, 0x0b,Filter_bw_control_bit, pAVLChip);
    // set Filter bandwidth end
    
    RDA5815WriteReg(u32Port, 0x04,0xc3, pAVLChip);     //add by rda 2011.8.9,RXON = 0 ,rxon=1,normal working
    RDA5815WriteReg(u32Port, 0x2b,0x97, pAVLChip);     //clk_interface_27m=1  add by rda 2012.1.12  
    RDA5815Sleep(5);//Wait 5ms;
  
    return 0;   
}




unsigned  int RDA5815Lock(AVL_uint32 u32Port, AVL_uint32 u32Frequency, AVL_uint32 u32SymbolRate, struct AVL_DVBSx_Chip * pAVLChip)
{ 
    U8 err = 0;


printf("***** RDA5815Lock   freq %d, symbol rate %d\r\n", u32Frequency, u32SymbolRate);
   // HDIC2501_OpenTunerI2C(TUNER_ADDRESS);

    RDA5815Initial( u32Port, pAVLChip);

    /*需要锁两次*/
    err = RDA5815Set( u32Port, u32Frequency, u32SymbolRate, pAVLChip);

    return err;
}



#if 0
// to get the lock status of tuner VCO & PLL
unsigned char RDA5815_LockStatus(unsigned char tuner_addr)
{
    unsigned char buffer, i ;

    for(i=0; i<100; i++)        // Loop times: 100 is recommanded; at least 30.
    {
        RDA5816sw_IIC_Read(tuner_addr,0x03,&buffer);
        if((buffer & 0x03) != 0x03)
        {
            return 1;
        }
    }
    
    return 0;
}

// to set RDA tuner to sleep status (low power consumption)
unsigned char RDA5815_Sleep(unsigned char tuner_addr)
{
    unsigned char buffer;

    RDA5816sw_IIC_Read(tuner_addr,0x04,&buffer);
    if ((buffer==0xff) || (buffer==0))
    {
        return 1;
    }
    else
    {
        RDA5815WriteReg(0x04, buffer & 0x7F);   
    }
    return 0;
}

// to wake up RDA tuner from sleep status
unsigned char RDA5815_Wakeup(unsigned char tuner_addr)
{
    unsigned char buffer;

    RDA5816sw_IIC_Read(tuner_addr,0x04,&buffer);
    if ((buffer==0xff) || (buffer==0))
    {
        return 1;
    }
    else
    {
        RDA5815WriteReg(0x04, buffer | 0x80);   
    }
    return 0;
}

// to get the running status of RDA tuner -- "Sleeping" or "Waking".
unsigned char RDA5815_RunningStatus(unsigned char tuner_addr)
{
    unsigned char buffer;

    RDA5816sw_IIC_Read(tuner_addr,0x04,&buffer);
    if ((buffer==0xff) || (buffer==0))
    {
        return 1;
    }
    else
    {
        if(buffer&0x80)
        {
            return RDA_Tuner_Waking;
        }
        else
        {
            return RDA_Tuner_Sleeping;
        }
    }
    return 0;
}


#endif

