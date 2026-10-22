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

ثابت int32_t  Dictionary_داده::TRIE_TYPE_بایتها = 0;
ثابت int32_t  Dictionary_داده::TRIE_TYPE_UCHARS = 1;
ثابت int32_t  Dictionary_داده::TRIE_TYPE_ماسک = 7;
ثابت int32_t  Dictionary_داده::TRIE_HAS_مقادیر = 8;

ثابت int32_t  Dictionary_داده::TRANSFORM_NONE = 0;
ثابت int32_t  Dictionary_داده::TRANSFORM_TYPE_جابه_جایی = 0x1000000;
ثابت int32_t  Dictionary_داده::TRANSFORM_TYPE_ماسک = 0x7f000000;
ثابت int32_t  Dictionary_داده::TRANSFORM_جابه_جایی_ماسک = 0x1fffff;
    
DictionaryMatcher::~DictionaryMatcher() {
}

UCharsDictionaryMatcher::~UCharsDictionaryMatcher() {
    udata_close(file);
}

int32_t UCharsDictionaryMatcher::getType() ثابت {
    أرجع Dictionary_داده::TRIE_TYPE_UCHARS;
}

int32_t UCharsDictionaryMatcher::matches(UText *متن, int32_t max_درازی_2, int32_t حد,
                            int32_t *lengths, int32_t *cpLengths, int32_t *مقادیر,
                            int32_t *پیشوند) ثابت {

    UCharsTrie uct(characters);
    int32_t starting_متن_شاخص = static_cast<int32_t>(utext_get_Native_شاخص(متن));
    int32_t کلمه_شمار = 0;
    int32_t کد_Points_Matched = 0;

    لكل (UChar32 c = utext_next32(متن); c >= 0; c=utext_next32(متن)) {
        UString_Trie_نتیجه نتیجه = (کد_Points_Matched == 0) ? uct.first(c) : uct.next(c);
        int32_t درازی_Matched = static_cast<int32_t>(utext_get_Native_شاخص(متن)) - starting_متن_شاخص;
        کد_Points_Matched += 1;
        إذا (USTRINGTRIE_HAS_مقدار(نتیجه)) {
            إذا (کلمه_شمار < حد) {
                إذا (مقادیر != اشاره_تهی) {
                    مقادیر[کلمه_شمار] = uct.get_مقدار();
                }
                إذا (lengths != اشاره_تهی) {
                    lengths[کلمه_شمار] = درازی_Matched;
                }
                إذا (cpLengths != اشاره_تهی) {
                    cpLengths[کلمه_شمار] = کد_Points_Matched;
                }
                ++کلمه_شمار;
            }
            إذا (نتیجه == USTRINGTRIE_FINAL_مقدار) {
                اكسر;
            }
        }
        وإلا إذا (نتیجه == USTRINGTRIE_NO_تطابق) {
            اكسر;
        }
        إذا (درازی_Matched >= max_درازی_2) {
            اكسر;
        }
    }

    إذا (پیشوند != اشاره_تهی) {
        *پیشوند = کد_Points_Matched;
    }
    أرجع کلمه_شمار;
}

بایتها_Dictionary_Matcher::~بایتها_Dictionary_Matcher() {
    udata_close(file);
}

UChar32 بایتها_Dictionary_Matcher::transform(UChar32 c) ثابت {
    إذا ((transformConstant & Dictionary_داده::TRANSFORM_TYPE_ماسک) == Dictionary_داده::TRANSFORM_TYPE_جابه_جایی) {
        إذا (c == 0x200D) {
            أرجع 0xFF;
        } وإلا إذا (c == 0x200C) {
            أرجع 0xFE;
        }
        int32_t delta = c - (transformConstant & Dictionary_داده::TRANSFORM_جابه_جایی_ماسک);
        إذا (delta < 0 || 0xFD < delta) {
            أرجع U_SENTINEL;
        }
        أرجع static_cast<UChar32>(delta);
    }
    أرجع c;
}

int32_t بایتها_Dictionary_Matcher::getType() ثابت {
    أرجع Dictionary_داده::TRIE_TYPE_بایتها;
}

int32_t بایتها_Dictionary_Matcher::matches(UText *متن, int32_t max_درازی_2, int32_t حد,
                            int32_t *lengths, int32_t *cpLengths, int32_t *مقادیر,
                            int32_t *پیشوند) ثابت {
    بایتها_Trie bt(characters);
    int32_t starting_متن_شاخص = static_cast<int32_t>(utext_get_Native_شاخص(متن));
    int32_t کلمه_شمار = 0;
    int32_t کد_Points_Matched = 0;

    لكل (UChar32 c = utext_next32(متن); c >= 0; c=utext_next32(متن)) {
        UString_Trie_نتیجه نتیجه = (کد_Points_Matched == 0) ? bt.first(transform(c)) : bt.next(transform(c));
        int32_t درازی_Matched = static_cast<int32_t>(utext_get_Native_شاخص(متن)) - starting_متن_شاخص;
        کد_Points_Matched += 1;
        إذا (USTRINGTRIE_HAS_مقدار(نتیجه)) {
            إذا (کلمه_شمار < حد) {
                إذا (مقادیر != اشاره_تهی) {
                    مقادیر[کلمه_شمار] = bt.get_مقدار();
                }
                إذا (lengths != اشاره_تهی) {
                    lengths[کلمه_شمار] = درازی_Matched;
                }
                إذا (cpLengths != اشاره_تهی) {
                    cpLengths[کلمه_شمار] = کد_Points_Matched;
                }
                ++کلمه_شمار;
            }
            إذا (نتیجه == USTRINGTRIE_FINAL_مقدار) {
                اكسر;
            }
        }
        وإلا إذا (نتیجه == USTRINGTRIE_NO_تطابق) {
            اكسر;
        }
        إذا (درازی_Matched >= max_درازی_2) {
            اكسر;
        }
    }

    إذا (پیشوند != اشاره_تهی) {
        *پیشوند = کد_Points_Matched;
    }
    أرجع کلمه_شمار;
}


U_NAMESPACE_انتها

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
udict_جابجا(ثابت UDataSwapper *ds, ثابت فراغ *درون_داده, int32_t length,
           فراغ *بیرون_داده, UError_کد *p_Error_کد) {
    ثابت UDataInfo *pInfo;
    int32_t سرآیند_اندازه;
    ثابت uint8_t *درون_بایتها;
    uint8_t *بیرون_بایتها;
    ثابت int32_t *درون_Indexes;
    int32_t indexes[Dictionary_داده::IX_شمار];
    int32_t i, جابه_جایی, size;

    سرآیند_اندازه = udata_جابجا_داده_سرآیند(ds, درون_داده, length, بیرون_داده, p_Error_کد);
    إذا (p_Error_کد == اشاره_تهی || U_FAILURE(*p_Error_کد)) أرجع 0;
    pInfo = (ثابت UDataInfo *)((ثابت حرف *)درون_داده + 4);
    إذا (!(pInfo->داده_Format[0] == 0x44 && 
          pInfo->داده_Format[1] == 0x69 && 
          pInfo->داده_Format[2] == 0x63 && 
          pInfo->داده_Format[3] == 0x74 && 
          pInfo->format_نسخه[0] == 1)) {
        udata_printError(ds, "udict_swap(): data format %02x.%02x.%02x.%02x (format version %02x) is not recognized as dictionary data\n",
                         pInfo->داده_Format[0], pInfo->داده_Format[1], pInfo->داده_Format[2], pInfo->داده_Format[3], pInfo->format_نسخه[0]);
        *p_Error_کد = U_UNSUPPORTED_ERROR;
        أرجع 0;
    }

    درون_بایتها = (ثابت uint8_t *)درون_داده + سرآیند_اندازه;
    بیرون_بایتها = (بیرون_داده == اشاره_تهی) ? اشاره_تهی : (uint8_t *)بیرون_داده + سرآیند_اندازه;

    درون_Indexes = (ثابت int32_t *)درون_بایتها;
    إذا (length >= 0) {
        length -= سرآیند_اندازه;
        إذا (length < (int32_t)(حجم(indexes))) {
            udata_printError(ds, "udict_swap(): too few bytes (%d after header) for dictionary data\n", length);
            *p_Error_کد = U_شاخص_OUTOFBOUNDS_ERROR;
            أرجع 0;
        }
    }

    لكل (i = 0; i < Dictionary_داده::IX_شمار; i++) {
        indexes[i] = udata_readInt32(ds, درون_Indexes[i]);
    }

    size = indexes[Dictionary_داده::IX_مجموع_اندازه];

    إذا (length >= 0) {
        إذا (length < size) {
            udata_printError(ds, "udict_swap(): too few bytes (%d after header) for all of dictionary data\n", length);
            *p_Error_کد = U_شاخص_OUTOFBOUNDS_ERROR;
            أرجع 0;
        }

        إذا (درون_بایتها != بیرون_بایتها) {
            uprv_memcpy(بیرون_بایتها, درون_بایتها, size);
        }

        جابه_جایی = 0;
        ds->جابجا_Array32(ds, درون_بایتها, حجم(indexes), بیرون_بایتها, p_Error_کد);
        جابه_جایی = (int32_t)حجم(indexes);
        int32_t trieType = indexes[Dictionary_داده::IX_TRIE_TYPE] & Dictionary_داده::TRIE_TYPE_ماسک;
        int32_t بعدی_جابه_جایی = indexes[Dictionary_داده::IX_RESERVED1_جابه_جایی];

        إذا (trieType == Dictionary_داده::TRIE_TYPE_UCHARS) {
            ds->جابجا_Array16(ds, درون_بایتها + جابه_جایی, بعدی_جابه_جایی - جابه_جایی, بیرون_بایتها + جابه_جایی, p_Error_کد);
        } وإلا إذا (trieType == Dictionary_داده::TRIE_TYPE_بایتها) {
            // nothing to do
        } وإلا {
            udata_printError(ds, "udict_swap(): unknown trie type!\n");
            *p_Error_کد = U_UNSUPPORTED_ERROR;
            أرجع 0;
        }

        // these next two sections are empty in the current format,
        // but may be used later.
        جابه_جایی = بعدی_جابه_جایی;
        بعدی_جابه_جایی = indexes[Dictionary_داده::IX_RESERVED2_جابه_جایی];
        جابه_جایی = بعدی_جابه_جایی;
        بعدی_جابه_جایی = indexes[Dictionary_داده::IX_مجموع_اندازه];
        جابه_جایی = بعدی_جابه_جایی;
    }
    أرجع سرآیند_اندازه + size;
}
#پایان_اگر
