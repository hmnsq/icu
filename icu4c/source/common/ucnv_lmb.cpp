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

#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ !UCONFIG_NO_CONVERSION && !UCONFIG_NO_LEGACY_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION

#అన్తర్భావ "unicode/ucnv_err.h"
#అన్తర్భావ "unicode/ucnv.h"
#అన్తర్భావ "unicode/uset.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "cstring.h"
#అన్తర్భావ "uassert.h"
#అన్తర్భావ "ucnv_imp.h"
#అన్తర్భావ "ucnv_bld.h"
#అన్తర్భావ "ucnv_cnv.h"

#నిర్వచితమైతే EBCDIC_RTL
    #అన్తర్భావ "ascii_a.h"
#అయితే_ముగింపు

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
#నిర్వచించు ULMBCS_CHARSIZE_MAX      3
/*
  The single-byte values from 0x20 to 0x7F are examples of single D1 bytes.
  We often have to figure out if byte values are below or above this, so we 
  use the ANSI nomenclature 'C0' and 'C1' to refer to the range of control 
  characters just above & below the common lower-ANSI  range */
#నిర్వచించు ULMBCS_C0_చివర           0x1F   
#నిర్వచించు ULMBCS_C1_ప్రారంభం         0x80   
/*
  Since LMBCS is always dealing in byte units. we create a local type here for 
  dealing with these units of LMBCS code units:

*/  
రకనిర్వచనం uint8_t ulmbcs_byte_t;

/* 
   Most of the values less than 0x20 are reserved in LMBCS to announce 
   which national  character standard is being used for the 'D' bytes. 
   In the comments we show the common name and the IBM character-set ID
   for these character-set announcers:
*/

#నిర్వచించు ULMBCS_GRP_L1         0x01   /* Latin-1    :ibm-850  */
#నిర్వచించు ULMBCS_GRP_GR         0x02   /* Greek      :ibm-851  */
#నిర్వచించు ULMBCS_GRP_HE         0x03   /* Hebrew     :ibm-1255 */
#నిర్వచించు ULMBCS_GRP_AR         0x04   /* Arabic     :ibm-1256 */
#నిర్వచించు ULMBCS_GRP_RU         0x05   /* Cyrillic   :ibm-1251 */
#నిర్వచించు ULMBCS_GRP_L2         0x06   /* Latin-2    :ibm-852  */
#నిర్వచించు ULMBCS_GRP_TR         0x08   /* Turkish    :ibm-1254 */
#నిర్వచించు ULMBCS_GRP_TH         0x0B   /* Thai       :ibm-874  */
#నిర్వచించు ULMBCS_GRP_JA         0x10   /* Japanese   :ibm-943  */
#నిర్వచించు ULMBCS_GRP_KO         0x11   /* Korean     :ibm-1261 */
#నిర్వచించు ULMBCS_GRP_TW         0x12   /* Chinese SC :ibm-950  */
#నిర్వచించు ULMBCS_GRP_CN         0x13   /* Chinese TC :ibm-1386 */

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

#నిర్వచించు ULMBCS_DOUBLEOPTGROUP_ప్రారంభం  0x10   

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
                     
#నిర్వచించు ULMBCS_HT    0x09   /* Fixed control char - Horizontal Tab */
#నిర్వచించు ULMBCS_LF    0x0A   /* Fixed control char - Line Feed */
#నిర్వచించు ULMBCS_CR    0x0D   /* Fixed control char - Carriage Return */

/* Then, 1-2-3 reserved a special single-byte character to put at the 
beginning of internal 'system' range names: */

#నిర్వచించు ULMBCS_123SYSTEMRANGE  0x19   

/* Then we needed a place to put all the other ansi control characters 
that must be moved to different values because LMBCS reserves those 
values for other purposes. To represent the control characters, we start 
with a first byte of 0xF & add the control character value as the 
second byte */
#నిర్వచించు ULMBCS_GRP_CTRL       0x0F   

/* For the C0 controls (less than 0x20), we add 0x20 to preserve the 
useful doctrine that any byte less than 0x20 in a LMBCS char must be 
the first byte of a character:*/
#నిర్వచించు ULMBCS_CTRLOFFSET      0x20   

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

#నిర్వచించు ULMBCS_GRP_EXCEPT     0x00    
/*
 Finally, as the durability and usefulness of UNICODE became clear, 
 LOTUS added a new group 0x14 to hold Unicode values not otherwise 
 represented in LMBCS: */
#నిర్వచించు ULMBCS_GRP_UNICODE    0x14   
/* The two bytes appearing after a 0x14 are interpreted as UFT-16 BE
(Big-Endian) characters. The exception comes when the UTF16 
representation would have a zero as the second byte. In that case,
'F6' is used in its place, and the bytes are swapped. (This prevents 
LMBCS from encoding any Unicode values of the form U+F6xx, but that's OK:
0xF6xx is in the middle of the Private Use Area.)*/
#నిర్వచించు ULMBCS_UNICOMPATZERO   0xF6   

/* It is also useful in our code to have a constant for the size of 
a LMBCS char that holds a literal Unicode value */
#నిర్వచించు ULMBCS_UNICODE_కొలత      3    

/* 
To squish the LMBCS representations down even further, and to make 
translations even faster,sometimes the optimization group byte can be dropped 
from a LMBCS character. This is decided on a process-by-process basis. The 
group byte that is dropped is called the 'optimization group'.

For Notes, the optimzation group is always 0x1.*/
#నిర్వచించు ULMBCS_DEFAULTOPTGROUP 0x1    
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

#నిర్వచించు ULMBCS_GRP_చివరి       0x13   /* last LMBCS group that has a converter */

స్థిర స్థిరరాశి అక్షరం * స్థిరరాశి Opt_సమూహం_Byte_గమ్యం_CPName[ULMBCS_GRP_చివరి + 1] = {
   /* 0x0000 */ "lmb-excp", /* internal home for the LOTUS exceptions list */
   /* 0x0001 */ "ibm-850",
   /* 0x0002 */ "ibm-851",
   /* 0x0003 */ "windows-1255",
   /* 0x0004 */ "windows-1256",
   /* 0x0005 */ "windows-1251",
   /* 0x0006 */ "ibm-852",
   /* 0x0007 */ శూన్యసూచిక,      /* Unused */
   /* 0x0008 */ "windows-1254",
   /* 0x0009 */ శూన్యసూచిక,      /* Control char HT */
   /* 0x000A */ శూన్యసూచిక,      /* Control char LF */
   /* 0x000B */ "windows-874",
   /* 0x000C */ శూన్యసూచిక,      /* Unused */
   /* 0x000D */ శూన్యసూచిక,      /* Control char CR */
   /* 0x000E */ శూన్యసూచిక,      /* Unused */
   /* 0x000F */ శూన్యసూచిక,      /* Control chars: 0x0F20 + C0/C1 character: algorithmic */
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

#నిర్వచించు ULMBCS_AMBIGUOUS_SBCS   0x80   /* could fit in more than one 
                                          LMBCS sbcs native encoding 
                                          (example: most accented latin) */
#నిర్వచించు ULMBCS_AMBIGUOUS_MBCS   0x81   /* could fit in more than one 
                                          LMBCS mbcs native encoding 
                                          (example: Unihan) */
#నిర్వచించు ULMBCS_AMBIGUOUS_ALL   0x82
/* And here's a simple way to see if a group falls in an appropriate range */
#నిర్వచించు ULMBCS_AMBIGUOUS_సరిపోలిక(agroup, xgroup) \
                  ((((agroup) == ULMBCS_AMBIGUOUS_SBCS) && \
                  (xgroup) < ULMBCS_DOUBLEOPTGROUP_ప్రారంభం) || \
                  (((agroup) == ULMBCS_AMBIGUOUS_MBCS) && \
                  (xgroup) >= ULMBCS_DOUBLEOPTGROUP_ప్రారంభం)) || \
                  ((agroup) == ULMBCS_AMBIGUOUS_ALL)


/* The table & some code to use it: */


స్థిర స్థిరరాశి నిర్మాణం _UniLMBCSGrpMap  
{
   స్థిరరాశి char16_t uni_ప్రారంభం_Range;
   స్థిరరాశి char16_t uni_చివర_Range;
   స్థిరరాశి ulmbcs_byte_t  GrpType;
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
   
స్థిర ulmbcs_byte_t 
కనుగొను_LMBCSUni_Range(char16_t uniChar)
{
   స్థిరరాశి నిర్మాణం _UniLMBCSGrpMap * p_పట్టిక = UniLMBCSGrpMap;

   అయితే (uniChar > p_పట్టిక->uni_చివర_Range) 
   {
      p_పట్టిక++;
   }

   ఒకవేళ (uniChar >= p_పట్టిక->uni_ప్రారంభం_Range) 
   {
      తిరిగి p_పట్టిక->GrpType;
   }
   తిరిగి ULMBCS_GRP_UNICODE;
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

స్థిర స్థిరరాశి నిర్మాణం _LocaleLMBCSGrpMap
{
   స్థిరరాశి అక్షరం    *LocaleID;
   స్థిరరాశి ulmbcs_byte_t Opt_సమూహం;
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
    {శూన్యసూచిక, ULMBCS_GRP_L1}
};


స్థిర ulmbcs_byte_t 
కనుగొను_LMBCSLocale(స్థిరరాశి అక్షరం *LocaleID)
{
   స్థిరరాశి నిర్మాణం _LocaleLMBCSGrpMap *p_పట్టిక = LocaleLMBCSGrpMap;

   ఒకవేళ ((!LocaleID) || (!*LocaleID)) 
   {
      తిరిగి 0;
   }

   అయితే (p_పట్టిక->LocaleID)
   {
      ఒకవేళ (*p_పట్టిక->LocaleID == *LocaleID) /* Check only first char for speed */
      {
         /* First char matches - check whole name, for entry-length */
         ఒకవేళ (uprv_strncmp(p_పట్టిక->LocaleID, LocaleID, అక్షరశ్రేణి_పొడవు(p_పట్టిక->LocaleID)) == 0)
            తిరిగి p_పట్టిక->Opt_సమూహం;
      }
      లేకపోతే
      ఒకవేళ (*p_పట్టిక->LocaleID > *LocaleID) /* Sorted alphabetically - exit */
         ఆపు;
      p_పట్టిక++;
   }
   తిరిగి ULMBCS_GRP_L1;
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

రకనిర్వచనం నిర్మాణం
  {
    UConverter_Shared_దత్తాంశం *OptGrpConverter[ULMBCS_GRP_చివరి+1];    /* Converter per Opt. grp. */
    uint8_t    Opt_సమూహం;                  /* default Opt. grp. for this LMBCS session */
    uint8_t    locale_Converter_సూచీ;      /* reasonable locale match for index */
  }
UConverter_దత్తాంశం_LMBCS;

U_CDECL_BEGIN
స్థిర శూన్య  U_CALLCONV _LMBCSClose(UConverter * _this);
U_CDECL_చివర

#నిర్వచించు DECLARE_LMBCS_దత్తాంశం(n) \
స్థిర స్థిరరాశి UConverterImpl _LMBCSImpl##n={\
    UCNV_LMBCS_##n,\
    శూన్యసూచిక,శూన్యసూచిక,\
    _LMBCSOpen##n,\
    _LMBCSClose,\
    శూన్యసూచిక,\
    _LMBCSToUnicodeWithOffsets,\
    _LMBCSToUnicodeWithOffsets,\
    _LMBCSFromUnicode,\
    _LMBCSFromUnicode,\
    శూన్యసూచిక,\
    శూన్యసూచిక,\
    శూన్యసూచిక,\
    శూన్యసూచిక,\
    _LMBCSSafeClone,\
    ucnv_get_Complete_Unicode_సమితి,\
    శూన్యసూచిక,\
    శూన్యసూచిక\
};\
స్థిర స్థిరరాశి UConverter_Static_దత్తాంశం LMBCSStatic_దత్తాంశం##n={\
  పరిమాణం(UConverter_Static_దత్తాంశం),\
 "LMBCS-"  #n,\
    0, UCNV_IBM, UCNV_LMBCS_##n, 1, 3,\
    { 0x3f, 0, 0, 0 },1,అబద్ధం,అబద్ధం,0,0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0} \
};\
స్థిరరాశి UConverter_Shared_దత్తాంశం _LMBCSData##n= \
        UCNV_IMMUTABLE_SHARED_దత్తాంశం_INITIALIZER(&LMBCSStatic_దత్తాంశం##n, &_LMBCSImpl##n);

 /* The only function we needed to duplicate 12 times was the 'open'
function, which will do basically the same thing except set a  different
optimization group. So, we put the common stuff into a worker function, 
and set up another macro to stamp out the 12 open functions:*/
#నిర్వచించు DEFINE_LMBCS_OPEN(n) \
స్థిర శూన్య U_CALLCONV \
   _LMBCSOpen##n(UConverter* _this, UConverter_ఎక్కించు_వాదనలు* p_వాదనలు, UError_సంకేతం* err) \
{ LMBCSOpen_కార్మికుడు(_this, p_వాదనలు, err, n); }



/* Here's the open worker & the common close function */
స్థిర శూన్య 
LMBCSOpen_కార్మికుడు(UConverter*  _this,
                 UConverter_ఎక్కించు_వాదనలు *p_వాదనలు,
                 UError_సంకేతం*  err,
                 ulmbcs_byte_t Opt_సమూహం)
{
    UConverter_దత్తాంశం_LMBCS* extraInfo = static_cast<UConverter_దత్తాంశం_LMBCS*>(uprv_malloc(పరిమాణం(UConverter_దత్తాంశం_LMBCS)));
    _this->extraInfo = extraInfo;
    ఒకవేళ(extraInfo != శూన్యసూచిక)
    {
        UConverterNamePieces దొంతర_Pieces;
        UConverter_ఎక్కించు_వాదనలు దొంతర_వాదనలు= UCNV_ఎక్కించు_వాదనలు_INITIALIZER;
        ulmbcs_byte_t i;

        uprv_memset(extraInfo, 0, పరిమాణం(UConverter_దత్తాంశం_LMBCS));

        దొంతర_వాదనలు.onlyTestIsLoadable = p_వాదనలు->onlyTestIsLoadable;

        కొరకు (i=0; i <= ULMBCS_GRP_చివరి && U_SUCCESS(*err); i++)         
        {
            ఒకవేళ(Opt_సమూహం_Byte_గమ్యం_CPName[i] != శూన్యసూచిక) {
                extraInfo->OptGrpConverter[i] = ucnv_ఎక్కించు_Shared_దత్తాంశం(Opt_సమూహం_Byte_గమ్యం_CPName[i], &దొంతర_Pieces, &దొంతర_వాదనలు, err);
            }
        }

        ఒకవేళ(U_FAILURE(*err) || p_వాదనలు->onlyTestIsLoadable) {
            _LMBCSClose(_this);
            తిరిగి;
        }
        extraInfo->Opt_సమూహం = Opt_సమూహం;
        extraInfo->locale_Converter_సూచీ = కనుగొను_LMBCSLocale(p_వాదనలు->locale);
    }
    లేకపోతే
    {
        *err = U_జ్ఞాపకం_కేటాయింపు_ERROR;
    }
}

U_CDECL_BEGIN
స్థిర శూన్య  U_CALLCONV
_LMBCSClose(UConverter *   _this) 
{
    ఒకవేళ (_this->extraInfo != శూన్యసూచిక)
    {
        ulmbcs_byte_t Ix;
        UConverter_దత్తాంశం_LMBCS * extraInfo = (UConverter_దత్తాంశం_LMBCS *) _this->extraInfo;

        కొరకు (Ix=0; Ix <= ULMBCS_GRP_చివరి; Ix++)
        {
           ఒకవేళ (extraInfo->OptGrpConverter[Ix] != శూన్యసూచిక)
              ucnv_unload_Shared_దత్తాంశం_If_Ready(extraInfo->OptGrpConverter[Ix]);
        }
        ఒకవేళ (!_this->isExtraLocal) {
            uprv_free (_this->extraInfo);
            _this->extraInfo = శూన్యసూచిక;
        }
    }
}

రకనిర్వచనం నిర్మాణం LMBCSClone {
    UConverter cnv;
    UConverter_దత్తాంశం_LMBCS lmbcs;
} LMBCSClone;

స్థిర UConverter *  U_CALLCONV
_LMBCSSafeClone(స్థిరరాశి UConverter *cnv, 
                శూన్య *దొంతర_బఫరు, 
                int32_t *p_బఫరు_కొలత, 
                UError_సంకేతం *స్థితి_2) {
    (శూన్య)స్థితి_2;
    LMBCSClone *newLMBCS;
    UConverter_దత్తాంశం_LMBCS *extraInfo;
    int32_t i;

    ఒకవేళ(*p_బఫరు_కొలత<=0) {
        *p_బఫరు_కొలత=(int32_t)పరిమాణం(LMBCSClone);
        తిరిగి శూన్యసూచిక;
    }

    extraInfo=(UConverter_దత్తాంశం_LMBCS *)cnv->extraInfo;
    newLMBCS=(LMBCSClone *)దొంతర_బఫరు;

    /* ucnv.c/ucnv_safeClone() copied the main UConverter already */

    uprv_memcpy(&newLMBCS->lmbcs, extraInfo, పరిమాణం(UConverter_దత్తాంశం_LMBCS));

    /* share the subconverters */
    కొరకు(i = 0; i <= ULMBCS_GRP_చివరి; ++i) {
        ఒకవేళ(extraInfo->OptGrpConverter[i] != శూన్యసూచిక) {
            ucnv_increment_Ref_లెక్క(extraInfo->OptGrpConverter[i]);
        }
    }

    newLMBCS->cnv.extraInfo = &newLMBCS->lmbcs;
    newLMBCS->cnv.isExtraLocal = నిజం;
    తిరిగి &newLMBCS->cnv;
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

స్థిర size_t
LMBCSConversion_కార్మికుడు (
   UConverter_దత్తాంశం_LMBCS * extraInfo,    /* subconverters, opt & locale groups */
   ulmbcs_byte_t సమూహం,                /* The group to try */
   ulmbcs_byte_t  * p_ప్రారంభం_LMBCS,              /* where to put the results */
   char16_t * pUniChar,                   /* The input unicode character */
   ulmbcs_byte_t * చివరి_Converter_సూచీ, /* output: track last successful group used */
   UBool * groups_tried                /* output: track any unsuccessful groups */
)   
{
   ulmbcs_byte_t  * pLMBCS = p_ప్రారంభం_LMBCS;
   UConverter_Shared_దత్తాంశం * xcnv = extraInfo->OptGrpConverter[సమూహం];

   పూర్ణాంకం బైట్లు_Converted;
   uint32_t value;
   ulmbcs_byte_t మొదటి_Byte;

   U_ASSERT(xcnv);
   U_ASSERT(సమూహం<ULMBCS_GRP_UNICODE);

   బైట్లు_Converted = ucnv_MBCSFromUChar32(xcnv, *pUniChar, &value, అబద్ధం);

   /* get the first result byte */
   ఒకవేళ(బైట్లు_Converted > 0) {
      మొదటి_Byte = (ulmbcs_byte_t)(value >> ((బైట్లు_Converted - 1) * 8));
   } లేకపోతే {
      /* most common failure mode is an unassigned character */
      groups_tried[సమూహం] = నిజం;
      తిరిగి 0;
   }

   *చివరి_Converter_సూచీ = సమూహం;

   /* All initial byte values in lower ascii range should have been caught by now,
      except with the exception group.
    */
   U_ASSERT((మొదటి_Byte <= ULMBCS_C0_చివర) || (మొదటి_Byte >= ULMBCS_C1_ప్రారంభం) || (సమూహం == ULMBCS_GRP_EXCEPT));
   
   /* use converted data: first write 0, 1 or two group bytes */
   ఒకవేళ (సమూహం != ULMBCS_GRP_EXCEPT && extraInfo->Opt_సమూహం != సమూహం)
   {
      *pLMBCS++ = సమూహం;
      ఒకవేళ (బైట్లు_Converted == 1 && సమూహం >= ULMBCS_DOUBLEOPTGROUP_ప్రారంభం)
      {
         *pLMBCS++ = సమూహం;
      }
   }

  /* don't emit control chars */
   ఒకవేళ ( బైట్లు_Converted == 1 && మొదటి_Byte < 0x20 )
      తిరిగి 0;


   /* then move over the converted data */
   ఎంపిక(బైట్లు_Converted)
   {
   స్థితి 4:
      *pLMBCS++ = (ulmbcs_byte_t)(value >> 24);
      U_FALLTHROUGH;
   స్థితి 3:
      *pLMBCS++ = (ulmbcs_byte_t)(value >> 16);
      U_FALLTHROUGH;
   స్థితి 2:
      *pLMBCS++ = (ulmbcs_byte_t)(value >> 8);
      U_FALLTHROUGH;
   స్థితి 1:
      *pLMBCS++ = (ulmbcs_byte_t)value;
      U_FALLTHROUGH;
   అప్రమేయ:
      /* will never occur */
      ఆపు;
   }

   తిరిగి (pLMBCS - p_ప్రారంభం_LMBCS);
}


/* This is a much simpler version of above, when we 
know we are writing LMBCS using the Unicode group
*/
స్థిర size_t 
LMBCSConvertUni(ulmbcs_byte_t * pLMBCS, char16_t uniChar)
{
     /* encode into LMBCS Unicode range */
   uint8_t దిగువ_Ch =   (uint8_t)(uniChar & 0x00FF);
   uint8_t ఎగువ_Ch  = (uint8_t)(uniChar >> 8);

   *pLMBCS++ = ULMBCS_GRP_UNICODE;

   ఒకవేళ (దిగువ_Ch == 0)
   {
      *pLMBCS++ = ULMBCS_UNICOMPATZERO;
      *pLMBCS++ = ఎగువ_Ch;
   }
   లేకపోతే
   {
      *pLMBCS++ = ఎగువ_Ch;
      *pLMBCS++ = దిగువ_Ch;
   }
   తిరిగి ULMBCS_UNICODE_కొలత;
}



/* The main Unicode to LMBCS conversion function */
స్థిర శూన్య  U_CALLCONV
_LMBCSFromUnicode(UConverter_From_Unicode_వాదనలు*     వాదనలు_2,
                  UError_సంకేతం*     err)
{
   ulmbcs_byte_t చివరి_Converter_సూచీ = 0;
   char16_t uniChar;
   ulmbcs_byte_t  LMBCS[ULMBCS_CHARSIZE_MAX];
   ulmbcs_byte_t  * pLMBCS;
   int32_t బైట్లు_written;
   UBool groups_tried[ULMBCS_GRP_చివరి+1];
   UConverter_దత్తాంశం_LMBCS * extraInfo = (UConverter_దత్తాంశం_LMBCS *) వాదనలు_2->converter->extraInfo;
   పూర్ణాంకం ఆధారం_సూచీ = 0; 

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
    ulmbcs_byte_t Old_Converter_సూచీ = 0;

   అయితే (వాదనలు_2->ఆధారం < వాదనలు_2->ఆధారం_హద్దు && !U_FAILURE(*err))
   {
      /*Fix for SPR#DJOE66JFN3 (Lotus)*/
      Old_Converter_సూచీ = extraInfo->locale_Converter_సూచీ;

      ఒకవేళ (వాదనలు_2->లక్ష్యం >= వాదనలు_2->లక్ష్యం_హద్దు)
      {
         *err = U_బఫరు_అధికప్రవాహం_ERROR;
         ఆపు;
      }
      uniChar = *(వాదనలు_2->ఆధారం);
      బైట్లు_written = 0;
      pLMBCS = LMBCS;

      /* check cases in rough order of how common they are, for speed */

      /* single byte matches: strategy 1 */
      /*Fix for SPR#DJOE66JFN3 (Lotus)*/
      ఒకవేళ((uniChar>=0x80) && (uniChar<=0xff)
      /*Fix for SPR#JUYA6XAERU and TSAO7GL5NK (Lotus)*/ &&(uniChar!=0xB1) &&(uniChar!=0xD7) &&(uniChar!=0xF7)
        &&(uniChar!=0xB0) &&(uniChar!=0xB4) &&(uniChar!=0xB6) &&(uniChar!=0xA7) &&(uniChar!=0xA8))
      {
            extraInfo->locale_Converter_సూచీ = ULMBCS_GRP_L1;
      }
      ఒకవేళ (((uniChar > ULMBCS_C0_చివర) && (uniChar < ULMBCS_C1_ప్రారంభం)) ||
          uniChar == 0 || uniChar == ULMBCS_HT || uniChar == ULMBCS_CR || 
          uniChar == ULMBCS_LF || uniChar == ULMBCS_123SYSTEMRANGE 
          )
      {
         *pLMBCS++ = (ulmbcs_byte_t ) uniChar;
         బైట్లు_written = 1;
      }


      ఒకవేళ (!బైట్లు_written) 
      {
         /* Check by UNICODE range (Strategy 2) */
         ulmbcs_byte_t సమూహం = కనుగొను_LMBCSUni_Range(uniChar);
         
         ఒకవేళ (సమూహం == ULMBCS_GRP_UNICODE)  /* (Strategy 2A) */
         {
            pLMBCS += LMBCSConvertUni(pLMBCS,uniChar);
            
            బైట్లు_written = (int32_t)(pLMBCS - LMBCS);
         }
         లేకపోతే ఒకవేళ (సమూహం == ULMBCS_GRP_CTRL)  /* (Strategy 2B) */
         {
            /* Handle control characters here */
            ఒకవేళ (uniChar <= ULMBCS_C0_చివర)
            {
               *pLMBCS++ = ULMBCS_GRP_CTRL;
               *pLMBCS++ = (ulmbcs_byte_t)(ULMBCS_CTRLOFFSET + uniChar);
            }
            లేకపోతే ఒకవేళ (uniChar >= ULMBCS_C1_ప్రారంభం && uniChar <= ULMBCS_C1_ప్రారంభం + ULMBCS_CTRLOFFSET)
            {
               *pLMBCS++ = ULMBCS_GRP_CTRL;
               *pLMBCS++ = (ulmbcs_byte_t ) (uniChar & 0x00FF);
            }
            బైట్లు_written = (int32_t)(pLMBCS - LMBCS);
         }
         లేకపోతే ఒకవేళ (సమూహం < ULMBCS_GRP_UNICODE)  /* (Strategy 2C) */
         {
            /* a specific converter has been identified - use it */
            బైట్లు_written = (int32_t)LMBCSConversion_కార్మికుడు (
                              extraInfo, సమూహం, pLMBCS, &uniChar, 
                              &చివరి_Converter_సూచీ, groups_tried);
         }
         ఒకవేళ (!బైట్లు_written)    /* the ambiguous group cases  (Strategy 3) */
         {
            uprv_memset(groups_tried, 0, పరిమాణం(groups_tried));

            /* check for non-default optimization group (Strategy 3A )*/
            ఒకవేళ ((extraInfo->Opt_సమూహం != 1) && (ULMBCS_AMBIGUOUS_సరిపోలిక(సమూహం, extraInfo->Opt_సమూహం)))
            {
                /*zhujin: upgrade, merge #39299 here (Lotus) */
                /*To make R5 compatible translation, look for exceptional group first for non-DBCS*/

                ఒకవేళ(extraInfo->locale_Converter_సూచీ < ULMBCS_DOUBLEOPTGROUP_ప్రారంభం)
                {
                  బైట్లు_written = (int32_t)LMBCSConversion_కార్మికుడు (extraInfo,
                     ULMBCS_GRP_L1, pLMBCS, &uniChar,
                     &చివరి_Converter_సూచీ, groups_tried);

                  ఒకవేళ(!బైట్లు_written)
                  {
                     బైట్లు_written = (int32_t)LMBCSConversion_కార్మికుడు (extraInfo,
                         ULMBCS_GRP_EXCEPT, pLMBCS, &uniChar,
                         &చివరి_Converter_సూచీ, groups_tried);
                  }
                  ఒకవేళ(!బైట్లు_written)
                  {
                      బైట్లు_written = (int32_t)LMBCSConversion_కార్మికుడు (extraInfo,
                          extraInfo->locale_Converter_సూచీ, pLMBCS, &uniChar,
                          &చివరి_Converter_సూచీ, groups_tried);
                  }
                }
                లేకపోతే
                {
                     బైట్లు_written = (int32_t)LMBCSConversion_కార్మికుడు (extraInfo,
                         extraInfo->locale_Converter_సూచీ, pLMBCS, &uniChar,
                         &చివరి_Converter_సూచీ, groups_tried);
                }
            }
            /* check for locale optimization group (Strategy 3B) */
            ఒకవేళ (!బైట్లు_written && (extraInfo->locale_Converter_సూచీ) && (ULMBCS_AMBIGUOUS_సరిపోలిక(సమూహం, extraInfo->locale_Converter_సూచీ)))
            {
                బైట్లు_written = (int32_t)LMBCSConversion_కార్మికుడు (extraInfo,
                        extraInfo->locale_Converter_సూచీ, pLMBCS, &uniChar, &చివరి_Converter_సూచీ, groups_tried);
            }
            /* check for last optimization group used for this string (Strategy 3C) */
            ఒకవేళ (!బైట్లు_written && (చివరి_Converter_సూచీ) && (ULMBCS_AMBIGUOUS_సరిపోలిక(సమూహం, చివరి_Converter_సూచీ)))
            {
                బైట్లు_written = (int32_t)LMBCSConversion_కార్మికుడు (extraInfo,
                        చివరి_Converter_సూచీ, pLMBCS, &uniChar, &చివరి_Converter_సూచీ, groups_tried);
            }
            ఒకవేళ (!బైట్లు_written)
            {
               /* just check every possible matching converter (Strategy 3D) */ 
               ulmbcs_byte_t grp_ప్రారంభం;
               ulmbcs_byte_t grp_చివర;  
               ulmbcs_byte_t grp_ix;
               grp_ప్రారంభం = (ulmbcs_byte_t)((సమూహం == ULMBCS_AMBIGUOUS_MBCS) 
                        ? ULMBCS_DOUBLEOPTGROUP_ప్రారంభం 
                        :  ULMBCS_GRP_L1);
               grp_చివర = (ulmbcs_byte_t)((సమూహం == ULMBCS_AMBIGUOUS_MBCS) 
                        ? ULMBCS_GRP_చివరి 
                        :  ULMBCS_GRP_TH);
               ఒకవేళ(సమూహం == ULMBCS_AMBIGUOUS_ALL)
               {
                   grp_ప్రారంభం = ULMBCS_GRP_L1;
                   grp_చివర = ULMBCS_GRP_చివరి;
               }
               కొరకు (grp_ix = grp_ప్రారంభం;
                   grp_ix <= grp_చివర && !బైట్లు_written; 
                    grp_ix++)
               {
                  ఒకవేళ (extraInfo->OptGrpConverter [grp_ix] && !groups_tried [grp_ix])
                  {
                     బైట్లు_written = (int32_t)LMBCSConversion_కార్మికుడు (extraInfo,
                       grp_ix, pLMBCS, &uniChar, 
                       &చివరి_Converter_సూచీ, groups_tried);
                  }
               }
                /* a final conversion fallback to the exceptions group if its likely 
                     to be single byte  (Strategy 3E) */
               ఒకవేళ (!బైట్లు_written && grp_ప్రారంభం == ULMBCS_GRP_L1)
               {
                  బైట్లు_written = (int32_t)LMBCSConversion_కార్మికుడు (extraInfo,
                     ULMBCS_GRP_EXCEPT, pLMBCS, &uniChar, 
                     &చివరి_Converter_సూచీ, groups_tried);
               }
            }
            /* all of our other strategies failed. Fallback to Unicode. (Strategy 4)*/
            ఒకవేళ (!బైట్లు_written)
            {

               pLMBCS += LMBCSConvertUni(pLMBCS, uniChar);
               బైట్లు_written = (int32_t)(pLMBCS - LMBCS);
            }
         }
      }
  
      /* we have a translation. increment source and write as much as possible to target */
      వాదనలు_2->ఆధారం++;
      pLMBCS = LMBCS;
      అయితే (వాదనలు_2->లక్ష్యం < వాదనలు_2->లక్ష్యం_హద్దు && బైట్లు_written--)
      {
         *(వాదనలు_2->లక్ష్యం)++ = *pLMBCS++;
         ఒకవేళ (వాదనలు_2->offsets)
         {
            *(వాదనలు_2->offsets)++ = ఆధారం_సూచీ;
         }
      }
      ఆధారం_సూచీ++;
      ఒకవేళ (బైట్లు_written > 0)
      {
         /* write any bytes that didn't fit in target to the error buffer,
            common code will move this to target if we get called back with
            enough target room
         */
         uint8_t * p_Error_బఫరు = వాదనలు_2->converter->char_Error_బఫరు;
         *err = U_బఫరు_అధికప్రవాహం_ERROR;
         వాదనలు_2->converter->char_Error_బఫరు_నిడివి = (int8_t)బైట్లు_written;
         అయితే (బైట్లు_written--)
         {
            *p_Error_బఫరు++ = *pLMBCS++;
         }
      }
      /*Fix for SPR#DJOE66JFN3 (Lotus)*/
      extraInfo->locale_Converter_సూచీ = Old_Converter_సూచీ;
   }     
}


/* Now, the Unicode from LMBCS section */


/* A function to call when we are looking at the Unicode group byte in LMBCS */
స్థిర char16_t
GetUniFromLMBCSUni(అక్షరం స్థిరరాశి ** ppLMBCSin)  /* Called with LMBCS-style Unicode byte stream */
{
   uint8_t  ఎగువ_Ch = *(*ppLMBCSin)++;  /* Big-endian Unicode in LMBCS compatibility group*/
   uint8_t  దిగువ_Ch  = *(*ppLMBCSin)++;

   ఒకవేళ (ఎగువ_Ch == ULMBCS_UNICOMPATZERO ) 
   {
      ఎగువ_Ch = దిగువ_Ch;
      దిగువ_Ch = 0; /* zero-byte in LSB special character */
   }
   తిరిగి (char16_t)((ఎగువ_Ch << 8) | దిగువ_Ch);
}



/* CHECK_SOURCE_LIMIT: Helper macro to verify that there are at least'index' 
   bytes left in source up to  sourceLimit.Errors appropriately if not.
   If we reach the limit, then update the source pointer to there to consume
   all input as required by ICU converter semantics.
*/

#నిర్వచించు CHECK_ఆధారం_హద్దు(సూచీ) UPRV_ఖండం_MACRO_BEGIN { \
    ఒకవేళ (వాదనలు_2->ఆధారం+సూచీ > వాదనలు_2->ఆధారం_హద్దు) { \
        *err = U_TRUNCATED_CHAR_దొరికింది; \
        వాదనలు_2->ఆధారం = వాదనలు_2->ఆధారం_హద్దు; \
        తిరిగి 0xffff; \
    } \
} UPRV_ఖండం_MACRO_చివర

/* Return the Unicode representation for the current LMBCS character */

స్థిర UChar32  U_CALLCONV
LMBCSGet_తదుపరి_UChar_కార్మికుడు(UConverter_గమ్యం_Unicode_వాదనలు*   వాదనలు_2,
                         UError_సంకేతం*   err)
{
    UChar32 uniChar = 0;    /* an output UNICODE char */
    ulmbcs_byte_t   CurByte; /* A byte from the input stream */

    /* error check */
    ఒకవేళ (వాదనలు_2->ఆధారం >= వాదనలు_2->ఆధారం_హద్దు)
    {
        *err = U_ILLEGAL_వాదం_ERROR;
        తిరిగి 0xffff;
    }
    /* Grab first byte & save address for error recovery */
    CurByte = *((ulmbcs_byte_t  *) (వాదనలు_2->ఆధారం++));
   
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

    ఒకవేళ(((CurByte > ULMBCS_C0_చివర) && (CurByte < ULMBCS_C1_ప్రారంభం)) /* ascii range */
    ||  (CurByte == 0) 
    ||  CurByte == ULMBCS_HT || CurByte == ULMBCS_CR 
    ||  CurByte == ULMBCS_LF || CurByte == ULMBCS_123SYSTEMRANGE)
    {
        uniChar = CurByte;
    }
    లేకపోతే  
    {
        UConverter_దత్తాంశం_LMBCS * extraInfo;
        ulmbcs_byte_t సమూహం; 
        UConverter_Shared_దత్తాంశం *cnv; 
        
        ఒకవేళ (CurByte == ULMBCS_GRP_CTRL)  /* Control character group - no opt group update */
        {
            ulmbcs_byte_t  C0C1byte;
            CHECK_ఆధారం_హద్దు(1);
            C0C1byte = *(వాదనలు_2->ఆధారం)++;
            uniChar = (C0C1byte < ULMBCS_C1_ప్రారంభం) ? C0C1byte - ULMBCS_CTRLOFFSET : C0C1byte;
        }
        లేకపోతే 
        ఒకవేళ (CurByte == ULMBCS_GRP_UNICODE) /* Unicode compatibility group: BigEndian UTF16 */
        {
            CHECK_ఆధారం_హద్దు(2);
     
            /* don't check for error indicators fffe/ffff below */
            తిరిగి GetUniFromLMBCSUni(&(వాదనలు_2->ఆధారం));
        }
        లేకపోతే ఒకవేళ (CurByte <= ULMBCS_CTRLOFFSET)  
        {
            సమూహం = CurByte;                   /* group byte is in the source */
            extraInfo = (UConverter_దత్తాంశం_LMBCS *) వాదనలు_2->converter->extraInfo;
            ఒకవేళ (సమూహం > ULMBCS_GRP_చివరి || (cnv = extraInfo->OptGrpConverter[సమూహం]) == శూన్యసూచిక)
            {
                /* this is not a valid group byte - no converter*/
                *err = U_INVALID_CHAR_దొరికింది;
            }      
            లేకపోతే ఒకవేళ (సమూహం >= ULMBCS_DOUBLEOPTGROUP_ప్రారంభం)    /* double byte conversion */
            {

                CHECK_ఆధారం_హద్దు(2);

                /* check for LMBCS doubled-group-byte case */
                ఒకవేళ (*వాదనలు_2->ఆధారం == సమూహం) {
                    /* single byte */
                    ++వాదనలు_2->ఆధారం;
                    uniChar = ucnv_MBCSSimple_Get_తదుపరి_UChar(cnv, వాదనలు_2->ఆధారం, 1, అబద్ధం);
                    ++వాదనలు_2->ఆధారం;
                } లేకపోతే {
                    /* double byte */
                    uniChar = ucnv_MBCSSimple_Get_తదుపరి_UChar(cnv, వాదనలు_2->ఆధారం, 2, అబద్ధం);
                    వాదనలు_2->ఆధారం += 2;
                }
            }
            లేకపోతే {                                  /* single byte conversion */
                CHECK_ఆధారం_హద్దు(1);
                CurByte = *(వాదనలు_2->ఆధారం)++;
        
                ఒకవేళ (CurByte >= ULMBCS_C1_ప్రారంభం)
                {
                    uniChar = MBCS_SINGLE_SIMPLE_GET_తదుపరి_BMP(cnv, CurByte);
                }
                లేకపోతే
                {
                    /* The non-optimizable oddballs where there is an explicit byte 
                    * AND the second byte is not in the upper ascii range
                    */
                    అక్షరం బైట్లు[2];

                    extraInfo = (UConverter_దత్తాంశం_LMBCS *) వాదనలు_2->converter->extraInfo;
                    cnv = extraInfo->OptGrpConverter [ULMBCS_GRP_EXCEPT];  
        
                    /* Lookup value must include opt group */
                    బైట్లు[0] = సమూహం;
                    బైట్లు[1] = CurByte;
                    uniChar = ucnv_MBCSSimple_Get_తదుపరి_UChar(cnv, బైట్లు, 2, అబద్ధం);
                }
            }
        }
        లేకపోతే ఒకవేళ (CurByte >= ULMBCS_C1_ప్రారంభం) /* group byte is implicit */
        {
            extraInfo = (UConverter_దత్తాంశం_LMBCS *) వాదనలు_2->converter->extraInfo;
            సమూహం = extraInfo->Opt_సమూహం;
            cnv = extraInfo->OptGrpConverter[సమూహం];
            ఒకవేళ (సమూహం >= ULMBCS_DOUBLEOPTGROUP_ప్రారంభం)    /* double byte conversion */
            {
                ఒకవేళ (!ucnv_MBCSIsLeadByte(cnv, CurByte))
                {
                    CHECK_ఆధారం_హద్దు(0);

                    /* let the MBCS conversion consume CurByte again */
                    uniChar = ucnv_MBCSSimple_Get_తదుపరి_UChar(cnv, వాదనలు_2->ఆధారం - 1, 1, అబద్ధం);
                }
                లేకపోతే
                {
                    CHECK_ఆధారం_హద్దు(1);
                    /* let the MBCS conversion consume CurByte again */
                    uniChar = ucnv_MBCSSimple_Get_తదుపరి_UChar(cnv, వాదనలు_2->ఆధారం - 1, 2, అబద్ధం);
                    ++వాదనలు_2->ఆధారం;
                }
            }
            లేకపోతే                                   /* single byte conversion */
            {
                uniChar = MBCS_SINGLE_SIMPLE_GET_తదుపరి_BMP(cnv, CurByte);
            }
        }
    }
    తిరిగి uniChar;
}


/* The exported function that converts lmbcs to one or more
   UChars - currently UTF-16
*/
స్థిర శూన్య  U_CALLCONV
_LMBCSToUnicodeWithOffsets(UConverter_గమ్యం_Unicode_వాదనలు*    వాదనలు_2,
                     UError_సంకేతం*    err)
{
   అక్షరం LMBCS [ULMBCS_CHARSIZE_MAX];
   char16_t uniChar;    /* one output UNICODE char */
   స్థిరరాశి అక్షరం * save_ఆధారం; /* beginning of current code point */
   స్థిరరాశి అక్షరం * p_ప్రారంభం_LMBCS = వాదనలు_2->ఆధారం;  /* beginning of whole string */
   స్థిరరాశి అక్షరం * err_ఆధారం = శూన్యసూచిక; /* pointer to actual input in case an error occurs */
   int8_t savebytes = 0;

   /* Process from source to limit, or until error */
   అయితే (U_SUCCESS(*err) && వాదనలు_2->ఆధారం_హద్దు > వాదనలు_2->ఆధారం && వాదనలు_2->లక్ష్యం_హద్దు > వాదనలు_2->లక్ష్యం)
   {
      save_ఆధారం = వాదనలు_2->ఆధారం; /* beginning of current code point */

      ఒకవేళ (వాదనలు_2->converter->గమ్యం_ULength) /* reassemble char from previous call */
      {
        స్థిరరాశి అక్షరం *save_ఆధారం_హద్దు; 
        size_t కొలత_old = వాదనలు_2->converter->గమ్యం_ULength;

         /* limit from source is either remainder of temp buffer, or user limit on source */
        size_t కొలత_new_maybe_1 = పరిమాణం(LMBCS) - కొలత_old;
        size_t కొలత_new_maybe_2 = వాదనలు_2->ఆధారం_హద్దు - వాదనలు_2->ఆధారం;
        size_t కొలత_new = (కొలత_new_maybe_1 < కొలత_new_maybe_2) ? కొలత_new_maybe_1 : కొలత_new_maybe_2;
         
      
        uprv_memcpy(LMBCS, వాదనలు_2->converter->గమ్యం_UBytes, కొలత_old);
        uprv_memcpy(LMBCS + కొలత_old, వాదనలు_2->ఆధారం, కొలత_new);
        save_ఆధారం_హద్దు = వాదనలు_2->ఆధారం_హద్దు;
        వాదనలు_2->ఆధారం = err_ఆధారం = LMBCS;
        వాదనలు_2->ఆధారం_హద్దు = LMBCS+కొలత_old+కొలత_new;
        savebytes = (int8_t)(కొలత_old+కొలత_new);
        uniChar = (char16_t) LMBCSGet_తదుపరి_UChar_కార్మికుడు(వాదనలు_2, err);
        వాదనలు_2->ఆధారం = save_ఆధారం + ((వాదనలు_2->ఆధారం - LMBCS) - కొలత_old);
        వాదనలు_2->ఆధారం_హద్దు = save_ఆధారం_హద్దు;

        ఒకవేళ (*err == U_TRUNCATED_CHAR_దొరికింది)
        {
            /* evil special case: source buffers so small a char spans more than 2 buffers */
            వాదనలు_2->converter->గమ్యం_ULength = savebytes;
            uprv_memcpy(వాదనలు_2->converter->గమ్యం_UBytes, LMBCS, savebytes);
            వాదనలు_2->ఆధారం = వాదనలు_2->ఆధారం_హద్దు;
            *err = U_ZERO_ERROR;
            తిరిగి;
         }
         లేకపోతే
         {
            /* clear the partial-char marker */
            వాదనలు_2->converter->గమ్యం_ULength = 0;
         }
      }
      లేకపోతే
      {
         err_ఆధారం = save_ఆధారం;
         uniChar = (char16_t) LMBCSGet_తదుపరి_UChar_కార్మికుడు(వాదనలు_2, err);
         savebytes = (int8_t)(వాదనలు_2->ఆధారం - save_ఆధారం);
      }
      ఒకవేళ (U_SUCCESS(*err))
      {
         ఒకవేళ (uniChar < 0xfffe)
         {
            *(వాదనలు_2->లక్ష్యం)++ = uniChar;
            ఒకవేళ(వాదనలు_2->offsets)
            {
               *(వాదనలు_2->offsets)++ = (int32_t)(save_ఆధారం - p_ప్రారంభం_LMBCS);
            }
         }
         లేకపోతే ఒకవేళ (uniChar == 0xfffe)
         {
            *err = U_INVALID_CHAR_దొరికింది;
         }
         లేకపోతే /* if (uniChar == 0xffff) */
         {
            *err = U_ILLEGAL_CHAR_దొరికింది;
         }
      }
   }
   /* if target ran out before source, return U_BUFFER_OVERFLOW_ERROR */
   ఒకవేళ (U_SUCCESS(*err) && వాదనలు_2->ఆధారం_హద్దు > వాదనలు_2->ఆధారం && వాదనలు_2->లక్ష్యం_హద్దు <= వాదనలు_2->లక్ష్యం)
   {
      *err = U_బఫరు_అధికప్రవాహం_ERROR;
   }
   లేకపోతే ఒకవేళ (U_FAILURE(*err)) 
   {
      /* If character incomplete or unmappable/illegal, store it in toUBytes[] */
      వాదనలు_2->converter->గమ్యం_ULength = savebytes;
      ఒకవేళ (savebytes > 0) {
         uprv_memcpy(వాదనలు_2->converter->గమ్యం_UBytes, err_ఆధారం, savebytes);
      }
      ఒకవేళ (*err == U_TRUNCATED_CHAR_దొరికింది) {
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


DECLARE_LMBCS_దత్తాంశం(1)
DECLARE_LMBCS_దత్తాంశం(2)
DECLARE_LMBCS_దత్తాంశం(3)
DECLARE_LMBCS_దత్తాంశం(4)
DECLARE_LMBCS_దత్తాంశం(5)
DECLARE_LMBCS_దత్తాంశం(6)
DECLARE_LMBCS_దత్తాంశం(8)
DECLARE_LMBCS_దత్తాంశం(11)
DECLARE_LMBCS_దత్తాంశం(16)
DECLARE_LMBCS_దత్తాంశం(17)
DECLARE_LMBCS_దత్తాంశం(18)
DECLARE_LMBCS_దత్తాంశం(19)

U_CDECL_చివర

#అయితే_ముగింపు /* #if !UCONFIG_NO_LEGACY_CONVERSION */
