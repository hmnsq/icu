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

#यदि !UCONFIG_NO_BREAK_ITERATION

U_NAMESPACE_BEGIN

अचर int32_t  DictionaryData::TRIE_TYPE_BYTES = 0;
अचर int32_t  DictionaryData::TRIE_TYPE_UCHARS = 1;
अचर int32_t  DictionaryData::TRIE_TYPE_MASK = 7;
अचर int32_t  DictionaryData::TRIE_HAS_मानहरू = 8;

अचर int32_t  DictionaryData::TRANSFORM_NONE = 0;
अचर int32_t  DictionaryData::TRANSFORM_TYPE_OFFSET = 0x1000000;
अचर int32_t  DictionaryData::TRANSFORM_TYPE_MASK = 0x7f000000;
अचर int32_t  DictionaryData::TRANSFORM_OFFSET_MASK = 0x1fffff;
    
DictionaryMatcher::~DictionaryMatcher() {
}

UCharsDictionaryMatcher::~UCharsDictionaryMatcher() {
    udata_close(file);
}

int32_t UCharsDictionaryMatcher::getType() अचर {
    फर्काऊ DictionaryData::TRIE_TYPE_UCHARS;
}

int32_t UCharsDictionaryMatcher::matches(UText *text, int32_t maxLength, int32_t limit,
                            int32_t *lengths, int32_t *cpLengths, int32_t *मानहरू,
                            int32_t *prefix) अचर {

    UCharsTrie uct(characters);
    int32_t startingTextIndex = static_cast<int32_t>(utext_getNativeIndex(text));
    int32_t word_गन्ती = 0;
    int32_t codePointsMatched = 0;

    लागि (UChar32 c = utext_next32(text); c >= 0; c=utext_next32(text)) {
        UStringTrieResult result = (codePointsMatched == 0) ? uct.first(c) : uct.next(c);
        int32_t lengthMatched = static_cast<int32_t>(utext_getNativeIndex(text)) - startingTextIndex;
        codePointsMatched += 1;
        यदि (USTRINGTRIE_HAS_मान(result)) {
            यदि (word_गन्ती < limit) {
                यदि (मानहरू != nullptr) {
                    मानहरू[word_गन्ती] = uct.get_मान();
                }
                यदि (lengths != nullptr) {
                    lengths[word_गन्ती] = lengthMatched;
                }
                यदि (cpLengths != nullptr) {
                    cpLengths[word_गन्ती] = codePointsMatched;
                }
                ++word_गन्ती;
            }
            यदि (result == USTRINGTRIE_FINAL_मान) {
                रोक;
            }
        }
        अन्यथा यदि (result == USTRINGTRIE_NO_MATCH) {
            रोक;
        }
        यदि (lengthMatched >= maxLength) {
            रोक;
        }
    }

    यदि (prefix != nullptr) {
        *prefix = codePointsMatched;
    }
    फर्काऊ word_गन्ती;
}

BytesDictionaryMatcher::~BytesDictionaryMatcher() {
    udata_close(file);
}

UChar32 BytesDictionaryMatcher::transform(UChar32 c) अचर {
    यदि ((transformConstant & DictionaryData::TRANSFORM_TYPE_MASK) == DictionaryData::TRANSFORM_TYPE_OFFSET) {
        यदि (c == 0x200D) {
            फर्काऊ 0xFF;
        } अन्यथा यदि (c == 0x200C) {
            फर्काऊ 0xFE;
        }
        int32_t delta = c - (transformConstant & DictionaryData::TRANSFORM_OFFSET_MASK);
        यदि (delta < 0 || 0xFD < delta) {
            फर्काऊ U_SENTINEL;
        }
        फर्काऊ static_cast<UChar32>(delta);
    }
    फर्काऊ c;
}

int32_t BytesDictionaryMatcher::getType() अचर {
    फर्काऊ DictionaryData::TRIE_TYPE_BYTES;
}

int32_t BytesDictionaryMatcher::matches(UText *text, int32_t maxLength, int32_t limit,
                            int32_t *lengths, int32_t *cpLengths, int32_t *मानहरू,
                            int32_t *prefix) अचर {
    BytesTrie bt(characters);
    int32_t startingTextIndex = static_cast<int32_t>(utext_getNativeIndex(text));
    int32_t word_गन्ती = 0;
    int32_t codePointsMatched = 0;

    लागि (UChar32 c = utext_next32(text); c >= 0; c=utext_next32(text)) {
        UStringTrieResult result = (codePointsMatched == 0) ? bt.first(transform(c)) : bt.next(transform(c));
        int32_t lengthMatched = static_cast<int32_t>(utext_getNativeIndex(text)) - startingTextIndex;
        codePointsMatched += 1;
        यदि (USTRINGTRIE_HAS_मान(result)) {
            यदि (word_गन्ती < limit) {
                यदि (मानहरू != nullptr) {
                    मानहरू[word_गन्ती] = bt.get_मान();
                }
                यदि (lengths != nullptr) {
                    lengths[word_गन्ती] = lengthMatched;
                }
                यदि (cpLengths != nullptr) {
                    cpLengths[word_गन्ती] = codePointsMatched;
                }
                ++word_गन्ती;
            }
            यदि (result == USTRINGTRIE_FINAL_मान) {
                रोक;
            }
        }
        अन्यथा यदि (result == USTRINGTRIE_NO_MATCH) {
            रोक;
        }
        यदि (lengthMatched >= maxLength) {
            रोक;
        }
    }

    यदि (prefix != nullptr) {
        *prefix = codePointsMatched;
    }
    फर्काऊ word_गन्ती;
}


U_NAMESPACE_END

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
udict_swap(अचर UDataSwapper *ds, अचर शून्य *inData, int32_t length,
           शून्य *outData, UErrorCode *pErrorCode) {
    अचर UDataInfo *pInfo;
    int32_t headerSize;
    अचर uint8_t *inBytes;
    uint8_t *outBytes;
    अचर int32_t *inIndexes;
    int32_t indexes[DictionaryData::IX_गन्ती];
    int32_t i, offset, size;

    headerSize = udata_swapDataHeader(ds, inData, length, outData, pErrorCode);
    यदि (pErrorCode == nullptr || U_FAILURE(*pErrorCode)) फर्काऊ 0;
    pInfo = (अचर UDataInfo *)((अचर वर्ण *)inData + 4);
    यदि (!(pInfo->dataFormat[0] == 0x44 && 
          pInfo->dataFormat[1] == 0x69 && 
          pInfo->dataFormat[2] == 0x63 && 
          pInfo->dataFormat[3] == 0x74 && 
          pInfo->formatVersion[0] == 1)) {
        udata_printError(ds, "udict_swap(): data format %02x.%02x.%02x.%02x (format version %02x) is not recognized as dictionary data\n",
                         pInfo->dataFormat[0], pInfo->dataFormat[1], pInfo->dataFormat[2], pInfo->dataFormat[3], pInfo->formatVersion[0]);
        *pErrorCode = U_UNSUPPORTED_ERROR;
        फर्काऊ 0;
    }

    inBytes = (अचर uint8_t *)inData + headerSize;
    outBytes = (outData == nullptr) ? nullptr : (uint8_t *)outData + headerSize;

    inIndexes = (अचर int32_t *)inBytes;
    यदि (length >= 0) {
        length -= headerSize;
        यदि (length < (int32_t)(परिमाण(indexes))) {
            udata_printError(ds, "udict_swap(): too few bytes (%d after header) for dictionary data\n", length);
            *pErrorCode = U_INDEX_OUTOFBOUNDS_ERROR;
            फर्काऊ 0;
        }
    }

    लागि (i = 0; i < DictionaryData::IX_गन्ती; i++) {
        indexes[i] = udata_readInt32(ds, inIndexes[i]);
    }

    size = indexes[DictionaryData::IX_TOTAL_SIZE];

    यदि (length >= 0) {
        यदि (length < size) {
            udata_printError(ds, "udict_swap(): too few bytes (%d after header) for all of dictionary data\n", length);
            *pErrorCode = U_INDEX_OUTOFBOUNDS_ERROR;
            फर्काऊ 0;
        }

        यदि (inBytes != outBytes) {
            uprv_memcpy(outBytes, inBytes, size);
        }

        offset = 0;
        ds->swapArray32(ds, inBytes, परिमाण(indexes), outBytes, pErrorCode);
        offset = (int32_t)परिमाण(indexes);
        int32_t trieType = indexes[DictionaryData::IX_TRIE_TYPE] & DictionaryData::TRIE_TYPE_MASK;
        int32_t nextOffset = indexes[DictionaryData::IX_RESERVED1_OFFSET];

        यदि (trieType == DictionaryData::TRIE_TYPE_UCHARS) {
            ds->swapArray16(ds, inBytes + offset, nextOffset - offset, outBytes + offset, pErrorCode);
        } अन्यथा यदि (trieType == DictionaryData::TRIE_TYPE_BYTES) {
            // nothing to do
        } अन्यथा {
            udata_printError(ds, "udict_swap(): unknown trie type!\n");
            *pErrorCode = U_UNSUPPORTED_ERROR;
            फर्काऊ 0;
        }

        // these next two sections are empty in the current format,
        // but may be used later.
        offset = nextOffset;
        nextOffset = indexes[DictionaryData::IX_RESERVED2_OFFSET];
        offset = nextOffset;
        nextOffset = indexes[DictionaryData::IX_TOTAL_SIZE];
        offset = nextOffset;
    }
    फर्काऊ headerSize + size;
}
#endif
