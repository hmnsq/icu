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

ثابت int32_t  Dictionary_انکڑے::TRIE_TYPE_بائٹاں = 0;
ثابت int32_t  Dictionary_انکڑے::TRIE_TYPE_UCHARS = 1;
ثابت int32_t  Dictionary_انکڑے::TRIE_TYPE_نقاب = 7;
ثابت int32_t  Dictionary_انکڑے::TRIE_HAS_اقدار = 8;

ثابت int32_t  Dictionary_انکڑے::TRANSFORM_NONE = 0;
ثابت int32_t  Dictionary_انکڑے::TRANSFORM_TYPE_وتھ = 0x1000000;
ثابت int32_t  Dictionary_انکڑے::TRANSFORM_TYPE_نقاب = 0x7f000000;
ثابت int32_t  Dictionary_انکڑے::TRANSFORM_وتھ_نقاب = 0x1fffff;
    
DictionaryMatcher::~DictionaryMatcher() {
}

UCharsDictionaryMatcher::~UCharsDictionaryMatcher() {
    udata_close(file);
}

int32_t UCharsDictionaryMatcher::getType() ثابت {
    أرجع Dictionary_انکڑے::TRIE_TYPE_UCHARS;
}

int32_t UCharsDictionaryMatcher::matches(UText *متن, int32_t max_لمائی_2, int32_t حد_بندی,
                            int32_t *lengths, int32_t *cpLengths, int32_t *اقدار,
                            int32_t *اگیتر) ثابت {

    UCharsTrie uct(characters);
    int32_t starting_متن_اشاریہ = static_cast<int32_t>(utext_get_Native_اشاریہ(متن));
    int32_t لفظ_گنتی = 0;
    int32_t کوڈ_Points_Matched = 0;

    لكل (UChar32 c = utext_next32(متن); c >= 0; c=utext_next32(متن)) {
        UString_Trie_نتیجہ نتیجہ = (کوڈ_Points_Matched == 0) ? uct.first(c) : uct.next(c);
        int32_t لمائی_Matched = static_cast<int32_t>(utext_get_Native_اشاریہ(متن)) - starting_متن_اشاریہ;
        کوڈ_Points_Matched += 1;
        إذا (USTRINGTRIE_HAS_قدر(نتیجہ)) {
            إذا (لفظ_گنتی < حد_بندی) {
                إذا (اقدار != خالی_سوچک) {
                    اقدار[لفظ_گنتی] = uct.get_قدر();
                }
                إذا (lengths != خالی_سوچک) {
                    lengths[لفظ_گنتی] = لمائی_Matched;
                }
                إذا (cpLengths != خالی_سوچک) {
                    cpLengths[لفظ_گنتی] = کوڈ_Points_Matched;
                }
                ++لفظ_گنتی;
            }
            إذا (نتیجہ == USTRINGTRIE_FINAL_قدر) {
                اكسر;
            }
        }
        وإلا إذا (نتیجہ == USTRINGTRIE_NO_میل) {
            اكسر;
        }
        إذا (لمائی_Matched >= max_لمائی_2) {
            اكسر;
        }
    }

    إذا (اگیتر != خالی_سوچک) {
        *اگیتر = کوڈ_Points_Matched;
    }
    أرجع لفظ_گنتی;
}

بائٹاں_Dictionary_Matcher::~بائٹاں_Dictionary_Matcher() {
    udata_close(file);
}

UChar32 بائٹاں_Dictionary_Matcher::transform(UChar32 c) ثابت {
    إذا ((transformConstant & Dictionary_انکڑے::TRANSFORM_TYPE_نقاب) == Dictionary_انکڑے::TRANSFORM_TYPE_وتھ) {
        إذا (c == 0x200D) {
            أرجع 0xFF;
        } وإلا إذا (c == 0x200C) {
            أرجع 0xFE;
        }
        int32_t delta = c - (transformConstant & Dictionary_انکڑے::TRANSFORM_وتھ_نقاب);
        إذا (delta < 0 || 0xFD < delta) {
            أرجع U_SENTINEL;
        }
        أرجع static_cast<UChar32>(delta);
    }
    أرجع c;
}

int32_t بائٹاں_Dictionary_Matcher::getType() ثابت {
    أرجع Dictionary_انکڑے::TRIE_TYPE_بائٹاں;
}

int32_t بائٹاں_Dictionary_Matcher::matches(UText *متن, int32_t max_لمائی_2, int32_t حد_بندی,
                            int32_t *lengths, int32_t *cpLengths, int32_t *اقدار,
                            int32_t *اگیتر) ثابت {
    بائٹاں_Trie bt(characters);
    int32_t starting_متن_اشاریہ = static_cast<int32_t>(utext_get_Native_اشاریہ(متن));
    int32_t لفظ_گنتی = 0;
    int32_t کوڈ_Points_Matched = 0;

    لكل (UChar32 c = utext_next32(متن); c >= 0; c=utext_next32(متن)) {
        UString_Trie_نتیجہ نتیجہ = (کوڈ_Points_Matched == 0) ? bt.first(transform(c)) : bt.next(transform(c));
        int32_t لمائی_Matched = static_cast<int32_t>(utext_get_Native_اشاریہ(متن)) - starting_متن_اشاریہ;
        کوڈ_Points_Matched += 1;
        إذا (USTRINGTRIE_HAS_قدر(نتیجہ)) {
            إذا (لفظ_گنتی < حد_بندی) {
                إذا (اقدار != خالی_سوچک) {
                    اقدار[لفظ_گنتی] = bt.get_قدر();
                }
                إذا (lengths != خالی_سوچک) {
                    lengths[لفظ_گنتی] = لمائی_Matched;
                }
                إذا (cpLengths != خالی_سوچک) {
                    cpLengths[لفظ_گنتی] = کوڈ_Points_Matched;
                }
                ++لفظ_گنتی;
            }
            إذا (نتیجہ == USTRINGTRIE_FINAL_قدر) {
                اكسر;
            }
        }
        وإلا إذا (نتیجہ == USTRINGTRIE_NO_میل) {
            اكسر;
        }
        إذا (لمائی_Matched >= max_لمائی_2) {
            اكسر;
        }
    }

    إذا (اگیتر != خالی_سوچک) {
        *اگیتر = کوڈ_Points_Matched;
    }
    أرجع لفظ_گنتی;
}


U_NAMESPACE_سرا

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
udict_وٹاندرا(ثابت UDataSwapper *ds, ثابت فراغ *وچ_انکڑے, int32_t length,
           فراغ *باہرلا_انکڑے, UError_کوڈ *p_Error_کوڈ) {
    ثابت UDataInfo *pInfo;
    int32_t سرخی_ناپ;
    ثابت uint8_t *وچ_بائٹاں;
    uint8_t *باہرلا_بائٹاں;
    ثابت int32_t *وچ_Indexes;
    int32_t indexes[Dictionary_انکڑے::IX_گنتی];
    int32_t i, وتھ, size;

    سرخی_ناپ = udata_وٹاندرا_انکڑے_سرخی(ds, وچ_انکڑے, length, باہرلا_انکڑے, p_Error_کوڈ);
    إذا (p_Error_کوڈ == خالی_سوچک || U_FAILURE(*p_Error_کوڈ)) أرجع 0;
    pInfo = (ثابت UDataInfo *)((ثابت حرف *)وچ_انکڑے + 4);
    إذا (!(pInfo->انکڑے_Format[0] == 0x44 && 
          pInfo->انکڑے_Format[1] == 0x69 && 
          pInfo->انکڑے_Format[2] == 0x63 && 
          pInfo->انکڑے_Format[3] == 0x74 && 
          pInfo->format_نسخہ[0] == 1)) {
        udata_printError(ds, "udict_swap(): data format %02x.%02x.%02x.%02x (format version %02x) is not recognized as dictionary data\n",
                         pInfo->انکڑے_Format[0], pInfo->انکڑے_Format[1], pInfo->انکڑے_Format[2], pInfo->انکڑے_Format[3], pInfo->format_نسخہ[0]);
        *p_Error_کوڈ = U_UNSUPPORTED_ERROR;
        أرجع 0;
    }

    وچ_بائٹاں = (ثابت uint8_t *)وچ_انکڑے + سرخی_ناپ;
    باہرلا_بائٹاں = (باہرلا_انکڑے == خالی_سوچک) ? خالی_سوچک : (uint8_t *)باہرلا_انکڑے + سرخی_ناپ;

    وچ_Indexes = (ثابت int32_t *)وچ_بائٹاں;
    إذا (length >= 0) {
        length -= سرخی_ناپ;
        إذا (length < (int32_t)(حجم(indexes))) {
            udata_printError(ds, "udict_swap(): too few bytes (%d after header) for dictionary data\n", length);
            *p_Error_کوڈ = U_اشاریہ_OUTOFBOUNDS_ERROR;
            أرجع 0;
        }
    }

    لكل (i = 0; i < Dictionary_انکڑے::IX_گنتی; i++) {
        indexes[i] = udata_readInt32(ds, وچ_Indexes[i]);
    }

    size = indexes[Dictionary_انکڑے::IX_کل_ناپ];

    إذا (length >= 0) {
        إذا (length < size) {
            udata_printError(ds, "udict_swap(): too few bytes (%d after header) for all of dictionary data\n", length);
            *p_Error_کوڈ = U_اشاریہ_OUTOFBOUNDS_ERROR;
            أرجع 0;
        }

        إذا (وچ_بائٹاں != باہرلا_بائٹاں) {
            uprv_memcpy(باہرلا_بائٹاں, وچ_بائٹاں, size);
        }

        وتھ = 0;
        ds->وٹاندرا_Array32(ds, وچ_بائٹاں, حجم(indexes), باہرلا_بائٹاں, p_Error_کوڈ);
        وتھ = (int32_t)حجم(indexes);
        int32_t trieType = indexes[Dictionary_انکڑے::IX_TRIE_TYPE] & Dictionary_انکڑے::TRIE_TYPE_نقاب;
        int32_t اگلا_وتھ = indexes[Dictionary_انکڑے::IX_RESERVED1_وتھ];

        إذا (trieType == Dictionary_انکڑے::TRIE_TYPE_UCHARS) {
            ds->وٹاندرا_Array16(ds, وچ_بائٹاں + وتھ, اگلا_وتھ - وتھ, باہرلا_بائٹاں + وتھ, p_Error_کوڈ);
        } وإلا إذا (trieType == Dictionary_انکڑے::TRIE_TYPE_بائٹاں) {
            // nothing to do
        } وإلا {
            udata_printError(ds, "udict_swap(): unknown trie type!\n");
            *p_Error_کوڈ = U_UNSUPPORTED_ERROR;
            أرجع 0;
        }

        // these next two sections are empty in the current format,
        // but may be used later.
        وتھ = اگلا_وتھ;
        اگلا_وتھ = indexes[Dictionary_انکڑے::IX_RESERVED2_وتھ];
        وتھ = اگلا_وتھ;
        اگلا_وتھ = indexes[Dictionary_انکڑے::IX_کل_ناپ];
        وتھ = اگلا_وتھ;
    }
    أرجع سرخی_ناپ + size;
}
#جے_انت
