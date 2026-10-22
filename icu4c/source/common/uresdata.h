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

#अगर_अघोषित __RESDATA_H__
#घोषणा __RESDATA_H__

#समावेश "unicode/utypes.h"
#समावेश "unicode/udata.h"
#समावेश "unicode/ures.h"
#समावेश "putilimp.h"
#समावेश "udataswp.h"

/**
 * Numeric constants for internal-only types of resource items.
 * These must use different numeric values than UResType constants
 * because they are used together.
 * Internal types are never returned by ures_getType().
 */
प्रकार क्रमागत {
    /** Include a negative value so that the compiler uses the same int type as for UResType. */
    URES_INTERNAL_NONE=-1,

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
} UResInternalType;

/*
 * A Resource is a 32-bit value that has 2 bit fields:
 * 31..28   4-bit type, see enum below
 * 27..0    28-bit four-byte-offset or value according to the type
 */
प्रकार uint32_t Resource;

#घोषणा RES_BOGUS 0xffffffff
#घोषणा RES_MAX_OFFSET 0x0fffffff

#घोषणा RES_GET_TYPE(res) ((int32_t)((res)>>28UL))
#घोषणा RES_GET_OFFSET(res) ((res)&0x0fffffff)
#घोषणा RES_GET_POINTER(p_मूल, res) ((p_मूल)+RES_GET_OFFSET(res))

/* get signed and unsigned integer values directly from the Resource handle
 * NOTE: For proper logging, please use the res_getInt() constexpr
 */
#अगर U_SIGNED_दायाँ_SHIFT_IS_ARITHMETIC
#   define RES_GET_INT_NO_TRACE(res) (((int32_t)((res)<<4L))>>4L)
#अन्यथा
#   define RES_GET_INT_NO_TRACE(res) (int32_t)(((res)&0x08000000) ? (res)|0xf0000000 : (res)&0x07ffffff)
#पूर्ण_अगर

#घोषणा RES_GET_UINT_NO_TRACE(res) ((res)&0x0fffffff)

#घोषणा URES_IS_ARRAY(type) ((int32_t)(type)==URES_ARRAY || (int32_t)(type)==URES_ARRAY16)
#घोषणा URES_IS_TABLE(type) ((int32_t)(type)==URES_TABLE || (int32_t)(type)==URES_TABLE16 || (int32_t)(type)==URES_TABLE32)
#घोषणा URES_IS_CONTAINER(type) (URES_IS_TABLE(type) || URES_IS_ARRAY(type))

#घोषणा URES_MAKE_RESOURCE(type, offset) (((Resource)(type)<<28)|(Resource)(offset))
#घोषणा URES_MAKE_EMPTY_RESOURCE(type) ((Resource)(type)<<28)

/* indexes[] value names; indexes are generally 32-bit (Resource) indexes */
क्रमागत {
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
    URES_सूचकांक_लंबाई,
    /**
     * [1] contains the top of the key strings,
     *     same as the bottom of resources or UTF-16 strings, rounded up
     */
    URES_सूचकांक_कुंजियाँ_शीर्ष,
    /** [2] contains the top of all resources */
    URES_सूचकांक_RESOURCES_शीर्ष,
    /**
     * [3] contains the top of the bundle,
     *     in case it were ever different from [2]
     */
    URES_सूचकांक_BUNDLE_शीर्ष,
    /** [4] max. length of any table */
    URES_सूचकांक_MAX_TABLE_लंबाई,
    /**
     * [5] attributes bit set, see URES_ATT_* (new in formatVersion 1.2)
     *
     * formatVersion>=3:
     *   bits 31..16 poolStringIndex16Limit
     *   bits 15..12 poolStringIndexLimit bits 27..24
     */
    URES_सूचकांक_ATTRIBUTES,
    /**
     * [6] top of the 16-bit units (UTF-16 string v2 UChars, URES_TABLE16, URES_ARRAY16),
     *     rounded up (new in formatVersion 2.0, ICU 4.4)
     */
    URES_सूचकांक_16_BIT_शीर्ष,
    /** [7] checksum of the pool bundle (new in formatVersion 2.0, ICU 4.4) */
    URES_सूचकांक_POOL_CHECKSUM,
    URES_सूचकांक_शीर्ष
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
#घोषणा URES_ATT_NO_FALLBACK 1

/*
 * Attributes for bundles that are, or use, a pool bundle.
 * A pool bundle provides key strings that are shared among several other bundles
 * to reduce their total size.
 * New in formatVersion 2 (ICU 4.4).
 */
#घोषणा URES_ATT_IS_POOL_BUNDLE 2
#घोषणा URES_ATT_USES_POOL_BUNDLE 4

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
प्रकार काष्ठा Resource_तथ्य {
    UDataMemory *तथ्य;
    स्थिर int32_t *p_मूल;
    स्थिर uint16_t *p16BitUnits;
    स्थिर अक्षर *pool_Bundle_कुंजियाँ;
    Resource मूल_Res;
    int32_t local_कुंजी_Limit;
    स्थिर uint16_t *poolBundleStrings;
    int32_t pool_String_सूचकांक_Limit;
    int32_t poolStringIndex16Limit;
    UBool noFallback; /* see URES_ATT_NO_FALLBACK */
    UBool isPoolBundle;
    UBool usesPoolBundle;
    UBool useNativeStrcmp;
} Resource_तथ्य;

काष्ठा UResource_तथ्य_Entry;   // forward declared for ResoureDataValue below; actually defined in uresimp.h

/*
 * Read a resource bundle from memory.
 */
U_CAPI व्योम U_EXPORT2
res_read(Resource_तथ्य *p_Res_तथ्य,
         स्थिर UDataInfo *pInfo, स्थिर व्योम *inBytes, int32_t लंबाई,
         UErrorCode *errorCode);

/*
 * Load a resource bundle file.
 * The ResourceData structure must be allocated externally.
 */
U_CFUNC व्योम
res_load(Resource_तथ्य *p_Res_तथ्य,
         स्थिर अक्षर *path, स्थिर अक्षर *name, UErrorCode *errorCode);

/*
 * Release a resource bundle file.
 * This does not release the ResourceData structure itself.
 */
U_CFUNC व्योम
res_unload(Resource_तथ्य *p_Res_तथ्य);

U_CAPI UResType U_EXPORT2
res_getPublicType(Resource res);

///////////////////////////////////////////////////////////////////////////
// To enable tracing, use the inline versions of the res_get* functions. //
///////////////////////////////////////////////////////////////////////////

/*
 * Return a pointer to a zero-terminated, const UChar* string
 * and set its length in *pLength.
 * Returns NULL if not found.
 */
U_CAPI स्थिर UChar * U_EXPORT2
res_getStringNoTrace(स्थिर Resource_तथ्य *p_Res_तथ्य, Resource res, int32_t *p_लंबाई);

U_CAPI स्थिर uint8_t * U_EXPORT2
res_get_द्विआधारी_No_Trace(स्थिर Resource_तथ्य *p_Res_तथ्य, Resource res, int32_t *p_लंबाई);

U_CAPI स्थिर int32_t * U_EXPORT2
res_getIntVectorNoTrace(स्थिर Resource_तथ्य *p_Res_तथ्य, Resource res, int32_t *p_लंबाई);

U_CAPI स्थिर UChar * U_EXPORT2
res_getAlias(स्थिर Resource_तथ्य *p_Res_तथ्य, Resource res, int32_t *p_लंबाई);

U_CAPI Resource U_EXPORT2
res_getResource(स्थिर Resource_तथ्य *p_Res_तथ्य, स्थिर अक्षर *कुंजी);

U_CAPI int32_t U_EXPORT2
res_गिनती_Array_वस्तुएँ(स्थिर Resource_तथ्य *p_Res_तथ्य, Resource res);

U_CAPI Resource U_EXPORT2
res_getArrayItem(स्थिर Resource_तथ्य *p_Res_तथ्य, Resource array, int32_t सूचकांक_S);

U_CAPI Resource U_EXPORT2
res_get_Table_Item_By_सूचकांक(स्थिर Resource_तथ्य *p_Res_तथ्य, Resource table, int32_t सूचकांक_S, स्थिर अक्षर ** कुंजी);

U_CAPI Resource U_EXPORT2
res_get_Table_Item_By_कुंजी(स्थिर Resource_तथ्य *p_Res_तथ्य, Resource table, int32_t *सूचकांक_S, स्थिर अक्षर* * कुंजी);

/**
 * Iterates over the path and stops when a scalar resource is found.
 * Follows aliases.
 * Modifies the contents of *path (replacing separators with NULs),
 * and also moves *path forward while it finds items.
 *
 * @param path input: "CollationElements/Sequence" or "zoneStrings/3/2" etc.;
 *             output: points to the part that has not yet been processed
 */
U_CFUNC Resource res_findResource(स्थिर Resource_तथ्य *p_Res_तथ्य, Resource r,
                                  अक्षर** path, स्थिर अक्षर** कुंजी);

#अगर_घोषित __cplusplus

#समावेश "resource.h"
#समावेश "restrace.h"

U_NAMESPACE_BEGIN

अंतरभूत स्थिर char16_t* res_getString(स्थिर ResourceTracer& traceInfo,
        स्थिर Resource_तथ्य *p_Res_तथ्य, Resource res, int32_t *p_लंबाई) {
    traceInfo.trace("string");
    वापस res_getStringNoTrace(p_Res_तथ्य, res, p_लंबाई);
}

अंतरभूत स्थिर uint8_t* res_get_द्विआधारी(स्थिर ResourceTracer& traceInfo,
        स्थिर Resource_तथ्य *p_Res_तथ्य, Resource res, int32_t *p_लंबाई) {
    traceInfo.trace("binary");
    वापस res_get_द्विआधारी_No_Trace(p_Res_तथ्य, res, p_लंबाई);
}

अंतरभूत स्थिर int32_t* res_getIntVector(स्थिर ResourceTracer& traceInfo,
        स्थिर Resource_तथ्य *p_Res_तथ्य, Resource res, int32_t *p_लंबाई) {
    traceInfo.trace("intvector");
    वापस res_getIntVectorNoTrace(p_Res_तथ्य, res, p_लंबाई);
}

अंतरभूत int32_t res_getInt(स्थिर ResourceTracer& traceInfo, Resource res) {
    traceInfo.trace("int");
    वापस RES_GET_INT_NO_TRACE(res);
}

अंतरभूत uint32_t res_getUInt(स्थिर ResourceTracer& traceInfo, Resource res) {
    traceInfo.trace("uint");
    वापस RES_GET_UINT_NO_TRACE(res);
}

class Resource_तथ्य_मूल्य : खुला Resource_मूल्य {
खुला:
    Resource_तथ्य_मूल्य() :
        p_Res_तथ्य(nullptr),
        valid_Locale_तथ्य_Entry(nullptr),
        res(static_cast<Resource>(URES_NONE)),
        fTraceInfo() {}
    भव ~Resource_तथ्य_मूल्य();

    व्योम set_तथ्य(स्थिर Resource_तथ्य &तथ्य) {
        p_Res_तथ्य = &तथ्य;
    }
    
    व्योम set_Valid_Locale_तथ्य_Entry(UResource_तथ्य_Entry *entry) {
        valid_Locale_तथ्य_Entry = entry;
    }

    व्योम setResource(Resource r, ResourceTracer&& traceInfo) {
        res = r;
        fTraceInfo = traceInfo;
    }

    स्थिर Resource_तथ्य &get_तथ्य() स्थिर { वापस *p_Res_तथ्य; }
    UResource_तथ्य_Entry *get_Valid_Locale_तथ्य_Entry() स्थिर { वापस valid_Locale_तथ्य_Entry; }
    Resource getResource() स्थिर { वापस res; }
    भव UResType getType() स्थिर override;
    भव स्थिर char16_t *getString(int32_t &लंबाई, UErrorCode &errorCode) स्थिर override;
    भव स्थिर char16_t *getAliasString(int32_t &लंबाई, UErrorCode &errorCode) स्थिर override;
    भव int32_t getInt(UErrorCode &errorCode) स्थिर override;
    भव uint32_t getUInt(UErrorCode &errorCode) स्थिर override;
    भव स्थिर int32_t *getIntVector(int32_t &लंबाई, UErrorCode &errorCode) स्थिर override;
    भव स्थिर uint8_t *get_द्विआधारी(int32_t &लंबाई, UErrorCode &errorCode) स्थिर override;
    भव ResourceArray getArray(UErrorCode &errorCode) स्थिर override;
    भव ResourceTable getTable(UErrorCode &errorCode) स्थिर override;
    भव UBool isNoInheritanceMarker() स्थिर override;
    भव int32_t getStringArray(UnicodeString *dest, int32_t capacity,
                                   UErrorCode &errorCode) स्थिर override;
    भव int32_t getStringArrayOrStringAsArray(UnicodeString *dest, int32_t capacity,
                                                  UErrorCode &errorCode) स्थिर override;
    भव UnicodeString getStringOrFirstOfArray(UErrorCode &errorCode) स्थिर override;

निजी:
    स्थिर Resource_तथ्य *p_Res_तथ्य;
    UResource_तथ्य_Entry *valid_Locale_तथ्य_Entry;
    Resource res;
    ResourceTracer fTraceInfo;
};

U_NAMESPACE_END

#पूर्ण_अगर  /* __cplusplus */

/**
 * Swap an ICU resource bundle. See udataswp.h.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
ures_swap(स्थिर UDataSwapper *ds,
          स्थिर व्योम *in_तथ्य, int32_t लंबाई, व्योम *out_तथ्य,
          UErrorCode *pErrorCode);

#पूर्ण_अगर
