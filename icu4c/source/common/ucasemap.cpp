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

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/brkiter.h"
#अन्तर्भाव "unicode/bytestream.h"
#अन्तर्भाव "unicode/casemap.h"
#अन्तर्भाव "unicode/edits.h"
#अन्तर्भाव "unicode/stringoptions.h"
#अन्तर्भाव "unicode/stringpiece.h"
#अन्तर्भाव "unicode/ubrk.h"
#अन्तर्भाव "unicode/uloc.h"
#अन्तर्भाव "unicode/ustring.h"
#अन्तर्भाव "unicode/ucasemap.h"
#जइ !UCONFIG_NO_BREAK_ITERATION
#अन्तर्भाव "unicode/utext.h"
#endif
#अन्तर्भाव "unicode/utf.h"
#अन्तर्भाव "unicode/utf8.h"
#अन्तर्भाव "unicode/utf16.h"
#अन्तर्भाव "bytesinkutil.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "cstring.h"
#अन्तर्भाव "uassert.h"
#अन्तर्भाव "ucase.h"
#अन्तर्भाव "ucasemap_imp.h"

U_NAMESPACE_USE

/* UCaseMap service object -------------------------------------------------- */

// The public type, unicode/ucasemap.h struct UCaseMap, is an opaque C type.
// We define a parallel C++ type in the ICU namespace for entry point renaming.

U_NAMESPACE_BEGIN

UCaseMapImpl::UCaseMapImpl(ध्रुव वर्ण *localeID, uint32_t opts, UErrorCode *pErrorCode) :
#जइ !UCONFIG_NO_BREAK_ITERATION
        iter(nullptr),
#endif
        caseLocale(UCASE_LOC_UNKNOWN), options(opts) {
    ucasemap_setLocale(reinterpret_cast<UCaseMap *>(एतत्), localeID, pErrorCode);
}

UCaseMapImpl::~UCaseMapImpl() {
#जइ !UCONFIG_NO_BREAK_ITERATION
    delete iter;
#endif
}

U_NAMESPACE_END

U_CAPI UCaseMap * U_EXPORT2
ucasemap_open(ध्रुव वर्ण *locale, uint32_t options, UErrorCode *pErrorCode) {
    जइ(U_FAILURE(*pErrorCode)) {
        प्रत्यागम nullptr;
    }
    UCaseMapImpl *csm = नूतन UCaseMapImpl(locale, options, pErrorCode);
    जइ(csm==nullptr) {
        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
        प्रत्यागम nullptr;
    } अण्णहा जइ (U_FAILURE(*pErrorCode)) {
        delete csm;
        प्रत्यागम nullptr;
    }
    प्रत्यागम reinterpret_cast<UCaseMap *>(csm);
}

U_CAPI शून्य U_EXPORT2
ucasemap_close(UCaseMap *csm) {
    delete reinterpret_cast<UCaseMapImpl *>(csm);
}

U_CAPI ध्रुव वर्ण* U_EXPORT2
ucasemap_getLocale(ध्रुव UCaseMap* csm U_LIFETIME_BOUND) {
    प्रत्यागम reinterpret_cast<ध्रुव UCaseMapImpl *>(csm)->locale;
}

U_CAPI uint32_t U_EXPORT2
ucasemap_getOptions(ध्रुव UCaseMap *csm) {
    प्रत्यागम reinterpret_cast<ध्रुव UCaseMapImpl *>(csm)->options;
}

U_CAPI शून्य U_EXPORT2
ucasemap_setLocale(UCaseMap *csm, ध्रुव वर्ण *locale, UErrorCode *pErrorCode) {
    जइ(U_FAILURE(*pErrorCode)) {
        प्रत्यागम;
    }
    auto *impl = reinterpret_cast<UCaseMapImpl *>(csm);
    जइ (locale != nullptr && *locale == 0) {
        impl->locale[0] = 0;
        impl->caseLocale = UCASE_LOC_मूलम्;
        प्रत्यागम;
    }

    UErrorCode bufferStatus = U_ZERO_ERROR;
    int32_t length=uloc_getName(locale, impl->locale, (int32_t)परिमाण(impl->locale), &bufferStatus);
    जइ(bufferStatus==U_BUFFER_OVERFLOW_ERROR || (U_SUCCESS(bufferStatus) && length==परिमाण(impl->locale))) {
        bufferStatus = U_ZERO_ERROR;
        /* we only really need the language code for case mappings */
        length=uloc_getLanguage(locale, impl->locale, (int32_t)परिमाण(impl->locale), &bufferStatus);
    }
    जइ(U_FAILURE(bufferStatus)) {
        *pErrorCode=bufferStatus;
    } अण्णहा जइ(length==परिमाण(impl->locale)) {
        *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
    }
    जइ(U_SUCCESS(*pErrorCode)) {     
        impl->caseLocale = ucase_getCaseLocale(impl->locale);
    } अण्णहा {
        impl->locale[0]=0;
        impl->caseLocale = UCASE_LOC_मूलम्;
    }
}

U_CAPI शून्य U_EXPORT2
ucasemap_setOptions(UCaseMap *csm, uint32_t options, UErrorCode *pErrorCode) {
    जइ(U_FAILURE(*pErrorCode)) {
        प्रत्यागम;
    }
    reinterpret_cast<UCaseMapImpl *>(csm)->options=options;
}

/* UTF-8 string case mappings ----------------------------------------------- */

/* TODO(markus): Move to a new, separate utf8case.cpp file. */

नामाकाश {

/* append a full case mapping result, see UCASE_MAX_STRING_LENGTH */
अन्तःस्थ UBool
appendResult(int32_t cpLength, int32_t result, ध्रुव char16_t *s,
             ByteSink &sink, uint32_t options, icu::Edits *edits, UErrorCode &errorCode) {
    U_ASSERT(U_SUCCESS(errorCode));

    /* decode the result */
    जइ(result<0) {
        /* (not) original code point */
        जइ(edits!=nullptr) {
            edits->addUnchanged(cpLength);
        }
        जइ((options & U_OMIT_UNCHANGED_TEXT) == 0) {
            ByteSinkUtil::appendCodePoint(cpLength, ~result, sink);
        }
    } अण्णहा {
        जइ(result<=UCASE_MAX_STRING_LENGTH) {
            // string: "result" is the UTF-16 length
            प्रत्यागम ByteSinkUtil::appendChange(cpLength, s, result, sink, edits, errorCode);
        } अण्णहा {
            ByteSinkUtil::appendCodePoint(cpLength, result, sink, edits);
        }
    }
    प्रत्यागम सच्च;
}

// See unicode/utf8.h U8_APPEND_UNSAFE().
अन्तःस्थ uint8_t getTwoByteLead(UChar32 c) { प्रत्यागम static_cast<uint8_t>((c >> 6) | 0xc0); }
अन्तःस्थ uint8_t getTwoByteTrail(UChar32 c) { प्रत्यागम static_cast<uint8_t>((c & 0x3f) | 0x80); }

UChar32 U_CALLCONV
utf8_caseContextIterator(शून्य *context, int8_t dir) {
    UCaseContext* csc = static_cast<UCaseContext*>(context);
    UChar32 c;

    जइ(dir<0) {
        /* reset for backward iteration */
        csc->index=csc->cpStart;
        csc->dir=dir;
    } अण्णहा जइ(dir>0) {
        /* reset for forward iteration */
        csc->index=csc->cpLimit;
        csc->dir=dir;
    } अण्णहा {
        /* continue current iteration direction */
        dir=csc->dir;
    }

    जइ(dir<0) {
        जइ(csc->start<csc->index) {
            U8_PREV((ध्रुव uint8_t *)csc->p, csc->start, csc->index, c);
            प्रत्यागम c;
        }
    } अण्णहा {
        जइ(csc->index<csc->limit) {
            U8_NEXT((ध्रुव uint8_t *)csc->p, csc->index, csc->limit, c);
            प्रत्यागम c;
        }
    }
    प्रत्यागम U_SENTINEL;
}

/**
 * caseLocale >= 0: Lowercases [srcStart..srcLimit[ but takes context [0..srcLength[ into account.
 * caseLocale < 0: Case-folds [srcStart..srcLimit[.
 */
शून्य toLower(int32_t caseLocale, uint32_t options,
             ध्रुव uint8_t *src, UCaseContext *csc, int32_t srcStart, int32_t srcLimit,
             icu::ByteSink &sink, icu::Edits *edits, UErrorCode &errorCode) {
    ध्रुव int8_t *latinToLower;
    जइ (caseLocale == UCASE_LOC_मूलम् ||
            (caseLocale >= 0 ?
                !(caseLocale == UCASE_LOC_TURKISH || caseLocale == UCASE_LOC_LITHUANIAN) :
                (options & _FOLD_CASE_OPTIONS_MASK) == U_FOLD_CASE_DEFAULT)) {
        latinToLower = LatinCase::TO_LOWER_NORMAL;
    } अण्णहा {
        latinToLower = LatinCase::TO_LOWER_TR_LT;
    }
    ध्रुव UTrie2 *trie = ucase_getTrie();
    int32_t prev = srcStart;
    int32_t srcIndex = srcStart;
    प्रति (;;) {
        // fast path for simple cases
        int32_t cpStart;
        UChar32 c;
        प्रति (;;) {
            जइ (U_FAILURE(errorCode) || srcIndex >= srcLimit) {
                c = U_SENTINEL;
                भङ्ग;
            }
            uint8_t lead = src[srcIndex++];
            जइ (lead <= 0x7f) {
                int8_t d = latinToLower[lead];
                जइ (d == LatinCase::EXC) {
                    cpStart = srcIndex - 1;
                    c = lead;
                    भङ्ग;
                }
                जइ (d == 0) { अनुवर्त; }
                ByteSinkUtil::appendUnchanged(src + prev, srcIndex - 1 - prev,
                                              sink, options, edits, errorCode);
                वर्ण ascii = static_cast<वर्ण>(lead + d);
                sink.Append(&ascii, 1);
                जइ (edits != nullptr) {
                    edits->addReplace(1, 1);
                }
                prev = srcIndex;
                अनुवर्त;
            } अण्णहा जइ (lead < 0xe3) {
                uint8_t t;
                जइ (0xc2 <= lead && lead <= 0xc5 && srcIndex < srcLimit &&
                        (t = src[srcIndex] - 0x80) <= 0x3f) {
                    // U+0080..U+017F
                    ++srcIndex;
                    c = ((lead - 0xc0) << 6) | t;
                    int8_t d = latinToLower[c];
                    जइ (d == LatinCase::EXC) {
                        cpStart = srcIndex - 2;
                        भङ्ग;
                    }
                    जइ (d == 0) { अनुवर्त; }
                    ByteSinkUtil::appendUnchanged(src + prev, srcIndex - 2 - prev,
                                                  sink, options, edits, errorCode);
                    ByteSinkUtil::appendTwoBytes(c + d, sink);
                    जइ (edits != nullptr) {
                        edits->addReplace(2, 2);
                    }
                    prev = srcIndex;
                    अनुवर्त;
                }
            } अण्णहा जइ ((lead <= 0xe9 || lead == 0xeb || lead == 0xec) &&
                    (srcIndex + 2) <= srcLimit &&
                    U8_IS_TRAIL(src[srcIndex]) && U8_IS_TRAIL(src[srcIndex + 1])) {
                // most of CJK: no case mappings
                srcIndex += 2;
                अनुवर्त;
            }
            cpStart = --srcIndex;
            U8_NEXT(src, srcIndex, srcLimit, c);
            जइ (c < 0) {
                // ill-formed UTF-8
                अनुवर्त;
            }
            uint16_t props = UTRIE2_GET16(trie, c);
            जइ (UCASE_HAS_EXCEPTION(props)) { भङ्ग; }
            int32_t delta;
            जइ (!UCASE_IS_UPPER_OR_TITLE(props) || (delta = UCASE_GET_DELTA(props)) == 0) {
                अनुवर्त;
            }
            ByteSinkUtil::appendUnchanged(src + prev, cpStart - prev,
                                          sink, options, edits, errorCode);
            ByteSinkUtil::appendCodePoint(srcIndex - cpStart, c + delta, sink, edits);
            prev = srcIndex;
        }
        जइ (c < 0) {
            भङ्ग;
        }
        // slow path
        ध्रुव char16_t *s;
        जइ (caseLocale >= 0) {
            csc->cpStart = cpStart;
            csc->cpLimit = srcIndex;
            c = ucase_toFullLower(c, utf8_caseContextIterator, csc, &s, caseLocale);
        } अण्णहा {
            c = ucase_toFullFolding(c, &s, options);
        }
        जइ (c >= 0) {
            ByteSinkUtil::appendUnchanged(src + prev, cpStart - prev,
                                          sink, options, edits, errorCode);
            appendResult(srcIndex - cpStart, c, s, sink, options, edits, errorCode);
            prev = srcIndex;
        }
    }
    ByteSinkUtil::appendUnchanged(src + prev, srcIndex - prev,
                                  sink, options, edits, errorCode);
}

शून्य toUpper(int32_t caseLocale, uint32_t options,
             ध्रुव uint8_t *src, UCaseContext *csc, int32_t srcLength,
             icu::ByteSink &sink, icu::Edits *edits, UErrorCode &errorCode) {
    ध्रुव int8_t *latinToUpper;
    जइ (caseLocale == UCASE_LOC_TURKISH) {
        latinToUpper = LatinCase::TO_UPPER_TR;
    } अण्णहा {
        latinToUpper = LatinCase::TO_UPPER_NORMAL;
    }
    ध्रुव UTrie2 *trie = ucase_getTrie();
    int32_t prev = 0;
    int32_t srcIndex = 0;
    प्रति (;;) {
        // fast path for simple cases
        int32_t cpStart;
        UChar32 c;
        प्रति (;;) {
            जइ (U_FAILURE(errorCode) || srcIndex >= srcLength) {
                c = U_SENTINEL;
                भङ्ग;
            }
            uint8_t lead = src[srcIndex++];
            जइ (lead <= 0x7f) {
                int8_t d = latinToUpper[lead];
                जइ (d == LatinCase::EXC) {
                    cpStart = srcIndex - 1;
                    c = lead;
                    भङ्ग;
                }
                जइ (d == 0) { अनुवर्त; }
                ByteSinkUtil::appendUnchanged(src + prev, srcIndex - 1 - prev,
                                              sink, options, edits, errorCode);
                वर्ण ascii = static_cast<वर्ण>(lead + d);
                sink.Append(&ascii, 1);
                जइ (edits != nullptr) {
                    edits->addReplace(1, 1);
                }
                prev = srcIndex;
                अनुवर्त;
            } अण्णहा जइ (lead < 0xe3) {
                uint8_t t;
                जइ (0xc2 <= lead && lead <= 0xc5 && srcIndex < srcLength &&
                        (t = src[srcIndex] - 0x80) <= 0x3f) {
                    // U+0080..U+017F
                    ++srcIndex;
                    c = ((lead - 0xc0) << 6) | t;
                    int8_t d = latinToUpper[c];
                    जइ (d == LatinCase::EXC) {
                        cpStart = srcIndex - 2;
                        भङ्ग;
                    }
                    जइ (d == 0) { अनुवर्त; }
                    ByteSinkUtil::appendUnchanged(src + prev, srcIndex - 2 - prev,
                                                  sink, options, edits, errorCode);
                    ByteSinkUtil::appendTwoBytes(c + d, sink);
                    जइ (edits != nullptr) {
                        edits->addReplace(2, 2);
                    }
                    prev = srcIndex;
                    अनुवर्त;
                }
            } अण्णहा जइ ((lead <= 0xe9 || lead == 0xeb || lead == 0xec) &&
                    (srcIndex + 2) <= srcLength &&
                    U8_IS_TRAIL(src[srcIndex]) && U8_IS_TRAIL(src[srcIndex + 1])) {
                // most of CJK: no case mappings
                srcIndex += 2;
                अनुवर्त;
            }
            cpStart = --srcIndex;
            U8_NEXT(src, srcIndex, srcLength, c);
            जइ (c < 0) {
                // ill-formed UTF-8
                अनुवर्त;
            }
            uint16_t props = UTRIE2_GET16(trie, c);
            जइ (UCASE_HAS_EXCEPTION(props)) { भङ्ग; }
            int32_t delta;
            जइ (UCASE_GET_TYPE(props) != UCASE_LOWER || (delta = UCASE_GET_DELTA(props)) == 0) {
                अनुवर्त;
            }
            ByteSinkUtil::appendUnchanged(src + prev, cpStart - prev,
                                          sink, options, edits, errorCode);
            ByteSinkUtil::appendCodePoint(srcIndex - cpStart, c + delta, sink, edits);
            prev = srcIndex;
        }
        जइ (c < 0) {
            भङ्ग;
        }
        // slow path
        csc->cpStart = cpStart;
        csc->cpLimit = srcIndex;
        ध्रुव char16_t *s;
        c = ucase_toFullUpper(c, utf8_caseContextIterator, csc, &s, caseLocale);
        जइ (c >= 0) {
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

#जइ !UCONFIG_NO_BREAK_ITERATION

नामाकाश {

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
int32_t maybeTitleDutchIJ(ध्रुव uint8_t *src, UChar32 c, int32_t start, int32_t segmentLimit,
                          ByteSink &sink, uint32_t options, icu::Edits *edits, UErrorCode &errorCode) {
    U_ASSERT(start < segmentLimit);

    int32_t index = start;
    तार्किक withAcute = मिच्छ;

    // If the conditions are met, then the following variables tell us what to output.
    int32_t unchanged1 = 0;  // code units before the j, or the whole sequence (0..3)
    तार्किक doTitleJ = मिच्छ;  // true if the j needs to be titlecased
    int32_t unchanged2 = 0;  // after the j (0 or 1)

    // next character after the first letter
    UChar32 c2;
    c2 = src[index++];

    // Is the first letter an i/I with accent?
    जइ (c == u'I') {
        जइ (c2 == ACUTE_BYTE0 && index < segmentLimit && src[index++] == ACUTE_BYTE1) {
            withAcute = सच्च;
            unchanged1 = 2;  // ACUTE is 2 code units in UTF-8
            जइ (index == segmentLimit) { प्रत्यागम start; }
            c2 = src[index++];
        }
    } अण्णहा {  // Í
        withAcute = सच्च;
    }

    // Is the next character a j/J?
    जइ (c2 == u'j') {
        doTitleJ = सच्च;
    } अण्णहा जइ (c2 == u'J') {
        ++unchanged1;
    } अण्णहा {
        प्रत्यागम start;
    }

    // A plain i/I must be followed by a plain j/J.
    // An i/I with acute must be followed by a j/J with acute.
    जइ (withAcute) {
        जइ ((index + 1) >= segmentLimit || src[index++] != ACUTE_BYTE0 || src[index++] != ACUTE_BYTE1) {
            प्रत्यागम start;
        }
        जइ (doTitleJ) {
            unchanged2 = 2;  // ACUTE is 2 code units in UTF-8
        } अण्णहा {
            unchanged1 = unchanged1 + 2;    // ACUTE is 2 code units in UTF-8
        }
    }

    // There must not be another combining mark.
    जइ (index < segmentLimit) {
        int32_t cp;
        int32_t i = index;
        U8_NEXT(src, i, segmentLimit, cp);
        uint32_t typeMask = U_GET_GC_MASK(cp);
        जइ ((typeMask & U_GC_M_MASK) != 0) {
            प्रत्यागम start;
        }
    }

    // Output the rest of the Dutch IJ.
    ByteSinkUtil::appendUnchanged(src + start, unchanged1, sink, options, edits, errorCode);
    start += unchanged1;
    जइ (doTitleJ) {
        ByteSinkUtil::appendCodePoint(1, u'J', sink, edits);
        ++start;
    }
    ByteSinkUtil::appendUnchanged(src + start, unchanged2, sink, options, edits, errorCode);

    U_ASSERT(start + unchanged2 == index);
    प्रत्यागम index;
}

}  // namespace

U_CFUNC शून्य U_CALLCONV
ucasemap_internalUTF8ToTitle(
        int32_t caseLocale, uint32_t options, BreakIterator *iter,
        ध्रुव uint8_t *src, int32_t srcLength,
        ByteSink &sink, icu::Edits *edits,
        UErrorCode &errorCode) {
    जइ (!ustrcase_checkTitleAdjustmentOptions(options, errorCode)) {
        प्रत्यागम;
    }

    /* set up local variables */
    UCaseContext csc=UCASECONTEXT_INITIALIZER;
    csc.p=(शून्य *)src;
    csc.limit=srcLength;
    int32_t prev=0;
    UBool isFirstIndex=सच्च;

    /* titlecasing loop */
    जाव(prev<srcLength) {
        /* find next index where to titlecase */
        int32_t index;
        जइ(isFirstIndex) {
            isFirstIndex=मिच्छ;
            index=iter->first();
        } अण्णहा {
            index=iter->next();
        }
        जइ(index==UBRK_DONE || index>srcLength) {
            index=srcLength;
        }

        /*
         * Segment [prev..index[ into 3 parts:
         * a) skipped characters (copy as-is) [prev..titleStart[
         * b) first letter (titlecase)              [titleStart..titleLimit[
         * c) subsequent characters (lowercase)                 [titleLimit..index[
         */
        जइ(prev<index) {
            /* find and copy skipped characters [prev..titleStart[ */
            int32_t titleStart=prev;
            int32_t titleLimit=prev;
            UChar32 c;
            U8_NEXT(src, titleLimit, index, c);
            जइ ((options&U_TITLECASE_NO_BREAK_ADJUSTMENT)==0) {
                // Adjust the titlecasing index to the next cased character,
                // or to the next letter/number/symbol/private use.
                // Stop with titleStart<titleLimit<=index
                // if there is a character to be titlecased,
                // or else stop with titleStart==titleLimit==index.
                UBool toCased = (options&U_TITLECASE_ADJUST_TO_CASED) != 0;
                जाव (toCased ? UCASE_NONE==ucase_getType(c) : !ustrcase_isLNS(c)) {
                    titleStart=titleLimit;
                    जइ(titleLimit==index) {
                        भङ्ग;
                    }
                    U8_NEXT(src, titleLimit, index, c);
                }
                जइ (prev < titleStart) {
                    जइ (!ByteSinkUtil::appendUnchanged(src+prev, titleStart-prev,
                                                       sink, options, edits, errorCode)) {
                        प्रत्यागम;
                    }
                }
            }

            जइ(titleStart<titleLimit) {
                /* titlecase c which is from [titleStart..titleLimit[ */
                जइ(c>=0) {
                    csc.cpStart=titleStart;
                    csc.cpLimit=titleLimit;
                    ध्रुव char16_t *s;
                    c=ucase_toFullTitle(c, utf8_caseContextIterator, &csc, &s, caseLocale);
                    जइ (!appendResult(titleLimit-titleStart, c, s, sink, options, edits, errorCode)) {
                        प्रत्यागम;
                    }
                } अण्णहा {
                    // Malformed UTF-8.
                    जइ (!ByteSinkUtil::appendUnchanged(src+titleStart, titleLimit-titleStart,
                                                       sink, options, edits, errorCode)) {
                        प्रत्यागम;
                    }
                }

                /* Special case Dutch IJ titlecasing */
                जइ (titleLimit < index &&
                    caseLocale == UCASE_LOC_DUTCH) {
                    जइ (c < 0) {
                        c = ~c;
                    }

                    जइ (c == u'I' || c == u'Í') {
                        titleLimit = maybeTitleDutchIJ(src, c, titleLimit, index, sink, options, edits, errorCode);
                    }
                }

                /* lowercase [titleLimit..index[ */
                जइ(titleLimit<index) {
                    जइ((options&U_TITLECASE_NO_LOWERCASE)==0) {
                        /* Normal operation: Lowercase the rest of the word. */
                        toLower(caseLocale, options,
                                src, &csc, titleLimit, index,
                                sink, edits, errorCode);
                        जइ(U_FAILURE(errorCode)) {
                            प्रत्यागम;
                        }
                    } अण्णहा {
                        /* Optionally just copy the rest of the word unchanged. */
                        जइ (!ByteSinkUtil::appendUnchanged(src+titleLimit, index-titleLimit,
                                                           sink, options, edits, errorCode)) {
                            प्रत्यागम;
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
नामाकाश GreekUpper {

UBool isFollowedByCasedLetter(ध्रुव uint8_t *s, int32_t i, int32_t length) {
    जाव (i < length) {
        UChar32 c;
        U8_NEXT(s, i, length, c);
        int32_t type = ucase_getTypeOrIgnorable(c);
        जइ ((type & UCASE_IGNORABLE) != 0) {
            // Case-ignorable, continue with the loop.
        } अण्णहा जइ (type != UCASE_NONE) {
            प्रत्यागम सच्च;  // Followed by cased letter.
        } अण्णहा {
            प्रत्यागम मिच्छ;  // Uncased and not case-ignorable.
        }
    }
    प्रत्यागम मिच्छ;  // Not followed by cased letter.
}

// Keep this consistent with the UTF-16 version in ustrcase.cpp and the Java version in CaseMap.java.
शून्य toUpper(uint32_t options,
             ध्रुव uint8_t *src, int32_t srcLength,
             ByteSink &sink, Edits *edits,
             UErrorCode &errorCode) {
    uint32_t state = 0;
    प्रति (int32_t i = 0; i < srcLength;) {
        int32_t nextIndex = i;
        UChar32 c;
        U8_NEXT(src, nextIndex, srcLength, c);
        uint32_t nextState = 0;
        int32_t type = ucase_getTypeOrIgnorable(c);
        जइ ((type & UCASE_IGNORABLE) != 0) {
            // c is case-ignorable
            nextState |= (state & AFTER_CASED);
        } अण्णहा जइ (type != UCASE_NONE) {
            // c is cased
            nextState |= AFTER_CASED;
        }
        uint32_t data = getLetterData(c);
        जइ (data > 0) {
            uint32_t upper = data & UPPER_MASK;
            // Add a dialytika to this iota or ypsilon vowel
            // if we removed a tonos from the previous vowel,
            // and that previous vowel did not also have (or gain) a dialytika.
            // Adding one only to the final vowel in a longer sequence
            // (which does not occur in normal writing) would require lookahead.
            // Set the same flag as for preserving an existing dialytika.
            जइ ((data & HAS_VOWEL) != 0 &&
                (state & (AFTER_VOWEL_WITH_PRECOMPOSED_ACCENT | AFTER_VOWEL_WITH_COMBINING_ACCENT)) !=
                    0 &&
                (upper == 0x399 || upper == 0x3A5)) {
                data |= (state & AFTER_VOWEL_WITH_PRECOMPOSED_ACCENT) != 0 ? HAS_DIALYTIKA
                                                                           : HAS_COMBINING_DIALYTIKA;
            }
            int32_t अङ्कः_Ypogegrammeni = 0;  // Map each one to a trailing, spacing, capital iota.
            जइ ((data & HAS_YPOGEGRAMMENI) != 0) {
                अङ्कः_Ypogegrammeni = 1;
            }
            ध्रुव UBool hasPrecomposedAccent = (data & HAS_ACCENT) != 0;
            // Skip combining diacritics after this Greek letter.
            int32_t nextNextIndex = nextIndex;
            जाव (nextIndex < srcLength) {
                UChar32 c2;
                U8_NEXT(src, nextNextIndex, srcLength, c2);
                uint32_t diacriticData = getDiacriticData(c2);
                जइ (diacriticData != 0) {
                    data |= diacriticData;
                    जइ ((diacriticData & HAS_YPOGEGRAMMENI) != 0) {
                        ++अङ्कः_Ypogegrammeni;
                    }
                    nextIndex = nextNextIndex;
                } अण्णहा {
                    भङ्ग;  // not a Greek diacritic
                }
            }
            जइ ((data & HAS_VOWEL_AND_ACCENT_AND_DIALYTIKA) == HAS_VOWEL_AND_ACCENT) {
                nextState |= hasPrecomposedAccent ? AFTER_VOWEL_WITH_PRECOMPOSED_ACCENT
                                                  : AFTER_VOWEL_WITH_COMBINING_ACCENT;
            }
            // Map according to Greek rules.
            UBool addTonos = मिच्छ;
            जइ (upper == 0x397 &&
                    (data & HAS_ACCENT) != 0 &&
                    अङ्कः_Ypogegrammeni == 0 &&
                    (state & AFTER_CASED) == 0 &&
                    !isFollowedByCasedLetter(src, nextIndex, srcLength)) {
                // Keep disjunctive "or" with (only) a tonos.
                // We use the same "word boundary" conditions as for the Final_Sigma test.
                जइ (hasPrecomposedAccent) {
                    upper = 0x389;  // Preserve the precomposed form.
                } अण्णहा {
                    addTonos = सच्च;
                }
            } अण्णहा जइ ((data & HAS_DIALYTIKA) != 0) {
                // Preserve a vowel with dialytika in precomposed form if it exists.
                जइ (upper == 0x399) {
                    upper = 0x3AA;
                    data &= ~HAS_EITHER_DIALYTIKA;
                } अण्णहा जइ (upper == 0x3A5) {
                    upper = 0x3AB;
                    data &= ~HAS_EITHER_DIALYTIKA;
                }
            }

            UBool change;
            जइ (edits == nullptr && (options & U_OMIT_UNCHANGED_TEXT) == 0) {
                change = सच्च;  // common, simple usage
            } अण्णहा {
                // Find out first whether we are changing the text.
                U_ASSERT(0x370 <= upper && upper <= 0x3ff);  // 2-byte UTF-8, main Greek block
                change = (i + 2) > nextIndex ||
                        src[i] != getTwoByteLead(upper) || src[i + 1] != getTwoByteTrail(upper) ||
                        अङ्कः_Ypogegrammeni > 0;
                int32_t i2 = i + 2;
                जइ ((data & HAS_EITHER_DIALYTIKA) != 0) {
                    change |= (i2 + 2) > nextIndex ||
                            src[i2] != static_cast<uint8_t>(u8"\u0308"[0]) ||
                            src[i2 + 1] != static_cast<uint8_t>(u8"\u0308"[1]);
                    i2 += 2;
                }
                जइ (addTonos) {
                    change |= (i2 + 2) > nextIndex ||
                            src[i2] != static_cast<uint8_t>(u8"\u0301"[0]) ||
                            src[i2 + 1] != static_cast<uint8_t>(u8"\u0301"[1]);
                    i2 += 2;
                }
                int32_t oldLength = nextIndex - i;
                int32_t newLength = (i2 - i) + अङ्कः_Ypogegrammeni * 2;  // 2 bytes per U+0399
                change |= oldLength != newLength;
                जइ (change) {
                    जइ (edits != nullptr) {
                        edits->addReplace(oldLength, newLength);
                    }
                } अण्णहा {
                    जइ (edits != nullptr) {
                        edits->addUnchanged(oldLength);
                    }
                    // Write unchanged text?
                    change = (options & U_OMIT_UNCHANGED_TEXT) == 0;
                }
            }

            जइ (change) {
                ByteSinkUtil::appendTwoBytes(upper, sink);
                जइ ((data & HAS_EITHER_DIALYTIKA) != 0) {
                    sink.AppendU8(u8"\u0308", 2);  // restore or add a dialytika
                }
                जइ (addTonos) {
                    sink.AppendU8(u8"\u0301", 2);
                }
                जाव (अङ्कः_Ypogegrammeni > 0) {
                    sink.AppendU8(u8"\u0399", 2);
                    --अङ्कः_Ypogegrammeni;
                }
            }
        } अण्णहा जइ(c>=0) {
            ध्रुव char16_t *s;
            c=ucase_toFullUpper(c, nullptr, nullptr, &s, UCASE_LOC_GREEK);
            जइ (!appendResult(nextIndex - i, c, s, sink, options, edits, errorCode)) {
                प्रत्यागम;
            }
        } अण्णहा {
            // Malformed UTF-8.
            जइ (!ByteSinkUtil::appendUnchanged(src+i, nextIndex-i,
                                               sink, options, edits, errorCode)) {
                प्रत्यागम;
            }
        }
        i = nextIndex;
        state = nextState;
    }
}

}  // namespace GreekUpper
U_NAMESPACE_END

स्थिर शून्य U_CALLCONV
ucasemap_internalUTF8ToLower(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_UNUSED
                             ध्रुव uint8_t *src, int32_t srcLength,
                             icu::ByteSink &sink, icu::Edits *edits,
                             UErrorCode &errorCode) {
    UCaseContext csc=UCASECONTEXT_INITIALIZER;
    csc.p=(शून्य *)src;
    csc.limit=srcLength;
    toLower(
        caseLocale, options,
        src, &csc, 0, srcLength,
        sink, edits, errorCode);
}

स्थिर शून्य U_CALLCONV
ucasemap_internalUTF8ToUpper(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_UNUSED
                             ध्रुव uint8_t *src, int32_t srcLength,
                             icu::ByteSink &sink, icu::Edits *edits,
                             UErrorCode &errorCode) {
    जइ (caseLocale == UCASE_LOC_GREEK) {
        GreekUpper::toUpper(options, src, srcLength, sink, edits, errorCode);
    } अण्णहा {
        UCaseContext csc=UCASECONTEXT_INITIALIZER;
        csc.p=(शून्य *)src;
        csc.limit=srcLength;
        toUpper(
            caseLocale, options,
            src, &csc, srcLength,
            sink, edits, errorCode);
    }
}

स्थिर शून्य U_CALLCONV
ucasemap_internalUTF8Fold(int32_t /* caseLocale */, uint32_t options, UCASEMAP_BREAK_ITERATOR_UNUSED
                          ध्रुव uint8_t *src, int32_t srcLength,
                          icu::ByteSink &sink, icu::Edits *edits,
                          UErrorCode &errorCode) {
    toLower(
        -1, options,
        src, nullptr, 0, srcLength,
        sink, edits, errorCode);
}

शून्य
ucasemap_mapUTF8(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_PARAM
                 ध्रुव वर्ण *src, int32_t srcLength,
                 UTF8CaseMapper *stringCaseMapper,
                 icu::ByteSink &sink, icu::Edits *edits,
                 UErrorCode &errorCode) {
    /* check argument values */
    जइ (U_FAILURE(errorCode)) {
        प्रत्यागम;
    }
    जइ ((src == nullptr && srcLength != 0) || srcLength < -1) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम;
    }

    // Get the string length.
    जइ (srcLength == -1) {
        srcLength = static_cast<int32_t>(uprv_strlen(src));
    }

    जइ (edits != nullptr && (options & U_EDITS_NO_RESET) == 0) {
        edits->reset();
    }
    stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                     reinterpret_cast<ध्रुव uint8_t*>(src), srcLength, sink, edits, errorCode);
    sink.Flush();
    जइ (U_SUCCESS(errorCode)) {
        जइ (edits != nullptr) {
            edits->copyErrorTo(errorCode);
        }
    }
}

int32_t
ucasemap_mapUTF8(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_PARAM
                 वर्ण *dest, int32_t destCapacity,
                 ध्रुव वर्ण *src, int32_t srcLength,
                 UTF8CaseMapper *stringCaseMapper,
                 icu::Edits *edits,
                 UErrorCode &errorCode) {
    /* check argument values */
    जइ(U_FAILURE(errorCode)) {
        प्रत्यागम 0;
    }
    जइ( destCapacity<0 ||
        (dest==nullptr && destCapacity>0) ||
        (src==nullptr && srcLength!=0) || srcLength<-1
    ) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम 0;
    }

    /* get the string length */
    जइ(srcLength==-1) {
        srcLength = static_cast<int32_t>(uprv_strlen(src));
    }

    /* check for overlapping source and destination */
    जइ( dest!=nullptr &&
        ((src>=dest && src<(dest+destCapacity)) ||
         (dest>=src && dest<(src+srcLength)))
    ) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम 0;
    }

    जइ (edits != nullptr && (options & U_EDITS_NO_RESET) == 0) {
        edits->reset();
    }
    int32_t reslen = ByteSinkUtil::viaByteSinkToTerminatedChars(
        dest, destCapacity,
        [&](ByteSink& sink, UErrorCode& status) {
            stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                             reinterpret_cast<ध्रुव uint8_t*>(src), srcLength, sink, edits, status);
        },
        errorCode);
    जइ (U_SUCCESS(errorCode) && edits != nullptr) {
        edits->copyErrorTo(errorCode);
    }
    प्रत्यागम reslen;
}

/* public API functions */

U_CAPI int32_t U_EXPORT2
ucasemap_utf8ToLower(ध्रुव UCaseMap *csm,
                     वर्ण *dest, int32_t destCapacity,
                     ध्रुव वर्ण *src, int32_t srcLength,
                     UErrorCode *pErrorCode) {
    auto *impl = reinterpret_cast<ध्रुव UCaseMapImpl *>(csm);
    प्रत्यागम ucasemap_mapUTF8(
        impl->caseLocale, impl->options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, destCapacity,
        src, srcLength,
        ucasemap_internalUTF8ToLower, nullptr, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
ucasemap_utf8ToUpper(ध्रुव UCaseMap *csm,
                     वर्ण *dest, int32_t destCapacity,
                     ध्रुव वर्ण *src, int32_t srcLength,
                     UErrorCode *pErrorCode) {
    auto *impl = reinterpret_cast<ध्रुव UCaseMapImpl *>(csm);
    प्रत्यागम ucasemap_mapUTF8(
        impl->caseLocale, impl->options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, destCapacity,
        src, srcLength,
        ucasemap_internalUTF8ToUpper, nullptr, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
ucasemap_utf8FoldCase(ध्रुव UCaseMap *csm,
                      वर्ण *dest, int32_t destCapacity,
                      ध्रुव वर्ण *src, int32_t srcLength,
                      UErrorCode *pErrorCode) {
    auto *impl = reinterpret_cast<ध्रुव UCaseMapImpl *>(csm);
    प्रत्यागम ucasemap_mapUTF8(
        UCASE_LOC_मूलम्, impl->options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, destCapacity,
        src, srcLength,
        ucasemap_internalUTF8Fold, nullptr, *pErrorCode);
}

U_NAMESPACE_BEGIN

शून्य CaseMap::utf8ToLower(
        ध्रुव वर्ण *locale, uint32_t options,
        StringPiece src, ByteSink &sink, Edits *edits,
        UErrorCode &errorCode) {
    ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        src.data(), src.length(),
        ucasemap_internalUTF8ToLower, sink, edits, errorCode);
}

शून्य CaseMap::utf8ToUpper(
        ध्रुव वर्ण *locale, uint32_t options,
        StringPiece src, ByteSink &sink, Edits *edits,
        UErrorCode &errorCode) {
    ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        src.data(), src.length(),
        ucasemap_internalUTF8ToUpper, sink, edits, errorCode);
}

शून्य CaseMap::utf8Fold(
        uint32_t options,
        StringPiece src, ByteSink &sink, Edits *edits,
        UErrorCode &errorCode) {
    ucasemap_mapUTF8(
        UCASE_LOC_मूलम्, options, UCASEMAP_BREAK_ITERATOR_NULL
        src.data(), src.length(),
        ucasemap_internalUTF8Fold, sink, edits, errorCode);
}

int32_t CaseMap::utf8ToLower(
        ध्रुव वर्ण *locale, uint32_t options,
        ध्रुव वर्ण *src, int32_t srcLength,
        वर्ण *dest, int32_t destCapacity, Edits *edits,
        UErrorCode &errorCode) {
    प्रत्यागम ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, destCapacity,
        src, srcLength,
        ucasemap_internalUTF8ToLower, edits, errorCode);
}

int32_t CaseMap::utf8ToUpper(
        ध्रुव वर्ण *locale, uint32_t options,
        ध्रुव वर्ण *src, int32_t srcLength,
        वर्ण *dest, int32_t destCapacity, Edits *edits,
        UErrorCode &errorCode) {
    प्रत्यागम ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, destCapacity,
        src, srcLength,
        ucasemap_internalUTF8ToUpper, edits, errorCode);
}

int32_t CaseMap::utf8Fold(
        uint32_t options,
        ध्रुव वर्ण *src, int32_t srcLength,
        वर्ण *dest, int32_t destCapacity, Edits *edits,
        UErrorCode &errorCode) {
    प्रत्यागम ucasemap_mapUTF8(
        UCASE_LOC_मूलम्, options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, destCapacity,
        src, srcLength,
        ucasemap_internalUTF8Fold, edits, errorCode);
}

U_NAMESPACE_END
