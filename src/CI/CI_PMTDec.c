// ********************************************************
//
//   Author/Copyright	Gero Kuehn / GkWare e.K.
//						Humboldtstrasse 177
//						45149 Essen
//						GERMANY 
//						Tel: +49 174 520 8026
//						Email: support@gkware.com
//						Web: http://www.gkware.com
//
//
// ********************************************************
//
//  This file contains the portable CI Lib PMT Decoder
//  
#include "all.h"
#include "CI_PMTDec.h"

#ifdef ENABLE_CI


/******************************************************************************
 * CI_ProInfo_GetDescriptor() - ...
 * DESCRIPTION: - 
 *     从节目描述信息中获取第index个子描述(nTag)回来,返回子描述的指针
 * Input  : pBuf: 节目描述信息  nLoopLen: 节目描述信息长度  
 *          nTag: 描述关键字    index:序号  
 * Output : 
 * Returns: 子描述的指针
 * 
 ******************************************************************************/
const BYTE *CI_ProInfo_GetDescriptor(const BYTE *pBuf, int nLoopLen, const short nTag, const BYTE index)
{
	int hitcount = 0;
	int i = 0;	

    if (pBuf == NULL    || nLoopLen < 0)
    {
        return NULL;
    }

	while(nLoopLen > 0)
	{
		if((pBuf[i]==nTag) || (nTag == -1)) {
			if(hitcount == index)
				return &pBuf[i];
			hitcount++;
		}
		nLoopLen -= (pBuf[i+1]+2);
		i += pBuf[i+1]+2;
	}
	return NULL;
}


/******************************************************************************
 * CI_ESInfo_GetESDescriptor() - ...
 * DESCRIPTION: - 
 *     从PID的ES描述信息中获取第index个子描述(nTag)回来,返回子描述的指针
 * Input  : pDesBuf: 节目描述信息       u16DesLength: 描述信息长度  
 *          nDescriptorTag:描述关键字   bCount:序号  
 * Output : 
 * Returns: 子描述的指针
 * 
 ******************************************************************************/
const BYTE *CI_ESInfo_GetESDescriptor(const BYTE *pDesBuf, U16 u16DesLength, const short nDescriptorTag, const BYTE bCount)
{
	U8*	pTemp  = NULL;
	U8	u8Tag = 0;
	U8	u8Length = 0;
	U16	offset  = 0;
    U8  bHitCount = 0;
    U8  *pDesc = NULL;

    if (pDesBuf == NULL)
    {
        return NULL;
    }

	pTemp = (U8 *)pDesBuf;
	for(offset=0;offset<u16DesLength;  )
	{      
        pDesc = pTemp;
		u8Tag = *pTemp;
		pTemp++;
		u8Length = *pTemp;
		pTemp++;
		pTemp += u8Length;
		offset = offset + u8Length + 2;
		if (u8Tag != nDescriptorTag) 
			continue;

        if(bHitCount==bCount)
        {
			return pDesc;
        }
		else
		{
			bHitCount++;
		}
	}

	return NULL;
}


const BYTE *CI_PMT_GetDescriptor(const BYTE *pBuf, const short nTag, const BYTE index)
{
	WORD wLen = (WORD)(((pBuf[1]&0x0F)<<8)|pBuf[2]);
	int nLoopLen;
	int hitcount = 0;
	int i;
	if(!wLen)
		return NULL;
	

	wLen-=1; // CRC
	nLoopLen = ((pBuf[10]&0x0F)<<8)|pBuf[11];
	i=12;
	while(nLoopLen && (i<wLen))
	{
		if((pBuf[i]==nTag) || (nTag == -1)) {
			if(hitcount == index)
				return &pBuf[i];
			hitcount++;
		}
		nLoopLen -= (pBuf[i+1]+2);
		i += pBuf[i+1]+2;
	}
	return NULL;
}

const BYTE *CI_PMT_GetESDescriptor(const BYTE *pSection, const BYTE bESEntry, const short nDescriptorTag, const BYTE bCount)
{
	WORD wSectionLength = (WORD)(((pSection[1]&0x0F)<<8)|pSection[2]);
	WORD wProgInfoDescrLength;

	int nPos = 10;
	WORD wESLoopCount = 0;
	WORD wESDescrLength;
	int DescrEnd;
	BYTE bHitCount;
	
	if(!wSectionLength)
		return NULL;
	wSectionLength-=1; // CRC

	wProgInfoDescrLength = (WORD)(((pSection[nPos]&0x0F)<<8) | pSection[nPos+1]);
	nPos +=2;	
	
	// skip the program descriptors
	nPos += wProgInfoDescrLength;
	// stream loop
	while(nPos < wSectionLength)
	{
		nPos +=3;
		wESDescrLength = (WORD)(((pSection[nPos]&0x0F)<<8) | pSection[nPos+1]);
		nPos +=2;
		
		DescrEnd = nPos + wESDescrLength;
		bHitCount=0;
		
		if(bESEntry == wESLoopCount)
		{
			while(nPos < DescrEnd) {
				if((pSection[nPos]==nDescriptorTag)||(nDescriptorTag == -1)){
					if((bHitCount==bCount)&&(bESEntry == wESLoopCount))
						return &pSection[nPos];
					else
						bHitCount++;
				}
				nPos+=pSection[nPos+1]+2;
				
			}
		} else
			nPos = DescrEnd;
		
		if(bESEntry == wESLoopCount)
			return NULL;
		wESLoopCount ++;
	}
	return NULL;
}

BOOL CI_PMT_GetESEntry(const BYTE *pSection, const BYTE bESEntry, BYTE *pStreamType, WORD *pwPID)
{
	WORD wSectionLength = (WORD)(((pSection[1]&0x0F)<<8)|pSection[2]);
	WORD wProgInfoDescrLength;

	int nPos = 10;
	WORD wESLoopCount = 0;
	WORD wESDescrLength;
	int DescrEnd;
	BYTE bHitCount;
	
	if(!wSectionLength)
		return FALSE;

	wSectionLength-=1; // CRC

	wProgInfoDescrLength = (WORD)(((pSection[nPos]&0x0F)<<8) | pSection[nPos+1]);
	nPos +=2;	
	
	// skip the program descriptors
	nPos += wProgInfoDescrLength;
	// stream loop
	while(nPos < wSectionLength)
	{
		*pStreamType = pSection[nPos];
		*pwPID = (WORD)(((pSection[nPos+1]<<8)|pSection[nPos+2])&0x1FFF);
		nPos +=3;
		wESDescrLength = (WORD)(((pSection[nPos]&0x0F)<<8) | pSection[nPos+1]);
		nPos +=2;
		
		DescrEnd = nPos + wESDescrLength;
		bHitCount=0;
		
		if(bESEntry == wESLoopCount)
			return TRUE;
		else
			nPos = DescrEnd;
		
		if(bESEntry == wESLoopCount)
			return FALSE;
		wESLoopCount ++;
	}
	return FALSE;
}

#endif // ENABLE_CI
