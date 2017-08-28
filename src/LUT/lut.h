/*
 * =====================================================================================
 *
 *       Filename:  lut.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/27/17 20:11:30
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  linsheng.pan (), life_is_legend@163.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef INCLUDE_LUT_H
#define INCLUDE_LUT_H 


#include "err/wv_err.h"
#include "appGlobal.h"
#include "TSProcess.h"

wvErrCode LUT_SetInputLUTIPInfo(U8 u8LUTIndex, U32 u32IP, U16 u16UDPPort, U8 aru8MAC[6]);
wvErrCode LUT_ClearInputLUT(U8 u8LUTIndex);
wvErrCode LUT_GetInputLUTEntry(U8 u8DestSlot, TSInfo *pstParamTS, INPUTLutEntry arstLUTEntry[MAX_INPUT_LUT_ENTRY_NUM], U16 *pu16EntryNum);
void LUT_SortInputLUTEntry(INPUTLutEntry arstLUTEntry[MAX_INPUT_LUT_ENTRY_NUM], U16 u16EntryNum);

wvErrCode LUT_WriteInputLUTEntry(U8 u8LUTIndex, INPUTLutEntry arstLUTEntry[MAX_INPUT_LUT_ENTRY_NUM], U16 u16EntryNum);

wvErrCode LUT_SetInputLUT(TSInfo *pstParamTS);


#endif /* ifndef INCLUDE_LUT_H */
