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

#अन्तर्भाव "unicode/utypes.h"

#यदि !UCONFIG_NO_CONVERSION && !UCONFIG_NO_LEGACY_CONVERSION

#अन्तर्भाव "unicode/ucnv.h"
#अन्तर्भाव "unicode/uset.h"
#अन्तर्भाव "unicode/ucnv_err.h"
#अन्तर्भाव "unicode/ucnv_cb.h"
#अन्तर्भाव "unicode/utf16.h"
#अन्तर्भाव "ucnv_imp.h"
#अन्तर्भाव "ucnv_bld.h"
#अन्तर्भाव "ucnv_cnv.h"
#अन्तर्भाव "ucnvmbcs.h"
#अन्तर्भाव "cstring.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "uassert.h"

#यदि_परिभाषितम् U_ENABLE_GENERIC_ISO_2022
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
#यदि_अन्तः

#यदि !UCONFIG_ONLY_HTML_CONVERSION
स्थिर ध्रुव वर्ण सरणम्_अन्तर्_STR[]  = "\x0F";
// static const char SHIFT_OUT_STR[] = "\x0E";
#यदि_अन्तः

#परिभाषय CR      0x0D
#परिभाषय LF      0x0A
#परिभाषय H_TAB   0x09
#परिभाषय V_TAB   0x0B
#परिभाषय SPACE   0x20

गणना {
    HWKANA_START=0xff61,
    HWKANA_अन्तः=0xff9f
};

/*
 * 94-character sets with native byte values A1..FE are encoded in ISO 2022
 * as bytes 21..7E. (Subtract 0x80.)
 * 96-character sets with native byte values A0..FF are encoded in ISO 2022
 * as bytes 20..7F. (Subtract 0x80.)
 * Do not encode C1 control codes with native bytes 80..9F
 * as bytes 00..1F (C0 control codes).
 */
गणना {
    GR94_START=0xa1,
    GR94_अन्तः=0xfe,
    GR96_START=0xa0,
    GR96_अन्तः=0xff
};

/*
 * ISO 2022 control codes must not be converted from Unicode
 * because they would mess up the byte stream.
 * The bit mask 0x0800c000 has bits set at bit positions 0xe, 0xf, 0x1b
 * corresponding to SO, SI, and ESC.
 */
#परिभाषय IS_2022_CONTROL(c) (((c)<0x20) && (((uint32_t)1<<(c))&0x0800c000)!=0)

/* for ISO-2022-JP and -CN implementations */
प्रकारनिर्देश गणना  {
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
#यदि UCONFIG_ONLY_HTML_CONVERSION
#परिभाषय IS_JP_DBCS(cs) (JISX208==(cs))
#अन्यथा
#परिभाषय IS_JP_DBCS(cs) (JISX208<=(cs) && (cs)<=KSC5601)
#यदि_अन्तः

#परिभाषय CSM(cs) ((uint16_t)1<<(cs))

/*
 * Each of these charset masks (with index x) contains a bit for a charset in exact correspondence
 * to whether that charset is used in the corresponding version x of ISO_2022,locale=ja,version=x
 *
 * Note: The converter uses some leniency:
 * - The escape sequence ESC ( I for half-width 7-bit Katakana is recognized in
 *   all versions, not just JIS7 and JIS8.
 * - ICU does not distinguish between different versions of JIS X 0208.
 */
#यदि UCONFIG_ONLY_HTML_CONVERSION
गणना { MAX_JA_संस्करणम्=0 };
#अन्यथा
गणना { MAX_JA_संस्करणम्=4 };
#यदि_अन्तः
स्थिर ध्रुव uint16_t jpCharsetMasks[MAX_JA_संस्करणम्+1]={
    CSM(ASCII)|CSM(JISX201)|CSM(JISX208)|CSM(HWKANA_7BIT),
#यदि !UCONFIG_ONLY_HTML_CONVERSION
    CSM(ASCII)|CSM(JISX201)|CSM(JISX208)|CSM(HWKANA_7BIT)|CSM(JISX212),
    CSM(ASCII)|CSM(JISX201)|CSM(JISX208)|CSM(HWKANA_7BIT)|CSM(JISX212)|CSM(GB2312)|CSM(KSC5601)|CSM(ISO8859_1)|CSM(ISO8859_7),
    CSM(ASCII)|CSM(JISX201)|CSM(JISX208)|CSM(HWKANA_7BIT)|CSM(JISX212)|CSM(GB2312)|CSM(KSC5601)|CSM(ISO8859_1)|CSM(ISO8859_7),
    CSM(ASCII)|CSM(JISX201)|CSM(JISX208)|CSM(HWKANA_7BIT)|CSM(JISX212)|CSM(GB2312)|CSM(KSC5601)|CSM(ISO8859_1)|CSM(ISO8859_7)
#यदि_अन्तः
};

प्रकारनिर्देश गणना {
        ASCII1=0,
        LATIN1,
        SBCS,
        DBCS,
        MBCS,
        HWKANA
}Cnv2022Type;

प्रकारनिर्देश संरचना ISO2022State {
    int8_t cs[4];       /* charset number for SI (G0)/SO (G1)/SS2 (G2)/SS3 (G3) */
    int8_t g;           /* 0..3 for G0..G3 (SI/SO/SS2/SS3) */
    int8_t पूर्व_G;       /* g before single shift (SS2 or SS3) */
} ISO2022State;

#परिभाषय UCNV_OPTIONS_संस्करणम्_आवरणम् 0xf
#परिभाषय UCNV_2022_MAX_CONVERTERS 10

प्रकारनिर्देश संरचना{
    UConverter_Shared_दत्त *myConverterArray[UCNV_2022_MAX_CONVERTERS];
    UConverter *currentConverter;
    Cnv2022Type currentType;
    ISO2022State गन्तव्यम्_U2022_State, fromU2022State;
    uint32_t कुञ्चिका;
    uint32_t संस्करणम्;
#यदि_परिभाषितम् U_ENABLE_GENERIC_ISO_2022
    UBool is_First_बफर;
#यदि_अन्तः
    UBool is_रिक्तानि_Segment;
    वर्ण name[30];
    वर्ण locale[3];
}UConverter_दत्त_ISO2022;

/* Protos */
/* ISO-2022 ----------------------------------------------------------------- */

/*Forward declaration */
U_CFUNC शून्य U_CALLCONV
ucnv_fromUnicode_UTF8(UConverter_From_Unicode_तर्काः * तर्काः_2,
                      UError_संहिता * err);
U_CFUNC शून्य U_CALLCONV
ucnv_fromUnicode_UTF8_OFFSETS_LOGIC(UConverter_From_Unicode_तर्काः * तर्काः_2,
                                    UError_संहिता * err);

#परिभाषय ESC_2022 0x1B /*ESC*/

प्रकारनिर्देश गणना
{
        INVALID_2022 = -1, /*Doesn't correspond to a valid iso 2022 escape sequence*/
        VALID_NON_TERMINAL_2022 = 0, /*so far corresponds to a valid iso 2022 escape sequence*/
        VALID_TERMINAL_2022 = 1, /*corresponds to a valid iso 2022 escape sequence*/
        VALID_MAYBE_TERMINAL_2022 = 2 /*so far matches one iso 2022 escape sequence, but by adding more characters might match another escape sequence*/
} UCNV_सारणी_अवस्थाः_2022;

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
स्थिर ध्रुव int8_t normalize_esq_chars_2022[256] = {
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

#यदि_परिभाषितम् U_ENABLE_GENERIC_ISO_2022
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
#यदि_अन्तः

#परिभाषय MAX_अवस्थाः_2022 74
स्थिर ध्रुव int32_t esc_Seq_State_सारणी_कुञ्चिका_2022[MAX_अवस्थाः_2022] = {
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

#यदि_परिभाषितम् U_ENABLE_GENERIC_ISO_2022

स्थिर ध्रुव वर्ण* ध्रुव esc_Seq_State_सारणी_परिणाम_2022[MAX_अवस्थाः_2022] = {
 /*  0                      1                        2                      3                   4                   5                        6                      7                       8                       9    */

     शून्यसूचकः                   ,शून्यसूचकः                   ,शून्यसूचकः                   ,शून्यसूचकः               ,शून्यसूचकः               ,शून्यसूचकः                   ,शून्यसूचकः                   ,शून्यसूचकः                   ,"latin1"               ,"latin1"
    ,"latin1"               ,"ibm-865"              ,"ibm-865"              ,"ibm-865"          ,"ibm-865"          ,"ibm-865"              ,"ibm-865"              ,"JISX0201"             ,"JISX0201"             ,"latin1"
    ,"latin1"               ,शून्यसूचकः                   ,"JISX-208"             ,"ibm-5478"         ,"JISX-208"         ,शून्यसूचकः                   ,शून्यसूचकः                   ,शून्यसूचकः                   ,शून्यसूचकः                   ,"UTF8"
    ,"ISO-8859-1"           ,"ISO-8859-7"           ,"JIS-X-208"            ,शून्यसूचकः               ,"ibm-955"          ,"ibm-367"              ,"ibm-952"              ,"ibm-949"              ,"JISX-212"             ,"ibm-1383"
    ,"ibm-952"              ,"ibm-964"              ,"ibm-964"              ,"ibm-964"          ,"ibm-964"          ,"ibm-964"              ,"ibm-964"              ,"ibm-5478"         ,"ibm-949"              ,"ISO-IR-165"
    ,"CNS-11643-1992,1"     ,"CNS-11643-1992,2"     ,"CNS-11643-1992,3"     ,"CNS-11643-1992,4" ,"CNS-11643-1992,5" ,"CNS-11643-1992,6"     ,"CNS-11643-1992,7"     ,"UTF16_PlatformEndian" ,"UTF16_PlatformEndian" ,"UTF16_PlatformEndian"
    ,"UTF16_PlatformEndian" ,"UTF16_PlatformEndian" ,"UTF16_PlatformEndian" ,शून्यसूचकः               ,"latin1"           ,"ibm-912"              ,"ibm-913"              ,"ibm-914"              ,"ibm-813"              ,"ibm-1089"
    ,"ibm-920"              ,"ibm-915"              ,"ibm-915"              ,"latin1"
};

#यदि_अन्तः

स्थिर ध्रुव int8_t esc_Seq_State_सारणी_मानम्_2022[MAX_अवस्थाः_2022] = {
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
प्रकारनिर्देश गणना{
#यदि_परिभाषितम् U_ENABLE_GENERIC_ISO_2022
    ISO_2022=0,
#यदि_अन्तः
    ISO_2022_JP=1,
#यदि !UCONFIG_ONLY_HTML_CONVERSION
    ISO_2022_KR=2,
    ISO_2022_CN=3
#यदि_अन्तः
} Variant2022;

/*********** ISO 2022 Converter Protos ***********/
स्थिर शून्य U_CALLCONV
_ISO2022Open(UConverter *cnv, UConverter_आरोपय_तर्काः *p_तर्काः, UError_संहिता *error_संहिता);

स्थिर शून्य U_CALLCONV
 _ISO2022Close(UConverter *converter);

स्थिर शून्य U_CALLCONV
_ISO2022Reset(UConverter *converter, UConverterResetChoice choice);

U_CDECL_BEGIN
स्थिर ध्रुव वर्ण * U_CALLCONV
_ISO2022getName(ध्रुव UConverter* cnv);
U_CDECL_अन्तः

स्थिर शून्य  U_CALLCONV
_ISO_2022_WriteSub(UConverter_From_Unicode_तर्काः *तर्काः_2, int32_t विस्थापनम्_सूचकांक, UError_संहिता *err);

U_CDECL_BEGIN
स्थिर UConverter * U_CALLCONV
_ISO_2022_SafeClone(ध्रुव UConverter *cnv, शून्य *राशिः_बफर, int32_t *p_बफर_आकार, UError_संहिता *स्थितिः);

U_CDECL_अन्तः

#यदि_परिभाषितम् U_ENABLE_GENERIC_ISO_2022
स्थिर शून्य U_CALLCONV
T_UConverter_गन्तव्यम्_Unicode_ISO_2022_OFFSETS_LOGIC(UConverter_गन्तव्यम्_Unicode_तर्काः* तर्काः_2, UError_संहिता* err);
#यदि_अन्तः

नामाकाश {

/*const UConverterSharedData _ISO2022Data;*/
बाह्य ध्रुव UConverter_Shared_दत्त _ISO2022JPData;

#यदि !UCONFIG_ONLY_HTML_CONVERSION
बाह्य ध्रुव UConverter_Shared_दत्त _ISO2022KRData;
बाह्य ध्रुव UConverter_Shared_दत्त _ISO2022CNData;
#यदि_अन्तः

}  // namespace

/*************** Converter implementations ******************/

/* The purpose of this function is to get around gcc compiler warnings. */
स्थिर अन्तःस्थ शून्य
fromUWriteUInt8(UConverter *cnv,
                 ध्रुव वर्ण *अष्टकानि, int32_t length,
                 uint8_t **target, ध्रुव वर्ण *target_सीमा,
                 int32_t **offsets,
                 int32_t source_सूचकांक,
                 UError_संहिता *p_Error_संहिता)
{
    वर्ण* targetChars = reinterpret_cast<वर्ण*>(*target);
    ucnv_from_UWrite_अष्टकानि(cnv, अष्टकानि, length, &targetChars, target_सीमा,
                         offsets, source_सूचकांक, p_Error_संहिता);
    *target = reinterpret_cast<uint8_t*>(targetChars);

}

स्थिर अन्तःस्थ शून्य
समुच्चयः_Initial_State_गन्तव्यम्_Unicode_KR(UConverter* /*converter*/, UConverter_दत्त_ISO2022 *my_Converter_दत्त){
    यदि(my_Converter_दत्त->संस्करणम् == 1) {
        UConverter *cnv = my_Converter_दत्त->currentConverter;

        cnv->गन्तव्यम्_Unicode_स्थितिः=0;     /*विस्थापनम्*/
        cnv->बहुलकम्=0;                /* state */
        cnv->गन्तव्यम्_ULength=0;           /* byteIndex */
    }
}

स्थिर अन्तःस्थ शून्य
समुच्चयः_Initial_State_From_Unicode_KR(UConverter* converter,UConverter_दत्त_ISO2022 *my_Converter_दत्त){
   /* in ISO-2022-KR the designator sequence appears only once
    * in a file so we append it only once
    */
    यदि( converter->char_Error_बफर_दीर्घता==0){

        converter->char_Error_बफर_दीर्घता = 4;
        converter->char_Error_बफर[0] = 0x1b;
        converter->char_Error_बफर[1] = 0x24;
        converter->char_Error_बफर[2] = 0x29;
        converter->char_Error_बफर[3] = 0x43;
    }
    यदि(my_Converter_दत्त->संस्करणम् == 1) {
        UConverter *cnv = my_Converter_दत्त->currentConverter;

        cnv->fromUChar32=0;
        cnv->from_Unicode_स्थितिः=1;   /* prevLength */
    }
}

स्थिर शून्य U_CALLCONV
_ISO2022Open(UConverter *cnv, UConverter_आरोपय_तर्काः *p_तर्काः, UError_संहिता *error_संहिता){

    वर्ण myLocale[7]={' ',' ',' ',' ',' ',' ', '\0'};

    cnv->extraInfo = uprv_malloc (परिमाण (UConverter_दत्त_ISO2022));
    यदि(cnv->extraInfo != शून्यसूचकः) {
        UConverterNamePieces राशिः_Pieces;
        UConverter_आरोपय_तर्काः राशिः_तर्काः=UCNV_आरोपय_तर्काः_INITIALIZER;
        UConverter_दत्त_ISO2022* my_Converter_दत्त = static_cast<UConverter_दत्त_ISO2022*>(cnv->extraInfo);
        uint32_t संस्करणम्;

        राशिः_तर्काः.onlyTestIsLoadable = p_तर्काः->onlyTestIsLoadable;

        uprv_memset(my_Converter_दत्त, 0, परिमाण(UConverter_दत्त_ISO2022));
        my_Converter_दत्त->currentType = ASCII1;
        cnv->from_Unicode_स्थितिः =असत्य;
        यदि(p_तर्काः->locale){
            uprv_strncpy(myLocale, p_तर्काः->locale, परिमाण(myLocale)-1);
        }
        संस्करणम् = p_तर्काः->options & UCNV_OPTIONS_संस्करणम्_आवरणम्;
        my_Converter_दत्त->संस्करणम् = संस्करणम्;
        यदि(myLocale[0]=='j' && (myLocale[1]=='a'|| myLocale[1]=='p') &&
            (myLocale[2]=='_' || myLocale[2]=='\0'))
        {
            /* open the required converters and cache them */
            यदि(संस्करणम्>MAX_JA_संस्करणम्) {
                // ICU 55 fails to open a converter for an unsupported version.
                // Previously, it fell back to version 0, but that would yield
                // unexpected behavior.
                *error_संहिता = U_MISSING_RESOURCE_ERROR;
                प्रत्यागम;
            }
            यदि(jpCharsetMasks[संस्करणम्]&CSM(ISO8859_7)) {
                my_Converter_दत्त->myConverterArray[ISO8859_7] =
                    ucnv_आरोपय_Shared_दत्त("ISO8859_7", &राशिः_Pieces, &राशिः_तर्काः, error_संहिता);
            }
            my_Converter_दत्त->myConverterArray[JISX208] =
                ucnv_आरोपय_Shared_दत्त("Shift-JIS", &राशिः_Pieces, &राशिः_तर्काः, error_संहिता);
            यदि(jpCharsetMasks[संस्करणम्]&CSM(JISX212)) {
                my_Converter_दत्त->myConverterArray[JISX212] =
                    ucnv_आरोपय_Shared_दत्त("jisx-212", &राशिः_Pieces, &राशिः_तर्काः, error_संहिता);
            }
            यदि(jpCharsetMasks[संस्करणम्]&CSM(GB2312)) {
                my_Converter_दत्त->myConverterArray[GB2312] =
                    ucnv_आरोपय_Shared_दत्त("ibm-5478", &राशिः_Pieces, &राशिः_तर्काः, error_संहिता);   /* gb_2312_80-1 */
            }
            यदि(jpCharsetMasks[संस्करणम्]&CSM(KSC5601)) {
                my_Converter_दत्त->myConverterArray[KSC5601] =
                    ucnv_आरोपय_Shared_दत्त("ksc_5601", &राशिः_Pieces, &राशिः_तर्काः, error_संहिता);
            }

            /* set the function pointers to appropriate functions */
            cnv->shared_दत्त = const_cast<UConverter_Shared_दत्त*>(&_ISO2022JPData);
            uprv_strcpy(my_Converter_दत्त->locale,"ja");

            (शून्य)uprv_strcpy(my_Converter_दत्त->name,"ISO_2022,locale=ja,version=");
            size_t len = uprv_strlen(my_Converter_दत्त->name);
            my_Converter_दत्त->name[len] = static_cast<वर्ण>(my_Converter_दत्त->संस्करणम् + static_cast<पूर्णाङ्क>('0'));
            my_Converter_दत्त->name[len+1]='\0';
        }
#यदि !UCONFIG_ONLY_HTML_CONVERSION
        अन्यथा यदि(myLocale[0]=='k' && (myLocale[1]=='o'|| myLocale[1]=='r') &&
            (myLocale[2]=='_' || myLocale[2]=='\0'))
        {
            यदि(संस्करणम्>1) {
                // ICU 55 fails to open a converter for an unsupported version.
                // Previously, it fell back to version 0, but that would yield
                // unexpected behavior.
                *error_संहिता = U_MISSING_RESOURCE_ERROR;
                प्रत्यागम;
            }
            ध्रुव वर्ण *cnvName;
            यदि(संस्करणम्==1) {
                cnvName="icu-internal-25546";
            } अन्यथा {
                cnvName="ibm-949";
                my_Converter_दत्त->संस्करणम्=संस्करणम्=0;
            }
            यदि(p_तर्काः->onlyTestIsLoadable) {
                ucnv_canCreateConverter(cnvName, error_संहिता);  /* errorCode carries result */
                uprv_free(cnv->extraInfo);
                cnv->extraInfo=शून्यसूचकः;
                प्रत्यागम;
            } अन्यथा {
                my_Converter_दत्त->currentConverter=ucnv_open(cnvName, error_संहिता);
                यदि (U_FAILURE(*error_संहिता)) {
                    _ISO2022Close(cnv);
                    प्रत्यागम;
                }

                यदि(संस्करणम्==1) {
                    (शून्य)uprv_strcpy(my_Converter_दत्त->name,"ISO_2022,locale=ko,version=1");
                    uprv_memcpy(cnv->subChars, my_Converter_दत्त->currentConverter->subChars, 4);
                    cnv->subCharLen = my_Converter_दत्त->currentConverter->subCharLen;
                }अन्यथा{
                    (शून्य)uprv_strcpy(my_Converter_दत्त->name,"ISO_2022,locale=ko,version=0");
                }

                /* initialize the state variables */
                समुच्चयः_Initial_State_गन्तव्यम्_Unicode_KR(cnv, my_Converter_दत्त);
                समुच्चयः_Initial_State_From_Unicode_KR(cnv, my_Converter_दत्त);

                /* set the function pointers to appropriate functions */
                cnv->shared_दत्त = const_cast<UConverter_Shared_दत्त*>(&_ISO2022KRData);
                uprv_strcpy(my_Converter_दत्त->locale,"ko");
            }
        }
        अन्यथा यदि(((myLocale[0]=='z' && myLocale[1]=='h') || (myLocale[0]=='c'&& myLocale[1]=='n'))&&
            (myLocale[2]=='_' || myLocale[2]=='\0'))
        {
            यदि(संस्करणम्>2) {
                // ICU 55 fails to open a converter for an unsupported version.
                // Previously, it fell back to version 0, but that would yield
                // unexpected behavior.
                *error_संहिता = U_MISSING_RESOURCE_ERROR;
                प्रत्यागम;
            }

            /* open the required converters and cache them */
            my_Converter_दत्त->myConverterArray[GB2312_1] =
                ucnv_आरोपय_Shared_दत्त("ibm-5478", &राशिः_Pieces, &राशिः_तर्काः, error_संहिता);
            यदि(संस्करणम्>=1) {
                my_Converter_दत्त->myConverterArray[ISO_IR_165] =
                    ucnv_आरोपय_Shared_दत्त("iso-ir-165", &राशिः_Pieces, &राशिः_तर्काः, error_संहिता);
            }
            my_Converter_दत्त->myConverterArray[CNS_11643] =
                ucnv_आरोपय_Shared_दत्त("cns-11643-1992", &राशिः_Pieces, &राशिः_तर्काः, error_संहिता);


            /* set the function pointers to appropriate functions */
            cnv->shared_दत्त = const_cast<UConverter_Shared_दत्त*>(&_ISO2022CNData);
            uprv_strcpy(my_Converter_दत्त->locale,"cn");

            यदि (संस्करणम्==0){
                my_Converter_दत्त->संस्करणम् = 0;
                (शून्य)uprv_strcpy(my_Converter_दत्त->name,"ISO_2022,locale=zh,version=0");
            }अन्यथा यदि (संस्करणम्==1){
                my_Converter_दत्त->संस्करणम् = 1;
                (शून्य)uprv_strcpy(my_Converter_दत्त->name,"ISO_2022,locale=zh,version=1");
            }अन्यथा {
                my_Converter_दत्त->संस्करणम् = 2;
                (शून्य)uprv_strcpy(my_Converter_दत्त->name,"ISO_2022,locale=zh,version=2");
            }
        }
#यदि_अन्तः  // !UCONFIG_ONLY_HTML_CONVERSION
        अन्यथा{
#यदि_परिभाषितम् U_ENABLE_GENERIC_ISO_2022
            my_Converter_दत्त->is_First_बफर = सत्य;

            /* append the UTF-8 escape sequence */
            cnv->char_Error_बफर_दीर्घता = 3;
            cnv->char_Error_बफर[0] = 0x1b;
            cnv->char_Error_बफर[1] = 0x25;
            cnv->char_Error_बफर[2] = 0x42;

            cnv->shared_दत्त=(UConverter_Shared_दत्त*)&ISO2022_दत्त;
            /* initialize the state variables */
            uprv_strcpy(my_Converter_दत्त->name,"ISO_2022");
#अन्यथा
            *error_संहिता = U_MISSING_RESOURCE_ERROR;
            // Was U_UNSUPPORTED_ERROR but changed in ICU 55 to a more standard
            // data loading error code.
            प्रत्यागम;
#यदि_अन्तः
        }

        cnv->max_अष्टकानि_प्रत्येकम्_UChar=cnv->shared_दत्त->static_दत्त->max_अष्टकानि_प्रत्येकम्_Char;

        यदि(U_FAILURE(*error_संहिता) || p_तर्काः->onlyTestIsLoadable) {
            _ISO2022Close(cnv);
        }
    } अन्यथा {
        *error_संहिता = U_स्मृतिः_आवण्टनम्_ERROR;
    }
}


स्थिर शून्य U_CALLCONV
_ISO2022Close(UConverter *converter) {
    UConverter_दत्त_ISO2022* my_दत्त = static_cast<UConverter_दत्त_ISO2022*>(converter->extraInfo);
    UConverter_Shared_दत्त **array = my_दत्त->myConverterArray;
    int32_t i;

    यदि (converter->extraInfo != शून्यसूचकः) {
        /*close the array of converter pointers and free the memory*/
        प्रति (i=0; i<UCNV_2022_MAX_CONVERTERS; i++) {
            यदि(array[i]!=शून्यसूचकः) {
                ucnv_unload_Shared_दत्त_If_Ready(array[i]);
            }
        }

        ucnv_close(my_दत्त->currentConverter);

        यदि(!converter->isExtraLocal){
            uprv_free (converter->extraInfo);
            converter->extraInfo = शून्यसूचकः;
        }
    }
}

स्थिर शून्य U_CALLCONV
_ISO2022Reset(UConverter *converter, UConverterResetChoice choice) {
    UConverter_दत्त_ISO2022* my_Converter_दत्त = static_cast<UConverter_दत्त_ISO2022*>(converter->extraInfo);
    यदि(choice<=UCNV_RESET_गन्तव्यम्_UNICODE) {
        uprv_memset(&my_Converter_दत्त->गन्तव्यम्_U2022_State, 0, परिमाण(ISO2022State));
        my_Converter_दत्त->कुञ्चिका = 0;
        my_Converter_दत्त->is_रिक्तानि_Segment = असत्य;
    }
    यदि(choice!=UCNV_RESET_गन्तव्यम्_UNICODE) {
        uprv_memset(&my_Converter_दत्त->fromU2022State, 0, परिमाण(ISO2022State));
    }
#यदि_परिभाषितम् U_ENABLE_GENERIC_ISO_2022
    यदि(my_Converter_दत्त->locale[0] == 0){
        यदि(choice<=UCNV_RESET_गन्तव्यम्_UNICODE) {
            my_Converter_दत्त->is_First_बफर = सत्य;
            my_Converter_दत्त->कुञ्चिका = 0;
            यदि (converter->बहुलकम् == UCNV_SO){
                ucnv_close (my_Converter_दत्त->currentConverter);
                my_Converter_दत्त->currentConverter=शून्यसूचकः;
            }
            converter->बहुलकम् = UCNV_SI;
        }
        यदि(choice!=UCNV_RESET_गन्तव्यम्_UNICODE) {
            /* re-append UTF-8 escape sequence */
            converter->char_Error_बफर_दीर्घता = 3;
            converter->char_Error_बफर[0] = 0x1b;
            converter->char_Error_बफर[1] = 0x28;
            converter->char_Error_बफर[2] = 0x42;
        }
    }
    अन्यथा
#यदि_अन्तः
    {
        /* reset the state variables */
        यदि(my_Converter_दत्त->locale[0] == 'k'){
            यदि(choice<=UCNV_RESET_गन्तव्यम्_UNICODE) {
                समुच्चयः_Initial_State_गन्तव्यम्_Unicode_KR(converter, my_Converter_दत्त);
            }
            यदि(choice!=UCNV_RESET_गन्तव्यम्_UNICODE) {
                समुच्चयः_Initial_State_From_Unicode_KR(converter, my_Converter_दत्त);
            }
        }
    }
}

U_CDECL_BEGIN

स्थिर ध्रुव वर्ण * U_CALLCONV
_ISO2022getName(ध्रुव UConverter* cnv){
    यदि(cnv->extraInfo){
        UConverter_दत्त_ISO2022* my_दत्त= (UConverter_दत्त_ISO2022*)cnv->extraInfo;
        प्रत्यागम my_दत्त->name;
    }
    प्रत्यागम शून्यसूचकः;
}

U_CDECL_अन्तः


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
स्थिर ध्रुव int8_t अग्र_State_गन्तव्यम्_Unicode_JP[MAX_अवस्थाः_2022]= {
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

#यदि !UCONFIG_ONLY_HTML_CONVERSION
/*************** to unicode *******************/
स्थिर ध्रुव int8_t अग्र_State_गन्तव्यम्_Unicode_CN[MAX_अवस्थाः_2022]= {
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
#यदि_अन्तः


स्थिर UCNV_सारणी_अवस्थाः_2022
get_कुञ्चिका_2022(वर्ण c,int32_t* कुञ्चिका,int32_t* विस्थापनम्){
    int32_t togo;
    int32_t नीचः = 0;
    int32_t hi = MAX_अवस्थाः_2022;
    int32_t oldmid=0;

    togo = normalize_esq_chars_2022[static_cast<uint8_t>(c)];
    यदि(togo == 0) {
        /* not a valid character anywhere in an escape sequence */
        *कुञ्चिका = 0;
        *विस्थापनम् = 0;
        प्रत्यागम INVALID_2022;
    }
    togo = (*कुञ्चिका << 5) + togo;

    यावत् (hi != नीचः)  /*द्विमान अन्वेषणम्*/{

        int32_t mid = (hi+नीचः) >> 1; /*Finds median*/

        यदि (mid == oldmid)
            भङ्ग;

        यदि (esc_Seq_State_सारणी_कुञ्चिका_2022[mid] > togo){
            hi = mid;
        }
        अन्यथा यदि (esc_Seq_State_सारणी_कुञ्चिका_2022[mid] < togo){
            नीचः = mid;
        }
        अन्यथा /*we found it*/{
            *कुञ्चिका = togo;
            *विस्थापनम् = mid;
            प्रत्यागम static_cast<UCNV_सारणी_अवस्थाः_2022>(esc_Seq_State_सारणी_मानम्_2022[mid]);
        }
        oldmid = mid;

    }

    *कुञ्चिका = 0;
    *विस्थापनम् = 0;
    प्रत्यागम INVALID_2022;
}

/*runs through a state machine to determine the escape sequence - codepage correspondence
 */
स्थिर शून्य
changeState_2022(UConverter* _this,
                ध्रुव वर्ण** source,
                ध्रुव वर्ण* source_सीमा,
                Variant2022 var,
                UError_संहिता* err){
    UCNV_सारणी_अवस्थाः_2022 value;
    UConverter_दत्त_ISO2022* myData2022 = static_cast<UConverter_दत्त_ISO2022*>(_this->extraInfo);
    uint32_t कुञ्चिका = myData2022->कुञ्चिका;
    int32_t विस्थापनम् = 0;
    int8_t initial_गन्तव्यम्_ULength = _this->गन्तव्यम्_ULength;
    वर्ण c;

    value = VALID_NON_TERMINAL_2022;
    यावत् (*source < source_सीमा) {
        c = *(*source)++;
        _this->गन्तव्यम्_UBytes[_this->गन्तव्यम्_ULength++] = static_cast<uint8_t>(c);
        value = get_कुञ्चिका_2022(c, reinterpret_cast<int32_t*>(&कुञ्चिका), &विस्थापनम्);

        विकल्प (value){

        पक्ष VALID_NON_TERMINAL_2022 :
            /* continue with the loop */
            भङ्ग;

        पक्ष VALID_TERMINAL_2022:
            कुञ्चिका = 0;
            गच्छ समाप्तम्_2;

        पक्ष INVALID_2022:
            गच्छ समाप्तम्_2;

        पक्ष VALID_MAYBE_TERMINAL_2022:
#यदि_परिभाषितम् U_ENABLE_GENERIC_ISO_2022
            /* ESC ( B is ambiguous only for ISO_2022 itself */
            यदि(var == ISO_2022) {
                /* discard toUBytes[] for ESC ( B because this sequence is correct and complete */
                _this->गन्तव्यम्_ULength = 0;

                /* TODO need to indicate that ESC ( B was seen; if failure, then need to replay from source or from MBCS-style replay */

                /* continue with the loop */
                value = VALID_NON_TERMINAL_2022;
                भङ्ग;
            } अन्यथा
#यदि_अन्तः
            {
                /* not ISO_2022 itself, finish here */
                value = VALID_TERMINAL_2022;
                कुञ्चिका = 0;
                गच्छ समाप्तम्_2;
            }
        }
    }

समाप्तम्_2:
    myData2022->कुञ्चिका = कुञ्चिका;

    यदि (value == VALID_NON_TERMINAL_2022) {
        /* indicate that the escape sequence is incomplete: key!=0 */
        प्रत्यागम;
    } अन्यथा यदि (value == INVALID_2022 ) {
        *err = U_ILLEGAL_ESCAPE_SEQUENCE;
    } अन्यथा /* value == VALID_TERMINAL_2022 */ {
        विकल्प(var){
#यदि_परिभाषितम् U_ENABLE_GENERIC_ISO_2022
        पक्ष ISO_2022:
        {
            ध्रुव वर्ण *chosenConverterName = esc_Seq_State_सारणी_परिणाम_2022[विस्थापनम्];
            यदि(chosenConverterName == शून्यसूचकः) {
                /* SS2 or SS3 */
                *err = U_UNSUPPORTED_ESCAPE_SEQUENCE;
                _this->गन्तव्यम्_UCallback_Reason = UCNV_UNASSIGNED;
                प्रत्यागम;
            }

            _this->बहुलकम् = UCNV_SI;
            ucnv_close(myData2022->currentConverter);
            myData2022->currentConverter = myUConverter = ucnv_open(chosenConverterName, err);
            यदि(U_SUCCESS(*err)) {
                myUConverter->fromCharErrorBehaviour = UCNV_गन्तव्यम्_U_CALLBACK_STOP;
                _this->बहुलकम् = UCNV_SO;
            }
            भङ्ग;
        }
#यदि_अन्तः
        पक्ष ISO_2022_JP:
            {
                StateEnum अस्थायी_State = static_cast<StateEnum>(अग्र_State_गन्तव्यम्_Unicode_JP[विस्थापनम्]);
                विकल्प(अस्थायी_State) {
                पक्ष INVALID_STATE:
                    *err = U_UNSUPPORTED_ESCAPE_SEQUENCE;
                    भङ्ग;
                पक्ष SS2_STATE:
                    यदि(myData2022->गन्तव्यम्_U2022_State.cs[2]!=0) {
                        यदि(myData2022->गन्तव्यम्_U2022_State.g<2) {
                            myData2022->गन्तव्यम्_U2022_State.पूर्व_G=myData2022->गन्तव्यम्_U2022_State.g;
                        }
                        myData2022->गन्तव्यम्_U2022_State.g=2;
                    } अन्यथा {
                        /* illegal to have SS2 before a matching designator */
                        *err = U_ILLEGAL_ESCAPE_SEQUENCE;
                    }
                    भङ्ग;
                /* case SS3_STATE: not used in ISO-2022-JP-x */
                पक्ष ISO8859_1:
                पक्ष ISO8859_7:
                    यदि((jpCharsetMasks[myData2022->संस्करणम्] & CSM(अस्थायी_State)) == 0) {
                        *err = U_UNSUPPORTED_ESCAPE_SEQUENCE;
                    } अन्यथा {
                        /* G2 charset for SS2 */
                        myData2022->गन्तव्यम्_U2022_State.cs[2] = static_cast<int8_t>(अस्थायी_State);
                    }
                    भङ्ग;
                स्वभाव:
                    यदि((jpCharsetMasks[myData2022->संस्करणम्] & CSM(अस्थायी_State)) == 0) {
                        *err = U_UNSUPPORTED_ESCAPE_SEQUENCE;
                    } अन्यथा {
                        /* G0 charset */
                        myData2022->गन्तव्यम्_U2022_State.cs[0] = static_cast<int8_t>(अस्थायी_State);
                    }
                    भङ्ग;
                }
            }
            भङ्ग;
#यदि !UCONFIG_ONLY_HTML_CONVERSION
        पक्ष ISO_2022_CN:
            {
                StateEnum अस्थायी_State = static_cast<StateEnum>(अग्र_State_गन्तव्यम्_Unicode_CN[विस्थापनम्]);
                विकल्प(अस्थायी_State) {
                पक्ष INVALID_STATE:
                    *err = U_UNSUPPORTED_ESCAPE_SEQUENCE;
                    भङ्ग;
                पक्ष SS2_STATE:
                    यदि(myData2022->गन्तव्यम्_U2022_State.cs[2]!=0) {
                        यदि(myData2022->गन्तव्यम्_U2022_State.g<2) {
                            myData2022->गन्तव्यम्_U2022_State.पूर्व_G=myData2022->गन्तव्यम्_U2022_State.g;
                        }
                        myData2022->गन्तव्यम्_U2022_State.g=2;
                    } अन्यथा {
                        /* illegal to have SS2 before a matching designator */
                        *err = U_ILLEGAL_ESCAPE_SEQUENCE;
                    }
                    भङ्ग;
                पक्ष SS3_STATE:
                    यदि(myData2022->गन्तव्यम्_U2022_State.cs[3]!=0) {
                        यदि(myData2022->गन्तव्यम्_U2022_State.g<2) {
                            myData2022->गन्तव्यम्_U2022_State.पूर्व_G=myData2022->गन्तव्यम्_U2022_State.g;
                        }
                        myData2022->गन्तव्यम्_U2022_State.g=3;
                    } अन्यथा {
                        /* illegal to have SS3 before a matching designator */
                        *err = U_ILLEGAL_ESCAPE_SEQUENCE;
                    }
                    भङ्ग;
                पक्ष ISO_IR_165:
                    यदि(myData2022->संस्करणम्==0) {
                        *err = U_UNSUPPORTED_ESCAPE_SEQUENCE;
                        भङ्ग;
                    }
                    U_FALLTHROUGH;
                पक्ष GB2312_1:
                    U_FALLTHROUGH;
                पक्ष CNS_11643_1:
                    myData2022->गन्तव्यम्_U2022_State.cs[1] = static_cast<int8_t>(अस्थायी_State);
                    भङ्ग;
                पक्ष CNS_11643_2:
                    myData2022->गन्तव्यम्_U2022_State.cs[2] = static_cast<int8_t>(अस्थायी_State);
                    भङ्ग;
                स्वभाव:
                    /* other CNS 11643 planes */
                    यदि(myData2022->संस्करणम्==0) {
                        *err = U_UNSUPPORTED_ESCAPE_SEQUENCE;
                    } अन्यथा {
                        myData2022->गन्तव्यम्_U2022_State.cs[3] = static_cast<int8_t>(अस्थायी_State);
                    }
                    भङ्ग;
                }
            }
            भङ्ग;
        पक्ष ISO_2022_KR:
            यदि(विस्थापनम्==0x30){
                /* nothing to be done, just accept this one escape sequence */
            } अन्यथा {
                *err = U_UNSUPPORTED_ESCAPE_SEQUENCE;
            }
            भङ्ग;
#यदि_अन्तः  // !UCONFIG_ONLY_HTML_CONVERSION

        स्वभाव:
            *err = U_ILLEGAL_ESCAPE_SEQUENCE;
            भङ्ग;
        }
    }
    यदि(U_SUCCESS(*err)) {
        _this->गन्तव्यम्_ULength = 0;
    } अन्यथा यदि(*err==U_ILLEGAL_ESCAPE_SEQUENCE) {
        यदि(_this->गन्तव्यम्_ULength>1) {
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
            int8_t पृष्ठभागः_बहिर्गमनम्_Distance=_this->गन्तव्यम्_ULength-1;
            int8_t अष्टकानि_From_This_बफर=_this->गन्तव्यम्_ULength-initial_गन्तव्यम्_ULength;
            यदि(पृष्ठभागः_बहिर्गमनम्_Distance<=अष्टकानि_From_This_बफर) {
                /* same as initialToULength<=1 */
                *source-=पृष्ठभागः_बहिर्गमनम्_Distance;
            } अन्यथा {
                /* Back out bytes from the previous buffer: Need to replay them. */
                _this->pre_गन्तव्यम्_ULength = static_cast<int8_t>(अष्टकानि_From_This_बफर - पृष्ठभागः_बहिर्गमनम्_Distance);
                /* same as -(initialToULength-1) */
                /* preToULength is negative! */
                uprv_memcpy(_this->pre_गन्तव्यम्_U, _this->गन्तव्यम्_UBytes+1, -_this->pre_गन्तव्यम्_ULength);
                *source-=अष्टकानि_From_This_बफर;
            }
            _this->गन्तव्यम्_ULength=1;
        }
    } अन्यथा यदि(*err==U_UNSUPPORTED_ESCAPE_SEQUENCE) {
        _this->गन्तव्यम्_UCallback_Reason = UCNV_UNASSIGNED;
    }
}

#यदि !UCONFIG_ONLY_HTML_CONVERSION
/*Checks the characters of the buffer against valid 2022 escape sequences
*if the match we return a pointer to the initial start of the sequence otherwise
*we return sourceLimit
*/
/*for 2022 looks ahead in the stream
 *to determine the longest possible convertible
 *data stream
 */
स्थिर अन्तःस्थ ध्रुव वर्ण*
get_अन्तः_सम्बन्धि_बफर_2022(ध्रुव वर्ण** source,
                   ध्रुव वर्ण* source_सीमा,
                   UBool /*flush*/){

    ध्रुव वर्ण* mySource = *source;

#यदि_परिभाषितम् U_ENABLE_GENERIC_ISO_2022
    यदि (*source >= source_सीमा)
        प्रत्यागम source_सीमा;

    कुरु{

        यदि (*mySource == ESC_2022){
            int8_t i;
            int32_t कुञ्चिका = 0;
            int32_t विस्थापनम्;
            UCNV_सारणी_अवस्थाः_2022 value = VALID_NON_TERMINAL_2022;

            /* Kludge: I could not
            * figure out the reason for validating an escape sequence
            * twice - once here and once in changeState_2022().
            * is it possible to have an ESC character in a ISO2022
            * byte stream which is valid in a code page? Is it legal?
            */
            प्रति (i=0;
            (mySource+i < source_सीमा)&&(value == VALID_NON_TERMINAL_2022);
            i++) {
                value =  get_कुञ्चिका_2022(*(mySource+i), &कुञ्चिका, &विस्थापनम्);
            }
            यदि (value > 0 || *mySource==ESC_2022)
                प्रत्यागम mySource;

            यदि ((value == VALID_NON_TERMINAL_2022)&&(!flush) )
                प्रत्यागम source_सीमा;
        }
    }यावत् (++mySource < source_सीमा);

    प्रत्यागम source_सीमा;
#अन्यथा
    यावत्(mySource < source_सीमा && *mySource != ESC_2022) {
        ++mySource;
    }
    प्रत्यागम mySource;
#यदि_अन्तः
}
#यदि_अन्तः

/* This inline function replicates code in _MBCSFromUChar32() function in ucnvmbcs.c
 * any future change in _MBCSFromUChar32() function should be reflected here.
 * @return number of bytes in *value; negative number if fallback; 0 if no mapping
 */
स्थिर अन्तःस्थ int32_t
MBCS_FROM_UCHAR32_ISO2022(UConverter_Shared_दत्त* shared_दत्त,
                                         UChar32 c,
                                         uint32_t* value,
                                         UBool useFallback,
                                         पूर्णाङ्क outputType)
{
    ध्रुव int32_t *cx;
    ध्रुव uint16_t *सारणी;
    uint32_t stage2_प्रविष्टिः;
    uint32_t my_मानम्;
    int32_t length;
    ध्रुव uint8_t *p;
    /*
     * TODO(markus): Use and require new, faster MBCS conversion table structures.
     * Use internal version of ucnv_open() that verifies that the new structures are available,
     * else U_INTERNAL_PROGRAM_ERROR.
     */
    /* BMP-only codepages are stored without stage 1 entries for supplementary code points */
    यदि(c<0x10000 || (shared_दत्त->mbcs.unicode_आवरणम्&UCNV_HAS_SUPPLEMENTARY)) {
        सारणी=shared_दत्त->mbcs.from_Unicode_सारणी;
        stage2_प्रविष्टिः=MBCS_STAGE_2_FROM_U(सारणी, c);
        /* get the bytes and the length for the output */
        यदि(outputType==MBCS_OUTPUT_2){
            my_मानम्=MBCS_मानम्_2_FROM_STAGE_2(shared_दत्त->mbcs.from_Unicode_अष्टकानि, stage2_प्रविष्टिः, c);
            यदि(my_मानम्<=0xff) {
                length=1;
            } अन्यथा {
                length=2;
            }
        } अन्यथा /* outputType==MBCS_OUTPUT_3 */ {
            p=MBCS_POINTER_3_FROM_STAGE_2(shared_दत्त->mbcs.from_Unicode_अष्टकानि, stage2_प्रविष्टिः, c);
            my_मानम् = (static_cast<uint32_t>(*p) << 16) | (static_cast<uint32_t>(p[1]) << 8) | p[2];
            यदि(my_मानम्<=0xff) {
                length=1;
            } अन्यथा यदि(my_मानम्<=0xffff) {
                length=2;
            } अन्यथा {
                length=3;
            }
        }
        /* is this code point assigned, or do we use fallbacks? */
        यदि((stage2_प्रविष्टिः&(1<<(16+(c&0xf))))!=0) {
            /* assigned */
            *value=my_मानम्;
            प्रत्यागम length;
        } अन्यथा यदि(FROM_U_USE_FALLBACK(useFallback, c) && my_मानम्!=0) {
            /*
             * We allow a 0 byte output if the "assigned" bit is set for this entry.
             * There is no way with this data structure for fallback output
             * to be a zero byte.
             */
            *value=my_मानम्;
            प्रत्यागम -length;
        }
    }

    cx=shared_दत्त->mbcs.extIndexes;
    यदि(cx!=शून्यसूचकः) {
        प्रत्यागम ucnv_ext_Simple_मेलनम्_From_U(cx, c, value, useFallback);
    }

    /* unassigned */
    प्रत्यागम 0;
}

/* This inline function replicates code in _MBCSSingleFromUChar32() function in ucnvmbcs.c
 * any future change in _MBCSSingleFromUChar32() function should be reflected here.
 * @param retval pointer to output byte
 * @return 1 roundtrip byte  0 no mapping  -1 fallback byte
 */
स्थिर अन्तःस्थ int32_t
MBCS_SINGLE_FROM_UCHAR32(UConverter_Shared_दत्त* shared_दत्त,
                                       UChar32 c,
                                       uint32_t* retval,
                                       UBool useFallback)
{
    ध्रुव uint16_t *सारणी;
    int32_t value;
    /* BMP-only codepages are stored without stage 1 entries for supplementary code points */
    यदि(c>=0x10000 && !(shared_दत्त->mbcs.unicode_आवरणम्&UCNV_HAS_SUPPLEMENTARY)) {
        प्रत्यागम 0;
    }
    /* convert the Unicode code point in c into codepage bytes (same as in _MBCSFromUnicodeWithOffsets) */
    सारणी=shared_दत्त->mbcs.from_Unicode_सारणी;
    /* get the byte for the output */
    value=MBCS_SINGLE_परिणाम_FROM_U(सारणी, (uint16_t *)shared_दत्त->mbcs.from_Unicode_अष्टकानि, c);
    /* is this code point assigned, or do we use fallbacks? */
    *retval = static_cast<uint32_t>(value & 0xff);
    यदि(value>=0xf00) {
        प्रत्यागम 1;  /* roundtrip */
    } अन्यथा यदि(useFallback ? value>=0x800 : value>=0xc00) {
        प्रत्यागम -1;  /* fallback taken */
    } अन्यथा {
        प्रत्यागम 0;  /* no mapping */
    }
}

/*
 * Check that the result is a 2-byte value with each byte in the range A1..FE
 * (strict EUC DBCS) before accepting it and subtracting 0x80 from each byte
 * to move it to the ISO 2022 range 21..7E.
 * Return 0 if out of range.
 */
स्थिर अन्तःस्थ uint32_t
_2022FromGR94DBCS(uint32_t value) {
    यदि (static_cast<uint16_t>(value - 0xa1a1) <= (0xfefe - 0xa1a1) &&
        static_cast<uint8_t>(value - 0xa1) <= (0xfe - 0xa1)
    ) {
        प्रत्यागम value - 0x8080;  /* shift down to 21..7e byte range */
    } अन्यथा {
        प्रत्यागम 0;  /* not valid for ISO 2022 */
    }
}

#यदि 0 /* 5691: Call sites now check for validity. They can just += 0x8080 after that. */
/*
 * This method does the reverse of _2022FromGR94DBCS(). Given the 2022 code point, it returns the
 * 2 byte value that is in the range A1..FE for each byte. Otherwise it returns the 2022 code point
 * unchanged. 
 */
स्थिर अन्तःस्थ uint32_t
_2022ToGR94DBCS(uint32_t value) {
    uint32_t return_मानम् = value + 0x8080;
    यदि( (uint16_t)(return_मानम् - 0xa1a1) <= (0xfefe - 0xa1a1) &&
        (uint8_t)(return_मानम् - 0xa1) <= (0xfe - 0xa1)) {
        प्रत्यागम return_मानम्;
    } अन्यथा {
        प्रत्यागम value;
    }
}
#यदि_अन्तः

#यदि_परिभाषितम् U_ENABLE_GENERIC_ISO_2022

/**********************************************************************************
*  ISO-2022 Converter
*
*
*/

स्थिर शून्य U_CALLCONV
T_UConverter_गन्तव्यम्_Unicode_ISO_2022_OFFSETS_LOGIC(UConverter_गन्तव्यम्_Unicode_तर्काः* तर्काः_2,
                                                           UError_संहिता* err){
    ध्रुव वर्ण* my_Source_सीमा, *real_Source_सीमा;
    ध्रुव वर्ण* sourceStart;
    ध्रुव char16_t* myTargetStart;
    UConverter* saveThis;
    UConverter_दत्त_ISO2022* my_दत्त;
    int8_t length;

    saveThis = तर्काः_2->converter;
    my_दत्त=((UConverter_दत्त_ISO2022*)(saveThis->extraInfo));

    real_Source_सीमा = तर्काः_2->source_सीमा;
    यावत् (तर्काः_2->source < real_Source_सीमा) {
        यदि(my_दत्त->कुञ्चिका == 0) { /* are we in the middle of an escape sequence? */
            /*Find the end of the buffer e.g : Next Escape Seq | end of Buffer*/
            my_Source_सीमा = get_अन्तः_सम्बन्धि_बफर_2022(&(तर्काः_2->source), real_Source_सीमा, तर्काः_2->flush);

            यदि(तर्काः_2->source < my_Source_सीमा) {
                यदि(my_दत्त->currentConverter==शून्यसूचकः) {
                    my_दत्त->currentConverter = ucnv_open("ASCII",err);
                    यदि(U_FAILURE(*err)){
                        प्रत्यागम;
                    }

                    my_दत्त->currentConverter->fromCharErrorBehaviour = UCNV_गन्तव्यम्_U_CALLBACK_STOP;
                    saveThis->बहुलकम् = UCNV_SO;
                }

                /* convert to before the ESC or until the end of the buffer */
                my_दत्त->is_First_बफर=असत्य;
                sourceStart = तर्काः_2->source;
                myTargetStart = तर्काः_2->target;
                तर्काः_2->converter = my_दत्त->currentConverter;
                ucnv_गन्तव्यम्_Unicode(तर्काः_2->converter,
                    &तर्काः_2->target,
                    तर्काः_2->target_सीमा,
                    &तर्काः_2->source,
                    my_Source_सीमा,
                    तर्काः_2->offsets,
                    (UBool)(तर्काः_2->flush && my_Source_सीमा == real_Source_सीमा),
                    err);
                तर्काः_2->converter = saveThis;

                यदि (*err == U_बफर_अतिप्रवाहः_ERROR) {
                    /* move the overflow buffer */
                    length = saveThis->UChar_Error_बफर_दीर्घता = my_दत्त->currentConverter->UChar_Error_बफर_दीर्घता;
                    my_दत्त->currentConverter->UChar_Error_बफर_दीर्घता = 0;
                    यदि(length > 0) {
                        uprv_memcpy(saveThis->UChar_Error_बफर,
                                    my_दत्त->currentConverter->UChar_Error_बफर,
                                    length*U_SIZEOF_UCHAR);
                    }
                    प्रत्यागम;
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
                यदि (U_FAILURE(*err) ||
                    (तर्काः_2->source == real_Source_सीमा) ||
                    (तर्काः_2->offsets != शून्यसूचकः && (तर्काः_2->target != myTargetStart || तर्काः_2->source != sourceStart) ||
                    (my_Source_सीमा < real_Source_सीमा && my_दत्त->currentConverter->गन्तव्यम्_ULength > 0))
                ) {
                    /* copy partial or error input for truncated detection and error handling */
                    यदि(U_FAILURE(*err)) {
                        length = saveThis->invalid_Char_दीर्घता = my_दत्त->currentConverter->invalid_Char_दीर्घता;
                        यदि(length > 0) {
                            uprv_memcpy(saveThis->invalid_Char_बफर, my_दत्त->currentConverter->invalid_Char_बफर, length);
                        }
                    } अन्यथा {
                        length = saveThis->गन्तव्यम्_ULength = my_दत्त->currentConverter->गन्तव्यम्_ULength;
                        यदि(length > 0) {
                            uprv_memcpy(saveThis->गन्तव्यम्_UBytes, my_दत्त->currentConverter->गन्तव्यम्_UBytes, length);
                            यदि(तर्काः_2->source < my_Source_सीमा) {
                                *err = U_TRUNCATED_CHAR_प्राप्तम्; /* truncated input before ESC */
                            }
                        }
                    }
                    प्रत्यागम;
                }
            }
        }

        sourceStart = तर्काः_2->source;
        changeState_2022(तर्काः_2->converter,
               &(तर्काः_2->source),
               real_Source_सीमा,
               ISO_2022,
               err);
        यदि (U_FAILURE(*err) || (तर्काः_2->source != sourceStart && तर्काः_2->offsets != शून्यसूचकः)) {
            /* let the ucnv.c code update its current offset */
            प्रत्यागम;
        }
    }
}

#यदि_अन्तः

/*
 * To Unicode Callback helper function
 */
स्थिर शून्य
गन्तव्यम्_Unicode_Callback(UConverter *cnv,
                  ध्रुव uint32_t sourceChar, ध्रुव uint32_t targetUniChar,
                  UError_संहिता* err){
    यदि(sourceChar>0xff){
        cnv->गन्तव्यम्_UBytes[0] = static_cast<uint8_t>(sourceChar >> 8);
        cnv->गन्तव्यम्_UBytes[1] = static_cast<uint8_t>(sourceChar);
        cnv->गन्तव्यम्_ULength = 2;
    }
    अन्यथा{
        cnv->गन्तव्यम्_UBytes[0] = static_cast<वर्ण>(sourceChar);
        cnv->गन्तव्यम्_ULength = 1;
    }

    यदि(targetUniChar == (missingCharMarker-1/*0xfffe*/)){
        *err = U_INVALID_CHAR_प्राप्तम्;
    }
    अन्यथा{
        *err = U_ILLEGAL_CHAR_प्राप्तम्;
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
स्थिर ध्रुव StateEnum jpCharsetPref[]={
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
स्थिर ध्रुव वर्ण escSeqChars[][6] ={
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
स्थिर  ध्रुव int8_t escSeqCharsLen[] ={
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
स्थिर अन्तःस्थ uint32_t
jisx201_गन्तव्यम्_U(uint32_t value) {
    यदि(value < 0x5c) {
        प्रत्यागम value;
    } अन्यथा यदि(value == 0x5c) {
        प्रत्यागम 0xa5;
    } अन्यथा यदि(value == 0x7e) {
        प्रत्यागम 0x203e;
    } अन्यथा /* value <= 0x7f */ {
        प्रत्यागम value;
    }
}

/* Map Unicode to 00..7F according to JIS X 0201. Return U+FFFE if unmappable. */
स्थिर अन्तःस्थ uint32_t
jisx201FromU(uint32_t value) {
    यदि(value<=0x7f) {
        यदि(value!=0x5c && value!=0x7e) {
            प्रत्यागम value;
        }
    } अन्यथा यदि(value==0xa5) {
        प्रत्यागम 0x5c;
    } अन्यथा यदि(value==0x203e) {
        प्रत्यागम 0x7e;
    }
    प्रत्यागम 0xfffe;
}

/*
 * Take a valid Shift-JIS byte pair, check that it is in the range corresponding
 * to JIS X 0208, and convert it to a pair of 21..7E bytes.
 * Return 0 if the byte pair is out of range.
 */
स्थिर अन्तःस्थ uint32_t
_2022FromSJIS(uint32_t value) {
    uint8_t trail;

    यदि(value > 0xEFFC) {
        प्रत्यागम 0;  /* beyond JIS X 0208 */
    }

    trail = static_cast<uint8_t>(value);

    value &= 0xff00;  /* lead byte */
    यदि(value <= 0x9f00) {
        value -= 0x7000;
    } अन्यथा /* 0xe000 <= value <= 0xef00 */ {
        value -= 0xb000;
    }
    value <<= 1;

    यदि(trail <= 0x9e) {
        value -= 0x100;
        यदि(trail <= 0x7e) {
            value |= trail - 0x1f;
        } अन्यथा {
            value |= trail - 0x20;
        }
    } अन्यथा /* trail <= 0xfc */ {
        value |= trail - 0x7e;
    }
    प्रत्यागम value;
}

/*
 * Convert a pair of JIS X 0208 21..7E bytes to Shift-JIS.
 * If either byte is outside 21..7E make sure that the result is not valid
 * for Shift-JIS so that the converter catches it.
 * Some invalid byte values already turn into equally invalid Shift-JIS
 * byte values and need not be tested explicitly.
 */
स्थिर अन्तःस्थ शून्य
_2022ToSJIS(uint8_t c1, uint8_t c2, वर्ण अष्टकानि[2]) {
    यदि(c1&1) {
        ++c1;
        यदि(c2 <= 0x5f) {
            c2 += 0x1f;
        } अन्यथा यदि(c2 <= 0x7e) {
            c2 += 0x20;
        } अन्यथा {
            c2 = 0;  /* invalid */
        }
    } अन्यथा {
        यदि (static_cast<uint8_t>(c2 - 0x21) <= ((0x7e) - 0x21)) {
            c2 += 0x7e;
        } अन्यथा {
            c2 = 0;  /* invalid */
        }
    }
    c1 >>= 1;
    यदि(c1 <= 0x2f) {
        c1 += 0x70;
    } अन्यथा यदि(c1 <= 0x3f) {
        c1 += 0xb0;
    } अन्यथा {
        c1 = 0;  /* invalid */
    }
    अष्टकानि[0] = static_cast<वर्ण>(c1);
    अष्टकानि[1] = static_cast<वर्ण>(c2);
}

/*
 * JIS X 0208 has fallbacks from Unicode half-width Katakana to full-width (DBCS)
 * Katakana.
 * Now that we use a Shift-JIS table for JIS X 0208 we need to hardcode these fallbacks
 * because Shift-JIS roundtrips half-width Katakana to single bytes.
 * These were the only fallbacks in ICU's jisx-208.ucm file.
 */
स्थिर ध्रुव uint16_t hwkana_fb[HWKANA_अन्तः - HWKANA_START + 1] = {
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

स्थिर शून्य U_CALLCONV
UConverter_fromUnicode_ISO_2022_JP_OFFSETS_LOGIC(UConverter_From_Unicode_तर्काः* तर्काः_2, UError_संहिता* err) {
    UConverter *cnv = तर्काः_2->converter;
    UConverter_दत्त_ISO2022 *converter_दत्त;
    ISO2022State *pFromU2022State;
    uint8_t* target = reinterpret_cast<uint8_t*>(तर्काः_2->target);
    ध्रुव uint8_t* target_सीमा = reinterpret_cast<ध्रुव uint8_t*>(तर्काः_2->target_सीमा);
    ध्रुव char16_t* source = तर्काः_2->source;
    ध्रुव char16_t* source_सीमा = तर्काः_2->source_सीमा;
    int32_t* offsets = तर्काः_2->offsets;
    UChar32 sourceChar;
    वर्ण बफर[8];
    int32_t len, बहिर्गमनम्_Len;
    int8_t choices[10];
    int32_t choice_गणनम्;
    uint32_t target_मानम् = 0;
    UBool useFallback;

    int32_t i;
    int8_t cs, g;

    /* set up the state */
    converter_दत्त = static_cast<UConverter_दत्त_ISO2022*>(cnv->extraInfo);
    pFromU2022State   = &converter_दत्त->fromU2022State;

    choice_गणनम् = 0;

    /* check if the last codepoint of previous buffer was a lead surrogate*/
    यदि((sourceChar = cnv->fromUChar32)!=0 && target< target_सीमा) {
        गच्छ getTrail;
    }

    यावत्(source < source_सीमा) {
        यदि(target < target_सीमा) {

            sourceChar  = *(source++);
            /*check if the char is a First surrogate*/
            यदि(U16_IS_SURROGATE(sourceChar)) {
                यदि(U16_IS_SURROGATE_LEAD(sourceChar)) {
getTrail:
                    /*look ahead to find the trail surrogate*/
                    यदि(source < source_सीमा) {
                        /* test the following code unit */
                        char16_t trail = *source;
                        यदि(U16_IS_TRAIL(trail)) {
                            source++;
                            sourceChar=U16_GET_SUPPLEMENTARY(sourceChar, trail);
                            cnv->fromUChar32=0x00;
                            /* convert this supplementary code point */
                            /* exit this condition tree */
                        } अन्यथा {
                            /* this is an unmatched lead code unit (1st surrogate) */
                            /* callback(illegal) */
                            *err=U_ILLEGAL_CHAR_प्राप्तम्;
                            cnv->fromUChar32=sourceChar;
                            भङ्ग;
                        }
                    } अन्यथा {
                        /* no more input */
                        cnv->fromUChar32=sourceChar;
                        भङ्ग;
                    }
                } अन्यथा {
                    /* this is an unmatched trail code unit (2nd surrogate) */
                    /* callback(illegal) */
                    *err=U_ILLEGAL_CHAR_प्राप्तम्;
                    cnv->fromUChar32=sourceChar;
                    भङ्ग;
                }
            }

            /* do not convert SO/SI/ESC */
            यदि(IS_2022_CONTROL(sourceChar)) {
                /* callback(illegal) */
                *err=U_ILLEGAL_CHAR_प्राप्तम्;
                cnv->fromUChar32=sourceChar;
                भङ्ग;
            }

            /* do the conversion */

            यदि(choice_गणनम् == 0) {
                uint16_t csm;

                /*
                 * The csm variable keeps track of which charsets are allowed
                 * and not used yet while building the choices[].
                 */
                csm = jpCharsetMasks[converter_दत्त->संस्करणम्];
                choice_गणनम् = 0;

                /* JIS7/8: try single-byte half-width Katakana before JISX208 */
                यदि(converter_दत्त->संस्करणम् == 3 || converter_दत्त->संस्करणम् == 4) {
                    choices[choice_गणनम्++] = static_cast<int8_t>(HWKANA_7BIT);
                }
                /* Do not try single-byte half-width Katakana for other versions. */
                csm &= ~CSM(HWKANA_7BIT);

                /* try the current G0 charset */
                choices[choice_गणनम्++] = cs = pFromU2022State->cs[0];
                csm &= ~CSM(cs);

                /* try the current G2 charset */
                यदि((cs = pFromU2022State->cs[2]) != 0) {
                    choices[choice_गणनम्++] = cs;
                    csm &= ~CSM(cs);
                }

                /* try all the other possible charsets */
                प्रति(i = 0; i < UPRV_LENGTHOF(jpCharsetPref); ++i) {
                    cs = static_cast<int8_t>(jpCharsetPref[i]);
                    यदि(CSM(cs) & csm) {
                        choices[choice_गणनम्++] = cs;
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

            प्रति(i = 0; i < choice_गणनम् && len <= 0; ++i) {
                uint32_t value;
                int32_t len2;
                int8_t cs0 = choices[i];
                विकल्प(cs0) {
                पक्ष ASCII:
                    यदि(sourceChar <= 0x7f) {
                        target_मानम् = static_cast<uint32_t>(sourceChar);
                        len = 1;
                        cs = cs0;
                        g = 0;
                    }
                    भङ्ग;
                पक्ष ISO8859_1:
                    यदि(GR96_START <= sourceChar && sourceChar <= GR96_अन्तः) {
                        target_मानम् = static_cast<uint32_t>(sourceChar) - 0x80;
                        len = 1;
                        cs = cs0;
                        g = 2;
                    }
                    भङ्ग;
                पक्ष HWKANA_7BIT:
                    यदि (static_cast<uint32_t>(sourceChar - HWKANA_START) <= (HWKANA_अन्तः - HWKANA_START)) {
                        यदि(converter_दत्त->संस्करणम्==3) {
                            /* JIS7: use G1 (SO) */
                            /* Shift U+FF61..U+FF9F to bytes 21..5F. */
                            target_मानम् = static_cast<uint32_t>(sourceChar - (HWKANA_START - 0x21));
                            len = 1;
                            pFromU2022State->cs[1] = cs = cs0; /* do not output an escape sequence */
                            g = 1;
                        } अन्यथा यदि(converter_दत्त->संस्करणम्==4) {
                            /* JIS8: use 8-bit bytes with any single-byte charset, see escape sequence output below */
                            /* Shift U+FF61..U+FF9F to bytes A1..DF. */
                            target_मानम् = static_cast<uint32_t>(sourceChar - (HWKANA_START - 0xa1));
                            len = 1;

                            cs = pFromU2022State->cs[0];
                            यदि(IS_JP_DBCS(cs)) {
                                /* switch from a DBCS charset to JISX201 */
                                cs = static_cast<int8_t>(JISX201);
                            }
                            /* else stay in the current G0 charset */
                            g = 0;
                        }
                        /* else do not use HWKANA_7BIT with other versions */
                    }
                    भङ्ग;
                पक्ष JISX201:
                    /* G0 SBCS */
                    value = jisx201FromU(sourceChar);
                    यदि(value <= 0x7f) {
                        target_मानम् = value;
                        len = 1;
                        cs = cs0;
                        g = 0;
                        useFallback = असत्य;
                    }
                    भङ्ग;
                पक्ष JISX208:
                    /* G0 DBCS from Shift-JIS table */
                    len2 = MBCS_FROM_UCHAR32_ISO2022(
                                converter_दत्त->myConverterArray[cs0],
                                sourceChar, &value,
                                useFallback, MBCS_OUTPUT_2);
                    यदि(len2 == 2 || (len2 == -2 && len == 0)) {  /* only accept DBCS: abs(len)==2 */
                        value = _2022FromSJIS(value);
                        यदि(value != 0) {
                            target_मानम् = value;
                            len = len2;
                            cs = cs0;
                            g = 0;
                            useFallback = असत्य;
                        }
                    } अन्यथा यदि(len == 0 && useFallback &&
                              static_cast<uint32_t>(sourceChar - HWKANA_START) <= (HWKANA_अन्तः - HWKANA_START)) {
                        target_मानम् = hwkana_fb[sourceChar - HWKANA_START];
                        len = -2;
                        cs = cs0;
                        g = 0;
                        useFallback = असत्य;
                    }
                    भङ्ग;
                पक्ष ISO8859_7:
                    /* G0 SBCS forced to 7-bit output */
                    len2 = MBCS_SINGLE_FROM_UCHAR32(
                                converter_दत्त->myConverterArray[cs0],
                                sourceChar, &value,
                                useFallback);
                    यदि(len2 != 0 && !(len2 < 0 && len != 0) && GR96_START <= value && value <= GR96_अन्तः) {
                        target_मानम् = value - 0x80;
                        len = len2;
                        cs = cs0;
                        g = 2;
                        useFallback = असत्य;
                    }
                    भङ्ग;
                स्वभाव:
                    /* G0 DBCS */
                    len2 = MBCS_FROM_UCHAR32_ISO2022(
                                converter_दत्त->myConverterArray[cs0],
                                sourceChar, &value,
                                useFallback, MBCS_OUTPUT_2);
                    यदि(len2 == 2 || (len2 == -2 && len == 0)) {  /* only accept DBCS: abs(len)==2 */
                        यदि(cs0 == KSC5601) {
                            /*
                             * Check for valid bytes for the encoding scheme.
                             * This is necessary because the sub-converter (windows-949)
                             * has a broader encoding scheme than is valid for 2022.
                             */
                            value = _2022FromGR94DBCS(value);
                            यदि(value == 0) {
                                भङ्ग;
                            }
                        }
                        target_मानम् = value;
                        len = len2;
                        cs = cs0;
                        g = 0;
                        useFallback = असत्य;
                    }
                    भङ्ग;
                }
            }

            यदि(len != 0) {
                यदि(len < 0) {
                    len = -len;  /* fallback */
                }
                बहिर्गमनम्_Len = 0; /* count output bytes */

                /* write SI if necessary (only for JIS7) */
                यदि(pFromU2022State->g == 1 && g == 0) {
                    बफर[बहिर्गमनम्_Len++] = UCNV_SI;
                    pFromU2022State->g = 0;
                }

                /* write the designation sequence if necessary */
                यदि(cs != pFromU2022State->cs[g]) {
                    int32_t escLen = escSeqCharsLen[cs];
                    uprv_memcpy(बफर + बहिर्गमनम्_Len, escSeqChars[cs], escLen);
                    बहिर्गमनम्_Len += escLen;
                    pFromU2022State->cs[g] = cs;

                    /* invalidate the choices[] */
                    choice_गणनम् = 0;
                }

                /* write the shift sequence if necessary */
                यदि(g != pFromU2022State->g) {
                    विकल्प(g) {
                    /* case 0 handled before writing escapes */
                    पक्ष 1:
                        बफर[बहिर्गमनम्_Len++] = UCNV_SO;
                        pFromU2022State->g = 1;
                        भङ्ग;
                    स्वभाव: /* case 2 */
                        बफर[बहिर्गमनम्_Len++] = 0x1b;
                        बफर[बहिर्गमनम्_Len++] = 0x4e;
                        भङ्ग;
                    /* no case 3: no SS3 in ISO-2022-JP-x */
                    }
                }

                /* write the output bytes */
                यदि(len == 1) {
                    बफर[बहिर्गमनम्_Len++] = static_cast<वर्ण>(target_मानम्);
                } अन्यथा /* len == 2 */ {
                    बफर[बहिर्गमनम्_Len++] = static_cast<वर्ण>(target_मानम् >> 8);
                    बफर[बहिर्गमनम्_Len++] = static_cast<वर्ण>(target_मानम्);
                }
            } अन्यथा {
                /*
                 * if we cannot find the character after checking all codepages
                 * then this is an error
                 */
                *err = U_INVALID_CHAR_प्राप्तम्;
                cnv->fromUChar32=sourceChar;
                भङ्ग;
            }

            यदि(sourceChar == CR || sourceChar == LF) {
                /* reset the G2 state at the end of a line (conversion got us into ASCII or JISX201 already) */
                pFromU2022State->cs[2] = 0;
                choice_गणनम् = 0;
            }

            /* output outLen>0 bytes in buffer[] */
            यदि(बहिर्गमनम्_Len == 1) {
                *target++ = बफर[0];
                यदि(offsets) {
                    *offsets++ = static_cast<int32_t>(source - तर्काः_2->source - 1); /* -1: known to be ASCII */
                }
            } अन्यथा यदि(बहिर्गमनम्_Len == 2 && (target + 2) <= target_सीमा) {
                *target++ = बफर[0];
                *target++ = बफर[1];
                यदि(offsets) {
                    int32_t source_सूचकांक = static_cast<int32_t>(source - तर्काः_2->source - U16_दीर्घता(sourceChar));
                    *offsets++ = source_सूचकांक;
                    *offsets++ = source_सूचकांक;
                }
            } अन्यथा {
                fromUWriteUInt8(
                    cnv,
                    बफर, बहिर्गमनम्_Len,
                    &target, reinterpret_cast<ध्रुव वर्ण*>(target_सीमा),
                    &offsets, static_cast<int32_t>(source - तर्काः_2->source - U16_दीर्घता(sourceChar)),
                    err);
                यदि(U_FAILURE(*err)) {
                    भङ्ग;
                }
            }
        } /* end if(myTargetIndex<myTargetLength) */
        अन्यथा{
            *err =U_बफर_अतिप्रवाहः_ERROR;
            भङ्ग;
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
    यदि( U_SUCCESS(*err) &&
        (pFromU2022State->g!=0 || pFromU2022State->cs[0]!=ASCII) &&
        तर्काः_2->flush && source>=source_सीमा && cnv->fromUChar32==0
    ) {
        int32_t source_सूचकांक;

        बहिर्गमनम्_Len = 0;

        यदि(pFromU2022State->g != 0) {
            बफर[बहिर्गमनम्_Len++] = UCNV_SI;
            pFromU2022State->g = 0;
        }

        यदि(pFromU2022State->cs[0] != ASCII) {
            int32_t escLen = escSeqCharsLen[ASCII];
            uprv_memcpy(बफर + बहिर्गमनम्_Len, escSeqChars[ASCII], escLen);
            बहिर्गमनम्_Len += escLen;
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
        source_सूचकांक = static_cast<int32_t>(source - तर्काः_2->source);
        यदि(source_सूचकांक>0) {
            --source_सूचकांक;
            यदि( U16_IS_TRAIL(तर्काः_2->source[source_सूचकांक]) &&
                (source_सूचकांक==0 || U16_IS_LEAD(तर्काः_2->source[source_सूचकांक-1]))
            ) {
                --source_सूचकांक;
            }
        } अन्यथा {
            source_सूचकांक=-1;
        }

        fromUWriteUInt8(
            cnv,
            बफर, बहिर्गमनम्_Len,
            &target, reinterpret_cast<ध्रुव वर्ण*>(target_सीमा),
            &offsets, source_सूचकांक,
            err);
    }

    /*save the state and return */
    तर्काः_2->source = source;
    तर्काः_2->target = reinterpret_cast<वर्ण*>(target);
}

/*************** to unicode *******************/

स्थिर शून्य U_CALLCONV
UConverter_गन्तव्यम्_Unicode_ISO_2022_JP_OFFSETS_LOGIC(UConverter_गन्तव्यम्_Unicode_तर्काः *तर्काः_2,
                                               UError_संहिता* err){
    वर्ण अस्थायी_Buf[2];
    ध्रुव वर्ण* mySource = const_cast<वर्ण*>(तर्काः_2->source);
    char16_t *myTarget = तर्काः_2->target;
    ध्रुव वर्ण *my_Source_सीमा = तर्काः_2->source_सीमा;
    uint32_t targetUniChar = 0x0000;
    uint32_t mySourceChar = 0x0000;
    uint32_t tmpSourceChar = 0x0000;
    UConverter_दत्त_ISO2022* my_दत्त;
    ISO2022State *p_गन्तव्यम्_U2022_State;
    StateEnum cs;

    my_दत्त = static_cast<UConverter_दत्त_ISO2022*>(तर्काः_2->converter->extraInfo);
    p_गन्तव्यम्_U2022_State = &my_दत्त->गन्तव्यम्_U2022_State;

    यदि(my_दत्त->कुञ्चिका != 0) {
        /* continue with a partial escape sequence */
        गच्छ escape;
    } अन्यथा यदि(तर्काः_2->converter->गन्तव्यम्_ULength == 1 && mySource < my_Source_सीमा && myTarget < तर्काः_2->target_सीमा) {
        /* continue with a partial double-byte character */
        mySourceChar = तर्काः_2->converter->गन्तव्यम्_UBytes[0];
        तर्काः_2->converter->गन्तव्यम्_ULength = 0;
        cs = static_cast<StateEnum>(p_गन्तव्यम्_U2022_State->cs[p_गन्तव्यम्_U2022_State->g]);
        targetUniChar = missingCharMarker;
        गच्छ getTrailByte;
    }

    यावत्(mySource < my_Source_सीमा){

        targetUniChar =missingCharMarker;

        यदि(myTarget < तर्काः_2->target_सीमा){

            mySourceChar = static_cast<अचिह्नित वर्ण>(*mySource++);

            विकल्प(mySourceChar) {
            पक्ष UCNV_SI:
                यदि(my_दत्त->संस्करणम्==3) {
                    p_गन्तव्यम्_U2022_State->g=0;
                    अनुवर्त;
                } अन्यथा {
                    /* only JIS7 uses SI/SO, not ISO-2022-JP-x */
                    my_दत्त->is_रिक्तानि_Segment = असत्य;	/* reset this, we have a different error */
                    भङ्ग;
                }

            पक्ष UCNV_SO:
                यदि(my_दत्त->संस्करणम्==3) {
                    /* JIS7: switch to G1 half-width Katakana */
                    p_गन्तव्यम्_U2022_State->cs[1] = static_cast<int8_t>(HWKANA_7BIT);
                    p_गन्तव्यम्_U2022_State->g=1;
                    अनुवर्त;
                } अन्यथा {
                    /* only JIS7 uses SI/SO, not ISO-2022-JP-x */
                    my_दत्त->is_रिक्तानि_Segment = असत्य;	/* reset this, we have a different error */
                    भङ्ग;
                }

            पक्ष ESC_2022:
                mySource--;
escape:
                {
                    ध्रुव वर्ण * mySourceBefore = mySource;
                    int8_t गन्तव्यम्_ULength_Before = तर्काः_2->converter->गन्तव्यम्_ULength;

                    changeState_2022(तर्काः_2->converter,&(mySource),
                        my_Source_सीमा, ISO_2022_JP,err);

                    /* If in ISO-2022-JP only and we successfully completed an escape sequence, but previous segment was empty, create an error */
                    यदि(my_दत्त->संस्करणम्==0 && my_दत्त->कुञ्चिका==0 && U_SUCCESS(*err) && my_दत्त->is_रिक्तानि_Segment) {
                        *err = U_ILLEGAL_ESCAPE_SEQUENCE;
                        तर्काः_2->converter->गन्तव्यम्_UCallback_Reason = UCNV_IRREGULAR;
                        तर्काः_2->converter->गन्तव्यम्_ULength = static_cast<int8_t>(गन्तव्यम्_ULength_Before + (mySource - mySourceBefore));
                    }
                }

                /* invalid or illegal escape sequence */
                यदि(U_FAILURE(*err)){
                    तर्काः_2->target = myTarget;
                    तर्काः_2->source = mySource;
                    my_दत्त->is_रिक्तानि_Segment = असत्य;	/* Reset to avoid future spurious errors */
                    प्रत्यागम;
                }
                /* If we successfully completed an escape sequence, we begin a new segment, empty so far */
                यदि(my_दत्त->कुञ्चिका==0) {
                    my_दत्त->is_रिक्तानि_Segment = सत्य;
                }
                अनुवर्त;

            /* ISO-2022-JP does not use single-byte (C1) SS2 and SS3 */

            पक्ष CR:
            पक्ष LF:
                /* automatically reset to single-byte mode */
                यदि (static_cast<StateEnum>(p_गन्तव्यम्_U2022_State->cs[0]) != ASCII &&
                    static_cast<StateEnum>(p_गन्तव्यम्_U2022_State->cs[0]) != JISX201) {
                    p_गन्तव्यम्_U2022_State->cs[0] = static_cast<int8_t>(ASCII);
                }
                p_गन्तव्यम्_U2022_State->cs[2] = 0;
                p_गन्तव्यम्_U2022_State->g = 0;
                U_FALLTHROUGH;
            स्वभाव:
                /* convert one or two bytes */
                my_दत्त->is_रिक्तानि_Segment = असत्य;
                cs = static_cast<StateEnum>(p_गन्तव्यम्_U2022_State->cs[p_गन्तव्यम्_U2022_State->g]);
                यदि (static_cast<uint8_t>(mySourceChar - 0xa1) <= (0xdf - 0xa1) && my_दत्त->संस्करणम् == 4 &&
                    !IS_JP_DBCS(cs)
                ) {
                    /* 8-bit halfwidth katakana in any single-byte mode for JIS8 */
                    targetUniChar = mySourceChar + (HWKANA_START - 0xa1);

                    /* return from a single-shift state to the previous one */
                    यदि(p_गन्तव्यम्_U2022_State->g >= 2) {
                        p_गन्तव्यम्_U2022_State->g=p_गन्तव्यम्_U2022_State->पूर्व_G;
                    }
                } अन्यथा विकल्प(cs) {
                पक्ष ASCII:
                    यदि(mySourceChar <= 0x7f) {
                        targetUniChar = mySourceChar;
                    }
                    भङ्ग;
                पक्ष ISO8859_1:
                    यदि(mySourceChar <= 0x7f) {
                        targetUniChar = mySourceChar + 0x80;
                    }
                    /* return from a single-shift state to the previous one */
                    p_गन्तव्यम्_U2022_State->g=p_गन्तव्यम्_U2022_State->पूर्व_G;
                    भङ्ग;
                पक्ष ISO8859_7:
                    यदि(mySourceChar <= 0x7f) {
                        /* convert mySourceChar+0x80 to use a normal 8-bit table */
                        targetUniChar =
                            MBCS_SINGLE_SIMPLE_GET_अग्र_BMP(
                                my_दत्त->myConverterArray[cs],
                                mySourceChar + 0x80);
                    }
                    /* return from a single-shift state to the previous one */
                    p_गन्तव्यम्_U2022_State->g=p_गन्तव्यम्_U2022_State->पूर्व_G;
                    भङ्ग;
                पक्ष JISX201:
                    यदि(mySourceChar <= 0x7f) {
                        targetUniChar = jisx201_गन्तव्यम्_U(mySourceChar);
                    }
                    भङ्ग;
                पक्ष HWKANA_7BIT:
                    यदि (static_cast<uint8_t>(mySourceChar - 0x21) <= (0x5f - 0x21)) {
                        /* 7-bit halfwidth Katakana */
                        targetUniChar = mySourceChar + (HWKANA_START - 0x21);
                    }
                    भङ्ग;
                स्वभाव:
                    /* G0 DBCS */
                    यदि(mySource < my_Source_सीमा) {
                        पूर्णाङ्क leadIsOk, trailIsOk;
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
                        यदि (leadIsOk && trailIsOk) {
                            ++mySource;
                            tmpSourceChar = (mySourceChar << 8) | trailByte;
                            यदि(cs == JISX208) {
                                _2022ToSJIS(static_cast<uint8_t>(mySourceChar), trailByte, अस्थायी_Buf);
                                mySourceChar = tmpSourceChar;
                            } अन्यथा {
                                /* Copy before we modify tmpSourceChar so toUnicodeCallback() sees the correct bytes. */
                                mySourceChar = tmpSourceChar;
                                यदि (cs == KSC5601) {
                                    tmpSourceChar += 0x8080;  /* = _2022ToGR94DBCS(tmpSourceChar) */
                                }
                                अस्थायी_Buf[0] = static_cast<वर्ण>(tmpSourceChar >> 8);
                                अस्थायी_Buf[1] = static_cast<वर्ण>(tmpSourceChar);
                            }
                            targetUniChar = ucnv_MBCSSimple_Get_अग्र_UChar(my_दत्त->myConverterArray[cs], अस्थायी_Buf, 2, असत्य);
                        } अन्यथा यदि (!(trailIsOk || IS_2022_CONTROL(trailByte))) {
                            /* report a pair of illegal bytes if the second byte is not a DBCS starter */
                            ++mySource;
                            /* add another bit so that the code below writes 2 bytes in case of error */
                            mySourceChar = 0x10000 | (mySourceChar << 8) | trailByte;
                        }
                    } अन्यथा {
                        तर्काः_2->converter->गन्तव्यम्_UBytes[0] = static_cast<uint8_t>(mySourceChar);
                        तर्काः_2->converter->गन्तव्यम्_ULength = 1;
                        गच्छ endloop;
                    }
                }  /* End of inner switch */
                भङ्ग;
            }  /* End of outer switch */
            यदि(targetUniChar < (missingCharMarker-1/*0xfffe*/)){
                यदि(तर्काः_2->offsets){
                    तर्काः_2->offsets[myTarget - तर्काः_2->target] = static_cast<int32_t>(mySource - तर्काः_2->source - (mySourceChar <= 0xff ? 1 : 2));
                }
                *(myTarget++) = static_cast<char16_t>(targetUniChar);
            }
            अन्यथा यदि(targetUniChar > missingCharMarker){
                /* disassemble the surrogate pair and write to output*/
                targetUniChar-=0x0010000;
                *myTarget = static_cast<char16_t>(0xd800 + static_cast<char16_t>(targetUniChar >> 10));
                यदि(तर्काः_2->offsets){
                    तर्काः_2->offsets[myTarget - तर्काः_2->target] = static_cast<int32_t>(mySource - तर्काः_2->source - (mySourceChar <= 0xff ? 1 : 2));
                }
                ++myTarget;
                यदि(myTarget< तर्काः_2->target_सीमा){
                    *myTarget = static_cast<char16_t>(0xdc00 + static_cast<char16_t>(targetUniChar & 0x3ff));
                    यदि(तर्काः_2->offsets){
                        तर्काः_2->offsets[myTarget - तर्काः_2->target] = static_cast<int32_t>(mySource - तर्काः_2->source - (mySourceChar <= 0xff ? 1 : 2));
                    }
                    ++myTarget;
                }अन्यथा{
                    तर्काः_2->converter->UChar_Error_बफर[तर्काः_2->converter->UChar_Error_बफर_दीर्घता++]=
                                    static_cast<char16_t>(0xdc00 + static_cast<char16_t>(targetUniChar & 0x3ff));
                }

            }
            अन्यथा{
                /* Call the callback function*/
                गन्तव्यम्_Unicode_Callback(तर्काः_2->converter,mySourceChar,targetUniChar,err);
                भङ्ग;
            }
        }
        अन्यथा{    /* goes with "if(myTarget < args->targetLimit)"  way up near top of function */
            *err =U_बफर_अतिप्रवाहः_ERROR;
            भङ्ग;
        }
    }
endloop:
    तर्काः_2->target = myTarget;
    तर्काः_2->source = mySource;
}


#यदि !UCONFIG_ONLY_HTML_CONVERSION
/***************************************************************
*   Rules for ISO-2022-KR encoding
*   i) The KSC5601 designator sequence should appear only once in a file,
*      at the beginning of a line before any KSC5601 characters. This usually
*      means that it appears by itself on the first line of the file
*  ii) There are only 2 shifting sequences SO to shift into double byte mode
*      and SI to shift into single byte mode
*/
स्थिर शून्य U_CALLCONV
UConverter_fromUnicode_ISO_2022_KR_OFFSETS_LOGIC_IBM(UConverter_From_Unicode_तर्काः* तर्काः_2, UError_संहिता* err){

    UConverter* saveConv = तर्काः_2->converter;
    UConverter_दत्त_ISO2022* my_Converter_दत्त = static_cast<UConverter_दत्त_ISO2022*>(saveConv->extraInfo);
    तर्काः_2->converter=my_Converter_दत्त->currentConverter;

    my_Converter_दत्त->currentConverter->fromUChar32 = saveConv->fromUChar32;
    ucnv_MBCSFromUnicodeWithOffsets(तर्काः_2,err);
    saveConv->fromUChar32 = my_Converter_दत्त->currentConverter->fromUChar32;

    यदि(*err == U_बफर_अतिप्रवाहः_ERROR) {
        यदि(my_Converter_दत्त->currentConverter->char_Error_बफर_दीर्घता > 0) {
            uprv_memcpy(
                saveConv->char_Error_बफर,
                my_Converter_दत्त->currentConverter->char_Error_बफर,
                my_Converter_दत्त->currentConverter->char_Error_बफर_दीर्घता);
        }
        saveConv->char_Error_बफर_दीर्घता = my_Converter_दत्त->currentConverter->char_Error_बफर_दीर्घता;
        my_Converter_दत्त->currentConverter->char_Error_बफर_दीर्घता = 0;
    }
    तर्काः_2->converter=saveConv;
}

स्थिर शून्य U_CALLCONV
UConverter_fromUnicode_ISO_2022_KR_OFFSETS_LOGIC(UConverter_From_Unicode_तर्काः* तर्काः_2, UError_संहिता* err){

    ध्रुव char16_t *source = तर्काः_2->source;
    ध्रुव char16_t *source_सीमा = तर्काः_2->source_सीमा;
    अचिह्नित वर्ण *target = reinterpret_cast<अचिह्नित वर्ण*>(तर्काः_2->target);
    अचिह्नित वर्ण *target_सीमा = reinterpret_cast<अचिह्नित वर्ण*>(const_cast<वर्ण*>(तर्काः_2->target_सीमा));
    int32_t* offsets = तर्काः_2->offsets;
    uint32_t targetByteUnit = 0x0000;
    UChar32 sourceChar = 0x0000;
    UBool isTargetByteDBCS;
    UBool oldIsTargetByteDBCS;
    UConverter_दत्त_ISO2022 *converter_दत्त;
    UConverter_Shared_दत्त* shared_दत्त;
    UBool useFallback;
    int32_t length =0;

    converter_दत्त = static_cast<UConverter_दत्त_ISO2022*>(तर्काः_2->converter->extraInfo);
    /* if the version is 1 then the user is requesting
     * conversion with ibm-25546 pass the arguments to
     * MBCS converter and return
     */
    यदि(converter_दत्त->संस्करणम्==1){
        UConverter_fromUnicode_ISO_2022_KR_OFFSETS_LOGIC_IBM(तर्काः_2,err);
        प्रत्यागम;
    }

    /* initialize data */
    shared_दत्त = converter_दत्त->currentConverter->shared_दत्त;
    useFallback = तर्काः_2->converter->useFallback;
    isTargetByteDBCS = static_cast<UBool>(तर्काः_2->converter->from_Unicode_स्थितिः);
    oldIsTargetByteDBCS = isTargetByteDBCS;

    isTargetByteDBCS = static_cast<UBool>(तर्काः_2->converter->from_Unicode_स्थितिः);
    यदि((sourceChar = तर्काः_2->converter->fromUChar32)!=0 && target <target_सीमा) {
        गच्छ getTrail;
    }
    यावत्(source < source_सीमा){

        targetByteUnit = missingCharMarker;

        यदि(target < (अचिह्नित वर्ण*) तर्काः_2->target_सीमा){
            sourceChar = *source++;

            /* do not convert SO/SI/ESC */
            यदि(IS_2022_CONTROL(sourceChar)) {
                /* callback(illegal) */
                *err=U_ILLEGAL_CHAR_प्राप्तम्;
                तर्काः_2->converter->fromUChar32=sourceChar;
                भङ्ग;
            }

            length = MBCS_FROM_UCHAR32_ISO2022(shared_दत्त,sourceChar,&targetByteUnit,useFallback,MBCS_OUTPUT_2);
            यदि(length < 0) {
                length = -length;  /* fallback */
            }
            /* only DBCS or SBCS characters are expected*/
            /* DB characters with high bit set to 1 are expected */
            यदि( length > 2 || length==0 ||
                (length == 1 && targetByteUnit > 0x7f) ||
                (length == 2 &&
                    (static_cast<uint16_t>(targetByteUnit - 0xa1a1) > (0xfefe - 0xa1a1) ||
                    static_cast<uint8_t>(targetByteUnit - 0xa1) > (0xfe - 0xa1)))
            ) {
                targetByteUnit=missingCharMarker;
            }
            यदि (targetByteUnit != missingCharMarker){

                oldIsTargetByteDBCS = isTargetByteDBCS;
                isTargetByteDBCS = static_cast<UBool>(targetByteUnit > 0x00FF);
                  /* append the shift sequence */
                यदि (oldIsTargetByteDBCS != isTargetByteDBCS ){

                    यदि (isTargetByteDBCS)
                        *target++ = UCNV_SO;
                    अन्यथा
                        *target++ = UCNV_SI;
                    यदि(offsets)
                        *(offsets++) = static_cast<int32_t>(source - तर्काः_2->source - 1);
                }
                /* write the targetUniChar  to target */
                यदि(targetByteUnit <= 0x00FF){
                    यदि( target < target_सीमा){
                        *(target++) = static_cast<अचिह्नित वर्ण>(targetByteUnit);
                        यदि(offsets){
                            *(offsets++) = static_cast<int32_t>(source - तर्काः_2->source - 1);
                        }

                    }अन्यथा{
                        तर्काः_2->converter->char_Error_बफर[तर्काः_2->converter->char_Error_बफर_दीर्घता++] = static_cast<अचिह्नित वर्ण>(targetByteUnit);
                        *err = U_बफर_अतिप्रवाहः_ERROR;
                    }
                }अन्यथा{
                    यदि(target < target_सीमा){
                        *(target++) = static_cast<अचिह्नित वर्ण>((targetByteUnit >> 8) - 0x80);
                        यदि(offsets){
                            *(offsets++) = static_cast<int32_t>(source - तर्काः_2->source - 1);
                        }
                        यदि(target < target_सीमा){
                            *(target++) = static_cast<अचिह्नित वर्ण>(targetByteUnit - 0x80);
                            यदि(offsets){
                                *(offsets++) = static_cast<int32_t>(source - तर्काः_2->source - 1);
                            }
                        }अन्यथा{
                            तर्काः_2->converter->char_Error_बफर[तर्काः_2->converter->char_Error_बफर_दीर्घता++] = static_cast<अचिह्नित वर्ण>(targetByteUnit - 0x80);
                            *err = U_बफर_अतिप्रवाहः_ERROR;
                        }
                    }अन्यथा{
                        तर्काः_2->converter->char_Error_बफर[तर्काः_2->converter->char_Error_बफर_दीर्घता++] = static_cast<अचिह्नित वर्ण>((targetByteUnit >> 8) - 0x80);
                        तर्काः_2->converter->char_Error_बफर[तर्काः_2->converter->char_Error_बफर_दीर्घता++] = static_cast<अचिह्नित वर्ण>(targetByteUnit - 0x80);
                        *err = U_बफर_अतिप्रवाहः_ERROR;
                    }
                }

            }
            अन्यथा{
                /* oops.. the code point is unassingned
                 * set the error and reason
                 */

                /*check if the char is a First surrogate*/
                यदि(U16_IS_SURROGATE(sourceChar)) {
                    यदि(U16_IS_SURROGATE_LEAD(sourceChar)) {
getTrail:
                        /*look ahead to find the trail surrogate*/
                        यदि(source <  source_सीमा) {
                            /* test the following code unit */
                            char16_t trail = *source;
                            यदि(U16_IS_TRAIL(trail)) {
                                source++;
                                sourceChar=U16_GET_SUPPLEMENTARY(sourceChar, trail);
                                *err = U_INVALID_CHAR_प्राप्तम्;
                                /* convert this surrogate code point */
                                /* exit this condition tree */
                            } अन्यथा {
                                /* this is an unmatched lead code unit (1st surrogate) */
                                /* callback(illegal) */
                                *err=U_ILLEGAL_CHAR_प्राप्तम्;
                            }
                        } अन्यथा {
                            /* no more input */
                            *err = U_ZERO_ERROR;
                        }
                    } अन्यथा {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *err=U_ILLEGAL_CHAR_प्राप्तम्;
                    }
                } अन्यथा {
                    /* callback(unassigned) for a BMP code point */
                    *err = U_INVALID_CHAR_प्राप्तम्;
                }

                तर्काः_2->converter->fromUChar32=sourceChar;
                भङ्ग;
            }
        } /* end if(myTargetIndex<myTargetLength) */
        अन्यथा{
            *err =U_बफर_अतिप्रवाहः_ERROR;
            भङ्ग;
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
    यदि( U_SUCCESS(*err) &&
        isTargetByteDBCS &&
        तर्काः_2->flush && source>=source_सीमा && तर्काः_2->converter->fromUChar32==0
    ) {
        int32_t source_सूचकांक;

        /* we are switching to ASCII */
        isTargetByteDBCS=असत्य;

        /* get the source index of the last input character */
        /*
         * TODO this would be simpler and more reliable if we used a pair
         * of sourceIndex/prevSourceIndex like in ucnvmbcs.c
         * so that we could simply use the prevSourceIndex here;
         * this code gives an incorrect result for the rare case of an unmatched
         * trail surrogate that is alone in the last buffer of the text stream
         */
        source_सूचकांक = static_cast<int32_t>(source - तर्काः_2->source);
        यदि(source_सूचकांक>0) {
            --source_सूचकांक;
            यदि( U16_IS_TRAIL(तर्काः_2->source[source_सूचकांक]) &&
                (source_सूचकांक==0 || U16_IS_LEAD(तर्काः_2->source[source_सूचकांक-1]))
            ) {
                --source_सूचकांक;
            }
        } अन्यथा {
            source_सूचकांक=-1;
        }

        fromUWriteUInt8(
            तर्काः_2->converter,
            सरणम्_अन्तर्_STR, 1,
            &target, reinterpret_cast<ध्रुव वर्ण*>(target_सीमा),
            &offsets, source_सूचकांक,
            err);
    }

    /*save the state and return */
    तर्काः_2->source = source;
    तर्काः_2->target = reinterpret_cast<वर्ण*>(target);
    तर्काः_2->converter->from_Unicode_स्थितिः = static_cast<uint32_t>(isTargetByteDBCS);
}

/************************ To Unicode ***************************************/

स्थिर शून्य U_CALLCONV
UConverter_गन्तव्यम्_Unicode_ISO_2022_KR_OFFSETS_LOGIC_IBM(UConverter_गन्तव्यम्_Unicode_तर्काः *तर्काः_2,
                                                            UError_संहिता* err){
    वर्ण ध्रुव* sourceStart;
    UConverter_दत्त_ISO2022* my_दत्त = static_cast<UConverter_दत्त_ISO2022*>(तर्काः_2->converter->extraInfo);

    UConverter_गन्तव्यम्_Unicode_तर्काः sub_तर्काः;
    int32_t min_तर्काः_आकार;

    /* set up the subconverter arguments */
    यदि(तर्काः_2->size<परिमाण(UConverter_गन्तव्यम्_Unicode_तर्काः)) {
        min_तर्काः_आकार = तर्काः_2->size;
    } अन्यथा {
        min_तर्काः_आकार = static_cast<int32_t>(परिमाण(UConverter_गन्तव्यम्_Unicode_तर्काः));
    }

    uprv_memcpy(&sub_तर्काः, तर्काः_2, min_तर्काः_आकार);
    sub_तर्काः.size = static_cast<uint16_t>(min_तर्काः_आकार);
    sub_तर्काः.converter = my_दत्त->currentConverter;

    /* remember the original start of the input for offsets */
    sourceStart = तर्काः_2->source;

    यदि(my_दत्त->कुञ्चिका != 0) {
        /* continue with a partial escape sequence */
        गच्छ escape;
    }

    यावत्(U_SUCCESS(*err) && तर्काः_2->source < तर्काः_2->source_सीमा) {
        /*Find the end of the buffer e.g : Next Escape Seq | end of Buffer*/
        sub_तर्काः.source = तर्काः_2->source;
        sub_तर्काः.source_सीमा = get_अन्तः_सम्बन्धि_बफर_2022(&(तर्काः_2->source), तर्काः_2->source_सीमा, तर्काः_2->flush);
        यदि(sub_तर्काः.source != sub_तर्काः.source_सीमा) {
            /*
             * get the current partial byte sequence
             *
             * it needs to be moved between the public and the subconverter
             * so that the conversion framework, which only sees the public
             * converter, can handle truncated and illegal input etc.
             */
            यदि(तर्काः_2->converter->गन्तव्यम्_ULength > 0) {
                uprv_memcpy(sub_तर्काः.converter->गन्तव्यम्_UBytes, तर्काः_2->converter->गन्तव्यम्_UBytes, तर्काः_2->converter->गन्तव्यम्_ULength);
            }
            sub_तर्काः.converter->गन्तव्यम्_ULength = तर्काः_2->converter->गन्तव्यम्_ULength;

            /*
             * Convert up to the end of the input, or to before the next escape character.
             * Does not handle conversion extensions because the preToU[] state etc.
             * is not copied.
             */
            ucnv_MBCSToUnicodeWithOffsets(&sub_तर्काः, err);

            यदि(तर्काः_2->offsets != शून्यसूचकः && sourceStart != तर्काः_2->source) {
                /* update offsets to base them on the actual start of the input */
                int32_t *offsets = तर्काः_2->offsets;
                char16_t *target = तर्काः_2->target;
                int32_t delta = static_cast<int32_t>(तर्काः_2->source - sourceStart);
                यावत्(target < sub_तर्काः.target) {
                    यदि(*offsets >= 0) {
                        *offsets += delta;
                    }
                    ++offsets;
                    ++target;
                }
            }
            तर्काः_2->source = sub_तर्काः.source;
            तर्काः_2->target = sub_तर्काः.target;
            तर्काः_2->offsets = sub_तर्काः.offsets;

            /* copy input/error/overflow buffers */
            यदि(sub_तर्काः.converter->गन्तव्यम्_ULength > 0) {
                uprv_memcpy(तर्काः_2->converter->गन्तव्यम्_UBytes, sub_तर्काः.converter->गन्तव्यम्_UBytes, sub_तर्काः.converter->गन्तव्यम्_ULength);
            }
            तर्काः_2->converter->गन्तव्यम्_ULength = sub_तर्काः.converter->गन्तव्यम्_ULength;

            यदि(*err == U_बफर_अतिप्रवाहः_ERROR) {
                यदि(sub_तर्काः.converter->UChar_Error_बफर_दीर्घता > 0) {
                    uprv_memcpy(तर्काः_2->converter->UChar_Error_बफर, sub_तर्काः.converter->UChar_Error_बफर,
                                sub_तर्काः.converter->UChar_Error_बफर_दीर्घता);
                }
                तर्काः_2->converter->UChar_Error_बफर_दीर्घता=sub_तर्काः.converter->UChar_Error_बफर_दीर्घता;
                sub_तर्काः.converter->UChar_Error_बफर_दीर्घता = 0;
            }
        }

        यदि (U_FAILURE(*err) || (तर्काः_2->source == तर्काः_2->source_सीमा)) {
            प्रत्यागम;
        }

escape:
        changeState_2022(तर्काः_2->converter,
               &(तर्काः_2->source),
               तर्काः_2->source_सीमा,
               ISO_2022_KR,
               err);
    }
}

स्थिर शून्य U_CALLCONV
UConverter_गन्तव्यम्_Unicode_ISO_2022_KR_OFFSETS_LOGIC(UConverter_गन्तव्यम्_Unicode_तर्काः *तर्काः_2,
                                                            UError_संहिता* err){
    वर्ण अस्थायी_Buf[2];
    ध्रुव वर्ण* mySource = const_cast<वर्ण*>(तर्काः_2->source);
    char16_t *myTarget = तर्काः_2->target;
    ध्रुव वर्ण *my_Source_सीमा = तर्काः_2->source_सीमा;
    UChar32 targetUniChar = 0x0000;
    char16_t mySourceChar = 0x0000;
    UConverter_दत्त_ISO2022* my_दत्त;
    UConverter_Shared_दत्त* shared_दत्त ;
    UBool useFallback;

    my_दत्त = static_cast<UConverter_दत्त_ISO2022*>(तर्काः_2->converter->extraInfo);
    यदि(my_दत्त->संस्करणम्==1){
        UConverter_गन्तव्यम्_Unicode_ISO_2022_KR_OFFSETS_LOGIC_IBM(तर्काः_2,err);
        प्रत्यागम;
    }

    /* initialize state */
    shared_दत्त = my_दत्त->currentConverter->shared_दत्त;
    useFallback = तर्काः_2->converter->useFallback;

    यदि(my_दत्त->कुञ्चिका != 0) {
        /* continue with a partial escape sequence */
        गच्छ escape;
    } अन्यथा यदि(तर्काः_2->converter->गन्तव्यम्_ULength == 1 && mySource < my_Source_सीमा && myTarget < तर्काः_2->target_सीमा) {
        /* continue with a partial double-byte character */
        mySourceChar = तर्काः_2->converter->गन्तव्यम्_UBytes[0];
        तर्काः_2->converter->गन्तव्यम्_ULength = 0;
        गच्छ getTrailByte;
    }

    यावत्(mySource< my_Source_सीमा){

        यदि(myTarget < तर्काः_2->target_सीमा){

            mySourceChar = static_cast<अचिह्नित वर्ण>(*mySource++);

            यदि(mySourceChar==UCNV_SI){
                my_दत्त->गन्तव्यम्_U2022_State.g = 0;
                यदि (my_दत्त->is_रिक्तानि_Segment) {
                    my_दत्त->is_रिक्तानि_Segment = असत्य;	/* we are handling it, reset to avoid future spurious errors */
                    *err = U_ILLEGAL_ESCAPE_SEQUENCE;
                    तर्काः_2->converter->गन्तव्यम्_UCallback_Reason = UCNV_IRREGULAR;
                    तर्काः_2->converter->गन्तव्यम्_UBytes[0] = static_cast<uint8_t>(mySourceChar);
                    तर्काः_2->converter->गन्तव्यम्_ULength = 1;
                    तर्काः_2->target = myTarget;
                    तर्काः_2->source = mySource;
                    प्रत्यागम;
                }
                /*consume the source */
                अनुवर्त;
            }अन्यथा यदि(mySourceChar==UCNV_SO){
                my_दत्त->गन्तव्यम्_U2022_State.g = 1;
                my_दत्त->is_रिक्तानि_Segment = सत्य;	/* Begin a new segment, empty so far */
                /*consume the source */
                अनुवर्त;
            }अन्यथा यदि(mySourceChar==ESC_2022){
                mySource--;
escape:
                my_दत्त->is_रिक्तानि_Segment = असत्य;	/* Any invalid ESC sequences will be detected separately, so just reset this */
                changeState_2022(तर्काः_2->converter,&(mySource),
                                my_Source_सीमा, ISO_2022_KR, err);
                यदि(U_FAILURE(*err)){
                    तर्काः_2->target = myTarget;
                    तर्काः_2->source = mySource;
                    प्रत्यागम;
                }
                अनुवर्त;
            }

            my_दत्त->is_रिक्तानि_Segment = असत्य;	/* Any invalid char errors will be detected separately, so just reset this */
            यदि(my_दत्त->गन्तव्यम्_U2022_State.g == 1) {
                यदि(mySource < my_Source_सीमा) {
                    पूर्णाङ्क leadIsOk, trailIsOk;
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
                    यदि (leadIsOk && trailIsOk) {
                        ++mySource;
                        अस्थायी_Buf[0] = static_cast<वर्ण>(mySourceChar + 0x80);
                        अस्थायी_Buf[1] = static_cast<वर्ण>(trailByte + 0x80);
                        targetUniChar = ucnv_MBCSSimple_Get_अग्र_UChar(shared_दत्त, अस्थायी_Buf, 2, useFallback);
                        mySourceChar = (mySourceChar << 8) | trailByte;
                    } अन्यथा यदि (!(trailIsOk || IS_2022_CONTROL(trailByte))) {
                        /* report a pair of illegal bytes if the second byte is not a DBCS starter */
                        ++mySource;
                        /* add another bit so that the code below writes 2 bytes in case of error */
                        mySourceChar = static_cast<char16_t>(0x10000 | (mySourceChar << 8) | trailByte);
                    }
                } अन्यथा {
                    तर्काः_2->converter->गन्तव्यम्_UBytes[0] = static_cast<uint8_t>(mySourceChar);
                    तर्काः_2->converter->गन्तव्यम्_ULength = 1;
                    भङ्ग;
                }
            }
            अन्यथा यदि(mySourceChar <= 0x7f) {
                targetUniChar = ucnv_MBCSSimple_Get_अग्र_UChar(shared_दत्त, mySource - 1, 1, useFallback);
            } अन्यथा {
                targetUniChar = 0xffff;
            }
            यदि(targetUniChar < 0xfffe){
                यदि(तर्काः_2->offsets) {
                    तर्काः_2->offsets[myTarget - तर्काः_2->target] = static_cast<int32_t>(mySource - तर्काः_2->source - (mySourceChar <= 0xff ? 1 : 2));
                }
                *(myTarget++) = static_cast<char16_t>(targetUniChar);
            }
            अन्यथा {
                /* Call the callback function*/
                गन्तव्यम्_Unicode_Callback(तर्काः_2->converter,mySourceChar,targetUniChar,err);
                भङ्ग;
            }
        }
        अन्यथा{
            *err =U_बफर_अतिप्रवाहः_ERROR;
            भङ्ग;
        }
    }
    तर्काः_2->target = myTarget;
    तर्काः_2->source = mySource;
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
स्थिर ध्रुव वर्ण GB_2312_80_STR[] = "\x1B\x24\x29\x41";
स्थिर ध्रुव वर्ण ISO_IR_165_STR[] = "\x1B\x24\x29\x45";
स्थिर ध्रुव वर्ण CNS_11643_1992_Plane_1_STR[] = "\x1B\x24\x29\x47";
स्थिर ध्रुव वर्ण CNS_11643_1992_Plane_2_STR[] = "\x1B\x24\x2A\x48";
स्थिर ध्रुव वर्ण CNS_11643_1992_Plane_3_STR[] = "\x1B\x24\x2B\x49";
स्थिर ध्रुव वर्ण CNS_11643_1992_Plane_4_STR[] = "\x1B\x24\x2B\x4A";
स्थिर ध्रुव वर्ण CNS_11643_1992_Plane_5_STR[] = "\x1B\x24\x2B\x4B";
स्थिर ध्रुव वर्ण CNS_11643_1992_Plane_6_STR[] = "\x1B\x24\x2B\x4C";
स्थिर ध्रुव वर्ण CNS_11643_1992_Plane_7_STR[] = "\x1B\x24\x2B\x4D";

/********************** ISO2022-CN Data **************************/
स्थिर ध्रुव वर्ण* ध्रुव escSeqCharsCN[10] ={
        सरणम्_अन्तर्_STR,                   /* 0 ASCII */
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

स्थिर शून्य U_CALLCONV
UConverter_fromUnicode_ISO_2022_CN_OFFSETS_LOGIC(UConverter_From_Unicode_तर्काः* तर्काः_2, UError_संहिता* err){
    UConverter *cnv = तर्काः_2->converter;
    UConverter_दत्त_ISO2022 *converter_दत्त;
    ISO2022State *pFromU2022State;
    uint8_t* target = reinterpret_cast<uint8_t*>(तर्काः_2->target);
    ध्रुव uint8_t* target_सीमा = reinterpret_cast<ध्रुव uint8_t*>(तर्काः_2->target_सीमा);
    ध्रुव char16_t* source = तर्काः_2->source;
    ध्रुव char16_t* source_सीमा = तर्काः_2->source_सीमा;
    int32_t* offsets = तर्काः_2->offsets;
    UChar32 sourceChar;
    वर्ण बफर[8];
    int32_t len;
    int8_t choices[3];
    int32_t choice_गणनम्;
    uint32_t target_मानम् = 0;
    UBool useFallback;

    /* set up the state */
    converter_दत्त = static_cast<UConverter_दत्त_ISO2022*>(cnv->extraInfo);
    pFromU2022State   = &converter_दत्त->fromU2022State;

    choice_गणनम् = 0;

    /* check if the last codepoint of previous buffer was a lead surrogate*/
    यदि((sourceChar = cnv->fromUChar32)!=0 && target< target_सीमा) {
        गच्छ getTrail;
    }

    यावत्( source < source_सीमा){
        यदि(target < target_सीमा){

            sourceChar  = *(source++);
            /*check if the char is a First surrogate*/
             यदि(U16_IS_SURROGATE(sourceChar)) {
                यदि(U16_IS_SURROGATE_LEAD(sourceChar)) {
getTrail:
                    /*look ahead to find the trail surrogate*/
                    यदि(source < source_सीमा) {
                        /* test the following code unit */
                        char16_t trail = *source;
                        यदि(U16_IS_TRAIL(trail)) {
                            source++;
                            sourceChar=U16_GET_SUPPLEMENTARY(sourceChar, trail);
                            cnv->fromUChar32=0x00;
                            /* convert this supplementary code point */
                            /* exit this condition tree */
                        } अन्यथा {
                            /* this is an unmatched lead code unit (1st surrogate) */
                            /* callback(illegal) */
                            *err=U_ILLEGAL_CHAR_प्राप्तम्;
                            cnv->fromUChar32=sourceChar;
                            भङ्ग;
                        }
                    } अन्यथा {
                        /* no more input */
                        cnv->fromUChar32=sourceChar;
                        भङ्ग;
                    }
                } अन्यथा {
                    /* this is an unmatched trail code unit (2nd surrogate) */
                    /* callback(illegal) */
                    *err=U_ILLEGAL_CHAR_प्राप्तम्;
                    cnv->fromUChar32=sourceChar;
                    भङ्ग;
                }
            }

            /* do the conversion */
            यदि(sourceChar <= 0x007f ){
                /* do not convert SO/SI/ESC */
                यदि(IS_2022_CONTROL(sourceChar)) {
                    /* callback(illegal) */
                    *err=U_ILLEGAL_CHAR_प्राप्तम्;
                    cnv->fromUChar32=sourceChar;
                    भङ्ग;
                }

                /* US-ASCII */
                यदि(pFromU2022State->g == 0) {
                    बफर[0] = static_cast<वर्ण>(sourceChar);
                    len = 1;
                } अन्यथा {
                    बफर[0] = UCNV_SI;
                    बफर[1] = static_cast<वर्ण>(sourceChar);
                    len = 2;
                    pFromU2022State->g = 0;
                    choice_गणनम् = 0;
                }
                यदि(sourceChar == CR || sourceChar == LF) {
                    /* reset the state at the end of a line */
                    uprv_memset(pFromU2022State, 0, परिमाण(ISO2022State));
                    choice_गणनम् = 0;
                }
            }
            अन्यथा{
                /* convert U+0080..U+10ffff */
                int32_t i;
                int8_t cs, g;

                यदि(choice_गणनम् == 0) {
                    /* try the current SO/G1 converter first */
                    choices[0] = pFromU2022State->cs[1];

                    /* default to GB2312_1 if none is designated yet */
                    यदि(choices[0] == 0) {
                        choices[0] = GB2312_1;
                    }

                    यदि(converter_दत्त->संस्करणम् == 0) {
                        /* ISO-2022-CN */

                        /* try the other SO/G1 converter; a CNS_11643_1 lookup may result in any plane */
                        यदि(choices[0] == GB2312_1) {
                            choices[1] = static_cast<int8_t>(CNS_11643_1);
                        } अन्यथा {
                            choices[1] = static_cast<int8_t>(GB2312_1);
                        }

                        choice_गणनम् = 2;
                    } अन्यथा यदि (converter_दत्त->संस्करणम् == 1) {
                        /* ISO-2022-CN-EXT */

                        /* try one of the other converters */
                        विकल्प(choices[0]) {
                        पक्ष GB2312_1:
                            choices[1] = static_cast<int8_t>(CNS_11643_1);
                            choices[2] = static_cast<int8_t>(ISO_IR_165);
                            भङ्ग;
                        पक्ष ISO_IR_165:
                            choices[1] = static_cast<int8_t>(GB2312_1);
                            choices[2] = static_cast<int8_t>(CNS_11643_1);
                            भङ्ग;
                        स्वभाव: /* CNS_11643_x */
                            choices[1] = static_cast<int8_t>(GB2312_1);
                            choices[2] = static_cast<int8_t>(ISO_IR_165);
                            भङ्ग;
                        }

                        choice_गणनम् = 3;
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

                प्रति(i = 0; i < choice_गणनम् && len <= 0; ++i) {
                    int8_t cs0 = choices[i];
                    यदि(cs0 > 0) {
                        uint32_t value;
                        int32_t len2;
                        यदि(cs0 >= CNS_11643_0) {
                            len2 = MBCS_FROM_UCHAR32_ISO2022(
                                        converter_दत्त->myConverterArray[CNS_11643],
                                        sourceChar,
                                        &value,
                                        useFallback,
                                        MBCS_OUTPUT_3);
                            यदि(len2 == 3 || (len2 == -3 && len == 0)) {
                                target_मानम् = value;
                                cs = static_cast<int8_t>(CNS_11643_0 + (value >> 16) - 0x80);
                                यदि(len2 >= 0) {
                                    len = 2;
                                } अन्यथा {
                                    len = -2;
                                    useFallback = असत्य;
                                }
                                यदि(cs == CNS_11643_1) {
                                    g = 1;
                                } अन्यथा यदि(cs == CNS_11643_2) {
                                    g = 2;
                                } अन्यथा /* plane 3..7 */ यदि(converter_दत्त->संस्करणम् == 1) {
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
                                        converter_दत्त->myConverterArray[cs0],
                                        sourceChar,
                                        &value,
                                        useFallback,
                                        MBCS_OUTPUT_2);
                            यदि(len2 == 2 || (len2 == -2 && len == 0)) {
                                target_मानम् = value;
                                len = len2;
                                cs = cs0;
                                g = 1;
                                useFallback = असत्य;
                            }
                        }
                    }
                }

                यदि(len != 0) {
                    len = 0; /* count output bytes; it must have been abs(len) == 2 */

                    /* write the designation sequence if necessary */
                    यदि(cs != pFromU2022State->cs[g]) {
                        यदि(cs < CNS_11643) {
                            uprv_memcpy(बफर, escSeqCharsCN[cs], 4);
                        } अन्यथा {
                            U_ASSERT(cs >= CNS_11643_1);
                            uprv_memcpy(बफर, escSeqCharsCN[CNS_11643 + (cs - CNS_11643_1)], 4);
                        }
                        len = 4;
                        pFromU2022State->cs[g] = cs;
                        यदि(g == 1) {
                            /* changing the SO/G1 charset invalidates the choices[] */
                            choice_गणनम् = 0;
                        }
                    }

                    /* write the shift sequence if necessary */
                    यदि(g != pFromU2022State->g) {
                        विकल्प(g) {
                        पक्ष 1:
                            बफर[len++] = UCNV_SO;

                            /* set the new state only if it is the locking shift SO/G1, not for SS2 or SS3 */
                            pFromU2022State->g = 1;
                            भङ्ग;
                        पक्ष 2:
                            बफर[len++] = 0x1b;
                            बफर[len++] = 0x4e;
                            भङ्ग;
                        स्वभाव: /* case 3 */
                            बफर[len++] = 0x1b;
                            बफर[len++] = 0x4f;
                            भङ्ग;
                        }
                    }

                    /* write the two output bytes */
                    बफर[len++] = static_cast<वर्ण>(target_मानम् >> 8);
                    बफर[len++] = static_cast<वर्ण>(target_मानम्);
                } अन्यथा {
                    /* if we cannot find the character after checking all codepages
                     * then this is an error
                     */
                    *err = U_INVALID_CHAR_प्राप्तम्;
                    cnv->fromUChar32=sourceChar;
                    भङ्ग;
                }
            }

            /* output len>0 bytes in buffer[] */
            यदि(len == 1) {
                *target++ = बफर[0];
                यदि(offsets) {
                    *offsets++ = static_cast<int32_t>(source - तर्काः_2->source - 1); /* -1: known to be ASCII */
                }
            } अन्यथा यदि(len == 2 && (target + 2) <= target_सीमा) {
                *target++ = बफर[0];
                *target++ = बफर[1];
                यदि(offsets) {
                    int32_t source_सूचकांक = static_cast<int32_t>(source - तर्काः_2->source - U16_दीर्घता(sourceChar));
                    *offsets++ = source_सूचकांक;
                    *offsets++ = source_सूचकांक;
                }
            } अन्यथा {
                fromUWriteUInt8(
                    cnv,
                    बफर, len,
                    &target, reinterpret_cast<ध्रुव वर्ण*>(target_सीमा),
                    &offsets, static_cast<int32_t>(source - तर्काः_2->source - U16_दीर्घता(sourceChar)),
                    err);
                यदि(U_FAILURE(*err)) {
                    भङ्ग;
                }
            }
        } /* end if(myTargetIndex<myTargetLength) */
        अन्यथा{
            *err =U_बफर_अतिप्रवाहः_ERROR;
            भङ्ग;
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
    यदि( U_SUCCESS(*err) &&
        pFromU2022State->g!=0 &&
        तर्काः_2->flush && source>=source_सीमा && cnv->fromUChar32==0
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
        source_सूचकांक = static_cast<int32_t>(source - तर्काः_2->source);
        यदि(source_सूचकांक>0) {
            --source_सूचकांक;
            यदि( U16_IS_TRAIL(तर्काः_2->source[source_सूचकांक]) &&
                (source_सूचकांक==0 || U16_IS_LEAD(तर्काः_2->source[source_सूचकांक-1]))
            ) {
                --source_सूचकांक;
            }
        } अन्यथा {
            source_सूचकांक=-1;
        }

        fromUWriteUInt8(
            cnv,
            सरणम्_अन्तर्_STR, 1,
            &target, reinterpret_cast<ध्रुव वर्ण*>(target_सीमा),
            &offsets, source_सूचकांक,
            err);
    }

    /*save the state and return */
    तर्काः_2->source = source;
    तर्काः_2->target = reinterpret_cast<वर्ण*>(target);
}


स्थिर शून्य U_CALLCONV
UConverter_गन्तव्यम्_Unicode_ISO_2022_CN_OFFSETS_LOGIC(UConverter_गन्तव्यम्_Unicode_तर्काः *तर्काः_2,
                                               UError_संहिता* err){
    वर्ण अस्थायी_Buf[3];
    ध्रुव वर्ण* mySource = const_cast<वर्ण*>(तर्काः_2->source);
    char16_t *myTarget = तर्काः_2->target;
    ध्रुव वर्ण *my_Source_सीमा = तर्काः_2->source_सीमा;
    uint32_t targetUniChar = 0x0000;
    uint32_t mySourceChar = 0x0000;
    UConverter_दत्त_ISO2022* my_दत्त;
    ISO2022State *p_गन्तव्यम्_U2022_State;

    my_दत्त = static_cast<UConverter_दत्त_ISO2022*>(तर्काः_2->converter->extraInfo);
    p_गन्तव्यम्_U2022_State = &my_दत्त->गन्तव्यम्_U2022_State;

    यदि(my_दत्त->कुञ्चिका != 0) {
        /* continue with a partial escape sequence */
        गच्छ escape;
    } अन्यथा यदि(तर्काः_2->converter->गन्तव्यम्_ULength == 1 && mySource < my_Source_सीमा && myTarget < तर्काः_2->target_सीमा) {
        /* continue with a partial double-byte character */
        mySourceChar = तर्काः_2->converter->गन्तव्यम्_UBytes[0];
        तर्काः_2->converter->गन्तव्यम्_ULength = 0;
        targetUniChar = missingCharMarker;
        गच्छ getTrailByte;
    }

    यावत्(mySource < my_Source_सीमा){

        targetUniChar =missingCharMarker;

        यदि(myTarget < तर्काः_2->target_सीमा){

            mySourceChar = static_cast<अचिह्नित वर्ण>(*mySource++);

            विकल्प(mySourceChar){
            पक्ष UCNV_SI:
                p_गन्तव्यम्_U2022_State->g=0;
                यदि (my_दत्त->is_रिक्तानि_Segment) {
                    my_दत्त->is_रिक्तानि_Segment = असत्य;	/* we are handling it, reset to avoid future spurious errors */
                    *err = U_ILLEGAL_ESCAPE_SEQUENCE;
                    तर्काः_2->converter->गन्तव्यम्_UCallback_Reason = UCNV_IRREGULAR;
                    तर्काः_2->converter->गन्तव्यम्_UBytes[0] = static_cast<uint8_t>(mySourceChar);
                    तर्काः_2->converter->गन्तव्यम्_ULength = 1;
                    तर्काः_2->target = myTarget;
                    तर्काः_2->source = mySource;
                    प्रत्यागम;
                }
                अनुवर्त;

            पक्ष UCNV_SO:
                यदि(p_गन्तव्यम्_U2022_State->cs[1] != 0) {
                    p_गन्तव्यम्_U2022_State->g=1;
                    my_दत्त->is_रिक्तानि_Segment = सत्य;	/* Begin a new segment, empty so far */
                    अनुवर्त;
                } अन्यथा {
                    /* illegal to have SO before a matching designator */
                    my_दत्त->is_रिक्तानि_Segment = असत्य;	/* Handling a different error, reset this to avoid future spurious errs */
                    भङ्ग;
                }

            पक्ष ESC_2022:
                mySource--;
escape:
                {
                    ध्रुव वर्ण * mySourceBefore = mySource;
                    int8_t गन्तव्यम्_ULength_Before = तर्काः_2->converter->गन्तव्यम्_ULength;

                    changeState_2022(तर्काः_2->converter,&(mySource),
                        my_Source_सीमा, ISO_2022_CN,err);

                    /* After SO there must be at least one character before a designator (designator error handled separately) */
                    यदि(my_दत्त->कुञ्चिका==0 && U_SUCCESS(*err) && my_दत्त->is_रिक्तानि_Segment) {
                        *err = U_ILLEGAL_ESCAPE_SEQUENCE;
                        तर्काः_2->converter->गन्तव्यम्_UCallback_Reason = UCNV_IRREGULAR;
                        तर्काः_2->converter->गन्तव्यम्_ULength = static_cast<int8_t>(गन्तव्यम्_ULength_Before + (mySource - mySourceBefore));
                    }
                }

                /* invalid or illegal escape sequence */
                यदि(U_FAILURE(*err)){
                    तर्काः_2->target = myTarget;
                    तर्काः_2->source = mySource;
                    my_दत्त->is_रिक्तानि_Segment = असत्य;	/* Reset to avoid future spurious errors */
                    प्रत्यागम;
                }
                अनुवर्त;

            /* ISO-2022-CN does not use single-byte (C1) SS2 and SS3 */

            पक्ष CR:
            पक्ष LF:
                uprv_memset(p_गन्तव्यम्_U2022_State, 0, परिमाण(ISO2022State));
                U_FALLTHROUGH;
            स्वभाव:
                /* convert one or two bytes */
                my_दत्त->is_रिक्तानि_Segment = असत्य;
                यदि(p_गन्तव्यम्_U2022_State->g != 0) {
                    यदि(mySource < my_Source_सीमा) {
                        UConverter_Shared_दत्त *cnv;
                        StateEnum अस्थायी_State;
                        int32_t अस्थायी_Buf_Len;
                        पूर्णाङ्क leadIsOk, trailIsOk;
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
                        यदि (leadIsOk && trailIsOk) {
                            ++mySource;
                            अस्थायी_State = static_cast<StateEnum>(p_गन्तव्यम्_U2022_State->cs[p_गन्तव्यम्_U2022_State->g]);
                            यदि(अस्थायी_State >= CNS_11643_0) {
                                cnv = my_दत्त->myConverterArray[CNS_11643];
                                अस्थायी_Buf[0] = static_cast<वर्ण>(0x80 + (अस्थायी_State - CNS_11643_0));
                                अस्थायी_Buf[1] = static_cast<वर्ण>(mySourceChar);
                                अस्थायी_Buf[2] = static_cast<वर्ण>(trailByte);
                                अस्थायी_Buf_Len = 3;

                            }अन्यथा{
                                U_ASSERT(अस्थायी_State<UCNV_2022_MAX_CONVERTERS);
                                cnv = my_दत्त->myConverterArray[अस्थायी_State];
                                अस्थायी_Buf[0] = static_cast<वर्ण>(mySourceChar);
                                अस्थायी_Buf[1] = static_cast<वर्ण>(trailByte);
                                अस्थायी_Buf_Len = 2;
                            }
                            targetUniChar = ucnv_MBCSSimple_Get_अग्र_UChar(cnv, अस्थायी_Buf, अस्थायी_Buf_Len, असत्य);
                            mySourceChar = (mySourceChar << 8) | trailByte;
                        } अन्यथा यदि (!(trailIsOk || IS_2022_CONTROL(trailByte))) {
                            /* report a pair of illegal bytes if the second byte is not a DBCS starter */
                            ++mySource;
                            /* add another bit so that the code below writes 2 bytes in case of error */
                            mySourceChar = 0x10000 | (mySourceChar << 8) | trailByte;
                        }
                        यदि(p_गन्तव्यम्_U2022_State->g>=2) {
                            /* return from a single-shift state to the previous one */
                            p_गन्तव्यम्_U2022_State->g=p_गन्तव्यम्_U2022_State->पूर्व_G;
                        }
                    } अन्यथा {
                        तर्काः_2->converter->गन्तव्यम्_UBytes[0] = static_cast<uint8_t>(mySourceChar);
                        तर्काः_2->converter->गन्तव्यम्_ULength = 1;
                        गच्छ endloop;
                    }
                }
                अन्यथा{
                    यदि(mySourceChar <= 0x7f) {
                        targetUniChar = static_cast<char16_t>(mySourceChar);
                    }
                }
                भङ्ग;
            }
            यदि(targetUniChar < (missingCharMarker-1/*0xfffe*/)){
                यदि(तर्काः_2->offsets){
                    तर्काः_2->offsets[myTarget - तर्काः_2->target] = static_cast<int32_t>(mySource - तर्काः_2->source - (mySourceChar <= 0xff ? 1 : 2));
                }
                *(myTarget++) = static_cast<char16_t>(targetUniChar);
            }
            अन्यथा यदि(targetUniChar > missingCharMarker){
                /* disassemble the surrogate pair and write to output*/
                targetUniChar-=0x0010000;
                *myTarget = static_cast<char16_t>(0xd800 + static_cast<char16_t>(targetUniChar >> 10));
                यदि(तर्काः_2->offsets){
                    तर्काः_2->offsets[myTarget - तर्काः_2->target] = static_cast<int32_t>(mySource - तर्काः_2->source - (mySourceChar <= 0xff ? 1 : 2));
                }
                ++myTarget;
                यदि(myTarget< तर्काः_2->target_सीमा){
                    *myTarget = static_cast<char16_t>(0xdc00 + static_cast<char16_t>(targetUniChar & 0x3ff));
                    यदि(तर्काः_2->offsets){
                        तर्काः_2->offsets[myTarget - तर्काः_2->target] = static_cast<int32_t>(mySource - तर्काः_2->source - (mySourceChar <= 0xff ? 1 : 2));
                    }
                    ++myTarget;
                }अन्यथा{
                    तर्काः_2->converter->UChar_Error_बफर[तर्काः_2->converter->UChar_Error_बफर_दीर्घता++]=
                                    static_cast<char16_t>(0xdc00 + static_cast<char16_t>(targetUniChar & 0x3ff));
                }

            }
            अन्यथा{
                /* Call the callback function*/
                गन्तव्यम्_Unicode_Callback(तर्काः_2->converter,mySourceChar,targetUniChar,err);
                भङ्ग;
            }
        }
        अन्यथा{
            *err =U_बफर_अतिप्रवाहः_ERROR;
            भङ्ग;
        }
    }
endloop:
    तर्काः_2->target = myTarget;
    तर्काः_2->source = mySource;
}
#यदि_अन्तः /* #if !UCONFIG_ONLY_HTML_CONVERSION */

स्थिर शून्य U_CALLCONV
_ISO_2022_WriteSub(UConverter_From_Unicode_तर्काः *तर्काः_2, int32_t विस्थापनम्_सूचकांक, UError_संहिता *err) {
    UConverter *cnv = तर्काः_2->converter;
    UConverter_दत्त_ISO2022* my_Converter_दत्त = static_cast<UConverter_दत्त_ISO2022*>(cnv->extraInfo);
    ISO2022State *pFromU2022State=&my_Converter_दत्त->fromU2022State;
    वर्ण *p, *subchar;
    वर्ण बफर[8];
    int32_t length;

    subchar = reinterpret_cast<वर्ण*>(cnv->subChars);
    length=cnv->subCharLen; /* assume length==1 for most variants */

    p = बफर;
    विकल्प(my_Converter_दत्त->locale[0]){
    पक्ष 'j':
        {
            int8_t cs;

            यदि(pFromU2022State->g == 1) {
                /* JIS7: switch from G1 to G0 */
                pFromU2022State->g = 0;
                *p++ = UCNV_SI;
            }

            cs = pFromU2022State->cs[0];
            यदि(cs != ASCII && cs != JISX201) {
                /* not in ASCII or JIS X 0201: switch to ASCII */
                pFromU2022State->cs[0] = static_cast<int8_t>(ASCII);
                *p++ = '\x1b';
                *p++ = '\x28';
                *p++ = '\x42';
            }

            *p++ = subchar[0];
            भङ्ग;
        }
    पक्ष 'c':
        यदि(pFromU2022State->g != 0) {
            /* not in ASCII mode: switch to ASCII */
            pFromU2022State->g = 0;
            *p++ = UCNV_SI;
        }
        *p++ = subchar[0];
        भङ्ग;
    पक्ष 'k':
        यदि(my_Converter_दत्त->संस्करणम् == 0) {
            यदि(length == 1) {
                यदि(तर्काः_2->converter->from_Unicode_स्थितिः) {
                    /* in DBCS mode: switch to SBCS */
                    तर्काः_2->converter->from_Unicode_स्थितिः = 0;
                    *p++ = UCNV_SI;
                }
                *p++ = subchar[0];
            } अन्यथा /* length == 2*/ {
                यदि(!तर्काः_2->converter->from_Unicode_स्थितिः) {
                    /* in SBCS mode: switch to DBCS */
                    तर्काः_2->converter->from_Unicode_स्थितिः = 1;
                    *p++ = UCNV_SO;
                }
                *p++ = subchar[0];
                *p++ = subchar[1];
            }
            भङ्ग;
        } अन्यथा {
            /* save the subconverter's substitution string */
            uint8_t *currentSubChars = my_Converter_दत्त->currentConverter->subChars;
            int8_t currentSubCharLen = my_Converter_दत्त->currentConverter->subCharLen;

            /* set our substitution string into the subconverter */
            my_Converter_दत्त->currentConverter->subChars = reinterpret_cast<uint8_t*>(subchar);
            my_Converter_दत्त->currentConverter->subCharLen = static_cast<int8_t>(length);

            /* let the subconverter write the subchar, set/retrieve fromUChar32 state */
            तर्काः_2->converter = my_Converter_दत्त->currentConverter;
            my_Converter_दत्त->currentConverter->fromUChar32 = cnv->fromUChar32;
            ucnv_cbFromUWriteSub(तर्काः_2, 0, err);
            cnv->fromUChar32 = my_Converter_दत्त->currentConverter->fromUChar32;
            तर्काः_2->converter = cnv;

            /* restore the subconverter's substitution string */
            my_Converter_दत्त->currentConverter->subChars = currentSubChars;
            my_Converter_दत्त->currentConverter->subCharLen = currentSubCharLen;

            यदि(*err == U_बफर_अतिप्रवाहः_ERROR) {
                यदि(my_Converter_दत्त->currentConverter->char_Error_बफर_दीर्घता > 0) {
                    uprv_memcpy(
                        cnv->char_Error_बफर,
                        my_Converter_दत्त->currentConverter->char_Error_बफर,
                        my_Converter_दत्त->currentConverter->char_Error_बफर_दीर्घता);
                }
                cnv->char_Error_बफर_दीर्घता = my_Converter_दत्त->currentConverter->char_Error_बफर_दीर्घता;
                my_Converter_दत्त->currentConverter->char_Error_बफर_दीर्घता = 0;
            }
            प्रत्यागम;
        }
    स्वभाव:
        /* not expected */
        भङ्ग;
    }
    ucnv_cb_From_UWrite_अष्टकानि(तर्काः_2,
                           बफर, static_cast<int32_t>(p - बफर),
                           विस्थापनम्_सूचकांक, err);
}

/*
 * Structure for cloning an ISO 2022 converter into a single memory block.
 */
संरचना cloneStruct
{
    UConverter cnv;
    UConverter currentConverter;
    UConverter_दत्त_ISO2022 mydata;
};


U_CDECL_BEGIN

स्थिर UConverter * U_CALLCONV
_ISO_2022_SafeClone(
            ध्रुव UConverter *cnv,
            शून्य *राशिः_बफर,
            int32_t *p_बफर_आकार,
            UError_संहिता *स्थितिः)
{
    संरचना cloneStruct * localClone;
    UConverter_दत्त_ISO2022 *cnv_दत्त;
    int32_t i, size;

    यदि (U_FAILURE(*स्थितिः)){
        प्रत्यागम शून्यसूचकः;
    }

    यदि (*p_बफर_आकार == 0) { /* 'preflighting' request - set needed size into *pBufferSize */
        *p_बफर_आकार = (int32_t)परिमाण(संरचना cloneStruct);
        प्रत्यागम शून्यसूचकः;
    }

    cnv_दत्त = (UConverter_दत्त_ISO2022 *)cnv->extraInfo;
    localClone = (संरचना cloneStruct *)राशिः_बफर;

    /* ucnv.c/ucnv_safeClone() copied the main UConverter already */

    uprv_memcpy(&localClone->mydata, cnv_दत्त, परिमाण(UConverter_दत्त_ISO2022));
    localClone->cnv.extraInfo = &localClone->mydata; /* set pointer to extra data */
    localClone->cnv.isExtraLocal = सत्य;

    /* share the subconverters */

    यदि(cnv_दत्त->currentConverter != शून्यसूचकः) {
        size = (int32_t)परिमाण(UConverter);
        localClone->mydata.currentConverter =
            ucnv_safeClone(cnv_दत्त->currentConverter,
                            &localClone->currentConverter,
                            &size, स्थितिः);
        यदि(U_FAILURE(*स्थितिः)) {
            प्रत्यागम शून्यसूचकः;
        }
    }

    प्रति(i=0; i<UCNV_2022_MAX_CONVERTERS; ++i) {
        यदि(cnv_दत्त->myConverterArray[i] != शून्यसूचकः) {
            ucnv_increment_Ref_गणनम्(cnv_दत्त->myConverterArray[i]);
        }
    }

    प्रत्यागम &localClone->cnv;
}

U_CDECL_अन्तः

स्थिर शून्य U_CALLCONV
ISO_2022_Get_Unicode_समुच्चयः(ध्रुव UConverter *cnv,
                    ध्रुव USetAdder *sa,
                    UConverter_Unicode_समुच्चयः which,
                    UError_संहिता *p_Error_संहिता)
{
    int32_t i;
    UConverter_दत्त_ISO2022* cnv_दत्त;

    यदि (U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम;
    }
#यदि_परिभाषितम् U_ENABLE_GENERIC_ISO_2022
    यदि (cnv->shared_दत्त == &ISO2022_दत्त) {
        /* We use UTF-8 in this case */
        sa->सङ्कलय_Range(sa->समुच्चयः, 0, 0xd7FF);
        sa->सङ्कलय_Range(sa->समुच्चयः, 0xE000, 0x10FFFF);
        प्रत्यागम;
    }
#यदि_अन्तः

    cnv_दत्त = static_cast<UConverter_दत्त_ISO2022*>(cnv->extraInfo);

    /* open a set and initialize it with code points that are algorithmically round-tripped */
    विकल्प(cnv_दत्त->locale[0]){
    पक्ष 'j':
        /* include JIS X 0201 which is hardcoded */
        sa->सङ्कलय(sa->समुच्चयः, 0xa5);
        sa->सङ्कलय(sa->समुच्चयः, 0x203e);
        यदि(jpCharsetMasks[cnv_दत्त->संस्करणम्]&CSM(ISO8859_1)) {
            /* include Latin-1 for some variants of JP */
            sa->सङ्कलय_Range(sa->समुच्चयः, 0, 0xff);
        } अन्यथा {
            /* include ASCII for JP */
            sa->सङ्कलय_Range(sa->समुच्चयः, 0, 0x7f);
        }
        यदि(cnv_दत्त->संस्करणम्==3 || cnv_दत्त->संस्करणम्==4 || which==UCNV_ROUNDTRIP_AND_FALLBACK_समुच्चयः) {
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
            sa->सङ्कलय_Range(sa->समुच्चयः, HWKANA_START, HWKANA_अन्तः);
        }
        भङ्ग;
#यदि !UCONFIG_ONLY_HTML_CONVERSION
    पक्ष 'c':
    पक्ष 'z':
        /* include ASCII for CN */
        sa->सङ्कलय_Range(sa->समुच्चयः, 0, 0x7f);
        भङ्ग;
    पक्ष 'k':
        /* there is only one converter for KR, and it is not in the myConverterArray[] */
        cnv_दत्त->currentConverter->shared_दत्त->impl->get_Unicode_समुच्चयः(
                cnv_दत्त->currentConverter, sa, which, p_Error_संहिता);
        /* the loop over myConverterArray[] will simply not find another converter */
        भङ्ग;
#यदि_अन्तः
    स्वभाव:
        भङ्ग;
    }

#यदि 0  /* Replaced by ucnv_MBCSGetFilteredUnicodeSetForUnicode() until we implement ucnv_getUnicodeSet() with reverse fallbacks. */
            यदि( (cnv_दत्त->locale[0]=='c' || cnv_दत्त->locale[0]=='z') &&
                cnv_दत्त->संस्करणम्==0 && i==CNS_11643
            ) {
                /* special handling for non-EXT ISO-2022-CN: add only code points for CNS planes 1 and 2 */
                ucnv_MBCSGet_Unicode_समुच्चयः_For_अष्टकानि(
                        cnv_दत्त->myConverterArray[i],
                        sa, UCNV_ROUNDTRIP_समुच्चयः,
                        0, 0x81, 0x82,
                        p_Error_संहिता);
            }
#यदि_अन्तः

    प्रति (i=0; i<UCNV_2022_MAX_CONVERTERS; i++) {
        UConverter_समुच्चयः_Filter filter;
        यदि(cnv_दत्त->myConverterArray[i]!=शून्यसूचकः) {
            यदि(cnv_दत्त->locale[0]=='j' && i==JISX208) {
                /*
                 * Only add code points that map to Shift-JIS codes
                 * corresponding to JIS X 0208.
                 */
                filter=UCNV_समुच्चयः_FILTER_SJIS;
#यदि !UCONFIG_ONLY_HTML_CONVERSION
            } अन्यथा यदि( (cnv_दत्त->locale[0]=='c' || cnv_दत्त->locale[0]=='z') &&
                       cnv_दत्त->संस्करणम्==0 && i==CNS_11643) {
                /*
                 * Version-specific for CN:
                 * CN version 0 does not map CNS planes 3..7 although
                 * they are all available in the CNS conversion table;
                 * CN version 1 (-EXT) does map them all.
                 * The two versions create different Unicode sets.
                 */
                filter=UCNV_समुच्चयः_FILTER_2022_CN;
            } अन्यथा यदि(i==KSC5601) {
                /*
                 * Some of the KSC 5601 tables (convrtrs.txt has this aliases on multiple tables)
                 * are broader than GR94.
                 */
                filter=UCNV_समुच्चयः_FILTER_GR94_DBCS;
#यदि_अन्तः
            } अन्यथा {
                filter=UCNV_समुच्चयः_FILTER_NONE;
            }
            ucnv_MBCSGet_Filtered_Unicode_समुच्चयः_For_Unicode(cnv_दत्त->myConverterArray[i], sa, which, filter, p_Error_संहिता);
        }
    }

    /*
     * ISO 2022 converters must not convert SO/SI/ESC despite what
     * sub-converters do by themselves.
     * Remove these characters from the set.
     */
    sa->remove(sa->समुच्चयः, 0x0e);
    sa->remove(sa->समुच्चयः, 0x0f);
    sa->remove(sa->समुच्चयः, 0x1b);

    /* ISO 2022 converters do not convert C1 controls either */
    sa->removeRange(sa->समुच्चयः, 0x80, 0x9f);
}

स्थिर ध्रुव UConverterImpl _ISO2022Impl={
    UCNV_ISO_2022,

    शून्यसूचकः,
    शून्यसूचकः,

    _ISO2022Open,
    _ISO2022Close,
    _ISO2022Reset,

#यदि_परिभाषितम् U_ENABLE_GENERIC_ISO_2022
    T_UConverter_गन्तव्यम्_Unicode_ISO_2022_OFFSETS_LOGIC,
    T_UConverter_गन्तव्यम्_Unicode_ISO_2022_OFFSETS_LOGIC,
    ucnv_fromUnicode_UTF8,
    ucnv_fromUnicode_UTF8_OFFSETS_LOGIC,
#अन्यथा
    शून्यसूचकः,
    शून्यसूचकः,
    शून्यसूचकः,
    शून्यसूचकः,
#यदि_अन्तः
    शून्यसूचकः,

    शून्यसूचकः,
    _ISO2022getName,
    _ISO_2022_WriteSub,
    _ISO_2022_SafeClone,
    ISO_2022_Get_Unicode_समुच्चयः,

    शून्यसूचकः,
    शून्यसूचकः
};
स्थिर ध्रुव UConverter_Static_दत्त ISO2022_Static_दत्त={
    परिमाण(UConverter_Static_दत्त),
    "ISO_2022",
    2022,
    UCNV_IBM,
    UCNV_ISO_2022,
    1,
    3, /* max 3 bytes per char16_t from UTF-8 (4 bytes from surrogate _pair_) */
    { 0x1a, 0, 0, 0 },
    1,
    असत्य,
    असत्य,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};
ध्रुव UConverter_Shared_दत्त ISO2022_दत्त=
        UCNV_IMMUTABLE_SHARED_दत्त_INITIALIZER(&ISO2022_Static_दत्त, &_ISO2022Impl);

/*************JP****************/
स्थिर ध्रुव UConverterImpl _ISO2022JPImpl={
    UCNV_ISO_2022,

    शून्यसूचकः,
    शून्यसूचकः,

    _ISO2022Open,
    _ISO2022Close,
    _ISO2022Reset,

    UConverter_गन्तव्यम्_Unicode_ISO_2022_JP_OFFSETS_LOGIC,
    UConverter_गन्तव्यम्_Unicode_ISO_2022_JP_OFFSETS_LOGIC,
    UConverter_fromUnicode_ISO_2022_JP_OFFSETS_LOGIC,
    UConverter_fromUnicode_ISO_2022_JP_OFFSETS_LOGIC,
    शून्यसूचकः,

    शून्यसूचकः,
    _ISO2022getName,
    _ISO_2022_WriteSub,
    _ISO_2022_SafeClone,
    ISO_2022_Get_Unicode_समुच्चयः,

    शून्यसूचकः,
    शून्यसूचकः
};
स्थिर ध्रुव UConverter_Static_दत्त ISO2022_JPStatic_दत्त={
    परिमाण(UConverter_Static_दत्त),
    "ISO_2022_JP",
    0,
    UCNV_IBM,
    UCNV_ISO_2022,
    1,
    6, /* max 6 bytes per char16_t: 4-byte escape sequence + DBCS */
    { 0x1a, 0, 0, 0 },
    1,
    असत्य,
    असत्य,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

नामाकाश {

ध्रुव UConverter_Shared_दत्त _ISO2022JPData=
        UCNV_IMMUTABLE_SHARED_दत्त_INITIALIZER(&ISO2022_JPStatic_दत्त, &_ISO2022JPImpl);

}  // namespace

#यदि !UCONFIG_ONLY_HTML_CONVERSION
/************* KR ***************/
स्थिर ध्रुव UConverterImpl _ISO2022KRImpl={
    UCNV_ISO_2022,

    शून्यसूचकः,
    शून्यसूचकः,

    _ISO2022Open,
    _ISO2022Close,
    _ISO2022Reset,

    UConverter_गन्तव्यम्_Unicode_ISO_2022_KR_OFFSETS_LOGIC,
    UConverter_गन्तव्यम्_Unicode_ISO_2022_KR_OFFSETS_LOGIC,
    UConverter_fromUnicode_ISO_2022_KR_OFFSETS_LOGIC,
    UConverter_fromUnicode_ISO_2022_KR_OFFSETS_LOGIC,
    शून्यसूचकः,

    शून्यसूचकः,
    _ISO2022getName,
    _ISO_2022_WriteSub,
    _ISO_2022_SafeClone,
    ISO_2022_Get_Unicode_समुच्चयः,

    शून्यसूचकः,
    शून्यसूचकः
};
स्थिर ध्रुव UConverter_Static_दत्त ISO2022_KRStatic_दत्त={
    परिमाण(UConverter_Static_दत्त),
    "ISO_2022_KR",
    0,
    UCNV_IBM,
    UCNV_ISO_2022,
    1,
    8, /* max 8 bytes per char16_t */
    { 0x1a, 0, 0, 0 },
    1,
    असत्य,
    असत्य,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

नामाकाश {

ध्रुव UConverter_Shared_दत्त _ISO2022KRData=
        UCNV_IMMUTABLE_SHARED_दत्त_INITIALIZER(&ISO2022_KRStatic_दत्त, &_ISO2022KRImpl);

}  // namespace

/*************** CN ***************/
स्थिर ध्रुव UConverterImpl _ISO2022CNImpl={

    UCNV_ISO_2022,

    शून्यसूचकः,
    शून्यसूचकः,

    _ISO2022Open,
    _ISO2022Close,
    _ISO2022Reset,

    UConverter_गन्तव्यम्_Unicode_ISO_2022_CN_OFFSETS_LOGIC,
    UConverter_गन्तव्यम्_Unicode_ISO_2022_CN_OFFSETS_LOGIC,
    UConverter_fromUnicode_ISO_2022_CN_OFFSETS_LOGIC,
    UConverter_fromUnicode_ISO_2022_CN_OFFSETS_LOGIC,
    शून्यसूचकः,

    शून्यसूचकः,
    _ISO2022getName,
    _ISO_2022_WriteSub,
    _ISO_2022_SafeClone,
    ISO_2022_Get_Unicode_समुच्चयः,

    शून्यसूचकः,
    शून्यसूचकः
};
स्थिर ध्रुव UConverter_Static_दत्त ISO2022_CNStatic_दत्त={
    परिमाण(UConverter_Static_दत्त),
    "ISO_2022_CN",
    0,
    UCNV_IBM,
    UCNV_ISO_2022,
    1,
    8, /* max 8 bytes per char16_t: 4-byte CNS designator + 2 bytes for SS2/SS3 + DBCS */
    { 0x1a, 0, 0, 0 },
    1,
    असत्य,
    असत्य,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

नामाकाश {

ध्रुव UConverter_Shared_दत्त _ISO2022CNData=
        UCNV_IMMUTABLE_SHARED_दत्त_INITIALIZER(&ISO2022_CNStatic_दत्त, &_ISO2022CNImpl);

}  // namespace
#यदि_अन्तः /* #if !UCONFIG_ONLY_HTML_CONVERSION */

#यदि_अन्तः /* #if !UCONFIG_NO_LEGACY_CONVERSION */
