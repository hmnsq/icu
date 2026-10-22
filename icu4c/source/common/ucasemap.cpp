// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*
*   Copyright (C) 2005-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  ucasemap.cpp
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2005may06
*   created by: Markus W. Scherer
*
*   Case mapping service object and functions using it.
*/

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/brkiter.h"
#ಅನ್ತರ್ಭಾವ "unicode/bytestream.h"
#ಅನ್ತರ್ಭಾವ "unicode/casemap.h"
#ಅನ್ತರ್ಭಾವ "unicode/edits.h"
#ಅನ್ತರ್ಭಾವ "unicode/stringoptions.h"
#ಅನ್ತರ್ಭಾವ "unicode/stringpiece.h"
#ಅನ್ತರ್ಭಾವ "unicode/ubrk.h"
#ಅನ್ತರ್ಭಾವ "unicode/uloc.h"
#ಅನ್ತರ್ಭಾವ "unicode/ustring.h"
#ಅನ್ತರ್ಭಾವ "unicode/ucasemap.h"
#ಆದರೆ !UCONFIG_NO_BREAK_ITERATION
#ಅನ್ತರ್ಭಾವ "unicode/utext.h"
#ಆದರೆ_ಅಂತ್ಯ
#ಅನ್ತರ್ಭಾವ "unicode/utf.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf8.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf16.h"
#ಅನ್ತರ್ಭಾವ "bytesinkutil.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"
#ಅನ್ತರ್ಭಾವ "ucase.h"
#ಅನ್ತರ್ಭಾವ "ucasemap_imp.h"

U_NAMESPACE_USE

/* UCaseMap service object -------------------------------------------------- */

// The public type, unicode/ucasemap.h struct UCaseMap, is an opaque C type.
// We define a parallel C++ type in the ICU namespace for entry point renaming.

U_NAMESPACE_BEGIN

UCaseMapImpl::UCaseMapImpl(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *localeID, uint32_t opts, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) :
#ಆದರೆ !UCONFIG_NO_BREAK_ITERATION
        iter(ಶೂನ್ಯಸೂಚಕ),
#ಆದರೆ_ಅಂತ್ಯ
        caseLocale(UCASE_LOC_UNKNOWN), options(opts) {
    ucasemap_ಗಣ_Locale(reinterpret_cast<UCaseMap *>(ಇದು), localeID, p_Error_ಸಂಕೇತ);
}

UCaseMapImpl::~UCaseMapImpl() {
#ಆದರೆ !UCONFIG_NO_BREAK_ITERATION
    ಅಳಿಸು iter;
#ಆದರೆ_ಅಂತ್ಯ
}

U_NAMESPACE_ಕೊನೆ

U_CAPI UCaseMap * U_EXPORT2
ucasemap_open(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *locale, uint32_t options, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    UCaseMapImpl *csm = ಹೊಸ UCaseMapImpl(locale, options, p_Error_ಸಂಕೇತ);
    ಆದರೆ(csm==ಶೂನ್ಯಸೂಚಕ) {
        *p_Error_ಸಂಕೇತ = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಅಳಿಸು csm;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಹಿಂತಿರುಗಿ reinterpret_cast<UCaseMap *>(csm);
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
ucasemap_close(UCaseMap *csm) {
    ಅಳಿಸು reinterpret_cast<UCaseMapImpl *>(csm);
}

U_CAPI ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* U_EXPORT2
ucasemap_getLocale(ಸ್ಥಿರಾಂಕ UCaseMap* csm U_LIFETIME_BOUND) {
    ಹಿಂತಿರುಗಿ reinterpret_cast<ಸ್ಥಿರಾಂಕ UCaseMapImpl *>(csm)->locale;
}

U_CAPI uint32_t U_EXPORT2
ucasemap_getOptions(ಸ್ಥಿರಾಂಕ UCaseMap *csm) {
    ಹಿಂತಿರುಗಿ reinterpret_cast<ಸ್ಥಿರಾಂಕ UCaseMapImpl *>(csm)->options;
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
ucasemap_ಗಣ_Locale(UCaseMap *csm, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *locale, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ;
    }
    ಸ್ವಯಂಚಾಲಿತ *impl = reinterpret_cast<UCaseMapImpl *>(csm);
    ಆದರೆ (locale != ಶೂನ್ಯಸೂಚಕ && *locale == 0) {
        impl->locale[0] = 0;
        impl->caseLocale = UCASE_LOC_ಬೇರು;
        ಹಿಂತಿರುಗಿ;
    }

    UError_ಸಂಕೇತ ಬಫರ್_ಸ್ಥಿತಿ = U_ZERO_ERROR;
    int32_t length=uloc_getName(locale, impl->locale, (int32_t)ಗಾತ್ರ(impl->locale), &ಬಫರ್_ಸ್ಥಿತಿ);
    ಆದರೆ(ಬಫರ್_ಸ್ಥಿತಿ==U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR || (U_SUCCESS(ಬಫರ್_ಸ್ಥಿತಿ) && length==ಗಾತ್ರ(impl->locale))) {
        ಬಫರ್_ಸ್ಥಿತಿ = U_ZERO_ERROR;
        /* we only really need the language code for case mappings */
        length=uloc_getLanguage(locale, impl->locale, (int32_t)ಗಾತ್ರ(impl->locale), &ಬಫರ್_ಸ್ಥಿತಿ);
    }
    ಆದರೆ(U_FAILURE(ಬಫರ್_ಸ್ಥಿತಿ)) {
        *p_Error_ಸಂಕೇತ=ಬಫರ್_ಸ್ಥಿತಿ;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(length==ಗಾತ್ರ(impl->locale)) {
        *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
    }
    ಆದರೆ(U_SUCCESS(*p_Error_ಸಂಕೇತ)) {     
        impl->caseLocale = ucase_getCaseLocale(impl->locale);
    } ಇಲ್ಲದಿದ್ದರೆ {
        impl->locale[0]=0;
        impl->caseLocale = UCASE_LOC_ಬೇರು;
    }
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
ucasemap_ಗಣ_Options(UCaseMap *csm, uint32_t options, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ;
    }
    reinterpret_cast<UCaseMapImpl *>(csm)->options=options;
}

/* UTF-8 string case mappings ----------------------------------------------- */

/* TODO(markus): Move to a new, separate utf8case.cpp file. */

ನಾಮಾಕಾಶ {

/* append a full case mapping result, see UCASE_MAX_STRING_LENGTH */
ಸಾಲೊಳಗೆ UBool
append_ಫಲಿತಾಂಶ(int32_t cp_ಉದ್ದಳತೆ, int32_t ಫಲಿತಾಂಶ, ಸ್ಥಿರಾಂಕ char16_t *s,
             ByteSink &sink, uint32_t options, icu::Edits *edits, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    U_ASSERT(U_SUCCESS(error_ಸಂಕೇತ));

    /* decode the result */
    ಆದರೆ(ಫಲಿತಾಂಶ<0) {
        /* (not) original code point */
        ಆದರೆ(edits!=ಶೂನ್ಯಸೂಚಕ) {
            edits->ಕೂಡಿಸು_Unchanged(cp_ಉದ್ದಳತೆ);
        }
        ಆದರೆ((options & U_OMIT_UNCHANGED_ಪಠ್ಯ) == 0) {
            ByteSinkUtil::append_ಸಂಕೇತ_ಬಿಂದು(cp_ಉದ್ದಳತೆ, ~ಫಲಿತಾಂಶ, sink);
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಆದರೆ(ಫಲಿತಾಂಶ<=UCASE_MAX_STRING_ಉದ್ದಳತೆ) {
            // string: "result" is the UTF-16 length
            ಹಿಂತಿರುಗಿ ByteSinkUtil::appendChange(cp_ಉದ್ದಳತೆ, s, ಫಲಿತಾಂಶ, sink, edits, error_ಸಂಕೇತ);
        } ಇಲ್ಲದಿದ್ದರೆ {
            ByteSinkUtil::append_ಸಂಕೇತ_ಬಿಂದು(cp_ಉದ್ದಳತೆ, ಫಲಿತಾಂಶ, sink, edits);
        }
    }
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

// See unicode/utf8.h U8_APPEND_UNSAFE().
ಸಾಲೊಳಗೆ uint8_t getTwoByteLead(UChar32 c) { ಹಿಂತಿರುಗಿ static_cast<uint8_t>((c >> 6) | 0xc0); }
ಸಾಲೊಳಗೆ uint8_t getTwoByteTrail(UChar32 c) { ಹಿಂತಿರುಗಿ static_cast<uint8_t>((c & 0x3f) | 0x80); }

UChar32 U_CALLCONV
utf8_caseContextIterator(ಶೂನ್ಯ *context, int8_t dir) {
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
            U8_PREV((ಸ್ಥಿರಾಂಕ uint8_t *)csc->p, csc->ಆರಂಭ, csc->ಸೂಚ್ಯಂಕ, c);
            ಹಿಂತಿರುಗಿ c;
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಆದರೆ(csc->ಸೂಚ್ಯಂಕ<csc->ಎಲ್ಲೆ) {
            U8_ಮುಂದಿನ((ಸ್ಥಿರಾಂಕ uint8_t *)csc->p, csc->ಸೂಚ್ಯಂಕ, csc->ಎಲ್ಲೆ, c);
            ಹಿಂತಿರುಗಿ c;
        }
    }
    ಹಿಂತಿರುಗಿ U_SENTINEL;
}

/**
 * caseLocale >= 0: Lowercases [srcStart..srcLimit[ but takes context [0..srcLength[ into account.
 * caseLocale < 0: Case-folds [srcStart..srcLimit[.
 */
ಶೂನ್ಯ ಗಮ್ಯ_ಅಧೋ(int32_t caseLocale, uint32_t options,
             ಸ್ಥಿರಾಂಕ uint8_t *src, UCaseContext *csc, int32_t src_ಆರಂಭ, int32_t src_ಎಲ್ಲೆ,
             icu::ByteSink &sink, icu::Edits *edits, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
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
    int32_t prev = src_ಆರಂಭ;
    int32_t src_ಸೂಚ್ಯಂಕ = src_ಆರಂಭ;
    ಗಾಗಿ (;;) {
        // fast path for simple cases
        int32_t cp_ಆರಂಭ;
        UChar32 c;
        ಗಾಗಿ (;;) {
            ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ) || src_ಸೂಚ್ಯಂಕ >= src_ಎಲ್ಲೆ) {
                c = U_SENTINEL;
                ನಿಲ್ಲಿಸು;
            }
            uint8_t lead = src[src_ಸೂಚ್ಯಂಕ++];
            ಆದರೆ (lead <= 0x7f) {
                int8_t d = latin_ಗಮ್ಯ_ಅಧೋ[lead];
                ಆದರೆ (d == LatinCase::EXC) {
                    cp_ಆರಂಭ = src_ಸೂಚ್ಯಂಕ - 1;
                    c = lead;
                    ನಿಲ್ಲಿಸು;
                }
                ಆದರೆ (d == 0) { ಮುಂದುವರಿ; }
                ByteSinkUtil::appendUnchanged(src + prev, src_ಸೂಚ್ಯಂಕ - 1 - prev,
                                              sink, options, edits, error_ಸಂಕೇತ);
                ಅಕ್ಷರ ascii = static_cast<ಅಕ್ಷರ>(lead + d);
                sink.Append(&ascii, 1);
                ಆದರೆ (edits != ಶೂನ್ಯಸೂಚಕ) {
                    edits->ಕೂಡಿಸು_Replace(1, 1);
                }
                prev = src_ಸೂಚ್ಯಂಕ;
                ಮುಂದುವರಿ;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (lead < 0xe3) {
                uint8_t t;
                ಆದರೆ (0xc2 <= lead && lead <= 0xc5 && src_ಸೂಚ್ಯಂಕ < src_ಎಲ್ಲೆ &&
                        (t = src[src_ಸೂಚ್ಯಂಕ] - 0x80) <= 0x3f) {
                    // U+0080..U+017F
                    ++src_ಸೂಚ್ಯಂಕ;
                    c = ((lead - 0xc0) << 6) | t;
                    int8_t d = latin_ಗಮ್ಯ_ಅಧೋ[c];
                    ಆದರೆ (d == LatinCase::EXC) {
                        cp_ಆರಂಭ = src_ಸೂಚ್ಯಂಕ - 2;
                        ನಿಲ್ಲಿಸು;
                    }
                    ಆದರೆ (d == 0) { ಮುಂದುವರಿ; }
                    ByteSinkUtil::appendUnchanged(src + prev, src_ಸೂಚ್ಯಂಕ - 2 - prev,
                                                  sink, options, edits, error_ಸಂಕೇತ);
                    ByteSinkUtil::append_Two_ಬೈಟುಗಳು(c + d, sink);
                    ಆದರೆ (edits != ಶೂನ್ಯಸೂಚಕ) {
                        edits->ಕೂಡಿಸು_Replace(2, 2);
                    }
                    prev = src_ಸೂಚ್ಯಂಕ;
                    ಮುಂದುವರಿ;
                }
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((lead <= 0xe9 || lead == 0xeb || lead == 0xec) &&
                    (src_ಸೂಚ್ಯಂಕ + 2) <= src_ಎಲ್ಲೆ &&
                    U8_IS_TRAIL(src[src_ಸೂಚ್ಯಂಕ]) && U8_IS_TRAIL(src[src_ಸೂಚ್ಯಂಕ + 1])) {
                // most of CJK: no case mappings
                src_ಸೂಚ್ಯಂಕ += 2;
                ಮುಂದುವರಿ;
            }
            cp_ಆರಂಭ = --src_ಸೂಚ್ಯಂಕ;
            U8_ಮುಂದಿನ(src, src_ಸೂಚ್ಯಂಕ, src_ಎಲ್ಲೆ, c);
            ಆದರೆ (c < 0) {
                // ill-formed UTF-8
                ಮುಂದುವರಿ;
            }
            uint16_t props = UTRIE2_GET16(trie, c);
            ಆದರೆ (UCASE_HAS_EXCEPTION(props)) { ನಿಲ್ಲಿಸು; }
            int32_t delta;
            ಆದರೆ (!UCASE_IS_ಊರ್ಧ್ವ_OR_TITLE(props) || (delta = UCASE_GET_DELTA(props)) == 0) {
                ಮುಂದುವರಿ;
            }
            ByteSinkUtil::appendUnchanged(src + prev, cp_ಆರಂಭ - prev,
                                          sink, options, edits, error_ಸಂಕೇತ);
            ByteSinkUtil::append_ಸಂಕೇತ_ಬಿಂದು(src_ಸೂಚ್ಯಂಕ - cp_ಆರಂಭ, c + delta, sink, edits);
            prev = src_ಸೂಚ್ಯಂಕ;
        }
        ಆದರೆ (c < 0) {
            ನಿಲ್ಲಿಸು;
        }
        // slow path
        ಸ್ಥಿರಾಂಕ char16_t *s;
        ಆದರೆ (caseLocale >= 0) {
            csc->cp_ಆರಂಭ = cp_ಆರಂಭ;
            csc->cp_ಎಲ್ಲೆ = src_ಸೂಚ್ಯಂಕ;
            c = ucase_ಗಮ್ಯ_ತುಂಬಿದ_ಅಧೋ(c, utf8_caseContextIterator, csc, &s, caseLocale);
        } ಇಲ್ಲದಿದ್ದರೆ {
            c = ucase_ಗಮ್ಯ_ತುಂಬಿದ_Folding(c, &s, options);
        }
        ಆದರೆ (c >= 0) {
            ByteSinkUtil::appendUnchanged(src + prev, cp_ಆರಂಭ - prev,
                                          sink, options, edits, error_ಸಂಕೇತ);
            append_ಫಲಿತಾಂಶ(src_ಸೂಚ್ಯಂಕ - cp_ಆರಂಭ, c, s, sink, options, edits, error_ಸಂಕೇತ);
            prev = src_ಸೂಚ್ಯಂಕ;
        }
    }
    ByteSinkUtil::appendUnchanged(src + prev, src_ಸೂಚ್ಯಂಕ - prev,
                                  sink, options, edits, error_ಸಂಕೇತ);
}

ಶೂನ್ಯ ಗಮ್ಯ_ಊರ್ಧ್ವ(int32_t caseLocale, uint32_t options,
             ಸ್ಥಿರಾಂಕ uint8_t *src, UCaseContext *csc, int32_t src_ಉದ್ದಳತೆ,
             icu::ByteSink &sink, icu::Edits *edits, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಸ್ಥಿರಾಂಕ int8_t *latin_ಗಮ್ಯ_ಊರ್ಧ್ವ;
    ಆದರೆ (caseLocale == UCASE_LOC_TURKISH) {
        latin_ಗಮ್ಯ_ಊರ್ಧ್ವ = LatinCase::ಗಮ್ಯ_ಊರ್ಧ್ವ_TR;
    } ಇಲ್ಲದಿದ್ದರೆ {
        latin_ಗಮ್ಯ_ಊರ್ಧ್ವ = LatinCase::ಗಮ್ಯ_ಊರ್ಧ್ವ_ಸಾಮಾನ್ಯ;
    }
    ಸ್ಥಿರಾಂಕ UTrie2 *trie = ucase_getTrie();
    int32_t prev = 0;
    int32_t src_ಸೂಚ್ಯಂಕ = 0;
    ಗಾಗಿ (;;) {
        // fast path for simple cases
        int32_t cp_ಆರಂಭ;
        UChar32 c;
        ಗಾಗಿ (;;) {
            ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ) || src_ಸೂಚ್ಯಂಕ >= src_ಉದ್ದಳತೆ) {
                c = U_SENTINEL;
                ನಿಲ್ಲಿಸು;
            }
            uint8_t lead = src[src_ಸೂಚ್ಯಂಕ++];
            ಆದರೆ (lead <= 0x7f) {
                int8_t d = latin_ಗಮ್ಯ_ಊರ್ಧ್ವ[lead];
                ಆದರೆ (d == LatinCase::EXC) {
                    cp_ಆರಂಭ = src_ಸೂಚ್ಯಂಕ - 1;
                    c = lead;
                    ನಿಲ್ಲಿಸು;
                }
                ಆದರೆ (d == 0) { ಮುಂದುವರಿ; }
                ByteSinkUtil::appendUnchanged(src + prev, src_ಸೂಚ್ಯಂಕ - 1 - prev,
                                              sink, options, edits, error_ಸಂಕೇತ);
                ಅಕ್ಷರ ascii = static_cast<ಅಕ್ಷರ>(lead + d);
                sink.Append(&ascii, 1);
                ಆದರೆ (edits != ಶೂನ್ಯಸೂಚಕ) {
                    edits->ಕೂಡಿಸು_Replace(1, 1);
                }
                prev = src_ಸೂಚ್ಯಂಕ;
                ಮುಂದುವರಿ;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (lead < 0xe3) {
                uint8_t t;
                ಆದರೆ (0xc2 <= lead && lead <= 0xc5 && src_ಸೂಚ್ಯಂಕ < src_ಉದ್ದಳತೆ &&
                        (t = src[src_ಸೂಚ್ಯಂಕ] - 0x80) <= 0x3f) {
                    // U+0080..U+017F
                    ++src_ಸೂಚ್ಯಂಕ;
                    c = ((lead - 0xc0) << 6) | t;
                    int8_t d = latin_ಗಮ್ಯ_ಊರ್ಧ್ವ[c];
                    ಆದರೆ (d == LatinCase::EXC) {
                        cp_ಆರಂಭ = src_ಸೂಚ್ಯಂಕ - 2;
                        ನಿಲ್ಲಿಸು;
                    }
                    ಆದರೆ (d == 0) { ಮುಂದುವರಿ; }
                    ByteSinkUtil::appendUnchanged(src + prev, src_ಸೂಚ್ಯಂಕ - 2 - prev,
                                                  sink, options, edits, error_ಸಂಕೇತ);
                    ByteSinkUtil::append_Two_ಬೈಟುಗಳು(c + d, sink);
                    ಆದರೆ (edits != ಶೂನ್ಯಸೂಚಕ) {
                        edits->ಕೂಡಿಸು_Replace(2, 2);
                    }
                    prev = src_ಸೂಚ್ಯಂಕ;
                    ಮುಂದುವರಿ;
                }
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((lead <= 0xe9 || lead == 0xeb || lead == 0xec) &&
                    (src_ಸೂಚ್ಯಂಕ + 2) <= src_ಉದ್ದಳತೆ &&
                    U8_IS_TRAIL(src[src_ಸೂಚ್ಯಂಕ]) && U8_IS_TRAIL(src[src_ಸೂಚ್ಯಂಕ + 1])) {
                // most of CJK: no case mappings
                src_ಸೂಚ್ಯಂಕ += 2;
                ಮುಂದುವರಿ;
            }
            cp_ಆರಂಭ = --src_ಸೂಚ್ಯಂಕ;
            U8_ಮುಂದಿನ(src, src_ಸೂಚ್ಯಂಕ, src_ಉದ್ದಳತೆ, c);
            ಆದರೆ (c < 0) {
                // ill-formed UTF-8
                ಮುಂದುವರಿ;
            }
            uint16_t props = UTRIE2_GET16(trie, c);
            ಆದರೆ (UCASE_HAS_EXCEPTION(props)) { ನಿಲ್ಲಿಸು; }
            int32_t delta;
            ಆದರೆ (UCASE_GET_TYPE(props) != UCASE_ಅಧೋ || (delta = UCASE_GET_DELTA(props)) == 0) {
                ಮುಂದುವರಿ;
            }
            ByteSinkUtil::appendUnchanged(src + prev, cp_ಆರಂಭ - prev,
                                          sink, options, edits, error_ಸಂಕೇತ);
            ByteSinkUtil::append_ಸಂಕೇತ_ಬಿಂದು(src_ಸೂಚ್ಯಂಕ - cp_ಆರಂಭ, c + delta, sink, edits);
            prev = src_ಸೂಚ್ಯಂಕ;
        }
        ಆದರೆ (c < 0) {
            ನಿಲ್ಲಿಸು;
        }
        // slow path
        csc->cp_ಆರಂಭ = cp_ಆರಂಭ;
        csc->cp_ಎಲ್ಲೆ = src_ಸೂಚ್ಯಂಕ;
        ಸ್ಥಿರಾಂಕ char16_t *s;
        c = ucase_ಗಮ್ಯ_ತುಂಬಿದ_ಊರ್ಧ್ವ(c, utf8_caseContextIterator, csc, &s, caseLocale);
        ಆದರೆ (c >= 0) {
            ByteSinkUtil::appendUnchanged(src + prev, cp_ಆರಂಭ - prev,
                                          sink, options, edits, error_ಸಂಕೇತ);
            append_ಫಲಿತಾಂಶ(src_ಸೂಚ್ಯಂಕ - cp_ಆರಂಭ, c, s, sink, options, edits, error_ಸಂಕೇತ);
            prev = src_ಸೂಚ್ಯಂಕ;
        }
    }
    ByteSinkUtil::appendUnchanged(src + prev, src_ಸೂಚ್ಯಂಕ - prev,
                                  sink, options, edits, error_ಸಂಕೇತ);
}

}  // namespace

#ಆದರೆ !UCONFIG_NO_BREAK_ITERATION

ನಾಮಾಕಾಶ {

constexpr uint8_t ACUTE_BYTE0 = u8"\u0301"[0];

constexpr uint8_t ACUTE_BYTE1 = u8"\u0301"[1];

/**
 * Input: c is a letter I with or without acute accent.
 * start is the index in src after c, and is less than segmentLimit.
 * If a plain i/I is followed by a plain j/J,
 * or an i/I with acute (precomposed or decomposed) is followed by a j/J with acute,
 * then we output accordingly.
 *
 * @return the src index after the titlecased sequence, or the start index if no Dutch IJ
 */
int32_t maybeTitleDutchIJ(ಸ್ಥಿರಾಂಕ uint8_t *src, UChar32 c, int32_t ಆರಂಭ, int32_t segment_ಎಲ್ಲೆ,
                          ByteSink &sink, uint32_t options, icu::Edits *edits, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    U_ASSERT(ಆರಂಭ < segment_ಎಲ್ಲೆ);

    int32_t ಸೂಚ್ಯಂಕ = ಆರಂಭ;
    ತಾರ್ಕಿಕ withAcute = ಸುಳ್ಳು;

    // If the conditions are met, then the following variables tell us what to output.
    int32_t unchanged1 = 0;  // code units before the j, or the whole sequence (0..3)
    ತಾರ್ಕಿಕ doTitleJ = ಸುಳ್ಳು;  // true if the j needs to be titlecased
    int32_t unchanged2 = 0;  // after the j (0 or 1)

    // next character after the first letter
    UChar32 c2;
    c2 = src[ಸೂಚ್ಯಂಕ++];

    // Is the first letter an i/I with accent?
    ಆದರೆ (c == u'I') {
        ಆದರೆ (c2 == ACUTE_BYTE0 && ಸೂಚ್ಯಂಕ < segment_ಎಲ್ಲೆ && src[ಸೂಚ್ಯಂಕ++] == ACUTE_BYTE1) {
            withAcute = ನಿಜ;
            unchanged1 = 2;  // ACUTE is 2 code units in UTF-8
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
        ಆದರೆ ((ಸೂಚ್ಯಂಕ + 1) >= segment_ಎಲ್ಲೆ || src[ಸೂಚ್ಯಂಕ++] != ACUTE_BYTE0 || src[ಸೂಚ್ಯಂಕ++] != ACUTE_BYTE1) {
            ಹಿಂತಿರುಗಿ ಆರಂಭ;
        }
        ಆದರೆ (doTitleJ) {
            unchanged2 = 2;  // ACUTE is 2 code units in UTF-8
        } ಇಲ್ಲದಿದ್ದರೆ {
            unchanged1 = unchanged1 + 2;    // ACUTE is 2 code units in UTF-8
        }
    }

    // There must not be another combining mark.
    ಆದರೆ (ಸೂಚ್ಯಂಕ < segment_ಎಲ್ಲೆ) {
        int32_t cp;
        int32_t i = ಸೂಚ್ಯಂಕ;
        U8_ಮುಂದಿನ(src, i, segment_ಎಲ್ಲೆ, cp);
        uint32_t type_ಮುಖವಾಡ = U_GET_GC_ಮುಖವಾಡ(cp);
        ಆದರೆ ((type_ಮುಖವಾಡ & U_GC_M_ಮುಖವಾಡ) != 0) {
            ಹಿಂತಿರುಗಿ ಆರಂಭ;
        }
    }

    // Output the rest of the Dutch IJ.
    ByteSinkUtil::appendUnchanged(src + ಆರಂಭ, unchanged1, sink, options, edits, error_ಸಂಕೇತ);
    ಆರಂಭ += unchanged1;
    ಆದರೆ (doTitleJ) {
        ByteSinkUtil::append_ಸಂಕೇತ_ಬಿಂದು(1, u'J', sink, edits);
        ++ಆರಂಭ;
    }
    ByteSinkUtil::appendUnchanged(src + ಆರಂಭ, unchanged2, sink, options, edits, error_ಸಂಕೇತ);

    U_ASSERT(ಆರಂಭ + unchanged2 == ಸೂಚ್ಯಂಕ);
    ಹಿಂತಿರುಗಿ ಸೂಚ್ಯಂಕ;
}

}  // namespace

U_CFUNC ಶೂನ್ಯ U_CALLCONV
ucasemap_ಆಂತರಿಕ_UTF8_ಗಮ್ಯ_Title(
        int32_t caseLocale, uint32_t options, BreakIterator *iter,
        ಸ್ಥಿರಾಂಕ uint8_t *src, int32_t src_ಉದ್ದಳತೆ,
        ByteSink &sink, icu::Edits *edits,
        UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಆದರೆ (!ustrcase_checkTitleAdjustmentOptions(options, error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ;
    }

    /* set up local variables */
    UCaseContext csc=UCASECONTEXT_INITIALIZER;
    csc.p=(ಶೂನ್ಯ *)src;
    csc.ಎಲ್ಲೆ=src_ಉದ್ದಳತೆ;
    int32_t prev=0;
    UBool is_ಮೊದಲ_ಸೂಚ್ಯಂಕ=ನಿಜ;

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
            /* find and copy skipped characters [prev..titleStart[ */
            int32_t title_ಆರಂಭ=prev;
            int32_t title_ಎಲ್ಲೆ=prev;
            UChar32 c;
            U8_ಮುಂದಿನ(src, title_ಎಲ್ಲೆ, ಸೂಚ್ಯಂಕ, c);
            ಆದರೆ ((options&U_TITLECASE_NO_BREAK_ADJUSTMENT)==0) {
                // Adjust the titlecasing index to the next cased character,
                // or to the next letter/number/symbol/private use.
                // Stop with titleStart<titleLimit<=index
                // if there is a character to be titlecased,
                // or else stop with titleStart==titleLimit==index.
                UBool ಗಮ್ಯ_Cased = (options&U_TITLECASE_ADJUST_ಗಮ್ಯ_CASED) != 0;
                ಆಗ (ಗಮ್ಯ_Cased ? UCASE_NONE==ucase_getType(c) : !ustrcase_isLNS(c)) {
                    title_ಆರಂಭ=title_ಎಲ್ಲೆ;
                    ಆದರೆ(title_ಎಲ್ಲೆ==ಸೂಚ್ಯಂಕ) {
                        ನಿಲ್ಲಿಸು;
                    }
                    U8_ಮುಂದಿನ(src, title_ಎಲ್ಲೆ, ಸೂಚ್ಯಂಕ, c);
                }
                ಆದರೆ (prev < title_ಆರಂಭ) {
                    ಆದರೆ (!ByteSinkUtil::appendUnchanged(src+prev, title_ಆರಂಭ-prev,
                                                       sink, options, edits, error_ಸಂಕೇತ)) {
                        ಹಿಂತಿರುಗಿ;
                    }
                }
            }

            ಆದರೆ(title_ಆರಂಭ<title_ಎಲ್ಲೆ) {
                /* titlecase c which is from [titleStart..titleLimit[ */
                ಆದರೆ(c>=0) {
                    csc.cp_ಆರಂಭ=title_ಆರಂಭ;
                    csc.cp_ಎಲ್ಲೆ=title_ಎಲ್ಲೆ;
                    ಸ್ಥಿರಾಂಕ char16_t *s;
                    c=ucase_ಗಮ್ಯ_ತುಂಬಿದ_Title(c, utf8_caseContextIterator, &csc, &s, caseLocale);
                    ಆದರೆ (!append_ಫಲಿತಾಂಶ(title_ಎಲ್ಲೆ-title_ಆರಂಭ, c, s, sink, options, edits, error_ಸಂಕೇತ)) {
                        ಹಿಂತಿರುಗಿ;
                    }
                } ಇಲ್ಲದಿದ್ದರೆ {
                    // Malformed UTF-8.
                    ಆದರೆ (!ByteSinkUtil::appendUnchanged(src+title_ಆರಂಭ, title_ಎಲ್ಲೆ-title_ಆರಂಭ,
                                                       sink, options, edits, error_ಸಂಕೇತ)) {
                        ಹಿಂತಿರುಗಿ;
                    }
                }

                /* Special case Dutch IJ titlecasing */
                ಆದರೆ (title_ಎಲ್ಲೆ < ಸೂಚ್ಯಂಕ &&
                    caseLocale == UCASE_LOC_DUTCH) {
                    ಆದರೆ (c < 0) {
                        c = ~c;
                    }

                    ಆದರೆ (c == u'I' || c == u'Í') {
                        title_ಎಲ್ಲೆ = maybeTitleDutchIJ(src, c, title_ಎಲ್ಲೆ, ಸೂಚ್ಯಂಕ, sink, options, edits, error_ಸಂಕೇತ);
                    }
                }

                /* lowercase [titleLimit..index[ */
                ಆದರೆ(title_ಎಲ್ಲೆ<ಸೂಚ್ಯಂಕ) {
                    ಆದರೆ((options&U_TITLECASE_NO_LOWERCASE)==0) {
                        /* Normal operation: Lowercase the rest of the word. */
                        ಗಮ್ಯ_ಅಧೋ(caseLocale, options,
                                src, &csc, title_ಎಲ್ಲೆ, ಸೂಚ್ಯಂಕ,
                                sink, edits, error_ಸಂಕೇತ);
                        ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
                            ಹಿಂತಿರುಗಿ;
                        }
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        /* Optionally just copy the rest of the word unchanged. */
                        ಆದರೆ (!ByteSinkUtil::appendUnchanged(src+title_ಎಲ್ಲೆ, ಸೂಚ್ಯಂಕ-title_ಎಲ್ಲೆ,
                                                           sink, options, edits, error_ಸಂಕೇತ)) {
                            ಹಿಂತಿರುಗಿ;
                        }
                    }
                }
            }
        }

        prev=ಸೂಚ್ಯಂಕ;
    }
}

#ಆದರೆ_ಅಂತ್ಯ

U_NAMESPACE_BEGIN
ನಾಮಾಕಾಶ Greek_ಊರ್ಧ್ವ {

UBool isFollowedByCasedLetter(ಸ್ಥಿರಾಂಕ uint8_t *s, int32_t i, int32_t length) {
    ಆಗ (i < length) {
        UChar32 c;
        U8_ಮುಂದಿನ(s, i, length, c);
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

// Keep this consistent with the UTF-16 version in ustrcase.cpp and the Java version in CaseMap.java.
ಶೂನ್ಯ ಗಮ್ಯ_ಊರ್ಧ್ವ(uint32_t options,
             ಸ್ಥಿರಾಂಕ uint8_t *src, int32_t src_ಉದ್ದಳತೆ,
             ByteSink &sink, Edits *edits,
             UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    uint32_t state = 0;
    ಗಾಗಿ (int32_t i = 0; i < src_ಉದ್ದಳತೆ;) {
        int32_t ಮುಂದಿನ_ಸೂಚ್ಯಂಕ = i;
        UChar32 c;
        U8_ಮುಂದಿನ(src, ಮುಂದಿನ_ಸೂಚ್ಯಂಕ, src_ಉದ್ದಳತೆ, c);
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
                ದತ್ತಾಂಶ |= (state & AFTER_VOWEL_WITH_PRECOMPOSED_ACCENT) != 0 ? HAS_DIALYTIKA
                                                                           : HAS_COMBINING_DIALYTIKA;
            }
            int32_t ಅಂಕೆ_Ypogegrammeni = 0;  // Map each one to a trailing, spacing, capital iota.
            ಆದರೆ ((ದತ್ತಾಂಶ & HAS_YPOGEGRAMMENI) != 0) {
                ಅಂಕೆ_Ypogegrammeni = 1;
            }
            ಸ್ಥಿರಾಂಕ UBool hasPrecomposedAccent = (ದತ್ತಾಂಶ & HAS_ACCENT) != 0;
            // Skip combining diacritics after this Greek letter.
            int32_t ಮುಂದಿನ_ಮುಂದಿನ_ಸೂಚ್ಯಂಕ = ಮುಂದಿನ_ಸೂಚ್ಯಂಕ;
            ಆಗ (ಮುಂದಿನ_ಸೂಚ್ಯಂಕ < src_ಉದ್ದಳತೆ) {
                UChar32 c2;
                U8_ಮುಂದಿನ(src, ಮುಂದಿನ_ಮುಂದಿನ_ಸೂಚ್ಯಂಕ, src_ಉದ್ದಳತೆ, c2);
                uint32_t diacritic_ದತ್ತಾಂಶ = get_Diacritic_ದತ್ತಾಂಶ(c2);
                ಆದರೆ (diacritic_ದತ್ತಾಂಶ != 0) {
                    ದತ್ತಾಂಶ |= diacritic_ದತ್ತಾಂಶ;
                    ಆದರೆ ((diacritic_ದತ್ತಾಂಶ & HAS_YPOGEGRAMMENI) != 0) {
                        ++ಅಂಕೆ_Ypogegrammeni;
                    }
                    ಮುಂದಿನ_ಸೂಚ್ಯಂಕ = ಮುಂದಿನ_ಮುಂದಿನ_ಸೂಚ್ಯಂಕ;
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
                U_ASSERT(0x370 <= upper && upper <= 0x3ff);  // 2-byte UTF-8, main Greek block
                change = (i + 2) > ಮುಂದಿನ_ಸೂಚ್ಯಂಕ ||
                        src[i] != getTwoByteLead(upper) || src[i + 1] != getTwoByteTrail(upper) ||
                        ಅಂಕೆ_Ypogegrammeni > 0;
                int32_t i2 = i + 2;
                ಆದರೆ ((ದತ್ತಾಂಶ & HAS_EITHER_DIALYTIKA) != 0) {
                    change |= (i2 + 2) > ಮುಂದಿನ_ಸೂಚ್ಯಂಕ ||
                            src[i2] != static_cast<uint8_t>(u8"\u0308"[0]) ||
                            src[i2 + 1] != static_cast<uint8_t>(u8"\u0308"[1]);
                    i2 += 2;
                }
                ಆದರೆ (ಕೂಡಿಸು_Tonos) {
                    change |= (i2 + 2) > ಮುಂದಿನ_ಸೂಚ್ಯಂಕ ||
                            src[i2] != static_cast<uint8_t>(u8"\u0301"[0]) ||
                            src[i2 + 1] != static_cast<uint8_t>(u8"\u0301"[1]);
                    i2 += 2;
                }
                int32_t old_ಉದ್ದಳತೆ = ಮುಂದಿನ_ಸೂಚ್ಯಂಕ - i;
                int32_t new_ಉದ್ದಳತೆ = (i2 - i) + ಅಂಕೆ_Ypogegrammeni * 2;  // 2 bytes per U+0399
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
                ByteSinkUtil::append_Two_ಬೈಟುಗಳು(upper, sink);
                ಆದರೆ ((ದತ್ತಾಂಶ & HAS_EITHER_DIALYTIKA) != 0) {
                    sink.AppendU8(u8"\u0308", 2);  // restore or add a dialytika
                }
                ಆದರೆ (ಕೂಡಿಸು_Tonos) {
                    sink.AppendU8(u8"\u0301", 2);
                }
                ಆಗ (ಅಂಕೆ_Ypogegrammeni > 0) {
                    sink.AppendU8(u8"\u0399", 2);
                    --ಅಂಕೆ_Ypogegrammeni;
                }
            }
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c>=0) {
            ಸ್ಥಿರಾಂಕ char16_t *s;
            c=ucase_ಗಮ್ಯ_ತುಂಬಿದ_ಊರ್ಧ್ವ(c, ಶೂನ್ಯಸೂಚಕ, ಶೂನ್ಯಸೂಚಕ, &s, UCASE_LOC_GREEK);
            ಆದರೆ (!append_ಫಲಿತಾಂಶ(ಮುಂದಿನ_ಸೂಚ್ಯಂಕ - i, c, s, sink, options, edits, error_ಸಂಕೇತ)) {
                ಹಿಂತಿರುಗಿ;
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            // Malformed UTF-8.
            ಆದರೆ (!ByteSinkUtil::appendUnchanged(src+i, ಮುಂದಿನ_ಸೂಚ್ಯಂಕ-i,
                                               sink, options, edits, error_ಸಂಕೇತ)) {
                ಹಿಂತಿರುಗಿ;
            }
        }
        i = ಮುಂದಿನ_ಸೂಚ್ಯಂಕ;
        state = ಮುಂದಿನ_State;
    }
}

}  // namespace GreekUpper
U_NAMESPACE_ಕೊನೆ

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
ucasemap_ಆಂತರಿಕ_UTF8_ಗಮ್ಯ_ಅಧೋ(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_ಬಳಸದ
                             ಸ್ಥಿರಾಂಕ uint8_t *src, int32_t src_ಉದ್ದಳತೆ,
                             icu::ByteSink &sink, icu::Edits *edits,
                             UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    UCaseContext csc=UCASECONTEXT_INITIALIZER;
    csc.p=(ಶೂನ್ಯ *)src;
    csc.ಎಲ್ಲೆ=src_ಉದ್ದಳತೆ;
    ಗಮ್ಯ_ಅಧೋ(
        caseLocale, options,
        src, &csc, 0, src_ಉದ್ದಳತೆ,
        sink, edits, error_ಸಂಕೇತ);
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
ucasemap_ಆಂತರಿಕ_UTF8_ಗಮ್ಯ_ಊರ್ಧ್ವ(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_ಬಳಸದ
                             ಸ್ಥಿರಾಂಕ uint8_t *src, int32_t src_ಉದ್ದಳತೆ,
                             icu::ByteSink &sink, icu::Edits *edits,
                             UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಆದರೆ (caseLocale == UCASE_LOC_GREEK) {
        Greek_ಊರ್ಧ್ವ::ಗಮ್ಯ_ಊರ್ಧ್ವ(options, src, src_ಉದ್ದಳತೆ, sink, edits, error_ಸಂಕೇತ);
    } ಇಲ್ಲದಿದ್ದರೆ {
        UCaseContext csc=UCASECONTEXT_INITIALIZER;
        csc.p=(ಶೂನ್ಯ *)src;
        csc.ಎಲ್ಲೆ=src_ಉದ್ದಳತೆ;
        ಗಮ್ಯ_ಊರ್ಧ್ವ(
            caseLocale, options,
            src, &csc, src_ಉದ್ದಳತೆ,
            sink, edits, error_ಸಂಕೇತ);
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
ucasemap_ಆಂತರಿಕ_UTF8_Fold(int32_t /* caseLocale */, uint32_t options, UCASEMAP_BREAK_ITERATOR_ಬಳಸದ
                          ಸ್ಥಿರಾಂಕ uint8_t *src, int32_t src_ಉದ್ದಳತೆ,
                          icu::ByteSink &sink, icu::Edits *edits,
                          UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಗಮ್ಯ_ಅಧೋ(
        -1, options,
        src, ಶೂನ್ಯಸೂಚಕ, 0, src_ಉದ್ದಳತೆ,
        sink, edits, error_ಸಂಕೇತ);
}

ಶೂನ್ಯ
ucasemap_mapUTF8(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_PARAM
                 ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *src, int32_t src_ಉದ್ದಳತೆ,
                 UTF8CaseMapper *stringCaseMapper,
                 icu::ByteSink &sink, icu::Edits *edits,
                 UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    /* check argument values */
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ ((src == ಶೂನ್ಯಸೂಚಕ && src_ಉದ್ದಳತೆ != 0) || src_ಉದ್ದಳತೆ < -1) {
        error_ಸಂಕೇತ = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }

    // Get the string length.
    ಆದರೆ (src_ಉದ್ದಳತೆ == -1) {
        src_ಉದ್ದಳತೆ = static_cast<int32_t>(uprv_strlen(src));
    }

    ಆದರೆ (edits != ಶೂನ್ಯಸೂಚಕ && (options & U_EDITS_NO_RESET) == 0) {
        edits->reset();
    }
    stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                     reinterpret_cast<ಸ್ಥಿರಾಂಕ uint8_t*>(src), src_ಉದ್ದಳತೆ, sink, edits, error_ಸಂಕೇತ);
    sink.Flush();
    ಆದರೆ (U_SUCCESS(error_ಸಂಕೇತ)) {
        ಆದರೆ (edits != ಶೂನ್ಯಸೂಚಕ) {
            edits->copy_Error_ಗಮ್ಯ(error_ಸಂಕೇತ);
        }
    }
}

int32_t
ucasemap_mapUTF8(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_PARAM
                 ಅಕ್ಷರ *dest, int32_t dest_ಸಾಮರ್ಥ್ಯ,
                 ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *src, int32_t src_ಉದ್ದಳತೆ,
                 UTF8CaseMapper *stringCaseMapper,
                 icu::Edits *edits,
                 UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    /* check argument values */
    ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ 0;
    }
    ಆದರೆ( dest_ಸಾಮರ್ಥ್ಯ<0 ||
        (dest==ಶೂನ್ಯಸೂಚಕ && dest_ಸಾಮರ್ಥ್ಯ>0) ||
        (src==ಶೂನ್ಯಸೂಚಕ && src_ಉದ್ದಳತೆ!=0) || src_ಉದ್ದಳತೆ<-1
    ) {
        error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }

    /* get the string length */
    ಆದರೆ(src_ಉದ್ದಳತೆ==-1) {
        src_ಉದ್ದಳತೆ = static_cast<int32_t>(uprv_strlen(src));
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
    int32_t reslen = ByteSinkUtil::ಮೂಲಕ_Byte_Sink_ಗಮ್ಯ_Terminated_Chars(
        dest, dest_ಸಾಮರ್ಥ್ಯ,
        [&](ByteSink& sink, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) {
            stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                             reinterpret_cast<ಸ್ಥಿರಾಂಕ uint8_t*>(src), src_ಉದ್ದಳತೆ, sink, edits, ಸ್ಥಿತಿ_2);
        },
        error_ಸಂಕೇತ);
    ಆದರೆ (U_SUCCESS(error_ಸಂಕೇತ) && edits != ಶೂನ್ಯಸೂಚಕ) {
        edits->copy_Error_ಗಮ್ಯ(error_ಸಂಕೇತ);
    }
    ಹಿಂತಿರುಗಿ reslen;
}

/* public API functions */

U_CAPI int32_t U_EXPORT2
ucasemap_utf8_ಗಮ್ಯ_ಅಧೋ(ಸ್ಥಿರಾಂಕ UCaseMap *csm,
                     ಅಕ್ಷರ *dest, int32_t dest_ಸಾಮರ್ಥ್ಯ,
                     ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *src, int32_t src_ಉದ್ದಳತೆ,
                     UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಸ್ವಯಂಚಾಲಿತ *impl = reinterpret_cast<ಸ್ಥಿರಾಂಕ UCaseMapImpl *>(csm);
    ಹಿಂತಿರುಗಿ ucasemap_mapUTF8(
        impl->caseLocale, impl->options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_ಸಾಮರ್ಥ್ಯ,
        src, src_ಉದ್ದಳತೆ,
        ucasemap_ಆಂತರಿಕ_UTF8_ಗಮ್ಯ_ಅಧೋ, ಶೂನ್ಯಸೂಚಕ, *p_Error_ಸಂಕೇತ);
}

U_CAPI int32_t U_EXPORT2
ucasemap_utf8_ಗಮ್ಯ_ಊರ್ಧ್ವ(ಸ್ಥಿರಾಂಕ UCaseMap *csm,
                     ಅಕ್ಷರ *dest, int32_t dest_ಸಾಮರ್ಥ್ಯ,
                     ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *src, int32_t src_ಉದ್ದಳತೆ,
                     UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಸ್ವಯಂಚಾಲಿತ *impl = reinterpret_cast<ಸ್ಥಿರಾಂಕ UCaseMapImpl *>(csm);
    ಹಿಂತಿರುಗಿ ucasemap_mapUTF8(
        impl->caseLocale, impl->options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_ಸಾಮರ್ಥ್ಯ,
        src, src_ಉದ್ದಳತೆ,
        ucasemap_ಆಂತರಿಕ_UTF8_ಗಮ್ಯ_ಊರ್ಧ್ವ, ಶೂನ್ಯಸೂಚಕ, *p_Error_ಸಂಕೇತ);
}

U_CAPI int32_t U_EXPORT2
ucasemap_utf8FoldCase(ಸ್ಥಿರಾಂಕ UCaseMap *csm,
                      ಅಕ್ಷರ *dest, int32_t dest_ಸಾಮರ್ಥ್ಯ,
                      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *src, int32_t src_ಉದ್ದಳತೆ,
                      UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಸ್ವಯಂಚಾಲಿತ *impl = reinterpret_cast<ಸ್ಥಿರಾಂಕ UCaseMapImpl *>(csm);
    ಹಿಂತಿರುಗಿ ucasemap_mapUTF8(
        UCASE_LOC_ಬೇರು, impl->options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_ಸಾಮರ್ಥ್ಯ,
        src, src_ಉದ್ದಳತೆ,
        ucasemap_ಆಂತರಿಕ_UTF8_Fold, ಶೂನ್ಯಸೂಚಕ, *p_Error_ಸಂಕೇತ);
}

U_NAMESPACE_BEGIN

ಶೂನ್ಯ CaseMap::utf8_ಗಮ್ಯ_ಅಧೋ(
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *locale, uint32_t options,
        StringPiece src, ByteSink &sink, Edits *edits,
        UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        src.ದತ್ತಾಂಶ(), src.length(),
        ucasemap_ಆಂತರಿಕ_UTF8_ಗಮ್ಯ_ಅಧೋ, sink, edits, error_ಸಂಕೇತ);
}

ಶೂನ್ಯ CaseMap::utf8_ಗಮ್ಯ_ಊರ್ಧ್ವ(
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *locale, uint32_t options,
        StringPiece src, ByteSink &sink, Edits *edits,
        UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        src.ದತ್ತಾಂಶ(), src.length(),
        ucasemap_ಆಂತರಿಕ_UTF8_ಗಮ್ಯ_ಊರ್ಧ್ವ, sink, edits, error_ಸಂಕೇತ);
}

ಶೂನ್ಯ CaseMap::utf8Fold(
        uint32_t options,
        StringPiece src, ByteSink &sink, Edits *edits,
        UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ucasemap_mapUTF8(
        UCASE_LOC_ಬೇರು, options, UCASEMAP_BREAK_ITERATOR_NULL
        src.ದತ್ತಾಂಶ(), src.length(),
        ucasemap_ಆಂತರಿಕ_UTF8_Fold, sink, edits, error_ಸಂಕೇತ);
}

int32_t CaseMap::utf8_ಗಮ್ಯ_ಅಧೋ(
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *locale, uint32_t options,
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *src, int32_t src_ಉದ್ದಳತೆ,
        ಅಕ್ಷರ *dest, int32_t dest_ಸಾಮರ್ಥ್ಯ, Edits *edits,
        UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಹಿಂತಿರುಗಿ ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_ಸಾಮರ್ಥ್ಯ,
        src, src_ಉದ್ದಳತೆ,
        ucasemap_ಆಂತರಿಕ_UTF8_ಗಮ್ಯ_ಅಧೋ, edits, error_ಸಂಕೇತ);
}

int32_t CaseMap::utf8_ಗಮ್ಯ_ಊರ್ಧ್ವ(
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *locale, uint32_t options,
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *src, int32_t src_ಉದ್ದಳತೆ,
        ಅಕ್ಷರ *dest, int32_t dest_ಸಾಮರ್ಥ್ಯ, Edits *edits,
        UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಹಿಂತಿರುಗಿ ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_ಸಾಮರ್ಥ್ಯ,
        src, src_ಉದ್ದಳತೆ,
        ucasemap_ಆಂತರಿಕ_UTF8_ಗಮ್ಯ_ಊರ್ಧ್ವ, edits, error_ಸಂಕೇತ);
}

int32_t CaseMap::utf8Fold(
        uint32_t options,
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *src, int32_t src_ಉದ್ದಳತೆ,
        ಅಕ್ಷರ *dest, int32_t dest_ಸಾಮರ್ಥ್ಯ, Edits *edits,
        UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಹಿಂತಿರುಗಿ ucasemap_mapUTF8(
        UCASE_LOC_ಬೇರು, options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_ಸಾಮರ್ಥ್ಯ,
        src, src_ಉದ್ದಳತೆ,
        ucasemap_ಆಂತರಿಕ_UTF8_Fold, edits, error_ಸಂಕೇತ);
}

U_NAMESPACE_ಕೊನೆ
