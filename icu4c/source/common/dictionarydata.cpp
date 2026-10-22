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

#जर !UCONFIG_NO_BREAK_ITERATION

U_NAMESPACE_BEGIN

स्थिरांक int32_t  DictionaryData::TRIE_TYPE_BYTES = 0;
स्थिरांक int32_t  DictionaryData::TRIE_TYPE_UCHARS = 1;
स्थिरांक int32_t  DictionaryData::TRIE_TYPE_MASK = 7;
स्थिरांक int32_t  DictionaryData::TRIE_HAS_मूल्ये = 8;

स्थिरांक int32_t  DictionaryData::TRANSFORM_NONE = 0;
स्थिरांक int32_t  DictionaryData::TRANSFORM_TYPE_OFFSET = 0x1000000;
स्थिरांक int32_t  DictionaryData::TRANSFORM_TYPE_MASK = 0x7f000000;
स्थिरांक int32_t  DictionaryData::TRANSFORM_OFFSET_MASK = 0x1fffff;
    
DictionaryMatcher::~DictionaryMatcher() {
}

UCharsDictionaryMatcher::~UCharsDictionaryMatcher() {
    udata_close(file);
}

int32_t UCharsDictionaryMatcher::getType() स्थिरांक {
    परत DictionaryData::TRIE_TYPE_UCHARS;
}

int32_t UCharsDictionaryMatcher::matches(UText *text, int32_t maxLength, int32_t limit,
                            int32_t *lengths, int32_t *cpLengths, int32_t *मूल्ये,
                            int32_t *prefix) स्थिरांक {

    UCharsTrie uct(characters);
    int32_t startingTextIndex = static_cast<int32_t>(utext_getNativeIndex(text));
    int32_t word_मोजणी = 0;
    int32_t codePointsMatched = 0;

    साठी (UChar32 c = utext_next32(text); c >= 0; c=utext_next32(text)) {
        UStringTrieResult result = (codePointsMatched == 0) ? uct.first(c) : uct.next(c);
        int32_t lengthMatched = static_cast<int32_t>(utext_getNativeIndex(text)) - startingTextIndex;
        codePointsMatched += 1;
        जर (USTRINGTRIE_HAS_मूल्य(result)) {
            जर (word_मोजणी < limit) {
                जर (मूल्ये != nullptr) {
                    मूल्ये[word_मोजणी] = uct.get_मूल्य();
                }
                जर (lengths != nullptr) {
                    lengths[word_मोजणी] = lengthMatched;
                }
                जर (cpLengths != nullptr) {
                    cpLengths[word_मोजणी] = codePointsMatched;
                }
                ++word_मोजणी;
            }
            जर (result == USTRINGTRIE_FINAL_मूल्य) {
                थांबा;
            }
        }
        अन्यथा जर (result == USTRINGTRIE_NO_MATCH) {
            थांबा;
        }
        जर (lengthMatched >= maxLength) {
            थांबा;
        }
    }

    जर (prefix != nullptr) {
        *prefix = codePointsMatched;
    }
    परत word_मोजणी;
}

BytesDictionaryMatcher::~BytesDictionaryMatcher() {
    udata_close(file);
}

UChar32 BytesDictionaryMatcher::transform(UChar32 c) स्थिरांक {
    जर ((transformConstant & DictionaryData::TRANSFORM_TYPE_MASK) == DictionaryData::TRANSFORM_TYPE_OFFSET) {
        जर (c == 0x200D) {
            परत 0xFF;
        } अन्यथा जर (c == 0x200C) {
            परत 0xFE;
        }
        int32_t delta = c - (transformConstant & DictionaryData::TRANSFORM_OFFSET_MASK);
        जर (delta < 0 || 0xFD < delta) {
            परत U_SENTINEL;
        }
        परत static_cast<UChar32>(delta);
    }
    परत c;
}

int32_t BytesDictionaryMatcher::getType() स्थिरांक {
    परत DictionaryData::TRIE_TYPE_BYTES;
}

int32_t BytesDictionaryMatcher::matches(UText *text, int32_t maxLength, int32_t limit,
                            int32_t *lengths, int32_t *cpLengths, int32_t *मूल्ये,
                            int32_t *prefix) स्थिरांक {
    BytesTrie bt(characters);
    int32_t startingTextIndex = static_cast<int32_t>(utext_getNativeIndex(text));
    int32_t word_मोजणी = 0;
    int32_t codePointsMatched = 0;

    साठी (UChar32 c = utext_next32(text); c >= 0; c=utext_next32(text)) {
        UStringTrieResult result = (codePointsMatched == 0) ? bt.first(transform(c)) : bt.next(transform(c));
        int32_t lengthMatched = static_cast<int32_t>(utext_getNativeIndex(text)) - startingTextIndex;
        codePointsMatched += 1;
        जर (USTRINGTRIE_HAS_मूल्य(result)) {
            जर (word_मोजणी < limit) {
                जर (मूल्ये != nullptr) {
                    मूल्ये[word_मोजणी] = bt.get_मूल्य();
                }
                जर (lengths != nullptr) {
                    lengths[word_मोजणी] = lengthMatched;
                }
                जर (cpLengths != nullptr) {
                    cpLengths[word_मोजणी] = codePointsMatched;
                }
                ++word_मोजणी;
            }
            जर (result == USTRINGTRIE_FINAL_मूल्य) {
                थांबा;
            }
        }
        अन्यथा जर (result == USTRINGTRIE_NO_MATCH) {
            थांबा;
        }
        जर (lengthMatched >= maxLength) {
            थांबा;
        }
    }

    जर (prefix != nullptr) {
        *prefix = codePointsMatched;
    }
    परत word_मोजणी;
}


U_NAMESPACE_END

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
udict_swap(स्थिरांक UDataSwapper *ds, स्थिरांक शून्य *inData, int32_t length,
           शून्य *outData, UErrorCode *pErrorCode) {
    स्थिरांक UDataInfo *pInfo;
    int32_t headerSize;
    स्थिरांक uint8_t *inBytes;
    uint8_t *outBytes;
    स्थिरांक int32_t *inIndexes;
    int32_t indexes[DictionaryData::IX_मोजणी];
    int32_t i, offset, size;

    headerSize = udata_swapDataHeader(ds, inData, length, outData, pErrorCode);
    जर (pErrorCode == nullptr || U_FAILURE(*pErrorCode)) परत 0;
    pInfo = (स्थिरांक UDataInfo *)((स्थिरांक अक्षर *)inData + 4);
    जर (!(pInfo->dataFormat[0] == 0x44 && 
          pInfo->dataFormat[1] == 0x69 && 
          pInfo->dataFormat[2] == 0x63 && 
          pInfo->dataFormat[3] == 0x74 && 
          pInfo->formatVersion[0] == 1)) {
        udata_printError(ds, "udict_swap(): data format %02x.%02x.%02x.%02x (format version %02x) is not recognized as dictionary data\n",
                         pInfo->dataFormat[0], pInfo->dataFormat[1], pInfo->dataFormat[2], pInfo->dataFormat[3], pInfo->formatVersion[0]);
        *pErrorCode = U_UNSUPPORTED_ERROR;
        परत 0;
    }

    inBytes = (स्थिरांक uint8_t *)inData + headerSize;
    outBytes = (outData == nullptr) ? nullptr : (uint8_t *)outData + headerSize;

    inIndexes = (स्थिरांक int32_t *)inBytes;
    जर (length >= 0) {
        length -= headerSize;
        जर (length < (int32_t)(आकार(indexes))) {
            udata_printError(ds, "udict_swap(): too few bytes (%d after header) for dictionary data\n", length);
            *pErrorCode = U_INDEX_OUTOFBOUNDS_ERROR;
            परत 0;
        }
    }

    साठी (i = 0; i < DictionaryData::IX_मोजणी; i++) {
        indexes[i] = udata_readInt32(ds, inIndexes[i]);
    }

    size = indexes[DictionaryData::IX_TOTAL_SIZE];

    जर (length >= 0) {
        जर (length < size) {
            udata_printError(ds, "udict_swap(): too few bytes (%d after header) for all of dictionary data\n", length);
            *pErrorCode = U_INDEX_OUTOFBOUNDS_ERROR;
            परत 0;
        }

        जर (inBytes != outBytes) {
            uprv_memcpy(outBytes, inBytes, size);
        }

        offset = 0;
        ds->swapArray32(ds, inBytes, आकार(indexes), outBytes, pErrorCode);
        offset = (int32_t)आकार(indexes);
        int32_t trieType = indexes[DictionaryData::IX_TRIE_TYPE] & DictionaryData::TRIE_TYPE_MASK;
        int32_t nextOffset = indexes[DictionaryData::IX_RESERVED1_OFFSET];

        जर (trieType == DictionaryData::TRIE_TYPE_UCHARS) {
            ds->swapArray16(ds, inBytes + offset, nextOffset - offset, outBytes + offset, pErrorCode);
        } अन्यथा जर (trieType == DictionaryData::TRIE_TYPE_BYTES) {
            // nothing to do
        } अन्यथा {
            udata_printError(ds, "udict_swap(): unknown trie type!\n");
            *pErrorCode = U_UNSUPPORTED_ERROR;
            परत 0;
        }

        // these next two sections are empty in the current format,
        // but may be used later.
        offset = nextOffset;
        nextOffset = indexes[DictionaryData::IX_RESERVED2_OFFSET];
        offset = nextOffset;
        nextOffset = indexes[DictionaryData::IX_TOTAL_SIZE];
        offset = nextOffset;
    }
    परत headerSize + size;
}
#endif
