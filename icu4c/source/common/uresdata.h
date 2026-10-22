// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
* Copyright (C) 1999-2016, International Business Machines
*                Corporation and others. All Rights Reserved.
******************************************************************************
*   file name:  uresdata.h
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 1999dec08
*   created by: Markus W. Scherer
*   06/24/02    weiv        Added support for resource sharing
*/

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __RESDATA_H__
#ವ್ಯಾಖ್ಯಾನಿಸು __RESDATA_H__

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/udata.h"
#ಅನ್ತರ್ಭಾವ "unicode/ures.h"
#ಅನ್ತರ್ಭಾವ "putilimp.h"
#ಅನ್ತರ್ಭಾವ "udataswp.h"

/**
 * Numeric constants for internal-only types of resource items.
 * These must use different numeric values than UResType constants
 * because they are used together.
 * Internal types are never returned by ures_getType().
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
    /** Include a negative value so that the compiler uses the same int type as for UResType. */
    URES_ಆಂತರಿಕ_NONE=-1,

    /** Resource type constant for tables with 32-bit count, key offsets and values. */
    URES_TABLE32=4,

    /**
     * Resource type constant for tables with 16-bit count, key offsets and values.
     * All values are URES_STRING_V2 strings.
     */
    URES_TABLE16=5,

    /** Resource type constant for 16-bit Unicode strings in formatVersion 2. */
    URES_STRING_V2=6,

    /**
     * Resource type constant for arrays with 16-bit count and values.
     * All values are URES_STRING_V2 strings.
     */
    URES_ARRAY16=9

    /* Resource type 15 is not defined but effectively used by RES_BOGUS=0xffffffff. */
} URes_ಆಂತರಿಕ_Type;

/*
 * A Resource is a 32-bit value that has 2 bit fields:
 * 31..28   4-bit type, see enum below
 * 27..0    28-bit four-byte-offset or value according to the type
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ uint32_t Resource;

#ವ್ಯಾಖ್ಯಾನಿಸು RES_BOGUS 0xffffffff
#ವ್ಯಾಖ್ಯಾನಿಸು RES_MAX_ಸ್ಥಾನಾಂತರ 0x0fffffff

#ವ್ಯಾಖ್ಯಾನಿಸು RES_GET_TYPE(res) ((int32_t)((res)>>28UL))
#ವ್ಯಾಖ್ಯಾನಿಸು RES_GET_ಸ್ಥಾನಾಂತರ(res) ((res)&0x0fffffff)
#ವ್ಯಾಖ್ಯಾನಿಸು RES_GET_ಸೂಚಕ(p_ಬೇರು, res) ((p_ಬೇರು)+RES_GET_ಸ್ಥಾನಾಂತರ(res))

/* get signed and unsigned integer values directly from the Resource handle
 * NOTE: For proper logging, please use the res_getInt() constexpr
 */
#ಆದರೆ U_SIGNED_ಬಲ_ಸರಿಸು_IS_ARITHMETIC
#   define RES_GET_INT_NO_TRACE(res) (((int32_t)((res)<<4L))>>4L)
#ಇಲ್ಲದಿದ್ದರೆ
#   define RES_GET_INT_NO_TRACE(res) (int32_t)(((res)&0x08000000) ? (res)|0xf0000000 : (res)&0x07ffffff)
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು RES_GET_UINT_NO_TRACE(res) ((res)&0x0fffffff)

#ವ್ಯಾಖ್ಯಾನಿಸು URES_IS_ARRAY(type) ((int32_t)(type)==URES_ARRAY || (int32_t)(type)==URES_ARRAY16)
#ವ್ಯಾಖ್ಯಾನಿಸು URES_IS_ಕೋಷ್ಟಕ(type) ((int32_t)(type)==URES_ಕೋಷ್ಟಕ || (int32_t)(type)==URES_TABLE16 || (int32_t)(type)==URES_TABLE32)
#ವ್ಯಾಖ್ಯಾನಿಸು URES_IS_CONTAINER(type) (URES_IS_ಕೋಷ್ಟಕ(type) || URES_IS_ARRAY(type))

#ವ್ಯಾಖ್ಯಾನಿಸು URES_MAKE_RESOURCE(type, ಸ್ಥಾನಾಂತರ) (((Resource)(type)<<28)|(Resource)(ಸ್ಥಾನಾಂತರ))
#ವ್ಯಾಖ್ಯಾನಿಸು URES_MAKE_ಖಾಲಿಗಳು_RESOURCE(type) ((Resource)(type)<<28)

/* indexes[] value names; indexes are generally 32-bit (Resource) indexes */
ಎಣಿಕೆ {
    /**
     * [0] contains the length of indexes[]
     * which is at most URES_INDEX_TOP of the latest format version
     *
     * formatVersion==1: all bits contain the length of indexes[]
     *   but the length is much less than 0xff;
     * formatVersion>1:
     *   only bits  7..0 contain the length of indexes[],
     *        bits 31..8 are reserved and set to 0
     * formatVersion>=3:
     *        bits 31..8 poolStringIndexLimit bits 23..0
     */
    URES_ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ,
    /**
     * [1] contains the top of the key strings,
     *     same as the bottom of resources or UTF-16 strings, rounded up
     */
    URES_ಸೂಚ್ಯಂಕ_ಕೀಲಿಗಳು_ತುದಿ,
    /** [2] contains the top of all resources */
    URES_ಸೂಚ್ಯಂಕ_RESOURCES_ತುದಿ,
    /**
     * [3] contains the top of the bundle,
     *     in case it were ever different from [2]
     */
    URES_ಸೂಚ್ಯಂಕ_BUNDLE_ತುದಿ,
    /** [4] max. length of any table */
    URES_ಸೂಚ್ಯಂಕ_MAX_ಕೋಷ್ಟಕ_ಉದ್ದಳತೆ,
    /**
     * [5] attributes bit set, see URES_ATT_* (new in formatVersion 1.2)
     *
     * formatVersion>=3:
     *   bits 31..16 poolStringIndex16Limit
     *   bits 15..12 poolStringIndexLimit bits 27..24
     */
    URES_ಸೂಚ್ಯಂಕ_ಗುಣಲಕ್ಷಣಗಳು,
    /**
     * [6] top of the 16-bit units (UTF-16 string v2 UChars, URES_TABLE16, URES_ARRAY16),
     *     rounded up (new in formatVersion 2.0, ICU 4.4)
     */
    URES_ಸೂಚ್ಯಂಕ_16_ಬಿಟ್_ತುದಿ,
    /** [7] checksum of the pool bundle (new in formatVersion 2.0, ICU 4.4) */
    URES_ಸೂಚ್ಯಂಕ_POOL_CHECKSUM,
    URES_ಸೂಚ್ಯಂಕ_ತುದಿ
};

/*
 * Nofallback attribute, attribute bit 0 in indexes[URES_INDEX_ATTRIBUTES].
 * New in formatVersion 1.2 (ICU 3.6).
 *
 * If set, then this resource bundle is a standalone bundle.
 * If not set, then the bundle participates in locale fallback, eventually
 * all the way to the root bundle.
 * If indexes[] is missing or too short, then the attribute cannot be determined
 * reliably. Dependency checking should ignore such bundles, and loading should
 * use fallbacks.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು URES_ATT_NO_FALLBACK 1

/*
 * Attributes for bundles that are, or use, a pool bundle.
 * A pool bundle provides key strings that are shared among several other bundles
 * to reduce their total size.
 * New in formatVersion 2 (ICU 4.4).
 */
#ವ್ಯಾಖ್ಯಾನಿಸು URES_ATT_IS_POOL_BUNDLE 2
#ವ್ಯಾಖ್ಯಾನಿಸು URES_ATT_ಬಳಸುತ್ತದೆ_POOL_BUNDLE 4

/*
 * File format for .res resource bundle files
 *
 * ICU 56: New in formatVersion 3 compared with 2: -------------
 *
 * Resource bundles can optionally use shared string-v2 values
 * stored in the pool bundle.
 * If so, then the indexes[] contain two new values
 * in previously-unused bits of existing indexes[] slots:
 * - poolStringIndexLimit:
 *     String-v2 offsets (in 32-bit Resource words) below this limit
 *     point to pool bundle string-v2 values.
 * - poolStringIndex16Limit:
 *     Resource16 string-v2 offsets below this limit
 *     point to pool bundle string-v2 values.
 * Guarantee: poolStringIndex16Limit <= poolStringIndexLimit
 *
 * The local bundle's poolStringIndexLimit is greater than
 * any pool bundle string index used in the local bundle.
 * The poolStringIndexLimit should not be greater than
 * the maximum possible pool bundle string index.
 *
 * The maximum possible pool bundle string index is the index to the last non-NUL
 * pool string character, due to suffix sharing.
 *
 * In the pool bundle, there is no structure that lists the strings.
 * (The root resource is an empty Table.)
 * If the strings need to be enumerated (as genrb --usePoolBundle does),
 * then iterate through the pool bundle's 16-bit-units array from the beginning.
 * Stop at the end of the array, or when an explicit or implicit string length
 * would lead beyond the end of the array,
 * or when an apparent string is not NUL-terminated.
 * (Future genrb version might terminate the strings with
 * what looks like a large explicit string length.)
 *
 * ICU 4.4: New in formatVersion 2 compared with 1.3: -------------
 *
 * Three new resource types -- String-v2, Table16 and Array16 -- have their
 * values stored in a new array of 16-bit units between the table key strings
 * and the start of the other resources.
 *
 * genrb eliminates duplicates among Unicode string-v2 values.
 * Multiple Unicode strings may use the same offset and string data,
 * or a short string may point to the suffix of a longer string. ("Suffix sharing")
 * For example, one string "abc" may be reused for another string "bc" by pointing
 * to the second character. (Short strings-v2 are NUL-terminated
 * and not preceded by an explicit length value.)
 *
 * It is allowed for all resource types to share values.
 * The swapper code (ures_swap()) has been modified so that it swaps each item
 * exactly once.
 *
 * A resource bundle may use a special pool bundle. Some or all of the table key strings
 * of the using-bundle are omitted, and the key string offsets for such key strings refer
 * to offsets in the pool bundle.
 * The using-bundle's and the pool-bundle's indexes[URES_INDEX_POOL_CHECKSUM] values
 * must match.
 * Two bits in indexes[URES_INDEX_ATTRIBUTES] indicate whether a resource bundle
 * is or uses a pool bundle.
 *
 * Table key strings must be compared in ASCII order, even if they are not
 * stored in ASCII.
 *
 * New in formatVersion 1.3 compared with 1.2: -------------
 *
 * genrb eliminates duplicates among key strings.
 * Multiple table items may share one key string, or one item may point
 * to the suffix of another's key string. ("Suffix sharing")
 * For example, one key "abc" may be reused for another key "bc" by pointing
 * to the second character. (Key strings are NUL-terminated.)
 *
 * -------------
 *
 * An ICU4C resource bundle file (.res) is a binary, memory-mappable file
 * with nested, hierarchical data structures.
 * It physically contains the following:
 *
 *   Resource root; -- 32-bit Resource item, root item for this bundle's tree;
 *                     currently, the root item must be a table or table32 resource item
 *   int32_t indexes[indexes[0]]; -- array of indexes for friendly
 *                                   reading and swapping; see URES_INDEX_* above
 *                                   new in formatVersion 1.1 (ICU 2.8)
 *   char keys[]; -- characters for key strings
 *                   (formatVersion 1.0: up to 65k of characters; 1.1: <2G)
 *                   (minus the space for root and indexes[]),
 *                   which consist of invariant characters (ASCII/EBCDIC) and are NUL-terminated;
 *                   padded to multiple of 4 bytes for 4-alignment of the following data
 *   uint16_t 16BitUnits[]; -- resources that are stored entirely as sequences of 16-bit units
 *                             (new in formatVersion 2/ICU 4.4)
 *                             data is indexed by the offset values in 16-bit resource types,
 *                             with offset 0 pointing to the beginning of this array;
 *                             there is a 0 at offset 0, for empty resources;
 *                             padded to multiple of 4 bytes for 4-alignment of the following data
 *   data; -- data directly and indirectly indexed by the root item;
 *            the structure is determined by walking the tree
 *
 * Each resource bundle item has a 32-bit Resource handle (see typedef above)
 * which contains the item type number in its upper 4 bits (31..28) and either
 * an offset or a direct value in its lower 28 bits (27..0).
 * The order of items is undefined and only determined by walking the tree.
 * Leaves of the tree may be stored first or last or anywhere in between,
 * and it is in theory possible to have unreferenced holes in the file.
 *
 * 16-bit-unit values:
 * Starting with formatVersion 2/ICU 4.4, some resources are stored in a special
 * array of 16-bit units. Each resource value is a sequence of 16-bit units,
 * with no per-resource padding to a 4-byte boundary.
 * 16-bit container types (Table16 and Array16) contain Resource16 values
 * which are offsets to String-v2 resources in the same 16-bit-units array.
 *
 * Direct values:
 * - Empty Unicode strings have an offset value of 0 in the Resource handle itself.
 * - Starting with formatVersion 2/ICU 4.4, an offset value of 0 for
 *   _any_ resource type indicates an empty value.
 * - Integer values are 28-bit values stored in the Resource handle itself;
 *   the interpretation of unsigned vs. signed integers is up to the application.
 *
 * All other types and values use 28-bit offsets to point to the item's data.
 * The offset is an index to the first 32-bit word of the value, relative to the
 * start of the resource data (i.e., the root item handle is at offset 0).
 * To get byte offsets, the offset is multiplied by 4 (or shifted left by 2 bits).
 * All resource item values are 4-aligned.
 *
 * New in formatVersion 2/ICU 4.4: Some types use offsets into the 16-bit-units array,
 * indexing 16-bit units in that array.
 *
 * The structures (memory layouts) for the values for each item type are listed
 * in the table below.
 *
 * Nested, hierarchical structures: -------------
 *
 * Table items contain key-value pairs where the keys are offsets to char * key strings.
 * The values of these pairs are either Resource handles or
 * offsets into the 16-bit-units array, depending on the table type.
 *
 * Array items are simple vectors of Resource handles,
 * or of offsets into the 16-bit-units array, depending on the array type.
 *
 * Table key string offsets: -------
 *
 * Key string offsets are relative to the start of the resource data (of the root handle),
 * i.e., the first string has an offset of 4+sizeof(indexes).
 * (After the 4-byte root handle and after the indexes array.)
 *
 * If the resource bundle uses a pool bundle, then some key strings are stored
 * in the pool bundle rather than in the local bundle itself.
 * - In a Table or Table16, the 16-bit key string offset is local if it is
 *   less than indexes[URES_INDEX_KEYS_TOP]<<2.
 *   Otherwise, subtract indexes[URES_INDEX_KEYS_TOP]<<2 to get the offset into
 *   the pool bundle key strings.
 * - In a Table32, the 32-bit key string offset is local if it is non-negative.
 *   Otherwise, reset bit 31 to get the pool key string offset.
 *
 * Unlike the local offset, the pool key offset is relative to
 * the start of the key strings, not to the start of the bundle.
 *
 * An alias item is special (and new in ICU 2.4): --------------
 *
 * Its memory layout is just like for a UnicodeString, but at runtime it resolves to
 * another resource bundle's item according to the path in the string.
 * This is used to share items across bundles that are in different lookup/fallback
 * chains (e.g., large collation data among zh_TW and zh_HK).
 * This saves space (for large items) and maintenance effort (less duplication of data).
 *
 * --------------------------------------------------------------------------
 *
 * Resource types:
 *
 * Most resources have their values stored at four-byte offsets from the start
 * of the resource data. These values are at least 4-aligned.
 * Some resource values are stored directly in the offset field of the Resource itself.
 * See UResType in unicode/ures.h for enumeration constants for Resource types.
 *
 * Some resources have their values stored as sequences of 16-bit units,
 * at 2-byte offsets from the start of a contiguous 16-bit-unit array between
 * the table key strings and the other resources. (new in formatVersion 2/ICU 4.4)
 * At offset 0 of that array is a 16-bit zero value for empty 16-bit resources.
 *
 * Resource16 values in Table16 and Array16 are 16-bit offsets to String-v2
 * resources, with the offsets relative to the start of the 16-bit-units array.
 * Starting with formatVersion 3/ICU 56, if offset<poolStringIndex16Limit
 * then use the pool bundle's 16-bit-units array,
 * otherwise subtract that limit and use the local 16-bit-units array.
 *
 * Type Name            Memory layout of values
 *                      (in parentheses: scalar, non-offset values)
 *
 * 0  Unicode String:   int32_t length, UChar[length], (UChar)0, (padding)
 *                  or  (empty string ("") if offset==0)
 * 1  Binary:           int32_t length, uint8_t[length], (padding)
 *                      - the start of the bytes is 16-aligned -
 * 2  Table:            uint16_t count, uint16_t keyStringOffsets[count], (uint16_t padding), Resource[count]
 * 3  Alias:            (physically same value layout as string, new in ICU 2.4)
 * 4  Table32:          int32_t count, int32_t keyStringOffsets[count], Resource[count]
 *                      (new in formatVersion 1.1/ICU 2.8)
 * 5  Table16:          uint16_t count, uint16_t keyStringOffsets[count], Resource16[count]
 *                      (stored in the 16-bit-units array; new in formatVersion 2/ICU 4.4)
 * 6  Unicode String-v2:UChar[length], (UChar)0; length determined by the first UChar:
 *                      - if first is not a trail surrogate, then the length is implicit
 *                        and u_strlen() needs to be called
 *                      - if first<0xdfef then length=first&0x3ff (and skip first)
 *                      - if first<0xdfff then length=((first-0xdfef)<<16) | second UChar
 *                      - if first==0xdfff then length=((second UChar)<<16) | third UChar
 *                      (stored in the 16-bit-units array; new in formatVersion 2/ICU 4.4)
 *
 *                      Starting with formatVersion 3/ICU 56, if offset<poolStringIndexLimit
 *                      then use the pool bundle's 16-bit-units array,
 *                      otherwise subtract that limit and use the local 16-bit-units array.
 *                      (Note different limits for Resource16 vs. Resource.)
 *
 * 7  Integer:          (28-bit offset is integer value)
 * 8  Array:            int32_t count, Resource[count]
 * 9  Array16:          uint16_t count, Resource16[count]
 *                      (stored in the 16-bit-units array; new in formatVersion 2/ICU 4.4)
 * 14 Integer Vector:   int32_t length, int32_t[length]
 * 15 Reserved:         This value denotes special purpose resources and is for internal use.
 *
 * Note that there are 3 types with data vector values:
 * - Vectors of 8-bit bytes stored as type Binary.
 * - Vectors of 16-bit words stored as type Unicode String or Unicode String-v2
 *                     (no value restrictions, all values 0..ffff allowed!).
 * - Vectors of 32-bit words stored as type Integer Vector.
 */

/*
 * Structure for a single, memory-mapped ResourceBundle.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ Resource_ದತ್ತಾಂಶ {
    UData_ಸ್ಮರಣೆ *ದತ್ತಾಂಶ;
    ಸ್ಥಿರಾಂಕ int32_t *p_ಬೇರು;
    ಸ್ಥಿರಾಂಕ uint16_t *p16_ಬಿಟ್_Units;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *pool_Bundle_ಕೀಲಿಗಳು;
    Resource ಬೇರು_Res;
    int32_t local_ಕೀಲಿ_ಎಲ್ಲೆ;
    ಸ್ಥಿರಾಂಕ uint16_t *pool_Bundle_ಅಕ್ಷರಸರಣಿಗಳು;
    int32_t pool_String_ಸೂಚ್ಯಂಕ_ಎಲ್ಲೆ;
    int32_t pool_String_Index16_ಎಲ್ಲೆ;
    UBool noFallback; /* see URES_ATT_NO_FALLBACK */
    UBool isPoolBundle;
    UBool ಬಳಸುತ್ತದೆ_Pool_Bundle;
    UBool useNativeStrcmp;
} Resource_ದತ್ತಾಂಶ;

ರಚನೆ UResource_ದತ್ತಾಂಶ_ನಮೂದು;   // forward declared for ResoureDataValue below; actually defined in uresimp.h

/*
 * Read a resource bundle from memory.
 */
U_CAPI ಶೂನ್ಯ U_EXPORT2
res_read(Resource_ದತ್ತಾಂಶ *p_Res_ದತ್ತಾಂಶ,
         ಸ್ಥಿರಾಂಕ UDataInfo *pInfo, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಒಳ_ಬೈಟುಗಳು, int32_t ಉದ್ದಳತೆ,
         UError_ಸಂಕೇತ *error_ಸಂಕೇತ);

/*
 * Load a resource bundle file.
 * The ResourceData structure must be allocated externally.
 */
U_CFUNC ಶೂನ್ಯ
res_ತುಂಬು(Resource_ದತ್ತಾಂಶ *p_Res_ದತ್ತಾಂಶ,
         ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, UError_ಸಂಕೇತ *error_ಸಂಕೇತ);

/*
 * Release a resource bundle file.
 * This does not release the ResourceData structure itself.
 */
U_CFUNC ಶೂನ್ಯ
res_unload(Resource_ದತ್ತಾಂಶ *p_Res_ದತ್ತಾಂಶ);

U_CAPI UResType U_EXPORT2
res_get_ಬಹಿರಂಗ_Type(Resource res);

///////////////////////////////////////////////////////////////////////////
// To enable tracing, use the inline versions of the res_get* functions. //
///////////////////////////////////////////////////////////////////////////

/*
 * Return a pointer to a zero-terminated, const UChar* string
 * and set its length in *pLength.
 * Returns NULL if not found.
 */
U_CAPI ಸ್ಥಿರಾಂಕ UChar * U_EXPORT2
res_getStringNoTrace(ಸ್ಥಿರಾಂಕ Resource_ದತ್ತಾಂಶ *p_Res_ದತ್ತಾಂಶ, Resource res, int32_t *p_ಉದ್ದಳತೆ);

U_CAPI ಸ್ಥಿರಾಂಕ uint8_t * U_EXPORT2
res_get_ದ್ವಿಮಾನ_No_Trace(ಸ್ಥಿರಾಂಕ Resource_ದತ್ತಾಂಶ *p_Res_ದತ್ತಾಂಶ, Resource res, int32_t *p_ಉದ್ದಳತೆ);

U_CAPI ಸ್ಥಿರಾಂಕ int32_t * U_EXPORT2
res_get_Int_ಸದಿಶ_No_Trace(ಸ್ಥಿರಾಂಕ Resource_ದತ್ತಾಂಶ *p_Res_ದತ್ತಾಂಶ, Resource res, int32_t *p_ಉದ್ದಳತೆ);

U_CAPI ಸ್ಥಿರಾಂಕ UChar * U_EXPORT2
res_getAlias(ಸ್ಥಿರಾಂಕ Resource_ದತ್ತಾಂಶ *p_Res_ದತ್ತಾಂಶ, Resource res, int32_t *p_ಉದ್ದಳತೆ);

U_CAPI Resource U_EXPORT2
res_getResource(ಸ್ಥಿರಾಂಕ Resource_ದತ್ತಾಂಶ *p_Res_ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ);

U_CAPI int32_t U_EXPORT2
res_ಎಣಿಕೆ_Array_ವಸ್ತುಗಳು(ಸ್ಥಿರಾಂಕ Resource_ದತ್ತಾಂಶ *p_Res_ದತ್ತಾಂಶ, Resource res);

U_CAPI Resource U_EXPORT2
res_get_Array_ಅಂಶ(ಸ್ಥಿರಾಂಕ Resource_ದತ್ತಾಂಶ *p_Res_ದತ್ತಾಂಶ, Resource array, int32_t ಸೂಚ್ಯಂಕ_S);

U_CAPI Resource U_EXPORT2
res_get_ಕೋಷ್ಟಕ_ಅಂಶ_By_ಸೂಚ್ಯಂಕ(ಸ್ಥಿರಾಂಕ Resource_ದತ್ತಾಂಶ *p_Res_ದತ್ತಾಂಶ, Resource ಕೋಷ್ಟಕ, int32_t ಸೂಚ್ಯಂಕ_S, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ ** ಕೀಲಿ);

U_CAPI Resource U_EXPORT2
res_get_ಕೋಷ್ಟಕ_ಅಂಶ_By_ಕೀಲಿ(ಸ್ಥಿರಾಂಕ Resource_ದತ್ತಾಂಶ *p_Res_ದತ್ತಾಂಶ, Resource ಕೋಷ್ಟಕ, int32_t *ಸೂಚ್ಯಂಕ_S, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* * ಕೀಲಿ);

/**
 * Iterates over the path and stops when a scalar resource is found.
 * Follows aliases.
 * Modifies the contents of *path (replacing separators with NULs),
 * and also moves *path forward while it finds items.
 *
 * @param path input: "CollationElements/Sequence" or "zoneStrings/3/2" etc.;
 *             output: points to the part that has not yet been processed
 */
U_CFUNC Resource res_ಕಂಡುಹಿಡಿ_Resource(ಸ್ಥಿರಾಂಕ Resource_ದತ್ತಾಂಶ *p_Res_ದತ್ತಾಂಶ, Resource r,
                                  ಅಕ್ಷರ** ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ** ಕೀಲಿ);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __cplusplus

#ಅನ್ತರ್ಭಾವ "resource.h"
#ಅನ್ತರ್ಭಾವ "restrace.h"

U_NAMESPACE_BEGIN

ಸಾಲೊಳಗೆ ಸ್ಥಿರಾಂಕ char16_t* res_getString(ಸ್ಥಿರಾಂಕ ResourceTracer& traceInfo,
        ಸ್ಥಿರಾಂಕ Resource_ದತ್ತಾಂಶ *p_Res_ದತ್ತಾಂಶ, Resource res, int32_t *p_ಉದ್ದಳತೆ) {
    traceInfo.trace("string");
    ಹಿಂತಿರುಗಿ res_getStringNoTrace(p_Res_ದತ್ತಾಂಶ, res, p_ಉದ್ದಳತೆ);
}

ಸಾಲೊಳಗೆ ಸ್ಥಿರಾಂಕ uint8_t* res_get_ದ್ವಿಮಾನ(ಸ್ಥಿರಾಂಕ ResourceTracer& traceInfo,
        ಸ್ಥಿರಾಂಕ Resource_ದತ್ತಾಂಶ *p_Res_ದತ್ತಾಂಶ, Resource res, int32_t *p_ಉದ್ದಳತೆ) {
    traceInfo.trace("binary");
    ಹಿಂತಿರುಗಿ res_get_ದ್ವಿಮಾನ_No_Trace(p_Res_ದತ್ತಾಂಶ, res, p_ಉದ್ದಳತೆ);
}

ಸಾಲೊಳಗೆ ಸ್ಥಿರಾಂಕ int32_t* res_get_Int_ಸದಿಶ(ಸ್ಥಿರಾಂಕ ResourceTracer& traceInfo,
        ಸ್ಥಿರಾಂಕ Resource_ದತ್ತಾಂಶ *p_Res_ದತ್ತಾಂಶ, Resource res, int32_t *p_ಉದ್ದಳತೆ) {
    traceInfo.trace("intvector");
    ಹಿಂತಿರುಗಿ res_get_Int_ಸದಿಶ_No_Trace(p_Res_ದತ್ತಾಂಶ, res, p_ಉದ್ದಳತೆ);
}

ಸಾಲೊಳಗೆ int32_t res_getInt(ಸ್ಥಿರಾಂಕ ResourceTracer& traceInfo, Resource res) {
    traceInfo.trace("int");
    ಹಿಂತಿರುಗಿ RES_GET_INT_NO_TRACE(res);
}

ಸಾಲೊಳಗೆ uint32_t res_getUInt(ಸ್ಥಿರಾಂಕ ResourceTracer& traceInfo, Resource res) {
    traceInfo.trace("uint");
    ಹಿಂತಿರುಗಿ RES_GET_UINT_NO_TRACE(res);
}

class Resource_ದತ್ತಾಂಶ_ಮೌಲ್ಯ : ಸಾರ್ವಜನಿಕ Resource_ಮೌಲ್ಯ {
ಸಾರ್ವಜನಿಕ:
    Resource_ದತ್ತಾಂಶ_ಮೌಲ್ಯ() :
        p_Res_ದತ್ತಾಂಶ(nullptr),
        valid_Locale_ದತ್ತಾಂಶ_ನಮೂದು(nullptr),
        res(static_cast<Resource>(URES_NONE)),
        fTraceInfo() {}
    ವಾಸ್ತವಿಕ ~Resource_ದತ್ತಾಂಶ_ಮೌಲ್ಯ();

    ಶೂನ್ಯ ಗಣ_ದತ್ತಾಂಶ(ಸ್ಥಿರಾಂಕ Resource_ದತ್ತಾಂಶ &ದತ್ತಾಂಶ) {
        p_Res_ದತ್ತಾಂಶ = &ದತ್ತಾಂಶ;
    }
    
    ಶೂನ್ಯ ಗಣ_Valid_Locale_ದತ್ತಾಂಶ_ನಮೂದು(UResource_ದತ್ತಾಂಶ_ನಮೂದು *ನಮೂದು) {
        valid_Locale_ದತ್ತಾಂಶ_ನಮೂದು = ನಮೂದು;
    }

    ಶೂನ್ಯ ಗಣ_Resource(Resource r, ResourceTracer&& traceInfo) {
        res = r;
        fTraceInfo = traceInfo;
    }

    ಸ್ಥಿರಾಂಕ Resource_ದತ್ತಾಂಶ &get_ದತ್ತಾಂಶ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ *p_Res_ದತ್ತಾಂಶ; }
    UResource_ದತ್ತಾಂಶ_ನಮೂದು *get_Valid_Locale_ದತ್ತಾಂಶ_ನಮೂದು() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ valid_Locale_ದತ್ತಾಂಶ_ನಮೂದು; }
    Resource getResource() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ res; }
    ವಾಸ್ತವಿಕ UResType getType() ಸ್ಥಿರಾಂಕ override;
    ವಾಸ್ತವಿಕ ಸ್ಥಿರಾಂಕ char16_t *getString(int32_t &ಉದ್ದಳತೆ, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ override;
    ವಾಸ್ತವಿಕ ಸ್ಥಿರಾಂಕ char16_t *getAliasString(int32_t &ಉದ್ದಳತೆ, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ override;
    ವಾಸ್ತವಿಕ int32_t getInt(UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ override;
    ವಾಸ್ತವಿಕ uint32_t getUInt(UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ override;
    ವಾಸ್ತವಿಕ ಸ್ಥಿರಾಂಕ int32_t *get_Int_ಸದಿಶ(int32_t &ಉದ್ದಳತೆ, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ override;
    ವಾಸ್ತವಿಕ ಸ್ಥಿರಾಂಕ uint8_t *get_ದ್ವಿಮಾನ(int32_t &ಉದ್ದಳತೆ, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ override;
    ವಾಸ್ತವಿಕ ResourceArray getArray(UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ override;
    ವಾಸ್ತವಿಕ Resource_ಕೋಷ್ಟಕ get_ಕೋಷ್ಟಕ(UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ override;
    ವಾಸ್ತವಿಕ UBool isNoInheritanceMarker() ಸ್ಥಿರಾಂಕ override;
    ವಾಸ್ತವಿಕ int32_t getStringArray(UnicodeString *dest, int32_t ಸಾಮರ್ಥ್ಯ,
                                   UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ override;
    ವಾಸ್ತವಿಕ int32_t getStringArrayOrStringAsArray(UnicodeString *dest, int32_t ಸಾಮರ್ಥ್ಯ,
                                                  UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ override;
    ವಾಸ್ತವಿಕ UnicodeString get_String_Or_ಮೊದಲ_ದ_Array(UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ override;

ಖಾಸಗಿ:
    ಸ್ಥಿರಾಂಕ Resource_ದತ್ತಾಂಶ *p_Res_ದತ್ತಾಂಶ;
    UResource_ದತ್ತಾಂಶ_ನಮೂದು *valid_Locale_ದತ್ತಾಂಶ_ನಮೂದು;
    Resource res;
    ResourceTracer fTraceInfo;
};

U_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ  /* __cplusplus */

/**
 * Swap an ICU resource bundle. See udataswp.h.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
ures_ಅದಲುಬದಲು(ಸ್ಥಿರಾಂಕ UDataSwapper *ds,
          ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಒಳ_ದತ್ತಾಂಶ, int32_t ಉದ್ದಳತೆ, ಶೂನ್ಯ *ಹೊರಹೋಗು_ದತ್ತಾಂಶ,
          UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ);

#ಆದರೆ_ಅಂತ್ಯ
