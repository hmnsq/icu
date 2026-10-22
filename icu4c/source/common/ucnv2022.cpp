// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
*   Copyright (C) 2000-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*   file name:  ucnv2022.cpp
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2000feb03
*   created by: Markus W. Scherer
*
*   Change history:
*
*   06/29/2000  helena  Major rewrite of the callback APIs.
*   08/08/2000  Ram     Included support for ISO-2022-JP-2
*                       Changed implementation of toUnicode
*                       function
*   08/21/2000  Ram     Added support for ISO-2022-KR
*   08/29/2000  Ram     Seperated implementation of EBCDIC to
*                       ucnvebdc.c
*   09/20/2000  Ram     Added support for ISO-2022-CN
*                       Added implementations for getNextUChar()
*                       for specific 2022 country variants.
*   10/31/2000  Ram     Implemented offsets logic functions
*/

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಆದರೆ !UCONFIG_NO_CONVERSION && !UCONFIG_NO_LEGACY_CONVERSION

#ಅನ್ತರ್ಭಾವ "unicode/ucnv.h"
#ಅನ್ತರ್ಭಾವ "unicode/uset.h"
#ಅನ್ತರ್ಭಾವ "unicode/ucnv_err.h"
#ಅನ್ತರ್ಭಾವ "unicode/ucnv_cb.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf16.h"
#ಅನ್ತರ್ಭಾವ "ucnv_imp.h"
#ಅನ್ತರ್ಭಾವ "ucnv_bld.h"
#ಅನ್ತರ್ಭಾವ "ucnv_cnv.h"
#ಅನ್ತರ್ಭಾವ "ucnvmbcs.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ U_ENABLE_GENERIC_ISO_2022
/*
 * I am disabling the generic ISO-2022 converter after proposing to do so on
 * the icu mailing list two days ago.
 *
 * Reasons:
 * 1. It does not fully support the ISO-2022/ECMA-35 specification with all of
 *    its designation sequences, single shifts with return to the previous state,
 *    switch-with-no-return to UTF-16BE or similar, etc.
 *    This is unlike the language-specific variants like ISO-2022-JP which
 *    require a much smaller repertoire of ISO-2022 features.
 *    These variants continue to be supported.
 * 2. I believe that no one is really using the generic ISO-2022 converter
 *    but rather always one of the language-specific variants.
 *    Note that ICU's generic ISO-2022 converter has always output one escape
 *    sequence followed by UTF-8 for the whole stream.
 * 3. Switching between subcharsets is extremely slow, because each time
 *    the previous converter is closed and a new one opened,
 *    without any kind of caching, least-recently-used list, etc.
 * 4. The code is currently buggy, and given the above it does not seem
 *    reasonable to spend the time on maintenance.
 * 5. ISO-2022 subcharsets should normally be used with 7-bit byte encodings.
 *    This means, for example, that when ISO-8859-7 is designated, the following
 *    ISO-2022 bytes 00..7f should be interpreted as ISO-8859-7 bytes 80..ff.
 *    The ICU ISO-2022 converter does not handle this - and has no information
 *    about which subconverter would have to be shifted vs. which is designed
 *    for 7-bit ISO-2022.
 *
 * Markus Scherer 2003-dec-03
 */
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ !UCONFIG_ONLY_HTML_CONVERSION
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ ಸರಿಸು_ಒಳ_STR[]  = "\x0F";
// static const char SHIFT_OUT_STR[] = "\x0E";
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು CR      0x0D
#ವ್ಯಾಖ್ಯಾನಿಸು LF      0x0A
#ವ್ಯಾಖ್ಯಾನಿಸು H_TAB   0x09
#ವ್ಯಾಖ್ಯಾನಿಸು V_TAB   0x0B
#ವ್ಯಾಖ್ಯಾನಿಸು SPACE   0x20

ಎಣಿಕೆ {
    HWKANA_ಆರಂಭ=0xff61,
    HWKANA_ಕೊನೆ=0xff9f
};

/*
 * 94-character sets with native byte values A1..FE are encoded in ISO 2022
 * as bytes 21..7E. (Subtract 0x80.)
 * 96-character sets with native byte values A0..FF are encoded in ISO 2022
 * as bytes 20..7F. (Subtract 0x80.)
 * Do not encode C1 control codes with native bytes 80..9F
 * as bytes 00..1F (C0 control codes).
 */
ಎಣಿಕೆ {
    GR94_ಆರಂಭ=0xa1,
    GR94_ಕೊನೆ=0xfe,
    GR96_ಆರಂಭ=0xa0,
    GR96_ಕೊನೆ=0xff
};

/*
 * ISO 2022 control codes must not be converted from Unicode
 * because they would mess up the byte stream.
 * The bit mask 0x0800c000 has bits set at bit positions 0xe, 0xf, 0x1b
 * corresponding to SO, SI, and ESC.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು IS_2022_CONTROL(c) (((c)<0x20) && (((uint32_t)1<<(c))&0x0800c000)!=0)

/* for ISO-2022-JP and -CN implementations */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ  {
        /* shared values */
        INVALID_STATE=-1,
        ASCII = 0,

        SS2_STATE=0x10,
        SS3_STATE,

        /* JP */
        ISO8859_1 = 1 ,
        ISO8859_7 = 2 ,
        JISX201  = 3,
        JISX208 = 4,
        JISX212 = 5,
        GB2312  =6,
        KSC5601 =7,
        HWKANA_7_ಬಿಟ್=8,    /* Halfwidth Katakana 7 bit */

        /* CN */
        /* the first few enum constants must keep their values because they correspond to myConverterArray[] */
        GB2312_1=1,
        ISO_IR_165=2,
        CNS_11643=3,

        /*
         * these are used in StateEnum and ISO2022State variables,
         * but CNS_11643 must be used to index into myConverterArray[]
         */
        CNS_11643_0=0x20,
        CNS_11643_1,
        CNS_11643_2,
        CNS_11643_3,
        CNS_11643_4,
        CNS_11643_5,
        CNS_11643_6,
        CNS_11643_7
} StateEnum;

/* is the StateEnum charset value for a DBCS charset? */
#ಆದರೆ UCONFIG_ONLY_HTML_CONVERSION
#ವ್ಯಾಖ್ಯಾನಿಸು IS_JP_DBCS(cs) (JISX208==(cs))
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು IS_JP_DBCS(cs) (JISX208<=(cs) && (cs)<=KSC5601)
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು CSM(cs) ((uint16_t)1<<(cs))

/*
 * Each of these charset masks (with index x) contains a bit for a charset in exact correspondence
 * to whether that charset is used in the corresponding version x of ISO_2022,locale=ja,version=x
 *
 * Note: The converter uses some leniency:
 * - The escape sequence ESC ( I for half-width 7-bit Katakana is recognized in
 *   all versions, not just JIS7 and JIS8.
 * - ICU does not distinguish between different versions of JIS X 0208.
 */
#ಆದರೆ UCONFIG_ONLY_HTML_CONVERSION
ಎಣಿಕೆ { MAX_JA_ಆವೃತ್ತಿ=0 };
#ಇಲ್ಲದಿದ್ದರೆ
ಎಣಿಕೆ { MAX_JA_ಆವೃತ್ತಿ=4 };
#ಆದರೆ_ಅಂತ್ಯ
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ uint16_t jpCharsetMasks[MAX_JA_ಆವೃತ್ತಿ+1]={
    CSM(ASCII)|CSM(JISX201)|CSM(JISX208)|CSM(HWKANA_7_ಬಿಟ್),
#ಆದರೆ !UCONFIG_ONLY_HTML_CONVERSION
    CSM(ASCII)|CSM(JISX201)|CSM(JISX208)|CSM(HWKANA_7_ಬಿಟ್)|CSM(JISX212),
    CSM(ASCII)|CSM(JISX201)|CSM(JISX208)|CSM(HWKANA_7_ಬಿಟ್)|CSM(JISX212)|CSM(GB2312)|CSM(KSC5601)|CSM(ISO8859_1)|CSM(ISO8859_7),
    CSM(ASCII)|CSM(JISX201)|CSM(JISX208)|CSM(HWKANA_7_ಬಿಟ್)|CSM(JISX212)|CSM(GB2312)|CSM(KSC5601)|CSM(ISO8859_1)|CSM(ISO8859_7),
    CSM(ASCII)|CSM(JISX201)|CSM(JISX208)|CSM(HWKANA_7_ಬಿಟ್)|CSM(JISX212)|CSM(GB2312)|CSM(KSC5601)|CSM(ISO8859_1)|CSM(ISO8859_7)
#ಆದರೆ_ಅಂತ್ಯ
};

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
        ASCII1=0,
        LATIN1,
        SBCS,
        DBCS,
        MBCS,
        HWKANA
}Cnv2022Type;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ ISO2022State {
    int8_t cs[4];       /* charset number for SI (G0)/SO (G1)/SS2 (G2)/SS3 (G3) */
    int8_t g;           /* 0..3 for G0..G3 (SI/SO/SS2/SS3) */
    int8_t prevG;       /* g before single shift (SS2 or SS3) */
} ISO2022State;

#ವ್ಯಾಖ್ಯಾನಿಸು UCNV_OPTIONS_ಆವೃತ್ತಿ_ಮುಖವಾಡ 0xf
#ವ್ಯಾಖ್ಯಾನಿಸು UCNV_2022_MAX_CONVERTERS 10

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ{
    UConverter_Shared_ದತ್ತಾಂಶ *myConverterArray[UCNV_2022_MAX_CONVERTERS];
    UConverter *currentConverter;
    Cnv2022Type currentType;
    ISO2022State ಗಮ್ಯ_U2022_State, fromU2022State;
    uint32_t ಕೀಲಿ;
    uint32_t ಆವೃತ್ತಿ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ U_ENABLE_GENERIC_ISO_2022
    UBool is_ಮೊದಲ_ಬಫರ್;
#ಆದರೆ_ಅಂತ್ಯ
    UBool is_ಖಾಲಿಗಳು_Segment;
    ಅಕ್ಷರ name[30];
    ಅಕ್ಷರ locale[3];
}UConverter_ದತ್ತಾಂಶ_ISO2022;

/* Protos */
/* ISO-2022 ----------------------------------------------------------------- */

/*Forward declaration */
U_CFUNC ಶೂನ್ಯ U_CALLCONV
ucnv_fromUnicode_UTF8(UConverter_From_Unicode_ವಾದಗಳು * ವಾದಗಳು_2,
                      UError_ಸಂಕೇತ * err);
U_CFUNC ಶೂನ್ಯ U_CALLCONV
ucnv_fromUnicode_UTF8_OFFSETS_LOGIC(UConverter_From_Unicode_ವಾದಗಳು * ವಾದಗಳು_2,
                                    UError_ಸಂಕೇತ * err);

#ವ್ಯಾಖ್ಯಾನಿಸು ESC_2022 0x1B /*ESC*/

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ
{
        INVALID_2022 = -1, /*Doesn't correspond to a valid iso 2022 escape sequence*/
        VALID_NON_TERMINAL_2022 = 0, /*so far corresponds to a valid iso 2022 escape sequence*/
        VALID_TERMINAL_2022 = 1, /*corresponds to a valid iso 2022 escape sequence*/
        VALID_MAYBE_TERMINAL_2022 = 2 /*so far matches one iso 2022 escape sequence, but by adding more characters might match another escape sequence*/
} UCNV_ಕೋಷ್ಟಕ_ಸ್ಥಿತಿಗಳು_2022;

/*
* The way these state transition arrays work is:
* ex : ESC$B is the sequence for JISX208
*      a) First Iteration: char is ESC
*          i) Get the value of ESC from normalize_esq_chars_2022[] with int value of ESC as index
*             int x = normalize_esq_chars_2022[27] which is equal to 1
*         ii) Search for this value in escSeqStateTable_Key_2022[]
*             value of x is stored at escSeqStateTable_Key_2022[0]
*        iii) Save this index as offset
*         iv) Get state of this sequence from escSeqStateTable_Value_2022[]
*             escSeqStateTable_Value_2022[offset], which is VALID_NON_TERMINAL_2022
*     b) Switch on this state and continue to next char
*          i) Get the value of $ from normalize_esq_chars_2022[] with int value of $ as index
*             which is normalize_esq_chars_2022[36] == 4
*         ii) x is currently 1(from above)
*               x<<=5 -- x is now 32
*               x+=normalize_esq_chars_2022[36]
*               now x is 36
*        iii) Search for this value in escSeqStateTable_Key_2022[]
*             value of x is stored at escSeqStateTable_Key_2022[2], so offset is 2
*         iv) Get state of this sequence from escSeqStateTable_Value_2022[]
*             escSeqStateTable_Value_2022[offset], which is VALID_NON_TERMINAL_2022
*     c) Switch on this state and continue to next char
*        i)  Get the value of B from normalize_esq_chars_2022[] with int value of B as index
*        ii) x is currently 36 (from above)
*            x<<=5 -- x is now 1152
*            x+=normalize_esq_chars_2022[66]
*            now x is 1161
*       iii) Search for this value in escSeqStateTable_Key_2022[]
*            value of x is stored at escSeqStateTable_Key_2022[21], so offset is 21
*        iv) Get state of this sequence from escSeqStateTable_Value_2022[21]
*            escSeqStateTable_Value_2022[offset], which is VALID_TERMINAL_2022
*         v) Get the converter name form escSeqStateTable_Result_2022[21] which is JISX208
*/


/*Below are the 3 arrays depicting a state transition table*/
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ int8_t normalize_esq_chars_2022[256] = {
/*       0      1       2       3       4      5       6        7       8       9           */

         0     ,0      ,0      ,0      ,0      ,0      ,0      ,0      ,0      ,0
        ,0     ,0      ,0      ,0      ,0      ,0      ,0      ,0      ,0      ,0
        ,0     ,0      ,0      ,0      ,0      ,0      ,0      ,1      ,0      ,0
        ,0     ,0      ,0      ,0      ,0      ,0      ,4      ,7      ,29      ,0
        ,2     ,24     ,26     ,27     ,0      ,3      ,23     ,6      ,0      ,0
        ,0     ,0      ,0      ,0      ,0      ,0      ,0      ,0      ,0      ,0
        ,0     ,0      ,0      ,0      ,5      ,8      ,9      ,10     ,11     ,12
        ,13    ,14     ,15     ,16     ,17     ,18     ,19     ,20     ,25     ,28
        ,0     ,0      ,21     ,0      ,0      ,0      ,0      ,0      ,0      ,0
        ,22    ,0      ,0      ,0      ,0      ,0      ,0      ,0      ,0      ,0
        ,0     ,0      ,0      ,0      ,0      ,0      ,0      ,0      ,0      ,0
        ,0     ,0      ,0      ,0      ,0      ,0      ,0      ,0      ,0      ,0
        ,0     ,0      ,0      ,0      ,0      ,0      ,0      ,0      ,0      ,0
        ,0     ,0      ,0      ,0      ,0      ,0      ,0      ,0      ,0      ,0
        ,0     ,0      ,0      ,0      ,0      ,0      ,0      ,0      ,0      ,0
        ,0     ,0      ,0      ,0      ,0      ,0      ,0      ,0      ,0      ,0
        ,0     ,0      ,0      ,0      ,0      ,0      ,0      ,0      ,0      ,0
        ,0     ,0      ,0      ,0      ,0      ,0      ,0      ,0      ,0      ,0
        ,0     ,0      ,0      ,0      ,0      ,0      ,0      ,0      ,0      ,0
        ,0     ,0      ,0      ,0      ,0      ,0      ,0      ,0      ,0      ,0
        ,0     ,0      ,0      ,0      ,0      ,0      ,0      ,0      ,0      ,0
        ,0     ,0      ,0      ,0      ,0      ,0      ,0      ,0      ,0      ,0
        ,0     ,0      ,0      ,0      ,0      ,0      ,0      ,0      ,0      ,0
        ,0     ,0      ,0      ,0      ,0      ,0      ,0      ,0      ,0      ,0
        ,0     ,0      ,0      ,0      ,0      ,0      ,0      ,0      ,0      ,0
        ,0     ,0      ,0      ,0      ,0      ,0
};

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ U_ENABLE_GENERIC_ISO_2022
/*
 * When the generic ISO-2022 converter is completely removed, not just disabled
 * per #ifdef, then the following state table and the associated tables that are
 * dimensioned with MAX_STATES_2022 should be trimmed.
 *
 * Especially, VALID_MAYBE_TERMINAL_2022 will not be used any more, and all of
 * the associated escape sequences starting with ESC ( B should be removed.
 * This includes the ones with key values 1097 and all of the ones above 1000000.
 *
 * For the latter, the tables can simply be truncated.
 * For the former, since the tables must be kept parallel, it is probably best
 * to simply duplicate an adjacent table cell, parallel in all tables.
 *
 * It may make sense to restructure the tables, especially by using small search
 * tables for the variants instead of indexing them parallel to the table here.
 */
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು MAX_ಸ್ಥಿತಿಗಳು_2022 74
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ int32_t esc_Seq_State_ಕೋಷ್ಟಕ_ಕೀಲಿ_2022[MAX_ಸ್ಥಿತಿಗಳು_2022] = {
/*   0           1           2           3           4           5           6           7           8           9           */

     1          ,34         ,36         ,39         ,55         ,57         ,60         ,61         ,1093       ,1096
    ,1097       ,1098       ,1099       ,1100       ,1101       ,1102       ,1103       ,1104       ,1105       ,1106
    ,1109       ,1154       ,1157       ,1160       ,1161       ,1176       ,1178       ,1179       ,1254       ,1257
    ,1768       ,1773       ,1957       ,35105      ,36933      ,36936      ,36937      ,36938      ,36939      ,36940
    ,36942      ,36943      ,36944      ,36945      ,36946      ,36947      ,36948      ,37640      ,37642      ,37644
    ,37646      ,37711      ,37744      ,37745      ,37746      ,37747      ,37748      ,40133      ,40136      ,40138
    ,40139      ,40140      ,40141      ,1123363    ,35947624   ,35947625   ,35947626   ,35947627   ,35947629   ,35947630
    ,35947631   ,35947635   ,35947636   ,35947638
};

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ U_ENABLE_GENERIC_ISO_2022

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಸ್ಥಿರಾಂಕ esc_Seq_State_ಕೋಷ್ಟಕ_ಫಲಿತಾಂಶ_2022[MAX_ಸ್ಥಿತಿಗಳು_2022] = {
 /*  0                      1                        2                      3                   4                   5                        6                      7                       8                       9    */

     ಶೂನ್ಯಸೂಚಕ                   ,ಶೂನ್ಯಸೂಚಕ                   ,ಶೂನ್ಯಸೂಚಕ                   ,ಶೂನ್ಯಸೂಚಕ               ,ಶೂನ್ಯಸೂಚಕ               ,ಶೂನ್ಯಸೂಚಕ                   ,ಶೂನ್ಯಸೂಚಕ                   ,ಶೂನ್ಯಸೂಚಕ                   ,"latin1"               ,"latin1"
    ,"latin1"               ,"ibm-865"              ,"ibm-865"              ,"ibm-865"          ,"ibm-865"          ,"ibm-865"              ,"ibm-865"              ,"JISX0201"             ,"JISX0201"             ,"latin1"
    ,"latin1"               ,ಶೂನ್ಯಸೂಚಕ                   ,"JISX-208"             ,"ibm-5478"         ,"JISX-208"         ,ಶೂನ್ಯಸೂಚಕ                   ,ಶೂನ್ಯಸೂಚಕ                   ,ಶೂನ್ಯಸೂಚಕ                   ,ಶೂನ್ಯಸೂಚಕ                   ,"UTF8"
    ,"ISO-8859-1"           ,"ISO-8859-7"           ,"JIS-X-208"            ,ಶೂನ್ಯಸೂಚಕ               ,"ibm-955"          ,"ibm-367"              ,"ibm-952"              ,"ibm-949"              ,"JISX-212"             ,"ibm-1383"
    ,"ibm-952"              ,"ibm-964"              ,"ibm-964"              ,"ibm-964"          ,"ibm-964"          ,"ibm-964"              ,"ibm-964"              ,"ibm-5478"         ,"ibm-949"              ,"ISO-IR-165"
    ,"CNS-11643-1992,1"     ,"CNS-11643-1992,2"     ,"CNS-11643-1992,3"     ,"CNS-11643-1992,4" ,"CNS-11643-1992,5" ,"CNS-11643-1992,6"     ,"CNS-11643-1992,7"     ,"UTF16_PlatformEndian" ,"UTF16_PlatformEndian" ,"UTF16_PlatformEndian"
    ,"UTF16_PlatformEndian" ,"UTF16_PlatformEndian" ,"UTF16_PlatformEndian" ,ಶೂನ್ಯಸೂಚಕ               ,"latin1"           ,"ibm-912"              ,"ibm-913"              ,"ibm-914"              ,"ibm-813"              ,"ibm-1089"
    ,"ibm-920"              ,"ibm-915"              ,"ibm-915"              ,"latin1"
};

#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ int8_t esc_Seq_State_ಕೋಷ್ಟಕ_ಮೌಲ್ಯ_2022[MAX_ಸ್ಥಿತಿಗಳು_2022] = {
/*          0                           1                         2                             3                           4                           5                               6                        7                          8                           9       */
     VALID_NON_TERMINAL_2022    ,VALID_NON_TERMINAL_2022    ,VALID_NON_TERMINAL_2022    ,VALID_NON_TERMINAL_2022     ,VALID_NON_TERMINAL_2022   ,VALID_TERMINAL_2022        ,VALID_TERMINAL_2022        ,VALID_NON_TERMINAL_2022    ,VALID_TERMINAL_2022        ,VALID_TERMINAL_2022
    ,VALID_MAYBE_TERMINAL_2022  ,VALID_TERMINAL_2022        ,VALID_TERMINAL_2022        ,VALID_TERMINAL_2022        ,VALID_TERMINAL_2022        ,VALID_TERMINAL_2022        ,VALID_TERMINAL_2022        ,VALID_TERMINAL_2022        ,VALID_TERMINAL_2022        ,VALID_TERMINAL_2022
    ,VALID_TERMINAL_2022        ,VALID_NON_TERMINAL_2022    ,VALID_TERMINAL_2022        ,VALID_TERMINAL_2022        ,VALID_TERMINAL_2022        ,VALID_NON_TERMINAL_2022    ,VALID_NON_TERMINAL_2022    ,VALID_NON_TERMINAL_2022    ,VALID_NON_TERMINAL_2022    ,VALID_TERMINAL_2022
    ,VALID_TERMINAL_2022        ,VALID_TERMINAL_2022        ,VALID_TERMINAL_2022        ,VALID_NON_TERMINAL_2022    ,VALID_TERMINAL_2022        ,VALID_TERMINAL_2022        ,VALID_TERMINAL_2022        ,VALID_TERMINAL_2022        ,VALID_TERMINAL_2022        ,VALID_TERMINAL_2022
    ,VALID_TERMINAL_2022        ,VALID_TERMINAL_2022        ,VALID_TERMINAL_2022        ,VALID_TERMINAL_2022        ,VALID_TERMINAL_2022        ,VALID_TERMINAL_2022        ,VALID_TERMINAL_2022        ,VALID_TERMINAL_2022        ,VALID_TERMINAL_2022        ,VALID_TERMINAL_2022
    ,VALID_TERMINAL_2022        ,VALID_TERMINAL_2022        ,VALID_TERMINAL_2022        ,VALID_TERMINAL_2022        ,VALID_TERMINAL_2022        ,VALID_TERMINAL_2022        ,VALID_TERMINAL_2022        ,VALID_TERMINAL_2022        ,VALID_TERMINAL_2022        ,VALID_TERMINAL_2022
    ,VALID_TERMINAL_2022        ,VALID_TERMINAL_2022        ,VALID_TERMINAL_2022        ,VALID_NON_TERMINAL_2022    ,VALID_TERMINAL_2022        ,VALID_TERMINAL_2022        ,VALID_TERMINAL_2022        ,VALID_TERMINAL_2022        ,VALID_TERMINAL_2022        ,VALID_TERMINAL_2022
    ,VALID_TERMINAL_2022        ,VALID_TERMINAL_2022        ,VALID_TERMINAL_2022        ,VALID_TERMINAL_2022
};

/* Type def for refactoring changeState_2022 code*/
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ U_ENABLE_GENERIC_ISO_2022
    ISO_2022=0,
#ಆದರೆ_ಅಂತ್ಯ
    ISO_2022_JP=1,
#ಆದರೆ !UCONFIG_ONLY_HTML_CONVERSION
    ISO_2022_KR=2,
    ISO_2022_CN=3
#ಆದರೆ_ಅಂತ್ಯ
} Variant2022;

/*********** ISO 2022 Converter Protos ***********/
ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
_ISO2022Open(UConverter *cnv, UConverter_ತುಂಬು_ವಾದಗಳು *p_ವಾದಗಳು, UError_ಸಂಕೇತ *error_ಸಂಕೇತ);

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
 _ISO2022Close(UConverter *converter);

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
_ISO2022Reset(UConverter *converter, UConverterResetChoice choice);

U_CDECL_BEGIN
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * U_CALLCONV
_ISO2022getName(ಸ್ಥಿರಾಂಕ UConverter* cnv);
U_CDECL_ಕೊನೆ

ಸ್ಥಿರ ಶೂನ್ಯ  U_CALLCONV
_ISO_2022_WriteSub(UConverter_From_Unicode_ವಾದಗಳು *ವಾದಗಳು_2, int32_t ಸ್ಥಾನಾಂತರ_ಸೂಚ್ಯಂಕ, UError_ಸಂಕೇತ *err);

U_CDECL_BEGIN
ಸ್ಥಿರ UConverter * U_CALLCONV
ISO_2022_ಸುರಕ್ಷಿತ_Clone(ಸ್ಥಿರಾಂಕ UConverter *cnv, ಶೂನ್ಯ *ರಾಶಿ_ಬಫರ್, int32_t *p_ಬಫರ್_ಗಾತ್ರ, UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2);

U_CDECL_ಕೊನೆ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ U_ENABLE_GENERIC_ISO_2022
ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
T_UConverter_ಗಮ್ಯ_Unicode_ISO_2022_OFFSETS_LOGIC(UConverter_ಗಮ್ಯ_Unicode_ವಾದಗಳು* ವಾದಗಳು_2, UError_ಸಂಕೇತ* err);
#ಆದರೆ_ಅಂತ್ಯ

ನಾಮಾಕಾಶ {

/*const UConverterSharedData _ISO2022Data;*/
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ UConverter_Shared_ದತ್ತಾಂಶ _ISO2022JPData;

#ಆದರೆ !UCONFIG_ONLY_HTML_CONVERSION
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ UConverter_Shared_ದತ್ತಾಂಶ _ISO2022KRData;
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ UConverter_Shared_ದತ್ತಾಂಶ _ISO2022CNData;
#ಆದರೆ_ಅಂತ್ಯ

}  // namespace

/*************** Converter implementations ******************/

/* The purpose of this function is to get around gcc compiler warnings. */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ
fromUWriteUInt8(UConverter *cnv,
                 ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೈಟುಗಳು, int32_t length,
                 uint8_t **ಗುರಿ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಗುರಿ_ಎಲ್ಲೆ,
                 int32_t **offsets,
                 int32_t ಮೂಲ_ಸೂಚ್ಯಂಕ,
                 UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ)
{
    ಅಕ್ಷರ* ಗುರಿ_Chars = reinterpret_cast<ಅಕ್ಷರ*>(*ಗುರಿ);
    ucnv_from_UWrite_ಬೈಟುಗಳು(cnv, ಬೈಟುಗಳು, length, &ಗುರಿ_Chars, ಗುರಿ_ಎಲ್ಲೆ,
                         offsets, ಮೂಲ_ಸೂಚ್ಯಂಕ, p_Error_ಸಂಕೇತ);
    *ಗುರಿ = reinterpret_cast<uint8_t*>(ಗುರಿ_Chars);

}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ
ಗಣ_Initial_State_ಗಮ್ಯ_Unicode_KR(UConverter* /*converter*/, UConverter_ದತ್ತಾಂಶ_ISO2022 *my_Converter_ದತ್ತಾಂಶ){
    ಆದರೆ(my_Converter_ದತ್ತಾಂಶ->ಆವೃತ್ತಿ == 1) {
        UConverter *cnv = my_Converter_ದತ್ತಾಂಶ->currentConverter;

        cnv->ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ=0;     /*ಸ್ಥಾನಾಂತರ*/
        cnv->ಬಹುಲಕ=0;                /* state */
        cnv->ಗಮ್ಯ_ULength=0;           /* byteIndex */
    }
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ
ಗಣ_Initial_State_From_Unicode_KR(UConverter* converter,UConverter_ದತ್ತಾಂಶ_ISO2022 *my_Converter_ದತ್ತಾಂಶ){
   /* in ISO-2022-KR the designator sequence appears only once
    * in a file so we append it only once
    */
    ಆದರೆ( converter->char_Error_ಬಫರ್_ಉದ್ದಳತೆ==0){

        converter->char_Error_ಬಫರ್_ಉದ್ದಳತೆ = 4;
        converter->char_Error_ಬಫರ್[0] = 0x1b;
        converter->char_Error_ಬಫರ್[1] = 0x24;
        converter->char_Error_ಬಫರ್[2] = 0x29;
        converter->char_Error_ಬಫರ್[3] = 0x43;
    }
    ಆದರೆ(my_Converter_ದತ್ತಾಂಶ->ಆವೃತ್ತಿ == 1) {
        UConverter *cnv = my_Converter_ದತ್ತಾಂಶ->currentConverter;

        cnv->fromUChar32=0;
        cnv->from_Unicode_ಸ್ಥಿತಿ=1;   /* prevLength */
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
_ISO2022Open(UConverter *cnv, UConverter_ತುಂಬು_ವಾದಗಳು *p_ವಾದಗಳು, UError_ಸಂಕೇತ *error_ಸಂಕೇತ){

    ಅಕ್ಷರ myLocale[7]={' ',' ',' ',' ',' ',' ', '\0'};

    cnv->extraInfo = uprv_malloc (ಗಾತ್ರ (UConverter_ದತ್ತಾಂಶ_ISO2022));
    ಆದರೆ(cnv->extraInfo != ಶೂನ್ಯಸೂಚಕ) {
        UConverterNamePieces ರಾಶಿ_Pieces;
        UConverter_ತುಂಬು_ವಾದಗಳು ರಾಶಿ_ವಾದಗಳು=UCNV_ತುಂಬು_ವಾದಗಳು_INITIALIZER;
        UConverter_ದತ್ತಾಂಶ_ISO2022* my_Converter_ದತ್ತಾಂಶ = static_cast<UConverter_ದತ್ತಾಂಶ_ISO2022*>(cnv->extraInfo);
        uint32_t ಆವೃತ್ತಿ;

        ರಾಶಿ_ವಾದಗಳು.onlyTestIsLoadable = p_ವಾದಗಳು->onlyTestIsLoadable;

        uprv_memset(my_Converter_ದತ್ತಾಂಶ, 0, ಗಾತ್ರ(UConverter_ದತ್ತಾಂಶ_ISO2022));
        my_Converter_ದತ್ತಾಂಶ->currentType = ASCII1;
        cnv->from_Unicode_ಸ್ಥಿತಿ =ಸುಳ್ಳು;
        ಆದರೆ(p_ವಾದಗಳು->locale){
            uprv_strncpy(myLocale, p_ವಾದಗಳು->locale, ಗಾತ್ರ(myLocale)-1);
        }
        ಆವೃತ್ತಿ = p_ವಾದಗಳು->options & UCNV_OPTIONS_ಆವೃತ್ತಿ_ಮುಖವಾಡ;
        my_Converter_ದತ್ತಾಂಶ->ಆವೃತ್ತಿ = ಆವೃತ್ತಿ;
        ಆದರೆ(myLocale[0]=='j' && (myLocale[1]=='a'|| myLocale[1]=='p') &&
            (myLocale[2]=='_' || myLocale[2]=='\0'))
        {
            /* open the required converters and cache them */
            ಆದರೆ(ಆವೃತ್ತಿ>MAX_JA_ಆವೃತ್ತಿ) {
                // ICU 55 fails to open a converter for an unsupported version.
                // Previously, it fell back to version 0, but that would yield
                // unexpected behavior.
                *error_ಸಂಕೇತ = U_MISSING_RESOURCE_ERROR;
                ಹಿಂತಿರುಗಿ;
            }
            ಆದರೆ(jpCharsetMasks[ಆವೃತ್ತಿ]&CSM(ISO8859_7)) {
                my_Converter_ದತ್ತಾಂಶ->myConverterArray[ISO8859_7] =
                    ucnv_ತುಂಬು_Shared_ದತ್ತಾಂಶ("ISO8859_7", &ರಾಶಿ_Pieces, &ರಾಶಿ_ವಾದಗಳು, error_ಸಂಕೇತ);
            }
            my_Converter_ದತ್ತಾಂಶ->myConverterArray[JISX208] =
                ucnv_ತುಂಬು_Shared_ದತ್ತಾಂಶ("Shift-JIS", &ರಾಶಿ_Pieces, &ರಾಶಿ_ವಾದಗಳು, error_ಸಂಕೇತ);
            ಆದರೆ(jpCharsetMasks[ಆವೃತ್ತಿ]&CSM(JISX212)) {
                my_Converter_ದತ್ತಾಂಶ->myConverterArray[JISX212] =
                    ucnv_ತುಂಬು_Shared_ದತ್ತಾಂಶ("jisx-212", &ರಾಶಿ_Pieces, &ರಾಶಿ_ವಾದಗಳು, error_ಸಂಕೇತ);
            }
            ಆದರೆ(jpCharsetMasks[ಆವೃತ್ತಿ]&CSM(GB2312)) {
                my_Converter_ದತ್ತಾಂಶ->myConverterArray[GB2312] =
                    ucnv_ತುಂಬು_Shared_ದತ್ತಾಂಶ("ibm-5478", &ರಾಶಿ_Pieces, &ರಾಶಿ_ವಾದಗಳು, error_ಸಂಕೇತ);   /* gb_2312_80-1 */
            }
            ಆದರೆ(jpCharsetMasks[ಆವೃತ್ತಿ]&CSM(KSC5601)) {
                my_Converter_ದತ್ತಾಂಶ->myConverterArray[KSC5601] =
                    ucnv_ತುಂಬು_Shared_ದತ್ತಾಂಶ("ksc_5601", &ರಾಶಿ_Pieces, &ರಾಶಿ_ವಾದಗಳು, error_ಸಂಕೇತ);
            }

            /* set the function pointers to appropriate functions */
            cnv->shared_ದತ್ತಾಂಶ = const_cast<UConverter_Shared_ದತ್ತಾಂಶ*>(&_ISO2022JPData);
            uprv_strcpy(my_Converter_ದತ್ತಾಂಶ->locale,"ja");

            (ಶೂನ್ಯ)uprv_strcpy(my_Converter_ದತ್ತಾಂಶ->name,"ISO_2022,locale=ja,version=");
            size_t len = uprv_strlen(my_Converter_ದತ್ತಾಂಶ->name);
            my_Converter_ದತ್ತಾಂಶ->name[len] = static_cast<ಅಕ್ಷರ>(my_Converter_ದತ್ತಾಂಶ->ಆವೃತ್ತಿ + static_cast<ಪೂರ್ಣಾಂಕ>('0'));
            my_Converter_ದತ್ತಾಂಶ->name[len+1]='\0';
        }
#ಆದರೆ !UCONFIG_ONLY_HTML_CONVERSION
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(myLocale[0]=='k' && (myLocale[1]=='o'|| myLocale[1]=='r') &&
            (myLocale[2]=='_' || myLocale[2]=='\0'))
        {
            ಆದರೆ(ಆವೃತ್ತಿ>1) {
                // ICU 55 fails to open a converter for an unsupported version.
                // Previously, it fell back to version 0, but that would yield
                // unexpected behavior.
                *error_ಸಂಕೇತ = U_MISSING_RESOURCE_ERROR;
                ಹಿಂತಿರುಗಿ;
            }
            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *cnvName;
            ಆದರೆ(ಆವೃತ್ತಿ==1) {
                cnvName="icu-internal-25546";
            } ಇಲ್ಲದಿದ್ದರೆ {
                cnvName="ibm-949";
                my_Converter_ದತ್ತಾಂಶ->ಆವೃತ್ತಿ=ಆವೃತ್ತಿ=0;
            }
            ಆದರೆ(p_ವಾದಗಳು->onlyTestIsLoadable) {
                ucnv_canCreateConverter(cnvName, error_ಸಂಕೇತ);  /* errorCode carries result */
                uprv_free(cnv->extraInfo);
                cnv->extraInfo=ಶೂನ್ಯಸೂಚಕ;
                ಹಿಂತಿರುಗಿ;
            } ಇಲ್ಲದಿದ್ದರೆ {
                my_Converter_ದತ್ತಾಂಶ->currentConverter=ucnv_open(cnvName, error_ಸಂಕೇತ);
                ಆದರೆ (U_FAILURE(*error_ಸಂಕೇತ)) {
                    _ISO2022Close(cnv);
                    ಹಿಂತಿರುಗಿ;
                }

                ಆದರೆ(ಆವೃತ್ತಿ==1) {
                    (ಶೂನ್ಯ)uprv_strcpy(my_Converter_ದತ್ತಾಂಶ->name,"ISO_2022,locale=ko,version=1");
                    uprv_memcpy(cnv->subChars, my_Converter_ದತ್ತಾಂಶ->currentConverter->subChars, 4);
                    cnv->subCharLen = my_Converter_ದತ್ತಾಂಶ->currentConverter->subCharLen;
                }ಇಲ್ಲದಿದ್ದರೆ{
                    (ಶೂನ್ಯ)uprv_strcpy(my_Converter_ದತ್ತಾಂಶ->name,"ISO_2022,locale=ko,version=0");
                }

                /* initialize the state variables */
                ಗಣ_Initial_State_ಗಮ್ಯ_Unicode_KR(cnv, my_Converter_ದತ್ತಾಂಶ);
                ಗಣ_Initial_State_From_Unicode_KR(cnv, my_Converter_ದತ್ತಾಂಶ);

                /* set the function pointers to appropriate functions */
                cnv->shared_ದತ್ತಾಂಶ = const_cast<UConverter_Shared_ದತ್ತಾಂಶ*>(&_ISO2022KRData);
                uprv_strcpy(my_Converter_ದತ್ತಾಂಶ->locale,"ko");
            }
        }
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(((myLocale[0]=='z' && myLocale[1]=='h') || (myLocale[0]=='c'&& myLocale[1]=='n'))&&
            (myLocale[2]=='_' || myLocale[2]=='\0'))
        {
            ಆದರೆ(ಆವೃತ್ತಿ>2) {
                // ICU 55 fails to open a converter for an unsupported version.
                // Previously, it fell back to version 0, but that would yield
                // unexpected behavior.
                *error_ಸಂಕೇತ = U_MISSING_RESOURCE_ERROR;
                ಹಿಂತಿರುಗಿ;
            }

            /* open the required converters and cache them */
            my_Converter_ದತ್ತಾಂಶ->myConverterArray[GB2312_1] =
                ucnv_ತುಂಬು_Shared_ದತ್ತಾಂಶ("ibm-5478", &ರಾಶಿ_Pieces, &ರಾಶಿ_ವಾದಗಳು, error_ಸಂಕೇತ);
            ಆದರೆ(ಆವೃತ್ತಿ>=1) {
                my_Converter_ದತ್ತಾಂಶ->myConverterArray[ISO_IR_165] =
                    ucnv_ತುಂಬು_Shared_ದತ್ತಾಂಶ("iso-ir-165", &ರಾಶಿ_Pieces, &ರಾಶಿ_ವಾದಗಳು, error_ಸಂಕೇತ);
            }
            my_Converter_ದತ್ತಾಂಶ->myConverterArray[CNS_11643] =
                ucnv_ತುಂಬು_Shared_ದತ್ತಾಂಶ("cns-11643-1992", &ರಾಶಿ_Pieces, &ರಾಶಿ_ವಾದಗಳು, error_ಸಂಕೇತ);


            /* set the function pointers to appropriate functions */
            cnv->shared_ದತ್ತಾಂಶ = const_cast<UConverter_Shared_ದತ್ತಾಂಶ*>(&_ISO2022CNData);
            uprv_strcpy(my_Converter_ದತ್ತಾಂಶ->locale,"cn");

            ಆದರೆ (ಆವೃತ್ತಿ==0){
                my_Converter_ದತ್ತಾಂಶ->ಆವೃತ್ತಿ = 0;
                (ಶೂನ್ಯ)uprv_strcpy(my_Converter_ದತ್ತಾಂಶ->name,"ISO_2022,locale=zh,version=0");
            }ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಆವೃತ್ತಿ==1){
                my_Converter_ದತ್ತಾಂಶ->ಆವೃತ್ತಿ = 1;
                (ಶೂನ್ಯ)uprv_strcpy(my_Converter_ದತ್ತಾಂಶ->name,"ISO_2022,locale=zh,version=1");
            }ಇಲ್ಲದಿದ್ದರೆ {
                my_Converter_ದತ್ತಾಂಶ->ಆವೃತ್ತಿ = 2;
                (ಶೂನ್ಯ)uprv_strcpy(my_Converter_ದತ್ತಾಂಶ->name,"ISO_2022,locale=zh,version=2");
            }
        }
#ಆದರೆ_ಅಂತ್ಯ  // !UCONFIG_ONLY_HTML_CONVERSION
        ಇಲ್ಲದಿದ್ದರೆ{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ U_ENABLE_GENERIC_ISO_2022
            my_Converter_ದತ್ತಾಂಶ->is_ಮೊದಲ_ಬಫರ್ = ನಿಜ;

            /* append the UTF-8 escape sequence */
            cnv->char_Error_ಬಫರ್_ಉದ್ದಳತೆ = 3;
            cnv->char_Error_ಬಫರ್[0] = 0x1b;
            cnv->char_Error_ಬಫರ್[1] = 0x25;
            cnv->char_Error_ಬಫರ್[2] = 0x42;

            cnv->shared_ದತ್ತಾಂಶ=(UConverter_Shared_ದತ್ತಾಂಶ*)&ISO2022_ದತ್ತಾಂಶ;
            /* initialize the state variables */
            uprv_strcpy(my_Converter_ದತ್ತಾಂಶ->name,"ISO_2022");
#ಇಲ್ಲದಿದ್ದರೆ
            *error_ಸಂಕೇತ = U_MISSING_RESOURCE_ERROR;
            // Was U_UNSUPPORTED_ERROR but changed in ICU 55 to a more standard
            // data loading error code.
            ಹಿಂತಿರುಗಿ;
#ಆದರೆ_ಅಂತ್ಯ
        }

        cnv->max_ಬೈಟುಗಳು_ಪ್ರತಿ_UChar=cnv->shared_ದತ್ತಾಂಶ->static_ದತ್ತಾಂಶ->max_ಬೈಟುಗಳು_ಪ್ರತಿ_Char;

        ಆದರೆ(U_FAILURE(*error_ಸಂಕೇತ) || p_ವಾದಗಳು->onlyTestIsLoadable) {
            _ISO2022Close(cnv);
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        *error_ಸಂಕೇತ = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
    }
}


ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
_ISO2022Close(UConverter *converter) {
    UConverter_ದತ್ತಾಂಶ_ISO2022* my_ದತ್ತಾಂಶ = static_cast<UConverter_ದತ್ತಾಂಶ_ISO2022*>(converter->extraInfo);
    UConverter_Shared_ದತ್ತಾಂಶ **array = my_ದತ್ತಾಂಶ->myConverterArray;
    int32_t i;

    ಆದರೆ (converter->extraInfo != ಶೂನ್ಯಸೂಚಕ) {
        /*close the array of converter pointers and free the memory*/
        ಗಾಗಿ (i=0; i<UCNV_2022_MAX_CONVERTERS; i++) {
            ಆದರೆ(array[i]!=ಶೂನ್ಯಸೂಚಕ) {
                ucnv_unload_Shared_ದತ್ತಾಂಶ_If_Ready(array[i]);
            }
        }

        ucnv_close(my_ದತ್ತಾಂಶ->currentConverter);

        ಆದರೆ(!converter->isExtraLocal){
            uprv_free (converter->extraInfo);
            converter->extraInfo = ಶೂನ್ಯಸೂಚಕ;
        }
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
_ISO2022Reset(UConverter *converter, UConverterResetChoice choice) {
    UConverter_ದತ್ತಾಂಶ_ISO2022* my_Converter_ದತ್ತಾಂಶ = static_cast<UConverter_ದತ್ತಾಂಶ_ISO2022*>(converter->extraInfo);
    ಆದರೆ(choice<=UCNV_RESET_ಗಮ್ಯ_UNICODE) {
        uprv_memset(&my_Converter_ದತ್ತಾಂಶ->ಗಮ್ಯ_U2022_State, 0, ಗಾತ್ರ(ISO2022State));
        my_Converter_ದತ್ತಾಂಶ->ಕೀಲಿ = 0;
        my_Converter_ದತ್ತಾಂಶ->is_ಖಾಲಿಗಳು_Segment = ಸುಳ್ಳು;
    }
    ಆದರೆ(choice!=UCNV_RESET_ಗಮ್ಯ_UNICODE) {
        uprv_memset(&my_Converter_ದತ್ತಾಂಶ->fromU2022State, 0, ಗಾತ್ರ(ISO2022State));
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ U_ENABLE_GENERIC_ISO_2022
    ಆದರೆ(my_Converter_ದತ್ತಾಂಶ->locale[0] == 0){
        ಆದರೆ(choice<=UCNV_RESET_ಗಮ್ಯ_UNICODE) {
            my_Converter_ದತ್ತಾಂಶ->is_ಮೊದಲ_ಬಫರ್ = ನಿಜ;
            my_Converter_ದತ್ತಾಂಶ->ಕೀಲಿ = 0;
            ಆದರೆ (converter->ಬಹುಲಕ == UCNV_SO){
                ucnv_close (my_Converter_ದತ್ತಾಂಶ->currentConverter);
                my_Converter_ದತ್ತಾಂಶ->currentConverter=ಶೂನ್ಯಸೂಚಕ;
            }
            converter->ಬಹುಲಕ = UCNV_SI;
        }
        ಆದರೆ(choice!=UCNV_RESET_ಗಮ್ಯ_UNICODE) {
            /* re-append UTF-8 escape sequence */
            converter->char_Error_ಬಫರ್_ಉದ್ದಳತೆ = 3;
            converter->char_Error_ಬಫರ್[0] = 0x1b;
            converter->char_Error_ಬಫರ್[1] = 0x28;
            converter->char_Error_ಬಫರ್[2] = 0x42;
        }
    }
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
    {
        /* reset the state variables */
        ಆದರೆ(my_Converter_ದತ್ತಾಂಶ->locale[0] == 'k'){
            ಆದರೆ(choice<=UCNV_RESET_ಗಮ್ಯ_UNICODE) {
                ಗಣ_Initial_State_ಗಮ್ಯ_Unicode_KR(converter, my_Converter_ದತ್ತಾಂಶ);
            }
            ಆದರೆ(choice!=UCNV_RESET_ಗಮ್ಯ_UNICODE) {
                ಗಣ_Initial_State_From_Unicode_KR(converter, my_Converter_ದತ್ತಾಂಶ);
            }
        }
    }
}

U_CDECL_BEGIN

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * U_CALLCONV
_ISO2022getName(ಸ್ಥಿರಾಂಕ UConverter* cnv){
    ಆದರೆ(cnv->extraInfo){
        UConverter_ದತ್ತಾಂಶ_ISO2022* my_ದತ್ತಾಂಶ= (UConverter_ದತ್ತಾಂಶ_ISO2022*)cnv->extraInfo;
        ಹಿಂತಿರುಗಿ my_ದತ್ತಾಂಶ->name;
    }
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
}

U_CDECL_ಕೊನೆ


/*************** to unicode *******************/
/****************************************************************************
 * Recognized escape sequences are
 * <ESC>(B  ASCII
 * <ESC>.A  ISO-8859-1
 * <ESC>.F  ISO-8859-7
 * <ESC>(J  JISX-201
 * <ESC>(I  JISX-201
 * <ESC>$B  JISX-208
 * <ESC>$@  JISX-208
 * <ESC>$(D JISX-212
 * <ESC>$A  GB2312
 * <ESC>$(C KSC5601
 */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ int8_t ಮುಂದಿನ_State_ಗಮ್ಯ_Unicode_JP[MAX_ಸ್ಥಿತಿಗಳು_2022]= {
/*      0                1               2               3               4               5               6               7               8               9    */
    INVALID_STATE   ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,SS2_STATE      ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE
    ,ASCII          ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,JISX201        ,HWKANA_7_ಬಿಟ್    ,JISX201        ,INVALID_STATE
    ,INVALID_STATE  ,INVALID_STATE  ,JISX208        ,GB2312         ,JISX208        ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE
    ,ISO8859_1      ,ISO8859_7      ,JISX208        ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,KSC5601        ,JISX212        ,INVALID_STATE
    ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE
    ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE
    ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE
    ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE
};

#ಆದರೆ !UCONFIG_ONLY_HTML_CONVERSION
/*************** to unicode *******************/
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ int8_t ಮುಂದಿನ_State_ಗಮ್ಯ_Unicode_CN[MAX_ಸ್ಥಿತಿಗಳು_2022]= {
/*      0                1               2               3               4               5               6               7               8               9    */
     INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,SS2_STATE      ,SS3_STATE      ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE
    ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE
    ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE
    ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE
    ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,GB2312_1       ,INVALID_STATE  ,ISO_IR_165
    ,CNS_11643_1    ,CNS_11643_2    ,CNS_11643_3    ,CNS_11643_4    ,CNS_11643_5    ,CNS_11643_6    ,CNS_11643_7    ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE
    ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE
    ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE
};
#ಆದರೆ_ಅಂತ್ಯ


ಸ್ಥಿರ UCNV_ಕೋಷ್ಟಕ_ಸ್ಥಿತಿಗಳು_2022
get_ಕೀಲಿ_2022(ಅಕ್ಷರ c,int32_t* ಕೀಲಿ,int32_t* ಸ್ಥಾನಾಂತರ){
    int32_t togo;
    int32_t ಕೆಳಗಿನ = 0;
    int32_t hi = MAX_ಸ್ಥಿತಿಗಳು_2022;
    int32_t oldmid=0;

    togo = normalize_esq_chars_2022[static_cast<uint8_t>(c)];
    ಆದರೆ(togo == 0) {
        /* not a valid character anywhere in an escape sequence */
        *ಕೀಲಿ = 0;
        *ಸ್ಥಾನಾಂತರ = 0;
        ಹಿಂತಿರುಗಿ INVALID_2022;
    }
    togo = (*ಕೀಲಿ << 5) + togo;

    ಆಗ (hi != ಕೆಳಗಿನ)  /*ದ್ವಿಮಾನ ಹುಡುಕು*/{

        int32_t mid = (hi+ಕೆಳಗಿನ) >> 1; /*Finds median*/

        ಆದರೆ (mid == oldmid)
            ನಿಲ್ಲಿಸು;

        ಆದರೆ (esc_Seq_State_ಕೋಷ್ಟಕ_ಕೀಲಿ_2022[mid] > togo){
            hi = mid;
        }
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (esc_Seq_State_ಕೋಷ್ಟಕ_ಕೀಲಿ_2022[mid] < togo){
            ಕೆಳಗಿನ = mid;
        }
        ಇಲ್ಲದಿದ್ದರೆ /*we found it*/{
            *ಕೀಲಿ = togo;
            *ಸ್ಥಾನಾಂತರ = mid;
            ಹಿಂತಿರುಗಿ static_cast<UCNV_ಕೋಷ್ಟಕ_ಸ್ಥಿತಿಗಳು_2022>(esc_Seq_State_ಕೋಷ್ಟಕ_ಮೌಲ್ಯ_2022[mid]);
        }
        oldmid = mid;

    }

    *ಕೀಲಿ = 0;
    *ಸ್ಥಾನಾಂತರ = 0;
    ಹಿಂತಿರುಗಿ INVALID_2022;
}

/*runs through a state machine to determine the escape sequence - codepage correspondence
 */
ಸ್ಥಿರ ಶೂನ್ಯ
changeState_2022(UConverter* _this,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ** ಮೂಲ,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಮೂಲ_ಎಲ್ಲೆ,
                Variant2022 var,
                UError_ಸಂಕೇತ* err){
    UCNV_ಕೋಷ್ಟಕ_ಸ್ಥಿತಿಗಳು_2022 value;
    UConverter_ದತ್ತಾಂಶ_ISO2022* myData2022 = static_cast<UConverter_ದತ್ತಾಂಶ_ISO2022*>(_this->extraInfo);
    uint32_t ಕೀಲಿ = myData2022->ಕೀಲಿ;
    int32_t ಸ್ಥಾನಾಂತರ = 0;
    int8_t initial_ಗಮ್ಯ_ULength = _this->ಗಮ್ಯ_ULength;
    ಅಕ್ಷರ c;

    value = VALID_NON_TERMINAL_2022;
    ಆಗ (*ಮೂಲ < ಮೂಲ_ಎಲ್ಲೆ) {
        c = *(*ಮೂಲ)++;
        _this->ಗಮ್ಯ_UBytes[_this->ಗಮ್ಯ_ULength++] = static_cast<uint8_t>(c);
        value = get_ಕೀಲಿ_2022(c, reinterpret_cast<int32_t*>(&ಕೀಲಿ), &ಸ್ಥಾನಾಂತರ);

        ಆಯ್ಕೆ (value){

        ಸ್ಥಿತಿ VALID_NON_TERMINAL_2022 :
            /* continue with the loop */
            ನಿಲ್ಲಿಸು;

        ಸ್ಥಿತಿ VALID_TERMINAL_2022:
            ಕೀಲಿ = 0;
            ಹೋಗು ಮುಗಿದ_2;

        ಸ್ಥಿತಿ INVALID_2022:
            ಹೋಗು ಮುಗಿದ_2;

        ಸ್ಥಿತಿ VALID_MAYBE_TERMINAL_2022:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ U_ENABLE_GENERIC_ISO_2022
            /* ESC ( B is ambiguous only for ISO_2022 itself */
            ಆದರೆ(var == ISO_2022) {
                /* discard toUBytes[] for ESC ( B because this sequence is correct and complete */
                _this->ಗಮ್ಯ_ULength = 0;

                /* TODO need to indicate that ESC ( B was seen; if failure, then need to replay from source or from MBCS-style replay */

                /* continue with the loop */
                value = VALID_NON_TERMINAL_2022;
                ನಿಲ್ಲಿಸು;
            } ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
            {
                /* not ISO_2022 itself, finish here */
                value = VALID_TERMINAL_2022;
                ಕೀಲಿ = 0;
                ಹೋಗು ಮುಗಿದ_2;
            }
        }
    }

ಮುಗಿದ_2:
    myData2022->ಕೀಲಿ = ಕೀಲಿ;

    ಆದರೆ (value == VALID_NON_TERMINAL_2022) {
        /* indicate that the escape sequence is incomplete: key!=0 */
        ಹಿಂತಿರುಗಿ;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (value == INVALID_2022 ) {
        *err = U_ILLEGAL_ESCAPE_SEQUENCE;
    } ಇಲ್ಲದಿದ್ದರೆ /* value == VALID_TERMINAL_2022 */ {
        ಆಯ್ಕೆ(var){
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ U_ENABLE_GENERIC_ISO_2022
        ಸ್ಥಿತಿ ISO_2022:
        {
            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *chosenConverterName = esc_Seq_State_ಕೋಷ್ಟಕ_ಫಲಿತಾಂಶ_2022[ಸ್ಥಾನಾಂತರ];
            ಆದರೆ(chosenConverterName == ಶೂನ್ಯಸೂಚಕ) {
                /* SS2 or SS3 */
                *err = U_UNSUPPORTED_ESCAPE_SEQUENCE;
                _this->ಗಮ್ಯ_UCallback_Reason = UCNV_UNASSIGNED;
                ಹಿಂತಿರುಗಿ;
            }

            _this->ಬಹುಲಕ = UCNV_SI;
            ucnv_close(myData2022->currentConverter);
            myData2022->currentConverter = myUConverter = ucnv_open(chosenConverterName, err);
            ಆದರೆ(U_SUCCESS(*err)) {
                myUConverter->fromCharErrorBehaviour = UCNV_ಗಮ್ಯ_U_CALLBACK_STOP;
                _this->ಬಹುಲಕ = UCNV_SO;
            }
            ನಿಲ್ಲಿಸು;
        }
#ಆದರೆ_ಅಂತ್ಯ
        ಸ್ಥಿತಿ ISO_2022_JP:
            {
                StateEnum ತಾತ್ಕಾಲಿಕ_State = static_cast<StateEnum>(ಮುಂದಿನ_State_ಗಮ್ಯ_Unicode_JP[ಸ್ಥಾನಾಂತರ]);
                ಆಯ್ಕೆ(ತಾತ್ಕಾಲಿಕ_State) {
                ಸ್ಥಿತಿ INVALID_STATE:
                    *err = U_UNSUPPORTED_ESCAPE_SEQUENCE;
                    ನಿಲ್ಲಿಸು;
                ಸ್ಥಿತಿ SS2_STATE:
                    ಆದರೆ(myData2022->ಗಮ್ಯ_U2022_State.cs[2]!=0) {
                        ಆದರೆ(myData2022->ಗಮ್ಯ_U2022_State.g<2) {
                            myData2022->ಗಮ್ಯ_U2022_State.prevG=myData2022->ಗಮ್ಯ_U2022_State.g;
                        }
                        myData2022->ಗಮ್ಯ_U2022_State.g=2;
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        /* illegal to have SS2 before a matching designator */
                        *err = U_ILLEGAL_ESCAPE_SEQUENCE;
                    }
                    ನಿಲ್ಲಿಸು;
                /* case SS3_STATE: not used in ISO-2022-JP-x */
                ಸ್ಥಿತಿ ISO8859_1:
                ಸ್ಥಿತಿ ISO8859_7:
                    ಆದರೆ((jpCharsetMasks[myData2022->ಆವೃತ್ತಿ] & CSM(ತಾತ್ಕಾಲಿಕ_State)) == 0) {
                        *err = U_UNSUPPORTED_ESCAPE_SEQUENCE;
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        /* G2 charset for SS2 */
                        myData2022->ಗಮ್ಯ_U2022_State.cs[2] = static_cast<int8_t>(ತಾತ್ಕಾಲಿಕ_State);
                    }
                    ನಿಲ್ಲಿಸು;
                ಪೂರ್ವನಿಯೋಜಿತ:
                    ಆದರೆ((jpCharsetMasks[myData2022->ಆವೃತ್ತಿ] & CSM(ತಾತ್ಕಾಲಿಕ_State)) == 0) {
                        *err = U_UNSUPPORTED_ESCAPE_SEQUENCE;
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        /* G0 charset */
                        myData2022->ಗಮ್ಯ_U2022_State.cs[0] = static_cast<int8_t>(ತಾತ್ಕಾಲಿಕ_State);
                    }
                    ನಿಲ್ಲಿಸು;
                }
            }
            ನಿಲ್ಲಿಸು;
#ಆದರೆ !UCONFIG_ONLY_HTML_CONVERSION
        ಸ್ಥಿತಿ ISO_2022_CN:
            {
                StateEnum ತಾತ್ಕಾಲಿಕ_State = static_cast<StateEnum>(ಮುಂದಿನ_State_ಗಮ್ಯ_Unicode_CN[ಸ್ಥಾನಾಂತರ]);
                ಆಯ್ಕೆ(ತಾತ್ಕಾಲಿಕ_State) {
                ಸ್ಥಿತಿ INVALID_STATE:
                    *err = U_UNSUPPORTED_ESCAPE_SEQUENCE;
                    ನಿಲ್ಲಿಸು;
                ಸ್ಥಿತಿ SS2_STATE:
                    ಆದರೆ(myData2022->ಗಮ್ಯ_U2022_State.cs[2]!=0) {
                        ಆದರೆ(myData2022->ಗಮ್ಯ_U2022_State.g<2) {
                            myData2022->ಗಮ್ಯ_U2022_State.prevG=myData2022->ಗಮ್ಯ_U2022_State.g;
                        }
                        myData2022->ಗಮ್ಯ_U2022_State.g=2;
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        /* illegal to have SS2 before a matching designator */
                        *err = U_ILLEGAL_ESCAPE_SEQUENCE;
                    }
                    ನಿಲ್ಲಿಸು;
                ಸ್ಥಿತಿ SS3_STATE:
                    ಆದರೆ(myData2022->ಗಮ್ಯ_U2022_State.cs[3]!=0) {
                        ಆದರೆ(myData2022->ಗಮ್ಯ_U2022_State.g<2) {
                            myData2022->ಗಮ್ಯ_U2022_State.prevG=myData2022->ಗಮ್ಯ_U2022_State.g;
                        }
                        myData2022->ಗಮ್ಯ_U2022_State.g=3;
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        /* illegal to have SS3 before a matching designator */
                        *err = U_ILLEGAL_ESCAPE_SEQUENCE;
                    }
                    ನಿಲ್ಲಿಸು;
                ಸ್ಥಿತಿ ISO_IR_165:
                    ಆದರೆ(myData2022->ಆವೃತ್ತಿ==0) {
                        *err = U_UNSUPPORTED_ESCAPE_SEQUENCE;
                        ನಿಲ್ಲಿಸು;
                    }
                    U_FALLTHROUGH;
                ಸ್ಥಿತಿ GB2312_1:
                    U_FALLTHROUGH;
                ಸ್ಥಿತಿ CNS_11643_1:
                    myData2022->ಗಮ್ಯ_U2022_State.cs[1] = static_cast<int8_t>(ತಾತ್ಕಾಲಿಕ_State);
                    ನಿಲ್ಲಿಸು;
                ಸ್ಥಿತಿ CNS_11643_2:
                    myData2022->ಗಮ್ಯ_U2022_State.cs[2] = static_cast<int8_t>(ತಾತ್ಕಾಲಿಕ_State);
                    ನಿಲ್ಲಿಸು;
                ಪೂರ್ವನಿಯೋಜಿತ:
                    /* other CNS 11643 planes */
                    ಆದರೆ(myData2022->ಆವೃತ್ತಿ==0) {
                        *err = U_UNSUPPORTED_ESCAPE_SEQUENCE;
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        myData2022->ಗಮ್ಯ_U2022_State.cs[3] = static_cast<int8_t>(ತಾತ್ಕಾಲಿಕ_State);
                    }
                    ನಿಲ್ಲಿಸು;
                }
            }
            ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ ISO_2022_KR:
            ಆದರೆ(ಸ್ಥಾನಾಂತರ==0x30){
                /* nothing to be done, just accept this one escape sequence */
            } ಇಲ್ಲದಿದ್ದರೆ {
                *err = U_UNSUPPORTED_ESCAPE_SEQUENCE;
            }
            ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ  // !UCONFIG_ONLY_HTML_CONVERSION

        ಪೂರ್ವನಿಯೋಜಿತ:
            *err = U_ILLEGAL_ESCAPE_SEQUENCE;
            ನಿಲ್ಲಿಸು;
        }
    }
    ಆದರೆ(U_SUCCESS(*err)) {
        _this->ಗಮ್ಯ_ULength = 0;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(*err==U_ILLEGAL_ESCAPE_SEQUENCE) {
        ಆದರೆ(_this->ಗಮ್ಯ_ULength>1) {
            /*
             * Ticket 5691: consistent illegal sequences:
             * - We include at least the first byte (ESC) in the illegal sequence.
             * - If any of the non-initial bytes could be the start of a character,
             *   we stop the illegal sequence before the first one of those.
             *   In escape sequences, all following bytes are "printable", that is,
             *   unless they are completely illegal (>7f in SBCS, outside 21..7e in DBCS),
             *   they are valid single/lead bytes.
             *   For simplicity, we always only report the initial ESC byte as the
             *   illegal sequence and back out all other bytes we looked at.
             */
            /* Back out some bytes. */
            int8_t ಹಿಂಭಾಗ_ಹೊರಹೋಗು_ದೂರ=_this->ಗಮ್ಯ_ULength-1;
            int8_t ಬೈಟುಗಳು_From_This_ಬಫರ್=_this->ಗಮ್ಯ_ULength-initial_ಗಮ್ಯ_ULength;
            ಆದರೆ(ಹಿಂಭಾಗ_ಹೊರಹೋಗು_ದೂರ<=ಬೈಟುಗಳು_From_This_ಬಫರ್) {
                /* same as initialToULength<=1 */
                *ಮೂಲ-=ಹಿಂಭಾಗ_ಹೊರಹೋಗು_ದೂರ;
            } ಇಲ್ಲದಿದ್ದರೆ {
                /* Back out bytes from the previous buffer: Need to replay them. */
                _this->pre_ಗಮ್ಯ_ULength = static_cast<int8_t>(ಬೈಟುಗಳು_From_This_ಬಫರ್ - ಹಿಂಭಾಗ_ಹೊರಹೋಗು_ದೂರ);
                /* same as -(initialToULength-1) */
                /* preToULength is negative! */
                uprv_memcpy(_this->pre_ಗಮ್ಯ_U, _this->ಗಮ್ಯ_UBytes+1, -_this->pre_ಗಮ್ಯ_ULength);
                *ಮೂಲ-=ಬೈಟುಗಳು_From_This_ಬಫರ್;
            }
            _this->ಗಮ್ಯ_ULength=1;
        }
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(*err==U_UNSUPPORTED_ESCAPE_SEQUENCE) {
        _this->ಗಮ್ಯ_UCallback_Reason = UCNV_UNASSIGNED;
    }
}

#ಆದರೆ !UCONFIG_ONLY_HTML_CONVERSION
/*Checks the characters of the buffer against valid 2022 escape sequences
*if the match we return a pointer to the initial start of the sequence otherwise
*we return sourceLimit
*/
/*for 2022 looks ahead in the stream
 *to determine the longest possible convertible
 *data stream
 */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*
get_ಕೊನೆ_ದ_ಬಫರ್_2022(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ** ಮೂಲ,
                   ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಮೂಲ_ಎಲ್ಲೆ,
                   UBool /*flush*/){

    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* my_ಮೂಲ = *ಮೂಲ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ U_ENABLE_GENERIC_ISO_2022
    ಆದರೆ (*ಮೂಲ >= ಮೂಲ_ಎಲ್ಲೆ)
        ಹಿಂತಿರುಗಿ ಮೂಲ_ಎಲ್ಲೆ;

    ಮಾಡು{

        ಆದರೆ (*my_ಮೂಲ == ESC_2022){
            int8_t i;
            int32_t ಕೀಲಿ = 0;
            int32_t ಸ್ಥಾನಾಂತರ;
            UCNV_ಕೋಷ್ಟಕ_ಸ್ಥಿತಿಗಳು_2022 value = VALID_NON_TERMINAL_2022;

            /* Kludge: I could not
            * figure out the reason for validating an escape sequence
            * twice - once here and once in changeState_2022().
            * is it possible to have an ESC character in a ISO2022
            * byte stream which is valid in a code page? Is it legal?
            */
            ಗಾಗಿ (i=0;
            (my_ಮೂಲ+i < ಮೂಲ_ಎಲ್ಲೆ)&&(value == VALID_NON_TERMINAL_2022);
            i++) {
                value =  get_ಕೀಲಿ_2022(*(my_ಮೂಲ+i), &ಕೀಲಿ, &ಸ್ಥಾನಾಂತರ);
            }
            ಆದರೆ (value > 0 || *my_ಮೂಲ==ESC_2022)
                ಹಿಂತಿರುಗಿ my_ಮೂಲ;

            ಆದರೆ ((value == VALID_NON_TERMINAL_2022)&&(!flush) )
                ಹಿಂತಿರುಗಿ ಮೂಲ_ಎಲ್ಲೆ;
        }
    }ಆಗ (++my_ಮೂಲ < ಮೂಲ_ಎಲ್ಲೆ);

    ಹಿಂತಿರುಗಿ ಮೂಲ_ಎಲ್ಲೆ;
#ಇಲ್ಲದಿದ್ದರೆ
    ಆಗ(my_ಮೂಲ < ಮೂಲ_ಎಲ್ಲೆ && *my_ಮೂಲ != ESC_2022) {
        ++my_ಮೂಲ;
    }
    ಹಿಂತಿರುಗಿ my_ಮೂಲ;
#ಆದರೆ_ಅಂತ್ಯ
}
#ಆದರೆ_ಅಂತ್ಯ

/* This inline function replicates code in _MBCSFromUChar32() function in ucnvmbcs.c
 * any future change in _MBCSFromUChar32() function should be reflected here.
 * @return number of bytes in *value; negative number if fallback; 0 if no mapping
 */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ int32_t
MBCS_FROM_UCHAR32_ISO2022(UConverter_Shared_ದತ್ತಾಂಶ* shared_ದತ್ತಾಂಶ,
                                         UChar32 c,
                                         uint32_t* value,
                                         UBool useFallback,
                                         ಪೂರ್ಣಾಂಕ outputType)
{
    ಸ್ಥಿರಾಂಕ int32_t *cx;
    ಸ್ಥಿರಾಂಕ uint16_t *ಕೋಷ್ಟಕ;
    uint32_t stage2_ನಮೂದು;
    uint32_t my_ಮೌಲ್ಯ;
    int32_t length;
    ಸ್ಥಿರಾಂಕ uint8_t *p;
    /*
     * TODO(markus): Use and require new, faster MBCS conversion table structures.
     * Use internal version of ucnv_open() that verifies that the new structures are available,
     * else U_INTERNAL_PROGRAM_ERROR.
     */
    /* BMP-only codepages are stored without stage 1 entries for supplementary code points */
    ಆದರೆ(c<0x10000 || (shared_ದತ್ತಾಂಶ->mbcs.unicode_ಮುಖವಾಡ&UCNV_HAS_SUPPLEMENTARY)) {
        ಕೋಷ್ಟಕ=shared_ದತ್ತಾಂಶ->mbcs.from_Unicode_ಕೋಷ್ಟಕ;
        stage2_ನಮೂದು=MBCS_STAGE_2_FROM_U(ಕೋಷ್ಟಕ, c);
        /* get the bytes and the length for the output */
        ಆದರೆ(outputType==MBCS_OUTPUT_2){
            my_ಮೌಲ್ಯ=MBCS_ಮೌಲ್ಯ_2_FROM_STAGE_2(shared_ದತ್ತಾಂಶ->mbcs.from_Unicode_ಬೈಟುಗಳು, stage2_ನಮೂದು, c);
            ಆದರೆ(my_ಮೌಲ್ಯ<=0xff) {
                length=1;
            } ಇಲ್ಲದಿದ್ದರೆ {
                length=2;
            }
        } ಇಲ್ಲದಿದ್ದರೆ /* outputType==MBCS_OUTPUT_3 */ {
            p=MBCS_ಸೂಚಕ_3_FROM_STAGE_2(shared_ದತ್ತಾಂಶ->mbcs.from_Unicode_ಬೈಟುಗಳು, stage2_ನಮೂದು, c);
            my_ಮೌಲ್ಯ = (static_cast<uint32_t>(*p) << 16) | (static_cast<uint32_t>(p[1]) << 8) | p[2];
            ಆದರೆ(my_ಮೌಲ್ಯ<=0xff) {
                length=1;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(my_ಮೌಲ್ಯ<=0xffff) {
                length=2;
            } ಇಲ್ಲದಿದ್ದರೆ {
                length=3;
            }
        }
        /* is this code point assigned, or do we use fallbacks? */
        ಆದರೆ((stage2_ನಮೂದು&(1<<(16+(c&0xf))))!=0) {
            /* assigned */
            *value=my_ಮೌಲ್ಯ;
            ಹಿಂತಿರುಗಿ length;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(FROM_U_USE_FALLBACK(useFallback, c) && my_ಮೌಲ್ಯ!=0) {
            /*
             * We allow a 0 byte output if the "assigned" bit is set for this entry.
             * There is no way with this data structure for fallback output
             * to be a zero byte.
             */
            *value=my_ಮೌಲ್ಯ;
            ಹಿಂತಿರುಗಿ -length;
        }
    }

    cx=shared_ದತ್ತಾಂಶ->mbcs.extIndexes;
    ಆದರೆ(cx!=ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ ucnv_ext_Simple_ಹೊಂದಿಕೆ_From_U(cx, c, value, useFallback);
    }

    /* unassigned */
    ಹಿಂತಿರುಗಿ 0;
}

/* This inline function replicates code in _MBCSSingleFromUChar32() function in ucnvmbcs.c
 * any future change in _MBCSSingleFromUChar32() function should be reflected here.
 * @param retval pointer to output byte
 * @return 1 roundtrip byte  0 no mapping  -1 fallback byte
 */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ int32_t
MBCS_SINGLE_FROM_UCHAR32(UConverter_Shared_ದತ್ತಾಂಶ* shared_ದತ್ತಾಂಶ,
                                       UChar32 c,
                                       uint32_t* retval,
                                       UBool useFallback)
{
    ಸ್ಥಿರಾಂಕ uint16_t *ಕೋಷ್ಟಕ;
    int32_t value;
    /* BMP-only codepages are stored without stage 1 entries for supplementary code points */
    ಆದರೆ(c>=0x10000 && !(shared_ದತ್ತಾಂಶ->mbcs.unicode_ಮುಖವಾಡ&UCNV_HAS_SUPPLEMENTARY)) {
        ಹಿಂತಿರುಗಿ 0;
    }
    /* convert the Unicode code point in c into codepage bytes (same as in _MBCSFromUnicodeWithOffsets) */
    ಕೋಷ್ಟಕ=shared_ದತ್ತಾಂಶ->mbcs.from_Unicode_ಕೋಷ್ಟಕ;
    /* get the byte for the output */
    value=MBCS_SINGLE_ಫಲಿತಾಂಶ_FROM_U(ಕೋಷ್ಟಕ, (uint16_t *)shared_ದತ್ತಾಂಶ->mbcs.from_Unicode_ಬೈಟುಗಳು, c);
    /* is this code point assigned, or do we use fallbacks? */
    *retval = static_cast<uint32_t>(value & 0xff);
    ಆದರೆ(value>=0xf00) {
        ಹಿಂತಿರುಗಿ 1;  /* roundtrip */
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(useFallback ? value>=0x800 : value>=0xc00) {
        ಹಿಂತಿರುಗಿ -1;  /* fallback taken */
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ 0;  /* no mapping */
    }
}

/*
 * Check that the result is a 2-byte value with each byte in the range A1..FE
 * (strict EUC DBCS) before accepting it and subtracting 0x80 from each byte
 * to move it to the ISO 2022 range 21..7E.
 * Return 0 if out of range.
 */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ uint32_t
_2022FromGR94DBCS(uint32_t value) {
    ಆದರೆ (static_cast<uint16_t>(value - 0xa1a1) <= (0xfefe - 0xa1a1) &&
        static_cast<uint8_t>(value - 0xa1) <= (0xfe - 0xa1)
    ) {
        ಹಿಂತಿರುಗಿ value - 0x8080;  /* shift down to 21..7e byte range */
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ 0;  /* not valid for ISO 2022 */
    }
}

#ಆದರೆ 0 /* 5691: Call sites now check for validity. They can just += 0x8080 after that. */
/*
 * This method does the reverse of _2022FromGR94DBCS(). Given the 2022 code point, it returns the
 * 2 byte value that is in the range A1..FE for each byte. Otherwise it returns the 2022 code point
 * unchanged. 
 */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ uint32_t
_2022ToGR94DBCS(uint32_t value) {
    uint32_t return_ಮೌಲ್ಯ = value + 0x8080;
    ಆದರೆ( (uint16_t)(return_ಮೌಲ್ಯ - 0xa1a1) <= (0xfefe - 0xa1a1) &&
        (uint8_t)(return_ಮೌಲ್ಯ - 0xa1) <= (0xfe - 0xa1)) {
        ಹಿಂತಿರುಗಿ return_ಮೌಲ್ಯ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ value;
    }
}
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ U_ENABLE_GENERIC_ISO_2022

/**********************************************************************************
*  ISO-2022 Converter
*
*
*/

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
T_UConverter_ಗಮ್ಯ_Unicode_ISO_2022_OFFSETS_LOGIC(UConverter_ಗಮ್ಯ_Unicode_ವಾದಗಳು* ವಾದಗಳು_2,
                                                           UError_ಸಂಕೇತ* err){
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* my_ಮೂಲ_ಎಲ್ಲೆ, *real_ಮೂಲ_ಎಲ್ಲೆ;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಮೂಲ_ಆರಂಭ;
    ಸ್ಥಿರಾಂಕ char16_t* my_ಗುರಿ_ಆರಂಭ;
    UConverter* saveThis;
    UConverter_ದತ್ತಾಂಶ_ISO2022* my_ದತ್ತಾಂಶ;
    int8_t length;

    saveThis = ವಾದಗಳು_2->converter;
    my_ದತ್ತಾಂಶ=((UConverter_ದತ್ತಾಂಶ_ISO2022*)(saveThis->extraInfo));

    real_ಮೂಲ_ಎಲ್ಲೆ = ವಾದಗಳು_2->ಮೂಲ_ಎಲ್ಲೆ;
    ಆಗ (ವಾದಗಳು_2->ಮೂಲ < real_ಮೂಲ_ಎಲ್ಲೆ) {
        ಆದರೆ(my_ದತ್ತಾಂಶ->ಕೀಲಿ == 0) { /* are we in the middle of an escape sequence? */
            /*Find the end of the buffer e.g : Next Escape Seq | end of Buffer*/
            my_ಮೂಲ_ಎಲ್ಲೆ = get_ಕೊನೆ_ದ_ಬಫರ್_2022(&(ವಾದಗಳು_2->ಮೂಲ), real_ಮೂಲ_ಎಲ್ಲೆ, ವಾದಗಳು_2->flush);

            ಆದರೆ(ವಾದಗಳು_2->ಮೂಲ < my_ಮೂಲ_ಎಲ್ಲೆ) {
                ಆದರೆ(my_ದತ್ತಾಂಶ->currentConverter==ಶೂನ್ಯಸೂಚಕ) {
                    my_ದತ್ತಾಂಶ->currentConverter = ucnv_open("ASCII",err);
                    ಆದರೆ(U_FAILURE(*err)){
                        ಹಿಂತಿರುಗಿ;
                    }

                    my_ದತ್ತಾಂಶ->currentConverter->fromCharErrorBehaviour = UCNV_ಗಮ್ಯ_U_CALLBACK_STOP;
                    saveThis->ಬಹುಲಕ = UCNV_SO;
                }

                /* convert to before the ESC or until the end of the buffer */
                my_ದತ್ತಾಂಶ->is_ಮೊದಲ_ಬಫರ್=ಸುಳ್ಳು;
                ಮೂಲ_ಆರಂಭ = ವಾದಗಳು_2->ಮೂಲ;
                my_ಗುರಿ_ಆರಂಭ = ವಾದಗಳು_2->ಗುರಿ;
                ವಾದಗಳು_2->converter = my_ದತ್ತಾಂಶ->currentConverter;
                ucnv_ಗಮ್ಯ_Unicode(ವಾದಗಳು_2->converter,
                    &ವಾದಗಳು_2->ಗುರಿ,
                    ವಾದಗಳು_2->ಗುರಿ_ಎಲ್ಲೆ,
                    &ವಾದಗಳು_2->ಮೂಲ,
                    my_ಮೂಲ_ಎಲ್ಲೆ,
                    ವಾದಗಳು_2->offsets,
                    (UBool)(ವಾದಗಳು_2->flush && my_ಮೂಲ_ಎಲ್ಲೆ == real_ಮೂಲ_ಎಲ್ಲೆ),
                    err);
                ವಾದಗಳು_2->converter = saveThis;

                ಆದರೆ (*err == U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR) {
                    /* move the overflow buffer */
                    length = saveThis->UChar_Error_ಬಫರ್_ಉದ್ದಳತೆ = my_ದತ್ತಾಂಶ->currentConverter->UChar_Error_ಬಫರ್_ಉದ್ದಳತೆ;
                    my_ದತ್ತಾಂಶ->currentConverter->UChar_Error_ಬಫರ್_ಉದ್ದಳತೆ = 0;
                    ಆದರೆ(length > 0) {
                        uprv_memcpy(saveThis->UChar_Error_ಬಫರ್,
                                    my_ದತ್ತಾಂಶ->currentConverter->UChar_Error_ಬಫರ್,
                                    length*U_SIZEOF_UCHAR);
                    }
                    ಹಿಂತಿರುಗಿ;
                }

                /*
                 * At least one of:
                 * -Error while converting
                 * -Done with entire buffer
                 * -Need to write offsets or update the current offset
                 *  (leave that up to the code in ucnv.c)
                 *
                 * or else we just stopped at an ESC byte and continue with changeState_2022()
                 */
                ಆದರೆ (U_FAILURE(*err) ||
                    (ವಾದಗಳು_2->ಮೂಲ == real_ಮೂಲ_ಎಲ್ಲೆ) ||
                    (ವಾದಗಳು_2->offsets != ಶೂನ್ಯಸೂಚಕ && (ವಾದಗಳು_2->ಗುರಿ != my_ಗುರಿ_ಆರಂಭ || ವಾದಗಳು_2->ಮೂಲ != ಮೂಲ_ಆರಂಭ) ||
                    (my_ಮೂಲ_ಎಲ್ಲೆ < real_ಮೂಲ_ಎಲ್ಲೆ && my_ದತ್ತಾಂಶ->currentConverter->ಗಮ್ಯ_ULength > 0))
                ) {
                    /* copy partial or error input for truncated detection and error handling */
                    ಆದರೆ(U_FAILURE(*err)) {
                        length = saveThis->invalid_Char_ಉದ್ದಳತೆ = my_ದತ್ತಾಂಶ->currentConverter->invalid_Char_ಉದ್ದಳತೆ;
                        ಆದರೆ(length > 0) {
                            uprv_memcpy(saveThis->invalid_Char_ಬಫರ್, my_ದತ್ತಾಂಶ->currentConverter->invalid_Char_ಬಫರ್, length);
                        }
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        length = saveThis->ಗಮ್ಯ_ULength = my_ದತ್ತಾಂಶ->currentConverter->ಗಮ್ಯ_ULength;
                        ಆದರೆ(length > 0) {
                            uprv_memcpy(saveThis->ಗಮ್ಯ_UBytes, my_ದತ್ತಾಂಶ->currentConverter->ಗಮ್ಯ_UBytes, length);
                            ಆದರೆ(ವಾದಗಳು_2->ಮೂಲ < my_ಮೂಲ_ಎಲ್ಲೆ) {
                                *err = U_TRUNCATED_CHAR_ಸಿಕ್ಕಿತು; /* truncated input before ESC */
                            }
                        }
                    }
                    ಹಿಂತಿರುಗಿ;
                }
            }
        }

        ಮೂಲ_ಆರಂಭ = ವಾದಗಳು_2->ಮೂಲ;
        changeState_2022(ವಾದಗಳು_2->converter,
               &(ವಾದಗಳು_2->ಮೂಲ),
               real_ಮೂಲ_ಎಲ್ಲೆ,
               ISO_2022,
               err);
        ಆದರೆ (U_FAILURE(*err) || (ವಾದಗಳು_2->ಮೂಲ != ಮೂಲ_ಆರಂಭ && ವಾದಗಳು_2->offsets != ಶೂನ್ಯಸೂಚಕ)) {
            /* let the ucnv.c code update its current offset */
            ಹಿಂತಿರುಗಿ;
        }
    }
}

#ಆದರೆ_ಅಂತ್ಯ

/*
 * To Unicode Callback helper function
 */
ಸ್ಥಿರ ಶೂನ್ಯ
ಗಮ್ಯ_Unicode_Callback(UConverter *cnv,
                  ಸ್ಥಿರಾಂಕ uint32_t ಮೂಲ_Char, ಸ್ಥಿರಾಂಕ uint32_t ಗುರಿ_Uni_Char,
                  UError_ಸಂಕೇತ* err){
    ಆದರೆ(ಮೂಲ_Char>0xff){
        cnv->ಗಮ್ಯ_UBytes[0] = static_cast<uint8_t>(ಮೂಲ_Char >> 8);
        cnv->ಗಮ್ಯ_UBytes[1] = static_cast<uint8_t>(ಮೂಲ_Char);
        cnv->ಗಮ್ಯ_ULength = 2;
    }
    ಇಲ್ಲದಿದ್ದರೆ{
        cnv->ಗಮ್ಯ_UBytes[0] = static_cast<ಅಕ್ಷರ>(ಮೂಲ_Char);
        cnv->ಗಮ್ಯ_ULength = 1;
    }

    ಆದರೆ(ಗುರಿ_Uni_Char == (missingCharMarker-1/*0xfffe*/)){
        *err = U_INVALID_CHAR_ಸಿಕ್ಕಿತು;
    }
    ಇಲ್ಲದಿದ್ದರೆ{
        *err = U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
    }
}

/**************************************ISO-2022-JP*************************************************/

/************************************** IMPORTANT **************************************************
* The UConverter_fromUnicode_ISO2022_JP converter does not use ucnv_fromUnicode() functions for SBCS,DBCS and
* MBCS; instead, the values are obtained directly by calling _MBCSFromUChar32().
* The converter iterates over each Unicode codepoint
* to obtain the equivalent codepoints from the codepages supported. Since the source buffer is
* processed one char at a time it would make sense to reduce the extra processing a canned converter
* would do as far as possible.
*
* If the implementation of these macros or structure of sharedData struct change in the future, make
* sure that ISO-2022 is also changed.
***************************************************************************************************
*/

/***************************************************************************************************
* Rules for ISO-2022-jp encoding
* (i)   Escape sequences must be fully contained within a line they should not
*       span new lines or CRs
* (ii)  If the last character on a line is represented by two bytes then an ASCII or
*       JIS-Roman character escape sequence should follow before the line terminates
* (iii) If the first character on the line is represented by two bytes then a two
*       byte character escape sequence should precede it
* (iv)  If no escape sequence is encountered then the characters are ASCII
* (v)   Latin(ISO-8859-1) and Greek(ISO-8859-7) characters must be designated to G2,
*       and invoked with SS2 (ESC N).
* (vi)  If there is any G0 designation in text, there must be a switch to
*       ASCII or to JIS X 0201-Roman before a space character (but not
*       necessarily before "ESC 4/14 2/0" or "ESC N ' '") or control
*       characters such as tab or CRLF.
* (vi)  Supported encodings:
*          ASCII, JISX201, JISX208, JISX212, GB2312, KSC5601, ISO-8859-1,ISO-8859-7
*
*  source : RFC-1554
*
*          JISX201, JISX208,JISX212 : new .cnv data files created
*          KSC5601 : alias to ibm-949 mapping table
*          GB2312 : alias to ibm-1386 mapping table
*          ISO-8859-1 : Algorithmic implemented as LATIN1 case
*          ISO-8859-7 : alias to ibm-9409 mapping table
*/

/* preference order of JP charsets */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ StateEnum jpCharsetPref[]={
    ASCII,
    JISX201,
    ISO8859_1,
    JISX208,
    ISO8859_7,
    JISX212,
    GB2312,
    KSC5601,
    HWKANA_7_ಬಿಟ್
};

/*
 * The escape sequences must be in order of the enum constants like JISX201  = 3,
 * not in order of jpCharsetPref[]!
 */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ escSeqChars[][6] ={
    "\x1B\x28\x42",         /* <ESC>(B  ASCII       */
    "\x1B\x2E\x41",         /* <ESC>.A  ISO-8859-1  */
    "\x1B\x2E\x46",         /* <ESC>.F  ISO-8859-7  */
    "\x1B\x28\x4A",         /* <ESC>(J  JISX-201    */
    "\x1B\x24\x42",         /* <ESC>$B  JISX-208    */
    "\x1B\x24\x28\x44",     /* <ESC>$(D JISX-212    */
    "\x1B\x24\x41",         /* <ESC>$A  GB2312      */
    "\x1B\x24\x28\x43",     /* <ESC>$(C KSC5601     */
    "\x1B\x28\x49"          /* <ESC>(I  HWKANA_7BIT */

};
ಸ್ಥಿರ  ಸ್ಥಿರಾಂಕ int8_t escSeqCharsLen[] ={
    3, /* length of <ESC>(B  ASCII       */
    3, /* length of <ESC>.A  ISO-8859-1  */
    3, /* length of <ESC>.F  ISO-8859-7  */
    3, /* length of <ESC>(J  JISX-201    */
    3, /* length of <ESC>$B  JISX-208    */
    4, /* length of <ESC>$(D JISX-212    */
    3, /* length of <ESC>$A  GB2312      */
    4, /* length of <ESC>$(C KSC5601     */
    3  /* length of <ESC>(I  HWKANA_7BIT */
};

/*
* The iteration over various code pages works this way:
* i)   Get the currentState from myConverterData->currentState
* ii)  Check if the character is mapped to a valid character in the currentState
*      Yes ->  a) set the initIterState to currentState
*       b) remain in this state until an invalid character is found
*      No  ->  a) go to the next code page and find the character
* iii) Before changing the state increment the current state check if the current state
*      is equal to the intitIteration state
*      Yes ->  A character that cannot be represented in any of the supported encodings
*       break and return a U_INVALID_CHARACTER error
*      No  ->  Continue and find the character in next code page
*
*
* TODO: Implement a priority technique where the users are allowed to set the priority of code pages
*/

/* Map 00..7F to Unicode according to JIS X 0201. */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ uint32_t
jisx201_ಗಮ್ಯ_U(uint32_t value) {
    ಆದರೆ(value < 0x5c) {
        ಹಿಂತಿರುಗಿ value;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(value == 0x5c) {
        ಹಿಂತಿರುಗಿ 0xa5;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(value == 0x7e) {
        ಹಿಂತಿರುಗಿ 0x203e;
    } ಇಲ್ಲದಿದ್ದರೆ /* value <= 0x7f */ {
        ಹಿಂತಿರುಗಿ value;
    }
}

/* Map Unicode to 00..7F according to JIS X 0201. Return U+FFFE if unmappable. */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ uint32_t
jisx201FromU(uint32_t value) {
    ಆದರೆ(value<=0x7f) {
        ಆದರೆ(value!=0x5c && value!=0x7e) {
            ಹಿಂತಿರುಗಿ value;
        }
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(value==0xa5) {
        ಹಿಂತಿರುಗಿ 0x5c;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(value==0x203e) {
        ಹಿಂತಿರುಗಿ 0x7e;
    }
    ಹಿಂತಿರುಗಿ 0xfffe;
}

/*
 * Take a valid Shift-JIS byte pair, check that it is in the range corresponding
 * to JIS X 0208, and convert it to a pair of 21..7E bytes.
 * Return 0 if the byte pair is out of range.
 */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ uint32_t
_2022FromSJIS(uint32_t value) {
    uint8_t trail;

    ಆದರೆ(value > 0xEFFC) {
        ಹಿಂತಿರುಗಿ 0;  /* beyond JIS X 0208 */
    }

    trail = static_cast<uint8_t>(value);

    value &= 0xff00;  /* lead byte */
    ಆದರೆ(value <= 0x9f00) {
        value -= 0x7000;
    } ಇಲ್ಲದಿದ್ದರೆ /* 0xe000 <= value <= 0xef00 */ {
        value -= 0xb000;
    }
    value <<= 1;

    ಆದರೆ(trail <= 0x9e) {
        value -= 0x100;
        ಆದರೆ(trail <= 0x7e) {
            value |= trail - 0x1f;
        } ಇಲ್ಲದಿದ್ದರೆ {
            value |= trail - 0x20;
        }
    } ಇಲ್ಲದಿದ್ದರೆ /* trail <= 0xfc */ {
        value |= trail - 0x7e;
    }
    ಹಿಂತಿರುಗಿ value;
}

/*
 * Convert a pair of JIS X 0208 21..7E bytes to Shift-JIS.
 * If either byte is outside 21..7E make sure that the result is not valid
 * for Shift-JIS so that the converter catches it.
 * Some invalid byte values already turn into equally invalid Shift-JIS
 * byte values and need not be tested explicitly.
 */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ
_2022ToSJIS(uint8_t c1, uint8_t c2, ಅಕ್ಷರ ಬೈಟುಗಳು[2]) {
    ಆದರೆ(c1&1) {
        ++c1;
        ಆದರೆ(c2 <= 0x5f) {
            c2 += 0x1f;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c2 <= 0x7e) {
            c2 += 0x20;
        } ಇಲ್ಲದಿದ್ದರೆ {
            c2 = 0;  /* invalid */
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಆದರೆ (static_cast<uint8_t>(c2 - 0x21) <= ((0x7e) - 0x21)) {
            c2 += 0x7e;
        } ಇಲ್ಲದಿದ್ದರೆ {
            c2 = 0;  /* invalid */
        }
    }
    c1 >>= 1;
    ಆದರೆ(c1 <= 0x2f) {
        c1 += 0x70;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c1 <= 0x3f) {
        c1 += 0xb0;
    } ಇಲ್ಲದಿದ್ದರೆ {
        c1 = 0;  /* invalid */
    }
    ಬೈಟುಗಳು[0] = static_cast<ಅಕ್ಷರ>(c1);
    ಬೈಟುಗಳು[1] = static_cast<ಅಕ್ಷರ>(c2);
}

/*
 * JIS X 0208 has fallbacks from Unicode half-width Katakana to full-width (DBCS)
 * Katakana.
 * Now that we use a Shift-JIS table for JIS X 0208 we need to hardcode these fallbacks
 * because Shift-JIS roundtrips half-width Katakana to single bytes.
 * These were the only fallbacks in ICU's jisx-208.ucm file.
 */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ uint16_t hwkana_fb[HWKANA_ಕೊನೆ - HWKANA_ಆರಂಭ + 1] = {
    0x2123,  /* U+FF61 */
    0x2156,
    0x2157,
    0x2122,
    0x2126,
    0x2572,
    0x2521,
    0x2523,
    0x2525,
    0x2527,
    0x2529,
    0x2563,
    0x2565,
    0x2567,
    0x2543,
    0x213C,  /* U+FF70 */
    0x2522,
    0x2524,
    0x2526,
    0x2528,
    0x252A,
    0x252B,
    0x252D,
    0x252F,
    0x2531,
    0x2533,
    0x2535,
    0x2537,
    0x2539,
    0x253B,
    0x253D,
    0x253F,  /* U+FF80 */
    0x2541,
    0x2544,
    0x2546,
    0x2548,
    0x254A,
    0x254B,
    0x254C,
    0x254D,
    0x254E,
    0x254F,
    0x2552,
    0x2555,
    0x2558,
    0x255B,
    0x255E,
    0x255F,  /* U+FF90 */
    0x2560,
    0x2561,
    0x2562,
    0x2564,
    0x2566,
    0x2568,
    0x2569,
    0x256A,
    0x256B,
    0x256C,
    0x256D,
    0x256F,
    0x2573,
    0x212B,
    0x212C   /* U+FF9F */
};

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
UConverter_fromUnicode_ISO_2022_JP_OFFSETS_LOGIC(UConverter_From_Unicode_ವಾದಗಳು* ವಾದಗಳು_2, UError_ಸಂಕೇತ* err) {
    UConverter *cnv = ವಾದಗಳು_2->converter;
    UConverter_ದತ್ತಾಂಶ_ISO2022 *converter_ದತ್ತಾಂಶ;
    ISO2022State *pFromU2022State;
    uint8_t* ಗುರಿ = reinterpret_cast<uint8_t*>(ವಾದಗಳು_2->ಗುರಿ);
    ಸ್ಥಿರಾಂಕ uint8_t* ಗುರಿ_ಎಲ್ಲೆ = reinterpret_cast<ಸ್ಥಿರಾಂಕ uint8_t*>(ವಾದಗಳು_2->ಗುರಿ_ಎಲ್ಲೆ);
    ಸ್ಥಿರಾಂಕ char16_t* ಮೂಲ = ವಾದಗಳು_2->ಮೂಲ;
    ಸ್ಥಿರಾಂಕ char16_t* ಮೂಲ_ಎಲ್ಲೆ = ವಾದಗಳು_2->ಮೂಲ_ಎಲ್ಲೆ;
    int32_t* offsets = ವಾದಗಳು_2->offsets;
    UChar32 ಮೂಲ_Char;
    ಅಕ್ಷರ ಬಫರ್[8];
    int32_t len, ಹೊರಹೋಗು_Len;
    int8_t choices[10];
    int32_t choice_ಎಣಿಕೆ;
    uint32_t ಗುರಿ_ಮೌಲ್ಯ = 0;
    UBool useFallback;

    int32_t i;
    int8_t cs, g;

    /* set up the state */
    converter_ದತ್ತಾಂಶ = static_cast<UConverter_ದತ್ತಾಂಶ_ISO2022*>(cnv->extraInfo);
    pFromU2022State   = &converter_ದತ್ತಾಂಶ->fromU2022State;

    choice_ಎಣಿಕೆ = 0;

    /* check if the last codepoint of previous buffer was a lead surrogate*/
    ಆದರೆ((ಮೂಲ_Char = cnv->fromUChar32)!=0 && ಗುರಿ< ಗುರಿ_ಎಲ್ಲೆ) {
        ಹೋಗು getTrail;
    }

    ಆಗ(ಮೂಲ < ಮೂಲ_ಎಲ್ಲೆ) {
        ಆದರೆ(ಗುರಿ < ಗುರಿ_ಎಲ್ಲೆ) {

            ಮೂಲ_Char  = *(ಮೂಲ++);
            /*check if the char is a First surrogate*/
            ಆದರೆ(U16_IS_SURROGATE(ಮೂಲ_Char)) {
                ಆದರೆ(U16_IS_SURROGATE_LEAD(ಮೂಲ_Char)) {
getTrail:
                    /*look ahead to find the trail surrogate*/
                    ಆದರೆ(ಮೂಲ < ಮೂಲ_ಎಲ್ಲೆ) {
                        /* test the following code unit */
                        char16_t trail = *ಮೂಲ;
                        ಆದರೆ(U16_IS_TRAIL(trail)) {
                            ಮೂಲ++;
                            ಮೂಲ_Char=U16_GET_SUPPLEMENTARY(ಮೂಲ_Char, trail);
                            cnv->fromUChar32=0x00;
                            /* convert this supplementary code point */
                            /* exit this condition tree */
                        } ಇಲ್ಲದಿದ್ದರೆ {
                            /* this is an unmatched lead code unit (1st surrogate) */
                            /* callback(illegal) */
                            *err=U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
                            cnv->fromUChar32=ಮೂಲ_Char;
                            ನಿಲ್ಲಿಸು;
                        }
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        /* no more input */
                        cnv->fromUChar32=ಮೂಲ_Char;
                        ನಿಲ್ಲಿಸು;
                    }
                } ಇಲ್ಲದಿದ್ದರೆ {
                    /* this is an unmatched trail code unit (2nd surrogate) */
                    /* callback(illegal) */
                    *err=U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
                    cnv->fromUChar32=ಮೂಲ_Char;
                    ನಿಲ್ಲಿಸು;
                }
            }

            /* do not convert SO/SI/ESC */
            ಆದರೆ(IS_2022_CONTROL(ಮೂಲ_Char)) {
                /* callback(illegal) */
                *err=U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
                cnv->fromUChar32=ಮೂಲ_Char;
                ನಿಲ್ಲಿಸು;
            }

            /* do the conversion */

            ಆದರೆ(choice_ಎಣಿಕೆ == 0) {
                uint16_t csm;

                /*
                 * The csm variable keeps track of which charsets are allowed
                 * and not used yet while building the choices[].
                 */
                csm = jpCharsetMasks[converter_ದತ್ತಾಂಶ->ಆವೃತ್ತಿ];
                choice_ಎಣಿಕೆ = 0;

                /* JIS7/8: try single-byte half-width Katakana before JISX208 */
                ಆದರೆ(converter_ದತ್ತಾಂಶ->ಆವೃತ್ತಿ == 3 || converter_ದತ್ತಾಂಶ->ಆವೃತ್ತಿ == 4) {
                    choices[choice_ಎಣಿಕೆ++] = static_cast<int8_t>(HWKANA_7_ಬಿಟ್);
                }
                /* Do not try single-byte half-width Katakana for other versions. */
                csm &= ~CSM(HWKANA_7_ಬಿಟ್);

                /* try the current G0 charset */
                choices[choice_ಎಣಿಕೆ++] = cs = pFromU2022State->cs[0];
                csm &= ~CSM(cs);

                /* try the current G2 charset */
                ಆದರೆ((cs = pFromU2022State->cs[2]) != 0) {
                    choices[choice_ಎಣಿಕೆ++] = cs;
                    csm &= ~CSM(cs);
                }

                /* try all the other possible charsets */
                ಗಾಗಿ(i = 0; i < UPRV_LENGTHOF(jpCharsetPref); ++i) {
                    cs = static_cast<int8_t>(jpCharsetPref[i]);
                    ಆದರೆ(CSM(cs) & csm) {
                        choices[choice_ಎಣಿಕೆ++] = cs;
                        csm &= ~CSM(cs);
                    }
                }
            }

            cs = g = 0;
            /*
             * len==0: no mapping found yet
             * len<0: found a fallback result: continue looking for a roundtrip but no further fallbacks
             * len>0: found a roundtrip result, done
             */
            len = 0;
            /*
             * We will turn off useFallback after finding a fallback,
             * but we still get fallbacks from PUA code points as usual.
             * Therefore, we will also need to check that we don't overwrite
             * an early fallback with a later one.
             */
            useFallback = cnv->useFallback;

            ಗಾಗಿ(i = 0; i < choice_ಎಣಿಕೆ && len <= 0; ++i) {
                uint32_t value;
                int32_t len2;
                int8_t cs0 = choices[i];
                ಆಯ್ಕೆ(cs0) {
                ಸ್ಥಿತಿ ASCII:
                    ಆದರೆ(ಮೂಲ_Char <= 0x7f) {
                        ಗುರಿ_ಮೌಲ್ಯ = static_cast<uint32_t>(ಮೂಲ_Char);
                        len = 1;
                        cs = cs0;
                        g = 0;
                    }
                    ನಿಲ್ಲಿಸು;
                ಸ್ಥಿತಿ ISO8859_1:
                    ಆದರೆ(GR96_ಆರಂಭ <= ಮೂಲ_Char && ಮೂಲ_Char <= GR96_ಕೊನೆ) {
                        ಗುರಿ_ಮೌಲ್ಯ = static_cast<uint32_t>(ಮೂಲ_Char) - 0x80;
                        len = 1;
                        cs = cs0;
                        g = 2;
                    }
                    ನಿಲ್ಲಿಸು;
                ಸ್ಥಿತಿ HWKANA_7_ಬಿಟ್:
                    ಆದರೆ (static_cast<uint32_t>(ಮೂಲ_Char - HWKANA_ಆರಂಭ) <= (HWKANA_ಕೊನೆ - HWKANA_ಆರಂಭ)) {
                        ಆದರೆ(converter_ದತ್ತಾಂಶ->ಆವೃತ್ತಿ==3) {
                            /* JIS7: use G1 (SO) */
                            /* Shift U+FF61..U+FF9F to bytes 21..5F. */
                            ಗುರಿ_ಮೌಲ್ಯ = static_cast<uint32_t>(ಮೂಲ_Char - (HWKANA_ಆರಂಭ - 0x21));
                            len = 1;
                            pFromU2022State->cs[1] = cs = cs0; /* do not output an escape sequence */
                            g = 1;
                        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(converter_ದತ್ತಾಂಶ->ಆವೃತ್ತಿ==4) {
                            /* JIS8: use 8-bit bytes with any single-byte charset, see escape sequence output below */
                            /* Shift U+FF61..U+FF9F to bytes A1..DF. */
                            ಗುರಿ_ಮೌಲ್ಯ = static_cast<uint32_t>(ಮೂಲ_Char - (HWKANA_ಆರಂಭ - 0xa1));
                            len = 1;

                            cs = pFromU2022State->cs[0];
                            ಆದರೆ(IS_JP_DBCS(cs)) {
                                /* switch from a DBCS charset to JISX201 */
                                cs = static_cast<int8_t>(JISX201);
                            }
                            /* else stay in the current G0 charset */
                            g = 0;
                        }
                        /* else do not use HWKANA_7BIT with other versions */
                    }
                    ನಿಲ್ಲಿಸು;
                ಸ್ಥಿತಿ JISX201:
                    /* G0 SBCS */
                    value = jisx201FromU(ಮೂಲ_Char);
                    ಆದರೆ(value <= 0x7f) {
                        ಗುರಿ_ಮೌಲ್ಯ = value;
                        len = 1;
                        cs = cs0;
                        g = 0;
                        useFallback = ಸುಳ್ಳು;
                    }
                    ನಿಲ್ಲಿಸು;
                ಸ್ಥಿತಿ JISX208:
                    /* G0 DBCS from Shift-JIS table */
                    len2 = MBCS_FROM_UCHAR32_ISO2022(
                                converter_ದತ್ತಾಂಶ->myConverterArray[cs0],
                                ಮೂಲ_Char, &value,
                                useFallback, MBCS_OUTPUT_2);
                    ಆದರೆ(len2 == 2 || (len2 == -2 && len == 0)) {  /* only accept DBCS: abs(len)==2 */
                        value = _2022FromSJIS(value);
                        ಆದರೆ(value != 0) {
                            ಗುರಿ_ಮೌಲ್ಯ = value;
                            len = len2;
                            cs = cs0;
                            g = 0;
                            useFallback = ಸುಳ್ಳು;
                        }
                    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(len == 0 && useFallback &&
                              static_cast<uint32_t>(ಮೂಲ_Char - HWKANA_ಆರಂಭ) <= (HWKANA_ಕೊನೆ - HWKANA_ಆರಂಭ)) {
                        ಗುರಿ_ಮೌಲ್ಯ = hwkana_fb[ಮೂಲ_Char - HWKANA_ಆರಂಭ];
                        len = -2;
                        cs = cs0;
                        g = 0;
                        useFallback = ಸುಳ್ಳು;
                    }
                    ನಿಲ್ಲಿಸು;
                ಸ್ಥಿತಿ ISO8859_7:
                    /* G0 SBCS forced to 7-bit output */
                    len2 = MBCS_SINGLE_FROM_UCHAR32(
                                converter_ದತ್ತಾಂಶ->myConverterArray[cs0],
                                ಮೂಲ_Char, &value,
                                useFallback);
                    ಆದರೆ(len2 != 0 && !(len2 < 0 && len != 0) && GR96_ಆರಂಭ <= value && value <= GR96_ಕೊನೆ) {
                        ಗುರಿ_ಮೌಲ್ಯ = value - 0x80;
                        len = len2;
                        cs = cs0;
                        g = 2;
                        useFallback = ಸುಳ್ಳು;
                    }
                    ನಿಲ್ಲಿಸು;
                ಪೂರ್ವನಿಯೋಜಿತ:
                    /* G0 DBCS */
                    len2 = MBCS_FROM_UCHAR32_ISO2022(
                                converter_ದತ್ತಾಂಶ->myConverterArray[cs0],
                                ಮೂಲ_Char, &value,
                                useFallback, MBCS_OUTPUT_2);
                    ಆದರೆ(len2 == 2 || (len2 == -2 && len == 0)) {  /* only accept DBCS: abs(len)==2 */
                        ಆದರೆ(cs0 == KSC5601) {
                            /*
                             * Check for valid bytes for the encoding scheme.
                             * This is necessary because the sub-converter (windows-949)
                             * has a broader encoding scheme than is valid for 2022.
                             */
                            value = _2022FromGR94DBCS(value);
                            ಆದರೆ(value == 0) {
                                ನಿಲ್ಲಿಸು;
                            }
                        }
                        ಗುರಿ_ಮೌಲ್ಯ = value;
                        len = len2;
                        cs = cs0;
                        g = 0;
                        useFallback = ಸುಳ್ಳು;
                    }
                    ನಿಲ್ಲಿಸು;
                }
            }

            ಆದರೆ(len != 0) {
                ಆದರೆ(len < 0) {
                    len = -len;  /* fallback */
                }
                ಹೊರಹೋಗು_Len = 0; /* count output bytes */

                /* write SI if necessary (only for JIS7) */
                ಆದರೆ(pFromU2022State->g == 1 && g == 0) {
                    ಬಫರ್[ಹೊರಹೋಗು_Len++] = UCNV_SI;
                    pFromU2022State->g = 0;
                }

                /* write the designation sequence if necessary */
                ಆದರೆ(cs != pFromU2022State->cs[g]) {
                    int32_t escLen = escSeqCharsLen[cs];
                    uprv_memcpy(ಬಫರ್ + ಹೊರಹೋಗು_Len, escSeqChars[cs], escLen);
                    ಹೊರಹೋಗು_Len += escLen;
                    pFromU2022State->cs[g] = cs;

                    /* invalidate the choices[] */
                    choice_ಎಣಿಕೆ = 0;
                }

                /* write the shift sequence if necessary */
                ಆದರೆ(g != pFromU2022State->g) {
                    ಆಯ್ಕೆ(g) {
                    /* case 0 handled before writing escapes */
                    ಸ್ಥಿತಿ 1:
                        ಬಫರ್[ಹೊರಹೋಗು_Len++] = UCNV_SO;
                        pFromU2022State->g = 1;
                        ನಿಲ್ಲಿಸು;
                    ಪೂರ್ವನಿಯೋಜಿತ: /* case 2 */
                        ಬಫರ್[ಹೊರಹೋಗು_Len++] = 0x1b;
                        ಬಫರ್[ಹೊರಹೋಗು_Len++] = 0x4e;
                        ನಿಲ್ಲಿಸು;
                    /* no case 3: no SS3 in ISO-2022-JP-x */
                    }
                }

                /* write the output bytes */
                ಆದರೆ(len == 1) {
                    ಬಫರ್[ಹೊರಹೋಗು_Len++] = static_cast<ಅಕ್ಷರ>(ಗುರಿ_ಮೌಲ್ಯ);
                } ಇಲ್ಲದಿದ್ದರೆ /* len == 2 */ {
                    ಬಫರ್[ಹೊರಹೋಗು_Len++] = static_cast<ಅಕ್ಷರ>(ಗುರಿ_ಮೌಲ್ಯ >> 8);
                    ಬಫರ್[ಹೊರಹೋಗು_Len++] = static_cast<ಅಕ್ಷರ>(ಗುರಿ_ಮೌಲ್ಯ);
                }
            } ಇಲ್ಲದಿದ್ದರೆ {
                /*
                 * if we cannot find the character after checking all codepages
                 * then this is an error
                 */
                *err = U_INVALID_CHAR_ಸಿಕ್ಕಿತು;
                cnv->fromUChar32=ಮೂಲ_Char;
                ನಿಲ್ಲಿಸು;
            }

            ಆದರೆ(ಮೂಲ_Char == CR || ಮೂಲ_Char == LF) {
                /* reset the G2 state at the end of a line (conversion got us into ASCII or JISX201 already) */
                pFromU2022State->cs[2] = 0;
                choice_ಎಣಿಕೆ = 0;
            }

            /* output outLen>0 bytes in buffer[] */
            ಆದರೆ(ಹೊರಹೋಗು_Len == 1) {
                *ಗುರಿ++ = ಬಫರ್[0];
                ಆದರೆ(offsets) {
                    *offsets++ = static_cast<int32_t>(ಮೂಲ - ವಾದಗಳು_2->ಮೂಲ - 1); /* -1: known to be ASCII */
                }
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಹೊರಹೋಗು_Len == 2 && (ಗುರಿ + 2) <= ಗುರಿ_ಎಲ್ಲೆ) {
                *ಗುರಿ++ = ಬಫರ್[0];
                *ಗುರಿ++ = ಬಫರ್[1];
                ಆದರೆ(offsets) {
                    int32_t ಮೂಲ_ಸೂಚ್ಯಂಕ = static_cast<int32_t>(ಮೂಲ - ವಾದಗಳು_2->ಮೂಲ - U16_ಉದ್ದಳತೆ(ಮೂಲ_Char));
                    *offsets++ = ಮೂಲ_ಸೂಚ್ಯಂಕ;
                    *offsets++ = ಮೂಲ_ಸೂಚ್ಯಂಕ;
                }
            } ಇಲ್ಲದಿದ್ದರೆ {
                fromUWriteUInt8(
                    cnv,
                    ಬಫರ್, ಹೊರಹೋಗು_Len,
                    &ಗುರಿ, reinterpret_cast<ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*>(ಗುರಿ_ಎಲ್ಲೆ),
                    &offsets, static_cast<int32_t>(ಮೂಲ - ವಾದಗಳು_2->ಮೂಲ - U16_ಉದ್ದಳತೆ(ಮೂಲ_Char)),
                    err);
                ಆದರೆ(U_FAILURE(*err)) {
                    ನಿಲ್ಲಿಸು;
                }
            }
        } /* end if(myTargetIndex<myTargetLength) */
        ಇಲ್ಲದಿದ್ದರೆ{
            *err =U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
            ನಿಲ್ಲಿಸು;
        }

    }/* end while(mySourceIndex<mySourceLength) */

    /*
     * the end of the input stream and detection of truncated input
     * are handled by the framework, but for ISO-2022-JP conversion
     * we need to be in ASCII mode at the very end
     *
     * conditions:
     *   successful
     *   in SO mode or not in ASCII mode
     *   end of input and no truncated input
     */
    ಆದರೆ( U_SUCCESS(*err) &&
        (pFromU2022State->g!=0 || pFromU2022State->cs[0]!=ASCII) &&
        ವಾದಗಳು_2->flush && ಮೂಲ>=ಮೂಲ_ಎಲ್ಲೆ && cnv->fromUChar32==0
    ) {
        int32_t ಮೂಲ_ಸೂಚ್ಯಂಕ;

        ಹೊರಹೋಗು_Len = 0;

        ಆದರೆ(pFromU2022State->g != 0) {
            ಬಫರ್[ಹೊರಹೋಗು_Len++] = UCNV_SI;
            pFromU2022State->g = 0;
        }

        ಆದರೆ(pFromU2022State->cs[0] != ASCII) {
            int32_t escLen = escSeqCharsLen[ASCII];
            uprv_memcpy(ಬಫರ್ + ಹೊರಹೋಗು_Len, escSeqChars[ASCII], escLen);
            ಹೊರಹೋಗು_Len += escLen;
            pFromU2022State->cs[0] = static_cast<int8_t>(ASCII);
        }

        /* get the source index of the last input character */
        /*
         * TODO this would be simpler and more reliable if we used a pair
         * of sourceIndex/prevSourceIndex like in ucnvmbcs.c
         * so that we could simply use the prevSourceIndex here;
         * this code gives an incorrect result for the rare case of an unmatched
         * trail surrogate that is alone in the last buffer of the text stream
         */
        ಮೂಲ_ಸೂಚ್ಯಂಕ = static_cast<int32_t>(ಮೂಲ - ವಾದಗಳು_2->ಮೂಲ);
        ಆದರೆ(ಮೂಲ_ಸೂಚ್ಯಂಕ>0) {
            --ಮೂಲ_ಸೂಚ್ಯಂಕ;
            ಆದರೆ( U16_IS_TRAIL(ವಾದಗಳು_2->ಮೂಲ[ಮೂಲ_ಸೂಚ್ಯಂಕ]) &&
                (ಮೂಲ_ಸೂಚ್ಯಂಕ==0 || U16_IS_LEAD(ವಾದಗಳು_2->ಮೂಲ[ಮೂಲ_ಸೂಚ್ಯಂಕ-1]))
            ) {
                --ಮೂಲ_ಸೂಚ್ಯಂಕ;
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಮೂಲ_ಸೂಚ್ಯಂಕ=-1;
        }

        fromUWriteUInt8(
            cnv,
            ಬಫರ್, ಹೊರಹೋಗು_Len,
            &ಗುರಿ, reinterpret_cast<ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*>(ಗುರಿ_ಎಲ್ಲೆ),
            &offsets, ಮೂಲ_ಸೂಚ್ಯಂಕ,
            err);
    }

    /*save the state and return */
    ವಾದಗಳು_2->ಮೂಲ = ಮೂಲ;
    ವಾದಗಳು_2->ಗುರಿ = reinterpret_cast<ಅಕ್ಷರ*>(ಗುರಿ);
}

/*************** to unicode *******************/

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
UConverter_ಗಮ್ಯ_Unicode_ISO_2022_JP_OFFSETS_LOGIC(UConverter_ಗಮ್ಯ_Unicode_ವಾದಗಳು *ವಾದಗಳು_2,
                                               UError_ಸಂಕೇತ* err){
    ಅಕ್ಷರ ತಾತ್ಕಾಲಿಕ_Buf[2];
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* my_ಮೂಲ = const_cast<ಅಕ್ಷರ*>(ವಾದಗಳು_2->ಮೂಲ);
    char16_t *my_ಗುರಿ = ವಾದಗಳು_2->ಗುರಿ;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *my_ಮೂಲ_ಎಲ್ಲೆ = ವಾದಗಳು_2->ಮೂಲ_ಎಲ್ಲೆ;
    uint32_t ಗುರಿ_Uni_Char = 0x0000;
    uint32_t my_ಮೂಲ_Char = 0x0000;
    uint32_t tmp_ಮೂಲ_Char = 0x0000;
    UConverter_ದತ್ತಾಂಶ_ISO2022* my_ದತ್ತಾಂಶ;
    ISO2022State *p_ಗಮ್ಯ_U2022_State;
    StateEnum cs;

    my_ದತ್ತಾಂಶ = static_cast<UConverter_ದತ್ತಾಂಶ_ISO2022*>(ವಾದಗಳು_2->converter->extraInfo);
    p_ಗಮ್ಯ_U2022_State = &my_ದತ್ತಾಂಶ->ಗಮ್ಯ_U2022_State;

    ಆದರೆ(my_ದತ್ತಾಂಶ->ಕೀಲಿ != 0) {
        /* continue with a partial escape sequence */
        ಹೋಗು escape;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ವಾದಗಳು_2->converter->ಗಮ್ಯ_ULength == 1 && my_ಮೂಲ < my_ಮೂಲ_ಎಲ್ಲೆ && my_ಗುರಿ < ವಾದಗಳು_2->ಗುರಿ_ಎಲ್ಲೆ) {
        /* continue with a partial double-byte character */
        my_ಮೂಲ_Char = ವಾದಗಳು_2->converter->ಗಮ್ಯ_UBytes[0];
        ವಾದಗಳು_2->converter->ಗಮ್ಯ_ULength = 0;
        cs = static_cast<StateEnum>(p_ಗಮ್ಯ_U2022_State->cs[p_ಗಮ್ಯ_U2022_State->g]);
        ಗುರಿ_Uni_Char = missingCharMarker;
        ಹೋಗು getTrailByte;
    }

    ಆಗ(my_ಮೂಲ < my_ಮೂಲ_ಎಲ್ಲೆ){

        ಗುರಿ_Uni_Char =missingCharMarker;

        ಆದರೆ(my_ಗುರಿ < ವಾದಗಳು_2->ಗುರಿ_ಎಲ್ಲೆ){

            my_ಮೂಲ_Char = static_cast<ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ>(*my_ಮೂಲ++);

            ಆಯ್ಕೆ(my_ಮೂಲ_Char) {
            ಸ್ಥಿತಿ UCNV_SI:
                ಆದರೆ(my_ದತ್ತಾಂಶ->ಆವೃತ್ತಿ==3) {
                    p_ಗಮ್ಯ_U2022_State->g=0;
                    ಮುಂದುವರಿ;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    /* only JIS7 uses SI/SO, not ISO-2022-JP-x */
                    my_ದತ್ತಾಂಶ->is_ಖಾಲಿಗಳು_Segment = ಸುಳ್ಳು;	/* reset this, we have a different error */
                    ನಿಲ್ಲಿಸು;
                }

            ಸ್ಥಿತಿ UCNV_SO:
                ಆದರೆ(my_ದತ್ತಾಂಶ->ಆವೃತ್ತಿ==3) {
                    /* JIS7: switch to G1 half-width Katakana */
                    p_ಗಮ್ಯ_U2022_State->cs[1] = static_cast<int8_t>(HWKANA_7_ಬಿಟ್);
                    p_ಗಮ್ಯ_U2022_State->g=1;
                    ಮುಂದುವರಿ;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    /* only JIS7 uses SI/SO, not ISO-2022-JP-x */
                    my_ದತ್ತಾಂಶ->is_ಖಾಲಿಗಳು_Segment = ಸುಳ್ಳು;	/* reset this, we have a different error */
                    ನಿಲ್ಲಿಸು;
                }

            ಸ್ಥಿತಿ ESC_2022:
                my_ಮೂಲ--;
escape:
                {
                    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * my_ಮೂಲ_Before = my_ಮೂಲ;
                    int8_t ಗಮ್ಯ_ULength_Before = ವಾದಗಳು_2->converter->ಗಮ್ಯ_ULength;

                    changeState_2022(ವಾದಗಳು_2->converter,&(my_ಮೂಲ),
                        my_ಮೂಲ_ಎಲ್ಲೆ, ISO_2022_JP,err);

                    /* If in ISO-2022-JP only and we successfully completed an escape sequence, but previous segment was empty, create an error */
                    ಆದರೆ(my_ದತ್ತಾಂಶ->ಆವೃತ್ತಿ==0 && my_ದತ್ತಾಂಶ->ಕೀಲಿ==0 && U_SUCCESS(*err) && my_ದತ್ತಾಂಶ->is_ಖಾಲಿಗಳು_Segment) {
                        *err = U_ILLEGAL_ESCAPE_SEQUENCE;
                        ವಾದಗಳು_2->converter->ಗಮ್ಯ_UCallback_Reason = UCNV_IRREGULAR;
                        ವಾದಗಳು_2->converter->ಗಮ್ಯ_ULength = static_cast<int8_t>(ಗಮ್ಯ_ULength_Before + (my_ಮೂಲ - my_ಮೂಲ_Before));
                    }
                }

                /* invalid or illegal escape sequence */
                ಆದರೆ(U_FAILURE(*err)){
                    ವಾದಗಳು_2->ಗುರಿ = my_ಗುರಿ;
                    ವಾದಗಳು_2->ಮೂಲ = my_ಮೂಲ;
                    my_ದತ್ತಾಂಶ->is_ಖಾಲಿಗಳು_Segment = ಸುಳ್ಳು;	/* Reset to avoid future spurious errors */
                    ಹಿಂತಿರುಗಿ;
                }
                /* If we successfully completed an escape sequence, we begin a new segment, empty so far */
                ಆದರೆ(my_ದತ್ತಾಂಶ->ಕೀಲಿ==0) {
                    my_ದತ್ತಾಂಶ->is_ಖಾಲಿಗಳು_Segment = ನಿಜ;
                }
                ಮುಂದುವರಿ;

            /* ISO-2022-JP does not use single-byte (C1) SS2 and SS3 */

            ಸ್ಥಿತಿ CR:
            ಸ್ಥಿತಿ LF:
                /* automatically reset to single-byte mode */
                ಆದರೆ (static_cast<StateEnum>(p_ಗಮ್ಯ_U2022_State->cs[0]) != ASCII &&
                    static_cast<StateEnum>(p_ಗಮ್ಯ_U2022_State->cs[0]) != JISX201) {
                    p_ಗಮ್ಯ_U2022_State->cs[0] = static_cast<int8_t>(ASCII);
                }
                p_ಗಮ್ಯ_U2022_State->cs[2] = 0;
                p_ಗಮ್ಯ_U2022_State->g = 0;
                U_FALLTHROUGH;
            ಪೂರ್ವನಿಯೋಜಿತ:
                /* convert one or two bytes */
                my_ದತ್ತಾಂಶ->is_ಖಾಲಿಗಳು_Segment = ಸುಳ್ಳು;
                cs = static_cast<StateEnum>(p_ಗಮ್ಯ_U2022_State->cs[p_ಗಮ್ಯ_U2022_State->g]);
                ಆದರೆ (static_cast<uint8_t>(my_ಮೂಲ_Char - 0xa1) <= (0xdf - 0xa1) && my_ದತ್ತಾಂಶ->ಆವೃತ್ತಿ == 4 &&
                    !IS_JP_DBCS(cs)
                ) {
                    /* 8-bit halfwidth katakana in any single-byte mode for JIS8 */
                    ಗುರಿ_Uni_Char = my_ಮೂಲ_Char + (HWKANA_ಆರಂಭ - 0xa1);

                    /* return from a single-shift state to the previous one */
                    ಆದರೆ(p_ಗಮ್ಯ_U2022_State->g >= 2) {
                        p_ಗಮ್ಯ_U2022_State->g=p_ಗಮ್ಯ_U2022_State->prevG;
                    }
                } ಇಲ್ಲದಿದ್ದರೆ ಆಯ್ಕೆ(cs) {
                ಸ್ಥಿತಿ ASCII:
                    ಆದರೆ(my_ಮೂಲ_Char <= 0x7f) {
                        ಗುರಿ_Uni_Char = my_ಮೂಲ_Char;
                    }
                    ನಿಲ್ಲಿಸು;
                ಸ್ಥಿತಿ ISO8859_1:
                    ಆದರೆ(my_ಮೂಲ_Char <= 0x7f) {
                        ಗುರಿ_Uni_Char = my_ಮೂಲ_Char + 0x80;
                    }
                    /* return from a single-shift state to the previous one */
                    p_ಗಮ್ಯ_U2022_State->g=p_ಗಮ್ಯ_U2022_State->prevG;
                    ನಿಲ್ಲಿಸು;
                ಸ್ಥಿತಿ ISO8859_7:
                    ಆದರೆ(my_ಮೂಲ_Char <= 0x7f) {
                        /* convert mySourceChar+0x80 to use a normal 8-bit table */
                        ಗುರಿ_Uni_Char =
                            MBCS_SINGLE_SIMPLE_GET_ಮುಂದಿನ_BMP(
                                my_ದತ್ತಾಂಶ->myConverterArray[cs],
                                my_ಮೂಲ_Char + 0x80);
                    }
                    /* return from a single-shift state to the previous one */
                    p_ಗಮ್ಯ_U2022_State->g=p_ಗಮ್ಯ_U2022_State->prevG;
                    ನಿಲ್ಲಿಸು;
                ಸ್ಥಿತಿ JISX201:
                    ಆದರೆ(my_ಮೂಲ_Char <= 0x7f) {
                        ಗುರಿ_Uni_Char = jisx201_ಗಮ್ಯ_U(my_ಮೂಲ_Char);
                    }
                    ನಿಲ್ಲಿಸು;
                ಸ್ಥಿತಿ HWKANA_7_ಬಿಟ್:
                    ಆದರೆ (static_cast<uint8_t>(my_ಮೂಲ_Char - 0x21) <= (0x5f - 0x21)) {
                        /* 7-bit halfwidth Katakana */
                        ಗುರಿ_Uni_Char = my_ಮೂಲ_Char + (HWKANA_ಆರಂಭ - 0x21);
                    }
                    ನಿಲ್ಲಿಸು;
                ಪೂರ್ವನಿಯೋಜಿತ:
                    /* G0 DBCS */
                    ಆದರೆ(my_ಮೂಲ < my_ಮೂಲ_ಎಲ್ಲೆ) {
                        ಪೂರ್ಣಾಂಕ leadIsOk, trailIsOk;
                        uint8_t trailByte;
getTrailByte:
                        trailByte = static_cast<uint8_t>(*my_ಮೂಲ);
                        /*
                         * Ticket 5691: consistent illegal sequences:
                         * - We include at least the first byte in the illegal sequence.
                         * - If any of the non-initial bytes could be the start of a character,
                         *   we stop the illegal sequence before the first one of those.
                         *
                         * In ISO-2022 DBCS, if the second byte is in the 21..7e range or is
                         * an ESC/SO/SI, we report only the first byte as the illegal sequence.
                         * Otherwise we convert or report the pair of bytes.
                         */
                        leadIsOk = static_cast<uint8_t>(my_ಮೂಲ_Char - 0x21) <= (0x7e - 0x21);
                        trailIsOk = static_cast<uint8_t>(trailByte - 0x21) <= (0x7e - 0x21);
                        ಆದರೆ (leadIsOk && trailIsOk) {
                            ++my_ಮೂಲ;
                            tmp_ಮೂಲ_Char = (my_ಮೂಲ_Char << 8) | trailByte;
                            ಆದರೆ(cs == JISX208) {
                                _2022ToSJIS(static_cast<uint8_t>(my_ಮೂಲ_Char), trailByte, ತಾತ್ಕಾಲಿಕ_Buf);
                                my_ಮೂಲ_Char = tmp_ಮೂಲ_Char;
                            } ಇಲ್ಲದಿದ್ದರೆ {
                                /* Copy before we modify tmpSourceChar so toUnicodeCallback() sees the correct bytes. */
                                my_ಮೂಲ_Char = tmp_ಮೂಲ_Char;
                                ಆದರೆ (cs == KSC5601) {
                                    tmp_ಮೂಲ_Char += 0x8080;  /* = _2022ToGR94DBCS(tmpSourceChar) */
                                }
                                ತಾತ್ಕಾಲಿಕ_Buf[0] = static_cast<ಅಕ್ಷರ>(tmp_ಮೂಲ_Char >> 8);
                                ತಾತ್ಕಾಲಿಕ_Buf[1] = static_cast<ಅಕ್ಷರ>(tmp_ಮೂಲ_Char);
                            }
                            ಗುರಿ_Uni_Char = ucnv_MBCSSimple_Get_ಮುಂದಿನ_UChar(my_ದತ್ತಾಂಶ->myConverterArray[cs], ತಾತ್ಕಾಲಿಕ_Buf, 2, ಸುಳ್ಳು);
                        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!(trailIsOk || IS_2022_CONTROL(trailByte))) {
                            /* report a pair of illegal bytes if the second byte is not a DBCS starter */
                            ++my_ಮೂಲ;
                            /* add another bit so that the code below writes 2 bytes in case of error */
                            my_ಮೂಲ_Char = 0x10000 | (my_ಮೂಲ_Char << 8) | trailByte;
                        }
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        ವಾದಗಳು_2->converter->ಗಮ್ಯ_UBytes[0] = static_cast<uint8_t>(my_ಮೂಲ_Char);
                        ವಾದಗಳು_2->converter->ಗಮ್ಯ_ULength = 1;
                        ಹೋಗು endloop;
                    }
                }  /* End of inner switch */
                ನಿಲ್ಲಿಸು;
            }  /* End of outer switch */
            ಆದರೆ(ಗುರಿ_Uni_Char < (missingCharMarker-1/*0xfffe*/)){
                ಆದರೆ(ವಾದಗಳು_2->offsets){
                    ವಾದಗಳು_2->offsets[my_ಗುರಿ - ವಾದಗಳು_2->ಗುರಿ] = static_cast<int32_t>(my_ಮೂಲ - ವಾದಗಳು_2->ಮೂಲ - (my_ಮೂಲ_Char <= 0xff ? 1 : 2));
                }
                *(my_ಗುರಿ++) = static_cast<char16_t>(ಗುರಿ_Uni_Char);
            }
            ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಗುರಿ_Uni_Char > missingCharMarker){
                /* disassemble the surrogate pair and write to output*/
                ಗುರಿ_Uni_Char-=0x0010000;
                *my_ಗುರಿ = static_cast<char16_t>(0xd800 + static_cast<char16_t>(ಗುರಿ_Uni_Char >> 10));
                ಆದರೆ(ವಾದಗಳು_2->offsets){
                    ವಾದಗಳು_2->offsets[my_ಗುರಿ - ವಾದಗಳು_2->ಗುರಿ] = static_cast<int32_t>(my_ಮೂಲ - ವಾದಗಳು_2->ಮೂಲ - (my_ಮೂಲ_Char <= 0xff ? 1 : 2));
                }
                ++my_ಗುರಿ;
                ಆದರೆ(my_ಗುರಿ< ವಾದಗಳು_2->ಗುರಿ_ಎಲ್ಲೆ){
                    *my_ಗುರಿ = static_cast<char16_t>(0xdc00 + static_cast<char16_t>(ಗುರಿ_Uni_Char & 0x3ff));
                    ಆದರೆ(ವಾದಗಳು_2->offsets){
                        ವಾದಗಳು_2->offsets[my_ಗುರಿ - ವಾದಗಳು_2->ಗುರಿ] = static_cast<int32_t>(my_ಮೂಲ - ವಾದಗಳು_2->ಮೂಲ - (my_ಮೂಲ_Char <= 0xff ? 1 : 2));
                    }
                    ++my_ಗುರಿ;
                }ಇಲ್ಲದಿದ್ದರೆ{
                    ವಾದಗಳು_2->converter->UChar_Error_ಬಫರ್[ವಾದಗಳು_2->converter->UChar_Error_ಬಫರ್_ಉದ್ದಳತೆ++]=
                                    static_cast<char16_t>(0xdc00 + static_cast<char16_t>(ಗುರಿ_Uni_Char & 0x3ff));
                }

            }
            ಇಲ್ಲದಿದ್ದರೆ{
                /* Call the callback function*/
                ಗಮ್ಯ_Unicode_Callback(ವಾದಗಳು_2->converter,my_ಮೂಲ_Char,ಗುರಿ_Uni_Char,err);
                ನಿಲ್ಲಿಸು;
            }
        }
        ಇಲ್ಲದಿದ್ದರೆ{    /* goes with "if(myTarget < args->targetLimit)"  way up near top of function */
            *err =U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
            ನಿಲ್ಲಿಸು;
        }
    }
endloop:
    ವಾದಗಳು_2->ಗುರಿ = my_ಗುರಿ;
    ವಾದಗಳು_2->ಮೂಲ = my_ಮೂಲ;
}


#ಆದರೆ !UCONFIG_ONLY_HTML_CONVERSION
/***************************************************************
*   Rules for ISO-2022-KR encoding
*   i) The KSC5601 designator sequence should appear only once in a file,
*      at the beginning of a line before any KSC5601 characters. This usually
*      means that it appears by itself on the first line of the file
*  ii) There are only 2 shifting sequences SO to shift into double byte mode
*      and SI to shift into single byte mode
*/
ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
UConverter_fromUnicode_ISO_2022_KR_OFFSETS_LOGIC_IBM(UConverter_From_Unicode_ವಾದಗಳು* ವಾದಗಳು_2, UError_ಸಂಕೇತ* err){

    UConverter* saveConv = ವಾದಗಳು_2->converter;
    UConverter_ದತ್ತಾಂಶ_ISO2022* my_Converter_ದತ್ತಾಂಶ = static_cast<UConverter_ದತ್ತಾಂಶ_ISO2022*>(saveConv->extraInfo);
    ವಾದಗಳು_2->converter=my_Converter_ದತ್ತಾಂಶ->currentConverter;

    my_Converter_ದತ್ತಾಂಶ->currentConverter->fromUChar32 = saveConv->fromUChar32;
    ucnv_MBCSFromUnicodeWithOffsets(ವಾದಗಳು_2,err);
    saveConv->fromUChar32 = my_Converter_ದತ್ತಾಂಶ->currentConverter->fromUChar32;

    ಆದರೆ(*err == U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR) {
        ಆದರೆ(my_Converter_ದತ್ತಾಂಶ->currentConverter->char_Error_ಬಫರ್_ಉದ್ದಳತೆ > 0) {
            uprv_memcpy(
                saveConv->char_Error_ಬಫರ್,
                my_Converter_ದತ್ತಾಂಶ->currentConverter->char_Error_ಬಫರ್,
                my_Converter_ದತ್ತಾಂಶ->currentConverter->char_Error_ಬಫರ್_ಉದ್ದಳತೆ);
        }
        saveConv->char_Error_ಬಫರ್_ಉದ್ದಳತೆ = my_Converter_ದತ್ತಾಂಶ->currentConverter->char_Error_ಬಫರ್_ಉದ್ದಳತೆ;
        my_Converter_ದತ್ತಾಂಶ->currentConverter->char_Error_ಬಫರ್_ಉದ್ದಳತೆ = 0;
    }
    ವಾದಗಳು_2->converter=saveConv;
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
UConverter_fromUnicode_ISO_2022_KR_OFFSETS_LOGIC(UConverter_From_Unicode_ವಾದಗಳು* ವಾದಗಳು_2, UError_ಸಂಕೇತ* err){

    ಸ್ಥಿರಾಂಕ char16_t *ಮೂಲ = ವಾದಗಳು_2->ಮೂಲ;
    ಸ್ಥಿರಾಂಕ char16_t *ಮೂಲ_ಎಲ್ಲೆ = ವಾದಗಳು_2->ಮೂಲ_ಎಲ್ಲೆ;
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಗುರಿ = reinterpret_cast<ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ*>(ವಾದಗಳು_2->ಗುರಿ);
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಗುರಿ_ಎಲ್ಲೆ = reinterpret_cast<ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ*>(const_cast<ಅಕ್ಷರ*>(ವಾದಗಳು_2->ಗುರಿ_ಎಲ್ಲೆ));
    int32_t* offsets = ವಾದಗಳು_2->offsets;
    uint32_t ಗುರಿ_Byte_Unit = 0x0000;
    UChar32 ಮೂಲ_Char = 0x0000;
    UBool is_ಗುರಿ_Byte_DBCS;
    UBool old_Is_ಗುರಿ_Byte_DBCS;
    UConverter_ದತ್ತಾಂಶ_ISO2022 *converter_ದತ್ತಾಂಶ;
    UConverter_Shared_ದತ್ತಾಂಶ* shared_ದತ್ತಾಂಶ;
    UBool useFallback;
    int32_t length =0;

    converter_ದತ್ತಾಂಶ = static_cast<UConverter_ದತ್ತಾಂಶ_ISO2022*>(ವಾದಗಳು_2->converter->extraInfo);
    /* if the version is 1 then the user is requesting
     * conversion with ibm-25546 pass the arguments to
     * MBCS converter and return
     */
    ಆದರೆ(converter_ದತ್ತಾಂಶ->ಆವೃತ್ತಿ==1){
        UConverter_fromUnicode_ISO_2022_KR_OFFSETS_LOGIC_IBM(ವಾದಗಳು_2,err);
        ಹಿಂತಿರುಗಿ;
    }

    /* initialize data */
    shared_ದತ್ತಾಂಶ = converter_ದತ್ತಾಂಶ->currentConverter->shared_ದತ್ತಾಂಶ;
    useFallback = ವಾದಗಳು_2->converter->useFallback;
    is_ಗುರಿ_Byte_DBCS = static_cast<UBool>(ವಾದಗಳು_2->converter->from_Unicode_ಸ್ಥಿತಿ);
    old_Is_ಗುರಿ_Byte_DBCS = is_ಗುರಿ_Byte_DBCS;

    is_ಗುರಿ_Byte_DBCS = static_cast<UBool>(ವಾದಗಳು_2->converter->from_Unicode_ಸ್ಥಿತಿ);
    ಆದರೆ((ಮೂಲ_Char = ವಾದಗಳು_2->converter->fromUChar32)!=0 && ಗುರಿ <ಗುರಿ_ಎಲ್ಲೆ) {
        ಹೋಗು getTrail;
    }
    ಆಗ(ಮೂಲ < ಮೂಲ_ಎಲ್ಲೆ){

        ಗುರಿ_Byte_Unit = missingCharMarker;

        ಆದರೆ(ಗುರಿ < (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ*) ವಾದಗಳು_2->ಗುರಿ_ಎಲ್ಲೆ){
            ಮೂಲ_Char = *ಮೂಲ++;

            /* do not convert SO/SI/ESC */
            ಆದರೆ(IS_2022_CONTROL(ಮೂಲ_Char)) {
                /* callback(illegal) */
                *err=U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
                ವಾದಗಳು_2->converter->fromUChar32=ಮೂಲ_Char;
                ನಿಲ್ಲಿಸು;
            }

            length = MBCS_FROM_UCHAR32_ISO2022(shared_ದತ್ತಾಂಶ,ಮೂಲ_Char,&ಗುರಿ_Byte_Unit,useFallback,MBCS_OUTPUT_2);
            ಆದರೆ(length < 0) {
                length = -length;  /* fallback */
            }
            /* only DBCS or SBCS characters are expected*/
            /* DB characters with high bit set to 1 are expected */
            ಆದರೆ( length > 2 || length==0 ||
                (length == 1 && ಗುರಿ_Byte_Unit > 0x7f) ||
                (length == 2 &&
                    (static_cast<uint16_t>(ಗುರಿ_Byte_Unit - 0xa1a1) > (0xfefe - 0xa1a1) ||
                    static_cast<uint8_t>(ಗುರಿ_Byte_Unit - 0xa1) > (0xfe - 0xa1)))
            ) {
                ಗುರಿ_Byte_Unit=missingCharMarker;
            }
            ಆದರೆ (ಗುರಿ_Byte_Unit != missingCharMarker){

                old_Is_ಗುರಿ_Byte_DBCS = is_ಗುರಿ_Byte_DBCS;
                is_ಗುರಿ_Byte_DBCS = static_cast<UBool>(ಗುರಿ_Byte_Unit > 0x00FF);
                  /* append the shift sequence */
                ಆದರೆ (old_Is_ಗುರಿ_Byte_DBCS != is_ಗುರಿ_Byte_DBCS ){

                    ಆದರೆ (is_ಗುರಿ_Byte_DBCS)
                        *ಗುರಿ++ = UCNV_SO;
                    ಇಲ್ಲದಿದ್ದರೆ
                        *ಗುರಿ++ = UCNV_SI;
                    ಆದರೆ(offsets)
                        *(offsets++) = static_cast<int32_t>(ಮೂಲ - ವಾದಗಳು_2->ಮೂಲ - 1);
                }
                /* write the targetUniChar  to target */
                ಆದರೆ(ಗುರಿ_Byte_Unit <= 0x00FF){
                    ಆದರೆ( ಗುರಿ < ಗುರಿ_ಎಲ್ಲೆ){
                        *(ಗುರಿ++) = static_cast<ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ>(ಗುರಿ_Byte_Unit);
                        ಆದರೆ(offsets){
                            *(offsets++) = static_cast<int32_t>(ಮೂಲ - ವಾದಗಳು_2->ಮೂಲ - 1);
                        }

                    }ಇಲ್ಲದಿದ್ದರೆ{
                        ವಾದಗಳು_2->converter->char_Error_ಬಫರ್[ವಾದಗಳು_2->converter->char_Error_ಬಫರ್_ಉದ್ದಳತೆ++] = static_cast<ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ>(ಗುರಿ_Byte_Unit);
                        *err = U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
                    }
                }ಇಲ್ಲದಿದ್ದರೆ{
                    ಆದರೆ(ಗುರಿ < ಗುರಿ_ಎಲ್ಲೆ){
                        *(ಗುರಿ++) = static_cast<ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ>((ಗುರಿ_Byte_Unit >> 8) - 0x80);
                        ಆದರೆ(offsets){
                            *(offsets++) = static_cast<int32_t>(ಮೂಲ - ವಾದಗಳು_2->ಮೂಲ - 1);
                        }
                        ಆದರೆ(ಗುರಿ < ಗುರಿ_ಎಲ್ಲೆ){
                            *(ಗುರಿ++) = static_cast<ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ>(ಗುರಿ_Byte_Unit - 0x80);
                            ಆದರೆ(offsets){
                                *(offsets++) = static_cast<int32_t>(ಮೂಲ - ವಾದಗಳು_2->ಮೂಲ - 1);
                            }
                        }ಇಲ್ಲದಿದ್ದರೆ{
                            ವಾದಗಳು_2->converter->char_Error_ಬಫರ್[ವಾದಗಳು_2->converter->char_Error_ಬಫರ್_ಉದ್ದಳತೆ++] = static_cast<ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ>(ಗುರಿ_Byte_Unit - 0x80);
                            *err = U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
                        }
                    }ಇಲ್ಲದಿದ್ದರೆ{
                        ವಾದಗಳು_2->converter->char_Error_ಬಫರ್[ವಾದಗಳು_2->converter->char_Error_ಬಫರ್_ಉದ್ದಳತೆ++] = static_cast<ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ>((ಗುರಿ_Byte_Unit >> 8) - 0x80);
                        ವಾದಗಳು_2->converter->char_Error_ಬಫರ್[ವಾದಗಳು_2->converter->char_Error_ಬಫರ್_ಉದ್ದಳತೆ++] = static_cast<ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ>(ಗುರಿ_Byte_Unit - 0x80);
                        *err = U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
                    }
                }

            }
            ಇಲ್ಲದಿದ್ದರೆ{
                /* oops.. the code point is unassingned
                 * set the error and reason
                 */

                /*check if the char is a First surrogate*/
                ಆದರೆ(U16_IS_SURROGATE(ಮೂಲ_Char)) {
                    ಆದರೆ(U16_IS_SURROGATE_LEAD(ಮೂಲ_Char)) {
getTrail:
                        /*look ahead to find the trail surrogate*/
                        ಆದರೆ(ಮೂಲ <  ಮೂಲ_ಎಲ್ಲೆ) {
                            /* test the following code unit */
                            char16_t trail = *ಮೂಲ;
                            ಆದರೆ(U16_IS_TRAIL(trail)) {
                                ಮೂಲ++;
                                ಮೂಲ_Char=U16_GET_SUPPLEMENTARY(ಮೂಲ_Char, trail);
                                *err = U_INVALID_CHAR_ಸಿಕ್ಕಿತು;
                                /* convert this surrogate code point */
                                /* exit this condition tree */
                            } ಇಲ್ಲದಿದ್ದರೆ {
                                /* this is an unmatched lead code unit (1st surrogate) */
                                /* callback(illegal) */
                                *err=U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
                            }
                        } ಇಲ್ಲದಿದ್ದರೆ {
                            /* no more input */
                            *err = U_ZERO_ERROR;
                        }
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *err=U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
                    }
                } ಇಲ್ಲದಿದ್ದರೆ {
                    /* callback(unassigned) for a BMP code point */
                    *err = U_INVALID_CHAR_ಸಿಕ್ಕಿತು;
                }

                ವಾದಗಳು_2->converter->fromUChar32=ಮೂಲ_Char;
                ನಿಲ್ಲಿಸು;
            }
        } /* end if(myTargetIndex<myTargetLength) */
        ಇಲ್ಲದಿದ್ದರೆ{
            *err =U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
            ನಿಲ್ಲಿಸು;
        }

    }/* end while(mySourceIndex<mySourceLength) */

    /*
     * the end of the input stream and detection of truncated input
     * are handled by the framework, but for ISO-2022-KR conversion
     * we need to be in ASCII mode at the very end
     *
     * conditions:
     *   successful
     *   not in ASCII mode
     *   end of input and no truncated input
     */
    ಆದರೆ( U_SUCCESS(*err) &&
        is_ಗುರಿ_Byte_DBCS &&
        ವಾದಗಳು_2->flush && ಮೂಲ>=ಮೂಲ_ಎಲ್ಲೆ && ವಾದಗಳು_2->converter->fromUChar32==0
    ) {
        int32_t ಮೂಲ_ಸೂಚ್ಯಂಕ;

        /* we are switching to ASCII */
        is_ಗುರಿ_Byte_DBCS=ಸುಳ್ಳು;

        /* get the source index of the last input character */
        /*
         * TODO this would be simpler and more reliable if we used a pair
         * of sourceIndex/prevSourceIndex like in ucnvmbcs.c
         * so that we could simply use the prevSourceIndex here;
         * this code gives an incorrect result for the rare case of an unmatched
         * trail surrogate that is alone in the last buffer of the text stream
         */
        ಮೂಲ_ಸೂಚ್ಯಂಕ = static_cast<int32_t>(ಮೂಲ - ವಾದಗಳು_2->ಮೂಲ);
        ಆದರೆ(ಮೂಲ_ಸೂಚ್ಯಂಕ>0) {
            --ಮೂಲ_ಸೂಚ್ಯಂಕ;
            ಆದರೆ( U16_IS_TRAIL(ವಾದಗಳು_2->ಮೂಲ[ಮೂಲ_ಸೂಚ್ಯಂಕ]) &&
                (ಮೂಲ_ಸೂಚ್ಯಂಕ==0 || U16_IS_LEAD(ವಾದಗಳು_2->ಮೂಲ[ಮೂಲ_ಸೂಚ್ಯಂಕ-1]))
            ) {
                --ಮೂಲ_ಸೂಚ್ಯಂಕ;
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಮೂಲ_ಸೂಚ್ಯಂಕ=-1;
        }

        fromUWriteUInt8(
            ವಾದಗಳು_2->converter,
            ಸರಿಸು_ಒಳ_STR, 1,
            &ಗುರಿ, reinterpret_cast<ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*>(ಗುರಿ_ಎಲ್ಲೆ),
            &offsets, ಮೂಲ_ಸೂಚ್ಯಂಕ,
            err);
    }

    /*save the state and return */
    ವಾದಗಳು_2->ಮೂಲ = ಮೂಲ;
    ವಾದಗಳು_2->ಗುರಿ = reinterpret_cast<ಅಕ್ಷರ*>(ಗುರಿ);
    ವಾದಗಳು_2->converter->from_Unicode_ಸ್ಥಿತಿ = static_cast<uint32_t>(is_ಗುರಿ_Byte_DBCS);
}

/************************ To Unicode ***************************************/

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
UConverter_ಗಮ್ಯ_Unicode_ISO_2022_KR_OFFSETS_LOGIC_IBM(UConverter_ಗಮ್ಯ_Unicode_ವಾದಗಳು *ವಾದಗಳು_2,
                                                            UError_ಸಂಕೇತ* err){
    ಅಕ್ಷರ ಸ್ಥಿರಾಂಕ* ಮೂಲ_ಆರಂಭ;
    UConverter_ದತ್ತಾಂಶ_ISO2022* my_ದತ್ತಾಂಶ = static_cast<UConverter_ದತ್ತಾಂಶ_ISO2022*>(ವಾದಗಳು_2->converter->extraInfo);

    UConverter_ಗಮ್ಯ_Unicode_ವಾದಗಳು sub_ವಾದಗಳು;
    int32_t min_ವಾದಗಳು_ಗಾತ್ರ;

    /* set up the subconverter arguments */
    ಆದರೆ(ವಾದಗಳು_2->size<ಗಾತ್ರ(UConverter_ಗಮ್ಯ_Unicode_ವಾದಗಳು)) {
        min_ವಾದಗಳು_ಗಾತ್ರ = ವಾದಗಳು_2->size;
    } ಇಲ್ಲದಿದ್ದರೆ {
        min_ವಾದಗಳು_ಗಾತ್ರ = static_cast<int32_t>(ಗಾತ್ರ(UConverter_ಗಮ್ಯ_Unicode_ವಾದಗಳು));
    }

    uprv_memcpy(&sub_ವಾದಗಳು, ವಾದಗಳು_2, min_ವಾದಗಳು_ಗಾತ್ರ);
    sub_ವಾದಗಳು.size = static_cast<uint16_t>(min_ವಾದಗಳು_ಗಾತ್ರ);
    sub_ವಾದಗಳು.converter = my_ದತ್ತಾಂಶ->currentConverter;

    /* remember the original start of the input for offsets */
    ಮೂಲ_ಆರಂಭ = ವಾದಗಳು_2->ಮೂಲ;

    ಆದರೆ(my_ದತ್ತಾಂಶ->ಕೀಲಿ != 0) {
        /* continue with a partial escape sequence */
        ಹೋಗು escape;
    }

    ಆಗ(U_SUCCESS(*err) && ವಾದಗಳು_2->ಮೂಲ < ವಾದಗಳು_2->ಮೂಲ_ಎಲ್ಲೆ) {
        /*Find the end of the buffer e.g : Next Escape Seq | end of Buffer*/
        sub_ವಾದಗಳು.ಮೂಲ = ವಾದಗಳು_2->ಮೂಲ;
        sub_ವಾದಗಳು.ಮೂಲ_ಎಲ್ಲೆ = get_ಕೊನೆ_ದ_ಬಫರ್_2022(&(ವಾದಗಳು_2->ಮೂಲ), ವಾದಗಳು_2->ಮೂಲ_ಎಲ್ಲೆ, ವಾದಗಳು_2->flush);
        ಆದರೆ(sub_ವಾದಗಳು.ಮೂಲ != sub_ವಾದಗಳು.ಮೂಲ_ಎಲ್ಲೆ) {
            /*
             * get the current partial byte sequence
             *
             * it needs to be moved between the public and the subconverter
             * so that the conversion framework, which only sees the public
             * converter, can handle truncated and illegal input etc.
             */
            ಆದರೆ(ವಾದಗಳು_2->converter->ಗಮ್ಯ_ULength > 0) {
                uprv_memcpy(sub_ವಾದಗಳು.converter->ಗಮ್ಯ_UBytes, ವಾದಗಳು_2->converter->ಗಮ್ಯ_UBytes, ವಾದಗಳು_2->converter->ಗಮ್ಯ_ULength);
            }
            sub_ವಾದಗಳು.converter->ಗಮ್ಯ_ULength = ವಾದಗಳು_2->converter->ಗಮ್ಯ_ULength;

            /*
             * Convert up to the end of the input, or to before the next escape character.
             * Does not handle conversion extensions because the preToU[] state etc.
             * is not copied.
             */
            ucnv_MBCSToUnicodeWithOffsets(&sub_ವಾದಗಳು, err);

            ಆದರೆ(ವಾದಗಳು_2->offsets != ಶೂನ್ಯಸೂಚಕ && ಮೂಲ_ಆರಂಭ != ವಾದಗಳು_2->ಮೂಲ) {
                /* update offsets to base them on the actual start of the input */
                int32_t *offsets = ವಾದಗಳು_2->offsets;
                char16_t *ಗುರಿ = ವಾದಗಳು_2->ಗುರಿ;
                int32_t delta = static_cast<int32_t>(ವಾದಗಳು_2->ಮೂಲ - ಮೂಲ_ಆರಂಭ);
                ಆಗ(ಗುರಿ < sub_ವಾದಗಳು.ಗುರಿ) {
                    ಆದರೆ(*offsets >= 0) {
                        *offsets += delta;
                    }
                    ++offsets;
                    ++ಗುರಿ;
                }
            }
            ವಾದಗಳು_2->ಮೂಲ = sub_ವಾದಗಳು.ಮೂಲ;
            ವಾದಗಳು_2->ಗುರಿ = sub_ವಾದಗಳು.ಗುರಿ;
            ವಾದಗಳು_2->offsets = sub_ವಾದಗಳು.offsets;

            /* copy input/error/overflow buffers */
            ಆದರೆ(sub_ವಾದಗಳು.converter->ಗಮ್ಯ_ULength > 0) {
                uprv_memcpy(ವಾದಗಳು_2->converter->ಗಮ್ಯ_UBytes, sub_ವಾದಗಳು.converter->ಗಮ್ಯ_UBytes, sub_ವಾದಗಳು.converter->ಗಮ್ಯ_ULength);
            }
            ವಾದಗಳು_2->converter->ಗಮ್ಯ_ULength = sub_ವಾದಗಳು.converter->ಗಮ್ಯ_ULength;

            ಆದರೆ(*err == U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR) {
                ಆದರೆ(sub_ವಾದಗಳು.converter->UChar_Error_ಬಫರ್_ಉದ್ದಳತೆ > 0) {
                    uprv_memcpy(ವಾದಗಳು_2->converter->UChar_Error_ಬಫರ್, sub_ವಾದಗಳು.converter->UChar_Error_ಬಫರ್,
                                sub_ವಾದಗಳು.converter->UChar_Error_ಬಫರ್_ಉದ್ದಳತೆ);
                }
                ವಾದಗಳು_2->converter->UChar_Error_ಬಫರ್_ಉದ್ದಳತೆ=sub_ವಾದಗಳು.converter->UChar_Error_ಬಫರ್_ಉದ್ದಳತೆ;
                sub_ವಾದಗಳು.converter->UChar_Error_ಬಫರ್_ಉದ್ದಳತೆ = 0;
            }
        }

        ಆದರೆ (U_FAILURE(*err) || (ವಾದಗಳು_2->ಮೂಲ == ವಾದಗಳು_2->ಮೂಲ_ಎಲ್ಲೆ)) {
            ಹಿಂತಿರುಗಿ;
        }

escape:
        changeState_2022(ವಾದಗಳು_2->converter,
               &(ವಾದಗಳು_2->ಮೂಲ),
               ವಾದಗಳು_2->ಮೂಲ_ಎಲ್ಲೆ,
               ISO_2022_KR,
               err);
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
UConverter_ಗಮ್ಯ_Unicode_ISO_2022_KR_OFFSETS_LOGIC(UConverter_ಗಮ್ಯ_Unicode_ವಾದಗಳು *ವಾದಗಳು_2,
                                                            UError_ಸಂಕೇತ* err){
    ಅಕ್ಷರ ತಾತ್ಕಾಲಿಕ_Buf[2];
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* my_ಮೂಲ = const_cast<ಅಕ್ಷರ*>(ವಾದಗಳು_2->ಮೂಲ);
    char16_t *my_ಗುರಿ = ವಾದಗಳು_2->ಗುರಿ;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *my_ಮೂಲ_ಎಲ್ಲೆ = ವಾದಗಳು_2->ಮೂಲ_ಎಲ್ಲೆ;
    UChar32 ಗುರಿ_Uni_Char = 0x0000;
    char16_t my_ಮೂಲ_Char = 0x0000;
    UConverter_ದತ್ತಾಂಶ_ISO2022* my_ದತ್ತಾಂಶ;
    UConverter_Shared_ದತ್ತಾಂಶ* shared_ದತ್ತಾಂಶ ;
    UBool useFallback;

    my_ದತ್ತಾಂಶ = static_cast<UConverter_ದತ್ತಾಂಶ_ISO2022*>(ವಾದಗಳು_2->converter->extraInfo);
    ಆದರೆ(my_ದತ್ತಾಂಶ->ಆವೃತ್ತಿ==1){
        UConverter_ಗಮ್ಯ_Unicode_ISO_2022_KR_OFFSETS_LOGIC_IBM(ವಾದಗಳು_2,err);
        ಹಿಂತಿರುಗಿ;
    }

    /* initialize state */
    shared_ದತ್ತಾಂಶ = my_ದತ್ತಾಂಶ->currentConverter->shared_ದತ್ತಾಂಶ;
    useFallback = ವಾದಗಳು_2->converter->useFallback;

    ಆದರೆ(my_ದತ್ತಾಂಶ->ಕೀಲಿ != 0) {
        /* continue with a partial escape sequence */
        ಹೋಗು escape;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ವಾದಗಳು_2->converter->ಗಮ್ಯ_ULength == 1 && my_ಮೂಲ < my_ಮೂಲ_ಎಲ್ಲೆ && my_ಗುರಿ < ವಾದಗಳು_2->ಗುರಿ_ಎಲ್ಲೆ) {
        /* continue with a partial double-byte character */
        my_ಮೂಲ_Char = ವಾದಗಳು_2->converter->ಗಮ್ಯ_UBytes[0];
        ವಾದಗಳು_2->converter->ಗಮ್ಯ_ULength = 0;
        ಹೋಗು getTrailByte;
    }

    ಆಗ(my_ಮೂಲ< my_ಮೂಲ_ಎಲ್ಲೆ){

        ಆದರೆ(my_ಗುರಿ < ವಾದಗಳು_2->ಗುರಿ_ಎಲ್ಲೆ){

            my_ಮೂಲ_Char = static_cast<ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ>(*my_ಮೂಲ++);

            ಆದರೆ(my_ಮೂಲ_Char==UCNV_SI){
                my_ದತ್ತಾಂಶ->ಗಮ್ಯ_U2022_State.g = 0;
                ಆದರೆ (my_ದತ್ತಾಂಶ->is_ಖಾಲಿಗಳು_Segment) {
                    my_ದತ್ತಾಂಶ->is_ಖಾಲಿಗಳು_Segment = ಸುಳ್ಳು;	/* we are handling it, reset to avoid future spurious errors */
                    *err = U_ILLEGAL_ESCAPE_SEQUENCE;
                    ವಾದಗಳು_2->converter->ಗಮ್ಯ_UCallback_Reason = UCNV_IRREGULAR;
                    ವಾದಗಳು_2->converter->ಗಮ್ಯ_UBytes[0] = static_cast<uint8_t>(my_ಮೂಲ_Char);
                    ವಾದಗಳು_2->converter->ಗಮ್ಯ_ULength = 1;
                    ವಾದಗಳು_2->ಗುರಿ = my_ಗುರಿ;
                    ವಾದಗಳು_2->ಮೂಲ = my_ಮೂಲ;
                    ಹಿಂತಿರುಗಿ;
                }
                /*consume the source */
                ಮುಂದುವರಿ;
            }ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(my_ಮೂಲ_Char==UCNV_SO){
                my_ದತ್ತಾಂಶ->ಗಮ್ಯ_U2022_State.g = 1;
                my_ದತ್ತಾಂಶ->is_ಖಾಲಿಗಳು_Segment = ನಿಜ;	/* Begin a new segment, empty so far */
                /*consume the source */
                ಮುಂದುವರಿ;
            }ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(my_ಮೂಲ_Char==ESC_2022){
                my_ಮೂಲ--;
escape:
                my_ದತ್ತಾಂಶ->is_ಖಾಲಿಗಳು_Segment = ಸುಳ್ಳು;	/* Any invalid ESC sequences will be detected separately, so just reset this */
                changeState_2022(ವಾದಗಳು_2->converter,&(my_ಮೂಲ),
                                my_ಮೂಲ_ಎಲ್ಲೆ, ISO_2022_KR, err);
                ಆದರೆ(U_FAILURE(*err)){
                    ವಾದಗಳು_2->ಗುರಿ = my_ಗುರಿ;
                    ವಾದಗಳು_2->ಮೂಲ = my_ಮೂಲ;
                    ಹಿಂತಿರುಗಿ;
                }
                ಮುಂದುವರಿ;
            }

            my_ದತ್ತಾಂಶ->is_ಖಾಲಿಗಳು_Segment = ಸುಳ್ಳು;	/* Any invalid char errors will be detected separately, so just reset this */
            ಆದರೆ(my_ದತ್ತಾಂಶ->ಗಮ್ಯ_U2022_State.g == 1) {
                ಆದರೆ(my_ಮೂಲ < my_ಮೂಲ_ಎಲ್ಲೆ) {
                    ಪೂರ್ಣಾಂಕ leadIsOk, trailIsOk;
                    uint8_t trailByte;
getTrailByte:
                    ಗುರಿ_Uni_Char = missingCharMarker;
                    trailByte = static_cast<uint8_t>(*my_ಮೂಲ);
                    /*
                     * Ticket 5691: consistent illegal sequences:
                     * - We include at least the first byte in the illegal sequence.
                     * - If any of the non-initial bytes could be the start of a character,
                     *   we stop the illegal sequence before the first one of those.
                     *
                     * In ISO-2022 DBCS, if the second byte is in the 21..7e range or is
                     * an ESC/SO/SI, we report only the first byte as the illegal sequence.
                     * Otherwise we convert or report the pair of bytes.
                     */
                    leadIsOk = static_cast<uint8_t>(my_ಮೂಲ_Char - 0x21) <= (0x7e - 0x21);
                    trailIsOk = static_cast<uint8_t>(trailByte - 0x21) <= (0x7e - 0x21);
                    ಆದರೆ (leadIsOk && trailIsOk) {
                        ++my_ಮೂಲ;
                        ತಾತ್ಕಾಲಿಕ_Buf[0] = static_cast<ಅಕ್ಷರ>(my_ಮೂಲ_Char + 0x80);
                        ತಾತ್ಕಾಲಿಕ_Buf[1] = static_cast<ಅಕ್ಷರ>(trailByte + 0x80);
                        ಗುರಿ_Uni_Char = ucnv_MBCSSimple_Get_ಮುಂದಿನ_UChar(shared_ದತ್ತಾಂಶ, ತಾತ್ಕಾಲಿಕ_Buf, 2, useFallback);
                        my_ಮೂಲ_Char = (my_ಮೂಲ_Char << 8) | trailByte;
                    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!(trailIsOk || IS_2022_CONTROL(trailByte))) {
                        /* report a pair of illegal bytes if the second byte is not a DBCS starter */
                        ++my_ಮೂಲ;
                        /* add another bit so that the code below writes 2 bytes in case of error */
                        my_ಮೂಲ_Char = static_cast<char16_t>(0x10000 | (my_ಮೂಲ_Char << 8) | trailByte);
                    }
                } ಇಲ್ಲದಿದ್ದರೆ {
                    ವಾದಗಳು_2->converter->ಗಮ್ಯ_UBytes[0] = static_cast<uint8_t>(my_ಮೂಲ_Char);
                    ವಾದಗಳು_2->converter->ಗಮ್ಯ_ULength = 1;
                    ನಿಲ್ಲಿಸು;
                }
            }
            ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(my_ಮೂಲ_Char <= 0x7f) {
                ಗುರಿ_Uni_Char = ucnv_MBCSSimple_Get_ಮುಂದಿನ_UChar(shared_ದತ್ತಾಂಶ, my_ಮೂಲ - 1, 1, useFallback);
            } ಇಲ್ಲದಿದ್ದರೆ {
                ಗುರಿ_Uni_Char = 0xffff;
            }
            ಆದರೆ(ಗುರಿ_Uni_Char < 0xfffe){
                ಆದರೆ(ವಾದಗಳು_2->offsets) {
                    ವಾದಗಳು_2->offsets[my_ಗುರಿ - ವಾದಗಳು_2->ಗುರಿ] = static_cast<int32_t>(my_ಮೂಲ - ವಾದಗಳು_2->ಮೂಲ - (my_ಮೂಲ_Char <= 0xff ? 1 : 2));
                }
                *(my_ಗುರಿ++) = static_cast<char16_t>(ಗುರಿ_Uni_Char);
            }
            ಇಲ್ಲದಿದ್ದರೆ {
                /* Call the callback function*/
                ಗಮ್ಯ_Unicode_Callback(ವಾದಗಳು_2->converter,my_ಮೂಲ_Char,ಗುರಿ_Uni_Char,err);
                ನಿಲ್ಲಿಸು;
            }
        }
        ಇಲ್ಲದಿದ್ದರೆ{
            *err =U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
            ನಿಲ್ಲಿಸು;
        }
    }
    ವಾದಗಳು_2->ಗುರಿ = my_ಗುರಿ;
    ವಾದಗಳು_2->ಮೂಲ = my_ಮೂಲ;
}

/*************************** END ISO2022-KR *********************************/

/*************************** ISO-2022-CN *********************************
*
* Rules for ISO-2022-CN Encoding:
* i)   The designator sequence must appear once on a line before any instance
*      of character set it designates.
* ii)  If two lines contain characters from the same character set, both lines
*      must include the designator sequence.
* iii) Once the designator sequence is known, a shifting sequence has to be found
*      to invoke the  shifting
* iv)  All lines start in ASCII and end in ASCII.
* v)   Four shifting sequences are employed for this purpose:
*
*      Sequcence   ASCII Eq    Charsets
*      ----------  -------    ---------
*      SI           <SI>        US-ASCII
*      SO           <SO>        CNS-11643-1992 Plane 1, GB2312, ISO-IR-165
*      SS2          <ESC>N      CNS-11643-1992 Plane 2
*      SS3          <ESC>O      CNS-11643-1992 Planes 3-7
*
* vi)
*      SOdesignator  : ESC "$" ")" finalchar_for_SO
*      SS2designator : ESC "$" "*" finalchar_for_SS2
*      SS3designator : ESC "$" "+" finalchar_for_SS3
*
*      ESC $ ) A       Indicates the bytes following SO are Chinese
*       characters as defined in GB 2312-80, until
*       another SOdesignation appears
*
*
*      ESC $ ) E       Indicates the bytes following SO are as defined
*       in ISO-IR-165 (for details, see section 2.1),
*       until another SOdesignation appears
*
*      ESC $ ) G       Indicates the bytes following SO are as defined
*       in CNS 11643-plane-1, until another
*       SOdesignation appears
*
*      ESC $ * H       Indicates the two bytes immediately following
*       SS2 is a Chinese character as defined in CNS
*       11643-plane-2, until another SS2designation
*       appears
*       (Meaning <ESC>N must precede every 2 byte
*        sequence.)
*
*      ESC $ + I       Indicates the immediate two bytes following SS3
*       is a Chinese character as defined in CNS
*       11643-plane-3, until another SS3designation
*       appears
*       (Meaning <ESC>O must precede every 2 byte
*        sequence.)
*
*      ESC $ + J       Indicates the immediate two bytes following SS3
*       is a Chinese character as defined in CNS
*       11643-plane-4, until another SS3designation
*       appears
*       (In English: <ESC>O must precede every 2 byte
*        sequence.)
*
*      ESC $ + K       Indicates the immediate two bytes following SS3
*       is a Chinese character as defined in CNS
*       11643-plane-5, until another SS3designation
*       appears
*
*      ESC $ + L       Indicates the immediate two bytes following SS3
*       is a Chinese character as defined in CNS
*       11643-plane-6, until another SS3designation
*       appears
*
*      ESC $ + M       Indicates the immediate two bytes following SS3
*       is a Chinese character as defined in CNS
*       11643-plane-7, until another SS3designation
*       appears
*
*       As in ISO-2022-CN, each line starts in ASCII, and ends in ASCII, and
*       has its own designation information before any Chinese characters
*       appear
*
*/

/* The following are defined this way to make the strings truly readonly */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ GB_2312_80_STR[] = "\x1B\x24\x29\x41";
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ ISO_IR_165_STR[] = "\x1B\x24\x29\x45";
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ CNS_11643_1992_Plane_1_STR[] = "\x1B\x24\x29\x47";
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ CNS_11643_1992_Plane_2_STR[] = "\x1B\x24\x2A\x48";
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ CNS_11643_1992_Plane_3_STR[] = "\x1B\x24\x2B\x49";
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ CNS_11643_1992_Plane_4_STR[] = "\x1B\x24\x2B\x4A";
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ CNS_11643_1992_Plane_5_STR[] = "\x1B\x24\x2B\x4B";
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ CNS_11643_1992_Plane_6_STR[] = "\x1B\x24\x2B\x4C";
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ CNS_11643_1992_Plane_7_STR[] = "\x1B\x24\x2B\x4D";

/********************** ISO2022-CN Data **************************/
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಸ್ಥಿರಾಂಕ escSeqCharsCN[10] ={
        ಸರಿಸು_ಒಳ_STR,                   /* 0 ASCII */
        GB_2312_80_STR,                 /* 1 GB2312_1 */
        ISO_IR_165_STR,                 /* 2 ISO_IR_165 */
        CNS_11643_1992_Plane_1_STR,
        CNS_11643_1992_Plane_2_STR,
        CNS_11643_1992_Plane_3_STR,
        CNS_11643_1992_Plane_4_STR,
        CNS_11643_1992_Plane_5_STR,
        CNS_11643_1992_Plane_6_STR,
        CNS_11643_1992_Plane_7_STR
};

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
UConverter_fromUnicode_ISO_2022_CN_OFFSETS_LOGIC(UConverter_From_Unicode_ವಾದಗಳು* ವಾದಗಳು_2, UError_ಸಂಕೇತ* err){
    UConverter *cnv = ವಾದಗಳು_2->converter;
    UConverter_ದತ್ತಾಂಶ_ISO2022 *converter_ದತ್ತಾಂಶ;
    ISO2022State *pFromU2022State;
    uint8_t* ಗುರಿ = reinterpret_cast<uint8_t*>(ವಾದಗಳು_2->ಗುರಿ);
    ಸ್ಥಿರಾಂಕ uint8_t* ಗುರಿ_ಎಲ್ಲೆ = reinterpret_cast<ಸ್ಥಿರಾಂಕ uint8_t*>(ವಾದಗಳು_2->ಗುರಿ_ಎಲ್ಲೆ);
    ಸ್ಥಿರಾಂಕ char16_t* ಮೂಲ = ವಾದಗಳು_2->ಮೂಲ;
    ಸ್ಥಿರಾಂಕ char16_t* ಮೂಲ_ಎಲ್ಲೆ = ವಾದಗಳು_2->ಮೂಲ_ಎಲ್ಲೆ;
    int32_t* offsets = ವಾದಗಳು_2->offsets;
    UChar32 ಮೂಲ_Char;
    ಅಕ್ಷರ ಬಫರ್[8];
    int32_t len;
    int8_t choices[3];
    int32_t choice_ಎಣಿಕೆ;
    uint32_t ಗುರಿ_ಮೌಲ್ಯ = 0;
    UBool useFallback;

    /* set up the state */
    converter_ದತ್ತಾಂಶ = static_cast<UConverter_ದತ್ತಾಂಶ_ISO2022*>(cnv->extraInfo);
    pFromU2022State   = &converter_ದತ್ತಾಂಶ->fromU2022State;

    choice_ಎಣಿಕೆ = 0;

    /* check if the last codepoint of previous buffer was a lead surrogate*/
    ಆದರೆ((ಮೂಲ_Char = cnv->fromUChar32)!=0 && ಗುರಿ< ಗುರಿ_ಎಲ್ಲೆ) {
        ಹೋಗು getTrail;
    }

    ಆಗ( ಮೂಲ < ಮೂಲ_ಎಲ್ಲೆ){
        ಆದರೆ(ಗುರಿ < ಗುರಿ_ಎಲ್ಲೆ){

            ಮೂಲ_Char  = *(ಮೂಲ++);
            /*check if the char is a First surrogate*/
             ಆದರೆ(U16_IS_SURROGATE(ಮೂಲ_Char)) {
                ಆದರೆ(U16_IS_SURROGATE_LEAD(ಮೂಲ_Char)) {
getTrail:
                    /*look ahead to find the trail surrogate*/
                    ಆದರೆ(ಮೂಲ < ಮೂಲ_ಎಲ್ಲೆ) {
                        /* test the following code unit */
                        char16_t trail = *ಮೂಲ;
                        ಆದರೆ(U16_IS_TRAIL(trail)) {
                            ಮೂಲ++;
                            ಮೂಲ_Char=U16_GET_SUPPLEMENTARY(ಮೂಲ_Char, trail);
                            cnv->fromUChar32=0x00;
                            /* convert this supplementary code point */
                            /* exit this condition tree */
                        } ಇಲ್ಲದಿದ್ದರೆ {
                            /* this is an unmatched lead code unit (1st surrogate) */
                            /* callback(illegal) */
                            *err=U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
                            cnv->fromUChar32=ಮೂಲ_Char;
                            ನಿಲ್ಲಿಸು;
                        }
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        /* no more input */
                        cnv->fromUChar32=ಮೂಲ_Char;
                        ನಿಲ್ಲಿಸು;
                    }
                } ಇಲ್ಲದಿದ್ದರೆ {
                    /* this is an unmatched trail code unit (2nd surrogate) */
                    /* callback(illegal) */
                    *err=U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
                    cnv->fromUChar32=ಮೂಲ_Char;
                    ನಿಲ್ಲಿಸು;
                }
            }

            /* do the conversion */
            ಆದರೆ(ಮೂಲ_Char <= 0x007f ){
                /* do not convert SO/SI/ESC */
                ಆದರೆ(IS_2022_CONTROL(ಮೂಲ_Char)) {
                    /* callback(illegal) */
                    *err=U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
                    cnv->fromUChar32=ಮೂಲ_Char;
                    ನಿಲ್ಲಿಸು;
                }

                /* US-ASCII */
                ಆದರೆ(pFromU2022State->g == 0) {
                    ಬಫರ್[0] = static_cast<ಅಕ್ಷರ>(ಮೂಲ_Char);
                    len = 1;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    ಬಫರ್[0] = UCNV_SI;
                    ಬಫರ್[1] = static_cast<ಅಕ್ಷರ>(ಮೂಲ_Char);
                    len = 2;
                    pFromU2022State->g = 0;
                    choice_ಎಣಿಕೆ = 0;
                }
                ಆದರೆ(ಮೂಲ_Char == CR || ಮೂಲ_Char == LF) {
                    /* reset the state at the end of a line */
                    uprv_memset(pFromU2022State, 0, ಗಾತ್ರ(ISO2022State));
                    choice_ಎಣಿಕೆ = 0;
                }
            }
            ಇಲ್ಲದಿದ್ದರೆ{
                /* convert U+0080..U+10ffff */
                int32_t i;
                int8_t cs, g;

                ಆದರೆ(choice_ಎಣಿಕೆ == 0) {
                    /* try the current SO/G1 converter first */
                    choices[0] = pFromU2022State->cs[1];

                    /* default to GB2312_1 if none is designated yet */
                    ಆದರೆ(choices[0] == 0) {
                        choices[0] = GB2312_1;
                    }

                    ಆದರೆ(converter_ದತ್ತಾಂಶ->ಆವೃತ್ತಿ == 0) {
                        /* ISO-2022-CN */

                        /* try the other SO/G1 converter; a CNS_11643_1 lookup may result in any plane */
                        ಆದರೆ(choices[0] == GB2312_1) {
                            choices[1] = static_cast<int8_t>(CNS_11643_1);
                        } ಇಲ್ಲದಿದ್ದರೆ {
                            choices[1] = static_cast<int8_t>(GB2312_1);
                        }

                        choice_ಎಣಿಕೆ = 2;
                    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (converter_ದತ್ತಾಂಶ->ಆವೃತ್ತಿ == 1) {
                        /* ISO-2022-CN-EXT */

                        /* try one of the other converters */
                        ಆಯ್ಕೆ(choices[0]) {
                        ಸ್ಥಿತಿ GB2312_1:
                            choices[1] = static_cast<int8_t>(CNS_11643_1);
                            choices[2] = static_cast<int8_t>(ISO_IR_165);
                            ನಿಲ್ಲಿಸು;
                        ಸ್ಥಿತಿ ISO_IR_165:
                            choices[1] = static_cast<int8_t>(GB2312_1);
                            choices[2] = static_cast<int8_t>(CNS_11643_1);
                            ನಿಲ್ಲಿಸು;
                        ಪೂರ್ವನಿಯೋಜಿತ: /* CNS_11643_x */
                            choices[1] = static_cast<int8_t>(GB2312_1);
                            choices[2] = static_cast<int8_t>(ISO_IR_165);
                            ನಿಲ್ಲಿಸು;
                        }

                        choice_ಎಣಿಕೆ = 3;
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        choices[0] = static_cast<int8_t>(CNS_11643_1);
                        choices[1] = static_cast<int8_t>(GB2312_1);
                    }
                }

                cs = g = 0;
                /*
                 * len==0: no mapping found yet
                 * len<0: found a fallback result: continue looking for a roundtrip but no further fallbacks
                 * len>0: found a roundtrip result, done
                 */
                len = 0;
                /*
                 * We will turn off useFallback after finding a fallback,
                 * but we still get fallbacks from PUA code points as usual.
                 * Therefore, we will also need to check that we don't overwrite
                 * an early fallback with a later one.
                 */
                useFallback = cnv->useFallback;

                ಗಾಗಿ(i = 0; i < choice_ಎಣಿಕೆ && len <= 0; ++i) {
                    int8_t cs0 = choices[i];
                    ಆದರೆ(cs0 > 0) {
                        uint32_t value;
                        int32_t len2;
                        ಆದರೆ(cs0 >= CNS_11643_0) {
                            len2 = MBCS_FROM_UCHAR32_ISO2022(
                                        converter_ದತ್ತಾಂಶ->myConverterArray[CNS_11643],
                                        ಮೂಲ_Char,
                                        &value,
                                        useFallback,
                                        MBCS_OUTPUT_3);
                            ಆದರೆ(len2 == 3 || (len2 == -3 && len == 0)) {
                                ಗುರಿ_ಮೌಲ್ಯ = value;
                                cs = static_cast<int8_t>(CNS_11643_0 + (value >> 16) - 0x80);
                                ಆದರೆ(len2 >= 0) {
                                    len = 2;
                                } ಇಲ್ಲದಿದ್ದರೆ {
                                    len = -2;
                                    useFallback = ಸುಳ್ಳು;
                                }
                                ಆದರೆ(cs == CNS_11643_1) {
                                    g = 1;
                                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(cs == CNS_11643_2) {
                                    g = 2;
                                } ಇಲ್ಲದಿದ್ದರೆ /* plane 3..7 */ ಆದರೆ(converter_ದತ್ತಾಂಶ->ಆವೃತ್ತಿ == 1) {
                                    g = 3;
                                } ಇಲ್ಲದಿದ್ದರೆ {
                                    /* ISO-2022-CN (without -EXT) does not support plane 3..7 */
                                    len = 0;
                                }
                            }
                        } ಇಲ್ಲದಿದ್ದರೆ {
                            /* GB2312_1 or ISO-IR-165 */
                            U_ASSERT(cs0<UCNV_2022_MAX_CONVERTERS);
                            len2 = MBCS_FROM_UCHAR32_ISO2022(
                                        converter_ದತ್ತಾಂಶ->myConverterArray[cs0],
                                        ಮೂಲ_Char,
                                        &value,
                                        useFallback,
                                        MBCS_OUTPUT_2);
                            ಆದರೆ(len2 == 2 || (len2 == -2 && len == 0)) {
                                ಗುರಿ_ಮೌಲ್ಯ = value;
                                len = len2;
                                cs = cs0;
                                g = 1;
                                useFallback = ಸುಳ್ಳು;
                            }
                        }
                    }
                }

                ಆದರೆ(len != 0) {
                    len = 0; /* count output bytes; it must have been abs(len) == 2 */

                    /* write the designation sequence if necessary */
                    ಆದರೆ(cs != pFromU2022State->cs[g]) {
                        ಆದರೆ(cs < CNS_11643) {
                            uprv_memcpy(ಬಫರ್, escSeqCharsCN[cs], 4);
                        } ಇಲ್ಲದಿದ್ದರೆ {
                            U_ASSERT(cs >= CNS_11643_1);
                            uprv_memcpy(ಬಫರ್, escSeqCharsCN[CNS_11643 + (cs - CNS_11643_1)], 4);
                        }
                        len = 4;
                        pFromU2022State->cs[g] = cs;
                        ಆದರೆ(g == 1) {
                            /* changing the SO/G1 charset invalidates the choices[] */
                            choice_ಎಣಿಕೆ = 0;
                        }
                    }

                    /* write the shift sequence if necessary */
                    ಆದರೆ(g != pFromU2022State->g) {
                        ಆಯ್ಕೆ(g) {
                        ಸ್ಥಿತಿ 1:
                            ಬಫರ್[len++] = UCNV_SO;

                            /* set the new state only if it is the locking shift SO/G1, not for SS2 or SS3 */
                            pFromU2022State->g = 1;
                            ನಿಲ್ಲಿಸು;
                        ಸ್ಥಿತಿ 2:
                            ಬಫರ್[len++] = 0x1b;
                            ಬಫರ್[len++] = 0x4e;
                            ನಿಲ್ಲಿಸು;
                        ಪೂರ್ವನಿಯೋಜಿತ: /* case 3 */
                            ಬಫರ್[len++] = 0x1b;
                            ಬಫರ್[len++] = 0x4f;
                            ನಿಲ್ಲಿಸು;
                        }
                    }

                    /* write the two output bytes */
                    ಬಫರ್[len++] = static_cast<ಅಕ್ಷರ>(ಗುರಿ_ಮೌಲ್ಯ >> 8);
                    ಬಫರ್[len++] = static_cast<ಅಕ್ಷರ>(ಗುರಿ_ಮೌಲ್ಯ);
                } ಇಲ್ಲದಿದ್ದರೆ {
                    /* if we cannot find the character after checking all codepages
                     * then this is an error
                     */
                    *err = U_INVALID_CHAR_ಸಿಕ್ಕಿತು;
                    cnv->fromUChar32=ಮೂಲ_Char;
                    ನಿಲ್ಲಿಸು;
                }
            }

            /* output len>0 bytes in buffer[] */
            ಆದರೆ(len == 1) {
                *ಗುರಿ++ = ಬಫರ್[0];
                ಆದರೆ(offsets) {
                    *offsets++ = static_cast<int32_t>(ಮೂಲ - ವಾದಗಳು_2->ಮೂಲ - 1); /* -1: known to be ASCII */
                }
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(len == 2 && (ಗುರಿ + 2) <= ಗುರಿ_ಎಲ್ಲೆ) {
                *ಗುರಿ++ = ಬಫರ್[0];
                *ಗುರಿ++ = ಬಫರ್[1];
                ಆದರೆ(offsets) {
                    int32_t ಮೂಲ_ಸೂಚ್ಯಂಕ = static_cast<int32_t>(ಮೂಲ - ವಾದಗಳು_2->ಮೂಲ - U16_ಉದ್ದಳತೆ(ಮೂಲ_Char));
                    *offsets++ = ಮೂಲ_ಸೂಚ್ಯಂಕ;
                    *offsets++ = ಮೂಲ_ಸೂಚ್ಯಂಕ;
                }
            } ಇಲ್ಲದಿದ್ದರೆ {
                fromUWriteUInt8(
                    cnv,
                    ಬಫರ್, len,
                    &ಗುರಿ, reinterpret_cast<ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*>(ಗುರಿ_ಎಲ್ಲೆ),
                    &offsets, static_cast<int32_t>(ಮೂಲ - ವಾದಗಳು_2->ಮೂಲ - U16_ಉದ್ದಳತೆ(ಮೂಲ_Char)),
                    err);
                ಆದರೆ(U_FAILURE(*err)) {
                    ನಿಲ್ಲಿಸು;
                }
            }
        } /* end if(myTargetIndex<myTargetLength) */
        ಇಲ್ಲದಿದ್ದರೆ{
            *err =U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
            ನಿಲ್ಲಿಸು;
        }

    }/* end while(mySourceIndex<mySourceLength) */

    /*
     * the end of the input stream and detection of truncated input
     * are handled by the framework, but for ISO-2022-CN conversion
     * we need to be in ASCII mode at the very end
     *
     * conditions:
     *   successful
     *   not in ASCII mode
     *   end of input and no truncated input
     */
    ಆದರೆ( U_SUCCESS(*err) &&
        pFromU2022State->g!=0 &&
        ವಾದಗಳು_2->flush && ಮೂಲ>=ಮೂಲ_ಎಲ್ಲೆ && cnv->fromUChar32==0
    ) {
        int32_t ಮೂಲ_ಸೂಚ್ಯಂಕ;

        /* we are switching to ASCII */
        pFromU2022State->g=0;

        /* get the source index of the last input character */
        /*
         * TODO this would be simpler and more reliable if we used a pair
         * of sourceIndex/prevSourceIndex like in ucnvmbcs.c
         * so that we could simply use the prevSourceIndex here;
         * this code gives an incorrect result for the rare case of an unmatched
         * trail surrogate that is alone in the last buffer of the text stream
         */
        ಮೂಲ_ಸೂಚ್ಯಂಕ = static_cast<int32_t>(ಮೂಲ - ವಾದಗಳು_2->ಮೂಲ);
        ಆದರೆ(ಮೂಲ_ಸೂಚ್ಯಂಕ>0) {
            --ಮೂಲ_ಸೂಚ್ಯಂಕ;
            ಆದರೆ( U16_IS_TRAIL(ವಾದಗಳು_2->ಮೂಲ[ಮೂಲ_ಸೂಚ್ಯಂಕ]) &&
                (ಮೂಲ_ಸೂಚ್ಯಂಕ==0 || U16_IS_LEAD(ವಾದಗಳು_2->ಮೂಲ[ಮೂಲ_ಸೂಚ್ಯಂಕ-1]))
            ) {
                --ಮೂಲ_ಸೂಚ್ಯಂಕ;
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಮೂಲ_ಸೂಚ್ಯಂಕ=-1;
        }

        fromUWriteUInt8(
            cnv,
            ಸರಿಸು_ಒಳ_STR, 1,
            &ಗುರಿ, reinterpret_cast<ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*>(ಗುರಿ_ಎಲ್ಲೆ),
            &offsets, ಮೂಲ_ಸೂಚ್ಯಂಕ,
            err);
    }

    /*save the state and return */
    ವಾದಗಳು_2->ಮೂಲ = ಮೂಲ;
    ವಾದಗಳು_2->ಗುರಿ = reinterpret_cast<ಅಕ್ಷರ*>(ಗುರಿ);
}


ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
UConverter_ಗಮ್ಯ_Unicode_ISO_2022_CN_OFFSETS_LOGIC(UConverter_ಗಮ್ಯ_Unicode_ವಾದಗಳು *ವಾದಗಳು_2,
                                               UError_ಸಂಕೇತ* err){
    ಅಕ್ಷರ ತಾತ್ಕಾಲಿಕ_Buf[3];
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* my_ಮೂಲ = const_cast<ಅಕ್ಷರ*>(ವಾದಗಳು_2->ಮೂಲ);
    char16_t *my_ಗುರಿ = ವಾದಗಳು_2->ಗುರಿ;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *my_ಮೂಲ_ಎಲ್ಲೆ = ವಾದಗಳು_2->ಮೂಲ_ಎಲ್ಲೆ;
    uint32_t ಗುರಿ_Uni_Char = 0x0000;
    uint32_t my_ಮೂಲ_Char = 0x0000;
    UConverter_ದತ್ತಾಂಶ_ISO2022* my_ದತ್ತಾಂಶ;
    ISO2022State *p_ಗಮ್ಯ_U2022_State;

    my_ದತ್ತಾಂಶ = static_cast<UConverter_ದತ್ತಾಂಶ_ISO2022*>(ವಾದಗಳು_2->converter->extraInfo);
    p_ಗಮ್ಯ_U2022_State = &my_ದತ್ತಾಂಶ->ಗಮ್ಯ_U2022_State;

    ಆದರೆ(my_ದತ್ತಾಂಶ->ಕೀಲಿ != 0) {
        /* continue with a partial escape sequence */
        ಹೋಗು escape;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ವಾದಗಳು_2->converter->ಗಮ್ಯ_ULength == 1 && my_ಮೂಲ < my_ಮೂಲ_ಎಲ್ಲೆ && my_ಗುರಿ < ವಾದಗಳು_2->ಗುರಿ_ಎಲ್ಲೆ) {
        /* continue with a partial double-byte character */
        my_ಮೂಲ_Char = ವಾದಗಳು_2->converter->ಗಮ್ಯ_UBytes[0];
        ವಾದಗಳು_2->converter->ಗಮ್ಯ_ULength = 0;
        ಗುರಿ_Uni_Char = missingCharMarker;
        ಹೋಗು getTrailByte;
    }

    ಆಗ(my_ಮೂಲ < my_ಮೂಲ_ಎಲ್ಲೆ){

        ಗುರಿ_Uni_Char =missingCharMarker;

        ಆದರೆ(my_ಗುರಿ < ವಾದಗಳು_2->ಗುರಿ_ಎಲ್ಲೆ){

            my_ಮೂಲ_Char = static_cast<ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ>(*my_ಮೂಲ++);

            ಆಯ್ಕೆ(my_ಮೂಲ_Char){
            ಸ್ಥಿತಿ UCNV_SI:
                p_ಗಮ್ಯ_U2022_State->g=0;
                ಆದರೆ (my_ದತ್ತಾಂಶ->is_ಖಾಲಿಗಳು_Segment) {
                    my_ದತ್ತಾಂಶ->is_ಖಾಲಿಗಳು_Segment = ಸುಳ್ಳು;	/* we are handling it, reset to avoid future spurious errors */
                    *err = U_ILLEGAL_ESCAPE_SEQUENCE;
                    ವಾದಗಳು_2->converter->ಗಮ್ಯ_UCallback_Reason = UCNV_IRREGULAR;
                    ವಾದಗಳು_2->converter->ಗಮ್ಯ_UBytes[0] = static_cast<uint8_t>(my_ಮೂಲ_Char);
                    ವಾದಗಳು_2->converter->ಗಮ್ಯ_ULength = 1;
                    ವಾದಗಳು_2->ಗುರಿ = my_ಗುರಿ;
                    ವಾದಗಳು_2->ಮೂಲ = my_ಮೂಲ;
                    ಹಿಂತಿರುಗಿ;
                }
                ಮುಂದುವರಿ;

            ಸ್ಥಿತಿ UCNV_SO:
                ಆದರೆ(p_ಗಮ್ಯ_U2022_State->cs[1] != 0) {
                    p_ಗಮ್ಯ_U2022_State->g=1;
                    my_ದತ್ತಾಂಶ->is_ಖಾಲಿಗಳು_Segment = ನಿಜ;	/* Begin a new segment, empty so far */
                    ಮುಂದುವರಿ;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    /* illegal to have SO before a matching designator */
                    my_ದತ್ತಾಂಶ->is_ಖಾಲಿಗಳು_Segment = ಸುಳ್ಳು;	/* Handling a different error, reset this to avoid future spurious errs */
                    ನಿಲ್ಲಿಸು;
                }

            ಸ್ಥಿತಿ ESC_2022:
                my_ಮೂಲ--;
escape:
                {
                    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * my_ಮೂಲ_Before = my_ಮೂಲ;
                    int8_t ಗಮ್ಯ_ULength_Before = ವಾದಗಳು_2->converter->ಗಮ್ಯ_ULength;

                    changeState_2022(ವಾದಗಳು_2->converter,&(my_ಮೂಲ),
                        my_ಮೂಲ_ಎಲ್ಲೆ, ISO_2022_CN,err);

                    /* After SO there must be at least one character before a designator (designator error handled separately) */
                    ಆದರೆ(my_ದತ್ತಾಂಶ->ಕೀಲಿ==0 && U_SUCCESS(*err) && my_ದತ್ತಾಂಶ->is_ಖಾಲಿಗಳು_Segment) {
                        *err = U_ILLEGAL_ESCAPE_SEQUENCE;
                        ವಾದಗಳು_2->converter->ಗಮ್ಯ_UCallback_Reason = UCNV_IRREGULAR;
                        ವಾದಗಳು_2->converter->ಗಮ್ಯ_ULength = static_cast<int8_t>(ಗಮ್ಯ_ULength_Before + (my_ಮೂಲ - my_ಮೂಲ_Before));
                    }
                }

                /* invalid or illegal escape sequence */
                ಆದರೆ(U_FAILURE(*err)){
                    ವಾದಗಳು_2->ಗುರಿ = my_ಗುರಿ;
                    ವಾದಗಳು_2->ಮೂಲ = my_ಮೂಲ;
                    my_ದತ್ತಾಂಶ->is_ಖಾಲಿಗಳು_Segment = ಸುಳ್ಳು;	/* Reset to avoid future spurious errors */
                    ಹಿಂತಿರುಗಿ;
                }
                ಮುಂದುವರಿ;

            /* ISO-2022-CN does not use single-byte (C1) SS2 and SS3 */

            ಸ್ಥಿತಿ CR:
            ಸ್ಥಿತಿ LF:
                uprv_memset(p_ಗಮ್ಯ_U2022_State, 0, ಗಾತ್ರ(ISO2022State));
                U_FALLTHROUGH;
            ಪೂರ್ವನಿಯೋಜಿತ:
                /* convert one or two bytes */
                my_ದತ್ತಾಂಶ->is_ಖಾಲಿಗಳು_Segment = ಸುಳ್ಳು;
                ಆದರೆ(p_ಗಮ್ಯ_U2022_State->g != 0) {
                    ಆದರೆ(my_ಮೂಲ < my_ಮೂಲ_ಎಲ್ಲೆ) {
                        UConverter_Shared_ದತ್ತಾಂಶ *cnv;
                        StateEnum ತಾತ್ಕಾಲಿಕ_State;
                        int32_t ತಾತ್ಕಾಲಿಕ_Buf_Len;
                        ಪೂರ್ಣಾಂಕ leadIsOk, trailIsOk;
                        uint8_t trailByte;
getTrailByte:
                        trailByte = static_cast<uint8_t>(*my_ಮೂಲ);
                        /*
                         * Ticket 5691: consistent illegal sequences:
                         * - We include at least the first byte in the illegal sequence.
                         * - If any of the non-initial bytes could be the start of a character,
                         *   we stop the illegal sequence before the first one of those.
                         *
                         * In ISO-2022 DBCS, if the second byte is in the 21..7e range or is
                         * an ESC/SO/SI, we report only the first byte as the illegal sequence.
                         * Otherwise we convert or report the pair of bytes.
                         */
                        leadIsOk = static_cast<uint8_t>(my_ಮೂಲ_Char - 0x21) <= (0x7e - 0x21);
                        trailIsOk = static_cast<uint8_t>(trailByte - 0x21) <= (0x7e - 0x21);
                        ಆದರೆ (leadIsOk && trailIsOk) {
                            ++my_ಮೂಲ;
                            ತಾತ್ಕಾಲಿಕ_State = static_cast<StateEnum>(p_ಗಮ್ಯ_U2022_State->cs[p_ಗಮ್ಯ_U2022_State->g]);
                            ಆದರೆ(ತಾತ್ಕಾಲಿಕ_State >= CNS_11643_0) {
                                cnv = my_ದತ್ತಾಂಶ->myConverterArray[CNS_11643];
                                ತಾತ್ಕಾಲಿಕ_Buf[0] = static_cast<ಅಕ್ಷರ>(0x80 + (ತಾತ್ಕಾಲಿಕ_State - CNS_11643_0));
                                ತಾತ್ಕಾಲಿಕ_Buf[1] = static_cast<ಅಕ್ಷರ>(my_ಮೂಲ_Char);
                                ತಾತ್ಕಾಲಿಕ_Buf[2] = static_cast<ಅಕ್ಷರ>(trailByte);
                                ತಾತ್ಕಾಲಿಕ_Buf_Len = 3;

                            }ಇಲ್ಲದಿದ್ದರೆ{
                                U_ASSERT(ತಾತ್ಕಾಲಿಕ_State<UCNV_2022_MAX_CONVERTERS);
                                cnv = my_ದತ್ತಾಂಶ->myConverterArray[ತಾತ್ಕಾಲಿಕ_State];
                                ತಾತ್ಕಾಲಿಕ_Buf[0] = static_cast<ಅಕ್ಷರ>(my_ಮೂಲ_Char);
                                ತಾತ್ಕಾಲಿಕ_Buf[1] = static_cast<ಅಕ್ಷರ>(trailByte);
                                ತಾತ್ಕಾಲಿಕ_Buf_Len = 2;
                            }
                            ಗುರಿ_Uni_Char = ucnv_MBCSSimple_Get_ಮುಂದಿನ_UChar(cnv, ತಾತ್ಕಾಲಿಕ_Buf, ತಾತ್ಕಾಲಿಕ_Buf_Len, ಸುಳ್ಳು);
                            my_ಮೂಲ_Char = (my_ಮೂಲ_Char << 8) | trailByte;
                        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!(trailIsOk || IS_2022_CONTROL(trailByte))) {
                            /* report a pair of illegal bytes if the second byte is not a DBCS starter */
                            ++my_ಮೂಲ;
                            /* add another bit so that the code below writes 2 bytes in case of error */
                            my_ಮೂಲ_Char = 0x10000 | (my_ಮೂಲ_Char << 8) | trailByte;
                        }
                        ಆದರೆ(p_ಗಮ್ಯ_U2022_State->g>=2) {
                            /* return from a single-shift state to the previous one */
                            p_ಗಮ್ಯ_U2022_State->g=p_ಗಮ್ಯ_U2022_State->prevG;
                        }
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        ವಾದಗಳು_2->converter->ಗಮ್ಯ_UBytes[0] = static_cast<uint8_t>(my_ಮೂಲ_Char);
                        ವಾದಗಳು_2->converter->ಗಮ್ಯ_ULength = 1;
                        ಹೋಗು endloop;
                    }
                }
                ಇಲ್ಲದಿದ್ದರೆ{
                    ಆದರೆ(my_ಮೂಲ_Char <= 0x7f) {
                        ಗುರಿ_Uni_Char = static_cast<char16_t>(my_ಮೂಲ_Char);
                    }
                }
                ನಿಲ್ಲಿಸು;
            }
            ಆದರೆ(ಗುರಿ_Uni_Char < (missingCharMarker-1/*0xfffe*/)){
                ಆದರೆ(ವಾದಗಳು_2->offsets){
                    ವಾದಗಳು_2->offsets[my_ಗುರಿ - ವಾದಗಳು_2->ಗುರಿ] = static_cast<int32_t>(my_ಮೂಲ - ವಾದಗಳು_2->ಮೂಲ - (my_ಮೂಲ_Char <= 0xff ? 1 : 2));
                }
                *(my_ಗುರಿ++) = static_cast<char16_t>(ಗುರಿ_Uni_Char);
            }
            ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಗುರಿ_Uni_Char > missingCharMarker){
                /* disassemble the surrogate pair and write to output*/
                ಗುರಿ_Uni_Char-=0x0010000;
                *my_ಗುರಿ = static_cast<char16_t>(0xd800 + static_cast<char16_t>(ಗುರಿ_Uni_Char >> 10));
                ಆದರೆ(ವಾದಗಳು_2->offsets){
                    ವಾದಗಳು_2->offsets[my_ಗುರಿ - ವಾದಗಳು_2->ಗುರಿ] = static_cast<int32_t>(my_ಮೂಲ - ವಾದಗಳು_2->ಮೂಲ - (my_ಮೂಲ_Char <= 0xff ? 1 : 2));
                }
                ++my_ಗುರಿ;
                ಆದರೆ(my_ಗುರಿ< ವಾದಗಳು_2->ಗುರಿ_ಎಲ್ಲೆ){
                    *my_ಗುರಿ = static_cast<char16_t>(0xdc00 + static_cast<char16_t>(ಗುರಿ_Uni_Char & 0x3ff));
                    ಆದರೆ(ವಾದಗಳು_2->offsets){
                        ವಾದಗಳು_2->offsets[my_ಗುರಿ - ವಾದಗಳು_2->ಗುರಿ] = static_cast<int32_t>(my_ಮೂಲ - ವಾದಗಳು_2->ಮೂಲ - (my_ಮೂಲ_Char <= 0xff ? 1 : 2));
                    }
                    ++my_ಗುರಿ;
                }ಇಲ್ಲದಿದ್ದರೆ{
                    ವಾದಗಳು_2->converter->UChar_Error_ಬಫರ್[ವಾದಗಳು_2->converter->UChar_Error_ಬಫರ್_ಉದ್ದಳತೆ++]=
                                    static_cast<char16_t>(0xdc00 + static_cast<char16_t>(ಗುರಿ_Uni_Char & 0x3ff));
                }

            }
            ಇಲ್ಲದಿದ್ದರೆ{
                /* Call the callback function*/
                ಗಮ್ಯ_Unicode_Callback(ವಾದಗಳು_2->converter,my_ಮೂಲ_Char,ಗುರಿ_Uni_Char,err);
                ನಿಲ್ಲಿಸು;
            }
        }
        ಇಲ್ಲದಿದ್ದರೆ{
            *err =U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
            ನಿಲ್ಲಿಸು;
        }
    }
endloop:
    ವಾದಗಳು_2->ಗುರಿ = my_ಗುರಿ;
    ವಾದಗಳು_2->ಮೂಲ = my_ಮೂಲ;
}
#ಆದರೆ_ಅಂತ್ಯ /* #if !UCONFIG_ONLY_HTML_CONVERSION */

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
_ISO_2022_WriteSub(UConverter_From_Unicode_ವಾದಗಳು *ವಾದಗಳು_2, int32_t ಸ್ಥಾನಾಂತರ_ಸೂಚ್ಯಂಕ, UError_ಸಂಕೇತ *err) {
    UConverter *cnv = ವಾದಗಳು_2->converter;
    UConverter_ದತ್ತಾಂಶ_ISO2022* my_Converter_ದತ್ತಾಂಶ = static_cast<UConverter_ದತ್ತಾಂಶ_ISO2022*>(cnv->extraInfo);
    ISO2022State *pFromU2022State=&my_Converter_ದತ್ತಾಂಶ->fromU2022State;
    ಅಕ್ಷರ *p, *subchar;
    ಅಕ್ಷರ ಬಫರ್[8];
    int32_t length;

    subchar = reinterpret_cast<ಅಕ್ಷರ*>(cnv->subChars);
    length=cnv->subCharLen; /* assume length==1 for most variants */

    p = ಬಫರ್;
    ಆಯ್ಕೆ(my_Converter_ದತ್ತಾಂಶ->locale[0]){
    ಸ್ಥಿತಿ 'j':
        {
            int8_t cs;

            ಆದರೆ(pFromU2022State->g == 1) {
                /* JIS7: switch from G1 to G0 */
                pFromU2022State->g = 0;
                *p++ = UCNV_SI;
            }

            cs = pFromU2022State->cs[0];
            ಆದರೆ(cs != ASCII && cs != JISX201) {
                /* not in ASCII or JIS X 0201: switch to ASCII */
                pFromU2022State->cs[0] = static_cast<int8_t>(ASCII);
                *p++ = '\x1b';
                *p++ = '\x28';
                *p++ = '\x42';
            }

            *p++ = subchar[0];
            ನಿಲ್ಲಿಸು;
        }
    ಸ್ಥಿತಿ 'c':
        ಆದರೆ(pFromU2022State->g != 0) {
            /* not in ASCII mode: switch to ASCII */
            pFromU2022State->g = 0;
            *p++ = UCNV_SI;
        }
        *p++ = subchar[0];
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ 'k':
        ಆದರೆ(my_Converter_ದತ್ತಾಂಶ->ಆವೃತ್ತಿ == 0) {
            ಆದರೆ(length == 1) {
                ಆದರೆ(ವಾದಗಳು_2->converter->from_Unicode_ಸ್ಥಿತಿ) {
                    /* in DBCS mode: switch to SBCS */
                    ವಾದಗಳು_2->converter->from_Unicode_ಸ್ಥಿತಿ = 0;
                    *p++ = UCNV_SI;
                }
                *p++ = subchar[0];
            } ಇಲ್ಲದಿದ್ದರೆ /* length == 2*/ {
                ಆದರೆ(!ವಾದಗಳು_2->converter->from_Unicode_ಸ್ಥಿತಿ) {
                    /* in SBCS mode: switch to DBCS */
                    ವಾದಗಳು_2->converter->from_Unicode_ಸ್ಥಿತಿ = 1;
                    *p++ = UCNV_SO;
                }
                *p++ = subchar[0];
                *p++ = subchar[1];
            }
            ನಿಲ್ಲಿಸು;
        } ಇಲ್ಲದಿದ್ದರೆ {
            /* save the subconverter's substitution string */
            uint8_t *currentSubChars = my_Converter_ದತ್ತಾಂಶ->currentConverter->subChars;
            int8_t currentSubCharLen = my_Converter_ದತ್ತಾಂಶ->currentConverter->subCharLen;

            /* set our substitution string into the subconverter */
            my_Converter_ದತ್ತಾಂಶ->currentConverter->subChars = reinterpret_cast<uint8_t*>(subchar);
            my_Converter_ದತ್ತಾಂಶ->currentConverter->subCharLen = static_cast<int8_t>(length);

            /* let the subconverter write the subchar, set/retrieve fromUChar32 state */
            ವಾದಗಳು_2->converter = my_Converter_ದತ್ತಾಂಶ->currentConverter;
            my_Converter_ದತ್ತಾಂಶ->currentConverter->fromUChar32 = cnv->fromUChar32;
            ucnv_cbFromUWriteSub(ವಾದಗಳು_2, 0, err);
            cnv->fromUChar32 = my_Converter_ದತ್ತಾಂಶ->currentConverter->fromUChar32;
            ವಾದಗಳು_2->converter = cnv;

            /* restore the subconverter's substitution string */
            my_Converter_ದತ್ತಾಂಶ->currentConverter->subChars = currentSubChars;
            my_Converter_ದತ್ತಾಂಶ->currentConverter->subCharLen = currentSubCharLen;

            ಆದರೆ(*err == U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR) {
                ಆದರೆ(my_Converter_ದತ್ತಾಂಶ->currentConverter->char_Error_ಬಫರ್_ಉದ್ದಳತೆ > 0) {
                    uprv_memcpy(
                        cnv->char_Error_ಬಫರ್,
                        my_Converter_ದತ್ತಾಂಶ->currentConverter->char_Error_ಬಫರ್,
                        my_Converter_ದತ್ತಾಂಶ->currentConverter->char_Error_ಬಫರ್_ಉದ್ದಳತೆ);
                }
                cnv->char_Error_ಬಫರ್_ಉದ್ದಳತೆ = my_Converter_ದತ್ತಾಂಶ->currentConverter->char_Error_ಬಫರ್_ಉದ್ದಳತೆ;
                my_Converter_ದತ್ತಾಂಶ->currentConverter->char_Error_ಬಫರ್_ಉದ್ದಳತೆ = 0;
            }
            ಹಿಂತಿರುಗಿ;
        }
    ಪೂರ್ವನಿಯೋಜಿತ:
        /* not expected */
        ನಿಲ್ಲಿಸು;
    }
    ucnv_cb_From_UWrite_ಬೈಟುಗಳು(ವಾದಗಳು_2,
                           ಬಫರ್, static_cast<int32_t>(p - ಬಫರ್),
                           ಸ್ಥಾನಾಂತರ_ಸೂಚ್ಯಂಕ, err);
}

/*
 * Structure for cloning an ISO 2022 converter into a single memory block.
 */
ರಚನೆ cloneStruct
{
    UConverter cnv;
    UConverter currentConverter;
    UConverter_ದತ್ತಾಂಶ_ISO2022 mydata;
};


U_CDECL_BEGIN

ಸ್ಥಿರ UConverter * U_CALLCONV
ISO_2022_ಸುರಕ್ಷಿತ_Clone(
            ಸ್ಥಿರಾಂಕ UConverter *cnv,
            ಶೂನ್ಯ *ರಾಶಿ_ಬಫರ್,
            int32_t *p_ಬಫರ್_ಗಾತ್ರ,
            UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2)
{
    ರಚನೆ cloneStruct * localClone;
    UConverter_ದತ್ತಾಂಶ_ISO2022 *cnv_ದತ್ತಾಂಶ;
    int32_t i, size;

    ಆದರೆ (U_FAILURE(*ಸ್ಥಿತಿ_2)){
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    ಆದರೆ (*p_ಬಫರ್_ಗಾತ್ರ == 0) { /* 'preflighting' request - set needed size into *pBufferSize */
        *p_ಬಫರ್_ಗಾತ್ರ = (int32_t)ಗಾತ್ರ(ರಚನೆ cloneStruct);
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    cnv_ದತ್ತಾಂಶ = (UConverter_ದತ್ತಾಂಶ_ISO2022 *)cnv->extraInfo;
    localClone = (ರಚನೆ cloneStruct *)ರಾಶಿ_ಬಫರ್;

    /* ucnv.c/ucnv_safeClone() copied the main UConverter already */

    uprv_memcpy(&localClone->mydata, cnv_ದತ್ತಾಂಶ, ಗಾತ್ರ(UConverter_ದತ್ತಾಂಶ_ISO2022));
    localClone->cnv.extraInfo = &localClone->mydata; /* set pointer to extra data */
    localClone->cnv.isExtraLocal = ನಿಜ;

    /* share the subconverters */

    ಆದರೆ(cnv_ದತ್ತಾಂಶ->currentConverter != ಶೂನ್ಯಸೂಚಕ) {
        size = (int32_t)ಗಾತ್ರ(UConverter);
        localClone->mydata.currentConverter =
            ucnv_ಸುರಕ್ಷಿತ_Clone(cnv_ದತ್ತಾಂಶ->currentConverter,
                            &localClone->currentConverter,
                            &size, ಸ್ಥಿತಿ_2);
        ಆದರೆ(U_FAILURE(*ಸ್ಥಿತಿ_2)) {
            ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
        }
    }

    ಗಾಗಿ(i=0; i<UCNV_2022_MAX_CONVERTERS; ++i) {
        ಆದರೆ(cnv_ದತ್ತಾಂಶ->myConverterArray[i] != ಶೂನ್ಯಸೂಚಕ) {
            ucnv_increment_Ref_ಎಣಿಕೆ(cnv_ದತ್ತಾಂಶ->myConverterArray[i]);
        }
    }

    ಹಿಂತಿರುಗಿ &localClone->cnv;
}

U_CDECL_ಕೊನೆ

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
ISO_2022_Get_Unicode_ಗಣ(ಸ್ಥಿರಾಂಕ UConverter *cnv,
                    ಸ್ಥಿರಾಂಕ USetAdder *sa,
                    UConverter_Unicode_ಗಣ which,
                    UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ)
{
    int32_t i;
    UConverter_ದತ್ತಾಂಶ_ISO2022* cnv_ದತ್ತಾಂಶ;

    ಆದರೆ (U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ;
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ U_ENABLE_GENERIC_ISO_2022
    ಆದರೆ (cnv->shared_ದತ್ತಾಂಶ == &ISO2022_ದತ್ತಾಂಶ) {
        /* We use UTF-8 in this case */
        sa->ಕೂಡಿಸು_Range(sa->ಗಣ, 0, 0xd7FF);
        sa->ಕೂಡಿಸು_Range(sa->ಗಣ, 0xE000, 0x10FFFF);
        ಹಿಂತಿರುಗಿ;
    }
#ಆದರೆ_ಅಂತ್ಯ

    cnv_ದತ್ತಾಂಶ = static_cast<UConverter_ದತ್ತಾಂಶ_ISO2022*>(cnv->extraInfo);

    /* open a set and initialize it with code points that are algorithmically round-tripped */
    ಆಯ್ಕೆ(cnv_ದತ್ತಾಂಶ->locale[0]){
    ಸ್ಥಿತಿ 'j':
        /* include JIS X 0201 which is hardcoded */
        sa->ಕೂಡಿಸು(sa->ಗಣ, 0xa5);
        sa->ಕೂಡಿಸು(sa->ಗಣ, 0x203e);
        ಆದರೆ(jpCharsetMasks[cnv_ದತ್ತಾಂಶ->ಆವೃತ್ತಿ]&CSM(ISO8859_1)) {
            /* include Latin-1 for some variants of JP */
            sa->ಕೂಡಿಸು_Range(sa->ಗಣ, 0, 0xff);
        } ಇಲ್ಲದಿದ್ದರೆ {
            /* include ASCII for JP */
            sa->ಕೂಡಿಸು_Range(sa->ಗಣ, 0, 0x7f);
        }
        ಆದರೆ(cnv_ದತ್ತಾಂಶ->ಆವೃತ್ತಿ==3 || cnv_ದತ್ತಾಂಶ->ಆವೃತ್ತಿ==4 || which==UCNV_ROUNDTRIP_AND_FALLBACK_ಗಣ) {
            /*
             * Do not test (jpCharsetMasks[cnvData->version]&CSM(HWKANA_7BIT))!=0
             * because the bit is on for all JP versions although only versions 3 & 4 (JIS7 & JIS8)
             * use half-width Katakana.
             * This is because all ISO-2022-JP variants are lenient in that they accept (in toUnicode)
             * half-width Katakana via the ESC ( I sequence.
             * However, we only emit (fromUnicode) half-width Katakana according to the
             * definition of each variant.
             *
             * When including fallbacks,
             * we need to include half-width Katakana Unicode code points for all JP variants because
             * JIS X 0208 has hardcoded fallbacks for them (which map to full-width Katakana).
             */
            /* include half-width Katakana for JP */
            sa->ಕೂಡಿಸು_Range(sa->ಗಣ, HWKANA_ಆರಂಭ, HWKANA_ಕೊನೆ);
        }
        ನಿಲ್ಲಿಸು;
#ಆದರೆ !UCONFIG_ONLY_HTML_CONVERSION
    ಸ್ಥಿತಿ 'c':
    ಸ್ಥಿತಿ 'z':
        /* include ASCII for CN */
        sa->ಕೂಡಿಸು_Range(sa->ಗಣ, 0, 0x7f);
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ 'k':
        /* there is only one converter for KR, and it is not in the myConverterArray[] */
        cnv_ದತ್ತಾಂಶ->currentConverter->shared_ದತ್ತಾಂಶ->impl->get_Unicode_ಗಣ(
                cnv_ದತ್ತಾಂಶ->currentConverter, sa, which, p_Error_ಸಂಕೇತ);
        /* the loop over myConverterArray[] will simply not find another converter */
        ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ವನಿಯೋಜಿತ:
        ನಿಲ್ಲಿಸು;
    }

#ಆದರೆ 0  /* Replaced by ucnv_MBCSGetFilteredUnicodeSetForUnicode() until we implement ucnv_getUnicodeSet() with reverse fallbacks. */
            ಆದರೆ( (cnv_ದತ್ತಾಂಶ->locale[0]=='c' || cnv_ದತ್ತಾಂಶ->locale[0]=='z') &&
                cnv_ದತ್ತಾಂಶ->ಆವೃತ್ತಿ==0 && i==CNS_11643
            ) {
                /* special handling for non-EXT ISO-2022-CN: add only code points for CNS planes 1 and 2 */
                ucnv_MBCSGet_Unicode_ಗಣ_For_ಬೈಟುಗಳು(
                        cnv_ದತ್ತಾಂಶ->myConverterArray[i],
                        sa, UCNV_ROUNDTRIP_ಗಣ,
                        0, 0x81, 0x82,
                        p_Error_ಸಂಕೇತ);
            }
#ಆದರೆ_ಅಂತ್ಯ

    ಗಾಗಿ (i=0; i<UCNV_2022_MAX_CONVERTERS; i++) {
        UConverter_ಗಣ_Filter filter;
        ಆದರೆ(cnv_ದತ್ತಾಂಶ->myConverterArray[i]!=ಶೂನ್ಯಸೂಚಕ) {
            ಆದರೆ(cnv_ದತ್ತಾಂಶ->locale[0]=='j' && i==JISX208) {
                /*
                 * Only add code points that map to Shift-JIS codes
                 * corresponding to JIS X 0208.
                 */
                filter=UCNV_ಗಣ_FILTER_SJIS;
#ಆದರೆ !UCONFIG_ONLY_HTML_CONVERSION
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ( (cnv_ದತ್ತಾಂಶ->locale[0]=='c' || cnv_ದತ್ತಾಂಶ->locale[0]=='z') &&
                       cnv_ದತ್ತಾಂಶ->ಆವೃತ್ತಿ==0 && i==CNS_11643) {
                /*
                 * Version-specific for CN:
                 * CN version 0 does not map CNS planes 3..7 although
                 * they are all available in the CNS conversion table;
                 * CN version 1 (-EXT) does map them all.
                 * The two versions create different Unicode sets.
                 */
                filter=UCNV_ಗಣ_FILTER_2022_CN;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(i==KSC5601) {
                /*
                 * Some of the KSC 5601 tables (convrtrs.txt has this aliases on multiple tables)
                 * are broader than GR94.
                 */
                filter=UCNV_ಗಣ_FILTER_GR94_DBCS;
#ಆದರೆ_ಅಂತ್ಯ
            } ಇಲ್ಲದಿದ್ದರೆ {
                filter=UCNV_ಗಣ_FILTER_NONE;
            }
            ucnv_MBCSGet_Filtered_Unicode_ಗಣ_For_Unicode(cnv_ದತ್ತಾಂಶ->myConverterArray[i], sa, which, filter, p_Error_ಸಂಕೇತ);
        }
    }

    /*
     * ISO 2022 converters must not convert SO/SI/ESC despite what
     * sub-converters do by themselves.
     * Remove these characters from the set.
     */
    sa->remove(sa->ಗಣ, 0x0e);
    sa->remove(sa->ಗಣ, 0x0f);
    sa->remove(sa->ಗಣ, 0x1b);

    /* ISO 2022 converters do not convert C1 controls either */
    sa->removeRange(sa->ಗಣ, 0x80, 0x9f);
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ UConverterImpl _ISO2022Impl={
    UCNV_ISO_2022,

    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ,

    _ISO2022Open,
    _ISO2022Close,
    _ISO2022Reset,

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ U_ENABLE_GENERIC_ISO_2022
    T_UConverter_ಗಮ್ಯ_Unicode_ISO_2022_OFFSETS_LOGIC,
    T_UConverter_ಗಮ್ಯ_Unicode_ISO_2022_OFFSETS_LOGIC,
    ucnv_fromUnicode_UTF8,
    ucnv_fromUnicode_UTF8_OFFSETS_LOGIC,
#ಇಲ್ಲದಿದ್ದರೆ
    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ,
#ಆದರೆ_ಅಂತ್ಯ
    ಶೂನ್ಯಸೂಚಕ,

    ಶೂನ್ಯಸೂಚಕ,
    _ISO2022getName,
    _ISO_2022_WriteSub,
    ISO_2022_ಸುರಕ್ಷಿತ_Clone,
    ISO_2022_Get_Unicode_ಗಣ,

    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ
};
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ UConverter_Static_ದತ್ತಾಂಶ ISO2022_Static_ದತ್ತಾಂಶ={
    ಗಾತ್ರ(UConverter_Static_ದತ್ತಾಂಶ),
    "ISO_2022",
    2022,
    UCNV_IBM,
    UCNV_ISO_2022,
    1,
    3, /* max 3 bytes per char16_t from UTF-8 (4 bytes from surrogate _pair_) */
    { 0x1a, 0, 0, 0 },
    1,
    ಸುಳ್ಳು,
    ಸುಳ್ಳು,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};
ಸ್ಥಿರಾಂಕ UConverter_Shared_ದತ್ತಾಂಶ ISO2022_ದತ್ತಾಂಶ=
        UCNV_IMMUTABLE_SHARED_ದತ್ತಾಂಶ_INITIALIZER(&ISO2022_Static_ದತ್ತಾಂಶ, &_ISO2022Impl);

/*************JP****************/
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ UConverterImpl _ISO2022JPImpl={
    UCNV_ISO_2022,

    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ,

    _ISO2022Open,
    _ISO2022Close,
    _ISO2022Reset,

    UConverter_ಗಮ್ಯ_Unicode_ISO_2022_JP_OFFSETS_LOGIC,
    UConverter_ಗಮ್ಯ_Unicode_ISO_2022_JP_OFFSETS_LOGIC,
    UConverter_fromUnicode_ISO_2022_JP_OFFSETS_LOGIC,
    UConverter_fromUnicode_ISO_2022_JP_OFFSETS_LOGIC,
    ಶೂನ್ಯಸೂಚಕ,

    ಶೂನ್ಯಸೂಚಕ,
    _ISO2022getName,
    _ISO_2022_WriteSub,
    ISO_2022_ಸುರಕ್ಷಿತ_Clone,
    ISO_2022_Get_Unicode_ಗಣ,

    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ
};
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ UConverter_Static_ದತ್ತಾಂಶ ISO2022_JPStatic_ದತ್ತಾಂಶ={
    ಗಾತ್ರ(UConverter_Static_ದತ್ತಾಂಶ),
    "ISO_2022_JP",
    0,
    UCNV_IBM,
    UCNV_ISO_2022,
    1,
    6, /* max 6 bytes per char16_t: 4-byte escape sequence + DBCS */
    { 0x1a, 0, 0, 0 },
    1,
    ಸುಳ್ಳು,
    ಸುಳ್ಳು,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

ನಾಮಾಕಾಶ {

ಸ್ಥಿರಾಂಕ UConverter_Shared_ದತ್ತಾಂಶ _ISO2022JPData=
        UCNV_IMMUTABLE_SHARED_ದತ್ತಾಂಶ_INITIALIZER(&ISO2022_JPStatic_ದತ್ತಾಂಶ, &_ISO2022JPImpl);

}  // namespace

#ಆದರೆ !UCONFIG_ONLY_HTML_CONVERSION
/************* KR ***************/
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ UConverterImpl _ISO2022KRImpl={
    UCNV_ISO_2022,

    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ,

    _ISO2022Open,
    _ISO2022Close,
    _ISO2022Reset,

    UConverter_ಗಮ್ಯ_Unicode_ISO_2022_KR_OFFSETS_LOGIC,
    UConverter_ಗಮ್ಯ_Unicode_ISO_2022_KR_OFFSETS_LOGIC,
    UConverter_fromUnicode_ISO_2022_KR_OFFSETS_LOGIC,
    UConverter_fromUnicode_ISO_2022_KR_OFFSETS_LOGIC,
    ಶೂನ್ಯಸೂಚಕ,

    ಶೂನ್ಯಸೂಚಕ,
    _ISO2022getName,
    _ISO_2022_WriteSub,
    ISO_2022_ಸುರಕ್ಷಿತ_Clone,
    ISO_2022_Get_Unicode_ಗಣ,

    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ
};
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ UConverter_Static_ದತ್ತಾಂಶ ISO2022_KRStatic_ದತ್ತಾಂಶ={
    ಗಾತ್ರ(UConverter_Static_ದತ್ತಾಂಶ),
    "ISO_2022_KR",
    0,
    UCNV_IBM,
    UCNV_ISO_2022,
    1,
    8, /* max 8 bytes per char16_t */
    { 0x1a, 0, 0, 0 },
    1,
    ಸುಳ್ಳು,
    ಸುಳ್ಳು,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

ನಾಮಾಕಾಶ {

ಸ್ಥಿರಾಂಕ UConverter_Shared_ದತ್ತಾಂಶ _ISO2022KRData=
        UCNV_IMMUTABLE_SHARED_ದತ್ತಾಂಶ_INITIALIZER(&ISO2022_KRStatic_ದತ್ತಾಂಶ, &_ISO2022KRImpl);

}  // namespace

/*************** CN ***************/
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ UConverterImpl _ISO2022CNImpl={

    UCNV_ISO_2022,

    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ,

    _ISO2022Open,
    _ISO2022Close,
    _ISO2022Reset,

    UConverter_ಗಮ್ಯ_Unicode_ISO_2022_CN_OFFSETS_LOGIC,
    UConverter_ಗಮ್ಯ_Unicode_ISO_2022_CN_OFFSETS_LOGIC,
    UConverter_fromUnicode_ISO_2022_CN_OFFSETS_LOGIC,
    UConverter_fromUnicode_ISO_2022_CN_OFFSETS_LOGIC,
    ಶೂನ್ಯಸೂಚಕ,

    ಶೂನ್ಯಸೂಚಕ,
    _ISO2022getName,
    _ISO_2022_WriteSub,
    ISO_2022_ಸುರಕ್ಷಿತ_Clone,
    ISO_2022_Get_Unicode_ಗಣ,

    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ
};
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ UConverter_Static_ದತ್ತಾಂಶ ISO2022_CNStatic_ದತ್ತಾಂಶ={
    ಗಾತ್ರ(UConverter_Static_ದತ್ತಾಂಶ),
    "ISO_2022_CN",
    0,
    UCNV_IBM,
    UCNV_ISO_2022,
    1,
    8, /* max 8 bytes per char16_t: 4-byte CNS designator + 2 bytes for SS2/SS3 + DBCS */
    { 0x1a, 0, 0, 0 },
    1,
    ಸುಳ್ಳು,
    ಸುಳ್ಳು,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

ನಾಮಾಕಾಶ {

ಸ್ಥಿರಾಂಕ UConverter_Shared_ದತ್ತಾಂಶ _ISO2022CNData=
        UCNV_IMMUTABLE_SHARED_ದತ್ತಾಂಶ_INITIALIZER(&ISO2022_CNStatic_ದತ್ತಾಂಶ, &_ISO2022CNImpl);

}  // namespace
#ಆದರೆ_ಅಂತ್ಯ /* #if !UCONFIG_ONLY_HTML_CONVERSION */

#ಆದರೆ_ಅಂತ್ಯ /* #if !UCONFIG_NO_LEGACY_CONVERSION */
