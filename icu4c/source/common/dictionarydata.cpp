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

#ਅਨ੍ਤਰ੍ਭਾਵ "dictionarydata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ucharstrie.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/bytestrie.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/udata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"

#ਜੇ !UCONFIG_NO_BREAK_ITERATION

U_NAMESPACE_BEGIN

ਅਚਲ int32_t  Dictionary_ਅੰਕੜੇ::TRIE_TYPE_ਬਾਈਟਾਂ = 0;
ਅਚਲ int32_t  Dictionary_ਅੰਕੜੇ::TRIE_TYPE_UCHARS = 1;
ਅਚਲ int32_t  Dictionary_ਅੰਕੜੇ::TRIE_TYPE_ਨਕਾਬ = 7;
ਅਚਲ int32_t  Dictionary_ਅੰਕੜੇ::TRIE_HAS_ਮੁੱਲਾਂ = 8;

ਅਚਲ int32_t  Dictionary_ਅੰਕੜੇ::TRANSFORM_NONE = 0;
ਅਚਲ int32_t  Dictionary_ਅੰਕੜੇ::TRANSFORM_TYPE_ਵਿੱਥ = 0x1000000;
ਅਚਲ int32_t  Dictionary_ਅੰਕੜੇ::TRANSFORM_TYPE_ਨਕਾਬ = 0x7f000000;
ਅਚਲ int32_t  Dictionary_ਅੰਕੜੇ::TRANSFORM_ਵਿੱਥ_ਨਕਾਬ = 0x1fffff;
    
DictionaryMatcher::~DictionaryMatcher() {
}

UCharsDictionaryMatcher::~UCharsDictionaryMatcher() {
    udata_close(file);
}

int32_t UCharsDictionaryMatcher::getType() ਅਚਲ {
    ਵਾਪਸ Dictionary_ਅੰਕੜੇ::TRIE_TYPE_UCHARS;
}

int32_t UCharsDictionaryMatcher::matches(UText *ਪਾਠ, int32_t max_ਲੰਮਾਈ_2, int32_t ਹੱਦਬੰਦੀ,
                            int32_t *lengths, int32_t *cpLengths, int32_t *ਮੁੱਲਾਂ,
                            int32_t *ਅਗੇਤਰ) ਅਚਲ {

    UCharsTrie uct(characters);
    int32_t starting_ਪਾਠ_ਸੂਚਕਾਂਕ = static_cast<int32_t>(utext_get_Native_ਸੂਚਕਾਂਕ(ਪਾਠ));
    int32_t ਸ਼ਬਦ_ਗਿਣਤੀ = 0;
    int32_t ਕੋਡ_Points_Matched = 0;

    ਲਈ (UChar32 c = utext_next32(ਪਾਠ); c >= 0; c=utext_next32(ਪਾਠ)) {
        UString_Trie_ਨਤੀਜਾ ਨਤੀਜਾ = (ਕੋਡ_Points_Matched == 0) ? uct.first(c) : uct.next(c);
        int32_t ਲੰਮਾਈ_Matched = static_cast<int32_t>(utext_get_Native_ਸੂਚਕਾਂਕ(ਪਾਠ)) - starting_ਪਾਠ_ਸੂਚਕਾਂਕ;
        ਕੋਡ_Points_Matched += 1;
        ਜੇ (USTRINGTRIE_HAS_ਮੁੱਲ(ਨਤੀਜਾ)) {
            ਜੇ (ਸ਼ਬਦ_ਗਿਣਤੀ < ਹੱਦਬੰਦੀ) {
                ਜੇ (ਮੁੱਲਾਂ != ਖਾਲੀ_ਸੂਚਕ) {
                    ਮੁੱਲਾਂ[ਸ਼ਬਦ_ਗਿਣਤੀ] = uct.get_ਮੁੱਲ();
                }
                ਜੇ (lengths != ਖਾਲੀ_ਸੂਚਕ) {
                    lengths[ਸ਼ਬਦ_ਗਿਣਤੀ] = ਲੰਮਾਈ_Matched;
                }
                ਜੇ (cpLengths != ਖਾਲੀ_ਸੂਚਕ) {
                    cpLengths[ਸ਼ਬਦ_ਗਿਣਤੀ] = ਕੋਡ_Points_Matched;
                }
                ++ਸ਼ਬਦ_ਗਿਣਤੀ;
            }
            ਜੇ (ਨਤੀਜਾ == USTRINGTRIE_FINAL_ਮੁੱਲ) {
                ਰੋਕੋ;
            }
        }
        ਨਹੀਂਤਾਂ ਜੇ (ਨਤੀਜਾ == USTRINGTRIE_NO_ਮੇਲ) {
            ਰੋਕੋ;
        }
        ਜੇ (ਲੰਮਾਈ_Matched >= max_ਲੰਮਾਈ_2) {
            ਰੋਕੋ;
        }
    }

    ਜੇ (ਅਗੇਤਰ != ਖਾਲੀ_ਸੂਚਕ) {
        *ਅਗੇਤਰ = ਕੋਡ_Points_Matched;
    }
    ਵਾਪਸ ਸ਼ਬਦ_ਗਿਣਤੀ;
}

ਬਾਈਟਾਂ_Dictionary_Matcher::~ਬਾਈਟਾਂ_Dictionary_Matcher() {
    udata_close(file);
}

UChar32 ਬਾਈਟਾਂ_Dictionary_Matcher::transform(UChar32 c) ਅਚਲ {
    ਜੇ ((transformConstant & Dictionary_ਅੰਕੜੇ::TRANSFORM_TYPE_ਨਕਾਬ) == Dictionary_ਅੰਕੜੇ::TRANSFORM_TYPE_ਵਿੱਥ) {
        ਜੇ (c == 0x200D) {
            ਵਾਪਸ 0xFF;
        } ਨਹੀਂਤਾਂ ਜੇ (c == 0x200C) {
            ਵਾਪਸ 0xFE;
        }
        int32_t delta = c - (transformConstant & Dictionary_ਅੰਕੜੇ::TRANSFORM_ਵਿੱਥ_ਨਕਾਬ);
        ਜੇ (delta < 0 || 0xFD < delta) {
            ਵਾਪਸ U_SENTINEL;
        }
        ਵਾਪਸ static_cast<UChar32>(delta);
    }
    ਵਾਪਸ c;
}

int32_t ਬਾਈਟਾਂ_Dictionary_Matcher::getType() ਅਚਲ {
    ਵਾਪਸ Dictionary_ਅੰਕੜੇ::TRIE_TYPE_ਬਾਈਟਾਂ;
}

int32_t ਬਾਈਟਾਂ_Dictionary_Matcher::matches(UText *ਪਾਠ, int32_t max_ਲੰਮਾਈ_2, int32_t ਹੱਦਬੰਦੀ,
                            int32_t *lengths, int32_t *cpLengths, int32_t *ਮੁੱਲਾਂ,
                            int32_t *ਅਗੇਤਰ) ਅਚਲ {
    ਬਾਈਟਾਂ_Trie bt(characters);
    int32_t starting_ਪਾਠ_ਸੂਚਕਾਂਕ = static_cast<int32_t>(utext_get_Native_ਸੂਚਕਾਂਕ(ਪਾਠ));
    int32_t ਸ਼ਬਦ_ਗਿਣਤੀ = 0;
    int32_t ਕੋਡ_Points_Matched = 0;

    ਲਈ (UChar32 c = utext_next32(ਪਾਠ); c >= 0; c=utext_next32(ਪਾਠ)) {
        UString_Trie_ਨਤੀਜਾ ਨਤੀਜਾ = (ਕੋਡ_Points_Matched == 0) ? bt.first(transform(c)) : bt.next(transform(c));
        int32_t ਲੰਮਾਈ_Matched = static_cast<int32_t>(utext_get_Native_ਸੂਚਕਾਂਕ(ਪਾਠ)) - starting_ਪਾਠ_ਸੂਚਕਾਂਕ;
        ਕੋਡ_Points_Matched += 1;
        ਜੇ (USTRINGTRIE_HAS_ਮੁੱਲ(ਨਤੀਜਾ)) {
            ਜੇ (ਸ਼ਬਦ_ਗਿਣਤੀ < ਹੱਦਬੰਦੀ) {
                ਜੇ (ਮੁੱਲਾਂ != ਖਾਲੀ_ਸੂਚਕ) {
                    ਮੁੱਲਾਂ[ਸ਼ਬਦ_ਗਿਣਤੀ] = bt.get_ਮੁੱਲ();
                }
                ਜੇ (lengths != ਖਾਲੀ_ਸੂਚਕ) {
                    lengths[ਸ਼ਬਦ_ਗਿਣਤੀ] = ਲੰਮਾਈ_Matched;
                }
                ਜੇ (cpLengths != ਖਾਲੀ_ਸੂਚਕ) {
                    cpLengths[ਸ਼ਬਦ_ਗਿਣਤੀ] = ਕੋਡ_Points_Matched;
                }
                ++ਸ਼ਬਦ_ਗਿਣਤੀ;
            }
            ਜੇ (ਨਤੀਜਾ == USTRINGTRIE_FINAL_ਮੁੱਲ) {
                ਰੋਕੋ;
            }
        }
        ਨਹੀਂਤਾਂ ਜੇ (ਨਤੀਜਾ == USTRINGTRIE_NO_ਮੇਲ) {
            ਰੋਕੋ;
        }
        ਜੇ (ਲੰਮਾਈ_Matched >= max_ਲੰਮਾਈ_2) {
            ਰੋਕੋ;
        }
    }

    ਜੇ (ਅਗੇਤਰ != ਖਾਲੀ_ਸੂਚਕ) {
        *ਅਗੇਤਰ = ਕੋਡ_Points_Matched;
    }
    ਵਾਪਸ ਸ਼ਬਦ_ਗਿਣਤੀ;
}


U_NAMESPACE_ਸਿਰਾ

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
udict_ਅਦਲਾ_ਬਦਲੀ(ਅਚਲ UDataSwapper *ds, ਅਚਲ ਖਾਲੀ *ਵਿੱਚ_ਅੰਕੜੇ, int32_t length,
           ਖਾਲੀ *ਬਾਹਰਲਾ_ਅੰਕੜੇ, UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਅਚਲ UDataInfo *pInfo;
    int32_t ਸਿਰਲੇਖ_ਮਾਪ;
    ਅਚਲ uint8_t *ਵਿੱਚ_ਬਾਈਟਾਂ;
    uint8_t *ਬਾਹਰਲਾ_ਬਾਈਟਾਂ;
    ਅਚਲ int32_t *ਵਿੱਚ_Indexes;
    int32_t indexes[Dictionary_ਅੰਕੜੇ::IX_ਗਿਣਤੀ];
    int32_t i, ਵਿੱਥ, size;

    ਸਿਰਲੇਖ_ਮਾਪ = udata_ਅਦਲਾ_ਬਦਲੀ_ਅੰਕੜੇ_ਸਿਰਲੇਖ(ds, ਵਿੱਚ_ਅੰਕੜੇ, length, ਬਾਹਰਲਾ_ਅੰਕੜੇ, p_Error_ਕੋਡ);
    ਜੇ (p_Error_ਕੋਡ == ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*p_Error_ਕੋਡ)) ਵਾਪਸ 0;
    pInfo = (ਅਚਲ UDataInfo *)((ਅਚਲ ਅੱਖਰ *)ਵਿੱਚ_ਅੰਕੜੇ + 4);
    ਜੇ (!(pInfo->ਅੰਕੜੇ_Format[0] == 0x44 && 
          pInfo->ਅੰਕੜੇ_Format[1] == 0x69 && 
          pInfo->ਅੰਕੜੇ_Format[2] == 0x63 && 
          pInfo->ਅੰਕੜੇ_Format[3] == 0x74 && 
          pInfo->format_ਸੰਸਕਰਣ[0] == 1)) {
        udata_printError(ds, "udict_swap(): data format %02x.%02x.%02x.%02x (format version %02x) is not recognized as dictionary data\n",
                         pInfo->ਅੰਕੜੇ_Format[0], pInfo->ਅੰਕੜੇ_Format[1], pInfo->ਅੰਕੜੇ_Format[2], pInfo->ਅੰਕੜੇ_Format[3], pInfo->format_ਸੰਸਕਰਣ[0]);
        *p_Error_ਕੋਡ = U_UNSUPPORTED_ERROR;
        ਵਾਪਸ 0;
    }

    ਵਿੱਚ_ਬਾਈਟਾਂ = (ਅਚਲ uint8_t *)ਵਿੱਚ_ਅੰਕੜੇ + ਸਿਰਲੇਖ_ਮਾਪ;
    ਬਾਹਰਲਾ_ਬਾਈਟਾਂ = (ਬਾਹਰਲਾ_ਅੰਕੜੇ == ਖਾਲੀ_ਸੂਚਕ) ? ਖਾਲੀ_ਸੂਚਕ : (uint8_t *)ਬਾਹਰਲਾ_ਅੰਕੜੇ + ਸਿਰਲੇਖ_ਮਾਪ;

    ਵਿੱਚ_Indexes = (ਅਚਲ int32_t *)ਵਿੱਚ_ਬਾਈਟਾਂ;
    ਜੇ (length >= 0) {
        length -= ਸਿਰਲੇਖ_ਮਾਪ;
        ਜੇ (length < (int32_t)(ਆਕਾਰ(indexes))) {
            udata_printError(ds, "udict_swap(): too few bytes (%d after header) for dictionary data\n", length);
            *p_Error_ਕੋਡ = U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
            ਵਾਪਸ 0;
        }
    }

    ਲਈ (i = 0; i < Dictionary_ਅੰਕੜੇ::IX_ਗਿਣਤੀ; i++) {
        indexes[i] = udata_readInt32(ds, ਵਿੱਚ_Indexes[i]);
    }

    size = indexes[Dictionary_ਅੰਕੜੇ::IX_ਕੁੱਲ_ਮਾਪ];

    ਜੇ (length >= 0) {
        ਜੇ (length < size) {
            udata_printError(ds, "udict_swap(): too few bytes (%d after header) for all of dictionary data\n", length);
            *p_Error_ਕੋਡ = U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
            ਵਾਪਸ 0;
        }

        ਜੇ (ਵਿੱਚ_ਬਾਈਟਾਂ != ਬਾਹਰਲਾ_ਬਾਈਟਾਂ) {
            uprv_memcpy(ਬਾਹਰਲਾ_ਬਾਈਟਾਂ, ਵਿੱਚ_ਬਾਈਟਾਂ, size);
        }

        ਵਿੱਥ = 0;
        ds->ਅਦਲਾ_ਬਦਲੀ_Array32(ds, ਵਿੱਚ_ਬਾਈਟਾਂ, ਆਕਾਰ(indexes), ਬਾਹਰਲਾ_ਬਾਈਟਾਂ, p_Error_ਕੋਡ);
        ਵਿੱਥ = (int32_t)ਆਕਾਰ(indexes);
        int32_t trieType = indexes[Dictionary_ਅੰਕੜੇ::IX_TRIE_TYPE] & Dictionary_ਅੰਕੜੇ::TRIE_TYPE_ਨਕਾਬ;
        int32_t ਅਗਲਾ_ਵਿੱਥ = indexes[Dictionary_ਅੰਕੜੇ::IX_RESERVED1_ਵਿੱਥ];

        ਜੇ (trieType == Dictionary_ਅੰਕੜੇ::TRIE_TYPE_UCHARS) {
            ds->ਅਦਲਾ_ਬਦਲੀ_Array16(ds, ਵਿੱਚ_ਬਾਈਟਾਂ + ਵਿੱਥ, ਅਗਲਾ_ਵਿੱਥ - ਵਿੱਥ, ਬਾਹਰਲਾ_ਬਾਈਟਾਂ + ਵਿੱਥ, p_Error_ਕੋਡ);
        } ਨਹੀਂਤਾਂ ਜੇ (trieType == Dictionary_ਅੰਕੜੇ::TRIE_TYPE_ਬਾਈਟਾਂ) {
            // nothing to do
        } ਨਹੀਂਤਾਂ {
            udata_printError(ds, "udict_swap(): unknown trie type!\n");
            *p_Error_ਕੋਡ = U_UNSUPPORTED_ERROR;
            ਵਾਪਸ 0;
        }

        // these next two sections are empty in the current format,
        // but may be used later.
        ਵਿੱਥ = ਅਗਲਾ_ਵਿੱਥ;
        ਅਗਲਾ_ਵਿੱਥ = indexes[Dictionary_ਅੰਕੜੇ::IX_RESERVED2_ਵਿੱਥ];
        ਵਿੱਥ = ਅਗਲਾ_ਵਿੱਥ;
        ਅਗਲਾ_ਵਿੱਥ = indexes[Dictionary_ਅੰਕੜੇ::IX_ਕੁੱਲ_ਮਾਪ];
        ਵਿੱਥ = ਅਗਲਾ_ਵਿੱਥ;
    }
    ਵਾਪਸ ਸਿਰਲੇਖ_ਮਾਪ + size;
}
#ਜੇ_ਅੰਤ
