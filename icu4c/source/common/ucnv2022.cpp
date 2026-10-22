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

#اگر_تعریف_شده U_ENABLE_GENERIC_ISO_2022
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
#پایان_اگر

#إذا !UCONFIG_ONLY_HTML_CONVERSION
ساكن ثابت حرف لغزش_درون_STR[]  = "\x0F";
// static const char SHIFT_OUT_STR[] = "\x0E";
#پایان_اگر

#تعریف_کن CR      0x0D
#تعریف_کن LF      0x0A
#تعریف_کن H_TAB   0x09
#تعریف_کن V_TAB   0x0B
#تعریف_کن SPACE   0x20

تعداد {
    HWKANA_شروع=0xff61,
    HWKANA_انتها=0xff9f
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
    GR94_شروع=0xa1,
    GR94_انتها=0xfe,
    GR96_شروع=0xa0,
    GR96_انتها=0xff
};

/*
 * ISO 2022 control codes must not be converted from Unicode
 * because they would mess up the byte stream.
 * The bit mask 0x0800c000 has bits set at bit positions 0xe, 0xf, 0x1b
 * corresponding to SO, SI, and ESC.
 */
#تعریف_کن IS_2022_CONTROL(c) (((c)<0x20) && (((uint32_t)1<<(c))&0x0800c000)!=0)

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
        HWKANA_7_بیت=8,    /* Halfwidth Katakana 7 bit */

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
#تعریف_کن IS_JP_DBCS(cs) (JISX208==(cs))
#وإلا
#تعریف_کن IS_JP_DBCS(cs) (JISX208<=(cs) && (cs)<=KSC5601)
#پایان_اگر

#تعریف_کن CSM(cs) ((uint16_t)1<<(cs))

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
تعداد { MAX_JA_نسخه=0 };
#وإلا
تعداد { MAX_JA_نسخه=4 };
#پایان_اگر
ساكن ثابت uint16_t jpCharsetMasks[MAX_JA_نسخه+1]={
    CSM(ASCII)|CSM(JISX201)|CSM(JISX208)|CSM(HWKANA_7_بیت),
#إذا !UCONFIG_ONLY_HTML_CONVERSION
    CSM(ASCII)|CSM(JISX201)|CSM(JISX208)|CSM(HWKANA_7_بیت)|CSM(JISX212),
    CSM(ASCII)|CSM(JISX201)|CSM(JISX208)|CSM(HWKANA_7_بیت)|CSM(JISX212)|CSM(GB2312)|CSM(KSC5601)|CSM(ISO8859_1)|CSM(ISO8859_7),
    CSM(ASCII)|CSM(JISX201)|CSM(JISX208)|CSM(HWKANA_7_بیت)|CSM(JISX212)|CSM(GB2312)|CSM(KSC5601)|CSM(ISO8859_1)|CSM(ISO8859_7),
    CSM(ASCII)|CSM(JISX201)|CSM(JISX208)|CSM(HWKANA_7_بیت)|CSM(JISX212)|CSM(GB2312)|CSM(KSC5601)|CSM(ISO8859_1)|CSM(ISO8859_7)
#پایان_اگر
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

#تعریف_کن UCNV_OPTIONS_نسخه_ماسک 0xf
#تعریف_کن UCNV_2022_MAX_CONVERTERS 10

تعريفنوع بنية{
    UConverter_Shared_داده *myConverterArray[UCNV_2022_MAX_CONVERTERS];
    UConverter *currentConverter;
    Cnv2022Type currentType;
    ISO2022State مقصد_U2022_State, fromU2022State;
    uint32_t کلید;
    uint32_t نسخه;
#اگر_تعریف_شده U_ENABLE_GENERIC_ISO_2022
    UBool is_اولی_میانگیر;
#پایان_اگر
    UBool is_خالی_Segment;
    حرف name[30];
    حرف locale[3];
}UConverter_داده_ISO2022;

/* Protos */
/* ISO-2022 ----------------------------------------------------------------- */

/*Forward declaration */
U_CFUNC فراغ U_CALLCONV
ucnv_fromUnicode_UTF8(UConverter_From_Unicode_آرگومانها * آرگومانها_2,
                      UError_کد * err);
U_CFUNC فراغ U_CALLCONV
ucnv_fromUnicode_UTF8_OFFSETS_LOGIC(UConverter_From_Unicode_آرگومانها * آرگومانها_2,
                                    UError_کد * err);

#تعریف_کن ESC_2022 0x1B /*ESC*/

تعريفنوع تعداد
{
        INVALID_2022 = -1, /*Doesn't correspond to a valid iso 2022 escape sequence*/
        VALID_NON_TERMINAL_2022 = 0, /*so far corresponds to a valid iso 2022 escape sequence*/
        VALID_TERMINAL_2022 = 1, /*corresponds to a valid iso 2022 escape sequence*/
        VALID_MAYBE_TERMINAL_2022 = 2 /*so far matches one iso 2022 escape sequence, but by adding more characters might match another escape sequence*/
} UCNV_جدول_حالتها_2022;

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

#اگر_تعریف_شده U_ENABLE_GENERIC_ISO_2022
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
#پایان_اگر

#تعریف_کن MAX_حالتها_2022 74
ساكن ثابت int32_t esc_Seq_State_جدول_کلید_2022[MAX_حالتها_2022] = {
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

#اگر_تعریف_شده U_ENABLE_GENERIC_ISO_2022

ساكن ثابت حرف* ثابت esc_Seq_State_جدول_نتیجه_2022[MAX_حالتها_2022] = {
 /*  0                      1                        2                      3                   4                   5                        6                      7                       8                       9    */

     اشاره_تهی                   ,اشاره_تهی                   ,اشاره_تهی                   ,اشاره_تهی               ,اشاره_تهی               ,اشاره_تهی                   ,اشاره_تهی                   ,اشاره_تهی                   ,"latin1"               ,"latin1"
    ,"latin1"               ,"ibm-865"              ,"ibm-865"              ,"ibm-865"          ,"ibm-865"          ,"ibm-865"              ,"ibm-865"              ,"JISX0201"             ,"JISX0201"             ,"latin1"
    ,"latin1"               ,اشاره_تهی                   ,"JISX-208"             ,"ibm-5478"         ,"JISX-208"         ,اشاره_تهی                   ,اشاره_تهی                   ,اشاره_تهی                   ,اشاره_تهی                   ,"UTF8"
    ,"ISO-8859-1"           ,"ISO-8859-7"           ,"JIS-X-208"            ,اشاره_تهی               ,"ibm-955"          ,"ibm-367"              ,"ibm-952"              ,"ibm-949"              ,"JISX-212"             ,"ibm-1383"
    ,"ibm-952"              ,"ibm-964"              ,"ibm-964"              ,"ibm-964"          ,"ibm-964"          ,"ibm-964"              ,"ibm-964"              ,"ibm-5478"         ,"ibm-949"              ,"ISO-IR-165"
    ,"CNS-11643-1992,1"     ,"CNS-11643-1992,2"     ,"CNS-11643-1992,3"     ,"CNS-11643-1992,4" ,"CNS-11643-1992,5" ,"CNS-11643-1992,6"     ,"CNS-11643-1992,7"     ,"UTF16_PlatformEndian" ,"UTF16_PlatformEndian" ,"UTF16_PlatformEndian"
    ,"UTF16_PlatformEndian" ,"UTF16_PlatformEndian" ,"UTF16_PlatformEndian" ,اشاره_تهی               ,"latin1"           ,"ibm-912"              ,"ibm-913"              ,"ibm-914"              ,"ibm-813"              ,"ibm-1089"
    ,"ibm-920"              ,"ibm-915"              ,"ibm-915"              ,"latin1"
};

#پایان_اگر

ساكن ثابت int8_t esc_Seq_State_جدول_مقدار_2022[MAX_حالتها_2022] = {
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
#اگر_تعریف_شده U_ENABLE_GENERIC_ISO_2022
    ISO_2022=0,
#پایان_اگر
    ISO_2022_JP=1,
#إذا !UCONFIG_ONLY_HTML_CONVERSION
    ISO_2022_KR=2,
    ISO_2022_CN=3
#پایان_اگر
} Variant2022;

/*********** ISO 2022 Converter Protos ***********/
ساكن فراغ U_CALLCONV
_ISO2022Open(UConverter *cnv, UConverter_بارگیری_آرگومانها *p_آرگومانها, UError_کد *error_کد);

ساكن فراغ U_CALLCONV
 _ISO2022Close(UConverter *converter);

ساكن فراغ U_CALLCONV
_ISO2022Reset(UConverter *converter, UConverterResetChoice choice);

U_CDECL_BEGIN
ساكن ثابت حرف * U_CALLCONV
_ISO2022getName(ثابت UConverter* cnv);
U_CDECL_انتها

ساكن فراغ  U_CALLCONV
_ISO_2022_WriteSub(UConverter_From_Unicode_آرگومانها *آرگومانها_2, int32_t جابه_جایی_شاخص, UError_کد *err);

U_CDECL_BEGIN
ساكن UConverter * U_CALLCONV
ISO_2022_امن_Clone(ثابت UConverter *cnv, فراغ *پشته_میانگیر, int32_t *p_میانگیر_اندازه, UError_کد *وضعیت);

U_CDECL_انتها

#اگر_تعریف_شده U_ENABLE_GENERIC_ISO_2022
ساكن فراغ U_CALLCONV
T_UConverter_مقصد_Unicode_ISO_2022_OFFSETS_LOGIC(UConverter_مقصد_Unicode_آرگومانها* آرگومانها_2, UError_کد* err);
#پایان_اگر

نطاق {

/*const UConverterSharedData _ISO2022Data;*/
خارجي ثابت UConverter_Shared_داده _ISO2022JPData;

#إذا !UCONFIG_ONLY_HTML_CONVERSION
خارجي ثابت UConverter_Shared_داده _ISO2022KRData;
خارجي ثابت UConverter_Shared_داده _ISO2022CNData;
#پایان_اگر

}  // namespace

/*************** Converter implementations ******************/

/* The purpose of this function is to get around gcc compiler warnings. */
ساكن مضمن فراغ
fromUWriteUInt8(UConverter *cnv,
                 ثابت حرف *بایتها, int32_t length,
                 uint8_t **هدف, ثابت حرف *هدف_حد,
                 int32_t **offsets,
                 int32_t منبع_شاخص,
                 UError_کد *p_Error_کد)
{
    حرف* هدف_Chars = reinterpret_cast<حرف*>(*هدف);
    ucnv_from_UWrite_بایتها(cnv, بایتها, length, &هدف_Chars, هدف_حد,
                         offsets, منبع_شاخص, p_Error_کد);
    *هدف = reinterpret_cast<uint8_t*>(هدف_Chars);

}

ساكن مضمن فراغ
مجموعه_Initial_State_مقصد_Unicode_KR(UConverter* /*converter*/, UConverter_داده_ISO2022 *my_Converter_داده){
    إذا(my_Converter_داده->نسخه == 1) {
        UConverter *cnv = my_Converter_داده->currentConverter;

        cnv->مقصد_Unicode_وضعیت=0;     /*جابه_جایی*/
        cnv->نما=0;                /* state */
        cnv->مقصد_ULength=0;           /* byteIndex */
    }
}

ساكن مضمن فراغ
مجموعه_Initial_State_From_Unicode_KR(UConverter* converter,UConverter_داده_ISO2022 *my_Converter_داده){
   /* in ISO-2022-KR the designator sequence appears only once
    * in a file so we append it only once
    */
    إذا( converter->char_Error_میانگیر_درازی==0){

        converter->char_Error_میانگیر_درازی = 4;
        converter->char_Error_میانگیر[0] = 0x1b;
        converter->char_Error_میانگیر[1] = 0x24;
        converter->char_Error_میانگیر[2] = 0x29;
        converter->char_Error_میانگیر[3] = 0x43;
    }
    إذا(my_Converter_داده->نسخه == 1) {
        UConverter *cnv = my_Converter_داده->currentConverter;

        cnv->fromUChar32=0;
        cnv->from_Unicode_وضعیت=1;   /* prevLength */
    }
}

ساكن فراغ U_CALLCONV
_ISO2022Open(UConverter *cnv, UConverter_بارگیری_آرگومانها *p_آرگومانها, UError_کد *error_کد){

    حرف myLocale[7]={' ',' ',' ',' ',' ',' ', '\0'};

    cnv->extraInfo = uprv_malloc (حجم (UConverter_داده_ISO2022));
    إذا(cnv->extraInfo != اشاره_تهی) {
        UConverterNamePieces پشته_Pieces;
        UConverter_بارگیری_آرگومانها پشته_آرگومانها=UCNV_بارگیری_آرگومانها_INITIALIZER;
        UConverter_داده_ISO2022* my_Converter_داده = static_cast<UConverter_داده_ISO2022*>(cnv->extraInfo);
        uint32_t نسخه;

        پشته_آرگومانها.onlyTestIsLoadable = p_آرگومانها->onlyTestIsLoadable;

        uprv_memset(my_Converter_داده, 0, حجم(UConverter_داده_ISO2022));
        my_Converter_داده->currentType = ASCII1;
        cnv->from_Unicode_وضعیت =خطأ;
        إذا(p_آرگومانها->locale){
            uprv_strncpy(myLocale, p_آرگومانها->locale, حجم(myLocale)-1);
        }
        نسخه = p_آرگومانها->options & UCNV_OPTIONS_نسخه_ماسک;
        my_Converter_داده->نسخه = نسخه;
        إذا(myLocale[0]=='j' && (myLocale[1]=='a'|| myLocale[1]=='p') &&
            (myLocale[2]=='_' || myLocale[2]=='\0'))
        {
            /* open the required converters and cache them */
            إذا(نسخه>MAX_JA_نسخه) {
                // ICU 55 fails to open a converter for an unsupported version.
                // Previously, it fell back to version 0, but that would yield
                // unexpected behavior.
                *error_کد = U_MISSING_RESOURCE_ERROR;
                أرجع;
            }
            إذا(jpCharsetMasks[نسخه]&CSM(ISO8859_7)) {
                my_Converter_داده->myConverterArray[ISO8859_7] =
                    ucnv_بارگیری_Shared_داده("ISO8859_7", &پشته_Pieces, &پشته_آرگومانها, error_کد);
            }
            my_Converter_داده->myConverterArray[JISX208] =
                ucnv_بارگیری_Shared_داده("Shift-JIS", &پشته_Pieces, &پشته_آرگومانها, error_کد);
            إذا(jpCharsetMasks[نسخه]&CSM(JISX212)) {
                my_Converter_داده->myConverterArray[JISX212] =
                    ucnv_بارگیری_Shared_داده("jisx-212", &پشته_Pieces, &پشته_آرگومانها, error_کد);
            }
            إذا(jpCharsetMasks[نسخه]&CSM(GB2312)) {
                my_Converter_داده->myConverterArray[GB2312] =
                    ucnv_بارگیری_Shared_داده("ibm-5478", &پشته_Pieces, &پشته_آرگومانها, error_کد);   /* gb_2312_80-1 */
            }
            إذا(jpCharsetMasks[نسخه]&CSM(KSC5601)) {
                my_Converter_داده->myConverterArray[KSC5601] =
                    ucnv_بارگیری_Shared_داده("ksc_5601", &پشته_Pieces, &پشته_آرگومانها, error_کد);
            }

            /* set the function pointers to appropriate functions */
            cnv->shared_داده = const_cast<UConverter_Shared_داده*>(&_ISO2022JPData);
            uprv_strcpy(my_Converter_داده->locale,"ja");

            (فراغ)uprv_strcpy(my_Converter_داده->name,"ISO_2022,locale=ja,version=");
            size_t len = uprv_strlen(my_Converter_داده->name);
            my_Converter_داده->name[len] = static_cast<حرف>(my_Converter_داده->نسخه + static_cast<صحيح>('0'));
            my_Converter_داده->name[len+1]='\0';
        }
#إذا !UCONFIG_ONLY_HTML_CONVERSION
        وإلا إذا(myLocale[0]=='k' && (myLocale[1]=='o'|| myLocale[1]=='r') &&
            (myLocale[2]=='_' || myLocale[2]=='\0'))
        {
            إذا(نسخه>1) {
                // ICU 55 fails to open a converter for an unsupported version.
                // Previously, it fell back to version 0, but that would yield
                // unexpected behavior.
                *error_کد = U_MISSING_RESOURCE_ERROR;
                أرجع;
            }
            ثابت حرف *cnvName;
            إذا(نسخه==1) {
                cnvName="icu-internal-25546";
            } وإلا {
                cnvName="ibm-949";
                my_Converter_داده->نسخه=نسخه=0;
            }
            إذا(p_آرگومانها->onlyTestIsLoadable) {
                ucnv_canCreateConverter(cnvName, error_کد);  /* errorCode carries result */
                uprv_free(cnv->extraInfo);
                cnv->extraInfo=اشاره_تهی;
                أرجع;
            } وإلا {
                my_Converter_داده->currentConverter=ucnv_open(cnvName, error_کد);
                إذا (U_FAILURE(*error_کد)) {
                    _ISO2022Close(cnv);
                    أرجع;
                }

                إذا(نسخه==1) {
                    (فراغ)uprv_strcpy(my_Converter_داده->name,"ISO_2022,locale=ko,version=1");
                    uprv_memcpy(cnv->subChars, my_Converter_داده->currentConverter->subChars, 4);
                    cnv->subCharLen = my_Converter_داده->currentConverter->subCharLen;
                }وإلا{
                    (فراغ)uprv_strcpy(my_Converter_داده->name,"ISO_2022,locale=ko,version=0");
                }

                /* initialize the state variables */
                مجموعه_Initial_State_مقصد_Unicode_KR(cnv, my_Converter_داده);
                مجموعه_Initial_State_From_Unicode_KR(cnv, my_Converter_داده);

                /* set the function pointers to appropriate functions */
                cnv->shared_داده = const_cast<UConverter_Shared_داده*>(&_ISO2022KRData);
                uprv_strcpy(my_Converter_داده->locale,"ko");
            }
        }
        وإلا إذا(((myLocale[0]=='z' && myLocale[1]=='h') || (myLocale[0]=='c'&& myLocale[1]=='n'))&&
            (myLocale[2]=='_' || myLocale[2]=='\0'))
        {
            إذا(نسخه>2) {
                // ICU 55 fails to open a converter for an unsupported version.
                // Previously, it fell back to version 0, but that would yield
                // unexpected behavior.
                *error_کد = U_MISSING_RESOURCE_ERROR;
                أرجع;
            }

            /* open the required converters and cache them */
            my_Converter_داده->myConverterArray[GB2312_1] =
                ucnv_بارگیری_Shared_داده("ibm-5478", &پشته_Pieces, &پشته_آرگومانها, error_کد);
            إذا(نسخه>=1) {
                my_Converter_داده->myConverterArray[ISO_IR_165] =
                    ucnv_بارگیری_Shared_داده("iso-ir-165", &پشته_Pieces, &پشته_آرگومانها, error_کد);
            }
            my_Converter_داده->myConverterArray[CNS_11643] =
                ucnv_بارگیری_Shared_داده("cns-11643-1992", &پشته_Pieces, &پشته_آرگومانها, error_کد);


            /* set the function pointers to appropriate functions */
            cnv->shared_داده = const_cast<UConverter_Shared_داده*>(&_ISO2022CNData);
            uprv_strcpy(my_Converter_داده->locale,"cn");

            إذا (نسخه==0){
                my_Converter_داده->نسخه = 0;
                (فراغ)uprv_strcpy(my_Converter_داده->name,"ISO_2022,locale=zh,version=0");
            }وإلا إذا (نسخه==1){
                my_Converter_داده->نسخه = 1;
                (فراغ)uprv_strcpy(my_Converter_داده->name,"ISO_2022,locale=zh,version=1");
            }وإلا {
                my_Converter_داده->نسخه = 2;
                (فراغ)uprv_strcpy(my_Converter_داده->name,"ISO_2022,locale=zh,version=2");
            }
        }
#پایان_اگر  // !UCONFIG_ONLY_HTML_CONVERSION
        وإلا{
#اگر_تعریف_شده U_ENABLE_GENERIC_ISO_2022
            my_Converter_داده->is_اولی_میانگیر = درست;

            /* append the UTF-8 escape sequence */
            cnv->char_Error_میانگیر_درازی = 3;
            cnv->char_Error_میانگیر[0] = 0x1b;
            cnv->char_Error_میانگیر[1] = 0x25;
            cnv->char_Error_میانگیر[2] = 0x42;

            cnv->shared_داده=(UConverter_Shared_داده*)&ISO2022_داده;
            /* initialize the state variables */
            uprv_strcpy(my_Converter_داده->name,"ISO_2022");
#وإلا
            *error_کد = U_MISSING_RESOURCE_ERROR;
            // Was U_UNSUPPORTED_ERROR but changed in ICU 55 to a more standard
            // data loading error code.
            أرجع;
#پایان_اگر
        }

        cnv->max_بایتها_در_هر_UChar=cnv->shared_داده->static_داده->max_بایتها_در_هر_Char;

        إذا(U_FAILURE(*error_کد) || p_آرگومانها->onlyTestIsLoadable) {
            _ISO2022Close(cnv);
        }
    } وإلا {
        *error_کد = U_حافظه_تخصیص_ERROR;
    }
}


ساكن فراغ U_CALLCONV
_ISO2022Close(UConverter *converter) {
    UConverter_داده_ISO2022* my_داده = static_cast<UConverter_داده_ISO2022*>(converter->extraInfo);
    UConverter_Shared_داده **array = my_داده->myConverterArray;
    int32_t i;

    إذا (converter->extraInfo != اشاره_تهی) {
        /*close the array of converter pointers and free the memory*/
        لكل (i=0; i<UCNV_2022_MAX_CONVERTERS; i++) {
            إذا(array[i]!=اشاره_تهی) {
                ucnv_unload_Shared_داده_If_Ready(array[i]);
            }
        }

        ucnv_close(my_داده->currentConverter);

        إذا(!converter->isExtraLocal){
            uprv_free (converter->extraInfo);
            converter->extraInfo = اشاره_تهی;
        }
    }
}

ساكن فراغ U_CALLCONV
_ISO2022Reset(UConverter *converter, UConverterResetChoice choice) {
    UConverter_داده_ISO2022* my_Converter_داده = static_cast<UConverter_داده_ISO2022*>(converter->extraInfo);
    إذا(choice<=UCNV_RESET_مقصد_UNICODE) {
        uprv_memset(&my_Converter_داده->مقصد_U2022_State, 0, حجم(ISO2022State));
        my_Converter_داده->کلید = 0;
        my_Converter_داده->is_خالی_Segment = خطأ;
    }
    إذا(choice!=UCNV_RESET_مقصد_UNICODE) {
        uprv_memset(&my_Converter_داده->fromU2022State, 0, حجم(ISO2022State));
    }
#اگر_تعریف_شده U_ENABLE_GENERIC_ISO_2022
    إذا(my_Converter_داده->locale[0] == 0){
        إذا(choice<=UCNV_RESET_مقصد_UNICODE) {
            my_Converter_داده->is_اولی_میانگیر = درست;
            my_Converter_داده->کلید = 0;
            إذا (converter->نما == UCNV_SO){
                ucnv_close (my_Converter_داده->currentConverter);
                my_Converter_داده->currentConverter=اشاره_تهی;
            }
            converter->نما = UCNV_SI;
        }
        إذا(choice!=UCNV_RESET_مقصد_UNICODE) {
            /* re-append UTF-8 escape sequence */
            converter->char_Error_میانگیر_درازی = 3;
            converter->char_Error_میانگیر[0] = 0x1b;
            converter->char_Error_میانگیر[1] = 0x28;
            converter->char_Error_میانگیر[2] = 0x42;
        }
    }
    وإلا
#پایان_اگر
    {
        /* reset the state variables */
        إذا(my_Converter_داده->locale[0] == 'k'){
            إذا(choice<=UCNV_RESET_مقصد_UNICODE) {
                مجموعه_Initial_State_مقصد_Unicode_KR(converter, my_Converter_داده);
            }
            إذا(choice!=UCNV_RESET_مقصد_UNICODE) {
                مجموعه_Initial_State_From_Unicode_KR(converter, my_Converter_داده);
            }
        }
    }
}

U_CDECL_BEGIN

ساكن ثابت حرف * U_CALLCONV
_ISO2022getName(ثابت UConverter* cnv){
    إذا(cnv->extraInfo){
        UConverter_داده_ISO2022* my_داده= (UConverter_داده_ISO2022*)cnv->extraInfo;
        أرجع my_داده->name;
    }
    أرجع اشاره_تهی;
}

U_CDECL_انتها


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
ساكن ثابت int8_t بعدی_State_مقصد_Unicode_JP[MAX_حالتها_2022]= {
/*      0                1               2               3               4               5               6               7               8               9    */
    INVALID_STATE   ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,SS2_STATE      ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE
    ,ASCII          ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,JISX201        ,HWKANA_7_بیت    ,JISX201        ,INVALID_STATE
    ,INVALID_STATE  ,INVALID_STATE  ,JISX208        ,GB2312         ,JISX208        ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE
    ,ISO8859_1      ,ISO8859_7      ,JISX208        ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,KSC5601        ,JISX212        ,INVALID_STATE
    ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE
    ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE
    ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE
    ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE
};

#إذا !UCONFIG_ONLY_HTML_CONVERSION
/*************** to unicode *******************/
ساكن ثابت int8_t بعدی_State_مقصد_Unicode_CN[MAX_حالتها_2022]= {
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
#پایان_اگر


ساكن UCNV_جدول_حالتها_2022
get_کلید_2022(حرف c,int32_t* کلید,int32_t* جابه_جایی){
    int32_t togo;
    int32_t کم = 0;
    int32_t hi = MAX_حالتها_2022;
    int32_t oldmid=0;

    togo = normalize_esq_chars_2022[static_cast<uint8_t>(c)];
    إذا(togo == 0) {
        /* not a valid character anywhere in an escape sequence */
        *کلید = 0;
        *جابه_جایی = 0;
        أرجع INVALID_2022;
    }
    togo = (*کلید << 5) + togo;

    طالما (hi != کم)  /*جستجوی دودویی*/{

        int32_t mid = (hi+کم) >> 1; /*Finds median*/

        إذا (mid == oldmid)
            اكسر;

        إذا (esc_Seq_State_جدول_کلید_2022[mid] > togo){
            hi = mid;
        }
        وإلا إذا (esc_Seq_State_جدول_کلید_2022[mid] < togo){
            کم = mid;
        }
        وإلا /*we found it*/{
            *کلید = togo;
            *جابه_جایی = mid;
            أرجع static_cast<UCNV_جدول_حالتها_2022>(esc_Seq_State_جدول_مقدار_2022[mid]);
        }
        oldmid = mid;

    }

    *کلید = 0;
    *جابه_جایی = 0;
    أرجع INVALID_2022;
}

/*runs through a state machine to determine the escape sequence - codepage correspondence
 */
ساكن فراغ
changeState_2022(UConverter* _this,
                ثابت حرف** منبع,
                ثابت حرف* منبع_حد,
                Variant2022 var,
                UError_کد* err){
    UCNV_جدول_حالتها_2022 value;
    UConverter_داده_ISO2022* myData2022 = static_cast<UConverter_داده_ISO2022*>(_this->extraInfo);
    uint32_t کلید = myData2022->کلید;
    int32_t جابه_جایی = 0;
    int8_t initial_مقصد_ULength = _this->مقصد_ULength;
    حرف c;

    value = VALID_NON_TERMINAL_2022;
    طالما (*منبع < منبع_حد) {
        c = *(*منبع)++;
        _this->مقصد_UBytes[_this->مقصد_ULength++] = static_cast<uint8_t>(c);
        value = get_کلید_2022(c, reinterpret_cast<int32_t*>(&کلید), &جابه_جایی);

        بدل (value){

        حالة VALID_NON_TERMINAL_2022 :
            /* continue with the loop */
            اكسر;

        حالة VALID_TERMINAL_2022:
            کلید = 0;
            اذهب انجامشده_2;

        حالة INVALID_2022:
            اذهب انجامشده_2;

        حالة VALID_MAYBE_TERMINAL_2022:
#اگر_تعریف_شده U_ENABLE_GENERIC_ISO_2022
            /* ESC ( B is ambiguous only for ISO_2022 itself */
            إذا(var == ISO_2022) {
                /* discard toUBytes[] for ESC ( B because this sequence is correct and complete */
                _this->مقصد_ULength = 0;

                /* TODO need to indicate that ESC ( B was seen; if failure, then need to replay from source or from MBCS-style replay */

                /* continue with the loop */
                value = VALID_NON_TERMINAL_2022;
                اكسر;
            } وإلا
#پایان_اگر
            {
                /* not ISO_2022 itself, finish here */
                value = VALID_TERMINAL_2022;
                کلید = 0;
                اذهب انجامشده_2;
            }
        }
    }

انجامشده_2:
    myData2022->کلید = کلید;

    إذا (value == VALID_NON_TERMINAL_2022) {
        /* indicate that the escape sequence is incomplete: key!=0 */
        أرجع;
    } وإلا إذا (value == INVALID_2022 ) {
        *err = U_ILLEGAL_ESCAPE_SEQUENCE;
    } وإلا /* value == VALID_TERMINAL_2022 */ {
        بدل(var){
#اگر_تعریف_شده U_ENABLE_GENERIC_ISO_2022
        حالة ISO_2022:
        {
            ثابت حرف *chosenConverterName = esc_Seq_State_جدول_نتیجه_2022[جابه_جایی];
            إذا(chosenConverterName == اشاره_تهی) {
                /* SS2 or SS3 */
                *err = U_UNSUPPORTED_ESCAPE_SEQUENCE;
                _this->مقصد_UCallback_Reason = UCNV_UNASSIGNED;
                أرجع;
            }

            _this->نما = UCNV_SI;
            ucnv_close(myData2022->currentConverter);
            myData2022->currentConverter = myUConverter = ucnv_open(chosenConverterName, err);
            إذا(U_SUCCESS(*err)) {
                myUConverter->fromCharErrorBehaviour = UCNV_مقصد_U_CALLBACK_STOP;
                _this->نما = UCNV_SO;
            }
            اكسر;
        }
#پایان_اگر
        حالة ISO_2022_JP:
            {
                StateEnum موقت_State = static_cast<StateEnum>(بعدی_State_مقصد_Unicode_JP[جابه_جایی]);
                بدل(موقت_State) {
                حالة INVALID_STATE:
                    *err = U_UNSUPPORTED_ESCAPE_SEQUENCE;
                    اكسر;
                حالة SS2_STATE:
                    إذا(myData2022->مقصد_U2022_State.cs[2]!=0) {
                        إذا(myData2022->مقصد_U2022_State.g<2) {
                            myData2022->مقصد_U2022_State.prevG=myData2022->مقصد_U2022_State.g;
                        }
                        myData2022->مقصد_U2022_State.g=2;
                    } وإلا {
                        /* illegal to have SS2 before a matching designator */
                        *err = U_ILLEGAL_ESCAPE_SEQUENCE;
                    }
                    اكسر;
                /* case SS3_STATE: not used in ISO-2022-JP-x */
                حالة ISO8859_1:
                حالة ISO8859_7:
                    إذا((jpCharsetMasks[myData2022->نسخه] & CSM(موقت_State)) == 0) {
                        *err = U_UNSUPPORTED_ESCAPE_SEQUENCE;
                    } وإلا {
                        /* G2 charset for SS2 */
                        myData2022->مقصد_U2022_State.cs[2] = static_cast<int8_t>(موقت_State);
                    }
                    اكسر;
                افتراضي:
                    إذا((jpCharsetMasks[myData2022->نسخه] & CSM(موقت_State)) == 0) {
                        *err = U_UNSUPPORTED_ESCAPE_SEQUENCE;
                    } وإلا {
                        /* G0 charset */
                        myData2022->مقصد_U2022_State.cs[0] = static_cast<int8_t>(موقت_State);
                    }
                    اكسر;
                }
            }
            اكسر;
#إذا !UCONFIG_ONLY_HTML_CONVERSION
        حالة ISO_2022_CN:
            {
                StateEnum موقت_State = static_cast<StateEnum>(بعدی_State_مقصد_Unicode_CN[جابه_جایی]);
                بدل(موقت_State) {
                حالة INVALID_STATE:
                    *err = U_UNSUPPORTED_ESCAPE_SEQUENCE;
                    اكسر;
                حالة SS2_STATE:
                    إذا(myData2022->مقصد_U2022_State.cs[2]!=0) {
                        إذا(myData2022->مقصد_U2022_State.g<2) {
                            myData2022->مقصد_U2022_State.prevG=myData2022->مقصد_U2022_State.g;
                        }
                        myData2022->مقصد_U2022_State.g=2;
                    } وإلا {
                        /* illegal to have SS2 before a matching designator */
                        *err = U_ILLEGAL_ESCAPE_SEQUENCE;
                    }
                    اكسر;
                حالة SS3_STATE:
                    إذا(myData2022->مقصد_U2022_State.cs[3]!=0) {
                        إذا(myData2022->مقصد_U2022_State.g<2) {
                            myData2022->مقصد_U2022_State.prevG=myData2022->مقصد_U2022_State.g;
                        }
                        myData2022->مقصد_U2022_State.g=3;
                    } وإلا {
                        /* illegal to have SS3 before a matching designator */
                        *err = U_ILLEGAL_ESCAPE_SEQUENCE;
                    }
                    اكسر;
                حالة ISO_IR_165:
                    إذا(myData2022->نسخه==0) {
                        *err = U_UNSUPPORTED_ESCAPE_SEQUENCE;
                        اكسر;
                    }
                    U_FALLTHROUGH;
                حالة GB2312_1:
                    U_FALLTHROUGH;
                حالة CNS_11643_1:
                    myData2022->مقصد_U2022_State.cs[1] = static_cast<int8_t>(موقت_State);
                    اكسر;
                حالة CNS_11643_2:
                    myData2022->مقصد_U2022_State.cs[2] = static_cast<int8_t>(موقت_State);
                    اكسر;
                افتراضي:
                    /* other CNS 11643 planes */
                    إذا(myData2022->نسخه==0) {
                        *err = U_UNSUPPORTED_ESCAPE_SEQUENCE;
                    } وإلا {
                        myData2022->مقصد_U2022_State.cs[3] = static_cast<int8_t>(موقت_State);
                    }
                    اكسر;
                }
            }
            اكسر;
        حالة ISO_2022_KR:
            إذا(جابه_جایی==0x30){
                /* nothing to be done, just accept this one escape sequence */
            } وإلا {
                *err = U_UNSUPPORTED_ESCAPE_SEQUENCE;
            }
            اكسر;
#پایان_اگر  // !UCONFIG_ONLY_HTML_CONVERSION

        افتراضي:
            *err = U_ILLEGAL_ESCAPE_SEQUENCE;
            اكسر;
        }
    }
    إذا(U_SUCCESS(*err)) {
        _this->مقصد_ULength = 0;
    } وإلا إذا(*err==U_ILLEGAL_ESCAPE_SEQUENCE) {
        إذا(_this->مقصد_ULength>1) {
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
            int8_t پشت_بیرون_فاصله=_this->مقصد_ULength-1;
            int8_t بایتها_From_This_میانگیر=_this->مقصد_ULength-initial_مقصد_ULength;
            إذا(پشت_بیرون_فاصله<=بایتها_From_This_میانگیر) {
                /* same as initialToULength<=1 */
                *منبع-=پشت_بیرون_فاصله;
            } وإلا {
                /* Back out bytes from the previous buffer: Need to replay them. */
                _this->pre_مقصد_ULength = static_cast<int8_t>(بایتها_From_This_میانگیر - پشت_بیرون_فاصله);
                /* same as -(initialToULength-1) */
                /* preToULength is negative! */
                uprv_memcpy(_this->pre_مقصد_U, _this->مقصد_UBytes+1, -_this->pre_مقصد_ULength);
                *منبع-=بایتها_From_This_میانگیر;
            }
            _this->مقصد_ULength=1;
        }
    } وإلا إذا(*err==U_UNSUPPORTED_ESCAPE_SEQUENCE) {
        _this->مقصد_UCallback_Reason = UCNV_UNASSIGNED;
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
get_انتها_از_میانگیر_2022(ثابت حرف** منبع,
                   ثابت حرف* منبع_حد,
                   UBool /*flush*/){

    ثابت حرف* my_منبع = *منبع;

#اگر_تعریف_شده U_ENABLE_GENERIC_ISO_2022
    إذا (*منبع >= منبع_حد)
        أرجع منبع_حد;

    افعل{

        إذا (*my_منبع == ESC_2022){
            int8_t i;
            int32_t کلید = 0;
            int32_t جابه_جایی;
            UCNV_جدول_حالتها_2022 value = VALID_NON_TERMINAL_2022;

            /* Kludge: I could not
            * figure out the reason for validating an escape sequence
            * twice - once here and once in changeState_2022().
            * is it possible to have an ESC character in a ISO2022
            * byte stream which is valid in a code page? Is it legal?
            */
            لكل (i=0;
            (my_منبع+i < منبع_حد)&&(value == VALID_NON_TERMINAL_2022);
            i++) {
                value =  get_کلید_2022(*(my_منبع+i), &کلید, &جابه_جایی);
            }
            إذا (value > 0 || *my_منبع==ESC_2022)
                أرجع my_منبع;

            إذا ((value == VALID_NON_TERMINAL_2022)&&(!flush) )
                أرجع منبع_حد;
        }
    }طالما (++my_منبع < منبع_حد);

    أرجع منبع_حد;
#وإلا
    طالما(my_منبع < منبع_حد && *my_منبع != ESC_2022) {
        ++my_منبع;
    }
    أرجع my_منبع;
#پایان_اگر
}
#پایان_اگر

/* This inline function replicates code in _MBCSFromUChar32() function in ucnvmbcs.c
 * any future change in _MBCSFromUChar32() function should be reflected here.
 * @return number of bytes in *value; negative number if fallback; 0 if no mapping
 */
ساكن مضمن int32_t
MBCS_FROM_UCHAR32_ISO2022(UConverter_Shared_داده* shared_داده,
                                         UChar32 c,
                                         uint32_t* value,
                                         UBool useFallback,
                                         صحيح outputType)
{
    ثابت int32_t *cx;
    ثابت uint16_t *جدول;
    uint32_t stage2_مدخل;
    uint32_t my_مقدار;
    int32_t length;
    ثابت uint8_t *p;
    /*
     * TODO(markus): Use and require new, faster MBCS conversion table structures.
     * Use internal version of ucnv_open() that verifies that the new structures are available,
     * else U_INTERNAL_PROGRAM_ERROR.
     */
    /* BMP-only codepages are stored without stage 1 entries for supplementary code points */
    إذا(c<0x10000 || (shared_داده->mbcs.unicode_ماسک&UCNV_HAS_SUPPLEMENTARY)) {
        جدول=shared_داده->mbcs.from_Unicode_جدول;
        stage2_مدخل=MBCS_STAGE_2_FROM_U(جدول, c);
        /* get the bytes and the length for the output */
        إذا(outputType==MBCS_OUTPUT_2){
            my_مقدار=MBCS_مقدار_2_FROM_STAGE_2(shared_داده->mbcs.from_Unicode_بایتها, stage2_مدخل, c);
            إذا(my_مقدار<=0xff) {
                length=1;
            } وإلا {
                length=2;
            }
        } وإلا /* outputType==MBCS_OUTPUT_3 */ {
            p=MBCS_اشاره_گر_3_FROM_STAGE_2(shared_داده->mbcs.from_Unicode_بایتها, stage2_مدخل, c);
            my_مقدار = (static_cast<uint32_t>(*p) << 16) | (static_cast<uint32_t>(p[1]) << 8) | p[2];
            إذا(my_مقدار<=0xff) {
                length=1;
            } وإلا إذا(my_مقدار<=0xffff) {
                length=2;
            } وإلا {
                length=3;
            }
        }
        /* is this code point assigned, or do we use fallbacks? */
        إذا((stage2_مدخل&(1<<(16+(c&0xf))))!=0) {
            /* assigned */
            *value=my_مقدار;
            أرجع length;
        } وإلا إذا(FROM_U_USE_FALLBACK(useFallback, c) && my_مقدار!=0) {
            /*
             * We allow a 0 byte output if the "assigned" bit is set for this entry.
             * There is no way with this data structure for fallback output
             * to be a zero byte.
             */
            *value=my_مقدار;
            أرجع -length;
        }
    }

    cx=shared_داده->mbcs.extIndexes;
    إذا(cx!=اشاره_تهی) {
        أرجع ucnv_ext_Simple_تطابق_From_U(cx, c, value, useFallback);
    }

    /* unassigned */
    أرجع 0;
}

/* This inline function replicates code in _MBCSSingleFromUChar32() function in ucnvmbcs.c
 * any future change in _MBCSSingleFromUChar32() function should be reflected here.
 * @param retval pointer to output byte
 * @return 1 roundtrip byte  0 no mapping  -1 fallback byte
 */
ساكن مضمن int32_t
MBCS_SINGLE_FROM_UCHAR32(UConverter_Shared_داده* shared_داده,
                                       UChar32 c,
                                       uint32_t* retval,
                                       UBool useFallback)
{
    ثابت uint16_t *جدول;
    int32_t value;
    /* BMP-only codepages are stored without stage 1 entries for supplementary code points */
    إذا(c>=0x10000 && !(shared_داده->mbcs.unicode_ماسک&UCNV_HAS_SUPPLEMENTARY)) {
        أرجع 0;
    }
    /* convert the Unicode code point in c into codepage bytes (same as in _MBCSFromUnicodeWithOffsets) */
    جدول=shared_داده->mbcs.from_Unicode_جدول;
    /* get the byte for the output */
    value=MBCS_SINGLE_نتیجه_FROM_U(جدول, (uint16_t *)shared_داده->mbcs.from_Unicode_بایتها, c);
    /* is this code point assigned, or do we use fallbacks? */
    *retval = static_cast<uint32_t>(value & 0xff);
    إذا(value>=0xf00) {
        أرجع 1;  /* roundtrip */
    } وإلا إذا(useFallback ? value>=0x800 : value>=0xc00) {
        أرجع -1;  /* fallback taken */
    } وإلا {
        أرجع 0;  /* no mapping */
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
        أرجع value - 0x8080;  /* shift down to 21..7e byte range */
    } وإلا {
        أرجع 0;  /* not valid for ISO 2022 */
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
    uint32_t return_مقدار = value + 0x8080;
    إذا( (uint16_t)(return_مقدار - 0xa1a1) <= (0xfefe - 0xa1a1) &&
        (uint8_t)(return_مقدار - 0xa1) <= (0xfe - 0xa1)) {
        أرجع return_مقدار;
    } وإلا {
        أرجع value;
    }
}
#پایان_اگر

#اگر_تعریف_شده U_ENABLE_GENERIC_ISO_2022

/**********************************************************************************
*  ISO-2022 Converter
*
*
*/

ساكن فراغ U_CALLCONV
T_UConverter_مقصد_Unicode_ISO_2022_OFFSETS_LOGIC(UConverter_مقصد_Unicode_آرگومانها* آرگومانها_2,
                                                           UError_کد* err){
    ثابت حرف* my_منبع_حد, *real_منبع_حد;
    ثابت حرف* منبع_شروع;
    ثابت char16_t* my_هدف_شروع;
    UConverter* saveThis;
    UConverter_داده_ISO2022* my_داده;
    int8_t length;

    saveThis = آرگومانها_2->converter;
    my_داده=((UConverter_داده_ISO2022*)(saveThis->extraInfo));

    real_منبع_حد = آرگومانها_2->منبع_حد;
    طالما (آرگومانها_2->منبع < real_منبع_حد) {
        إذا(my_داده->کلید == 0) { /* are we in the middle of an escape sequence? */
            /*Find the end of the buffer e.g : Next Escape Seq | end of Buffer*/
            my_منبع_حد = get_انتها_از_میانگیر_2022(&(آرگومانها_2->منبع), real_منبع_حد, آرگومانها_2->flush);

            إذا(آرگومانها_2->منبع < my_منبع_حد) {
                إذا(my_داده->currentConverter==اشاره_تهی) {
                    my_داده->currentConverter = ucnv_open("ASCII",err);
                    إذا(U_FAILURE(*err)){
                        أرجع;
                    }

                    my_داده->currentConverter->fromCharErrorBehaviour = UCNV_مقصد_U_CALLBACK_STOP;
                    saveThis->نما = UCNV_SO;
                }

                /* convert to before the ESC or until the end of the buffer */
                my_داده->is_اولی_میانگیر=خطأ;
                منبع_شروع = آرگومانها_2->منبع;
                my_هدف_شروع = آرگومانها_2->هدف;
                آرگومانها_2->converter = my_داده->currentConverter;
                ucnv_مقصد_Unicode(آرگومانها_2->converter,
                    &آرگومانها_2->هدف,
                    آرگومانها_2->هدف_حد,
                    &آرگومانها_2->منبع,
                    my_منبع_حد,
                    آرگومانها_2->offsets,
                    (UBool)(آرگومانها_2->flush && my_منبع_حد == real_منبع_حد),
                    err);
                آرگومانها_2->converter = saveThis;

                إذا (*err == U_میانگیر_سرریز_ERROR) {
                    /* move the overflow buffer */
                    length = saveThis->UChar_Error_میانگیر_درازی = my_داده->currentConverter->UChar_Error_میانگیر_درازی;
                    my_داده->currentConverter->UChar_Error_میانگیر_درازی = 0;
                    إذا(length > 0) {
                        uprv_memcpy(saveThis->UChar_Error_میانگیر,
                                    my_داده->currentConverter->UChar_Error_میانگیر,
                                    length*U_SIZEOF_UCHAR);
                    }
                    أرجع;
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
                    (آرگومانها_2->منبع == real_منبع_حد) ||
                    (آرگومانها_2->offsets != اشاره_تهی && (آرگومانها_2->هدف != my_هدف_شروع || آرگومانها_2->منبع != منبع_شروع) ||
                    (my_منبع_حد < real_منبع_حد && my_داده->currentConverter->مقصد_ULength > 0))
                ) {
                    /* copy partial or error input for truncated detection and error handling */
                    إذا(U_FAILURE(*err)) {
                        length = saveThis->invalid_Char_درازی = my_داده->currentConverter->invalid_Char_درازی;
                        إذا(length > 0) {
                            uprv_memcpy(saveThis->invalid_Char_میانگیر, my_داده->currentConverter->invalid_Char_میانگیر, length);
                        }
                    } وإلا {
                        length = saveThis->مقصد_ULength = my_داده->currentConverter->مقصد_ULength;
                        إذا(length > 0) {
                            uprv_memcpy(saveThis->مقصد_UBytes, my_داده->currentConverter->مقصد_UBytes, length);
                            إذا(آرگومانها_2->منبع < my_منبع_حد) {
                                *err = U_TRUNCATED_CHAR_یافت; /* truncated input before ESC */
                            }
                        }
                    }
                    أرجع;
                }
            }
        }

        منبع_شروع = آرگومانها_2->منبع;
        changeState_2022(آرگومانها_2->converter,
               &(آرگومانها_2->منبع),
               real_منبع_حد,
               ISO_2022,
               err);
        إذا (U_FAILURE(*err) || (آرگومانها_2->منبع != منبع_شروع && آرگومانها_2->offsets != اشاره_تهی)) {
            /* let the ucnv.c code update its current offset */
            أرجع;
        }
    }
}

#پایان_اگر

/*
 * To Unicode Callback helper function
 */
ساكن فراغ
مقصد_Unicode_Callback(UConverter *cnv,
                  ثابت uint32_t منبع_Char, ثابت uint32_t هدف_Uni_Char,
                  UError_کد* err){
    إذا(منبع_Char>0xff){
        cnv->مقصد_UBytes[0] = static_cast<uint8_t>(منبع_Char >> 8);
        cnv->مقصد_UBytes[1] = static_cast<uint8_t>(منبع_Char);
        cnv->مقصد_ULength = 2;
    }
    وإلا{
        cnv->مقصد_UBytes[0] = static_cast<حرف>(منبع_Char);
        cnv->مقصد_ULength = 1;
    }

    إذا(هدف_Uni_Char == (missingCharMarker-1/*0xfffe*/)){
        *err = U_INVALID_CHAR_یافت;
    }
    وإلا{
        *err = U_ILLEGAL_CHAR_یافت;
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
    HWKANA_7_بیت
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
jisx201_مقصد_U(uint32_t value) {
    إذا(value < 0x5c) {
        أرجع value;
    } وإلا إذا(value == 0x5c) {
        أرجع 0xa5;
    } وإلا إذا(value == 0x7e) {
        أرجع 0x203e;
    } وإلا /* value <= 0x7f */ {
        أرجع value;
    }
}

/* Map Unicode to 00..7F according to JIS X 0201. Return U+FFFE if unmappable. */
ساكن مضمن uint32_t
jisx201FromU(uint32_t value) {
    إذا(value<=0x7f) {
        إذا(value!=0x5c && value!=0x7e) {
            أرجع value;
        }
    } وإلا إذا(value==0xa5) {
        أرجع 0x5c;
    } وإلا إذا(value==0x203e) {
        أرجع 0x7e;
    }
    أرجع 0xfffe;
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
        أرجع 0;  /* beyond JIS X 0208 */
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
    أرجع value;
}

/*
 * Convert a pair of JIS X 0208 21..7E bytes to Shift-JIS.
 * If either byte is outside 21..7E make sure that the result is not valid
 * for Shift-JIS so that the converter catches it.
 * Some invalid byte values already turn into equally invalid Shift-JIS
 * byte values and need not be tested explicitly.
 */
ساكن مضمن فراغ
_2022ToSJIS(uint8_t c1, uint8_t c2, حرف بایتها[2]) {
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
    بایتها[0] = static_cast<حرف>(c1);
    بایتها[1] = static_cast<حرف>(c2);
}

/*
 * JIS X 0208 has fallbacks from Unicode half-width Katakana to full-width (DBCS)
 * Katakana.
 * Now that we use a Shift-JIS table for JIS X 0208 we need to hardcode these fallbacks
 * because Shift-JIS roundtrips half-width Katakana to single bytes.
 * These were the only fallbacks in ICU's jisx-208.ucm file.
 */
ساكن ثابت uint16_t hwkana_fb[HWKANA_انتها - HWKANA_شروع + 1] = {
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
UConverter_fromUnicode_ISO_2022_JP_OFFSETS_LOGIC(UConverter_From_Unicode_آرگومانها* آرگومانها_2, UError_کد* err) {
    UConverter *cnv = آرگومانها_2->converter;
    UConverter_داده_ISO2022 *converter_داده;
    ISO2022State *pFromU2022State;
    uint8_t* هدف = reinterpret_cast<uint8_t*>(آرگومانها_2->هدف);
    ثابت uint8_t* هدف_حد = reinterpret_cast<ثابت uint8_t*>(آرگومانها_2->هدف_حد);
    ثابت char16_t* منبع = آرگومانها_2->منبع;
    ثابت char16_t* منبع_حد = آرگومانها_2->منبع_حد;
    int32_t* offsets = آرگومانها_2->offsets;
    UChar32 منبع_Char;
    حرف میانگیر[8];
    int32_t len, بیرون_Len;
    int8_t choices[10];
    int32_t choice_شمار;
    uint32_t هدف_مقدار = 0;
    UBool useFallback;

    int32_t i;
    int8_t cs, g;

    /* set up the state */
    converter_داده = static_cast<UConverter_داده_ISO2022*>(cnv->extraInfo);
    pFromU2022State   = &converter_داده->fromU2022State;

    choice_شمار = 0;

    /* check if the last codepoint of previous buffer was a lead surrogate*/
    إذا((منبع_Char = cnv->fromUChar32)!=0 && هدف< هدف_حد) {
        اذهب getTrail;
    }

    طالما(منبع < منبع_حد) {
        إذا(هدف < هدف_حد) {

            منبع_Char  = *(منبع++);
            /*check if the char is a First surrogate*/
            إذا(U16_IS_SURROGATE(منبع_Char)) {
                إذا(U16_IS_SURROGATE_LEAD(منبع_Char)) {
getTrail:
                    /*look ahead to find the trail surrogate*/
                    إذا(منبع < منبع_حد) {
                        /* test the following code unit */
                        char16_t trail = *منبع;
                        إذا(U16_IS_TRAIL(trail)) {
                            منبع++;
                            منبع_Char=U16_GET_SUPPLEMENTARY(منبع_Char, trail);
                            cnv->fromUChar32=0x00;
                            /* convert this supplementary code point */
                            /* exit this condition tree */
                        } وإلا {
                            /* this is an unmatched lead code unit (1st surrogate) */
                            /* callback(illegal) */
                            *err=U_ILLEGAL_CHAR_یافت;
                            cnv->fromUChar32=منبع_Char;
                            اكسر;
                        }
                    } وإلا {
                        /* no more input */
                        cnv->fromUChar32=منبع_Char;
                        اكسر;
                    }
                } وإلا {
                    /* this is an unmatched trail code unit (2nd surrogate) */
                    /* callback(illegal) */
                    *err=U_ILLEGAL_CHAR_یافت;
                    cnv->fromUChar32=منبع_Char;
                    اكسر;
                }
            }

            /* do not convert SO/SI/ESC */
            إذا(IS_2022_CONTROL(منبع_Char)) {
                /* callback(illegal) */
                *err=U_ILLEGAL_CHAR_یافت;
                cnv->fromUChar32=منبع_Char;
                اكسر;
            }

            /* do the conversion */

            إذا(choice_شمار == 0) {
                uint16_t csm;

                /*
                 * The csm variable keeps track of which charsets are allowed
                 * and not used yet while building the choices[].
                 */
                csm = jpCharsetMasks[converter_داده->نسخه];
                choice_شمار = 0;

                /* JIS7/8: try single-byte half-width Katakana before JISX208 */
                إذا(converter_داده->نسخه == 3 || converter_داده->نسخه == 4) {
                    choices[choice_شمار++] = static_cast<int8_t>(HWKANA_7_بیت);
                }
                /* Do not try single-byte half-width Katakana for other versions. */
                csm &= ~CSM(HWKANA_7_بیت);

                /* try the current G0 charset */
                choices[choice_شمار++] = cs = pFromU2022State->cs[0];
                csm &= ~CSM(cs);

                /* try the current G2 charset */
                إذا((cs = pFromU2022State->cs[2]) != 0) {
                    choices[choice_شمار++] = cs;
                    csm &= ~CSM(cs);
                }

                /* try all the other possible charsets */
                لكل(i = 0; i < UPRV_LENGTHOF(jpCharsetPref); ++i) {
                    cs = static_cast<int8_t>(jpCharsetPref[i]);
                    إذا(CSM(cs) & csm) {
                        choices[choice_شمار++] = cs;
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

            لكل(i = 0; i < choice_شمار && len <= 0; ++i) {
                uint32_t value;
                int32_t len2;
                int8_t cs0 = choices[i];
                بدل(cs0) {
                حالة ASCII:
                    إذا(منبع_Char <= 0x7f) {
                        هدف_مقدار = static_cast<uint32_t>(منبع_Char);
                        len = 1;
                        cs = cs0;
                        g = 0;
                    }
                    اكسر;
                حالة ISO8859_1:
                    إذا(GR96_شروع <= منبع_Char && منبع_Char <= GR96_انتها) {
                        هدف_مقدار = static_cast<uint32_t>(منبع_Char) - 0x80;
                        len = 1;
                        cs = cs0;
                        g = 2;
                    }
                    اكسر;
                حالة HWKANA_7_بیت:
                    إذا (static_cast<uint32_t>(منبع_Char - HWKANA_شروع) <= (HWKANA_انتها - HWKANA_شروع)) {
                        إذا(converter_داده->نسخه==3) {
                            /* JIS7: use G1 (SO) */
                            /* Shift U+FF61..U+FF9F to bytes 21..5F. */
                            هدف_مقدار = static_cast<uint32_t>(منبع_Char - (HWKANA_شروع - 0x21));
                            len = 1;
                            pFromU2022State->cs[1] = cs = cs0; /* do not output an escape sequence */
                            g = 1;
                        } وإلا إذا(converter_داده->نسخه==4) {
                            /* JIS8: use 8-bit bytes with any single-byte charset, see escape sequence output below */
                            /* Shift U+FF61..U+FF9F to bytes A1..DF. */
                            هدف_مقدار = static_cast<uint32_t>(منبع_Char - (HWKANA_شروع - 0xa1));
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
                    value = jisx201FromU(منبع_Char);
                    إذا(value <= 0x7f) {
                        هدف_مقدار = value;
                        len = 1;
                        cs = cs0;
                        g = 0;
                        useFallback = خطأ;
                    }
                    اكسر;
                حالة JISX208:
                    /* G0 DBCS from Shift-JIS table */
                    len2 = MBCS_FROM_UCHAR32_ISO2022(
                                converter_داده->myConverterArray[cs0],
                                منبع_Char, &value,
                                useFallback, MBCS_OUTPUT_2);
                    إذا(len2 == 2 || (len2 == -2 && len == 0)) {  /* only accept DBCS: abs(len)==2 */
                        value = _2022FromSJIS(value);
                        إذا(value != 0) {
                            هدف_مقدار = value;
                            len = len2;
                            cs = cs0;
                            g = 0;
                            useFallback = خطأ;
                        }
                    } وإلا إذا(len == 0 && useFallback &&
                              static_cast<uint32_t>(منبع_Char - HWKANA_شروع) <= (HWKANA_انتها - HWKANA_شروع)) {
                        هدف_مقدار = hwkana_fb[منبع_Char - HWKANA_شروع];
                        len = -2;
                        cs = cs0;
                        g = 0;
                        useFallback = خطأ;
                    }
                    اكسر;
                حالة ISO8859_7:
                    /* G0 SBCS forced to 7-bit output */
                    len2 = MBCS_SINGLE_FROM_UCHAR32(
                                converter_داده->myConverterArray[cs0],
                                منبع_Char, &value,
                                useFallback);
                    إذا(len2 != 0 && !(len2 < 0 && len != 0) && GR96_شروع <= value && value <= GR96_انتها) {
                        هدف_مقدار = value - 0x80;
                        len = len2;
                        cs = cs0;
                        g = 2;
                        useFallback = خطأ;
                    }
                    اكسر;
                افتراضي:
                    /* G0 DBCS */
                    len2 = MBCS_FROM_UCHAR32_ISO2022(
                                converter_داده->myConverterArray[cs0],
                                منبع_Char, &value,
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
                        هدف_مقدار = value;
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
                بیرون_Len = 0; /* count output bytes */

                /* write SI if necessary (only for JIS7) */
                إذا(pFromU2022State->g == 1 && g == 0) {
                    میانگیر[بیرون_Len++] = UCNV_SI;
                    pFromU2022State->g = 0;
                }

                /* write the designation sequence if necessary */
                إذا(cs != pFromU2022State->cs[g]) {
                    int32_t escLen = escSeqCharsLen[cs];
                    uprv_memcpy(میانگیر + بیرون_Len, escSeqChars[cs], escLen);
                    بیرون_Len += escLen;
                    pFromU2022State->cs[g] = cs;

                    /* invalidate the choices[] */
                    choice_شمار = 0;
                }

                /* write the shift sequence if necessary */
                إذا(g != pFromU2022State->g) {
                    بدل(g) {
                    /* case 0 handled before writing escapes */
                    حالة 1:
                        میانگیر[بیرون_Len++] = UCNV_SO;
                        pFromU2022State->g = 1;
                        اكسر;
                    افتراضي: /* case 2 */
                        میانگیر[بیرون_Len++] = 0x1b;
                        میانگیر[بیرون_Len++] = 0x4e;
                        اكسر;
                    /* no case 3: no SS3 in ISO-2022-JP-x */
                    }
                }

                /* write the output bytes */
                إذا(len == 1) {
                    میانگیر[بیرون_Len++] = static_cast<حرف>(هدف_مقدار);
                } وإلا /* len == 2 */ {
                    میانگیر[بیرون_Len++] = static_cast<حرف>(هدف_مقدار >> 8);
                    میانگیر[بیرون_Len++] = static_cast<حرف>(هدف_مقدار);
                }
            } وإلا {
                /*
                 * if we cannot find the character after checking all codepages
                 * then this is an error
                 */
                *err = U_INVALID_CHAR_یافت;
                cnv->fromUChar32=منبع_Char;
                اكسر;
            }

            إذا(منبع_Char == CR || منبع_Char == LF) {
                /* reset the G2 state at the end of a line (conversion got us into ASCII or JISX201 already) */
                pFromU2022State->cs[2] = 0;
                choice_شمار = 0;
            }

            /* output outLen>0 bytes in buffer[] */
            إذا(بیرون_Len == 1) {
                *هدف++ = میانگیر[0];
                إذا(offsets) {
                    *offsets++ = static_cast<int32_t>(منبع - آرگومانها_2->منبع - 1); /* -1: known to be ASCII */
                }
            } وإلا إذا(بیرون_Len == 2 && (هدف + 2) <= هدف_حد) {
                *هدف++ = میانگیر[0];
                *هدف++ = میانگیر[1];
                إذا(offsets) {
                    int32_t منبع_شاخص = static_cast<int32_t>(منبع - آرگومانها_2->منبع - U16_درازی(منبع_Char));
                    *offsets++ = منبع_شاخص;
                    *offsets++ = منبع_شاخص;
                }
            } وإلا {
                fromUWriteUInt8(
                    cnv,
                    میانگیر, بیرون_Len,
                    &هدف, reinterpret_cast<ثابت حرف*>(هدف_حد),
                    &offsets, static_cast<int32_t>(منبع - آرگومانها_2->منبع - U16_درازی(منبع_Char)),
                    err);
                إذا(U_FAILURE(*err)) {
                    اكسر;
                }
            }
        } /* end if(myTargetIndex<myTargetLength) */
        وإلا{
            *err =U_میانگیر_سرریز_ERROR;
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
        آرگومانها_2->flush && منبع>=منبع_حد && cnv->fromUChar32==0
    ) {
        int32_t منبع_شاخص;

        بیرون_Len = 0;

        إذا(pFromU2022State->g != 0) {
            میانگیر[بیرون_Len++] = UCNV_SI;
            pFromU2022State->g = 0;
        }

        إذا(pFromU2022State->cs[0] != ASCII) {
            int32_t escLen = escSeqCharsLen[ASCII];
            uprv_memcpy(میانگیر + بیرون_Len, escSeqChars[ASCII], escLen);
            بیرون_Len += escLen;
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
        منبع_شاخص = static_cast<int32_t>(منبع - آرگومانها_2->منبع);
        إذا(منبع_شاخص>0) {
            --منبع_شاخص;
            إذا( U16_IS_TRAIL(آرگومانها_2->منبع[منبع_شاخص]) &&
                (منبع_شاخص==0 || U16_IS_LEAD(آرگومانها_2->منبع[منبع_شاخص-1]))
            ) {
                --منبع_شاخص;
            }
        } وإلا {
            منبع_شاخص=-1;
        }

        fromUWriteUInt8(
            cnv,
            میانگیر, بیرون_Len,
            &هدف, reinterpret_cast<ثابت حرف*>(هدف_حد),
            &offsets, منبع_شاخص,
            err);
    }

    /*save the state and return */
    آرگومانها_2->منبع = منبع;
    آرگومانها_2->هدف = reinterpret_cast<حرف*>(هدف);
}

/*************** to unicode *******************/

ساكن فراغ U_CALLCONV
UConverter_مقصد_Unicode_ISO_2022_JP_OFFSETS_LOGIC(UConverter_مقصد_Unicode_آرگومانها *آرگومانها_2,
                                               UError_کد* err){
    حرف موقت_Buf[2];
    ثابت حرف* my_منبع = const_cast<حرف*>(آرگومانها_2->منبع);
    char16_t *my_هدف = آرگومانها_2->هدف;
    ثابت حرف *my_منبع_حد = آرگومانها_2->منبع_حد;
    uint32_t هدف_Uni_Char = 0x0000;
    uint32_t my_منبع_Char = 0x0000;
    uint32_t tmp_منبع_Char = 0x0000;
    UConverter_داده_ISO2022* my_داده;
    ISO2022State *p_مقصد_U2022_State;
    StateEnum cs;

    my_داده = static_cast<UConverter_داده_ISO2022*>(آرگومانها_2->converter->extraInfo);
    p_مقصد_U2022_State = &my_داده->مقصد_U2022_State;

    إذا(my_داده->کلید != 0) {
        /* continue with a partial escape sequence */
        اذهب escape;
    } وإلا إذا(آرگومانها_2->converter->مقصد_ULength == 1 && my_منبع < my_منبع_حد && my_هدف < آرگومانها_2->هدف_حد) {
        /* continue with a partial double-byte character */
        my_منبع_Char = آرگومانها_2->converter->مقصد_UBytes[0];
        آرگومانها_2->converter->مقصد_ULength = 0;
        cs = static_cast<StateEnum>(p_مقصد_U2022_State->cs[p_مقصد_U2022_State->g]);
        هدف_Uni_Char = missingCharMarker;
        اذهب getTrailByte;
    }

    طالما(my_منبع < my_منبع_حد){

        هدف_Uni_Char =missingCharMarker;

        إذا(my_هدف < آرگومانها_2->هدف_حد){

            my_منبع_Char = static_cast<بیعلامت حرف>(*my_منبع++);

            بدل(my_منبع_Char) {
            حالة UCNV_SI:
                إذا(my_داده->نسخه==3) {
                    p_مقصد_U2022_State->g=0;
                    استمر;
                } وإلا {
                    /* only JIS7 uses SI/SO, not ISO-2022-JP-x */
                    my_داده->is_خالی_Segment = خطأ;	/* reset this, we have a different error */
                    اكسر;
                }

            حالة UCNV_SO:
                إذا(my_داده->نسخه==3) {
                    /* JIS7: switch to G1 half-width Katakana */
                    p_مقصد_U2022_State->cs[1] = static_cast<int8_t>(HWKANA_7_بیت);
                    p_مقصد_U2022_State->g=1;
                    استمر;
                } وإلا {
                    /* only JIS7 uses SI/SO, not ISO-2022-JP-x */
                    my_داده->is_خالی_Segment = خطأ;	/* reset this, we have a different error */
                    اكسر;
                }

            حالة ESC_2022:
                my_منبع--;
escape:
                {
                    ثابت حرف * my_منبع_Before = my_منبع;
                    int8_t مقصد_ULength_Before = آرگومانها_2->converter->مقصد_ULength;

                    changeState_2022(آرگومانها_2->converter,&(my_منبع),
                        my_منبع_حد, ISO_2022_JP,err);

                    /* If in ISO-2022-JP only and we successfully completed an escape sequence, but previous segment was empty, create an error */
                    إذا(my_داده->نسخه==0 && my_داده->کلید==0 && U_SUCCESS(*err) && my_داده->is_خالی_Segment) {
                        *err = U_ILLEGAL_ESCAPE_SEQUENCE;
                        آرگومانها_2->converter->مقصد_UCallback_Reason = UCNV_IRREGULAR;
                        آرگومانها_2->converter->مقصد_ULength = static_cast<int8_t>(مقصد_ULength_Before + (my_منبع - my_منبع_Before));
                    }
                }

                /* invalid or illegal escape sequence */
                إذا(U_FAILURE(*err)){
                    آرگومانها_2->هدف = my_هدف;
                    آرگومانها_2->منبع = my_منبع;
                    my_داده->is_خالی_Segment = خطأ;	/* Reset to avoid future spurious errors */
                    أرجع;
                }
                /* If we successfully completed an escape sequence, we begin a new segment, empty so far */
                إذا(my_داده->کلید==0) {
                    my_داده->is_خالی_Segment = درست;
                }
                استمر;

            /* ISO-2022-JP does not use single-byte (C1) SS2 and SS3 */

            حالة CR:
            حالة LF:
                /* automatically reset to single-byte mode */
                إذا (static_cast<StateEnum>(p_مقصد_U2022_State->cs[0]) != ASCII &&
                    static_cast<StateEnum>(p_مقصد_U2022_State->cs[0]) != JISX201) {
                    p_مقصد_U2022_State->cs[0] = static_cast<int8_t>(ASCII);
                }
                p_مقصد_U2022_State->cs[2] = 0;
                p_مقصد_U2022_State->g = 0;
                U_FALLTHROUGH;
            افتراضي:
                /* convert one or two bytes */
                my_داده->is_خالی_Segment = خطأ;
                cs = static_cast<StateEnum>(p_مقصد_U2022_State->cs[p_مقصد_U2022_State->g]);
                إذا (static_cast<uint8_t>(my_منبع_Char - 0xa1) <= (0xdf - 0xa1) && my_داده->نسخه == 4 &&
                    !IS_JP_DBCS(cs)
                ) {
                    /* 8-bit halfwidth katakana in any single-byte mode for JIS8 */
                    هدف_Uni_Char = my_منبع_Char + (HWKANA_شروع - 0xa1);

                    /* return from a single-shift state to the previous one */
                    إذا(p_مقصد_U2022_State->g >= 2) {
                        p_مقصد_U2022_State->g=p_مقصد_U2022_State->prevG;
                    }
                } وإلا بدل(cs) {
                حالة ASCII:
                    إذا(my_منبع_Char <= 0x7f) {
                        هدف_Uni_Char = my_منبع_Char;
                    }
                    اكسر;
                حالة ISO8859_1:
                    إذا(my_منبع_Char <= 0x7f) {
                        هدف_Uni_Char = my_منبع_Char + 0x80;
                    }
                    /* return from a single-shift state to the previous one */
                    p_مقصد_U2022_State->g=p_مقصد_U2022_State->prevG;
                    اكسر;
                حالة ISO8859_7:
                    إذا(my_منبع_Char <= 0x7f) {
                        /* convert mySourceChar+0x80 to use a normal 8-bit table */
                        هدف_Uni_Char =
                            MBCS_SINGLE_SIMPLE_GET_بعدی_BMP(
                                my_داده->myConverterArray[cs],
                                my_منبع_Char + 0x80);
                    }
                    /* return from a single-shift state to the previous one */
                    p_مقصد_U2022_State->g=p_مقصد_U2022_State->prevG;
                    اكسر;
                حالة JISX201:
                    إذا(my_منبع_Char <= 0x7f) {
                        هدف_Uni_Char = jisx201_مقصد_U(my_منبع_Char);
                    }
                    اكسر;
                حالة HWKANA_7_بیت:
                    إذا (static_cast<uint8_t>(my_منبع_Char - 0x21) <= (0x5f - 0x21)) {
                        /* 7-bit halfwidth Katakana */
                        هدف_Uni_Char = my_منبع_Char + (HWKANA_شروع - 0x21);
                    }
                    اكسر;
                افتراضي:
                    /* G0 DBCS */
                    إذا(my_منبع < my_منبع_حد) {
                        صحيح leadIsOk, trailIsOk;
                        uint8_t trailByte;
getTrailByte:
                        trailByte = static_cast<uint8_t>(*my_منبع);
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
                        leadIsOk = static_cast<uint8_t>(my_منبع_Char - 0x21) <= (0x7e - 0x21);
                        trailIsOk = static_cast<uint8_t>(trailByte - 0x21) <= (0x7e - 0x21);
                        إذا (leadIsOk && trailIsOk) {
                            ++my_منبع;
                            tmp_منبع_Char = (my_منبع_Char << 8) | trailByte;
                            إذا(cs == JISX208) {
                                _2022ToSJIS(static_cast<uint8_t>(my_منبع_Char), trailByte, موقت_Buf);
                                my_منبع_Char = tmp_منبع_Char;
                            } وإلا {
                                /* Copy before we modify tmpSourceChar so toUnicodeCallback() sees the correct bytes. */
                                my_منبع_Char = tmp_منبع_Char;
                                إذا (cs == KSC5601) {
                                    tmp_منبع_Char += 0x8080;  /* = _2022ToGR94DBCS(tmpSourceChar) */
                                }
                                موقت_Buf[0] = static_cast<حرف>(tmp_منبع_Char >> 8);
                                موقت_Buf[1] = static_cast<حرف>(tmp_منبع_Char);
                            }
                            هدف_Uni_Char = ucnv_MBCSSimple_Get_بعدی_UChar(my_داده->myConverterArray[cs], موقت_Buf, 2, خطأ);
                        } وإلا إذا (!(trailIsOk || IS_2022_CONTROL(trailByte))) {
                            /* report a pair of illegal bytes if the second byte is not a DBCS starter */
                            ++my_منبع;
                            /* add another bit so that the code below writes 2 bytes in case of error */
                            my_منبع_Char = 0x10000 | (my_منبع_Char << 8) | trailByte;
                        }
                    } وإلا {
                        آرگومانها_2->converter->مقصد_UBytes[0] = static_cast<uint8_t>(my_منبع_Char);
                        آرگومانها_2->converter->مقصد_ULength = 1;
                        اذهب endloop;
                    }
                }  /* End of inner switch */
                اكسر;
            }  /* End of outer switch */
            إذا(هدف_Uni_Char < (missingCharMarker-1/*0xfffe*/)){
                إذا(آرگومانها_2->offsets){
                    آرگومانها_2->offsets[my_هدف - آرگومانها_2->هدف] = static_cast<int32_t>(my_منبع - آرگومانها_2->منبع - (my_منبع_Char <= 0xff ? 1 : 2));
                }
                *(my_هدف++) = static_cast<char16_t>(هدف_Uni_Char);
            }
            وإلا إذا(هدف_Uni_Char > missingCharMarker){
                /* disassemble the surrogate pair and write to output*/
                هدف_Uni_Char-=0x0010000;
                *my_هدف = static_cast<char16_t>(0xd800 + static_cast<char16_t>(هدف_Uni_Char >> 10));
                إذا(آرگومانها_2->offsets){
                    آرگومانها_2->offsets[my_هدف - آرگومانها_2->هدف] = static_cast<int32_t>(my_منبع - آرگومانها_2->منبع - (my_منبع_Char <= 0xff ? 1 : 2));
                }
                ++my_هدف;
                إذا(my_هدف< آرگومانها_2->هدف_حد){
                    *my_هدف = static_cast<char16_t>(0xdc00 + static_cast<char16_t>(هدف_Uni_Char & 0x3ff));
                    إذا(آرگومانها_2->offsets){
                        آرگومانها_2->offsets[my_هدف - آرگومانها_2->هدف] = static_cast<int32_t>(my_منبع - آرگومانها_2->منبع - (my_منبع_Char <= 0xff ? 1 : 2));
                    }
                    ++my_هدف;
                }وإلا{
                    آرگومانها_2->converter->UChar_Error_میانگیر[آرگومانها_2->converter->UChar_Error_میانگیر_درازی++]=
                                    static_cast<char16_t>(0xdc00 + static_cast<char16_t>(هدف_Uni_Char & 0x3ff));
                }

            }
            وإلا{
                /* Call the callback function*/
                مقصد_Unicode_Callback(آرگومانها_2->converter,my_منبع_Char,هدف_Uni_Char,err);
                اكسر;
            }
        }
        وإلا{    /* goes with "if(myTarget < args->targetLimit)"  way up near top of function */
            *err =U_میانگیر_سرریز_ERROR;
            اكسر;
        }
    }
endloop:
    آرگومانها_2->هدف = my_هدف;
    آرگومانها_2->منبع = my_منبع;
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
UConverter_fromUnicode_ISO_2022_KR_OFFSETS_LOGIC_IBM(UConverter_From_Unicode_آرگومانها* آرگومانها_2, UError_کد* err){

    UConverter* saveConv = آرگومانها_2->converter;
    UConverter_داده_ISO2022* my_Converter_داده = static_cast<UConverter_داده_ISO2022*>(saveConv->extraInfo);
    آرگومانها_2->converter=my_Converter_داده->currentConverter;

    my_Converter_داده->currentConverter->fromUChar32 = saveConv->fromUChar32;
    ucnv_MBCSFromUnicodeWithOffsets(آرگومانها_2,err);
    saveConv->fromUChar32 = my_Converter_داده->currentConverter->fromUChar32;

    إذا(*err == U_میانگیر_سرریز_ERROR) {
        إذا(my_Converter_داده->currentConverter->char_Error_میانگیر_درازی > 0) {
            uprv_memcpy(
                saveConv->char_Error_میانگیر,
                my_Converter_داده->currentConverter->char_Error_میانگیر,
                my_Converter_داده->currentConverter->char_Error_میانگیر_درازی);
        }
        saveConv->char_Error_میانگیر_درازی = my_Converter_داده->currentConverter->char_Error_میانگیر_درازی;
        my_Converter_داده->currentConverter->char_Error_میانگیر_درازی = 0;
    }
    آرگومانها_2->converter=saveConv;
}

ساكن فراغ U_CALLCONV
UConverter_fromUnicode_ISO_2022_KR_OFFSETS_LOGIC(UConverter_From_Unicode_آرگومانها* آرگومانها_2, UError_کد* err){

    ثابت char16_t *منبع = آرگومانها_2->منبع;
    ثابت char16_t *منبع_حد = آرگومانها_2->منبع_حد;
    بیعلامت حرف *هدف = reinterpret_cast<بیعلامت حرف*>(آرگومانها_2->هدف);
    بیعلامت حرف *هدف_حد = reinterpret_cast<بیعلامت حرف*>(const_cast<حرف*>(آرگومانها_2->هدف_حد));
    int32_t* offsets = آرگومانها_2->offsets;
    uint32_t هدف_Byte_Unit = 0x0000;
    UChar32 منبع_Char = 0x0000;
    UBool is_هدف_Byte_DBCS;
    UBool old_Is_هدف_Byte_DBCS;
    UConverter_داده_ISO2022 *converter_داده;
    UConverter_Shared_داده* shared_داده;
    UBool useFallback;
    int32_t length =0;

    converter_داده = static_cast<UConverter_داده_ISO2022*>(آرگومانها_2->converter->extraInfo);
    /* if the version is 1 then the user is requesting
     * conversion with ibm-25546 pass the arguments to
     * MBCS converter and return
     */
    إذا(converter_داده->نسخه==1){
        UConverter_fromUnicode_ISO_2022_KR_OFFSETS_LOGIC_IBM(آرگومانها_2,err);
        أرجع;
    }

    /* initialize data */
    shared_داده = converter_داده->currentConverter->shared_داده;
    useFallback = آرگومانها_2->converter->useFallback;
    is_هدف_Byte_DBCS = static_cast<UBool>(آرگومانها_2->converter->from_Unicode_وضعیت);
    old_Is_هدف_Byte_DBCS = is_هدف_Byte_DBCS;

    is_هدف_Byte_DBCS = static_cast<UBool>(آرگومانها_2->converter->from_Unicode_وضعیت);
    إذا((منبع_Char = آرگومانها_2->converter->fromUChar32)!=0 && هدف <هدف_حد) {
        اذهب getTrail;
    }
    طالما(منبع < منبع_حد){

        هدف_Byte_Unit = missingCharMarker;

        إذا(هدف < (بیعلامت حرف*) آرگومانها_2->هدف_حد){
            منبع_Char = *منبع++;

            /* do not convert SO/SI/ESC */
            إذا(IS_2022_CONTROL(منبع_Char)) {
                /* callback(illegal) */
                *err=U_ILLEGAL_CHAR_یافت;
                آرگومانها_2->converter->fromUChar32=منبع_Char;
                اكسر;
            }

            length = MBCS_FROM_UCHAR32_ISO2022(shared_داده,منبع_Char,&هدف_Byte_Unit,useFallback,MBCS_OUTPUT_2);
            إذا(length < 0) {
                length = -length;  /* fallback */
            }
            /* only DBCS or SBCS characters are expected*/
            /* DB characters with high bit set to 1 are expected */
            إذا( length > 2 || length==0 ||
                (length == 1 && هدف_Byte_Unit > 0x7f) ||
                (length == 2 &&
                    (static_cast<uint16_t>(هدف_Byte_Unit - 0xa1a1) > (0xfefe - 0xa1a1) ||
                    static_cast<uint8_t>(هدف_Byte_Unit - 0xa1) > (0xfe - 0xa1)))
            ) {
                هدف_Byte_Unit=missingCharMarker;
            }
            إذا (هدف_Byte_Unit != missingCharMarker){

                old_Is_هدف_Byte_DBCS = is_هدف_Byte_DBCS;
                is_هدف_Byte_DBCS = static_cast<UBool>(هدف_Byte_Unit > 0x00FF);
                  /* append the shift sequence */
                إذا (old_Is_هدف_Byte_DBCS != is_هدف_Byte_DBCS ){

                    إذا (is_هدف_Byte_DBCS)
                        *هدف++ = UCNV_SO;
                    وإلا
                        *هدف++ = UCNV_SI;
                    إذا(offsets)
                        *(offsets++) = static_cast<int32_t>(منبع - آرگومانها_2->منبع - 1);
                }
                /* write the targetUniChar  to target */
                إذا(هدف_Byte_Unit <= 0x00FF){
                    إذا( هدف < هدف_حد){
                        *(هدف++) = static_cast<بیعلامت حرف>(هدف_Byte_Unit);
                        إذا(offsets){
                            *(offsets++) = static_cast<int32_t>(منبع - آرگومانها_2->منبع - 1);
                        }

                    }وإلا{
                        آرگومانها_2->converter->char_Error_میانگیر[آرگومانها_2->converter->char_Error_میانگیر_درازی++] = static_cast<بیعلامت حرف>(هدف_Byte_Unit);
                        *err = U_میانگیر_سرریز_ERROR;
                    }
                }وإلا{
                    إذا(هدف < هدف_حد){
                        *(هدف++) = static_cast<بیعلامت حرف>((هدف_Byte_Unit >> 8) - 0x80);
                        إذا(offsets){
                            *(offsets++) = static_cast<int32_t>(منبع - آرگومانها_2->منبع - 1);
                        }
                        إذا(هدف < هدف_حد){
                            *(هدف++) = static_cast<بیعلامت حرف>(هدف_Byte_Unit - 0x80);
                            إذا(offsets){
                                *(offsets++) = static_cast<int32_t>(منبع - آرگومانها_2->منبع - 1);
                            }
                        }وإلا{
                            آرگومانها_2->converter->char_Error_میانگیر[آرگومانها_2->converter->char_Error_میانگیر_درازی++] = static_cast<بیعلامت حرف>(هدف_Byte_Unit - 0x80);
                            *err = U_میانگیر_سرریز_ERROR;
                        }
                    }وإلا{
                        آرگومانها_2->converter->char_Error_میانگیر[آرگومانها_2->converter->char_Error_میانگیر_درازی++] = static_cast<بیعلامت حرف>((هدف_Byte_Unit >> 8) - 0x80);
                        آرگومانها_2->converter->char_Error_میانگیر[آرگومانها_2->converter->char_Error_میانگیر_درازی++] = static_cast<بیعلامت حرف>(هدف_Byte_Unit - 0x80);
                        *err = U_میانگیر_سرریز_ERROR;
                    }
                }

            }
            وإلا{
                /* oops.. the code point is unassingned
                 * set the error and reason
                 */

                /*check if the char is a First surrogate*/
                إذا(U16_IS_SURROGATE(منبع_Char)) {
                    إذا(U16_IS_SURROGATE_LEAD(منبع_Char)) {
getTrail:
                        /*look ahead to find the trail surrogate*/
                        إذا(منبع <  منبع_حد) {
                            /* test the following code unit */
                            char16_t trail = *منبع;
                            إذا(U16_IS_TRAIL(trail)) {
                                منبع++;
                                منبع_Char=U16_GET_SUPPLEMENTARY(منبع_Char, trail);
                                *err = U_INVALID_CHAR_یافت;
                                /* convert this surrogate code point */
                                /* exit this condition tree */
                            } وإلا {
                                /* this is an unmatched lead code unit (1st surrogate) */
                                /* callback(illegal) */
                                *err=U_ILLEGAL_CHAR_یافت;
                            }
                        } وإلا {
                            /* no more input */
                            *err = U_ZERO_ERROR;
                        }
                    } وإلا {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *err=U_ILLEGAL_CHAR_یافت;
                    }
                } وإلا {
                    /* callback(unassigned) for a BMP code point */
                    *err = U_INVALID_CHAR_یافت;
                }

                آرگومانها_2->converter->fromUChar32=منبع_Char;
                اكسر;
            }
        } /* end if(myTargetIndex<myTargetLength) */
        وإلا{
            *err =U_میانگیر_سرریز_ERROR;
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
        is_هدف_Byte_DBCS &&
        آرگومانها_2->flush && منبع>=منبع_حد && آرگومانها_2->converter->fromUChar32==0
    ) {
        int32_t منبع_شاخص;

        /* we are switching to ASCII */
        is_هدف_Byte_DBCS=خطأ;

        /* get the source index of the last input character */
        /*
         * TODO this would be simpler and more reliable if we used a pair
         * of sourceIndex/prevSourceIndex like in ucnvmbcs.c
         * so that we could simply use the prevSourceIndex here;
         * this code gives an incorrect result for the rare case of an unmatched
         * trail surrogate that is alone in the last buffer of the text stream
         */
        منبع_شاخص = static_cast<int32_t>(منبع - آرگومانها_2->منبع);
        إذا(منبع_شاخص>0) {
            --منبع_شاخص;
            إذا( U16_IS_TRAIL(آرگومانها_2->منبع[منبع_شاخص]) &&
                (منبع_شاخص==0 || U16_IS_LEAD(آرگومانها_2->منبع[منبع_شاخص-1]))
            ) {
                --منبع_شاخص;
            }
        } وإلا {
            منبع_شاخص=-1;
        }

        fromUWriteUInt8(
            آرگومانها_2->converter,
            لغزش_درون_STR, 1,
            &هدف, reinterpret_cast<ثابت حرف*>(هدف_حد),
            &offsets, منبع_شاخص,
            err);
    }

    /*save the state and return */
    آرگومانها_2->منبع = منبع;
    آرگومانها_2->هدف = reinterpret_cast<حرف*>(هدف);
    آرگومانها_2->converter->from_Unicode_وضعیت = static_cast<uint32_t>(is_هدف_Byte_DBCS);
}

/************************ To Unicode ***************************************/

ساكن فراغ U_CALLCONV
UConverter_مقصد_Unicode_ISO_2022_KR_OFFSETS_LOGIC_IBM(UConverter_مقصد_Unicode_آرگومانها *آرگومانها_2,
                                                            UError_کد* err){
    حرف ثابت* منبع_شروع;
    UConverter_داده_ISO2022* my_داده = static_cast<UConverter_داده_ISO2022*>(آرگومانها_2->converter->extraInfo);

    UConverter_مقصد_Unicode_آرگومانها sub_آرگومانها;
    int32_t min_آرگومانها_اندازه;

    /* set up the subconverter arguments */
    إذا(آرگومانها_2->size<حجم(UConverter_مقصد_Unicode_آرگومانها)) {
        min_آرگومانها_اندازه = آرگومانها_2->size;
    } وإلا {
        min_آرگومانها_اندازه = static_cast<int32_t>(حجم(UConverter_مقصد_Unicode_آرگومانها));
    }

    uprv_memcpy(&sub_آرگومانها, آرگومانها_2, min_آرگومانها_اندازه);
    sub_آرگومانها.size = static_cast<uint16_t>(min_آرگومانها_اندازه);
    sub_آرگومانها.converter = my_داده->currentConverter;

    /* remember the original start of the input for offsets */
    منبع_شروع = آرگومانها_2->منبع;

    إذا(my_داده->کلید != 0) {
        /* continue with a partial escape sequence */
        اذهب escape;
    }

    طالما(U_SUCCESS(*err) && آرگومانها_2->منبع < آرگومانها_2->منبع_حد) {
        /*Find the end of the buffer e.g : Next Escape Seq | end of Buffer*/
        sub_آرگومانها.منبع = آرگومانها_2->منبع;
        sub_آرگومانها.منبع_حد = get_انتها_از_میانگیر_2022(&(آرگومانها_2->منبع), آرگومانها_2->منبع_حد, آرگومانها_2->flush);
        إذا(sub_آرگومانها.منبع != sub_آرگومانها.منبع_حد) {
            /*
             * get the current partial byte sequence
             *
             * it needs to be moved between the public and the subconverter
             * so that the conversion framework, which only sees the public
             * converter, can handle truncated and illegal input etc.
             */
            إذا(آرگومانها_2->converter->مقصد_ULength > 0) {
                uprv_memcpy(sub_آرگومانها.converter->مقصد_UBytes, آرگومانها_2->converter->مقصد_UBytes, آرگومانها_2->converter->مقصد_ULength);
            }
            sub_آرگومانها.converter->مقصد_ULength = آرگومانها_2->converter->مقصد_ULength;

            /*
             * Convert up to the end of the input, or to before the next escape character.
             * Does not handle conversion extensions because the preToU[] state etc.
             * is not copied.
             */
            ucnv_MBCSToUnicodeWithOffsets(&sub_آرگومانها, err);

            إذا(آرگومانها_2->offsets != اشاره_تهی && منبع_شروع != آرگومانها_2->منبع) {
                /* update offsets to base them on the actual start of the input */
                int32_t *offsets = آرگومانها_2->offsets;
                char16_t *هدف = آرگومانها_2->هدف;
                int32_t delta = static_cast<int32_t>(آرگومانها_2->منبع - منبع_شروع);
                طالما(هدف < sub_آرگومانها.هدف) {
                    إذا(*offsets >= 0) {
                        *offsets += delta;
                    }
                    ++offsets;
                    ++هدف;
                }
            }
            آرگومانها_2->منبع = sub_آرگومانها.منبع;
            آرگومانها_2->هدف = sub_آرگومانها.هدف;
            آرگومانها_2->offsets = sub_آرگومانها.offsets;

            /* copy input/error/overflow buffers */
            إذا(sub_آرگومانها.converter->مقصد_ULength > 0) {
                uprv_memcpy(آرگومانها_2->converter->مقصد_UBytes, sub_آرگومانها.converter->مقصد_UBytes, sub_آرگومانها.converter->مقصد_ULength);
            }
            آرگومانها_2->converter->مقصد_ULength = sub_آرگومانها.converter->مقصد_ULength;

            إذا(*err == U_میانگیر_سرریز_ERROR) {
                إذا(sub_آرگومانها.converter->UChar_Error_میانگیر_درازی > 0) {
                    uprv_memcpy(آرگومانها_2->converter->UChar_Error_میانگیر, sub_آرگومانها.converter->UChar_Error_میانگیر,
                                sub_آرگومانها.converter->UChar_Error_میانگیر_درازی);
                }
                آرگومانها_2->converter->UChar_Error_میانگیر_درازی=sub_آرگومانها.converter->UChar_Error_میانگیر_درازی;
                sub_آرگومانها.converter->UChar_Error_میانگیر_درازی = 0;
            }
        }

        إذا (U_FAILURE(*err) || (آرگومانها_2->منبع == آرگومانها_2->منبع_حد)) {
            أرجع;
        }

escape:
        changeState_2022(آرگومانها_2->converter,
               &(آرگومانها_2->منبع),
               آرگومانها_2->منبع_حد,
               ISO_2022_KR,
               err);
    }
}

ساكن فراغ U_CALLCONV
UConverter_مقصد_Unicode_ISO_2022_KR_OFFSETS_LOGIC(UConverter_مقصد_Unicode_آرگومانها *آرگومانها_2,
                                                            UError_کد* err){
    حرف موقت_Buf[2];
    ثابت حرف* my_منبع = const_cast<حرف*>(آرگومانها_2->منبع);
    char16_t *my_هدف = آرگومانها_2->هدف;
    ثابت حرف *my_منبع_حد = آرگومانها_2->منبع_حد;
    UChar32 هدف_Uni_Char = 0x0000;
    char16_t my_منبع_Char = 0x0000;
    UConverter_داده_ISO2022* my_داده;
    UConverter_Shared_داده* shared_داده ;
    UBool useFallback;

    my_داده = static_cast<UConverter_داده_ISO2022*>(آرگومانها_2->converter->extraInfo);
    إذا(my_داده->نسخه==1){
        UConverter_مقصد_Unicode_ISO_2022_KR_OFFSETS_LOGIC_IBM(آرگومانها_2,err);
        أرجع;
    }

    /* initialize state */
    shared_داده = my_داده->currentConverter->shared_داده;
    useFallback = آرگومانها_2->converter->useFallback;

    إذا(my_داده->کلید != 0) {
        /* continue with a partial escape sequence */
        اذهب escape;
    } وإلا إذا(آرگومانها_2->converter->مقصد_ULength == 1 && my_منبع < my_منبع_حد && my_هدف < آرگومانها_2->هدف_حد) {
        /* continue with a partial double-byte character */
        my_منبع_Char = آرگومانها_2->converter->مقصد_UBytes[0];
        آرگومانها_2->converter->مقصد_ULength = 0;
        اذهب getTrailByte;
    }

    طالما(my_منبع< my_منبع_حد){

        إذا(my_هدف < آرگومانها_2->هدف_حد){

            my_منبع_Char = static_cast<بیعلامت حرف>(*my_منبع++);

            إذا(my_منبع_Char==UCNV_SI){
                my_داده->مقصد_U2022_State.g = 0;
                إذا (my_داده->is_خالی_Segment) {
                    my_داده->is_خالی_Segment = خطأ;	/* we are handling it, reset to avoid future spurious errors */
                    *err = U_ILLEGAL_ESCAPE_SEQUENCE;
                    آرگومانها_2->converter->مقصد_UCallback_Reason = UCNV_IRREGULAR;
                    آرگومانها_2->converter->مقصد_UBytes[0] = static_cast<uint8_t>(my_منبع_Char);
                    آرگومانها_2->converter->مقصد_ULength = 1;
                    آرگومانها_2->هدف = my_هدف;
                    آرگومانها_2->منبع = my_منبع;
                    أرجع;
                }
                /*consume the source */
                استمر;
            }وإلا إذا(my_منبع_Char==UCNV_SO){
                my_داده->مقصد_U2022_State.g = 1;
                my_داده->is_خالی_Segment = درست;	/* Begin a new segment, empty so far */
                /*consume the source */
                استمر;
            }وإلا إذا(my_منبع_Char==ESC_2022){
                my_منبع--;
escape:
                my_داده->is_خالی_Segment = خطأ;	/* Any invalid ESC sequences will be detected separately, so just reset this */
                changeState_2022(آرگومانها_2->converter,&(my_منبع),
                                my_منبع_حد, ISO_2022_KR, err);
                إذا(U_FAILURE(*err)){
                    آرگومانها_2->هدف = my_هدف;
                    آرگومانها_2->منبع = my_منبع;
                    أرجع;
                }
                استمر;
            }

            my_داده->is_خالی_Segment = خطأ;	/* Any invalid char errors will be detected separately, so just reset this */
            إذا(my_داده->مقصد_U2022_State.g == 1) {
                إذا(my_منبع < my_منبع_حد) {
                    صحيح leadIsOk, trailIsOk;
                    uint8_t trailByte;
getTrailByte:
                    هدف_Uni_Char = missingCharMarker;
                    trailByte = static_cast<uint8_t>(*my_منبع);
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
                    leadIsOk = static_cast<uint8_t>(my_منبع_Char - 0x21) <= (0x7e - 0x21);
                    trailIsOk = static_cast<uint8_t>(trailByte - 0x21) <= (0x7e - 0x21);
                    إذا (leadIsOk && trailIsOk) {
                        ++my_منبع;
                        موقت_Buf[0] = static_cast<حرف>(my_منبع_Char + 0x80);
                        موقت_Buf[1] = static_cast<حرف>(trailByte + 0x80);
                        هدف_Uni_Char = ucnv_MBCSSimple_Get_بعدی_UChar(shared_داده, موقت_Buf, 2, useFallback);
                        my_منبع_Char = (my_منبع_Char << 8) | trailByte;
                    } وإلا إذا (!(trailIsOk || IS_2022_CONTROL(trailByte))) {
                        /* report a pair of illegal bytes if the second byte is not a DBCS starter */
                        ++my_منبع;
                        /* add another bit so that the code below writes 2 bytes in case of error */
                        my_منبع_Char = static_cast<char16_t>(0x10000 | (my_منبع_Char << 8) | trailByte);
                    }
                } وإلا {
                    آرگومانها_2->converter->مقصد_UBytes[0] = static_cast<uint8_t>(my_منبع_Char);
                    آرگومانها_2->converter->مقصد_ULength = 1;
                    اكسر;
                }
            }
            وإلا إذا(my_منبع_Char <= 0x7f) {
                هدف_Uni_Char = ucnv_MBCSSimple_Get_بعدی_UChar(shared_داده, my_منبع - 1, 1, useFallback);
            } وإلا {
                هدف_Uni_Char = 0xffff;
            }
            إذا(هدف_Uni_Char < 0xfffe){
                إذا(آرگومانها_2->offsets) {
                    آرگومانها_2->offsets[my_هدف - آرگومانها_2->هدف] = static_cast<int32_t>(my_منبع - آرگومانها_2->منبع - (my_منبع_Char <= 0xff ? 1 : 2));
                }
                *(my_هدف++) = static_cast<char16_t>(هدف_Uni_Char);
            }
            وإلا {
                /* Call the callback function*/
                مقصد_Unicode_Callback(آرگومانها_2->converter,my_منبع_Char,هدف_Uni_Char,err);
                اكسر;
            }
        }
        وإلا{
            *err =U_میانگیر_سرریز_ERROR;
            اكسر;
        }
    }
    آرگومانها_2->هدف = my_هدف;
    آرگومانها_2->منبع = my_منبع;
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
        لغزش_درون_STR,                   /* 0 ASCII */
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
UConverter_fromUnicode_ISO_2022_CN_OFFSETS_LOGIC(UConverter_From_Unicode_آرگومانها* آرگومانها_2, UError_کد* err){
    UConverter *cnv = آرگومانها_2->converter;
    UConverter_داده_ISO2022 *converter_داده;
    ISO2022State *pFromU2022State;
    uint8_t* هدف = reinterpret_cast<uint8_t*>(آرگومانها_2->هدف);
    ثابت uint8_t* هدف_حد = reinterpret_cast<ثابت uint8_t*>(آرگومانها_2->هدف_حد);
    ثابت char16_t* منبع = آرگومانها_2->منبع;
    ثابت char16_t* منبع_حد = آرگومانها_2->منبع_حد;
    int32_t* offsets = آرگومانها_2->offsets;
    UChar32 منبع_Char;
    حرف میانگیر[8];
    int32_t len;
    int8_t choices[3];
    int32_t choice_شمار;
    uint32_t هدف_مقدار = 0;
    UBool useFallback;

    /* set up the state */
    converter_داده = static_cast<UConverter_داده_ISO2022*>(cnv->extraInfo);
    pFromU2022State   = &converter_داده->fromU2022State;

    choice_شمار = 0;

    /* check if the last codepoint of previous buffer was a lead surrogate*/
    إذا((منبع_Char = cnv->fromUChar32)!=0 && هدف< هدف_حد) {
        اذهب getTrail;
    }

    طالما( منبع < منبع_حد){
        إذا(هدف < هدف_حد){

            منبع_Char  = *(منبع++);
            /*check if the char is a First surrogate*/
             إذا(U16_IS_SURROGATE(منبع_Char)) {
                إذا(U16_IS_SURROGATE_LEAD(منبع_Char)) {
getTrail:
                    /*look ahead to find the trail surrogate*/
                    إذا(منبع < منبع_حد) {
                        /* test the following code unit */
                        char16_t trail = *منبع;
                        إذا(U16_IS_TRAIL(trail)) {
                            منبع++;
                            منبع_Char=U16_GET_SUPPLEMENTARY(منبع_Char, trail);
                            cnv->fromUChar32=0x00;
                            /* convert this supplementary code point */
                            /* exit this condition tree */
                        } وإلا {
                            /* this is an unmatched lead code unit (1st surrogate) */
                            /* callback(illegal) */
                            *err=U_ILLEGAL_CHAR_یافت;
                            cnv->fromUChar32=منبع_Char;
                            اكسر;
                        }
                    } وإلا {
                        /* no more input */
                        cnv->fromUChar32=منبع_Char;
                        اكسر;
                    }
                } وإلا {
                    /* this is an unmatched trail code unit (2nd surrogate) */
                    /* callback(illegal) */
                    *err=U_ILLEGAL_CHAR_یافت;
                    cnv->fromUChar32=منبع_Char;
                    اكسر;
                }
            }

            /* do the conversion */
            إذا(منبع_Char <= 0x007f ){
                /* do not convert SO/SI/ESC */
                إذا(IS_2022_CONTROL(منبع_Char)) {
                    /* callback(illegal) */
                    *err=U_ILLEGAL_CHAR_یافت;
                    cnv->fromUChar32=منبع_Char;
                    اكسر;
                }

                /* US-ASCII */
                إذا(pFromU2022State->g == 0) {
                    میانگیر[0] = static_cast<حرف>(منبع_Char);
                    len = 1;
                } وإلا {
                    میانگیر[0] = UCNV_SI;
                    میانگیر[1] = static_cast<حرف>(منبع_Char);
                    len = 2;
                    pFromU2022State->g = 0;
                    choice_شمار = 0;
                }
                إذا(منبع_Char == CR || منبع_Char == LF) {
                    /* reset the state at the end of a line */
                    uprv_memset(pFromU2022State, 0, حجم(ISO2022State));
                    choice_شمار = 0;
                }
            }
            وإلا{
                /* convert U+0080..U+10ffff */
                int32_t i;
                int8_t cs, g;

                إذا(choice_شمار == 0) {
                    /* try the current SO/G1 converter first */
                    choices[0] = pFromU2022State->cs[1];

                    /* default to GB2312_1 if none is designated yet */
                    إذا(choices[0] == 0) {
                        choices[0] = GB2312_1;
                    }

                    إذا(converter_داده->نسخه == 0) {
                        /* ISO-2022-CN */

                        /* try the other SO/G1 converter; a CNS_11643_1 lookup may result in any plane */
                        إذا(choices[0] == GB2312_1) {
                            choices[1] = static_cast<int8_t>(CNS_11643_1);
                        } وإلا {
                            choices[1] = static_cast<int8_t>(GB2312_1);
                        }

                        choice_شمار = 2;
                    } وإلا إذا (converter_داده->نسخه == 1) {
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

                        choice_شمار = 3;
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

                لكل(i = 0; i < choice_شمار && len <= 0; ++i) {
                    int8_t cs0 = choices[i];
                    إذا(cs0 > 0) {
                        uint32_t value;
                        int32_t len2;
                        إذا(cs0 >= CNS_11643_0) {
                            len2 = MBCS_FROM_UCHAR32_ISO2022(
                                        converter_داده->myConverterArray[CNS_11643],
                                        منبع_Char,
                                        &value,
                                        useFallback,
                                        MBCS_OUTPUT_3);
                            إذا(len2 == 3 || (len2 == -3 && len == 0)) {
                                هدف_مقدار = value;
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
                                } وإلا /* plane 3..7 */ إذا(converter_داده->نسخه == 1) {
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
                                        converter_داده->myConverterArray[cs0],
                                        منبع_Char,
                                        &value,
                                        useFallback,
                                        MBCS_OUTPUT_2);
                            إذا(len2 == 2 || (len2 == -2 && len == 0)) {
                                هدف_مقدار = value;
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
                            uprv_memcpy(میانگیر, escSeqCharsCN[cs], 4);
                        } وإلا {
                            U_ASSERT(cs >= CNS_11643_1);
                            uprv_memcpy(میانگیر, escSeqCharsCN[CNS_11643 + (cs - CNS_11643_1)], 4);
                        }
                        len = 4;
                        pFromU2022State->cs[g] = cs;
                        إذا(g == 1) {
                            /* changing the SO/G1 charset invalidates the choices[] */
                            choice_شمار = 0;
                        }
                    }

                    /* write the shift sequence if necessary */
                    إذا(g != pFromU2022State->g) {
                        بدل(g) {
                        حالة 1:
                            میانگیر[len++] = UCNV_SO;

                            /* set the new state only if it is the locking shift SO/G1, not for SS2 or SS3 */
                            pFromU2022State->g = 1;
                            اكسر;
                        حالة 2:
                            میانگیر[len++] = 0x1b;
                            میانگیر[len++] = 0x4e;
                            اكسر;
                        افتراضي: /* case 3 */
                            میانگیر[len++] = 0x1b;
                            میانگیر[len++] = 0x4f;
                            اكسر;
                        }
                    }

                    /* write the two output bytes */
                    میانگیر[len++] = static_cast<حرف>(هدف_مقدار >> 8);
                    میانگیر[len++] = static_cast<حرف>(هدف_مقدار);
                } وإلا {
                    /* if we cannot find the character after checking all codepages
                     * then this is an error
                     */
                    *err = U_INVALID_CHAR_یافت;
                    cnv->fromUChar32=منبع_Char;
                    اكسر;
                }
            }

            /* output len>0 bytes in buffer[] */
            إذا(len == 1) {
                *هدف++ = میانگیر[0];
                إذا(offsets) {
                    *offsets++ = static_cast<int32_t>(منبع - آرگومانها_2->منبع - 1); /* -1: known to be ASCII */
                }
            } وإلا إذا(len == 2 && (هدف + 2) <= هدف_حد) {
                *هدف++ = میانگیر[0];
                *هدف++ = میانگیر[1];
                إذا(offsets) {
                    int32_t منبع_شاخص = static_cast<int32_t>(منبع - آرگومانها_2->منبع - U16_درازی(منبع_Char));
                    *offsets++ = منبع_شاخص;
                    *offsets++ = منبع_شاخص;
                }
            } وإلا {
                fromUWriteUInt8(
                    cnv,
                    میانگیر, len,
                    &هدف, reinterpret_cast<ثابت حرف*>(هدف_حد),
                    &offsets, static_cast<int32_t>(منبع - آرگومانها_2->منبع - U16_درازی(منبع_Char)),
                    err);
                إذا(U_FAILURE(*err)) {
                    اكسر;
                }
            }
        } /* end if(myTargetIndex<myTargetLength) */
        وإلا{
            *err =U_میانگیر_سرریز_ERROR;
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
        آرگومانها_2->flush && منبع>=منبع_حد && cnv->fromUChar32==0
    ) {
        int32_t منبع_شاخص;

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
        منبع_شاخص = static_cast<int32_t>(منبع - آرگومانها_2->منبع);
        إذا(منبع_شاخص>0) {
            --منبع_شاخص;
            إذا( U16_IS_TRAIL(آرگومانها_2->منبع[منبع_شاخص]) &&
                (منبع_شاخص==0 || U16_IS_LEAD(آرگومانها_2->منبع[منبع_شاخص-1]))
            ) {
                --منبع_شاخص;
            }
        } وإلا {
            منبع_شاخص=-1;
        }

        fromUWriteUInt8(
            cnv,
            لغزش_درون_STR, 1,
            &هدف, reinterpret_cast<ثابت حرف*>(هدف_حد),
            &offsets, منبع_شاخص,
            err);
    }

    /*save the state and return */
    آرگومانها_2->منبع = منبع;
    آرگومانها_2->هدف = reinterpret_cast<حرف*>(هدف);
}


ساكن فراغ U_CALLCONV
UConverter_مقصد_Unicode_ISO_2022_CN_OFFSETS_LOGIC(UConverter_مقصد_Unicode_آرگومانها *آرگومانها_2,
                                               UError_کد* err){
    حرف موقت_Buf[3];
    ثابت حرف* my_منبع = const_cast<حرف*>(آرگومانها_2->منبع);
    char16_t *my_هدف = آرگومانها_2->هدف;
    ثابت حرف *my_منبع_حد = آرگومانها_2->منبع_حد;
    uint32_t هدف_Uni_Char = 0x0000;
    uint32_t my_منبع_Char = 0x0000;
    UConverter_داده_ISO2022* my_داده;
    ISO2022State *p_مقصد_U2022_State;

    my_داده = static_cast<UConverter_داده_ISO2022*>(آرگومانها_2->converter->extraInfo);
    p_مقصد_U2022_State = &my_داده->مقصد_U2022_State;

    إذا(my_داده->کلید != 0) {
        /* continue with a partial escape sequence */
        اذهب escape;
    } وإلا إذا(آرگومانها_2->converter->مقصد_ULength == 1 && my_منبع < my_منبع_حد && my_هدف < آرگومانها_2->هدف_حد) {
        /* continue with a partial double-byte character */
        my_منبع_Char = آرگومانها_2->converter->مقصد_UBytes[0];
        آرگومانها_2->converter->مقصد_ULength = 0;
        هدف_Uni_Char = missingCharMarker;
        اذهب getTrailByte;
    }

    طالما(my_منبع < my_منبع_حد){

        هدف_Uni_Char =missingCharMarker;

        إذا(my_هدف < آرگومانها_2->هدف_حد){

            my_منبع_Char = static_cast<بیعلامت حرف>(*my_منبع++);

            بدل(my_منبع_Char){
            حالة UCNV_SI:
                p_مقصد_U2022_State->g=0;
                إذا (my_داده->is_خالی_Segment) {
                    my_داده->is_خالی_Segment = خطأ;	/* we are handling it, reset to avoid future spurious errors */
                    *err = U_ILLEGAL_ESCAPE_SEQUENCE;
                    آرگومانها_2->converter->مقصد_UCallback_Reason = UCNV_IRREGULAR;
                    آرگومانها_2->converter->مقصد_UBytes[0] = static_cast<uint8_t>(my_منبع_Char);
                    آرگومانها_2->converter->مقصد_ULength = 1;
                    آرگومانها_2->هدف = my_هدف;
                    آرگومانها_2->منبع = my_منبع;
                    أرجع;
                }
                استمر;

            حالة UCNV_SO:
                إذا(p_مقصد_U2022_State->cs[1] != 0) {
                    p_مقصد_U2022_State->g=1;
                    my_داده->is_خالی_Segment = درست;	/* Begin a new segment, empty so far */
                    استمر;
                } وإلا {
                    /* illegal to have SO before a matching designator */
                    my_داده->is_خالی_Segment = خطأ;	/* Handling a different error, reset this to avoid future spurious errs */
                    اكسر;
                }

            حالة ESC_2022:
                my_منبع--;
escape:
                {
                    ثابت حرف * my_منبع_Before = my_منبع;
                    int8_t مقصد_ULength_Before = آرگومانها_2->converter->مقصد_ULength;

                    changeState_2022(آرگومانها_2->converter,&(my_منبع),
                        my_منبع_حد, ISO_2022_CN,err);

                    /* After SO there must be at least one character before a designator (designator error handled separately) */
                    إذا(my_داده->کلید==0 && U_SUCCESS(*err) && my_داده->is_خالی_Segment) {
                        *err = U_ILLEGAL_ESCAPE_SEQUENCE;
                        آرگومانها_2->converter->مقصد_UCallback_Reason = UCNV_IRREGULAR;
                        آرگومانها_2->converter->مقصد_ULength = static_cast<int8_t>(مقصد_ULength_Before + (my_منبع - my_منبع_Before));
                    }
                }

                /* invalid or illegal escape sequence */
                إذا(U_FAILURE(*err)){
                    آرگومانها_2->هدف = my_هدف;
                    آرگومانها_2->منبع = my_منبع;
                    my_داده->is_خالی_Segment = خطأ;	/* Reset to avoid future spurious errors */
                    أرجع;
                }
                استمر;

            /* ISO-2022-CN does not use single-byte (C1) SS2 and SS3 */

            حالة CR:
            حالة LF:
                uprv_memset(p_مقصد_U2022_State, 0, حجم(ISO2022State));
                U_FALLTHROUGH;
            افتراضي:
                /* convert one or two bytes */
                my_داده->is_خالی_Segment = خطأ;
                إذا(p_مقصد_U2022_State->g != 0) {
                    إذا(my_منبع < my_منبع_حد) {
                        UConverter_Shared_داده *cnv;
                        StateEnum موقت_State;
                        int32_t موقت_Buf_Len;
                        صحيح leadIsOk, trailIsOk;
                        uint8_t trailByte;
getTrailByte:
                        trailByte = static_cast<uint8_t>(*my_منبع);
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
                        leadIsOk = static_cast<uint8_t>(my_منبع_Char - 0x21) <= (0x7e - 0x21);
                        trailIsOk = static_cast<uint8_t>(trailByte - 0x21) <= (0x7e - 0x21);
                        إذا (leadIsOk && trailIsOk) {
                            ++my_منبع;
                            موقت_State = static_cast<StateEnum>(p_مقصد_U2022_State->cs[p_مقصد_U2022_State->g]);
                            إذا(موقت_State >= CNS_11643_0) {
                                cnv = my_داده->myConverterArray[CNS_11643];
                                موقت_Buf[0] = static_cast<حرف>(0x80 + (موقت_State - CNS_11643_0));
                                موقت_Buf[1] = static_cast<حرف>(my_منبع_Char);
                                موقت_Buf[2] = static_cast<حرف>(trailByte);
                                موقت_Buf_Len = 3;

                            }وإلا{
                                U_ASSERT(موقت_State<UCNV_2022_MAX_CONVERTERS);
                                cnv = my_داده->myConverterArray[موقت_State];
                                موقت_Buf[0] = static_cast<حرف>(my_منبع_Char);
                                موقت_Buf[1] = static_cast<حرف>(trailByte);
                                موقت_Buf_Len = 2;
                            }
                            هدف_Uni_Char = ucnv_MBCSSimple_Get_بعدی_UChar(cnv, موقت_Buf, موقت_Buf_Len, خطأ);
                            my_منبع_Char = (my_منبع_Char << 8) | trailByte;
                        } وإلا إذا (!(trailIsOk || IS_2022_CONTROL(trailByte))) {
                            /* report a pair of illegal bytes if the second byte is not a DBCS starter */
                            ++my_منبع;
                            /* add another bit so that the code below writes 2 bytes in case of error */
                            my_منبع_Char = 0x10000 | (my_منبع_Char << 8) | trailByte;
                        }
                        إذا(p_مقصد_U2022_State->g>=2) {
                            /* return from a single-shift state to the previous one */
                            p_مقصد_U2022_State->g=p_مقصد_U2022_State->prevG;
                        }
                    } وإلا {
                        آرگومانها_2->converter->مقصد_UBytes[0] = static_cast<uint8_t>(my_منبع_Char);
                        آرگومانها_2->converter->مقصد_ULength = 1;
                        اذهب endloop;
                    }
                }
                وإلا{
                    إذا(my_منبع_Char <= 0x7f) {
                        هدف_Uni_Char = static_cast<char16_t>(my_منبع_Char);
                    }
                }
                اكسر;
            }
            إذا(هدف_Uni_Char < (missingCharMarker-1/*0xfffe*/)){
                إذا(آرگومانها_2->offsets){
                    آرگومانها_2->offsets[my_هدف - آرگومانها_2->هدف] = static_cast<int32_t>(my_منبع - آرگومانها_2->منبع - (my_منبع_Char <= 0xff ? 1 : 2));
                }
                *(my_هدف++) = static_cast<char16_t>(هدف_Uni_Char);
            }
            وإلا إذا(هدف_Uni_Char > missingCharMarker){
                /* disassemble the surrogate pair and write to output*/
                هدف_Uni_Char-=0x0010000;
                *my_هدف = static_cast<char16_t>(0xd800 + static_cast<char16_t>(هدف_Uni_Char >> 10));
                إذا(آرگومانها_2->offsets){
                    آرگومانها_2->offsets[my_هدف - آرگومانها_2->هدف] = static_cast<int32_t>(my_منبع - آرگومانها_2->منبع - (my_منبع_Char <= 0xff ? 1 : 2));
                }
                ++my_هدف;
                إذا(my_هدف< آرگومانها_2->هدف_حد){
                    *my_هدف = static_cast<char16_t>(0xdc00 + static_cast<char16_t>(هدف_Uni_Char & 0x3ff));
                    إذا(آرگومانها_2->offsets){
                        آرگومانها_2->offsets[my_هدف - آرگومانها_2->هدف] = static_cast<int32_t>(my_منبع - آرگومانها_2->منبع - (my_منبع_Char <= 0xff ? 1 : 2));
                    }
                    ++my_هدف;
                }وإلا{
                    آرگومانها_2->converter->UChar_Error_میانگیر[آرگومانها_2->converter->UChar_Error_میانگیر_درازی++]=
                                    static_cast<char16_t>(0xdc00 + static_cast<char16_t>(هدف_Uni_Char & 0x3ff));
                }

            }
            وإلا{
                /* Call the callback function*/
                مقصد_Unicode_Callback(آرگومانها_2->converter,my_منبع_Char,هدف_Uni_Char,err);
                اكسر;
            }
        }
        وإلا{
            *err =U_میانگیر_سرریز_ERROR;
            اكسر;
        }
    }
endloop:
    آرگومانها_2->هدف = my_هدف;
    آرگومانها_2->منبع = my_منبع;
}
#پایان_اگر /* #if !UCONFIG_ONLY_HTML_CONVERSION */

ساكن فراغ U_CALLCONV
_ISO_2022_WriteSub(UConverter_From_Unicode_آرگومانها *آرگومانها_2, int32_t جابه_جایی_شاخص, UError_کد *err) {
    UConverter *cnv = آرگومانها_2->converter;
    UConverter_داده_ISO2022* my_Converter_داده = static_cast<UConverter_داده_ISO2022*>(cnv->extraInfo);
    ISO2022State *pFromU2022State=&my_Converter_داده->fromU2022State;
    حرف *p, *subchar;
    حرف میانگیر[8];
    int32_t length;

    subchar = reinterpret_cast<حرف*>(cnv->subChars);
    length=cnv->subCharLen; /* assume length==1 for most variants */

    p = میانگیر;
    بدل(my_Converter_داده->locale[0]){
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
        إذا(my_Converter_داده->نسخه == 0) {
            إذا(length == 1) {
                إذا(آرگومانها_2->converter->from_Unicode_وضعیت) {
                    /* in DBCS mode: switch to SBCS */
                    آرگومانها_2->converter->from_Unicode_وضعیت = 0;
                    *p++ = UCNV_SI;
                }
                *p++ = subchar[0];
            } وإلا /* length == 2*/ {
                إذا(!آرگومانها_2->converter->from_Unicode_وضعیت) {
                    /* in SBCS mode: switch to DBCS */
                    آرگومانها_2->converter->from_Unicode_وضعیت = 1;
                    *p++ = UCNV_SO;
                }
                *p++ = subchar[0];
                *p++ = subchar[1];
            }
            اكسر;
        } وإلا {
            /* save the subconverter's substitution string */
            uint8_t *currentSubChars = my_Converter_داده->currentConverter->subChars;
            int8_t currentSubCharLen = my_Converter_داده->currentConverter->subCharLen;

            /* set our substitution string into the subconverter */
            my_Converter_داده->currentConverter->subChars = reinterpret_cast<uint8_t*>(subchar);
            my_Converter_داده->currentConverter->subCharLen = static_cast<int8_t>(length);

            /* let the subconverter write the subchar, set/retrieve fromUChar32 state */
            آرگومانها_2->converter = my_Converter_داده->currentConverter;
            my_Converter_داده->currentConverter->fromUChar32 = cnv->fromUChar32;
            ucnv_cbFromUWriteSub(آرگومانها_2, 0, err);
            cnv->fromUChar32 = my_Converter_داده->currentConverter->fromUChar32;
            آرگومانها_2->converter = cnv;

            /* restore the subconverter's substitution string */
            my_Converter_داده->currentConverter->subChars = currentSubChars;
            my_Converter_داده->currentConverter->subCharLen = currentSubCharLen;

            إذا(*err == U_میانگیر_سرریز_ERROR) {
                إذا(my_Converter_داده->currentConverter->char_Error_میانگیر_درازی > 0) {
                    uprv_memcpy(
                        cnv->char_Error_میانگیر,
                        my_Converter_داده->currentConverter->char_Error_میانگیر,
                        my_Converter_داده->currentConverter->char_Error_میانگیر_درازی);
                }
                cnv->char_Error_میانگیر_درازی = my_Converter_داده->currentConverter->char_Error_میانگیر_درازی;
                my_Converter_داده->currentConverter->char_Error_میانگیر_درازی = 0;
            }
            أرجع;
        }
    افتراضي:
        /* not expected */
        اكسر;
    }
    ucnv_cb_From_UWrite_بایتها(آرگومانها_2,
                           میانگیر, static_cast<int32_t>(p - میانگیر),
                           جابه_جایی_شاخص, err);
}

/*
 * Structure for cloning an ISO 2022 converter into a single memory block.
 */
بنية cloneStruct
{
    UConverter cnv;
    UConverter currentConverter;
    UConverter_داده_ISO2022 mydata;
};


U_CDECL_BEGIN

ساكن UConverter * U_CALLCONV
ISO_2022_امن_Clone(
            ثابت UConverter *cnv,
            فراغ *پشته_میانگیر,
            int32_t *p_میانگیر_اندازه,
            UError_کد *وضعیت)
{
    بنية cloneStruct * localClone;
    UConverter_داده_ISO2022 *cnv_داده;
    int32_t i, size;

    إذا (U_FAILURE(*وضعیت)){
        أرجع اشاره_تهی;
    }

    إذا (*p_میانگیر_اندازه == 0) { /* 'preflighting' request - set needed size into *pBufferSize */
        *p_میانگیر_اندازه = (int32_t)حجم(بنية cloneStruct);
        أرجع اشاره_تهی;
    }

    cnv_داده = (UConverter_داده_ISO2022 *)cnv->extraInfo;
    localClone = (بنية cloneStruct *)پشته_میانگیر;

    /* ucnv.c/ucnv_safeClone() copied the main UConverter already */

    uprv_memcpy(&localClone->mydata, cnv_داده, حجم(UConverter_داده_ISO2022));
    localClone->cnv.extraInfo = &localClone->mydata; /* set pointer to extra data */
    localClone->cnv.isExtraLocal = درست;

    /* share the subconverters */

    إذا(cnv_داده->currentConverter != اشاره_تهی) {
        size = (int32_t)حجم(UConverter);
        localClone->mydata.currentConverter =
            ucnv_امن_Clone(cnv_داده->currentConverter,
                            &localClone->currentConverter,
                            &size, وضعیت);
        إذا(U_FAILURE(*وضعیت)) {
            أرجع اشاره_تهی;
        }
    }

    لكل(i=0; i<UCNV_2022_MAX_CONVERTERS; ++i) {
        إذا(cnv_داده->myConverterArray[i] != اشاره_تهی) {
            ucnv_increment_Ref_شمار(cnv_داده->myConverterArray[i]);
        }
    }

    أرجع &localClone->cnv;
}

U_CDECL_انتها

ساكن فراغ U_CALLCONV
ISO_2022_Get_Unicode_مجموعه(ثابت UConverter *cnv,
                    ثابت USetAdder *sa,
                    UConverter_Unicode_مجموعه which,
                    UError_کد *p_Error_کد)
{
    int32_t i;
    UConverter_داده_ISO2022* cnv_داده;

    إذا (U_FAILURE(*p_Error_کد)) {
        أرجع;
    }
#اگر_تعریف_شده U_ENABLE_GENERIC_ISO_2022
    إذا (cnv->shared_داده == &ISO2022_داده) {
        /* We use UTF-8 in this case */
        sa->جمع_کن_Range(sa->مجموعه, 0, 0xd7FF);
        sa->جمع_کن_Range(sa->مجموعه, 0xE000, 0x10FFFF);
        أرجع;
    }
#پایان_اگر

    cnv_داده = static_cast<UConverter_داده_ISO2022*>(cnv->extraInfo);

    /* open a set and initialize it with code points that are algorithmically round-tripped */
    بدل(cnv_داده->locale[0]){
    حالة 'j':
        /* include JIS X 0201 which is hardcoded */
        sa->جمع_کن(sa->مجموعه, 0xa5);
        sa->جمع_کن(sa->مجموعه, 0x203e);
        إذا(jpCharsetMasks[cnv_داده->نسخه]&CSM(ISO8859_1)) {
            /* include Latin-1 for some variants of JP */
            sa->جمع_کن_Range(sa->مجموعه, 0, 0xff);
        } وإلا {
            /* include ASCII for JP */
            sa->جمع_کن_Range(sa->مجموعه, 0, 0x7f);
        }
        إذا(cnv_داده->نسخه==3 || cnv_داده->نسخه==4 || which==UCNV_ROUNDTRIP_AND_FALLBACK_مجموعه) {
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
            sa->جمع_کن_Range(sa->مجموعه, HWKANA_شروع, HWKANA_انتها);
        }
        اكسر;
#إذا !UCONFIG_ONLY_HTML_CONVERSION
    حالة 'c':
    حالة 'z':
        /* include ASCII for CN */
        sa->جمع_کن_Range(sa->مجموعه, 0, 0x7f);
        اكسر;
    حالة 'k':
        /* there is only one converter for KR, and it is not in the myConverterArray[] */
        cnv_داده->currentConverter->shared_داده->impl->get_Unicode_مجموعه(
                cnv_داده->currentConverter, sa, which, p_Error_کد);
        /* the loop over myConverterArray[] will simply not find another converter */
        اكسر;
#پایان_اگر
    افتراضي:
        اكسر;
    }

#إذا 0  /* Replaced by ucnv_MBCSGetFilteredUnicodeSetForUnicode() until we implement ucnv_getUnicodeSet() with reverse fallbacks. */
            إذا( (cnv_داده->locale[0]=='c' || cnv_داده->locale[0]=='z') &&
                cnv_داده->نسخه==0 && i==CNS_11643
            ) {
                /* special handling for non-EXT ISO-2022-CN: add only code points for CNS planes 1 and 2 */
                ucnv_MBCSGet_Unicode_مجموعه_For_بایتها(
                        cnv_داده->myConverterArray[i],
                        sa, UCNV_ROUNDTRIP_مجموعه,
                        0, 0x81, 0x82,
                        p_Error_کد);
            }
#پایان_اگر

    لكل (i=0; i<UCNV_2022_MAX_CONVERTERS; i++) {
        UConverter_مجموعه_Filter filter;
        إذا(cnv_داده->myConverterArray[i]!=اشاره_تهی) {
            إذا(cnv_داده->locale[0]=='j' && i==JISX208) {
                /*
                 * Only add code points that map to Shift-JIS codes
                 * corresponding to JIS X 0208.
                 */
                filter=UCNV_مجموعه_FILTER_SJIS;
#إذا !UCONFIG_ONLY_HTML_CONVERSION
            } وإلا إذا( (cnv_داده->locale[0]=='c' || cnv_داده->locale[0]=='z') &&
                       cnv_داده->نسخه==0 && i==CNS_11643) {
                /*
                 * Version-specific for CN:
                 * CN version 0 does not map CNS planes 3..7 although
                 * they are all available in the CNS conversion table;
                 * CN version 1 (-EXT) does map them all.
                 * The two versions create different Unicode sets.
                 */
                filter=UCNV_مجموعه_FILTER_2022_CN;
            } وإلا إذا(i==KSC5601) {
                /*
                 * Some of the KSC 5601 tables (convrtrs.txt has this aliases on multiple tables)
                 * are broader than GR94.
                 */
                filter=UCNV_مجموعه_FILTER_GR94_DBCS;
#پایان_اگر
            } وإلا {
                filter=UCNV_مجموعه_FILTER_NONE;
            }
            ucnv_MBCSGet_Filtered_Unicode_مجموعه_For_Unicode(cnv_داده->myConverterArray[i], sa, which, filter, p_Error_کد);
        }
    }

    /*
     * ISO 2022 converters must not convert SO/SI/ESC despite what
     * sub-converters do by themselves.
     * Remove these characters from the set.
     */
    sa->remove(sa->مجموعه, 0x0e);
    sa->remove(sa->مجموعه, 0x0f);
    sa->remove(sa->مجموعه, 0x1b);

    /* ISO 2022 converters do not convert C1 controls either */
    sa->removeRange(sa->مجموعه, 0x80, 0x9f);
}

ساكن ثابت UConverterImpl _ISO2022Impl={
    UCNV_ISO_2022,

    اشاره_تهی,
    اشاره_تهی,

    _ISO2022Open,
    _ISO2022Close,
    _ISO2022Reset,

#اگر_تعریف_شده U_ENABLE_GENERIC_ISO_2022
    T_UConverter_مقصد_Unicode_ISO_2022_OFFSETS_LOGIC,
    T_UConverter_مقصد_Unicode_ISO_2022_OFFSETS_LOGIC,
    ucnv_fromUnicode_UTF8,
    ucnv_fromUnicode_UTF8_OFFSETS_LOGIC,
#وإلا
    اشاره_تهی,
    اشاره_تهی,
    اشاره_تهی,
    اشاره_تهی,
#پایان_اگر
    اشاره_تهی,

    اشاره_تهی,
    _ISO2022getName,
    _ISO_2022_WriteSub,
    ISO_2022_امن_Clone,
    ISO_2022_Get_Unicode_مجموعه,

    اشاره_تهی,
    اشاره_تهی
};
ساكن ثابت UConverter_Static_داده ISO2022_Static_داده={
    حجم(UConverter_Static_داده),
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
ثابت UConverter_Shared_داده ISO2022_داده=
        UCNV_IMMUTABLE_SHARED_داده_INITIALIZER(&ISO2022_Static_داده, &_ISO2022Impl);

/*************JP****************/
ساكن ثابت UConverterImpl _ISO2022JPImpl={
    UCNV_ISO_2022,

    اشاره_تهی,
    اشاره_تهی,

    _ISO2022Open,
    _ISO2022Close,
    _ISO2022Reset,

    UConverter_مقصد_Unicode_ISO_2022_JP_OFFSETS_LOGIC,
    UConverter_مقصد_Unicode_ISO_2022_JP_OFFSETS_LOGIC,
    UConverter_fromUnicode_ISO_2022_JP_OFFSETS_LOGIC,
    UConverter_fromUnicode_ISO_2022_JP_OFFSETS_LOGIC,
    اشاره_تهی,

    اشاره_تهی,
    _ISO2022getName,
    _ISO_2022_WriteSub,
    ISO_2022_امن_Clone,
    ISO_2022_Get_Unicode_مجموعه,

    اشاره_تهی,
    اشاره_تهی
};
ساكن ثابت UConverter_Static_داده ISO2022_JPStatic_داده={
    حجم(UConverter_Static_داده),
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

ثابت UConverter_Shared_داده _ISO2022JPData=
        UCNV_IMMUTABLE_SHARED_داده_INITIALIZER(&ISO2022_JPStatic_داده, &_ISO2022JPImpl);

}  // namespace

#إذا !UCONFIG_ONLY_HTML_CONVERSION
/************* KR ***************/
ساكن ثابت UConverterImpl _ISO2022KRImpl={
    UCNV_ISO_2022,

    اشاره_تهی,
    اشاره_تهی,

    _ISO2022Open,
    _ISO2022Close,
    _ISO2022Reset,

    UConverter_مقصد_Unicode_ISO_2022_KR_OFFSETS_LOGIC,
    UConverter_مقصد_Unicode_ISO_2022_KR_OFFSETS_LOGIC,
    UConverter_fromUnicode_ISO_2022_KR_OFFSETS_LOGIC,
    UConverter_fromUnicode_ISO_2022_KR_OFFSETS_LOGIC,
    اشاره_تهی,

    اشاره_تهی,
    _ISO2022getName,
    _ISO_2022_WriteSub,
    ISO_2022_امن_Clone,
    ISO_2022_Get_Unicode_مجموعه,

    اشاره_تهی,
    اشاره_تهی
};
ساكن ثابت UConverter_Static_داده ISO2022_KRStatic_داده={
    حجم(UConverter_Static_داده),
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

ثابت UConverter_Shared_داده _ISO2022KRData=
        UCNV_IMMUTABLE_SHARED_داده_INITIALIZER(&ISO2022_KRStatic_داده, &_ISO2022KRImpl);

}  // namespace

/*************** CN ***************/
ساكن ثابت UConverterImpl _ISO2022CNImpl={

    UCNV_ISO_2022,

    اشاره_تهی,
    اشاره_تهی,

    _ISO2022Open,
    _ISO2022Close,
    _ISO2022Reset,

    UConverter_مقصد_Unicode_ISO_2022_CN_OFFSETS_LOGIC,
    UConverter_مقصد_Unicode_ISO_2022_CN_OFFSETS_LOGIC,
    UConverter_fromUnicode_ISO_2022_CN_OFFSETS_LOGIC,
    UConverter_fromUnicode_ISO_2022_CN_OFFSETS_LOGIC,
    اشاره_تهی,

    اشاره_تهی,
    _ISO2022getName,
    _ISO_2022_WriteSub,
    ISO_2022_امن_Clone,
    ISO_2022_Get_Unicode_مجموعه,

    اشاره_تهی,
    اشاره_تهی
};
ساكن ثابت UConverter_Static_داده ISO2022_CNStatic_داده={
    حجم(UConverter_Static_داده),
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

ثابت UConverter_Shared_داده _ISO2022CNData=
        UCNV_IMMUTABLE_SHARED_داده_INITIALIZER(&ISO2022_CNStatic_داده, &_ISO2022CNImpl);

}  // namespace
#پایان_اگر /* #if !UCONFIG_ONLY_HTML_CONVERSION */

#پایان_اگر /* #if !UCONFIG_NO_LEGACY_CONVERSION */
