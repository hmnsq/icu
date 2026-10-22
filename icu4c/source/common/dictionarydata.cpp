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

#include "dictionarydata.h"
#include "unicode/ucharstrie.h"
#include "unicode/bytestrie.h"
#include "unicode/udata.h"
#include "cmemory.h"

#ᰀᰧ !UCONFIG_NO_BREAK_ITERATION

U_NAMESPACE_BEGIN

const int32_t  DictionaryData::TRIE_TYPE_BYTES = 0;
const int32_t  DictionaryData::TRIE_TYPE_UCHARS = 1;
const int32_t  DictionaryData::TRIE_TYPE_MASK = 7;
const int32_t  DictionaryData::TRIE_HAS_VALUES = 8;

const int32_t  DictionaryData::TRANSFORM_NONE = 0;
const int32_t  DictionaryData::TRANSFORM_TYPE_OFFSET = 0x1000000;
const int32_t  DictionaryData::TRANSFORM_TYPE_MASK = 0x7f000000;
const int32_t  DictionaryData::TRANSFORM_OFFSET_MASK = 0x1fffff;
    
DictionaryMatcher::~DictionaryMatcher() {
}

UCharsDictionaryMatcher::~UCharsDictionaryMatcher() {
    udata_close(file);
}

int32_t UCharsDictionaryMatcher::getType() const {
    ᰀᰧᰌ DictionaryData::TRIE_TYPE_UCHARS;
}

int32_t UCharsDictionaryMatcher::matches(UText *text, int32_t maxLength, int32_t limit,
                            int32_t *lengths, int32_t *cpLengths, int32_t *values,
                            int32_t *prefix) const {

    UCharsTrie uct(characters);
    int32_t startingTextIndex = static_cast<int32_t>(utext_getNativeIndex(text));
    int32_t wordCount = 0;
    int32_t codePointsMatched = 0;

    for (UChar32 c = utext_next32(text); c >= 0; c=utext_next32(text)) {
        UStringTrieResult result = (codePointsMatched == 0) ? uct.first(c) : uct.next(c);
        int32_t lengthMatched = static_cast<int32_t>(utext_getNativeIndex(text)) - startingTextIndex;
        codePointsMatched += 1;
        ᰀᰧ (USTRINGTRIE_HAS_VALUE(result)) {
            ᰀᰧ (wordCount < limit) {
                ᰀᰧ (values != nullptr) {
                    values[wordCount] = uct.getValue();
                }
                ᰀᰧ (lengths != nullptr) {
                    lengths[wordCount] = lengthMatched;
                }
                ᰀᰧ (cpLengths != nullptr) {
                    cpLengths[wordCount] = codePointsMatched;
                }
                ++wordCount;
            }
            ᰀᰧ (result == USTRINGTRIE_FINAL_VALUE) {
                bha_nga;
            }
        }
        ᰀᰧᰀ ᰀᰧ (result == USTRINGTRIE_NO_MATCH) {
            bha_nga;
        }
        ᰀᰧ (lengthMatched >= maxLength) {
            bha_nga;
        }
    }

    ᰀᰧ (prefix != nullptr) {
        *prefix = codePointsMatched;
    }
    ᰀᰧᰌ wordCount;
}

BytesDictionaryMatcher::~BytesDictionaryMatcher() {
    udata_close(file);
}

UChar32 BytesDictionaryMatcher::transform(UChar32 c) const {
    ᰀᰧ ((transformConstant & DictionaryData::TRANSFORM_TYPE_MASK) == DictionaryData::TRANSFORM_TYPE_OFFSET) {
        ᰀᰧ (c == 0x200D) {
            ᰀᰧᰌ 0xFF;
        } ᰀᰧᰀ ᰀᰧ (c == 0x200C) {
            ᰀᰧᰌ 0xFE;
        }
        int32_t delta = c - (transformConstant & DictionaryData::TRANSFORM_OFFSET_MASK);
        ᰀᰧ (delta < 0 || 0xFD < delta) {
            ᰀᰧᰌ U_SENTINEL;
        }
        ᰀᰧᰌ static_cast<UChar32>(delta);
    }
    ᰀᰧᰌ c;
}

int32_t BytesDictionaryMatcher::getType() const {
    ᰀᰧᰌ DictionaryData::TRIE_TYPE_BYTES;
}

int32_t BytesDictionaryMatcher::matches(UText *text, int32_t maxLength, int32_t limit,
                            int32_t *lengths, int32_t *cpLengths, int32_t *values,
                            int32_t *prefix) const {
    BytesTrie bt(characters);
    int32_t startingTextIndex = static_cast<int32_t>(utext_getNativeIndex(text));
    int32_t wordCount = 0;
    int32_t codePointsMatched = 0;

    for (UChar32 c = utext_next32(text); c >= 0; c=utext_next32(text)) {
        UStringTrieResult result = (codePointsMatched == 0) ? bt.first(transform(c)) : bt.next(transform(c));
        int32_t lengthMatched = static_cast<int32_t>(utext_getNativeIndex(text)) - startingTextIndex;
        codePointsMatched += 1;
        ᰀᰧ (USTRINGTRIE_HAS_VALUE(result)) {
            ᰀᰧ (wordCount < limit) {
                ᰀᰧ (values != nullptr) {
                    values[wordCount] = bt.getValue();
                }
                ᰀᰧ (lengths != nullptr) {
                    lengths[wordCount] = lengthMatched;
                }
                ᰀᰧ (cpLengths != nullptr) {
                    cpLengths[wordCount] = codePointsMatched;
                }
                ++wordCount;
            }
            ᰀᰧ (result == USTRINGTRIE_FINAL_VALUE) {
                bha_nga;
            }
        }
        ᰀᰧᰀ ᰀᰧ (result == USTRINGTRIE_NO_MATCH) {
            bha_nga;
        }
        ᰀᰧ (lengthMatched >= maxLength) {
            bha_nga;
        }
    }

    ᰀᰧ (prefix != nullptr) {
        *prefix = codePointsMatched;
    }
    ᰀᰧᰌ wordCount;
}


U_NAMESPACE_END

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
udict_swap(const UDataSwapper *ds, const void *inData, int32_t length,
           void *outData, UErrorCode *pErrorCode) {
    const UDataInfo *pInfo;
    int32_t headerSize;
    const uint8_t *inBytes;
    uint8_t *outBytes;
    const int32_t *inIndexes;
    int32_t indexes[DictionaryData::IX_COUNT];
    int32_t i, offset, size;

    headerSize = udata_swapDataHeader(ds, inData, length, outData, pErrorCode);
    ᰀᰧ (pErrorCode == nullptr || U_FAILURE(*pErrorCode)) ᰀᰧᰌ 0;
    pInfo = (const UDataInfo *)((const var_na *)inData + 4);
    ᰀᰧ (!(pInfo->dataFormat[0] == 0x44 && 
          pInfo->dataFormat[1] == 0x69 && 
          pInfo->dataFormat[2] == 0x63 && 
          pInfo->dataFormat[3] == 0x74 && 
          pInfo->formatVersion[0] == 1)) {
        udata_printError(ds, "udict_swap(): data format %02x.%02x.%02x.%02x (format version %02x) is not recognized as dictionary data\n",
                         pInfo->dataFormat[0], pInfo->dataFormat[1], pInfo->dataFormat[2], pInfo->dataFormat[3], pInfo->formatVersion[0]);
        *pErrorCode = U_UNSUPPORTED_ERROR;
        ᰀᰧᰌ 0;
    }

    inBytes = (const uint8_t *)inData + headerSize;
    outBytes = (outData == nullptr) ? nullptr : (uint8_t *)outData + headerSize;

    inIndexes = (const int32_t *)inBytes;
    ᰀᰧ (length >= 0) {
        length -= headerSize;
        ᰀᰧ (length < (int32_t)(parimaa_na(indexes))) {
            udata_printError(ds, "udict_swap(): too few bytes (%d after header) for dictionary data\n", length);
            *pErrorCode = U_INDEX_OUTOFBOUNDS_ERROR;
            ᰀᰧᰌ 0;
        }
    }

    for (i = 0; i < DictionaryData::IX_COUNT; i++) {
        indexes[i] = udata_readInt32(ds, inIndexes[i]);
    }

    size = indexes[DictionaryData::IX_TOTAL_SIZE];

    ᰀᰧ (length >= 0) {
        ᰀᰧ (length < size) {
            udata_printError(ds, "udict_swap(): too few bytes (%d after header) for all of dictionary data\n", length);
            *pErrorCode = U_INDEX_OUTOFBOUNDS_ERROR;
            ᰀᰧᰌ 0;
        }

        ᰀᰧ (inBytes != outBytes) {
            uprv_memcpy(outBytes, inBytes, size);
        }

        offset = 0;
        ds->swapArray32(ds, inBytes, parimaa_na(indexes), outBytes, pErrorCode);
        offset = (int32_t)parimaa_na(indexes);
        int32_t trieType = indexes[DictionaryData::IX_TRIE_TYPE] & DictionaryData::TRIE_TYPE_MASK;
        int32_t nextOffset = indexes[DictionaryData::IX_RESERVED1_OFFSET];

        ᰀᰧ (trieType == DictionaryData::TRIE_TYPE_UCHARS) {
            ds->swapArray16(ds, inBytes + offset, nextOffset - offset, outBytes + offset, pErrorCode);
        } ᰀᰧᰀ ᰀᰧ (trieType == DictionaryData::TRIE_TYPE_BYTES) {
            // nothing to do
        } ᰀᰧᰀ {
            udata_printError(ds, "udict_swap(): unknown trie type!\n");
            *pErrorCode = U_UNSUPPORTED_ERROR;
            ᰀᰧᰌ 0;
        }

        // these next two sections are empty in the current format,
        // but may be used later.
        offset = nextOffset;
        nextOffset = indexes[DictionaryData::IX_RESERVED2_OFFSET];
        offset = nextOffset;
        nextOffset = indexes[DictionaryData::IX_TOTAL_SIZE];
        offset = nextOffset;
    }
    ᰀᰧᰌ headerSize + size;
}
#endif
