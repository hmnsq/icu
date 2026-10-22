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

#समावेश "unicode/utypes.h"

#अगर !UCONFIG_NO_CONVERSION && !UCONFIG_NO_LEGACY_CONVERSION

#समावेश "unicode/ucnv.h"
#समावेश "unicode/uset.h"
#समावेश "unicode/ucnv_err.h"
#समावेश "unicode/ucnv_cb.h"
#समावेश "unicode/utf16.h"
#समावेश "ucnv_imp.h"
#समावेश "ucnv_bld.h"
#समावेश "ucnv_cnv.h"
#समावेश "ucnvmbcs.h"
#समावेश "cstring.h"
#समावेश "cmemory.h"
#समावेश "uassert.h"

#अगर_घोषित U_ENABLE_GENERIC_ISO_2022
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
#पूर्ण_अगर

#अगर !UCONFIG_ONLY_HTML_CONVERSION
अटल स्थिर अक्षर SHIFT_IN_STR[]  = "\x0F";
// static const char SHIFT_OUT_STR[] = "\x0E";
#पूर्ण_अगर

#घोषणा CR      0x0D
#घोषणा LF      0x0A
#घोषणा H_TAB   0x09
#घोषणा V_TAB   0x0B
#घोषणा SPACE   0x20

क्रमागत {
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
क्रमागत {
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
#घोषणा IS_2022_CONTROL(c) (((c)<0x20) && (((uint32_t)1<<(c))&0x0800c000)!=0)

/* for ISO-2022-JP and -CN implementations */
प्रकार क्रमागत  {
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
#अगर UCONFIG_ONLY_HTML_CONVERSION
#घोषणा IS_JP_DBCS(cs) (JISX208==(cs))
#अन्यथा
#घोषणा IS_JP_DBCS(cs) (JISX208<=(cs) && (cs)<=KSC5601)
#पूर्ण_अगर

#घोषणा CSM(cs) ((uint16_t)1<<(cs))

/*
 * Each of these charset masks (with index x) contains a bit for a charset in exact correspondence
 * to whether that charset is used in the corresponding version x of ISO_2022,locale=ja,version=x
 *
 * Note: The converter uses some leniency:
 * - The escape sequence ESC ( I for half-width 7-bit Katakana is recognized in
 *   all versions, not just JIS7 and JIS8.
 * - ICU does not distinguish between different versions of JIS X 0208.
 */
#अगर UCONFIG_ONLY_HTML_CONVERSION
क्रमागत { MAX_JA_VERSION=0 };
#अन्यथा
क्रमागत { MAX_JA_VERSION=4 };
#पूर्ण_अगर
अटल स्थिर uint16_t jpCharsetMasks[MAX_JA_VERSION+1]={
    CSM(ASCII)|CSM(JISX201)|CSM(JISX208)|CSM(HWKANA_7BIT),
#अगर !UCONFIG_ONLY_HTML_CONVERSION
    CSM(ASCII)|CSM(JISX201)|CSM(JISX208)|CSM(HWKANA_7BIT)|CSM(JISX212),
    CSM(ASCII)|CSM(JISX201)|CSM(JISX208)|CSM(HWKANA_7BIT)|CSM(JISX212)|CSM(GB2312)|CSM(KSC5601)|CSM(ISO8859_1)|CSM(ISO8859_7),
    CSM(ASCII)|CSM(JISX201)|CSM(JISX208)|CSM(HWKANA_7BIT)|CSM(JISX212)|CSM(GB2312)|CSM(KSC5601)|CSM(ISO8859_1)|CSM(ISO8859_7),
    CSM(ASCII)|CSM(JISX201)|CSM(JISX208)|CSM(HWKANA_7BIT)|CSM(JISX212)|CSM(GB2312)|CSM(KSC5601)|CSM(ISO8859_1)|CSM(ISO8859_7)
#पूर्ण_अगर
};

प्रकार क्रमागत {
        ASCII1=0,
        LATIN1,
        SBCS,
        DBCS,
        MBCS,
        HWKANA
}Cnv2022Type;

प्रकार काष्ठा ISO2022State {
    int8_t cs[4];       /* charset number for SI (G0)/SO (G1)/SS2 (G2)/SS3 (G3) */
    int8_t g;           /* 0..3 for G0..G3 (SI/SO/SS2/SS3) */
    int8_t पूर्व_G;       /* g before single shift (SS2 or SS3) */
} ISO2022State;

#घोषणा UCNV_OPTIONS_VERSION_MASK 0xf
#घोषणा UCNV_2022_MAX_CONVERTERS 10

प्रकार काष्ठा{
    UConverter_Shared_तथ्य *myConverterArray[UCNV_2022_MAX_CONVERTERS];
    UConverter *currentConverter;
    Cnv2022Type currentType;
    ISO2022State toU2022State, fromU2022State;
    uint32_t कुंजी;
    uint32_t version;
#अगर_घोषित U_ENABLE_GENERIC_ISO_2022
    UBool is_First_बफर;
#पूर्ण_अगर
    UBool isEmptySegment;
    अक्षर name[30];
    अक्षर locale[3];
}UConverter_तथ्य_ISO2022;

/* Protos */
/* ISO-2022 ----------------------------------------------------------------- */

/*Forward declaration */
U_CFUNC व्योम U_CALLCONV
ucnv_fromUnicode_UTF8(UConverterFromUnicodeArgs * args,
                      UErrorCode * err);
U_CFUNC व्योम U_CALLCONV
ucnv_fromUnicode_UTF8_OFFSETS_LOGIC(UConverterFromUnicodeArgs * args,
                                    UErrorCode * err);

#घोषणा ESC_2022 0x1B /*ESC*/

प्रकार क्रमागत
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
अटल स्थिर int8_t normalize_esq_chars_2022[256] = {
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

#अगर_घोषित U_ENABLE_GENERIC_ISO_2022
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
#पूर्ण_अगर

#घोषणा MAX_STATES_2022 74
अटल स्थिर int32_t esc_Seq_State_Table_कुंजी_2022[MAX_STATES_2022] = {
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

#अगर_घोषित U_ENABLE_GENERIC_ISO_2022

अटल स्थिर अक्षर* स्थिर esc_Seq_State_Table_परिणाम_2022[MAX_STATES_2022] = {
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

#पूर्ण_अगर

अटल स्थिर int8_t esc_Seq_State_Table_मूल्य_2022[MAX_STATES_2022] = {
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
प्रकार क्रमागत{
#अगर_घोषित U_ENABLE_GENERIC_ISO_2022
    ISO_2022=0,
#पूर्ण_अगर
    ISO_2022_JP=1,
#अगर !UCONFIG_ONLY_HTML_CONVERSION
    ISO_2022_KR=2,
    ISO_2022_CN=3
#पूर्ण_अगर
} Variant2022;

/*********** ISO 2022 Converter Protos ***********/
अटल व्योम U_CALLCONV
_ISO2022Open(UConverter *cnv, UConverterLoadArgs *pArgs, UErrorCode *errorCode);

अटल व्योम U_CALLCONV
 _ISO2022Close(UConverter *converter);

अटल व्योम U_CALLCONV
_ISO2022Reset(UConverter *converter, UConverterResetChoice choice);

U_CDECL_BEGIN
अटल स्थिर अक्षर * U_CALLCONV
_ISO2022getName(स्थिर UConverter* cnv);
U_CDECL_END

अटल व्योम  U_CALLCONV
_ISO_2022_WriteSub(UConverterFromUnicodeArgs *args, int32_t offset_सूचकांक, UErrorCode *err);

U_CDECL_BEGIN
अटल UConverter * U_CALLCONV
_ISO_2022_SafeClone(स्थिर UConverter *cnv, व्योम *ढेर_बफर, int32_t *p_बफर_आकार, UErrorCode *status);

U_CDECL_END

#अगर_घोषित U_ENABLE_GENERIC_ISO_2022
अटल व्योम U_CALLCONV
T_UConverter_toUnicode_ISO_2022_OFFSETS_LOGIC(UConverterToUnicodeArgs* args, UErrorCode* err);
#पूर्ण_अगर

namespace {

/*const UConverterSharedData _ISO2022Data;*/
बाह्य स्थिर UConverter_Shared_तथ्य _ISO2022JPData;

#अगर !UCONFIG_ONLY_HTML_CONVERSION
बाह्य स्थिर UConverter_Shared_तथ्य _ISO2022KRData;
बाह्य स्थिर UConverter_Shared_तथ्य _ISO2022CNData;
#पूर्ण_अगर

}  // namespace

/*************** Converter implementations ******************/

/* The purpose of this function is to get around gcc compiler warnings. */
अटल अंतरभूत व्योम
fromUWriteUInt8(UConverter *cnv,
                 स्थिर अक्षर *bytes, int32_t length,
                 uint8_t **target, स्थिर अक्षर *targetLimit,
                 int32_t **offsets,
                 int32_t source_सूचकांक,
                 UErrorCode *pErrorCode)
{
    अक्षर* targetChars = reinterpret_cast<अक्षर*>(*target);
    ucnv_fromUWriteBytes(cnv, bytes, length, &targetChars, targetLimit,
                         offsets, source_सूचकांक, pErrorCode);
    *target = reinterpret_cast<uint8_t*>(targetChars);

}

अटल अंतरभूत व्योम
setInitialStateToUnicodeKR(UConverter* /*converter*/, UConverter_तथ्य_ISO2022 *my_Converter_तथ्य){
    अगर(my_Converter_तथ्य->version == 1) {
        UConverter *cnv = my_Converter_तथ्य->currentConverter;

        cnv->toUnicodeStatus=0;     /* offset */
        cnv->mode=0;                /* state */
        cnv->toULength=0;           /* byteIndex */
    }
}

अटल अंतरभूत व्योम
setInitialStateFromUnicodeKR(UConverter* converter,UConverter_तथ्य_ISO2022 *my_Converter_तथ्य){
   /* in ISO-2022-KR the designator sequence appears only once
    * in a file so we append it only once
    */
    अगर( converter->char_Error_बफर_लंबाई==0){

        converter->char_Error_बफर_लंबाई = 4;
        converter->char_Error_बफर[0] = 0x1b;
        converter->char_Error_बफर[1] = 0x24;
        converter->char_Error_बफर[2] = 0x29;
        converter->char_Error_बफर[3] = 0x43;
    }
    अगर(my_Converter_तथ्य->version == 1) {
        UConverter *cnv = my_Converter_तथ्य->currentConverter;

        cnv->fromUChar32=0;
        cnv->fromUnicodeStatus=1;   /* prevLength */
    }
}

अटल व्योम U_CALLCONV
_ISO2022Open(UConverter *cnv, UConverterLoadArgs *pArgs, UErrorCode *errorCode){

    अक्षर myLocale[7]={' ',' ',' ',' ',' ',' ', '\0'};

    cnv->extraInfo = uprv_malloc (माप (UConverter_तथ्य_ISO2022));
    अगर(cnv->extraInfo != nullptr) {
        UConverterNamePieces ढेर_Pieces;
        UConverterLoadArgs ढेर_Args=UCNV_LOAD_ARGS_INITIALIZER;
        UConverter_तथ्य_ISO2022* my_Converter_तथ्य = static_cast<UConverter_तथ्य_ISO2022*>(cnv->extraInfo);
        uint32_t version;

        ढेर_Args.onlyTestIsLoadable = pArgs->onlyTestIsLoadable;

        uprv_memset(my_Converter_तथ्य, 0, माप(UConverter_तथ्य_ISO2022));
        my_Converter_तथ्य->currentType = ASCII1;
        cnv->fromUnicodeStatus =false;
        अगर(pArgs->locale){
            uprv_strncpy(myLocale, pArgs->locale, माप(myLocale)-1);
        }
        version = pArgs->options & UCNV_OPTIONS_VERSION_MASK;
        my_Converter_तथ्य->version = version;
        अगर(myLocale[0]=='j' && (myLocale[1]=='a'|| myLocale[1]=='p') &&
            (myLocale[2]=='_' || myLocale[2]=='\0'))
        {
            /* open the required converters and cache them */
            अगर(version>MAX_JA_VERSION) {
                // ICU 55 fails to open a converter for an unsupported version.
                // Previously, it fell back to version 0, but that would yield
                // unexpected behavior.
                *errorCode = U_MISSING_RESOURCE_ERROR;
                वापस;
            }
            अगर(jpCharsetMasks[version]&CSM(ISO8859_7)) {
                my_Converter_तथ्य->myConverterArray[ISO8859_7] =
                    ucnv_load_Shared_तथ्य("ISO8859_7", &ढेर_Pieces, &ढेर_Args, errorCode);
            }
            my_Converter_तथ्य->myConverterArray[JISX208] =
                ucnv_load_Shared_तथ्य("Shift-JIS", &ढेर_Pieces, &ढेर_Args, errorCode);
            अगर(jpCharsetMasks[version]&CSM(JISX212)) {
                my_Converter_तथ्य->myConverterArray[JISX212] =
                    ucnv_load_Shared_तथ्य("jisx-212", &ढेर_Pieces, &ढेर_Args, errorCode);
            }
            अगर(jpCharsetMasks[version]&CSM(GB2312)) {
                my_Converter_तथ्य->myConverterArray[GB2312] =
                    ucnv_load_Shared_तथ्य("ibm-5478", &ढेर_Pieces, &ढेर_Args, errorCode);   /* gb_2312_80-1 */
            }
            अगर(jpCharsetMasks[version]&CSM(KSC5601)) {
                my_Converter_तथ्य->myConverterArray[KSC5601] =
                    ucnv_load_Shared_तथ्य("ksc_5601", &ढेर_Pieces, &ढेर_Args, errorCode);
            }

            /* set the function pointers to appropriate functions */
            cnv->shared_तथ्य = const_cast<UConverter_Shared_तथ्य*>(&_ISO2022JPData);
            uprv_strcpy(my_Converter_तथ्य->locale,"ja");

            (व्योम)uprv_strcpy(my_Converter_तथ्य->name,"ISO_2022,locale=ja,version=");
            माप_प्रकार len = uprv_strlen(my_Converter_तथ्य->name);
            my_Converter_तथ्य->name[len] = static_cast<अक्षर>(my_Converter_तथ्य->version + static_cast<पूर्णांक>('0'));
            my_Converter_तथ्य->name[len+1]='\0';
        }
#अगर !UCONFIG_ONLY_HTML_CONVERSION
        अन्यथा अगर(myLocale[0]=='k' && (myLocale[1]=='o'|| myLocale[1]=='r') &&
            (myLocale[2]=='_' || myLocale[2]=='\0'))
        {
            अगर(version>1) {
                // ICU 55 fails to open a converter for an unsupported version.
                // Previously, it fell back to version 0, but that would yield
                // unexpected behavior.
                *errorCode = U_MISSING_RESOURCE_ERROR;
                वापस;
            }
            स्थिर अक्षर *cnvName;
            अगर(version==1) {
                cnvName="icu-internal-25546";
            } अन्यथा {
                cnvName="ibm-949";
                my_Converter_तथ्य->version=version=0;
            }
            अगर(pArgs->onlyTestIsLoadable) {
                ucnv_canCreateConverter(cnvName, errorCode);  /* errorCode carries result */
                uprv_free(cnv->extraInfo);
                cnv->extraInfo=nullptr;
                वापस;
            } अन्यथा {
                my_Converter_तथ्य->currentConverter=ucnv_open(cnvName, errorCode);
                अगर (U_FAILURE(*errorCode)) {
                    _ISO2022Close(cnv);
                    वापस;
                }

                अगर(version==1) {
                    (व्योम)uprv_strcpy(my_Converter_तथ्य->name,"ISO_2022,locale=ko,version=1");
                    uprv_memcpy(cnv->subChars, my_Converter_तथ्य->currentConverter->subChars, 4);
                    cnv->subCharLen = my_Converter_तथ्य->currentConverter->subCharLen;
                }अन्यथा{
                    (व्योम)uprv_strcpy(my_Converter_तथ्य->name,"ISO_2022,locale=ko,version=0");
                }

                /* initialize the state variables */
                setInitialStateToUnicodeKR(cnv, my_Converter_तथ्य);
                setInitialStateFromUnicodeKR(cnv, my_Converter_तथ्य);

                /* set the function pointers to appropriate functions */
                cnv->shared_तथ्य = const_cast<UConverter_Shared_तथ्य*>(&_ISO2022KRData);
                uprv_strcpy(my_Converter_तथ्य->locale,"ko");
            }
        }
        अन्यथा अगर(((myLocale[0]=='z' && myLocale[1]=='h') || (myLocale[0]=='c'&& myLocale[1]=='n'))&&
            (myLocale[2]=='_' || myLocale[2]=='\0'))
        {
            अगर(version>2) {
                // ICU 55 fails to open a converter for an unsupported version.
                // Previously, it fell back to version 0, but that would yield
                // unexpected behavior.
                *errorCode = U_MISSING_RESOURCE_ERROR;
                वापस;
            }

            /* open the required converters and cache them */
            my_Converter_तथ्य->myConverterArray[GB2312_1] =
                ucnv_load_Shared_तथ्य("ibm-5478", &ढेर_Pieces, &ढेर_Args, errorCode);
            अगर(version>=1) {
                my_Converter_तथ्य->myConverterArray[ISO_IR_165] =
                    ucnv_load_Shared_तथ्य("iso-ir-165", &ढेर_Pieces, &ढेर_Args, errorCode);
            }
            my_Converter_तथ्य->myConverterArray[CNS_11643] =
                ucnv_load_Shared_तथ्य("cns-11643-1992", &ढेर_Pieces, &ढेर_Args, errorCode);


            /* set the function pointers to appropriate functions */
            cnv->shared_तथ्य = const_cast<UConverter_Shared_तथ्य*>(&_ISO2022CNData);
            uprv_strcpy(my_Converter_तथ्य->locale,"cn");

            अगर (version==0){
                my_Converter_तथ्य->version = 0;
                (व्योम)uprv_strcpy(my_Converter_तथ्य->name,"ISO_2022,locale=zh,version=0");
            }अन्यथा अगर (version==1){
                my_Converter_तथ्य->version = 1;
                (व्योम)uprv_strcpy(my_Converter_तथ्य->name,"ISO_2022,locale=zh,version=1");
            }अन्यथा {
                my_Converter_तथ्य->version = 2;
                (व्योम)uprv_strcpy(my_Converter_तथ्य->name,"ISO_2022,locale=zh,version=2");
            }
        }
#पूर्ण_अगर  // !UCONFIG_ONLY_HTML_CONVERSION
        अन्यथा{
#अगर_घोषित U_ENABLE_GENERIC_ISO_2022
            my_Converter_तथ्य->is_First_बफर = true;

            /* append the UTF-8 escape sequence */
            cnv->char_Error_बफर_लंबाई = 3;
            cnv->char_Error_बफर[0] = 0x1b;
            cnv->char_Error_बफर[1] = 0x25;
            cnv->char_Error_बफर[2] = 0x42;

            cnv->shared_तथ्य=(UConverter_Shared_तथ्य*)&ISO2022_तथ्य;
            /* initialize the state variables */
            uprv_strcpy(my_Converter_तथ्य->name,"ISO_2022");
#अन्यथा
            *errorCode = U_MISSING_RESOURCE_ERROR;
            // Was U_UNSUPPORTED_ERROR but changed in ICU 55 to a more standard
            // data loading error code.
            वापस;
#पूर्ण_अगर
        }

        cnv->maxBytesPerUChar=cnv->shared_तथ्य->static_तथ्य->maxBytesPerChar;

        अगर(U_FAILURE(*errorCode) || pArgs->onlyTestIsLoadable) {
            _ISO2022Close(cnv);
        }
    } अन्यथा {
        *errorCode = U_MEMORY_ALLOCATION_ERROR;
    }
}


अटल व्योम U_CALLCONV
_ISO2022Close(UConverter *converter) {
    UConverter_तथ्य_ISO2022* my_तथ्य = static_cast<UConverter_तथ्य_ISO2022*>(converter->extraInfo);
    UConverter_Shared_तथ्य **array = my_तथ्य->myConverterArray;
    int32_t i;

    अगर (converter->extraInfo != nullptr) {
        /*close the array of converter pointers and free the memory*/
        क्रम (i=0; i<UCNV_2022_MAX_CONVERTERS; i++) {
            अगर(array[i]!=nullptr) {
                ucnv_unload_Shared_तथ्य_If_Ready(array[i]);
            }
        }

        ucnv_close(my_तथ्य->currentConverter);

        अगर(!converter->isExtraLocal){
            uprv_free (converter->extraInfo);
            converter->extraInfo = nullptr;
        }
    }
}

अटल व्योम U_CALLCONV
_ISO2022Reset(UConverter *converter, UConverterResetChoice choice) {
    UConverter_तथ्य_ISO2022* my_Converter_तथ्य = static_cast<UConverter_तथ्य_ISO2022*>(converter->extraInfo);
    अगर(choice<=UCNV_RESET_TO_UNICODE) {
        uprv_memset(&my_Converter_तथ्य->toU2022State, 0, माप(ISO2022State));
        my_Converter_तथ्य->कुंजी = 0;
        my_Converter_तथ्य->isEmptySegment = false;
    }
    अगर(choice!=UCNV_RESET_TO_UNICODE) {
        uprv_memset(&my_Converter_तथ्य->fromU2022State, 0, माप(ISO2022State));
    }
#अगर_घोषित U_ENABLE_GENERIC_ISO_2022
    अगर(my_Converter_तथ्य->locale[0] == 0){
        अगर(choice<=UCNV_RESET_TO_UNICODE) {
            my_Converter_तथ्य->is_First_बफर = true;
            my_Converter_तथ्य->कुंजी = 0;
            अगर (converter->mode == UCNV_SO){
                ucnv_close (my_Converter_तथ्य->currentConverter);
                my_Converter_तथ्य->currentConverter=nullptr;
            }
            converter->mode = UCNV_SI;
        }
        अगर(choice!=UCNV_RESET_TO_UNICODE) {
            /* re-append UTF-8 escape sequence */
            converter->char_Error_बफर_लंबाई = 3;
            converter->char_Error_बफर[0] = 0x1b;
            converter->char_Error_बफर[1] = 0x28;
            converter->char_Error_बफर[2] = 0x42;
        }
    }
    अन्यथा
#पूर्ण_अगर
    {
        /* reset the state variables */
        अगर(my_Converter_तथ्य->locale[0] == 'k'){
            अगर(choice<=UCNV_RESET_TO_UNICODE) {
                setInitialStateToUnicodeKR(converter, my_Converter_तथ्य);
            }
            अगर(choice!=UCNV_RESET_TO_UNICODE) {
                setInitialStateFromUnicodeKR(converter, my_Converter_तथ्य);
            }
        }
    }
}

U_CDECL_BEGIN

अटल स्थिर अक्षर * U_CALLCONV
_ISO2022getName(स्थिर UConverter* cnv){
    अगर(cnv->extraInfo){
        UConverter_तथ्य_ISO2022* my_तथ्य= (UConverter_तथ्य_ISO2022*)cnv->extraInfo;
        वापस my_तथ्य->name;
    }
    वापस nullptr;
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
अटल स्थिर int8_t अग्र_State_To_Unicode_JP[MAX_STATES_2022]= {
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

#अगर !UCONFIG_ONLY_HTML_CONVERSION
/*************** to unicode *******************/
अटल स्थिर int8_t अग्र_State_To_Unicode_CN[MAX_STATES_2022]= {
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
#पूर्ण_अगर


अटल UCNV_TableStates_2022
get_कुंजी_2022(अक्षर c,int32_t* कुंजी,int32_t* offset){
    int32_t togo;
    int32_t निचला = 0;
    int32_t hi = MAX_STATES_2022;
    int32_t oldmid=0;

    togo = normalize_esq_chars_2022[static_cast<uint8_t>(c)];
    अगर(togo == 0) {
        /* not a valid character anywhere in an escape sequence */
        *कुंजी = 0;
        *offset = 0;
        वापस INVALID_2022;
    }
    togo = (*कुंजी << 5) + togo;

    जबतक (hi != निचला)  /*द्विआधारी खोज*/{

        int32_t mid = (hi+निचला) >> 1; /*Finds median*/

        अगर (mid == oldmid)
            अवरोध;

        अगर (esc_Seq_State_Table_कुंजी_2022[mid] > togo){
            hi = mid;
        }
        अन्यथा अगर (esc_Seq_State_Table_कुंजी_2022[mid] < togo){
            निचला = mid;
        }
        अन्यथा /*we found it*/{
            *कुंजी = togo;
            *offset = mid;
            वापस static_cast<UCNV_TableStates_2022>(esc_Seq_State_Table_मूल्य_2022[mid]);
        }
        oldmid = mid;

    }

    *कुंजी = 0;
    *offset = 0;
    वापस INVALID_2022;
}

/*runs through a state machine to determine the escape sequence - codepage correspondence
 */
अटल व्योम
changeState_2022(UConverter* _this,
                स्थिर अक्षर** source,
                स्थिर अक्षर* sourceLimit,
                Variant2022 var,
                UErrorCode* err){
    UCNV_TableStates_2022 value;
    UConverter_तथ्य_ISO2022* myData2022 = static_cast<UConverter_तथ्य_ISO2022*>(_this->extraInfo);
    uint32_t कुंजी = myData2022->कुंजी;
    int32_t offset = 0;
    int8_t initialToULength = _this->toULength;
    अक्षर c;

    value = VALID_NON_TERMINAL_2022;
    जबतक (*source < sourceLimit) {
        c = *(*source)++;
        _this->toUBytes[_this->toULength++] = static_cast<uint8_t>(c);
        value = get_कुंजी_2022(c, reinterpret_cast<int32_t*>(&कुंजी), &offset);

        चयन (value){

        हाल VALID_NON_TERMINAL_2022 :
            /* continue with the loop */
            अवरोध;

        हाल VALID_TERMINAL_2022:
            कुंजी = 0;
            जाओ DONE;

        हाल INVALID_2022:
            जाओ DONE;

        हाल VALID_MAYBE_TERMINAL_2022:
#अगर_घोषित U_ENABLE_GENERIC_ISO_2022
            /* ESC ( B is ambiguous only for ISO_2022 itself */
            अगर(var == ISO_2022) {
                /* discard toUBytes[] for ESC ( B because this sequence is correct and complete */
                _this->toULength = 0;

                /* TODO need to indicate that ESC ( B was seen; if failure, then need to replay from source or from MBCS-style replay */

                /* continue with the loop */
                value = VALID_NON_TERMINAL_2022;
                अवरोध;
            } अन्यथा
#पूर्ण_अगर
            {
                /* not ISO_2022 itself, finish here */
                value = VALID_TERMINAL_2022;
                कुंजी = 0;
                जाओ DONE;
            }
        }
    }

DONE:
    myData2022->कुंजी = कुंजी;

    अगर (value == VALID_NON_TERMINAL_2022) {
        /* indicate that the escape sequence is incomplete: key!=0 */
        वापस;
    } अन्यथा अगर (value == INVALID_2022 ) {
        *err = U_ILLEGAL_ESCAPE_SEQUENCE;
    } अन्यथा /* value == VALID_TERMINAL_2022 */ {
        चयन(var){
#अगर_घोषित U_ENABLE_GENERIC_ISO_2022
        हाल ISO_2022:
        {
            स्थिर अक्षर *chosenConverterName = esc_Seq_State_Table_परिणाम_2022[offset];
            अगर(chosenConverterName == nullptr) {
                /* SS2 or SS3 */
                *err = U_UNSUPPORTED_ESCAPE_SEQUENCE;
                _this->toUCallbackReason = UCNV_UNASSIGNED;
                वापस;
            }

            _this->mode = UCNV_SI;
            ucnv_close(myData2022->currentConverter);
            myData2022->currentConverter = myUConverter = ucnv_open(chosenConverterName, err);
            अगर(U_SUCCESS(*err)) {
                myUConverter->fromCharErrorBehaviour = UCNV_TO_U_CALLBACK_STOP;
                _this->mode = UCNV_SO;
            }
            अवरोध;
        }
#पूर्ण_अगर
        हाल ISO_2022_JP:
            {
                StateEnum अस्थायी_State = static_cast<StateEnum>(अग्र_State_To_Unicode_JP[offset]);
                चयन(अस्थायी_State) {
                हाल INVALID_STATE:
                    *err = U_UNSUPPORTED_ESCAPE_SEQUENCE;
                    अवरोध;
                हाल SS2_STATE:
                    अगर(myData2022->toU2022State.cs[2]!=0) {
                        अगर(myData2022->toU2022State.g<2) {
                            myData2022->toU2022State.पूर्व_G=myData2022->toU2022State.g;
                        }
                        myData2022->toU2022State.g=2;
                    } अन्यथा {
                        /* illegal to have SS2 before a matching designator */
                        *err = U_ILLEGAL_ESCAPE_SEQUENCE;
                    }
                    अवरोध;
                /* case SS3_STATE: not used in ISO-2022-JP-x */
                हाल ISO8859_1:
                हाल ISO8859_7:
                    अगर((jpCharsetMasks[myData2022->version] & CSM(अस्थायी_State)) == 0) {
                        *err = U_UNSUPPORTED_ESCAPE_SEQUENCE;
                    } अन्यथा {
                        /* G2 charset for SS2 */
                        myData2022->toU2022State.cs[2] = static_cast<int8_t>(अस्थायी_State);
                    }
                    अवरोध;
                शेष:
                    अगर((jpCharsetMasks[myData2022->version] & CSM(अस्थायी_State)) == 0) {
                        *err = U_UNSUPPORTED_ESCAPE_SEQUENCE;
                    } अन्यथा {
                        /* G0 charset */
                        myData2022->toU2022State.cs[0] = static_cast<int8_t>(अस्थायी_State);
                    }
                    अवरोध;
                }
            }
            अवरोध;
#अगर !UCONFIG_ONLY_HTML_CONVERSION
        हाल ISO_2022_CN:
            {
                StateEnum अस्थायी_State = static_cast<StateEnum>(अग्र_State_To_Unicode_CN[offset]);
                चयन(अस्थायी_State) {
                हाल INVALID_STATE:
                    *err = U_UNSUPPORTED_ESCAPE_SEQUENCE;
                    अवरोध;
                हाल SS2_STATE:
                    अगर(myData2022->toU2022State.cs[2]!=0) {
                        अगर(myData2022->toU2022State.g<2) {
                            myData2022->toU2022State.पूर्व_G=myData2022->toU2022State.g;
                        }
                        myData2022->toU2022State.g=2;
                    } अन्यथा {
                        /* illegal to have SS2 before a matching designator */
                        *err = U_ILLEGAL_ESCAPE_SEQUENCE;
                    }
                    अवरोध;
                हाल SS3_STATE:
                    अगर(myData2022->toU2022State.cs[3]!=0) {
                        अगर(myData2022->toU2022State.g<2) {
                            myData2022->toU2022State.पूर्व_G=myData2022->toU2022State.g;
                        }
                        myData2022->toU2022State.g=3;
                    } अन्यथा {
                        /* illegal to have SS3 before a matching designator */
                        *err = U_ILLEGAL_ESCAPE_SEQUENCE;
                    }
                    अवरोध;
                हाल ISO_IR_165:
                    अगर(myData2022->version==0) {
                        *err = U_UNSUPPORTED_ESCAPE_SEQUENCE;
                        अवरोध;
                    }
                    U_FALLTHROUGH;
                हाल GB2312_1:
                    U_FALLTHROUGH;
                हाल CNS_11643_1:
                    myData2022->toU2022State.cs[1] = static_cast<int8_t>(अस्थायी_State);
                    अवरोध;
                हाल CNS_11643_2:
                    myData2022->toU2022State.cs[2] = static_cast<int8_t>(अस्थायी_State);
                    अवरोध;
                शेष:
                    /* other CNS 11643 planes */
                    अगर(myData2022->version==0) {
                        *err = U_UNSUPPORTED_ESCAPE_SEQUENCE;
                    } अन्यथा {
                        myData2022->toU2022State.cs[3] = static_cast<int8_t>(अस्थायी_State);
                    }
                    अवरोध;
                }
            }
            अवरोध;
        हाल ISO_2022_KR:
            अगर(offset==0x30){
                /* nothing to be done, just accept this one escape sequence */
            } अन्यथा {
                *err = U_UNSUPPORTED_ESCAPE_SEQUENCE;
            }
            अवरोध;
#पूर्ण_अगर  // !UCONFIG_ONLY_HTML_CONVERSION

        शेष:
            *err = U_ILLEGAL_ESCAPE_SEQUENCE;
            अवरोध;
        }
    }
    अगर(U_SUCCESS(*err)) {
        _this->toULength = 0;
    } अन्यथा अगर(*err==U_ILLEGAL_ESCAPE_SEQUENCE) {
        अगर(_this->toULength>1) {
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
            int8_t bytes_From_This_बफर=_this->toULength-initialToULength;
            अगर(backOutDistance<=bytes_From_This_बफर) {
                /* same as initialToULength<=1 */
                *source-=backOutDistance;
            } अन्यथा {
                /* Back out bytes from the previous buffer: Need to replay them. */
                _this->preToULength = static_cast<int8_t>(bytes_From_This_बफर - backOutDistance);
                /* same as -(initialToULength-1) */
                /* preToULength is negative! */
                uprv_memcpy(_this->preToU, _this->toUBytes+1, -_this->preToULength);
                *source-=bytes_From_This_बफर;
            }
            _this->toULength=1;
        }
    } अन्यथा अगर(*err==U_UNSUPPORTED_ESCAPE_SEQUENCE) {
        _this->toUCallbackReason = UCNV_UNASSIGNED;
    }
}

#अगर !UCONFIG_ONLY_HTML_CONVERSION
/*Checks the characters of the buffer against valid 2022 escape sequences
*if the match we return a pointer to the initial start of the sequence otherwise
*we return sourceLimit
*/
/*for 2022 looks ahead in the stream
 *to determine the longest possible convertible
 *data stream
 */
अटल अंतरभूत स्थिर अक्षर*
get_End_Of_बफर_2022(स्थिर अक्षर** source,
                   स्थिर अक्षर* sourceLimit,
                   UBool /*flush*/){

    स्थिर अक्षर* mySource = *source;

#अगर_घोषित U_ENABLE_GENERIC_ISO_2022
    अगर (*source >= sourceLimit)
        वापस sourceLimit;

    करो{

        अगर (*mySource == ESC_2022){
            int8_t i;
            int32_t कुंजी = 0;
            int32_t offset;
            UCNV_TableStates_2022 value = VALID_NON_TERMINAL_2022;

            /* Kludge: I could not
            * figure out the reason for validating an escape sequence
            * twice - once here and once in changeState_2022().
            * is it possible to have an ESC character in a ISO2022
            * byte stream which is valid in a code page? Is it legal?
            */
            क्रम (i=0;
            (mySource+i < sourceLimit)&&(value == VALID_NON_TERMINAL_2022);
            i++) {
                value =  get_कुंजी_2022(*(mySource+i), &कुंजी, &offset);
            }
            अगर (value > 0 || *mySource==ESC_2022)
                वापस mySource;

            अगर ((value == VALID_NON_TERMINAL_2022)&&(!सफाई) )
                वापस sourceLimit;
        }
    }जबतक (++mySource < sourceLimit);

    वापस sourceLimit;
#अन्यथा
    जबतक(mySource < sourceLimit && *mySource != ESC_2022) {
        ++mySource;
    }
    वापस mySource;
#पूर्ण_अगर
}
#पूर्ण_अगर

/* This inline function replicates code in _MBCSFromUChar32() function in ucnvmbcs.c
 * any future change in _MBCSFromUChar32() function should be reflected here.
 * @return number of bytes in *value; negative number if fallback; 0 if no mapping
 */
अटल अंतरभूत int32_t
MBCS_FROM_UCHAR32_ISO2022(UConverter_Shared_तथ्य* shared_तथ्य,
                                         UChar32 c,
                                         uint32_t* value,
                                         UBool useFallback,
                                         पूर्णांक outputType)
{
    स्थिर int32_t *cx;
    स्थिर uint16_t *table;
    uint32_t stage2Entry;
    uint32_t my_मूल्य;
    int32_t length;
    स्थिर uint8_t *p;
    /*
     * TODO(markus): Use and require new, faster MBCS conversion table structures.
     * Use internal version of ucnv_open() that verifies that the new structures are available,
     * else U_INTERNAL_PROGRAM_ERROR.
     */
    /* BMP-only codepages are stored without stage 1 entries for supplementary code points */
    अगर(c<0x10000 || (shared_तथ्य->mbcs.unicodeMask&UCNV_HAS_SUPPLEMENTARY)) {
        table=shared_तथ्य->mbcs.fromUnicodeTable;
        stage2Entry=MBCS_STAGE_2_FROM_U(table, c);
        /* get the bytes and the length for the output */
        अगर(outputType==MBCS_OUTPUT_2){
            my_मूल्य=MBCS_मूल्य_2_FROM_STAGE_2(shared_तथ्य->mbcs.fromUnicodeBytes, stage2Entry, c);
            अगर(my_मूल्य<=0xff) {
                length=1;
            } अन्यथा {
                length=2;
            }
        } अन्यथा /* outputType==MBCS_OUTPUT_3 */ {
            p=MBCS_POINTER_3_FROM_STAGE_2(shared_तथ्य->mbcs.fromUnicodeBytes, stage2Entry, c);
            my_मूल्य = (static_cast<uint32_t>(*p) << 16) | (static_cast<uint32_t>(p[1]) << 8) | p[2];
            अगर(my_मूल्य<=0xff) {
                length=1;
            } अन्यथा अगर(my_मूल्य<=0xffff) {
                length=2;
            } अन्यथा {
                length=3;
            }
        }
        /* is this code point assigned, or do we use fallbacks? */
        अगर((stage2Entry&(1<<(16+(c&0xf))))!=0) {
            /* assigned */
            *value=my_मूल्य;
            वापस length;
        } अन्यथा अगर(FROM_U_USE_FALLBACK(useFallback, c) && my_मूल्य!=0) {
            /*
             * We allow a 0 byte output if the "assigned" bit is set for this entry.
             * There is no way with this data structure for fallback output
             * to be a zero byte.
             */
            *value=my_मूल्य;
            वापस -length;
        }
    }

    cx=shared_तथ्य->mbcs.extIndexes;
    अगर(cx!=nullptr) {
        वापस ucnv_extSimpleMatchFromU(cx, c, value, useFallback);
    }

    /* unassigned */
    वापस 0;
}

/* This inline function replicates code in _MBCSSingleFromUChar32() function in ucnvmbcs.c
 * any future change in _MBCSSingleFromUChar32() function should be reflected here.
 * @param retval pointer to output byte
 * @return 1 roundtrip byte  0 no mapping  -1 fallback byte
 */
अटल अंतरभूत int32_t
MBCS_SINGLE_FROM_UCHAR32(UConverter_Shared_तथ्य* shared_तथ्य,
                                       UChar32 c,
                                       uint32_t* retval,
                                       UBool useFallback)
{
    स्थिर uint16_t *table;
    int32_t value;
    /* BMP-only codepages are stored without stage 1 entries for supplementary code points */
    अगर(c>=0x10000 && !(shared_तथ्य->mbcs.unicodeMask&UCNV_HAS_SUPPLEMENTARY)) {
        वापस 0;
    }
    /* convert the Unicode code point in c into codepage bytes (same as in _MBCSFromUnicodeWithOffsets) */
    table=shared_तथ्य->mbcs.fromUnicodeTable;
    /* get the byte for the output */
    value=MBCS_SINGLE_परिणाम_FROM_U(table, (uint16_t *)shared_तथ्य->mbcs.fromUnicodeBytes, c);
    /* is this code point assigned, or do we use fallbacks? */
    *retval = static_cast<uint32_t>(value & 0xff);
    अगर(value>=0xf00) {
        वापस 1;  /* roundtrip */
    } अन्यथा अगर(useFallback ? value>=0x800 : value>=0xc00) {
        वापस -1;  /* fallback taken */
    } अन्यथा {
        वापस 0;  /* no mapping */
    }
}

/*
 * Check that the result is a 2-byte value with each byte in the range A1..FE
 * (strict EUC DBCS) before accepting it and subtracting 0x80 from each byte
 * to move it to the ISO 2022 range 21..7E.
 * Return 0 if out of range.
 */
अटल अंतरभूत uint32_t
_2022FromGR94DBCS(uint32_t value) {
    अगर (static_cast<uint16_t>(value - 0xa1a1) <= (0xfefe - 0xa1a1) &&
        static_cast<uint8_t>(value - 0xa1) <= (0xfe - 0xa1)
    ) {
        वापस value - 0x8080;  /* shift down to 21..7e byte range */
    } अन्यथा {
        वापस 0;  /* not valid for ISO 2022 */
    }
}

#अगर 0 /* 5691: Call sites now check for validity. They can just += 0x8080 after that. */
/*
 * This method does the reverse of _2022FromGR94DBCS(). Given the 2022 code point, it returns the
 * 2 byte value that is in the range A1..FE for each byte. Otherwise it returns the 2022 code point
 * unchanged. 
 */
अटल अंतरभूत uint32_t
_2022ToGR94DBCS(uint32_t value) {
    uint32_t return_मूल्य = value + 0x8080;
    अगर( (uint16_t)(return_मूल्य - 0xa1a1) <= (0xfefe - 0xa1a1) &&
        (uint8_t)(return_मूल्य - 0xa1) <= (0xfe - 0xa1)) {
        वापस return_मूल्य;
    } अन्यथा {
        वापस value;
    }
}
#पूर्ण_अगर

#अगर_घोषित U_ENABLE_GENERIC_ISO_2022

/**********************************************************************************
*  ISO-2022 Converter
*
*
*/

अटल व्योम U_CALLCONV
T_UConverter_toUnicode_ISO_2022_OFFSETS_LOGIC(UConverterToUnicodeArgs* args,
                                                           UErrorCode* err){
    स्थिर अक्षर* mySourceLimit, *realSourceLimit;
    स्थिर अक्षर* sourceStart;
    स्थिर char16_t* myTargetStart;
    UConverter* saveThis;
    UConverter_तथ्य_ISO2022* my_तथ्य;
    int8_t length;

    saveThis = args->converter;
    my_तथ्य=((UConverter_तथ्य_ISO2022*)(saveThis->extraInfo));

    realSourceLimit = args->sourceLimit;
    जबतक (args->source < realSourceLimit) {
        अगर(my_तथ्य->कुंजी == 0) { /* are we in the middle of an escape sequence? */
            /*Find the end of the buffer e.g : Next Escape Seq | end of Buffer*/
            mySourceLimit = get_End_Of_बफर_2022(&(args->source), realSourceLimit, args->सफाई);

            अगर(args->source < mySourceLimit) {
                अगर(my_तथ्य->currentConverter==nullptr) {
                    my_तथ्य->currentConverter = ucnv_open("ASCII",err);
                    अगर(U_FAILURE(*err)){
                        वापस;
                    }

                    my_तथ्य->currentConverter->fromCharErrorBehaviour = UCNV_TO_U_CALLBACK_STOP;
                    saveThis->mode = UCNV_SO;
                }

                /* convert to before the ESC or until the end of the buffer */
                my_तथ्य->is_First_बफर=false;
                sourceStart = args->source;
                myTargetStart = args->target;
                args->converter = my_तथ्य->currentConverter;
                ucnv_toUnicode(args->converter,
                    &args->target,
                    args->targetLimit,
                    &args->source,
                    mySourceLimit,
                    args->offsets,
                    (UBool)(args->सफाई && mySourceLimit == realSourceLimit),
                    err);
                args->converter = saveThis;

                अगर (*err == U_बफर_OVERFLOW_ERROR) {
                    /* move the overflow buffer */
                    length = saveThis->UChar_Error_बफर_लंबाई = my_तथ्य->currentConverter->UChar_Error_बफर_लंबाई;
                    my_तथ्य->currentConverter->UChar_Error_बफर_लंबाई = 0;
                    अगर(length > 0) {
                        uprv_memcpy(saveThis->UChar_Error_बफर,
                                    my_तथ्य->currentConverter->UChar_Error_बफर,
                                    length*U_SIZEOF_UCHAR);
                    }
                    वापस;
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
                अगर (U_FAILURE(*err) ||
                    (args->source == realSourceLimit) ||
                    (args->offsets != nullptr && (args->target != myTargetStart || args->source != sourceStart) ||
                    (mySourceLimit < realSourceLimit && my_तथ्य->currentConverter->toULength > 0))
                ) {
                    /* copy partial or error input for truncated detection and error handling */
                    अगर(U_FAILURE(*err)) {
                        length = saveThis->invalid_Char_लंबाई = my_तथ्य->currentConverter->invalid_Char_लंबाई;
                        अगर(length > 0) {
                            uprv_memcpy(saveThis->invalid_Char_बफर, my_तथ्य->currentConverter->invalid_Char_बफर, length);
                        }
                    } अन्यथा {
                        length = saveThis->toULength = my_तथ्य->currentConverter->toULength;
                        अगर(length > 0) {
                            uprv_memcpy(saveThis->toUBytes, my_तथ्य->currentConverter->toUBytes, length);
                            अगर(args->source < mySourceLimit) {
                                *err = U_TRUNCATED_CHAR_FOUND; /* truncated input before ESC */
                            }
                        }
                    }
                    वापस;
                }
            }
        }

        sourceStart = args->source;
        changeState_2022(args->converter,
               &(args->source),
               realSourceLimit,
               ISO_2022,
               err);
        अगर (U_FAILURE(*err) || (args->source != sourceStart && args->offsets != nullptr)) {
            /* let the ucnv.c code update its current offset */
            वापस;
        }
    }
}

#पूर्ण_अगर

/*
 * To Unicode Callback helper function
 */
अटल व्योम
toUnicodeCallback(UConverter *cnv,
                  स्थिर uint32_t sourceChar, स्थिर uint32_t targetUniChar,
                  UErrorCode* err){
    अगर(sourceChar>0xff){
        cnv->toUBytes[0] = static_cast<uint8_t>(sourceChar >> 8);
        cnv->toUBytes[1] = static_cast<uint8_t>(sourceChar);
        cnv->toULength = 2;
    }
    अन्यथा{
        cnv->toUBytes[0] = static_cast<अक्षर>(sourceChar);
        cnv->toULength = 1;
    }

    अगर(targetUniChar == (missingCharMarker-1/*0xfffe*/)){
        *err = U_INVALID_CHAR_FOUND;
    }
    अन्यथा{
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
अटल स्थिर StateEnum jpCharsetPref[]={
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
अटल स्थिर अक्षर escSeqChars[][6] ={
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
अटल  स्थिर int8_t escSeqCharsLen[] ={
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
अटल अंतरभूत uint32_t
jisx201ToU(uint32_t value) {
    अगर(value < 0x5c) {
        वापस value;
    } अन्यथा अगर(value == 0x5c) {
        वापस 0xa5;
    } अन्यथा अगर(value == 0x7e) {
        वापस 0x203e;
    } अन्यथा /* value <= 0x7f */ {
        वापस value;
    }
}

/* Map Unicode to 00..7F according to JIS X 0201. Return U+FFFE if unmappable. */
अटल अंतरभूत uint32_t
jisx201FromU(uint32_t value) {
    अगर(value<=0x7f) {
        अगर(value!=0x5c && value!=0x7e) {
            वापस value;
        }
    } अन्यथा अगर(value==0xa5) {
        वापस 0x5c;
    } अन्यथा अगर(value==0x203e) {
        वापस 0x7e;
    }
    वापस 0xfffe;
}

/*
 * Take a valid Shift-JIS byte pair, check that it is in the range corresponding
 * to JIS X 0208, and convert it to a pair of 21..7E bytes.
 * Return 0 if the byte pair is out of range.
 */
अटल अंतरभूत uint32_t
_2022FromSJIS(uint32_t value) {
    uint8_t trail;

    अगर(value > 0xEFFC) {
        वापस 0;  /* beyond JIS X 0208 */
    }

    trail = static_cast<uint8_t>(value);

    value &= 0xff00;  /* lead byte */
    अगर(value <= 0x9f00) {
        value -= 0x7000;
    } अन्यथा /* 0xe000 <= value <= 0xef00 */ {
        value -= 0xb000;
    }
    value <<= 1;

    अगर(trail <= 0x9e) {
        value -= 0x100;
        अगर(trail <= 0x7e) {
            value |= trail - 0x1f;
        } अन्यथा {
            value |= trail - 0x20;
        }
    } अन्यथा /* trail <= 0xfc */ {
        value |= trail - 0x7e;
    }
    वापस value;
}

/*
 * Convert a pair of JIS X 0208 21..7E bytes to Shift-JIS.
 * If either byte is outside 21..7E make sure that the result is not valid
 * for Shift-JIS so that the converter catches it.
 * Some invalid byte values already turn into equally invalid Shift-JIS
 * byte values and need not be tested explicitly.
 */
अटल अंतरभूत व्योम
_2022ToSJIS(uint8_t c1, uint8_t c2, अक्षर bytes[2]) {
    अगर(c1&1) {
        ++c1;
        अगर(c2 <= 0x5f) {
            c2 += 0x1f;
        } अन्यथा अगर(c2 <= 0x7e) {
            c2 += 0x20;
        } अन्यथा {
            c2 = 0;  /* invalid */
        }
    } अन्यथा {
        अगर (static_cast<uint8_t>(c2 - 0x21) <= ((0x7e) - 0x21)) {
            c2 += 0x7e;
        } अन्यथा {
            c2 = 0;  /* invalid */
        }
    }
    c1 >>= 1;
    अगर(c1 <= 0x2f) {
        c1 += 0x70;
    } अन्यथा अगर(c1 <= 0x3f) {
        c1 += 0xb0;
    } अन्यथा {
        c1 = 0;  /* invalid */
    }
    bytes[0] = static_cast<अक्षर>(c1);
    bytes[1] = static_cast<अक्षर>(c2);
}

/*
 * JIS X 0208 has fallbacks from Unicode half-width Katakana to full-width (DBCS)
 * Katakana.
 * Now that we use a Shift-JIS table for JIS X 0208 we need to hardcode these fallbacks
 * because Shift-JIS roundtrips half-width Katakana to single bytes.
 * These were the only fallbacks in ICU's jisx-208.ucm file.
 */
अटल स्थिर uint16_t hwkana_fb[HWKANA_END - HWKANA_START + 1] = {
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

अटल व्योम U_CALLCONV
UConverter_fromUnicode_ISO_2022_JP_OFFSETS_LOGIC(UConverterFromUnicodeArgs* args, UErrorCode* err) {
    UConverter *cnv = args->converter;
    UConverter_तथ्य_ISO2022 *converter_तथ्य;
    ISO2022State *pFromU2022State;
    uint8_t* target = reinterpret_cast<uint8_t*>(args->target);
    स्थिर uint8_t* targetLimit = reinterpret_cast<स्थिर uint8_t*>(args->targetLimit);
    स्थिर char16_t* source = args->source;
    स्थिर char16_t* sourceLimit = args->sourceLimit;
    int32_t* offsets = args->offsets;
    UChar32 sourceChar;
    अक्षर बफर[8];
    int32_t len, outLen;
    int8_t choices[10];
    int32_t choice_गिनती;
    uint32_t target_मूल्य = 0;
    UBool useFallback;

    int32_t i;
    int8_t cs, g;

    /* set up the state */
    converter_तथ्य = static_cast<UConverter_तथ्य_ISO2022*>(cnv->extraInfo);
    pFromU2022State   = &converter_तथ्य->fromU2022State;

    choice_गिनती = 0;

    /* check if the last codepoint of previous buffer was a lead surrogate*/
    अगर((sourceChar = cnv->fromUChar32)!=0 && target< targetLimit) {
        जाओ getTrail;
    }

    जबतक(source < sourceLimit) {
        अगर(target < targetLimit) {

            sourceChar  = *(source++);
            /*check if the char is a First surrogate*/
            अगर(U16_IS_SURROGATE(sourceChar)) {
                अगर(U16_IS_SURROGATE_LEAD(sourceChar)) {
getTrail:
                    /*look ahead to find the trail surrogate*/
                    अगर(source < sourceLimit) {
                        /* test the following code unit */
                        char16_t trail = *source;
                        अगर(U16_IS_TRAIL(trail)) {
                            source++;
                            sourceChar=U16_GET_SUPPLEMENTARY(sourceChar, trail);
                            cnv->fromUChar32=0x00;
                            /* convert this supplementary code point */
                            /* exit this condition tree */
                        } अन्यथा {
                            /* this is an unmatched lead code unit (1st surrogate) */
                            /* callback(illegal) */
                            *err=U_ILLEGAL_CHAR_FOUND;
                            cnv->fromUChar32=sourceChar;
                            अवरोध;
                        }
                    } अन्यथा {
                        /* no more input */
                        cnv->fromUChar32=sourceChar;
                        अवरोध;
                    }
                } अन्यथा {
                    /* this is an unmatched trail code unit (2nd surrogate) */
                    /* callback(illegal) */
                    *err=U_ILLEGAL_CHAR_FOUND;
                    cnv->fromUChar32=sourceChar;
                    अवरोध;
                }
            }

            /* do not convert SO/SI/ESC */
            अगर(IS_2022_CONTROL(sourceChar)) {
                /* callback(illegal) */
                *err=U_ILLEGAL_CHAR_FOUND;
                cnv->fromUChar32=sourceChar;
                अवरोध;
            }

            /* do the conversion */

            अगर(choice_गिनती == 0) {
                uint16_t csm;

                /*
                 * The csm variable keeps track of which charsets are allowed
                 * and not used yet while building the choices[].
                 */
                csm = jpCharsetMasks[converter_तथ्य->version];
                choice_गिनती = 0;

                /* JIS7/8: try single-byte half-width Katakana before JISX208 */
                अगर(converter_तथ्य->version == 3 || converter_तथ्य->version == 4) {
                    choices[choice_गिनती++] = static_cast<int8_t>(HWKANA_7BIT);
                }
                /* Do not try single-byte half-width Katakana for other versions. */
                csm &= ~CSM(HWKANA_7BIT);

                /* try the current G0 charset */
                choices[choice_गिनती++] = cs = pFromU2022State->cs[0];
                csm &= ~CSM(cs);

                /* try the current G2 charset */
                अगर((cs = pFromU2022State->cs[2]) != 0) {
                    choices[choice_गिनती++] = cs;
                    csm &= ~CSM(cs);
                }

                /* try all the other possible charsets */
                क्रम(i = 0; i < UPRV_LENGTHOF(jpCharsetPref); ++i) {
                    cs = static_cast<int8_t>(jpCharsetPref[i]);
                    अगर(CSM(cs) & csm) {
                        choices[choice_गिनती++] = cs;
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

            क्रम(i = 0; i < choice_गिनती && len <= 0; ++i) {
                uint32_t value;
                int32_t len2;
                int8_t cs0 = choices[i];
                चयन(cs0) {
                हाल ASCII:
                    अगर(sourceChar <= 0x7f) {
                        target_मूल्य = static_cast<uint32_t>(sourceChar);
                        len = 1;
                        cs = cs0;
                        g = 0;
                    }
                    अवरोध;
                हाल ISO8859_1:
                    अगर(GR96_START <= sourceChar && sourceChar <= GR96_END) {
                        target_मूल्य = static_cast<uint32_t>(sourceChar) - 0x80;
                        len = 1;
                        cs = cs0;
                        g = 2;
                    }
                    अवरोध;
                हाल HWKANA_7BIT:
                    अगर (static_cast<uint32_t>(sourceChar - HWKANA_START) <= (HWKANA_END - HWKANA_START)) {
                        अगर(converter_तथ्य->version==3) {
                            /* JIS7: use G1 (SO) */
                            /* Shift U+FF61..U+FF9F to bytes 21..5F. */
                            target_मूल्य = static_cast<uint32_t>(sourceChar - (HWKANA_START - 0x21));
                            len = 1;
                            pFromU2022State->cs[1] = cs = cs0; /* do not output an escape sequence */
                            g = 1;
                        } अन्यथा अगर(converter_तथ्य->version==4) {
                            /* JIS8: use 8-bit bytes with any single-byte charset, see escape sequence output below */
                            /* Shift U+FF61..U+FF9F to bytes A1..DF. */
                            target_मूल्य = static_cast<uint32_t>(sourceChar - (HWKANA_START - 0xa1));
                            len = 1;

                            cs = pFromU2022State->cs[0];
                            अगर(IS_JP_DBCS(cs)) {
                                /* switch from a DBCS charset to JISX201 */
                                cs = static_cast<int8_t>(JISX201);
                            }
                            /* else stay in the current G0 charset */
                            g = 0;
                        }
                        /* else do not use HWKANA_7BIT with other versions */
                    }
                    अवरोध;
                हाल JISX201:
                    /* G0 SBCS */
                    value = jisx201FromU(sourceChar);
                    अगर(value <= 0x7f) {
                        target_मूल्य = value;
                        len = 1;
                        cs = cs0;
                        g = 0;
                        useFallback = false;
                    }
                    अवरोध;
                हाल JISX208:
                    /* G0 DBCS from Shift-JIS table */
                    len2 = MBCS_FROM_UCHAR32_ISO2022(
                                converter_तथ्य->myConverterArray[cs0],
                                sourceChar, &value,
                                useFallback, MBCS_OUTPUT_2);
                    अगर(len2 == 2 || (len2 == -2 && len == 0)) {  /* only accept DBCS: abs(len)==2 */
                        value = _2022FromSJIS(value);
                        अगर(value != 0) {
                            target_मूल्य = value;
                            len = len2;
                            cs = cs0;
                            g = 0;
                            useFallback = false;
                        }
                    } अन्यथा अगर(len == 0 && useFallback &&
                              static_cast<uint32_t>(sourceChar - HWKANA_START) <= (HWKANA_END - HWKANA_START)) {
                        target_मूल्य = hwkana_fb[sourceChar - HWKANA_START];
                        len = -2;
                        cs = cs0;
                        g = 0;
                        useFallback = false;
                    }
                    अवरोध;
                हाल ISO8859_7:
                    /* G0 SBCS forced to 7-bit output */
                    len2 = MBCS_SINGLE_FROM_UCHAR32(
                                converter_तथ्य->myConverterArray[cs0],
                                sourceChar, &value,
                                useFallback);
                    अगर(len2 != 0 && !(len2 < 0 && len != 0) && GR96_START <= value && value <= GR96_END) {
                        target_मूल्य = value - 0x80;
                        len = len2;
                        cs = cs0;
                        g = 2;
                        useFallback = false;
                    }
                    अवरोध;
                शेष:
                    /* G0 DBCS */
                    len2 = MBCS_FROM_UCHAR32_ISO2022(
                                converter_तथ्य->myConverterArray[cs0],
                                sourceChar, &value,
                                useFallback, MBCS_OUTPUT_2);
                    अगर(len2 == 2 || (len2 == -2 && len == 0)) {  /* only accept DBCS: abs(len)==2 */
                        अगर(cs0 == KSC5601) {
                            /*
                             * Check for valid bytes for the encoding scheme.
                             * This is necessary because the sub-converter (windows-949)
                             * has a broader encoding scheme than is valid for 2022.
                             */
                            value = _2022FromGR94DBCS(value);
                            अगर(value == 0) {
                                अवरोध;
                            }
                        }
                        target_मूल्य = value;
                        len = len2;
                        cs = cs0;
                        g = 0;
                        useFallback = false;
                    }
                    अवरोध;
                }
            }

            अगर(len != 0) {
                अगर(len < 0) {
                    len = -len;  /* fallback */
                }
                outLen = 0; /* count output bytes */

                /* write SI if necessary (only for JIS7) */
                अगर(pFromU2022State->g == 1 && g == 0) {
                    बफर[outLen++] = UCNV_SI;
                    pFromU2022State->g = 0;
                }

                /* write the designation sequence if necessary */
                अगर(cs != pFromU2022State->cs[g]) {
                    int32_t escLen = escSeqCharsLen[cs];
                    uprv_memcpy(बफर + outLen, escSeqChars[cs], escLen);
                    outLen += escLen;
                    pFromU2022State->cs[g] = cs;

                    /* invalidate the choices[] */
                    choice_गिनती = 0;
                }

                /* write the shift sequence if necessary */
                अगर(g != pFromU2022State->g) {
                    चयन(g) {
                    /* case 0 handled before writing escapes */
                    हाल 1:
                        बफर[outLen++] = UCNV_SO;
                        pFromU2022State->g = 1;
                        अवरोध;
                    शेष: /* case 2 */
                        बफर[outLen++] = 0x1b;
                        बफर[outLen++] = 0x4e;
                        अवरोध;
                    /* no case 3: no SS3 in ISO-2022-JP-x */
                    }
                }

                /* write the output bytes */
                अगर(len == 1) {
                    बफर[outLen++] = static_cast<अक्षर>(target_मूल्य);
                } अन्यथा /* len == 2 */ {
                    बफर[outLen++] = static_cast<अक्षर>(target_मूल्य >> 8);
                    बफर[outLen++] = static_cast<अक्षर>(target_मूल्य);
                }
            } अन्यथा {
                /*
                 * if we cannot find the character after checking all codepages
                 * then this is an error
                 */
                *err = U_INVALID_CHAR_FOUND;
                cnv->fromUChar32=sourceChar;
                अवरोध;
            }

            अगर(sourceChar == CR || sourceChar == LF) {
                /* reset the G2 state at the end of a line (conversion got us into ASCII or JISX201 already) */
                pFromU2022State->cs[2] = 0;
                choice_गिनती = 0;
            }

            /* output outLen>0 bytes in buffer[] */
            अगर(outLen == 1) {
                *target++ = बफर[0];
                अगर(offsets) {
                    *offsets++ = static_cast<int32_t>(source - args->source - 1); /* -1: known to be ASCII */
                }
            } अन्यथा अगर(outLen == 2 && (target + 2) <= targetLimit) {
                *target++ = बफर[0];
                *target++ = बफर[1];
                अगर(offsets) {
                    int32_t source_सूचकांक = static_cast<int32_t>(source - args->source - U16_लंबाई(sourceChar));
                    *offsets++ = source_सूचकांक;
                    *offsets++ = source_सूचकांक;
                }
            } अन्यथा {
                fromUWriteUInt8(
                    cnv,
                    बफर, outLen,
                    &target, reinterpret_cast<स्थिर अक्षर*>(targetLimit),
                    &offsets, static_cast<int32_t>(source - args->source - U16_लंबाई(sourceChar)),
                    err);
                अगर(U_FAILURE(*err)) {
                    अवरोध;
                }
            }
        } /* end if(myTargetIndex<myTargetLength) */
        अन्यथा{
            *err =U_बफर_OVERFLOW_ERROR;
            अवरोध;
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
    अगर( U_SUCCESS(*err) &&
        (pFromU2022State->g!=0 || pFromU2022State->cs[0]!=ASCII) &&
        args->सफाई && source>=sourceLimit && cnv->fromUChar32==0
    ) {
        int32_t source_सूचकांक;

        outLen = 0;

        अगर(pFromU2022State->g != 0) {
            बफर[outLen++] = UCNV_SI;
            pFromU2022State->g = 0;
        }

        अगर(pFromU2022State->cs[0] != ASCII) {
            int32_t escLen = escSeqCharsLen[ASCII];
            uprv_memcpy(बफर + outLen, escSeqChars[ASCII], escLen);
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
        source_सूचकांक = static_cast<int32_t>(source - args->source);
        अगर(source_सूचकांक>0) {
            --source_सूचकांक;
            अगर( U16_IS_TRAIL(args->source[source_सूचकांक]) &&
                (source_सूचकांक==0 || U16_IS_LEAD(args->source[source_सूचकांक-1]))
            ) {
                --source_सूचकांक;
            }
        } अन्यथा {
            source_सूचकांक=-1;
        }

        fromUWriteUInt8(
            cnv,
            बफर, outLen,
            &target, reinterpret_cast<स्थिर अक्षर*>(targetLimit),
            &offsets, source_सूचकांक,
            err);
    }

    /*save the state and return */
    args->source = source;
    args->target = reinterpret_cast<अक्षर*>(target);
}

/*************** to unicode *******************/

अटल व्योम U_CALLCONV
UConverter_toUnicode_ISO_2022_JP_OFFSETS_LOGIC(UConverterToUnicodeArgs *args,
                                               UErrorCode* err){
    अक्षर अस्थायी_Buf[2];
    स्थिर अक्षर* mySource = const_cast<अक्षर*>(args->source);
    char16_t *myTarget = args->target;
    स्थिर अक्षर *mySourceLimit = args->sourceLimit;
    uint32_t targetUniChar = 0x0000;
    uint32_t mySourceChar = 0x0000;
    uint32_t tmpSourceChar = 0x0000;
    UConverter_तथ्य_ISO2022* my_तथ्य;
    ISO2022State *pToU2022State;
    StateEnum cs;

    my_तथ्य = static_cast<UConverter_तथ्य_ISO2022*>(args->converter->extraInfo);
    pToU2022State = &my_तथ्य->toU2022State;

    अगर(my_तथ्य->कुंजी != 0) {
        /* continue with a partial escape sequence */
        जाओ escape;
    } अन्यथा अगर(args->converter->toULength == 1 && mySource < mySourceLimit && myTarget < args->targetLimit) {
        /* continue with a partial double-byte character */
        mySourceChar = args->converter->toUBytes[0];
        args->converter->toULength = 0;
        cs = static_cast<StateEnum>(pToU2022State->cs[pToU2022State->g]);
        targetUniChar = missingCharMarker;
        जाओ getTrailByte;
    }

    जबतक(mySource < mySourceLimit){

        targetUniChar =missingCharMarker;

        अगर(myTarget < args->targetLimit){

            mySourceChar = static_cast<अचिन्हित अक्षर>(*mySource++);

            चयन(mySourceChar) {
            हाल UCNV_SI:
                अगर(my_तथ्य->version==3) {
                    pToU2022State->g=0;
                    जारी;
                } अन्यथा {
                    /* only JIS7 uses SI/SO, not ISO-2022-JP-x */
                    my_तथ्य->isEmptySegment = false;	/* reset this, we have a different error */
                    अवरोध;
                }

            हाल UCNV_SO:
                अगर(my_तथ्य->version==3) {
                    /* JIS7: switch to G1 half-width Katakana */
                    pToU2022State->cs[1] = static_cast<int8_t>(HWKANA_7BIT);
                    pToU2022State->g=1;
                    जारी;
                } अन्यथा {
                    /* only JIS7 uses SI/SO, not ISO-2022-JP-x */
                    my_तथ्य->isEmptySegment = false;	/* reset this, we have a different error */
                    अवरोध;
                }

            हाल ESC_2022:
                mySource--;
escape:
                {
                    स्थिर अक्षर * mySourceBefore = mySource;
                    int8_t toULengthBefore = args->converter->toULength;

                    changeState_2022(args->converter,&(mySource),
                        mySourceLimit, ISO_2022_JP,err);

                    /* If in ISO-2022-JP only and we successfully completed an escape sequence, but previous segment was empty, create an error */
                    अगर(my_तथ्य->version==0 && my_तथ्य->कुंजी==0 && U_SUCCESS(*err) && my_तथ्य->isEmptySegment) {
                        *err = U_ILLEGAL_ESCAPE_SEQUENCE;
                        args->converter->toUCallbackReason = UCNV_IRREGULAR;
                        args->converter->toULength = static_cast<int8_t>(toULengthBefore + (mySource - mySourceBefore));
                    }
                }

                /* invalid or illegal escape sequence */
                अगर(U_FAILURE(*err)){
                    args->target = myTarget;
                    args->source = mySource;
                    my_तथ्य->isEmptySegment = false;	/* Reset to avoid future spurious errors */
                    वापस;
                }
                /* If we successfully completed an escape sequence, we begin a new segment, empty so far */
                अगर(my_तथ्य->कुंजी==0) {
                    my_तथ्य->isEmptySegment = true;
                }
                जारी;

            /* ISO-2022-JP does not use single-byte (C1) SS2 and SS3 */

            हाल CR:
            हाल LF:
                /* automatically reset to single-byte mode */
                अगर (static_cast<StateEnum>(pToU2022State->cs[0]) != ASCII &&
                    static_cast<StateEnum>(pToU2022State->cs[0]) != JISX201) {
                    pToU2022State->cs[0] = static_cast<int8_t>(ASCII);
                }
                pToU2022State->cs[2] = 0;
                pToU2022State->g = 0;
                U_FALLTHROUGH;
            शेष:
                /* convert one or two bytes */
                my_तथ्य->isEmptySegment = false;
                cs = static_cast<StateEnum>(pToU2022State->cs[pToU2022State->g]);
                अगर (static_cast<uint8_t>(mySourceChar - 0xa1) <= (0xdf - 0xa1) && my_तथ्य->version == 4 &&
                    !IS_JP_DBCS(cs)
                ) {
                    /* 8-bit halfwidth katakana in any single-byte mode for JIS8 */
                    targetUniChar = mySourceChar + (HWKANA_START - 0xa1);

                    /* return from a single-shift state to the previous one */
                    अगर(pToU2022State->g >= 2) {
                        pToU2022State->g=pToU2022State->पूर्व_G;
                    }
                } अन्यथा चयन(cs) {
                हाल ASCII:
                    अगर(mySourceChar <= 0x7f) {
                        targetUniChar = mySourceChar;
                    }
                    अवरोध;
                हाल ISO8859_1:
                    अगर(mySourceChar <= 0x7f) {
                        targetUniChar = mySourceChar + 0x80;
                    }
                    /* return from a single-shift state to the previous one */
                    pToU2022State->g=pToU2022State->पूर्व_G;
                    अवरोध;
                हाल ISO8859_7:
                    अगर(mySourceChar <= 0x7f) {
                        /* convert mySourceChar+0x80 to use a normal 8-bit table */
                        targetUniChar =
                            MBCS_SINGLE_SIMPLE_GET_अग्र_BMP(
                                my_तथ्य->myConverterArray[cs],
                                mySourceChar + 0x80);
                    }
                    /* return from a single-shift state to the previous one */
                    pToU2022State->g=pToU2022State->पूर्व_G;
                    अवरोध;
                हाल JISX201:
                    अगर(mySourceChar <= 0x7f) {
                        targetUniChar = jisx201ToU(mySourceChar);
                    }
                    अवरोध;
                हाल HWKANA_7BIT:
                    अगर (static_cast<uint8_t>(mySourceChar - 0x21) <= (0x5f - 0x21)) {
                        /* 7-bit halfwidth Katakana */
                        targetUniChar = mySourceChar + (HWKANA_START - 0x21);
                    }
                    अवरोध;
                शेष:
                    /* G0 DBCS */
                    अगर(mySource < mySourceLimit) {
                        पूर्णांक leadIsOk, trailIsOk;
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
                        अगर (leadIsOk && trailIsOk) {
                            ++mySource;
                            tmpSourceChar = (mySourceChar << 8) | trailByte;
                            अगर(cs == JISX208) {
                                _2022ToSJIS(static_cast<uint8_t>(mySourceChar), trailByte, अस्थायी_Buf);
                                mySourceChar = tmpSourceChar;
                            } अन्यथा {
                                /* Copy before we modify tmpSourceChar so toUnicodeCallback() sees the correct bytes. */
                                mySourceChar = tmpSourceChar;
                                अगर (cs == KSC5601) {
                                    tmpSourceChar += 0x8080;  /* = _2022ToGR94DBCS(tmpSourceChar) */
                                }
                                अस्थायी_Buf[0] = static_cast<अक्षर>(tmpSourceChar >> 8);
                                अस्थायी_Buf[1] = static_cast<अक्षर>(tmpSourceChar);
                            }
                            targetUniChar = ucnv_MBCSSimple_Get_अग्र_UChar(my_तथ्य->myConverterArray[cs], अस्थायी_Buf, 2, false);
                        } अन्यथा अगर (!(trailIsOk || IS_2022_CONTROL(trailByte))) {
                            /* report a pair of illegal bytes if the second byte is not a DBCS starter */
                            ++mySource;
                            /* add another bit so that the code below writes 2 bytes in case of error */
                            mySourceChar = 0x10000 | (mySourceChar << 8) | trailByte;
                        }
                    } अन्यथा {
                        args->converter->toUBytes[0] = static_cast<uint8_t>(mySourceChar);
                        args->converter->toULength = 1;
                        जाओ endloop;
                    }
                }  /* End of inner switch */
                अवरोध;
            }  /* End of outer switch */
            अगर(targetUniChar < (missingCharMarker-1/*0xfffe*/)){
                अगर(args->offsets){
                    args->offsets[myTarget - args->target] = static_cast<int32_t>(mySource - args->source - (mySourceChar <= 0xff ? 1 : 2));
                }
                *(myTarget++) = static_cast<char16_t>(targetUniChar);
            }
            अन्यथा अगर(targetUniChar > missingCharMarker){
                /* disassemble the surrogate pair and write to output*/
                targetUniChar-=0x0010000;
                *myTarget = static_cast<char16_t>(0xd800 + static_cast<char16_t>(targetUniChar >> 10));
                अगर(args->offsets){
                    args->offsets[myTarget - args->target] = static_cast<int32_t>(mySource - args->source - (mySourceChar <= 0xff ? 1 : 2));
                }
                ++myTarget;
                अगर(myTarget< args->targetLimit){
                    *myTarget = static_cast<char16_t>(0xdc00 + static_cast<char16_t>(targetUniChar & 0x3ff));
                    अगर(args->offsets){
                        args->offsets[myTarget - args->target] = static_cast<int32_t>(mySource - args->source - (mySourceChar <= 0xff ? 1 : 2));
                    }
                    ++myTarget;
                }अन्यथा{
                    args->converter->UChar_Error_बफर[args->converter->UChar_Error_बफर_लंबाई++]=
                                    static_cast<char16_t>(0xdc00 + static_cast<char16_t>(targetUniChar & 0x3ff));
                }

            }
            अन्यथा{
                /* Call the callback function*/
                toUnicodeCallback(args->converter,mySourceChar,targetUniChar,err);
                अवरोध;
            }
        }
        अन्यथा{    /* goes with "if(myTarget < args->targetLimit)"  way up near top of function */
            *err =U_बफर_OVERFLOW_ERROR;
            अवरोध;
        }
    }
endloop:
    args->target = myTarget;
    args->source = mySource;
}


#अगर !UCONFIG_ONLY_HTML_CONVERSION
/***************************************************************
*   Rules for ISO-2022-KR encoding
*   i) The KSC5601 designator sequence should appear only once in a file,
*      at the beginning of a line before any KSC5601 characters. This usually
*      means that it appears by itself on the first line of the file
*  ii) There are only 2 shifting sequences SO to shift into double byte mode
*      and SI to shift into single byte mode
*/
अटल व्योम U_CALLCONV
UConverter_fromUnicode_ISO_2022_KR_OFFSETS_LOGIC_IBM(UConverterFromUnicodeArgs* args, UErrorCode* err){

    UConverter* saveConv = args->converter;
    UConverter_तथ्य_ISO2022* my_Converter_तथ्य = static_cast<UConverter_तथ्य_ISO2022*>(saveConv->extraInfo);
    args->converter=my_Converter_तथ्य->currentConverter;

    my_Converter_तथ्य->currentConverter->fromUChar32 = saveConv->fromUChar32;
    ucnv_MBCSFromUnicodeWithOffsets(args,err);
    saveConv->fromUChar32 = my_Converter_तथ्य->currentConverter->fromUChar32;

    अगर(*err == U_बफर_OVERFLOW_ERROR) {
        अगर(my_Converter_तथ्य->currentConverter->char_Error_बफर_लंबाई > 0) {
            uprv_memcpy(
                saveConv->char_Error_बफर,
                my_Converter_तथ्य->currentConverter->char_Error_बफर,
                my_Converter_तथ्य->currentConverter->char_Error_बफर_लंबाई);
        }
        saveConv->char_Error_बफर_लंबाई = my_Converter_तथ्य->currentConverter->char_Error_बफर_लंबाई;
        my_Converter_तथ्य->currentConverter->char_Error_बफर_लंबाई = 0;
    }
    args->converter=saveConv;
}

अटल व्योम U_CALLCONV
UConverter_fromUnicode_ISO_2022_KR_OFFSETS_LOGIC(UConverterFromUnicodeArgs* args, UErrorCode* err){

    स्थिर char16_t *source = args->source;
    स्थिर char16_t *sourceLimit = args->sourceLimit;
    अचिन्हित अक्षर *target = reinterpret_cast<अचिन्हित अक्षर*>(args->target);
    अचिन्हित अक्षर *targetLimit = reinterpret_cast<अचिन्हित अक्षर*>(const_cast<अक्षर*>(args->targetLimit));
    int32_t* offsets = args->offsets;
    uint32_t targetByteUnit = 0x0000;
    UChar32 sourceChar = 0x0000;
    UBool isTargetByteDBCS;
    UBool oldIsTargetByteDBCS;
    UConverter_तथ्य_ISO2022 *converter_तथ्य;
    UConverter_Shared_तथ्य* shared_तथ्य;
    UBool useFallback;
    int32_t length =0;

    converter_तथ्य = static_cast<UConverter_तथ्य_ISO2022*>(args->converter->extraInfo);
    /* if the version is 1 then the user is requesting
     * conversion with ibm-25546 pass the arguments to
     * MBCS converter and return
     */
    अगर(converter_तथ्य->version==1){
        UConverter_fromUnicode_ISO_2022_KR_OFFSETS_LOGIC_IBM(args,err);
        वापस;
    }

    /* initialize data */
    shared_तथ्य = converter_तथ्य->currentConverter->shared_तथ्य;
    useFallback = args->converter->useFallback;
    isTargetByteDBCS = static_cast<UBool>(args->converter->fromUnicodeStatus);
    oldIsTargetByteDBCS = isTargetByteDBCS;

    isTargetByteDBCS = static_cast<UBool>(args->converter->fromUnicodeStatus);
    अगर((sourceChar = args->converter->fromUChar32)!=0 && target <targetLimit) {
        जाओ getTrail;
    }
    जबतक(source < sourceLimit){

        targetByteUnit = missingCharMarker;

        अगर(target < (अचिन्हित अक्षर*) args->targetLimit){
            sourceChar = *source++;

            /* do not convert SO/SI/ESC */
            अगर(IS_2022_CONTROL(sourceChar)) {
                /* callback(illegal) */
                *err=U_ILLEGAL_CHAR_FOUND;
                args->converter->fromUChar32=sourceChar;
                अवरोध;
            }

            length = MBCS_FROM_UCHAR32_ISO2022(shared_तथ्य,sourceChar,&targetByteUnit,useFallback,MBCS_OUTPUT_2);
            अगर(length < 0) {
                length = -length;  /* fallback */
            }
            /* only DBCS or SBCS characters are expected*/
            /* DB characters with high bit set to 1 are expected */
            अगर( length > 2 || length==0 ||
                (length == 1 && targetByteUnit > 0x7f) ||
                (length == 2 &&
                    (static_cast<uint16_t>(targetByteUnit - 0xa1a1) > (0xfefe - 0xa1a1) ||
                    static_cast<uint8_t>(targetByteUnit - 0xa1) > (0xfe - 0xa1)))
            ) {
                targetByteUnit=missingCharMarker;
            }
            अगर (targetByteUnit != missingCharMarker){

                oldIsTargetByteDBCS = isTargetByteDBCS;
                isTargetByteDBCS = static_cast<UBool>(targetByteUnit > 0x00FF);
                  /* append the shift sequence */
                अगर (oldIsTargetByteDBCS != isTargetByteDBCS ){

                    अगर (isTargetByteDBCS)
                        *target++ = UCNV_SO;
                    अन्यथा
                        *target++ = UCNV_SI;
                    अगर(offsets)
                        *(offsets++) = static_cast<int32_t>(source - args->source - 1);
                }
                /* write the targetUniChar  to target */
                अगर(targetByteUnit <= 0x00FF){
                    अगर( target < targetLimit){
                        *(target++) = static_cast<अचिन्हित अक्षर>(targetByteUnit);
                        अगर(offsets){
                            *(offsets++) = static_cast<int32_t>(source - args->source - 1);
                        }

                    }अन्यथा{
                        args->converter->char_Error_बफर[args->converter->char_Error_बफर_लंबाई++] = static_cast<अचिन्हित अक्षर>(targetByteUnit);
                        *err = U_बफर_OVERFLOW_ERROR;
                    }
                }अन्यथा{
                    अगर(target < targetLimit){
                        *(target++) = static_cast<अचिन्हित अक्षर>((targetByteUnit >> 8) - 0x80);
                        अगर(offsets){
                            *(offsets++) = static_cast<int32_t>(source - args->source - 1);
                        }
                        अगर(target < targetLimit){
                            *(target++) = static_cast<अचिन्हित अक्षर>(targetByteUnit - 0x80);
                            अगर(offsets){
                                *(offsets++) = static_cast<int32_t>(source - args->source - 1);
                            }
                        }अन्यथा{
                            args->converter->char_Error_बफर[args->converter->char_Error_बफर_लंबाई++] = static_cast<अचिन्हित अक्षर>(targetByteUnit - 0x80);
                            *err = U_बफर_OVERFLOW_ERROR;
                        }
                    }अन्यथा{
                        args->converter->char_Error_बफर[args->converter->char_Error_बफर_लंबाई++] = static_cast<अचिन्हित अक्षर>((targetByteUnit >> 8) - 0x80);
                        args->converter->char_Error_बफर[args->converter->char_Error_बफर_लंबाई++] = static_cast<अचिन्हित अक्षर>(targetByteUnit - 0x80);
                        *err = U_बफर_OVERFLOW_ERROR;
                    }
                }

            }
            अन्यथा{
                /* oops.. the code point is unassingned
                 * set the error and reason
                 */

                /*check if the char is a First surrogate*/
                अगर(U16_IS_SURROGATE(sourceChar)) {
                    अगर(U16_IS_SURROGATE_LEAD(sourceChar)) {
getTrail:
                        /*look ahead to find the trail surrogate*/
                        अगर(source <  sourceLimit) {
                            /* test the following code unit */
                            char16_t trail = *source;
                            अगर(U16_IS_TRAIL(trail)) {
                                source++;
                                sourceChar=U16_GET_SUPPLEMENTARY(sourceChar, trail);
                                *err = U_INVALID_CHAR_FOUND;
                                /* convert this surrogate code point */
                                /* exit this condition tree */
                            } अन्यथा {
                                /* this is an unmatched lead code unit (1st surrogate) */
                                /* callback(illegal) */
                                *err=U_ILLEGAL_CHAR_FOUND;
                            }
                        } अन्यथा {
                            /* no more input */
                            *err = U_ZERO_ERROR;
                        }
                    } अन्यथा {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *err=U_ILLEGAL_CHAR_FOUND;
                    }
                } अन्यथा {
                    /* callback(unassigned) for a BMP code point */
                    *err = U_INVALID_CHAR_FOUND;
                }

                args->converter->fromUChar32=sourceChar;
                अवरोध;
            }
        } /* end if(myTargetIndex<myTargetLength) */
        अन्यथा{
            *err =U_बफर_OVERFLOW_ERROR;
            अवरोध;
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
    अगर( U_SUCCESS(*err) &&
        isTargetByteDBCS &&
        args->सफाई && source>=sourceLimit && args->converter->fromUChar32==0
    ) {
        int32_t source_सूचकांक;

        /* we are switching to ASCII */
        isTargetByteDBCS=false;

        /* get the source index of the last input character */
        /*
         * TODO this would be simpler and more reliable if we used a pair
         * of sourceIndex/prevSourceIndex like in ucnvmbcs.c
         * so that we could simply use the prevSourceIndex here;
         * this code gives an incorrect result for the rare case of an unmatched
         * trail surrogate that is alone in the last buffer of the text stream
         */
        source_सूचकांक = static_cast<int32_t>(source - args->source);
        अगर(source_सूचकांक>0) {
            --source_सूचकांक;
            अगर( U16_IS_TRAIL(args->source[source_सूचकांक]) &&
                (source_सूचकांक==0 || U16_IS_LEAD(args->source[source_सूचकांक-1]))
            ) {
                --source_सूचकांक;
            }
        } अन्यथा {
            source_सूचकांक=-1;
        }

        fromUWriteUInt8(
            args->converter,
            SHIFT_IN_STR, 1,
            &target, reinterpret_cast<स्थिर अक्षर*>(targetLimit),
            &offsets, source_सूचकांक,
            err);
    }

    /*save the state and return */
    args->source = source;
    args->target = reinterpret_cast<अक्षर*>(target);
    args->converter->fromUnicodeStatus = static_cast<uint32_t>(isTargetByteDBCS);
}

/************************ To Unicode ***************************************/

अटल व्योम U_CALLCONV
UConverter_toUnicode_ISO_2022_KR_OFFSETS_LOGIC_IBM(UConverterToUnicodeArgs *args,
                                                            UErrorCode* err){
    अक्षर स्थिर* sourceStart;
    UConverter_तथ्य_ISO2022* my_तथ्य = static_cast<UConverter_तथ्य_ISO2022*>(args->converter->extraInfo);

    UConverterToUnicodeArgs subArgs;
    int32_t min_Args_आकार;

    /* set up the subconverter arguments */
    अगर(args->size<माप(UConverterToUnicodeArgs)) {
        min_Args_आकार = args->size;
    } अन्यथा {
        min_Args_आकार = static_cast<int32_t>(माप(UConverterToUnicodeArgs));
    }

    uprv_memcpy(&subArgs, args, min_Args_आकार);
    subArgs.size = static_cast<uint16_t>(min_Args_आकार);
    subArgs.converter = my_तथ्य->currentConverter;

    /* remember the original start of the input for offsets */
    sourceStart = args->source;

    अगर(my_तथ्य->कुंजी != 0) {
        /* continue with a partial escape sequence */
        जाओ escape;
    }

    जबतक(U_SUCCESS(*err) && args->source < args->sourceLimit) {
        /*Find the end of the buffer e.g : Next Escape Seq | end of Buffer*/
        subArgs.source = args->source;
        subArgs.sourceLimit = get_End_Of_बफर_2022(&(args->source), args->sourceLimit, args->सफाई);
        अगर(subArgs.source != subArgs.sourceLimit) {
            /*
             * get the current partial byte sequence
             *
             * it needs to be moved between the public and the subconverter
             * so that the conversion framework, which only sees the public
             * converter, can handle truncated and illegal input etc.
             */
            अगर(args->converter->toULength > 0) {
                uprv_memcpy(subArgs.converter->toUBytes, args->converter->toUBytes, args->converter->toULength);
            }
            subArgs.converter->toULength = args->converter->toULength;

            /*
             * Convert up to the end of the input, or to before the next escape character.
             * Does not handle conversion extensions because the preToU[] state etc.
             * is not copied.
             */
            ucnv_MBCSToUnicodeWithOffsets(&subArgs, err);

            अगर(args->offsets != nullptr && sourceStart != args->source) {
                /* update offsets to base them on the actual start of the input */
                int32_t *offsets = args->offsets;
                char16_t *target = args->target;
                int32_t delta = static_cast<int32_t>(args->source - sourceStart);
                जबतक(target < subArgs.target) {
                    अगर(*offsets >= 0) {
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
            अगर(subArgs.converter->toULength > 0) {
                uprv_memcpy(args->converter->toUBytes, subArgs.converter->toUBytes, subArgs.converter->toULength);
            }
            args->converter->toULength = subArgs.converter->toULength;

            अगर(*err == U_बफर_OVERFLOW_ERROR) {
                अगर(subArgs.converter->UChar_Error_बफर_लंबाई > 0) {
                    uprv_memcpy(args->converter->UChar_Error_बफर, subArgs.converter->UChar_Error_बफर,
                                subArgs.converter->UChar_Error_बफर_लंबाई);
                }
                args->converter->UChar_Error_बफर_लंबाई=subArgs.converter->UChar_Error_बफर_लंबाई;
                subArgs.converter->UChar_Error_बफर_लंबाई = 0;
            }
        }

        अगर (U_FAILURE(*err) || (args->source == args->sourceLimit)) {
            वापस;
        }

escape:
        changeState_2022(args->converter,
               &(args->source),
               args->sourceLimit,
               ISO_2022_KR,
               err);
    }
}

अटल व्योम U_CALLCONV
UConverter_toUnicode_ISO_2022_KR_OFFSETS_LOGIC(UConverterToUnicodeArgs *args,
                                                            UErrorCode* err){
    अक्षर अस्थायी_Buf[2];
    स्थिर अक्षर* mySource = const_cast<अक्षर*>(args->source);
    char16_t *myTarget = args->target;
    स्थिर अक्षर *mySourceLimit = args->sourceLimit;
    UChar32 targetUniChar = 0x0000;
    char16_t mySourceChar = 0x0000;
    UConverter_तथ्य_ISO2022* my_तथ्य;
    UConverter_Shared_तथ्य* shared_तथ्य ;
    UBool useFallback;

    my_तथ्य = static_cast<UConverter_तथ्य_ISO2022*>(args->converter->extraInfo);
    अगर(my_तथ्य->version==1){
        UConverter_toUnicode_ISO_2022_KR_OFFSETS_LOGIC_IBM(args,err);
        वापस;
    }

    /* initialize state */
    shared_तथ्य = my_तथ्य->currentConverter->shared_तथ्य;
    useFallback = args->converter->useFallback;

    अगर(my_तथ्य->कुंजी != 0) {
        /* continue with a partial escape sequence */
        जाओ escape;
    } अन्यथा अगर(args->converter->toULength == 1 && mySource < mySourceLimit && myTarget < args->targetLimit) {
        /* continue with a partial double-byte character */
        mySourceChar = args->converter->toUBytes[0];
        args->converter->toULength = 0;
        जाओ getTrailByte;
    }

    जबतक(mySource< mySourceLimit){

        अगर(myTarget < args->targetLimit){

            mySourceChar = static_cast<अचिन्हित अक्षर>(*mySource++);

            अगर(mySourceChar==UCNV_SI){
                my_तथ्य->toU2022State.g = 0;
                अगर (my_तथ्य->isEmptySegment) {
                    my_तथ्य->isEmptySegment = false;	/* we are handling it, reset to avoid future spurious errors */
                    *err = U_ILLEGAL_ESCAPE_SEQUENCE;
                    args->converter->toUCallbackReason = UCNV_IRREGULAR;
                    args->converter->toUBytes[0] = static_cast<uint8_t>(mySourceChar);
                    args->converter->toULength = 1;
                    args->target = myTarget;
                    args->source = mySource;
                    वापस;
                }
                /*consume the source */
                जारी;
            }अन्यथा अगर(mySourceChar==UCNV_SO){
                my_तथ्य->toU2022State.g = 1;
                my_तथ्य->isEmptySegment = true;	/* Begin a new segment, empty so far */
                /*consume the source */
                जारी;
            }अन्यथा अगर(mySourceChar==ESC_2022){
                mySource--;
escape:
                my_तथ्य->isEmptySegment = false;	/* Any invalid ESC sequences will be detected separately, so just reset this */
                changeState_2022(args->converter,&(mySource),
                                mySourceLimit, ISO_2022_KR, err);
                अगर(U_FAILURE(*err)){
                    args->target = myTarget;
                    args->source = mySource;
                    वापस;
                }
                जारी;
            }

            my_तथ्य->isEmptySegment = false;	/* Any invalid char errors will be detected separately, so just reset this */
            अगर(my_तथ्य->toU2022State.g == 1) {
                अगर(mySource < mySourceLimit) {
                    पूर्णांक leadIsOk, trailIsOk;
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
                    अगर (leadIsOk && trailIsOk) {
                        ++mySource;
                        अस्थायी_Buf[0] = static_cast<अक्षर>(mySourceChar + 0x80);
                        अस्थायी_Buf[1] = static_cast<अक्षर>(trailByte + 0x80);
                        targetUniChar = ucnv_MBCSSimple_Get_अग्र_UChar(shared_तथ्य, अस्थायी_Buf, 2, useFallback);
                        mySourceChar = (mySourceChar << 8) | trailByte;
                    } अन्यथा अगर (!(trailIsOk || IS_2022_CONTROL(trailByte))) {
                        /* report a pair of illegal bytes if the second byte is not a DBCS starter */
                        ++mySource;
                        /* add another bit so that the code below writes 2 bytes in case of error */
                        mySourceChar = static_cast<char16_t>(0x10000 | (mySourceChar << 8) | trailByte);
                    }
                } अन्यथा {
                    args->converter->toUBytes[0] = static_cast<uint8_t>(mySourceChar);
                    args->converter->toULength = 1;
                    अवरोध;
                }
            }
            अन्यथा अगर(mySourceChar <= 0x7f) {
                targetUniChar = ucnv_MBCSSimple_Get_अग्र_UChar(shared_तथ्य, mySource - 1, 1, useFallback);
            } अन्यथा {
                targetUniChar = 0xffff;
            }
            अगर(targetUniChar < 0xfffe){
                अगर(args->offsets) {
                    args->offsets[myTarget - args->target] = static_cast<int32_t>(mySource - args->source - (mySourceChar <= 0xff ? 1 : 2));
                }
                *(myTarget++) = static_cast<char16_t>(targetUniChar);
            }
            अन्यथा {
                /* Call the callback function*/
                toUnicodeCallback(args->converter,mySourceChar,targetUniChar,err);
                अवरोध;
            }
        }
        अन्यथा{
            *err =U_बफर_OVERFLOW_ERROR;
            अवरोध;
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
अटल स्थिर अक्षर GB_2312_80_STR[] = "\x1B\x24\x29\x41";
अटल स्थिर अक्षर ISO_IR_165_STR[] = "\x1B\x24\x29\x45";
अटल स्थिर अक्षर CNS_11643_1992_Plane_1_STR[] = "\x1B\x24\x29\x47";
अटल स्थिर अक्षर CNS_11643_1992_Plane_2_STR[] = "\x1B\x24\x2A\x48";
अटल स्थिर अक्षर CNS_11643_1992_Plane_3_STR[] = "\x1B\x24\x2B\x49";
अटल स्थिर अक्षर CNS_11643_1992_Plane_4_STR[] = "\x1B\x24\x2B\x4A";
अटल स्थिर अक्षर CNS_11643_1992_Plane_5_STR[] = "\x1B\x24\x2B\x4B";
अटल स्थिर अक्षर CNS_11643_1992_Plane_6_STR[] = "\x1B\x24\x2B\x4C";
अटल स्थिर अक्षर CNS_11643_1992_Plane_7_STR[] = "\x1B\x24\x2B\x4D";

/********************** ISO2022-CN Data **************************/
अटल स्थिर अक्षर* स्थिर escSeqCharsCN[10] ={
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

अटल व्योम U_CALLCONV
UConverter_fromUnicode_ISO_2022_CN_OFFSETS_LOGIC(UConverterFromUnicodeArgs* args, UErrorCode* err){
    UConverter *cnv = args->converter;
    UConverter_तथ्य_ISO2022 *converter_तथ्य;
    ISO2022State *pFromU2022State;
    uint8_t* target = reinterpret_cast<uint8_t*>(args->target);
    स्थिर uint8_t* targetLimit = reinterpret_cast<स्थिर uint8_t*>(args->targetLimit);
    स्थिर char16_t* source = args->source;
    स्थिर char16_t* sourceLimit = args->sourceLimit;
    int32_t* offsets = args->offsets;
    UChar32 sourceChar;
    अक्षर बफर[8];
    int32_t len;
    int8_t choices[3];
    int32_t choice_गिनती;
    uint32_t target_मूल्य = 0;
    UBool useFallback;

    /* set up the state */
    converter_तथ्य = static_cast<UConverter_तथ्य_ISO2022*>(cnv->extraInfo);
    pFromU2022State   = &converter_तथ्य->fromU2022State;

    choice_गिनती = 0;

    /* check if the last codepoint of previous buffer was a lead surrogate*/
    अगर((sourceChar = cnv->fromUChar32)!=0 && target< targetLimit) {
        जाओ getTrail;
    }

    जबतक( source < sourceLimit){
        अगर(target < targetLimit){

            sourceChar  = *(source++);
            /*check if the char is a First surrogate*/
             अगर(U16_IS_SURROGATE(sourceChar)) {
                अगर(U16_IS_SURROGATE_LEAD(sourceChar)) {
getTrail:
                    /*look ahead to find the trail surrogate*/
                    अगर(source < sourceLimit) {
                        /* test the following code unit */
                        char16_t trail = *source;
                        अगर(U16_IS_TRAIL(trail)) {
                            source++;
                            sourceChar=U16_GET_SUPPLEMENTARY(sourceChar, trail);
                            cnv->fromUChar32=0x00;
                            /* convert this supplementary code point */
                            /* exit this condition tree */
                        } अन्यथा {
                            /* this is an unmatched lead code unit (1st surrogate) */
                            /* callback(illegal) */
                            *err=U_ILLEGAL_CHAR_FOUND;
                            cnv->fromUChar32=sourceChar;
                            अवरोध;
                        }
                    } अन्यथा {
                        /* no more input */
                        cnv->fromUChar32=sourceChar;
                        अवरोध;
                    }
                } अन्यथा {
                    /* this is an unmatched trail code unit (2nd surrogate) */
                    /* callback(illegal) */
                    *err=U_ILLEGAL_CHAR_FOUND;
                    cnv->fromUChar32=sourceChar;
                    अवरोध;
                }
            }

            /* do the conversion */
            अगर(sourceChar <= 0x007f ){
                /* do not convert SO/SI/ESC */
                अगर(IS_2022_CONTROL(sourceChar)) {
                    /* callback(illegal) */
                    *err=U_ILLEGAL_CHAR_FOUND;
                    cnv->fromUChar32=sourceChar;
                    अवरोध;
                }

                /* US-ASCII */
                अगर(pFromU2022State->g == 0) {
                    बफर[0] = static_cast<अक्षर>(sourceChar);
                    len = 1;
                } अन्यथा {
                    बफर[0] = UCNV_SI;
                    बफर[1] = static_cast<अक्षर>(sourceChar);
                    len = 2;
                    pFromU2022State->g = 0;
                    choice_गिनती = 0;
                }
                अगर(sourceChar == CR || sourceChar == LF) {
                    /* reset the state at the end of a line */
                    uprv_memset(pFromU2022State, 0, माप(ISO2022State));
                    choice_गिनती = 0;
                }
            }
            अन्यथा{
                /* convert U+0080..U+10ffff */
                int32_t i;
                int8_t cs, g;

                अगर(choice_गिनती == 0) {
                    /* try the current SO/G1 converter first */
                    choices[0] = pFromU2022State->cs[1];

                    /* default to GB2312_1 if none is designated yet */
                    अगर(choices[0] == 0) {
                        choices[0] = GB2312_1;
                    }

                    अगर(converter_तथ्य->version == 0) {
                        /* ISO-2022-CN */

                        /* try the other SO/G1 converter; a CNS_11643_1 lookup may result in any plane */
                        अगर(choices[0] == GB2312_1) {
                            choices[1] = static_cast<int8_t>(CNS_11643_1);
                        } अन्यथा {
                            choices[1] = static_cast<int8_t>(GB2312_1);
                        }

                        choice_गिनती = 2;
                    } अन्यथा अगर (converter_तथ्य->version == 1) {
                        /* ISO-2022-CN-EXT */

                        /* try one of the other converters */
                        चयन(choices[0]) {
                        हाल GB2312_1:
                            choices[1] = static_cast<int8_t>(CNS_11643_1);
                            choices[2] = static_cast<int8_t>(ISO_IR_165);
                            अवरोध;
                        हाल ISO_IR_165:
                            choices[1] = static_cast<int8_t>(GB2312_1);
                            choices[2] = static_cast<int8_t>(CNS_11643_1);
                            अवरोध;
                        शेष: /* CNS_11643_x */
                            choices[1] = static_cast<int8_t>(GB2312_1);
                            choices[2] = static_cast<int8_t>(ISO_IR_165);
                            अवरोध;
                        }

                        choice_गिनती = 3;
                    } अन्यथा {
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

                क्रम(i = 0; i < choice_गिनती && len <= 0; ++i) {
                    int8_t cs0 = choices[i];
                    अगर(cs0 > 0) {
                        uint32_t value;
                        int32_t len2;
                        अगर(cs0 >= CNS_11643_0) {
                            len2 = MBCS_FROM_UCHAR32_ISO2022(
                                        converter_तथ्य->myConverterArray[CNS_11643],
                                        sourceChar,
                                        &value,
                                        useFallback,
                                        MBCS_OUTPUT_3);
                            अगर(len2 == 3 || (len2 == -3 && len == 0)) {
                                target_मूल्य = value;
                                cs = static_cast<int8_t>(CNS_11643_0 + (value >> 16) - 0x80);
                                अगर(len2 >= 0) {
                                    len = 2;
                                } अन्यथा {
                                    len = -2;
                                    useFallback = false;
                                }
                                अगर(cs == CNS_11643_1) {
                                    g = 1;
                                } अन्यथा अगर(cs == CNS_11643_2) {
                                    g = 2;
                                } अन्यथा /* plane 3..7 */ अगर(converter_तथ्य->version == 1) {
                                    g = 3;
                                } अन्यथा {
                                    /* ISO-2022-CN (without -EXT) does not support plane 3..7 */
                                    len = 0;
                                }
                            }
                        } अन्यथा {
                            /* GB2312_1 or ISO-IR-165 */
                            U_ASSERT(cs0<UCNV_2022_MAX_CONVERTERS);
                            len2 = MBCS_FROM_UCHAR32_ISO2022(
                                        converter_तथ्य->myConverterArray[cs0],
                                        sourceChar,
                                        &value,
                                        useFallback,
                                        MBCS_OUTPUT_2);
                            अगर(len2 == 2 || (len2 == -2 && len == 0)) {
                                target_मूल्य = value;
                                len = len2;
                                cs = cs0;
                                g = 1;
                                useFallback = false;
                            }
                        }
                    }
                }

                अगर(len != 0) {
                    len = 0; /* count output bytes; it must have been abs(len) == 2 */

                    /* write the designation sequence if necessary */
                    अगर(cs != pFromU2022State->cs[g]) {
                        अगर(cs < CNS_11643) {
                            uprv_memcpy(बफर, escSeqCharsCN[cs], 4);
                        } अन्यथा {
                            U_ASSERT(cs >= CNS_11643_1);
                            uprv_memcpy(बफर, escSeqCharsCN[CNS_11643 + (cs - CNS_11643_1)], 4);
                        }
                        len = 4;
                        pFromU2022State->cs[g] = cs;
                        अगर(g == 1) {
                            /* changing the SO/G1 charset invalidates the choices[] */
                            choice_गिनती = 0;
                        }
                    }

                    /* write the shift sequence if necessary */
                    अगर(g != pFromU2022State->g) {
                        चयन(g) {
                        हाल 1:
                            बफर[len++] = UCNV_SO;

                            /* set the new state only if it is the locking shift SO/G1, not for SS2 or SS3 */
                            pFromU2022State->g = 1;
                            अवरोध;
                        हाल 2:
                            बफर[len++] = 0x1b;
                            बफर[len++] = 0x4e;
                            अवरोध;
                        शेष: /* case 3 */
                            बफर[len++] = 0x1b;
                            बफर[len++] = 0x4f;
                            अवरोध;
                        }
                    }

                    /* write the two output bytes */
                    बफर[len++] = static_cast<अक्षर>(target_मूल्य >> 8);
                    बफर[len++] = static_cast<अक्षर>(target_मूल्य);
                } अन्यथा {
                    /* if we cannot find the character after checking all codepages
                     * then this is an error
                     */
                    *err = U_INVALID_CHAR_FOUND;
                    cnv->fromUChar32=sourceChar;
                    अवरोध;
                }
            }

            /* output len>0 bytes in buffer[] */
            अगर(len == 1) {
                *target++ = बफर[0];
                अगर(offsets) {
                    *offsets++ = static_cast<int32_t>(source - args->source - 1); /* -1: known to be ASCII */
                }
            } अन्यथा अगर(len == 2 && (target + 2) <= targetLimit) {
                *target++ = बफर[0];
                *target++ = बफर[1];
                अगर(offsets) {
                    int32_t source_सूचकांक = static_cast<int32_t>(source - args->source - U16_लंबाई(sourceChar));
                    *offsets++ = source_सूचकांक;
                    *offsets++ = source_सूचकांक;
                }
            } अन्यथा {
                fromUWriteUInt8(
                    cnv,
                    बफर, len,
                    &target, reinterpret_cast<स्थिर अक्षर*>(targetLimit),
                    &offsets, static_cast<int32_t>(source - args->source - U16_लंबाई(sourceChar)),
                    err);
                अगर(U_FAILURE(*err)) {
                    अवरोध;
                }
            }
        } /* end if(myTargetIndex<myTargetLength) */
        अन्यथा{
            *err =U_बफर_OVERFLOW_ERROR;
            अवरोध;
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
    अगर( U_SUCCESS(*err) &&
        pFromU2022State->g!=0 &&
        args->सफाई && source>=sourceLimit && cnv->fromUChar32==0
    ) {
        int32_t source_सूचकांक;

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
        source_सूचकांक = static_cast<int32_t>(source - args->source);
        अगर(source_सूचकांक>0) {
            --source_सूचकांक;
            अगर( U16_IS_TRAIL(args->source[source_सूचकांक]) &&
                (source_सूचकांक==0 || U16_IS_LEAD(args->source[source_सूचकांक-1]))
            ) {
                --source_सूचकांक;
            }
        } अन्यथा {
            source_सूचकांक=-1;
        }

        fromUWriteUInt8(
            cnv,
            SHIFT_IN_STR, 1,
            &target, reinterpret_cast<स्थिर अक्षर*>(targetLimit),
            &offsets, source_सूचकांक,
            err);
    }

    /*save the state and return */
    args->source = source;
    args->target = reinterpret_cast<अक्षर*>(target);
}


अटल व्योम U_CALLCONV
UConverter_toUnicode_ISO_2022_CN_OFFSETS_LOGIC(UConverterToUnicodeArgs *args,
                                               UErrorCode* err){
    अक्षर अस्थायी_Buf[3];
    स्थिर अक्षर* mySource = const_cast<अक्षर*>(args->source);
    char16_t *myTarget = args->target;
    स्थिर अक्षर *mySourceLimit = args->sourceLimit;
    uint32_t targetUniChar = 0x0000;
    uint32_t mySourceChar = 0x0000;
    UConverter_तथ्य_ISO2022* my_तथ्य;
    ISO2022State *pToU2022State;

    my_तथ्य = static_cast<UConverter_तथ्य_ISO2022*>(args->converter->extraInfo);
    pToU2022State = &my_तथ्य->toU2022State;

    अगर(my_तथ्य->कुंजी != 0) {
        /* continue with a partial escape sequence */
        जाओ escape;
    } अन्यथा अगर(args->converter->toULength == 1 && mySource < mySourceLimit && myTarget < args->targetLimit) {
        /* continue with a partial double-byte character */
        mySourceChar = args->converter->toUBytes[0];
        args->converter->toULength = 0;
        targetUniChar = missingCharMarker;
        जाओ getTrailByte;
    }

    जबतक(mySource < mySourceLimit){

        targetUniChar =missingCharMarker;

        अगर(myTarget < args->targetLimit){

            mySourceChar = static_cast<अचिन्हित अक्षर>(*mySource++);

            चयन(mySourceChar){
            हाल UCNV_SI:
                pToU2022State->g=0;
                अगर (my_तथ्य->isEmptySegment) {
                    my_तथ्य->isEmptySegment = false;	/* we are handling it, reset to avoid future spurious errors */
                    *err = U_ILLEGAL_ESCAPE_SEQUENCE;
                    args->converter->toUCallbackReason = UCNV_IRREGULAR;
                    args->converter->toUBytes[0] = static_cast<uint8_t>(mySourceChar);
                    args->converter->toULength = 1;
                    args->target = myTarget;
                    args->source = mySource;
                    वापस;
                }
                जारी;

            हाल UCNV_SO:
                अगर(pToU2022State->cs[1] != 0) {
                    pToU2022State->g=1;
                    my_तथ्य->isEmptySegment = true;	/* Begin a new segment, empty so far */
                    जारी;
                } अन्यथा {
                    /* illegal to have SO before a matching designator */
                    my_तथ्य->isEmptySegment = false;	/* Handling a different error, reset this to avoid future spurious errs */
                    अवरोध;
                }

            हाल ESC_2022:
                mySource--;
escape:
                {
                    स्थिर अक्षर * mySourceBefore = mySource;
                    int8_t toULengthBefore = args->converter->toULength;

                    changeState_2022(args->converter,&(mySource),
                        mySourceLimit, ISO_2022_CN,err);

                    /* After SO there must be at least one character before a designator (designator error handled separately) */
                    अगर(my_तथ्य->कुंजी==0 && U_SUCCESS(*err) && my_तथ्य->isEmptySegment) {
                        *err = U_ILLEGAL_ESCAPE_SEQUENCE;
                        args->converter->toUCallbackReason = UCNV_IRREGULAR;
                        args->converter->toULength = static_cast<int8_t>(toULengthBefore + (mySource - mySourceBefore));
                    }
                }

                /* invalid or illegal escape sequence */
                अगर(U_FAILURE(*err)){
                    args->target = myTarget;
                    args->source = mySource;
                    my_तथ्य->isEmptySegment = false;	/* Reset to avoid future spurious errors */
                    वापस;
                }
                जारी;

            /* ISO-2022-CN does not use single-byte (C1) SS2 and SS3 */

            हाल CR:
            हाल LF:
                uprv_memset(pToU2022State, 0, माप(ISO2022State));
                U_FALLTHROUGH;
            शेष:
                /* convert one or two bytes */
                my_तथ्य->isEmptySegment = false;
                अगर(pToU2022State->g != 0) {
                    अगर(mySource < mySourceLimit) {
                        UConverter_Shared_तथ्य *cnv;
                        StateEnum अस्थायी_State;
                        int32_t अस्थायी_Buf_Len;
                        पूर्णांक leadIsOk, trailIsOk;
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
                        अगर (leadIsOk && trailIsOk) {
                            ++mySource;
                            अस्थायी_State = static_cast<StateEnum>(pToU2022State->cs[pToU2022State->g]);
                            अगर(अस्थायी_State >= CNS_11643_0) {
                                cnv = my_तथ्य->myConverterArray[CNS_11643];
                                अस्थायी_Buf[0] = static_cast<अक्षर>(0x80 + (अस्थायी_State - CNS_11643_0));
                                अस्थायी_Buf[1] = static_cast<अक्षर>(mySourceChar);
                                अस्थायी_Buf[2] = static_cast<अक्षर>(trailByte);
                                अस्थायी_Buf_Len = 3;

                            }अन्यथा{
                                U_ASSERT(अस्थायी_State<UCNV_2022_MAX_CONVERTERS);
                                cnv = my_तथ्य->myConverterArray[अस्थायी_State];
                                अस्थायी_Buf[0] = static_cast<अक्षर>(mySourceChar);
                                अस्थायी_Buf[1] = static_cast<अक्षर>(trailByte);
                                अस्थायी_Buf_Len = 2;
                            }
                            targetUniChar = ucnv_MBCSSimple_Get_अग्र_UChar(cnv, अस्थायी_Buf, अस्थायी_Buf_Len, false);
                            mySourceChar = (mySourceChar << 8) | trailByte;
                        } अन्यथा अगर (!(trailIsOk || IS_2022_CONTROL(trailByte))) {
                            /* report a pair of illegal bytes if the second byte is not a DBCS starter */
                            ++mySource;
                            /* add another bit so that the code below writes 2 bytes in case of error */
                            mySourceChar = 0x10000 | (mySourceChar << 8) | trailByte;
                        }
                        अगर(pToU2022State->g>=2) {
                            /* return from a single-shift state to the previous one */
                            pToU2022State->g=pToU2022State->पूर्व_G;
                        }
                    } अन्यथा {
                        args->converter->toUBytes[0] = static_cast<uint8_t>(mySourceChar);
                        args->converter->toULength = 1;
                        जाओ endloop;
                    }
                }
                अन्यथा{
                    अगर(mySourceChar <= 0x7f) {
                        targetUniChar = static_cast<char16_t>(mySourceChar);
                    }
                }
                अवरोध;
            }
            अगर(targetUniChar < (missingCharMarker-1/*0xfffe*/)){
                अगर(args->offsets){
                    args->offsets[myTarget - args->target] = static_cast<int32_t>(mySource - args->source - (mySourceChar <= 0xff ? 1 : 2));
                }
                *(myTarget++) = static_cast<char16_t>(targetUniChar);
            }
            अन्यथा अगर(targetUniChar > missingCharMarker){
                /* disassemble the surrogate pair and write to output*/
                targetUniChar-=0x0010000;
                *myTarget = static_cast<char16_t>(0xd800 + static_cast<char16_t>(targetUniChar >> 10));
                अगर(args->offsets){
                    args->offsets[myTarget - args->target] = static_cast<int32_t>(mySource - args->source - (mySourceChar <= 0xff ? 1 : 2));
                }
                ++myTarget;
                अगर(myTarget< args->targetLimit){
                    *myTarget = static_cast<char16_t>(0xdc00 + static_cast<char16_t>(targetUniChar & 0x3ff));
                    अगर(args->offsets){
                        args->offsets[myTarget - args->target] = static_cast<int32_t>(mySource - args->source - (mySourceChar <= 0xff ? 1 : 2));
                    }
                    ++myTarget;
                }अन्यथा{
                    args->converter->UChar_Error_बफर[args->converter->UChar_Error_बफर_लंबाई++]=
                                    static_cast<char16_t>(0xdc00 + static_cast<char16_t>(targetUniChar & 0x3ff));
                }

            }
            अन्यथा{
                /* Call the callback function*/
                toUnicodeCallback(args->converter,mySourceChar,targetUniChar,err);
                अवरोध;
            }
        }
        अन्यथा{
            *err =U_बफर_OVERFLOW_ERROR;
            अवरोध;
        }
    }
endloop:
    args->target = myTarget;
    args->source = mySource;
}
#पूर्ण_अगर /* #if !UCONFIG_ONLY_HTML_CONVERSION */

अटल व्योम U_CALLCONV
_ISO_2022_WriteSub(UConverterFromUnicodeArgs *args, int32_t offset_सूचकांक, UErrorCode *err) {
    UConverter *cnv = args->converter;
    UConverter_तथ्य_ISO2022* my_Converter_तथ्य = static_cast<UConverter_तथ्य_ISO2022*>(cnv->extraInfo);
    ISO2022State *pFromU2022State=&my_Converter_तथ्य->fromU2022State;
    अक्षर *p, *subchar;
    अक्षर बफर[8];
    int32_t length;

    subchar = reinterpret_cast<अक्षर*>(cnv->subChars);
    length=cnv->subCharLen; /* assume length==1 for most variants */

    p = बफर;
    चयन(my_Converter_तथ्य->locale[0]){
    हाल 'j':
        {
            int8_t cs;

            अगर(pFromU2022State->g == 1) {
                /* JIS7: switch from G1 to G0 */
                pFromU2022State->g = 0;
                *p++ = UCNV_SI;
            }

            cs = pFromU2022State->cs[0];
            अगर(cs != ASCII && cs != JISX201) {
                /* not in ASCII or JIS X 0201: switch to ASCII */
                pFromU2022State->cs[0] = static_cast<int8_t>(ASCII);
                *p++ = '\x1b';
                *p++ = '\x28';
                *p++ = '\x42';
            }

            *p++ = subchar[0];
            अवरोध;
        }
    हाल 'c':
        अगर(pFromU2022State->g != 0) {
            /* not in ASCII mode: switch to ASCII */
            pFromU2022State->g = 0;
            *p++ = UCNV_SI;
        }
        *p++ = subchar[0];
        अवरोध;
    हाल 'k':
        अगर(my_Converter_तथ्य->version == 0) {
            अगर(length == 1) {
                अगर(args->converter->fromUnicodeStatus) {
                    /* in DBCS mode: switch to SBCS */
                    args->converter->fromUnicodeStatus = 0;
                    *p++ = UCNV_SI;
                }
                *p++ = subchar[0];
            } अन्यथा /* length == 2*/ {
                अगर(!args->converter->fromUnicodeStatus) {
                    /* in SBCS mode: switch to DBCS */
                    args->converter->fromUnicodeStatus = 1;
                    *p++ = UCNV_SO;
                }
                *p++ = subchar[0];
                *p++ = subchar[1];
            }
            अवरोध;
        } अन्यथा {
            /* save the subconverter's substitution string */
            uint8_t *currentSubChars = my_Converter_तथ्य->currentConverter->subChars;
            int8_t currentSubCharLen = my_Converter_तथ्य->currentConverter->subCharLen;

            /* set our substitution string into the subconverter */
            my_Converter_तथ्य->currentConverter->subChars = reinterpret_cast<uint8_t*>(subchar);
            my_Converter_तथ्य->currentConverter->subCharLen = static_cast<int8_t>(length);

            /* let the subconverter write the subchar, set/retrieve fromUChar32 state */
            args->converter = my_Converter_तथ्य->currentConverter;
            my_Converter_तथ्य->currentConverter->fromUChar32 = cnv->fromUChar32;
            ucnv_cbFromUWriteSub(args, 0, err);
            cnv->fromUChar32 = my_Converter_तथ्य->currentConverter->fromUChar32;
            args->converter = cnv;

            /* restore the subconverter's substitution string */
            my_Converter_तथ्य->currentConverter->subChars = currentSubChars;
            my_Converter_तथ्य->currentConverter->subCharLen = currentSubCharLen;

            अगर(*err == U_बफर_OVERFLOW_ERROR) {
                अगर(my_Converter_तथ्य->currentConverter->char_Error_बफर_लंबाई > 0) {
                    uprv_memcpy(
                        cnv->char_Error_बफर,
                        my_Converter_तथ्य->currentConverter->char_Error_बफर,
                        my_Converter_तथ्य->currentConverter->char_Error_बफर_लंबाई);
                }
                cnv->char_Error_बफर_लंबाई = my_Converter_तथ्य->currentConverter->char_Error_बफर_लंबाई;
                my_Converter_तथ्य->currentConverter->char_Error_बफर_लंबाई = 0;
            }
            वापस;
        }
    शेष:
        /* not expected */
        अवरोध;
    }
    ucnv_cbFromUWriteBytes(args,
                           बफर, static_cast<int32_t>(p - बफर),
                           offset_सूचकांक, err);
}

/*
 * Structure for cloning an ISO 2022 converter into a single memory block.
 */
काष्ठा cloneStruct
{
    UConverter cnv;
    UConverter currentConverter;
    UConverter_तथ्य_ISO2022 mydata;
};


U_CDECL_BEGIN

अटल UConverter * U_CALLCONV
_ISO_2022_SafeClone(
            स्थिर UConverter *cnv,
            व्योम *ढेर_बफर,
            int32_t *p_बफर_आकार,
            UErrorCode *status)
{
    काष्ठा cloneStruct * localClone;
    UConverter_तथ्य_ISO2022 *cnv_तथ्य;
    int32_t i, size;

    अगर (U_FAILURE(*status)){
        वापस nullptr;
    }

    अगर (*p_बफर_आकार == 0) { /* 'preflighting' request - set needed size into *pBufferSize */
        *p_बफर_आकार = (int32_t)माप(काष्ठा cloneStruct);
        वापस nullptr;
    }

    cnv_तथ्य = (UConverter_तथ्य_ISO2022 *)cnv->extraInfo;
    localClone = (काष्ठा cloneStruct *)ढेर_बफर;

    /* ucnv.c/ucnv_safeClone() copied the main UConverter already */

    uprv_memcpy(&localClone->mydata, cnv_तथ्य, माप(UConverter_तथ्य_ISO2022));
    localClone->cnv.extraInfo = &localClone->mydata; /* set pointer to extra data */
    localClone->cnv.isExtraLocal = true;

    /* share the subconverters */

    अगर(cnv_तथ्य->currentConverter != nullptr) {
        size = (int32_t)माप(UConverter);
        localClone->mydata.currentConverter =
            ucnv_safeClone(cnv_तथ्य->currentConverter,
                            &localClone->currentConverter,
                            &size, status);
        अगर(U_FAILURE(*status)) {
            वापस nullptr;
        }
    }

    क्रम(i=0; i<UCNV_2022_MAX_CONVERTERS; ++i) {
        अगर(cnv_तथ्य->myConverterArray[i] != nullptr) {
            ucnv_increment_Ref_गिनती(cnv_तथ्य->myConverterArray[i]);
        }
    }

    वापस &localClone->cnv;
}

U_CDECL_END

अटल व्योम U_CALLCONV
_ISO_2022_GetUnicodeSet(स्थिर UConverter *cnv,
                    स्थिर USetAdder *sa,
                    UConverterUnicodeSet which,
                    UErrorCode *pErrorCode)
{
    int32_t i;
    UConverter_तथ्य_ISO2022* cnv_तथ्य;

    अगर (U_FAILURE(*pErrorCode)) {
        वापस;
    }
#अगर_घोषित U_ENABLE_GENERIC_ISO_2022
    अगर (cnv->shared_तथ्य == &ISO2022_तथ्य) {
        /* We use UTF-8 in this case */
        sa->addRange(sa->set, 0, 0xd7FF);
        sa->addRange(sa->set, 0xE000, 0x10FFFF);
        वापस;
    }
#पूर्ण_अगर

    cnv_तथ्य = static_cast<UConverter_तथ्य_ISO2022*>(cnv->extraInfo);

    /* open a set and initialize it with code points that are algorithmically round-tripped */
    चयन(cnv_तथ्य->locale[0]){
    हाल 'j':
        /* include JIS X 0201 which is hardcoded */
        sa->add(sa->set, 0xa5);
        sa->add(sa->set, 0x203e);
        अगर(jpCharsetMasks[cnv_तथ्य->version]&CSM(ISO8859_1)) {
            /* include Latin-1 for some variants of JP */
            sa->addRange(sa->set, 0, 0xff);
        } अन्यथा {
            /* include ASCII for JP */
            sa->addRange(sa->set, 0, 0x7f);
        }
        अगर(cnv_तथ्य->version==3 || cnv_तथ्य->version==4 || which==UCNV_ROUNDTRIP_AND_FALLBACK_SET) {
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
        अवरोध;
#अगर !UCONFIG_ONLY_HTML_CONVERSION
    हाल 'c':
    हाल 'z':
        /* include ASCII for CN */
        sa->addRange(sa->set, 0, 0x7f);
        अवरोध;
    हाल 'k':
        /* there is only one converter for KR, and it is not in the myConverterArray[] */
        cnv_तथ्य->currentConverter->shared_तथ्य->impl->getUnicodeSet(
                cnv_तथ्य->currentConverter, sa, which, pErrorCode);
        /* the loop over myConverterArray[] will simply not find another converter */
        अवरोध;
#पूर्ण_अगर
    शेष:
        अवरोध;
    }

#अगर 0  /* Replaced by ucnv_MBCSGetFilteredUnicodeSetForUnicode() until we implement ucnv_getUnicodeSet() with reverse fallbacks. */
            अगर( (cnv_तथ्य->locale[0]=='c' || cnv_तथ्य->locale[0]=='z') &&
                cnv_तथ्य->version==0 && i==CNS_11643
            ) {
                /* special handling for non-EXT ISO-2022-CN: add only code points for CNS planes 1 and 2 */
                ucnv_MBCSGetUnicodeSetForBytes(
                        cnv_तथ्य->myConverterArray[i],
                        sa, UCNV_ROUNDTRIP_SET,
                        0, 0x81, 0x82,
                        pErrorCode);
            }
#पूर्ण_अगर

    क्रम (i=0; i<UCNV_2022_MAX_CONVERTERS; i++) {
        UConverterSetFilter filter;
        अगर(cnv_तथ्य->myConverterArray[i]!=nullptr) {
            अगर(cnv_तथ्य->locale[0]=='j' && i==JISX208) {
                /*
                 * Only add code points that map to Shift-JIS codes
                 * corresponding to JIS X 0208.
                 */
                filter=UCNV_SET_FILTER_SJIS;
#अगर !UCONFIG_ONLY_HTML_CONVERSION
            } अन्यथा अगर( (cnv_तथ्य->locale[0]=='c' || cnv_तथ्य->locale[0]=='z') &&
                       cnv_तथ्य->version==0 && i==CNS_11643) {
                /*
                 * Version-specific for CN:
                 * CN version 0 does not map CNS planes 3..7 although
                 * they are all available in the CNS conversion table;
                 * CN version 1 (-EXT) does map them all.
                 * The two versions create different Unicode sets.
                 */
                filter=UCNV_SET_FILTER_2022_CN;
            } अन्यथा अगर(i==KSC5601) {
                /*
                 * Some of the KSC 5601 tables (convrtrs.txt has this aliases on multiple tables)
                 * are broader than GR94.
                 */
                filter=UCNV_SET_FILTER_GR94DBCS;
#पूर्ण_अगर
            } अन्यथा {
                filter=UCNV_SET_FILTER_NONE;
            }
            ucnv_MBCSGetFilteredUnicodeSetForUnicode(cnv_तथ्य->myConverterArray[i], sa, which, filter, pErrorCode);
        }
    }

    /*
     * ISO 2022 converters must not convert SO/SI/ESC despite what
     * sub-converters do by themselves.
     * Remove these characters from the set.
     */
    sa->हटाओ(sa->set, 0x0e);
    sa->हटाओ(sa->set, 0x0f);
    sa->हटाओ(sa->set, 0x1b);

    /* ISO 2022 converters do not convert C1 controls either */
    sa->removeRange(sa->set, 0x80, 0x9f);
}

अटल स्थिर UConverterImpl _ISO2022Impl={
    UCNV_ISO_2022,

    nullptr,
    nullptr,

    _ISO2022Open,
    _ISO2022Close,
    _ISO2022Reset,

#अगर_घोषित U_ENABLE_GENERIC_ISO_2022
    T_UConverter_toUnicode_ISO_2022_OFFSETS_LOGIC,
    T_UConverter_toUnicode_ISO_2022_OFFSETS_LOGIC,
    ucnv_fromUnicode_UTF8,
    ucnv_fromUnicode_UTF8_OFFSETS_LOGIC,
#अन्यथा
    nullptr,
    nullptr,
    nullptr,
    nullptr,
#पूर्ण_अगर
    nullptr,

    nullptr,
    _ISO2022getName,
    _ISO_2022_WriteSub,
    _ISO_2022_SafeClone,
    _ISO_2022_GetUnicodeSet,

    nullptr,
    nullptr
};
अटल स्थिर UConverter_Static_तथ्य ISO2022_Static_तथ्य={
    माप(UConverter_Static_तथ्य),
    "ISO_2022",
    2022,
    UCNV_IBM,
    UCNV_ISO_2022,
    1,
    3, /* max 3 bytes per char16_t from UTF-8 (4 bytes from surrogate _pair_) */
    { 0x1a, 0, 0, 0 },
    1,
    false,
    false,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};
स्थिर UConverter_Shared_तथ्य ISO2022_तथ्य=
        UCNV_IMMUTABLE_SHARED_तथ्य_INITIALIZER(&ISO2022_Static_तथ्य, &_ISO2022Impl);

/*************JP****************/
अटल स्थिर UConverterImpl _ISO2022JPImpl={
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
अटल स्थिर UConverter_Static_तथ्य ISO2022_JPStatic_तथ्य={
    माप(UConverter_Static_तथ्य),
    "ISO_2022_JP",
    0,
    UCNV_IBM,
    UCNV_ISO_2022,
    1,
    6, /* max 6 bytes per char16_t: 4-byte escape sequence + DBCS */
    { 0x1a, 0, 0, 0 },
    1,
    false,
    false,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

namespace {

स्थिर UConverter_Shared_तथ्य _ISO2022JPData=
        UCNV_IMMUTABLE_SHARED_तथ्य_INITIALIZER(&ISO2022_JPStatic_तथ्य, &_ISO2022JPImpl);

}  // namespace

#अगर !UCONFIG_ONLY_HTML_CONVERSION
/************* KR ***************/
अटल स्थिर UConverterImpl _ISO2022KRImpl={
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
अटल स्थिर UConverter_Static_तथ्य ISO2022_KRStatic_तथ्य={
    माप(UConverter_Static_तथ्य),
    "ISO_2022_KR",
    0,
    UCNV_IBM,
    UCNV_ISO_2022,
    1,
    8, /* max 8 bytes per char16_t */
    { 0x1a, 0, 0, 0 },
    1,
    false,
    false,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

namespace {

स्थिर UConverter_Shared_तथ्य _ISO2022KRData=
        UCNV_IMMUTABLE_SHARED_तथ्य_INITIALIZER(&ISO2022_KRStatic_तथ्य, &_ISO2022KRImpl);

}  // namespace

/*************** CN ***************/
अटल स्थिर UConverterImpl _ISO2022CNImpl={

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
अटल स्थिर UConverter_Static_तथ्य ISO2022_CNStatic_तथ्य={
    माप(UConverter_Static_तथ्य),
    "ISO_2022_CN",
    0,
    UCNV_IBM,
    UCNV_ISO_2022,
    1,
    8, /* max 8 bytes per char16_t: 4-byte CNS designator + 2 bytes for SS2/SS3 + DBCS */
    { 0x1a, 0, 0, 0 },
    1,
    false,
    false,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

namespace {

स्थिर UConverter_Shared_तथ्य _ISO2022CNData=
        UCNV_IMMUTABLE_SHARED_तथ्य_INITIALIZER(&ISO2022_CNStatic_तथ्य, &_ISO2022CNImpl);

}  // namespace
#पूर्ण_अगर /* #if !UCONFIG_ONLY_HTML_CONVERSION */

#पूर्ण_अगर /* #if !UCONFIG_NO_LEGACY_CONVERSION */
