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
#जर !UCONFIG_NO_BREAK_ITERATION
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

UCaseMapImpl::UCaseMapImpl(स्थिरांक अक्षर *localeID, uint32_t opts, UErrorCode *pErrorCode) :
#जर !UCONFIG_NO_BREAK_ITERATION
        iter(nullptr),
#endif
        caseLocale(UCASE_LOC_UNKNOWN), options(opts) {
    ucasemap_setLocale(reinterpret_cast<UCaseMap *>(हे), localeID, pErrorCode);
}

UCaseMapImpl::~UCaseMapImpl() {
#जर !UCONFIG_NO_BREAK_ITERATION
    delete iter;
#endif
}

U_NAMESPACE_END

U_CAPI UCaseMap * U_EXPORT2
ucasemap_open(स्थिरांक अक्षर *locale, uint32_t options, UErrorCode *pErrorCode) {
    जर(U_FAILURE(*pErrorCode)) {
        परत nullptr;
    }
    UCaseMapImpl *csm = नवीन UCaseMapImpl(locale, options, pErrorCode);
    जर(csm==nullptr) {
        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
        परत nullptr;
    } अन्यथा जर (U_FAILURE(*pErrorCode)) {
        delete csm;
        परत nullptr;
    }
    परत reinterpret_cast<UCaseMap *>(csm);
}

U_CAPI शून्य U_EXPORT2
ucasemap_close(UCaseMap *csm) {
    delete reinterpret_cast<UCaseMapImpl *>(csm);
}

U_CAPI स्थिरांक अक्षर* U_EXPORT2
ucasemap_getLocale(स्थिरांक UCaseMap* csm U_LIFETIME_BOUND) {
    परत reinterpret_cast<स्थिरांक UCaseMapImpl *>(csm)->locale;
}

U_CAPI uint32_t U_EXPORT2
ucasemap_getOptions(स्थिरांक UCaseMap *csm) {
    परत reinterpret_cast<स्थिरांक UCaseMapImpl *>(csm)->options;
}

U_CAPI शून्य U_EXPORT2
ucasemap_setLocale(UCaseMap *csm, स्थिरांक अक्षर *locale, UErrorCode *pErrorCode) {
    जर(U_FAILURE(*pErrorCode)) {
        परत;
    }
    स्वतः *impl = reinterpret_cast<UCaseMapImpl *>(csm);
    जर (locale != nullptr && *locale == 0) {
        impl->locale[0] = 0;
        impl->caseLocale = UCASE_LOC_मूळ;
        परत;
    }

    UErrorCode bufferStatus = U_ZERO_ERROR;
    int32_t length=uloc_getName(locale, impl->locale, (int32_t)आकार(impl->locale), &bufferStatus);
    जर(bufferStatus==U_BUFFER_OVERFLOW_ERROR || (U_SUCCESS(bufferStatus) && length==आकार(impl->locale))) {
        bufferStatus = U_ZERO_ERROR;
        /* we only really need the language code for case mappings */
        length=uloc_getLanguage(locale, impl->locale, (int32_t)आकार(impl->locale), &bufferStatus);
    }
    जर(U_FAILURE(bufferStatus)) {
        *pErrorCode=bufferStatus;
    } अन्यथा जर(length==आकार(impl->locale)) {
        *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
    }
    जर(U_SUCCESS(*pErrorCode)) {     
        impl->caseLocale = ucase_getCaseLocale(impl->locale);
    } अन्यथा {
        impl->locale[0]=0;
        impl->caseLocale = UCASE_LOC_मूळ;
    }
}

U_CAPI शून्य U_EXPORT2
ucasemap_setOptions(UCaseMap *csm, uint32_t options, UErrorCode *pErrorCode) {
    जर(U_FAILURE(*pErrorCode)) {
        परत;
    }
    reinterpret_cast<UCaseMapImpl *>(csm)->options=options;
}

/* UTF-8 string case mappings ----------------------------------------------- */

/* TODO(markus): Move to a new, separate utf8case.cpp file. */

नामाकाश {

/* append a full case mapping result, see UCASE_MAX_STRING_LENGTH */
ओळीत UBool
appendResult(int32_t cpLength, int32_t result, स्थिरांक char16_t *s,
             ByteSink &sink, uint32_t options, icu::Edits *edits, UErrorCode &errorCode) {
    U_ASSERT(U_SUCCESS(errorCode));

    /* decode the result */
    जर(result<0) {
        /* (not) original code point */
        जर(edits!=nullptr) {
            edits->addUnchanged(cpLength);
        }
        जर((options & U_OMIT_UNCHANGED_TEXT) == 0) {
            ByteSinkUtil::appendCodePoint(cpLength, ~result, sink);
        }
    } अन्यथा {
        जर(result<=UCASE_MAX_STRING_LENGTH) {
            // string: "result" is the UTF-16 length
            परत ByteSinkUtil::appendChange(cpLength, s, result, sink, edits, errorCode);
        } अन्यथा {
            ByteSinkUtil::appendCodePoint(cpLength, result, sink, edits);
        }
    }
    परत सत्य;
}

// See unicode/utf8.h U8_APPEND_UNSAFE().
ओळीत uint8_t getTwoByteLead(UChar32 c) { परत static_cast<uint8_t>((c >> 6) | 0xc0); }
ओळीत uint8_t getTwoByteTrail(UChar32 c) { परत static_cast<uint8_t>((c & 0x3f) | 0x80); }

UChar32 U_CALLCONV
utf8_caseContextIterator(शून्य *context, int8_t dir) {
    UCaseContext* csc = static_cast<UCaseContext*>(context);
    UChar32 c;

    जर(dir<0) {
        /* reset for backward iteration */
        csc->index=csc->cpStart;
        csc->dir=dir;
    } अन्यथा जर(dir>0) {
        /* reset for forward iteration */
        csc->index=csc->cpLimit;
        csc->dir=dir;
    } अन्यथा {
        /* continue current iteration direction */
        dir=csc->dir;
    }

    जर(dir<0) {
        जर(csc->start<csc->index) {
            U8_PREV((स्थिरांक uint8_t *)csc->p, csc->start, csc->index, c);
            परत c;
        }
    } अन्यथा {
        जर(csc->index<csc->limit) {
            U8_NEXT((स्थिरांक uint8_t *)csc->p, csc->index, csc->limit, c);
            परत c;
        }
    }
    परत U_SENTINEL;
}

/**
 * caseLocale >= 0: Lowercases [srcStart..srcLimit[ but takes context [0..srcLength[ into account.
 * caseLocale < 0: Case-folds [srcStart..srcLimit[.
 */
शून्य toLower(int32_t caseLocale, uint32_t options,
             स्थिरांक uint8_t *src, UCaseContext *csc, int32_t srcStart, int32_t srcLimit,
             icu::ByteSink &sink, icu::Edits *edits, UErrorCode &errorCode) {
    स्थिरांक int8_t *latinToLower;
    जर (caseLocale == UCASE_LOC_मूळ ||
            (caseLocale >= 0 ?
                !(caseLocale == UCASE_LOC_TURKISH || caseLocale == UCASE_LOC_LITHUANIAN) :
                (options & _FOLD_CASE_OPTIONS_MASK) == U_FOLD_CASE_DEFAULT)) {
        latinToLower = LatinCase::TO_LOWER_NORMAL;
    } अन्यथा {
        latinToLower = LatinCase::TO_LOWER_TR_LT;
    }
    स्थिरांक UTrie2 *trie = ucase_getTrie();
    int32_t prev = srcStart;
    int32_t srcIndex = srcStart;
    साठी (;;) {
        // fast path for simple cases
        int32_t cpStart;
        UChar32 c;
        साठी (;;) {
            जर (U_FAILURE(errorCode) || srcIndex >= srcLimit) {
                c = U_SENTINEL;
                थांबा;
            }
            uint8_t lead = src[srcIndex++];
            जर (lead <= 0x7f) {
                int8_t d = latinToLower[lead];
                जर (d == LatinCase::EXC) {
                    cpStart = srcIndex - 1;
                    c = lead;
                    थांबा;
                }
                जर (d == 0) { सुरूठेवा; }
                ByteSinkUtil::appendUnchanged(src + prev, srcIndex - 1 - prev,
                                              sink, options, edits, errorCode);
                अक्षर ascii = static_cast<अक्षर>(lead + d);
                sink.Append(&ascii, 1);
                जर (edits != nullptr) {
                    edits->addReplace(1, 1);
                }
                prev = srcIndex;
                सुरूठेवा;
            } अन्यथा जर (lead < 0xe3) {
                uint8_t t;
                जर (0xc2 <= lead && lead <= 0xc5 && srcIndex < srcLimit &&
                        (t = src[srcIndex] - 0x80) <= 0x3f) {
                    // U+0080..U+017F
                    ++srcIndex;
                    c = ((lead - 0xc0) << 6) | t;
                    int8_t d = latinToLower[c];
                    जर (d == LatinCase::EXC) {
                        cpStart = srcIndex - 2;
                        थांबा;
                    }
                    जर (d == 0) { सुरूठेवा; }
                    ByteSinkUtil::appendUnchanged(src + prev, srcIndex - 2 - prev,
                                                  sink, options, edits, errorCode);
                    ByteSinkUtil::appendTwoBytes(c + d, sink);
                    जर (edits != nullptr) {
                        edits->addReplace(2, 2);
                    }
                    prev = srcIndex;
                    सुरूठेवा;
                }
            } अन्यथा जर ((lead <= 0xe9 || lead == 0xeb || lead == 0xec) &&
                    (srcIndex + 2) <= srcLimit &&
                    U8_IS_TRAIL(src[srcIndex]) && U8_IS_TRAIL(src[srcIndex + 1])) {
                // most of CJK: no case mappings
                srcIndex += 2;
                सुरूठेवा;
            }
            cpStart = --srcIndex;
            U8_NEXT(src, srcIndex, srcLimit, c);
            जर (c < 0) {
                // ill-formed UTF-8
                सुरूठेवा;
            }
            uint16_t props = UTRIE2_GET16(trie, c);
            जर (UCASE_HAS_EXCEPTION(props)) { थांबा; }
            int32_t delta;
            जर (!UCASE_IS_UPPER_OR_TITLE(props) || (delta = UCASE_GET_DELTA(props)) == 0) {
                सुरूठेवा;
            }
            ByteSinkUtil::appendUnchanged(src + prev, cpStart - prev,
                                          sink, options, edits, errorCode);
            ByteSinkUtil::appendCodePoint(srcIndex - cpStart, c + delta, sink, edits);
            prev = srcIndex;
        }
        जर (c < 0) {
            थांबा;
        }
        // slow path
        स्थिरांक char16_t *s;
        जर (caseLocale >= 0) {
            csc->cpStart = cpStart;
            csc->cpLimit = srcIndex;
            c = ucase_toFullLower(c, utf8_caseContextIterator, csc, &s, caseLocale);
        } अन्यथा {
            c = ucase_toFullFolding(c, &s, options);
        }
        जर (c >= 0) {
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
             स्थिरांक uint8_t *src, UCaseContext *csc, int32_t srcLength,
             icu::ByteSink &sink, icu::Edits *edits, UErrorCode &errorCode) {
    स्थिरांक int8_t *latinToUpper;
    जर (caseLocale == UCASE_LOC_TURKISH) {
        latinToUpper = LatinCase::TO_UPPER_TR;
    } अन्यथा {
        latinToUpper = LatinCase::TO_UPPER_NORMAL;
    }
    स्थिरांक UTrie2 *trie = ucase_getTrie();
    int32_t prev = 0;
    int32_t srcIndex = 0;
    साठी (;;) {
        // fast path for simple cases
        int32_t cpStart;
        UChar32 c;
        साठी (;;) {
            जर (U_FAILURE(errorCode) || srcIndex >= srcLength) {
                c = U_SENTINEL;
                थांबा;
            }
            uint8_t lead = src[srcIndex++];
            जर (lead <= 0x7f) {
                int8_t d = latinToUpper[lead];
                जर (d == LatinCase::EXC) {
                    cpStart = srcIndex - 1;
                    c = lead;
                    थांबा;
                }
                जर (d == 0) { सुरूठेवा; }
                ByteSinkUtil::appendUnchanged(src + prev, srcIndex - 1 - prev,
                                              sink, options, edits, errorCode);
                अक्षर ascii = static_cast<अक्षर>(lead + d);
                sink.Append(&ascii, 1);
                जर (edits != nullptr) {
                    edits->addReplace(1, 1);
                }
                prev = srcIndex;
                सुरूठेवा;
            } अन्यथा जर (lead < 0xe3) {
                uint8_t t;
                जर (0xc2 <= lead && lead <= 0xc5 && srcIndex < srcLength &&
                        (t = src[srcIndex] - 0x80) <= 0x3f) {
                    // U+0080..U+017F
                    ++srcIndex;
                    c = ((lead - 0xc0) << 6) | t;
                    int8_t d = latinToUpper[c];
                    जर (d == LatinCase::EXC) {
                        cpStart = srcIndex - 2;
                        थांबा;
                    }
                    जर (d == 0) { सुरूठेवा; }
                    ByteSinkUtil::appendUnchanged(src + prev, srcIndex - 2 - prev,
                                                  sink, options, edits, errorCode);
                    ByteSinkUtil::appendTwoBytes(c + d, sink);
                    जर (edits != nullptr) {
                        edits->addReplace(2, 2);
                    }
                    prev = srcIndex;
                    सुरूठेवा;
                }
            } अन्यथा जर ((lead <= 0xe9 || lead == 0xeb || lead == 0xec) &&
                    (srcIndex + 2) <= srcLength &&
                    U8_IS_TRAIL(src[srcIndex]) && U8_IS_TRAIL(src[srcIndex + 1])) {
                // most of CJK: no case mappings
                srcIndex += 2;
                सुरूठेवा;
            }
            cpStart = --srcIndex;
            U8_NEXT(src, srcIndex, srcLength, c);
            जर (c < 0) {
                // ill-formed UTF-8
                सुरूठेवा;
            }
            uint16_t props = UTRIE2_GET16(trie, c);
            जर (UCASE_HAS_EXCEPTION(props)) { थांबा; }
            int32_t delta;
            जर (UCASE_GET_TYPE(props) != UCASE_LOWER || (delta = UCASE_GET_DELTA(props)) == 0) {
                सुरूठेवा;
            }
            ByteSinkUtil::appendUnchanged(src + prev, cpStart - prev,
                                          sink, options, edits, errorCode);
            ByteSinkUtil::appendCodePoint(srcIndex - cpStart, c + delta, sink, edits);
            prev = srcIndex;
        }
        जर (c < 0) {
            थांबा;
        }
        // slow path
        csc->cpStart = cpStart;
        csc->cpLimit = srcIndex;
        स्थिरांक char16_t *s;
        c = ucase_toFullUpper(c, utf8_caseContextIterator, csc, &s, caseLocale);
        जर (c >= 0) {
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

#जर !UCONFIG_NO_BREAK_ITERATION

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
int32_t maybeTitleDutchIJ(स्थिरांक uint8_t *src, UChar32 c, int32_t start, int32_t segmentLimit,
                          ByteSink &sink, uint32_t options, icu::Edits *edits, UErrorCode &errorCode) {
    U_ASSERT(start < segmentLimit);

    int32_t index = start;
    तार्किक withAcute = असत्य;

    // If the conditions are met, then the following variables tell us what to output.
    int32_t unchanged1 = 0;  // code units before the j, or the whole sequence (0..3)
    तार्किक doTitleJ = असत्य;  // true if the j needs to be titlecased
    int32_t unchanged2 = 0;  // after the j (0 or 1)

    // next character after the first letter
    UChar32 c2;
    c2 = src[index++];

    // Is the first letter an i/I with accent?
    जर (c == u'I') {
        जर (c2 == ACUTE_BYTE0 && index < segmentLimit && src[index++] == ACUTE_BYTE1) {
            withAcute = सत्य;
            unchanged1 = 2;  // ACUTE is 2 code units in UTF-8
            जर (index == segmentLimit) { परत start; }
            c2 = src[index++];
        }
    } अन्यथा {  // Í
        withAcute = सत्य;
    }

    // Is the next character a j/J?
    जर (c2 == u'j') {
        doTitleJ = सत्य;
    } अन्यथा जर (c2 == u'J') {
        ++unchanged1;
    } अन्यथा {
        परत start;
    }

    // A plain i/I must be followed by a plain j/J.
    // An i/I with acute must be followed by a j/J with acute.
    जर (withAcute) {
        जर ((index + 1) >= segmentLimit || src[index++] != ACUTE_BYTE0 || src[index++] != ACUTE_BYTE1) {
            परत start;
        }
        जर (doTitleJ) {
            unchanged2 = 2;  // ACUTE is 2 code units in UTF-8
        } अन्यथा {
            unchanged1 = unchanged1 + 2;    // ACUTE is 2 code units in UTF-8
        }
    }

    // There must not be another combining mark.
    जर (index < segmentLimit) {
        int32_t cp;
        int32_t i = index;
        U8_NEXT(src, i, segmentLimit, cp);
        uint32_t typeMask = U_GET_GC_MASK(cp);
        जर ((typeMask & U_GC_M_MASK) != 0) {
            परत start;
        }
    }

    // Output the rest of the Dutch IJ.
    ByteSinkUtil::appendUnchanged(src + start, unchanged1, sink, options, edits, errorCode);
    start += unchanged1;
    जर (doTitleJ) {
        ByteSinkUtil::appendCodePoint(1, u'J', sink, edits);
        ++start;
    }
    ByteSinkUtil::appendUnchanged(src + start, unchanged2, sink, options, edits, errorCode);

    U_ASSERT(start + unchanged2 == index);
    परत index;
}

}  // namespace

U_CFUNC शून्य U_CALLCONV
ucasemap_internalUTF8ToTitle(
        int32_t caseLocale, uint32_t options, BreakIterator *iter,
        स्थिरांक uint8_t *src, int32_t srcLength,
        ByteSink &sink, icu::Edits *edits,
        UErrorCode &errorCode) {
    जर (!ustrcase_checkTitleAdjustmentOptions(options, errorCode)) {
        परत;
    }

    /* set up local variables */
    UCaseContext csc=UCASECONTEXT_INITIALIZER;
    csc.p=(शून्य *)src;
    csc.limit=srcLength;
    int32_t prev=0;
    UBool isFirstIndex=सत्य;

    /* titlecasing loop */
    पर्यंत(prev<srcLength) {
        /* find next index where to titlecase */
        int32_t index;
        जर(isFirstIndex) {
            isFirstIndex=असत्य;
            index=iter->first();
        } अन्यथा {
            index=iter->next();
        }
        जर(index==UBRK_DONE || index>srcLength) {
            index=srcLength;
        }

        /*
         * Segment [prev..index[ into 3 parts:
         * a) skipped characters (copy as-is) [prev..titleStart[
         * b) first letter (titlecase)              [titleStart..titleLimit[
         * c) subsequent characters (lowercase)                 [titleLimit..index[
         */
        जर(prev<index) {
            /* find and copy skipped characters [prev..titleStart[ */
            int32_t titleStart=prev;
            int32_t titleLimit=prev;
            UChar32 c;
            U8_NEXT(src, titleLimit, index, c);
            जर ((options&U_TITLECASE_NO_BREAK_ADJUSTMENT)==0) {
                // Adjust the titlecasing index to the next cased character,
                // or to the next letter/number/symbol/private use.
                // Stop with titleStart<titleLimit<=index
                // if there is a character to be titlecased,
                // or else stop with titleStart==titleLimit==index.
                UBool toCased = (options&U_TITLECASE_ADJUST_TO_CASED) != 0;
                पर्यंत (toCased ? UCASE_NONE==ucase_getType(c) : !ustrcase_isLNS(c)) {
                    titleStart=titleLimit;
                    जर(titleLimit==index) {
                        थांबा;
                    }
                    U8_NEXT(src, titleLimit, index, c);
                }
                जर (prev < titleStart) {
                    जर (!ByteSinkUtil::appendUnchanged(src+prev, titleStart-prev,
                                                       sink, options, edits, errorCode)) {
                        परत;
                    }
                }
            }

            जर(titleStart<titleLimit) {
                /* titlecase c which is from [titleStart..titleLimit[ */
                जर(c>=0) {
                    csc.cpStart=titleStart;
                    csc.cpLimit=titleLimit;
                    स्थिरांक char16_t *s;
                    c=ucase_toFullTitle(c, utf8_caseContextIterator, &csc, &s, caseLocale);
                    जर (!appendResult(titleLimit-titleStart, c, s, sink, options, edits, errorCode)) {
                        परत;
                    }
                } अन्यथा {
                    // Malformed UTF-8.
                    जर (!ByteSinkUtil::appendUnchanged(src+titleStart, titleLimit-titleStart,
                                                       sink, options, edits, errorCode)) {
                        परत;
                    }
                }

                /* Special case Dutch IJ titlecasing */
                जर (titleLimit < index &&
                    caseLocale == UCASE_LOC_DUTCH) {
                    जर (c < 0) {
                        c = ~c;
                    }

                    जर (c == u'I' || c == u'Í') {
                        titleLimit = maybeTitleDutchIJ(src, c, titleLimit, index, sink, options, edits, errorCode);
                    }
                }

                /* lowercase [titleLimit..index[ */
                जर(titleLimit<index) {
                    जर((options&U_TITLECASE_NO_LOWERCASE)==0) {
                        /* Normal operation: Lowercase the rest of the word. */
                        toLower(caseLocale, options,
                                src, &csc, titleLimit, index,
                                sink, edits, errorCode);
                        जर(U_FAILURE(errorCode)) {
                            परत;
                        }
                    } अन्यथा {
                        /* Optionally just copy the rest of the word unchanged. */
                        जर (!ByteSinkUtil::appendUnchanged(src+titleLimit, index-titleLimit,
                                                           sink, options, edits, errorCode)) {
                            परत;
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

UBool isFollowedByCasedLetter(स्थिरांक uint8_t *s, int32_t i, int32_t length) {
    पर्यंत (i < length) {
        UChar32 c;
        U8_NEXT(s, i, length, c);
        int32_t type = ucase_getTypeOrIgnorable(c);
        जर ((type & UCASE_IGNORABLE) != 0) {
            // Case-ignorable, continue with the loop.
        } अन्यथा जर (type != UCASE_NONE) {
            परत सत्य;  // Followed by cased letter.
        } अन्यथा {
            परत असत्य;  // Uncased and not case-ignorable.
        }
    }
    परत असत्य;  // Not followed by cased letter.
}

// Keep this consistent with the UTF-16 version in ustrcase.cpp and the Java version in CaseMap.java.
शून्य toUpper(uint32_t options,
             स्थिरांक uint8_t *src, int32_t srcLength,
             ByteSink &sink, Edits *edits,
             UErrorCode &errorCode) {
    uint32_t state = 0;
    साठी (int32_t i = 0; i < srcLength;) {
        int32_t nextIndex = i;
        UChar32 c;
        U8_NEXT(src, nextIndex, srcLength, c);
        uint32_t nextState = 0;
        int32_t type = ucase_getTypeOrIgnorable(c);
        जर ((type & UCASE_IGNORABLE) != 0) {
            // c is case-ignorable
            nextState |= (state & AFTER_CASED);
        } अन्यथा जर (type != UCASE_NONE) {
            // c is cased
            nextState |= AFTER_CASED;
        }
        uint32_t data = getLetterData(c);
        जर (data > 0) {
            uint32_t upper = data & UPPER_MASK;
            // Add a dialytika to this iota or ypsilon vowel
            // if we removed a tonos from the previous vowel,
            // and that previous vowel did not also have (or gain) a dialytika.
            // Adding one only to the final vowel in a longer sequence
            // (which does not occur in normal writing) would require lookahead.
            // Set the same flag as for preserving an existing dialytika.
            जर ((data & HAS_VOWEL) != 0 &&
                (state & (AFTER_VOWEL_WITH_PRECOMPOSED_ACCENT | AFTER_VOWEL_WITH_COMBINING_ACCENT)) !=
                    0 &&
                (upper == 0x399 || upper == 0x3A5)) {
                data |= (state & AFTER_VOWEL_WITH_PRECOMPOSED_ACCENT) != 0 ? HAS_DIALYTIKA
                                                                           : HAS_COMBINING_DIALYTIKA;
            }
            int32_t अंक_Ypogegrammeni = 0;  // Map each one to a trailing, spacing, capital iota.
            जर ((data & HAS_YPOGEGRAMMENI) != 0) {
                अंक_Ypogegrammeni = 1;
            }
            स्थिरांक UBool hasPrecomposedAccent = (data & HAS_ACCENT) != 0;
            // Skip combining diacritics after this Greek letter.
            int32_t nextNextIndex = nextIndex;
            पर्यंत (nextIndex < srcLength) {
                UChar32 c2;
                U8_NEXT(src, nextNextIndex, srcLength, c2);
                uint32_t diacriticData = getDiacriticData(c2);
                जर (diacriticData != 0) {
                    data |= diacriticData;
                    जर ((diacriticData & HAS_YPOGEGRAMMENI) != 0) {
                        ++अंक_Ypogegrammeni;
                    }
                    nextIndex = nextNextIndex;
                } अन्यथा {
                    थांबा;  // not a Greek diacritic
                }
            }
            जर ((data & HAS_VOWEL_AND_ACCENT_AND_DIALYTIKA) == HAS_VOWEL_AND_ACCENT) {
                nextState |= hasPrecomposedAccent ? AFTER_VOWEL_WITH_PRECOMPOSED_ACCENT
                                                  : AFTER_VOWEL_WITH_COMBINING_ACCENT;
            }
            // Map according to Greek rules.
            UBool addTonos = असत्य;
            जर (upper == 0x397 &&
                    (data & HAS_ACCENT) != 0 &&
                    अंक_Ypogegrammeni == 0 &&
                    (state & AFTER_CASED) == 0 &&
                    !isFollowedByCasedLetter(src, nextIndex, srcLength)) {
                // Keep disjunctive "or" with (only) a tonos.
                // We use the same "word boundary" conditions as for the Final_Sigma test.
                जर (hasPrecomposedAccent) {
                    upper = 0x389;  // Preserve the precomposed form.
                } अन्यथा {
                    addTonos = सत्य;
                }
            } अन्यथा जर ((data & HAS_DIALYTIKA) != 0) {
                // Preserve a vowel with dialytika in precomposed form if it exists.
                जर (upper == 0x399) {
                    upper = 0x3AA;
                    data &= ~HAS_EITHER_DIALYTIKA;
                } अन्यथा जर (upper == 0x3A5) {
                    upper = 0x3AB;
                    data &= ~HAS_EITHER_DIALYTIKA;
                }
            }

            UBool change;
            जर (edits == nullptr && (options & U_OMIT_UNCHANGED_TEXT) == 0) {
                change = सत्य;  // common, simple usage
            } अन्यथा {
                // Find out first whether we are changing the text.
                U_ASSERT(0x370 <= upper && upper <= 0x3ff);  // 2-byte UTF-8, main Greek block
                change = (i + 2) > nextIndex ||
                        src[i] != getTwoByteLead(upper) || src[i + 1] != getTwoByteTrail(upper) ||
                        अंक_Ypogegrammeni > 0;
                int32_t i2 = i + 2;
                जर ((data & HAS_EITHER_DIALYTIKA) != 0) {
                    change |= (i2 + 2) > nextIndex ||
                            src[i2] != static_cast<uint8_t>(u8"\u0308"[0]) ||
                            src[i2 + 1] != static_cast<uint8_t>(u8"\u0308"[1]);
                    i2 += 2;
                }
                जर (addTonos) {
                    change |= (i2 + 2) > nextIndex ||
                            src[i2] != static_cast<uint8_t>(u8"\u0301"[0]) ||
                            src[i2 + 1] != static_cast<uint8_t>(u8"\u0301"[1]);
                    i2 += 2;
                }
                int32_t oldLength = nextIndex - i;
                int32_t newLength = (i2 - i) + अंक_Ypogegrammeni * 2;  // 2 bytes per U+0399
                change |= oldLength != newLength;
                जर (change) {
                    जर (edits != nullptr) {
                        edits->addReplace(oldLength, newLength);
                    }
                } अन्यथा {
                    जर (edits != nullptr) {
                        edits->addUnchanged(oldLength);
                    }
                    // Write unchanged text?
                    change = (options & U_OMIT_UNCHANGED_TEXT) == 0;
                }
            }

            जर (change) {
                ByteSinkUtil::appendTwoBytes(upper, sink);
                जर ((data & HAS_EITHER_DIALYTIKA) != 0) {
                    sink.AppendU8(u8"\u0308", 2);  // restore or add a dialytika
                }
                जर (addTonos) {
                    sink.AppendU8(u8"\u0301", 2);
                }
                पर्यंत (अंक_Ypogegrammeni > 0) {
                    sink.AppendU8(u8"\u0399", 2);
                    --अंक_Ypogegrammeni;
                }
            }
        } अन्यथा जर(c>=0) {
            स्थिरांक char16_t *s;
            c=ucase_toFullUpper(c, nullptr, nullptr, &s, UCASE_LOC_GREEK);
            जर (!appendResult(nextIndex - i, c, s, sink, options, edits, errorCode)) {
                परत;
            }
        } अन्यथा {
            // Malformed UTF-8.
            जर (!ByteSinkUtil::appendUnchanged(src+i, nextIndex-i,
                                               sink, options, edits, errorCode)) {
                परत;
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
                             स्थिरांक uint8_t *src, int32_t srcLength,
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
                             स्थिरांक uint8_t *src, int32_t srcLength,
                             icu::ByteSink &sink, icu::Edits *edits,
                             UErrorCode &errorCode) {
    जर (caseLocale == UCASE_LOC_GREEK) {
        GreekUpper::toUpper(options, src, srcLength, sink, edits, errorCode);
    } अन्यथा {
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
                          स्थिरांक uint8_t *src, int32_t srcLength,
                          icu::ByteSink &sink, icu::Edits *edits,
                          UErrorCode &errorCode) {
    toLower(
        -1, options,
        src, nullptr, 0, srcLength,
        sink, edits, errorCode);
}

शून्य
ucasemap_mapUTF8(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_PARAM
                 स्थिरांक अक्षर *src, int32_t srcLength,
                 UTF8CaseMapper *stringCaseMapper,
                 icu::ByteSink &sink, icu::Edits *edits,
                 UErrorCode &errorCode) {
    /* check argument values */
    जर (U_FAILURE(errorCode)) {
        परत;
    }
    जर ((src == nullptr && srcLength != 0) || srcLength < -1) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        परत;
    }

    // Get the string length.
    जर (srcLength == -1) {
        srcLength = static_cast<int32_t>(uprv_strlen(src));
    }

    जर (edits != nullptr && (options & U_EDITS_NO_RESET) == 0) {
        edits->reset();
    }
    stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                     reinterpret_cast<स्थिरांक uint8_t*>(src), srcLength, sink, edits, errorCode);
    sink.Flush();
    जर (U_SUCCESS(errorCode)) {
        जर (edits != nullptr) {
            edits->copyErrorTo(errorCode);
        }
    }
}

int32_t
ucasemap_mapUTF8(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_PARAM
                 अक्षर *dest, int32_t destCapacity,
                 स्थिरांक अक्षर *src, int32_t srcLength,
                 UTF8CaseMapper *stringCaseMapper,
                 icu::Edits *edits,
                 UErrorCode &errorCode) {
    /* check argument values */
    जर(U_FAILURE(errorCode)) {
        परत 0;
    }
    जर( destCapacity<0 ||
        (dest==nullptr && destCapacity>0) ||
        (src==nullptr && srcLength!=0) || srcLength<-1
    ) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        परत 0;
    }

    /* get the string length */
    जर(srcLength==-1) {
        srcLength = static_cast<int32_t>(uprv_strlen(src));
    }

    /* check for overlapping source and destination */
    जर( dest!=nullptr &&
        ((src>=dest && src<(dest+destCapacity)) ||
         (dest>=src && dest<(src+srcLength)))
    ) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        परत 0;
    }

    जर (edits != nullptr && (options & U_EDITS_NO_RESET) == 0) {
        edits->reset();
    }
    int32_t reslen = ByteSinkUtil::viaByteSinkToTerminatedChars(
        dest, destCapacity,
        [&](ByteSink& sink, UErrorCode& status) {
            stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                             reinterpret_cast<स्थिरांक uint8_t*>(src), srcLength, sink, edits, status);
        },
        errorCode);
    जर (U_SUCCESS(errorCode) && edits != nullptr) {
        edits->copyErrorTo(errorCode);
    }
    परत reslen;
}

/* public API functions */

U_CAPI int32_t U_EXPORT2
ucasemap_utf8ToLower(स्थिरांक UCaseMap *csm,
                     अक्षर *dest, int32_t destCapacity,
                     स्थिरांक अक्षर *src, int32_t srcLength,
                     UErrorCode *pErrorCode) {
    स्वतः *impl = reinterpret_cast<स्थिरांक UCaseMapImpl *>(csm);
    परत ucasemap_mapUTF8(
        impl->caseLocale, impl->options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, destCapacity,
        src, srcLength,
        ucasemap_internalUTF8ToLower, nullptr, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
ucasemap_utf8ToUpper(स्थिरांक UCaseMap *csm,
                     अक्षर *dest, int32_t destCapacity,
                     स्थिरांक अक्षर *src, int32_t srcLength,
                     UErrorCode *pErrorCode) {
    स्वतः *impl = reinterpret_cast<स्थिरांक UCaseMapImpl *>(csm);
    परत ucasemap_mapUTF8(
        impl->caseLocale, impl->options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, destCapacity,
        src, srcLength,
        ucasemap_internalUTF8ToUpper, nullptr, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
ucasemap_utf8FoldCase(स्थिरांक UCaseMap *csm,
                      अक्षर *dest, int32_t destCapacity,
                      स्थिरांक अक्षर *src, int32_t srcLength,
                      UErrorCode *pErrorCode) {
    स्वतः *impl = reinterpret_cast<स्थिरांक UCaseMapImpl *>(csm);
    परत ucasemap_mapUTF8(
        UCASE_LOC_मूळ, impl->options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, destCapacity,
        src, srcLength,
        ucasemap_internalUTF8Fold, nullptr, *pErrorCode);
}

U_NAMESPACE_BEGIN

शून्य CaseMap::utf8ToLower(
        स्थिरांक अक्षर *locale, uint32_t options,
        StringPiece src, ByteSink &sink, Edits *edits,
        UErrorCode &errorCode) {
    ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        src.data(), src.length(),
        ucasemap_internalUTF8ToLower, sink, edits, errorCode);
}

शून्य CaseMap::utf8ToUpper(
        स्थिरांक अक्षर *locale, uint32_t options,
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
        UCASE_LOC_मूळ, options, UCASEMAP_BREAK_ITERATOR_NULL
        src.data(), src.length(),
        ucasemap_internalUTF8Fold, sink, edits, errorCode);
}

int32_t CaseMap::utf8ToLower(
        स्थिरांक अक्षर *locale, uint32_t options,
        स्थिरांक अक्षर *src, int32_t srcLength,
        अक्षर *dest, int32_t destCapacity, Edits *edits,
        UErrorCode &errorCode) {
    परत ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, destCapacity,
        src, srcLength,
        ucasemap_internalUTF8ToLower, edits, errorCode);
}

int32_t CaseMap::utf8ToUpper(
        स्थिरांक अक्षर *locale, uint32_t options,
        स्थिरांक अक्षर *src, int32_t srcLength,
        अक्षर *dest, int32_t destCapacity, Edits *edits,
        UErrorCode &errorCode) {
    परत ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, destCapacity,
        src, srcLength,
        ucasemap_internalUTF8ToUpper, edits, errorCode);
}

int32_t CaseMap::utf8Fold(
        uint32_t options,
        स्थिरांक अक्षर *src, int32_t srcLength,
        अक्षर *dest, int32_t destCapacity, Edits *edits,
        UErrorCode &errorCode) {
    परत ucasemap_mapUTF8(
        UCASE_LOC_मूळ, options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, destCapacity,
        src, srcLength,
        ucasemap_internalUTF8Fold, edits, errorCode);
}

U_NAMESPACE_END
