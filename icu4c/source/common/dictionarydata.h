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

#ifndef __DICTIONARYDATA_H__
#define __DICTIONARYDATA_H__

#অন্তর্ভাৱ "unicode/utypes.h"

#যদি !UCONFIG_NO_BREAK_ITERATION

#অন্তর্ভাৱ "unicode/utext.h"
#অন্তর্ভাৱ "unicode/udata.h"
#অন্তর্ভাৱ "udataswp.h"
#অন্তর্ভাৱ "unicode/uobject.h"
#অন্তর্ভাৱ "unicode/ustringtrie.h"

U_NAMESPACE_BEGIN

class UCharsTrie;
class BytesTrie;

class U_COMMON_API DictionaryData : সার্ৱজনিক UMemory {
সার্ৱজনিক:
    স্থির ধ্রুৱ int32_t TRIE_TYPE_BYTES; // = 0;
    স্থির ধ্রুৱ int32_t TRIE_TYPE_UCHARS; // = 1;
    স্থির ধ্রুৱ int32_t TRIE_TYPE_MASK; // = 7;
    স্থির ধ্রুৱ int32_t TRIE_HAS_মানসমূহ; // = 8;

    স্থির ধ্রুৱ int32_t TRANSFORM_NONE; // = 0;
    স্থির ধ্রুৱ int32_t TRANSFORM_TYPE_OFFSET; // = 0x1000000;
    স্থির ধ্রুৱ int32_t TRANSFORM_TYPE_MASK; // = 0x7f000000;
    স্থির ধ্রুৱ int32_t TRANSFORM_OFFSET_MASK; // = 0x1fffff;

    গণনা {
        // Byte offsets from the start of the data, after the generic header.
        IX_STRING_TRIE_OFFSET,
        IX_RESERVED1_OFFSET,
        IX_RESERVED2_OFFSET,
        IX_TOTAL_SIZE,

        // Trie type: TRIE_HAS_VALUES | TRIE_TYPE_BYTES etc.
        IX_TRIE_TYPE,
        // Transform specification: TRANSFORM_TYPE_OFFSET | 0xe00 etc.
        IX_TRANSFORM,

        IX_RESERVED6,
        IX_RESERVED7,
        IX_গুনতি
    };
};

/**
 * Wrapper class around generic dictionaries, implementing matches().
 * getType() should return a TRIE_TYPE_??? constant from DictionaryData.
 * 
 * All implementations of this interface must be thread-safe if they are to be used inside of the
 * dictionary-based break iteration code.
 */
class U_COMMON_API DictionaryMatcher : সার্ৱজনিক UMemory {
সার্ৱজনিক:
    DictionaryMatcher() {}
    আভাস ~DictionaryMatcher();
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
    আভাস int32_t matches(UText *text, int32_t maxLength, int32_t limit,
                            int32_t *lengths, int32_t *cpLengths, int32_t *মানসমূহ,
                            int32_t *prefix) ধ্রুৱ = 0;

    /** @return DictionaryData::TRIE_TYPE_XYZ */
    আভাস int32_t getType() ধ্রুৱ = 0;
};

// Implementation of the DictionaryMatcher interface for a UCharsTrie dictionary
class U_COMMON_API UCharsDictionaryMatcher : সার্ৱজনিক DictionaryMatcher {
সার্ৱজনিক:
    // constructs a new UCharsDictionaryMatcher.
    // The UDataMemory * will be closed on this object's destruction.
    UCharsDictionaryMatcher(ধ্রুৱ char16_t *c, UDataMemory *f) : characters(c), file(f) { }
    আভাস ~UCharsDictionaryMatcher();
    আভাস int32_t matches(UText *text, int32_t maxLength, int32_t limit,
                            int32_t *lengths, int32_t *cpLengths, int32_t *মানসমূহ,
                            int32_t *prefix) ধ্রুৱ override;
    আভাস int32_t getType() ধ্রুৱ override;
গুপ্ত:
    ধ্রুৱ char16_t *characters;
    UDataMemory *file;
};

// Implementation of the DictionaryMatcher interface for a BytesTrie dictionary
class U_COMMON_API BytesDictionaryMatcher : সার্ৱজনিক DictionaryMatcher {
সার্ৱজনিক:
    // constructs a new BytesTrieDictionaryMatcher
    // the transform constant should be the constant read from the file, not a masked version!
    // the UDataMemory * fed in here will be closed on this object's destruction
    BytesDictionaryMatcher(ধ্রুৱ ৱর্ণ *c, int32_t t, UDataMemory *f)
            : characters(c), transformConstant(t), file(f) { }
    আভাস ~BytesDictionaryMatcher();
    আভাস int32_t matches(UText *text, int32_t maxLength, int32_t limit,
                            int32_t *lengths, int32_t *cpLengths, int32_t *মানসমূহ,
                            int32_t *prefix) ধ্রুৱ override;
    আভাস int32_t getType() ধ্রুৱ override;
গুপ্ত:
    UChar32 transform(UChar32 c) ধ্রুৱ;

    ধ্রুৱ ৱর্ণ *characters;
    int32_t transformConstant;
    UDataMemory *file;
};

U_NAMESPACE_END

U_CAPI int32_t U_EXPORT2
udict_swap(ধ্রুৱ UDataSwapper *ds, ধ্রুৱ শূন্য *inData, int32_t length, শূন্য *outData, UErrorCode *pErrorCode);

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

#endif  /* !UCONFIG_NO_BREAK_ITERATION */
#endif  /* __DICTIONARYDATA_H__ */
