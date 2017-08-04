#ifndef MxL608_H
#define MxL608_H

    #include "MxL608_TunerApi.h"
    #include "MxL608_OEM_Drv.h"
    #include "AVL6381/ITuner.h"

    #ifdef AVL_CPLUSPLUS
extern "C" {
    #endif

        AVL6381_ErrorCode MxL608_Initialize(AVL_uint32 port, AVL_Tuner *pTuner);
        AVL6381_ErrorCode MxL608_GetStatus(AVL_uint32 u32Port,AVL_Tuner *pTuner);
        AVL6381_ErrorCode MxL608_Lock(Avl6381_PortIndex_t ePortIndex, AVL_Tuner *pTuner);

    #ifdef AVL_CPLUSPLUS
}
    #endif

#endif
