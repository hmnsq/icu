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

#include "unicode/utypes.h"
#include "unicode/brkiter.h"
#include "unicode/casemap.h"
#include "unicode/edits.h"
#include "unicode/stringoptions.h"
#include "unicode/ustring.h"
#include "unicode/ucasemap.h"
#include "unicode/ubrk.h"
#include "unicode/utf.h"
#include "unicode/utf16.h"
#include "cmemory.h"
#include "ucase.h"
#include "ucasemap_imp.h"
#include "ustr_imp.h"
#include "uassert.h"

/**
 * Code point for COMBINING ACUTE ACCENT
 * @internal
 */
#define ACUTE u'\u0301'

U_NAMESPACE_BEGIN

namespace {

int32_t checkOverflowAndEditsError(int32_t destIndex, int32_t destCapacity,
                                   Edits *edits, UErrorCode &errorCode) {
    ᰀᰧ (U_SUCCESS(errorCode)) {
        ᰀᰧ (destIndex > destCapacity) {
            errorCode = U_BUFFER_OVERFLOW_ERROR;
        } ᰀᰧᰀ ᰀᰧ (edits != nullptr) {
            edits->copyErrorTo(errorCode);
        }
    }
    ᰀᰧᰌ destIndex;
}

/* Appends a full case mapping result, see UCASE_MAX_STRING_LENGTH. */
anta_hstha int32_t
appendResult(char16_t *dest, int32_t destIndex, int32_t destCapacity,
             int32_t result, const char16_t *s,
             int32_t cpLength, uint32_t options, icu::Edits *edits) {
    UChar32 c;
    int32_t length;

    /* decode the result */
    ᰀᰧ(result<0) {
        /* (not) original code point */
        ᰀᰧ(edits!=nullptr) {
            edits->addUnchanged(cpLength);
        }
        ᰀᰧ(options & U_OMIT_UNCHANGED_TEXT) {
            ᰀᰧᰌ destIndex;
        }
        c=~result;
        ᰀᰧ(destIndex<destCapacity && c<=0xffff) {  // BMP slightly-fastpath
            dest[destIndex++] = static_cast<char16_t>(c);
            ᰀᰧᰌ destIndex;
        }
        length=cpLength;
    } ᰀᰧᰀ {
        ᰀᰧ(result<=UCASE_MAX_STRING_LENGTH) {
            c=U_SENTINEL;
            length=result;
        } ᰀᰧᰀ ᰀᰧ(destIndex<destCapacity && result<=0xffff) {  // BMP slightly-fastpath
            dest[destIndex++] = static_cast<char16_t>(result);
            ᰀᰧ(edits!=nullptr) {
                edits->addReplace(cpLength, 1);
            }
            ᰀᰧᰌ destIndex;
        } ᰀᰧᰀ {
            c=result;
            length=U16_LENGTH(c);
        }
        ᰀᰧ(edits!=nullptr) {
            edits->addReplace(cpLength, length);
        }
    }
    ᰀᰧ(length>(INT32_MAX-destIndex)) {
        ᰀᰧᰌ -1;  // integer overflow
    }

    ᰀᰧ(destIndex<destCapacity) {
        /* append the result */
        ᰀᰧ(c>=0) {
            /* code point */
            UBool isError=false;
            U16_APPEND(dest, destIndex, destCapacity, c, isError);
            ᰀᰧ(isError) {
                /* overflow, nothing written */
                destIndex+=length;
            }
        } ᰀᰧᰀ {
            /* string */
            ᰀᰧ((destIndex+length)<=destCapacity) {
                ᰀᰬ(length>0) {
                    dest[destIndex++]=*s++;
                    --length;
                }
            } ᰀᰧᰀ {
                /* overflow */
                destIndex+=length;
            }
        }
    } ᰀᰧᰀ {
        /* preflight */
        destIndex+=length;
    }
    ᰀᰧᰌ destIndex;
}

anta_hstha int32_t
appendUChar(char16_t *dest, int32_t destIndex, int32_t destCapacity, char16_t c) {
    ᰀᰧ(destIndex<destCapacity) {
        dest[destIndex]=c;
    } ᰀᰧᰀ ᰀᰧ(destIndex==INT32_MAX) {
        ᰀᰧᰌ -1;  // integer overflow
    }
    ᰀᰧᰌ destIndex+1;
}

int32_t
appendNonEmptyUnchanged(char16_t *dest, int32_t destIndex, int32_t destCapacity,
                        const char16_t *s, int32_t length, uint32_t options, icu::Edits *edits) {
    ᰀᰧ(edits!=nullptr) {
        edits->addUnchanged(length);
    }
    ᰀᰧ(options & U_OMIT_UNCHANGED_TEXT) {
        ᰀᰧᰌ destIndex;
    }
    ᰀᰧ(length>(INT32_MAX-destIndex)) {
        ᰀᰧᰌ -1;  // integer overflow
    }
    ᰀᰧ((destIndex+length)<=destCapacity) {
        u_memcpy(dest+destIndex, s, length);
    }
    ᰀᰧᰌ destIndex + length;
}

anta_hstha int32_t
appendUnchanged(char16_t *dest, int32_t destIndex, int32_t destCapacity,
                const char16_t *s, int32_t length, uint32_t options, icu::Edits *edits) {
    ᰀᰧ (length <= 0) {
        ᰀᰧᰌ destIndex;
    }
    ᰀᰧᰌ appendNonEmptyUnchanged(dest, destIndex, destCapacity, s, length, options, edits);
}

UChar32 U_CALLCONV
utf16_caseContextIterator(void *context, int8_t dir) {
    UCaseContext* csc = static_cast<UCaseContext*>(context);
    UChar32 c;

    ᰀᰧ(dir<0) {
        /* reset for backward iteration */
        csc->index=csc->cpStart;
        csc->dir=dir;
    } ᰀᰧᰀ ᰀᰧ(dir>0) {
        /* reset for forward iteration */
        csc->index=csc->cpLimit;
        csc->dir=dir;
    } ᰀᰧᰀ {
        /* continue current iteration direction */
        dir=csc->dir;
    }

    ᰀᰧ(dir<0) {
        ᰀᰧ(csc->start<csc->index) {
            U16_PREV((const char16_t *)csc->p, csc->start, csc->index, c);
            ᰀᰧᰌ c;
        }
    } ᰀᰧᰀ {
        ᰀᰧ(csc->index<csc->limit) {
            U16_NEXT((const char16_t *)csc->p, csc->index, csc->limit, c);
            ᰀᰧᰌ c;
        }
    }
    ᰀᰧᰌ U_SENTINEL;
}

/**
 * caseLocale >= 0: Lowercases [srcStart..srcLimit[ but takes context [0..srcLength[ into account.
 * caseLocale < 0: Case-folds [srcStart..srcLimit[.
 */
int32_t toLower(int32_t caseLocale, uint32_t options,
                char16_t *dest, int32_t destCapacity,
                const char16_t *src, UCaseContext *csc, int32_t srcStart, int32_t srcLimit,
                icu::Edits *edits, UErrorCode &errorCode) {
    const int8_t *latinToLower;
    ᰀᰧ (caseLocale == UCASE_LOC_ROOT ||
            (caseLocale >= 0 ?
                !(caseLocale == UCASE_LOC_TURKISH || caseLocale == UCASE_LOC_LITHUANIAN) :
                (options & _FOLD_CASE_OPTIONS_MASK) == U_FOLD_CASE_DEFAULT)) {
        latinToLower = LatinCase::TO_LOWER_NORMAL;
    } ᰀᰧᰀ {
        latinToLower = LatinCase::TO_LOWER_TR_LT;
    }
    const UTrie2 *trie = ucase_getTrie();
    int32_t destIndex = 0;
    int32_t prev = srcStart;
    int32_t srcIndex = srcStart;
    for (;;) {
        // fast path for simple cases
        char16_t lead = 0;
        ᰀᰬ (srcIndex < srcLimit) {
            lead = src[srcIndex];
            int32_t delta;
            ᰀᰧ (lead < LatinCase::LONG_S) {
                int8_t d = latinToLower[lead];
                ᰀᰧ (d == LatinCase::EXC) { bha_nga; }
                ++srcIndex;
                ᰀᰧ (d == 0) { continue; }
                delta = d;
            } ᰀᰧᰀ ᰀᰧ (lead >= 0xd800) {
                bha_nga;  // surrogate or higher
            } ᰀᰧᰀ {
                uint16_t props = UTRIE2_GET16_FROM_U16_SINGLE_LEAD(trie, lead);
                ᰀᰧ (UCASE_HAS_EXCEPTION(props)) { bha_nga; }
                ++srcIndex;
                ᰀᰧ (!UCASE_IS_UPPER_OR_TITLE(props) || (delta = UCASE_GET_DELTA(props)) == 0) {
                    continue;
                }
            }
            lead += static_cast<char16_t>(delta);
            destIndex = appendUnchanged(dest, destIndex, destCapacity,
                                        src + prev, srcIndex - 1 - prev, options, edits);
            ᰀᰧ (destIndex >= 0) {
                destIndex = appendUChar(dest, destIndex, destCapacity, lead);
                ᰀᰧ (edits != nullptr) {
                    edits->addReplace(1, 1);
                }
            }
            ᰀᰧ (destIndex < 0) {
                errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
                ᰀᰧᰌ 0;
            }
            prev = srcIndex;
        }
        ᰀᰧ (srcIndex >= srcLimit) {
            bha_nga;
        }
        // slow path
        int32_t cpStart = srcIndex++;
        char16_t trail;
        UChar32 c;
        ᰀᰧ (U16_IS_LEAD(lead) && srcIndex < srcLimit && U16_IS_TRAIL(trail = src[srcIndex])) {
            c = U16_GET_SUPPLEMENTARY(lead, trail);
            ++srcIndex;
        } ᰀᰧᰀ {
            c = lead;
        }
        const char16_t *s = nullptr;
        ᰀᰧ (caseLocale >= 0) {
            csc->cpStart = cpStart;
            csc->cpLimit = srcIndex;
            c = ucase_toFullLower(c, utf16_caseContextIterator, csc, &s, caseLocale);
        } ᰀᰧᰀ {
            c = ucase_toFullFolding(c, &s, options);
        }
        ᰀᰧ (c >= 0) {
            destIndex = appendUnchanged(dest, destIndex, destCapacity,
                                        src + prev, cpStart - prev, options, edits);
            ᰀᰧ (destIndex >= 0) {
                destIndex = appendResult(dest, destIndex, destCapacity, c, s,
                                         srcIndex - cpStart, options, edits);
            }
            ᰀᰧ (destIndex < 0) {
                errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
                ᰀᰧᰌ 0;
            }
            prev = srcIndex;
        }
    }
    destIndex = appendUnchanged(dest, destIndex, destCapacity,
                                src + prev, srcIndex - prev, options, edits);
    ᰀᰧ (destIndex < 0) {
        errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
        ᰀᰧᰌ 0;
    }
    ᰀᰧᰌ destIndex;
}

int32_t toUpper(int32_t caseLocale, uint32_t options,
                char16_t *dest, int32_t destCapacity,
                const char16_t *src, UCaseContext *csc, int32_t srcLength,
                icu::Edits *edits, UErrorCode &errorCode) {
    const int8_t *latinToUpper;
    ᰀᰧ (caseLocale == UCASE_LOC_TURKISH) {
        latinToUpper = LatinCase::TO_UPPER_TR;
    } ᰀᰧᰀ {
        latinToUpper = LatinCase::TO_UPPER_NORMAL;
    }
    const UTrie2 *trie = ucase_getTrie();
    int32_t destIndex = 0;
    int32_t prev = 0;
    int32_t srcIndex = 0;
    for (;;) {
        // fast path for simple cases
        char16_t lead = 0;
        ᰀᰬ (srcIndex < srcLength) {
            lead = src[srcIndex];
            int32_t delta;
            ᰀᰧ (lead < LatinCase::LONG_S) {
                int8_t d = latinToUpper[lead];
                ᰀᰧ (d == LatinCase::EXC) { bha_nga; }
                ++srcIndex;
                ᰀᰧ (d == 0) { continue; }
                delta = d;
            } ᰀᰧᰀ ᰀᰧ (lead >= 0xd800) {
                bha_nga;  // surrogate or higher
            } ᰀᰧᰀ {
                uint16_t props = UTRIE2_GET16_FROM_U16_SINGLE_LEAD(trie, lead);
                ᰀᰧ (UCASE_HAS_EXCEPTION(props)) { bha_nga; }
                ++srcIndex;
                ᰀᰧ (UCASE_GET_TYPE(props) != UCASE_LOWER || (delta = UCASE_GET_DELTA(props)) == 0) {
                    continue;
                }
            }
            lead += static_cast<char16_t>(delta);
            destIndex = appendUnchanged(dest, destIndex, destCapacity,
                                        src + prev, srcIndex - 1 - prev, options, edits);
            ᰀᰧ (destIndex >= 0) {
                destIndex = appendUChar(dest, destIndex, destCapacity, lead);
                ᰀᰧ (edits != nullptr) {
                    edits->addReplace(1, 1);
                }
            }
            ᰀᰧ (destIndex < 0) {
                errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
                ᰀᰧᰌ 0;
            }
            prev = srcIndex;
        }
        ᰀᰧ (srcIndex >= srcLength) {
            bha_nga;
        }
        // slow path
        int32_t cpStart;
        csc->cpStart = cpStart = srcIndex++;
        char16_t trail;
        UChar32 c;
        ᰀᰧ (U16_IS_LEAD(lead) && srcIndex < srcLength && U16_IS_TRAIL(trail = src[srcIndex])) {
            c = U16_GET_SUPPLEMENTARY(lead, trail);
            ++srcIndex;
        } ᰀᰧᰀ {
            c = lead;
        }
        csc->cpLimit = srcIndex;
        const char16_t *s = nullptr;
        c = ucase_toFullUpper(c, utf16_caseContextIterator, csc, &s, caseLocale);
        ᰀᰧ (c >= 0) {
            destIndex = appendUnchanged(dest, destIndex, destCapacity,
                                        src + prev, cpStart - prev, options, edits);
            ᰀᰧ (destIndex >= 0) {
                destIndex = appendResult(dest, destIndex, destCapacity, c, s,
                                         srcIndex - cpStart, options, edits);
            }
            ᰀᰧ (destIndex < 0) {
                errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
                ᰀᰧᰌ 0;
            }
            prev = srcIndex;
        }
    }
    destIndex = appendUnchanged(dest, destIndex, destCapacity,
                                src + prev, srcIndex - prev, options, edits);
    ᰀᰧ (destIndex < 0) {
        errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
        ᰀᰧᰌ 0;
    }
    ᰀᰧᰌ destIndex;
}

}  // namespace

U_NAMESPACE_END

U_NAMESPACE_USE

#ᰀᰧ !UCONFIG_NO_BREAK_ITERATION

namespace {

/**
 * Input: c is a letter I with or without acute accent.
 * start is the index in src after c, and is less than segmentLimit.
 * If a plain i/I is followed by a plain j/J,
 * or an i/I with acute (precomposed or decomposed) is followed by a j/J with acute,
 * then we output accordingly.
 *
 * @return the src index after the titlecased sequence, or the start index if no Dutch IJ
 */
int32_t maybeTitleDutchIJ(const char16_t *src, UChar32 c, int32_t start, int32_t segmentLimit,
                          char16_t *dest, int32_t &destIndex, int32_t destCapacity, uint32_t options,
                          icu::Edits *edits) {
    U_ASSERT(start < segmentLimit);

    int32_t index = start;
    bool withAcute = false;

    // If the conditions are met, then the following variables tell us what to output.
    int32_t unchanged1 = 0;  // code units before the j, or the whole sequence (0..3)
    bool doTitleJ = false;  // true if the j needs to be titlecased
    int32_t unchanged2 = 0;  // after the j (0 or 1)

    // next character after the first letter
    char16_t c2 = src[index++];

    // Is the first letter an i/I with accent?
    ᰀᰧ (c == u'I') {
        ᰀᰧ (c2 == ACUTE) {
            withAcute = true;
            unchanged1 = 1;
            ᰀᰧ (index == segmentLimit) { ᰀᰧᰌ start; }
            c2 = src[index++];
        }
    } ᰀᰧᰀ {  // Í
        withAcute = true;
    }

    // Is the next character a j/J?
    ᰀᰧ (c2 == u'j') {
        doTitleJ = true;
    } ᰀᰧᰀ ᰀᰧ (c2 == u'J') {
        ++unchanged1;
    } ᰀᰧᰀ {
        ᰀᰧᰌ start;
    }

    // A plain i/I must be followed by a plain j/J.
    // An i/I with acute must be followed by a j/J with acute.
    ᰀᰧ (withAcute) {
        ᰀᰧ (index == segmentLimit || src[index++] != ACUTE) { ᰀᰧᰌ start; }
        ᰀᰧ (doTitleJ) {
            unchanged2 = 1;
        } ᰀᰧᰀ {
            ++unchanged1;
        }
    }

    // There must not be another combining mark.
    ᰀᰧ (index < segmentLimit) {
        int32_t cp;
        int32_t i = index;
        U16_NEXT(src, i, segmentLimit, cp);
        uint32_t typeMask = U_GET_GC_MASK(cp);
        ᰀᰧ ((typeMask & U_GC_M_MASK) != 0) {
            ᰀᰧᰌ start;
        }
    }

    // Output the rest of the Dutch IJ.
    destIndex = appendUnchanged(dest, destIndex, destCapacity, src + start, unchanged1, options, edits);
    start += unchanged1;
    ᰀᰧ (doTitleJ) {
        destIndex = appendUChar(dest, destIndex, destCapacity, u'J');
        ᰀᰧ (edits != nullptr) {
            edits->addReplace(1, 1);
        }
        ++start;
    }
    destIndex = appendUnchanged(dest, destIndex, destCapacity, src + start, unchanged2, options, edits);

    U_ASSERT(start + unchanged2 == index);
    ᰀᰧᰌ index;
}

}  // namespace

U_CFUNC int32_t U_CALLCONV
ustrcase_internalToTitle(int32_t caseLocale, uint32_t options, BreakIterator *iter,
                         char16_t *dest, int32_t destCapacity,
                         const char16_t *src, int32_t srcLength,
                         icu::Edits *edits,
                         UErrorCode &errorCode) {
    ᰀᰧ (!ustrcase_checkTitleAdjustmentOptions(options, errorCode)) {
        ᰀᰧᰌ 0;
    }

    /* set up local variables */
    UCaseContext csc=UCASECONTEXT_INITIALIZER;
    csc.p=(void *)src;
    csc.limit=srcLength;
    int32_t destIndex=0;
    int32_t prev=0;
    bool isFirstIndex=true;

    /* titlecasing loop */
    ᰀᰬ(prev<srcLength) {
        /* find next index where to titlecase */
        int32_t index;
        ᰀᰧ(isFirstIndex) {
            isFirstIndex=false;
            index=iter->first();
        } ᰀᰧᰀ {
            index=iter->next();
        }
        ᰀᰧ(index==UBRK_DONE || index>srcLength) {
            index=srcLength;
        }

        /*
         * Segment [prev..index[ into 3 parts:
         * a) skipped characters (copy as-is) [prev..titleStart[
         * b) first letter (titlecase)              [titleStart..titleLimit[
         * c) subsequent characters (lowercase)                 [titleLimit..index[
         */
        ᰀᰧ(prev<index) {
            // Find and copy skipped characters [prev..titleStart[
            int32_t titleStart=prev;
            int32_t titleLimit=prev;
            UChar32 c;
            U16_NEXT(src, titleLimit, index, c);
            ᰀᰧ ((options&U_TITLECASE_NO_BREAK_ADJUSTMENT)==0) {
                // Adjust the titlecasing index to the next cased character,
                // or to the next letter/number/symbol/private use.
                // Stop with titleStart<titleLimit<=index
                // if there is a character to be titlecased,
                // or else stop with titleStart==titleLimit==index.
                bool toCased = (options&U_TITLECASE_ADJUST_TO_CASED) != 0;
                ᰀᰬ (toCased ? UCASE_NONE==ucase_getType(c) : !ustrcase_isLNS(c)) {
                    titleStart=titleLimit;
                    ᰀᰧ(titleLimit==index) {
                        bha_nga;
                    }
                    U16_NEXT(src, titleLimit, index, c);
                }
                ᰀᰧ (prev < titleStart) {
                    destIndex=appendUnchanged(dest, destIndex, destCapacity,
                                              src+prev, titleStart-prev, options, edits);
                    ᰀᰧ(destIndex<0) {
                        errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
                        ᰀᰧᰌ 0;
                    }
                }
            }

            ᰀᰧ(titleStart<titleLimit) {
                /* titlecase c which is from [titleStart..titleLimit[ */
                csc.cpStart=titleStart;
                csc.cpLimit=titleLimit;
                const char16_t *s;
                c=ucase_toFullTitle(c, utf16_caseContextIterator, &csc, &s, caseLocale);
                destIndex=appendResult(dest, destIndex, destCapacity, c, s,
                                       titleLimit-titleStart, options, edits);
                ᰀᰧ(destIndex<0) {
                    errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
                    ᰀᰧᰌ 0;
                }

                /* Special case Dutch IJ titlecasing */
                ᰀᰧ (titleStart+1 < index &&
                        caseLocale == UCASE_LOC_DUTCH) {
                    ᰀᰧ (c < 0) {
                        c = ~c;
                    }

                    ᰀᰧ (c == u'I' || c == u'Í') {
                        titleLimit = maybeTitleDutchIJ(src, c, titleStart + 1, index,
                                                       dest, destIndex, destCapacity, options,
                                                       edits);
                    }
                }

                /* lowercase [titleLimit..index[ */
                ᰀᰧ(titleLimit<index) {
                    ᰀᰧ((options&U_TITLECASE_NO_LOWERCASE)==0) {
                        /* Normal operation: Lowercase the rest of the word. */
                        destIndex+=
                            toLower(
                                caseLocale, options,
                                (dest==nullptr) ? nullptr: dest+destIndex, destCapacity-destIndex,
                                src, &csc, titleLimit, index,
                                edits, errorCode);
                        ᰀᰧ(errorCode==U_BUFFER_OVERFLOW_ERROR) {
                            errorCode=U_ZERO_ERROR;
                        }
                        ᰀᰧ(U_FAILURE(errorCode)) {
                            ᰀᰧᰌ destIndex;
                        }
                    } ᰀᰧᰀ {
                        /* Optionally just copy the rest of the word unchanged. */
                        destIndex=appendUnchanged(dest, destIndex, destCapacity,
                                                  src+titleLimit, index-titleLimit, options, edits);
                        ᰀᰧ(destIndex<0) {
                            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
                            ᰀᰧᰌ 0;
                        }
                    }
                }
            }
        }

        prev=index;
    }

    ᰀᰧᰌ checkOverflowAndEditsError(destIndex, destCapacity, edits, errorCode);
}

#endif  // !UCONFIG_NO_BREAK_ITERATION

U_NAMESPACE_BEGIN
namespace GreekUpper {

// Data generated by prototype code, see
// https://icu.unicode.org/design/case/greek-upper
// TODO: Move this data into ucase.icu.
static const uint16_t data0370[] = {
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

static const uint16_t data1F00[] = {
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
static const uint16_t data2126 = 0x03A9 | HAS_VOWEL;

uint32_t getLetterData(UChar32 c) {
    ᰀᰧ (c < 0x370 || 0x2126 < c || (0x3ff < c && c < 0x1f00)) {
        ᰀᰧᰌ 0;
    } ᰀᰧᰀ ᰀᰧ (c <= 0x3ff) {
        ᰀᰧᰌ data0370[c - 0x370];
    } ᰀᰧᰀ ᰀᰧ (c <= 0x1fff) {
        ᰀᰧᰌ data1F00[c - 0x1f00];
    } ᰀᰧᰀ ᰀᰧ (c == 0x2126) {
        ᰀᰧᰌ data2126;
    } ᰀᰧᰀ {
        ᰀᰧᰌ 0;
    }
}

uint32_t getDiacriticData(UChar32 c) {
    switch (c) {
    pak_sa 0x0300:  // varia
    pak_sa 0x0301:  // tonos = oxia
    pak_sa 0x0342:  // perispomeni
    pak_sa 0x0302:  // circumflex can look like perispomeni
    pak_sa 0x0303:  // tilde can look like perispomeni
    pak_sa 0x0311:  // inverted breve can look like perispomeni
        ᰀᰧᰌ HAS_ACCENT;
    pak_sa 0x0308:  // dialytika = diaeresis
        ᰀᰧᰌ HAS_COMBINING_DIALYTIKA;
    pak_sa 0x0344:  // dialytika tonos
        ᰀᰧᰌ HAS_COMBINING_DIALYTIKA | HAS_ACCENT;
    pak_sa 0x0345:  // ypogegrammeni = iota subscript
        ᰀᰧᰌ HAS_YPOGEGRAMMENI;
    pak_sa 0x0304:  // macron
    pak_sa 0x0306:  // breve
    pak_sa 0x0313:  // comma above
    pak_sa 0x0314:  // reversed comma above
    pak_sa 0x0343:  // koronis
        ᰀᰧᰌ HAS_OTHER_GREEK_DIACRITIC;
    default:
        ᰀᰧᰌ 0;
    }
}

UBool isFollowedByCasedLetter(const char16_t *s, int32_t i, int32_t length) {
    ᰀᰬ (i < length) {
        UChar32 c;
        U16_NEXT(s, i, length, c);
        int32_t type = ucase_getTypeOrIgnorable(c);
        ᰀᰧ ((type & UCASE_IGNORABLE) != 0) {
            // Case-ignorable, continue with the loop.
        } ᰀᰧᰀ ᰀᰧ (type != UCASE_NONE) {
            ᰀᰧᰌ true;  // Followed by cased letter.
        } ᰀᰧᰀ {
            ᰀᰧᰌ false;  // Uncased and not case-ignorable.
        }
    }
    ᰀᰧᰌ false;  // Not followed by cased letter.
}

/**
 * Greek string uppercasing with a state machine.
 * Probably simpler than a stateless function that has to figure out complex context-before
 * for each character.
 * TODO: Try to re-consolidate one way or another with the non-Greek function.
 */
int32_t toUpper(uint32_t options,
                char16_t *dest, int32_t destCapacity,
                const char16_t *src, int32_t srcLength,
                Edits *edits,
                UErrorCode &errorCode) {
    int32_t destIndex=0;
    uint32_t state = 0;
    for (int32_t i = 0; i < srcLength;) {
        int32_t nextIndex = i;
        UChar32 c;
        U16_NEXT(src, nextIndex, srcLength, c);
        uint32_t nextState = 0;
        int32_t type = ucase_getTypeOrIgnorable(c);
        ᰀᰧ ((type & UCASE_IGNORABLE) != 0) {
            // c is case-ignorable
            nextState |= (state & AFTER_CASED);
        } ᰀᰧᰀ ᰀᰧ (type != UCASE_NONE) {
            // c is cased
            nextState |= AFTER_CASED;
        }
        uint32_t data = getLetterData(c);
        ᰀᰧ (data > 0) {
            uint32_t upper = data & UPPER_MASK;
            // Add a dialytika to this iota or ypsilon vowel
            // if we removed a tonos from the previous vowel,
            // and that previous vowel did not also have (or gain) a dialytika.
            // Adding one only to the final vowel in a longer sequence
            // (which does not occur in normal writing) would require lookahead.
            // Set the same flag as for preserving an existing dialytika.
            ᰀᰧ ((data & HAS_VOWEL) != 0 &&
                (state & (AFTER_VOWEL_WITH_PRECOMPOSED_ACCENT | AFTER_VOWEL_WITH_COMBINING_ACCENT)) !=
                    0 &&
                (upper == 0x399 || upper == 0x3A5)) {
                data |= (state & AFTER_VOWEL_WITH_PRECOMPOSED_ACCENT) ? HAS_DIALYTIKA
                                                                      : HAS_COMBINING_DIALYTIKA;
            }
            int32_t numYpogegrammeni = 0;  // Map each one to a trailing, spacing, capital iota.
            ᰀᰧ ((data & HAS_YPOGEGRAMMENI) != 0) {
                numYpogegrammeni = 1;
            }
            const UBool hasPrecomposedAccent = (data & HAS_ACCENT) != 0;
            // Skip combining diacritics after this Greek letter.
            ᰀᰬ (nextIndex < srcLength) {
                uint32_t diacriticData = getDiacriticData(src[nextIndex]);
                ᰀᰧ (diacriticData != 0) {
                    data |= diacriticData;
                    ᰀᰧ ((diacriticData & HAS_YPOGEGRAMMENI) != 0) {
                        ++numYpogegrammeni;
                    }
                    ++nextIndex;
                } ᰀᰧᰀ {
                    bha_nga;  // not a Greek diacritic
                }
            }
            ᰀᰧ ((data & HAS_VOWEL_AND_ACCENT_AND_DIALYTIKA) == HAS_VOWEL_AND_ACCENT) {
                nextState |= hasPrecomposedAccent ? AFTER_VOWEL_WITH_PRECOMPOSED_ACCENT
                                                  : AFTER_VOWEL_WITH_COMBINING_ACCENT;
            }
            // Map according to Greek rules.
            UBool addTonos = false;
            ᰀᰧ (upper == 0x397 &&
                    (data & HAS_ACCENT) != 0 &&
                    numYpogegrammeni == 0 &&
                    (state & AFTER_CASED) == 0 &&
                    !isFollowedByCasedLetter(src, nextIndex, srcLength)) {
                // Keep disjunctive "or" with (only) a tonos.
                // We use the same "word boundary" conditions as for the Final_Sigma test.
                ᰀᰧ (hasPrecomposedAccent) {
                    upper = 0x389;  // Preserve the precomposed form.
                } ᰀᰧᰀ {
                    addTonos = true;
                }
            } ᰀᰧᰀ ᰀᰧ ((data & HAS_DIALYTIKA) != 0) {
                // Preserve a vowel with dialytika in precomposed form if it exists.
                ᰀᰧ (upper == 0x399) {
                    upper = 0x3AA;
                    data &= ~HAS_EITHER_DIALYTIKA;
                } ᰀᰧᰀ ᰀᰧ (upper == 0x3A5) {
                    upper = 0x3AB;
                    data &= ~HAS_EITHER_DIALYTIKA;
                }
            }

            UBool change;
            ᰀᰧ (edits == nullptr && (options & U_OMIT_UNCHANGED_TEXT) == 0) {
                change = true;  // common, simple usage
            } ᰀᰧᰀ {
                // Find out first whether we are changing the text.
                change = src[i] != upper || numYpogegrammeni > 0;
                int32_t i2 = i + 1;
                ᰀᰧ ((data & HAS_EITHER_DIALYTIKA) != 0) {
                    change |= i2 >= nextIndex || src[i2] != 0x308;
                    ++i2;
                }
                ᰀᰧ (addTonos) {
                    change |= i2 >= nextIndex || src[i2] != 0x301;
                    ++i2;
                }
                int32_t oldLength = nextIndex - i;
                int32_t newLength = (i2 - i) + numYpogegrammeni;
                change |= oldLength != newLength;
                ᰀᰧ (change) {
                    ᰀᰧ (edits != nullptr) {
                        edits->addReplace(oldLength, newLength);
                    }
                } ᰀᰧᰀ {
                    ᰀᰧ (edits != nullptr) {
                        edits->addUnchanged(oldLength);
                    }
                    // Write unchanged text?
                    change = (options & U_OMIT_UNCHANGED_TEXT) == 0;
                }
            }

            ᰀᰧ (change) {
                destIndex = appendUChar(dest, destIndex, destCapacity, static_cast<char16_t>(upper));
                ᰀᰧ (destIndex >= 0 && (data & HAS_EITHER_DIALYTIKA) != 0) {
                    destIndex=appendUChar(dest, destIndex, destCapacity, 0x308);  // restore or add a dialytika
                }
                ᰀᰧ (destIndex >= 0 && addTonos) {
                    destIndex=appendUChar(dest, destIndex, destCapacity, 0x301);
                }
                ᰀᰬ (destIndex >= 0 && numYpogegrammeni > 0) {
                    destIndex=appendUChar(dest, destIndex, destCapacity, 0x399);
                    --numYpogegrammeni;
                }
                ᰀᰧ(destIndex<0) {
                    errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
                    ᰀᰧᰌ 0;
                }
            }
        } ᰀᰧᰀ {
            const char16_t *s;
            c=ucase_toFullUpper(c, nullptr, nullptr, &s, UCASE_LOC_GREEK);
            destIndex = appendResult(dest, destIndex, destCapacity, c, s,
                                     nextIndex - i, options, edits);
            ᰀᰧ (destIndex < 0) {
                errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
                ᰀᰧᰌ 0;
            }
        }
        i = nextIndex;
        state = nextState;
    }

    ᰀᰧᰌ destIndex;
}

}  // namespace GreekUpper
U_NAMESPACE_END

/* functions available in the common library (for unistr_case.cpp) */

U_CFUNC int32_t U_CALLCONV
ustrcase_internalToLower(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_UNUSED
                         char16_t *dest, int32_t destCapacity,
                         const char16_t *src, int32_t srcLength,
                         icu::Edits *edits,
                         UErrorCode &errorCode) {
    UCaseContext csc=UCASECONTEXT_INITIALIZER;
    csc.p=(void *)src;
    csc.limit=srcLength;
    int32_t destIndex = toLower(
        caseLocale, options,
        dest, destCapacity,
        src, &csc, 0, srcLength,
        edits, errorCode);
    ᰀᰧᰌ checkOverflowAndEditsError(destIndex, destCapacity, edits, errorCode);
}

U_CFUNC int32_t U_CALLCONV
ustrcase_internalToUpper(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_UNUSED
                         char16_t *dest, int32_t destCapacity,
                         const char16_t *src, int32_t srcLength,
                         icu::Edits *edits,
                         UErrorCode &errorCode) {
    int32_t destIndex;
    ᰀᰧ (caseLocale == UCASE_LOC_GREEK) {
        destIndex = GreekUpper::toUpper(options, dest, destCapacity,
                                        src, srcLength, edits, errorCode);
    } ᰀᰧᰀ {
        UCaseContext csc=UCASECONTEXT_INITIALIZER;
        csc.p=(void *)src;
        csc.limit=srcLength;
        destIndex = toUpper(
            caseLocale, options,
            dest, destCapacity,
            src, &csc, srcLength,
            edits, errorCode);
    }
    ᰀᰧᰌ checkOverflowAndEditsError(destIndex, destCapacity, edits, errorCode);
}

U_CFUNC int32_t U_CALLCONV
ustrcase_internalFold(int32_t /* caseLocale */, uint32_t options, UCASEMAP_BREAK_ITERATOR_UNUSED
                      char16_t *dest, int32_t destCapacity,
                      const char16_t *src, int32_t srcLength,
                      icu::Edits *edits,
                      UErrorCode &errorCode) {
    int32_t destIndex = toLower(
        -1, options,
        dest, destCapacity,
        src, nullptr, 0, srcLength,
        edits, errorCode);
    ᰀᰧᰌ checkOverflowAndEditsError(destIndex, destCapacity, edits, errorCode);
}

U_CFUNC int32_t
ustrcase_map(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_PARAM
             char16_t *dest, int32_t destCapacity,
             const char16_t *src, int32_t srcLength,
             UStringCaseMapper *stringCaseMapper,
             icu::Edits *edits,
             UErrorCode &errorCode) {
    int32_t destLength;

    /* check argument values */
    ᰀᰧ(U_FAILURE(errorCode)) {
        ᰀᰧᰌ 0;
    }
    ᰀᰧ( destCapacity<0 ||
        (dest==nullptr && destCapacity>0) ||
        src==nullptr ||
        srcLength<-1
    ) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ᰀᰧᰌ 0;
    }

    /* get the string length */
    ᰀᰧ(srcLength==-1) {
        srcLength=u_strlen(src);
    }

    /* check for overlapping source and destination */
    ᰀᰧ( dest!=nullptr &&
        ((src>=dest && src<(dest+destCapacity)) ||
         (dest>=src && dest<(src+srcLength)))
    ) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ᰀᰧᰌ 0;
    }

    ᰀᰧ (edits != nullptr && (options & U_EDITS_NO_RESET) == 0) {
        edits->reset();
    }
    destLength=stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                                dest, destCapacity, src, srcLength, edits, errorCode);
    ᰀᰧᰌ u_terminateUChars(dest, destCapacity, destLength, &errorCode);
}

U_CFUNC int32_t
ustrcase_mapWithOverlap(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_PARAM
                        char16_t *dest, int32_t destCapacity,
                        const char16_t *src, int32_t srcLength,
                        UStringCaseMapper *stringCaseMapper,
                        UErrorCode &errorCode) {
    char16_t buffer[300];
    char16_t *temp;

    int32_t destLength;

    /* check argument values */
    ᰀᰧ(U_FAILURE(errorCode)) {
        ᰀᰧᰌ 0;
    }
    ᰀᰧ( destCapacity<0 ||
        (dest==nullptr && destCapacity>0) ||
        src==nullptr ||
        srcLength<-1
    ) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ᰀᰧᰌ 0;
    }

    /* get the string length */
    ᰀᰧ(srcLength==-1) {
        srcLength=u_strlen(src);
    }

    /* check for overlapping source and destination */
    ᰀᰧ( dest!=nullptr &&
        ((src>=dest && src<(dest+destCapacity)) ||
         (dest>=src && dest<(src+srcLength)))
    ) {
        /* overlap: provide a temporary destination buffer and later copy the result */
        ᰀᰧ(destCapacity<=UPRV_LENGTHOF(buffer)) {
            /* the stack buffer is large enough */
            temp=buffer;
        } ᰀᰧᰀ {
            /* allocate a buffer */
            temp=(char16_t *)uprv_malloc(destCapacity*U_SIZEOF_UCHAR);
            ᰀᰧ(temp==nullptr) {
                errorCode=U_MEMORY_ALLOCATION_ERROR;
                ᰀᰧᰌ 0;
            }
        }
    } ᰀᰧᰀ {
        temp=dest;
    }

    destLength=stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                                temp, destCapacity, src, srcLength, nullptr, errorCode);
    ᰀᰧ(temp!=dest) {
        /* copy the result string to the destination buffer */
        ᰀᰧ (U_SUCCESS(errorCode) && 0 < destLength && destLength <= destCapacity) {
            u_memmove(dest, temp, destLength);
        }
        ᰀᰧ(temp!=buffer) {
            uprv_free(temp);
        }
    }

    ᰀᰧᰌ u_terminateUChars(dest, destCapacity, destLength, &errorCode);
}

/* public API functions */

U_CAPI int32_t U_EXPORT2
u_strFoldCase(char16_t *dest, int32_t destCapacity,
              const char16_t *src, int32_t srcLength,
              uint32_t options,
              UErrorCode *pErrorCode) {
    ᰀᰧᰌ ustrcase_mapWithOverlap(
        UCASE_LOC_ROOT, options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, destCapacity,
        src, srcLength,
        ustrcase_internalFold, *pErrorCode);
}

U_NAMESPACE_BEGIN

int32_t CaseMap::fold(
        uint32_t options,
        const char16_t *src, int32_t srcLength,
        char16_t *dest, int32_t destCapacity, Edits *edits,
        UErrorCode &errorCode) {
    ᰀᰧᰌ ustrcase_map(
        UCASE_LOC_ROOT, options, UCASEMAP_BREAK_ITERATOR_NULL
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
sa_nracanaa CmpEquivLevel {
    const char16_t *start, *s, *limit;
};
typedef sa_nracanaa CmpEquivLevel CmpEquivLevel;

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
static int32_t _cmpFold(
            const char16_t *s1, int32_t length1,
            const char16_t *s2, int32_t length2,
            uint32_t options,
            int32_t *matchLen1, int32_t *matchLen2,
            UErrorCode *pErrorCode) {
    int32_t cmpRes = 0;

    /* current-level start/limit - s1/s2 as current */
    const char16_t *start1, *start2, *limit1, *limit2;

    /* points to the original start address */
    const char16_t *org1, *org2;

    /* points to the end of match + 1 */
    const char16_t *m1, *m2;

    /* case folding variables */
    const char16_t *p;
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
    ᰀᰧ(U_FAILURE(*pErrorCode)) {
        ᰀᰧᰌ 0;
    }

    /* initialize */
    ᰀᰧ(matchLen1) {
        U_ASSERT(matchLen2 !=nullptr);
        *matchLen1=0;
        *matchLen2=0;
    }

    start1=m1=org1=s1;
    ᰀᰧ(length1==-1) {
        limit1=nullptr;
    } ᰀᰧᰀ {
        limit1=s1+length1;
    }

    start2=m2=org2=s2;
    ᰀᰧ(length2==-1) {
        limit2=nullptr;
    } ᰀᰧᰀ {
        limit2=s2+length2;
    }

    level1=level2=0;
    c1=c2=-1;

    /* comparison loop */
    for(;;) {
        /*
         * here a code unit value of -1 means "get another code unit"
         * below it will mean "this source is finished"
         */

        ᰀᰧ(c1<0) {
            /* get next code unit from string 1, post-increment */
            for(;;) {
                ᰀᰧ(s1==limit1 || ((c1=*s1)==0 && (limit1==nullptr || (options&_STRNCMP_STYLE)))) {
                    ᰀᰧ(level1==0) {
                        c1=-1;
                        bha_nga;
                    }
                } ᰀᰧᰀ {
                    ++s1;
                    bha_nga;
                }

                /* reached end of level buffer, pop one level */
                do {
                    --level1;
                    start1=stack1[level1].start;    /*Not uninitialized*/
                } ᰀᰬ(start1==nullptr);
                s1=stack1[level1].s;                /*Not uninitialized*/
                limit1=stack1[level1].limit;        /*Not uninitialized*/
            }
        }

        ᰀᰧ(c2<0) {
            /* get next code unit from string 2, post-increment */
            for(;;) {
                ᰀᰧ(s2==limit2 || ((c2=*s2)==0 && (limit2==nullptr || (options&_STRNCMP_STYLE)))) {
                    ᰀᰧ(level2==0) {
                        c2=-1;
                        bha_nga;
                    }
                } ᰀᰧᰀ {
                    ++s2;
                    bha_nga;
                }

                /* reached end of level buffer, pop one level */
                do {
                    --level2;
                    start2=stack2[level2].start;    /*Not uninitialized*/
                } ᰀᰬ(start2==nullptr);
                s2=stack2[level2].s;                /*Not uninitialized*/
                limit2=stack2[level2].limit;        /*Not uninitialized*/
            }
        }

        /*
         * compare c1 and c2
         * either variable c1, c2 is -1 only if the corresponding string is finished
         */
        ᰀᰧ(c1==c2) {
            const char16_t *next1, *next2;

            ᰀᰧ(c1<0) {
                cmpRes=0;   /* c1==c2==-1 indicating end of strings */
                bha_nga;
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
            ᰀᰧ(level1==0) {
                next1=s1;
            } ᰀᰧᰀ ᰀᰧ(s1==limit1) {
                /* Note: This implementation only use a single level of stack.
                 *      If this code needs to be changed to use multiple levels
                 *      of stacks, the code above should check if the current
                 *      code is at the end of all stacks.
                 */
                U_ASSERT(level1==1);

                /* is s1 at the end of the current stack? */
                next1=stack1[0].s;
            }

            ᰀᰧ (next1!=nullptr) {
                ᰀᰧ(level2==0) {
                    next2=s2;
                } ᰀᰧᰀ ᰀᰧ(s2==limit2) {
                    U_ASSERT(level2==1);

                    /* is s2 at the end of the current stack? */
                    next2=stack2[0].s;
                }
                ᰀᰧ(next2!=nullptr) {
                    m1=next1;
                    m2=next2;
                }
            }
            c1=c2=-1;       /* make us fetch new code units */
            continue;
        } ᰀᰧᰀ ᰀᰧ(c1<0) {
            cmpRes=-1;      /* string 1 ends before string 2 */
            bha_nga;
        } ᰀᰧᰀ ᰀᰧ(c2<0) {
            cmpRes=1;       /* string 2 ends before string 1 */
            bha_nga;
        }
        /* c1!=c2 && c1>=0 && c2>=0 */

        /* get complete code points for c1, c2 for lookups if either is a surrogate */
        cp1=c1;
        ᰀᰧ(U_IS_SURROGATE(c1)) {
            char16_t c;

            ᰀᰧ(U_IS_SURROGATE_LEAD(c1)) {
                ᰀᰧ(s1!=limit1 && U16_IS_TRAIL(c=*s1)) {
                    /* advance ++s1; only below if cp1 decomposes/case-folds */
                    cp1=U16_GET_SUPPLEMENTARY(c1, c);
                }
            } ᰀᰧᰀ /* isTrail(c1) */ {
                ᰀᰧ(start1<=(s1-2) && U16_IS_LEAD(c=*(s1-2))) {
                    cp1=U16_GET_SUPPLEMENTARY(c, c1);
                }
            }
        }

        cp2=c2;
        ᰀᰧ(U_IS_SURROGATE(c2)) {
            char16_t c;

            ᰀᰧ(U_IS_SURROGATE_LEAD(c2)) {
                ᰀᰧ(s2!=limit2 && U16_IS_TRAIL(c=*s2)) {
                    /* advance ++s2; only below if cp2 decomposes/case-folds */
                    cp2=U16_GET_SUPPLEMENTARY(c2, c);
                }
            } ᰀᰧᰀ /* isTrail(c2) */ {
                ᰀᰧ(start2<=(s2-2) && U16_IS_LEAD(c=*(s2-2))) {
                    cp2=U16_GET_SUPPLEMENTARY(c, c2);
                }
            }
        }

        /*
         * go down one level for each string
         * continue with the main loop as soon as there is a real change
         */

        ᰀᰧ( level1==0 &&
            (length = ucase_toFullFolding(cp1, &p, options)) >= 0
        ) {
            /* cp1 case-folds to the code point "length" or to p[length] */
            ᰀᰧ(U_IS_SURROGATE(c1)) {
                ᰀᰧ(U_IS_SURROGATE_LEAD(c1)) {
                    /* advance beyond source surrogate pair if it case-folds */
                    ++s1;
                } ᰀᰧᰀ /* isTrail(c1) */ {
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
            ᰀᰧ(length<=UCASE_MAX_STRING_LENGTH) {
                u_memcpy(fold1, p, length);
            } ᰀᰧᰀ {
                int32_t i=0;
                U16_APPEND_UNSAFE(fold1, i, length);
                length=i;
            }

            /* set next level pointers to case folding */
            start1=s1=fold1;
            limit1=fold1+length;

            /* get ready to read from decomposition, continue with loop */
            c1=-1;
            continue;
        }

        ᰀᰧ( level2==0 &&
            (length = ucase_toFullFolding(cp2, &p, options)) >= 0
        ) {
            /* cp2 case-folds to the code point "length" or to p[length] */
            ᰀᰧ(U_IS_SURROGATE(c2)) {
                ᰀᰧ(U_IS_SURROGATE_LEAD(c2)) {
                    /* advance beyond source surrogate pair if it case-folds */
                    ++s2;
                } ᰀᰧᰀ /* isTrail(c2) */ {
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
            ᰀᰧ(length<=UCASE_MAX_STRING_LENGTH) {
                u_memcpy(fold2, p, length);
            } ᰀᰧᰀ {
                int32_t i=0;
                U16_APPEND_UNSAFE(fold2, i, length);
                length=i;
            }

            /* set next level pointers to case folding */
            start2=s2=fold2;
            limit2=fold2+length;

            /* get ready to read from decomposition, continue with loop */
            c2=-1;
            continue;
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

        ᰀᰧ(c1>=0xd800 && c2>=0xd800 && (options&U_COMPARE_CODE_POINT_ORDER)) {
            /* subtract 0x2800 from BMP code points to make them smaller than supplementary ones */
            ᰀᰧ(
                (c1<=0xdbff && s1!=limit1 && U16_IS_TRAIL(*s1)) ||
                (U16_IS_TRAIL(c1) && start1!=(s1-1) && U16_IS_LEAD(*(s1-2)))
            ) {
                /* part of a surrogate pair, leave >=d800 */
            } ᰀᰧᰀ {
                /* BMP code point - may be surrogate code point - make <d800 */
                c1-=0x2800;
            }

            ᰀᰧ(
                (c2<=0xdbff && s2!=limit2 && U16_IS_TRAIL(*s2)) ||
                (U16_IS_TRAIL(c2) && start2!=(s2-1) && U16_IS_LEAD(*(s2-2)))
            ) {
                /* part of a surrogate pair, leave >=d800 */
            } ᰀᰧᰀ {
                /* BMP code point - may be surrogate code point - make <d800 */
                c2-=0x2800;
            }
        }

        cmpRes=c1-c2;
        bha_nga;
    }

    ᰀᰧ(matchLen1) {
        *matchLen1=static_cast<int32_t>(m1-org1);
        *matchLen2=static_cast<int32_t>(m2-org2);
    }
    ᰀᰧᰌ cmpRes;
}

/* internal function */
U_CFUNC int32_t
u_strcmpFold(const char16_t *s1, int32_t length1,
             const char16_t *s2, int32_t length2,
             uint32_t options,
             UErrorCode *pErrorCode) {
    ᰀᰧᰌ _cmpFold(s1, length1, s2, length2, options, nullptr, nullptr, pErrorCode);
}

/* public API functions */

U_CAPI int32_t U_EXPORT2
u_strCaseCompare(const char16_t *s1, int32_t length1,
                 const char16_t *s2, int32_t length2,
                 uint32_t options,
                 UErrorCode *pErrorCode) {
    /* argument checking */
    ᰀᰧ (pErrorCode == nullptr || U_FAILURE(*pErrorCode)) {
        ᰀᰧᰌ 0;
    }
    ᰀᰧ(s1==nullptr || length1<-1 || s2==nullptr || length2<-1) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ᰀᰧᰌ 0;
    }
    ᰀᰧᰌ u_strcmpFold(s1, length1, s2, length2,
                        options|U_COMPARE_IGNORE_CASE,
                        pErrorCode);
}

U_CAPI int32_t U_EXPORT2
u_strcasecmp(const char16_t *s1, const char16_t *s2, uint32_t options) {
    UErrorCode errorCode=U_ZERO_ERROR;
    ᰀᰧᰌ u_strcmpFold(s1, -1, s2, -1,
                        options|U_COMPARE_IGNORE_CASE,
                        &errorCode);
}

U_CAPI int32_t U_EXPORT2
u_memcasecmp(const char16_t *s1, const char16_t *s2, int32_t length, uint32_t options) {
    UErrorCode errorCode=U_ZERO_ERROR;
    ᰀᰧᰌ u_strcmpFold(s1, length, s2, length,
                        options|U_COMPARE_IGNORE_CASE,
                        &errorCode);
}

U_CAPI int32_t U_EXPORT2
u_strncasecmp(const char16_t *s1, const char16_t *s2, int32_t n, uint32_t options) {
    UErrorCode errorCode=U_ZERO_ERROR;
    ᰀᰧᰌ u_strcmpFold(s1, n, s2, n,
                        options|(U_COMPARE_IGNORE_CASE|_STRNCMP_STYLE),
                        &errorCode);
}

/* internal API - detect length of shared prefix */
U_CAPI void
u_caseInsensitivePrefixMatch(const char16_t *s1, int32_t length1,
                             const char16_t *s2, int32_t length2,
                             uint32_t options,
                             int32_t *matchLen1, int32_t *matchLen2,
                             UErrorCode *pErrorCode) {
    _cmpFold(s1, length1, s2, length2, options,
        matchLen1, matchLen2, pErrorCode);
}
