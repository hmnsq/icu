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

#அந்தர்பாவ "dictionarydata.h"
#அந்தர்பாவ "unicode/ucharstrie.h"
#அந்தர்பாவ "unicode/bytestrie.h"
#அந்தர்பாவ "unicode/udata.h"
#அந்தர்பாவ "cmemory.h"

#எனில் !UCONFIG_NO_BREAK_ITERATION

U_NAMESPACE_BEGIN

மாறிலி int32_t  DictionaryData::TRIE_TYPE_BYTES = 0;
மாறிலி int32_t  DictionaryData::TRIE_TYPE_UCHARS = 1;
மாறிலி int32_t  DictionaryData::TRIE_TYPE_MASK = 7;
மாறிலி int32_t  DictionaryData::TRIE_HAS_மதிப்புகள் = 8;

மாறிலி int32_t  DictionaryData::TRANSFORM_NONE = 0;
மாறிலி int32_t  DictionaryData::TRANSFORM_TYPE_OFFSET = 0x1000000;
மாறிலி int32_t  DictionaryData::TRANSFORM_TYPE_MASK = 0x7f000000;
மாறிலி int32_t  DictionaryData::TRANSFORM_OFFSET_MASK = 0x1fffff;
    
DictionaryMatcher::~DictionaryMatcher() {
}

UCharsDictionaryMatcher::~UCharsDictionaryMatcher() {
    udata_close(file);
}

int32_t UCharsDictionaryMatcher::getType() மாறிலி {
    திரும்பு DictionaryData::TRIE_TYPE_UCHARS;
}

int32_t UCharsDictionaryMatcher::matches(UText *text, int32_t maxLength, int32_t limit,
                            int32_t *lengths, int32_t *cpLengths, int32_t *மதிப்புகள்,
                            int32_t *prefix) மாறிலி {

    UCharsTrie uct(characters);
    int32_t startingTextIndex = static_cast<int32_t>(utext_getNativeIndex(text));
    int32_t word_மொத்தம் = 0;
    int32_t codePointsMatched = 0;

    க்கு (UChar32 c = utext_next32(text); c >= 0; c=utext_next32(text)) {
        UStringTrieResult result = (codePointsMatched == 0) ? uct.first(c) : uct.next(c);
        int32_t lengthMatched = static_cast<int32_t>(utext_getNativeIndex(text)) - startingTextIndex;
        codePointsMatched += 1;
        எனில் (USTRINGTRIE_HAS_மதிப்பு(result)) {
            எனில் (word_மொத்தம் < limit) {
                எனில் (மதிப்புகள் != nullptr) {
                    மதிப்புகள்[word_மொத்தம்] = uct.get_மதிப்பு();
                }
                எனில் (lengths != nullptr) {
                    lengths[word_மொத்தம்] = lengthMatched;
                }
                எனில் (cpLengths != nullptr) {
                    cpLengths[word_மொத்தம்] = codePointsMatched;
                }
                ++word_மொத்தம்;
            }
            எனில் (result == USTRINGTRIE_FINAL_மதிப்பு) {
                நிறுத்து;
            }
        }
        இல்லையெனில் எனில் (result == USTRINGTRIE_NO_MATCH) {
            நிறுத்து;
        }
        எனில் (lengthMatched >= maxLength) {
            நிறுத்து;
        }
    }

    எனில் (prefix != nullptr) {
        *prefix = codePointsMatched;
    }
    திரும்பு word_மொத்தம்;
}

BytesDictionaryMatcher::~BytesDictionaryMatcher() {
    udata_close(file);
}

UChar32 BytesDictionaryMatcher::transform(UChar32 c) மாறிலி {
    எனில் ((transformConstant & DictionaryData::TRANSFORM_TYPE_MASK) == DictionaryData::TRANSFORM_TYPE_OFFSET) {
        எனில் (c == 0x200D) {
            திரும்பு 0xFF;
        } இல்லையெனில் எனில் (c == 0x200C) {
            திரும்பு 0xFE;
        }
        int32_t delta = c - (transformConstant & DictionaryData::TRANSFORM_OFFSET_MASK);
        எனில் (delta < 0 || 0xFD < delta) {
            திரும்பு U_SENTINEL;
        }
        திரும்பு static_cast<UChar32>(delta);
    }
    திரும்பு c;
}

int32_t BytesDictionaryMatcher::getType() மாறிலி {
    திரும்பு DictionaryData::TRIE_TYPE_BYTES;
}

int32_t BytesDictionaryMatcher::matches(UText *text, int32_t maxLength, int32_t limit,
                            int32_t *lengths, int32_t *cpLengths, int32_t *மதிப்புகள்,
                            int32_t *prefix) மாறிலி {
    BytesTrie bt(characters);
    int32_t startingTextIndex = static_cast<int32_t>(utext_getNativeIndex(text));
    int32_t word_மொத்தம் = 0;
    int32_t codePointsMatched = 0;

    க்கு (UChar32 c = utext_next32(text); c >= 0; c=utext_next32(text)) {
        UStringTrieResult result = (codePointsMatched == 0) ? bt.first(transform(c)) : bt.next(transform(c));
        int32_t lengthMatched = static_cast<int32_t>(utext_getNativeIndex(text)) - startingTextIndex;
        codePointsMatched += 1;
        எனில் (USTRINGTRIE_HAS_மதிப்பு(result)) {
            எனில் (word_மொத்தம் < limit) {
                எனில் (மதிப்புகள் != nullptr) {
                    மதிப்புகள்[word_மொத்தம்] = bt.get_மதிப்பு();
                }
                எனில் (lengths != nullptr) {
                    lengths[word_மொத்தம்] = lengthMatched;
                }
                எனில் (cpLengths != nullptr) {
                    cpLengths[word_மொத்தம்] = codePointsMatched;
                }
                ++word_மொத்தம்;
            }
            எனில் (result == USTRINGTRIE_FINAL_மதிப்பு) {
                நிறுத்து;
            }
        }
        இல்லையெனில் எனில் (result == USTRINGTRIE_NO_MATCH) {
            நிறுத்து;
        }
        எனில் (lengthMatched >= maxLength) {
            நிறுத்து;
        }
    }

    எனில் (prefix != nullptr) {
        *prefix = codePointsMatched;
    }
    திரும்பு word_மொத்தம்;
}


U_NAMESPACE_END

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
udict_swap(மாறிலி UDataSwapper *ds, மாறிலி வெறுமை *inData, int32_t length,
           வெறுமை *outData, UErrorCode *pErrorCode) {
    மாறிலி UDataInfo *pInfo;
    int32_t headerSize;
    மாறிலி uint8_t *inBytes;
    uint8_t *outBytes;
    மாறிலி int32_t *inIndexes;
    int32_t indexes[DictionaryData::IX_மொத்தம்];
    int32_t i, offset, size;

    headerSize = udata_swapDataHeader(ds, inData, length, outData, pErrorCode);
    எனில் (pErrorCode == nullptr || U_FAILURE(*pErrorCode)) திரும்பு 0;
    pInfo = (மாறிலி UDataInfo *)((மாறிலி எழுத்து *)inData + 4);
    எனில் (!(pInfo->dataFormat[0] == 0x44 && 
          pInfo->dataFormat[1] == 0x69 && 
          pInfo->dataFormat[2] == 0x63 && 
          pInfo->dataFormat[3] == 0x74 && 
          pInfo->formatVersion[0] == 1)) {
        udata_printError(ds, "udict_swap(): data format %02x.%02x.%02x.%02x (format version %02x) is not recognized as dictionary data\n",
                         pInfo->dataFormat[0], pInfo->dataFormat[1], pInfo->dataFormat[2], pInfo->dataFormat[3], pInfo->formatVersion[0]);
        *pErrorCode = U_UNSUPPORTED_ERROR;
        திரும்பு 0;
    }

    inBytes = (மாறிலி uint8_t *)inData + headerSize;
    outBytes = (outData == nullptr) ? nullptr : (uint8_t *)outData + headerSize;

    inIndexes = (மாறிலி int32_t *)inBytes;
    எனில் (length >= 0) {
        length -= headerSize;
        எனில் (length < (int32_t)(அளவு(indexes))) {
            udata_printError(ds, "udict_swap(): too few bytes (%d after header) for dictionary data\n", length);
            *pErrorCode = U_INDEX_OUTOFBOUNDS_ERROR;
            திரும்பு 0;
        }
    }

    க்கு (i = 0; i < DictionaryData::IX_மொத்தம்; i++) {
        indexes[i] = udata_readInt32(ds, inIndexes[i]);
    }

    size = indexes[DictionaryData::IX_TOTAL_SIZE];

    எனில் (length >= 0) {
        எனில் (length < size) {
            udata_printError(ds, "udict_swap(): too few bytes (%d after header) for all of dictionary data\n", length);
            *pErrorCode = U_INDEX_OUTOFBOUNDS_ERROR;
            திரும்பு 0;
        }

        எனில் (inBytes != outBytes) {
            uprv_memcpy(outBytes, inBytes, size);
        }

        offset = 0;
        ds->swapArray32(ds, inBytes, அளவு(indexes), outBytes, pErrorCode);
        offset = (int32_t)அளவு(indexes);
        int32_t trieType = indexes[DictionaryData::IX_TRIE_TYPE] & DictionaryData::TRIE_TYPE_MASK;
        int32_t nextOffset = indexes[DictionaryData::IX_RESERVED1_OFFSET];

        எனில் (trieType == DictionaryData::TRIE_TYPE_UCHARS) {
            ds->swapArray16(ds, inBytes + offset, nextOffset - offset, outBytes + offset, pErrorCode);
        } இல்லையெனில் எனில் (trieType == DictionaryData::TRIE_TYPE_BYTES) {
            // nothing to do
        } இல்லையெனில் {
            udata_printError(ds, "udict_swap(): unknown trie type!\n");
            *pErrorCode = U_UNSUPPORTED_ERROR;
            திரும்பு 0;
        }

        // these next two sections are empty in the current format,
        // but may be used later.
        offset = nextOffset;
        nextOffset = indexes[DictionaryData::IX_RESERVED2_OFFSET];
        offset = nextOffset;
        nextOffset = indexes[DictionaryData::IX_TOTAL_SIZE];
        offset = nextOffset;
    }
    திரும்பு headerSize + size;
}
#endif
