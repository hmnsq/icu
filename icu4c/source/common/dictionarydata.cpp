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

ثابت int32_t  Dictionary_معلومات::TRIE_TYPE_بایټونه = 0;
ثابت int32_t  Dictionary_معلومات::TRIE_TYPE_UCHARS = 1;
ثابت int32_t  Dictionary_معلومات::TRIE_TYPE_ماسک = 7;
ثابت int32_t  Dictionary_معلومات::TRIE_HAS_ارزښتونه = 8;

ثابت int32_t  Dictionary_معلومات::TRANSFORM_NONE = 0;
ثابت int32_t  Dictionary_معلومات::TRANSFORM_TYPE_دننه_واټن = 0x1000000;
ثابت int32_t  Dictionary_معلومات::TRANSFORM_TYPE_ماسک = 0x7f000000;
ثابت int32_t  Dictionary_معلومات::TRANSFORM_دننه_واټن_ماسک = 0x1fffff;
    
DictionaryMatcher::~DictionaryMatcher() {
}

UCharsDictionaryMatcher::~UCharsDictionaryMatcher() {
    udata_close(file);
}

int32_t UCharsDictionaryMatcher::getType() ثابت {
    أرجع Dictionary_معلومات::TRIE_TYPE_UCHARS;
}

int32_t UCharsDictionaryMatcher::matches(UText *متن, int32_t max_اوږدتیا_2, int32_t حد,
                            int32_t *lengths, int32_t *cpLengths, int32_t *ارزښتونه,
                            int32_t *مختاړی) ثابت {

    UCharsTrie uct(characters);
    int32_t starting_متن_شاخص = static_cast<int32_t>(utext_get_Native_شاخص(متن));
    int32_t کلمه_شمېر = 0;
    int32_t کوډ_Points_Matched = 0;

    لكل (UChar32 c = utext_next32(متن); c >= 0; c=utext_next32(متن)) {
        UString_Trie_پایله پایله = (کوډ_Points_Matched == 0) ? uct.first(c) : uct.next(c);
        int32_t اوږدتیا_Matched = static_cast<int32_t>(utext_get_Native_شاخص(متن)) - starting_متن_شاخص;
        کوډ_Points_Matched += 1;
        إذا (USTRINGTRIE_HAS_ارزښت(پایله)) {
            إذا (کلمه_شمېر < حد) {
                إذا (ارزښتونه != تش_اشاره) {
                    ارزښتونه[کلمه_شمېر] = uct.get_ارزښت();
                }
                إذا (lengths != تش_اشاره) {
                    lengths[کلمه_شمېر] = اوږدتیا_Matched;
                }
                إذا (cpLengths != تش_اشاره) {
                    cpLengths[کلمه_شمېر] = کوډ_Points_Matched;
                }
                ++کلمه_شمېر;
            }
            إذا (پایله == USTRINGTRIE_FINAL_ارزښت) {
                اكسر;
            }
        }
        وإلا إذا (پایله == USTRINGTRIE_NO_سمون) {
            اكسر;
        }
        إذا (اوږدتیا_Matched >= max_اوږدتیا_2) {
            اكسر;
        }
    }

    إذا (مختاړی != تش_اشاره) {
        *مختاړی = کوډ_Points_Matched;
    }
    أرجع کلمه_شمېر;
}

بایټونه_Dictionary_Matcher::~بایټونه_Dictionary_Matcher() {
    udata_close(file);
}

UChar32 بایټونه_Dictionary_Matcher::transform(UChar32 c) ثابت {
    إذا ((transformConstant & Dictionary_معلومات::TRANSFORM_TYPE_ماسک) == Dictionary_معلومات::TRANSFORM_TYPE_دننه_واټن) {
        إذا (c == 0x200D) {
            أرجع 0xFF;
        } وإلا إذا (c == 0x200C) {
            أرجع 0xFE;
        }
        int32_t delta = c - (transformConstant & Dictionary_معلومات::TRANSFORM_دننه_واټن_ماسک);
        إذا (delta < 0 || 0xFD < delta) {
            أرجع U_SENTINEL;
        }
        أرجع static_cast<UChar32>(delta);
    }
    أرجع c;
}

int32_t بایټونه_Dictionary_Matcher::getType() ثابت {
    أرجع Dictionary_معلومات::TRIE_TYPE_بایټونه;
}

int32_t بایټونه_Dictionary_Matcher::matches(UText *متن, int32_t max_اوږدتیا_2, int32_t حد,
                            int32_t *lengths, int32_t *cpLengths, int32_t *ارزښتونه,
                            int32_t *مختاړی) ثابت {
    بایټونه_Trie bt(characters);
    int32_t starting_متن_شاخص = static_cast<int32_t>(utext_get_Native_شاخص(متن));
    int32_t کلمه_شمېر = 0;
    int32_t کوډ_Points_Matched = 0;

    لكل (UChar32 c = utext_next32(متن); c >= 0; c=utext_next32(متن)) {
        UString_Trie_پایله پایله = (کوډ_Points_Matched == 0) ? bt.first(transform(c)) : bt.next(transform(c));
        int32_t اوږدتیا_Matched = static_cast<int32_t>(utext_get_Native_شاخص(متن)) - starting_متن_شاخص;
        کوډ_Points_Matched += 1;
        إذا (USTRINGTRIE_HAS_ارزښت(پایله)) {
            إذا (کلمه_شمېر < حد) {
                إذا (ارزښتونه != تش_اشاره) {
                    ارزښتونه[کلمه_شمېر] = bt.get_ارزښت();
                }
                إذا (lengths != تش_اشاره) {
                    lengths[کلمه_شمېر] = اوږدتیا_Matched;
                }
                إذا (cpLengths != تش_اشاره) {
                    cpLengths[کلمه_شمېر] = کوډ_Points_Matched;
                }
                ++کلمه_شمېر;
            }
            إذا (پایله == USTRINGTRIE_FINAL_ارزښت) {
                اكسر;
            }
        }
        وإلا إذا (پایله == USTRINGTRIE_NO_سمون) {
            اكسر;
        }
        إذا (اوږدتیا_Matched >= max_اوږدتیا_2) {
            اكسر;
        }
    }

    إذا (مختاړی != تش_اشاره) {
        *مختاړی = کوډ_Points_Matched;
    }
    أرجع کلمه_شمېر;
}


U_NAMESPACE_پای_ټکی

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
udict_بدلون(ثابت UDataSwapper *ds, ثابت فراغ *دننه_معلومات, int32_t length,
           فراغ *بهر_معلومات, UError_کوډ *p_Error_کوډ) {
    ثابت UDataInfo *pInfo;
    int32_t سرلیک_اندازه;
    ثابت uint8_t *دننه_بایټونه;
    uint8_t *بهر_بایټونه;
    ثابت int32_t *دننه_Indexes;
    int32_t indexes[Dictionary_معلومات::IX_شمېر];
    int32_t i, دننه_واټن, size;

    سرلیک_اندازه = udata_بدلون_معلومات_سرلیک(ds, دننه_معلومات, length, بهر_معلومات, p_Error_کوډ);
    إذا (p_Error_کوډ == تش_اشاره || U_FAILURE(*p_Error_کوډ)) أرجع 0;
    pInfo = (ثابت UDataInfo *)((ثابت حرف *)دننه_معلومات + 4);
    إذا (!(pInfo->معلومات_Format[0] == 0x44 && 
          pInfo->معلومات_Format[1] == 0x69 && 
          pInfo->معلومات_Format[2] == 0x63 && 
          pInfo->معلومات_Format[3] == 0x74 && 
          pInfo->format_نسخه[0] == 1)) {
        udata_printError(ds, "udict_swap(): data format %02x.%02x.%02x.%02x (format version %02x) is not recognized as dictionary data\n",
                         pInfo->معلومات_Format[0], pInfo->معلومات_Format[1], pInfo->معلومات_Format[2], pInfo->معلومات_Format[3], pInfo->format_نسخه[0]);
        *p_Error_کوډ = U_UNSUPPORTED_ERROR;
        أرجع 0;
    }

    دننه_بایټونه = (ثابت uint8_t *)دننه_معلومات + سرلیک_اندازه;
    بهر_بایټونه = (بهر_معلومات == تش_اشاره) ? تش_اشاره : (uint8_t *)بهر_معلومات + سرلیک_اندازه;

    دننه_Indexes = (ثابت int32_t *)دننه_بایټونه;
    إذا (length >= 0) {
        length -= سرلیک_اندازه;
        إذا (length < (int32_t)(حجم(indexes))) {
            udata_printError(ds, "udict_swap(): too few bytes (%d after header) for dictionary data\n", length);
            *p_Error_کوډ = U_شاخص_OUTOFBOUNDS_ERROR;
            أرجع 0;
        }
    }

    لكل (i = 0; i < Dictionary_معلومات::IX_شمېر; i++) {
        indexes[i] = udata_readInt32(ds, دننه_Indexes[i]);
    }

    size = indexes[Dictionary_معلومات::IX_ټول_اندازه];

    إذا (length >= 0) {
        إذا (length < size) {
            udata_printError(ds, "udict_swap(): too few bytes (%d after header) for all of dictionary data\n", length);
            *p_Error_کوډ = U_شاخص_OUTOFBOUNDS_ERROR;
            أرجع 0;
        }

        إذا (دننه_بایټونه != بهر_بایټونه) {
            uprv_memcpy(بهر_بایټونه, دننه_بایټونه, size);
        }

        دننه_واټن = 0;
        ds->بدلون_Array32(ds, دننه_بایټونه, حجم(indexes), بهر_بایټونه, p_Error_کوډ);
        دننه_واټن = (int32_t)حجم(indexes);
        int32_t trieType = indexes[Dictionary_معلومات::IX_TRIE_TYPE] & Dictionary_معلومات::TRIE_TYPE_ماسک;
        int32_t بل_دننه_واټن = indexes[Dictionary_معلومات::IX_RESERVED1_دننه_واټن];

        إذا (trieType == Dictionary_معلومات::TRIE_TYPE_UCHARS) {
            ds->بدلون_Array16(ds, دننه_بایټونه + دننه_واټن, بل_دننه_واټن - دننه_واټن, بهر_بایټونه + دننه_واټن, p_Error_کوډ);
        } وإلا إذا (trieType == Dictionary_معلومات::TRIE_TYPE_بایټونه) {
            // nothing to do
        } وإلا {
            udata_printError(ds, "udict_swap(): unknown trie type!\n");
            *p_Error_کوډ = U_UNSUPPORTED_ERROR;
            أرجع 0;
        }

        // these next two sections are empty in the current format,
        // but may be used later.
        دننه_واټن = بل_دننه_واټن;
        بل_دننه_واټن = indexes[Dictionary_معلومات::IX_RESERVED2_دننه_واټن];
        دننه_واټن = بل_دننه_واټن;
        بل_دننه_واټن = indexes[Dictionary_معلومات::IX_ټول_اندازه];
        دننه_واټن = بل_دننه_واټن;
    }
    أرجع سرلیک_اندازه + size;
}
#د_که_پای
