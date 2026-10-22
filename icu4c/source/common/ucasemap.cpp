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

#അന്തര്ഭാവ "unicode/utypes.h"
#അന്തര്ഭാവ "unicode/brkiter.h"
#അന്തര്ഭാവ "unicode/bytestream.h"
#അന്തര്ഭാവ "unicode/casemap.h"
#അന്തര്ഭാവ "unicode/edits.h"
#അന്തര്ഭാവ "unicode/stringoptions.h"
#അന്തര്ഭാവ "unicode/stringpiece.h"
#അന്തര്ഭാവ "unicode/ubrk.h"
#അന്തര്ഭാവ "unicode/uloc.h"
#അന്തര്ഭാവ "unicode/ustring.h"
#അന്തര്ഭാവ "unicode/ucasemap.h"
#എങ്കിൽ !UCONFIG_NO_BREAK_ITERATION
#അന്തര്ഭാവ "unicode/utext.h"
#endif
#അന്തര്ഭാവ "unicode/utf.h"
#അന്തര്ഭാവ "unicode/utf8.h"
#അന്തര്ഭാവ "unicode/utf16.h"
#അന്തര്ഭാവ "bytesinkutil.h"
#അന്തര്ഭാവ "cmemory.h"
#അന്തര്ഭാവ "cstring.h"
#അന്തര്ഭാവ "uassert.h"
#അന്തര്ഭാവ "ucase.h"
#അന്തര്ഭാവ "ucasemap_imp.h"

U_NAMESPACE_USE

/* UCaseMap service object -------------------------------------------------- */

// The public type, unicode/ucasemap.h struct UCaseMap, is an opaque C type.
// We define a parallel C++ type in the ICU namespace for entry point renaming.

U_NAMESPACE_BEGIN

UCaseMapImpl::UCaseMapImpl(സ്ഥിരം അക്ഷരം *localeID, uint32_t opts, UErrorCode *pErrorCode) :
#എങ്കിൽ !UCONFIG_NO_BREAK_ITERATION
        iter(nullptr),
#endif
        caseLocale(UCASE_LOC_UNKNOWN), options(opts) {
    ucasemap_setLocale(reinterpret_cast<UCaseMap *>(ഇത്), localeID, pErrorCode);
}

UCaseMapImpl::~UCaseMapImpl() {
#എങ്കിൽ !UCONFIG_NO_BREAK_ITERATION
    delete iter;
#endif
}

U_NAMESPACE_END

U_CAPI UCaseMap * U_EXPORT2
ucasemap_open(സ്ഥിരം അക്ഷരം *locale, uint32_t options, UErrorCode *pErrorCode) {
    എങ്കിൽ(U_FAILURE(*pErrorCode)) {
        തിരികെ nullptr;
    }
    UCaseMapImpl *csm = പുതിയ UCaseMapImpl(locale, options, pErrorCode);
    എങ്കിൽ(csm==nullptr) {
        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
        തിരികെ nullptr;
    } അല്ലെങ്കിൽ എങ്കിൽ (U_FAILURE(*pErrorCode)) {
        delete csm;
        തിരികെ nullptr;
    }
    തിരികെ reinterpret_cast<UCaseMap *>(csm);
}

U_CAPI ശൂന്യം U_EXPORT2
ucasemap_close(UCaseMap *csm) {
    delete reinterpret_cast<UCaseMapImpl *>(csm);
}

U_CAPI സ്ഥിരം അക്ഷരം* U_EXPORT2
ucasemap_getLocale(സ്ഥിരം UCaseMap* csm U_LIFETIME_BOUND) {
    തിരികെ reinterpret_cast<സ്ഥിരം UCaseMapImpl *>(csm)->locale;
}

U_CAPI uint32_t U_EXPORT2
ucasemap_getOptions(സ്ഥിരം UCaseMap *csm) {
    തിരികെ reinterpret_cast<സ്ഥിരം UCaseMapImpl *>(csm)->options;
}

U_CAPI ശൂന്യം U_EXPORT2
ucasemap_setLocale(UCaseMap *csm, സ്ഥിരം അക്ഷരം *locale, UErrorCode *pErrorCode) {
    എങ്കിൽ(U_FAILURE(*pErrorCode)) {
        തിരികെ;
    }
    auto *impl = reinterpret_cast<UCaseMapImpl *>(csm);
    എങ്കിൽ (locale != nullptr && *locale == 0) {
        impl->locale[0] = 0;
        impl->caseLocale = UCASE_LOC_വേര്;
        തിരികെ;
    }

    UErrorCode bufferStatus = U_ZERO_ERROR;
    int32_t length=uloc_getName(locale, impl->locale, (int32_t)വലുപ്പം(impl->locale), &bufferStatus);
    എങ്കിൽ(bufferStatus==U_BUFFER_OVERFLOW_ERROR || (U_SUCCESS(bufferStatus) && length==വലുപ്പം(impl->locale))) {
        bufferStatus = U_ZERO_ERROR;
        /* we only really need the language code for case mappings */
        length=uloc_getLanguage(locale, impl->locale, (int32_t)വലുപ്പം(impl->locale), &bufferStatus);
    }
    എങ്കിൽ(U_FAILURE(bufferStatus)) {
        *pErrorCode=bufferStatus;
    } അല്ലെങ്കിൽ എങ്കിൽ(length==വലുപ്പം(impl->locale)) {
        *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
    }
    എങ്കിൽ(U_SUCCESS(*pErrorCode)) {     
        impl->caseLocale = ucase_getCaseLocale(impl->locale);
    } അല്ലെങ്കിൽ {
        impl->locale[0]=0;
        impl->caseLocale = UCASE_LOC_വേര്;
    }
}

U_CAPI ശൂന്യം U_EXPORT2
ucasemap_setOptions(UCaseMap *csm, uint32_t options, UErrorCode *pErrorCode) {
    എങ്കിൽ(U_FAILURE(*pErrorCode)) {
        തിരികെ;
    }
    reinterpret_cast<UCaseMapImpl *>(csm)->options=options;
}

/* UTF-8 string case mappings ----------------------------------------------- */

/* TODO(markus): Move to a new, separate utf8case.cpp file. */

നാമാകാശ {

/* append a full case mapping result, see UCASE_MAX_STRING_LENGTH */
വരിയിൽ UBool
appendResult(int32_t cpLength, int32_t result, സ്ഥിരം char16_t *s,
             ByteSink &sink, uint32_t options, icu::Edits *edits, UErrorCode &errorCode) {
    U_ASSERT(U_SUCCESS(errorCode));

    /* decode the result */
    എങ്കിൽ(result<0) {
        /* (not) original code point */
        എങ്കിൽ(edits!=nullptr) {
            edits->addUnchanged(cpLength);
        }
        എങ്കിൽ((options & U_OMIT_UNCHANGED_TEXT) == 0) {
            ByteSinkUtil::appendCodePoint(cpLength, ~result, sink);
        }
    } അല്ലെങ്കിൽ {
        എങ്കിൽ(result<=UCASE_MAX_STRING_LENGTH) {
            // string: "result" is the UTF-16 length
            തിരികെ ByteSinkUtil::appendChange(cpLength, s, result, sink, edits, errorCode);
        } അല്ലെങ്കിൽ {
            ByteSinkUtil::appendCodePoint(cpLength, result, sink, edits);
        }
    }
    തിരികെ സത്യം;
}

// See unicode/utf8.h U8_APPEND_UNSAFE().
വരിയിൽ uint8_t getTwoByteLead(UChar32 c) { തിരികെ static_cast<uint8_t>((c >> 6) | 0xc0); }
വരിയിൽ uint8_t getTwoByteTrail(UChar32 c) { തിരികെ static_cast<uint8_t>((c & 0x3f) | 0x80); }

UChar32 U_CALLCONV
utf8_caseContextIterator(ശൂന്യം *context, int8_t dir) {
    UCaseContext* csc = static_cast<UCaseContext*>(context);
    UChar32 c;

    എങ്കിൽ(dir<0) {
        /* reset for backward iteration */
        csc->index=csc->cpStart;
        csc->dir=dir;
    } അല്ലെങ്കിൽ എങ്കിൽ(dir>0) {
        /* reset for forward iteration */
        csc->index=csc->cpLimit;
        csc->dir=dir;
    } അല്ലെങ്കിൽ {
        /* continue current iteration direction */
        dir=csc->dir;
    }

    എങ്കിൽ(dir<0) {
        എങ്കിൽ(csc->start<csc->index) {
            U8_PREV((സ്ഥിരം uint8_t *)csc->p, csc->start, csc->index, c);
            തിരികെ c;
        }
    } അല്ലെങ്കിൽ {
        എങ്കിൽ(csc->index<csc->limit) {
            U8_NEXT((സ്ഥിരം uint8_t *)csc->p, csc->index, csc->limit, c);
            തിരികെ c;
        }
    }
    തിരികെ U_SENTINEL;
}

/**
 * caseLocale >= 0: Lowercases [srcStart..srcLimit[ but takes context [0..srcLength[ into account.
 * caseLocale < 0: Case-folds [srcStart..srcLimit[.
 */
ശൂന്യം toLower(int32_t caseLocale, uint32_t options,
             സ്ഥിരം uint8_t *src, UCaseContext *csc, int32_t srcStart, int32_t srcLimit,
             icu::ByteSink &sink, icu::Edits *edits, UErrorCode &errorCode) {
    സ്ഥിരം int8_t *latinToLower;
    എങ്കിൽ (caseLocale == UCASE_LOC_വേര് ||
            (caseLocale >= 0 ?
                !(caseLocale == UCASE_LOC_TURKISH || caseLocale == UCASE_LOC_LITHUANIAN) :
                (options & _FOLD_CASE_OPTIONS_MASK) == U_FOLD_CASE_DEFAULT)) {
        latinToLower = LatinCase::TO_LOWER_NORMAL;
    } അല്ലെങ്കിൽ {
        latinToLower = LatinCase::TO_LOWER_TR_LT;
    }
    സ്ഥിരം UTrie2 *trie = ucase_getTrie();
    int32_t prev = srcStart;
    int32_t srcIndex = srcStart;
    വേണ്ടി (;;) {
        // fast path for simple cases
        int32_t cpStart;
        UChar32 c;
        വേണ്ടി (;;) {
            എങ്കിൽ (U_FAILURE(errorCode) || srcIndex >= srcLimit) {
                c = U_SENTINEL;
                നിർത്തുക;
            }
            uint8_t lead = src[srcIndex++];
            എങ്കിൽ (lead <= 0x7f) {
                int8_t d = latinToLower[lead];
                എങ്കിൽ (d == LatinCase::EXC) {
                    cpStart = srcIndex - 1;
                    c = lead;
                    നിർത്തുക;
                }
                എങ്കിൽ (d == 0) { തുടരുക; }
                ByteSinkUtil::appendUnchanged(src + prev, srcIndex - 1 - prev,
                                              sink, options, edits, errorCode);
                അക്ഷരം ascii = static_cast<അക്ഷരം>(lead + d);
                sink.Append(&ascii, 1);
                എങ്കിൽ (edits != nullptr) {
                    edits->addReplace(1, 1);
                }
                prev = srcIndex;
                തുടരുക;
            } അല്ലെങ്കിൽ എങ്കിൽ (lead < 0xe3) {
                uint8_t t;
                എങ്കിൽ (0xc2 <= lead && lead <= 0xc5 && srcIndex < srcLimit &&
                        (t = src[srcIndex] - 0x80) <= 0x3f) {
                    // U+0080..U+017F
                    ++srcIndex;
                    c = ((lead - 0xc0) << 6) | t;
                    int8_t d = latinToLower[c];
                    എങ്കിൽ (d == LatinCase::EXC) {
                        cpStart = srcIndex - 2;
                        നിർത്തുക;
                    }
                    എങ്കിൽ (d == 0) { തുടരുക; }
                    ByteSinkUtil::appendUnchanged(src + prev, srcIndex - 2 - prev,
                                                  sink, options, edits, errorCode);
                    ByteSinkUtil::appendTwoBytes(c + d, sink);
                    എങ്കിൽ (edits != nullptr) {
                        edits->addReplace(2, 2);
                    }
                    prev = srcIndex;
                    തുടരുക;
                }
            } അല്ലെങ്കിൽ എങ്കിൽ ((lead <= 0xe9 || lead == 0xeb || lead == 0xec) &&
                    (srcIndex + 2) <= srcLimit &&
                    U8_IS_TRAIL(src[srcIndex]) && U8_IS_TRAIL(src[srcIndex + 1])) {
                // most of CJK: no case mappings
                srcIndex += 2;
                തുടരുക;
            }
            cpStart = --srcIndex;
            U8_NEXT(src, srcIndex, srcLimit, c);
            എങ്കിൽ (c < 0) {
                // ill-formed UTF-8
                തുടരുക;
            }
            uint16_t props = UTRIE2_GET16(trie, c);
            എങ്കിൽ (UCASE_HAS_EXCEPTION(props)) { നിർത്തുക; }
            int32_t delta;
            എങ്കിൽ (!UCASE_IS_UPPER_OR_TITLE(props) || (delta = UCASE_GET_DELTA(props)) == 0) {
                തുടരുക;
            }
            ByteSinkUtil::appendUnchanged(src + prev, cpStart - prev,
                                          sink, options, edits, errorCode);
            ByteSinkUtil::appendCodePoint(srcIndex - cpStart, c + delta, sink, edits);
            prev = srcIndex;
        }
        എങ്കിൽ (c < 0) {
            നിർത്തുക;
        }
        // slow path
        സ്ഥിരം char16_t *s;
        എങ്കിൽ (caseLocale >= 0) {
            csc->cpStart = cpStart;
            csc->cpLimit = srcIndex;
            c = ucase_toFullLower(c, utf8_caseContextIterator, csc, &s, caseLocale);
        } അല്ലെങ്കിൽ {
            c = ucase_toFullFolding(c, &s, options);
        }
        എങ്കിൽ (c >= 0) {
            ByteSinkUtil::appendUnchanged(src + prev, cpStart - prev,
                                          sink, options, edits, errorCode);
            appendResult(srcIndex - cpStart, c, s, sink, options, edits, errorCode);
            prev = srcIndex;
        }
    }
    ByteSinkUtil::appendUnchanged(src + prev, srcIndex - prev,
                                  sink, options, edits, errorCode);
}

ശൂന്യം toUpper(int32_t caseLocale, uint32_t options,
             സ്ഥിരം uint8_t *src, UCaseContext *csc, int32_t srcLength,
             icu::ByteSink &sink, icu::Edits *edits, UErrorCode &errorCode) {
    സ്ഥിരം int8_t *latinToUpper;
    എങ്കിൽ (caseLocale == UCASE_LOC_TURKISH) {
        latinToUpper = LatinCase::TO_UPPER_TR;
    } അല്ലെങ്കിൽ {
        latinToUpper = LatinCase::TO_UPPER_NORMAL;
    }
    സ്ഥിരം UTrie2 *trie = ucase_getTrie();
    int32_t prev = 0;
    int32_t srcIndex = 0;
    വേണ്ടി (;;) {
        // fast path for simple cases
        int32_t cpStart;
        UChar32 c;
        വേണ്ടി (;;) {
            എങ്കിൽ (U_FAILURE(errorCode) || srcIndex >= srcLength) {
                c = U_SENTINEL;
                നിർത്തുക;
            }
            uint8_t lead = src[srcIndex++];
            എങ്കിൽ (lead <= 0x7f) {
                int8_t d = latinToUpper[lead];
                എങ്കിൽ (d == LatinCase::EXC) {
                    cpStart = srcIndex - 1;
                    c = lead;
                    നിർത്തുക;
                }
                എങ്കിൽ (d == 0) { തുടരുക; }
                ByteSinkUtil::appendUnchanged(src + prev, srcIndex - 1 - prev,
                                              sink, options, edits, errorCode);
                അക്ഷരം ascii = static_cast<അക്ഷരം>(lead + d);
                sink.Append(&ascii, 1);
                എങ്കിൽ (edits != nullptr) {
                    edits->addReplace(1, 1);
                }
                prev = srcIndex;
                തുടരുക;
            } അല്ലെങ്കിൽ എങ്കിൽ (lead < 0xe3) {
                uint8_t t;
                എങ്കിൽ (0xc2 <= lead && lead <= 0xc5 && srcIndex < srcLength &&
                        (t = src[srcIndex] - 0x80) <= 0x3f) {
                    // U+0080..U+017F
                    ++srcIndex;
                    c = ((lead - 0xc0) << 6) | t;
                    int8_t d = latinToUpper[c];
                    എങ്കിൽ (d == LatinCase::EXC) {
                        cpStart = srcIndex - 2;
                        നിർത്തുക;
                    }
                    എങ്കിൽ (d == 0) { തുടരുക; }
                    ByteSinkUtil::appendUnchanged(src + prev, srcIndex - 2 - prev,
                                                  sink, options, edits, errorCode);
                    ByteSinkUtil::appendTwoBytes(c + d, sink);
                    എങ്കിൽ (edits != nullptr) {
                        edits->addReplace(2, 2);
                    }
                    prev = srcIndex;
                    തുടരുക;
                }
            } അല്ലെങ്കിൽ എങ്കിൽ ((lead <= 0xe9 || lead == 0xeb || lead == 0xec) &&
                    (srcIndex + 2) <= srcLength &&
                    U8_IS_TRAIL(src[srcIndex]) && U8_IS_TRAIL(src[srcIndex + 1])) {
                // most of CJK: no case mappings
                srcIndex += 2;
                തുടരുക;
            }
            cpStart = --srcIndex;
            U8_NEXT(src, srcIndex, srcLength, c);
            എങ്കിൽ (c < 0) {
                // ill-formed UTF-8
                തുടരുക;
            }
            uint16_t props = UTRIE2_GET16(trie, c);
            എങ്കിൽ (UCASE_HAS_EXCEPTION(props)) { നിർത്തുക; }
            int32_t delta;
            എങ്കിൽ (UCASE_GET_TYPE(props) != UCASE_LOWER || (delta = UCASE_GET_DELTA(props)) == 0) {
                തുടരുക;
            }
            ByteSinkUtil::appendUnchanged(src + prev, cpStart - prev,
                                          sink, options, edits, errorCode);
            ByteSinkUtil::appendCodePoint(srcIndex - cpStart, c + delta, sink, edits);
            prev = srcIndex;
        }
        എങ്കിൽ (c < 0) {
            നിർത്തുക;
        }
        // slow path
        csc->cpStart = cpStart;
        csc->cpLimit = srcIndex;
        സ്ഥിരം char16_t *s;
        c = ucase_toFullUpper(c, utf8_caseContextIterator, csc, &s, caseLocale);
        എങ്കിൽ (c >= 0) {
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

#എങ്കിൽ !UCONFIG_NO_BREAK_ITERATION

നാമാകാശ {

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
int32_t maybeTitleDutchIJ(സ്ഥിരം uint8_t *src, UChar32 c, int32_t start, int32_t segmentLimit,
                          ByteSink &sink, uint32_t options, icu::Edits *edits, UErrorCode &errorCode) {
    U_ASSERT(start < segmentLimit);

    int32_t index = start;
    തർക്കം withAcute = അസത്യം;

    // If the conditions are met, then the following variables tell us what to output.
    int32_t unchanged1 = 0;  // code units before the j, or the whole sequence (0..3)
    തർക്കം doTitleJ = അസത്യം;  // true if the j needs to be titlecased
    int32_t unchanged2 = 0;  // after the j (0 or 1)

    // next character after the first letter
    UChar32 c2;
    c2 = src[index++];

    // Is the first letter an i/I with accent?
    എങ്കിൽ (c == u'I') {
        എങ്കിൽ (c2 == ACUTE_BYTE0 && index < segmentLimit && src[index++] == ACUTE_BYTE1) {
            withAcute = സത്യം;
            unchanged1 = 2;  // ACUTE is 2 code units in UTF-8
            എങ്കിൽ (index == segmentLimit) { തിരികെ start; }
            c2 = src[index++];
        }
    } അല്ലെങ്കിൽ {  // Í
        withAcute = സത്യം;
    }

    // Is the next character a j/J?
    എങ്കിൽ (c2 == u'j') {
        doTitleJ = സത്യം;
    } അല്ലെങ്കിൽ എങ്കിൽ (c2 == u'J') {
        ++unchanged1;
    } അല്ലെങ്കിൽ {
        തിരികെ start;
    }

    // A plain i/I must be followed by a plain j/J.
    // An i/I with acute must be followed by a j/J with acute.
    എങ്കിൽ (withAcute) {
        എങ്കിൽ ((index + 1) >= segmentLimit || src[index++] != ACUTE_BYTE0 || src[index++] != ACUTE_BYTE1) {
            തിരികെ start;
        }
        എങ്കിൽ (doTitleJ) {
            unchanged2 = 2;  // ACUTE is 2 code units in UTF-8
        } അല്ലെങ്കിൽ {
            unchanged1 = unchanged1 + 2;    // ACUTE is 2 code units in UTF-8
        }
    }

    // There must not be another combining mark.
    എങ്കിൽ (index < segmentLimit) {
        int32_t cp;
        int32_t i = index;
        U8_NEXT(src, i, segmentLimit, cp);
        uint32_t typeMask = U_GET_GC_MASK(cp);
        എങ്കിൽ ((typeMask & U_GC_M_MASK) != 0) {
            തിരികെ start;
        }
    }

    // Output the rest of the Dutch IJ.
    ByteSinkUtil::appendUnchanged(src + start, unchanged1, sink, options, edits, errorCode);
    start += unchanged1;
    എങ്കിൽ (doTitleJ) {
        ByteSinkUtil::appendCodePoint(1, u'J', sink, edits);
        ++start;
    }
    ByteSinkUtil::appendUnchanged(src + start, unchanged2, sink, options, edits, errorCode);

    U_ASSERT(start + unchanged2 == index);
    തിരികെ index;
}

}  // namespace

U_CFUNC ശൂന്യം U_CALLCONV
ucasemap_internalUTF8ToTitle(
        int32_t caseLocale, uint32_t options, BreakIterator *iter,
        സ്ഥിരം uint8_t *src, int32_t srcLength,
        ByteSink &sink, icu::Edits *edits,
        UErrorCode &errorCode) {
    എങ്കിൽ (!ustrcase_checkTitleAdjustmentOptions(options, errorCode)) {
        തിരികെ;
    }

    /* set up local variables */
    UCaseContext csc=UCASECONTEXT_INITIALIZER;
    csc.p=(ശൂന്യം *)src;
    csc.limit=srcLength;
    int32_t prev=0;
    UBool isFirstIndex=സത്യം;

    /* titlecasing loop */
    ആകുമ്പോൾ(prev<srcLength) {
        /* find next index where to titlecase */
        int32_t index;
        എങ്കിൽ(isFirstIndex) {
            isFirstIndex=അസത്യം;
            index=iter->first();
        } അല്ലെങ്കിൽ {
            index=iter->next();
        }
        എങ്കിൽ(index==UBRK_DONE || index>srcLength) {
            index=srcLength;
        }

        /*
         * Segment [prev..index[ into 3 parts:
         * a) skipped characters (copy as-is) [prev..titleStart[
         * b) first letter (titlecase)              [titleStart..titleLimit[
         * c) subsequent characters (lowercase)                 [titleLimit..index[
         */
        എങ്കിൽ(prev<index) {
            /* find and copy skipped characters [prev..titleStart[ */
            int32_t titleStart=prev;
            int32_t titleLimit=prev;
            UChar32 c;
            U8_NEXT(src, titleLimit, index, c);
            എങ്കിൽ ((options&U_TITLECASE_NO_BREAK_ADJUSTMENT)==0) {
                // Adjust the titlecasing index to the next cased character,
                // or to the next letter/number/symbol/private use.
                // Stop with titleStart<titleLimit<=index
                // if there is a character to be titlecased,
                // or else stop with titleStart==titleLimit==index.
                UBool toCased = (options&U_TITLECASE_ADJUST_TO_CASED) != 0;
                ആകുമ്പോൾ (toCased ? UCASE_NONE==ucase_getType(c) : !ustrcase_isLNS(c)) {
                    titleStart=titleLimit;
                    എങ്കിൽ(titleLimit==index) {
                        നിർത്തുക;
                    }
                    U8_NEXT(src, titleLimit, index, c);
                }
                എങ്കിൽ (prev < titleStart) {
                    എങ്കിൽ (!ByteSinkUtil::appendUnchanged(src+prev, titleStart-prev,
                                                       sink, options, edits, errorCode)) {
                        തിരികെ;
                    }
                }
            }

            എങ്കിൽ(titleStart<titleLimit) {
                /* titlecase c which is from [titleStart..titleLimit[ */
                എങ്കിൽ(c>=0) {
                    csc.cpStart=titleStart;
                    csc.cpLimit=titleLimit;
                    സ്ഥിരം char16_t *s;
                    c=ucase_toFullTitle(c, utf8_caseContextIterator, &csc, &s, caseLocale);
                    എങ്കിൽ (!appendResult(titleLimit-titleStart, c, s, sink, options, edits, errorCode)) {
                        തിരികെ;
                    }
                } അല്ലെങ്കിൽ {
                    // Malformed UTF-8.
                    എങ്കിൽ (!ByteSinkUtil::appendUnchanged(src+titleStart, titleLimit-titleStart,
                                                       sink, options, edits, errorCode)) {
                        തിരികെ;
                    }
                }

                /* Special case Dutch IJ titlecasing */
                എങ്കിൽ (titleLimit < index &&
                    caseLocale == UCASE_LOC_DUTCH) {
                    എങ്കിൽ (c < 0) {
                        c = ~c;
                    }

                    എങ്കിൽ (c == u'I' || c == u'Í') {
                        titleLimit = maybeTitleDutchIJ(src, c, titleLimit, index, sink, options, edits, errorCode);
                    }
                }

                /* lowercase [titleLimit..index[ */
                എങ്കിൽ(titleLimit<index) {
                    എങ്കിൽ((options&U_TITLECASE_NO_LOWERCASE)==0) {
                        /* Normal operation: Lowercase the rest of the word. */
                        toLower(caseLocale, options,
                                src, &csc, titleLimit, index,
                                sink, edits, errorCode);
                        എങ്കിൽ(U_FAILURE(errorCode)) {
                            തിരികെ;
                        }
                    } അല്ലെങ്കിൽ {
                        /* Optionally just copy the rest of the word unchanged. */
                        എങ്കിൽ (!ByteSinkUtil::appendUnchanged(src+titleLimit, index-titleLimit,
                                                           sink, options, edits, errorCode)) {
                            തിരികെ;
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
നാമാകാശ GreekUpper {

UBool isFollowedByCasedLetter(സ്ഥിരം uint8_t *s, int32_t i, int32_t length) {
    ആകുമ്പോൾ (i < length) {
        UChar32 c;
        U8_NEXT(s, i, length, c);
        int32_t type = ucase_getTypeOrIgnorable(c);
        എങ്കിൽ ((type & UCASE_IGNORABLE) != 0) {
            // Case-ignorable, continue with the loop.
        } അല്ലെങ്കിൽ എങ്കിൽ (type != UCASE_NONE) {
            തിരികെ സത്യം;  // Followed by cased letter.
        } അല്ലെങ്കിൽ {
            തിരികെ അസത്യം;  // Uncased and not case-ignorable.
        }
    }
    തിരികെ അസത്യം;  // Not followed by cased letter.
}

// Keep this consistent with the UTF-16 version in ustrcase.cpp and the Java version in CaseMap.java.
ശൂന്യം toUpper(uint32_t options,
             സ്ഥിരം uint8_t *src, int32_t srcLength,
             ByteSink &sink, Edits *edits,
             UErrorCode &errorCode) {
    uint32_t state = 0;
    വേണ്ടി (int32_t i = 0; i < srcLength;) {
        int32_t nextIndex = i;
        UChar32 c;
        U8_NEXT(src, nextIndex, srcLength, c);
        uint32_t nextState = 0;
        int32_t type = ucase_getTypeOrIgnorable(c);
        എങ്കിൽ ((type & UCASE_IGNORABLE) != 0) {
            // c is case-ignorable
            nextState |= (state & AFTER_CASED);
        } അല്ലെങ്കിൽ എങ്കിൽ (type != UCASE_NONE) {
            // c is cased
            nextState |= AFTER_CASED;
        }
        uint32_t data = getLetterData(c);
        എങ്കിൽ (data > 0) {
            uint32_t upper = data & UPPER_MASK;
            // Add a dialytika to this iota or ypsilon vowel
            // if we removed a tonos from the previous vowel,
            // and that previous vowel did not also have (or gain) a dialytika.
            // Adding one only to the final vowel in a longer sequence
            // (which does not occur in normal writing) would require lookahead.
            // Set the same flag as for preserving an existing dialytika.
            എങ്കിൽ ((data & HAS_VOWEL) != 0 &&
                (state & (AFTER_VOWEL_WITH_PRECOMPOSED_ACCENT | AFTER_VOWEL_WITH_COMBINING_ACCENT)) !=
                    0 &&
                (upper == 0x399 || upper == 0x3A5)) {
                data |= (state & AFTER_VOWEL_WITH_PRECOMPOSED_ACCENT) != 0 ? HAS_DIALYTIKA
                                                                           : HAS_COMBINING_DIALYTIKA;
            }
            int32_t അക്കം_Ypogegrammeni = 0;  // Map each one to a trailing, spacing, capital iota.
            എങ്കിൽ ((data & HAS_YPOGEGRAMMENI) != 0) {
                അക്കം_Ypogegrammeni = 1;
            }
            സ്ഥിരം UBool hasPrecomposedAccent = (data & HAS_ACCENT) != 0;
            // Skip combining diacritics after this Greek letter.
            int32_t nextNextIndex = nextIndex;
            ആകുമ്പോൾ (nextIndex < srcLength) {
                UChar32 c2;
                U8_NEXT(src, nextNextIndex, srcLength, c2);
                uint32_t diacriticData = getDiacriticData(c2);
                എങ്കിൽ (diacriticData != 0) {
                    data |= diacriticData;
                    എങ്കിൽ ((diacriticData & HAS_YPOGEGRAMMENI) != 0) {
                        ++അക്കം_Ypogegrammeni;
                    }
                    nextIndex = nextNextIndex;
                } അല്ലെങ്കിൽ {
                    നിർത്തുക;  // not a Greek diacritic
                }
            }
            എങ്കിൽ ((data & HAS_VOWEL_AND_ACCENT_AND_DIALYTIKA) == HAS_VOWEL_AND_ACCENT) {
                nextState |= hasPrecomposedAccent ? AFTER_VOWEL_WITH_PRECOMPOSED_ACCENT
                                                  : AFTER_VOWEL_WITH_COMBINING_ACCENT;
            }
            // Map according to Greek rules.
            UBool addTonos = അസത്യം;
            എങ്കിൽ (upper == 0x397 &&
                    (data & HAS_ACCENT) != 0 &&
                    അക്കം_Ypogegrammeni == 0 &&
                    (state & AFTER_CASED) == 0 &&
                    !isFollowedByCasedLetter(src, nextIndex, srcLength)) {
                // Keep disjunctive "or" with (only) a tonos.
                // We use the same "word boundary" conditions as for the Final_Sigma test.
                എങ്കിൽ (hasPrecomposedAccent) {
                    upper = 0x389;  // Preserve the precomposed form.
                } അല്ലെങ്കിൽ {
                    addTonos = സത്യം;
                }
            } അല്ലെങ്കിൽ എങ്കിൽ ((data & HAS_DIALYTIKA) != 0) {
                // Preserve a vowel with dialytika in precomposed form if it exists.
                എങ്കിൽ (upper == 0x399) {
                    upper = 0x3AA;
                    data &= ~HAS_EITHER_DIALYTIKA;
                } അല്ലെങ്കിൽ എങ്കിൽ (upper == 0x3A5) {
                    upper = 0x3AB;
                    data &= ~HAS_EITHER_DIALYTIKA;
                }
            }

            UBool change;
            എങ്കിൽ (edits == nullptr && (options & U_OMIT_UNCHANGED_TEXT) == 0) {
                change = സത്യം;  // common, simple usage
            } അല്ലെങ്കിൽ {
                // Find out first whether we are changing the text.
                U_ASSERT(0x370 <= upper && upper <= 0x3ff);  // 2-byte UTF-8, main Greek block
                change = (i + 2) > nextIndex ||
                        src[i] != getTwoByteLead(upper) || src[i + 1] != getTwoByteTrail(upper) ||
                        അക്കം_Ypogegrammeni > 0;
                int32_t i2 = i + 2;
                എങ്കിൽ ((data & HAS_EITHER_DIALYTIKA) != 0) {
                    change |= (i2 + 2) > nextIndex ||
                            src[i2] != static_cast<uint8_t>(u8"\u0308"[0]) ||
                            src[i2 + 1] != static_cast<uint8_t>(u8"\u0308"[1]);
                    i2 += 2;
                }
                എങ്കിൽ (addTonos) {
                    change |= (i2 + 2) > nextIndex ||
                            src[i2] != static_cast<uint8_t>(u8"\u0301"[0]) ||
                            src[i2 + 1] != static_cast<uint8_t>(u8"\u0301"[1]);
                    i2 += 2;
                }
                int32_t oldLength = nextIndex - i;
                int32_t newLength = (i2 - i) + അക്കം_Ypogegrammeni * 2;  // 2 bytes per U+0399
                change |= oldLength != newLength;
                എങ്കിൽ (change) {
                    എങ്കിൽ (edits != nullptr) {
                        edits->addReplace(oldLength, newLength);
                    }
                } അല്ലെങ്കിൽ {
                    എങ്കിൽ (edits != nullptr) {
                        edits->addUnchanged(oldLength);
                    }
                    // Write unchanged text?
                    change = (options & U_OMIT_UNCHANGED_TEXT) == 0;
                }
            }

            എങ്കിൽ (change) {
                ByteSinkUtil::appendTwoBytes(upper, sink);
                എങ്കിൽ ((data & HAS_EITHER_DIALYTIKA) != 0) {
                    sink.AppendU8(u8"\u0308", 2);  // restore or add a dialytika
                }
                എങ്കിൽ (addTonos) {
                    sink.AppendU8(u8"\u0301", 2);
                }
                ആകുമ്പോൾ (അക്കം_Ypogegrammeni > 0) {
                    sink.AppendU8(u8"\u0399", 2);
                    --അക്കം_Ypogegrammeni;
                }
            }
        } അല്ലെങ്കിൽ എങ്കിൽ(c>=0) {
            സ്ഥിരം char16_t *s;
            c=ucase_toFullUpper(c, nullptr, nullptr, &s, UCASE_LOC_GREEK);
            എങ്കിൽ (!appendResult(nextIndex - i, c, s, sink, options, edits, errorCode)) {
                തിരികെ;
            }
        } അല്ലെങ്കിൽ {
            // Malformed UTF-8.
            എങ്കിൽ (!ByteSinkUtil::appendUnchanged(src+i, nextIndex-i,
                                               sink, options, edits, errorCode)) {
                തിരികെ;
            }
        }
        i = nextIndex;
        state = nextState;
    }
}

}  // namespace GreekUpper
U_NAMESPACE_END

സ്ഥിതം ശൂന്യം U_CALLCONV
ucasemap_internalUTF8ToLower(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_UNUSED
                             സ്ഥിരം uint8_t *src, int32_t srcLength,
                             icu::ByteSink &sink, icu::Edits *edits,
                             UErrorCode &errorCode) {
    UCaseContext csc=UCASECONTEXT_INITIALIZER;
    csc.p=(ശൂന്യം *)src;
    csc.limit=srcLength;
    toLower(
        caseLocale, options,
        src, &csc, 0, srcLength,
        sink, edits, errorCode);
}

സ്ഥിതം ശൂന്യം U_CALLCONV
ucasemap_internalUTF8ToUpper(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_UNUSED
                             സ്ഥിരം uint8_t *src, int32_t srcLength,
                             icu::ByteSink &sink, icu::Edits *edits,
                             UErrorCode &errorCode) {
    എങ്കിൽ (caseLocale == UCASE_LOC_GREEK) {
        GreekUpper::toUpper(options, src, srcLength, sink, edits, errorCode);
    } അല്ലെങ്കിൽ {
        UCaseContext csc=UCASECONTEXT_INITIALIZER;
        csc.p=(ശൂന്യം *)src;
        csc.limit=srcLength;
        toUpper(
            caseLocale, options,
            src, &csc, srcLength,
            sink, edits, errorCode);
    }
}

സ്ഥിതം ശൂന്യം U_CALLCONV
ucasemap_internalUTF8Fold(int32_t /* caseLocale */, uint32_t options, UCASEMAP_BREAK_ITERATOR_UNUSED
                          സ്ഥിരം uint8_t *src, int32_t srcLength,
                          icu::ByteSink &sink, icu::Edits *edits,
                          UErrorCode &errorCode) {
    toLower(
        -1, options,
        src, nullptr, 0, srcLength,
        sink, edits, errorCode);
}

ശൂന്യം
ucasemap_mapUTF8(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_PARAM
                 സ്ഥിരം അക്ഷരം *src, int32_t srcLength,
                 UTF8CaseMapper *stringCaseMapper,
                 icu::ByteSink &sink, icu::Edits *edits,
                 UErrorCode &errorCode) {
    /* check argument values */
    എങ്കിൽ (U_FAILURE(errorCode)) {
        തിരികെ;
    }
    എങ്കിൽ ((src == nullptr && srcLength != 0) || srcLength < -1) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ;
    }

    // Get the string length.
    എങ്കിൽ (srcLength == -1) {
        srcLength = static_cast<int32_t>(uprv_strlen(src));
    }

    എങ്കിൽ (edits != nullptr && (options & U_EDITS_NO_RESET) == 0) {
        edits->reset();
    }
    stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                     reinterpret_cast<സ്ഥിരം uint8_t*>(src), srcLength, sink, edits, errorCode);
    sink.Flush();
    എങ്കിൽ (U_SUCCESS(errorCode)) {
        എങ്കിൽ (edits != nullptr) {
            edits->copyErrorTo(errorCode);
        }
    }
}

int32_t
ucasemap_mapUTF8(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_PARAM
                 അക്ഷരം *dest, int32_t destCapacity,
                 സ്ഥിരം അക്ഷരം *src, int32_t srcLength,
                 UTF8CaseMapper *stringCaseMapper,
                 icu::Edits *edits,
                 UErrorCode &errorCode) {
    /* check argument values */
    എങ്കിൽ(U_FAILURE(errorCode)) {
        തിരികെ 0;
    }
    എങ്കിൽ( destCapacity<0 ||
        (dest==nullptr && destCapacity>0) ||
        (src==nullptr && srcLength!=0) || srcLength<-1
    ) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ 0;
    }

    /* get the string length */
    എങ്കിൽ(srcLength==-1) {
        srcLength = static_cast<int32_t>(uprv_strlen(src));
    }

    /* check for overlapping source and destination */
    എങ്കിൽ( dest!=nullptr &&
        ((src>=dest && src<(dest+destCapacity)) ||
         (dest>=src && dest<(src+srcLength)))
    ) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ 0;
    }

    എങ്കിൽ (edits != nullptr && (options & U_EDITS_NO_RESET) == 0) {
        edits->reset();
    }
    int32_t reslen = ByteSinkUtil::viaByteSinkToTerminatedChars(
        dest, destCapacity,
        [&](ByteSink& sink, UErrorCode& status) {
            stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                             reinterpret_cast<സ്ഥിരം uint8_t*>(src), srcLength, sink, edits, status);
        },
        errorCode);
    എങ്കിൽ (U_SUCCESS(errorCode) && edits != nullptr) {
        edits->copyErrorTo(errorCode);
    }
    തിരികെ reslen;
}

/* public API functions */

U_CAPI int32_t U_EXPORT2
ucasemap_utf8ToLower(സ്ഥിരം UCaseMap *csm,
                     അക്ഷരം *dest, int32_t destCapacity,
                     സ്ഥിരം അക്ഷരം *src, int32_t srcLength,
                     UErrorCode *pErrorCode) {
    auto *impl = reinterpret_cast<സ്ഥിരം UCaseMapImpl *>(csm);
    തിരികെ ucasemap_mapUTF8(
        impl->caseLocale, impl->options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, destCapacity,
        src, srcLength,
        ucasemap_internalUTF8ToLower, nullptr, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
ucasemap_utf8ToUpper(സ്ഥിരം UCaseMap *csm,
                     അക്ഷരം *dest, int32_t destCapacity,
                     സ്ഥിരം അക്ഷരം *src, int32_t srcLength,
                     UErrorCode *pErrorCode) {
    auto *impl = reinterpret_cast<സ്ഥിരം UCaseMapImpl *>(csm);
    തിരികെ ucasemap_mapUTF8(
        impl->caseLocale, impl->options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, destCapacity,
        src, srcLength,
        ucasemap_internalUTF8ToUpper, nullptr, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
ucasemap_utf8FoldCase(സ്ഥിരം UCaseMap *csm,
                      അക്ഷരം *dest, int32_t destCapacity,
                      സ്ഥിരം അക്ഷരം *src, int32_t srcLength,
                      UErrorCode *pErrorCode) {
    auto *impl = reinterpret_cast<സ്ഥിരം UCaseMapImpl *>(csm);
    തിരികെ ucasemap_mapUTF8(
        UCASE_LOC_വേര്, impl->options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, destCapacity,
        src, srcLength,
        ucasemap_internalUTF8Fold, nullptr, *pErrorCode);
}

U_NAMESPACE_BEGIN

ശൂന്യം CaseMap::utf8ToLower(
        സ്ഥിരം അക്ഷരം *locale, uint32_t options,
        StringPiece src, ByteSink &sink, Edits *edits,
        UErrorCode &errorCode) {
    ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        src.data(), src.length(),
        ucasemap_internalUTF8ToLower, sink, edits, errorCode);
}

ശൂന്യം CaseMap::utf8ToUpper(
        സ്ഥിരം അക്ഷരം *locale, uint32_t options,
        StringPiece src, ByteSink &sink, Edits *edits,
        UErrorCode &errorCode) {
    ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        src.data(), src.length(),
        ucasemap_internalUTF8ToUpper, sink, edits, errorCode);
}

ശൂന്യം CaseMap::utf8Fold(
        uint32_t options,
        StringPiece src, ByteSink &sink, Edits *edits,
        UErrorCode &errorCode) {
    ucasemap_mapUTF8(
        UCASE_LOC_വേര്, options, UCASEMAP_BREAK_ITERATOR_NULL
        src.data(), src.length(),
        ucasemap_internalUTF8Fold, sink, edits, errorCode);
}

int32_t CaseMap::utf8ToLower(
        സ്ഥിരം അക്ഷരം *locale, uint32_t options,
        സ്ഥിരം അക്ഷരം *src, int32_t srcLength,
        അക്ഷരം *dest, int32_t destCapacity, Edits *edits,
        UErrorCode &errorCode) {
    തിരികെ ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, destCapacity,
        src, srcLength,
        ucasemap_internalUTF8ToLower, edits, errorCode);
}

int32_t CaseMap::utf8ToUpper(
        സ്ഥിരം അക്ഷരം *locale, uint32_t options,
        സ്ഥിരം അക്ഷരം *src, int32_t srcLength,
        അക്ഷരം *dest, int32_t destCapacity, Edits *edits,
        UErrorCode &errorCode) {
    തിരികെ ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, destCapacity,
        src, srcLength,
        ucasemap_internalUTF8ToUpper, edits, errorCode);
}

int32_t CaseMap::utf8Fold(
        uint32_t options,
        സ്ഥിരം അക്ഷരം *src, int32_t srcLength,
        അക്ഷരം *dest, int32_t destCapacity, Edits *edits,
        UErrorCode &errorCode) {
    തിരികെ ucasemap_mapUTF8(
        UCASE_LOC_വേര്, options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, destCapacity,
        src, srcLength,
        ucasemap_internalUTF8Fold, edits, errorCode);
}

U_NAMESPACE_END
