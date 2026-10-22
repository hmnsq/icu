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

#অন্তর্ভাৱ "dictionarydata.h"
#অন্তর্ভাৱ "unicode/ucharstrie.h"
#অন্তর্ভাৱ "unicode/bytestrie.h"
#অন্তর্ভাৱ "unicode/udata.h"
#অন্তর্ভাৱ "cmemory.h"

#যদি !UCONFIG_NO_BREAK_ITERATION

U_NAMESPACE_BEGIN

ধ্রুৱ int32_t  DictionaryData::TRIE_TYPE_BYTES = 0;
ধ্রুৱ int32_t  DictionaryData::TRIE_TYPE_UCHARS = 1;
ধ্রুৱ int32_t  DictionaryData::TRIE_TYPE_MASK = 7;
ধ্রুৱ int32_t  DictionaryData::TRIE_HAS_মানসমূহ = 8;

ধ্রুৱ int32_t  DictionaryData::TRANSFORM_NONE = 0;
ধ্রুৱ int32_t  DictionaryData::TRANSFORM_TYPE_OFFSET = 0x1000000;
ধ্রুৱ int32_t  DictionaryData::TRANSFORM_TYPE_MASK = 0x7f000000;
ধ্রুৱ int32_t  DictionaryData::TRANSFORM_OFFSET_MASK = 0x1fffff;
    
DictionaryMatcher::~DictionaryMatcher() {
}

UCharsDictionaryMatcher::~UCharsDictionaryMatcher() {
    udata_close(file);
}

int32_t UCharsDictionaryMatcher::getType() ধ্রুৱ {
    ঘূৰাই DictionaryData::TRIE_TYPE_UCHARS;
}

int32_t UCharsDictionaryMatcher::matches(UText *text, int32_t maxLength, int32_t limit,
                            int32_t *lengths, int32_t *cpLengths, int32_t *মানসমূহ,
                            int32_t *prefix) ধ্রুৱ {

    UCharsTrie uct(characters);
    int32_t startingTextIndex = static_cast<int32_t>(utext_getNativeIndex(text));
    int32_t word_গুনতি = 0;
    int32_t codePointsMatched = 0;

    বাবে (UChar32 c = utext_next32(text); c >= 0; c=utext_next32(text)) {
        UStringTrieResult result = (codePointsMatched == 0) ? uct.first(c) : uct.next(c);
        int32_t lengthMatched = static_cast<int32_t>(utext_getNativeIndex(text)) - startingTextIndex;
        codePointsMatched += 1;
        যদি (USTRINGTRIE_HAS_মান(result)) {
            যদি (word_গুনতি < limit) {
                যদি (মানসমূহ != nullptr) {
                    মানসমূহ[word_গুনতি] = uct.get_মান();
                }
                যদি (lengths != nullptr) {
                    lengths[word_গুনতি] = lengthMatched;
                }
                যদি (cpLengths != nullptr) {
                    cpLengths[word_গুনতি] = codePointsMatched;
                }
                ++word_গুনতি;
            }
            যদি (result == USTRINGTRIE_FINAL_মান) {
                ভঙ্গ;
            }
        }
        অন্যথা যদি (result == USTRINGTRIE_NO_MATCH) {
            ভঙ্গ;
        }
        যদি (lengthMatched >= maxLength) {
            ভঙ্গ;
        }
    }

    যদি (prefix != nullptr) {
        *prefix = codePointsMatched;
    }
    ঘূৰাই word_গুনতি;
}

BytesDictionaryMatcher::~BytesDictionaryMatcher() {
    udata_close(file);
}

UChar32 BytesDictionaryMatcher::transform(UChar32 c) ধ্রুৱ {
    যদি ((transformConstant & DictionaryData::TRANSFORM_TYPE_MASK) == DictionaryData::TRANSFORM_TYPE_OFFSET) {
        যদি (c == 0x200D) {
            ঘূৰাই 0xFF;
        } অন্যথা যদি (c == 0x200C) {
            ঘূৰাই 0xFE;
        }
        int32_t delta = c - (transformConstant & DictionaryData::TRANSFORM_OFFSET_MASK);
        যদি (delta < 0 || 0xFD < delta) {
            ঘূৰাই U_SENTINEL;
        }
        ঘূৰাই static_cast<UChar32>(delta);
    }
    ঘূৰাই c;
}

int32_t BytesDictionaryMatcher::getType() ধ্রুৱ {
    ঘূৰাই DictionaryData::TRIE_TYPE_BYTES;
}

int32_t BytesDictionaryMatcher::matches(UText *text, int32_t maxLength, int32_t limit,
                            int32_t *lengths, int32_t *cpLengths, int32_t *মানসমূহ,
                            int32_t *prefix) ধ্রুৱ {
    BytesTrie bt(characters);
    int32_t startingTextIndex = static_cast<int32_t>(utext_getNativeIndex(text));
    int32_t word_গুনতি = 0;
    int32_t codePointsMatched = 0;

    বাবে (UChar32 c = utext_next32(text); c >= 0; c=utext_next32(text)) {
        UStringTrieResult result = (codePointsMatched == 0) ? bt.first(transform(c)) : bt.next(transform(c));
        int32_t lengthMatched = static_cast<int32_t>(utext_getNativeIndex(text)) - startingTextIndex;
        codePointsMatched += 1;
        যদি (USTRINGTRIE_HAS_মান(result)) {
            যদি (word_গুনতি < limit) {
                যদি (মানসমূহ != nullptr) {
                    মানসমূহ[word_গুনতি] = bt.get_মান();
                }
                যদি (lengths != nullptr) {
                    lengths[word_গুনতি] = lengthMatched;
                }
                যদি (cpLengths != nullptr) {
                    cpLengths[word_গুনতি] = codePointsMatched;
                }
                ++word_গুনতি;
            }
            যদি (result == USTRINGTRIE_FINAL_মান) {
                ভঙ্গ;
            }
        }
        অন্যথা যদি (result == USTRINGTRIE_NO_MATCH) {
            ভঙ্গ;
        }
        যদি (lengthMatched >= maxLength) {
            ভঙ্গ;
        }
    }

    যদি (prefix != nullptr) {
        *prefix = codePointsMatched;
    }
    ঘূৰাই word_গুনতি;
}


U_NAMESPACE_END

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
udict_swap(ধ্রুৱ UDataSwapper *ds, ধ্রুৱ শূন্য *inData, int32_t length,
           শূন্য *outData, UErrorCode *pErrorCode) {
    ধ্রুৱ UDataInfo *pInfo;
    int32_t headerSize;
    ধ্রুৱ uint8_t *inBytes;
    uint8_t *outBytes;
    ধ্রুৱ int32_t *inIndexes;
    int32_t indexes[DictionaryData::IX_গুনতি];
    int32_t i, offset, size;

    headerSize = udata_swapDataHeader(ds, inData, length, outData, pErrorCode);
    যদি (pErrorCode == nullptr || U_FAILURE(*pErrorCode)) ঘূৰাই 0;
    pInfo = (ধ্রুৱ UDataInfo *)((ধ্রুৱ ৱর্ণ *)inData + 4);
    যদি (!(pInfo->dataFormat[0] == 0x44 && 
          pInfo->dataFormat[1] == 0x69 && 
          pInfo->dataFormat[2] == 0x63 && 
          pInfo->dataFormat[3] == 0x74 && 
          pInfo->formatVersion[0] == 1)) {
        udata_printError(ds, "udict_swap(): data format %02x.%02x.%02x.%02x (format version %02x) is not recognized as dictionary data\n",
                         pInfo->dataFormat[0], pInfo->dataFormat[1], pInfo->dataFormat[2], pInfo->dataFormat[3], pInfo->formatVersion[0]);
        *pErrorCode = U_UNSUPPORTED_ERROR;
        ঘূৰাই 0;
    }

    inBytes = (ধ্রুৱ uint8_t *)inData + headerSize;
    outBytes = (outData == nullptr) ? nullptr : (uint8_t *)outData + headerSize;

    inIndexes = (ধ্রুৱ int32_t *)inBytes;
    যদি (length >= 0) {
        length -= headerSize;
        যদি (length < (int32_t)(পরিমাণ(indexes))) {
            udata_printError(ds, "udict_swap(): too few bytes (%d after header) for dictionary data\n", length);
            *pErrorCode = U_INDEX_OUTOFBOUNDS_ERROR;
            ঘূৰাই 0;
        }
    }

    বাবে (i = 0; i < DictionaryData::IX_গুনতি; i++) {
        indexes[i] = udata_readInt32(ds, inIndexes[i]);
    }

    size = indexes[DictionaryData::IX_TOTAL_SIZE];

    যদি (length >= 0) {
        যদি (length < size) {
            udata_printError(ds, "udict_swap(): too few bytes (%d after header) for all of dictionary data\n", length);
            *pErrorCode = U_INDEX_OUTOFBOUNDS_ERROR;
            ঘূৰাই 0;
        }

        যদি (inBytes != outBytes) {
            uprv_memcpy(outBytes, inBytes, size);
        }

        offset = 0;
        ds->swapArray32(ds, inBytes, পরিমাণ(indexes), outBytes, pErrorCode);
        offset = (int32_t)পরিমাণ(indexes);
        int32_t trieType = indexes[DictionaryData::IX_TRIE_TYPE] & DictionaryData::TRIE_TYPE_MASK;
        int32_t nextOffset = indexes[DictionaryData::IX_RESERVED1_OFFSET];

        যদি (trieType == DictionaryData::TRIE_TYPE_UCHARS) {
            ds->swapArray16(ds, inBytes + offset, nextOffset - offset, outBytes + offset, pErrorCode);
        } অন্যথা যদি (trieType == DictionaryData::TRIE_TYPE_BYTES) {
            // nothing to do
        } অন্যথা {
            udata_printError(ds, "udict_swap(): unknown trie type!\n");
            *pErrorCode = U_UNSUPPORTED_ERROR;
            ঘূৰাই 0;
        }

        // these next two sections are empty in the current format,
        // but may be used later.
        offset = nextOffset;
        nextOffset = indexes[DictionaryData::IX_RESERVED2_OFFSET];
        offset = nextOffset;
        nextOffset = indexes[DictionaryData::IX_TOTAL_SIZE];
        offset = nextOffset;
    }
    ঘূৰাই headerSize + size;
}
#endif
