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

#अन्तर्भाव "dictionarydata.h"
#अन्तर्भाव "unicode/ucharstrie.h"
#अन्तर्भाव "unicode/bytestrie.h"
#अन्तर्भाव "unicode/udata.h"
#अन्तर्भाव "cmemory.h"

#जइ !UCONFIG_NO_BREAK_ITERATION

U_NAMESPACE_BEGIN

ध्रुव int32_t  DictionaryData::TRIE_TYPE_BYTES = 0;
ध्रुव int32_t  DictionaryData::TRIE_TYPE_UCHARS = 1;
ध्रुव int32_t  DictionaryData::TRIE_TYPE_MASK = 7;
ध्रुव int32_t  DictionaryData::TRIE_HAS_मानानि = 8;

ध्रुव int32_t  DictionaryData::TRANSFORM_NONE = 0;
ध्रुव int32_t  DictionaryData::TRANSFORM_TYPE_OFFSET = 0x1000000;
ध्रुव int32_t  DictionaryData::TRANSFORM_TYPE_MASK = 0x7f000000;
ध्रुव int32_t  DictionaryData::TRANSFORM_OFFSET_MASK = 0x1fffff;
    
DictionaryMatcher::~DictionaryMatcher() {
}

UCharsDictionaryMatcher::~UCharsDictionaryMatcher() {
    udata_close(file);
}

int32_t UCharsDictionaryMatcher::getType() ध्रुव {
    प्रत्यागम DictionaryData::TRIE_TYPE_UCHARS;
}

int32_t UCharsDictionaryMatcher::matches(UText *text, int32_t maxLength, int32_t limit,
                            int32_t *lengths, int32_t *cpLengths, int32_t *मानानि,
                            int32_t *prefix) ध्रुव {

    UCharsTrie uct(characters);
    int32_t startingTextIndex = static_cast<int32_t>(utext_getNativeIndex(text));
    int32_t word_गणनम् = 0;
    int32_t codePointsMatched = 0;

    प्रति (UChar32 c = utext_next32(text); c >= 0; c=utext_next32(text)) {
        UStringTrieResult result = (codePointsMatched == 0) ? uct.first(c) : uct.next(c);
        int32_t lengthMatched = static_cast<int32_t>(utext_getNativeIndex(text)) - startingTextIndex;
        codePointsMatched += 1;
        जइ (USTRINGTRIE_HAS_मानम्(result)) {
            जइ (word_गणनम् < limit) {
                जइ (मानानि != nullptr) {
                    मानानि[word_गणनम्] = uct.get_मानम्();
                }
                जइ (lengths != nullptr) {
                    lengths[word_गणनम्] = lengthMatched;
                }
                जइ (cpLengths != nullptr) {
                    cpLengths[word_गणनम्] = codePointsMatched;
                }
                ++word_गणनम्;
            }
            जइ (result == USTRINGTRIE_FINAL_मानम्) {
                भङ्ग;
            }
        }
        अण्णहा जइ (result == USTRINGTRIE_NO_MATCH) {
            भङ्ग;
        }
        जइ (lengthMatched >= maxLength) {
            भङ्ग;
        }
    }

    जइ (prefix != nullptr) {
        *prefix = codePointsMatched;
    }
    प्रत्यागम word_गणनम्;
}

BytesDictionaryMatcher::~BytesDictionaryMatcher() {
    udata_close(file);
}

UChar32 BytesDictionaryMatcher::transform(UChar32 c) ध्रुव {
    जइ ((transformConstant & DictionaryData::TRANSFORM_TYPE_MASK) == DictionaryData::TRANSFORM_TYPE_OFFSET) {
        जइ (c == 0x200D) {
            प्रत्यागम 0xFF;
        } अण्णहा जइ (c == 0x200C) {
            प्रत्यागम 0xFE;
        }
        int32_t delta = c - (transformConstant & DictionaryData::TRANSFORM_OFFSET_MASK);
        जइ (delta < 0 || 0xFD < delta) {
            प्रत्यागम U_SENTINEL;
        }
        प्रत्यागम static_cast<UChar32>(delta);
    }
    प्रत्यागम c;
}

int32_t BytesDictionaryMatcher::getType() ध्रुव {
    प्रत्यागम DictionaryData::TRIE_TYPE_BYTES;
}

int32_t BytesDictionaryMatcher::matches(UText *text, int32_t maxLength, int32_t limit,
                            int32_t *lengths, int32_t *cpLengths, int32_t *मानानि,
                            int32_t *prefix) ध्रुव {
    BytesTrie bt(characters);
    int32_t startingTextIndex = static_cast<int32_t>(utext_getNativeIndex(text));
    int32_t word_गणनम् = 0;
    int32_t codePointsMatched = 0;

    प्रति (UChar32 c = utext_next32(text); c >= 0; c=utext_next32(text)) {
        UStringTrieResult result = (codePointsMatched == 0) ? bt.first(transform(c)) : bt.next(transform(c));
        int32_t lengthMatched = static_cast<int32_t>(utext_getNativeIndex(text)) - startingTextIndex;
        codePointsMatched += 1;
        जइ (USTRINGTRIE_HAS_मानम्(result)) {
            जइ (word_गणनम् < limit) {
                जइ (मानानि != nullptr) {
                    मानानि[word_गणनम्] = bt.get_मानम्();
                }
                जइ (lengths != nullptr) {
                    lengths[word_गणनम्] = lengthMatched;
                }
                जइ (cpLengths != nullptr) {
                    cpLengths[word_गणनम्] = codePointsMatched;
                }
                ++word_गणनम्;
            }
            जइ (result == USTRINGTRIE_FINAL_मानम्) {
                भङ्ग;
            }
        }
        अण्णहा जइ (result == USTRINGTRIE_NO_MATCH) {
            भङ्ग;
        }
        जइ (lengthMatched >= maxLength) {
            भङ्ग;
        }
    }

    जइ (prefix != nullptr) {
        *prefix = codePointsMatched;
    }
    प्रत्यागम word_गणनम्;
}


U_NAMESPACE_END

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
udict_swap(ध्रुव UDataSwapper *ds, ध्रुव शून्य *inData, int32_t length,
           शून्य *outData, UErrorCode *pErrorCode) {
    ध्रुव UDataInfo *pInfo;
    int32_t headerSize;
    ध्रुव uint8_t *inBytes;
    uint8_t *outBytes;
    ध्रुव int32_t *inIndexes;
    int32_t indexes[DictionaryData::IX_गणनम्];
    int32_t i, offset, size;

    headerSize = udata_swapDataHeader(ds, inData, length, outData, pErrorCode);
    जइ (pErrorCode == nullptr || U_FAILURE(*pErrorCode)) प्रत्यागम 0;
    pInfo = (ध्रुव UDataInfo *)((ध्रुव वर्ण *)inData + 4);
    जइ (!(pInfo->dataFormat[0] == 0x44 && 
          pInfo->dataFormat[1] == 0x69 && 
          pInfo->dataFormat[2] == 0x63 && 
          pInfo->dataFormat[3] == 0x74 && 
          pInfo->formatVersion[0] == 1)) {
        udata_printError(ds, "udict_swap(): data format %02x.%02x.%02x.%02x (format version %02x) is not recognized as dictionary data\n",
                         pInfo->dataFormat[0], pInfo->dataFormat[1], pInfo->dataFormat[2], pInfo->dataFormat[3], pInfo->formatVersion[0]);
        *pErrorCode = U_UNSUPPORTED_ERROR;
        प्रत्यागम 0;
    }

    inBytes = (ध्रुव uint8_t *)inData + headerSize;
    outBytes = (outData == nullptr) ? nullptr : (uint8_t *)outData + headerSize;

    inIndexes = (ध्रुव int32_t *)inBytes;
    जइ (length >= 0) {
        length -= headerSize;
        जइ (length < (int32_t)(परिमाण(indexes))) {
            udata_printError(ds, "udict_swap(): too few bytes (%d after header) for dictionary data\n", length);
            *pErrorCode = U_INDEX_OUTOFBOUNDS_ERROR;
            प्रत्यागम 0;
        }
    }

    प्रति (i = 0; i < DictionaryData::IX_गणनम्; i++) {
        indexes[i] = udata_readInt32(ds, inIndexes[i]);
    }

    size = indexes[DictionaryData::IX_TOTAL_SIZE];

    जइ (length >= 0) {
        जइ (length < size) {
            udata_printError(ds, "udict_swap(): too few bytes (%d after header) for all of dictionary data\n", length);
            *pErrorCode = U_INDEX_OUTOFBOUNDS_ERROR;
            प्रत्यागम 0;
        }

        जइ (inBytes != outBytes) {
            uprv_memcpy(outBytes, inBytes, size);
        }

        offset = 0;
        ds->swapArray32(ds, inBytes, परिमाण(indexes), outBytes, pErrorCode);
        offset = (int32_t)परिमाण(indexes);
        int32_t trieType = indexes[DictionaryData::IX_TRIE_TYPE] & DictionaryData::TRIE_TYPE_MASK;
        int32_t nextOffset = indexes[DictionaryData::IX_RESERVED1_OFFSET];

        जइ (trieType == DictionaryData::TRIE_TYPE_UCHARS) {
            ds->swapArray16(ds, inBytes + offset, nextOffset - offset, outBytes + offset, pErrorCode);
        } अण्णहा जइ (trieType == DictionaryData::TRIE_TYPE_BYTES) {
            // nothing to do
        } अण्णहा {
            udata_printError(ds, "udict_swap(): unknown trie type!\n");
            *pErrorCode = U_UNSUPPORTED_ERROR;
            प्रत्यागम 0;
        }

        // these next two sections are empty in the current format,
        // but may be used later.
        offset = nextOffset;
        nextOffset = indexes[DictionaryData::IX_RESERVED2_OFFSET];
        offset = nextOffset;
        nextOffset = indexes[DictionaryData::IX_TOTAL_SIZE];
        offset = nextOffset;
    }
    प्रत्यागम headerSize + size;
}
#endif
