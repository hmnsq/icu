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

#ضمن "unicode/utypes.h"

#إذا !UCONFIG_NO_CONVERSION && !UCONFIG_NO_LEGACY_CONVERSION

#ضمن "unicode/ucnv.h"
#ضمن "unicode/uset.h"
#ضمن "unicode/ucnv_err.h"
#ضمن "unicode/ucnv_cb.h"
#ضمن "unicode/utf16.h"
#ضمن "ucnv_imp.h"
#ضمن "ucnv_bld.h"
#ضمن "ucnv_cnv.h"
#ضمن "ucnvmbcs.h"
#ضمن "cstring.h"
#ضمن "cmemory.h"
#ضمن "uassert.h"

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

#إذا !UCONFIG_ONLY_HTML_CONVERSION
ساكن ثابت حرف SHIFT_IN_STR[]  = "\x0F";
// static const char SHIFT_OUT_STR[] = "\x0E";
#endif

#define CR      0x0D
#define LF      0x0A
#define H_TAB   0x09
#define V_TAB   0x0B
#define SPACE   0x20

تعداد {
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
تعداد {
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
تعريفنوع تعداد  {
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
#إذا UCONFIG_ONLY_HTML_CONVERSION
#define IS_JP_DBCS(cs) (JISX208==(cs))
#وإلا
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
#إذا UCONFIG_ONLY_HTML_CONVERSION
تعداد { MAX_JA_VERSION=0 };
#وإلا
تعداد { MAX_JA_VERSION=4 };
#endif
ساكن ثابت uint16_t jpCharsetMasks[MAX_JA_VERSION+1]={
    CSM(ASCII)|CSM(JISX201)|CSM(JISX208)|CSM(HWKANA_7BIT),
#إذا !UCONFIG_ONLY_HTML_CONVERSION
    CSM(ASCII)|CSM(JISX201)|CSM(JISX208)|CSM(HWKANA_7BIT)|CSM(JISX212),
    CSM(ASCII)|CSM(JISX201)|CSM(JISX208)|CSM(HWKANA_7BIT)|CSM(JISX212)|CSM(GB2312)|CSM(KSC5601)|CSM(ISO8859_1)|CSM(ISO8859_7),
    CSM(ASCII)|CSM(JISX201)|CSM(JISX208)|CSM(HWKANA_7BIT)|CSM(JISX212)|CSM(GB2312)|CSM(KSC5601)|CSM(ISO8859_1)|CSM(ISO8859_7),
    CSM(ASCII)|CSM(JISX201)|CSM(JISX208)|CSM(HWKANA_7BIT)|CSM(JISX212)|CSM(GB2312)|CSM(KSC5601)|CSM(ISO8859_1)|CSM(ISO8859_7)
#endif
};

تعريفنوع تعداد {
        ASCII1=0,
        LATIN1,
        SBCS,
        DBCS,
        MBCS,
        HWKANA
}Cnv2022Type;

تعريفنوع بنية ISO2022State {
    int8_t cs[4];       /* charset number for SI (G0)/SO (G1)/SS2 (G2)/SS3 (G3) */
    int8_t g;           /* 0..3 for G0..G3 (SI/SO/SS2/SS3) */
    int8_t prevG;       /* g before single shift (SS2 or SS3) */
} ISO2022State;

#define UCNV_OPTIONS_VERSION_MASK 0xf
#define UCNV_2022_MAX_CONVERTERS 10

تعريفنوع بنية{
    UConverterSharedData *myConverterArray[UCNV_2022_MAX_CONVERTERS];
    UConverter *currentConverter;
    Cnv2022Type currentType;
    ISO2022State toU2022State, fromU2022State;
    uint32_t مفتاح;
    uint32_t version;
#ifdef U_ENABLE_GENERIC_ISO_2022
    UBool isFirstBuffer;
#endif
    UBool isEmptySegment;
    حرف name[30];
    حرف locale[3];
}UConverterDataISO2022;

/* Protos */
/* ISO-2022 ----------------------------------------------------------------- */

/*Forward declaration */
U_CFUNC فراغ U_CALLCONV
ucnv_fromUnicode_UTF8(UConverterFromUnicodeArgs * args,
                      UErrorCode * err);
U_CFUNC فراغ U_CALLCONV
ucnv_fromUnicode_UTF8_OFFSETS_LOGIC(UConverterFromUnicodeArgs * args,
                                    UErrorCode * err);

#define ESC_2022 0x1B /*ESC*/

تعريفنوع تعداد
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
ساكن ثابت int8_t normalize_esq_chars_2022[256] = {
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
ساكن ثابت int32_t esc_Seq_State_Table_مفتاح_2022[MAX_STATES_2022] = {
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

ساكن ثابت حرف* ثابت escSeqStateTable_Result_2022[MAX_STATES_2022] = {
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

ساكن ثابت int8_t esc_Seq_State_Table_قيمة_2022[MAX_STATES_2022] = {
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
تعريفنوع تعداد{
#ifdef U_ENABLE_GENERIC_ISO_2022
    ISO_2022=0,
#endif
    ISO_2022_JP=1,
#إذا !UCONFIG_ONLY_HTML_CONVERSION
    ISO_2022_KR=2,
    ISO_2022_CN=3
#endif
} Variant2022;

/*********** ISO 2022 Converter Protos ***********/
ساكن فراغ U_CALLCONV
_ISO2022Open(UConverter *cnv, UConverterLoadArgs *pArgs, UErrorCode *errorCode);

ساكن فراغ U_CALLCONV
 _ISO2022Close(UConverter *converter);

ساكن فراغ U_CALLCONV
_ISO2022Reset(UConverter *converter, UConverterResetChoice choice);

U_CDECL_BEGIN
ساكن ثابت حرف * U_CALLCONV
_ISO2022getName(ثابت UConverter* cnv);
U_CDECL_END

ساكن فراغ  U_CALLCONV
_ISO_2022_WriteSub(UConverterFromUnicodeArgs *args, int32_t offsetIndex, UErrorCode *err);

U_CDECL_BEGIN
ساكن UConverter * U_CALLCONV
_ISO_2022_SafeClone(ثابت UConverter *cnv, فراغ *مكدس_Buffer, int32_t *pBufferSize, UErrorCode *status);

U_CDECL_END

#ifdef U_ENABLE_GENERIC_ISO_2022
ساكن فراغ U_CALLCONV
T_UConverter_toUnicode_ISO_2022_OFFSETS_LOGIC(UConverterToUnicodeArgs* args, UErrorCode* err);
#endif

نطاق {

/*const UConverterSharedData _ISO2022Data;*/
خارجي ثابت UConverterSharedData _ISO2022JPData;

#إذا !UCONFIG_ONLY_HTML_CONVERSION
خارجي ثابت UConverterSharedData _ISO2022KRData;
خارجي ثابت UConverterSharedData _ISO2022CNData;
#endif

}  // namespace

/*************** Converter implementations ******************/

/* The purpose of this function is to get around gcc compiler warnings. */
ساكن مضمن فراغ
fromUWriteUInt8(UConverter *cnv,
                 ثابت حرف *bytes, int32_t length,
                 uint8_t **target, ثابت حرف *targetLimit,
                 int32_t **offsets,
                 int32_t sourceIndex,
                 UErrorCode *pErrorCode)
{
    حرف* targetChars = reinterpret_cast<حرف*>(*target);
    ucnv_fromUWriteBytes(cnv, bytes, length, &targetChars, targetLimit,
                         offsets, sourceIndex, pErrorCode);
    *target = reinterpret_cast<uint8_t*>(targetChars);

}

ساكن مضمن فراغ
setInitialStateToUnicodeKR(UConverter* /*converter*/, UConverterDataISO2022 *myConverterData){
    إذا(myConverterData->version == 1) {
        UConverter *cnv = myConverterData->currentConverter;

        cnv->toUnicodeStatus=0;     /* offset */
        cnv->mode=0;                /* state */
        cnv->toULength=0;           /* byteIndex */
    }
}

ساكن مضمن فراغ
setInitialStateFromUnicodeKR(UConverter* converter,UConverterDataISO2022 *myConverterData){
   /* in ISO-2022-KR the designator sequence appears only once
    * in a file so we append it only once
    */
    إذا( converter->charErrorBufferLength==0){

        converter->charErrorBufferLength = 4;
        converter->charErrorBuffer[0] = 0x1b;
        converter->charErrorBuffer[1] = 0x24;
        converter->charErrorBuffer[2] = 0x29;
        converter->charErrorBuffer[3] = 0x43;
    }
    إذا(myConverterData->version == 1) {
        UConverter *cnv = myConverterData->currentConverter;

        cnv->fromUChar32=0;
        cnv->fromUnicodeStatus=1;   /* prevLength */
    }
}

ساكن فراغ U_CALLCONV
_ISO2022Open(UConverter *cnv, UConverterLoadArgs *pArgs, UErrorCode *errorCode){

    حرف myLocale[7]={' ',' ',' ',' ',' ',' ', '\0'};

    cnv->extraInfo = uprv_malloc (حجم (UConverterDataISO2022));
    إذا(cnv->extraInfo != nullptr) {
        UConverterNamePieces مكدس_Pieces;
        UConverterLoadArgs مكدس_Args=UCNV_LOAD_ARGS_INITIALIZER;
        UConverterDataISO2022* myConverterData = static_cast<UConverterDataISO2022*>(cnv->extraInfo);
        uint32_t version;

        مكدس_Args.onlyTestIsLoadable = pArgs->onlyTestIsLoadable;

        uprv_memset(myConverterData, 0, حجم(UConverterDataISO2022));
        myConverterData->currentType = ASCII1;
        cnv->fromUnicodeStatus =خطأ;
        إذا(pArgs->locale){
            uprv_strncpy(myLocale, pArgs->locale, حجم(myLocale)-1);
        }
        version = pArgs->options & UCNV_OPTIONS_VERSION_MASK;
        myConverterData->version = version;
        إذا(myLocale[0]=='j' && (myLocale[1]=='a'|| myLocale[1]=='p') &&
            (myLocale[2]=='_' || myLocale[2]=='\0'))
        {
            /* open the required converters and cache them */
            إذا(version>MAX_JA_VERSION) {
                // ICU 55 fails to open a converter for an unsupported version.
                // Previously, it fell back to version 0, but that would yield
                // unexpected behavior.
                *errorCode = U_MISSING_RESOURCE_ERROR;
                إرجاع;
            }
            إذا(jpCharsetMasks[version]&CSM(ISO8859_7)) {
                myConverterData->myConverterArray[ISO8859_7] =
                    ucnv_loadSharedData("ISO8859_7", &مكدس_Pieces, &مكدس_Args, errorCode);
            }
            myConverterData->myConverterArray[JISX208] =
                ucnv_loadSharedData("Shift-JIS", &مكدس_Pieces, &مكدس_Args, errorCode);
            إذا(jpCharsetMasks[version]&CSM(JISX212)) {
                myConverterData->myConverterArray[JISX212] =
                    ucnv_loadSharedData("jisx-212", &مكدس_Pieces, &مكدس_Args, errorCode);
            }
            إذا(jpCharsetMasks[version]&CSM(GB2312)) {
                myConverterData->myConverterArray[GB2312] =
                    ucnv_loadSharedData("ibm-5478", &مكدس_Pieces, &مكدس_Args, errorCode);   /* gb_2312_80-1 */
            }
            إذا(jpCharsetMasks[version]&CSM(KSC5601)) {
                myConverterData->myConverterArray[KSC5601] =
                    ucnv_loadSharedData("ksc_5601", &مكدس_Pieces, &مكدس_Args, errorCode);
            }

            /* set the function pointers to appropriate functions */
            cnv->sharedData = const_cast<UConverterSharedData*>(&_ISO2022JPData);
            uprv_strcpy(myConverterData->locale,"ja");

            (فراغ)uprv_strcpy(myConverterData->name,"ISO_2022,locale=ja,version=");
            size_t len = uprv_strlen(myConverterData->name);
            myConverterData->name[len] = static_cast<حرف>(myConverterData->version + static_cast<صحيح>('0'));
            myConverterData->name[len+1]='\0';
        }
#إذا !UCONFIG_ONLY_HTML_CONVERSION
        وإلا إذا(myLocale[0]=='k' && (myLocale[1]=='o'|| myLocale[1]=='r') &&
            (myLocale[2]=='_' || myLocale[2]=='\0'))
        {
            إذا(version>1) {
                // ICU 55 fails to open a converter for an unsupported version.
                // Previously, it fell back to version 0, but that would yield
                // unexpected behavior.
                *errorCode = U_MISSING_RESOURCE_ERROR;
                إرجاع;
            }
            ثابت حرف *cnvName;
            إذا(version==1) {
                cnvName="icu-internal-25546";
            } وإلا {
                cnvName="ibm-949";
                myConverterData->version=version=0;
            }
            إذا(pArgs->onlyTestIsLoadable) {
                ucnv_canCreateConverter(cnvName, errorCode);  /* errorCode carries result */
                uprv_free(cnv->extraInfo);
                cnv->extraInfo=nullptr;
                إرجاع;
            } وإلا {
                myConverterData->currentConverter=ucnv_open(cnvName, errorCode);
                إذا (U_FAILURE(*errorCode)) {
                    _ISO2022Close(cnv);
                    إرجاع;
                }

                إذا(version==1) {
                    (فراغ)uprv_strcpy(myConverterData->name,"ISO_2022,locale=ko,version=1");
                    uprv_memcpy(cnv->subChars, myConverterData->currentConverter->subChars, 4);
                    cnv->subCharLen = myConverterData->currentConverter->subCharLen;
                }وإلا{
                    (فراغ)uprv_strcpy(myConverterData->name,"ISO_2022,locale=ko,version=0");
                }

                /* initialize the state variables */
                setInitialStateToUnicodeKR(cnv, myConverterData);
                setInitialStateFromUnicodeKR(cnv, myConverterData);

                /* set the function pointers to appropriate functions */
                cnv->sharedData = const_cast<UConverterSharedData*>(&_ISO2022KRData);
                uprv_strcpy(myConverterData->locale,"ko");
            }
        }
        وإلا إذا(((myLocale[0]=='z' && myLocale[1]=='h') || (myLocale[0]=='c'&& myLocale[1]=='n'))&&
            (myLocale[2]=='_' || myLocale[2]=='\0'))
        {
            إذا(version>2) {
                // ICU 55 fails to open a converter for an unsupported version.
                // Previously, it fell back to version 0, but that would yield
                // unexpected behavior.
                *errorCode = U_MISSING_RESOURCE_ERROR;
                إرجاع;
            }

            /* open the required converters and cache them */
            myConverterData->myConverterArray[GB2312_1] =
                ucnv_loadSharedData("ibm-5478", &مكدس_Pieces, &مكدس_Args, errorCode);
            إذا(version>=1) {
                myConverterData->myConverterArray[ISO_IR_165] =
                    ucnv_loadSharedData("iso-ir-165", &مكدس_Pieces, &مكدس_Args, errorCode);
            }
            myConverterData->myConverterArray[CNS_11643] =
                ucnv_loadSharedData("cns-11643-1992", &مكدس_Pieces, &مكدس_Args, errorCode);


            /* set the function pointers to appropriate functions */
            cnv->sharedData = const_cast<UConverterSharedData*>(&_ISO2022CNData);
            uprv_strcpy(myConverterData->locale,"cn");

            إذا (version==0){
                myConverterData->version = 0;
                (فراغ)uprv_strcpy(myConverterData->name,"ISO_2022,locale=zh,version=0");
            }وإلا إذا (version==1){
                myConverterData->version = 1;
                (فراغ)uprv_strcpy(myConverterData->name,"ISO_2022,locale=zh,version=1");
            }وإلا {
                myConverterData->version = 2;
                (فراغ)uprv_strcpy(myConverterData->name,"ISO_2022,locale=zh,version=2");
            }
        }
#endif  // !UCONFIG_ONLY_HTML_CONVERSION
        وإلا{
#ifdef U_ENABLE_GENERIC_ISO_2022
            myConverterData->isFirstBuffer = true;

            /* append the UTF-8 escape sequence */
            cnv->charErrorBufferLength = 3;
            cnv->charErrorBuffer[0] = 0x1b;
            cnv->charErrorBuffer[1] = 0x25;
            cnv->charErrorBuffer[2] = 0x42;

            cnv->sharedData=(UConverterSharedData*)&_ISO2022Data;
            /* initialize the state variables */
            uprv_strcpy(myConverterData->name,"ISO_2022");
#وإلا
            *errorCode = U_MISSING_RESOURCE_ERROR;
            // Was U_UNSUPPORTED_ERROR but changed in ICU 55 to a more standard
            // data loading error code.
            إرجاع;
#endif
        }

        cnv->maxBytesPerUChar=cnv->sharedData->staticData->maxBytesPerChar;

        إذا(U_FAILURE(*errorCode) || pArgs->onlyTestIsLoadable) {
            _ISO2022Close(cnv);
        }
    } وإلا {
        *errorCode = U_MEMORY_ALLOCATION_ERROR;
    }
}


ساكن فراغ U_CALLCONV
_ISO2022Close(UConverter *converter) {
    UConverterDataISO2022* myData = static_cast<UConverterDataISO2022*>(converter->extraInfo);
    UConverterSharedData **array = myData->myConverterArray;
    int32_t i;

    إذا (converter->extraInfo != nullptr) {
        /*close the array of converter pointers and free the memory*/
        لكل (i=0; i<UCNV_2022_MAX_CONVERTERS; i++) {
            إذا(array[i]!=nullptr) {
                ucnv_unloadSharedDataIfReady(array[i]);
            }
        }

        ucnv_close(myData->currentConverter);

        إذا(!converter->isExtraLocal){
            uprv_free (converter->extraInfo);
            converter->extraInfo = nullptr;
        }
    }
}

ساكن فراغ U_CALLCONV
_ISO2022Reset(UConverter *converter, UConverterResetChoice choice) {
    UConverterDataISO2022* myConverterData = static_cast<UConverterDataISO2022*>(converter->extraInfo);
    إذا(choice<=UCNV_RESET_TO_UNICODE) {
        uprv_memset(&myConverterData->toU2022State, 0, حجم(ISO2022State));
        myConverterData->مفتاح = 0;
        myConverterData->isEmptySegment = خطأ;
    }
    إذا(choice!=UCNV_RESET_TO_UNICODE) {
        uprv_memset(&myConverterData->fromU2022State, 0, حجم(ISO2022State));
    }
#ifdef U_ENABLE_GENERIC_ISO_2022
    إذا(myConverterData->locale[0] == 0){
        إذا(choice<=UCNV_RESET_TO_UNICODE) {
            myConverterData->isFirstBuffer = true;
            myConverterData->مفتاح = 0;
            إذا (converter->mode == UCNV_SO){
                ucnv_close (myConverterData->currentConverter);
                myConverterData->currentConverter=nullptr;
            }
            converter->mode = UCNV_SI;
        }
        إذا(choice!=UCNV_RESET_TO_UNICODE) {
            /* re-append UTF-8 escape sequence */
            converter->charErrorBufferLength = 3;
            converter->charErrorBuffer[0] = 0x1b;
            converter->charErrorBuffer[1] = 0x28;
            converter->charErrorBuffer[2] = 0x42;
        }
    }
    وإلا
#endif
    {
        /* reset the state variables */
        إذا(myConverterData->locale[0] == 'k'){
            إذا(choice<=UCNV_RESET_TO_UNICODE) {
                setInitialStateToUnicodeKR(converter, myConverterData);
            }
            إذا(choice!=UCNV_RESET_TO_UNICODE) {
                setInitialStateFromUnicodeKR(converter, myConverterData);
            }
        }
    }
}

U_CDECL_BEGIN

ساكن ثابت حرف * U_CALLCONV
_ISO2022getName(ثابت UConverter* cnv){
    إذا(cnv->extraInfo){
        UConverterDataISO2022* myData= (UConverterDataISO2022*)cnv->extraInfo;
        إرجاع myData->name;
    }
    إرجاع nullptr;
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
ساكن ثابت int8_t nextStateToUnicodeJP[MAX_STATES_2022]= {
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

#إذا !UCONFIG_ONLY_HTML_CONVERSION
/*************** to unicode *******************/
ساكن ثابت int8_t nextStateToUnicodeCN[MAX_STATES_2022]= {
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


ساكن UCNV_TableStates_2022
get_مفتاح_2022(حرف c,int32_t* مفتاح,int32_t* offset){
    int32_t togo;
    int32_t أدنى = 0;
    int32_t hi = MAX_STATES_2022;
    int32_t oldmid=0;

    togo = normalize_esq_chars_2022[static_cast<uint8_t>(c)];
    إذا(togo == 0) {
        /* not a valid character anywhere in an escape sequence */
        *مفتاح = 0;
        *offset = 0;
        إرجاع INVALID_2022;
    }
    togo = (*مفتاح << 5) + togo;

    بينما (hi != أدنى)  /*بحث ثنائي*/{

        int32_t mid = (hi+أدنى) >> 1; /*Finds median*/

        إذا (mid == oldmid)
            اكسر;

        إذا (esc_Seq_State_Table_مفتاح_2022[mid] > togo){
            hi = mid;
        }
        وإلا إذا (esc_Seq_State_Table_مفتاح_2022[mid] < togo){
            أدنى = mid;
        }
        وإلا /*we found it*/{
            *مفتاح = togo;
            *offset = mid;
            إرجاع static_cast<UCNV_TableStates_2022>(esc_Seq_State_Table_قيمة_2022[mid]);
        }
        oldmid = mid;

    }

    *مفتاح = 0;
    *offset = 0;
    إرجاع INVALID_2022;
}

/*runs through a state machine to determine the escape sequence - codepage correspondence
 */
ساكن فراغ
changeState_2022(UConverter* _this,
                ثابت حرف** source,
                ثابت حرف* sourceLimit,
                Variant2022 var,
                UErrorCode* err){
    UCNV_TableStates_2022 value;
    UConverterDataISO2022* myData2022 = static_cast<UConverterDataISO2022*>(_this->extraInfo);
    uint32_t مفتاح = myData2022->مفتاح;
    int32_t offset = 0;
    int8_t initialToULength = _this->toULength;
    حرف c;

    value = VALID_NON_TERMINAL_2022;
    بينما (*source < sourceLimit) {
        c = *(*source)++;
        _this->toUBytes[_this->toULength++] = static_cast<uint8_t>(c);
        value = get_مفتاح_2022(c, reinterpret_cast<int32_t*>(&مفتاح), &offset);

        بدل (value){

        حالة VALID_NON_TERMINAL_2022 :
            /* continue with the loop */
            اكسر;

        حالة VALID_TERMINAL_2022:
            مفتاح = 0;
            اذهب DONE;

        حالة INVALID_2022:
            اذهب DONE;

        حالة VALID_MAYBE_TERMINAL_2022:
#ifdef U_ENABLE_GENERIC_ISO_2022
            /* ESC ( B is ambiguous only for ISO_2022 itself */
            إذا(var == ISO_2022) {
                /* discard toUBytes[] for ESC ( B because this sequence is correct and complete */
                _this->toULength = 0;

                /* TODO need to indicate that ESC ( B was seen; if failure, then need to replay from source or from MBCS-style replay */

                /* continue with the loop */
                value = VALID_NON_TERMINAL_2022;
                اكسر;
            } وإلا
#endif
            {
                /* not ISO_2022 itself, finish here */
                value = VALID_TERMINAL_2022;
                مفتاح = 0;
                اذهب DONE;
            }
        }
    }

DONE:
    myData2022->مفتاح = مفتاح;

    إذا (value == VALID_NON_TERMINAL_2022) {
        /* indicate that the escape sequence is incomplete: key!=0 */
        إرجاع;
    } وإلا إذا (value == INVALID_2022 ) {
        *err = U_ILLEGAL_ESCAPE_SEQUENCE;
    } وإلا /* value == VALID_TERMINAL_2022 */ {
        بدل(var){
#ifdef U_ENABLE_GENERIC_ISO_2022
        حالة ISO_2022:
        {
            ثابت حرف *chosenConverterName = escSeqStateTable_Result_2022[offset];
            إذا(chosenConverterName == nullptr) {
                /* SS2 or SS3 */
                *err = U_UNSUPPORTED_ESCAPE_SEQUENCE;
                _this->toUCallbackReason = UCNV_UNASSIGNED;
                إرجاع;
            }

            _this->mode = UCNV_SI;
            ucnv_close(myData2022->currentConverter);
            myData2022->currentConverter = myUConverter = ucnv_open(chosenConverterName, err);
            إذا(U_SUCCESS(*err)) {
                myUConverter->fromCharErrorBehaviour = UCNV_TO_U_CALLBACK_STOP;
                _this->mode = UCNV_SO;
            }
            اكسر;
        }
#endif
        حالة ISO_2022_JP:
            {
                StateEnum tempState = static_cast<StateEnum>(nextStateToUnicodeJP[offset]);
                بدل(tempState) {
                حالة INVALID_STATE:
                    *err = U_UNSUPPORTED_ESCAPE_SEQUENCE;
                    اكسر;
                حالة SS2_STATE:
                    إذا(myData2022->toU2022State.cs[2]!=0) {
                        إذا(myData2022->toU2022State.g<2) {
                            myData2022->toU2022State.prevG=myData2022->toU2022State.g;
                        }
                        myData2022->toU2022State.g=2;
                    } وإلا {
                        /* illegal to have SS2 before a matching designator */
                        *err = U_ILLEGAL_ESCAPE_SEQUENCE;
                    }
                    اكسر;
                /* case SS3_STATE: not used in ISO-2022-JP-x */
                حالة ISO8859_1:
                حالة ISO8859_7:
                    إذا((jpCharsetMasks[myData2022->version] & CSM(tempState)) == 0) {
                        *err = U_UNSUPPORTED_ESCAPE_SEQUENCE;
                    } وإلا {
                        /* G2 charset for SS2 */
                        myData2022->toU2022State.cs[2] = static_cast<int8_t>(tempState);
                    }
                    اكسر;
                افتراضي:
                    إذا((jpCharsetMasks[myData2022->version] & CSM(tempState)) == 0) {
                        *err = U_UNSUPPORTED_ESCAPE_SEQUENCE;
                    } وإلا {
                        /* G0 charset */
                        myData2022->toU2022State.cs[0] = static_cast<int8_t>(tempState);
                    }
                    اكسر;
                }
            }
            اكسر;
#إذا !UCONFIG_ONLY_HTML_CONVERSION
        حالة ISO_2022_CN:
            {
                StateEnum tempState = static_cast<StateEnum>(nextStateToUnicodeCN[offset]);
                بدل(tempState) {
                حالة INVALID_STATE:
                    *err = U_UNSUPPORTED_ESCAPE_SEQUENCE;
                    اكسر;
                حالة SS2_STATE:
                    إذا(myData2022->toU2022State.cs[2]!=0) {
                        إذا(myData2022->toU2022State.g<2) {
                            myData2022->toU2022State.prevG=myData2022->toU2022State.g;
                        }
                        myData2022->toU2022State.g=2;
                    } وإلا {
                        /* illegal to have SS2 before a matching designator */
                        *err = U_ILLEGAL_ESCAPE_SEQUENCE;
                    }
                    اكسر;
                حالة SS3_STATE:
                    إذا(myData2022->toU2022State.cs[3]!=0) {
                        إذا(myData2022->toU2022State.g<2) {
                            myData2022->toU2022State.prevG=myData2022->toU2022State.g;
                        }
                        myData2022->toU2022State.g=3;
                    } وإلا {
                        /* illegal to have SS3 before a matching designator */
                        *err = U_ILLEGAL_ESCAPE_SEQUENCE;
                    }
                    اكسر;
                حالة ISO_IR_165:
                    إذا(myData2022->version==0) {
                        *err = U_UNSUPPORTED_ESCAPE_SEQUENCE;
                        اكسر;
                    }
                    U_FALLTHROUGH;
                حالة GB2312_1:
                    U_FALLTHROUGH;
                حالة CNS_11643_1:
                    myData2022->toU2022State.cs[1] = static_cast<int8_t>(tempState);
                    اكسر;
                حالة CNS_11643_2:
                    myData2022->toU2022State.cs[2] = static_cast<int8_t>(tempState);
                    اكسر;
                افتراضي:
                    /* other CNS 11643 planes */
                    إذا(myData2022->version==0) {
                        *err = U_UNSUPPORTED_ESCAPE_SEQUENCE;
                    } وإلا {
                        myData2022->toU2022State.cs[3] = static_cast<int8_t>(tempState);
                    }
                    اكسر;
                }
            }
            اكسر;
        حالة ISO_2022_KR:
            إذا(offset==0x30){
                /* nothing to be done, just accept this one escape sequence */
            } وإلا {
                *err = U_UNSUPPORTED_ESCAPE_SEQUENCE;
            }
            اكسر;
#endif  // !UCONFIG_ONLY_HTML_CONVERSION

        افتراضي:
            *err = U_ILLEGAL_ESCAPE_SEQUENCE;
            اكسر;
        }
    }
    إذا(U_SUCCESS(*err)) {
        _this->toULength = 0;
    } وإلا إذا(*err==U_ILLEGAL_ESCAPE_SEQUENCE) {
        إذا(_this->toULength>1) {
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
            int8_t bytesFromThisBuffer=_this->toULength-initialToULength;
            إذا(backOutDistance<=bytesFromThisBuffer) {
                /* same as initialToULength<=1 */
                *source-=backOutDistance;
            } وإلا {
                /* Back out bytes from the previous buffer: Need to replay them. */
                _this->preToULength = static_cast<int8_t>(bytesFromThisBuffer - backOutDistance);
                /* same as -(initialToULength-1) */
                /* preToULength is negative! */
                uprv_memcpy(_this->preToU, _this->toUBytes+1, -_this->preToULength);
                *source-=bytesFromThisBuffer;
            }
            _this->toULength=1;
        }
    } وإلا إذا(*err==U_UNSUPPORTED_ESCAPE_SEQUENCE) {
        _this->toUCallbackReason = UCNV_UNASSIGNED;
    }
}

#إذا !UCONFIG_ONLY_HTML_CONVERSION
/*Checks the characters of the buffer against valid 2022 escape sequences
*if the match we return a pointer to the initial start of the sequence otherwise
*we return sourceLimit
*/
/*for 2022 looks ahead in the stream
 *to determine the longest possible convertible
 *data stream
 */
ساكن مضمن ثابت حرف*
getEndOfBuffer_2022(ثابت حرف** source,
                   ثابت حرف* sourceLimit,
                   UBool /*flush*/){

    ثابت حرف* mySource = *source;

#ifdef U_ENABLE_GENERIC_ISO_2022
    إذا (*source >= sourceLimit)
        إرجاع sourceLimit;

    افعل{

        إذا (*mySource == ESC_2022){
            int8_t i;
            int32_t مفتاح = 0;
            int32_t offset;
            UCNV_TableStates_2022 value = VALID_NON_TERMINAL_2022;

            /* Kludge: I could not
            * figure out the reason for validating an escape sequence
            * twice - once here and once in changeState_2022().
            * is it possible to have an ESC character in a ISO2022
            * byte stream which is valid in a code page? Is it legal?
            */
            لكل (i=0;
            (mySource+i < sourceLimit)&&(value == VALID_NON_TERMINAL_2022);
            i++) {
                value =  get_مفتاح_2022(*(mySource+i), &مفتاح, &offset);
            }
            إذا (value > 0 || *mySource==ESC_2022)
                إرجاع mySource;

            إذا ((value == VALID_NON_TERMINAL_2022)&&(!flush) )
                إرجاع sourceLimit;
        }
    }بينما (++mySource < sourceLimit);

    إرجاع sourceLimit;
#وإلا
    بينما(mySource < sourceLimit && *mySource != ESC_2022) {
        ++mySource;
    }
    إرجاع mySource;
#endif
}
#endif

/* This inline function replicates code in _MBCSFromUChar32() function in ucnvmbcs.c
 * any future change in _MBCSFromUChar32() function should be reflected here.
 * @return number of bytes in *value; negative number if fallback; 0 if no mapping
 */
ساكن مضمن int32_t
MBCS_FROM_UCHAR32_ISO2022(UConverterSharedData* sharedData,
                                         UChar32 c,
                                         uint32_t* value,
                                         UBool useFallback,
                                         صحيح outputType)
{
    ثابت int32_t *cx;
    ثابت uint16_t *table;
    uint32_t stage2Entry;
    uint32_t my_قيمة;
    int32_t length;
    ثابت uint8_t *p;
    /*
     * TODO(markus): Use and require new, faster MBCS conversion table structures.
     * Use internal version of ucnv_open() that verifies that the new structures are available,
     * else U_INTERNAL_PROGRAM_ERROR.
     */
    /* BMP-only codepages are stored without stage 1 entries for supplementary code points */
    إذا(c<0x10000 || (sharedData->mbcs.unicodeMask&UCNV_HAS_SUPPLEMENTARY)) {
        table=sharedData->mbcs.fromUnicodeTable;
        stage2Entry=MBCS_STAGE_2_FROM_U(table, c);
        /* get the bytes and the length for the output */
        إذا(outputType==MBCS_OUTPUT_2){
            my_قيمة=MBCS_قيمة_2_FROM_STAGE_2(sharedData->mbcs.fromUnicodeBytes, stage2Entry, c);
            إذا(my_قيمة<=0xff) {
                length=1;
            } وإلا {
                length=2;
            }
        } وإلا /* outputType==MBCS_OUTPUT_3 */ {
            p=MBCS_POINTER_3_FROM_STAGE_2(sharedData->mbcs.fromUnicodeBytes, stage2Entry, c);
            my_قيمة = (static_cast<uint32_t>(*p) << 16) | (static_cast<uint32_t>(p[1]) << 8) | p[2];
            إذا(my_قيمة<=0xff) {
                length=1;
            } وإلا إذا(my_قيمة<=0xffff) {
                length=2;
            } وإلا {
                length=3;
            }
        }
        /* is this code point assigned, or do we use fallbacks? */
        إذا((stage2Entry&(1<<(16+(c&0xf))))!=0) {
            /* assigned */
            *value=my_قيمة;
            إرجاع length;
        } وإلا إذا(FROM_U_USE_FALLBACK(useFallback, c) && my_قيمة!=0) {
            /*
             * We allow a 0 byte output if the "assigned" bit is set for this entry.
             * There is no way with this data structure for fallback output
             * to be a zero byte.
             */
            *value=my_قيمة;
            إرجاع -length;
        }
    }

    cx=sharedData->mbcs.extIndexes;
    إذا(cx!=nullptr) {
        إرجاع ucnv_extSimpleMatchFromU(cx, c, value, useFallback);
    }

    /* unassigned */
    إرجاع 0;
}

/* This inline function replicates code in _MBCSSingleFromUChar32() function in ucnvmbcs.c
 * any future change in _MBCSSingleFromUChar32() function should be reflected here.
 * @param retval pointer to output byte
 * @return 1 roundtrip byte  0 no mapping  -1 fallback byte
 */
ساكن مضمن int32_t
MBCS_SINGLE_FROM_UCHAR32(UConverterSharedData* sharedData,
                                       UChar32 c,
                                       uint32_t* retval,
                                       UBool useFallback)
{
    ثابت uint16_t *table;
    int32_t value;
    /* BMP-only codepages are stored without stage 1 entries for supplementary code points */
    إذا(c>=0x10000 && !(sharedData->mbcs.unicodeMask&UCNV_HAS_SUPPLEMENTARY)) {
        إرجاع 0;
    }
    /* convert the Unicode code point in c into codepage bytes (same as in _MBCSFromUnicodeWithOffsets) */
    table=sharedData->mbcs.fromUnicodeTable;
    /* get the byte for the output */
    value=MBCS_SINGLE_RESULT_FROM_U(table, (uint16_t *)sharedData->mbcs.fromUnicodeBytes, c);
    /* is this code point assigned, or do we use fallbacks? */
    *retval = static_cast<uint32_t>(value & 0xff);
    إذا(value>=0xf00) {
        إرجاع 1;  /* roundtrip */
    } وإلا إذا(useFallback ? value>=0x800 : value>=0xc00) {
        إرجاع -1;  /* fallback taken */
    } وإلا {
        إرجاع 0;  /* no mapping */
    }
}

/*
 * Check that the result is a 2-byte value with each byte in the range A1..FE
 * (strict EUC DBCS) before accepting it and subtracting 0x80 from each byte
 * to move it to the ISO 2022 range 21..7E.
 * Return 0 if out of range.
 */
ساكن مضمن uint32_t
_2022FromGR94DBCS(uint32_t value) {
    إذا (static_cast<uint16_t>(value - 0xa1a1) <= (0xfefe - 0xa1a1) &&
        static_cast<uint8_t>(value - 0xa1) <= (0xfe - 0xa1)
    ) {
        إرجاع value - 0x8080;  /* shift down to 21..7e byte range */
    } وإلا {
        إرجاع 0;  /* not valid for ISO 2022 */
    }
}

#إذا 0 /* 5691: Call sites now check for validity. They can just += 0x8080 after that. */
/*
 * This method does the reverse of _2022FromGR94DBCS(). Given the 2022 code point, it returns the
 * 2 byte value that is in the range A1..FE for each byte. Otherwise it returns the 2022 code point
 * unchanged. 
 */
ساكن مضمن uint32_t
_2022ToGR94DBCS(uint32_t value) {
    uint32_t return_قيمة = value + 0x8080;
    إذا( (uint16_t)(return_قيمة - 0xa1a1) <= (0xfefe - 0xa1a1) &&
        (uint8_t)(return_قيمة - 0xa1) <= (0xfe - 0xa1)) {
        إرجاع return_قيمة;
    } وإلا {
        إرجاع value;
    }
}
#endif

#ifdef U_ENABLE_GENERIC_ISO_2022

/**********************************************************************************
*  ISO-2022 Converter
*
*
*/

ساكن فراغ U_CALLCONV
T_UConverter_toUnicode_ISO_2022_OFFSETS_LOGIC(UConverterToUnicodeArgs* args,
                                                           UErrorCode* err){
    ثابت حرف* mySourceLimit, *realSourceLimit;
    ثابت حرف* sourceStart;
    ثابت char16_t* myTargetStart;
    UConverter* saveThis;
    UConverterDataISO2022* myData;
    int8_t length;

    saveThis = args->converter;
    myData=((UConverterDataISO2022*)(saveThis->extraInfo));

    realSourceLimit = args->sourceLimit;
    بينما (args->source < realSourceLimit) {
        إذا(myData->مفتاح == 0) { /* are we in the middle of an escape sequence? */
            /*Find the end of the buffer e.g : Next Escape Seq | end of Buffer*/
            mySourceLimit = getEndOfBuffer_2022(&(args->source), realSourceLimit, args->flush);

            إذا(args->source < mySourceLimit) {
                إذا(myData->currentConverter==nullptr) {
                    myData->currentConverter = ucnv_open("ASCII",err);
                    إذا(U_FAILURE(*err)){
                        إرجاع;
                    }

                    myData->currentConverter->fromCharErrorBehaviour = UCNV_TO_U_CALLBACK_STOP;
                    saveThis->mode = UCNV_SO;
                }

                /* convert to before the ESC or until the end of the buffer */
                myData->isFirstBuffer=خطأ;
                sourceStart = args->source;
                myTargetStart = args->target;
                args->converter = myData->currentConverter;
                ucnv_toUnicode(args->converter,
                    &args->target,
                    args->targetLimit,
                    &args->source,
                    mySourceLimit,
                    args->offsets,
                    (UBool)(args->flush && mySourceLimit == realSourceLimit),
                    err);
                args->converter = saveThis;

                إذا (*err == U_BUFFER_OVERFLOW_ERROR) {
                    /* move the overflow buffer */
                    length = saveThis->UCharErrorBufferLength = myData->currentConverter->UCharErrorBufferLength;
                    myData->currentConverter->UCharErrorBufferLength = 0;
                    إذا(length > 0) {
                        uprv_memcpy(saveThis->UCharErrorBuffer,
                                    myData->currentConverter->UCharErrorBuffer,
                                    length*U_SIZEOF_UCHAR);
                    }
                    إرجاع;
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
                إذا (U_FAILURE(*err) ||
                    (args->source == realSourceLimit) ||
                    (args->offsets != nullptr && (args->target != myTargetStart || args->source != sourceStart) ||
                    (mySourceLimit < realSourceLimit && myData->currentConverter->toULength > 0))
                ) {
                    /* copy partial or error input for truncated detection and error handling */
                    إذا(U_FAILURE(*err)) {
                        length = saveThis->invalidCharLength = myData->currentConverter->invalidCharLength;
                        إذا(length > 0) {
                            uprv_memcpy(saveThis->invalidCharBuffer, myData->currentConverter->invalidCharBuffer, length);
                        }
                    } وإلا {
                        length = saveThis->toULength = myData->currentConverter->toULength;
                        إذا(length > 0) {
                            uprv_memcpy(saveThis->toUBytes, myData->currentConverter->toUBytes, length);
                            إذا(args->source < mySourceLimit) {
                                *err = U_TRUNCATED_CHAR_FOUND; /* truncated input before ESC */
                            }
                        }
                    }
                    إرجاع;
                }
            }
        }

        sourceStart = args->source;
        changeState_2022(args->converter,
               &(args->source),
               realSourceLimit,
               ISO_2022,
               err);
        إذا (U_FAILURE(*err) || (args->source != sourceStart && args->offsets != nullptr)) {
            /* let the ucnv.c code update its current offset */
            إرجاع;
        }
    }
}

#endif

/*
 * To Unicode Callback helper function
 */
ساكن فراغ
toUnicodeCallback(UConverter *cnv,
                  ثابت uint32_t sourceChar, ثابت uint32_t targetUniChar,
                  UErrorCode* err){
    إذا(sourceChar>0xff){
        cnv->toUBytes[0] = static_cast<uint8_t>(sourceChar >> 8);
        cnv->toUBytes[1] = static_cast<uint8_t>(sourceChar);
        cnv->toULength = 2;
    }
    وإلا{
        cnv->toUBytes[0] = static_cast<حرف>(sourceChar);
        cnv->toULength = 1;
    }

    إذا(targetUniChar == (missingCharMarker-1/*0xfffe*/)){
        *err = U_INVALID_CHAR_FOUND;
    }
    وإلا{
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
ساكن ثابت StateEnum jpCharsetPref[]={
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
ساكن ثابت حرف escSeqChars[][6] ={
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
ساكن  ثابت int8_t escSeqCharsLen[] ={
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
ساكن مضمن uint32_t
jisx201ToU(uint32_t value) {
    إذا(value < 0x5c) {
        إرجاع value;
    } وإلا إذا(value == 0x5c) {
        إرجاع 0xa5;
    } وإلا إذا(value == 0x7e) {
        إرجاع 0x203e;
    } وإلا /* value <= 0x7f */ {
        إرجاع value;
    }
}

/* Map Unicode to 00..7F according to JIS X 0201. Return U+FFFE if unmappable. */
ساكن مضمن uint32_t
jisx201FromU(uint32_t value) {
    إذا(value<=0x7f) {
        إذا(value!=0x5c && value!=0x7e) {
            إرجاع value;
        }
    } وإلا إذا(value==0xa5) {
        إرجاع 0x5c;
    } وإلا إذا(value==0x203e) {
        إرجاع 0x7e;
    }
    إرجاع 0xfffe;
}

/*
 * Take a valid Shift-JIS byte pair, check that it is in the range corresponding
 * to JIS X 0208, and convert it to a pair of 21..7E bytes.
 * Return 0 if the byte pair is out of range.
 */
ساكن مضمن uint32_t
_2022FromSJIS(uint32_t value) {
    uint8_t trail;

    إذا(value > 0xEFFC) {
        إرجاع 0;  /* beyond JIS X 0208 */
    }

    trail = static_cast<uint8_t>(value);

    value &= 0xff00;  /* lead byte */
    إذا(value <= 0x9f00) {
        value -= 0x7000;
    } وإلا /* 0xe000 <= value <= 0xef00 */ {
        value -= 0xb000;
    }
    value <<= 1;

    إذا(trail <= 0x9e) {
        value -= 0x100;
        إذا(trail <= 0x7e) {
            value |= trail - 0x1f;
        } وإلا {
            value |= trail - 0x20;
        }
    } وإلا /* trail <= 0xfc */ {
        value |= trail - 0x7e;
    }
    إرجاع value;
}

/*
 * Convert a pair of JIS X 0208 21..7E bytes to Shift-JIS.
 * If either byte is outside 21..7E make sure that the result is not valid
 * for Shift-JIS so that the converter catches it.
 * Some invalid byte values already turn into equally invalid Shift-JIS
 * byte values and need not be tested explicitly.
 */
ساكن مضمن فراغ
_2022ToSJIS(uint8_t c1, uint8_t c2, حرف bytes[2]) {
    إذا(c1&1) {
        ++c1;
        إذا(c2 <= 0x5f) {
            c2 += 0x1f;
        } وإلا إذا(c2 <= 0x7e) {
            c2 += 0x20;
        } وإلا {
            c2 = 0;  /* invalid */
        }
    } وإلا {
        إذا (static_cast<uint8_t>(c2 - 0x21) <= ((0x7e) - 0x21)) {
            c2 += 0x7e;
        } وإلا {
            c2 = 0;  /* invalid */
        }
    }
    c1 >>= 1;
    إذا(c1 <= 0x2f) {
        c1 += 0x70;
    } وإلا إذا(c1 <= 0x3f) {
        c1 += 0xb0;
    } وإلا {
        c1 = 0;  /* invalid */
    }
    bytes[0] = static_cast<حرف>(c1);
    bytes[1] = static_cast<حرف>(c2);
}

/*
 * JIS X 0208 has fallbacks from Unicode half-width Katakana to full-width (DBCS)
 * Katakana.
 * Now that we use a Shift-JIS table for JIS X 0208 we need to hardcode these fallbacks
 * because Shift-JIS roundtrips half-width Katakana to single bytes.
 * These were the only fallbacks in ICU's jisx-208.ucm file.
 */
ساكن ثابت uint16_t hwkana_fb[HWKANA_END - HWKANA_START + 1] = {
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

ساكن فراغ U_CALLCONV
UConverter_fromUnicode_ISO_2022_JP_OFFSETS_LOGIC(UConverterFromUnicodeArgs* args, UErrorCode* err) {
    UConverter *cnv = args->converter;
    UConverterDataISO2022 *converterData;
    ISO2022State *pFromU2022State;
    uint8_t* target = reinterpret_cast<uint8_t*>(args->target);
    ثابت uint8_t* targetLimit = reinterpret_cast<ثابت uint8_t*>(args->targetLimit);
    ثابت char16_t* source = args->source;
    ثابت char16_t* sourceLimit = args->sourceLimit;
    int32_t* offsets = args->offsets;
    UChar32 sourceChar;
    حرف buffer[8];
    int32_t len, outLen;
    int8_t choices[10];
    int32_t choice_تعداد;
    uint32_t target_قيمة = 0;
    UBool useFallback;

    int32_t i;
    int8_t cs, g;

    /* set up the state */
    converterData = static_cast<UConverterDataISO2022*>(cnv->extraInfo);
    pFromU2022State   = &converterData->fromU2022State;

    choice_تعداد = 0;

    /* check if the last codepoint of previous buffer was a lead surrogate*/
    إذا((sourceChar = cnv->fromUChar32)!=0 && target< targetLimit) {
        اذهب getTrail;
    }

    بينما(source < sourceLimit) {
        إذا(target < targetLimit) {

            sourceChar  = *(source++);
            /*check if the char is a First surrogate*/
            إذا(U16_IS_SURROGATE(sourceChar)) {
                إذا(U16_IS_SURROGATE_LEAD(sourceChar)) {
getTrail:
                    /*look ahead to find the trail surrogate*/
                    إذا(source < sourceLimit) {
                        /* test the following code unit */
                        char16_t trail = *source;
                        إذا(U16_IS_TRAIL(trail)) {
                            source++;
                            sourceChar=U16_GET_SUPPLEMENTARY(sourceChar, trail);
                            cnv->fromUChar32=0x00;
                            /* convert this supplementary code point */
                            /* exit this condition tree */
                        } وإلا {
                            /* this is an unmatched lead code unit (1st surrogate) */
                            /* callback(illegal) */
                            *err=U_ILLEGAL_CHAR_FOUND;
                            cnv->fromUChar32=sourceChar;
                            اكسر;
                        }
                    } وإلا {
                        /* no more input */
                        cnv->fromUChar32=sourceChar;
                        اكسر;
                    }
                } وإلا {
                    /* this is an unmatched trail code unit (2nd surrogate) */
                    /* callback(illegal) */
                    *err=U_ILLEGAL_CHAR_FOUND;
                    cnv->fromUChar32=sourceChar;
                    اكسر;
                }
            }

            /* do not convert SO/SI/ESC */
            إذا(IS_2022_CONTROL(sourceChar)) {
                /* callback(illegal) */
                *err=U_ILLEGAL_CHAR_FOUND;
                cnv->fromUChar32=sourceChar;
                اكسر;
            }

            /* do the conversion */

            إذا(choice_تعداد == 0) {
                uint16_t csm;

                /*
                 * The csm variable keeps track of which charsets are allowed
                 * and not used yet while building the choices[].
                 */
                csm = jpCharsetMasks[converterData->version];
                choice_تعداد = 0;

                /* JIS7/8: try single-byte half-width Katakana before JISX208 */
                إذا(converterData->version == 3 || converterData->version == 4) {
                    choices[choice_تعداد++] = static_cast<int8_t>(HWKANA_7BIT);
                }
                /* Do not try single-byte half-width Katakana for other versions. */
                csm &= ~CSM(HWKANA_7BIT);

                /* try the current G0 charset */
                choices[choice_تعداد++] = cs = pFromU2022State->cs[0];
                csm &= ~CSM(cs);

                /* try the current G2 charset */
                إذا((cs = pFromU2022State->cs[2]) != 0) {
                    choices[choice_تعداد++] = cs;
                    csm &= ~CSM(cs);
                }

                /* try all the other possible charsets */
                لكل(i = 0; i < UPRV_LENGTHOF(jpCharsetPref); ++i) {
                    cs = static_cast<int8_t>(jpCharsetPref[i]);
                    إذا(CSM(cs) & csm) {
                        choices[choice_تعداد++] = cs;
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

            لكل(i = 0; i < choice_تعداد && len <= 0; ++i) {
                uint32_t value;
                int32_t len2;
                int8_t cs0 = choices[i];
                بدل(cs0) {
                حالة ASCII:
                    إذا(sourceChar <= 0x7f) {
                        target_قيمة = static_cast<uint32_t>(sourceChar);
                        len = 1;
                        cs = cs0;
                        g = 0;
                    }
                    اكسر;
                حالة ISO8859_1:
                    إذا(GR96_START <= sourceChar && sourceChar <= GR96_END) {
                        target_قيمة = static_cast<uint32_t>(sourceChar) - 0x80;
                        len = 1;
                        cs = cs0;
                        g = 2;
                    }
                    اكسر;
                حالة HWKANA_7BIT:
                    إذا (static_cast<uint32_t>(sourceChar - HWKANA_START) <= (HWKANA_END - HWKANA_START)) {
                        إذا(converterData->version==3) {
                            /* JIS7: use G1 (SO) */
                            /* Shift U+FF61..U+FF9F to bytes 21..5F. */
                            target_قيمة = static_cast<uint32_t>(sourceChar - (HWKANA_START - 0x21));
                            len = 1;
                            pFromU2022State->cs[1] = cs = cs0; /* do not output an escape sequence */
                            g = 1;
                        } وإلا إذا(converterData->version==4) {
                            /* JIS8: use 8-bit bytes with any single-byte charset, see escape sequence output below */
                            /* Shift U+FF61..U+FF9F to bytes A1..DF. */
                            target_قيمة = static_cast<uint32_t>(sourceChar - (HWKANA_START - 0xa1));
                            len = 1;

                            cs = pFromU2022State->cs[0];
                            إذا(IS_JP_DBCS(cs)) {
                                /* switch from a DBCS charset to JISX201 */
                                cs = static_cast<int8_t>(JISX201);
                            }
                            /* else stay in the current G0 charset */
                            g = 0;
                        }
                        /* else do not use HWKANA_7BIT with other versions */
                    }
                    اكسر;
                حالة JISX201:
                    /* G0 SBCS */
                    value = jisx201FromU(sourceChar);
                    إذا(value <= 0x7f) {
                        target_قيمة = value;
                        len = 1;
                        cs = cs0;
                        g = 0;
                        useFallback = خطأ;
                    }
                    اكسر;
                حالة JISX208:
                    /* G0 DBCS from Shift-JIS table */
                    len2 = MBCS_FROM_UCHAR32_ISO2022(
                                converterData->myConverterArray[cs0],
                                sourceChar, &value,
                                useFallback, MBCS_OUTPUT_2);
                    إذا(len2 == 2 || (len2 == -2 && len == 0)) {  /* only accept DBCS: abs(len)==2 */
                        value = _2022FromSJIS(value);
                        إذا(value != 0) {
                            target_قيمة = value;
                            len = len2;
                            cs = cs0;
                            g = 0;
                            useFallback = خطأ;
                        }
                    } وإلا إذا(len == 0 && useFallback &&
                              static_cast<uint32_t>(sourceChar - HWKANA_START) <= (HWKANA_END - HWKANA_START)) {
                        target_قيمة = hwkana_fb[sourceChar - HWKANA_START];
                        len = -2;
                        cs = cs0;
                        g = 0;
                        useFallback = خطأ;
                    }
                    اكسر;
                حالة ISO8859_7:
                    /* G0 SBCS forced to 7-bit output */
                    len2 = MBCS_SINGLE_FROM_UCHAR32(
                                converterData->myConverterArray[cs0],
                                sourceChar, &value,
                                useFallback);
                    إذا(len2 != 0 && !(len2 < 0 && len != 0) && GR96_START <= value && value <= GR96_END) {
                        target_قيمة = value - 0x80;
                        len = len2;
                        cs = cs0;
                        g = 2;
                        useFallback = خطأ;
                    }
                    اكسر;
                افتراضي:
                    /* G0 DBCS */
                    len2 = MBCS_FROM_UCHAR32_ISO2022(
                                converterData->myConverterArray[cs0],
                                sourceChar, &value,
                                useFallback, MBCS_OUTPUT_2);
                    إذا(len2 == 2 || (len2 == -2 && len == 0)) {  /* only accept DBCS: abs(len)==2 */
                        إذا(cs0 == KSC5601) {
                            /*
                             * Check for valid bytes for the encoding scheme.
                             * This is necessary because the sub-converter (windows-949)
                             * has a broader encoding scheme than is valid for 2022.
                             */
                            value = _2022FromGR94DBCS(value);
                            إذا(value == 0) {
                                اكسر;
                            }
                        }
                        target_قيمة = value;
                        len = len2;
                        cs = cs0;
                        g = 0;
                        useFallback = خطأ;
                    }
                    اكسر;
                }
            }

            إذا(len != 0) {
                إذا(len < 0) {
                    len = -len;  /* fallback */
                }
                outLen = 0; /* count output bytes */

                /* write SI if necessary (only for JIS7) */
                إذا(pFromU2022State->g == 1 && g == 0) {
                    buffer[outLen++] = UCNV_SI;
                    pFromU2022State->g = 0;
                }

                /* write the designation sequence if necessary */
                إذا(cs != pFromU2022State->cs[g]) {
                    int32_t escLen = escSeqCharsLen[cs];
                    uprv_memcpy(buffer + outLen, escSeqChars[cs], escLen);
                    outLen += escLen;
                    pFromU2022State->cs[g] = cs;

                    /* invalidate the choices[] */
                    choice_تعداد = 0;
                }

                /* write the shift sequence if necessary */
                إذا(g != pFromU2022State->g) {
                    بدل(g) {
                    /* case 0 handled before writing escapes */
                    حالة 1:
                        buffer[outLen++] = UCNV_SO;
                        pFromU2022State->g = 1;
                        اكسر;
                    افتراضي: /* case 2 */
                        buffer[outLen++] = 0x1b;
                        buffer[outLen++] = 0x4e;
                        اكسر;
                    /* no case 3: no SS3 in ISO-2022-JP-x */
                    }
                }

                /* write the output bytes */
                إذا(len == 1) {
                    buffer[outLen++] = static_cast<حرف>(target_قيمة);
                } وإلا /* len == 2 */ {
                    buffer[outLen++] = static_cast<حرف>(target_قيمة >> 8);
                    buffer[outLen++] = static_cast<حرف>(target_قيمة);
                }
            } وإلا {
                /*
                 * if we cannot find the character after checking all codepages
                 * then this is an error
                 */
                *err = U_INVALID_CHAR_FOUND;
                cnv->fromUChar32=sourceChar;
                اكسر;
            }

            إذا(sourceChar == CR || sourceChar == LF) {
                /* reset the G2 state at the end of a line (conversion got us into ASCII or JISX201 already) */
                pFromU2022State->cs[2] = 0;
                choice_تعداد = 0;
            }

            /* output outLen>0 bytes in buffer[] */
            إذا(outLen == 1) {
                *target++ = buffer[0];
                إذا(offsets) {
                    *offsets++ = static_cast<int32_t>(source - args->source - 1); /* -1: known to be ASCII */
                }
            } وإلا إذا(outLen == 2 && (target + 2) <= targetLimit) {
                *target++ = buffer[0];
                *target++ = buffer[1];
                إذا(offsets) {
                    int32_t sourceIndex = static_cast<int32_t>(source - args->source - U16_LENGTH(sourceChar));
                    *offsets++ = sourceIndex;
                    *offsets++ = sourceIndex;
                }
            } وإلا {
                fromUWriteUInt8(
                    cnv,
                    buffer, outLen,
                    &target, reinterpret_cast<ثابت حرف*>(targetLimit),
                    &offsets, static_cast<int32_t>(source - args->source - U16_LENGTH(sourceChar)),
                    err);
                إذا(U_FAILURE(*err)) {
                    اكسر;
                }
            }
        } /* end if(myTargetIndex<myTargetLength) */
        وإلا{
            *err =U_BUFFER_OVERFLOW_ERROR;
            اكسر;
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
    إذا( U_SUCCESS(*err) &&
        (pFromU2022State->g!=0 || pFromU2022State->cs[0]!=ASCII) &&
        args->flush && source>=sourceLimit && cnv->fromUChar32==0
    ) {
        int32_t sourceIndex;

        outLen = 0;

        إذا(pFromU2022State->g != 0) {
            buffer[outLen++] = UCNV_SI;
            pFromU2022State->g = 0;
        }

        إذا(pFromU2022State->cs[0] != ASCII) {
            int32_t escLen = escSeqCharsLen[ASCII];
            uprv_memcpy(buffer + outLen, escSeqChars[ASCII], escLen);
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
        sourceIndex = static_cast<int32_t>(source - args->source);
        إذا(sourceIndex>0) {
            --sourceIndex;
            إذا( U16_IS_TRAIL(args->source[sourceIndex]) &&
                (sourceIndex==0 || U16_IS_LEAD(args->source[sourceIndex-1]))
            ) {
                --sourceIndex;
            }
        } وإلا {
            sourceIndex=-1;
        }

        fromUWriteUInt8(
            cnv,
            buffer, outLen,
            &target, reinterpret_cast<ثابت حرف*>(targetLimit),
            &offsets, sourceIndex,
            err);
    }

    /*save the state and return */
    args->source = source;
    args->target = reinterpret_cast<حرف*>(target);
}

/*************** to unicode *******************/

ساكن فراغ U_CALLCONV
UConverter_toUnicode_ISO_2022_JP_OFFSETS_LOGIC(UConverterToUnicodeArgs *args,
                                               UErrorCode* err){
    حرف tempBuf[2];
    ثابت حرف* mySource = const_cast<حرف*>(args->source);
    char16_t *myTarget = args->target;
    ثابت حرف *mySourceLimit = args->sourceLimit;
    uint32_t targetUniChar = 0x0000;
    uint32_t mySourceChar = 0x0000;
    uint32_t tmpSourceChar = 0x0000;
    UConverterDataISO2022* myData;
    ISO2022State *pToU2022State;
    StateEnum cs;

    myData = static_cast<UConverterDataISO2022*>(args->converter->extraInfo);
    pToU2022State = &myData->toU2022State;

    إذا(myData->مفتاح != 0) {
        /* continue with a partial escape sequence */
        اذهب escape;
    } وإلا إذا(args->converter->toULength == 1 && mySource < mySourceLimit && myTarget < args->targetLimit) {
        /* continue with a partial double-byte character */
        mySourceChar = args->converter->toUBytes[0];
        args->converter->toULength = 0;
        cs = static_cast<StateEnum>(pToU2022State->cs[pToU2022State->g]);
        targetUniChar = missingCharMarker;
        اذهب getTrailByte;
    }

    بينما(mySource < mySourceLimit){

        targetUniChar =missingCharMarker;

        إذا(myTarget < args->targetLimit){

            mySourceChar = static_cast<unsigned حرف>(*mySource++);

            بدل(mySourceChar) {
            حالة UCNV_SI:
                إذا(myData->version==3) {
                    pToU2022State->g=0;
                    استمر;
                } وإلا {
                    /* only JIS7 uses SI/SO, not ISO-2022-JP-x */
                    myData->isEmptySegment = خطأ;	/* reset this, we have a different error */
                    اكسر;
                }

            حالة UCNV_SO:
                إذا(myData->version==3) {
                    /* JIS7: switch to G1 half-width Katakana */
                    pToU2022State->cs[1] = static_cast<int8_t>(HWKANA_7BIT);
                    pToU2022State->g=1;
                    استمر;
                } وإلا {
                    /* only JIS7 uses SI/SO, not ISO-2022-JP-x */
                    myData->isEmptySegment = خطأ;	/* reset this, we have a different error */
                    اكسر;
                }

            حالة ESC_2022:
                mySource--;
escape:
                {
                    ثابت حرف * mySourceBefore = mySource;
                    int8_t toULengthBefore = args->converter->toULength;

                    changeState_2022(args->converter,&(mySource),
                        mySourceLimit, ISO_2022_JP,err);

                    /* If in ISO-2022-JP only and we successfully completed an escape sequence, but previous segment was empty, create an error */
                    إذا(myData->version==0 && myData->مفتاح==0 && U_SUCCESS(*err) && myData->isEmptySegment) {
                        *err = U_ILLEGAL_ESCAPE_SEQUENCE;
                        args->converter->toUCallbackReason = UCNV_IRREGULAR;
                        args->converter->toULength = static_cast<int8_t>(toULengthBefore + (mySource - mySourceBefore));
                    }
                }

                /* invalid or illegal escape sequence */
                إذا(U_FAILURE(*err)){
                    args->target = myTarget;
                    args->source = mySource;
                    myData->isEmptySegment = خطأ;	/* Reset to avoid future spurious errors */
                    إرجاع;
                }
                /* If we successfully completed an escape sequence, we begin a new segment, empty so far */
                إذا(myData->مفتاح==0) {
                    myData->isEmptySegment = true;
                }
                استمر;

            /* ISO-2022-JP does not use single-byte (C1) SS2 and SS3 */

            حالة CR:
            حالة LF:
                /* automatically reset to single-byte mode */
                إذا (static_cast<StateEnum>(pToU2022State->cs[0]) != ASCII &&
                    static_cast<StateEnum>(pToU2022State->cs[0]) != JISX201) {
                    pToU2022State->cs[0] = static_cast<int8_t>(ASCII);
                }
                pToU2022State->cs[2] = 0;
                pToU2022State->g = 0;
                U_FALLTHROUGH;
            افتراضي:
                /* convert one or two bytes */
                myData->isEmptySegment = خطأ;
                cs = static_cast<StateEnum>(pToU2022State->cs[pToU2022State->g]);
                إذا (static_cast<uint8_t>(mySourceChar - 0xa1) <= (0xdf - 0xa1) && myData->version == 4 &&
                    !IS_JP_DBCS(cs)
                ) {
                    /* 8-bit halfwidth katakana in any single-byte mode for JIS8 */
                    targetUniChar = mySourceChar + (HWKANA_START - 0xa1);

                    /* return from a single-shift state to the previous one */
                    إذا(pToU2022State->g >= 2) {
                        pToU2022State->g=pToU2022State->prevG;
                    }
                } وإلا بدل(cs) {
                حالة ASCII:
                    إذا(mySourceChar <= 0x7f) {
                        targetUniChar = mySourceChar;
                    }
                    اكسر;
                حالة ISO8859_1:
                    إذا(mySourceChar <= 0x7f) {
                        targetUniChar = mySourceChar + 0x80;
                    }
                    /* return from a single-shift state to the previous one */
                    pToU2022State->g=pToU2022State->prevG;
                    اكسر;
                حالة ISO8859_7:
                    إذا(mySourceChar <= 0x7f) {
                        /* convert mySourceChar+0x80 to use a normal 8-bit table */
                        targetUniChar =
                            _MBCS_SINGLE_SIMPLE_GET_NEXT_BMP(
                                myData->myConverterArray[cs],
                                mySourceChar + 0x80);
                    }
                    /* return from a single-shift state to the previous one */
                    pToU2022State->g=pToU2022State->prevG;
                    اكسر;
                حالة JISX201:
                    إذا(mySourceChar <= 0x7f) {
                        targetUniChar = jisx201ToU(mySourceChar);
                    }
                    اكسر;
                حالة HWKANA_7BIT:
                    إذا (static_cast<uint8_t>(mySourceChar - 0x21) <= (0x5f - 0x21)) {
                        /* 7-bit halfwidth Katakana */
                        targetUniChar = mySourceChar + (HWKANA_START - 0x21);
                    }
                    اكسر;
                افتراضي:
                    /* G0 DBCS */
                    إذا(mySource < mySourceLimit) {
                        صحيح leadIsOk, trailIsOk;
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
                        إذا (leadIsOk && trailIsOk) {
                            ++mySource;
                            tmpSourceChar = (mySourceChar << 8) | trailByte;
                            إذا(cs == JISX208) {
                                _2022ToSJIS(static_cast<uint8_t>(mySourceChar), trailByte, tempBuf);
                                mySourceChar = tmpSourceChar;
                            } وإلا {
                                /* Copy before we modify tmpSourceChar so toUnicodeCallback() sees the correct bytes. */
                                mySourceChar = tmpSourceChar;
                                إذا (cs == KSC5601) {
                                    tmpSourceChar += 0x8080;  /* = _2022ToGR94DBCS(tmpSourceChar) */
                                }
                                tempBuf[0] = static_cast<حرف>(tmpSourceChar >> 8);
                                tempBuf[1] = static_cast<حرف>(tmpSourceChar);
                            }
                            targetUniChar = ucnv_MBCSSimpleGetNextUChar(myData->myConverterArray[cs], tempBuf, 2, خطأ);
                        } وإلا إذا (!(trailIsOk || IS_2022_CONTROL(trailByte))) {
                            /* report a pair of illegal bytes if the second byte is not a DBCS starter */
                            ++mySource;
                            /* add another bit so that the code below writes 2 bytes in case of error */
                            mySourceChar = 0x10000 | (mySourceChar << 8) | trailByte;
                        }
                    } وإلا {
                        args->converter->toUBytes[0] = static_cast<uint8_t>(mySourceChar);
                        args->converter->toULength = 1;
                        اذهب endloop;
                    }
                }  /* End of inner switch */
                اكسر;
            }  /* End of outer switch */
            إذا(targetUniChar < (missingCharMarker-1/*0xfffe*/)){
                إذا(args->offsets){
                    args->offsets[myTarget - args->target] = static_cast<int32_t>(mySource - args->source - (mySourceChar <= 0xff ? 1 : 2));
                }
                *(myTarget++) = static_cast<char16_t>(targetUniChar);
            }
            وإلا إذا(targetUniChar > missingCharMarker){
                /* disassemble the surrogate pair and write to output*/
                targetUniChar-=0x0010000;
                *myTarget = static_cast<char16_t>(0xd800 + static_cast<char16_t>(targetUniChar >> 10));
                إذا(args->offsets){
                    args->offsets[myTarget - args->target] = static_cast<int32_t>(mySource - args->source - (mySourceChar <= 0xff ? 1 : 2));
                }
                ++myTarget;
                إذا(myTarget< args->targetLimit){
                    *myTarget = static_cast<char16_t>(0xdc00 + static_cast<char16_t>(targetUniChar & 0x3ff));
                    إذا(args->offsets){
                        args->offsets[myTarget - args->target] = static_cast<int32_t>(mySource - args->source - (mySourceChar <= 0xff ? 1 : 2));
                    }
                    ++myTarget;
                }وإلا{
                    args->converter->UCharErrorBuffer[args->converter->UCharErrorBufferLength++]=
                                    static_cast<char16_t>(0xdc00 + static_cast<char16_t>(targetUniChar & 0x3ff));
                }

            }
            وإلا{
                /* Call the callback function*/
                toUnicodeCallback(args->converter,mySourceChar,targetUniChar,err);
                اكسر;
            }
        }
        وإلا{    /* goes with "if(myTarget < args->targetLimit)"  way up near top of function */
            *err =U_BUFFER_OVERFLOW_ERROR;
            اكسر;
        }
    }
endloop:
    args->target = myTarget;
    args->source = mySource;
}


#إذا !UCONFIG_ONLY_HTML_CONVERSION
/***************************************************************
*   Rules for ISO-2022-KR encoding
*   i) The KSC5601 designator sequence should appear only once in a file,
*      at the beginning of a line before any KSC5601 characters. This usually
*      means that it appears by itself on the first line of the file
*  ii) There are only 2 shifting sequences SO to shift into double byte mode
*      and SI to shift into single byte mode
*/
ساكن فراغ U_CALLCONV
UConverter_fromUnicode_ISO_2022_KR_OFFSETS_LOGIC_IBM(UConverterFromUnicodeArgs* args, UErrorCode* err){

    UConverter* saveConv = args->converter;
    UConverterDataISO2022* myConverterData = static_cast<UConverterDataISO2022*>(saveConv->extraInfo);
    args->converter=myConverterData->currentConverter;

    myConverterData->currentConverter->fromUChar32 = saveConv->fromUChar32;
    ucnv_MBCSFromUnicodeWithOffsets(args,err);
    saveConv->fromUChar32 = myConverterData->currentConverter->fromUChar32;

    إذا(*err == U_BUFFER_OVERFLOW_ERROR) {
        إذا(myConverterData->currentConverter->charErrorBufferLength > 0) {
            uprv_memcpy(
                saveConv->charErrorBuffer,
                myConverterData->currentConverter->charErrorBuffer,
                myConverterData->currentConverter->charErrorBufferLength);
        }
        saveConv->charErrorBufferLength = myConverterData->currentConverter->charErrorBufferLength;
        myConverterData->currentConverter->charErrorBufferLength = 0;
    }
    args->converter=saveConv;
}

ساكن فراغ U_CALLCONV
UConverter_fromUnicode_ISO_2022_KR_OFFSETS_LOGIC(UConverterFromUnicodeArgs* args, UErrorCode* err){

    ثابت char16_t *source = args->source;
    ثابت char16_t *sourceLimit = args->sourceLimit;
    unsigned حرف *target = reinterpret_cast<unsigned حرف*>(args->target);
    unsigned حرف *targetLimit = reinterpret_cast<unsigned حرف*>(const_cast<حرف*>(args->targetLimit));
    int32_t* offsets = args->offsets;
    uint32_t targetByteUnit = 0x0000;
    UChar32 sourceChar = 0x0000;
    UBool isTargetByteDBCS;
    UBool oldIsTargetByteDBCS;
    UConverterDataISO2022 *converterData;
    UConverterSharedData* sharedData;
    UBool useFallback;
    int32_t length =0;

    converterData = static_cast<UConverterDataISO2022*>(args->converter->extraInfo);
    /* if the version is 1 then the user is requesting
     * conversion with ibm-25546 pass the arguments to
     * MBCS converter and return
     */
    إذا(converterData->version==1){
        UConverter_fromUnicode_ISO_2022_KR_OFFSETS_LOGIC_IBM(args,err);
        إرجاع;
    }

    /* initialize data */
    sharedData = converterData->currentConverter->sharedData;
    useFallback = args->converter->useFallback;
    isTargetByteDBCS = static_cast<UBool>(args->converter->fromUnicodeStatus);
    oldIsTargetByteDBCS = isTargetByteDBCS;

    isTargetByteDBCS = static_cast<UBool>(args->converter->fromUnicodeStatus);
    إذا((sourceChar = args->converter->fromUChar32)!=0 && target <targetLimit) {
        اذهب getTrail;
    }
    بينما(source < sourceLimit){

        targetByteUnit = missingCharMarker;

        إذا(target < (unsigned حرف*) args->targetLimit){
            sourceChar = *source++;

            /* do not convert SO/SI/ESC */
            إذا(IS_2022_CONTROL(sourceChar)) {
                /* callback(illegal) */
                *err=U_ILLEGAL_CHAR_FOUND;
                args->converter->fromUChar32=sourceChar;
                اكسر;
            }

            length = MBCS_FROM_UCHAR32_ISO2022(sharedData,sourceChar,&targetByteUnit,useFallback,MBCS_OUTPUT_2);
            إذا(length < 0) {
                length = -length;  /* fallback */
            }
            /* only DBCS or SBCS characters are expected*/
            /* DB characters with high bit set to 1 are expected */
            إذا( length > 2 || length==0 ||
                (length == 1 && targetByteUnit > 0x7f) ||
                (length == 2 &&
                    (static_cast<uint16_t>(targetByteUnit - 0xa1a1) > (0xfefe - 0xa1a1) ||
                    static_cast<uint8_t>(targetByteUnit - 0xa1) > (0xfe - 0xa1)))
            ) {
                targetByteUnit=missingCharMarker;
            }
            إذا (targetByteUnit != missingCharMarker){

                oldIsTargetByteDBCS = isTargetByteDBCS;
                isTargetByteDBCS = static_cast<UBool>(targetByteUnit > 0x00FF);
                  /* append the shift sequence */
                إذا (oldIsTargetByteDBCS != isTargetByteDBCS ){

                    إذا (isTargetByteDBCS)
                        *target++ = UCNV_SO;
                    وإلا
                        *target++ = UCNV_SI;
                    إذا(offsets)
                        *(offsets++) = static_cast<int32_t>(source - args->source - 1);
                }
                /* write the targetUniChar  to target */
                إذا(targetByteUnit <= 0x00FF){
                    إذا( target < targetLimit){
                        *(target++) = static_cast<unsigned حرف>(targetByteUnit);
                        إذا(offsets){
                            *(offsets++) = static_cast<int32_t>(source - args->source - 1);
                        }

                    }وإلا{
                        args->converter->charErrorBuffer[args->converter->charErrorBufferLength++] = static_cast<unsigned حرف>(targetByteUnit);
                        *err = U_BUFFER_OVERFLOW_ERROR;
                    }
                }وإلا{
                    إذا(target < targetLimit){
                        *(target++) = static_cast<unsigned حرف>((targetByteUnit >> 8) - 0x80);
                        إذا(offsets){
                            *(offsets++) = static_cast<int32_t>(source - args->source - 1);
                        }
                        إذا(target < targetLimit){
                            *(target++) = static_cast<unsigned حرف>(targetByteUnit - 0x80);
                            إذا(offsets){
                                *(offsets++) = static_cast<int32_t>(source - args->source - 1);
                            }
                        }وإلا{
                            args->converter->charErrorBuffer[args->converter->charErrorBufferLength++] = static_cast<unsigned حرف>(targetByteUnit - 0x80);
                            *err = U_BUFFER_OVERFLOW_ERROR;
                        }
                    }وإلا{
                        args->converter->charErrorBuffer[args->converter->charErrorBufferLength++] = static_cast<unsigned حرف>((targetByteUnit >> 8) - 0x80);
                        args->converter->charErrorBuffer[args->converter->charErrorBufferLength++] = static_cast<unsigned حرف>(targetByteUnit - 0x80);
                        *err = U_BUFFER_OVERFLOW_ERROR;
                    }
                }

            }
            وإلا{
                /* oops.. the code point is unassingned
                 * set the error and reason
                 */

                /*check if the char is a First surrogate*/
                إذا(U16_IS_SURROGATE(sourceChar)) {
                    إذا(U16_IS_SURROGATE_LEAD(sourceChar)) {
getTrail:
                        /*look ahead to find the trail surrogate*/
                        إذا(source <  sourceLimit) {
                            /* test the following code unit */
                            char16_t trail = *source;
                            إذا(U16_IS_TRAIL(trail)) {
                                source++;
                                sourceChar=U16_GET_SUPPLEMENTARY(sourceChar, trail);
                                *err = U_INVALID_CHAR_FOUND;
                                /* convert this surrogate code point */
                                /* exit this condition tree */
                            } وإلا {
                                /* this is an unmatched lead code unit (1st surrogate) */
                                /* callback(illegal) */
                                *err=U_ILLEGAL_CHAR_FOUND;
                            }
                        } وإلا {
                            /* no more input */
                            *err = U_ZERO_ERROR;
                        }
                    } وإلا {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *err=U_ILLEGAL_CHAR_FOUND;
                    }
                } وإلا {
                    /* callback(unassigned) for a BMP code point */
                    *err = U_INVALID_CHAR_FOUND;
                }

                args->converter->fromUChar32=sourceChar;
                اكسر;
            }
        } /* end if(myTargetIndex<myTargetLength) */
        وإلا{
            *err =U_BUFFER_OVERFLOW_ERROR;
            اكسر;
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
    إذا( U_SUCCESS(*err) &&
        isTargetByteDBCS &&
        args->flush && source>=sourceLimit && args->converter->fromUChar32==0
    ) {
        int32_t sourceIndex;

        /* we are switching to ASCII */
        isTargetByteDBCS=خطأ;

        /* get the source index of the last input character */
        /*
         * TODO this would be simpler and more reliable if we used a pair
         * of sourceIndex/prevSourceIndex like in ucnvmbcs.c
         * so that we could simply use the prevSourceIndex here;
         * this code gives an incorrect result for the rare case of an unmatched
         * trail surrogate that is alone in the last buffer of the text stream
         */
        sourceIndex = static_cast<int32_t>(source - args->source);
        إذا(sourceIndex>0) {
            --sourceIndex;
            إذا( U16_IS_TRAIL(args->source[sourceIndex]) &&
                (sourceIndex==0 || U16_IS_LEAD(args->source[sourceIndex-1]))
            ) {
                --sourceIndex;
            }
        } وإلا {
            sourceIndex=-1;
        }

        fromUWriteUInt8(
            args->converter,
            SHIFT_IN_STR, 1,
            &target, reinterpret_cast<ثابت حرف*>(targetLimit),
            &offsets, sourceIndex,
            err);
    }

    /*save the state and return */
    args->source = source;
    args->target = reinterpret_cast<حرف*>(target);
    args->converter->fromUnicodeStatus = static_cast<uint32_t>(isTargetByteDBCS);
}

/************************ To Unicode ***************************************/

ساكن فراغ U_CALLCONV
UConverter_toUnicode_ISO_2022_KR_OFFSETS_LOGIC_IBM(UConverterToUnicodeArgs *args,
                                                            UErrorCode* err){
    حرف ثابت* sourceStart;
    UConverterDataISO2022* myData = static_cast<UConverterDataISO2022*>(args->converter->extraInfo);

    UConverterToUnicodeArgs subArgs;
    int32_t minArgsSize;

    /* set up the subconverter arguments */
    إذا(args->size<حجم(UConverterToUnicodeArgs)) {
        minArgsSize = args->size;
    } وإلا {
        minArgsSize = static_cast<int32_t>(حجم(UConverterToUnicodeArgs));
    }

    uprv_memcpy(&subArgs, args, minArgsSize);
    subArgs.size = static_cast<uint16_t>(minArgsSize);
    subArgs.converter = myData->currentConverter;

    /* remember the original start of the input for offsets */
    sourceStart = args->source;

    إذا(myData->مفتاح != 0) {
        /* continue with a partial escape sequence */
        اذهب escape;
    }

    بينما(U_SUCCESS(*err) && args->source < args->sourceLimit) {
        /*Find the end of the buffer e.g : Next Escape Seq | end of Buffer*/
        subArgs.source = args->source;
        subArgs.sourceLimit = getEndOfBuffer_2022(&(args->source), args->sourceLimit, args->flush);
        إذا(subArgs.source != subArgs.sourceLimit) {
            /*
             * get the current partial byte sequence
             *
             * it needs to be moved between the public and the subconverter
             * so that the conversion framework, which only sees the public
             * converter, can handle truncated and illegal input etc.
             */
            إذا(args->converter->toULength > 0) {
                uprv_memcpy(subArgs.converter->toUBytes, args->converter->toUBytes, args->converter->toULength);
            }
            subArgs.converter->toULength = args->converter->toULength;

            /*
             * Convert up to the end of the input, or to before the next escape character.
             * Does not handle conversion extensions because the preToU[] state etc.
             * is not copied.
             */
            ucnv_MBCSToUnicodeWithOffsets(&subArgs, err);

            إذا(args->offsets != nullptr && sourceStart != args->source) {
                /* update offsets to base them on the actual start of the input */
                int32_t *offsets = args->offsets;
                char16_t *target = args->target;
                int32_t delta = static_cast<int32_t>(args->source - sourceStart);
                بينما(target < subArgs.target) {
                    إذا(*offsets >= 0) {
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
            إذا(subArgs.converter->toULength > 0) {
                uprv_memcpy(args->converter->toUBytes, subArgs.converter->toUBytes, subArgs.converter->toULength);
            }
            args->converter->toULength = subArgs.converter->toULength;

            إذا(*err == U_BUFFER_OVERFLOW_ERROR) {
                إذا(subArgs.converter->UCharErrorBufferLength > 0) {
                    uprv_memcpy(args->converter->UCharErrorBuffer, subArgs.converter->UCharErrorBuffer,
                                subArgs.converter->UCharErrorBufferLength);
                }
                args->converter->UCharErrorBufferLength=subArgs.converter->UCharErrorBufferLength;
                subArgs.converter->UCharErrorBufferLength = 0;
            }
        }

        إذا (U_FAILURE(*err) || (args->source == args->sourceLimit)) {
            إرجاع;
        }

escape:
        changeState_2022(args->converter,
               &(args->source),
               args->sourceLimit,
               ISO_2022_KR,
               err);
    }
}

ساكن فراغ U_CALLCONV
UConverter_toUnicode_ISO_2022_KR_OFFSETS_LOGIC(UConverterToUnicodeArgs *args,
                                                            UErrorCode* err){
    حرف tempBuf[2];
    ثابت حرف* mySource = const_cast<حرف*>(args->source);
    char16_t *myTarget = args->target;
    ثابت حرف *mySourceLimit = args->sourceLimit;
    UChar32 targetUniChar = 0x0000;
    char16_t mySourceChar = 0x0000;
    UConverterDataISO2022* myData;
    UConverterSharedData* sharedData ;
    UBool useFallback;

    myData = static_cast<UConverterDataISO2022*>(args->converter->extraInfo);
    إذا(myData->version==1){
        UConverter_toUnicode_ISO_2022_KR_OFFSETS_LOGIC_IBM(args,err);
        إرجاع;
    }

    /* initialize state */
    sharedData = myData->currentConverter->sharedData;
    useFallback = args->converter->useFallback;

    إذا(myData->مفتاح != 0) {
        /* continue with a partial escape sequence */
        اذهب escape;
    } وإلا إذا(args->converter->toULength == 1 && mySource < mySourceLimit && myTarget < args->targetLimit) {
        /* continue with a partial double-byte character */
        mySourceChar = args->converter->toUBytes[0];
        args->converter->toULength = 0;
        اذهب getTrailByte;
    }

    بينما(mySource< mySourceLimit){

        إذا(myTarget < args->targetLimit){

            mySourceChar = static_cast<unsigned حرف>(*mySource++);

            إذا(mySourceChar==UCNV_SI){
                myData->toU2022State.g = 0;
                إذا (myData->isEmptySegment) {
                    myData->isEmptySegment = خطأ;	/* we are handling it, reset to avoid future spurious errors */
                    *err = U_ILLEGAL_ESCAPE_SEQUENCE;
                    args->converter->toUCallbackReason = UCNV_IRREGULAR;
                    args->converter->toUBytes[0] = static_cast<uint8_t>(mySourceChar);
                    args->converter->toULength = 1;
                    args->target = myTarget;
                    args->source = mySource;
                    إرجاع;
                }
                /*consume the source */
                استمر;
            }وإلا إذا(mySourceChar==UCNV_SO){
                myData->toU2022State.g = 1;
                myData->isEmptySegment = true;	/* Begin a new segment, empty so far */
                /*consume the source */
                استمر;
            }وإلا إذا(mySourceChar==ESC_2022){
                mySource--;
escape:
                myData->isEmptySegment = خطأ;	/* Any invalid ESC sequences will be detected separately, so just reset this */
                changeState_2022(args->converter,&(mySource),
                                mySourceLimit, ISO_2022_KR, err);
                إذا(U_FAILURE(*err)){
                    args->target = myTarget;
                    args->source = mySource;
                    إرجاع;
                }
                استمر;
            }

            myData->isEmptySegment = خطأ;	/* Any invalid char errors will be detected separately, so just reset this */
            إذا(myData->toU2022State.g == 1) {
                إذا(mySource < mySourceLimit) {
                    صحيح leadIsOk, trailIsOk;
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
                    إذا (leadIsOk && trailIsOk) {
                        ++mySource;
                        tempBuf[0] = static_cast<حرف>(mySourceChar + 0x80);
                        tempBuf[1] = static_cast<حرف>(trailByte + 0x80);
                        targetUniChar = ucnv_MBCSSimpleGetNextUChar(sharedData, tempBuf, 2, useFallback);
                        mySourceChar = (mySourceChar << 8) | trailByte;
                    } وإلا إذا (!(trailIsOk || IS_2022_CONTROL(trailByte))) {
                        /* report a pair of illegal bytes if the second byte is not a DBCS starter */
                        ++mySource;
                        /* add another bit so that the code below writes 2 bytes in case of error */
                        mySourceChar = static_cast<char16_t>(0x10000 | (mySourceChar << 8) | trailByte);
                    }
                } وإلا {
                    args->converter->toUBytes[0] = static_cast<uint8_t>(mySourceChar);
                    args->converter->toULength = 1;
                    اكسر;
                }
            }
            وإلا إذا(mySourceChar <= 0x7f) {
                targetUniChar = ucnv_MBCSSimpleGetNextUChar(sharedData, mySource - 1, 1, useFallback);
            } وإلا {
                targetUniChar = 0xffff;
            }
            إذا(targetUniChar < 0xfffe){
                إذا(args->offsets) {
                    args->offsets[myTarget - args->target] = static_cast<int32_t>(mySource - args->source - (mySourceChar <= 0xff ? 1 : 2));
                }
                *(myTarget++) = static_cast<char16_t>(targetUniChar);
            }
            وإلا {
                /* Call the callback function*/
                toUnicodeCallback(args->converter,mySourceChar,targetUniChar,err);
                اكسر;
            }
        }
        وإلا{
            *err =U_BUFFER_OVERFLOW_ERROR;
            اكسر;
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
ساكن ثابت حرف GB_2312_80_STR[] = "\x1B\x24\x29\x41";
ساكن ثابت حرف ISO_IR_165_STR[] = "\x1B\x24\x29\x45";
ساكن ثابت حرف CNS_11643_1992_Plane_1_STR[] = "\x1B\x24\x29\x47";
ساكن ثابت حرف CNS_11643_1992_Plane_2_STR[] = "\x1B\x24\x2A\x48";
ساكن ثابت حرف CNS_11643_1992_Plane_3_STR[] = "\x1B\x24\x2B\x49";
ساكن ثابت حرف CNS_11643_1992_Plane_4_STR[] = "\x1B\x24\x2B\x4A";
ساكن ثابت حرف CNS_11643_1992_Plane_5_STR[] = "\x1B\x24\x2B\x4B";
ساكن ثابت حرف CNS_11643_1992_Plane_6_STR[] = "\x1B\x24\x2B\x4C";
ساكن ثابت حرف CNS_11643_1992_Plane_7_STR[] = "\x1B\x24\x2B\x4D";

/********************** ISO2022-CN Data **************************/
ساكن ثابت حرف* ثابت escSeqCharsCN[10] ={
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

ساكن فراغ U_CALLCONV
UConverter_fromUnicode_ISO_2022_CN_OFFSETS_LOGIC(UConverterFromUnicodeArgs* args, UErrorCode* err){
    UConverter *cnv = args->converter;
    UConverterDataISO2022 *converterData;
    ISO2022State *pFromU2022State;
    uint8_t* target = reinterpret_cast<uint8_t*>(args->target);
    ثابت uint8_t* targetLimit = reinterpret_cast<ثابت uint8_t*>(args->targetLimit);
    ثابت char16_t* source = args->source;
    ثابت char16_t* sourceLimit = args->sourceLimit;
    int32_t* offsets = args->offsets;
    UChar32 sourceChar;
    حرف buffer[8];
    int32_t len;
    int8_t choices[3];
    int32_t choice_تعداد;
    uint32_t target_قيمة = 0;
    UBool useFallback;

    /* set up the state */
    converterData = static_cast<UConverterDataISO2022*>(cnv->extraInfo);
    pFromU2022State   = &converterData->fromU2022State;

    choice_تعداد = 0;

    /* check if the last codepoint of previous buffer was a lead surrogate*/
    إذا((sourceChar = cnv->fromUChar32)!=0 && target< targetLimit) {
        اذهب getTrail;
    }

    بينما( source < sourceLimit){
        إذا(target < targetLimit){

            sourceChar  = *(source++);
            /*check if the char is a First surrogate*/
             إذا(U16_IS_SURROGATE(sourceChar)) {
                إذا(U16_IS_SURROGATE_LEAD(sourceChar)) {
getTrail:
                    /*look ahead to find the trail surrogate*/
                    إذا(source < sourceLimit) {
                        /* test the following code unit */
                        char16_t trail = *source;
                        إذا(U16_IS_TRAIL(trail)) {
                            source++;
                            sourceChar=U16_GET_SUPPLEMENTARY(sourceChar, trail);
                            cnv->fromUChar32=0x00;
                            /* convert this supplementary code point */
                            /* exit this condition tree */
                        } وإلا {
                            /* this is an unmatched lead code unit (1st surrogate) */
                            /* callback(illegal) */
                            *err=U_ILLEGAL_CHAR_FOUND;
                            cnv->fromUChar32=sourceChar;
                            اكسر;
                        }
                    } وإلا {
                        /* no more input */
                        cnv->fromUChar32=sourceChar;
                        اكسر;
                    }
                } وإلا {
                    /* this is an unmatched trail code unit (2nd surrogate) */
                    /* callback(illegal) */
                    *err=U_ILLEGAL_CHAR_FOUND;
                    cnv->fromUChar32=sourceChar;
                    اكسر;
                }
            }

            /* do the conversion */
            إذا(sourceChar <= 0x007f ){
                /* do not convert SO/SI/ESC */
                إذا(IS_2022_CONTROL(sourceChar)) {
                    /* callback(illegal) */
                    *err=U_ILLEGAL_CHAR_FOUND;
                    cnv->fromUChar32=sourceChar;
                    اكسر;
                }

                /* US-ASCII */
                إذا(pFromU2022State->g == 0) {
                    buffer[0] = static_cast<حرف>(sourceChar);
                    len = 1;
                } وإلا {
                    buffer[0] = UCNV_SI;
                    buffer[1] = static_cast<حرف>(sourceChar);
                    len = 2;
                    pFromU2022State->g = 0;
                    choice_تعداد = 0;
                }
                إذا(sourceChar == CR || sourceChar == LF) {
                    /* reset the state at the end of a line */
                    uprv_memset(pFromU2022State, 0, حجم(ISO2022State));
                    choice_تعداد = 0;
                }
            }
            وإلا{
                /* convert U+0080..U+10ffff */
                int32_t i;
                int8_t cs, g;

                إذا(choice_تعداد == 0) {
                    /* try the current SO/G1 converter first */
                    choices[0] = pFromU2022State->cs[1];

                    /* default to GB2312_1 if none is designated yet */
                    إذا(choices[0] == 0) {
                        choices[0] = GB2312_1;
                    }

                    إذا(converterData->version == 0) {
                        /* ISO-2022-CN */

                        /* try the other SO/G1 converter; a CNS_11643_1 lookup may result in any plane */
                        إذا(choices[0] == GB2312_1) {
                            choices[1] = static_cast<int8_t>(CNS_11643_1);
                        } وإلا {
                            choices[1] = static_cast<int8_t>(GB2312_1);
                        }

                        choice_تعداد = 2;
                    } وإلا إذا (converterData->version == 1) {
                        /* ISO-2022-CN-EXT */

                        /* try one of the other converters */
                        بدل(choices[0]) {
                        حالة GB2312_1:
                            choices[1] = static_cast<int8_t>(CNS_11643_1);
                            choices[2] = static_cast<int8_t>(ISO_IR_165);
                            اكسر;
                        حالة ISO_IR_165:
                            choices[1] = static_cast<int8_t>(GB2312_1);
                            choices[2] = static_cast<int8_t>(CNS_11643_1);
                            اكسر;
                        افتراضي: /* CNS_11643_x */
                            choices[1] = static_cast<int8_t>(GB2312_1);
                            choices[2] = static_cast<int8_t>(ISO_IR_165);
                            اكسر;
                        }

                        choice_تعداد = 3;
                    } وإلا {
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

                لكل(i = 0; i < choice_تعداد && len <= 0; ++i) {
                    int8_t cs0 = choices[i];
                    إذا(cs0 > 0) {
                        uint32_t value;
                        int32_t len2;
                        إذا(cs0 >= CNS_11643_0) {
                            len2 = MBCS_FROM_UCHAR32_ISO2022(
                                        converterData->myConverterArray[CNS_11643],
                                        sourceChar,
                                        &value,
                                        useFallback,
                                        MBCS_OUTPUT_3);
                            إذا(len2 == 3 || (len2 == -3 && len == 0)) {
                                target_قيمة = value;
                                cs = static_cast<int8_t>(CNS_11643_0 + (value >> 16) - 0x80);
                                إذا(len2 >= 0) {
                                    len = 2;
                                } وإلا {
                                    len = -2;
                                    useFallback = خطأ;
                                }
                                إذا(cs == CNS_11643_1) {
                                    g = 1;
                                } وإلا إذا(cs == CNS_11643_2) {
                                    g = 2;
                                } وإلا /* plane 3..7 */ إذا(converterData->version == 1) {
                                    g = 3;
                                } وإلا {
                                    /* ISO-2022-CN (without -EXT) does not support plane 3..7 */
                                    len = 0;
                                }
                            }
                        } وإلا {
                            /* GB2312_1 or ISO-IR-165 */
                            U_ASSERT(cs0<UCNV_2022_MAX_CONVERTERS);
                            len2 = MBCS_FROM_UCHAR32_ISO2022(
                                        converterData->myConverterArray[cs0],
                                        sourceChar,
                                        &value,
                                        useFallback,
                                        MBCS_OUTPUT_2);
                            إذا(len2 == 2 || (len2 == -2 && len == 0)) {
                                target_قيمة = value;
                                len = len2;
                                cs = cs0;
                                g = 1;
                                useFallback = خطأ;
                            }
                        }
                    }
                }

                إذا(len != 0) {
                    len = 0; /* count output bytes; it must have been abs(len) == 2 */

                    /* write the designation sequence if necessary */
                    إذا(cs != pFromU2022State->cs[g]) {
                        إذا(cs < CNS_11643) {
                            uprv_memcpy(buffer, escSeqCharsCN[cs], 4);
                        } وإلا {
                            U_ASSERT(cs >= CNS_11643_1);
                            uprv_memcpy(buffer, escSeqCharsCN[CNS_11643 + (cs - CNS_11643_1)], 4);
                        }
                        len = 4;
                        pFromU2022State->cs[g] = cs;
                        إذا(g == 1) {
                            /* changing the SO/G1 charset invalidates the choices[] */
                            choice_تعداد = 0;
                        }
                    }

                    /* write the shift sequence if necessary */
                    إذا(g != pFromU2022State->g) {
                        بدل(g) {
                        حالة 1:
                            buffer[len++] = UCNV_SO;

                            /* set the new state only if it is the locking shift SO/G1, not for SS2 or SS3 */
                            pFromU2022State->g = 1;
                            اكسر;
                        حالة 2:
                            buffer[len++] = 0x1b;
                            buffer[len++] = 0x4e;
                            اكسر;
                        افتراضي: /* case 3 */
                            buffer[len++] = 0x1b;
                            buffer[len++] = 0x4f;
                            اكسر;
                        }
                    }

                    /* write the two output bytes */
                    buffer[len++] = static_cast<حرف>(target_قيمة >> 8);
                    buffer[len++] = static_cast<حرف>(target_قيمة);
                } وإلا {
                    /* if we cannot find the character after checking all codepages
                     * then this is an error
                     */
                    *err = U_INVALID_CHAR_FOUND;
                    cnv->fromUChar32=sourceChar;
                    اكسر;
                }
            }

            /* output len>0 bytes in buffer[] */
            إذا(len == 1) {
                *target++ = buffer[0];
                إذا(offsets) {
                    *offsets++ = static_cast<int32_t>(source - args->source - 1); /* -1: known to be ASCII */
                }
            } وإلا إذا(len == 2 && (target + 2) <= targetLimit) {
                *target++ = buffer[0];
                *target++ = buffer[1];
                إذا(offsets) {
                    int32_t sourceIndex = static_cast<int32_t>(source - args->source - U16_LENGTH(sourceChar));
                    *offsets++ = sourceIndex;
                    *offsets++ = sourceIndex;
                }
            } وإلا {
                fromUWriteUInt8(
                    cnv,
                    buffer, len,
                    &target, reinterpret_cast<ثابت حرف*>(targetLimit),
                    &offsets, static_cast<int32_t>(source - args->source - U16_LENGTH(sourceChar)),
                    err);
                إذا(U_FAILURE(*err)) {
                    اكسر;
                }
            }
        } /* end if(myTargetIndex<myTargetLength) */
        وإلا{
            *err =U_BUFFER_OVERFLOW_ERROR;
            اكسر;
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
    إذا( U_SUCCESS(*err) &&
        pFromU2022State->g!=0 &&
        args->flush && source>=sourceLimit && cnv->fromUChar32==0
    ) {
        int32_t sourceIndex;

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
        sourceIndex = static_cast<int32_t>(source - args->source);
        إذا(sourceIndex>0) {
            --sourceIndex;
            إذا( U16_IS_TRAIL(args->source[sourceIndex]) &&
                (sourceIndex==0 || U16_IS_LEAD(args->source[sourceIndex-1]))
            ) {
                --sourceIndex;
            }
        } وإلا {
            sourceIndex=-1;
        }

        fromUWriteUInt8(
            cnv,
            SHIFT_IN_STR, 1,
            &target, reinterpret_cast<ثابت حرف*>(targetLimit),
            &offsets, sourceIndex,
            err);
    }

    /*save the state and return */
    args->source = source;
    args->target = reinterpret_cast<حرف*>(target);
}


ساكن فراغ U_CALLCONV
UConverter_toUnicode_ISO_2022_CN_OFFSETS_LOGIC(UConverterToUnicodeArgs *args,
                                               UErrorCode* err){
    حرف tempBuf[3];
    ثابت حرف* mySource = const_cast<حرف*>(args->source);
    char16_t *myTarget = args->target;
    ثابت حرف *mySourceLimit = args->sourceLimit;
    uint32_t targetUniChar = 0x0000;
    uint32_t mySourceChar = 0x0000;
    UConverterDataISO2022* myData;
    ISO2022State *pToU2022State;

    myData = static_cast<UConverterDataISO2022*>(args->converter->extraInfo);
    pToU2022State = &myData->toU2022State;

    إذا(myData->مفتاح != 0) {
        /* continue with a partial escape sequence */
        اذهب escape;
    } وإلا إذا(args->converter->toULength == 1 && mySource < mySourceLimit && myTarget < args->targetLimit) {
        /* continue with a partial double-byte character */
        mySourceChar = args->converter->toUBytes[0];
        args->converter->toULength = 0;
        targetUniChar = missingCharMarker;
        اذهب getTrailByte;
    }

    بينما(mySource < mySourceLimit){

        targetUniChar =missingCharMarker;

        إذا(myTarget < args->targetLimit){

            mySourceChar = static_cast<unsigned حرف>(*mySource++);

            بدل(mySourceChar){
            حالة UCNV_SI:
                pToU2022State->g=0;
                إذا (myData->isEmptySegment) {
                    myData->isEmptySegment = خطأ;	/* we are handling it, reset to avoid future spurious errors */
                    *err = U_ILLEGAL_ESCAPE_SEQUENCE;
                    args->converter->toUCallbackReason = UCNV_IRREGULAR;
                    args->converter->toUBytes[0] = static_cast<uint8_t>(mySourceChar);
                    args->converter->toULength = 1;
                    args->target = myTarget;
                    args->source = mySource;
                    إرجاع;
                }
                استمر;

            حالة UCNV_SO:
                إذا(pToU2022State->cs[1] != 0) {
                    pToU2022State->g=1;
                    myData->isEmptySegment = true;	/* Begin a new segment, empty so far */
                    استمر;
                } وإلا {
                    /* illegal to have SO before a matching designator */
                    myData->isEmptySegment = خطأ;	/* Handling a different error, reset this to avoid future spurious errs */
                    اكسر;
                }

            حالة ESC_2022:
                mySource--;
escape:
                {
                    ثابت حرف * mySourceBefore = mySource;
                    int8_t toULengthBefore = args->converter->toULength;

                    changeState_2022(args->converter,&(mySource),
                        mySourceLimit, ISO_2022_CN,err);

                    /* After SO there must be at least one character before a designator (designator error handled separately) */
                    إذا(myData->مفتاح==0 && U_SUCCESS(*err) && myData->isEmptySegment) {
                        *err = U_ILLEGAL_ESCAPE_SEQUENCE;
                        args->converter->toUCallbackReason = UCNV_IRREGULAR;
                        args->converter->toULength = static_cast<int8_t>(toULengthBefore + (mySource - mySourceBefore));
                    }
                }

                /* invalid or illegal escape sequence */
                إذا(U_FAILURE(*err)){
                    args->target = myTarget;
                    args->source = mySource;
                    myData->isEmptySegment = خطأ;	/* Reset to avoid future spurious errors */
                    إرجاع;
                }
                استمر;

            /* ISO-2022-CN does not use single-byte (C1) SS2 and SS3 */

            حالة CR:
            حالة LF:
                uprv_memset(pToU2022State, 0, حجم(ISO2022State));
                U_FALLTHROUGH;
            افتراضي:
                /* convert one or two bytes */
                myData->isEmptySegment = خطأ;
                إذا(pToU2022State->g != 0) {
                    إذا(mySource < mySourceLimit) {
                        UConverterSharedData *cnv;
                        StateEnum tempState;
                        int32_t tempBufLen;
                        صحيح leadIsOk, trailIsOk;
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
                        إذا (leadIsOk && trailIsOk) {
                            ++mySource;
                            tempState = static_cast<StateEnum>(pToU2022State->cs[pToU2022State->g]);
                            إذا(tempState >= CNS_11643_0) {
                                cnv = myData->myConverterArray[CNS_11643];
                                tempBuf[0] = static_cast<حرف>(0x80 + (tempState - CNS_11643_0));
                                tempBuf[1] = static_cast<حرف>(mySourceChar);
                                tempBuf[2] = static_cast<حرف>(trailByte);
                                tempBufLen = 3;

                            }وإلا{
                                U_ASSERT(tempState<UCNV_2022_MAX_CONVERTERS);
                                cnv = myData->myConverterArray[tempState];
                                tempBuf[0] = static_cast<حرف>(mySourceChar);
                                tempBuf[1] = static_cast<حرف>(trailByte);
                                tempBufLen = 2;
                            }
                            targetUniChar = ucnv_MBCSSimpleGetNextUChar(cnv, tempBuf, tempBufLen, خطأ);
                            mySourceChar = (mySourceChar << 8) | trailByte;
                        } وإلا إذا (!(trailIsOk || IS_2022_CONTROL(trailByte))) {
                            /* report a pair of illegal bytes if the second byte is not a DBCS starter */
                            ++mySource;
                            /* add another bit so that the code below writes 2 bytes in case of error */
                            mySourceChar = 0x10000 | (mySourceChar << 8) | trailByte;
                        }
                        إذا(pToU2022State->g>=2) {
                            /* return from a single-shift state to the previous one */
                            pToU2022State->g=pToU2022State->prevG;
                        }
                    } وإلا {
                        args->converter->toUBytes[0] = static_cast<uint8_t>(mySourceChar);
                        args->converter->toULength = 1;
                        اذهب endloop;
                    }
                }
                وإلا{
                    إذا(mySourceChar <= 0x7f) {
                        targetUniChar = static_cast<char16_t>(mySourceChar);
                    }
                }
                اكسر;
            }
            إذا(targetUniChar < (missingCharMarker-1/*0xfffe*/)){
                إذا(args->offsets){
                    args->offsets[myTarget - args->target] = static_cast<int32_t>(mySource - args->source - (mySourceChar <= 0xff ? 1 : 2));
                }
                *(myTarget++) = static_cast<char16_t>(targetUniChar);
            }
            وإلا إذا(targetUniChar > missingCharMarker){
                /* disassemble the surrogate pair and write to output*/
                targetUniChar-=0x0010000;
                *myTarget = static_cast<char16_t>(0xd800 + static_cast<char16_t>(targetUniChar >> 10));
                إذا(args->offsets){
                    args->offsets[myTarget - args->target] = static_cast<int32_t>(mySource - args->source - (mySourceChar <= 0xff ? 1 : 2));
                }
                ++myTarget;
                إذا(myTarget< args->targetLimit){
                    *myTarget = static_cast<char16_t>(0xdc00 + static_cast<char16_t>(targetUniChar & 0x3ff));
                    إذا(args->offsets){
                        args->offsets[myTarget - args->target] = static_cast<int32_t>(mySource - args->source - (mySourceChar <= 0xff ? 1 : 2));
                    }
                    ++myTarget;
                }وإلا{
                    args->converter->UCharErrorBuffer[args->converter->UCharErrorBufferLength++]=
                                    static_cast<char16_t>(0xdc00 + static_cast<char16_t>(targetUniChar & 0x3ff));
                }

            }
            وإلا{
                /* Call the callback function*/
                toUnicodeCallback(args->converter,mySourceChar,targetUniChar,err);
                اكسر;
            }
        }
        وإلا{
            *err =U_BUFFER_OVERFLOW_ERROR;
            اكسر;
        }
    }
endloop:
    args->target = myTarget;
    args->source = mySource;
}
#endif /* #if !UCONFIG_ONLY_HTML_CONVERSION */

ساكن فراغ U_CALLCONV
_ISO_2022_WriteSub(UConverterFromUnicodeArgs *args, int32_t offsetIndex, UErrorCode *err) {
    UConverter *cnv = args->converter;
    UConverterDataISO2022* myConverterData = static_cast<UConverterDataISO2022*>(cnv->extraInfo);
    ISO2022State *pFromU2022State=&myConverterData->fromU2022State;
    حرف *p, *subchar;
    حرف buffer[8];
    int32_t length;

    subchar = reinterpret_cast<حرف*>(cnv->subChars);
    length=cnv->subCharLen; /* assume length==1 for most variants */

    p = buffer;
    بدل(myConverterData->locale[0]){
    حالة 'j':
        {
            int8_t cs;

            إذا(pFromU2022State->g == 1) {
                /* JIS7: switch from G1 to G0 */
                pFromU2022State->g = 0;
                *p++ = UCNV_SI;
            }

            cs = pFromU2022State->cs[0];
            إذا(cs != ASCII && cs != JISX201) {
                /* not in ASCII or JIS X 0201: switch to ASCII */
                pFromU2022State->cs[0] = static_cast<int8_t>(ASCII);
                *p++ = '\x1b';
                *p++ = '\x28';
                *p++ = '\x42';
            }

            *p++ = subchar[0];
            اكسر;
        }
    حالة 'c':
        إذا(pFromU2022State->g != 0) {
            /* not in ASCII mode: switch to ASCII */
            pFromU2022State->g = 0;
            *p++ = UCNV_SI;
        }
        *p++ = subchar[0];
        اكسر;
    حالة 'k':
        إذا(myConverterData->version == 0) {
            إذا(length == 1) {
                إذا(args->converter->fromUnicodeStatus) {
                    /* in DBCS mode: switch to SBCS */
                    args->converter->fromUnicodeStatus = 0;
                    *p++ = UCNV_SI;
                }
                *p++ = subchar[0];
            } وإلا /* length == 2*/ {
                إذا(!args->converter->fromUnicodeStatus) {
                    /* in SBCS mode: switch to DBCS */
                    args->converter->fromUnicodeStatus = 1;
                    *p++ = UCNV_SO;
                }
                *p++ = subchar[0];
                *p++ = subchar[1];
            }
            اكسر;
        } وإلا {
            /* save the subconverter's substitution string */
            uint8_t *currentSubChars = myConverterData->currentConverter->subChars;
            int8_t currentSubCharLen = myConverterData->currentConverter->subCharLen;

            /* set our substitution string into the subconverter */
            myConverterData->currentConverter->subChars = reinterpret_cast<uint8_t*>(subchar);
            myConverterData->currentConverter->subCharLen = static_cast<int8_t>(length);

            /* let the subconverter write the subchar, set/retrieve fromUChar32 state */
            args->converter = myConverterData->currentConverter;
            myConverterData->currentConverter->fromUChar32 = cnv->fromUChar32;
            ucnv_cbFromUWriteSub(args, 0, err);
            cnv->fromUChar32 = myConverterData->currentConverter->fromUChar32;
            args->converter = cnv;

            /* restore the subconverter's substitution string */
            myConverterData->currentConverter->subChars = currentSubChars;
            myConverterData->currentConverter->subCharLen = currentSubCharLen;

            إذا(*err == U_BUFFER_OVERFLOW_ERROR) {
                إذا(myConverterData->currentConverter->charErrorBufferLength > 0) {
                    uprv_memcpy(
                        cnv->charErrorBuffer,
                        myConverterData->currentConverter->charErrorBuffer,
                        myConverterData->currentConverter->charErrorBufferLength);
                }
                cnv->charErrorBufferLength = myConverterData->currentConverter->charErrorBufferLength;
                myConverterData->currentConverter->charErrorBufferLength = 0;
            }
            إرجاع;
        }
    افتراضي:
        /* not expected */
        اكسر;
    }
    ucnv_cbFromUWriteBytes(args,
                           buffer, static_cast<int32_t>(p - buffer),
                           offsetIndex, err);
}

/*
 * Structure for cloning an ISO 2022 converter into a single memory block.
 */
بنية cloneStruct
{
    UConverter cnv;
    UConverter currentConverter;
    UConverterDataISO2022 mydata;
};


U_CDECL_BEGIN

ساكن UConverter * U_CALLCONV
_ISO_2022_SafeClone(
            ثابت UConverter *cnv,
            فراغ *مكدس_Buffer,
            int32_t *pBufferSize,
            UErrorCode *status)
{
    بنية cloneStruct * localClone;
    UConverterDataISO2022 *cnvData;
    int32_t i, size;

    إذا (U_FAILURE(*status)){
        إرجاع nullptr;
    }

    إذا (*pBufferSize == 0) { /* 'preflighting' request - set needed size into *pBufferSize */
        *pBufferSize = (int32_t)حجم(بنية cloneStruct);
        إرجاع nullptr;
    }

    cnvData = (UConverterDataISO2022 *)cnv->extraInfo;
    localClone = (بنية cloneStruct *)مكدس_Buffer;

    /* ucnv.c/ucnv_safeClone() copied the main UConverter already */

    uprv_memcpy(&localClone->mydata, cnvData, حجم(UConverterDataISO2022));
    localClone->cnv.extraInfo = &localClone->mydata; /* set pointer to extra data */
    localClone->cnv.isExtraLocal = true;

    /* share the subconverters */

    إذا(cnvData->currentConverter != nullptr) {
        size = (int32_t)حجم(UConverter);
        localClone->mydata.currentConverter =
            ucnv_safeClone(cnvData->currentConverter,
                            &localClone->currentConverter,
                            &size, status);
        إذا(U_FAILURE(*status)) {
            إرجاع nullptr;
        }
    }

    لكل(i=0; i<UCNV_2022_MAX_CONVERTERS; ++i) {
        إذا(cnvData->myConverterArray[i] != nullptr) {
            ucnv_increment_Ref_تعداد(cnvData->myConverterArray[i]);
        }
    }

    إرجاع &localClone->cnv;
}

U_CDECL_END

ساكن فراغ U_CALLCONV
_ISO_2022_GetUnicodeSet(ثابت UConverter *cnv,
                    ثابت USetAdder *sa,
                    UConverterUnicodeSet which,
                    UErrorCode *pErrorCode)
{
    int32_t i;
    UConverterDataISO2022* cnvData;

    إذا (U_FAILURE(*pErrorCode)) {
        إرجاع;
    }
#ifdef U_ENABLE_GENERIC_ISO_2022
    إذا (cnv->sharedData == &_ISO2022Data) {
        /* We use UTF-8 in this case */
        sa->addRange(sa->set, 0, 0xd7FF);
        sa->addRange(sa->set, 0xE000, 0x10FFFF);
        إرجاع;
    }
#endif

    cnvData = static_cast<UConverterDataISO2022*>(cnv->extraInfo);

    /* open a set and initialize it with code points that are algorithmically round-tripped */
    بدل(cnvData->locale[0]){
    حالة 'j':
        /* include JIS X 0201 which is hardcoded */
        sa->add(sa->set, 0xa5);
        sa->add(sa->set, 0x203e);
        إذا(jpCharsetMasks[cnvData->version]&CSM(ISO8859_1)) {
            /* include Latin-1 for some variants of JP */
            sa->addRange(sa->set, 0, 0xff);
        } وإلا {
            /* include ASCII for JP */
            sa->addRange(sa->set, 0, 0x7f);
        }
        إذا(cnvData->version==3 || cnvData->version==4 || which==UCNV_ROUNDTRIP_AND_FALLBACK_SET) {
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
        اكسر;
#إذا !UCONFIG_ONLY_HTML_CONVERSION
    حالة 'c':
    حالة 'z':
        /* include ASCII for CN */
        sa->addRange(sa->set, 0, 0x7f);
        اكسر;
    حالة 'k':
        /* there is only one converter for KR, and it is not in the myConverterArray[] */
        cnvData->currentConverter->sharedData->impl->getUnicodeSet(
                cnvData->currentConverter, sa, which, pErrorCode);
        /* the loop over myConverterArray[] will simply not find another converter */
        اكسر;
#endif
    افتراضي:
        اكسر;
    }

#إذا 0  /* Replaced by ucnv_MBCSGetFilteredUnicodeSetForUnicode() until we implement ucnv_getUnicodeSet() with reverse fallbacks. */
            إذا( (cnvData->locale[0]=='c' || cnvData->locale[0]=='z') &&
                cnvData->version==0 && i==CNS_11643
            ) {
                /* special handling for non-EXT ISO-2022-CN: add only code points for CNS planes 1 and 2 */
                ucnv_MBCSGetUnicodeSetForBytes(
                        cnvData->myConverterArray[i],
                        sa, UCNV_ROUNDTRIP_SET,
                        0, 0x81, 0x82,
                        pErrorCode);
            }
#endif

    لكل (i=0; i<UCNV_2022_MAX_CONVERTERS; i++) {
        UConverterSetFilter filter;
        إذا(cnvData->myConverterArray[i]!=nullptr) {
            إذا(cnvData->locale[0]=='j' && i==JISX208) {
                /*
                 * Only add code points that map to Shift-JIS codes
                 * corresponding to JIS X 0208.
                 */
                filter=UCNV_SET_FILTER_SJIS;
#إذا !UCONFIG_ONLY_HTML_CONVERSION
            } وإلا إذا( (cnvData->locale[0]=='c' || cnvData->locale[0]=='z') &&
                       cnvData->version==0 && i==CNS_11643) {
                /*
                 * Version-specific for CN:
                 * CN version 0 does not map CNS planes 3..7 although
                 * they are all available in the CNS conversion table;
                 * CN version 1 (-EXT) does map them all.
                 * The two versions create different Unicode sets.
                 */
                filter=UCNV_SET_FILTER_2022_CN;
            } وإلا إذا(i==KSC5601) {
                /*
                 * Some of the KSC 5601 tables (convrtrs.txt has this aliases on multiple tables)
                 * are broader than GR94.
                 */
                filter=UCNV_SET_FILTER_GR94DBCS;
#endif
            } وإلا {
                filter=UCNV_SET_FILTER_NONE;
            }
            ucnv_MBCSGetFilteredUnicodeSetForUnicode(cnvData->myConverterArray[i], sa, which, filter, pErrorCode);
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

ساكن ثابت UConverterImpl _ISO2022Impl={
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
#وإلا
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
ساكن ثابت UConverterStaticData _ISO2022StaticData={
    حجم(UConverterStaticData),
    "ISO_2022",
    2022,
    UCNV_IBM,
    UCNV_ISO_2022,
    1,
    3, /* max 3 bytes per char16_t from UTF-8 (4 bytes from surrogate _pair_) */
    { 0x1a, 0, 0, 0 },
    1,
    خطأ,
    خطأ,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};
ثابت UConverterSharedData _ISO2022Data=
        UCNV_IMMUTABLE_SHARED_DATA_INITIALIZER(&_ISO2022StaticData, &_ISO2022Impl);

/*************JP****************/
ساكن ثابت UConverterImpl _ISO2022JPImpl={
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
ساكن ثابت UConverterStaticData _ISO2022JPStaticData={
    حجم(UConverterStaticData),
    "ISO_2022_JP",
    0,
    UCNV_IBM,
    UCNV_ISO_2022,
    1,
    6, /* max 6 bytes per char16_t: 4-byte escape sequence + DBCS */
    { 0x1a, 0, 0, 0 },
    1,
    خطأ,
    خطأ,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

نطاق {

ثابت UConverterSharedData _ISO2022JPData=
        UCNV_IMMUTABLE_SHARED_DATA_INITIALIZER(&_ISO2022JPStaticData, &_ISO2022JPImpl);

}  // namespace

#إذا !UCONFIG_ONLY_HTML_CONVERSION
/************* KR ***************/
ساكن ثابت UConverterImpl _ISO2022KRImpl={
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
ساكن ثابت UConverterStaticData _ISO2022KRStaticData={
    حجم(UConverterStaticData),
    "ISO_2022_KR",
    0,
    UCNV_IBM,
    UCNV_ISO_2022,
    1,
    8, /* max 8 bytes per char16_t */
    { 0x1a, 0, 0, 0 },
    1,
    خطأ,
    خطأ,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

نطاق {

ثابت UConverterSharedData _ISO2022KRData=
        UCNV_IMMUTABLE_SHARED_DATA_INITIALIZER(&_ISO2022KRStaticData, &_ISO2022KRImpl);

}  // namespace

/*************** CN ***************/
ساكن ثابت UConverterImpl _ISO2022CNImpl={

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
ساكن ثابت UConverterStaticData _ISO2022CNStaticData={
    حجم(UConverterStaticData),
    "ISO_2022_CN",
    0,
    UCNV_IBM,
    UCNV_ISO_2022,
    1,
    8, /* max 8 bytes per char16_t: 4-byte CNS designator + 2 bytes for SS2/SS3 + DBCS */
    { 0x1a, 0, 0, 0 },
    1,
    خطأ,
    خطأ,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

نطاق {

ثابت UConverterSharedData _ISO2022CNData=
        UCNV_IMMUTABLE_SHARED_DATA_INITIALIZER(&_ISO2022CNStaticData, &_ISO2022CNImpl);

}  // namespace
#endif /* #if !UCONFIG_ONLY_HTML_CONVERSION */

#endif /* #if !UCONFIG_NO_LEGACY_CONVERSION */
