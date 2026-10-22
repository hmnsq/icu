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

#جے_بنا_تعریف __DICTIONARYDATA_H__
#تعریف_کرو __DICTIONARYDATA_H__

#ضمن "unicode/utypes.h"

#إذا !UCONFIG_NO_BREAK_ITERATION

#ضمن "unicode/utext.h"
#ضمن "unicode/udata.h"
#ضمن "udataswp.h"
#ضمن "unicode/uobject.h"
#ضمن "unicode/ustringtrie.h"

U_NAMESPACE_BEGIN

class UCharsTrie;
class بائٹاں_Trie;

class U_COMMON_API Dictionary_انکڑے : عام UMemory {
عام:
    ساكن ثابت int32_t TRIE_TYPE_بائٹاں; // = 0;
    ساكن ثابت int32_t TRIE_TYPE_UCHARS; // = 1;
    ساكن ثابت int32_t TRIE_TYPE_نقاب; // = 7;
    ساكن ثابت int32_t TRIE_HAS_اقدار; // = 8;

    ساكن ثابت int32_t TRANSFORM_NONE; // = 0;
    ساكن ثابت int32_t TRANSFORM_TYPE_وتھ; // = 0x1000000;
    ساكن ثابت int32_t TRANSFORM_TYPE_نقاب; // = 0x7f000000;
    ساكن ثابت int32_t TRANSFORM_وتھ_نقاب; // = 0x1fffff;

    تعداد {
        // Byte offsets from the start of the data, after the generic header.
        IX_STRING_TRIE_وتھ,
        IX_RESERVED1_وتھ,
        IX_RESERVED2_وتھ,
        IX_کل_ناپ,

        // Trie type: TRIE_HAS_VALUES | TRIE_TYPE_BYTES etc.
        IX_TRIE_TYPE,
        // Transform specification: TRANSFORM_TYPE_OFFSET | 0xe00 etc.
        IX_TRANSFORM,

        IX_RESERVED6,
        IX_RESERVED7,
        IX_گنتی
    };
};

/**
 * Wrapper class around generic dictionaries, implementing matches().
 * getType() should return a TRIE_TYPE_??? constant from DictionaryData.
 * 
 * All implementations of this interface must be thread-safe if they are to be used inside of the
 * dictionary-based break iteration code.
 */
class U_COMMON_API DictionaryMatcher : عام UMemory {
عام:
    DictionaryMatcher() {}
    مجازی ~DictionaryMatcher();
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
    مجازی int32_t matches(UText *متن, int32_t max_لمائی_2, int32_t حد_بندی,
                            int32_t *lengths, int32_t *cpLengths, int32_t *اقدار,
                            int32_t *اگیتر) ثابت = 0;

    /** @return DictionaryData::TRIE_TYPE_XYZ */
    مجازی int32_t getType() ثابت = 0;
};

// Implementation of the DictionaryMatcher interface for a UCharsTrie dictionary
class U_COMMON_API UCharsDictionaryMatcher : عام DictionaryMatcher {
عام:
    // constructs a new UCharsDictionaryMatcher.
    // The UDataMemory * will be closed on this object's destruction.
    UCharsDictionaryMatcher(ثابت char16_t *c, UData_حافظہ *f) : characters(c), file(f) { }
    مجازی ~UCharsDictionaryMatcher();
    مجازی int32_t matches(UText *متن, int32_t max_لمائی_2, int32_t حد_بندی,
                            int32_t *lengths, int32_t *cpLengths, int32_t *اقدار,
                            int32_t *اگیتر) ثابت override;
    مجازی int32_t getType() ثابت override;
خاص:
    ثابت char16_t *characters;
    UData_حافظہ *file;
};

// Implementation of the DictionaryMatcher interface for a BytesTrie dictionary
class U_COMMON_API بائٹاں_Dictionary_Matcher : عام DictionaryMatcher {
عام:
    // constructs a new BytesTrieDictionaryMatcher
    // the transform constant should be the constant read from the file, not a masked version!
    // the UDataMemory * fed in here will be closed on this object's destruction
    بائٹاں_Dictionary_Matcher(ثابت حرف *c, int32_t t, UData_حافظہ *f)
            : characters(c), transformConstant(t), file(f) { }
    مجازی ~بائٹاں_Dictionary_Matcher();
    مجازی int32_t matches(UText *متن, int32_t max_لمائی_2, int32_t حد_بندی,
                            int32_t *lengths, int32_t *cpLengths, int32_t *اقدار,
                            int32_t *اگیتر) ثابت override;
    مجازی int32_t getType() ثابت override;
خاص:
    UChar32 transform(UChar32 c) ثابت;

    ثابت حرف *characters;
    int32_t transformConstant;
    UData_حافظہ *file;
};

U_NAMESPACE_سرا

U_CAPI int32_t U_EXPORT2
udict_وٹاندرا(ثابت UDataSwapper *ds, ثابت فراغ *وچ_انکڑے, int32_t لمائی, فراغ *باہرلا_انکڑے, UError_کوڈ *p_Error_کوڈ);

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

#جے_انت  /* !UCONFIG_NO_BREAK_ITERATION */
#جے_انت  /* __DICTIONARYDATA_H__ */
