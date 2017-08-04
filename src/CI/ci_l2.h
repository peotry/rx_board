
#ifndef __CI_L2_H_
#define __CI_L2_H_



void WriteBufferSize(PCMCIA_HANDLE hCI, WORD wBufferSize);

void CI_WriteData(CI_TPDUBUFHANDLE hBuf, WORD wBufferSize, const BYTE *pData, WORD wPMTEndFlag);

BOOL CI_ReallyWriteData(CI_HANDLE hCI,WORD wBufferSize, const BYTE *pData);

void Read_CI_T(CI_HANDLE hCI);

BOOL WaitForCIBit(PCMCIA_HANDLE hCI,BYTE BitMask);

BOOL CI_CheckSlot(PCMCIA_HANDLE hPCMCIA, BYTE bSlotNum);

BOOL CI_CheckConfig(PCMCIA_HANDLE hPCMCIA, PCMCIA_INFO *pInfo, BYTE bSlotNum);

#endif  // __CI_L2_H_