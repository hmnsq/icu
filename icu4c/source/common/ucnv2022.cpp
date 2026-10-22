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

#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ !UCONFIG_NO_CONVERSION && !UCONFIG_NO_LEGACY_CONVERSION

#అన్తర్భావ "unicode/ucnv.h"
#అన్తర్భావ "unicode/uset.h"
#అన్తర్భావ "unicode/ucnv_err.h"
#అన్తర్భావ "unicode/ucnv_cb.h"
#అన్తర్భావ "unicode/utf16.h"
#అన్తర్భావ "ucnv_imp.h"
#అన్తర్భావ "ucnv_bld.h"
#అన్తర్భావ "ucnv_cnv.h"
#అన్తర్భావ "ucnvmbcs.h"
#అన్తర్భావ "cstring.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "uassert.h"

#నిర్వచితమైతే U_ENABLE_GENERIC_ISO_2022
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
#అయితే_ముగింపు

#ఒకవేళ !UCONFIG_ONLY_HTML_CONVERSION
స్థిర స్థిరరాశి అక్షరం జరుపు_లోపల_STR[]  = "\x0F";
// static const char SHIFT_OUT_STR[] = "\x0E";
#అయితే_ముగింపు

#నిర్వచించు CR      0x0D
#నిర్వచించు LF      0x0A
#నిర్వచించు H_TAB   0x09
#నిర్వచించు V_TAB   0x0B
#నిర్వచించు SPACE   0x20

గణన {
    HWKANA_ప్రారంభం=0xff61,
    HWKANA_చివర=0xff9f
};

/*
 * 94-character sets with native byte values A1..FE are encoded in ISO 2022
 * as bytes 21..7E. (Subtract 0x80.)
 * 96-character sets with native byte values A0..FF are encoded in ISO 2022
 * as bytes 20..7F. (Subtract 0x80.)
 * Do not encode C1 control codes with native bytes 80..9F
 * as bytes 00..1F (C0 control codes).
 */
గణన {
    GR94_ప్రారంభం=0xa1,
    GR94_చివర=0xfe,
    GR96_ప్రారంభం=0xa0,
    GR96_చివర=0xff
};

/*
 * ISO 2022 control codes must not be converted from Unicode
 * because they would mess up the byte stream.
 * The bit mask 0x0800c000 has bits set at bit positions 0xe, 0xf, 0x1b
 * corresponding to SO, SI, and ESC.
 */
#నిర్వచించు IS_2022_CONTROL(c) (((c)<0x20) && (((uint32_t)1<<(c))&0x0800c000)!=0)

/* for ISO-2022-JP and -CN implementations */
రకనిర్వచనం గణన  {
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
        HWKANA_7_బిట్టు=8,    /* Halfwidth Katakana 7 bit */

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
#ఒకవేళ UCONFIG_ONLY_HTML_CONVERSION
#నిర్వచించు IS_JP_DBCS(cs) (JISX208==(cs))
#లేకపోతే
#నిర్వచించు IS_JP_DBCS(cs) (JISX208<=(cs) && (cs)<=KSC5601)
#అయితే_ముగింపు

#నిర్వచించు CSM(cs) ((uint16_t)1<<(cs))

/*
 * Each of these charset masks (with index x) contains a bit for a charset in exact correspondence
 * to whether that charset is used in the corresponding version x of ISO_2022,locale=ja,version=x
 *
 * Note: The converter uses some leniency:
 * - The escape sequence ESC ( I for half-width 7-bit Katakana is recognized in
 *   all versions, not just JIS7 and JIS8.
 * - ICU does not distinguish between different versions of JIS X 0208.
 */
#ఒకవేళ UCONFIG_ONLY_HTML_CONVERSION
గణన { MAX_JA_సంస్కరణ=0 };
#లేకపోతే
గణన { MAX_JA_సంస్కరణ=4 };
#అయితే_ముగింపు
స్థిర స్థిరరాశి uint16_t jpCharsetMasks[MAX_JA_సంస్కరణ+1]={
    CSM(ASCII)|CSM(JISX201)|CSM(JISX208)|CSM(HWKANA_7_బిట్టు),
#ఒకవేళ !UCONFIG_ONLY_HTML_CONVERSION
    CSM(ASCII)|CSM(JISX201)|CSM(JISX208)|CSM(HWKANA_7_బిట్టు)|CSM(JISX212),
    CSM(ASCII)|CSM(JISX201)|CSM(JISX208)|CSM(HWKANA_7_బిట్టు)|CSM(JISX212)|CSM(GB2312)|CSM(KSC5601)|CSM(ISO8859_1)|CSM(ISO8859_7),
    CSM(ASCII)|CSM(JISX201)|CSM(JISX208)|CSM(HWKANA_7_బిట్టు)|CSM(JISX212)|CSM(GB2312)|CSM(KSC5601)|CSM(ISO8859_1)|CSM(ISO8859_7),
    CSM(ASCII)|CSM(JISX201)|CSM(JISX208)|CSM(HWKANA_7_బిట్టు)|CSM(JISX212)|CSM(GB2312)|CSM(KSC5601)|CSM(ISO8859_1)|CSM(ISO8859_7)
#అయితే_ముగింపు
};

రకనిర్వచనం గణన {
        ASCII1=0,
        LATIN1,
        SBCS,
        DBCS,
        MBCS,
        HWKANA
}Cnv2022Type;

రకనిర్వచనం నిర్మాణం ISO2022State {
    int8_t cs[4];       /* charset number for SI (G0)/SO (G1)/SS2 (G2)/SS3 (G3) */
    int8_t g;           /* 0..3 for G0..G3 (SI/SO/SS2/SS3) */
    int8_t prevG;       /* g before single shift (SS2 or SS3) */
} ISO2022State;

#నిర్వచించు UCNV_OPTIONS_సంస్కరణ_ముసుగు 0xf
#నిర్వచించు UCNV_2022_MAX_CONVERTERS 10

రకనిర్వచనం నిర్మాణం{
    UConverter_Shared_దత్తాంశం *myConverterArray[UCNV_2022_MAX_CONVERTERS];
    UConverter *currentConverter;
    Cnv2022Type currentType;
    ISO2022State గమ్యం_U2022_State, fromU2022State;
    uint32_t కీలకం;
    uint32_t సంస్కరణ;
#నిర్వచితమైతే U_ENABLE_GENERIC_ISO_2022
    UBool is_మొదటి_బఫరు;
#అయితే_ముగింపు
    UBool is_ఖాళీలు_Segment;
    అక్షరం name[30];
    అక్షరం locale[3];
}UConverter_దత్తాంశం_ISO2022;

/* Protos */
/* ISO-2022 ----------------------------------------------------------------- */

/*Forward declaration */
U_CFUNC శూన్య U_CALLCONV
ucnv_fromUnicode_UTF8(UConverter_From_Unicode_వాదనలు * వాదనలు_2,
                      UError_సంకేతం * err);
U_CFUNC శూన్య U_CALLCONV
ucnv_fromUnicode_UTF8_OFFSETS_LOGIC(UConverter_From_Unicode_వాదనలు * వాదనలు_2,
                                    UError_సంకేతం * err);

#నిర్వచించు ESC_2022 0x1B /*ESC*/

రకనిర్వచనం గణన
{
        INVALID_2022 = -1, /*Doesn't correspond to a valid iso 2022 escape sequence*/
        VALID_NON_TERMINAL_2022 = 0, /*so far corresponds to a valid iso 2022 escape sequence*/
        VALID_TERMINAL_2022 = 1, /*corresponds to a valid iso 2022 escape sequence*/
        VALID_MAYBE_TERMINAL_2022 = 2 /*so far matches one iso 2022 escape sequence, but by adding more characters might match another escape sequence*/
} UCNV_పట్టిక_స్థితులు_2022;

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
స్థిర స్థిరరాశి int8_t normalize_esq_chars_2022[256] = {
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

#నిర్వచితమైతే U_ENABLE_GENERIC_ISO_2022
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
#అయితే_ముగింపు

#నిర్వచించు MAX_స్థితులు_2022 74
స్థిర స్థిరరాశి int32_t esc_Seq_State_పట్టిక_కీలకం_2022[MAX_స్థితులు_2022] = {
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

#నిర్వచితమైతే U_ENABLE_GENERIC_ISO_2022

స్థిర స్థిరరాశి అక్షరం* స్థిరరాశి esc_Seq_State_పట్టిక_ఫలితం_2022[MAX_స్థితులు_2022] = {
 /*  0                      1                        2                      3                   4                   5                        6                      7                       8                       9    */

     శూన్యసూచిక                   ,శూన్యసూచిక                   ,శూన్యసూచిక                   ,శూన్యసూచిక               ,శూన్యసూచిక               ,శూన్యసూచిక                   ,శూన్యసూచిక                   ,శూన్యసూచిక                   ,"latin1"               ,"latin1"
    ,"latin1"               ,"ibm-865"              ,"ibm-865"              ,"ibm-865"          ,"ibm-865"          ,"ibm-865"              ,"ibm-865"              ,"JISX0201"             ,"JISX0201"             ,"latin1"
    ,"latin1"               ,శూన్యసూచిక                   ,"JISX-208"             ,"ibm-5478"         ,"JISX-208"         ,శూన్యసూచిక                   ,శూన్యసూచిక                   ,శూన్యసూచిక                   ,శూన్యసూచిక                   ,"UTF8"
    ,"ISO-8859-1"           ,"ISO-8859-7"           ,"JIS-X-208"            ,శూన్యసూచిక               ,"ibm-955"          ,"ibm-367"              ,"ibm-952"              ,"ibm-949"              ,"JISX-212"             ,"ibm-1383"
    ,"ibm-952"              ,"ibm-964"              ,"ibm-964"              ,"ibm-964"          ,"ibm-964"          ,"ibm-964"              ,"ibm-964"              ,"ibm-5478"         ,"ibm-949"              ,"ISO-IR-165"
    ,"CNS-11643-1992,1"     ,"CNS-11643-1992,2"     ,"CNS-11643-1992,3"     ,"CNS-11643-1992,4" ,"CNS-11643-1992,5" ,"CNS-11643-1992,6"     ,"CNS-11643-1992,7"     ,"UTF16_PlatformEndian" ,"UTF16_PlatformEndian" ,"UTF16_PlatformEndian"
    ,"UTF16_PlatformEndian" ,"UTF16_PlatformEndian" ,"UTF16_PlatformEndian" ,శూన్యసూచిక               ,"latin1"           ,"ibm-912"              ,"ibm-913"              ,"ibm-914"              ,"ibm-813"              ,"ibm-1089"
    ,"ibm-920"              ,"ibm-915"              ,"ibm-915"              ,"latin1"
};

#అయితే_ముగింపు

స్థిర స్థిరరాశి int8_t esc_Seq_State_పట్టిక_విలువ_2022[MAX_స్థితులు_2022] = {
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
రకనిర్వచనం గణన{
#నిర్వచితమైతే U_ENABLE_GENERIC_ISO_2022
    ISO_2022=0,
#అయితే_ముగింపు
    ISO_2022_JP=1,
#ఒకవేళ !UCONFIG_ONLY_HTML_CONVERSION
    ISO_2022_KR=2,
    ISO_2022_CN=3
#అయితే_ముగింపు
} Variant2022;

/*********** ISO 2022 Converter Protos ***********/
స్థిర శూన్య U_CALLCONV
_ISO2022Open(UConverter *cnv, UConverter_ఎక్కించు_వాదనలు *p_వాదనలు, UError_సంకేతం *error_సంకేతం);

స్థిర శూన్య U_CALLCONV
 _ISO2022Close(UConverter *converter);

స్థిర శూన్య U_CALLCONV
_ISO2022Reset(UConverter *converter, UConverterResetChoice choice);

U_CDECL_BEGIN
స్థిర స్థిరరాశి అక్షరం * U_CALLCONV
_ISO2022getName(స్థిరరాశి UConverter* cnv);
U_CDECL_చివర

స్థిర శూన్య  U_CALLCONV
_ISO_2022_WriteSub(UConverter_From_Unicode_వాదనలు *వాదనలు_2, int32_t స్థానభ్రంశం_సూచీ, UError_సంకేతం *err);

U_CDECL_BEGIN
స్థిర UConverter * U_CALLCONV
ISO_2022_సురక్షిత_Clone(స్థిరరాశి UConverter *cnv, శూన్య *దొంతర_బఫరు, int32_t *p_బఫరు_కొలత, UError_సంకేతం *స్థితి_2);

U_CDECL_చివర

#నిర్వచితమైతే U_ENABLE_GENERIC_ISO_2022
స్థిర శూన్య U_CALLCONV
T_UConverter_గమ్యం_Unicode_ISO_2022_OFFSETS_LOGIC(UConverter_గమ్యం_Unicode_వాదనలు* వాదనలు_2, UError_సంకేతం* err);
#అయితే_ముగింపు

నామాకాశ {

/*const UConverterSharedData _ISO2022Data;*/
బాహ్య స్థిరరాశి UConverter_Shared_దత్తాంశం _ISO2022JPData;

#ఒకవేళ !UCONFIG_ONLY_HTML_CONVERSION
బాహ్య స్థిరరాశి UConverter_Shared_దత్తాంశం _ISO2022KRData;
బాహ్య స్థిరరాశి UConverter_Shared_దత్తాంశం _ISO2022CNData;
#అయితే_ముగింపు

}  // namespace

/*************** Converter implementations ******************/

/* The purpose of this function is to get around gcc compiler warnings. */
స్థిర అంతఃవరుస శూన్య
fromUWriteUInt8(UConverter *cnv,
                 స్థిరరాశి అక్షరం *బైట్లు, int32_t length,
                 uint8_t **లక్ష్యం, స్థిరరాశి అక్షరం *లక్ష్యం_హద్దు,
                 int32_t **offsets,
                 int32_t ఆధారం_సూచీ,
                 UError_సంకేతం *p_Error_సంకేతం)
{
    అక్షరం* లక్ష్యం_Chars = reinterpret_cast<అక్షరం*>(*లక్ష్యం);
    ucnv_from_UWrite_బైట్లు(cnv, బైట్లు, length, &లక్ష్యం_Chars, లక్ష్యం_హద్దు,
                         offsets, ఆధారం_సూచీ, p_Error_సంకేతం);
    *లక్ష్యం = reinterpret_cast<uint8_t*>(లక్ష్యం_Chars);

}

స్థిర అంతఃవరుస శూన్య
సమితి_Initial_State_గమ్యం_Unicode_KR(UConverter* /*converter*/, UConverter_దత్తాంశం_ISO2022 *my_Converter_దత్తాంశం){
    ఒకవేళ(my_Converter_దత్తాంశం->సంస్కరణ == 1) {
        UConverter *cnv = my_Converter_దత్తాంశం->currentConverter;

        cnv->గమ్యం_Unicode_స్థితి=0;     /*స్థానభ్రంశం*/
        cnv->బహుళకం=0;                /* state */
        cnv->గమ్యం_ULength=0;           /* byteIndex */
    }
}

స్థిర అంతఃవరుస శూన్య
సమితి_Initial_State_From_Unicode_KR(UConverter* converter,UConverter_దత్తాంశం_ISO2022 *my_Converter_దత్తాంశం){
   /* in ISO-2022-KR the designator sequence appears only once
    * in a file so we append it only once
    */
    ఒకవేళ( converter->char_Error_బఫరు_నిడివి==0){

        converter->char_Error_బఫరు_నిడివి = 4;
        converter->char_Error_బఫరు[0] = 0x1b;
        converter->char_Error_బఫరు[1] = 0x24;
        converter->char_Error_బఫరు[2] = 0x29;
        converter->char_Error_బఫరు[3] = 0x43;
    }
    ఒకవేళ(my_Converter_దత్తాంశం->సంస్కరణ == 1) {
        UConverter *cnv = my_Converter_దత్తాంశం->currentConverter;

        cnv->fromUChar32=0;
        cnv->from_Unicode_స్థితి=1;   /* prevLength */
    }
}

స్థిర శూన్య U_CALLCONV
_ISO2022Open(UConverter *cnv, UConverter_ఎక్కించు_వాదనలు *p_వాదనలు, UError_సంకేతం *error_సంకేతం){

    అక్షరం myLocale[7]={' ',' ',' ',' ',' ',' ', '\0'};

    cnv->extraInfo = uprv_malloc (పరిమాణం (UConverter_దత్తాంశం_ISO2022));
    ఒకవేళ(cnv->extraInfo != శూన్యసూచిక) {
        UConverterNamePieces దొంతర_Pieces;
        UConverter_ఎక్కించు_వాదనలు దొంతర_వాదనలు=UCNV_ఎక్కించు_వాదనలు_INITIALIZER;
        UConverter_దత్తాంశం_ISO2022* my_Converter_దత్తాంశం = static_cast<UConverter_దత్తాంశం_ISO2022*>(cnv->extraInfo);
        uint32_t సంస్కరణ;

        దొంతర_వాదనలు.onlyTestIsLoadable = p_వాదనలు->onlyTestIsLoadable;

        uprv_memset(my_Converter_దత్తాంశం, 0, పరిమాణం(UConverter_దత్తాంశం_ISO2022));
        my_Converter_దత్తాంశం->currentType = ASCII1;
        cnv->from_Unicode_స్థితి =అబద్ధం;
        ఒకవేళ(p_వాదనలు->locale){
            uprv_strncpy(myLocale, p_వాదనలు->locale, పరిమాణం(myLocale)-1);
        }
        సంస్కరణ = p_వాదనలు->options & UCNV_OPTIONS_సంస్కరణ_ముసుగు;
        my_Converter_దత్తాంశం->సంస్కరణ = సంస్కరణ;
        ఒకవేళ(myLocale[0]=='j' && (myLocale[1]=='a'|| myLocale[1]=='p') &&
            (myLocale[2]=='_' || myLocale[2]=='\0'))
        {
            /* open the required converters and cache them */
            ఒకవేళ(సంస్కరణ>MAX_JA_సంస్కరణ) {
                // ICU 55 fails to open a converter for an unsupported version.
                // Previously, it fell back to version 0, but that would yield
                // unexpected behavior.
                *error_సంకేతం = U_MISSING_RESOURCE_ERROR;
                తిరిగి;
            }
            ఒకవేళ(jpCharsetMasks[సంస్కరణ]&CSM(ISO8859_7)) {
                my_Converter_దత్తాంశం->myConverterArray[ISO8859_7] =
                    ucnv_ఎక్కించు_Shared_దత్తాంశం("ISO8859_7", &దొంతర_Pieces, &దొంతర_వాదనలు, error_సంకేతం);
            }
            my_Converter_దత్తాంశం->myConverterArray[JISX208] =
                ucnv_ఎక్కించు_Shared_దత్తాంశం("Shift-JIS", &దొంతర_Pieces, &దొంతర_వాదనలు, error_సంకేతం);
            ఒకవేళ(jpCharsetMasks[సంస్కరణ]&CSM(JISX212)) {
                my_Converter_దత్తాంశం->myConverterArray[JISX212] =
                    ucnv_ఎక్కించు_Shared_దత్తాంశం("jisx-212", &దొంతర_Pieces, &దొంతర_వాదనలు, error_సంకేతం);
            }
            ఒకవేళ(jpCharsetMasks[సంస్కరణ]&CSM(GB2312)) {
                my_Converter_దత్తాంశం->myConverterArray[GB2312] =
                    ucnv_ఎక్కించు_Shared_దత్తాంశం("ibm-5478", &దొంతర_Pieces, &దొంతర_వాదనలు, error_సంకేతం);   /* gb_2312_80-1 */
            }
            ఒకవేళ(jpCharsetMasks[సంస్కరణ]&CSM(KSC5601)) {
                my_Converter_దత్తాంశం->myConverterArray[KSC5601] =
                    ucnv_ఎక్కించు_Shared_దత్తాంశం("ksc_5601", &దొంతర_Pieces, &దొంతర_వాదనలు, error_సంకేతం);
            }

            /* set the function pointers to appropriate functions */
            cnv->shared_దత్తాంశం = const_cast<UConverter_Shared_దత్తాంశం*>(&_ISO2022JPData);
            uprv_strcpy(my_Converter_దత్తాంశం->locale,"ja");

            (శూన్య)uprv_strcpy(my_Converter_దత్తాంశం->name,"ISO_2022,locale=ja,version=");
            size_t len = uprv_strlen(my_Converter_దత్తాంశం->name);
            my_Converter_దత్తాంశం->name[len] = static_cast<అక్షరం>(my_Converter_దత్తాంశం->సంస్కరణ + static_cast<పూర్ణాంకం>('0'));
            my_Converter_దత్తాంశం->name[len+1]='\0';
        }
#ఒకవేళ !UCONFIG_ONLY_HTML_CONVERSION
        లేకపోతే ఒకవేళ(myLocale[0]=='k' && (myLocale[1]=='o'|| myLocale[1]=='r') &&
            (myLocale[2]=='_' || myLocale[2]=='\0'))
        {
            ఒకవేళ(సంస్కరణ>1) {
                // ICU 55 fails to open a converter for an unsupported version.
                // Previously, it fell back to version 0, but that would yield
                // unexpected behavior.
                *error_సంకేతం = U_MISSING_RESOURCE_ERROR;
                తిరిగి;
            }
            స్థిరరాశి అక్షరం *cnvName;
            ఒకవేళ(సంస్కరణ==1) {
                cnvName="icu-internal-25546";
            } లేకపోతే {
                cnvName="ibm-949";
                my_Converter_దత్తాంశం->సంస్కరణ=సంస్కరణ=0;
            }
            ఒకవేళ(p_వాదనలు->onlyTestIsLoadable) {
                ucnv_canCreateConverter(cnvName, error_సంకేతం);  /* errorCode carries result */
                uprv_free(cnv->extraInfo);
                cnv->extraInfo=శూన్యసూచిక;
                తిరిగి;
            } లేకపోతే {
                my_Converter_దత్తాంశం->currentConverter=ucnv_open(cnvName, error_సంకేతం);
                ఒకవేళ (U_FAILURE(*error_సంకేతం)) {
                    _ISO2022Close(cnv);
                    తిరిగి;
                }

                ఒకవేళ(సంస్కరణ==1) {
                    (శూన్య)uprv_strcpy(my_Converter_దత్తాంశం->name,"ISO_2022,locale=ko,version=1");
                    uprv_memcpy(cnv->subChars, my_Converter_దత్తాంశం->currentConverter->subChars, 4);
                    cnv->subCharLen = my_Converter_దత్తాంశం->currentConverter->subCharLen;
                }లేకపోతే{
                    (శూన్య)uprv_strcpy(my_Converter_దత్తాంశం->name,"ISO_2022,locale=ko,version=0");
                }

                /* initialize the state variables */
                సమితి_Initial_State_గమ్యం_Unicode_KR(cnv, my_Converter_దత్తాంశం);
                సమితి_Initial_State_From_Unicode_KR(cnv, my_Converter_దత్తాంశం);

                /* set the function pointers to appropriate functions */
                cnv->shared_దత్తాంశం = const_cast<UConverter_Shared_దత్తాంశం*>(&_ISO2022KRData);
                uprv_strcpy(my_Converter_దత్తాంశం->locale,"ko");
            }
        }
        లేకపోతే ఒకవేళ(((myLocale[0]=='z' && myLocale[1]=='h') || (myLocale[0]=='c'&& myLocale[1]=='n'))&&
            (myLocale[2]=='_' || myLocale[2]=='\0'))
        {
            ఒకవేళ(సంస్కరణ>2) {
                // ICU 55 fails to open a converter for an unsupported version.
                // Previously, it fell back to version 0, but that would yield
                // unexpected behavior.
                *error_సంకేతం = U_MISSING_RESOURCE_ERROR;
                తిరిగి;
            }

            /* open the required converters and cache them */
            my_Converter_దత్తాంశం->myConverterArray[GB2312_1] =
                ucnv_ఎక్కించు_Shared_దత్తాంశం("ibm-5478", &దొంతర_Pieces, &దొంతర_వాదనలు, error_సంకేతం);
            ఒకవేళ(సంస్కరణ>=1) {
                my_Converter_దత్తాంశం->myConverterArray[ISO_IR_165] =
                    ucnv_ఎక్కించు_Shared_దత్తాంశం("iso-ir-165", &దొంతర_Pieces, &దొంతర_వాదనలు, error_సంకేతం);
            }
            my_Converter_దత్తాంశం->myConverterArray[CNS_11643] =
                ucnv_ఎక్కించు_Shared_దత్తాంశం("cns-11643-1992", &దొంతర_Pieces, &దొంతర_వాదనలు, error_సంకేతం);


            /* set the function pointers to appropriate functions */
            cnv->shared_దత్తాంశం = const_cast<UConverter_Shared_దత్తాంశం*>(&_ISO2022CNData);
            uprv_strcpy(my_Converter_దత్తాంశం->locale,"cn");

            ఒకవేళ (సంస్కరణ==0){
                my_Converter_దత్తాంశం->సంస్కరణ = 0;
                (శూన్య)uprv_strcpy(my_Converter_దత్తాంశం->name,"ISO_2022,locale=zh,version=0");
            }లేకపోతే ఒకవేళ (సంస్కరణ==1){
                my_Converter_దత్తాంశం->సంస్కరణ = 1;
                (శూన్య)uprv_strcpy(my_Converter_దత్తాంశం->name,"ISO_2022,locale=zh,version=1");
            }లేకపోతే {
                my_Converter_దత్తాంశం->సంస్కరణ = 2;
                (శూన్య)uprv_strcpy(my_Converter_దత్తాంశం->name,"ISO_2022,locale=zh,version=2");
            }
        }
#అయితే_ముగింపు  // !UCONFIG_ONLY_HTML_CONVERSION
        లేకపోతే{
#నిర్వచితమైతే U_ENABLE_GENERIC_ISO_2022
            my_Converter_దత్తాంశం->is_మొదటి_బఫరు = నిజం;

            /* append the UTF-8 escape sequence */
            cnv->char_Error_బఫరు_నిడివి = 3;
            cnv->char_Error_బఫరు[0] = 0x1b;
            cnv->char_Error_బఫరు[1] = 0x25;
            cnv->char_Error_బఫరు[2] = 0x42;

            cnv->shared_దత్తాంశం=(UConverter_Shared_దత్తాంశం*)&ISO2022_దత్తాంశం;
            /* initialize the state variables */
            uprv_strcpy(my_Converter_దత్తాంశం->name,"ISO_2022");
#లేకపోతే
            *error_సంకేతం = U_MISSING_RESOURCE_ERROR;
            // Was U_UNSUPPORTED_ERROR but changed in ICU 55 to a more standard
            // data loading error code.
            తిరిగి;
#అయితే_ముగింపు
        }

        cnv->max_బైట్లు_చొప్పున_UChar=cnv->shared_దత్తాంశం->static_దత్తాంశం->max_బైట్లు_చొప్పున_Char;

        ఒకవేళ(U_FAILURE(*error_సంకేతం) || p_వాదనలు->onlyTestIsLoadable) {
            _ISO2022Close(cnv);
        }
    } లేకపోతే {
        *error_సంకేతం = U_జ్ఞాపకం_కేటాయింపు_ERROR;
    }
}


స్థిర శూన్య U_CALLCONV
_ISO2022Close(UConverter *converter) {
    UConverter_దత్తాంశం_ISO2022* my_దత్తాంశం = static_cast<UConverter_దత్తాంశం_ISO2022*>(converter->extraInfo);
    UConverter_Shared_దత్తాంశం **array = my_దత్తాంశం->myConverterArray;
    int32_t i;

    ఒకవేళ (converter->extraInfo != శూన్యసూచిక) {
        /*close the array of converter pointers and free the memory*/
        కొరకు (i=0; i<UCNV_2022_MAX_CONVERTERS; i++) {
            ఒకవేళ(array[i]!=శూన్యసూచిక) {
                ucnv_unload_Shared_దత్తాంశం_If_Ready(array[i]);
            }
        }

        ucnv_close(my_దత్తాంశం->currentConverter);

        ఒకవేళ(!converter->isExtraLocal){
            uprv_free (converter->extraInfo);
            converter->extraInfo = శూన్యసూచిక;
        }
    }
}

స్థిర శూన్య U_CALLCONV
_ISO2022Reset(UConverter *converter, UConverterResetChoice choice) {
    UConverter_దత్తాంశం_ISO2022* my_Converter_దత్తాంశం = static_cast<UConverter_దత్తాంశం_ISO2022*>(converter->extraInfo);
    ఒకవేళ(choice<=UCNV_RESET_గమ్యం_UNICODE) {
        uprv_memset(&my_Converter_దత్తాంశం->గమ్యం_U2022_State, 0, పరిమాణం(ISO2022State));
        my_Converter_దత్తాంశం->కీలకం = 0;
        my_Converter_దత్తాంశం->is_ఖాళీలు_Segment = అబద్ధం;
    }
    ఒకవేళ(choice!=UCNV_RESET_గమ్యం_UNICODE) {
        uprv_memset(&my_Converter_దత్తాంశం->fromU2022State, 0, పరిమాణం(ISO2022State));
    }
#నిర్వచితమైతే U_ENABLE_GENERIC_ISO_2022
    ఒకవేళ(my_Converter_దత్తాంశం->locale[0] == 0){
        ఒకవేళ(choice<=UCNV_RESET_గమ్యం_UNICODE) {
            my_Converter_దత్తాంశం->is_మొదటి_బఫరు = నిజం;
            my_Converter_దత్తాంశం->కీలకం = 0;
            ఒకవేళ (converter->బహుళకం == UCNV_SO){
                ucnv_close (my_Converter_దత్తాంశం->currentConverter);
                my_Converter_దత్తాంశం->currentConverter=శూన్యసూచిక;
            }
            converter->బహుళకం = UCNV_SI;
        }
        ఒకవేళ(choice!=UCNV_RESET_గమ్యం_UNICODE) {
            /* re-append UTF-8 escape sequence */
            converter->char_Error_బఫరు_నిడివి = 3;
            converter->char_Error_బఫరు[0] = 0x1b;
            converter->char_Error_బఫరు[1] = 0x28;
            converter->char_Error_బఫరు[2] = 0x42;
        }
    }
    లేకపోతే
#అయితే_ముగింపు
    {
        /* reset the state variables */
        ఒకవేళ(my_Converter_దత్తాంశం->locale[0] == 'k'){
            ఒకవేళ(choice<=UCNV_RESET_గమ్యం_UNICODE) {
                సమితి_Initial_State_గమ్యం_Unicode_KR(converter, my_Converter_దత్తాంశం);
            }
            ఒకవేళ(choice!=UCNV_RESET_గమ్యం_UNICODE) {
                సమితి_Initial_State_From_Unicode_KR(converter, my_Converter_దత్తాంశం);
            }
        }
    }
}

U_CDECL_BEGIN

స్థిర స్థిరరాశి అక్షరం * U_CALLCONV
_ISO2022getName(స్థిరరాశి UConverter* cnv){
    ఒకవేళ(cnv->extraInfo){
        UConverter_దత్తాంశం_ISO2022* my_దత్తాంశం= (UConverter_దత్తాంశం_ISO2022*)cnv->extraInfo;
        తిరిగి my_దత్తాంశం->name;
    }
    తిరిగి శూన్యసూచిక;
}

U_CDECL_చివర


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
స్థిర స్థిరరాశి int8_t తదుపరి_State_గమ్యం_Unicode_JP[MAX_స్థితులు_2022]= {
/*      0                1               2               3               4               5               6               7               8               9    */
    INVALID_STATE   ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,SS2_STATE      ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE
    ,ASCII          ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,JISX201        ,HWKANA_7_బిట్టు    ,JISX201        ,INVALID_STATE
    ,INVALID_STATE  ,INVALID_STATE  ,JISX208        ,GB2312         ,JISX208        ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE
    ,ISO8859_1      ,ISO8859_7      ,JISX208        ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,KSC5601        ,JISX212        ,INVALID_STATE
    ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE
    ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE
    ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE
    ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE  ,INVALID_STATE
};

#ఒకవేళ !UCONFIG_ONLY_HTML_CONVERSION
/*************** to unicode *******************/
స్థిర స్థిరరాశి int8_t తదుపరి_State_గమ్యం_Unicode_CN[MAX_స్థితులు_2022]= {
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
#అయితే_ముగింపు


స్థిర UCNV_పట్టిక_స్థితులు_2022
get_కీలకం_2022(అక్షరం c,int32_t* కీలకం,int32_t* స్థానభ్రంశం){
    int32_t togo;
    int32_t దిగువ = 0;
    int32_t hi = MAX_స్థితులు_2022;
    int32_t oldmid=0;

    togo = normalize_esq_chars_2022[static_cast<uint8_t>(c)];
    ఒకవేళ(togo == 0) {
        /* not a valid character anywhere in an escape sequence */
        *కీలకం = 0;
        *స్థానభ్రంశం = 0;
        తిరిగి INVALID_2022;
    }
    togo = (*కీలకం << 5) + togo;

    అయితే (hi != దిగువ)  /*ద్విమాన శోధన*/{

        int32_t mid = (hi+దిగువ) >> 1; /*Finds median*/

        ఒకవేళ (mid == oldmid)
            ఆపు;

        ఒకవేళ (esc_Seq_State_పట్టిక_కీలకం_2022[mid] > togo){
            hi = mid;
        }
        లేకపోతే ఒకవేళ (esc_Seq_State_పట్టిక_కీలకం_2022[mid] < togo){
            దిగువ = mid;
        }
        లేకపోతే /*we found it*/{
            *కీలకం = togo;
            *స్థానభ్రంశం = mid;
            తిరిగి static_cast<UCNV_పట్టిక_స్థితులు_2022>(esc_Seq_State_పట్టిక_విలువ_2022[mid]);
        }
        oldmid = mid;

    }

    *కీలకం = 0;
    *స్థానభ్రంశం = 0;
    తిరిగి INVALID_2022;
}

/*runs through a state machine to determine the escape sequence - codepage correspondence
 */
స్థిర శూన్య
changeState_2022(UConverter* _this,
                స్థిరరాశి అక్షరం** ఆధారం,
                స్థిరరాశి అక్షరం* ఆధారం_హద్దు,
                Variant2022 var,
                UError_సంకేతం* err){
    UCNV_పట్టిక_స్థితులు_2022 value;
    UConverter_దత్తాంశం_ISO2022* myData2022 = static_cast<UConverter_దత్తాంశం_ISO2022*>(_this->extraInfo);
    uint32_t కీలకం = myData2022->కీలకం;
    int32_t స్థానభ్రంశం = 0;
    int8_t initial_గమ్యం_ULength = _this->గమ్యం_ULength;
    అక్షరం c;

    value = VALID_NON_TERMINAL_2022;
    అయితే (*ఆధారం < ఆధారం_హద్దు) {
        c = *(*ఆధారం)++;
        _this->గమ్యం_UBytes[_this->గమ్యం_ULength++] = static_cast<uint8_t>(c);
        value = get_కీలకం_2022(c, reinterpret_cast<int32_t*>(&కీలకం), &స్థానభ్రంశం);

        ఎంపిక (value){

        స్థితి VALID_NON_TERMINAL_2022 :
            /* continue with the loop */
            ఆపు;

        స్థితి VALID_TERMINAL_2022:
            కీలకం = 0;
            వెళ్ళు పూర్తి_2;

        స్థితి INVALID_2022:
            వెళ్ళు పూర్తి_2;

        స్థితి VALID_MAYBE_TERMINAL_2022:
#నిర్వచితమైతే U_ENABLE_GENERIC_ISO_2022
            /* ESC ( B is ambiguous only for ISO_2022 itself */
            ఒకవేళ(var == ISO_2022) {
                /* discard toUBytes[] for ESC ( B because this sequence is correct and complete */
                _this->గమ్యం_ULength = 0;

                /* TODO need to indicate that ESC ( B was seen; if failure, then need to replay from source or from MBCS-style replay */

                /* continue with the loop */
                value = VALID_NON_TERMINAL_2022;
                ఆపు;
            } లేకపోతే
#అయితే_ముగింపు
            {
                /* not ISO_2022 itself, finish here */
                value = VALID_TERMINAL_2022;
                కీలకం = 0;
                వెళ్ళు పూర్తి_2;
            }
        }
    }

పూర్తి_2:
    myData2022->కీలకం = కీలకం;

    ఒకవేళ (value == VALID_NON_TERMINAL_2022) {
        /* indicate that the escape sequence is incomplete: key!=0 */
        తిరిగి;
    } లేకపోతే ఒకవేళ (value == INVALID_2022 ) {
        *err = U_ILLEGAL_ESCAPE_SEQUENCE;
    } లేకపోతే /* value == VALID_TERMINAL_2022 */ {
        ఎంపిక(var){
#నిర్వచితమైతే U_ENABLE_GENERIC_ISO_2022
        స్థితి ISO_2022:
        {
            స్థిరరాశి అక్షరం *chosenConverterName = esc_Seq_State_పట్టిక_ఫలితం_2022[స్థానభ్రంశం];
            ఒకవేళ(chosenConverterName == శూన్యసూచిక) {
                /* SS2 or SS3 */
                *err = U_UNSUPPORTED_ESCAPE_SEQUENCE;
                _this->గమ్యం_UCallback_Reason = UCNV_UNASSIGNED;
                తిరిగి;
            }

            _this->బహుళకం = UCNV_SI;
            ucnv_close(myData2022->currentConverter);
            myData2022->currentConverter = myUConverter = ucnv_open(chosenConverterName, err);
            ఒకవేళ(U_SUCCESS(*err)) {
                myUConverter->fromCharErrorBehaviour = UCNV_గమ్యం_U_CALLBACK_STOP;
                _this->బహుళకం = UCNV_SO;
            }
            ఆపు;
        }
#అయితే_ముగింపు
        స్థితి ISO_2022_JP:
            {
                StateEnum తాత్కాలిక_State = static_cast<StateEnum>(తదుపరి_State_గమ్యం_Unicode_JP[స్థానభ్రంశం]);
                ఎంపిక(తాత్కాలిక_State) {
                స్థితి INVALID_STATE:
                    *err = U_UNSUPPORTED_ESCAPE_SEQUENCE;
                    ఆపు;
                స్థితి SS2_STATE:
                    ఒకవేళ(myData2022->గమ్యం_U2022_State.cs[2]!=0) {
                        ఒకవేళ(myData2022->గమ్యం_U2022_State.g<2) {
                            myData2022->గమ్యం_U2022_State.prevG=myData2022->గమ్యం_U2022_State.g;
                        }
                        myData2022->గమ్యం_U2022_State.g=2;
                    } లేకపోతే {
                        /* illegal to have SS2 before a matching designator */
                        *err = U_ILLEGAL_ESCAPE_SEQUENCE;
                    }
                    ఆపు;
                /* case SS3_STATE: not used in ISO-2022-JP-x */
                స్థితి ISO8859_1:
                స్థితి ISO8859_7:
                    ఒకవేళ((jpCharsetMasks[myData2022->సంస్కరణ] & CSM(తాత్కాలిక_State)) == 0) {
                        *err = U_UNSUPPORTED_ESCAPE_SEQUENCE;
                    } లేకపోతే {
                        /* G2 charset for SS2 */
                        myData2022->గమ్యం_U2022_State.cs[2] = static_cast<int8_t>(తాత్కాలిక_State);
                    }
                    ఆపు;
                అప్రమేయ:
                    ఒకవేళ((jpCharsetMasks[myData2022->సంస్కరణ] & CSM(తాత్కాలిక_State)) == 0) {
                        *err = U_UNSUPPORTED_ESCAPE_SEQUENCE;
                    } లేకపోతే {
                        /* G0 charset */
                        myData2022->గమ్యం_U2022_State.cs[0] = static_cast<int8_t>(తాత్కాలిక_State);
                    }
                    ఆపు;
                }
            }
            ఆపు;
#ఒకవేళ !UCONFIG_ONLY_HTML_CONVERSION
        స్థితి ISO_2022_CN:
            {
                StateEnum తాత్కాలిక_State = static_cast<StateEnum>(తదుపరి_State_గమ్యం_Unicode_CN[స్థానభ్రంశం]);
                ఎంపిక(తాత్కాలిక_State) {
                స్థితి INVALID_STATE:
                    *err = U_UNSUPPORTED_ESCAPE_SEQUENCE;
                    ఆపు;
                స్థితి SS2_STATE:
                    ఒకవేళ(myData2022->గమ్యం_U2022_State.cs[2]!=0) {
                        ఒకవేళ(myData2022->గమ్యం_U2022_State.g<2) {
                            myData2022->గమ్యం_U2022_State.prevG=myData2022->గమ్యం_U2022_State.g;
                        }
                        myData2022->గమ్యం_U2022_State.g=2;
                    } లేకపోతే {
                        /* illegal to have SS2 before a matching designator */
                        *err = U_ILLEGAL_ESCAPE_SEQUENCE;
                    }
                    ఆపు;
                స్థితి SS3_STATE:
                    ఒకవేళ(myData2022->గమ్యం_U2022_State.cs[3]!=0) {
                        ఒకవేళ(myData2022->గమ్యం_U2022_State.g<2) {
                            myData2022->గమ్యం_U2022_State.prevG=myData2022->గమ్యం_U2022_State.g;
                        }
                        myData2022->గమ్యం_U2022_State.g=3;
                    } లేకపోతే {
                        /* illegal to have SS3 before a matching designator */
                        *err = U_ILLEGAL_ESCAPE_SEQUENCE;
                    }
                    ఆపు;
                స్థితి ISO_IR_165:
                    ఒకవేళ(myData2022->సంస్కరణ==0) {
                        *err = U_UNSUPPORTED_ESCAPE_SEQUENCE;
                        ఆపు;
                    }
                    U_FALLTHROUGH;
                స్థితి GB2312_1:
                    U_FALLTHROUGH;
                స్థితి CNS_11643_1:
                    myData2022->గమ్యం_U2022_State.cs[1] = static_cast<int8_t>(తాత్కాలిక_State);
                    ఆపు;
                స్థితి CNS_11643_2:
                    myData2022->గమ్యం_U2022_State.cs[2] = static_cast<int8_t>(తాత్కాలిక_State);
                    ఆపు;
                అప్రమేయ:
                    /* other CNS 11643 planes */
                    ఒకవేళ(myData2022->సంస్కరణ==0) {
                        *err = U_UNSUPPORTED_ESCAPE_SEQUENCE;
                    } లేకపోతే {
                        myData2022->గమ్యం_U2022_State.cs[3] = static_cast<int8_t>(తాత్కాలిక_State);
                    }
                    ఆపు;
                }
            }
            ఆపు;
        స్థితి ISO_2022_KR:
            ఒకవేళ(స్థానభ్రంశం==0x30){
                /* nothing to be done, just accept this one escape sequence */
            } లేకపోతే {
                *err = U_UNSUPPORTED_ESCAPE_SEQUENCE;
            }
            ఆపు;
#అయితే_ముగింపు  // !UCONFIG_ONLY_HTML_CONVERSION

        అప్రమేయ:
            *err = U_ILLEGAL_ESCAPE_SEQUENCE;
            ఆపు;
        }
    }
    ఒకవేళ(U_SUCCESS(*err)) {
        _this->గమ్యం_ULength = 0;
    } లేకపోతే ఒకవేళ(*err==U_ILLEGAL_ESCAPE_SEQUENCE) {
        ఒకవేళ(_this->గమ్యం_ULength>1) {
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
            int8_t వెనుక_బహిర్గతం_దూరం=_this->గమ్యం_ULength-1;
            int8_t బైట్లు_From_This_బఫరు=_this->గమ్యం_ULength-initial_గమ్యం_ULength;
            ఒకవేళ(వెనుక_బహిర్గతం_దూరం<=బైట్లు_From_This_బఫరు) {
                /* same as initialToULength<=1 */
                *ఆధారం-=వెనుక_బహిర్గతం_దూరం;
            } లేకపోతే {
                /* Back out bytes from the previous buffer: Need to replay them. */
                _this->pre_గమ్యం_ULength = static_cast<int8_t>(బైట్లు_From_This_బఫరు - వెనుక_బహిర్గతం_దూరం);
                /* same as -(initialToULength-1) */
                /* preToULength is negative! */
                uprv_memcpy(_this->pre_గమ్యం_U, _this->గమ్యం_UBytes+1, -_this->pre_గమ్యం_ULength);
                *ఆధారం-=బైట్లు_From_This_బఫరు;
            }
            _this->గమ్యం_ULength=1;
        }
    } లేకపోతే ఒకవేళ(*err==U_UNSUPPORTED_ESCAPE_SEQUENCE) {
        _this->గమ్యం_UCallback_Reason = UCNV_UNASSIGNED;
    }
}

#ఒకవేళ !UCONFIG_ONLY_HTML_CONVERSION
/*Checks the characters of the buffer against valid 2022 escape sequences
*if the match we return a pointer to the initial start of the sequence otherwise
*we return sourceLimit
*/
/*for 2022 looks ahead in the stream
 *to determine the longest possible convertible
 *data stream
 */
స్థిర అంతఃవరుస స్థిరరాశి అక్షరం*
get_చివర_యొక్క_బఫరు_2022(స్థిరరాశి అక్షరం** ఆధారం,
                   స్థిరరాశి అక్షరం* ఆధారం_హద్దు,
                   UBool /*flush*/){

    స్థిరరాశి అక్షరం* my_ఆధారం = *ఆధారం;

#నిర్వచితమైతే U_ENABLE_GENERIC_ISO_2022
    ఒకవేళ (*ఆధారం >= ఆధారం_హద్దు)
        తిరిగి ఆధారం_హద్దు;

    చేయి{

        ఒకవేళ (*my_ఆధారం == ESC_2022){
            int8_t i;
            int32_t కీలకం = 0;
            int32_t స్థానభ్రంశం;
            UCNV_పట్టిక_స్థితులు_2022 value = VALID_NON_TERMINAL_2022;

            /* Kludge: I could not
            * figure out the reason for validating an escape sequence
            * twice - once here and once in changeState_2022().
            * is it possible to have an ESC character in a ISO2022
            * byte stream which is valid in a code page? Is it legal?
            */
            కొరకు (i=0;
            (my_ఆధారం+i < ఆధారం_హద్దు)&&(value == VALID_NON_TERMINAL_2022);
            i++) {
                value =  get_కీలకం_2022(*(my_ఆధారం+i), &కీలకం, &స్థానభ్రంశం);
            }
            ఒకవేళ (value > 0 || *my_ఆధారం==ESC_2022)
                తిరిగి my_ఆధారం;

            ఒకవేళ ((value == VALID_NON_TERMINAL_2022)&&(!flush) )
                తిరిగి ఆధారం_హద్దు;
        }
    }అయితే (++my_ఆధారం < ఆధారం_హద్దు);

    తిరిగి ఆధారం_హద్దు;
#లేకపోతే
    అయితే(my_ఆధారం < ఆధారం_హద్దు && *my_ఆధారం != ESC_2022) {
        ++my_ఆధారం;
    }
    తిరిగి my_ఆధారం;
#అయితే_ముగింపు
}
#అయితే_ముగింపు

/* This inline function replicates code in _MBCSFromUChar32() function in ucnvmbcs.c
 * any future change in _MBCSFromUChar32() function should be reflected here.
 * @return number of bytes in *value; negative number if fallback; 0 if no mapping
 */
స్థిర అంతఃవరుస int32_t
MBCS_FROM_UCHAR32_ISO2022(UConverter_Shared_దత్తాంశం* shared_దత్తాంశం,
                                         UChar32 c,
                                         uint32_t* value,
                                         UBool useFallback,
                                         పూర్ణాంకం outputType)
{
    స్థిరరాశి int32_t *cx;
    స్థిరరాశి uint16_t *పట్టిక;
    uint32_t stage2_నమోదు;
    uint32_t my_విలువ;
    int32_t length;
    స్థిరరాశి uint8_t *p;
    /*
     * TODO(markus): Use and require new, faster MBCS conversion table structures.
     * Use internal version of ucnv_open() that verifies that the new structures are available,
     * else U_INTERNAL_PROGRAM_ERROR.
     */
    /* BMP-only codepages are stored without stage 1 entries for supplementary code points */
    ఒకవేళ(c<0x10000 || (shared_దత్తాంశం->mbcs.unicode_ముసుగు&UCNV_HAS_SUPPLEMENTARY)) {
        పట్టిక=shared_దత్తాంశం->mbcs.from_Unicode_పట్టిక;
        stage2_నమోదు=MBCS_STAGE_2_FROM_U(పట్టిక, c);
        /* get the bytes and the length for the output */
        ఒకవేళ(outputType==MBCS_OUTPUT_2){
            my_విలువ=MBCS_విలువ_2_FROM_STAGE_2(shared_దత్తాంశం->mbcs.from_Unicode_బైట్లు, stage2_నమోదు, c);
            ఒకవేళ(my_విలువ<=0xff) {
                length=1;
            } లేకపోతే {
                length=2;
            }
        } లేకపోతే /* outputType==MBCS_OUTPUT_3 */ {
            p=MBCS_సూచిక_3_FROM_STAGE_2(shared_దత్తాంశం->mbcs.from_Unicode_బైట్లు, stage2_నమోదు, c);
            my_విలువ = (static_cast<uint32_t>(*p) << 16) | (static_cast<uint32_t>(p[1]) << 8) | p[2];
            ఒకవేళ(my_విలువ<=0xff) {
                length=1;
            } లేకపోతే ఒకవేళ(my_విలువ<=0xffff) {
                length=2;
            } లేకపోతే {
                length=3;
            }
        }
        /* is this code point assigned, or do we use fallbacks? */
        ఒకవేళ((stage2_నమోదు&(1<<(16+(c&0xf))))!=0) {
            /* assigned */
            *value=my_విలువ;
            తిరిగి length;
        } లేకపోతే ఒకవేళ(FROM_U_USE_FALLBACK(useFallback, c) && my_విలువ!=0) {
            /*
             * We allow a 0 byte output if the "assigned" bit is set for this entry.
             * There is no way with this data structure for fallback output
             * to be a zero byte.
             */
            *value=my_విలువ;
            తిరిగి -length;
        }
    }

    cx=shared_దత్తాంశం->mbcs.extIndexes;
    ఒకవేళ(cx!=శూన్యసూచిక) {
        తిరిగి ucnv_ext_Simple_సరిపోలిక_From_U(cx, c, value, useFallback);
    }

    /* unassigned */
    తిరిగి 0;
}

/* This inline function replicates code in _MBCSSingleFromUChar32() function in ucnvmbcs.c
 * any future change in _MBCSSingleFromUChar32() function should be reflected here.
 * @param retval pointer to output byte
 * @return 1 roundtrip byte  0 no mapping  -1 fallback byte
 */
స్థిర అంతఃవరుస int32_t
MBCS_SINGLE_FROM_UCHAR32(UConverter_Shared_దత్తాంశం* shared_దత్తాంశం,
                                       UChar32 c,
                                       uint32_t* retval,
                                       UBool useFallback)
{
    స్థిరరాశి uint16_t *పట్టిక;
    int32_t value;
    /* BMP-only codepages are stored without stage 1 entries for supplementary code points */
    ఒకవేళ(c>=0x10000 && !(shared_దత్తాంశం->mbcs.unicode_ముసుగు&UCNV_HAS_SUPPLEMENTARY)) {
        తిరిగి 0;
    }
    /* convert the Unicode code point in c into codepage bytes (same as in _MBCSFromUnicodeWithOffsets) */
    పట్టిక=shared_దత్తాంశం->mbcs.from_Unicode_పట్టిక;
    /* get the byte for the output */
    value=MBCS_SINGLE_ఫలితం_FROM_U(పట్టిక, (uint16_t *)shared_దత్తాంశం->mbcs.from_Unicode_బైట్లు, c);
    /* is this code point assigned, or do we use fallbacks? */
    *retval = static_cast<uint32_t>(value & 0xff);
    ఒకవేళ(value>=0xf00) {
        తిరిగి 1;  /* roundtrip */
    } లేకపోతే ఒకవేళ(useFallback ? value>=0x800 : value>=0xc00) {
        తిరిగి -1;  /* fallback taken */
    } లేకపోతే {
        తిరిగి 0;  /* no mapping */
    }
}

/*
 * Check that the result is a 2-byte value with each byte in the range A1..FE
 * (strict EUC DBCS) before accepting it and subtracting 0x80 from each byte
 * to move it to the ISO 2022 range 21..7E.
 * Return 0 if out of range.
 */
స్థిర అంతఃవరుస uint32_t
_2022FromGR94DBCS(uint32_t value) {
    ఒకవేళ (static_cast<uint16_t>(value - 0xa1a1) <= (0xfefe - 0xa1a1) &&
        static_cast<uint8_t>(value - 0xa1) <= (0xfe - 0xa1)
    ) {
        తిరిగి value - 0x8080;  /* shift down to 21..7e byte range */
    } లేకపోతే {
        తిరిగి 0;  /* not valid for ISO 2022 */
    }
}

#ఒకవేళ 0 /* 5691: Call sites now check for validity. They can just += 0x8080 after that. */
/*
 * This method does the reverse of _2022FromGR94DBCS(). Given the 2022 code point, it returns the
 * 2 byte value that is in the range A1..FE for each byte. Otherwise it returns the 2022 code point
 * unchanged. 
 */
స్థిర అంతఃవరుస uint32_t
_2022ToGR94DBCS(uint32_t value) {
    uint32_t return_విలువ = value + 0x8080;
    ఒకవేళ( (uint16_t)(return_విలువ - 0xa1a1) <= (0xfefe - 0xa1a1) &&
        (uint8_t)(return_విలువ - 0xa1) <= (0xfe - 0xa1)) {
        తిరిగి return_విలువ;
    } లేకపోతే {
        తిరిగి value;
    }
}
#అయితే_ముగింపు

#నిర్వచితమైతే U_ENABLE_GENERIC_ISO_2022

/**********************************************************************************
*  ISO-2022 Converter
*
*
*/

స్థిర శూన్య U_CALLCONV
T_UConverter_గమ్యం_Unicode_ISO_2022_OFFSETS_LOGIC(UConverter_గమ్యం_Unicode_వాదనలు* వాదనలు_2,
                                                           UError_సంకేతం* err){
    స్థిరరాశి అక్షరం* my_ఆధారం_హద్దు, *real_ఆధారం_హద్దు;
    స్థిరరాశి అక్షరం* ఆధారం_ప్రారంభం;
    స్థిరరాశి char16_t* my_లక్ష్యం_ప్రారంభం;
    UConverter* saveThis;
    UConverter_దత్తాంశం_ISO2022* my_దత్తాంశం;
    int8_t length;

    saveThis = వాదనలు_2->converter;
    my_దత్తాంశం=((UConverter_దత్తాంశం_ISO2022*)(saveThis->extraInfo));

    real_ఆధారం_హద్దు = వాదనలు_2->ఆధారం_హద్దు;
    అయితే (వాదనలు_2->ఆధారం < real_ఆధారం_హద్దు) {
        ఒకవేళ(my_దత్తాంశం->కీలకం == 0) { /* are we in the middle of an escape sequence? */
            /*Find the end of the buffer e.g : Next Escape Seq | end of Buffer*/
            my_ఆధారం_హద్దు = get_చివర_యొక్క_బఫరు_2022(&(వాదనలు_2->ఆధారం), real_ఆధారం_హద్దు, వాదనలు_2->flush);

            ఒకవేళ(వాదనలు_2->ఆధారం < my_ఆధారం_హద్దు) {
                ఒకవేళ(my_దత్తాంశం->currentConverter==శూన్యసూచిక) {
                    my_దత్తాంశం->currentConverter = ucnv_open("ASCII",err);
                    ఒకవేళ(U_FAILURE(*err)){
                        తిరిగి;
                    }

                    my_దత్తాంశం->currentConverter->fromCharErrorBehaviour = UCNV_గమ్యం_U_CALLBACK_STOP;
                    saveThis->బహుళకం = UCNV_SO;
                }

                /* convert to before the ESC or until the end of the buffer */
                my_దత్తాంశం->is_మొదటి_బఫరు=అబద్ధం;
                ఆధారం_ప్రారంభం = వాదనలు_2->ఆధారం;
                my_లక్ష్యం_ప్రారంభం = వాదనలు_2->లక్ష్యం;
                వాదనలు_2->converter = my_దత్తాంశం->currentConverter;
                ucnv_గమ్యం_Unicode(వాదనలు_2->converter,
                    &వాదనలు_2->లక్ష్యం,
                    వాదనలు_2->లక్ష్యం_హద్దు,
                    &వాదనలు_2->ఆధారం,
                    my_ఆధారం_హద్దు,
                    వాదనలు_2->offsets,
                    (UBool)(వాదనలు_2->flush && my_ఆధారం_హద్దు == real_ఆధారం_హద్దు),
                    err);
                వాదనలు_2->converter = saveThis;

                ఒకవేళ (*err == U_బఫరు_అధికప్రవాహం_ERROR) {
                    /* move the overflow buffer */
                    length = saveThis->UChar_Error_బఫరు_నిడివి = my_దత్తాంశం->currentConverter->UChar_Error_బఫరు_నిడివి;
                    my_దత్తాంశం->currentConverter->UChar_Error_బఫరు_నిడివి = 0;
                    ఒకవేళ(length > 0) {
                        uprv_memcpy(saveThis->UChar_Error_బఫరు,
                                    my_దత్తాంశం->currentConverter->UChar_Error_బఫరు,
                                    length*U_SIZEOF_UCHAR);
                    }
                    తిరిగి;
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
                ఒకవేళ (U_FAILURE(*err) ||
                    (వాదనలు_2->ఆధారం == real_ఆధారం_హద్దు) ||
                    (వాదనలు_2->offsets != శూన్యసూచిక && (వాదనలు_2->లక్ష్యం != my_లక్ష్యం_ప్రారంభం || వాదనలు_2->ఆధారం != ఆధారం_ప్రారంభం) ||
                    (my_ఆధారం_హద్దు < real_ఆధారం_హద్దు && my_దత్తాంశం->currentConverter->గమ్యం_ULength > 0))
                ) {
                    /* copy partial or error input for truncated detection and error handling */
                    ఒకవేళ(U_FAILURE(*err)) {
                        length = saveThis->invalid_Char_నిడివి = my_దత్తాంశం->currentConverter->invalid_Char_నిడివి;
                        ఒకవేళ(length > 0) {
                            uprv_memcpy(saveThis->invalid_Char_బఫరు, my_దత్తాంశం->currentConverter->invalid_Char_బఫరు, length);
                        }
                    } లేకపోతే {
                        length = saveThis->గమ్యం_ULength = my_దత్తాంశం->currentConverter->గమ్యం_ULength;
                        ఒకవేళ(length > 0) {
                            uprv_memcpy(saveThis->గమ్యం_UBytes, my_దత్తాంశం->currentConverter->గమ్యం_UBytes, length);
                            ఒకవేళ(వాదనలు_2->ఆధారం < my_ఆధారం_హద్దు) {
                                *err = U_TRUNCATED_CHAR_దొరికింది; /* truncated input before ESC */
                            }
                        }
                    }
                    తిరిగి;
                }
            }
        }

        ఆధారం_ప్రారంభం = వాదనలు_2->ఆధారం;
        changeState_2022(వాదనలు_2->converter,
               &(వాదనలు_2->ఆధారం),
               real_ఆధారం_హద్దు,
               ISO_2022,
               err);
        ఒకవేళ (U_FAILURE(*err) || (వాదనలు_2->ఆధారం != ఆధారం_ప్రారంభం && వాదనలు_2->offsets != శూన్యసూచిక)) {
            /* let the ucnv.c code update its current offset */
            తిరిగి;
        }
    }
}

#అయితే_ముగింపు

/*
 * To Unicode Callback helper function
 */
స్థిర శూన్య
గమ్యం_Unicode_Callback(UConverter *cnv,
                  స్థిరరాశి uint32_t ఆధారం_Char, స్థిరరాశి uint32_t లక్ష్యం_Uni_Char,
                  UError_సంకేతం* err){
    ఒకవేళ(ఆధారం_Char>0xff){
        cnv->గమ్యం_UBytes[0] = static_cast<uint8_t>(ఆధారం_Char >> 8);
        cnv->గమ్యం_UBytes[1] = static_cast<uint8_t>(ఆధారం_Char);
        cnv->గమ్యం_ULength = 2;
    }
    లేకపోతే{
        cnv->గమ్యం_UBytes[0] = static_cast<అక్షరం>(ఆధారం_Char);
        cnv->గమ్యం_ULength = 1;
    }

    ఒకవేళ(లక్ష్యం_Uni_Char == (missingCharMarker-1/*0xfffe*/)){
        *err = U_INVALID_CHAR_దొరికింది;
    }
    లేకపోతే{
        *err = U_ILLEGAL_CHAR_దొరికింది;
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
స్థిర స్థిరరాశి StateEnum jpCharsetPref[]={
    ASCII,
    JISX201,
    ISO8859_1,
    JISX208,
    ISO8859_7,
    JISX212,
    GB2312,
    KSC5601,
    HWKANA_7_బిట్టు
};

/*
 * The escape sequences must be in order of the enum constants like JISX201  = 3,
 * not in order of jpCharsetPref[]!
 */
స్థిర స్థిరరాశి అక్షరం escSeqChars[][6] ={
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
స్థిర  స్థిరరాశి int8_t escSeqCharsLen[] ={
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
స్థిర అంతఃవరుస uint32_t
jisx201_గమ్యం_U(uint32_t value) {
    ఒకవేళ(value < 0x5c) {
        తిరిగి value;
    } లేకపోతే ఒకవేళ(value == 0x5c) {
        తిరిగి 0xa5;
    } లేకపోతే ఒకవేళ(value == 0x7e) {
        తిరిగి 0x203e;
    } లేకపోతే /* value <= 0x7f */ {
        తిరిగి value;
    }
}

/* Map Unicode to 00..7F according to JIS X 0201. Return U+FFFE if unmappable. */
స్థిర అంతఃవరుస uint32_t
jisx201FromU(uint32_t value) {
    ఒకవేళ(value<=0x7f) {
        ఒకవేళ(value!=0x5c && value!=0x7e) {
            తిరిగి value;
        }
    } లేకపోతే ఒకవేళ(value==0xa5) {
        తిరిగి 0x5c;
    } లేకపోతే ఒకవేళ(value==0x203e) {
        తిరిగి 0x7e;
    }
    తిరిగి 0xfffe;
}

/*
 * Take a valid Shift-JIS byte pair, check that it is in the range corresponding
 * to JIS X 0208, and convert it to a pair of 21..7E bytes.
 * Return 0 if the byte pair is out of range.
 */
స్థిర అంతఃవరుస uint32_t
_2022FromSJIS(uint32_t value) {
    uint8_t trail;

    ఒకవేళ(value > 0xEFFC) {
        తిరిగి 0;  /* beyond JIS X 0208 */
    }

    trail = static_cast<uint8_t>(value);

    value &= 0xff00;  /* lead byte */
    ఒకవేళ(value <= 0x9f00) {
        value -= 0x7000;
    } లేకపోతే /* 0xe000 <= value <= 0xef00 */ {
        value -= 0xb000;
    }
    value <<= 1;

    ఒకవేళ(trail <= 0x9e) {
        value -= 0x100;
        ఒకవేళ(trail <= 0x7e) {
            value |= trail - 0x1f;
        } లేకపోతే {
            value |= trail - 0x20;
        }
    } లేకపోతే /* trail <= 0xfc */ {
        value |= trail - 0x7e;
    }
    తిరిగి value;
}

/*
 * Convert a pair of JIS X 0208 21..7E bytes to Shift-JIS.
 * If either byte is outside 21..7E make sure that the result is not valid
 * for Shift-JIS so that the converter catches it.
 * Some invalid byte values already turn into equally invalid Shift-JIS
 * byte values and need not be tested explicitly.
 */
స్థిర అంతఃవరుస శూన్య
_2022ToSJIS(uint8_t c1, uint8_t c2, అక్షరం బైట్లు[2]) {
    ఒకవేళ(c1&1) {
        ++c1;
        ఒకవేళ(c2 <= 0x5f) {
            c2 += 0x1f;
        } లేకపోతే ఒకవేళ(c2 <= 0x7e) {
            c2 += 0x20;
        } లేకపోతే {
            c2 = 0;  /* invalid */
        }
    } లేకపోతే {
        ఒకవేళ (static_cast<uint8_t>(c2 - 0x21) <= ((0x7e) - 0x21)) {
            c2 += 0x7e;
        } లేకపోతే {
            c2 = 0;  /* invalid */
        }
    }
    c1 >>= 1;
    ఒకవేళ(c1 <= 0x2f) {
        c1 += 0x70;
    } లేకపోతే ఒకవేళ(c1 <= 0x3f) {
        c1 += 0xb0;
    } లేకపోతే {
        c1 = 0;  /* invalid */
    }
    బైట్లు[0] = static_cast<అక్షరం>(c1);
    బైట్లు[1] = static_cast<అక్షరం>(c2);
}

/*
 * JIS X 0208 has fallbacks from Unicode half-width Katakana to full-width (DBCS)
 * Katakana.
 * Now that we use a Shift-JIS table for JIS X 0208 we need to hardcode these fallbacks
 * because Shift-JIS roundtrips half-width Katakana to single bytes.
 * These were the only fallbacks in ICU's jisx-208.ucm file.
 */
స్థిర స్థిరరాశి uint16_t hwkana_fb[HWKANA_చివర - HWKANA_ప్రారంభం + 1] = {
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

స్థిర శూన్య U_CALLCONV
UConverter_fromUnicode_ISO_2022_JP_OFFSETS_LOGIC(UConverter_From_Unicode_వాదనలు* వాదనలు_2, UError_సంకేతం* err) {
    UConverter *cnv = వాదనలు_2->converter;
    UConverter_దత్తాంశం_ISO2022 *converter_దత్తాంశం;
    ISO2022State *pFromU2022State;
    uint8_t* లక్ష్యం = reinterpret_cast<uint8_t*>(వాదనలు_2->లక్ష్యం);
    స్థిరరాశి uint8_t* లక్ష్యం_హద్దు = reinterpret_cast<స్థిరరాశి uint8_t*>(వాదనలు_2->లక్ష్యం_హద్దు);
    స్థిరరాశి char16_t* ఆధారం = వాదనలు_2->ఆధారం;
    స్థిరరాశి char16_t* ఆధారం_హద్దు = వాదనలు_2->ఆధారం_హద్దు;
    int32_t* offsets = వాదనలు_2->offsets;
    UChar32 ఆధారం_Char;
    అక్షరం బఫరు[8];
    int32_t len, బహిర్గతం_Len;
    int8_t choices[10];
    int32_t choice_లెక్క;
    uint32_t లక్ష్యం_విలువ = 0;
    UBool useFallback;

    int32_t i;
    int8_t cs, g;

    /* set up the state */
    converter_దత్తాంశం = static_cast<UConverter_దత్తాంశం_ISO2022*>(cnv->extraInfo);
    pFromU2022State   = &converter_దత్తాంశం->fromU2022State;

    choice_లెక్క = 0;

    /* check if the last codepoint of previous buffer was a lead surrogate*/
    ఒకవేళ((ఆధారం_Char = cnv->fromUChar32)!=0 && లక్ష్యం< లక్ష్యం_హద్దు) {
        వెళ్ళు getTrail;
    }

    అయితే(ఆధారం < ఆధారం_హద్దు) {
        ఒకవేళ(లక్ష్యం < లక్ష్యం_హద్దు) {

            ఆధారం_Char  = *(ఆధారం++);
            /*check if the char is a First surrogate*/
            ఒకవేళ(U16_IS_SURROGATE(ఆధారం_Char)) {
                ఒకవేళ(U16_IS_SURROGATE_LEAD(ఆధారం_Char)) {
getTrail:
                    /*look ahead to find the trail surrogate*/
                    ఒకవేళ(ఆధారం < ఆధారం_హద్దు) {
                        /* test the following code unit */
                        char16_t trail = *ఆధారం;
                        ఒకవేళ(U16_IS_TRAIL(trail)) {
                            ఆధారం++;
                            ఆధారం_Char=U16_GET_SUPPLEMENTARY(ఆధారం_Char, trail);
                            cnv->fromUChar32=0x00;
                            /* convert this supplementary code point */
                            /* exit this condition tree */
                        } లేకపోతే {
                            /* this is an unmatched lead code unit (1st surrogate) */
                            /* callback(illegal) */
                            *err=U_ILLEGAL_CHAR_దొరికింది;
                            cnv->fromUChar32=ఆధారం_Char;
                            ఆపు;
                        }
                    } లేకపోతే {
                        /* no more input */
                        cnv->fromUChar32=ఆధారం_Char;
                        ఆపు;
                    }
                } లేకపోతే {
                    /* this is an unmatched trail code unit (2nd surrogate) */
                    /* callback(illegal) */
                    *err=U_ILLEGAL_CHAR_దొరికింది;
                    cnv->fromUChar32=ఆధారం_Char;
                    ఆపు;
                }
            }

            /* do not convert SO/SI/ESC */
            ఒకవేళ(IS_2022_CONTROL(ఆధారం_Char)) {
                /* callback(illegal) */
                *err=U_ILLEGAL_CHAR_దొరికింది;
                cnv->fromUChar32=ఆధారం_Char;
                ఆపు;
            }

            /* do the conversion */

            ఒకవేళ(choice_లెక్క == 0) {
                uint16_t csm;

                /*
                 * The csm variable keeps track of which charsets are allowed
                 * and not used yet while building the choices[].
                 */
                csm = jpCharsetMasks[converter_దత్తాంశం->సంస్కరణ];
                choice_లెక్క = 0;

                /* JIS7/8: try single-byte half-width Katakana before JISX208 */
                ఒకవేళ(converter_దత్తాంశం->సంస్కరణ == 3 || converter_దత్తాంశం->సంస్కరణ == 4) {
                    choices[choice_లెక్క++] = static_cast<int8_t>(HWKANA_7_బిట్టు);
                }
                /* Do not try single-byte half-width Katakana for other versions. */
                csm &= ~CSM(HWKANA_7_బిట్టు);

                /* try the current G0 charset */
                choices[choice_లెక్క++] = cs = pFromU2022State->cs[0];
                csm &= ~CSM(cs);

                /* try the current G2 charset */
                ఒకవేళ((cs = pFromU2022State->cs[2]) != 0) {
                    choices[choice_లెక్క++] = cs;
                    csm &= ~CSM(cs);
                }

                /* try all the other possible charsets */
                కొరకు(i = 0; i < UPRV_LENGTHOF(jpCharsetPref); ++i) {
                    cs = static_cast<int8_t>(jpCharsetPref[i]);
                    ఒకవేళ(CSM(cs) & csm) {
                        choices[choice_లెక్క++] = cs;
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

            కొరకు(i = 0; i < choice_లెక్క && len <= 0; ++i) {
                uint32_t value;
                int32_t len2;
                int8_t cs0 = choices[i];
                ఎంపిక(cs0) {
                స్థితి ASCII:
                    ఒకవేళ(ఆధారం_Char <= 0x7f) {
                        లక్ష్యం_విలువ = static_cast<uint32_t>(ఆధారం_Char);
                        len = 1;
                        cs = cs0;
                        g = 0;
                    }
                    ఆపు;
                స్థితి ISO8859_1:
                    ఒకవేళ(GR96_ప్రారంభం <= ఆధారం_Char && ఆధారం_Char <= GR96_చివర) {
                        లక్ష్యం_విలువ = static_cast<uint32_t>(ఆధారం_Char) - 0x80;
                        len = 1;
                        cs = cs0;
                        g = 2;
                    }
                    ఆపు;
                స్థితి HWKANA_7_బిట్టు:
                    ఒకవేళ (static_cast<uint32_t>(ఆధారం_Char - HWKANA_ప్రారంభం) <= (HWKANA_చివర - HWKANA_ప్రారంభం)) {
                        ఒకవేళ(converter_దత్తాంశం->సంస్కరణ==3) {
                            /* JIS7: use G1 (SO) */
                            /* Shift U+FF61..U+FF9F to bytes 21..5F. */
                            లక్ష్యం_విలువ = static_cast<uint32_t>(ఆధారం_Char - (HWKANA_ప్రారంభం - 0x21));
                            len = 1;
                            pFromU2022State->cs[1] = cs = cs0; /* do not output an escape sequence */
                            g = 1;
                        } లేకపోతే ఒకవేళ(converter_దత్తాంశం->సంస్కరణ==4) {
                            /* JIS8: use 8-bit bytes with any single-byte charset, see escape sequence output below */
                            /* Shift U+FF61..U+FF9F to bytes A1..DF. */
                            లక్ష్యం_విలువ = static_cast<uint32_t>(ఆధారం_Char - (HWKANA_ప్రారంభం - 0xa1));
                            len = 1;

                            cs = pFromU2022State->cs[0];
                            ఒకవేళ(IS_JP_DBCS(cs)) {
                                /* switch from a DBCS charset to JISX201 */
                                cs = static_cast<int8_t>(JISX201);
                            }
                            /* else stay in the current G0 charset */
                            g = 0;
                        }
                        /* else do not use HWKANA_7BIT with other versions */
                    }
                    ఆపు;
                స్థితి JISX201:
                    /* G0 SBCS */
                    value = jisx201FromU(ఆధారం_Char);
                    ఒకవేళ(value <= 0x7f) {
                        లక్ష్యం_విలువ = value;
                        len = 1;
                        cs = cs0;
                        g = 0;
                        useFallback = అబద్ధం;
                    }
                    ఆపు;
                స్థితి JISX208:
                    /* G0 DBCS from Shift-JIS table */
                    len2 = MBCS_FROM_UCHAR32_ISO2022(
                                converter_దత్తాంశం->myConverterArray[cs0],
                                ఆధారం_Char, &value,
                                useFallback, MBCS_OUTPUT_2);
                    ఒకవేళ(len2 == 2 || (len2 == -2 && len == 0)) {  /* only accept DBCS: abs(len)==2 */
                        value = _2022FromSJIS(value);
                        ఒకవేళ(value != 0) {
                            లక్ష్యం_విలువ = value;
                            len = len2;
                            cs = cs0;
                            g = 0;
                            useFallback = అబద్ధం;
                        }
                    } లేకపోతే ఒకవేళ(len == 0 && useFallback &&
                              static_cast<uint32_t>(ఆధారం_Char - HWKANA_ప్రారంభం) <= (HWKANA_చివర - HWKANA_ప్రారంభం)) {
                        లక్ష్యం_విలువ = hwkana_fb[ఆధారం_Char - HWKANA_ప్రారంభం];
                        len = -2;
                        cs = cs0;
                        g = 0;
                        useFallback = అబద్ధం;
                    }
                    ఆపు;
                స్థితి ISO8859_7:
                    /* G0 SBCS forced to 7-bit output */
                    len2 = MBCS_SINGLE_FROM_UCHAR32(
                                converter_దత్తాంశం->myConverterArray[cs0],
                                ఆధారం_Char, &value,
                                useFallback);
                    ఒకవేళ(len2 != 0 && !(len2 < 0 && len != 0) && GR96_ప్రారంభం <= value && value <= GR96_చివర) {
                        లక్ష్యం_విలువ = value - 0x80;
                        len = len2;
                        cs = cs0;
                        g = 2;
                        useFallback = అబద్ధం;
                    }
                    ఆపు;
                అప్రమేయ:
                    /* G0 DBCS */
                    len2 = MBCS_FROM_UCHAR32_ISO2022(
                                converter_దత్తాంశం->myConverterArray[cs0],
                                ఆధారం_Char, &value,
                                useFallback, MBCS_OUTPUT_2);
                    ఒకవేళ(len2 == 2 || (len2 == -2 && len == 0)) {  /* only accept DBCS: abs(len)==2 */
                        ఒకవేళ(cs0 == KSC5601) {
                            /*
                             * Check for valid bytes for the encoding scheme.
                             * This is necessary because the sub-converter (windows-949)
                             * has a broader encoding scheme than is valid for 2022.
                             */
                            value = _2022FromGR94DBCS(value);
                            ఒకవేళ(value == 0) {
                                ఆపు;
                            }
                        }
                        లక్ష్యం_విలువ = value;
                        len = len2;
                        cs = cs0;
                        g = 0;
                        useFallback = అబద్ధం;
                    }
                    ఆపు;
                }
            }

            ఒకవేళ(len != 0) {
                ఒకవేళ(len < 0) {
                    len = -len;  /* fallback */
                }
                బహిర్గతం_Len = 0; /* count output bytes */

                /* write SI if necessary (only for JIS7) */
                ఒకవేళ(pFromU2022State->g == 1 && g == 0) {
                    బఫరు[బహిర్గతం_Len++] = UCNV_SI;
                    pFromU2022State->g = 0;
                }

                /* write the designation sequence if necessary */
                ఒకవేళ(cs != pFromU2022State->cs[g]) {
                    int32_t escLen = escSeqCharsLen[cs];
                    uprv_memcpy(బఫరు + బహిర్గతం_Len, escSeqChars[cs], escLen);
                    బహిర్గతం_Len += escLen;
                    pFromU2022State->cs[g] = cs;

                    /* invalidate the choices[] */
                    choice_లెక్క = 0;
                }

                /* write the shift sequence if necessary */
                ఒకవేళ(g != pFromU2022State->g) {
                    ఎంపిక(g) {
                    /* case 0 handled before writing escapes */
                    స్థితి 1:
                        బఫరు[బహిర్గతం_Len++] = UCNV_SO;
                        pFromU2022State->g = 1;
                        ఆపు;
                    అప్రమేయ: /* case 2 */
                        బఫరు[బహిర్గతం_Len++] = 0x1b;
                        బఫరు[బహిర్గతం_Len++] = 0x4e;
                        ఆపు;
                    /* no case 3: no SS3 in ISO-2022-JP-x */
                    }
                }

                /* write the output bytes */
                ఒకవేళ(len == 1) {
                    బఫరు[బహిర్గతం_Len++] = static_cast<అక్షరం>(లక్ష్యం_విలువ);
                } లేకపోతే /* len == 2 */ {
                    బఫరు[బహిర్గతం_Len++] = static_cast<అక్షరం>(లక్ష్యం_విలువ >> 8);
                    బఫరు[బహిర్గతం_Len++] = static_cast<అక్షరం>(లక్ష్యం_విలువ);
                }
            } లేకపోతే {
                /*
                 * if we cannot find the character after checking all codepages
                 * then this is an error
                 */
                *err = U_INVALID_CHAR_దొరికింది;
                cnv->fromUChar32=ఆధారం_Char;
                ఆపు;
            }

            ఒకవేళ(ఆధారం_Char == CR || ఆధారం_Char == LF) {
                /* reset the G2 state at the end of a line (conversion got us into ASCII or JISX201 already) */
                pFromU2022State->cs[2] = 0;
                choice_లెక్క = 0;
            }

            /* output outLen>0 bytes in buffer[] */
            ఒకవేళ(బహిర్గతం_Len == 1) {
                *లక్ష్యం++ = బఫరు[0];
                ఒకవేళ(offsets) {
                    *offsets++ = static_cast<int32_t>(ఆధారం - వాదనలు_2->ఆధారం - 1); /* -1: known to be ASCII */
                }
            } లేకపోతే ఒకవేళ(బహిర్గతం_Len == 2 && (లక్ష్యం + 2) <= లక్ష్యం_హద్దు) {
                *లక్ష్యం++ = బఫరు[0];
                *లక్ష్యం++ = బఫరు[1];
                ఒకవేళ(offsets) {
                    int32_t ఆధారం_సూచీ = static_cast<int32_t>(ఆధారం - వాదనలు_2->ఆధారం - U16_నిడివి(ఆధారం_Char));
                    *offsets++ = ఆధారం_సూచీ;
                    *offsets++ = ఆధారం_సూచీ;
                }
            } లేకపోతే {
                fromUWriteUInt8(
                    cnv,
                    బఫరు, బహిర్గతం_Len,
                    &లక్ష్యం, reinterpret_cast<స్థిరరాశి అక్షరం*>(లక్ష్యం_హద్దు),
                    &offsets, static_cast<int32_t>(ఆధారం - వాదనలు_2->ఆధారం - U16_నిడివి(ఆధారం_Char)),
                    err);
                ఒకవేళ(U_FAILURE(*err)) {
                    ఆపు;
                }
            }
        } /* end if(myTargetIndex<myTargetLength) */
        లేకపోతే{
            *err =U_బఫరు_అధికప్రవాహం_ERROR;
            ఆపు;
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
    ఒకవేళ( U_SUCCESS(*err) &&
        (pFromU2022State->g!=0 || pFromU2022State->cs[0]!=ASCII) &&
        వాదనలు_2->flush && ఆధారం>=ఆధారం_హద్దు && cnv->fromUChar32==0
    ) {
        int32_t ఆధారం_సూచీ;

        బహిర్గతం_Len = 0;

        ఒకవేళ(pFromU2022State->g != 0) {
            బఫరు[బహిర్గతం_Len++] = UCNV_SI;
            pFromU2022State->g = 0;
        }

        ఒకవేళ(pFromU2022State->cs[0] != ASCII) {
            int32_t escLen = escSeqCharsLen[ASCII];
            uprv_memcpy(బఫరు + బహిర్గతం_Len, escSeqChars[ASCII], escLen);
            బహిర్గతం_Len += escLen;
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
        ఆధారం_సూచీ = static_cast<int32_t>(ఆధారం - వాదనలు_2->ఆధారం);
        ఒకవేళ(ఆధారం_సూచీ>0) {
            --ఆధారం_సూచీ;
            ఒకవేళ( U16_IS_TRAIL(వాదనలు_2->ఆధారం[ఆధారం_సూచీ]) &&
                (ఆధారం_సూచీ==0 || U16_IS_LEAD(వాదనలు_2->ఆధారం[ఆధారం_సూచీ-1]))
            ) {
                --ఆధారం_సూచీ;
            }
        } లేకపోతే {
            ఆధారం_సూచీ=-1;
        }

        fromUWriteUInt8(
            cnv,
            బఫరు, బహిర్గతం_Len,
            &లక్ష్యం, reinterpret_cast<స్థిరరాశి అక్షరం*>(లక్ష్యం_హద్దు),
            &offsets, ఆధారం_సూచీ,
            err);
    }

    /*save the state and return */
    వాదనలు_2->ఆధారం = ఆధారం;
    వాదనలు_2->లక్ష్యం = reinterpret_cast<అక్షరం*>(లక్ష్యం);
}

/*************** to unicode *******************/

స్థిర శూన్య U_CALLCONV
UConverter_గమ్యం_Unicode_ISO_2022_JP_OFFSETS_LOGIC(UConverter_గమ్యం_Unicode_వాదనలు *వాదనలు_2,
                                               UError_సంకేతం* err){
    అక్షరం తాత్కాలిక_Buf[2];
    స్థిరరాశి అక్షరం* my_ఆధారం = const_cast<అక్షరం*>(వాదనలు_2->ఆధారం);
    char16_t *my_లక్ష్యం = వాదనలు_2->లక్ష్యం;
    స్థిరరాశి అక్షరం *my_ఆధారం_హద్దు = వాదనలు_2->ఆధారం_హద్దు;
    uint32_t లక్ష్యం_Uni_Char = 0x0000;
    uint32_t my_ఆధారం_Char = 0x0000;
    uint32_t tmp_ఆధారం_Char = 0x0000;
    UConverter_దత్తాంశం_ISO2022* my_దత్తాంశం;
    ISO2022State *p_గమ్యం_U2022_State;
    StateEnum cs;

    my_దత్తాంశం = static_cast<UConverter_దత్తాంశం_ISO2022*>(వాదనలు_2->converter->extraInfo);
    p_గమ్యం_U2022_State = &my_దత్తాంశం->గమ్యం_U2022_State;

    ఒకవేళ(my_దత్తాంశం->కీలకం != 0) {
        /* continue with a partial escape sequence */
        వెళ్ళు escape;
    } లేకపోతే ఒకవేళ(వాదనలు_2->converter->గమ్యం_ULength == 1 && my_ఆధారం < my_ఆధారం_హద్దు && my_లక్ష్యం < వాదనలు_2->లక్ష్యం_హద్దు) {
        /* continue with a partial double-byte character */
        my_ఆధారం_Char = వాదనలు_2->converter->గమ్యం_UBytes[0];
        వాదనలు_2->converter->గమ్యం_ULength = 0;
        cs = static_cast<StateEnum>(p_గమ్యం_U2022_State->cs[p_గమ్యం_U2022_State->g]);
        లక్ష్యం_Uni_Char = missingCharMarker;
        వెళ్ళు getTrailByte;
    }

    అయితే(my_ఆధారం < my_ఆధారం_హద్దు){

        లక్ష్యం_Uni_Char =missingCharMarker;

        ఒకవేళ(my_లక్ష్యం < వాదనలు_2->లక్ష్యం_హద్దు){

            my_ఆధారం_Char = static_cast<గుర్తులేని అక్షరం>(*my_ఆధారం++);

            ఎంపిక(my_ఆధారం_Char) {
            స్థితి UCNV_SI:
                ఒకవేళ(my_దత్తాంశం->సంస్కరణ==3) {
                    p_గమ్యం_U2022_State->g=0;
                    కొనసాగు;
                } లేకపోతే {
                    /* only JIS7 uses SI/SO, not ISO-2022-JP-x */
                    my_దత్తాంశం->is_ఖాళీలు_Segment = అబద్ధం;	/* reset this, we have a different error */
                    ఆపు;
                }

            స్థితి UCNV_SO:
                ఒకవేళ(my_దత్తాంశం->సంస్కరణ==3) {
                    /* JIS7: switch to G1 half-width Katakana */
                    p_గమ్యం_U2022_State->cs[1] = static_cast<int8_t>(HWKANA_7_బిట్టు);
                    p_గమ్యం_U2022_State->g=1;
                    కొనసాగు;
                } లేకపోతే {
                    /* only JIS7 uses SI/SO, not ISO-2022-JP-x */
                    my_దత్తాంశం->is_ఖాళీలు_Segment = అబద్ధం;	/* reset this, we have a different error */
                    ఆపు;
                }

            స్థితి ESC_2022:
                my_ఆధారం--;
escape:
                {
                    స్థిరరాశి అక్షరం * my_ఆధారం_Before = my_ఆధారం;
                    int8_t గమ్యం_ULength_Before = వాదనలు_2->converter->గమ్యం_ULength;

                    changeState_2022(వాదనలు_2->converter,&(my_ఆధారం),
                        my_ఆధారం_హద్దు, ISO_2022_JP,err);

                    /* If in ISO-2022-JP only and we successfully completed an escape sequence, but previous segment was empty, create an error */
                    ఒకవేళ(my_దత్తాంశం->సంస్కరణ==0 && my_దత్తాంశం->కీలకం==0 && U_SUCCESS(*err) && my_దత్తాంశం->is_ఖాళీలు_Segment) {
                        *err = U_ILLEGAL_ESCAPE_SEQUENCE;
                        వాదనలు_2->converter->గమ్యం_UCallback_Reason = UCNV_IRREGULAR;
                        వాదనలు_2->converter->గమ్యం_ULength = static_cast<int8_t>(గమ్యం_ULength_Before + (my_ఆధారం - my_ఆధారం_Before));
                    }
                }

                /* invalid or illegal escape sequence */
                ఒకవేళ(U_FAILURE(*err)){
                    వాదనలు_2->లక్ష్యం = my_లక్ష్యం;
                    వాదనలు_2->ఆధారం = my_ఆధారం;
                    my_దత్తాంశం->is_ఖాళీలు_Segment = అబద్ధం;	/* Reset to avoid future spurious errors */
                    తిరిగి;
                }
                /* If we successfully completed an escape sequence, we begin a new segment, empty so far */
                ఒకవేళ(my_దత్తాంశం->కీలకం==0) {
                    my_దత్తాంశం->is_ఖాళీలు_Segment = నిజం;
                }
                కొనసాగు;

            /* ISO-2022-JP does not use single-byte (C1) SS2 and SS3 */

            స్థితి CR:
            స్థితి LF:
                /* automatically reset to single-byte mode */
                ఒకవేళ (static_cast<StateEnum>(p_గమ్యం_U2022_State->cs[0]) != ASCII &&
                    static_cast<StateEnum>(p_గమ్యం_U2022_State->cs[0]) != JISX201) {
                    p_గమ్యం_U2022_State->cs[0] = static_cast<int8_t>(ASCII);
                }
                p_గమ్యం_U2022_State->cs[2] = 0;
                p_గమ్యం_U2022_State->g = 0;
                U_FALLTHROUGH;
            అప్రమేయ:
                /* convert one or two bytes */
                my_దత్తాంశం->is_ఖాళీలు_Segment = అబద్ధం;
                cs = static_cast<StateEnum>(p_గమ్యం_U2022_State->cs[p_గమ్యం_U2022_State->g]);
                ఒకవేళ (static_cast<uint8_t>(my_ఆధారం_Char - 0xa1) <= (0xdf - 0xa1) && my_దత్తాంశం->సంస్కరణ == 4 &&
                    !IS_JP_DBCS(cs)
                ) {
                    /* 8-bit halfwidth katakana in any single-byte mode for JIS8 */
                    లక్ష్యం_Uni_Char = my_ఆధారం_Char + (HWKANA_ప్రారంభం - 0xa1);

                    /* return from a single-shift state to the previous one */
                    ఒకవేళ(p_గమ్యం_U2022_State->g >= 2) {
                        p_గమ్యం_U2022_State->g=p_గమ్యం_U2022_State->prevG;
                    }
                } లేకపోతే ఎంపిక(cs) {
                స్థితి ASCII:
                    ఒకవేళ(my_ఆధారం_Char <= 0x7f) {
                        లక్ష్యం_Uni_Char = my_ఆధారం_Char;
                    }
                    ఆపు;
                స్థితి ISO8859_1:
                    ఒకవేళ(my_ఆధారం_Char <= 0x7f) {
                        లక్ష్యం_Uni_Char = my_ఆధారం_Char + 0x80;
                    }
                    /* return from a single-shift state to the previous one */
                    p_గమ్యం_U2022_State->g=p_గమ్యం_U2022_State->prevG;
                    ఆపు;
                స్థితి ISO8859_7:
                    ఒకవేళ(my_ఆధారం_Char <= 0x7f) {
                        /* convert mySourceChar+0x80 to use a normal 8-bit table */
                        లక్ష్యం_Uni_Char =
                            MBCS_SINGLE_SIMPLE_GET_తదుపరి_BMP(
                                my_దత్తాంశం->myConverterArray[cs],
                                my_ఆధారం_Char + 0x80);
                    }
                    /* return from a single-shift state to the previous one */
                    p_గమ్యం_U2022_State->g=p_గమ్యం_U2022_State->prevG;
                    ఆపు;
                స్థితి JISX201:
                    ఒకవేళ(my_ఆధారం_Char <= 0x7f) {
                        లక్ష్యం_Uni_Char = jisx201_గమ్యం_U(my_ఆధారం_Char);
                    }
                    ఆపు;
                స్థితి HWKANA_7_బిట్టు:
                    ఒకవేళ (static_cast<uint8_t>(my_ఆధారం_Char - 0x21) <= (0x5f - 0x21)) {
                        /* 7-bit halfwidth Katakana */
                        లక్ష్యం_Uni_Char = my_ఆధారం_Char + (HWKANA_ప్రారంభం - 0x21);
                    }
                    ఆపు;
                అప్రమేయ:
                    /* G0 DBCS */
                    ఒకవేళ(my_ఆధారం < my_ఆధారం_హద్దు) {
                        పూర్ణాంకం leadIsOk, trailIsOk;
                        uint8_t trailByte;
getTrailByte:
                        trailByte = static_cast<uint8_t>(*my_ఆధారం);
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
                        leadIsOk = static_cast<uint8_t>(my_ఆధారం_Char - 0x21) <= (0x7e - 0x21);
                        trailIsOk = static_cast<uint8_t>(trailByte - 0x21) <= (0x7e - 0x21);
                        ఒకవేళ (leadIsOk && trailIsOk) {
                            ++my_ఆధారం;
                            tmp_ఆధారం_Char = (my_ఆధారం_Char << 8) | trailByte;
                            ఒకవేళ(cs == JISX208) {
                                _2022ToSJIS(static_cast<uint8_t>(my_ఆధారం_Char), trailByte, తాత్కాలిక_Buf);
                                my_ఆధారం_Char = tmp_ఆధారం_Char;
                            } లేకపోతే {
                                /* Copy before we modify tmpSourceChar so toUnicodeCallback() sees the correct bytes. */
                                my_ఆధారం_Char = tmp_ఆధారం_Char;
                                ఒకవేళ (cs == KSC5601) {
                                    tmp_ఆధారం_Char += 0x8080;  /* = _2022ToGR94DBCS(tmpSourceChar) */
                                }
                                తాత్కాలిక_Buf[0] = static_cast<అక్షరం>(tmp_ఆధారం_Char >> 8);
                                తాత్కాలిక_Buf[1] = static_cast<అక్షరం>(tmp_ఆధారం_Char);
                            }
                            లక్ష్యం_Uni_Char = ucnv_MBCSSimple_Get_తదుపరి_UChar(my_దత్తాంశం->myConverterArray[cs], తాత్కాలిక_Buf, 2, అబద్ధం);
                        } లేకపోతే ఒకవేళ (!(trailIsOk || IS_2022_CONTROL(trailByte))) {
                            /* report a pair of illegal bytes if the second byte is not a DBCS starter */
                            ++my_ఆధారం;
                            /* add another bit so that the code below writes 2 bytes in case of error */
                            my_ఆధారం_Char = 0x10000 | (my_ఆధారం_Char << 8) | trailByte;
                        }
                    } లేకపోతే {
                        వాదనలు_2->converter->గమ్యం_UBytes[0] = static_cast<uint8_t>(my_ఆధారం_Char);
                        వాదనలు_2->converter->గమ్యం_ULength = 1;
                        వెళ్ళు endloop;
                    }
                }  /* End of inner switch */
                ఆపు;
            }  /* End of outer switch */
            ఒకవేళ(లక్ష్యం_Uni_Char < (missingCharMarker-1/*0xfffe*/)){
                ఒకవేళ(వాదనలు_2->offsets){
                    వాదనలు_2->offsets[my_లక్ష్యం - వాదనలు_2->లక్ష్యం] = static_cast<int32_t>(my_ఆధారం - వాదనలు_2->ఆధారం - (my_ఆధారం_Char <= 0xff ? 1 : 2));
                }
                *(my_లక్ష్యం++) = static_cast<char16_t>(లక్ష్యం_Uni_Char);
            }
            లేకపోతే ఒకవేళ(లక్ష్యం_Uni_Char > missingCharMarker){
                /* disassemble the surrogate pair and write to output*/
                లక్ష్యం_Uni_Char-=0x0010000;
                *my_లక్ష్యం = static_cast<char16_t>(0xd800 + static_cast<char16_t>(లక్ష్యం_Uni_Char >> 10));
                ఒకవేళ(వాదనలు_2->offsets){
                    వాదనలు_2->offsets[my_లక్ష్యం - వాదనలు_2->లక్ష్యం] = static_cast<int32_t>(my_ఆధారం - వాదనలు_2->ఆధారం - (my_ఆధారం_Char <= 0xff ? 1 : 2));
                }
                ++my_లక్ష్యం;
                ఒకవేళ(my_లక్ష్యం< వాదనలు_2->లక్ష్యం_హద్దు){
                    *my_లక్ష్యం = static_cast<char16_t>(0xdc00 + static_cast<char16_t>(లక్ష్యం_Uni_Char & 0x3ff));
                    ఒకవేళ(వాదనలు_2->offsets){
                        వాదనలు_2->offsets[my_లక్ష్యం - వాదనలు_2->లక్ష్యం] = static_cast<int32_t>(my_ఆధారం - వాదనలు_2->ఆధారం - (my_ఆధారం_Char <= 0xff ? 1 : 2));
                    }
                    ++my_లక్ష్యం;
                }లేకపోతే{
                    వాదనలు_2->converter->UChar_Error_బఫరు[వాదనలు_2->converter->UChar_Error_బఫరు_నిడివి++]=
                                    static_cast<char16_t>(0xdc00 + static_cast<char16_t>(లక్ష్యం_Uni_Char & 0x3ff));
                }

            }
            లేకపోతే{
                /* Call the callback function*/
                గమ్యం_Unicode_Callback(వాదనలు_2->converter,my_ఆధారం_Char,లక్ష్యం_Uni_Char,err);
                ఆపు;
            }
        }
        లేకపోతే{    /* goes with "if(myTarget < args->targetLimit)"  way up near top of function */
            *err =U_బఫరు_అధికప్రవాహం_ERROR;
            ఆపు;
        }
    }
endloop:
    వాదనలు_2->లక్ష్యం = my_లక్ష్యం;
    వాదనలు_2->ఆధారం = my_ఆధారం;
}


#ఒకవేళ !UCONFIG_ONLY_HTML_CONVERSION
/***************************************************************
*   Rules for ISO-2022-KR encoding
*   i) The KSC5601 designator sequence should appear only once in a file,
*      at the beginning of a line before any KSC5601 characters. This usually
*      means that it appears by itself on the first line of the file
*  ii) There are only 2 shifting sequences SO to shift into double byte mode
*      and SI to shift into single byte mode
*/
స్థిర శూన్య U_CALLCONV
UConverter_fromUnicode_ISO_2022_KR_OFFSETS_LOGIC_IBM(UConverter_From_Unicode_వాదనలు* వాదనలు_2, UError_సంకేతం* err){

    UConverter* saveConv = వాదనలు_2->converter;
    UConverter_దత్తాంశం_ISO2022* my_Converter_దత్తాంశం = static_cast<UConverter_దత్తాంశం_ISO2022*>(saveConv->extraInfo);
    వాదనలు_2->converter=my_Converter_దత్తాంశం->currentConverter;

    my_Converter_దత్తాంశం->currentConverter->fromUChar32 = saveConv->fromUChar32;
    ucnv_MBCSFromUnicodeWithOffsets(వాదనలు_2,err);
    saveConv->fromUChar32 = my_Converter_దత్తాంశం->currentConverter->fromUChar32;

    ఒకవేళ(*err == U_బఫరు_అధికప్రవాహం_ERROR) {
        ఒకవేళ(my_Converter_దత్తాంశం->currentConverter->char_Error_బఫరు_నిడివి > 0) {
            uprv_memcpy(
                saveConv->char_Error_బఫరు,
                my_Converter_దత్తాంశం->currentConverter->char_Error_బఫరు,
                my_Converter_దత్తాంశం->currentConverter->char_Error_బఫరు_నిడివి);
        }
        saveConv->char_Error_బఫరు_నిడివి = my_Converter_దత్తాంశం->currentConverter->char_Error_బఫరు_నిడివి;
        my_Converter_దత్తాంశం->currentConverter->char_Error_బఫరు_నిడివి = 0;
    }
    వాదనలు_2->converter=saveConv;
}

స్థిర శూన్య U_CALLCONV
UConverter_fromUnicode_ISO_2022_KR_OFFSETS_LOGIC(UConverter_From_Unicode_వాదనలు* వాదనలు_2, UError_సంకేతం* err){

    స్థిరరాశి char16_t *ఆధారం = వాదనలు_2->ఆధారం;
    స్థిరరాశి char16_t *ఆధారం_హద్దు = వాదనలు_2->ఆధారం_హద్దు;
    గుర్తులేని అక్షరం *లక్ష్యం = reinterpret_cast<గుర్తులేని అక్షరం*>(వాదనలు_2->లక్ష్యం);
    గుర్తులేని అక్షరం *లక్ష్యం_హద్దు = reinterpret_cast<గుర్తులేని అక్షరం*>(const_cast<అక్షరం*>(వాదనలు_2->లక్ష్యం_హద్దు));
    int32_t* offsets = వాదనలు_2->offsets;
    uint32_t లక్ష్యం_Byte_Unit = 0x0000;
    UChar32 ఆధారం_Char = 0x0000;
    UBool is_లక్ష్యం_Byte_DBCS;
    UBool old_Is_లక్ష్యం_Byte_DBCS;
    UConverter_దత్తాంశం_ISO2022 *converter_దత్తాంశం;
    UConverter_Shared_దత్తాంశం* shared_దత్తాంశం;
    UBool useFallback;
    int32_t length =0;

    converter_దత్తాంశం = static_cast<UConverter_దత్తాంశం_ISO2022*>(వాదనలు_2->converter->extraInfo);
    /* if the version is 1 then the user is requesting
     * conversion with ibm-25546 pass the arguments to
     * MBCS converter and return
     */
    ఒకవేళ(converter_దత్తాంశం->సంస్కరణ==1){
        UConverter_fromUnicode_ISO_2022_KR_OFFSETS_LOGIC_IBM(వాదనలు_2,err);
        తిరిగి;
    }

    /* initialize data */
    shared_దత్తాంశం = converter_దత్తాంశం->currentConverter->shared_దత్తాంశం;
    useFallback = వాదనలు_2->converter->useFallback;
    is_లక్ష్యం_Byte_DBCS = static_cast<UBool>(వాదనలు_2->converter->from_Unicode_స్థితి);
    old_Is_లక్ష్యం_Byte_DBCS = is_లక్ష్యం_Byte_DBCS;

    is_లక్ష్యం_Byte_DBCS = static_cast<UBool>(వాదనలు_2->converter->from_Unicode_స్థితి);
    ఒకవేళ((ఆధారం_Char = వాదనలు_2->converter->fromUChar32)!=0 && లక్ష్యం <లక్ష్యం_హద్దు) {
        వెళ్ళు getTrail;
    }
    అయితే(ఆధారం < ఆధారం_హద్దు){

        లక్ష్యం_Byte_Unit = missingCharMarker;

        ఒకవేళ(లక్ష్యం < (గుర్తులేని అక్షరం*) వాదనలు_2->లక్ష్యం_హద్దు){
            ఆధారం_Char = *ఆధారం++;

            /* do not convert SO/SI/ESC */
            ఒకవేళ(IS_2022_CONTROL(ఆధారం_Char)) {
                /* callback(illegal) */
                *err=U_ILLEGAL_CHAR_దొరికింది;
                వాదనలు_2->converter->fromUChar32=ఆధారం_Char;
                ఆపు;
            }

            length = MBCS_FROM_UCHAR32_ISO2022(shared_దత్తాంశం,ఆధారం_Char,&లక్ష్యం_Byte_Unit,useFallback,MBCS_OUTPUT_2);
            ఒకవేళ(length < 0) {
                length = -length;  /* fallback */
            }
            /* only DBCS or SBCS characters are expected*/
            /* DB characters with high bit set to 1 are expected */
            ఒకవేళ( length > 2 || length==0 ||
                (length == 1 && లక్ష్యం_Byte_Unit > 0x7f) ||
                (length == 2 &&
                    (static_cast<uint16_t>(లక్ష్యం_Byte_Unit - 0xa1a1) > (0xfefe - 0xa1a1) ||
                    static_cast<uint8_t>(లక్ష్యం_Byte_Unit - 0xa1) > (0xfe - 0xa1)))
            ) {
                లక్ష్యం_Byte_Unit=missingCharMarker;
            }
            ఒకవేళ (లక్ష్యం_Byte_Unit != missingCharMarker){

                old_Is_లక్ష్యం_Byte_DBCS = is_లక్ష్యం_Byte_DBCS;
                is_లక్ష్యం_Byte_DBCS = static_cast<UBool>(లక్ష్యం_Byte_Unit > 0x00FF);
                  /* append the shift sequence */
                ఒకవేళ (old_Is_లక్ష్యం_Byte_DBCS != is_లక్ష్యం_Byte_DBCS ){

                    ఒకవేళ (is_లక్ష్యం_Byte_DBCS)
                        *లక్ష్యం++ = UCNV_SO;
                    లేకపోతే
                        *లక్ష్యం++ = UCNV_SI;
                    ఒకవేళ(offsets)
                        *(offsets++) = static_cast<int32_t>(ఆధారం - వాదనలు_2->ఆధారం - 1);
                }
                /* write the targetUniChar  to target */
                ఒకవేళ(లక్ష్యం_Byte_Unit <= 0x00FF){
                    ఒకవేళ( లక్ష్యం < లక్ష్యం_హద్దు){
                        *(లక్ష్యం++) = static_cast<గుర్తులేని అక్షరం>(లక్ష్యం_Byte_Unit);
                        ఒకవేళ(offsets){
                            *(offsets++) = static_cast<int32_t>(ఆధారం - వాదనలు_2->ఆధారం - 1);
                        }

                    }లేకపోతే{
                        వాదనలు_2->converter->char_Error_బఫరు[వాదనలు_2->converter->char_Error_బఫరు_నిడివి++] = static_cast<గుర్తులేని అక్షరం>(లక్ష్యం_Byte_Unit);
                        *err = U_బఫరు_అధికప్రవాహం_ERROR;
                    }
                }లేకపోతే{
                    ఒకవేళ(లక్ష్యం < లక్ష్యం_హద్దు){
                        *(లక్ష్యం++) = static_cast<గుర్తులేని అక్షరం>((లక్ష్యం_Byte_Unit >> 8) - 0x80);
                        ఒకవేళ(offsets){
                            *(offsets++) = static_cast<int32_t>(ఆధారం - వాదనలు_2->ఆధారం - 1);
                        }
                        ఒకవేళ(లక్ష్యం < లక్ష్యం_హద్దు){
                            *(లక్ష్యం++) = static_cast<గుర్తులేని అక్షరం>(లక్ష్యం_Byte_Unit - 0x80);
                            ఒకవేళ(offsets){
                                *(offsets++) = static_cast<int32_t>(ఆధారం - వాదనలు_2->ఆధారం - 1);
                            }
                        }లేకపోతే{
                            వాదనలు_2->converter->char_Error_బఫరు[వాదనలు_2->converter->char_Error_బఫరు_నిడివి++] = static_cast<గుర్తులేని అక్షరం>(లక్ష్యం_Byte_Unit - 0x80);
                            *err = U_బఫరు_అధికప్రవాహం_ERROR;
                        }
                    }లేకపోతే{
                        వాదనలు_2->converter->char_Error_బఫరు[వాదనలు_2->converter->char_Error_బఫరు_నిడివి++] = static_cast<గుర్తులేని అక్షరం>((లక్ష్యం_Byte_Unit >> 8) - 0x80);
                        వాదనలు_2->converter->char_Error_బఫరు[వాదనలు_2->converter->char_Error_బఫరు_నిడివి++] = static_cast<గుర్తులేని అక్షరం>(లక్ష్యం_Byte_Unit - 0x80);
                        *err = U_బఫరు_అధికప్రవాహం_ERROR;
                    }
                }

            }
            లేకపోతే{
                /* oops.. the code point is unassingned
                 * set the error and reason
                 */

                /*check if the char is a First surrogate*/
                ఒకవేళ(U16_IS_SURROGATE(ఆధారం_Char)) {
                    ఒకవేళ(U16_IS_SURROGATE_LEAD(ఆధారం_Char)) {
getTrail:
                        /*look ahead to find the trail surrogate*/
                        ఒకవేళ(ఆధారం <  ఆధారం_హద్దు) {
                            /* test the following code unit */
                            char16_t trail = *ఆధారం;
                            ఒకవేళ(U16_IS_TRAIL(trail)) {
                                ఆధారం++;
                                ఆధారం_Char=U16_GET_SUPPLEMENTARY(ఆధారం_Char, trail);
                                *err = U_INVALID_CHAR_దొరికింది;
                                /* convert this surrogate code point */
                                /* exit this condition tree */
                            } లేకపోతే {
                                /* this is an unmatched lead code unit (1st surrogate) */
                                /* callback(illegal) */
                                *err=U_ILLEGAL_CHAR_దొరికింది;
                            }
                        } లేకపోతే {
                            /* no more input */
                            *err = U_ZERO_ERROR;
                        }
                    } లేకపోతే {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *err=U_ILLEGAL_CHAR_దొరికింది;
                    }
                } లేకపోతే {
                    /* callback(unassigned) for a BMP code point */
                    *err = U_INVALID_CHAR_దొరికింది;
                }

                వాదనలు_2->converter->fromUChar32=ఆధారం_Char;
                ఆపు;
            }
        } /* end if(myTargetIndex<myTargetLength) */
        లేకపోతే{
            *err =U_బఫరు_అధికప్రవాహం_ERROR;
            ఆపు;
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
    ఒకవేళ( U_SUCCESS(*err) &&
        is_లక్ష్యం_Byte_DBCS &&
        వాదనలు_2->flush && ఆధారం>=ఆధారం_హద్దు && వాదనలు_2->converter->fromUChar32==0
    ) {
        int32_t ఆధారం_సూచీ;

        /* we are switching to ASCII */
        is_లక్ష్యం_Byte_DBCS=అబద్ధం;

        /* get the source index of the last input character */
        /*
         * TODO this would be simpler and more reliable if we used a pair
         * of sourceIndex/prevSourceIndex like in ucnvmbcs.c
         * so that we could simply use the prevSourceIndex here;
         * this code gives an incorrect result for the rare case of an unmatched
         * trail surrogate that is alone in the last buffer of the text stream
         */
        ఆధారం_సూచీ = static_cast<int32_t>(ఆధారం - వాదనలు_2->ఆధారం);
        ఒకవేళ(ఆధారం_సూచీ>0) {
            --ఆధారం_సూచీ;
            ఒకవేళ( U16_IS_TRAIL(వాదనలు_2->ఆధారం[ఆధారం_సూచీ]) &&
                (ఆధారం_సూచీ==0 || U16_IS_LEAD(వాదనలు_2->ఆధారం[ఆధారం_సూచీ-1]))
            ) {
                --ఆధారం_సూచీ;
            }
        } లేకపోతే {
            ఆధారం_సూచీ=-1;
        }

        fromUWriteUInt8(
            వాదనలు_2->converter,
            జరుపు_లోపల_STR, 1,
            &లక్ష్యం, reinterpret_cast<స్థిరరాశి అక్షరం*>(లక్ష్యం_హద్దు),
            &offsets, ఆధారం_సూచీ,
            err);
    }

    /*save the state and return */
    వాదనలు_2->ఆధారం = ఆధారం;
    వాదనలు_2->లక్ష్యం = reinterpret_cast<అక్షరం*>(లక్ష్యం);
    వాదనలు_2->converter->from_Unicode_స్థితి = static_cast<uint32_t>(is_లక్ష్యం_Byte_DBCS);
}

/************************ To Unicode ***************************************/

స్థిర శూన్య U_CALLCONV
UConverter_గమ్యం_Unicode_ISO_2022_KR_OFFSETS_LOGIC_IBM(UConverter_గమ్యం_Unicode_వాదనలు *వాదనలు_2,
                                                            UError_సంకేతం* err){
    అక్షరం స్థిరరాశి* ఆధారం_ప్రారంభం;
    UConverter_దత్తాంశం_ISO2022* my_దత్తాంశం = static_cast<UConverter_దత్తాంశం_ISO2022*>(వాదనలు_2->converter->extraInfo);

    UConverter_గమ్యం_Unicode_వాదనలు sub_వాదనలు;
    int32_t min_వాదనలు_కొలత;

    /* set up the subconverter arguments */
    ఒకవేళ(వాదనలు_2->size<పరిమాణం(UConverter_గమ్యం_Unicode_వాదనలు)) {
        min_వాదనలు_కొలత = వాదనలు_2->size;
    } లేకపోతే {
        min_వాదనలు_కొలత = static_cast<int32_t>(పరిమాణం(UConverter_గమ్యం_Unicode_వాదనలు));
    }

    uprv_memcpy(&sub_వాదనలు, వాదనలు_2, min_వాదనలు_కొలత);
    sub_వాదనలు.size = static_cast<uint16_t>(min_వాదనలు_కొలత);
    sub_వాదనలు.converter = my_దత్తాంశం->currentConverter;

    /* remember the original start of the input for offsets */
    ఆధారం_ప్రారంభం = వాదనలు_2->ఆధారం;

    ఒకవేళ(my_దత్తాంశం->కీలకం != 0) {
        /* continue with a partial escape sequence */
        వెళ్ళు escape;
    }

    అయితే(U_SUCCESS(*err) && వాదనలు_2->ఆధారం < వాదనలు_2->ఆధారం_హద్దు) {
        /*Find the end of the buffer e.g : Next Escape Seq | end of Buffer*/
        sub_వాదనలు.ఆధారం = వాదనలు_2->ఆధారం;
        sub_వాదనలు.ఆధారం_హద్దు = get_చివర_యొక్క_బఫరు_2022(&(వాదనలు_2->ఆధారం), వాదనలు_2->ఆధారం_హద్దు, వాదనలు_2->flush);
        ఒకవేళ(sub_వాదనలు.ఆధారం != sub_వాదనలు.ఆధారం_హద్దు) {
            /*
             * get the current partial byte sequence
             *
             * it needs to be moved between the public and the subconverter
             * so that the conversion framework, which only sees the public
             * converter, can handle truncated and illegal input etc.
             */
            ఒకవేళ(వాదనలు_2->converter->గమ్యం_ULength > 0) {
                uprv_memcpy(sub_వాదనలు.converter->గమ్యం_UBytes, వాదనలు_2->converter->గమ్యం_UBytes, వాదనలు_2->converter->గమ్యం_ULength);
            }
            sub_వాదనలు.converter->గమ్యం_ULength = వాదనలు_2->converter->గమ్యం_ULength;

            /*
             * Convert up to the end of the input, or to before the next escape character.
             * Does not handle conversion extensions because the preToU[] state etc.
             * is not copied.
             */
            ucnv_MBCSToUnicodeWithOffsets(&sub_వాదనలు, err);

            ఒకవేళ(వాదనలు_2->offsets != శూన్యసూచిక && ఆధారం_ప్రారంభం != వాదనలు_2->ఆధారం) {
                /* update offsets to base them on the actual start of the input */
                int32_t *offsets = వాదనలు_2->offsets;
                char16_t *లక్ష్యం = వాదనలు_2->లక్ష్యం;
                int32_t delta = static_cast<int32_t>(వాదనలు_2->ఆధారం - ఆధారం_ప్రారంభం);
                అయితే(లక్ష్యం < sub_వాదనలు.లక్ష్యం) {
                    ఒకవేళ(*offsets >= 0) {
                        *offsets += delta;
                    }
                    ++offsets;
                    ++లక్ష్యం;
                }
            }
            వాదనలు_2->ఆధారం = sub_వాదనలు.ఆధారం;
            వాదనలు_2->లక్ష్యం = sub_వాదనలు.లక్ష్యం;
            వాదనలు_2->offsets = sub_వాదనలు.offsets;

            /* copy input/error/overflow buffers */
            ఒకవేళ(sub_వాదనలు.converter->గమ్యం_ULength > 0) {
                uprv_memcpy(వాదనలు_2->converter->గమ్యం_UBytes, sub_వాదనలు.converter->గమ్యం_UBytes, sub_వాదనలు.converter->గమ్యం_ULength);
            }
            వాదనలు_2->converter->గమ్యం_ULength = sub_వాదనలు.converter->గమ్యం_ULength;

            ఒకవేళ(*err == U_బఫరు_అధికప్రవాహం_ERROR) {
                ఒకవేళ(sub_వాదనలు.converter->UChar_Error_బఫరు_నిడివి > 0) {
                    uprv_memcpy(వాదనలు_2->converter->UChar_Error_బఫరు, sub_వాదనలు.converter->UChar_Error_బఫరు,
                                sub_వాదనలు.converter->UChar_Error_బఫరు_నిడివి);
                }
                వాదనలు_2->converter->UChar_Error_బఫరు_నిడివి=sub_వాదనలు.converter->UChar_Error_బఫరు_నిడివి;
                sub_వాదనలు.converter->UChar_Error_బఫరు_నిడివి = 0;
            }
        }

        ఒకవేళ (U_FAILURE(*err) || (వాదనలు_2->ఆధారం == వాదనలు_2->ఆధారం_హద్దు)) {
            తిరిగి;
        }

escape:
        changeState_2022(వాదనలు_2->converter,
               &(వాదనలు_2->ఆధారం),
               వాదనలు_2->ఆధారం_హద్దు,
               ISO_2022_KR,
               err);
    }
}

స్థిర శూన్య U_CALLCONV
UConverter_గమ్యం_Unicode_ISO_2022_KR_OFFSETS_LOGIC(UConverter_గమ్యం_Unicode_వాదనలు *వాదనలు_2,
                                                            UError_సంకేతం* err){
    అక్షరం తాత్కాలిక_Buf[2];
    స్థిరరాశి అక్షరం* my_ఆధారం = const_cast<అక్షరం*>(వాదనలు_2->ఆధారం);
    char16_t *my_లక్ష్యం = వాదనలు_2->లక్ష్యం;
    స్థిరరాశి అక్షరం *my_ఆధారం_హద్దు = వాదనలు_2->ఆధారం_హద్దు;
    UChar32 లక్ష్యం_Uni_Char = 0x0000;
    char16_t my_ఆధారం_Char = 0x0000;
    UConverter_దత్తాంశం_ISO2022* my_దత్తాంశం;
    UConverter_Shared_దత్తాంశం* shared_దత్తాంశం ;
    UBool useFallback;

    my_దత్తాంశం = static_cast<UConverter_దత్తాంశం_ISO2022*>(వాదనలు_2->converter->extraInfo);
    ఒకవేళ(my_దత్తాంశం->సంస్కరణ==1){
        UConverter_గమ్యం_Unicode_ISO_2022_KR_OFFSETS_LOGIC_IBM(వాదనలు_2,err);
        తిరిగి;
    }

    /* initialize state */
    shared_దత్తాంశం = my_దత్తాంశం->currentConverter->shared_దత్తాంశం;
    useFallback = వాదనలు_2->converter->useFallback;

    ఒకవేళ(my_దత్తాంశం->కీలకం != 0) {
        /* continue with a partial escape sequence */
        వెళ్ళు escape;
    } లేకపోతే ఒకవేళ(వాదనలు_2->converter->గమ్యం_ULength == 1 && my_ఆధారం < my_ఆధారం_హద్దు && my_లక్ష్యం < వాదనలు_2->లక్ష్యం_హద్దు) {
        /* continue with a partial double-byte character */
        my_ఆధారం_Char = వాదనలు_2->converter->గమ్యం_UBytes[0];
        వాదనలు_2->converter->గమ్యం_ULength = 0;
        వెళ్ళు getTrailByte;
    }

    అయితే(my_ఆధారం< my_ఆధారం_హద్దు){

        ఒకవేళ(my_లక్ష్యం < వాదనలు_2->లక్ష్యం_హద్దు){

            my_ఆధారం_Char = static_cast<గుర్తులేని అక్షరం>(*my_ఆధారం++);

            ఒకవేళ(my_ఆధారం_Char==UCNV_SI){
                my_దత్తాంశం->గమ్యం_U2022_State.g = 0;
                ఒకవేళ (my_దత్తాంశం->is_ఖాళీలు_Segment) {
                    my_దత్తాంశం->is_ఖాళీలు_Segment = అబద్ధం;	/* we are handling it, reset to avoid future spurious errors */
                    *err = U_ILLEGAL_ESCAPE_SEQUENCE;
                    వాదనలు_2->converter->గమ్యం_UCallback_Reason = UCNV_IRREGULAR;
                    వాదనలు_2->converter->గమ్యం_UBytes[0] = static_cast<uint8_t>(my_ఆధారం_Char);
                    వాదనలు_2->converter->గమ్యం_ULength = 1;
                    వాదనలు_2->లక్ష్యం = my_లక్ష్యం;
                    వాదనలు_2->ఆధారం = my_ఆధారం;
                    తిరిగి;
                }
                /*consume the source */
                కొనసాగు;
            }లేకపోతే ఒకవేళ(my_ఆధారం_Char==UCNV_SO){
                my_దత్తాంశం->గమ్యం_U2022_State.g = 1;
                my_దత్తాంశం->is_ఖాళీలు_Segment = నిజం;	/* Begin a new segment, empty so far */
                /*consume the source */
                కొనసాగు;
            }లేకపోతే ఒకవేళ(my_ఆధారం_Char==ESC_2022){
                my_ఆధారం--;
escape:
                my_దత్తాంశం->is_ఖాళీలు_Segment = అబద్ధం;	/* Any invalid ESC sequences will be detected separately, so just reset this */
                changeState_2022(వాదనలు_2->converter,&(my_ఆధారం),
                                my_ఆధారం_హద్దు, ISO_2022_KR, err);
                ఒకవేళ(U_FAILURE(*err)){
                    వాదనలు_2->లక్ష్యం = my_లక్ష్యం;
                    వాదనలు_2->ఆధారం = my_ఆధారం;
                    తిరిగి;
                }
                కొనసాగు;
            }

            my_దత్తాంశం->is_ఖాళీలు_Segment = అబద్ధం;	/* Any invalid char errors will be detected separately, so just reset this */
            ఒకవేళ(my_దత్తాంశం->గమ్యం_U2022_State.g == 1) {
                ఒకవేళ(my_ఆధారం < my_ఆధారం_హద్దు) {
                    పూర్ణాంకం leadIsOk, trailIsOk;
                    uint8_t trailByte;
getTrailByte:
                    లక్ష్యం_Uni_Char = missingCharMarker;
                    trailByte = static_cast<uint8_t>(*my_ఆధారం);
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
                    leadIsOk = static_cast<uint8_t>(my_ఆధారం_Char - 0x21) <= (0x7e - 0x21);
                    trailIsOk = static_cast<uint8_t>(trailByte - 0x21) <= (0x7e - 0x21);
                    ఒకవేళ (leadIsOk && trailIsOk) {
                        ++my_ఆధారం;
                        తాత్కాలిక_Buf[0] = static_cast<అక్షరం>(my_ఆధారం_Char + 0x80);
                        తాత్కాలిక_Buf[1] = static_cast<అక్షరం>(trailByte + 0x80);
                        లక్ష్యం_Uni_Char = ucnv_MBCSSimple_Get_తదుపరి_UChar(shared_దత్తాంశం, తాత్కాలిక_Buf, 2, useFallback);
                        my_ఆధారం_Char = (my_ఆధారం_Char << 8) | trailByte;
                    } లేకపోతే ఒకవేళ (!(trailIsOk || IS_2022_CONTROL(trailByte))) {
                        /* report a pair of illegal bytes if the second byte is not a DBCS starter */
                        ++my_ఆధారం;
                        /* add another bit so that the code below writes 2 bytes in case of error */
                        my_ఆధారం_Char = static_cast<char16_t>(0x10000 | (my_ఆధారం_Char << 8) | trailByte);
                    }
                } లేకపోతే {
                    వాదనలు_2->converter->గమ్యం_UBytes[0] = static_cast<uint8_t>(my_ఆధారం_Char);
                    వాదనలు_2->converter->గమ్యం_ULength = 1;
                    ఆపు;
                }
            }
            లేకపోతే ఒకవేళ(my_ఆధారం_Char <= 0x7f) {
                లక్ష్యం_Uni_Char = ucnv_MBCSSimple_Get_తదుపరి_UChar(shared_దత్తాంశం, my_ఆధారం - 1, 1, useFallback);
            } లేకపోతే {
                లక్ష్యం_Uni_Char = 0xffff;
            }
            ఒకవేళ(లక్ష్యం_Uni_Char < 0xfffe){
                ఒకవేళ(వాదనలు_2->offsets) {
                    వాదనలు_2->offsets[my_లక్ష్యం - వాదనలు_2->లక్ష్యం] = static_cast<int32_t>(my_ఆధారం - వాదనలు_2->ఆధారం - (my_ఆధారం_Char <= 0xff ? 1 : 2));
                }
                *(my_లక్ష్యం++) = static_cast<char16_t>(లక్ష్యం_Uni_Char);
            }
            లేకపోతే {
                /* Call the callback function*/
                గమ్యం_Unicode_Callback(వాదనలు_2->converter,my_ఆధారం_Char,లక్ష్యం_Uni_Char,err);
                ఆపు;
            }
        }
        లేకపోతే{
            *err =U_బఫరు_అధికప్రవాహం_ERROR;
            ఆపు;
        }
    }
    వాదనలు_2->లక్ష్యం = my_లక్ష్యం;
    వాదనలు_2->ఆధారం = my_ఆధారం;
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
స్థిర స్థిరరాశి అక్షరం GB_2312_80_STR[] = "\x1B\x24\x29\x41";
స్థిర స్థిరరాశి అక్షరం ISO_IR_165_STR[] = "\x1B\x24\x29\x45";
స్థిర స్థిరరాశి అక్షరం CNS_11643_1992_Plane_1_STR[] = "\x1B\x24\x29\x47";
స్థిర స్థిరరాశి అక్షరం CNS_11643_1992_Plane_2_STR[] = "\x1B\x24\x2A\x48";
స్థిర స్థిరరాశి అక్షరం CNS_11643_1992_Plane_3_STR[] = "\x1B\x24\x2B\x49";
స్థిర స్థిరరాశి అక్షరం CNS_11643_1992_Plane_4_STR[] = "\x1B\x24\x2B\x4A";
స్థిర స్థిరరాశి అక్షరం CNS_11643_1992_Plane_5_STR[] = "\x1B\x24\x2B\x4B";
స్థిర స్థిరరాశి అక్షరం CNS_11643_1992_Plane_6_STR[] = "\x1B\x24\x2B\x4C";
స్థిర స్థిరరాశి అక్షరం CNS_11643_1992_Plane_7_STR[] = "\x1B\x24\x2B\x4D";

/********************** ISO2022-CN Data **************************/
స్థిర స్థిరరాశి అక్షరం* స్థిరరాశి escSeqCharsCN[10] ={
        జరుపు_లోపల_STR,                   /* 0 ASCII */
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

స్థిర శూన్య U_CALLCONV
UConverter_fromUnicode_ISO_2022_CN_OFFSETS_LOGIC(UConverter_From_Unicode_వాదనలు* వాదనలు_2, UError_సంకేతం* err){
    UConverter *cnv = వాదనలు_2->converter;
    UConverter_దత్తాంశం_ISO2022 *converter_దత్తాంశం;
    ISO2022State *pFromU2022State;
    uint8_t* లక్ష్యం = reinterpret_cast<uint8_t*>(వాదనలు_2->లక్ష్యం);
    స్థిరరాశి uint8_t* లక్ష్యం_హద్దు = reinterpret_cast<స్థిరరాశి uint8_t*>(వాదనలు_2->లక్ష్యం_హద్దు);
    స్థిరరాశి char16_t* ఆధారం = వాదనలు_2->ఆధారం;
    స్థిరరాశి char16_t* ఆధారం_హద్దు = వాదనలు_2->ఆధారం_హద్దు;
    int32_t* offsets = వాదనలు_2->offsets;
    UChar32 ఆధారం_Char;
    అక్షరం బఫరు[8];
    int32_t len;
    int8_t choices[3];
    int32_t choice_లెక్క;
    uint32_t లక్ష్యం_విలువ = 0;
    UBool useFallback;

    /* set up the state */
    converter_దత్తాంశం = static_cast<UConverter_దత్తాంశం_ISO2022*>(cnv->extraInfo);
    pFromU2022State   = &converter_దత్తాంశం->fromU2022State;

    choice_లెక్క = 0;

    /* check if the last codepoint of previous buffer was a lead surrogate*/
    ఒకవేళ((ఆధారం_Char = cnv->fromUChar32)!=0 && లక్ష్యం< లక్ష్యం_హద్దు) {
        వెళ్ళు getTrail;
    }

    అయితే( ఆధారం < ఆధారం_హద్దు){
        ఒకవేళ(లక్ష్యం < లక్ష్యం_హద్దు){

            ఆధారం_Char  = *(ఆధారం++);
            /*check if the char is a First surrogate*/
             ఒకవేళ(U16_IS_SURROGATE(ఆధారం_Char)) {
                ఒకవేళ(U16_IS_SURROGATE_LEAD(ఆధారం_Char)) {
getTrail:
                    /*look ahead to find the trail surrogate*/
                    ఒకవేళ(ఆధారం < ఆధారం_హద్దు) {
                        /* test the following code unit */
                        char16_t trail = *ఆధారం;
                        ఒకవేళ(U16_IS_TRAIL(trail)) {
                            ఆధారం++;
                            ఆధారం_Char=U16_GET_SUPPLEMENTARY(ఆధారం_Char, trail);
                            cnv->fromUChar32=0x00;
                            /* convert this supplementary code point */
                            /* exit this condition tree */
                        } లేకపోతే {
                            /* this is an unmatched lead code unit (1st surrogate) */
                            /* callback(illegal) */
                            *err=U_ILLEGAL_CHAR_దొరికింది;
                            cnv->fromUChar32=ఆధారం_Char;
                            ఆపు;
                        }
                    } లేకపోతే {
                        /* no more input */
                        cnv->fromUChar32=ఆధారం_Char;
                        ఆపు;
                    }
                } లేకపోతే {
                    /* this is an unmatched trail code unit (2nd surrogate) */
                    /* callback(illegal) */
                    *err=U_ILLEGAL_CHAR_దొరికింది;
                    cnv->fromUChar32=ఆధారం_Char;
                    ఆపు;
                }
            }

            /* do the conversion */
            ఒకవేళ(ఆధారం_Char <= 0x007f ){
                /* do not convert SO/SI/ESC */
                ఒకవేళ(IS_2022_CONTROL(ఆధారం_Char)) {
                    /* callback(illegal) */
                    *err=U_ILLEGAL_CHAR_దొరికింది;
                    cnv->fromUChar32=ఆధారం_Char;
                    ఆపు;
                }

                /* US-ASCII */
                ఒకవేళ(pFromU2022State->g == 0) {
                    బఫరు[0] = static_cast<అక్షరం>(ఆధారం_Char);
                    len = 1;
                } లేకపోతే {
                    బఫరు[0] = UCNV_SI;
                    బఫరు[1] = static_cast<అక్షరం>(ఆధారం_Char);
                    len = 2;
                    pFromU2022State->g = 0;
                    choice_లెక్క = 0;
                }
                ఒకవేళ(ఆధారం_Char == CR || ఆధారం_Char == LF) {
                    /* reset the state at the end of a line */
                    uprv_memset(pFromU2022State, 0, పరిమాణం(ISO2022State));
                    choice_లెక్క = 0;
                }
            }
            లేకపోతే{
                /* convert U+0080..U+10ffff */
                int32_t i;
                int8_t cs, g;

                ఒకవేళ(choice_లెక్క == 0) {
                    /* try the current SO/G1 converter first */
                    choices[0] = pFromU2022State->cs[1];

                    /* default to GB2312_1 if none is designated yet */
                    ఒకవేళ(choices[0] == 0) {
                        choices[0] = GB2312_1;
                    }

                    ఒకవేళ(converter_దత్తాంశం->సంస్కరణ == 0) {
                        /* ISO-2022-CN */

                        /* try the other SO/G1 converter; a CNS_11643_1 lookup may result in any plane */
                        ఒకవేళ(choices[0] == GB2312_1) {
                            choices[1] = static_cast<int8_t>(CNS_11643_1);
                        } లేకపోతే {
                            choices[1] = static_cast<int8_t>(GB2312_1);
                        }

                        choice_లెక్క = 2;
                    } లేకపోతే ఒకవేళ (converter_దత్తాంశం->సంస్కరణ == 1) {
                        /* ISO-2022-CN-EXT */

                        /* try one of the other converters */
                        ఎంపిక(choices[0]) {
                        స్థితి GB2312_1:
                            choices[1] = static_cast<int8_t>(CNS_11643_1);
                            choices[2] = static_cast<int8_t>(ISO_IR_165);
                            ఆపు;
                        స్థితి ISO_IR_165:
                            choices[1] = static_cast<int8_t>(GB2312_1);
                            choices[2] = static_cast<int8_t>(CNS_11643_1);
                            ఆపు;
                        అప్రమేయ: /* CNS_11643_x */
                            choices[1] = static_cast<int8_t>(GB2312_1);
                            choices[2] = static_cast<int8_t>(ISO_IR_165);
                            ఆపు;
                        }

                        choice_లెక్క = 3;
                    } లేకపోతే {
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

                కొరకు(i = 0; i < choice_లెక్క && len <= 0; ++i) {
                    int8_t cs0 = choices[i];
                    ఒకవేళ(cs0 > 0) {
                        uint32_t value;
                        int32_t len2;
                        ఒకవేళ(cs0 >= CNS_11643_0) {
                            len2 = MBCS_FROM_UCHAR32_ISO2022(
                                        converter_దత్తాంశం->myConverterArray[CNS_11643],
                                        ఆధారం_Char,
                                        &value,
                                        useFallback,
                                        MBCS_OUTPUT_3);
                            ఒకవేళ(len2 == 3 || (len2 == -3 && len == 0)) {
                                లక్ష్యం_విలువ = value;
                                cs = static_cast<int8_t>(CNS_11643_0 + (value >> 16) - 0x80);
                                ఒకవేళ(len2 >= 0) {
                                    len = 2;
                                } లేకపోతే {
                                    len = -2;
                                    useFallback = అబద్ధం;
                                }
                                ఒకవేళ(cs == CNS_11643_1) {
                                    g = 1;
                                } లేకపోతే ఒకవేళ(cs == CNS_11643_2) {
                                    g = 2;
                                } లేకపోతే /* plane 3..7 */ ఒకవేళ(converter_దత్తాంశం->సంస్కరణ == 1) {
                                    g = 3;
                                } లేకపోతే {
                                    /* ISO-2022-CN (without -EXT) does not support plane 3..7 */
                                    len = 0;
                                }
                            }
                        } లేకపోతే {
                            /* GB2312_1 or ISO-IR-165 */
                            U_ASSERT(cs0<UCNV_2022_MAX_CONVERTERS);
                            len2 = MBCS_FROM_UCHAR32_ISO2022(
                                        converter_దత్తాంశం->myConverterArray[cs0],
                                        ఆధారం_Char,
                                        &value,
                                        useFallback,
                                        MBCS_OUTPUT_2);
                            ఒకవేళ(len2 == 2 || (len2 == -2 && len == 0)) {
                                లక్ష్యం_విలువ = value;
                                len = len2;
                                cs = cs0;
                                g = 1;
                                useFallback = అబద్ధం;
                            }
                        }
                    }
                }

                ఒకవేళ(len != 0) {
                    len = 0; /* count output bytes; it must have been abs(len) == 2 */

                    /* write the designation sequence if necessary */
                    ఒకవేళ(cs != pFromU2022State->cs[g]) {
                        ఒకవేళ(cs < CNS_11643) {
                            uprv_memcpy(బఫరు, escSeqCharsCN[cs], 4);
                        } లేకపోతే {
                            U_ASSERT(cs >= CNS_11643_1);
                            uprv_memcpy(బఫరు, escSeqCharsCN[CNS_11643 + (cs - CNS_11643_1)], 4);
                        }
                        len = 4;
                        pFromU2022State->cs[g] = cs;
                        ఒకవేళ(g == 1) {
                            /* changing the SO/G1 charset invalidates the choices[] */
                            choice_లెక్క = 0;
                        }
                    }

                    /* write the shift sequence if necessary */
                    ఒకవేళ(g != pFromU2022State->g) {
                        ఎంపిక(g) {
                        స్థితి 1:
                            బఫరు[len++] = UCNV_SO;

                            /* set the new state only if it is the locking shift SO/G1, not for SS2 or SS3 */
                            pFromU2022State->g = 1;
                            ఆపు;
                        స్థితి 2:
                            బఫరు[len++] = 0x1b;
                            బఫరు[len++] = 0x4e;
                            ఆపు;
                        అప్రమేయ: /* case 3 */
                            బఫరు[len++] = 0x1b;
                            బఫరు[len++] = 0x4f;
                            ఆపు;
                        }
                    }

                    /* write the two output bytes */
                    బఫరు[len++] = static_cast<అక్షరం>(లక్ష్యం_విలువ >> 8);
                    బఫరు[len++] = static_cast<అక్షరం>(లక్ష్యం_విలువ);
                } లేకపోతే {
                    /* if we cannot find the character after checking all codepages
                     * then this is an error
                     */
                    *err = U_INVALID_CHAR_దొరికింది;
                    cnv->fromUChar32=ఆధారం_Char;
                    ఆపు;
                }
            }

            /* output len>0 bytes in buffer[] */
            ఒకవేళ(len == 1) {
                *లక్ష్యం++ = బఫరు[0];
                ఒకవేళ(offsets) {
                    *offsets++ = static_cast<int32_t>(ఆధారం - వాదనలు_2->ఆధారం - 1); /* -1: known to be ASCII */
                }
            } లేకపోతే ఒకవేళ(len == 2 && (లక్ష్యం + 2) <= లక్ష్యం_హద్దు) {
                *లక్ష్యం++ = బఫరు[0];
                *లక్ష్యం++ = బఫరు[1];
                ఒకవేళ(offsets) {
                    int32_t ఆధారం_సూచీ = static_cast<int32_t>(ఆధారం - వాదనలు_2->ఆధారం - U16_నిడివి(ఆధారం_Char));
                    *offsets++ = ఆధారం_సూచీ;
                    *offsets++ = ఆధారం_సూచీ;
                }
            } లేకపోతే {
                fromUWriteUInt8(
                    cnv,
                    బఫరు, len,
                    &లక్ష్యం, reinterpret_cast<స్థిరరాశి అక్షరం*>(లక్ష్యం_హద్దు),
                    &offsets, static_cast<int32_t>(ఆధారం - వాదనలు_2->ఆధారం - U16_నిడివి(ఆధారం_Char)),
                    err);
                ఒకవేళ(U_FAILURE(*err)) {
                    ఆపు;
                }
            }
        } /* end if(myTargetIndex<myTargetLength) */
        లేకపోతే{
            *err =U_బఫరు_అధికప్రవాహం_ERROR;
            ఆపు;
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
    ఒకవేళ( U_SUCCESS(*err) &&
        pFromU2022State->g!=0 &&
        వాదనలు_2->flush && ఆధారం>=ఆధారం_హద్దు && cnv->fromUChar32==0
    ) {
        int32_t ఆధారం_సూచీ;

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
        ఆధారం_సూచీ = static_cast<int32_t>(ఆధారం - వాదనలు_2->ఆధారం);
        ఒకవేళ(ఆధారం_సూచీ>0) {
            --ఆధారం_సూచీ;
            ఒకవేళ( U16_IS_TRAIL(వాదనలు_2->ఆధారం[ఆధారం_సూచీ]) &&
                (ఆధారం_సూచీ==0 || U16_IS_LEAD(వాదనలు_2->ఆధారం[ఆధారం_సూచీ-1]))
            ) {
                --ఆధారం_సూచీ;
            }
        } లేకపోతే {
            ఆధారం_సూచీ=-1;
        }

        fromUWriteUInt8(
            cnv,
            జరుపు_లోపల_STR, 1,
            &లక్ష్యం, reinterpret_cast<స్థిరరాశి అక్షరం*>(లక్ష్యం_హద్దు),
            &offsets, ఆధారం_సూచీ,
            err);
    }

    /*save the state and return */
    వాదనలు_2->ఆధారం = ఆధారం;
    వాదనలు_2->లక్ష్యం = reinterpret_cast<అక్షరం*>(లక్ష్యం);
}


స్థిర శూన్య U_CALLCONV
UConverter_గమ్యం_Unicode_ISO_2022_CN_OFFSETS_LOGIC(UConverter_గమ్యం_Unicode_వాదనలు *వాదనలు_2,
                                               UError_సంకేతం* err){
    అక్షరం తాత్కాలిక_Buf[3];
    స్థిరరాశి అక్షరం* my_ఆధారం = const_cast<అక్షరం*>(వాదనలు_2->ఆధారం);
    char16_t *my_లక్ష్యం = వాదనలు_2->లక్ష్యం;
    స్థిరరాశి అక్షరం *my_ఆధారం_హద్దు = వాదనలు_2->ఆధారం_హద్దు;
    uint32_t లక్ష్యం_Uni_Char = 0x0000;
    uint32_t my_ఆధారం_Char = 0x0000;
    UConverter_దత్తాంశం_ISO2022* my_దత్తాంశం;
    ISO2022State *p_గమ్యం_U2022_State;

    my_దత్తాంశం = static_cast<UConverter_దత్తాంశం_ISO2022*>(వాదనలు_2->converter->extraInfo);
    p_గమ్యం_U2022_State = &my_దత్తాంశం->గమ్యం_U2022_State;

    ఒకవేళ(my_దత్తాంశం->కీలకం != 0) {
        /* continue with a partial escape sequence */
        వెళ్ళు escape;
    } లేకపోతే ఒకవేళ(వాదనలు_2->converter->గమ్యం_ULength == 1 && my_ఆధారం < my_ఆధారం_హద్దు && my_లక్ష్యం < వాదనలు_2->లక్ష్యం_హద్దు) {
        /* continue with a partial double-byte character */
        my_ఆధారం_Char = వాదనలు_2->converter->గమ్యం_UBytes[0];
        వాదనలు_2->converter->గమ్యం_ULength = 0;
        లక్ష్యం_Uni_Char = missingCharMarker;
        వెళ్ళు getTrailByte;
    }

    అయితే(my_ఆధారం < my_ఆధారం_హద్దు){

        లక్ష్యం_Uni_Char =missingCharMarker;

        ఒకవేళ(my_లక్ష్యం < వాదనలు_2->లక్ష్యం_హద్దు){

            my_ఆధారం_Char = static_cast<గుర్తులేని అక్షరం>(*my_ఆధారం++);

            ఎంపిక(my_ఆధారం_Char){
            స్థితి UCNV_SI:
                p_గమ్యం_U2022_State->g=0;
                ఒకవేళ (my_దత్తాంశం->is_ఖాళీలు_Segment) {
                    my_దత్తాంశం->is_ఖాళీలు_Segment = అబద్ధం;	/* we are handling it, reset to avoid future spurious errors */
                    *err = U_ILLEGAL_ESCAPE_SEQUENCE;
                    వాదనలు_2->converter->గమ్యం_UCallback_Reason = UCNV_IRREGULAR;
                    వాదనలు_2->converter->గమ్యం_UBytes[0] = static_cast<uint8_t>(my_ఆధారం_Char);
                    వాదనలు_2->converter->గమ్యం_ULength = 1;
                    వాదనలు_2->లక్ష్యం = my_లక్ష్యం;
                    వాదనలు_2->ఆధారం = my_ఆధారం;
                    తిరిగి;
                }
                కొనసాగు;

            స్థితి UCNV_SO:
                ఒకవేళ(p_గమ్యం_U2022_State->cs[1] != 0) {
                    p_గమ్యం_U2022_State->g=1;
                    my_దత్తాంశం->is_ఖాళీలు_Segment = నిజం;	/* Begin a new segment, empty so far */
                    కొనసాగు;
                } లేకపోతే {
                    /* illegal to have SO before a matching designator */
                    my_దత్తాంశం->is_ఖాళీలు_Segment = అబద్ధం;	/* Handling a different error, reset this to avoid future spurious errs */
                    ఆపు;
                }

            స్థితి ESC_2022:
                my_ఆధారం--;
escape:
                {
                    స్థిరరాశి అక్షరం * my_ఆధారం_Before = my_ఆధారం;
                    int8_t గమ్యం_ULength_Before = వాదనలు_2->converter->గమ్యం_ULength;

                    changeState_2022(వాదనలు_2->converter,&(my_ఆధారం),
                        my_ఆధారం_హద్దు, ISO_2022_CN,err);

                    /* After SO there must be at least one character before a designator (designator error handled separately) */
                    ఒకవేళ(my_దత్తాంశం->కీలకం==0 && U_SUCCESS(*err) && my_దత్తాంశం->is_ఖాళీలు_Segment) {
                        *err = U_ILLEGAL_ESCAPE_SEQUENCE;
                        వాదనలు_2->converter->గమ్యం_UCallback_Reason = UCNV_IRREGULAR;
                        వాదనలు_2->converter->గమ్యం_ULength = static_cast<int8_t>(గమ్యం_ULength_Before + (my_ఆధారం - my_ఆధారం_Before));
                    }
                }

                /* invalid or illegal escape sequence */
                ఒకవేళ(U_FAILURE(*err)){
                    వాదనలు_2->లక్ష్యం = my_లక్ష్యం;
                    వాదనలు_2->ఆధారం = my_ఆధారం;
                    my_దత్తాంశం->is_ఖాళీలు_Segment = అబద్ధం;	/* Reset to avoid future spurious errors */
                    తిరిగి;
                }
                కొనసాగు;

            /* ISO-2022-CN does not use single-byte (C1) SS2 and SS3 */

            స్థితి CR:
            స్థితి LF:
                uprv_memset(p_గమ్యం_U2022_State, 0, పరిమాణం(ISO2022State));
                U_FALLTHROUGH;
            అప్రమేయ:
                /* convert one or two bytes */
                my_దత్తాంశం->is_ఖాళీలు_Segment = అబద్ధం;
                ఒకవేళ(p_గమ్యం_U2022_State->g != 0) {
                    ఒకవేళ(my_ఆధారం < my_ఆధారం_హద్దు) {
                        UConverter_Shared_దత్తాంశం *cnv;
                        StateEnum తాత్కాలిక_State;
                        int32_t తాత్కాలిక_Buf_Len;
                        పూర్ణాంకం leadIsOk, trailIsOk;
                        uint8_t trailByte;
getTrailByte:
                        trailByte = static_cast<uint8_t>(*my_ఆధారం);
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
                        leadIsOk = static_cast<uint8_t>(my_ఆధారం_Char - 0x21) <= (0x7e - 0x21);
                        trailIsOk = static_cast<uint8_t>(trailByte - 0x21) <= (0x7e - 0x21);
                        ఒకవేళ (leadIsOk && trailIsOk) {
                            ++my_ఆధారం;
                            తాత్కాలిక_State = static_cast<StateEnum>(p_గమ్యం_U2022_State->cs[p_గమ్యం_U2022_State->g]);
                            ఒకవేళ(తాత్కాలిక_State >= CNS_11643_0) {
                                cnv = my_దత్తాంశం->myConverterArray[CNS_11643];
                                తాత్కాలిక_Buf[0] = static_cast<అక్షరం>(0x80 + (తాత్కాలిక_State - CNS_11643_0));
                                తాత్కాలిక_Buf[1] = static_cast<అక్షరం>(my_ఆధారం_Char);
                                తాత్కాలిక_Buf[2] = static_cast<అక్షరం>(trailByte);
                                తాత్కాలిక_Buf_Len = 3;

                            }లేకపోతే{
                                U_ASSERT(తాత్కాలిక_State<UCNV_2022_MAX_CONVERTERS);
                                cnv = my_దత్తాంశం->myConverterArray[తాత్కాలిక_State];
                                తాత్కాలిక_Buf[0] = static_cast<అక్షరం>(my_ఆధారం_Char);
                                తాత్కాలిక_Buf[1] = static_cast<అక్షరం>(trailByte);
                                తాత్కాలిక_Buf_Len = 2;
                            }
                            లక్ష్యం_Uni_Char = ucnv_MBCSSimple_Get_తదుపరి_UChar(cnv, తాత్కాలిక_Buf, తాత్కాలిక_Buf_Len, అబద్ధం);
                            my_ఆధారం_Char = (my_ఆధారం_Char << 8) | trailByte;
                        } లేకపోతే ఒకవేళ (!(trailIsOk || IS_2022_CONTROL(trailByte))) {
                            /* report a pair of illegal bytes if the second byte is not a DBCS starter */
                            ++my_ఆధారం;
                            /* add another bit so that the code below writes 2 bytes in case of error */
                            my_ఆధారం_Char = 0x10000 | (my_ఆధారం_Char << 8) | trailByte;
                        }
                        ఒకవేళ(p_గమ్యం_U2022_State->g>=2) {
                            /* return from a single-shift state to the previous one */
                            p_గమ్యం_U2022_State->g=p_గమ్యం_U2022_State->prevG;
                        }
                    } లేకపోతే {
                        వాదనలు_2->converter->గమ్యం_UBytes[0] = static_cast<uint8_t>(my_ఆధారం_Char);
                        వాదనలు_2->converter->గమ్యం_ULength = 1;
                        వెళ్ళు endloop;
                    }
                }
                లేకపోతే{
                    ఒకవేళ(my_ఆధారం_Char <= 0x7f) {
                        లక్ష్యం_Uni_Char = static_cast<char16_t>(my_ఆధారం_Char);
                    }
                }
                ఆపు;
            }
            ఒకవేళ(లక్ష్యం_Uni_Char < (missingCharMarker-1/*0xfffe*/)){
                ఒకవేళ(వాదనలు_2->offsets){
                    వాదనలు_2->offsets[my_లక్ష్యం - వాదనలు_2->లక్ష్యం] = static_cast<int32_t>(my_ఆధారం - వాదనలు_2->ఆధారం - (my_ఆధారం_Char <= 0xff ? 1 : 2));
                }
                *(my_లక్ష్యం++) = static_cast<char16_t>(లక్ష్యం_Uni_Char);
            }
            లేకపోతే ఒకవేళ(లక్ష్యం_Uni_Char > missingCharMarker){
                /* disassemble the surrogate pair and write to output*/
                లక్ష్యం_Uni_Char-=0x0010000;
                *my_లక్ష్యం = static_cast<char16_t>(0xd800 + static_cast<char16_t>(లక్ష్యం_Uni_Char >> 10));
                ఒకవేళ(వాదనలు_2->offsets){
                    వాదనలు_2->offsets[my_లక్ష్యం - వాదనలు_2->లక్ష్యం] = static_cast<int32_t>(my_ఆధారం - వాదనలు_2->ఆధారం - (my_ఆధారం_Char <= 0xff ? 1 : 2));
                }
                ++my_లక్ష్యం;
                ఒకవేళ(my_లక్ష్యం< వాదనలు_2->లక్ష్యం_హద్దు){
                    *my_లక్ష్యం = static_cast<char16_t>(0xdc00 + static_cast<char16_t>(లక్ష్యం_Uni_Char & 0x3ff));
                    ఒకవేళ(వాదనలు_2->offsets){
                        వాదనలు_2->offsets[my_లక్ష్యం - వాదనలు_2->లక్ష్యం] = static_cast<int32_t>(my_ఆధారం - వాదనలు_2->ఆధారం - (my_ఆధారం_Char <= 0xff ? 1 : 2));
                    }
                    ++my_లక్ష్యం;
                }లేకపోతే{
                    వాదనలు_2->converter->UChar_Error_బఫరు[వాదనలు_2->converter->UChar_Error_బఫరు_నిడివి++]=
                                    static_cast<char16_t>(0xdc00 + static_cast<char16_t>(లక్ష్యం_Uni_Char & 0x3ff));
                }

            }
            లేకపోతే{
                /* Call the callback function*/
                గమ్యం_Unicode_Callback(వాదనలు_2->converter,my_ఆధారం_Char,లక్ష్యం_Uni_Char,err);
                ఆపు;
            }
        }
        లేకపోతే{
            *err =U_బఫరు_అధికప్రవాహం_ERROR;
            ఆపు;
        }
    }
endloop:
    వాదనలు_2->లక్ష్యం = my_లక్ష్యం;
    వాదనలు_2->ఆధారం = my_ఆధారం;
}
#అయితే_ముగింపు /* #if !UCONFIG_ONLY_HTML_CONVERSION */

స్థిర శూన్య U_CALLCONV
_ISO_2022_WriteSub(UConverter_From_Unicode_వాదనలు *వాదనలు_2, int32_t స్థానభ్రంశం_సూచీ, UError_సంకేతం *err) {
    UConverter *cnv = వాదనలు_2->converter;
    UConverter_దత్తాంశం_ISO2022* my_Converter_దత్తాంశం = static_cast<UConverter_దత్తాంశం_ISO2022*>(cnv->extraInfo);
    ISO2022State *pFromU2022State=&my_Converter_దత్తాంశం->fromU2022State;
    అక్షరం *p, *subchar;
    అక్షరం బఫరు[8];
    int32_t length;

    subchar = reinterpret_cast<అక్షరం*>(cnv->subChars);
    length=cnv->subCharLen; /* assume length==1 for most variants */

    p = బఫరు;
    ఎంపిక(my_Converter_దత్తాంశం->locale[0]){
    స్థితి 'j':
        {
            int8_t cs;

            ఒకవేళ(pFromU2022State->g == 1) {
                /* JIS7: switch from G1 to G0 */
                pFromU2022State->g = 0;
                *p++ = UCNV_SI;
            }

            cs = pFromU2022State->cs[0];
            ఒకవేళ(cs != ASCII && cs != JISX201) {
                /* not in ASCII or JIS X 0201: switch to ASCII */
                pFromU2022State->cs[0] = static_cast<int8_t>(ASCII);
                *p++ = '\x1b';
                *p++ = '\x28';
                *p++ = '\x42';
            }

            *p++ = subchar[0];
            ఆపు;
        }
    స్థితి 'c':
        ఒకవేళ(pFromU2022State->g != 0) {
            /* not in ASCII mode: switch to ASCII */
            pFromU2022State->g = 0;
            *p++ = UCNV_SI;
        }
        *p++ = subchar[0];
        ఆపు;
    స్థితి 'k':
        ఒకవేళ(my_Converter_దత్తాంశం->సంస్కరణ == 0) {
            ఒకవేళ(length == 1) {
                ఒకవేళ(వాదనలు_2->converter->from_Unicode_స్థితి) {
                    /* in DBCS mode: switch to SBCS */
                    వాదనలు_2->converter->from_Unicode_స్థితి = 0;
                    *p++ = UCNV_SI;
                }
                *p++ = subchar[0];
            } లేకపోతే /* length == 2*/ {
                ఒకవేళ(!వాదనలు_2->converter->from_Unicode_స్థితి) {
                    /* in SBCS mode: switch to DBCS */
                    వాదనలు_2->converter->from_Unicode_స్థితి = 1;
                    *p++ = UCNV_SO;
                }
                *p++ = subchar[0];
                *p++ = subchar[1];
            }
            ఆపు;
        } లేకపోతే {
            /* save the subconverter's substitution string */
            uint8_t *currentSubChars = my_Converter_దత్తాంశం->currentConverter->subChars;
            int8_t currentSubCharLen = my_Converter_దత్తాంశం->currentConverter->subCharLen;

            /* set our substitution string into the subconverter */
            my_Converter_దత్తాంశం->currentConverter->subChars = reinterpret_cast<uint8_t*>(subchar);
            my_Converter_దత్తాంశం->currentConverter->subCharLen = static_cast<int8_t>(length);

            /* let the subconverter write the subchar, set/retrieve fromUChar32 state */
            వాదనలు_2->converter = my_Converter_దత్తాంశం->currentConverter;
            my_Converter_దత్తాంశం->currentConverter->fromUChar32 = cnv->fromUChar32;
            ucnv_cbFromUWriteSub(వాదనలు_2, 0, err);
            cnv->fromUChar32 = my_Converter_దత్తాంశం->currentConverter->fromUChar32;
            వాదనలు_2->converter = cnv;

            /* restore the subconverter's substitution string */
            my_Converter_దత్తాంశం->currentConverter->subChars = currentSubChars;
            my_Converter_దత్తాంశం->currentConverter->subCharLen = currentSubCharLen;

            ఒకవేళ(*err == U_బఫరు_అధికప్రవాహం_ERROR) {
                ఒకవేళ(my_Converter_దత్తాంశం->currentConverter->char_Error_బఫరు_నిడివి > 0) {
                    uprv_memcpy(
                        cnv->char_Error_బఫరు,
                        my_Converter_దత్తాంశం->currentConverter->char_Error_బఫరు,
                        my_Converter_దత్తాంశం->currentConverter->char_Error_బఫరు_నిడివి);
                }
                cnv->char_Error_బఫరు_నిడివి = my_Converter_దత్తాంశం->currentConverter->char_Error_బఫరు_నిడివి;
                my_Converter_దత్తాంశం->currentConverter->char_Error_బఫరు_నిడివి = 0;
            }
            తిరిగి;
        }
    అప్రమేయ:
        /* not expected */
        ఆపు;
    }
    ucnv_cb_From_UWrite_బైట్లు(వాదనలు_2,
                           బఫరు, static_cast<int32_t>(p - బఫరు),
                           స్థానభ్రంశం_సూచీ, err);
}

/*
 * Structure for cloning an ISO 2022 converter into a single memory block.
 */
నిర్మాణం cloneStruct
{
    UConverter cnv;
    UConverter currentConverter;
    UConverter_దత్తాంశం_ISO2022 mydata;
};


U_CDECL_BEGIN

స్థిర UConverter * U_CALLCONV
ISO_2022_సురక్షిత_Clone(
            స్థిరరాశి UConverter *cnv,
            శూన్య *దొంతర_బఫరు,
            int32_t *p_బఫరు_కొలత,
            UError_సంకేతం *స్థితి_2)
{
    నిర్మాణం cloneStruct * localClone;
    UConverter_దత్తాంశం_ISO2022 *cnv_దత్తాంశం;
    int32_t i, size;

    ఒకవేళ (U_FAILURE(*స్థితి_2)){
        తిరిగి శూన్యసూచిక;
    }

    ఒకవేళ (*p_బఫరు_కొలత == 0) { /* 'preflighting' request - set needed size into *pBufferSize */
        *p_బఫరు_కొలత = (int32_t)పరిమాణం(నిర్మాణం cloneStruct);
        తిరిగి శూన్యసూచిక;
    }

    cnv_దత్తాంశం = (UConverter_దత్తాంశం_ISO2022 *)cnv->extraInfo;
    localClone = (నిర్మాణం cloneStruct *)దొంతర_బఫరు;

    /* ucnv.c/ucnv_safeClone() copied the main UConverter already */

    uprv_memcpy(&localClone->mydata, cnv_దత్తాంశం, పరిమాణం(UConverter_దత్తాంశం_ISO2022));
    localClone->cnv.extraInfo = &localClone->mydata; /* set pointer to extra data */
    localClone->cnv.isExtraLocal = నిజం;

    /* share the subconverters */

    ఒకవేళ(cnv_దత్తాంశం->currentConverter != శూన్యసూచిక) {
        size = (int32_t)పరిమాణం(UConverter);
        localClone->mydata.currentConverter =
            ucnv_సురక్షిత_Clone(cnv_దత్తాంశం->currentConverter,
                            &localClone->currentConverter,
                            &size, స్థితి_2);
        ఒకవేళ(U_FAILURE(*స్థితి_2)) {
            తిరిగి శూన్యసూచిక;
        }
    }

    కొరకు(i=0; i<UCNV_2022_MAX_CONVERTERS; ++i) {
        ఒకవేళ(cnv_దత్తాంశం->myConverterArray[i] != శూన్యసూచిక) {
            ucnv_increment_Ref_లెక్క(cnv_దత్తాంశం->myConverterArray[i]);
        }
    }

    తిరిగి &localClone->cnv;
}

U_CDECL_చివర

స్థిర శూన్య U_CALLCONV
ISO_2022_Get_Unicode_సమితి(స్థిరరాశి UConverter *cnv,
                    స్థిరరాశి USetAdder *sa,
                    UConverter_Unicode_సమితి which,
                    UError_సంకేతం *p_Error_సంకేతం)
{
    int32_t i;
    UConverter_దత్తాంశం_ISO2022* cnv_దత్తాంశం;

    ఒకవేళ (U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి;
    }
#నిర్వచితమైతే U_ENABLE_GENERIC_ISO_2022
    ఒకవేళ (cnv->shared_దత్తాంశం == &ISO2022_దత్తాంశం) {
        /* We use UTF-8 in this case */
        sa->కూడు_Range(sa->సమితి, 0, 0xd7FF);
        sa->కూడు_Range(sa->సమితి, 0xE000, 0x10FFFF);
        తిరిగి;
    }
#అయితే_ముగింపు

    cnv_దత్తాంశం = static_cast<UConverter_దత్తాంశం_ISO2022*>(cnv->extraInfo);

    /* open a set and initialize it with code points that are algorithmically round-tripped */
    ఎంపిక(cnv_దత్తాంశం->locale[0]){
    స్థితి 'j':
        /* include JIS X 0201 which is hardcoded */
        sa->కూడు(sa->సమితి, 0xa5);
        sa->కూడు(sa->సమితి, 0x203e);
        ఒకవేళ(jpCharsetMasks[cnv_దత్తాంశం->సంస్కరణ]&CSM(ISO8859_1)) {
            /* include Latin-1 for some variants of JP */
            sa->కూడు_Range(sa->సమితి, 0, 0xff);
        } లేకపోతే {
            /* include ASCII for JP */
            sa->కూడు_Range(sa->సమితి, 0, 0x7f);
        }
        ఒకవేళ(cnv_దత్తాంశం->సంస్కరణ==3 || cnv_దత్తాంశం->సంస్కరణ==4 || which==UCNV_ROUNDTRIP_AND_FALLBACK_సమితి) {
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
            sa->కూడు_Range(sa->సమితి, HWKANA_ప్రారంభం, HWKANA_చివర);
        }
        ఆపు;
#ఒకవేళ !UCONFIG_ONLY_HTML_CONVERSION
    స్థితి 'c':
    స్థితి 'z':
        /* include ASCII for CN */
        sa->కూడు_Range(sa->సమితి, 0, 0x7f);
        ఆపు;
    స్థితి 'k':
        /* there is only one converter for KR, and it is not in the myConverterArray[] */
        cnv_దత్తాంశం->currentConverter->shared_దత్తాంశం->impl->get_Unicode_సమితి(
                cnv_దత్తాంశం->currentConverter, sa, which, p_Error_సంకేతం);
        /* the loop over myConverterArray[] will simply not find another converter */
        ఆపు;
#అయితే_ముగింపు
    అప్రమేయ:
        ఆపు;
    }

#ఒకవేళ 0  /* Replaced by ucnv_MBCSGetFilteredUnicodeSetForUnicode() until we implement ucnv_getUnicodeSet() with reverse fallbacks. */
            ఒకవేళ( (cnv_దత్తాంశం->locale[0]=='c' || cnv_దత్తాంశం->locale[0]=='z') &&
                cnv_దత్తాంశం->సంస్కరణ==0 && i==CNS_11643
            ) {
                /* special handling for non-EXT ISO-2022-CN: add only code points for CNS planes 1 and 2 */
                ucnv_MBCSGet_Unicode_సమితి_For_బైట్లు(
                        cnv_దత్తాంశం->myConverterArray[i],
                        sa, UCNV_ROUNDTRIP_సమితి,
                        0, 0x81, 0x82,
                        p_Error_సంకేతం);
            }
#అయితే_ముగింపు

    కొరకు (i=0; i<UCNV_2022_MAX_CONVERTERS; i++) {
        UConverter_సమితి_Filter filter;
        ఒకవేళ(cnv_దత్తాంశం->myConverterArray[i]!=శూన్యసూచిక) {
            ఒకవేళ(cnv_దత్తాంశం->locale[0]=='j' && i==JISX208) {
                /*
                 * Only add code points that map to Shift-JIS codes
                 * corresponding to JIS X 0208.
                 */
                filter=UCNV_సమితి_FILTER_SJIS;
#ఒకవేళ !UCONFIG_ONLY_HTML_CONVERSION
            } లేకపోతే ఒకవేళ( (cnv_దత్తాంశం->locale[0]=='c' || cnv_దత్తాంశం->locale[0]=='z') &&
                       cnv_దత్తాంశం->సంస్కరణ==0 && i==CNS_11643) {
                /*
                 * Version-specific for CN:
                 * CN version 0 does not map CNS planes 3..7 although
                 * they are all available in the CNS conversion table;
                 * CN version 1 (-EXT) does map them all.
                 * The two versions create different Unicode sets.
                 */
                filter=UCNV_సమితి_FILTER_2022_CN;
            } లేకపోతే ఒకవేళ(i==KSC5601) {
                /*
                 * Some of the KSC 5601 tables (convrtrs.txt has this aliases on multiple tables)
                 * are broader than GR94.
                 */
                filter=UCNV_సమితి_FILTER_GR94_DBCS;
#అయితే_ముగింపు
            } లేకపోతే {
                filter=UCNV_సమితి_FILTER_NONE;
            }
            ucnv_MBCSGet_Filtered_Unicode_సమితి_For_Unicode(cnv_దత్తాంశం->myConverterArray[i], sa, which, filter, p_Error_సంకేతం);
        }
    }

    /*
     * ISO 2022 converters must not convert SO/SI/ESC despite what
     * sub-converters do by themselves.
     * Remove these characters from the set.
     */
    sa->remove(sa->సమితి, 0x0e);
    sa->remove(sa->సమితి, 0x0f);
    sa->remove(sa->సమితి, 0x1b);

    /* ISO 2022 converters do not convert C1 controls either */
    sa->removeRange(sa->సమితి, 0x80, 0x9f);
}

స్థిర స్థిరరాశి UConverterImpl _ISO2022Impl={
    UCNV_ISO_2022,

    శూన్యసూచిక,
    శూన్యసూచిక,

    _ISO2022Open,
    _ISO2022Close,
    _ISO2022Reset,

#నిర్వచితమైతే U_ENABLE_GENERIC_ISO_2022
    T_UConverter_గమ్యం_Unicode_ISO_2022_OFFSETS_LOGIC,
    T_UConverter_గమ్యం_Unicode_ISO_2022_OFFSETS_LOGIC,
    ucnv_fromUnicode_UTF8,
    ucnv_fromUnicode_UTF8_OFFSETS_LOGIC,
#లేకపోతే
    శూన్యసూచిక,
    శూన్యసూచిక,
    శూన్యసూచిక,
    శూన్యసూచిక,
#అయితే_ముగింపు
    శూన్యసూచిక,

    శూన్యసూచిక,
    _ISO2022getName,
    _ISO_2022_WriteSub,
    ISO_2022_సురక్షిత_Clone,
    ISO_2022_Get_Unicode_సమితి,

    శూన్యసూచిక,
    శూన్యసూచిక
};
స్థిర స్థిరరాశి UConverter_Static_దత్తాంశం ISO2022_Static_దత్తాంశం={
    పరిమాణం(UConverter_Static_దత్తాంశం),
    "ISO_2022",
    2022,
    UCNV_IBM,
    UCNV_ISO_2022,
    1,
    3, /* max 3 bytes per char16_t from UTF-8 (4 bytes from surrogate _pair_) */
    { 0x1a, 0, 0, 0 },
    1,
    అబద్ధం,
    అబద్ధం,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};
స్థిరరాశి UConverter_Shared_దత్తాంశం ISO2022_దత్తాంశం=
        UCNV_IMMUTABLE_SHARED_దత్తాంశం_INITIALIZER(&ISO2022_Static_దత్తాంశం, &_ISO2022Impl);

/*************JP****************/
స్థిర స్థిరరాశి UConverterImpl _ISO2022JPImpl={
    UCNV_ISO_2022,

    శూన్యసూచిక,
    శూన్యసూచిక,

    _ISO2022Open,
    _ISO2022Close,
    _ISO2022Reset,

    UConverter_గమ్యం_Unicode_ISO_2022_JP_OFFSETS_LOGIC,
    UConverter_గమ్యం_Unicode_ISO_2022_JP_OFFSETS_LOGIC,
    UConverter_fromUnicode_ISO_2022_JP_OFFSETS_LOGIC,
    UConverter_fromUnicode_ISO_2022_JP_OFFSETS_LOGIC,
    శూన్యసూచిక,

    శూన్యసూచిక,
    _ISO2022getName,
    _ISO_2022_WriteSub,
    ISO_2022_సురక్షిత_Clone,
    ISO_2022_Get_Unicode_సమితి,

    శూన్యసూచిక,
    శూన్యసూచిక
};
స్థిర స్థిరరాశి UConverter_Static_దత్తాంశం ISO2022_JPStatic_దత్తాంశం={
    పరిమాణం(UConverter_Static_దత్తాంశం),
    "ISO_2022_JP",
    0,
    UCNV_IBM,
    UCNV_ISO_2022,
    1,
    6, /* max 6 bytes per char16_t: 4-byte escape sequence + DBCS */
    { 0x1a, 0, 0, 0 },
    1,
    అబద్ధం,
    అబద్ధం,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

నామాకాశ {

స్థిరరాశి UConverter_Shared_దత్తాంశం _ISO2022JPData=
        UCNV_IMMUTABLE_SHARED_దత్తాంశం_INITIALIZER(&ISO2022_JPStatic_దత్తాంశం, &_ISO2022JPImpl);

}  // namespace

#ఒకవేళ !UCONFIG_ONLY_HTML_CONVERSION
/************* KR ***************/
స్థిర స్థిరరాశి UConverterImpl _ISO2022KRImpl={
    UCNV_ISO_2022,

    శూన్యసూచిక,
    శూన్యసూచిక,

    _ISO2022Open,
    _ISO2022Close,
    _ISO2022Reset,

    UConverter_గమ్యం_Unicode_ISO_2022_KR_OFFSETS_LOGIC,
    UConverter_గమ్యం_Unicode_ISO_2022_KR_OFFSETS_LOGIC,
    UConverter_fromUnicode_ISO_2022_KR_OFFSETS_LOGIC,
    UConverter_fromUnicode_ISO_2022_KR_OFFSETS_LOGIC,
    శూన్యసూచిక,

    శూన్యసూచిక,
    _ISO2022getName,
    _ISO_2022_WriteSub,
    ISO_2022_సురక్షిత_Clone,
    ISO_2022_Get_Unicode_సమితి,

    శూన్యసూచిక,
    శూన్యసూచిక
};
స్థిర స్థిరరాశి UConverter_Static_దత్తాంశం ISO2022_KRStatic_దత్తాంశం={
    పరిమాణం(UConverter_Static_దత్తాంశం),
    "ISO_2022_KR",
    0,
    UCNV_IBM,
    UCNV_ISO_2022,
    1,
    8, /* max 8 bytes per char16_t */
    { 0x1a, 0, 0, 0 },
    1,
    అబద్ధం,
    అబద్ధం,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

నామాకాశ {

స్థిరరాశి UConverter_Shared_దత్తాంశం _ISO2022KRData=
        UCNV_IMMUTABLE_SHARED_దత్తాంశం_INITIALIZER(&ISO2022_KRStatic_దత్తాంశం, &_ISO2022KRImpl);

}  // namespace

/*************** CN ***************/
స్థిర స్థిరరాశి UConverterImpl _ISO2022CNImpl={

    UCNV_ISO_2022,

    శూన్యసూచిక,
    శూన్యసూచిక,

    _ISO2022Open,
    _ISO2022Close,
    _ISO2022Reset,

    UConverter_గమ్యం_Unicode_ISO_2022_CN_OFFSETS_LOGIC,
    UConverter_గమ్యం_Unicode_ISO_2022_CN_OFFSETS_LOGIC,
    UConverter_fromUnicode_ISO_2022_CN_OFFSETS_LOGIC,
    UConverter_fromUnicode_ISO_2022_CN_OFFSETS_LOGIC,
    శూన్యసూచిక,

    శూన్యసూచిక,
    _ISO2022getName,
    _ISO_2022_WriteSub,
    ISO_2022_సురక్షిత_Clone,
    ISO_2022_Get_Unicode_సమితి,

    శూన్యసూచిక,
    శూన్యసూచిక
};
స్థిర స్థిరరాశి UConverter_Static_దత్తాంశం ISO2022_CNStatic_దత్తాంశం={
    పరిమాణం(UConverter_Static_దత్తాంశం),
    "ISO_2022_CN",
    0,
    UCNV_IBM,
    UCNV_ISO_2022,
    1,
    8, /* max 8 bytes per char16_t: 4-byte CNS designator + 2 bytes for SS2/SS3 + DBCS */
    { 0x1a, 0, 0, 0 },
    1,
    అబద్ధం,
    అబద్ధం,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

నామాకాశ {

స్థిరరాశి UConverter_Shared_దత్తాంశం _ISO2022CNData=
        UCNV_IMMUTABLE_SHARED_దత్తాంశం_INITIALIZER(&ISO2022_CNStatic_దత్తాంశం, &_ISO2022CNImpl);

}  // namespace
#అయితే_ముగింపు /* #if !UCONFIG_ONLY_HTML_CONVERSION */

#అయితే_ముగింపు /* #if !UCONFIG_NO_LEGACY_CONVERSION */
