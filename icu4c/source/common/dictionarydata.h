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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __DICTIONARYDATA_H__
#ವ್ಯಾಖ್ಯಾನಿಸು __DICTIONARYDATA_H__

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಆದರೆ !UCONFIG_NO_BREAK_ITERATION

#ಅನ್ತರ್ಭಾವ "unicode/utext.h"
#ಅನ್ತರ್ಭಾವ "unicode/udata.h"
#ಅನ್ತರ್ಭಾವ "udataswp.h"
#ಅನ್ತರ್ಭಾವ "unicode/uobject.h"
#ಅನ್ತರ್ಭಾವ "unicode/ustringtrie.h"

U_NAMESPACE_BEGIN

class UCharsTrie;
class ಬೈಟುಗಳು_Trie;

class U_COMMON_API Dictionary_ದತ್ತಾಂಶ : ಸಾರ್ವಜನಿಕ UMemory {
ಸಾರ್ವಜನಿಕ:
    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ int32_t TRIE_TYPE_ಬೈಟುಗಳು; // = 0;
    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ int32_t TRIE_TYPE_UCHARS; // = 1;
    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ int32_t TRIE_TYPE_ಮುಖವಾಡ; // = 7;
    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ int32_t TRIE_HAS_ಮೌಲ್ಯಗಳು; // = 8;

    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ int32_t TRANSFORM_NONE; // = 0;
    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ int32_t TRANSFORM_TYPE_ಸ್ಥಾನಾಂತರ; // = 0x1000000;
    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ int32_t TRANSFORM_TYPE_ಮುಖವಾಡ; // = 0x7f000000;
    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ int32_t TRANSFORM_ಸ್ಥಾನಾಂತರ_ಮುಖವಾಡ; // = 0x1fffff;

    ಎಣಿಕೆ {
        // Byte offsets from the start of the data, after the generic header.
        IX_STRING_TRIE_ಸ್ಥಾನಾಂತರ,
        IX_RESERVED1_ಸ್ಥಾನಾಂತರ,
        IX_RESERVED2_ಸ್ಥಾನಾಂತರ,
        IX_ಒಟ್ಟು_ಗಾತ್ರ,

        // Trie type: TRIE_HAS_VALUES | TRIE_TYPE_BYTES etc.
        IX_TRIE_TYPE,
        // Transform specification: TRANSFORM_TYPE_OFFSET | 0xe00 etc.
        IX_TRANSFORM,

        IX_RESERVED6,
        IX_RESERVED7,
        IX_ಎಣಿಕೆ
    };
};

/**
 * Wrapper class around generic dictionaries, implementing matches().
 * getType() should return a TRIE_TYPE_??? constant from DictionaryData.
 * 
 * All implementations of this interface must be thread-safe if they are to be used inside of the
 * dictionary-based break iteration code.
 */
class U_COMMON_API DictionaryMatcher : ಸಾರ್ವಜನಿಕ UMemory {
ಸಾರ್ವಜನಿಕ:
    DictionaryMatcher() {}
    ವಾಸ್ತವಿಕ ~DictionaryMatcher();
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
    ವಾಸ್ತವಿಕ int32_t matches(UText *ಪಠ್ಯ, int32_t max_ಉದ್ದಳತೆ_2, int32_t ಎಲ್ಲೆ,
                            int32_t *lengths, int32_t *cpLengths, int32_t *ಮೌಲ್ಯಗಳು,
                            int32_t *ಪೂರ್ವಪ್ರತ್ಯಯ) ಸ್ಥಿರಾಂಕ = 0;

    /** @return DictionaryData::TRIE_TYPE_XYZ */
    ವಾಸ್ತವಿಕ int32_t getType() ಸ್ಥಿರಾಂಕ = 0;
};

// Implementation of the DictionaryMatcher interface for a UCharsTrie dictionary
class U_COMMON_API UCharsDictionaryMatcher : ಸಾರ್ವಜನಿಕ DictionaryMatcher {
ಸಾರ್ವಜನಿಕ:
    // constructs a new UCharsDictionaryMatcher.
    // The UDataMemory * will be closed on this object's destruction.
    UCharsDictionaryMatcher(ಸ್ಥಿರಾಂಕ char16_t *c, UData_ಸ್ಮರಣೆ *f) : characters(c), file(f) { }
    ವಾಸ್ತವಿಕ ~UCharsDictionaryMatcher();
    ವಾಸ್ತವಿಕ int32_t matches(UText *ಪಠ್ಯ, int32_t max_ಉದ್ದಳತೆ_2, int32_t ಎಲ್ಲೆ,
                            int32_t *lengths, int32_t *cpLengths, int32_t *ಮೌಲ್ಯಗಳು,
                            int32_t *ಪೂರ್ವಪ್ರತ್ಯಯ) ಸ್ಥಿರಾಂಕ override;
    ವಾಸ್ತವಿಕ int32_t getType() ಸ್ಥಿರಾಂಕ override;
ಖಾಸಗಿ:
    ಸ್ಥಿರಾಂಕ char16_t *characters;
    UData_ಸ್ಮರಣೆ *file;
};

// Implementation of the DictionaryMatcher interface for a BytesTrie dictionary
class U_COMMON_API ಬೈಟುಗಳು_Dictionary_Matcher : ಸಾರ್ವಜನಿಕ DictionaryMatcher {
ಸಾರ್ವಜನಿಕ:
    // constructs a new BytesTrieDictionaryMatcher
    // the transform constant should be the constant read from the file, not a masked version!
    // the UDataMemory * fed in here will be closed on this object's destruction
    ಬೈಟುಗಳು_Dictionary_Matcher(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *c, int32_t t, UData_ಸ್ಮರಣೆ *f)
            : characters(c), transformConstant(t), file(f) { }
    ವಾಸ್ತವಿಕ ~ಬೈಟುಗಳು_Dictionary_Matcher();
    ವಾಸ್ತವಿಕ int32_t matches(UText *ಪಠ್ಯ, int32_t max_ಉದ್ದಳತೆ_2, int32_t ಎಲ್ಲೆ,
                            int32_t *lengths, int32_t *cpLengths, int32_t *ಮೌಲ್ಯಗಳು,
                            int32_t *ಪೂರ್ವಪ್ರತ್ಯಯ) ಸ್ಥಿರಾಂಕ override;
    ವಾಸ್ತವಿಕ int32_t getType() ಸ್ಥಿರಾಂಕ override;
ಖಾಸಗಿ:
    UChar32 transform(UChar32 c) ಸ್ಥಿರಾಂಕ;

    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *characters;
    int32_t transformConstant;
    UData_ಸ್ಮರಣೆ *file;
};

U_NAMESPACE_ಕೊನೆ

U_CAPI int32_t U_EXPORT2
udict_ಅದಲುಬದಲು(ಸ್ಥಿರಾಂಕ UDataSwapper *ds, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಒಳ_ದತ್ತಾಂಶ, int32_t ಉದ್ದಳತೆ, ಶೂನ್ಯ *ಹೊರಹೋಗು_ದತ್ತಾಂಶ, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ);

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

#ಆದರೆ_ಅಂತ್ಯ  /* !UCONFIG_NO_BREAK_ITERATION */
#ಆದರೆ_ಅಂತ್ಯ  /* __DICTIONARYDATA_H__ */
