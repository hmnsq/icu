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

#ಅನ್ತರ್ಭಾವ "dictionarydata.h"
#ಅನ್ತರ್ಭಾವ "unicode/ucharstrie.h"
#ಅನ್ತರ್ಭಾವ "unicode/bytestrie.h"
#ಅನ್ತರ್ಭಾವ "unicode/udata.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"

#ಆದರೆ !UCONFIG_NO_BREAK_ITERATION

U_NAMESPACE_BEGIN

ಸ್ಥಿರಾಂಕ int32_t  Dictionary_ದತ್ತಾಂಶ::TRIE_TYPE_ಬೈಟುಗಳು = 0;
ಸ್ಥಿರಾಂಕ int32_t  Dictionary_ದತ್ತಾಂಶ::TRIE_TYPE_UCHARS = 1;
ಸ್ಥಿರಾಂಕ int32_t  Dictionary_ದತ್ತಾಂಶ::TRIE_TYPE_ಮುಖವಾಡ = 7;
ಸ್ಥಿರಾಂಕ int32_t  Dictionary_ದತ್ತಾಂಶ::TRIE_HAS_ಮೌಲ್ಯಗಳು = 8;

ಸ್ಥಿರಾಂಕ int32_t  Dictionary_ದತ್ತಾಂಶ::TRANSFORM_NONE = 0;
ಸ್ಥಿರಾಂಕ int32_t  Dictionary_ದತ್ತಾಂಶ::TRANSFORM_TYPE_ಸ್ಥಾನಾಂತರ = 0x1000000;
ಸ್ಥಿರಾಂಕ int32_t  Dictionary_ದತ್ತಾಂಶ::TRANSFORM_TYPE_ಮುಖವಾಡ = 0x7f000000;
ಸ್ಥಿರಾಂಕ int32_t  Dictionary_ದತ್ತಾಂಶ::TRANSFORM_ಸ್ಥಾನಾಂತರ_ಮುಖವಾಡ = 0x1fffff;
    
DictionaryMatcher::~DictionaryMatcher() {
}

UCharsDictionaryMatcher::~UCharsDictionaryMatcher() {
    udata_close(file);
}

int32_t UCharsDictionaryMatcher::getType() ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ Dictionary_ದತ್ತಾಂಶ::TRIE_TYPE_UCHARS;
}

int32_t UCharsDictionaryMatcher::matches(UText *ಪಠ್ಯ, int32_t max_ಉದ್ದಳತೆ_2, int32_t ಎಲ್ಲೆ,
                            int32_t *lengths, int32_t *cpLengths, int32_t *ಮೌಲ್ಯಗಳು,
                            int32_t *ಪೂರ್ವಪ್ರತ್ಯಯ) ಸ್ಥಿರಾಂಕ {

    UCharsTrie uct(characters);
    int32_t starting_ಪಠ್ಯ_ಸೂಚ್ಯಂಕ = static_cast<int32_t>(utext_get_Native_ಸೂಚ್ಯಂಕ(ಪಠ್ಯ));
    int32_t ಶಬ್ದ_ಎಣಿಕೆ = 0;
    int32_t ಸಂಕೇತ_Points_Matched = 0;

    ಗಾಗಿ (UChar32 c = utext_next32(ಪಠ್ಯ); c >= 0; c=utext_next32(ಪಠ್ಯ)) {
        UString_Trie_ಫಲಿತಾಂಶ ಫಲಿತಾಂಶ = (ಸಂಕೇತ_Points_Matched == 0) ? uct.first(c) : uct.next(c);
        int32_t ಉದ್ದಳತೆ_Matched = static_cast<int32_t>(utext_get_Native_ಸೂಚ್ಯಂಕ(ಪಠ್ಯ)) - starting_ಪಠ್ಯ_ಸೂಚ್ಯಂಕ;
        ಸಂಕೇತ_Points_Matched += 1;
        ಆದರೆ (USTRINGTRIE_HAS_ಮೌಲ್ಯ(ಫಲಿತಾಂಶ)) {
            ಆದರೆ (ಶಬ್ದ_ಎಣಿಕೆ < ಎಲ್ಲೆ) {
                ಆದರೆ (ಮೌಲ್ಯಗಳು != ಶೂನ್ಯಸೂಚಕ) {
                    ಮೌಲ್ಯಗಳು[ಶಬ್ದ_ಎಣಿಕೆ] = uct.get_ಮೌಲ್ಯ();
                }
                ಆದರೆ (lengths != ಶೂನ್ಯಸೂಚಕ) {
                    lengths[ಶಬ್ದ_ಎಣಿಕೆ] = ಉದ್ದಳತೆ_Matched;
                }
                ಆದರೆ (cpLengths != ಶೂನ್ಯಸೂಚಕ) {
                    cpLengths[ಶಬ್ದ_ಎಣಿಕೆ] = ಸಂಕೇತ_Points_Matched;
                }
                ++ಶಬ್ದ_ಎಣಿಕೆ;
            }
            ಆದರೆ (ಫಲಿತಾಂಶ == USTRINGTRIE_FINAL_ಮೌಲ್ಯ) {
                ನಿಲ್ಲಿಸು;
            }
        }
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಫಲಿತಾಂಶ == USTRINGTRIE_NO_ಹೊಂದಿಕೆ) {
            ನಿಲ್ಲಿಸು;
        }
        ಆದರೆ (ಉದ್ದಳತೆ_Matched >= max_ಉದ್ದಳತೆ_2) {
            ನಿಲ್ಲಿಸು;
        }
    }

    ಆದರೆ (ಪೂರ್ವಪ್ರತ್ಯಯ != ಶೂನ್ಯಸೂಚಕ) {
        *ಪೂರ್ವಪ್ರತ್ಯಯ = ಸಂಕೇತ_Points_Matched;
    }
    ಹಿಂತಿರುಗಿ ಶಬ್ದ_ಎಣಿಕೆ;
}

ಬೈಟುಗಳು_Dictionary_Matcher::~ಬೈಟುಗಳು_Dictionary_Matcher() {
    udata_close(file);
}

UChar32 ಬೈಟುಗಳು_Dictionary_Matcher::transform(UChar32 c) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ ((transformConstant & Dictionary_ದತ್ತಾಂಶ::TRANSFORM_TYPE_ಮುಖವಾಡ) == Dictionary_ದತ್ತಾಂಶ::TRANSFORM_TYPE_ಸ್ಥಾನಾಂತರ) {
        ಆದರೆ (c == 0x200D) {
            ಹಿಂತಿರುಗಿ 0xFF;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == 0x200C) {
            ಹಿಂತಿರುಗಿ 0xFE;
        }
        int32_t delta = c - (transformConstant & Dictionary_ದತ್ತಾಂಶ::TRANSFORM_ಸ್ಥಾನಾಂತರ_ಮುಖವಾಡ);
        ಆದರೆ (delta < 0 || 0xFD < delta) {
            ಹಿಂತಿರುಗಿ U_SENTINEL;
        }
        ಹಿಂತಿರುಗಿ static_cast<UChar32>(delta);
    }
    ಹಿಂತಿರುಗಿ c;
}

int32_t ಬೈಟುಗಳು_Dictionary_Matcher::getType() ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ Dictionary_ದತ್ತಾಂಶ::TRIE_TYPE_ಬೈಟುಗಳು;
}

int32_t ಬೈಟುಗಳು_Dictionary_Matcher::matches(UText *ಪಠ್ಯ, int32_t max_ಉದ್ದಳತೆ_2, int32_t ಎಲ್ಲೆ,
                            int32_t *lengths, int32_t *cpLengths, int32_t *ಮೌಲ್ಯಗಳು,
                            int32_t *ಪೂರ್ವಪ್ರತ್ಯಯ) ಸ್ಥಿರಾಂಕ {
    ಬೈಟುಗಳು_Trie bt(characters);
    int32_t starting_ಪಠ್ಯ_ಸೂಚ್ಯಂಕ = static_cast<int32_t>(utext_get_Native_ಸೂಚ್ಯಂಕ(ಪಠ್ಯ));
    int32_t ಶಬ್ದ_ಎಣಿಕೆ = 0;
    int32_t ಸಂಕೇತ_Points_Matched = 0;

    ಗಾಗಿ (UChar32 c = utext_next32(ಪಠ್ಯ); c >= 0; c=utext_next32(ಪಠ್ಯ)) {
        UString_Trie_ಫಲಿತಾಂಶ ಫಲಿತಾಂಶ = (ಸಂಕೇತ_Points_Matched == 0) ? bt.first(transform(c)) : bt.next(transform(c));
        int32_t ಉದ್ದಳತೆ_Matched = static_cast<int32_t>(utext_get_Native_ಸೂಚ್ಯಂಕ(ಪಠ್ಯ)) - starting_ಪಠ್ಯ_ಸೂಚ್ಯಂಕ;
        ಸಂಕೇತ_Points_Matched += 1;
        ಆದರೆ (USTRINGTRIE_HAS_ಮೌಲ್ಯ(ಫಲಿತಾಂಶ)) {
            ಆದರೆ (ಶಬ್ದ_ಎಣಿಕೆ < ಎಲ್ಲೆ) {
                ಆದರೆ (ಮೌಲ್ಯಗಳು != ಶೂನ್ಯಸೂಚಕ) {
                    ಮೌಲ್ಯಗಳು[ಶಬ್ದ_ಎಣಿಕೆ] = bt.get_ಮೌಲ್ಯ();
                }
                ಆದರೆ (lengths != ಶೂನ್ಯಸೂಚಕ) {
                    lengths[ಶಬ್ದ_ಎಣಿಕೆ] = ಉದ್ದಳತೆ_Matched;
                }
                ಆದರೆ (cpLengths != ಶೂನ್ಯಸೂಚಕ) {
                    cpLengths[ಶಬ್ದ_ಎಣಿಕೆ] = ಸಂಕೇತ_Points_Matched;
                }
                ++ಶಬ್ದ_ಎಣಿಕೆ;
            }
            ಆದರೆ (ಫಲಿತಾಂಶ == USTRINGTRIE_FINAL_ಮೌಲ್ಯ) {
                ನಿಲ್ಲಿಸು;
            }
        }
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಫಲಿತಾಂಶ == USTRINGTRIE_NO_ಹೊಂದಿಕೆ) {
            ನಿಲ್ಲಿಸು;
        }
        ಆದರೆ (ಉದ್ದಳತೆ_Matched >= max_ಉದ್ದಳತೆ_2) {
            ನಿಲ್ಲಿಸು;
        }
    }

    ಆದರೆ (ಪೂರ್ವಪ್ರತ್ಯಯ != ಶೂನ್ಯಸೂಚಕ) {
        *ಪೂರ್ವಪ್ರತ್ಯಯ = ಸಂಕೇತ_Points_Matched;
    }
    ಹಿಂತಿರುಗಿ ಶಬ್ದ_ಎಣಿಕೆ;
}


U_NAMESPACE_ಕೊನೆ

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
udict_ಅದಲುಬದಲು(ಸ್ಥಿರಾಂಕ UDataSwapper *ds, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಒಳ_ದತ್ತಾಂಶ, int32_t length,
           ಶೂನ್ಯ *ಹೊರಹೋಗು_ದತ್ತಾಂಶ, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಸ್ಥಿರಾಂಕ UDataInfo *pInfo;
    int32_t ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ;
    ಸ್ಥಿರಾಂಕ uint8_t *ಒಳ_ಬೈಟುಗಳು;
    uint8_t *ಹೊರಹೋಗು_ಬೈಟುಗಳು;
    ಸ್ಥಿರಾಂಕ int32_t *ಒಳ_Indexes;
    int32_t indexes[Dictionary_ದತ್ತಾಂಶ::IX_ಎಣಿಕೆ];
    int32_t i, ಸ್ಥಾನಾಂತರ, size;

    ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ = udata_ಅದಲುಬದಲು_ದತ್ತಾಂಶ_ಶೀರ್ಷಿಕೆ(ds, ಒಳ_ದತ್ತಾಂಶ, length, ಹೊರಹೋಗು_ದತ್ತಾಂಶ, p_Error_ಸಂಕೇತ);
    ಆದರೆ (p_Error_ಸಂಕೇತ == ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*p_Error_ಸಂಕೇತ)) ಹಿಂತಿರುಗಿ 0;
    pInfo = (ಸ್ಥಿರಾಂಕ UDataInfo *)((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)ಒಳ_ದತ್ತಾಂಶ + 4);
    ಆದರೆ (!(pInfo->ದತ್ತಾಂಶ_Format[0] == 0x44 && 
          pInfo->ದತ್ತಾಂಶ_Format[1] == 0x69 && 
          pInfo->ದತ್ತಾಂಶ_Format[2] == 0x63 && 
          pInfo->ದತ್ತಾಂಶ_Format[3] == 0x74 && 
          pInfo->format_ಆವೃತ್ತಿ[0] == 1)) {
        udata_printError(ds, "udict_swap(): data format %02x.%02x.%02x.%02x (format version %02x) is not recognized as dictionary data\n",
                         pInfo->ದತ್ತಾಂಶ_Format[0], pInfo->ದತ್ತಾಂಶ_Format[1], pInfo->ದತ್ತಾಂಶ_Format[2], pInfo->ದತ್ತಾಂಶ_Format[3], pInfo->format_ಆವೃತ್ತಿ[0]);
        *p_Error_ಸಂಕೇತ = U_UNSUPPORTED_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }

    ಒಳ_ಬೈಟುಗಳು = (ಸ್ಥಿರಾಂಕ uint8_t *)ಒಳ_ದತ್ತಾಂಶ + ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ;
    ಹೊರಹೋಗು_ಬೈಟುಗಳು = (ಹೊರಹೋಗು_ದತ್ತಾಂಶ == ಶೂನ್ಯಸೂಚಕ) ? ಶೂನ್ಯಸೂಚಕ : (uint8_t *)ಹೊರಹೋಗು_ದತ್ತಾಂಶ + ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ;

    ಒಳ_Indexes = (ಸ್ಥಿರಾಂಕ int32_t *)ಒಳ_ಬೈಟುಗಳು;
    ಆದರೆ (length >= 0) {
        length -= ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ;
        ಆದರೆ (length < (int32_t)(ಗಾತ್ರ(indexes))) {
            udata_printError(ds, "udict_swap(): too few bytes (%d after header) for dictionary data\n", length);
            *p_Error_ಸಂಕೇತ = U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
            ಹಿಂತಿರುಗಿ 0;
        }
    }

    ಗಾಗಿ (i = 0; i < Dictionary_ದತ್ತಾಂಶ::IX_ಎಣಿಕೆ; i++) {
        indexes[i] = udata_readInt32(ds, ಒಳ_Indexes[i]);
    }

    size = indexes[Dictionary_ದತ್ತಾಂಶ::IX_ಒಟ್ಟು_ಗಾತ್ರ];

    ಆದರೆ (length >= 0) {
        ಆದರೆ (length < size) {
            udata_printError(ds, "udict_swap(): too few bytes (%d after header) for all of dictionary data\n", length);
            *p_Error_ಸಂಕೇತ = U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
            ಹಿಂತಿರುಗಿ 0;
        }

        ಆದರೆ (ಒಳ_ಬೈಟುಗಳು != ಹೊರಹೋಗು_ಬೈಟುಗಳು) {
            uprv_memcpy(ಹೊರಹೋಗು_ಬೈಟುಗಳು, ಒಳ_ಬೈಟುಗಳು, size);
        }

        ಸ್ಥಾನಾಂತರ = 0;
        ds->ಅದಲುಬದಲು_Array32(ds, ಒಳ_ಬೈಟುಗಳು, ಗಾತ್ರ(indexes), ಹೊರಹೋಗು_ಬೈಟುಗಳು, p_Error_ಸಂಕೇತ);
        ಸ್ಥಾನಾಂತರ = (int32_t)ಗಾತ್ರ(indexes);
        int32_t trieType = indexes[Dictionary_ದತ್ತಾಂಶ::IX_TRIE_TYPE] & Dictionary_ದತ್ತಾಂಶ::TRIE_TYPE_ಮುಖವಾಡ;
        int32_t ಮುಂದಿನ_ಸ್ಥಾನಾಂತರ = indexes[Dictionary_ದತ್ತಾಂಶ::IX_RESERVED1_ಸ್ಥಾನಾಂತರ];

        ಆದರೆ (trieType == Dictionary_ದತ್ತಾಂಶ::TRIE_TYPE_UCHARS) {
            ds->ಅದಲುಬದಲು_Array16(ds, ಒಳ_ಬೈಟುಗಳು + ಸ್ಥಾನಾಂತರ, ಮುಂದಿನ_ಸ್ಥಾನಾಂತರ - ಸ್ಥಾನಾಂತರ, ಹೊರಹೋಗು_ಬೈಟುಗಳು + ಸ್ಥಾನಾಂತರ, p_Error_ಸಂಕೇತ);
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (trieType == Dictionary_ದತ್ತಾಂಶ::TRIE_TYPE_ಬೈಟುಗಳು) {
            // nothing to do
        } ಇಲ್ಲದಿದ್ದರೆ {
            udata_printError(ds, "udict_swap(): unknown trie type!\n");
            *p_Error_ಸಂಕೇತ = U_UNSUPPORTED_ERROR;
            ಹಿಂತಿರುಗಿ 0;
        }

        // these next two sections are empty in the current format,
        // but may be used later.
        ಸ್ಥಾನಾಂತರ = ಮುಂದಿನ_ಸ್ಥಾನಾಂತರ;
        ಮುಂದಿನ_ಸ್ಥಾನಾಂತರ = indexes[Dictionary_ದತ್ತಾಂಶ::IX_RESERVED2_ಸ್ಥಾನಾಂತರ];
        ಸ್ಥಾನಾಂತರ = ಮುಂದಿನ_ಸ್ಥಾನಾಂತರ;
        ಮುಂದಿನ_ಸ್ಥಾನಾಂತರ = indexes[Dictionary_ದತ್ತಾಂಶ::IX_ಒಟ್ಟು_ಗಾತ್ರ];
        ಸ್ಥಾನಾಂತರ = ಮುಂದಿನ_ಸ್ಥಾನಾಂತರ;
    }
    ಹಿಂತಿರುಗಿ ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ + size;
}
#ಆದರೆ_ಅಂತ್ಯ
