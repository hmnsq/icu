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

#شامل "unicode/utypes.h"
#شامل "unicode/brkiter.h"
#شامل "unicode/bytestream.h"
#شامل "unicode/casemap.h"
#شامل "unicode/edits.h"
#شامل "unicode/stringoptions.h"
#شامل "unicode/stringpiece.h"
#شامل "unicode/ubrk.h"
#شامل "unicode/uloc.h"
#شامل "unicode/ustring.h"
#شامل "unicode/ucasemap.h"
#اگر !UCONFIG_NO_BREAK_ITERATION
#شامل "unicode/utext.h"
#اگر_ختم
#شامل "unicode/utf.h"
#شامل "unicode/utf8.h"
#شامل "unicode/utf16.h"
#شامل "bytesinkutil.h"
#شامل "cmemory.h"
#شامل "cstring.h"
#شامل "uassert.h"
#شامل "ucase.h"
#شامل "ucasemap_imp.h"

U_NAMESPACE_USE

/* UCaseMap service object -------------------------------------------------- */

// The public type, unicode/ucasemap.h struct UCaseMap, is an opaque C type.
// We define a parallel C++ type in the ICU namespace for entry point renaming.

U_NAMESPACE_BEGIN

UCaseMapImpl::UCaseMapImpl(ثابت حرف *localeID, uint32_t opts, UError_رمز *p_Error_رمز) :
#اگر !UCONFIG_NO_BREAK_ITERATION
        iter(خالی_اشاریہ),
#اگر_ختم
        caseLocale(UCASE_LOC_UNKNOWN), options(opts) {
    ucasemap_سیٹ_Locale(reinterpret_cast<UCaseMap *>(یہ), localeID, p_Error_رمز);
}

UCaseMapImpl::~UCaseMapImpl() {
#اگر !UCONFIG_NO_BREAK_ITERATION
    حذف iter;
#اگر_ختم
}

U_NAMESPACE_انتہا

U_CAPI UCaseMap * U_EXPORT2
ucasemap_open(ثابت حرف *locale, uint32_t options, UError_رمز *p_Error_رمز) {
    اگر(U_FAILURE(*p_Error_رمز)) {
        واپس خالی_اشاریہ;
    }
    UCaseMapImpl *csm = نیا UCaseMapImpl(locale, options, p_Error_رمز);
    اگر(csm==خالی_اشاریہ) {
        *p_Error_رمز = U_حافظہ_تخصیص_ERROR;
        واپس خالی_اشاریہ;
    } ورنہ اگر (U_FAILURE(*p_Error_رمز)) {
        حذف csm;
        واپس خالی_اشاریہ;
    }
    واپس reinterpret_cast<UCaseMap *>(csm);
}

U_CAPI خالی U_EXPORT2
ucasemap_close(UCaseMap *csm) {
    حذف reinterpret_cast<UCaseMapImpl *>(csm);
}

U_CAPI ثابت حرف* U_EXPORT2
ucasemap_getLocale(ثابت UCaseMap* csm U_LIFETIME_BOUND) {
    واپس reinterpret_cast<ثابت UCaseMapImpl *>(csm)->locale;
}

U_CAPI uint32_t U_EXPORT2
ucasemap_getOptions(ثابت UCaseMap *csm) {
    واپس reinterpret_cast<ثابت UCaseMapImpl *>(csm)->options;
}

U_CAPI خالی U_EXPORT2
ucasemap_سیٹ_Locale(UCaseMap *csm, ثابت حرف *locale, UError_رمز *p_Error_رمز) {
    اگر(U_FAILURE(*p_Error_رمز)) {
        واپس;
    }
    خودکار *impl = reinterpret_cast<UCaseMapImpl *>(csm);
    اگر (locale != خالی_اشاریہ && *locale == 0) {
        impl->locale[0] = 0;
        impl->caseLocale = UCASE_LOC_جڑ;
        واپس;
    }

    UError_رمز بفر_حالت = U_ZERO_ERROR;
    int32_t length=uloc_getName(locale, impl->locale, (int32_t)حجم(impl->locale), &بفر_حالت);
    اگر(بفر_حالت==U_بفر_حد_سے_تجاوز_ERROR || (U_SUCCESS(بفر_حالت) && length==حجم(impl->locale))) {
        بفر_حالت = U_ZERO_ERROR;
        /* we only really need the language code for case mappings */
        length=uloc_getLanguage(locale, impl->locale, (int32_t)حجم(impl->locale), &بفر_حالت);
    }
    اگر(U_FAILURE(بفر_حالت)) {
        *p_Error_رمز=بفر_حالت;
    } ورنہ اگر(length==حجم(impl->locale)) {
        *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
    }
    اگر(U_SUCCESS(*p_Error_رمز)) {     
        impl->caseLocale = ucase_getCaseLocale(impl->locale);
    } ورنہ {
        impl->locale[0]=0;
        impl->caseLocale = UCASE_LOC_جڑ;
    }
}

U_CAPI خالی U_EXPORT2
ucasemap_سیٹ_Options(UCaseMap *csm, uint32_t options, UError_رمز *p_Error_رمز) {
    اگر(U_FAILURE(*p_Error_رمز)) {
        واپس;
    }
    reinterpret_cast<UCaseMapImpl *>(csm)->options=options;
}

/* UTF-8 string case mappings ----------------------------------------------- */

/* TODO(markus): Move to a new, separate utf8case.cpp file. */

نامخلا {

/* append a full case mapping result, see UCASE_MAX_STRING_LENGTH */
اندرونی UBool
append_نتیجہ(int32_t cp_لمبائی, int32_t نتیجہ, ثابت char16_t *s,
             ByteSink &sink, uint32_t options, icu::Edits *edits, UError_رمز &error_رمز) {
    U_ASSERT(U_SUCCESS(error_رمز));

    /* decode the result */
    اگر(نتیجہ<0) {
        /* (not) original code point */
        اگر(edits!=خالی_اشاریہ) {
            edits->جوڑو_Unchanged(cp_لمبائی);
        }
        اگر((options & U_OMIT_UNCHANGED_متن) == 0) {
            ByteSinkUtil::append_رمز_نقطہ(cp_لمبائی, ~نتیجہ, sink);
        }
    } ورنہ {
        اگر(نتیجہ<=UCASE_MAX_STRING_لمبائی) {
            // string: "result" is the UTF-16 length
            واپس ByteSinkUtil::appendChange(cp_لمبائی, s, نتیجہ, sink, edits, error_رمز);
        } ورنہ {
            ByteSinkUtil::append_رمز_نقطہ(cp_لمبائی, نتیجہ, sink, edits);
        }
    }
    واپس سچ;
}

// See unicode/utf8.h U8_APPEND_UNSAFE().
اندرونی uint8_t getTwoByteLead(UChar32 c) { واپس static_cast<uint8_t>((c >> 6) | 0xc0); }
اندرونی uint8_t getTwoByteTrail(UChar32 c) { واپس static_cast<uint8_t>((c & 0x3f) | 0x80); }

UChar32 U_CALLCONV
utf8_caseContextIterator(خالی *context, int8_t dir) {
    UCaseContext* csc = static_cast<UCaseContext*>(context);
    UChar32 c;

    اگر(dir<0) {
        /* reset for backward iteration */
        csc->اشاریہ=csc->cp_آغاز;
        csc->dir=dir;
    } ورنہ اگر(dir>0) {
        /* reset for forward iteration */
        csc->اشاریہ=csc->cp_حد_بندی;
        csc->dir=dir;
    } ورنہ {
        /* continue current iteration direction */
        dir=csc->dir;
    }

    اگر(dir<0) {
        اگر(csc->آغاز<csc->اشاریہ) {
            U8_پچھلا((ثابت uint8_t *)csc->p, csc->آغاز, csc->اشاریہ, c);
            واپس c;
        }
    } ورنہ {
        اگر(csc->اشاریہ<csc->حد_بندی) {
            U8_اگلا((ثابت uint8_t *)csc->p, csc->اشاریہ, csc->حد_بندی, c);
            واپس c;
        }
    }
    واپس U_SENTINEL;
}

/**
 * caseLocale >= 0: Lowercases [srcStart..srcLimit[ but takes context [0..srcLength[ into account.
 * caseLocale < 0: Case-folds [srcStart..srcLimit[.
 */
خالی مقصد_زیریں(int32_t caseLocale, uint32_t options,
             ثابت uint8_t *src, UCaseContext *csc, int32_t src_آغاز, int32_t src_حد_بندی,
             icu::ByteSink &sink, icu::Edits *edits, UError_رمز &error_رمز) {
    ثابت int8_t *latin_مقصد_زیریں;
    اگر (caseLocale == UCASE_LOC_جڑ ||
            (caseLocale >= 0 ?
                !(caseLocale == UCASE_LOC_TURKISH || caseLocale == UCASE_LOC_LITHUANIAN) :
                (options & FOLD_CASE_OPTIONS_نقاب) == U_FOLD_CASE_DEFAULT)) {
        latin_مقصد_زیریں = LatinCase::مقصد_زیریں_معمول;
    } ورنہ {
        latin_مقصد_زیریں = LatinCase::مقصد_زیریں_TR_LT;
    }
    ثابت UTrie2 *trie = ucase_getTrie();
    int32_t پچھلا = src_آغاز;
    int32_t src_اشاریہ = src_آغاز;
    برائے (;;) {
        // fast path for simple cases
        int32_t cp_آغاز;
        UChar32 c;
        برائے (;;) {
            اگر (U_FAILURE(error_رمز) || src_اشاریہ >= src_حد_بندی) {
                c = U_SENTINEL;
                توڑو;
            }
            uint8_t lead = src[src_اشاریہ++];
            اگر (lead <= 0x7f) {
                int8_t d = latin_مقصد_زیریں[lead];
                اگر (d == LatinCase::EXC) {
                    cp_آغاز = src_اشاریہ - 1;
                    c = lead;
                    توڑو;
                }
                اگر (d == 0) { جاری; }
                ByteSinkUtil::appendUnchanged(src + پچھلا, src_اشاریہ - 1 - پچھلا,
                                              sink, options, edits, error_رمز);
                حرف ascii = static_cast<حرف>(lead + d);
                sink.Append(&ascii, 1);
                اگر (edits != خالی_اشاریہ) {
                    edits->جوڑو_Replace(1, 1);
                }
                پچھلا = src_اشاریہ;
                جاری;
            } ورنہ اگر (lead < 0xe3) {
                uint8_t t;
                اگر (0xc2 <= lead && lead <= 0xc5 && src_اشاریہ < src_حد_بندی &&
                        (t = src[src_اشاریہ] - 0x80) <= 0x3f) {
                    // U+0080..U+017F
                    ++src_اشاریہ;
                    c = ((lead - 0xc0) << 6) | t;
                    int8_t d = latin_مقصد_زیریں[c];
                    اگر (d == LatinCase::EXC) {
                        cp_آغاز = src_اشاریہ - 2;
                        توڑو;
                    }
                    اگر (d == 0) { جاری; }
                    ByteSinkUtil::appendUnchanged(src + پچھلا, src_اشاریہ - 2 - پچھلا,
                                                  sink, options, edits, error_رمز);
                    ByteSinkUtil::append_Two_بائٹیں(c + d, sink);
                    اگر (edits != خالی_اشاریہ) {
                        edits->جوڑو_Replace(2, 2);
                    }
                    پچھلا = src_اشاریہ;
                    جاری;
                }
            } ورنہ اگر ((lead <= 0xe9 || lead == 0xeb || lead == 0xec) &&
                    (src_اشاریہ + 2) <= src_حد_بندی &&
                    U8_IS_TRAIL(src[src_اشاریہ]) && U8_IS_TRAIL(src[src_اشاریہ + 1])) {
                // most of CJK: no case mappings
                src_اشاریہ += 2;
                جاری;
            }
            cp_آغاز = --src_اشاریہ;
            U8_اگلا(src, src_اشاریہ, src_حد_بندی, c);
            اگر (c < 0) {
                // ill-formed UTF-8
                جاری;
            }
            uint16_t props = UTRIE2_GET16(trie, c);
            اگر (UCASE_HAS_EXCEPTION(props)) { توڑو; }
            int32_t delta;
            اگر (!UCASE_IS_بالائی_OR_TITLE(props) || (delta = UCASE_GET_DELTA(props)) == 0) {
                جاری;
            }
            ByteSinkUtil::appendUnchanged(src + پچھلا, cp_آغاز - پچھلا,
                                          sink, options, edits, error_رمز);
            ByteSinkUtil::append_رمز_نقطہ(src_اشاریہ - cp_آغاز, c + delta, sink, edits);
            پچھلا = src_اشاریہ;
        }
        اگر (c < 0) {
            توڑو;
        }
        // slow path
        ثابت char16_t *s;
        اگر (caseLocale >= 0) {
            csc->cp_آغاز = cp_آغاز;
            csc->cp_حد_بندی = src_اشاریہ;
            c = ucase_مقصد_بھرا_زیریں(c, utf8_caseContextIterator, csc, &s, caseLocale);
        } ورنہ {
            c = ucase_مقصد_بھرا_Folding(c, &s, options);
        }
        اگر (c >= 0) {
            ByteSinkUtil::appendUnchanged(src + پچھلا, cp_آغاز - پچھلا,
                                          sink, options, edits, error_رمز);
            append_نتیجہ(src_اشاریہ - cp_آغاز, c, s, sink, options, edits, error_رمز);
            پچھلا = src_اشاریہ;
        }
    }
    ByteSinkUtil::appendUnchanged(src + پچھلا, src_اشاریہ - پچھلا,
                                  sink, options, edits, error_رمز);
}

خالی مقصد_بالائی(int32_t caseLocale, uint32_t options,
             ثابت uint8_t *src, UCaseContext *csc, int32_t src_لمبائی,
             icu::ByteSink &sink, icu::Edits *edits, UError_رمز &error_رمز) {
    ثابت int8_t *latin_مقصد_بالائی;
    اگر (caseLocale == UCASE_LOC_TURKISH) {
        latin_مقصد_بالائی = LatinCase::مقصد_بالائی_TR;
    } ورنہ {
        latin_مقصد_بالائی = LatinCase::مقصد_بالائی_معمول;
    }
    ثابت UTrie2 *trie = ucase_getTrie();
    int32_t پچھلا = 0;
    int32_t src_اشاریہ = 0;
    برائے (;;) {
        // fast path for simple cases
        int32_t cp_آغاز;
        UChar32 c;
        برائے (;;) {
            اگر (U_FAILURE(error_رمز) || src_اشاریہ >= src_لمبائی) {
                c = U_SENTINEL;
                توڑو;
            }
            uint8_t lead = src[src_اشاریہ++];
            اگر (lead <= 0x7f) {
                int8_t d = latin_مقصد_بالائی[lead];
                اگر (d == LatinCase::EXC) {
                    cp_آغاز = src_اشاریہ - 1;
                    c = lead;
                    توڑو;
                }
                اگر (d == 0) { جاری; }
                ByteSinkUtil::appendUnchanged(src + پچھلا, src_اشاریہ - 1 - پچھلا,
                                              sink, options, edits, error_رمز);
                حرف ascii = static_cast<حرف>(lead + d);
                sink.Append(&ascii, 1);
                اگر (edits != خالی_اشاریہ) {
                    edits->جوڑو_Replace(1, 1);
                }
                پچھلا = src_اشاریہ;
                جاری;
            } ورنہ اگر (lead < 0xe3) {
                uint8_t t;
                اگر (0xc2 <= lead && lead <= 0xc5 && src_اشاریہ < src_لمبائی &&
                        (t = src[src_اشاریہ] - 0x80) <= 0x3f) {
                    // U+0080..U+017F
                    ++src_اشاریہ;
                    c = ((lead - 0xc0) << 6) | t;
                    int8_t d = latin_مقصد_بالائی[c];
                    اگر (d == LatinCase::EXC) {
                        cp_آغاز = src_اشاریہ - 2;
                        توڑو;
                    }
                    اگر (d == 0) { جاری; }
                    ByteSinkUtil::appendUnchanged(src + پچھلا, src_اشاریہ - 2 - پچھلا,
                                                  sink, options, edits, error_رمز);
                    ByteSinkUtil::append_Two_بائٹیں(c + d, sink);
                    اگر (edits != خالی_اشاریہ) {
                        edits->جوڑو_Replace(2, 2);
                    }
                    پچھلا = src_اشاریہ;
                    جاری;
                }
            } ورنہ اگر ((lead <= 0xe9 || lead == 0xeb || lead == 0xec) &&
                    (src_اشاریہ + 2) <= src_لمبائی &&
                    U8_IS_TRAIL(src[src_اشاریہ]) && U8_IS_TRAIL(src[src_اشاریہ + 1])) {
                // most of CJK: no case mappings
                src_اشاریہ += 2;
                جاری;
            }
            cp_آغاز = --src_اشاریہ;
            U8_اگلا(src, src_اشاریہ, src_لمبائی, c);
            اگر (c < 0) {
                // ill-formed UTF-8
                جاری;
            }
            uint16_t props = UTRIE2_GET16(trie, c);
            اگر (UCASE_HAS_EXCEPTION(props)) { توڑو; }
            int32_t delta;
            اگر (UCASE_GET_TYPE(props) != UCASE_زیریں || (delta = UCASE_GET_DELTA(props)) == 0) {
                جاری;
            }
            ByteSinkUtil::appendUnchanged(src + پچھلا, cp_آغاز - پچھلا,
                                          sink, options, edits, error_رمز);
            ByteSinkUtil::append_رمز_نقطہ(src_اشاریہ - cp_آغاز, c + delta, sink, edits);
            پچھلا = src_اشاریہ;
        }
        اگر (c < 0) {
            توڑو;
        }
        // slow path
        csc->cp_آغاز = cp_آغاز;
        csc->cp_حد_بندی = src_اشاریہ;
        ثابت char16_t *s;
        c = ucase_مقصد_بھرا_بالائی(c, utf8_caseContextIterator, csc, &s, caseLocale);
        اگر (c >= 0) {
            ByteSinkUtil::appendUnchanged(src + پچھلا, cp_آغاز - پچھلا,
                                          sink, options, edits, error_رمز);
            append_نتیجہ(src_اشاریہ - cp_آغاز, c, s, sink, options, edits, error_رمز);
            پچھلا = src_اشاریہ;
        }
    }
    ByteSinkUtil::appendUnchanged(src + پچھلا, src_اشاریہ - پچھلا,
                                  sink, options, edits, error_رمز);
}

}  // namespace

#اگر !UCONFIG_NO_BREAK_ITERATION

نامخلا {

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
int32_t maybeTitleDutchIJ(ثابت uint8_t *src, UChar32 c, int32_t آغاز, int32_t segment_حد_بندی,
                          ByteSink &sink, uint32_t options, icu::Edits *edits, UError_رمز &error_رمز) {
    U_ASSERT(آغاز < segment_حد_بندی);

    int32_t اشاریہ = آغاز;
    منطقی withAcute = جھوٹ;

    // If the conditions are met, then the following variables tell us what to output.
    int32_t unchanged1 = 0;  // code units before the j, or the whole sequence (0..3)
    منطقی doTitleJ = جھوٹ;  // true if the j needs to be titlecased
    int32_t unchanged2 = 0;  // after the j (0 or 1)

    // next character after the first letter
    UChar32 c2;
    c2 = src[اشاریہ++];

    // Is the first letter an i/I with accent?
    اگر (c == u'I') {
        اگر (c2 == ACUTE_BYTE0 && اشاریہ < segment_حد_بندی && src[اشاریہ++] == ACUTE_BYTE1) {
            withAcute = سچ;
            unchanged1 = 2;  // ACUTE is 2 code units in UTF-8
            اگر (اشاریہ == segment_حد_بندی) { واپس آغاز; }
            c2 = src[اشاریہ++];
        }
    } ورنہ {  // Í
        withAcute = سچ;
    }

    // Is the next character a j/J?
    اگر (c2 == u'j') {
        doTitleJ = سچ;
    } ورنہ اگر (c2 == u'J') {
        ++unchanged1;
    } ورنہ {
        واپس آغاز;
    }

    // A plain i/I must be followed by a plain j/J.
    // An i/I with acute must be followed by a j/J with acute.
    اگر (withAcute) {
        اگر ((اشاریہ + 1) >= segment_حد_بندی || src[اشاریہ++] != ACUTE_BYTE0 || src[اشاریہ++] != ACUTE_BYTE1) {
            واپس آغاز;
        }
        اگر (doTitleJ) {
            unchanged2 = 2;  // ACUTE is 2 code units in UTF-8
        } ورنہ {
            unchanged1 = unchanged1 + 2;    // ACUTE is 2 code units in UTF-8
        }
    }

    // There must not be another combining mark.
    اگر (اشاریہ < segment_حد_بندی) {
        int32_t cp;
        int32_t i = اشاریہ;
        U8_اگلا(src, i, segment_حد_بندی, cp);
        uint32_t type_نقاب = U_GET_GC_نقاب(cp);
        اگر ((type_نقاب & U_GC_M_نقاب) != 0) {
            واپس آغاز;
        }
    }

    // Output the rest of the Dutch IJ.
    ByteSinkUtil::appendUnchanged(src + آغاز, unchanged1, sink, options, edits, error_رمز);
    آغاز += unchanged1;
    اگر (doTitleJ) {
        ByteSinkUtil::append_رمز_نقطہ(1, u'J', sink, edits);
        ++آغاز;
    }
    ByteSinkUtil::appendUnchanged(src + آغاز, unchanged2, sink, options, edits, error_رمز);

    U_ASSERT(آغاز + unchanged2 == اشاریہ);
    واپس اشاریہ;
}

}  // namespace

U_CFUNC خالی U_CALLCONV
ucasemap_اندرونی_UTF8_مقصد_Title(
        int32_t caseLocale, uint32_t options, BreakIterator *iter,
        ثابت uint8_t *src, int32_t src_لمبائی,
        ByteSink &sink, icu::Edits *edits,
        UError_رمز &error_رمز) {
    اگر (!ustrcase_checkTitleAdjustmentOptions(options, error_رمز)) {
        واپس;
    }

    /* set up local variables */
    UCaseContext csc=UCASECONTEXT_INITIALIZER;
    csc.p=(خالی *)src;
    csc.حد_بندی=src_لمبائی;
    int32_t پچھلا=0;
    UBool is_پہلا_اشاریہ=سچ;

    /* titlecasing loop */
    جب(پچھلا<src_لمبائی) {
        /* find next index where to titlecase */
        int32_t اشاریہ;
        اگر(is_پہلا_اشاریہ) {
            is_پہلا_اشاریہ=جھوٹ;
            اشاریہ=iter->first();
        } ورنہ {
            اشاریہ=iter->next();
        }
        اگر(اشاریہ==UBRK_مکمل || اشاریہ>src_لمبائی) {
            اشاریہ=src_لمبائی;
        }

        /*
         * Segment [prev..index[ into 3 parts:
         * a) skipped characters (copy as-is) [prev..titleStart[
         * b) first letter (titlecase)              [titleStart..titleLimit[
         * c) subsequent characters (lowercase)                 [titleLimit..index[
         */
        اگر(پچھلا<اشاریہ) {
            /* find and copy skipped characters [prev..titleStart[ */
            int32_t title_آغاز=پچھلا;
            int32_t title_حد_بندی=پچھلا;
            UChar32 c;
            U8_اگلا(src, title_حد_بندی, اشاریہ, c);
            اگر ((options&U_TITLECASE_NO_BREAK_ADJUSTMENT)==0) {
                // Adjust the titlecasing index to the next cased character,
                // or to the next letter/number/symbol/private use.
                // Stop with titleStart<titleLimit<=index
                // if there is a character to be titlecased,
                // or else stop with titleStart==titleLimit==index.
                UBool مقصد_Cased = (options&U_TITLECASE_ADJUST_مقصد_CASED) != 0;
                جب (مقصد_Cased ? UCASE_NONE==ucase_getType(c) : !ustrcase_isLNS(c)) {
                    title_آغاز=title_حد_بندی;
                    اگر(title_حد_بندی==اشاریہ) {
                        توڑو;
                    }
                    U8_اگلا(src, title_حد_بندی, اشاریہ, c);
                }
                اگر (پچھلا < title_آغاز) {
                    اگر (!ByteSinkUtil::appendUnchanged(src+پچھلا, title_آغاز-پچھلا,
                                                       sink, options, edits, error_رمز)) {
                        واپس;
                    }
                }
            }

            اگر(title_آغاز<title_حد_بندی) {
                /* titlecase c which is from [titleStart..titleLimit[ */
                اگر(c>=0) {
                    csc.cp_آغاز=title_آغاز;
                    csc.cp_حد_بندی=title_حد_بندی;
                    ثابت char16_t *s;
                    c=ucase_مقصد_بھرا_Title(c, utf8_caseContextIterator, &csc, &s, caseLocale);
                    اگر (!append_نتیجہ(title_حد_بندی-title_آغاز, c, s, sink, options, edits, error_رمز)) {
                        واپس;
                    }
                } ورنہ {
                    // Malformed UTF-8.
                    اگر (!ByteSinkUtil::appendUnchanged(src+title_آغاز, title_حد_بندی-title_آغاز,
                                                       sink, options, edits, error_رمز)) {
                        واپس;
                    }
                }

                /* Special case Dutch IJ titlecasing */
                اگر (title_حد_بندی < اشاریہ &&
                    caseLocale == UCASE_LOC_DUTCH) {
                    اگر (c < 0) {
                        c = ~c;
                    }

                    اگر (c == u'I' || c == u'Í') {
                        title_حد_بندی = maybeTitleDutchIJ(src, c, title_حد_بندی, اشاریہ, sink, options, edits, error_رمز);
                    }
                }

                /* lowercase [titleLimit..index[ */
                اگر(title_حد_بندی<اشاریہ) {
                    اگر((options&U_TITLECASE_NO_LOWERCASE)==0) {
                        /* Normal operation: Lowercase the rest of the word. */
                        مقصد_زیریں(caseLocale, options,
                                src, &csc, title_حد_بندی, اشاریہ,
                                sink, edits, error_رمز);
                        اگر(U_FAILURE(error_رمز)) {
                            واپس;
                        }
                    } ورنہ {
                        /* Optionally just copy the rest of the word unchanged. */
                        اگر (!ByteSinkUtil::appendUnchanged(src+title_حد_بندی, اشاریہ-title_حد_بندی,
                                                           sink, options, edits, error_رمز)) {
                            واپس;
                        }
                    }
                }
            }
        }

        پچھلا=اشاریہ;
    }
}

#اگر_ختم

U_NAMESPACE_BEGIN
نامخلا Greek_بالائی {

UBool isFollowedByCasedLetter(ثابت uint8_t *s, int32_t i, int32_t length) {
    جب (i < length) {
        UChar32 c;
        U8_اگلا(s, i, length, c);
        int32_t type = ucase_getTypeOrIgnorable(c);
        اگر ((type & UCASE_IGNORABLE) != 0) {
            // Case-ignorable, continue with the loop.
        } ورنہ اگر (type != UCASE_NONE) {
            واپس سچ;  // Followed by cased letter.
        } ورنہ {
            واپس جھوٹ;  // Uncased and not case-ignorable.
        }
    }
    واپس جھوٹ;  // Not followed by cased letter.
}

// Keep this consistent with the UTF-16 version in ustrcase.cpp and the Java version in CaseMap.java.
خالی مقصد_بالائی(uint32_t options,
             ثابت uint8_t *src, int32_t src_لمبائی,
             ByteSink &sink, Edits *edits,
             UError_رمز &error_رمز) {
    uint32_t state = 0;
    برائے (int32_t i = 0; i < src_لمبائی;) {
        int32_t اگلا_اشاریہ = i;
        UChar32 c;
        U8_اگلا(src, اگلا_اشاریہ, src_لمبائی, c);
        uint32_t اگلا_State = 0;
        int32_t type = ucase_getTypeOrIgnorable(c);
        اگر ((type & UCASE_IGNORABLE) != 0) {
            // c is case-ignorable
            اگلا_State |= (state & AFTER_CASED);
        } ورنہ اگر (type != UCASE_NONE) {
            // c is cased
            اگلا_State |= AFTER_CASED;
        }
        uint32_t ڈیٹا = get_Letter_ڈیٹا(c);
        اگر (ڈیٹا > 0) {
            uint32_t upper = ڈیٹا & بالائی_نقاب;
            // Add a dialytika to this iota or ypsilon vowel
            // if we removed a tonos from the previous vowel,
            // and that previous vowel did not also have (or gain) a dialytika.
            // Adding one only to the final vowel in a longer sequence
            // (which does not occur in normal writing) would require lookahead.
            // Set the same flag as for preserving an existing dialytika.
            اگر ((ڈیٹا & HAS_VOWEL) != 0 &&
                (state & (AFTER_VOWEL_WITH_PRECOMPOSED_ACCENT | AFTER_VOWEL_WITH_COMBINING_ACCENT)) !=
                    0 &&
                (upper == 0x399 || upper == 0x3A5)) {
                ڈیٹا |= (state & AFTER_VOWEL_WITH_PRECOMPOSED_ACCENT) != 0 ? HAS_DIALYTIKA
                                                                           : HAS_COMBINING_DIALYTIKA;
            }
            int32_t عدد_Ypogegrammeni = 0;  // Map each one to a trailing, spacing, capital iota.
            اگر ((ڈیٹا & HAS_YPOGEGRAMMENI) != 0) {
                عدد_Ypogegrammeni = 1;
            }
            ثابت UBool hasPrecomposedAccent = (ڈیٹا & HAS_ACCENT) != 0;
            // Skip combining diacritics after this Greek letter.
            int32_t اگلا_اگلا_اشاریہ = اگلا_اشاریہ;
            جب (اگلا_اشاریہ < src_لمبائی) {
                UChar32 c2;
                U8_اگلا(src, اگلا_اگلا_اشاریہ, src_لمبائی, c2);
                uint32_t diacritic_ڈیٹا = get_Diacritic_ڈیٹا(c2);
                اگر (diacritic_ڈیٹا != 0) {
                    ڈیٹا |= diacritic_ڈیٹا;
                    اگر ((diacritic_ڈیٹا & HAS_YPOGEGRAMMENI) != 0) {
                        ++عدد_Ypogegrammeni;
                    }
                    اگلا_اشاریہ = اگلا_اگلا_اشاریہ;
                } ورنہ {
                    توڑو;  // not a Greek diacritic
                }
            }
            اگر ((ڈیٹا & HAS_VOWEL_AND_ACCENT_AND_DIALYTIKA) == HAS_VOWEL_AND_ACCENT) {
                اگلا_State |= hasPrecomposedAccent ? AFTER_VOWEL_WITH_PRECOMPOSED_ACCENT
                                                  : AFTER_VOWEL_WITH_COMBINING_ACCENT;
            }
            // Map according to Greek rules.
            UBool جوڑو_Tonos = جھوٹ;
            اگر (upper == 0x397 &&
                    (ڈیٹا & HAS_ACCENT) != 0 &&
                    عدد_Ypogegrammeni == 0 &&
                    (state & AFTER_CASED) == 0 &&
                    !isFollowedByCasedLetter(src, اگلا_اشاریہ, src_لمبائی)) {
                // Keep disjunctive "or" with (only) a tonos.
                // We use the same "word boundary" conditions as for the Final_Sigma test.
                اگر (hasPrecomposedAccent) {
                    upper = 0x389;  // Preserve the precomposed form.
                } ورنہ {
                    جوڑو_Tonos = سچ;
                }
            } ورنہ اگر ((ڈیٹا & HAS_DIALYTIKA) != 0) {
                // Preserve a vowel with dialytika in precomposed form if it exists.
                اگر (upper == 0x399) {
                    upper = 0x3AA;
                    ڈیٹا &= ~HAS_EITHER_DIALYTIKA;
                } ورنہ اگر (upper == 0x3A5) {
                    upper = 0x3AB;
                    ڈیٹا &= ~HAS_EITHER_DIALYTIKA;
                }
            }

            UBool change;
            اگر (edits == خالی_اشاریہ && (options & U_OMIT_UNCHANGED_متن) == 0) {
                change = سچ;  // common, simple usage
            } ورنہ {
                // Find out first whether we are changing the text.
                U_ASSERT(0x370 <= upper && upper <= 0x3ff);  // 2-byte UTF-8, main Greek block
                change = (i + 2) > اگلا_اشاریہ ||
                        src[i] != getTwoByteLead(upper) || src[i + 1] != getTwoByteTrail(upper) ||
                        عدد_Ypogegrammeni > 0;
                int32_t i2 = i + 2;
                اگر ((ڈیٹا & HAS_EITHER_DIALYTIKA) != 0) {
                    change |= (i2 + 2) > اگلا_اشاریہ ||
                            src[i2] != static_cast<uint8_t>(u8"\u0308"[0]) ||
                            src[i2 + 1] != static_cast<uint8_t>(u8"\u0308"[1]);
                    i2 += 2;
                }
                اگر (جوڑو_Tonos) {
                    change |= (i2 + 2) > اگلا_اشاریہ ||
                            src[i2] != static_cast<uint8_t>(u8"\u0301"[0]) ||
                            src[i2 + 1] != static_cast<uint8_t>(u8"\u0301"[1]);
                    i2 += 2;
                }
                int32_t old_لمبائی = اگلا_اشاریہ - i;
                int32_t new_لمبائی = (i2 - i) + عدد_Ypogegrammeni * 2;  // 2 bytes per U+0399
                change |= old_لمبائی != new_لمبائی;
                اگر (change) {
                    اگر (edits != خالی_اشاریہ) {
                        edits->جوڑو_Replace(old_لمبائی, new_لمبائی);
                    }
                } ورنہ {
                    اگر (edits != خالی_اشاریہ) {
                        edits->جوڑو_Unchanged(old_لمبائی);
                    }
                    // Write unchanged text?
                    change = (options & U_OMIT_UNCHANGED_متن) == 0;
                }
            }

            اگر (change) {
                ByteSinkUtil::append_Two_بائٹیں(upper, sink);
                اگر ((ڈیٹا & HAS_EITHER_DIALYTIKA) != 0) {
                    sink.AppendU8(u8"\u0308", 2);  // restore or add a dialytika
                }
                اگر (جوڑو_Tonos) {
                    sink.AppendU8(u8"\u0301", 2);
                }
                جب (عدد_Ypogegrammeni > 0) {
                    sink.AppendU8(u8"\u0399", 2);
                    --عدد_Ypogegrammeni;
                }
            }
        } ورنہ اگر(c>=0) {
            ثابت char16_t *s;
            c=ucase_مقصد_بھرا_بالائی(c, خالی_اشاریہ, خالی_اشاریہ, &s, UCASE_LOC_GREEK);
            اگر (!append_نتیجہ(اگلا_اشاریہ - i, c, s, sink, options, edits, error_رمز)) {
                واپس;
            }
        } ورنہ {
            // Malformed UTF-8.
            اگر (!ByteSinkUtil::appendUnchanged(src+i, اگلا_اشاریہ-i,
                                               sink, options, edits, error_رمز)) {
                واپس;
            }
        }
        i = اگلا_اشاریہ;
        state = اگلا_State;
    }
}

}  // namespace GreekUpper
U_NAMESPACE_انتہا

ساکن خالی U_CALLCONV
ucasemap_اندرونی_UTF8_مقصد_زیریں(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_غیر_مستعمل
                             ثابت uint8_t *src, int32_t src_لمبائی,
                             icu::ByteSink &sink, icu::Edits *edits,
                             UError_رمز &error_رمز) {
    UCaseContext csc=UCASECONTEXT_INITIALIZER;
    csc.p=(خالی *)src;
    csc.حد_بندی=src_لمبائی;
    مقصد_زیریں(
        caseLocale, options,
        src, &csc, 0, src_لمبائی,
        sink, edits, error_رمز);
}

ساکن خالی U_CALLCONV
ucasemap_اندرونی_UTF8_مقصد_بالائی(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_غیر_مستعمل
                             ثابت uint8_t *src, int32_t src_لمبائی,
                             icu::ByteSink &sink, icu::Edits *edits,
                             UError_رمز &error_رمز) {
    اگر (caseLocale == UCASE_LOC_GREEK) {
        Greek_بالائی::مقصد_بالائی(options, src, src_لمبائی, sink, edits, error_رمز);
    } ورنہ {
        UCaseContext csc=UCASECONTEXT_INITIALIZER;
        csc.p=(خالی *)src;
        csc.حد_بندی=src_لمبائی;
        مقصد_بالائی(
            caseLocale, options,
            src, &csc, src_لمبائی,
            sink, edits, error_رمز);
    }
}

ساکن خالی U_CALLCONV
ucasemap_اندرونی_UTF8_Fold(int32_t /* caseLocale */, uint32_t options, UCASEMAP_BREAK_ITERATOR_غیر_مستعمل
                          ثابت uint8_t *src, int32_t src_لمبائی,
                          icu::ByteSink &sink, icu::Edits *edits,
                          UError_رمز &error_رمز) {
    مقصد_زیریں(
        -1, options,
        src, خالی_اشاریہ, 0, src_لمبائی,
        sink, edits, error_رمز);
}

خالی
ucasemap_mapUTF8(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_PARAM
                 ثابت حرف *src, int32_t src_لمبائی,
                 UTF8CaseMapper *stringCaseMapper,
                 icu::ByteSink &sink, icu::Edits *edits,
                 UError_رمز &error_رمز) {
    /* check argument values */
    اگر (U_FAILURE(error_رمز)) {
        واپس;
    }
    اگر ((src == خالی_اشاریہ && src_لمبائی != 0) || src_لمبائی < -1) {
        error_رمز = U_ILLEGAL_دلیل_ERROR;
        واپس;
    }

    // Get the string length.
    اگر (src_لمبائی == -1) {
        src_لمبائی = static_cast<int32_t>(uprv_strlen(src));
    }

    اگر (edits != خالی_اشاریہ && (options & U_EDITS_NO_RESET) == 0) {
        edits->reset();
    }
    stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                     reinterpret_cast<ثابت uint8_t*>(src), src_لمبائی, sink, edits, error_رمز);
    sink.Flush();
    اگر (U_SUCCESS(error_رمز)) {
        اگر (edits != خالی_اشاریہ) {
            edits->copy_Error_مقصد(error_رمز);
        }
    }
}

int32_t
ucasemap_mapUTF8(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_PARAM
                 حرف *dest, int32_t dest_استعداد,
                 ثابت حرف *src, int32_t src_لمبائی,
                 UTF8CaseMapper *stringCaseMapper,
                 icu::Edits *edits,
                 UError_رمز &error_رمز) {
    /* check argument values */
    اگر(U_FAILURE(error_رمز)) {
        واپس 0;
    }
    اگر( dest_استعداد<0 ||
        (dest==خالی_اشاریہ && dest_استعداد>0) ||
        (src==خالی_اشاریہ && src_لمبائی!=0) || src_لمبائی<-1
    ) {
        error_رمز=U_ILLEGAL_دلیل_ERROR;
        واپس 0;
    }

    /* get the string length */
    اگر(src_لمبائی==-1) {
        src_لمبائی = static_cast<int32_t>(uprv_strlen(src));
    }

    /* check for overlapping source and destination */
    اگر( dest!=خالی_اشاریہ &&
        ((src>=dest && src<(dest+dest_استعداد)) ||
         (dest>=src && dest<(src+src_لمبائی)))
    ) {
        error_رمز=U_ILLEGAL_دلیل_ERROR;
        واپس 0;
    }

    اگر (edits != خالی_اشاریہ && (options & U_EDITS_NO_RESET) == 0) {
        edits->reset();
    }
    int32_t reslen = ByteSinkUtil::بذریعہ_Byte_Sink_مقصد_Terminated_Chars(
        dest, dest_استعداد,
        [&](ByteSink& sink, UError_رمز& حالت_2) {
            stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                             reinterpret_cast<ثابت uint8_t*>(src), src_لمبائی, sink, edits, حالت_2);
        },
        error_رمز);
    اگر (U_SUCCESS(error_رمز) && edits != خالی_اشاریہ) {
        edits->copy_Error_مقصد(error_رمز);
    }
    واپس reslen;
}

/* public API functions */

U_CAPI int32_t U_EXPORT2
ucasemap_utf8_مقصد_زیریں(ثابت UCaseMap *csm,
                     حرف *dest, int32_t dest_استعداد,
                     ثابت حرف *src, int32_t src_لمبائی,
                     UError_رمز *p_Error_رمز) {
    خودکار *impl = reinterpret_cast<ثابت UCaseMapImpl *>(csm);
    واپس ucasemap_mapUTF8(
        impl->caseLocale, impl->options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_استعداد,
        src, src_لمبائی,
        ucasemap_اندرونی_UTF8_مقصد_زیریں, خالی_اشاریہ, *p_Error_رمز);
}

U_CAPI int32_t U_EXPORT2
ucasemap_utf8_مقصد_بالائی(ثابت UCaseMap *csm,
                     حرف *dest, int32_t dest_استعداد,
                     ثابت حرف *src, int32_t src_لمبائی,
                     UError_رمز *p_Error_رمز) {
    خودکار *impl = reinterpret_cast<ثابت UCaseMapImpl *>(csm);
    واپس ucasemap_mapUTF8(
        impl->caseLocale, impl->options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_استعداد,
        src, src_لمبائی,
        ucasemap_اندرونی_UTF8_مقصد_بالائی, خالی_اشاریہ, *p_Error_رمز);
}

U_CAPI int32_t U_EXPORT2
ucasemap_utf8FoldCase(ثابت UCaseMap *csm,
                      حرف *dest, int32_t dest_استعداد,
                      ثابت حرف *src, int32_t src_لمبائی,
                      UError_رمز *p_Error_رمز) {
    خودکار *impl = reinterpret_cast<ثابت UCaseMapImpl *>(csm);
    واپس ucasemap_mapUTF8(
        UCASE_LOC_جڑ, impl->options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_استعداد,
        src, src_لمبائی,
        ucasemap_اندرونی_UTF8_Fold, خالی_اشاریہ, *p_Error_رمز);
}

U_NAMESPACE_BEGIN

خالی CaseMap::utf8_مقصد_زیریں(
        ثابت حرف *locale, uint32_t options,
        StringPiece src, ByteSink &sink, Edits *edits,
        UError_رمز &error_رمز) {
    ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        src.ڈیٹا(), src.length(),
        ucasemap_اندرونی_UTF8_مقصد_زیریں, sink, edits, error_رمز);
}

خالی CaseMap::utf8_مقصد_بالائی(
        ثابت حرف *locale, uint32_t options,
        StringPiece src, ByteSink &sink, Edits *edits,
        UError_رمز &error_رمز) {
    ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        src.ڈیٹا(), src.length(),
        ucasemap_اندرونی_UTF8_مقصد_بالائی, sink, edits, error_رمز);
}

خالی CaseMap::utf8Fold(
        uint32_t options,
        StringPiece src, ByteSink &sink, Edits *edits,
        UError_رمز &error_رمز) {
    ucasemap_mapUTF8(
        UCASE_LOC_جڑ, options, UCASEMAP_BREAK_ITERATOR_NULL
        src.ڈیٹا(), src.length(),
        ucasemap_اندرونی_UTF8_Fold, sink, edits, error_رمز);
}

int32_t CaseMap::utf8_مقصد_زیریں(
        ثابت حرف *locale, uint32_t options,
        ثابت حرف *src, int32_t src_لمبائی,
        حرف *dest, int32_t dest_استعداد, Edits *edits,
        UError_رمز &error_رمز) {
    واپس ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_استعداد,
        src, src_لمبائی,
        ucasemap_اندرونی_UTF8_مقصد_زیریں, edits, error_رمز);
}

int32_t CaseMap::utf8_مقصد_بالائی(
        ثابت حرف *locale, uint32_t options,
        ثابت حرف *src, int32_t src_لمبائی,
        حرف *dest, int32_t dest_استعداد, Edits *edits,
        UError_رمز &error_رمز) {
    واپس ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_استعداد,
        src, src_لمبائی,
        ucasemap_اندرونی_UTF8_مقصد_بالائی, edits, error_رمز);
}

int32_t CaseMap::utf8Fold(
        uint32_t options,
        ثابت حرف *src, int32_t src_لمبائی,
        حرف *dest, int32_t dest_استعداد, Edits *edits,
        UError_رمز &error_رمز) {
    واپس ucasemap_mapUTF8(
        UCASE_LOC_جڑ, options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_استعداد,
        src, src_لمبائی,
        ucasemap_اندرونی_UTF8_Fold, edits, error_رمز);
}

U_NAMESPACE_انتہا
