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

#અન્તર્ભાવ "dictionarydata.h"
#અન્તર્ભાવ "unicode/ucharstrie.h"
#અન્તર્ભાવ "unicode/bytestrie.h"
#અન્તર્ભાવ "unicode/udata.h"
#અન્તર્ભાવ "cmemory.h"

#જો !UCONFIG_NO_BREAK_ITERATION

U_NAMESPACE_BEGIN

અચલ int32_t  DictionaryData::TRIE_TYPE_BYTES = 0;
અચલ int32_t  DictionaryData::TRIE_TYPE_UCHARS = 1;
અચલ int32_t  DictionaryData::TRIE_TYPE_MASK = 7;
અચલ int32_t  DictionaryData::TRIE_HAS_કિંમતો = 8;

અચલ int32_t  DictionaryData::TRANSFORM_NONE = 0;
અચલ int32_t  DictionaryData::TRANSFORM_TYPE_OFFSET = 0x1000000;
અચલ int32_t  DictionaryData::TRANSFORM_TYPE_MASK = 0x7f000000;
અચલ int32_t  DictionaryData::TRANSFORM_OFFSET_MASK = 0x1fffff;
    
DictionaryMatcher::~DictionaryMatcher() {
}

UCharsDictionaryMatcher::~UCharsDictionaryMatcher() {
    udata_close(file);
}

int32_t UCharsDictionaryMatcher::getType() અચલ {
    પાછા DictionaryData::TRIE_TYPE_UCHARS;
}

int32_t UCharsDictionaryMatcher::matches(UText *text, int32_t maxLength, int32_t limit,
                            int32_t *lengths, int32_t *cpLengths, int32_t *કિંમતો,
                            int32_t *prefix) અચલ {

    UCharsTrie uct(characters);
    int32_t startingTextIndex = static_cast<int32_t>(utext_getNativeIndex(text));
    int32_t word_ગણતરી = 0;
    int32_t codePointsMatched = 0;

    માટે (UChar32 c = utext_next32(text); c >= 0; c=utext_next32(text)) {
        UStringTrieResult result = (codePointsMatched == 0) ? uct.first(c) : uct.next(c);
        int32_t lengthMatched = static_cast<int32_t>(utext_getNativeIndex(text)) - startingTextIndex;
        codePointsMatched += 1;
        જો (USTRINGTRIE_HAS_કિંમત(result)) {
            જો (word_ગણતરી < limit) {
                જો (કિંમતો != nullptr) {
                    કિંમતો[word_ગણતરી] = uct.get_કિંમત();
                }
                જો (lengths != nullptr) {
                    lengths[word_ગણતરી] = lengthMatched;
                }
                જો (cpLengths != nullptr) {
                    cpLengths[word_ગણતરી] = codePointsMatched;
                }
                ++word_ગણતરી;
            }
            જો (result == USTRINGTRIE_FINAL_કિંમત) {
                તોડો;
            }
        }
        નહીંતર જો (result == USTRINGTRIE_NO_MATCH) {
            તોડો;
        }
        જો (lengthMatched >= maxLength) {
            તોડો;
        }
    }

    જો (prefix != nullptr) {
        *prefix = codePointsMatched;
    }
    પાછા word_ગણતરી;
}

BytesDictionaryMatcher::~BytesDictionaryMatcher() {
    udata_close(file);
}

UChar32 BytesDictionaryMatcher::transform(UChar32 c) અચલ {
    જો ((transformConstant & DictionaryData::TRANSFORM_TYPE_MASK) == DictionaryData::TRANSFORM_TYPE_OFFSET) {
        જો (c == 0x200D) {
            પાછા 0xFF;
        } નહીંતર જો (c == 0x200C) {
            પાછા 0xFE;
        }
        int32_t delta = c - (transformConstant & DictionaryData::TRANSFORM_OFFSET_MASK);
        જો (delta < 0 || 0xFD < delta) {
            પાછા U_SENTINEL;
        }
        પાછા static_cast<UChar32>(delta);
    }
    પાછા c;
}

int32_t BytesDictionaryMatcher::getType() અચલ {
    પાછા DictionaryData::TRIE_TYPE_BYTES;
}

int32_t BytesDictionaryMatcher::matches(UText *text, int32_t maxLength, int32_t limit,
                            int32_t *lengths, int32_t *cpLengths, int32_t *કિંમતો,
                            int32_t *prefix) અચલ {
    BytesTrie bt(characters);
    int32_t startingTextIndex = static_cast<int32_t>(utext_getNativeIndex(text));
    int32_t word_ગણતરી = 0;
    int32_t codePointsMatched = 0;

    માટે (UChar32 c = utext_next32(text); c >= 0; c=utext_next32(text)) {
        UStringTrieResult result = (codePointsMatched == 0) ? bt.first(transform(c)) : bt.next(transform(c));
        int32_t lengthMatched = static_cast<int32_t>(utext_getNativeIndex(text)) - startingTextIndex;
        codePointsMatched += 1;
        જો (USTRINGTRIE_HAS_કિંમત(result)) {
            જો (word_ગણતરી < limit) {
                જો (કિંમતો != nullptr) {
                    કિંમતો[word_ગણતરી] = bt.get_કિંમત();
                }
                જો (lengths != nullptr) {
                    lengths[word_ગણતરી] = lengthMatched;
                }
                જો (cpLengths != nullptr) {
                    cpLengths[word_ગણતરી] = codePointsMatched;
                }
                ++word_ગણતરી;
            }
            જો (result == USTRINGTRIE_FINAL_કિંમત) {
                તોડો;
            }
        }
        નહીંતર જો (result == USTRINGTRIE_NO_MATCH) {
            તોડો;
        }
        જો (lengthMatched >= maxLength) {
            તોડો;
        }
    }

    જો (prefix != nullptr) {
        *prefix = codePointsMatched;
    }
    પાછા word_ગણતરી;
}


U_NAMESPACE_END

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
udict_swap(અચલ UDataSwapper *ds, અચલ શૂન્ય *inData, int32_t length,
           શૂન્ય *outData, UErrorCode *pErrorCode) {
    અચલ UDataInfo *pInfo;
    int32_t headerSize;
    અચલ uint8_t *inBytes;
    uint8_t *outBytes;
    અચલ int32_t *inIndexes;
    int32_t indexes[DictionaryData::IX_ગણતરી];
    int32_t i, offset, size;

    headerSize = udata_swapDataHeader(ds, inData, length, outData, pErrorCode);
    જો (pErrorCode == nullptr || U_FAILURE(*pErrorCode)) પાછા 0;
    pInfo = (અચલ UDataInfo *)((અચલ અક્ષર *)inData + 4);
    જો (!(pInfo->dataFormat[0] == 0x44 && 
          pInfo->dataFormat[1] == 0x69 && 
          pInfo->dataFormat[2] == 0x63 && 
          pInfo->dataFormat[3] == 0x74 && 
          pInfo->formatVersion[0] == 1)) {
        udata_printError(ds, "udict_swap(): data format %02x.%02x.%02x.%02x (format version %02x) is not recognized as dictionary data\n",
                         pInfo->dataFormat[0], pInfo->dataFormat[1], pInfo->dataFormat[2], pInfo->dataFormat[3], pInfo->formatVersion[0]);
        *pErrorCode = U_UNSUPPORTED_ERROR;
        પાછા 0;
    }

    inBytes = (અચલ uint8_t *)inData + headerSize;
    outBytes = (outData == nullptr) ? nullptr : (uint8_t *)outData + headerSize;

    inIndexes = (અચલ int32_t *)inBytes;
    જો (length >= 0) {
        length -= headerSize;
        જો (length < (int32_t)(કદ(indexes))) {
            udata_printError(ds, "udict_swap(): too few bytes (%d after header) for dictionary data\n", length);
            *pErrorCode = U_INDEX_OUTOFBOUNDS_ERROR;
            પાછા 0;
        }
    }

    માટે (i = 0; i < DictionaryData::IX_ગણતરી; i++) {
        indexes[i] = udata_readInt32(ds, inIndexes[i]);
    }

    size = indexes[DictionaryData::IX_TOTAL_SIZE];

    જો (length >= 0) {
        જો (length < size) {
            udata_printError(ds, "udict_swap(): too few bytes (%d after header) for all of dictionary data\n", length);
            *pErrorCode = U_INDEX_OUTOFBOUNDS_ERROR;
            પાછા 0;
        }

        જો (inBytes != outBytes) {
            uprv_memcpy(outBytes, inBytes, size);
        }

        offset = 0;
        ds->swapArray32(ds, inBytes, કદ(indexes), outBytes, pErrorCode);
        offset = (int32_t)કદ(indexes);
        int32_t trieType = indexes[DictionaryData::IX_TRIE_TYPE] & DictionaryData::TRIE_TYPE_MASK;
        int32_t nextOffset = indexes[DictionaryData::IX_RESERVED1_OFFSET];

        જો (trieType == DictionaryData::TRIE_TYPE_UCHARS) {
            ds->swapArray16(ds, inBytes + offset, nextOffset - offset, outBytes + offset, pErrorCode);
        } નહીંતર જો (trieType == DictionaryData::TRIE_TYPE_BYTES) {
            // nothing to do
        } નહીંતર {
            udata_printError(ds, "udict_swap(): unknown trie type!\n");
            *pErrorCode = U_UNSUPPORTED_ERROR;
            પાછા 0;
        }

        // these next two sections are empty in the current format,
        // but may be used later.
        offset = nextOffset;
        nextOffset = indexes[DictionaryData::IX_RESERVED2_OFFSET];
        offset = nextOffset;
        nextOffset = indexes[DictionaryData::IX_TOTAL_SIZE];
        offset = nextOffset;
    }
    પાછા headerSize + size;
}
#endif
