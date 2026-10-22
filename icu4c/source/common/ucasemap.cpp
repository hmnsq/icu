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
#جيڪڏهن_پورو
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

UCaseMapImpl::UCaseMapImpl(ثابت حرف *localeID, uint32_t opts, UError_ڪوڊ *p_Error_ڪوڊ) :
#إذا !UCONFIG_NO_BREAK_ITERATION
        iter(خالي_اشارو),
#جيڪڏهن_پورو
        caseLocale(UCASE_LOC_UNKNOWN), options(opts) {
    ucasemap_سيٽ_Locale(reinterpret_cast<UCaseMap *>(هذا), localeID, p_Error_ڪوڊ);
}

UCaseMapImpl::~UCaseMapImpl() {
#إذا !UCONFIG_NO_BREAK_ITERATION
    ميٽيو iter;
#جيڪڏهن_پورو
}

U_NAMESPACE_پڇاڙي

U_CAPI UCaseMap * U_EXPORT2
ucasemap_open(ثابت حرف *locale, uint32_t options, UError_ڪوڊ *p_Error_ڪوڊ) {
    إذا(U_FAILURE(*p_Error_ڪوڊ)) {
        أرجع خالي_اشارو;
    }
    UCaseMapImpl *csm = جديد UCaseMapImpl(locale, options, p_Error_ڪوڊ);
    إذا(csm==خالي_اشارو) {
        *p_Error_ڪوڊ = U_يادگيري_ورهاست_ERROR;
        أرجع خالي_اشارو;
    } وإلا إذا (U_FAILURE(*p_Error_ڪوڊ)) {
        ميٽيو csm;
        أرجع خالي_اشارو;
    }
    أرجع reinterpret_cast<UCaseMap *>(csm);
}

U_CAPI فراغ U_EXPORT2
ucasemap_close(UCaseMap *csm) {
    ميٽيو reinterpret_cast<UCaseMapImpl *>(csm);
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
ucasemap_سيٽ_Locale(UCaseMap *csm, ثابت حرف *locale, UError_ڪوڊ *p_Error_ڪوڊ) {
    إذا(U_FAILURE(*p_Error_ڪوڊ)) {
        أرجع;
    }
    خودڪار *impl = reinterpret_cast<UCaseMapImpl *>(csm);
    إذا (locale != خالي_اشارو && *locale == 0) {
        impl->locale[0] = 0;
        impl->caseLocale = UCASE_LOC_پاڙ;
        أرجع;
    }

    UError_ڪوڊ بفر_حالت = U_ZERO_ERROR;
    int32_t length=uloc_getName(locale, impl->locale, (int32_t)حجم(impl->locale), &بفر_حالت);
    إذا(بفر_حالت==U_بفر_حد_کان_مٿي_ERROR || (U_SUCCESS(بفر_حالت) && length==حجم(impl->locale))) {
        بفر_حالت = U_ZERO_ERROR;
        /* we only really need the language code for case mappings */
        length=uloc_getLanguage(locale, impl->locale, (int32_t)حجم(impl->locale), &بفر_حالت);
    }
    إذا(U_FAILURE(بفر_حالت)) {
        *p_Error_ڪوڊ=بفر_حالت;
    } وإلا إذا(length==حجم(impl->locale)) {
        *p_Error_ڪوڊ=U_بفر_حد_کان_مٿي_ERROR;
    }
    إذا(U_SUCCESS(*p_Error_ڪوڊ)) {     
        impl->caseLocale = ucase_getCaseLocale(impl->locale);
    } وإلا {
        impl->locale[0]=0;
        impl->caseLocale = UCASE_LOC_پاڙ;
    }
}

U_CAPI فراغ U_EXPORT2
ucasemap_سيٽ_Options(UCaseMap *csm, uint32_t options, UError_ڪوڊ *p_Error_ڪوڊ) {
    إذا(U_FAILURE(*p_Error_ڪوڊ)) {
        أرجع;
    }
    reinterpret_cast<UCaseMapImpl *>(csm)->options=options;
}

/* UTF-8 string case mappings ----------------------------------------------- */

/* TODO(markus): Move to a new, separate utf8case.cpp file. */

نطاق {

/* append a full case mapping result, see UCASE_MAX_STRING_LENGTH */
مضمن UBool
append_نتيجو(int32_t cp_ڊيگهائي, int32_t نتيجو, ثابت char16_t *s,
             ByteSink &sink, uint32_t options, icu::Edits *edits, UError_ڪوڊ &error_ڪوڊ) {
    U_ASSERT(U_SUCCESS(error_ڪوڊ));

    /* decode the result */
    إذا(نتيجو<0) {
        /* (not) original code point */
        إذا(edits!=خالي_اشارو) {
            edits->جوڙيو_Unchanged(cp_ڊيگهائي);
        }
        إذا((options & U_OMIT_UNCHANGED_متن) == 0) {
            ByteSinkUtil::append_ڪوڊ_نقطو(cp_ڊيگهائي, ~نتيجو, sink);
        }
    } وإلا {
        إذا(نتيجو<=UCASE_MAX_STRING_ڊيگهائي) {
            // string: "result" is the UTF-16 length
            أرجع ByteSinkUtil::appendChange(cp_ڊيگهائي, s, نتيجو, sink, edits, error_ڪوڊ);
        } وإلا {
            ByteSinkUtil::append_ڪوڊ_نقطو(cp_ڊيگهائي, نتيجو, sink, edits);
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
        csc->اشاريو=csc->cp_شروعات;
        csc->dir=dir;
    } وإلا إذا(dir>0) {
        /* reset for forward iteration */
        csc->اشاريو=csc->cp_حد_بندي;
        csc->dir=dir;
    } وإلا {
        /* continue current iteration direction */
        dir=csc->dir;
    }

    إذا(dir<0) {
        إذا(csc->شروعات<csc->اشاريو) {
            U8_PREV((ثابت uint8_t *)csc->p, csc->شروعات, csc->اشاريو, c);
            أرجع c;
        }
    } وإلا {
        إذا(csc->اشاريو<csc->حد_بندي) {
            U8_ايندڙ((ثابت uint8_t *)csc->p, csc->اشاريو, csc->حد_بندي, c);
            أرجع c;
        }
    }
    أرجع U_SENTINEL;
}

/**
 * caseLocale >= 0: Lowercases [srcStart..srcLimit[ but takes context [0..srcLength[ into account.
 * caseLocale < 0: Case-folds [srcStart..srcLimit[.
 */
فراغ منزل_هيٺاهون(int32_t caseLocale, uint32_t options,
             ثابت uint8_t *src, UCaseContext *csc, int32_t src_شروعات, int32_t src_حد_بندي,
             icu::ByteSink &sink, icu::Edits *edits, UError_ڪوڊ &error_ڪوڊ) {
    ثابت int8_t *latin_منزل_هيٺاهون;
    إذا (caseLocale == UCASE_LOC_پاڙ ||
            (caseLocale >= 0 ?
                !(caseLocale == UCASE_LOC_TURKISH || caseLocale == UCASE_LOC_LITHUANIAN) :
                (options & FOLD_CASE_OPTIONS_نقاب) == U_FOLD_CASE_DEFAULT)) {
        latin_منزل_هيٺاهون = LatinCase::منزل_هيٺاهون_عام;
    } وإلا {
        latin_منزل_هيٺاهون = LatinCase::منزل_هيٺاهون_TR_LT;
    }
    ثابت UTrie2 *trie = ucase_getTrie();
    int32_t prev = src_شروعات;
    int32_t src_اشاريو = src_شروعات;
    لكل (;;) {
        // fast path for simple cases
        int32_t cp_شروعات;
        UChar32 c;
        لكل (;;) {
            إذا (U_FAILURE(error_ڪوڊ) || src_اشاريو >= src_حد_بندي) {
                c = U_SENTINEL;
                اكسر;
            }
            uint8_t lead = src[src_اشاريو++];
            إذا (lead <= 0x7f) {
                int8_t d = latin_منزل_هيٺاهون[lead];
                إذا (d == LatinCase::EXC) {
                    cp_شروعات = src_اشاريو - 1;
                    c = lead;
                    اكسر;
                }
                إذا (d == 0) { استمر; }
                ByteSinkUtil::appendUnchanged(src + prev, src_اشاريو - 1 - prev,
                                              sink, options, edits, error_ڪوڊ);
                حرف ascii = static_cast<حرف>(lead + d);
                sink.Append(&ascii, 1);
                إذا (edits != خالي_اشارو) {
                    edits->جوڙيو_Replace(1, 1);
                }
                prev = src_اشاريو;
                استمر;
            } وإلا إذا (lead < 0xe3) {
                uint8_t t;
                إذا (0xc2 <= lead && lead <= 0xc5 && src_اشاريو < src_حد_بندي &&
                        (t = src[src_اشاريو] - 0x80) <= 0x3f) {
                    // U+0080..U+017F
                    ++src_اشاريو;
                    c = ((lead - 0xc0) << 6) | t;
                    int8_t d = latin_منزل_هيٺاهون[c];
                    إذا (d == LatinCase::EXC) {
                        cp_شروعات = src_اشاريو - 2;
                        اكسر;
                    }
                    إذا (d == 0) { استمر; }
                    ByteSinkUtil::appendUnchanged(src + prev, src_اشاريو - 2 - prev,
                                                  sink, options, edits, error_ڪوڊ);
                    ByteSinkUtil::append_Two_بائيٽون(c + d, sink);
                    إذا (edits != خالي_اشارو) {
                        edits->جوڙيو_Replace(2, 2);
                    }
                    prev = src_اشاريو;
                    استمر;
                }
            } وإلا إذا ((lead <= 0xe9 || lead == 0xeb || lead == 0xec) &&
                    (src_اشاريو + 2) <= src_حد_بندي &&
                    U8_IS_TRAIL(src[src_اشاريو]) && U8_IS_TRAIL(src[src_اشاريو + 1])) {
                // most of CJK: no case mappings
                src_اشاريو += 2;
                استمر;
            }
            cp_شروعات = --src_اشاريو;
            U8_ايندڙ(src, src_اشاريو, src_حد_بندي, c);
            إذا (c < 0) {
                // ill-formed UTF-8
                استمر;
            }
            uint16_t props = UTRIE2_GET16(trie, c);
            إذا (UCASE_HAS_EXCEPTION(props)) { اكسر; }
            int32_t delta;
            إذا (!UCASE_IS_مٿانهون_OR_TITLE(props) || (delta = UCASE_GET_DELTA(props)) == 0) {
                استمر;
            }
            ByteSinkUtil::appendUnchanged(src + prev, cp_شروعات - prev,
                                          sink, options, edits, error_ڪوڊ);
            ByteSinkUtil::append_ڪوڊ_نقطو(src_اشاريو - cp_شروعات, c + delta, sink, edits);
            prev = src_اشاريو;
        }
        إذا (c < 0) {
            اكسر;
        }
        // slow path
        ثابت char16_t *s;
        إذا (caseLocale >= 0) {
            csc->cp_شروعات = cp_شروعات;
            csc->cp_حد_بندي = src_اشاريو;
            c = ucase_منزل_ڀريل_هيٺاهون(c, utf8_caseContextIterator, csc, &s, caseLocale);
        } وإلا {
            c = ucase_منزل_ڀريل_Folding(c, &s, options);
        }
        إذا (c >= 0) {
            ByteSinkUtil::appendUnchanged(src + prev, cp_شروعات - prev,
                                          sink, options, edits, error_ڪوڊ);
            append_نتيجو(src_اشاريو - cp_شروعات, c, s, sink, options, edits, error_ڪوڊ);
            prev = src_اشاريو;
        }
    }
    ByteSinkUtil::appendUnchanged(src + prev, src_اشاريو - prev,
                                  sink, options, edits, error_ڪوڊ);
}

فراغ منزل_مٿانهون(int32_t caseLocale, uint32_t options,
             ثابت uint8_t *src, UCaseContext *csc, int32_t src_ڊيگهائي,
             icu::ByteSink &sink, icu::Edits *edits, UError_ڪوڊ &error_ڪوڊ) {
    ثابت int8_t *latin_منزل_مٿانهون;
    إذا (caseLocale == UCASE_LOC_TURKISH) {
        latin_منزل_مٿانهون = LatinCase::منزل_مٿانهون_TR;
    } وإلا {
        latin_منزل_مٿانهون = LatinCase::منزل_مٿانهون_عام;
    }
    ثابت UTrie2 *trie = ucase_getTrie();
    int32_t prev = 0;
    int32_t src_اشاريو = 0;
    لكل (;;) {
        // fast path for simple cases
        int32_t cp_شروعات;
        UChar32 c;
        لكل (;;) {
            إذا (U_FAILURE(error_ڪوڊ) || src_اشاريو >= src_ڊيگهائي) {
                c = U_SENTINEL;
                اكسر;
            }
            uint8_t lead = src[src_اشاريو++];
            إذا (lead <= 0x7f) {
                int8_t d = latin_منزل_مٿانهون[lead];
                إذا (d == LatinCase::EXC) {
                    cp_شروعات = src_اشاريو - 1;
                    c = lead;
                    اكسر;
                }
                إذا (d == 0) { استمر; }
                ByteSinkUtil::appendUnchanged(src + prev, src_اشاريو - 1 - prev,
                                              sink, options, edits, error_ڪوڊ);
                حرف ascii = static_cast<حرف>(lead + d);
                sink.Append(&ascii, 1);
                إذا (edits != خالي_اشارو) {
                    edits->جوڙيو_Replace(1, 1);
                }
                prev = src_اشاريو;
                استمر;
            } وإلا إذا (lead < 0xe3) {
                uint8_t t;
                إذا (0xc2 <= lead && lead <= 0xc5 && src_اشاريو < src_ڊيگهائي &&
                        (t = src[src_اشاريو] - 0x80) <= 0x3f) {
                    // U+0080..U+017F
                    ++src_اشاريو;
                    c = ((lead - 0xc0) << 6) | t;
                    int8_t d = latin_منزل_مٿانهون[c];
                    إذا (d == LatinCase::EXC) {
                        cp_شروعات = src_اشاريو - 2;
                        اكسر;
                    }
                    إذا (d == 0) { استمر; }
                    ByteSinkUtil::appendUnchanged(src + prev, src_اشاريو - 2 - prev,
                                                  sink, options, edits, error_ڪوڊ);
                    ByteSinkUtil::append_Two_بائيٽون(c + d, sink);
                    إذا (edits != خالي_اشارو) {
                        edits->جوڙيو_Replace(2, 2);
                    }
                    prev = src_اشاريو;
                    استمر;
                }
            } وإلا إذا ((lead <= 0xe9 || lead == 0xeb || lead == 0xec) &&
                    (src_اشاريو + 2) <= src_ڊيگهائي &&
                    U8_IS_TRAIL(src[src_اشاريو]) && U8_IS_TRAIL(src[src_اشاريو + 1])) {
                // most of CJK: no case mappings
                src_اشاريو += 2;
                استمر;
            }
            cp_شروعات = --src_اشاريو;
            U8_ايندڙ(src, src_اشاريو, src_ڊيگهائي, c);
            إذا (c < 0) {
                // ill-formed UTF-8
                استمر;
            }
            uint16_t props = UTRIE2_GET16(trie, c);
            إذا (UCASE_HAS_EXCEPTION(props)) { اكسر; }
            int32_t delta;
            إذا (UCASE_GET_TYPE(props) != UCASE_هيٺاهون || (delta = UCASE_GET_DELTA(props)) == 0) {
                استمر;
            }
            ByteSinkUtil::appendUnchanged(src + prev, cp_شروعات - prev,
                                          sink, options, edits, error_ڪوڊ);
            ByteSinkUtil::append_ڪوڊ_نقطو(src_اشاريو - cp_شروعات, c + delta, sink, edits);
            prev = src_اشاريو;
        }
        إذا (c < 0) {
            اكسر;
        }
        // slow path
        csc->cp_شروعات = cp_شروعات;
        csc->cp_حد_بندي = src_اشاريو;
        ثابت char16_t *s;
        c = ucase_منزل_ڀريل_مٿانهون(c, utf8_caseContextIterator, csc, &s, caseLocale);
        إذا (c >= 0) {
            ByteSinkUtil::appendUnchanged(src + prev, cp_شروعات - prev,
                                          sink, options, edits, error_ڪوڊ);
            append_نتيجو(src_اشاريو - cp_شروعات, c, s, sink, options, edits, error_ڪوڊ);
            prev = src_اشاريو;
        }
    }
    ByteSinkUtil::appendUnchanged(src + prev, src_اشاريو - prev,
                                  sink, options, edits, error_ڪوڊ);
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
int32_t maybeTitleDutchIJ(ثابت uint8_t *src, UChar32 c, int32_t شروعات, int32_t segment_حد_بندي,
                          ByteSink &sink, uint32_t options, icu::Edits *edits, UError_ڪوڊ &error_ڪوڊ) {
    U_ASSERT(شروعات < segment_حد_بندي);

    int32_t اشاريو = شروعات;
    منطقي withAcute = خطأ;

    // If the conditions are met, then the following variables tell us what to output.
    int32_t unchanged1 = 0;  // code units before the j, or the whole sequence (0..3)
    منطقي doTitleJ = خطأ;  // true if the j needs to be titlecased
    int32_t unchanged2 = 0;  // after the j (0 or 1)

    // next character after the first letter
    UChar32 c2;
    c2 = src[اشاريو++];

    // Is the first letter an i/I with accent?
    إذا (c == u'I') {
        إذا (c2 == ACUTE_BYTE0 && اشاريو < segment_حد_بندي && src[اشاريو++] == ACUTE_BYTE1) {
            withAcute = سچ;
            unchanged1 = 2;  // ACUTE is 2 code units in UTF-8
            إذا (اشاريو == segment_حد_بندي) { أرجع شروعات; }
            c2 = src[اشاريو++];
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
        أرجع شروعات;
    }

    // A plain i/I must be followed by a plain j/J.
    // An i/I with acute must be followed by a j/J with acute.
    إذا (withAcute) {
        إذا ((اشاريو + 1) >= segment_حد_بندي || src[اشاريو++] != ACUTE_BYTE0 || src[اشاريو++] != ACUTE_BYTE1) {
            أرجع شروعات;
        }
        إذا (doTitleJ) {
            unchanged2 = 2;  // ACUTE is 2 code units in UTF-8
        } وإلا {
            unchanged1 = unchanged1 + 2;    // ACUTE is 2 code units in UTF-8
        }
    }

    // There must not be another combining mark.
    إذا (اشاريو < segment_حد_بندي) {
        int32_t cp;
        int32_t i = اشاريو;
        U8_ايندڙ(src, i, segment_حد_بندي, cp);
        uint32_t type_نقاب = U_GET_GC_نقاب(cp);
        إذا ((type_نقاب & U_GC_M_نقاب) != 0) {
            أرجع شروعات;
        }
    }

    // Output the rest of the Dutch IJ.
    ByteSinkUtil::appendUnchanged(src + شروعات, unchanged1, sink, options, edits, error_ڪوڊ);
    شروعات += unchanged1;
    إذا (doTitleJ) {
        ByteSinkUtil::append_ڪوڊ_نقطو(1, u'J', sink, edits);
        ++شروعات;
    }
    ByteSinkUtil::appendUnchanged(src + شروعات, unchanged2, sink, options, edits, error_ڪوڊ);

    U_ASSERT(شروعات + unchanged2 == اشاريو);
    أرجع اشاريو;
}

}  // namespace

U_CFUNC فراغ U_CALLCONV
ucasemap_اندروني_UTF8_منزل_Title(
        int32_t caseLocale, uint32_t options, BreakIterator *iter,
        ثابت uint8_t *src, int32_t src_ڊيگهائي,
        ByteSink &sink, icu::Edits *edits,
        UError_ڪوڊ &error_ڪوڊ) {
    إذا (!ustrcase_checkTitleAdjustmentOptions(options, error_ڪوڊ)) {
        أرجع;
    }

    /* set up local variables */
    UCaseContext csc=UCASECONTEXT_INITIALIZER;
    csc.p=(فراغ *)src;
    csc.حد_بندي=src_ڊيگهائي;
    int32_t prev=0;
    UBool is_پهريون_اشاريو=سچ;

    /* titlecasing loop */
    طالما(prev<src_ڊيگهائي) {
        /* find next index where to titlecase */
        int32_t اشاريو;
        إذا(is_پهريون_اشاريو) {
            is_پهريون_اشاريو=خطأ;
            اشاريو=iter->first();
        } وإلا {
            اشاريو=iter->next();
        }
        إذا(اشاريو==UBRK_پورو || اشاريو>src_ڊيگهائي) {
            اشاريو=src_ڊيگهائي;
        }

        /*
         * Segment [prev..index[ into 3 parts:
         * a) skipped characters (copy as-is) [prev..titleStart[
         * b) first letter (titlecase)              [titleStart..titleLimit[
         * c) subsequent characters (lowercase)                 [titleLimit..index[
         */
        إذا(prev<اشاريو) {
            /* find and copy skipped characters [prev..titleStart[ */
            int32_t title_شروعات=prev;
            int32_t title_حد_بندي=prev;
            UChar32 c;
            U8_ايندڙ(src, title_حد_بندي, اشاريو, c);
            إذا ((options&U_TITLECASE_NO_BREAK_ADJUSTMENT)==0) {
                // Adjust the titlecasing index to the next cased character,
                // or to the next letter/number/symbol/private use.
                // Stop with titleStart<titleLimit<=index
                // if there is a character to be titlecased,
                // or else stop with titleStart==titleLimit==index.
                UBool منزل_Cased = (options&U_TITLECASE_ADJUST_منزل_CASED) != 0;
                طالما (منزل_Cased ? UCASE_NONE==ucase_getType(c) : !ustrcase_isLNS(c)) {
                    title_شروعات=title_حد_بندي;
                    إذا(title_حد_بندي==اشاريو) {
                        اكسر;
                    }
                    U8_ايندڙ(src, title_حد_بندي, اشاريو, c);
                }
                إذا (prev < title_شروعات) {
                    إذا (!ByteSinkUtil::appendUnchanged(src+prev, title_شروعات-prev,
                                                       sink, options, edits, error_ڪوڊ)) {
                        أرجع;
                    }
                }
            }

            إذا(title_شروعات<title_حد_بندي) {
                /* titlecase c which is from [titleStart..titleLimit[ */
                إذا(c>=0) {
                    csc.cp_شروعات=title_شروعات;
                    csc.cp_حد_بندي=title_حد_بندي;
                    ثابت char16_t *s;
                    c=ucase_منزل_ڀريل_Title(c, utf8_caseContextIterator, &csc, &s, caseLocale);
                    إذا (!append_نتيجو(title_حد_بندي-title_شروعات, c, s, sink, options, edits, error_ڪوڊ)) {
                        أرجع;
                    }
                } وإلا {
                    // Malformed UTF-8.
                    إذا (!ByteSinkUtil::appendUnchanged(src+title_شروعات, title_حد_بندي-title_شروعات,
                                                       sink, options, edits, error_ڪوڊ)) {
                        أرجع;
                    }
                }

                /* Special case Dutch IJ titlecasing */
                إذا (title_حد_بندي < اشاريو &&
                    caseLocale == UCASE_LOC_DUTCH) {
                    إذا (c < 0) {
                        c = ~c;
                    }

                    إذا (c == u'I' || c == u'Í') {
                        title_حد_بندي = maybeTitleDutchIJ(src, c, title_حد_بندي, اشاريو, sink, options, edits, error_ڪوڊ);
                    }
                }

                /* lowercase [titleLimit..index[ */
                إذا(title_حد_بندي<اشاريو) {
                    إذا((options&U_TITLECASE_NO_LOWERCASE)==0) {
                        /* Normal operation: Lowercase the rest of the word. */
                        منزل_هيٺاهون(caseLocale, options,
                                src, &csc, title_حد_بندي, اشاريو,
                                sink, edits, error_ڪوڊ);
                        إذا(U_FAILURE(error_ڪوڊ)) {
                            أرجع;
                        }
                    } وإلا {
                        /* Optionally just copy the rest of the word unchanged. */
                        إذا (!ByteSinkUtil::appendUnchanged(src+title_حد_بندي, اشاريو-title_حد_بندي,
                                                           sink, options, edits, error_ڪوڊ)) {
                            أرجع;
                        }
                    }
                }
            }
        }

        prev=اشاريو;
    }
}

#جيڪڏهن_پورو

U_NAMESPACE_BEGIN
نطاق Greek_مٿانهون {

UBool isFollowedByCasedLetter(ثابت uint8_t *s, int32_t i, int32_t length) {
    طالما (i < length) {
        UChar32 c;
        U8_ايندڙ(s, i, length, c);
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
فراغ منزل_مٿانهون(uint32_t options,
             ثابت uint8_t *src, int32_t src_ڊيگهائي,
             ByteSink &sink, Edits *edits,
             UError_ڪوڊ &error_ڪوڊ) {
    uint32_t state = 0;
    لكل (int32_t i = 0; i < src_ڊيگهائي;) {
        int32_t ايندڙ_اشاريو = i;
        UChar32 c;
        U8_ايندڙ(src, ايندڙ_اشاريو, src_ڊيگهائي, c);
        uint32_t ايندڙ_State = 0;
        int32_t type = ucase_getTypeOrIgnorable(c);
        إذا ((type & UCASE_IGNORABLE) != 0) {
            // c is case-ignorable
            ايندڙ_State |= (state & AFTER_CASED);
        } وإلا إذا (type != UCASE_NONE) {
            // c is cased
            ايندڙ_State |= AFTER_CASED;
        }
        uint32_t معلومات = get_Letter_معلومات(c);
        إذا (معلومات > 0) {
            uint32_t upper = معلومات & مٿانهون_نقاب;
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
            int32_t انگ_Ypogegrammeni = 0;  // Map each one to a trailing, spacing, capital iota.
            إذا ((معلومات & HAS_YPOGEGRAMMENI) != 0) {
                انگ_Ypogegrammeni = 1;
            }
            ثابت UBool hasPrecomposedAccent = (معلومات & HAS_ACCENT) != 0;
            // Skip combining diacritics after this Greek letter.
            int32_t ايندڙ_ايندڙ_اشاريو = ايندڙ_اشاريو;
            طالما (ايندڙ_اشاريو < src_ڊيگهائي) {
                UChar32 c2;
                U8_ايندڙ(src, ايندڙ_ايندڙ_اشاريو, src_ڊيگهائي, c2);
                uint32_t diacritic_معلومات = get_Diacritic_معلومات(c2);
                إذا (diacritic_معلومات != 0) {
                    معلومات |= diacritic_معلومات;
                    إذا ((diacritic_معلومات & HAS_YPOGEGRAMMENI) != 0) {
                        ++انگ_Ypogegrammeni;
                    }
                    ايندڙ_اشاريو = ايندڙ_ايندڙ_اشاريو;
                } وإلا {
                    اكسر;  // not a Greek diacritic
                }
            }
            إذا ((معلومات & HAS_VOWEL_AND_ACCENT_AND_DIALYTIKA) == HAS_VOWEL_AND_ACCENT) {
                ايندڙ_State |= hasPrecomposedAccent ? AFTER_VOWEL_WITH_PRECOMPOSED_ACCENT
                                                  : AFTER_VOWEL_WITH_COMBINING_ACCENT;
            }
            // Map according to Greek rules.
            UBool جوڙيو_Tonos = خطأ;
            إذا (upper == 0x397 &&
                    (معلومات & HAS_ACCENT) != 0 &&
                    انگ_Ypogegrammeni == 0 &&
                    (state & AFTER_CASED) == 0 &&
                    !isFollowedByCasedLetter(src, ايندڙ_اشاريو, src_ڊيگهائي)) {
                // Keep disjunctive "or" with (only) a tonos.
                // We use the same "word boundary" conditions as for the Final_Sigma test.
                إذا (hasPrecomposedAccent) {
                    upper = 0x389;  // Preserve the precomposed form.
                } وإلا {
                    جوڙيو_Tonos = سچ;
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
            إذا (edits == خالي_اشارو && (options & U_OMIT_UNCHANGED_متن) == 0) {
                change = سچ;  // common, simple usage
            } وإلا {
                // Find out first whether we are changing the text.
                U_ASSERT(0x370 <= upper && upper <= 0x3ff);  // 2-byte UTF-8, main Greek block
                change = (i + 2) > ايندڙ_اشاريو ||
                        src[i] != getTwoByteLead(upper) || src[i + 1] != getTwoByteTrail(upper) ||
                        انگ_Ypogegrammeni > 0;
                int32_t i2 = i + 2;
                إذا ((معلومات & HAS_EITHER_DIALYTIKA) != 0) {
                    change |= (i2 + 2) > ايندڙ_اشاريو ||
                            src[i2] != static_cast<uint8_t>(u8"\u0308"[0]) ||
                            src[i2 + 1] != static_cast<uint8_t>(u8"\u0308"[1]);
                    i2 += 2;
                }
                إذا (جوڙيو_Tonos) {
                    change |= (i2 + 2) > ايندڙ_اشاريو ||
                            src[i2] != static_cast<uint8_t>(u8"\u0301"[0]) ||
                            src[i2 + 1] != static_cast<uint8_t>(u8"\u0301"[1]);
                    i2 += 2;
                }
                int32_t old_ڊيگهائي = ايندڙ_اشاريو - i;
                int32_t new_ڊيگهائي = (i2 - i) + انگ_Ypogegrammeni * 2;  // 2 bytes per U+0399
                change |= old_ڊيگهائي != new_ڊيگهائي;
                إذا (change) {
                    إذا (edits != خالي_اشارو) {
                        edits->جوڙيو_Replace(old_ڊيگهائي, new_ڊيگهائي);
                    }
                } وإلا {
                    إذا (edits != خالي_اشارو) {
                        edits->جوڙيو_Unchanged(old_ڊيگهائي);
                    }
                    // Write unchanged text?
                    change = (options & U_OMIT_UNCHANGED_متن) == 0;
                }
            }

            إذا (change) {
                ByteSinkUtil::append_Two_بائيٽون(upper, sink);
                إذا ((معلومات & HAS_EITHER_DIALYTIKA) != 0) {
                    sink.AppendU8(u8"\u0308", 2);  // restore or add a dialytika
                }
                إذا (جوڙيو_Tonos) {
                    sink.AppendU8(u8"\u0301", 2);
                }
                طالما (انگ_Ypogegrammeni > 0) {
                    sink.AppendU8(u8"\u0399", 2);
                    --انگ_Ypogegrammeni;
                }
            }
        } وإلا إذا(c>=0) {
            ثابت char16_t *s;
            c=ucase_منزل_ڀريل_مٿانهون(c, خالي_اشارو, خالي_اشارو, &s, UCASE_LOC_GREEK);
            إذا (!append_نتيجو(ايندڙ_اشاريو - i, c, s, sink, options, edits, error_ڪوڊ)) {
                أرجع;
            }
        } وإلا {
            // Malformed UTF-8.
            إذا (!ByteSinkUtil::appendUnchanged(src+i, ايندڙ_اشاريو-i,
                                               sink, options, edits, error_ڪوڊ)) {
                أرجع;
            }
        }
        i = ايندڙ_اشاريو;
        state = ايندڙ_State;
    }
}

}  // namespace GreekUpper
U_NAMESPACE_پڇاڙي

ساكن فراغ U_CALLCONV
ucasemap_اندروني_UTF8_منزل_هيٺاهون(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_اڻ_استعمال_ٿيل
                             ثابت uint8_t *src, int32_t src_ڊيگهائي,
                             icu::ByteSink &sink, icu::Edits *edits,
                             UError_ڪوڊ &error_ڪوڊ) {
    UCaseContext csc=UCASECONTEXT_INITIALIZER;
    csc.p=(فراغ *)src;
    csc.حد_بندي=src_ڊيگهائي;
    منزل_هيٺاهون(
        caseLocale, options,
        src, &csc, 0, src_ڊيگهائي,
        sink, edits, error_ڪوڊ);
}

ساكن فراغ U_CALLCONV
ucasemap_اندروني_UTF8_منزل_مٿانهون(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_اڻ_استعمال_ٿيل
                             ثابت uint8_t *src, int32_t src_ڊيگهائي,
                             icu::ByteSink &sink, icu::Edits *edits,
                             UError_ڪوڊ &error_ڪوڊ) {
    إذا (caseLocale == UCASE_LOC_GREEK) {
        Greek_مٿانهون::منزل_مٿانهون(options, src, src_ڊيگهائي, sink, edits, error_ڪوڊ);
    } وإلا {
        UCaseContext csc=UCASECONTEXT_INITIALIZER;
        csc.p=(فراغ *)src;
        csc.حد_بندي=src_ڊيگهائي;
        منزل_مٿانهون(
            caseLocale, options,
            src, &csc, src_ڊيگهائي,
            sink, edits, error_ڪوڊ);
    }
}

ساكن فراغ U_CALLCONV
ucasemap_اندروني_UTF8_Fold(int32_t /* caseLocale */, uint32_t options, UCASEMAP_BREAK_ITERATOR_اڻ_استعمال_ٿيل
                          ثابت uint8_t *src, int32_t src_ڊيگهائي,
                          icu::ByteSink &sink, icu::Edits *edits,
                          UError_ڪوڊ &error_ڪوڊ) {
    منزل_هيٺاهون(
        -1, options,
        src, خالي_اشارو, 0, src_ڊيگهائي,
        sink, edits, error_ڪوڊ);
}

فراغ
ucasemap_mapUTF8(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_PARAM
                 ثابت حرف *src, int32_t src_ڊيگهائي,
                 UTF8CaseMapper *stringCaseMapper,
                 icu::ByteSink &sink, icu::Edits *edits,
                 UError_ڪوڊ &error_ڪوڊ) {
    /* check argument values */
    إذا (U_FAILURE(error_ڪوڊ)) {
        أرجع;
    }
    إذا ((src == خالي_اشارو && src_ڊيگهائي != 0) || src_ڊيگهائي < -1) {
        error_ڪوڊ = U_ILLEGAL_دليل_ERROR;
        أرجع;
    }

    // Get the string length.
    إذا (src_ڊيگهائي == -1) {
        src_ڊيگهائي = static_cast<int32_t>(uprv_strlen(src));
    }

    إذا (edits != خالي_اشارو && (options & U_EDITS_NO_RESET) == 0) {
        edits->reset();
    }
    stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                     reinterpret_cast<ثابت uint8_t*>(src), src_ڊيگهائي, sink, edits, error_ڪوڊ);
    sink.Flush();
    إذا (U_SUCCESS(error_ڪوڊ)) {
        إذا (edits != خالي_اشارو) {
            edits->copy_Error_منزل(error_ڪوڊ);
        }
    }
}

int32_t
ucasemap_mapUTF8(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_PARAM
                 حرف *dest, int32_t dest_صلاحيت,
                 ثابت حرف *src, int32_t src_ڊيگهائي,
                 UTF8CaseMapper *stringCaseMapper,
                 icu::Edits *edits,
                 UError_ڪوڊ &error_ڪوڊ) {
    /* check argument values */
    إذا(U_FAILURE(error_ڪوڊ)) {
        أرجع 0;
    }
    إذا( dest_صلاحيت<0 ||
        (dest==خالي_اشارو && dest_صلاحيت>0) ||
        (src==خالي_اشارو && src_ڊيگهائي!=0) || src_ڊيگهائي<-1
    ) {
        error_ڪوڊ=U_ILLEGAL_دليل_ERROR;
        أرجع 0;
    }

    /* get the string length */
    إذا(src_ڊيگهائي==-1) {
        src_ڊيگهائي = static_cast<int32_t>(uprv_strlen(src));
    }

    /* check for overlapping source and destination */
    إذا( dest!=خالي_اشارو &&
        ((src>=dest && src<(dest+dest_صلاحيت)) ||
         (dest>=src && dest<(src+src_ڊيگهائي)))
    ) {
        error_ڪوڊ=U_ILLEGAL_دليل_ERROR;
        أرجع 0;
    }

    إذا (edits != خالي_اشارو && (options & U_EDITS_NO_RESET) == 0) {
        edits->reset();
    }
    int32_t reslen = ByteSinkUtil::ذريعي_Byte_Sink_منزل_Terminated_Chars(
        dest, dest_صلاحيت,
        [&](ByteSink& sink, UError_ڪوڊ& حالت) {
            stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                             reinterpret_cast<ثابت uint8_t*>(src), src_ڊيگهائي, sink, edits, حالت);
        },
        error_ڪوڊ);
    إذا (U_SUCCESS(error_ڪوڊ) && edits != خالي_اشارو) {
        edits->copy_Error_منزل(error_ڪوڊ);
    }
    أرجع reslen;
}

/* public API functions */

U_CAPI int32_t U_EXPORT2
ucasemap_utf8_منزل_هيٺاهون(ثابت UCaseMap *csm,
                     حرف *dest, int32_t dest_صلاحيت,
                     ثابت حرف *src, int32_t src_ڊيگهائي,
                     UError_ڪوڊ *p_Error_ڪوڊ) {
    خودڪار *impl = reinterpret_cast<ثابت UCaseMapImpl *>(csm);
    أرجع ucasemap_mapUTF8(
        impl->caseLocale, impl->options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_صلاحيت,
        src, src_ڊيگهائي,
        ucasemap_اندروني_UTF8_منزل_هيٺاهون, خالي_اشارو, *p_Error_ڪوڊ);
}

U_CAPI int32_t U_EXPORT2
ucasemap_utf8_منزل_مٿانهون(ثابت UCaseMap *csm,
                     حرف *dest, int32_t dest_صلاحيت,
                     ثابت حرف *src, int32_t src_ڊيگهائي,
                     UError_ڪوڊ *p_Error_ڪوڊ) {
    خودڪار *impl = reinterpret_cast<ثابت UCaseMapImpl *>(csm);
    أرجع ucasemap_mapUTF8(
        impl->caseLocale, impl->options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_صلاحيت,
        src, src_ڊيگهائي,
        ucasemap_اندروني_UTF8_منزل_مٿانهون, خالي_اشارو, *p_Error_ڪوڊ);
}

U_CAPI int32_t U_EXPORT2
ucasemap_utf8FoldCase(ثابت UCaseMap *csm,
                      حرف *dest, int32_t dest_صلاحيت,
                      ثابت حرف *src, int32_t src_ڊيگهائي,
                      UError_ڪوڊ *p_Error_ڪوڊ) {
    خودڪار *impl = reinterpret_cast<ثابت UCaseMapImpl *>(csm);
    أرجع ucasemap_mapUTF8(
        UCASE_LOC_پاڙ, impl->options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_صلاحيت,
        src, src_ڊيگهائي,
        ucasemap_اندروني_UTF8_Fold, خالي_اشارو, *p_Error_ڪوڊ);
}

U_NAMESPACE_BEGIN

فراغ CaseMap::utf8_منزل_هيٺاهون(
        ثابت حرف *locale, uint32_t options,
        StringPiece src, ByteSink &sink, Edits *edits,
        UError_ڪوڊ &error_ڪوڊ) {
    ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        src.معلومات(), src.length(),
        ucasemap_اندروني_UTF8_منزل_هيٺاهون, sink, edits, error_ڪوڊ);
}

فراغ CaseMap::utf8_منزل_مٿانهون(
        ثابت حرف *locale, uint32_t options,
        StringPiece src, ByteSink &sink, Edits *edits,
        UError_ڪوڊ &error_ڪوڊ) {
    ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        src.معلومات(), src.length(),
        ucasemap_اندروني_UTF8_منزل_مٿانهون, sink, edits, error_ڪوڊ);
}

فراغ CaseMap::utf8Fold(
        uint32_t options,
        StringPiece src, ByteSink &sink, Edits *edits,
        UError_ڪوڊ &error_ڪوڊ) {
    ucasemap_mapUTF8(
        UCASE_LOC_پاڙ, options, UCASEMAP_BREAK_ITERATOR_NULL
        src.معلومات(), src.length(),
        ucasemap_اندروني_UTF8_Fold, sink, edits, error_ڪوڊ);
}

int32_t CaseMap::utf8_منزل_هيٺاهون(
        ثابت حرف *locale, uint32_t options,
        ثابت حرف *src, int32_t src_ڊيگهائي,
        حرف *dest, int32_t dest_صلاحيت, Edits *edits,
        UError_ڪوڊ &error_ڪوڊ) {
    أرجع ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_صلاحيت,
        src, src_ڊيگهائي,
        ucasemap_اندروني_UTF8_منزل_هيٺاهون, edits, error_ڪوڊ);
}

int32_t CaseMap::utf8_منزل_مٿانهون(
        ثابت حرف *locale, uint32_t options,
        ثابت حرف *src, int32_t src_ڊيگهائي,
        حرف *dest, int32_t dest_صلاحيت, Edits *edits,
        UError_ڪوڊ &error_ڪوڊ) {
    أرجع ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_صلاحيت,
        src, src_ڊيگهائي,
        ucasemap_اندروني_UTF8_منزل_مٿانهون, edits, error_ڪوڊ);
}

int32_t CaseMap::utf8Fold(
        uint32_t options,
        ثابت حرف *src, int32_t src_ڊيگهائي,
        حرف *dest, int32_t dest_صلاحيت, Edits *edits,
        UError_ڪوڊ &error_ڪوڊ) {
    أرجع ucasemap_mapUTF8(
        UCASE_LOC_پاڙ, options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_صلاحيت,
        src, src_ڊيگهائي,
        ucasemap_اندروني_UTF8_Fold, edits, error_ڪوڊ);
}

U_NAMESPACE_پڇاڙي
