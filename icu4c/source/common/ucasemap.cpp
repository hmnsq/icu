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
#د_که_پای
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

UCaseMapImpl::UCaseMapImpl(ثابت حرف *localeID, uint32_t opts, UError_کوډ *p_Error_کوډ) :
#إذا !UCONFIG_NO_BREAK_ITERATION
        iter(تش_اشاره),
#د_که_پای
        caseLocale(UCASE_LOC_UNKNOWN), options(opts) {
    ucasemap_سېټ_Locale(reinterpret_cast<UCaseMap *>(هذا), localeID, p_Error_کوډ);
}

UCaseMapImpl::~UCaseMapImpl() {
#إذا !UCONFIG_NO_BREAK_ITERATION
    ړنګول iter;
#د_که_پای
}

U_NAMESPACE_پای_ټکی

U_CAPI UCaseMap * U_EXPORT2
ucasemap_open(ثابت حرف *locale, uint32_t options, UError_کوډ *p_Error_کوډ) {
    إذا(U_FAILURE(*p_Error_کوډ)) {
        أرجع تش_اشاره;
    }
    UCaseMapImpl *csm = جديد UCaseMapImpl(locale, options, p_Error_کوډ);
    إذا(csm==تش_اشاره) {
        *p_Error_کوډ = U_حافظه_ویش_ERROR;
        أرجع تش_اشاره;
    } وإلا إذا (U_FAILURE(*p_Error_کوډ)) {
        ړنګول csm;
        أرجع تش_اشاره;
    }
    أرجع reinterpret_cast<UCaseMap *>(csm);
}

U_CAPI فراغ U_EXPORT2
ucasemap_close(UCaseMap *csm) {
    ړنګول reinterpret_cast<UCaseMapImpl *>(csm);
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
ucasemap_سېټ_Locale(UCaseMap *csm, ثابت حرف *locale, UError_کوډ *p_Error_کوډ) {
    إذا(U_FAILURE(*p_Error_کوډ)) {
        أرجع;
    }
    خپلکاری *impl = reinterpret_cast<UCaseMapImpl *>(csm);
    إذا (locale != تش_اشاره && *locale == 0) {
        impl->locale[0] = 0;
        impl->caseLocale = UCASE_LOC_ریښه;
        أرجع;
    }

    UError_کوډ بفر_حالت = U_ZERO_ERROR;
    int32_t length=uloc_getName(locale, impl->locale, (int32_t)حجم(impl->locale), &بفر_حالت);
    إذا(بفر_حالت==U_بفر_ډکېدنه_ERROR || (U_SUCCESS(بفر_حالت) && length==حجم(impl->locale))) {
        بفر_حالت = U_ZERO_ERROR;
        /* we only really need the language code for case mappings */
        length=uloc_getLanguage(locale, impl->locale, (int32_t)حجم(impl->locale), &بفر_حالت);
    }
    إذا(U_FAILURE(بفر_حالت)) {
        *p_Error_کوډ=بفر_حالت;
    } وإلا إذا(length==حجم(impl->locale)) {
        *p_Error_کوډ=U_بفر_ډکېدنه_ERROR;
    }
    إذا(U_SUCCESS(*p_Error_کوډ)) {     
        impl->caseLocale = ucase_getCaseLocale(impl->locale);
    } وإلا {
        impl->locale[0]=0;
        impl->caseLocale = UCASE_LOC_ریښه;
    }
}

U_CAPI فراغ U_EXPORT2
ucasemap_سېټ_Options(UCaseMap *csm, uint32_t options, UError_کوډ *p_Error_کوډ) {
    إذا(U_FAILURE(*p_Error_کوډ)) {
        أرجع;
    }
    reinterpret_cast<UCaseMapImpl *>(csm)->options=options;
}

/* UTF-8 string case mappings ----------------------------------------------- */

/* TODO(markus): Move to a new, separate utf8case.cpp file. */

نطاق {

/* append a full case mapping result, see UCASE_MAX_STRING_LENGTH */
مضمن UBool
append_پایله(int32_t cp_اوږدتیا, int32_t پایله, ثابت char16_t *s,
             ByteSink &sink, uint32_t options, icu::Edits *edits, UError_کوډ &error_کوډ) {
    U_ASSERT(U_SUCCESS(error_کوډ));

    /* decode the result */
    إذا(پایله<0) {
        /* (not) original code point */
        إذا(edits!=تش_اشاره) {
            edits->جمع_کړه_Unchanged(cp_اوږدتیا);
        }
        إذا((options & U_OMIT_UNCHANGED_متن) == 0) {
            ByteSinkUtil::append_کوډ_ټکی(cp_اوږدتیا, ~پایله, sink);
        }
    } وإلا {
        إذا(پایله<=UCASE_MAX_STRING_اوږدتیا) {
            // string: "result" is the UTF-16 length
            أرجع ByteSinkUtil::appendChange(cp_اوږدتیا, s, پایله, sink, edits, error_کوډ);
        } وإلا {
            ByteSinkUtil::append_کوډ_ټکی(cp_اوږدتیا, پایله, sink, edits);
        }
    }
    أرجع ریښتیا;
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
        csc->شاخص=csc->cp_پیل;
        csc->dir=dir;
    } وإلا إذا(dir>0) {
        /* reset for forward iteration */
        csc->شاخص=csc->cp_حد;
        csc->dir=dir;
    } وإلا {
        /* continue current iteration direction */
        dir=csc->dir;
    }

    إذا(dir<0) {
        إذا(csc->پیل<csc->شاخص) {
            U8_PREV((ثابت uint8_t *)csc->p, csc->پیل, csc->شاخص, c);
            أرجع c;
        }
    } وإلا {
        إذا(csc->شاخص<csc->حد) {
            U8_بل((ثابت uint8_t *)csc->p, csc->شاخص, csc->حد, c);
            أرجع c;
        }
    }
    أرجع U_SENTINEL;
}

/**
 * caseLocale >= 0: Lowercases [srcStart..srcLimit[ but takes context [0..srcLength[ into account.
 * caseLocale < 0: Case-folds [srcStart..srcLimit[.
 */
فراغ منزل_ښکتنی(int32_t caseLocale, uint32_t options,
             ثابت uint8_t *src, UCaseContext *csc, int32_t src_پیل, int32_t src_حد,
             icu::ByteSink &sink, icu::Edits *edits, UError_کوډ &error_کوډ) {
    ثابت int8_t *latin_منزل_ښکتنی;
    إذا (caseLocale == UCASE_LOC_ریښه ||
            (caseLocale >= 0 ?
                !(caseLocale == UCASE_LOC_TURKISH || caseLocale == UCASE_LOC_LITHUANIAN) :
                (options & FOLD_CASE_OPTIONS_ماسک) == U_FOLD_CASE_DEFAULT)) {
        latin_منزل_ښکتنی = LatinCase::منزل_ښکتنی_نورمال;
    } وإلا {
        latin_منزل_ښکتنی = LatinCase::منزل_ښکتنی_TR_LT;
    }
    ثابت UTrie2 *trie = ucase_getTrie();
    int32_t prev = src_پیل;
    int32_t src_شاخص = src_پیل;
    لكل (;;) {
        // fast path for simple cases
        int32_t cp_پیل;
        UChar32 c;
        لكل (;;) {
            إذا (U_FAILURE(error_کوډ) || src_شاخص >= src_حد) {
                c = U_SENTINEL;
                اكسر;
            }
            uint8_t lead = src[src_شاخص++];
            إذا (lead <= 0x7f) {
                int8_t d = latin_منزل_ښکتنی[lead];
                إذا (d == LatinCase::EXC) {
                    cp_پیل = src_شاخص - 1;
                    c = lead;
                    اكسر;
                }
                إذا (d == 0) { استمر; }
                ByteSinkUtil::appendUnchanged(src + prev, src_شاخص - 1 - prev,
                                              sink, options, edits, error_کوډ);
                حرف ascii = static_cast<حرف>(lead + d);
                sink.Append(&ascii, 1);
                إذا (edits != تش_اشاره) {
                    edits->جمع_کړه_Replace(1, 1);
                }
                prev = src_شاخص;
                استمر;
            } وإلا إذا (lead < 0xe3) {
                uint8_t t;
                إذا (0xc2 <= lead && lead <= 0xc5 && src_شاخص < src_حد &&
                        (t = src[src_شاخص] - 0x80) <= 0x3f) {
                    // U+0080..U+017F
                    ++src_شاخص;
                    c = ((lead - 0xc0) << 6) | t;
                    int8_t d = latin_منزل_ښکتنی[c];
                    إذا (d == LatinCase::EXC) {
                        cp_پیل = src_شاخص - 2;
                        اكسر;
                    }
                    إذا (d == 0) { استمر; }
                    ByteSinkUtil::appendUnchanged(src + prev, src_شاخص - 2 - prev,
                                                  sink, options, edits, error_کوډ);
                    ByteSinkUtil::append_Two_بایټونه(c + d, sink);
                    إذا (edits != تش_اشاره) {
                        edits->جمع_کړه_Replace(2, 2);
                    }
                    prev = src_شاخص;
                    استمر;
                }
            } وإلا إذا ((lead <= 0xe9 || lead == 0xeb || lead == 0xec) &&
                    (src_شاخص + 2) <= src_حد &&
                    U8_IS_TRAIL(src[src_شاخص]) && U8_IS_TRAIL(src[src_شاخص + 1])) {
                // most of CJK: no case mappings
                src_شاخص += 2;
                استمر;
            }
            cp_پیل = --src_شاخص;
            U8_بل(src, src_شاخص, src_حد, c);
            إذا (c < 0) {
                // ill-formed UTF-8
                استمر;
            }
            uint16_t props = UTRIE2_GET16(trie, c);
            إذا (UCASE_HAS_EXCEPTION(props)) { اكسر; }
            int32_t delta;
            إذا (!UCASE_IS_پاسنی_OR_TITLE(props) || (delta = UCASE_GET_DELTA(props)) == 0) {
                استمر;
            }
            ByteSinkUtil::appendUnchanged(src + prev, cp_پیل - prev,
                                          sink, options, edits, error_کوډ);
            ByteSinkUtil::append_کوډ_ټکی(src_شاخص - cp_پیل, c + delta, sink, edits);
            prev = src_شاخص;
        }
        إذا (c < 0) {
            اكسر;
        }
        // slow path
        ثابت char16_t *s;
        إذا (caseLocale >= 0) {
            csc->cp_پیل = cp_پیل;
            csc->cp_حد = src_شاخص;
            c = ucase_منزل_ډک_ښکتنی(c, utf8_caseContextIterator, csc, &s, caseLocale);
        } وإلا {
            c = ucase_منزل_ډک_Folding(c, &s, options);
        }
        إذا (c >= 0) {
            ByteSinkUtil::appendUnchanged(src + prev, cp_پیل - prev,
                                          sink, options, edits, error_کوډ);
            append_پایله(src_شاخص - cp_پیل, c, s, sink, options, edits, error_کوډ);
            prev = src_شاخص;
        }
    }
    ByteSinkUtil::appendUnchanged(src + prev, src_شاخص - prev,
                                  sink, options, edits, error_کوډ);
}

فراغ منزل_پاسنی(int32_t caseLocale, uint32_t options,
             ثابت uint8_t *src, UCaseContext *csc, int32_t src_اوږدتیا,
             icu::ByteSink &sink, icu::Edits *edits, UError_کوډ &error_کوډ) {
    ثابت int8_t *latin_منزل_پاسنی;
    إذا (caseLocale == UCASE_LOC_TURKISH) {
        latin_منزل_پاسنی = LatinCase::منزل_پاسنی_TR;
    } وإلا {
        latin_منزل_پاسنی = LatinCase::منزل_پاسنی_نورمال;
    }
    ثابت UTrie2 *trie = ucase_getTrie();
    int32_t prev = 0;
    int32_t src_شاخص = 0;
    لكل (;;) {
        // fast path for simple cases
        int32_t cp_پیل;
        UChar32 c;
        لكل (;;) {
            إذا (U_FAILURE(error_کوډ) || src_شاخص >= src_اوږدتیا) {
                c = U_SENTINEL;
                اكسر;
            }
            uint8_t lead = src[src_شاخص++];
            إذا (lead <= 0x7f) {
                int8_t d = latin_منزل_پاسنی[lead];
                إذا (d == LatinCase::EXC) {
                    cp_پیل = src_شاخص - 1;
                    c = lead;
                    اكسر;
                }
                إذا (d == 0) { استمر; }
                ByteSinkUtil::appendUnchanged(src + prev, src_شاخص - 1 - prev,
                                              sink, options, edits, error_کوډ);
                حرف ascii = static_cast<حرف>(lead + d);
                sink.Append(&ascii, 1);
                إذا (edits != تش_اشاره) {
                    edits->جمع_کړه_Replace(1, 1);
                }
                prev = src_شاخص;
                استمر;
            } وإلا إذا (lead < 0xe3) {
                uint8_t t;
                إذا (0xc2 <= lead && lead <= 0xc5 && src_شاخص < src_اوږدتیا &&
                        (t = src[src_شاخص] - 0x80) <= 0x3f) {
                    // U+0080..U+017F
                    ++src_شاخص;
                    c = ((lead - 0xc0) << 6) | t;
                    int8_t d = latin_منزل_پاسنی[c];
                    إذا (d == LatinCase::EXC) {
                        cp_پیل = src_شاخص - 2;
                        اكسر;
                    }
                    إذا (d == 0) { استمر; }
                    ByteSinkUtil::appendUnchanged(src + prev, src_شاخص - 2 - prev,
                                                  sink, options, edits, error_کوډ);
                    ByteSinkUtil::append_Two_بایټونه(c + d, sink);
                    إذا (edits != تش_اشاره) {
                        edits->جمع_کړه_Replace(2, 2);
                    }
                    prev = src_شاخص;
                    استمر;
                }
            } وإلا إذا ((lead <= 0xe9 || lead == 0xeb || lead == 0xec) &&
                    (src_شاخص + 2) <= src_اوږدتیا &&
                    U8_IS_TRAIL(src[src_شاخص]) && U8_IS_TRAIL(src[src_شاخص + 1])) {
                // most of CJK: no case mappings
                src_شاخص += 2;
                استمر;
            }
            cp_پیل = --src_شاخص;
            U8_بل(src, src_شاخص, src_اوږدتیا, c);
            إذا (c < 0) {
                // ill-formed UTF-8
                استمر;
            }
            uint16_t props = UTRIE2_GET16(trie, c);
            إذا (UCASE_HAS_EXCEPTION(props)) { اكسر; }
            int32_t delta;
            إذا (UCASE_GET_TYPE(props) != UCASE_ښکتنی || (delta = UCASE_GET_DELTA(props)) == 0) {
                استمر;
            }
            ByteSinkUtil::appendUnchanged(src + prev, cp_پیل - prev,
                                          sink, options, edits, error_کوډ);
            ByteSinkUtil::append_کوډ_ټکی(src_شاخص - cp_پیل, c + delta, sink, edits);
            prev = src_شاخص;
        }
        إذا (c < 0) {
            اكسر;
        }
        // slow path
        csc->cp_پیل = cp_پیل;
        csc->cp_حد = src_شاخص;
        ثابت char16_t *s;
        c = ucase_منزل_ډک_پاسنی(c, utf8_caseContextIterator, csc, &s, caseLocale);
        إذا (c >= 0) {
            ByteSinkUtil::appendUnchanged(src + prev, cp_پیل - prev,
                                          sink, options, edits, error_کوډ);
            append_پایله(src_شاخص - cp_پیل, c, s, sink, options, edits, error_کوډ);
            prev = src_شاخص;
        }
    }
    ByteSinkUtil::appendUnchanged(src + prev, src_شاخص - prev,
                                  sink, options, edits, error_کوډ);
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
int32_t maybeTitleDutchIJ(ثابت uint8_t *src, UChar32 c, int32_t پیل, int32_t segment_حد,
                          ByteSink &sink, uint32_t options, icu::Edits *edits, UError_کوډ &error_کوډ) {
    U_ASSERT(پیل < segment_حد);

    int32_t شاخص = پیل;
    منطقي withAcute = خطأ;

    // If the conditions are met, then the following variables tell us what to output.
    int32_t unchanged1 = 0;  // code units before the j, or the whole sequence (0..3)
    منطقي doTitleJ = خطأ;  // true if the j needs to be titlecased
    int32_t unchanged2 = 0;  // after the j (0 or 1)

    // next character after the first letter
    UChar32 c2;
    c2 = src[شاخص++];

    // Is the first letter an i/I with accent?
    إذا (c == u'I') {
        إذا (c2 == ACUTE_BYTE0 && شاخص < segment_حد && src[شاخص++] == ACUTE_BYTE1) {
            withAcute = ریښتیا;
            unchanged1 = 2;  // ACUTE is 2 code units in UTF-8
            إذا (شاخص == segment_حد) { أرجع پیل; }
            c2 = src[شاخص++];
        }
    } وإلا {  // Í
        withAcute = ریښتیا;
    }

    // Is the next character a j/J?
    إذا (c2 == u'j') {
        doTitleJ = ریښتیا;
    } وإلا إذا (c2 == u'J') {
        ++unchanged1;
    } وإلا {
        أرجع پیل;
    }

    // A plain i/I must be followed by a plain j/J.
    // An i/I with acute must be followed by a j/J with acute.
    إذا (withAcute) {
        إذا ((شاخص + 1) >= segment_حد || src[شاخص++] != ACUTE_BYTE0 || src[شاخص++] != ACUTE_BYTE1) {
            أرجع پیل;
        }
        إذا (doTitleJ) {
            unchanged2 = 2;  // ACUTE is 2 code units in UTF-8
        } وإلا {
            unchanged1 = unchanged1 + 2;    // ACUTE is 2 code units in UTF-8
        }
    }

    // There must not be another combining mark.
    إذا (شاخص < segment_حد) {
        int32_t cp;
        int32_t i = شاخص;
        U8_بل(src, i, segment_حد, cp);
        uint32_t type_ماسک = U_GET_GC_ماسک(cp);
        إذا ((type_ماسک & U_GC_M_ماسک) != 0) {
            أرجع پیل;
        }
    }

    // Output the rest of the Dutch IJ.
    ByteSinkUtil::appendUnchanged(src + پیل, unchanged1, sink, options, edits, error_کوډ);
    پیل += unchanged1;
    إذا (doTitleJ) {
        ByteSinkUtil::append_کوډ_ټکی(1, u'J', sink, edits);
        ++پیل;
    }
    ByteSinkUtil::appendUnchanged(src + پیل, unchanged2, sink, options, edits, error_کوډ);

    U_ASSERT(پیل + unchanged2 == شاخص);
    أرجع شاخص;
}

}  // namespace

U_CFUNC فراغ U_CALLCONV
ucasemap_دننی_UTF8_منزل_Title(
        int32_t caseLocale, uint32_t options, BreakIterator *iter,
        ثابت uint8_t *src, int32_t src_اوږدتیا,
        ByteSink &sink, icu::Edits *edits,
        UError_کوډ &error_کوډ) {
    إذا (!ustrcase_checkTitleAdjustmentOptions(options, error_کوډ)) {
        أرجع;
    }

    /* set up local variables */
    UCaseContext csc=UCASECONTEXT_INITIALIZER;
    csc.p=(فراغ *)src;
    csc.حد=src_اوږدتیا;
    int32_t prev=0;
    UBool is_لومړی_شاخص=ریښتیا;

    /* titlecasing loop */
    طالما(prev<src_اوږدتیا) {
        /* find next index where to titlecase */
        int32_t شاخص;
        إذا(is_لومړی_شاخص) {
            is_لومړی_شاخص=خطأ;
            شاخص=iter->first();
        } وإلا {
            شاخص=iter->next();
        }
        إذا(شاخص==UBRK_بشپړ || شاخص>src_اوږدتیا) {
            شاخص=src_اوږدتیا;
        }

        /*
         * Segment [prev..index[ into 3 parts:
         * a) skipped characters (copy as-is) [prev..titleStart[
         * b) first letter (titlecase)              [titleStart..titleLimit[
         * c) subsequent characters (lowercase)                 [titleLimit..index[
         */
        إذا(prev<شاخص) {
            /* find and copy skipped characters [prev..titleStart[ */
            int32_t title_پیل=prev;
            int32_t title_حد=prev;
            UChar32 c;
            U8_بل(src, title_حد, شاخص, c);
            إذا ((options&U_TITLECASE_NO_BREAK_ADJUSTMENT)==0) {
                // Adjust the titlecasing index to the next cased character,
                // or to the next letter/number/symbol/private use.
                // Stop with titleStart<titleLimit<=index
                // if there is a character to be titlecased,
                // or else stop with titleStart==titleLimit==index.
                UBool منزل_Cased = (options&U_TITLECASE_ADJUST_منزل_CASED) != 0;
                طالما (منزل_Cased ? UCASE_NONE==ucase_getType(c) : !ustrcase_isLNS(c)) {
                    title_پیل=title_حد;
                    إذا(title_حد==شاخص) {
                        اكسر;
                    }
                    U8_بل(src, title_حد, شاخص, c);
                }
                إذا (prev < title_پیل) {
                    إذا (!ByteSinkUtil::appendUnchanged(src+prev, title_پیل-prev,
                                                       sink, options, edits, error_کوډ)) {
                        أرجع;
                    }
                }
            }

            إذا(title_پیل<title_حد) {
                /* titlecase c which is from [titleStart..titleLimit[ */
                إذا(c>=0) {
                    csc.cp_پیل=title_پیل;
                    csc.cp_حد=title_حد;
                    ثابت char16_t *s;
                    c=ucase_منزل_ډک_Title(c, utf8_caseContextIterator, &csc, &s, caseLocale);
                    إذا (!append_پایله(title_حد-title_پیل, c, s, sink, options, edits, error_کوډ)) {
                        أرجع;
                    }
                } وإلا {
                    // Malformed UTF-8.
                    إذا (!ByteSinkUtil::appendUnchanged(src+title_پیل, title_حد-title_پیل,
                                                       sink, options, edits, error_کوډ)) {
                        أرجع;
                    }
                }

                /* Special case Dutch IJ titlecasing */
                إذا (title_حد < شاخص &&
                    caseLocale == UCASE_LOC_DUTCH) {
                    إذا (c < 0) {
                        c = ~c;
                    }

                    إذا (c == u'I' || c == u'Í') {
                        title_حد = maybeTitleDutchIJ(src, c, title_حد, شاخص, sink, options, edits, error_کوډ);
                    }
                }

                /* lowercase [titleLimit..index[ */
                إذا(title_حد<شاخص) {
                    إذا((options&U_TITLECASE_NO_LOWERCASE)==0) {
                        /* Normal operation: Lowercase the rest of the word. */
                        منزل_ښکتنی(caseLocale, options,
                                src, &csc, title_حد, شاخص,
                                sink, edits, error_کوډ);
                        إذا(U_FAILURE(error_کوډ)) {
                            أرجع;
                        }
                    } وإلا {
                        /* Optionally just copy the rest of the word unchanged. */
                        إذا (!ByteSinkUtil::appendUnchanged(src+title_حد, شاخص-title_حد,
                                                           sink, options, edits, error_کوډ)) {
                            أرجع;
                        }
                    }
                }
            }
        }

        prev=شاخص;
    }
}

#د_که_پای

U_NAMESPACE_BEGIN
نطاق Greek_پاسنی {

UBool isFollowedByCasedLetter(ثابت uint8_t *s, int32_t i, int32_t length) {
    طالما (i < length) {
        UChar32 c;
        U8_بل(s, i, length, c);
        int32_t type = ucase_getTypeOrIgnorable(c);
        إذا ((type & UCASE_IGNORABLE) != 0) {
            // Case-ignorable, continue with the loop.
        } وإلا إذا (type != UCASE_NONE) {
            أرجع ریښتیا;  // Followed by cased letter.
        } وإلا {
            أرجع خطأ;  // Uncased and not case-ignorable.
        }
    }
    أرجع خطأ;  // Not followed by cased letter.
}

// Keep this consistent with the UTF-16 version in ustrcase.cpp and the Java version in CaseMap.java.
فراغ منزل_پاسنی(uint32_t options,
             ثابت uint8_t *src, int32_t src_اوږدتیا,
             ByteSink &sink, Edits *edits,
             UError_کوډ &error_کوډ) {
    uint32_t state = 0;
    لكل (int32_t i = 0; i < src_اوږدتیا;) {
        int32_t بل_شاخص = i;
        UChar32 c;
        U8_بل(src, بل_شاخص, src_اوږدتیا, c);
        uint32_t بل_State = 0;
        int32_t type = ucase_getTypeOrIgnorable(c);
        إذا ((type & UCASE_IGNORABLE) != 0) {
            // c is case-ignorable
            بل_State |= (state & AFTER_CASED);
        } وإلا إذا (type != UCASE_NONE) {
            // c is cased
            بل_State |= AFTER_CASED;
        }
        uint32_t معلومات = get_Letter_معلومات(c);
        إذا (معلومات > 0) {
            uint32_t upper = معلومات & پاسنی_ماسک;
            // Add a dialytika to this iota or ypsilon vowel
            // if we removed a tonos from the previous vowel,
            // and that previous vowel did not also have (or gain) a dialytika.
            // Adding one only to the final vowel in a longer sequence
            // (which does not occur in normal writing) would require lookahead.
            // Set the same flag as for preserving an existing dialytika.
            إذا ((معلومات & HAS_VOWEL) != 0 &&
                (state & (AFTER_VOWEL_WITH_PRECOMPOSED_ACCENT | AFTER_VOWEL_WITH_COMBINING_ACCENT)) !=
                    0 &&
                (upper == 0x399 || upper == 0x3A5)) {
                معلومات |= (state & AFTER_VOWEL_WITH_PRECOMPOSED_ACCENT) != 0 ? HAS_DIALYTIKA
                                                                           : HAS_COMBINING_DIALYTIKA;
            }
            int32_t عدد_Ypogegrammeni = 0;  // Map each one to a trailing, spacing, capital iota.
            إذا ((معلومات & HAS_YPOGEGRAMMENI) != 0) {
                عدد_Ypogegrammeni = 1;
            }
            ثابت UBool hasPrecomposedAccent = (معلومات & HAS_ACCENT) != 0;
            // Skip combining diacritics after this Greek letter.
            int32_t بل_بل_شاخص = بل_شاخص;
            طالما (بل_شاخص < src_اوږدتیا) {
                UChar32 c2;
                U8_بل(src, بل_بل_شاخص, src_اوږدتیا, c2);
                uint32_t diacritic_معلومات = get_Diacritic_معلومات(c2);
                إذا (diacritic_معلومات != 0) {
                    معلومات |= diacritic_معلومات;
                    إذا ((diacritic_معلومات & HAS_YPOGEGRAMMENI) != 0) {
                        ++عدد_Ypogegrammeni;
                    }
                    بل_شاخص = بل_بل_شاخص;
                } وإلا {
                    اكسر;  // not a Greek diacritic
                }
            }
            إذا ((معلومات & HAS_VOWEL_AND_ACCENT_AND_DIALYTIKA) == HAS_VOWEL_AND_ACCENT) {
                بل_State |= hasPrecomposedAccent ? AFTER_VOWEL_WITH_PRECOMPOSED_ACCENT
                                                  : AFTER_VOWEL_WITH_COMBINING_ACCENT;
            }
            // Map according to Greek rules.
            UBool جمع_کړه_Tonos = خطأ;
            إذا (upper == 0x397 &&
                    (معلومات & HAS_ACCENT) != 0 &&
                    عدد_Ypogegrammeni == 0 &&
                    (state & AFTER_CASED) == 0 &&
                    !isFollowedByCasedLetter(src, بل_شاخص, src_اوږدتیا)) {
                // Keep disjunctive "or" with (only) a tonos.
                // We use the same "word boundary" conditions as for the Final_Sigma test.
                إذا (hasPrecomposedAccent) {
                    upper = 0x389;  // Preserve the precomposed form.
                } وإلا {
                    جمع_کړه_Tonos = ریښتیا;
                }
            } وإلا إذا ((معلومات & HAS_DIALYTIKA) != 0) {
                // Preserve a vowel with dialytika in precomposed form if it exists.
                إذا (upper == 0x399) {
                    upper = 0x3AA;
                    معلومات &= ~HAS_EITHER_DIALYTIKA;
                } وإلا إذا (upper == 0x3A5) {
                    upper = 0x3AB;
                    معلومات &= ~HAS_EITHER_DIALYTIKA;
                }
            }

            UBool change;
            إذا (edits == تش_اشاره && (options & U_OMIT_UNCHANGED_متن) == 0) {
                change = ریښتیا;  // common, simple usage
            } وإلا {
                // Find out first whether we are changing the text.
                U_ASSERT(0x370 <= upper && upper <= 0x3ff);  // 2-byte UTF-8, main Greek block
                change = (i + 2) > بل_شاخص ||
                        src[i] != getTwoByteLead(upper) || src[i + 1] != getTwoByteTrail(upper) ||
                        عدد_Ypogegrammeni > 0;
                int32_t i2 = i + 2;
                إذا ((معلومات & HAS_EITHER_DIALYTIKA) != 0) {
                    change |= (i2 + 2) > بل_شاخص ||
                            src[i2] != static_cast<uint8_t>(u8"\u0308"[0]) ||
                            src[i2 + 1] != static_cast<uint8_t>(u8"\u0308"[1]);
                    i2 += 2;
                }
                إذا (جمع_کړه_Tonos) {
                    change |= (i2 + 2) > بل_شاخص ||
                            src[i2] != static_cast<uint8_t>(u8"\u0301"[0]) ||
                            src[i2 + 1] != static_cast<uint8_t>(u8"\u0301"[1]);
                    i2 += 2;
                }
                int32_t old_اوږدتیا = بل_شاخص - i;
                int32_t new_اوږدتیا = (i2 - i) + عدد_Ypogegrammeni * 2;  // 2 bytes per U+0399
                change |= old_اوږدتیا != new_اوږدتیا;
                إذا (change) {
                    إذا (edits != تش_اشاره) {
                        edits->جمع_کړه_Replace(old_اوږدتیا, new_اوږدتیا);
                    }
                } وإلا {
                    إذا (edits != تش_اشاره) {
                        edits->جمع_کړه_Unchanged(old_اوږدتیا);
                    }
                    // Write unchanged text?
                    change = (options & U_OMIT_UNCHANGED_متن) == 0;
                }
            }

            إذا (change) {
                ByteSinkUtil::append_Two_بایټونه(upper, sink);
                إذا ((معلومات & HAS_EITHER_DIALYTIKA) != 0) {
                    sink.AppendU8(u8"\u0308", 2);  // restore or add a dialytika
                }
                إذا (جمع_کړه_Tonos) {
                    sink.AppendU8(u8"\u0301", 2);
                }
                طالما (عدد_Ypogegrammeni > 0) {
                    sink.AppendU8(u8"\u0399", 2);
                    --عدد_Ypogegrammeni;
                }
            }
        } وإلا إذا(c>=0) {
            ثابت char16_t *s;
            c=ucase_منزل_ډک_پاسنی(c, تش_اشاره, تش_اشاره, &s, UCASE_LOC_GREEK);
            إذا (!append_پایله(بل_شاخص - i, c, s, sink, options, edits, error_کوډ)) {
                أرجع;
            }
        } وإلا {
            // Malformed UTF-8.
            إذا (!ByteSinkUtil::appendUnchanged(src+i, بل_شاخص-i,
                                               sink, options, edits, error_کوډ)) {
                أرجع;
            }
        }
        i = بل_شاخص;
        state = بل_State;
    }
}

}  // namespace GreekUpper
U_NAMESPACE_پای_ټکی

ساكن فراغ U_CALLCONV
ucasemap_دننی_UTF8_منزل_ښکتنی(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_ناکارول_شوی
                             ثابت uint8_t *src, int32_t src_اوږدتیا,
                             icu::ByteSink &sink, icu::Edits *edits,
                             UError_کوډ &error_کوډ) {
    UCaseContext csc=UCASECONTEXT_INITIALIZER;
    csc.p=(فراغ *)src;
    csc.حد=src_اوږدتیا;
    منزل_ښکتنی(
        caseLocale, options,
        src, &csc, 0, src_اوږدتیا,
        sink, edits, error_کوډ);
}

ساكن فراغ U_CALLCONV
ucasemap_دننی_UTF8_منزل_پاسنی(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_ناکارول_شوی
                             ثابت uint8_t *src, int32_t src_اوږدتیا,
                             icu::ByteSink &sink, icu::Edits *edits,
                             UError_کوډ &error_کوډ) {
    إذا (caseLocale == UCASE_LOC_GREEK) {
        Greek_پاسنی::منزل_پاسنی(options, src, src_اوږدتیا, sink, edits, error_کوډ);
    } وإلا {
        UCaseContext csc=UCASECONTEXT_INITIALIZER;
        csc.p=(فراغ *)src;
        csc.حد=src_اوږدتیا;
        منزل_پاسنی(
            caseLocale, options,
            src, &csc, src_اوږدتیا,
            sink, edits, error_کوډ);
    }
}

ساكن فراغ U_CALLCONV
ucasemap_دننی_UTF8_Fold(int32_t /* caseLocale */, uint32_t options, UCASEMAP_BREAK_ITERATOR_ناکارول_شوی
                          ثابت uint8_t *src, int32_t src_اوږدتیا,
                          icu::ByteSink &sink, icu::Edits *edits,
                          UError_کوډ &error_کوډ) {
    منزل_ښکتنی(
        -1, options,
        src, تش_اشاره, 0, src_اوږدتیا,
        sink, edits, error_کوډ);
}

فراغ
ucasemap_mapUTF8(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_PARAM
                 ثابت حرف *src, int32_t src_اوږدتیا,
                 UTF8CaseMapper *stringCaseMapper,
                 icu::ByteSink &sink, icu::Edits *edits,
                 UError_کوډ &error_کوډ) {
    /* check argument values */
    إذا (U_FAILURE(error_کوډ)) {
        أرجع;
    }
    إذا ((src == تش_اشاره && src_اوږدتیا != 0) || src_اوږدتیا < -1) {
        error_کوډ = U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }

    // Get the string length.
    إذا (src_اوږدتیا == -1) {
        src_اوږدتیا = static_cast<int32_t>(uprv_strlen(src));
    }

    إذا (edits != تش_اشاره && (options & U_EDITS_NO_RESET) == 0) {
        edits->reset();
    }
    stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                     reinterpret_cast<ثابت uint8_t*>(src), src_اوږدتیا, sink, edits, error_کوډ);
    sink.Flush();
    إذا (U_SUCCESS(error_کوډ)) {
        إذا (edits != تش_اشاره) {
            edits->copy_Error_منزل(error_کوډ);
        }
    }
}

int32_t
ucasemap_mapUTF8(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_PARAM
                 حرف *dest, int32_t dest_ظرفیت,
                 ثابت حرف *src, int32_t src_اوږدتیا,
                 UTF8CaseMapper *stringCaseMapper,
                 icu::Edits *edits,
                 UError_کوډ &error_کوډ) {
    /* check argument values */
    إذا(U_FAILURE(error_کوډ)) {
        أرجع 0;
    }
    إذا( dest_ظرفیت<0 ||
        (dest==تش_اشاره && dest_ظرفیت>0) ||
        (src==تش_اشاره && src_اوږدتیا!=0) || src_اوږدتیا<-1
    ) {
        error_کوډ=U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }

    /* get the string length */
    إذا(src_اوږدتیا==-1) {
        src_اوږدتیا = static_cast<int32_t>(uprv_strlen(src));
    }

    /* check for overlapping source and destination */
    إذا( dest!=تش_اشاره &&
        ((src>=dest && src<(dest+dest_ظرفیت)) ||
         (dest>=src && dest<(src+src_اوږدتیا)))
    ) {
        error_کوډ=U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }

    إذا (edits != تش_اشاره && (options & U_EDITS_NO_RESET) == 0) {
        edits->reset();
    }
    int32_t reslen = ByteSinkUtil::له_لارې_Byte_Sink_منزل_Terminated_Chars(
        dest, dest_ظرفیت,
        [&](ByteSink& sink, UError_کوډ& حالت) {
            stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                             reinterpret_cast<ثابت uint8_t*>(src), src_اوږدتیا, sink, edits, حالت);
        },
        error_کوډ);
    إذا (U_SUCCESS(error_کوډ) && edits != تش_اشاره) {
        edits->copy_Error_منزل(error_کوډ);
    }
    أرجع reslen;
}

/* public API functions */

U_CAPI int32_t U_EXPORT2
ucasemap_utf8_منزل_ښکتنی(ثابت UCaseMap *csm,
                     حرف *dest, int32_t dest_ظرفیت,
                     ثابت حرف *src, int32_t src_اوږدتیا,
                     UError_کوډ *p_Error_کوډ) {
    خپلکاری *impl = reinterpret_cast<ثابت UCaseMapImpl *>(csm);
    أرجع ucasemap_mapUTF8(
        impl->caseLocale, impl->options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_ظرفیت,
        src, src_اوږدتیا,
        ucasemap_دننی_UTF8_منزل_ښکتنی, تش_اشاره, *p_Error_کوډ);
}

U_CAPI int32_t U_EXPORT2
ucasemap_utf8_منزل_پاسنی(ثابت UCaseMap *csm,
                     حرف *dest, int32_t dest_ظرفیت,
                     ثابت حرف *src, int32_t src_اوږدتیا,
                     UError_کوډ *p_Error_کوډ) {
    خپلکاری *impl = reinterpret_cast<ثابت UCaseMapImpl *>(csm);
    أرجع ucasemap_mapUTF8(
        impl->caseLocale, impl->options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_ظرفیت,
        src, src_اوږدتیا,
        ucasemap_دننی_UTF8_منزل_پاسنی, تش_اشاره, *p_Error_کوډ);
}

U_CAPI int32_t U_EXPORT2
ucasemap_utf8FoldCase(ثابت UCaseMap *csm,
                      حرف *dest, int32_t dest_ظرفیت,
                      ثابت حرف *src, int32_t src_اوږدتیا,
                      UError_کوډ *p_Error_کوډ) {
    خپلکاری *impl = reinterpret_cast<ثابت UCaseMapImpl *>(csm);
    أرجع ucasemap_mapUTF8(
        UCASE_LOC_ریښه, impl->options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_ظرفیت,
        src, src_اوږدتیا,
        ucasemap_دننی_UTF8_Fold, تش_اشاره, *p_Error_کوډ);
}

U_NAMESPACE_BEGIN

فراغ CaseMap::utf8_منزل_ښکتنی(
        ثابت حرف *locale, uint32_t options,
        StringPiece src, ByteSink &sink, Edits *edits,
        UError_کوډ &error_کوډ) {
    ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        src.معلومات(), src.length(),
        ucasemap_دننی_UTF8_منزل_ښکتنی, sink, edits, error_کوډ);
}

فراغ CaseMap::utf8_منزل_پاسنی(
        ثابت حرف *locale, uint32_t options,
        StringPiece src, ByteSink &sink, Edits *edits,
        UError_کوډ &error_کوډ) {
    ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        src.معلومات(), src.length(),
        ucasemap_دننی_UTF8_منزل_پاسنی, sink, edits, error_کوډ);
}

فراغ CaseMap::utf8Fold(
        uint32_t options,
        StringPiece src, ByteSink &sink, Edits *edits,
        UError_کوډ &error_کوډ) {
    ucasemap_mapUTF8(
        UCASE_LOC_ریښه, options, UCASEMAP_BREAK_ITERATOR_NULL
        src.معلومات(), src.length(),
        ucasemap_دننی_UTF8_Fold, sink, edits, error_کوډ);
}

int32_t CaseMap::utf8_منزل_ښکتنی(
        ثابت حرف *locale, uint32_t options,
        ثابت حرف *src, int32_t src_اوږدتیا,
        حرف *dest, int32_t dest_ظرفیت, Edits *edits,
        UError_کوډ &error_کوډ) {
    أرجع ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_ظرفیت,
        src, src_اوږدتیا,
        ucasemap_دننی_UTF8_منزل_ښکتنی, edits, error_کوډ);
}

int32_t CaseMap::utf8_منزل_پاسنی(
        ثابت حرف *locale, uint32_t options,
        ثابت حرف *src, int32_t src_اوږدتیا,
        حرف *dest, int32_t dest_ظرفیت, Edits *edits,
        UError_کوډ &error_کوډ) {
    أرجع ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_ظرفیت,
        src, src_اوږدتیا,
        ucasemap_دننی_UTF8_منزل_پاسنی, edits, error_کوډ);
}

int32_t CaseMap::utf8Fold(
        uint32_t options,
        ثابت حرف *src, int32_t src_اوږدتیا,
        حرف *dest, int32_t dest_ظرفیت, Edits *edits,
        UError_کوډ &error_کوډ) {
    أرجع ucasemap_mapUTF8(
        UCASE_LOC_ریښه, options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_ظرفیت,
        src, src_اوږدتیا,
        ucasemap_دننی_UTF8_Fold, edits, error_کوډ);
}

U_NAMESPACE_پای_ټکی
