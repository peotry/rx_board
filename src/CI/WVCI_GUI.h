/******************************************************************************
 * WVCI_GUI - main
 * 
 * Copyright 2008-2013 HuiZhou WELLAV Technology Co.,Ltd.
 * 
 * DESCRIPTION: - 
 *      
 * modification history
 * --------------------
 * 02 Jun 2012, shouquan.tang : 执行第一次代码自审操作
 * --------------------
 ******************************************************************************/

#ifndef __WVCI_GUI_H_
#define __WVCI_GUI_H_

#include "all.h"

#ifdef WVCI_USE_PRINT_FOR_GUI
int WVCI_GUIMessageBox(const BYTE *pucInfo);
int WVCI_GUIParseMessage(U8 u8Slot, const BYTE *pucData, int nDataLen);
#endif
// 


#endif //__WVCI_GUI_H_ 



