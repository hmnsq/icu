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

#અન્તર્ભાવ "unicode/utypes.h"
#અન્તર્ભાવ "unicode/brkiter.h"
#અન્તર્ભાવ "unicode/bytestream.h"
#અન્તર્ભાવ "unicode/casemap.h"
#અન્તર્ભાવ "unicode/edits.h"
#અન્તર્ભાવ "unicode/stringoptions.h"
#અન્તર્ભાવ "unicode/stringpiece.h"
#અન્તર્ભાવ "unicode/ubrk.h"
#અન્તર્ભાવ "unicode/uloc.h"
#અન્તર્ભાવ "unicode/ustring.h"
#અન્તર્ભાવ "unicode/ucasemap.h"
#જો !UCONFIG_NO_BREAK_ITERATION
#અન્તર્ભાવ "unicode/utext.h"
#endif
#અન્તર્ભાવ "unicode/utf.h"
#અન્તર્ભાવ "unicode/utf8.h"
#અન્તર્ભાવ "unicode/utf16.h"
#અન્તર્ભાવ "bytesinkutil.h"
#અન્તર્ભાવ "cmemory.h"
#અન્તર્ભાવ "cstring.h"
#અન્તર્ભાવ "uassert.h"
#અન્તર્ભાવ "ucase.h"
#અન્તર્ભાવ "ucasemap_imp.h"

U_NAMESPACE_USE

/* UCaseMap service object -------------------------------------------------- */

// The public type, unicode/ucasemap.h struct UCaseMap, is an opaque C type.
// We define a parallel C++ type in the ICU namespace for entry point renaming.

U_NAMESPACE_BEGIN

UCaseMapImpl::UCaseMapImpl(અચલ અક્ષર *localeID, uint32_t opts, UErrorCode *pErrorCode) :
#જો !UCONFIG_NO_BREAK_ITERATION
        iter(nullptr),
#endif
        caseLocale(UCASE_LOC_UNKNOWN), options(opts) {
    ucasemap_setLocale(reinterpret_cast<UCaseMap *>(આ), localeID, pErrorCode);
}

UCaseMapImpl::~UCaseMapImpl() {
#જો !UCONFIG_NO_BREAK_ITERATION
    delete iter;
#endif
}

U_NAMESPACE_END

U_CAPI UCaseMap * U_EXPORT2
ucasemap_open(અચલ અક્ષર *locale, uint32_t options, UErrorCode *pErrorCode) {
    જો(U_FAILURE(*pErrorCode)) {
        પાછા nullptr;
    }
    UCaseMapImpl *csm = નવું UCaseMapImpl(locale, options, pErrorCode);
    જો(csm==nullptr) {
        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
        પાછા nullptr;
    } નહીંતર જો (U_FAILURE(*pErrorCode)) {
        delete csm;
        પાછા nullptr;
    }
    પાછા reinterpret_cast<UCaseMap *>(csm);
}

U_CAPI શૂન્ય U_EXPORT2
ucasemap_close(UCaseMap *csm) {
    delete reinterpret_cast<UCaseMapImpl *>(csm);
}

U_CAPI અચલ અક્ષર* U_EXPORT2
ucasemap_getLocale(અચલ UCaseMap* csm U_LIFETIME_BOUND) {
    પાછા reinterpret_cast<અચલ UCaseMapImpl *>(csm)->locale;
}

U_CAPI uint32_t U_EXPORT2
ucasemap_getOptions(અચલ UCaseMap *csm) {
    પાછા reinterpret_cast<અચલ UCaseMapImpl *>(csm)->options;
}

U_CAPI શૂન્ય U_EXPORT2
ucasemap_setLocale(UCaseMap *csm, અચલ અક્ષર *locale, UErrorCode *pErrorCode) {
    જો(U_FAILURE(*pErrorCode)) {
        પાછા;
    }
    auto *impl = reinterpret_cast<UCaseMapImpl *>(csm);
    જો (locale != nullptr && *locale == 0) {
        impl->locale[0] = 0;
        impl->caseLocale = UCASE_LOC_મૂળ;
        પાછા;
    }

    UErrorCode bufferStatus = U_ZERO_ERROR;
    int32_t length=uloc_getName(locale, impl->locale, (int32_t)કદ(impl->locale), &bufferStatus);
    જો(bufferStatus==U_BUFFER_OVERFLOW_ERROR || (U_SUCCESS(bufferStatus) && length==કદ(impl->locale))) {
        bufferStatus = U_ZERO_ERROR;
        /* we only really need the language code for case mappings */
        length=uloc_getLanguage(locale, impl->locale, (int32_t)કદ(impl->locale), &bufferStatus);
    }
    જો(U_FAILURE(bufferStatus)) {
        *pErrorCode=bufferStatus;
    } નહીંતર જો(length==કદ(impl->locale)) {
        *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
    }
    જો(U_SUCCESS(*pErrorCode)) {     
        impl->caseLocale = ucase_getCaseLocale(impl->locale);
    } નહીંતર {
        impl->locale[0]=0;
        impl->caseLocale = UCASE_LOC_મૂળ;
    }
}

U_CAPI શૂન્ય U_EXPORT2
ucasemap_setOptions(UCaseMap *csm, uint32_t options, UErrorCode *pErrorCode) {
    જો(U_FAILURE(*pErrorCode)) {
        પાછા;
    }
    reinterpret_cast<UCaseMapImpl *>(csm)->options=options;
}

/* UTF-8 string case mappings ----------------------------------------------- */

/* TODO(markus): Move to a new, separate utf8case.cpp file. */

નામાકાશ {

/* append a full case mapping result, see UCASE_MAX_STRING_LENGTH */
અંતઃરેખ UBool
appendResult(int32_t cpLength, int32_t result, અચલ char16_t *s,
             ByteSink &sink, uint32_t options, icu::Edits *edits, UErrorCode &errorCode) {
    U_ASSERT(U_SUCCESS(errorCode));

    /* decode the result */
    જો(result<0) {
        /* (not) original code point */
        જો(edits!=nullptr) {
            edits->addUnchanged(cpLength);
        }
        જો((options & U_OMIT_UNCHANGED_TEXT) == 0) {
            ByteSinkUtil::appendCodePoint(cpLength, ~result, sink);
        }
    } નહીંતર {
        જો(result<=UCASE_MAX_STRING_LENGTH) {
            // string: "result" is the UTF-16 length
            પાછા ByteSinkUtil::appendChange(cpLength, s, result, sink, edits, errorCode);
        } નહીંતર {
            ByteSinkUtil::appendCodePoint(cpLength, result, sink, edits);
        }
    }
    પાછા સાચું;
}

// See unicode/utf8.h U8_APPEND_UNSAFE().
અંતઃરેખ uint8_t getTwoByteLead(UChar32 c) { પાછા static_cast<uint8_t>((c >> 6) | 0xc0); }
અંતઃરેખ uint8_t getTwoByteTrail(UChar32 c) { પાછા static_cast<uint8_t>((c & 0x3f) | 0x80); }

UChar32 U_CALLCONV
utf8_caseContextIterator(શૂન્ય *context, int8_t dir) {
    UCaseContext* csc = static_cast<UCaseContext*>(context);
    UChar32 c;

    જો(dir<0) {
        /* reset for backward iteration */
        csc->index=csc->cpStart;
        csc->dir=dir;
    } નહીંતર જો(dir>0) {
        /* reset for forward iteration */
        csc->index=csc->cpLimit;
        csc->dir=dir;
    } નહીંતર {
        /* continue current iteration direction */
        dir=csc->dir;
    }

    જો(dir<0) {
        જો(csc->start<csc->index) {
            U8_PREV((અચલ uint8_t *)csc->p, csc->start, csc->index, c);
            પાછા c;
        }
    } નહીંતર {
        જો(csc->index<csc->limit) {
            U8_NEXT((અચલ uint8_t *)csc->p, csc->index, csc->limit, c);
            પાછા c;
        }
    }
    પાછા U_SENTINEL;
}

/**
 * caseLocale >= 0: Lowercases [srcStart..srcLimit[ but takes context [0..srcLength[ into account.
 * caseLocale < 0: Case-folds [srcStart..srcLimit[.
 */
શૂન્ય toLower(int32_t caseLocale, uint32_t options,
             અચલ uint8_t *src, UCaseContext *csc, int32_t srcStart, int32_t srcLimit,
             icu::ByteSink &sink, icu::Edits *edits, UErrorCode &errorCode) {
    અચલ int8_t *latinToLower;
    જો (caseLocale == UCASE_LOC_મૂળ ||
            (caseLocale >= 0 ?
                !(caseLocale == UCASE_LOC_TURKISH || caseLocale == UCASE_LOC_LITHUANIAN) :
                (options & _FOLD_CASE_OPTIONS_MASK) == U_FOLD_CASE_DEFAULT)) {
        latinToLower = LatinCase::TO_LOWER_NORMAL;
    } નહીંતર {
        latinToLower = LatinCase::TO_LOWER_TR_LT;
    }
    અચલ UTrie2 *trie = ucase_getTrie();
    int32_t prev = srcStart;
    int32_t srcIndex = srcStart;
    માટે (;;) {
        // fast path for simple cases
        int32_t cpStart;
        UChar32 c;
        માટે (;;) {
            જો (U_FAILURE(errorCode) || srcIndex >= srcLimit) {
                c = U_SENTINEL;
                તોડો;
            }
            uint8_t lead = src[srcIndex++];
            જો (lead <= 0x7f) {
                int8_t d = latinToLower[lead];
                જો (d == LatinCase::EXC) {
                    cpStart = srcIndex - 1;
                    c = lead;
                    તોડો;
                }
                જો (d == 0) { ચાલુ; }
                ByteSinkUtil::appendUnchanged(src + prev, srcIndex - 1 - prev,
                                              sink, options, edits, errorCode);
                અક્ષર ascii = static_cast<અક્ષર>(lead + d);
                sink.Append(&ascii, 1);
                જો (edits != nullptr) {
                    edits->addReplace(1, 1);
                }
                prev = srcIndex;
                ચાલુ;
            } નહીંતર જો (lead < 0xe3) {
                uint8_t t;
                જો (0xc2 <= lead && lead <= 0xc5 && srcIndex < srcLimit &&
                        (t = src[srcIndex] - 0x80) <= 0x3f) {
                    // U+0080..U+017F
                    ++srcIndex;
                    c = ((lead - 0xc0) << 6) | t;
                    int8_t d = latinToLower[c];
                    જો (d == LatinCase::EXC) {
                        cpStart = srcIndex - 2;
                        તોડો;
                    }
                    જો (d == 0) { ચાલુ; }
                    ByteSinkUtil::appendUnchanged(src + prev, srcIndex - 2 - prev,
                                                  sink, options, edits, errorCode);
                    ByteSinkUtil::appendTwoBytes(c + d, sink);
                    જો (edits != nullptr) {
                        edits->addReplace(2, 2);
                    }
                    prev = srcIndex;
                    ચાલુ;
                }
            } નહીંતર જો ((lead <= 0xe9 || lead == 0xeb || lead == 0xec) &&
                    (srcIndex + 2) <= srcLimit &&
                    U8_IS_TRAIL(src[srcIndex]) && U8_IS_TRAIL(src[srcIndex + 1])) {
                // most of CJK: no case mappings
                srcIndex += 2;
                ચાલુ;
            }
            cpStart = --srcIndex;
            U8_NEXT(src, srcIndex, srcLimit, c);
            જો (c < 0) {
                // ill-formed UTF-8
                ચાલુ;
            }
            uint16_t props = UTRIE2_GET16(trie, c);
            જો (UCASE_HAS_EXCEPTION(props)) { તોડો; }
            int32_t delta;
            જો (!UCASE_IS_UPPER_OR_TITLE(props) || (delta = UCASE_GET_DELTA(props)) == 0) {
                ચાલુ;
            }
            ByteSinkUtil::appendUnchanged(src + prev, cpStart - prev,
                                          sink, options, edits, errorCode);
            ByteSinkUtil::appendCodePoint(srcIndex - cpStart, c + delta, sink, edits);
            prev = srcIndex;
        }
        જો (c < 0) {
            તોડો;
        }
        // slow path
        અચલ char16_t *s;
        જો (caseLocale >= 0) {
            csc->cpStart = cpStart;
            csc->cpLimit = srcIndex;
            c = ucase_toFullLower(c, utf8_caseContextIterator, csc, &s, caseLocale);
        } નહીંતર {
            c = ucase_toFullFolding(c, &s, options);
        }
        જો (c >= 0) {
            ByteSinkUtil::appendUnchanged(src + prev, cpStart - prev,
                                          sink, options, edits, errorCode);
            appendResult(srcIndex - cpStart, c, s, sink, options, edits, errorCode);
            prev = srcIndex;
        }
    }
    ByteSinkUtil::appendUnchanged(src + prev, srcIndex - prev,
                                  sink, options, edits, errorCode);
}

શૂન્ય toUpper(int32_t caseLocale, uint32_t options,
             અચલ uint8_t *src, UCaseContext *csc, int32_t srcLength,
             icu::ByteSink &sink, icu::Edits *edits, UErrorCode &errorCode) {
    અચલ int8_t *latinToUpper;
    જો (caseLocale == UCASE_LOC_TURKISH) {
        latinToUpper = LatinCase::TO_UPPER_TR;
    } નહીંતર {
        latinToUpper = LatinCase::TO_UPPER_NORMAL;
    }
    અચલ UTrie2 *trie = ucase_getTrie();
    int32_t prev = 0;
    int32_t srcIndex = 0;
    માટે (;;) {
        // fast path for simple cases
        int32_t cpStart;
        UChar32 c;
        માટે (;;) {
            જો (U_FAILURE(errorCode) || srcIndex >= srcLength) {
                c = U_SENTINEL;
                તોડો;
            }
            uint8_t lead = src[srcIndex++];
            જો (lead <= 0x7f) {
                int8_t d = latinToUpper[lead];
                જો (d == LatinCase::EXC) {
                    cpStart = srcIndex - 1;
                    c = lead;
                    તોડો;
                }
                જો (d == 0) { ચાલુ; }
                ByteSinkUtil::appendUnchanged(src + prev, srcIndex - 1 - prev,
                                              sink, options, edits, errorCode);
                અક્ષર ascii = static_cast<અક્ષર>(lead + d);
                sink.Append(&ascii, 1);
                જો (edits != nullptr) {
                    edits->addReplace(1, 1);
                }
                prev = srcIndex;
                ચાલુ;
            } નહીંતર જો (lead < 0xe3) {
                uint8_t t;
                જો (0xc2 <= lead && lead <= 0xc5 && srcIndex < srcLength &&
                        (t = src[srcIndex] - 0x80) <= 0x3f) {
                    // U+0080..U+017F
                    ++srcIndex;
                    c = ((lead - 0xc0) << 6) | t;
                    int8_t d = latinToUpper[c];
                    જો (d == LatinCase::EXC) {
                        cpStart = srcIndex - 2;
                        તોડો;
                    }
                    જો (d == 0) { ચાલુ; }
                    ByteSinkUtil::appendUnchanged(src + prev, srcIndex - 2 - prev,
                                                  sink, options, edits, errorCode);
                    ByteSinkUtil::appendTwoBytes(c + d, sink);
                    જો (edits != nullptr) {
                        edits->addReplace(2, 2);
                    }
                    prev = srcIndex;
                    ચાલુ;
                }
            } નહીંતર જો ((lead <= 0xe9 || lead == 0xeb || lead == 0xec) &&
                    (srcIndex + 2) <= srcLength &&
                    U8_IS_TRAIL(src[srcIndex]) && U8_IS_TRAIL(src[srcIndex + 1])) {
                // most of CJK: no case mappings
                srcIndex += 2;
                ચાલુ;
            }
            cpStart = --srcIndex;
            U8_NEXT(src, srcIndex, srcLength, c);
            જો (c < 0) {
                // ill-formed UTF-8
                ચાલુ;
            }
            uint16_t props = UTRIE2_GET16(trie, c);
            જો (UCASE_HAS_EXCEPTION(props)) { તોડો; }
            int32_t delta;
            જો (UCASE_GET_TYPE(props) != UCASE_LOWER || (delta = UCASE_GET_DELTA(props)) == 0) {
                ચાલુ;
            }
            ByteSinkUtil::appendUnchanged(src + prev, cpStart - prev,
                                          sink, options, edits, errorCode);
            ByteSinkUtil::appendCodePoint(srcIndex - cpStart, c + delta, sink, edits);
            prev = srcIndex;
        }
        જો (c < 0) {
            તોડો;
        }
        // slow path
        csc->cpStart = cpStart;
        csc->cpLimit = srcIndex;
        અચલ char16_t *s;
        c = ucase_toFullUpper(c, utf8_caseContextIterator, csc, &s, caseLocale);
        જો (c >= 0) {
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

#જો !UCONFIG_NO_BREAK_ITERATION

નામાકાશ {

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
int32_t maybeTitleDutchIJ(અચલ uint8_t *src, UChar32 c, int32_t start, int32_t segmentLimit,
                          ByteSink &sink, uint32_t options, icu::Edits *edits, UErrorCode &errorCode) {
    U_ASSERT(start < segmentLimit);

    int32_t index = start;
    તાર્કિક withAcute = ખોટું;

    // If the conditions are met, then the following variables tell us what to output.
    int32_t unchanged1 = 0;  // code units before the j, or the whole sequence (0..3)
    તાર્કિક doTitleJ = ખોટું;  // true if the j needs to be titlecased
    int32_t unchanged2 = 0;  // after the j (0 or 1)

    // next character after the first letter
    UChar32 c2;
    c2 = src[index++];

    // Is the first letter an i/I with accent?
    જો (c == u'I') {
        જો (c2 == ACUTE_BYTE0 && index < segmentLimit && src[index++] == ACUTE_BYTE1) {
            withAcute = સાચું;
            unchanged1 = 2;  // ACUTE is 2 code units in UTF-8
            જો (index == segmentLimit) { પાછા start; }
            c2 = src[index++];
        }
    } નહીંતર {  // Í
        withAcute = સાચું;
    }

    // Is the next character a j/J?
    જો (c2 == u'j') {
        doTitleJ = સાચું;
    } નહીંતર જો (c2 == u'J') {
        ++unchanged1;
    } નહીંતર {
        પાછા start;
    }

    // A plain i/I must be followed by a plain j/J.
    // An i/I with acute must be followed by a j/J with acute.
    જો (withAcute) {
        જો ((index + 1) >= segmentLimit || src[index++] != ACUTE_BYTE0 || src[index++] != ACUTE_BYTE1) {
            પાછા start;
        }
        જો (doTitleJ) {
            unchanged2 = 2;  // ACUTE is 2 code units in UTF-8
        } નહીંતર {
            unchanged1 = unchanged1 + 2;    // ACUTE is 2 code units in UTF-8
        }
    }

    // There must not be another combining mark.
    જો (index < segmentLimit) {
        int32_t cp;
        int32_t i = index;
        U8_NEXT(src, i, segmentLimit, cp);
        uint32_t typeMask = U_GET_GC_MASK(cp);
        જો ((typeMask & U_GC_M_MASK) != 0) {
            પાછા start;
        }
    }

    // Output the rest of the Dutch IJ.
    ByteSinkUtil::appendUnchanged(src + start, unchanged1, sink, options, edits, errorCode);
    start += unchanged1;
    જો (doTitleJ) {
        ByteSinkUtil::appendCodePoint(1, u'J', sink, edits);
        ++start;
    }
    ByteSinkUtil::appendUnchanged(src + start, unchanged2, sink, options, edits, errorCode);

    U_ASSERT(start + unchanged2 == index);
    પાછા index;
}

}  // namespace

U_CFUNC શૂન્ય U_CALLCONV
ucasemap_internalUTF8ToTitle(
        int32_t caseLocale, uint32_t options, BreakIterator *iter,
        અચલ uint8_t *src, int32_t srcLength,
        ByteSink &sink, icu::Edits *edits,
        UErrorCode &errorCode) {
    જો (!ustrcase_checkTitleAdjustmentOptions(options, errorCode)) {
        પાછા;
    }

    /* set up local variables */
    UCaseContext csc=UCASECONTEXT_INITIALIZER;
    csc.p=(શૂન્ય *)src;
    csc.limit=srcLength;
    int32_t prev=0;
    UBool isFirstIndex=સાચું;

    /* titlecasing loop */
    જ્યારે(prev<srcLength) {
        /* find next index where to titlecase */
        int32_t index;
        જો(isFirstIndex) {
            isFirstIndex=ખોટું;
            index=iter->first();
        } નહીંતર {
            index=iter->next();
        }
        જો(index==UBRK_DONE || index>srcLength) {
            index=srcLength;
        }

        /*
         * Segment [prev..index[ into 3 parts:
         * a) skipped characters (copy as-is) [prev..titleStart[
         * b) first letter (titlecase)              [titleStart..titleLimit[
         * c) subsequent characters (lowercase)                 [titleLimit..index[
         */
        જો(prev<index) {
            /* find and copy skipped characters [prev..titleStart[ */
            int32_t titleStart=prev;
            int32_t titleLimit=prev;
            UChar32 c;
            U8_NEXT(src, titleLimit, index, c);
            જો ((options&U_TITLECASE_NO_BREAK_ADJUSTMENT)==0) {
                // Adjust the titlecasing index to the next cased character,
                // or to the next letter/number/symbol/private use.
                // Stop with titleStart<titleLimit<=index
                // if there is a character to be titlecased,
                // or else stop with titleStart==titleLimit==index.
                UBool toCased = (options&U_TITLECASE_ADJUST_TO_CASED) != 0;
                જ્યારે (toCased ? UCASE_NONE==ucase_getType(c) : !ustrcase_isLNS(c)) {
                    titleStart=titleLimit;
                    જો(titleLimit==index) {
                        તોડો;
                    }
                    U8_NEXT(src, titleLimit, index, c);
                }
                જો (prev < titleStart) {
                    જો (!ByteSinkUtil::appendUnchanged(src+prev, titleStart-prev,
                                                       sink, options, edits, errorCode)) {
                        પાછા;
                    }
                }
            }

            જો(titleStart<titleLimit) {
                /* titlecase c which is from [titleStart..titleLimit[ */
                જો(c>=0) {
                    csc.cpStart=titleStart;
                    csc.cpLimit=titleLimit;
                    અચલ char16_t *s;
                    c=ucase_toFullTitle(c, utf8_caseContextIterator, &csc, &s, caseLocale);
                    જો (!appendResult(titleLimit-titleStart, c, s, sink, options, edits, errorCode)) {
                        પાછા;
                    }
                } નહીંતર {
                    // Malformed UTF-8.
                    જો (!ByteSinkUtil::appendUnchanged(src+titleStart, titleLimit-titleStart,
                                                       sink, options, edits, errorCode)) {
                        પાછા;
                    }
                }

                /* Special case Dutch IJ titlecasing */
                જો (titleLimit < index &&
                    caseLocale == UCASE_LOC_DUTCH) {
                    જો (c < 0) {
                        c = ~c;
                    }

                    જો (c == u'I' || c == u'Í') {
                        titleLimit = maybeTitleDutchIJ(src, c, titleLimit, index, sink, options, edits, errorCode);
                    }
                }

                /* lowercase [titleLimit..index[ */
                જો(titleLimit<index) {
                    જો((options&U_TITLECASE_NO_LOWERCASE)==0) {
                        /* Normal operation: Lowercase the rest of the word. */
                        toLower(caseLocale, options,
                                src, &csc, titleLimit, index,
                                sink, edits, errorCode);
                        જો(U_FAILURE(errorCode)) {
                            પાછા;
                        }
                    } નહીંતર {
                        /* Optionally just copy the rest of the word unchanged. */
                        જો (!ByteSinkUtil::appendUnchanged(src+titleLimit, index-titleLimit,
                                                           sink, options, edits, errorCode)) {
                            પાછા;
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
નામાકાશ GreekUpper {

UBool isFollowedByCasedLetter(અચલ uint8_t *s, int32_t i, int32_t length) {
    જ્યારે (i < length) {
        UChar32 c;
        U8_NEXT(s, i, length, c);
        int32_t type = ucase_getTypeOrIgnorable(c);
        જો ((type & UCASE_IGNORABLE) != 0) {
            // Case-ignorable, continue with the loop.
        } નહીંતર જો (type != UCASE_NONE) {
            પાછા સાચું;  // Followed by cased letter.
        } નહીંતર {
            પાછા ખોટું;  // Uncased and not case-ignorable.
        }
    }
    પાછા ખોટું;  // Not followed by cased letter.
}

// Keep this consistent with the UTF-16 version in ustrcase.cpp and the Java version in CaseMap.java.
શૂન્ય toUpper(uint32_t options,
             અચલ uint8_t *src, int32_t srcLength,
             ByteSink &sink, Edits *edits,
             UErrorCode &errorCode) {
    uint32_t state = 0;
    માટે (int32_t i = 0; i < srcLength;) {
        int32_t nextIndex = i;
        UChar32 c;
        U8_NEXT(src, nextIndex, srcLength, c);
        uint32_t nextState = 0;
        int32_t type = ucase_getTypeOrIgnorable(c);
        જો ((type & UCASE_IGNORABLE) != 0) {
            // c is case-ignorable
            nextState |= (state & AFTER_CASED);
        } નહીંતર જો (type != UCASE_NONE) {
            // c is cased
            nextState |= AFTER_CASED;
        }
        uint32_t data = getLetterData(c);
        જો (data > 0) {
            uint32_t upper = data & UPPER_MASK;
            // Add a dialytika to this iota or ypsilon vowel
            // if we removed a tonos from the previous vowel,
            // and that previous vowel did not also have (or gain) a dialytika.
            // Adding one only to the final vowel in a longer sequence
            // (which does not occur in normal writing) would require lookahead.
            // Set the same flag as for preserving an existing dialytika.
            જો ((data & HAS_VOWEL) != 0 &&
                (state & (AFTER_VOWEL_WITH_PRECOMPOSED_ACCENT | AFTER_VOWEL_WITH_COMBINING_ACCENT)) !=
                    0 &&
                (upper == 0x399 || upper == 0x3A5)) {
                data |= (state & AFTER_VOWEL_WITH_PRECOMPOSED_ACCENT) != 0 ? HAS_DIALYTIKA
                                                                           : HAS_COMBINING_DIALYTIKA;
            }
            int32_t અંક_Ypogegrammeni = 0;  // Map each one to a trailing, spacing, capital iota.
            જો ((data & HAS_YPOGEGRAMMENI) != 0) {
                અંક_Ypogegrammeni = 1;
            }
            અચલ UBool hasPrecomposedAccent = (data & HAS_ACCENT) != 0;
            // Skip combining diacritics after this Greek letter.
            int32_t nextNextIndex = nextIndex;
            જ્યારે (nextIndex < srcLength) {
                UChar32 c2;
                U8_NEXT(src, nextNextIndex, srcLength, c2);
                uint32_t diacriticData = getDiacriticData(c2);
                જો (diacriticData != 0) {
                    data |= diacriticData;
                    જો ((diacriticData & HAS_YPOGEGRAMMENI) != 0) {
                        ++અંક_Ypogegrammeni;
                    }
                    nextIndex = nextNextIndex;
                } નહીંતર {
                    તોડો;  // not a Greek diacritic
                }
            }
            જો ((data & HAS_VOWEL_AND_ACCENT_AND_DIALYTIKA) == HAS_VOWEL_AND_ACCENT) {
                nextState |= hasPrecomposedAccent ? AFTER_VOWEL_WITH_PRECOMPOSED_ACCENT
                                                  : AFTER_VOWEL_WITH_COMBINING_ACCENT;
            }
            // Map according to Greek rules.
            UBool addTonos = ખોટું;
            જો (upper == 0x397 &&
                    (data & HAS_ACCENT) != 0 &&
                    અંક_Ypogegrammeni == 0 &&
                    (state & AFTER_CASED) == 0 &&
                    !isFollowedByCasedLetter(src, nextIndex, srcLength)) {
                // Keep disjunctive "or" with (only) a tonos.
                // We use the same "word boundary" conditions as for the Final_Sigma test.
                જો (hasPrecomposedAccent) {
                    upper = 0x389;  // Preserve the precomposed form.
                } નહીંતર {
                    addTonos = સાચું;
                }
            } નહીંતર જો ((data & HAS_DIALYTIKA) != 0) {
                // Preserve a vowel with dialytika in precomposed form if it exists.
                જો (upper == 0x399) {
                    upper = 0x3AA;
                    data &= ~HAS_EITHER_DIALYTIKA;
                } નહીંતર જો (upper == 0x3A5) {
                    upper = 0x3AB;
                    data &= ~HAS_EITHER_DIALYTIKA;
                }
            }

            UBool change;
            જો (edits == nullptr && (options & U_OMIT_UNCHANGED_TEXT) == 0) {
                change = સાચું;  // common, simple usage
            } નહીંતર {
                // Find out first whether we are changing the text.
                U_ASSERT(0x370 <= upper && upper <= 0x3ff);  // 2-byte UTF-8, main Greek block
                change = (i + 2) > nextIndex ||
                        src[i] != getTwoByteLead(upper) || src[i + 1] != getTwoByteTrail(upper) ||
                        અંક_Ypogegrammeni > 0;
                int32_t i2 = i + 2;
                જો ((data & HAS_EITHER_DIALYTIKA) != 0) {
                    change |= (i2 + 2) > nextIndex ||
                            src[i2] != static_cast<uint8_t>(u8"\u0308"[0]) ||
                            src[i2 + 1] != static_cast<uint8_t>(u8"\u0308"[1]);
                    i2 += 2;
                }
                જો (addTonos) {
                    change |= (i2 + 2) > nextIndex ||
                            src[i2] != static_cast<uint8_t>(u8"\u0301"[0]) ||
                            src[i2 + 1] != static_cast<uint8_t>(u8"\u0301"[1]);
                    i2 += 2;
                }
                int32_t oldLength = nextIndex - i;
                int32_t newLength = (i2 - i) + અંક_Ypogegrammeni * 2;  // 2 bytes per U+0399
                change |= oldLength != newLength;
                જો (change) {
                    જો (edits != nullptr) {
                        edits->addReplace(oldLength, newLength);
                    }
                } નહીંતર {
                    જો (edits != nullptr) {
                        edits->addUnchanged(oldLength);
                    }
                    // Write unchanged text?
                    change = (options & U_OMIT_UNCHANGED_TEXT) == 0;
                }
            }

            જો (change) {
                ByteSinkUtil::appendTwoBytes(upper, sink);
                જો ((data & HAS_EITHER_DIALYTIKA) != 0) {
                    sink.AppendU8(u8"\u0308", 2);  // restore or add a dialytika
                }
                જો (addTonos) {
                    sink.AppendU8(u8"\u0301", 2);
                }
                જ્યારે (અંક_Ypogegrammeni > 0) {
                    sink.AppendU8(u8"\u0399", 2);
                    --અંક_Ypogegrammeni;
                }
            }
        } નહીંતર જો(c>=0) {
            અચલ char16_t *s;
            c=ucase_toFullUpper(c, nullptr, nullptr, &s, UCASE_LOC_GREEK);
            જો (!appendResult(nextIndex - i, c, s, sink, options, edits, errorCode)) {
                પાછા;
            }
        } નહીંતર {
            // Malformed UTF-8.
            જો (!ByteSinkUtil::appendUnchanged(src+i, nextIndex-i,
                                               sink, options, edits, errorCode)) {
                પાછા;
            }
        }
        i = nextIndex;
        state = nextState;
    }
}

}  // namespace GreekUpper
U_NAMESPACE_END

સ્થિર શૂન્ય U_CALLCONV
ucasemap_internalUTF8ToLower(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_UNUSED
                             અચલ uint8_t *src, int32_t srcLength,
                             icu::ByteSink &sink, icu::Edits *edits,
                             UErrorCode &errorCode) {
    UCaseContext csc=UCASECONTEXT_INITIALIZER;
    csc.p=(શૂન્ય *)src;
    csc.limit=srcLength;
    toLower(
        caseLocale, options,
        src, &csc, 0, srcLength,
        sink, edits, errorCode);
}

સ્થિર શૂન્ય U_CALLCONV
ucasemap_internalUTF8ToUpper(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_UNUSED
                             અચલ uint8_t *src, int32_t srcLength,
                             icu::ByteSink &sink, icu::Edits *edits,
                             UErrorCode &errorCode) {
    જો (caseLocale == UCASE_LOC_GREEK) {
        GreekUpper::toUpper(options, src, srcLength, sink, edits, errorCode);
    } નહીંતર {
        UCaseContext csc=UCASECONTEXT_INITIALIZER;
        csc.p=(શૂન્ય *)src;
        csc.limit=srcLength;
        toUpper(
            caseLocale, options,
            src, &csc, srcLength,
            sink, edits, errorCode);
    }
}

સ્થિર શૂન્ય U_CALLCONV
ucasemap_internalUTF8Fold(int32_t /* caseLocale */, uint32_t options, UCASEMAP_BREAK_ITERATOR_UNUSED
                          અચલ uint8_t *src, int32_t srcLength,
                          icu::ByteSink &sink, icu::Edits *edits,
                          UErrorCode &errorCode) {
    toLower(
        -1, options,
        src, nullptr, 0, srcLength,
        sink, edits, errorCode);
}

શૂન્ય
ucasemap_mapUTF8(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_PARAM
                 અચલ અક્ષર *src, int32_t srcLength,
                 UTF8CaseMapper *stringCaseMapper,
                 icu::ByteSink &sink, icu::Edits *edits,
                 UErrorCode &errorCode) {
    /* check argument values */
    જો (U_FAILURE(errorCode)) {
        પાછા;
    }
    જો ((src == nullptr && srcLength != 0) || srcLength < -1) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        પાછા;
    }

    // Get the string length.
    જો (srcLength == -1) {
        srcLength = static_cast<int32_t>(uprv_strlen(src));
    }

    જો (edits != nullptr && (options & U_EDITS_NO_RESET) == 0) {
        edits->reset();
    }
    stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                     reinterpret_cast<અચલ uint8_t*>(src), srcLength, sink, edits, errorCode);
    sink.Flush();
    જો (U_SUCCESS(errorCode)) {
        જો (edits != nullptr) {
            edits->copyErrorTo(errorCode);
        }
    }
}

int32_t
ucasemap_mapUTF8(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_PARAM
                 અક્ષર *dest, int32_t destCapacity,
                 અચલ અક્ષર *src, int32_t srcLength,
                 UTF8CaseMapper *stringCaseMapper,
                 icu::Edits *edits,
                 UErrorCode &errorCode) {
    /* check argument values */
    જો(U_FAILURE(errorCode)) {
        પાછા 0;
    }
    જો( destCapacity<0 ||
        (dest==nullptr && destCapacity>0) ||
        (src==nullptr && srcLength!=0) || srcLength<-1
    ) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        પાછા 0;
    }

    /* get the string length */
    જો(srcLength==-1) {
        srcLength = static_cast<int32_t>(uprv_strlen(src));
    }

    /* check for overlapping source and destination */
    જો( dest!=nullptr &&
        ((src>=dest && src<(dest+destCapacity)) ||
         (dest>=src && dest<(src+srcLength)))
    ) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        પાછા 0;
    }

    જો (edits != nullptr && (options & U_EDITS_NO_RESET) == 0) {
        edits->reset();
    }
    int32_t reslen = ByteSinkUtil::viaByteSinkToTerminatedChars(
        dest, destCapacity,
        [&](ByteSink& sink, UErrorCode& status) {
            stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                             reinterpret_cast<અચલ uint8_t*>(src), srcLength, sink, edits, status);
        },
        errorCode);
    જો (U_SUCCESS(errorCode) && edits != nullptr) {
        edits->copyErrorTo(errorCode);
    }
    પાછા reslen;
}

/* public API functions */

U_CAPI int32_t U_EXPORT2
ucasemap_utf8ToLower(અચલ UCaseMap *csm,
                     અક્ષર *dest, int32_t destCapacity,
                     અચલ અક્ષર *src, int32_t srcLength,
                     UErrorCode *pErrorCode) {
    auto *impl = reinterpret_cast<અચલ UCaseMapImpl *>(csm);
    પાછા ucasemap_mapUTF8(
        impl->caseLocale, impl->options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, destCapacity,
        src, srcLength,
        ucasemap_internalUTF8ToLower, nullptr, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
ucasemap_utf8ToUpper(અચલ UCaseMap *csm,
                     અક્ષર *dest, int32_t destCapacity,
                     અચલ અક્ષર *src, int32_t srcLength,
                     UErrorCode *pErrorCode) {
    auto *impl = reinterpret_cast<અચલ UCaseMapImpl *>(csm);
    પાછા ucasemap_mapUTF8(
        impl->caseLocale, impl->options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, destCapacity,
        src, srcLength,
        ucasemap_internalUTF8ToUpper, nullptr, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
ucasemap_utf8FoldCase(અચલ UCaseMap *csm,
                      અક્ષર *dest, int32_t destCapacity,
                      અચલ અક્ષર *src, int32_t srcLength,
                      UErrorCode *pErrorCode) {
    auto *impl = reinterpret_cast<અચલ UCaseMapImpl *>(csm);
    પાછા ucasemap_mapUTF8(
        UCASE_LOC_મૂળ, impl->options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, destCapacity,
        src, srcLength,
        ucasemap_internalUTF8Fold, nullptr, *pErrorCode);
}

U_NAMESPACE_BEGIN

શૂન્ય CaseMap::utf8ToLower(
        અચલ અક્ષર *locale, uint32_t options,
        StringPiece src, ByteSink &sink, Edits *edits,
        UErrorCode &errorCode) {
    ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        src.data(), src.length(),
        ucasemap_internalUTF8ToLower, sink, edits, errorCode);
}

શૂન્ય CaseMap::utf8ToUpper(
        અચલ અક્ષર *locale, uint32_t options,
        StringPiece src, ByteSink &sink, Edits *edits,
        UErrorCode &errorCode) {
    ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        src.data(), src.length(),
        ucasemap_internalUTF8ToUpper, sink, edits, errorCode);
}

શૂન્ય CaseMap::utf8Fold(
        uint32_t options,
        StringPiece src, ByteSink &sink, Edits *edits,
        UErrorCode &errorCode) {
    ucasemap_mapUTF8(
        UCASE_LOC_મૂળ, options, UCASEMAP_BREAK_ITERATOR_NULL
        src.data(), src.length(),
        ucasemap_internalUTF8Fold, sink, edits, errorCode);
}

int32_t CaseMap::utf8ToLower(
        અચલ અક્ષર *locale, uint32_t options,
        અચલ અક્ષર *src, int32_t srcLength,
        અક્ષર *dest, int32_t destCapacity, Edits *edits,
        UErrorCode &errorCode) {
    પાછા ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, destCapacity,
        src, srcLength,
        ucasemap_internalUTF8ToLower, edits, errorCode);
}

int32_t CaseMap::utf8ToUpper(
        અચલ અક્ષર *locale, uint32_t options,
        અચલ અક્ષર *src, int32_t srcLength,
        અક્ષર *dest, int32_t destCapacity, Edits *edits,
        UErrorCode &errorCode) {
    પાછા ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, destCapacity,
        src, srcLength,
        ucasemap_internalUTF8ToUpper, edits, errorCode);
}

int32_t CaseMap::utf8Fold(
        uint32_t options,
        અચલ અક્ષર *src, int32_t srcLength,
        અક્ષર *dest, int32_t destCapacity, Edits *edits,
        UErrorCode &errorCode) {
    પાછા ucasemap_mapUTF8(
        UCASE_LOC_મૂળ, options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, destCapacity,
        src, srcLength,
        ucasemap_internalUTF8Fold, edits, errorCode);
}

U_NAMESPACE_END
