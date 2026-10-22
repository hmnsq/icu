// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*
*   Copyright (C) 2000-2013, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
******************************************************************************
*   file name:  ucnvmbcs.h
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2000jul07
*   created by: Markus W. Scherer
*/

#اگر_تعریف_نشده __UCNVMBCS_H__
#تعریف_کن __UCNVMBCS_H__

#ضمن "unicode/utypes.h"

#إذا !UCONFIG_NO_CONVERSION

#ضمن "unicode/ucnv.h"
#ضمن "ucnv_cnv.h"
#ضمن "ucnv_ext.h"

/**
 * ICU conversion (.cnv) data file structure, following the usual UDataInfo
 * header.
 *
 * Format version: 6.2
 *
 * struct UConverterStaticData -- struct containing the converter name, IBM CCSID,
 *                                min/max bytes per character, etc.
 *                                see ucnv_bld.h
 *
 * --------------------
 *
 * The static data is followed by conversionType-specific data structures.
 * At the moment, there are only variations of MBCS converters. They all have
 * the same toUnicode structures, while the fromUnicode structures for SBCS
 * differ from those for other MBCS-style converters.
 *
 * _MBCSHeader.version 5 is optional and not backward-compatible
 * (as usual for changes in the major version field).
 *
 * Versions 5.m work like versions 4.m except:
 * - The _MBCSHeader has variable length (and is always longer than in version 4).
 *   See the struct _MBCSHeader further description below.
 * - There is a set of flags which indicate further incompatible changes.
 *   (Reader code must reject the file if it does not recognize them all.)
 * - In particular, one of these flags indicates that most of the fromUnicode
 *   data is missing and must be reconstituted from the toUnicode data
 *   and from the utf8Friendly mbcsIndex at load time.
 *   (This only works with a utf8Friendly table.)
 *   In this case, makeconv may increase maxFastUChar automatically to U+FFFF.
 *
 * The first of these versions is 5.3, which is like 4.3 except for the differences above.
 *
 * When possible, makeconv continues to generate version 4.m files.
 *
 * _MBCSHeader.version 5.4/4.4 supports "good one-way" mappings (|4)
 * in the extension tables (fromUTableValues bit 30). See ucnv_ext.h for details.
 *
 * _MBCSHeader.version 4.3 optionally modifies the fromUnicode data structures
 * slightly and optionally adds a table for conversion to MBCS (non-SBCS)
 * charsets.
 *
 * The modifications are to make the data utf8Friendly. Not every 4.3 file
 * file contains utf8Friendly data.
 * It is utf8Friendly if _MBCSHeader.version[2]!=0.
 * In this case, the data structures are utf8Friendly up to the code point
 *   maxFastUChar=((_MBCSHeader.version[2]<<8)|0xff)
 *
 * A utf8Friendly file has fromUnicode stage 3 entries for code points up to
 * maxFastUChar allocated in blocks of 64 for indexing with the 6 bits from
 * a UTF-8 trail byte. ASCII is allocated linearly with 128 contiguous entries.
 *
 * In addition, a utf8Friendly MBCS file contains an additional
 *   uint16_t mbcsIndex[(maxFastUChar+1)>>6];
 * which replaces the stage 1 and 2 tables for indexing with bits from the
 * UTF-8 lead byte and middle trail byte. Unlike the older MBCS stage 2 table,
 * the mbcsIndex does not contain roundtrip flags. Therefore, all fallbacks
 * from code points up to maxFastUChar (and roundtrips to 0x00) are moved to
 * the extension data structure. This also allows for faster roundtrip
 * conversion from UTF-16.
 *
 * SBCS files do not contain an additional sbcsIndex[] array because the
 * proportional size increase would be noticeable, but the runtime
 * code builds one for the code point range for which the runtime conversion
 * code is optimized.
 *
 * For SBCS, maxFastUChar should be at least U+0FFF. The initial makeconv
 * implementation sets it to U+1FFF. Because the sbcsIndex is not stored in
 * the file, a larger maxFastUChar only affects stage 3 block allocation size
 * and is free in empty blocks. (Larger blocks with sparse contents cause larger
 * files.) U+1FFF includes almost all of the small scripts.
 * U+0FFF covers UTF-8 two-byte sequences and three-byte sequences starting with
 * 0xe0. This includes most scripts with legacy SBCS charsets.
 * The initial runtime implementation using 4.3 files only builds an sbcsIndex
 * for code points up to U+0FFF.
 *
 * For MBCS, maxFastUChar should be at least U+D7FF (=initial value).
 * This boundary is convenient because practically all of the commonly used
 * characters are below it, and because it is the boundary to surrogate
 * code points, above which special handling is necessary anyway.
 * (Surrogate pair assembly for UTF-16, validity checking for UTF-8.)
 *
 * maxFastUChar could be up to U+FFFF to cover the whole BMP, which could be
 * useful especially for conversion from UTF-8 when the input can be assumed
 * to be valid, because the surrogate range would then not have to be
 * checked.
 * (With maxFastUChar=0xffff, makeconv would have to check for mbcsIndex value
 * overflow because with the all-unassigned block 0 and nearly full mappings
 * from the BMP it is theoretically possible that an index into stage 3
 * exceeds 16 bits.)
 *
 * _MBCSHeader.version 4.2 adds an optional conversion extension data structure.
 * If it is present, then an ICU version reading header versions 4.0 or 4.1
 * will be able to use the base table and ignore the extension.
 *
 * The unicodeMask in the static data is part of the base table data structure.
 * Especially, the UCNV_HAS_SUPPLEMENTARY flag determines the length of the
 * fromUnicode stage 1 array.
 * The static data unicodeMask refers only to the base table's properties if
 * a base table is included.
 * In an extension-only file, the static data unicodeMask is 0.
 * The extension data indexes have a separate field with the unicodeMask flags.
 *
 * MBCS-style data structure following the static data.
 * Offsets are counted in bytes from the beginning of the MBCS header structure.
 * Details about usage in comments in ucnvmbcs.c.
 *
 * struct _MBCSHeader (see the definition in this header file below)
 * contains 32-bit fields as follows:
 * 8 values:
 *  0   uint8_t[4]  MBCS version in UVersionInfo format (currently 4.3.x.0)
 *  1   uint32_t    countStates
 *  2   uint32_t    countToUFallbacks
 *  3   uint32_t    offsetToUCodeUnits
 *  4   uint32_t    offsetFromUTable
 *  5   uint32_t    offsetFromUBytes
 *  6   uint32_t    flags, bits:
 *                      31.. 8 offsetExtension -- _MBCSHeader.version 4.2 (ICU 2.8) and higher
 *                                                0 for older versions and if
 *                                                there is not extension structure
 *                       7.. 0 outputType
 *  7   uint32_t    fromUBytesLength -- _MBCSHeader.version 4.1 (ICU 2.4) and higher
 *                  counts bytes in fromUBytes[]
 *
 * New and required in version 5:
 *  8   uint32_t    options, bits:
 *                      31..16 reserved for flags that can be added without breaking
 *                                 backward compatibility
 *                      15.. 6 reserved for flags whose addition will break
 *                                 backward compatibility
 *                           6 MBCS_OPT_FROM_U -- if set,
 *                                 then most of the fromUnicode data is omitted;
 *                                 fullStage2Length is present and the missing
 *                                 bottom part of stage 2 must be reconstituted from
 *                                 the toUnicode data;
 *                                 stage 3 is missing completely as well;
 *                                 not used for SBCS tables
 *                       5.. 0 length of the _MBCSHeader (number of uint32_t)
 *
 * New and optional in version 5:
 *  9   uint32_t    fullStage2Length: used if MBCS_OPT_FROM_U is set
 *                                 specifies the full length of stage 2
 *                                 including the omitted part
 *
 * if(outputType==MBCS_OUTPUT_EXT_ONLY) {
 *     -- base table name for extension-only table
 *     char baseTableName[variable]; -- with NUL plus padding for 4-alignment
 *
 *     -- all _MBCSHeader fields except for version and flags are 0
 * } else {
 *     -- normal base table with optional extension
 *
 *     int32_t stateTable[countStates][256];
 *    
 *     struct _MBCSToUFallback { (fallbacks are sorted by offset)
 *         uint32_t offset;
 *         UChar32 codePoint;
 *     } toUFallbacks[countToUFallbacks];
 *    
 *     uint16_t unicodeCodeUnits[(offsetFromUTable-offsetToUCodeUnits)/2];
 *                  (padded to an even number of units)
 *    
 *     -- stage 1 tables
 *     if(staticData.unicodeMask&UCNV_HAS_SUPPLEMENTARY) {
 *         -- stage 1 table for all of Unicode
 *         uint16_t fromUTable[0x440]; (32-bit-aligned)
 *     } else {
 *         -- BMP-only tables have a smaller stage 1 table
 *         uint16_t fromUTable[0x40]; (32-bit-aligned)
 *     }
 *
 *     -- stage 2 tables
 *        length determined by top of stage 1 and bottom of stage 3 tables
 *     if(outputType==MBCS_OUTPUT_1) {
 *         -- SBCS: pure indexes
 *         uint16_t stage 2 indexes[?];
 *     } else {
 *         -- DBCS, MBCS, EBCDIC_STATEFUL, ...: roundtrip flags and indexes
 *         uint32_t stage 2 flags and indexes[?];
 *         if(options&MBCS_OPT_NO_FROM_U) {
 *             stage 2 really has length fullStage2Length
 *             and the omitted lower part must be reconstituted from
 *             the toUnicode data
 *         }
 *     }
 *
 *     -- stage 3 tables with byte results
 *     if(outputType==MBCS_OUTPUT_1) {
 *         -- SBCS: each 16-bit result contains flags and the result byte, see ucnvmbcs.c
 *         uint16_t fromUBytes[fromUBytesLength/2];
 *     } else if(!(options&MBCS_OPT_NO_FROM_U)) {
 *         -- DBCS, MBCS, EBCDIC_STATEFUL, ... 2/3/4 bytes result, see ucnvmbcs.c
 *         uint8_t fromUBytes[fromUBytesLength]; or
 *         uint16_t fromUBytes[fromUBytesLength/2]; or
 *         uint32_t fromUBytes[fromUBytesLength/4];
 *     } else {
 *         fromUBytes[] must be reconstituted from the toUnicode data
 *     }
 *
 *     -- optional utf8Friendly mbcsIndex -- _MBCSHeader.version 4.3 (ICU 3.8) and higher
 *     if(outputType!=MBCS_OUTPUT_1 &&
 *        _MBCSHeader.version[1]>=3 &&
 *        (maxFastUChar=_MBCSHeader.version[2])!=0
 *     ) {
 *         maxFastUChar=(maxFastUChar<<8)|0xff;
 *         uint16_t mbcsIndex[(maxFastUChar+1)>>6];
 *     }
 * }
 *
 * -- extension table, details see ucnv_ext.h
 * int32_t indexes[>=32]; ...
 */

/* MBCS converter data and state -------------------------------------------- */

تعداد {
    MBCS_MAX_STATE_شمار=128
};

/**
 * MBCS action codes for conversions to Unicode.
 * These values are in bits 23..20 of the state table entries.
 */
تعداد {
    MBCS_STATE_VALID_مستقیم_16,
    MBCS_STATE_VALID_مستقیم_20,

    MBCS_STATE_FALLBACK_مستقیم_16,
    MBCS_STATE_FALLBACK_مستقیم_20,

    MBCS_STATE_VALID_16,
    MBCS_STATE_VALID_16_PAIR,

    MBCS_STATE_UNASSIGNED,
    MBCS_STATE_ILLEGAL,

    MBCS_STATE_CHANGE_ONLY
};

/* Macros for state table entries */
#تعریف_کن MBCS_مدخل_TRANSITION(state, جابه_جایی) (int32_t)(((int32_t)(state)<<24L)|(جابه_جایی))
#تعریف_کن MBCS_مدخل_TRANSITION_مجموعه_جابه_جایی(مدخل, جابه_جایی) (int32_t)(((مدخل)&0xff000000)|(جابه_جایی))
#تعریف_کن MBCS_مدخل_TRANSITION_جمع_کن_جابه_جایی(مدخل, جابه_جایی) (int32_t)((مدخل)+(جابه_جایی))

#تعریف_کن MBCS_مدخل_FINAL(state, action, مقدار_2) (int32_t)(0x80000000|((int32_t)(state)<<24L)|((action)<<20L)|(مقدار_2))
#تعریف_کن MBCS_مدخل_مجموعه_FINAL(مدخل) (int32_t)((مدخل)|0x80000000)
#تعریف_کن MBCS_مدخل_FINAL_مجموعه_ACTION(مدخل, action) (int32_t)(((مدخل)&0xff0fffff)|((int32_t)(action)<<20L))
#تعریف_کن MBCS_مدخل_FINAL_مجموعه_مقدار(مدخل, مقدار_2) (int32_t)(((مدخل)&0xfff00000)|(مقدار_2))
#تعریف_کن MBCS_مدخل_FINAL_مجموعه_ACTION_مقدار(مدخل, action, مقدار_2) (int32_t)(((مدخل)&0xff000000)|((int32_t)(action)<<20L)|(مقدار_2))

#تعریف_کن MBCS_مدخل_مجموعه_STATE(مدخل, state) (int32_t)(((مدخل)&0x80ffffff)|((int32_t)(state)<<24L))

#تعریف_کن MBCS_مدخل_STATE(مدخل) ((((uint32_t)مدخل)>>24)&0x7f)

#تعریف_کن MBCS_مدخل_IS_TRANSITION(مدخل) ((مدخل)>=0)
#تعریف_کن MBCS_مدخل_IS_FINAL(مدخل) ((مدخل)<0)

#تعریف_کن MBCS_مدخل_TRANSITION_STATE(مدخل) (((uint32_t)مدخل)>>24)
#تعریف_کن MBCS_مدخل_TRANSITION_جابه_جایی(مدخل) ((مدخل)&0xffffff)

#تعریف_کن MBCS_مدخل_FINAL_STATE(مدخل) ((((uint32_t)مدخل)>>24)&0x7f)
#تعریف_کن MBCS_مدخل_FINAL_IS_VALID_مستقیم_16(مدخل) ((مدخل)<(int32_t)0x80100000)
#تعریف_کن MBCS_مدخل_FINAL_ACTION(مدخل) ((((uint32_t)مدخل)>>20)&0xf)
#تعریف_کن MBCS_مدخل_FINAL_مقدار(مدخل) ((مدخل)&0xfffff)
#تعریف_کن MBCS_مدخل_FINAL_مقدار_16(مدخل) (uint16_t)(مدخل)

#تعریف_کن IS_ASCII_ROUNDTRIP(b, asciiRoundtrips) (((asciiRoundtrips) & (1<<((b)>>2)))!=0)

/* single-byte fromUnicode: get the 16-bit result word */
#تعریف_کن MBCS_SINGLE_نتیجه_FROM_U(جدول, results, c) (results)[ (جدول)[ (جدول)[(c)>>10] +(((c)>>4)&0x3f) ] +((c)&0xf) ]

/* single-byte fromUnicode using the sbcsIndex */
#تعریف_کن SBCS_نتیجه_FROM_کم_BMP(جدول, results, c) (results)[ (جدول)[(c)>>6] +((c)&0x3f) ]

/* single-byte fromUTF8 using the sbcsIndex; l and t must be masked externally; can be l=0 and t<=0x7f */
#تعریف_کن SBCS_نتیجه_FROM_UTF8(جدول, results, l, t) (results)[ (جدول)[l] +(t) ]

/* multi-byte fromUnicode: get the 32-bit stage 2 entry */
#تعریف_کن MBCS_STAGE_2_FROM_U(جدول, c) ((ثابت uint32_t *)(جدول))[ (جدول)[(c)>>10] +(((c)>>4)&0x3f) ]
#تعریف_کن MBCS_FROM_U_IS_ROUNDTRIP(stage2_مدخل, c) ( ((stage2_مدخل) & ((uint32_t)1<< (16+((c)&0xf)) )) !=0)

#تعریف_کن MBCS_مقدار_2_FROM_STAGE_2(بایتها, stage2_مدخل, c) ((uint16_t *)(بایتها))[16*(uint32_t)(uint16_t)(stage2_مدخل)+((c)&0xf)]
#تعریف_کن MBCS_مقدار_4_FROM_STAGE_2(بایتها, stage2_مدخل, c) ((uint32_t *)(بایتها))[16*(uint32_t)(uint16_t)(stage2_مدخل)+((c)&0xf)]

#تعریف_کن MBCS_اشاره_گر_3_FROM_STAGE_2(بایتها, stage2_مدخل, c) ((بایتها)+(16*(uint32_t)(uint16_t)(stage2_مدخل)+((c)&0xf))*3)

/* double-byte fromUnicode using the mbcsIndex */
#تعریف_کن DBCS_نتیجه_FROM_MOST_BMP(جدول, results, c) (results)[ (جدول)[(c)>>6] +((c)&0x3f) ]

/* double-byte fromUTF8 using the mbcsIndex; l and t1 combined into lt1; lt1 and t2 must be masked externally */
#تعریف_کن DBCS_نتیجه_FROM_UTF8(جدول, results, lt1, t2) (results)[ (جدول)[lt1] +(t2) ]


/**
 * MBCS output types for conversions from Unicode.
 * These per-converter types determine the storage method in stage 3 of the lookup table,
 * mostly how many bytes are stored per entry.
 */
تعداد {
    MBCS_OUTPUT_1,          /* 0 */
    MBCS_OUTPUT_2,          /* 1 */
    MBCS_OUTPUT_3,          /* 2 */
    MBCS_OUTPUT_4,          /* 3 */

    MBCS_OUTPUT_3_EUC=8,    /* 8 */
    MBCS_OUTPUT_4_EUC,      /* 9 */

    MBCS_OUTPUT_2_SISO=12,  /* c */
    MBCS_OUTPUT_2_HZ,       /* d */

    MBCS_OUTPUT_EXT_ONLY,   /* e */

    MBCS_OUTPUT_شمار,

    MBCS_OUTPUT_DBCS_ONLY=0xdb  /* runtime-only type for DBCS-only handling of SISO tables */
};

/**
 * Fallbacks to Unicode are stored outside the normal state table and code point structures
 * in a vector of items of this type. They are sorted by offset.
 */
تعريفنوع بنية {
    uint32_t جابه_جایی;
    UChar32 کد_نقطه;
} _MBCSToUFallback;

/** Constants for fast and UTF-8-friendly conversion. */
تعداد {
    SBCS_FAST_MAX=0x0fff,               /* maximum code point with UTF-8-friendly SBCS runtime code, see makeconv SBCS_UTF8_MAX */
    SBCS_FAST_حد=SBCS_FAST_MAX+1,    /* =0x1000 */
    MBCS_FAST_MAX=0xd7ff,               /* maximum code point with UTF-8-friendly MBCS runtime code, see makeconv MBCS_UTF8_MAX */
    MBCS_FAST_حد=MBCS_FAST_MAX+1     /* =0xd800 */
};

/**
 * This is the MBCS part of the UConverterTable union (a runtime data structure).
 * It keeps all the per-converter data and points into the loaded mapping tables.
 *
 * utf8Friendly data structures added with _MBCSHeader.version 4.3
 */
تعريفنوع بنية UConverterMBCSTable {
    /* toUnicode */
    uint8_t شمار_حالتها, dbcsOnlyState, state_جدول_Owned;
    uint32_t شمار_مقصد_UFallbacks;

    ثابت int32_t (*state_جدول)/*[countStates]*/[256];
    int32_t (*جابجا_LFNLState_جدول)/*[countStates]*/[256]; /* for swaplfnl */
    ثابت uint16_t *unicode_کد_Units/*[countUnicodeResults]*/;
    ثابت _MBCSToUFallback *مقصد_UFallbacks;

    /* fromUnicode */
    ثابت uint16_t *from_Unicode_جدول;
    ثابت uint16_t *mbcs_شاخص;              /* for fast conversion from most of BMP to MBCS (utf8Friendly data) */
    uint16_t sbcs_شاخص[SBCS_FAST_حد>>6]; /* for fast conversion from low BMP to SBCS (utf8Friendly data) */
    ثابت uint8_t *from_Unicode_بایتها;
    uint8_t *جابجا_LFNLFrom_Unicode_بایتها;      /* for swaplfnl */
    uint32_t from_UBytes_درازی;
    uint8_t outputType, unicode_ماسک;
    UBool utf8Friendly;                     /* for utf8Friendly data */
    UChar maxFastUChar;                     /* for utf8Friendly data */

    /* roundtrips */
    uint32_t asciiRoundtrips;

    /* reconstituted data that was omitted from the .cnv file */
    uint8_t *reconstituted_داده;

    /* converter name for swaplfnl */
    حرف *جابجا_LFNLName;

    /* extension data */
    بنية UConverter_Shared_داده *پایه_Shared_داده;
    ثابت int32_t *extIndexes;
} UConverterMBCSTable;

#تعریف_کن UCNV_MBCS_جدول_INITIALIZER { \
    /* toUnicode */ \
    0, 0, 0, \
    0, \
     \
    عدم, \
    عدم, \
    عدم, \
    عدم, \
     \
    /* fromUnicode */ \
    عدم, \
    عدم, \
    { 0 }, \
    عدم, \
    عدم, \
    0, \
    0, 0, \
    false, \
    0, \
     \
    /* roundtrips */ \
    0, \
     \
    /* reconstituted data that was omitted from the .cnv file */ \
    عدم, \
     \
    /* converter name for swaplfnl */ \
    عدم, \
     \
    /* extension data */ \
    عدم, \
    عدم \
}

تعداد {
    MBCS_OPT_درازی_ماسک=0x3f,
    MBCS_OPT_NO_FROM_U=0x40,
    /*
     * If any of the following options bits are set,
     * then the file must be rejected.
     */
    MBCS_OPT_INCOMPATIBLE_ماسک=0xffc0,
    /*
     * Remove bits from this mask as more options are recognized
     * by all implementations that use this constant.
     */
    MBCS_OPT_UNKNOWN_INCOMPATIBLE_ماسک=0xff80
};

تعداد {
    MBCS_سرآیند_V4_درازی=8,
    MBCS_سرآیند_V5_MIN_درازی=9
};

/**
 * MBCS data header. See data format description above.
 */
تعريفنوع بنية {
    UVersionInfo نسخه;
    uint32_t شمار_حالتها,
             شمار_مقصد_UFallbacks,
             جابه_جایی_مقصد_UCode_Units,
             جابه_جایی_From_UTable,
             جابه_جایی_From_UBytes,
             flags,
             from_UBytes_درازی;

    /* new and required in version 5 */
    uint32_t options;

    /* new and optional in version 5; used if options&MBCS_OPT_NO_FROM_U */
    uint32_t پر_Stage2_درازی;  /* number of 32-bit units */
} _MBCSHeader;

#تعریف_کن UCNV_MBCS_سرآیند_INITIALIZER { { 0 },  0, 0, 0, 0, 0, 0, 0,  0,  0 }

/*
 * This is a simple version of _MBCSGetNextUChar() that is used
 * by other converter implementations.
 * It only returns an "assigned" result if it consumes the entire input.
 * It does not use state from the converter, nor error codes.
 * It does not handle the EBCDIC swaplfnl option (set in UConverter).
 * It handles conversion extensions but not GB 18030.
 *
 * Return value:
 * U+fffe   unassigned
 * U+ffff   illegal
 * otherwise the Unicode code point
 */
U_CFUNC UChar32
ucnv_MBCSSimple_Get_بعدی_UChar(UConverter_Shared_داده *shared_داده,
                        ثابت حرف *منبع, int32_t درازی,
                        UBool useFallback);

/**
 * This version of _MBCSSimpleGetNextUChar() is optimized for single-byte, single-state codepages.
 * It does not handle the EBCDIC swaplfnl option (set in UConverter).
 * It does not handle conversion extensions (_extToU()).
 */
U_CFUNC UChar32
ucnv_MBCSSingle_Simple_Get_بعدی_UChar(UConverter_Shared_داده *shared_داده,
                              uint8_t b, UBool useFallback);

/**
 * This macro version of _MBCSSingleSimpleGetNextUChar() gets a code point from a byte.
 * It works for single-byte, single-state codepages that only map
 * to and from BMP code points, and it always
 * returns fallback values.
 */
#تعریف_کن MBCS_SINGLE_SIMPLE_GET_بعدی_BMP(shared_داده, b) \
    (UChar)MBCS_مدخل_FINAL_مقدار_16((shared_داده)->mbcs.state_جدول[0][(uint8_t)(b)])

/**
 * This is an internal function that allows other converter implementations
 * to check whether a byte is a lead byte.
 */
U_CFUNC UBool
ucnv_MBCSIsLeadByte(UConverter_Shared_داده *shared_داده, حرف byte);

/** This is a macro version of _MBCSIsLeadByte(). */
#تعریف_کن _MBCS_IS_LEAD_BYTE(shared_داده, byte) \
    (UBool)MBCS_مدخل_IS_TRANSITION((shared_داده)->mbcs.state_جدول[0][(uint8_t)(byte)])

/*
 * This is another simple conversion function for internal use by other
 * conversion implementations.
 * It does not use the converter state nor call callbacks.
 * It does not handle the EBCDIC swaplfnl option (set in UConverter).
 * It handles conversion extensions but not GB 18030.
 *
 * It converts one single Unicode code point into codepage bytes, encoded
 * as one 32-bit value. The function returns the number of bytes in *pValue:
 * 1..4 the number of bytes in *pValue
 * 0    unassigned (*pValue undefined)
 * -1   illegal (currently not used, *pValue undefined)
 *
 * *pValue will contain the resulting bytes with the last byte in bits 7..0,
 * the second to last byte in bits 15..8, etc.
 * Currently, the function assumes but does not check that 0<=c<=0x10ffff.
 */
U_CFUNC int32_t
ucnv_MBCSFromUChar32(UConverter_Shared_داده *shared_داده,
                 UChar32 c, uint32_t *p_مقدار,
                 UBool useFallback);

/**
 * This version of _MBCSFromUChar32() is optimized for single-byte codepages.
 * It does not handle the EBCDIC swaplfnl option (set in UConverter).
 *
 * It returns the codepage byte for the code point, or -1 if it is unassigned.
 */
U_CFUNC int32_t
ucnv_MBCSSingleFromUChar32(UConverter_Shared_داده *shared_داده,
                       UChar32 c,
                       UBool useFallback);

/**
 * SBCS, DBCS, and EBCDIC_STATEFUL are replaced by MBCS, but
 * we cheat a little about the type, returning the old types if appropriate.
 */
U_CFUNC UConverterType
ucnv_MBCSGetType(ثابت UConverter* converter);

U_CFUNC فراغ 
ucnv_MBCSFromUnicodeWithOffsets(UConverter_From_Unicode_آرگومانها *p_آرگومانها,
                            UError_کد *p_Error_کد);
U_CFUNC فراغ 
ucnv_MBCSToUnicodeWithOffsets(UConverter_مقصد_Unicode_آرگومانها *p_آرگومانها,
                          UError_کد *p_Error_کد);

/*
 * Internal function returning a UnicodeSet for toUnicode() conversion.
 * Currently only used for ISO-2022-CN, and only handles roundtrip mappings.
 * In the future, if we add support for fallback sets, this function
 * needs to be updated.
 * Handles extensions.
 * Does not empty the set first.
 */
U_CFUNC فراغ
ucnv_MBCSGet_Unicode_مجموعه_For_Unicode(ثابت UConverter_Shared_داده *shared_داده,
                                 ثابت USetAdder *sa,
                                 UConverter_Unicode_مجموعه which,
                                 UError_کد *p_Error_کد);

/*
 * Same as ucnv_MBCSGetUnicodeSetForUnicode() but
 * the set can be filtered by encoding scheme.
 * Used by stateful converters which share regular conversion tables
 * but only use a subset of their mappings.
 */
U_CFUNC فراغ
ucnv_MBCSGet_Filtered_Unicode_مجموعه_For_Unicode(ثابت UConverter_Shared_داده *shared_داده,
                                         ثابت USetAdder *sa,
                                         UConverter_Unicode_مجموعه which,
                                         UConverter_مجموعه_Filter filter,
                                         UError_کد *p_Error_کد);

#پایان_اگر

#پایان_اگر
