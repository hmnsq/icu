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

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/brkiter.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/bytestream.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/casemap.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/edits.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/stringoptions.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/stringpiece.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ubrk.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uloc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ustring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ucasemap.h"
#ਜੇ !UCONFIG_NO_BREAK_ITERATION
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utext.h"
#ਜੇ_ਅੰਤ
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utf.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utf8.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utf16.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "bytesinkutil.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cstring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uassert.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucase.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucasemap_imp.h"

U_NAMESPACE_USE

/* UCaseMap service object -------------------------------------------------- */

// The public type, unicode/ucasemap.h struct UCaseMap, is an opaque C type.
// We define a parallel C++ type in the ICU namespace for entry point renaming.

U_NAMESPACE_BEGIN

UCaseMapImpl::UCaseMapImpl(ਅਚਲ ਅੱਖਰ *localeID, uint32_t opts, UError_ਕੋਡ *p_Error_ਕੋਡ) :
#ਜੇ !UCONFIG_NO_BREAK_ITERATION
        iter(ਖਾਲੀ_ਸੂਚਕ),
#ਜੇ_ਅੰਤ
        caseLocale(UCASE_LOC_UNKNOWN), options(opts) {
    ucasemap_ਸੈੱਟ_Locale(reinterpret_cast<UCaseMap *>(ਇਹ), localeID, p_Error_ਕੋਡ);
}

UCaseMapImpl::~UCaseMapImpl() {
#ਜੇ !UCONFIG_NO_BREAK_ITERATION
    ਮਿਟਾਓ iter;
#ਜੇ_ਅੰਤ
}

U_NAMESPACE_ਸਿਰਾ

U_CAPI UCaseMap * U_EXPORT2
ucasemap_open(ਅਚਲ ਅੱਖਰ *locale, uint32_t options, UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    UCaseMapImpl *csm = ਨਵਾਂ UCaseMapImpl(locale, options, p_Error_ਕੋਡ);
    ਜੇ(csm==ਖਾਲੀ_ਸੂਚਕ) {
        *p_Error_ਕੋਡ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    } ਨਹੀਂਤਾਂ ਜੇ (U_FAILURE(*p_Error_ਕੋਡ)) {
        ਮਿਟਾਓ csm;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਵਾਪਸ reinterpret_cast<UCaseMap *>(csm);
}

U_CAPI ਖਾਲੀ U_EXPORT2
ucasemap_close(UCaseMap *csm) {
    ਮਿਟਾਓ reinterpret_cast<UCaseMapImpl *>(csm);
}

U_CAPI ਅਚਲ ਅੱਖਰ* U_EXPORT2
ucasemap_getLocale(ਅਚਲ UCaseMap* csm U_LIFETIME_BOUND) {
    ਵਾਪਸ reinterpret_cast<ਅਚਲ UCaseMapImpl *>(csm)->locale;
}

U_CAPI uint32_t U_EXPORT2
ucasemap_getOptions(ਅਚਲ UCaseMap *csm) {
    ਵਾਪਸ reinterpret_cast<ਅਚਲ UCaseMapImpl *>(csm)->options;
}

U_CAPI ਖਾਲੀ U_EXPORT2
ucasemap_ਸੈੱਟ_Locale(UCaseMap *csm, ਅਚਲ ਅੱਖਰ *locale, UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ;
    }
    ਸਵੈਚਾਲਿਤ *impl = reinterpret_cast<UCaseMapImpl *>(csm);
    ਜੇ (locale != ਖਾਲੀ_ਸੂਚਕ && *locale == 0) {
        impl->locale[0] = 0;
        impl->caseLocale = UCASE_LOC_ਜੜ੍ਹ;
        ਵਾਪਸ;
    }

    UError_ਕੋਡ ਬਫਰ_ਸਥਿਤੀ = U_ZERO_ERROR;
    int32_t length=uloc_getName(locale, impl->locale, (int32_t)ਆਕਾਰ(impl->locale), &ਬਫਰ_ਸਥਿਤੀ);
    ਜੇ(ਬਫਰ_ਸਥਿਤੀ==U_ਬਫਰ_ਉਛਾਲ_ERROR || (U_SUCCESS(ਬਫਰ_ਸਥਿਤੀ) && length==ਆਕਾਰ(impl->locale))) {
        ਬਫਰ_ਸਥਿਤੀ = U_ZERO_ERROR;
        /* we only really need the language code for case mappings */
        length=uloc_getLanguage(locale, impl->locale, (int32_t)ਆਕਾਰ(impl->locale), &ਬਫਰ_ਸਥਿਤੀ);
    }
    ਜੇ(U_FAILURE(ਬਫਰ_ਸਥਿਤੀ)) {
        *p_Error_ਕੋਡ=ਬਫਰ_ਸਥਿਤੀ;
    } ਨਹੀਂਤਾਂ ਜੇ(length==ਆਕਾਰ(impl->locale)) {
        *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
    }
    ਜੇ(U_SUCCESS(*p_Error_ਕੋਡ)) {     
        impl->caseLocale = ucase_getCaseLocale(impl->locale);
    } ਨਹੀਂਤਾਂ {
        impl->locale[0]=0;
        impl->caseLocale = UCASE_LOC_ਜੜ੍ਹ;
    }
}

U_CAPI ਖਾਲੀ U_EXPORT2
ucasemap_ਸੈੱਟ_Options(UCaseMap *csm, uint32_t options, UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ;
    }
    reinterpret_cast<UCaseMapImpl *>(csm)->options=options;
}

/* UTF-8 string case mappings ----------------------------------------------- */

/* TODO(markus): Move to a new, separate utf8case.cpp file. */

ਨਾਮਾਕਾਸ਼ {

/* append a full case mapping result, see UCASE_MAX_STRING_LENGTH */
ਸਤਰਅੰਦਰ UBool
append_ਨਤੀਜਾ(int32_t cp_ਲੰਮਾਈ, int32_t ਨਤੀਜਾ, ਅਚਲ char16_t *s,
             ByteSink &sink, uint32_t options, icu::Edits *edits, UError_ਕੋਡ &error_ਕੋਡ) {
    U_ASSERT(U_SUCCESS(error_ਕੋਡ));

    /* decode the result */
    ਜੇ(ਨਤੀਜਾ<0) {
        /* (not) original code point */
        ਜੇ(edits!=ਖਾਲੀ_ਸੂਚਕ) {
            edits->ਜੋੜੋ_Unchanged(cp_ਲੰਮਾਈ);
        }
        ਜੇ((options & U_OMIT_UNCHANGED_ਪਾਠ) == 0) {
            ByteSinkUtil::append_ਕੋਡ_ਬਿੰਦੂ(cp_ਲੰਮਾਈ, ~ਨਤੀਜਾ, sink);
        }
    } ਨਹੀਂਤਾਂ {
        ਜੇ(ਨਤੀਜਾ<=UCASE_MAX_STRING_ਲੰਮਾਈ) {
            // string: "result" is the UTF-16 length
            ਵਾਪਸ ByteSinkUtil::appendChange(cp_ਲੰਮਾਈ, s, ਨਤੀਜਾ, sink, edits, error_ਕੋਡ);
        } ਨਹੀਂਤਾਂ {
            ByteSinkUtil::append_ਕੋਡ_ਬਿੰਦੂ(cp_ਲੰਮਾਈ, ਨਤੀਜਾ, sink, edits);
        }
    }
    ਵਾਪਸ ਸੱਚ;
}

// See unicode/utf8.h U8_APPEND_UNSAFE().
ਸਤਰਅੰਦਰ uint8_t getTwoByteLead(UChar32 c) { ਵਾਪਸ static_cast<uint8_t>((c >> 6) | 0xc0); }
ਸਤਰਅੰਦਰ uint8_t getTwoByteTrail(UChar32 c) { ਵਾਪਸ static_cast<uint8_t>((c & 0x3f) | 0x80); }

UChar32 U_CALLCONV
utf8_caseContextIterator(ਖਾਲੀ *context, int8_t dir) {
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
            U8_PREV((ਅਚਲ uint8_t *)csc->p, csc->ਸ਼ੁਰੂ, csc->ਸੂਚਕਾਂਕ, c);
            ਵਾਪਸ c;
        }
    } ਨਹੀਂਤਾਂ {
        ਜੇ(csc->ਸੂਚਕਾਂਕ<csc->ਹੱਦਬੰਦੀ) {
            U8_ਅਗਲਾ((ਅਚਲ uint8_t *)csc->p, csc->ਸੂਚਕਾਂਕ, csc->ਹੱਦਬੰਦੀ, c);
            ਵਾਪਸ c;
        }
    }
    ਵਾਪਸ U_SENTINEL;
}

/**
 * caseLocale >= 0: Lowercases [srcStart..srcLimit[ but takes context [0..srcLength[ into account.
 * caseLocale < 0: Case-folds [srcStart..srcLimit[.
 */
ਖਾਲੀ ਮੰਜ਼ਿਲ_ਹੇਠਲਾ(int32_t caseLocale, uint32_t options,
             ਅਚਲ uint8_t *src, UCaseContext *csc, int32_t src_ਸ਼ੁਰੂ, int32_t src_ਹੱਦਬੰਦੀ,
             icu::ByteSink &sink, icu::Edits *edits, UError_ਕੋਡ &error_ਕੋਡ) {
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
    int32_t prev = src_ਸ਼ੁਰੂ;
    int32_t src_ਸੂਚਕਾਂਕ = src_ਸ਼ੁਰੂ;
    ਲਈ (;;) {
        // fast path for simple cases
        int32_t cp_ਸ਼ੁਰੂ;
        UChar32 c;
        ਲਈ (;;) {
            ਜੇ (U_FAILURE(error_ਕੋਡ) || src_ਸੂਚਕਾਂਕ >= src_ਹੱਦਬੰਦੀ) {
                c = U_SENTINEL;
                ਰੋਕੋ;
            }
            uint8_t lead = src[src_ਸੂਚਕਾਂਕ++];
            ਜੇ (lead <= 0x7f) {
                int8_t d = latin_ਮੰਜ਼ਿਲ_ਹੇਠਲਾ[lead];
                ਜੇ (d == LatinCase::EXC) {
                    cp_ਸ਼ੁਰੂ = src_ਸੂਚਕਾਂਕ - 1;
                    c = lead;
                    ਰੋਕੋ;
                }
                ਜੇ (d == 0) { ਜਾਰੀ; }
                ByteSinkUtil::appendUnchanged(src + prev, src_ਸੂਚਕਾਂਕ - 1 - prev,
                                              sink, options, edits, error_ਕੋਡ);
                ਅੱਖਰ ascii = static_cast<ਅੱਖਰ>(lead + d);
                sink.Append(&ascii, 1);
                ਜੇ (edits != ਖਾਲੀ_ਸੂਚਕ) {
                    edits->ਜੋੜੋ_Replace(1, 1);
                }
                prev = src_ਸੂਚਕਾਂਕ;
                ਜਾਰੀ;
            } ਨਹੀਂਤਾਂ ਜੇ (lead < 0xe3) {
                uint8_t t;
                ਜੇ (0xc2 <= lead && lead <= 0xc5 && src_ਸੂਚਕਾਂਕ < src_ਹੱਦਬੰਦੀ &&
                        (t = src[src_ਸੂਚਕਾਂਕ] - 0x80) <= 0x3f) {
                    // U+0080..U+017F
                    ++src_ਸੂਚਕਾਂਕ;
                    c = ((lead - 0xc0) << 6) | t;
                    int8_t d = latin_ਮੰਜ਼ਿਲ_ਹੇਠਲਾ[c];
                    ਜੇ (d == LatinCase::EXC) {
                        cp_ਸ਼ੁਰੂ = src_ਸੂਚਕਾਂਕ - 2;
                        ਰੋਕੋ;
                    }
                    ਜੇ (d == 0) { ਜਾਰੀ; }
                    ByteSinkUtil::appendUnchanged(src + prev, src_ਸੂਚਕਾਂਕ - 2 - prev,
                                                  sink, options, edits, error_ਕੋਡ);
                    ByteSinkUtil::append_Two_ਬਾਈਟਾਂ(c + d, sink);
                    ਜੇ (edits != ਖਾਲੀ_ਸੂਚਕ) {
                        edits->ਜੋੜੋ_Replace(2, 2);
                    }
                    prev = src_ਸੂਚਕਾਂਕ;
                    ਜਾਰੀ;
                }
            } ਨਹੀਂਤਾਂ ਜੇ ((lead <= 0xe9 || lead == 0xeb || lead == 0xec) &&
                    (src_ਸੂਚਕਾਂਕ + 2) <= src_ਹੱਦਬੰਦੀ &&
                    U8_IS_TRAIL(src[src_ਸੂਚਕਾਂਕ]) && U8_IS_TRAIL(src[src_ਸੂਚਕਾਂਕ + 1])) {
                // most of CJK: no case mappings
                src_ਸੂਚਕਾਂਕ += 2;
                ਜਾਰੀ;
            }
            cp_ਸ਼ੁਰੂ = --src_ਸੂਚਕਾਂਕ;
            U8_ਅਗਲਾ(src, src_ਸੂਚਕਾਂਕ, src_ਹੱਦਬੰਦੀ, c);
            ਜੇ (c < 0) {
                // ill-formed UTF-8
                ਜਾਰੀ;
            }
            uint16_t props = UTRIE2_GET16(trie, c);
            ਜੇ (UCASE_HAS_EXCEPTION(props)) { ਰੋਕੋ; }
            int32_t delta;
            ਜੇ (!UCASE_IS_ਉੱਤਲਾ_OR_TITLE(props) || (delta = UCASE_GET_DELTA(props)) == 0) {
                ਜਾਰੀ;
            }
            ByteSinkUtil::appendUnchanged(src + prev, cp_ਸ਼ੁਰੂ - prev,
                                          sink, options, edits, error_ਕੋਡ);
            ByteSinkUtil::append_ਕੋਡ_ਬਿੰਦੂ(src_ਸੂਚਕਾਂਕ - cp_ਸ਼ੁਰੂ, c + delta, sink, edits);
            prev = src_ਸੂਚਕਾਂਕ;
        }
        ਜੇ (c < 0) {
            ਰੋਕੋ;
        }
        // slow path
        ਅਚਲ char16_t *s;
        ਜੇ (caseLocale >= 0) {
            csc->cp_ਸ਼ੁਰੂ = cp_ਸ਼ੁਰੂ;
            csc->cp_ਹੱਦਬੰਦੀ = src_ਸੂਚਕਾਂਕ;
            c = ucase_ਮੰਜ਼ਿਲ_ਭਰਿਆ_ਹੇਠਲਾ(c, utf8_caseContextIterator, csc, &s, caseLocale);
        } ਨਹੀਂਤਾਂ {
            c = ucase_ਮੰਜ਼ਿਲ_ਭਰਿਆ_Folding(c, &s, options);
        }
        ਜੇ (c >= 0) {
            ByteSinkUtil::appendUnchanged(src + prev, cp_ਸ਼ੁਰੂ - prev,
                                          sink, options, edits, error_ਕੋਡ);
            append_ਨਤੀਜਾ(src_ਸੂਚਕਾਂਕ - cp_ਸ਼ੁਰੂ, c, s, sink, options, edits, error_ਕੋਡ);
            prev = src_ਸੂਚਕਾਂਕ;
        }
    }
    ByteSinkUtil::appendUnchanged(src + prev, src_ਸੂਚਕਾਂਕ - prev,
                                  sink, options, edits, error_ਕੋਡ);
}

ਖਾਲੀ ਮੰਜ਼ਿਲ_ਉੱਤਲਾ(int32_t caseLocale, uint32_t options,
             ਅਚਲ uint8_t *src, UCaseContext *csc, int32_t src_ਲੰਮਾਈ,
             icu::ByteSink &sink, icu::Edits *edits, UError_ਕੋਡ &error_ਕੋਡ) {
    ਅਚਲ int8_t *latin_ਮੰਜ਼ਿਲ_ਉੱਤਲਾ;
    ਜੇ (caseLocale == UCASE_LOC_TURKISH) {
        latin_ਮੰਜ਼ਿਲ_ਉੱਤਲਾ = LatinCase::ਮੰਜ਼ਿਲ_ਉੱਤਲਾ_TR;
    } ਨਹੀਂਤਾਂ {
        latin_ਮੰਜ਼ਿਲ_ਉੱਤਲਾ = LatinCase::ਮੰਜ਼ਿਲ_ਉੱਤਲਾ_ਸਾਧਾਰਨ;
    }
    ਅਚਲ UTrie2 *trie = ucase_getTrie();
    int32_t prev = 0;
    int32_t src_ਸੂਚਕਾਂਕ = 0;
    ਲਈ (;;) {
        // fast path for simple cases
        int32_t cp_ਸ਼ੁਰੂ;
        UChar32 c;
        ਲਈ (;;) {
            ਜੇ (U_FAILURE(error_ਕੋਡ) || src_ਸੂਚਕਾਂਕ >= src_ਲੰਮਾਈ) {
                c = U_SENTINEL;
                ਰੋਕੋ;
            }
            uint8_t lead = src[src_ਸੂਚਕਾਂਕ++];
            ਜੇ (lead <= 0x7f) {
                int8_t d = latin_ਮੰਜ਼ਿਲ_ਉੱਤਲਾ[lead];
                ਜੇ (d == LatinCase::EXC) {
                    cp_ਸ਼ੁਰੂ = src_ਸੂਚਕਾਂਕ - 1;
                    c = lead;
                    ਰੋਕੋ;
                }
                ਜੇ (d == 0) { ਜਾਰੀ; }
                ByteSinkUtil::appendUnchanged(src + prev, src_ਸੂਚਕਾਂਕ - 1 - prev,
                                              sink, options, edits, error_ਕੋਡ);
                ਅੱਖਰ ascii = static_cast<ਅੱਖਰ>(lead + d);
                sink.Append(&ascii, 1);
                ਜੇ (edits != ਖਾਲੀ_ਸੂਚਕ) {
                    edits->ਜੋੜੋ_Replace(1, 1);
                }
                prev = src_ਸੂਚਕਾਂਕ;
                ਜਾਰੀ;
            } ਨਹੀਂਤਾਂ ਜੇ (lead < 0xe3) {
                uint8_t t;
                ਜੇ (0xc2 <= lead && lead <= 0xc5 && src_ਸੂਚਕਾਂਕ < src_ਲੰਮਾਈ &&
                        (t = src[src_ਸੂਚਕਾਂਕ] - 0x80) <= 0x3f) {
                    // U+0080..U+017F
                    ++src_ਸੂਚਕਾਂਕ;
                    c = ((lead - 0xc0) << 6) | t;
                    int8_t d = latin_ਮੰਜ਼ਿਲ_ਉੱਤਲਾ[c];
                    ਜੇ (d == LatinCase::EXC) {
                        cp_ਸ਼ੁਰੂ = src_ਸੂਚਕਾਂਕ - 2;
                        ਰੋਕੋ;
                    }
                    ਜੇ (d == 0) { ਜਾਰੀ; }
                    ByteSinkUtil::appendUnchanged(src + prev, src_ਸੂਚਕਾਂਕ - 2 - prev,
                                                  sink, options, edits, error_ਕੋਡ);
                    ByteSinkUtil::append_Two_ਬਾਈਟਾਂ(c + d, sink);
                    ਜੇ (edits != ਖਾਲੀ_ਸੂਚਕ) {
                        edits->ਜੋੜੋ_Replace(2, 2);
                    }
                    prev = src_ਸੂਚਕਾਂਕ;
                    ਜਾਰੀ;
                }
            } ਨਹੀਂਤਾਂ ਜੇ ((lead <= 0xe9 || lead == 0xeb || lead == 0xec) &&
                    (src_ਸੂਚਕਾਂਕ + 2) <= src_ਲੰਮਾਈ &&
                    U8_IS_TRAIL(src[src_ਸੂਚਕਾਂਕ]) && U8_IS_TRAIL(src[src_ਸੂਚਕਾਂਕ + 1])) {
                // most of CJK: no case mappings
                src_ਸੂਚਕਾਂਕ += 2;
                ਜਾਰੀ;
            }
            cp_ਸ਼ੁਰੂ = --src_ਸੂਚਕਾਂਕ;
            U8_ਅਗਲਾ(src, src_ਸੂਚਕਾਂਕ, src_ਲੰਮਾਈ, c);
            ਜੇ (c < 0) {
                // ill-formed UTF-8
                ਜਾਰੀ;
            }
            uint16_t props = UTRIE2_GET16(trie, c);
            ਜੇ (UCASE_HAS_EXCEPTION(props)) { ਰੋਕੋ; }
            int32_t delta;
            ਜੇ (UCASE_GET_TYPE(props) != UCASE_ਹੇਠਲਾ || (delta = UCASE_GET_DELTA(props)) == 0) {
                ਜਾਰੀ;
            }
            ByteSinkUtil::appendUnchanged(src + prev, cp_ਸ਼ੁਰੂ - prev,
                                          sink, options, edits, error_ਕੋਡ);
            ByteSinkUtil::append_ਕੋਡ_ਬਿੰਦੂ(src_ਸੂਚਕਾਂਕ - cp_ਸ਼ੁਰੂ, c + delta, sink, edits);
            prev = src_ਸੂਚਕਾਂਕ;
        }
        ਜੇ (c < 0) {
            ਰੋਕੋ;
        }
        // slow path
        csc->cp_ਸ਼ੁਰੂ = cp_ਸ਼ੁਰੂ;
        csc->cp_ਹੱਦਬੰਦੀ = src_ਸੂਚਕਾਂਕ;
        ਅਚਲ char16_t *s;
        c = ucase_ਮੰਜ਼ਿਲ_ਭਰਿਆ_ਉੱਤਲਾ(c, utf8_caseContextIterator, csc, &s, caseLocale);
        ਜੇ (c >= 0) {
            ByteSinkUtil::appendUnchanged(src + prev, cp_ਸ਼ੁਰੂ - prev,
                                          sink, options, edits, error_ਕੋਡ);
            append_ਨਤੀਜਾ(src_ਸੂਚਕਾਂਕ - cp_ਸ਼ੁਰੂ, c, s, sink, options, edits, error_ਕੋਡ);
            prev = src_ਸੂਚਕਾਂਕ;
        }
    }
    ByteSinkUtil::appendUnchanged(src + prev, src_ਸੂਚਕਾਂਕ - prev,
                                  sink, options, edits, error_ਕੋਡ);
}

}  // namespace

#ਜੇ !UCONFIG_NO_BREAK_ITERATION

ਨਾਮਾਕਾਸ਼ {

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
int32_t maybeTitleDutchIJ(ਅਚਲ uint8_t *src, UChar32 c, int32_t ਸ਼ੁਰੂ, int32_t segment_ਹੱਦਬੰਦੀ,
                          ByteSink &sink, uint32_t options, icu::Edits *edits, UError_ਕੋਡ &error_ਕੋਡ) {
    U_ASSERT(ਸ਼ੁਰੂ < segment_ਹੱਦਬੰਦੀ);

    int32_t ਸੂਚਕਾਂਕ = ਸ਼ੁਰੂ;
    ਤਾਰਕਿਕ withAcute = ਝੂਠ;

    // If the conditions are met, then the following variables tell us what to output.
    int32_t unchanged1 = 0;  // code units before the j, or the whole sequence (0..3)
    ਤਾਰਕਿਕ doTitleJ = ਝੂਠ;  // true if the j needs to be titlecased
    int32_t unchanged2 = 0;  // after the j (0 or 1)

    // next character after the first letter
    UChar32 c2;
    c2 = src[ਸੂਚਕਾਂਕ++];

    // Is the first letter an i/I with accent?
    ਜੇ (c == u'I') {
        ਜੇ (c2 == ACUTE_BYTE0 && ਸੂਚਕਾਂਕ < segment_ਹੱਦਬੰਦੀ && src[ਸੂਚਕਾਂਕ++] == ACUTE_BYTE1) {
            withAcute = ਸੱਚ;
            unchanged1 = 2;  // ACUTE is 2 code units in UTF-8
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
        ਜੇ ((ਸੂਚਕਾਂਕ + 1) >= segment_ਹੱਦਬੰਦੀ || src[ਸੂਚਕਾਂਕ++] != ACUTE_BYTE0 || src[ਸੂਚਕਾਂਕ++] != ACUTE_BYTE1) {
            ਵਾਪਸ ਸ਼ੁਰੂ;
        }
        ਜੇ (doTitleJ) {
            unchanged2 = 2;  // ACUTE is 2 code units in UTF-8
        } ਨਹੀਂਤਾਂ {
            unchanged1 = unchanged1 + 2;    // ACUTE is 2 code units in UTF-8
        }
    }

    // There must not be another combining mark.
    ਜੇ (ਸੂਚਕਾਂਕ < segment_ਹੱਦਬੰਦੀ) {
        int32_t cp;
        int32_t i = ਸੂਚਕਾਂਕ;
        U8_ਅਗਲਾ(src, i, segment_ਹੱਦਬੰਦੀ, cp);
        uint32_t type_ਨਕਾਬ = U_GET_GC_ਨਕਾਬ(cp);
        ਜੇ ((type_ਨਕਾਬ & U_GC_M_ਨਕਾਬ) != 0) {
            ਵਾਪਸ ਸ਼ੁਰੂ;
        }
    }

    // Output the rest of the Dutch IJ.
    ByteSinkUtil::appendUnchanged(src + ਸ਼ੁਰੂ, unchanged1, sink, options, edits, error_ਕੋਡ);
    ਸ਼ੁਰੂ += unchanged1;
    ਜੇ (doTitleJ) {
        ByteSinkUtil::append_ਕੋਡ_ਬਿੰਦੂ(1, u'J', sink, edits);
        ++ਸ਼ੁਰੂ;
    }
    ByteSinkUtil::appendUnchanged(src + ਸ਼ੁਰੂ, unchanged2, sink, options, edits, error_ਕੋਡ);

    U_ASSERT(ਸ਼ੁਰੂ + unchanged2 == ਸੂਚਕਾਂਕ);
    ਵਾਪਸ ਸੂਚਕਾਂਕ;
}

}  // namespace

U_CFUNC ਖਾਲੀ U_CALLCONV
ucasemap_ਅੰਦਰੂਨੀ_UTF8_ਮੰਜ਼ਿਲ_Title(
        int32_t caseLocale, uint32_t options, BreakIterator *iter,
        ਅਚਲ uint8_t *src, int32_t src_ਲੰਮਾਈ,
        ByteSink &sink, icu::Edits *edits,
        UError_ਕੋਡ &error_ਕੋਡ) {
    ਜੇ (!ustrcase_checkTitleAdjustmentOptions(options, error_ਕੋਡ)) {
        ਵਾਪਸ;
    }

    /* set up local variables */
    UCaseContext csc=UCASECONTEXT_INITIALIZER;
    csc.p=(ਖਾਲੀ *)src;
    csc.ਹੱਦਬੰਦੀ=src_ਲੰਮਾਈ;
    int32_t prev=0;
    UBool is_ਪਹਿਲਾ_ਸੂਚਕਾਂਕ=ਸੱਚ;

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
            /* find and copy skipped characters [prev..titleStart[ */
            int32_t title_ਸ਼ੁਰੂ=prev;
            int32_t title_ਹੱਦਬੰਦੀ=prev;
            UChar32 c;
            U8_ਅਗਲਾ(src, title_ਹੱਦਬੰਦੀ, ਸੂਚਕਾਂਕ, c);
            ਜੇ ((options&U_TITLECASE_NO_BREAK_ADJUSTMENT)==0) {
                // Adjust the titlecasing index to the next cased character,
                // or to the next letter/number/symbol/private use.
                // Stop with titleStart<titleLimit<=index
                // if there is a character to be titlecased,
                // or else stop with titleStart==titleLimit==index.
                UBool ਮੰਜ਼ਿਲ_Cased = (options&U_TITLECASE_ADJUST_ਮੰਜ਼ਿਲ_CASED) != 0;
                ਜਦੋਂ (ਮੰਜ਼ਿਲ_Cased ? UCASE_NONE==ucase_getType(c) : !ustrcase_isLNS(c)) {
                    title_ਸ਼ੁਰੂ=title_ਹੱਦਬੰਦੀ;
                    ਜੇ(title_ਹੱਦਬੰਦੀ==ਸੂਚਕਾਂਕ) {
                        ਰੋਕੋ;
                    }
                    U8_ਅਗਲਾ(src, title_ਹੱਦਬੰਦੀ, ਸੂਚਕਾਂਕ, c);
                }
                ਜੇ (prev < title_ਸ਼ੁਰੂ) {
                    ਜੇ (!ByteSinkUtil::appendUnchanged(src+prev, title_ਸ਼ੁਰੂ-prev,
                                                       sink, options, edits, error_ਕੋਡ)) {
                        ਵਾਪਸ;
                    }
                }
            }

            ਜੇ(title_ਸ਼ੁਰੂ<title_ਹੱਦਬੰਦੀ) {
                /* titlecase c which is from [titleStart..titleLimit[ */
                ਜੇ(c>=0) {
                    csc.cp_ਸ਼ੁਰੂ=title_ਸ਼ੁਰੂ;
                    csc.cp_ਹੱਦਬੰਦੀ=title_ਹੱਦਬੰਦੀ;
                    ਅਚਲ char16_t *s;
                    c=ucase_ਮੰਜ਼ਿਲ_ਭਰਿਆ_Title(c, utf8_caseContextIterator, &csc, &s, caseLocale);
                    ਜੇ (!append_ਨਤੀਜਾ(title_ਹੱਦਬੰਦੀ-title_ਸ਼ੁਰੂ, c, s, sink, options, edits, error_ਕੋਡ)) {
                        ਵਾਪਸ;
                    }
                } ਨਹੀਂਤਾਂ {
                    // Malformed UTF-8.
                    ਜੇ (!ByteSinkUtil::appendUnchanged(src+title_ਸ਼ੁਰੂ, title_ਹੱਦਬੰਦੀ-title_ਸ਼ੁਰੂ,
                                                       sink, options, edits, error_ਕੋਡ)) {
                        ਵਾਪਸ;
                    }
                }

                /* Special case Dutch IJ titlecasing */
                ਜੇ (title_ਹੱਦਬੰਦੀ < ਸੂਚਕਾਂਕ &&
                    caseLocale == UCASE_LOC_DUTCH) {
                    ਜੇ (c < 0) {
                        c = ~c;
                    }

                    ਜੇ (c == u'I' || c == u'Í') {
                        title_ਹੱਦਬੰਦੀ = maybeTitleDutchIJ(src, c, title_ਹੱਦਬੰਦੀ, ਸੂਚਕਾਂਕ, sink, options, edits, error_ਕੋਡ);
                    }
                }

                /* lowercase [titleLimit..index[ */
                ਜੇ(title_ਹੱਦਬੰਦੀ<ਸੂਚਕਾਂਕ) {
                    ਜੇ((options&U_TITLECASE_NO_LOWERCASE)==0) {
                        /* Normal operation: Lowercase the rest of the word. */
                        ਮੰਜ਼ਿਲ_ਹੇਠਲਾ(caseLocale, options,
                                src, &csc, title_ਹੱਦਬੰਦੀ, ਸੂਚਕਾਂਕ,
                                sink, edits, error_ਕੋਡ);
                        ਜੇ(U_FAILURE(error_ਕੋਡ)) {
                            ਵਾਪਸ;
                        }
                    } ਨਹੀਂਤਾਂ {
                        /* Optionally just copy the rest of the word unchanged. */
                        ਜੇ (!ByteSinkUtil::appendUnchanged(src+title_ਹੱਦਬੰਦੀ, ਸੂਚਕਾਂਕ-title_ਹੱਦਬੰਦੀ,
                                                           sink, options, edits, error_ਕੋਡ)) {
                            ਵਾਪਸ;
                        }
                    }
                }
            }
        }

        prev=ਸੂਚਕਾਂਕ;
    }
}

#ਜੇ_ਅੰਤ

U_NAMESPACE_BEGIN
ਨਾਮਾਕਾਸ਼ Greek_ਉੱਤਲਾ {

UBool isFollowedByCasedLetter(ਅਚਲ uint8_t *s, int32_t i, int32_t length) {
    ਜਦੋਂ (i < length) {
        UChar32 c;
        U8_ਅਗਲਾ(s, i, length, c);
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

// Keep this consistent with the UTF-16 version in ustrcase.cpp and the Java version in CaseMap.java.
ਖਾਲੀ ਮੰਜ਼ਿਲ_ਉੱਤਲਾ(uint32_t options,
             ਅਚਲ uint8_t *src, int32_t src_ਲੰਮਾਈ,
             ByteSink &sink, Edits *edits,
             UError_ਕੋਡ &error_ਕੋਡ) {
    uint32_t state = 0;
    ਲਈ (int32_t i = 0; i < src_ਲੰਮਾਈ;) {
        int32_t ਅਗਲਾ_ਸੂਚਕਾਂਕ = i;
        UChar32 c;
        U8_ਅਗਲਾ(src, ਅਗਲਾ_ਸੂਚਕਾਂਕ, src_ਲੰਮਾਈ, c);
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
                ਅੰਕੜੇ |= (state & AFTER_VOWEL_WITH_PRECOMPOSED_ACCENT) != 0 ? HAS_DIALYTIKA
                                                                           : HAS_COMBINING_DIALYTIKA;
            }
            int32_t ਅੰਕ_Ypogegrammeni = 0;  // Map each one to a trailing, spacing, capital iota.
            ਜੇ ((ਅੰਕੜੇ & HAS_YPOGEGRAMMENI) != 0) {
                ਅੰਕ_Ypogegrammeni = 1;
            }
            ਅਚਲ UBool hasPrecomposedAccent = (ਅੰਕੜੇ & HAS_ACCENT) != 0;
            // Skip combining diacritics after this Greek letter.
            int32_t ਅਗਲਾ_ਅਗਲਾ_ਸੂਚਕਾਂਕ = ਅਗਲਾ_ਸੂਚਕਾਂਕ;
            ਜਦੋਂ (ਅਗਲਾ_ਸੂਚਕਾਂਕ < src_ਲੰਮਾਈ) {
                UChar32 c2;
                U8_ਅਗਲਾ(src, ਅਗਲਾ_ਅਗਲਾ_ਸੂਚਕਾਂਕ, src_ਲੰਮਾਈ, c2);
                uint32_t diacritic_ਅੰਕੜੇ = get_Diacritic_ਅੰਕੜੇ(c2);
                ਜੇ (diacritic_ਅੰਕੜੇ != 0) {
                    ਅੰਕੜੇ |= diacritic_ਅੰਕੜੇ;
                    ਜੇ ((diacritic_ਅੰਕੜੇ & HAS_YPOGEGRAMMENI) != 0) {
                        ++ਅੰਕ_Ypogegrammeni;
                    }
                    ਅਗਲਾ_ਸੂਚਕਾਂਕ = ਅਗਲਾ_ਅਗਲਾ_ਸੂਚਕਾਂਕ;
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
                U_ASSERT(0x370 <= upper && upper <= 0x3ff);  // 2-byte UTF-8, main Greek block
                change = (i + 2) > ਅਗਲਾ_ਸੂਚਕਾਂਕ ||
                        src[i] != getTwoByteLead(upper) || src[i + 1] != getTwoByteTrail(upper) ||
                        ਅੰਕ_Ypogegrammeni > 0;
                int32_t i2 = i + 2;
                ਜੇ ((ਅੰਕੜੇ & HAS_EITHER_DIALYTIKA) != 0) {
                    change |= (i2 + 2) > ਅਗਲਾ_ਸੂਚਕਾਂਕ ||
                            src[i2] != static_cast<uint8_t>(u8"\u0308"[0]) ||
                            src[i2 + 1] != static_cast<uint8_t>(u8"\u0308"[1]);
                    i2 += 2;
                }
                ਜੇ (ਜੋੜੋ_Tonos) {
                    change |= (i2 + 2) > ਅਗਲਾ_ਸੂਚਕਾਂਕ ||
                            src[i2] != static_cast<uint8_t>(u8"\u0301"[0]) ||
                            src[i2 + 1] != static_cast<uint8_t>(u8"\u0301"[1]);
                    i2 += 2;
                }
                int32_t old_ਲੰਮਾਈ = ਅਗਲਾ_ਸੂਚਕਾਂਕ - i;
                int32_t new_ਲੰਮਾਈ = (i2 - i) + ਅੰਕ_Ypogegrammeni * 2;  // 2 bytes per U+0399
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
                ByteSinkUtil::append_Two_ਬਾਈਟਾਂ(upper, sink);
                ਜੇ ((ਅੰਕੜੇ & HAS_EITHER_DIALYTIKA) != 0) {
                    sink.AppendU8(u8"\u0308", 2);  // restore or add a dialytika
                }
                ਜੇ (ਜੋੜੋ_Tonos) {
                    sink.AppendU8(u8"\u0301", 2);
                }
                ਜਦੋਂ (ਅੰਕ_Ypogegrammeni > 0) {
                    sink.AppendU8(u8"\u0399", 2);
                    --ਅੰਕ_Ypogegrammeni;
                }
            }
        } ਨਹੀਂਤਾਂ ਜੇ(c>=0) {
            ਅਚਲ char16_t *s;
            c=ucase_ਮੰਜ਼ਿਲ_ਭਰਿਆ_ਉੱਤਲਾ(c, ਖਾਲੀ_ਸੂਚਕ, ਖਾਲੀ_ਸੂਚਕ, &s, UCASE_LOC_GREEK);
            ਜੇ (!append_ਨਤੀਜਾ(ਅਗਲਾ_ਸੂਚਕਾਂਕ - i, c, s, sink, options, edits, error_ਕੋਡ)) {
                ਵਾਪਸ;
            }
        } ਨਹੀਂਤਾਂ {
            // Malformed UTF-8.
            ਜੇ (!ByteSinkUtil::appendUnchanged(src+i, ਅਗਲਾ_ਸੂਚਕਾਂਕ-i,
                                               sink, options, edits, error_ਕੋਡ)) {
                ਵਾਪਸ;
            }
        }
        i = ਅਗਲਾ_ਸੂਚਕਾਂਕ;
        state = ਅਗਲਾ_State;
    }
}

}  // namespace GreekUpper
U_NAMESPACE_ਸਿਰਾ

ਸਥਿਰ ਖਾਲੀ U_CALLCONV
ucasemap_ਅੰਦਰੂਨੀ_UTF8_ਮੰਜ਼ਿਲ_ਹੇਠਲਾ(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_ਅਣਵਰਤਿਆ
                             ਅਚਲ uint8_t *src, int32_t src_ਲੰਮਾਈ,
                             icu::ByteSink &sink, icu::Edits *edits,
                             UError_ਕੋਡ &error_ਕੋਡ) {
    UCaseContext csc=UCASECONTEXT_INITIALIZER;
    csc.p=(ਖਾਲੀ *)src;
    csc.ਹੱਦਬੰਦੀ=src_ਲੰਮਾਈ;
    ਮੰਜ਼ਿਲ_ਹੇਠਲਾ(
        caseLocale, options,
        src, &csc, 0, src_ਲੰਮਾਈ,
        sink, edits, error_ਕੋਡ);
}

ਸਥਿਰ ਖਾਲੀ U_CALLCONV
ucasemap_ਅੰਦਰੂਨੀ_UTF8_ਮੰਜ਼ਿਲ_ਉੱਤਲਾ(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_ਅਣਵਰਤਿਆ
                             ਅਚਲ uint8_t *src, int32_t src_ਲੰਮਾਈ,
                             icu::ByteSink &sink, icu::Edits *edits,
                             UError_ਕੋਡ &error_ਕੋਡ) {
    ਜੇ (caseLocale == UCASE_LOC_GREEK) {
        Greek_ਉੱਤਲਾ::ਮੰਜ਼ਿਲ_ਉੱਤਲਾ(options, src, src_ਲੰਮਾਈ, sink, edits, error_ਕੋਡ);
    } ਨਹੀਂਤਾਂ {
        UCaseContext csc=UCASECONTEXT_INITIALIZER;
        csc.p=(ਖਾਲੀ *)src;
        csc.ਹੱਦਬੰਦੀ=src_ਲੰਮਾਈ;
        ਮੰਜ਼ਿਲ_ਉੱਤਲਾ(
            caseLocale, options,
            src, &csc, src_ਲੰਮਾਈ,
            sink, edits, error_ਕੋਡ);
    }
}

ਸਥਿਰ ਖਾਲੀ U_CALLCONV
ucasemap_ਅੰਦਰੂਨੀ_UTF8_Fold(int32_t /* caseLocale */, uint32_t options, UCASEMAP_BREAK_ITERATOR_ਅਣਵਰਤਿਆ
                          ਅਚਲ uint8_t *src, int32_t src_ਲੰਮਾਈ,
                          icu::ByteSink &sink, icu::Edits *edits,
                          UError_ਕੋਡ &error_ਕੋਡ) {
    ਮੰਜ਼ਿਲ_ਹੇਠਲਾ(
        -1, options,
        src, ਖਾਲੀ_ਸੂਚਕ, 0, src_ਲੰਮਾਈ,
        sink, edits, error_ਕੋਡ);
}

ਖਾਲੀ
ucasemap_mapUTF8(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_PARAM
                 ਅਚਲ ਅੱਖਰ *src, int32_t src_ਲੰਮਾਈ,
                 UTF8CaseMapper *stringCaseMapper,
                 icu::ByteSink &sink, icu::Edits *edits,
                 UError_ਕੋਡ &error_ਕੋਡ) {
    /* check argument values */
    ਜੇ (U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ;
    }
    ਜੇ ((src == ਖਾਲੀ_ਸੂਚਕ && src_ਲੰਮਾਈ != 0) || src_ਲੰਮਾਈ < -1) {
        error_ਕੋਡ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ;
    }

    // Get the string length.
    ਜੇ (src_ਲੰਮਾਈ == -1) {
        src_ਲੰਮਾਈ = static_cast<int32_t>(uprv_strlen(src));
    }

    ਜੇ (edits != ਖਾਲੀ_ਸੂਚਕ && (options & U_EDITS_NO_RESET) == 0) {
        edits->reset();
    }
    stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                     reinterpret_cast<ਅਚਲ uint8_t*>(src), src_ਲੰਮਾਈ, sink, edits, error_ਕੋਡ);
    sink.Flush();
    ਜੇ (U_SUCCESS(error_ਕੋਡ)) {
        ਜੇ (edits != ਖਾਲੀ_ਸੂਚਕ) {
            edits->copy_Error_ਮੰਜ਼ਿਲ(error_ਕੋਡ);
        }
    }
}

int32_t
ucasemap_mapUTF8(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_PARAM
                 ਅੱਖਰ *dest, int32_t dest_ਸਮਰੱਥਾ,
                 ਅਚਲ ਅੱਖਰ *src, int32_t src_ਲੰਮਾਈ,
                 UTF8CaseMapper *stringCaseMapper,
                 icu::Edits *edits,
                 UError_ਕੋਡ &error_ਕੋਡ) {
    /* check argument values */
    ਜੇ(U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ 0;
    }
    ਜੇ( dest_ਸਮਰੱਥਾ<0 ||
        (dest==ਖਾਲੀ_ਸੂਚਕ && dest_ਸਮਰੱਥਾ>0) ||
        (src==ਖਾਲੀ_ਸੂਚਕ && src_ਲੰਮਾਈ!=0) || src_ਲੰਮਾਈ<-1
    ) {
        error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ 0;
    }

    /* get the string length */
    ਜੇ(src_ਲੰਮਾਈ==-1) {
        src_ਲੰਮਾਈ = static_cast<int32_t>(uprv_strlen(src));
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
    int32_t reslen = ByteSinkUtil::ਰਾਹੀਂ_Byte_Sink_ਮੰਜ਼ਿਲ_Terminated_Chars(
        dest, dest_ਸਮਰੱਥਾ,
        [&](ByteSink& sink, UError_ਕੋਡ& ਸਥਿਤੀ) {
            stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                             reinterpret_cast<ਅਚਲ uint8_t*>(src), src_ਲੰਮਾਈ, sink, edits, ਸਥਿਤੀ);
        },
        error_ਕੋਡ);
    ਜੇ (U_SUCCESS(error_ਕੋਡ) && edits != ਖਾਲੀ_ਸੂਚਕ) {
        edits->copy_Error_ਮੰਜ਼ਿਲ(error_ਕੋਡ);
    }
    ਵਾਪਸ reslen;
}

/* public API functions */

U_CAPI int32_t U_EXPORT2
ucasemap_utf8_ਮੰਜ਼ਿਲ_ਹੇਠਲਾ(ਅਚਲ UCaseMap *csm,
                     ਅੱਖਰ *dest, int32_t dest_ਸਮਰੱਥਾ,
                     ਅਚਲ ਅੱਖਰ *src, int32_t src_ਲੰਮਾਈ,
                     UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਸਵੈਚਾਲਿਤ *impl = reinterpret_cast<ਅਚਲ UCaseMapImpl *>(csm);
    ਵਾਪਸ ucasemap_mapUTF8(
        impl->caseLocale, impl->options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_ਸਮਰੱਥਾ,
        src, src_ਲੰਮਾਈ,
        ucasemap_ਅੰਦਰੂਨੀ_UTF8_ਮੰਜ਼ਿਲ_ਹੇਠਲਾ, ਖਾਲੀ_ਸੂਚਕ, *p_Error_ਕੋਡ);
}

U_CAPI int32_t U_EXPORT2
ucasemap_utf8_ਮੰਜ਼ਿਲ_ਉੱਤਲਾ(ਅਚਲ UCaseMap *csm,
                     ਅੱਖਰ *dest, int32_t dest_ਸਮਰੱਥਾ,
                     ਅਚਲ ਅੱਖਰ *src, int32_t src_ਲੰਮਾਈ,
                     UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਸਵੈਚਾਲਿਤ *impl = reinterpret_cast<ਅਚਲ UCaseMapImpl *>(csm);
    ਵਾਪਸ ucasemap_mapUTF8(
        impl->caseLocale, impl->options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_ਸਮਰੱਥਾ,
        src, src_ਲੰਮਾਈ,
        ucasemap_ਅੰਦਰੂਨੀ_UTF8_ਮੰਜ਼ਿਲ_ਉੱਤਲਾ, ਖਾਲੀ_ਸੂਚਕ, *p_Error_ਕੋਡ);
}

U_CAPI int32_t U_EXPORT2
ucasemap_utf8FoldCase(ਅਚਲ UCaseMap *csm,
                      ਅੱਖਰ *dest, int32_t dest_ਸਮਰੱਥਾ,
                      ਅਚਲ ਅੱਖਰ *src, int32_t src_ਲੰਮਾਈ,
                      UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਸਵੈਚਾਲਿਤ *impl = reinterpret_cast<ਅਚਲ UCaseMapImpl *>(csm);
    ਵਾਪਸ ucasemap_mapUTF8(
        UCASE_LOC_ਜੜ੍ਹ, impl->options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_ਸਮਰੱਥਾ,
        src, src_ਲੰਮਾਈ,
        ucasemap_ਅੰਦਰੂਨੀ_UTF8_Fold, ਖਾਲੀ_ਸੂਚਕ, *p_Error_ਕੋਡ);
}

U_NAMESPACE_BEGIN

ਖਾਲੀ CaseMap::utf8_ਮੰਜ਼ਿਲ_ਹੇਠਲਾ(
        ਅਚਲ ਅੱਖਰ *locale, uint32_t options,
        StringPiece src, ByteSink &sink, Edits *edits,
        UError_ਕੋਡ &error_ਕੋਡ) {
    ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        src.ਅੰਕੜੇ(), src.length(),
        ucasemap_ਅੰਦਰੂਨੀ_UTF8_ਮੰਜ਼ਿਲ_ਹੇਠਲਾ, sink, edits, error_ਕੋਡ);
}

ਖਾਲੀ CaseMap::utf8_ਮੰਜ਼ਿਲ_ਉੱਤਲਾ(
        ਅਚਲ ਅੱਖਰ *locale, uint32_t options,
        StringPiece src, ByteSink &sink, Edits *edits,
        UError_ਕੋਡ &error_ਕੋਡ) {
    ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        src.ਅੰਕੜੇ(), src.length(),
        ucasemap_ਅੰਦਰੂਨੀ_UTF8_ਮੰਜ਼ਿਲ_ਉੱਤਲਾ, sink, edits, error_ਕੋਡ);
}

ਖਾਲੀ CaseMap::utf8Fold(
        uint32_t options,
        StringPiece src, ByteSink &sink, Edits *edits,
        UError_ਕੋਡ &error_ਕੋਡ) {
    ucasemap_mapUTF8(
        UCASE_LOC_ਜੜ੍ਹ, options, UCASEMAP_BREAK_ITERATOR_NULL
        src.ਅੰਕੜੇ(), src.length(),
        ucasemap_ਅੰਦਰੂਨੀ_UTF8_Fold, sink, edits, error_ਕੋਡ);
}

int32_t CaseMap::utf8_ਮੰਜ਼ਿਲ_ਹੇਠਲਾ(
        ਅਚਲ ਅੱਖਰ *locale, uint32_t options,
        ਅਚਲ ਅੱਖਰ *src, int32_t src_ਲੰਮਾਈ,
        ਅੱਖਰ *dest, int32_t dest_ਸਮਰੱਥਾ, Edits *edits,
        UError_ਕੋਡ &error_ਕੋਡ) {
    ਵਾਪਸ ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_ਸਮਰੱਥਾ,
        src, src_ਲੰਮਾਈ,
        ucasemap_ਅੰਦਰੂਨੀ_UTF8_ਮੰਜ਼ਿਲ_ਹੇਠਲਾ, edits, error_ਕੋਡ);
}

int32_t CaseMap::utf8_ਮੰਜ਼ਿਲ_ਉੱਤਲਾ(
        ਅਚਲ ਅੱਖਰ *locale, uint32_t options,
        ਅਚਲ ਅੱਖਰ *src, int32_t src_ਲੰਮਾਈ,
        ਅੱਖਰ *dest, int32_t dest_ਸਮਰੱਥਾ, Edits *edits,
        UError_ਕੋਡ &error_ਕੋਡ) {
    ਵਾਪਸ ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_ਸਮਰੱਥਾ,
        src, src_ਲੰਮਾਈ,
        ucasemap_ਅੰਦਰੂਨੀ_UTF8_ਮੰਜ਼ਿਲ_ਉੱਤਲਾ, edits, error_ਕੋਡ);
}

int32_t CaseMap::utf8Fold(
        uint32_t options,
        ਅਚਲ ਅੱਖਰ *src, int32_t src_ਲੰਮਾਈ,
        ਅੱਖਰ *dest, int32_t dest_ਸਮਰੱਥਾ, Edits *edits,
        UError_ਕੋਡ &error_ਕੋਡ) {
    ਵਾਪਸ ucasemap_mapUTF8(
        UCASE_LOC_ਜੜ੍ਹ, options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_ਸਮਰੱਥਾ,
        src, src_ਲੰਮਾਈ,
        ucasemap_ਅੰਦਰੂਨੀ_UTF8_Fold, edits, error_ਕੋਡ);
}

U_NAMESPACE_ਸਿਰਾ
