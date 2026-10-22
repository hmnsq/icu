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

#অন্তর্ভাৱ "unicode/utypes.h"
#অন্তর্ভাৱ "unicode/brkiter.h"
#অন্তর্ভাৱ "unicode/bytestream.h"
#অন্তর্ভাৱ "unicode/casemap.h"
#অন্তর্ভাৱ "unicode/edits.h"
#অন্তর্ভাৱ "unicode/stringoptions.h"
#অন্তর্ভাৱ "unicode/stringpiece.h"
#অন্তর্ভাৱ "unicode/ubrk.h"
#অন্তর্ভাৱ "unicode/uloc.h"
#অন্তর্ভাৱ "unicode/ustring.h"
#অন্তর্ভাৱ "unicode/ucasemap.h"
#যদি !UCONFIG_NO_BREAK_ITERATION
#অন্তর্ভাৱ "unicode/utext.h"
#endif
#অন্তর্ভাৱ "unicode/utf.h"
#অন্তর্ভাৱ "unicode/utf8.h"
#অন্তর্ভাৱ "unicode/utf16.h"
#অন্তর্ভাৱ "bytesinkutil.h"
#অন্তর্ভাৱ "cmemory.h"
#অন্তর্ভাৱ "cstring.h"
#অন্তর্ভাৱ "uassert.h"
#অন্তর্ভাৱ "ucase.h"
#অন্তর্ভাৱ "ucasemap_imp.h"

U_NAMESPACE_USE

/* UCaseMap service object -------------------------------------------------- */

// The public type, unicode/ucasemap.h struct UCaseMap, is an opaque C type.
// We define a parallel C++ type in the ICU namespace for entry point renaming.

U_NAMESPACE_BEGIN

UCaseMapImpl::UCaseMapImpl(ধ্রুৱ ৱর্ণ *localeID, uint32_t opts, UErrorCode *pErrorCode) :
#যদি !UCONFIG_NO_BREAK_ITERATION
        iter(nullptr),
#endif
        caseLocale(UCASE_LOC_UNKNOWN), options(opts) {
    ucasemap_setLocale(reinterpret_cast<UCaseMap *>(এতত্), localeID, pErrorCode);
}

UCaseMapImpl::~UCaseMapImpl() {
#যদি !UCONFIG_NO_BREAK_ITERATION
    delete iter;
#endif
}

U_NAMESPACE_END

U_CAPI UCaseMap * U_EXPORT2
ucasemap_open(ধ্রুৱ ৱর্ণ *locale, uint32_t options, UErrorCode *pErrorCode) {
    যদি(U_FAILURE(*pErrorCode)) {
        ঘূৰাই nullptr;
    }
    UCaseMapImpl *csm = নূতন UCaseMapImpl(locale, options, pErrorCode);
    যদি(csm==nullptr) {
        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
        ঘূৰাই nullptr;
    } অন্যথা যদি (U_FAILURE(*pErrorCode)) {
        delete csm;
        ঘূৰাই nullptr;
    }
    ঘূৰাই reinterpret_cast<UCaseMap *>(csm);
}

U_CAPI শূন্য U_EXPORT2
ucasemap_close(UCaseMap *csm) {
    delete reinterpret_cast<UCaseMapImpl *>(csm);
}

U_CAPI ধ্রুৱ ৱর্ণ* U_EXPORT2
ucasemap_getLocale(ধ্রুৱ UCaseMap* csm U_LIFETIME_BOUND) {
    ঘূৰাই reinterpret_cast<ধ্রুৱ UCaseMapImpl *>(csm)->locale;
}

U_CAPI uint32_t U_EXPORT2
ucasemap_getOptions(ধ্রুৱ UCaseMap *csm) {
    ঘূৰাই reinterpret_cast<ধ্রুৱ UCaseMapImpl *>(csm)->options;
}

U_CAPI শূন্য U_EXPORT2
ucasemap_setLocale(UCaseMap *csm, ধ্রুৱ ৱর্ণ *locale, UErrorCode *pErrorCode) {
    যদি(U_FAILURE(*pErrorCode)) {
        ঘূৰাই;
    }
    auto *impl = reinterpret_cast<UCaseMapImpl *>(csm);
    যদি (locale != nullptr && *locale == 0) {
        impl->locale[0] = 0;
        impl->caseLocale = UCASE_LOC_মূল;
        ঘূৰাই;
    }

    UErrorCode bufferStatus = U_ZERO_ERROR;
    int32_t length=uloc_getName(locale, impl->locale, (int32_t)পরিমাণ(impl->locale), &bufferStatus);
    যদি(bufferStatus==U_BUFFER_OVERFLOW_ERROR || (U_SUCCESS(bufferStatus) && length==পরিমাণ(impl->locale))) {
        bufferStatus = U_ZERO_ERROR;
        /* we only really need the language code for case mappings */
        length=uloc_getLanguage(locale, impl->locale, (int32_t)পরিমাণ(impl->locale), &bufferStatus);
    }
    যদি(U_FAILURE(bufferStatus)) {
        *pErrorCode=bufferStatus;
    } অন্যথা যদি(length==পরিমাণ(impl->locale)) {
        *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
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
        ঘূৰাই;
    }
    reinterpret_cast<UCaseMapImpl *>(csm)->options=options;
}

/* UTF-8 string case mappings ----------------------------------------------- */

/* TODO(markus): Move to a new, separate utf8case.cpp file. */

নামাকাশ {

/* append a full case mapping result, see UCASE_MAX_STRING_LENGTH */
অন্তঃস্থ UBool
appendResult(int32_t cpLength, int32_t result, ধ্রুৱ char16_t *s,
             ByteSink &sink, uint32_t options, icu::Edits *edits, UErrorCode &errorCode) {
    U_ASSERT(U_SUCCESS(errorCode));

    /* decode the result */
    যদি(result<0) {
        /* (not) original code point */
        যদি(edits!=nullptr) {
            edits->addUnchanged(cpLength);
        }
        যদি((options & U_OMIT_UNCHANGED_TEXT) == 0) {
            ByteSinkUtil::appendCodePoint(cpLength, ~result, sink);
        }
    } অন্যথা {
        যদি(result<=UCASE_MAX_STRING_LENGTH) {
            // string: "result" is the UTF-16 length
            ঘূৰাই ByteSinkUtil::appendChange(cpLength, s, result, sink, edits, errorCode);
        } অন্যথা {
            ByteSinkUtil::appendCodePoint(cpLength, result, sink, edits);
        }
    }
    ঘূৰাই সঁচা;
}

// See unicode/utf8.h U8_APPEND_UNSAFE().
অন্তঃস্থ uint8_t getTwoByteLead(UChar32 c) { ঘূৰাই static_cast<uint8_t>((c >> 6) | 0xc0); }
অন্তঃস্থ uint8_t getTwoByteTrail(UChar32 c) { ঘূৰাই static_cast<uint8_t>((c & 0x3f) | 0x80); }

UChar32 U_CALLCONV
utf8_caseContextIterator(শূন্য *context, int8_t dir) {
    UCaseContext* csc = static_cast<UCaseContext*>(context);
    UChar32 c;

    যদি(dir<0) {
        /* reset for backward iteration */
        csc->index=csc->cpStart;
        csc->dir=dir;
    } অন্যথা যদি(dir>0) {
        /* reset for forward iteration */
        csc->index=csc->cpLimit;
        csc->dir=dir;
    } অন্যথা {
        /* continue current iteration direction */
        dir=csc->dir;
    }

    যদি(dir<0) {
        যদি(csc->start<csc->index) {
            U8_PREV((ধ্রুৱ uint8_t *)csc->p, csc->start, csc->index, c);
            ঘূৰাই c;
        }
    } অন্যথা {
        যদি(csc->index<csc->limit) {
            U8_NEXT((ধ্রুৱ uint8_t *)csc->p, csc->index, csc->limit, c);
            ঘূৰাই c;
        }
    }
    ঘূৰাই U_SENTINEL;
}

/**
 * caseLocale >= 0: Lowercases [srcStart..srcLimit[ but takes context [0..srcLength[ into account.
 * caseLocale < 0: Case-folds [srcStart..srcLimit[.
 */
শূন্য toLower(int32_t caseLocale, uint32_t options,
             ধ্রুৱ uint8_t *src, UCaseContext *csc, int32_t srcStart, int32_t srcLimit,
             icu::ByteSink &sink, icu::Edits *edits, UErrorCode &errorCode) {
    ধ্রুৱ int8_t *latinToLower;
    যদি (caseLocale == UCASE_LOC_মূল ||
            (caseLocale >= 0 ?
                !(caseLocale == UCASE_LOC_TURKISH || caseLocale == UCASE_LOC_LITHUANIAN) :
                (options & _FOLD_CASE_OPTIONS_MASK) == U_FOLD_CASE_DEFAULT)) {
        latinToLower = LatinCase::TO_LOWER_NORMAL;
    } অন্যথা {
        latinToLower = LatinCase::TO_LOWER_TR_LT;
    }
    ধ্রুৱ UTrie2 *trie = ucase_getTrie();
    int32_t prev = srcStart;
    int32_t srcIndex = srcStart;
    বাবে (;;) {
        // fast path for simple cases
        int32_t cpStart;
        UChar32 c;
        বাবে (;;) {
            যদি (U_FAILURE(errorCode) || srcIndex >= srcLimit) {
                c = U_SENTINEL;
                ভঙ্গ;
            }
            uint8_t lead = src[srcIndex++];
            যদি (lead <= 0x7f) {
                int8_t d = latinToLower[lead];
                যদি (d == LatinCase::EXC) {
                    cpStart = srcIndex - 1;
                    c = lead;
                    ভঙ্গ;
                }
                যদি (d == 0) { অনুৱর্ত; }
                ByteSinkUtil::appendUnchanged(src + prev, srcIndex - 1 - prev,
                                              sink, options, edits, errorCode);
                ৱর্ণ ascii = static_cast<ৱর্ণ>(lead + d);
                sink.Append(&ascii, 1);
                যদি (edits != nullptr) {
                    edits->addReplace(1, 1);
                }
                prev = srcIndex;
                অনুৱর্ত;
            } অন্যথা যদি (lead < 0xe3) {
                uint8_t t;
                যদি (0xc2 <= lead && lead <= 0xc5 && srcIndex < srcLimit &&
                        (t = src[srcIndex] - 0x80) <= 0x3f) {
                    // U+0080..U+017F
                    ++srcIndex;
                    c = ((lead - 0xc0) << 6) | t;
                    int8_t d = latinToLower[c];
                    যদি (d == LatinCase::EXC) {
                        cpStart = srcIndex - 2;
                        ভঙ্গ;
                    }
                    যদি (d == 0) { অনুৱর্ত; }
                    ByteSinkUtil::appendUnchanged(src + prev, srcIndex - 2 - prev,
                                                  sink, options, edits, errorCode);
                    ByteSinkUtil::appendTwoBytes(c + d, sink);
                    যদি (edits != nullptr) {
                        edits->addReplace(2, 2);
                    }
                    prev = srcIndex;
                    অনুৱর্ত;
                }
            } অন্যথা যদি ((lead <= 0xe9 || lead == 0xeb || lead == 0xec) &&
                    (srcIndex + 2) <= srcLimit &&
                    U8_IS_TRAIL(src[srcIndex]) && U8_IS_TRAIL(src[srcIndex + 1])) {
                // most of CJK: no case mappings
                srcIndex += 2;
                অনুৱর্ত;
            }
            cpStart = --srcIndex;
            U8_NEXT(src, srcIndex, srcLimit, c);
            যদি (c < 0) {
                // ill-formed UTF-8
                অনুৱর্ত;
            }
            uint16_t props = UTRIE2_GET16(trie, c);
            যদি (UCASE_HAS_EXCEPTION(props)) { ভঙ্গ; }
            int32_t delta;
            যদি (!UCASE_IS_UPPER_OR_TITLE(props) || (delta = UCASE_GET_DELTA(props)) == 0) {
                অনুৱর্ত;
            }
            ByteSinkUtil::appendUnchanged(src + prev, cpStart - prev,
                                          sink, options, edits, errorCode);
            ByteSinkUtil::appendCodePoint(srcIndex - cpStart, c + delta, sink, edits);
            prev = srcIndex;
        }
        যদি (c < 0) {
            ভঙ্গ;
        }
        // slow path
        ধ্রুৱ char16_t *s;
        যদি (caseLocale >= 0) {
            csc->cpStart = cpStart;
            csc->cpLimit = srcIndex;
            c = ucase_toFullLower(c, utf8_caseContextIterator, csc, &s, caseLocale);
        } অন্যথা {
            c = ucase_toFullFolding(c, &s, options);
        }
        যদি (c >= 0) {
            ByteSinkUtil::appendUnchanged(src + prev, cpStart - prev,
                                          sink, options, edits, errorCode);
            appendResult(srcIndex - cpStart, c, s, sink, options, edits, errorCode);
            prev = srcIndex;
        }
    }
    ByteSinkUtil::appendUnchanged(src + prev, srcIndex - prev,
                                  sink, options, edits, errorCode);
}

শূন্য toUpper(int32_t caseLocale, uint32_t options,
             ধ্রুৱ uint8_t *src, UCaseContext *csc, int32_t srcLength,
             icu::ByteSink &sink, icu::Edits *edits, UErrorCode &errorCode) {
    ধ্রুৱ int8_t *latinToUpper;
    যদি (caseLocale == UCASE_LOC_TURKISH) {
        latinToUpper = LatinCase::TO_UPPER_TR;
    } অন্যথা {
        latinToUpper = LatinCase::TO_UPPER_NORMAL;
    }
    ধ্রুৱ UTrie2 *trie = ucase_getTrie();
    int32_t prev = 0;
    int32_t srcIndex = 0;
    বাবে (;;) {
        // fast path for simple cases
        int32_t cpStart;
        UChar32 c;
        বাবে (;;) {
            যদি (U_FAILURE(errorCode) || srcIndex >= srcLength) {
                c = U_SENTINEL;
                ভঙ্গ;
            }
            uint8_t lead = src[srcIndex++];
            যদি (lead <= 0x7f) {
                int8_t d = latinToUpper[lead];
                যদি (d == LatinCase::EXC) {
                    cpStart = srcIndex - 1;
                    c = lead;
                    ভঙ্গ;
                }
                যদি (d == 0) { অনুৱর্ত; }
                ByteSinkUtil::appendUnchanged(src + prev, srcIndex - 1 - prev,
                                              sink, options, edits, errorCode);
                ৱর্ণ ascii = static_cast<ৱর্ণ>(lead + d);
                sink.Append(&ascii, 1);
                যদি (edits != nullptr) {
                    edits->addReplace(1, 1);
                }
                prev = srcIndex;
                অনুৱর্ত;
            } অন্যথা যদি (lead < 0xe3) {
                uint8_t t;
                যদি (0xc2 <= lead && lead <= 0xc5 && srcIndex < srcLength &&
                        (t = src[srcIndex] - 0x80) <= 0x3f) {
                    // U+0080..U+017F
                    ++srcIndex;
                    c = ((lead - 0xc0) << 6) | t;
                    int8_t d = latinToUpper[c];
                    যদি (d == LatinCase::EXC) {
                        cpStart = srcIndex - 2;
                        ভঙ্গ;
                    }
                    যদি (d == 0) { অনুৱর্ত; }
                    ByteSinkUtil::appendUnchanged(src + prev, srcIndex - 2 - prev,
                                                  sink, options, edits, errorCode);
                    ByteSinkUtil::appendTwoBytes(c + d, sink);
                    যদি (edits != nullptr) {
                        edits->addReplace(2, 2);
                    }
                    prev = srcIndex;
                    অনুৱর্ত;
                }
            } অন্যথা যদি ((lead <= 0xe9 || lead == 0xeb || lead == 0xec) &&
                    (srcIndex + 2) <= srcLength &&
                    U8_IS_TRAIL(src[srcIndex]) && U8_IS_TRAIL(src[srcIndex + 1])) {
                // most of CJK: no case mappings
                srcIndex += 2;
                অনুৱর্ত;
            }
            cpStart = --srcIndex;
            U8_NEXT(src, srcIndex, srcLength, c);
            যদি (c < 0) {
                // ill-formed UTF-8
                অনুৱর্ত;
            }
            uint16_t props = UTRIE2_GET16(trie, c);
            যদি (UCASE_HAS_EXCEPTION(props)) { ভঙ্গ; }
            int32_t delta;
            যদি (UCASE_GET_TYPE(props) != UCASE_LOWER || (delta = UCASE_GET_DELTA(props)) == 0) {
                অনুৱর্ত;
            }
            ByteSinkUtil::appendUnchanged(src + prev, cpStart - prev,
                                          sink, options, edits, errorCode);
            ByteSinkUtil::appendCodePoint(srcIndex - cpStart, c + delta, sink, edits);
            prev = srcIndex;
        }
        যদি (c < 0) {
            ভঙ্গ;
        }
        // slow path
        csc->cpStart = cpStart;
        csc->cpLimit = srcIndex;
        ধ্রুৱ char16_t *s;
        c = ucase_toFullUpper(c, utf8_caseContextIterator, csc, &s, caseLocale);
        যদি (c >= 0) {
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
int32_t maybeTitleDutchIJ(ধ্রুৱ uint8_t *src, UChar32 c, int32_t start, int32_t segmentLimit,
                          ByteSink &sink, uint32_t options, icu::Edits *edits, UErrorCode &errorCode) {
    U_ASSERT(start < segmentLimit);

    int32_t index = start;
    তার্কিক withAcute = মিছা;

    // If the conditions are met, then the following variables tell us what to output.
    int32_t unchanged1 = 0;  // code units before the j, or the whole sequence (0..3)
    তার্কিক doTitleJ = মিছা;  // true if the j needs to be titlecased
    int32_t unchanged2 = 0;  // after the j (0 or 1)

    // next character after the first letter
    UChar32 c2;
    c2 = src[index++];

    // Is the first letter an i/I with accent?
    যদি (c == u'I') {
        যদি (c2 == ACUTE_BYTE0 && index < segmentLimit && src[index++] == ACUTE_BYTE1) {
            withAcute = সঁচা;
            unchanged1 = 2;  // ACUTE is 2 code units in UTF-8
            যদি (index == segmentLimit) { ঘূৰাই start; }
            c2 = src[index++];
        }
    } অন্যথা {  // Í
        withAcute = সঁচা;
    }

    // Is the next character a j/J?
    যদি (c2 == u'j') {
        doTitleJ = সঁচা;
    } অন্যথা যদি (c2 == u'J') {
        ++unchanged1;
    } অন্যথা {
        ঘূৰাই start;
    }

    // A plain i/I must be followed by a plain j/J.
    // An i/I with acute must be followed by a j/J with acute.
    যদি (withAcute) {
        যদি ((index + 1) >= segmentLimit || src[index++] != ACUTE_BYTE0 || src[index++] != ACUTE_BYTE1) {
            ঘূৰাই start;
        }
        যদি (doTitleJ) {
            unchanged2 = 2;  // ACUTE is 2 code units in UTF-8
        } অন্যথা {
            unchanged1 = unchanged1 + 2;    // ACUTE is 2 code units in UTF-8
        }
    }

    // There must not be another combining mark.
    যদি (index < segmentLimit) {
        int32_t cp;
        int32_t i = index;
        U8_NEXT(src, i, segmentLimit, cp);
        uint32_t typeMask = U_GET_GC_MASK(cp);
        যদি ((typeMask & U_GC_M_MASK) != 0) {
            ঘূৰাই start;
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

    U_ASSERT(start + unchanged2 == index);
    ঘূৰাই index;
}

}  // namespace

U_CFUNC শূন্য U_CALLCONV
ucasemap_internalUTF8ToTitle(
        int32_t caseLocale, uint32_t options, BreakIterator *iter,
        ধ্রুৱ uint8_t *src, int32_t srcLength,
        ByteSink &sink, icu::Edits *edits,
        UErrorCode &errorCode) {
    যদি (!ustrcase_checkTitleAdjustmentOptions(options, errorCode)) {
        ঘূৰাই;
    }

    /* set up local variables */
    UCaseContext csc=UCASECONTEXT_INITIALIZER;
    csc.p=(শূন্য *)src;
    csc.limit=srcLength;
    int32_t prev=0;
    UBool isFirstIndex=সঁচা;

    /* titlecasing loop */
    যেতিয়ালৈ(prev<srcLength) {
        /* find next index where to titlecase */
        int32_t index;
        যদি(isFirstIndex) {
            isFirstIndex=মিছা;
            index=iter->first();
        } অন্যথা {
            index=iter->next();
        }
        যদি(index==UBRK_DONE || index>srcLength) {
            index=srcLength;
        }

        /*
         * Segment [prev..index[ into 3 parts:
         * a) skipped characters (copy as-is) [prev..titleStart[
         * b) first letter (titlecase)              [titleStart..titleLimit[
         * c) subsequent characters (lowercase)                 [titleLimit..index[
         */
        যদি(prev<index) {
            /* find and copy skipped characters [prev..titleStart[ */
            int32_t titleStart=prev;
            int32_t titleLimit=prev;
            UChar32 c;
            U8_NEXT(src, titleLimit, index, c);
            যদি ((options&U_TITLECASE_NO_BREAK_ADJUSTMENT)==0) {
                // Adjust the titlecasing index to the next cased character,
                // or to the next letter/number/symbol/private use.
                // Stop with titleStart<titleLimit<=index
                // if there is a character to be titlecased,
                // or else stop with titleStart==titleLimit==index.
                UBool toCased = (options&U_TITLECASE_ADJUST_TO_CASED) != 0;
                যেতিয়ালৈ (toCased ? UCASE_NONE==ucase_getType(c) : !ustrcase_isLNS(c)) {
                    titleStart=titleLimit;
                    যদি(titleLimit==index) {
                        ভঙ্গ;
                    }
                    U8_NEXT(src, titleLimit, index, c);
                }
                যদি (prev < titleStart) {
                    যদি (!ByteSinkUtil::appendUnchanged(src+prev, titleStart-prev,
                                                       sink, options, edits, errorCode)) {
                        ঘূৰাই;
                    }
                }
            }

            যদি(titleStart<titleLimit) {
                /* titlecase c which is from [titleStart..titleLimit[ */
                যদি(c>=0) {
                    csc.cpStart=titleStart;
                    csc.cpLimit=titleLimit;
                    ধ্রুৱ char16_t *s;
                    c=ucase_toFullTitle(c, utf8_caseContextIterator, &csc, &s, caseLocale);
                    যদি (!appendResult(titleLimit-titleStart, c, s, sink, options, edits, errorCode)) {
                        ঘূৰাই;
                    }
                } অন্যথা {
                    // Malformed UTF-8.
                    যদি (!ByteSinkUtil::appendUnchanged(src+titleStart, titleLimit-titleStart,
                                                       sink, options, edits, errorCode)) {
                        ঘূৰাই;
                    }
                }

                /* Special case Dutch IJ titlecasing */
                যদি (titleLimit < index &&
                    caseLocale == UCASE_LOC_DUTCH) {
                    যদি (c < 0) {
                        c = ~c;
                    }

                    যদি (c == u'I' || c == u'Í') {
                        titleLimit = maybeTitleDutchIJ(src, c, titleLimit, index, sink, options, edits, errorCode);
                    }
                }

                /* lowercase [titleLimit..index[ */
                যদি(titleLimit<index) {
                    যদি((options&U_TITLECASE_NO_LOWERCASE)==0) {
                        /* Normal operation: Lowercase the rest of the word. */
                        toLower(caseLocale, options,
                                src, &csc, titleLimit, index,
                                sink, edits, errorCode);
                        যদি(U_FAILURE(errorCode)) {
                            ঘূৰাই;
                        }
                    } অন্যথা {
                        /* Optionally just copy the rest of the word unchanged. */
                        যদি (!ByteSinkUtil::appendUnchanged(src+titleLimit, index-titleLimit,
                                                           sink, options, edits, errorCode)) {
                            ঘূৰাই;
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
নামাকাশ GreekUpper {

UBool isFollowedByCasedLetter(ধ্রুৱ uint8_t *s, int32_t i, int32_t length) {
    যেতিয়ালৈ (i < length) {
        UChar32 c;
        U8_NEXT(s, i, length, c);
        int32_t type = ucase_getTypeOrIgnorable(c);
        যদি ((type & UCASE_IGNORABLE) != 0) {
            // Case-ignorable, continue with the loop.
        } অন্যথা যদি (type != UCASE_NONE) {
            ঘূৰাই সঁচা;  // Followed by cased letter.
        } অন্যথা {
            ঘূৰাই মিছা;  // Uncased and not case-ignorable.
        }
    }
    ঘূৰাই মিছা;  // Not followed by cased letter.
}

// Keep this consistent with the UTF-16 version in ustrcase.cpp and the Java version in CaseMap.java.
শূন্য toUpper(uint32_t options,
             ধ্রুৱ uint8_t *src, int32_t srcLength,
             ByteSink &sink, Edits *edits,
             UErrorCode &errorCode) {
    uint32_t state = 0;
    বাবে (int32_t i = 0; i < srcLength;) {
        int32_t nextIndex = i;
        UChar32 c;
        U8_NEXT(src, nextIndex, srcLength, c);
        uint32_t nextState = 0;
        int32_t type = ucase_getTypeOrIgnorable(c);
        যদি ((type & UCASE_IGNORABLE) != 0) {
            // c is case-ignorable
            nextState |= (state & AFTER_CASED);
        } অন্যথা যদি (type != UCASE_NONE) {
            // c is cased
            nextState |= AFTER_CASED;
        }
        uint32_t data = getLetterData(c);
        যদি (data > 0) {
            uint32_t upper = data & UPPER_MASK;
            // Add a dialytika to this iota or ypsilon vowel
            // if we removed a tonos from the previous vowel,
            // and that previous vowel did not also have (or gain) a dialytika.
            // Adding one only to the final vowel in a longer sequence
            // (which does not occur in normal writing) would require lookahead.
            // Set the same flag as for preserving an existing dialytika.
            যদি ((data & HAS_VOWEL) != 0 &&
                (state & (AFTER_VOWEL_WITH_PRECOMPOSED_ACCENT | AFTER_VOWEL_WITH_COMBINING_ACCENT)) !=
                    0 &&
                (upper == 0x399 || upper == 0x3A5)) {
                data |= (state & AFTER_VOWEL_WITH_PRECOMPOSED_ACCENT) != 0 ? HAS_DIALYTIKA
                                                                           : HAS_COMBINING_DIALYTIKA;
            }
            int32_t অঙ্ক_Ypogegrammeni = 0;  // Map each one to a trailing, spacing, capital iota.
            যদি ((data & HAS_YPOGEGRAMMENI) != 0) {
                অঙ্ক_Ypogegrammeni = 1;
            }
            ধ্রুৱ UBool hasPrecomposedAccent = (data & HAS_ACCENT) != 0;
            // Skip combining diacritics after this Greek letter.
            int32_t nextNextIndex = nextIndex;
            যেতিয়ালৈ (nextIndex < srcLength) {
                UChar32 c2;
                U8_NEXT(src, nextNextIndex, srcLength, c2);
                uint32_t diacriticData = getDiacriticData(c2);
                যদি (diacriticData != 0) {
                    data |= diacriticData;
                    যদি ((diacriticData & HAS_YPOGEGRAMMENI) != 0) {
                        ++অঙ্ক_Ypogegrammeni;
                    }
                    nextIndex = nextNextIndex;
                } অন্যথা {
                    ভঙ্গ;  // not a Greek diacritic
                }
            }
            যদি ((data & HAS_VOWEL_AND_ACCENT_AND_DIALYTIKA) == HAS_VOWEL_AND_ACCENT) {
                nextState |= hasPrecomposedAccent ? AFTER_VOWEL_WITH_PRECOMPOSED_ACCENT
                                                  : AFTER_VOWEL_WITH_COMBINING_ACCENT;
            }
            // Map according to Greek rules.
            UBool addTonos = মিছা;
            যদি (upper == 0x397 &&
                    (data & HAS_ACCENT) != 0 &&
                    অঙ্ক_Ypogegrammeni == 0 &&
                    (state & AFTER_CASED) == 0 &&
                    !isFollowedByCasedLetter(src, nextIndex, srcLength)) {
                // Keep disjunctive "or" with (only) a tonos.
                // We use the same "word boundary" conditions as for the Final_Sigma test.
                যদি (hasPrecomposedAccent) {
                    upper = 0x389;  // Preserve the precomposed form.
                } অন্যথা {
                    addTonos = সঁচা;
                }
            } অন্যথা যদি ((data & HAS_DIALYTIKA) != 0) {
                // Preserve a vowel with dialytika in precomposed form if it exists.
                যদি (upper == 0x399) {
                    upper = 0x3AA;
                    data &= ~HAS_EITHER_DIALYTIKA;
                } অন্যথা যদি (upper == 0x3A5) {
                    upper = 0x3AB;
                    data &= ~HAS_EITHER_DIALYTIKA;
                }
            }

            UBool change;
            যদি (edits == nullptr && (options & U_OMIT_UNCHANGED_TEXT) == 0) {
                change = সঁচা;  // common, simple usage
            } অন্যথা {
                // Find out first whether we are changing the text.
                U_ASSERT(0x370 <= upper && upper <= 0x3ff);  // 2-byte UTF-8, main Greek block
                change = (i + 2) > nextIndex ||
                        src[i] != getTwoByteLead(upper) || src[i + 1] != getTwoByteTrail(upper) ||
                        অঙ্ক_Ypogegrammeni > 0;
                int32_t i2 = i + 2;
                যদি ((data & HAS_EITHER_DIALYTIKA) != 0) {
                    change |= (i2 + 2) > nextIndex ||
                            src[i2] != static_cast<uint8_t>(u8"\u0308"[0]) ||
                            src[i2 + 1] != static_cast<uint8_t>(u8"\u0308"[1]);
                    i2 += 2;
                }
                যদি (addTonos) {
                    change |= (i2 + 2) > nextIndex ||
                            src[i2] != static_cast<uint8_t>(u8"\u0301"[0]) ||
                            src[i2 + 1] != static_cast<uint8_t>(u8"\u0301"[1]);
                    i2 += 2;
                }
                int32_t oldLength = nextIndex - i;
                int32_t newLength = (i2 - i) + অঙ্ক_Ypogegrammeni * 2;  // 2 bytes per U+0399
                change |= oldLength != newLength;
                যদি (change) {
                    যদি (edits != nullptr) {
                        edits->addReplace(oldLength, newLength);
                    }
                } অন্যথা {
                    যদি (edits != nullptr) {
                        edits->addUnchanged(oldLength);
                    }
                    // Write unchanged text?
                    change = (options & U_OMIT_UNCHANGED_TEXT) == 0;
                }
            }

            যদি (change) {
                ByteSinkUtil::appendTwoBytes(upper, sink);
                যদি ((data & HAS_EITHER_DIALYTIKA) != 0) {
                    sink.AppendU8(u8"\u0308", 2);  // restore or add a dialytika
                }
                যদি (addTonos) {
                    sink.AppendU8(u8"\u0301", 2);
                }
                যেতিয়ালৈ (অঙ্ক_Ypogegrammeni > 0) {
                    sink.AppendU8(u8"\u0399", 2);
                    --অঙ্ক_Ypogegrammeni;
                }
            }
        } অন্যথা যদি(c>=0) {
            ধ্রুৱ char16_t *s;
            c=ucase_toFullUpper(c, nullptr, nullptr, &s, UCASE_LOC_GREEK);
            যদি (!appendResult(nextIndex - i, c, s, sink, options, edits, errorCode)) {
                ঘূৰাই;
            }
        } অন্যথা {
            // Malformed UTF-8.
            যদি (!ByteSinkUtil::appendUnchanged(src+i, nextIndex-i,
                                               sink, options, edits, errorCode)) {
                ঘূৰাই;
            }
        }
        i = nextIndex;
        state = nextState;
    }
}

}  // namespace GreekUpper
U_NAMESPACE_END

স্থির শূন্য U_CALLCONV
ucasemap_internalUTF8ToLower(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_UNUSED
                             ধ্রুৱ uint8_t *src, int32_t srcLength,
                             icu::ByteSink &sink, icu::Edits *edits,
                             UErrorCode &errorCode) {
    UCaseContext csc=UCASECONTEXT_INITIALIZER;
    csc.p=(শূন্য *)src;
    csc.limit=srcLength;
    toLower(
        caseLocale, options,
        src, &csc, 0, srcLength,
        sink, edits, errorCode);
}

স্থির শূন্য U_CALLCONV
ucasemap_internalUTF8ToUpper(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_UNUSED
                             ধ্রুৱ uint8_t *src, int32_t srcLength,
                             icu::ByteSink &sink, icu::Edits *edits,
                             UErrorCode &errorCode) {
    যদি (caseLocale == UCASE_LOC_GREEK) {
        GreekUpper::toUpper(options, src, srcLength, sink, edits, errorCode);
    } অন্যথা {
        UCaseContext csc=UCASECONTEXT_INITIALIZER;
        csc.p=(শূন্য *)src;
        csc.limit=srcLength;
        toUpper(
            caseLocale, options,
            src, &csc, srcLength,
            sink, edits, errorCode);
    }
}

স্থির শূন্য U_CALLCONV
ucasemap_internalUTF8Fold(int32_t /* caseLocale */, uint32_t options, UCASEMAP_BREAK_ITERATOR_UNUSED
                          ধ্রুৱ uint8_t *src, int32_t srcLength,
                          icu::ByteSink &sink, icu::Edits *edits,
                          UErrorCode &errorCode) {
    toLower(
        -1, options,
        src, nullptr, 0, srcLength,
        sink, edits, errorCode);
}

শূন্য
ucasemap_mapUTF8(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_PARAM
                 ধ্রুৱ ৱর্ণ *src, int32_t srcLength,
                 UTF8CaseMapper *stringCaseMapper,
                 icu::ByteSink &sink, icu::Edits *edits,
                 UErrorCode &errorCode) {
    /* check argument values */
    যদি (U_FAILURE(errorCode)) {
        ঘূৰাই;
    }
    যদি ((src == nullptr && srcLength != 0) || srcLength < -1) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ঘূৰাই;
    }

    // Get the string length.
    যদি (srcLength == -1) {
        srcLength = static_cast<int32_t>(uprv_strlen(src));
    }

    যদি (edits != nullptr && (options & U_EDITS_NO_RESET) == 0) {
        edits->reset();
    }
    stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                     reinterpret_cast<ধ্রুৱ uint8_t*>(src), srcLength, sink, edits, errorCode);
    sink.Flush();
    যদি (U_SUCCESS(errorCode)) {
        যদি (edits != nullptr) {
            edits->copyErrorTo(errorCode);
        }
    }
}

int32_t
ucasemap_mapUTF8(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_PARAM
                 ৱর্ণ *dest, int32_t destCapacity,
                 ধ্রুৱ ৱর্ণ *src, int32_t srcLength,
                 UTF8CaseMapper *stringCaseMapper,
                 icu::Edits *edits,
                 UErrorCode &errorCode) {
    /* check argument values */
    যদি(U_FAILURE(errorCode)) {
        ঘূৰাই 0;
    }
    যদি( destCapacity<0 ||
        (dest==nullptr && destCapacity>0) ||
        (src==nullptr && srcLength!=0) || srcLength<-1
    ) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ঘূৰাই 0;
    }

    /* get the string length */
    যদি(srcLength==-1) {
        srcLength = static_cast<int32_t>(uprv_strlen(src));
    }

    /* check for overlapping source and destination */
    যদি( dest!=nullptr &&
        ((src>=dest && src<(dest+destCapacity)) ||
         (dest>=src && dest<(src+srcLength)))
    ) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ঘূৰাই 0;
    }

    যদি (edits != nullptr && (options & U_EDITS_NO_RESET) == 0) {
        edits->reset();
    }
    int32_t reslen = ByteSinkUtil::viaByteSinkToTerminatedChars(
        dest, destCapacity,
        [&](ByteSink& sink, UErrorCode& status) {
            stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                             reinterpret_cast<ধ্রুৱ uint8_t*>(src), srcLength, sink, edits, status);
        },
        errorCode);
    যদি (U_SUCCESS(errorCode) && edits != nullptr) {
        edits->copyErrorTo(errorCode);
    }
    ঘূৰাই reslen;
}

/* public API functions */

U_CAPI int32_t U_EXPORT2
ucasemap_utf8ToLower(ধ্রুৱ UCaseMap *csm,
                     ৱর্ণ *dest, int32_t destCapacity,
                     ধ্রুৱ ৱর্ণ *src, int32_t srcLength,
                     UErrorCode *pErrorCode) {
    auto *impl = reinterpret_cast<ধ্রুৱ UCaseMapImpl *>(csm);
    ঘূৰাই ucasemap_mapUTF8(
        impl->caseLocale, impl->options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, destCapacity,
        src, srcLength,
        ucasemap_internalUTF8ToLower, nullptr, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
ucasemap_utf8ToUpper(ধ্রুৱ UCaseMap *csm,
                     ৱর্ণ *dest, int32_t destCapacity,
                     ধ্রুৱ ৱর্ণ *src, int32_t srcLength,
                     UErrorCode *pErrorCode) {
    auto *impl = reinterpret_cast<ধ্রুৱ UCaseMapImpl *>(csm);
    ঘূৰাই ucasemap_mapUTF8(
        impl->caseLocale, impl->options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, destCapacity,
        src, srcLength,
        ucasemap_internalUTF8ToUpper, nullptr, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
ucasemap_utf8FoldCase(ধ্রুৱ UCaseMap *csm,
                      ৱর্ণ *dest, int32_t destCapacity,
                      ধ্রুৱ ৱর্ণ *src, int32_t srcLength,
                      UErrorCode *pErrorCode) {
    auto *impl = reinterpret_cast<ধ্রুৱ UCaseMapImpl *>(csm);
    ঘূৰাই ucasemap_mapUTF8(
        UCASE_LOC_মূল, impl->options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, destCapacity,
        src, srcLength,
        ucasemap_internalUTF8Fold, nullptr, *pErrorCode);
}

U_NAMESPACE_BEGIN

শূন্য CaseMap::utf8ToLower(
        ধ্রুৱ ৱর্ণ *locale, uint32_t options,
        StringPiece src, ByteSink &sink, Edits *edits,
        UErrorCode &errorCode) {
    ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        src.data(), src.length(),
        ucasemap_internalUTF8ToLower, sink, edits, errorCode);
}

শূন্য CaseMap::utf8ToUpper(
        ধ্রুৱ ৱর্ণ *locale, uint32_t options,
        StringPiece src, ByteSink &sink, Edits *edits,
        UErrorCode &errorCode) {
    ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        src.data(), src.length(),
        ucasemap_internalUTF8ToUpper, sink, edits, errorCode);
}

শূন্য CaseMap::utf8Fold(
        uint32_t options,
        StringPiece src, ByteSink &sink, Edits *edits,
        UErrorCode &errorCode) {
    ucasemap_mapUTF8(
        UCASE_LOC_মূল, options, UCASEMAP_BREAK_ITERATOR_NULL
        src.data(), src.length(),
        ucasemap_internalUTF8Fold, sink, edits, errorCode);
}

int32_t CaseMap::utf8ToLower(
        ধ্রুৱ ৱর্ণ *locale, uint32_t options,
        ধ্রুৱ ৱর্ণ *src, int32_t srcLength,
        ৱর্ণ *dest, int32_t destCapacity, Edits *edits,
        UErrorCode &errorCode) {
    ঘূৰাই ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, destCapacity,
        src, srcLength,
        ucasemap_internalUTF8ToLower, edits, errorCode);
}

int32_t CaseMap::utf8ToUpper(
        ধ্রুৱ ৱর্ণ *locale, uint32_t options,
        ধ্রুৱ ৱর্ণ *src, int32_t srcLength,
        ৱর্ণ *dest, int32_t destCapacity, Edits *edits,
        UErrorCode &errorCode) {
    ঘূৰাই ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, destCapacity,
        src, srcLength,
        ucasemap_internalUTF8ToUpper, edits, errorCode);
}

int32_t CaseMap::utf8Fold(
        uint32_t options,
        ধ্রুৱ ৱর্ণ *src, int32_t srcLength,
        ৱর্ণ *dest, int32_t destCapacity, Edits *edits,
        UErrorCode &errorCode) {
    ঘূৰাই ucasemap_mapUTF8(
        UCASE_LOC_মূল, options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, destCapacity,
        src, srcLength,
        ucasemap_internalUTF8Fold, edits, errorCode);
}

U_NAMESPACE_END
