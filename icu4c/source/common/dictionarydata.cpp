// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
* Copyright (C) 2014-2016, International Business Machines
* Corporation and others.  All Rights Reserved.
*******************************************************************************
* dictionarydata.h
*
* created on: 2012may31
* created by: Markus W. Scherer & Maxime Serrano
*/

#ಅನ್ತರ್ಭಾವ "dictionarydata.h"
#ಅನ್ತರ್ಭಾವ "unicode/ucharstrie.h"
#ಅನ್ತರ್ಭಾವ "unicode/bytestrie.h"
#ಅನ್ತರ್ಭಾವ "unicode/udata.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"

#ಯದಿ !UCONFIG_NO_BREAK_ITERATION

U_NAMESPACE_BEGIN

ಧ್ರುವ int32_t  DictionaryData::TRIE_TYPE_BYTES = 0;
ಧ್ರುವ int32_t  DictionaryData::TRIE_TYPE_UCHARS = 1;
ಧ್ರುವ int32_t  DictionaryData::TRIE_TYPE_MASK = 7;
ಧ್ರುವ int32_t  DictionaryData::TRIE_HAS_ಮೌಲ್ಯಗಳು = 8;

ಧ್ರುವ int32_t  DictionaryData::TRANSFORM_NONE = 0;
ಧ್ರುವ int32_t  DictionaryData::TRANSFORM_TYPE_OFFSET = 0x1000000;
ಧ್ರುವ int32_t  DictionaryData::TRANSFORM_TYPE_MASK = 0x7f000000;
ಧ್ರುವ int32_t  DictionaryData::TRANSFORM_OFFSET_MASK = 0x1fffff;
    
DictionaryMatcher::~DictionaryMatcher() {
}

UCharsDictionaryMatcher::~UCharsDictionaryMatcher() {
    udata_close(file);
}

int32_t UCharsDictionaryMatcher::getType() ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ DictionaryData::TRIE_TYPE_UCHARS;
}

int32_t UCharsDictionaryMatcher::matches(UText *text, int32_t maxLength, int32_t limit,
                            int32_t *lengths, int32_t *cpLengths, int32_t *ಮೌಲ್ಯಗಳು,
                            int32_t *prefix) ಧ್ರುವ {

    UCharsTrie uct(characters);
    int32_t startingTextIndex = static_cast<int32_t>(utext_getNativeIndex(text));
    int32_t word_ಎಣಿಕೆ = 0;
    int32_t codePointsMatched = 0;

    ಪ್ರತಿ (UChar32 c = utext_next32(text); c >= 0; c=utext_next32(text)) {
        UStringTrieResult result = (codePointsMatched == 0) ? uct.first(c) : uct.next(c);
        int32_t lengthMatched = static_cast<int32_t>(utext_getNativeIndex(text)) - startingTextIndex;
        codePointsMatched += 1;
        ಯದಿ (USTRINGTRIE_HAS_ಮೌಲ್ಯ(result)) {
            ಯದಿ (word_ಎಣಿಕೆ < limit) {
                ಯದಿ (ಮೌಲ್ಯಗಳು != nullptr) {
                    ಮೌಲ್ಯಗಳು[word_ಎಣಿಕೆ] = uct.get_ಮೌಲ್ಯ();
                }
                ಯದಿ (lengths != nullptr) {
                    lengths[word_ಎಣಿಕೆ] = lengthMatched;
                }
                ಯದಿ (cpLengths != nullptr) {
                    cpLengths[word_ಎಣಿಕೆ] = codePointsMatched;
                }
                ++word_ಎಣಿಕೆ;
            }
            ಯದಿ (result == USTRINGTRIE_FINAL_ಮೌಲ್ಯ) {
                ಭಙ್ಗ;
            }
        }
        ಅನ್ಯಥಾ ಯದಿ (result == USTRINGTRIE_NO_MATCH) {
            ಭಙ್ಗ;
        }
        ಯದಿ (lengthMatched >= maxLength) {
            ಭಙ್ಗ;
        }
    }

    ಯದಿ (prefix != nullptr) {
        *prefix = codePointsMatched;
    }
    ಪ್ರತ್ಯಾಗಮ word_ಎಣಿಕೆ;
}

BytesDictionaryMatcher::~BytesDictionaryMatcher() {
    udata_close(file);
}

UChar32 BytesDictionaryMatcher::transform(UChar32 c) ಧ್ರುವ {
    ಯದಿ ((transformConstant & DictionaryData::TRANSFORM_TYPE_MASK) == DictionaryData::TRANSFORM_TYPE_OFFSET) {
        ಯದಿ (c == 0x200D) {
            ಪ್ರತ್ಯಾಗಮ 0xFF;
        } ಅನ್ಯಥಾ ಯದಿ (c == 0x200C) {
            ಪ್ರತ್ಯಾಗಮ 0xFE;
        }
        int32_t delta = c - (transformConstant & DictionaryData::TRANSFORM_OFFSET_MASK);
        ಯದಿ (delta < 0 || 0xFD < delta) {
            ಪ್ರತ್ಯಾಗಮ U_SENTINEL;
        }
        ಪ್ರತ್ಯಾಗಮ static_cast<UChar32>(delta);
    }
    ಪ್ರತ್ಯಾಗಮ c;
}

int32_t BytesDictionaryMatcher::getType() ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ DictionaryData::TRIE_TYPE_BYTES;
}

int32_t BytesDictionaryMatcher::matches(UText *text, int32_t maxLength, int32_t limit,
                            int32_t *lengths, int32_t *cpLengths, int32_t *ಮೌಲ್ಯಗಳು,
                            int32_t *prefix) ಧ್ರುವ {
    BytesTrie bt(characters);
    int32_t startingTextIndex = static_cast<int32_t>(utext_getNativeIndex(text));
    int32_t word_ಎಣಿಕೆ = 0;
    int32_t codePointsMatched = 0;

    ಪ್ರತಿ (UChar32 c = utext_next32(text); c >= 0; c=utext_next32(text)) {
        UStringTrieResult result = (codePointsMatched == 0) ? bt.first(transform(c)) : bt.next(transform(c));
        int32_t lengthMatched = static_cast<int32_t>(utext_getNativeIndex(text)) - startingTextIndex;
        codePointsMatched += 1;
        ಯದಿ (USTRINGTRIE_HAS_ಮೌಲ್ಯ(result)) {
            ಯದಿ (word_ಎಣಿಕೆ < limit) {
                ಯದಿ (ಮೌಲ್ಯಗಳು != nullptr) {
                    ಮೌಲ್ಯಗಳು[word_ಎಣಿಕೆ] = bt.get_ಮೌಲ್ಯ();
                }
                ಯದಿ (lengths != nullptr) {
                    lengths[word_ಎಣಿಕೆ] = lengthMatched;
                }
                ಯದಿ (cpLengths != nullptr) {
                    cpLengths[word_ಎಣಿಕೆ] = codePointsMatched;
                }
                ++word_ಎಣಿಕೆ;
            }
            ಯದಿ (result == USTRINGTRIE_FINAL_ಮೌಲ್ಯ) {
                ಭಙ್ಗ;
            }
        }
        ಅನ್ಯಥಾ ಯದಿ (result == USTRINGTRIE_NO_MATCH) {
            ಭಙ್ಗ;
        }
        ಯದಿ (lengthMatched >= maxLength) {
            ಭಙ್ಗ;
        }
    }

    ಯದಿ (prefix != nullptr) {
        *prefix = codePointsMatched;
    }
    ಪ್ರತ್ಯಾಗಮ word_ಎಣಿಕೆ;
}


U_NAMESPACE_END

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
udict_swap(ಧ್ರುವ UDataSwapper *ds, ಧ್ರುವ ಶೂನ್ಯ *inData, int32_t length,
           ಶೂನ್ಯ *outData, UErrorCode *pErrorCode) {
    ಧ್ರುವ UDataInfo *pInfo;
    int32_t headerSize;
    ಧ್ರುವ uint8_t *inBytes;
    uint8_t *outBytes;
    ಧ್ರುವ int32_t *inIndexes;
    int32_t indexes[DictionaryData::IX_ಎಣಿಕೆ];
    int32_t i, offset, size;

    headerSize = udata_swapDataHeader(ds, inData, length, outData, pErrorCode);
    ಯದಿ (pErrorCode == nullptr || U_FAILURE(*pErrorCode)) ಪ್ರತ್ಯಾಗಮ 0;
    pInfo = (ಧ್ರುವ UDataInfo *)((ಧ್ರುವ ವರ್ಣ *)inData + 4);
    ಯದಿ (!(pInfo->dataFormat[0] == 0x44 && 
          pInfo->dataFormat[1] == 0x69 && 
          pInfo->dataFormat[2] == 0x63 && 
          pInfo->dataFormat[3] == 0x74 && 
          pInfo->formatVersion[0] == 1)) {
        udata_printError(ds, "udict_swap(): data format %02x.%02x.%02x.%02x (format version %02x) is not recognized as dictionary data\n",
                         pInfo->dataFormat[0], pInfo->dataFormat[1], pInfo->dataFormat[2], pInfo->dataFormat[3], pInfo->formatVersion[0]);
        *pErrorCode = U_UNSUPPORTED_ERROR;
        ಪ್ರತ್ಯಾಗಮ 0;
    }

    inBytes = (ಧ್ರುವ uint8_t *)inData + headerSize;
    outBytes = (outData == nullptr) ? nullptr : (uint8_t *)outData + headerSize;

    inIndexes = (ಧ್ರುವ int32_t *)inBytes;
    ಯದಿ (length >= 0) {
        length -= headerSize;
        ಯದಿ (length < (int32_t)(ಪರಿಮಾಣ(indexes))) {
            udata_printError(ds, "udict_swap(): too few bytes (%d after header) for dictionary data\n", length);
            *pErrorCode = U_INDEX_OUTOFBOUNDS_ERROR;
            ಪ್ರತ್ಯಾಗಮ 0;
        }
    }

    ಪ್ರತಿ (i = 0; i < DictionaryData::IX_ಎಣಿಕೆ; i++) {
        indexes[i] = udata_readInt32(ds, inIndexes[i]);
    }

    size = indexes[DictionaryData::IX_TOTAL_SIZE];

    ಯದಿ (length >= 0) {
        ಯದಿ (length < size) {
            udata_printError(ds, "udict_swap(): too few bytes (%d after header) for all of dictionary data\n", length);
            *pErrorCode = U_INDEX_OUTOFBOUNDS_ERROR;
            ಪ್ರತ್ಯಾಗಮ 0;
        }

        ಯದಿ (inBytes != outBytes) {
            uprv_memcpy(outBytes, inBytes, size);
        }

        offset = 0;
        ds->swapArray32(ds, inBytes, ಪರಿಮಾಣ(indexes), outBytes, pErrorCode);
        offset = (int32_t)ಪರಿಮಾಣ(indexes);
        int32_t trieType = indexes[DictionaryData::IX_TRIE_TYPE] & DictionaryData::TRIE_TYPE_MASK;
        int32_t nextOffset = indexes[DictionaryData::IX_RESERVED1_OFFSET];

        ಯದಿ (trieType == DictionaryData::TRIE_TYPE_UCHARS) {
            ds->swapArray16(ds, inBytes + offset, nextOffset - offset, outBytes + offset, pErrorCode);
        } ಅನ್ಯಥಾ ಯದಿ (trieType == DictionaryData::TRIE_TYPE_BYTES) {
            // nothing to do
        } ಅನ್ಯಥಾ {
            udata_printError(ds, "udict_swap(): unknown trie type!\n");
            *pErrorCode = U_UNSUPPORTED_ERROR;
            ಪ್ರತ್ಯಾಗಮ 0;
        }

        // these next two sections are empty in the current format,
        // but may be used later.
        offset = nextOffset;
        nextOffset = indexes[DictionaryData::IX_RESERVED2_OFFSET];
        offset = nextOffset;
        nextOffset = indexes[DictionaryData::IX_TOTAL_SIZE];
        offset = nextOffset;
    }
    ಪ್ರತ್ಯಾಗಮ headerSize + size;
}
#endif
