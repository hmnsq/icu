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

#അന്തര്ഭാവ "dictionarydata.h"
#അന്തര്ഭാവ "unicode/ucharstrie.h"
#അന്തര്ഭാവ "unicode/bytestrie.h"
#അന്തര്ഭാവ "unicode/udata.h"
#അന്തര്ഭാവ "cmemory.h"

#എങ്കിൽ !UCONFIG_NO_BREAK_ITERATION

U_NAMESPACE_BEGIN

സ്ഥിരം int32_t  DictionaryData::TRIE_TYPE_BYTES = 0;
സ്ഥിരം int32_t  DictionaryData::TRIE_TYPE_UCHARS = 1;
സ്ഥിരം int32_t  DictionaryData::TRIE_TYPE_MASK = 7;
സ്ഥിരം int32_t  DictionaryData::TRIE_HAS_മൂല്യങ്ങൾ = 8;

സ്ഥിരം int32_t  DictionaryData::TRANSFORM_NONE = 0;
സ്ഥിരം int32_t  DictionaryData::TRANSFORM_TYPE_OFFSET = 0x1000000;
സ്ഥിരം int32_t  DictionaryData::TRANSFORM_TYPE_MASK = 0x7f000000;
സ്ഥിരം int32_t  DictionaryData::TRANSFORM_OFFSET_MASK = 0x1fffff;
    
DictionaryMatcher::~DictionaryMatcher() {
}

UCharsDictionaryMatcher::~UCharsDictionaryMatcher() {
    udata_close(file);
}

int32_t UCharsDictionaryMatcher::getType() സ്ഥിരം {
    തിരികെ DictionaryData::TRIE_TYPE_UCHARS;
}

int32_t UCharsDictionaryMatcher::matches(UText *text, int32_t maxLength, int32_t limit,
                            int32_t *lengths, int32_t *cpLengths, int32_t *മൂല്യങ്ങൾ,
                            int32_t *prefix) സ്ഥിരം {

    UCharsTrie uct(characters);
    int32_t startingTextIndex = static_cast<int32_t>(utext_getNativeIndex(text));
    int32_t word_എണ്ണം = 0;
    int32_t codePointsMatched = 0;

    വേണ്ടി (UChar32 c = utext_next32(text); c >= 0; c=utext_next32(text)) {
        UStringTrieResult result = (codePointsMatched == 0) ? uct.first(c) : uct.next(c);
        int32_t lengthMatched = static_cast<int32_t>(utext_getNativeIndex(text)) - startingTextIndex;
        codePointsMatched += 1;
        എങ്കിൽ (USTRINGTRIE_HAS_മൂല്യം(result)) {
            എങ്കിൽ (word_എണ്ണം < limit) {
                എങ്കിൽ (മൂല്യങ്ങൾ != nullptr) {
                    മൂല്യങ്ങൾ[word_എണ്ണം] = uct.get_മൂല്യം();
                }
                എങ്കിൽ (lengths != nullptr) {
                    lengths[word_എണ്ണം] = lengthMatched;
                }
                എങ്കിൽ (cpLengths != nullptr) {
                    cpLengths[word_എണ്ണം] = codePointsMatched;
                }
                ++word_എണ്ണം;
            }
            എങ്കിൽ (result == USTRINGTRIE_FINAL_മൂല്യം) {
                നിർത്തുക;
            }
        }
        അല്ലെങ്കിൽ എങ്കിൽ (result == USTRINGTRIE_NO_MATCH) {
            നിർത്തുക;
        }
        എങ്കിൽ (lengthMatched >= maxLength) {
            നിർത്തുക;
        }
    }

    എങ്കിൽ (prefix != nullptr) {
        *prefix = codePointsMatched;
    }
    തിരികെ word_എണ്ണം;
}

BytesDictionaryMatcher::~BytesDictionaryMatcher() {
    udata_close(file);
}

UChar32 BytesDictionaryMatcher::transform(UChar32 c) സ്ഥിരം {
    എങ്കിൽ ((transformConstant & DictionaryData::TRANSFORM_TYPE_MASK) == DictionaryData::TRANSFORM_TYPE_OFFSET) {
        എങ്കിൽ (c == 0x200D) {
            തിരികെ 0xFF;
        } അല്ലെങ്കിൽ എങ്കിൽ (c == 0x200C) {
            തിരികെ 0xFE;
        }
        int32_t delta = c - (transformConstant & DictionaryData::TRANSFORM_OFFSET_MASK);
        എങ്കിൽ (delta < 0 || 0xFD < delta) {
            തിരികെ U_SENTINEL;
        }
        തിരികെ static_cast<UChar32>(delta);
    }
    തിരികെ c;
}

int32_t BytesDictionaryMatcher::getType() സ്ഥിരം {
    തിരികെ DictionaryData::TRIE_TYPE_BYTES;
}

int32_t BytesDictionaryMatcher::matches(UText *text, int32_t maxLength, int32_t limit,
                            int32_t *lengths, int32_t *cpLengths, int32_t *മൂല്യങ്ങൾ,
                            int32_t *prefix) സ്ഥിരം {
    BytesTrie bt(characters);
    int32_t startingTextIndex = static_cast<int32_t>(utext_getNativeIndex(text));
    int32_t word_എണ്ണം = 0;
    int32_t codePointsMatched = 0;

    വേണ്ടി (UChar32 c = utext_next32(text); c >= 0; c=utext_next32(text)) {
        UStringTrieResult result = (codePointsMatched == 0) ? bt.first(transform(c)) : bt.next(transform(c));
        int32_t lengthMatched = static_cast<int32_t>(utext_getNativeIndex(text)) - startingTextIndex;
        codePointsMatched += 1;
        എങ്കിൽ (USTRINGTRIE_HAS_മൂല്യം(result)) {
            എങ്കിൽ (word_എണ്ണം < limit) {
                എങ്കിൽ (മൂല്യങ്ങൾ != nullptr) {
                    മൂല്യങ്ങൾ[word_എണ്ണം] = bt.get_മൂല്യം();
                }
                എങ്കിൽ (lengths != nullptr) {
                    lengths[word_എണ്ണം] = lengthMatched;
                }
                എങ്കിൽ (cpLengths != nullptr) {
                    cpLengths[word_എണ്ണം] = codePointsMatched;
                }
                ++word_എണ്ണം;
            }
            എങ്കിൽ (result == USTRINGTRIE_FINAL_മൂല്യം) {
                നിർത്തുക;
            }
        }
        അല്ലെങ്കിൽ എങ്കിൽ (result == USTRINGTRIE_NO_MATCH) {
            നിർത്തുക;
        }
        എങ്കിൽ (lengthMatched >= maxLength) {
            നിർത്തുക;
        }
    }

    എങ്കിൽ (prefix != nullptr) {
        *prefix = codePointsMatched;
    }
    തിരികെ word_എണ്ണം;
}


U_NAMESPACE_END

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
udict_swap(സ്ഥിരം UDataSwapper *ds, സ്ഥിരം ശൂന്യം *inData, int32_t length,
           ശൂന്യം *outData, UErrorCode *pErrorCode) {
    സ്ഥിരം UDataInfo *pInfo;
    int32_t headerSize;
    സ്ഥിരം uint8_t *inBytes;
    uint8_t *outBytes;
    സ്ഥിരം int32_t *inIndexes;
    int32_t indexes[DictionaryData::IX_എണ്ണം];
    int32_t i, offset, size;

    headerSize = udata_swapDataHeader(ds, inData, length, outData, pErrorCode);
    എങ്കിൽ (pErrorCode == nullptr || U_FAILURE(*pErrorCode)) തിരികെ 0;
    pInfo = (സ്ഥിരം UDataInfo *)((സ്ഥിരം അക്ഷരം *)inData + 4);
    എങ്കിൽ (!(pInfo->dataFormat[0] == 0x44 && 
          pInfo->dataFormat[1] == 0x69 && 
          pInfo->dataFormat[2] == 0x63 && 
          pInfo->dataFormat[3] == 0x74 && 
          pInfo->formatVersion[0] == 1)) {
        udata_printError(ds, "udict_swap(): data format %02x.%02x.%02x.%02x (format version %02x) is not recognized as dictionary data\n",
                         pInfo->dataFormat[0], pInfo->dataFormat[1], pInfo->dataFormat[2], pInfo->dataFormat[3], pInfo->formatVersion[0]);
        *pErrorCode = U_UNSUPPORTED_ERROR;
        തിരികെ 0;
    }

    inBytes = (സ്ഥിരം uint8_t *)inData + headerSize;
    outBytes = (outData == nullptr) ? nullptr : (uint8_t *)outData + headerSize;

    inIndexes = (സ്ഥിരം int32_t *)inBytes;
    എങ്കിൽ (length >= 0) {
        length -= headerSize;
        എങ്കിൽ (length < (int32_t)(വലുപ്പം(indexes))) {
            udata_printError(ds, "udict_swap(): too few bytes (%d after header) for dictionary data\n", length);
            *pErrorCode = U_INDEX_OUTOFBOUNDS_ERROR;
            തിരികെ 0;
        }
    }

    വേണ്ടി (i = 0; i < DictionaryData::IX_എണ്ണം; i++) {
        indexes[i] = udata_readInt32(ds, inIndexes[i]);
    }

    size = indexes[DictionaryData::IX_TOTAL_SIZE];

    എങ്കിൽ (length >= 0) {
        എങ്കിൽ (length < size) {
            udata_printError(ds, "udict_swap(): too few bytes (%d after header) for all of dictionary data\n", length);
            *pErrorCode = U_INDEX_OUTOFBOUNDS_ERROR;
            തിരികെ 0;
        }

        എങ്കിൽ (inBytes != outBytes) {
            uprv_memcpy(outBytes, inBytes, size);
        }

        offset = 0;
        ds->swapArray32(ds, inBytes, വലുപ്പം(indexes), outBytes, pErrorCode);
        offset = (int32_t)വലുപ്പം(indexes);
        int32_t trieType = indexes[DictionaryData::IX_TRIE_TYPE] & DictionaryData::TRIE_TYPE_MASK;
        int32_t nextOffset = indexes[DictionaryData::IX_RESERVED1_OFFSET];

        എങ്കിൽ (trieType == DictionaryData::TRIE_TYPE_UCHARS) {
            ds->swapArray16(ds, inBytes + offset, nextOffset - offset, outBytes + offset, pErrorCode);
        } അല്ലെങ്കിൽ എങ്കിൽ (trieType == DictionaryData::TRIE_TYPE_BYTES) {
            // nothing to do
        } അല്ലെങ്കിൽ {
            udata_printError(ds, "udict_swap(): unknown trie type!\n");
            *pErrorCode = U_UNSUPPORTED_ERROR;
            തിരികെ 0;
        }

        // these next two sections are empty in the current format,
        // but may be used later.
        offset = nextOffset;
        nextOffset = indexes[DictionaryData::IX_RESERVED2_OFFSET];
        offset = nextOffset;
        nextOffset = indexes[DictionaryData::IX_TOTAL_SIZE];
        offset = nextOffset;
    }
    തിരികെ headerSize + size;
}
#endif
