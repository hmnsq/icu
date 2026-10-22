// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*
*   Copyright (C) 2001-2015, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  ustrcase.cpp
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2002feb20
*   created by: Markus W. Scherer
*
*   Implementation file for string casing C API functions.
*   Uses functions from uchar.c for basic functionality that requires access
*   to the Unicode Character Database (uprops.dat).
*/

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/brkiter.h"
#ಅನ್ತರ್ಭಾವ "unicode/casemap.h"
#ಅನ್ತರ್ಭಾವ "unicode/edits.h"
#ಅನ್ತರ್ಭಾವ "unicode/stringoptions.h"
#ಅನ್ತರ್ಭಾವ "unicode/ustring.h"
#ಅನ್ತರ್ಭಾವ "unicode/ucasemap.h"
#ಅನ್ತರ್ಭಾವ "unicode/ubrk.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf16.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "ucase.h"
#ಅನ್ತರ್ಭಾವ "ucasemap_imp.h"
#ಅನ್ತರ್ಭಾವ "ustr_imp.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"

/**
 * Code point for COMBINING ACUTE ACCENT
 * @internal
 */
#ವ್ಯಾಖ್ಯಾನಿಸು ACUTE u'\u0301'

U_NAMESPACE_BEGIN

ನಾಮಾಕಾಶ {

int32_t check_ಅತಿಪ್ರವಾಹ_And_Edits_Error(int32_t dest_ಸೂಚ್ಯಂಕ, int32_t dest_ಸಾಮರ್ಥ್ಯ,
                                   Edits *edits, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಆದರೆ (U_SUCCESS(error_ಸಂಕೇತ)) {
        ಆದರೆ (dest_ಸೂಚ್ಯಂಕ > dest_ಸಾಮರ್ಥ್ಯ) {
            error_ಸಂಕೇತ = U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (edits != ಶೂನ್ಯಸೂಚಕ) {
            edits->copy_Error_ಗಮ್ಯ(error_ಸಂಕೇತ);
        }
    }
    ಹಿಂತಿರುಗಿ dest_ಸೂಚ್ಯಂಕ;
}

/* Appends a full case mapping result, see UCASE_MAX_STRING_LENGTH. */
ಸಾಲೊಳಗೆ int32_t
append_ಫಲಿತಾಂಶ(char16_t *dest, int32_t dest_ಸೂಚ್ಯಂಕ, int32_t dest_ಸಾಮರ್ಥ್ಯ,
             int32_t ಫಲಿತಾಂಶ, ಸ್ಥಿರಾಂಕ char16_t *s,
             int32_t cp_ಉದ್ದಳತೆ, uint32_t options, icu::Edits *edits) {
    UChar32 c;
    int32_t length;

    /* decode the result */
    ಆದರೆ(ಫಲಿತಾಂಶ<0) {
        /* (not) original code point */
        ಆದರೆ(edits!=ಶೂನ್ಯಸೂಚಕ) {
            edits->ಕೂಡಿಸು_Unchanged(cp_ಉದ್ದಳತೆ);
        }
        ಆದರೆ(options & U_OMIT_UNCHANGED_ಪಠ್ಯ) {
            ಹಿಂತಿರುಗಿ dest_ಸೂಚ್ಯಂಕ;
        }
        c=~ಫಲಿತಾಂಶ;
        ಆದರೆ(dest_ಸೂಚ್ಯಂಕ<dest_ಸಾಮರ್ಥ್ಯ && c<=0xffff) {  // BMP slightly-fastpath
            dest[dest_ಸೂಚ್ಯಂಕ++] = static_cast<char16_t>(c);
            ಹಿಂತಿರುಗಿ dest_ಸೂಚ್ಯಂಕ;
        }
        length=cp_ಉದ್ದಳತೆ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಆದರೆ(ಫಲಿತಾಂಶ<=UCASE_MAX_STRING_ಉದ್ದಳತೆ) {
            c=U_SENTINEL;
            length=ಫಲಿತಾಂಶ;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(dest_ಸೂಚ್ಯಂಕ<dest_ಸಾಮರ್ಥ್ಯ && ಫಲಿತಾಂಶ<=0xffff) {  // BMP slightly-fastpath
            dest[dest_ಸೂಚ್ಯಂಕ++] = static_cast<char16_t>(ಫಲಿತಾಂಶ);
            ಆದರೆ(edits!=ಶೂನ್ಯಸೂಚಕ) {
                edits->ಕೂಡಿಸು_Replace(cp_ಉದ್ದಳತೆ, 1);
            }
            ಹಿಂತಿರುಗಿ dest_ಸೂಚ್ಯಂಕ;
        } ಇಲ್ಲದಿದ್ದರೆ {
            c=ಫಲಿತಾಂಶ;
            length=U16_ಉದ್ದಳತೆ(c);
        }
        ಆದರೆ(edits!=ಶೂನ್ಯಸೂಚಕ) {
            edits->ಕೂಡಿಸು_Replace(cp_ಉದ್ದಳತೆ, length);
        }
    }
    ಆದರೆ(length>(INT32_MAX-dest_ಸೂಚ್ಯಂಕ)) {
        ಹಿಂತಿರುಗಿ -1;  // integer overflow
    }

    ಆದರೆ(dest_ಸೂಚ್ಯಂಕ<dest_ಸಾಮರ್ಥ್ಯ) {
        /* append the result */
        ಆದರೆ(c>=0) {
            /* code point */
            UBool isError=ಸುಳ್ಳು;
            U16_APPEND(dest, dest_ಸೂಚ್ಯಂಕ, dest_ಸಾಮರ್ಥ್ಯ, c, isError);
            ಆದರೆ(isError) {
                /* overflow, nothing written */
                dest_ಸೂಚ್ಯಂಕ+=length;
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            /* string */
            ಆದರೆ((dest_ಸೂಚ್ಯಂಕ+length)<=dest_ಸಾಮರ್ಥ್ಯ) {
                ಆಗ(length>0) {
                    dest[dest_ಸೂಚ್ಯಂಕ++]=*s++;
                    --length;
                }
            } ಇಲ್ಲದಿದ್ದರೆ {
                /*ಅತಿಪ್ರವಾಹ*/
                dest_ಸೂಚ್ಯಂಕ+=length;
            }
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* preflight */
        dest_ಸೂಚ್ಯಂಕ+=length;
    }
    ಹಿಂತಿರುಗಿ dest_ಸೂಚ್ಯಂಕ;
}

ಸಾಲೊಳಗೆ int32_t
appendUChar(char16_t *dest, int32_t dest_ಸೂಚ್ಯಂಕ, int32_t dest_ಸಾಮರ್ಥ್ಯ, char16_t c) {
    ಆದರೆ(dest_ಸೂಚ್ಯಂಕ<dest_ಸಾಮರ್ಥ್ಯ) {
        dest[dest_ಸೂಚ್ಯಂಕ]=c;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(dest_ಸೂಚ್ಯಂಕ==INT32_MAX) {
        ಹಿಂತಿರುಗಿ -1;  // integer overflow
    }
    ಹಿಂತಿರುಗಿ dest_ಸೂಚ್ಯಂಕ+1;
}

int32_t
append_Non_ಖಾಲಿಗಳು_Unchanged(char16_t *dest, int32_t dest_ಸೂಚ್ಯಂಕ, int32_t dest_ಸಾಮರ್ಥ್ಯ,
                        ಸ್ಥಿರಾಂಕ char16_t *s, int32_t length, uint32_t options, icu::Edits *edits) {
    ಆದರೆ(edits!=ಶೂನ್ಯಸೂಚಕ) {
        edits->ಕೂಡಿಸು_Unchanged(length);
    }
    ಆದರೆ(options & U_OMIT_UNCHANGED_ಪಠ್ಯ) {
        ಹಿಂತಿರುಗಿ dest_ಸೂಚ್ಯಂಕ;
    }
    ಆದರೆ(length>(INT32_MAX-dest_ಸೂಚ್ಯಂಕ)) {
        ಹಿಂತಿರುಗಿ -1;  // integer overflow
    }
    ಆದರೆ((dest_ಸೂಚ್ಯಂಕ+length)<=dest_ಸಾಮರ್ಥ್ಯ) {
        u_memcpy(dest+dest_ಸೂಚ್ಯಂಕ, s, length);
    }
    ಹಿಂತಿರುಗಿ dest_ಸೂಚ್ಯಂಕ + length;
}

ಸಾಲೊಳಗೆ int32_t
appendUnchanged(char16_t *dest, int32_t dest_ಸೂಚ್ಯಂಕ, int32_t dest_ಸಾಮರ್ಥ್ಯ,
                ಸ್ಥಿರಾಂಕ char16_t *s, int32_t length, uint32_t options, icu::Edits *edits) {
    ಆದರೆ (length <= 0) {
        ಹಿಂತಿರುಗಿ dest_ಸೂಚ್ಯಂಕ;
    }
    ಹಿಂತಿರುಗಿ append_Non_ಖಾಲಿಗಳು_Unchanged(dest, dest_ಸೂಚ್ಯಂಕ, dest_ಸಾಮರ್ಥ್ಯ, s, length, options, edits);
}

UChar32 U_CALLCONV
utf16_caseContextIterator(ಶೂನ್ಯ *context, int8_t dir) {
    UCaseContext* csc = static_cast<UCaseContext*>(context);
    UChar32 c;

    ಆದರೆ(dir<0) {
        /* reset for backward iteration */
        csc->ಸೂಚ್ಯಂಕ=csc->cp_ಆರಂಭ;
        csc->dir=dir;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(dir>0) {
        /* reset for forward iteration */
        csc->ಸೂಚ್ಯಂಕ=csc->cp_ಎಲ್ಲೆ;
        csc->dir=dir;
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* continue current iteration direction */
        dir=csc->dir;
    }

    ಆದರೆ(dir<0) {
        ಆದರೆ(csc->ಆರಂಭ<csc->ಸೂಚ್ಯಂಕ) {
            U16_PREV((ಸ್ಥಿರಾಂಕ char16_t *)csc->p, csc->ಆರಂಭ, csc->ಸೂಚ್ಯಂಕ, c);
            ಹಿಂತಿರುಗಿ c;
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಆದರೆ(csc->ಸೂಚ್ಯಂಕ<csc->ಎಲ್ಲೆ) {
            U16_ಮುಂದಿನ((ಸ್ಥಿರಾಂಕ char16_t *)csc->p, csc->ಸೂಚ್ಯಂಕ, csc->ಎಲ್ಲೆ, c);
            ಹಿಂತಿರುಗಿ c;
        }
    }
    ಹಿಂತಿರುಗಿ U_SENTINEL;
}

/**
 * caseLocale >= 0: Lowercases [srcStart..srcLimit[ but takes context [0..srcLength[ into account.
 * caseLocale < 0: Case-folds [srcStart..srcLimit[.
 */
int32_t ಗಮ್ಯ_ಅಧೋ(int32_t caseLocale, uint32_t options,
                char16_t *dest, int32_t dest_ಸಾಮರ್ಥ್ಯ,
                ಸ್ಥಿರಾಂಕ char16_t *src, UCaseContext *csc, int32_t src_ಆರಂಭ, int32_t src_ಎಲ್ಲೆ,
                icu::Edits *edits, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಸ್ಥಿರಾಂಕ int8_t *latin_ಗಮ್ಯ_ಅಧೋ;
    ಆದರೆ (caseLocale == UCASE_LOC_ಬೇರು ||
            (caseLocale >= 0 ?
                !(caseLocale == UCASE_LOC_TURKISH || caseLocale == UCASE_LOC_LITHUANIAN) :
                (options & FOLD_CASE_OPTIONS_ಮುಖವಾಡ) == U_FOLD_CASE_DEFAULT)) {
        latin_ಗಮ್ಯ_ಅಧೋ = LatinCase::ಗಮ್ಯ_ಅಧೋ_ಸಾಮಾನ್ಯ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        latin_ಗಮ್ಯ_ಅಧೋ = LatinCase::ಗಮ್ಯ_ಅಧೋ_TR_LT;
    }
    ಸ್ಥಿರಾಂಕ UTrie2 *trie = ucase_getTrie();
    int32_t dest_ಸೂಚ್ಯಂಕ = 0;
    int32_t prev = src_ಆರಂಭ;
    int32_t src_ಸೂಚ್ಯಂಕ = src_ಆರಂಭ;
    ಗಾಗಿ (;;) {
        // fast path for simple cases
        char16_t lead = 0;
        ಆಗ (src_ಸೂಚ್ಯಂಕ < src_ಎಲ್ಲೆ) {
            lead = src[src_ಸೂಚ್ಯಂಕ];
            int32_t delta;
            ಆದರೆ (lead < LatinCase::LONG_S) {
                int8_t d = latin_ಗಮ್ಯ_ಅಧೋ[lead];
                ಆದರೆ (d == LatinCase::EXC) { ನಿಲ್ಲಿಸು; }
                ++src_ಸೂಚ್ಯಂಕ;
                ಆದರೆ (d == 0) { ಮುಂದುವರಿ; }
                delta = d;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (lead >= 0xd800) {
                ನಿಲ್ಲಿಸು;  // surrogate or higher
            } ಇಲ್ಲದಿದ್ದರೆ {
                uint16_t props = UTRIE2_GET16_FROM_U16_SINGLE_LEAD(trie, lead);
                ಆದರೆ (UCASE_HAS_EXCEPTION(props)) { ನಿಲ್ಲಿಸು; }
                ++src_ಸೂಚ್ಯಂಕ;
                ಆದರೆ (!UCASE_IS_ಊರ್ಧ್ವ_OR_TITLE(props) || (delta = UCASE_GET_DELTA(props)) == 0) {
                    ಮುಂದುವರಿ;
                }
            }
            lead += static_cast<char16_t>(delta);
            dest_ಸೂಚ್ಯಂಕ = appendUnchanged(dest, dest_ಸೂಚ್ಯಂಕ, dest_ಸಾಮರ್ಥ್ಯ,
                                        src + prev, src_ಸೂಚ್ಯಂಕ - 1 - prev, options, edits);
            ಆದರೆ (dest_ಸೂಚ್ಯಂಕ >= 0) {
                dest_ಸೂಚ್ಯಂಕ = appendUChar(dest, dest_ಸೂಚ್ಯಂಕ, dest_ಸಾಮರ್ಥ್ಯ, lead);
                ಆದರೆ (edits != ಶೂನ್ಯಸೂಚಕ) {
                    edits->ಕೂಡಿಸು_Replace(1, 1);
                }
            }
            ಆದರೆ (dest_ಸೂಚ್ಯಂಕ < 0) {
                error_ಸಂಕೇತ = U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
                ಹಿಂತಿರುಗಿ 0;
            }
            prev = src_ಸೂಚ್ಯಂಕ;
        }
        ಆದರೆ (src_ಸೂಚ್ಯಂಕ >= src_ಎಲ್ಲೆ) {
            ನಿಲ್ಲಿಸು;
        }
        // slow path
        int32_t cp_ಆರಂಭ = src_ಸೂಚ್ಯಂಕ++;
        char16_t trail;
        UChar32 c;
        ಆದರೆ (U16_IS_LEAD(lead) && src_ಸೂಚ್ಯಂಕ < src_ಎಲ್ಲೆ && U16_IS_TRAIL(trail = src[src_ಸೂಚ್ಯಂಕ])) {
            c = U16_GET_SUPPLEMENTARY(lead, trail);
            ++src_ಸೂಚ್ಯಂಕ;
        } ಇಲ್ಲದಿದ್ದರೆ {
            c = lead;
        }
        ಸ್ಥಿರಾಂಕ char16_t *s = ಶೂನ್ಯಸೂಚಕ;
        ಆದರೆ (caseLocale >= 0) {
            csc->cp_ಆರಂಭ = cp_ಆರಂಭ;
            csc->cp_ಎಲ್ಲೆ = src_ಸೂಚ್ಯಂಕ;
            c = ucase_ಗಮ್ಯ_ತುಂಬಿದ_ಅಧೋ(c, utf16_caseContextIterator, csc, &s, caseLocale);
        } ಇಲ್ಲದಿದ್ದರೆ {
            c = ucase_ಗಮ್ಯ_ತುಂಬಿದ_Folding(c, &s, options);
        }
        ಆದರೆ (c >= 0) {
            dest_ಸೂಚ್ಯಂಕ = appendUnchanged(dest, dest_ಸೂಚ್ಯಂಕ, dest_ಸಾಮರ್ಥ್ಯ,
                                        src + prev, cp_ಆರಂಭ - prev, options, edits);
            ಆದರೆ (dest_ಸೂಚ್ಯಂಕ >= 0) {
                dest_ಸೂಚ್ಯಂಕ = append_ಫಲಿತಾಂಶ(dest, dest_ಸೂಚ್ಯಂಕ, dest_ಸಾಮರ್ಥ್ಯ, c, s,
                                         src_ಸೂಚ್ಯಂಕ - cp_ಆರಂಭ, options, edits);
            }
            ಆದರೆ (dest_ಸೂಚ್ಯಂಕ < 0) {
                error_ಸಂಕೇತ = U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
                ಹಿಂತಿರುಗಿ 0;
            }
            prev = src_ಸೂಚ್ಯಂಕ;
        }
    }
    dest_ಸೂಚ್ಯಂಕ = appendUnchanged(dest, dest_ಸೂಚ್ಯಂಕ, dest_ಸಾಮರ್ಥ್ಯ,
                                src + prev, src_ಸೂಚ್ಯಂಕ - prev, options, edits);
    ಆದರೆ (dest_ಸೂಚ್ಯಂಕ < 0) {
        error_ಸಂಕೇತ = U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }
    ಹಿಂತಿರುಗಿ dest_ಸೂಚ್ಯಂಕ;
}

int32_t ಗಮ್ಯ_ಊರ್ಧ್ವ(int32_t caseLocale, uint32_t options,
                char16_t *dest, int32_t dest_ಸಾಮರ್ಥ್ಯ,
                ಸ್ಥಿರಾಂಕ char16_t *src, UCaseContext *csc, int32_t src_ಉದ್ದಳತೆ,
                icu::Edits *edits, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಸ್ಥಿರಾಂಕ int8_t *latin_ಗಮ್ಯ_ಊರ್ಧ್ವ;
    ಆದರೆ (caseLocale == UCASE_LOC_TURKISH) {
        latin_ಗಮ್ಯ_ಊರ್ಧ್ವ = LatinCase::ಗಮ್ಯ_ಊರ್ಧ್ವ_TR;
    } ಇಲ್ಲದಿದ್ದರೆ {
        latin_ಗಮ್ಯ_ಊರ್ಧ್ವ = LatinCase::ಗಮ್ಯ_ಊರ್ಧ್ವ_ಸಾಮಾನ್ಯ;
    }
    ಸ್ಥಿರಾಂಕ UTrie2 *trie = ucase_getTrie();
    int32_t dest_ಸೂಚ್ಯಂಕ = 0;
    int32_t prev = 0;
    int32_t src_ಸೂಚ್ಯಂಕ = 0;
    ಗಾಗಿ (;;) {
        // fast path for simple cases
        char16_t lead = 0;
        ಆಗ (src_ಸೂಚ್ಯಂಕ < src_ಉದ್ದಳತೆ) {
            lead = src[src_ಸೂಚ್ಯಂಕ];
            int32_t delta;
            ಆದರೆ (lead < LatinCase::LONG_S) {
                int8_t d = latin_ಗಮ್ಯ_ಊರ್ಧ್ವ[lead];
                ಆದರೆ (d == LatinCase::EXC) { ನಿಲ್ಲಿಸು; }
                ++src_ಸೂಚ್ಯಂಕ;
                ಆದರೆ (d == 0) { ಮುಂದುವರಿ; }
                delta = d;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (lead >= 0xd800) {
                ನಿಲ್ಲಿಸು;  // surrogate or higher
            } ಇಲ್ಲದಿದ್ದರೆ {
                uint16_t props = UTRIE2_GET16_FROM_U16_SINGLE_LEAD(trie, lead);
                ಆದರೆ (UCASE_HAS_EXCEPTION(props)) { ನಿಲ್ಲಿಸು; }
                ++src_ಸೂಚ್ಯಂಕ;
                ಆದರೆ (UCASE_GET_TYPE(props) != UCASE_ಅಧೋ || (delta = UCASE_GET_DELTA(props)) == 0) {
                    ಮುಂದುವರಿ;
                }
            }
            lead += static_cast<char16_t>(delta);
            dest_ಸೂಚ್ಯಂಕ = appendUnchanged(dest, dest_ಸೂಚ್ಯಂಕ, dest_ಸಾಮರ್ಥ್ಯ,
                                        src + prev, src_ಸೂಚ್ಯಂಕ - 1 - prev, options, edits);
            ಆದರೆ (dest_ಸೂಚ್ಯಂಕ >= 0) {
                dest_ಸೂಚ್ಯಂಕ = appendUChar(dest, dest_ಸೂಚ್ಯಂಕ, dest_ಸಾಮರ್ಥ್ಯ, lead);
                ಆದರೆ (edits != ಶೂನ್ಯಸೂಚಕ) {
                    edits->ಕೂಡಿಸು_Replace(1, 1);
                }
            }
            ಆದರೆ (dest_ಸೂಚ್ಯಂಕ < 0) {
                error_ಸಂಕೇತ = U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
                ಹಿಂತಿರುಗಿ 0;
            }
            prev = src_ಸೂಚ್ಯಂಕ;
        }
        ಆದರೆ (src_ಸೂಚ್ಯಂಕ >= src_ಉದ್ದಳತೆ) {
            ನಿಲ್ಲಿಸು;
        }
        // slow path
        int32_t cp_ಆರಂಭ;
        csc->cp_ಆರಂಭ = cp_ಆರಂಭ = src_ಸೂಚ್ಯಂಕ++;
        char16_t trail;
        UChar32 c;
        ಆದರೆ (U16_IS_LEAD(lead) && src_ಸೂಚ್ಯಂಕ < src_ಉದ್ದಳತೆ && U16_IS_TRAIL(trail = src[src_ಸೂಚ್ಯಂಕ])) {
            c = U16_GET_SUPPLEMENTARY(lead, trail);
            ++src_ಸೂಚ್ಯಂಕ;
        } ಇಲ್ಲದಿದ್ದರೆ {
            c = lead;
        }
        csc->cp_ಎಲ್ಲೆ = src_ಸೂಚ್ಯಂಕ;
        ಸ್ಥಿರಾಂಕ char16_t *s = ಶೂನ್ಯಸೂಚಕ;
        c = ucase_ಗಮ್ಯ_ತುಂಬಿದ_ಊರ್ಧ್ವ(c, utf16_caseContextIterator, csc, &s, caseLocale);
        ಆದರೆ (c >= 0) {
            dest_ಸೂಚ್ಯಂಕ = appendUnchanged(dest, dest_ಸೂಚ್ಯಂಕ, dest_ಸಾಮರ್ಥ್ಯ,
                                        src + prev, cp_ಆರಂಭ - prev, options, edits);
            ಆದರೆ (dest_ಸೂಚ್ಯಂಕ >= 0) {
                dest_ಸೂಚ್ಯಂಕ = append_ಫಲಿತಾಂಶ(dest, dest_ಸೂಚ್ಯಂಕ, dest_ಸಾಮರ್ಥ್ಯ, c, s,
                                         src_ಸೂಚ್ಯಂಕ - cp_ಆರಂಭ, options, edits);
            }
            ಆದರೆ (dest_ಸೂಚ್ಯಂಕ < 0) {
                error_ಸಂಕೇತ = U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
                ಹಿಂತಿರುಗಿ 0;
            }
            prev = src_ಸೂಚ್ಯಂಕ;
        }
    }
    dest_ಸೂಚ್ಯಂಕ = appendUnchanged(dest, dest_ಸೂಚ್ಯಂಕ, dest_ಸಾಮರ್ಥ್ಯ,
                                src + prev, src_ಸೂಚ್ಯಂಕ - prev, options, edits);
    ಆದರೆ (dest_ಸೂಚ್ಯಂಕ < 0) {
        error_ಸಂಕೇತ = U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }
    ಹಿಂತಿರುಗಿ dest_ಸೂಚ್ಯಂಕ;
}

}  // namespace

U_NAMESPACE_ಕೊನೆ

U_NAMESPACE_USE

#ಆದರೆ !UCONFIG_NO_BREAK_ITERATION

ನಾಮಾಕಾಶ {

/**
 * Input: c is a letter I with or without acute accent.
 * start is the index in src after c, and is less than segmentLimit.
 * If a plain i/I is followed by a plain j/J,
 * or an i/I with acute (precomposed or decomposed) is followed by a j/J with acute,
 * then we output accordingly.
 *
 * @return the src index after the titlecased sequence, or the start index if no Dutch IJ
 */
int32_t maybeTitleDutchIJ(ಸ್ಥಿರಾಂಕ char16_t *src, UChar32 c, int32_t ಆರಂಭ, int32_t segment_ಎಲ್ಲೆ,
                          char16_t *dest, int32_t &dest_ಸೂಚ್ಯಂಕ, int32_t dest_ಸಾಮರ್ಥ್ಯ, uint32_t options,
                          icu::Edits *edits) {
    U_ASSERT(ಆರಂಭ < segment_ಎಲ್ಲೆ);

    int32_t ಸೂಚ್ಯಂಕ = ಆರಂಭ;
    ತಾರ್ಕಿಕ withAcute = ಸುಳ್ಳು;

    // If the conditions are met, then the following variables tell us what to output.
    int32_t unchanged1 = 0;  // code units before the j, or the whole sequence (0..3)
    ತಾರ್ಕಿಕ doTitleJ = ಸುಳ್ಳು;  // true if the j needs to be titlecased
    int32_t unchanged2 = 0;  // after the j (0 or 1)

    // next character after the first letter
    char16_t c2 = src[ಸೂಚ್ಯಂಕ++];

    // Is the first letter an i/I with accent?
    ಆದರೆ (c == u'I') {
        ಆದರೆ (c2 == ACUTE) {
            withAcute = ನಿಜ;
            unchanged1 = 1;
            ಆದರೆ (ಸೂಚ್ಯಂಕ == segment_ಎಲ್ಲೆ) { ಹಿಂತಿರುಗಿ ಆರಂಭ; }
            c2 = src[ಸೂಚ್ಯಂಕ++];
        }
    } ಇಲ್ಲದಿದ್ದರೆ {  // Í
        withAcute = ನಿಜ;
    }

    // Is the next character a j/J?
    ಆದರೆ (c2 == u'j') {
        doTitleJ = ನಿಜ;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c2 == u'J') {
        ++unchanged1;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ ಆರಂಭ;
    }

    // A plain i/I must be followed by a plain j/J.
    // An i/I with acute must be followed by a j/J with acute.
    ಆದರೆ (withAcute) {
        ಆದರೆ (ಸೂಚ್ಯಂಕ == segment_ಎಲ್ಲೆ || src[ಸೂಚ್ಯಂಕ++] != ACUTE) { ಹಿಂತಿರುಗಿ ಆರಂಭ; }
        ಆದರೆ (doTitleJ) {
            unchanged2 = 1;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ++unchanged1;
        }
    }

    // There must not be another combining mark.
    ಆದರೆ (ಸೂಚ್ಯಂಕ < segment_ಎಲ್ಲೆ) {
        int32_t cp;
        int32_t i = ಸೂಚ್ಯಂಕ;
        U16_ಮುಂದಿನ(src, i, segment_ಎಲ್ಲೆ, cp);
        uint32_t type_ಮುಖವಾಡ = U_GET_GC_ಮುಖವಾಡ(cp);
        ಆದರೆ ((type_ಮುಖವಾಡ & U_GC_M_ಮುಖವಾಡ) != 0) {
            ಹಿಂತಿರುಗಿ ಆರಂಭ;
        }
    }

    // Output the rest of the Dutch IJ.
    dest_ಸೂಚ್ಯಂಕ = appendUnchanged(dest, dest_ಸೂಚ್ಯಂಕ, dest_ಸಾಮರ್ಥ್ಯ, src + ಆರಂಭ, unchanged1, options, edits);
    ಆರಂಭ += unchanged1;
    ಆದರೆ (doTitleJ) {
        dest_ಸೂಚ್ಯಂಕ = appendUChar(dest, dest_ಸೂಚ್ಯಂಕ, dest_ಸಾಮರ್ಥ್ಯ, u'J');
        ಆದರೆ (edits != ಶೂನ್ಯಸೂಚಕ) {
            edits->ಕೂಡಿಸು_Replace(1, 1);
        }
        ++ಆರಂಭ;
    }
    dest_ಸೂಚ್ಯಂಕ = appendUnchanged(dest, dest_ಸೂಚ್ಯಂಕ, dest_ಸಾಮರ್ಥ್ಯ, src + ಆರಂಭ, unchanged2, options, edits);

    U_ASSERT(ಆರಂಭ + unchanged2 == ಸೂಚ್ಯಂಕ);
    ಹಿಂತಿರುಗಿ ಸೂಚ್ಯಂಕ;
}

}  // namespace

U_CFUNC int32_t U_CALLCONV
ustrcase_ಆಂತರಿಕ_ಗಮ್ಯ_Title(int32_t caseLocale, uint32_t options, BreakIterator *iter,
                         char16_t *dest, int32_t dest_ಸಾಮರ್ಥ್ಯ,
                         ಸ್ಥಿರಾಂಕ char16_t *src, int32_t src_ಉದ್ದಳತೆ,
                         icu::Edits *edits,
                         UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಆದರೆ (!ustrcase_checkTitleAdjustmentOptions(options, error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ 0;
    }

    /* set up local variables */
    UCaseContext csc=UCASECONTEXT_INITIALIZER;
    csc.p=(ಶೂನ್ಯ *)src;
    csc.ಎಲ್ಲೆ=src_ಉದ್ದಳತೆ;
    int32_t dest_ಸೂಚ್ಯಂಕ=0;
    int32_t prev=0;
    ತಾರ್ಕಿಕ is_ಮೊದಲ_ಸೂಚ್ಯಂಕ=ನಿಜ;

    /* titlecasing loop */
    ಆಗ(prev<src_ಉದ್ದಳತೆ) {
        /* find next index where to titlecase */
        int32_t ಸೂಚ್ಯಂಕ;
        ಆದರೆ(is_ಮೊದಲ_ಸೂಚ್ಯಂಕ) {
            is_ಮೊದಲ_ಸೂಚ್ಯಂಕ=ಸುಳ್ಳು;
            ಸೂಚ್ಯಂಕ=iter->first();
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಸೂಚ್ಯಂಕ=iter->next();
        }
        ಆದರೆ(ಸೂಚ್ಯಂಕ==UBRK_ಮುಗಿದ || ಸೂಚ್ಯಂಕ>src_ಉದ್ದಳತೆ) {
            ಸೂಚ್ಯಂಕ=src_ಉದ್ದಳತೆ;
        }

        /*
         * Segment [prev..index[ into 3 parts:
         * a) skipped characters (copy as-is) [prev..titleStart[
         * b) first letter (titlecase)              [titleStart..titleLimit[
         * c) subsequent characters (lowercase)                 [titleLimit..index[
         */
        ಆದರೆ(prev<ಸೂಚ್ಯಂಕ) {
            // Find and copy skipped characters [prev..titleStart[
            int32_t title_ಆರಂಭ=prev;
            int32_t title_ಎಲ್ಲೆ=prev;
            UChar32 c;
            U16_ಮುಂದಿನ(src, title_ಎಲ್ಲೆ, ಸೂಚ್ಯಂಕ, c);
            ಆದರೆ ((options&U_TITLECASE_NO_BREAK_ADJUSTMENT)==0) {
                // Adjust the titlecasing index to the next cased character,
                // or to the next letter/number/symbol/private use.
                // Stop with titleStart<titleLimit<=index
                // if there is a character to be titlecased,
                // or else stop with titleStart==titleLimit==index.
                ತಾರ್ಕಿಕ ಗಮ್ಯ_Cased = (options&U_TITLECASE_ADJUST_ಗಮ್ಯ_CASED) != 0;
                ಆಗ (ಗಮ್ಯ_Cased ? UCASE_NONE==ucase_getType(c) : !ustrcase_isLNS(c)) {
                    title_ಆರಂಭ=title_ಎಲ್ಲೆ;
                    ಆದರೆ(title_ಎಲ್ಲೆ==ಸೂಚ್ಯಂಕ) {
                        ನಿಲ್ಲಿಸು;
                    }
                    U16_ಮುಂದಿನ(src, title_ಎಲ್ಲೆ, ಸೂಚ್ಯಂಕ, c);
                }
                ಆದರೆ (prev < title_ಆರಂಭ) {
                    dest_ಸೂಚ್ಯಂಕ=appendUnchanged(dest, dest_ಸೂಚ್ಯಂಕ, dest_ಸಾಮರ್ಥ್ಯ,
                                              src+prev, title_ಆರಂಭ-prev, options, edits);
                    ಆದರೆ(dest_ಸೂಚ್ಯಂಕ<0) {
                        error_ಸಂಕೇತ=U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
                        ಹಿಂತಿರುಗಿ 0;
                    }
                }
            }

            ಆದರೆ(title_ಆರಂಭ<title_ಎಲ್ಲೆ) {
                /* titlecase c which is from [titleStart..titleLimit[ */
                csc.cp_ಆರಂಭ=title_ಆರಂಭ;
                csc.cp_ಎಲ್ಲೆ=title_ಎಲ್ಲೆ;
                ಸ್ಥಿರಾಂಕ char16_t *s;
                c=ucase_ಗಮ್ಯ_ತುಂಬಿದ_Title(c, utf16_caseContextIterator, &csc, &s, caseLocale);
                dest_ಸೂಚ್ಯಂಕ=append_ಫಲಿತಾಂಶ(dest, dest_ಸೂಚ್ಯಂಕ, dest_ಸಾಮರ್ಥ್ಯ, c, s,
                                       title_ಎಲ್ಲೆ-title_ಆರಂಭ, options, edits);
                ಆದರೆ(dest_ಸೂಚ್ಯಂಕ<0) {
                    error_ಸಂಕೇತ=U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
                    ಹಿಂತಿರುಗಿ 0;
                }

                /* Special case Dutch IJ titlecasing */
                ಆದರೆ (title_ಆರಂಭ+1 < ಸೂಚ್ಯಂಕ &&
                        caseLocale == UCASE_LOC_DUTCH) {
                    ಆದರೆ (c < 0) {
                        c = ~c;
                    }

                    ಆದರೆ (c == u'I' || c == u'Í') {
                        title_ಎಲ್ಲೆ = maybeTitleDutchIJ(src, c, title_ಆರಂಭ + 1, ಸೂಚ್ಯಂಕ,
                                                       dest, dest_ಸೂಚ್ಯಂಕ, dest_ಸಾಮರ್ಥ್ಯ, options,
                                                       edits);
                    }
                }

                /* lowercase [titleLimit..index[ */
                ಆದರೆ(title_ಎಲ್ಲೆ<ಸೂಚ್ಯಂಕ) {
                    ಆದರೆ((options&U_TITLECASE_NO_LOWERCASE)==0) {
                        /* Normal operation: Lowercase the rest of the word. */
                        dest_ಸೂಚ್ಯಂಕ+=
                            ಗಮ್ಯ_ಅಧೋ(
                                caseLocale, options,
                                (dest==ಶೂನ್ಯಸೂಚಕ) ? ಶೂನ್ಯಸೂಚಕ: dest+dest_ಸೂಚ್ಯಂಕ, dest_ಸಾಮರ್ಥ್ಯ-dest_ಸೂಚ್ಯಂಕ,
                                src, &csc, title_ಎಲ್ಲೆ, ಸೂಚ್ಯಂಕ,
                                edits, error_ಸಂಕೇತ);
                        ಆದರೆ(error_ಸಂಕೇತ==U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR) {
                            error_ಸಂಕೇತ=U_ZERO_ERROR;
                        }
                        ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
                            ಹಿಂತಿರುಗಿ dest_ಸೂಚ್ಯಂಕ;
                        }
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        /* Optionally just copy the rest of the word unchanged. */
                        dest_ಸೂಚ್ಯಂಕ=appendUnchanged(dest, dest_ಸೂಚ್ಯಂಕ, dest_ಸಾಮರ್ಥ್ಯ,
                                                  src+title_ಎಲ್ಲೆ, ಸೂಚ್ಯಂಕ-title_ಎಲ್ಲೆ, options, edits);
                        ಆದರೆ(dest_ಸೂಚ್ಯಂಕ<0) {
                            error_ಸಂಕೇತ=U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
                            ಹಿಂತಿರುಗಿ 0;
                        }
                    }
                }
            }
        }

        prev=ಸೂಚ್ಯಂಕ;
    }

    ಹಿಂತಿರುಗಿ check_ಅತಿಪ್ರವಾಹ_And_Edits_Error(dest_ಸೂಚ್ಯಂಕ, dest_ಸಾಮರ್ಥ್ಯ, edits, error_ಸಂಕೇತ);
}

#ಆದರೆ_ಅಂತ್ಯ  // !UCONFIG_NO_BREAK_ITERATION

U_NAMESPACE_BEGIN
ನಾಮಾಕಾಶ Greek_ಊರ್ಧ್ವ {

// Data generated by prototype code, see
// https://icu.unicode.org/design/case/greek-upper
// TODO: Move this data into ucase.icu.
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ uint16_t data0370[] = {
    // U+0370..03FF
    0x0370,
    0x0370,
    0x0372,
    0x0372,
    0,
    0,
    0x0376,
    0x0376,
    0,
    0,
    0x037A,
    0x03FD,
    0x03FE,
    0x03FF,
    0,
    0x037F,
    0,
    0,
    0,
    0,
    0,
    0,
    0x0391 | HAS_VOWEL | HAS_ACCENT,
    0,
    0x0395 | HAS_VOWEL | HAS_ACCENT,
    0x0397 | HAS_VOWEL | HAS_ACCENT,
    0x0399 | HAS_VOWEL | HAS_ACCENT,
    0,
    0x039F | HAS_VOWEL | HAS_ACCENT,
    0,
    0x03A5 | HAS_VOWEL | HAS_ACCENT,
    0x03A9 | HAS_VOWEL | HAS_ACCENT,
    0x0399 | HAS_VOWEL | HAS_ACCENT | HAS_DIALYTIKA,
    0x0391 | HAS_VOWEL,
    0x0392,
    0x0393,
    0x0394,
    0x0395 | HAS_VOWEL,
    0x0396,
    0x0397 | HAS_VOWEL,
    0x0398,
    0x0399 | HAS_VOWEL,
    0x039A,
    0x039B,
    0x039C,
    0x039D,
    0x039E,
    0x039F | HAS_VOWEL,
    0x03A0,
    0x03A1,
    0,
    0x03A3,
    0x03A4,
    0x03A5 | HAS_VOWEL,
    0x03A6,
    0x03A7,
    0x03A8,
    0x03A9 | HAS_VOWEL,
    0x0399 | HAS_VOWEL | HAS_DIALYTIKA,
    0x03A5 | HAS_VOWEL | HAS_DIALYTIKA,
    0x0391 | HAS_VOWEL | HAS_ACCENT,
    0x0395 | HAS_VOWEL | HAS_ACCENT,
    0x0397 | HAS_VOWEL | HAS_ACCENT,
    0x0399 | HAS_VOWEL | HAS_ACCENT,
    0x03A5 | HAS_VOWEL | HAS_ACCENT | HAS_DIALYTIKA,
    0x0391 | HAS_VOWEL,
    0x0392,
    0x0393,
    0x0394,
    0x0395 | HAS_VOWEL,
    0x0396,
    0x0397 | HAS_VOWEL,
    0x0398,
    0x0399 | HAS_VOWEL,
    0x039A,
    0x039B,
    0x039C,
    0x039D,
    0x039E,
    0x039F | HAS_VOWEL,
    0x03A0,
    0x03A1,
    0x03A3,
    0x03A3,
    0x03A4,
    0x03A5 | HAS_VOWEL,
    0x03A6,
    0x03A7,
    0x03A8,
    0x03A9 | HAS_VOWEL,
    0x0399 | HAS_VOWEL | HAS_DIALYTIKA,
    0x03A5 | HAS_VOWEL | HAS_DIALYTIKA,
    0x039F | HAS_VOWEL | HAS_ACCENT,
    0x03A5 | HAS_VOWEL | HAS_ACCENT,
    0x03A9 | HAS_VOWEL | HAS_ACCENT,
    0x03CF,
    0x0392,
    0x0398,
    0x03D2,
    0x03D2 | HAS_ACCENT,
    0x03D2 | HAS_DIALYTIKA,
    0x03A6,
    0x03A0,
    0x03CF,
    0x03D8,
    0x03D8,
    0x03DA,
    0x03DA,
    0x03DC,
    0x03DC,
    0x03DE,
    0x03DE,
    0x03E0,
    0x03E0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0x039A,
    0x03A1,
    0x03F9,
    0x037F,
    0x03F4,
    0x0395 | HAS_VOWEL,
    0,
    0x03F7,
    0x03F7,
    0x03F9,
    0x03FA,
    0x03FA,
    0x03FC,
    0x03FD,
    0x03FE,
    0x03FF,
};

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ uint16_t data1F00[] = {
    // U+1F00..1FFF
    0x0391 | HAS_VOWEL,
    0x0391 | HAS_VOWEL,
    0x0391 | HAS_VOWEL | HAS_ACCENT,
    0x0391 | HAS_VOWEL | HAS_ACCENT,
    0x0391 | HAS_VOWEL | HAS_ACCENT,
    0x0391 | HAS_VOWEL | HAS_ACCENT,
    0x0391 | HAS_VOWEL | HAS_ACCENT,
    0x0391 | HAS_VOWEL | HAS_ACCENT,
    0x0391 | HAS_VOWEL,
    0x0391 | HAS_VOWEL,
    0x0391 | HAS_VOWEL | HAS_ACCENT,
    0x0391 | HAS_VOWEL | HAS_ACCENT,
    0x0391 | HAS_VOWEL | HAS_ACCENT,
    0x0391 | HAS_VOWEL | HAS_ACCENT,
    0x0391 | HAS_VOWEL | HAS_ACCENT,
    0x0391 | HAS_VOWEL | HAS_ACCENT,
    0x0395 | HAS_VOWEL,
    0x0395 | HAS_VOWEL,
    0x0395 | HAS_VOWEL | HAS_ACCENT,
    0x0395 | HAS_VOWEL | HAS_ACCENT,
    0x0395 | HAS_VOWEL | HAS_ACCENT,
    0x0395 | HAS_VOWEL | HAS_ACCENT,
    0,
    0,
    0x0395 | HAS_VOWEL,
    0x0395 | HAS_VOWEL,
    0x0395 | HAS_VOWEL | HAS_ACCENT,
    0x0395 | HAS_VOWEL | HAS_ACCENT,
    0x0395 | HAS_VOWEL | HAS_ACCENT,
    0x0395 | HAS_VOWEL | HAS_ACCENT,
    0,
    0,
    0x0397 | HAS_VOWEL,
    0x0397 | HAS_VOWEL,
    0x0397 | HAS_VOWEL | HAS_ACCENT,
    0x0397 | HAS_VOWEL | HAS_ACCENT,
    0x0397 | HAS_VOWEL | HAS_ACCENT,
    0x0397 | HAS_VOWEL | HAS_ACCENT,
    0x0397 | HAS_VOWEL | HAS_ACCENT,
    0x0397 | HAS_VOWEL | HAS_ACCENT,
    0x0397 | HAS_VOWEL,
    0x0397 | HAS_VOWEL,
    0x0397 | HAS_VOWEL | HAS_ACCENT,
    0x0397 | HAS_VOWEL | HAS_ACCENT,
    0x0397 | HAS_VOWEL | HAS_ACCENT,
    0x0397 | HAS_VOWEL | HAS_ACCENT,
    0x0397 | HAS_VOWEL | HAS_ACCENT,
    0x0397 | HAS_VOWEL | HAS_ACCENT,
    0x0399 | HAS_VOWEL,
    0x0399 | HAS_VOWEL,
    0x0399 | HAS_VOWEL | HAS_ACCENT,
    0x0399 | HAS_VOWEL | HAS_ACCENT,
    0x0399 | HAS_VOWEL | HAS_ACCENT,
    0x0399 | HAS_VOWEL | HAS_ACCENT,
    0x0399 | HAS_VOWEL | HAS_ACCENT,
    0x0399 | HAS_VOWEL | HAS_ACCENT,
    0x0399 | HAS_VOWEL,
    0x0399 | HAS_VOWEL,
    0x0399 | HAS_VOWEL | HAS_ACCENT,
    0x0399 | HAS_VOWEL | HAS_ACCENT,
    0x0399 | HAS_VOWEL | HAS_ACCENT,
    0x0399 | HAS_VOWEL | HAS_ACCENT,
    0x0399 | HAS_VOWEL | HAS_ACCENT,
    0x0399 | HAS_VOWEL | HAS_ACCENT,
    0x039F | HAS_VOWEL,
    0x039F | HAS_VOWEL,
    0x039F | HAS_VOWEL | HAS_ACCENT,
    0x039F | HAS_VOWEL | HAS_ACCENT,
    0x039F | HAS_VOWEL | HAS_ACCENT,
    0x039F | HAS_VOWEL | HAS_ACCENT,
    0,
    0,
    0x039F | HAS_VOWEL,
    0x039F | HAS_VOWEL,
    0x039F | HAS_VOWEL | HAS_ACCENT,
    0x039F | HAS_VOWEL | HAS_ACCENT,
    0x039F | HAS_VOWEL | HAS_ACCENT,
    0x039F | HAS_VOWEL | HAS_ACCENT,
    0,
    0,
    0x03A5 | HAS_VOWEL,
    0x03A5 | HAS_VOWEL,
    0x03A5 | HAS_VOWEL | HAS_ACCENT,
    0x03A5 | HAS_VOWEL | HAS_ACCENT,
    0x03A5 | HAS_VOWEL | HAS_ACCENT,
    0x03A5 | HAS_VOWEL | HAS_ACCENT,
    0x03A5 | HAS_VOWEL | HAS_ACCENT,
    0x03A5 | HAS_VOWEL | HAS_ACCENT,
    0,
    0x03A5 | HAS_VOWEL,
    0,
    0x03A5 | HAS_VOWEL | HAS_ACCENT,
    0,
    0x03A5 | HAS_VOWEL | HAS_ACCENT,
    0,
    0x03A5 | HAS_VOWEL | HAS_ACCENT,
    0x03A9 | HAS_VOWEL,
    0x03A9 | HAS_VOWEL,
    0x03A9 | HAS_VOWEL | HAS_ACCENT,
    0x03A9 | HAS_VOWEL | HAS_ACCENT,
    0x03A9 | HAS_VOWEL | HAS_ACCENT,
    0x03A9 | HAS_VOWEL | HAS_ACCENT,
    0x03A9 | HAS_VOWEL | HAS_ACCENT,
    0x03A9 | HAS_VOWEL | HAS_ACCENT,
    0x03A9 | HAS_VOWEL,
    0x03A9 | HAS_VOWEL,
    0x03A9 | HAS_VOWEL | HAS_ACCENT,
    0x03A9 | HAS_VOWEL | HAS_ACCENT,
    0x03A9 | HAS_VOWEL | HAS_ACCENT,
    0x03A9 | HAS_VOWEL | HAS_ACCENT,
    0x03A9 | HAS_VOWEL | HAS_ACCENT,
    0x03A9 | HAS_VOWEL | HAS_ACCENT,
    0x0391 | HAS_VOWEL | HAS_ACCENT,
    0x0391 | HAS_VOWEL | HAS_ACCENT,
    0x0395 | HAS_VOWEL | HAS_ACCENT,
    0x0395 | HAS_VOWEL | HAS_ACCENT,
    0x0397 | HAS_VOWEL | HAS_ACCENT,
    0x0397 | HAS_VOWEL | HAS_ACCENT,
    0x0399 | HAS_VOWEL | HAS_ACCENT,
    0x0399 | HAS_VOWEL | HAS_ACCENT,
    0x039F | HAS_VOWEL | HAS_ACCENT,
    0x039F | HAS_VOWEL | HAS_ACCENT,
    0x03A5 | HAS_VOWEL | HAS_ACCENT,
    0x03A5 | HAS_VOWEL | HAS_ACCENT,
    0x03A9 | HAS_VOWEL | HAS_ACCENT,
    0x03A9 | HAS_VOWEL | HAS_ACCENT,
    0,
    0,
    0x0391 | HAS_VOWEL | HAS_YPOGEGRAMMENI,
    0x0391 | HAS_VOWEL | HAS_YPOGEGRAMMENI,
    0x0391 | HAS_VOWEL | HAS_YPOGEGRAMMENI | HAS_ACCENT,
    0x0391 | HAS_VOWEL | HAS_YPOGEGRAMMENI | HAS_ACCENT,
    0x0391 | HAS_VOWEL | HAS_YPOGEGRAMMENI | HAS_ACCENT,
    0x0391 | HAS_VOWEL | HAS_YPOGEGRAMMENI | HAS_ACCENT,
    0x0391 | HAS_VOWEL | HAS_YPOGEGRAMMENI | HAS_ACCENT,
    0x0391 | HAS_VOWEL | HAS_YPOGEGRAMMENI | HAS_ACCENT,
    0x0391 | HAS_VOWEL | HAS_YPOGEGRAMMENI,
    0x0391 | HAS_VOWEL | HAS_YPOGEGRAMMENI,
    0x0391 | HAS_VOWEL | HAS_YPOGEGRAMMENI | HAS_ACCENT,
    0x0391 | HAS_VOWEL | HAS_YPOGEGRAMMENI | HAS_ACCENT,
    0x0391 | HAS_VOWEL | HAS_YPOGEGRAMMENI | HAS_ACCENT,
    0x0391 | HAS_VOWEL | HAS_YPOGEGRAMMENI | HAS_ACCENT,
    0x0391 | HAS_VOWEL | HAS_YPOGEGRAMMENI | HAS_ACCENT,
    0x0391 | HAS_VOWEL | HAS_YPOGEGRAMMENI | HAS_ACCENT,
    0x0397 | HAS_VOWEL | HAS_YPOGEGRAMMENI,
    0x0397 | HAS_VOWEL | HAS_YPOGEGRAMMENI,
    0x0397 | HAS_VOWEL | HAS_YPOGEGRAMMENI | HAS_ACCENT,
    0x0397 | HAS_VOWEL | HAS_YPOGEGRAMMENI | HAS_ACCENT,
    0x0397 | HAS_VOWEL | HAS_YPOGEGRAMMENI | HAS_ACCENT,
    0x0397 | HAS_VOWEL | HAS_YPOGEGRAMMENI | HAS_ACCENT,
    0x0397 | HAS_VOWEL | HAS_YPOGEGRAMMENI | HAS_ACCENT,
    0x0397 | HAS_VOWEL | HAS_YPOGEGRAMMENI | HAS_ACCENT,
    0x0397 | HAS_VOWEL | HAS_YPOGEGRAMMENI,
    0x0397 | HAS_VOWEL | HAS_YPOGEGRAMMENI,
    0x0397 | HAS_VOWEL | HAS_YPOGEGRAMMENI | HAS_ACCENT,
    0x0397 | HAS_VOWEL | HAS_YPOGEGRAMMENI | HAS_ACCENT,
    0x0397 | HAS_VOWEL | HAS_YPOGEGRAMMENI | HAS_ACCENT,
    0x0397 | HAS_VOWEL | HAS_YPOGEGRAMMENI | HAS_ACCENT,
    0x0397 | HAS_VOWEL | HAS_YPOGEGRAMMENI | HAS_ACCENT,
    0x0397 | HAS_VOWEL | HAS_YPOGEGRAMMENI | HAS_ACCENT,
    0x03A9 | HAS_VOWEL | HAS_YPOGEGRAMMENI,
    0x03A9 | HAS_VOWEL | HAS_YPOGEGRAMMENI,
    0x03A9 | HAS_VOWEL | HAS_YPOGEGRAMMENI | HAS_ACCENT,
    0x03A9 | HAS_VOWEL | HAS_YPOGEGRAMMENI | HAS_ACCENT,
    0x03A9 | HAS_VOWEL | HAS_YPOGEGRAMMENI | HAS_ACCENT,
    0x03A9 | HAS_VOWEL | HAS_YPOGEGRAMMENI | HAS_ACCENT,
    0x03A9 | HAS_VOWEL | HAS_YPOGEGRAMMENI | HAS_ACCENT,
    0x03A9 | HAS_VOWEL | HAS_YPOGEGRAMMENI | HAS_ACCENT,
    0x03A9 | HAS_VOWEL | HAS_YPOGEGRAMMENI,
    0x03A9 | HAS_VOWEL | HAS_YPOGEGRAMMENI,
    0x03A9 | HAS_VOWEL | HAS_YPOGEGRAMMENI | HAS_ACCENT,
    0x03A9 | HAS_VOWEL | HAS_YPOGEGRAMMENI | HAS_ACCENT,
    0x03A9 | HAS_VOWEL | HAS_YPOGEGRAMMENI | HAS_ACCENT,
    0x03A9 | HAS_VOWEL | HAS_YPOGEGRAMMENI | HAS_ACCENT,
    0x03A9 | HAS_VOWEL | HAS_YPOGEGRAMMENI | HAS_ACCENT,
    0x03A9 | HAS_VOWEL | HAS_YPOGEGRAMMENI | HAS_ACCENT,
    0x0391 | HAS_VOWEL,
    0x0391 | HAS_VOWEL,
    0x0391 | HAS_VOWEL | HAS_YPOGEGRAMMENI | HAS_ACCENT,
    0x0391 | HAS_VOWEL | HAS_YPOGEGRAMMENI,
    0x0391 | HAS_VOWEL | HAS_YPOGEGRAMMENI | HAS_ACCENT,
    0,
    0x0391 | HAS_VOWEL | HAS_ACCENT,
    0x0391 | HAS_VOWEL | HAS_YPOGEGRAMMENI | HAS_ACCENT,
    0x0391 | HAS_VOWEL,
    0x0391 | HAS_VOWEL,
    0x0391 | HAS_VOWEL | HAS_ACCENT,
    0x0391 | HAS_VOWEL | HAS_ACCENT,
    0x0391 | HAS_VOWEL | HAS_YPOGEGRAMMENI,
    0,
    0x0399 | HAS_VOWEL,
    0,
    0,
    0,
    0x0397 | HAS_VOWEL | HAS_YPOGEGRAMMENI | HAS_ACCENT,
    0x0397 | HAS_VOWEL | HAS_YPOGEGRAMMENI,
    0x0397 | HAS_VOWEL | HAS_YPOGEGRAMMENI | HAS_ACCENT,
    0,
    0x0397 | HAS_VOWEL | HAS_ACCENT,
    0x0397 | HAS_VOWEL | HAS_YPOGEGRAMMENI | HAS_ACCENT,
    0x0395 | HAS_VOWEL | HAS_ACCENT,
    0x0395 | HAS_VOWEL | HAS_ACCENT,
    0x0397 | HAS_VOWEL | HAS_ACCENT,
    0x0397 | HAS_VOWEL | HAS_ACCENT,
    0x0397 | HAS_VOWEL | HAS_YPOGEGRAMMENI,
    0,
    0,
    0,
    0x0399 | HAS_VOWEL,
    0x0399 | HAS_VOWEL,
    0x0399 | HAS_VOWEL | HAS_ACCENT | HAS_DIALYTIKA,
    0x0399 | HAS_VOWEL | HAS_ACCENT | HAS_DIALYTIKA,
    0,
    0,
    0x0399 | HAS_VOWEL | HAS_ACCENT,
    0x0399 | HAS_VOWEL | HAS_ACCENT | HAS_DIALYTIKA,
    0x0399 | HAS_VOWEL,
    0x0399 | HAS_VOWEL,
    0x0399 | HAS_VOWEL | HAS_ACCENT,
    0x0399 | HAS_VOWEL | HAS_ACCENT,
    0,
    0,
    0,
    0,
    0x03A5 | HAS_VOWEL,
    0x03A5 | HAS_VOWEL,
    0x03A5 | HAS_VOWEL | HAS_ACCENT | HAS_DIALYTIKA,
    0x03A5 | HAS_VOWEL | HAS_ACCENT | HAS_DIALYTIKA,
    0x03A1,
    0x03A1,
    0x03A5 | HAS_VOWEL | HAS_ACCENT,
    0x03A5 | HAS_VOWEL | HAS_ACCENT | HAS_DIALYTIKA,
    0x03A5 | HAS_VOWEL,
    0x03A5 | HAS_VOWEL,
    0x03A5 | HAS_VOWEL | HAS_ACCENT,
    0x03A5 | HAS_VOWEL | HAS_ACCENT,
    0x03A1,
    0,
    0,
    0,
    0,
    0,
    0x03A9 | HAS_VOWEL | HAS_YPOGEGRAMMENI | HAS_ACCENT,
    0x03A9 | HAS_VOWEL | HAS_YPOGEGRAMMENI,
    0x03A9 | HAS_VOWEL | HAS_YPOGEGRAMMENI | HAS_ACCENT,
    0,
    0x03A9 | HAS_VOWEL | HAS_ACCENT,
    0x03A9 | HAS_VOWEL | HAS_YPOGEGRAMMENI | HAS_ACCENT,
    0x039F | HAS_VOWEL | HAS_ACCENT,
    0x039F | HAS_VOWEL | HAS_ACCENT,
    0x03A9 | HAS_VOWEL | HAS_ACCENT,
    0x03A9 | HAS_VOWEL | HAS_ACCENT,
    0x03A9 | HAS_VOWEL | HAS_YPOGEGRAMMENI,
    0,
    0,
    0,
};

// U+2126 Ohm sign
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ uint16_t data2126 = 0x03A9 | HAS_VOWEL;

uint32_t get_Letter_ದತ್ತಾಂಶ(UChar32 c) {
    ಆದರೆ (c < 0x370 || 0x2126 < c || (0x3ff < c && c < 0x1f00)) {
        ಹಿಂತಿರುಗಿ 0;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c <= 0x3ff) {
        ಹಿಂತಿರುಗಿ data0370[c - 0x370];
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c <= 0x1fff) {
        ಹಿಂತಿರುಗಿ data1F00[c - 0x1f00];
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == 0x2126) {
        ಹಿಂತಿರುಗಿ data2126;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ 0;
    }
}

uint32_t get_Diacritic_ದತ್ತಾಂಶ(UChar32 c) {
    ಆಯ್ಕೆ (c) {
    ಸ್ಥಿತಿ 0x0300:  // varia
    ಸ್ಥಿತಿ 0x0301:  // tonos = oxia
    ಸ್ಥಿತಿ 0x0342:  // perispomeni
    ಸ್ಥಿತಿ 0x0302:  // circumflex can look like perispomeni
    ಸ್ಥಿತಿ 0x0303:  // tilde can look like perispomeni
    ಸ್ಥಿತಿ 0x0311:  // inverted breve can look like perispomeni
        ಹಿಂತಿರುಗಿ HAS_ACCENT;
    ಸ್ಥಿತಿ 0x0308:  // dialytika = diaeresis
        ಹಿಂತಿರುಗಿ HAS_COMBINING_DIALYTIKA;
    ಸ್ಥಿತಿ 0x0344:  // dialytika tonos
        ಹಿಂತಿರುಗಿ HAS_COMBINING_DIALYTIKA | HAS_ACCENT;
    ಸ್ಥಿತಿ 0x0345:  // ypogegrammeni = iota subscript
        ಹಿಂತಿರುಗಿ HAS_YPOGEGRAMMENI;
    ಸ್ಥಿತಿ 0x0304:  // macron
    ಸ್ಥಿತಿ 0x0306:  // breve
    ಸ್ಥಿತಿ 0x0313:  // comma above
    ಸ್ಥಿತಿ 0x0314:  // reversed comma above
    ಸ್ಥಿತಿ 0x0343:  // koronis
        ಹಿಂತಿರುಗಿ HAS_OTHER_GREEK_DIACRITIC;
    ಪೂರ್ವನಿಯೋಜಿತ:
        ಹಿಂತಿರುಗಿ 0;
    }
}

UBool isFollowedByCasedLetter(ಸ್ಥಿರಾಂಕ char16_t *s, int32_t i, int32_t length) {
    ಆಗ (i < length) {
        UChar32 c;
        U16_ಮುಂದಿನ(s, i, length, c);
        int32_t type = ucase_getTypeOrIgnorable(c);
        ಆದರೆ ((type & UCASE_IGNORABLE) != 0) {
            // Case-ignorable, continue with the loop.
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (type != UCASE_NONE) {
            ಹಿಂತಿರುಗಿ ನಿಜ;  // Followed by cased letter.
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;  // Uncased and not case-ignorable.
        }
    }
    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;  // Not followed by cased letter.
}

/**
 * Greek string uppercasing with a state machine.
 * Probably simpler than a stateless function that has to figure out complex context-before
 * for each character.
 * TODO: Try to re-consolidate one way or another with the non-Greek function.
 */
int32_t ಗಮ್ಯ_ಊರ್ಧ್ವ(uint32_t options,
                char16_t *dest, int32_t dest_ಸಾಮರ್ಥ್ಯ,
                ಸ್ಥಿರಾಂಕ char16_t *src, int32_t src_ಉದ್ದಳತೆ,
                Edits *edits,
                UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    int32_t dest_ಸೂಚ್ಯಂಕ=0;
    uint32_t state = 0;
    ಗಾಗಿ (int32_t i = 0; i < src_ಉದ್ದಳತೆ;) {
        int32_t ಮುಂದಿನ_ಸೂಚ್ಯಂಕ = i;
        UChar32 c;
        U16_ಮುಂದಿನ(src, ಮುಂದಿನ_ಸೂಚ್ಯಂಕ, src_ಉದ್ದಳತೆ, c);
        uint32_t ಮುಂದಿನ_State = 0;
        int32_t type = ucase_getTypeOrIgnorable(c);
        ಆದರೆ ((type & UCASE_IGNORABLE) != 0) {
            // c is case-ignorable
            ಮುಂದಿನ_State |= (state & AFTER_CASED);
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (type != UCASE_NONE) {
            // c is cased
            ಮುಂದಿನ_State |= AFTER_CASED;
        }
        uint32_t ದತ್ತಾಂಶ = get_Letter_ದತ್ತಾಂಶ(c);
        ಆದರೆ (ದತ್ತಾಂಶ > 0) {
            uint32_t upper = ದತ್ತಾಂಶ & ಊರ್ಧ್ವ_ಮುಖವಾಡ;
            // Add a dialytika to this iota or ypsilon vowel
            // if we removed a tonos from the previous vowel,
            // and that previous vowel did not also have (or gain) a dialytika.
            // Adding one only to the final vowel in a longer sequence
            // (which does not occur in normal writing) would require lookahead.
            // Set the same flag as for preserving an existing dialytika.
            ಆದರೆ ((ದತ್ತಾಂಶ & HAS_VOWEL) != 0 &&
                (state & (AFTER_VOWEL_WITH_PRECOMPOSED_ACCENT | AFTER_VOWEL_WITH_COMBINING_ACCENT)) !=
                    0 &&
                (upper == 0x399 || upper == 0x3A5)) {
                ದತ್ತಾಂಶ |= (state & AFTER_VOWEL_WITH_PRECOMPOSED_ACCENT) ? HAS_DIALYTIKA
                                                                      : HAS_COMBINING_DIALYTIKA;
            }
            int32_t ಅಂಕೆ_Ypogegrammeni = 0;  // Map each one to a trailing, spacing, capital iota.
            ಆದರೆ ((ದತ್ತಾಂಶ & HAS_YPOGEGRAMMENI) != 0) {
                ಅಂಕೆ_Ypogegrammeni = 1;
            }
            ಸ್ಥಿರಾಂಕ UBool hasPrecomposedAccent = (ದತ್ತಾಂಶ & HAS_ACCENT) != 0;
            // Skip combining diacritics after this Greek letter.
            ಆಗ (ಮುಂದಿನ_ಸೂಚ್ಯಂಕ < src_ಉದ್ದಳತೆ) {
                uint32_t diacritic_ದತ್ತಾಂಶ = get_Diacritic_ದತ್ತಾಂಶ(src[ಮುಂದಿನ_ಸೂಚ್ಯಂಕ]);
                ಆದರೆ (diacritic_ದತ್ತಾಂಶ != 0) {
                    ದತ್ತಾಂಶ |= diacritic_ದತ್ತಾಂಶ;
                    ಆದರೆ ((diacritic_ದತ್ತಾಂಶ & HAS_YPOGEGRAMMENI) != 0) {
                        ++ಅಂಕೆ_Ypogegrammeni;
                    }
                    ++ಮುಂದಿನ_ಸೂಚ್ಯಂಕ;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    ನಿಲ್ಲಿಸು;  // not a Greek diacritic
                }
            }
            ಆದರೆ ((ದತ್ತಾಂಶ & HAS_VOWEL_AND_ACCENT_AND_DIALYTIKA) == HAS_VOWEL_AND_ACCENT) {
                ಮುಂದಿನ_State |= hasPrecomposedAccent ? AFTER_VOWEL_WITH_PRECOMPOSED_ACCENT
                                                  : AFTER_VOWEL_WITH_COMBINING_ACCENT;
            }
            // Map according to Greek rules.
            UBool ಕೂಡಿಸು_Tonos = ಸುಳ್ಳು;
            ಆದರೆ (upper == 0x397 &&
                    (ದತ್ತಾಂಶ & HAS_ACCENT) != 0 &&
                    ಅಂಕೆ_Ypogegrammeni == 0 &&
                    (state & AFTER_CASED) == 0 &&
                    !isFollowedByCasedLetter(src, ಮುಂದಿನ_ಸೂಚ್ಯಂಕ, src_ಉದ್ದಳತೆ)) {
                // Keep disjunctive "or" with (only) a tonos.
                // We use the same "word boundary" conditions as for the Final_Sigma test.
                ಆದರೆ (hasPrecomposedAccent) {
                    upper = 0x389;  // Preserve the precomposed form.
                } ಇಲ್ಲದಿದ್ದರೆ {
                    ಕೂಡಿಸು_Tonos = ನಿಜ;
                }
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((ದತ್ತಾಂಶ & HAS_DIALYTIKA) != 0) {
                // Preserve a vowel with dialytika in precomposed form if it exists.
                ಆದರೆ (upper == 0x399) {
                    upper = 0x3AA;
                    ದತ್ತಾಂಶ &= ~HAS_EITHER_DIALYTIKA;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (upper == 0x3A5) {
                    upper = 0x3AB;
                    ದತ್ತಾಂಶ &= ~HAS_EITHER_DIALYTIKA;
                }
            }

            UBool change;
            ಆದರೆ (edits == ಶೂನ್ಯಸೂಚಕ && (options & U_OMIT_UNCHANGED_ಪಠ್ಯ) == 0) {
                change = ನಿಜ;  // common, simple usage
            } ಇಲ್ಲದಿದ್ದರೆ {
                // Find out first whether we are changing the text.
                change = src[i] != upper || ಅಂಕೆ_Ypogegrammeni > 0;
                int32_t i2 = i + 1;
                ಆದರೆ ((ದತ್ತಾಂಶ & HAS_EITHER_DIALYTIKA) != 0) {
                    change |= i2 >= ಮುಂದಿನ_ಸೂಚ್ಯಂಕ || src[i2] != 0x308;
                    ++i2;
                }
                ಆದರೆ (ಕೂಡಿಸು_Tonos) {
                    change |= i2 >= ಮುಂದಿನ_ಸೂಚ್ಯಂಕ || src[i2] != 0x301;
                    ++i2;
                }
                int32_t old_ಉದ್ದಳತೆ = ಮುಂದಿನ_ಸೂಚ್ಯಂಕ - i;
                int32_t new_ಉದ್ದಳತೆ = (i2 - i) + ಅಂಕೆ_Ypogegrammeni;
                change |= old_ಉದ್ದಳತೆ != new_ಉದ್ದಳತೆ;
                ಆದರೆ (change) {
                    ಆದರೆ (edits != ಶೂನ್ಯಸೂಚಕ) {
                        edits->ಕೂಡಿಸು_Replace(old_ಉದ್ದಳತೆ, new_ಉದ್ದಳತೆ);
                    }
                } ಇಲ್ಲದಿದ್ದರೆ {
                    ಆದರೆ (edits != ಶೂನ್ಯಸೂಚಕ) {
                        edits->ಕೂಡಿಸು_Unchanged(old_ಉದ್ದಳತೆ);
                    }
                    // Write unchanged text?
                    change = (options & U_OMIT_UNCHANGED_ಪಠ್ಯ) == 0;
                }
            }

            ಆದರೆ (change) {
                dest_ಸೂಚ್ಯಂಕ = appendUChar(dest, dest_ಸೂಚ್ಯಂಕ, dest_ಸಾಮರ್ಥ್ಯ, static_cast<char16_t>(upper));
                ಆದರೆ (dest_ಸೂಚ್ಯಂಕ >= 0 && (ದತ್ತಾಂಶ & HAS_EITHER_DIALYTIKA) != 0) {
                    dest_ಸೂಚ್ಯಂಕ=appendUChar(dest, dest_ಸೂಚ್ಯಂಕ, dest_ಸಾಮರ್ಥ್ಯ, 0x308);  // restore or add a dialytika
                }
                ಆದರೆ (dest_ಸೂಚ್ಯಂಕ >= 0 && ಕೂಡಿಸು_Tonos) {
                    dest_ಸೂಚ್ಯಂಕ=appendUChar(dest, dest_ಸೂಚ್ಯಂಕ, dest_ಸಾಮರ್ಥ್ಯ, 0x301);
                }
                ಆಗ (dest_ಸೂಚ್ಯಂಕ >= 0 && ಅಂಕೆ_Ypogegrammeni > 0) {
                    dest_ಸೂಚ್ಯಂಕ=appendUChar(dest, dest_ಸೂಚ್ಯಂಕ, dest_ಸಾಮರ್ಥ್ಯ, 0x399);
                    --ಅಂಕೆ_Ypogegrammeni;
                }
                ಆದರೆ(dest_ಸೂಚ್ಯಂಕ<0) {
                    error_ಸಂಕೇತ=U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
                    ಹಿಂತಿರುಗಿ 0;
                }
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಸ್ಥಿರಾಂಕ char16_t *s;
            c=ucase_ಗಮ್ಯ_ತುಂಬಿದ_ಊರ್ಧ್ವ(c, ಶೂನ್ಯಸೂಚಕ, ಶೂನ್ಯಸೂಚಕ, &s, UCASE_LOC_GREEK);
            dest_ಸೂಚ್ಯಂಕ = append_ಫಲಿತಾಂಶ(dest, dest_ಸೂಚ್ಯಂಕ, dest_ಸಾಮರ್ಥ್ಯ, c, s,
                                     ಮುಂದಿನ_ಸೂಚ್ಯಂಕ - i, options, edits);
            ಆದರೆ (dest_ಸೂಚ್ಯಂಕ < 0) {
                error_ಸಂಕೇತ = U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
                ಹಿಂತಿರುಗಿ 0;
            }
        }
        i = ಮುಂದಿನ_ಸೂಚ್ಯಂಕ;
        state = ಮುಂದಿನ_State;
    }

    ಹಿಂತಿರುಗಿ dest_ಸೂಚ್ಯಂಕ;
}

}  // namespace GreekUpper
U_NAMESPACE_ಕೊನೆ

/* functions available in the common library (for unistr_case.cpp) */

U_CFUNC int32_t U_CALLCONV
ustrcase_ಆಂತರಿಕ_ಗಮ್ಯ_ಅಧೋ(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_ಬಳಸದ
                         char16_t *dest, int32_t dest_ಸಾಮರ್ಥ್ಯ,
                         ಸ್ಥಿರಾಂಕ char16_t *src, int32_t src_ಉದ್ದಳತೆ,
                         icu::Edits *edits,
                         UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    UCaseContext csc=UCASECONTEXT_INITIALIZER;
    csc.p=(ಶೂನ್ಯ *)src;
    csc.ಎಲ್ಲೆ=src_ಉದ್ದಳತೆ;
    int32_t dest_ಸೂಚ್ಯಂಕ = ಗಮ್ಯ_ಅಧೋ(
        caseLocale, options,
        dest, dest_ಸಾಮರ್ಥ್ಯ,
        src, &csc, 0, src_ಉದ್ದಳತೆ,
        edits, error_ಸಂಕೇತ);
    ಹಿಂತಿರುಗಿ check_ಅತಿಪ್ರವಾಹ_And_Edits_Error(dest_ಸೂಚ್ಯಂಕ, dest_ಸಾಮರ್ಥ್ಯ, edits, error_ಸಂಕೇತ);
}

U_CFUNC int32_t U_CALLCONV
ustrcase_ಆಂತರಿಕ_ಗಮ್ಯ_ಊರ್ಧ್ವ(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_ಬಳಸದ
                         char16_t *dest, int32_t dest_ಸಾಮರ್ಥ್ಯ,
                         ಸ್ಥಿರಾಂಕ char16_t *src, int32_t src_ಉದ್ದಳತೆ,
                         icu::Edits *edits,
                         UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    int32_t dest_ಸೂಚ್ಯಂಕ;
    ಆದರೆ (caseLocale == UCASE_LOC_GREEK) {
        dest_ಸೂಚ್ಯಂಕ = Greek_ಊರ್ಧ್ವ::ಗಮ್ಯ_ಊರ್ಧ್ವ(options, dest, dest_ಸಾಮರ್ಥ್ಯ,
                                        src, src_ಉದ್ದಳತೆ, edits, error_ಸಂಕೇತ);
    } ಇಲ್ಲದಿದ್ದರೆ {
        UCaseContext csc=UCASECONTEXT_INITIALIZER;
        csc.p=(ಶೂನ್ಯ *)src;
        csc.ಎಲ್ಲೆ=src_ಉದ್ದಳತೆ;
        dest_ಸೂಚ್ಯಂಕ = ಗಮ್ಯ_ಊರ್ಧ್ವ(
            caseLocale, options,
            dest, dest_ಸಾಮರ್ಥ್ಯ,
            src, &csc, src_ಉದ್ದಳತೆ,
            edits, error_ಸಂಕೇತ);
    }
    ಹಿಂತಿರುಗಿ check_ಅತಿಪ್ರವಾಹ_And_Edits_Error(dest_ಸೂಚ್ಯಂಕ, dest_ಸಾಮರ್ಥ್ಯ, edits, error_ಸಂಕೇತ);
}

U_CFUNC int32_t U_CALLCONV
ustrcase_ಆಂತರಿಕ_Fold(int32_t /* caseLocale */, uint32_t options, UCASEMAP_BREAK_ITERATOR_ಬಳಸದ
                      char16_t *dest, int32_t dest_ಸಾಮರ್ಥ್ಯ,
                      ಸ್ಥಿರಾಂಕ char16_t *src, int32_t src_ಉದ್ದಳತೆ,
                      icu::Edits *edits,
                      UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    int32_t dest_ಸೂಚ್ಯಂಕ = ಗಮ್ಯ_ಅಧೋ(
        -1, options,
        dest, dest_ಸಾಮರ್ಥ್ಯ,
        src, ಶೂನ್ಯಸೂಚಕ, 0, src_ಉದ್ದಳತೆ,
        edits, error_ಸಂಕೇತ);
    ಹಿಂತಿರುಗಿ check_ಅತಿಪ್ರವಾಹ_And_Edits_Error(dest_ಸೂಚ್ಯಂಕ, dest_ಸಾಮರ್ಥ್ಯ, edits, error_ಸಂಕೇತ);
}

U_CFUNC int32_t
ustrcase_map(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_PARAM
             char16_t *dest, int32_t dest_ಸಾಮರ್ಥ್ಯ,
             ಸ್ಥಿರಾಂಕ char16_t *src, int32_t src_ಉದ್ದಳತೆ,
             UStringCaseMapper *stringCaseMapper,
             icu::Edits *edits,
             UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    int32_t dest_ಉದ್ದಳತೆ;

    /* check argument values */
    ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ 0;
    }
    ಆದರೆ( dest_ಸಾಮರ್ಥ್ಯ<0 ||
        (dest==ಶೂನ್ಯಸೂಚಕ && dest_ಸಾಮರ್ಥ್ಯ>0) ||
        src==ಶೂನ್ಯಸೂಚಕ ||
        src_ಉದ್ದಳತೆ<-1
    ) {
        error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }

    /* get the string length */
    ಆದರೆ(src_ಉದ್ದಳತೆ==-1) {
        src_ಉದ್ದಳತೆ=u_strlen(src);
    }

    /* check for overlapping source and destination */
    ಆದರೆ( dest!=ಶೂನ್ಯಸೂಚಕ &&
        ((src>=dest && src<(dest+dest_ಸಾಮರ್ಥ್ಯ)) ||
         (dest>=src && dest<(src+src_ಉದ್ದಳತೆ)))
    ) {
        error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }

    ಆದರೆ (edits != ಶೂನ್ಯಸೂಚಕ && (options & U_EDITS_NO_RESET) == 0) {
        edits->reset();
    }
    dest_ಉದ್ದಳತೆ=stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                                dest, dest_ಸಾಮರ್ಥ್ಯ, src, src_ಉದ್ದಳತೆ, edits, error_ಸಂಕೇತ);
    ಹಿಂತಿರುಗಿ u_terminateUChars(dest, dest_ಸಾಮರ್ಥ್ಯ, dest_ಉದ್ದಳತೆ, &error_ಸಂಕೇತ);
}

U_CFUNC int32_t
ustrcase_mapWithOverlap(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_PARAM
                        char16_t *dest, int32_t dest_ಸಾಮರ್ಥ್ಯ,
                        ಸ್ಥಿರಾಂಕ char16_t *src, int32_t src_ಉದ್ದಳತೆ,
                        UStringCaseMapper *stringCaseMapper,
                        UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    char16_t ಬಫರ್[300];
    char16_t *ತಾತ್ಕಾಲಿಕ;

    int32_t dest_ಉದ್ದಳತೆ;

    /* check argument values */
    ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ 0;
    }
    ಆದರೆ( dest_ಸಾಮರ್ಥ್ಯ<0 ||
        (dest==ಶೂನ್ಯಸೂಚಕ && dest_ಸಾಮರ್ಥ್ಯ>0) ||
        src==ಶೂನ್ಯಸೂಚಕ ||
        src_ಉದ್ದಳತೆ<-1
    ) {
        error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }

    /* get the string length */
    ಆದರೆ(src_ಉದ್ದಳತೆ==-1) {
        src_ಉದ್ದಳತೆ=u_strlen(src);
    }

    /* check for overlapping source and destination */
    ಆದರೆ( dest!=ಶೂನ್ಯಸೂಚಕ &&
        ((src>=dest && src<(dest+dest_ಸಾಮರ್ಥ್ಯ)) ||
         (dest>=src && dest<(src+src_ಉದ್ದಳತೆ)))
    ) {
        /* overlap: provide a temporary destination buffer and later copy the result */
        ಆದರೆ(dest_ಸಾಮರ್ಥ್ಯ<=UPRV_LENGTHOF(ಬಫರ್)) {
            /* the stack buffer is large enough */
            ತಾತ್ಕಾಲಿಕ=ಬಫರ್;
        } ಇಲ್ಲದಿದ್ದರೆ {
            /* allocate a buffer */
            ತಾತ್ಕಾಲಿಕ=(char16_t *)uprv_malloc(dest_ಸಾಮರ್ಥ್ಯ*U_SIZEOF_UCHAR);
            ಆದರೆ(ತಾತ್ಕಾಲಿಕ==ಶೂನ್ಯಸೂಚಕ) {
                error_ಸಂಕೇತ=U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
                ಹಿಂತಿರುಗಿ 0;
            }
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        ತಾತ್ಕಾಲಿಕ=dest;
    }

    dest_ಉದ್ದಳತೆ=stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                                ತಾತ್ಕಾಲಿಕ, dest_ಸಾಮರ್ಥ್ಯ, src, src_ಉದ್ದಳತೆ, ಶೂನ್ಯಸೂಚಕ, error_ಸಂಕೇತ);
    ಆದರೆ(ತಾತ್ಕಾಲಿಕ!=dest) {
        /* copy the result string to the destination buffer */
        ಆದರೆ (U_SUCCESS(error_ಸಂಕೇತ) && 0 < dest_ಉದ್ದಳತೆ && dest_ಉದ್ದಳತೆ <= dest_ಸಾಮರ್ಥ್ಯ) {
            u_memmove(dest, ತಾತ್ಕಾಲಿಕ, dest_ಉದ್ದಳತೆ);
        }
        ಆದರೆ(ತಾತ್ಕಾಲಿಕ!=ಬಫರ್) {
            uprv_free(ತಾತ್ಕಾಲಿಕ);
        }
    }

    ಹಿಂತಿರುಗಿ u_terminateUChars(dest, dest_ಸಾಮರ್ಥ್ಯ, dest_ಉದ್ದಳತೆ, &error_ಸಂಕೇತ);
}

/* public API functions */

U_CAPI int32_t U_EXPORT2
u_strFoldCase(char16_t *dest, int32_t dest_ಸಾಮರ್ಥ್ಯ,
              ಸ್ಥಿರಾಂಕ char16_t *src, int32_t src_ಉದ್ದಳತೆ,
              uint32_t options,
              UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಹಿಂತಿರುಗಿ ustrcase_mapWithOverlap(
        UCASE_LOC_ಬೇರು, options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_ಸಾಮರ್ಥ್ಯ,
        src, src_ಉದ್ದಳತೆ,
        ustrcase_ಆಂತರಿಕ_Fold, *p_Error_ಸಂಕೇತ);
}

U_NAMESPACE_BEGIN

int32_t CaseMap::fold(
        uint32_t options,
        ಸ್ಥಿರಾಂಕ char16_t *src, int32_t src_ಉದ್ದಳತೆ,
        char16_t *dest, int32_t dest_ಸಾಮರ್ಥ್ಯ, Edits *edits,
        UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಹಿಂತಿರುಗಿ ustrcase_map(
        UCASE_LOC_ಬೇರು, options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_ಸಾಮರ್ಥ್ಯ,
        src, src_ಉದ್ದಳತೆ,
        ustrcase_ಆಂತರಿಕ_Fold, edits, error_ಸಂಕೇತ);
}

U_NAMESPACE_ಕೊನೆ

/* case-insensitive string comparisons -------------------------------------- */

/*
 * This function is a copy of unorm_cmpEquivFold() minus the parts for
 * canonical equivalence.
 * Keep the functions in sync, and see there for how this works.
 * The duplication is for modularization:
 * It makes caseless (but not canonical caseless) matches independent of
 * the normalization code.
 */

/* stack element for previous-level source/decomposition pointers */
ರಚನೆ CmpEquivLevel {
    ಸ್ಥಿರಾಂಕ char16_t *ಆರಂಭ, *s, *ಎಲ್ಲೆ;
};
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ CmpEquivLevel CmpEquivLevel;

/**
 * Internal implementation code comparing string with case fold.
 * This function is called from u_strcmpFold() and u_caseInsensitivePrefixMatch().
 *
 * @param s1            input string 1
 * @param length1       length of string 1, or -1 (NUL terminated)
 * @param s2            input string 2
 * @param length2       length of string 2, or -1 (NUL terminated)
 * @param options       compare options
 * @param matchLen1     (output) length of partial prefix match in s1
 * @param matchLen2     (output) length of partial prefix match in s2
 * @param pErrorCode    receives error status
 * @return The result of comparison
 */
ಸ್ಥಿರ int32_t _cmpFold(
            ಸ್ಥಿರಾಂಕ char16_t *s1, int32_t length1,
            ಸ್ಥಿರಾಂಕ char16_t *s2, int32_t length2,
            uint32_t options,
            int32_t *ಹೊಂದಿಕೆ_Len1, int32_t *ಹೊಂದಿಕೆ_Len2,
            UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    int32_t cmpRes = 0;

    /* current-level start/limit - s1/s2 as current */
    ಸ್ಥಿರಾಂಕ char16_t *start1, *start2, *limit1, *limit2;

    /* points to the original start address */
    ಸ್ಥಿರಾಂಕ char16_t *org1, *org2;

    /* points to the end of match + 1 */
    ಸ್ಥಿರಾಂಕ char16_t *m1, *m2;

    /* case folding variables */
    ಸ್ಥಿರಾಂಕ char16_t *p;
    int32_t length;

    /* stacks of previous-level start/current/limit */
    CmpEquivLevel stack1[2], stack2[2];

    /* case folding buffers, only use current-level start/limit */
    char16_t fold1[UCASE_MAX_STRING_ಉದ್ದಳತೆ+1], fold2[UCASE_MAX_STRING_ಉದ್ದಳತೆ+1];

    /* track which is the current level per string */
    int32_t level1, level2;

    /* current code units, and code points for lookups */
    UChar32 c1, c2, cp1, cp2;

    /* no argument error checking because this itself is not an API */

    /*
     * assume that at least the option U_COMPARE_IGNORE_CASE is set
     * otherwise this function would have to behave exactly as uprv_strCompare()
     */
    ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ 0;
    }

    /* initialize */
    ಆದರೆ(ಹೊಂದಿಕೆ_Len1) {
        U_ASSERT(ಹೊಂದಿಕೆ_Len2 !=ಶೂನ್ಯಸೂಚಕ);
        *ಹೊಂದಿಕೆ_Len1=0;
        *ಹೊಂದಿಕೆ_Len2=0;
    }

    start1=m1=org1=s1;
    ಆದರೆ(length1==-1) {
        limit1=ಶೂನ್ಯಸೂಚಕ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        limit1=s1+length1;
    }

    start2=m2=org2=s2;
    ಆದರೆ(length2==-1) {
        limit2=ಶೂನ್ಯಸೂಚಕ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        limit2=s2+length2;
    }

    level1=level2=0;
    c1=c2=-1;

    /* comparison loop */
    ಗಾಗಿ(;;) {
        /*
         * here a code unit value of -1 means "get another code unit"
         * below it will mean "this source is finished"
         */

        ಆದರೆ(c1<0) {
            /* get next code unit from string 1, post-increment */
            ಗಾಗಿ(;;) {
                ಆದರೆ(s1==limit1 || ((c1=*s1)==0 && (limit1==ಶೂನ್ಯಸೂಚಕ || (options&_STRNCMP_STYLE)))) {
                    ಆದರೆ(level1==0) {
                        c1=-1;
                        ನಿಲ್ಲಿಸು;
                    }
                } ಇಲ್ಲದಿದ್ದರೆ {
                    ++s1;
                    ನಿಲ್ಲಿಸು;
                }

                /* reached end of level buffer, pop one level */
                ಮಾಡು {
                    --level1;
                    start1=stack1[level1].ಆರಂಭ;    /*Not uninitialized*/
                } ಆಗ(start1==ಶೂನ್ಯಸೂಚಕ);
                s1=stack1[level1].s;                /*Not uninitialized*/
                limit1=stack1[level1].ಎಲ್ಲೆ;        /*Not uninitialized*/
            }
        }

        ಆದರೆ(c2<0) {
            /* get next code unit from string 2, post-increment */
            ಗಾಗಿ(;;) {
                ಆದರೆ(s2==limit2 || ((c2=*s2)==0 && (limit2==ಶೂನ್ಯಸೂಚಕ || (options&_STRNCMP_STYLE)))) {
                    ಆದರೆ(level2==0) {
                        c2=-1;
                        ನಿಲ್ಲಿಸು;
                    }
                } ಇಲ್ಲದಿದ್ದರೆ {
                    ++s2;
                    ನಿಲ್ಲಿಸು;
                }

                /* reached end of level buffer, pop one level */
                ಮಾಡು {
                    --level2;
                    start2=stack2[level2].ಆರಂಭ;    /*Not uninitialized*/
                } ಆಗ(start2==ಶೂನ್ಯಸೂಚಕ);
                s2=stack2[level2].s;                /*Not uninitialized*/
                limit2=stack2[level2].ಎಲ್ಲೆ;        /*Not uninitialized*/
            }
        }

        /*
         * compare c1 and c2
         * either variable c1, c2 is -1 only if the corresponding string is finished
         */
        ಆದರೆ(c1==c2) {
            ಸ್ಥಿರಾಂಕ char16_t *next1, *next2;

            ಆದರೆ(c1<0) {
                cmpRes=0;   /* c1==c2==-1 indicating end of strings */
                ನಿಲ್ಲಿಸು;
            }

            /*
             * Note: Move the match positions in both strings at the same time
             *      only when corresponding code point(s) in the original strings
             *      are fully consumed. For example, when comparing s1="Fust" and
             *      s2="Fu\u00dfball", s2[2] is folded into "ss", and s1[2] matches
             *      the first code point in the case-folded data. But the second "s"
             *      has no matching code point in s1, so this implementation returns
             *      2 as the prefix match length ("Fu").
             */
            next1=next2=ಶೂನ್ಯಸೂಚಕ;
            ಆದರೆ(level1==0) {
                next1=s1;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(s1==limit1) {
                /* Note: This implementation only use a single level of stack.
                 *      If this code needs to be changed to use multiple levels
                 *      of stacks, the code above should check if the current
                 *      code is at the end of all stacks.
                 */
                U_ASSERT(level1==1);

                /* is s1 at the end of the current stack? */
                next1=stack1[0].s;
            }

            ಆದರೆ (next1!=ಶೂನ್ಯಸೂಚಕ) {
                ಆದರೆ(level2==0) {
                    next2=s2;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(s2==limit2) {
                    U_ASSERT(level2==1);

                    /* is s2 at the end of the current stack? */
                    next2=stack2[0].s;
                }
                ಆದರೆ(next2!=ಶೂನ್ಯಸೂಚಕ) {
                    m1=next1;
                    m2=next2;
                }
            }
            c1=c2=-1;       /* make us fetch new code units */
            ಮುಂದುವರಿ;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c1<0) {
            cmpRes=-1;      /* string 1 ends before string 2 */
            ನಿಲ್ಲಿಸು;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c2<0) {
            cmpRes=1;       /* string 2 ends before string 1 */
            ನಿಲ್ಲಿಸು;
        }
        /* c1!=c2 && c1>=0 && c2>=0 */

        /* get complete code points for c1, c2 for lookups if either is a surrogate */
        cp1=c1;
        ಆದರೆ(U_IS_SURROGATE(c1)) {
            char16_t c;

            ಆದರೆ(U_IS_SURROGATE_LEAD(c1)) {
                ಆದರೆ(s1!=limit1 && U16_IS_TRAIL(c=*s1)) {
                    /* advance ++s1; only below if cp1 decomposes/case-folds */
                    cp1=U16_GET_SUPPLEMENTARY(c1, c);
                }
            } ಇಲ್ಲದಿದ್ದರೆ /* isTrail(c1) */ {
                ಆದರೆ(start1<=(s1-2) && U16_IS_LEAD(c=*(s1-2))) {
                    cp1=U16_GET_SUPPLEMENTARY(c, c1);
                }
            }
        }

        cp2=c2;
        ಆದರೆ(U_IS_SURROGATE(c2)) {
            char16_t c;

            ಆದರೆ(U_IS_SURROGATE_LEAD(c2)) {
                ಆದರೆ(s2!=limit2 && U16_IS_TRAIL(c=*s2)) {
                    /* advance ++s2; only below if cp2 decomposes/case-folds */
                    cp2=U16_GET_SUPPLEMENTARY(c2, c);
                }
            } ಇಲ್ಲದಿದ್ದರೆ /* isTrail(c2) */ {
                ಆದರೆ(start2<=(s2-2) && U16_IS_LEAD(c=*(s2-2))) {
                    cp2=U16_GET_SUPPLEMENTARY(c, c2);
                }
            }
        }

        /*
         * go down one level for each string
         * continue with the main loop as soon as there is a real change
         */

        ಆದರೆ( level1==0 &&
            (length = ucase_ಗಮ್ಯ_ತುಂಬಿದ_Folding(cp1, &p, options)) >= 0
        ) {
            /* cp1 case-folds to the code point "length" or to p[length] */
            ಆದರೆ(U_IS_SURROGATE(c1)) {
                ಆದರೆ(U_IS_SURROGATE_LEAD(c1)) {
                    /* advance beyond source surrogate pair if it case-folds */
                    ++s1;
                } ಇಲ್ಲದಿದ್ದರೆ /* isTrail(c1) */ {
                    /*
                     * we got a supplementary code point when hitting its trail surrogate,
                     * therefore the lead surrogate must have been the same as in the other string;
                     * compare this decomposition with the lead surrogate in the other string
                     * remember that this simulates bulk text replacement:
                     * the decomposition would replace the entire code point
                     */
                    --s2;
                    --m2;
                    c2=*(s2-1);
                }
            }

            /* push current level pointers */
            stack1[0].ಆರಂಭ=start1;
            stack1[0].s=s1;
            stack1[0].ಎಲ್ಲೆ=limit1;
            ++level1;

            /* copy the folding result to fold1[] */
            ಆದರೆ(length<=UCASE_MAX_STRING_ಉದ್ದಳತೆ) {
                u_memcpy(fold1, p, length);
            } ಇಲ್ಲದಿದ್ದರೆ {
                int32_t i=0;
                U16_APPEND_UNSAFE(fold1, i, length);
                length=i;
            }

            /* set next level pointers to case folding */
            start1=s1=fold1;
            limit1=fold1+length;

            /* get ready to read from decomposition, continue with loop */
            c1=-1;
            ಮುಂದುವರಿ;
        }

        ಆದರೆ( level2==0 &&
            (length = ucase_ಗಮ್ಯ_ತುಂಬಿದ_Folding(cp2, &p, options)) >= 0
        ) {
            /* cp2 case-folds to the code point "length" or to p[length] */
            ಆದರೆ(U_IS_SURROGATE(c2)) {
                ಆದರೆ(U_IS_SURROGATE_LEAD(c2)) {
                    /* advance beyond source surrogate pair if it case-folds */
                    ++s2;
                } ಇಲ್ಲದಿದ್ದರೆ /* isTrail(c2) */ {
                    /*
                     * we got a supplementary code point when hitting its trail surrogate,
                     * therefore the lead surrogate must have been the same as in the other string;
                     * compare this decomposition with the lead surrogate in the other string
                     * remember that this simulates bulk text replacement:
                     * the decomposition would replace the entire code point
                     */
                    --s1;
                    --m2;
                    c1=*(s1-1);
                }
            }

            /* push current level pointers */
            stack2[0].ಆರಂಭ=start2;
            stack2[0].s=s2;
            stack2[0].ಎಲ್ಲೆ=limit2;
            ++level2;

            /* copy the folding result to fold2[] */
            ಆದರೆ(length<=UCASE_MAX_STRING_ಉದ್ದಳತೆ) {
                u_memcpy(fold2, p, length);
            } ಇಲ್ಲದಿದ್ದರೆ {
                int32_t i=0;
                U16_APPEND_UNSAFE(fold2, i, length);
                length=i;
            }

            /* set next level pointers to case folding */
            start2=s2=fold2;
            limit2=fold2+length;

            /* get ready to read from decomposition, continue with loop */
            c2=-1;
            ಮುಂದುವರಿ;
        }

        /*
         * no decomposition/case folding, max level for both sides:
         * return difference result
         *
         * code point order comparison must not just return cp1-cp2
         * because when single surrogates are present then the surrogate pairs
         * that formed cp1 and cp2 may be from different string indexes
         *
         * example: { d800 d800 dc01 } vs. { d800 dc00 }, compare at second code units
         * c1=d800 cp1=10001 c2=dc00 cp2=10000
         * cp1-cp2>0 but c1-c2<0 and in fact in UTF-32 it is { d800 10001 } < { 10000 }
         *
         * therefore, use same fix-up as in ustring.c/uprv_strCompare()
         * except: uprv_strCompare() fetches c=*s while this functions fetches c=*s++
         * so we have slightly different pointer/start/limit comparisons here
         */

        ಆದರೆ(c1>=0xd800 && c2>=0xd800 && (options&U_COMPARE_ಸಂಕೇತ_ಬಿಂದು_ಕ್ರಮ)) {
            /* subtract 0x2800 from BMP code points to make them smaller than supplementary ones */
            ಆದರೆ(
                (c1<=0xdbff && s1!=limit1 && U16_IS_TRAIL(*s1)) ||
                (U16_IS_TRAIL(c1) && start1!=(s1-1) && U16_IS_LEAD(*(s1-2)))
            ) {
                /* part of a surrogate pair, leave >=d800 */
            } ಇಲ್ಲದಿದ್ದರೆ {
                /* BMP code point - may be surrogate code point - make <d800 */
                c1-=0x2800;
            }

            ಆದರೆ(
                (c2<=0xdbff && s2!=limit2 && U16_IS_TRAIL(*s2)) ||
                (U16_IS_TRAIL(c2) && start2!=(s2-1) && U16_IS_LEAD(*(s2-2)))
            ) {
                /* part of a surrogate pair, leave >=d800 */
            } ಇಲ್ಲದಿದ್ದರೆ {
                /* BMP code point - may be surrogate code point - make <d800 */
                c2-=0x2800;
            }
        }

        cmpRes=c1-c2;
        ನಿಲ್ಲಿಸು;
    }

    ಆದರೆ(ಹೊಂದಿಕೆ_Len1) {
        *ಹೊಂದಿಕೆ_Len1=static_cast<int32_t>(m1-org1);
        *ಹೊಂದಿಕೆ_Len2=static_cast<int32_t>(m2-org2);
    }
    ಹಿಂತಿರುಗಿ cmpRes;
}

/* internal function */
U_CFUNC int32_t
u_strcmpFold(ಸ್ಥಿರಾಂಕ char16_t *s1, int32_t length1,
             ಸ್ಥಿರಾಂಕ char16_t *s2, int32_t length2,
             uint32_t options,
             UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಹಿಂತಿರುಗಿ _cmpFold(s1, length1, s2, length2, options, ಶೂನ್ಯಸೂಚಕ, ಶೂನ್ಯಸೂಚಕ, p_Error_ಸಂಕೇತ);
}

/* public API functions */

U_CAPI int32_t U_EXPORT2
u_strCaseCompare(ಸ್ಥಿರಾಂಕ char16_t *s1, int32_t length1,
                 ಸ್ಥಿರಾಂಕ char16_t *s2, int32_t length2,
                 uint32_t options,
                 UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    /* argument checking */
    ಆದರೆ (p_Error_ಸಂಕೇತ == ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ 0;
    }
    ಆದರೆ(s1==ಶೂನ್ಯಸೂಚಕ || length1<-1 || s2==ಶೂನ್ಯಸೂಚಕ || length2<-1) {
        *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }
    ಹಿಂತಿರುಗಿ u_strcmpFold(s1, length1, s2, length2,
                        options|U_COMPARE_IGNORE_CASE,
                        p_Error_ಸಂಕೇತ);
}

U_CAPI int32_t U_EXPORT2
u_strcasecmp(ಸ್ಥಿರಾಂಕ char16_t *s1, ಸ್ಥಿರಾಂಕ char16_t *s2, uint32_t options) {
    UError_ಸಂಕೇತ error_ಸಂಕೇತ=U_ZERO_ERROR;
    ಹಿಂತಿರುಗಿ u_strcmpFold(s1, -1, s2, -1,
                        options|U_COMPARE_IGNORE_CASE,
                        &error_ಸಂಕೇತ);
}

U_CAPI int32_t U_EXPORT2
u_memcasecmp(ಸ್ಥಿರಾಂಕ char16_t *s1, ಸ್ಥಿರಾಂಕ char16_t *s2, int32_t length, uint32_t options) {
    UError_ಸಂಕೇತ error_ಸಂಕೇತ=U_ZERO_ERROR;
    ಹಿಂತಿರುಗಿ u_strcmpFold(s1, length, s2, length,
                        options|U_COMPARE_IGNORE_CASE,
                        &error_ಸಂಕೇತ);
}

U_CAPI int32_t U_EXPORT2
u_strncasecmp(ಸ್ಥಿರಾಂಕ char16_t *s1, ಸ್ಥಿರಾಂಕ char16_t *s2, int32_t n, uint32_t options) {
    UError_ಸಂಕೇತ error_ಸಂಕೇತ=U_ZERO_ERROR;
    ಹಿಂತಿರುಗಿ u_strcmpFold(s1, n, s2, n,
                        options|(U_COMPARE_IGNORE_CASE|_STRNCMP_STYLE),
                        &error_ಸಂಕೇತ);
}

/* internal API - detect length of shared prefix */
U_CAPI ಶೂನ್ಯ
u_case_Insensitive_ಪೂರ್ವಪ್ರತ್ಯಯ_ಹೊಂದಿಕೆ(ಸ್ಥಿರಾಂಕ char16_t *s1, int32_t length1,
                             ಸ್ಥಿರಾಂಕ char16_t *s2, int32_t length2,
                             uint32_t options,
                             int32_t *ಹೊಂದಿಕೆ_Len1, int32_t *ಹೊಂದಿಕೆ_Len2,
                             UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    _cmpFold(s1, length1, s2, length2, options,
        ಹೊಂದಿಕೆ_Len1, ಹೊಂದಿಕೆ_Len2, p_Error_ಸಂಕೇತ);
}
