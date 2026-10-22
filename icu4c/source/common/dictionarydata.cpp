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

#شامل "dictionarydata.h"
#شامل "unicode/ucharstrie.h"
#شامل "unicode/bytestrie.h"
#شامل "unicode/udata.h"
#شامل "cmemory.h"

#اگر !UCONFIG_NO_BREAK_ITERATION

U_NAMESPACE_BEGIN

ثابت int32_t  Dictionary_ڈیٹا::TRIE_TYPE_بائٹیں = 0;
ثابت int32_t  Dictionary_ڈیٹا::TRIE_TYPE_UCHARS = 1;
ثابت int32_t  Dictionary_ڈیٹا::TRIE_TYPE_نقاب = 7;
ثابت int32_t  Dictionary_ڈیٹا::TRIE_HAS_اقدار = 8;

ثابت int32_t  Dictionary_ڈیٹا::TRANSFORM_NONE = 0;
ثابت int32_t  Dictionary_ڈیٹا::TRANSFORM_TYPE_انحراف = 0x1000000;
ثابت int32_t  Dictionary_ڈیٹا::TRANSFORM_TYPE_نقاب = 0x7f000000;
ثابت int32_t  Dictionary_ڈیٹا::TRANSFORM_انحراف_نقاب = 0x1fffff;
    
DictionaryMatcher::~DictionaryMatcher() {
}

UCharsDictionaryMatcher::~UCharsDictionaryMatcher() {
    udata_close(file);
}

int32_t UCharsDictionaryMatcher::getType() ثابت {
    واپس Dictionary_ڈیٹا::TRIE_TYPE_UCHARS;
}

int32_t UCharsDictionaryMatcher::matches(UText *متن, int32_t max_لمبائی_2, int32_t حد_بندی,
                            int32_t *lengths, int32_t *cpLengths, int32_t *اقدار,
                            int32_t *سابقہ) ثابت {

    UCharsTrie uct(characters);
    int32_t starting_متن_اشاریہ = static_cast<int32_t>(utext_get_Native_اشاریہ(متن));
    int32_t لفظ_گنتی = 0;
    int32_t رمز_Points_Matched = 0;

    برائے (UChar32 c = utext_next32(متن); c >= 0; c=utext_next32(متن)) {
        UString_Trie_نتیجہ نتیجہ = (رمز_Points_Matched == 0) ? uct.first(c) : uct.next(c);
        int32_t لمبائی_Matched = static_cast<int32_t>(utext_get_Native_اشاریہ(متن)) - starting_متن_اشاریہ;
        رمز_Points_Matched += 1;
        اگر (USTRINGTRIE_HAS_قدر(نتیجہ)) {
            اگر (لفظ_گنتی < حد_بندی) {
                اگر (اقدار != خالی_اشاریہ) {
                    اقدار[لفظ_گنتی] = uct.get_قدر();
                }
                اگر (lengths != خالی_اشاریہ) {
                    lengths[لفظ_گنتی] = لمبائی_Matched;
                }
                اگر (cpLengths != خالی_اشاریہ) {
                    cpLengths[لفظ_گنتی] = رمز_Points_Matched;
                }
                ++لفظ_گنتی;
            }
            اگر (نتیجہ == USTRINGTRIE_FINAL_قدر) {
                توڑو;
            }
        }
        ورنہ اگر (نتیجہ == USTRINGTRIE_NO_مطابقت) {
            توڑو;
        }
        اگر (لمبائی_Matched >= max_لمبائی_2) {
            توڑو;
        }
    }

    اگر (سابقہ != خالی_اشاریہ) {
        *سابقہ = رمز_Points_Matched;
    }
    واپس لفظ_گنتی;
}

بائٹیں_Dictionary_Matcher::~بائٹیں_Dictionary_Matcher() {
    udata_close(file);
}

UChar32 بائٹیں_Dictionary_Matcher::transform(UChar32 c) ثابت {
    اگر ((transformConstant & Dictionary_ڈیٹا::TRANSFORM_TYPE_نقاب) == Dictionary_ڈیٹا::TRANSFORM_TYPE_انحراف) {
        اگر (c == 0x200D) {
            واپس 0xFF;
        } ورنہ اگر (c == 0x200C) {
            واپس 0xFE;
        }
        int32_t delta = c - (transformConstant & Dictionary_ڈیٹا::TRANSFORM_انحراف_نقاب);
        اگر (delta < 0 || 0xFD < delta) {
            واپس U_SENTINEL;
        }
        واپس static_cast<UChar32>(delta);
    }
    واپس c;
}

int32_t بائٹیں_Dictionary_Matcher::getType() ثابت {
    واپس Dictionary_ڈیٹا::TRIE_TYPE_بائٹیں;
}

int32_t بائٹیں_Dictionary_Matcher::matches(UText *متن, int32_t max_لمبائی_2, int32_t حد_بندی,
                            int32_t *lengths, int32_t *cpLengths, int32_t *اقدار,
                            int32_t *سابقہ) ثابت {
    بائٹیں_Trie bt(characters);
    int32_t starting_متن_اشاریہ = static_cast<int32_t>(utext_get_Native_اشاریہ(متن));
    int32_t لفظ_گنتی = 0;
    int32_t رمز_Points_Matched = 0;

    برائے (UChar32 c = utext_next32(متن); c >= 0; c=utext_next32(متن)) {
        UString_Trie_نتیجہ نتیجہ = (رمز_Points_Matched == 0) ? bt.first(transform(c)) : bt.next(transform(c));
        int32_t لمبائی_Matched = static_cast<int32_t>(utext_get_Native_اشاریہ(متن)) - starting_متن_اشاریہ;
        رمز_Points_Matched += 1;
        اگر (USTRINGTRIE_HAS_قدر(نتیجہ)) {
            اگر (لفظ_گنتی < حد_بندی) {
                اگر (اقدار != خالی_اشاریہ) {
                    اقدار[لفظ_گنتی] = bt.get_قدر();
                }
                اگر (lengths != خالی_اشاریہ) {
                    lengths[لفظ_گنتی] = لمبائی_Matched;
                }
                اگر (cpLengths != خالی_اشاریہ) {
                    cpLengths[لفظ_گنتی] = رمز_Points_Matched;
                }
                ++لفظ_گنتی;
            }
            اگر (نتیجہ == USTRINGTRIE_FINAL_قدر) {
                توڑو;
            }
        }
        ورنہ اگر (نتیجہ == USTRINGTRIE_NO_مطابقت) {
            توڑو;
        }
        اگر (لمبائی_Matched >= max_لمبائی_2) {
            توڑو;
        }
    }

    اگر (سابقہ != خالی_اشاریہ) {
        *سابقہ = رمز_Points_Matched;
    }
    واپس لفظ_گنتی;
}


U_NAMESPACE_انتہا

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
udict_تبادلہ(ثابت UDataSwapper *ds, ثابت خالی *اندر_ڈیٹا, int32_t length,
           خالی *برآمد_ڈیٹا, UError_رمز *p_Error_رمز) {
    ثابت UDataInfo *pInfo;
    int32_t سرخی_سائز;
    ثابت uint8_t *اندر_بائٹیں;
    uint8_t *برآمد_بائٹیں;
    ثابت int32_t *اندر_Indexes;
    int32_t indexes[Dictionary_ڈیٹا::IX_گنتی];
    int32_t i, انحراف, size;

    سرخی_سائز = udata_تبادلہ_ڈیٹا_سرخی(ds, اندر_ڈیٹا, length, برآمد_ڈیٹا, p_Error_رمز);
    اگر (p_Error_رمز == خالی_اشاریہ || U_FAILURE(*p_Error_رمز)) واپس 0;
    pInfo = (ثابت UDataInfo *)((ثابت حرف *)اندر_ڈیٹا + 4);
    اگر (!(pInfo->ڈیٹا_Format[0] == 0x44 && 
          pInfo->ڈیٹا_Format[1] == 0x69 && 
          pInfo->ڈیٹا_Format[2] == 0x63 && 
          pInfo->ڈیٹا_Format[3] == 0x74 && 
          pInfo->format_نسخہ[0] == 1)) {
        udata_printError(ds, "udict_swap(): data format %02x.%02x.%02x.%02x (format version %02x) is not recognized as dictionary data\n",
                         pInfo->ڈیٹا_Format[0], pInfo->ڈیٹا_Format[1], pInfo->ڈیٹا_Format[2], pInfo->ڈیٹا_Format[3], pInfo->format_نسخہ[0]);
        *p_Error_رمز = U_UNSUPPORTED_ERROR;
        واپس 0;
    }

    اندر_بائٹیں = (ثابت uint8_t *)اندر_ڈیٹا + سرخی_سائز;
    برآمد_بائٹیں = (برآمد_ڈیٹا == خالی_اشاریہ) ? خالی_اشاریہ : (uint8_t *)برآمد_ڈیٹا + سرخی_سائز;

    اندر_Indexes = (ثابت int32_t *)اندر_بائٹیں;
    اگر (length >= 0) {
        length -= سرخی_سائز;
        اگر (length < (int32_t)(حجم(indexes))) {
            udata_printError(ds, "udict_swap(): too few bytes (%d after header) for dictionary data\n", length);
            *p_Error_رمز = U_اشاریہ_OUTOFBOUNDS_ERROR;
            واپس 0;
        }
    }

    برائے (i = 0; i < Dictionary_ڈیٹا::IX_گنتی; i++) {
        indexes[i] = udata_readInt32(ds, اندر_Indexes[i]);
    }

    size = indexes[Dictionary_ڈیٹا::IX_کل_سائز];

    اگر (length >= 0) {
        اگر (length < size) {
            udata_printError(ds, "udict_swap(): too few bytes (%d after header) for all of dictionary data\n", length);
            *p_Error_رمز = U_اشاریہ_OUTOFBOUNDS_ERROR;
            واپس 0;
        }

        اگر (اندر_بائٹیں != برآمد_بائٹیں) {
            uprv_memcpy(برآمد_بائٹیں, اندر_بائٹیں, size);
        }

        انحراف = 0;
        ds->تبادلہ_Array32(ds, اندر_بائٹیں, حجم(indexes), برآمد_بائٹیں, p_Error_رمز);
        انحراف = (int32_t)حجم(indexes);
        int32_t trieType = indexes[Dictionary_ڈیٹا::IX_TRIE_TYPE] & Dictionary_ڈیٹا::TRIE_TYPE_نقاب;
        int32_t اگلا_انحراف = indexes[Dictionary_ڈیٹا::IX_RESERVED1_انحراف];

        اگر (trieType == Dictionary_ڈیٹا::TRIE_TYPE_UCHARS) {
            ds->تبادلہ_Array16(ds, اندر_بائٹیں + انحراف, اگلا_انحراف - انحراف, برآمد_بائٹیں + انحراف, p_Error_رمز);
        } ورنہ اگر (trieType == Dictionary_ڈیٹا::TRIE_TYPE_بائٹیں) {
            // nothing to do
        } ورنہ {
            udata_printError(ds, "udict_swap(): unknown trie type!\n");
            *p_Error_رمز = U_UNSUPPORTED_ERROR;
            واپس 0;
        }

        // these next two sections are empty in the current format,
        // but may be used later.
        انحراف = اگلا_انحراف;
        اگلا_انحراف = indexes[Dictionary_ڈیٹا::IX_RESERVED2_انحراف];
        انحراف = اگلا_انحراف;
        اگلا_انحراف = indexes[Dictionary_ڈیٹا::IX_کل_سائز];
        انحراف = اگلا_انحراف;
    }
    واپس سرخی_سائز + size;
}
#اگر_ختم
