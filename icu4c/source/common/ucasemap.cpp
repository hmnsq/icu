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

#அந்தர்பாவ "unicode/utypes.h"
#அந்தர்பாவ "unicode/brkiter.h"
#அந்தர்பாவ "unicode/bytestream.h"
#அந்தர்பாவ "unicode/casemap.h"
#அந்தர்பாவ "unicode/edits.h"
#அந்தர்பாவ "unicode/stringoptions.h"
#அந்தர்பாவ "unicode/stringpiece.h"
#அந்தர்பாவ "unicode/ubrk.h"
#அந்தர்பாவ "unicode/uloc.h"
#அந்தர்பாவ "unicode/ustring.h"
#அந்தர்பாவ "unicode/ucasemap.h"
#எனில் !UCONFIG_NO_BREAK_ITERATION
#அந்தர்பாவ "unicode/utext.h"
#endif
#அந்தர்பாவ "unicode/utf.h"
#அந்தர்பாவ "unicode/utf8.h"
#அந்தர்பாவ "unicode/utf16.h"
#அந்தர்பாவ "bytesinkutil.h"
#அந்தர்பாவ "cmemory.h"
#அந்தர்பாவ "cstring.h"
#அந்தர்பாவ "uassert.h"
#அந்தர்பாவ "ucase.h"
#அந்தர்பாவ "ucasemap_imp.h"

U_NAMESPACE_USE

/* UCaseMap service object -------------------------------------------------- */

// The public type, unicode/ucasemap.h struct UCaseMap, is an opaque C type.
// We define a parallel C++ type in the ICU namespace for entry point renaming.

U_NAMESPACE_BEGIN

UCaseMapImpl::UCaseMapImpl(மாறிலி எழுத்து *localeID, uint32_t opts, UErrorCode *pErrorCode) :
#எனில் !UCONFIG_NO_BREAK_ITERATION
        iter(nullptr),
#endif
        caseLocale(UCASE_LOC_UNKNOWN), options(opts) {
    ucasemap_setLocale(reinterpret_cast<UCaseMap *>(இது), localeID, pErrorCode);
}

UCaseMapImpl::~UCaseMapImpl() {
#எனில் !UCONFIG_NO_BREAK_ITERATION
    delete iter;
#endif
}

U_NAMESPACE_END

U_CAPI UCaseMap * U_EXPORT2
ucasemap_open(மாறிலி எழுத்து *locale, uint32_t options, UErrorCode *pErrorCode) {
    எனில்(U_FAILURE(*pErrorCode)) {
        திரும்பு nullptr;
    }
    UCaseMapImpl *csm = புதிய UCaseMapImpl(locale, options, pErrorCode);
    எனில்(csm==nullptr) {
        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
        திரும்பு nullptr;
    } இல்லையெனில் எனில் (U_FAILURE(*pErrorCode)) {
        delete csm;
        திரும்பு nullptr;
    }
    திரும்பு reinterpret_cast<UCaseMap *>(csm);
}

U_CAPI வெறுமை U_EXPORT2
ucasemap_close(UCaseMap *csm) {
    delete reinterpret_cast<UCaseMapImpl *>(csm);
}

U_CAPI மாறிலி எழுத்து* U_EXPORT2
ucasemap_getLocale(மாறிலி UCaseMap* csm U_LIFETIME_BOUND) {
    திரும்பு reinterpret_cast<மாறிலி UCaseMapImpl *>(csm)->locale;
}

U_CAPI uint32_t U_EXPORT2
ucasemap_getOptions(மாறிலி UCaseMap *csm) {
    திரும்பு reinterpret_cast<மாறிலி UCaseMapImpl *>(csm)->options;
}

U_CAPI வெறுமை U_EXPORT2
ucasemap_setLocale(UCaseMap *csm, மாறிலி எழுத்து *locale, UErrorCode *pErrorCode) {
    எனில்(U_FAILURE(*pErrorCode)) {
        திரும்பு;
    }
    auto *impl = reinterpret_cast<UCaseMapImpl *>(csm);
    எனில் (locale != nullptr && *locale == 0) {
        impl->locale[0] = 0;
        impl->caseLocale = UCASE_LOC_வேர்;
        திரும்பு;
    }

    UErrorCode bufferStatus = U_ZERO_ERROR;
    int32_t length=uloc_getName(locale, impl->locale, (int32_t)அளவு(impl->locale), &bufferStatus);
    எனில்(bufferStatus==U_BUFFER_OVERFLOW_ERROR || (U_SUCCESS(bufferStatus) && length==அளவு(impl->locale))) {
        bufferStatus = U_ZERO_ERROR;
        /* we only really need the language code for case mappings */
        length=uloc_getLanguage(locale, impl->locale, (int32_t)அளவு(impl->locale), &bufferStatus);
    }
    எனில்(U_FAILURE(bufferStatus)) {
        *pErrorCode=bufferStatus;
    } இல்லையெனில் எனில்(length==அளவு(impl->locale)) {
        *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
    }
    எனில்(U_SUCCESS(*pErrorCode)) {     
        impl->caseLocale = ucase_getCaseLocale(impl->locale);
    } இல்லையெனில் {
        impl->locale[0]=0;
        impl->caseLocale = UCASE_LOC_வேர்;
    }
}

U_CAPI வெறுமை U_EXPORT2
ucasemap_setOptions(UCaseMap *csm, uint32_t options, UErrorCode *pErrorCode) {
    எனில்(U_FAILURE(*pErrorCode)) {
        திரும்பு;
    }
    reinterpret_cast<UCaseMapImpl *>(csm)->options=options;
}

/* UTF-8 string case mappings ----------------------------------------------- */

/* TODO(markus): Move to a new, separate utf8case.cpp file. */

நாமாகாஶ {

/* append a full case mapping result, see UCASE_MAX_STRING_LENGTH */
உள்வரி UBool
appendResult(int32_t cpLength, int32_t result, மாறிலி char16_t *s,
             ByteSink &sink, uint32_t options, icu::Edits *edits, UErrorCode &errorCode) {
    U_ASSERT(U_SUCCESS(errorCode));

    /* decode the result */
    எனில்(result<0) {
        /* (not) original code point */
        எனில்(edits!=nullptr) {
            edits->addUnchanged(cpLength);
        }
        எனில்((options & U_OMIT_UNCHANGED_TEXT) == 0) {
            ByteSinkUtil::appendCodePoint(cpLength, ~result, sink);
        }
    } இல்லையெனில் {
        எனில்(result<=UCASE_MAX_STRING_LENGTH) {
            // string: "result" is the UTF-16 length
            திரும்பு ByteSinkUtil::appendChange(cpLength, s, result, sink, edits, errorCode);
        } இல்லையெனில் {
            ByteSinkUtil::appendCodePoint(cpLength, result, sink, edits);
        }
    }
    திரும்பு உண்மை;
}

// See unicode/utf8.h U8_APPEND_UNSAFE().
உள்வரி uint8_t getTwoByteLead(UChar32 c) { திரும்பு static_cast<uint8_t>((c >> 6) | 0xc0); }
உள்வரி uint8_t getTwoByteTrail(UChar32 c) { திரும்பு static_cast<uint8_t>((c & 0x3f) | 0x80); }

UChar32 U_CALLCONV
utf8_caseContextIterator(வெறுமை *context, int8_t dir) {
    UCaseContext* csc = static_cast<UCaseContext*>(context);
    UChar32 c;

    எனில்(dir<0) {
        /* reset for backward iteration */
        csc->index=csc->cpStart;
        csc->dir=dir;
    } இல்லையெனில் எனில்(dir>0) {
        /* reset for forward iteration */
        csc->index=csc->cpLimit;
        csc->dir=dir;
    } இல்லையெனில் {
        /* continue current iteration direction */
        dir=csc->dir;
    }

    எனில்(dir<0) {
        எனில்(csc->start<csc->index) {
            U8_PREV((மாறிலி uint8_t *)csc->p, csc->start, csc->index, c);
            திரும்பு c;
        }
    } இல்லையெனில் {
        எனில்(csc->index<csc->limit) {
            U8_NEXT((மாறிலி uint8_t *)csc->p, csc->index, csc->limit, c);
            திரும்பு c;
        }
    }
    திரும்பு U_SENTINEL;
}

/**
 * caseLocale >= 0: Lowercases [srcStart..srcLimit[ but takes context [0..srcLength[ into account.
 * caseLocale < 0: Case-folds [srcStart..srcLimit[.
 */
வெறுமை toLower(int32_t caseLocale, uint32_t options,
             மாறிலி uint8_t *src, UCaseContext *csc, int32_t srcStart, int32_t srcLimit,
             icu::ByteSink &sink, icu::Edits *edits, UErrorCode &errorCode) {
    மாறிலி int8_t *latinToLower;
    எனில் (caseLocale == UCASE_LOC_வேர் ||
            (caseLocale >= 0 ?
                !(caseLocale == UCASE_LOC_TURKISH || caseLocale == UCASE_LOC_LITHUANIAN) :
                (options & _FOLD_CASE_OPTIONS_MASK) == U_FOLD_CASE_DEFAULT)) {
        latinToLower = LatinCase::TO_LOWER_NORMAL;
    } இல்லையெனில் {
        latinToLower = LatinCase::TO_LOWER_TR_LT;
    }
    மாறிலி UTrie2 *trie = ucase_getTrie();
    int32_t prev = srcStart;
    int32_t srcIndex = srcStart;
    க்கு (;;) {
        // fast path for simple cases
        int32_t cpStart;
        UChar32 c;
        க்கு (;;) {
            எனில் (U_FAILURE(errorCode) || srcIndex >= srcLimit) {
                c = U_SENTINEL;
                நிறுத்து;
            }
            uint8_t lead = src[srcIndex++];
            எனில் (lead <= 0x7f) {
                int8_t d = latinToLower[lead];
                எனில் (d == LatinCase::EXC) {
                    cpStart = srcIndex - 1;
                    c = lead;
                    நிறுத்து;
                }
                எனில் (d == 0) { தொடர்; }
                ByteSinkUtil::appendUnchanged(src + prev, srcIndex - 1 - prev,
                                              sink, options, edits, errorCode);
                எழுத்து ascii = static_cast<எழுத்து>(lead + d);
                sink.Append(&ascii, 1);
                எனில் (edits != nullptr) {
                    edits->addReplace(1, 1);
                }
                prev = srcIndex;
                தொடர்;
            } இல்லையெனில் எனில் (lead < 0xe3) {
                uint8_t t;
                எனில் (0xc2 <= lead && lead <= 0xc5 && srcIndex < srcLimit &&
                        (t = src[srcIndex] - 0x80) <= 0x3f) {
                    // U+0080..U+017F
                    ++srcIndex;
                    c = ((lead - 0xc0) << 6) | t;
                    int8_t d = latinToLower[c];
                    எனில் (d == LatinCase::EXC) {
                        cpStart = srcIndex - 2;
                        நிறுத்து;
                    }
                    எனில் (d == 0) { தொடர்; }
                    ByteSinkUtil::appendUnchanged(src + prev, srcIndex - 2 - prev,
                                                  sink, options, edits, errorCode);
                    ByteSinkUtil::appendTwoBytes(c + d, sink);
                    எனில் (edits != nullptr) {
                        edits->addReplace(2, 2);
                    }
                    prev = srcIndex;
                    தொடர்;
                }
            } இல்லையெனில் எனில் ((lead <= 0xe9 || lead == 0xeb || lead == 0xec) &&
                    (srcIndex + 2) <= srcLimit &&
                    U8_IS_TRAIL(src[srcIndex]) && U8_IS_TRAIL(src[srcIndex + 1])) {
                // most of CJK: no case mappings
                srcIndex += 2;
                தொடர்;
            }
            cpStart = --srcIndex;
            U8_NEXT(src, srcIndex, srcLimit, c);
            எனில் (c < 0) {
                // ill-formed UTF-8
                தொடர்;
            }
            uint16_t props = UTRIE2_GET16(trie, c);
            எனில் (UCASE_HAS_EXCEPTION(props)) { நிறுத்து; }
            int32_t delta;
            எனில் (!UCASE_IS_UPPER_OR_TITLE(props) || (delta = UCASE_GET_DELTA(props)) == 0) {
                தொடர்;
            }
            ByteSinkUtil::appendUnchanged(src + prev, cpStart - prev,
                                          sink, options, edits, errorCode);
            ByteSinkUtil::appendCodePoint(srcIndex - cpStart, c + delta, sink, edits);
            prev = srcIndex;
        }
        எனில் (c < 0) {
            நிறுத்து;
        }
        // slow path
        மாறிலி char16_t *s;
        எனில் (caseLocale >= 0) {
            csc->cpStart = cpStart;
            csc->cpLimit = srcIndex;
            c = ucase_toFullLower(c, utf8_caseContextIterator, csc, &s, caseLocale);
        } இல்லையெனில் {
            c = ucase_toFullFolding(c, &s, options);
        }
        எனில் (c >= 0) {
            ByteSinkUtil::appendUnchanged(src + prev, cpStart - prev,
                                          sink, options, edits, errorCode);
            appendResult(srcIndex - cpStart, c, s, sink, options, edits, errorCode);
            prev = srcIndex;
        }
    }
    ByteSinkUtil::appendUnchanged(src + prev, srcIndex - prev,
                                  sink, options, edits, errorCode);
}

வெறுமை toUpper(int32_t caseLocale, uint32_t options,
             மாறிலி uint8_t *src, UCaseContext *csc, int32_t srcLength,
             icu::ByteSink &sink, icu::Edits *edits, UErrorCode &errorCode) {
    மாறிலி int8_t *latinToUpper;
    எனில் (caseLocale == UCASE_LOC_TURKISH) {
        latinToUpper = LatinCase::TO_UPPER_TR;
    } இல்லையெனில் {
        latinToUpper = LatinCase::TO_UPPER_NORMAL;
    }
    மாறிலி UTrie2 *trie = ucase_getTrie();
    int32_t prev = 0;
    int32_t srcIndex = 0;
    க்கு (;;) {
        // fast path for simple cases
        int32_t cpStart;
        UChar32 c;
        க்கு (;;) {
            எனில் (U_FAILURE(errorCode) || srcIndex >= srcLength) {
                c = U_SENTINEL;
                நிறுத்து;
            }
            uint8_t lead = src[srcIndex++];
            எனில் (lead <= 0x7f) {
                int8_t d = latinToUpper[lead];
                எனில் (d == LatinCase::EXC) {
                    cpStart = srcIndex - 1;
                    c = lead;
                    நிறுத்து;
                }
                எனில் (d == 0) { தொடர்; }
                ByteSinkUtil::appendUnchanged(src + prev, srcIndex - 1 - prev,
                                              sink, options, edits, errorCode);
                எழுத்து ascii = static_cast<எழுத்து>(lead + d);
                sink.Append(&ascii, 1);
                எனில் (edits != nullptr) {
                    edits->addReplace(1, 1);
                }
                prev = srcIndex;
                தொடர்;
            } இல்லையெனில் எனில் (lead < 0xe3) {
                uint8_t t;
                எனில் (0xc2 <= lead && lead <= 0xc5 && srcIndex < srcLength &&
                        (t = src[srcIndex] - 0x80) <= 0x3f) {
                    // U+0080..U+017F
                    ++srcIndex;
                    c = ((lead - 0xc0) << 6) | t;
                    int8_t d = latinToUpper[c];
                    எனில் (d == LatinCase::EXC) {
                        cpStart = srcIndex - 2;
                        நிறுத்து;
                    }
                    எனில் (d == 0) { தொடர்; }
                    ByteSinkUtil::appendUnchanged(src + prev, srcIndex - 2 - prev,
                                                  sink, options, edits, errorCode);
                    ByteSinkUtil::appendTwoBytes(c + d, sink);
                    எனில் (edits != nullptr) {
                        edits->addReplace(2, 2);
                    }
                    prev = srcIndex;
                    தொடர்;
                }
            } இல்லையெனில் எனில் ((lead <= 0xe9 || lead == 0xeb || lead == 0xec) &&
                    (srcIndex + 2) <= srcLength &&
                    U8_IS_TRAIL(src[srcIndex]) && U8_IS_TRAIL(src[srcIndex + 1])) {
                // most of CJK: no case mappings
                srcIndex += 2;
                தொடர்;
            }
            cpStart = --srcIndex;
            U8_NEXT(src, srcIndex, srcLength, c);
            எனில் (c < 0) {
                // ill-formed UTF-8
                தொடர்;
            }
            uint16_t props = UTRIE2_GET16(trie, c);
            எனில் (UCASE_HAS_EXCEPTION(props)) { நிறுத்து; }
            int32_t delta;
            எனில் (UCASE_GET_TYPE(props) != UCASE_LOWER || (delta = UCASE_GET_DELTA(props)) == 0) {
                தொடர்;
            }
            ByteSinkUtil::appendUnchanged(src + prev, cpStart - prev,
                                          sink, options, edits, errorCode);
            ByteSinkUtil::appendCodePoint(srcIndex - cpStart, c + delta, sink, edits);
            prev = srcIndex;
        }
        எனில் (c < 0) {
            நிறுத்து;
        }
        // slow path
        csc->cpStart = cpStart;
        csc->cpLimit = srcIndex;
        மாறிலி char16_t *s;
        c = ucase_toFullUpper(c, utf8_caseContextIterator, csc, &s, caseLocale);
        எனில் (c >= 0) {
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

#எனில் !UCONFIG_NO_BREAK_ITERATION

நாமாகாஶ {

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
int32_t maybeTitleDutchIJ(மாறிலி uint8_t *src, UChar32 c, int32_t start, int32_t segmentLimit,
                          ByteSink &sink, uint32_t options, icu::Edits *edits, UErrorCode &errorCode) {
    U_ASSERT(start < segmentLimit);

    int32_t index = start;
    தருக்க withAcute = பொய்;

    // If the conditions are met, then the following variables tell us what to output.
    int32_t unchanged1 = 0;  // code units before the j, or the whole sequence (0..3)
    தருக்க doTitleJ = பொய்;  // true if the j needs to be titlecased
    int32_t unchanged2 = 0;  // after the j (0 or 1)

    // next character after the first letter
    UChar32 c2;
    c2 = src[index++];

    // Is the first letter an i/I with accent?
    எனில் (c == u'I') {
        எனில் (c2 == ACUTE_BYTE0 && index < segmentLimit && src[index++] == ACUTE_BYTE1) {
            withAcute = உண்மை;
            unchanged1 = 2;  // ACUTE is 2 code units in UTF-8
            எனில் (index == segmentLimit) { திரும்பு start; }
            c2 = src[index++];
        }
    } இல்லையெனில் {  // Í
        withAcute = உண்மை;
    }

    // Is the next character a j/J?
    எனில் (c2 == u'j') {
        doTitleJ = உண்மை;
    } இல்லையெனில் எனில் (c2 == u'J') {
        ++unchanged1;
    } இல்லையெனில் {
        திரும்பு start;
    }

    // A plain i/I must be followed by a plain j/J.
    // An i/I with acute must be followed by a j/J with acute.
    எனில் (withAcute) {
        எனில் ((index + 1) >= segmentLimit || src[index++] != ACUTE_BYTE0 || src[index++] != ACUTE_BYTE1) {
            திரும்பு start;
        }
        எனில் (doTitleJ) {
            unchanged2 = 2;  // ACUTE is 2 code units in UTF-8
        } இல்லையெனில் {
            unchanged1 = unchanged1 + 2;    // ACUTE is 2 code units in UTF-8
        }
    }

    // There must not be another combining mark.
    எனில் (index < segmentLimit) {
        int32_t cp;
        int32_t i = index;
        U8_NEXT(src, i, segmentLimit, cp);
        uint32_t typeMask = U_GET_GC_MASK(cp);
        எனில் ((typeMask & U_GC_M_MASK) != 0) {
            திரும்பு start;
        }
    }

    // Output the rest of the Dutch IJ.
    ByteSinkUtil::appendUnchanged(src + start, unchanged1, sink, options, edits, errorCode);
    start += unchanged1;
    எனில் (doTitleJ) {
        ByteSinkUtil::appendCodePoint(1, u'J', sink, edits);
        ++start;
    }
    ByteSinkUtil::appendUnchanged(src + start, unchanged2, sink, options, edits, errorCode);

    U_ASSERT(start + unchanged2 == index);
    திரும்பு index;
}

}  // namespace

U_CFUNC வெறுமை U_CALLCONV
ucasemap_internalUTF8ToTitle(
        int32_t caseLocale, uint32_t options, BreakIterator *iter,
        மாறிலி uint8_t *src, int32_t srcLength,
        ByteSink &sink, icu::Edits *edits,
        UErrorCode &errorCode) {
    எனில் (!ustrcase_checkTitleAdjustmentOptions(options, errorCode)) {
        திரும்பு;
    }

    /* set up local variables */
    UCaseContext csc=UCASECONTEXT_INITIALIZER;
    csc.p=(வெறுமை *)src;
    csc.limit=srcLength;
    int32_t prev=0;
    UBool isFirstIndex=உண்மை;

    /* titlecasing loop */
    போது(prev<srcLength) {
        /* find next index where to titlecase */
        int32_t index;
        எனில்(isFirstIndex) {
            isFirstIndex=பொய்;
            index=iter->first();
        } இல்லையெனில் {
            index=iter->next();
        }
        எனில்(index==UBRK_DONE || index>srcLength) {
            index=srcLength;
        }

        /*
         * Segment [prev..index[ into 3 parts:
         * a) skipped characters (copy as-is) [prev..titleStart[
         * b) first letter (titlecase)              [titleStart..titleLimit[
         * c) subsequent characters (lowercase)                 [titleLimit..index[
         */
        எனில்(prev<index) {
            /* find and copy skipped characters [prev..titleStart[ */
            int32_t titleStart=prev;
            int32_t titleLimit=prev;
            UChar32 c;
            U8_NEXT(src, titleLimit, index, c);
            எனில் ((options&U_TITLECASE_NO_BREAK_ADJUSTMENT)==0) {
                // Adjust the titlecasing index to the next cased character,
                // or to the next letter/number/symbol/private use.
                // Stop with titleStart<titleLimit<=index
                // if there is a character to be titlecased,
                // or else stop with titleStart==titleLimit==index.
                UBool toCased = (options&U_TITLECASE_ADJUST_TO_CASED) != 0;
                போது (toCased ? UCASE_NONE==ucase_getType(c) : !ustrcase_isLNS(c)) {
                    titleStart=titleLimit;
                    எனில்(titleLimit==index) {
                        நிறுத்து;
                    }
                    U8_NEXT(src, titleLimit, index, c);
                }
                எனில் (prev < titleStart) {
                    எனில் (!ByteSinkUtil::appendUnchanged(src+prev, titleStart-prev,
                                                       sink, options, edits, errorCode)) {
                        திரும்பு;
                    }
                }
            }

            எனில்(titleStart<titleLimit) {
                /* titlecase c which is from [titleStart..titleLimit[ */
                எனில்(c>=0) {
                    csc.cpStart=titleStart;
                    csc.cpLimit=titleLimit;
                    மாறிலி char16_t *s;
                    c=ucase_toFullTitle(c, utf8_caseContextIterator, &csc, &s, caseLocale);
                    எனில் (!appendResult(titleLimit-titleStart, c, s, sink, options, edits, errorCode)) {
                        திரும்பு;
                    }
                } இல்லையெனில் {
                    // Malformed UTF-8.
                    எனில் (!ByteSinkUtil::appendUnchanged(src+titleStart, titleLimit-titleStart,
                                                       sink, options, edits, errorCode)) {
                        திரும்பு;
                    }
                }

                /* Special case Dutch IJ titlecasing */
                எனில் (titleLimit < index &&
                    caseLocale == UCASE_LOC_DUTCH) {
                    எனில் (c < 0) {
                        c = ~c;
                    }

                    எனில் (c == u'I' || c == u'Í') {
                        titleLimit = maybeTitleDutchIJ(src, c, titleLimit, index, sink, options, edits, errorCode);
                    }
                }

                /* lowercase [titleLimit..index[ */
                எனில்(titleLimit<index) {
                    எனில்((options&U_TITLECASE_NO_LOWERCASE)==0) {
                        /* Normal operation: Lowercase the rest of the word. */
                        toLower(caseLocale, options,
                                src, &csc, titleLimit, index,
                                sink, edits, errorCode);
                        எனில்(U_FAILURE(errorCode)) {
                            திரும்பு;
                        }
                    } இல்லையெனில் {
                        /* Optionally just copy the rest of the word unchanged. */
                        எனில் (!ByteSinkUtil::appendUnchanged(src+titleLimit, index-titleLimit,
                                                           sink, options, edits, errorCode)) {
                            திரும்பு;
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
நாமாகாஶ GreekUpper {

UBool isFollowedByCasedLetter(மாறிலி uint8_t *s, int32_t i, int32_t length) {
    போது (i < length) {
        UChar32 c;
        U8_NEXT(s, i, length, c);
        int32_t type = ucase_getTypeOrIgnorable(c);
        எனில் ((type & UCASE_IGNORABLE) != 0) {
            // Case-ignorable, continue with the loop.
        } இல்லையெனில் எனில் (type != UCASE_NONE) {
            திரும்பு உண்மை;  // Followed by cased letter.
        } இல்லையெனில் {
            திரும்பு பொய்;  // Uncased and not case-ignorable.
        }
    }
    திரும்பு பொய்;  // Not followed by cased letter.
}

// Keep this consistent with the UTF-16 version in ustrcase.cpp and the Java version in CaseMap.java.
வெறுமை toUpper(uint32_t options,
             மாறிலி uint8_t *src, int32_t srcLength,
             ByteSink &sink, Edits *edits,
             UErrorCode &errorCode) {
    uint32_t state = 0;
    க்கு (int32_t i = 0; i < srcLength;) {
        int32_t nextIndex = i;
        UChar32 c;
        U8_NEXT(src, nextIndex, srcLength, c);
        uint32_t nextState = 0;
        int32_t type = ucase_getTypeOrIgnorable(c);
        எனில் ((type & UCASE_IGNORABLE) != 0) {
            // c is case-ignorable
            nextState |= (state & AFTER_CASED);
        } இல்லையெனில் எனில் (type != UCASE_NONE) {
            // c is cased
            nextState |= AFTER_CASED;
        }
        uint32_t data = getLetterData(c);
        எனில் (data > 0) {
            uint32_t upper = data & UPPER_MASK;
            // Add a dialytika to this iota or ypsilon vowel
            // if we removed a tonos from the previous vowel,
            // and that previous vowel did not also have (or gain) a dialytika.
            // Adding one only to the final vowel in a longer sequence
            // (which does not occur in normal writing) would require lookahead.
            // Set the same flag as for preserving an existing dialytika.
            எனில் ((data & HAS_VOWEL) != 0 &&
                (state & (AFTER_VOWEL_WITH_PRECOMPOSED_ACCENT | AFTER_VOWEL_WITH_COMBINING_ACCENT)) !=
                    0 &&
                (upper == 0x399 || upper == 0x3A5)) {
                data |= (state & AFTER_VOWEL_WITH_PRECOMPOSED_ACCENT) != 0 ? HAS_DIALYTIKA
                                                                           : HAS_COMBINING_DIALYTIKA;
            }
            int32_t இலக்கம்_Ypogegrammeni = 0;  // Map each one to a trailing, spacing, capital iota.
            எனில் ((data & HAS_YPOGEGRAMMENI) != 0) {
                இலக்கம்_Ypogegrammeni = 1;
            }
            மாறிலி UBool hasPrecomposedAccent = (data & HAS_ACCENT) != 0;
            // Skip combining diacritics after this Greek letter.
            int32_t nextNextIndex = nextIndex;
            போது (nextIndex < srcLength) {
                UChar32 c2;
                U8_NEXT(src, nextNextIndex, srcLength, c2);
                uint32_t diacriticData = getDiacriticData(c2);
                எனில் (diacriticData != 0) {
                    data |= diacriticData;
                    எனில் ((diacriticData & HAS_YPOGEGRAMMENI) != 0) {
                        ++இலக்கம்_Ypogegrammeni;
                    }
                    nextIndex = nextNextIndex;
                } இல்லையெனில் {
                    நிறுத்து;  // not a Greek diacritic
                }
            }
            எனில் ((data & HAS_VOWEL_AND_ACCENT_AND_DIALYTIKA) == HAS_VOWEL_AND_ACCENT) {
                nextState |= hasPrecomposedAccent ? AFTER_VOWEL_WITH_PRECOMPOSED_ACCENT
                                                  : AFTER_VOWEL_WITH_COMBINING_ACCENT;
            }
            // Map according to Greek rules.
            UBool addTonos = பொய்;
            எனில் (upper == 0x397 &&
                    (data & HAS_ACCENT) != 0 &&
                    இலக்கம்_Ypogegrammeni == 0 &&
                    (state & AFTER_CASED) == 0 &&
                    !isFollowedByCasedLetter(src, nextIndex, srcLength)) {
                // Keep disjunctive "or" with (only) a tonos.
                // We use the same "word boundary" conditions as for the Final_Sigma test.
                எனில் (hasPrecomposedAccent) {
                    upper = 0x389;  // Preserve the precomposed form.
                } இல்லையெனில் {
                    addTonos = உண்மை;
                }
            } இல்லையெனில் எனில் ((data & HAS_DIALYTIKA) != 0) {
                // Preserve a vowel with dialytika in precomposed form if it exists.
                எனில் (upper == 0x399) {
                    upper = 0x3AA;
                    data &= ~HAS_EITHER_DIALYTIKA;
                } இல்லையெனில் எனில் (upper == 0x3A5) {
                    upper = 0x3AB;
                    data &= ~HAS_EITHER_DIALYTIKA;
                }
            }

            UBool change;
            எனில் (edits == nullptr && (options & U_OMIT_UNCHANGED_TEXT) == 0) {
                change = உண்மை;  // common, simple usage
            } இல்லையெனில் {
                // Find out first whether we are changing the text.
                U_ASSERT(0x370 <= upper && upper <= 0x3ff);  // 2-byte UTF-8, main Greek block
                change = (i + 2) > nextIndex ||
                        src[i] != getTwoByteLead(upper) || src[i + 1] != getTwoByteTrail(upper) ||
                        இலக்கம்_Ypogegrammeni > 0;
                int32_t i2 = i + 2;
                எனில் ((data & HAS_EITHER_DIALYTIKA) != 0) {
                    change |= (i2 + 2) > nextIndex ||
                            src[i2] != static_cast<uint8_t>(u8"\u0308"[0]) ||
                            src[i2 + 1] != static_cast<uint8_t>(u8"\u0308"[1]);
                    i2 += 2;
                }
                எனில் (addTonos) {
                    change |= (i2 + 2) > nextIndex ||
                            src[i2] != static_cast<uint8_t>(u8"\u0301"[0]) ||
                            src[i2 + 1] != static_cast<uint8_t>(u8"\u0301"[1]);
                    i2 += 2;
                }
                int32_t oldLength = nextIndex - i;
                int32_t newLength = (i2 - i) + இலக்கம்_Ypogegrammeni * 2;  // 2 bytes per U+0399
                change |= oldLength != newLength;
                எனில் (change) {
                    எனில் (edits != nullptr) {
                        edits->addReplace(oldLength, newLength);
                    }
                } இல்லையெனில் {
                    எனில் (edits != nullptr) {
                        edits->addUnchanged(oldLength);
                    }
                    // Write unchanged text?
                    change = (options & U_OMIT_UNCHANGED_TEXT) == 0;
                }
            }

            எனில் (change) {
                ByteSinkUtil::appendTwoBytes(upper, sink);
                எனில் ((data & HAS_EITHER_DIALYTIKA) != 0) {
                    sink.AppendU8(u8"\u0308", 2);  // restore or add a dialytika
                }
                எனில் (addTonos) {
                    sink.AppendU8(u8"\u0301", 2);
                }
                போது (இலக்கம்_Ypogegrammeni > 0) {
                    sink.AppendU8(u8"\u0399", 2);
                    --இலக்கம்_Ypogegrammeni;
                }
            }
        } இல்லையெனில் எனில்(c>=0) {
            மாறிலி char16_t *s;
            c=ucase_toFullUpper(c, nullptr, nullptr, &s, UCASE_LOC_GREEK);
            எனில் (!appendResult(nextIndex - i, c, s, sink, options, edits, errorCode)) {
                திரும்பு;
            }
        } இல்லையெனில் {
            // Malformed UTF-8.
            எனில் (!ByteSinkUtil::appendUnchanged(src+i, nextIndex-i,
                                               sink, options, edits, errorCode)) {
                திரும்பு;
            }
        }
        i = nextIndex;
        state = nextState;
    }
}

}  // namespace GreekUpper
U_NAMESPACE_END

நிலையான வெறுமை U_CALLCONV
ucasemap_internalUTF8ToLower(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_UNUSED
                             மாறிலி uint8_t *src, int32_t srcLength,
                             icu::ByteSink &sink, icu::Edits *edits,
                             UErrorCode &errorCode) {
    UCaseContext csc=UCASECONTEXT_INITIALIZER;
    csc.p=(வெறுமை *)src;
    csc.limit=srcLength;
    toLower(
        caseLocale, options,
        src, &csc, 0, srcLength,
        sink, edits, errorCode);
}

நிலையான வெறுமை U_CALLCONV
ucasemap_internalUTF8ToUpper(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_UNUSED
                             மாறிலி uint8_t *src, int32_t srcLength,
                             icu::ByteSink &sink, icu::Edits *edits,
                             UErrorCode &errorCode) {
    எனில் (caseLocale == UCASE_LOC_GREEK) {
        GreekUpper::toUpper(options, src, srcLength, sink, edits, errorCode);
    } இல்லையெனில் {
        UCaseContext csc=UCASECONTEXT_INITIALIZER;
        csc.p=(வெறுமை *)src;
        csc.limit=srcLength;
        toUpper(
            caseLocale, options,
            src, &csc, srcLength,
            sink, edits, errorCode);
    }
}

நிலையான வெறுமை U_CALLCONV
ucasemap_internalUTF8Fold(int32_t /* caseLocale */, uint32_t options, UCASEMAP_BREAK_ITERATOR_UNUSED
                          மாறிலி uint8_t *src, int32_t srcLength,
                          icu::ByteSink &sink, icu::Edits *edits,
                          UErrorCode &errorCode) {
    toLower(
        -1, options,
        src, nullptr, 0, srcLength,
        sink, edits, errorCode);
}

வெறுமை
ucasemap_mapUTF8(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_PARAM
                 மாறிலி எழுத்து *src, int32_t srcLength,
                 UTF8CaseMapper *stringCaseMapper,
                 icu::ByteSink &sink, icu::Edits *edits,
                 UErrorCode &errorCode) {
    /* check argument values */
    எனில் (U_FAILURE(errorCode)) {
        திரும்பு;
    }
    எனில் ((src == nullptr && srcLength != 0) || srcLength < -1) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு;
    }

    // Get the string length.
    எனில் (srcLength == -1) {
        srcLength = static_cast<int32_t>(uprv_strlen(src));
    }

    எனில் (edits != nullptr && (options & U_EDITS_NO_RESET) == 0) {
        edits->reset();
    }
    stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                     reinterpret_cast<மாறிலி uint8_t*>(src), srcLength, sink, edits, errorCode);
    sink.Flush();
    எனில் (U_SUCCESS(errorCode)) {
        எனில் (edits != nullptr) {
            edits->copyErrorTo(errorCode);
        }
    }
}

int32_t
ucasemap_mapUTF8(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_PARAM
                 எழுத்து *dest, int32_t destCapacity,
                 மாறிலி எழுத்து *src, int32_t srcLength,
                 UTF8CaseMapper *stringCaseMapper,
                 icu::Edits *edits,
                 UErrorCode &errorCode) {
    /* check argument values */
    எனில்(U_FAILURE(errorCode)) {
        திரும்பு 0;
    }
    எனில்( destCapacity<0 ||
        (dest==nullptr && destCapacity>0) ||
        (src==nullptr && srcLength!=0) || srcLength<-1
    ) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு 0;
    }

    /* get the string length */
    எனில்(srcLength==-1) {
        srcLength = static_cast<int32_t>(uprv_strlen(src));
    }

    /* check for overlapping source and destination */
    எனில்( dest!=nullptr &&
        ((src>=dest && src<(dest+destCapacity)) ||
         (dest>=src && dest<(src+srcLength)))
    ) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு 0;
    }

    எனில் (edits != nullptr && (options & U_EDITS_NO_RESET) == 0) {
        edits->reset();
    }
    int32_t reslen = ByteSinkUtil::viaByteSinkToTerminatedChars(
        dest, destCapacity,
        [&](ByteSink& sink, UErrorCode& status) {
            stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                             reinterpret_cast<மாறிலி uint8_t*>(src), srcLength, sink, edits, status);
        },
        errorCode);
    எனில் (U_SUCCESS(errorCode) && edits != nullptr) {
        edits->copyErrorTo(errorCode);
    }
    திரும்பு reslen;
}

/* public API functions */

U_CAPI int32_t U_EXPORT2
ucasemap_utf8ToLower(மாறிலி UCaseMap *csm,
                     எழுத்து *dest, int32_t destCapacity,
                     மாறிலி எழுத்து *src, int32_t srcLength,
                     UErrorCode *pErrorCode) {
    auto *impl = reinterpret_cast<மாறிலி UCaseMapImpl *>(csm);
    திரும்பு ucasemap_mapUTF8(
        impl->caseLocale, impl->options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, destCapacity,
        src, srcLength,
        ucasemap_internalUTF8ToLower, nullptr, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
ucasemap_utf8ToUpper(மாறிலி UCaseMap *csm,
                     எழுத்து *dest, int32_t destCapacity,
                     மாறிலி எழுத்து *src, int32_t srcLength,
                     UErrorCode *pErrorCode) {
    auto *impl = reinterpret_cast<மாறிலி UCaseMapImpl *>(csm);
    திரும்பு ucasemap_mapUTF8(
        impl->caseLocale, impl->options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, destCapacity,
        src, srcLength,
        ucasemap_internalUTF8ToUpper, nullptr, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
ucasemap_utf8FoldCase(மாறிலி UCaseMap *csm,
                      எழுத்து *dest, int32_t destCapacity,
                      மாறிலி எழுத்து *src, int32_t srcLength,
                      UErrorCode *pErrorCode) {
    auto *impl = reinterpret_cast<மாறிலி UCaseMapImpl *>(csm);
    திரும்பு ucasemap_mapUTF8(
        UCASE_LOC_வேர், impl->options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, destCapacity,
        src, srcLength,
        ucasemap_internalUTF8Fold, nullptr, *pErrorCode);
}

U_NAMESPACE_BEGIN

வெறுமை CaseMap::utf8ToLower(
        மாறிலி எழுத்து *locale, uint32_t options,
        StringPiece src, ByteSink &sink, Edits *edits,
        UErrorCode &errorCode) {
    ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        src.data(), src.length(),
        ucasemap_internalUTF8ToLower, sink, edits, errorCode);
}

வெறுமை CaseMap::utf8ToUpper(
        மாறிலி எழுத்து *locale, uint32_t options,
        StringPiece src, ByteSink &sink, Edits *edits,
        UErrorCode &errorCode) {
    ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        src.data(), src.length(),
        ucasemap_internalUTF8ToUpper, sink, edits, errorCode);
}

வெறுமை CaseMap::utf8Fold(
        uint32_t options,
        StringPiece src, ByteSink &sink, Edits *edits,
        UErrorCode &errorCode) {
    ucasemap_mapUTF8(
        UCASE_LOC_வேர், options, UCASEMAP_BREAK_ITERATOR_NULL
        src.data(), src.length(),
        ucasemap_internalUTF8Fold, sink, edits, errorCode);
}

int32_t CaseMap::utf8ToLower(
        மாறிலி எழுத்து *locale, uint32_t options,
        மாறிலி எழுத்து *src, int32_t srcLength,
        எழுத்து *dest, int32_t destCapacity, Edits *edits,
        UErrorCode &errorCode) {
    திரும்பு ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, destCapacity,
        src, srcLength,
        ucasemap_internalUTF8ToLower, edits, errorCode);
}

int32_t CaseMap::utf8ToUpper(
        மாறிலி எழுத்து *locale, uint32_t options,
        மாறிலி எழுத்து *src, int32_t srcLength,
        எழுத்து *dest, int32_t destCapacity, Edits *edits,
        UErrorCode &errorCode) {
    திரும்பு ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, destCapacity,
        src, srcLength,
        ucasemap_internalUTF8ToUpper, edits, errorCode);
}

int32_t CaseMap::utf8Fold(
        uint32_t options,
        மாறிலி எழுத்து *src, int32_t srcLength,
        எழுத்து *dest, int32_t destCapacity, Edits *edits,
        UErrorCode &errorCode) {
    திரும்பு ucasemap_mapUTF8(
        UCASE_LOC_வேர், options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, destCapacity,
        src, srcLength,
        ucasemap_internalUTF8Fold, edits, errorCode);
}

U_NAMESPACE_END
