#ifndef __UNICODE_LANGUAGE_H_
#define __UNICODE_LANGUAGE_H_

void Convert_8859_to_U(const unsigned char *From, unsigned short  *to);
int  Convert_U_to_8859_5(unsigned short *pwFrom, int nLen,unsigned char *pucTo);
int  Convert_U_to_GB(unsigned short *pwFrom, int nLen,unsigned char *pucTo);
int Convert_U_to_BIG5(unsigned short *pwFrom, int nLen,unsigned char *pucTo);
int  Unicode2Utf8( char* out, int outLength,const unsigned short * in, int inLength );
int  utf16_to_utf8(unsigned short * u16str, char * u8str);
void Convert_CNCode_to_U(unsigned char *From, unsigned short  *to);
BOOL IsGBSimplifiedChinese(const unsigned char *From);
BOOL ConvertGBToUTF16(const unsigned char *mChar, unsigned short *wChar);
BOOL ConvertBIG5ToUTF16(const unsigned char *mChar, unsigned short *wChar);
BOOL UCS2_to_UTF8(U16* ucs2_code, U8* utf8_code);
BOOL UTF82Unicode(const unsigned char *mChar, unsigned short *wChar);
BOOL IsUnicodeCyrillic(const unsigned short *pu16Src);
BOOL Convert_UTF8_to_U_TaiWan(const unsigned char *mChar, int nLen ,unsigned short *wChar, int * pnLen);
BOOL IsUnicodeTaiwanChinese(const unsigned short *pu16Src);
BOOL IsUnicodeSimplifiedChinese(const unsigned short *pu16Src);

#endif  //__UNICODE_LANGUAGE_H_
