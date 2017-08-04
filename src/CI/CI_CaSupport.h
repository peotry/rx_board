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
#ifndef __CI_CASUPPORT_H
#define __CI_CASUPPORT_H

#ifdef __cplusplus
extern "C" {
#endif

//! @file CI_CaSupport.h
//! @brief This file contains the core API of the ca support resource

//! This is the resource id of the ca support resource
#define RESOURCEID_CASUPPORT			(0x00030041)
//! This is the ca support resource structure
extern const RESOURCE_INFO CaSupport;

#ifdef ENABLE_AUTOMATIC_PMT_SEND
//! For normal single-service descrambling mode, this buffer contains the last PMT passed into CI_PMTArrived.
//! When a new module is inserted and this buffer has already been filled, the CI library automatically
//! converts this stored PMT into a CAPMT specifically tailored for this CI module.
//! Do not attempt to use this function for multi channel descrambling. The PMTs stored in this
//! buffer are always converted using CA_PMT_LIST_MANAGEMENT_ONLY.
extern BYTE  g_PMTBuf[MAX_LEN_CA_PMT];
//! This is the size of the PMT stored in g_PMTBuf.
extern WORD  g_wPMTLen;
#endif // ENABLE_AUTOMATIC_PMT_SEND


//! This function has to be called by the CI host, when a new PMT arrives
//! in a single-service (=non-PVR) scenario. This function converts the
//! PMT into a EN50221 compatible CA-PMT and sends it to all inserted
//! CI modules. The PMT is also buffered (in g_PMTBuf) and sent to modules which are
//! inserted later. 
//! This function should not be called for every PMT arriving at the decoder but only if
//! the service or the version of the PMT for the current service changes.
//! @param bPMTBuf Pointer to the MPEG2 PMT
//! @param wPMTLen Size of the buffer that PMTBuf points to
void CI_PMTArrived(const BYTE *bPMTBuf, WORD wPMTLen);


//! This internal function is used to send a generated CA-PMT.
//! It can be used to send custom-built CAPMTs for multichannel descrambling.
//! For single channel (normal) descrambling the function CI_PMTArrived should be used instead.
void CaSupport_SendPMT(CI_HANDLE hCI, const BYTE *pData, WORD wLen);

#define TAG_T_CA_INFO_ENQ	(0x9F8030)
#define TAG_T_CA_INFO		(0x9F8031)
#define TAG_T_CA_PMT		(0x9F8032)
#define TAG_T_CA_PMT_REPLY	(0x9F8033)


#ifdef __cplusplus
}
#endif

#endif // __CI_CASUPPORT_H
