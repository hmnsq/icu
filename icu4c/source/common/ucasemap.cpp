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

#অন্তর্ভাব "unicode/utypes.h"
#অন্তর্ভাব "unicode/brkiter.h"
#অন্তর্ভাব "unicode/bytestream.h"
#অন্তর্ভাব "unicode/casemap.h"
#অন্তর্ভাব "unicode/edits.h"
#অন্তর্ভাব "unicode/stringoptions.h"
#অন্তর্ভাব "unicode/stringpiece.h"
#অন্তর্ভাব "unicode/ubrk.h"
#অন্তর্ভাব "unicode/uloc.h"
#অন্তর্ভাব "unicode/ustring.h"
#অন্তর্ভাব "unicode/ucasemap.h"
#যদি !UCONFIG_NO_BREAK_ITERATION
#অন্তর্ভাব "unicode/utext.h"
#endif
#অন্তর্ভাব "unicode/utf.h"
#অন্তর্ভাব "unicode/utf8.h"
#অন্তর্ভাব "unicode/utf16.h"
#অন্তর্ভাব "bytesinkutil.h"
#অন্তর্ভাব "cmemory.h"
#অন্তর্ভাব "cstring.h"
#অন্তর্ভাব "uassert.h"
#অন্তর্ভাব "ucase.h"
#অন্তর্ভাব "ucasemap_imp.h"

U_NAMESPACE_USE

/* UCaseMap service object -------------------------------------------------- */

// The public type, unicode/ucasemap.h struct UCaseMap, is an opaque C type.
// We define a parallel C++ type in the ICU namespace for entry point renaming.

U_NAMESPACE_BEGIN

UCaseMapImpl::UCaseMapImpl(ধ্রুবক অক্ষর *localeID, uint32_t opts, UErrorCode *pErrorCode) :
#যদি !UCONFIG_NO_BREAK_ITERATION
        iter(nullptr),
#endif
        caseLocale(UCASE_LOC_UNKNOWN), options(opts) {
    ucasemap_setLocale(reinterpret_cast<UCaseMap *>(এই), localeID, pErrorCode);
}

UCaseMapImpl::~UCaseMapImpl() {
#যদি !UCONFIG_NO_BREAK_ITERATION
    delete iter;
#endif
}

U_NAMESPACE_END

U_CAPI UCaseMap * U_EXPORT2
ucasemap_open(ধ্রুবক অক্ষর *locale, uint32_t options, UErrorCode *pErrorCode) {
    যদি(U_FAILURE(*pErrorCode)) {
        ফেরত nullptr;
    }
    UCaseMapImpl *csm = নতুন UCaseMapImpl(locale, options, pErrorCode);
    যদি(csm==nullptr) {
        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
        ফেরত nullptr;
    } অন্যথা যদি (U_FAILURE(*pErrorCode)) {
        delete csm;
        ফেরত nullptr;
    }
    ফেরত reinterpret_cast<UCaseMap *>(csm);
}

U_CAPI শূন্য U_EXPORT2
ucasemap_close(UCaseMap *csm) {
    delete reinterpret_cast<UCaseMapImpl *>(csm);
}

U_CAPI ধ্রুবক অক্ষর* U_EXPORT2
ucasemap_getLocale(ধ্রুবক UCaseMap* csm U_LIFETIME_BOUND) {
    ফেরত reinterpret_cast<ধ্রুবক UCaseMapImpl *>(csm)->locale;
}

U_CAPI uint32_t U_EXPORT2
ucasemap_getOptions(ধ্রুবক UCaseMap *csm) {
    ফেরত reinterpret_cast<ধ্রুবক UCaseMapImpl *>(csm)->options;
}

U_CAPI শূন্য U_EXPORT2
ucasemap_setLocale(UCaseMap *csm, ধ্রুবক অক্ষর *locale, UErrorCode *pErrorCode) {
    যদি(U_FAILURE(*pErrorCode)) {
        ফেরত;
    }
    auto *impl = reinterpret_cast<UCaseMapImpl *>(csm);
    যদি (locale != nullptr && *locale == 0) {
        impl->locale[0] = 0;
        impl->caseLocale = UCASE_LOC_মূল;
        ফেরত;
    }

    UErrorCode বাফার_Status = U_ZERO_ERROR;
    int32_t length=uloc_getName(locale, impl->locale, (int32_t)আকার(impl->locale), &বাফার_Status);
    যদি(বাফার_Status==U_বাফার_OVERFLOW_ERROR || (U_SUCCESS(বাফার_Status) && length==আকার(impl->locale))) {
        বাফার_Status = U_ZERO_ERROR;
        /* we only really need the language code for case mappings */
        length=uloc_getLanguage(locale, impl->locale, (int32_t)আকার(impl->locale), &বাফার_Status);
    }
    যদি(U_FAILURE(বাফার_Status)) {
        *pErrorCode=বাফার_Status;
    } অন্যথা যদি(length==আকার(impl->locale)) {
        *pErrorCode=U_বাফার_OVERFLOW_ERROR;
    }
    যদি(U_SUCCESS(*pErrorCode)) {     
        impl->caseLocale = ucase_getCaseLocale(impl->locale);
    } অন্যথা {
        impl->locale[0]=0;
        impl->caseLocale = UCASE_LOC_মূল;
    }
}

U_CAPI শূন্য U_EXPORT2
ucasemap_setOptions(UCaseMap *csm, uint32_t options, UErrorCode *pErrorCode) {
    যদি(U_FAILURE(*pErrorCode)) {
        ফেরত;
    }
    reinterpret_cast<UCaseMapImpl *>(csm)->options=options;
}

/* UTF-8 string case mappings ----------------------------------------------- */

/* TODO(markus): Move to a new, separate utf8case.cpp file. */

নামাকাশ {

/* append a full case mapping result, see UCASE_MAX_STRING_LENGTH */
অন্তঃরেখ UBool
append_ফলাফল(int32_t cp_দৈর্ঘ্য, int32_t ফলাফল, ধ্রুবক char16_t *s,
             ByteSink &sink, uint32_t options, icu::Edits *edits, UErrorCode &errorCode) {
    U_ASSERT(U_SUCCESS(errorCode));

    /* decode the result */
    যদি(ফলাফল<0) {
        /* (not) original code point */
        যদি(edits!=nullptr) {
            edits->addUnchanged(cp_দৈর্ঘ্য);
        }
        যদি((options & U_OMIT_UNCHANGED_TEXT) == 0) {
            ByteSinkUtil::appendCodePoint(cp_দৈর্ঘ্য, ~ফলাফল, sink);
        }
    } অন্যথা {
        যদি(ফলাফল<=UCASE_MAX_STRING_দৈর্ঘ্য) {
            // string: "result" is the UTF-16 length
            ফেরত ByteSinkUtil::appendChange(cp_দৈর্ঘ্য, s, ফলাফল, sink, edits, errorCode);
        } অন্যথা {
            ByteSinkUtil::appendCodePoint(cp_দৈর্ঘ্য, ফলাফল, sink, edits);
        }
    }
    ফেরত সত্য;
}

// See unicode/utf8.h U8_APPEND_UNSAFE().
অন্তঃরেখ uint8_t getTwoByteLead(UChar32 c) { ফেরত static_cast<uint8_t>((c >> 6) | 0xc0); }
অন্তঃরেখ uint8_t getTwoByteTrail(UChar32 c) { ফেরত static_cast<uint8_t>((c & 0x3f) | 0x80); }

UChar32 U_CALLCONV
utf8_caseContextIterator(শূন্য *context, int8_t dir) {
    UCaseContext* csc = static_cast<UCaseContext*>(context);
    UChar32 c;

    যদি(dir<0) {
        /* reset for backward iteration */
        csc->সূচক=csc->cpStart;
        csc->dir=dir;
    } অন্যথা যদি(dir>0) {
        /* reset for forward iteration */
        csc->সূচক=csc->cpLimit;
        csc->dir=dir;
    } অন্যথা {
        /* continue current iteration direction */
        dir=csc->dir;
    }

    যদি(dir<0) {
        যদি(csc->start<csc->সূচক) {
            U8_পূর্ব((ধ্রুবক uint8_t *)csc->p, csc->start, csc->সূচক, c);
            ফেরত c;
        }
    } অন্যথা {
        যদি(csc->সূচক<csc->limit) {
            U8_পরবর্তী((ধ্রুবক uint8_t *)csc->p, csc->সূচক, csc->limit, c);
            ফেরত c;
        }
    }
    ফেরত U_SENTINEL;
}

/**
 * caseLocale >= 0: Lowercases [srcStart..srcLimit[ but takes context [0..srcLength[ into account.
 * caseLocale < 0: Case-folds [srcStart..srcLimit[.
 */
শূন্য toLower(int32_t caseLocale, uint32_t options,
             ধ্রুবক uint8_t *src, UCaseContext *csc, int32_t srcStart, int32_t srcLimit,
             icu::ByteSink &sink, icu::Edits *edits, UErrorCode &errorCode) {
    ধ্রুবক int8_t *latinToLower;
    যদি (caseLocale == UCASE_LOC_মূল ||
            (caseLocale >= 0 ?
                !(caseLocale == UCASE_LOC_TURKISH || caseLocale == UCASE_LOC_LITHUANIAN) :
                (options & _FOLD_CASE_OPTIONS_MASK) == U_FOLD_CASE_DEFAULT)) {
        latinToLower = LatinCase::TO_LOWER_NORMAL;
    } অন্যথা {
        latinToLower = LatinCase::TO_LOWER_TR_LT;
    }
    ধ্রুবক UTrie2 *trie = ucase_getTrie();
    int32_t পূর্ব = srcStart;
    int32_t src_সূচক = srcStart;
    জন্য (;;) {
        // fast path for simple cases
        int32_t cpStart;
        UChar32 c;
        জন্য (;;) {
            যদি (U_FAILURE(errorCode) || src_সূচক >= srcLimit) {
                c = U_SENTINEL;
                বিরতি;
            }
            uint8_t lead = src[src_সূচক++];
            যদি (lead <= 0x7f) {
                int8_t d = latinToLower[lead];
                যদি (d == LatinCase::EXC) {
                    cpStart = src_সূচক - 1;
                    c = lead;
                    বিরতি;
                }
                যদি (d == 0) { চালু; }
                ByteSinkUtil::appendUnchanged(src + পূর্ব, src_সূচক - 1 - পূর্ব,
                                              sink, options, edits, errorCode);
                অক্ষর ascii = static_cast<অক্ষর>(lead + d);
                sink.Append(&ascii, 1);
                যদি (edits != nullptr) {
                    edits->addReplace(1, 1);
                }
                পূর্ব = src_সূচক;
                চালু;
            } অন্যথা যদি (lead < 0xe3) {
                uint8_t t;
                যদি (0xc2 <= lead && lead <= 0xc5 && src_সূচক < srcLimit &&
                        (t = src[src_সূচক] - 0x80) <= 0x3f) {
                    // U+0080..U+017F
                    ++src_সূচক;
                    c = ((lead - 0xc0) << 6) | t;
                    int8_t d = latinToLower[c];
                    যদি (d == LatinCase::EXC) {
                        cpStart = src_সূচক - 2;
                        বিরতি;
                    }
                    যদি (d == 0) { চালু; }
                    ByteSinkUtil::appendUnchanged(src + পূর্ব, src_সূচক - 2 - পূর্ব,
                                                  sink, options, edits, errorCode);
                    ByteSinkUtil::appendTwoBytes(c + d, sink);
                    যদি (edits != nullptr) {
                        edits->addReplace(2, 2);
                    }
                    পূর্ব = src_সূচক;
                    চালু;
                }
            } অন্যথা যদি ((lead <= 0xe9 || lead == 0xeb || lead == 0xec) &&
                    (src_সূচক + 2) <= srcLimit &&
                    U8_IS_TRAIL(src[src_সূচক]) && U8_IS_TRAIL(src[src_সূচক + 1])) {
                // most of CJK: no case mappings
                src_সূচক += 2;
                চালু;
            }
            cpStart = --src_সূচক;
            U8_পরবর্তী(src, src_সূচক, srcLimit, c);
            যদি (c < 0) {
                // ill-formed UTF-8
                চালু;
            }
            uint16_t props = UTRIE2_GET16(trie, c);
            যদি (UCASE_HAS_EXCEPTION(props)) { বিরতি; }
            int32_t delta;
            যদি (!UCASE_IS_UPPER_OR_TITLE(props) || (delta = UCASE_GET_DELTA(props)) == 0) {
                চালু;
            }
            ByteSinkUtil::appendUnchanged(src + পূর্ব, cpStart - পূর্ব,
                                          sink, options, edits, errorCode);
            ByteSinkUtil::appendCodePoint(src_সূচক - cpStart, c + delta, sink, edits);
            পূর্ব = src_সূচক;
        }
        যদি (c < 0) {
            বিরতি;
        }
        // slow path
        ধ্রুবক char16_t *s;
        যদি (caseLocale >= 0) {
            csc->cpStart = cpStart;
            csc->cpLimit = src_সূচক;
            c = ucase_toFullLower(c, utf8_caseContextIterator, csc, &s, caseLocale);
        } অন্যথা {
            c = ucase_toFullFolding(c, &s, options);
        }
        যদি (c >= 0) {
            ByteSinkUtil::appendUnchanged(src + পূর্ব, cpStart - পূর্ব,
                                          sink, options, edits, errorCode);
            append_ফলাফল(src_সূচক - cpStart, c, s, sink, options, edits, errorCode);
            পূর্ব = src_সূচক;
        }
    }
    ByteSinkUtil::appendUnchanged(src + পূর্ব, src_সূচক - পূর্ব,
                                  sink, options, edits, errorCode);
}

শূন্য toUpper(int32_t caseLocale, uint32_t options,
             ধ্রুবক uint8_t *src, UCaseContext *csc, int32_t src_দৈর্ঘ্য,
             icu::ByteSink &sink, icu::Edits *edits, UErrorCode &errorCode) {
    ধ্রুবক int8_t *latinToUpper;
    যদি (caseLocale == UCASE_LOC_TURKISH) {
        latinToUpper = LatinCase::TO_UPPER_TR;
    } অন্যথা {
        latinToUpper = LatinCase::TO_UPPER_NORMAL;
    }
    ধ্রুবক UTrie2 *trie = ucase_getTrie();
    int32_t পূর্ব = 0;
    int32_t src_সূচক = 0;
    জন্য (;;) {
        // fast path for simple cases
        int32_t cpStart;
        UChar32 c;
        জন্য (;;) {
            যদি (U_FAILURE(errorCode) || src_সূচক >= src_দৈর্ঘ্য) {
                c = U_SENTINEL;
                বিরতি;
            }
            uint8_t lead = src[src_সূচক++];
            যদি (lead <= 0x7f) {
                int8_t d = latinToUpper[lead];
                যদি (d == LatinCase::EXC) {
                    cpStart = src_সূচক - 1;
                    c = lead;
                    বিরতি;
                }
                যদি (d == 0) { চালু; }
                ByteSinkUtil::appendUnchanged(src + পূর্ব, src_সূচক - 1 - পূর্ব,
                                              sink, options, edits, errorCode);
                অক্ষর ascii = static_cast<অক্ষর>(lead + d);
                sink.Append(&ascii, 1);
                যদি (edits != nullptr) {
                    edits->addReplace(1, 1);
                }
                পূর্ব = src_সূচক;
                চালু;
            } অন্যথা যদি (lead < 0xe3) {
                uint8_t t;
                যদি (0xc2 <= lead && lead <= 0xc5 && src_সূচক < src_দৈর্ঘ্য &&
                        (t = src[src_সূচক] - 0x80) <= 0x3f) {
                    // U+0080..U+017F
                    ++src_সূচক;
                    c = ((lead - 0xc0) << 6) | t;
                    int8_t d = latinToUpper[c];
                    যদি (d == LatinCase::EXC) {
                        cpStart = src_সূচক - 2;
                        বিরতি;
                    }
                    যদি (d == 0) { চালু; }
                    ByteSinkUtil::appendUnchanged(src + পূর্ব, src_সূচক - 2 - পূর্ব,
                                                  sink, options, edits, errorCode);
                    ByteSinkUtil::appendTwoBytes(c + d, sink);
                    যদি (edits != nullptr) {
                        edits->addReplace(2, 2);
                    }
                    পূর্ব = src_সূচক;
                    চালু;
                }
            } অন্যথা যদি ((lead <= 0xe9 || lead == 0xeb || lead == 0xec) &&
                    (src_সূচক + 2) <= src_দৈর্ঘ্য &&
                    U8_IS_TRAIL(src[src_সূচক]) && U8_IS_TRAIL(src[src_সূচক + 1])) {
                // most of CJK: no case mappings
                src_সূচক += 2;
                চালু;
            }
            cpStart = --src_সূচক;
            U8_পরবর্তী(src, src_সূচক, src_দৈর্ঘ্য, c);
            যদি (c < 0) {
                // ill-formed UTF-8
                চালু;
            }
            uint16_t props = UTRIE2_GET16(trie, c);
            যদি (UCASE_HAS_EXCEPTION(props)) { বিরতি; }
            int32_t delta;
            যদি (UCASE_GET_TYPE(props) != UCASE_LOWER || (delta = UCASE_GET_DELTA(props)) == 0) {
                চালু;
            }
            ByteSinkUtil::appendUnchanged(src + পূর্ব, cpStart - পূর্ব,
                                          sink, options, edits, errorCode);
            ByteSinkUtil::appendCodePoint(src_সূচক - cpStart, c + delta, sink, edits);
            পূর্ব = src_সূচক;
        }
        যদি (c < 0) {
            বিরতি;
        }
        // slow path
        csc->cpStart = cpStart;
        csc->cpLimit = src_সূচক;
        ধ্রুবক char16_t *s;
        c = ucase_toFullUpper(c, utf8_caseContextIterator, csc, &s, caseLocale);
        যদি (c >= 0) {
            ByteSinkUtil::appendUnchanged(src + পূর্ব, cpStart - পূর্ব,
                                          sink, options, edits, errorCode);
            append_ফলাফল(src_সূচক - cpStart, c, s, sink, options, edits, errorCode);
            পূর্ব = src_সূচক;
        }
    }
    ByteSinkUtil::appendUnchanged(src + পূর্ব, src_সূচক - পূর্ব,
                                  sink, options, edits, errorCode);
}

}  // namespace

#যদি !UCONFIG_NO_BREAK_ITERATION

নামাকাশ {

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
int32_t maybeTitleDutchIJ(ধ্রুবক uint8_t *src, UChar32 c, int32_t start, int32_t segmentLimit,
                          ByteSink &sink, uint32_t options, icu::Edits *edits, UErrorCode &errorCode) {
    U_ASSERT(start < segmentLimit);

    int32_t সূচক = start;
    যৌক্তিক withAcute = মিথ্যা;

    // If the conditions are met, then the following variables tell us what to output.
    int32_t unchanged1 = 0;  // code units before the j, or the whole sequence (0..3)
    যৌক্তিক doTitleJ = মিথ্যা;  // true if the j needs to be titlecased
    int32_t unchanged2 = 0;  // after the j (0 or 1)

    // next character after the first letter
    UChar32 c2;
    c2 = src[সূচক++];

    // Is the first letter an i/I with accent?
    যদি (c == u'I') {
        যদি (c2 == ACUTE_BYTE0 && সূচক < segmentLimit && src[সূচক++] == ACUTE_BYTE1) {
            withAcute = সত্য;
            unchanged1 = 2;  // ACUTE is 2 code units in UTF-8
            যদি (সূচক == segmentLimit) { ফেরত start; }
            c2 = src[সূচক++];
        }
    } অন্যথা {  // Í
        withAcute = সত্য;
    }

    // Is the next character a j/J?
    যদি (c2 == u'j') {
        doTitleJ = সত্য;
    } অন্যথা যদি (c2 == u'J') {
        ++unchanged1;
    } অন্যথা {
        ফেরত start;
    }

    // A plain i/I must be followed by a plain j/J.
    // An i/I with acute must be followed by a j/J with acute.
    যদি (withAcute) {
        যদি ((সূচক + 1) >= segmentLimit || src[সূচক++] != ACUTE_BYTE0 || src[সূচক++] != ACUTE_BYTE1) {
            ফেরত start;
        }
        যদি (doTitleJ) {
            unchanged2 = 2;  // ACUTE is 2 code units in UTF-8
        } অন্যথা {
            unchanged1 = unchanged1 + 2;    // ACUTE is 2 code units in UTF-8
        }
    }

    // There must not be another combining mark.
    যদি (সূচক < segmentLimit) {
        int32_t cp;
        int32_t i = সূচক;
        U8_পরবর্তী(src, i, segmentLimit, cp);
        uint32_t typeMask = U_GET_GC_MASK(cp);
        যদি ((typeMask & U_GC_M_MASK) != 0) {
            ফেরত start;
        }
    }

    // Output the rest of the Dutch IJ.
    ByteSinkUtil::appendUnchanged(src + start, unchanged1, sink, options, edits, errorCode);
    start += unchanged1;
    যদি (doTitleJ) {
        ByteSinkUtil::appendCodePoint(1, u'J', sink, edits);
        ++start;
    }
    ByteSinkUtil::appendUnchanged(src + start, unchanged2, sink, options, edits, errorCode);

    U_ASSERT(start + unchanged2 == সূচক);
    ফেরত সূচক;
}

}  // namespace

U_CFUNC শূন্য U_CALLCONV
ucasemap_internalUTF8ToTitle(
        int32_t caseLocale, uint32_t options, BreakIterator *iter,
        ধ্রুবক uint8_t *src, int32_t src_দৈর্ঘ্য,
        ByteSink &sink, icu::Edits *edits,
        UErrorCode &errorCode) {
    যদি (!ustrcase_checkTitleAdjustmentOptions(options, errorCode)) {
        ফেরত;
    }

    /* set up local variables */
    UCaseContext csc=UCASECONTEXT_INITIALIZER;
    csc.p=(শূন্য *)src;
    csc.limit=src_দৈর্ঘ্য;
    int32_t পূর্ব=0;
    UBool is_First_সূচক=সত্য;

    /* titlecasing loop */
    যখন(পূর্ব<src_দৈর্ঘ্য) {
        /* find next index where to titlecase */
        int32_t সূচক;
        যদি(is_First_সূচক) {
            is_First_সূচক=মিথ্যা;
            সূচক=iter->first();
        } অন্যথা {
            সূচক=iter->next();
        }
        যদি(সূচক==UBRK_DONE || সূচক>src_দৈর্ঘ্য) {
            সূচক=src_দৈর্ঘ্য;
        }

        /*
         * Segment [prev..index[ into 3 parts:
         * a) skipped characters (copy as-is) [prev..titleStart[
         * b) first letter (titlecase)              [titleStart..titleLimit[
         * c) subsequent characters (lowercase)                 [titleLimit..index[
         */
        যদি(পূর্ব<সূচক) {
            /* find and copy skipped characters [prev..titleStart[ */
            int32_t titleStart=পূর্ব;
            int32_t titleLimit=পূর্ব;
            UChar32 c;
            U8_পরবর্তী(src, titleLimit, সূচক, c);
            যদি ((options&U_TITLECASE_NO_BREAK_ADJUSTMENT)==0) {
                // Adjust the titlecasing index to the next cased character,
                // or to the next letter/number/symbol/private use.
                // Stop with titleStart<titleLimit<=index
                // if there is a character to be titlecased,
                // or else stop with titleStart==titleLimit==index.
                UBool toCased = (options&U_TITLECASE_ADJUST_TO_CASED) != 0;
                যখন (toCased ? UCASE_NONE==ucase_getType(c) : !ustrcase_isLNS(c)) {
                    titleStart=titleLimit;
                    যদি(titleLimit==সূচক) {
                        বিরতি;
                    }
                    U8_পরবর্তী(src, titleLimit, সূচক, c);
                }
                যদি (পূর্ব < titleStart) {
                    যদি (!ByteSinkUtil::appendUnchanged(src+পূর্ব, titleStart-পূর্ব,
                                                       sink, options, edits, errorCode)) {
                        ফেরত;
                    }
                }
            }

            যদি(titleStart<titleLimit) {
                /* titlecase c which is from [titleStart..titleLimit[ */
                যদি(c>=0) {
                    csc.cpStart=titleStart;
                    csc.cpLimit=titleLimit;
                    ধ্রুবক char16_t *s;
                    c=ucase_toFullTitle(c, utf8_caseContextIterator, &csc, &s, caseLocale);
                    যদি (!append_ফলাফল(titleLimit-titleStart, c, s, sink, options, edits, errorCode)) {
                        ফেরত;
                    }
                } অন্যথা {
                    // Malformed UTF-8.
                    যদি (!ByteSinkUtil::appendUnchanged(src+titleStart, titleLimit-titleStart,
                                                       sink, options, edits, errorCode)) {
                        ফেরত;
                    }
                }

                /* Special case Dutch IJ titlecasing */
                যদি (titleLimit < সূচক &&
                    caseLocale == UCASE_LOC_DUTCH) {
                    যদি (c < 0) {
                        c = ~c;
                    }

                    যদি (c == u'I' || c == u'Í') {
                        titleLimit = maybeTitleDutchIJ(src, c, titleLimit, সূচক, sink, options, edits, errorCode);
                    }
                }

                /* lowercase [titleLimit..index[ */
                যদি(titleLimit<সূচক) {
                    যদি((options&U_TITLECASE_NO_LOWERCASE)==0) {
                        /* Normal operation: Lowercase the rest of the word. */
                        toLower(caseLocale, options,
                                src, &csc, titleLimit, সূচক,
                                sink, edits, errorCode);
                        যদি(U_FAILURE(errorCode)) {
                            ফেরত;
                        }
                    } অন্যথা {
                        /* Optionally just copy the rest of the word unchanged. */
                        যদি (!ByteSinkUtil::appendUnchanged(src+titleLimit, সূচক-titleLimit,
                                                           sink, options, edits, errorCode)) {
                            ফেরত;
                        }
                    }
                }
            }
        }

        পূর্ব=সূচক;
    }
}

#endif

U_NAMESPACE_BEGIN
নামাকাশ GreekUpper {

UBool isFollowedByCasedLetter(ধ্রুবক uint8_t *s, int32_t i, int32_t length) {
    যখন (i < length) {
        UChar32 c;
        U8_পরবর্তী(s, i, length, c);
        int32_t type = ucase_getTypeOrIgnorable(c);
        যদি ((type & UCASE_IGNORABLE) != 0) {
            // Case-ignorable, continue with the loop.
        } অন্যথা যদি (type != UCASE_NONE) {
            ফেরত সত্য;  // Followed by cased letter.
        } অন্যথা {
            ফেরত মিথ্যা;  // Uncased and not case-ignorable.
        }
    }
    ফেরত মিথ্যা;  // Not followed by cased letter.
}

// Keep this consistent with the UTF-16 version in ustrcase.cpp and the Java version in CaseMap.java.
শূন্য toUpper(uint32_t options,
             ধ্রুবক uint8_t *src, int32_t src_দৈর্ঘ্য,
             ByteSink &sink, Edits *edits,
             UErrorCode &errorCode) {
    uint32_t state = 0;
    জন্য (int32_t i = 0; i < src_দৈর্ঘ্য;) {
        int32_t পরবর্তী_সূচক = i;
        UChar32 c;
        U8_পরবর্তী(src, পরবর্তী_সূচক, src_দৈর্ঘ্য, c);
        uint32_t পরবর্তী_State = 0;
        int32_t type = ucase_getTypeOrIgnorable(c);
        যদি ((type & UCASE_IGNORABLE) != 0) {
            // c is case-ignorable
            পরবর্তী_State |= (state & AFTER_CASED);
        } অন্যথা যদি (type != UCASE_NONE) {
            // c is cased
            পরবর্তী_State |= AFTER_CASED;
        }
        uint32_t তথ্য = get_Letter_তথ্য(c);
        যদি (তথ্য > 0) {
            uint32_t upper = তথ্য & UPPER_MASK;
            // Add a dialytika to this iota or ypsilon vowel
            // if we removed a tonos from the previous vowel,
            // and that previous vowel did not also have (or gain) a dialytika.
            // Adding one only to the final vowel in a longer sequence
            // (which does not occur in normal writing) would require lookahead.
            // Set the same flag as for preserving an existing dialytika.
            যদি ((তথ্য & HAS_VOWEL) != 0 &&
                (state & (AFTER_VOWEL_WITH_PRECOMPOSED_ACCENT | AFTER_VOWEL_WITH_COMBINING_ACCENT)) !=
                    0 &&
                (upper == 0x399 || upper == 0x3A5)) {
                তথ্য |= (state & AFTER_VOWEL_WITH_PRECOMPOSED_ACCENT) != 0 ? HAS_DIALYTIKA
                                                                           : HAS_COMBINING_DIALYTIKA;
            }
            int32_t অঙ্ক_Ypogegrammeni = 0;  // Map each one to a trailing, spacing, capital iota.
            যদি ((তথ্য & HAS_YPOGEGRAMMENI) != 0) {
                অঙ্ক_Ypogegrammeni = 1;
            }
            ধ্রুবক UBool hasPrecomposedAccent = (তথ্য & HAS_ACCENT) != 0;
            // Skip combining diacritics after this Greek letter.
            int32_t পরবর্তী_পরবর্তী_সূচক = পরবর্তী_সূচক;
            যখন (পরবর্তী_সূচক < src_দৈর্ঘ্য) {
                UChar32 c2;
                U8_পরবর্তী(src, পরবর্তী_পরবর্তী_সূচক, src_দৈর্ঘ্য, c2);
                uint32_t diacritic_তথ্য = get_Diacritic_তথ্য(c2);
                যদি (diacritic_তথ্য != 0) {
                    তথ্য |= diacritic_তথ্য;
                    যদি ((diacritic_তথ্য & HAS_YPOGEGRAMMENI) != 0) {
                        ++অঙ্ক_Ypogegrammeni;
                    }
                    পরবর্তী_সূচক = পরবর্তী_পরবর্তী_সূচক;
                } অন্যথা {
                    বিরতি;  // not a Greek diacritic
                }
            }
            যদি ((তথ্য & HAS_VOWEL_AND_ACCENT_AND_DIALYTIKA) == HAS_VOWEL_AND_ACCENT) {
                পরবর্তী_State |= hasPrecomposedAccent ? AFTER_VOWEL_WITH_PRECOMPOSED_ACCENT
                                                  : AFTER_VOWEL_WITH_COMBINING_ACCENT;
            }
            // Map according to Greek rules.
            UBool addTonos = মিথ্যা;
            যদি (upper == 0x397 &&
                    (তথ্য & HAS_ACCENT) != 0 &&
                    অঙ্ক_Ypogegrammeni == 0 &&
                    (state & AFTER_CASED) == 0 &&
                    !isFollowedByCasedLetter(src, পরবর্তী_সূচক, src_দৈর্ঘ্য)) {
                // Keep disjunctive "or" with (only) a tonos.
                // We use the same "word boundary" conditions as for the Final_Sigma test.
                যদি (hasPrecomposedAccent) {
                    upper = 0x389;  // Preserve the precomposed form.
                } অন্যথা {
                    addTonos = সত্য;
                }
            } অন্যথা যদি ((তথ্য & HAS_DIALYTIKA) != 0) {
                // Preserve a vowel with dialytika in precomposed form if it exists.
                যদি (upper == 0x399) {
                    upper = 0x3AA;
                    তথ্য &= ~HAS_EITHER_DIALYTIKA;
                } অন্যথা যদি (upper == 0x3A5) {
                    upper = 0x3AB;
                    তথ্য &= ~HAS_EITHER_DIALYTIKA;
                }
            }

            UBool change;
            যদি (edits == nullptr && (options & U_OMIT_UNCHANGED_TEXT) == 0) {
                change = সত্য;  // common, simple usage
            } অন্যথা {
                // Find out first whether we are changing the text.
                U_ASSERT(0x370 <= upper && upper <= 0x3ff);  // 2-byte UTF-8, main Greek block
                change = (i + 2) > পরবর্তী_সূচক ||
                        src[i] != getTwoByteLead(upper) || src[i + 1] != getTwoByteTrail(upper) ||
                        অঙ্ক_Ypogegrammeni > 0;
                int32_t i2 = i + 2;
                যদি ((তথ্য & HAS_EITHER_DIALYTIKA) != 0) {
                    change |= (i2 + 2) > পরবর্তী_সূচক ||
                            src[i2] != static_cast<uint8_t>(u8"\u0308"[0]) ||
                            src[i2 + 1] != static_cast<uint8_t>(u8"\u0308"[1]);
                    i2 += 2;
                }
                যদি (addTonos) {
                    change |= (i2 + 2) > পরবর্তী_সূচক ||
                            src[i2] != static_cast<uint8_t>(u8"\u0301"[0]) ||
                            src[i2 + 1] != static_cast<uint8_t>(u8"\u0301"[1]);
                    i2 += 2;
                }
                int32_t old_দৈর্ঘ্য = পরবর্তী_সূচক - i;
                int32_t new_দৈর্ঘ্য = (i2 - i) + অঙ্ক_Ypogegrammeni * 2;  // 2 bytes per U+0399
                change |= old_দৈর্ঘ্য != new_দৈর্ঘ্য;
                যদি (change) {
                    যদি (edits != nullptr) {
                        edits->addReplace(old_দৈর্ঘ্য, new_দৈর্ঘ্য);
                    }
                } অন্যথা {
                    যদি (edits != nullptr) {
                        edits->addUnchanged(old_দৈর্ঘ্য);
                    }
                    // Write unchanged text?
                    change = (options & U_OMIT_UNCHANGED_TEXT) == 0;
                }
            }

            যদি (change) {
                ByteSinkUtil::appendTwoBytes(upper, sink);
                যদি ((তথ্য & HAS_EITHER_DIALYTIKA) != 0) {
                    sink.AppendU8(u8"\u0308", 2);  // restore or add a dialytika
                }
                যদি (addTonos) {
                    sink.AppendU8(u8"\u0301", 2);
                }
                যখন (অঙ্ক_Ypogegrammeni > 0) {
                    sink.AppendU8(u8"\u0399", 2);
                    --অঙ্ক_Ypogegrammeni;
                }
            }
        } অন্যথা যদি(c>=0) {
            ধ্রুবক char16_t *s;
            c=ucase_toFullUpper(c, nullptr, nullptr, &s, UCASE_LOC_GREEK);
            যদি (!append_ফলাফল(পরবর্তী_সূচক - i, c, s, sink, options, edits, errorCode)) {
                ফেরত;
            }
        } অন্যথা {
            // Malformed UTF-8.
            যদি (!ByteSinkUtil::appendUnchanged(src+i, পরবর্তী_সূচক-i,
                                               sink, options, edits, errorCode)) {
                ফেরত;
            }
        }
        i = পরবর্তী_সূচক;
        state = পরবর্তী_State;
    }
}

}  // namespace GreekUpper
U_NAMESPACE_END

স্থির শূন্য U_CALLCONV
ucasemap_internalUTF8ToLower(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_UNUSED
                             ধ্রুবক uint8_t *src, int32_t src_দৈর্ঘ্য,
                             icu::ByteSink &sink, icu::Edits *edits,
                             UErrorCode &errorCode) {
    UCaseContext csc=UCASECONTEXT_INITIALIZER;
    csc.p=(শূন্য *)src;
    csc.limit=src_দৈর্ঘ্য;
    toLower(
        caseLocale, options,
        src, &csc, 0, src_দৈর্ঘ্য,
        sink, edits, errorCode);
}

স্থির শূন্য U_CALLCONV
ucasemap_internalUTF8ToUpper(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_UNUSED
                             ধ্রুবক uint8_t *src, int32_t src_দৈর্ঘ্য,
                             icu::ByteSink &sink, icu::Edits *edits,
                             UErrorCode &errorCode) {
    যদি (caseLocale == UCASE_LOC_GREEK) {
        GreekUpper::toUpper(options, src, src_দৈর্ঘ্য, sink, edits, errorCode);
    } অন্যথা {
        UCaseContext csc=UCASECONTEXT_INITIALIZER;
        csc.p=(শূন্য *)src;
        csc.limit=src_দৈর্ঘ্য;
        toUpper(
            caseLocale, options,
            src, &csc, src_দৈর্ঘ্য,
            sink, edits, errorCode);
    }
}

স্থির শূন্য U_CALLCONV
ucasemap_internalUTF8Fold(int32_t /* caseLocale */, uint32_t options, UCASEMAP_BREAK_ITERATOR_UNUSED
                          ধ্রুবক uint8_t *src, int32_t src_দৈর্ঘ্য,
                          icu::ByteSink &sink, icu::Edits *edits,
                          UErrorCode &errorCode) {
    toLower(
        -1, options,
        src, nullptr, 0, src_দৈর্ঘ্য,
        sink, edits, errorCode);
}

শূন্য
ucasemap_mapUTF8(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_PARAM
                 ধ্রুবক অক্ষর *src, int32_t src_দৈর্ঘ্য,
                 UTF8CaseMapper *stringCaseMapper,
                 icu::ByteSink &sink, icu::Edits *edits,
                 UErrorCode &errorCode) {
    /* check argument values */
    যদি (U_FAILURE(errorCode)) {
        ফেরত;
    }
    যদি ((src == nullptr && src_দৈর্ঘ্য != 0) || src_দৈর্ঘ্য < -1) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত;
    }

    // Get the string length.
    যদি (src_দৈর্ঘ্য == -1) {
        src_দৈর্ঘ্য = static_cast<int32_t>(uprv_strlen(src));
    }

    যদি (edits != nullptr && (options & U_EDITS_NO_RESET) == 0) {
        edits->reset();
    }
    stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                     reinterpret_cast<ধ্রুবক uint8_t*>(src), src_দৈর্ঘ্য, sink, edits, errorCode);
    sink.Flush();
    যদি (U_SUCCESS(errorCode)) {
        যদি (edits != nullptr) {
            edits->copyErrorTo(errorCode);
        }
    }
}

int32_t
ucasemap_mapUTF8(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_PARAM
                 অক্ষর *dest, int32_t destCapacity,
                 ধ্রুবক অক্ষর *src, int32_t src_দৈর্ঘ্য,
                 UTF8CaseMapper *stringCaseMapper,
                 icu::Edits *edits,
                 UErrorCode &errorCode) {
    /* check argument values */
    যদি(U_FAILURE(errorCode)) {
        ফেরত 0;
    }
    যদি( destCapacity<0 ||
        (dest==nullptr && destCapacity>0) ||
        (src==nullptr && src_দৈর্ঘ্য!=0) || src_দৈর্ঘ্য<-1
    ) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত 0;
    }

    /* get the string length */
    যদি(src_দৈর্ঘ্য==-1) {
        src_দৈর্ঘ্য = static_cast<int32_t>(uprv_strlen(src));
    }

    /* check for overlapping source and destination */
    যদি( dest!=nullptr &&
        ((src>=dest && src<(dest+destCapacity)) ||
         (dest>=src && dest<(src+src_দৈর্ঘ্য)))
    ) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত 0;
    }

    যদি (edits != nullptr && (options & U_EDITS_NO_RESET) == 0) {
        edits->reset();
    }
    int32_t reslen = ByteSinkUtil::viaByteSinkToTerminatedChars(
        dest, destCapacity,
        [&](ByteSink& sink, UErrorCode& status) {
            stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                             reinterpret_cast<ধ্রুবক uint8_t*>(src), src_দৈর্ঘ্য, sink, edits, status);
        },
        errorCode);
    যদি (U_SUCCESS(errorCode) && edits != nullptr) {
        edits->copyErrorTo(errorCode);
    }
    ফেরত reslen;
}

/* public API functions */

U_CAPI int32_t U_EXPORT2
ucasemap_utf8ToLower(ধ্রুবক UCaseMap *csm,
                     অক্ষর *dest, int32_t destCapacity,
                     ধ্রুবক অক্ষর *src, int32_t src_দৈর্ঘ্য,
                     UErrorCode *pErrorCode) {
    auto *impl = reinterpret_cast<ধ্রুবক UCaseMapImpl *>(csm);
    ফেরত ucasemap_mapUTF8(
        impl->caseLocale, impl->options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, destCapacity,
        src, src_দৈর্ঘ্য,
        ucasemap_internalUTF8ToLower, nullptr, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
ucasemap_utf8ToUpper(ধ্রুবক UCaseMap *csm,
                     অক্ষর *dest, int32_t destCapacity,
                     ধ্রুবক অক্ষর *src, int32_t src_দৈর্ঘ্য,
                     UErrorCode *pErrorCode) {
    auto *impl = reinterpret_cast<ধ্রুবক UCaseMapImpl *>(csm);
    ফেরত ucasemap_mapUTF8(
        impl->caseLocale, impl->options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, destCapacity,
        src, src_দৈর্ঘ্য,
        ucasemap_internalUTF8ToUpper, nullptr, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
ucasemap_utf8FoldCase(ধ্রুবক UCaseMap *csm,
                      অক্ষর *dest, int32_t destCapacity,
                      ধ্রুবক অক্ষর *src, int32_t src_দৈর্ঘ্য,
                      UErrorCode *pErrorCode) {
    auto *impl = reinterpret_cast<ধ্রুবক UCaseMapImpl *>(csm);
    ফেরত ucasemap_mapUTF8(
        UCASE_LOC_মূল, impl->options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, destCapacity,
        src, src_দৈর্ঘ্য,
        ucasemap_internalUTF8Fold, nullptr, *pErrorCode);
}

U_NAMESPACE_BEGIN

শূন্য CaseMap::utf8ToLower(
        ধ্রুবক অক্ষর *locale, uint32_t options,
        StringPiece src, ByteSink &sink, Edits *edits,
        UErrorCode &errorCode) {
    ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        src.তথ্য(), src.length(),
        ucasemap_internalUTF8ToLower, sink, edits, errorCode);
}

শূন্য CaseMap::utf8ToUpper(
        ধ্রুবক অক্ষর *locale, uint32_t options,
        StringPiece src, ByteSink &sink, Edits *edits,
        UErrorCode &errorCode) {
    ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        src.তথ্য(), src.length(),
        ucasemap_internalUTF8ToUpper, sink, edits, errorCode);
}

শূন্য CaseMap::utf8Fold(
        uint32_t options,
        StringPiece src, ByteSink &sink, Edits *edits,
        UErrorCode &errorCode) {
    ucasemap_mapUTF8(
        UCASE_LOC_মূল, options, UCASEMAP_BREAK_ITERATOR_NULL
        src.তথ্য(), src.length(),
        ucasemap_internalUTF8Fold, sink, edits, errorCode);
}

int32_t CaseMap::utf8ToLower(
        ধ্রুবক অক্ষর *locale, uint32_t options,
        ধ্রুবক অক্ষর *src, int32_t src_দৈর্ঘ্য,
        অক্ষর *dest, int32_t destCapacity, Edits *edits,
        UErrorCode &errorCode) {
    ফেরত ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, destCapacity,
        src, src_দৈর্ঘ্য,
        ucasemap_internalUTF8ToLower, edits, errorCode);
}

int32_t CaseMap::utf8ToUpper(
        ধ্রুবক অক্ষর *locale, uint32_t options,
        ধ্রুবক অক্ষর *src, int32_t src_দৈর্ঘ্য,
        অক্ষর *dest, int32_t destCapacity, Edits *edits,
        UErrorCode &errorCode) {
    ফেরত ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, destCapacity,
        src, src_দৈর্ঘ্য,
        ucasemap_internalUTF8ToUpper, edits, errorCode);
}

int32_t CaseMap::utf8Fold(
        uint32_t options,
        ধ্রুবক অক্ষর *src, int32_t src_দৈর্ঘ্য,
        অক্ষর *dest, int32_t destCapacity, Edits *edits,
        UErrorCode &errorCode) {
    ফেরত ucasemap_mapUTF8(
        UCASE_LOC_মূল, options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, destCapacity,
        src, src_দৈর্ঘ্য,
        ucasemap_internalUTF8Fold, edits, errorCode);
}

U_NAMESPACE_END
