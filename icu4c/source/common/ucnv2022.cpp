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

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

#ਜੇ !UCONFIG_NO_CONVERSION && !UCONFIG_NO_LEGACY_CONVERSION

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ucnv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uset.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ucnv_err.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ucnv_cb.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utf16.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucnv_imp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucnv_bld.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucnv_cnv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucnvmbcs.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cstring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uassert.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ U_ENABLE_GENERIC_ISO_2022
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
#ਜੇ_ਅੰਤ

#ਜੇ !UCONFIG_ONLY_HTML_CONVERSION
ਸਥਿਰ ਅਚਲ ਅੱਖਰ ਖਿਸਕਾਓ_ਵਿੱਚ_STR[]  = "\x0F";
// static const char SHIFT_OUT_STR[] = "\x0E";
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CR      0x0D
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LF      0x0A
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ H_TAB   0x09
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ V_TAB   0x0B
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPACE   0x20

ਗਿਣਤੀ {
    HWKANA_ਸ਼ੁਰੂ=0xff61,
    HWKANA_ਸਿਰਾ=0xff9f
};

/*
 * 94-character sets with native byte values A1..FE are encoded in ISO 2022
 * as bytes 21..7E. (Subtract 0x80.)
 * 96-character sets with native byte values A0..FF are encoded in ISO 2022
 * as bytes 20..7F. (Subtract 0x80.)
 * Do not encode C1 control codes with native bytes 80..9F
 * as bytes 00..1F (C0 control codes).
 */
ਗਿਣਤੀ {
    GR94_ਸ਼ੁਰੂ=0xa1,
    GR94_ਸਿਰਾ=0xfe,
    GR96_ਸ਼ੁਰੂ=0xa0,
    GR96_ਸਿਰਾ=0xff
};

/*
 * ISO 2022 control codes must not be converted from Unicode
 * because they would mess up the byte stream.
 * The bit mask 0x0800c000 has bits set at bit positions 0xe, 0xf, 0x1b
 * corresponding to SO, SI, and ESC.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IS_2022_CONTROL(c) (((c)<0x20) && (((uint32_t)1<<(c))&0x0800c000)!=0)

/* for ISO-2022-JP and -CN implementations */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ  {
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
        HWKANA_7_ਦੋਆਧਾਰੀ_ਅੰਕ=8,    /* Halfwidth Katakana 7 bit */

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
#ਜੇ UCONFIG_ONLY_HTML_CONVERSION
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IS_JP_DBCS(cs) (JISX208==(cs))
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IS_JP_DBCS(cs) (JISX208<=(cs) && (cs)<=KSC5601)
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CSM(cs) ((uint16_t)1<<(cs))

/*
 * Each of these charset masks (with index x) contains a bit for a charset in exact correspondence
 * to whether that charset is used in the corresponding version x of ISO_2022,locale=ja,version=x
 *
 * Note: The converter uses some leniency:
 * - The escape sequence ESC ( I for half-width 7-bit Katakana is recognized in
 *   all versions, not just JIS7 and JIS8.
 * - ICU does not distinguish between different versions of JIS X 0208.
 */
#ਜੇ UCONFIG_ONLY_HTML_CONVERSION
ਗਿਣਤੀ { MAX_JA_ਸੰਸਕਰਣ=0 };
#ਨਹੀਂਤਾਂ
ਗਿਣਤੀ { MAX_JA_ਸੰਸਕਰਣ=4 };
#ਜੇ_ਅੰਤ
ਸਥਿਰ ਅਚਲ uint16_t jpCharsetMasks[MAX_JA_ਸੰਸਕਰਣ+1]={
    CSM(ASCII)|CSM(JISX201)|CSM(JISX208)|CSM(HWKANA_7_ਦੋਆਧਾਰੀ_ਅੰਕ),
#ਜੇ !UCONFIG_ONLY_HTML_CONVERSION
    CSM(ASCII)|CSM(JISX201)|CSM(JISX208)|CSM(HWKANA_7_ਦੋਆਧਾਰੀ_ਅੰਕ)|CSM(JISX212),
    CSM(ASCII)|CSM(JISX201)|CSM(JISX208)|CSM(HWKANA_7_ਦੋਆਧਾਰੀ_ਅੰਕ)|CSM(JISX212)|CSM(GB2312)|CSM(KSC5601)|CSM(ISO8859_1)|CSM(ISO8859_7),
    CSM(ASCII)|CSM(JISX201)|CSM(JISX208)|CSM(HWKANA_7_ਦੋਆਧਾਰੀ_ਅੰਕ)|CSM(JISX212)|CSM(GB2312)|CSM(KSC5601)|CSM(ISO8859_1)|CSM(ISO8859_7),
    CSM(ASCII)|CSM(JISX201)|CSM(JISX208)|CSM(HWKANA_7_ਦੋਆਧਾਰੀ_ਅੰਕ)|CSM(JISX212)|CSM(GB2312)|CSM(KSC5601)|CSM(ISO8859_1)|CSM(ISO8859_7)
#ਜੇ_ਅੰਤ
};

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
        ASCII1=0,
        LATIN1,
        SBCS,
        DBCS,
        MBCS,
        HWKANA
}Cnv2022Type;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ ISO2022State {
    int8_t cs[4];       /* charset number for SI (G0)/SO (G1)/SS2 (G2)/SS3 (G3) */
    int8_t g;           /* 0..3 for G0..G3 (SI/SO/SS2/SS3) */
    int8_t prevG;       /* g before single shift (SS2 or SS3) */
} ISO2022State;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UCNV_OPTIONS_ਸੰਸਕਰਣ_ਨਕਾਬ 0xf
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UCNV_2022_MAX_CONVERTERS 10

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ{
    UConverter_Shared_ਅੰਕੜੇ *myConverterArray[UCNV_2022_MAX_CONVERTERS];
    UConverter *currentConverter;
    Cnv2022Type currentType;
    ISO2022State ਮੰਜ਼ਿਲ_U2022_State, fromU2022State;
    uint32_t ਕੁੰਜੀ;
    uint32_t ਸੰਸਕਰਣ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ U_ENABLE_GENERIC_ISO_2022
    UBool is_ਪਹਿਲਾ_ਬਫਰ;
#ਜੇ_ਅੰਤ
    UBool is_ਖਾਲੀ_ਥਾਵਾਂ_Segment;
    ਅੱਖਰ name[30];
    ਅੱਖਰ locale[3];
}UConverter_ਅੰਕੜੇ_ISO2022;

/* Protos */
/* ISO-2022 ----------------------------------------------------------------- */

/*Forward declaration */
U_CFUNC ਖਾਲੀ U_CALLCONV
ucnv_fromUnicode_UTF8(UConverter_From_Unicode_ਦਲੀਲਾਂ * ਦਲੀਲਾਂ_2,
                      UError_ਕੋਡ * err);
U_CFUNC ਖਾਲੀ U_CALLCONV
ucnv_fromUnicode_UTF8_OFFSETS_LOGIC(UConverter_From_Unicode_ਦਲੀਲਾਂ * ਦਲੀਲਾਂ_2,
                                    UError_ਕੋਡ * err);

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ESC_2022 0x1B /*ESC*/

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ
{
        INVALID_2022 = -1, /*Doesn't correspond to a valid iso 2022 escape sequence*/
        VALID_NON_TERMINAL_2022 = 0, /*so far corresponds to a valid iso 2022 escape sequence*/
        VALID_TERMINAL_2022 = 1, /*corresponds to a valid iso 2022 escape sequence*/
        VALID_MAYBE_TERMINAL_2022 = 2 /*so far matches one iso 2022 escape sequence, but by adding more characters might match another escape sequence*/
} UCNV_ਸਾਰਣੀ_ਹਾਲਤਾਂ_2022;

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
ਸਥਿਰ ਅਚਲ int8_t normalize_esq_chars_2022[256] = {
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

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ U_ENABLE_GENERIC_ISO_2022
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
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_ਹਾਲਤਾਂ_2022 74
ਸਥਿਰ ਅਚਲ int32_t esc_Seq_State_ਸਾਰਣੀ_ਕੁੰਜੀ_2022[MAX_ਹਾਲਤਾਂ_2022] = {
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

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ U_ENABLE_GENERIC_ISO_2022

ਸਥਿਰ ਅਚਲ ਅੱਖਰ* ਅਚਲ esc_Seq_State_ਸਾਰਣੀ_ਨਤੀਜਾ_2022[MAX_ਹਾਲਤਾਂ_2022] = {
 /*  0                      1                        2                      3                   4                   5                        6                      7                       8                       9    */

     ਖਾਲੀ_ਸੂਚਕ                   ,ਖਾਲੀ_ਸੂਚਕ                   ,ਖਾਲੀ_ਸੂਚਕ                   ,ਖਾਲੀ_ਸੂਚਕ               ,ਖਾਲੀ_ਸੂਚਕ               ,ਖਾਲੀ_ਸੂਚਕ                   ,ਖਾਲੀ_ਸੂਚਕ                   ,ਖਾਲੀ_ਸੂਚਕ                   ,"latin1"               ,"latin1"
    ,"latin1"               ,"ibm-865"              ,"ibm-865"              ,"ibm-865"          ,"ibm-865"          ,"ibm-865"              ,"ibm-865"              ,"JISX0201"             ,"JISX0201"             ,"latin1"
    ,"latin1"               ,ਖਾਲੀ_ਸੂਚਕ                   ,"JISX-208"             ,"ibm-5478"         ,"JISX-208"         ,ਖਾਲੀ_ਸੂਚਕ                   ,ਖਾਲੀ_ਸੂਚਕ                   ,ਖਾਲੀ_ਸੂਚਕ                   ,ਖਾਲੀ_ਸੂਚਕ                   ,"UTF8"
    ,"ISO-8859-1"           ,"ISO-8859-7"           ,"JIS-X-208"            ,ਖਾਲੀ_ਸੂਚਕ               ,"ibm-955"          ,"ibm-367"              ,"ibm-952"              ,"ibm-949"              ,"JISX-212"             ,"ibm-1383"
    ,"ibm-952"              ,"ibm-964"              ,"ibm-964"              ,"ibm-964"          ,"ibm-964"          ,"ibm-964"              ,"ibm-964"              ,"ibm-5478"         ,"ibm-949"              ,"ISO-IR-165"
    ,"CNS-11643-1992,1"     ,"CNS-11643-1992,2"     ,"CNS-11643-1992,3"     ,"CNS-11643-1992,4" ,"CNS-11643-1992,5" ,"CNS-11643-1992,6"     ,"CNS-11643-1992,7"     ,"UTF16_PlatformEndian" ,"UTF16_PlatformEndian" ,"UTF16_PlatformEndian"
    ,"UTF16_PlatformEndian" ,"UTF16_PlatformEndian" ,"UTF16_PlatformEndian" ,ਖਾਲੀ_ਸੂਚਕ               ,"latin1"           ,"ibm-912"              ,"ibm-913"              ,"ibm-914"              ,"ibm-813"              ,"ibm-1089"
    ,"ibm-920"              ,"ibm-915"              ,"ibm-915"              ,"latin1"
};

#ਜੇ_ਅੰਤ

ਸਥਿਰ ਅਚਲ int8_t esc_Seq_State_ਸਾਰਣੀ_ਮੁੱਲ_2022[MAX_ਹਾਲਤਾਂ_2022] = {
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
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ U_ENABLE_GENERIC_ISO_2022
    ISO_2022=0,
#ਜੇ_ਅੰਤ
    ISO_2022_JP=1,
#ਜੇ !UCONFIG_ONLY_HTML_CONVERSION
    ISO_2022_KR=2,
    ISO_2022_CN=3
#ਜੇ_ਅੰਤ
} Variant2022;

/*********** ISO 2022 Converter Protos ***********/
ਸਥਿਰ ਖਾਲੀ U_CALLCONV
_ISO2022Open(UConverter *cnv, UConverter_ਲੱਦੋ_ਦਲੀਲਾਂ *p_ਦਲੀਲਾਂ, UError_ਕੋਡ *error_ਕੋਡ);

ਸਥਿਰ ਖਾਲੀ U_CALLCONV
 _ISO2022Close(UConverter *converter);

ਸਥਿਰ ਖਾਲੀ U_CALLCONV
_ISO2022Reset(UConverter *converter, UConverterResetChoice choice);

U_CDECL_BEGIN
ਸਥਿਰ ਅਚਲ ਅੱਖਰ * U_CALLCONV
_ISO2022getName(ਅਚਲ UConverter* cnv);
U_CDECL_ਸਿਰਾ

ਸਥਿਰ ਖਾਲੀ  U_CALLCONV
_ISO_2022_WriteSub(UConverter_From_Unicode_ਦਲੀਲਾਂ *ਦਲੀਲਾਂ_2, int32_t ਵਿੱਥ_ਸੂਚਕਾਂਕ, UError_ਕੋਡ *err);

U_CDECL_BEGIN
ਸਥਿਰ UConverter * U_CALLCONV
ISO_2022_ਸੁਰੱਖਿਅਤ_Clone(ਅਚਲ UConverter *cnv, ਖਾਲੀ *ਢੇਰ_ਬਫਰ, int32_t *p_ਬਫਰ_ਮਾਪ, UError_ਕੋਡ *ਸਥਿਤੀ);

U_CDECL_ਸਿਰਾ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ U_ENABLE_GENERIC_ISO_2022
ਸਥਿਰ ਖਾਲੀ U_CALLCONV
T_UConverter_ਮੰਜ਼ਿਲ_Unicode_ISO_2022_OFFSETS_LOGIC(UConverter_ਮੰਜ਼ਿਲ_Unicode_ਦਲੀਲਾਂ* ਦਲੀਲਾਂ_2, UError_ਕੋਡ* err);
#ਜੇ_ਅੰਤ

ਨਾਮਾਕਾਸ਼ {

/*const UConverterSharedData _ISO2022Data;*/
ਬਾਹਰੀ ਅਚਲ UConverter_Shared_ਅੰਕੜੇ _ISO2022JPData;

#ਜੇ !UCONFIG_ONLY_HTML_CONVERSION
ਬਾਹਰੀ ਅਚਲ UConverter_Shared_ਅੰਕੜੇ _ISO2022KRData;
ਬਾਹਰੀ ਅਚਲ UConverter_Shared_ਅੰਕੜੇ _ISO2022CNData;
#ਜੇ_ਅੰਤ

}  // namespace

/*************** Converter implementations ******************/

/* The purpose of this function is to get around gcc compiler warnings. */
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ
fromUWriteUInt8(UConverter *cnv,
                 ਅਚਲ ਅੱਖਰ *ਬਾਈਟਾਂ, int32_t length,
                 uint8_t **ਨਿਸ਼ਾਨਾ, ਅਚਲ ਅੱਖਰ *ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ,
                 int32_t **offsets,
                 int32_t ਸਰੋਤ_ਸੂਚਕਾਂਕ,
                 UError_ਕੋਡ *p_Error_ਕੋਡ)
{
    ਅੱਖਰ* ਨਿਸ਼ਾਨਾ_Chars = reinterpret_cast<ਅੱਖਰ*>(*ਨਿਸ਼ਾਨਾ);
    ucnv_from_UWrite_ਬਾਈਟਾਂ(cnv, ਬਾਈਟਾਂ, length, &ਨਿਸ਼ਾਨਾ_Chars, ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ,
                         offsets, ਸਰੋਤ_ਸੂਚਕਾਂਕ, p_Error_ਕੋਡ);
    *ਨਿਸ਼ਾਨਾ = reinterpret_cast<uint8_t*>(ਨਿਸ਼ਾਨਾ_Chars);

}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ
ਸੈੱਟ_Initial_State_ਮੰਜ਼ਿਲ_Unicode_KR(UConverter* /*converter*/, UConverter_ਅੰਕੜੇ_ISO2022 *my_Converter_ਅੰਕੜੇ){
    ਜੇ(my_Converter_ਅੰਕੜੇ->ਸੰਸਕਰਣ == 1) {
        UConverter *cnv = my_Converter_ਅੰਕੜੇ->currentConverter;

        cnv->ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ=0;     /*ਵਿੱਥ*/
        cnv->ਬਹੁਲਕ=0;                /* state */
        cnv->ਮੰਜ਼ਿਲ_ULength=0;           /* byteIndex */
    }
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ
ਸੈੱਟ_Initial_State_From_Unicode_KR(UConverter* converter,UConverter_ਅੰਕੜੇ_ISO2022 *my_Converter_ਅੰਕੜੇ){
   /* in ISO-2022-KR the designator sequence appears only once
    * in a file so we append it only once
    */
    ਜੇ( converter->char_Error_ਬਫਰ_ਲੰਮਾਈ==0){

        converter->char_Error_ਬਫਰ_ਲੰਮਾਈ = 4;
        converter->char_Error_ਬਫਰ[0] = 0x1b;
        converter->char_Error_ਬਫਰ[1] = 0x24;
        converter->char_Error_ਬਫਰ[2] = 0x29;
        converter->char_Error_ਬਫਰ[3] = 0x43;
    }
    ਜੇ(my_Converter_ਅੰਕੜੇ->ਸੰਸਕਰਣ == 1) {
        UConverter *cnv = my_Converter_ਅੰਕੜੇ->currentConverter;

        cnv->fromUChar32=0;
        cnv->from_Unicode_ਸਥਿਤੀ=1;   /* prevLength */
    }
}

ਸਥਿਰ ਖਾਲੀ U_CALLCONV
_ISO2022Open(UConverter *cnv, UConverter_ਲੱਦੋ_ਦਲੀਲਾਂ *p_ਦਲੀਲਾਂ, UError_ਕੋਡ *error_ਕੋਡ){

    ਅੱਖਰ myLocale[7]={' ',' ',' ',' ',' ',' ', '\0'};

    cnv->extraInfo = uprv_malloc (ਆਕਾਰ (UConverter_ਅੰਕੜੇ_ISO2022));
    ਜੇ(cnv->extraInfo != ਖਾਲੀ_ਸੂਚਕ) {
        UConverterNamePieces ਢੇਰ_Pieces;
        UConverter_ਲੱਦੋ_ਦਲੀਲਾਂ ਢੇਰ_ਦਲੀਲਾਂ=UCNV_ਲੱਦੋ_ਦਲੀਲਾਂ_INITIALIZER;
        UConverter_ਅੰਕੜੇ_ISO2022* my_Converter_ਅੰਕੜੇ = static_cast<UConverter_ਅੰਕੜੇ_ISO2022*>(cnv->extraInfo);
        uint32_t ਸੰਸਕਰਣ;

        ਢੇਰ_ਦਲੀਲਾਂ.onlyTestIsLoadable = p_ਦਲੀਲਾਂ->onlyTestIsLoadable;

        uprv_memset(my_Converter_ਅੰਕੜੇ, 0, ਆਕਾਰ(UConverter_ਅੰਕੜੇ_ISO2022));
        my_Converter_ਅੰਕੜੇ->currentType = ASCII1;
        cnv->from_Unicode_ਸਥਿਤੀ =ਝੂਠ;
        ਜੇ(p_ਦਲੀਲਾਂ->locale){
            uprv_strncpy(myLocale, p_ਦਲੀਲਾਂ->locale, ਆਕਾਰ(myLocale)-1);
        }
        ਸੰਸਕਰਣ = p_ਦਲੀਲਾਂ->options & UCNV_OPTIONS_ਸੰਸਕਰਣ_ਨਕਾਬ;
        my_Converter_ਅੰਕੜੇ->ਸੰਸਕਰਣ = ਸੰਸਕਰਣ;
        ਜੇ(myLocale[0]=='j' && (myLocale[1]=='a'|| myLocale[1]=='p') &&
            (myLocale[2]=='_' || myLocale[2]=='\0'))
        {
            /* open the required converters and cache them */
            ਜੇ(ਸੰਸਕਰਣ>MAX_JA_ਸੰਸਕਰਣ) {
                // ICU 55 fails to open a converter for an unsupported version.
                // Previously, it fell back to version 0, but that would yield
                // unexpected behavior.
                *error_ਕੋਡ = U_MISSING_RESOURCE_ERROR;
                ਵਾਪਸ;
            }
            ਜੇ(jpCharsetMasks[ਸੰਸਕਰਣ]&CSM(ISO8859_7)) {
                my_Converter_ਅੰਕੜੇ->myConverterArray[ISO8859_7] =
                    ucnv_ਲੱਦੋ_Shared_ਅੰਕੜੇ("ISO8859_7", &ਢੇਰ_Pieces, &ਢੇਰ_ਦਲੀਲਾਂ, error_ਕੋਡ);
            }
            my_Converter_ਅੰਕੜੇ->myConverterArray[JISX208] =
                ucnv_ਲੱਦੋ_Shared_ਅੰਕੜੇ("Shift-JIS", &ਢੇਰ_Pieces, &ਢੇਰ_ਦਲੀਲਾਂ, error_ਕੋਡ);
            ਜੇ(jpCharsetMasks[ਸੰਸਕਰਣ]&CSM(JISX212)) {
                my_Converter_ਅੰਕੜੇ->myConverterArray[JISX212] =
                    ucnv_ਲੱਦੋ_Shared_ਅੰਕੜੇ("jisx-212", &ਢੇਰ_Pieces, &ਢੇਰ_ਦਲੀਲਾਂ, error_ਕੋਡ);
            }
            ਜੇ(jpCharsetMasks[ਸੰਸਕਰਣ]&CSM(GB2312)) {
                my_Converter_ਅੰਕੜੇ->myConverterArray[GB2312] =
                    ucnv_ਲੱਦੋ_Shared_ਅੰਕੜੇ("ibm-5478", &ਢੇਰ_Pieces, &ਢੇਰ_ਦਲੀਲਾਂ, error_ਕੋਡ);   /* gb_2312_80-1 */
            }
            ਜੇ(jpCharsetMasks[ਸੰਸਕਰਣ]&CSM(KSC5601)) {
                my_Converter_ਅੰਕੜੇ->myConverterArray[KSC5601] =
                    ucnv_ਲੱਦੋ_Shared_ਅੰਕੜੇ("ksc_5601", &ਢੇਰ_Pieces, &ਢੇਰ_ਦਲੀਲਾਂ, error_ਕੋਡ);
            }

            /* set the function pointers to appropriate functions */
            cnv->shared_ਅੰਕੜੇ = const_cast<UConverter_Shared_ਅੰਕੜੇ*>(&_ISO2022JPData);
            uprv_strcpy(my_Converter_ਅੰਕੜੇ->locale,"ja");

            (ਖਾਲੀ)uprv_strcpy(my_Converter_ਅੰਕੜੇ->name,"ISO_2022,locale=ja,version=");
            size_t len = uprv_strlen(my_Converter_ਅੰਕੜੇ->name);
            my_Converter_ਅੰਕੜੇ->name[len] = static_cast<ਅੱਖਰ>(my_Converter_ਅੰਕੜੇ->ਸੰਸਕਰਣ + static_cast<ਪੂਰਨ>('0'));
            my_Converter_ਅੰਕੜੇ->name[len+1]='\0';
        }
#ਜੇ !UCONFIG_ONLY_HTML_CONVERSION
        ਨਹੀਂਤਾਂ ਜੇ(myLocale[0]=='k' && (myLocale[1]=='o'|| myLocale[1]=='r') &&
            (myLocale[2]=='_' || myLocale[2]=='\0'))
        {
            ਜੇ(ਸੰਸਕਰਣ>1) {
                // ICU 55 fails to open a converter for an unsupported version.
                // Previously, it fell back to version 0, but that would yield
                // unexpected behavior.
                *error_ਕੋਡ = U_MISSING_RESOURCE_ERROR;
                ਵਾਪਸ;
            }
            ਅਚਲ ਅੱਖਰ *cnvName;
            ਜੇ(ਸੰਸਕਰਣ==1) {
                cnvName="icu-internal-25546";
            } ਨਹੀਂਤਾਂ {
                cnvName="ibm-949";
                my_Converter_ਅੰਕੜੇ->ਸੰਸਕਰਣ=ਸੰਸਕਰਣ=0;
            }
            ਜੇ(p_ਦਲੀਲਾਂ->onlyTestIsLoadable) {
                ucnv_canCreateConverter(cnvName, error_ਕੋਡ);  /* errorCode carries result */
                uprv_free(cnv->extraInfo);
                cnv->extraInfo=ਖਾਲੀ_ਸੂਚਕ;
                ਵਾਪਸ;
            } ਨਹੀਂਤਾਂ {
                my_Converter_ਅੰਕੜੇ->currentConverter=ucnv_open(cnvName, error_ਕੋਡ);
                ਜੇ (U_FAILURE(*error_ਕੋਡ)) {
                    _ISO2022Close(cnv);
                    ਵਾਪਸ;
                }

                ਜੇ(ਸੰਸਕਰਣ==1) {
                    (ਖਾਲੀ)uprv_strcpy(my_Converter_ਅੰਕੜੇ->name,"ISO_2022,locale=ko,version=1");
                    uprv_memcpy(cnv->subChars, my_Converter_ਅੰਕੜੇ->currentConverter->subChars, 4);
                    cnv->subCharLen = my_Converter_ਅੰਕੜੇ->currentConverter->subCharLen;
                }ਨਹੀਂਤਾਂ{
                    (ਖਾਲੀ)uprv_strcpy(my_Converter_ਅੰਕੜੇ->name,"ISO_2022,locale=ko,version=0");
                }

                /* initialize the state variables */
                ਸੈੱਟ_Initial_State_ਮੰਜ਼ਿਲ_Unicode_KR(cnv, my_Converter_ਅੰਕੜੇ);
                ਸੈੱਟ_Initial_State_From_Unicode_KR(cnv, my_Converter_ਅੰਕੜੇ);

                /* set the function pointers to appropriate functions */
                cnv->shared_ਅੰਕੜੇ = const_cast<UConverter_Shared_ਅੰਕੜੇ*>(&_ISO2022KRData);
                uprv_strcpy(my_Converter_ਅੰਕੜੇ->locale,"ko");
            }
        }
        ਨਹੀਂਤਾਂ ਜੇ(((myLocale[0]=='z' && myLocale[1]=='h') || (myLocale[0]=='c'&& myLocale[1]=='n'))&&
            (myLocale[2]=='_' || myLocale[2]=='\0'))
        {
            ਜੇ(ਸੰਸਕਰਣ>2) {
                // ICU 55 fails to open a converter for an unsupported version.
                // Previously, it fell back to version 0, but that would yield
                // unexpected behavior.
                *error_ਕੋਡ = U_MISSING_RESOURCE_ERROR;
                ਵਾਪਸ;
            }

            /* open the required converters and cache them */
            my_Converter_ਅੰਕੜੇ->myConverterArray[GB2312_1] =
                ucnv_ਲੱਦੋ_Shared_ਅੰਕੜੇ("ibm-5478", &ਢੇਰ_Pieces, &ਢੇਰ_ਦਲੀਲਾਂ, error_ਕੋਡ);
            ਜੇ(ਸੰਸਕਰਣ>=1) {
                my_Converter_ਅੰਕੜੇ->myConverterArray[ISO_IR_165] =
                    ucnv_ਲੱਦੋ_Shared_ਅੰਕੜੇ("iso-ir-165", &ਢੇਰ_Pieces, &ਢੇਰ_ਦਲੀਲਾਂ, error_ਕੋਡ);
            }
            my_Converter_ਅੰਕੜੇ->myConverterArray[CNS_11643] =
                ucnv_ਲੱਦੋ_Shared_ਅੰਕੜੇ("cns-11643-1992", &ਢੇਰ_Pieces, &ਢੇਰ_ਦਲੀਲਾਂ, error_ਕੋਡ);


            /* set the function pointers to appropriate functions */
            cnv->shared_ਅੰਕੜੇ = const_cast<UConverter_Shared_ਅੰਕੜੇ*>(&_ISO2022CNData);
            uprv_strcpy(my_Converter_ਅੰਕੜੇ->locale,"cn");

            ਜੇ (ਸੰਸਕਰਣ==0){
                my_Converter_ਅੰਕੜੇ->ਸੰਸਕਰਣ = 0;
                (ਖਾਲੀ)uprv_strcpy(my_Converter_ਅੰਕੜੇ->name,"ISO_2022,locale=zh,version=0");
            }ਨਹੀਂਤਾਂ ਜੇ (ਸੰਸਕਰਣ==1){
                my_Converter_ਅੰਕੜੇ->ਸੰਸਕਰਣ = 1;
                (ਖਾਲੀ)uprv_strcpy(my_Converter_ਅੰਕੜੇ->name,"ISO_2022,locale=zh,version=1");
            }ਨਹੀਂਤਾਂ {
                my_Converter_ਅੰਕੜੇ->ਸੰਸਕਰਣ = 2;
                (ਖਾਲੀ)uprv_strcpy(my_Converter_ਅੰਕੜੇ->name,"ISO_2022,locale=zh,version=2");
            }
        }
#ਜੇ_ਅੰਤ  // !UCONFIG_ONLY_HTML_CONVERSION
        ਨਹੀਂਤਾਂ{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ U_ENABLE_GENERIC_ISO_2022
            my_Converter_ਅੰਕੜੇ->is_ਪਹਿਲਾ_ਬਫਰ = ਸੱਚ;

            /* append the UTF-8 escape sequence */
            cnv->char_Error_ਬਫਰ_ਲੰਮਾਈ = 3;
            cnv->char_Error_ਬਫਰ[0] = 0x1b;
            cnv->char_Error_ਬਫਰ[1] = 0x25;
            cnv->char_Error_ਬਫਰ[2] = 0x42;

            cnv->shared_ਅੰਕੜੇ=(UConverter_Shared_ਅੰਕੜੇ*)&ISO2022_ਅੰਕੜੇ;
            /* initialize the state variables */
            uprv_strcpy(my_Converter_ਅੰਕੜੇ->name,"ISO_2022");
#ਨਹੀਂਤਾਂ
            *error_ਕੋਡ = U_MISSING_RESOURCE_ERROR;
            // Was U_UNSUPPORTED_ERROR but changed in ICU 55 to a more standard
            // data loading error code.
            ਵਾਪਸ;
#ਜੇ_ਅੰਤ
        }

        cnv->max_ਬਾਈਟਾਂ_ਪ੍ਰਤੀ_UChar=cnv->shared_ਅੰਕੜੇ->static_ਅੰਕੜੇ->max_ਬਾਈਟਾਂ_ਪ੍ਰਤੀ_Char;

        ਜੇ(U_FAILURE(*error_ਕੋਡ) || p_ਦਲੀਲਾਂ->onlyTestIsLoadable) {
            _ISO2022Close(cnv);
        }
    } ਨਹੀਂਤਾਂ {
        *error_ਕੋਡ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
    }
}


ਸਥਿਰ ਖਾਲੀ U_CALLCONV
_ISO2022Close(UConverter *converter) {
    UConverter_ਅੰਕੜੇ_ISO2022* my_ਅੰਕੜੇ = static_cast<UConverter_ਅੰਕੜੇ_ISO2022*>(converter->extraInfo);
    UConverter_Shared_ਅੰਕੜੇ **array = my_ਅੰਕੜੇ->myConverterArray;
    int32_t i;

    ਜੇ (converter->extraInfo != ਖਾਲੀ_ਸੂਚਕ) {
        /*close the array of converter pointers and free the memory*/
        ਲਈ (i=0; i<UCNV_2022_MAX_CONVERTERS; i++) {
            ਜੇ(array[i]!=ਖਾਲੀ_ਸੂਚਕ) {
                ucnv_unload_Shared_ਅੰਕੜੇ_If_Ready(array[i]);
            }
        }

        ucnv_close(my_ਅੰਕੜੇ->currentConverter);

        ਜੇ(!converter->isExtraLocal){
            uprv_free (converter->extraInfo);
            converter->extraInfo = ਖਾਲੀ_ਸੂਚਕ;
        }
    }
}

ਸਥਿਰ ਖਾਲੀ U_CALLCONV
_ISO2022Reset(UConverter *converter, UConverterResetChoice choice) {
    UConverter_ਅੰਕੜੇ_ISO2022* my_Converter_ਅੰਕੜੇ = static_cast<UConverter_ਅੰਕੜੇ_ISO2022*>(converter->extraInfo);
    ਜੇ(choice<=UCNV_RESET_ਮੰਜ਼ਿਲ_UNICODE) {
        uprv_memset(&my_Converter_ਅੰਕੜੇ->ਮੰਜ਼ਿਲ_U2022_State, 0, ਆਕਾਰ(ISO2022State));
        my_Converter_ਅੰਕੜੇ->ਕੁੰਜੀ = 0;
        my_Converter_ਅੰਕੜੇ->is_ਖਾਲੀ_ਥਾਵਾਂ_Segment = ਝੂਠ;
    }
    ਜੇ(choice!=UCNV_RESET_ਮੰਜ਼ਿਲ_UNICODE) {
        uprv_memset(&my_Converter_ਅੰਕੜੇ->fromU2022State, 0, ਆਕਾਰ(ISO2022State));
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ U_ENABLE_GENERIC_ISO_2022
    ਜੇ(my_Converter_ਅੰਕੜੇ->locale[0] == 0){
        ਜੇ(choice<=UCNV_RESET_ਮੰਜ਼ਿਲ_UNICODE) {
            my_Converter_ਅੰਕੜੇ->is_ਪਹਿਲਾ_ਬਫਰ = ਸੱਚ;
            my_Converter_ਅੰਕੜੇ->ਕੁੰਜੀ = 0;
            ਜੇ (converter->ਬਹੁਲਕ == UCNV_SO){
                ucnv_close (my_Converter_ਅੰਕੜੇ->currentConverter);
                my_Converter_ਅੰਕੜੇ->currentConverter=ਖਾਲੀ_ਸੂਚਕ;
            }
            converter->ਬਹੁਲਕ = UCNV_SI;
        }
        ਜੇ(choice!=UCNV_RESET_ਮੰਜ਼ਿਲ_UNICODE) {
            /* re-append UTF-8 escape sequence */
            converter->char_Error_ਬਫਰ_ਲੰਮਾਈ = 3;
            converter->char_Error_ਬਫਰ[0] = 0x1b;
            converter->char_Error_ਬਫਰ[1] = 0x28;
            converter->char_Error_ਬਫਰ[2] = 0x42;
        }
    }
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
    {
        /* reset the state variables */
        ਜੇ(my_Converter_ਅੰਕੜੇ->locale[0] == 'k'){
            ਜੇ(choice<=UCNV_RESET_ਮੰਜ਼ਿਲ_UNICODE) {
                ਸੈੱਟ_Initial_State_ਮੰਜ਼ਿਲ_Unicode_KR(converter, my_Converter_ਅੰਕੜੇ);
            }
            ਜੇ(choice!=UCNV_RESET_ਮੰਜ਼ਿਲ_UNICODE) {
                ਸੈੱਟ_Initial_State_From_Unicode_KR(converter, my_Converter_ਅੰਕੜੇ);
            }
        }
    }
}

U_CDECL_BEGIN

ਸਥਿਰ ਅਚਲ ਅੱਖਰ * U_CALLCONV
_ISO2022getName(ਅਚਲ UConverter* cnv){
    ਜੇ(cnv->extraInfo){
        UConverter_ਅੰਕੜੇ_ISO2022* my_ਅੰਕੜੇ= (UConverter_ਅੰਕੜੇ_ISO2022*)cnv->extraInfo;
        ਵਾਪਸ my_ਅੰਕੜੇ->name;
    }
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
}

U_CDECL_ਸਿਰਾ


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
ਸਥਿਰ ਅਚਲ int8_t ਅਗਲਾ_State_ਮੰਜ਼ਿਲ_Unicode_JP[MAX_ਹਾਲਤਾਂ_2022]= {
/*      0                1               2               3               4               5               6               7               8               9    */
    INVALID_STATE   ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,SS2_STATE      ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE
    ,ASCII          ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,JISX201        ,HWKANA_7_ਦੋਆਧਾਰੀ_ਅੰਕ    ,JISX201        ,INVALID_STATE
    ,INVALID_STATE  ,INVALID_STATE  ,JISX208        ,GB2312         ,JISX208        ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE
    ,ISO8859_1      ,ISO8859_7      ,JISX208        ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,KSC5601        ,JISX212        ,INVALID_STATE
    ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE
    ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE
    ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE
    ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE
};

#ਜੇ !UCONFIG_ONLY_HTML_CONVERSION
/*************** to unicode *******************/
ਸਥਿਰ ਅਚਲ int8_t ਅਗਲਾ_State_ਮੰਜ਼ਿਲ_Unicode_CN[MAX_ਹਾਲਤਾਂ_2022]= {
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
#ਜੇ_ਅੰਤ


ਸਥਿਰ UCNV_ਸਾਰਣੀ_ਹਾਲਤਾਂ_2022
get_ਕੁੰਜੀ_2022(ਅੱਖਰ c,int32_t* ਕੁੰਜੀ,int32_t* ਵਿੱਥ){
    int32_t togo;
    int32_t ਨੀਵਾਂ = 0;
    int32_t hi = MAX_ਹਾਲਤਾਂ_2022;
    int32_t oldmid=0;

    togo = normalize_esq_chars_2022[static_cast<uint8_t>(c)];
    ਜੇ(togo == 0) {
        /* not a valid character anywhere in an escape sequence */
        *ਕੁੰਜੀ = 0;
        *ਵਿੱਥ = 0;
        ਵਾਪਸ INVALID_2022;
    }
    togo = (*ਕੁੰਜੀ << 5) + togo;

    ਜਦੋਂ (hi != ਨੀਵਾਂ)  /*ਦੁਆਧਾਰੀ ਖੋਜ*/{

        int32_t mid = (hi+ਨੀਵਾਂ) >> 1; /*Finds median*/

        ਜੇ (mid == oldmid)
            ਰੋਕੋ;

        ਜੇ (esc_Seq_State_ਸਾਰਣੀ_ਕੁੰਜੀ_2022[mid] > togo){
            hi = mid;
        }
        ਨਹੀਂਤਾਂ ਜੇ (esc_Seq_State_ਸਾਰਣੀ_ਕੁੰਜੀ_2022[mid] < togo){
            ਨੀਵਾਂ = mid;
        }
        ਨਹੀਂਤਾਂ /*we found it*/{
            *ਕੁੰਜੀ = togo;
            *ਵਿੱਥ = mid;
            ਵਾਪਸ static_cast<UCNV_ਸਾਰਣੀ_ਹਾਲਤਾਂ_2022>(esc_Seq_State_ਸਾਰਣੀ_ਮੁੱਲ_2022[mid]);
        }
        oldmid = mid;

    }

    *ਕੁੰਜੀ = 0;
    *ਵਿੱਥ = 0;
    ਵਾਪਸ INVALID_2022;
}

/*runs through a state machine to determine the escape sequence - codepage correspondence
 */
ਸਥਿਰ ਖਾਲੀ
changeState_2022(UConverter* _this,
                ਅਚਲ ਅੱਖਰ** ਸਰੋਤ,
                ਅਚਲ ਅੱਖਰ* ਸਰੋਤ_ਹੱਦਬੰਦੀ,
                Variant2022 var,
                UError_ਕੋਡ* err){
    UCNV_ਸਾਰਣੀ_ਹਾਲਤਾਂ_2022 value;
    UConverter_ਅੰਕੜੇ_ISO2022* myData2022 = static_cast<UConverter_ਅੰਕੜੇ_ISO2022*>(_this->extraInfo);
    uint32_t ਕੁੰਜੀ = myData2022->ਕੁੰਜੀ;
    int32_t ਵਿੱਥ = 0;
    int8_t initial_ਮੰਜ਼ਿਲ_ULength = _this->ਮੰਜ਼ਿਲ_ULength;
    ਅੱਖਰ c;

    value = VALID_NON_TERMINAL_2022;
    ਜਦੋਂ (*ਸਰੋਤ < ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
        c = *(*ਸਰੋਤ)++;
        _this->ਮੰਜ਼ਿਲ_UBytes[_this->ਮੰਜ਼ਿਲ_ULength++] = static_cast<uint8_t>(c);
        value = get_ਕੁੰਜੀ_2022(c, reinterpret_cast<int32_t*>(&ਕੁੰਜੀ), &ਵਿੱਥ);

        ਚੋਣ (value){

        ਹਾਲਤ VALID_NON_TERMINAL_2022 :
            /* continue with the loop */
            ਰੋਕੋ;

        ਹਾਲਤ VALID_TERMINAL_2022:
            ਕੁੰਜੀ = 0;
            ਜਾਓ ਪੂਰਾ_2;

        ਹਾਲਤ INVALID_2022:
            ਜਾਓ ਪੂਰਾ_2;

        ਹਾਲਤ VALID_MAYBE_TERMINAL_2022:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ U_ENABLE_GENERIC_ISO_2022
            /* ESC ( B is ambiguous only for ISO_2022 itself */
            ਜੇ(var == ISO_2022) {
                /* discard toUBytes[] for ESC ( B because this sequence is correct and complete */
                _this->ਮੰਜ਼ਿਲ_ULength = 0;

                /* TODO need to indicate that ESC ( B was seen; if failure, then need to replay from source or from MBCS-style replay */

                /* continue with the loop */
                value = VALID_NON_TERMINAL_2022;
                ਰੋਕੋ;
            } ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
            {
                /* not ISO_2022 itself, finish here */
                value = VALID_TERMINAL_2022;
                ਕੁੰਜੀ = 0;
                ਜਾਓ ਪੂਰਾ_2;
            }
        }
    }

ਪੂਰਾ_2:
    myData2022->ਕੁੰਜੀ = ਕੁੰਜੀ;

    ਜੇ (value == VALID_NON_TERMINAL_2022) {
        /* indicate that the escape sequence is incomplete: key!=0 */
        ਵਾਪਸ;
    } ਨਹੀਂਤਾਂ ਜੇ (value == INVALID_2022 ) {
        *err = U_ILLEGAL_ESCAPE_SEQUENCE;
    } ਨਹੀਂਤਾਂ /* value == VALID_TERMINAL_2022 */ {
        ਚੋਣ(var){
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ U_ENABLE_GENERIC_ISO_2022
        ਹਾਲਤ ISO_2022:
        {
            ਅਚਲ ਅੱਖਰ *chosenConverterName = esc_Seq_State_ਸਾਰਣੀ_ਨਤੀਜਾ_2022[ਵਿੱਥ];
            ਜੇ(chosenConverterName == ਖਾਲੀ_ਸੂਚਕ) {
                /* SS2 or SS3 */
                *err = U_UNSUPPORTED_ESCAPE_SEQUENCE;
                _this->ਮੰਜ਼ਿਲ_UCallback_Reason = UCNV_UNASSIGNED;
                ਵਾਪਸ;
            }

            _this->ਬਹੁਲਕ = UCNV_SI;
            ucnv_close(myData2022->currentConverter);
            myData2022->currentConverter = myUConverter = ucnv_open(chosenConverterName, err);
            ਜੇ(U_SUCCESS(*err)) {
                myUConverter->fromCharErrorBehaviour = UCNV_ਮੰਜ਼ਿਲ_U_CALLBACK_STOP;
                _this->ਬਹੁਲਕ = UCNV_SO;
            }
            ਰੋਕੋ;
        }
#ਜੇ_ਅੰਤ
        ਹਾਲਤ ISO_2022_JP:
            {
                StateEnum ਆਰਜ਼ੀ_State = static_cast<StateEnum>(ਅਗਲਾ_State_ਮੰਜ਼ਿਲ_Unicode_JP[ਵਿੱਥ]);
                ਚੋਣ(ਆਰਜ਼ੀ_State) {
                ਹਾਲਤ INVALID_STATE:
                    *err = U_UNSUPPORTED_ESCAPE_SEQUENCE;
                    ਰੋਕੋ;
                ਹਾਲਤ SS2_STATE:
                    ਜੇ(myData2022->ਮੰਜ਼ਿਲ_U2022_State.cs[2]!=0) {
                        ਜੇ(myData2022->ਮੰਜ਼ਿਲ_U2022_State.g<2) {
                            myData2022->ਮੰਜ਼ਿਲ_U2022_State.prevG=myData2022->ਮੰਜ਼ਿਲ_U2022_State.g;
                        }
                        myData2022->ਮੰਜ਼ਿਲ_U2022_State.g=2;
                    } ਨਹੀਂਤਾਂ {
                        /* illegal to have SS2 before a matching designator */
                        *err = U_ILLEGAL_ESCAPE_SEQUENCE;
                    }
                    ਰੋਕੋ;
                /* case SS3_STATE: not used in ISO-2022-JP-x */
                ਹਾਲਤ ISO8859_1:
                ਹਾਲਤ ISO8859_7:
                    ਜੇ((jpCharsetMasks[myData2022->ਸੰਸਕਰਣ] & CSM(ਆਰਜ਼ੀ_State)) == 0) {
                        *err = U_UNSUPPORTED_ESCAPE_SEQUENCE;
                    } ਨਹੀਂਤਾਂ {
                        /* G2 charset for SS2 */
                        myData2022->ਮੰਜ਼ਿਲ_U2022_State.cs[2] = static_cast<int8_t>(ਆਰਜ਼ੀ_State);
                    }
                    ਰੋਕੋ;
                ਮੂਲ:
                    ਜੇ((jpCharsetMasks[myData2022->ਸੰਸਕਰਣ] & CSM(ਆਰਜ਼ੀ_State)) == 0) {
                        *err = U_UNSUPPORTED_ESCAPE_SEQUENCE;
                    } ਨਹੀਂਤਾਂ {
                        /* G0 charset */
                        myData2022->ਮੰਜ਼ਿਲ_U2022_State.cs[0] = static_cast<int8_t>(ਆਰਜ਼ੀ_State);
                    }
                    ਰੋਕੋ;
                }
            }
            ਰੋਕੋ;
#ਜੇ !UCONFIG_ONLY_HTML_CONVERSION
        ਹਾਲਤ ISO_2022_CN:
            {
                StateEnum ਆਰਜ਼ੀ_State = static_cast<StateEnum>(ਅਗਲਾ_State_ਮੰਜ਼ਿਲ_Unicode_CN[ਵਿੱਥ]);
                ਚੋਣ(ਆਰਜ਼ੀ_State) {
                ਹਾਲਤ INVALID_STATE:
                    *err = U_UNSUPPORTED_ESCAPE_SEQUENCE;
                    ਰੋਕੋ;
                ਹਾਲਤ SS2_STATE:
                    ਜੇ(myData2022->ਮੰਜ਼ਿਲ_U2022_State.cs[2]!=0) {
                        ਜੇ(myData2022->ਮੰਜ਼ਿਲ_U2022_State.g<2) {
                            myData2022->ਮੰਜ਼ਿਲ_U2022_State.prevG=myData2022->ਮੰਜ਼ਿਲ_U2022_State.g;
                        }
                        myData2022->ਮੰਜ਼ਿਲ_U2022_State.g=2;
                    } ਨਹੀਂਤਾਂ {
                        /* illegal to have SS2 before a matching designator */
                        *err = U_ILLEGAL_ESCAPE_SEQUENCE;
                    }
                    ਰੋਕੋ;
                ਹਾਲਤ SS3_STATE:
                    ਜੇ(myData2022->ਮੰਜ਼ਿਲ_U2022_State.cs[3]!=0) {
                        ਜੇ(myData2022->ਮੰਜ਼ਿਲ_U2022_State.g<2) {
                            myData2022->ਮੰਜ਼ਿਲ_U2022_State.prevG=myData2022->ਮੰਜ਼ਿਲ_U2022_State.g;
                        }
                        myData2022->ਮੰਜ਼ਿਲ_U2022_State.g=3;
                    } ਨਹੀਂਤਾਂ {
                        /* illegal to have SS3 before a matching designator */
                        *err = U_ILLEGAL_ESCAPE_SEQUENCE;
                    }
                    ਰੋਕੋ;
                ਹਾਲਤ ISO_IR_165:
                    ਜੇ(myData2022->ਸੰਸਕਰਣ==0) {
                        *err = U_UNSUPPORTED_ESCAPE_SEQUENCE;
                        ਰੋਕੋ;
                    }
                    U_FALLTHROUGH;
                ਹਾਲਤ GB2312_1:
                    U_FALLTHROUGH;
                ਹਾਲਤ CNS_11643_1:
                    myData2022->ਮੰਜ਼ਿਲ_U2022_State.cs[1] = static_cast<int8_t>(ਆਰਜ਼ੀ_State);
                    ਰੋਕੋ;
                ਹਾਲਤ CNS_11643_2:
                    myData2022->ਮੰਜ਼ਿਲ_U2022_State.cs[2] = static_cast<int8_t>(ਆਰਜ਼ੀ_State);
                    ਰੋਕੋ;
                ਮੂਲ:
                    /* other CNS 11643 planes */
                    ਜੇ(myData2022->ਸੰਸਕਰਣ==0) {
                        *err = U_UNSUPPORTED_ESCAPE_SEQUENCE;
                    } ਨਹੀਂਤਾਂ {
                        myData2022->ਮੰਜ਼ਿਲ_U2022_State.cs[3] = static_cast<int8_t>(ਆਰਜ਼ੀ_State);
                    }
                    ਰੋਕੋ;
                }
            }
            ਰੋਕੋ;
        ਹਾਲਤ ISO_2022_KR:
            ਜੇ(ਵਿੱਥ==0x30){
                /* nothing to be done, just accept this one escape sequence */
            } ਨਹੀਂਤਾਂ {
                *err = U_UNSUPPORTED_ESCAPE_SEQUENCE;
            }
            ਰੋਕੋ;
#ਜੇ_ਅੰਤ  // !UCONFIG_ONLY_HTML_CONVERSION

        ਮੂਲ:
            *err = U_ILLEGAL_ESCAPE_SEQUENCE;
            ਰੋਕੋ;
        }
    }
    ਜੇ(U_SUCCESS(*err)) {
        _this->ਮੰਜ਼ਿਲ_ULength = 0;
    } ਨਹੀਂਤਾਂ ਜੇ(*err==U_ILLEGAL_ESCAPE_SEQUENCE) {
        ਜੇ(_this->ਮੰਜ਼ਿਲ_ULength>1) {
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
            int8_t ਪਿੱਠ_ਬਾਹਰਲਾ_ਦੂਰੀ=_this->ਮੰਜ਼ਿਲ_ULength-1;
            int8_t ਬਾਈਟਾਂ_From_This_ਬਫਰ=_this->ਮੰਜ਼ਿਲ_ULength-initial_ਮੰਜ਼ਿਲ_ULength;
            ਜੇ(ਪਿੱਠ_ਬਾਹਰਲਾ_ਦੂਰੀ<=ਬਾਈਟਾਂ_From_This_ਬਫਰ) {
                /* same as initialToULength<=1 */
                *ਸਰੋਤ-=ਪਿੱਠ_ਬਾਹਰਲਾ_ਦੂਰੀ;
            } ਨਹੀਂਤਾਂ {
                /* Back out bytes from the previous buffer: Need to replay them. */
                _this->pre_ਮੰਜ਼ਿਲ_ULength = static_cast<int8_t>(ਬਾਈਟਾਂ_From_This_ਬਫਰ - ਪਿੱਠ_ਬਾਹਰਲਾ_ਦੂਰੀ);
                /* same as -(initialToULength-1) */
                /* preToULength is negative! */
                uprv_memcpy(_this->pre_ਮੰਜ਼ਿਲ_U, _this->ਮੰਜ਼ਿਲ_UBytes+1, -_this->pre_ਮੰਜ਼ਿਲ_ULength);
                *ਸਰੋਤ-=ਬਾਈਟਾਂ_From_This_ਬਫਰ;
            }
            _this->ਮੰਜ਼ਿਲ_ULength=1;
        }
    } ਨਹੀਂਤਾਂ ਜੇ(*err==U_UNSUPPORTED_ESCAPE_SEQUENCE) {
        _this->ਮੰਜ਼ਿਲ_UCallback_Reason = UCNV_UNASSIGNED;
    }
}

#ਜੇ !UCONFIG_ONLY_HTML_CONVERSION
/*Checks the characters of the buffer against valid 2022 escape sequences
*if the match we return a pointer to the initial start of the sequence otherwise
*we return sourceLimit
*/
/*for 2022 looks ahead in the stream
 *to determine the longest possible convertible
 *data stream
 */
ਸਥਿਰ ਸਤਰਅੰਦਰ ਅਚਲ ਅੱਖਰ*
get_ਸਿਰਾ_ਦਾ_ਬਫਰ_2022(ਅਚਲ ਅੱਖਰ** ਸਰੋਤ,
                   ਅਚਲ ਅੱਖਰ* ਸਰੋਤ_ਹੱਦਬੰਦੀ,
                   UBool /*flush*/){

    ਅਚਲ ਅੱਖਰ* my_ਸਰੋਤ = *ਸਰੋਤ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ U_ENABLE_GENERIC_ISO_2022
    ਜੇ (*ਸਰੋਤ >= ਸਰੋਤ_ਹੱਦਬੰਦੀ)
        ਵਾਪਸ ਸਰੋਤ_ਹੱਦਬੰਦੀ;

    ਕਰੋ{

        ਜੇ (*my_ਸਰੋਤ == ESC_2022){
            int8_t i;
            int32_t ਕੁੰਜੀ = 0;
            int32_t ਵਿੱਥ;
            UCNV_ਸਾਰਣੀ_ਹਾਲਤਾਂ_2022 value = VALID_NON_TERMINAL_2022;

            /* Kludge: I could not
            * figure out the reason for validating an escape sequence
            * twice - once here and once in changeState_2022().
            * is it possible to have an ESC character in a ISO2022
            * byte stream which is valid in a code page? Is it legal?
            */
            ਲਈ (i=0;
            (my_ਸਰੋਤ+i < ਸਰੋਤ_ਹੱਦਬੰਦੀ)&&(value == VALID_NON_TERMINAL_2022);
            i++) {
                value =  get_ਕੁੰਜੀ_2022(*(my_ਸਰੋਤ+i), &ਕੁੰਜੀ, &ਵਿੱਥ);
            }
            ਜੇ (value > 0 || *my_ਸਰੋਤ==ESC_2022)
                ਵਾਪਸ my_ਸਰੋਤ;

            ਜੇ ((value == VALID_NON_TERMINAL_2022)&&(!flush) )
                ਵਾਪਸ ਸਰੋਤ_ਹੱਦਬੰਦੀ;
        }
    }ਜਦੋਂ (++my_ਸਰੋਤ < ਸਰੋਤ_ਹੱਦਬੰਦੀ);

    ਵਾਪਸ ਸਰੋਤ_ਹੱਦਬੰਦੀ;
#ਨਹੀਂਤਾਂ
    ਜਦੋਂ(my_ਸਰੋਤ < ਸਰੋਤ_ਹੱਦਬੰਦੀ && *my_ਸਰੋਤ != ESC_2022) {
        ++my_ਸਰੋਤ;
    }
    ਵਾਪਸ my_ਸਰੋਤ;
#ਜੇ_ਅੰਤ
}
#ਜੇ_ਅੰਤ

/* This inline function replicates code in _MBCSFromUChar32() function in ucnvmbcs.c
 * any future change in _MBCSFromUChar32() function should be reflected here.
 * @return number of bytes in *value; negative number if fallback; 0 if no mapping
 */
ਸਥਿਰ ਸਤਰਅੰਦਰ int32_t
MBCS_FROM_UCHAR32_ISO2022(UConverter_Shared_ਅੰਕੜੇ* shared_ਅੰਕੜੇ,
                                         UChar32 c,
                                         uint32_t* value,
                                         UBool useFallback,
                                         ਪੂਰਨ outputType)
{
    ਅਚਲ int32_t *cx;
    ਅਚਲ uint16_t *ਸਾਰਣੀ;
    uint32_t stage2_ਇੰਦਰਾਜ;
    uint32_t my_ਮੁੱਲ;
    int32_t length;
    ਅਚਲ uint8_t *p;
    /*
     * TODO(markus): Use and require new, faster MBCS conversion table structures.
     * Use internal version of ucnv_open() that verifies that the new structures are available,
     * else U_INTERNAL_PROGRAM_ERROR.
     */
    /* BMP-only codepages are stored without stage 1 entries for supplementary code points */
    ਜੇ(c<0x10000 || (shared_ਅੰਕੜੇ->mbcs.unicode_ਨਕਾਬ&UCNV_HAS_SUPPLEMENTARY)) {
        ਸਾਰਣੀ=shared_ਅੰਕੜੇ->mbcs.from_Unicode_ਸਾਰਣੀ;
        stage2_ਇੰਦਰਾਜ=MBCS_STAGE_2_FROM_U(ਸਾਰਣੀ, c);
        /* get the bytes and the length for the output */
        ਜੇ(outputType==MBCS_OUTPUT_2){
            my_ਮੁੱਲ=MBCS_ਮੁੱਲ_2_FROM_STAGE_2(shared_ਅੰਕੜੇ->mbcs.from_Unicode_ਬਾਈਟਾਂ, stage2_ਇੰਦਰਾਜ, c);
            ਜੇ(my_ਮੁੱਲ<=0xff) {
                length=1;
            } ਨਹੀਂਤਾਂ {
                length=2;
            }
        } ਨਹੀਂਤਾਂ /* outputType==MBCS_OUTPUT_3 */ {
            p=MBCS_ਸੂਚਕ_3_FROM_STAGE_2(shared_ਅੰਕੜੇ->mbcs.from_Unicode_ਬਾਈਟਾਂ, stage2_ਇੰਦਰਾਜ, c);
            my_ਮੁੱਲ = (static_cast<uint32_t>(*p) << 16) | (static_cast<uint32_t>(p[1]) << 8) | p[2];
            ਜੇ(my_ਮੁੱਲ<=0xff) {
                length=1;
            } ਨਹੀਂਤਾਂ ਜੇ(my_ਮੁੱਲ<=0xffff) {
                length=2;
            } ਨਹੀਂਤਾਂ {
                length=3;
            }
        }
        /* is this code point assigned, or do we use fallbacks? */
        ਜੇ((stage2_ਇੰਦਰਾਜ&(1<<(16+(c&0xf))))!=0) {
            /* assigned */
            *value=my_ਮੁੱਲ;
            ਵਾਪਸ length;
        } ਨਹੀਂਤਾਂ ਜੇ(FROM_U_USE_FALLBACK(useFallback, c) && my_ਮੁੱਲ!=0) {
            /*
             * We allow a 0 byte output if the "assigned" bit is set for this entry.
             * There is no way with this data structure for fallback output
             * to be a zero byte.
             */
            *value=my_ਮੁੱਲ;
            ਵਾਪਸ -length;
        }
    }

    cx=shared_ਅੰਕੜੇ->mbcs.extIndexes;
    ਜੇ(cx!=ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ ucnv_ext_Simple_ਮੇਲ_From_U(cx, c, value, useFallback);
    }

    /* unassigned */
    ਵਾਪਸ 0;
}

/* This inline function replicates code in _MBCSSingleFromUChar32() function in ucnvmbcs.c
 * any future change in _MBCSSingleFromUChar32() function should be reflected here.
 * @param retval pointer to output byte
 * @return 1 roundtrip byte  0 no mapping  -1 fallback byte
 */
ਸਥਿਰ ਸਤਰਅੰਦਰ int32_t
MBCS_SINGLE_FROM_UCHAR32(UConverter_Shared_ਅੰਕੜੇ* shared_ਅੰਕੜੇ,
                                       UChar32 c,
                                       uint32_t* retval,
                                       UBool useFallback)
{
    ਅਚਲ uint16_t *ਸਾਰਣੀ;
    int32_t value;
    /* BMP-only codepages are stored without stage 1 entries for supplementary code points */
    ਜੇ(c>=0x10000 && !(shared_ਅੰਕੜੇ->mbcs.unicode_ਨਕਾਬ&UCNV_HAS_SUPPLEMENTARY)) {
        ਵਾਪਸ 0;
    }
    /* convert the Unicode code point in c into codepage bytes (same as in _MBCSFromUnicodeWithOffsets) */
    ਸਾਰਣੀ=shared_ਅੰਕੜੇ->mbcs.from_Unicode_ਸਾਰਣੀ;
    /* get the byte for the output */
    value=MBCS_SINGLE_ਨਤੀਜਾ_FROM_U(ਸਾਰਣੀ, (uint16_t *)shared_ਅੰਕੜੇ->mbcs.from_Unicode_ਬਾਈਟਾਂ, c);
    /* is this code point assigned, or do we use fallbacks? */
    *retval = static_cast<uint32_t>(value & 0xff);
    ਜੇ(value>=0xf00) {
        ਵਾਪਸ 1;  /* roundtrip */
    } ਨਹੀਂਤਾਂ ਜੇ(useFallback ? value>=0x800 : value>=0xc00) {
        ਵਾਪਸ -1;  /* fallback taken */
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ 0;  /* no mapping */
    }
}

/*
 * Check that the result is a 2-byte value with each byte in the range A1..FE
 * (strict EUC DBCS) before accepting it and subtracting 0x80 from each byte
 * to move it to the ISO 2022 range 21..7E.
 * Return 0 if out of range.
 */
ਸਥਿਰ ਸਤਰਅੰਦਰ uint32_t
_2022FromGR94DBCS(uint32_t value) {
    ਜੇ (static_cast<uint16_t>(value - 0xa1a1) <= (0xfefe - 0xa1a1) &&
        static_cast<uint8_t>(value - 0xa1) <= (0xfe - 0xa1)
    ) {
        ਵਾਪਸ value - 0x8080;  /* shift down to 21..7e byte range */
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ 0;  /* not valid for ISO 2022 */
    }
}

#ਜੇ 0 /* 5691: Call sites now check for validity. They can just += 0x8080 after that. */
/*
 * This method does the reverse of _2022FromGR94DBCS(). Given the 2022 code point, it returns the
 * 2 byte value that is in the range A1..FE for each byte. Otherwise it returns the 2022 code point
 * unchanged. 
 */
ਸਥਿਰ ਸਤਰਅੰਦਰ uint32_t
_2022ToGR94DBCS(uint32_t value) {
    uint32_t return_ਮੁੱਲ = value + 0x8080;
    ਜੇ( (uint16_t)(return_ਮੁੱਲ - 0xa1a1) <= (0xfefe - 0xa1a1) &&
        (uint8_t)(return_ਮੁੱਲ - 0xa1) <= (0xfe - 0xa1)) {
        ਵਾਪਸ return_ਮੁੱਲ;
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ value;
    }
}
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ U_ENABLE_GENERIC_ISO_2022

/**********************************************************************************
*  ISO-2022 Converter
*
*
*/

ਸਥਿਰ ਖਾਲੀ U_CALLCONV
T_UConverter_ਮੰਜ਼ਿਲ_Unicode_ISO_2022_OFFSETS_LOGIC(UConverter_ਮੰਜ਼ਿਲ_Unicode_ਦਲੀਲਾਂ* ਦਲੀਲਾਂ_2,
                                                           UError_ਕੋਡ* err){
    ਅਚਲ ਅੱਖਰ* my_ਸਰੋਤ_ਹੱਦਬੰਦੀ, *real_ਸਰੋਤ_ਹੱਦਬੰਦੀ;
    ਅਚਲ ਅੱਖਰ* ਸਰੋਤ_ਸ਼ੁਰੂ;
    ਅਚਲ char16_t* my_ਨਿਸ਼ਾਨਾ_ਸ਼ੁਰੂ;
    UConverter* saveThis;
    UConverter_ਅੰਕੜੇ_ISO2022* my_ਅੰਕੜੇ;
    int8_t length;

    saveThis = ਦਲੀਲਾਂ_2->converter;
    my_ਅੰਕੜੇ=((UConverter_ਅੰਕੜੇ_ISO2022*)(saveThis->extraInfo));

    real_ਸਰੋਤ_ਹੱਦਬੰਦੀ = ਦਲੀਲਾਂ_2->ਸਰੋਤ_ਹੱਦਬੰਦੀ;
    ਜਦੋਂ (ਦਲੀਲਾਂ_2->ਸਰੋਤ < real_ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
        ਜੇ(my_ਅੰਕੜੇ->ਕੁੰਜੀ == 0) { /* are we in the middle of an escape sequence? */
            /*Find the end of the buffer e.g : Next Escape Seq | end of Buffer*/
            my_ਸਰੋਤ_ਹੱਦਬੰਦੀ = get_ਸਿਰਾ_ਦਾ_ਬਫਰ_2022(&(ਦਲੀਲਾਂ_2->ਸਰੋਤ), real_ਸਰੋਤ_ਹੱਦਬੰਦੀ, ਦਲੀਲਾਂ_2->flush);

            ਜੇ(ਦਲੀਲਾਂ_2->ਸਰੋਤ < my_ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
                ਜੇ(my_ਅੰਕੜੇ->currentConverter==ਖਾਲੀ_ਸੂਚਕ) {
                    my_ਅੰਕੜੇ->currentConverter = ucnv_open("ASCII",err);
                    ਜੇ(U_FAILURE(*err)){
                        ਵਾਪਸ;
                    }

                    my_ਅੰਕੜੇ->currentConverter->fromCharErrorBehaviour = UCNV_ਮੰਜ਼ਿਲ_U_CALLBACK_STOP;
                    saveThis->ਬਹੁਲਕ = UCNV_SO;
                }

                /* convert to before the ESC or until the end of the buffer */
                my_ਅੰਕੜੇ->is_ਪਹਿਲਾ_ਬਫਰ=ਝੂਠ;
                ਸਰੋਤ_ਸ਼ੁਰੂ = ਦਲੀਲਾਂ_2->ਸਰੋਤ;
                my_ਨਿਸ਼ਾਨਾ_ਸ਼ੁਰੂ = ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ;
                ਦਲੀਲਾਂ_2->converter = my_ਅੰਕੜੇ->currentConverter;
                ucnv_ਮੰਜ਼ਿਲ_Unicode(ਦਲੀਲਾਂ_2->converter,
                    &ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ,
                    ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ,
                    &ਦਲੀਲਾਂ_2->ਸਰੋਤ,
                    my_ਸਰੋਤ_ਹੱਦਬੰਦੀ,
                    ਦਲੀਲਾਂ_2->offsets,
                    (UBool)(ਦਲੀਲਾਂ_2->flush && my_ਸਰੋਤ_ਹੱਦਬੰਦੀ == real_ਸਰੋਤ_ਹੱਦਬੰਦੀ),
                    err);
                ਦਲੀਲਾਂ_2->converter = saveThis;

                ਜੇ (*err == U_ਬਫਰ_ਉਛਾਲ_ERROR) {
                    /* move the overflow buffer */
                    length = saveThis->UChar_Error_ਬਫਰ_ਲੰਮਾਈ = my_ਅੰਕੜੇ->currentConverter->UChar_Error_ਬਫਰ_ਲੰਮਾਈ;
                    my_ਅੰਕੜੇ->currentConverter->UChar_Error_ਬਫਰ_ਲੰਮਾਈ = 0;
                    ਜੇ(length > 0) {
                        uprv_memcpy(saveThis->UChar_Error_ਬਫਰ,
                                    my_ਅੰਕੜੇ->currentConverter->UChar_Error_ਬਫਰ,
                                    length*U_SIZEOF_UCHAR);
                    }
                    ਵਾਪਸ;
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
                ਜੇ (U_FAILURE(*err) ||
                    (ਦਲੀਲਾਂ_2->ਸਰੋਤ == real_ਸਰੋਤ_ਹੱਦਬੰਦੀ) ||
                    (ਦਲੀਲਾਂ_2->offsets != ਖਾਲੀ_ਸੂਚਕ && (ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ != my_ਨਿਸ਼ਾਨਾ_ਸ਼ੁਰੂ || ਦਲੀਲਾਂ_2->ਸਰੋਤ != ਸਰੋਤ_ਸ਼ੁਰੂ) ||
                    (my_ਸਰੋਤ_ਹੱਦਬੰਦੀ < real_ਸਰੋਤ_ਹੱਦਬੰਦੀ && my_ਅੰਕੜੇ->currentConverter->ਮੰਜ਼ਿਲ_ULength > 0))
                ) {
                    /* copy partial or error input for truncated detection and error handling */
                    ਜੇ(U_FAILURE(*err)) {
                        length = saveThis->invalid_Char_ਲੰਮਾਈ = my_ਅੰਕੜੇ->currentConverter->invalid_Char_ਲੰਮਾਈ;
                        ਜੇ(length > 0) {
                            uprv_memcpy(saveThis->invalid_Char_ਬਫਰ, my_ਅੰਕੜੇ->currentConverter->invalid_Char_ਬਫਰ, length);
                        }
                    } ਨਹੀਂਤਾਂ {
                        length = saveThis->ਮੰਜ਼ਿਲ_ULength = my_ਅੰਕੜੇ->currentConverter->ਮੰਜ਼ਿਲ_ULength;
                        ਜੇ(length > 0) {
                            uprv_memcpy(saveThis->ਮੰਜ਼ਿਲ_UBytes, my_ਅੰਕੜੇ->currentConverter->ਮੰਜ਼ਿਲ_UBytes, length);
                            ਜੇ(ਦਲੀਲਾਂ_2->ਸਰੋਤ < my_ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
                                *err = U_TRUNCATED_CHAR_ਮਿਲਿਆ; /* truncated input before ESC */
                            }
                        }
                    }
                    ਵਾਪਸ;
                }
            }
        }

        ਸਰੋਤ_ਸ਼ੁਰੂ = ਦਲੀਲਾਂ_2->ਸਰੋਤ;
        changeState_2022(ਦਲੀਲਾਂ_2->converter,
               &(ਦਲੀਲਾਂ_2->ਸਰੋਤ),
               real_ਸਰੋਤ_ਹੱਦਬੰਦੀ,
               ISO_2022,
               err);
        ਜੇ (U_FAILURE(*err) || (ਦਲੀਲਾਂ_2->ਸਰੋਤ != ਸਰੋਤ_ਸ਼ੁਰੂ && ਦਲੀਲਾਂ_2->offsets != ਖਾਲੀ_ਸੂਚਕ)) {
            /* let the ucnv.c code update its current offset */
            ਵਾਪਸ;
        }
    }
}

#ਜੇ_ਅੰਤ

/*
 * To Unicode Callback helper function
 */
ਸਥਿਰ ਖਾਲੀ
ਮੰਜ਼ਿਲ_Unicode_Callback(UConverter *cnv,
                  ਅਚਲ uint32_t ਸਰੋਤ_Char, ਅਚਲ uint32_t ਨਿਸ਼ਾਨਾ_Uni_Char,
                  UError_ਕੋਡ* err){
    ਜੇ(ਸਰੋਤ_Char>0xff){
        cnv->ਮੰਜ਼ਿਲ_UBytes[0] = static_cast<uint8_t>(ਸਰੋਤ_Char >> 8);
        cnv->ਮੰਜ਼ਿਲ_UBytes[1] = static_cast<uint8_t>(ਸਰੋਤ_Char);
        cnv->ਮੰਜ਼ਿਲ_ULength = 2;
    }
    ਨਹੀਂਤਾਂ{
        cnv->ਮੰਜ਼ਿਲ_UBytes[0] = static_cast<ਅੱਖਰ>(ਸਰੋਤ_Char);
        cnv->ਮੰਜ਼ਿਲ_ULength = 1;
    }

    ਜੇ(ਨਿਸ਼ਾਨਾ_Uni_Char == (missingCharMarker-1/*0xfffe*/)){
        *err = U_INVALID_CHAR_ਮਿਲਿਆ;
    }
    ਨਹੀਂਤਾਂ{
        *err = U_ILLEGAL_CHAR_ਮਿਲਿਆ;
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
ਸਥਿਰ ਅਚਲ StateEnum jpCharsetPref[]={
    ASCII,
    JISX201,
    ISO8859_1,
    JISX208,
    ISO8859_7,
    JISX212,
    GB2312,
    KSC5601,
    HWKANA_7_ਦੋਆਧਾਰੀ_ਅੰਕ
};

/*
 * The escape sequences must be in order of the enum constants like JISX201  = 3,
 * not in order of jpCharsetPref[]!
 */
ਸਥਿਰ ਅਚਲ ਅੱਖਰ escSeqChars[][6] ={
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
ਸਥਿਰ  ਅਚਲ int8_t escSeqCharsLen[] ={
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
ਸਥਿਰ ਸਤਰਅੰਦਰ uint32_t
jisx201_ਮੰਜ਼ਿਲ_U(uint32_t value) {
    ਜੇ(value < 0x5c) {
        ਵਾਪਸ value;
    } ਨਹੀਂਤਾਂ ਜੇ(value == 0x5c) {
        ਵਾਪਸ 0xa5;
    } ਨਹੀਂਤਾਂ ਜੇ(value == 0x7e) {
        ਵਾਪਸ 0x203e;
    } ਨਹੀਂਤਾਂ /* value <= 0x7f */ {
        ਵਾਪਸ value;
    }
}

/* Map Unicode to 00..7F according to JIS X 0201. Return U+FFFE if unmappable. */
ਸਥਿਰ ਸਤਰਅੰਦਰ uint32_t
jisx201FromU(uint32_t value) {
    ਜੇ(value<=0x7f) {
        ਜੇ(value!=0x5c && value!=0x7e) {
            ਵਾਪਸ value;
        }
    } ਨਹੀਂਤਾਂ ਜੇ(value==0xa5) {
        ਵਾਪਸ 0x5c;
    } ਨਹੀਂਤਾਂ ਜੇ(value==0x203e) {
        ਵਾਪਸ 0x7e;
    }
    ਵਾਪਸ 0xfffe;
}

/*
 * Take a valid Shift-JIS byte pair, check that it is in the range corresponding
 * to JIS X 0208, and convert it to a pair of 21..7E bytes.
 * Return 0 if the byte pair is out of range.
 */
ਸਥਿਰ ਸਤਰਅੰਦਰ uint32_t
_2022FromSJIS(uint32_t value) {
    uint8_t trail;

    ਜੇ(value > 0xEFFC) {
        ਵਾਪਸ 0;  /* beyond JIS X 0208 */
    }

    trail = static_cast<uint8_t>(value);

    value &= 0xff00;  /* lead byte */
    ਜੇ(value <= 0x9f00) {
        value -= 0x7000;
    } ਨਹੀਂਤਾਂ /* 0xe000 <= value <= 0xef00 */ {
        value -= 0xb000;
    }
    value <<= 1;

    ਜੇ(trail <= 0x9e) {
        value -= 0x100;
        ਜੇ(trail <= 0x7e) {
            value |= trail - 0x1f;
        } ਨਹੀਂਤਾਂ {
            value |= trail - 0x20;
        }
    } ਨਹੀਂਤਾਂ /* trail <= 0xfc */ {
        value |= trail - 0x7e;
    }
    ਵਾਪਸ value;
}

/*
 * Convert a pair of JIS X 0208 21..7E bytes to Shift-JIS.
 * If either byte is outside 21..7E make sure that the result is not valid
 * for Shift-JIS so that the converter catches it.
 * Some invalid byte values already turn into equally invalid Shift-JIS
 * byte values and need not be tested explicitly.
 */
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ
_2022ToSJIS(uint8_t c1, uint8_t c2, ਅੱਖਰ ਬਾਈਟਾਂ[2]) {
    ਜੇ(c1&1) {
        ++c1;
        ਜੇ(c2 <= 0x5f) {
            c2 += 0x1f;
        } ਨਹੀਂਤਾਂ ਜੇ(c2 <= 0x7e) {
            c2 += 0x20;
        } ਨਹੀਂਤਾਂ {
            c2 = 0;  /* invalid */
        }
    } ਨਹੀਂਤਾਂ {
        ਜੇ (static_cast<uint8_t>(c2 - 0x21) <= ((0x7e) - 0x21)) {
            c2 += 0x7e;
        } ਨਹੀਂਤਾਂ {
            c2 = 0;  /* invalid */
        }
    }
    c1 >>= 1;
    ਜੇ(c1 <= 0x2f) {
        c1 += 0x70;
    } ਨਹੀਂਤਾਂ ਜੇ(c1 <= 0x3f) {
        c1 += 0xb0;
    } ਨਹੀਂਤਾਂ {
        c1 = 0;  /* invalid */
    }
    ਬਾਈਟਾਂ[0] = static_cast<ਅੱਖਰ>(c1);
    ਬਾਈਟਾਂ[1] = static_cast<ਅੱਖਰ>(c2);
}

/*
 * JIS X 0208 has fallbacks from Unicode half-width Katakana to full-width (DBCS)
 * Katakana.
 * Now that we use a Shift-JIS table for JIS X 0208 we need to hardcode these fallbacks
 * because Shift-JIS roundtrips half-width Katakana to single bytes.
 * These were the only fallbacks in ICU's jisx-208.ucm file.
 */
ਸਥਿਰ ਅਚਲ uint16_t hwkana_fb[HWKANA_ਸਿਰਾ - HWKANA_ਸ਼ੁਰੂ + 1] = {
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

ਸਥਿਰ ਖਾਲੀ U_CALLCONV
UConverter_fromUnicode_ISO_2022_JP_OFFSETS_LOGIC(UConverter_From_Unicode_ਦਲੀਲਾਂ* ਦਲੀਲਾਂ_2, UError_ਕੋਡ* err) {
    UConverter *cnv = ਦਲੀਲਾਂ_2->converter;
    UConverter_ਅੰਕੜੇ_ISO2022 *converter_ਅੰਕੜੇ;
    ISO2022State *pFromU2022State;
    uint8_t* ਨਿਸ਼ਾਨਾ = reinterpret_cast<uint8_t*>(ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ);
    ਅਚਲ uint8_t* ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ = reinterpret_cast<ਅਚਲ uint8_t*>(ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ);
    ਅਚਲ char16_t* ਸਰੋਤ = ਦਲੀਲਾਂ_2->ਸਰੋਤ;
    ਅਚਲ char16_t* ਸਰੋਤ_ਹੱਦਬੰਦੀ = ਦਲੀਲਾਂ_2->ਸਰੋਤ_ਹੱਦਬੰਦੀ;
    int32_t* offsets = ਦਲੀਲਾਂ_2->offsets;
    UChar32 ਸਰੋਤ_Char;
    ਅੱਖਰ ਬਫਰ[8];
    int32_t len, ਬਾਹਰਲਾ_Len;
    int8_t choices[10];
    int32_t choice_ਗਿਣਤੀ;
    uint32_t ਨਿਸ਼ਾਨਾ_ਮੁੱਲ = 0;
    UBool useFallback;

    int32_t i;
    int8_t cs, g;

    /* set up the state */
    converter_ਅੰਕੜੇ = static_cast<UConverter_ਅੰਕੜੇ_ISO2022*>(cnv->extraInfo);
    pFromU2022State   = &converter_ਅੰਕੜੇ->fromU2022State;

    choice_ਗਿਣਤੀ = 0;

    /* check if the last codepoint of previous buffer was a lead surrogate*/
    ਜੇ((ਸਰੋਤ_Char = cnv->fromUChar32)!=0 && ਨਿਸ਼ਾਨਾ< ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ) {
        ਜਾਓ getTrail;
    }

    ਜਦੋਂ(ਸਰੋਤ < ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
        ਜੇ(ਨਿਸ਼ਾਨਾ < ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ) {

            ਸਰੋਤ_Char  = *(ਸਰੋਤ++);
            /*check if the char is a First surrogate*/
            ਜੇ(U16_IS_SURROGATE(ਸਰੋਤ_Char)) {
                ਜੇ(U16_IS_SURROGATE_LEAD(ਸਰੋਤ_Char)) {
getTrail:
                    /*look ahead to find the trail surrogate*/
                    ਜੇ(ਸਰੋਤ < ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
                        /* test the following code unit */
                        char16_t trail = *ਸਰੋਤ;
                        ਜੇ(U16_IS_TRAIL(trail)) {
                            ਸਰੋਤ++;
                            ਸਰੋਤ_Char=U16_GET_SUPPLEMENTARY(ਸਰੋਤ_Char, trail);
                            cnv->fromUChar32=0x00;
                            /* convert this supplementary code point */
                            /* exit this condition tree */
                        } ਨਹੀਂਤਾਂ {
                            /* this is an unmatched lead code unit (1st surrogate) */
                            /* callback(illegal) */
                            *err=U_ILLEGAL_CHAR_ਮਿਲਿਆ;
                            cnv->fromUChar32=ਸਰੋਤ_Char;
                            ਰੋਕੋ;
                        }
                    } ਨਹੀਂਤਾਂ {
                        /* no more input */
                        cnv->fromUChar32=ਸਰੋਤ_Char;
                        ਰੋਕੋ;
                    }
                } ਨਹੀਂਤਾਂ {
                    /* this is an unmatched trail code unit (2nd surrogate) */
                    /* callback(illegal) */
                    *err=U_ILLEGAL_CHAR_ਮਿਲਿਆ;
                    cnv->fromUChar32=ਸਰੋਤ_Char;
                    ਰੋਕੋ;
                }
            }

            /* do not convert SO/SI/ESC */
            ਜੇ(IS_2022_CONTROL(ਸਰੋਤ_Char)) {
                /* callback(illegal) */
                *err=U_ILLEGAL_CHAR_ਮਿਲਿਆ;
                cnv->fromUChar32=ਸਰੋਤ_Char;
                ਰੋਕੋ;
            }

            /* do the conversion */

            ਜੇ(choice_ਗਿਣਤੀ == 0) {
                uint16_t csm;

                /*
                 * The csm variable keeps track of which charsets are allowed
                 * and not used yet while building the choices[].
                 */
                csm = jpCharsetMasks[converter_ਅੰਕੜੇ->ਸੰਸਕਰਣ];
                choice_ਗਿਣਤੀ = 0;

                /* JIS7/8: try single-byte half-width Katakana before JISX208 */
                ਜੇ(converter_ਅੰਕੜੇ->ਸੰਸਕਰਣ == 3 || converter_ਅੰਕੜੇ->ਸੰਸਕਰਣ == 4) {
                    choices[choice_ਗਿਣਤੀ++] = static_cast<int8_t>(HWKANA_7_ਦੋਆਧਾਰੀ_ਅੰਕ);
                }
                /* Do not try single-byte half-width Katakana for other versions. */
                csm &= ~CSM(HWKANA_7_ਦੋਆਧਾਰੀ_ਅੰਕ);

                /* try the current G0 charset */
                choices[choice_ਗਿਣਤੀ++] = cs = pFromU2022State->cs[0];
                csm &= ~CSM(cs);

                /* try the current G2 charset */
                ਜੇ((cs = pFromU2022State->cs[2]) != 0) {
                    choices[choice_ਗਿਣਤੀ++] = cs;
                    csm &= ~CSM(cs);
                }

                /* try all the other possible charsets */
                ਲਈ(i = 0; i < UPRV_LENGTHOF(jpCharsetPref); ++i) {
                    cs = static_cast<int8_t>(jpCharsetPref[i]);
                    ਜੇ(CSM(cs) & csm) {
                        choices[choice_ਗਿਣਤੀ++] = cs;
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

            ਲਈ(i = 0; i < choice_ਗਿਣਤੀ && len <= 0; ++i) {
                uint32_t value;
                int32_t len2;
                int8_t cs0 = choices[i];
                ਚੋਣ(cs0) {
                ਹਾਲਤ ASCII:
                    ਜੇ(ਸਰੋਤ_Char <= 0x7f) {
                        ਨਿਸ਼ਾਨਾ_ਮੁੱਲ = static_cast<uint32_t>(ਸਰੋਤ_Char);
                        len = 1;
                        cs = cs0;
                        g = 0;
                    }
                    ਰੋਕੋ;
                ਹਾਲਤ ISO8859_1:
                    ਜੇ(GR96_ਸ਼ੁਰੂ <= ਸਰੋਤ_Char && ਸਰੋਤ_Char <= GR96_ਸਿਰਾ) {
                        ਨਿਸ਼ਾਨਾ_ਮੁੱਲ = static_cast<uint32_t>(ਸਰੋਤ_Char) - 0x80;
                        len = 1;
                        cs = cs0;
                        g = 2;
                    }
                    ਰੋਕੋ;
                ਹਾਲਤ HWKANA_7_ਦੋਆਧਾਰੀ_ਅੰਕ:
                    ਜੇ (static_cast<uint32_t>(ਸਰੋਤ_Char - HWKANA_ਸ਼ੁਰੂ) <= (HWKANA_ਸਿਰਾ - HWKANA_ਸ਼ੁਰੂ)) {
                        ਜੇ(converter_ਅੰਕੜੇ->ਸੰਸਕਰਣ==3) {
                            /* JIS7: use G1 (SO) */
                            /* Shift U+FF61..U+FF9F to bytes 21..5F. */
                            ਨਿਸ਼ਾਨਾ_ਮੁੱਲ = static_cast<uint32_t>(ਸਰੋਤ_Char - (HWKANA_ਸ਼ੁਰੂ - 0x21));
                            len = 1;
                            pFromU2022State->cs[1] = cs = cs0; /* do not output an escape sequence */
                            g = 1;
                        } ਨਹੀਂਤਾਂ ਜੇ(converter_ਅੰਕੜੇ->ਸੰਸਕਰਣ==4) {
                            /* JIS8: use 8-bit bytes with any single-byte charset, see escape sequence output below */
                            /* Shift U+FF61..U+FF9F to bytes A1..DF. */
                            ਨਿਸ਼ਾਨਾ_ਮੁੱਲ = static_cast<uint32_t>(ਸਰੋਤ_Char - (HWKANA_ਸ਼ੁਰੂ - 0xa1));
                            len = 1;

                            cs = pFromU2022State->cs[0];
                            ਜੇ(IS_JP_DBCS(cs)) {
                                /* switch from a DBCS charset to JISX201 */
                                cs = static_cast<int8_t>(JISX201);
                            }
                            /* else stay in the current G0 charset */
                            g = 0;
                        }
                        /* else do not use HWKANA_7BIT with other versions */
                    }
                    ਰੋਕੋ;
                ਹਾਲਤ JISX201:
                    /* G0 SBCS */
                    value = jisx201FromU(ਸਰੋਤ_Char);
                    ਜੇ(value <= 0x7f) {
                        ਨਿਸ਼ਾਨਾ_ਮੁੱਲ = value;
                        len = 1;
                        cs = cs0;
                        g = 0;
                        useFallback = ਝੂਠ;
                    }
                    ਰੋਕੋ;
                ਹਾਲਤ JISX208:
                    /* G0 DBCS from Shift-JIS table */
                    len2 = MBCS_FROM_UCHAR32_ISO2022(
                                converter_ਅੰਕੜੇ->myConverterArray[cs0],
                                ਸਰੋਤ_Char, &value,
                                useFallback, MBCS_OUTPUT_2);
                    ਜੇ(len2 == 2 || (len2 == -2 && len == 0)) {  /* only accept DBCS: abs(len)==2 */
                        value = _2022FromSJIS(value);
                        ਜੇ(value != 0) {
                            ਨਿਸ਼ਾਨਾ_ਮੁੱਲ = value;
                            len = len2;
                            cs = cs0;
                            g = 0;
                            useFallback = ਝੂਠ;
                        }
                    } ਨਹੀਂਤਾਂ ਜੇ(len == 0 && useFallback &&
                              static_cast<uint32_t>(ਸਰੋਤ_Char - HWKANA_ਸ਼ੁਰੂ) <= (HWKANA_ਸਿਰਾ - HWKANA_ਸ਼ੁਰੂ)) {
                        ਨਿਸ਼ਾਨਾ_ਮੁੱਲ = hwkana_fb[ਸਰੋਤ_Char - HWKANA_ਸ਼ੁਰੂ];
                        len = -2;
                        cs = cs0;
                        g = 0;
                        useFallback = ਝੂਠ;
                    }
                    ਰੋਕੋ;
                ਹਾਲਤ ISO8859_7:
                    /* G0 SBCS forced to 7-bit output */
                    len2 = MBCS_SINGLE_FROM_UCHAR32(
                                converter_ਅੰਕੜੇ->myConverterArray[cs0],
                                ਸਰੋਤ_Char, &value,
                                useFallback);
                    ਜੇ(len2 != 0 && !(len2 < 0 && len != 0) && GR96_ਸ਼ੁਰੂ <= value && value <= GR96_ਸਿਰਾ) {
                        ਨਿਸ਼ਾਨਾ_ਮੁੱਲ = value - 0x80;
                        len = len2;
                        cs = cs0;
                        g = 2;
                        useFallback = ਝੂਠ;
                    }
                    ਰੋਕੋ;
                ਮੂਲ:
                    /* G0 DBCS */
                    len2 = MBCS_FROM_UCHAR32_ISO2022(
                                converter_ਅੰਕੜੇ->myConverterArray[cs0],
                                ਸਰੋਤ_Char, &value,
                                useFallback, MBCS_OUTPUT_2);
                    ਜੇ(len2 == 2 || (len2 == -2 && len == 0)) {  /* only accept DBCS: abs(len)==2 */
                        ਜੇ(cs0 == KSC5601) {
                            /*
                             * Check for valid bytes for the encoding scheme.
                             * This is necessary because the sub-converter (windows-949)
                             * has a broader encoding scheme than is valid for 2022.
                             */
                            value = _2022FromGR94DBCS(value);
                            ਜੇ(value == 0) {
                                ਰੋਕੋ;
                            }
                        }
                        ਨਿਸ਼ਾਨਾ_ਮੁੱਲ = value;
                        len = len2;
                        cs = cs0;
                        g = 0;
                        useFallback = ਝੂਠ;
                    }
                    ਰੋਕੋ;
                }
            }

            ਜੇ(len != 0) {
                ਜੇ(len < 0) {
                    len = -len;  /* fallback */
                }
                ਬਾਹਰਲਾ_Len = 0; /* count output bytes */

                /* write SI if necessary (only for JIS7) */
                ਜੇ(pFromU2022State->g == 1 && g == 0) {
                    ਬਫਰ[ਬਾਹਰਲਾ_Len++] = UCNV_SI;
                    pFromU2022State->g = 0;
                }

                /* write the designation sequence if necessary */
                ਜੇ(cs != pFromU2022State->cs[g]) {
                    int32_t escLen = escSeqCharsLen[cs];
                    uprv_memcpy(ਬਫਰ + ਬਾਹਰਲਾ_Len, escSeqChars[cs], escLen);
                    ਬਾਹਰਲਾ_Len += escLen;
                    pFromU2022State->cs[g] = cs;

                    /* invalidate the choices[] */
                    choice_ਗਿਣਤੀ = 0;
                }

                /* write the shift sequence if necessary */
                ਜੇ(g != pFromU2022State->g) {
                    ਚੋਣ(g) {
                    /* case 0 handled before writing escapes */
                    ਹਾਲਤ 1:
                        ਬਫਰ[ਬਾਹਰਲਾ_Len++] = UCNV_SO;
                        pFromU2022State->g = 1;
                        ਰੋਕੋ;
                    ਮੂਲ: /* case 2 */
                        ਬਫਰ[ਬਾਹਰਲਾ_Len++] = 0x1b;
                        ਬਫਰ[ਬਾਹਰਲਾ_Len++] = 0x4e;
                        ਰੋਕੋ;
                    /* no case 3: no SS3 in ISO-2022-JP-x */
                    }
                }

                /* write the output bytes */
                ਜੇ(len == 1) {
                    ਬਫਰ[ਬਾਹਰਲਾ_Len++] = static_cast<ਅੱਖਰ>(ਨਿਸ਼ਾਨਾ_ਮੁੱਲ);
                } ਨਹੀਂਤਾਂ /* len == 2 */ {
                    ਬਫਰ[ਬਾਹਰਲਾ_Len++] = static_cast<ਅੱਖਰ>(ਨਿਸ਼ਾਨਾ_ਮੁੱਲ >> 8);
                    ਬਫਰ[ਬਾਹਰਲਾ_Len++] = static_cast<ਅੱਖਰ>(ਨਿਸ਼ਾਨਾ_ਮੁੱਲ);
                }
            } ਨਹੀਂਤਾਂ {
                /*
                 * if we cannot find the character after checking all codepages
                 * then this is an error
                 */
                *err = U_INVALID_CHAR_ਮਿਲਿਆ;
                cnv->fromUChar32=ਸਰੋਤ_Char;
                ਰੋਕੋ;
            }

            ਜੇ(ਸਰੋਤ_Char == CR || ਸਰੋਤ_Char == LF) {
                /* reset the G2 state at the end of a line (conversion got us into ASCII or JISX201 already) */
                pFromU2022State->cs[2] = 0;
                choice_ਗਿਣਤੀ = 0;
            }

            /* output outLen>0 bytes in buffer[] */
            ਜੇ(ਬਾਹਰਲਾ_Len == 1) {
                *ਨਿਸ਼ਾਨਾ++ = ਬਫਰ[0];
                ਜੇ(offsets) {
                    *offsets++ = static_cast<int32_t>(ਸਰੋਤ - ਦਲੀਲਾਂ_2->ਸਰੋਤ - 1); /* -1: known to be ASCII */
                }
            } ਨਹੀਂਤਾਂ ਜੇ(ਬਾਹਰਲਾ_Len == 2 && (ਨਿਸ਼ਾਨਾ + 2) <= ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ) {
                *ਨਿਸ਼ਾਨਾ++ = ਬਫਰ[0];
                *ਨਿਸ਼ਾਨਾ++ = ਬਫਰ[1];
                ਜੇ(offsets) {
                    int32_t ਸਰੋਤ_ਸੂਚਕਾਂਕ = static_cast<int32_t>(ਸਰੋਤ - ਦਲੀਲਾਂ_2->ਸਰੋਤ - U16_ਲੰਮਾਈ(ਸਰੋਤ_Char));
                    *offsets++ = ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                    *offsets++ = ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                }
            } ਨਹੀਂਤਾਂ {
                fromUWriteUInt8(
                    cnv,
                    ਬਫਰ, ਬਾਹਰਲਾ_Len,
                    &ਨਿਸ਼ਾਨਾ, reinterpret_cast<ਅਚਲ ਅੱਖਰ*>(ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ),
                    &offsets, static_cast<int32_t>(ਸਰੋਤ - ਦਲੀਲਾਂ_2->ਸਰੋਤ - U16_ਲੰਮਾਈ(ਸਰੋਤ_Char)),
                    err);
                ਜੇ(U_FAILURE(*err)) {
                    ਰੋਕੋ;
                }
            }
        } /* end if(myTargetIndex<myTargetLength) */
        ਨਹੀਂਤਾਂ{
            *err =U_ਬਫਰ_ਉਛਾਲ_ERROR;
            ਰੋਕੋ;
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
    ਜੇ( U_SUCCESS(*err) &&
        (pFromU2022State->g!=0 || pFromU2022State->cs[0]!=ASCII) &&
        ਦਲੀਲਾਂ_2->flush && ਸਰੋਤ>=ਸਰੋਤ_ਹੱਦਬੰਦੀ && cnv->fromUChar32==0
    ) {
        int32_t ਸਰੋਤ_ਸੂਚਕਾਂਕ;

        ਬਾਹਰਲਾ_Len = 0;

        ਜੇ(pFromU2022State->g != 0) {
            ਬਫਰ[ਬਾਹਰਲਾ_Len++] = UCNV_SI;
            pFromU2022State->g = 0;
        }

        ਜੇ(pFromU2022State->cs[0] != ASCII) {
            int32_t escLen = escSeqCharsLen[ASCII];
            uprv_memcpy(ਬਫਰ + ਬਾਹਰਲਾ_Len, escSeqChars[ASCII], escLen);
            ਬਾਹਰਲਾ_Len += escLen;
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
        ਸਰੋਤ_ਸੂਚਕਾਂਕ = static_cast<int32_t>(ਸਰੋਤ - ਦਲੀਲਾਂ_2->ਸਰੋਤ);
        ਜੇ(ਸਰੋਤ_ਸੂਚਕਾਂਕ>0) {
            --ਸਰੋਤ_ਸੂਚਕਾਂਕ;
            ਜੇ( U16_IS_TRAIL(ਦਲੀਲਾਂ_2->ਸਰੋਤ[ਸਰੋਤ_ਸੂਚਕਾਂਕ]) &&
                (ਸਰੋਤ_ਸੂਚਕਾਂਕ==0 || U16_IS_LEAD(ਦਲੀਲਾਂ_2->ਸਰੋਤ[ਸਰੋਤ_ਸੂਚਕਾਂਕ-1]))
            ) {
                --ਸਰੋਤ_ਸੂਚਕਾਂਕ;
            }
        } ਨਹੀਂਤਾਂ {
            ਸਰੋਤ_ਸੂਚਕਾਂਕ=-1;
        }

        fromUWriteUInt8(
            cnv,
            ਬਫਰ, ਬਾਹਰਲਾ_Len,
            &ਨਿਸ਼ਾਨਾ, reinterpret_cast<ਅਚਲ ਅੱਖਰ*>(ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ),
            &offsets, ਸਰੋਤ_ਸੂਚਕਾਂਕ,
            err);
    }

    /*save the state and return */
    ਦਲੀਲਾਂ_2->ਸਰੋਤ = ਸਰੋਤ;
    ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ = reinterpret_cast<ਅੱਖਰ*>(ਨਿਸ਼ਾਨਾ);
}

/*************** to unicode *******************/

ਸਥਿਰ ਖਾਲੀ U_CALLCONV
UConverter_ਮੰਜ਼ਿਲ_Unicode_ISO_2022_JP_OFFSETS_LOGIC(UConverter_ਮੰਜ਼ਿਲ_Unicode_ਦਲੀਲਾਂ *ਦਲੀਲਾਂ_2,
                                               UError_ਕੋਡ* err){
    ਅੱਖਰ ਆਰਜ਼ੀ_Buf[2];
    ਅਚਲ ਅੱਖਰ* my_ਸਰੋਤ = const_cast<ਅੱਖਰ*>(ਦਲੀਲਾਂ_2->ਸਰੋਤ);
    char16_t *my_ਨਿਸ਼ਾਨਾ = ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ;
    ਅਚਲ ਅੱਖਰ *my_ਸਰੋਤ_ਹੱਦਬੰਦੀ = ਦਲੀਲਾਂ_2->ਸਰੋਤ_ਹੱਦਬੰਦੀ;
    uint32_t ਨਿਸ਼ਾਨਾ_Uni_Char = 0x0000;
    uint32_t my_ਸਰੋਤ_Char = 0x0000;
    uint32_t tmp_ਸਰੋਤ_Char = 0x0000;
    UConverter_ਅੰਕੜੇ_ISO2022* my_ਅੰਕੜੇ;
    ISO2022State *p_ਮੰਜ਼ਿਲ_U2022_State;
    StateEnum cs;

    my_ਅੰਕੜੇ = static_cast<UConverter_ਅੰਕੜੇ_ISO2022*>(ਦਲੀਲਾਂ_2->converter->extraInfo);
    p_ਮੰਜ਼ਿਲ_U2022_State = &my_ਅੰਕੜੇ->ਮੰਜ਼ਿਲ_U2022_State;

    ਜੇ(my_ਅੰਕੜੇ->ਕੁੰਜੀ != 0) {
        /* continue with a partial escape sequence */
        ਜਾਓ escape;
    } ਨਹੀਂਤਾਂ ਜੇ(ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_ULength == 1 && my_ਸਰੋਤ < my_ਸਰੋਤ_ਹੱਦਬੰਦੀ && my_ਨਿਸ਼ਾਨਾ < ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ) {
        /* continue with a partial double-byte character */
        my_ਸਰੋਤ_Char = ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_UBytes[0];
        ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_ULength = 0;
        cs = static_cast<StateEnum>(p_ਮੰਜ਼ਿਲ_U2022_State->cs[p_ਮੰਜ਼ਿਲ_U2022_State->g]);
        ਨਿਸ਼ਾਨਾ_Uni_Char = missingCharMarker;
        ਜਾਓ getTrailByte;
    }

    ਜਦੋਂ(my_ਸਰੋਤ < my_ਸਰੋਤ_ਹੱਦਬੰਦੀ){

        ਨਿਸ਼ਾਨਾ_Uni_Char =missingCharMarker;

        ਜੇ(my_ਨਿਸ਼ਾਨਾ < ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ){

            my_ਸਰੋਤ_Char = static_cast<ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ>(*my_ਸਰੋਤ++);

            ਚੋਣ(my_ਸਰੋਤ_Char) {
            ਹਾਲਤ UCNV_SI:
                ਜੇ(my_ਅੰਕੜੇ->ਸੰਸਕਰਣ==3) {
                    p_ਮੰਜ਼ਿਲ_U2022_State->g=0;
                    ਜਾਰੀ;
                } ਨਹੀਂਤਾਂ {
                    /* only JIS7 uses SI/SO, not ISO-2022-JP-x */
                    my_ਅੰਕੜੇ->is_ਖਾਲੀ_ਥਾਵਾਂ_Segment = ਝੂਠ;	/* reset this, we have a different error */
                    ਰੋਕੋ;
                }

            ਹਾਲਤ UCNV_SO:
                ਜੇ(my_ਅੰਕੜੇ->ਸੰਸਕਰਣ==3) {
                    /* JIS7: switch to G1 half-width Katakana */
                    p_ਮੰਜ਼ਿਲ_U2022_State->cs[1] = static_cast<int8_t>(HWKANA_7_ਦੋਆਧਾਰੀ_ਅੰਕ);
                    p_ਮੰਜ਼ਿਲ_U2022_State->g=1;
                    ਜਾਰੀ;
                } ਨਹੀਂਤਾਂ {
                    /* only JIS7 uses SI/SO, not ISO-2022-JP-x */
                    my_ਅੰਕੜੇ->is_ਖਾਲੀ_ਥਾਵਾਂ_Segment = ਝੂਠ;	/* reset this, we have a different error */
                    ਰੋਕੋ;
                }

            ਹਾਲਤ ESC_2022:
                my_ਸਰੋਤ--;
escape:
                {
                    ਅਚਲ ਅੱਖਰ * my_ਸਰੋਤ_Before = my_ਸਰੋਤ;
                    int8_t ਮੰਜ਼ਿਲ_ULength_Before = ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_ULength;

                    changeState_2022(ਦਲੀਲਾਂ_2->converter,&(my_ਸਰੋਤ),
                        my_ਸਰੋਤ_ਹੱਦਬੰਦੀ, ISO_2022_JP,err);

                    /* If in ISO-2022-JP only and we successfully completed an escape sequence, but previous segment was empty, create an error */
                    ਜੇ(my_ਅੰਕੜੇ->ਸੰਸਕਰਣ==0 && my_ਅੰਕੜੇ->ਕੁੰਜੀ==0 && U_SUCCESS(*err) && my_ਅੰਕੜੇ->is_ਖਾਲੀ_ਥਾਵਾਂ_Segment) {
                        *err = U_ILLEGAL_ESCAPE_SEQUENCE;
                        ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_UCallback_Reason = UCNV_IRREGULAR;
                        ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_ULength = static_cast<int8_t>(ਮੰਜ਼ਿਲ_ULength_Before + (my_ਸਰੋਤ - my_ਸਰੋਤ_Before));
                    }
                }

                /* invalid or illegal escape sequence */
                ਜੇ(U_FAILURE(*err)){
                    ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ = my_ਨਿਸ਼ਾਨਾ;
                    ਦਲੀਲਾਂ_2->ਸਰੋਤ = my_ਸਰੋਤ;
                    my_ਅੰਕੜੇ->is_ਖਾਲੀ_ਥਾਵਾਂ_Segment = ਝੂਠ;	/* Reset to avoid future spurious errors */
                    ਵਾਪਸ;
                }
                /* If we successfully completed an escape sequence, we begin a new segment, empty so far */
                ਜੇ(my_ਅੰਕੜੇ->ਕੁੰਜੀ==0) {
                    my_ਅੰਕੜੇ->is_ਖਾਲੀ_ਥਾਵਾਂ_Segment = ਸੱਚ;
                }
                ਜਾਰੀ;

            /* ISO-2022-JP does not use single-byte (C1) SS2 and SS3 */

            ਹਾਲਤ CR:
            ਹਾਲਤ LF:
                /* automatically reset to single-byte mode */
                ਜੇ (static_cast<StateEnum>(p_ਮੰਜ਼ਿਲ_U2022_State->cs[0]) != ASCII &&
                    static_cast<StateEnum>(p_ਮੰਜ਼ਿਲ_U2022_State->cs[0]) != JISX201) {
                    p_ਮੰਜ਼ਿਲ_U2022_State->cs[0] = static_cast<int8_t>(ASCII);
                }
                p_ਮੰਜ਼ਿਲ_U2022_State->cs[2] = 0;
                p_ਮੰਜ਼ਿਲ_U2022_State->g = 0;
                U_FALLTHROUGH;
            ਮੂਲ:
                /* convert one or two bytes */
                my_ਅੰਕੜੇ->is_ਖਾਲੀ_ਥਾਵਾਂ_Segment = ਝੂਠ;
                cs = static_cast<StateEnum>(p_ਮੰਜ਼ਿਲ_U2022_State->cs[p_ਮੰਜ਼ਿਲ_U2022_State->g]);
                ਜੇ (static_cast<uint8_t>(my_ਸਰੋਤ_Char - 0xa1) <= (0xdf - 0xa1) && my_ਅੰਕੜੇ->ਸੰਸਕਰਣ == 4 &&
                    !IS_JP_DBCS(cs)
                ) {
                    /* 8-bit halfwidth katakana in any single-byte mode for JIS8 */
                    ਨਿਸ਼ਾਨਾ_Uni_Char = my_ਸਰੋਤ_Char + (HWKANA_ਸ਼ੁਰੂ - 0xa1);

                    /* return from a single-shift state to the previous one */
                    ਜੇ(p_ਮੰਜ਼ਿਲ_U2022_State->g >= 2) {
                        p_ਮੰਜ਼ਿਲ_U2022_State->g=p_ਮੰਜ਼ਿਲ_U2022_State->prevG;
                    }
                } ਨਹੀਂਤਾਂ ਚੋਣ(cs) {
                ਹਾਲਤ ASCII:
                    ਜੇ(my_ਸਰੋਤ_Char <= 0x7f) {
                        ਨਿਸ਼ਾਨਾ_Uni_Char = my_ਸਰੋਤ_Char;
                    }
                    ਰੋਕੋ;
                ਹਾਲਤ ISO8859_1:
                    ਜੇ(my_ਸਰੋਤ_Char <= 0x7f) {
                        ਨਿਸ਼ਾਨਾ_Uni_Char = my_ਸਰੋਤ_Char + 0x80;
                    }
                    /* return from a single-shift state to the previous one */
                    p_ਮੰਜ਼ਿਲ_U2022_State->g=p_ਮੰਜ਼ਿਲ_U2022_State->prevG;
                    ਰੋਕੋ;
                ਹਾਲਤ ISO8859_7:
                    ਜੇ(my_ਸਰੋਤ_Char <= 0x7f) {
                        /* convert mySourceChar+0x80 to use a normal 8-bit table */
                        ਨਿਸ਼ਾਨਾ_Uni_Char =
                            MBCS_SINGLE_SIMPLE_GET_ਅਗਲਾ_BMP(
                                my_ਅੰਕੜੇ->myConverterArray[cs],
                                my_ਸਰੋਤ_Char + 0x80);
                    }
                    /* return from a single-shift state to the previous one */
                    p_ਮੰਜ਼ਿਲ_U2022_State->g=p_ਮੰਜ਼ਿਲ_U2022_State->prevG;
                    ਰੋਕੋ;
                ਹਾਲਤ JISX201:
                    ਜੇ(my_ਸਰੋਤ_Char <= 0x7f) {
                        ਨਿਸ਼ਾਨਾ_Uni_Char = jisx201_ਮੰਜ਼ਿਲ_U(my_ਸਰੋਤ_Char);
                    }
                    ਰੋਕੋ;
                ਹਾਲਤ HWKANA_7_ਦੋਆਧਾਰੀ_ਅੰਕ:
                    ਜੇ (static_cast<uint8_t>(my_ਸਰੋਤ_Char - 0x21) <= (0x5f - 0x21)) {
                        /* 7-bit halfwidth Katakana */
                        ਨਿਸ਼ਾਨਾ_Uni_Char = my_ਸਰੋਤ_Char + (HWKANA_ਸ਼ੁਰੂ - 0x21);
                    }
                    ਰੋਕੋ;
                ਮੂਲ:
                    /* G0 DBCS */
                    ਜੇ(my_ਸਰੋਤ < my_ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
                        ਪੂਰਨ leadIsOk, trailIsOk;
                        uint8_t trailByte;
getTrailByte:
                        trailByte = static_cast<uint8_t>(*my_ਸਰੋਤ);
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
                        leadIsOk = static_cast<uint8_t>(my_ਸਰੋਤ_Char - 0x21) <= (0x7e - 0x21);
                        trailIsOk = static_cast<uint8_t>(trailByte - 0x21) <= (0x7e - 0x21);
                        ਜੇ (leadIsOk && trailIsOk) {
                            ++my_ਸਰੋਤ;
                            tmp_ਸਰੋਤ_Char = (my_ਸਰੋਤ_Char << 8) | trailByte;
                            ਜੇ(cs == JISX208) {
                                _2022ToSJIS(static_cast<uint8_t>(my_ਸਰੋਤ_Char), trailByte, ਆਰਜ਼ੀ_Buf);
                                my_ਸਰੋਤ_Char = tmp_ਸਰੋਤ_Char;
                            } ਨਹੀਂਤਾਂ {
                                /* Copy before we modify tmpSourceChar so toUnicodeCallback() sees the correct bytes. */
                                my_ਸਰੋਤ_Char = tmp_ਸਰੋਤ_Char;
                                ਜੇ (cs == KSC5601) {
                                    tmp_ਸਰੋਤ_Char += 0x8080;  /* = _2022ToGR94DBCS(tmpSourceChar) */
                                }
                                ਆਰਜ਼ੀ_Buf[0] = static_cast<ਅੱਖਰ>(tmp_ਸਰੋਤ_Char >> 8);
                                ਆਰਜ਼ੀ_Buf[1] = static_cast<ਅੱਖਰ>(tmp_ਸਰੋਤ_Char);
                            }
                            ਨਿਸ਼ਾਨਾ_Uni_Char = ucnv_MBCSSimple_Get_ਅਗਲਾ_UChar(my_ਅੰਕੜੇ->myConverterArray[cs], ਆਰਜ਼ੀ_Buf, 2, ਝੂਠ);
                        } ਨਹੀਂਤਾਂ ਜੇ (!(trailIsOk || IS_2022_CONTROL(trailByte))) {
                            /* report a pair of illegal bytes if the second byte is not a DBCS starter */
                            ++my_ਸਰੋਤ;
                            /* add another bit so that the code below writes 2 bytes in case of error */
                            my_ਸਰੋਤ_Char = 0x10000 | (my_ਸਰੋਤ_Char << 8) | trailByte;
                        }
                    } ਨਹੀਂਤਾਂ {
                        ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_UBytes[0] = static_cast<uint8_t>(my_ਸਰੋਤ_Char);
                        ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_ULength = 1;
                        ਜਾਓ endloop;
                    }
                }  /* End of inner switch */
                ਰੋਕੋ;
            }  /* End of outer switch */
            ਜੇ(ਨਿਸ਼ਾਨਾ_Uni_Char < (missingCharMarker-1/*0xfffe*/)){
                ਜੇ(ਦਲੀਲਾਂ_2->offsets){
                    ਦਲੀਲਾਂ_2->offsets[my_ਨਿਸ਼ਾਨਾ - ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ] = static_cast<int32_t>(my_ਸਰੋਤ - ਦਲੀਲਾਂ_2->ਸਰੋਤ - (my_ਸਰੋਤ_Char <= 0xff ? 1 : 2));
                }
                *(my_ਨਿਸ਼ਾਨਾ++) = static_cast<char16_t>(ਨਿਸ਼ਾਨਾ_Uni_Char);
            }
            ਨਹੀਂਤਾਂ ਜੇ(ਨਿਸ਼ਾਨਾ_Uni_Char > missingCharMarker){
                /* disassemble the surrogate pair and write to output*/
                ਨਿਸ਼ਾਨਾ_Uni_Char-=0x0010000;
                *my_ਨਿਸ਼ਾਨਾ = static_cast<char16_t>(0xd800 + static_cast<char16_t>(ਨਿਸ਼ਾਨਾ_Uni_Char >> 10));
                ਜੇ(ਦਲੀਲਾਂ_2->offsets){
                    ਦਲੀਲਾਂ_2->offsets[my_ਨਿਸ਼ਾਨਾ - ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ] = static_cast<int32_t>(my_ਸਰੋਤ - ਦਲੀਲਾਂ_2->ਸਰੋਤ - (my_ਸਰੋਤ_Char <= 0xff ? 1 : 2));
                }
                ++my_ਨਿਸ਼ਾਨਾ;
                ਜੇ(my_ਨਿਸ਼ਾਨਾ< ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ){
                    *my_ਨਿਸ਼ਾਨਾ = static_cast<char16_t>(0xdc00 + static_cast<char16_t>(ਨਿਸ਼ਾਨਾ_Uni_Char & 0x3ff));
                    ਜੇ(ਦਲੀਲਾਂ_2->offsets){
                        ਦਲੀਲਾਂ_2->offsets[my_ਨਿਸ਼ਾਨਾ - ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ] = static_cast<int32_t>(my_ਸਰੋਤ - ਦਲੀਲਾਂ_2->ਸਰੋਤ - (my_ਸਰੋਤ_Char <= 0xff ? 1 : 2));
                    }
                    ++my_ਨਿਸ਼ਾਨਾ;
                }ਨਹੀਂਤਾਂ{
                    ਦਲੀਲਾਂ_2->converter->UChar_Error_ਬਫਰ[ਦਲੀਲਾਂ_2->converter->UChar_Error_ਬਫਰ_ਲੰਮਾਈ++]=
                                    static_cast<char16_t>(0xdc00 + static_cast<char16_t>(ਨਿਸ਼ਾਨਾ_Uni_Char & 0x3ff));
                }

            }
            ਨਹੀਂਤਾਂ{
                /* Call the callback function*/
                ਮੰਜ਼ਿਲ_Unicode_Callback(ਦਲੀਲਾਂ_2->converter,my_ਸਰੋਤ_Char,ਨਿਸ਼ਾਨਾ_Uni_Char,err);
                ਰੋਕੋ;
            }
        }
        ਨਹੀਂਤਾਂ{    /* goes with "if(myTarget < args->targetLimit)"  way up near top of function */
            *err =U_ਬਫਰ_ਉਛਾਲ_ERROR;
            ਰੋਕੋ;
        }
    }
endloop:
    ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ = my_ਨਿਸ਼ਾਨਾ;
    ਦਲੀਲਾਂ_2->ਸਰੋਤ = my_ਸਰੋਤ;
}


#ਜੇ !UCONFIG_ONLY_HTML_CONVERSION
/***************************************************************
*   Rules for ISO-2022-KR encoding
*   i) The KSC5601 designator sequence should appear only once in a file,
*      at the beginning of a line before any KSC5601 characters. This usually
*      means that it appears by itself on the first line of the file
*  ii) There are only 2 shifting sequences SO to shift into double byte mode
*      and SI to shift into single byte mode
*/
ਸਥਿਰ ਖਾਲੀ U_CALLCONV
UConverter_fromUnicode_ISO_2022_KR_OFFSETS_LOGIC_IBM(UConverter_From_Unicode_ਦਲੀਲਾਂ* ਦਲੀਲਾਂ_2, UError_ਕੋਡ* err){

    UConverter* saveConv = ਦਲੀਲਾਂ_2->converter;
    UConverter_ਅੰਕੜੇ_ISO2022* my_Converter_ਅੰਕੜੇ = static_cast<UConverter_ਅੰਕੜੇ_ISO2022*>(saveConv->extraInfo);
    ਦਲੀਲਾਂ_2->converter=my_Converter_ਅੰਕੜੇ->currentConverter;

    my_Converter_ਅੰਕੜੇ->currentConverter->fromUChar32 = saveConv->fromUChar32;
    ucnv_MBCSFromUnicodeWithOffsets(ਦਲੀਲਾਂ_2,err);
    saveConv->fromUChar32 = my_Converter_ਅੰਕੜੇ->currentConverter->fromUChar32;

    ਜੇ(*err == U_ਬਫਰ_ਉਛਾਲ_ERROR) {
        ਜੇ(my_Converter_ਅੰਕੜੇ->currentConverter->char_Error_ਬਫਰ_ਲੰਮਾਈ > 0) {
            uprv_memcpy(
                saveConv->char_Error_ਬਫਰ,
                my_Converter_ਅੰਕੜੇ->currentConverter->char_Error_ਬਫਰ,
                my_Converter_ਅੰਕੜੇ->currentConverter->char_Error_ਬਫਰ_ਲੰਮਾਈ);
        }
        saveConv->char_Error_ਬਫਰ_ਲੰਮਾਈ = my_Converter_ਅੰਕੜੇ->currentConverter->char_Error_ਬਫਰ_ਲੰਮਾਈ;
        my_Converter_ਅੰਕੜੇ->currentConverter->char_Error_ਬਫਰ_ਲੰਮਾਈ = 0;
    }
    ਦਲੀਲਾਂ_2->converter=saveConv;
}

ਸਥਿਰ ਖਾਲੀ U_CALLCONV
UConverter_fromUnicode_ISO_2022_KR_OFFSETS_LOGIC(UConverter_From_Unicode_ਦਲੀਲਾਂ* ਦਲੀਲਾਂ_2, UError_ਕੋਡ* err){

    ਅਚਲ char16_t *ਸਰੋਤ = ਦਲੀਲਾਂ_2->ਸਰੋਤ;
    ਅਚਲ char16_t *ਸਰੋਤ_ਹੱਦਬੰਦੀ = ਦਲੀਲਾਂ_2->ਸਰੋਤ_ਹੱਦਬੰਦੀ;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਨਿਸ਼ਾਨਾ = reinterpret_cast<ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ*>(ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ);
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ = reinterpret_cast<ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ*>(const_cast<ਅੱਖਰ*>(ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ));
    int32_t* offsets = ਦਲੀਲਾਂ_2->offsets;
    uint32_t ਨਿਸ਼ਾਨਾ_Byte_Unit = 0x0000;
    UChar32 ਸਰੋਤ_Char = 0x0000;
    UBool is_ਨਿਸ਼ਾਨਾ_Byte_DBCS;
    UBool old_Is_ਨਿਸ਼ਾਨਾ_Byte_DBCS;
    UConverter_ਅੰਕੜੇ_ISO2022 *converter_ਅੰਕੜੇ;
    UConverter_Shared_ਅੰਕੜੇ* shared_ਅੰਕੜੇ;
    UBool useFallback;
    int32_t length =0;

    converter_ਅੰਕੜੇ = static_cast<UConverter_ਅੰਕੜੇ_ISO2022*>(ਦਲੀਲਾਂ_2->converter->extraInfo);
    /* if the version is 1 then the user is requesting
     * conversion with ibm-25546 pass the arguments to
     * MBCS converter and return
     */
    ਜੇ(converter_ਅੰਕੜੇ->ਸੰਸਕਰਣ==1){
        UConverter_fromUnicode_ISO_2022_KR_OFFSETS_LOGIC_IBM(ਦਲੀਲਾਂ_2,err);
        ਵਾਪਸ;
    }

    /* initialize data */
    shared_ਅੰਕੜੇ = converter_ਅੰਕੜੇ->currentConverter->shared_ਅੰਕੜੇ;
    useFallback = ਦਲੀਲਾਂ_2->converter->useFallback;
    is_ਨਿਸ਼ਾਨਾ_Byte_DBCS = static_cast<UBool>(ਦਲੀਲਾਂ_2->converter->from_Unicode_ਸਥਿਤੀ);
    old_Is_ਨਿਸ਼ਾਨਾ_Byte_DBCS = is_ਨਿਸ਼ਾਨਾ_Byte_DBCS;

    is_ਨਿਸ਼ਾਨਾ_Byte_DBCS = static_cast<UBool>(ਦਲੀਲਾਂ_2->converter->from_Unicode_ਸਥਿਤੀ);
    ਜੇ((ਸਰੋਤ_Char = ਦਲੀਲਾਂ_2->converter->fromUChar32)!=0 && ਨਿਸ਼ਾਨਾ <ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ) {
        ਜਾਓ getTrail;
    }
    ਜਦੋਂ(ਸਰੋਤ < ਸਰੋਤ_ਹੱਦਬੰਦੀ){

        ਨਿਸ਼ਾਨਾ_Byte_Unit = missingCharMarker;

        ਜੇ(ਨਿਸ਼ਾਨਾ < (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ*) ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ){
            ਸਰੋਤ_Char = *ਸਰੋਤ++;

            /* do not convert SO/SI/ESC */
            ਜੇ(IS_2022_CONTROL(ਸਰੋਤ_Char)) {
                /* callback(illegal) */
                *err=U_ILLEGAL_CHAR_ਮਿਲਿਆ;
                ਦਲੀਲਾਂ_2->converter->fromUChar32=ਸਰੋਤ_Char;
                ਰੋਕੋ;
            }

            length = MBCS_FROM_UCHAR32_ISO2022(shared_ਅੰਕੜੇ,ਸਰੋਤ_Char,&ਨਿਸ਼ਾਨਾ_Byte_Unit,useFallback,MBCS_OUTPUT_2);
            ਜੇ(length < 0) {
                length = -length;  /* fallback */
            }
            /* only DBCS or SBCS characters are expected*/
            /* DB characters with high bit set to 1 are expected */
            ਜੇ( length > 2 || length==0 ||
                (length == 1 && ਨਿਸ਼ਾਨਾ_Byte_Unit > 0x7f) ||
                (length == 2 &&
                    (static_cast<uint16_t>(ਨਿਸ਼ਾਨਾ_Byte_Unit - 0xa1a1) > (0xfefe - 0xa1a1) ||
                    static_cast<uint8_t>(ਨਿਸ਼ਾਨਾ_Byte_Unit - 0xa1) > (0xfe - 0xa1)))
            ) {
                ਨਿਸ਼ਾਨਾ_Byte_Unit=missingCharMarker;
            }
            ਜੇ (ਨਿਸ਼ਾਨਾ_Byte_Unit != missingCharMarker){

                old_Is_ਨਿਸ਼ਾਨਾ_Byte_DBCS = is_ਨਿਸ਼ਾਨਾ_Byte_DBCS;
                is_ਨਿਸ਼ਾਨਾ_Byte_DBCS = static_cast<UBool>(ਨਿਸ਼ਾਨਾ_Byte_Unit > 0x00FF);
                  /* append the shift sequence */
                ਜੇ (old_Is_ਨਿਸ਼ਾਨਾ_Byte_DBCS != is_ਨਿਸ਼ਾਨਾ_Byte_DBCS ){

                    ਜੇ (is_ਨਿਸ਼ਾਨਾ_Byte_DBCS)
                        *ਨਿਸ਼ਾਨਾ++ = UCNV_SO;
                    ਨਹੀਂਤਾਂ
                        *ਨਿਸ਼ਾਨਾ++ = UCNV_SI;
                    ਜੇ(offsets)
                        *(offsets++) = static_cast<int32_t>(ਸਰੋਤ - ਦਲੀਲਾਂ_2->ਸਰੋਤ - 1);
                }
                /* write the targetUniChar  to target */
                ਜੇ(ਨਿਸ਼ਾਨਾ_Byte_Unit <= 0x00FF){
                    ਜੇ( ਨਿਸ਼ਾਨਾ < ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ){
                        *(ਨਿਸ਼ਾਨਾ++) = static_cast<ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ>(ਨਿਸ਼ਾਨਾ_Byte_Unit);
                        ਜੇ(offsets){
                            *(offsets++) = static_cast<int32_t>(ਸਰੋਤ - ਦਲੀਲਾਂ_2->ਸਰੋਤ - 1);
                        }

                    }ਨਹੀਂਤਾਂ{
                        ਦਲੀਲਾਂ_2->converter->char_Error_ਬਫਰ[ਦਲੀਲਾਂ_2->converter->char_Error_ਬਫਰ_ਲੰਮਾਈ++] = static_cast<ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ>(ਨਿਸ਼ਾਨਾ_Byte_Unit);
                        *err = U_ਬਫਰ_ਉਛਾਲ_ERROR;
                    }
                }ਨਹੀਂਤਾਂ{
                    ਜੇ(ਨਿਸ਼ਾਨਾ < ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ){
                        *(ਨਿਸ਼ਾਨਾ++) = static_cast<ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ>((ਨਿਸ਼ਾਨਾ_Byte_Unit >> 8) - 0x80);
                        ਜੇ(offsets){
                            *(offsets++) = static_cast<int32_t>(ਸਰੋਤ - ਦਲੀਲਾਂ_2->ਸਰੋਤ - 1);
                        }
                        ਜੇ(ਨਿਸ਼ਾਨਾ < ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ){
                            *(ਨਿਸ਼ਾਨਾ++) = static_cast<ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ>(ਨਿਸ਼ਾਨਾ_Byte_Unit - 0x80);
                            ਜੇ(offsets){
                                *(offsets++) = static_cast<int32_t>(ਸਰੋਤ - ਦਲੀਲਾਂ_2->ਸਰੋਤ - 1);
                            }
                        }ਨਹੀਂਤਾਂ{
                            ਦਲੀਲਾਂ_2->converter->char_Error_ਬਫਰ[ਦਲੀਲਾਂ_2->converter->char_Error_ਬਫਰ_ਲੰਮਾਈ++] = static_cast<ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ>(ਨਿਸ਼ਾਨਾ_Byte_Unit - 0x80);
                            *err = U_ਬਫਰ_ਉਛਾਲ_ERROR;
                        }
                    }ਨਹੀਂਤਾਂ{
                        ਦਲੀਲਾਂ_2->converter->char_Error_ਬਫਰ[ਦਲੀਲਾਂ_2->converter->char_Error_ਬਫਰ_ਲੰਮਾਈ++] = static_cast<ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ>((ਨਿਸ਼ਾਨਾ_Byte_Unit >> 8) - 0x80);
                        ਦਲੀਲਾਂ_2->converter->char_Error_ਬਫਰ[ਦਲੀਲਾਂ_2->converter->char_Error_ਬਫਰ_ਲੰਮਾਈ++] = static_cast<ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ>(ਨਿਸ਼ਾਨਾ_Byte_Unit - 0x80);
                        *err = U_ਬਫਰ_ਉਛਾਲ_ERROR;
                    }
                }

            }
            ਨਹੀਂਤਾਂ{
                /* oops.. the code point is unassingned
                 * set the error and reason
                 */

                /*check if the char is a First surrogate*/
                ਜੇ(U16_IS_SURROGATE(ਸਰੋਤ_Char)) {
                    ਜੇ(U16_IS_SURROGATE_LEAD(ਸਰੋਤ_Char)) {
getTrail:
                        /*look ahead to find the trail surrogate*/
                        ਜੇ(ਸਰੋਤ <  ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
                            /* test the following code unit */
                            char16_t trail = *ਸਰੋਤ;
                            ਜੇ(U16_IS_TRAIL(trail)) {
                                ਸਰੋਤ++;
                                ਸਰੋਤ_Char=U16_GET_SUPPLEMENTARY(ਸਰੋਤ_Char, trail);
                                *err = U_INVALID_CHAR_ਮਿਲਿਆ;
                                /* convert this surrogate code point */
                                /* exit this condition tree */
                            } ਨਹੀਂਤਾਂ {
                                /* this is an unmatched lead code unit (1st surrogate) */
                                /* callback(illegal) */
                                *err=U_ILLEGAL_CHAR_ਮਿਲਿਆ;
                            }
                        } ਨਹੀਂਤਾਂ {
                            /* no more input */
                            *err = U_ZERO_ERROR;
                        }
                    } ਨਹੀਂਤਾਂ {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *err=U_ILLEGAL_CHAR_ਮਿਲਿਆ;
                    }
                } ਨਹੀਂਤਾਂ {
                    /* callback(unassigned) for a BMP code point */
                    *err = U_INVALID_CHAR_ਮਿਲਿਆ;
                }

                ਦਲੀਲਾਂ_2->converter->fromUChar32=ਸਰੋਤ_Char;
                ਰੋਕੋ;
            }
        } /* end if(myTargetIndex<myTargetLength) */
        ਨਹੀਂਤਾਂ{
            *err =U_ਬਫਰ_ਉਛਾਲ_ERROR;
            ਰੋਕੋ;
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
    ਜੇ( U_SUCCESS(*err) &&
        is_ਨਿਸ਼ਾਨਾ_Byte_DBCS &&
        ਦਲੀਲਾਂ_2->flush && ਸਰੋਤ>=ਸਰੋਤ_ਹੱਦਬੰਦੀ && ਦਲੀਲਾਂ_2->converter->fromUChar32==0
    ) {
        int32_t ਸਰੋਤ_ਸੂਚਕਾਂਕ;

        /* we are switching to ASCII */
        is_ਨਿਸ਼ਾਨਾ_Byte_DBCS=ਝੂਠ;

        /* get the source index of the last input character */
        /*
         * TODO this would be simpler and more reliable if we used a pair
         * of sourceIndex/prevSourceIndex like in ucnvmbcs.c
         * so that we could simply use the prevSourceIndex here;
         * this code gives an incorrect result for the rare case of an unmatched
         * trail surrogate that is alone in the last buffer of the text stream
         */
        ਸਰੋਤ_ਸੂਚਕਾਂਕ = static_cast<int32_t>(ਸਰੋਤ - ਦਲੀਲਾਂ_2->ਸਰੋਤ);
        ਜੇ(ਸਰੋਤ_ਸੂਚਕਾਂਕ>0) {
            --ਸਰੋਤ_ਸੂਚਕਾਂਕ;
            ਜੇ( U16_IS_TRAIL(ਦਲੀਲਾਂ_2->ਸਰੋਤ[ਸਰੋਤ_ਸੂਚਕਾਂਕ]) &&
                (ਸਰੋਤ_ਸੂਚਕਾਂਕ==0 || U16_IS_LEAD(ਦਲੀਲਾਂ_2->ਸਰੋਤ[ਸਰੋਤ_ਸੂਚਕਾਂਕ-1]))
            ) {
                --ਸਰੋਤ_ਸੂਚਕਾਂਕ;
            }
        } ਨਹੀਂਤਾਂ {
            ਸਰੋਤ_ਸੂਚਕਾਂਕ=-1;
        }

        fromUWriteUInt8(
            ਦਲੀਲਾਂ_2->converter,
            ਖਿਸਕਾਓ_ਵਿੱਚ_STR, 1,
            &ਨਿਸ਼ਾਨਾ, reinterpret_cast<ਅਚਲ ਅੱਖਰ*>(ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ),
            &offsets, ਸਰੋਤ_ਸੂਚਕਾਂਕ,
            err);
    }

    /*save the state and return */
    ਦਲੀਲਾਂ_2->ਸਰੋਤ = ਸਰੋਤ;
    ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ = reinterpret_cast<ਅੱਖਰ*>(ਨਿਸ਼ਾਨਾ);
    ਦਲੀਲਾਂ_2->converter->from_Unicode_ਸਥਿਤੀ = static_cast<uint32_t>(is_ਨਿਸ਼ਾਨਾ_Byte_DBCS);
}

/************************ To Unicode ***************************************/

ਸਥਿਰ ਖਾਲੀ U_CALLCONV
UConverter_ਮੰਜ਼ਿਲ_Unicode_ISO_2022_KR_OFFSETS_LOGIC_IBM(UConverter_ਮੰਜ਼ਿਲ_Unicode_ਦਲੀਲਾਂ *ਦਲੀਲਾਂ_2,
                                                            UError_ਕੋਡ* err){
    ਅੱਖਰ ਅਚਲ* ਸਰੋਤ_ਸ਼ੁਰੂ;
    UConverter_ਅੰਕੜੇ_ISO2022* my_ਅੰਕੜੇ = static_cast<UConverter_ਅੰਕੜੇ_ISO2022*>(ਦਲੀਲਾਂ_2->converter->extraInfo);

    UConverter_ਮੰਜ਼ਿਲ_Unicode_ਦਲੀਲਾਂ sub_ਦਲੀਲਾਂ;
    int32_t min_ਦਲੀਲਾਂ_ਮਾਪ;

    /* set up the subconverter arguments */
    ਜੇ(ਦਲੀਲਾਂ_2->size<ਆਕਾਰ(UConverter_ਮੰਜ਼ਿਲ_Unicode_ਦਲੀਲਾਂ)) {
        min_ਦਲੀਲਾਂ_ਮਾਪ = ਦਲੀਲਾਂ_2->size;
    } ਨਹੀਂਤਾਂ {
        min_ਦਲੀਲਾਂ_ਮਾਪ = static_cast<int32_t>(ਆਕਾਰ(UConverter_ਮੰਜ਼ਿਲ_Unicode_ਦਲੀਲਾਂ));
    }

    uprv_memcpy(&sub_ਦਲੀਲਾਂ, ਦਲੀਲਾਂ_2, min_ਦਲੀਲਾਂ_ਮਾਪ);
    sub_ਦਲੀਲਾਂ.size = static_cast<uint16_t>(min_ਦਲੀਲਾਂ_ਮਾਪ);
    sub_ਦਲੀਲਾਂ.converter = my_ਅੰਕੜੇ->currentConverter;

    /* remember the original start of the input for offsets */
    ਸਰੋਤ_ਸ਼ੁਰੂ = ਦਲੀਲਾਂ_2->ਸਰੋਤ;

    ਜੇ(my_ਅੰਕੜੇ->ਕੁੰਜੀ != 0) {
        /* continue with a partial escape sequence */
        ਜਾਓ escape;
    }

    ਜਦੋਂ(U_SUCCESS(*err) && ਦਲੀਲਾਂ_2->ਸਰੋਤ < ਦਲੀਲਾਂ_2->ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
        /*Find the end of the buffer e.g : Next Escape Seq | end of Buffer*/
        sub_ਦਲੀਲਾਂ.ਸਰੋਤ = ਦਲੀਲਾਂ_2->ਸਰੋਤ;
        sub_ਦਲੀਲਾਂ.ਸਰੋਤ_ਹੱਦਬੰਦੀ = get_ਸਿਰਾ_ਦਾ_ਬਫਰ_2022(&(ਦਲੀਲਾਂ_2->ਸਰੋਤ), ਦਲੀਲਾਂ_2->ਸਰੋਤ_ਹੱਦਬੰਦੀ, ਦਲੀਲਾਂ_2->flush);
        ਜੇ(sub_ਦਲੀਲਾਂ.ਸਰੋਤ != sub_ਦਲੀਲਾਂ.ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
            /*
             * get the current partial byte sequence
             *
             * it needs to be moved between the public and the subconverter
             * so that the conversion framework, which only sees the public
             * converter, can handle truncated and illegal input etc.
             */
            ਜੇ(ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_ULength > 0) {
                uprv_memcpy(sub_ਦਲੀਲਾਂ.converter->ਮੰਜ਼ਿਲ_UBytes, ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_UBytes, ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_ULength);
            }
            sub_ਦਲੀਲਾਂ.converter->ਮੰਜ਼ਿਲ_ULength = ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_ULength;

            /*
             * Convert up to the end of the input, or to before the next escape character.
             * Does not handle conversion extensions because the preToU[] state etc.
             * is not copied.
             */
            ucnv_MBCSToUnicodeWithOffsets(&sub_ਦਲੀਲਾਂ, err);

            ਜੇ(ਦਲੀਲਾਂ_2->offsets != ਖਾਲੀ_ਸੂਚਕ && ਸਰੋਤ_ਸ਼ੁਰੂ != ਦਲੀਲਾਂ_2->ਸਰੋਤ) {
                /* update offsets to base them on the actual start of the input */
                int32_t *offsets = ਦਲੀਲਾਂ_2->offsets;
                char16_t *ਨਿਸ਼ਾਨਾ = ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ;
                int32_t delta = static_cast<int32_t>(ਦਲੀਲਾਂ_2->ਸਰੋਤ - ਸਰੋਤ_ਸ਼ੁਰੂ);
                ਜਦੋਂ(ਨਿਸ਼ਾਨਾ < sub_ਦਲੀਲਾਂ.ਨਿਸ਼ਾਨਾ) {
                    ਜੇ(*offsets >= 0) {
                        *offsets += delta;
                    }
                    ++offsets;
                    ++ਨਿਸ਼ਾਨਾ;
                }
            }
            ਦਲੀਲਾਂ_2->ਸਰੋਤ = sub_ਦਲੀਲਾਂ.ਸਰੋਤ;
            ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ = sub_ਦਲੀਲਾਂ.ਨਿਸ਼ਾਨਾ;
            ਦਲੀਲਾਂ_2->offsets = sub_ਦਲੀਲਾਂ.offsets;

            /* copy input/error/overflow buffers */
            ਜੇ(sub_ਦਲੀਲਾਂ.converter->ਮੰਜ਼ਿਲ_ULength > 0) {
                uprv_memcpy(ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_UBytes, sub_ਦਲੀਲਾਂ.converter->ਮੰਜ਼ਿਲ_UBytes, sub_ਦਲੀਲਾਂ.converter->ਮੰਜ਼ਿਲ_ULength);
            }
            ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_ULength = sub_ਦਲੀਲਾਂ.converter->ਮੰਜ਼ਿਲ_ULength;

            ਜੇ(*err == U_ਬਫਰ_ਉਛਾਲ_ERROR) {
                ਜੇ(sub_ਦਲੀਲਾਂ.converter->UChar_Error_ਬਫਰ_ਲੰਮਾਈ > 0) {
                    uprv_memcpy(ਦਲੀਲਾਂ_2->converter->UChar_Error_ਬਫਰ, sub_ਦਲੀਲਾਂ.converter->UChar_Error_ਬਫਰ,
                                sub_ਦਲੀਲਾਂ.converter->UChar_Error_ਬਫਰ_ਲੰਮਾਈ);
                }
                ਦਲੀਲਾਂ_2->converter->UChar_Error_ਬਫਰ_ਲੰਮਾਈ=sub_ਦਲੀਲਾਂ.converter->UChar_Error_ਬਫਰ_ਲੰਮਾਈ;
                sub_ਦਲੀਲਾਂ.converter->UChar_Error_ਬਫਰ_ਲੰਮਾਈ = 0;
            }
        }

        ਜੇ (U_FAILURE(*err) || (ਦਲੀਲਾਂ_2->ਸਰੋਤ == ਦਲੀਲਾਂ_2->ਸਰੋਤ_ਹੱਦਬੰਦੀ)) {
            ਵਾਪਸ;
        }

escape:
        changeState_2022(ਦਲੀਲਾਂ_2->converter,
               &(ਦਲੀਲਾਂ_2->ਸਰੋਤ),
               ਦਲੀਲਾਂ_2->ਸਰੋਤ_ਹੱਦਬੰਦੀ,
               ISO_2022_KR,
               err);
    }
}

ਸਥਿਰ ਖਾਲੀ U_CALLCONV
UConverter_ਮੰਜ਼ਿਲ_Unicode_ISO_2022_KR_OFFSETS_LOGIC(UConverter_ਮੰਜ਼ਿਲ_Unicode_ਦਲੀਲਾਂ *ਦਲੀਲਾਂ_2,
                                                            UError_ਕੋਡ* err){
    ਅੱਖਰ ਆਰਜ਼ੀ_Buf[2];
    ਅਚਲ ਅੱਖਰ* my_ਸਰੋਤ = const_cast<ਅੱਖਰ*>(ਦਲੀਲਾਂ_2->ਸਰੋਤ);
    char16_t *my_ਨਿਸ਼ਾਨਾ = ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ;
    ਅਚਲ ਅੱਖਰ *my_ਸਰੋਤ_ਹੱਦਬੰਦੀ = ਦਲੀਲਾਂ_2->ਸਰੋਤ_ਹੱਦਬੰਦੀ;
    UChar32 ਨਿਸ਼ਾਨਾ_Uni_Char = 0x0000;
    char16_t my_ਸਰੋਤ_Char = 0x0000;
    UConverter_ਅੰਕੜੇ_ISO2022* my_ਅੰਕੜੇ;
    UConverter_Shared_ਅੰਕੜੇ* shared_ਅੰਕੜੇ ;
    UBool useFallback;

    my_ਅੰਕੜੇ = static_cast<UConverter_ਅੰਕੜੇ_ISO2022*>(ਦਲੀਲਾਂ_2->converter->extraInfo);
    ਜੇ(my_ਅੰਕੜੇ->ਸੰਸਕਰਣ==1){
        UConverter_ਮੰਜ਼ਿਲ_Unicode_ISO_2022_KR_OFFSETS_LOGIC_IBM(ਦਲੀਲਾਂ_2,err);
        ਵਾਪਸ;
    }

    /* initialize state */
    shared_ਅੰਕੜੇ = my_ਅੰਕੜੇ->currentConverter->shared_ਅੰਕੜੇ;
    useFallback = ਦਲੀਲਾਂ_2->converter->useFallback;

    ਜੇ(my_ਅੰਕੜੇ->ਕੁੰਜੀ != 0) {
        /* continue with a partial escape sequence */
        ਜਾਓ escape;
    } ਨਹੀਂਤਾਂ ਜੇ(ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_ULength == 1 && my_ਸਰੋਤ < my_ਸਰੋਤ_ਹੱਦਬੰਦੀ && my_ਨਿਸ਼ਾਨਾ < ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ) {
        /* continue with a partial double-byte character */
        my_ਸਰੋਤ_Char = ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_UBytes[0];
        ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_ULength = 0;
        ਜਾਓ getTrailByte;
    }

    ਜਦੋਂ(my_ਸਰੋਤ< my_ਸਰੋਤ_ਹੱਦਬੰਦੀ){

        ਜੇ(my_ਨਿਸ਼ਾਨਾ < ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ){

            my_ਸਰੋਤ_Char = static_cast<ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ>(*my_ਸਰੋਤ++);

            ਜੇ(my_ਸਰੋਤ_Char==UCNV_SI){
                my_ਅੰਕੜੇ->ਮੰਜ਼ਿਲ_U2022_State.g = 0;
                ਜੇ (my_ਅੰਕੜੇ->is_ਖਾਲੀ_ਥਾਵਾਂ_Segment) {
                    my_ਅੰਕੜੇ->is_ਖਾਲੀ_ਥਾਵਾਂ_Segment = ਝੂਠ;	/* we are handling it, reset to avoid future spurious errors */
                    *err = U_ILLEGAL_ESCAPE_SEQUENCE;
                    ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_UCallback_Reason = UCNV_IRREGULAR;
                    ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_UBytes[0] = static_cast<uint8_t>(my_ਸਰੋਤ_Char);
                    ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_ULength = 1;
                    ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ = my_ਨਿਸ਼ਾਨਾ;
                    ਦਲੀਲਾਂ_2->ਸਰੋਤ = my_ਸਰੋਤ;
                    ਵਾਪਸ;
                }
                /*consume the source */
                ਜਾਰੀ;
            }ਨਹੀਂਤਾਂ ਜੇ(my_ਸਰੋਤ_Char==UCNV_SO){
                my_ਅੰਕੜੇ->ਮੰਜ਼ਿਲ_U2022_State.g = 1;
                my_ਅੰਕੜੇ->is_ਖਾਲੀ_ਥਾਵਾਂ_Segment = ਸੱਚ;	/* Begin a new segment, empty so far */
                /*consume the source */
                ਜਾਰੀ;
            }ਨਹੀਂਤਾਂ ਜੇ(my_ਸਰੋਤ_Char==ESC_2022){
                my_ਸਰੋਤ--;
escape:
                my_ਅੰਕੜੇ->is_ਖਾਲੀ_ਥਾਵਾਂ_Segment = ਝੂਠ;	/* Any invalid ESC sequences will be detected separately, so just reset this */
                changeState_2022(ਦਲੀਲਾਂ_2->converter,&(my_ਸਰੋਤ),
                                my_ਸਰੋਤ_ਹੱਦਬੰਦੀ, ISO_2022_KR, err);
                ਜੇ(U_FAILURE(*err)){
                    ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ = my_ਨਿਸ਼ਾਨਾ;
                    ਦਲੀਲਾਂ_2->ਸਰੋਤ = my_ਸਰੋਤ;
                    ਵਾਪਸ;
                }
                ਜਾਰੀ;
            }

            my_ਅੰਕੜੇ->is_ਖਾਲੀ_ਥਾਵਾਂ_Segment = ਝੂਠ;	/* Any invalid char errors will be detected separately, so just reset this */
            ਜੇ(my_ਅੰਕੜੇ->ਮੰਜ਼ਿਲ_U2022_State.g == 1) {
                ਜੇ(my_ਸਰੋਤ < my_ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
                    ਪੂਰਨ leadIsOk, trailIsOk;
                    uint8_t trailByte;
getTrailByte:
                    ਨਿਸ਼ਾਨਾ_Uni_Char = missingCharMarker;
                    trailByte = static_cast<uint8_t>(*my_ਸਰੋਤ);
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
                    leadIsOk = static_cast<uint8_t>(my_ਸਰੋਤ_Char - 0x21) <= (0x7e - 0x21);
                    trailIsOk = static_cast<uint8_t>(trailByte - 0x21) <= (0x7e - 0x21);
                    ਜੇ (leadIsOk && trailIsOk) {
                        ++my_ਸਰੋਤ;
                        ਆਰਜ਼ੀ_Buf[0] = static_cast<ਅੱਖਰ>(my_ਸਰੋਤ_Char + 0x80);
                        ਆਰਜ਼ੀ_Buf[1] = static_cast<ਅੱਖਰ>(trailByte + 0x80);
                        ਨਿਸ਼ਾਨਾ_Uni_Char = ucnv_MBCSSimple_Get_ਅਗਲਾ_UChar(shared_ਅੰਕੜੇ, ਆਰਜ਼ੀ_Buf, 2, useFallback);
                        my_ਸਰੋਤ_Char = (my_ਸਰੋਤ_Char << 8) | trailByte;
                    } ਨਹੀਂਤਾਂ ਜੇ (!(trailIsOk || IS_2022_CONTROL(trailByte))) {
                        /* report a pair of illegal bytes if the second byte is not a DBCS starter */
                        ++my_ਸਰੋਤ;
                        /* add another bit so that the code below writes 2 bytes in case of error */
                        my_ਸਰੋਤ_Char = static_cast<char16_t>(0x10000 | (my_ਸਰੋਤ_Char << 8) | trailByte);
                    }
                } ਨਹੀਂਤਾਂ {
                    ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_UBytes[0] = static_cast<uint8_t>(my_ਸਰੋਤ_Char);
                    ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_ULength = 1;
                    ਰੋਕੋ;
                }
            }
            ਨਹੀਂਤਾਂ ਜੇ(my_ਸਰੋਤ_Char <= 0x7f) {
                ਨਿਸ਼ਾਨਾ_Uni_Char = ucnv_MBCSSimple_Get_ਅਗਲਾ_UChar(shared_ਅੰਕੜੇ, my_ਸਰੋਤ - 1, 1, useFallback);
            } ਨਹੀਂਤਾਂ {
                ਨਿਸ਼ਾਨਾ_Uni_Char = 0xffff;
            }
            ਜੇ(ਨਿਸ਼ਾਨਾ_Uni_Char < 0xfffe){
                ਜੇ(ਦਲੀਲਾਂ_2->offsets) {
                    ਦਲੀਲਾਂ_2->offsets[my_ਨਿਸ਼ਾਨਾ - ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ] = static_cast<int32_t>(my_ਸਰੋਤ - ਦਲੀਲਾਂ_2->ਸਰੋਤ - (my_ਸਰੋਤ_Char <= 0xff ? 1 : 2));
                }
                *(my_ਨਿਸ਼ਾਨਾ++) = static_cast<char16_t>(ਨਿਸ਼ਾਨਾ_Uni_Char);
            }
            ਨਹੀਂਤਾਂ {
                /* Call the callback function*/
                ਮੰਜ਼ਿਲ_Unicode_Callback(ਦਲੀਲਾਂ_2->converter,my_ਸਰੋਤ_Char,ਨਿਸ਼ਾਨਾ_Uni_Char,err);
                ਰੋਕੋ;
            }
        }
        ਨਹੀਂਤਾਂ{
            *err =U_ਬਫਰ_ਉਛਾਲ_ERROR;
            ਰੋਕੋ;
        }
    }
    ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ = my_ਨਿਸ਼ਾਨਾ;
    ਦਲੀਲਾਂ_2->ਸਰੋਤ = my_ਸਰੋਤ;
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
ਸਥਿਰ ਅਚਲ ਅੱਖਰ GB_2312_80_STR[] = "\x1B\x24\x29\x41";
ਸਥਿਰ ਅਚਲ ਅੱਖਰ ISO_IR_165_STR[] = "\x1B\x24\x29\x45";
ਸਥਿਰ ਅਚਲ ਅੱਖਰ CNS_11643_1992_Plane_1_STR[] = "\x1B\x24\x29\x47";
ਸਥਿਰ ਅਚਲ ਅੱਖਰ CNS_11643_1992_Plane_2_STR[] = "\x1B\x24\x2A\x48";
ਸਥਿਰ ਅਚਲ ਅੱਖਰ CNS_11643_1992_Plane_3_STR[] = "\x1B\x24\x2B\x49";
ਸਥਿਰ ਅਚਲ ਅੱਖਰ CNS_11643_1992_Plane_4_STR[] = "\x1B\x24\x2B\x4A";
ਸਥਿਰ ਅਚਲ ਅੱਖਰ CNS_11643_1992_Plane_5_STR[] = "\x1B\x24\x2B\x4B";
ਸਥਿਰ ਅਚਲ ਅੱਖਰ CNS_11643_1992_Plane_6_STR[] = "\x1B\x24\x2B\x4C";
ਸਥਿਰ ਅਚਲ ਅੱਖਰ CNS_11643_1992_Plane_7_STR[] = "\x1B\x24\x2B\x4D";

/********************** ISO2022-CN Data **************************/
ਸਥਿਰ ਅਚਲ ਅੱਖਰ* ਅਚਲ escSeqCharsCN[10] ={
        ਖਿਸਕਾਓ_ਵਿੱਚ_STR,                   /* 0 ASCII */
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

ਸਥਿਰ ਖਾਲੀ U_CALLCONV
UConverter_fromUnicode_ISO_2022_CN_OFFSETS_LOGIC(UConverter_From_Unicode_ਦਲੀਲਾਂ* ਦਲੀਲਾਂ_2, UError_ਕੋਡ* err){
    UConverter *cnv = ਦਲੀਲਾਂ_2->converter;
    UConverter_ਅੰਕੜੇ_ISO2022 *converter_ਅੰਕੜੇ;
    ISO2022State *pFromU2022State;
    uint8_t* ਨਿਸ਼ਾਨਾ = reinterpret_cast<uint8_t*>(ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ);
    ਅਚਲ uint8_t* ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ = reinterpret_cast<ਅਚਲ uint8_t*>(ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ);
    ਅਚਲ char16_t* ਸਰੋਤ = ਦਲੀਲਾਂ_2->ਸਰੋਤ;
    ਅਚਲ char16_t* ਸਰੋਤ_ਹੱਦਬੰਦੀ = ਦਲੀਲਾਂ_2->ਸਰੋਤ_ਹੱਦਬੰਦੀ;
    int32_t* offsets = ਦਲੀਲਾਂ_2->offsets;
    UChar32 ਸਰੋਤ_Char;
    ਅੱਖਰ ਬਫਰ[8];
    int32_t len;
    int8_t choices[3];
    int32_t choice_ਗਿਣਤੀ;
    uint32_t ਨਿਸ਼ਾਨਾ_ਮੁੱਲ = 0;
    UBool useFallback;

    /* set up the state */
    converter_ਅੰਕੜੇ = static_cast<UConverter_ਅੰਕੜੇ_ISO2022*>(cnv->extraInfo);
    pFromU2022State   = &converter_ਅੰਕੜੇ->fromU2022State;

    choice_ਗਿਣਤੀ = 0;

    /* check if the last codepoint of previous buffer was a lead surrogate*/
    ਜੇ((ਸਰੋਤ_Char = cnv->fromUChar32)!=0 && ਨਿਸ਼ਾਨਾ< ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ) {
        ਜਾਓ getTrail;
    }

    ਜਦੋਂ( ਸਰੋਤ < ਸਰੋਤ_ਹੱਦਬੰਦੀ){
        ਜੇ(ਨਿਸ਼ਾਨਾ < ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ){

            ਸਰੋਤ_Char  = *(ਸਰੋਤ++);
            /*check if the char is a First surrogate*/
             ਜੇ(U16_IS_SURROGATE(ਸਰੋਤ_Char)) {
                ਜੇ(U16_IS_SURROGATE_LEAD(ਸਰੋਤ_Char)) {
getTrail:
                    /*look ahead to find the trail surrogate*/
                    ਜੇ(ਸਰੋਤ < ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
                        /* test the following code unit */
                        char16_t trail = *ਸਰੋਤ;
                        ਜੇ(U16_IS_TRAIL(trail)) {
                            ਸਰੋਤ++;
                            ਸਰੋਤ_Char=U16_GET_SUPPLEMENTARY(ਸਰੋਤ_Char, trail);
                            cnv->fromUChar32=0x00;
                            /* convert this supplementary code point */
                            /* exit this condition tree */
                        } ਨਹੀਂਤਾਂ {
                            /* this is an unmatched lead code unit (1st surrogate) */
                            /* callback(illegal) */
                            *err=U_ILLEGAL_CHAR_ਮਿਲਿਆ;
                            cnv->fromUChar32=ਸਰੋਤ_Char;
                            ਰੋਕੋ;
                        }
                    } ਨਹੀਂਤਾਂ {
                        /* no more input */
                        cnv->fromUChar32=ਸਰੋਤ_Char;
                        ਰੋਕੋ;
                    }
                } ਨਹੀਂਤਾਂ {
                    /* this is an unmatched trail code unit (2nd surrogate) */
                    /* callback(illegal) */
                    *err=U_ILLEGAL_CHAR_ਮਿਲਿਆ;
                    cnv->fromUChar32=ਸਰੋਤ_Char;
                    ਰੋਕੋ;
                }
            }

            /* do the conversion */
            ਜੇ(ਸਰੋਤ_Char <= 0x007f ){
                /* do not convert SO/SI/ESC */
                ਜੇ(IS_2022_CONTROL(ਸਰੋਤ_Char)) {
                    /* callback(illegal) */
                    *err=U_ILLEGAL_CHAR_ਮਿਲਿਆ;
                    cnv->fromUChar32=ਸਰੋਤ_Char;
                    ਰੋਕੋ;
                }

                /* US-ASCII */
                ਜੇ(pFromU2022State->g == 0) {
                    ਬਫਰ[0] = static_cast<ਅੱਖਰ>(ਸਰੋਤ_Char);
                    len = 1;
                } ਨਹੀਂਤਾਂ {
                    ਬਫਰ[0] = UCNV_SI;
                    ਬਫਰ[1] = static_cast<ਅੱਖਰ>(ਸਰੋਤ_Char);
                    len = 2;
                    pFromU2022State->g = 0;
                    choice_ਗਿਣਤੀ = 0;
                }
                ਜੇ(ਸਰੋਤ_Char == CR || ਸਰੋਤ_Char == LF) {
                    /* reset the state at the end of a line */
                    uprv_memset(pFromU2022State, 0, ਆਕਾਰ(ISO2022State));
                    choice_ਗਿਣਤੀ = 0;
                }
            }
            ਨਹੀਂਤਾਂ{
                /* convert U+0080..U+10ffff */
                int32_t i;
                int8_t cs, g;

                ਜੇ(choice_ਗਿਣਤੀ == 0) {
                    /* try the current SO/G1 converter first */
                    choices[0] = pFromU2022State->cs[1];

                    /* default to GB2312_1 if none is designated yet */
                    ਜੇ(choices[0] == 0) {
                        choices[0] = GB2312_1;
                    }

                    ਜੇ(converter_ਅੰਕੜੇ->ਸੰਸਕਰਣ == 0) {
                        /* ISO-2022-CN */

                        /* try the other SO/G1 converter; a CNS_11643_1 lookup may result in any plane */
                        ਜੇ(choices[0] == GB2312_1) {
                            choices[1] = static_cast<int8_t>(CNS_11643_1);
                        } ਨਹੀਂਤਾਂ {
                            choices[1] = static_cast<int8_t>(GB2312_1);
                        }

                        choice_ਗਿਣਤੀ = 2;
                    } ਨਹੀਂਤਾਂ ਜੇ (converter_ਅੰਕੜੇ->ਸੰਸਕਰਣ == 1) {
                        /* ISO-2022-CN-EXT */

                        /* try one of the other converters */
                        ਚੋਣ(choices[0]) {
                        ਹਾਲਤ GB2312_1:
                            choices[1] = static_cast<int8_t>(CNS_11643_1);
                            choices[2] = static_cast<int8_t>(ISO_IR_165);
                            ਰੋਕੋ;
                        ਹਾਲਤ ISO_IR_165:
                            choices[1] = static_cast<int8_t>(GB2312_1);
                            choices[2] = static_cast<int8_t>(CNS_11643_1);
                            ਰੋਕੋ;
                        ਮੂਲ: /* CNS_11643_x */
                            choices[1] = static_cast<int8_t>(GB2312_1);
                            choices[2] = static_cast<int8_t>(ISO_IR_165);
                            ਰੋਕੋ;
                        }

                        choice_ਗਿਣਤੀ = 3;
                    } ਨਹੀਂਤਾਂ {
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

                ਲਈ(i = 0; i < choice_ਗਿਣਤੀ && len <= 0; ++i) {
                    int8_t cs0 = choices[i];
                    ਜੇ(cs0 > 0) {
                        uint32_t value;
                        int32_t len2;
                        ਜੇ(cs0 >= CNS_11643_0) {
                            len2 = MBCS_FROM_UCHAR32_ISO2022(
                                        converter_ਅੰਕੜੇ->myConverterArray[CNS_11643],
                                        ਸਰੋਤ_Char,
                                        &value,
                                        useFallback,
                                        MBCS_OUTPUT_3);
                            ਜੇ(len2 == 3 || (len2 == -3 && len == 0)) {
                                ਨਿਸ਼ਾਨਾ_ਮੁੱਲ = value;
                                cs = static_cast<int8_t>(CNS_11643_0 + (value >> 16) - 0x80);
                                ਜੇ(len2 >= 0) {
                                    len = 2;
                                } ਨਹੀਂਤਾਂ {
                                    len = -2;
                                    useFallback = ਝੂਠ;
                                }
                                ਜੇ(cs == CNS_11643_1) {
                                    g = 1;
                                } ਨਹੀਂਤਾਂ ਜੇ(cs == CNS_11643_2) {
                                    g = 2;
                                } ਨਹੀਂਤਾਂ /* plane 3..7 */ ਜੇ(converter_ਅੰਕੜੇ->ਸੰਸਕਰਣ == 1) {
                                    g = 3;
                                } ਨਹੀਂਤਾਂ {
                                    /* ISO-2022-CN (without -EXT) does not support plane 3..7 */
                                    len = 0;
                                }
                            }
                        } ਨਹੀਂਤਾਂ {
                            /* GB2312_1 or ISO-IR-165 */
                            U_ASSERT(cs0<UCNV_2022_MAX_CONVERTERS);
                            len2 = MBCS_FROM_UCHAR32_ISO2022(
                                        converter_ਅੰਕੜੇ->myConverterArray[cs0],
                                        ਸਰੋਤ_Char,
                                        &value,
                                        useFallback,
                                        MBCS_OUTPUT_2);
                            ਜੇ(len2 == 2 || (len2 == -2 && len == 0)) {
                                ਨਿਸ਼ਾਨਾ_ਮੁੱਲ = value;
                                len = len2;
                                cs = cs0;
                                g = 1;
                                useFallback = ਝੂਠ;
                            }
                        }
                    }
                }

                ਜੇ(len != 0) {
                    len = 0; /* count output bytes; it must have been abs(len) == 2 */

                    /* write the designation sequence if necessary */
                    ਜੇ(cs != pFromU2022State->cs[g]) {
                        ਜੇ(cs < CNS_11643) {
                            uprv_memcpy(ਬਫਰ, escSeqCharsCN[cs], 4);
                        } ਨਹੀਂਤਾਂ {
                            U_ASSERT(cs >= CNS_11643_1);
                            uprv_memcpy(ਬਫਰ, escSeqCharsCN[CNS_11643 + (cs - CNS_11643_1)], 4);
                        }
                        len = 4;
                        pFromU2022State->cs[g] = cs;
                        ਜੇ(g == 1) {
                            /* changing the SO/G1 charset invalidates the choices[] */
                            choice_ਗਿਣਤੀ = 0;
                        }
                    }

                    /* write the shift sequence if necessary */
                    ਜੇ(g != pFromU2022State->g) {
                        ਚੋਣ(g) {
                        ਹਾਲਤ 1:
                            ਬਫਰ[len++] = UCNV_SO;

                            /* set the new state only if it is the locking shift SO/G1, not for SS2 or SS3 */
                            pFromU2022State->g = 1;
                            ਰੋਕੋ;
                        ਹਾਲਤ 2:
                            ਬਫਰ[len++] = 0x1b;
                            ਬਫਰ[len++] = 0x4e;
                            ਰੋਕੋ;
                        ਮੂਲ: /* case 3 */
                            ਬਫਰ[len++] = 0x1b;
                            ਬਫਰ[len++] = 0x4f;
                            ਰੋਕੋ;
                        }
                    }

                    /* write the two output bytes */
                    ਬਫਰ[len++] = static_cast<ਅੱਖਰ>(ਨਿਸ਼ਾਨਾ_ਮੁੱਲ >> 8);
                    ਬਫਰ[len++] = static_cast<ਅੱਖਰ>(ਨਿਸ਼ਾਨਾ_ਮੁੱਲ);
                } ਨਹੀਂਤਾਂ {
                    /* if we cannot find the character after checking all codepages
                     * then this is an error
                     */
                    *err = U_INVALID_CHAR_ਮਿਲਿਆ;
                    cnv->fromUChar32=ਸਰੋਤ_Char;
                    ਰੋਕੋ;
                }
            }

            /* output len>0 bytes in buffer[] */
            ਜੇ(len == 1) {
                *ਨਿਸ਼ਾਨਾ++ = ਬਫਰ[0];
                ਜੇ(offsets) {
                    *offsets++ = static_cast<int32_t>(ਸਰੋਤ - ਦਲੀਲਾਂ_2->ਸਰੋਤ - 1); /* -1: known to be ASCII */
                }
            } ਨਹੀਂਤਾਂ ਜੇ(len == 2 && (ਨਿਸ਼ਾਨਾ + 2) <= ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ) {
                *ਨਿਸ਼ਾਨਾ++ = ਬਫਰ[0];
                *ਨਿਸ਼ਾਨਾ++ = ਬਫਰ[1];
                ਜੇ(offsets) {
                    int32_t ਸਰੋਤ_ਸੂਚਕਾਂਕ = static_cast<int32_t>(ਸਰੋਤ - ਦਲੀਲਾਂ_2->ਸਰੋਤ - U16_ਲੰਮਾਈ(ਸਰੋਤ_Char));
                    *offsets++ = ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                    *offsets++ = ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                }
            } ਨਹੀਂਤਾਂ {
                fromUWriteUInt8(
                    cnv,
                    ਬਫਰ, len,
                    &ਨਿਸ਼ਾਨਾ, reinterpret_cast<ਅਚਲ ਅੱਖਰ*>(ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ),
                    &offsets, static_cast<int32_t>(ਸਰੋਤ - ਦਲੀਲਾਂ_2->ਸਰੋਤ - U16_ਲੰਮਾਈ(ਸਰੋਤ_Char)),
                    err);
                ਜੇ(U_FAILURE(*err)) {
                    ਰੋਕੋ;
                }
            }
        } /* end if(myTargetIndex<myTargetLength) */
        ਨਹੀਂਤਾਂ{
            *err =U_ਬਫਰ_ਉਛਾਲ_ERROR;
            ਰੋਕੋ;
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
    ਜੇ( U_SUCCESS(*err) &&
        pFromU2022State->g!=0 &&
        ਦਲੀਲਾਂ_2->flush && ਸਰੋਤ>=ਸਰੋਤ_ਹੱਦਬੰਦੀ && cnv->fromUChar32==0
    ) {
        int32_t ਸਰੋਤ_ਸੂਚਕਾਂਕ;

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
        ਸਰੋਤ_ਸੂਚਕਾਂਕ = static_cast<int32_t>(ਸਰੋਤ - ਦਲੀਲਾਂ_2->ਸਰੋਤ);
        ਜੇ(ਸਰੋਤ_ਸੂਚਕਾਂਕ>0) {
            --ਸਰੋਤ_ਸੂਚਕਾਂਕ;
            ਜੇ( U16_IS_TRAIL(ਦਲੀਲਾਂ_2->ਸਰੋਤ[ਸਰੋਤ_ਸੂਚਕਾਂਕ]) &&
                (ਸਰੋਤ_ਸੂਚਕਾਂਕ==0 || U16_IS_LEAD(ਦਲੀਲਾਂ_2->ਸਰੋਤ[ਸਰੋਤ_ਸੂਚਕਾਂਕ-1]))
            ) {
                --ਸਰੋਤ_ਸੂਚਕਾਂਕ;
            }
        } ਨਹੀਂਤਾਂ {
            ਸਰੋਤ_ਸੂਚਕਾਂਕ=-1;
        }

        fromUWriteUInt8(
            cnv,
            ਖਿਸਕਾਓ_ਵਿੱਚ_STR, 1,
            &ਨਿਸ਼ਾਨਾ, reinterpret_cast<ਅਚਲ ਅੱਖਰ*>(ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ),
            &offsets, ਸਰੋਤ_ਸੂਚਕਾਂਕ,
            err);
    }

    /*save the state and return */
    ਦਲੀਲਾਂ_2->ਸਰੋਤ = ਸਰੋਤ;
    ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ = reinterpret_cast<ਅੱਖਰ*>(ਨਿਸ਼ਾਨਾ);
}


ਸਥਿਰ ਖਾਲੀ U_CALLCONV
UConverter_ਮੰਜ਼ਿਲ_Unicode_ISO_2022_CN_OFFSETS_LOGIC(UConverter_ਮੰਜ਼ਿਲ_Unicode_ਦਲੀਲਾਂ *ਦਲੀਲਾਂ_2,
                                               UError_ਕੋਡ* err){
    ਅੱਖਰ ਆਰਜ਼ੀ_Buf[3];
    ਅਚਲ ਅੱਖਰ* my_ਸਰੋਤ = const_cast<ਅੱਖਰ*>(ਦਲੀਲਾਂ_2->ਸਰੋਤ);
    char16_t *my_ਨਿਸ਼ਾਨਾ = ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ;
    ਅਚਲ ਅੱਖਰ *my_ਸਰੋਤ_ਹੱਦਬੰਦੀ = ਦਲੀਲਾਂ_2->ਸਰੋਤ_ਹੱਦਬੰਦੀ;
    uint32_t ਨਿਸ਼ਾਨਾ_Uni_Char = 0x0000;
    uint32_t my_ਸਰੋਤ_Char = 0x0000;
    UConverter_ਅੰਕੜੇ_ISO2022* my_ਅੰਕੜੇ;
    ISO2022State *p_ਮੰਜ਼ਿਲ_U2022_State;

    my_ਅੰਕੜੇ = static_cast<UConverter_ਅੰਕੜੇ_ISO2022*>(ਦਲੀਲਾਂ_2->converter->extraInfo);
    p_ਮੰਜ਼ਿਲ_U2022_State = &my_ਅੰਕੜੇ->ਮੰਜ਼ਿਲ_U2022_State;

    ਜੇ(my_ਅੰਕੜੇ->ਕੁੰਜੀ != 0) {
        /* continue with a partial escape sequence */
        ਜਾਓ escape;
    } ਨਹੀਂਤਾਂ ਜੇ(ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_ULength == 1 && my_ਸਰੋਤ < my_ਸਰੋਤ_ਹੱਦਬੰਦੀ && my_ਨਿਸ਼ਾਨਾ < ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ) {
        /* continue with a partial double-byte character */
        my_ਸਰੋਤ_Char = ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_UBytes[0];
        ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_ULength = 0;
        ਨਿਸ਼ਾਨਾ_Uni_Char = missingCharMarker;
        ਜਾਓ getTrailByte;
    }

    ਜਦੋਂ(my_ਸਰੋਤ < my_ਸਰੋਤ_ਹੱਦਬੰਦੀ){

        ਨਿਸ਼ਾਨਾ_Uni_Char =missingCharMarker;

        ਜੇ(my_ਨਿਸ਼ਾਨਾ < ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ){

            my_ਸਰੋਤ_Char = static_cast<ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ>(*my_ਸਰੋਤ++);

            ਚੋਣ(my_ਸਰੋਤ_Char){
            ਹਾਲਤ UCNV_SI:
                p_ਮੰਜ਼ਿਲ_U2022_State->g=0;
                ਜੇ (my_ਅੰਕੜੇ->is_ਖਾਲੀ_ਥਾਵਾਂ_Segment) {
                    my_ਅੰਕੜੇ->is_ਖਾਲੀ_ਥਾਵਾਂ_Segment = ਝੂਠ;	/* we are handling it, reset to avoid future spurious errors */
                    *err = U_ILLEGAL_ESCAPE_SEQUENCE;
                    ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_UCallback_Reason = UCNV_IRREGULAR;
                    ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_UBytes[0] = static_cast<uint8_t>(my_ਸਰੋਤ_Char);
                    ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_ULength = 1;
                    ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ = my_ਨਿਸ਼ਾਨਾ;
                    ਦਲੀਲਾਂ_2->ਸਰੋਤ = my_ਸਰੋਤ;
                    ਵਾਪਸ;
                }
                ਜਾਰੀ;

            ਹਾਲਤ UCNV_SO:
                ਜੇ(p_ਮੰਜ਼ਿਲ_U2022_State->cs[1] != 0) {
                    p_ਮੰਜ਼ਿਲ_U2022_State->g=1;
                    my_ਅੰਕੜੇ->is_ਖਾਲੀ_ਥਾਵਾਂ_Segment = ਸੱਚ;	/* Begin a new segment, empty so far */
                    ਜਾਰੀ;
                } ਨਹੀਂਤਾਂ {
                    /* illegal to have SO before a matching designator */
                    my_ਅੰਕੜੇ->is_ਖਾਲੀ_ਥਾਵਾਂ_Segment = ਝੂਠ;	/* Handling a different error, reset this to avoid future spurious errs */
                    ਰੋਕੋ;
                }

            ਹਾਲਤ ESC_2022:
                my_ਸਰੋਤ--;
escape:
                {
                    ਅਚਲ ਅੱਖਰ * my_ਸਰੋਤ_Before = my_ਸਰੋਤ;
                    int8_t ਮੰਜ਼ਿਲ_ULength_Before = ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_ULength;

                    changeState_2022(ਦਲੀਲਾਂ_2->converter,&(my_ਸਰੋਤ),
                        my_ਸਰੋਤ_ਹੱਦਬੰਦੀ, ISO_2022_CN,err);

                    /* After SO there must be at least one character before a designator (designator error handled separately) */
                    ਜੇ(my_ਅੰਕੜੇ->ਕੁੰਜੀ==0 && U_SUCCESS(*err) && my_ਅੰਕੜੇ->is_ਖਾਲੀ_ਥਾਵਾਂ_Segment) {
                        *err = U_ILLEGAL_ESCAPE_SEQUENCE;
                        ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_UCallback_Reason = UCNV_IRREGULAR;
                        ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_ULength = static_cast<int8_t>(ਮੰਜ਼ਿਲ_ULength_Before + (my_ਸਰੋਤ - my_ਸਰੋਤ_Before));
                    }
                }

                /* invalid or illegal escape sequence */
                ਜੇ(U_FAILURE(*err)){
                    ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ = my_ਨਿਸ਼ਾਨਾ;
                    ਦਲੀਲਾਂ_2->ਸਰੋਤ = my_ਸਰੋਤ;
                    my_ਅੰਕੜੇ->is_ਖਾਲੀ_ਥਾਵਾਂ_Segment = ਝੂਠ;	/* Reset to avoid future spurious errors */
                    ਵਾਪਸ;
                }
                ਜਾਰੀ;

            /* ISO-2022-CN does not use single-byte (C1) SS2 and SS3 */

            ਹਾਲਤ CR:
            ਹਾਲਤ LF:
                uprv_memset(p_ਮੰਜ਼ਿਲ_U2022_State, 0, ਆਕਾਰ(ISO2022State));
                U_FALLTHROUGH;
            ਮੂਲ:
                /* convert one or two bytes */
                my_ਅੰਕੜੇ->is_ਖਾਲੀ_ਥਾਵਾਂ_Segment = ਝੂਠ;
                ਜੇ(p_ਮੰਜ਼ਿਲ_U2022_State->g != 0) {
                    ਜੇ(my_ਸਰੋਤ < my_ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
                        UConverter_Shared_ਅੰਕੜੇ *cnv;
                        StateEnum ਆਰਜ਼ੀ_State;
                        int32_t ਆਰਜ਼ੀ_Buf_Len;
                        ਪੂਰਨ leadIsOk, trailIsOk;
                        uint8_t trailByte;
getTrailByte:
                        trailByte = static_cast<uint8_t>(*my_ਸਰੋਤ);
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
                        leadIsOk = static_cast<uint8_t>(my_ਸਰੋਤ_Char - 0x21) <= (0x7e - 0x21);
                        trailIsOk = static_cast<uint8_t>(trailByte - 0x21) <= (0x7e - 0x21);
                        ਜੇ (leadIsOk && trailIsOk) {
                            ++my_ਸਰੋਤ;
                            ਆਰਜ਼ੀ_State = static_cast<StateEnum>(p_ਮੰਜ਼ਿਲ_U2022_State->cs[p_ਮੰਜ਼ਿਲ_U2022_State->g]);
                            ਜੇ(ਆਰਜ਼ੀ_State >= CNS_11643_0) {
                                cnv = my_ਅੰਕੜੇ->myConverterArray[CNS_11643];
                                ਆਰਜ਼ੀ_Buf[0] = static_cast<ਅੱਖਰ>(0x80 + (ਆਰਜ਼ੀ_State - CNS_11643_0));
                                ਆਰਜ਼ੀ_Buf[1] = static_cast<ਅੱਖਰ>(my_ਸਰੋਤ_Char);
                                ਆਰਜ਼ੀ_Buf[2] = static_cast<ਅੱਖਰ>(trailByte);
                                ਆਰਜ਼ੀ_Buf_Len = 3;

                            }ਨਹੀਂਤਾਂ{
                                U_ASSERT(ਆਰਜ਼ੀ_State<UCNV_2022_MAX_CONVERTERS);
                                cnv = my_ਅੰਕੜੇ->myConverterArray[ਆਰਜ਼ੀ_State];
                                ਆਰਜ਼ੀ_Buf[0] = static_cast<ਅੱਖਰ>(my_ਸਰੋਤ_Char);
                                ਆਰਜ਼ੀ_Buf[1] = static_cast<ਅੱਖਰ>(trailByte);
                                ਆਰਜ਼ੀ_Buf_Len = 2;
                            }
                            ਨਿਸ਼ਾਨਾ_Uni_Char = ucnv_MBCSSimple_Get_ਅਗਲਾ_UChar(cnv, ਆਰਜ਼ੀ_Buf, ਆਰਜ਼ੀ_Buf_Len, ਝੂਠ);
                            my_ਸਰੋਤ_Char = (my_ਸਰੋਤ_Char << 8) | trailByte;
                        } ਨਹੀਂਤਾਂ ਜੇ (!(trailIsOk || IS_2022_CONTROL(trailByte))) {
                            /* report a pair of illegal bytes if the second byte is not a DBCS starter */
                            ++my_ਸਰੋਤ;
                            /* add another bit so that the code below writes 2 bytes in case of error */
                            my_ਸਰੋਤ_Char = 0x10000 | (my_ਸਰੋਤ_Char << 8) | trailByte;
                        }
                        ਜੇ(p_ਮੰਜ਼ਿਲ_U2022_State->g>=2) {
                            /* return from a single-shift state to the previous one */
                            p_ਮੰਜ਼ਿਲ_U2022_State->g=p_ਮੰਜ਼ਿਲ_U2022_State->prevG;
                        }
                    } ਨਹੀਂਤਾਂ {
                        ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_UBytes[0] = static_cast<uint8_t>(my_ਸਰੋਤ_Char);
                        ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_ULength = 1;
                        ਜਾਓ endloop;
                    }
                }
                ਨਹੀਂਤਾਂ{
                    ਜੇ(my_ਸਰੋਤ_Char <= 0x7f) {
                        ਨਿਸ਼ਾਨਾ_Uni_Char = static_cast<char16_t>(my_ਸਰੋਤ_Char);
                    }
                }
                ਰੋਕੋ;
            }
            ਜੇ(ਨਿਸ਼ਾਨਾ_Uni_Char < (missingCharMarker-1/*0xfffe*/)){
                ਜੇ(ਦਲੀਲਾਂ_2->offsets){
                    ਦਲੀਲਾਂ_2->offsets[my_ਨਿਸ਼ਾਨਾ - ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ] = static_cast<int32_t>(my_ਸਰੋਤ - ਦਲੀਲਾਂ_2->ਸਰੋਤ - (my_ਸਰੋਤ_Char <= 0xff ? 1 : 2));
                }
                *(my_ਨਿਸ਼ਾਨਾ++) = static_cast<char16_t>(ਨਿਸ਼ਾਨਾ_Uni_Char);
            }
            ਨਹੀਂਤਾਂ ਜੇ(ਨਿਸ਼ਾਨਾ_Uni_Char > missingCharMarker){
                /* disassemble the surrogate pair and write to output*/
                ਨਿਸ਼ਾਨਾ_Uni_Char-=0x0010000;
                *my_ਨਿਸ਼ਾਨਾ = static_cast<char16_t>(0xd800 + static_cast<char16_t>(ਨਿਸ਼ਾਨਾ_Uni_Char >> 10));
                ਜੇ(ਦਲੀਲਾਂ_2->offsets){
                    ਦਲੀਲਾਂ_2->offsets[my_ਨਿਸ਼ਾਨਾ - ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ] = static_cast<int32_t>(my_ਸਰੋਤ - ਦਲੀਲਾਂ_2->ਸਰੋਤ - (my_ਸਰੋਤ_Char <= 0xff ? 1 : 2));
                }
                ++my_ਨਿਸ਼ਾਨਾ;
                ਜੇ(my_ਨਿਸ਼ਾਨਾ< ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ){
                    *my_ਨਿਸ਼ਾਨਾ = static_cast<char16_t>(0xdc00 + static_cast<char16_t>(ਨਿਸ਼ਾਨਾ_Uni_Char & 0x3ff));
                    ਜੇ(ਦਲੀਲਾਂ_2->offsets){
                        ਦਲੀਲਾਂ_2->offsets[my_ਨਿਸ਼ਾਨਾ - ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ] = static_cast<int32_t>(my_ਸਰੋਤ - ਦਲੀਲਾਂ_2->ਸਰੋਤ - (my_ਸਰੋਤ_Char <= 0xff ? 1 : 2));
                    }
                    ++my_ਨਿਸ਼ਾਨਾ;
                }ਨਹੀਂਤਾਂ{
                    ਦਲੀਲਾਂ_2->converter->UChar_Error_ਬਫਰ[ਦਲੀਲਾਂ_2->converter->UChar_Error_ਬਫਰ_ਲੰਮਾਈ++]=
                                    static_cast<char16_t>(0xdc00 + static_cast<char16_t>(ਨਿਸ਼ਾਨਾ_Uni_Char & 0x3ff));
                }

            }
            ਨਹੀਂਤਾਂ{
                /* Call the callback function*/
                ਮੰਜ਼ਿਲ_Unicode_Callback(ਦਲੀਲਾਂ_2->converter,my_ਸਰੋਤ_Char,ਨਿਸ਼ਾਨਾ_Uni_Char,err);
                ਰੋਕੋ;
            }
        }
        ਨਹੀਂਤਾਂ{
            *err =U_ਬਫਰ_ਉਛਾਲ_ERROR;
            ਰੋਕੋ;
        }
    }
endloop:
    ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ = my_ਨਿਸ਼ਾਨਾ;
    ਦਲੀਲਾਂ_2->ਸਰੋਤ = my_ਸਰੋਤ;
}
#ਜੇ_ਅੰਤ /* #if !UCONFIG_ONLY_HTML_CONVERSION */

ਸਥਿਰ ਖਾਲੀ U_CALLCONV
_ISO_2022_WriteSub(UConverter_From_Unicode_ਦਲੀਲਾਂ *ਦਲੀਲਾਂ_2, int32_t ਵਿੱਥ_ਸੂਚਕਾਂਕ, UError_ਕੋਡ *err) {
    UConverter *cnv = ਦਲੀਲਾਂ_2->converter;
    UConverter_ਅੰਕੜੇ_ISO2022* my_Converter_ਅੰਕੜੇ = static_cast<UConverter_ਅੰਕੜੇ_ISO2022*>(cnv->extraInfo);
    ISO2022State *pFromU2022State=&my_Converter_ਅੰਕੜੇ->fromU2022State;
    ਅੱਖਰ *p, *subchar;
    ਅੱਖਰ ਬਫਰ[8];
    int32_t length;

    subchar = reinterpret_cast<ਅੱਖਰ*>(cnv->subChars);
    length=cnv->subCharLen; /* assume length==1 for most variants */

    p = ਬਫਰ;
    ਚੋਣ(my_Converter_ਅੰਕੜੇ->locale[0]){
    ਹਾਲਤ 'j':
        {
            int8_t cs;

            ਜੇ(pFromU2022State->g == 1) {
                /* JIS7: switch from G1 to G0 */
                pFromU2022State->g = 0;
                *p++ = UCNV_SI;
            }

            cs = pFromU2022State->cs[0];
            ਜੇ(cs != ASCII && cs != JISX201) {
                /* not in ASCII or JIS X 0201: switch to ASCII */
                pFromU2022State->cs[0] = static_cast<int8_t>(ASCII);
                *p++ = '\x1b';
                *p++ = '\x28';
                *p++ = '\x42';
            }

            *p++ = subchar[0];
            ਰੋਕੋ;
        }
    ਹਾਲਤ 'c':
        ਜੇ(pFromU2022State->g != 0) {
            /* not in ASCII mode: switch to ASCII */
            pFromU2022State->g = 0;
            *p++ = UCNV_SI;
        }
        *p++ = subchar[0];
        ਰੋਕੋ;
    ਹਾਲਤ 'k':
        ਜੇ(my_Converter_ਅੰਕੜੇ->ਸੰਸਕਰਣ == 0) {
            ਜੇ(length == 1) {
                ਜੇ(ਦਲੀਲਾਂ_2->converter->from_Unicode_ਸਥਿਤੀ) {
                    /* in DBCS mode: switch to SBCS */
                    ਦਲੀਲਾਂ_2->converter->from_Unicode_ਸਥਿਤੀ = 0;
                    *p++ = UCNV_SI;
                }
                *p++ = subchar[0];
            } ਨਹੀਂਤਾਂ /* length == 2*/ {
                ਜੇ(!ਦਲੀਲਾਂ_2->converter->from_Unicode_ਸਥਿਤੀ) {
                    /* in SBCS mode: switch to DBCS */
                    ਦਲੀਲਾਂ_2->converter->from_Unicode_ਸਥਿਤੀ = 1;
                    *p++ = UCNV_SO;
                }
                *p++ = subchar[0];
                *p++ = subchar[1];
            }
            ਰੋਕੋ;
        } ਨਹੀਂਤਾਂ {
            /* save the subconverter's substitution string */
            uint8_t *currentSubChars = my_Converter_ਅੰਕੜੇ->currentConverter->subChars;
            int8_t currentSubCharLen = my_Converter_ਅੰਕੜੇ->currentConverter->subCharLen;

            /* set our substitution string into the subconverter */
            my_Converter_ਅੰਕੜੇ->currentConverter->subChars = reinterpret_cast<uint8_t*>(subchar);
            my_Converter_ਅੰਕੜੇ->currentConverter->subCharLen = static_cast<int8_t>(length);

            /* let the subconverter write the subchar, set/retrieve fromUChar32 state */
            ਦਲੀਲਾਂ_2->converter = my_Converter_ਅੰਕੜੇ->currentConverter;
            my_Converter_ਅੰਕੜੇ->currentConverter->fromUChar32 = cnv->fromUChar32;
            ucnv_cbFromUWriteSub(ਦਲੀਲਾਂ_2, 0, err);
            cnv->fromUChar32 = my_Converter_ਅੰਕੜੇ->currentConverter->fromUChar32;
            ਦਲੀਲਾਂ_2->converter = cnv;

            /* restore the subconverter's substitution string */
            my_Converter_ਅੰਕੜੇ->currentConverter->subChars = currentSubChars;
            my_Converter_ਅੰਕੜੇ->currentConverter->subCharLen = currentSubCharLen;

            ਜੇ(*err == U_ਬਫਰ_ਉਛਾਲ_ERROR) {
                ਜੇ(my_Converter_ਅੰਕੜੇ->currentConverter->char_Error_ਬਫਰ_ਲੰਮਾਈ > 0) {
                    uprv_memcpy(
                        cnv->char_Error_ਬਫਰ,
                        my_Converter_ਅੰਕੜੇ->currentConverter->char_Error_ਬਫਰ,
                        my_Converter_ਅੰਕੜੇ->currentConverter->char_Error_ਬਫਰ_ਲੰਮਾਈ);
                }
                cnv->char_Error_ਬਫਰ_ਲੰਮਾਈ = my_Converter_ਅੰਕੜੇ->currentConverter->char_Error_ਬਫਰ_ਲੰਮਾਈ;
                my_Converter_ਅੰਕੜੇ->currentConverter->char_Error_ਬਫਰ_ਲੰਮਾਈ = 0;
            }
            ਵਾਪਸ;
        }
    ਮੂਲ:
        /* not expected */
        ਰੋਕੋ;
    }
    ucnv_cb_From_UWrite_ਬਾਈਟਾਂ(ਦਲੀਲਾਂ_2,
                           ਬਫਰ, static_cast<int32_t>(p - ਬਫਰ),
                           ਵਿੱਥ_ਸੂਚਕਾਂਕ, err);
}

/*
 * Structure for cloning an ISO 2022 converter into a single memory block.
 */
ਬਣਤਰ cloneStruct
{
    UConverter cnv;
    UConverter currentConverter;
    UConverter_ਅੰਕੜੇ_ISO2022 mydata;
};


U_CDECL_BEGIN

ਸਥਿਰ UConverter * U_CALLCONV
ISO_2022_ਸੁਰੱਖਿਅਤ_Clone(
            ਅਚਲ UConverter *cnv,
            ਖਾਲੀ *ਢੇਰ_ਬਫਰ,
            int32_t *p_ਬਫਰ_ਮਾਪ,
            UError_ਕੋਡ *ਸਥਿਤੀ)
{
    ਬਣਤਰ cloneStruct * localClone;
    UConverter_ਅੰਕੜੇ_ISO2022 *cnv_ਅੰਕੜੇ;
    int32_t i, size;

    ਜੇ (U_FAILURE(*ਸਥਿਤੀ)){
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    ਜੇ (*p_ਬਫਰ_ਮਾਪ == 0) { /* 'preflighting' request - set needed size into *pBufferSize */
        *p_ਬਫਰ_ਮਾਪ = (int32_t)ਆਕਾਰ(ਬਣਤਰ cloneStruct);
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    cnv_ਅੰਕੜੇ = (UConverter_ਅੰਕੜੇ_ISO2022 *)cnv->extraInfo;
    localClone = (ਬਣਤਰ cloneStruct *)ਢੇਰ_ਬਫਰ;

    /* ucnv.c/ucnv_safeClone() copied the main UConverter already */

    uprv_memcpy(&localClone->mydata, cnv_ਅੰਕੜੇ, ਆਕਾਰ(UConverter_ਅੰਕੜੇ_ISO2022));
    localClone->cnv.extraInfo = &localClone->mydata; /* set pointer to extra data */
    localClone->cnv.isExtraLocal = ਸੱਚ;

    /* share the subconverters */

    ਜੇ(cnv_ਅੰਕੜੇ->currentConverter != ਖਾਲੀ_ਸੂਚਕ) {
        size = (int32_t)ਆਕਾਰ(UConverter);
        localClone->mydata.currentConverter =
            ucnv_ਸੁਰੱਖਿਅਤ_Clone(cnv_ਅੰਕੜੇ->currentConverter,
                            &localClone->currentConverter,
                            &size, ਸਥਿਤੀ);
        ਜੇ(U_FAILURE(*ਸਥਿਤੀ)) {
            ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
        }
    }

    ਲਈ(i=0; i<UCNV_2022_MAX_CONVERTERS; ++i) {
        ਜੇ(cnv_ਅੰਕੜੇ->myConverterArray[i] != ਖਾਲੀ_ਸੂਚਕ) {
            ucnv_increment_Ref_ਗਿਣਤੀ(cnv_ਅੰਕੜੇ->myConverterArray[i]);
        }
    }

    ਵਾਪਸ &localClone->cnv;
}

U_CDECL_ਸਿਰਾ

ਸਥਿਰ ਖਾਲੀ U_CALLCONV
ISO_2022_Get_Unicode_ਸੈੱਟ(ਅਚਲ UConverter *cnv,
                    ਅਚਲ USetAdder *sa,
                    UConverter_Unicode_ਸੈੱਟ which,
                    UError_ਕੋਡ *p_Error_ਕੋਡ)
{
    int32_t i;
    UConverter_ਅੰਕੜੇ_ISO2022* cnv_ਅੰਕੜੇ;

    ਜੇ (U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ;
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ U_ENABLE_GENERIC_ISO_2022
    ਜੇ (cnv->shared_ਅੰਕੜੇ == &ISO2022_ਅੰਕੜੇ) {
        /* We use UTF-8 in this case */
        sa->ਜੋੜੋ_Range(sa->ਸੈੱਟ, 0, 0xd7FF);
        sa->ਜੋੜੋ_Range(sa->ਸੈੱਟ, 0xE000, 0x10FFFF);
        ਵਾਪਸ;
    }
#ਜੇ_ਅੰਤ

    cnv_ਅੰਕੜੇ = static_cast<UConverter_ਅੰਕੜੇ_ISO2022*>(cnv->extraInfo);

    /* open a set and initialize it with code points that are algorithmically round-tripped */
    ਚੋਣ(cnv_ਅੰਕੜੇ->locale[0]){
    ਹਾਲਤ 'j':
        /* include JIS X 0201 which is hardcoded */
        sa->ਜੋੜੋ(sa->ਸੈੱਟ, 0xa5);
        sa->ਜੋੜੋ(sa->ਸੈੱਟ, 0x203e);
        ਜੇ(jpCharsetMasks[cnv_ਅੰਕੜੇ->ਸੰਸਕਰਣ]&CSM(ISO8859_1)) {
            /* include Latin-1 for some variants of JP */
            sa->ਜੋੜੋ_Range(sa->ਸੈੱਟ, 0, 0xff);
        } ਨਹੀਂਤਾਂ {
            /* include ASCII for JP */
            sa->ਜੋੜੋ_Range(sa->ਸੈੱਟ, 0, 0x7f);
        }
        ਜੇ(cnv_ਅੰਕੜੇ->ਸੰਸਕਰਣ==3 || cnv_ਅੰਕੜੇ->ਸੰਸਕਰਣ==4 || which==UCNV_ROUNDTRIP_AND_FALLBACK_ਸੈੱਟ) {
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
            sa->ਜੋੜੋ_Range(sa->ਸੈੱਟ, HWKANA_ਸ਼ੁਰੂ, HWKANA_ਸਿਰਾ);
        }
        ਰੋਕੋ;
#ਜੇ !UCONFIG_ONLY_HTML_CONVERSION
    ਹਾਲਤ 'c':
    ਹਾਲਤ 'z':
        /* include ASCII for CN */
        sa->ਜੋੜੋ_Range(sa->ਸੈੱਟ, 0, 0x7f);
        ਰੋਕੋ;
    ਹਾਲਤ 'k':
        /* there is only one converter for KR, and it is not in the myConverterArray[] */
        cnv_ਅੰਕੜੇ->currentConverter->shared_ਅੰਕੜੇ->impl->get_Unicode_ਸੈੱਟ(
                cnv_ਅੰਕੜੇ->currentConverter, sa, which, p_Error_ਕੋਡ);
        /* the loop over myConverterArray[] will simply not find another converter */
        ਰੋਕੋ;
#ਜੇ_ਅੰਤ
    ਮੂਲ:
        ਰੋਕੋ;
    }

#ਜੇ 0  /* Replaced by ucnv_MBCSGetFilteredUnicodeSetForUnicode() until we implement ucnv_getUnicodeSet() with reverse fallbacks. */
            ਜੇ( (cnv_ਅੰਕੜੇ->locale[0]=='c' || cnv_ਅੰਕੜੇ->locale[0]=='z') &&
                cnv_ਅੰਕੜੇ->ਸੰਸਕਰਣ==0 && i==CNS_11643
            ) {
                /* special handling for non-EXT ISO-2022-CN: add only code points for CNS planes 1 and 2 */
                ucnv_MBCSGet_Unicode_ਸੈੱਟ_For_ਬਾਈਟਾਂ(
                        cnv_ਅੰਕੜੇ->myConverterArray[i],
                        sa, UCNV_ROUNDTRIP_ਸੈੱਟ,
                        0, 0x81, 0x82,
                        p_Error_ਕੋਡ);
            }
#ਜੇ_ਅੰਤ

    ਲਈ (i=0; i<UCNV_2022_MAX_CONVERTERS; i++) {
        UConverter_ਸੈੱਟ_Filter filter;
        ਜੇ(cnv_ਅੰਕੜੇ->myConverterArray[i]!=ਖਾਲੀ_ਸੂਚਕ) {
            ਜੇ(cnv_ਅੰਕੜੇ->locale[0]=='j' && i==JISX208) {
                /*
                 * Only add code points that map to Shift-JIS codes
                 * corresponding to JIS X 0208.
                 */
                filter=UCNV_ਸੈੱਟ_FILTER_SJIS;
#ਜੇ !UCONFIG_ONLY_HTML_CONVERSION
            } ਨਹੀਂਤਾਂ ਜੇ( (cnv_ਅੰਕੜੇ->locale[0]=='c' || cnv_ਅੰਕੜੇ->locale[0]=='z') &&
                       cnv_ਅੰਕੜੇ->ਸੰਸਕਰਣ==0 && i==CNS_11643) {
                /*
                 * Version-specific for CN:
                 * CN version 0 does not map CNS planes 3..7 although
                 * they are all available in the CNS conversion table;
                 * CN version 1 (-EXT) does map them all.
                 * The two versions create different Unicode sets.
                 */
                filter=UCNV_ਸੈੱਟ_FILTER_2022_CN;
            } ਨਹੀਂਤਾਂ ਜੇ(i==KSC5601) {
                /*
                 * Some of the KSC 5601 tables (convrtrs.txt has this aliases on multiple tables)
                 * are broader than GR94.
                 */
                filter=UCNV_ਸੈੱਟ_FILTER_GR94_DBCS;
#ਜੇ_ਅੰਤ
            } ਨਹੀਂਤਾਂ {
                filter=UCNV_ਸੈੱਟ_FILTER_NONE;
            }
            ucnv_MBCSGet_Filtered_Unicode_ਸੈੱਟ_For_Unicode(cnv_ਅੰਕੜੇ->myConverterArray[i], sa, which, filter, p_Error_ਕੋਡ);
        }
    }

    /*
     * ISO 2022 converters must not convert SO/SI/ESC despite what
     * sub-converters do by themselves.
     * Remove these characters from the set.
     */
    sa->remove(sa->ਸੈੱਟ, 0x0e);
    sa->remove(sa->ਸੈੱਟ, 0x0f);
    sa->remove(sa->ਸੈੱਟ, 0x1b);

    /* ISO 2022 converters do not convert C1 controls either */
    sa->removeRange(sa->ਸੈੱਟ, 0x80, 0x9f);
}

ਸਥਿਰ ਅਚਲ UConverterImpl _ISO2022Impl={
    UCNV_ISO_2022,

    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ,

    _ISO2022Open,
    _ISO2022Close,
    _ISO2022Reset,

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ U_ENABLE_GENERIC_ISO_2022
    T_UConverter_ਮੰਜ਼ਿਲ_Unicode_ISO_2022_OFFSETS_LOGIC,
    T_UConverter_ਮੰਜ਼ਿਲ_Unicode_ISO_2022_OFFSETS_LOGIC,
    ucnv_fromUnicode_UTF8,
    ucnv_fromUnicode_UTF8_OFFSETS_LOGIC,
#ਨਹੀਂਤਾਂ
    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ,
#ਜੇ_ਅੰਤ
    ਖਾਲੀ_ਸੂਚਕ,

    ਖਾਲੀ_ਸੂਚਕ,
    _ISO2022getName,
    _ISO_2022_WriteSub,
    ISO_2022_ਸੁਰੱਖਿਅਤ_Clone,
    ISO_2022_Get_Unicode_ਸੈੱਟ,

    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ
};
ਸਥਿਰ ਅਚਲ UConverter_Static_ਅੰਕੜੇ ISO2022_Static_ਅੰਕੜੇ={
    ਆਕਾਰ(UConverter_Static_ਅੰਕੜੇ),
    "ISO_2022",
    2022,
    UCNV_IBM,
    UCNV_ISO_2022,
    1,
    3, /* max 3 bytes per char16_t from UTF-8 (4 bytes from surrogate _pair_) */
    { 0x1a, 0, 0, 0 },
    1,
    ਝੂਠ,
    ਝੂਠ,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};
ਅਚਲ UConverter_Shared_ਅੰਕੜੇ ISO2022_ਅੰਕੜੇ=
        UCNV_IMMUTABLE_SHARED_ਅੰਕੜੇ_INITIALIZER(&ISO2022_Static_ਅੰਕੜੇ, &_ISO2022Impl);

/*************JP****************/
ਸਥਿਰ ਅਚਲ UConverterImpl _ISO2022JPImpl={
    UCNV_ISO_2022,

    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ,

    _ISO2022Open,
    _ISO2022Close,
    _ISO2022Reset,

    UConverter_ਮੰਜ਼ਿਲ_Unicode_ISO_2022_JP_OFFSETS_LOGIC,
    UConverter_ਮੰਜ਼ਿਲ_Unicode_ISO_2022_JP_OFFSETS_LOGIC,
    UConverter_fromUnicode_ISO_2022_JP_OFFSETS_LOGIC,
    UConverter_fromUnicode_ISO_2022_JP_OFFSETS_LOGIC,
    ਖਾਲੀ_ਸੂਚਕ,

    ਖਾਲੀ_ਸੂਚਕ,
    _ISO2022getName,
    _ISO_2022_WriteSub,
    ISO_2022_ਸੁਰੱਖਿਅਤ_Clone,
    ISO_2022_Get_Unicode_ਸੈੱਟ,

    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ
};
ਸਥਿਰ ਅਚਲ UConverter_Static_ਅੰਕੜੇ ISO2022_JPStatic_ਅੰਕੜੇ={
    ਆਕਾਰ(UConverter_Static_ਅੰਕੜੇ),
    "ISO_2022_JP",
    0,
    UCNV_IBM,
    UCNV_ISO_2022,
    1,
    6, /* max 6 bytes per char16_t: 4-byte escape sequence + DBCS */
    { 0x1a, 0, 0, 0 },
    1,
    ਝੂਠ,
    ਝੂਠ,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

ਨਾਮਾਕਾਸ਼ {

ਅਚਲ UConverter_Shared_ਅੰਕੜੇ _ISO2022JPData=
        UCNV_IMMUTABLE_SHARED_ਅੰਕੜੇ_INITIALIZER(&ISO2022_JPStatic_ਅੰਕੜੇ, &_ISO2022JPImpl);

}  // namespace

#ਜੇ !UCONFIG_ONLY_HTML_CONVERSION
/************* KR ***************/
ਸਥਿਰ ਅਚਲ UConverterImpl _ISO2022KRImpl={
    UCNV_ISO_2022,

    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ,

    _ISO2022Open,
    _ISO2022Close,
    _ISO2022Reset,

    UConverter_ਮੰਜ਼ਿਲ_Unicode_ISO_2022_KR_OFFSETS_LOGIC,
    UConverter_ਮੰਜ਼ਿਲ_Unicode_ISO_2022_KR_OFFSETS_LOGIC,
    UConverter_fromUnicode_ISO_2022_KR_OFFSETS_LOGIC,
    UConverter_fromUnicode_ISO_2022_KR_OFFSETS_LOGIC,
    ਖਾਲੀ_ਸੂਚਕ,

    ਖਾਲੀ_ਸੂਚਕ,
    _ISO2022getName,
    _ISO_2022_WriteSub,
    ISO_2022_ਸੁਰੱਖਿਅਤ_Clone,
    ISO_2022_Get_Unicode_ਸੈੱਟ,

    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ
};
ਸਥਿਰ ਅਚਲ UConverter_Static_ਅੰਕੜੇ ISO2022_KRStatic_ਅੰਕੜੇ={
    ਆਕਾਰ(UConverter_Static_ਅੰਕੜੇ),
    "ISO_2022_KR",
    0,
    UCNV_IBM,
    UCNV_ISO_2022,
    1,
    8, /* max 8 bytes per char16_t */
    { 0x1a, 0, 0, 0 },
    1,
    ਝੂਠ,
    ਝੂਠ,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

ਨਾਮਾਕਾਸ਼ {

ਅਚਲ UConverter_Shared_ਅੰਕੜੇ _ISO2022KRData=
        UCNV_IMMUTABLE_SHARED_ਅੰਕੜੇ_INITIALIZER(&ISO2022_KRStatic_ਅੰਕੜੇ, &_ISO2022KRImpl);

}  // namespace

/*************** CN ***************/
ਸਥਿਰ ਅਚਲ UConverterImpl _ISO2022CNImpl={

    UCNV_ISO_2022,

    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ,

    _ISO2022Open,
    _ISO2022Close,
    _ISO2022Reset,

    UConverter_ਮੰਜ਼ਿਲ_Unicode_ISO_2022_CN_OFFSETS_LOGIC,
    UConverter_ਮੰਜ਼ਿਲ_Unicode_ISO_2022_CN_OFFSETS_LOGIC,
    UConverter_fromUnicode_ISO_2022_CN_OFFSETS_LOGIC,
    UConverter_fromUnicode_ISO_2022_CN_OFFSETS_LOGIC,
    ਖਾਲੀ_ਸੂਚਕ,

    ਖਾਲੀ_ਸੂਚਕ,
    _ISO2022getName,
    _ISO_2022_WriteSub,
    ISO_2022_ਸੁਰੱਖਿਅਤ_Clone,
    ISO_2022_Get_Unicode_ਸੈੱਟ,

    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ
};
ਸਥਿਰ ਅਚਲ UConverter_Static_ਅੰਕੜੇ ISO2022_CNStatic_ਅੰਕੜੇ={
    ਆਕਾਰ(UConverter_Static_ਅੰਕੜੇ),
    "ISO_2022_CN",
    0,
    UCNV_IBM,
    UCNV_ISO_2022,
    1,
    8, /* max 8 bytes per char16_t: 4-byte CNS designator + 2 bytes for SS2/SS3 + DBCS */
    { 0x1a, 0, 0, 0 },
    1,
    ਝੂਠ,
    ਝੂਠ,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

ਨਾਮਾਕਾਸ਼ {

ਅਚਲ UConverter_Shared_ਅੰਕੜੇ _ISO2022CNData=
        UCNV_IMMUTABLE_SHARED_ਅੰਕੜੇ_INITIALIZER(&ISO2022_CNStatic_ਅੰਕੜੇ, &_ISO2022CNImpl);

}  // namespace
#ਜੇ_ਅੰਤ /* #if !UCONFIG_ONLY_HTML_CONVERSION */

#ਜੇ_ਅੰਤ /* #if !UCONFIG_NO_LEGACY_CONVERSION */
