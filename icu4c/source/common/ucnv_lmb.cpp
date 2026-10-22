// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*  
**********************************************************************
*   Copyright (C) 2000-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*   file name:  ucnv_lmb.cpp
*   encoding:   UTF-8
*   tab size:   4 (not used)
*   indentation:4
*
*   created on: 2000feb09
*   created by: Brendan Murray
*   extensively hacked up by: Jim Snyder-Grant
*
* Modification History:
* 
*   Date        Name             Description
* 
*   06/20/2000  helena           OS/400 port changes; mostly typecast.
*   06/27/2000  Jim Snyder-Grant Deal with partial characters and small buffers.
*                                Add comments to document LMBCS format and implementation
*                                restructured order & breakdown of functions
*   06/28/2000  helena           Major rewrite for the callback API changes.
*/

#ضمن "unicode/utypes.h"

#إذا !UCONFIG_NO_CONVERSION && !UCONFIG_NO_LEGACY_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION

#ضمن "unicode/ucnv_err.h"
#ضمن "unicode/ucnv.h"
#ضمن "unicode/uset.h"
#ضمن "cmemory.h"
#ضمن "cstring.h"
#ضمن "uassert.h"
#ضمن "ucnv_imp.h"
#ضمن "ucnv_bld.h"
#ضمن "ucnv_cnv.h"

#اگر_تعریف_شده EBCDIC_RTL
    #ضمن "ascii_a.h"
#پایان_اگر

/*
  LMBCS

  (Lotus Multi-Byte Character Set)

  LMBCS was invented in the late 1980's and is primarily used in Lotus Notes 
  databases and in Lotus 1-2-3 files. Programmers who work with the APIs 
  into these products will sometimes need to deal with strings in this format.

  The code in this file provides an implementation for an ICU converter of 
  LMBCS to and from Unicode. 

  Since the LMBCS character set is only sparsely documented in existing 
  printed or online material, we have added  extensive annotation to this 
  file to serve as a guide to understanding LMBCS. 

  LMBCS was originally designed with these four sometimes-competing design goals:

  -Provide encodings for the characters in 12 existing national standards
   (plus a few other characters)
  -Minimal memory footprint
  -Maximal speed of conversion into the existing national character sets
  -No need to track a changing state as you interpret a string.


  All of the national character sets LMBCS was trying to encode are 'ANSI'
  based, in that the bytes from 0x20 - 0x7F are almost exactly the 
  same common Latin unaccented characters and symbols in all character sets. 

  So, in order to help meet the speed & memory design goals, the common ANSI 
  bytes from 0x20-0x7F are represented by the same single-byte values in LMBCS. 

  The general LMBCS code unit is from 1-3 bytes. We can describe the 3 bytes as
  follows:

  [G] D1 [D2]

  That is, a sometimes-optional 'group' byte, followed by 1 and sometimes 2
  data bytes. The maximum size of a LMBCS character is 3 bytes:
*/
#تعریف_کن ULMBCS_CHARSIZE_MAX      3
/*
  The single-byte values from 0x20 to 0x7F are examples of single D1 bytes.
  We often have to figure out if byte values are below or above this, so we 
  use the ANSI nomenclature 'C0' and 'C1' to refer to the range of control 
  characters just above & below the common lower-ANSI  range */
#تعریف_کن ULMBCS_C0_انتها           0x1F   
#تعریف_کن ULMBCS_C1_شروع         0x80   
/*
  Since LMBCS is always dealing in byte units. we create a local type here for 
  dealing with these units of LMBCS code units:

*/  
تعريفنوع uint8_t ulmbcs_byte_t;

/* 
   Most of the values less than 0x20 are reserved in LMBCS to announce 
   which national  character standard is being used for the 'D' bytes. 
   In the comments we show the common name and the IBM character-set ID
   for these character-set announcers:
*/

#تعریف_کن ULMBCS_GRP_L1         0x01   /* Latin-1    :ibm-850  */
#تعریف_کن ULMBCS_GRP_GR         0x02   /* Greek      :ibm-851  */
#تعریف_کن ULMBCS_GRP_HE         0x03   /* Hebrew     :ibm-1255 */
#تعریف_کن ULMBCS_GRP_AR         0x04   /* Arabic     :ibm-1256 */
#تعریف_کن ULMBCS_GRP_RU         0x05   /* Cyrillic   :ibm-1251 */
#تعریف_کن ULMBCS_GRP_L2         0x06   /* Latin-2    :ibm-852  */
#تعریف_کن ULMBCS_GRP_TR         0x08   /* Turkish    :ibm-1254 */
#تعریف_کن ULMBCS_GRP_TH         0x0B   /* Thai       :ibm-874  */
#تعریف_کن ULMBCS_GRP_JA         0x10   /* Japanese   :ibm-943  */
#تعریف_کن ULMBCS_GRP_KO         0x11   /* Korean     :ibm-1261 */
#تعریف_کن ULMBCS_GRP_TW         0x12   /* Chinese SC :ibm-950  */
#تعریف_کن ULMBCS_GRP_CN         0x13   /* Chinese TC :ibm-1386 */

/*
   So, the beginning of understanding LMBCS is that IF the first byte of a LMBCS 
   character is one of those 12 values, you can interpret the remaining bytes of 
   that character as coming from one of those character sets. Since the lower 
   ANSI bytes already are represented in single bytes, using one of the character 
   set announcers is used to announce a character that starts with a byte of 
   0x80 or greater.

   The character sets are  arranged so that the single byte sets all appear 
   before the multi-byte character sets. When we need to tell whether a 
   group byte is for a single byte char set or not we use this define: */

#تعریف_کن ULMBCS_DOUBLEOPTGROUP_شروع  0x10   

/* 
However, to fully understand LMBCS, you must also understand a series of 
exceptions & optimizations made in service of the design goals. 

First, those of you who are character set mavens may have noticed that
the 'double-byte' character sets are actually multi-byte character sets 
that can have 1 or two bytes, even in the upper-ascii range. To force
each group byte to introduce a fixed-width encoding (to make it faster to 
count characters), we use a convention of doubling up on the group byte 
to introduce any single-byte character > 0x80 in an otherwise double-byte
character set. So, for example, the LMBCS sequence x10 x10 xAE is the 
same as '0xAE' in the Japanese code page 943.

Next, you will notice that the list of group bytes has some gaps. 
These are used in various ways.

We reserve a few special single byte values for common control 
characters. These are in the same place as their ANSI equivalents for speed.
*/
                     
#تعریف_کن ULMBCS_HT    0x09   /* Fixed control char - Horizontal Tab */
#تعریف_کن ULMBCS_LF    0x0A   /* Fixed control char - Line Feed */
#تعریف_کن ULMBCS_CR    0x0D   /* Fixed control char - Carriage Return */

/* Then, 1-2-3 reserved a special single-byte character to put at the 
beginning of internal 'system' range names: */

#تعریف_کن ULMBCS_123SYSTEMRANGE  0x19   

/* Then we needed a place to put all the other ansi control characters 
that must be moved to different values because LMBCS reserves those 
values for other purposes. To represent the control characters, we start 
with a first byte of 0xF & add the control character value as the 
second byte */
#تعریف_کن ULMBCS_GRP_CTRL       0x0F   

/* For the C0 controls (less than 0x20), we add 0x20 to preserve the 
useful doctrine that any byte less than 0x20 in a LMBCS char must be 
the first byte of a character:*/
#تعریف_کن ULMBCS_CTRLOFFSET      0x20   

/* 
Where to put the characters that aren't part of any of the 12 national 
character sets? The first thing that was done, in the earlier years of 
LMBCS, was to use up the spaces of the form

  [G] D1, 
  
 where  'G' was one of the single-byte character groups, and
 D1 was less than 0x80. These sequences are gathered together 
 into a Lotus-invented doublebyte character set to represent a 
 lot of stray values. Internally, in this implementation, we track this 
 as group '0', as a place to tuck this exceptions list.*/

#تعریف_کن ULMBCS_GRP_EXCEPT     0x00    
/*
 Finally, as the durability and usefulness of UNICODE became clear, 
 LOTUS added a new group 0x14 to hold Unicode values not otherwise 
 represented in LMBCS: */
#تعریف_کن ULMBCS_GRP_UNICODE    0x14   
/* The two bytes appearing after a 0x14 are interpreted as UFT-16 BE
(Big-Endian) characters. The exception comes when the UTF16 
representation would have a zero as the second byte. In that case,
'F6' is used in its place, and the bytes are swapped. (This prevents 
LMBCS from encoding any Unicode values of the form U+F6xx, but that's OK:
0xF6xx is in the middle of the Private Use Area.)*/
#تعریف_کن ULMBCS_UNICOMPATZERO   0xF6   

/* It is also useful in our code to have a constant for the size of 
a LMBCS char that holds a literal Unicode value */
#تعریف_کن ULMBCS_UNICODE_اندازه      3    

/* 
To squish the LMBCS representations down even further, and to make 
translations even faster,sometimes the optimization group byte can be dropped 
from a LMBCS character. This is decided on a process-by-process basis. The 
group byte that is dropped is called the 'optimization group'.

For Notes, the optimzation group is always 0x1.*/
#تعریف_کن ULMBCS_DEFAULTOPTGROUP 0x1    
/* For 1-2-3 files, the optimzation group is stored in the header of the 1-2-3 
file. 

 In any case, when using ICU, you either pass in the 
optimization group as part of the name of the converter (LMBCS-1, LMBCS-2, 
etc.). Using plain 'LMBCS' as the name of the converter will give you 
LMBCS-1.


*** Implementation strategy ***


Because of the extensive use of other character sets, the LMBCS converter
keeps a mapping between optimization groups and IBM character sets, so that
ICU converters can be created and used as needed. */

/* As you can see, even though any byte below 0x20 could be an optimization 
byte, only those at 0x13 or below can map to an actual converter. To limit
some loops and searches, we define a value for that last group converter:*/

#تعریف_کن ULMBCS_GRP_آخرین       0x13   /* last LMBCS group that has a converter */

ساكن ثابت حرف * ثابت Opt_گروه_Byte_مقصد_CPName[ULMBCS_GRP_آخرین + 1] = {
   /* 0x0000 */ "lmb-excp", /* internal home for the LOTUS exceptions list */
   /* 0x0001 */ "ibm-850",
   /* 0x0002 */ "ibm-851",
   /* 0x0003 */ "windows-1255",
   /* 0x0004 */ "windows-1256",
   /* 0x0005 */ "windows-1251",
   /* 0x0006 */ "ibm-852",
   /* 0x0007 */ اشاره_تهی,      /* Unused */
   /* 0x0008 */ "windows-1254",
   /* 0x0009 */ اشاره_تهی,      /* Control char HT */
   /* 0x000A */ اشاره_تهی,      /* Control char LF */
   /* 0x000B */ "windows-874",
   /* 0x000C */ اشاره_تهی,      /* Unused */
   /* 0x000D */ اشاره_تهی,      /* Control char CR */
   /* 0x000E */ اشاره_تهی,      /* Unused */
   /* 0x000F */ اشاره_تهی,      /* Control chars: 0x0F20 + C0/C1 character: algorithmic */
   /* 0x0010 */ "windows-932",
   /* 0x0011 */ "windows-949",
   /* 0x0012 */ "windows-950",
   /* 0x0013 */ "windows-936"

   /* The rest are null, including the 0x0014 Unicode compatibility region
   and 0x0019, the 1-2-3 system range control char */      
};


/* That's approximately all the data that's needed for translating 
  LMBCS to Unicode. 


However, to translate Unicode to LMBCS, we need some more support.

That's because there are often more than one possible mappings from a Unicode
code point back into LMBCS. The first thing we do is look up into a table
to figure out if there are more than one possible mappings. This table,
arranged by Unicode values (including ranges) either lists which group 
to use, or says that it could go into one or more of the SBCS sets, or
into one or more of the DBCS sets.  (If the character exists in both DBCS & 
SBCS, the table will place it in the SBCS sets, to make the LMBCS code point 
length as small as possible. Here's the two special markers we use to indicate
ambiguous mappings: */

#تعریف_کن ULMBCS_AMBIGUOUS_SBCS   0x80   /* could fit in more than one 
                                          LMBCS sbcs native encoding 
                                          (example: most accented latin) */
#تعریف_کن ULMBCS_AMBIGUOUS_MBCS   0x81   /* could fit in more than one 
                                          LMBCS mbcs native encoding 
                                          (example: Unihan) */
#تعریف_کن ULMBCS_AMBIGUOUS_ALL   0x82
/* And here's a simple way to see if a group falls in an appropriate range */
#تعریف_کن ULMBCS_AMBIGUOUS_تطابق(agroup, xgroup) \
                  ((((agroup) == ULMBCS_AMBIGUOUS_SBCS) && \
                  (xgroup) < ULMBCS_DOUBLEOPTGROUP_شروع) || \
                  (((agroup) == ULMBCS_AMBIGUOUS_MBCS) && \
                  (xgroup) >= ULMBCS_DOUBLEOPTGROUP_شروع)) || \
                  ((agroup) == ULMBCS_AMBIGUOUS_ALL)


/* The table & some code to use it: */


ساكن ثابت بنية _UniLMBCSGrpMap  
{
   ثابت char16_t uni_شروع_Range;
   ثابت char16_t uni_انتها_Range;
   ثابت ulmbcs_byte_t  GrpType;
} UniLMBCSGrpMap[]
=
{

    {0x0001, 0x001F,  ULMBCS_GRP_CTRL},
    {0x0080, 0x009F,  ULMBCS_GRP_CTRL},
    {0x00A0, 0x00A6,  ULMBCS_AMBIGUOUS_SBCS},
    {0x00A7, 0x00A8,  ULMBCS_AMBIGUOUS_ALL},
    {0x00A9, 0x00AF,  ULMBCS_AMBIGUOUS_SBCS},
    {0x00B0, 0x00B1,  ULMBCS_AMBIGUOUS_ALL},
    {0x00B2, 0x00B3,  ULMBCS_AMBIGUOUS_SBCS},
    {0x00B4, 0x00B4,  ULMBCS_AMBIGUOUS_ALL},
    {0x00B5, 0x00B5,  ULMBCS_AMBIGUOUS_SBCS},
    {0x00B6, 0x00B6,  ULMBCS_AMBIGUOUS_ALL},
    {0x00B7, 0x00D6,  ULMBCS_AMBIGUOUS_SBCS},
    {0x00D7, 0x00D7,  ULMBCS_AMBIGUOUS_ALL},
    {0x00D8, 0x00F6,  ULMBCS_AMBIGUOUS_SBCS},
    {0x00F7, 0x00F7,  ULMBCS_AMBIGUOUS_ALL},
    {0x00F8, 0x01CD,  ULMBCS_AMBIGUOUS_SBCS},
    {0x01CE, 0x01CE,  ULMBCS_GRP_TW },
    {0x01CF, 0x02B9,  ULMBCS_AMBIGUOUS_SBCS},
    {0x02BA, 0x02BA,  ULMBCS_GRP_CN},
    {0x02BC, 0x02C8,  ULMBCS_AMBIGUOUS_SBCS},
    {0x02C9, 0x02D0,  ULMBCS_AMBIGUOUS_MBCS},
    {0x02D8, 0x02DD,  ULMBCS_AMBIGUOUS_SBCS},
    {0x0384, 0x0390,  ULMBCS_AMBIGUOUS_SBCS},
    {0x0391, 0x03A9,  ULMBCS_AMBIGUOUS_ALL},
    {0x03AA, 0x03B0,  ULMBCS_AMBIGUOUS_SBCS},
    {0x03B1, 0x03C9,  ULMBCS_AMBIGUOUS_ALL},
    {0x03CA, 0x03CE,  ULMBCS_AMBIGUOUS_SBCS},
    {0x0400, 0x0400,  ULMBCS_GRP_RU},
    {0x0401, 0x0401,  ULMBCS_AMBIGUOUS_ALL},
    {0x0402, 0x040F,  ULMBCS_GRP_RU},
    {0x0410, 0x0431,  ULMBCS_AMBIGUOUS_ALL},
    {0x0432, 0x044E,  ULMBCS_GRP_RU},
    {0x044F, 0x044F,  ULMBCS_AMBIGUOUS_ALL},
    {0x0450, 0x0491,  ULMBCS_GRP_RU},
    {0x05B0, 0x05F2,  ULMBCS_GRP_HE},
    {0x060C, 0x06AF,  ULMBCS_GRP_AR},
    {0x0E01, 0x0E5B,  ULMBCS_GRP_TH},
    {0x200C, 0x200F,  ULMBCS_AMBIGUOUS_SBCS},
    {0x2010, 0x2010,  ULMBCS_AMBIGUOUS_MBCS},
    {0x2013, 0x2014,  ULMBCS_AMBIGUOUS_SBCS},
    {0x2015, 0x2015,  ULMBCS_AMBIGUOUS_MBCS},
    {0x2016, 0x2016,  ULMBCS_AMBIGUOUS_MBCS},
    {0x2017, 0x2017,  ULMBCS_AMBIGUOUS_SBCS},
    {0x2018, 0x2019,  ULMBCS_AMBIGUOUS_ALL},
    {0x201A, 0x201B,  ULMBCS_AMBIGUOUS_SBCS},
    {0x201C, 0x201D,  ULMBCS_AMBIGUOUS_ALL},
    {0x201E, 0x201F,  ULMBCS_AMBIGUOUS_SBCS},
    {0x2020, 0x2021,  ULMBCS_AMBIGUOUS_ALL},
    {0x2022, 0x2024,  ULMBCS_AMBIGUOUS_SBCS},
    {0x2025, 0x2025,  ULMBCS_AMBIGUOUS_MBCS},
    {0x2026, 0x2026,  ULMBCS_AMBIGUOUS_ALL},
    {0x2027, 0x2027,  ULMBCS_GRP_TW},
    {0x2030, 0x2030,  ULMBCS_AMBIGUOUS_ALL},
    {0x2031, 0x2031,  ULMBCS_AMBIGUOUS_SBCS},
    {0x2032, 0x2033,  ULMBCS_AMBIGUOUS_MBCS},
    {0x2035, 0x2035,  ULMBCS_AMBIGUOUS_MBCS},
    {0x2039, 0x203A,  ULMBCS_AMBIGUOUS_SBCS},
    {0x203B, 0x203B,  ULMBCS_AMBIGUOUS_MBCS},
    {0x203C, 0x203C,  ULMBCS_GRP_EXCEPT},
    {0x2074, 0x2074,  ULMBCS_GRP_KO},
    {0x207F, 0x207F,  ULMBCS_GRP_EXCEPT},
    {0x2081, 0x2084,  ULMBCS_GRP_KO},
    {0x20A4, 0x20AC,  ULMBCS_AMBIGUOUS_SBCS},
    {0x2103, 0x2109,  ULMBCS_AMBIGUOUS_MBCS},
    {0x2111, 0x2120,  ULMBCS_AMBIGUOUS_SBCS},
    /*zhujin: upgrade, for regressiont test, spr HKIA4YHTSU*/
    {0x2121, 0x2121,  ULMBCS_AMBIGUOUS_MBCS},
    {0x2122, 0x2126,  ULMBCS_AMBIGUOUS_SBCS},
    {0x212B, 0x212B,  ULMBCS_AMBIGUOUS_MBCS},
    {0x2135, 0x2135,  ULMBCS_AMBIGUOUS_SBCS},
    {0x2153, 0x2154,  ULMBCS_GRP_KO},
    {0x215B, 0x215E,  ULMBCS_GRP_EXCEPT},
    {0x2160, 0x2179,  ULMBCS_AMBIGUOUS_MBCS},
    {0x2190, 0x2193,  ULMBCS_AMBIGUOUS_ALL},
    {0x2194, 0x2195,  ULMBCS_GRP_EXCEPT},
    {0x2196, 0x2199,  ULMBCS_AMBIGUOUS_MBCS},
    {0x21A8, 0x21A8,  ULMBCS_GRP_EXCEPT},
    {0x21B8, 0x21B9,  ULMBCS_GRP_CN},
    {0x21D0, 0x21D1,  ULMBCS_GRP_EXCEPT},
    {0x21D2, 0x21D2,  ULMBCS_AMBIGUOUS_MBCS},
    {0x21D3, 0x21D3,  ULMBCS_GRP_EXCEPT},
    {0x21D4, 0x21D4,  ULMBCS_AMBIGUOUS_MBCS},
    {0x21D5, 0x21D5,  ULMBCS_GRP_EXCEPT},
    {0x21E7, 0x21E7,  ULMBCS_GRP_CN},
    {0x2200, 0x2200,  ULMBCS_AMBIGUOUS_MBCS},
    {0x2201, 0x2201,  ULMBCS_GRP_EXCEPT},
    {0x2202, 0x2202,  ULMBCS_AMBIGUOUS_MBCS},
    {0x2203, 0x2203,  ULMBCS_AMBIGUOUS_MBCS},
    {0x2204, 0x2206,  ULMBCS_GRP_EXCEPT},
    {0x2207, 0x2208,  ULMBCS_AMBIGUOUS_MBCS},
    {0x2209, 0x220A,  ULMBCS_GRP_EXCEPT},
    {0x220B, 0x220B,  ULMBCS_AMBIGUOUS_MBCS},
    {0x220F, 0x2215,  ULMBCS_AMBIGUOUS_MBCS},
    {0x2219, 0x2219,  ULMBCS_GRP_EXCEPT},
    {0x221A, 0x221A,  ULMBCS_AMBIGUOUS_MBCS},
    {0x221B, 0x221C,  ULMBCS_GRP_EXCEPT},
    {0x221D, 0x221E,  ULMBCS_AMBIGUOUS_MBCS},
    {0x221F, 0x221F,  ULMBCS_GRP_EXCEPT},
    {0x2220, 0x2220,  ULMBCS_AMBIGUOUS_MBCS},
    {0x2223, 0x222A,  ULMBCS_AMBIGUOUS_MBCS},
    {0x222B, 0x223D,  ULMBCS_AMBIGUOUS_MBCS},
    {0x2245, 0x2248,  ULMBCS_GRP_EXCEPT},
    {0x224C, 0x224C,  ULMBCS_GRP_TW},
    {0x2252, 0x2252,  ULMBCS_AMBIGUOUS_MBCS},
    {0x2260, 0x2261,  ULMBCS_AMBIGUOUS_MBCS},
    {0x2262, 0x2265,  ULMBCS_GRP_EXCEPT},
    {0x2266, 0x226F,  ULMBCS_AMBIGUOUS_MBCS},
    {0x2282, 0x2283,  ULMBCS_AMBIGUOUS_MBCS},
    {0x2284, 0x2285,  ULMBCS_GRP_EXCEPT},
    {0x2286, 0x2287,  ULMBCS_AMBIGUOUS_MBCS},
    {0x2288, 0x2297,  ULMBCS_GRP_EXCEPT},
    {0x2299, 0x22BF,  ULMBCS_AMBIGUOUS_MBCS},
    {0x22C0, 0x22C0,  ULMBCS_GRP_EXCEPT},
    {0x2310, 0x2310,  ULMBCS_GRP_EXCEPT},
    {0x2312, 0x2312,  ULMBCS_AMBIGUOUS_MBCS},
    {0x2318, 0x2321,  ULMBCS_GRP_EXCEPT},
    {0x2318, 0x2321,  ULMBCS_GRP_CN},
    {0x2460, 0x24E9,  ULMBCS_AMBIGUOUS_MBCS},
    {0x2500, 0x2500,  ULMBCS_AMBIGUOUS_SBCS},
    {0x2501, 0x2501,  ULMBCS_AMBIGUOUS_MBCS},
    {0x2502, 0x2502,  ULMBCS_AMBIGUOUS_ALL},
    {0x2503, 0x2503,  ULMBCS_AMBIGUOUS_MBCS},
    {0x2504, 0x2505,  ULMBCS_GRP_TW},
    {0x2506, 0x2665,  ULMBCS_AMBIGUOUS_ALL},
    {0x2666, 0x2666,  ULMBCS_GRP_EXCEPT},
    {0x2667, 0x2669,  ULMBCS_AMBIGUOUS_SBCS},
    {0x266A, 0x266A,  ULMBCS_AMBIGUOUS_ALL},
    {0x266B, 0x266C,  ULMBCS_AMBIGUOUS_SBCS},
    {0x266D, 0x266D,  ULMBCS_AMBIGUOUS_MBCS},
    {0x266E, 0x266E,  ULMBCS_AMBIGUOUS_SBCS},
    {0x266F, 0x266F,  ULMBCS_GRP_JA},
    {0x2670, 0x2E7F,  ULMBCS_AMBIGUOUS_SBCS},
    {0x2E80, 0xF861,  ULMBCS_AMBIGUOUS_MBCS},
    {0xF862, 0xF8FF,  ULMBCS_GRP_EXCEPT},
    {0xF900, 0xFA2D,  ULMBCS_AMBIGUOUS_MBCS},
    {0xFB00, 0xFEFF,  ULMBCS_AMBIGUOUS_SBCS},
    {0xFF01, 0xFFEE,  ULMBCS_AMBIGUOUS_MBCS},
    {0xFFFF, 0xFFFF,  ULMBCS_GRP_UNICODE}
};
   
ساكن ulmbcs_byte_t 
پیدا_کن_LMBCSUni_Range(char16_t uniChar)
{
   ثابت بنية _UniLMBCSGrpMap * p_جدول = UniLMBCSGrpMap;

   طالما (uniChar > p_جدول->uni_انتها_Range) 
   {
      p_جدول++;
   }

   إذا (uniChar >= p_جدول->uni_شروع_Range) 
   {
      أرجع p_جدول->GrpType;
   }
   أرجع ULMBCS_GRP_UNICODE;
}

/* 
We also ask the creator of a converter to send in a preferred locale 
that we can use in resolving ambiguous mappings. They send the locale
in as a string, and we map it, if possible, to one of the 
LMBCS groups. We use this table, and the associated code, to 
do the lookup: */

/**************************************************
  This table maps locale ID's to LMBCS opt groups.
  The default return is group 0x01. Note that for
  performance reasons, the table is sorted in
  increasing alphabetic order, with the notable
  exception of zhTW. This is to force the check
  for Traditonal Chinese before dropping back to
  Simplified.

  Note too that the Latin-1 groups have been
  commented out because it's the default, and
  this shortens the table, allowing a serial
  search to go quickly.
 *************************************************/

ساكن ثابت بنية _LocaleLMBCSGrpMap
{
   ثابت حرف    *LocaleID;
   ثابت ulmbcs_byte_t Opt_گروه;
} LocaleLMBCSGrpMap[] =
{
    {"ar", ULMBCS_GRP_AR},
    {"be", ULMBCS_GRP_RU},
    {"bg", ULMBCS_GRP_L2},
   /* {"ca", ULMBCS_GRP_L1}, */
    {"cs", ULMBCS_GRP_L2},
   /* {"da", ULMBCS_GRP_L1}, */
   /* {"de", ULMBCS_GRP_L1}, */
    {"el", ULMBCS_GRP_GR},
   /* {"en", ULMBCS_GRP_L1}, */
   /* {"es", ULMBCS_GRP_L1}, */
   /* {"et", ULMBCS_GRP_L1}, */
   /* {"fi", ULMBCS_GRP_L1}, */
   /* {"fr", ULMBCS_GRP_L1}, */
    {"he", ULMBCS_GRP_HE},
    {"hu", ULMBCS_GRP_L2},
   /* {"is", ULMBCS_GRP_L1}, */
   /* {"it", ULMBCS_GRP_L1}, */
    {"iw", ULMBCS_GRP_HE},
    {"ja", ULMBCS_GRP_JA},
    {"ko", ULMBCS_GRP_KO},
   /* {"lt", ULMBCS_GRP_L1}, */
   /* {"lv", ULMBCS_GRP_L1}, */
    {"mk", ULMBCS_GRP_RU},
   /* {"nl", ULMBCS_GRP_L1}, */
   /* {"no", ULMBCS_GRP_L1}, */
    {"pl", ULMBCS_GRP_L2},
   /* {"pt", ULMBCS_GRP_L1}, */
    {"ro", ULMBCS_GRP_L2},
    {"ru", ULMBCS_GRP_RU},
    {"sh", ULMBCS_GRP_L2},
    {"sk", ULMBCS_GRP_L2},
    {"sl", ULMBCS_GRP_L2},
    {"sq", ULMBCS_GRP_L2},
    {"sr", ULMBCS_GRP_RU},
   /* {"sv", ULMBCS_GRP_L1}, */
    {"th", ULMBCS_GRP_TH},
    {"tr", ULMBCS_GRP_TR},
    {"uk", ULMBCS_GRP_RU},
   /* {"vi", ULMBCS_GRP_L1}, */
    {"zhTW", ULMBCS_GRP_TW},
    {"zh", ULMBCS_GRP_CN},
    {اشاره_تهی, ULMBCS_GRP_L1}
};


ساكن ulmbcs_byte_t 
پیدا_کن_LMBCSLocale(ثابت حرف *LocaleID)
{
   ثابت بنية _LocaleLMBCSGrpMap *p_جدول = LocaleLMBCSGrpMap;

   إذا ((!LocaleID) || (!*LocaleID)) 
   {
      أرجع 0;
   }

   طالما (p_جدول->LocaleID)
   {
      إذا (*p_جدول->LocaleID == *LocaleID) /* Check only first char for speed */
      {
         /* First char matches - check whole name, for entry-length */
         إذا (uprv_strncmp(p_جدول->LocaleID, LocaleID, طول_رشته(p_جدول->LocaleID)) == 0)
            أرجع p_جدول->Opt_گروه;
      }
      وإلا
      إذا (*p_جدول->LocaleID > *LocaleID) /* Sorted alphabetically - exit */
         اكسر;
      p_جدول++;
   }
   أرجع ULMBCS_GRP_L1;
}


/* 
  Before we get to the main body of code, here's how we hook up to the rest 
  of ICU. ICU converters are required to define a structure that includes 
  some function pointers, and some common data, in the style of a C++
  vtable. There is also room in there for converter-specific data. LMBCS
  uses that converter-specific data to keep track of the 12 subconverters
  we use, the optimization group, and the group (if any) that matches the 
  locale. We have one structure instantiated for each of the 12 possible
  optimization groups. To avoid typos & to avoid boring the reader, we 
  put the declarations of these structures and functions into macros. To see 
  the definitions of these structures, see unicode\ucnv_bld.h
*/

تعريفنوع بنية
  {
    UConverter_Shared_داده *OptGrpConverter[ULMBCS_GRP_آخرین+1];    /* Converter per Opt. grp. */
    uint8_t    Opt_گروه;                  /* default Opt. grp. for this LMBCS session */
    uint8_t    locale_Converter_شاخص;      /* reasonable locale match for index */
  }
UConverter_داده_LMBCS;

U_CDECL_BEGIN
ساكن فراغ  U_CALLCONV _LMBCSClose(UConverter * _this);
U_CDECL_انتها

#تعریف_کن DECLARE_LMBCS_داده(n) \
ساكن ثابت UConverterImpl _LMBCSImpl##n={\
    UCNV_LMBCS_##n,\
    اشاره_تهی,اشاره_تهی,\
    _LMBCSOpen##n,\
    _LMBCSClose,\
    اشاره_تهی,\
    _LMBCSToUnicodeWithOffsets,\
    _LMBCSToUnicodeWithOffsets,\
    _LMBCSFromUnicode,\
    _LMBCSFromUnicode,\
    اشاره_تهی,\
    اشاره_تهی,\
    اشاره_تهی,\
    اشاره_تهی,\
    _LMBCSSafeClone,\
    ucnv_get_Complete_Unicode_مجموعه,\
    اشاره_تهی,\
    اشاره_تهی\
};\
ساكن ثابت UConverter_Static_داده LMBCSStatic_داده##n={\
  حجم(UConverter_Static_داده),\
 "LMBCS-"  #n,\
    0, UCNV_IBM, UCNV_LMBCS_##n, 1, 3,\
    { 0x3f, 0, 0, 0 },1,خطأ,خطأ,0,0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0} \
};\
ثابت UConverter_Shared_داده _LMBCSData##n= \
        UCNV_IMMUTABLE_SHARED_داده_INITIALIZER(&LMBCSStatic_داده##n, &_LMBCSImpl##n);

 /* The only function we needed to duplicate 12 times was the 'open'
function, which will do basically the same thing except set a  different
optimization group. So, we put the common stuff into a worker function, 
and set up another macro to stamp out the 12 open functions:*/
#تعریف_کن DEFINE_LMBCS_OPEN(n) \
ساكن فراغ U_CALLCONV \
   _LMBCSOpen##n(UConverter* _this, UConverter_بارگیری_آرگومانها* p_آرگومانها, UError_کد* err) \
{ LMBCSOpen_کارگر(_this, p_آرگومانها, err, n); }



/* Here's the open worker & the common close function */
ساكن فراغ 
LMBCSOpen_کارگر(UConverter*  _this,
                 UConverter_بارگیری_آرگومانها *p_آرگومانها,
                 UError_کد*  err,
                 ulmbcs_byte_t Opt_گروه)
{
    UConverter_داده_LMBCS* extraInfo = static_cast<UConverter_داده_LMBCS*>(uprv_malloc(حجم(UConverter_داده_LMBCS)));
    _this->extraInfo = extraInfo;
    إذا(extraInfo != اشاره_تهی)
    {
        UConverterNamePieces پشته_Pieces;
        UConverter_بارگیری_آرگومانها پشته_آرگومانها= UCNV_بارگیری_آرگومانها_INITIALIZER;
        ulmbcs_byte_t i;

        uprv_memset(extraInfo, 0, حجم(UConverter_داده_LMBCS));

        پشته_آرگومانها.onlyTestIsLoadable = p_آرگومانها->onlyTestIsLoadable;

        لكل (i=0; i <= ULMBCS_GRP_آخرین && U_SUCCESS(*err); i++)         
        {
            إذا(Opt_گروه_Byte_مقصد_CPName[i] != اشاره_تهی) {
                extraInfo->OptGrpConverter[i] = ucnv_بارگیری_Shared_داده(Opt_گروه_Byte_مقصد_CPName[i], &پشته_Pieces, &پشته_آرگومانها, err);
            }
        }

        إذا(U_FAILURE(*err) || p_آرگومانها->onlyTestIsLoadable) {
            _LMBCSClose(_this);
            أرجع;
        }
        extraInfo->Opt_گروه = Opt_گروه;
        extraInfo->locale_Converter_شاخص = پیدا_کن_LMBCSLocale(p_آرگومانها->locale);
    }
    وإلا
    {
        *err = U_حافظه_تخصیص_ERROR;
    }
}

U_CDECL_BEGIN
ساكن فراغ  U_CALLCONV
_LMBCSClose(UConverter *   _this) 
{
    إذا (_this->extraInfo != اشاره_تهی)
    {
        ulmbcs_byte_t Ix;
        UConverter_داده_LMBCS * extraInfo = (UConverter_داده_LMBCS *) _this->extraInfo;

        لكل (Ix=0; Ix <= ULMBCS_GRP_آخرین; Ix++)
        {
           إذا (extraInfo->OptGrpConverter[Ix] != اشاره_تهی)
              ucnv_unload_Shared_داده_If_Ready(extraInfo->OptGrpConverter[Ix]);
        }
        إذا (!_this->isExtraLocal) {
            uprv_free (_this->extraInfo);
            _this->extraInfo = اشاره_تهی;
        }
    }
}

تعريفنوع بنية LMBCSClone {
    UConverter cnv;
    UConverter_داده_LMBCS lmbcs;
} LMBCSClone;

ساكن UConverter *  U_CALLCONV
_LMBCSSafeClone(ثابت UConverter *cnv, 
                فراغ *پشته_میانگیر, 
                int32_t *p_میانگیر_اندازه, 
                UError_کد *وضعیت) {
    (فراغ)وضعیت;
    LMBCSClone *newLMBCS;
    UConverter_داده_LMBCS *extraInfo;
    int32_t i;

    إذا(*p_میانگیر_اندازه<=0) {
        *p_میانگیر_اندازه=(int32_t)حجم(LMBCSClone);
        أرجع اشاره_تهی;
    }

    extraInfo=(UConverter_داده_LMBCS *)cnv->extraInfo;
    newLMBCS=(LMBCSClone *)پشته_میانگیر;

    /* ucnv.c/ucnv_safeClone() copied the main UConverter already */

    uprv_memcpy(&newLMBCS->lmbcs, extraInfo, حجم(UConverter_داده_LMBCS));

    /* share the subconverters */
    لكل(i = 0; i <= ULMBCS_GRP_آخرین; ++i) {
        إذا(extraInfo->OptGrpConverter[i] != اشاره_تهی) {
            ucnv_increment_Ref_شمار(extraInfo->OptGrpConverter[i]);
        }
    }

    newLMBCS->cnv.extraInfo = &newLMBCS->lmbcs;
    newLMBCS->cnv.isExtraLocal = درست;
    أرجع &newLMBCS->cnv;
}

/*
 * There used to be a _LMBCSGetUnicodeSet() function here (up to svn revision 20117)
 * which added all code points except for U+F6xx
 * because those cannot be represented in the Unicode group.
 * However, it turns out that windows-950 has roundtrips for all of U+F6xx
 * which means that LMBCS can convert all Unicode code points after all.
 * We now simply use ucnv_getCompleteUnicodeSet().
 *
 * This may need to be looked at again as Lotus uses _LMBCSGetUnicodeSet(). (091216)
 */

/* 
   Here's the basic helper function that we use when converting from
   Unicode to LMBCS, and we suspect that a Unicode character will fit into 
   one of the 12 groups. The return value is the number of bytes written 
   starting at pStartLMBCS (if any).
*/

ساكن size_t
LMBCSConversion_کارگر (
   UConverter_داده_LMBCS * extraInfo,    /* subconverters, opt & locale groups */
   ulmbcs_byte_t گروه,                /* The group to try */
   ulmbcs_byte_t  * p_شروع_LMBCS,              /* where to put the results */
   char16_t * pUniChar,                   /* The input unicode character */
   ulmbcs_byte_t * آخرین_Converter_شاخص, /* output: track last successful group used */
   UBool * groups_tried                /* output: track any unsuccessful groups */
)   
{
   ulmbcs_byte_t  * pLMBCS = p_شروع_LMBCS;
   UConverter_Shared_داده * xcnv = extraInfo->OptGrpConverter[گروه];

   صحيح بایتها_Converted;
   uint32_t value;
   ulmbcs_byte_t اولی_Byte;

   U_ASSERT(xcnv);
   U_ASSERT(گروه<ULMBCS_GRP_UNICODE);

   بایتها_Converted = ucnv_MBCSFromUChar32(xcnv, *pUniChar, &value, خطأ);

   /* get the first result byte */
   إذا(بایتها_Converted > 0) {
      اولی_Byte = (ulmbcs_byte_t)(value >> ((بایتها_Converted - 1) * 8));
   } وإلا {
      /* most common failure mode is an unassigned character */
      groups_tried[گروه] = درست;
      أرجع 0;
   }

   *آخرین_Converter_شاخص = گروه;

   /* All initial byte values in lower ascii range should have been caught by now,
      except with the exception group.
    */
   U_ASSERT((اولی_Byte <= ULMBCS_C0_انتها) || (اولی_Byte >= ULMBCS_C1_شروع) || (گروه == ULMBCS_GRP_EXCEPT));
   
   /* use converted data: first write 0, 1 or two group bytes */
   إذا (گروه != ULMBCS_GRP_EXCEPT && extraInfo->Opt_گروه != گروه)
   {
      *pLMBCS++ = گروه;
      إذا (بایتها_Converted == 1 && گروه >= ULMBCS_DOUBLEOPTGROUP_شروع)
      {
         *pLMBCS++ = گروه;
      }
   }

  /* don't emit control chars */
   إذا ( بایتها_Converted == 1 && اولی_Byte < 0x20 )
      أرجع 0;


   /* then move over the converted data */
   بدل(بایتها_Converted)
   {
   حالة 4:
      *pLMBCS++ = (ulmbcs_byte_t)(value >> 24);
      U_FALLTHROUGH;
   حالة 3:
      *pLMBCS++ = (ulmbcs_byte_t)(value >> 16);
      U_FALLTHROUGH;
   حالة 2:
      *pLMBCS++ = (ulmbcs_byte_t)(value >> 8);
      U_FALLTHROUGH;
   حالة 1:
      *pLMBCS++ = (ulmbcs_byte_t)value;
      U_FALLTHROUGH;
   افتراضي:
      /* will never occur */
      اكسر;
   }

   أرجع (pLMBCS - p_شروع_LMBCS);
}


/* This is a much simpler version of above, when we 
know we are writing LMBCS using the Unicode group
*/
ساكن size_t 
LMBCSConvertUni(ulmbcs_byte_t * pLMBCS, char16_t uniChar)
{
     /* encode into LMBCS Unicode range */
   uint8_t کم_Ch =   (uint8_t)(uniChar & 0x00FF);
   uint8_t بالا_Ch  = (uint8_t)(uniChar >> 8);

   *pLMBCS++ = ULMBCS_GRP_UNICODE;

   إذا (کم_Ch == 0)
   {
      *pLMBCS++ = ULMBCS_UNICOMPATZERO;
      *pLMBCS++ = بالا_Ch;
   }
   وإلا
   {
      *pLMBCS++ = بالا_Ch;
      *pLMBCS++ = کم_Ch;
   }
   أرجع ULMBCS_UNICODE_اندازه;
}



/* The main Unicode to LMBCS conversion function */
ساكن فراغ  U_CALLCONV
_LMBCSFromUnicode(UConverter_From_Unicode_آرگومانها*     آرگومانها_2,
                  UError_کد*     err)
{
   ulmbcs_byte_t آخرین_Converter_شاخص = 0;
   char16_t uniChar;
   ulmbcs_byte_t  LMBCS[ULMBCS_CHARSIZE_MAX];
   ulmbcs_byte_t  * pLMBCS;
   int32_t بایتها_written;
   UBool groups_tried[ULMBCS_GRP_آخرین+1];
   UConverter_داده_LMBCS * extraInfo = (UConverter_داده_LMBCS *) آرگومانها_2->converter->extraInfo;
   صحيح منبع_شاخص = 0; 

   /* Basic strategy: attempt to fill in local LMBCS 1-char buffer.(LMBCS)
      If that succeeds, see if it will all fit into the target & copy it over 
      if it does.

      We try conversions in the following order:

      1. Single-byte ascii & special fixed control chars (&null)
      2. Look up group in table & try that (could be 
            A) Unicode group
            B) control group,
            C) national encoding, 
               or ambiguous SBCS or MBCS group (on to step 4...)
        
      3. If its ambiguous, try this order:
         A) The optimization group
         B) The locale group
         C) The last group that succeeded with this string.
         D) every other group that's relevant (single or double)
         E) If its single-byte ambiguous, try the exceptions group

      4. And as a grand fallback: Unicode
   */

    /*Fix for SPR#DJOE66JFN3 (Lotus)*/
    ulmbcs_byte_t Old_Converter_شاخص = 0;

   طالما (آرگومانها_2->منبع < آرگومانها_2->منبع_حد && !U_FAILURE(*err))
   {
      /*Fix for SPR#DJOE66JFN3 (Lotus)*/
      Old_Converter_شاخص = extraInfo->locale_Converter_شاخص;

      إذا (آرگومانها_2->هدف >= آرگومانها_2->هدف_حد)
      {
         *err = U_میانگیر_سرریز_ERROR;
         اكسر;
      }
      uniChar = *(آرگومانها_2->منبع);
      بایتها_written = 0;
      pLMBCS = LMBCS;

      /* check cases in rough order of how common they are, for speed */

      /* single byte matches: strategy 1 */
      /*Fix for SPR#DJOE66JFN3 (Lotus)*/
      إذا((uniChar>=0x80) && (uniChar<=0xff)
      /*Fix for SPR#JUYA6XAERU and TSAO7GL5NK (Lotus)*/ &&(uniChar!=0xB1) &&(uniChar!=0xD7) &&(uniChar!=0xF7)
        &&(uniChar!=0xB0) &&(uniChar!=0xB4) &&(uniChar!=0xB6) &&(uniChar!=0xA7) &&(uniChar!=0xA8))
      {
            extraInfo->locale_Converter_شاخص = ULMBCS_GRP_L1;
      }
      إذا (((uniChar > ULMBCS_C0_انتها) && (uniChar < ULMBCS_C1_شروع)) ||
          uniChar == 0 || uniChar == ULMBCS_HT || uniChar == ULMBCS_CR || 
          uniChar == ULMBCS_LF || uniChar == ULMBCS_123SYSTEMRANGE 
          )
      {
         *pLMBCS++ = (ulmbcs_byte_t ) uniChar;
         بایتها_written = 1;
      }


      إذا (!بایتها_written) 
      {
         /* Check by UNICODE range (Strategy 2) */
         ulmbcs_byte_t گروه = پیدا_کن_LMBCSUni_Range(uniChar);
         
         إذا (گروه == ULMBCS_GRP_UNICODE)  /* (Strategy 2A) */
         {
            pLMBCS += LMBCSConvertUni(pLMBCS,uniChar);
            
            بایتها_written = (int32_t)(pLMBCS - LMBCS);
         }
         وإلا إذا (گروه == ULMBCS_GRP_CTRL)  /* (Strategy 2B) */
         {
            /* Handle control characters here */
            إذا (uniChar <= ULMBCS_C0_انتها)
            {
               *pLMBCS++ = ULMBCS_GRP_CTRL;
               *pLMBCS++ = (ulmbcs_byte_t)(ULMBCS_CTRLOFFSET + uniChar);
            }
            وإلا إذا (uniChar >= ULMBCS_C1_شروع && uniChar <= ULMBCS_C1_شروع + ULMBCS_CTRLOFFSET)
            {
               *pLMBCS++ = ULMBCS_GRP_CTRL;
               *pLMBCS++ = (ulmbcs_byte_t ) (uniChar & 0x00FF);
            }
            بایتها_written = (int32_t)(pLMBCS - LMBCS);
         }
         وإلا إذا (گروه < ULMBCS_GRP_UNICODE)  /* (Strategy 2C) */
         {
            /* a specific converter has been identified - use it */
            بایتها_written = (int32_t)LMBCSConversion_کارگر (
                              extraInfo, گروه, pLMBCS, &uniChar, 
                              &آخرین_Converter_شاخص, groups_tried);
         }
         إذا (!بایتها_written)    /* the ambiguous group cases  (Strategy 3) */
         {
            uprv_memset(groups_tried, 0, حجم(groups_tried));

            /* check for non-default optimization group (Strategy 3A )*/
            إذا ((extraInfo->Opt_گروه != 1) && (ULMBCS_AMBIGUOUS_تطابق(گروه, extraInfo->Opt_گروه)))
            {
                /*zhujin: upgrade, merge #39299 here (Lotus) */
                /*To make R5 compatible translation, look for exceptional group first for non-DBCS*/

                إذا(extraInfo->locale_Converter_شاخص < ULMBCS_DOUBLEOPTGROUP_شروع)
                {
                  بایتها_written = (int32_t)LMBCSConversion_کارگر (extraInfo,
                     ULMBCS_GRP_L1, pLMBCS, &uniChar,
                     &آخرین_Converter_شاخص, groups_tried);

                  إذا(!بایتها_written)
                  {
                     بایتها_written = (int32_t)LMBCSConversion_کارگر (extraInfo,
                         ULMBCS_GRP_EXCEPT, pLMBCS, &uniChar,
                         &آخرین_Converter_شاخص, groups_tried);
                  }
                  إذا(!بایتها_written)
                  {
                      بایتها_written = (int32_t)LMBCSConversion_کارگر (extraInfo,
                          extraInfo->locale_Converter_شاخص, pLMBCS, &uniChar,
                          &آخرین_Converter_شاخص, groups_tried);
                  }
                }
                وإلا
                {
                     بایتها_written = (int32_t)LMBCSConversion_کارگر (extraInfo,
                         extraInfo->locale_Converter_شاخص, pLMBCS, &uniChar,
                         &آخرین_Converter_شاخص, groups_tried);
                }
            }
            /* check for locale optimization group (Strategy 3B) */
            إذا (!بایتها_written && (extraInfo->locale_Converter_شاخص) && (ULMBCS_AMBIGUOUS_تطابق(گروه, extraInfo->locale_Converter_شاخص)))
            {
                بایتها_written = (int32_t)LMBCSConversion_کارگر (extraInfo,
                        extraInfo->locale_Converter_شاخص, pLMBCS, &uniChar, &آخرین_Converter_شاخص, groups_tried);
            }
            /* check for last optimization group used for this string (Strategy 3C) */
            إذا (!بایتها_written && (آخرین_Converter_شاخص) && (ULMBCS_AMBIGUOUS_تطابق(گروه, آخرین_Converter_شاخص)))
            {
                بایتها_written = (int32_t)LMBCSConversion_کارگر (extraInfo,
                        آخرین_Converter_شاخص, pLMBCS, &uniChar, &آخرین_Converter_شاخص, groups_tried);
            }
            إذا (!بایتها_written)
            {
               /* just check every possible matching converter (Strategy 3D) */ 
               ulmbcs_byte_t grp_شروع;
               ulmbcs_byte_t grp_انتها;  
               ulmbcs_byte_t grp_ix;
               grp_شروع = (ulmbcs_byte_t)((گروه == ULMBCS_AMBIGUOUS_MBCS) 
                        ? ULMBCS_DOUBLEOPTGROUP_شروع 
                        :  ULMBCS_GRP_L1);
               grp_انتها = (ulmbcs_byte_t)((گروه == ULMBCS_AMBIGUOUS_MBCS) 
                        ? ULMBCS_GRP_آخرین 
                        :  ULMBCS_GRP_TH);
               إذا(گروه == ULMBCS_AMBIGUOUS_ALL)
               {
                   grp_شروع = ULMBCS_GRP_L1;
                   grp_انتها = ULMBCS_GRP_آخرین;
               }
               لكل (grp_ix = grp_شروع;
                   grp_ix <= grp_انتها && !بایتها_written; 
                    grp_ix++)
               {
                  إذا (extraInfo->OptGrpConverter [grp_ix] && !groups_tried [grp_ix])
                  {
                     بایتها_written = (int32_t)LMBCSConversion_کارگر (extraInfo,
                       grp_ix, pLMBCS, &uniChar, 
                       &آخرین_Converter_شاخص, groups_tried);
                  }
               }
                /* a final conversion fallback to the exceptions group if its likely 
                     to be single byte  (Strategy 3E) */
               إذا (!بایتها_written && grp_شروع == ULMBCS_GRP_L1)
               {
                  بایتها_written = (int32_t)LMBCSConversion_کارگر (extraInfo,
                     ULMBCS_GRP_EXCEPT, pLMBCS, &uniChar, 
                     &آخرین_Converter_شاخص, groups_tried);
               }
            }
            /* all of our other strategies failed. Fallback to Unicode. (Strategy 4)*/
            إذا (!بایتها_written)
            {

               pLMBCS += LMBCSConvertUni(pLMBCS, uniChar);
               بایتها_written = (int32_t)(pLMBCS - LMBCS);
            }
         }
      }
  
      /* we have a translation. increment source and write as much as possible to target */
      آرگومانها_2->منبع++;
      pLMBCS = LMBCS;
      طالما (آرگومانها_2->هدف < آرگومانها_2->هدف_حد && بایتها_written--)
      {
         *(آرگومانها_2->هدف)++ = *pLMBCS++;
         إذا (آرگومانها_2->offsets)
         {
            *(آرگومانها_2->offsets)++ = منبع_شاخص;
         }
      }
      منبع_شاخص++;
      إذا (بایتها_written > 0)
      {
         /* write any bytes that didn't fit in target to the error buffer,
            common code will move this to target if we get called back with
            enough target room
         */
         uint8_t * p_Error_میانگیر = آرگومانها_2->converter->char_Error_میانگیر;
         *err = U_میانگیر_سرریز_ERROR;
         آرگومانها_2->converter->char_Error_میانگیر_درازی = (int8_t)بایتها_written;
         طالما (بایتها_written--)
         {
            *p_Error_میانگیر++ = *pLMBCS++;
         }
      }
      /*Fix for SPR#DJOE66JFN3 (Lotus)*/
      extraInfo->locale_Converter_شاخص = Old_Converter_شاخص;
   }     
}


/* Now, the Unicode from LMBCS section */


/* A function to call when we are looking at the Unicode group byte in LMBCS */
ساكن char16_t
GetUniFromLMBCSUni(حرف ثابت ** ppLMBCSin)  /* Called with LMBCS-style Unicode byte stream */
{
   uint8_t  بالا_Ch = *(*ppLMBCSin)++;  /* Big-endian Unicode in LMBCS compatibility group*/
   uint8_t  کم_Ch  = *(*ppLMBCSin)++;

   إذا (بالا_Ch == ULMBCS_UNICOMPATZERO ) 
   {
      بالا_Ch = کم_Ch;
      کم_Ch = 0; /* zero-byte in LSB special character */
   }
   أرجع (char16_t)((بالا_Ch << 8) | کم_Ch);
}



/* CHECK_SOURCE_LIMIT: Helper macro to verify that there are at least'index' 
   bytes left in source up to  sourceLimit.Errors appropriately if not.
   If we reach the limit, then update the source pointer to there to consume
   all input as required by ICU converter semantics.
*/

#تعریف_کن CHECK_منبع_حد(شاخص) UPRV_بلوک_MACRO_BEGIN { \
    إذا (آرگومانها_2->منبع+شاخص > آرگومانها_2->منبع_حد) { \
        *err = U_TRUNCATED_CHAR_یافت; \
        آرگومانها_2->منبع = آرگومانها_2->منبع_حد; \
        أرجع 0xffff; \
    } \
} UPRV_بلوک_MACRO_انتها

/* Return the Unicode representation for the current LMBCS character */

ساكن UChar32  U_CALLCONV
LMBCSGet_بعدی_UChar_کارگر(UConverter_مقصد_Unicode_آرگومانها*   آرگومانها_2,
                         UError_کد*   err)
{
    UChar32 uniChar = 0;    /* an output UNICODE char */
    ulmbcs_byte_t   CurByte; /* A byte from the input stream */

    /* error check */
    إذا (آرگومانها_2->منبع >= آرگومانها_2->منبع_حد)
    {
        *err = U_ILLEGAL_آرگومان_ERROR;
        أرجع 0xffff;
    }
    /* Grab first byte & save address for error recovery */
    CurByte = *((ulmbcs_byte_t  *) (آرگومانها_2->منبع++));
   
    /*
    * at entry of each if clause:
    * 1. 'CurByte' points at the first byte of a LMBCS character
    * 2. '*source'points to the next byte of the source stream after 'CurByte' 
    *
    * the job of each if clause is:
    * 1. set '*source' to point at the beginning of next char (nop if LMBCS char is only 1 byte)
    * 2. set 'uniChar' up with the right Unicode value, or set 'err' appropriately
    */
   
    /* First lets check the simple fixed values. */

    إذا(((CurByte > ULMBCS_C0_انتها) && (CurByte < ULMBCS_C1_شروع)) /* ascii range */
    ||  (CurByte == 0) 
    ||  CurByte == ULMBCS_HT || CurByte == ULMBCS_CR 
    ||  CurByte == ULMBCS_LF || CurByte == ULMBCS_123SYSTEMRANGE)
    {
        uniChar = CurByte;
    }
    وإلا  
    {
        UConverter_داده_LMBCS * extraInfo;
        ulmbcs_byte_t گروه; 
        UConverter_Shared_داده *cnv; 
        
        إذا (CurByte == ULMBCS_GRP_CTRL)  /* Control character group - no opt group update */
        {
            ulmbcs_byte_t  C0C1byte;
            CHECK_منبع_حد(1);
            C0C1byte = *(آرگومانها_2->منبع)++;
            uniChar = (C0C1byte < ULMBCS_C1_شروع) ? C0C1byte - ULMBCS_CTRLOFFSET : C0C1byte;
        }
        وإلا 
        إذا (CurByte == ULMBCS_GRP_UNICODE) /* Unicode compatibility group: BigEndian UTF16 */
        {
            CHECK_منبع_حد(2);
     
            /* don't check for error indicators fffe/ffff below */
            أرجع GetUniFromLMBCSUni(&(آرگومانها_2->منبع));
        }
        وإلا إذا (CurByte <= ULMBCS_CTRLOFFSET)  
        {
            گروه = CurByte;                   /* group byte is in the source */
            extraInfo = (UConverter_داده_LMBCS *) آرگومانها_2->converter->extraInfo;
            إذا (گروه > ULMBCS_GRP_آخرین || (cnv = extraInfo->OptGrpConverter[گروه]) == اشاره_تهی)
            {
                /* this is not a valid group byte - no converter*/
                *err = U_INVALID_CHAR_یافت;
            }      
            وإلا إذا (گروه >= ULMBCS_DOUBLEOPTGROUP_شروع)    /* double byte conversion */
            {

                CHECK_منبع_حد(2);

                /* check for LMBCS doubled-group-byte case */
                إذا (*آرگومانها_2->منبع == گروه) {
                    /* single byte */
                    ++آرگومانها_2->منبع;
                    uniChar = ucnv_MBCSSimple_Get_بعدی_UChar(cnv, آرگومانها_2->منبع, 1, خطأ);
                    ++آرگومانها_2->منبع;
                } وإلا {
                    /* double byte */
                    uniChar = ucnv_MBCSSimple_Get_بعدی_UChar(cnv, آرگومانها_2->منبع, 2, خطأ);
                    آرگومانها_2->منبع += 2;
                }
            }
            وإلا {                                  /* single byte conversion */
                CHECK_منبع_حد(1);
                CurByte = *(آرگومانها_2->منبع)++;
        
                إذا (CurByte >= ULMBCS_C1_شروع)
                {
                    uniChar = MBCS_SINGLE_SIMPLE_GET_بعدی_BMP(cnv, CurByte);
                }
                وإلا
                {
                    /* The non-optimizable oddballs where there is an explicit byte 
                    * AND the second byte is not in the upper ascii range
                    */
                    حرف بایتها[2];

                    extraInfo = (UConverter_داده_LMBCS *) آرگومانها_2->converter->extraInfo;
                    cnv = extraInfo->OptGrpConverter [ULMBCS_GRP_EXCEPT];  
        
                    /* Lookup value must include opt group */
                    بایتها[0] = گروه;
                    بایتها[1] = CurByte;
                    uniChar = ucnv_MBCSSimple_Get_بعدی_UChar(cnv, بایتها, 2, خطأ);
                }
            }
        }
        وإلا إذا (CurByte >= ULMBCS_C1_شروع) /* group byte is implicit */
        {
            extraInfo = (UConverter_داده_LMBCS *) آرگومانها_2->converter->extraInfo;
            گروه = extraInfo->Opt_گروه;
            cnv = extraInfo->OptGrpConverter[گروه];
            إذا (گروه >= ULMBCS_DOUBLEOPTGROUP_شروع)    /* double byte conversion */
            {
                إذا (!ucnv_MBCSIsLeadByte(cnv, CurByte))
                {
                    CHECK_منبع_حد(0);

                    /* let the MBCS conversion consume CurByte again */
                    uniChar = ucnv_MBCSSimple_Get_بعدی_UChar(cnv, آرگومانها_2->منبع - 1, 1, خطأ);
                }
                وإلا
                {
                    CHECK_منبع_حد(1);
                    /* let the MBCS conversion consume CurByte again */
                    uniChar = ucnv_MBCSSimple_Get_بعدی_UChar(cnv, آرگومانها_2->منبع - 1, 2, خطأ);
                    ++آرگومانها_2->منبع;
                }
            }
            وإلا                                   /* single byte conversion */
            {
                uniChar = MBCS_SINGLE_SIMPLE_GET_بعدی_BMP(cnv, CurByte);
            }
        }
    }
    أرجع uniChar;
}


/* The exported function that converts lmbcs to one or more
   UChars - currently UTF-16
*/
ساكن فراغ  U_CALLCONV
_LMBCSToUnicodeWithOffsets(UConverter_مقصد_Unicode_آرگومانها*    آرگومانها_2,
                     UError_کد*    err)
{
   حرف LMBCS [ULMBCS_CHARSIZE_MAX];
   char16_t uniChar;    /* one output UNICODE char */
   ثابت حرف * save_منبع; /* beginning of current code point */
   ثابت حرف * p_شروع_LMBCS = آرگومانها_2->منبع;  /* beginning of whole string */
   ثابت حرف * err_منبع = اشاره_تهی; /* pointer to actual input in case an error occurs */
   int8_t savebytes = 0;

   /* Process from source to limit, or until error */
   طالما (U_SUCCESS(*err) && آرگومانها_2->منبع_حد > آرگومانها_2->منبع && آرگومانها_2->هدف_حد > آرگومانها_2->هدف)
   {
      save_منبع = آرگومانها_2->منبع; /* beginning of current code point */

      إذا (آرگومانها_2->converter->مقصد_ULength) /* reassemble char from previous call */
      {
        ثابت حرف *save_منبع_حد; 
        size_t اندازه_old = آرگومانها_2->converter->مقصد_ULength;

         /* limit from source is either remainder of temp buffer, or user limit on source */
        size_t اندازه_new_maybe_1 = حجم(LMBCS) - اندازه_old;
        size_t اندازه_new_maybe_2 = آرگومانها_2->منبع_حد - آرگومانها_2->منبع;
        size_t اندازه_new = (اندازه_new_maybe_1 < اندازه_new_maybe_2) ? اندازه_new_maybe_1 : اندازه_new_maybe_2;
         
      
        uprv_memcpy(LMBCS, آرگومانها_2->converter->مقصد_UBytes, اندازه_old);
        uprv_memcpy(LMBCS + اندازه_old, آرگومانها_2->منبع, اندازه_new);
        save_منبع_حد = آرگومانها_2->منبع_حد;
        آرگومانها_2->منبع = err_منبع = LMBCS;
        آرگومانها_2->منبع_حد = LMBCS+اندازه_old+اندازه_new;
        savebytes = (int8_t)(اندازه_old+اندازه_new);
        uniChar = (char16_t) LMBCSGet_بعدی_UChar_کارگر(آرگومانها_2, err);
        آرگومانها_2->منبع = save_منبع + ((آرگومانها_2->منبع - LMBCS) - اندازه_old);
        آرگومانها_2->منبع_حد = save_منبع_حد;

        إذا (*err == U_TRUNCATED_CHAR_یافت)
        {
            /* evil special case: source buffers so small a char spans more than 2 buffers */
            آرگومانها_2->converter->مقصد_ULength = savebytes;
            uprv_memcpy(آرگومانها_2->converter->مقصد_UBytes, LMBCS, savebytes);
            آرگومانها_2->منبع = آرگومانها_2->منبع_حد;
            *err = U_ZERO_ERROR;
            أرجع;
         }
         وإلا
         {
            /* clear the partial-char marker */
            آرگومانها_2->converter->مقصد_ULength = 0;
         }
      }
      وإلا
      {
         err_منبع = save_منبع;
         uniChar = (char16_t) LMBCSGet_بعدی_UChar_کارگر(آرگومانها_2, err);
         savebytes = (int8_t)(آرگومانها_2->منبع - save_منبع);
      }
      إذا (U_SUCCESS(*err))
      {
         إذا (uniChar < 0xfffe)
         {
            *(آرگومانها_2->هدف)++ = uniChar;
            إذا(آرگومانها_2->offsets)
            {
               *(آرگومانها_2->offsets)++ = (int32_t)(save_منبع - p_شروع_LMBCS);
            }
         }
         وإلا إذا (uniChar == 0xfffe)
         {
            *err = U_INVALID_CHAR_یافت;
         }
         وإلا /* if (uniChar == 0xffff) */
         {
            *err = U_ILLEGAL_CHAR_یافت;
         }
      }
   }
   /* if target ran out before source, return U_BUFFER_OVERFLOW_ERROR */
   إذا (U_SUCCESS(*err) && آرگومانها_2->منبع_حد > آرگومانها_2->منبع && آرگومانها_2->هدف_حد <= آرگومانها_2->هدف)
   {
      *err = U_میانگیر_سرریز_ERROR;
   }
   وإلا إذا (U_FAILURE(*err)) 
   {
      /* If character incomplete or unmappable/illegal, store it in toUBytes[] */
      آرگومانها_2->converter->مقصد_ULength = savebytes;
      إذا (savebytes > 0) {
         uprv_memcpy(آرگومانها_2->converter->مقصد_UBytes, err_منبع, savebytes);
      }
      إذا (*err == U_TRUNCATED_CHAR_یافت) {
         *err = U_ZERO_ERROR;
      }
   }
}

/* And now, the macroized declarations of data & functions: */
DEFINE_LMBCS_OPEN(1)
DEFINE_LMBCS_OPEN(2)
DEFINE_LMBCS_OPEN(3)
DEFINE_LMBCS_OPEN(4)
DEFINE_LMBCS_OPEN(5)
DEFINE_LMBCS_OPEN(6)
DEFINE_LMBCS_OPEN(8)
DEFINE_LMBCS_OPEN(11)
DEFINE_LMBCS_OPEN(16)
DEFINE_LMBCS_OPEN(17)
DEFINE_LMBCS_OPEN(18)
DEFINE_LMBCS_OPEN(19)


DECLARE_LMBCS_داده(1)
DECLARE_LMBCS_داده(2)
DECLARE_LMBCS_داده(3)
DECLARE_LMBCS_داده(4)
DECLARE_LMBCS_داده(5)
DECLARE_LMBCS_داده(6)
DECLARE_LMBCS_داده(8)
DECLARE_LMBCS_داده(11)
DECLARE_LMBCS_داده(16)
DECLARE_LMBCS_داده(17)
DECLARE_LMBCS_داده(18)
DECLARE_LMBCS_داده(19)

U_CDECL_انتها

#پایان_اگر /* #if !UCONFIG_NO_LEGACY_CONVERSION */
