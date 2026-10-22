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

#ضمن "dictionarydata.h"
#ضمن "unicode/ucharstrie.h"
#ضمن "unicode/bytestrie.h"
#ضمن "unicode/udata.h"
#ضمن "cmemory.h"

#إذا !UCONFIG_NO_BREAK_ITERATION

U_NAMESPACE_BEGIN

ثابت int32_t  Dictionary_معلومات::TRIE_TYPE_بائيٽون = 0;
ثابت int32_t  Dictionary_معلومات::TRIE_TYPE_UCHARS = 1;
ثابت int32_t  Dictionary_معلومات::TRIE_TYPE_نقاب = 7;
ثابت int32_t  Dictionary_معلومات::TRIE_HAS_قدرون = 8;

ثابت int32_t  Dictionary_معلومات::TRANSFORM_NONE = 0;
ثابت int32_t  Dictionary_معلومات::TRANSFORM_TYPE_وٿي = 0x1000000;
ثابت int32_t  Dictionary_معلومات::TRANSFORM_TYPE_نقاب = 0x7f000000;
ثابت int32_t  Dictionary_معلومات::TRANSFORM_وٿي_نقاب = 0x1fffff;
    
DictionaryMatcher::~DictionaryMatcher() {
}

UCharsDictionaryMatcher::~UCharsDictionaryMatcher() {
    udata_close(file);
}

int32_t UCharsDictionaryMatcher::getType() ثابت {
    أرجع Dictionary_معلومات::TRIE_TYPE_UCHARS;
}

int32_t UCharsDictionaryMatcher::matches(UText *متن, int32_t max_ڊيگهائي_2, int32_t حد_بندي,
                            int32_t *lengths, int32_t *cpLengths, int32_t *قدرون,
                            int32_t *اڳياڙي) ثابت {

    UCharsTrie uct(characters);
    int32_t starting_متن_اشاريو = static_cast<int32_t>(utext_get_Native_اشاريو(متن));
    int32_t لفظ_ڳڻپ = 0;
    int32_t ڪوڊ_Points_Matched = 0;

    لكل (UChar32 c = utext_next32(متن); c >= 0; c=utext_next32(متن)) {
        UString_Trie_نتيجو نتيجو = (ڪوڊ_Points_Matched == 0) ? uct.first(c) : uct.next(c);
        int32_t ڊيگهائي_Matched = static_cast<int32_t>(utext_get_Native_اشاريو(متن)) - starting_متن_اشاريو;
        ڪوڊ_Points_Matched += 1;
        إذا (USTRINGTRIE_HAS_قدر(نتيجو)) {
            إذا (لفظ_ڳڻپ < حد_بندي) {
                إذا (قدرون != خالي_اشارو) {
                    قدرون[لفظ_ڳڻپ] = uct.get_قدر();
                }
                إذا (lengths != خالي_اشارو) {
                    lengths[لفظ_ڳڻپ] = ڊيگهائي_Matched;
                }
                إذا (cpLengths != خالي_اشارو) {
                    cpLengths[لفظ_ڳڻپ] = ڪوڊ_Points_Matched;
                }
                ++لفظ_ڳڻپ;
            }
            إذا (نتيجو == USTRINGTRIE_FINAL_قدر) {
                اكسر;
            }
        }
        وإلا إذا (نتيجو == USTRINGTRIE_NO_ميلاپ) {
            اكسر;
        }
        إذا (ڊيگهائي_Matched >= max_ڊيگهائي_2) {
            اكسر;
        }
    }

    إذا (اڳياڙي != خالي_اشارو) {
        *اڳياڙي = ڪوڊ_Points_Matched;
    }
    أرجع لفظ_ڳڻپ;
}

بائيٽون_Dictionary_Matcher::~بائيٽون_Dictionary_Matcher() {
    udata_close(file);
}

UChar32 بائيٽون_Dictionary_Matcher::transform(UChar32 c) ثابت {
    إذا ((transformConstant & Dictionary_معلومات::TRANSFORM_TYPE_نقاب) == Dictionary_معلومات::TRANSFORM_TYPE_وٿي) {
        إذا (c == 0x200D) {
            أرجع 0xFF;
        } وإلا إذا (c == 0x200C) {
            أرجع 0xFE;
        }
        int32_t delta = c - (transformConstant & Dictionary_معلومات::TRANSFORM_وٿي_نقاب);
        إذا (delta < 0 || 0xFD < delta) {
            أرجع U_SENTINEL;
        }
        أرجع static_cast<UChar32>(delta);
    }
    أرجع c;
}

int32_t بائيٽون_Dictionary_Matcher::getType() ثابت {
    أرجع Dictionary_معلومات::TRIE_TYPE_بائيٽون;
}

int32_t بائيٽون_Dictionary_Matcher::matches(UText *متن, int32_t max_ڊيگهائي_2, int32_t حد_بندي,
                            int32_t *lengths, int32_t *cpLengths, int32_t *قدرون,
                            int32_t *اڳياڙي) ثابت {
    بائيٽون_Trie bt(characters);
    int32_t starting_متن_اشاريو = static_cast<int32_t>(utext_get_Native_اشاريو(متن));
    int32_t لفظ_ڳڻپ = 0;
    int32_t ڪوڊ_Points_Matched = 0;

    لكل (UChar32 c = utext_next32(متن); c >= 0; c=utext_next32(متن)) {
        UString_Trie_نتيجو نتيجو = (ڪوڊ_Points_Matched == 0) ? bt.first(transform(c)) : bt.next(transform(c));
        int32_t ڊيگهائي_Matched = static_cast<int32_t>(utext_get_Native_اشاريو(متن)) - starting_متن_اشاريو;
        ڪوڊ_Points_Matched += 1;
        إذا (USTRINGTRIE_HAS_قدر(نتيجو)) {
            إذا (لفظ_ڳڻپ < حد_بندي) {
                إذا (قدرون != خالي_اشارو) {
                    قدرون[لفظ_ڳڻپ] = bt.get_قدر();
                }
                إذا (lengths != خالي_اشارو) {
                    lengths[لفظ_ڳڻپ] = ڊيگهائي_Matched;
                }
                إذا (cpLengths != خالي_اشارو) {
                    cpLengths[لفظ_ڳڻپ] = ڪوڊ_Points_Matched;
                }
                ++لفظ_ڳڻپ;
            }
            إذا (نتيجو == USTRINGTRIE_FINAL_قدر) {
                اكسر;
            }
        }
        وإلا إذا (نتيجو == USTRINGTRIE_NO_ميلاپ) {
            اكسر;
        }
        إذا (ڊيگهائي_Matched >= max_ڊيگهائي_2) {
            اكسر;
        }
    }

    إذا (اڳياڙي != خالي_اشارو) {
        *اڳياڙي = ڪوڊ_Points_Matched;
    }
    أرجع لفظ_ڳڻپ;
}


U_NAMESPACE_پڇاڙي

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
udict_مٽا_سٽا(ثابت UDataSwapper *ds, ثابت فراغ *اندريون_معلومات, int32_t length,
           فراغ *ٻاهريون_معلومات, UError_ڪوڊ *p_Error_ڪوڊ) {
    ثابت UDataInfo *pInfo;
    int32_t سرو_سائيز;
    ثابت uint8_t *اندريون_بائيٽون;
    uint8_t *ٻاهريون_بائيٽون;
    ثابت int32_t *اندريون_Indexes;
    int32_t indexes[Dictionary_معلومات::IX_ڳڻپ];
    int32_t i, وٿي, size;

    سرو_سائيز = udata_مٽا_سٽا_معلومات_سرو(ds, اندريون_معلومات, length, ٻاهريون_معلومات, p_Error_ڪوڊ);
    إذا (p_Error_ڪوڊ == خالي_اشارو || U_FAILURE(*p_Error_ڪوڊ)) أرجع 0;
    pInfo = (ثابت UDataInfo *)((ثابت حرف *)اندريون_معلومات + 4);
    إذا (!(pInfo->معلومات_Format[0] == 0x44 && 
          pInfo->معلومات_Format[1] == 0x69 && 
          pInfo->معلومات_Format[2] == 0x63 && 
          pInfo->معلومات_Format[3] == 0x74 && 
          pInfo->format_نسخو[0] == 1)) {
        udata_printError(ds, "udict_swap(): data format %02x.%02x.%02x.%02x (format version %02x) is not recognized as dictionary data\n",
                         pInfo->معلومات_Format[0], pInfo->معلومات_Format[1], pInfo->معلومات_Format[2], pInfo->معلومات_Format[3], pInfo->format_نسخو[0]);
        *p_Error_ڪوڊ = U_UNSUPPORTED_ERROR;
        أرجع 0;
    }

    اندريون_بائيٽون = (ثابت uint8_t *)اندريون_معلومات + سرو_سائيز;
    ٻاهريون_بائيٽون = (ٻاهريون_معلومات == خالي_اشارو) ? خالي_اشارو : (uint8_t *)ٻاهريون_معلومات + سرو_سائيز;

    اندريون_Indexes = (ثابت int32_t *)اندريون_بائيٽون;
    إذا (length >= 0) {
        length -= سرو_سائيز;
        إذا (length < (int32_t)(حجم(indexes))) {
            udata_printError(ds, "udict_swap(): too few bytes (%d after header) for dictionary data\n", length);
            *p_Error_ڪوڊ = U_اشاريو_OUTOFBOUNDS_ERROR;
            أرجع 0;
        }
    }

    لكل (i = 0; i < Dictionary_معلومات::IX_ڳڻپ; i++) {
        indexes[i] = udata_readInt32(ds, اندريون_Indexes[i]);
    }

    size = indexes[Dictionary_معلومات::IX_ڪل_سائيز];

    إذا (length >= 0) {
        إذا (length < size) {
            udata_printError(ds, "udict_swap(): too few bytes (%d after header) for all of dictionary data\n", length);
            *p_Error_ڪوڊ = U_اشاريو_OUTOFBOUNDS_ERROR;
            أرجع 0;
        }

        إذا (اندريون_بائيٽون != ٻاهريون_بائيٽون) {
            uprv_memcpy(ٻاهريون_بائيٽون, اندريون_بائيٽون, size);
        }

        وٿي = 0;
        ds->مٽا_سٽا_Array32(ds, اندريون_بائيٽون, حجم(indexes), ٻاهريون_بائيٽون, p_Error_ڪوڊ);
        وٿي = (int32_t)حجم(indexes);
        int32_t trieType = indexes[Dictionary_معلومات::IX_TRIE_TYPE] & Dictionary_معلومات::TRIE_TYPE_نقاب;
        int32_t ايندڙ_وٿي = indexes[Dictionary_معلومات::IX_RESERVED1_وٿي];

        إذا (trieType == Dictionary_معلومات::TRIE_TYPE_UCHARS) {
            ds->مٽا_سٽا_Array16(ds, اندريون_بائيٽون + وٿي, ايندڙ_وٿي - وٿي, ٻاهريون_بائيٽون + وٿي, p_Error_ڪوڊ);
        } وإلا إذا (trieType == Dictionary_معلومات::TRIE_TYPE_بائيٽون) {
            // nothing to do
        } وإلا {
            udata_printError(ds, "udict_swap(): unknown trie type!\n");
            *p_Error_ڪوڊ = U_UNSUPPORTED_ERROR;
            أرجع 0;
        }

        // these next two sections are empty in the current format,
        // but may be used later.
        وٿي = ايندڙ_وٿي;
        ايندڙ_وٿي = indexes[Dictionary_معلومات::IX_RESERVED2_وٿي];
        وٿي = ايندڙ_وٿي;
        ايندڙ_وٿي = indexes[Dictionary_معلومات::IX_ڪل_سائيز];
        وٿي = ايندڙ_وٿي;
    }
    أرجع سرو_سائيز + size;
}
#جيڪڏهن_پورو
