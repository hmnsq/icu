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
#پایان_اگر
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

UCaseMapImpl::UCaseMapImpl(ثابت حرف *localeID, uint32_t opts, UError_کد *p_Error_کد) :
#إذا !UCONFIG_NO_BREAK_ITERATION
        iter(اشاره_تهی),
#پایان_اگر
        caseLocale(UCASE_LOC_UNKNOWN), options(opts) {
    ucasemap_مجموعه_Locale(reinterpret_cast<UCaseMap *>(هذا), localeID, p_Error_کد);
}

UCaseMapImpl::~UCaseMapImpl() {
#إذا !UCONFIG_NO_BREAK_ITERATION
    حذف iter;
#پایان_اگر
}

U_NAMESPACE_انتها

U_CAPI UCaseMap * U_EXPORT2
ucasemap_open(ثابت حرف *locale, uint32_t options, UError_کد *p_Error_کد) {
    إذا(U_FAILURE(*p_Error_کد)) {
        أرجع اشاره_تهی;
    }
    UCaseMapImpl *csm = جديد UCaseMapImpl(locale, options, p_Error_کد);
    إذا(csm==اشاره_تهی) {
        *p_Error_کد = U_حافظه_تخصیص_ERROR;
        أرجع اشاره_تهی;
    } وإلا إذا (U_FAILURE(*p_Error_کد)) {
        حذف csm;
        أرجع اشاره_تهی;
    }
    أرجع reinterpret_cast<UCaseMap *>(csm);
}

U_CAPI فراغ U_EXPORT2
ucasemap_close(UCaseMap *csm) {
    حذف reinterpret_cast<UCaseMapImpl *>(csm);
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
ucasemap_مجموعه_Locale(UCaseMap *csm, ثابت حرف *locale, UError_کد *p_Error_کد) {
    إذا(U_FAILURE(*p_Error_کد)) {
        أرجع;
    }
    خودکار *impl = reinterpret_cast<UCaseMapImpl *>(csm);
    إذا (locale != اشاره_تهی && *locale == 0) {
        impl->locale[0] = 0;
        impl->caseLocale = UCASE_LOC_ریشه;
        أرجع;
    }

    UError_کد میانگیر_وضعیت = U_ZERO_ERROR;
    int32_t length=uloc_getName(locale, impl->locale, (int32_t)حجم(impl->locale), &میانگیر_وضعیت);
    إذا(میانگیر_وضعیت==U_میانگیر_سرریز_ERROR || (U_SUCCESS(میانگیر_وضعیت) && length==حجم(impl->locale))) {
        میانگیر_وضعیت = U_ZERO_ERROR;
        /* we only really need the language code for case mappings */
        length=uloc_getLanguage(locale, impl->locale, (int32_t)حجم(impl->locale), &میانگیر_وضعیت);
    }
    إذا(U_FAILURE(میانگیر_وضعیت)) {
        *p_Error_کد=میانگیر_وضعیت;
    } وإلا إذا(length==حجم(impl->locale)) {
        *p_Error_کد=U_میانگیر_سرریز_ERROR;
    }
    إذا(U_SUCCESS(*p_Error_کد)) {     
        impl->caseLocale = ucase_getCaseLocale(impl->locale);
    } وإلا {
        impl->locale[0]=0;
        impl->caseLocale = UCASE_LOC_ریشه;
    }
}

U_CAPI فراغ U_EXPORT2
ucasemap_مجموعه_Options(UCaseMap *csm, uint32_t options, UError_کد *p_Error_کد) {
    إذا(U_FAILURE(*p_Error_کد)) {
        أرجع;
    }
    reinterpret_cast<UCaseMapImpl *>(csm)->options=options;
}

/* UTF-8 string case mappings ----------------------------------------------- */

/* TODO(markus): Move to a new, separate utf8case.cpp file. */

نطاق {

/* append a full case mapping result, see UCASE_MAX_STRING_LENGTH */
مضمن UBool
append_نتیجه(int32_t cp_درازی, int32_t نتیجه, ثابت char16_t *s,
             ByteSink &sink, uint32_t options, icu::Edits *edits, UError_کد &error_کد) {
    U_ASSERT(U_SUCCESS(error_کد));

    /* decode the result */
    إذا(نتیجه<0) {
        /* (not) original code point */
        إذا(edits!=اشاره_تهی) {
            edits->جمع_کن_Unchanged(cp_درازی);
        }
        إذا((options & U_OMIT_UNCHANGED_متن) == 0) {
            ByteSinkUtil::append_کد_نقطه(cp_درازی, ~نتیجه, sink);
        }
    } وإلا {
        إذا(نتیجه<=UCASE_MAX_STRING_درازی) {
            // string: "result" is the UTF-16 length
            أرجع ByteSinkUtil::appendChange(cp_درازی, s, نتیجه, sink, edits, error_کد);
        } وإلا {
            ByteSinkUtil::append_کد_نقطه(cp_درازی, نتیجه, sink, edits);
        }
    }
    أرجع درست;
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
        csc->شاخص=csc->cp_شروع;
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
        إذا(csc->شروع<csc->شاخص) {
            U8_PREV((ثابت uint8_t *)csc->p, csc->شروع, csc->شاخص, c);
            أرجع c;
        }
    } وإلا {
        إذا(csc->شاخص<csc->حد) {
            U8_بعدی((ثابت uint8_t *)csc->p, csc->شاخص, csc->حد, c);
            أرجع c;
        }
    }
    أرجع U_SENTINEL;
}

/**
 * caseLocale >= 0: Lowercases [srcStart..srcLimit[ but takes context [0..srcLength[ into account.
 * caseLocale < 0: Case-folds [srcStart..srcLimit[.
 */
فراغ مقصد_پایینی(int32_t caseLocale, uint32_t options,
             ثابت uint8_t *src, UCaseContext *csc, int32_t src_شروع, int32_t src_حد,
             icu::ByteSink &sink, icu::Edits *edits, UError_کد &error_کد) {
    ثابت int8_t *latin_مقصد_پایینی;
    إذا (caseLocale == UCASE_LOC_ریشه ||
            (caseLocale >= 0 ?
                !(caseLocale == UCASE_LOC_TURKISH || caseLocale == UCASE_LOC_LITHUANIAN) :
                (options & FOLD_CASE_OPTIONS_ماسک) == U_FOLD_CASE_DEFAULT)) {
        latin_مقصد_پایینی = LatinCase::مقصد_پایینی_نرمال;
    } وإلا {
        latin_مقصد_پایینی = LatinCase::مقصد_پایینی_TR_LT;
    }
    ثابت UTrie2 *trie = ucase_getTrie();
    int32_t prev = src_شروع;
    int32_t src_شاخص = src_شروع;
    لكل (;;) {
        // fast path for simple cases
        int32_t cp_شروع;
        UChar32 c;
        لكل (;;) {
            إذا (U_FAILURE(error_کد) || src_شاخص >= src_حد) {
                c = U_SENTINEL;
                اكسر;
            }
            uint8_t lead = src[src_شاخص++];
            إذا (lead <= 0x7f) {
                int8_t d = latin_مقصد_پایینی[lead];
                إذا (d == LatinCase::EXC) {
                    cp_شروع = src_شاخص - 1;
                    c = lead;
                    اكسر;
                }
                إذا (d == 0) { استمر; }
                ByteSinkUtil::appendUnchanged(src + prev, src_شاخص - 1 - prev,
                                              sink, options, edits, error_کد);
                حرف ascii = static_cast<حرف>(lead + d);
                sink.Append(&ascii, 1);
                إذا (edits != اشاره_تهی) {
                    edits->جمع_کن_Replace(1, 1);
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
                    int8_t d = latin_مقصد_پایینی[c];
                    إذا (d == LatinCase::EXC) {
                        cp_شروع = src_شاخص - 2;
                        اكسر;
                    }
                    إذا (d == 0) { استمر; }
                    ByteSinkUtil::appendUnchanged(src + prev, src_شاخص - 2 - prev,
                                                  sink, options, edits, error_کد);
                    ByteSinkUtil::append_Two_بایتها(c + d, sink);
                    إذا (edits != اشاره_تهی) {
                        edits->جمع_کن_Replace(2, 2);
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
            cp_شروع = --src_شاخص;
            U8_بعدی(src, src_شاخص, src_حد, c);
            إذا (c < 0) {
                // ill-formed UTF-8
                استمر;
            }
            uint16_t props = UTRIE2_GET16(trie, c);
            إذا (UCASE_HAS_EXCEPTION(props)) { اكسر; }
            int32_t delta;
            إذا (!UCASE_IS_فوقانی_OR_TITLE(props) || (delta = UCASE_GET_DELTA(props)) == 0) {
                استمر;
            }
            ByteSinkUtil::appendUnchanged(src + prev, cp_شروع - prev,
                                          sink, options, edits, error_کد);
            ByteSinkUtil::append_کد_نقطه(src_شاخص - cp_شروع, c + delta, sink, edits);
            prev = src_شاخص;
        }
        إذا (c < 0) {
            اكسر;
        }
        // slow path
        ثابت char16_t *s;
        إذا (caseLocale >= 0) {
            csc->cp_شروع = cp_شروع;
            csc->cp_حد = src_شاخص;
            c = ucase_مقصد_پر_پایینی(c, utf8_caseContextIterator, csc, &s, caseLocale);
        } وإلا {
            c = ucase_مقصد_پر_Folding(c, &s, options);
        }
        إذا (c >= 0) {
            ByteSinkUtil::appendUnchanged(src + prev, cp_شروع - prev,
                                          sink, options, edits, error_کد);
            append_نتیجه(src_شاخص - cp_شروع, c, s, sink, options, edits, error_کد);
            prev = src_شاخص;
        }
    }
    ByteSinkUtil::appendUnchanged(src + prev, src_شاخص - prev,
                                  sink, options, edits, error_کد);
}

فراغ مقصد_فوقانی(int32_t caseLocale, uint32_t options,
             ثابت uint8_t *src, UCaseContext *csc, int32_t src_درازی,
             icu::ByteSink &sink, icu::Edits *edits, UError_کد &error_کد) {
    ثابت int8_t *latin_مقصد_فوقانی;
    إذا (caseLocale == UCASE_LOC_TURKISH) {
        latin_مقصد_فوقانی = LatinCase::مقصد_فوقانی_TR;
    } وإلا {
        latin_مقصد_فوقانی = LatinCase::مقصد_فوقانی_نرمال;
    }
    ثابت UTrie2 *trie = ucase_getTrie();
    int32_t prev = 0;
    int32_t src_شاخص = 0;
    لكل (;;) {
        // fast path for simple cases
        int32_t cp_شروع;
        UChar32 c;
        لكل (;;) {
            إذا (U_FAILURE(error_کد) || src_شاخص >= src_درازی) {
                c = U_SENTINEL;
                اكسر;
            }
            uint8_t lead = src[src_شاخص++];
            إذا (lead <= 0x7f) {
                int8_t d = latin_مقصد_فوقانی[lead];
                إذا (d == LatinCase::EXC) {
                    cp_شروع = src_شاخص - 1;
                    c = lead;
                    اكسر;
                }
                إذا (d == 0) { استمر; }
                ByteSinkUtil::appendUnchanged(src + prev, src_شاخص - 1 - prev,
                                              sink, options, edits, error_کد);
                حرف ascii = static_cast<حرف>(lead + d);
                sink.Append(&ascii, 1);
                إذا (edits != اشاره_تهی) {
                    edits->جمع_کن_Replace(1, 1);
                }
                prev = src_شاخص;
                استمر;
            } وإلا إذا (lead < 0xe3) {
                uint8_t t;
                إذا (0xc2 <= lead && lead <= 0xc5 && src_شاخص < src_درازی &&
                        (t = src[src_شاخص] - 0x80) <= 0x3f) {
                    // U+0080..U+017F
                    ++src_شاخص;
                    c = ((lead - 0xc0) << 6) | t;
                    int8_t d = latin_مقصد_فوقانی[c];
                    إذا (d == LatinCase::EXC) {
                        cp_شروع = src_شاخص - 2;
                        اكسر;
                    }
                    إذا (d == 0) { استمر; }
                    ByteSinkUtil::appendUnchanged(src + prev, src_شاخص - 2 - prev,
                                                  sink, options, edits, error_کد);
                    ByteSinkUtil::append_Two_بایتها(c + d, sink);
                    إذا (edits != اشاره_تهی) {
                        edits->جمع_کن_Replace(2, 2);
                    }
                    prev = src_شاخص;
                    استمر;
                }
            } وإلا إذا ((lead <= 0xe9 || lead == 0xeb || lead == 0xec) &&
                    (src_شاخص + 2) <= src_درازی &&
                    U8_IS_TRAIL(src[src_شاخص]) && U8_IS_TRAIL(src[src_شاخص + 1])) {
                // most of CJK: no case mappings
                src_شاخص += 2;
                استمر;
            }
            cp_شروع = --src_شاخص;
            U8_بعدی(src, src_شاخص, src_درازی, c);
            إذا (c < 0) {
                // ill-formed UTF-8
                استمر;
            }
            uint16_t props = UTRIE2_GET16(trie, c);
            إذا (UCASE_HAS_EXCEPTION(props)) { اكسر; }
            int32_t delta;
            إذا (UCASE_GET_TYPE(props) != UCASE_پایینی || (delta = UCASE_GET_DELTA(props)) == 0) {
                استمر;
            }
            ByteSinkUtil::appendUnchanged(src + prev, cp_شروع - prev,
                                          sink, options, edits, error_کد);
            ByteSinkUtil::append_کد_نقطه(src_شاخص - cp_شروع, c + delta, sink, edits);
            prev = src_شاخص;
        }
        إذا (c < 0) {
            اكسر;
        }
        // slow path
        csc->cp_شروع = cp_شروع;
        csc->cp_حد = src_شاخص;
        ثابت char16_t *s;
        c = ucase_مقصد_پر_فوقانی(c, utf8_caseContextIterator, csc, &s, caseLocale);
        إذا (c >= 0) {
            ByteSinkUtil::appendUnchanged(src + prev, cp_شروع - prev,
                                          sink, options, edits, error_کد);
            append_نتیجه(src_شاخص - cp_شروع, c, s, sink, options, edits, error_کد);
            prev = src_شاخص;
        }
    }
    ByteSinkUtil::appendUnchanged(src + prev, src_شاخص - prev,
                                  sink, options, edits, error_کد);
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
int32_t maybeTitleDutchIJ(ثابت uint8_t *src, UChar32 c, int32_t شروع, int32_t segment_حد,
                          ByteSink &sink, uint32_t options, icu::Edits *edits, UError_کد &error_کد) {
    U_ASSERT(شروع < segment_حد);

    int32_t شاخص = شروع;
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
            withAcute = درست;
            unchanged1 = 2;  // ACUTE is 2 code units in UTF-8
            إذا (شاخص == segment_حد) { أرجع شروع; }
            c2 = src[شاخص++];
        }
    } وإلا {  // Í
        withAcute = درست;
    }

    // Is the next character a j/J?
    إذا (c2 == u'j') {
        doTitleJ = درست;
    } وإلا إذا (c2 == u'J') {
        ++unchanged1;
    } وإلا {
        أرجع شروع;
    }

    // A plain i/I must be followed by a plain j/J.
    // An i/I with acute must be followed by a j/J with acute.
    إذا (withAcute) {
        إذا ((شاخص + 1) >= segment_حد || src[شاخص++] != ACUTE_BYTE0 || src[شاخص++] != ACUTE_BYTE1) {
            أرجع شروع;
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
        U8_بعدی(src, i, segment_حد, cp);
        uint32_t type_ماسک = U_GET_GC_ماسک(cp);
        إذا ((type_ماسک & U_GC_M_ماسک) != 0) {
            أرجع شروع;
        }
    }

    // Output the rest of the Dutch IJ.
    ByteSinkUtil::appendUnchanged(src + شروع, unchanged1, sink, options, edits, error_کد);
    شروع += unchanged1;
    إذا (doTitleJ) {
        ByteSinkUtil::append_کد_نقطه(1, u'J', sink, edits);
        ++شروع;
    }
    ByteSinkUtil::appendUnchanged(src + شروع, unchanged2, sink, options, edits, error_کد);

    U_ASSERT(شروع + unchanged2 == شاخص);
    أرجع شاخص;
}

}  // namespace

U_CFUNC فراغ U_CALLCONV
ucasemap_داخلی_UTF8_مقصد_Title(
        int32_t caseLocale, uint32_t options, BreakIterator *iter,
        ثابت uint8_t *src, int32_t src_درازی,
        ByteSink &sink, icu::Edits *edits,
        UError_کد &error_کد) {
    إذا (!ustrcase_checkTitleAdjustmentOptions(options, error_کد)) {
        أرجع;
    }

    /* set up local variables */
    UCaseContext csc=UCASECONTEXT_INITIALIZER;
    csc.p=(فراغ *)src;
    csc.حد=src_درازی;
    int32_t prev=0;
    UBool is_اولی_شاخص=درست;

    /* titlecasing loop */
    طالما(prev<src_درازی) {
        /* find next index where to titlecase */
        int32_t شاخص;
        إذا(is_اولی_شاخص) {
            is_اولی_شاخص=خطأ;
            شاخص=iter->first();
        } وإلا {
            شاخص=iter->next();
        }
        إذا(شاخص==UBRK_انجامشده || شاخص>src_درازی) {
            شاخص=src_درازی;
        }

        /*
         * Segment [prev..index[ into 3 parts:
         * a) skipped characters (copy as-is) [prev..titleStart[
         * b) first letter (titlecase)              [titleStart..titleLimit[
         * c) subsequent characters (lowercase)                 [titleLimit..index[
         */
        إذا(prev<شاخص) {
            /* find and copy skipped characters [prev..titleStart[ */
            int32_t title_شروع=prev;
            int32_t title_حد=prev;
            UChar32 c;
            U8_بعدی(src, title_حد, شاخص, c);
            إذا ((options&U_TITLECASE_NO_BREAK_ADJUSTMENT)==0) {
                // Adjust the titlecasing index to the next cased character,
                // or to the next letter/number/symbol/private use.
                // Stop with titleStart<titleLimit<=index
                // if there is a character to be titlecased,
                // or else stop with titleStart==titleLimit==index.
                UBool مقصد_Cased = (options&U_TITLECASE_ADJUST_مقصد_CASED) != 0;
                طالما (مقصد_Cased ? UCASE_NONE==ucase_getType(c) : !ustrcase_isLNS(c)) {
                    title_شروع=title_حد;
                    إذا(title_حد==شاخص) {
                        اكسر;
                    }
                    U8_بعدی(src, title_حد, شاخص, c);
                }
                إذا (prev < title_شروع) {
                    إذا (!ByteSinkUtil::appendUnchanged(src+prev, title_شروع-prev,
                                                       sink, options, edits, error_کد)) {
                        أرجع;
                    }
                }
            }

            إذا(title_شروع<title_حد) {
                /* titlecase c which is from [titleStart..titleLimit[ */
                إذا(c>=0) {
                    csc.cp_شروع=title_شروع;
                    csc.cp_حد=title_حد;
                    ثابت char16_t *s;
                    c=ucase_مقصد_پر_Title(c, utf8_caseContextIterator, &csc, &s, caseLocale);
                    إذا (!append_نتیجه(title_حد-title_شروع, c, s, sink, options, edits, error_کد)) {
                        أرجع;
                    }
                } وإلا {
                    // Malformed UTF-8.
                    إذا (!ByteSinkUtil::appendUnchanged(src+title_شروع, title_حد-title_شروع,
                                                       sink, options, edits, error_کد)) {
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
                        title_حد = maybeTitleDutchIJ(src, c, title_حد, شاخص, sink, options, edits, error_کد);
                    }
                }

                /* lowercase [titleLimit..index[ */
                إذا(title_حد<شاخص) {
                    إذا((options&U_TITLECASE_NO_LOWERCASE)==0) {
                        /* Normal operation: Lowercase the rest of the word. */
                        مقصد_پایینی(caseLocale, options,
                                src, &csc, title_حد, شاخص,
                                sink, edits, error_کد);
                        إذا(U_FAILURE(error_کد)) {
                            أرجع;
                        }
                    } وإلا {
                        /* Optionally just copy the rest of the word unchanged. */
                        إذا (!ByteSinkUtil::appendUnchanged(src+title_حد, شاخص-title_حد,
                                                           sink, options, edits, error_کد)) {
                            أرجع;
                        }
                    }
                }
            }
        }

        prev=شاخص;
    }
}

#پایان_اگر

U_NAMESPACE_BEGIN
نطاق Greek_فوقانی {

UBool isFollowedByCasedLetter(ثابت uint8_t *s, int32_t i, int32_t length) {
    طالما (i < length) {
        UChar32 c;
        U8_بعدی(s, i, length, c);
        int32_t type = ucase_getTypeOrIgnorable(c);
        إذا ((type & UCASE_IGNORABLE) != 0) {
            // Case-ignorable, continue with the loop.
        } وإلا إذا (type != UCASE_NONE) {
            أرجع درست;  // Followed by cased letter.
        } وإلا {
            أرجع خطأ;  // Uncased and not case-ignorable.
        }
    }
    أرجع خطأ;  // Not followed by cased letter.
}

// Keep this consistent with the UTF-16 version in ustrcase.cpp and the Java version in CaseMap.java.
فراغ مقصد_فوقانی(uint32_t options,
             ثابت uint8_t *src, int32_t src_درازی,
             ByteSink &sink, Edits *edits,
             UError_کد &error_کد) {
    uint32_t state = 0;
    لكل (int32_t i = 0; i < src_درازی;) {
        int32_t بعدی_شاخص = i;
        UChar32 c;
        U8_بعدی(src, بعدی_شاخص, src_درازی, c);
        uint32_t بعدی_State = 0;
        int32_t type = ucase_getTypeOrIgnorable(c);
        إذا ((type & UCASE_IGNORABLE) != 0) {
            // c is case-ignorable
            بعدی_State |= (state & AFTER_CASED);
        } وإلا إذا (type != UCASE_NONE) {
            // c is cased
            بعدی_State |= AFTER_CASED;
        }
        uint32_t داده = get_Letter_داده(c);
        إذا (داده > 0) {
            uint32_t upper = داده & فوقانی_ماسک;
            // Add a dialytika to this iota or ypsilon vowel
            // if we removed a tonos from the previous vowel,
            // and that previous vowel did not also have (or gain) a dialytika.
            // Adding one only to the final vowel in a longer sequence
            // (which does not occur in normal writing) would require lookahead.
            // Set the same flag as for preserving an existing dialytika.
            إذا ((داده & HAS_VOWEL) != 0 &&
                (state & (AFTER_VOWEL_WITH_PRECOMPOSED_ACCENT | AFTER_VOWEL_WITH_COMBINING_ACCENT)) !=
                    0 &&
                (upper == 0x399 || upper == 0x3A5)) {
                داده |= (state & AFTER_VOWEL_WITH_PRECOMPOSED_ACCENT) != 0 ? HAS_DIALYTIKA
                                                                           : HAS_COMBINING_DIALYTIKA;
            }
            int32_t رقم_Ypogegrammeni = 0;  // Map each one to a trailing, spacing, capital iota.
            إذا ((داده & HAS_YPOGEGRAMMENI) != 0) {
                رقم_Ypogegrammeni = 1;
            }
            ثابت UBool hasPrecomposedAccent = (داده & HAS_ACCENT) != 0;
            // Skip combining diacritics after this Greek letter.
            int32_t بعدی_بعدی_شاخص = بعدی_شاخص;
            طالما (بعدی_شاخص < src_درازی) {
                UChar32 c2;
                U8_بعدی(src, بعدی_بعدی_شاخص, src_درازی, c2);
                uint32_t diacritic_داده = get_Diacritic_داده(c2);
                إذا (diacritic_داده != 0) {
                    داده |= diacritic_داده;
                    إذا ((diacritic_داده & HAS_YPOGEGRAMMENI) != 0) {
                        ++رقم_Ypogegrammeni;
                    }
                    بعدی_شاخص = بعدی_بعدی_شاخص;
                } وإلا {
                    اكسر;  // not a Greek diacritic
                }
            }
            إذا ((داده & HAS_VOWEL_AND_ACCENT_AND_DIALYTIKA) == HAS_VOWEL_AND_ACCENT) {
                بعدی_State |= hasPrecomposedAccent ? AFTER_VOWEL_WITH_PRECOMPOSED_ACCENT
                                                  : AFTER_VOWEL_WITH_COMBINING_ACCENT;
            }
            // Map according to Greek rules.
            UBool جمع_کن_Tonos = خطأ;
            إذا (upper == 0x397 &&
                    (داده & HAS_ACCENT) != 0 &&
                    رقم_Ypogegrammeni == 0 &&
                    (state & AFTER_CASED) == 0 &&
                    !isFollowedByCasedLetter(src, بعدی_شاخص, src_درازی)) {
                // Keep disjunctive "or" with (only) a tonos.
                // We use the same "word boundary" conditions as for the Final_Sigma test.
                إذا (hasPrecomposedAccent) {
                    upper = 0x389;  // Preserve the precomposed form.
                } وإلا {
                    جمع_کن_Tonos = درست;
                }
            } وإلا إذا ((داده & HAS_DIALYTIKA) != 0) {
                // Preserve a vowel with dialytika in precomposed form if it exists.
                إذا (upper == 0x399) {
                    upper = 0x3AA;
                    داده &= ~HAS_EITHER_DIALYTIKA;
                } وإلا إذا (upper == 0x3A5) {
                    upper = 0x3AB;
                    داده &= ~HAS_EITHER_DIALYTIKA;
                }
            }

            UBool change;
            إذا (edits == اشاره_تهی && (options & U_OMIT_UNCHANGED_متن) == 0) {
                change = درست;  // common, simple usage
            } وإلا {
                // Find out first whether we are changing the text.
                U_ASSERT(0x370 <= upper && upper <= 0x3ff);  // 2-byte UTF-8, main Greek block
                change = (i + 2) > بعدی_شاخص ||
                        src[i] != getTwoByteLead(upper) || src[i + 1] != getTwoByteTrail(upper) ||
                        رقم_Ypogegrammeni > 0;
                int32_t i2 = i + 2;
                إذا ((داده & HAS_EITHER_DIALYTIKA) != 0) {
                    change |= (i2 + 2) > بعدی_شاخص ||
                            src[i2] != static_cast<uint8_t>(u8"\u0308"[0]) ||
                            src[i2 + 1] != static_cast<uint8_t>(u8"\u0308"[1]);
                    i2 += 2;
                }
                إذا (جمع_کن_Tonos) {
                    change |= (i2 + 2) > بعدی_شاخص ||
                            src[i2] != static_cast<uint8_t>(u8"\u0301"[0]) ||
                            src[i2 + 1] != static_cast<uint8_t>(u8"\u0301"[1]);
                    i2 += 2;
                }
                int32_t old_درازی = بعدی_شاخص - i;
                int32_t new_درازی = (i2 - i) + رقم_Ypogegrammeni * 2;  // 2 bytes per U+0399
                change |= old_درازی != new_درازی;
                إذا (change) {
                    إذا (edits != اشاره_تهی) {
                        edits->جمع_کن_Replace(old_درازی, new_درازی);
                    }
                } وإلا {
                    إذا (edits != اشاره_تهی) {
                        edits->جمع_کن_Unchanged(old_درازی);
                    }
                    // Write unchanged text?
                    change = (options & U_OMIT_UNCHANGED_متن) == 0;
                }
            }

            إذا (change) {
                ByteSinkUtil::append_Two_بایتها(upper, sink);
                إذا ((داده & HAS_EITHER_DIALYTIKA) != 0) {
                    sink.AppendU8(u8"\u0308", 2);  // restore or add a dialytika
                }
                إذا (جمع_کن_Tonos) {
                    sink.AppendU8(u8"\u0301", 2);
                }
                طالما (رقم_Ypogegrammeni > 0) {
                    sink.AppendU8(u8"\u0399", 2);
                    --رقم_Ypogegrammeni;
                }
            }
        } وإلا إذا(c>=0) {
            ثابت char16_t *s;
            c=ucase_مقصد_پر_فوقانی(c, اشاره_تهی, اشاره_تهی, &s, UCASE_LOC_GREEK);
            إذا (!append_نتیجه(بعدی_شاخص - i, c, s, sink, options, edits, error_کد)) {
                أرجع;
            }
        } وإلا {
            // Malformed UTF-8.
            إذا (!ByteSinkUtil::appendUnchanged(src+i, بعدی_شاخص-i,
                                               sink, options, edits, error_کد)) {
                أرجع;
            }
        }
        i = بعدی_شاخص;
        state = بعدی_State;
    }
}

}  // namespace GreekUpper
U_NAMESPACE_انتها

ساكن فراغ U_CALLCONV
ucasemap_داخلی_UTF8_مقصد_پایینی(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_استفادهنشده
                             ثابت uint8_t *src, int32_t src_درازی,
                             icu::ByteSink &sink, icu::Edits *edits,
                             UError_کد &error_کد) {
    UCaseContext csc=UCASECONTEXT_INITIALIZER;
    csc.p=(فراغ *)src;
    csc.حد=src_درازی;
    مقصد_پایینی(
        caseLocale, options,
        src, &csc, 0, src_درازی,
        sink, edits, error_کد);
}

ساكن فراغ U_CALLCONV
ucasemap_داخلی_UTF8_مقصد_فوقانی(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_استفادهنشده
                             ثابت uint8_t *src, int32_t src_درازی,
                             icu::ByteSink &sink, icu::Edits *edits,
                             UError_کد &error_کد) {
    إذا (caseLocale == UCASE_LOC_GREEK) {
        Greek_فوقانی::مقصد_فوقانی(options, src, src_درازی, sink, edits, error_کد);
    } وإلا {
        UCaseContext csc=UCASECONTEXT_INITIALIZER;
        csc.p=(فراغ *)src;
        csc.حد=src_درازی;
        مقصد_فوقانی(
            caseLocale, options,
            src, &csc, src_درازی,
            sink, edits, error_کد);
    }
}

ساكن فراغ U_CALLCONV
ucasemap_داخلی_UTF8_Fold(int32_t /* caseLocale */, uint32_t options, UCASEMAP_BREAK_ITERATOR_استفادهنشده
                          ثابت uint8_t *src, int32_t src_درازی,
                          icu::ByteSink &sink, icu::Edits *edits,
                          UError_کد &error_کد) {
    مقصد_پایینی(
        -1, options,
        src, اشاره_تهی, 0, src_درازی,
        sink, edits, error_کد);
}

فراغ
ucasemap_mapUTF8(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_PARAM
                 ثابت حرف *src, int32_t src_درازی,
                 UTF8CaseMapper *stringCaseMapper,
                 icu::ByteSink &sink, icu::Edits *edits,
                 UError_کد &error_کد) {
    /* check argument values */
    إذا (U_FAILURE(error_کد)) {
        أرجع;
    }
    إذا ((src == اشاره_تهی && src_درازی != 0) || src_درازی < -1) {
        error_کد = U_ILLEGAL_آرگومان_ERROR;
        أرجع;
    }

    // Get the string length.
    إذا (src_درازی == -1) {
        src_درازی = static_cast<int32_t>(uprv_strlen(src));
    }

    إذا (edits != اشاره_تهی && (options & U_EDITS_NO_RESET) == 0) {
        edits->reset();
    }
    stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                     reinterpret_cast<ثابت uint8_t*>(src), src_درازی, sink, edits, error_کد);
    sink.Flush();
    إذا (U_SUCCESS(error_کد)) {
        إذا (edits != اشاره_تهی) {
            edits->copy_Error_مقصد(error_کد);
        }
    }
}

int32_t
ucasemap_mapUTF8(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_PARAM
                 حرف *dest, int32_t dest_ظرفیت,
                 ثابت حرف *src, int32_t src_درازی,
                 UTF8CaseMapper *stringCaseMapper,
                 icu::Edits *edits,
                 UError_کد &error_کد) {
    /* check argument values */
    إذا(U_FAILURE(error_کد)) {
        أرجع 0;
    }
    إذا( dest_ظرفیت<0 ||
        (dest==اشاره_تهی && dest_ظرفیت>0) ||
        (src==اشاره_تهی && src_درازی!=0) || src_درازی<-1
    ) {
        error_کد=U_ILLEGAL_آرگومان_ERROR;
        أرجع 0;
    }

    /* get the string length */
    إذا(src_درازی==-1) {
        src_درازی = static_cast<int32_t>(uprv_strlen(src));
    }

    /* check for overlapping source and destination */
    إذا( dest!=اشاره_تهی &&
        ((src>=dest && src<(dest+dest_ظرفیت)) ||
         (dest>=src && dest<(src+src_درازی)))
    ) {
        error_کد=U_ILLEGAL_آرگومان_ERROR;
        أرجع 0;
    }

    إذا (edits != اشاره_تهی && (options & U_EDITS_NO_RESET) == 0) {
        edits->reset();
    }
    int32_t reslen = ByteSinkUtil::از_طریق_Byte_Sink_مقصد_Terminated_Chars(
        dest, dest_ظرفیت,
        [&](ByteSink& sink, UError_کد& وضعیت) {
            stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                             reinterpret_cast<ثابت uint8_t*>(src), src_درازی, sink, edits, وضعیت);
        },
        error_کد);
    إذا (U_SUCCESS(error_کد) && edits != اشاره_تهی) {
        edits->copy_Error_مقصد(error_کد);
    }
    أرجع reslen;
}

/* public API functions */

U_CAPI int32_t U_EXPORT2
ucasemap_utf8_مقصد_پایینی(ثابت UCaseMap *csm,
                     حرف *dest, int32_t dest_ظرفیت,
                     ثابت حرف *src, int32_t src_درازی,
                     UError_کد *p_Error_کد) {
    خودکار *impl = reinterpret_cast<ثابت UCaseMapImpl *>(csm);
    أرجع ucasemap_mapUTF8(
        impl->caseLocale, impl->options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_ظرفیت,
        src, src_درازی,
        ucasemap_داخلی_UTF8_مقصد_پایینی, اشاره_تهی, *p_Error_کد);
}

U_CAPI int32_t U_EXPORT2
ucasemap_utf8_مقصد_فوقانی(ثابت UCaseMap *csm,
                     حرف *dest, int32_t dest_ظرفیت,
                     ثابت حرف *src, int32_t src_درازی,
                     UError_کد *p_Error_کد) {
    خودکار *impl = reinterpret_cast<ثابت UCaseMapImpl *>(csm);
    أرجع ucasemap_mapUTF8(
        impl->caseLocale, impl->options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_ظرفیت,
        src, src_درازی,
        ucasemap_داخلی_UTF8_مقصد_فوقانی, اشاره_تهی, *p_Error_کد);
}

U_CAPI int32_t U_EXPORT2
ucasemap_utf8FoldCase(ثابت UCaseMap *csm,
                      حرف *dest, int32_t dest_ظرفیت,
                      ثابت حرف *src, int32_t src_درازی,
                      UError_کد *p_Error_کد) {
    خودکار *impl = reinterpret_cast<ثابت UCaseMapImpl *>(csm);
    أرجع ucasemap_mapUTF8(
        UCASE_LOC_ریشه, impl->options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_ظرفیت,
        src, src_درازی,
        ucasemap_داخلی_UTF8_Fold, اشاره_تهی, *p_Error_کد);
}

U_NAMESPACE_BEGIN

فراغ CaseMap::utf8_مقصد_پایینی(
        ثابت حرف *locale, uint32_t options,
        StringPiece src, ByteSink &sink, Edits *edits,
        UError_کد &error_کد) {
    ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        src.داده(), src.length(),
        ucasemap_داخلی_UTF8_مقصد_پایینی, sink, edits, error_کد);
}

فراغ CaseMap::utf8_مقصد_فوقانی(
        ثابت حرف *locale, uint32_t options,
        StringPiece src, ByteSink &sink, Edits *edits,
        UError_کد &error_کد) {
    ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        src.داده(), src.length(),
        ucasemap_داخلی_UTF8_مقصد_فوقانی, sink, edits, error_کد);
}

فراغ CaseMap::utf8Fold(
        uint32_t options,
        StringPiece src, ByteSink &sink, Edits *edits,
        UError_کد &error_کد) {
    ucasemap_mapUTF8(
        UCASE_LOC_ریشه, options, UCASEMAP_BREAK_ITERATOR_NULL
        src.داده(), src.length(),
        ucasemap_داخلی_UTF8_Fold, sink, edits, error_کد);
}

int32_t CaseMap::utf8_مقصد_پایینی(
        ثابت حرف *locale, uint32_t options,
        ثابت حرف *src, int32_t src_درازی,
        حرف *dest, int32_t dest_ظرفیت, Edits *edits,
        UError_کد &error_کد) {
    أرجع ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_ظرفیت,
        src, src_درازی,
        ucasemap_داخلی_UTF8_مقصد_پایینی, edits, error_کد);
}

int32_t CaseMap::utf8_مقصد_فوقانی(
        ثابت حرف *locale, uint32_t options,
        ثابت حرف *src, int32_t src_درازی,
        حرف *dest, int32_t dest_ظرفیت, Edits *edits,
        UError_کد &error_کد) {
    أرجع ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_ظرفیت,
        src, src_درازی,
        ucasemap_داخلی_UTF8_مقصد_فوقانی, edits, error_کد);
}

int32_t CaseMap::utf8Fold(
        uint32_t options,
        ثابت حرف *src, int32_t src_درازی,
        حرف *dest, int32_t dest_ظرفیت, Edits *edits,
        UError_کد &error_کد) {
    أرجع ucasemap_mapUTF8(
        UCASE_LOC_ریشه, options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_ظرفیت,
        src, src_درازی,
        ucasemap_داخلی_UTF8_Fold, edits, error_کد);
}

U_NAMESPACE_انتها
