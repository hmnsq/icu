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

#include "unicode/utypes.h"
#include "unicode/brkiter.h"
#include "unicode/bytestream.h"
#include "unicode/casemap.h"
#include "unicode/edits.h"
#include "unicode/stringoptions.h"
#include "unicode/stringpiece.h"
#include "unicode/ubrk.h"
#include "unicode/uloc.h"
#include "unicode/ustring.h"
#include "unicode/ucasemap.h"
#ប្រសិនបើ !UCONFIG_NO_BREAK_ITERATION
#include "unicode/utext.h"
#endif
#include "unicode/utf.h"
#include "unicode/utf8.h"
#include "unicode/utf16.h"
#include "bytesinkutil.h"
#include "cmemory.h"
#include "cstring.h"
#include "uassert.h"
#include "ucase.h"
#include "ucasemap_imp.h"

U_NAMESPACE_USE

/* UCaseMap service object -------------------------------------------------- */

// The public type, unicode/ucasemap.h struct UCaseMap, is an opaque C type.
// We define a parallel C++ type in the ICU namespace for entry point renaming.

U_NAMESPACE_BEGIN

UCaseMapImpl::UCaseMapImpl(const var_na *localeID, uint32_t opts, UErrorCode *pErrorCode) :
#ប្រសិនបើ !UCONFIG_NO_BREAK_ITERATION
        iter(nullptr),
#endif
        caseLocale(UCASE_LOC_UNKNOWN), options(opts) {
    ucasemap_setLocale(reinterpret_cast<UCaseMap *>(this), localeID, pErrorCode);
}

UCaseMapImpl::~UCaseMapImpl() {
#ប្រសិនបើ !UCONFIG_NO_BREAK_ITERATION
    delete iter;
#endif
}

U_NAMESPACE_END

U_CAPI UCaseMap * U_EXPORT2
ucasemap_open(const var_na *locale, uint32_t options, UErrorCode *pErrorCode) {
    ប្រសិនបើ(U_FAILURE(*pErrorCode)) {
        ត្រឡប់ nullptr;
    }
    UCaseMapImpl *csm = new UCaseMapImpl(locale, options, pErrorCode);
    ប្រសិនបើ(csm==nullptr) {
        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
        ត្រឡប់ nullptr;
    } បើមិនដូច្នេះ ប្រសិនបើ (U_FAILURE(*pErrorCode)) {
        delete csm;
        ត្រឡប់ nullptr;
    }
    ត្រឡប់ reinterpret_cast<UCaseMap *>(csm);
}

U_CAPI void U_EXPORT2
ucasemap_close(UCaseMap *csm) {
    delete reinterpret_cast<UCaseMapImpl *>(csm);
}

U_CAPI const var_na* U_EXPORT2
ucasemap_getLocale(const UCaseMap* csm U_LIFETIME_BOUND) {
    ត្រឡប់ reinterpret_cast<const UCaseMapImpl *>(csm)->locale;
}

U_CAPI uint32_t U_EXPORT2
ucasemap_getOptions(const UCaseMap *csm) {
    ត្រឡប់ reinterpret_cast<const UCaseMapImpl *>(csm)->options;
}

U_CAPI void U_EXPORT2
ucasemap_setLocale(UCaseMap *csm, const var_na *locale, UErrorCode *pErrorCode) {
    ប្រសិនបើ(U_FAILURE(*pErrorCode)) {
        ត្រឡប់;
    }
    auto *impl = reinterpret_cast<UCaseMapImpl *>(csm);
    ប្រសិនបើ (locale != nullptr && *locale == 0) {
        impl->locale[0] = 0;
        impl->caseLocale = UCASE_LOC_ROOT;
        ត្រឡប់;
    }

    UErrorCode bufferStatus = U_ZERO_ERROR;
    int32_t length=uloc_getName(locale, impl->locale, (int32_t)parimaa_na(impl->locale), &bufferStatus);
    ប្រសិនបើ(bufferStatus==U_BUFFER_OVERFLOW_ERROR || (U_SUCCESS(bufferStatus) && length==parimaa_na(impl->locale))) {
        bufferStatus = U_ZERO_ERROR;
        /* we only really need the language code for case mappings */
        length=uloc_getLanguage(locale, impl->locale, (int32_t)parimaa_na(impl->locale), &bufferStatus);
    }
    ប្រសិនបើ(U_FAILURE(bufferStatus)) {
        *pErrorCode=bufferStatus;
    } បើមិនដូច្នេះ ប្រសិនបើ(length==parimaa_na(impl->locale)) {
        *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
    }
    ប្រសិនបើ(U_SUCCESS(*pErrorCode)) {     
        impl->caseLocale = ucase_getCaseLocale(impl->locale);
    } បើមិនដូច្នេះ {
        impl->locale[0]=0;
        impl->caseLocale = UCASE_LOC_ROOT;
    }
}

U_CAPI void U_EXPORT2
ucasemap_setOptions(UCaseMap *csm, uint32_t options, UErrorCode *pErrorCode) {
    ប្រសិនបើ(U_FAILURE(*pErrorCode)) {
        ត្រឡប់;
    }
    reinterpret_cast<UCaseMapImpl *>(csm)->options=options;
}

/* UTF-8 string case mappings ----------------------------------------------- */

/* TODO(markus): Move to a new, separate utf8case.cpp file. */

namespace {

/* append a full case mapping result, see UCASE_MAX_STRING_LENGTH */
anta_hstha UBool
appendResult(int32_t cpLength, int32_t result, const char16_t *s,
             ByteSink &sink, uint32_t options, icu::Edits *edits, UErrorCode &errorCode) {
    U_ASSERT(U_SUCCESS(errorCode));

    /* decode the result */
    ប្រសិនបើ(result<0) {
        /* (not) original code point */
        ប្រសិនបើ(edits!=nullptr) {
            edits->addUnchanged(cpLength);
        }
        ប្រសិនបើ((options & U_OMIT_UNCHANGED_TEXT) == 0) {
            ByteSinkUtil::appendCodePoint(cpLength, ~result, sink);
        }
    } បើមិនដូច្នេះ {
        ប្រសិនបើ(result<=UCASE_MAX_STRING_LENGTH) {
            // string: "result" is the UTF-16 length
            ត្រឡប់ ByteSinkUtil::appendChange(cpLength, s, result, sink, edits, errorCode);
        } បើមិនដូច្នេះ {
            ByteSinkUtil::appendCodePoint(cpLength, result, sink, edits);
        }
    }
    ត្រឡប់ true;
}

// See unicode/utf8.h U8_APPEND_UNSAFE().
anta_hstha uint8_t getTwoByteLead(UChar32 c) { ត្រឡប់ static_cast<uint8_t>((c >> 6) | 0xc0); }
anta_hstha uint8_t getTwoByteTrail(UChar32 c) { ត្រឡប់ static_cast<uint8_t>((c & 0x3f) | 0x80); }

UChar32 U_CALLCONV
utf8_caseContextIterator(void *context, int8_t dir) {
    UCaseContext* csc = static_cast<UCaseContext*>(context);
    UChar32 c;

    ប្រសិនបើ(dir<0) {
        /* reset for backward iteration */
        csc->index=csc->cpStart;
        csc->dir=dir;
    } បើមិនដូច្នេះ ប្រសិនបើ(dir>0) {
        /* reset for forward iteration */
        csc->index=csc->cpLimit;
        csc->dir=dir;
    } បើមិនដូច្នេះ {
        /* continue current iteration direction */
        dir=csc->dir;
    }

    ប្រសិនបើ(dir<0) {
        ប្រសិនបើ(csc->start<csc->index) {
            U8_PREV((const uint8_t *)csc->p, csc->start, csc->index, c);
            ត្រឡប់ c;
        }
    } បើមិនដូច្នេះ {
        ប្រសិនបើ(csc->index<csc->limit) {
            U8_NEXT((const uint8_t *)csc->p, csc->index, csc->limit, c);
            ត្រឡប់ c;
        }
    }
    ត្រឡប់ U_SENTINEL;
}

/**
 * caseLocale >= 0: Lowercases [srcStart..srcLimit[ but takes context [0..srcLength[ into account.
 * caseLocale < 0: Case-folds [srcStart..srcLimit[.
 */
void toLower(int32_t caseLocale, uint32_t options,
             const uint8_t *src, UCaseContext *csc, int32_t srcStart, int32_t srcLimit,
             icu::ByteSink &sink, icu::Edits *edits, UErrorCode &errorCode) {
    const int8_t *latinToLower;
    ប្រសិនបើ (caseLocale == UCASE_LOC_ROOT ||
            (caseLocale >= 0 ?
                !(caseLocale == UCASE_LOC_TURKISH || caseLocale == UCASE_LOC_LITHUANIAN) :
                (options & _FOLD_CASE_OPTIONS_MASK) == U_FOLD_CASE_DEFAULT)) {
        latinToLower = LatinCase::TO_LOWER_NORMAL;
    } បើមិនដូច្នេះ {
        latinToLower = LatinCase::TO_LOWER_TR_LT;
    }
    const UTrie2 *trie = ucase_getTrie();
    int32_t prev = srcStart;
    int32_t srcIndex = srcStart;
    សម្រាប់ (;;) {
        // fast path for simple cases
        int32_t cpStart;
        UChar32 c;
        សម្រាប់ (;;) {
            ប្រសិនបើ (U_FAILURE(errorCode) || srcIndex >= srcLimit) {
                c = U_SENTINEL;
                bha_nga;
            }
            uint8_t lead = src[srcIndex++];
            ប្រសិនបើ (lead <= 0x7f) {
                int8_t d = latinToLower[lead];
                ប្រសិនបើ (d == LatinCase::EXC) {
                    cpStart = srcIndex - 1;
                    c = lead;
                    bha_nga;
                }
                ប្រសិនបើ (d == 0) { continue; }
                ByteSinkUtil::appendUnchanged(src + prev, srcIndex - 1 - prev,
                                              sink, options, edits, errorCode);
                var_na ascii = static_cast<var_na>(lead + d);
                sink.Append(&ascii, 1);
                ប្រសិនបើ (edits != nullptr) {
                    edits->addReplace(1, 1);
                }
                prev = srcIndex;
                continue;
            } បើមិនដូច្នេះ ប្រសិនបើ (lead < 0xe3) {
                uint8_t t;
                ប្រសិនបើ (0xc2 <= lead && lead <= 0xc5 && srcIndex < srcLimit &&
                        (t = src[srcIndex] - 0x80) <= 0x3f) {
                    // U+0080..U+017F
                    ++srcIndex;
                    c = ((lead - 0xc0) << 6) | t;
                    int8_t d = latinToLower[c];
                    ប្រសិនបើ (d == LatinCase::EXC) {
                        cpStart = srcIndex - 2;
                        bha_nga;
                    }
                    ប្រសិនបើ (d == 0) { continue; }
                    ByteSinkUtil::appendUnchanged(src + prev, srcIndex - 2 - prev,
                                                  sink, options, edits, errorCode);
                    ByteSinkUtil::appendTwoBytes(c + d, sink);
                    ប្រសិនបើ (edits != nullptr) {
                        edits->addReplace(2, 2);
                    }
                    prev = srcIndex;
                    continue;
                }
            } បើមិនដូច្នេះ ប្រសិនបើ ((lead <= 0xe9 || lead == 0xeb || lead == 0xec) &&
                    (srcIndex + 2) <= srcLimit &&
                    U8_IS_TRAIL(src[srcIndex]) && U8_IS_TRAIL(src[srcIndex + 1])) {
                // most of CJK: no case mappings
                srcIndex += 2;
                continue;
            }
            cpStart = --srcIndex;
            U8_NEXT(src, srcIndex, srcLimit, c);
            ប្រសិនបើ (c < 0) {
                // ill-formed UTF-8
                continue;
            }
            uint16_t props = UTRIE2_GET16(trie, c);
            ប្រសិនបើ (UCASE_HAS_EXCEPTION(props)) { bha_nga; }
            int32_t delta;
            ប្រសិនបើ (!UCASE_IS_UPPER_OR_TITLE(props) || (delta = UCASE_GET_DELTA(props)) == 0) {
                continue;
            }
            ByteSinkUtil::appendUnchanged(src + prev, cpStart - prev,
                                          sink, options, edits, errorCode);
            ByteSinkUtil::appendCodePoint(srcIndex - cpStart, c + delta, sink, edits);
            prev = srcIndex;
        }
        ប្រសិនបើ (c < 0) {
            bha_nga;
        }
        // slow path
        const char16_t *s;
        ប្រសិនបើ (caseLocale >= 0) {
            csc->cpStart = cpStart;
            csc->cpLimit = srcIndex;
            c = ucase_toFullLower(c, utf8_caseContextIterator, csc, &s, caseLocale);
        } បើមិនដូច្នេះ {
            c = ucase_toFullFolding(c, &s, options);
        }
        ប្រសិនបើ (c >= 0) {
            ByteSinkUtil::appendUnchanged(src + prev, cpStart - prev,
                                          sink, options, edits, errorCode);
            appendResult(srcIndex - cpStart, c, s, sink, options, edits, errorCode);
            prev = srcIndex;
        }
    }
    ByteSinkUtil::appendUnchanged(src + prev, srcIndex - prev,
                                  sink, options, edits, errorCode);
}

void toUpper(int32_t caseLocale, uint32_t options,
             const uint8_t *src, UCaseContext *csc, int32_t srcLength,
             icu::ByteSink &sink, icu::Edits *edits, UErrorCode &errorCode) {
    const int8_t *latinToUpper;
    ប្រសិនបើ (caseLocale == UCASE_LOC_TURKISH) {
        latinToUpper = LatinCase::TO_UPPER_TR;
    } បើមិនដូច្នេះ {
        latinToUpper = LatinCase::TO_UPPER_NORMAL;
    }
    const UTrie2 *trie = ucase_getTrie();
    int32_t prev = 0;
    int32_t srcIndex = 0;
    សម្រាប់ (;;) {
        // fast path for simple cases
        int32_t cpStart;
        UChar32 c;
        សម្រាប់ (;;) {
            ប្រសិនបើ (U_FAILURE(errorCode) || srcIndex >= srcLength) {
                c = U_SENTINEL;
                bha_nga;
            }
            uint8_t lead = src[srcIndex++];
            ប្រសិនបើ (lead <= 0x7f) {
                int8_t d = latinToUpper[lead];
                ប្រសិនបើ (d == LatinCase::EXC) {
                    cpStart = srcIndex - 1;
                    c = lead;
                    bha_nga;
                }
                ប្រសិនបើ (d == 0) { continue; }
                ByteSinkUtil::appendUnchanged(src + prev, srcIndex - 1 - prev,
                                              sink, options, edits, errorCode);
                var_na ascii = static_cast<var_na>(lead + d);
                sink.Append(&ascii, 1);
                ប្រសិនបើ (edits != nullptr) {
                    edits->addReplace(1, 1);
                }
                prev = srcIndex;
                continue;
            } បើមិនដូច្នេះ ប្រសិនបើ (lead < 0xe3) {
                uint8_t t;
                ប្រសិនបើ (0xc2 <= lead && lead <= 0xc5 && srcIndex < srcLength &&
                        (t = src[srcIndex] - 0x80) <= 0x3f) {
                    // U+0080..U+017F
                    ++srcIndex;
                    c = ((lead - 0xc0) << 6) | t;
                    int8_t d = latinToUpper[c];
                    ប្រសិនបើ (d == LatinCase::EXC) {
                        cpStart = srcIndex - 2;
                        bha_nga;
                    }
                    ប្រសិនបើ (d == 0) { continue; }
                    ByteSinkUtil::appendUnchanged(src + prev, srcIndex - 2 - prev,
                                                  sink, options, edits, errorCode);
                    ByteSinkUtil::appendTwoBytes(c + d, sink);
                    ប្រសិនបើ (edits != nullptr) {
                        edits->addReplace(2, 2);
                    }
                    prev = srcIndex;
                    continue;
                }
            } បើមិនដូច្នេះ ប្រសិនបើ ((lead <= 0xe9 || lead == 0xeb || lead == 0xec) &&
                    (srcIndex + 2) <= srcLength &&
                    U8_IS_TRAIL(src[srcIndex]) && U8_IS_TRAIL(src[srcIndex + 1])) {
                // most of CJK: no case mappings
                srcIndex += 2;
                continue;
            }
            cpStart = --srcIndex;
            U8_NEXT(src, srcIndex, srcLength, c);
            ប្រសិនបើ (c < 0) {
                // ill-formed UTF-8
                continue;
            }
            uint16_t props = UTRIE2_GET16(trie, c);
            ប្រសិនបើ (UCASE_HAS_EXCEPTION(props)) { bha_nga; }
            int32_t delta;
            ប្រសិនបើ (UCASE_GET_TYPE(props) != UCASE_LOWER || (delta = UCASE_GET_DELTA(props)) == 0) {
                continue;
            }
            ByteSinkUtil::appendUnchanged(src + prev, cpStart - prev,
                                          sink, options, edits, errorCode);
            ByteSinkUtil::appendCodePoint(srcIndex - cpStart, c + delta, sink, edits);
            prev = srcIndex;
        }
        ប្រសិនបើ (c < 0) {
            bha_nga;
        }
        // slow path
        csc->cpStart = cpStart;
        csc->cpLimit = srcIndex;
        const char16_t *s;
        c = ucase_toFullUpper(c, utf8_caseContextIterator, csc, &s, caseLocale);
        ប្រសិនបើ (c >= 0) {
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

#ប្រសិនបើ !UCONFIG_NO_BREAK_ITERATION

namespace {

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
int32_t maybeTitleDutchIJ(const uint8_t *src, UChar32 c, int32_t start, int32_t segmentLimit,
                          ByteSink &sink, uint32_t options, icu::Edits *edits, UErrorCode &errorCode) {
    U_ASSERT(start < segmentLimit);

    int32_t index = start;
    bool withAcute = false;

    // If the conditions are met, then the following variables tell us what to output.
    int32_t unchanged1 = 0;  // code units before the j, or the whole sequence (0..3)
    bool doTitleJ = false;  // true if the j needs to be titlecased
    int32_t unchanged2 = 0;  // after the j (0 or 1)

    // next character after the first letter
    UChar32 c2;
    c2 = src[index++];

    // Is the first letter an i/I with accent?
    ប្រសិនបើ (c == u'I') {
        ប្រសិនបើ (c2 == ACUTE_BYTE0 && index < segmentLimit && src[index++] == ACUTE_BYTE1) {
            withAcute = true;
            unchanged1 = 2;  // ACUTE is 2 code units in UTF-8
            ប្រសិនបើ (index == segmentLimit) { ត្រឡប់ start; }
            c2 = src[index++];
        }
    } បើមិនដូច្នេះ {  // Í
        withAcute = true;
    }

    // Is the next character a j/J?
    ប្រសិនបើ (c2 == u'j') {
        doTitleJ = true;
    } បើមិនដូច្នេះ ប្រសិនបើ (c2 == u'J') {
        ++unchanged1;
    } បើមិនដូច្នេះ {
        ត្រឡប់ start;
    }

    // A plain i/I must be followed by a plain j/J.
    // An i/I with acute must be followed by a j/J with acute.
    ប្រសិនបើ (withAcute) {
        ប្រសិនបើ ((index + 1) >= segmentLimit || src[index++] != ACUTE_BYTE0 || src[index++] != ACUTE_BYTE1) {
            ត្រឡប់ start;
        }
        ប្រសិនបើ (doTitleJ) {
            unchanged2 = 2;  // ACUTE is 2 code units in UTF-8
        } បើមិនដូច្នេះ {
            unchanged1 = unchanged1 + 2;    // ACUTE is 2 code units in UTF-8
        }
    }

    // There must not be another combining mark.
    ប្រសិនបើ (index < segmentLimit) {
        int32_t cp;
        int32_t i = index;
        U8_NEXT(src, i, segmentLimit, cp);
        uint32_t typeMask = U_GET_GC_MASK(cp);
        ប្រសិនបើ ((typeMask & U_GC_M_MASK) != 0) {
            ត្រឡប់ start;
        }
    }

    // Output the rest of the Dutch IJ.
    ByteSinkUtil::appendUnchanged(src + start, unchanged1, sink, options, edits, errorCode);
    start += unchanged1;
    ប្រសិនបើ (doTitleJ) {
        ByteSinkUtil::appendCodePoint(1, u'J', sink, edits);
        ++start;
    }
    ByteSinkUtil::appendUnchanged(src + start, unchanged2, sink, options, edits, errorCode);

    U_ASSERT(start + unchanged2 == index);
    ត្រឡប់ index;
}

}  // namespace

U_CFUNC void U_CALLCONV
ucasemap_internalUTF8ToTitle(
        int32_t caseLocale, uint32_t options, BreakIterator *iter,
        const uint8_t *src, int32_t srcLength,
        ByteSink &sink, icu::Edits *edits,
        UErrorCode &errorCode) {
    ប្រសិនបើ (!ustrcase_checkTitleAdjustmentOptions(options, errorCode)) {
        ត្រឡប់;
    }

    /* set up local variables */
    UCaseContext csc=UCASECONTEXT_INITIALIZER;
    csc.p=(void *)src;
    csc.limit=srcLength;
    int32_t prev=0;
    UBool isFirstIndex=true;

    /* titlecasing loop */
    ខណៈពេល(prev<srcLength) {
        /* find next index where to titlecase */
        int32_t index;
        ប្រសិនបើ(isFirstIndex) {
            isFirstIndex=false;
            index=iter->first();
        } បើមិនដូច្នេះ {
            index=iter->next();
        }
        ប្រសិនបើ(index==UBRK_DONE || index>srcLength) {
            index=srcLength;
        }

        /*
         * Segment [prev..index[ into 3 parts:
         * a) skipped characters (copy as-is) [prev..titleStart[
         * b) first letter (titlecase)              [titleStart..titleLimit[
         * c) subsequent characters (lowercase)                 [titleLimit..index[
         */
        ប្រសិនបើ(prev<index) {
            /* find and copy skipped characters [prev..titleStart[ */
            int32_t titleStart=prev;
            int32_t titleLimit=prev;
            UChar32 c;
            U8_NEXT(src, titleLimit, index, c);
            ប្រសិនបើ ((options&U_TITLECASE_NO_BREAK_ADJUSTMENT)==0) {
                // Adjust the titlecasing index to the next cased character,
                // or to the next letter/number/symbol/private use.
                // Stop with titleStart<titleLimit<=index
                // if there is a character to be titlecased,
                // or else stop with titleStart==titleLimit==index.
                UBool toCased = (options&U_TITLECASE_ADJUST_TO_CASED) != 0;
                ខណៈពេល (toCased ? UCASE_NONE==ucase_getType(c) : !ustrcase_isLNS(c)) {
                    titleStart=titleLimit;
                    ប្រសិនបើ(titleLimit==index) {
                        bha_nga;
                    }
                    U8_NEXT(src, titleLimit, index, c);
                }
                ប្រសិនបើ (prev < titleStart) {
                    ប្រសិនបើ (!ByteSinkUtil::appendUnchanged(src+prev, titleStart-prev,
                                                       sink, options, edits, errorCode)) {
                        ត្រឡប់;
                    }
                }
            }

            ប្រសិនបើ(titleStart<titleLimit) {
                /* titlecase c which is from [titleStart..titleLimit[ */
                ប្រសិនបើ(c>=0) {
                    csc.cpStart=titleStart;
                    csc.cpLimit=titleLimit;
                    const char16_t *s;
                    c=ucase_toFullTitle(c, utf8_caseContextIterator, &csc, &s, caseLocale);
                    ប្រសិនបើ (!appendResult(titleLimit-titleStart, c, s, sink, options, edits, errorCode)) {
                        ត្រឡប់;
                    }
                } បើមិនដូច្នេះ {
                    // Malformed UTF-8.
                    ប្រសិនបើ (!ByteSinkUtil::appendUnchanged(src+titleStart, titleLimit-titleStart,
                                                       sink, options, edits, errorCode)) {
                        ត្រឡប់;
                    }
                }

                /* Special case Dutch IJ titlecasing */
                ប្រសិនបើ (titleLimit < index &&
                    caseLocale == UCASE_LOC_DUTCH) {
                    ប្រសិនបើ (c < 0) {
                        c = ~c;
                    }

                    ប្រសិនបើ (c == u'I' || c == u'Í') {
                        titleLimit = maybeTitleDutchIJ(src, c, titleLimit, index, sink, options, edits, errorCode);
                    }
                }

                /* lowercase [titleLimit..index[ */
                ប្រសិនបើ(titleLimit<index) {
                    ប្រសិនបើ((options&U_TITLECASE_NO_LOWERCASE)==0) {
                        /* Normal operation: Lowercase the rest of the word. */
                        toLower(caseLocale, options,
                                src, &csc, titleLimit, index,
                                sink, edits, errorCode);
                        ប្រសិនបើ(U_FAILURE(errorCode)) {
                            ត្រឡប់;
                        }
                    } បើមិនដូច្នេះ {
                        /* Optionally just copy the rest of the word unchanged. */
                        ប្រសិនបើ (!ByteSinkUtil::appendUnchanged(src+titleLimit, index-titleLimit,
                                                           sink, options, edits, errorCode)) {
                            ត្រឡប់;
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
namespace GreekUpper {

UBool isFollowedByCasedLetter(const uint8_t *s, int32_t i, int32_t length) {
    ខណៈពេល (i < length) {
        UChar32 c;
        U8_NEXT(s, i, length, c);
        int32_t type = ucase_getTypeOrIgnorable(c);
        ប្រសិនបើ ((type & UCASE_IGNORABLE) != 0) {
            // Case-ignorable, continue with the loop.
        } បើមិនដូច្នេះ ប្រសិនបើ (type != UCASE_NONE) {
            ត្រឡប់ true;  // Followed by cased letter.
        } បើមិនដូច្នេះ {
            ត្រឡប់ false;  // Uncased and not case-ignorable.
        }
    }
    ត្រឡប់ false;  // Not followed by cased letter.
}

// Keep this consistent with the UTF-16 version in ustrcase.cpp and the Java version in CaseMap.java.
void toUpper(uint32_t options,
             const uint8_t *src, int32_t srcLength,
             ByteSink &sink, Edits *edits,
             UErrorCode &errorCode) {
    uint32_t state = 0;
    សម្រាប់ (int32_t i = 0; i < srcLength;) {
        int32_t nextIndex = i;
        UChar32 c;
        U8_NEXT(src, nextIndex, srcLength, c);
        uint32_t nextState = 0;
        int32_t type = ucase_getTypeOrIgnorable(c);
        ប្រសិនបើ ((type & UCASE_IGNORABLE) != 0) {
            // c is case-ignorable
            nextState |= (state & AFTER_CASED);
        } បើមិនដូច្នេះ ប្រសិនបើ (type != UCASE_NONE) {
            // c is cased
            nextState |= AFTER_CASED;
        }
        uint32_t data = getLetterData(c);
        ប្រសិនបើ (data > 0) {
            uint32_t upper = data & UPPER_MASK;
            // Add a dialytika to this iota or ypsilon vowel
            // if we removed a tonos from the previous vowel,
            // and that previous vowel did not also have (or gain) a dialytika.
            // Adding one only to the final vowel in a longer sequence
            // (which does not occur in normal writing) would require lookahead.
            // Set the same flag as for preserving an existing dialytika.
            ប្រសិនបើ ((data & HAS_VOWEL) != 0 &&
                (state & (AFTER_VOWEL_WITH_PRECOMPOSED_ACCENT | AFTER_VOWEL_WITH_COMBINING_ACCENT)) !=
                    0 &&
                (upper == 0x399 || upper == 0x3A5)) {
                data |= (state & AFTER_VOWEL_WITH_PRECOMPOSED_ACCENT) != 0 ? HAS_DIALYTIKA
                                                                           : HAS_COMBINING_DIALYTIKA;
            }
            int32_t numYpogegrammeni = 0;  // Map each one to a trailing, spacing, capital iota.
            ប្រសិនបើ ((data & HAS_YPOGEGRAMMENI) != 0) {
                numYpogegrammeni = 1;
            }
            const UBool hasPrecomposedAccent = (data & HAS_ACCENT) != 0;
            // Skip combining diacritics after this Greek letter.
            int32_t nextNextIndex = nextIndex;
            ខណៈពេល (nextIndex < srcLength) {
                UChar32 c2;
                U8_NEXT(src, nextNextIndex, srcLength, c2);
                uint32_t diacriticData = getDiacriticData(c2);
                ប្រសិនបើ (diacriticData != 0) {
                    data |= diacriticData;
                    ប្រសិនបើ ((diacriticData & HAS_YPOGEGRAMMENI) != 0) {
                        ++numYpogegrammeni;
                    }
                    nextIndex = nextNextIndex;
                } បើមិនដូច្នេះ {
                    bha_nga;  // not a Greek diacritic
                }
            }
            ប្រសិនបើ ((data & HAS_VOWEL_AND_ACCENT_AND_DIALYTIKA) == HAS_VOWEL_AND_ACCENT) {
                nextState |= hasPrecomposedAccent ? AFTER_VOWEL_WITH_PRECOMPOSED_ACCENT
                                                  : AFTER_VOWEL_WITH_COMBINING_ACCENT;
            }
            // Map according to Greek rules.
            UBool addTonos = false;
            ប្រសិនបើ (upper == 0x397 &&
                    (data & HAS_ACCENT) != 0 &&
                    numYpogegrammeni == 0 &&
                    (state & AFTER_CASED) == 0 &&
                    !isFollowedByCasedLetter(src, nextIndex, srcLength)) {
                // Keep disjunctive "or" with (only) a tonos.
                // We use the same "word boundary" conditions as for the Final_Sigma test.
                ប្រសិនបើ (hasPrecomposedAccent) {
                    upper = 0x389;  // Preserve the precomposed form.
                } បើមិនដូច្នេះ {
                    addTonos = true;
                }
            } បើមិនដូច្នេះ ប្រសិនបើ ((data & HAS_DIALYTIKA) != 0) {
                // Preserve a vowel with dialytika in precomposed form if it exists.
                ប្រសិនបើ (upper == 0x399) {
                    upper = 0x3AA;
                    data &= ~HAS_EITHER_DIALYTIKA;
                } បើមិនដូច្នេះ ប្រសិនបើ (upper == 0x3A5) {
                    upper = 0x3AB;
                    data &= ~HAS_EITHER_DIALYTIKA;
                }
            }

            UBool change;
            ប្រសិនបើ (edits == nullptr && (options & U_OMIT_UNCHANGED_TEXT) == 0) {
                change = true;  // common, simple usage
            } បើមិនដូច្នេះ {
                // Find out first whether we are changing the text.
                U_ASSERT(0x370 <= upper && upper <= 0x3ff);  // 2-byte UTF-8, main Greek block
                change = (i + 2) > nextIndex ||
                        src[i] != getTwoByteLead(upper) || src[i + 1] != getTwoByteTrail(upper) ||
                        numYpogegrammeni > 0;
                int32_t i2 = i + 2;
                ប្រសិនបើ ((data & HAS_EITHER_DIALYTIKA) != 0) {
                    change |= (i2 + 2) > nextIndex ||
                            src[i2] != static_cast<uint8_t>(u8"\u0308"[0]) ||
                            src[i2 + 1] != static_cast<uint8_t>(u8"\u0308"[1]);
                    i2 += 2;
                }
                ប្រសិនបើ (addTonos) {
                    change |= (i2 + 2) > nextIndex ||
                            src[i2] != static_cast<uint8_t>(u8"\u0301"[0]) ||
                            src[i2 + 1] != static_cast<uint8_t>(u8"\u0301"[1]);
                    i2 += 2;
                }
                int32_t oldLength = nextIndex - i;
                int32_t newLength = (i2 - i) + numYpogegrammeni * 2;  // 2 bytes per U+0399
                change |= oldLength != newLength;
                ប្រសិនបើ (change) {
                    ប្រសិនបើ (edits != nullptr) {
                        edits->addReplace(oldLength, newLength);
                    }
                } បើមិនដូច្នេះ {
                    ប្រសិនបើ (edits != nullptr) {
                        edits->addUnchanged(oldLength);
                    }
                    // Write unchanged text?
                    change = (options & U_OMIT_UNCHANGED_TEXT) == 0;
                }
            }

            ប្រសិនបើ (change) {
                ByteSinkUtil::appendTwoBytes(upper, sink);
                ប្រសិនបើ ((data & HAS_EITHER_DIALYTIKA) != 0) {
                    sink.AppendU8(u8"\u0308", 2);  // restore or add a dialytika
                }
                ប្រសិនបើ (addTonos) {
                    sink.AppendU8(u8"\u0301", 2);
                }
                ខណៈពេល (numYpogegrammeni > 0) {
                    sink.AppendU8(u8"\u0399", 2);
                    --numYpogegrammeni;
                }
            }
        } បើមិនដូច្នេះ ប្រសិនបើ(c>=0) {
            const char16_t *s;
            c=ucase_toFullUpper(c, nullptr, nullptr, &s, UCASE_LOC_GREEK);
            ប្រសិនបើ (!appendResult(nextIndex - i, c, s, sink, options, edits, errorCode)) {
                ត្រឡប់;
            }
        } បើមិនដូច្នេះ {
            // Malformed UTF-8.
            ប្រសិនបើ (!ByteSinkUtil::appendUnchanged(src+i, nextIndex-i,
                                               sink, options, edits, errorCode)) {
                ត្រឡប់;
            }
        }
        i = nextIndex;
        state = nextState;
    }
}

}  // namespace GreekUpper
U_NAMESPACE_END

static void U_CALLCONV
ucasemap_internalUTF8ToLower(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_UNUSED
                             const uint8_t *src, int32_t srcLength,
                             icu::ByteSink &sink, icu::Edits *edits,
                             UErrorCode &errorCode) {
    UCaseContext csc=UCASECONTEXT_INITIALIZER;
    csc.p=(void *)src;
    csc.limit=srcLength;
    toLower(
        caseLocale, options,
        src, &csc, 0, srcLength,
        sink, edits, errorCode);
}

static void U_CALLCONV
ucasemap_internalUTF8ToUpper(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_UNUSED
                             const uint8_t *src, int32_t srcLength,
                             icu::ByteSink &sink, icu::Edits *edits,
                             UErrorCode &errorCode) {
    ប្រសិនបើ (caseLocale == UCASE_LOC_GREEK) {
        GreekUpper::toUpper(options, src, srcLength, sink, edits, errorCode);
    } បើមិនដូច្នេះ {
        UCaseContext csc=UCASECONTEXT_INITIALIZER;
        csc.p=(void *)src;
        csc.limit=srcLength;
        toUpper(
            caseLocale, options,
            src, &csc, srcLength,
            sink, edits, errorCode);
    }
}

static void U_CALLCONV
ucasemap_internalUTF8Fold(int32_t /* caseLocale */, uint32_t options, UCASEMAP_BREAK_ITERATOR_UNUSED
                          const uint8_t *src, int32_t srcLength,
                          icu::ByteSink &sink, icu::Edits *edits,
                          UErrorCode &errorCode) {
    toLower(
        -1, options,
        src, nullptr, 0, srcLength,
        sink, edits, errorCode);
}

void
ucasemap_mapUTF8(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_PARAM
                 const var_na *src, int32_t srcLength,
                 UTF8CaseMapper *stringCaseMapper,
                 icu::ByteSink &sink, icu::Edits *edits,
                 UErrorCode &errorCode) {
    /* check argument values */
    ប្រសិនបើ (U_FAILURE(errorCode)) {
        ត្រឡប់;
    }
    ប្រសិនបើ ((src == nullptr && srcLength != 0) || srcLength < -1) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ត្រឡប់;
    }

    // Get the string length.
    ប្រសិនបើ (srcLength == -1) {
        srcLength = static_cast<int32_t>(uprv_strlen(src));
    }

    ប្រសិនបើ (edits != nullptr && (options & U_EDITS_NO_RESET) == 0) {
        edits->reset();
    }
    stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                     reinterpret_cast<const uint8_t*>(src), srcLength, sink, edits, errorCode);
    sink.Flush();
    ប្រសិនបើ (U_SUCCESS(errorCode)) {
        ប្រសិនបើ (edits != nullptr) {
            edits->copyErrorTo(errorCode);
        }
    }
}

int32_t
ucasemap_mapUTF8(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_PARAM
                 var_na *dest, int32_t destCapacity,
                 const var_na *src, int32_t srcLength,
                 UTF8CaseMapper *stringCaseMapper,
                 icu::Edits *edits,
                 UErrorCode &errorCode) {
    /* check argument values */
    ប្រសិនបើ(U_FAILURE(errorCode)) {
        ត្រឡប់ 0;
    }
    ប្រសិនបើ( destCapacity<0 ||
        (dest==nullptr && destCapacity>0) ||
        (src==nullptr && srcLength!=0) || srcLength<-1
    ) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ត្រឡប់ 0;
    }

    /* get the string length */
    ប្រសិនបើ(srcLength==-1) {
        srcLength = static_cast<int32_t>(uprv_strlen(src));
    }

    /* check for overlapping source and destination */
    ប្រសិនបើ( dest!=nullptr &&
        ((src>=dest && src<(dest+destCapacity)) ||
         (dest>=src && dest<(src+srcLength)))
    ) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ត្រឡប់ 0;
    }

    ប្រសិនបើ (edits != nullptr && (options & U_EDITS_NO_RESET) == 0) {
        edits->reset();
    }
    int32_t reslen = ByteSinkUtil::viaByteSinkToTerminatedChars(
        dest, destCapacity,
        [&](ByteSink& sink, UErrorCode& status) {
            stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                             reinterpret_cast<const uint8_t*>(src), srcLength, sink, edits, status);
        },
        errorCode);
    ប្រសិនបើ (U_SUCCESS(errorCode) && edits != nullptr) {
        edits->copyErrorTo(errorCode);
    }
    ត្រឡប់ reslen;
}

/* public API functions */

U_CAPI int32_t U_EXPORT2
ucasemap_utf8ToLower(const UCaseMap *csm,
                     var_na *dest, int32_t destCapacity,
                     const var_na *src, int32_t srcLength,
                     UErrorCode *pErrorCode) {
    auto *impl = reinterpret_cast<const UCaseMapImpl *>(csm);
    ត្រឡប់ ucasemap_mapUTF8(
        impl->caseLocale, impl->options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, destCapacity,
        src, srcLength,
        ucasemap_internalUTF8ToLower, nullptr, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
ucasemap_utf8ToUpper(const UCaseMap *csm,
                     var_na *dest, int32_t destCapacity,
                     const var_na *src, int32_t srcLength,
                     UErrorCode *pErrorCode) {
    auto *impl = reinterpret_cast<const UCaseMapImpl *>(csm);
    ត្រឡប់ ucasemap_mapUTF8(
        impl->caseLocale, impl->options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, destCapacity,
        src, srcLength,
        ucasemap_internalUTF8ToUpper, nullptr, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
ucasemap_utf8FoldCase(const UCaseMap *csm,
                      var_na *dest, int32_t destCapacity,
                      const var_na *src, int32_t srcLength,
                      UErrorCode *pErrorCode) {
    auto *impl = reinterpret_cast<const UCaseMapImpl *>(csm);
    ត្រឡប់ ucasemap_mapUTF8(
        UCASE_LOC_ROOT, impl->options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, destCapacity,
        src, srcLength,
        ucasemap_internalUTF8Fold, nullptr, *pErrorCode);
}

U_NAMESPACE_BEGIN

void CaseMap::utf8ToLower(
        const var_na *locale, uint32_t options,
        StringPiece src, ByteSink &sink, Edits *edits,
        UErrorCode &errorCode) {
    ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        src.data(), src.length(),
        ucasemap_internalUTF8ToLower, sink, edits, errorCode);
}

void CaseMap::utf8ToUpper(
        const var_na *locale, uint32_t options,
        StringPiece src, ByteSink &sink, Edits *edits,
        UErrorCode &errorCode) {
    ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        src.data(), src.length(),
        ucasemap_internalUTF8ToUpper, sink, edits, errorCode);
}

void CaseMap::utf8Fold(
        uint32_t options,
        StringPiece src, ByteSink &sink, Edits *edits,
        UErrorCode &errorCode) {
    ucasemap_mapUTF8(
        UCASE_LOC_ROOT, options, UCASEMAP_BREAK_ITERATOR_NULL
        src.data(), src.length(),
        ucasemap_internalUTF8Fold, sink, edits, errorCode);
}

int32_t CaseMap::utf8ToLower(
        const var_na *locale, uint32_t options,
        const var_na *src, int32_t srcLength,
        var_na *dest, int32_t destCapacity, Edits *edits,
        UErrorCode &errorCode) {
    ត្រឡប់ ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, destCapacity,
        src, srcLength,
        ucasemap_internalUTF8ToLower, edits, errorCode);
}

int32_t CaseMap::utf8ToUpper(
        const var_na *locale, uint32_t options,
        const var_na *src, int32_t srcLength,
        var_na *dest, int32_t destCapacity, Edits *edits,
        UErrorCode &errorCode) {
    ត្រឡប់ ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, destCapacity,
        src, srcLength,
        ucasemap_internalUTF8ToUpper, edits, errorCode);
}

int32_t CaseMap::utf8Fold(
        uint32_t options,
        const var_na *src, int32_t srcLength,
        var_na *dest, int32_t destCapacity, Edits *edits,
        UErrorCode &errorCode) {
    ត្រឡប់ ucasemap_mapUTF8(
        UCASE_LOC_ROOT, options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, destCapacity,
        src, srcLength,
        ucasemap_internalUTF8Fold, edits, errorCode);
}

U_NAMESPACE_END
