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

#నిర్వచితం_కాకపోతే __DICTIONARYDATA_H__
#నిర్వచించు __DICTIONARYDATA_H__

#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ !UCONFIG_NO_BREAK_ITERATION

#అన్తర్భావ "unicode/utext.h"
#అన్తర్భావ "unicode/udata.h"
#అన్తర్భావ "udataswp.h"
#అన్తర్భావ "unicode/uobject.h"
#అన్తర్భావ "unicode/ustringtrie.h"

U_NAMESPACE_BEGIN

class UCharsTrie;
class బైట్లు_Trie;

class U_COMMON_API Dictionary_దత్తాంశం : బహిరంగ UMemory {
బహిరంగ:
    స్థిర స్థిరరాశి int32_t TRIE_TYPE_బైట్లు; // = 0;
    స్థిర స్థిరరాశి int32_t TRIE_TYPE_UCHARS; // = 1;
    స్థిర స్థిరరాశి int32_t TRIE_TYPE_ముసుగు; // = 7;
    స్థిర స్థిరరాశి int32_t TRIE_HAS_విలువలు; // = 8;

    స్థిర స్థిరరాశి int32_t TRANSFORM_NONE; // = 0;
    స్థిర స్థిరరాశి int32_t TRANSFORM_TYPE_స్థానభ్రంశం; // = 0x1000000;
    స్థిర స్థిరరాశి int32_t TRANSFORM_TYPE_ముసుగు; // = 0x7f000000;
    స్థిర స్థిరరాశి int32_t TRANSFORM_స్థానభ్రంశం_ముసుగు; // = 0x1fffff;

    గణన {
        // Byte offsets from the start of the data, after the generic header.
        IX_STRING_TRIE_స్థానభ్రంశం,
        IX_RESERVED1_స్థానభ్రంశం,
        IX_RESERVED2_స్థానభ్రంశం,
        IX_మొత్తం_కొలత,

        // Trie type: TRIE_HAS_VALUES | TRIE_TYPE_BYTES etc.
        IX_TRIE_TYPE,
        // Transform specification: TRANSFORM_TYPE_OFFSET | 0xe00 etc.
        IX_TRANSFORM,

        IX_RESERVED6,
        IX_RESERVED7,
        IX_లెక్క
    };
};

/**
 * Wrapper class around generic dictionaries, implementing matches().
 * getType() should return a TRIE_TYPE_??? constant from DictionaryData.
 * 
 * All implementations of this interface must be thread-safe if they are to be used inside of the
 * dictionary-based break iteration code.
 */
class U_COMMON_API DictionaryMatcher : బహిరంగ UMemory {
బహిరంగ:
    DictionaryMatcher() {}
    మాయా ~DictionaryMatcher();
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
    మాయా int32_t matches(UText *పాఠ్యం, int32_t max_నిడివి_2, int32_t హద్దు,
                            int32_t *lengths, int32_t *cpLengths, int32_t *విలువలు,
                            int32_t *ఉపసర్గ) స్థిరరాశి = 0;

    /** @return DictionaryData::TRIE_TYPE_XYZ */
    మాయా int32_t getType() స్థిరరాశి = 0;
};

// Implementation of the DictionaryMatcher interface for a UCharsTrie dictionary
class U_COMMON_API UCharsDictionaryMatcher : బహిరంగ DictionaryMatcher {
బహిరంగ:
    // constructs a new UCharsDictionaryMatcher.
    // The UDataMemory * will be closed on this object's destruction.
    UCharsDictionaryMatcher(స్థిరరాశి char16_t *c, UData_జ్ఞాపకం *f) : characters(c), file(f) { }
    మాయా ~UCharsDictionaryMatcher();
    మాయా int32_t matches(UText *పాఠ్యం, int32_t max_నిడివి_2, int32_t హద్దు,
                            int32_t *lengths, int32_t *cpLengths, int32_t *విలువలు,
                            int32_t *ఉపసర్గ) స్థిరరాశి override;
    మాయా int32_t getType() స్థిరరాశి override;
అంతరంగ:
    స్థిరరాశి char16_t *characters;
    UData_జ్ఞాపకం *file;
};

// Implementation of the DictionaryMatcher interface for a BytesTrie dictionary
class U_COMMON_API బైట్లు_Dictionary_Matcher : బహిరంగ DictionaryMatcher {
బహిరంగ:
    // constructs a new BytesTrieDictionaryMatcher
    // the transform constant should be the constant read from the file, not a masked version!
    // the UDataMemory * fed in here will be closed on this object's destruction
    బైట్లు_Dictionary_Matcher(స్థిరరాశి అక్షరం *c, int32_t t, UData_జ్ఞాపకం *f)
            : characters(c), transformConstant(t), file(f) { }
    మాయా ~బైట్లు_Dictionary_Matcher();
    మాయా int32_t matches(UText *పాఠ్యం, int32_t max_నిడివి_2, int32_t హద్దు,
                            int32_t *lengths, int32_t *cpLengths, int32_t *విలువలు,
                            int32_t *ఉపసర్గ) స్థిరరాశి override;
    మాయా int32_t getType() స్థిరరాశి override;
అంతరంగ:
    UChar32 transform(UChar32 c) స్థిరరాశి;

    స్థిరరాశి అక్షరం *characters;
    int32_t transformConstant;
    UData_జ్ఞాపకం *file;
};

U_NAMESPACE_చివర

U_CAPI int32_t U_EXPORT2
udict_మార్పిడి(స్థిరరాశి UDataSwapper *ds, స్థిరరాశి శూన్య *లోపల_దత్తాంశం, int32_t నిడివి, శూన్య *బహిర్గతం_దత్తాంశం, UError_సంకేతం *p_Error_సంకేతం);

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

#అయితే_ముగింపు  /* !UCONFIG_NO_BREAK_ITERATION */
#అయితే_ముగింపు  /* __DICTIONARYDATA_H__ */
