// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*
*   Copyright (C) 2001-2015, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  ustrcase.cpp
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2002feb20
*   created by: Markus W. Scherer
*
*   Implementation file for string casing C API functions.
*   Uses functions from uchar.c for basic functionality that requires access
*   to the Unicode Character Database (uprops.dat).
*/

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"
#ଅନ୍ତର୍ଭାଵ "unicode/brkiter.h"
#ଅନ୍ତର୍ଭାଵ "unicode/casemap.h"
#ଅନ୍ତର୍ଭାଵ "unicode/edits.h"
#ଅନ୍ତର୍ଭାଵ "unicode/stringoptions.h"
#ଅନ୍ତର୍ଭାଵ "unicode/ustring.h"
#ଅନ୍ତର୍ଭାଵ "unicode/ucasemap.h"
#ଅନ୍ତର୍ଭାଵ "unicode/ubrk.h"
#ଅନ୍ତର୍ଭାଵ "unicode/utf.h"
#ଅନ୍ତର୍ଭାଵ "unicode/utf16.h"
#ଅନ୍ତର୍ଭାଵ "cmemory.h"
#ଅନ୍ତର୍ଭାଵ "ucase.h"
#ଅନ୍ତର୍ଭାଵ "ucasemap_imp.h"
#ଅନ୍ତର୍ଭାଵ "ustr_imp.h"
#ଅନ୍ତର୍ଭାଵ "uassert.h"

/**
 * Code point for COMBINING ACUTE ACCENT
 * @internal
 */
#define ACUTE u'\u0301'

U_NAMESPACE_BEGIN

ନାମାକାଶ {

int32_t checkOverflowAndEditsError(int32_t destIndex, int32_t destCapacity,
                                   Edits *edits, UErrorCode &errorCode) {
    ଯଦି (U_SUCCESS(errorCode)) {
        ଯଦି (destIndex > destCapacity) {
            errorCode = U_BUFFER_OVERFLOW_ERROR;
        } ଅନ୍ୟଥା ଯଦି (edits != nullptr) {
            edits->copyErrorTo(errorCode);
        }
    }
    ଫେରନ୍ତୁ destIndex;
}

/* Appends a full case mapping result, see UCASE_MAX_STRING_LENGTH. */
ଧାଡ଼ିଭିତରେ int32_t
appendResult(char16_t *dest, int32_t destIndex, int32_t destCapacity,
             int32_t result, ଅଚଳ char16_t *s,
             int32_t cpLength, uint32_t options, icu::Edits *edits) {
    UChar32 c;
    int32_t length;

    /* decode the result */
    ଯଦି(result<0) {
        /* (not) original code point */
        ଯଦି(edits!=nullptr) {
            edits->addUnchanged(cpLength);
        }
        ଯଦି(options & U_OMIT_UNCHANGED_TEXT) {
            ଫେରନ୍ତୁ destIndex;
        }
        c=~result;
        ଯଦି(destIndex<destCapacity && c<=0xffff) {  // BMP slightly-fastpath
            dest[destIndex++] = static_cast<char16_t>(c);
            ଫେରନ୍ତୁ destIndex;
        }
        length=cpLength;
    } ଅନ୍ୟଥା {
        ଯଦି(result<=UCASE_MAX_STRING_LENGTH) {
            c=U_SENTINEL;
            length=result;
        } ଅନ୍ୟଥା ଯଦି(destIndex<destCapacity && result<=0xffff) {  // BMP slightly-fastpath
            dest[destIndex++] = static_cast<char16_t>(result);
            ଯଦି(edits!=nullptr) {
                edits->addReplace(cpLength, 1);
            }
            ଫେରନ୍ତୁ destIndex;
        } ଅନ୍ୟଥା {
            c=result;
            length=U16_LENGTH(c);
        }
        ଯଦି(edits!=nullptr) {
            edits->addReplace(cpLength, length);
        }
    }
    ଯଦି(length>(INT32_MAX-destIndex)) {
        ଫେରନ୍ତୁ -1;  // integer overflow
    }

    ଯଦି(destIndex<destCapacity) {
        /* append the result */
        ଯଦି(c>=0) {
            /* code point */
            UBool isError=ମିଥ୍ୟା;
            U16_APPEND(dest, destIndex, destCapacity, c, isError);
            ଯଦି(isError) {
                /* overflow, nothing written */
                destIndex+=length;
            }
        } ଅନ୍ୟଥା {
            /* string */
            ଯଦି((destIndex+length)<=destCapacity) {
                ଯେତେବେଳେ(length>0) {
                    dest[destIndex++]=*s++;
                    --length;
                }
            } ଅନ୍ୟଥା {
                /* overflow */
                destIndex+=length;
            }
        }
    } ଅନ୍ୟଥା {
        /* preflight */
        destIndex+=length;
    }
    ଫେରନ୍ତୁ destIndex;
}

ଧାଡ଼ିଭିତରେ int32_t
appendUChar(char16_t *dest, int32_t destIndex, int32_t destCapacity, char16_t c) {
    ଯଦି(destIndex<destCapacity) {
        dest[destIndex]=c;
    } ଅନ୍ୟଥା ଯଦି(destIndex==INT32_MAX) {
        ଫେରନ୍ତୁ -1;  // integer overflow
    }
    ଫେରନ୍ତୁ destIndex+1;
}

int32_t
appendNonEmptyUnchanged(char16_t *dest, int32_t destIndex, int32_t destCapacity,
                        ଅଚଳ char16_t *s, int32_t length, uint32_t options, icu::Edits *edits) {
    ଯଦି(edits!=nullptr) {
        edits->addUnchanged(length);
    }
    ଯଦି(options & U_OMIT_UNCHANGED_TEXT) {
        ଫେରନ୍ତୁ destIndex;
    }
    ଯଦି(length>(INT32_MAX-destIndex)) {
        ଫେରନ୍ତୁ -1;  // integer overflow
    }
    ଯଦି((destIndex+length)<=destCapacity) {
        u_memcpy(dest+destIndex, s, length);
    }
    ଫେରନ୍ତୁ destIndex + length;
}

ଧାଡ଼ିଭିତରେ int32_t
appendUnchanged(char16_t *dest, int32_t destIndex, int32_t destCapacity,
                ଅଚଳ char16_t *s, int32_t length, uint32_t options, icu::Edits *edits) {
    ଯଦି (length <= 0) {
        ଫେରନ୍ତୁ destIndex;
    }
    ଫେରନ୍ତୁ appendNonEmptyUnchanged(dest, destIndex, destCapacity, s, length, options, edits);
}

UChar32 U_CALLCONV
utf16_caseContextIterator(ଶୂନ୍ୟ *context, int8_t dir) {
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
            U16_PREV((ଅଚଳ char16_t *)csc->p, csc->start, csc->index, c);
            ଫେରନ୍ତୁ c;
        }
    } ଅନ୍ୟଥା {
        ଯଦି(csc->index<csc->limit) {
            U16_NEXT((ଅଚଳ char16_t *)csc->p, csc->index, csc->limit, c);
            ଫେରନ୍ତୁ c;
        }
    }
    ଫେରନ୍ତୁ U_SENTINEL;
}

/**
 * caseLocale >= 0: Lowercases [srcStart..srcLimit[ but takes context [0..srcLength[ into account.
 * caseLocale < 0: Case-folds [srcStart..srcLimit[.
 */
int32_t toLower(int32_t caseLocale, uint32_t options,
                char16_t *dest, int32_t destCapacity,
                ଅଚଳ char16_t *src, UCaseContext *csc, int32_t srcStart, int32_t srcLimit,
                icu::Edits *edits, UErrorCode &errorCode) {
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
    int32_t destIndex = 0;
    int32_t prev = srcStart;
    int32_t srcIndex = srcStart;
    ପାଇଁ (;;) {
        // fast path for simple cases
        char16_t lead = 0;
        ଯେତେବେଳେ (srcIndex < srcLimit) {
            lead = src[srcIndex];
            int32_t delta;
            ଯଦି (lead < LatinCase::LONG_S) {
                int8_t d = latinToLower[lead];
                ଯଦି (d == LatinCase::EXC) { ଭାଙ୍ଗ; }
                ++srcIndex;
                ଯଦି (d == 0) { ଜାରି; }
                delta = d;
            } ଅନ୍ୟଥା ଯଦି (lead >= 0xd800) {
                ଭାଙ୍ଗ;  // surrogate or higher
            } ଅନ୍ୟଥା {
                uint16_t props = UTRIE2_GET16_FROM_U16_SINGLE_LEAD(trie, lead);
                ଯଦି (UCASE_HAS_EXCEPTION(props)) { ଭାଙ୍ଗ; }
                ++srcIndex;
                ଯଦି (!UCASE_IS_UPPER_OR_TITLE(props) || (delta = UCASE_GET_DELTA(props)) == 0) {
                    ଜାରି;
                }
            }
            lead += static_cast<char16_t>(delta);
            destIndex = appendUnchanged(dest, destIndex, destCapacity,
                                        src + prev, srcIndex - 1 - prev, options, edits);
            ଯଦି (destIndex >= 0) {
                destIndex = appendUChar(dest, destIndex, destCapacity, lead);
                ଯଦି (edits != nullptr) {
                    edits->addReplace(1, 1);
                }
            }
            ଯଦି (destIndex < 0) {
                errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
                ଫେରନ୍ତୁ 0;
            }
            prev = srcIndex;
        }
        ଯଦି (srcIndex >= srcLimit) {
            ଭାଙ୍ଗ;
        }
        // slow path
        int32_t cpStart = srcIndex++;
        char16_t trail;
        UChar32 c;
        ଯଦି (U16_IS_LEAD(lead) && srcIndex < srcLimit && U16_IS_TRAIL(trail = src[srcIndex])) {
            c = U16_GET_SUPPLEMENTARY(lead, trail);
            ++srcIndex;
        } ଅନ୍ୟଥା {
            c = lead;
        }
        ଅଚଳ char16_t *s = nullptr;
        ଯଦି (caseLocale >= 0) {
            csc->cpStart = cpStart;
            csc->cpLimit = srcIndex;
            c = ucase_toFullLower(c, utf16_caseContextIterator, csc, &s, caseLocale);
        } ଅନ୍ୟଥା {
            c = ucase_toFullFolding(c, &s, options);
        }
        ଯଦି (c >= 0) {
            destIndex = appendUnchanged(dest, destIndex, destCapacity,
                                        src + prev, cpStart - prev, options, edits);
            ଯଦି (destIndex >= 0) {
                destIndex = appendResult(dest, destIndex, destCapacity, c, s,
                                         srcIndex - cpStart, options, edits);
            }
            ଯଦି (destIndex < 0) {
                errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
                ଫେରନ୍ତୁ 0;
            }
            prev = srcIndex;
        }
    }
    destIndex = appendUnchanged(dest, destIndex, destCapacity,
                                src + prev, srcIndex - prev, options, edits);
    ଯଦି (destIndex < 0) {
        errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
        ଫେରନ୍ତୁ 0;
    }
    ଫେରନ୍ତୁ destIndex;
}

int32_t toUpper(int32_t caseLocale, uint32_t options,
                char16_t *dest, int32_t destCapacity,
                ଅଚଳ char16_t *src, UCaseContext *csc, int32_t srcLength,
                icu::Edits *edits, UErrorCode &errorCode) {
    ଅଚଳ int8_t *latinToUpper;
    ଯଦି (caseLocale == UCASE_LOC_TURKISH) {
        latinToUpper = LatinCase::TO_UPPER_TR;
    } ଅନ୍ୟଥା {
        latinToUpper = LatinCase::TO_UPPER_NORMAL;
    }
    ଅଚଳ UTrie2 *trie = ucase_getTrie();
    int32_t destIndex = 0;
    int32_t prev = 0;
    int32_t srcIndex = 0;
    ପାଇଁ (;;) {
        // fast path for simple cases
        char16_t lead = 0;
        ଯେତେବେଳେ (srcIndex < srcLength) {
            lead = src[srcIndex];
            int32_t delta;
            ଯଦି (lead < LatinCase::LONG_S) {
                int8_t d = latinToUpper[lead];
                ଯଦି (d == LatinCase::EXC) { ଭାଙ୍ଗ; }
                ++srcIndex;
                ଯଦି (d == 0) { ଜାରି; }
                delta = d;
            } ଅନ୍ୟଥା ଯଦି (lead >= 0xd800) {
                ଭାଙ୍ଗ;  // surrogate or higher
            } ଅନ୍ୟଥା {
                uint16_t props = UTRIE2_GET16_FROM_U16_SINGLE_LEAD(trie, lead);
                ଯଦି (UCASE_HAS_EXCEPTION(props)) { ଭାଙ୍ଗ; }
                ++srcIndex;
                ଯଦି (UCASE_GET_TYPE(props) != UCASE_LOWER || (delta = UCASE_GET_DELTA(props)) == 0) {
                    ଜାରି;
                }
            }
            lead += static_cast<char16_t>(delta);
            destIndex = appendUnchanged(dest, destIndex, destCapacity,
                                        src + prev, srcIndex - 1 - prev, options, edits);
            ଯଦି (destIndex >= 0) {
                destIndex = appendUChar(dest, destIndex, destCapacity, lead);
                ଯଦି (edits != nullptr) {
                    edits->addReplace(1, 1);
                }
            }
            ଯଦି (destIndex < 0) {
                errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
                ଫେରନ୍ତୁ 0;
            }
            prev = srcIndex;
        }
        ଯଦି (srcIndex >= srcLength) {
            ଭାଙ୍ଗ;
        }
        // slow path
        int32_t cpStart;
        csc->cpStart = cpStart = srcIndex++;
        char16_t trail;
        UChar32 c;
        ଯଦି (U16_IS_LEAD(lead) && srcIndex < srcLength && U16_IS_TRAIL(trail = src[srcIndex])) {
            c = U16_GET_SUPPLEMENTARY(lead, trail);
            ++srcIndex;
        } ଅନ୍ୟଥା {
            c = lead;
        }
        csc->cpLimit = srcIndex;
        ଅଚଳ char16_t *s = nullptr;
        c = ucase_toFullUpper(c, utf16_caseContextIterator, csc, &s, caseLocale);
        ଯଦି (c >= 0) {
            destIndex = appendUnchanged(dest, destIndex, destCapacity,
                                        src + prev, cpStart - prev, options, edits);
            ଯଦି (destIndex >= 0) {
                destIndex = appendResult(dest, destIndex, destCapacity, c, s,
                                         srcIndex - cpStart, options, edits);
            }
            ଯଦି (destIndex < 0) {
                errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
                ଫେରନ୍ତୁ 0;
            }
            prev = srcIndex;
        }
    }
    destIndex = appendUnchanged(dest, destIndex, destCapacity,
                                src + prev, srcIndex - prev, options, edits);
    ଯଦି (destIndex < 0) {
        errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
        ଫେରନ୍ତୁ 0;
    }
    ଫେରନ୍ତୁ destIndex;
}

}  // namespace

U_NAMESPACE_END

U_NAMESPACE_USE

#ଯଦି !UCONFIG_NO_BREAK_ITERATION

ନାମାକାଶ {

/**
 * Input: c is a letter I with or without acute accent.
 * start is the index in src after c, and is less than segmentLimit.
 * If a plain i/I is followed by a plain j/J,
 * or an i/I with acute (precomposed or decomposed) is followed by a j/J with acute,
 * then we output accordingly.
 *
 * @return the src index after the titlecased sequence, or the start index if no Dutch IJ
 */
int32_t maybeTitleDutchIJ(ଅଚଳ char16_t *src, UChar32 c, int32_t start, int32_t segmentLimit,
                          char16_t *dest, int32_t &destIndex, int32_t destCapacity, uint32_t options,
                          icu::Edits *edits) {
    U_ASSERT(start < segmentLimit);

    int32_t index = start;
    ତାର୍କିକ withAcute = ମିଥ୍ୟା;

    // If the conditions are met, then the following variables tell us what to output.
    int32_t unchanged1 = 0;  // code units before the j, or the whole sequence (0..3)
    ତାର୍କିକ doTitleJ = ମିଥ୍ୟା;  // true if the j needs to be titlecased
    int32_t unchanged2 = 0;  // after the j (0 or 1)

    // next character after the first letter
    char16_t c2 = src[index++];

    // Is the first letter an i/I with accent?
    ଯଦି (c == u'I') {
        ଯଦି (c2 == ACUTE) {
            withAcute = ସତ୍ୟ;
            unchanged1 = 1;
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
        ଯଦି (index == segmentLimit || src[index++] != ACUTE) { ଫେରନ୍ତୁ start; }
        ଯଦି (doTitleJ) {
            unchanged2 = 1;
        } ଅନ୍ୟଥା {
            ++unchanged1;
        }
    }

    // There must not be another combining mark.
    ଯଦି (index < segmentLimit) {
        int32_t cp;
        int32_t i = index;
        U16_NEXT(src, i, segmentLimit, cp);
        uint32_t typeMask = U_GET_GC_MASK(cp);
        ଯଦି ((typeMask & U_GC_M_MASK) != 0) {
            ଫେରନ୍ତୁ start;
        }
    }

    // Output the rest of the Dutch IJ.
    destIndex = appendUnchanged(dest, destIndex, destCapacity, src + start, unchanged1, options, edits);
    start += unchanged1;
    ଯଦି (doTitleJ) {
        destIndex = appendUChar(dest, destIndex, destCapacity, u'J');
        ଯଦି (edits != nullptr) {
            edits->addReplace(1, 1);
        }
        ++start;
    }
    destIndex = appendUnchanged(dest, destIndex, destCapacity, src + start, unchanged2, options, edits);

    U_ASSERT(start + unchanged2 == index);
    ଫେରନ୍ତୁ index;
}

}  // namespace

U_CFUNC int32_t U_CALLCONV
ustrcase_internalToTitle(int32_t caseLocale, uint32_t options, BreakIterator *iter,
                         char16_t *dest, int32_t destCapacity,
                         ଅଚଳ char16_t *src, int32_t srcLength,
                         icu::Edits *edits,
                         UErrorCode &errorCode) {
    ଯଦି (!ustrcase_checkTitleAdjustmentOptions(options, errorCode)) {
        ଫେରନ୍ତୁ 0;
    }

    /* set up local variables */
    UCaseContext csc=UCASECONTEXT_INITIALIZER;
    csc.p=(ଶୂନ୍ୟ *)src;
    csc.limit=srcLength;
    int32_t destIndex=0;
    int32_t prev=0;
    ତାର୍କିକ isFirstIndex=ସତ୍ୟ;

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
            // Find and copy skipped characters [prev..titleStart[
            int32_t titleStart=prev;
            int32_t titleLimit=prev;
            UChar32 c;
            U16_NEXT(src, titleLimit, index, c);
            ଯଦି ((options&U_TITLECASE_NO_BREAK_ADJUSTMENT)==0) {
                // Adjust the titlecasing index to the next cased character,
                // or to the next letter/number/symbol/private use.
                // Stop with titleStart<titleLimit<=index
                // if there is a character to be titlecased,
                // or else stop with titleStart==titleLimit==index.
                ତାର୍କିକ toCased = (options&U_TITLECASE_ADJUST_TO_CASED) != 0;
                ଯେତେବେଳେ (toCased ? UCASE_NONE==ucase_getType(c) : !ustrcase_isLNS(c)) {
                    titleStart=titleLimit;
                    ଯଦି(titleLimit==index) {
                        ଭାଙ୍ଗ;
                    }
                    U16_NEXT(src, titleLimit, index, c);
                }
                ଯଦି (prev < titleStart) {
                    destIndex=appendUnchanged(dest, destIndex, destCapacity,
                                              src+prev, titleStart-prev, options, edits);
                    ଯଦି(destIndex<0) {
                        errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
                        ଫେରନ୍ତୁ 0;
                    }
                }
            }

            ଯଦି(titleStart<titleLimit) {
                /* titlecase c which is from [titleStart..titleLimit[ */
                csc.cpStart=titleStart;
                csc.cpLimit=titleLimit;
                ଅଚଳ char16_t *s;
                c=ucase_toFullTitle(c, utf16_caseContextIterator, &csc, &s, caseLocale);
                destIndex=appendResult(dest, destIndex, destCapacity, c, s,
                                       titleLimit-titleStart, options, edits);
                ଯଦି(destIndex<0) {
                    errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
                    ଫେରନ୍ତୁ 0;
                }

                /* Special case Dutch IJ titlecasing */
                ଯଦି (titleStart+1 < index &&
                        caseLocale == UCASE_LOC_DUTCH) {
                    ଯଦି (c < 0) {
                        c = ~c;
                    }

                    ଯଦି (c == u'I' || c == u'Í') {
                        titleLimit = maybeTitleDutchIJ(src, c, titleStart + 1, index,
                                                       dest, destIndex, destCapacity, options,
                                                       edits);
                    }
                }

                /* lowercase [titleLimit..index[ */
                ଯଦି(titleLimit<index) {
                    ଯଦି((options&U_TITLECASE_NO_LOWERCASE)==0) {
                        /* Normal operation: Lowercase the rest of the word. */
                        destIndex+=
                            toLower(
                                caseLocale, options,
                                (dest==nullptr) ? nullptr: dest+destIndex, destCapacity-destIndex,
                                src, &csc, titleLimit, index,
                                edits, errorCode);
                        ଯଦି(errorCode==U_BUFFER_OVERFLOW_ERROR) {
                            errorCode=U_ZERO_ERROR;
                        }
                        ଯଦି(U_FAILURE(errorCode)) {
                            ଫେରନ୍ତୁ destIndex;
                        }
                    } ଅନ୍ୟଥା {
                        /* Optionally just copy the rest of the word unchanged. */
                        destIndex=appendUnchanged(dest, destIndex, destCapacity,
                                                  src+titleLimit, index-titleLimit, options, edits);
                        ଯଦି(destIndex<0) {
                            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
                            ଫେରନ୍ତୁ 0;
                        }
                    }
                }
            }
        }

        prev=index;
    }

    ଫେରନ୍ତୁ checkOverflowAndEditsError(destIndex, destCapacity, edits, errorCode);
}

#endif  // !UCONFIG_NO_BREAK_ITERATION

U_NAMESPACE_BEGIN
ନାମାକାଶ GreekUpper {

// Data generated by prototype code, see
// https://icu.unicode.org/design/case/greek-upper
// TODO: Move this data into ucase.icu.
ସ୍ଥିର ଅଚଳ uint16_t data0370[] = {
    // U+0370..03FF
    0x0370,
    0x0370,
    0x0372,
    0x0372,
    0,
    0,
    0x0376,
    0x0376,
    0,
    0,
    0x037A,
    0x03FD,
    0x03FE,
    0x03FF,
    0,
    0x037F,
    0,
    0,
    0,
    0,
    0,
    0,
    0x0391 | HAS_VOWEL | HAS_ACCENT,
    0,
    0x0395 | HAS_VOWEL | HAS_ACCENT,
    0x0397 | HAS_VOWEL | HAS_ACCENT,
    0x0399 | HAS_VOWEL | HAS_ACCENT,
    0,
    0x039F | HAS_VOWEL | HAS_ACCENT,
    0,
    0x03A5 | HAS_VOWEL | HAS_ACCENT,
    0x03A9 | HAS_VOWEL | HAS_ACCENT,
    0x0399 | HAS_VOWEL | HAS_ACCENT | HAS_DIALYTIKA,
    0x0391 | HAS_VOWEL,
    0x0392,
    0x0393,
    0x0394,
    0x0395 | HAS_VOWEL,
    0x0396,
    0x0397 | HAS_VOWEL,
    0x0398,
    0x0399 | HAS_VOWEL,
    0x039A,
    0x039B,
    0x039C,
    0x039D,
    0x039E,
    0x039F | HAS_VOWEL,
    0x03A0,
    0x03A1,
    0,
    0x03A3,
    0x03A4,
    0x03A5 | HAS_VOWEL,
    0x03A6,
    0x03A7,
    0x03A8,
    0x03A9 | HAS_VOWEL,
    0x0399 | HAS_VOWEL | HAS_DIALYTIKA,
    0x03A5 | HAS_VOWEL | HAS_DIALYTIKA,
    0x0391 | HAS_VOWEL | HAS_ACCENT,
    0x0395 | HAS_VOWEL | HAS_ACCENT,
    0x0397 | HAS_VOWEL | HAS_ACCENT,
    0x0399 | HAS_VOWEL | HAS_ACCENT,
    0x03A5 | HAS_VOWEL | HAS_ACCENT | HAS_DIALYTIKA,
    0x0391 | HAS_VOWEL,
    0x0392,
    0x0393,
    0x0394,
    0x0395 | HAS_VOWEL,
    0x0396,
    0x0397 | HAS_VOWEL,
    0x0398,
    0x0399 | HAS_VOWEL,
    0x039A,
    0x039B,
    0x039C,
    0x039D,
    0x039E,
    0x039F | HAS_VOWEL,
    0x03A0,
    0x03A1,
    0x03A3,
    0x03A3,
    0x03A4,
    0x03A5 | HAS_VOWEL,
    0x03A6,
    0x03A7,
    0x03A8,
    0x03A9 | HAS_VOWEL,
    0x0399 | HAS_VOWEL | HAS_DIALYTIKA,
    0x03A5 | HAS_VOWEL | HAS_DIALYTIKA,
    0x039F | HAS_VOWEL | HAS_ACCENT,
    0x03A5 | HAS_VOWEL | HAS_ACCENT,
    0x03A9 | HAS_VOWEL | HAS_ACCENT,
    0x03CF,
    0x0392,
    0x0398,
    0x03D2,
    0x03D2 | HAS_ACCENT,
    0x03D2 | HAS_DIALYTIKA,
    0x03A6,
    0x03A0,
    0x03CF,
    0x03D8,
    0x03D8,
    0x03DA,
    0x03DA,
    0x03DC,
    0x03DC,
    0x03DE,
    0x03DE,
    0x03E0,
    0x03E0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0x039A,
    0x03A1,
    0x03F9,
    0x037F,
    0x03F4,
    0x0395 | HAS_VOWEL,
    0,
    0x03F7,
    0x03F7,
    0x03F9,
    0x03FA,
    0x03FA,
    0x03FC,
    0x03FD,
    0x03FE,
    0x03FF,
};

ସ୍ଥିର ଅଚଳ uint16_t data1F00[] = {
    // U+1F00..1FFF
    0x0391 | HAS_VOWEL,
    0x0391 | HAS_VOWEL,
    0x0391 | HAS_VOWEL | HAS_ACCENT,
    0x0391 | HAS_VOWEL | HAS_ACCENT,
    0x0391 | HAS_VOWEL | HAS_ACCENT,
    0x0391 | HAS_VOWEL | HAS_ACCENT,
    0x0391 | HAS_VOWEL | HAS_ACCENT,
    0x0391 | HAS_VOWEL | HAS_ACCENT,
    0x0391 | HAS_VOWEL,
    0x0391 | HAS_VOWEL,
    0x0391 | HAS_VOWEL | HAS_ACCENT,
    0x0391 | HAS_VOWEL | HAS_ACCENT,
    0x0391 | HAS_VOWEL | HAS_ACCENT,
    0x0391 | HAS_VOWEL | HAS_ACCENT,
    0x0391 | HAS_VOWEL | HAS_ACCENT,
    0x0391 | HAS_VOWEL | HAS_ACCENT,
    0x0395 | HAS_VOWEL,
    0x0395 | HAS_VOWEL,
    0x0395 | HAS_VOWEL | HAS_ACCENT,
    0x0395 | HAS_VOWEL | HAS_ACCENT,
    0x0395 | HAS_VOWEL | HAS_ACCENT,
    0x0395 | HAS_VOWEL | HAS_ACCENT,
    0,
    0,
    0x0395 | HAS_VOWEL,
    0x0395 | HAS_VOWEL,
    0x0395 | HAS_VOWEL | HAS_ACCENT,
    0x0395 | HAS_VOWEL | HAS_ACCENT,
    0x0395 | HAS_VOWEL | HAS_ACCENT,
    0x0395 | HAS_VOWEL | HAS_ACCENT,
    0,
    0,
    0x0397 | HAS_VOWEL,
    0x0397 | HAS_VOWEL,
    0x0397 | HAS_VOWEL | HAS_ACCENT,
    0x0397 | HAS_VOWEL | HAS_ACCENT,
    0x0397 | HAS_VOWEL | HAS_ACCENT,
    0x0397 | HAS_VOWEL | HAS_ACCENT,
    0x0397 | HAS_VOWEL | HAS_ACCENT,
    0x0397 | HAS_VOWEL | HAS_ACCENT,
    0x0397 | HAS_VOWEL,
    0x0397 | HAS_VOWEL,
    0x0397 | HAS_VOWEL | HAS_ACCENT,
    0x0397 | HAS_VOWEL | HAS_ACCENT,
    0x0397 | HAS_VOWEL | HAS_ACCENT,
    0x0397 | HAS_VOWEL | HAS_ACCENT,
    0x0397 | HAS_VOWEL | HAS_ACCENT,
    0x0397 | HAS_VOWEL | HAS_ACCENT,
    0x0399 | HAS_VOWEL,
    0x0399 | HAS_VOWEL,
    0x0399 | HAS_VOWEL | HAS_ACCENT,
    0x0399 | HAS_VOWEL | HAS_ACCENT,
    0x0399 | HAS_VOWEL | HAS_ACCENT,
    0x0399 | HAS_VOWEL | HAS_ACCENT,
    0x0399 | HAS_VOWEL | HAS_ACCENT,
    0x0399 | HAS_VOWEL | HAS_ACCENT,
    0x0399 | HAS_VOWEL,
    0x0399 | HAS_VOWEL,
    0x0399 | HAS_VOWEL | HAS_ACCENT,
    0x0399 | HAS_VOWEL | HAS_ACCENT,
    0x0399 | HAS_VOWEL | HAS_ACCENT,
    0x0399 | HAS_VOWEL | HAS_ACCENT,
    0x0399 | HAS_VOWEL | HAS_ACCENT,
    0x0399 | HAS_VOWEL | HAS_ACCENT,
    0x039F | HAS_VOWEL,
    0x039F | HAS_VOWEL,
    0x039F | HAS_VOWEL | HAS_ACCENT,
    0x039F | HAS_VOWEL | HAS_ACCENT,
    0x039F | HAS_VOWEL | HAS_ACCENT,
    0x039F | HAS_VOWEL | HAS_ACCENT,
    0,
    0,
    0x039F | HAS_VOWEL,
    0x039F | HAS_VOWEL,
    0x039F | HAS_VOWEL | HAS_ACCENT,
    0x039F | HAS_VOWEL | HAS_ACCENT,
    0x039F | HAS_VOWEL | HAS_ACCENT,
    0x039F | HAS_VOWEL | HAS_ACCENT,
    0,
    0,
    0x03A5 | HAS_VOWEL,
    0x03A5 | HAS_VOWEL,
    0x03A5 | HAS_VOWEL | HAS_ACCENT,
    0x03A5 | HAS_VOWEL | HAS_ACCENT,
    0x03A5 | HAS_VOWEL | HAS_ACCENT,
    0x03A5 | HAS_VOWEL | HAS_ACCENT,
    0x03A5 | HAS_VOWEL | HAS_ACCENT,
    0x03A5 | HAS_VOWEL | HAS_ACCENT,
    0,
    0x03A5 | HAS_VOWEL,
    0,
    0x03A5 | HAS_VOWEL | HAS_ACCENT,
    0,
    0x03A5 | HAS_VOWEL | HAS_ACCENT,
    0,
    0x03A5 | HAS_VOWEL | HAS_ACCENT,
    0x03A9 | HAS_VOWEL,
    0x03A9 | HAS_VOWEL,
    0x03A9 | HAS_VOWEL | HAS_ACCENT,
    0x03A9 | HAS_VOWEL | HAS_ACCENT,
    0x03A9 | HAS_VOWEL | HAS_ACCENT,
    0x03A9 | HAS_VOWEL | HAS_ACCENT,
    0x03A9 | HAS_VOWEL | HAS_ACCENT,
    0x03A9 | HAS_VOWEL | HAS_ACCENT,
    0x03A9 | HAS_VOWEL,
    0x03A9 | HAS_VOWEL,
    0x03A9 | HAS_VOWEL | HAS_ACCENT,
    0x03A9 | HAS_VOWEL | HAS_ACCENT,
    0x03A9 | HAS_VOWEL | HAS_ACCENT,
    0x03A9 | HAS_VOWEL | HAS_ACCENT,
    0x03A9 | HAS_VOWEL | HAS_ACCENT,
    0x03A9 | HAS_VOWEL | HAS_ACCENT,
    0x0391 | HAS_VOWEL | HAS_ACCENT,
    0x0391 | HAS_VOWEL | HAS_ACCENT,
    0x0395 | HAS_VOWEL | HAS_ACCENT,
    0x0395 | HAS_VOWEL | HAS_ACCENT,
    0x0397 | HAS_VOWEL | HAS_ACCENT,
    0x0397 | HAS_VOWEL | HAS_ACCENT,
    0x0399 | HAS_VOWEL | HAS_ACCENT,
    0x0399 | HAS_VOWEL | HAS_ACCENT,
    0x039F | HAS_VOWEL | HAS_ACCENT,
    0x039F | HAS_VOWEL | HAS_ACCENT,
    0x03A5 | HAS_VOWEL | HAS_ACCENT,
    0x03A5 | HAS_VOWEL | HAS_ACCENT,
    0x03A9 | HAS_VOWEL | HAS_ACCENT,
    0x03A9 | HAS_VOWEL | HAS_ACCENT,
    0,
    0,
    0x0391 | HAS_VOWEL | HAS_YPOGEGRAMMENI,
    0x0391 | HAS_VOWEL | HAS_YPOGEGRAMMENI,
    0x0391 | HAS_VOWEL | HAS_YPOGEGRAMMENI | HAS_ACCENT,
    0x0391 | HAS_VOWEL | HAS_YPOGEGRAMMENI | HAS_ACCENT,
    0x0391 | HAS_VOWEL | HAS_YPOGEGRAMMENI | HAS_ACCENT,
    0x0391 | HAS_VOWEL | HAS_YPOGEGRAMMENI | HAS_ACCENT,
    0x0391 | HAS_VOWEL | HAS_YPOGEGRAMMENI | HAS_ACCENT,
    0x0391 | HAS_VOWEL | HAS_YPOGEGRAMMENI | HAS_ACCENT,
    0x0391 | HAS_VOWEL | HAS_YPOGEGRAMMENI,
    0x0391 | HAS_VOWEL | HAS_YPOGEGRAMMENI,
    0x0391 | HAS_VOWEL | HAS_YPOGEGRAMMENI | HAS_ACCENT,
    0x0391 | HAS_VOWEL | HAS_YPOGEGRAMMENI | HAS_ACCENT,
    0x0391 | HAS_VOWEL | HAS_YPOGEGRAMMENI | HAS_ACCENT,
    0x0391 | HAS_VOWEL | HAS_YPOGEGRAMMENI | HAS_ACCENT,
    0x0391 | HAS_VOWEL | HAS_YPOGEGRAMMENI | HAS_ACCENT,
    0x0391 | HAS_VOWEL | HAS_YPOGEGRAMMENI | HAS_ACCENT,
    0x0397 | HAS_VOWEL | HAS_YPOGEGRAMMENI,
    0x0397 | HAS_VOWEL | HAS_YPOGEGRAMMENI,
    0x0397 | HAS_VOWEL | HAS_YPOGEGRAMMENI | HAS_ACCENT,
    0x0397 | HAS_VOWEL | HAS_YPOGEGRAMMENI | HAS_ACCENT,
    0x0397 | HAS_VOWEL | HAS_YPOGEGRAMMENI | HAS_ACCENT,
    0x0397 | HAS_VOWEL | HAS_YPOGEGRAMMENI | HAS_ACCENT,
    0x0397 | HAS_VOWEL | HAS_YPOGEGRAMMENI | HAS_ACCENT,
    0x0397 | HAS_VOWEL | HAS_YPOGEGRAMMENI | HAS_ACCENT,
    0x0397 | HAS_VOWEL | HAS_YPOGEGRAMMENI,
    0x0397 | HAS_VOWEL | HAS_YPOGEGRAMMENI,
    0x0397 | HAS_VOWEL | HAS_YPOGEGRAMMENI | HAS_ACCENT,
    0x0397 | HAS_VOWEL | HAS_YPOGEGRAMMENI | HAS_ACCENT,
    0x0397 | HAS_VOWEL | HAS_YPOGEGRAMMENI | HAS_ACCENT,
    0x0397 | HAS_VOWEL | HAS_YPOGEGRAMMENI | HAS_ACCENT,
    0x0397 | HAS_VOWEL | HAS_YPOGEGRAMMENI | HAS_ACCENT,
    0x0397 | HAS_VOWEL | HAS_YPOGEGRAMMENI | HAS_ACCENT,
    0x03A9 | HAS_VOWEL | HAS_YPOGEGRAMMENI,
    0x03A9 | HAS_VOWEL | HAS_YPOGEGRAMMENI,
    0x03A9 | HAS_VOWEL | HAS_YPOGEGRAMMENI | HAS_ACCENT,
    0x03A9 | HAS_VOWEL | HAS_YPOGEGRAMMENI | HAS_ACCENT,
    0x03A9 | HAS_VOWEL | HAS_YPOGEGRAMMENI | HAS_ACCENT,
    0x03A9 | HAS_VOWEL | HAS_YPOGEGRAMMENI | HAS_ACCENT,
    0x03A9 | HAS_VOWEL | HAS_YPOGEGRAMMENI | HAS_ACCENT,
    0x03A9 | HAS_VOWEL | HAS_YPOGEGRAMMENI | HAS_ACCENT,
    0x03A9 | HAS_VOWEL | HAS_YPOGEGRAMMENI,
    0x03A9 | HAS_VOWEL | HAS_YPOGEGRAMMENI,
    0x03A9 | HAS_VOWEL | HAS_YPOGEGRAMMENI | HAS_ACCENT,
    0x03A9 | HAS_VOWEL | HAS_YPOGEGRAMMENI | HAS_ACCENT,
    0x03A9 | HAS_VOWEL | HAS_YPOGEGRAMMENI | HAS_ACCENT,
    0x03A9 | HAS_VOWEL | HAS_YPOGEGRAMMENI | HAS_ACCENT,
    0x03A9 | HAS_VOWEL | HAS_YPOGEGRAMMENI | HAS_ACCENT,
    0x03A9 | HAS_VOWEL | HAS_YPOGEGRAMMENI | HAS_ACCENT,
    0x0391 | HAS_VOWEL,
    0x0391 | HAS_VOWEL,
    0x0391 | HAS_VOWEL | HAS_YPOGEGRAMMENI | HAS_ACCENT,
    0x0391 | HAS_VOWEL | HAS_YPOGEGRAMMENI,
    0x0391 | HAS_VOWEL | HAS_YPOGEGRAMMENI | HAS_ACCENT,
    0,
    0x0391 | HAS_VOWEL | HAS_ACCENT,
    0x0391 | HAS_VOWEL | HAS_YPOGEGRAMMENI | HAS_ACCENT,
    0x0391 | HAS_VOWEL,
    0x0391 | HAS_VOWEL,
    0x0391 | HAS_VOWEL | HAS_ACCENT,
    0x0391 | HAS_VOWEL | HAS_ACCENT,
    0x0391 | HAS_VOWEL | HAS_YPOGEGRAMMENI,
    0,
    0x0399 | HAS_VOWEL,
    0,
    0,
    0,
    0x0397 | HAS_VOWEL | HAS_YPOGEGRAMMENI | HAS_ACCENT,
    0x0397 | HAS_VOWEL | HAS_YPOGEGRAMMENI,
    0x0397 | HAS_VOWEL | HAS_YPOGEGRAMMENI | HAS_ACCENT,
    0,
    0x0397 | HAS_VOWEL | HAS_ACCENT,
    0x0397 | HAS_VOWEL | HAS_YPOGEGRAMMENI | HAS_ACCENT,
    0x0395 | HAS_VOWEL | HAS_ACCENT,
    0x0395 | HAS_VOWEL | HAS_ACCENT,
    0x0397 | HAS_VOWEL | HAS_ACCENT,
    0x0397 | HAS_VOWEL | HAS_ACCENT,
    0x0397 | HAS_VOWEL | HAS_YPOGEGRAMMENI,
    0,
    0,
    0,
    0x0399 | HAS_VOWEL,
    0x0399 | HAS_VOWEL,
    0x0399 | HAS_VOWEL | HAS_ACCENT | HAS_DIALYTIKA,
    0x0399 | HAS_VOWEL | HAS_ACCENT | HAS_DIALYTIKA,
    0,
    0,
    0x0399 | HAS_VOWEL | HAS_ACCENT,
    0x0399 | HAS_VOWEL | HAS_ACCENT | HAS_DIALYTIKA,
    0x0399 | HAS_VOWEL,
    0x0399 | HAS_VOWEL,
    0x0399 | HAS_VOWEL | HAS_ACCENT,
    0x0399 | HAS_VOWEL | HAS_ACCENT,
    0,
    0,
    0,
    0,
    0x03A5 | HAS_VOWEL,
    0x03A5 | HAS_VOWEL,
    0x03A5 | HAS_VOWEL | HAS_ACCENT | HAS_DIALYTIKA,
    0x03A5 | HAS_VOWEL | HAS_ACCENT | HAS_DIALYTIKA,
    0x03A1,
    0x03A1,
    0x03A5 | HAS_VOWEL | HAS_ACCENT,
    0x03A5 | HAS_VOWEL | HAS_ACCENT | HAS_DIALYTIKA,
    0x03A5 | HAS_VOWEL,
    0x03A5 | HAS_VOWEL,
    0x03A5 | HAS_VOWEL | HAS_ACCENT,
    0x03A5 | HAS_VOWEL | HAS_ACCENT,
    0x03A1,
    0,
    0,
    0,
    0,
    0,
    0x03A9 | HAS_VOWEL | HAS_YPOGEGRAMMENI | HAS_ACCENT,
    0x03A9 | HAS_VOWEL | HAS_YPOGEGRAMMENI,
    0x03A9 | HAS_VOWEL | HAS_YPOGEGRAMMENI | HAS_ACCENT,
    0,
    0x03A9 | HAS_VOWEL | HAS_ACCENT,
    0x03A9 | HAS_VOWEL | HAS_YPOGEGRAMMENI | HAS_ACCENT,
    0x039F | HAS_VOWEL | HAS_ACCENT,
    0x039F | HAS_VOWEL | HAS_ACCENT,
    0x03A9 | HAS_VOWEL | HAS_ACCENT,
    0x03A9 | HAS_VOWEL | HAS_ACCENT,
    0x03A9 | HAS_VOWEL | HAS_YPOGEGRAMMENI,
    0,
    0,
    0,
};

// U+2126 Ohm sign
ସ୍ଥିର ଅଚଳ uint16_t data2126 = 0x03A9 | HAS_VOWEL;

uint32_t getLetterData(UChar32 c) {
    ଯଦି (c < 0x370 || 0x2126 < c || (0x3ff < c && c < 0x1f00)) {
        ଫେରନ୍ତୁ 0;
    } ଅନ୍ୟଥା ଯଦି (c <= 0x3ff) {
        ଫେରନ୍ତୁ data0370[c - 0x370];
    } ଅନ୍ୟଥା ଯଦି (c <= 0x1fff) {
        ଫେରନ୍ତୁ data1F00[c - 0x1f00];
    } ଅନ୍ୟଥା ଯଦି (c == 0x2126) {
        ଫେରନ୍ତୁ data2126;
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ 0;
    }
}

uint32_t getDiacriticData(UChar32 c) {
    ବାଛ (c) {
    ସ୍ଥିତି 0x0300:  // varia
    ସ୍ଥିତି 0x0301:  // tonos = oxia
    ସ୍ଥିତି 0x0342:  // perispomeni
    ସ୍ଥିତି 0x0302:  // circumflex can look like perispomeni
    ସ୍ଥିତି 0x0303:  // tilde can look like perispomeni
    ସ୍ଥିତି 0x0311:  // inverted breve can look like perispomeni
        ଫେରନ୍ତୁ HAS_ACCENT;
    ସ୍ଥିତି 0x0308:  // dialytika = diaeresis
        ଫେରନ୍ତୁ HAS_COMBINING_DIALYTIKA;
    ସ୍ଥିତି 0x0344:  // dialytika tonos
        ଫେରନ୍ତୁ HAS_COMBINING_DIALYTIKA | HAS_ACCENT;
    ସ୍ଥିତି 0x0345:  // ypogegrammeni = iota subscript
        ଫେରନ୍ତୁ HAS_YPOGEGRAMMENI;
    ସ୍ଥିତି 0x0304:  // macron
    ସ୍ଥିତି 0x0306:  // breve
    ସ୍ଥିତି 0x0313:  // comma above
    ସ୍ଥିତି 0x0314:  // reversed comma above
    ସ୍ଥିତି 0x0343:  // koronis
        ଫେରନ୍ତୁ HAS_OTHER_GREEK_DIACRITIC;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        ଫେରନ୍ତୁ 0;
    }
}

UBool isFollowedByCasedLetter(ଅଚଳ char16_t *s, int32_t i, int32_t length) {
    ଯେତେବେଳେ (i < length) {
        UChar32 c;
        U16_NEXT(s, i, length, c);
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

/**
 * Greek string uppercasing with a state machine.
 * Probably simpler than a stateless function that has to figure out complex context-before
 * for each character.
 * TODO: Try to re-consolidate one way or another with the non-Greek function.
 */
int32_t toUpper(uint32_t options,
                char16_t *dest, int32_t destCapacity,
                ଅଚଳ char16_t *src, int32_t srcLength,
                Edits *edits,
                UErrorCode &errorCode) {
    int32_t destIndex=0;
    uint32_t state = 0;
    ପାଇଁ (int32_t i = 0; i < srcLength;) {
        int32_t nextIndex = i;
        UChar32 c;
        U16_NEXT(src, nextIndex, srcLength, c);
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
                data |= (state & AFTER_VOWEL_WITH_PRECOMPOSED_ACCENT) ? HAS_DIALYTIKA
                                                                      : HAS_COMBINING_DIALYTIKA;
            }
            int32_t ଅଙ୍କ_Ypogegrammeni = 0;  // Map each one to a trailing, spacing, capital iota.
            ଯଦି ((data & HAS_YPOGEGRAMMENI) != 0) {
                ଅଙ୍କ_Ypogegrammeni = 1;
            }
            ଅଚଳ UBool hasPrecomposedAccent = (data & HAS_ACCENT) != 0;
            // Skip combining diacritics after this Greek letter.
            ଯେତେବେଳେ (nextIndex < srcLength) {
                uint32_t diacriticData = getDiacriticData(src[nextIndex]);
                ଯଦି (diacriticData != 0) {
                    data |= diacriticData;
                    ଯଦି ((diacriticData & HAS_YPOGEGRAMMENI) != 0) {
                        ++ଅଙ୍କ_Ypogegrammeni;
                    }
                    ++nextIndex;
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
                change = src[i] != upper || ଅଙ୍କ_Ypogegrammeni > 0;
                int32_t i2 = i + 1;
                ଯଦି ((data & HAS_EITHER_DIALYTIKA) != 0) {
                    change |= i2 >= nextIndex || src[i2] != 0x308;
                    ++i2;
                }
                ଯଦି (addTonos) {
                    change |= i2 >= nextIndex || src[i2] != 0x301;
                    ++i2;
                }
                int32_t oldLength = nextIndex - i;
                int32_t newLength = (i2 - i) + ଅଙ୍କ_Ypogegrammeni;
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
                destIndex = appendUChar(dest, destIndex, destCapacity, static_cast<char16_t>(upper));
                ଯଦି (destIndex >= 0 && (data & HAS_EITHER_DIALYTIKA) != 0) {
                    destIndex=appendUChar(dest, destIndex, destCapacity, 0x308);  // restore or add a dialytika
                }
                ଯଦି (destIndex >= 0 && addTonos) {
                    destIndex=appendUChar(dest, destIndex, destCapacity, 0x301);
                }
                ଯେତେବେଳେ (destIndex >= 0 && ଅଙ୍କ_Ypogegrammeni > 0) {
                    destIndex=appendUChar(dest, destIndex, destCapacity, 0x399);
                    --ଅଙ୍କ_Ypogegrammeni;
                }
                ଯଦି(destIndex<0) {
                    errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
                    ଫେରନ୍ତୁ 0;
                }
            }
        } ଅନ୍ୟଥା {
            ଅଚଳ char16_t *s;
            c=ucase_toFullUpper(c, nullptr, nullptr, &s, UCASE_LOC_GREEK);
            destIndex = appendResult(dest, destIndex, destCapacity, c, s,
                                     nextIndex - i, options, edits);
            ଯଦି (destIndex < 0) {
                errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
                ଫେରନ୍ତୁ 0;
            }
        }
        i = nextIndex;
        state = nextState;
    }

    ଫେରନ୍ତୁ destIndex;
}

}  // namespace GreekUpper
U_NAMESPACE_END

/* functions available in the common library (for unistr_case.cpp) */

U_CFUNC int32_t U_CALLCONV
ustrcase_internalToLower(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_UNUSED
                         char16_t *dest, int32_t destCapacity,
                         ଅଚଳ char16_t *src, int32_t srcLength,
                         icu::Edits *edits,
                         UErrorCode &errorCode) {
    UCaseContext csc=UCASECONTEXT_INITIALIZER;
    csc.p=(ଶୂନ୍ୟ *)src;
    csc.limit=srcLength;
    int32_t destIndex = toLower(
        caseLocale, options,
        dest, destCapacity,
        src, &csc, 0, srcLength,
        edits, errorCode);
    ଫେରନ୍ତୁ checkOverflowAndEditsError(destIndex, destCapacity, edits, errorCode);
}

U_CFUNC int32_t U_CALLCONV
ustrcase_internalToUpper(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_UNUSED
                         char16_t *dest, int32_t destCapacity,
                         ଅଚଳ char16_t *src, int32_t srcLength,
                         icu::Edits *edits,
                         UErrorCode &errorCode) {
    int32_t destIndex;
    ଯଦି (caseLocale == UCASE_LOC_GREEK) {
        destIndex = GreekUpper::toUpper(options, dest, destCapacity,
                                        src, srcLength, edits, errorCode);
    } ଅନ୍ୟଥା {
        UCaseContext csc=UCASECONTEXT_INITIALIZER;
        csc.p=(ଶୂନ୍ୟ *)src;
        csc.limit=srcLength;
        destIndex = toUpper(
            caseLocale, options,
            dest, destCapacity,
            src, &csc, srcLength,
            edits, errorCode);
    }
    ଫେରନ୍ତୁ checkOverflowAndEditsError(destIndex, destCapacity, edits, errorCode);
}

U_CFUNC int32_t U_CALLCONV
ustrcase_internalFold(int32_t /* caseLocale */, uint32_t options, UCASEMAP_BREAK_ITERATOR_UNUSED
                      char16_t *dest, int32_t destCapacity,
                      ଅଚଳ char16_t *src, int32_t srcLength,
                      icu::Edits *edits,
                      UErrorCode &errorCode) {
    int32_t destIndex = toLower(
        -1, options,
        dest, destCapacity,
        src, nullptr, 0, srcLength,
        edits, errorCode);
    ଫେରନ୍ତୁ checkOverflowAndEditsError(destIndex, destCapacity, edits, errorCode);
}

U_CFUNC int32_t
ustrcase_map(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_PARAM
             char16_t *dest, int32_t destCapacity,
             ଅଚଳ char16_t *src, int32_t srcLength,
             UStringCaseMapper *stringCaseMapper,
             icu::Edits *edits,
             UErrorCode &errorCode) {
    int32_t destLength;

    /* check argument values */
    ଯଦି(U_FAILURE(errorCode)) {
        ଫେରନ୍ତୁ 0;
    }
    ଯଦି( destCapacity<0 ||
        (dest==nullptr && destCapacity>0) ||
        src==nullptr ||
        srcLength<-1
    ) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ 0;
    }

    /* get the string length */
    ଯଦି(srcLength==-1) {
        srcLength=u_strlen(src);
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
    destLength=stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                                dest, destCapacity, src, srcLength, edits, errorCode);
    ଫେରନ୍ତୁ u_terminateUChars(dest, destCapacity, destLength, &errorCode);
}

U_CFUNC int32_t
ustrcase_mapWithOverlap(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_PARAM
                        char16_t *dest, int32_t destCapacity,
                        ଅଚଳ char16_t *src, int32_t srcLength,
                        UStringCaseMapper *stringCaseMapper,
                        UErrorCode &errorCode) {
    char16_t buffer[300];
    char16_t *temp;

    int32_t destLength;

    /* check argument values */
    ଯଦି(U_FAILURE(errorCode)) {
        ଫେରନ୍ତୁ 0;
    }
    ଯଦି( destCapacity<0 ||
        (dest==nullptr && destCapacity>0) ||
        src==nullptr ||
        srcLength<-1
    ) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ 0;
    }

    /* get the string length */
    ଯଦି(srcLength==-1) {
        srcLength=u_strlen(src);
    }

    /* check for overlapping source and destination */
    ଯଦି( dest!=nullptr &&
        ((src>=dest && src<(dest+destCapacity)) ||
         (dest>=src && dest<(src+srcLength)))
    ) {
        /* overlap: provide a temporary destination buffer and later copy the result */
        ଯଦି(destCapacity<=UPRV_LENGTHOF(buffer)) {
            /* the stack buffer is large enough */
            temp=buffer;
        } ଅନ୍ୟଥା {
            /* allocate a buffer */
            temp=(char16_t *)uprv_malloc(destCapacity*U_SIZEOF_UCHAR);
            ଯଦି(temp==nullptr) {
                errorCode=U_MEMORY_ALLOCATION_ERROR;
                ଫେରନ୍ତୁ 0;
            }
        }
    } ଅନ୍ୟଥା {
        temp=dest;
    }

    destLength=stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                                temp, destCapacity, src, srcLength, nullptr, errorCode);
    ଯଦି(temp!=dest) {
        /* copy the result string to the destination buffer */
        ଯଦି (U_SUCCESS(errorCode) && 0 < destLength && destLength <= destCapacity) {
            u_memmove(dest, temp, destLength);
        }
        ଯଦି(temp!=buffer) {
            uprv_free(temp);
        }
    }

    ଫେରନ୍ତୁ u_terminateUChars(dest, destCapacity, destLength, &errorCode);
}

/* public API functions */

U_CAPI int32_t U_EXPORT2
u_strFoldCase(char16_t *dest, int32_t destCapacity,
              ଅଚଳ char16_t *src, int32_t srcLength,
              uint32_t options,
              UErrorCode *pErrorCode) {
    ଫେରନ୍ତୁ ustrcase_mapWithOverlap(
        UCASE_LOC_ମୂଳ, options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, destCapacity,
        src, srcLength,
        ustrcase_internalFold, *pErrorCode);
}

U_NAMESPACE_BEGIN

int32_t CaseMap::fold(
        uint32_t options,
        ଅଚଳ char16_t *src, int32_t srcLength,
        char16_t *dest, int32_t destCapacity, Edits *edits,
        UErrorCode &errorCode) {
    ଫେରନ୍ତୁ ustrcase_map(
        UCASE_LOC_ମୂଳ, options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, destCapacity,
        src, srcLength,
        ustrcase_internalFold, edits, errorCode);
}

U_NAMESPACE_END

/* case-insensitive string comparisons -------------------------------------- */

/*
 * This function is a copy of unorm_cmpEquivFold() minus the parts for
 * canonical equivalence.
 * Keep the functions in sync, and see there for how this works.
 * The duplication is for modularization:
 * It makes caseless (but not canonical caseless) matches independent of
 * the normalization code.
 */

/* stack element for previous-level source/decomposition pointers */
ସଂରଚନା CmpEquivLevel {
    ଅଚଳ char16_t *start, *s, *limit;
};
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା CmpEquivLevel CmpEquivLevel;

/**
 * Internal implementation code comparing string with case fold.
 * This function is called from u_strcmpFold() and u_caseInsensitivePrefixMatch().
 *
 * @param s1            input string 1
 * @param length1       length of string 1, or -1 (NUL terminated)
 * @param s2            input string 2
 * @param length2       length of string 2, or -1 (NUL terminated)
 * @param options       compare options
 * @param matchLen1     (output) length of partial prefix match in s1
 * @param matchLen2     (output) length of partial prefix match in s2
 * @param pErrorCode    receives error status
 * @return The result of comparison
 */
ସ୍ଥିର int32_t _cmpFold(
            ଅଚଳ char16_t *s1, int32_t length1,
            ଅଚଳ char16_t *s2, int32_t length2,
            uint32_t options,
            int32_t *matchLen1, int32_t *matchLen2,
            UErrorCode *pErrorCode) {
    int32_t cmpRes = 0;

    /* current-level start/limit - s1/s2 as current */
    ଅଚଳ char16_t *start1, *start2, *limit1, *limit2;

    /* points to the original start address */
    ଅଚଳ char16_t *org1, *org2;

    /* points to the end of match + 1 */
    ଅଚଳ char16_t *m1, *m2;

    /* case folding variables */
    ଅଚଳ char16_t *p;
    int32_t length;

    /* stacks of previous-level start/current/limit */
    CmpEquivLevel stack1[2], stack2[2];

    /* case folding buffers, only use current-level start/limit */
    char16_t fold1[UCASE_MAX_STRING_LENGTH+1], fold2[UCASE_MAX_STRING_LENGTH+1];

    /* track which is the current level per string */
    int32_t level1, level2;

    /* current code units, and code points for lookups */
    UChar32 c1, c2, cp1, cp2;

    /* no argument error checking because this itself is not an API */

    /*
     * assume that at least the option U_COMPARE_IGNORE_CASE is set
     * otherwise this function would have to behave exactly as uprv_strCompare()
     */
    ଯଦି(U_FAILURE(*pErrorCode)) {
        ଫେରନ୍ତୁ 0;
    }

    /* initialize */
    ଯଦି(matchLen1) {
        U_ASSERT(matchLen2 !=nullptr);
        *matchLen1=0;
        *matchLen2=0;
    }

    start1=m1=org1=s1;
    ଯଦି(length1==-1) {
        limit1=nullptr;
    } ଅନ୍ୟଥା {
        limit1=s1+length1;
    }

    start2=m2=org2=s2;
    ଯଦି(length2==-1) {
        limit2=nullptr;
    } ଅନ୍ୟଥା {
        limit2=s2+length2;
    }

    level1=level2=0;
    c1=c2=-1;

    /* comparison loop */
    ପାଇଁ(;;) {
        /*
         * here a code unit value of -1 means "get another code unit"
         * below it will mean "this source is finished"
         */

        ଯଦି(c1<0) {
            /* get next code unit from string 1, post-increment */
            ପାଇଁ(;;) {
                ଯଦି(s1==limit1 || ((c1=*s1)==0 && (limit1==nullptr || (options&_STRNCMP_STYLE)))) {
                    ଯଦି(level1==0) {
                        c1=-1;
                        ଭାଙ୍ଗ;
                    }
                } ଅନ୍ୟଥା {
                    ++s1;
                    ଭାଙ୍ଗ;
                }

                /* reached end of level buffer, pop one level */
                କର {
                    --level1;
                    start1=stack1[level1].start;    /*Not uninitialized*/
                } ଯେତେବେଳେ(start1==nullptr);
                s1=stack1[level1].s;                /*Not uninitialized*/
                limit1=stack1[level1].limit;        /*Not uninitialized*/
            }
        }

        ଯଦି(c2<0) {
            /* get next code unit from string 2, post-increment */
            ପାଇଁ(;;) {
                ଯଦି(s2==limit2 || ((c2=*s2)==0 && (limit2==nullptr || (options&_STRNCMP_STYLE)))) {
                    ଯଦି(level2==0) {
                        c2=-1;
                        ଭାଙ୍ଗ;
                    }
                } ଅନ୍ୟଥା {
                    ++s2;
                    ଭାଙ୍ଗ;
                }

                /* reached end of level buffer, pop one level */
                କର {
                    --level2;
                    start2=stack2[level2].start;    /*Not uninitialized*/
                } ଯେତେବେଳେ(start2==nullptr);
                s2=stack2[level2].s;                /*Not uninitialized*/
                limit2=stack2[level2].limit;        /*Not uninitialized*/
            }
        }

        /*
         * compare c1 and c2
         * either variable c1, c2 is -1 only if the corresponding string is finished
         */
        ଯଦି(c1==c2) {
            ଅଚଳ char16_t *next1, *next2;

            ଯଦି(c1<0) {
                cmpRes=0;   /* c1==c2==-1 indicating end of strings */
                ଭାଙ୍ଗ;
            }

            /*
             * Note: Move the match positions in both strings at the same time
             *      only when corresponding code point(s) in the original strings
             *      are fully consumed. For example, when comparing s1="Fust" and
             *      s2="Fu\u00dfball", s2[2] is folded into "ss", and s1[2] matches
             *      the first code point in the case-folded data. But the second "s"
             *      has no matching code point in s1, so this implementation returns
             *      2 as the prefix match length ("Fu").
             */
            next1=next2=nullptr;
            ଯଦି(level1==0) {
                next1=s1;
            } ଅନ୍ୟଥା ଯଦି(s1==limit1) {
                /* Note: This implementation only use a single level of stack.
                 *      If this code needs to be changed to use multiple levels
                 *      of stacks, the code above should check if the current
                 *      code is at the end of all stacks.
                 */
                U_ASSERT(level1==1);

                /* is s1 at the end of the current stack? */
                next1=stack1[0].s;
            }

            ଯଦି (next1!=nullptr) {
                ଯଦି(level2==0) {
                    next2=s2;
                } ଅନ୍ୟଥା ଯଦି(s2==limit2) {
                    U_ASSERT(level2==1);

                    /* is s2 at the end of the current stack? */
                    next2=stack2[0].s;
                }
                ଯଦି(next2!=nullptr) {
                    m1=next1;
                    m2=next2;
                }
            }
            c1=c2=-1;       /* make us fetch new code units */
            ଜାରି;
        } ଅନ୍ୟଥା ଯଦି(c1<0) {
            cmpRes=-1;      /* string 1 ends before string 2 */
            ଭାଙ୍ଗ;
        } ଅନ୍ୟଥା ଯଦି(c2<0) {
            cmpRes=1;       /* string 2 ends before string 1 */
            ଭାଙ୍ଗ;
        }
        /* c1!=c2 && c1>=0 && c2>=0 */

        /* get complete code points for c1, c2 for lookups if either is a surrogate */
        cp1=c1;
        ଯଦି(U_IS_SURROGATE(c1)) {
            char16_t c;

            ଯଦି(U_IS_SURROGATE_LEAD(c1)) {
                ଯଦି(s1!=limit1 && U16_IS_TRAIL(c=*s1)) {
                    /* advance ++s1; only below if cp1 decomposes/case-folds */
                    cp1=U16_GET_SUPPLEMENTARY(c1, c);
                }
            } ଅନ୍ୟଥା /* isTrail(c1) */ {
                ଯଦି(start1<=(s1-2) && U16_IS_LEAD(c=*(s1-2))) {
                    cp1=U16_GET_SUPPLEMENTARY(c, c1);
                }
            }
        }

        cp2=c2;
        ଯଦି(U_IS_SURROGATE(c2)) {
            char16_t c;

            ଯଦି(U_IS_SURROGATE_LEAD(c2)) {
                ଯଦି(s2!=limit2 && U16_IS_TRAIL(c=*s2)) {
                    /* advance ++s2; only below if cp2 decomposes/case-folds */
                    cp2=U16_GET_SUPPLEMENTARY(c2, c);
                }
            } ଅନ୍ୟଥା /* isTrail(c2) */ {
                ଯଦି(start2<=(s2-2) && U16_IS_LEAD(c=*(s2-2))) {
                    cp2=U16_GET_SUPPLEMENTARY(c, c2);
                }
            }
        }

        /*
         * go down one level for each string
         * continue with the main loop as soon as there is a real change
         */

        ଯଦି( level1==0 &&
            (length = ucase_toFullFolding(cp1, &p, options)) >= 0
        ) {
            /* cp1 case-folds to the code point "length" or to p[length] */
            ଯଦି(U_IS_SURROGATE(c1)) {
                ଯଦି(U_IS_SURROGATE_LEAD(c1)) {
                    /* advance beyond source surrogate pair if it case-folds */
                    ++s1;
                } ଅନ୍ୟଥା /* isTrail(c1) */ {
                    /*
                     * we got a supplementary code point when hitting its trail surrogate,
                     * therefore the lead surrogate must have been the same as in the other string;
                     * compare this decomposition with the lead surrogate in the other string
                     * remember that this simulates bulk text replacement:
                     * the decomposition would replace the entire code point
                     */
                    --s2;
                    --m2;
                    c2=*(s2-1);
                }
            }

            /* push current level pointers */
            stack1[0].start=start1;
            stack1[0].s=s1;
            stack1[0].limit=limit1;
            ++level1;

            /* copy the folding result to fold1[] */
            ଯଦି(length<=UCASE_MAX_STRING_LENGTH) {
                u_memcpy(fold1, p, length);
            } ଅନ୍ୟଥା {
                int32_t i=0;
                U16_APPEND_UNSAFE(fold1, i, length);
                length=i;
            }

            /* set next level pointers to case folding */
            start1=s1=fold1;
            limit1=fold1+length;

            /* get ready to read from decomposition, continue with loop */
            c1=-1;
            ଜାରି;
        }

        ଯଦି( level2==0 &&
            (length = ucase_toFullFolding(cp2, &p, options)) >= 0
        ) {
            /* cp2 case-folds to the code point "length" or to p[length] */
            ଯଦି(U_IS_SURROGATE(c2)) {
                ଯଦି(U_IS_SURROGATE_LEAD(c2)) {
                    /* advance beyond source surrogate pair if it case-folds */
                    ++s2;
                } ଅନ୍ୟଥା /* isTrail(c2) */ {
                    /*
                     * we got a supplementary code point when hitting its trail surrogate,
                     * therefore the lead surrogate must have been the same as in the other string;
                     * compare this decomposition with the lead surrogate in the other string
                     * remember that this simulates bulk text replacement:
                     * the decomposition would replace the entire code point
                     */
                    --s1;
                    --m2;
                    c1=*(s1-1);
                }
            }

            /* push current level pointers */
            stack2[0].start=start2;
            stack2[0].s=s2;
            stack2[0].limit=limit2;
            ++level2;

            /* copy the folding result to fold2[] */
            ଯଦି(length<=UCASE_MAX_STRING_LENGTH) {
                u_memcpy(fold2, p, length);
            } ଅନ୍ୟଥା {
                int32_t i=0;
                U16_APPEND_UNSAFE(fold2, i, length);
                length=i;
            }

            /* set next level pointers to case folding */
            start2=s2=fold2;
            limit2=fold2+length;

            /* get ready to read from decomposition, continue with loop */
            c2=-1;
            ଜାରି;
        }

        /*
         * no decomposition/case folding, max level for both sides:
         * return difference result
         *
         * code point order comparison must not just return cp1-cp2
         * because when single surrogates are present then the surrogate pairs
         * that formed cp1 and cp2 may be from different string indexes
         *
         * example: { d800 d800 dc01 } vs. { d800 dc00 }, compare at second code units
         * c1=d800 cp1=10001 c2=dc00 cp2=10000
         * cp1-cp2>0 but c1-c2<0 and in fact in UTF-32 it is { d800 10001 } < { 10000 }
         *
         * therefore, use same fix-up as in ustring.c/uprv_strCompare()
         * except: uprv_strCompare() fetches c=*s while this functions fetches c=*s++
         * so we have slightly different pointer/start/limit comparisons here
         */

        ଯଦି(c1>=0xd800 && c2>=0xd800 && (options&U_COMPARE_CODE_POINT_ORDER)) {
            /* subtract 0x2800 from BMP code points to make them smaller than supplementary ones */
            ଯଦି(
                (c1<=0xdbff && s1!=limit1 && U16_IS_TRAIL(*s1)) ||
                (U16_IS_TRAIL(c1) && start1!=(s1-1) && U16_IS_LEAD(*(s1-2)))
            ) {
                /* part of a surrogate pair, leave >=d800 */
            } ଅନ୍ୟଥା {
                /* BMP code point - may be surrogate code point - make <d800 */
                c1-=0x2800;
            }

            ଯଦି(
                (c2<=0xdbff && s2!=limit2 && U16_IS_TRAIL(*s2)) ||
                (U16_IS_TRAIL(c2) && start2!=(s2-1) && U16_IS_LEAD(*(s2-2)))
            ) {
                /* part of a surrogate pair, leave >=d800 */
            } ଅନ୍ୟଥା {
                /* BMP code point - may be surrogate code point - make <d800 */
                c2-=0x2800;
            }
        }

        cmpRes=c1-c2;
        ଭାଙ୍ଗ;
    }

    ଯଦି(matchLen1) {
        *matchLen1=static_cast<int32_t>(m1-org1);
        *matchLen2=static_cast<int32_t>(m2-org2);
    }
    ଫେରନ୍ତୁ cmpRes;
}

/* internal function */
U_CFUNC int32_t
u_strcmpFold(ଅଚଳ char16_t *s1, int32_t length1,
             ଅଚଳ char16_t *s2, int32_t length2,
             uint32_t options,
             UErrorCode *pErrorCode) {
    ଫେରନ୍ତୁ _cmpFold(s1, length1, s2, length2, options, nullptr, nullptr, pErrorCode);
}

/* public API functions */

U_CAPI int32_t U_EXPORT2
u_strCaseCompare(ଅଚଳ char16_t *s1, int32_t length1,
                 ଅଚଳ char16_t *s2, int32_t length2,
                 uint32_t options,
                 UErrorCode *pErrorCode) {
    /* argument checking */
    ଯଦି (pErrorCode == nullptr || U_FAILURE(*pErrorCode)) {
        ଫେରନ୍ତୁ 0;
    }
    ଯଦି(s1==nullptr || length1<-1 || s2==nullptr || length2<-1) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ 0;
    }
    ଫେରନ୍ତୁ u_strcmpFold(s1, length1, s2, length2,
                        options|U_COMPARE_IGNORE_CASE,
                        pErrorCode);
}

U_CAPI int32_t U_EXPORT2
u_strcasecmp(ଅଚଳ char16_t *s1, ଅଚଳ char16_t *s2, uint32_t options) {
    UErrorCode errorCode=U_ZERO_ERROR;
    ଫେରନ୍ତୁ u_strcmpFold(s1, -1, s2, -1,
                        options|U_COMPARE_IGNORE_CASE,
                        &errorCode);
}

U_CAPI int32_t U_EXPORT2
u_memcasecmp(ଅଚଳ char16_t *s1, ଅଚଳ char16_t *s2, int32_t length, uint32_t options) {
    UErrorCode errorCode=U_ZERO_ERROR;
    ଫେରନ୍ତୁ u_strcmpFold(s1, length, s2, length,
                        options|U_COMPARE_IGNORE_CASE,
                        &errorCode);
}

U_CAPI int32_t U_EXPORT2
u_strncasecmp(ଅଚଳ char16_t *s1, ଅଚଳ char16_t *s2, int32_t n, uint32_t options) {
    UErrorCode errorCode=U_ZERO_ERROR;
    ଫେରନ୍ତୁ u_strcmpFold(s1, n, s2, n,
                        options|(U_COMPARE_IGNORE_CASE|_STRNCMP_STYLE),
                        &errorCode);
}

/* internal API - detect length of shared prefix */
U_CAPI ଶୂନ୍ୟ
u_caseInsensitivePrefixMatch(ଅଚଳ char16_t *s1, int32_t length1,
                             ଅଚଳ char16_t *s2, int32_t length2,
                             uint32_t options,
                             int32_t *matchLen1, int32_t *matchLen2,
                             UErrorCode *pErrorCode) {
    _cmpFold(s1, length1, s2, length2, options,
        matchLen1, matchLen2, pErrorCode);
}
