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

#অন্তর্ভাব "dictionarydata.h"
#অন্তর্ভাব "unicode/ucharstrie.h"
#অন্তর্ভাব "unicode/bytestrie.h"
#অন্তর্ভাব "unicode/udata.h"
#অন্তর্ভাব "cmemory.h"

#যদি !UCONFIG_NO_BREAK_ITERATION

U_NAMESPACE_BEGIN

ধ্রুবক int32_t  Dictionary_তথ্য::TRIE_TYPE_BYTES = 0;
ধ্রুবক int32_t  Dictionary_তথ্য::TRIE_TYPE_UCHARS = 1;
ধ্রুবক int32_t  Dictionary_তথ্য::TRIE_TYPE_MASK = 7;
ধ্রুবক int32_t  Dictionary_তথ্য::TRIE_HAS_মানসমূহ = 8;

ধ্রুবক int32_t  Dictionary_তথ্য::TRANSFORM_NONE = 0;
ধ্রুবক int32_t  Dictionary_তথ্য::TRANSFORM_TYPE_OFFSET = 0x1000000;
ধ্রুবক int32_t  Dictionary_তথ্য::TRANSFORM_TYPE_MASK = 0x7f000000;
ধ্রুবক int32_t  Dictionary_তথ্য::TRANSFORM_OFFSET_MASK = 0x1fffff;
    
DictionaryMatcher::~DictionaryMatcher() {
}

UCharsDictionaryMatcher::~UCharsDictionaryMatcher() {
    udata_close(file);
}

int32_t UCharsDictionaryMatcher::getType() ধ্রুবক {
    ফেরত Dictionary_তথ্য::TRIE_TYPE_UCHARS;
}

int32_t UCharsDictionaryMatcher::matches(UText *text, int32_t max_দৈর্ঘ্য_2, int32_t limit,
                            int32_t *lengths, int32_t *cpLengths, int32_t *মানসমূহ,
                            int32_t *prefix) ধ্রুবক {

    UCharsTrie uct(characters);
    int32_t starting_Text_সূচক = static_cast<int32_t>(utext_get_Native_সূচক(text));
    int32_t word_গুনতি = 0;
    int32_t codePointsMatched = 0;

    জন্য (UChar32 c = utext_next32(text); c >= 0; c=utext_next32(text)) {
        UString_Trie_ফলাফল ফলাফল = (codePointsMatched == 0) ? uct.first(c) : uct.next(c);
        int32_t দৈর্ঘ্য_Matched = static_cast<int32_t>(utext_get_Native_সূচক(text)) - starting_Text_সূচক;
        codePointsMatched += 1;
        যদি (USTRINGTRIE_HAS_মান(ফলাফল)) {
            যদি (word_গুনতি < limit) {
                যদি (মানসমূহ != nullptr) {
                    মানসমূহ[word_গুনতি] = uct.get_মান();
                }
                যদি (lengths != nullptr) {
                    lengths[word_গুনতি] = দৈর্ঘ্য_Matched;
                }
                যদি (cpLengths != nullptr) {
                    cpLengths[word_গুনতি] = codePointsMatched;
                }
                ++word_গুনতি;
            }
            যদি (ফলাফল == USTRINGTRIE_FINAL_মান) {
                বিরতি;
            }
        }
        অন্যথা যদি (ফলাফল == USTRINGTRIE_NO_MATCH) {
            বিরতি;
        }
        যদি (দৈর্ঘ্য_Matched >= max_দৈর্ঘ্য_2) {
            বিরতি;
        }
    }

    যদি (prefix != nullptr) {
        *prefix = codePointsMatched;
    }
    ফেরত word_গুনতি;
}

BytesDictionaryMatcher::~BytesDictionaryMatcher() {
    udata_close(file);
}

UChar32 BytesDictionaryMatcher::transform(UChar32 c) ধ্রুবক {
    যদি ((transformConstant & Dictionary_তথ্য::TRANSFORM_TYPE_MASK) == Dictionary_তথ্য::TRANSFORM_TYPE_OFFSET) {
        যদি (c == 0x200D) {
            ফেরত 0xFF;
        } অন্যথা যদি (c == 0x200C) {
            ফেরত 0xFE;
        }
        int32_t delta = c - (transformConstant & Dictionary_তথ্য::TRANSFORM_OFFSET_MASK);
        যদি (delta < 0 || 0xFD < delta) {
            ফেরত U_SENTINEL;
        }
        ফেরত static_cast<UChar32>(delta);
    }
    ফেরত c;
}

int32_t BytesDictionaryMatcher::getType() ধ্রুবক {
    ফেরত Dictionary_তথ্য::TRIE_TYPE_BYTES;
}

int32_t BytesDictionaryMatcher::matches(UText *text, int32_t max_দৈর্ঘ্য_2, int32_t limit,
                            int32_t *lengths, int32_t *cpLengths, int32_t *মানসমূহ,
                            int32_t *prefix) ধ্রুবক {
    BytesTrie bt(characters);
    int32_t starting_Text_সূচক = static_cast<int32_t>(utext_get_Native_সূচক(text));
    int32_t word_গুনতি = 0;
    int32_t codePointsMatched = 0;

    জন্য (UChar32 c = utext_next32(text); c >= 0; c=utext_next32(text)) {
        UString_Trie_ফলাফল ফলাফল = (codePointsMatched == 0) ? bt.first(transform(c)) : bt.next(transform(c));
        int32_t দৈর্ঘ্য_Matched = static_cast<int32_t>(utext_get_Native_সূচক(text)) - starting_Text_সূচক;
        codePointsMatched += 1;
        যদি (USTRINGTRIE_HAS_মান(ফলাফল)) {
            যদি (word_গুনতি < limit) {
                যদি (মানসমূহ != nullptr) {
                    মানসমূহ[word_গুনতি] = bt.get_মান();
                }
                যদি (lengths != nullptr) {
                    lengths[word_গুনতি] = দৈর্ঘ্য_Matched;
                }
                যদি (cpLengths != nullptr) {
                    cpLengths[word_গুনতি] = codePointsMatched;
                }
                ++word_গুনতি;
            }
            যদি (ফলাফল == USTRINGTRIE_FINAL_মান) {
                বিরতি;
            }
        }
        অন্যথা যদি (ফলাফল == USTRINGTRIE_NO_MATCH) {
            বিরতি;
        }
        যদি (দৈর্ঘ্য_Matched >= max_দৈর্ঘ্য_2) {
            বিরতি;
        }
    }

    যদি (prefix != nullptr) {
        *prefix = codePointsMatched;
    }
    ফেরত word_গুনতি;
}


U_NAMESPACE_END

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
udict_swap(ধ্রুবক UDataSwapper *ds, ধ্রুবক শূন্য *in_তথ্য, int32_t length,
           শূন্য *out_তথ্য, UErrorCode *pErrorCode) {
    ধ্রুবক UDataInfo *pInfo;
    int32_t header_আকার;
    ধ্রুবক uint8_t *inBytes;
    uint8_t *outBytes;
    ধ্রুবক int32_t *inIndexes;
    int32_t indexes[Dictionary_তথ্য::IX_গুনতি];
    int32_t i, offset, size;

    header_আকার = udata_swap_তথ্য_Header(ds, in_তথ্য, length, out_তথ্য, pErrorCode);
    যদি (pErrorCode == nullptr || U_FAILURE(*pErrorCode)) ফেরত 0;
    pInfo = (ধ্রুবক UDataInfo *)((ধ্রুবক অক্ষর *)in_তথ্য + 4);
    যদি (!(pInfo->তথ্য_Format[0] == 0x44 && 
          pInfo->তথ্য_Format[1] == 0x69 && 
          pInfo->তথ্য_Format[2] == 0x63 && 
          pInfo->তথ্য_Format[3] == 0x74 && 
          pInfo->formatVersion[0] == 1)) {
        udata_printError(ds, "udict_swap(): data format %02x.%02x.%02x.%02x (format version %02x) is not recognized as dictionary data\n",
                         pInfo->তথ্য_Format[0], pInfo->তথ্য_Format[1], pInfo->তথ্য_Format[2], pInfo->তথ্য_Format[3], pInfo->formatVersion[0]);
        *pErrorCode = U_UNSUPPORTED_ERROR;
        ফেরত 0;
    }

    inBytes = (ধ্রুবক uint8_t *)in_তথ্য + header_আকার;
    outBytes = (out_তথ্য == nullptr) ? nullptr : (uint8_t *)out_তথ্য + header_আকার;

    inIndexes = (ধ্রুবক int32_t *)inBytes;
    যদি (length >= 0) {
        length -= header_আকার;
        যদি (length < (int32_t)(আকার(indexes))) {
            udata_printError(ds, "udict_swap(): too few bytes (%d after header) for dictionary data\n", length);
            *pErrorCode = U_সূচক_OUTOFBOUNDS_ERROR;
            ফেরত 0;
        }
    }

    জন্য (i = 0; i < Dictionary_তথ্য::IX_গুনতি; i++) {
        indexes[i] = udata_readInt32(ds, inIndexes[i]);
    }

    size = indexes[Dictionary_তথ্য::IX_যোগ_আকার];

    যদি (length >= 0) {
        যদি (length < size) {
            udata_printError(ds, "udict_swap(): too few bytes (%d after header) for all of dictionary data\n", length);
            *pErrorCode = U_সূচক_OUTOFBOUNDS_ERROR;
            ফেরত 0;
        }

        যদি (inBytes != outBytes) {
            uprv_memcpy(outBytes, inBytes, size);
        }

        offset = 0;
        ds->swapArray32(ds, inBytes, আকার(indexes), outBytes, pErrorCode);
        offset = (int32_t)আকার(indexes);
        int32_t trieType = indexes[Dictionary_তথ্য::IX_TRIE_TYPE] & Dictionary_তথ্য::TRIE_TYPE_MASK;
        int32_t পরবর্তী_Offset = indexes[Dictionary_তথ্য::IX_RESERVED1_OFFSET];

        যদি (trieType == Dictionary_তথ্য::TRIE_TYPE_UCHARS) {
            ds->swapArray16(ds, inBytes + offset, পরবর্তী_Offset - offset, outBytes + offset, pErrorCode);
        } অন্যথা যদি (trieType == Dictionary_তথ্য::TRIE_TYPE_BYTES) {
            // nothing to do
        } অন্যথা {
            udata_printError(ds, "udict_swap(): unknown trie type!\n");
            *pErrorCode = U_UNSUPPORTED_ERROR;
            ফেরত 0;
        }

        // these next two sections are empty in the current format,
        // but may be used later.
        offset = পরবর্তী_Offset;
        পরবর্তী_Offset = indexes[Dictionary_তথ্য::IX_RESERVED2_OFFSET];
        offset = পরবর্তী_Offset;
        পরবর্তী_Offset = indexes[Dictionary_তথ্য::IX_যোগ_আকার];
        offset = পরবর্তী_Offset;
    }
    ফেরত header_আকার + size;
}
#endif
