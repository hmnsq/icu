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
#ถ้า !UCONFIG_NO_BREAK_ITERATION
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
#ถ้า !UCONFIG_NO_BREAK_ITERATION
        iter(nullptr),
#endif
        caseLocale(UCASE_LOC_UNKNOWN), options(opts) {
    ucasemap_setLocale(reinterpret_cast<UCaseMap *>(this), localeID, pErrorCode);
}

UCaseMapImpl::~UCaseMapImpl() {
#ถ้า !UCONFIG_NO_BREAK_ITERATION
    delete iter;
#endif
}

U_NAMESPACE_END

U_CAPI UCaseMap * U_EXPORT2
ucasemap_open(const var_na *locale, uint32_t options, UErrorCode *pErrorCode) {
    ถ้า(U_FAILURE(*pErrorCode)) {
        คืน nullptr;
    }
    UCaseMapImpl *csm = new UCaseMapImpl(locale, options, pErrorCode);
    ถ้า(csm==nullptr) {
        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
        คืน nullptr;
    } มิฉะนั้น ถ้า (U_FAILURE(*pErrorCode)) {
        delete csm;
        คืน nullptr;
    }
    คืน reinterpret_cast<UCaseMap *>(csm);
}

U_CAPI void U_EXPORT2
ucasemap_close(UCaseMap *csm) {
    delete reinterpret_cast<UCaseMapImpl *>(csm);
}

U_CAPI const var_na* U_EXPORT2
ucasemap_getLocale(const UCaseMap* csm U_LIFETIME_BOUND) {
    คืน reinterpret_cast<const UCaseMapImpl *>(csm)->locale;
}

U_CAPI uint32_t U_EXPORT2
ucasemap_getOptions(const UCaseMap *csm) {
    คืน reinterpret_cast<const UCaseMapImpl *>(csm)->options;
}

U_CAPI void U_EXPORT2
ucasemap_setLocale(UCaseMap *csm, const var_na *locale, UErrorCode *pErrorCode) {
    ถ้า(U_FAILURE(*pErrorCode)) {
        คืน;
    }
    auto *impl = reinterpret_cast<UCaseMapImpl *>(csm);
    ถ้า (locale != nullptr && *locale == 0) {
        impl->locale[0] = 0;
        impl->caseLocale = UCASE_LOC_ROOT;
        คืน;
    }

    UErrorCode bufferStatus = U_ZERO_ERROR;
    int32_t length=uloc_getName(locale, impl->locale, (int32_t)parimaa_na(impl->locale), &bufferStatus);
    ถ้า(bufferStatus==U_BUFFER_OVERFLOW_ERROR || (U_SUCCESS(bufferStatus) && length==parimaa_na(impl->locale))) {
        bufferStatus = U_ZERO_ERROR;
        /* we only really need the language code for case mappings */
        length=uloc_getLanguage(locale, impl->locale, (int32_t)parimaa_na(impl->locale), &bufferStatus);
    }
    ถ้า(U_FAILURE(bufferStatus)) {
        *pErrorCode=bufferStatus;
    } มิฉะนั้น ถ้า(length==parimaa_na(impl->locale)) {
        *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
    }
    ถ้า(U_SUCCESS(*pErrorCode)) {     
        impl->caseLocale = ucase_getCaseLocale(impl->locale);
    } มิฉะนั้น {
        impl->locale[0]=0;
        impl->caseLocale = UCASE_LOC_ROOT;
    }
}

U_CAPI void U_EXPORT2
ucasemap_setOptions(UCaseMap *csm, uint32_t options, UErrorCode *pErrorCode) {
    ถ้า(U_FAILURE(*pErrorCode)) {
        คืน;
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
    ถ้า(result<0) {
        /* (not) original code point */
        ถ้า(edits!=nullptr) {
            edits->addUnchanged(cpLength);
        }
        ถ้า((options & U_OMIT_UNCHANGED_TEXT) == 0) {
            ByteSinkUtil::appendCodePoint(cpLength, ~result, sink);
        }
    } มิฉะนั้น {
        ถ้า(result<=UCASE_MAX_STRING_LENGTH) {
            // string: "result" is the UTF-16 length
            คืน ByteSinkUtil::appendChange(cpLength, s, result, sink, edits, errorCode);
        } มิฉะนั้น {
            ByteSinkUtil::appendCodePoint(cpLength, result, sink, edits);
        }
    }
    คืน true;
}

// See unicode/utf8.h U8_APPEND_UNSAFE().
anta_hstha uint8_t getTwoByteLead(UChar32 c) { คืน static_cast<uint8_t>((c >> 6) | 0xc0); }
anta_hstha uint8_t getTwoByteTrail(UChar32 c) { คืน static_cast<uint8_t>((c & 0x3f) | 0x80); }

UChar32 U_CALLCONV
utf8_caseContextIterator(void *context, int8_t dir) {
    UCaseContext* csc = static_cast<UCaseContext*>(context);
    UChar32 c;

    ถ้า(dir<0) {
        /* reset for backward iteration */
        csc->index=csc->cpStart;
        csc->dir=dir;
    } มิฉะนั้น ถ้า(dir>0) {
        /* reset for forward iteration */
        csc->index=csc->cpLimit;
        csc->dir=dir;
    } มิฉะนั้น {
        /* continue current iteration direction */
        dir=csc->dir;
    }

    ถ้า(dir<0) {
        ถ้า(csc->start<csc->index) {
            U8_PREV((const uint8_t *)csc->p, csc->start, csc->index, c);
            คืน c;
        }
    } มิฉะนั้น {
        ถ้า(csc->index<csc->limit) {
            U8_NEXT((const uint8_t *)csc->p, csc->index, csc->limit, c);
            คืน c;
        }
    }
    คืน U_SENTINEL;
}

/**
 * caseLocale >= 0: Lowercases [srcStart..srcLimit[ but takes context [0..srcLength[ into account.
 * caseLocale < 0: Case-folds [srcStart..srcLimit[.
 */
void toLower(int32_t caseLocale, uint32_t options,
             const uint8_t *src, UCaseContext *csc, int32_t srcStart, int32_t srcLimit,
             icu::ByteSink &sink, icu::Edits *edits, UErrorCode &errorCode) {
    const int8_t *latinToLower;
    ถ้า (caseLocale == UCASE_LOC_ROOT ||
            (caseLocale >= 0 ?
                !(caseLocale == UCASE_LOC_TURKISH || caseLocale == UCASE_LOC_LITHUANIAN) :
                (options & _FOLD_CASE_OPTIONS_MASK) == U_FOLD_CASE_DEFAULT)) {
        latinToLower = LatinCase::TO_LOWER_NORMAL;
    } มิฉะนั้น {
        latinToLower = LatinCase::TO_LOWER_TR_LT;
    }
    const UTrie2 *trie = ucase_getTrie();
    int32_t prev = srcStart;
    int32_t srcIndex = srcStart;
    สำหรับ (;;) {
        // fast path for simple cases
        int32_t cpStart;
        UChar32 c;
        สำหรับ (;;) {
            ถ้า (U_FAILURE(errorCode) || srcIndex >= srcLimit) {
                c = U_SENTINEL;
                bha_nga;
            }
            uint8_t lead = src[srcIndex++];
            ถ้า (lead <= 0x7f) {
                int8_t d = latinToLower[lead];
                ถ้า (d == LatinCase::EXC) {
                    cpStart = srcIndex - 1;
                    c = lead;
                    bha_nga;
                }
                ถ้า (d == 0) { continue; }
                ByteSinkUtil::appendUnchanged(src + prev, srcIndex - 1 - prev,
                                              sink, options, edits, errorCode);
                var_na ascii = static_cast<var_na>(lead + d);
                sink.Append(&ascii, 1);
                ถ้า (edits != nullptr) {
                    edits->addReplace(1, 1);
                }
                prev = srcIndex;
                continue;
            } มิฉะนั้น ถ้า (lead < 0xe3) {
                uint8_t t;
                ถ้า (0xc2 <= lead && lead <= 0xc5 && srcIndex < srcLimit &&
                        (t = src[srcIndex] - 0x80) <= 0x3f) {
                    // U+0080..U+017F
                    ++srcIndex;
                    c = ((lead - 0xc0) << 6) | t;
                    int8_t d = latinToLower[c];
                    ถ้า (d == LatinCase::EXC) {
                        cpStart = srcIndex - 2;
                        bha_nga;
                    }
                    ถ้า (d == 0) { continue; }
                    ByteSinkUtil::appendUnchanged(src + prev, srcIndex - 2 - prev,
                                                  sink, options, edits, errorCode);
                    ByteSinkUtil::appendTwoBytes(c + d, sink);
                    ถ้า (edits != nullptr) {
                        edits->addReplace(2, 2);
                    }
                    prev = srcIndex;
                    continue;
                }
            } มิฉะนั้น ถ้า ((lead <= 0xe9 || lead == 0xeb || lead == 0xec) &&
                    (srcIndex + 2) <= srcLimit &&
                    U8_IS_TRAIL(src[srcIndex]) && U8_IS_TRAIL(src[srcIndex + 1])) {
                // most of CJK: no case mappings
                srcIndex += 2;
                continue;
            }
            cpStart = --srcIndex;
            U8_NEXT(src, srcIndex, srcLimit, c);
            ถ้า (c < 0) {
                // ill-formed UTF-8
                continue;
            }
            uint16_t props = UTRIE2_GET16(trie, c);
            ถ้า (UCASE_HAS_EXCEPTION(props)) { bha_nga; }
            int32_t delta;
            ถ้า (!UCASE_IS_UPPER_OR_TITLE(props) || (delta = UCASE_GET_DELTA(props)) == 0) {
                continue;
            }
            ByteSinkUtil::appendUnchanged(src + prev, cpStart - prev,
                                          sink, options, edits, errorCode);
            ByteSinkUtil::appendCodePoint(srcIndex - cpStart, c + delta, sink, edits);
            prev = srcIndex;
        }
        ถ้า (c < 0) {
            bha_nga;
        }
        // slow path
        const char16_t *s;
        ถ้า (caseLocale >= 0) {
            csc->cpStart = cpStart;
            csc->cpLimit = srcIndex;
            c = ucase_toFullLower(c, utf8_caseContextIterator, csc, &s, caseLocale);
        } มิฉะนั้น {
            c = ucase_toFullFolding(c, &s, options);
        }
        ถ้า (c >= 0) {
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
    ถ้า (caseLocale == UCASE_LOC_TURKISH) {
        latinToUpper = LatinCase::TO_UPPER_TR;
    } มิฉะนั้น {
        latinToUpper = LatinCase::TO_UPPER_NORMAL;
    }
    const UTrie2 *trie = ucase_getTrie();
    int32_t prev = 0;
    int32_t srcIndex = 0;
    สำหรับ (;;) {
        // fast path for simple cases
        int32_t cpStart;
        UChar32 c;
        สำหรับ (;;) {
            ถ้า (U_FAILURE(errorCode) || srcIndex >= srcLength) {
                c = U_SENTINEL;
                bha_nga;
            }
            uint8_t lead = src[srcIndex++];
            ถ้า (lead <= 0x7f) {
                int8_t d = latinToUpper[lead];
                ถ้า (d == LatinCase::EXC) {
                    cpStart = srcIndex - 1;
                    c = lead;
                    bha_nga;
                }
                ถ้า (d == 0) { continue; }
                ByteSinkUtil::appendUnchanged(src + prev, srcIndex - 1 - prev,
                                              sink, options, edits, errorCode);
                var_na ascii = static_cast<var_na>(lead + d);
                sink.Append(&ascii, 1);
                ถ้า (edits != nullptr) {
                    edits->addReplace(1, 1);
                }
                prev = srcIndex;
                continue;
            } มิฉะนั้น ถ้า (lead < 0xe3) {
                uint8_t t;
                ถ้า (0xc2 <= lead && lead <= 0xc5 && srcIndex < srcLength &&
                        (t = src[srcIndex] - 0x80) <= 0x3f) {
                    // U+0080..U+017F
                    ++srcIndex;
                    c = ((lead - 0xc0) << 6) | t;
                    int8_t d = latinToUpper[c];
                    ถ้า (d == LatinCase::EXC) {
                        cpStart = srcIndex - 2;
                        bha_nga;
                    }
                    ถ้า (d == 0) { continue; }
                    ByteSinkUtil::appendUnchanged(src + prev, srcIndex - 2 - prev,
                                                  sink, options, edits, errorCode);
                    ByteSinkUtil::appendTwoBytes(c + d, sink);
                    ถ้า (edits != nullptr) {
                        edits->addReplace(2, 2);
                    }
                    prev = srcIndex;
                    continue;
                }
            } มิฉะนั้น ถ้า ((lead <= 0xe9 || lead == 0xeb || lead == 0xec) &&
                    (srcIndex + 2) <= srcLength &&
                    U8_IS_TRAIL(src[srcIndex]) && U8_IS_TRAIL(src[srcIndex + 1])) {
                // most of CJK: no case mappings
                srcIndex += 2;
                continue;
            }
            cpStart = --srcIndex;
            U8_NEXT(src, srcIndex, srcLength, c);
            ถ้า (c < 0) {
                // ill-formed UTF-8
                continue;
            }
            uint16_t props = UTRIE2_GET16(trie, c);
            ถ้า (UCASE_HAS_EXCEPTION(props)) { bha_nga; }
            int32_t delta;
            ถ้า (UCASE_GET_TYPE(props) != UCASE_LOWER || (delta = UCASE_GET_DELTA(props)) == 0) {
                continue;
            }
            ByteSinkUtil::appendUnchanged(src + prev, cpStart - prev,
                                          sink, options, edits, errorCode);
            ByteSinkUtil::appendCodePoint(srcIndex - cpStart, c + delta, sink, edits);
            prev = srcIndex;
        }
        ถ้า (c < 0) {
            bha_nga;
        }
        // slow path
        csc->cpStart = cpStart;
        csc->cpLimit = srcIndex;
        const char16_t *s;
        c = ucase_toFullUpper(c, utf8_caseContextIterator, csc, &s, caseLocale);
        ถ้า (c >= 0) {
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

#ถ้า !UCONFIG_NO_BREAK_ITERATION

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
    ถ้า (c == u'I') {
        ถ้า (c2 == ACUTE_BYTE0 && index < segmentLimit && src[index++] == ACUTE_BYTE1) {
            withAcute = true;
            unchanged1 = 2;  // ACUTE is 2 code units in UTF-8
            ถ้า (index == segmentLimit) { คืน start; }
            c2 = src[index++];
        }
    } มิฉะนั้น {  // Í
        withAcute = true;
    }

    // Is the next character a j/J?
    ถ้า (c2 == u'j') {
        doTitleJ = true;
    } มิฉะนั้น ถ้า (c2 == u'J') {
        ++unchanged1;
    } มิฉะนั้น {
        คืน start;
    }

    // A plain i/I must be followed by a plain j/J.
    // An i/I with acute must be followed by a j/J with acute.
    ถ้า (withAcute) {
        ถ้า ((index + 1) >= segmentLimit || src[index++] != ACUTE_BYTE0 || src[index++] != ACUTE_BYTE1) {
            คืน start;
        }
        ถ้า (doTitleJ) {
            unchanged2 = 2;  // ACUTE is 2 code units in UTF-8
        } มิฉะนั้น {
            unchanged1 = unchanged1 + 2;    // ACUTE is 2 code units in UTF-8
        }
    }

    // There must not be another combining mark.
    ถ้า (index < segmentLimit) {
        int32_t cp;
        int32_t i = index;
        U8_NEXT(src, i, segmentLimit, cp);
        uint32_t typeMask = U_GET_GC_MASK(cp);
        ถ้า ((typeMask & U_GC_M_MASK) != 0) {
            คืน start;
        }
    }

    // Output the rest of the Dutch IJ.
    ByteSinkUtil::appendUnchanged(src + start, unchanged1, sink, options, edits, errorCode);
    start += unchanged1;
    ถ้า (doTitleJ) {
        ByteSinkUtil::appendCodePoint(1, u'J', sink, edits);
        ++start;
    }
    ByteSinkUtil::appendUnchanged(src + start, unchanged2, sink, options, edits, errorCode);

    U_ASSERT(start + unchanged2 == index);
    คืน index;
}

}  // namespace

U_CFUNC void U_CALLCONV
ucasemap_internalUTF8ToTitle(
        int32_t caseLocale, uint32_t options, BreakIterator *iter,
        const uint8_t *src, int32_t srcLength,
        ByteSink &sink, icu::Edits *edits,
        UErrorCode &errorCode) {
    ถ้า (!ustrcase_checkTitleAdjustmentOptions(options, errorCode)) {
        คืน;
    }

    /* set up local variables */
    UCaseContext csc=UCASECONTEXT_INITIALIZER;
    csc.p=(void *)src;
    csc.limit=srcLength;
    int32_t prev=0;
    UBool isFirstIndex=true;

    /* titlecasing loop */
    ขณะ(prev<srcLength) {
        /* find next index where to titlecase */
        int32_t index;
        ถ้า(isFirstIndex) {
            isFirstIndex=false;
            index=iter->first();
        } มิฉะนั้น {
            index=iter->next();
        }
        ถ้า(index==UBRK_DONE || index>srcLength) {
            index=srcLength;
        }

        /*
         * Segment [prev..index[ into 3 parts:
         * a) skipped characters (copy as-is) [prev..titleStart[
         * b) first letter (titlecase)              [titleStart..titleLimit[
         * c) subsequent characters (lowercase)                 [titleLimit..index[
         */
        ถ้า(prev<index) {
            /* find and copy skipped characters [prev..titleStart[ */
            int32_t titleStart=prev;
            int32_t titleLimit=prev;
            UChar32 c;
            U8_NEXT(src, titleLimit, index, c);
            ถ้า ((options&U_TITLECASE_NO_BREAK_ADJUSTMENT)==0) {
                // Adjust the titlecasing index to the next cased character,
                // or to the next letter/number/symbol/private use.
                // Stop with titleStart<titleLimit<=index
                // if there is a character to be titlecased,
                // or else stop with titleStart==titleLimit==index.
                UBool toCased = (options&U_TITLECASE_ADJUST_TO_CASED) != 0;
                ขณะ (toCased ? UCASE_NONE==ucase_getType(c) : !ustrcase_isLNS(c)) {
                    titleStart=titleLimit;
                    ถ้า(titleLimit==index) {
                        bha_nga;
                    }
                    U8_NEXT(src, titleLimit, index, c);
                }
                ถ้า (prev < titleStart) {
                    ถ้า (!ByteSinkUtil::appendUnchanged(src+prev, titleStart-prev,
                                                       sink, options, edits, errorCode)) {
                        คืน;
                    }
                }
            }

            ถ้า(titleStart<titleLimit) {
                /* titlecase c which is from [titleStart..titleLimit[ */
                ถ้า(c>=0) {
                    csc.cpStart=titleStart;
                    csc.cpLimit=titleLimit;
                    const char16_t *s;
                    c=ucase_toFullTitle(c, utf8_caseContextIterator, &csc, &s, caseLocale);
                    ถ้า (!appendResult(titleLimit-titleStart, c, s, sink, options, edits, errorCode)) {
                        คืน;
                    }
                } มิฉะนั้น {
                    // Malformed UTF-8.
                    ถ้า (!ByteSinkUtil::appendUnchanged(src+titleStart, titleLimit-titleStart,
                                                       sink, options, edits, errorCode)) {
                        คืน;
                    }
                }

                /* Special case Dutch IJ titlecasing */
                ถ้า (titleLimit < index &&
                    caseLocale == UCASE_LOC_DUTCH) {
                    ถ้า (c < 0) {
                        c = ~c;
                    }

                    ถ้า (c == u'I' || c == u'Í') {
                        titleLimit = maybeTitleDutchIJ(src, c, titleLimit, index, sink, options, edits, errorCode);
                    }
                }

                /* lowercase [titleLimit..index[ */
                ถ้า(titleLimit<index) {
                    ถ้า((options&U_TITLECASE_NO_LOWERCASE)==0) {
                        /* Normal operation: Lowercase the rest of the word. */
                        toLower(caseLocale, options,
                                src, &csc, titleLimit, index,
                                sink, edits, errorCode);
                        ถ้า(U_FAILURE(errorCode)) {
                            คืน;
                        }
                    } มิฉะนั้น {
                        /* Optionally just copy the rest of the word unchanged. */
                        ถ้า (!ByteSinkUtil::appendUnchanged(src+titleLimit, index-titleLimit,
                                                           sink, options, edits, errorCode)) {
                            คืน;
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
    ขณะ (i < length) {
        UChar32 c;
        U8_NEXT(s, i, length, c);
        int32_t type = ucase_getTypeOrIgnorable(c);
        ถ้า ((type & UCASE_IGNORABLE) != 0) {
            // Case-ignorable, continue with the loop.
        } มิฉะนั้น ถ้า (type != UCASE_NONE) {
            คืน true;  // Followed by cased letter.
        } มิฉะนั้น {
            คืน false;  // Uncased and not case-ignorable.
        }
    }
    คืน false;  // Not followed by cased letter.
}

// Keep this consistent with the UTF-16 version in ustrcase.cpp and the Java version in CaseMap.java.
void toUpper(uint32_t options,
             const uint8_t *src, int32_t srcLength,
             ByteSink &sink, Edits *edits,
             UErrorCode &errorCode) {
    uint32_t state = 0;
    สำหรับ (int32_t i = 0; i < srcLength;) {
        int32_t nextIndex = i;
        UChar32 c;
        U8_NEXT(src, nextIndex, srcLength, c);
        uint32_t nextState = 0;
        int32_t type = ucase_getTypeOrIgnorable(c);
        ถ้า ((type & UCASE_IGNORABLE) != 0) {
            // c is case-ignorable
            nextState |= (state & AFTER_CASED);
        } มิฉะนั้น ถ้า (type != UCASE_NONE) {
            // c is cased
            nextState |= AFTER_CASED;
        }
        uint32_t data = getLetterData(c);
        ถ้า (data > 0) {
            uint32_t upper = data & UPPER_MASK;
            // Add a dialytika to this iota or ypsilon vowel
            // if we removed a tonos from the previous vowel,
            // and that previous vowel did not also have (or gain) a dialytika.
            // Adding one only to the final vowel in a longer sequence
            // (which does not occur in normal writing) would require lookahead.
            // Set the same flag as for preserving an existing dialytika.
            ถ้า ((data & HAS_VOWEL) != 0 &&
                (state & (AFTER_VOWEL_WITH_PRECOMPOSED_ACCENT | AFTER_VOWEL_WITH_COMBINING_ACCENT)) !=
                    0 &&
                (upper == 0x399 || upper == 0x3A5)) {
                data |= (state & AFTER_VOWEL_WITH_PRECOMPOSED_ACCENT) != 0 ? HAS_DIALYTIKA
                                                                           : HAS_COMBINING_DIALYTIKA;
            }
            int32_t numYpogegrammeni = 0;  // Map each one to a trailing, spacing, capital iota.
            ถ้า ((data & HAS_YPOGEGRAMMENI) != 0) {
                numYpogegrammeni = 1;
            }
            const UBool hasPrecomposedAccent = (data & HAS_ACCENT) != 0;
            // Skip combining diacritics after this Greek letter.
            int32_t nextNextIndex = nextIndex;
            ขณะ (nextIndex < srcLength) {
                UChar32 c2;
                U8_NEXT(src, nextNextIndex, srcLength, c2);
                uint32_t diacriticData = getDiacriticData(c2);
                ถ้า (diacriticData != 0) {
                    data |= diacriticData;
                    ถ้า ((diacriticData & HAS_YPOGEGRAMMENI) != 0) {
                        ++numYpogegrammeni;
                    }
                    nextIndex = nextNextIndex;
                } มิฉะนั้น {
                    bha_nga;  // not a Greek diacritic
                }
            }
            ถ้า ((data & HAS_VOWEL_AND_ACCENT_AND_DIALYTIKA) == HAS_VOWEL_AND_ACCENT) {
                nextState |= hasPrecomposedAccent ? AFTER_VOWEL_WITH_PRECOMPOSED_ACCENT
                                                  : AFTER_VOWEL_WITH_COMBINING_ACCENT;
            }
            // Map according to Greek rules.
            UBool addTonos = false;
            ถ้า (upper == 0x397 &&
                    (data & HAS_ACCENT) != 0 &&
                    numYpogegrammeni == 0 &&
                    (state & AFTER_CASED) == 0 &&
                    !isFollowedByCasedLetter(src, nextIndex, srcLength)) {
                // Keep disjunctive "or" with (only) a tonos.
                // We use the same "word boundary" conditions as for the Final_Sigma test.
                ถ้า (hasPrecomposedAccent) {
                    upper = 0x389;  // Preserve the precomposed form.
                } มิฉะนั้น {
                    addTonos = true;
                }
            } มิฉะนั้น ถ้า ((data & HAS_DIALYTIKA) != 0) {
                // Preserve a vowel with dialytika in precomposed form if it exists.
                ถ้า (upper == 0x399) {
                    upper = 0x3AA;
                    data &= ~HAS_EITHER_DIALYTIKA;
                } มิฉะนั้น ถ้า (upper == 0x3A5) {
                    upper = 0x3AB;
                    data &= ~HAS_EITHER_DIALYTIKA;
                }
            }

            UBool change;
            ถ้า (edits == nullptr && (options & U_OMIT_UNCHANGED_TEXT) == 0) {
                change = true;  // common, simple usage
            } มิฉะนั้น {
                // Find out first whether we are changing the text.
                U_ASSERT(0x370 <= upper && upper <= 0x3ff);  // 2-byte UTF-8, main Greek block
                change = (i + 2) > nextIndex ||
                        src[i] != getTwoByteLead(upper) || src[i + 1] != getTwoByteTrail(upper) ||
                        numYpogegrammeni > 0;
                int32_t i2 = i + 2;
                ถ้า ((data & HAS_EITHER_DIALYTIKA) != 0) {
                    change |= (i2 + 2) > nextIndex ||
                            src[i2] != static_cast<uint8_t>(u8"\u0308"[0]) ||
                            src[i2 + 1] != static_cast<uint8_t>(u8"\u0308"[1]);
                    i2 += 2;
                }
                ถ้า (addTonos) {
                    change |= (i2 + 2) > nextIndex ||
                            src[i2] != static_cast<uint8_t>(u8"\u0301"[0]) ||
                            src[i2 + 1] != static_cast<uint8_t>(u8"\u0301"[1]);
                    i2 += 2;
                }
                int32_t oldLength = nextIndex - i;
                int32_t newLength = (i2 - i) + numYpogegrammeni * 2;  // 2 bytes per U+0399
                change |= oldLength != newLength;
                ถ้า (change) {
                    ถ้า (edits != nullptr) {
                        edits->addReplace(oldLength, newLength);
                    }
                } มิฉะนั้น {
                    ถ้า (edits != nullptr) {
                        edits->addUnchanged(oldLength);
                    }
                    // Write unchanged text?
                    change = (options & U_OMIT_UNCHANGED_TEXT) == 0;
                }
            }

            ถ้า (change) {
                ByteSinkUtil::appendTwoBytes(upper, sink);
                ถ้า ((data & HAS_EITHER_DIALYTIKA) != 0) {
                    sink.AppendU8(u8"\u0308", 2);  // restore or add a dialytika
                }
                ถ้า (addTonos) {
                    sink.AppendU8(u8"\u0301", 2);
                }
                ขณะ (numYpogegrammeni > 0) {
                    sink.AppendU8(u8"\u0399", 2);
                    --numYpogegrammeni;
                }
            }
        } มิฉะนั้น ถ้า(c>=0) {
            const char16_t *s;
            c=ucase_toFullUpper(c, nullptr, nullptr, &s, UCASE_LOC_GREEK);
            ถ้า (!appendResult(nextIndex - i, c, s, sink, options, edits, errorCode)) {
                คืน;
            }
        } มิฉะนั้น {
            // Malformed UTF-8.
            ถ้า (!ByteSinkUtil::appendUnchanged(src+i, nextIndex-i,
                                               sink, options, edits, errorCode)) {
                คืน;
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
    ถ้า (caseLocale == UCASE_LOC_GREEK) {
        GreekUpper::toUpper(options, src, srcLength, sink, edits, errorCode);
    } มิฉะนั้น {
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
    ถ้า (U_FAILURE(errorCode)) {
        คืน;
    }
    ถ้า ((src == nullptr && srcLength != 0) || srcLength < -1) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        คืน;
    }

    // Get the string length.
    ถ้า (srcLength == -1) {
        srcLength = static_cast<int32_t>(uprv_strlen(src));
    }

    ถ้า (edits != nullptr && (options & U_EDITS_NO_RESET) == 0) {
        edits->reset();
    }
    stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                     reinterpret_cast<const uint8_t*>(src), srcLength, sink, edits, errorCode);
    sink.Flush();
    ถ้า (U_SUCCESS(errorCode)) {
        ถ้า (edits != nullptr) {
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
    ถ้า(U_FAILURE(errorCode)) {
        คืน 0;
    }
    ถ้า( destCapacity<0 ||
        (dest==nullptr && destCapacity>0) ||
        (src==nullptr && srcLength!=0) || srcLength<-1
    ) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        คืน 0;
    }

    /* get the string length */
    ถ้า(srcLength==-1) {
        srcLength = static_cast<int32_t>(uprv_strlen(src));
    }

    /* check for overlapping source and destination */
    ถ้า( dest!=nullptr &&
        ((src>=dest && src<(dest+destCapacity)) ||
         (dest>=src && dest<(src+srcLength)))
    ) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        คืน 0;
    }

    ถ้า (edits != nullptr && (options & U_EDITS_NO_RESET) == 0) {
        edits->reset();
    }
    int32_t reslen = ByteSinkUtil::viaByteSinkToTerminatedChars(
        dest, destCapacity,
        [&](ByteSink& sink, UErrorCode& status) {
            stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                             reinterpret_cast<const uint8_t*>(src), srcLength, sink, edits, status);
        },
        errorCode);
    ถ้า (U_SUCCESS(errorCode) && edits != nullptr) {
        edits->copyErrorTo(errorCode);
    }
    คืน reslen;
}

/* public API functions */

U_CAPI int32_t U_EXPORT2
ucasemap_utf8ToLower(const UCaseMap *csm,
                     var_na *dest, int32_t destCapacity,
                     const var_na *src, int32_t srcLength,
                     UErrorCode *pErrorCode) {
    auto *impl = reinterpret_cast<const UCaseMapImpl *>(csm);
    คืน ucasemap_mapUTF8(
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
    คืน ucasemap_mapUTF8(
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
    คืน ucasemap_mapUTF8(
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
    คืน ucasemap_mapUTF8(
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
    คืน ucasemap_mapUTF8(
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
    คืน ucasemap_mapUTF8(
        UCASE_LOC_ROOT, options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, destCapacity,
        src, srcLength,
        ucasemap_internalUTF8Fold, edits, errorCode);
}

U_NAMESPACE_END
