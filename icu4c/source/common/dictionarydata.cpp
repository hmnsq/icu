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

#ଅନ୍ତର୍ଭାଵ "dictionarydata.h"
#ଅନ୍ତର୍ଭାଵ "unicode/ucharstrie.h"
#ଅନ୍ତର୍ଭାଵ "unicode/bytestrie.h"
#ଅନ୍ତର୍ଭାଵ "unicode/udata.h"
#ଅନ୍ତର୍ଭାଵ "cmemory.h"

#ଯଦି !UCONFIG_NO_BREAK_ITERATION

U_NAMESPACE_BEGIN

ଅଚଳ int32_t  DictionaryData::TRIE_TYPE_BYTES = 0;
ଅଚଳ int32_t  DictionaryData::TRIE_TYPE_UCHARS = 1;
ଅଚଳ int32_t  DictionaryData::TRIE_TYPE_MASK = 7;
ଅଚଳ int32_t  DictionaryData::TRIE_HAS_ମୂଲ୍ୟଗୁଡ଼ିକ = 8;

ଅଚଳ int32_t  DictionaryData::TRANSFORM_NONE = 0;
ଅଚଳ int32_t  DictionaryData::TRANSFORM_TYPE_OFFSET = 0x1000000;
ଅଚଳ int32_t  DictionaryData::TRANSFORM_TYPE_MASK = 0x7f000000;
ଅଚଳ int32_t  DictionaryData::TRANSFORM_OFFSET_MASK = 0x1fffff;
    
DictionaryMatcher::~DictionaryMatcher() {
}

UCharsDictionaryMatcher::~UCharsDictionaryMatcher() {
    udata_close(file);
}

int32_t UCharsDictionaryMatcher::getType() ଅଚଳ {
    ଫେରନ୍ତୁ DictionaryData::TRIE_TYPE_UCHARS;
}

int32_t UCharsDictionaryMatcher::matches(UText *text, int32_t maxLength, int32_t limit,
                            int32_t *lengths, int32_t *cpLengths, int32_t *ମୂଲ୍ୟଗୁଡ଼ିକ,
                            int32_t *prefix) ଅଚଳ {

    UCharsTrie uct(characters);
    int32_t startingTextIndex = static_cast<int32_t>(utext_getNativeIndex(text));
    int32_t word_ଗଣନା = 0;
    int32_t codePointsMatched = 0;

    ପାଇଁ (UChar32 c = utext_next32(text); c >= 0; c=utext_next32(text)) {
        UStringTrieResult result = (codePointsMatched == 0) ? uct.first(c) : uct.next(c);
        int32_t lengthMatched = static_cast<int32_t>(utext_getNativeIndex(text)) - startingTextIndex;
        codePointsMatched += 1;
        ଯଦି (USTRINGTRIE_HAS_ମୂଲ୍ୟ(result)) {
            ଯଦି (word_ଗଣନା < limit) {
                ଯଦି (ମୂଲ୍ୟଗୁଡ଼ିକ != nullptr) {
                    ମୂଲ୍ୟଗୁଡ଼ିକ[word_ଗଣନା] = uct.get_ମୂଲ୍ୟ();
                }
                ଯଦି (lengths != nullptr) {
                    lengths[word_ଗଣନା] = lengthMatched;
                }
                ଯଦି (cpLengths != nullptr) {
                    cpLengths[word_ଗଣନା] = codePointsMatched;
                }
                ++word_ଗଣନା;
            }
            ଯଦି (result == USTRINGTRIE_FINAL_ମୂଲ୍ୟ) {
                ଭାଙ୍ଗ;
            }
        }
        ଅନ୍ୟଥା ଯଦି (result == USTRINGTRIE_NO_MATCH) {
            ଭାଙ୍ଗ;
        }
        ଯଦି (lengthMatched >= maxLength) {
            ଭାଙ୍ଗ;
        }
    }

    ଯଦି (prefix != nullptr) {
        *prefix = codePointsMatched;
    }
    ଫେରନ୍ତୁ word_ଗଣନା;
}

BytesDictionaryMatcher::~BytesDictionaryMatcher() {
    udata_close(file);
}

UChar32 BytesDictionaryMatcher::transform(UChar32 c) ଅଚଳ {
    ଯଦି ((transformConstant & DictionaryData::TRANSFORM_TYPE_MASK) == DictionaryData::TRANSFORM_TYPE_OFFSET) {
        ଯଦି (c == 0x200D) {
            ଫେରନ୍ତୁ 0xFF;
        } ଅନ୍ୟଥା ଯଦି (c == 0x200C) {
            ଫେରନ୍ତୁ 0xFE;
        }
        int32_t delta = c - (transformConstant & DictionaryData::TRANSFORM_OFFSET_MASK);
        ଯଦି (delta < 0 || 0xFD < delta) {
            ଫେରନ୍ତୁ U_SENTINEL;
        }
        ଫେରନ୍ତୁ static_cast<UChar32>(delta);
    }
    ଫେରନ୍ତୁ c;
}

int32_t BytesDictionaryMatcher::getType() ଅଚଳ {
    ଫେରନ୍ତୁ DictionaryData::TRIE_TYPE_BYTES;
}

int32_t BytesDictionaryMatcher::matches(UText *text, int32_t maxLength, int32_t limit,
                            int32_t *lengths, int32_t *cpLengths, int32_t *ମୂଲ୍ୟଗୁଡ଼ିକ,
                            int32_t *prefix) ଅଚଳ {
    BytesTrie bt(characters);
    int32_t startingTextIndex = static_cast<int32_t>(utext_getNativeIndex(text));
    int32_t word_ଗଣନା = 0;
    int32_t codePointsMatched = 0;

    ପାଇଁ (UChar32 c = utext_next32(text); c >= 0; c=utext_next32(text)) {
        UStringTrieResult result = (codePointsMatched == 0) ? bt.first(transform(c)) : bt.next(transform(c));
        int32_t lengthMatched = static_cast<int32_t>(utext_getNativeIndex(text)) - startingTextIndex;
        codePointsMatched += 1;
        ଯଦି (USTRINGTRIE_HAS_ମୂଲ୍ୟ(result)) {
            ଯଦି (word_ଗଣନା < limit) {
                ଯଦି (ମୂଲ୍ୟଗୁଡ଼ିକ != nullptr) {
                    ମୂଲ୍ୟଗୁଡ଼ିକ[word_ଗଣନା] = bt.get_ମୂଲ୍ୟ();
                }
                ଯଦି (lengths != nullptr) {
                    lengths[word_ଗଣନା] = lengthMatched;
                }
                ଯଦି (cpLengths != nullptr) {
                    cpLengths[word_ଗଣନା] = codePointsMatched;
                }
                ++word_ଗଣନା;
            }
            ଯଦି (result == USTRINGTRIE_FINAL_ମୂଲ୍ୟ) {
                ଭାଙ୍ଗ;
            }
        }
        ଅନ୍ୟଥା ଯଦି (result == USTRINGTRIE_NO_MATCH) {
            ଭାଙ୍ଗ;
        }
        ଯଦି (lengthMatched >= maxLength) {
            ଭାଙ୍ଗ;
        }
    }

    ଯଦି (prefix != nullptr) {
        *prefix = codePointsMatched;
    }
    ଫେରନ୍ତୁ word_ଗଣନା;
}


U_NAMESPACE_END

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
udict_swap(ଅଚଳ UDataSwapper *ds, ଅଚଳ ଶୂନ୍ୟ *inData, int32_t length,
           ଶୂନ୍ୟ *outData, UErrorCode *pErrorCode) {
    ଅଚଳ UDataInfo *pInfo;
    int32_t headerSize;
    ଅଚଳ uint8_t *inBytes;
    uint8_t *outBytes;
    ଅଚଳ int32_t *inIndexes;
    int32_t indexes[DictionaryData::IX_ଗଣନା];
    int32_t i, offset, size;

    headerSize = udata_swapDataHeader(ds, inData, length, outData, pErrorCode);
    ଯଦି (pErrorCode == nullptr || U_FAILURE(*pErrorCode)) ଫେରନ୍ତୁ 0;
    pInfo = (ଅଚଳ UDataInfo *)((ଅଚଳ ଅକ୍ଷର *)inData + 4);
    ଯଦି (!(pInfo->dataFormat[0] == 0x44 && 
          pInfo->dataFormat[1] == 0x69 && 
          pInfo->dataFormat[2] == 0x63 && 
          pInfo->dataFormat[3] == 0x74 && 
          pInfo->formatVersion[0] == 1)) {
        udata_printError(ds, "udict_swap(): data format %02x.%02x.%02x.%02x (format version %02x) is not recognized as dictionary data\n",
                         pInfo->dataFormat[0], pInfo->dataFormat[1], pInfo->dataFormat[2], pInfo->dataFormat[3], pInfo->formatVersion[0]);
        *pErrorCode = U_UNSUPPORTED_ERROR;
        ଫେରନ୍ତୁ 0;
    }

    inBytes = (ଅଚଳ uint8_t *)inData + headerSize;
    outBytes = (outData == nullptr) ? nullptr : (uint8_t *)outData + headerSize;

    inIndexes = (ଅଚଳ int32_t *)inBytes;
    ଯଦି (length >= 0) {
        length -= headerSize;
        ଯଦି (length < (int32_t)(ଆକାର(indexes))) {
            udata_printError(ds, "udict_swap(): too few bytes (%d after header) for dictionary data\n", length);
            *pErrorCode = U_INDEX_OUTOFBOUNDS_ERROR;
            ଫେରନ୍ତୁ 0;
        }
    }

    ପାଇଁ (i = 0; i < DictionaryData::IX_ଗଣନା; i++) {
        indexes[i] = udata_readInt32(ds, inIndexes[i]);
    }

    size = indexes[DictionaryData::IX_TOTAL_SIZE];

    ଯଦି (length >= 0) {
        ଯଦି (length < size) {
            udata_printError(ds, "udict_swap(): too few bytes (%d after header) for all of dictionary data\n", length);
            *pErrorCode = U_INDEX_OUTOFBOUNDS_ERROR;
            ଫେରନ୍ତୁ 0;
        }

        ଯଦି (inBytes != outBytes) {
            uprv_memcpy(outBytes, inBytes, size);
        }

        offset = 0;
        ds->swapArray32(ds, inBytes, ଆକାର(indexes), outBytes, pErrorCode);
        offset = (int32_t)ଆକାର(indexes);
        int32_t trieType = indexes[DictionaryData::IX_TRIE_TYPE] & DictionaryData::TRIE_TYPE_MASK;
        int32_t nextOffset = indexes[DictionaryData::IX_RESERVED1_OFFSET];

        ଯଦି (trieType == DictionaryData::TRIE_TYPE_UCHARS) {
            ds->swapArray16(ds, inBytes + offset, nextOffset - offset, outBytes + offset, pErrorCode);
        } ଅନ୍ୟଥା ଯଦି (trieType == DictionaryData::TRIE_TYPE_BYTES) {
            // nothing to do
        } ଅନ୍ୟଥା {
            udata_printError(ds, "udict_swap(): unknown trie type!\n");
            *pErrorCode = U_UNSUPPORTED_ERROR;
            ଫେରନ୍ତୁ 0;
        }

        // these next two sections are empty in the current format,
        // but may be used later.
        offset = nextOffset;
        nextOffset = indexes[DictionaryData::IX_RESERVED2_OFFSET];
        offset = nextOffset;
        nextOffset = indexes[DictionaryData::IX_TOTAL_SIZE];
        offset = nextOffset;
    }
    ଫେରନ୍ତୁ headerSize + size;
}
#endif
