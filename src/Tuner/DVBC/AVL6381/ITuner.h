#ifndef ITUNER_H
#define ITUNER_H

#include "AVL6381_API.h"

    #ifdef AVL_CPLUSPLUS
extern "C" {
    #endif

struct Tuner_Status
{
    AVL_uint16 m_uiReset;
    AVL_uint16 m_uiLock;
    AVL_uint16 m_uiALBC;
    AVL_uint16 m_uiAGC;
};

typedef enum AVL6381_TunerLockStatus
{
    AVL6381_TUNER_UNLOCK = 0,       
    AVL6381_TUNER_LOCKED = 1
}AVL6381_TunerLockStatus;


typedef struct AVL_Tuner
{
    AVL_uint16 m_uiSlaveAddress;
    AVL_uint16 m_uiI2C_BusClockSpeed_kHz;//should be removed
    AVL_uint32 m_uiFrequency_Hz;

    AVL_uint32 uiIFHz;
    AVL_uint32 uiBandwidthHz;//new added
    AVL6381_AGCPola enumAGCPola;//new added
    AVL6381_TunerLockStatus enumLockStatus;//new added
    AVL_uchar m_ucDTVMode;//new added
    AVL_uchar m_ucCrystal;//new added for crystal option

    struct Tuner_Status m_sStatus;//shoule be removed
    void * m_pParameters;//not used yet, keep it for furture extension
    
    struct AVL6381_Chip * m_pAVL_Chip;//should be removed
    AVL_uint16 (* m_pInitializeFunc)(struct AVL_Tuner *);       ///< A pointer to the tuner initialization function.
    AVL6381_ErrorCode (* m_pGetLockStatusFunc)(struct AVL_Tuner *);     ///< A pointer to the tuner GetLockStatus function.
    AVL6381_ErrorCode (* m_pLockFunc)(struct AVL_Tuner *);          ///< A pointer to the tuner Lock function.
    AVL6381_ErrorCode (* m_pGetRFStrengthFunc)(struct AVL_Tuner *, AVL_int32 *);

}AVL_Tuner;

    #ifdef AVL_CPLUSPLUS
}
    #endif
    
#endif

