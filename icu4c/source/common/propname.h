// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
* Copyright (c) 2002-2011, International Business Machines
* Corporation and others.  All Rights Reserved.
**********************************************************************
* Author: Alan Liu
* Created: October 30 2002
* Since: ICU 2.4
* 2010nov19 Markus Scherer  Rewrite for formatVersion 2.
**********************************************************************
*/
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ PROPNAME_H
#ವ್ಯಾಖ್ಯಾನಿಸು PROPNAME_H

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/bytestrie.h"
#ಅನ್ತರ್ಭಾವ "unicode/uchar.h"
#ಅನ್ತರ್ಭಾವ "udataswp.h"
#ಅನ್ತರ್ಭಾವ "uprops.h"

/*
 * This header defines the in-memory layout of the property names data
 * structure representing the UCD data files PropertyAliases.txt and
 * PropertyValueAliases.txt.  It is used by:
 *   propname.cpp - reads data
 *   genpname     - creates data
 */

/* low-level char * property name comparison -------------------------------- */

U_CDECL_BEGIN

/**
 * \var uprv_comparePropertyNames
 * Unicode property names and property value names are compared "loosely".
 *
 * UCD.html 4.0.1 says:
 *   For all property names, property value names, and for property values for
 *   Enumerated, Binary, or Catalog properties, use the following
 *   loose matching rule:
 *
 *   LM3. Ignore case, whitespace, underscore ('_'), and hyphens.
 *
 * This function does just that, for (char *) name strings.
 * It is almost identical to ucnv_compareNames() but also ignores
 * C0 White_Space characters (U+0009..U+000d, and U+0085 on EBCDIC).
 *
 * @internal
 */

U_CAPI int32_t U_EXPORT2
uprv_compare_ASCIIProperty_ಹೆಸರುಗಳು(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name1, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name2);

U_CAPI int32_t U_EXPORT2
uprv_compare_EBCDICProperty_ಹೆಸರುಗಳು(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name1, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name2);

#ಆದರೆ U_CHARSET_FAMILY==U_ASCII_FAMILY
#   define uprv_compare_Property_ಹೆಸರುಗಳು uprv_compare_ASCIIProperty_ಹೆಸರುಗಳು
#ಅಥವಾಆದರೆ U_CHARSET_FAMILY==U_EBCDIC_FAMILY
#   define uprv_compare_Property_ಹೆಸರುಗಳು uprv_compare_EBCDICProperty_ಹೆಸರುಗಳು
#ಇಲ್ಲದಿದ್ದರೆ
#   error U_CHARSET_FAMILY is not valid
#ಆದರೆ_ಅಂತ್ಯ

U_CDECL_ಕೊನೆ

/* UDataMemory structure and signatures ------------------------------------- */

#ವ್ಯಾಖ್ಯಾನಿಸು PNAME_ದತ್ತಾಂಶ_NAME "pnames"
#ವ್ಯಾಖ್ಯಾನಿಸು PNAME_ದತ್ತಾಂಶ_TYPE "icu"

/* Fields in UDataInfo: */

/* PNAME_SIG[] is encoded as numeric literals for compatibility with the HP compiler */
#ವ್ಯಾಖ್ಯಾನಿಸು PNAME_SIG_0 ((uint8_t)0x70) /* p */
#ವ್ಯಾಖ್ಯಾನಿಸು PNAME_SIG_1 ((uint8_t)0x6E) /* n */
#ವ್ಯಾಖ್ಯಾನಿಸು PNAME_SIG_2 ((uint8_t)0x61) /* a */
#ವ್ಯಾಖ್ಯಾನಿಸು PNAME_SIG_3 ((uint8_t)0x6D) /* m */

U_NAMESPACE_BEGIN

class Prop_Name_ದತ್ತಾಂಶ {
ಸಾರ್ವಜನಿಕ:
    ಎಣಿಕೆ {
        // Byte offsets from the start of the data, after the generic header.
        IX_ಮೌಲ್ಯ_MAPS_ಸ್ಥಾನಾಂತರ,
        IX_BYTE_TRIES_ಸ್ಥಾನಾಂತರ,
        IX_NAME_GROUPS_ಸ್ಥಾನಾಂತರ,
        IX_RESERVED3_ಸ್ಥಾನಾಂತರ,
        IX_RESERVED4_ಸ್ಥಾನಾಂತರ,
        IX_ಒಟ್ಟು_ಗಾತ್ರ,

        // Other values.
        IX_MAX_NAME_ಉದ್ದಳತೆ,
        IX_RESERVED7,
        IX_ಎಣಿಕೆ
    };

    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *getPropertyName(int32_t property, int32_t nameChoice);
    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *get_Property_ಮೌಲ್ಯ_Name(int32_t property, int32_t ಮೌಲ್ಯ_2, int32_t nameChoice);

    ಸ್ಥಿರ int32_t getPropertyEnum(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *alias);
    ಸ್ಥಿರ int32_t get_Property_ಮೌಲ್ಯ_Enum(int32_t property, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *alias);

ಖಾಸಗಿ:
    ಸ್ಥಿರ int32_t ಕಂಡುಹಿಡಿ_Property(int32_t property);
    ಸ್ಥಿರ int32_t ಕಂಡುಹಿಡಿ_Property_ಮೌಲ್ಯ_Name_ಗುಂಪು(int32_t ಮೌಲ್ಯ_Map_ಸೂಚ್ಯಂಕ, int32_t ಮೌಲ್ಯ_2);
    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *getName(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name_ಗುಂಪು, int32_t name_ಸೂಚ್ಯಂಕ);
    ಸ್ಥಿರ UBool ಒಳಗೊಂಡಿದೆ_Name(ಬೈಟುಗಳು_Trie &trie, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name);

    ಸ್ಥಿರ int32_t get_Property_Or_ಮೌಲ್ಯ_Enum(int32_t ಬೈಟುಗಳು_Trie_ಸ್ಥಾನಾಂತರ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *alias);

    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ int32_t indexes[];
    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ int32_t ಮೌಲ್ಯ_Maps[];
    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ uint8_t ಬೈಟುಗಳು_Tries[];
    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ nameGroups[];
};

/*
 * pnames.icu formatVersion 2
 *
 * formatVersion 2 is new in ICU 4.8.
 * In ICU 4.8, the pnames.icu data file is used only in ICU4J.
 * ICU4C 4.8 has the same data structures hardcoded in source/common/propname_data.h.
 *
 * For documentation of pnames.icu formatVersion 1 see ICU4C 4.6 (2010-dec-01)
 * or earlier versions of this header file (source/common/propname.h).
 *
 * The pnames.icu begins with the standard ICU DataHeader/UDataInfo.
 * After that:
 *
 * int32_t indexes[8];
 *
 *      (See the PropNameData::IX_... constants.)
 *
 *      The first 6 indexes are byte offsets from the beginning of the data
 *      (beginning of indexes[]) to following structures.
 *      The length of each structure is the difference between its offset
 *      and the next one.
 *      All offsets are filled in: Where there is no data between two offsets,
 *      those two offsets are the same.
 *      The last offset (indexes[PropNameData::IX_TOTAL_SIZE]) indicates the
 *      total number of bytes in the file. (Not counting the standard headers.)
 *
 *      The sixth index (indexes[PropNameData::IX_MAX_NAME_LENGTH]) has the
 *      maximum length of any Unicode property (or property value) alias.
 *      (Without normalization, that is, including underscores etc.)
 *
 * int32_t valueMaps[];
 *
 *      The valueMaps[] begins with a map from UProperty enums to properties,
 *      followed by the per-property value maps from property values to names,
 *      for those properties that have named values.
 *      (Binary & enumerated, plus General_Category_Mask.)
 *
 *      valueMaps[0] contains the number of UProperty enum ranges.
 *      For each range:
 *        int32_t start, limit -- first and last+1 UProperty enum of a dense range
 *        Followed by (limit-start) pairs of
 *          int32_t nameGroupOffset;
 *            Offset into nameGroups[] for the property's names/aliases.
 *          int32_t valueMapIndex;
 *            Offset of the property's value map in the valueMaps[] array.
 *            If the valueMapIndex is 0, then the property does not have named values.
 *
 *      For each property's value map:
 *      int32_t bytesTrieOffset; -- Offset into bytesTries[] for name->value mapping.
 *      int32_t numRanges;
 *        If numRanges is in the range 1..15, then that many ranges of values follow.
 *        Per range:
 *          int32_t start, limit -- first and last+1 UProperty enum of a range
 *          Followed by (limit-start) entries of
 *            int32_t nameGroupOffset;
 *              Offset into nameGroups[] for the property value's names/aliases.
 *              If the nameGroupOffset is 0, then this is not a named value for this property.
 *              (That is, the ranges need not be dense.)
 *        If numRanges is >=0x10, then (numRanges-0x10) sorted values
 *        and then (numRanges-0x10) corresponding nameGroupOffsets follow.
 *        Values are sorted as signed integers.
 *        In this case, the set of values is dense; no nameGroupOffset will be 0.
 *
 *      For both properties and property values, ranges are sorted by their start/limit values.
 *
 * uint8_t bytesTries[];
 *
 *      This is a sequence of BytesTrie structures, byte-serialized tries for
 *      mapping from names/aliases to values.
 *      The first one maps from property names/aliases to UProperty enum constants.
 *      The following ones are indexed by property value map bytesTrieOffsets
 *      for mapping each property's names/aliases to their property values.
 *
 * char nameGroups[];
 *
 *      This is a sequence of property name groups.
 *      Each group is a list of names/aliases (invariant-character strings) for
 *      one property or property value, in the order of UCharNameChoice.
 *      The first byte of each group is the number of names in the group.
 *      It is followed by that many NUL-terminated strings.
 *      The first string is for the short name; if there is no short name,
 *      then the first string is empty.
 *      The second string is the long name. Further strings are additional aliases.
 *
 *      The first name group is for a property rather than a property value,
 *      so that a nameGroupOffset of 0 can be used to indicate "no value"
 *      in a property's sparse value ranges.
 */

U_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ
