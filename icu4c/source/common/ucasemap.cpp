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

#ضمن "unicode/utypes.h"
#ضمن "unicode/brkiter.h"
#ضمن "unicode/bytestream.h"
#ضمن "unicode/casemap.h"
#ضمن "unicode/edits.h"
#ضمن "unicode/stringoptions.h"
#ضمن "unicode/stringpiece.h"
#ضمن "unicode/ubrk.h"
#ضمن "unicode/uloc.h"
#ضمن "unicode/ustring.h"
#ضمن "unicode/ucasemap.h"
#إذا !UCONFIG_NO_BREAK_ITERATION
#ضمن "unicode/utext.h"
#جے_انت
#ضمن "unicode/utf.h"
#ضمن "unicode/utf8.h"
#ضمن "unicode/utf16.h"
#ضمن "bytesinkutil.h"
#ضمن "cmemory.h"
#ضمن "cstring.h"
#ضمن "uassert.h"
#ضمن "ucase.h"
#ضمن "ucasemap_imp.h"

U_NAMESPACE_USE

/* UCaseMap service object -------------------------------------------------- */

// The public type, unicode/ucasemap.h struct UCaseMap, is an opaque C type.
// We define a parallel C++ type in the ICU namespace for entry point renaming.

U_NAMESPACE_BEGIN

UCaseMapImpl::UCaseMapImpl(ثابت حرف *localeID, uint32_t opts, UError_کوڈ *p_Error_کوڈ) :
#إذا !UCONFIG_NO_BREAK_ITERATION
        iter(خالی_سوچک),
#جے_انت
        caseLocale(UCASE_LOC_UNKNOWN), options(opts) {
    ucasemap_سیٹ_Locale(reinterpret_cast<UCaseMap *>(هذا), localeID, p_Error_کوڈ);
}

UCaseMapImpl::~UCaseMapImpl() {
#إذا !UCONFIG_NO_BREAK_ITERATION
    مٹاؤ iter;
#جے_انت
}

U_NAMESPACE_سرا

U_CAPI UCaseMap * U_EXPORT2
ucasemap_open(ثابت حرف *locale, uint32_t options, UError_کوڈ *p_Error_کوڈ) {
    إذا(U_FAILURE(*p_Error_کوڈ)) {
        أرجع خالی_سوچک;
    }
    UCaseMapImpl *csm = جديد UCaseMapImpl(locale, options, p_Error_کوڈ);
    إذا(csm==خالی_سوچک) {
        *p_Error_کوڈ = U_حافظہ_ونڈ_ERROR;
        أرجع خالی_سوچک;
    } وإلا إذا (U_FAILURE(*p_Error_کوڈ)) {
        مٹاؤ csm;
        أرجع خالی_سوچک;
    }
    أرجع reinterpret_cast<UCaseMap *>(csm);
}

U_CAPI فراغ U_EXPORT2
ucasemap_close(UCaseMap *csm) {
    مٹاؤ reinterpret_cast<UCaseMapImpl *>(csm);
}

U_CAPI ثابت حرف* U_EXPORT2
ucasemap_getLocale(ثابت UCaseMap* csm U_LIFETIME_BOUND) {
    أرجع reinterpret_cast<ثابت UCaseMapImpl *>(csm)->locale;
}

U_CAPI uint32_t U_EXPORT2
ucasemap_getOptions(ثابت UCaseMap *csm) {
    أرجع reinterpret_cast<ثابت UCaseMapImpl *>(csm)->options;
}

U_CAPI فراغ U_EXPORT2
ucasemap_سیٹ_Locale(UCaseMap *csm, ثابت حرف *locale, UError_کوڈ *p_Error_کوڈ) {
    إذا(U_FAILURE(*p_Error_کوڈ)) {
        أرجع;
    }
    خودکار *impl = reinterpret_cast<UCaseMapImpl *>(csm);
    إذا (locale != خالی_سوچک && *locale == 0) {
        impl->locale[0] = 0;
        impl->caseLocale = UCASE_LOC_جڑ;
        أرجع;
    }

    UError_کوڈ بفر_حالت = U_ZERO_ERROR;
    int32_t length=uloc_getName(locale, impl->locale, (int32_t)حجم(impl->locale), &بفر_حالت);
    إذا(بفر_حالت==U_بفر_حد_توں_ودھ_ERROR || (U_SUCCESS(بفر_حالت) && length==حجم(impl->locale))) {
        بفر_حالت = U_ZERO_ERROR;
        /* we only really need the language code for case mappings */
        length=uloc_getLanguage(locale, impl->locale, (int32_t)حجم(impl->locale), &بفر_حالت);
    }
    إذا(U_FAILURE(بفر_حالت)) {
        *p_Error_کوڈ=بفر_حالت;
    } وإلا إذا(length==حجم(impl->locale)) {
        *p_Error_کوڈ=U_بفر_حد_توں_ودھ_ERROR;
    }
    إذا(U_SUCCESS(*p_Error_کوڈ)) {     
        impl->caseLocale = ucase_getCaseLocale(impl->locale);
    } وإلا {
        impl->locale[0]=0;
        impl->caseLocale = UCASE_LOC_جڑ;
    }
}

U_CAPI فراغ U_EXPORT2
ucasemap_سیٹ_Options(UCaseMap *csm, uint32_t options, UError_کوڈ *p_Error_کوڈ) {
    إذا(U_FAILURE(*p_Error_کوڈ)) {
        أرجع;
    }
    reinterpret_cast<UCaseMapImpl *>(csm)->options=options;
}

/* UTF-8 string case mappings ----------------------------------------------- */

/* TODO(markus): Move to a new, separate utf8case.cpp file. */

نطاق {

/* append a full case mapping result, see UCASE_MAX_STRING_LENGTH */
مضمن UBool
append_نتیجہ(int32_t cp_لمائی, int32_t نتیجہ, ثابت char16_t *s,
             ByteSink &sink, uint32_t options, icu::Edits *edits, UError_کوڈ &error_کوڈ) {
    U_ASSERT(U_SUCCESS(error_کوڈ));

    /* decode the result */
    إذا(نتیجہ<0) {
        /* (not) original code point */
        إذا(edits!=خالی_سوچک) {
            edits->جوڑو_Unchanged(cp_لمائی);
        }
        إذا((options & U_OMIT_UNCHANGED_متن) == 0) {
            ByteSinkUtil::append_کوڈ_نقطہ(cp_لمائی, ~نتیجہ, sink);
        }
    } وإلا {
        إذا(نتیجہ<=UCASE_MAX_STRING_لمائی) {
            // string: "result" is the UTF-16 length
            أرجع ByteSinkUtil::appendChange(cp_لمائی, s, نتیجہ, sink, edits, error_کوڈ);
        } وإلا {
            ByteSinkUtil::append_کوڈ_نقطہ(cp_لمائی, نتیجہ, sink, edits);
        }
    }
    أرجع سچ;
}

// See unicode/utf8.h U8_APPEND_UNSAFE().
مضمن uint8_t getTwoByteLead(UChar32 c) { أرجع static_cast<uint8_t>((c >> 6) | 0xc0); }
مضمن uint8_t getTwoByteTrail(UChar32 c) { أرجع static_cast<uint8_t>((c & 0x3f) | 0x80); }

UChar32 U_CALLCONV
utf8_caseContextIterator(فراغ *context, int8_t dir) {
    UCaseContext* csc = static_cast<UCaseContext*>(context);
    UChar32 c;

    إذا(dir<0) {
        /* reset for backward iteration */
        csc->اشاریہ=csc->cp_شروع;
        csc->dir=dir;
    } وإلا إذا(dir>0) {
        /* reset for forward iteration */
        csc->اشاریہ=csc->cp_حد_بندی;
        csc->dir=dir;
    } وإلا {
        /* continue current iteration direction */
        dir=csc->dir;
    }

    إذا(dir<0) {
        إذا(csc->شروع<csc->اشاریہ) {
            U8_PREV((ثابت uint8_t *)csc->p, csc->شروع, csc->اشاریہ, c);
            أرجع c;
        }
    } وإلا {
        إذا(csc->اشاریہ<csc->حد_بندی) {
            U8_اگلا((ثابت uint8_t *)csc->p, csc->اشاریہ, csc->حد_بندی, c);
            أرجع c;
        }
    }
    أرجع U_SENTINEL;
}

/**
 * caseLocale >= 0: Lowercases [srcStart..srcLimit[ but takes context [0..srcLength[ into account.
 * caseLocale < 0: Case-folds [srcStart..srcLimit[.
 */
فراغ منزل_ہیٹھلا(int32_t caseLocale, uint32_t options,
             ثابت uint8_t *src, UCaseContext *csc, int32_t src_شروع, int32_t src_حد_بندی,
             icu::ByteSink &sink, icu::Edits *edits, UError_کوڈ &error_کوڈ) {
    ثابت int8_t *latin_منزل_ہیٹھلا;
    إذا (caseLocale == UCASE_LOC_جڑ ||
            (caseLocale >= 0 ?
                !(caseLocale == UCASE_LOC_TURKISH || caseLocale == UCASE_LOC_LITHUANIAN) :
                (options & FOLD_CASE_OPTIONS_نقاب) == U_FOLD_CASE_DEFAULT)) {
        latin_منزل_ہیٹھلا = LatinCase::منزل_ہیٹھلا_عام;
    } وإلا {
        latin_منزل_ہیٹھلا = LatinCase::منزل_ہیٹھلا_TR_LT;
    }
    ثابت UTrie2 *trie = ucase_getTrie();
    int32_t prev = src_شروع;
    int32_t src_اشاریہ = src_شروع;
    لكل (;;) {
        // fast path for simple cases
        int32_t cp_شروع;
        UChar32 c;
        لكل (;;) {
            إذا (U_FAILURE(error_کوڈ) || src_اشاریہ >= src_حد_بندی) {
                c = U_SENTINEL;
                اكسر;
            }
            uint8_t lead = src[src_اشاریہ++];
            إذا (lead <= 0x7f) {
                int8_t d = latin_منزل_ہیٹھلا[lead];
                إذا (d == LatinCase::EXC) {
                    cp_شروع = src_اشاریہ - 1;
                    c = lead;
                    اكسر;
                }
                إذا (d == 0) { استمر; }
                ByteSinkUtil::appendUnchanged(src + prev, src_اشاریہ - 1 - prev,
                                              sink, options, edits, error_کوڈ);
                حرف ascii = static_cast<حرف>(lead + d);
                sink.Append(&ascii, 1);
                إذا (edits != خالی_سوچک) {
                    edits->جوڑو_Replace(1, 1);
                }
                prev = src_اشاریہ;
                استمر;
            } وإلا إذا (lead < 0xe3) {
                uint8_t t;
                إذا (0xc2 <= lead && lead <= 0xc5 && src_اشاریہ < src_حد_بندی &&
                        (t = src[src_اشاریہ] - 0x80) <= 0x3f) {
                    // U+0080..U+017F
                    ++src_اشاریہ;
                    c = ((lead - 0xc0) << 6) | t;
                    int8_t d = latin_منزل_ہیٹھلا[c];
                    إذا (d == LatinCase::EXC) {
                        cp_شروع = src_اشاریہ - 2;
                        اكسر;
                    }
                    إذا (d == 0) { استمر; }
                    ByteSinkUtil::appendUnchanged(src + prev, src_اشاریہ - 2 - prev,
                                                  sink, options, edits, error_کوڈ);
                    ByteSinkUtil::append_Two_بائٹاں(c + d, sink);
                    إذا (edits != خالی_سوچک) {
                        edits->جوڑو_Replace(2, 2);
                    }
                    prev = src_اشاریہ;
                    استمر;
                }
            } وإلا إذا ((lead <= 0xe9 || lead == 0xeb || lead == 0xec) &&
                    (src_اشاریہ + 2) <= src_حد_بندی &&
                    U8_IS_TRAIL(src[src_اشاریہ]) && U8_IS_TRAIL(src[src_اشاریہ + 1])) {
                // most of CJK: no case mappings
                src_اشاریہ += 2;
                استمر;
            }
            cp_شروع = --src_اشاریہ;
            U8_اگلا(src, src_اشاریہ, src_حد_بندی, c);
            إذا (c < 0) {
                // ill-formed UTF-8
                استمر;
            }
            uint16_t props = UTRIE2_GET16(trie, c);
            إذا (UCASE_HAS_EXCEPTION(props)) { اكسر; }
            int32_t delta;
            إذا (!UCASE_IS_اتلا_OR_TITLE(props) || (delta = UCASE_GET_DELTA(props)) == 0) {
                استمر;
            }
            ByteSinkUtil::appendUnchanged(src + prev, cp_شروع - prev,
                                          sink, options, edits, error_کوڈ);
            ByteSinkUtil::append_کوڈ_نقطہ(src_اشاریہ - cp_شروع, c + delta, sink, edits);
            prev = src_اشاریہ;
        }
        إذا (c < 0) {
            اكسر;
        }
        // slow path
        ثابت char16_t *s;
        إذا (caseLocale >= 0) {
            csc->cp_شروع = cp_شروع;
            csc->cp_حد_بندی = src_اشاریہ;
            c = ucase_منزل_بھریا_ہیٹھلا(c, utf8_caseContextIterator, csc, &s, caseLocale);
        } وإلا {
            c = ucase_منزل_بھریا_Folding(c, &s, options);
        }
        إذا (c >= 0) {
            ByteSinkUtil::appendUnchanged(src + prev, cp_شروع - prev,
                                          sink, options, edits, error_کوڈ);
            append_نتیجہ(src_اشاریہ - cp_شروع, c, s, sink, options, edits, error_کوڈ);
            prev = src_اشاریہ;
        }
    }
    ByteSinkUtil::appendUnchanged(src + prev, src_اشاریہ - prev,
                                  sink, options, edits, error_کوڈ);
}

فراغ منزل_اتلا(int32_t caseLocale, uint32_t options,
             ثابت uint8_t *src, UCaseContext *csc, int32_t src_لمائی,
             icu::ByteSink &sink, icu::Edits *edits, UError_کوڈ &error_کوڈ) {
    ثابت int8_t *latin_منزل_اتلا;
    إذا (caseLocale == UCASE_LOC_TURKISH) {
        latin_منزل_اتلا = LatinCase::منزل_اتلا_TR;
    } وإلا {
        latin_منزل_اتلا = LatinCase::منزل_اتلا_عام;
    }
    ثابت UTrie2 *trie = ucase_getTrie();
    int32_t prev = 0;
    int32_t src_اشاریہ = 0;
    لكل (;;) {
        // fast path for simple cases
        int32_t cp_شروع;
        UChar32 c;
        لكل (;;) {
            إذا (U_FAILURE(error_کوڈ) || src_اشاریہ >= src_لمائی) {
                c = U_SENTINEL;
                اكسر;
            }
            uint8_t lead = src[src_اشاریہ++];
            إذا (lead <= 0x7f) {
                int8_t d = latin_منزل_اتلا[lead];
                إذا (d == LatinCase::EXC) {
                    cp_شروع = src_اشاریہ - 1;
                    c = lead;
                    اكسر;
                }
                إذا (d == 0) { استمر; }
                ByteSinkUtil::appendUnchanged(src + prev, src_اشاریہ - 1 - prev,
                                              sink, options, edits, error_کوڈ);
                حرف ascii = static_cast<حرف>(lead + d);
                sink.Append(&ascii, 1);
                إذا (edits != خالی_سوچک) {
                    edits->جوڑو_Replace(1, 1);
                }
                prev = src_اشاریہ;
                استمر;
            } وإلا إذا (lead < 0xe3) {
                uint8_t t;
                إذا (0xc2 <= lead && lead <= 0xc5 && src_اشاریہ < src_لمائی &&
                        (t = src[src_اشاریہ] - 0x80) <= 0x3f) {
                    // U+0080..U+017F
                    ++src_اشاریہ;
                    c = ((lead - 0xc0) << 6) | t;
                    int8_t d = latin_منزل_اتلا[c];
                    إذا (d == LatinCase::EXC) {
                        cp_شروع = src_اشاریہ - 2;
                        اكسر;
                    }
                    إذا (d == 0) { استمر; }
                    ByteSinkUtil::appendUnchanged(src + prev, src_اشاریہ - 2 - prev,
                                                  sink, options, edits, error_کوڈ);
                    ByteSinkUtil::append_Two_بائٹاں(c + d, sink);
                    إذا (edits != خالی_سوچک) {
                        edits->جوڑو_Replace(2, 2);
                    }
                    prev = src_اشاریہ;
                    استمر;
                }
            } وإلا إذا ((lead <= 0xe9 || lead == 0xeb || lead == 0xec) &&
                    (src_اشاریہ + 2) <= src_لمائی &&
                    U8_IS_TRAIL(src[src_اشاریہ]) && U8_IS_TRAIL(src[src_اشاریہ + 1])) {
                // most of CJK: no case mappings
                src_اشاریہ += 2;
                استمر;
            }
            cp_شروع = --src_اشاریہ;
            U8_اگلا(src, src_اشاریہ, src_لمائی, c);
            إذا (c < 0) {
                // ill-formed UTF-8
                استمر;
            }
            uint16_t props = UTRIE2_GET16(trie, c);
            إذا (UCASE_HAS_EXCEPTION(props)) { اكسر; }
            int32_t delta;
            إذا (UCASE_GET_TYPE(props) != UCASE_ہیٹھلا || (delta = UCASE_GET_DELTA(props)) == 0) {
                استمر;
            }
            ByteSinkUtil::appendUnchanged(src + prev, cp_شروع - prev,
                                          sink, options, edits, error_کوڈ);
            ByteSinkUtil::append_کوڈ_نقطہ(src_اشاریہ - cp_شروع, c + delta, sink, edits);
            prev = src_اشاریہ;
        }
        إذا (c < 0) {
            اكسر;
        }
        // slow path
        csc->cp_شروع = cp_شروع;
        csc->cp_حد_بندی = src_اشاریہ;
        ثابت char16_t *s;
        c = ucase_منزل_بھریا_اتلا(c, utf8_caseContextIterator, csc, &s, caseLocale);
        إذا (c >= 0) {
            ByteSinkUtil::appendUnchanged(src + prev, cp_شروع - prev,
                                          sink, options, edits, error_کوڈ);
            append_نتیجہ(src_اشاریہ - cp_شروع, c, s, sink, options, edits, error_کوڈ);
            prev = src_اشاریہ;
        }
    }
    ByteSinkUtil::appendUnchanged(src + prev, src_اشاریہ - prev,
                                  sink, options, edits, error_کوڈ);
}

}  // namespace

#إذا !UCONFIG_NO_BREAK_ITERATION

نطاق {

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
int32_t maybeTitleDutchIJ(ثابت uint8_t *src, UChar32 c, int32_t شروع, int32_t segment_حد_بندی,
                          ByteSink &sink, uint32_t options, icu::Edits *edits, UError_کوڈ &error_کوڈ) {
    U_ASSERT(شروع < segment_حد_بندی);

    int32_t اشاریہ = شروع;
    منطقي withAcute = خطأ;

    // If the conditions are met, then the following variables tell us what to output.
    int32_t unchanged1 = 0;  // code units before the j, or the whole sequence (0..3)
    منطقي doTitleJ = خطأ;  // true if the j needs to be titlecased
    int32_t unchanged2 = 0;  // after the j (0 or 1)

    // next character after the first letter
    UChar32 c2;
    c2 = src[اشاریہ++];

    // Is the first letter an i/I with accent?
    إذا (c == u'I') {
        إذا (c2 == ACUTE_BYTE0 && اشاریہ < segment_حد_بندی && src[اشاریہ++] == ACUTE_BYTE1) {
            withAcute = سچ;
            unchanged1 = 2;  // ACUTE is 2 code units in UTF-8
            إذا (اشاریہ == segment_حد_بندی) { أرجع شروع; }
            c2 = src[اشاریہ++];
        }
    } وإلا {  // Í
        withAcute = سچ;
    }

    // Is the next character a j/J?
    إذا (c2 == u'j') {
        doTitleJ = سچ;
    } وإلا إذا (c2 == u'J') {
        ++unchanged1;
    } وإلا {
        أرجع شروع;
    }

    // A plain i/I must be followed by a plain j/J.
    // An i/I with acute must be followed by a j/J with acute.
    إذا (withAcute) {
        إذا ((اشاریہ + 1) >= segment_حد_بندی || src[اشاریہ++] != ACUTE_BYTE0 || src[اشاریہ++] != ACUTE_BYTE1) {
            أرجع شروع;
        }
        إذا (doTitleJ) {
            unchanged2 = 2;  // ACUTE is 2 code units in UTF-8
        } وإلا {
            unchanged1 = unchanged1 + 2;    // ACUTE is 2 code units in UTF-8
        }
    }

    // There must not be another combining mark.
    إذا (اشاریہ < segment_حد_بندی) {
        int32_t cp;
        int32_t i = اشاریہ;
        U8_اگلا(src, i, segment_حد_بندی, cp);
        uint32_t type_نقاب = U_GET_GC_نقاب(cp);
        إذا ((type_نقاب & U_GC_M_نقاب) != 0) {
            أرجع شروع;
        }
    }

    // Output the rest of the Dutch IJ.
    ByteSinkUtil::appendUnchanged(src + شروع, unchanged1, sink, options, edits, error_کوڈ);
    شروع += unchanged1;
    إذا (doTitleJ) {
        ByteSinkUtil::append_کوڈ_نقطہ(1, u'J', sink, edits);
        ++شروع;
    }
    ByteSinkUtil::appendUnchanged(src + شروع, unchanged2, sink, options, edits, error_کوڈ);

    U_ASSERT(شروع + unchanged2 == اشاریہ);
    أرجع اشاریہ;
}

}  // namespace

U_CFUNC فراغ U_CALLCONV
ucasemap_اندرونی_UTF8_منزل_Title(
        int32_t caseLocale, uint32_t options, BreakIterator *iter,
        ثابت uint8_t *src, int32_t src_لمائی,
        ByteSink &sink, icu::Edits *edits,
        UError_کوڈ &error_کوڈ) {
    إذا (!ustrcase_checkTitleAdjustmentOptions(options, error_کوڈ)) {
        أرجع;
    }

    /* set up local variables */
    UCaseContext csc=UCASECONTEXT_INITIALIZER;
    csc.p=(فراغ *)src;
    csc.حد_بندی=src_لمائی;
    int32_t prev=0;
    UBool is_پہلا_اشاریہ=سچ;

    /* titlecasing loop */
    طالما(prev<src_لمائی) {
        /* find next index where to titlecase */
        int32_t اشاریہ;
        إذا(is_پہلا_اشاریہ) {
            is_پہلا_اشاریہ=خطأ;
            اشاریہ=iter->first();
        } وإلا {
            اشاریہ=iter->next();
        }
        إذا(اشاریہ==UBRK_پورا || اشاریہ>src_لمائی) {
            اشاریہ=src_لمائی;
        }

        /*
         * Segment [prev..index[ into 3 parts:
         * a) skipped characters (copy as-is) [prev..titleStart[
         * b) first letter (titlecase)              [titleStart..titleLimit[
         * c) subsequent characters (lowercase)                 [titleLimit..index[
         */
        إذا(prev<اشاریہ) {
            /* find and copy skipped characters [prev..titleStart[ */
            int32_t title_شروع=prev;
            int32_t title_حد_بندی=prev;
            UChar32 c;
            U8_اگلا(src, title_حد_بندی, اشاریہ, c);
            إذا ((options&U_TITLECASE_NO_BREAK_ADJUSTMENT)==0) {
                // Adjust the titlecasing index to the next cased character,
                // or to the next letter/number/symbol/private use.
                // Stop with titleStart<titleLimit<=index
                // if there is a character to be titlecased,
                // or else stop with titleStart==titleLimit==index.
                UBool منزل_Cased = (options&U_TITLECASE_ADJUST_منزل_CASED) != 0;
                طالما (منزل_Cased ? UCASE_NONE==ucase_getType(c) : !ustrcase_isLNS(c)) {
                    title_شروع=title_حد_بندی;
                    إذا(title_حد_بندی==اشاریہ) {
                        اكسر;
                    }
                    U8_اگلا(src, title_حد_بندی, اشاریہ, c);
                }
                إذا (prev < title_شروع) {
                    إذا (!ByteSinkUtil::appendUnchanged(src+prev, title_شروع-prev,
                                                       sink, options, edits, error_کوڈ)) {
                        أرجع;
                    }
                }
            }

            إذا(title_شروع<title_حد_بندی) {
                /* titlecase c which is from [titleStart..titleLimit[ */
                إذا(c>=0) {
                    csc.cp_شروع=title_شروع;
                    csc.cp_حد_بندی=title_حد_بندی;
                    ثابت char16_t *s;
                    c=ucase_منزل_بھریا_Title(c, utf8_caseContextIterator, &csc, &s, caseLocale);
                    إذا (!append_نتیجہ(title_حد_بندی-title_شروع, c, s, sink, options, edits, error_کوڈ)) {
                        أرجع;
                    }
                } وإلا {
                    // Malformed UTF-8.
                    إذا (!ByteSinkUtil::appendUnchanged(src+title_شروع, title_حد_بندی-title_شروع,
                                                       sink, options, edits, error_کوڈ)) {
                        أرجع;
                    }
                }

                /* Special case Dutch IJ titlecasing */
                إذا (title_حد_بندی < اشاریہ &&
                    caseLocale == UCASE_LOC_DUTCH) {
                    إذا (c < 0) {
                        c = ~c;
                    }

                    إذا (c == u'I' || c == u'Í') {
                        title_حد_بندی = maybeTitleDutchIJ(src, c, title_حد_بندی, اشاریہ, sink, options, edits, error_کوڈ);
                    }
                }

                /* lowercase [titleLimit..index[ */
                إذا(title_حد_بندی<اشاریہ) {
                    إذا((options&U_TITLECASE_NO_LOWERCASE)==0) {
                        /* Normal operation: Lowercase the rest of the word. */
                        منزل_ہیٹھلا(caseLocale, options,
                                src, &csc, title_حد_بندی, اشاریہ,
                                sink, edits, error_کوڈ);
                        إذا(U_FAILURE(error_کوڈ)) {
                            أرجع;
                        }
                    } وإلا {
                        /* Optionally just copy the rest of the word unchanged. */
                        إذا (!ByteSinkUtil::appendUnchanged(src+title_حد_بندی, اشاریہ-title_حد_بندی,
                                                           sink, options, edits, error_کوڈ)) {
                            أرجع;
                        }
                    }
                }
            }
        }

        prev=اشاریہ;
    }
}

#جے_انت

U_NAMESPACE_BEGIN
نطاق Greek_اتلا {

UBool isFollowedByCasedLetter(ثابت uint8_t *s, int32_t i, int32_t length) {
    طالما (i < length) {
        UChar32 c;
        U8_اگلا(s, i, length, c);
        int32_t type = ucase_getTypeOrIgnorable(c);
        إذا ((type & UCASE_IGNORABLE) != 0) {
            // Case-ignorable, continue with the loop.
        } وإلا إذا (type != UCASE_NONE) {
            أرجع سچ;  // Followed by cased letter.
        } وإلا {
            أرجع خطأ;  // Uncased and not case-ignorable.
        }
    }
    أرجع خطأ;  // Not followed by cased letter.
}

// Keep this consistent with the UTF-16 version in ustrcase.cpp and the Java version in CaseMap.java.
فراغ منزل_اتلا(uint32_t options,
             ثابت uint8_t *src, int32_t src_لمائی,
             ByteSink &sink, Edits *edits,
             UError_کوڈ &error_کوڈ) {
    uint32_t state = 0;
    لكل (int32_t i = 0; i < src_لمائی;) {
        int32_t اگلا_اشاریہ = i;
        UChar32 c;
        U8_اگلا(src, اگلا_اشاریہ, src_لمائی, c);
        uint32_t اگلا_State = 0;
        int32_t type = ucase_getTypeOrIgnorable(c);
        إذا ((type & UCASE_IGNORABLE) != 0) {
            // c is case-ignorable
            اگلا_State |= (state & AFTER_CASED);
        } وإلا إذا (type != UCASE_NONE) {
            // c is cased
            اگلا_State |= AFTER_CASED;
        }
        uint32_t انکڑے = get_Letter_انکڑے(c);
        إذا (انکڑے > 0) {
            uint32_t upper = انکڑے & اتلا_نقاب;
            // Add a dialytika to this iota or ypsilon vowel
            // if we removed a tonos from the previous vowel,
            // and that previous vowel did not also have (or gain) a dialytika.
            // Adding one only to the final vowel in a longer sequence
            // (which does not occur in normal writing) would require lookahead.
            // Set the same flag as for preserving an existing dialytika.
            إذا ((انکڑے & HAS_VOWEL) != 0 &&
                (state & (AFTER_VOWEL_WITH_PRECOMPOSED_ACCENT | AFTER_VOWEL_WITH_COMBINING_ACCENT)) !=
                    0 &&
                (upper == 0x399 || upper == 0x3A5)) {
                انکڑے |= (state & AFTER_VOWEL_WITH_PRECOMPOSED_ACCENT) != 0 ? HAS_DIALYTIKA
                                                                           : HAS_COMBINING_DIALYTIKA;
            }
            int32_t عدد_Ypogegrammeni = 0;  // Map each one to a trailing, spacing, capital iota.
            إذا ((انکڑے & HAS_YPOGEGRAMMENI) != 0) {
                عدد_Ypogegrammeni = 1;
            }
            ثابت UBool hasPrecomposedAccent = (انکڑے & HAS_ACCENT) != 0;
            // Skip combining diacritics after this Greek letter.
            int32_t اگلا_اگلا_اشاریہ = اگلا_اشاریہ;
            طالما (اگلا_اشاریہ < src_لمائی) {
                UChar32 c2;
                U8_اگلا(src, اگلا_اگلا_اشاریہ, src_لمائی, c2);
                uint32_t diacritic_انکڑے = get_Diacritic_انکڑے(c2);
                إذا (diacritic_انکڑے != 0) {
                    انکڑے |= diacritic_انکڑے;
                    إذا ((diacritic_انکڑے & HAS_YPOGEGRAMMENI) != 0) {
                        ++عدد_Ypogegrammeni;
                    }
                    اگلا_اشاریہ = اگلا_اگلا_اشاریہ;
                } وإلا {
                    اكسر;  // not a Greek diacritic
                }
            }
            إذا ((انکڑے & HAS_VOWEL_AND_ACCENT_AND_DIALYTIKA) == HAS_VOWEL_AND_ACCENT) {
                اگلا_State |= hasPrecomposedAccent ? AFTER_VOWEL_WITH_PRECOMPOSED_ACCENT
                                                  : AFTER_VOWEL_WITH_COMBINING_ACCENT;
            }
            // Map according to Greek rules.
            UBool جوڑو_Tonos = خطأ;
            إذا (upper == 0x397 &&
                    (انکڑے & HAS_ACCENT) != 0 &&
                    عدد_Ypogegrammeni == 0 &&
                    (state & AFTER_CASED) == 0 &&
                    !isFollowedByCasedLetter(src, اگلا_اشاریہ, src_لمائی)) {
                // Keep disjunctive "or" with (only) a tonos.
                // We use the same "word boundary" conditions as for the Final_Sigma test.
                إذا (hasPrecomposedAccent) {
                    upper = 0x389;  // Preserve the precomposed form.
                } وإلا {
                    جوڑو_Tonos = سچ;
                }
            } وإلا إذا ((انکڑے & HAS_DIALYTIKA) != 0) {
                // Preserve a vowel with dialytika in precomposed form if it exists.
                إذا (upper == 0x399) {
                    upper = 0x3AA;
                    انکڑے &= ~HAS_EITHER_DIALYTIKA;
                } وإلا إذا (upper == 0x3A5) {
                    upper = 0x3AB;
                    انکڑے &= ~HAS_EITHER_DIALYTIKA;
                }
            }

            UBool change;
            إذا (edits == خالی_سوچک && (options & U_OMIT_UNCHANGED_متن) == 0) {
                change = سچ;  // common, simple usage
            } وإلا {
                // Find out first whether we are changing the text.
                U_ASSERT(0x370 <= upper && upper <= 0x3ff);  // 2-byte UTF-8, main Greek block
                change = (i + 2) > اگلا_اشاریہ ||
                        src[i] != getTwoByteLead(upper) || src[i + 1] != getTwoByteTrail(upper) ||
                        عدد_Ypogegrammeni > 0;
                int32_t i2 = i + 2;
                إذا ((انکڑے & HAS_EITHER_DIALYTIKA) != 0) {
                    change |= (i2 + 2) > اگلا_اشاریہ ||
                            src[i2] != static_cast<uint8_t>(u8"\u0308"[0]) ||
                            src[i2 + 1] != static_cast<uint8_t>(u8"\u0308"[1]);
                    i2 += 2;
                }
                إذا (جوڑو_Tonos) {
                    change |= (i2 + 2) > اگلا_اشاریہ ||
                            src[i2] != static_cast<uint8_t>(u8"\u0301"[0]) ||
                            src[i2 + 1] != static_cast<uint8_t>(u8"\u0301"[1]);
                    i2 += 2;
                }
                int32_t old_لمائی = اگلا_اشاریہ - i;
                int32_t new_لمائی = (i2 - i) + عدد_Ypogegrammeni * 2;  // 2 bytes per U+0399
                change |= old_لمائی != new_لمائی;
                إذا (change) {
                    إذا (edits != خالی_سوچک) {
                        edits->جوڑو_Replace(old_لمائی, new_لمائی);
                    }
                } وإلا {
                    إذا (edits != خالی_سوچک) {
                        edits->جوڑو_Unchanged(old_لمائی);
                    }
                    // Write unchanged text?
                    change = (options & U_OMIT_UNCHANGED_متن) == 0;
                }
            }

            إذا (change) {
                ByteSinkUtil::append_Two_بائٹاں(upper, sink);
                إذا ((انکڑے & HAS_EITHER_DIALYTIKA) != 0) {
                    sink.AppendU8(u8"\u0308", 2);  // restore or add a dialytika
                }
                إذا (جوڑو_Tonos) {
                    sink.AppendU8(u8"\u0301", 2);
                }
                طالما (عدد_Ypogegrammeni > 0) {
                    sink.AppendU8(u8"\u0399", 2);
                    --عدد_Ypogegrammeni;
                }
            }
        } وإلا إذا(c>=0) {
            ثابت char16_t *s;
            c=ucase_منزل_بھریا_اتلا(c, خالی_سوچک, خالی_سوچک, &s, UCASE_LOC_GREEK);
            إذا (!append_نتیجہ(اگلا_اشاریہ - i, c, s, sink, options, edits, error_کوڈ)) {
                أرجع;
            }
        } وإلا {
            // Malformed UTF-8.
            إذا (!ByteSinkUtil::appendUnchanged(src+i, اگلا_اشاریہ-i,
                                               sink, options, edits, error_کوڈ)) {
                أرجع;
            }
        }
        i = اگلا_اشاریہ;
        state = اگلا_State;
    }
}

}  // namespace GreekUpper
U_NAMESPACE_سرا

ساكن فراغ U_CALLCONV
ucasemap_اندرونی_UTF8_منزل_ہیٹھلا(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_بن_ورتیا
                             ثابت uint8_t *src, int32_t src_لمائی,
                             icu::ByteSink &sink, icu::Edits *edits,
                             UError_کوڈ &error_کوڈ) {
    UCaseContext csc=UCASECONTEXT_INITIALIZER;
    csc.p=(فراغ *)src;
    csc.حد_بندی=src_لمائی;
    منزل_ہیٹھلا(
        caseLocale, options,
        src, &csc, 0, src_لمائی,
        sink, edits, error_کوڈ);
}

ساكن فراغ U_CALLCONV
ucasemap_اندرونی_UTF8_منزل_اتلا(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_بن_ورتیا
                             ثابت uint8_t *src, int32_t src_لمائی,
                             icu::ByteSink &sink, icu::Edits *edits,
                             UError_کوڈ &error_کوڈ) {
    إذا (caseLocale == UCASE_LOC_GREEK) {
        Greek_اتلا::منزل_اتلا(options, src, src_لمائی, sink, edits, error_کوڈ);
    } وإلا {
        UCaseContext csc=UCASECONTEXT_INITIALIZER;
        csc.p=(فراغ *)src;
        csc.حد_بندی=src_لمائی;
        منزل_اتلا(
            caseLocale, options,
            src, &csc, src_لمائی,
            sink, edits, error_کوڈ);
    }
}

ساكن فراغ U_CALLCONV
ucasemap_اندرونی_UTF8_Fold(int32_t /* caseLocale */, uint32_t options, UCASEMAP_BREAK_ITERATOR_بن_ورتیا
                          ثابت uint8_t *src, int32_t src_لمائی,
                          icu::ByteSink &sink, icu::Edits *edits,
                          UError_کوڈ &error_کوڈ) {
    منزل_ہیٹھلا(
        -1, options,
        src, خالی_سوچک, 0, src_لمائی,
        sink, edits, error_کوڈ);
}

فراغ
ucasemap_mapUTF8(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_PARAM
                 ثابت حرف *src, int32_t src_لمائی,
                 UTF8CaseMapper *stringCaseMapper,
                 icu::ByteSink &sink, icu::Edits *edits,
                 UError_کوڈ &error_کوڈ) {
    /* check argument values */
    إذا (U_FAILURE(error_کوڈ)) {
        أرجع;
    }
    إذا ((src == خالی_سوچک && src_لمائی != 0) || src_لمائی < -1) {
        error_کوڈ = U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }

    // Get the string length.
    إذا (src_لمائی == -1) {
        src_لمائی = static_cast<int32_t>(uprv_strlen(src));
    }

    إذا (edits != خالی_سوچک && (options & U_EDITS_NO_RESET) == 0) {
        edits->reset();
    }
    stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                     reinterpret_cast<ثابت uint8_t*>(src), src_لمائی, sink, edits, error_کوڈ);
    sink.Flush();
    إذا (U_SUCCESS(error_کوڈ)) {
        إذا (edits != خالی_سوچک) {
            edits->copy_Error_منزل(error_کوڈ);
        }
    }
}

int32_t
ucasemap_mapUTF8(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_PARAM
                 حرف *dest, int32_t dest_سمرتھا,
                 ثابت حرف *src, int32_t src_لمائی,
                 UTF8CaseMapper *stringCaseMapper,
                 icu::Edits *edits,
                 UError_کوڈ &error_کوڈ) {
    /* check argument values */
    إذا(U_FAILURE(error_کوڈ)) {
        أرجع 0;
    }
    إذا( dest_سمرتھا<0 ||
        (dest==خالی_سوچک && dest_سمرتھا>0) ||
        (src==خالی_سوچک && src_لمائی!=0) || src_لمائی<-1
    ) {
        error_کوڈ=U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }

    /* get the string length */
    إذا(src_لمائی==-1) {
        src_لمائی = static_cast<int32_t>(uprv_strlen(src));
    }

    /* check for overlapping source and destination */
    إذا( dest!=خالی_سوچک &&
        ((src>=dest && src<(dest+dest_سمرتھا)) ||
         (dest>=src && dest<(src+src_لمائی)))
    ) {
        error_کوڈ=U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }

    إذا (edits != خالی_سوچک && (options & U_EDITS_NO_RESET) == 0) {
        edits->reset();
    }
    int32_t reslen = ByteSinkUtil::راہیں_Byte_Sink_منزل_Terminated_Chars(
        dest, dest_سمرتھا,
        [&](ByteSink& sink, UError_کوڈ& حالت) {
            stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                             reinterpret_cast<ثابت uint8_t*>(src), src_لمائی, sink, edits, حالت);
        },
        error_کوڈ);
    إذا (U_SUCCESS(error_کوڈ) && edits != خالی_سوچک) {
        edits->copy_Error_منزل(error_کوڈ);
    }
    أرجع reslen;
}

/* public API functions */

U_CAPI int32_t U_EXPORT2
ucasemap_utf8_منزل_ہیٹھلا(ثابت UCaseMap *csm,
                     حرف *dest, int32_t dest_سمرتھا,
                     ثابت حرف *src, int32_t src_لمائی,
                     UError_کوڈ *p_Error_کوڈ) {
    خودکار *impl = reinterpret_cast<ثابت UCaseMapImpl *>(csm);
    أرجع ucasemap_mapUTF8(
        impl->caseLocale, impl->options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_سمرتھا,
        src, src_لمائی,
        ucasemap_اندرونی_UTF8_منزل_ہیٹھلا, خالی_سوچک, *p_Error_کوڈ);
}

U_CAPI int32_t U_EXPORT2
ucasemap_utf8_منزل_اتلا(ثابت UCaseMap *csm,
                     حرف *dest, int32_t dest_سمرتھا,
                     ثابت حرف *src, int32_t src_لمائی,
                     UError_کوڈ *p_Error_کوڈ) {
    خودکار *impl = reinterpret_cast<ثابت UCaseMapImpl *>(csm);
    أرجع ucasemap_mapUTF8(
        impl->caseLocale, impl->options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_سمرتھا,
        src, src_لمائی,
        ucasemap_اندرونی_UTF8_منزل_اتلا, خالی_سوچک, *p_Error_کوڈ);
}

U_CAPI int32_t U_EXPORT2
ucasemap_utf8FoldCase(ثابت UCaseMap *csm,
                      حرف *dest, int32_t dest_سمرتھا,
                      ثابت حرف *src, int32_t src_لمائی,
                      UError_کوڈ *p_Error_کوڈ) {
    خودکار *impl = reinterpret_cast<ثابت UCaseMapImpl *>(csm);
    أرجع ucasemap_mapUTF8(
        UCASE_LOC_جڑ, impl->options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_سمرتھا,
        src, src_لمائی,
        ucasemap_اندرونی_UTF8_Fold, خالی_سوچک, *p_Error_کوڈ);
}

U_NAMESPACE_BEGIN

فراغ CaseMap::utf8_منزل_ہیٹھلا(
        ثابت حرف *locale, uint32_t options,
        StringPiece src, ByteSink &sink, Edits *edits,
        UError_کوڈ &error_کوڈ) {
    ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        src.انکڑے(), src.length(),
        ucasemap_اندرونی_UTF8_منزل_ہیٹھلا, sink, edits, error_کوڈ);
}

فراغ CaseMap::utf8_منزل_اتلا(
        ثابت حرف *locale, uint32_t options,
        StringPiece src, ByteSink &sink, Edits *edits,
        UError_کوڈ &error_کوڈ) {
    ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        src.انکڑے(), src.length(),
        ucasemap_اندرونی_UTF8_منزل_اتلا, sink, edits, error_کوڈ);
}

فراغ CaseMap::utf8Fold(
        uint32_t options,
        StringPiece src, ByteSink &sink, Edits *edits,
        UError_کوڈ &error_کوڈ) {
    ucasemap_mapUTF8(
        UCASE_LOC_جڑ, options, UCASEMAP_BREAK_ITERATOR_NULL
        src.انکڑے(), src.length(),
        ucasemap_اندرونی_UTF8_Fold, sink, edits, error_کوڈ);
}

int32_t CaseMap::utf8_منزل_ہیٹھلا(
        ثابت حرف *locale, uint32_t options,
        ثابت حرف *src, int32_t src_لمائی,
        حرف *dest, int32_t dest_سمرتھا, Edits *edits,
        UError_کوڈ &error_کوڈ) {
    أرجع ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_سمرتھا,
        src, src_لمائی,
        ucasemap_اندرونی_UTF8_منزل_ہیٹھلا, edits, error_کوڈ);
}

int32_t CaseMap::utf8_منزل_اتلا(
        ثابت حرف *locale, uint32_t options,
        ثابت حرف *src, int32_t src_لمائی,
        حرف *dest, int32_t dest_سمرتھا, Edits *edits,
        UError_کوڈ &error_کوڈ) {
    أرجع ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_سمرتھا,
        src, src_لمائی,
        ucasemap_اندرونی_UTF8_منزل_اتلا, edits, error_کوڈ);
}

int32_t CaseMap::utf8Fold(
        uint32_t options,
        ثابت حرف *src, int32_t src_لمائی,
        حرف *dest, int32_t dest_سمرتھا, Edits *edits,
        UError_کوڈ &error_کوڈ) {
    أرجع ucasemap_mapUTF8(
        UCASE_LOC_جڑ, options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_سمرتھا,
        src, src_لمائی,
        ucasemap_اندرونی_UTF8_Fold, edits, error_کوڈ);
}

U_NAMESPACE_سرا
