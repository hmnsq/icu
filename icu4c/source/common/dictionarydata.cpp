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

#ضمن "dictionarydata.h"
#ضمن "unicode/ucharstrie.h"
#ضمن "unicode/bytestrie.h"
#ضمن "unicode/udata.h"
#ضمن "cmemory.h"

#إذا !UCONFIG_NO_BREAK_ITERATION

U_NAMESPACE_BEGIN

ثابت int32_t  DictionaryData::TRIE_TYPE_BYTES = 0;
ثابت int32_t  DictionaryData::TRIE_TYPE_UCHARS = 1;
ثابت int32_t  DictionaryData::TRIE_TYPE_MASK = 7;
ثابت int32_t  DictionaryData::TRIE_HAS_بەهاکان = 8;

ثابت int32_t  DictionaryData::TRANSFORM_NONE = 0;
ثابت int32_t  DictionaryData::TRANSFORM_TYPE_OFFSET = 0x1000000;
ثابت int32_t  DictionaryData::TRANSFORM_TYPE_MASK = 0x7f000000;
ثابت int32_t  DictionaryData::TRANSFORM_OFFSET_MASK = 0x1fffff;
    
DictionaryMatcher::~DictionaryMatcher() {
}

UCharsDictionaryMatcher::~UCharsDictionaryMatcher() {
    udata_close(file);
}

int32_t UCharsDictionaryMatcher::getType() ثابت {
    أرجع DictionaryData::TRIE_TYPE_UCHARS;
}

int32_t UCharsDictionaryMatcher::matches(UText *text, int32_t maxLength, int32_t limit,
                            int32_t *lengths, int32_t *cpLengths, int32_t *بەهاکان,
                            int32_t *prefix) ثابت {

    UCharsTrie uct(characters);
    int32_t startingTextIndex = static_cast<int32_t>(utext_getNativeIndex(text));
    int32_t word_ژماردن = 0;
    int32_t codePointsMatched = 0;

    لكل (UChar32 c = utext_next32(text); c >= 0; c=utext_next32(text)) {
        UStringTrieResult result = (codePointsMatched == 0) ? uct.first(c) : uct.next(c);
        int32_t lengthMatched = static_cast<int32_t>(utext_getNativeIndex(text)) - startingTextIndex;
        codePointsMatched += 1;
        إذا (USTRINGTRIE_HAS_بەها(result)) {
            إذا (word_ژماردن < limit) {
                إذا (بەهاکان != nullptr) {
                    بەهاکان[word_ژماردن] = uct.get_بەها();
                }
                إذا (lengths != nullptr) {
                    lengths[word_ژماردن] = lengthMatched;
                }
                إذا (cpLengths != nullptr) {
                    cpLengths[word_ژماردن] = codePointsMatched;
                }
                ++word_ژماردن;
            }
            إذا (result == USTRINGTRIE_FINAL_بەها) {
                اكسر;
            }
        }
        وإلا إذا (result == USTRINGTRIE_NO_MATCH) {
            اكسر;
        }
        إذا (lengthMatched >= maxLength) {
            اكسر;
        }
    }

    إذا (prefix != nullptr) {
        *prefix = codePointsMatched;
    }
    أرجع word_ژماردن;
}

BytesDictionaryMatcher::~BytesDictionaryMatcher() {
    udata_close(file);
}

UChar32 BytesDictionaryMatcher::transform(UChar32 c) ثابت {
    إذا ((transformConstant & DictionaryData::TRANSFORM_TYPE_MASK) == DictionaryData::TRANSFORM_TYPE_OFFSET) {
        إذا (c == 0x200D) {
            أرجع 0xFF;
        } وإلا إذا (c == 0x200C) {
            أرجع 0xFE;
        }
        int32_t delta = c - (transformConstant & DictionaryData::TRANSFORM_OFFSET_MASK);
        إذا (delta < 0 || 0xFD < delta) {
            أرجع U_SENTINEL;
        }
        أرجع static_cast<UChar32>(delta);
    }
    أرجع c;
}

int32_t BytesDictionaryMatcher::getType() ثابت {
    أرجع DictionaryData::TRIE_TYPE_BYTES;
}

int32_t BytesDictionaryMatcher::matches(UText *text, int32_t maxLength, int32_t limit,
                            int32_t *lengths, int32_t *cpLengths, int32_t *بەهاکان,
                            int32_t *prefix) ثابت {
    BytesTrie bt(characters);
    int32_t startingTextIndex = static_cast<int32_t>(utext_getNativeIndex(text));
    int32_t word_ژماردن = 0;
    int32_t codePointsMatched = 0;

    لكل (UChar32 c = utext_next32(text); c >= 0; c=utext_next32(text)) {
        UStringTrieResult result = (codePointsMatched == 0) ? bt.first(transform(c)) : bt.next(transform(c));
        int32_t lengthMatched = static_cast<int32_t>(utext_getNativeIndex(text)) - startingTextIndex;
        codePointsMatched += 1;
        إذا (USTRINGTRIE_HAS_بەها(result)) {
            إذا (word_ژماردن < limit) {
                إذا (بەهاکان != nullptr) {
                    بەهاکان[word_ژماردن] = bt.get_بەها();
                }
                إذا (lengths != nullptr) {
                    lengths[word_ژماردن] = lengthMatched;
                }
                إذا (cpLengths != nullptr) {
                    cpLengths[word_ژماردن] = codePointsMatched;
                }
                ++word_ژماردن;
            }
            إذا (result == USTRINGTRIE_FINAL_بەها) {
                اكسر;
            }
        }
        وإلا إذا (result == USTRINGTRIE_NO_MATCH) {
            اكسر;
        }
        إذا (lengthMatched >= maxLength) {
            اكسر;
        }
    }

    إذا (prefix != nullptr) {
        *prefix = codePointsMatched;
    }
    أرجع word_ژماردن;
}


U_NAMESPACE_END

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
udict_swap(ثابت UDataSwapper *ds, ثابت فراغ *inData, int32_t length,
           فراغ *outData, UErrorCode *pErrorCode) {
    ثابت UDataInfo *pInfo;
    int32_t headerSize;
    ثابت uint8_t *inBytes;
    uint8_t *outBytes;
    ثابت int32_t *inIndexes;
    int32_t indexes[DictionaryData::IX_ژماردن];
    int32_t i, offset, size;

    headerSize = udata_swapDataHeader(ds, inData, length, outData, pErrorCode);
    إذا (pErrorCode == nullptr || U_FAILURE(*pErrorCode)) أرجع 0;
    pInfo = (ثابت UDataInfo *)((ثابت حرف *)inData + 4);
    إذا (!(pInfo->dataFormat[0] == 0x44 && 
          pInfo->dataFormat[1] == 0x69 && 
          pInfo->dataFormat[2] == 0x63 && 
          pInfo->dataFormat[3] == 0x74 && 
          pInfo->formatVersion[0] == 1)) {
        udata_printError(ds, "udict_swap(): data format %02x.%02x.%02x.%02x (format version %02x) is not recognized as dictionary data\n",
                         pInfo->dataFormat[0], pInfo->dataFormat[1], pInfo->dataFormat[2], pInfo->dataFormat[3], pInfo->formatVersion[0]);
        *pErrorCode = U_UNSUPPORTED_ERROR;
        أرجع 0;
    }

    inBytes = (ثابت uint8_t *)inData + headerSize;
    outBytes = (outData == nullptr) ? nullptr : (uint8_t *)outData + headerSize;

    inIndexes = (ثابت int32_t *)inBytes;
    إذا (length >= 0) {
        length -= headerSize;
        إذا (length < (int32_t)(حجم(indexes))) {
            udata_printError(ds, "udict_swap(): too few bytes (%d after header) for dictionary data\n", length);
            *pErrorCode = U_INDEX_OUTOFBOUNDS_ERROR;
            أرجع 0;
        }
    }

    لكل (i = 0; i < DictionaryData::IX_ژماردن; i++) {
        indexes[i] = udata_readInt32(ds, inIndexes[i]);
    }

    size = indexes[DictionaryData::IX_TOTAL_SIZE];

    إذا (length >= 0) {
        إذا (length < size) {
            udata_printError(ds, "udict_swap(): too few bytes (%d after header) for all of dictionary data\n", length);
            *pErrorCode = U_INDEX_OUTOFBOUNDS_ERROR;
            أرجع 0;
        }

        إذا (inBytes != outBytes) {
            uprv_memcpy(outBytes, inBytes, size);
        }

        offset = 0;
        ds->swapArray32(ds, inBytes, حجم(indexes), outBytes, pErrorCode);
        offset = (int32_t)حجم(indexes);
        int32_t trieType = indexes[DictionaryData::IX_TRIE_TYPE] & DictionaryData::TRIE_TYPE_MASK;
        int32_t nextOffset = indexes[DictionaryData::IX_RESERVED1_OFFSET];

        إذا (trieType == DictionaryData::TRIE_TYPE_UCHARS) {
            ds->swapArray16(ds, inBytes + offset, nextOffset - offset, outBytes + offset, pErrorCode);
        } وإلا إذا (trieType == DictionaryData::TRIE_TYPE_BYTES) {
            // nothing to do
        } وإلا {
            udata_printError(ds, "udict_swap(): unknown trie type!\n");
            *pErrorCode = U_UNSUPPORTED_ERROR;
            أرجع 0;
        }

        // these next two sections are empty in the current format,
        // but may be used later.
        offset = nextOffset;
        nextOffset = indexes[DictionaryData::IX_RESERVED2_OFFSET];
        offset = nextOffset;
        nextOffset = indexes[DictionaryData::IX_TOTAL_SIZE];
        offset = nextOffset;
    }
    أرجع headerSize + size;
}
#endif
