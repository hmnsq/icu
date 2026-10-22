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

#شامل "dictionarydata.h"
#شامل "unicode/ucharstrie.h"
#شامل "unicode/bytestrie.h"
#شامل "unicode/udata.h"
#شامل "cmemory.h"

#اگر !UCONFIG_NO_BREAK_ITERATION

U_NAMESPACE_BEGIN

ثابت int32_t  DictionaryData::TRIE_TYPE_BYTES = 0;
ثابت int32_t  DictionaryData::TRIE_TYPE_UCHARS = 1;
ثابت int32_t  DictionaryData::TRIE_TYPE_MASK = 7;
ثابت int32_t  DictionaryData::TRIE_HAS_مقادیر = 8;

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
    بازگشت DictionaryData::TRIE_TYPE_UCHARS;
}

int32_t UCharsDictionaryMatcher::matches(UText *text, int32_t maxLength, int32_t limit,
                            int32_t *lengths, int32_t *cpLengths, int32_t *مقادیر,
                            int32_t *prefix) ثابت {

    UCharsTrie uct(characters);
    int32_t startingTextIndex = static_cast<int32_t>(utext_getNativeIndex(text));
    int32_t word_شمار = 0;
    int32_t codePointsMatched = 0;

    برای (UChar32 c = utext_next32(text); c >= 0; c=utext_next32(text)) {
        UStringTrieResult result = (codePointsMatched == 0) ? uct.first(c) : uct.next(c);
        int32_t lengthMatched = static_cast<int32_t>(utext_getNativeIndex(text)) - startingTextIndex;
        codePointsMatched += 1;
        اگر (USTRINGTRIE_HAS_مقدار(result)) {
            اگر (word_شمار < limit) {
                اگر (مقادیر != nullptr) {
                    مقادیر[word_شمار] = uct.get_مقدار();
                }
                اگر (lengths != nullptr) {
                    lengths[word_شمار] = lengthMatched;
                }
                اگر (cpLengths != nullptr) {
                    cpLengths[word_شمار] = codePointsMatched;
                }
                ++word_شمار;
            }
            اگر (result == USTRINGTRIE_FINAL_مقدار) {
                بشکن;
            }
        }
        وگرنه اگر (result == USTRINGTRIE_NO_MATCH) {
            بشکن;
        }
        اگر (lengthMatched >= maxLength) {
            بشکن;
        }
    }

    اگر (prefix != nullptr) {
        *prefix = codePointsMatched;
    }
    بازگشت word_شمار;
}

BytesDictionaryMatcher::~BytesDictionaryMatcher() {
    udata_close(file);
}

UChar32 BytesDictionaryMatcher::transform(UChar32 c) ثابت {
    اگر ((transformConstant & DictionaryData::TRANSFORM_TYPE_MASK) == DictionaryData::TRANSFORM_TYPE_OFFSET) {
        اگر (c == 0x200D) {
            بازگشت 0xFF;
        } وگرنه اگر (c == 0x200C) {
            بازگشت 0xFE;
        }
        int32_t delta = c - (transformConstant & DictionaryData::TRANSFORM_OFFSET_MASK);
        اگر (delta < 0 || 0xFD < delta) {
            بازگشت U_SENTINEL;
        }
        بازگشت static_cast<UChar32>(delta);
    }
    بازگشت c;
}

int32_t BytesDictionaryMatcher::getType() ثابت {
    بازگشت DictionaryData::TRIE_TYPE_BYTES;
}

int32_t BytesDictionaryMatcher::matches(UText *text, int32_t maxLength, int32_t limit,
                            int32_t *lengths, int32_t *cpLengths, int32_t *مقادیر,
                            int32_t *prefix) ثابت {
    BytesTrie bt(characters);
    int32_t startingTextIndex = static_cast<int32_t>(utext_getNativeIndex(text));
    int32_t word_شمار = 0;
    int32_t codePointsMatched = 0;

    برای (UChar32 c = utext_next32(text); c >= 0; c=utext_next32(text)) {
        UStringTrieResult result = (codePointsMatched == 0) ? bt.first(transform(c)) : bt.next(transform(c));
        int32_t lengthMatched = static_cast<int32_t>(utext_getNativeIndex(text)) - startingTextIndex;
        codePointsMatched += 1;
        اگر (USTRINGTRIE_HAS_مقدار(result)) {
            اگر (word_شمار < limit) {
                اگر (مقادیر != nullptr) {
                    مقادیر[word_شمار] = bt.get_مقدار();
                }
                اگر (lengths != nullptr) {
                    lengths[word_شمار] = lengthMatched;
                }
                اگر (cpLengths != nullptr) {
                    cpLengths[word_شمار] = codePointsMatched;
                }
                ++word_شمار;
            }
            اگر (result == USTRINGTRIE_FINAL_مقدار) {
                بشکن;
            }
        }
        وگرنه اگر (result == USTRINGTRIE_NO_MATCH) {
            بشکن;
        }
        اگر (lengthMatched >= maxLength) {
            بشکن;
        }
    }

    اگر (prefix != nullptr) {
        *prefix = codePointsMatched;
    }
    بازگشت word_شمار;
}


U_NAMESPACE_END

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
udict_swap(ثابت UDataSwapper *ds, ثابت تهی *inData, int32_t length,
           تهی *outData, UErrorCode *pErrorCode) {
    ثابت UDataInfo *pInfo;
    int32_t headerSize;
    ثابت uint8_t *inBytes;
    uint8_t *outBytes;
    ثابت int32_t *inIndexes;
    int32_t indexes[DictionaryData::IX_شمار];
    int32_t i, offset, size;

    headerSize = udata_swapDataHeader(ds, inData, length, outData, pErrorCode);
    اگر (pErrorCode == nullptr || U_FAILURE(*pErrorCode)) بازگشت 0;
    pInfo = (ثابت UDataInfo *)((ثابت نویسه *)inData + 4);
    اگر (!(pInfo->dataFormat[0] == 0x44 && 
          pInfo->dataFormat[1] == 0x69 && 
          pInfo->dataFormat[2] == 0x63 && 
          pInfo->dataFormat[3] == 0x74 && 
          pInfo->formatVersion[0] == 1)) {
        udata_printError(ds, "udict_swap(): data format %02x.%02x.%02x.%02x (format version %02x) is not recognized as dictionary data\n",
                         pInfo->dataFormat[0], pInfo->dataFormat[1], pInfo->dataFormat[2], pInfo->dataFormat[3], pInfo->formatVersion[0]);
        *pErrorCode = U_UNSUPPORTED_ERROR;
        بازگشت 0;
    }

    inBytes = (ثابت uint8_t *)inData + headerSize;
    outBytes = (outData == nullptr) ? nullptr : (uint8_t *)outData + headerSize;

    inIndexes = (ثابت int32_t *)inBytes;
    اگر (length >= 0) {
        length -= headerSize;
        اگر (length < (int32_t)(اندازه(indexes))) {
            udata_printError(ds, "udict_swap(): too few bytes (%d after header) for dictionary data\n", length);
            *pErrorCode = U_INDEX_OUTOFBOUNDS_ERROR;
            بازگشت 0;
        }
    }

    برای (i = 0; i < DictionaryData::IX_شمار; i++) {
        indexes[i] = udata_readInt32(ds, inIndexes[i]);
    }

    size = indexes[DictionaryData::IX_TOTAL_SIZE];

    اگر (length >= 0) {
        اگر (length < size) {
            udata_printError(ds, "udict_swap(): too few bytes (%d after header) for all of dictionary data\n", length);
            *pErrorCode = U_INDEX_OUTOFBOUNDS_ERROR;
            بازگشت 0;
        }

        اگر (inBytes != outBytes) {
            uprv_memcpy(outBytes, inBytes, size);
        }

        offset = 0;
        ds->swapArray32(ds, inBytes, اندازه(indexes), outBytes, pErrorCode);
        offset = (int32_t)اندازه(indexes);
        int32_t trieType = indexes[DictionaryData::IX_TRIE_TYPE] & DictionaryData::TRIE_TYPE_MASK;
        int32_t nextOffset = indexes[DictionaryData::IX_RESERVED1_OFFSET];

        اگر (trieType == DictionaryData::TRIE_TYPE_UCHARS) {
            ds->swapArray16(ds, inBytes + offset, nextOffset - offset, outBytes + offset, pErrorCode);
        } وگرنه اگر (trieType == DictionaryData::TRIE_TYPE_BYTES) {
            // nothing to do
        } وگرنه {
            udata_printError(ds, "udict_swap(): unknown trie type!\n");
            *pErrorCode = U_UNSUPPORTED_ERROR;
            بازگشت 0;
        }

        // these next two sections are empty in the current format,
        // but may be used later.
        offset = nextOffset;
        nextOffset = indexes[DictionaryData::IX_RESERVED2_OFFSET];
        offset = nextOffset;
        nextOffset = indexes[DictionaryData::IX_TOTAL_SIZE];
        offset = nextOffset;
    }
    بازگشت headerSize + size;
}
#endif
