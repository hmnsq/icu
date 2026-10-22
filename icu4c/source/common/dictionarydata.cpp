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

#समावेश "dictionarydata.h"
#समावेश "unicode/ucharstrie.h"
#समावेश "unicode/bytestrie.h"
#समावेश "unicode/udata.h"
#समावेश "cmemory.h"

#अगर !UCONFIG_NO_BREAK_ITERATION

U_NAMESPACE_BEGIN

स्थिर int32_t  Dictionary_तथ्य::TRIE_TYPE_BYTES = 0;
स्थिर int32_t  Dictionary_तथ्य::TRIE_TYPE_UCHARS = 1;
स्थिर int32_t  Dictionary_तथ्य::TRIE_TYPE_MASK = 7;
स्थिर int32_t  Dictionary_तथ्य::TRIE_HAS_मूल्यावली = 8;

स्थिर int32_t  Dictionary_तथ्य::TRANSFORM_NONE = 0;
स्थिर int32_t  Dictionary_तथ्य::TRANSFORM_TYPE_OFFSET = 0x1000000;
स्थिर int32_t  Dictionary_तथ्य::TRANSFORM_TYPE_MASK = 0x7f000000;
स्थिर int32_t  Dictionary_तथ्य::TRANSFORM_OFFSET_MASK = 0x1fffff;
    
DictionaryMatcher::~DictionaryMatcher() {
}

UCharsDictionaryMatcher::~UCharsDictionaryMatcher() {
    udata_close(file);
}

int32_t UCharsDictionaryMatcher::getType() स्थिर {
    वापस Dictionary_तथ्य::TRIE_TYPE_UCHARS;
}

int32_t UCharsDictionaryMatcher::matches(UText *text, int32_t max_लंबाई_2, int32_t limit,
                            int32_t *lengths, int32_t *cpLengths, int32_t *मूल्यावली,
                            int32_t *prefix) स्थिर {

    UCharsTrie uct(characters);
    int32_t starting_Text_सूचकांक = static_cast<int32_t>(utext_get_Native_सूचकांक(text));
    int32_t word_गिनती = 0;
    int32_t codePointsMatched = 0;

    क्रम (UChar32 c = utext_next32(text); c >= 0; c=utext_next32(text)) {
        UString_Trie_परिणाम परिणाम = (codePointsMatched == 0) ? uct.first(c) : uct.next(c);
        int32_t लंबाई_Matched = static_cast<int32_t>(utext_get_Native_सूचकांक(text)) - starting_Text_सूचकांक;
        codePointsMatched += 1;
        अगर (USTRINGTRIE_HAS_मूल्य(परिणाम)) {
            अगर (word_गिनती < limit) {
                अगर (मूल्यावली != nullptr) {
                    मूल्यावली[word_गिनती] = uct.get_मूल्य();
                }
                अगर (lengths != nullptr) {
                    lengths[word_गिनती] = लंबाई_Matched;
                }
                अगर (cpLengths != nullptr) {
                    cpLengths[word_गिनती] = codePointsMatched;
                }
                ++word_गिनती;
            }
            अगर (परिणाम == USTRINGTRIE_FINAL_मूल्य) {
                अवरोध;
            }
        }
        अन्यथा अगर (परिणाम == USTRINGTRIE_NO_MATCH) {
            अवरोध;
        }
        अगर (लंबाई_Matched >= max_लंबाई_2) {
            अवरोध;
        }
    }

    अगर (prefix != nullptr) {
        *prefix = codePointsMatched;
    }
    वापस word_गिनती;
}

BytesDictionaryMatcher::~BytesDictionaryMatcher() {
    udata_close(file);
}

UChar32 BytesDictionaryMatcher::transform(UChar32 c) स्थिर {
    अगर ((transformConstant & Dictionary_तथ्य::TRANSFORM_TYPE_MASK) == Dictionary_तथ्य::TRANSFORM_TYPE_OFFSET) {
        अगर (c == 0x200D) {
            वापस 0xFF;
        } अन्यथा अगर (c == 0x200C) {
            वापस 0xFE;
        }
        int32_t delta = c - (transformConstant & Dictionary_तथ्य::TRANSFORM_OFFSET_MASK);
        अगर (delta < 0 || 0xFD < delta) {
            वापस U_SENTINEL;
        }
        वापस static_cast<UChar32>(delta);
    }
    वापस c;
}

int32_t BytesDictionaryMatcher::getType() स्थिर {
    वापस Dictionary_तथ्य::TRIE_TYPE_BYTES;
}

int32_t BytesDictionaryMatcher::matches(UText *text, int32_t max_लंबाई_2, int32_t limit,
                            int32_t *lengths, int32_t *cpLengths, int32_t *मूल्यावली,
                            int32_t *prefix) स्थिर {
    BytesTrie bt(characters);
    int32_t starting_Text_सूचकांक = static_cast<int32_t>(utext_get_Native_सूचकांक(text));
    int32_t word_गिनती = 0;
    int32_t codePointsMatched = 0;

    क्रम (UChar32 c = utext_next32(text); c >= 0; c=utext_next32(text)) {
        UString_Trie_परिणाम परिणाम = (codePointsMatched == 0) ? bt.first(transform(c)) : bt.next(transform(c));
        int32_t लंबाई_Matched = static_cast<int32_t>(utext_get_Native_सूचकांक(text)) - starting_Text_सूचकांक;
        codePointsMatched += 1;
        अगर (USTRINGTRIE_HAS_मूल्य(परिणाम)) {
            अगर (word_गिनती < limit) {
                अगर (मूल्यावली != nullptr) {
                    मूल्यावली[word_गिनती] = bt.get_मूल्य();
                }
                अगर (lengths != nullptr) {
                    lengths[word_गिनती] = लंबाई_Matched;
                }
                अगर (cpLengths != nullptr) {
                    cpLengths[word_गिनती] = codePointsMatched;
                }
                ++word_गिनती;
            }
            अगर (परिणाम == USTRINGTRIE_FINAL_मूल्य) {
                अवरोध;
            }
        }
        अन्यथा अगर (परिणाम == USTRINGTRIE_NO_MATCH) {
            अवरोध;
        }
        अगर (लंबाई_Matched >= max_लंबाई_2) {
            अवरोध;
        }
    }

    अगर (prefix != nullptr) {
        *prefix = codePointsMatched;
    }
    वापस word_गिनती;
}


U_NAMESPACE_END

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
udict_swap(स्थिर UDataSwapper *ds, स्थिर व्योम *in_तथ्य, int32_t length,
           व्योम *out_तथ्य, UErrorCode *pErrorCode) {
    स्थिर UDataInfo *pInfo;
    int32_t header_आकार;
    स्थिर uint8_t *inBytes;
    uint8_t *outBytes;
    स्थिर int32_t *inIndexes;
    int32_t indexes[Dictionary_तथ्य::IX_गिनती];
    int32_t i, offset, size;

    header_आकार = udata_swap_तथ्य_Header(ds, in_तथ्य, length, out_तथ्य, pErrorCode);
    अगर (pErrorCode == nullptr || U_FAILURE(*pErrorCode)) वापस 0;
    pInfo = (स्थिर UDataInfo *)((स्थिर अक्षर *)in_तथ्य + 4);
    अगर (!(pInfo->तथ्य_Format[0] == 0x44 && 
          pInfo->तथ्य_Format[1] == 0x69 && 
          pInfo->तथ्य_Format[2] == 0x63 && 
          pInfo->तथ्य_Format[3] == 0x74 && 
          pInfo->formatVersion[0] == 1)) {
        udata_printError(ds, "udict_swap(): data format %02x.%02x.%02x.%02x (format version %02x) is not recognized as dictionary data\n",
                         pInfo->तथ्य_Format[0], pInfo->तथ्य_Format[1], pInfo->तथ्य_Format[2], pInfo->तथ्य_Format[3], pInfo->formatVersion[0]);
        *pErrorCode = U_UNSUPPORTED_ERROR;
        वापस 0;
    }

    inBytes = (स्थिर uint8_t *)in_तथ्य + header_आकार;
    outBytes = (out_तथ्य == nullptr) ? nullptr : (uint8_t *)out_तथ्य + header_आकार;

    inIndexes = (स्थिर int32_t *)inBytes;
    अगर (length >= 0) {
        length -= header_आकार;
        अगर (length < (int32_t)(माप(indexes))) {
            udata_printError(ds, "udict_swap(): too few bytes (%d after header) for dictionary data\n", length);
            *pErrorCode = U_सूचकांक_OUTOFBOUNDS_ERROR;
            वापस 0;
        }
    }

    क्रम (i = 0; i < Dictionary_तथ्य::IX_गिनती; i++) {
        indexes[i] = udata_readInt32(ds, inIndexes[i]);
    }

    size = indexes[Dictionary_तथ्य::IX_योग_आकार];

    अगर (length >= 0) {
        अगर (length < size) {
            udata_printError(ds, "udict_swap(): too few bytes (%d after header) for all of dictionary data\n", length);
            *pErrorCode = U_सूचकांक_OUTOFBOUNDS_ERROR;
            वापस 0;
        }

        अगर (inBytes != outBytes) {
            uprv_memcpy(outBytes, inBytes, size);
        }

        offset = 0;
        ds->swapArray32(ds, inBytes, माप(indexes), outBytes, pErrorCode);
        offset = (int32_t)माप(indexes);
        int32_t trieType = indexes[Dictionary_तथ्य::IX_TRIE_TYPE] & Dictionary_तथ्य::TRIE_TYPE_MASK;
        int32_t अग्र_Offset = indexes[Dictionary_तथ्य::IX_RESERVED1_OFFSET];

        अगर (trieType == Dictionary_तथ्य::TRIE_TYPE_UCHARS) {
            ds->swapArray16(ds, inBytes + offset, अग्र_Offset - offset, outBytes + offset, pErrorCode);
        } अन्यथा अगर (trieType == Dictionary_तथ्य::TRIE_TYPE_BYTES) {
            // nothing to do
        } अन्यथा {
            udata_printError(ds, "udict_swap(): unknown trie type!\n");
            *pErrorCode = U_UNSUPPORTED_ERROR;
            वापस 0;
        }

        // these next two sections are empty in the current format,
        // but may be used later.
        offset = अग्र_Offset;
        अग्र_Offset = indexes[Dictionary_तथ्य::IX_RESERVED2_OFFSET];
        offset = अग्र_Offset;
        अग्र_Offset = indexes[Dictionary_तथ्य::IX_योग_आकार];
        offset = अग्र_Offset;
    }
    वापस header_आकार + size;
}
#पूर्ण_अगर
