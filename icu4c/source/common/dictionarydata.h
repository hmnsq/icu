// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
* Copyright (C) 2014, International Business Machines
* Corporation and others.  All Rights Reserved.
*******************************************************************************
* dictionarydata.h
*
* created on: 2012may31
* created by: Markus W. Scherer & Maxime Serrano
*/

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __DICTIONARYDATA_H__
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __DICTIONARYDATA_H__

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

#ਜੇ !UCONFIG_NO_BREAK_ITERATION

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utext.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/udata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "udataswp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uobject.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ustringtrie.h"

U_NAMESPACE_BEGIN

class UCharsTrie;
class ਬਾਈਟਾਂ_Trie;

class U_COMMON_API Dictionary_ਅੰਕੜੇ : ਜਨਤਕ UMemory {
ਜਨਤਕ:
    ਸਥਿਰ ਅਚਲ int32_t TRIE_TYPE_ਬਾਈਟਾਂ; // = 0;
    ਸਥਿਰ ਅਚਲ int32_t TRIE_TYPE_UCHARS; // = 1;
    ਸਥਿਰ ਅਚਲ int32_t TRIE_TYPE_ਨਕਾਬ; // = 7;
    ਸਥਿਰ ਅਚਲ int32_t TRIE_HAS_ਮੁੱਲਾਂ; // = 8;

    ਸਥਿਰ ਅਚਲ int32_t TRANSFORM_NONE; // = 0;
    ਸਥਿਰ ਅਚਲ int32_t TRANSFORM_TYPE_ਵਿੱਥ; // = 0x1000000;
    ਸਥਿਰ ਅਚਲ int32_t TRANSFORM_TYPE_ਨਕਾਬ; // = 0x7f000000;
    ਸਥਿਰ ਅਚਲ int32_t TRANSFORM_ਵਿੱਥ_ਨਕਾਬ; // = 0x1fffff;

    ਗਿਣਤੀ {
        // Byte offsets from the start of the data, after the generic header.
        IX_STRING_TRIE_ਵਿੱਥ,
        IX_RESERVED1_ਵਿੱਥ,
        IX_RESERVED2_ਵਿੱਥ,
        IX_ਕੁੱਲ_ਮਾਪ,

        // Trie type: TRIE_HAS_VALUES | TRIE_TYPE_BYTES etc.
        IX_TRIE_TYPE,
        // Transform specification: TRANSFORM_TYPE_OFFSET | 0xe00 etc.
        IX_TRANSFORM,

        IX_RESERVED6,
        IX_RESERVED7,
        IX_ਗਿਣਤੀ
    };
};

/**
 * Wrapper class around generic dictionaries, implementing matches().
 * getType() should return a TRIE_TYPE_??? constant from DictionaryData.
 * 
 * All implementations of this interface must be thread-safe if they are to be used inside of the
 * dictionary-based break iteration code.
 */
class U_COMMON_API DictionaryMatcher : ਜਨਤਕ UMemory {
ਜਨਤਕ:
    DictionaryMatcher() {}
    ਆਭਾਸੀ ~DictionaryMatcher();
    // this should emulate CompactTrieDictionary::matches()
    /*  @param text      The text in which to look for matching words. Matching begins
     *                   at the current position of the UText.
     *  @param maxLength The max length of match to consider. Units are the native indexing
     *                   units of the UText.
     *  @param limit     Capacity of output arrays, which is also the maximum number of
     *                   matching words to be found.
     *  @param lengths   output array, filled with the lengths of the matches, in order,
     *                   from shortest to longest. Lengths are in native indexing units
     *                   of the UText. May be nullptr.
     *  @param cpLengths output array, filled with the lengths of the matches, in order,
     *                   from shortest to longest. Lengths are the number of Unicode code points.
     *                   May be nullptr.
     *  @param values    Output array, filled with the values associated with the words found.
     *                   May be nullptr.
     *  @param prefix    Output parameter, the code point length of the prefix match, even if that
     *                   prefix didn't lead to a complete word. Will always be >= the cpLength
     *                   of the longest complete word matched. May be nullptr.
     *  @return          Number of matching words found.
     */
    ਆਭਾਸੀ int32_t matches(UText *ਪਾਠ, int32_t max_ਲੰਮਾਈ_2, int32_t ਹੱਦਬੰਦੀ,
                            int32_t *lengths, int32_t *cpLengths, int32_t *ਮੁੱਲਾਂ,
                            int32_t *ਅਗੇਤਰ) ਅਚਲ = 0;

    /** @return DictionaryData::TRIE_TYPE_XYZ */
    ਆਭਾਸੀ int32_t getType() ਅਚਲ = 0;
};

// Implementation of the DictionaryMatcher interface for a UCharsTrie dictionary
class U_COMMON_API UCharsDictionaryMatcher : ਜਨਤਕ DictionaryMatcher {
ਜਨਤਕ:
    // constructs a new UCharsDictionaryMatcher.
    // The UDataMemory * will be closed on this object's destruction.
    UCharsDictionaryMatcher(ਅਚਲ char16_t *c, UData_ਸਮ੍ਰਿਤੀ *f) : characters(c), file(f) { }
    ਆਭਾਸੀ ~UCharsDictionaryMatcher();
    ਆਭਾਸੀ int32_t matches(UText *ਪਾਠ, int32_t max_ਲੰਮਾਈ_2, int32_t ਹੱਦਬੰਦੀ,
                            int32_t *lengths, int32_t *cpLengths, int32_t *ਮੁੱਲਾਂ,
                            int32_t *ਅਗੇਤਰ) ਅਚਲ override;
    ਆਭਾਸੀ int32_t getType() ਅਚਲ override;
ਨਿਜੀ:
    ਅਚਲ char16_t *characters;
    UData_ਸਮ੍ਰਿਤੀ *file;
};

// Implementation of the DictionaryMatcher interface for a BytesTrie dictionary
class U_COMMON_API ਬਾਈਟਾਂ_Dictionary_Matcher : ਜਨਤਕ DictionaryMatcher {
ਜਨਤਕ:
    // constructs a new BytesTrieDictionaryMatcher
    // the transform constant should be the constant read from the file, not a masked version!
    // the UDataMemory * fed in here will be closed on this object's destruction
    ਬਾਈਟਾਂ_Dictionary_Matcher(ਅਚਲ ਅੱਖਰ *c, int32_t t, UData_ਸਮ੍ਰਿਤੀ *f)
            : characters(c), transformConstant(t), file(f) { }
    ਆਭਾਸੀ ~ਬਾਈਟਾਂ_Dictionary_Matcher();
    ਆਭਾਸੀ int32_t matches(UText *ਪਾਠ, int32_t max_ਲੰਮਾਈ_2, int32_t ਹੱਦਬੰਦੀ,
                            int32_t *lengths, int32_t *cpLengths, int32_t *ਮੁੱਲਾਂ,
                            int32_t *ਅਗੇਤਰ) ਅਚਲ override;
    ਆਭਾਸੀ int32_t getType() ਅਚਲ override;
ਨਿਜੀ:
    UChar32 transform(UChar32 c) ਅਚਲ;

    ਅਚਲ ਅੱਖਰ *characters;
    int32_t transformConstant;
    UData_ਸਮ੍ਰਿਤੀ *file;
};

U_NAMESPACE_ਸਿਰਾ

U_CAPI int32_t U_EXPORT2
udict_ਅਦਲਾ_ਬਦਲੀ(ਅਚਲ UDataSwapper *ds, ਅਚਲ ਖਾਲੀ *ਵਿੱਚ_ਅੰਕੜੇ, int32_t ਲੰਮਾਈ, ਖਾਲੀ *ਬਾਹਰਲਾ_ਅੰਕੜੇ, UError_ਕੋਡ *p_Error_ਕੋਡ);

/**
 * Format of dictionary .dict data files.
 * Format version 1.0.
 *
 * A dictionary .dict data file contains a byte-serialized BytesTrie or
 * a UChars-serialized UCharsTrie.
 * Such files are used in dictionary-based break iteration (DBBI).
 *
 * For a BytesTrie, a transformation type is specified for
 * transforming Unicode strings into byte sequences.
 *
 * A .dict file begins with a standard ICU data file header
 * (DataHeader, see ucmndata.h and unicode/udata.h).
 * The UDataInfo.dataVersion field is currently unused (set to 0.0.0.0).
 *
 * After the header, the file contains the following parts.
 * Constants are defined in the DictionaryData class.
 *
 * For the data structure of BytesTrie & UCharsTrie see
 * https://icu.unicode.org/design/struct/tries
 * and the bytestrie.h and ucharstrie.h header files.
 *
 * int32_t indexes[indexesLength]; -- indexesLength=indexes[IX_STRING_TRIE_OFFSET]/4;
 *
 *      The first four indexes are byte offsets in ascending order.
 *      Each byte offset marks the start of the next part in the data file,
 *      and the end of the previous one.
 *      When two consecutive byte offsets are the same, then the corresponding part is empty.
 *      Byte offsets are offsets from after the header,
 *      that is, from the beginning of the indexes[].
 *      Each part starts at an offset with proper alignment for its data.
 *      If necessary, the previous part may include padding bytes to achieve this alignment.
 *
 *      trieType=indexes[IX_TRIE_TYPE] defines the trie type.
 *      transform=indexes[IX_TRANSFORM] defines the Unicode-to-bytes transformation.
 *          If the transformation type is TRANSFORM_TYPE_OFFSET,
 *          then the lower 21 bits contain the offset code point.
 *          Each code point c is mapped to byte b = (c - offset).
 *          Code points outside the range offset..(offset+0xff) cannot be mapped
 *          and do not occur in the dictionary.
 *
 * stringTrie; -- a serialized BytesTrie or UCharsTrie
 *
 *      The dictionary maps strings to specific values (TRIE_HAS_VALUES bit set in trieType),
 *      or it maps all strings to 0 (TRIE_HAS_VALUES bit not set).
 */

#ਜੇ_ਅੰਤ  /* !UCONFIG_NO_BREAK_ITERATION */
#ਜੇ_ਅੰਤ  /* __DICTIONARYDATA_H__ */
