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

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"
#ଅନ୍ତର୍ଭାଵ "unicode/brkiter.h"
#ଅନ୍ତର୍ଭାଵ "unicode/bytestream.h"
#ଅନ୍ତର୍ଭାଵ "unicode/casemap.h"
#ଅନ୍ତର୍ଭାଵ "unicode/edits.h"
#ଅନ୍ତର୍ଭାଵ "unicode/stringoptions.h"
#ଅନ୍ତର୍ଭାଵ "unicode/stringpiece.h"
#ଅନ୍ତର୍ଭାଵ "unicode/ubrk.h"
#ଅନ୍ତର୍ଭାଵ "unicode/uloc.h"
#ଅନ୍ତର୍ଭାଵ "unicode/ustring.h"
#ଅନ୍ତର୍ଭାଵ "unicode/ucasemap.h"
#ଯଦି !UCONFIG_NO_BREAK_ITERATION
#ଅନ୍ତର୍ଭାଵ "unicode/utext.h"
#endif
#ଅନ୍ତର୍ଭାଵ "unicode/utf.h"
#ଅନ୍ତର୍ଭାଵ "unicode/utf8.h"
#ଅନ୍ତର୍ଭାଵ "unicode/utf16.h"
#ଅନ୍ତର୍ଭାଵ "bytesinkutil.h"
#ଅନ୍ତର୍ଭାଵ "cmemory.h"
#ଅନ୍ତର୍ଭାଵ "cstring.h"
#ଅନ୍ତର୍ଭାଵ "uassert.h"
#ଅନ୍ତର୍ଭାଵ "ucase.h"
#ଅନ୍ତର୍ଭାଵ "ucasemap_imp.h"

U_NAMESPACE_USE

/* UCaseMap service object -------------------------------------------------- */

// The public type, unicode/ucasemap.h struct UCaseMap, is an opaque C type.
// We define a parallel C++ type in the ICU namespace for entry point renaming.

U_NAMESPACE_BEGIN

UCaseMapImpl::UCaseMapImpl(ଅଚଳ ଅକ୍ଷର *localeID, uint32_t opts, UErrorCode *pErrorCode) :
#ଯଦି !UCONFIG_NO_BREAK_ITERATION
        iter(nullptr),
#endif
        caseLocale(UCASE_LOC_UNKNOWN), options(opts) {
    ucasemap_setLocale(reinterpret_cast<UCaseMap *>(ଏହି), localeID, pErrorCode);
}

UCaseMapImpl::~UCaseMapImpl() {
#ଯଦି !UCONFIG_NO_BREAK_ITERATION
    delete iter;
#endif
}

U_NAMESPACE_END

U_CAPI UCaseMap * U_EXPORT2
ucasemap_open(ଅଚଳ ଅକ୍ଷର *locale, uint32_t options, UErrorCode *pErrorCode) {
    ଯଦି(U_FAILURE(*pErrorCode)) {
        ଫେରନ୍ତୁ nullptr;
    }
    UCaseMapImpl *csm = ନୂତନ UCaseMapImpl(locale, options, pErrorCode);
    ଯଦି(csm==nullptr) {
        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
        ଫେରନ୍ତୁ nullptr;
    } ଅନ୍ୟଥା ଯଦି (U_FAILURE(*pErrorCode)) {
        delete csm;
        ଫେରନ୍ତୁ nullptr;
    }
    ଫେରନ୍ତୁ reinterpret_cast<UCaseMap *>(csm);
}

U_CAPI ଶୂନ୍ୟ U_EXPORT2
ucasemap_close(UCaseMap *csm) {
    delete reinterpret_cast<UCaseMapImpl *>(csm);
}

U_CAPI ଅଚଳ ଅକ୍ଷର* U_EXPORT2
ucasemap_getLocale(ଅଚଳ UCaseMap* csm U_LIFETIME_BOUND) {
    ଫେରନ୍ତୁ reinterpret_cast<ଅଚଳ UCaseMapImpl *>(csm)->locale;
}

U_CAPI uint32_t U_EXPORT2
ucasemap_getOptions(ଅଚଳ UCaseMap *csm) {
    ଫେରନ୍ତୁ reinterpret_cast<ଅଚଳ UCaseMapImpl *>(csm)->options;
}

U_CAPI ଶୂନ୍ୟ U_EXPORT2
ucasemap_setLocale(UCaseMap *csm, ଅଚଳ ଅକ୍ଷର *locale, UErrorCode *pErrorCode) {
    ଯଦି(U_FAILURE(*pErrorCode)) {
        ଫେରନ୍ତୁ;
    }
    auto *impl = reinterpret_cast<UCaseMapImpl *>(csm);
    ଯଦି (locale != nullptr && *locale == 0) {
        impl->locale[0] = 0;
        impl->caseLocale = UCASE_LOC_ମୂଳ;
        ଫେରନ୍ତୁ;
    }

    UErrorCode bufferStatus = U_ZERO_ERROR;
    int32_t length=uloc_getName(locale, impl->locale, (int32_t)ଆକାର(impl->locale), &bufferStatus);
    ଯଦି(bufferStatus==U_BUFFER_OVERFLOW_ERROR || (U_SUCCESS(bufferStatus) && length==ଆକାର(impl->locale))) {
        bufferStatus = U_ZERO_ERROR;
        /* we only really need the language code for case mappings */
        length=uloc_getLanguage(locale, impl->locale, (int32_t)ଆକାର(impl->locale), &bufferStatus);
    }
    ଯଦି(U_FAILURE(bufferStatus)) {
        *pErrorCode=bufferStatus;
    } ଅନ୍ୟଥା ଯଦି(length==ଆକାର(impl->locale)) {
        *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
    }
    ଯଦି(U_SUCCESS(*pErrorCode)) {     
        impl->caseLocale = ucase_getCaseLocale(impl->locale);
    } ଅନ୍ୟଥା {
        impl->locale[0]=0;
        impl->caseLocale = UCASE_LOC_ମୂଳ;
    }
}

U_CAPI ଶୂନ୍ୟ U_EXPORT2
ucasemap_setOptions(UCaseMap *csm, uint32_t options, UErrorCode *pErrorCode) {
    ଯଦି(U_FAILURE(*pErrorCode)) {
        ଫେରନ୍ତୁ;
    }
    reinterpret_cast<UCaseMapImpl *>(csm)->options=options;
}

/* UTF-8 string case mappings ----------------------------------------------- */

/* TODO(markus): Move to a new, separate utf8case.cpp file. */

ନାମାକାଶ {

/* append a full case mapping result, see UCASE_MAX_STRING_LENGTH */
ଧାଡ଼ିଭିତରେ UBool
appendResult(int32_t cpLength, int32_t result, ଅଚଳ char16_t *s,
             ByteSink &sink, uint32_t options, icu::Edits *edits, UErrorCode &errorCode) {
    U_ASSERT(U_SUCCESS(errorCode));

    /* decode the result */
    ଯଦି(result<0) {
        /* (not) original code point */
        ଯଦି(edits!=nullptr) {
            edits->addUnchanged(cpLength);
        }
        ଯଦି((options & U_OMIT_UNCHANGED_TEXT) == 0) {
            ByteSinkUtil::appendCodePoint(cpLength, ~result, sink);
        }
    } ଅନ୍ୟଥା {
        ଯଦି(result<=UCASE_MAX_STRING_LENGTH) {
            // string: "result" is the UTF-16 length
            ଫେରନ୍ତୁ ByteSinkUtil::appendChange(cpLength, s, result, sink, edits, errorCode);
        } ଅନ୍ୟଥା {
            ByteSinkUtil::appendCodePoint(cpLength, result, sink, edits);
        }
    }
    ଫେରନ୍ତୁ ସତ୍ୟ;
}

// See unicode/utf8.h U8_APPEND_UNSAFE().
ଧାଡ଼ିଭିତରେ uint8_t getTwoByteLead(UChar32 c) { ଫେରନ୍ତୁ static_cast<uint8_t>((c >> 6) | 0xc0); }
ଧାଡ଼ିଭିତରେ uint8_t getTwoByteTrail(UChar32 c) { ଫେରନ୍ତୁ static_cast<uint8_t>((c & 0x3f) | 0x80); }

UChar32 U_CALLCONV
utf8_caseContextIterator(ଶୂନ୍ୟ *context, int8_t dir) {
    UCaseContext* csc = static_cast<UCaseContext*>(context);
    UChar32 c;

    ଯଦି(dir<0) {
        /* reset for backward iteration */
        csc->index=csc->cpStart;
        csc->dir=dir;
    } ଅନ୍ୟଥା ଯଦି(dir>0) {
        /* reset for forward iteration */
        csc->index=csc->cpLimit;
        csc->dir=dir;
    } ଅନ୍ୟଥା {
        /* continue current iteration direction */
        dir=csc->dir;
    }

    ଯଦି(dir<0) {
        ଯଦି(csc->start<csc->index) {
            U8_PREV((ଅଚଳ uint8_t *)csc->p, csc->start, csc->index, c);
            ଫେରନ୍ତୁ c;
        }
    } ଅନ୍ୟଥା {
        ଯଦି(csc->index<csc->limit) {
            U8_NEXT((ଅଚଳ uint8_t *)csc->p, csc->index, csc->limit, c);
            ଫେରନ୍ତୁ c;
        }
    }
    ଫେରନ୍ତୁ U_SENTINEL;
}

/**
 * caseLocale >= 0: Lowercases [srcStart..srcLimit[ but takes context [0..srcLength[ into account.
 * caseLocale < 0: Case-folds [srcStart..srcLimit[.
 */
ଶୂନ୍ୟ toLower(int32_t caseLocale, uint32_t options,
             ଅଚଳ uint8_t *src, UCaseContext *csc, int32_t srcStart, int32_t srcLimit,
             icu::ByteSink &sink, icu::Edits *edits, UErrorCode &errorCode) {
    ଅଚଳ int8_t *latinToLower;
    ଯଦି (caseLocale == UCASE_LOC_ମୂଳ ||
            (caseLocale >= 0 ?
                !(caseLocale == UCASE_LOC_TURKISH || caseLocale == UCASE_LOC_LITHUANIAN) :
                (options & _FOLD_CASE_OPTIONS_MASK) == U_FOLD_CASE_DEFAULT)) {
        latinToLower = LatinCase::TO_LOWER_NORMAL;
    } ଅନ୍ୟଥା {
        latinToLower = LatinCase::TO_LOWER_TR_LT;
    }
    ଅଚଳ UTrie2 *trie = ucase_getTrie();
    int32_t prev = srcStart;
    int32_t srcIndex = srcStart;
    ପାଇଁ (;;) {
        // fast path for simple cases
        int32_t cpStart;
        UChar32 c;
        ପାଇଁ (;;) {
            ଯଦି (U_FAILURE(errorCode) || srcIndex >= srcLimit) {
                c = U_SENTINEL;
                ଭାଙ୍ଗ;
            }
            uint8_t lead = src[srcIndex++];
            ଯଦି (lead <= 0x7f) {
                int8_t d = latinToLower[lead];
                ଯଦି (d == LatinCase::EXC) {
                    cpStart = srcIndex - 1;
                    c = lead;
                    ଭାଙ୍ଗ;
                }
                ଯଦି (d == 0) { ଜାରି; }
                ByteSinkUtil::appendUnchanged(src + prev, srcIndex - 1 - prev,
                                              sink, options, edits, errorCode);
                ଅକ୍ଷର ascii = static_cast<ଅକ୍ଷର>(lead + d);
                sink.Append(&ascii, 1);
                ଯଦି (edits != nullptr) {
                    edits->addReplace(1, 1);
                }
                prev = srcIndex;
                ଜାରି;
            } ଅନ୍ୟଥା ଯଦି (lead < 0xe3) {
                uint8_t t;
                ଯଦି (0xc2 <= lead && lead <= 0xc5 && srcIndex < srcLimit &&
                        (t = src[srcIndex] - 0x80) <= 0x3f) {
                    // U+0080..U+017F
                    ++srcIndex;
                    c = ((lead - 0xc0) << 6) | t;
                    int8_t d = latinToLower[c];
                    ଯଦି (d == LatinCase::EXC) {
                        cpStart = srcIndex - 2;
                        ଭାଙ୍ଗ;
                    }
                    ଯଦି (d == 0) { ଜାରି; }
                    ByteSinkUtil::appendUnchanged(src + prev, srcIndex - 2 - prev,
                                                  sink, options, edits, errorCode);
                    ByteSinkUtil::appendTwoBytes(c + d, sink);
                    ଯଦି (edits != nullptr) {
                        edits->addReplace(2, 2);
                    }
                    prev = srcIndex;
                    ଜାରି;
                }
            } ଅନ୍ୟଥା ଯଦି ((lead <= 0xe9 || lead == 0xeb || lead == 0xec) &&
                    (srcIndex + 2) <= srcLimit &&
                    U8_IS_TRAIL(src[srcIndex]) && U8_IS_TRAIL(src[srcIndex + 1])) {
                // most of CJK: no case mappings
                srcIndex += 2;
                ଜାରି;
            }
            cpStart = --srcIndex;
            U8_NEXT(src, srcIndex, srcLimit, c);
            ଯଦି (c < 0) {
                // ill-formed UTF-8
                ଜାରି;
            }
            uint16_t props = UTRIE2_GET16(trie, c);
            ଯଦି (UCASE_HAS_EXCEPTION(props)) { ଭାଙ୍ଗ; }
            int32_t delta;
            ଯଦି (!UCASE_IS_UPPER_OR_TITLE(props) || (delta = UCASE_GET_DELTA(props)) == 0) {
                ଜାରି;
            }
            ByteSinkUtil::appendUnchanged(src + prev, cpStart - prev,
                                          sink, options, edits, errorCode);
            ByteSinkUtil::appendCodePoint(srcIndex - cpStart, c + delta, sink, edits);
            prev = srcIndex;
        }
        ଯଦି (c < 0) {
            ଭାଙ୍ଗ;
        }
        // slow path
        ଅଚଳ char16_t *s;
        ଯଦି (caseLocale >= 0) {
            csc->cpStart = cpStart;
            csc->cpLimit = srcIndex;
            c = ucase_toFullLower(c, utf8_caseContextIterator, csc, &s, caseLocale);
        } ଅନ୍ୟଥା {
            c = ucase_toFullFolding(c, &s, options);
        }
        ଯଦି (c >= 0) {
            ByteSinkUtil::appendUnchanged(src + prev, cpStart - prev,
                                          sink, options, edits, errorCode);
            appendResult(srcIndex - cpStart, c, s, sink, options, edits, errorCode);
            prev = srcIndex;
        }
    }
    ByteSinkUtil::appendUnchanged(src + prev, srcIndex - prev,
                                  sink, options, edits, errorCode);
}

ଶୂନ୍ୟ toUpper(int32_t caseLocale, uint32_t options,
             ଅଚଳ uint8_t *src, UCaseContext *csc, int32_t srcLength,
             icu::ByteSink &sink, icu::Edits *edits, UErrorCode &errorCode) {
    ଅଚଳ int8_t *latinToUpper;
    ଯଦି (caseLocale == UCASE_LOC_TURKISH) {
        latinToUpper = LatinCase::TO_UPPER_TR;
    } ଅନ୍ୟଥା {
        latinToUpper = LatinCase::TO_UPPER_NORMAL;
    }
    ଅଚଳ UTrie2 *trie = ucase_getTrie();
    int32_t prev = 0;
    int32_t srcIndex = 0;
    ପାଇଁ (;;) {
        // fast path for simple cases
        int32_t cpStart;
        UChar32 c;
        ପାଇଁ (;;) {
            ଯଦି (U_FAILURE(errorCode) || srcIndex >= srcLength) {
                c = U_SENTINEL;
                ଭାଙ୍ଗ;
            }
            uint8_t lead = src[srcIndex++];
            ଯଦି (lead <= 0x7f) {
                int8_t d = latinToUpper[lead];
                ଯଦି (d == LatinCase::EXC) {
                    cpStart = srcIndex - 1;
                    c = lead;
                    ଭାଙ୍ଗ;
                }
                ଯଦି (d == 0) { ଜାରି; }
                ByteSinkUtil::appendUnchanged(src + prev, srcIndex - 1 - prev,
                                              sink, options, edits, errorCode);
                ଅକ୍ଷର ascii = static_cast<ଅକ୍ଷର>(lead + d);
                sink.Append(&ascii, 1);
                ଯଦି (edits != nullptr) {
                    edits->addReplace(1, 1);
                }
                prev = srcIndex;
                ଜାରି;
            } ଅନ୍ୟଥା ଯଦି (lead < 0xe3) {
                uint8_t t;
                ଯଦି (0xc2 <= lead && lead <= 0xc5 && srcIndex < srcLength &&
                        (t = src[srcIndex] - 0x80) <= 0x3f) {
                    // U+0080..U+017F
                    ++srcIndex;
                    c = ((lead - 0xc0) << 6) | t;
                    int8_t d = latinToUpper[c];
                    ଯଦି (d == LatinCase::EXC) {
                        cpStart = srcIndex - 2;
                        ଭାଙ୍ଗ;
                    }
                    ଯଦି (d == 0) { ଜାରି; }
                    ByteSinkUtil::appendUnchanged(src + prev, srcIndex - 2 - prev,
                                                  sink, options, edits, errorCode);
                    ByteSinkUtil::appendTwoBytes(c + d, sink);
                    ଯଦି (edits != nullptr) {
                        edits->addReplace(2, 2);
                    }
                    prev = srcIndex;
                    ଜାରି;
                }
            } ଅନ୍ୟଥା ଯଦି ((lead <= 0xe9 || lead == 0xeb || lead == 0xec) &&
                    (srcIndex + 2) <= srcLength &&
                    U8_IS_TRAIL(src[srcIndex]) && U8_IS_TRAIL(src[srcIndex + 1])) {
                // most of CJK: no case mappings
                srcIndex += 2;
                ଜାରି;
            }
            cpStart = --srcIndex;
            U8_NEXT(src, srcIndex, srcLength, c);
            ଯଦି (c < 0) {
                // ill-formed UTF-8
                ଜାରି;
            }
            uint16_t props = UTRIE2_GET16(trie, c);
            ଯଦି (UCASE_HAS_EXCEPTION(props)) { ଭାଙ୍ଗ; }
            int32_t delta;
            ଯଦି (UCASE_GET_TYPE(props) != UCASE_LOWER || (delta = UCASE_GET_DELTA(props)) == 0) {
                ଜାରି;
            }
            ByteSinkUtil::appendUnchanged(src + prev, cpStart - prev,
                                          sink, options, edits, errorCode);
            ByteSinkUtil::appendCodePoint(srcIndex - cpStart, c + delta, sink, edits);
            prev = srcIndex;
        }
        ଯଦି (c < 0) {
            ଭାଙ୍ଗ;
        }
        // slow path
        csc->cpStart = cpStart;
        csc->cpLimit = srcIndex;
        ଅଚଳ char16_t *s;
        c = ucase_toFullUpper(c, utf8_caseContextIterator, csc, &s, caseLocale);
        ଯଦି (c >= 0) {
            ByteSinkUtil::appendUnchanged(src + prev, cpStart - prev,
                                          sink, options, edits, errorCode);
            appendResult(srcIndex - cpStart, c, s, sink, options, edits, errorCode);
            prev = srcIndex;
        }
    }
    ByteSinkUtil::appendUnchanged(src + prev, srcIndex - prev,
                                  sink, options, edits, errorCode);
}

}  // namespace

#ଯଦି !UCONFIG_NO_BREAK_ITERATION

ନାମାକାଶ {

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
int32_t maybeTitleDutchIJ(ଅଚଳ uint8_t *src, UChar32 c, int32_t start, int32_t segmentLimit,
                          ByteSink &sink, uint32_t options, icu::Edits *edits, UErrorCode &errorCode) {
    U_ASSERT(start < segmentLimit);

    int32_t index = start;
    ତାର୍କିକ withAcute = ମିଥ୍ୟା;

    // If the conditions are met, then the following variables tell us what to output.
    int32_t unchanged1 = 0;  // code units before the j, or the whole sequence (0..3)
    ତାର୍କିକ doTitleJ = ମିଥ୍ୟା;  // true if the j needs to be titlecased
    int32_t unchanged2 = 0;  // after the j (0 or 1)

    // next character after the first letter
    UChar32 c2;
    c2 = src[index++];

    // Is the first letter an i/I with accent?
    ଯଦି (c == u'I') {
        ଯଦି (c2 == ACUTE_BYTE0 && index < segmentLimit && src[index++] == ACUTE_BYTE1) {
            withAcute = ସତ୍ୟ;
            unchanged1 = 2;  // ACUTE is 2 code units in UTF-8
            ଯଦି (index == segmentLimit) { ଫେରନ୍ତୁ start; }
            c2 = src[index++];
        }
    } ଅନ୍ୟଥା {  // Í
        withAcute = ସତ୍ୟ;
    }

    // Is the next character a j/J?
    ଯଦି (c2 == u'j') {
        doTitleJ = ସତ୍ୟ;
    } ଅନ୍ୟଥା ଯଦି (c2 == u'J') {
        ++unchanged1;
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ start;
    }

    // A plain i/I must be followed by a plain j/J.
    // An i/I with acute must be followed by a j/J with acute.
    ଯଦି (withAcute) {
        ଯଦି ((index + 1) >= segmentLimit || src[index++] != ACUTE_BYTE0 || src[index++] != ACUTE_BYTE1) {
            ଫେରନ୍ତୁ start;
        }
        ଯଦି (doTitleJ) {
            unchanged2 = 2;  // ACUTE is 2 code units in UTF-8
        } ଅନ୍ୟଥା {
            unchanged1 = unchanged1 + 2;    // ACUTE is 2 code units in UTF-8
        }
    }

    // There must not be another combining mark.
    ଯଦି (index < segmentLimit) {
        int32_t cp;
        int32_t i = index;
        U8_NEXT(src, i, segmentLimit, cp);
        uint32_t typeMask = U_GET_GC_MASK(cp);
        ଯଦି ((typeMask & U_GC_M_MASK) != 0) {
            ଫେରନ୍ତୁ start;
        }
    }

    // Output the rest of the Dutch IJ.
    ByteSinkUtil::appendUnchanged(src + start, unchanged1, sink, options, edits, errorCode);
    start += unchanged1;
    ଯଦି (doTitleJ) {
        ByteSinkUtil::appendCodePoint(1, u'J', sink, edits);
        ++start;
    }
    ByteSinkUtil::appendUnchanged(src + start, unchanged2, sink, options, edits, errorCode);

    U_ASSERT(start + unchanged2 == index);
    ଫେରନ୍ତୁ index;
}

}  // namespace

U_CFUNC ଶୂନ୍ୟ U_CALLCONV
ucasemap_internalUTF8ToTitle(
        int32_t caseLocale, uint32_t options, BreakIterator *iter,
        ଅଚଳ uint8_t *src, int32_t srcLength,
        ByteSink &sink, icu::Edits *edits,
        UErrorCode &errorCode) {
    ଯଦି (!ustrcase_checkTitleAdjustmentOptions(options, errorCode)) {
        ଫେରନ୍ତୁ;
    }

    /* set up local variables */
    UCaseContext csc=UCASECONTEXT_INITIALIZER;
    csc.p=(ଶୂନ୍ୟ *)src;
    csc.limit=srcLength;
    int32_t prev=0;
    UBool isFirstIndex=ସତ୍ୟ;

    /* titlecasing loop */
    ଯେତେବେଳେ(prev<srcLength) {
        /* find next index where to titlecase */
        int32_t index;
        ଯଦି(isFirstIndex) {
            isFirstIndex=ମିଥ୍ୟା;
            index=iter->first();
        } ଅନ୍ୟଥା {
            index=iter->next();
        }
        ଯଦି(index==UBRK_DONE || index>srcLength) {
            index=srcLength;
        }

        /*
         * Segment [prev..index[ into 3 parts:
         * a) skipped characters (copy as-is) [prev..titleStart[
         * b) first letter (titlecase)              [titleStart..titleLimit[
         * c) subsequent characters (lowercase)                 [titleLimit..index[
         */
        ଯଦି(prev<index) {
            /* find and copy skipped characters [prev..titleStart[ */
            int32_t titleStart=prev;
            int32_t titleLimit=prev;
            UChar32 c;
            U8_NEXT(src, titleLimit, index, c);
            ଯଦି ((options&U_TITLECASE_NO_BREAK_ADJUSTMENT)==0) {
                // Adjust the titlecasing index to the next cased character,
                // or to the next letter/number/symbol/private use.
                // Stop with titleStart<titleLimit<=index
                // if there is a character to be titlecased,
                // or else stop with titleStart==titleLimit==index.
                UBool toCased = (options&U_TITLECASE_ADJUST_TO_CASED) != 0;
                ଯେତେବେଳେ (toCased ? UCASE_NONE==ucase_getType(c) : !ustrcase_isLNS(c)) {
                    titleStart=titleLimit;
                    ଯଦି(titleLimit==index) {
                        ଭାଙ୍ଗ;
                    }
                    U8_NEXT(src, titleLimit, index, c);
                }
                ଯଦି (prev < titleStart) {
                    ଯଦି (!ByteSinkUtil::appendUnchanged(src+prev, titleStart-prev,
                                                       sink, options, edits, errorCode)) {
                        ଫେରନ୍ତୁ;
                    }
                }
            }

            ଯଦି(titleStart<titleLimit) {
                /* titlecase c which is from [titleStart..titleLimit[ */
                ଯଦି(c>=0) {
                    csc.cpStart=titleStart;
                    csc.cpLimit=titleLimit;
                    ଅଚଳ char16_t *s;
                    c=ucase_toFullTitle(c, utf8_caseContextIterator, &csc, &s, caseLocale);
                    ଯଦି (!appendResult(titleLimit-titleStart, c, s, sink, options, edits, errorCode)) {
                        ଫେରନ୍ତୁ;
                    }
                } ଅନ୍ୟଥା {
                    // Malformed UTF-8.
                    ଯଦି (!ByteSinkUtil::appendUnchanged(src+titleStart, titleLimit-titleStart,
                                                       sink, options, edits, errorCode)) {
                        ଫେରନ୍ତୁ;
                    }
                }

                /* Special case Dutch IJ titlecasing */
                ଯଦି (titleLimit < index &&
                    caseLocale == UCASE_LOC_DUTCH) {
                    ଯଦି (c < 0) {
                        c = ~c;
                    }

                    ଯଦି (c == u'I' || c == u'Í') {
                        titleLimit = maybeTitleDutchIJ(src, c, titleLimit, index, sink, options, edits, errorCode);
                    }
                }

                /* lowercase [titleLimit..index[ */
                ଯଦି(titleLimit<index) {
                    ଯଦି((options&U_TITLECASE_NO_LOWERCASE)==0) {
                        /* Normal operation: Lowercase the rest of the word. */
                        toLower(caseLocale, options,
                                src, &csc, titleLimit, index,
                                sink, edits, errorCode);
                        ଯଦି(U_FAILURE(errorCode)) {
                            ଫେରନ୍ତୁ;
                        }
                    } ଅନ୍ୟଥା {
                        /* Optionally just copy the rest of the word unchanged. */
                        ଯଦି (!ByteSinkUtil::appendUnchanged(src+titleLimit, index-titleLimit,
                                                           sink, options, edits, errorCode)) {
                            ଫେରନ୍ତୁ;
                        }
                    }
                }
            }
        }

        prev=index;
    }
}

#endif

U_NAMESPACE_BEGIN
ନାମାକାଶ GreekUpper {

UBool isFollowedByCasedLetter(ଅଚଳ uint8_t *s, int32_t i, int32_t length) {
    ଯେତେବେଳେ (i < length) {
        UChar32 c;
        U8_NEXT(s, i, length, c);
        int32_t type = ucase_getTypeOrIgnorable(c);
        ଯଦି ((type & UCASE_IGNORABLE) != 0) {
            // Case-ignorable, continue with the loop.
        } ଅନ୍ୟଥା ଯଦି (type != UCASE_NONE) {
            ଫେରନ୍ତୁ ସତ୍ୟ;  // Followed by cased letter.
        } ଅନ୍ୟଥା {
            ଫେରନ୍ତୁ ମିଥ୍ୟା;  // Uncased and not case-ignorable.
        }
    }
    ଫେରନ୍ତୁ ମିଥ୍ୟା;  // Not followed by cased letter.
}

// Keep this consistent with the UTF-16 version in ustrcase.cpp and the Java version in CaseMap.java.
ଶୂନ୍ୟ toUpper(uint32_t options,
             ଅଚଳ uint8_t *src, int32_t srcLength,
             ByteSink &sink, Edits *edits,
             UErrorCode &errorCode) {
    uint32_t state = 0;
    ପାଇଁ (int32_t i = 0; i < srcLength;) {
        int32_t nextIndex = i;
        UChar32 c;
        U8_NEXT(src, nextIndex, srcLength, c);
        uint32_t nextState = 0;
        int32_t type = ucase_getTypeOrIgnorable(c);
        ଯଦି ((type & UCASE_IGNORABLE) != 0) {
            // c is case-ignorable
            nextState |= (state & AFTER_CASED);
        } ଅନ୍ୟଥା ଯଦି (type != UCASE_NONE) {
            // c is cased
            nextState |= AFTER_CASED;
        }
        uint32_t data = getLetterData(c);
        ଯଦି (data > 0) {
            uint32_t upper = data & UPPER_MASK;
            // Add a dialytika to this iota or ypsilon vowel
            // if we removed a tonos from the previous vowel,
            // and that previous vowel did not also have (or gain) a dialytika.
            // Adding one only to the final vowel in a longer sequence
            // (which does not occur in normal writing) would require lookahead.
            // Set the same flag as for preserving an existing dialytika.
            ଯଦି ((data & HAS_VOWEL) != 0 &&
                (state & (AFTER_VOWEL_WITH_PRECOMPOSED_ACCENT | AFTER_VOWEL_WITH_COMBINING_ACCENT)) !=
                    0 &&
                (upper == 0x399 || upper == 0x3A5)) {
                data |= (state & AFTER_VOWEL_WITH_PRECOMPOSED_ACCENT) != 0 ? HAS_DIALYTIKA
                                                                           : HAS_COMBINING_DIALYTIKA;
            }
            int32_t ଅଙ୍କ_Ypogegrammeni = 0;  // Map each one to a trailing, spacing, capital iota.
            ଯଦି ((data & HAS_YPOGEGRAMMENI) != 0) {
                ଅଙ୍କ_Ypogegrammeni = 1;
            }
            ଅଚଳ UBool hasPrecomposedAccent = (data & HAS_ACCENT) != 0;
            // Skip combining diacritics after this Greek letter.
            int32_t nextNextIndex = nextIndex;
            ଯେତେବେଳେ (nextIndex < srcLength) {
                UChar32 c2;
                U8_NEXT(src, nextNextIndex, srcLength, c2);
                uint32_t diacriticData = getDiacriticData(c2);
                ଯଦି (diacriticData != 0) {
                    data |= diacriticData;
                    ଯଦି ((diacriticData & HAS_YPOGEGRAMMENI) != 0) {
                        ++ଅଙ୍କ_Ypogegrammeni;
                    }
                    nextIndex = nextNextIndex;
                } ଅନ୍ୟଥା {
                    ଭାଙ୍ଗ;  // not a Greek diacritic
                }
            }
            ଯଦି ((data & HAS_VOWEL_AND_ACCENT_AND_DIALYTIKA) == HAS_VOWEL_AND_ACCENT) {
                nextState |= hasPrecomposedAccent ? AFTER_VOWEL_WITH_PRECOMPOSED_ACCENT
                                                  : AFTER_VOWEL_WITH_COMBINING_ACCENT;
            }
            // Map according to Greek rules.
            UBool addTonos = ମିଥ୍ୟା;
            ଯଦି (upper == 0x397 &&
                    (data & HAS_ACCENT) != 0 &&
                    ଅଙ୍କ_Ypogegrammeni == 0 &&
                    (state & AFTER_CASED) == 0 &&
                    !isFollowedByCasedLetter(src, nextIndex, srcLength)) {
                // Keep disjunctive "or" with (only) a tonos.
                // We use the same "word boundary" conditions as for the Final_Sigma test.
                ଯଦି (hasPrecomposedAccent) {
                    upper = 0x389;  // Preserve the precomposed form.
                } ଅନ୍ୟଥା {
                    addTonos = ସତ୍ୟ;
                }
            } ଅନ୍ୟଥା ଯଦି ((data & HAS_DIALYTIKA) != 0) {
                // Preserve a vowel with dialytika in precomposed form if it exists.
                ଯଦି (upper == 0x399) {
                    upper = 0x3AA;
                    data &= ~HAS_EITHER_DIALYTIKA;
                } ଅନ୍ୟଥା ଯଦି (upper == 0x3A5) {
                    upper = 0x3AB;
                    data &= ~HAS_EITHER_DIALYTIKA;
                }
            }

            UBool change;
            ଯଦି (edits == nullptr && (options & U_OMIT_UNCHANGED_TEXT) == 0) {
                change = ସତ୍ୟ;  // common, simple usage
            } ଅନ୍ୟଥା {
                // Find out first whether we are changing the text.
                U_ASSERT(0x370 <= upper && upper <= 0x3ff);  // 2-byte UTF-8, main Greek block
                change = (i + 2) > nextIndex ||
                        src[i] != getTwoByteLead(upper) || src[i + 1] != getTwoByteTrail(upper) ||
                        ଅଙ୍କ_Ypogegrammeni > 0;
                int32_t i2 = i + 2;
                ଯଦି ((data & HAS_EITHER_DIALYTIKA) != 0) {
                    change |= (i2 + 2) > nextIndex ||
                            src[i2] != static_cast<uint8_t>(u8"\u0308"[0]) ||
                            src[i2 + 1] != static_cast<uint8_t>(u8"\u0308"[1]);
                    i2 += 2;
                }
                ଯଦି (addTonos) {
                    change |= (i2 + 2) > nextIndex ||
                            src[i2] != static_cast<uint8_t>(u8"\u0301"[0]) ||
                            src[i2 + 1] != static_cast<uint8_t>(u8"\u0301"[1]);
                    i2 += 2;
                }
                int32_t oldLength = nextIndex - i;
                int32_t newLength = (i2 - i) + ଅଙ୍କ_Ypogegrammeni * 2;  // 2 bytes per U+0399
                change |= oldLength != newLength;
                ଯଦି (change) {
                    ଯଦି (edits != nullptr) {
                        edits->addReplace(oldLength, newLength);
                    }
                } ଅନ୍ୟଥା {
                    ଯଦି (edits != nullptr) {
                        edits->addUnchanged(oldLength);
                    }
                    // Write unchanged text?
                    change = (options & U_OMIT_UNCHANGED_TEXT) == 0;
                }
            }

            ଯଦି (change) {
                ByteSinkUtil::appendTwoBytes(upper, sink);
                ଯଦି ((data & HAS_EITHER_DIALYTIKA) != 0) {
                    sink.AppendU8(u8"\u0308", 2);  // restore or add a dialytika
                }
                ଯଦି (addTonos) {
                    sink.AppendU8(u8"\u0301", 2);
                }
                ଯେତେବେଳେ (ଅଙ୍କ_Ypogegrammeni > 0) {
                    sink.AppendU8(u8"\u0399", 2);
                    --ଅଙ୍କ_Ypogegrammeni;
                }
            }
        } ଅନ୍ୟଥା ଯଦି(c>=0) {
            ଅଚଳ char16_t *s;
            c=ucase_toFullUpper(c, nullptr, nullptr, &s, UCASE_LOC_GREEK);
            ଯଦି (!appendResult(nextIndex - i, c, s, sink, options, edits, errorCode)) {
                ଫେରନ୍ତୁ;
            }
        } ଅନ୍ୟଥା {
            // Malformed UTF-8.
            ଯଦି (!ByteSinkUtil::appendUnchanged(src+i, nextIndex-i,
                                               sink, options, edits, errorCode)) {
                ଫେରନ୍ତୁ;
            }
        }
        i = nextIndex;
        state = nextState;
    }
}

}  // namespace GreekUpper
U_NAMESPACE_END

ସ୍ଥିର ଶୂନ୍ୟ U_CALLCONV
ucasemap_internalUTF8ToLower(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_UNUSED
                             ଅଚଳ uint8_t *src, int32_t srcLength,
                             icu::ByteSink &sink, icu::Edits *edits,
                             UErrorCode &errorCode) {
    UCaseContext csc=UCASECONTEXT_INITIALIZER;
    csc.p=(ଶୂନ୍ୟ *)src;
    csc.limit=srcLength;
    toLower(
        caseLocale, options,
        src, &csc, 0, srcLength,
        sink, edits, errorCode);
}

ସ୍ଥିର ଶୂନ୍ୟ U_CALLCONV
ucasemap_internalUTF8ToUpper(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_UNUSED
                             ଅଚଳ uint8_t *src, int32_t srcLength,
                             icu::ByteSink &sink, icu::Edits *edits,
                             UErrorCode &errorCode) {
    ଯଦି (caseLocale == UCASE_LOC_GREEK) {
        GreekUpper::toUpper(options, src, srcLength, sink, edits, errorCode);
    } ଅନ୍ୟଥା {
        UCaseContext csc=UCASECONTEXT_INITIALIZER;
        csc.p=(ଶୂନ୍ୟ *)src;
        csc.limit=srcLength;
        toUpper(
            caseLocale, options,
            src, &csc, srcLength,
            sink, edits, errorCode);
    }
}

ସ୍ଥିର ଶୂନ୍ୟ U_CALLCONV
ucasemap_internalUTF8Fold(int32_t /* caseLocale */, uint32_t options, UCASEMAP_BREAK_ITERATOR_UNUSED
                          ଅଚଳ uint8_t *src, int32_t srcLength,
                          icu::ByteSink &sink, icu::Edits *edits,
                          UErrorCode &errorCode) {
    toLower(
        -1, options,
        src, nullptr, 0, srcLength,
        sink, edits, errorCode);
}

ଶୂନ୍ୟ
ucasemap_mapUTF8(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_PARAM
                 ଅଚଳ ଅକ୍ଷର *src, int32_t srcLength,
                 UTF8CaseMapper *stringCaseMapper,
                 icu::ByteSink &sink, icu::Edits *edits,
                 UErrorCode &errorCode) {
    /* check argument values */
    ଯଦି (U_FAILURE(errorCode)) {
        ଫେରନ୍ତୁ;
    }
    ଯଦି ((src == nullptr && srcLength != 0) || srcLength < -1) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ;
    }

    // Get the string length.
    ଯଦି (srcLength == -1) {
        srcLength = static_cast<int32_t>(uprv_strlen(src));
    }

    ଯଦି (edits != nullptr && (options & U_EDITS_NO_RESET) == 0) {
        edits->reset();
    }
    stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                     reinterpret_cast<ଅଚଳ uint8_t*>(src), srcLength, sink, edits, errorCode);
    sink.Flush();
    ଯଦି (U_SUCCESS(errorCode)) {
        ଯଦି (edits != nullptr) {
            edits->copyErrorTo(errorCode);
        }
    }
}

int32_t
ucasemap_mapUTF8(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_PARAM
                 ଅକ୍ଷର *dest, int32_t destCapacity,
                 ଅଚଳ ଅକ୍ଷର *src, int32_t srcLength,
                 UTF8CaseMapper *stringCaseMapper,
                 icu::Edits *edits,
                 UErrorCode &errorCode) {
    /* check argument values */
    ଯଦି(U_FAILURE(errorCode)) {
        ଫେରନ୍ତୁ 0;
    }
    ଯଦି( destCapacity<0 ||
        (dest==nullptr && destCapacity>0) ||
        (src==nullptr && srcLength!=0) || srcLength<-1
    ) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ 0;
    }

    /* get the string length */
    ଯଦି(srcLength==-1) {
        srcLength = static_cast<int32_t>(uprv_strlen(src));
    }

    /* check for overlapping source and destination */
    ଯଦି( dest!=nullptr &&
        ((src>=dest && src<(dest+destCapacity)) ||
         (dest>=src && dest<(src+srcLength)))
    ) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ 0;
    }

    ଯଦି (edits != nullptr && (options & U_EDITS_NO_RESET) == 0) {
        edits->reset();
    }
    int32_t reslen = ByteSinkUtil::viaByteSinkToTerminatedChars(
        dest, destCapacity,
        [&](ByteSink& sink, UErrorCode& status) {
            stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                             reinterpret_cast<ଅଚଳ uint8_t*>(src), srcLength, sink, edits, status);
        },
        errorCode);
    ଯଦି (U_SUCCESS(errorCode) && edits != nullptr) {
        edits->copyErrorTo(errorCode);
    }
    ଫେରନ୍ତୁ reslen;
}

/* public API functions */

U_CAPI int32_t U_EXPORT2
ucasemap_utf8ToLower(ଅଚଳ UCaseMap *csm,
                     ଅକ୍ଷର *dest, int32_t destCapacity,
                     ଅଚଳ ଅକ୍ଷର *src, int32_t srcLength,
                     UErrorCode *pErrorCode) {
    auto *impl = reinterpret_cast<ଅଚଳ UCaseMapImpl *>(csm);
    ଫେରନ୍ତୁ ucasemap_mapUTF8(
        impl->caseLocale, impl->options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, destCapacity,
        src, srcLength,
        ucasemap_internalUTF8ToLower, nullptr, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
ucasemap_utf8ToUpper(ଅଚଳ UCaseMap *csm,
                     ଅକ୍ଷର *dest, int32_t destCapacity,
                     ଅଚଳ ଅକ୍ଷର *src, int32_t srcLength,
                     UErrorCode *pErrorCode) {
    auto *impl = reinterpret_cast<ଅଚଳ UCaseMapImpl *>(csm);
    ଫେରନ୍ତୁ ucasemap_mapUTF8(
        impl->caseLocale, impl->options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, destCapacity,
        src, srcLength,
        ucasemap_internalUTF8ToUpper, nullptr, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
ucasemap_utf8FoldCase(ଅଚଳ UCaseMap *csm,
                      ଅକ୍ଷର *dest, int32_t destCapacity,
                      ଅଚଳ ଅକ୍ଷର *src, int32_t srcLength,
                      UErrorCode *pErrorCode) {
    auto *impl = reinterpret_cast<ଅଚଳ UCaseMapImpl *>(csm);
    ଫେରନ୍ତୁ ucasemap_mapUTF8(
        UCASE_LOC_ମୂଳ, impl->options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, destCapacity,
        src, srcLength,
        ucasemap_internalUTF8Fold, nullptr, *pErrorCode);
}

U_NAMESPACE_BEGIN

ଶୂନ୍ୟ CaseMap::utf8ToLower(
        ଅଚଳ ଅକ୍ଷର *locale, uint32_t options,
        StringPiece src, ByteSink &sink, Edits *edits,
        UErrorCode &errorCode) {
    ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        src.data(), src.length(),
        ucasemap_internalUTF8ToLower, sink, edits, errorCode);
}

ଶୂନ୍ୟ CaseMap::utf8ToUpper(
        ଅଚଳ ଅକ୍ଷର *locale, uint32_t options,
        StringPiece src, ByteSink &sink, Edits *edits,
        UErrorCode &errorCode) {
    ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        src.data(), src.length(),
        ucasemap_internalUTF8ToUpper, sink, edits, errorCode);
}

ଶୂନ୍ୟ CaseMap::utf8Fold(
        uint32_t options,
        StringPiece src, ByteSink &sink, Edits *edits,
        UErrorCode &errorCode) {
    ucasemap_mapUTF8(
        UCASE_LOC_ମୂଳ, options, UCASEMAP_BREAK_ITERATOR_NULL
        src.data(), src.length(),
        ucasemap_internalUTF8Fold, sink, edits, errorCode);
}

int32_t CaseMap::utf8ToLower(
        ଅଚଳ ଅକ୍ଷର *locale, uint32_t options,
        ଅଚଳ ଅକ୍ଷର *src, int32_t srcLength,
        ଅକ୍ଷର *dest, int32_t destCapacity, Edits *edits,
        UErrorCode &errorCode) {
    ଫେରନ୍ତୁ ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, destCapacity,
        src, srcLength,
        ucasemap_internalUTF8ToLower, edits, errorCode);
}

int32_t CaseMap::utf8ToUpper(
        ଅଚଳ ଅକ୍ଷର *locale, uint32_t options,
        ଅଚଳ ଅକ୍ଷର *src, int32_t srcLength,
        ଅକ୍ଷର *dest, int32_t destCapacity, Edits *edits,
        UErrorCode &errorCode) {
    ଫେରନ୍ତୁ ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, destCapacity,
        src, srcLength,
        ucasemap_internalUTF8ToUpper, edits, errorCode);
}

int32_t CaseMap::utf8Fold(
        uint32_t options,
        ଅଚଳ ଅକ୍ଷର *src, int32_t srcLength,
        ଅକ୍ଷର *dest, int32_t destCapacity, Edits *edits,
        UErrorCode &errorCode) {
    ଫେରନ୍ତୁ ucasemap_mapUTF8(
        UCASE_LOC_ମୂଳ, options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, destCapacity,
        src, srcLength,
        ucasemap_internalUTF8Fold, edits, errorCode);
}

U_NAMESPACE_END
