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

#समावेश "unicode/utypes.h"

#अगर !UCONFIG_NO_CONVERSION && !UCONFIG_NO_LEGACY_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION

#समावेश "unicode/ucnv_err.h"
#समावेश "unicode/ucnv.h"
#समावेश "unicode/uset.h"
#समावेश "cmemory.h"
#समावेश "cstring.h"
#समावेश "uassert.h"
#समावेश "ucnv_imp.h"
#समावेश "ucnv_bld.h"
#समावेश "ucnv_cnv.h"

#अगर_घोषित EBCDIC_RTL
    #समावेश "ascii_a.h"
#पूर्ण_अगर

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
#घोषणा ULMBCS_CHARSIZE_MAX      3
/*
  The single-byte values from 0x20 to 0x7F are examples of single D1 bytes.
  We often have to figure out if byte values are below or above this, so we 
  use the ANSI nomenclature 'C0' and 'C1' to refer to the range of control 
  characters just above & below the common lower-ANSI  range */
#घोषणा ULMBCS_C0END           0x1F   
#घोषणा ULMBCS_C1START         0x80   
/*
  Since LMBCS is always dealing in byte units. we create a local type here for 
  dealing with these units of LMBCS code units:

*/  
प्रकार uint8_t ulmbcs_byte_t;

/* 
   Most of the values less than 0x20 are reserved in LMBCS to announce 
   which national  character standard is being used for the 'D' bytes. 
   In the comments we show the common name and the IBM character-set ID
   for these character-set announcers:
*/

#घोषणा ULMBCS_GRP_L1         0x01   /* Latin-1    :ibm-850  */
#घोषणा ULMBCS_GRP_GR         0x02   /* Greek      :ibm-851  */
#घोषणा ULMBCS_GRP_HE         0x03   /* Hebrew     :ibm-1255 */
#घोषणा ULMBCS_GRP_AR         0x04   /* Arabic     :ibm-1256 */
#घोषणा ULMBCS_GRP_RU         0x05   /* Cyrillic   :ibm-1251 */
#घोषणा ULMBCS_GRP_L2         0x06   /* Latin-2    :ibm-852  */
#घोषणा ULMBCS_GRP_TR         0x08   /* Turkish    :ibm-1254 */
#घोषणा ULMBCS_GRP_TH         0x0B   /* Thai       :ibm-874  */
#घोषणा ULMBCS_GRP_JA         0x10   /* Japanese   :ibm-943  */
#घोषणा ULMBCS_GRP_KO         0x11   /* Korean     :ibm-1261 */
#घोषणा ULMBCS_GRP_TW         0x12   /* Chinese SC :ibm-950  */
#घोषणा ULMBCS_GRP_CN         0x13   /* Chinese TC :ibm-1386 */

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

#घोषणा ULMBCS_DOUBLEOPTGROUP_START  0x10   

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
                     
#घोषणा ULMBCS_HT    0x09   /* Fixed control char - Horizontal Tab */
#घोषणा ULMBCS_LF    0x0A   /* Fixed control char - Line Feed */
#घोषणा ULMBCS_CR    0x0D   /* Fixed control char - Carriage Return */

/* Then, 1-2-3 reserved a special single-byte character to put at the 
beginning of internal 'system' range names: */

#घोषणा ULMBCS_123SYSTEMRANGE  0x19   

/* Then we needed a place to put all the other ansi control characters 
that must be moved to different values because LMBCS reserves those 
values for other purposes. To represent the control characters, we start 
with a first byte of 0xF & add the control character value as the 
second byte */
#घोषणा ULMBCS_GRP_CTRL       0x0F   

/* For the C0 controls (less than 0x20), we add 0x20 to preserve the 
useful doctrine that any byte less than 0x20 in a LMBCS char must be 
the first byte of a character:*/
#घोषणा ULMBCS_CTRLOFFSET      0x20   

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

#घोषणा ULMBCS_GRP_EXCEPT     0x00    
/*
 Finally, as the durability and usefulness of UNICODE became clear, 
 LOTUS added a new group 0x14 to hold Unicode values not otherwise 
 represented in LMBCS: */
#घोषणा ULMBCS_GRP_UNICODE    0x14   
/* The two bytes appearing after a 0x14 are interpreted as UFT-16 BE
(Big-Endian) characters. The exception comes when the UTF16 
representation would have a zero as the second byte. In that case,
'F6' is used in its place, and the bytes are swapped. (This prevents 
LMBCS from encoding any Unicode values of the form U+F6xx, but that's OK:
0xF6xx is in the middle of the Private Use Area.)*/
#घोषणा ULMBCS_UNICOMPATZERO   0xF6   

/* It is also useful in our code to have a constant for the size of 
a LMBCS char that holds a literal Unicode value */
#घोषणा ULMBCS_UNICODE_आकार      3    

/* 
To squish the LMBCS representations down even further, and to make 
translations even faster,sometimes the optimization group byte can be dropped 
from a LMBCS character. This is decided on a process-by-process basis. The 
group byte that is dropped is called the 'optimization group'.

For Notes, the optimzation group is always 0x1.*/
#घोषणा ULMBCS_DEFAULTOPTGROUP 0x1    
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

#घोषणा ULMBCS_GRP_LAST       0x13   /* last LMBCS group that has a converter */

अटल स्थिर अक्षर * स्थिर OptGroupByteToCPName[ULMBCS_GRP_LAST + 1] = {
   /* 0x0000 */ "lmb-excp", /* internal home for the LOTUS exceptions list */
   /* 0x0001 */ "ibm-850",
   /* 0x0002 */ "ibm-851",
   /* 0x0003 */ "windows-1255",
   /* 0x0004 */ "windows-1256",
   /* 0x0005 */ "windows-1251",
   /* 0x0006 */ "ibm-852",
   /* 0x0007 */ nullptr,      /* Unused */
   /* 0x0008 */ "windows-1254",
   /* 0x0009 */ nullptr,      /* Control char HT */
   /* 0x000A */ nullptr,      /* Control char LF */
   /* 0x000B */ "windows-874",
   /* 0x000C */ nullptr,      /* Unused */
   /* 0x000D */ nullptr,      /* Control char CR */
   /* 0x000E */ nullptr,      /* Unused */
   /* 0x000F */ nullptr,      /* Control chars: 0x0F20 + C0/C1 character: algorithmic */
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

#घोषणा ULMBCS_AMBIGUOUS_SBCS   0x80   /* could fit in more than one 
                                          LMBCS sbcs native encoding 
                                          (example: most accented latin) */
#घोषणा ULMBCS_AMBIGUOUS_MBCS   0x81   /* could fit in more than one 
                                          LMBCS mbcs native encoding 
                                          (example: Unihan) */
#घोषणा ULMBCS_AMBIGUOUS_ALL   0x82
/* And here's a simple way to see if a group falls in an appropriate range */
#घोषणा ULMBCS_AMBIGUOUS_MATCH(agroup, xgroup) \
                  ((((agroup) == ULMBCS_AMBIGUOUS_SBCS) && \
                  (xgroup) < ULMBCS_DOUBLEOPTGROUP_START) || \
                  (((agroup) == ULMBCS_AMBIGUOUS_MBCS) && \
                  (xgroup) >= ULMBCS_DOUBLEOPTGROUP_START)) || \
                  ((agroup) == ULMBCS_AMBIGUOUS_ALL)


/* The table & some code to use it: */


अटल स्थिर काष्ठा _UniLMBCSGrpMap  
{
   स्थिर char16_t uniStartRange;
   स्थिर char16_t uniEndRange;
   स्थिर ulmbcs_byte_t  GrpType;
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
   
अटल ulmbcs_byte_t 
FindLMBCSUniRange(char16_t uniChar)
{
   स्थिर काष्ठा _UniLMBCSGrpMap * pTable = UniLMBCSGrpMap;

   जबतक (uniChar > pTable->uniEndRange) 
   {
      pTable++;
   }

   अगर (uniChar >= pTable->uniStartRange) 
   {
      वापस pTable->GrpType;
   }
   वापस ULMBCS_GRP_UNICODE;
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

अटल स्थिर काष्ठा _LocaleLMBCSGrpMap
{
   स्थिर अक्षर    *LocaleID;
   स्थिर ulmbcs_byte_t OptGroup;
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
    {nullptr, ULMBCS_GRP_L1}
};


अटल ulmbcs_byte_t 
FindLMBCSLocale(स्थिर अक्षर *LocaleID)
{
   स्थिर काष्ठा _LocaleLMBCSGrpMap *pTable = LocaleLMBCSGrpMap;

   अगर ((!LocaleID) || (!*LocaleID)) 
   {
      वापस 0;
   }

   जबतक (pTable->LocaleID)
   {
      अगर (*pTable->LocaleID == *LocaleID) /* Check only first char for speed */
      {
         /* First char matches - check whole name, for entry-length */
         अगर (uprv_strncmp(pTable->LocaleID, LocaleID, म_माप(pTable->LocaleID)) == 0)
            वापस pTable->OptGroup;
      }
      अन्यथा
      अगर (*pTable->LocaleID > *LocaleID) /* Sorted alphabetically - exit */
         अवरोध;
      pTable++;
   }
   वापस ULMBCS_GRP_L1;
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

प्रकार काष्ठा
  {
    UConverter_Shared_तथ्य *OptGrpConverter[ULMBCS_GRP_LAST+1];    /* Converter per Opt. grp. */
    uint8_t    OptGroup;                  /* default Opt. grp. for this LMBCS session */
    uint8_t    locale_Converter_सूचकांक;      /* reasonable locale match for index */
  }
UConverter_तथ्य_LMBCS;

U_CDECL_BEGIN
अटल व्योम  U_CALLCONV _LMBCSClose(UConverter * _this);
U_CDECL_END

#घोषणा DECLARE_LMBCS_तथ्य(n) \
अटल स्थिर UConverterImpl _LMBCSImpl##n={\
    UCNV_LMBCS_##n,\
    nullptr,nullptr,\
    _LMBCSOpen##n,\
    _LMBCSClose,\
    nullptr,\
    _LMBCSToUnicodeWithOffsets,\
    _LMBCSToUnicodeWithOffsets,\
    _LMBCSFromUnicode,\
    _LMBCSFromUnicode,\
    nullptr,\
    nullptr,\
    nullptr,\
    nullptr,\
    _LMBCSSafeClone,\
    ucnv_getCompleteUnicodeSet,\
    nullptr,\
    nullptr\
};\
अटल स्थिर UConverter_Static_तथ्य LMBCSStatic_तथ्य##n={\
  माप(UConverter_Static_तथ्य),\
 "LMBCS-"  #n,\
    0, UCNV_IBM, UCNV_LMBCS_##n, 1, 3,\
    { 0x3f, 0, 0, 0 },1,false,false,0,0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0} \
};\
स्थिर UConverter_Shared_तथ्य _LMBCSData##n= \
        UCNV_IMMUTABLE_SHARED_तथ्य_INITIALIZER(&LMBCSStatic_तथ्य##n, &_LMBCSImpl##n);

 /* The only function we needed to duplicate 12 times was the 'open'
function, which will do basically the same thing except set a  different
optimization group. So, we put the common stuff into a worker function, 
and set up another macro to stamp out the 12 open functions:*/
#घोषणा DEFINE_LMBCS_OPEN(n) \
अटल व्योम U_CALLCONV \
   _LMBCSOpen##n(UConverter* _this, UConverterLoadArgs* pArgs, UErrorCode* err) \
{ _LMBCSOpenWorker(_this, pArgs, err, n); }



/* Here's the open worker & the common close function */
अटल व्योम 
_LMBCSOpenWorker(UConverter*  _this,
                 UConverterLoadArgs *pArgs,
                 UErrorCode*  err,
                 ulmbcs_byte_t OptGroup)
{
    UConverter_तथ्य_LMBCS* extraInfo = static_cast<UConverter_तथ्य_LMBCS*>(uprv_malloc(माप(UConverter_तथ्य_LMBCS)));
    _this->extraInfo = extraInfo;
    अगर(extraInfo != nullptr)
    {
        UConverterNamePieces ढेर_Pieces;
        UConverterLoadArgs ढेर_Args= UCNV_LOAD_ARGS_INITIALIZER;
        ulmbcs_byte_t i;

        uprv_memset(extraInfo, 0, माप(UConverter_तथ्य_LMBCS));

        ढेर_Args.onlyTestIsLoadable = pArgs->onlyTestIsLoadable;

        क्रम (i=0; i <= ULMBCS_GRP_LAST && U_SUCCESS(*err); i++)         
        {
            अगर(OptGroupByteToCPName[i] != nullptr) {
                extraInfo->OptGrpConverter[i] = ucnv_load_Shared_तथ्य(OptGroupByteToCPName[i], &ढेर_Pieces, &ढेर_Args, err);
            }
        }

        अगर(U_FAILURE(*err) || pArgs->onlyTestIsLoadable) {
            _LMBCSClose(_this);
            वापस;
        }
        extraInfo->OptGroup = OptGroup;
        extraInfo->locale_Converter_सूचकांक = FindLMBCSLocale(pArgs->locale);
    }
    अन्यथा
    {
        *err = U_MEMORY_ALLOCATION_ERROR;
    }
}

U_CDECL_BEGIN
अटल व्योम  U_CALLCONV
_LMBCSClose(UConverter *   _this) 
{
    अगर (_this->extraInfo != nullptr)
    {
        ulmbcs_byte_t Ix;
        UConverter_तथ्य_LMBCS * extraInfo = (UConverter_तथ्य_LMBCS *) _this->extraInfo;

        क्रम (Ix=0; Ix <= ULMBCS_GRP_LAST; Ix++)
        {
           अगर (extraInfo->OptGrpConverter[Ix] != nullptr)
              ucnv_unload_Shared_तथ्य_If_Ready(extraInfo->OptGrpConverter[Ix]);
        }
        अगर (!_this->isExtraLocal) {
            uprv_free (_this->extraInfo);
            _this->extraInfo = nullptr;
        }
    }
}

प्रकार काष्ठा LMBCSClone {
    UConverter cnv;
    UConverter_तथ्य_LMBCS lmbcs;
} LMBCSClone;

अटल UConverter *  U_CALLCONV
_LMBCSSafeClone(स्थिर UConverter *cnv, 
                व्योम *ढेर_बफर, 
                int32_t *p_बफर_आकार, 
                UErrorCode *status) {
    (व्योम)status;
    LMBCSClone *newLMBCS;
    UConverter_तथ्य_LMBCS *extraInfo;
    int32_t i;

    अगर(*p_बफर_आकार<=0) {
        *p_बफर_आकार=(int32_t)माप(LMBCSClone);
        वापस nullptr;
    }

    extraInfo=(UConverter_तथ्य_LMBCS *)cnv->extraInfo;
    newLMBCS=(LMBCSClone *)ढेर_बफर;

    /* ucnv.c/ucnv_safeClone() copied the main UConverter already */

    uprv_memcpy(&newLMBCS->lmbcs, extraInfo, माप(UConverter_तथ्य_LMBCS));

    /* share the subconverters */
    क्रम(i = 0; i <= ULMBCS_GRP_LAST; ++i) {
        अगर(extraInfo->OptGrpConverter[i] != nullptr) {
            ucnv_increment_Ref_गिनती(extraInfo->OptGrpConverter[i]);
        }
    }

    newLMBCS->cnv.extraInfo = &newLMBCS->lmbcs;
    newLMBCS->cnv.isExtraLocal = true;
    वापस &newLMBCS->cnv;
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

अटल माप_प्रकार
LMBCSConversionWorker (
   UConverter_तथ्य_LMBCS * extraInfo,    /* subconverters, opt & locale groups */
   ulmbcs_byte_t group,                /* The group to try */
   ulmbcs_byte_t  * pStartLMBCS,              /* where to put the results */
   char16_t * pUniChar,                   /* The input unicode character */
   ulmbcs_byte_t * last_Converter_सूचकांक, /* output: track last successful group used */
   UBool * groups_tried                /* output: track any unsuccessful groups */
)   
{
   ulmbcs_byte_t  * pLMBCS = pStartLMBCS;
   UConverter_Shared_तथ्य * xcnv = extraInfo->OptGrpConverter[group];

   पूर्णांक bytesConverted;
   uint32_t value;
   ulmbcs_byte_t firstByte;

   U_ASSERT(xcnv);
   U_ASSERT(group<ULMBCS_GRP_UNICODE);

   bytesConverted = ucnv_MBCSFromUChar32(xcnv, *pUniChar, &value, false);

   /* get the first result byte */
   अगर(bytesConverted > 0) {
      firstByte = (ulmbcs_byte_t)(value >> ((bytesConverted - 1) * 8));
   } अन्यथा {
      /* most common failure mode is an unassigned character */
      groups_tried[group] = true;
      वापस 0;
   }

   *last_Converter_सूचकांक = group;

   /* All initial byte values in lower ascii range should have been caught by now,
      except with the exception group.
    */
   U_ASSERT((firstByte <= ULMBCS_C0END) || (firstByte >= ULMBCS_C1START) || (group == ULMBCS_GRP_EXCEPT));
   
   /* use converted data: first write 0, 1 or two group bytes */
   अगर (group != ULMBCS_GRP_EXCEPT && extraInfo->OptGroup != group)
   {
      *pLMBCS++ = group;
      अगर (bytesConverted == 1 && group >= ULMBCS_DOUBLEOPTGROUP_START)
      {
         *pLMBCS++ = group;
      }
   }

  /* don't emit control chars */
   अगर ( bytesConverted == 1 && firstByte < 0x20 )
      वापस 0;


   /* then move over the converted data */
   चयन(bytesConverted)
   {
   हाल 4:
      *pLMBCS++ = (ulmbcs_byte_t)(value >> 24);
      U_FALLTHROUGH;
   हाल 3:
      *pLMBCS++ = (ulmbcs_byte_t)(value >> 16);
      U_FALLTHROUGH;
   हाल 2:
      *pLMBCS++ = (ulmbcs_byte_t)(value >> 8);
      U_FALLTHROUGH;
   हाल 1:
      *pLMBCS++ = (ulmbcs_byte_t)value;
      U_FALLTHROUGH;
   शेष:
      /* will never occur */
      अवरोध;
   }

   वापस (pLMBCS - pStartLMBCS);
}


/* This is a much simpler version of above, when we 
know we are writing LMBCS using the Unicode group
*/
अटल माप_प्रकार 
LMBCSConvertUni(ulmbcs_byte_t * pLMBCS, char16_t uniChar)
{
     /* encode into LMBCS Unicode range */
   uint8_t निचला_Ch =   (uint8_t)(uniChar & 0x00FF);
   uint8_t ऊपरी_Ch  = (uint8_t)(uniChar >> 8);

   *pLMBCS++ = ULMBCS_GRP_UNICODE;

   अगर (निचला_Ch == 0)
   {
      *pLMBCS++ = ULMBCS_UNICOMPATZERO;
      *pLMBCS++ = ऊपरी_Ch;
   }
   अन्यथा
   {
      *pLMBCS++ = ऊपरी_Ch;
      *pLMBCS++ = निचला_Ch;
   }
   वापस ULMBCS_UNICODE_आकार;
}



/* The main Unicode to LMBCS conversion function */
अटल व्योम  U_CALLCONV
_LMBCSFromUnicode(UConverterFromUnicodeArgs*     args,
                  UErrorCode*     err)
{
   ulmbcs_byte_t last_Converter_सूचकांक = 0;
   char16_t uniChar;
   ulmbcs_byte_t  LMBCS[ULMBCS_CHARSIZE_MAX];
   ulmbcs_byte_t  * pLMBCS;
   int32_t bytes_written;
   UBool groups_tried[ULMBCS_GRP_LAST+1];
   UConverter_तथ्य_LMBCS * extraInfo = (UConverter_तथ्य_LMBCS *) args->converter->extraInfo;
   पूर्णांक source_सूचकांक = 0; 

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
    ulmbcs_byte_t Old_Converter_सूचकांक = 0;

   जबतक (args->source < args->sourceLimit && !U_FAILURE(*err))
   {
      /*Fix for SPR#DJOE66JFN3 (Lotus)*/
      Old_Converter_सूचकांक = extraInfo->locale_Converter_सूचकांक;

      अगर (args->target >= args->targetLimit)
      {
         *err = U_बफर_OVERFLOW_ERROR;
         अवरोध;
      }
      uniChar = *(args->source);
      bytes_written = 0;
      pLMBCS = LMBCS;

      /* check cases in rough order of how common they are, for speed */

      /* single byte matches: strategy 1 */
      /*Fix for SPR#DJOE66JFN3 (Lotus)*/
      अगर((uniChar>=0x80) && (uniChar<=0xff)
      /*Fix for SPR#JUYA6XAERU and TSAO7GL5NK (Lotus)*/ &&(uniChar!=0xB1) &&(uniChar!=0xD7) &&(uniChar!=0xF7)
        &&(uniChar!=0xB0) &&(uniChar!=0xB4) &&(uniChar!=0xB6) &&(uniChar!=0xA7) &&(uniChar!=0xA8))
      {
            extraInfo->locale_Converter_सूचकांक = ULMBCS_GRP_L1;
      }
      अगर (((uniChar > ULMBCS_C0END) && (uniChar < ULMBCS_C1START)) ||
          uniChar == 0 || uniChar == ULMBCS_HT || uniChar == ULMBCS_CR || 
          uniChar == ULMBCS_LF || uniChar == ULMBCS_123SYSTEMRANGE 
          )
      {
         *pLMBCS++ = (ulmbcs_byte_t ) uniChar;
         bytes_written = 1;
      }


      अगर (!bytes_written) 
      {
         /* Check by UNICODE range (Strategy 2) */
         ulmbcs_byte_t group = FindLMBCSUniRange(uniChar);
         
         अगर (group == ULMBCS_GRP_UNICODE)  /* (Strategy 2A) */
         {
            pLMBCS += LMBCSConvertUni(pLMBCS,uniChar);
            
            bytes_written = (int32_t)(pLMBCS - LMBCS);
         }
         अन्यथा अगर (group == ULMBCS_GRP_CTRL)  /* (Strategy 2B) */
         {
            /* Handle control characters here */
            अगर (uniChar <= ULMBCS_C0END)
            {
               *pLMBCS++ = ULMBCS_GRP_CTRL;
               *pLMBCS++ = (ulmbcs_byte_t)(ULMBCS_CTRLOFFSET + uniChar);
            }
            अन्यथा अगर (uniChar >= ULMBCS_C1START && uniChar <= ULMBCS_C1START + ULMBCS_CTRLOFFSET)
            {
               *pLMBCS++ = ULMBCS_GRP_CTRL;
               *pLMBCS++ = (ulmbcs_byte_t ) (uniChar & 0x00FF);
            }
            bytes_written = (int32_t)(pLMBCS - LMBCS);
         }
         अन्यथा अगर (group < ULMBCS_GRP_UNICODE)  /* (Strategy 2C) */
         {
            /* a specific converter has been identified - use it */
            bytes_written = (int32_t)LMBCSConversionWorker (
                              extraInfo, group, pLMBCS, &uniChar, 
                              &last_Converter_सूचकांक, groups_tried);
         }
         अगर (!bytes_written)    /* the ambiguous group cases  (Strategy 3) */
         {
            uprv_memset(groups_tried, 0, माप(groups_tried));

            /* check for non-default optimization group (Strategy 3A )*/
            अगर ((extraInfo->OptGroup != 1) && (ULMBCS_AMBIGUOUS_MATCH(group, extraInfo->OptGroup)))
            {
                /*zhujin: upgrade, merge #39299 here (Lotus) */
                /*To make R5 compatible translation, look for exceptional group first for non-DBCS*/

                अगर(extraInfo->locale_Converter_सूचकांक < ULMBCS_DOUBLEOPTGROUP_START)
                {
                  bytes_written = (int32_t)LMBCSConversionWorker (extraInfo,
                     ULMBCS_GRP_L1, pLMBCS, &uniChar,
                     &last_Converter_सूचकांक, groups_tried);

                  अगर(!bytes_written)
                  {
                     bytes_written = (int32_t)LMBCSConversionWorker (extraInfo,
                         ULMBCS_GRP_EXCEPT, pLMBCS, &uniChar,
                         &last_Converter_सूचकांक, groups_tried);
                  }
                  अगर(!bytes_written)
                  {
                      bytes_written = (int32_t)LMBCSConversionWorker (extraInfo,
                          extraInfo->locale_Converter_सूचकांक, pLMBCS, &uniChar,
                          &last_Converter_सूचकांक, groups_tried);
                  }
                }
                अन्यथा
                {
                     bytes_written = (int32_t)LMBCSConversionWorker (extraInfo,
                         extraInfo->locale_Converter_सूचकांक, pLMBCS, &uniChar,
                         &last_Converter_सूचकांक, groups_tried);
                }
            }
            /* check for locale optimization group (Strategy 3B) */
            अगर (!bytes_written && (extraInfo->locale_Converter_सूचकांक) && (ULMBCS_AMBIGUOUS_MATCH(group, extraInfo->locale_Converter_सूचकांक)))
            {
                bytes_written = (int32_t)LMBCSConversionWorker (extraInfo,
                        extraInfo->locale_Converter_सूचकांक, pLMBCS, &uniChar, &last_Converter_सूचकांक, groups_tried);
            }
            /* check for last optimization group used for this string (Strategy 3C) */
            अगर (!bytes_written && (last_Converter_सूचकांक) && (ULMBCS_AMBIGUOUS_MATCH(group, last_Converter_सूचकांक)))
            {
                bytes_written = (int32_t)LMBCSConversionWorker (extraInfo,
                        last_Converter_सूचकांक, pLMBCS, &uniChar, &last_Converter_सूचकांक, groups_tried);
            }
            अगर (!bytes_written)
            {
               /* just check every possible matching converter (Strategy 3D) */ 
               ulmbcs_byte_t grp_start;
               ulmbcs_byte_t grp_end;  
               ulmbcs_byte_t grp_ix;
               grp_start = (ulmbcs_byte_t)((group == ULMBCS_AMBIGUOUS_MBCS) 
                        ? ULMBCS_DOUBLEOPTGROUP_START 
                        :  ULMBCS_GRP_L1);
               grp_end = (ulmbcs_byte_t)((group == ULMBCS_AMBIGUOUS_MBCS) 
                        ? ULMBCS_GRP_LAST 
                        :  ULMBCS_GRP_TH);
               अगर(group == ULMBCS_AMBIGUOUS_ALL)
               {
                   grp_start = ULMBCS_GRP_L1;
                   grp_end = ULMBCS_GRP_LAST;
               }
               क्रम (grp_ix = grp_start;
                   grp_ix <= grp_end && !bytes_written; 
                    grp_ix++)
               {
                  अगर (extraInfo->OptGrpConverter [grp_ix] && !groups_tried [grp_ix])
                  {
                     bytes_written = (int32_t)LMBCSConversionWorker (extraInfo,
                       grp_ix, pLMBCS, &uniChar, 
                       &last_Converter_सूचकांक, groups_tried);
                  }
               }
                /* a final conversion fallback to the exceptions group if its likely 
                     to be single byte  (Strategy 3E) */
               अगर (!bytes_written && grp_start == ULMBCS_GRP_L1)
               {
                  bytes_written = (int32_t)LMBCSConversionWorker (extraInfo,
                     ULMBCS_GRP_EXCEPT, pLMBCS, &uniChar, 
                     &last_Converter_सूचकांक, groups_tried);
               }
            }
            /* all of our other strategies failed. Fallback to Unicode. (Strategy 4)*/
            अगर (!bytes_written)
            {

               pLMBCS += LMBCSConvertUni(pLMBCS, uniChar);
               bytes_written = (int32_t)(pLMBCS - LMBCS);
            }
         }
      }
  
      /* we have a translation. increment source and write as much as possible to target */
      args->source++;
      pLMBCS = LMBCS;
      जबतक (args->target < args->targetLimit && bytes_written--)
      {
         *(args->target)++ = *pLMBCS++;
         अगर (args->offsets)
         {
            *(args->offsets)++ = source_सूचकांक;
         }
      }
      source_सूचकांक++;
      अगर (bytes_written > 0)
      {
         /* write any bytes that didn't fit in target to the error buffer,
            common code will move this to target if we get called back with
            enough target room
         */
         uint8_t * p_Error_बफर = args->converter->char_Error_बफर;
         *err = U_बफर_OVERFLOW_ERROR;
         args->converter->char_Error_बफर_लंबाई = (int8_t)bytes_written;
         जबतक (bytes_written--)
         {
            *p_Error_बफर++ = *pLMBCS++;
         }
      }
      /*Fix for SPR#DJOE66JFN3 (Lotus)*/
      extraInfo->locale_Converter_सूचकांक = Old_Converter_सूचकांक;
   }     
}


/* Now, the Unicode from LMBCS section */


/* A function to call when we are looking at the Unicode group byte in LMBCS */
अटल char16_t
GetUniFromLMBCSUni(अक्षर स्थिर ** ppLMBCSin)  /* Called with LMBCS-style Unicode byte stream */
{
   uint8_t  ऊपरी_Ch = *(*ppLMBCSin)++;  /* Big-endian Unicode in LMBCS compatibility group*/
   uint8_t  निचला_Ch  = *(*ppLMBCSin)++;

   अगर (ऊपरी_Ch == ULMBCS_UNICOMPATZERO ) 
   {
      ऊपरी_Ch = निचला_Ch;
      निचला_Ch = 0; /* zero-byte in LSB special character */
   }
   वापस (char16_t)((ऊपरी_Ch << 8) | निचला_Ch);
}



/* CHECK_SOURCE_LIMIT: Helper macro to verify that there are at least'index' 
   bytes left in source up to  sourceLimit.Errors appropriately if not.
   If we reach the limit, then update the source pointer to there to consume
   all input as required by ICU converter semantics.
*/

#घोषणा CHECK_SOURCE_LIMIT(सूचकांक) UPRV_BLOCK_MACRO_BEGIN { \
    अगर (args->source+सूचकांक > args->sourceLimit) { \
        *err = U_TRUNCATED_CHAR_FOUND; \
        args->source = args->sourceLimit; \
        वापस 0xffff; \
    } \
} UPRV_BLOCK_MACRO_END

/* Return the Unicode representation for the current LMBCS character */

अटल UChar32  U_CALLCONV
LMBCSGet_अग्र_UChar_Worker(UConverterToUnicodeArgs*   args,
                         UErrorCode*   err)
{
    UChar32 uniChar = 0;    /* an output UNICODE char */
    ulmbcs_byte_t   CurByte; /* A byte from the input stream */

    /* error check */
    अगर (args->source >= args->sourceLimit)
    {
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        वापस 0xffff;
    }
    /* Grab first byte & save address for error recovery */
    CurByte = *((ulmbcs_byte_t  *) (args->source++));
   
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

    अगर(((CurByte > ULMBCS_C0END) && (CurByte < ULMBCS_C1START)) /* ascii range */
    ||  (CurByte == 0) 
    ||  CurByte == ULMBCS_HT || CurByte == ULMBCS_CR 
    ||  CurByte == ULMBCS_LF || CurByte == ULMBCS_123SYSTEMRANGE)
    {
        uniChar = CurByte;
    }
    अन्यथा  
    {
        UConverter_तथ्य_LMBCS * extraInfo;
        ulmbcs_byte_t group; 
        UConverter_Shared_तथ्य *cnv; 
        
        अगर (CurByte == ULMBCS_GRP_CTRL)  /* Control character group - no opt group update */
        {
            ulmbcs_byte_t  C0C1byte;
            CHECK_SOURCE_LIMIT(1);
            C0C1byte = *(args->source)++;
            uniChar = (C0C1byte < ULMBCS_C1START) ? C0C1byte - ULMBCS_CTRLOFFSET : C0C1byte;
        }
        अन्यथा 
        अगर (CurByte == ULMBCS_GRP_UNICODE) /* Unicode compatibility group: BigEndian UTF16 */
        {
            CHECK_SOURCE_LIMIT(2);
     
            /* don't check for error indicators fffe/ffff below */
            वापस GetUniFromLMBCSUni(&(args->source));
        }
        अन्यथा अगर (CurByte <= ULMBCS_CTRLOFFSET)  
        {
            group = CurByte;                   /* group byte is in the source */
            extraInfo = (UConverter_तथ्य_LMBCS *) args->converter->extraInfo;
            अगर (group > ULMBCS_GRP_LAST || (cnv = extraInfo->OptGrpConverter[group]) == nullptr)
            {
                /* this is not a valid group byte - no converter*/
                *err = U_INVALID_CHAR_FOUND;
            }      
            अन्यथा अगर (group >= ULMBCS_DOUBLEOPTGROUP_START)    /* double byte conversion */
            {

                CHECK_SOURCE_LIMIT(2);

                /* check for LMBCS doubled-group-byte case */
                अगर (*args->source == group) {
                    /* single byte */
                    ++args->source;
                    uniChar = ucnv_MBCSSimple_Get_अग्र_UChar(cnv, args->source, 1, false);
                    ++args->source;
                } अन्यथा {
                    /* double byte */
                    uniChar = ucnv_MBCSSimple_Get_अग्र_UChar(cnv, args->source, 2, false);
                    args->source += 2;
                }
            }
            अन्यथा {                                  /* single byte conversion */
                CHECK_SOURCE_LIMIT(1);
                CurByte = *(args->source)++;
        
                अगर (CurByte >= ULMBCS_C1START)
                {
                    uniChar = MBCS_SINGLE_SIMPLE_GET_अग्र_BMP(cnv, CurByte);
                }
                अन्यथा
                {
                    /* The non-optimizable oddballs where there is an explicit byte 
                    * AND the second byte is not in the upper ascii range
                    */
                    अक्षर bytes[2];

                    extraInfo = (UConverter_तथ्य_LMBCS *) args->converter->extraInfo;
                    cnv = extraInfo->OptGrpConverter [ULMBCS_GRP_EXCEPT];  
        
                    /* Lookup value must include opt group */
                    bytes[0] = group;
                    bytes[1] = CurByte;
                    uniChar = ucnv_MBCSSimple_Get_अग्र_UChar(cnv, bytes, 2, false);
                }
            }
        }
        अन्यथा अगर (CurByte >= ULMBCS_C1START) /* group byte is implicit */
        {
            extraInfo = (UConverter_तथ्य_LMBCS *) args->converter->extraInfo;
            group = extraInfo->OptGroup;
            cnv = extraInfo->OptGrpConverter[group];
            अगर (group >= ULMBCS_DOUBLEOPTGROUP_START)    /* double byte conversion */
            {
                अगर (!ucnv_MBCSIsLeadByte(cnv, CurByte))
                {
                    CHECK_SOURCE_LIMIT(0);

                    /* let the MBCS conversion consume CurByte again */
                    uniChar = ucnv_MBCSSimple_Get_अग्र_UChar(cnv, args->source - 1, 1, false);
                }
                अन्यथा
                {
                    CHECK_SOURCE_LIMIT(1);
                    /* let the MBCS conversion consume CurByte again */
                    uniChar = ucnv_MBCSSimple_Get_अग्र_UChar(cnv, args->source - 1, 2, false);
                    ++args->source;
                }
            }
            अन्यथा                                   /* single byte conversion */
            {
                uniChar = MBCS_SINGLE_SIMPLE_GET_अग्र_BMP(cnv, CurByte);
            }
        }
    }
    वापस uniChar;
}


/* The exported function that converts lmbcs to one or more
   UChars - currently UTF-16
*/
अटल व्योम  U_CALLCONV
_LMBCSToUnicodeWithOffsets(UConverterToUnicodeArgs*    args,
                     UErrorCode*    err)
{
   अक्षर LMBCS [ULMBCS_CHARSIZE_MAX];
   char16_t uniChar;    /* one output UNICODE char */
   स्थिर अक्षर * saveSource; /* beginning of current code point */
   स्थिर अक्षर * pStartLMBCS = args->source;  /* beginning of whole string */
   स्थिर अक्षर * errSource = nullptr; /* pointer to actual input in case an error occurs */
   int8_t savebytes = 0;

   /* Process from source to limit, or until error */
   जबतक (U_SUCCESS(*err) && args->sourceLimit > args->source && args->targetLimit > args->target)
   {
      saveSource = args->source; /* beginning of current code point */

      अगर (args->converter->toULength) /* reassemble char from previous call */
      {
        स्थिर अक्षर *saveSourceLimit; 
        माप_प्रकार आकार_old = args->converter->toULength;

         /* limit from source is either remainder of temp buffer, or user limit on source */
        माप_प्रकार आकार_new_maybe_1 = माप(LMBCS) - आकार_old;
        माप_प्रकार आकार_new_maybe_2 = args->sourceLimit - args->source;
        माप_प्रकार आकार_new = (आकार_new_maybe_1 < आकार_new_maybe_2) ? आकार_new_maybe_1 : आकार_new_maybe_2;
         
      
        uprv_memcpy(LMBCS, args->converter->toUBytes, आकार_old);
        uprv_memcpy(LMBCS + आकार_old, args->source, आकार_new);
        saveSourceLimit = args->sourceLimit;
        args->source = errSource = LMBCS;
        args->sourceLimit = LMBCS+आकार_old+आकार_new;
        savebytes = (int8_t)(आकार_old+आकार_new);
        uniChar = (char16_t) LMBCSGet_अग्र_UChar_Worker(args, err);
        args->source = saveSource + ((args->source - LMBCS) - आकार_old);
        args->sourceLimit = saveSourceLimit;

        अगर (*err == U_TRUNCATED_CHAR_FOUND)
        {
            /* evil special case: source buffers so small a char spans more than 2 buffers */
            args->converter->toULength = savebytes;
            uprv_memcpy(args->converter->toUBytes, LMBCS, savebytes);
            args->source = args->sourceLimit;
            *err = U_ZERO_ERROR;
            वापस;
         }
         अन्यथा
         {
            /* clear the partial-char marker */
            args->converter->toULength = 0;
         }
      }
      अन्यथा
      {
         errSource = saveSource;
         uniChar = (char16_t) LMBCSGet_अग्र_UChar_Worker(args, err);
         savebytes = (int8_t)(args->source - saveSource);
      }
      अगर (U_SUCCESS(*err))
      {
         अगर (uniChar < 0xfffe)
         {
            *(args->target)++ = uniChar;
            अगर(args->offsets)
            {
               *(args->offsets)++ = (int32_t)(saveSource - pStartLMBCS);
            }
         }
         अन्यथा अगर (uniChar == 0xfffe)
         {
            *err = U_INVALID_CHAR_FOUND;
         }
         अन्यथा /* if (uniChar == 0xffff) */
         {
            *err = U_ILLEGAL_CHAR_FOUND;
         }
      }
   }
   /* if target ran out before source, return U_BUFFER_OVERFLOW_ERROR */
   अगर (U_SUCCESS(*err) && args->sourceLimit > args->source && args->targetLimit <= args->target)
   {
      *err = U_बफर_OVERFLOW_ERROR;
   }
   अन्यथा अगर (U_FAILURE(*err)) 
   {
      /* If character incomplete or unmappable/illegal, store it in toUBytes[] */
      args->converter->toULength = savebytes;
      अगर (savebytes > 0) {
         uprv_memcpy(args->converter->toUBytes, errSource, savebytes);
      }
      अगर (*err == U_TRUNCATED_CHAR_FOUND) {
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


DECLARE_LMBCS_तथ्य(1)
DECLARE_LMBCS_तथ्य(2)
DECLARE_LMBCS_तथ्य(3)
DECLARE_LMBCS_तथ्य(4)
DECLARE_LMBCS_तथ्य(5)
DECLARE_LMBCS_तथ्य(6)
DECLARE_LMBCS_तथ्य(8)
DECLARE_LMBCS_तथ्य(11)
DECLARE_LMBCS_तथ्य(16)
DECLARE_LMBCS_तथ्य(17)
DECLARE_LMBCS_तथ्य(18)
DECLARE_LMBCS_तथ्य(19)

U_CDECL_END

#पूर्ण_अगर /* #if !UCONFIG_NO_LEGACY_CONVERSION */
