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

#समावेश "unicode/utypes.h"
#समावेश "unicode/brkiter.h"
#समावेश "unicode/bytestream.h"
#समावेश "unicode/casemap.h"
#समावेश "unicode/edits.h"
#समावेश "unicode/stringoptions.h"
#समावेश "unicode/stringpiece.h"
#समावेश "unicode/ubrk.h"
#समावेश "unicode/uloc.h"
#समावेश "unicode/ustring.h"
#समावेश "unicode/ucasemap.h"
#अगर !UCONFIG_NO_BREAK_ITERATION
#समावेश "unicode/utext.h"
#पूर्ण_अगर
#समावेश "unicode/utf.h"
#समावेश "unicode/utf8.h"
#समावेश "unicode/utf16.h"
#समावेश "bytesinkutil.h"
#समावेश "cmemory.h"
#समावेश "cstring.h"
#समावेश "uassert.h"
#समावेश "ucase.h"
#समावेश "ucasemap_imp.h"

U_NAMESPACE_USE

/* UCaseMap service object -------------------------------------------------- */

// The public type, unicode/ucasemap.h struct UCaseMap, is an opaque C type.
// We define a parallel C++ type in the ICU namespace for entry point renaming.

U_NAMESPACE_BEGIN

UCaseMapImpl::UCaseMapImpl(स्थिर अक्षर *localeID, uint32_t opts, UErrorCode *pErrorCode) :
#अगर !UCONFIG_NO_BREAK_ITERATION
        iter(nullptr),
#पूर्ण_अगर
        caseLocale(UCASE_LOC_UNKNOWN), options(opts) {
    ucasemap_setLocale(reinterpret_cast<UCaseMap *>(यह), localeID, pErrorCode);
}

UCaseMapImpl::~UCaseMapImpl() {
#अगर !UCONFIG_NO_BREAK_ITERATION
    मिटाओ iter;
#पूर्ण_अगर
}

U_NAMESPACE_END

U_CAPI UCaseMap * U_EXPORT2
ucasemap_open(स्थिर अक्षर *locale, uint32_t options, UErrorCode *pErrorCode) {
    अगर(U_FAILURE(*pErrorCode)) {
        वापस nullptr;
    }
    UCaseMapImpl *csm = नया UCaseMapImpl(locale, options, pErrorCode);
    अगर(csm==nullptr) {
        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
        वापस nullptr;
    } अन्यथा अगर (U_FAILURE(*pErrorCode)) {
        मिटाओ csm;
        वापस nullptr;
    }
    वापस reinterpret_cast<UCaseMap *>(csm);
}

U_CAPI व्योम U_EXPORT2
ucasemap_close(UCaseMap *csm) {
    मिटाओ reinterpret_cast<UCaseMapImpl *>(csm);
}

U_CAPI स्थिर अक्षर* U_EXPORT2
ucasemap_getLocale(स्थिर UCaseMap* csm U_LIFETIME_BOUND) {
    वापस reinterpret_cast<स्थिर UCaseMapImpl *>(csm)->locale;
}

U_CAPI uint32_t U_EXPORT2
ucasemap_getOptions(स्थिर UCaseMap *csm) {
    वापस reinterpret_cast<स्थिर UCaseMapImpl *>(csm)->options;
}

U_CAPI व्योम U_EXPORT2
ucasemap_setLocale(UCaseMap *csm, स्थिर अक्षर *locale, UErrorCode *pErrorCode) {
    अगर(U_FAILURE(*pErrorCode)) {
        वापस;
    }
    स्वतः *impl = reinterpret_cast<UCaseMapImpl *>(csm);
    अगर (locale != nullptr && *locale == 0) {
        impl->locale[0] = 0;
        impl->caseLocale = UCASE_LOC_मूल;
        वापस;
    }

    UErrorCode बफर_Status = U_ZERO_ERROR;
    int32_t length=uloc_getName(locale, impl->locale, (int32_t)माप(impl->locale), &बफर_Status);
    अगर(बफर_Status==U_बफर_OVERFLOW_ERROR || (U_SUCCESS(बफर_Status) && length==माप(impl->locale))) {
        बफर_Status = U_ZERO_ERROR;
        /* we only really need the language code for case mappings */
        length=uloc_getLanguage(locale, impl->locale, (int32_t)माप(impl->locale), &बफर_Status);
    }
    अगर(U_FAILURE(बफर_Status)) {
        *pErrorCode=बफर_Status;
    } अन्यथा अगर(length==माप(impl->locale)) {
        *pErrorCode=U_बफर_OVERFLOW_ERROR;
    }
    अगर(U_SUCCESS(*pErrorCode)) {     
        impl->caseLocale = ucase_getCaseLocale(impl->locale);
    } अन्यथा {
        impl->locale[0]=0;
        impl->caseLocale = UCASE_LOC_मूल;
    }
}

U_CAPI व्योम U_EXPORT2
ucasemap_setOptions(UCaseMap *csm, uint32_t options, UErrorCode *pErrorCode) {
    अगर(U_FAILURE(*pErrorCode)) {
        वापस;
    }
    reinterpret_cast<UCaseMapImpl *>(csm)->options=options;
}

/* UTF-8 string case mappings ----------------------------------------------- */

/* TODO(markus): Move to a new, separate utf8case.cpp file. */

namespace {

/* append a full case mapping result, see UCASE_MAX_STRING_LENGTH */
अंतरभूत UBool
append_परिणाम(int32_t cp_लंबाई, int32_t परिणाम, स्थिर char16_t *s,
             ByteSink &sink, uint32_t options, icu::Edits *edits, UErrorCode &errorCode) {
    U_ASSERT(U_SUCCESS(errorCode));

    /* decode the result */
    अगर(परिणाम<0) {
        /* (not) original code point */
        अगर(edits!=nullptr) {
            edits->addUnchanged(cp_लंबाई);
        }
        अगर((options & U_OMIT_UNCHANGED_TEXT) == 0) {
            ByteSinkUtil::appendCodePoint(cp_लंबाई, ~परिणाम, sink);
        }
    } अन्यथा {
        अगर(परिणाम<=UCASE_MAX_STRING_लंबाई) {
            // string: "result" is the UTF-16 length
            वापस ByteSinkUtil::appendChange(cp_लंबाई, s, परिणाम, sink, edits, errorCode);
        } अन्यथा {
            ByteSinkUtil::appendCodePoint(cp_लंबाई, परिणाम, sink, edits);
        }
    }
    वापस true;
}

// See unicode/utf8.h U8_APPEND_UNSAFE().
अंतरभूत uint8_t getTwoByteLead(UChar32 c) { वापस static_cast<uint8_t>((c >> 6) | 0xc0); }
अंतरभूत uint8_t getTwoByteTrail(UChar32 c) { वापस static_cast<uint8_t>((c & 0x3f) | 0x80); }

UChar32 U_CALLCONV
utf8_caseContextIterator(व्योम *context, int8_t dir) {
    UCaseContext* csc = static_cast<UCaseContext*>(context);
    UChar32 c;

    अगर(dir<0) {
        /* reset for backward iteration */
        csc->सूचकांक=csc->cpStart;
        csc->dir=dir;
    } अन्यथा अगर(dir>0) {
        /* reset for forward iteration */
        csc->सूचकांक=csc->cpLimit;
        csc->dir=dir;
    } अन्यथा {
        /* continue current iteration direction */
        dir=csc->dir;
    }

    अगर(dir<0) {
        अगर(csc->start<csc->सूचकांक) {
            U8_पूर्व((स्थिर uint8_t *)csc->p, csc->start, csc->सूचकांक, c);
            वापस c;
        }
    } अन्यथा {
        अगर(csc->सूचकांक<csc->limit) {
            U8_अग्र((स्थिर uint8_t *)csc->p, csc->सूचकांक, csc->limit, c);
            वापस c;
        }
    }
    वापस U_SENTINEL;
}

/**
 * caseLocale >= 0: Lowercases [srcStart..srcLimit[ but takes context [0..srcLength[ into account.
 * caseLocale < 0: Case-folds [srcStart..srcLimit[.
 */
व्योम toLower(int32_t caseLocale, uint32_t options,
             स्थिर uint8_t *src, UCaseContext *csc, int32_t srcStart, int32_t srcLimit,
             icu::ByteSink &sink, icu::Edits *edits, UErrorCode &errorCode) {
    स्थिर int8_t *latinToLower;
    अगर (caseLocale == UCASE_LOC_मूल ||
            (caseLocale >= 0 ?
                !(caseLocale == UCASE_LOC_TURKISH || caseLocale == UCASE_LOC_LITHUANIAN) :
                (options & _FOLD_CASE_OPTIONS_MASK) == U_FOLD_CASE_DEFAULT)) {
        latinToLower = LatinCase::TO_LOWER_NORMAL;
    } अन्यथा {
        latinToLower = LatinCase::TO_LOWER_TR_LT;
    }
    स्थिर UTrie2 *trie = ucase_getTrie();
    int32_t पूर्व = srcStart;
    int32_t src_सूचकांक = srcStart;
    क्रम (;;) {
        // fast path for simple cases
        int32_t cpStart;
        UChar32 c;
        क्रम (;;) {
            अगर (U_FAILURE(errorCode) || src_सूचकांक >= srcLimit) {
                c = U_SENTINEL;
                अवरोध;
            }
            uint8_t lead = src[src_सूचकांक++];
            अगर (lead <= 0x7f) {
                int8_t d = latinToLower[lead];
                अगर (d == LatinCase::EXC) {
                    cpStart = src_सूचकांक - 1;
                    c = lead;
                    अवरोध;
                }
                अगर (d == 0) { जारी; }
                ByteSinkUtil::appendUnchanged(src + पूर्व, src_सूचकांक - 1 - पूर्व,
                                              sink, options, edits, errorCode);
                अक्षर ascii = static_cast<अक्षर>(lead + d);
                sink.Append(&ascii, 1);
                अगर (edits != nullptr) {
                    edits->addReplace(1, 1);
                }
                पूर्व = src_सूचकांक;
                जारी;
            } अन्यथा अगर (lead < 0xe3) {
                uint8_t t;
                अगर (0xc2 <= lead && lead <= 0xc5 && src_सूचकांक < srcLimit &&
                        (t = src[src_सूचकांक] - 0x80) <= 0x3f) {
                    // U+0080..U+017F
                    ++src_सूचकांक;
                    c = ((lead - 0xc0) << 6) | t;
                    int8_t d = latinToLower[c];
                    अगर (d == LatinCase::EXC) {
                        cpStart = src_सूचकांक - 2;
                        अवरोध;
                    }
                    अगर (d == 0) { जारी; }
                    ByteSinkUtil::appendUnchanged(src + पूर्व, src_सूचकांक - 2 - पूर्व,
                                                  sink, options, edits, errorCode);
                    ByteSinkUtil::appendTwoBytes(c + d, sink);
                    अगर (edits != nullptr) {
                        edits->addReplace(2, 2);
                    }
                    पूर्व = src_सूचकांक;
                    जारी;
                }
            } अन्यथा अगर ((lead <= 0xe9 || lead == 0xeb || lead == 0xec) &&
                    (src_सूचकांक + 2) <= srcLimit &&
                    U8_IS_TRAIL(src[src_सूचकांक]) && U8_IS_TRAIL(src[src_सूचकांक + 1])) {
                // most of CJK: no case mappings
                src_सूचकांक += 2;
                जारी;
            }
            cpStart = --src_सूचकांक;
            U8_अग्र(src, src_सूचकांक, srcLimit, c);
            अगर (c < 0) {
                // ill-formed UTF-8
                जारी;
            }
            uint16_t props = UTRIE2_GET16(trie, c);
            अगर (UCASE_HAS_EXCEPTION(props)) { अवरोध; }
            int32_t delta;
            अगर (!UCASE_IS_UPPER_OR_TITLE(props) || (delta = UCASE_GET_DELTA(props)) == 0) {
                जारी;
            }
            ByteSinkUtil::appendUnchanged(src + पूर्व, cpStart - पूर्व,
                                          sink, options, edits, errorCode);
            ByteSinkUtil::appendCodePoint(src_सूचकांक - cpStart, c + delta, sink, edits);
            पूर्व = src_सूचकांक;
        }
        अगर (c < 0) {
            अवरोध;
        }
        // slow path
        स्थिर char16_t *s;
        अगर (caseLocale >= 0) {
            csc->cpStart = cpStart;
            csc->cpLimit = src_सूचकांक;
            c = ucase_toFullLower(c, utf8_caseContextIterator, csc, &s, caseLocale);
        } अन्यथा {
            c = ucase_toFullFolding(c, &s, options);
        }
        अगर (c >= 0) {
            ByteSinkUtil::appendUnchanged(src + पूर्व, cpStart - पूर्व,
                                          sink, options, edits, errorCode);
            append_परिणाम(src_सूचकांक - cpStart, c, s, sink, options, edits, errorCode);
            पूर्व = src_सूचकांक;
        }
    }
    ByteSinkUtil::appendUnchanged(src + पूर्व, src_सूचकांक - पूर्व,
                                  sink, options, edits, errorCode);
}

व्योम toUpper(int32_t caseLocale, uint32_t options,
             स्थिर uint8_t *src, UCaseContext *csc, int32_t src_लंबाई,
             icu::ByteSink &sink, icu::Edits *edits, UErrorCode &errorCode) {
    स्थिर int8_t *latinToUpper;
    अगर (caseLocale == UCASE_LOC_TURKISH) {
        latinToUpper = LatinCase::TO_UPPER_TR;
    } अन्यथा {
        latinToUpper = LatinCase::TO_UPPER_NORMAL;
    }
    स्थिर UTrie2 *trie = ucase_getTrie();
    int32_t पूर्व = 0;
    int32_t src_सूचकांक = 0;
    क्रम (;;) {
        // fast path for simple cases
        int32_t cpStart;
        UChar32 c;
        क्रम (;;) {
            अगर (U_FAILURE(errorCode) || src_सूचकांक >= src_लंबाई) {
                c = U_SENTINEL;
                अवरोध;
            }
            uint8_t lead = src[src_सूचकांक++];
            अगर (lead <= 0x7f) {
                int8_t d = latinToUpper[lead];
                अगर (d == LatinCase::EXC) {
                    cpStart = src_सूचकांक - 1;
                    c = lead;
                    अवरोध;
                }
                अगर (d == 0) { जारी; }
                ByteSinkUtil::appendUnchanged(src + पूर्व, src_सूचकांक - 1 - पूर्व,
                                              sink, options, edits, errorCode);
                अक्षर ascii = static_cast<अक्षर>(lead + d);
                sink.Append(&ascii, 1);
                अगर (edits != nullptr) {
                    edits->addReplace(1, 1);
                }
                पूर्व = src_सूचकांक;
                जारी;
            } अन्यथा अगर (lead < 0xe3) {
                uint8_t t;
                अगर (0xc2 <= lead && lead <= 0xc5 && src_सूचकांक < src_लंबाई &&
                        (t = src[src_सूचकांक] - 0x80) <= 0x3f) {
                    // U+0080..U+017F
                    ++src_सूचकांक;
                    c = ((lead - 0xc0) << 6) | t;
                    int8_t d = latinToUpper[c];
                    अगर (d == LatinCase::EXC) {
                        cpStart = src_सूचकांक - 2;
                        अवरोध;
                    }
                    अगर (d == 0) { जारी; }
                    ByteSinkUtil::appendUnchanged(src + पूर्व, src_सूचकांक - 2 - पूर्व,
                                                  sink, options, edits, errorCode);
                    ByteSinkUtil::appendTwoBytes(c + d, sink);
                    अगर (edits != nullptr) {
                        edits->addReplace(2, 2);
                    }
                    पूर्व = src_सूचकांक;
                    जारी;
                }
            } अन्यथा अगर ((lead <= 0xe9 || lead == 0xeb || lead == 0xec) &&
                    (src_सूचकांक + 2) <= src_लंबाई &&
                    U8_IS_TRAIL(src[src_सूचकांक]) && U8_IS_TRAIL(src[src_सूचकांक + 1])) {
                // most of CJK: no case mappings
                src_सूचकांक += 2;
                जारी;
            }
            cpStart = --src_सूचकांक;
            U8_अग्र(src, src_सूचकांक, src_लंबाई, c);
            अगर (c < 0) {
                // ill-formed UTF-8
                जारी;
            }
            uint16_t props = UTRIE2_GET16(trie, c);
            अगर (UCASE_HAS_EXCEPTION(props)) { अवरोध; }
            int32_t delta;
            अगर (UCASE_GET_TYPE(props) != UCASE_LOWER || (delta = UCASE_GET_DELTA(props)) == 0) {
                जारी;
            }
            ByteSinkUtil::appendUnchanged(src + पूर्व, cpStart - पूर्व,
                                          sink, options, edits, errorCode);
            ByteSinkUtil::appendCodePoint(src_सूचकांक - cpStart, c + delta, sink, edits);
            पूर्व = src_सूचकांक;
        }
        अगर (c < 0) {
            अवरोध;
        }
        // slow path
        csc->cpStart = cpStart;
        csc->cpLimit = src_सूचकांक;
        स्थिर char16_t *s;
        c = ucase_toFullUpper(c, utf8_caseContextIterator, csc, &s, caseLocale);
        अगर (c >= 0) {
            ByteSinkUtil::appendUnchanged(src + पूर्व, cpStart - पूर्व,
                                          sink, options, edits, errorCode);
            append_परिणाम(src_सूचकांक - cpStart, c, s, sink, options, edits, errorCode);
            पूर्व = src_सूचकांक;
        }
    }
    ByteSinkUtil::appendUnchanged(src + पूर्व, src_सूचकांक - पूर्व,
                                  sink, options, edits, errorCode);
}

}  // namespace

#अगर !UCONFIG_NO_BREAK_ITERATION

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
int32_t maybeTitleDutchIJ(स्थिर uint8_t *src, UChar32 c, int32_t start, int32_t segmentLimit,
                          ByteSink &sink, uint32_t options, icu::Edits *edits, UErrorCode &errorCode) {
    U_ASSERT(start < segmentLimit);

    int32_t सूचकांक = start;
    bool withAcute = false;

    // If the conditions are met, then the following variables tell us what to output.
    int32_t unchanged1 = 0;  // code units before the j, or the whole sequence (0..3)
    bool doTitleJ = false;  // true if the j needs to be titlecased
    int32_t unchanged2 = 0;  // after the j (0 or 1)

    // next character after the first letter
    UChar32 c2;
    c2 = src[सूचकांक++];

    // Is the first letter an i/I with accent?
    अगर (c == u'I') {
        अगर (c2 == ACUTE_BYTE0 && सूचकांक < segmentLimit && src[सूचकांक++] == ACUTE_BYTE1) {
            withAcute = true;
            unchanged1 = 2;  // ACUTE is 2 code units in UTF-8
            अगर (सूचकांक == segmentLimit) { वापस start; }
            c2 = src[सूचकांक++];
        }
    } अन्यथा {  // Í
        withAcute = true;
    }

    // Is the next character a j/J?
    अगर (c2 == u'j') {
        doTitleJ = true;
    } अन्यथा अगर (c2 == u'J') {
        ++unchanged1;
    } अन्यथा {
        वापस start;
    }

    // A plain i/I must be followed by a plain j/J.
    // An i/I with acute must be followed by a j/J with acute.
    अगर (withAcute) {
        अगर ((सूचकांक + 1) >= segmentLimit || src[सूचकांक++] != ACUTE_BYTE0 || src[सूचकांक++] != ACUTE_BYTE1) {
            वापस start;
        }
        अगर (doTitleJ) {
            unchanged2 = 2;  // ACUTE is 2 code units in UTF-8
        } अन्यथा {
            unchanged1 = unchanged1 + 2;    // ACUTE is 2 code units in UTF-8
        }
    }

    // There must not be another combining mark.
    अगर (सूचकांक < segmentLimit) {
        int32_t cp;
        int32_t i = सूचकांक;
        U8_अग्र(src, i, segmentLimit, cp);
        uint32_t typeMask = U_GET_GC_MASK(cp);
        अगर ((typeMask & U_GC_M_MASK) != 0) {
            वापस start;
        }
    }

    // Output the rest of the Dutch IJ.
    ByteSinkUtil::appendUnchanged(src + start, unchanged1, sink, options, edits, errorCode);
    start += unchanged1;
    अगर (doTitleJ) {
        ByteSinkUtil::appendCodePoint(1, u'J', sink, edits);
        ++start;
    }
    ByteSinkUtil::appendUnchanged(src + start, unchanged2, sink, options, edits, errorCode);

    U_ASSERT(start + unchanged2 == सूचकांक);
    वापस सूचकांक;
}

}  // namespace

U_CFUNC व्योम U_CALLCONV
ucasemap_internalUTF8ToTitle(
        int32_t caseLocale, uint32_t options, BreakIterator *iter,
        स्थिर uint8_t *src, int32_t src_लंबाई,
        ByteSink &sink, icu::Edits *edits,
        UErrorCode &errorCode) {
    अगर (!ustrcase_checkTitleAdjustmentOptions(options, errorCode)) {
        वापस;
    }

    /* set up local variables */
    UCaseContext csc=UCASECONTEXT_INITIALIZER;
    csc.p=(व्योम *)src;
    csc.limit=src_लंबाई;
    int32_t पूर्व=0;
    UBool is_First_सूचकांक=true;

    /* titlecasing loop */
    जबतक(पूर्व<src_लंबाई) {
        /* find next index where to titlecase */
        int32_t सूचकांक;
        अगर(is_First_सूचकांक) {
            is_First_सूचकांक=false;
            सूचकांक=iter->first();
        } अन्यथा {
            सूचकांक=iter->next();
        }
        अगर(सूचकांक==UBRK_DONE || सूचकांक>src_लंबाई) {
            सूचकांक=src_लंबाई;
        }

        /*
         * Segment [prev..index[ into 3 parts:
         * a) skipped characters (copy as-is) [prev..titleStart[
         * b) first letter (titlecase)              [titleStart..titleLimit[
         * c) subsequent characters (lowercase)                 [titleLimit..index[
         */
        अगर(पूर्व<सूचकांक) {
            /* find and copy skipped characters [prev..titleStart[ */
            int32_t titleStart=पूर्व;
            int32_t titleLimit=पूर्व;
            UChar32 c;
            U8_अग्र(src, titleLimit, सूचकांक, c);
            अगर ((options&U_TITLECASE_NO_BREAK_ADJUSTMENT)==0) {
                // Adjust the titlecasing index to the next cased character,
                // or to the next letter/number/symbol/private use.
                // Stop with titleStart<titleLimit<=index
                // if there is a character to be titlecased,
                // or else stop with titleStart==titleLimit==index.
                UBool toCased = (options&U_TITLECASE_ADJUST_TO_CASED) != 0;
                जबतक (toCased ? UCASE_NONE==ucase_getType(c) : !ustrcase_isLNS(c)) {
                    titleStart=titleLimit;
                    अगर(titleLimit==सूचकांक) {
                        अवरोध;
                    }
                    U8_अग्र(src, titleLimit, सूचकांक, c);
                }
                अगर (पूर्व < titleStart) {
                    अगर (!ByteSinkUtil::appendUnchanged(src+पूर्व, titleStart-पूर्व,
                                                       sink, options, edits, errorCode)) {
                        वापस;
                    }
                }
            }

            अगर(titleStart<titleLimit) {
                /* titlecase c which is from [titleStart..titleLimit[ */
                अगर(c>=0) {
                    csc.cpStart=titleStart;
                    csc.cpLimit=titleLimit;
                    स्थिर char16_t *s;
                    c=ucase_toFullTitle(c, utf8_caseContextIterator, &csc, &s, caseLocale);
                    अगर (!append_परिणाम(titleLimit-titleStart, c, s, sink, options, edits, errorCode)) {
                        वापस;
                    }
                } अन्यथा {
                    // Malformed UTF-8.
                    अगर (!ByteSinkUtil::appendUnchanged(src+titleStart, titleLimit-titleStart,
                                                       sink, options, edits, errorCode)) {
                        वापस;
                    }
                }

                /* Special case Dutch IJ titlecasing */
                अगर (titleLimit < सूचकांक &&
                    caseLocale == UCASE_LOC_DUTCH) {
                    अगर (c < 0) {
                        c = ~c;
                    }

                    अगर (c == u'I' || c == u'Í') {
                        titleLimit = maybeTitleDutchIJ(src, c, titleLimit, सूचकांक, sink, options, edits, errorCode);
                    }
                }

                /* lowercase [titleLimit..index[ */
                अगर(titleLimit<सूचकांक) {
                    अगर((options&U_TITLECASE_NO_LOWERCASE)==0) {
                        /* Normal operation: Lowercase the rest of the word. */
                        toLower(caseLocale, options,
                                src, &csc, titleLimit, सूचकांक,
                                sink, edits, errorCode);
                        अगर(U_FAILURE(errorCode)) {
                            वापस;
                        }
                    } अन्यथा {
                        /* Optionally just copy the rest of the word unchanged. */
                        अगर (!ByteSinkUtil::appendUnchanged(src+titleLimit, सूचकांक-titleLimit,
                                                           sink, options, edits, errorCode)) {
                            वापस;
                        }
                    }
                }
            }
        }

        पूर्व=सूचकांक;
    }
}

#पूर्ण_अगर

U_NAMESPACE_BEGIN
namespace GreekUpper {

UBool isFollowedByCasedLetter(स्थिर uint8_t *s, int32_t i, int32_t length) {
    जबतक (i < length) {
        UChar32 c;
        U8_अग्र(s, i, length, c);
        int32_t type = ucase_getTypeOrIgnorable(c);
        अगर ((type & UCASE_IGNORABLE) != 0) {
            // Case-ignorable, continue with the loop.
        } अन्यथा अगर (type != UCASE_NONE) {
            वापस true;  // Followed by cased letter.
        } अन्यथा {
            वापस false;  // Uncased and not case-ignorable.
        }
    }
    वापस false;  // Not followed by cased letter.
}

// Keep this consistent with the UTF-16 version in ustrcase.cpp and the Java version in CaseMap.java.
व्योम toUpper(uint32_t options,
             स्थिर uint8_t *src, int32_t src_लंबाई,
             ByteSink &sink, Edits *edits,
             UErrorCode &errorCode) {
    uint32_t state = 0;
    क्रम (int32_t i = 0; i < src_लंबाई;) {
        int32_t अग्र_सूचकांक = i;
        UChar32 c;
        U8_अग्र(src, अग्र_सूचकांक, src_लंबाई, c);
        uint32_t अग्र_State = 0;
        int32_t type = ucase_getTypeOrIgnorable(c);
        अगर ((type & UCASE_IGNORABLE) != 0) {
            // c is case-ignorable
            अग्र_State |= (state & AFTER_CASED);
        } अन्यथा अगर (type != UCASE_NONE) {
            // c is cased
            अग्र_State |= AFTER_CASED;
        }
        uint32_t तथ्य = get_Letter_तथ्य(c);
        अगर (तथ्य > 0) {
            uint32_t upper = तथ्य & UPPER_MASK;
            // Add a dialytika to this iota or ypsilon vowel
            // if we removed a tonos from the previous vowel,
            // and that previous vowel did not also have (or gain) a dialytika.
            // Adding one only to the final vowel in a longer sequence
            // (which does not occur in normal writing) would require lookahead.
            // Set the same flag as for preserving an existing dialytika.
            अगर ((तथ्य & HAS_VOWEL) != 0 &&
                (state & (AFTER_VOWEL_WITH_PRECOMPOSED_ACCENT | AFTER_VOWEL_WITH_COMBINING_ACCENT)) !=
                    0 &&
                (upper == 0x399 || upper == 0x3A5)) {
                तथ्य |= (state & AFTER_VOWEL_WITH_PRECOMPOSED_ACCENT) != 0 ? HAS_DIALYTIKA
                                                                           : HAS_COMBINING_DIALYTIKA;
            }
            int32_t अंक_Ypogegrammeni = 0;  // Map each one to a trailing, spacing, capital iota.
            अगर ((तथ्य & HAS_YPOGEGRAMMENI) != 0) {
                अंक_Ypogegrammeni = 1;
            }
            स्थिर UBool hasPrecomposedAccent = (तथ्य & HAS_ACCENT) != 0;
            // Skip combining diacritics after this Greek letter.
            int32_t अग्र_अग्र_सूचकांक = अग्र_सूचकांक;
            जबतक (अग्र_सूचकांक < src_लंबाई) {
                UChar32 c2;
                U8_अग्र(src, अग्र_अग्र_सूचकांक, src_लंबाई, c2);
                uint32_t diacritic_तथ्य = get_Diacritic_तथ्य(c2);
                अगर (diacritic_तथ्य != 0) {
                    तथ्य |= diacritic_तथ्य;
                    अगर ((diacritic_तथ्य & HAS_YPOGEGRAMMENI) != 0) {
                        ++अंक_Ypogegrammeni;
                    }
                    अग्र_सूचकांक = अग्र_अग्र_सूचकांक;
                } अन्यथा {
                    अवरोध;  // not a Greek diacritic
                }
            }
            अगर ((तथ्य & HAS_VOWEL_AND_ACCENT_AND_DIALYTIKA) == HAS_VOWEL_AND_ACCENT) {
                अग्र_State |= hasPrecomposedAccent ? AFTER_VOWEL_WITH_PRECOMPOSED_ACCENT
                                                  : AFTER_VOWEL_WITH_COMBINING_ACCENT;
            }
            // Map according to Greek rules.
            UBool addTonos = false;
            अगर (upper == 0x397 &&
                    (तथ्य & HAS_ACCENT) != 0 &&
                    अंक_Ypogegrammeni == 0 &&
                    (state & AFTER_CASED) == 0 &&
                    !isFollowedByCasedLetter(src, अग्र_सूचकांक, src_लंबाई)) {
                // Keep disjunctive "or" with (only) a tonos.
                // We use the same "word boundary" conditions as for the Final_Sigma test.
                अगर (hasPrecomposedAccent) {
                    upper = 0x389;  // Preserve the precomposed form.
                } अन्यथा {
                    addTonos = true;
                }
            } अन्यथा अगर ((तथ्य & HAS_DIALYTIKA) != 0) {
                // Preserve a vowel with dialytika in precomposed form if it exists.
                अगर (upper == 0x399) {
                    upper = 0x3AA;
                    तथ्य &= ~HAS_EITHER_DIALYTIKA;
                } अन्यथा अगर (upper == 0x3A5) {
                    upper = 0x3AB;
                    तथ्य &= ~HAS_EITHER_DIALYTIKA;
                }
            }

            UBool change;
            अगर (edits == nullptr && (options & U_OMIT_UNCHANGED_TEXT) == 0) {
                change = true;  // common, simple usage
            } अन्यथा {
                // Find out first whether we are changing the text.
                U_ASSERT(0x370 <= upper && upper <= 0x3ff);  // 2-byte UTF-8, main Greek block
                change = (i + 2) > अग्र_सूचकांक ||
                        src[i] != getTwoByteLead(upper) || src[i + 1] != getTwoByteTrail(upper) ||
                        अंक_Ypogegrammeni > 0;
                int32_t i2 = i + 2;
                अगर ((तथ्य & HAS_EITHER_DIALYTIKA) != 0) {
                    change |= (i2 + 2) > अग्र_सूचकांक ||
                            src[i2] != static_cast<uint8_t>(u8"\u0308"[0]) ||
                            src[i2 + 1] != static_cast<uint8_t>(u8"\u0308"[1]);
                    i2 += 2;
                }
                अगर (addTonos) {
                    change |= (i2 + 2) > अग्र_सूचकांक ||
                            src[i2] != static_cast<uint8_t>(u8"\u0301"[0]) ||
                            src[i2 + 1] != static_cast<uint8_t>(u8"\u0301"[1]);
                    i2 += 2;
                }
                int32_t old_लंबाई = अग्र_सूचकांक - i;
                int32_t new_लंबाई = (i2 - i) + अंक_Ypogegrammeni * 2;  // 2 bytes per U+0399
                change |= old_लंबाई != new_लंबाई;
                अगर (change) {
                    अगर (edits != nullptr) {
                        edits->addReplace(old_लंबाई, new_लंबाई);
                    }
                } अन्यथा {
                    अगर (edits != nullptr) {
                        edits->addUnchanged(old_लंबाई);
                    }
                    // Write unchanged text?
                    change = (options & U_OMIT_UNCHANGED_TEXT) == 0;
                }
            }

            अगर (change) {
                ByteSinkUtil::appendTwoBytes(upper, sink);
                अगर ((तथ्य & HAS_EITHER_DIALYTIKA) != 0) {
                    sink.AppendU8(u8"\u0308", 2);  // restore or add a dialytika
                }
                अगर (addTonos) {
                    sink.AppendU8(u8"\u0301", 2);
                }
                जबतक (अंक_Ypogegrammeni > 0) {
                    sink.AppendU8(u8"\u0399", 2);
                    --अंक_Ypogegrammeni;
                }
            }
        } अन्यथा अगर(c>=0) {
            स्थिर char16_t *s;
            c=ucase_toFullUpper(c, nullptr, nullptr, &s, UCASE_LOC_GREEK);
            अगर (!append_परिणाम(अग्र_सूचकांक - i, c, s, sink, options, edits, errorCode)) {
                वापस;
            }
        } अन्यथा {
            // Malformed UTF-8.
            अगर (!ByteSinkUtil::appendUnchanged(src+i, अग्र_सूचकांक-i,
                                               sink, options, edits, errorCode)) {
                वापस;
            }
        }
        i = अग्र_सूचकांक;
        state = अग्र_State;
    }
}

}  // namespace GreekUpper
U_NAMESPACE_END

अटल व्योम U_CALLCONV
ucasemap_internalUTF8ToLower(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_UNUSED
                             स्थिर uint8_t *src, int32_t src_लंबाई,
                             icu::ByteSink &sink, icu::Edits *edits,
                             UErrorCode &errorCode) {
    UCaseContext csc=UCASECONTEXT_INITIALIZER;
    csc.p=(व्योम *)src;
    csc.limit=src_लंबाई;
    toLower(
        caseLocale, options,
        src, &csc, 0, src_लंबाई,
        sink, edits, errorCode);
}

अटल व्योम U_CALLCONV
ucasemap_internalUTF8ToUpper(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_UNUSED
                             स्थिर uint8_t *src, int32_t src_लंबाई,
                             icu::ByteSink &sink, icu::Edits *edits,
                             UErrorCode &errorCode) {
    अगर (caseLocale == UCASE_LOC_GREEK) {
        GreekUpper::toUpper(options, src, src_लंबाई, sink, edits, errorCode);
    } अन्यथा {
        UCaseContext csc=UCASECONTEXT_INITIALIZER;
        csc.p=(व्योम *)src;
        csc.limit=src_लंबाई;
        toUpper(
            caseLocale, options,
            src, &csc, src_लंबाई,
            sink, edits, errorCode);
    }
}

अटल व्योम U_CALLCONV
ucasemap_internalUTF8Fold(int32_t /* caseLocale */, uint32_t options, UCASEMAP_BREAK_ITERATOR_UNUSED
                          स्थिर uint8_t *src, int32_t src_लंबाई,
                          icu::ByteSink &sink, icu::Edits *edits,
                          UErrorCode &errorCode) {
    toLower(
        -1, options,
        src, nullptr, 0, src_लंबाई,
        sink, edits, errorCode);
}

व्योम
ucasemap_mapUTF8(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_PARAM
                 स्थिर अक्षर *src, int32_t src_लंबाई,
                 UTF8CaseMapper *stringCaseMapper,
                 icu::ByteSink &sink, icu::Edits *edits,
                 UErrorCode &errorCode) {
    /* check argument values */
    अगर (U_FAILURE(errorCode)) {
        वापस;
    }
    अगर ((src == nullptr && src_लंबाई != 0) || src_लंबाई < -1) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        वापस;
    }

    // Get the string length.
    अगर (src_लंबाई == -1) {
        src_लंबाई = static_cast<int32_t>(uprv_strlen(src));
    }

    अगर (edits != nullptr && (options & U_EDITS_NO_RESET) == 0) {
        edits->reset();
    }
    stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                     reinterpret_cast<स्थिर uint8_t*>(src), src_लंबाई, sink, edits, errorCode);
    sink.Flush();
    अगर (U_SUCCESS(errorCode)) {
        अगर (edits != nullptr) {
            edits->copyErrorTo(errorCode);
        }
    }
}

int32_t
ucasemap_mapUTF8(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_PARAM
                 अक्षर *dest, int32_t destCapacity,
                 स्थिर अक्षर *src, int32_t src_लंबाई,
                 UTF8CaseMapper *stringCaseMapper,
                 icu::Edits *edits,
                 UErrorCode &errorCode) {
    /* check argument values */
    अगर(U_FAILURE(errorCode)) {
        वापस 0;
    }
    अगर( destCapacity<0 ||
        (dest==nullptr && destCapacity>0) ||
        (src==nullptr && src_लंबाई!=0) || src_लंबाई<-1
    ) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        वापस 0;
    }

    /* get the string length */
    अगर(src_लंबाई==-1) {
        src_लंबाई = static_cast<int32_t>(uprv_strlen(src));
    }

    /* check for overlapping source and destination */
    अगर( dest!=nullptr &&
        ((src>=dest && src<(dest+destCapacity)) ||
         (dest>=src && dest<(src+src_लंबाई)))
    ) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        वापस 0;
    }

    अगर (edits != nullptr && (options & U_EDITS_NO_RESET) == 0) {
        edits->reset();
    }
    int32_t reslen = ByteSinkUtil::viaByteSinkToTerminatedChars(
        dest, destCapacity,
        [&](ByteSink& sink, UErrorCode& status) {
            stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                             reinterpret_cast<स्थिर uint8_t*>(src), src_लंबाई, sink, edits, status);
        },
        errorCode);
    अगर (U_SUCCESS(errorCode) && edits != nullptr) {
        edits->copyErrorTo(errorCode);
    }
    वापस reslen;
}

/* public API functions */

U_CAPI int32_t U_EXPORT2
ucasemap_utf8ToLower(स्थिर UCaseMap *csm,
                     अक्षर *dest, int32_t destCapacity,
                     स्थिर अक्षर *src, int32_t src_लंबाई,
                     UErrorCode *pErrorCode) {
    स्वतः *impl = reinterpret_cast<स्थिर UCaseMapImpl *>(csm);
    वापस ucasemap_mapUTF8(
        impl->caseLocale, impl->options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, destCapacity,
        src, src_लंबाई,
        ucasemap_internalUTF8ToLower, nullptr, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
ucasemap_utf8ToUpper(स्थिर UCaseMap *csm,
                     अक्षर *dest, int32_t destCapacity,
                     स्थिर अक्षर *src, int32_t src_लंबाई,
                     UErrorCode *pErrorCode) {
    स्वतः *impl = reinterpret_cast<स्थिर UCaseMapImpl *>(csm);
    वापस ucasemap_mapUTF8(
        impl->caseLocale, impl->options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, destCapacity,
        src, src_लंबाई,
        ucasemap_internalUTF8ToUpper, nullptr, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
ucasemap_utf8FoldCase(स्थिर UCaseMap *csm,
                      अक्षर *dest, int32_t destCapacity,
                      स्थिर अक्षर *src, int32_t src_लंबाई,
                      UErrorCode *pErrorCode) {
    स्वतः *impl = reinterpret_cast<स्थिर UCaseMapImpl *>(csm);
    वापस ucasemap_mapUTF8(
        UCASE_LOC_मूल, impl->options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, destCapacity,
        src, src_लंबाई,
        ucasemap_internalUTF8Fold, nullptr, *pErrorCode);
}

U_NAMESPACE_BEGIN

व्योम CaseMap::utf8ToLower(
        स्थिर अक्षर *locale, uint32_t options,
        StringPiece src, ByteSink &sink, Edits *edits,
        UErrorCode &errorCode) {
    ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        src.तथ्य(), src.length(),
        ucasemap_internalUTF8ToLower, sink, edits, errorCode);
}

व्योम CaseMap::utf8ToUpper(
        स्थिर अक्षर *locale, uint32_t options,
        StringPiece src, ByteSink &sink, Edits *edits,
        UErrorCode &errorCode) {
    ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        src.तथ्य(), src.length(),
        ucasemap_internalUTF8ToUpper, sink, edits, errorCode);
}

व्योम CaseMap::utf8Fold(
        uint32_t options,
        StringPiece src, ByteSink &sink, Edits *edits,
        UErrorCode &errorCode) {
    ucasemap_mapUTF8(
        UCASE_LOC_मूल, options, UCASEMAP_BREAK_ITERATOR_NULL
        src.तथ्य(), src.length(),
        ucasemap_internalUTF8Fold, sink, edits, errorCode);
}

int32_t CaseMap::utf8ToLower(
        स्थिर अक्षर *locale, uint32_t options,
        स्थिर अक्षर *src, int32_t src_लंबाई,
        अक्षर *dest, int32_t destCapacity, Edits *edits,
        UErrorCode &errorCode) {
    वापस ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, destCapacity,
        src, src_लंबाई,
        ucasemap_internalUTF8ToLower, edits, errorCode);
}

int32_t CaseMap::utf8ToUpper(
        स्थिर अक्षर *locale, uint32_t options,
        स्थिर अक्षर *src, int32_t src_लंबाई,
        अक्षर *dest, int32_t destCapacity, Edits *edits,
        UErrorCode &errorCode) {
    वापस ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, destCapacity,
        src, src_लंबाई,
        ucasemap_internalUTF8ToUpper, edits, errorCode);
}

int32_t CaseMap::utf8Fold(
        uint32_t options,
        स्थिर अक्षर *src, int32_t src_लंबाई,
        अक्षर *dest, int32_t destCapacity, Edits *edits,
        UErrorCode &errorCode) {
    वापस ucasemap_mapUTF8(
        UCASE_LOC_मूल, options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, destCapacity,
        src, src_लंबाई,
        ucasemap_internalUTF8Fold, edits, errorCode);
}

U_NAMESPACE_END
