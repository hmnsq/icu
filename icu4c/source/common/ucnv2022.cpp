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

#অন্তর্ভাব "unicode/utypes.h"

#যদি !UCONFIG_NO_CONVERSION && !UCONFIG_NO_LEGACY_CONVERSION

#অন্তর্ভাব "unicode/ucnv.h"
#অন্তর্ভাব "unicode/uset.h"
#অন্তর্ভাব "unicode/ucnv_err.h"
#অন্তর্ভাব "unicode/ucnv_cb.h"
#অন্তর্ভাব "unicode/utf16.h"
#অন্তর্ভাব "ucnv_imp.h"
#অন্তর্ভাব "ucnv_bld.h"
#অন্তর্ভাব "ucnv_cnv.h"
#অন্তর্ভাব "ucnvmbcs.h"
#অন্তর্ভাব "cstring.h"
#অন্তর্ভাব "cmemory.h"
#অন্তর্ভাব "uassert.h"

#ifdef U_ENABLE_GENERIC_ISO_2022
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
#endif

#যদি !UCONFIG_ONLY_HTML_CONVERSION
স্থির ধ্রুবক অক্ষর SHIFT_IN_STR[]  = "\x0F";
// static const char SHIFT_OUT_STR[] = "\x0E";
#endif

#define CR      0x0D
#define LF      0x0A
#define H_TAB   0x09
#define V_TAB   0x0B
#define SPACE   0x20

গণনা {
    HWKANA_START=0xff61,
    HWKANA_END=0xff9f
};

/*
 * 94-character sets with native byte values A1..FE are encoded in ISO 2022
 * as bytes 21..7E. (Subtract 0x80.)
 * 96-character sets with native byte values A0..FF are encoded in ISO 2022
 * as bytes 20..7F. (Subtract 0x80.)
 * Do not encode C1 control codes with native bytes 80..9F
 * as bytes 00..1F (C0 control codes).
 */
গণনা {
    GR94_START=0xa1,
    GR94_END=0xfe,
    GR96_START=0xa0,
    GR96_END=0xff
};

/*
 * ISO 2022 control codes must not be converted from Unicode
 * because they would mess up the byte stream.
 * The bit mask 0x0800c000 has bits set at bit positions 0xe, 0xf, 0x1b
 * corresponding to SO, SI, and ESC.
 */
#define IS_2022_CONTROL(c) (((c)<0x20) && (((uint32_t)1<<(c))&0x0800c000)!=0)

/* for ISO-2022-JP and -CN implementations */
প্রকারলেখ গণনা  {
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
        HWKANA_7BIT=8,    /* Halfwidth Katakana 7 bit */

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
#যদি UCONFIG_ONLY_HTML_CONVERSION
#define IS_JP_DBCS(cs) (JISX208==(cs))
#অন্যথা
#define IS_JP_DBCS(cs) (JISX208<=(cs) && (cs)<=KSC5601)
#endif

#define CSM(cs) ((uint16_t)1<<(cs))

/*
 * Each of these charset masks (with index x) contains a bit for a charset in exact correspondence
 * to whether that charset is used in the corresponding version x of ISO_2022,locale=ja,version=x
 *
 * Note: The converter uses some leniency:
 * - The escape sequence ESC ( I for half-width 7-bit Katakana is recognized in
 *   all versions, not just JIS7 and JIS8.
 * - ICU does not distinguish between different versions of JIS X 0208.
 */
#যদি UCONFIG_ONLY_HTML_CONVERSION
গণনা { MAX_JA_VERSION=0 };
#অন্যথা
গণনা { MAX_JA_VERSION=4 };
#endif
স্থির ধ্রুবক uint16_t jpCharsetMasks[MAX_JA_VERSION+1]={
    CSM(ASCII)|CSM(JISX201)|CSM(JISX208)|CSM(HWKANA_7BIT),
#যদি !UCONFIG_ONLY_HTML_CONVERSION
    CSM(ASCII)|CSM(JISX201)|CSM(JISX208)|CSM(HWKANA_7BIT)|CSM(JISX212),
    CSM(ASCII)|CSM(JISX201)|CSM(JISX208)|CSM(HWKANA_7BIT)|CSM(JISX212)|CSM(GB2312)|CSM(KSC5601)|CSM(ISO8859_1)|CSM(ISO8859_7),
    CSM(ASCII)|CSM(JISX201)|CSM(JISX208)|CSM(HWKANA_7BIT)|CSM(JISX212)|CSM(GB2312)|CSM(KSC5601)|CSM(ISO8859_1)|CSM(ISO8859_7),
    CSM(ASCII)|CSM(JISX201)|CSM(JISX208)|CSM(HWKANA_7BIT)|CSM(JISX212)|CSM(GB2312)|CSM(KSC5601)|CSM(ISO8859_1)|CSM(ISO8859_7)
#endif
};

প্রকারলেখ গণনা {
        ASCII1=0,
        LATIN1,
        SBCS,
        DBCS,
        MBCS,
        HWKANA
}Cnv2022Type;

প্রকারলেখ কাঠামো ISO2022State {
    int8_t cs[4];       /* charset number for SI (G0)/SO (G1)/SS2 (G2)/SS3 (G3) */
    int8_t g;           /* 0..3 for G0..G3 (SI/SO/SS2/SS3) */
    int8_t পূর্ব_G;       /* g before single shift (SS2 or SS3) */
} ISO2022State;

#define UCNV_OPTIONS_VERSION_MASK 0xf
#define UCNV_2022_MAX_CONVERTERS 10

প্রকারলেখ কাঠামো{
    UConverter_Shared_তথ্য *myConverterArray[UCNV_2022_MAX_CONVERTERS];
    UConverter *currentConverter;
    Cnv2022Type currentType;
    ISO2022State toU2022State, fromU2022State;
    uint32_t চাবি;
    uint32_t version;
#ifdef U_ENABLE_GENERIC_ISO_2022
    UBool is_First_বাফার;
#endif
    UBool isEmptySegment;
    অক্ষর name[30];
    অক্ষর locale[3];
}UConverter_তথ্য_ISO2022;

/* Protos */
/* ISO-2022 ----------------------------------------------------------------- */

/*Forward declaration */
U_CFUNC শূন্য U_CALLCONV
ucnv_fromUnicode_UTF8(UConverterFromUnicodeArgs * args,
                      UErrorCode * err);
U_CFUNC শূন্য U_CALLCONV
ucnv_fromUnicode_UTF8_OFFSETS_LOGIC(UConverterFromUnicodeArgs * args,
                                    UErrorCode * err);

#define ESC_2022 0x1B /*ESC*/

প্রকারলেখ গণনা
{
        INVALID_2022 = -1, /*Doesn't correspond to a valid iso 2022 escape sequence*/
        VALID_NON_TERMINAL_2022 = 0, /*so far corresponds to a valid iso 2022 escape sequence*/
        VALID_TERMINAL_2022 = 1, /*corresponds to a valid iso 2022 escape sequence*/
        VALID_MAYBE_TERMINAL_2022 = 2 /*so far matches one iso 2022 escape sequence, but by adding more characters might match another escape sequence*/
} UCNV_TableStates_2022;

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
স্থির ধ্রুবক int8_t normalize_esq_chars_2022[256] = {
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

#ifdef U_ENABLE_GENERIC_ISO_2022
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
#endif

#define MAX_STATES_2022 74
স্থির ধ্রুবক int32_t esc_Seq_State_Table_চাবি_2022[MAX_STATES_2022] = {
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

#ifdef U_ENABLE_GENERIC_ISO_2022

স্থির ধ্রুবক অক্ষর* ধ্রুবক esc_Seq_State_Table_ফলাফল_2022[MAX_STATES_2022] = {
 /*  0                      1                        2                      3                   4                   5                        6                      7                       8                       9    */

     nullptr                   ,nullptr                   ,nullptr                   ,nullptr               ,nullptr               ,nullptr                   ,nullptr                   ,nullptr                   ,"latin1"               ,"latin1"
    ,"latin1"               ,"ibm-865"              ,"ibm-865"              ,"ibm-865"          ,"ibm-865"          ,"ibm-865"              ,"ibm-865"              ,"JISX0201"             ,"JISX0201"             ,"latin1"
    ,"latin1"               ,nullptr                   ,"JISX-208"             ,"ibm-5478"         ,"JISX-208"         ,nullptr                   ,nullptr                   ,nullptr                   ,nullptr                   ,"UTF8"
    ,"ISO-8859-1"           ,"ISO-8859-7"           ,"JIS-X-208"            ,nullptr               ,"ibm-955"          ,"ibm-367"              ,"ibm-952"              ,"ibm-949"              ,"JISX-212"             ,"ibm-1383"
    ,"ibm-952"              ,"ibm-964"              ,"ibm-964"              ,"ibm-964"          ,"ibm-964"          ,"ibm-964"              ,"ibm-964"              ,"ibm-5478"         ,"ibm-949"              ,"ISO-IR-165"
    ,"CNS-11643-1992,1"     ,"CNS-11643-1992,2"     ,"CNS-11643-1992,3"     ,"CNS-11643-1992,4" ,"CNS-11643-1992,5" ,"CNS-11643-1992,6"     ,"CNS-11643-1992,7"     ,"UTF16_PlatformEndian" ,"UTF16_PlatformEndian" ,"UTF16_PlatformEndian"
    ,"UTF16_PlatformEndian" ,"UTF16_PlatformEndian" ,"UTF16_PlatformEndian" ,nullptr               ,"latin1"           ,"ibm-912"              ,"ibm-913"              ,"ibm-914"              ,"ibm-813"              ,"ibm-1089"
    ,"ibm-920"              ,"ibm-915"              ,"ibm-915"              ,"latin1"
};

#endif

স্থির ধ্রুবক int8_t esc_Seq_State_Table_মান_2022[MAX_STATES_2022] = {
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
প্রকারলেখ গণনা{
#ifdef U_ENABLE_GENERIC_ISO_2022
    ISO_2022=0,
#endif
    ISO_2022_JP=1,
#যদি !UCONFIG_ONLY_HTML_CONVERSION
    ISO_2022_KR=2,
    ISO_2022_CN=3
#endif
} Variant2022;

/*********** ISO 2022 Converter Protos ***********/
স্থির শূন্য U_CALLCONV
_ISO2022Open(UConverter *cnv, UConverterLoadArgs *pArgs, UErrorCode *errorCode);

স্থির শূন্য U_CALLCONV
 _ISO2022Close(UConverter *converter);

স্থির শূন্য U_CALLCONV
_ISO2022Reset(UConverter *converter, UConverterResetChoice choice);

U_CDECL_BEGIN
স্থির ধ্রুবক অক্ষর * U_CALLCONV
_ISO2022getName(ধ্রুবক UConverter* cnv);
U_CDECL_END

স্থির শূন্য  U_CALLCONV
_ISO_2022_WriteSub(UConverterFromUnicodeArgs *args, int32_t offset_সূচক, UErrorCode *err);

U_CDECL_BEGIN
স্থির UConverter * U_CALLCONV
_ISO_2022_SafeClone(ধ্রুবক UConverter *cnv, শূন্য *স্তূপ_বাফার, int32_t *p_বাফার_আকার, UErrorCode *status);

U_CDECL_END

#ifdef U_ENABLE_GENERIC_ISO_2022
স্থির শূন্য U_CALLCONV
T_UConverter_toUnicode_ISO_2022_OFFSETS_LOGIC(UConverterToUnicodeArgs* args, UErrorCode* err);
#endif

নামাকাশ {

/*const UConverterSharedData _ISO2022Data;*/
বাহ্যিক ধ্রুবক UConverter_Shared_তথ্য _ISO2022JPData;

#যদি !UCONFIG_ONLY_HTML_CONVERSION
বাহ্যিক ধ্রুবক UConverter_Shared_তথ্য _ISO2022KRData;
বাহ্যিক ধ্রুবক UConverter_Shared_তথ্য _ISO2022CNData;
#endif

}  // namespace

/*************** Converter implementations ******************/

/* The purpose of this function is to get around gcc compiler warnings. */
স্থির অন্তঃরেখ শূন্য
fromUWriteUInt8(UConverter *cnv,
                 ধ্রুবক অক্ষর *bytes, int32_t length,
                 uint8_t **target, ধ্রুবক অক্ষর *targetLimit,
                 int32_t **offsets,
                 int32_t source_সূচক,
                 UErrorCode *pErrorCode)
{
    অক্ষর* targetChars = reinterpret_cast<অক্ষর*>(*target);
    ucnv_fromUWriteBytes(cnv, bytes, length, &targetChars, targetLimit,
                         offsets, source_সূচক, pErrorCode);
    *target = reinterpret_cast<uint8_t*>(targetChars);

}

স্থির অন্তঃরেখ শূন্য
setInitialStateToUnicodeKR(UConverter* /*converter*/, UConverter_তথ্য_ISO2022 *my_Converter_তথ্য){
    যদি(my_Converter_তথ্য->version == 1) {
        UConverter *cnv = my_Converter_তথ্য->currentConverter;

        cnv->toUnicodeStatus=0;     /* offset */
        cnv->mode=0;                /* state */
        cnv->toULength=0;           /* byteIndex */
    }
}

স্থির অন্তঃরেখ শূন্য
setInitialStateFromUnicodeKR(UConverter* converter,UConverter_তথ্য_ISO2022 *my_Converter_তথ্য){
   /* in ISO-2022-KR the designator sequence appears only once
    * in a file so we append it only once
    */
    যদি( converter->char_Error_বাফার_দৈর্ঘ্য==0){

        converter->char_Error_বাফার_দৈর্ঘ্য = 4;
        converter->char_Error_বাফার[0] = 0x1b;
        converter->char_Error_বাফার[1] = 0x24;
        converter->char_Error_বাফার[2] = 0x29;
        converter->char_Error_বাফার[3] = 0x43;
    }
    যদি(my_Converter_তথ্য->version == 1) {
        UConverter *cnv = my_Converter_তথ্য->currentConverter;

        cnv->fromUChar32=0;
        cnv->fromUnicodeStatus=1;   /* prevLength */
    }
}

স্থির শূন্য U_CALLCONV
_ISO2022Open(UConverter *cnv, UConverterLoadArgs *pArgs, UErrorCode *errorCode){

    অক্ষর myLocale[7]={' ',' ',' ',' ',' ',' ', '\0'};

    cnv->extraInfo = uprv_malloc (আকার (UConverter_তথ্য_ISO2022));
    যদি(cnv->extraInfo != nullptr) {
        UConverterNamePieces স্তূপ_Pieces;
        UConverterLoadArgs স্তূপ_Args=UCNV_LOAD_ARGS_INITIALIZER;
        UConverter_তথ্য_ISO2022* my_Converter_তথ্য = static_cast<UConverter_তথ্য_ISO2022*>(cnv->extraInfo);
        uint32_t version;

        স্তূপ_Args.onlyTestIsLoadable = pArgs->onlyTestIsLoadable;

        uprv_memset(my_Converter_তথ্য, 0, আকার(UConverter_তথ্য_ISO2022));
        my_Converter_তথ্য->currentType = ASCII1;
        cnv->fromUnicodeStatus =মিথ্যা;
        যদি(pArgs->locale){
            uprv_strncpy(myLocale, pArgs->locale, আকার(myLocale)-1);
        }
        version = pArgs->options & UCNV_OPTIONS_VERSION_MASK;
        my_Converter_তথ্য->version = version;
        যদি(myLocale[0]=='j' && (myLocale[1]=='a'|| myLocale[1]=='p') &&
            (myLocale[2]=='_' || myLocale[2]=='\0'))
        {
            /* open the required converters and cache them */
            যদি(version>MAX_JA_VERSION) {
                // ICU 55 fails to open a converter for an unsupported version.
                // Previously, it fell back to version 0, but that would yield
                // unexpected behavior.
                *errorCode = U_MISSING_RESOURCE_ERROR;
                ফেরত;
            }
            যদি(jpCharsetMasks[version]&CSM(ISO8859_7)) {
                my_Converter_তথ্য->myConverterArray[ISO8859_7] =
                    ucnv_load_Shared_তথ্য("ISO8859_7", &স্তূপ_Pieces, &স্তূপ_Args, errorCode);
            }
            my_Converter_তথ্য->myConverterArray[JISX208] =
                ucnv_load_Shared_তথ্য("Shift-JIS", &স্তূপ_Pieces, &স্তূপ_Args, errorCode);
            যদি(jpCharsetMasks[version]&CSM(JISX212)) {
                my_Converter_তথ্য->myConverterArray[JISX212] =
                    ucnv_load_Shared_তথ্য("jisx-212", &স্তূপ_Pieces, &স্তূপ_Args, errorCode);
            }
            যদি(jpCharsetMasks[version]&CSM(GB2312)) {
                my_Converter_তথ্য->myConverterArray[GB2312] =
                    ucnv_load_Shared_তথ্য("ibm-5478", &স্তূপ_Pieces, &স্তূপ_Args, errorCode);   /* gb_2312_80-1 */
            }
            যদি(jpCharsetMasks[version]&CSM(KSC5601)) {
                my_Converter_তথ্য->myConverterArray[KSC5601] =
                    ucnv_load_Shared_তথ্য("ksc_5601", &স্তূপ_Pieces, &স্তূপ_Args, errorCode);
            }

            /* set the function pointers to appropriate functions */
            cnv->shared_তথ্য = const_cast<UConverter_Shared_তথ্য*>(&_ISO2022JPData);
            uprv_strcpy(my_Converter_তথ্য->locale,"ja");

            (শূন্য)uprv_strcpy(my_Converter_তথ্য->name,"ISO_2022,locale=ja,version=");
            size_t len = uprv_strlen(my_Converter_তথ্য->name);
            my_Converter_তথ্য->name[len] = static_cast<অক্ষর>(my_Converter_তথ্য->version + static_cast<পূর্ণসংখ্যা>('0'));
            my_Converter_তথ্য->name[len+1]='\0';
        }
#যদি !UCONFIG_ONLY_HTML_CONVERSION
        অন্যথা যদি(myLocale[0]=='k' && (myLocale[1]=='o'|| myLocale[1]=='r') &&
            (myLocale[2]=='_' || myLocale[2]=='\0'))
        {
            যদি(version>1) {
                // ICU 55 fails to open a converter for an unsupported version.
                // Previously, it fell back to version 0, but that would yield
                // unexpected behavior.
                *errorCode = U_MISSING_RESOURCE_ERROR;
                ফেরত;
            }
            ধ্রুবক অক্ষর *cnvName;
            যদি(version==1) {
                cnvName="icu-internal-25546";
            } অন্যথা {
                cnvName="ibm-949";
                my_Converter_তথ্য->version=version=0;
            }
            যদি(pArgs->onlyTestIsLoadable) {
                ucnv_canCreateConverter(cnvName, errorCode);  /* errorCode carries result */
                uprv_free(cnv->extraInfo);
                cnv->extraInfo=nullptr;
                ফেরত;
            } অন্যথা {
                my_Converter_তথ্য->currentConverter=ucnv_open(cnvName, errorCode);
                যদি (U_FAILURE(*errorCode)) {
                    _ISO2022Close(cnv);
                    ফেরত;
                }

                যদি(version==1) {
                    (শূন্য)uprv_strcpy(my_Converter_তথ্য->name,"ISO_2022,locale=ko,version=1");
                    uprv_memcpy(cnv->subChars, my_Converter_তথ্য->currentConverter->subChars, 4);
                    cnv->subCharLen = my_Converter_তথ্য->currentConverter->subCharLen;
                }অন্যথা{
                    (শূন্য)uprv_strcpy(my_Converter_তথ্য->name,"ISO_2022,locale=ko,version=0");
                }

                /* initialize the state variables */
                setInitialStateToUnicodeKR(cnv, my_Converter_তথ্য);
                setInitialStateFromUnicodeKR(cnv, my_Converter_তথ্য);

                /* set the function pointers to appropriate functions */
                cnv->shared_তথ্য = const_cast<UConverter_Shared_তথ্য*>(&_ISO2022KRData);
                uprv_strcpy(my_Converter_তথ্য->locale,"ko");
            }
        }
        অন্যথা যদি(((myLocale[0]=='z' && myLocale[1]=='h') || (myLocale[0]=='c'&& myLocale[1]=='n'))&&
            (myLocale[2]=='_' || myLocale[2]=='\0'))
        {
            যদি(version>2) {
                // ICU 55 fails to open a converter for an unsupported version.
                // Previously, it fell back to version 0, but that would yield
                // unexpected behavior.
                *errorCode = U_MISSING_RESOURCE_ERROR;
                ফেরত;
            }

            /* open the required converters and cache them */
            my_Converter_তথ্য->myConverterArray[GB2312_1] =
                ucnv_load_Shared_তথ্য("ibm-5478", &স্তূপ_Pieces, &স্তূপ_Args, errorCode);
            যদি(version>=1) {
                my_Converter_তথ্য->myConverterArray[ISO_IR_165] =
                    ucnv_load_Shared_তথ্য("iso-ir-165", &স্তূপ_Pieces, &স্তূপ_Args, errorCode);
            }
            my_Converter_তথ্য->myConverterArray[CNS_11643] =
                ucnv_load_Shared_তথ্য("cns-11643-1992", &স্তূপ_Pieces, &স্তূপ_Args, errorCode);


            /* set the function pointers to appropriate functions */
            cnv->shared_তথ্য = const_cast<UConverter_Shared_তথ্য*>(&_ISO2022CNData);
            uprv_strcpy(my_Converter_তথ্য->locale,"cn");

            যদি (version==0){
                my_Converter_তথ্য->version = 0;
                (শূন্য)uprv_strcpy(my_Converter_তথ্য->name,"ISO_2022,locale=zh,version=0");
            }অন্যথা যদি (version==1){
                my_Converter_তথ্য->version = 1;
                (শূন্য)uprv_strcpy(my_Converter_তথ্য->name,"ISO_2022,locale=zh,version=1");
            }অন্যথা {
                my_Converter_তথ্য->version = 2;
                (শূন্য)uprv_strcpy(my_Converter_তথ্য->name,"ISO_2022,locale=zh,version=2");
            }
        }
#endif  // !UCONFIG_ONLY_HTML_CONVERSION
        অন্যথা{
#ifdef U_ENABLE_GENERIC_ISO_2022
            my_Converter_তথ্য->is_First_বাফার = সত্য;

            /* append the UTF-8 escape sequence */
            cnv->char_Error_বাফার_দৈর্ঘ্য = 3;
            cnv->char_Error_বাফার[0] = 0x1b;
            cnv->char_Error_বাফার[1] = 0x25;
            cnv->char_Error_বাফার[2] = 0x42;

            cnv->shared_তথ্য=(UConverter_Shared_তথ্য*)&ISO2022_তথ্য;
            /* initialize the state variables */
            uprv_strcpy(my_Converter_তথ্য->name,"ISO_2022");
#অন্যথা
            *errorCode = U_MISSING_RESOURCE_ERROR;
            // Was U_UNSUPPORTED_ERROR but changed in ICU 55 to a more standard
            // data loading error code.
            ফেরত;
#endif
        }

        cnv->maxBytesPerUChar=cnv->shared_তথ্য->static_তথ্য->maxBytesPerChar;

        যদি(U_FAILURE(*errorCode) || pArgs->onlyTestIsLoadable) {
            _ISO2022Close(cnv);
        }
    } অন্যথা {
        *errorCode = U_MEMORY_ALLOCATION_ERROR;
    }
}


স্থির শূন্য U_CALLCONV
_ISO2022Close(UConverter *converter) {
    UConverter_তথ্য_ISO2022* my_তথ্য = static_cast<UConverter_তথ্য_ISO2022*>(converter->extraInfo);
    UConverter_Shared_তথ্য **array = my_তথ্য->myConverterArray;
    int32_t i;

    যদি (converter->extraInfo != nullptr) {
        /*close the array of converter pointers and free the memory*/
        জন্য (i=0; i<UCNV_2022_MAX_CONVERTERS; i++) {
            যদি(array[i]!=nullptr) {
                ucnv_unload_Shared_তথ্য_If_Ready(array[i]);
            }
        }

        ucnv_close(my_তথ্য->currentConverter);

        যদি(!converter->isExtraLocal){
            uprv_free (converter->extraInfo);
            converter->extraInfo = nullptr;
        }
    }
}

স্থির শূন্য U_CALLCONV
_ISO2022Reset(UConverter *converter, UConverterResetChoice choice) {
    UConverter_তথ্য_ISO2022* my_Converter_তথ্য = static_cast<UConverter_তথ্য_ISO2022*>(converter->extraInfo);
    যদি(choice<=UCNV_RESET_TO_UNICODE) {
        uprv_memset(&my_Converter_তথ্য->toU2022State, 0, আকার(ISO2022State));
        my_Converter_তথ্য->চাবি = 0;
        my_Converter_তথ্য->isEmptySegment = মিথ্যা;
    }
    যদি(choice!=UCNV_RESET_TO_UNICODE) {
        uprv_memset(&my_Converter_তথ্য->fromU2022State, 0, আকার(ISO2022State));
    }
#ifdef U_ENABLE_GENERIC_ISO_2022
    যদি(my_Converter_তথ্য->locale[0] == 0){
        যদি(choice<=UCNV_RESET_TO_UNICODE) {
            my_Converter_তথ্য->is_First_বাফার = সত্য;
            my_Converter_তথ্য->চাবি = 0;
            যদি (converter->mode == UCNV_SO){
                ucnv_close (my_Converter_তথ্য->currentConverter);
                my_Converter_তথ্য->currentConverter=nullptr;
            }
            converter->mode = UCNV_SI;
        }
        যদি(choice!=UCNV_RESET_TO_UNICODE) {
            /* re-append UTF-8 escape sequence */
            converter->char_Error_বাফার_দৈর্ঘ্য = 3;
            converter->char_Error_বাফার[0] = 0x1b;
            converter->char_Error_বাফার[1] = 0x28;
            converter->char_Error_বাফার[2] = 0x42;
        }
    }
    অন্যথা
#endif
    {
        /* reset the state variables */
        যদি(my_Converter_তথ্য->locale[0] == 'k'){
            যদি(choice<=UCNV_RESET_TO_UNICODE) {
                setInitialStateToUnicodeKR(converter, my_Converter_তথ্য);
            }
            যদি(choice!=UCNV_RESET_TO_UNICODE) {
                setInitialStateFromUnicodeKR(converter, my_Converter_তথ্য);
            }
        }
    }
}

U_CDECL_BEGIN

স্থির ধ্রুবক অক্ষর * U_CALLCONV
_ISO2022getName(ধ্রুবক UConverter* cnv){
    যদি(cnv->extraInfo){
        UConverter_তথ্য_ISO2022* my_তথ্য= (UConverter_তথ্য_ISO2022*)cnv->extraInfo;
        ফেরত my_তথ্য->name;
    }
    ফেরত nullptr;
}

U_CDECL_END


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
স্থির ধ্রুবক int8_t পরবর্তী_State_To_Unicode_JP[MAX_STATES_2022]= {
/*      0                1               2               3               4               5               6               7               8               9    */
    INVALID_STATE   ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,SS2_STATE      ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE
    ,ASCII          ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,JISX201        ,HWKANA_7BIT    ,JISX201        ,INVALID_STATE
    ,INVALID_STATE  ,INVALID_STATE  ,JISX208        ,GB2312         ,JISX208        ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE
    ,ISO8859_1      ,ISO8859_7      ,JISX208        ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,KSC5601        ,JISX212        ,INVALID_STATE
    ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE
    ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE
    ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE
    ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE
};

#যদি !UCONFIG_ONLY_HTML_CONVERSION
/*************** to unicode *******************/
স্থির ধ্রুবক int8_t পরবর্তী_State_To_Unicode_CN[MAX_STATES_2022]= {
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
#endif


স্থির UCNV_TableStates_2022
get_চাবি_2022(অক্ষর c,int32_t* চাবি,int32_t* offset){
    int32_t togo;
    int32_t নিচের = 0;
    int32_t hi = MAX_STATES_2022;
    int32_t oldmid=0;

    togo = normalize_esq_chars_2022[static_cast<uint8_t>(c)];
    যদি(togo == 0) {
        /* not a valid character anywhere in an escape sequence */
        *চাবি = 0;
        *offset = 0;
        ফেরত INVALID_2022;
    }
    togo = (*চাবি << 5) + togo;

    যখন (hi != নিচের)  /*দ্বিমিক অনুসন্ধান*/{

        int32_t mid = (hi+নিচের) >> 1; /*Finds median*/

        যদি (mid == oldmid)
            বিরতি;

        যদি (esc_Seq_State_Table_চাবি_2022[mid] > togo){
            hi = mid;
        }
        অন্যথা যদি (esc_Seq_State_Table_চাবি_2022[mid] < togo){
            নিচের = mid;
        }
        অন্যথা /*we found it*/{
            *চাবি = togo;
            *offset = mid;
            ফেরত static_cast<UCNV_TableStates_2022>(esc_Seq_State_Table_মান_2022[mid]);
        }
        oldmid = mid;

    }

    *চাবি = 0;
    *offset = 0;
    ফেরত INVALID_2022;
}

/*runs through a state machine to determine the escape sequence - codepage correspondence
 */
স্থির শূন্য
changeState_2022(UConverter* _this,
                ধ্রুবক অক্ষর** source,
                ধ্রুবক অক্ষর* sourceLimit,
                Variant2022 var,
                UErrorCode* err){
    UCNV_TableStates_2022 value;
    UConverter_তথ্য_ISO2022* myData2022 = static_cast<UConverter_তথ্য_ISO2022*>(_this->extraInfo);
    uint32_t চাবি = myData2022->চাবি;
    int32_t offset = 0;
    int8_t initialToULength = _this->toULength;
    অক্ষর c;

    value = VALID_NON_TERMINAL_2022;
    যখন (*source < sourceLimit) {
        c = *(*source)++;
        _this->toUBytes[_this->toULength++] = static_cast<uint8_t>(c);
        value = get_চাবি_2022(c, reinterpret_cast<int32_t*>(&চাবি), &offset);

        নির্বাচন (value){

        ক্ষেত্র VALID_NON_TERMINAL_2022 :
            /* continue with the loop */
            বিরতি;

        ক্ষেত্র VALID_TERMINAL_2022:
            চাবি = 0;
            যাও DONE;

        ক্ষেত্র INVALID_2022:
            যাও DONE;

        ক্ষেত্র VALID_MAYBE_TERMINAL_2022:
#ifdef U_ENABLE_GENERIC_ISO_2022
            /* ESC ( B is ambiguous only for ISO_2022 itself */
            যদি(var == ISO_2022) {
                /* discard toUBytes[] for ESC ( B because this sequence is correct and complete */
                _this->toULength = 0;

                /* TODO need to indicate that ESC ( B was seen; if failure, then need to replay from source or from MBCS-style replay */

                /* continue with the loop */
                value = VALID_NON_TERMINAL_2022;
                বিরতি;
            } অন্যথা
#endif
            {
                /* not ISO_2022 itself, finish here */
                value = VALID_TERMINAL_2022;
                চাবি = 0;
                যাও DONE;
            }
        }
    }

DONE:
    myData2022->চাবি = চাবি;

    যদি (value == VALID_NON_TERMINAL_2022) {
        /* indicate that the escape sequence is incomplete: key!=0 */
        ফেরত;
    } অন্যথা যদি (value == INVALID_2022 ) {
        *err = U_ILLEGAL_ESCAPE_SEQUENCE;
    } অন্যথা /* value == VALID_TERMINAL_2022 */ {
        নির্বাচন(var){
#ifdef U_ENABLE_GENERIC_ISO_2022
        ক্ষেত্র ISO_2022:
        {
            ধ্রুবক অক্ষর *chosenConverterName = esc_Seq_State_Table_ফলাফল_2022[offset];
            যদি(chosenConverterName == nullptr) {
                /* SS2 or SS3 */
                *err = U_UNSUPPORTED_ESCAPE_SEQUENCE;
                _this->toUCallbackReason = UCNV_UNASSIGNED;
                ফেরত;
            }

            _this->mode = UCNV_SI;
            ucnv_close(myData2022->currentConverter);
            myData2022->currentConverter = myUConverter = ucnv_open(chosenConverterName, err);
            যদি(U_SUCCESS(*err)) {
                myUConverter->fromCharErrorBehaviour = UCNV_TO_U_CALLBACK_STOP;
                _this->mode = UCNV_SO;
            }
            বিরতি;
        }
#endif
        ক্ষেত্র ISO_2022_JP:
            {
                StateEnum অস্থায়ী_State = static_cast<StateEnum>(পরবর্তী_State_To_Unicode_JP[offset]);
                নির্বাচন(অস্থায়ী_State) {
                ক্ষেত্র INVALID_STATE:
                    *err = U_UNSUPPORTED_ESCAPE_SEQUENCE;
                    বিরতি;
                ক্ষেত্র SS2_STATE:
                    যদি(myData2022->toU2022State.cs[2]!=0) {
                        যদি(myData2022->toU2022State.g<2) {
                            myData2022->toU2022State.পূর্ব_G=myData2022->toU2022State.g;
                        }
                        myData2022->toU2022State.g=2;
                    } অন্যথা {
                        /* illegal to have SS2 before a matching designator */
                        *err = U_ILLEGAL_ESCAPE_SEQUENCE;
                    }
                    বিরতি;
                /* case SS3_STATE: not used in ISO-2022-JP-x */
                ক্ষেত্র ISO8859_1:
                ক্ষেত্র ISO8859_7:
                    যদি((jpCharsetMasks[myData2022->version] & CSM(অস্থায়ী_State)) == 0) {
                        *err = U_UNSUPPORTED_ESCAPE_SEQUENCE;
                    } অন্যথা {
                        /* G2 charset for SS2 */
                        myData2022->toU2022State.cs[2] = static_cast<int8_t>(অস্থায়ী_State);
                    }
                    বিরতি;
                পূর্বনির্ধারিত:
                    যদি((jpCharsetMasks[myData2022->version] & CSM(অস্থায়ী_State)) == 0) {
                        *err = U_UNSUPPORTED_ESCAPE_SEQUENCE;
                    } অন্যথা {
                        /* G0 charset */
                        myData2022->toU2022State.cs[0] = static_cast<int8_t>(অস্থায়ী_State);
                    }
                    বিরতি;
                }
            }
            বিরতি;
#যদি !UCONFIG_ONLY_HTML_CONVERSION
        ক্ষেত্র ISO_2022_CN:
            {
                StateEnum অস্থায়ী_State = static_cast<StateEnum>(পরবর্তী_State_To_Unicode_CN[offset]);
                নির্বাচন(অস্থায়ী_State) {
                ক্ষেত্র INVALID_STATE:
                    *err = U_UNSUPPORTED_ESCAPE_SEQUENCE;
                    বিরতি;
                ক্ষেত্র SS2_STATE:
                    যদি(myData2022->toU2022State.cs[2]!=0) {
                        যদি(myData2022->toU2022State.g<2) {
                            myData2022->toU2022State.পূর্ব_G=myData2022->toU2022State.g;
                        }
                        myData2022->toU2022State.g=2;
                    } অন্যথা {
                        /* illegal to have SS2 before a matching designator */
                        *err = U_ILLEGAL_ESCAPE_SEQUENCE;
                    }
                    বিরতি;
                ক্ষেত্র SS3_STATE:
                    যদি(myData2022->toU2022State.cs[3]!=0) {
                        যদি(myData2022->toU2022State.g<2) {
                            myData2022->toU2022State.পূর্ব_G=myData2022->toU2022State.g;
                        }
                        myData2022->toU2022State.g=3;
                    } অন্যথা {
                        /* illegal to have SS3 before a matching designator */
                        *err = U_ILLEGAL_ESCAPE_SEQUENCE;
                    }
                    বিরতি;
                ক্ষেত্র ISO_IR_165:
                    যদি(myData2022->version==0) {
                        *err = U_UNSUPPORTED_ESCAPE_SEQUENCE;
                        বিরতি;
                    }
                    U_FALLTHROUGH;
                ক্ষেত্র GB2312_1:
                    U_FALLTHROUGH;
                ক্ষেত্র CNS_11643_1:
                    myData2022->toU2022State.cs[1] = static_cast<int8_t>(অস্থায়ী_State);
                    বিরতি;
                ক্ষেত্র CNS_11643_2:
                    myData2022->toU2022State.cs[2] = static_cast<int8_t>(অস্থায়ী_State);
                    বিরতি;
                পূর্বনির্ধারিত:
                    /* other CNS 11643 planes */
                    যদি(myData2022->version==0) {
                        *err = U_UNSUPPORTED_ESCAPE_SEQUENCE;
                    } অন্যথা {
                        myData2022->toU2022State.cs[3] = static_cast<int8_t>(অস্থায়ী_State);
                    }
                    বিরতি;
                }
            }
            বিরতি;
        ক্ষেত্র ISO_2022_KR:
            যদি(offset==0x30){
                /* nothing to be done, just accept this one escape sequence */
            } অন্যথা {
                *err = U_UNSUPPORTED_ESCAPE_SEQUENCE;
            }
            বিরতি;
#endif  // !UCONFIG_ONLY_HTML_CONVERSION

        পূর্বনির্ধারিত:
            *err = U_ILLEGAL_ESCAPE_SEQUENCE;
            বিরতি;
        }
    }
    যদি(U_SUCCESS(*err)) {
        _this->toULength = 0;
    } অন্যথা যদি(*err==U_ILLEGAL_ESCAPE_SEQUENCE) {
        যদি(_this->toULength>1) {
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
            int8_t backOutDistance=_this->toULength-1;
            int8_t bytes_From_This_বাফার=_this->toULength-initialToULength;
            যদি(backOutDistance<=bytes_From_This_বাফার) {
                /* same as initialToULength<=1 */
                *source-=backOutDistance;
            } অন্যথা {
                /* Back out bytes from the previous buffer: Need to replay them. */
                _this->preToULength = static_cast<int8_t>(bytes_From_This_বাফার - backOutDistance);
                /* same as -(initialToULength-1) */
                /* preToULength is negative! */
                uprv_memcpy(_this->preToU, _this->toUBytes+1, -_this->preToULength);
                *source-=bytes_From_This_বাফার;
            }
            _this->toULength=1;
        }
    } অন্যথা যদি(*err==U_UNSUPPORTED_ESCAPE_SEQUENCE) {
        _this->toUCallbackReason = UCNV_UNASSIGNED;
    }
}

#যদি !UCONFIG_ONLY_HTML_CONVERSION
/*Checks the characters of the buffer against valid 2022 escape sequences
*if the match we return a pointer to the initial start of the sequence otherwise
*we return sourceLimit
*/
/*for 2022 looks ahead in the stream
 *to determine the longest possible convertible
 *data stream
 */
স্থির অন্তঃরেখ ধ্রুবক অক্ষর*
get_End_Of_বাফার_2022(ধ্রুবক অক্ষর** source,
                   ধ্রুবক অক্ষর* sourceLimit,
                   UBool /*flush*/){

    ধ্রুবক অক্ষর* mySource = *source;

#ifdef U_ENABLE_GENERIC_ISO_2022
    যদি (*source >= sourceLimit)
        ফেরত sourceLimit;

    করো{

        যদি (*mySource == ESC_2022){
            int8_t i;
            int32_t চাবি = 0;
            int32_t offset;
            UCNV_TableStates_2022 value = VALID_NON_TERMINAL_2022;

            /* Kludge: I could not
            * figure out the reason for validating an escape sequence
            * twice - once here and once in changeState_2022().
            * is it possible to have an ESC character in a ISO2022
            * byte stream which is valid in a code page? Is it legal?
            */
            জন্য (i=0;
            (mySource+i < sourceLimit)&&(value == VALID_NON_TERMINAL_2022);
            i++) {
                value =  get_চাবি_2022(*(mySource+i), &চাবি, &offset);
            }
            যদি (value > 0 || *mySource==ESC_2022)
                ফেরত mySource;

            যদি ((value == VALID_NON_TERMINAL_2022)&&(!flush) )
                ফেরত sourceLimit;
        }
    }যখন (++mySource < sourceLimit);

    ফেরত sourceLimit;
#অন্যথা
    যখন(mySource < sourceLimit && *mySource != ESC_2022) {
        ++mySource;
    }
    ফেরত mySource;
#endif
}
#endif

/* This inline function replicates code in _MBCSFromUChar32() function in ucnvmbcs.c
 * any future change in _MBCSFromUChar32() function should be reflected here.
 * @return number of bytes in *value; negative number if fallback; 0 if no mapping
 */
স্থির অন্তঃরেখ int32_t
MBCS_FROM_UCHAR32_ISO2022(UConverter_Shared_তথ্য* shared_তথ্য,
                                         UChar32 c,
                                         uint32_t* value,
                                         UBool useFallback,
                                         পূর্ণসংখ্যা outputType)
{
    ধ্রুবক int32_t *cx;
    ধ্রুবক uint16_t *table;
    uint32_t stage2Entry;
    uint32_t my_মান;
    int32_t length;
    ধ্রুবক uint8_t *p;
    /*
     * TODO(markus): Use and require new, faster MBCS conversion table structures.
     * Use internal version of ucnv_open() that verifies that the new structures are available,
     * else U_INTERNAL_PROGRAM_ERROR.
     */
    /* BMP-only codepages are stored without stage 1 entries for supplementary code points */
    যদি(c<0x10000 || (shared_তথ্য->mbcs.unicodeMask&UCNV_HAS_SUPPLEMENTARY)) {
        table=shared_তথ্য->mbcs.fromUnicodeTable;
        stage2Entry=MBCS_STAGE_2_FROM_U(table, c);
        /* get the bytes and the length for the output */
        যদি(outputType==MBCS_OUTPUT_2){
            my_মান=MBCS_মান_2_FROM_STAGE_2(shared_তথ্য->mbcs.fromUnicodeBytes, stage2Entry, c);
            যদি(my_মান<=0xff) {
                length=1;
            } অন্যথা {
                length=2;
            }
        } অন্যথা /* outputType==MBCS_OUTPUT_3 */ {
            p=MBCS_POINTER_3_FROM_STAGE_2(shared_তথ্য->mbcs.fromUnicodeBytes, stage2Entry, c);
            my_মান = (static_cast<uint32_t>(*p) << 16) | (static_cast<uint32_t>(p[1]) << 8) | p[2];
            যদি(my_মান<=0xff) {
                length=1;
            } অন্যথা যদি(my_মান<=0xffff) {
                length=2;
            } অন্যথা {
                length=3;
            }
        }
        /* is this code point assigned, or do we use fallbacks? */
        যদি((stage2Entry&(1<<(16+(c&0xf))))!=0) {
            /* assigned */
            *value=my_মান;
            ফেরত length;
        } অন্যথা যদি(FROM_U_USE_FALLBACK(useFallback, c) && my_মান!=0) {
            /*
             * We allow a 0 byte output if the "assigned" bit is set for this entry.
             * There is no way with this data structure for fallback output
             * to be a zero byte.
             */
            *value=my_মান;
            ফেরত -length;
        }
    }

    cx=shared_তথ্য->mbcs.extIndexes;
    যদি(cx!=nullptr) {
        ফেরত ucnv_extSimpleMatchFromU(cx, c, value, useFallback);
    }

    /* unassigned */
    ফেরত 0;
}

/* This inline function replicates code in _MBCSSingleFromUChar32() function in ucnvmbcs.c
 * any future change in _MBCSSingleFromUChar32() function should be reflected here.
 * @param retval pointer to output byte
 * @return 1 roundtrip byte  0 no mapping  -1 fallback byte
 */
স্থির অন্তঃরেখ int32_t
MBCS_SINGLE_FROM_UCHAR32(UConverter_Shared_তথ্য* shared_তথ্য,
                                       UChar32 c,
                                       uint32_t* retval,
                                       UBool useFallback)
{
    ধ্রুবক uint16_t *table;
    int32_t value;
    /* BMP-only codepages are stored without stage 1 entries for supplementary code points */
    যদি(c>=0x10000 && !(shared_তথ্য->mbcs.unicodeMask&UCNV_HAS_SUPPLEMENTARY)) {
        ফেরত 0;
    }
    /* convert the Unicode code point in c into codepage bytes (same as in _MBCSFromUnicodeWithOffsets) */
    table=shared_তথ্য->mbcs.fromUnicodeTable;
    /* get the byte for the output */
    value=MBCS_SINGLE_ফলাফল_FROM_U(table, (uint16_t *)shared_তথ্য->mbcs.fromUnicodeBytes, c);
    /* is this code point assigned, or do we use fallbacks? */
    *retval = static_cast<uint32_t>(value & 0xff);
    যদি(value>=0xf00) {
        ফেরত 1;  /* roundtrip */
    } অন্যথা যদি(useFallback ? value>=0x800 : value>=0xc00) {
        ফেরত -1;  /* fallback taken */
    } অন্যথা {
        ফেরত 0;  /* no mapping */
    }
}

/*
 * Check that the result is a 2-byte value with each byte in the range A1..FE
 * (strict EUC DBCS) before accepting it and subtracting 0x80 from each byte
 * to move it to the ISO 2022 range 21..7E.
 * Return 0 if out of range.
 */
স্থির অন্তঃরেখ uint32_t
_2022FromGR94DBCS(uint32_t value) {
    যদি (static_cast<uint16_t>(value - 0xa1a1) <= (0xfefe - 0xa1a1) &&
        static_cast<uint8_t>(value - 0xa1) <= (0xfe - 0xa1)
    ) {
        ফেরত value - 0x8080;  /* shift down to 21..7e byte range */
    } অন্যথা {
        ফেরত 0;  /* not valid for ISO 2022 */
    }
}

#যদি 0 /* 5691: Call sites now check for validity. They can just += 0x8080 after that. */
/*
 * This method does the reverse of _2022FromGR94DBCS(). Given the 2022 code point, it returns the
 * 2 byte value that is in the range A1..FE for each byte. Otherwise it returns the 2022 code point
 * unchanged. 
 */
স্থির অন্তঃরেখ uint32_t
_2022ToGR94DBCS(uint32_t value) {
    uint32_t return_মান = value + 0x8080;
    যদি( (uint16_t)(return_মান - 0xa1a1) <= (0xfefe - 0xa1a1) &&
        (uint8_t)(return_মান - 0xa1) <= (0xfe - 0xa1)) {
        ফেরত return_মান;
    } অন্যথা {
        ফেরত value;
    }
}
#endif

#ifdef U_ENABLE_GENERIC_ISO_2022

/**********************************************************************************
*  ISO-2022 Converter
*
*
*/

স্থির শূন্য U_CALLCONV
T_UConverter_toUnicode_ISO_2022_OFFSETS_LOGIC(UConverterToUnicodeArgs* args,
                                                           UErrorCode* err){
    ধ্রুবক অক্ষর* mySourceLimit, *realSourceLimit;
    ধ্রুবক অক্ষর* sourceStart;
    ধ্রুবক char16_t* myTargetStart;
    UConverter* saveThis;
    UConverter_তথ্য_ISO2022* my_তথ্য;
    int8_t length;

    saveThis = args->converter;
    my_তথ্য=((UConverter_তথ্য_ISO2022*)(saveThis->extraInfo));

    realSourceLimit = args->sourceLimit;
    যখন (args->source < realSourceLimit) {
        যদি(my_তথ্য->চাবি == 0) { /* are we in the middle of an escape sequence? */
            /*Find the end of the buffer e.g : Next Escape Seq | end of Buffer*/
            mySourceLimit = get_End_Of_বাফার_2022(&(args->source), realSourceLimit, args->flush);

            যদি(args->source < mySourceLimit) {
                যদি(my_তথ্য->currentConverter==nullptr) {
                    my_তথ্য->currentConverter = ucnv_open("ASCII",err);
                    যদি(U_FAILURE(*err)){
                        ফেরত;
                    }

                    my_তথ্য->currentConverter->fromCharErrorBehaviour = UCNV_TO_U_CALLBACK_STOP;
                    saveThis->mode = UCNV_SO;
                }

                /* convert to before the ESC or until the end of the buffer */
                my_তথ্য->is_First_বাফার=মিথ্যা;
                sourceStart = args->source;
                myTargetStart = args->target;
                args->converter = my_তথ্য->currentConverter;
                ucnv_toUnicode(args->converter,
                    &args->target,
                    args->targetLimit,
                    &args->source,
                    mySourceLimit,
                    args->offsets,
                    (UBool)(args->flush && mySourceLimit == realSourceLimit),
                    err);
                args->converter = saveThis;

                যদি (*err == U_বাফার_OVERFLOW_ERROR) {
                    /* move the overflow buffer */
                    length = saveThis->UChar_Error_বাফার_দৈর্ঘ্য = my_তথ্য->currentConverter->UChar_Error_বাফার_দৈর্ঘ্য;
                    my_তথ্য->currentConverter->UChar_Error_বাফার_দৈর্ঘ্য = 0;
                    যদি(length > 0) {
                        uprv_memcpy(saveThis->UChar_Error_বাফার,
                                    my_তথ্য->currentConverter->UChar_Error_বাফার,
                                    length*U_SIZEOF_UCHAR);
                    }
                    ফেরত;
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
                যদি (U_FAILURE(*err) ||
                    (args->source == realSourceLimit) ||
                    (args->offsets != nullptr && (args->target != myTargetStart || args->source != sourceStart) ||
                    (mySourceLimit < realSourceLimit && my_তথ্য->currentConverter->toULength > 0))
                ) {
                    /* copy partial or error input for truncated detection and error handling */
                    যদি(U_FAILURE(*err)) {
                        length = saveThis->invalid_Char_দৈর্ঘ্য = my_তথ্য->currentConverter->invalid_Char_দৈর্ঘ্য;
                        যদি(length > 0) {
                            uprv_memcpy(saveThis->invalid_Char_বাফার, my_তথ্য->currentConverter->invalid_Char_বাফার, length);
                        }
                    } অন্যথা {
                        length = saveThis->toULength = my_তথ্য->currentConverter->toULength;
                        যদি(length > 0) {
                            uprv_memcpy(saveThis->toUBytes, my_তথ্য->currentConverter->toUBytes, length);
                            যদি(args->source < mySourceLimit) {
                                *err = U_TRUNCATED_CHAR_FOUND; /* truncated input before ESC */
                            }
                        }
                    }
                    ফেরত;
                }
            }
        }

        sourceStart = args->source;
        changeState_2022(args->converter,
               &(args->source),
               realSourceLimit,
               ISO_2022,
               err);
        যদি (U_FAILURE(*err) || (args->source != sourceStart && args->offsets != nullptr)) {
            /* let the ucnv.c code update its current offset */
            ফেরত;
        }
    }
}

#endif

/*
 * To Unicode Callback helper function
 */
স্থির শূন্য
toUnicodeCallback(UConverter *cnv,
                  ধ্রুবক uint32_t sourceChar, ধ্রুবক uint32_t targetUniChar,
                  UErrorCode* err){
    যদি(sourceChar>0xff){
        cnv->toUBytes[0] = static_cast<uint8_t>(sourceChar >> 8);
        cnv->toUBytes[1] = static_cast<uint8_t>(sourceChar);
        cnv->toULength = 2;
    }
    অন্যথা{
        cnv->toUBytes[0] = static_cast<অক্ষর>(sourceChar);
        cnv->toULength = 1;
    }

    যদি(targetUniChar == (missingCharMarker-1/*0xfffe*/)){
        *err = U_INVALID_CHAR_FOUND;
    }
    অন্যথা{
        *err = U_ILLEGAL_CHAR_FOUND;
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
স্থির ধ্রুবক StateEnum jpCharsetPref[]={
    ASCII,
    JISX201,
    ISO8859_1,
    JISX208,
    ISO8859_7,
    JISX212,
    GB2312,
    KSC5601,
    HWKANA_7BIT
};

/*
 * The escape sequences must be in order of the enum constants like JISX201  = 3,
 * not in order of jpCharsetPref[]!
 */
স্থির ধ্রুবক অক্ষর escSeqChars[][6] ={
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
স্থির  ধ্রুবক int8_t escSeqCharsLen[] ={
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
স্থির অন্তঃরেখ uint32_t
jisx201ToU(uint32_t value) {
    যদি(value < 0x5c) {
        ফেরত value;
    } অন্যথা যদি(value == 0x5c) {
        ফেরত 0xa5;
    } অন্যথা যদি(value == 0x7e) {
        ফেরত 0x203e;
    } অন্যথা /* value <= 0x7f */ {
        ফেরত value;
    }
}

/* Map Unicode to 00..7F according to JIS X 0201. Return U+FFFE if unmappable. */
স্থির অন্তঃরেখ uint32_t
jisx201FromU(uint32_t value) {
    যদি(value<=0x7f) {
        যদি(value!=0x5c && value!=0x7e) {
            ফেরত value;
        }
    } অন্যথা যদি(value==0xa5) {
        ফেরত 0x5c;
    } অন্যথা যদি(value==0x203e) {
        ফেরত 0x7e;
    }
    ফেরত 0xfffe;
}

/*
 * Take a valid Shift-JIS byte pair, check that it is in the range corresponding
 * to JIS X 0208, and convert it to a pair of 21..7E bytes.
 * Return 0 if the byte pair is out of range.
 */
স্থির অন্তঃরেখ uint32_t
_2022FromSJIS(uint32_t value) {
    uint8_t trail;

    যদি(value > 0xEFFC) {
        ফেরত 0;  /* beyond JIS X 0208 */
    }

    trail = static_cast<uint8_t>(value);

    value &= 0xff00;  /* lead byte */
    যদি(value <= 0x9f00) {
        value -= 0x7000;
    } অন্যথা /* 0xe000 <= value <= 0xef00 */ {
        value -= 0xb000;
    }
    value <<= 1;

    যদি(trail <= 0x9e) {
        value -= 0x100;
        যদি(trail <= 0x7e) {
            value |= trail - 0x1f;
        } অন্যথা {
            value |= trail - 0x20;
        }
    } অন্যথা /* trail <= 0xfc */ {
        value |= trail - 0x7e;
    }
    ফেরত value;
}

/*
 * Convert a pair of JIS X 0208 21..7E bytes to Shift-JIS.
 * If either byte is outside 21..7E make sure that the result is not valid
 * for Shift-JIS so that the converter catches it.
 * Some invalid byte values already turn into equally invalid Shift-JIS
 * byte values and need not be tested explicitly.
 */
স্থির অন্তঃরেখ শূন্য
_2022ToSJIS(uint8_t c1, uint8_t c2, অক্ষর bytes[2]) {
    যদি(c1&1) {
        ++c1;
        যদি(c2 <= 0x5f) {
            c2 += 0x1f;
        } অন্যথা যদি(c2 <= 0x7e) {
            c2 += 0x20;
        } অন্যথা {
            c2 = 0;  /* invalid */
        }
    } অন্যথা {
        যদি (static_cast<uint8_t>(c2 - 0x21) <= ((0x7e) - 0x21)) {
            c2 += 0x7e;
        } অন্যথা {
            c2 = 0;  /* invalid */
        }
    }
    c1 >>= 1;
    যদি(c1 <= 0x2f) {
        c1 += 0x70;
    } অন্যথা যদি(c1 <= 0x3f) {
        c1 += 0xb0;
    } অন্যথা {
        c1 = 0;  /* invalid */
    }
    bytes[0] = static_cast<অক্ষর>(c1);
    bytes[1] = static_cast<অক্ষর>(c2);
}

/*
 * JIS X 0208 has fallbacks from Unicode half-width Katakana to full-width (DBCS)
 * Katakana.
 * Now that we use a Shift-JIS table for JIS X 0208 we need to hardcode these fallbacks
 * because Shift-JIS roundtrips half-width Katakana to single bytes.
 * These were the only fallbacks in ICU's jisx-208.ucm file.
 */
স্থির ধ্রুবক uint16_t hwkana_fb[HWKANA_END - HWKANA_START + 1] = {
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

স্থির শূন্য U_CALLCONV
UConverter_fromUnicode_ISO_2022_JP_OFFSETS_LOGIC(UConverterFromUnicodeArgs* args, UErrorCode* err) {
    UConverter *cnv = args->converter;
    UConverter_তথ্য_ISO2022 *converter_তথ্য;
    ISO2022State *pFromU2022State;
    uint8_t* target = reinterpret_cast<uint8_t*>(args->target);
    ধ্রুবক uint8_t* targetLimit = reinterpret_cast<ধ্রুবক uint8_t*>(args->targetLimit);
    ধ্রুবক char16_t* source = args->source;
    ধ্রুবক char16_t* sourceLimit = args->sourceLimit;
    int32_t* offsets = args->offsets;
    UChar32 sourceChar;
    অক্ষর বাফার[8];
    int32_t len, outLen;
    int8_t choices[10];
    int32_t choice_গুনতি;
    uint32_t target_মান = 0;
    UBool useFallback;

    int32_t i;
    int8_t cs, g;

    /* set up the state */
    converter_তথ্য = static_cast<UConverter_তথ্য_ISO2022*>(cnv->extraInfo);
    pFromU2022State   = &converter_তথ্য->fromU2022State;

    choice_গুনতি = 0;

    /* check if the last codepoint of previous buffer was a lead surrogate*/
    যদি((sourceChar = cnv->fromUChar32)!=0 && target< targetLimit) {
        যাও getTrail;
    }

    যখন(source < sourceLimit) {
        যদি(target < targetLimit) {

            sourceChar  = *(source++);
            /*check if the char is a First surrogate*/
            যদি(U16_IS_SURROGATE(sourceChar)) {
                যদি(U16_IS_SURROGATE_LEAD(sourceChar)) {
getTrail:
                    /*look ahead to find the trail surrogate*/
                    যদি(source < sourceLimit) {
                        /* test the following code unit */
                        char16_t trail = *source;
                        যদি(U16_IS_TRAIL(trail)) {
                            source++;
                            sourceChar=U16_GET_SUPPLEMENTARY(sourceChar, trail);
                            cnv->fromUChar32=0x00;
                            /* convert this supplementary code point */
                            /* exit this condition tree */
                        } অন্যথা {
                            /* this is an unmatched lead code unit (1st surrogate) */
                            /* callback(illegal) */
                            *err=U_ILLEGAL_CHAR_FOUND;
                            cnv->fromUChar32=sourceChar;
                            বিরতি;
                        }
                    } অন্যথা {
                        /* no more input */
                        cnv->fromUChar32=sourceChar;
                        বিরতি;
                    }
                } অন্যথা {
                    /* this is an unmatched trail code unit (2nd surrogate) */
                    /* callback(illegal) */
                    *err=U_ILLEGAL_CHAR_FOUND;
                    cnv->fromUChar32=sourceChar;
                    বিরতি;
                }
            }

            /* do not convert SO/SI/ESC */
            যদি(IS_2022_CONTROL(sourceChar)) {
                /* callback(illegal) */
                *err=U_ILLEGAL_CHAR_FOUND;
                cnv->fromUChar32=sourceChar;
                বিরতি;
            }

            /* do the conversion */

            যদি(choice_গুনতি == 0) {
                uint16_t csm;

                /*
                 * The csm variable keeps track of which charsets are allowed
                 * and not used yet while building the choices[].
                 */
                csm = jpCharsetMasks[converter_তথ্য->version];
                choice_গুনতি = 0;

                /* JIS7/8: try single-byte half-width Katakana before JISX208 */
                যদি(converter_তথ্য->version == 3 || converter_তথ্য->version == 4) {
                    choices[choice_গুনতি++] = static_cast<int8_t>(HWKANA_7BIT);
                }
                /* Do not try single-byte half-width Katakana for other versions. */
                csm &= ~CSM(HWKANA_7BIT);

                /* try the current G0 charset */
                choices[choice_গুনতি++] = cs = pFromU2022State->cs[0];
                csm &= ~CSM(cs);

                /* try the current G2 charset */
                যদি((cs = pFromU2022State->cs[2]) != 0) {
                    choices[choice_গুনতি++] = cs;
                    csm &= ~CSM(cs);
                }

                /* try all the other possible charsets */
                জন্য(i = 0; i < UPRV_LENGTHOF(jpCharsetPref); ++i) {
                    cs = static_cast<int8_t>(jpCharsetPref[i]);
                    যদি(CSM(cs) & csm) {
                        choices[choice_গুনতি++] = cs;
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

            জন্য(i = 0; i < choice_গুনতি && len <= 0; ++i) {
                uint32_t value;
                int32_t len2;
                int8_t cs0 = choices[i];
                নির্বাচন(cs0) {
                ক্ষেত্র ASCII:
                    যদি(sourceChar <= 0x7f) {
                        target_মান = static_cast<uint32_t>(sourceChar);
                        len = 1;
                        cs = cs0;
                        g = 0;
                    }
                    বিরতি;
                ক্ষেত্র ISO8859_1:
                    যদি(GR96_START <= sourceChar && sourceChar <= GR96_END) {
                        target_মান = static_cast<uint32_t>(sourceChar) - 0x80;
                        len = 1;
                        cs = cs0;
                        g = 2;
                    }
                    বিরতি;
                ক্ষেত্র HWKANA_7BIT:
                    যদি (static_cast<uint32_t>(sourceChar - HWKANA_START) <= (HWKANA_END - HWKANA_START)) {
                        যদি(converter_তথ্য->version==3) {
                            /* JIS7: use G1 (SO) */
                            /* Shift U+FF61..U+FF9F to bytes 21..5F. */
                            target_মান = static_cast<uint32_t>(sourceChar - (HWKANA_START - 0x21));
                            len = 1;
                            pFromU2022State->cs[1] = cs = cs0; /* do not output an escape sequence */
                            g = 1;
                        } অন্যথা যদি(converter_তথ্য->version==4) {
                            /* JIS8: use 8-bit bytes with any single-byte charset, see escape sequence output below */
                            /* Shift U+FF61..U+FF9F to bytes A1..DF. */
                            target_মান = static_cast<uint32_t>(sourceChar - (HWKANA_START - 0xa1));
                            len = 1;

                            cs = pFromU2022State->cs[0];
                            যদি(IS_JP_DBCS(cs)) {
                                /* switch from a DBCS charset to JISX201 */
                                cs = static_cast<int8_t>(JISX201);
                            }
                            /* else stay in the current G0 charset */
                            g = 0;
                        }
                        /* else do not use HWKANA_7BIT with other versions */
                    }
                    বিরতি;
                ক্ষেত্র JISX201:
                    /* G0 SBCS */
                    value = jisx201FromU(sourceChar);
                    যদি(value <= 0x7f) {
                        target_মান = value;
                        len = 1;
                        cs = cs0;
                        g = 0;
                        useFallback = মিথ্যা;
                    }
                    বিরতি;
                ক্ষেত্র JISX208:
                    /* G0 DBCS from Shift-JIS table */
                    len2 = MBCS_FROM_UCHAR32_ISO2022(
                                converter_তথ্য->myConverterArray[cs0],
                                sourceChar, &value,
                                useFallback, MBCS_OUTPUT_2);
                    যদি(len2 == 2 || (len2 == -2 && len == 0)) {  /* only accept DBCS: abs(len)==2 */
                        value = _2022FromSJIS(value);
                        যদি(value != 0) {
                            target_মান = value;
                            len = len2;
                            cs = cs0;
                            g = 0;
                            useFallback = মিথ্যা;
                        }
                    } অন্যথা যদি(len == 0 && useFallback &&
                              static_cast<uint32_t>(sourceChar - HWKANA_START) <= (HWKANA_END - HWKANA_START)) {
                        target_মান = hwkana_fb[sourceChar - HWKANA_START];
                        len = -2;
                        cs = cs0;
                        g = 0;
                        useFallback = মিথ্যা;
                    }
                    বিরতি;
                ক্ষেত্র ISO8859_7:
                    /* G0 SBCS forced to 7-bit output */
                    len2 = MBCS_SINGLE_FROM_UCHAR32(
                                converter_তথ্য->myConverterArray[cs0],
                                sourceChar, &value,
                                useFallback);
                    যদি(len2 != 0 && !(len2 < 0 && len != 0) && GR96_START <= value && value <= GR96_END) {
                        target_মান = value - 0x80;
                        len = len2;
                        cs = cs0;
                        g = 2;
                        useFallback = মিথ্যা;
                    }
                    বিরতি;
                পূর্বনির্ধারিত:
                    /* G0 DBCS */
                    len2 = MBCS_FROM_UCHAR32_ISO2022(
                                converter_তথ্য->myConverterArray[cs0],
                                sourceChar, &value,
                                useFallback, MBCS_OUTPUT_2);
                    যদি(len2 == 2 || (len2 == -2 && len == 0)) {  /* only accept DBCS: abs(len)==2 */
                        যদি(cs0 == KSC5601) {
                            /*
                             * Check for valid bytes for the encoding scheme.
                             * This is necessary because the sub-converter (windows-949)
                             * has a broader encoding scheme than is valid for 2022.
                             */
                            value = _2022FromGR94DBCS(value);
                            যদি(value == 0) {
                                বিরতি;
                            }
                        }
                        target_মান = value;
                        len = len2;
                        cs = cs0;
                        g = 0;
                        useFallback = মিথ্যা;
                    }
                    বিরতি;
                }
            }

            যদি(len != 0) {
                যদি(len < 0) {
                    len = -len;  /* fallback */
                }
                outLen = 0; /* count output bytes */

                /* write SI if necessary (only for JIS7) */
                যদি(pFromU2022State->g == 1 && g == 0) {
                    বাফার[outLen++] = UCNV_SI;
                    pFromU2022State->g = 0;
                }

                /* write the designation sequence if necessary */
                যদি(cs != pFromU2022State->cs[g]) {
                    int32_t escLen = escSeqCharsLen[cs];
                    uprv_memcpy(বাফার + outLen, escSeqChars[cs], escLen);
                    outLen += escLen;
                    pFromU2022State->cs[g] = cs;

                    /* invalidate the choices[] */
                    choice_গুনতি = 0;
                }

                /* write the shift sequence if necessary */
                যদি(g != pFromU2022State->g) {
                    নির্বাচন(g) {
                    /* case 0 handled before writing escapes */
                    ক্ষেত্র 1:
                        বাফার[outLen++] = UCNV_SO;
                        pFromU2022State->g = 1;
                        বিরতি;
                    পূর্বনির্ধারিত: /* case 2 */
                        বাফার[outLen++] = 0x1b;
                        বাফার[outLen++] = 0x4e;
                        বিরতি;
                    /* no case 3: no SS3 in ISO-2022-JP-x */
                    }
                }

                /* write the output bytes */
                যদি(len == 1) {
                    বাফার[outLen++] = static_cast<অক্ষর>(target_মান);
                } অন্যথা /* len == 2 */ {
                    বাফার[outLen++] = static_cast<অক্ষর>(target_মান >> 8);
                    বাফার[outLen++] = static_cast<অক্ষর>(target_মান);
                }
            } অন্যথা {
                /*
                 * if we cannot find the character after checking all codepages
                 * then this is an error
                 */
                *err = U_INVALID_CHAR_FOUND;
                cnv->fromUChar32=sourceChar;
                বিরতি;
            }

            যদি(sourceChar == CR || sourceChar == LF) {
                /* reset the G2 state at the end of a line (conversion got us into ASCII or JISX201 already) */
                pFromU2022State->cs[2] = 0;
                choice_গুনতি = 0;
            }

            /* output outLen>0 bytes in buffer[] */
            যদি(outLen == 1) {
                *target++ = বাফার[0];
                যদি(offsets) {
                    *offsets++ = static_cast<int32_t>(source - args->source - 1); /* -1: known to be ASCII */
                }
            } অন্যথা যদি(outLen == 2 && (target + 2) <= targetLimit) {
                *target++ = বাফার[0];
                *target++ = বাফার[1];
                যদি(offsets) {
                    int32_t source_সূচক = static_cast<int32_t>(source - args->source - U16_দৈর্ঘ্য(sourceChar));
                    *offsets++ = source_সূচক;
                    *offsets++ = source_সূচক;
                }
            } অন্যথা {
                fromUWriteUInt8(
                    cnv,
                    বাফার, outLen,
                    &target, reinterpret_cast<ধ্রুবক অক্ষর*>(targetLimit),
                    &offsets, static_cast<int32_t>(source - args->source - U16_দৈর্ঘ্য(sourceChar)),
                    err);
                যদি(U_FAILURE(*err)) {
                    বিরতি;
                }
            }
        } /* end if(myTargetIndex<myTargetLength) */
        অন্যথা{
            *err =U_বাফার_OVERFLOW_ERROR;
            বিরতি;
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
    যদি( U_SUCCESS(*err) &&
        (pFromU2022State->g!=0 || pFromU2022State->cs[0]!=ASCII) &&
        args->flush && source>=sourceLimit && cnv->fromUChar32==0
    ) {
        int32_t source_সূচক;

        outLen = 0;

        যদি(pFromU2022State->g != 0) {
            বাফার[outLen++] = UCNV_SI;
            pFromU2022State->g = 0;
        }

        যদি(pFromU2022State->cs[0] != ASCII) {
            int32_t escLen = escSeqCharsLen[ASCII];
            uprv_memcpy(বাফার + outLen, escSeqChars[ASCII], escLen);
            outLen += escLen;
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
        source_সূচক = static_cast<int32_t>(source - args->source);
        যদি(source_সূচক>0) {
            --source_সূচক;
            যদি( U16_IS_TRAIL(args->source[source_সূচক]) &&
                (source_সূচক==0 || U16_IS_LEAD(args->source[source_সূচক-1]))
            ) {
                --source_সূচক;
            }
        } অন্যথা {
            source_সূচক=-1;
        }

        fromUWriteUInt8(
            cnv,
            বাফার, outLen,
            &target, reinterpret_cast<ধ্রুবক অক্ষর*>(targetLimit),
            &offsets, source_সূচক,
            err);
    }

    /*save the state and return */
    args->source = source;
    args->target = reinterpret_cast<অক্ষর*>(target);
}

/*************** to unicode *******************/

স্থির শূন্য U_CALLCONV
UConverter_toUnicode_ISO_2022_JP_OFFSETS_LOGIC(UConverterToUnicodeArgs *args,
                                               UErrorCode* err){
    অক্ষর অস্থায়ী_Buf[2];
    ধ্রুবক অক্ষর* mySource = const_cast<অক্ষর*>(args->source);
    char16_t *myTarget = args->target;
    ধ্রুবক অক্ষর *mySourceLimit = args->sourceLimit;
    uint32_t targetUniChar = 0x0000;
    uint32_t mySourceChar = 0x0000;
    uint32_t tmpSourceChar = 0x0000;
    UConverter_তথ্য_ISO2022* my_তথ্য;
    ISO2022State *pToU2022State;
    StateEnum cs;

    my_তথ্য = static_cast<UConverter_তথ্য_ISO2022*>(args->converter->extraInfo);
    pToU2022State = &my_তথ্য->toU2022State;

    যদি(my_তথ্য->চাবি != 0) {
        /* continue with a partial escape sequence */
        যাও escape;
    } অন্যথা যদি(args->converter->toULength == 1 && mySource < mySourceLimit && myTarget < args->targetLimit) {
        /* continue with a partial double-byte character */
        mySourceChar = args->converter->toUBytes[0];
        args->converter->toULength = 0;
        cs = static_cast<StateEnum>(pToU2022State->cs[pToU2022State->g]);
        targetUniChar = missingCharMarker;
        যাও getTrailByte;
    }

    যখন(mySource < mySourceLimit){

        targetUniChar =missingCharMarker;

        যদি(myTarget < args->targetLimit){

            mySourceChar = static_cast<unsigned অক্ষর>(*mySource++);

            নির্বাচন(mySourceChar) {
            ক্ষেত্র UCNV_SI:
                যদি(my_তথ্য->version==3) {
                    pToU2022State->g=0;
                    চালু;
                } অন্যথা {
                    /* only JIS7 uses SI/SO, not ISO-2022-JP-x */
                    my_তথ্য->isEmptySegment = মিথ্যা;	/* reset this, we have a different error */
                    বিরতি;
                }

            ক্ষেত্র UCNV_SO:
                যদি(my_তথ্য->version==3) {
                    /* JIS7: switch to G1 half-width Katakana */
                    pToU2022State->cs[1] = static_cast<int8_t>(HWKANA_7BIT);
                    pToU2022State->g=1;
                    চালু;
                } অন্যথা {
                    /* only JIS7 uses SI/SO, not ISO-2022-JP-x */
                    my_তথ্য->isEmptySegment = মিথ্যা;	/* reset this, we have a different error */
                    বিরতি;
                }

            ক্ষেত্র ESC_2022:
                mySource--;
escape:
                {
                    ধ্রুবক অক্ষর * mySourceBefore = mySource;
                    int8_t toULengthBefore = args->converter->toULength;

                    changeState_2022(args->converter,&(mySource),
                        mySourceLimit, ISO_2022_JP,err);

                    /* If in ISO-2022-JP only and we successfully completed an escape sequence, but previous segment was empty, create an error */
                    যদি(my_তথ্য->version==0 && my_তথ্য->চাবি==0 && U_SUCCESS(*err) && my_তথ্য->isEmptySegment) {
                        *err = U_ILLEGAL_ESCAPE_SEQUENCE;
                        args->converter->toUCallbackReason = UCNV_IRREGULAR;
                        args->converter->toULength = static_cast<int8_t>(toULengthBefore + (mySource - mySourceBefore));
                    }
                }

                /* invalid or illegal escape sequence */
                যদি(U_FAILURE(*err)){
                    args->target = myTarget;
                    args->source = mySource;
                    my_তথ্য->isEmptySegment = মিথ্যা;	/* Reset to avoid future spurious errors */
                    ফেরত;
                }
                /* If we successfully completed an escape sequence, we begin a new segment, empty so far */
                যদি(my_তথ্য->চাবি==0) {
                    my_তথ্য->isEmptySegment = সত্য;
                }
                চালু;

            /* ISO-2022-JP does not use single-byte (C1) SS2 and SS3 */

            ক্ষেত্র CR:
            ক্ষেত্র LF:
                /* automatically reset to single-byte mode */
                যদি (static_cast<StateEnum>(pToU2022State->cs[0]) != ASCII &&
                    static_cast<StateEnum>(pToU2022State->cs[0]) != JISX201) {
                    pToU2022State->cs[0] = static_cast<int8_t>(ASCII);
                }
                pToU2022State->cs[2] = 0;
                pToU2022State->g = 0;
                U_FALLTHROUGH;
            পূর্বনির্ধারিত:
                /* convert one or two bytes */
                my_তথ্য->isEmptySegment = মিথ্যা;
                cs = static_cast<StateEnum>(pToU2022State->cs[pToU2022State->g]);
                যদি (static_cast<uint8_t>(mySourceChar - 0xa1) <= (0xdf - 0xa1) && my_তথ্য->version == 4 &&
                    !IS_JP_DBCS(cs)
                ) {
                    /* 8-bit halfwidth katakana in any single-byte mode for JIS8 */
                    targetUniChar = mySourceChar + (HWKANA_START - 0xa1);

                    /* return from a single-shift state to the previous one */
                    যদি(pToU2022State->g >= 2) {
                        pToU2022State->g=pToU2022State->পূর্ব_G;
                    }
                } অন্যথা নির্বাচন(cs) {
                ক্ষেত্র ASCII:
                    যদি(mySourceChar <= 0x7f) {
                        targetUniChar = mySourceChar;
                    }
                    বিরতি;
                ক্ষেত্র ISO8859_1:
                    যদি(mySourceChar <= 0x7f) {
                        targetUniChar = mySourceChar + 0x80;
                    }
                    /* return from a single-shift state to the previous one */
                    pToU2022State->g=pToU2022State->পূর্ব_G;
                    বিরতি;
                ক্ষেত্র ISO8859_7:
                    যদি(mySourceChar <= 0x7f) {
                        /* convert mySourceChar+0x80 to use a normal 8-bit table */
                        targetUniChar =
                            MBCS_SINGLE_SIMPLE_GET_পরবর্তী_BMP(
                                my_তথ্য->myConverterArray[cs],
                                mySourceChar + 0x80);
                    }
                    /* return from a single-shift state to the previous one */
                    pToU2022State->g=pToU2022State->পূর্ব_G;
                    বিরতি;
                ক্ষেত্র JISX201:
                    যদি(mySourceChar <= 0x7f) {
                        targetUniChar = jisx201ToU(mySourceChar);
                    }
                    বিরতি;
                ক্ষেত্র HWKANA_7BIT:
                    যদি (static_cast<uint8_t>(mySourceChar - 0x21) <= (0x5f - 0x21)) {
                        /* 7-bit halfwidth Katakana */
                        targetUniChar = mySourceChar + (HWKANA_START - 0x21);
                    }
                    বিরতি;
                পূর্বনির্ধারিত:
                    /* G0 DBCS */
                    যদি(mySource < mySourceLimit) {
                        পূর্ণসংখ্যা leadIsOk, trailIsOk;
                        uint8_t trailByte;
getTrailByte:
                        trailByte = static_cast<uint8_t>(*mySource);
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
                        leadIsOk = static_cast<uint8_t>(mySourceChar - 0x21) <= (0x7e - 0x21);
                        trailIsOk = static_cast<uint8_t>(trailByte - 0x21) <= (0x7e - 0x21);
                        যদি (leadIsOk && trailIsOk) {
                            ++mySource;
                            tmpSourceChar = (mySourceChar << 8) | trailByte;
                            যদি(cs == JISX208) {
                                _2022ToSJIS(static_cast<uint8_t>(mySourceChar), trailByte, অস্থায়ী_Buf);
                                mySourceChar = tmpSourceChar;
                            } অন্যথা {
                                /* Copy before we modify tmpSourceChar so toUnicodeCallback() sees the correct bytes. */
                                mySourceChar = tmpSourceChar;
                                যদি (cs == KSC5601) {
                                    tmpSourceChar += 0x8080;  /* = _2022ToGR94DBCS(tmpSourceChar) */
                                }
                                অস্থায়ী_Buf[0] = static_cast<অক্ষর>(tmpSourceChar >> 8);
                                অস্থায়ী_Buf[1] = static_cast<অক্ষর>(tmpSourceChar);
                            }
                            targetUniChar = ucnv_MBCSSimple_Get_পরবর্তী_UChar(my_তথ্য->myConverterArray[cs], অস্থায়ী_Buf, 2, মিথ্যা);
                        } অন্যথা যদি (!(trailIsOk || IS_2022_CONTROL(trailByte))) {
                            /* report a pair of illegal bytes if the second byte is not a DBCS starter */
                            ++mySource;
                            /* add another bit so that the code below writes 2 bytes in case of error */
                            mySourceChar = 0x10000 | (mySourceChar << 8) | trailByte;
                        }
                    } অন্যথা {
                        args->converter->toUBytes[0] = static_cast<uint8_t>(mySourceChar);
                        args->converter->toULength = 1;
                        যাও endloop;
                    }
                }  /* End of inner switch */
                বিরতি;
            }  /* End of outer switch */
            যদি(targetUniChar < (missingCharMarker-1/*0xfffe*/)){
                যদি(args->offsets){
                    args->offsets[myTarget - args->target] = static_cast<int32_t>(mySource - args->source - (mySourceChar <= 0xff ? 1 : 2));
                }
                *(myTarget++) = static_cast<char16_t>(targetUniChar);
            }
            অন্যথা যদি(targetUniChar > missingCharMarker){
                /* disassemble the surrogate pair and write to output*/
                targetUniChar-=0x0010000;
                *myTarget = static_cast<char16_t>(0xd800 + static_cast<char16_t>(targetUniChar >> 10));
                যদি(args->offsets){
                    args->offsets[myTarget - args->target] = static_cast<int32_t>(mySource - args->source - (mySourceChar <= 0xff ? 1 : 2));
                }
                ++myTarget;
                যদি(myTarget< args->targetLimit){
                    *myTarget = static_cast<char16_t>(0xdc00 + static_cast<char16_t>(targetUniChar & 0x3ff));
                    যদি(args->offsets){
                        args->offsets[myTarget - args->target] = static_cast<int32_t>(mySource - args->source - (mySourceChar <= 0xff ? 1 : 2));
                    }
                    ++myTarget;
                }অন্যথা{
                    args->converter->UChar_Error_বাফার[args->converter->UChar_Error_বাফার_দৈর্ঘ্য++]=
                                    static_cast<char16_t>(0xdc00 + static_cast<char16_t>(targetUniChar & 0x3ff));
                }

            }
            অন্যথা{
                /* Call the callback function*/
                toUnicodeCallback(args->converter,mySourceChar,targetUniChar,err);
                বিরতি;
            }
        }
        অন্যথা{    /* goes with "if(myTarget < args->targetLimit)"  way up near top of function */
            *err =U_বাফার_OVERFLOW_ERROR;
            বিরতি;
        }
    }
endloop:
    args->target = myTarget;
    args->source = mySource;
}


#যদি !UCONFIG_ONLY_HTML_CONVERSION
/***************************************************************
*   Rules for ISO-2022-KR encoding
*   i) The KSC5601 designator sequence should appear only once in a file,
*      at the beginning of a line before any KSC5601 characters. This usually
*      means that it appears by itself on the first line of the file
*  ii) There are only 2 shifting sequences SO to shift into double byte mode
*      and SI to shift into single byte mode
*/
স্থির শূন্য U_CALLCONV
UConverter_fromUnicode_ISO_2022_KR_OFFSETS_LOGIC_IBM(UConverterFromUnicodeArgs* args, UErrorCode* err){

    UConverter* saveConv = args->converter;
    UConverter_তথ্য_ISO2022* my_Converter_তথ্য = static_cast<UConverter_তথ্য_ISO2022*>(saveConv->extraInfo);
    args->converter=my_Converter_তথ্য->currentConverter;

    my_Converter_তথ্য->currentConverter->fromUChar32 = saveConv->fromUChar32;
    ucnv_MBCSFromUnicodeWithOffsets(args,err);
    saveConv->fromUChar32 = my_Converter_তথ্য->currentConverter->fromUChar32;

    যদি(*err == U_বাফার_OVERFLOW_ERROR) {
        যদি(my_Converter_তথ্য->currentConverter->char_Error_বাফার_দৈর্ঘ্য > 0) {
            uprv_memcpy(
                saveConv->char_Error_বাফার,
                my_Converter_তথ্য->currentConverter->char_Error_বাফার,
                my_Converter_তথ্য->currentConverter->char_Error_বাফার_দৈর্ঘ্য);
        }
        saveConv->char_Error_বাফার_দৈর্ঘ্য = my_Converter_তথ্য->currentConverter->char_Error_বাফার_দৈর্ঘ্য;
        my_Converter_তথ্য->currentConverter->char_Error_বাফার_দৈর্ঘ্য = 0;
    }
    args->converter=saveConv;
}

স্থির শূন্য U_CALLCONV
UConverter_fromUnicode_ISO_2022_KR_OFFSETS_LOGIC(UConverterFromUnicodeArgs* args, UErrorCode* err){

    ধ্রুবক char16_t *source = args->source;
    ধ্রুবক char16_t *sourceLimit = args->sourceLimit;
    unsigned অক্ষর *target = reinterpret_cast<unsigned অক্ষর*>(args->target);
    unsigned অক্ষর *targetLimit = reinterpret_cast<unsigned অক্ষর*>(const_cast<অক্ষর*>(args->targetLimit));
    int32_t* offsets = args->offsets;
    uint32_t targetByteUnit = 0x0000;
    UChar32 sourceChar = 0x0000;
    UBool isTargetByteDBCS;
    UBool oldIsTargetByteDBCS;
    UConverter_তথ্য_ISO2022 *converter_তথ্য;
    UConverter_Shared_তথ্য* shared_তথ্য;
    UBool useFallback;
    int32_t length =0;

    converter_তথ্য = static_cast<UConverter_তথ্য_ISO2022*>(args->converter->extraInfo);
    /* if the version is 1 then the user is requesting
     * conversion with ibm-25546 pass the arguments to
     * MBCS converter and return
     */
    যদি(converter_তথ্য->version==1){
        UConverter_fromUnicode_ISO_2022_KR_OFFSETS_LOGIC_IBM(args,err);
        ফেরত;
    }

    /* initialize data */
    shared_তথ্য = converter_তথ্য->currentConverter->shared_তথ্য;
    useFallback = args->converter->useFallback;
    isTargetByteDBCS = static_cast<UBool>(args->converter->fromUnicodeStatus);
    oldIsTargetByteDBCS = isTargetByteDBCS;

    isTargetByteDBCS = static_cast<UBool>(args->converter->fromUnicodeStatus);
    যদি((sourceChar = args->converter->fromUChar32)!=0 && target <targetLimit) {
        যাও getTrail;
    }
    যখন(source < sourceLimit){

        targetByteUnit = missingCharMarker;

        যদি(target < (unsigned অক্ষর*) args->targetLimit){
            sourceChar = *source++;

            /* do not convert SO/SI/ESC */
            যদি(IS_2022_CONTROL(sourceChar)) {
                /* callback(illegal) */
                *err=U_ILLEGAL_CHAR_FOUND;
                args->converter->fromUChar32=sourceChar;
                বিরতি;
            }

            length = MBCS_FROM_UCHAR32_ISO2022(shared_তথ্য,sourceChar,&targetByteUnit,useFallback,MBCS_OUTPUT_2);
            যদি(length < 0) {
                length = -length;  /* fallback */
            }
            /* only DBCS or SBCS characters are expected*/
            /* DB characters with high bit set to 1 are expected */
            যদি( length > 2 || length==0 ||
                (length == 1 && targetByteUnit > 0x7f) ||
                (length == 2 &&
                    (static_cast<uint16_t>(targetByteUnit - 0xa1a1) > (0xfefe - 0xa1a1) ||
                    static_cast<uint8_t>(targetByteUnit - 0xa1) > (0xfe - 0xa1)))
            ) {
                targetByteUnit=missingCharMarker;
            }
            যদি (targetByteUnit != missingCharMarker){

                oldIsTargetByteDBCS = isTargetByteDBCS;
                isTargetByteDBCS = static_cast<UBool>(targetByteUnit > 0x00FF);
                  /* append the shift sequence */
                যদি (oldIsTargetByteDBCS != isTargetByteDBCS ){

                    যদি (isTargetByteDBCS)
                        *target++ = UCNV_SO;
                    অন্যথা
                        *target++ = UCNV_SI;
                    যদি(offsets)
                        *(offsets++) = static_cast<int32_t>(source - args->source - 1);
                }
                /* write the targetUniChar  to target */
                যদি(targetByteUnit <= 0x00FF){
                    যদি( target < targetLimit){
                        *(target++) = static_cast<unsigned অক্ষর>(targetByteUnit);
                        যদি(offsets){
                            *(offsets++) = static_cast<int32_t>(source - args->source - 1);
                        }

                    }অন্যথা{
                        args->converter->char_Error_বাফার[args->converter->char_Error_বাফার_দৈর্ঘ্য++] = static_cast<unsigned অক্ষর>(targetByteUnit);
                        *err = U_বাফার_OVERFLOW_ERROR;
                    }
                }অন্যথা{
                    যদি(target < targetLimit){
                        *(target++) = static_cast<unsigned অক্ষর>((targetByteUnit >> 8) - 0x80);
                        যদি(offsets){
                            *(offsets++) = static_cast<int32_t>(source - args->source - 1);
                        }
                        যদি(target < targetLimit){
                            *(target++) = static_cast<unsigned অক্ষর>(targetByteUnit - 0x80);
                            যদি(offsets){
                                *(offsets++) = static_cast<int32_t>(source - args->source - 1);
                            }
                        }অন্যথা{
                            args->converter->char_Error_বাফার[args->converter->char_Error_বাফার_দৈর্ঘ্য++] = static_cast<unsigned অক্ষর>(targetByteUnit - 0x80);
                            *err = U_বাফার_OVERFLOW_ERROR;
                        }
                    }অন্যথা{
                        args->converter->char_Error_বাফার[args->converter->char_Error_বাফার_দৈর্ঘ্য++] = static_cast<unsigned অক্ষর>((targetByteUnit >> 8) - 0x80);
                        args->converter->char_Error_বাফার[args->converter->char_Error_বাফার_দৈর্ঘ্য++] = static_cast<unsigned অক্ষর>(targetByteUnit - 0x80);
                        *err = U_বাফার_OVERFLOW_ERROR;
                    }
                }

            }
            অন্যথা{
                /* oops.. the code point is unassingned
                 * set the error and reason
                 */

                /*check if the char is a First surrogate*/
                যদি(U16_IS_SURROGATE(sourceChar)) {
                    যদি(U16_IS_SURROGATE_LEAD(sourceChar)) {
getTrail:
                        /*look ahead to find the trail surrogate*/
                        যদি(source <  sourceLimit) {
                            /* test the following code unit */
                            char16_t trail = *source;
                            যদি(U16_IS_TRAIL(trail)) {
                                source++;
                                sourceChar=U16_GET_SUPPLEMENTARY(sourceChar, trail);
                                *err = U_INVALID_CHAR_FOUND;
                                /* convert this surrogate code point */
                                /* exit this condition tree */
                            } অন্যথা {
                                /* this is an unmatched lead code unit (1st surrogate) */
                                /* callback(illegal) */
                                *err=U_ILLEGAL_CHAR_FOUND;
                            }
                        } অন্যথা {
                            /* no more input */
                            *err = U_ZERO_ERROR;
                        }
                    } অন্যথা {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *err=U_ILLEGAL_CHAR_FOUND;
                    }
                } অন্যথা {
                    /* callback(unassigned) for a BMP code point */
                    *err = U_INVALID_CHAR_FOUND;
                }

                args->converter->fromUChar32=sourceChar;
                বিরতি;
            }
        } /* end if(myTargetIndex<myTargetLength) */
        অন্যথা{
            *err =U_বাফার_OVERFLOW_ERROR;
            বিরতি;
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
    যদি( U_SUCCESS(*err) &&
        isTargetByteDBCS &&
        args->flush && source>=sourceLimit && args->converter->fromUChar32==0
    ) {
        int32_t source_সূচক;

        /* we are switching to ASCII */
        isTargetByteDBCS=মিথ্যা;

        /* get the source index of the last input character */
        /*
         * TODO this would be simpler and more reliable if we used a pair
         * of sourceIndex/prevSourceIndex like in ucnvmbcs.c
         * so that we could simply use the prevSourceIndex here;
         * this code gives an incorrect result for the rare case of an unmatched
         * trail surrogate that is alone in the last buffer of the text stream
         */
        source_সূচক = static_cast<int32_t>(source - args->source);
        যদি(source_সূচক>0) {
            --source_সূচক;
            যদি( U16_IS_TRAIL(args->source[source_সূচক]) &&
                (source_সূচক==0 || U16_IS_LEAD(args->source[source_সূচক-1]))
            ) {
                --source_সূচক;
            }
        } অন্যথা {
            source_সূচক=-1;
        }

        fromUWriteUInt8(
            args->converter,
            SHIFT_IN_STR, 1,
            &target, reinterpret_cast<ধ্রুবক অক্ষর*>(targetLimit),
            &offsets, source_সূচক,
            err);
    }

    /*save the state and return */
    args->source = source;
    args->target = reinterpret_cast<অক্ষর*>(target);
    args->converter->fromUnicodeStatus = static_cast<uint32_t>(isTargetByteDBCS);
}

/************************ To Unicode ***************************************/

স্থির শূন্য U_CALLCONV
UConverter_toUnicode_ISO_2022_KR_OFFSETS_LOGIC_IBM(UConverterToUnicodeArgs *args,
                                                            UErrorCode* err){
    অক্ষর ধ্রুবক* sourceStart;
    UConverter_তথ্য_ISO2022* my_তথ্য = static_cast<UConverter_তথ্য_ISO2022*>(args->converter->extraInfo);

    UConverterToUnicodeArgs subArgs;
    int32_t min_Args_আকার;

    /* set up the subconverter arguments */
    যদি(args->size<আকার(UConverterToUnicodeArgs)) {
        min_Args_আকার = args->size;
    } অন্যথা {
        min_Args_আকার = static_cast<int32_t>(আকার(UConverterToUnicodeArgs));
    }

    uprv_memcpy(&subArgs, args, min_Args_আকার);
    subArgs.size = static_cast<uint16_t>(min_Args_আকার);
    subArgs.converter = my_তথ্য->currentConverter;

    /* remember the original start of the input for offsets */
    sourceStart = args->source;

    যদি(my_তথ্য->চাবি != 0) {
        /* continue with a partial escape sequence */
        যাও escape;
    }

    যখন(U_SUCCESS(*err) && args->source < args->sourceLimit) {
        /*Find the end of the buffer e.g : Next Escape Seq | end of Buffer*/
        subArgs.source = args->source;
        subArgs.sourceLimit = get_End_Of_বাফার_2022(&(args->source), args->sourceLimit, args->flush);
        যদি(subArgs.source != subArgs.sourceLimit) {
            /*
             * get the current partial byte sequence
             *
             * it needs to be moved between the public and the subconverter
             * so that the conversion framework, which only sees the public
             * converter, can handle truncated and illegal input etc.
             */
            যদি(args->converter->toULength > 0) {
                uprv_memcpy(subArgs.converter->toUBytes, args->converter->toUBytes, args->converter->toULength);
            }
            subArgs.converter->toULength = args->converter->toULength;

            /*
             * Convert up to the end of the input, or to before the next escape character.
             * Does not handle conversion extensions because the preToU[] state etc.
             * is not copied.
             */
            ucnv_MBCSToUnicodeWithOffsets(&subArgs, err);

            যদি(args->offsets != nullptr && sourceStart != args->source) {
                /* update offsets to base them on the actual start of the input */
                int32_t *offsets = args->offsets;
                char16_t *target = args->target;
                int32_t delta = static_cast<int32_t>(args->source - sourceStart);
                যখন(target < subArgs.target) {
                    যদি(*offsets >= 0) {
                        *offsets += delta;
                    }
                    ++offsets;
                    ++target;
                }
            }
            args->source = subArgs.source;
            args->target = subArgs.target;
            args->offsets = subArgs.offsets;

            /* copy input/error/overflow buffers */
            যদি(subArgs.converter->toULength > 0) {
                uprv_memcpy(args->converter->toUBytes, subArgs.converter->toUBytes, subArgs.converter->toULength);
            }
            args->converter->toULength = subArgs.converter->toULength;

            যদি(*err == U_বাফার_OVERFLOW_ERROR) {
                যদি(subArgs.converter->UChar_Error_বাফার_দৈর্ঘ্য > 0) {
                    uprv_memcpy(args->converter->UChar_Error_বাফার, subArgs.converter->UChar_Error_বাফার,
                                subArgs.converter->UChar_Error_বাফার_দৈর্ঘ্য);
                }
                args->converter->UChar_Error_বাফার_দৈর্ঘ্য=subArgs.converter->UChar_Error_বাফার_দৈর্ঘ্য;
                subArgs.converter->UChar_Error_বাফার_দৈর্ঘ্য = 0;
            }
        }

        যদি (U_FAILURE(*err) || (args->source == args->sourceLimit)) {
            ফেরত;
        }

escape:
        changeState_2022(args->converter,
               &(args->source),
               args->sourceLimit,
               ISO_2022_KR,
               err);
    }
}

স্থির শূন্য U_CALLCONV
UConverter_toUnicode_ISO_2022_KR_OFFSETS_LOGIC(UConverterToUnicodeArgs *args,
                                                            UErrorCode* err){
    অক্ষর অস্থায়ী_Buf[2];
    ধ্রুবক অক্ষর* mySource = const_cast<অক্ষর*>(args->source);
    char16_t *myTarget = args->target;
    ধ্রুবক অক্ষর *mySourceLimit = args->sourceLimit;
    UChar32 targetUniChar = 0x0000;
    char16_t mySourceChar = 0x0000;
    UConverter_তথ্য_ISO2022* my_তথ্য;
    UConverter_Shared_তথ্য* shared_তথ্য ;
    UBool useFallback;

    my_তথ্য = static_cast<UConverter_তথ্য_ISO2022*>(args->converter->extraInfo);
    যদি(my_তথ্য->version==1){
        UConverter_toUnicode_ISO_2022_KR_OFFSETS_LOGIC_IBM(args,err);
        ফেরত;
    }

    /* initialize state */
    shared_তথ্য = my_তথ্য->currentConverter->shared_তথ্য;
    useFallback = args->converter->useFallback;

    যদি(my_তথ্য->চাবি != 0) {
        /* continue with a partial escape sequence */
        যাও escape;
    } অন্যথা যদি(args->converter->toULength == 1 && mySource < mySourceLimit && myTarget < args->targetLimit) {
        /* continue with a partial double-byte character */
        mySourceChar = args->converter->toUBytes[0];
        args->converter->toULength = 0;
        যাও getTrailByte;
    }

    যখন(mySource< mySourceLimit){

        যদি(myTarget < args->targetLimit){

            mySourceChar = static_cast<unsigned অক্ষর>(*mySource++);

            যদি(mySourceChar==UCNV_SI){
                my_তথ্য->toU2022State.g = 0;
                যদি (my_তথ্য->isEmptySegment) {
                    my_তথ্য->isEmptySegment = মিথ্যা;	/* we are handling it, reset to avoid future spurious errors */
                    *err = U_ILLEGAL_ESCAPE_SEQUENCE;
                    args->converter->toUCallbackReason = UCNV_IRREGULAR;
                    args->converter->toUBytes[0] = static_cast<uint8_t>(mySourceChar);
                    args->converter->toULength = 1;
                    args->target = myTarget;
                    args->source = mySource;
                    ফেরত;
                }
                /*consume the source */
                চালু;
            }অন্যথা যদি(mySourceChar==UCNV_SO){
                my_তথ্য->toU2022State.g = 1;
                my_তথ্য->isEmptySegment = সত্য;	/* Begin a new segment, empty so far */
                /*consume the source */
                চালু;
            }অন্যথা যদি(mySourceChar==ESC_2022){
                mySource--;
escape:
                my_তথ্য->isEmptySegment = মিথ্যা;	/* Any invalid ESC sequences will be detected separately, so just reset this */
                changeState_2022(args->converter,&(mySource),
                                mySourceLimit, ISO_2022_KR, err);
                যদি(U_FAILURE(*err)){
                    args->target = myTarget;
                    args->source = mySource;
                    ফেরত;
                }
                চালু;
            }

            my_তথ্য->isEmptySegment = মিথ্যা;	/* Any invalid char errors will be detected separately, so just reset this */
            যদি(my_তথ্য->toU2022State.g == 1) {
                যদি(mySource < mySourceLimit) {
                    পূর্ণসংখ্যা leadIsOk, trailIsOk;
                    uint8_t trailByte;
getTrailByte:
                    targetUniChar = missingCharMarker;
                    trailByte = static_cast<uint8_t>(*mySource);
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
                    leadIsOk = static_cast<uint8_t>(mySourceChar - 0x21) <= (0x7e - 0x21);
                    trailIsOk = static_cast<uint8_t>(trailByte - 0x21) <= (0x7e - 0x21);
                    যদি (leadIsOk && trailIsOk) {
                        ++mySource;
                        অস্থায়ী_Buf[0] = static_cast<অক্ষর>(mySourceChar + 0x80);
                        অস্থায়ী_Buf[1] = static_cast<অক্ষর>(trailByte + 0x80);
                        targetUniChar = ucnv_MBCSSimple_Get_পরবর্তী_UChar(shared_তথ্য, অস্থায়ী_Buf, 2, useFallback);
                        mySourceChar = (mySourceChar << 8) | trailByte;
                    } অন্যথা যদি (!(trailIsOk || IS_2022_CONTROL(trailByte))) {
                        /* report a pair of illegal bytes if the second byte is not a DBCS starter */
                        ++mySource;
                        /* add another bit so that the code below writes 2 bytes in case of error */
                        mySourceChar = static_cast<char16_t>(0x10000 | (mySourceChar << 8) | trailByte);
                    }
                } অন্যথা {
                    args->converter->toUBytes[0] = static_cast<uint8_t>(mySourceChar);
                    args->converter->toULength = 1;
                    বিরতি;
                }
            }
            অন্যথা যদি(mySourceChar <= 0x7f) {
                targetUniChar = ucnv_MBCSSimple_Get_পরবর্তী_UChar(shared_তথ্য, mySource - 1, 1, useFallback);
            } অন্যথা {
                targetUniChar = 0xffff;
            }
            যদি(targetUniChar < 0xfffe){
                যদি(args->offsets) {
                    args->offsets[myTarget - args->target] = static_cast<int32_t>(mySource - args->source - (mySourceChar <= 0xff ? 1 : 2));
                }
                *(myTarget++) = static_cast<char16_t>(targetUniChar);
            }
            অন্যথা {
                /* Call the callback function*/
                toUnicodeCallback(args->converter,mySourceChar,targetUniChar,err);
                বিরতি;
            }
        }
        অন্যথা{
            *err =U_বাফার_OVERFLOW_ERROR;
            বিরতি;
        }
    }
    args->target = myTarget;
    args->source = mySource;
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
স্থির ধ্রুবক অক্ষর GB_2312_80_STR[] = "\x1B\x24\x29\x41";
স্থির ধ্রুবক অক্ষর ISO_IR_165_STR[] = "\x1B\x24\x29\x45";
স্থির ধ্রুবক অক্ষর CNS_11643_1992_Plane_1_STR[] = "\x1B\x24\x29\x47";
স্থির ধ্রুবক অক্ষর CNS_11643_1992_Plane_2_STR[] = "\x1B\x24\x2A\x48";
স্থির ধ্রুবক অক্ষর CNS_11643_1992_Plane_3_STR[] = "\x1B\x24\x2B\x49";
স্থির ধ্রুবক অক্ষর CNS_11643_1992_Plane_4_STR[] = "\x1B\x24\x2B\x4A";
স্থির ধ্রুবক অক্ষর CNS_11643_1992_Plane_5_STR[] = "\x1B\x24\x2B\x4B";
স্থির ধ্রুবক অক্ষর CNS_11643_1992_Plane_6_STR[] = "\x1B\x24\x2B\x4C";
স্থির ধ্রুবক অক্ষর CNS_11643_1992_Plane_7_STR[] = "\x1B\x24\x2B\x4D";

/********************** ISO2022-CN Data **************************/
স্থির ধ্রুবক অক্ষর* ধ্রুবক escSeqCharsCN[10] ={
        SHIFT_IN_STR,                   /* 0 ASCII */
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

স্থির শূন্য U_CALLCONV
UConverter_fromUnicode_ISO_2022_CN_OFFSETS_LOGIC(UConverterFromUnicodeArgs* args, UErrorCode* err){
    UConverter *cnv = args->converter;
    UConverter_তথ্য_ISO2022 *converter_তথ্য;
    ISO2022State *pFromU2022State;
    uint8_t* target = reinterpret_cast<uint8_t*>(args->target);
    ধ্রুবক uint8_t* targetLimit = reinterpret_cast<ধ্রুবক uint8_t*>(args->targetLimit);
    ধ্রুবক char16_t* source = args->source;
    ধ্রুবক char16_t* sourceLimit = args->sourceLimit;
    int32_t* offsets = args->offsets;
    UChar32 sourceChar;
    অক্ষর বাফার[8];
    int32_t len;
    int8_t choices[3];
    int32_t choice_গুনতি;
    uint32_t target_মান = 0;
    UBool useFallback;

    /* set up the state */
    converter_তথ্য = static_cast<UConverter_তথ্য_ISO2022*>(cnv->extraInfo);
    pFromU2022State   = &converter_তথ্য->fromU2022State;

    choice_গুনতি = 0;

    /* check if the last codepoint of previous buffer was a lead surrogate*/
    যদি((sourceChar = cnv->fromUChar32)!=0 && target< targetLimit) {
        যাও getTrail;
    }

    যখন( source < sourceLimit){
        যদি(target < targetLimit){

            sourceChar  = *(source++);
            /*check if the char is a First surrogate*/
             যদি(U16_IS_SURROGATE(sourceChar)) {
                যদি(U16_IS_SURROGATE_LEAD(sourceChar)) {
getTrail:
                    /*look ahead to find the trail surrogate*/
                    যদি(source < sourceLimit) {
                        /* test the following code unit */
                        char16_t trail = *source;
                        যদি(U16_IS_TRAIL(trail)) {
                            source++;
                            sourceChar=U16_GET_SUPPLEMENTARY(sourceChar, trail);
                            cnv->fromUChar32=0x00;
                            /* convert this supplementary code point */
                            /* exit this condition tree */
                        } অন্যথা {
                            /* this is an unmatched lead code unit (1st surrogate) */
                            /* callback(illegal) */
                            *err=U_ILLEGAL_CHAR_FOUND;
                            cnv->fromUChar32=sourceChar;
                            বিরতি;
                        }
                    } অন্যথা {
                        /* no more input */
                        cnv->fromUChar32=sourceChar;
                        বিরতি;
                    }
                } অন্যথা {
                    /* this is an unmatched trail code unit (2nd surrogate) */
                    /* callback(illegal) */
                    *err=U_ILLEGAL_CHAR_FOUND;
                    cnv->fromUChar32=sourceChar;
                    বিরতি;
                }
            }

            /* do the conversion */
            যদি(sourceChar <= 0x007f ){
                /* do not convert SO/SI/ESC */
                যদি(IS_2022_CONTROL(sourceChar)) {
                    /* callback(illegal) */
                    *err=U_ILLEGAL_CHAR_FOUND;
                    cnv->fromUChar32=sourceChar;
                    বিরতি;
                }

                /* US-ASCII */
                যদি(pFromU2022State->g == 0) {
                    বাফার[0] = static_cast<অক্ষর>(sourceChar);
                    len = 1;
                } অন্যথা {
                    বাফার[0] = UCNV_SI;
                    বাফার[1] = static_cast<অক্ষর>(sourceChar);
                    len = 2;
                    pFromU2022State->g = 0;
                    choice_গুনতি = 0;
                }
                যদি(sourceChar == CR || sourceChar == LF) {
                    /* reset the state at the end of a line */
                    uprv_memset(pFromU2022State, 0, আকার(ISO2022State));
                    choice_গুনতি = 0;
                }
            }
            অন্যথা{
                /* convert U+0080..U+10ffff */
                int32_t i;
                int8_t cs, g;

                যদি(choice_গুনতি == 0) {
                    /* try the current SO/G1 converter first */
                    choices[0] = pFromU2022State->cs[1];

                    /* default to GB2312_1 if none is designated yet */
                    যদি(choices[0] == 0) {
                        choices[0] = GB2312_1;
                    }

                    যদি(converter_তথ্য->version == 0) {
                        /* ISO-2022-CN */

                        /* try the other SO/G1 converter; a CNS_11643_1 lookup may result in any plane */
                        যদি(choices[0] == GB2312_1) {
                            choices[1] = static_cast<int8_t>(CNS_11643_1);
                        } অন্যথা {
                            choices[1] = static_cast<int8_t>(GB2312_1);
                        }

                        choice_গুনতি = 2;
                    } অন্যথা যদি (converter_তথ্য->version == 1) {
                        /* ISO-2022-CN-EXT */

                        /* try one of the other converters */
                        নির্বাচন(choices[0]) {
                        ক্ষেত্র GB2312_1:
                            choices[1] = static_cast<int8_t>(CNS_11643_1);
                            choices[2] = static_cast<int8_t>(ISO_IR_165);
                            বিরতি;
                        ক্ষেত্র ISO_IR_165:
                            choices[1] = static_cast<int8_t>(GB2312_1);
                            choices[2] = static_cast<int8_t>(CNS_11643_1);
                            বিরতি;
                        পূর্বনির্ধারিত: /* CNS_11643_x */
                            choices[1] = static_cast<int8_t>(GB2312_1);
                            choices[2] = static_cast<int8_t>(ISO_IR_165);
                            বিরতি;
                        }

                        choice_গুনতি = 3;
                    } অন্যথা {
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

                জন্য(i = 0; i < choice_গুনতি && len <= 0; ++i) {
                    int8_t cs0 = choices[i];
                    যদি(cs0 > 0) {
                        uint32_t value;
                        int32_t len2;
                        যদি(cs0 >= CNS_11643_0) {
                            len2 = MBCS_FROM_UCHAR32_ISO2022(
                                        converter_তথ্য->myConverterArray[CNS_11643],
                                        sourceChar,
                                        &value,
                                        useFallback,
                                        MBCS_OUTPUT_3);
                            যদি(len2 == 3 || (len2 == -3 && len == 0)) {
                                target_মান = value;
                                cs = static_cast<int8_t>(CNS_11643_0 + (value >> 16) - 0x80);
                                যদি(len2 >= 0) {
                                    len = 2;
                                } অন্যথা {
                                    len = -2;
                                    useFallback = মিথ্যা;
                                }
                                যদি(cs == CNS_11643_1) {
                                    g = 1;
                                } অন্যথা যদি(cs == CNS_11643_2) {
                                    g = 2;
                                } অন্যথা /* plane 3..7 */ যদি(converter_তথ্য->version == 1) {
                                    g = 3;
                                } অন্যথা {
                                    /* ISO-2022-CN (without -EXT) does not support plane 3..7 */
                                    len = 0;
                                }
                            }
                        } অন্যথা {
                            /* GB2312_1 or ISO-IR-165 */
                            U_ASSERT(cs0<UCNV_2022_MAX_CONVERTERS);
                            len2 = MBCS_FROM_UCHAR32_ISO2022(
                                        converter_তথ্য->myConverterArray[cs0],
                                        sourceChar,
                                        &value,
                                        useFallback,
                                        MBCS_OUTPUT_2);
                            যদি(len2 == 2 || (len2 == -2 && len == 0)) {
                                target_মান = value;
                                len = len2;
                                cs = cs0;
                                g = 1;
                                useFallback = মিথ্যা;
                            }
                        }
                    }
                }

                যদি(len != 0) {
                    len = 0; /* count output bytes; it must have been abs(len) == 2 */

                    /* write the designation sequence if necessary */
                    যদি(cs != pFromU2022State->cs[g]) {
                        যদি(cs < CNS_11643) {
                            uprv_memcpy(বাফার, escSeqCharsCN[cs], 4);
                        } অন্যথা {
                            U_ASSERT(cs >= CNS_11643_1);
                            uprv_memcpy(বাফার, escSeqCharsCN[CNS_11643 + (cs - CNS_11643_1)], 4);
                        }
                        len = 4;
                        pFromU2022State->cs[g] = cs;
                        যদি(g == 1) {
                            /* changing the SO/G1 charset invalidates the choices[] */
                            choice_গুনতি = 0;
                        }
                    }

                    /* write the shift sequence if necessary */
                    যদি(g != pFromU2022State->g) {
                        নির্বাচন(g) {
                        ক্ষেত্র 1:
                            বাফার[len++] = UCNV_SO;

                            /* set the new state only if it is the locking shift SO/G1, not for SS2 or SS3 */
                            pFromU2022State->g = 1;
                            বিরতি;
                        ক্ষেত্র 2:
                            বাফার[len++] = 0x1b;
                            বাফার[len++] = 0x4e;
                            বিরতি;
                        পূর্বনির্ধারিত: /* case 3 */
                            বাফার[len++] = 0x1b;
                            বাফার[len++] = 0x4f;
                            বিরতি;
                        }
                    }

                    /* write the two output bytes */
                    বাফার[len++] = static_cast<অক্ষর>(target_মান >> 8);
                    বাফার[len++] = static_cast<অক্ষর>(target_মান);
                } অন্যথা {
                    /* if we cannot find the character after checking all codepages
                     * then this is an error
                     */
                    *err = U_INVALID_CHAR_FOUND;
                    cnv->fromUChar32=sourceChar;
                    বিরতি;
                }
            }

            /* output len>0 bytes in buffer[] */
            যদি(len == 1) {
                *target++ = বাফার[0];
                যদি(offsets) {
                    *offsets++ = static_cast<int32_t>(source - args->source - 1); /* -1: known to be ASCII */
                }
            } অন্যথা যদি(len == 2 && (target + 2) <= targetLimit) {
                *target++ = বাফার[0];
                *target++ = বাফার[1];
                যদি(offsets) {
                    int32_t source_সূচক = static_cast<int32_t>(source - args->source - U16_দৈর্ঘ্য(sourceChar));
                    *offsets++ = source_সূচক;
                    *offsets++ = source_সূচক;
                }
            } অন্যথা {
                fromUWriteUInt8(
                    cnv,
                    বাফার, len,
                    &target, reinterpret_cast<ধ্রুবক অক্ষর*>(targetLimit),
                    &offsets, static_cast<int32_t>(source - args->source - U16_দৈর্ঘ্য(sourceChar)),
                    err);
                যদি(U_FAILURE(*err)) {
                    বিরতি;
                }
            }
        } /* end if(myTargetIndex<myTargetLength) */
        অন্যথা{
            *err =U_বাফার_OVERFLOW_ERROR;
            বিরতি;
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
    যদি( U_SUCCESS(*err) &&
        pFromU2022State->g!=0 &&
        args->flush && source>=sourceLimit && cnv->fromUChar32==0
    ) {
        int32_t source_সূচক;

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
        source_সূচক = static_cast<int32_t>(source - args->source);
        যদি(source_সূচক>0) {
            --source_সূচক;
            যদি( U16_IS_TRAIL(args->source[source_সূচক]) &&
                (source_সূচক==0 || U16_IS_LEAD(args->source[source_সূচক-1]))
            ) {
                --source_সূচক;
            }
        } অন্যথা {
            source_সূচক=-1;
        }

        fromUWriteUInt8(
            cnv,
            SHIFT_IN_STR, 1,
            &target, reinterpret_cast<ধ্রুবক অক্ষর*>(targetLimit),
            &offsets, source_সূচক,
            err);
    }

    /*save the state and return */
    args->source = source;
    args->target = reinterpret_cast<অক্ষর*>(target);
}


স্থির শূন্য U_CALLCONV
UConverter_toUnicode_ISO_2022_CN_OFFSETS_LOGIC(UConverterToUnicodeArgs *args,
                                               UErrorCode* err){
    অক্ষর অস্থায়ী_Buf[3];
    ধ্রুবক অক্ষর* mySource = const_cast<অক্ষর*>(args->source);
    char16_t *myTarget = args->target;
    ধ্রুবক অক্ষর *mySourceLimit = args->sourceLimit;
    uint32_t targetUniChar = 0x0000;
    uint32_t mySourceChar = 0x0000;
    UConverter_তথ্য_ISO2022* my_তথ্য;
    ISO2022State *pToU2022State;

    my_তথ্য = static_cast<UConverter_তথ্য_ISO2022*>(args->converter->extraInfo);
    pToU2022State = &my_তথ্য->toU2022State;

    যদি(my_তথ্য->চাবি != 0) {
        /* continue with a partial escape sequence */
        যাও escape;
    } অন্যথা যদি(args->converter->toULength == 1 && mySource < mySourceLimit && myTarget < args->targetLimit) {
        /* continue with a partial double-byte character */
        mySourceChar = args->converter->toUBytes[0];
        args->converter->toULength = 0;
        targetUniChar = missingCharMarker;
        যাও getTrailByte;
    }

    যখন(mySource < mySourceLimit){

        targetUniChar =missingCharMarker;

        যদি(myTarget < args->targetLimit){

            mySourceChar = static_cast<unsigned অক্ষর>(*mySource++);

            নির্বাচন(mySourceChar){
            ক্ষেত্র UCNV_SI:
                pToU2022State->g=0;
                যদি (my_তথ্য->isEmptySegment) {
                    my_তথ্য->isEmptySegment = মিথ্যা;	/* we are handling it, reset to avoid future spurious errors */
                    *err = U_ILLEGAL_ESCAPE_SEQUENCE;
                    args->converter->toUCallbackReason = UCNV_IRREGULAR;
                    args->converter->toUBytes[0] = static_cast<uint8_t>(mySourceChar);
                    args->converter->toULength = 1;
                    args->target = myTarget;
                    args->source = mySource;
                    ফেরত;
                }
                চালু;

            ক্ষেত্র UCNV_SO:
                যদি(pToU2022State->cs[1] != 0) {
                    pToU2022State->g=1;
                    my_তথ্য->isEmptySegment = সত্য;	/* Begin a new segment, empty so far */
                    চালু;
                } অন্যথা {
                    /* illegal to have SO before a matching designator */
                    my_তথ্য->isEmptySegment = মিথ্যা;	/* Handling a different error, reset this to avoid future spurious errs */
                    বিরতি;
                }

            ক্ষেত্র ESC_2022:
                mySource--;
escape:
                {
                    ধ্রুবক অক্ষর * mySourceBefore = mySource;
                    int8_t toULengthBefore = args->converter->toULength;

                    changeState_2022(args->converter,&(mySource),
                        mySourceLimit, ISO_2022_CN,err);

                    /* After SO there must be at least one character before a designator (designator error handled separately) */
                    যদি(my_তথ্য->চাবি==0 && U_SUCCESS(*err) && my_তথ্য->isEmptySegment) {
                        *err = U_ILLEGAL_ESCAPE_SEQUENCE;
                        args->converter->toUCallbackReason = UCNV_IRREGULAR;
                        args->converter->toULength = static_cast<int8_t>(toULengthBefore + (mySource - mySourceBefore));
                    }
                }

                /* invalid or illegal escape sequence */
                যদি(U_FAILURE(*err)){
                    args->target = myTarget;
                    args->source = mySource;
                    my_তথ্য->isEmptySegment = মিথ্যা;	/* Reset to avoid future spurious errors */
                    ফেরত;
                }
                চালু;

            /* ISO-2022-CN does not use single-byte (C1) SS2 and SS3 */

            ক্ষেত্র CR:
            ক্ষেত্র LF:
                uprv_memset(pToU2022State, 0, আকার(ISO2022State));
                U_FALLTHROUGH;
            পূর্বনির্ধারিত:
                /* convert one or two bytes */
                my_তথ্য->isEmptySegment = মিথ্যা;
                যদি(pToU2022State->g != 0) {
                    যদি(mySource < mySourceLimit) {
                        UConverter_Shared_তথ্য *cnv;
                        StateEnum অস্থায়ী_State;
                        int32_t অস্থায়ী_Buf_Len;
                        পূর্ণসংখ্যা leadIsOk, trailIsOk;
                        uint8_t trailByte;
getTrailByte:
                        trailByte = static_cast<uint8_t>(*mySource);
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
                        leadIsOk = static_cast<uint8_t>(mySourceChar - 0x21) <= (0x7e - 0x21);
                        trailIsOk = static_cast<uint8_t>(trailByte - 0x21) <= (0x7e - 0x21);
                        যদি (leadIsOk && trailIsOk) {
                            ++mySource;
                            অস্থায়ী_State = static_cast<StateEnum>(pToU2022State->cs[pToU2022State->g]);
                            যদি(অস্থায়ী_State >= CNS_11643_0) {
                                cnv = my_তথ্য->myConverterArray[CNS_11643];
                                অস্থায়ী_Buf[0] = static_cast<অক্ষর>(0x80 + (অস্থায়ী_State - CNS_11643_0));
                                অস্থায়ী_Buf[1] = static_cast<অক্ষর>(mySourceChar);
                                অস্থায়ী_Buf[2] = static_cast<অক্ষর>(trailByte);
                                অস্থায়ী_Buf_Len = 3;

                            }অন্যথা{
                                U_ASSERT(অস্থায়ী_State<UCNV_2022_MAX_CONVERTERS);
                                cnv = my_তথ্য->myConverterArray[অস্থায়ী_State];
                                অস্থায়ী_Buf[0] = static_cast<অক্ষর>(mySourceChar);
                                অস্থায়ী_Buf[1] = static_cast<অক্ষর>(trailByte);
                                অস্থায়ী_Buf_Len = 2;
                            }
                            targetUniChar = ucnv_MBCSSimple_Get_পরবর্তী_UChar(cnv, অস্থায়ী_Buf, অস্থায়ী_Buf_Len, মিথ্যা);
                            mySourceChar = (mySourceChar << 8) | trailByte;
                        } অন্যথা যদি (!(trailIsOk || IS_2022_CONTROL(trailByte))) {
                            /* report a pair of illegal bytes if the second byte is not a DBCS starter */
                            ++mySource;
                            /* add another bit so that the code below writes 2 bytes in case of error */
                            mySourceChar = 0x10000 | (mySourceChar << 8) | trailByte;
                        }
                        যদি(pToU2022State->g>=2) {
                            /* return from a single-shift state to the previous one */
                            pToU2022State->g=pToU2022State->পূর্ব_G;
                        }
                    } অন্যথা {
                        args->converter->toUBytes[0] = static_cast<uint8_t>(mySourceChar);
                        args->converter->toULength = 1;
                        যাও endloop;
                    }
                }
                অন্যথা{
                    যদি(mySourceChar <= 0x7f) {
                        targetUniChar = static_cast<char16_t>(mySourceChar);
                    }
                }
                বিরতি;
            }
            যদি(targetUniChar < (missingCharMarker-1/*0xfffe*/)){
                যদি(args->offsets){
                    args->offsets[myTarget - args->target] = static_cast<int32_t>(mySource - args->source - (mySourceChar <= 0xff ? 1 : 2));
                }
                *(myTarget++) = static_cast<char16_t>(targetUniChar);
            }
            অন্যথা যদি(targetUniChar > missingCharMarker){
                /* disassemble the surrogate pair and write to output*/
                targetUniChar-=0x0010000;
                *myTarget = static_cast<char16_t>(0xd800 + static_cast<char16_t>(targetUniChar >> 10));
                যদি(args->offsets){
                    args->offsets[myTarget - args->target] = static_cast<int32_t>(mySource - args->source - (mySourceChar <= 0xff ? 1 : 2));
                }
                ++myTarget;
                যদি(myTarget< args->targetLimit){
                    *myTarget = static_cast<char16_t>(0xdc00 + static_cast<char16_t>(targetUniChar & 0x3ff));
                    যদি(args->offsets){
                        args->offsets[myTarget - args->target] = static_cast<int32_t>(mySource - args->source - (mySourceChar <= 0xff ? 1 : 2));
                    }
                    ++myTarget;
                }অন্যথা{
                    args->converter->UChar_Error_বাফার[args->converter->UChar_Error_বাফার_দৈর্ঘ্য++]=
                                    static_cast<char16_t>(0xdc00 + static_cast<char16_t>(targetUniChar & 0x3ff));
                }

            }
            অন্যথা{
                /* Call the callback function*/
                toUnicodeCallback(args->converter,mySourceChar,targetUniChar,err);
                বিরতি;
            }
        }
        অন্যথা{
            *err =U_বাফার_OVERFLOW_ERROR;
            বিরতি;
        }
    }
endloop:
    args->target = myTarget;
    args->source = mySource;
}
#endif /* #if !UCONFIG_ONLY_HTML_CONVERSION */

স্থির শূন্য U_CALLCONV
_ISO_2022_WriteSub(UConverterFromUnicodeArgs *args, int32_t offset_সূচক, UErrorCode *err) {
    UConverter *cnv = args->converter;
    UConverter_তথ্য_ISO2022* my_Converter_তথ্য = static_cast<UConverter_তথ্য_ISO2022*>(cnv->extraInfo);
    ISO2022State *pFromU2022State=&my_Converter_তথ্য->fromU2022State;
    অক্ষর *p, *subchar;
    অক্ষর বাফার[8];
    int32_t length;

    subchar = reinterpret_cast<অক্ষর*>(cnv->subChars);
    length=cnv->subCharLen; /* assume length==1 for most variants */

    p = বাফার;
    নির্বাচন(my_Converter_তথ্য->locale[0]){
    ক্ষেত্র 'j':
        {
            int8_t cs;

            যদি(pFromU2022State->g == 1) {
                /* JIS7: switch from G1 to G0 */
                pFromU2022State->g = 0;
                *p++ = UCNV_SI;
            }

            cs = pFromU2022State->cs[0];
            যদি(cs != ASCII && cs != JISX201) {
                /* not in ASCII or JIS X 0201: switch to ASCII */
                pFromU2022State->cs[0] = static_cast<int8_t>(ASCII);
                *p++ = '\x1b';
                *p++ = '\x28';
                *p++ = '\x42';
            }

            *p++ = subchar[0];
            বিরতি;
        }
    ক্ষেত্র 'c':
        যদি(pFromU2022State->g != 0) {
            /* not in ASCII mode: switch to ASCII */
            pFromU2022State->g = 0;
            *p++ = UCNV_SI;
        }
        *p++ = subchar[0];
        বিরতি;
    ক্ষেত্র 'k':
        যদি(my_Converter_তথ্য->version == 0) {
            যদি(length == 1) {
                যদি(args->converter->fromUnicodeStatus) {
                    /* in DBCS mode: switch to SBCS */
                    args->converter->fromUnicodeStatus = 0;
                    *p++ = UCNV_SI;
                }
                *p++ = subchar[0];
            } অন্যথা /* length == 2*/ {
                যদি(!args->converter->fromUnicodeStatus) {
                    /* in SBCS mode: switch to DBCS */
                    args->converter->fromUnicodeStatus = 1;
                    *p++ = UCNV_SO;
                }
                *p++ = subchar[0];
                *p++ = subchar[1];
            }
            বিরতি;
        } অন্যথা {
            /* save the subconverter's substitution string */
            uint8_t *currentSubChars = my_Converter_তথ্য->currentConverter->subChars;
            int8_t currentSubCharLen = my_Converter_তথ্য->currentConverter->subCharLen;

            /* set our substitution string into the subconverter */
            my_Converter_তথ্য->currentConverter->subChars = reinterpret_cast<uint8_t*>(subchar);
            my_Converter_তথ্য->currentConverter->subCharLen = static_cast<int8_t>(length);

            /* let the subconverter write the subchar, set/retrieve fromUChar32 state */
            args->converter = my_Converter_তথ্য->currentConverter;
            my_Converter_তথ্য->currentConverter->fromUChar32 = cnv->fromUChar32;
            ucnv_cbFromUWriteSub(args, 0, err);
            cnv->fromUChar32 = my_Converter_তথ্য->currentConverter->fromUChar32;
            args->converter = cnv;

            /* restore the subconverter's substitution string */
            my_Converter_তথ্য->currentConverter->subChars = currentSubChars;
            my_Converter_তথ্য->currentConverter->subCharLen = currentSubCharLen;

            যদি(*err == U_বাফার_OVERFLOW_ERROR) {
                যদি(my_Converter_তথ্য->currentConverter->char_Error_বাফার_দৈর্ঘ্য > 0) {
                    uprv_memcpy(
                        cnv->char_Error_বাফার,
                        my_Converter_তথ্য->currentConverter->char_Error_বাফার,
                        my_Converter_তথ্য->currentConverter->char_Error_বাফার_দৈর্ঘ্য);
                }
                cnv->char_Error_বাফার_দৈর্ঘ্য = my_Converter_তথ্য->currentConverter->char_Error_বাফার_দৈর্ঘ্য;
                my_Converter_তথ্য->currentConverter->char_Error_বাফার_দৈর্ঘ্য = 0;
            }
            ফেরত;
        }
    পূর্বনির্ধারিত:
        /* not expected */
        বিরতি;
    }
    ucnv_cbFromUWriteBytes(args,
                           বাফার, static_cast<int32_t>(p - বাফার),
                           offset_সূচক, err);
}

/*
 * Structure for cloning an ISO 2022 converter into a single memory block.
 */
কাঠামো cloneStruct
{
    UConverter cnv;
    UConverter currentConverter;
    UConverter_তথ্য_ISO2022 mydata;
};


U_CDECL_BEGIN

স্থির UConverter * U_CALLCONV
_ISO_2022_SafeClone(
            ধ্রুবক UConverter *cnv,
            শূন্য *স্তূপ_বাফার,
            int32_t *p_বাফার_আকার,
            UErrorCode *status)
{
    কাঠামো cloneStruct * localClone;
    UConverter_তথ্য_ISO2022 *cnv_তথ্য;
    int32_t i, size;

    যদি (U_FAILURE(*status)){
        ফেরত nullptr;
    }

    যদি (*p_বাফার_আকার == 0) { /* 'preflighting' request - set needed size into *pBufferSize */
        *p_বাফার_আকার = (int32_t)আকার(কাঠামো cloneStruct);
        ফেরত nullptr;
    }

    cnv_তথ্য = (UConverter_তথ্য_ISO2022 *)cnv->extraInfo;
    localClone = (কাঠামো cloneStruct *)স্তূপ_বাফার;

    /* ucnv.c/ucnv_safeClone() copied the main UConverter already */

    uprv_memcpy(&localClone->mydata, cnv_তথ্য, আকার(UConverter_তথ্য_ISO2022));
    localClone->cnv.extraInfo = &localClone->mydata; /* set pointer to extra data */
    localClone->cnv.isExtraLocal = সত্য;

    /* share the subconverters */

    যদি(cnv_তথ্য->currentConverter != nullptr) {
        size = (int32_t)আকার(UConverter);
        localClone->mydata.currentConverter =
            ucnv_safeClone(cnv_তথ্য->currentConverter,
                            &localClone->currentConverter,
                            &size, status);
        যদি(U_FAILURE(*status)) {
            ফেরত nullptr;
        }
    }

    জন্য(i=0; i<UCNV_2022_MAX_CONVERTERS; ++i) {
        যদি(cnv_তথ্য->myConverterArray[i] != nullptr) {
            ucnv_increment_Ref_গুনতি(cnv_তথ্য->myConverterArray[i]);
        }
    }

    ফেরত &localClone->cnv;
}

U_CDECL_END

স্থির শূন্য U_CALLCONV
_ISO_2022_GetUnicodeSet(ধ্রুবক UConverter *cnv,
                    ধ্রুবক USetAdder *sa,
                    UConverterUnicodeSet which,
                    UErrorCode *pErrorCode)
{
    int32_t i;
    UConverter_তথ্য_ISO2022* cnv_তথ্য;

    যদি (U_FAILURE(*pErrorCode)) {
        ফেরত;
    }
#ifdef U_ENABLE_GENERIC_ISO_2022
    যদি (cnv->shared_তথ্য == &ISO2022_তথ্য) {
        /* We use UTF-8 in this case */
        sa->addRange(sa->set, 0, 0xd7FF);
        sa->addRange(sa->set, 0xE000, 0x10FFFF);
        ফেরত;
    }
#endif

    cnv_তথ্য = static_cast<UConverter_তথ্য_ISO2022*>(cnv->extraInfo);

    /* open a set and initialize it with code points that are algorithmically round-tripped */
    নির্বাচন(cnv_তথ্য->locale[0]){
    ক্ষেত্র 'j':
        /* include JIS X 0201 which is hardcoded */
        sa->add(sa->set, 0xa5);
        sa->add(sa->set, 0x203e);
        যদি(jpCharsetMasks[cnv_তথ্য->version]&CSM(ISO8859_1)) {
            /* include Latin-1 for some variants of JP */
            sa->addRange(sa->set, 0, 0xff);
        } অন্যথা {
            /* include ASCII for JP */
            sa->addRange(sa->set, 0, 0x7f);
        }
        যদি(cnv_তথ্য->version==3 || cnv_তথ্য->version==4 || which==UCNV_ROUNDTRIP_AND_FALLBACK_SET) {
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
            sa->addRange(sa->set, HWKANA_START, HWKANA_END);
        }
        বিরতি;
#যদি !UCONFIG_ONLY_HTML_CONVERSION
    ক্ষেত্র 'c':
    ক্ষেত্র 'z':
        /* include ASCII for CN */
        sa->addRange(sa->set, 0, 0x7f);
        বিরতি;
    ক্ষেত্র 'k':
        /* there is only one converter for KR, and it is not in the myConverterArray[] */
        cnv_তথ্য->currentConverter->shared_তথ্য->impl->getUnicodeSet(
                cnv_তথ্য->currentConverter, sa, which, pErrorCode);
        /* the loop over myConverterArray[] will simply not find another converter */
        বিরতি;
#endif
    পূর্বনির্ধারিত:
        বিরতি;
    }

#যদি 0  /* Replaced by ucnv_MBCSGetFilteredUnicodeSetForUnicode() until we implement ucnv_getUnicodeSet() with reverse fallbacks. */
            যদি( (cnv_তথ্য->locale[0]=='c' || cnv_তথ্য->locale[0]=='z') &&
                cnv_তথ্য->version==0 && i==CNS_11643
            ) {
                /* special handling for non-EXT ISO-2022-CN: add only code points for CNS planes 1 and 2 */
                ucnv_MBCSGetUnicodeSetForBytes(
                        cnv_তথ্য->myConverterArray[i],
                        sa, UCNV_ROUNDTRIP_SET,
                        0, 0x81, 0x82,
                        pErrorCode);
            }
#endif

    জন্য (i=0; i<UCNV_2022_MAX_CONVERTERS; i++) {
        UConverterSetFilter filter;
        যদি(cnv_তথ্য->myConverterArray[i]!=nullptr) {
            যদি(cnv_তথ্য->locale[0]=='j' && i==JISX208) {
                /*
                 * Only add code points that map to Shift-JIS codes
                 * corresponding to JIS X 0208.
                 */
                filter=UCNV_SET_FILTER_SJIS;
#যদি !UCONFIG_ONLY_HTML_CONVERSION
            } অন্যথা যদি( (cnv_তথ্য->locale[0]=='c' || cnv_তথ্য->locale[0]=='z') &&
                       cnv_তথ্য->version==0 && i==CNS_11643) {
                /*
                 * Version-specific for CN:
                 * CN version 0 does not map CNS planes 3..7 although
                 * they are all available in the CNS conversion table;
                 * CN version 1 (-EXT) does map them all.
                 * The two versions create different Unicode sets.
                 */
                filter=UCNV_SET_FILTER_2022_CN;
            } অন্যথা যদি(i==KSC5601) {
                /*
                 * Some of the KSC 5601 tables (convrtrs.txt has this aliases on multiple tables)
                 * are broader than GR94.
                 */
                filter=UCNV_SET_FILTER_GR94DBCS;
#endif
            } অন্যথা {
                filter=UCNV_SET_FILTER_NONE;
            }
            ucnv_MBCSGetFilteredUnicodeSetForUnicode(cnv_তথ্য->myConverterArray[i], sa, which, filter, pErrorCode);
        }
    }

    /*
     * ISO 2022 converters must not convert SO/SI/ESC despite what
     * sub-converters do by themselves.
     * Remove these characters from the set.
     */
    sa->remove(sa->set, 0x0e);
    sa->remove(sa->set, 0x0f);
    sa->remove(sa->set, 0x1b);

    /* ISO 2022 converters do not convert C1 controls either */
    sa->removeRange(sa->set, 0x80, 0x9f);
}

স্থির ধ্রুবক UConverterImpl _ISO2022Impl={
    UCNV_ISO_2022,

    nullptr,
    nullptr,

    _ISO2022Open,
    _ISO2022Close,
    _ISO2022Reset,

#ifdef U_ENABLE_GENERIC_ISO_2022
    T_UConverter_toUnicode_ISO_2022_OFFSETS_LOGIC,
    T_UConverter_toUnicode_ISO_2022_OFFSETS_LOGIC,
    ucnv_fromUnicode_UTF8,
    ucnv_fromUnicode_UTF8_OFFSETS_LOGIC,
#অন্যথা
    nullptr,
    nullptr,
    nullptr,
    nullptr,
#endif
    nullptr,

    nullptr,
    _ISO2022getName,
    _ISO_2022_WriteSub,
    _ISO_2022_SafeClone,
    _ISO_2022_GetUnicodeSet,

    nullptr,
    nullptr
};
স্থির ধ্রুবক UConverter_Static_তথ্য ISO2022_Static_তথ্য={
    আকার(UConverter_Static_তথ্য),
    "ISO_2022",
    2022,
    UCNV_IBM,
    UCNV_ISO_2022,
    1,
    3, /* max 3 bytes per char16_t from UTF-8 (4 bytes from surrogate _pair_) */
    { 0x1a, 0, 0, 0 },
    1,
    মিথ্যা,
    মিথ্যা,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};
ধ্রুবক UConverter_Shared_তথ্য ISO2022_তথ্য=
        UCNV_IMMUTABLE_SHARED_তথ্য_INITIALIZER(&ISO2022_Static_তথ্য, &_ISO2022Impl);

/*************JP****************/
স্থির ধ্রুবক UConverterImpl _ISO2022JPImpl={
    UCNV_ISO_2022,

    nullptr,
    nullptr,

    _ISO2022Open,
    _ISO2022Close,
    _ISO2022Reset,

    UConverter_toUnicode_ISO_2022_JP_OFFSETS_LOGIC,
    UConverter_toUnicode_ISO_2022_JP_OFFSETS_LOGIC,
    UConverter_fromUnicode_ISO_2022_JP_OFFSETS_LOGIC,
    UConverter_fromUnicode_ISO_2022_JP_OFFSETS_LOGIC,
    nullptr,

    nullptr,
    _ISO2022getName,
    _ISO_2022_WriteSub,
    _ISO_2022_SafeClone,
    _ISO_2022_GetUnicodeSet,

    nullptr,
    nullptr
};
স্থির ধ্রুবক UConverter_Static_তথ্য ISO2022_JPStatic_তথ্য={
    আকার(UConverter_Static_তথ্য),
    "ISO_2022_JP",
    0,
    UCNV_IBM,
    UCNV_ISO_2022,
    1,
    6, /* max 6 bytes per char16_t: 4-byte escape sequence + DBCS */
    { 0x1a, 0, 0, 0 },
    1,
    মিথ্যা,
    মিথ্যা,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

নামাকাশ {

ধ্রুবক UConverter_Shared_তথ্য _ISO2022JPData=
        UCNV_IMMUTABLE_SHARED_তথ্য_INITIALIZER(&ISO2022_JPStatic_তথ্য, &_ISO2022JPImpl);

}  // namespace

#যদি !UCONFIG_ONLY_HTML_CONVERSION
/************* KR ***************/
স্থির ধ্রুবক UConverterImpl _ISO2022KRImpl={
    UCNV_ISO_2022,

    nullptr,
    nullptr,

    _ISO2022Open,
    _ISO2022Close,
    _ISO2022Reset,

    UConverter_toUnicode_ISO_2022_KR_OFFSETS_LOGIC,
    UConverter_toUnicode_ISO_2022_KR_OFFSETS_LOGIC,
    UConverter_fromUnicode_ISO_2022_KR_OFFSETS_LOGIC,
    UConverter_fromUnicode_ISO_2022_KR_OFFSETS_LOGIC,
    nullptr,

    nullptr,
    _ISO2022getName,
    _ISO_2022_WriteSub,
    _ISO_2022_SafeClone,
    _ISO_2022_GetUnicodeSet,

    nullptr,
    nullptr
};
স্থির ধ্রুবক UConverter_Static_তথ্য ISO2022_KRStatic_তথ্য={
    আকার(UConverter_Static_তথ্য),
    "ISO_2022_KR",
    0,
    UCNV_IBM,
    UCNV_ISO_2022,
    1,
    8, /* max 8 bytes per char16_t */
    { 0x1a, 0, 0, 0 },
    1,
    মিথ্যা,
    মিথ্যা,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

নামাকাশ {

ধ্রুবক UConverter_Shared_তথ্য _ISO2022KRData=
        UCNV_IMMUTABLE_SHARED_তথ্য_INITIALIZER(&ISO2022_KRStatic_তথ্য, &_ISO2022KRImpl);

}  // namespace

/*************** CN ***************/
স্থির ধ্রুবক UConverterImpl _ISO2022CNImpl={

    UCNV_ISO_2022,

    nullptr,
    nullptr,

    _ISO2022Open,
    _ISO2022Close,
    _ISO2022Reset,

    UConverter_toUnicode_ISO_2022_CN_OFFSETS_LOGIC,
    UConverter_toUnicode_ISO_2022_CN_OFFSETS_LOGIC,
    UConverter_fromUnicode_ISO_2022_CN_OFFSETS_LOGIC,
    UConverter_fromUnicode_ISO_2022_CN_OFFSETS_LOGIC,
    nullptr,

    nullptr,
    _ISO2022getName,
    _ISO_2022_WriteSub,
    _ISO_2022_SafeClone,
    _ISO_2022_GetUnicodeSet,

    nullptr,
    nullptr
};
স্থির ধ্রুবক UConverter_Static_তথ্য ISO2022_CNStatic_তথ্য={
    আকার(UConverter_Static_তথ্য),
    "ISO_2022_CN",
    0,
    UCNV_IBM,
    UCNV_ISO_2022,
    1,
    8, /* max 8 bytes per char16_t: 4-byte CNS designator + 2 bytes for SS2/SS3 + DBCS */
    { 0x1a, 0, 0, 0 },
    1,
    মিথ্যা,
    মিথ্যা,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

নামাকাশ {

ধ্রুবক UConverter_Shared_তথ্য _ISO2022CNData=
        UCNV_IMMUTABLE_SHARED_তথ্য_INITIALIZER(&ISO2022_CNStatic_তথ্য, &_ISO2022CNImpl);

}  // namespace
#endif /* #if !UCONFIG_ONLY_HTML_CONVERSION */

#endif /* #if !UCONFIG_NO_LEGACY_CONVERSION */
