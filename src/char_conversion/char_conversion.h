/*
 * =============================================================================
 *
 *       Filename:  char_conversion.h
 *
 *    Description:  字符转换utf8,unicode,gb2312
 *
 *        Version:  1.0
 *        Created:  2017-02-08 15:54:25 
 *       Revision:  1.0
 *
 *         Author:  xubin
 *        Company:  Taichuan
 *
 * =============================================================================
 */
#ifndef _CHAR_CONVERSION_H
#define _CHAR_CONVERSION_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

	int utf8ToUnicodeChar (unsigned char *ch, int *unicode);
	int utf8ToUnicode (unsigned char * utf8_str,
			unsigned short * unicode_str,
			int unicode_str_size);
	int gb2312ToUtf8( char*  ptDestText,
			int  nDestLength,
			char*  ptSrcText,
			int  nSrcLength);

	int utf8ToGb2312(char *gb2312_str,
			int len_gb2312,
			char *utf8_str,
			int len_utf8);

			
	char * ConvertEnc( char *encFrom, char *encTo, const char * in);
	int utf8togb2312(const char *sourcebuf,size_t sourcelen,char *destbuf,size_t destlen);
	int gb2312toutf8(const char *sourcebuf,size_t sourcelen,char *destbuf,size_t destlen);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif
