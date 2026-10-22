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

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/brkiter.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/casemap.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/edits.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/stringoptions.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ustring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ucasemap.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ubrk.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utf.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utf16.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucase.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucasemap_imp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ustr_imp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uassert.h"

/**
 * Code point for COMBINING ACUTE ACCENT
 * @internal
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ACUTE u'\u0301'

U_NAMESPACE_BEGIN

ਨਾਮਾਕਾਸ਼ {

int32_t check_ਉਛਾਲ_And_Edits_Error(int32_t dest_ਸੂਚਕਾਂਕ, int32_t dest_ਸਮਰੱਥਾ,
                                   Edits *edits, UError_ਕੋਡ &error_ਕੋਡ) {
    ਜੇ (U_SUCCESS(error_ਕੋਡ)) {
        ਜੇ (dest_ਸੂਚਕਾਂਕ > dest_ਸਮਰੱਥਾ) {
            error_ਕੋਡ = U_ਬਫਰ_ਉਛਾਲ_ERROR;
        } ਨਹੀਂਤਾਂ ਜੇ (edits != ਖਾਲੀ_ਸੂਚਕ) {
            edits->copy_Error_ਮੰਜ਼ਿਲ(error_ਕੋਡ);
        }
    }
    ਵਾਪਸ dest_ਸੂਚਕਾਂਕ;
}

/* Appends a full case mapping result, see UCASE_MAX_STRING_LENGTH. */
ਸਤਰਅੰਦਰ int32_t
append_ਨਤੀਜਾ(char16_t *dest, int32_t dest_ਸੂਚਕਾਂਕ, int32_t dest_ਸਮਰੱਥਾ,
             int32_t ਨਤੀਜਾ, ਅਚਲ char16_t *s,
             int32_t cp_ਲੰਮਾਈ, uint32_t options, icu::Edits *edits) {
    UChar32 c;
    int32_t length;

    /* decode the result */
    ਜੇ(ਨਤੀਜਾ<0) {
        /* (not) original code point */
        ਜੇ(edits!=ਖਾਲੀ_ਸੂਚਕ) {
            edits->ਜੋੜੋ_Unchanged(cp_ਲੰਮਾਈ);
        }
        ਜੇ(options & U_OMIT_UNCHANGED_ਪਾਠ) {
            ਵਾਪਸ dest_ਸੂਚਕਾਂਕ;
        }
        c=~ਨਤੀਜਾ;
        ਜੇ(dest_ਸੂਚਕਾਂਕ<dest_ਸਮਰੱਥਾ && c<=0xffff) {  // BMP slightly-fastpath
            dest[dest_ਸੂਚਕਾਂਕ++] = static_cast<char16_t>(c);
            ਵਾਪਸ dest_ਸੂਚਕਾਂਕ;
        }
        length=cp_ਲੰਮਾਈ;
    } ਨਹੀਂਤਾਂ {
        ਜੇ(ਨਤੀਜਾ<=UCASE_MAX_STRING_ਲੰਮਾਈ) {
            c=U_SENTINEL;
            length=ਨਤੀਜਾ;
        } ਨਹੀਂਤਾਂ ਜੇ(dest_ਸੂਚਕਾਂਕ<dest_ਸਮਰੱਥਾ && ਨਤੀਜਾ<=0xffff) {  // BMP slightly-fastpath
            dest[dest_ਸੂਚਕਾਂਕ++] = static_cast<char16_t>(ਨਤੀਜਾ);
            ਜੇ(edits!=ਖਾਲੀ_ਸੂਚਕ) {
                edits->ਜੋੜੋ_Replace(cp_ਲੰਮਾਈ, 1);
            }
            ਵਾਪਸ dest_ਸੂਚਕਾਂਕ;
        } ਨਹੀਂਤਾਂ {
            c=ਨਤੀਜਾ;
            length=U16_ਲੰਮਾਈ(c);
        }
        ਜੇ(edits!=ਖਾਲੀ_ਸੂਚਕ) {
            edits->ਜੋੜੋ_Replace(cp_ਲੰਮਾਈ, length);
        }
    }
    ਜੇ(length>(INT32_MAX-dest_ਸੂਚਕਾਂਕ)) {
        ਵਾਪਸ -1;  // integer overflow
    }

    ਜੇ(dest_ਸੂਚਕਾਂਕ<dest_ਸਮਰੱਥਾ) {
        /* append the result */
        ਜੇ(c>=0) {
            /* code point */
            UBool isError=ਝੂਠ;
            U16_APPEND(dest, dest_ਸੂਚਕਾਂਕ, dest_ਸਮਰੱਥਾ, c, isError);
            ਜੇ(isError) {
                /* overflow, nothing written */
                dest_ਸੂਚਕਾਂਕ+=length;
            }
        } ਨਹੀਂਤਾਂ {
            /* string */
            ਜੇ((dest_ਸੂਚਕਾਂਕ+length)<=dest_ਸਮਰੱਥਾ) {
                ਜਦੋਂ(length>0) {
                    dest[dest_ਸੂਚਕਾਂਕ++]=*s++;
                    --length;
                }
            } ਨਹੀਂਤਾਂ {
                /*ਉਛਾਲ*/
                dest_ਸੂਚਕਾਂਕ+=length;
            }
        }
    } ਨਹੀਂਤਾਂ {
        /* preflight */
        dest_ਸੂਚਕਾਂਕ+=length;
    }
    ਵਾਪਸ dest_ਸੂਚਕਾਂਕ;
}

ਸਤਰਅੰਦਰ int32_t
appendUChar(char16_t *dest, int32_t dest_ਸੂਚਕਾਂਕ, int32_t dest_ਸਮਰੱਥਾ, char16_t c) {
    ਜੇ(dest_ਸੂਚਕਾਂਕ<dest_ਸਮਰੱਥਾ) {
        dest[dest_ਸੂਚਕਾਂਕ]=c;
    } ਨਹੀਂਤਾਂ ਜੇ(dest_ਸੂਚਕਾਂਕ==INT32_MAX) {
        ਵਾਪਸ -1;  // integer overflow
    }
    ਵਾਪਸ dest_ਸੂਚਕਾਂਕ+1;
}

int32_t
append_Non_ਖਾਲੀ_ਥਾਵਾਂ_Unchanged(char16_t *dest, int32_t dest_ਸੂਚਕਾਂਕ, int32_t dest_ਸਮਰੱਥਾ,
                        ਅਚਲ char16_t *s, int32_t length, uint32_t options, icu::Edits *edits) {
    ਜੇ(edits!=ਖਾਲੀ_ਸੂਚਕ) {
        edits->ਜੋੜੋ_Unchanged(length);
    }
    ਜੇ(options & U_OMIT_UNCHANGED_ਪਾਠ) {
        ਵਾਪਸ dest_ਸੂਚਕਾਂਕ;
    }
    ਜੇ(length>(INT32_MAX-dest_ਸੂਚਕਾਂਕ)) {
        ਵਾਪਸ -1;  // integer overflow
    }
    ਜੇ((dest_ਸੂਚਕਾਂਕ+length)<=dest_ਸਮਰੱਥਾ) {
        u_memcpy(dest+dest_ਸੂਚਕਾਂਕ, s, length);
    }
    ਵਾਪਸ dest_ਸੂਚਕਾਂਕ + length;
}

ਸਤਰਅੰਦਰ int32_t
appendUnchanged(char16_t *dest, int32_t dest_ਸੂਚਕਾਂਕ, int32_t dest_ਸਮਰੱਥਾ,
                ਅਚਲ char16_t *s, int32_t length, uint32_t options, icu::Edits *edits) {
    ਜੇ (length <= 0) {
        ਵਾਪਸ dest_ਸੂਚਕਾਂਕ;
    }
    ਵਾਪਸ append_Non_ਖਾਲੀ_ਥਾਵਾਂ_Unchanged(dest, dest_ਸੂਚਕਾਂਕ, dest_ਸਮਰੱਥਾ, s, length, options, edits);
}

UChar32 U_CALLCONV
utf16_caseContextIterator(ਖਾਲੀ *context, int8_t dir) {
    UCaseContext* csc = static_cast<UCaseContext*>(context);
    UChar32 c;

    ਜੇ(dir<0) {
        /* reset for backward iteration */
        csc->ਸੂਚਕਾਂਕ=csc->cp_ਸ਼ੁਰੂ;
        csc->dir=dir;
    } ਨਹੀਂਤਾਂ ਜੇ(dir>0) {
        /* reset for forward iteration */
        csc->ਸੂਚਕਾਂਕ=csc->cp_ਹੱਦਬੰਦੀ;
        csc->dir=dir;
    } ਨਹੀਂਤਾਂ {
        /* continue current iteration direction */
        dir=csc->dir;
    }

    ਜੇ(dir<0) {
        ਜੇ(csc->ਸ਼ੁਰੂ<csc->ਸੂਚਕਾਂਕ) {
            U16_PREV((ਅਚਲ char16_t *)csc->p, csc->ਸ਼ੁਰੂ, csc->ਸੂਚਕਾਂਕ, c);
            ਵਾਪਸ c;
        }
    } ਨਹੀਂਤਾਂ {
        ਜੇ(csc->ਸੂਚਕਾਂਕ<csc->ਹੱਦਬੰਦੀ) {
            U16_ਅਗਲਾ((ਅਚਲ char16_t *)csc->p, csc->ਸੂਚਕਾਂਕ, csc->ਹੱਦਬੰਦੀ, c);
            ਵਾਪਸ c;
        }
    }
    ਵਾਪਸ U_SENTINEL;
}

/**
 * caseLocale >= 0: Lowercases [srcStart..srcLimit[ but takes context [0..srcLength[ into account.
 * caseLocale < 0: Case-folds [srcStart..srcLimit[.
 */
int32_t ਮੰਜ਼ਿਲ_ਹੇਠਲਾ(int32_t caseLocale, uint32_t options,
                char16_t *dest, int32_t dest_ਸਮਰੱਥਾ,
                ਅਚਲ char16_t *src, UCaseContext *csc, int32_t src_ਸ਼ੁਰੂ, int32_t src_ਹੱਦਬੰਦੀ,
                icu::Edits *edits, UError_ਕੋਡ &error_ਕੋਡ) {
    ਅਚਲ int8_t *latin_ਮੰਜ਼ਿਲ_ਹੇਠਲਾ;
    ਜੇ (caseLocale == UCASE_LOC_ਜੜ੍ਹ ||
            (caseLocale >= 0 ?
                !(caseLocale == UCASE_LOC_TURKISH || caseLocale == UCASE_LOC_LITHUANIAN) :
                (options & FOLD_CASE_OPTIONS_ਨਕਾਬ) == U_FOLD_CASE_DEFAULT)) {
        latin_ਮੰਜ਼ਿਲ_ਹੇਠਲਾ = LatinCase::ਮੰਜ਼ਿਲ_ਹੇਠਲਾ_ਸਾਧਾਰਨ;
    } ਨਹੀਂਤਾਂ {
        latin_ਮੰਜ਼ਿਲ_ਹੇਠਲਾ = LatinCase::ਮੰਜ਼ਿਲ_ਹੇਠਲਾ_TR_LT;
    }
    ਅਚਲ UTrie2 *trie = ucase_getTrie();
    int32_t dest_ਸੂਚਕਾਂਕ = 0;
    int32_t prev = src_ਸ਼ੁਰੂ;
    int32_t src_ਸੂਚਕਾਂਕ = src_ਸ਼ੁਰੂ;
    ਲਈ (;;) {
        // fast path for simple cases
        char16_t lead = 0;
        ਜਦੋਂ (src_ਸੂਚਕਾਂਕ < src_ਹੱਦਬੰਦੀ) {
            lead = src[src_ਸੂਚਕਾਂਕ];
            int32_t delta;
            ਜੇ (lead < LatinCase::LONG_S) {
                int8_t d = latin_ਮੰਜ਼ਿਲ_ਹੇਠਲਾ[lead];
                ਜੇ (d == LatinCase::EXC) { ਰੋਕੋ; }
                ++src_ਸੂਚਕਾਂਕ;
                ਜੇ (d == 0) { ਜਾਰੀ; }
                delta = d;
            } ਨਹੀਂਤਾਂ ਜੇ (lead >= 0xd800) {
                ਰੋਕੋ;  // surrogate or higher
            } ਨਹੀਂਤਾਂ {
                uint16_t props = UTRIE2_GET16_FROM_U16_SINGLE_LEAD(trie, lead);
                ਜੇ (UCASE_HAS_EXCEPTION(props)) { ਰੋਕੋ; }
                ++src_ਸੂਚਕਾਂਕ;
                ਜੇ (!UCASE_IS_ਉੱਤਲਾ_OR_TITLE(props) || (delta = UCASE_GET_DELTA(props)) == 0) {
                    ਜਾਰੀ;
                }
            }
            lead += static_cast<char16_t>(delta);
            dest_ਸੂਚਕਾਂਕ = appendUnchanged(dest, dest_ਸੂਚਕਾਂਕ, dest_ਸਮਰੱਥਾ,
                                        src + prev, src_ਸੂਚਕਾਂਕ - 1 - prev, options, edits);
            ਜੇ (dest_ਸੂਚਕਾਂਕ >= 0) {
                dest_ਸੂਚਕਾਂਕ = appendUChar(dest, dest_ਸੂਚਕਾਂਕ, dest_ਸਮਰੱਥਾ, lead);
                ਜੇ (edits != ਖਾਲੀ_ਸੂਚਕ) {
                    edits->ਜੋੜੋ_Replace(1, 1);
                }
            }
            ਜੇ (dest_ਸੂਚਕਾਂਕ < 0) {
                error_ਕੋਡ = U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
                ਵਾਪਸ 0;
            }
            prev = src_ਸੂਚਕਾਂਕ;
        }
        ਜੇ (src_ਸੂਚਕਾਂਕ >= src_ਹੱਦਬੰਦੀ) {
            ਰੋਕੋ;
        }
        // slow path
        int32_t cp_ਸ਼ੁਰੂ = src_ਸੂਚਕਾਂਕ++;
        char16_t trail;
        UChar32 c;
        ਜੇ (U16_IS_LEAD(lead) && src_ਸੂਚਕਾਂਕ < src_ਹੱਦਬੰਦੀ && U16_IS_TRAIL(trail = src[src_ਸੂਚਕਾਂਕ])) {
            c = U16_GET_SUPPLEMENTARY(lead, trail);
            ++src_ਸੂਚਕਾਂਕ;
        } ਨਹੀਂਤਾਂ {
            c = lead;
        }
        ਅਚਲ char16_t *s = ਖਾਲੀ_ਸੂਚਕ;
        ਜੇ (caseLocale >= 0) {
            csc->cp_ਸ਼ੁਰੂ = cp_ਸ਼ੁਰੂ;
            csc->cp_ਹੱਦਬੰਦੀ = src_ਸੂਚਕਾਂਕ;
            c = ucase_ਮੰਜ਼ਿਲ_ਭਰਿਆ_ਹੇਠਲਾ(c, utf16_caseContextIterator, csc, &s, caseLocale);
        } ਨਹੀਂਤਾਂ {
            c = ucase_ਮੰਜ਼ਿਲ_ਭਰਿਆ_Folding(c, &s, options);
        }
        ਜੇ (c >= 0) {
            dest_ਸੂਚਕਾਂਕ = appendUnchanged(dest, dest_ਸੂਚਕਾਂਕ, dest_ਸਮਰੱਥਾ,
                                        src + prev, cp_ਸ਼ੁਰੂ - prev, options, edits);
            ਜੇ (dest_ਸੂਚਕਾਂਕ >= 0) {
                dest_ਸੂਚਕਾਂਕ = append_ਨਤੀਜਾ(dest, dest_ਸੂਚਕਾਂਕ, dest_ਸਮਰੱਥਾ, c, s,
                                         src_ਸੂਚਕਾਂਕ - cp_ਸ਼ੁਰੂ, options, edits);
            }
            ਜੇ (dest_ਸੂਚਕਾਂਕ < 0) {
                error_ਕੋਡ = U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
                ਵਾਪਸ 0;
            }
            prev = src_ਸੂਚਕਾਂਕ;
        }
    }
    dest_ਸੂਚਕਾਂਕ = appendUnchanged(dest, dest_ਸੂਚਕਾਂਕ, dest_ਸਮਰੱਥਾ,
                                src + prev, src_ਸੂਚਕਾਂਕ - prev, options, edits);
    ਜੇ (dest_ਸੂਚਕਾਂਕ < 0) {
        error_ਕੋਡ = U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
        ਵਾਪਸ 0;
    }
    ਵਾਪਸ dest_ਸੂਚਕਾਂਕ;
}

int32_t ਮੰਜ਼ਿਲ_ਉੱਤਲਾ(int32_t caseLocale, uint32_t options,
                char16_t *dest, int32_t dest_ਸਮਰੱਥਾ,
                ਅਚਲ char16_t *src, UCaseContext *csc, int32_t src_ਲੰਮਾਈ,
                icu::Edits *edits, UError_ਕੋਡ &error_ਕੋਡ) {
    ਅਚਲ int8_t *latin_ਮੰਜ਼ਿਲ_ਉੱਤਲਾ;
    ਜੇ (caseLocale == UCASE_LOC_TURKISH) {
        latin_ਮੰਜ਼ਿਲ_ਉੱਤਲਾ = LatinCase::ਮੰਜ਼ਿਲ_ਉੱਤਲਾ_TR;
    } ਨਹੀਂਤਾਂ {
        latin_ਮੰਜ਼ਿਲ_ਉੱਤਲਾ = LatinCase::ਮੰਜ਼ਿਲ_ਉੱਤਲਾ_ਸਾਧਾਰਨ;
    }
    ਅਚਲ UTrie2 *trie = ucase_getTrie();
    int32_t dest_ਸੂਚਕਾਂਕ = 0;
    int32_t prev = 0;
    int32_t src_ਸੂਚਕਾਂਕ = 0;
    ਲਈ (;;) {
        // fast path for simple cases
        char16_t lead = 0;
        ਜਦੋਂ (src_ਸੂਚਕਾਂਕ < src_ਲੰਮਾਈ) {
            lead = src[src_ਸੂਚਕਾਂਕ];
            int32_t delta;
            ਜੇ (lead < LatinCase::LONG_S) {
                int8_t d = latin_ਮੰਜ਼ਿਲ_ਉੱਤਲਾ[lead];
                ਜੇ (d == LatinCase::EXC) { ਰੋਕੋ; }
                ++src_ਸੂਚਕਾਂਕ;
                ਜੇ (d == 0) { ਜਾਰੀ; }
                delta = d;
            } ਨਹੀਂਤਾਂ ਜੇ (lead >= 0xd800) {
                ਰੋਕੋ;  // surrogate or higher
            } ਨਹੀਂਤਾਂ {
                uint16_t props = UTRIE2_GET16_FROM_U16_SINGLE_LEAD(trie, lead);
                ਜੇ (UCASE_HAS_EXCEPTION(props)) { ਰੋਕੋ; }
                ++src_ਸੂਚਕਾਂਕ;
                ਜੇ (UCASE_GET_TYPE(props) != UCASE_ਹੇਠਲਾ || (delta = UCASE_GET_DELTA(props)) == 0) {
                    ਜਾਰੀ;
                }
            }
            lead += static_cast<char16_t>(delta);
            dest_ਸੂਚਕਾਂਕ = appendUnchanged(dest, dest_ਸੂਚਕਾਂਕ, dest_ਸਮਰੱਥਾ,
                                        src + prev, src_ਸੂਚਕਾਂਕ - 1 - prev, options, edits);
            ਜੇ (dest_ਸੂਚਕਾਂਕ >= 0) {
                dest_ਸੂਚਕਾਂਕ = appendUChar(dest, dest_ਸੂਚਕਾਂਕ, dest_ਸਮਰੱਥਾ, lead);
                ਜੇ (edits != ਖਾਲੀ_ਸੂਚਕ) {
                    edits->ਜੋੜੋ_Replace(1, 1);
                }
            }
            ਜੇ (dest_ਸੂਚਕਾਂਕ < 0) {
                error_ਕੋਡ = U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
                ਵਾਪਸ 0;
            }
            prev = src_ਸੂਚਕਾਂਕ;
        }
        ਜੇ (src_ਸੂਚਕਾਂਕ >= src_ਲੰਮਾਈ) {
            ਰੋਕੋ;
        }
        // slow path
        int32_t cp_ਸ਼ੁਰੂ;
        csc->cp_ਸ਼ੁਰੂ = cp_ਸ਼ੁਰੂ = src_ਸੂਚਕਾਂਕ++;
        char16_t trail;
        UChar32 c;
        ਜੇ (U16_IS_LEAD(lead) && src_ਸੂਚਕਾਂਕ < src_ਲੰਮਾਈ && U16_IS_TRAIL(trail = src[src_ਸੂਚਕਾਂਕ])) {
            c = U16_GET_SUPPLEMENTARY(lead, trail);
            ++src_ਸੂਚਕਾਂਕ;
        } ਨਹੀਂਤਾਂ {
            c = lead;
        }
        csc->cp_ਹੱਦਬੰਦੀ = src_ਸੂਚਕਾਂਕ;
        ਅਚਲ char16_t *s = ਖਾਲੀ_ਸੂਚਕ;
        c = ucase_ਮੰਜ਼ਿਲ_ਭਰਿਆ_ਉੱਤਲਾ(c, utf16_caseContextIterator, csc, &s, caseLocale);
        ਜੇ (c >= 0) {
            dest_ਸੂਚਕਾਂਕ = appendUnchanged(dest, dest_ਸੂਚਕਾਂਕ, dest_ਸਮਰੱਥਾ,
                                        src + prev, cp_ਸ਼ੁਰੂ - prev, options, edits);
            ਜੇ (dest_ਸੂਚਕਾਂਕ >= 0) {
                dest_ਸੂਚਕਾਂਕ = append_ਨਤੀਜਾ(dest, dest_ਸੂਚਕਾਂਕ, dest_ਸਮਰੱਥਾ, c, s,
                                         src_ਸੂਚਕਾਂਕ - cp_ਸ਼ੁਰੂ, options, edits);
            }
            ਜੇ (dest_ਸੂਚਕਾਂਕ < 0) {
                error_ਕੋਡ = U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
                ਵਾਪਸ 0;
            }
            prev = src_ਸੂਚਕਾਂਕ;
        }
    }
    dest_ਸੂਚਕਾਂਕ = appendUnchanged(dest, dest_ਸੂਚਕਾਂਕ, dest_ਸਮਰੱਥਾ,
                                src + prev, src_ਸੂਚਕਾਂਕ - prev, options, edits);
    ਜੇ (dest_ਸੂਚਕਾਂਕ < 0) {
        error_ਕੋਡ = U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
        ਵਾਪਸ 0;
    }
    ਵਾਪਸ dest_ਸੂਚਕਾਂਕ;
}

}  // namespace

U_NAMESPACE_ਸਿਰਾ

U_NAMESPACE_USE

#ਜੇ !UCONFIG_NO_BREAK_ITERATION

ਨਾਮਾਕਾਸ਼ {

/**
 * Input: c is a letter I with or without acute accent.
 * start is the index in src after c, and is less than segmentLimit.
 * If a plain i/I is followed by a plain j/J,
 * or an i/I with acute (precomposed or decomposed) is followed by a j/J with acute,
 * then we output accordingly.
 *
 * @return the src index after the titlecased sequence, or the start index if no Dutch IJ
 */
int32_t maybeTitleDutchIJ(ਅਚਲ char16_t *src, UChar32 c, int32_t ਸ਼ੁਰੂ, int32_t segment_ਹੱਦਬੰਦੀ,
                          char16_t *dest, int32_t &dest_ਸੂਚਕਾਂਕ, int32_t dest_ਸਮਰੱਥਾ, uint32_t options,
                          icu::Edits *edits) {
    U_ASSERT(ਸ਼ੁਰੂ < segment_ਹੱਦਬੰਦੀ);

    int32_t ਸੂਚਕਾਂਕ = ਸ਼ੁਰੂ;
    ਤਾਰਕਿਕ withAcute = ਝੂਠ;

    // If the conditions are met, then the following variables tell us what to output.
    int32_t unchanged1 = 0;  // code units before the j, or the whole sequence (0..3)
    ਤਾਰਕਿਕ doTitleJ = ਝੂਠ;  // true if the j needs to be titlecased
    int32_t unchanged2 = 0;  // after the j (0 or 1)

    // next character after the first letter
    char16_t c2 = src[ਸੂਚਕਾਂਕ++];

    // Is the first letter an i/I with accent?
    ਜੇ (c == u'I') {
        ਜੇ (c2 == ACUTE) {
            withAcute = ਸੱਚ;
            unchanged1 = 1;
            ਜੇ (ਸੂਚਕਾਂਕ == segment_ਹੱਦਬੰਦੀ) { ਵਾਪਸ ਸ਼ੁਰੂ; }
            c2 = src[ਸੂਚਕਾਂਕ++];
        }
    } ਨਹੀਂਤਾਂ {  // Í
        withAcute = ਸੱਚ;
    }

    // Is the next character a j/J?
    ਜੇ (c2 == u'j') {
        doTitleJ = ਸੱਚ;
    } ਨਹੀਂਤਾਂ ਜੇ (c2 == u'J') {
        ++unchanged1;
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ ਸ਼ੁਰੂ;
    }

    // A plain i/I must be followed by a plain j/J.
    // An i/I with acute must be followed by a j/J with acute.
    ਜੇ (withAcute) {
        ਜੇ (ਸੂਚਕਾਂਕ == segment_ਹੱਦਬੰਦੀ || src[ਸੂਚਕਾਂਕ++] != ACUTE) { ਵਾਪਸ ਸ਼ੁਰੂ; }
        ਜੇ (doTitleJ) {
            unchanged2 = 1;
        } ਨਹੀਂਤਾਂ {
            ++unchanged1;
        }
    }

    // There must not be another combining mark.
    ਜੇ (ਸੂਚਕਾਂਕ < segment_ਹੱਦਬੰਦੀ) {
        int32_t cp;
        int32_t i = ਸੂਚਕਾਂਕ;
        U16_ਅਗਲਾ(src, i, segment_ਹੱਦਬੰਦੀ, cp);
        uint32_t type_ਨਕਾਬ = U_GET_GC_ਨਕਾਬ(cp);
        ਜੇ ((type_ਨਕਾਬ & U_GC_M_ਨਕਾਬ) != 0) {
            ਵਾਪਸ ਸ਼ੁਰੂ;
        }
    }

    // Output the rest of the Dutch IJ.
    dest_ਸੂਚਕਾਂਕ = appendUnchanged(dest, dest_ਸੂਚਕਾਂਕ, dest_ਸਮਰੱਥਾ, src + ਸ਼ੁਰੂ, unchanged1, options, edits);
    ਸ਼ੁਰੂ += unchanged1;
    ਜੇ (doTitleJ) {
        dest_ਸੂਚਕਾਂਕ = appendUChar(dest, dest_ਸੂਚਕਾਂਕ, dest_ਸਮਰੱਥਾ, u'J');
        ਜੇ (edits != ਖਾਲੀ_ਸੂਚਕ) {
            edits->ਜੋੜੋ_Replace(1, 1);
        }
        ++ਸ਼ੁਰੂ;
    }
    dest_ਸੂਚਕਾਂਕ = appendUnchanged(dest, dest_ਸੂਚਕਾਂਕ, dest_ਸਮਰੱਥਾ, src + ਸ਼ੁਰੂ, unchanged2, options, edits);

    U_ASSERT(ਸ਼ੁਰੂ + unchanged2 == ਸੂਚਕਾਂਕ);
    ਵਾਪਸ ਸੂਚਕਾਂਕ;
}

}  // namespace

U_CFUNC int32_t U_CALLCONV
ustrcase_ਅੰਦਰੂਨੀ_ਮੰਜ਼ਿਲ_Title(int32_t caseLocale, uint32_t options, BreakIterator *iter,
                         char16_t *dest, int32_t dest_ਸਮਰੱਥਾ,
                         ਅਚਲ char16_t *src, int32_t src_ਲੰਮਾਈ,
                         icu::Edits *edits,
                         UError_ਕੋਡ &error_ਕੋਡ) {
    ਜੇ (!ustrcase_checkTitleAdjustmentOptions(options, error_ਕੋਡ)) {
        ਵਾਪਸ 0;
    }

    /* set up local variables */
    UCaseContext csc=UCASECONTEXT_INITIALIZER;
    csc.p=(ਖਾਲੀ *)src;
    csc.ਹੱਦਬੰਦੀ=src_ਲੰਮਾਈ;
    int32_t dest_ਸੂਚਕਾਂਕ=0;
    int32_t prev=0;
    ਤਾਰਕਿਕ is_ਪਹਿਲਾ_ਸੂਚਕਾਂਕ=ਸੱਚ;

    /* titlecasing loop */
    ਜਦੋਂ(prev<src_ਲੰਮਾਈ) {
        /* find next index where to titlecase */
        int32_t ਸੂਚਕਾਂਕ;
        ਜੇ(is_ਪਹਿਲਾ_ਸੂਚਕਾਂਕ) {
            is_ਪਹਿਲਾ_ਸੂਚਕਾਂਕ=ਝੂਠ;
            ਸੂਚਕਾਂਕ=iter->first();
        } ਨਹੀਂਤਾਂ {
            ਸੂਚਕਾਂਕ=iter->next();
        }
        ਜੇ(ਸੂਚਕਾਂਕ==UBRK_ਪੂਰਾ || ਸੂਚਕਾਂਕ>src_ਲੰਮਾਈ) {
            ਸੂਚਕਾਂਕ=src_ਲੰਮਾਈ;
        }

        /*
         * Segment [prev..index[ into 3 parts:
         * a) skipped characters (copy as-is) [prev..titleStart[
         * b) first letter (titlecase)              [titleStart..titleLimit[
         * c) subsequent characters (lowercase)                 [titleLimit..index[
         */
        ਜੇ(prev<ਸੂਚਕਾਂਕ) {
            // Find and copy skipped characters [prev..titleStart[
            int32_t title_ਸ਼ੁਰੂ=prev;
            int32_t title_ਹੱਦਬੰਦੀ=prev;
            UChar32 c;
            U16_ਅਗਲਾ(src, title_ਹੱਦਬੰਦੀ, ਸੂਚਕਾਂਕ, c);
            ਜੇ ((options&U_TITLECASE_NO_BREAK_ADJUSTMENT)==0) {
                // Adjust the titlecasing index to the next cased character,
                // or to the next letter/number/symbol/private use.
                // Stop with titleStart<titleLimit<=index
                // if there is a character to be titlecased,
                // or else stop with titleStart==titleLimit==index.
                ਤਾਰਕਿਕ ਮੰਜ਼ਿਲ_Cased = (options&U_TITLECASE_ADJUST_ਮੰਜ਼ਿਲ_CASED) != 0;
                ਜਦੋਂ (ਮੰਜ਼ਿਲ_Cased ? UCASE_NONE==ucase_getType(c) : !ustrcase_isLNS(c)) {
                    title_ਸ਼ੁਰੂ=title_ਹੱਦਬੰਦੀ;
                    ਜੇ(title_ਹੱਦਬੰਦੀ==ਸੂਚਕਾਂਕ) {
                        ਰੋਕੋ;
                    }
                    U16_ਅਗਲਾ(src, title_ਹੱਦਬੰਦੀ, ਸੂਚਕਾਂਕ, c);
                }
                ਜੇ (prev < title_ਸ਼ੁਰੂ) {
                    dest_ਸੂਚਕਾਂਕ=appendUnchanged(dest, dest_ਸੂਚਕਾਂਕ, dest_ਸਮਰੱਥਾ,
                                              src+prev, title_ਸ਼ੁਰੂ-prev, options, edits);
                    ਜੇ(dest_ਸੂਚਕਾਂਕ<0) {
                        error_ਕੋਡ=U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
                        ਵਾਪਸ 0;
                    }
                }
            }

            ਜੇ(title_ਸ਼ੁਰੂ<title_ਹੱਦਬੰਦੀ) {
                /* titlecase c which is from [titleStart..titleLimit[ */
                csc.cp_ਸ਼ੁਰੂ=title_ਸ਼ੁਰੂ;
                csc.cp_ਹੱਦਬੰਦੀ=title_ਹੱਦਬੰਦੀ;
                ਅਚਲ char16_t *s;
                c=ucase_ਮੰਜ਼ਿਲ_ਭਰਿਆ_Title(c, utf16_caseContextIterator, &csc, &s, caseLocale);
                dest_ਸੂਚਕਾਂਕ=append_ਨਤੀਜਾ(dest, dest_ਸੂਚਕਾਂਕ, dest_ਸਮਰੱਥਾ, c, s,
                                       title_ਹੱਦਬੰਦੀ-title_ਸ਼ੁਰੂ, options, edits);
                ਜੇ(dest_ਸੂਚਕਾਂਕ<0) {
                    error_ਕੋਡ=U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
                    ਵਾਪਸ 0;
                }

                /* Special case Dutch IJ titlecasing */
                ਜੇ (title_ਸ਼ੁਰੂ+1 < ਸੂਚਕਾਂਕ &&
                        caseLocale == UCASE_LOC_DUTCH) {
                    ਜੇ (c < 0) {
                        c = ~c;
                    }

                    ਜੇ (c == u'I' || c == u'Í') {
                        title_ਹੱਦਬੰਦੀ = maybeTitleDutchIJ(src, c, title_ਸ਼ੁਰੂ + 1, ਸੂਚਕਾਂਕ,
                                                       dest, dest_ਸੂਚਕਾਂਕ, dest_ਸਮਰੱਥਾ, options,
                                                       edits);
                    }
                }

                /* lowercase [titleLimit..index[ */
                ਜੇ(title_ਹੱਦਬੰਦੀ<ਸੂਚਕਾਂਕ) {
                    ਜੇ((options&U_TITLECASE_NO_LOWERCASE)==0) {
                        /* Normal operation: Lowercase the rest of the word. */
                        dest_ਸੂਚਕਾਂਕ+=
                            ਮੰਜ਼ਿਲ_ਹੇਠਲਾ(
                                caseLocale, options,
                                (dest==ਖਾਲੀ_ਸੂਚਕ) ? ਖਾਲੀ_ਸੂਚਕ: dest+dest_ਸੂਚਕਾਂਕ, dest_ਸਮਰੱਥਾ-dest_ਸੂਚਕਾਂਕ,
                                src, &csc, title_ਹੱਦਬੰਦੀ, ਸੂਚਕਾਂਕ,
                                edits, error_ਕੋਡ);
                        ਜੇ(error_ਕੋਡ==U_ਬਫਰ_ਉਛਾਲ_ERROR) {
                            error_ਕੋਡ=U_ZERO_ERROR;
                        }
                        ਜੇ(U_FAILURE(error_ਕੋਡ)) {
                            ਵਾਪਸ dest_ਸੂਚਕਾਂਕ;
                        }
                    } ਨਹੀਂਤਾਂ {
                        /* Optionally just copy the rest of the word unchanged. */
                        dest_ਸੂਚਕਾਂਕ=appendUnchanged(dest, dest_ਸੂਚਕਾਂਕ, dest_ਸਮਰੱਥਾ,
                                                  src+title_ਹੱਦਬੰਦੀ, ਸੂਚਕਾਂਕ-title_ਹੱਦਬੰਦੀ, options, edits);
                        ਜੇ(dest_ਸੂਚਕਾਂਕ<0) {
                            error_ਕੋਡ=U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
                            ਵਾਪਸ 0;
                        }
                    }
                }
            }
        }

        prev=ਸੂਚਕਾਂਕ;
    }

    ਵਾਪਸ check_ਉਛਾਲ_And_Edits_Error(dest_ਸੂਚਕਾਂਕ, dest_ਸਮਰੱਥਾ, edits, error_ਕੋਡ);
}

#ਜੇ_ਅੰਤ  // !UCONFIG_NO_BREAK_ITERATION

U_NAMESPACE_BEGIN
ਨਾਮਾਕਾਸ਼ Greek_ਉੱਤਲਾ {

// Data generated by prototype code, see
// https://icu.unicode.org/design/case/greek-upper
// TODO: Move this data into ucase.icu.
ਸਥਿਰ ਅਚਲ uint16_t data0370[] = {
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

ਸਥਿਰ ਅਚਲ uint16_t data1F00[] = {
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
ਸਥਿਰ ਅਚਲ uint16_t data2126 = 0x03A9 | HAS_VOWEL;

uint32_t get_Letter_ਅੰਕੜੇ(UChar32 c) {
    ਜੇ (c < 0x370 || 0x2126 < c || (0x3ff < c && c < 0x1f00)) {
        ਵਾਪਸ 0;
    } ਨਹੀਂਤਾਂ ਜੇ (c <= 0x3ff) {
        ਵਾਪਸ data0370[c - 0x370];
    } ਨਹੀਂਤਾਂ ਜੇ (c <= 0x1fff) {
        ਵਾਪਸ data1F00[c - 0x1f00];
    } ਨਹੀਂਤਾਂ ਜੇ (c == 0x2126) {
        ਵਾਪਸ data2126;
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ 0;
    }
}

uint32_t get_Diacritic_ਅੰਕੜੇ(UChar32 c) {
    ਚੋਣ (c) {
    ਹਾਲਤ 0x0300:  // varia
    ਹਾਲਤ 0x0301:  // tonos = oxia
    ਹਾਲਤ 0x0342:  // perispomeni
    ਹਾਲਤ 0x0302:  // circumflex can look like perispomeni
    ਹਾਲਤ 0x0303:  // tilde can look like perispomeni
    ਹਾਲਤ 0x0311:  // inverted breve can look like perispomeni
        ਵਾਪਸ HAS_ACCENT;
    ਹਾਲਤ 0x0308:  // dialytika = diaeresis
        ਵਾਪਸ HAS_COMBINING_DIALYTIKA;
    ਹਾਲਤ 0x0344:  // dialytika tonos
        ਵਾਪਸ HAS_COMBINING_DIALYTIKA | HAS_ACCENT;
    ਹਾਲਤ 0x0345:  // ypogegrammeni = iota subscript
        ਵਾਪਸ HAS_YPOGEGRAMMENI;
    ਹਾਲਤ 0x0304:  // macron
    ਹਾਲਤ 0x0306:  // breve
    ਹਾਲਤ 0x0313:  // comma above
    ਹਾਲਤ 0x0314:  // reversed comma above
    ਹਾਲਤ 0x0343:  // koronis
        ਵਾਪਸ HAS_OTHER_GREEK_DIACRITIC;
    ਮੂਲ:
        ਵਾਪਸ 0;
    }
}

UBool isFollowedByCasedLetter(ਅਚਲ char16_t *s, int32_t i, int32_t length) {
    ਜਦੋਂ (i < length) {
        UChar32 c;
        U16_ਅਗਲਾ(s, i, length, c);
        int32_t type = ucase_getTypeOrIgnorable(c);
        ਜੇ ((type & UCASE_IGNORABLE) != 0) {
            // Case-ignorable, continue with the loop.
        } ਨਹੀਂਤਾਂ ਜੇ (type != UCASE_NONE) {
            ਵਾਪਸ ਸੱਚ;  // Followed by cased letter.
        } ਨਹੀਂਤਾਂ {
            ਵਾਪਸ ਝੂਠ;  // Uncased and not case-ignorable.
        }
    }
    ਵਾਪਸ ਝੂਠ;  // Not followed by cased letter.
}

/**
 * Greek string uppercasing with a state machine.
 * Probably simpler than a stateless function that has to figure out complex context-before
 * for each character.
 * TODO: Try to re-consolidate one way or another with the non-Greek function.
 */
int32_t ਮੰਜ਼ਿਲ_ਉੱਤਲਾ(uint32_t options,
                char16_t *dest, int32_t dest_ਸਮਰੱਥਾ,
                ਅਚਲ char16_t *src, int32_t src_ਲੰਮਾਈ,
                Edits *edits,
                UError_ਕੋਡ &error_ਕੋਡ) {
    int32_t dest_ਸੂਚਕਾਂਕ=0;
    uint32_t state = 0;
    ਲਈ (int32_t i = 0; i < src_ਲੰਮਾਈ;) {
        int32_t ਅਗਲਾ_ਸੂਚਕਾਂਕ = i;
        UChar32 c;
        U16_ਅਗਲਾ(src, ਅਗਲਾ_ਸੂਚਕਾਂਕ, src_ਲੰਮਾਈ, c);
        uint32_t ਅਗਲਾ_State = 0;
        int32_t type = ucase_getTypeOrIgnorable(c);
        ਜੇ ((type & UCASE_IGNORABLE) != 0) {
            // c is case-ignorable
            ਅਗਲਾ_State |= (state & AFTER_CASED);
        } ਨਹੀਂਤਾਂ ਜੇ (type != UCASE_NONE) {
            // c is cased
            ਅਗਲਾ_State |= AFTER_CASED;
        }
        uint32_t ਅੰਕੜੇ = get_Letter_ਅੰਕੜੇ(c);
        ਜੇ (ਅੰਕੜੇ > 0) {
            uint32_t upper = ਅੰਕੜੇ & ਉੱਤਲਾ_ਨਕਾਬ;
            // Add a dialytika to this iota or ypsilon vowel
            // if we removed a tonos from the previous vowel,
            // and that previous vowel did not also have (or gain) a dialytika.
            // Adding one only to the final vowel in a longer sequence
            // (which does not occur in normal writing) would require lookahead.
            // Set the same flag as for preserving an existing dialytika.
            ਜੇ ((ਅੰਕੜੇ & HAS_VOWEL) != 0 &&
                (state & (AFTER_VOWEL_WITH_PRECOMPOSED_ACCENT | AFTER_VOWEL_WITH_COMBINING_ACCENT)) !=
                    0 &&
                (upper == 0x399 || upper == 0x3A5)) {
                ਅੰਕੜੇ |= (state & AFTER_VOWEL_WITH_PRECOMPOSED_ACCENT) ? HAS_DIALYTIKA
                                                                      : HAS_COMBINING_DIALYTIKA;
            }
            int32_t ਅੰਕ_Ypogegrammeni = 0;  // Map each one to a trailing, spacing, capital iota.
            ਜੇ ((ਅੰਕੜੇ & HAS_YPOGEGRAMMENI) != 0) {
                ਅੰਕ_Ypogegrammeni = 1;
            }
            ਅਚਲ UBool hasPrecomposedAccent = (ਅੰਕੜੇ & HAS_ACCENT) != 0;
            // Skip combining diacritics after this Greek letter.
            ਜਦੋਂ (ਅਗਲਾ_ਸੂਚਕਾਂਕ < src_ਲੰਮਾਈ) {
                uint32_t diacritic_ਅੰਕੜੇ = get_Diacritic_ਅੰਕੜੇ(src[ਅਗਲਾ_ਸੂਚਕਾਂਕ]);
                ਜੇ (diacritic_ਅੰਕੜੇ != 0) {
                    ਅੰਕੜੇ |= diacritic_ਅੰਕੜੇ;
                    ਜੇ ((diacritic_ਅੰਕੜੇ & HAS_YPOGEGRAMMENI) != 0) {
                        ++ਅੰਕ_Ypogegrammeni;
                    }
                    ++ਅਗਲਾ_ਸੂਚਕਾਂਕ;
                } ਨਹੀਂਤਾਂ {
                    ਰੋਕੋ;  // not a Greek diacritic
                }
            }
            ਜੇ ((ਅੰਕੜੇ & HAS_VOWEL_AND_ACCENT_AND_DIALYTIKA) == HAS_VOWEL_AND_ACCENT) {
                ਅਗਲਾ_State |= hasPrecomposedAccent ? AFTER_VOWEL_WITH_PRECOMPOSED_ACCENT
                                                  : AFTER_VOWEL_WITH_COMBINING_ACCENT;
            }
            // Map according to Greek rules.
            UBool ਜੋੜੋ_Tonos = ਝੂਠ;
            ਜੇ (upper == 0x397 &&
                    (ਅੰਕੜੇ & HAS_ACCENT) != 0 &&
                    ਅੰਕ_Ypogegrammeni == 0 &&
                    (state & AFTER_CASED) == 0 &&
                    !isFollowedByCasedLetter(src, ਅਗਲਾ_ਸੂਚਕਾਂਕ, src_ਲੰਮਾਈ)) {
                // Keep disjunctive "or" with (only) a tonos.
                // We use the same "word boundary" conditions as for the Final_Sigma test.
                ਜੇ (hasPrecomposedAccent) {
                    upper = 0x389;  // Preserve the precomposed form.
                } ਨਹੀਂਤਾਂ {
                    ਜੋੜੋ_Tonos = ਸੱਚ;
                }
            } ਨਹੀਂਤਾਂ ਜੇ ((ਅੰਕੜੇ & HAS_DIALYTIKA) != 0) {
                // Preserve a vowel with dialytika in precomposed form if it exists.
                ਜੇ (upper == 0x399) {
                    upper = 0x3AA;
                    ਅੰਕੜੇ &= ~HAS_EITHER_DIALYTIKA;
                } ਨਹੀਂਤਾਂ ਜੇ (upper == 0x3A5) {
                    upper = 0x3AB;
                    ਅੰਕੜੇ &= ~HAS_EITHER_DIALYTIKA;
                }
            }

            UBool change;
            ਜੇ (edits == ਖਾਲੀ_ਸੂਚਕ && (options & U_OMIT_UNCHANGED_ਪਾਠ) == 0) {
                change = ਸੱਚ;  // common, simple usage
            } ਨਹੀਂਤਾਂ {
                // Find out first whether we are changing the text.
                change = src[i] != upper || ਅੰਕ_Ypogegrammeni > 0;
                int32_t i2 = i + 1;
                ਜੇ ((ਅੰਕੜੇ & HAS_EITHER_DIALYTIKA) != 0) {
                    change |= i2 >= ਅਗਲਾ_ਸੂਚਕਾਂਕ || src[i2] != 0x308;
                    ++i2;
                }
                ਜੇ (ਜੋੜੋ_Tonos) {
                    change |= i2 >= ਅਗਲਾ_ਸੂਚਕਾਂਕ || src[i2] != 0x301;
                    ++i2;
                }
                int32_t old_ਲੰਮਾਈ = ਅਗਲਾ_ਸੂਚਕਾਂਕ - i;
                int32_t new_ਲੰਮਾਈ = (i2 - i) + ਅੰਕ_Ypogegrammeni;
                change |= old_ਲੰਮਾਈ != new_ਲੰਮਾਈ;
                ਜੇ (change) {
                    ਜੇ (edits != ਖਾਲੀ_ਸੂਚਕ) {
                        edits->ਜੋੜੋ_Replace(old_ਲੰਮਾਈ, new_ਲੰਮਾਈ);
                    }
                } ਨਹੀਂਤਾਂ {
                    ਜੇ (edits != ਖਾਲੀ_ਸੂਚਕ) {
                        edits->ਜੋੜੋ_Unchanged(old_ਲੰਮਾਈ);
                    }
                    // Write unchanged text?
                    change = (options & U_OMIT_UNCHANGED_ਪਾਠ) == 0;
                }
            }

            ਜੇ (change) {
                dest_ਸੂਚਕਾਂਕ = appendUChar(dest, dest_ਸੂਚਕਾਂਕ, dest_ਸਮਰੱਥਾ, static_cast<char16_t>(upper));
                ਜੇ (dest_ਸੂਚਕਾਂਕ >= 0 && (ਅੰਕੜੇ & HAS_EITHER_DIALYTIKA) != 0) {
                    dest_ਸੂਚਕਾਂਕ=appendUChar(dest, dest_ਸੂਚਕਾਂਕ, dest_ਸਮਰੱਥਾ, 0x308);  // restore or add a dialytika
                }
                ਜੇ (dest_ਸੂਚਕਾਂਕ >= 0 && ਜੋੜੋ_Tonos) {
                    dest_ਸੂਚਕਾਂਕ=appendUChar(dest, dest_ਸੂਚਕਾਂਕ, dest_ਸਮਰੱਥਾ, 0x301);
                }
                ਜਦੋਂ (dest_ਸੂਚਕਾਂਕ >= 0 && ਅੰਕ_Ypogegrammeni > 0) {
                    dest_ਸੂਚਕਾਂਕ=appendUChar(dest, dest_ਸੂਚਕਾਂਕ, dest_ਸਮਰੱਥਾ, 0x399);
                    --ਅੰਕ_Ypogegrammeni;
                }
                ਜੇ(dest_ਸੂਚਕਾਂਕ<0) {
                    error_ਕੋਡ=U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
                    ਵਾਪਸ 0;
                }
            }
        } ਨਹੀਂਤਾਂ {
            ਅਚਲ char16_t *s;
            c=ucase_ਮੰਜ਼ਿਲ_ਭਰਿਆ_ਉੱਤਲਾ(c, ਖਾਲੀ_ਸੂਚਕ, ਖਾਲੀ_ਸੂਚਕ, &s, UCASE_LOC_GREEK);
            dest_ਸੂਚਕਾਂਕ = append_ਨਤੀਜਾ(dest, dest_ਸੂਚਕਾਂਕ, dest_ਸਮਰੱਥਾ, c, s,
                                     ਅਗਲਾ_ਸੂਚਕਾਂਕ - i, options, edits);
            ਜੇ (dest_ਸੂਚਕਾਂਕ < 0) {
                error_ਕੋਡ = U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
                ਵਾਪਸ 0;
            }
        }
        i = ਅਗਲਾ_ਸੂਚਕਾਂਕ;
        state = ਅਗਲਾ_State;
    }

    ਵਾਪਸ dest_ਸੂਚਕਾਂਕ;
}

}  // namespace GreekUpper
U_NAMESPACE_ਸਿਰਾ

/* functions available in the common library (for unistr_case.cpp) */

U_CFUNC int32_t U_CALLCONV
ustrcase_ਅੰਦਰੂਨੀ_ਮੰਜ਼ਿਲ_ਹੇਠਲਾ(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_ਅਣਵਰਤਿਆ
                         char16_t *dest, int32_t dest_ਸਮਰੱਥਾ,
                         ਅਚਲ char16_t *src, int32_t src_ਲੰਮਾਈ,
                         icu::Edits *edits,
                         UError_ਕੋਡ &error_ਕੋਡ) {
    UCaseContext csc=UCASECONTEXT_INITIALIZER;
    csc.p=(ਖਾਲੀ *)src;
    csc.ਹੱਦਬੰਦੀ=src_ਲੰਮਾਈ;
    int32_t dest_ਸੂਚਕਾਂਕ = ਮੰਜ਼ਿਲ_ਹੇਠਲਾ(
        caseLocale, options,
        dest, dest_ਸਮਰੱਥਾ,
        src, &csc, 0, src_ਲੰਮਾਈ,
        edits, error_ਕੋਡ);
    ਵਾਪਸ check_ਉਛਾਲ_And_Edits_Error(dest_ਸੂਚਕਾਂਕ, dest_ਸਮਰੱਥਾ, edits, error_ਕੋਡ);
}

U_CFUNC int32_t U_CALLCONV
ustrcase_ਅੰਦਰੂਨੀ_ਮੰਜ਼ਿਲ_ਉੱਤਲਾ(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_ਅਣਵਰਤਿਆ
                         char16_t *dest, int32_t dest_ਸਮਰੱਥਾ,
                         ਅਚਲ char16_t *src, int32_t src_ਲੰਮਾਈ,
                         icu::Edits *edits,
                         UError_ਕੋਡ &error_ਕੋਡ) {
    int32_t dest_ਸੂਚਕਾਂਕ;
    ਜੇ (caseLocale == UCASE_LOC_GREEK) {
        dest_ਸੂਚਕਾਂਕ = Greek_ਉੱਤਲਾ::ਮੰਜ਼ਿਲ_ਉੱਤਲਾ(options, dest, dest_ਸਮਰੱਥਾ,
                                        src, src_ਲੰਮਾਈ, edits, error_ਕੋਡ);
    } ਨਹੀਂਤਾਂ {
        UCaseContext csc=UCASECONTEXT_INITIALIZER;
        csc.p=(ਖਾਲੀ *)src;
        csc.ਹੱਦਬੰਦੀ=src_ਲੰਮਾਈ;
        dest_ਸੂਚਕਾਂਕ = ਮੰਜ਼ਿਲ_ਉੱਤਲਾ(
            caseLocale, options,
            dest, dest_ਸਮਰੱਥਾ,
            src, &csc, src_ਲੰਮਾਈ,
            edits, error_ਕੋਡ);
    }
    ਵਾਪਸ check_ਉਛਾਲ_And_Edits_Error(dest_ਸੂਚਕਾਂਕ, dest_ਸਮਰੱਥਾ, edits, error_ਕੋਡ);
}

U_CFUNC int32_t U_CALLCONV
ustrcase_ਅੰਦਰੂਨੀ_Fold(int32_t /* caseLocale */, uint32_t options, UCASEMAP_BREAK_ITERATOR_ਅਣਵਰਤਿਆ
                      char16_t *dest, int32_t dest_ਸਮਰੱਥਾ,
                      ਅਚਲ char16_t *src, int32_t src_ਲੰਮਾਈ,
                      icu::Edits *edits,
                      UError_ਕੋਡ &error_ਕੋਡ) {
    int32_t dest_ਸੂਚਕਾਂਕ = ਮੰਜ਼ਿਲ_ਹੇਠਲਾ(
        -1, options,
        dest, dest_ਸਮਰੱਥਾ,
        src, ਖਾਲੀ_ਸੂਚਕ, 0, src_ਲੰਮਾਈ,
        edits, error_ਕੋਡ);
    ਵਾਪਸ check_ਉਛਾਲ_And_Edits_Error(dest_ਸੂਚਕਾਂਕ, dest_ਸਮਰੱਥਾ, edits, error_ਕੋਡ);
}

U_CFUNC int32_t
ustrcase_map(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_PARAM
             char16_t *dest, int32_t dest_ਸਮਰੱਥਾ,
             ਅਚਲ char16_t *src, int32_t src_ਲੰਮਾਈ,
             UStringCaseMapper *stringCaseMapper,
             icu::Edits *edits,
             UError_ਕੋਡ &error_ਕੋਡ) {
    int32_t dest_ਲੰਮਾਈ;

    /* check argument values */
    ਜੇ(U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ 0;
    }
    ਜੇ( dest_ਸਮਰੱਥਾ<0 ||
        (dest==ਖਾਲੀ_ਸੂਚਕ && dest_ਸਮਰੱਥਾ>0) ||
        src==ਖਾਲੀ_ਸੂਚਕ ||
        src_ਲੰਮਾਈ<-1
    ) {
        error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ 0;
    }

    /* get the string length */
    ਜੇ(src_ਲੰਮਾਈ==-1) {
        src_ਲੰਮਾਈ=u_strlen(src);
    }

    /* check for overlapping source and destination */
    ਜੇ( dest!=ਖਾਲੀ_ਸੂਚਕ &&
        ((src>=dest && src<(dest+dest_ਸਮਰੱਥਾ)) ||
         (dest>=src && dest<(src+src_ਲੰਮਾਈ)))
    ) {
        error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ 0;
    }

    ਜੇ (edits != ਖਾਲੀ_ਸੂਚਕ && (options & U_EDITS_NO_RESET) == 0) {
        edits->reset();
    }
    dest_ਲੰਮਾਈ=stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                                dest, dest_ਸਮਰੱਥਾ, src, src_ਲੰਮਾਈ, edits, error_ਕੋਡ);
    ਵਾਪਸ u_terminateUChars(dest, dest_ਸਮਰੱਥਾ, dest_ਲੰਮਾਈ, &error_ਕੋਡ);
}

U_CFUNC int32_t
ustrcase_mapWithOverlap(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_PARAM
                        char16_t *dest, int32_t dest_ਸਮਰੱਥਾ,
                        ਅਚਲ char16_t *src, int32_t src_ਲੰਮਾਈ,
                        UStringCaseMapper *stringCaseMapper,
                        UError_ਕੋਡ &error_ਕੋਡ) {
    char16_t ਬਫਰ[300];
    char16_t *ਆਰਜ਼ੀ;

    int32_t dest_ਲੰਮਾਈ;

    /* check argument values */
    ਜੇ(U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ 0;
    }
    ਜੇ( dest_ਸਮਰੱਥਾ<0 ||
        (dest==ਖਾਲੀ_ਸੂਚਕ && dest_ਸਮਰੱਥਾ>0) ||
        src==ਖਾਲੀ_ਸੂਚਕ ||
        src_ਲੰਮਾਈ<-1
    ) {
        error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ 0;
    }

    /* get the string length */
    ਜੇ(src_ਲੰਮਾਈ==-1) {
        src_ਲੰਮਾਈ=u_strlen(src);
    }

    /* check for overlapping source and destination */
    ਜੇ( dest!=ਖਾਲੀ_ਸੂਚਕ &&
        ((src>=dest && src<(dest+dest_ਸਮਰੱਥਾ)) ||
         (dest>=src && dest<(src+src_ਲੰਮਾਈ)))
    ) {
        /* overlap: provide a temporary destination buffer and later copy the result */
        ਜੇ(dest_ਸਮਰੱਥਾ<=UPRV_LENGTHOF(ਬਫਰ)) {
            /* the stack buffer is large enough */
            ਆਰਜ਼ੀ=ਬਫਰ;
        } ਨਹੀਂਤਾਂ {
            /* allocate a buffer */
            ਆਰਜ਼ੀ=(char16_t *)uprv_malloc(dest_ਸਮਰੱਥਾ*U_SIZEOF_UCHAR);
            ਜੇ(ਆਰਜ਼ੀ==ਖਾਲੀ_ਸੂਚਕ) {
                error_ਕੋਡ=U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
                ਵਾਪਸ 0;
            }
        }
    } ਨਹੀਂਤਾਂ {
        ਆਰਜ਼ੀ=dest;
    }

    dest_ਲੰਮਾਈ=stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                                ਆਰਜ਼ੀ, dest_ਸਮਰੱਥਾ, src, src_ਲੰਮਾਈ, ਖਾਲੀ_ਸੂਚਕ, error_ਕੋਡ);
    ਜੇ(ਆਰਜ਼ੀ!=dest) {
        /* copy the result string to the destination buffer */
        ਜੇ (U_SUCCESS(error_ਕੋਡ) && 0 < dest_ਲੰਮਾਈ && dest_ਲੰਮਾਈ <= dest_ਸਮਰੱਥਾ) {
            u_memmove(dest, ਆਰਜ਼ੀ, dest_ਲੰਮਾਈ);
        }
        ਜੇ(ਆਰਜ਼ੀ!=ਬਫਰ) {
            uprv_free(ਆਰਜ਼ੀ);
        }
    }

    ਵਾਪਸ u_terminateUChars(dest, dest_ਸਮਰੱਥਾ, dest_ਲੰਮਾਈ, &error_ਕੋਡ);
}

/* public API functions */

U_CAPI int32_t U_EXPORT2
u_strFoldCase(char16_t *dest, int32_t dest_ਸਮਰੱਥਾ,
              ਅਚਲ char16_t *src, int32_t src_ਲੰਮਾਈ,
              uint32_t options,
              UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਵਾਪਸ ustrcase_mapWithOverlap(
        UCASE_LOC_ਜੜ੍ਹ, options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_ਸਮਰੱਥਾ,
        src, src_ਲੰਮਾਈ,
        ustrcase_ਅੰਦਰੂਨੀ_Fold, *p_Error_ਕੋਡ);
}

U_NAMESPACE_BEGIN

int32_t CaseMap::fold(
        uint32_t options,
        ਅਚਲ char16_t *src, int32_t src_ਲੰਮਾਈ,
        char16_t *dest, int32_t dest_ਸਮਰੱਥਾ, Edits *edits,
        UError_ਕੋਡ &error_ਕੋਡ) {
    ਵਾਪਸ ustrcase_map(
        UCASE_LOC_ਜੜ੍ਹ, options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_ਸਮਰੱਥਾ,
        src, src_ਲੰਮਾਈ,
        ustrcase_ਅੰਦਰੂਨੀ_Fold, edits, error_ਕੋਡ);
}

U_NAMESPACE_ਸਿਰਾ

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
ਬਣਤਰ CmpEquivLevel {
    ਅਚਲ char16_t *ਸ਼ੁਰੂ, *s, *ਹੱਦਬੰਦੀ;
};
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ CmpEquivLevel CmpEquivLevel;

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
ਸਥਿਰ int32_t _cmpFold(
            ਅਚਲ char16_t *s1, int32_t length1,
            ਅਚਲ char16_t *s2, int32_t length2,
            uint32_t options,
            int32_t *ਮੇਲ_Len1, int32_t *ਮੇਲ_Len2,
            UError_ਕੋਡ *p_Error_ਕੋਡ) {
    int32_t cmpRes = 0;

    /* current-level start/limit - s1/s2 as current */
    ਅਚਲ char16_t *start1, *start2, *limit1, *limit2;

    /* points to the original start address */
    ਅਚਲ char16_t *org1, *org2;

    /* points to the end of match + 1 */
    ਅਚਲ char16_t *m1, *m2;

    /* case folding variables */
    ਅਚਲ char16_t *p;
    int32_t length;

    /* stacks of previous-level start/current/limit */
    CmpEquivLevel stack1[2], stack2[2];

    /* case folding buffers, only use current-level start/limit */
    char16_t fold1[UCASE_MAX_STRING_ਲੰਮਾਈ+1], fold2[UCASE_MAX_STRING_ਲੰਮਾਈ+1];

    /* track which is the current level per string */
    int32_t level1, level2;

    /* current code units, and code points for lookups */
    UChar32 c1, c2, cp1, cp2;

    /* no argument error checking because this itself is not an API */

    /*
     * assume that at least the option U_COMPARE_IGNORE_CASE is set
     * otherwise this function would have to behave exactly as uprv_strCompare()
     */
    ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ 0;
    }

    /* initialize */
    ਜੇ(ਮੇਲ_Len1) {
        U_ASSERT(ਮੇਲ_Len2 !=ਖਾਲੀ_ਸੂਚਕ);
        *ਮੇਲ_Len1=0;
        *ਮੇਲ_Len2=0;
    }

    start1=m1=org1=s1;
    ਜੇ(length1==-1) {
        limit1=ਖਾਲੀ_ਸੂਚਕ;
    } ਨਹੀਂਤਾਂ {
        limit1=s1+length1;
    }

    start2=m2=org2=s2;
    ਜੇ(length2==-1) {
        limit2=ਖਾਲੀ_ਸੂਚਕ;
    } ਨਹੀਂਤਾਂ {
        limit2=s2+length2;
    }

    level1=level2=0;
    c1=c2=-1;

    /* comparison loop */
    ਲਈ(;;) {
        /*
         * here a code unit value of -1 means "get another code unit"
         * below it will mean "this source is finished"
         */

        ਜੇ(c1<0) {
            /* get next code unit from string 1, post-increment */
            ਲਈ(;;) {
                ਜੇ(s1==limit1 || ((c1=*s1)==0 && (limit1==ਖਾਲੀ_ਸੂਚਕ || (options&_STRNCMP_STYLE)))) {
                    ਜੇ(level1==0) {
                        c1=-1;
                        ਰੋਕੋ;
                    }
                } ਨਹੀਂਤਾਂ {
                    ++s1;
                    ਰੋਕੋ;
                }

                /* reached end of level buffer, pop one level */
                ਕਰੋ {
                    --level1;
                    start1=stack1[level1].ਸ਼ੁਰੂ;    /*Not uninitialized*/
                } ਜਦੋਂ(start1==ਖਾਲੀ_ਸੂਚਕ);
                s1=stack1[level1].s;                /*Not uninitialized*/
                limit1=stack1[level1].ਹੱਦਬੰਦੀ;        /*Not uninitialized*/
            }
        }

        ਜੇ(c2<0) {
            /* get next code unit from string 2, post-increment */
            ਲਈ(;;) {
                ਜੇ(s2==limit2 || ((c2=*s2)==0 && (limit2==ਖਾਲੀ_ਸੂਚਕ || (options&_STRNCMP_STYLE)))) {
                    ਜੇ(level2==0) {
                        c2=-1;
                        ਰੋਕੋ;
                    }
                } ਨਹੀਂਤਾਂ {
                    ++s2;
                    ਰੋਕੋ;
                }

                /* reached end of level buffer, pop one level */
                ਕਰੋ {
                    --level2;
                    start2=stack2[level2].ਸ਼ੁਰੂ;    /*Not uninitialized*/
                } ਜਦੋਂ(start2==ਖਾਲੀ_ਸੂਚਕ);
                s2=stack2[level2].s;                /*Not uninitialized*/
                limit2=stack2[level2].ਹੱਦਬੰਦੀ;        /*Not uninitialized*/
            }
        }

        /*
         * compare c1 and c2
         * either variable c1, c2 is -1 only if the corresponding string is finished
         */
        ਜੇ(c1==c2) {
            ਅਚਲ char16_t *next1, *next2;

            ਜੇ(c1<0) {
                cmpRes=0;   /* c1==c2==-1 indicating end of strings */
                ਰੋਕੋ;
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
            next1=next2=ਖਾਲੀ_ਸੂਚਕ;
            ਜੇ(level1==0) {
                next1=s1;
            } ਨਹੀਂਤਾਂ ਜੇ(s1==limit1) {
                /* Note: This implementation only use a single level of stack.
                 *      If this code needs to be changed to use multiple levels
                 *      of stacks, the code above should check if the current
                 *      code is at the end of all stacks.
                 */
                U_ASSERT(level1==1);

                /* is s1 at the end of the current stack? */
                next1=stack1[0].s;
            }

            ਜੇ (next1!=ਖਾਲੀ_ਸੂਚਕ) {
                ਜੇ(level2==0) {
                    next2=s2;
                } ਨਹੀਂਤਾਂ ਜੇ(s2==limit2) {
                    U_ASSERT(level2==1);

                    /* is s2 at the end of the current stack? */
                    next2=stack2[0].s;
                }
                ਜੇ(next2!=ਖਾਲੀ_ਸੂਚਕ) {
                    m1=next1;
                    m2=next2;
                }
            }
            c1=c2=-1;       /* make us fetch new code units */
            ਜਾਰੀ;
        } ਨਹੀਂਤਾਂ ਜੇ(c1<0) {
            cmpRes=-1;      /* string 1 ends before string 2 */
            ਰੋਕੋ;
        } ਨਹੀਂਤਾਂ ਜੇ(c2<0) {
            cmpRes=1;       /* string 2 ends before string 1 */
            ਰੋਕੋ;
        }
        /* c1!=c2 && c1>=0 && c2>=0 */

        /* get complete code points for c1, c2 for lookups if either is a surrogate */
        cp1=c1;
        ਜੇ(U_IS_SURROGATE(c1)) {
            char16_t c;

            ਜੇ(U_IS_SURROGATE_LEAD(c1)) {
                ਜੇ(s1!=limit1 && U16_IS_TRAIL(c=*s1)) {
                    /* advance ++s1; only below if cp1 decomposes/case-folds */
                    cp1=U16_GET_SUPPLEMENTARY(c1, c);
                }
            } ਨਹੀਂਤਾਂ /* isTrail(c1) */ {
                ਜੇ(start1<=(s1-2) && U16_IS_LEAD(c=*(s1-2))) {
                    cp1=U16_GET_SUPPLEMENTARY(c, c1);
                }
            }
        }

        cp2=c2;
        ਜੇ(U_IS_SURROGATE(c2)) {
            char16_t c;

            ਜੇ(U_IS_SURROGATE_LEAD(c2)) {
                ਜੇ(s2!=limit2 && U16_IS_TRAIL(c=*s2)) {
                    /* advance ++s2; only below if cp2 decomposes/case-folds */
                    cp2=U16_GET_SUPPLEMENTARY(c2, c);
                }
            } ਨਹੀਂਤਾਂ /* isTrail(c2) */ {
                ਜੇ(start2<=(s2-2) && U16_IS_LEAD(c=*(s2-2))) {
                    cp2=U16_GET_SUPPLEMENTARY(c, c2);
                }
            }
        }

        /*
         * go down one level for each string
         * continue with the main loop as soon as there is a real change
         */

        ਜੇ( level1==0 &&
            (length = ucase_ਮੰਜ਼ਿਲ_ਭਰਿਆ_Folding(cp1, &p, options)) >= 0
        ) {
            /* cp1 case-folds to the code point "length" or to p[length] */
            ਜੇ(U_IS_SURROGATE(c1)) {
                ਜੇ(U_IS_SURROGATE_LEAD(c1)) {
                    /* advance beyond source surrogate pair if it case-folds */
                    ++s1;
                } ਨਹੀਂਤਾਂ /* isTrail(c1) */ {
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
            stack1[0].ਸ਼ੁਰੂ=start1;
            stack1[0].s=s1;
            stack1[0].ਹੱਦਬੰਦੀ=limit1;
            ++level1;

            /* copy the folding result to fold1[] */
            ਜੇ(length<=UCASE_MAX_STRING_ਲੰਮਾਈ) {
                u_memcpy(fold1, p, length);
            } ਨਹੀਂਤਾਂ {
                int32_t i=0;
                U16_APPEND_UNSAFE(fold1, i, length);
                length=i;
            }

            /* set next level pointers to case folding */
            start1=s1=fold1;
            limit1=fold1+length;

            /* get ready to read from decomposition, continue with loop */
            c1=-1;
            ਜਾਰੀ;
        }

        ਜੇ( level2==0 &&
            (length = ucase_ਮੰਜ਼ਿਲ_ਭਰਿਆ_Folding(cp2, &p, options)) >= 0
        ) {
            /* cp2 case-folds to the code point "length" or to p[length] */
            ਜੇ(U_IS_SURROGATE(c2)) {
                ਜੇ(U_IS_SURROGATE_LEAD(c2)) {
                    /* advance beyond source surrogate pair if it case-folds */
                    ++s2;
                } ਨਹੀਂਤਾਂ /* isTrail(c2) */ {
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
            stack2[0].ਸ਼ੁਰੂ=start2;
            stack2[0].s=s2;
            stack2[0].ਹੱਦਬੰਦੀ=limit2;
            ++level2;

            /* copy the folding result to fold2[] */
            ਜੇ(length<=UCASE_MAX_STRING_ਲੰਮਾਈ) {
                u_memcpy(fold2, p, length);
            } ਨਹੀਂਤਾਂ {
                int32_t i=0;
                U16_APPEND_UNSAFE(fold2, i, length);
                length=i;
            }

            /* set next level pointers to case folding */
            start2=s2=fold2;
            limit2=fold2+length;

            /* get ready to read from decomposition, continue with loop */
            c2=-1;
            ਜਾਰੀ;
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

        ਜੇ(c1>=0xd800 && c2>=0xd800 && (options&U_COMPARE_ਕੋਡ_ਬਿੰਦੂ_ਕ੍ਰਮ)) {
            /* subtract 0x2800 from BMP code points to make them smaller than supplementary ones */
            ਜੇ(
                (c1<=0xdbff && s1!=limit1 && U16_IS_TRAIL(*s1)) ||
                (U16_IS_TRAIL(c1) && start1!=(s1-1) && U16_IS_LEAD(*(s1-2)))
            ) {
                /* part of a surrogate pair, leave >=d800 */
            } ਨਹੀਂਤਾਂ {
                /* BMP code point - may be surrogate code point - make <d800 */
                c1-=0x2800;
            }

            ਜੇ(
                (c2<=0xdbff && s2!=limit2 && U16_IS_TRAIL(*s2)) ||
                (U16_IS_TRAIL(c2) && start2!=(s2-1) && U16_IS_LEAD(*(s2-2)))
            ) {
                /* part of a surrogate pair, leave >=d800 */
            } ਨਹੀਂਤਾਂ {
                /* BMP code point - may be surrogate code point - make <d800 */
                c2-=0x2800;
            }
        }

        cmpRes=c1-c2;
        ਰੋਕੋ;
    }

    ਜੇ(ਮੇਲ_Len1) {
        *ਮੇਲ_Len1=static_cast<int32_t>(m1-org1);
        *ਮੇਲ_Len2=static_cast<int32_t>(m2-org2);
    }
    ਵਾਪਸ cmpRes;
}

/* internal function */
U_CFUNC int32_t
u_strcmpFold(ਅਚਲ char16_t *s1, int32_t length1,
             ਅਚਲ char16_t *s2, int32_t length2,
             uint32_t options,
             UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਵਾਪਸ _cmpFold(s1, length1, s2, length2, options, ਖਾਲੀ_ਸੂਚਕ, ਖਾਲੀ_ਸੂਚਕ, p_Error_ਕੋਡ);
}

/* public API functions */

U_CAPI int32_t U_EXPORT2
u_strCaseCompare(ਅਚਲ char16_t *s1, int32_t length1,
                 ਅਚਲ char16_t *s2, int32_t length2,
                 uint32_t options,
                 UError_ਕੋਡ *p_Error_ਕੋਡ) {
    /* argument checking */
    ਜੇ (p_Error_ਕੋਡ == ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ 0;
    }
    ਜੇ(s1==ਖਾਲੀ_ਸੂਚਕ || length1<-1 || s2==ਖਾਲੀ_ਸੂਚਕ || length2<-1) {
        *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ 0;
    }
    ਵਾਪਸ u_strcmpFold(s1, length1, s2, length2,
                        options|U_COMPARE_IGNORE_CASE,
                        p_Error_ਕੋਡ);
}

U_CAPI int32_t U_EXPORT2
u_strcasecmp(ਅਚਲ char16_t *s1, ਅਚਲ char16_t *s2, uint32_t options) {
    UError_ਕੋਡ error_ਕੋਡ=U_ZERO_ERROR;
    ਵਾਪਸ u_strcmpFold(s1, -1, s2, -1,
                        options|U_COMPARE_IGNORE_CASE,
                        &error_ਕੋਡ);
}

U_CAPI int32_t U_EXPORT2
u_memcasecmp(ਅਚਲ char16_t *s1, ਅਚਲ char16_t *s2, int32_t length, uint32_t options) {
    UError_ਕੋਡ error_ਕੋਡ=U_ZERO_ERROR;
    ਵਾਪਸ u_strcmpFold(s1, length, s2, length,
                        options|U_COMPARE_IGNORE_CASE,
                        &error_ਕੋਡ);
}

U_CAPI int32_t U_EXPORT2
u_strncasecmp(ਅਚਲ char16_t *s1, ਅਚਲ char16_t *s2, int32_t n, uint32_t options) {
    UError_ਕੋਡ error_ਕੋਡ=U_ZERO_ERROR;
    ਵਾਪਸ u_strcmpFold(s1, n, s2, n,
                        options|(U_COMPARE_IGNORE_CASE|_STRNCMP_STYLE),
                        &error_ਕੋਡ);
}

/* internal API - detect length of shared prefix */
U_CAPI ਖਾਲੀ
u_case_Insensitive_ਅਗੇਤਰ_ਮੇਲ(ਅਚਲ char16_t *s1, int32_t length1,
                             ਅਚਲ char16_t *s2, int32_t length2,
                             uint32_t options,
                             int32_t *ਮੇਲ_Len1, int32_t *ਮੇਲ_Len2,
                             UError_ਕੋਡ *p_Error_ਕੋਡ) {
    _cmpFold(s1, length1, s2, length2, options,
        ਮੇਲ_Len1, ਮੇਲ_Len2, p_Error_ਕੋਡ);
}
