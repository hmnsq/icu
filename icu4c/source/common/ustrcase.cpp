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

#അന്തര്ഭാവ "unicode/utypes.h"
#അന്തര്ഭാവ "unicode/brkiter.h"
#അന്തര്ഭാവ "unicode/casemap.h"
#അന്തര്ഭാവ "unicode/edits.h"
#അന്തര്ഭാവ "unicode/stringoptions.h"
#അന്തര്ഭാവ "unicode/ustring.h"
#അന്തര്ഭാവ "unicode/ucasemap.h"
#അന്തര്ഭാവ "unicode/ubrk.h"
#അന്തര്ഭാവ "unicode/utf.h"
#അന്തര്ഭാവ "unicode/utf16.h"
#അന്തര്ഭാവ "cmemory.h"
#അന്തര്ഭാവ "ucase.h"
#അന്തര്ഭാവ "ucasemap_imp.h"
#അന്തര്ഭാവ "ustr_imp.h"
#അന്തര്ഭാവ "uassert.h"

/**
 * Code point for COMBINING ACUTE ACCENT
 * @internal
 */
#define ACUTE u'\u0301'

U_NAMESPACE_BEGIN

നാമാകാശ {

int32_t checkOverflowAndEditsError(int32_t destIndex, int32_t destCapacity,
                                   Edits *edits, UErrorCode &errorCode) {
    എങ്കിൽ (U_SUCCESS(errorCode)) {
        എങ്കിൽ (destIndex > destCapacity) {
            errorCode = U_BUFFER_OVERFLOW_ERROR;
        } അല്ലെങ്കിൽ എങ്കിൽ (edits != nullptr) {
            edits->copyErrorTo(errorCode);
        }
    }
    തിരികെ destIndex;
}

/* Appends a full case mapping result, see UCASE_MAX_STRING_LENGTH. */
വരിയിൽ int32_t
appendResult(char16_t *dest, int32_t destIndex, int32_t destCapacity,
             int32_t result, സ്ഥിരം char16_t *s,
             int32_t cpLength, uint32_t options, icu::Edits *edits) {
    UChar32 c;
    int32_t length;

    /* decode the result */
    എങ്കിൽ(result<0) {
        /* (not) original code point */
        എങ്കിൽ(edits!=nullptr) {
            edits->addUnchanged(cpLength);
        }
        എങ്കിൽ(options & U_OMIT_UNCHANGED_TEXT) {
            തിരികെ destIndex;
        }
        c=~result;
        എങ്കിൽ(destIndex<destCapacity && c<=0xffff) {  // BMP slightly-fastpath
            dest[destIndex++] = static_cast<char16_t>(c);
            തിരികെ destIndex;
        }
        length=cpLength;
    } അല്ലെങ്കിൽ {
        എങ്കിൽ(result<=UCASE_MAX_STRING_LENGTH) {
            c=U_SENTINEL;
            length=result;
        } അല്ലെങ്കിൽ എങ്കിൽ(destIndex<destCapacity && result<=0xffff) {  // BMP slightly-fastpath
            dest[destIndex++] = static_cast<char16_t>(result);
            എങ്കിൽ(edits!=nullptr) {
                edits->addReplace(cpLength, 1);
            }
            തിരികെ destIndex;
        } അല്ലെങ്കിൽ {
            c=result;
            length=U16_LENGTH(c);
        }
        എങ്കിൽ(edits!=nullptr) {
            edits->addReplace(cpLength, length);
        }
    }
    എങ്കിൽ(length>(INT32_MAX-destIndex)) {
        തിരികെ -1;  // integer overflow
    }

    എങ്കിൽ(destIndex<destCapacity) {
        /* append the result */
        എങ്കിൽ(c>=0) {
            /* code point */
            UBool isError=അസത്യം;
            U16_APPEND(dest, destIndex, destCapacity, c, isError);
            എങ്കിൽ(isError) {
                /* overflow, nothing written */
                destIndex+=length;
            }
        } അല്ലെങ്കിൽ {
            /* string */
            എങ്കിൽ((destIndex+length)<=destCapacity) {
                ആകുമ്പോൾ(length>0) {
                    dest[destIndex++]=*s++;
                    --length;
                }
            } അല്ലെങ്കിൽ {
                /* overflow */
                destIndex+=length;
            }
        }
    } അല്ലെങ്കിൽ {
        /* preflight */
        destIndex+=length;
    }
    തിരികെ destIndex;
}

വരിയിൽ int32_t
appendUChar(char16_t *dest, int32_t destIndex, int32_t destCapacity, char16_t c) {
    എങ്കിൽ(destIndex<destCapacity) {
        dest[destIndex]=c;
    } അല്ലെങ്കിൽ എങ്കിൽ(destIndex==INT32_MAX) {
        തിരികെ -1;  // integer overflow
    }
    തിരികെ destIndex+1;
}

int32_t
appendNonEmptyUnchanged(char16_t *dest, int32_t destIndex, int32_t destCapacity,
                        സ്ഥിരം char16_t *s, int32_t length, uint32_t options, icu::Edits *edits) {
    എങ്കിൽ(edits!=nullptr) {
        edits->addUnchanged(length);
    }
    എങ്കിൽ(options & U_OMIT_UNCHANGED_TEXT) {
        തിരികെ destIndex;
    }
    എങ്കിൽ(length>(INT32_MAX-destIndex)) {
        തിരികെ -1;  // integer overflow
    }
    എങ്കിൽ((destIndex+length)<=destCapacity) {
        u_memcpy(dest+destIndex, s, length);
    }
    തിരികെ destIndex + length;
}

വരിയിൽ int32_t
appendUnchanged(char16_t *dest, int32_t destIndex, int32_t destCapacity,
                സ്ഥിരം char16_t *s, int32_t length, uint32_t options, icu::Edits *edits) {
    എങ്കിൽ (length <= 0) {
        തിരികെ destIndex;
    }
    തിരികെ appendNonEmptyUnchanged(dest, destIndex, destCapacity, s, length, options, edits);
}

UChar32 U_CALLCONV
utf16_caseContextIterator(ശൂന്യം *context, int8_t dir) {
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
            U16_PREV((സ്ഥിരം char16_t *)csc->p, csc->start, csc->index, c);
            തിരികെ c;
        }
    } അല്ലെങ്കിൽ {
        എങ്കിൽ(csc->index<csc->limit) {
            U16_NEXT((സ്ഥിരം char16_t *)csc->p, csc->index, csc->limit, c);
            തിരികെ c;
        }
    }
    തിരികെ U_SENTINEL;
}

/**
 * caseLocale >= 0: Lowercases [srcStart..srcLimit[ but takes context [0..srcLength[ into account.
 * caseLocale < 0: Case-folds [srcStart..srcLimit[.
 */
int32_t toLower(int32_t caseLocale, uint32_t options,
                char16_t *dest, int32_t destCapacity,
                സ്ഥിരം char16_t *src, UCaseContext *csc, int32_t srcStart, int32_t srcLimit,
                icu::Edits *edits, UErrorCode &errorCode) {
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
    int32_t destIndex = 0;
    int32_t prev = srcStart;
    int32_t srcIndex = srcStart;
    വേണ്ടി (;;) {
        // fast path for simple cases
        char16_t lead = 0;
        ആകുമ്പോൾ (srcIndex < srcLimit) {
            lead = src[srcIndex];
            int32_t delta;
            എങ്കിൽ (lead < LatinCase::LONG_S) {
                int8_t d = latinToLower[lead];
                എങ്കിൽ (d == LatinCase::EXC) { നിർത്തുക; }
                ++srcIndex;
                എങ്കിൽ (d == 0) { തുടരുക; }
                delta = d;
            } അല്ലെങ്കിൽ എങ്കിൽ (lead >= 0xd800) {
                നിർത്തുക;  // surrogate or higher
            } അല്ലെങ്കിൽ {
                uint16_t props = UTRIE2_GET16_FROM_U16_SINGLE_LEAD(trie, lead);
                എങ്കിൽ (UCASE_HAS_EXCEPTION(props)) { നിർത്തുക; }
                ++srcIndex;
                എങ്കിൽ (!UCASE_IS_UPPER_OR_TITLE(props) || (delta = UCASE_GET_DELTA(props)) == 0) {
                    തുടരുക;
                }
            }
            lead += static_cast<char16_t>(delta);
            destIndex = appendUnchanged(dest, destIndex, destCapacity,
                                        src + prev, srcIndex - 1 - prev, options, edits);
            എങ്കിൽ (destIndex >= 0) {
                destIndex = appendUChar(dest, destIndex, destCapacity, lead);
                എങ്കിൽ (edits != nullptr) {
                    edits->addReplace(1, 1);
                }
            }
            എങ്കിൽ (destIndex < 0) {
                errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
                തിരികെ 0;
            }
            prev = srcIndex;
        }
        എങ്കിൽ (srcIndex >= srcLimit) {
            നിർത്തുക;
        }
        // slow path
        int32_t cpStart = srcIndex++;
        char16_t trail;
        UChar32 c;
        എങ്കിൽ (U16_IS_LEAD(lead) && srcIndex < srcLimit && U16_IS_TRAIL(trail = src[srcIndex])) {
            c = U16_GET_SUPPLEMENTARY(lead, trail);
            ++srcIndex;
        } അല്ലെങ്കിൽ {
            c = lead;
        }
        സ്ഥിരം char16_t *s = nullptr;
        എങ്കിൽ (caseLocale >= 0) {
            csc->cpStart = cpStart;
            csc->cpLimit = srcIndex;
            c = ucase_toFullLower(c, utf16_caseContextIterator, csc, &s, caseLocale);
        } അല്ലെങ്കിൽ {
            c = ucase_toFullFolding(c, &s, options);
        }
        എങ്കിൽ (c >= 0) {
            destIndex = appendUnchanged(dest, destIndex, destCapacity,
                                        src + prev, cpStart - prev, options, edits);
            എങ്കിൽ (destIndex >= 0) {
                destIndex = appendResult(dest, destIndex, destCapacity, c, s,
                                         srcIndex - cpStart, options, edits);
            }
            എങ്കിൽ (destIndex < 0) {
                errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
                തിരികെ 0;
            }
            prev = srcIndex;
        }
    }
    destIndex = appendUnchanged(dest, destIndex, destCapacity,
                                src + prev, srcIndex - prev, options, edits);
    എങ്കിൽ (destIndex < 0) {
        errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
        തിരികെ 0;
    }
    തിരികെ destIndex;
}

int32_t toUpper(int32_t caseLocale, uint32_t options,
                char16_t *dest, int32_t destCapacity,
                സ്ഥിരം char16_t *src, UCaseContext *csc, int32_t srcLength,
                icu::Edits *edits, UErrorCode &errorCode) {
    സ്ഥിരം int8_t *latinToUpper;
    എങ്കിൽ (caseLocale == UCASE_LOC_TURKISH) {
        latinToUpper = LatinCase::TO_UPPER_TR;
    } അല്ലെങ്കിൽ {
        latinToUpper = LatinCase::TO_UPPER_NORMAL;
    }
    സ്ഥിരം UTrie2 *trie = ucase_getTrie();
    int32_t destIndex = 0;
    int32_t prev = 0;
    int32_t srcIndex = 0;
    വേണ്ടി (;;) {
        // fast path for simple cases
        char16_t lead = 0;
        ആകുമ്പോൾ (srcIndex < srcLength) {
            lead = src[srcIndex];
            int32_t delta;
            എങ്കിൽ (lead < LatinCase::LONG_S) {
                int8_t d = latinToUpper[lead];
                എങ്കിൽ (d == LatinCase::EXC) { നിർത്തുക; }
                ++srcIndex;
                എങ്കിൽ (d == 0) { തുടരുക; }
                delta = d;
            } അല്ലെങ്കിൽ എങ്കിൽ (lead >= 0xd800) {
                നിർത്തുക;  // surrogate or higher
            } അല്ലെങ്കിൽ {
                uint16_t props = UTRIE2_GET16_FROM_U16_SINGLE_LEAD(trie, lead);
                എങ്കിൽ (UCASE_HAS_EXCEPTION(props)) { നിർത്തുക; }
                ++srcIndex;
                എങ്കിൽ (UCASE_GET_TYPE(props) != UCASE_LOWER || (delta = UCASE_GET_DELTA(props)) == 0) {
                    തുടരുക;
                }
            }
            lead += static_cast<char16_t>(delta);
            destIndex = appendUnchanged(dest, destIndex, destCapacity,
                                        src + prev, srcIndex - 1 - prev, options, edits);
            എങ്കിൽ (destIndex >= 0) {
                destIndex = appendUChar(dest, destIndex, destCapacity, lead);
                എങ്കിൽ (edits != nullptr) {
                    edits->addReplace(1, 1);
                }
            }
            എങ്കിൽ (destIndex < 0) {
                errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
                തിരികെ 0;
            }
            prev = srcIndex;
        }
        എങ്കിൽ (srcIndex >= srcLength) {
            നിർത്തുക;
        }
        // slow path
        int32_t cpStart;
        csc->cpStart = cpStart = srcIndex++;
        char16_t trail;
        UChar32 c;
        എങ്കിൽ (U16_IS_LEAD(lead) && srcIndex < srcLength && U16_IS_TRAIL(trail = src[srcIndex])) {
            c = U16_GET_SUPPLEMENTARY(lead, trail);
            ++srcIndex;
        } അല്ലെങ്കിൽ {
            c = lead;
        }
        csc->cpLimit = srcIndex;
        സ്ഥിരം char16_t *s = nullptr;
        c = ucase_toFullUpper(c, utf16_caseContextIterator, csc, &s, caseLocale);
        എങ്കിൽ (c >= 0) {
            destIndex = appendUnchanged(dest, destIndex, destCapacity,
                                        src + prev, cpStart - prev, options, edits);
            എങ്കിൽ (destIndex >= 0) {
                destIndex = appendResult(dest, destIndex, destCapacity, c, s,
                                         srcIndex - cpStart, options, edits);
            }
            എങ്കിൽ (destIndex < 0) {
                errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
                തിരികെ 0;
            }
            prev = srcIndex;
        }
    }
    destIndex = appendUnchanged(dest, destIndex, destCapacity,
                                src + prev, srcIndex - prev, options, edits);
    എങ്കിൽ (destIndex < 0) {
        errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
        തിരികെ 0;
    }
    തിരികെ destIndex;
}

}  // namespace

U_NAMESPACE_END

U_NAMESPACE_USE

#എങ്കിൽ !UCONFIG_NO_BREAK_ITERATION

നാമാകാശ {

/**
 * Input: c is a letter I with or without acute accent.
 * start is the index in src after c, and is less than segmentLimit.
 * If a plain i/I is followed by a plain j/J,
 * or an i/I with acute (precomposed or decomposed) is followed by a j/J with acute,
 * then we output accordingly.
 *
 * @return the src index after the titlecased sequence, or the start index if no Dutch IJ
 */
int32_t maybeTitleDutchIJ(സ്ഥിരം char16_t *src, UChar32 c, int32_t start, int32_t segmentLimit,
                          char16_t *dest, int32_t &destIndex, int32_t destCapacity, uint32_t options,
                          icu::Edits *edits) {
    U_ASSERT(start < segmentLimit);

    int32_t index = start;
    തർക്കം withAcute = അസത്യം;

    // If the conditions are met, then the following variables tell us what to output.
    int32_t unchanged1 = 0;  // code units before the j, or the whole sequence (0..3)
    തർക്കം doTitleJ = അസത്യം;  // true if the j needs to be titlecased
    int32_t unchanged2 = 0;  // after the j (0 or 1)

    // next character after the first letter
    char16_t c2 = src[index++];

    // Is the first letter an i/I with accent?
    എങ്കിൽ (c == u'I') {
        എങ്കിൽ (c2 == ACUTE) {
            withAcute = സത്യം;
            unchanged1 = 1;
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
        എങ്കിൽ (index == segmentLimit || src[index++] != ACUTE) { തിരികെ start; }
        എങ്കിൽ (doTitleJ) {
            unchanged2 = 1;
        } അല്ലെങ്കിൽ {
            ++unchanged1;
        }
    }

    // There must not be another combining mark.
    എങ്കിൽ (index < segmentLimit) {
        int32_t cp;
        int32_t i = index;
        U16_NEXT(src, i, segmentLimit, cp);
        uint32_t typeMask = U_GET_GC_MASK(cp);
        എങ്കിൽ ((typeMask & U_GC_M_MASK) != 0) {
            തിരികെ start;
        }
    }

    // Output the rest of the Dutch IJ.
    destIndex = appendUnchanged(dest, destIndex, destCapacity, src + start, unchanged1, options, edits);
    start += unchanged1;
    എങ്കിൽ (doTitleJ) {
        destIndex = appendUChar(dest, destIndex, destCapacity, u'J');
        എങ്കിൽ (edits != nullptr) {
            edits->addReplace(1, 1);
        }
        ++start;
    }
    destIndex = appendUnchanged(dest, destIndex, destCapacity, src + start, unchanged2, options, edits);

    U_ASSERT(start + unchanged2 == index);
    തിരികെ index;
}

}  // namespace

U_CFUNC int32_t U_CALLCONV
ustrcase_internalToTitle(int32_t caseLocale, uint32_t options, BreakIterator *iter,
                         char16_t *dest, int32_t destCapacity,
                         സ്ഥിരം char16_t *src, int32_t srcLength,
                         icu::Edits *edits,
                         UErrorCode &errorCode) {
    എങ്കിൽ (!ustrcase_checkTitleAdjustmentOptions(options, errorCode)) {
        തിരികെ 0;
    }

    /* set up local variables */
    UCaseContext csc=UCASECONTEXT_INITIALIZER;
    csc.p=(ശൂന്യം *)src;
    csc.limit=srcLength;
    int32_t destIndex=0;
    int32_t prev=0;
    തർക്കം isFirstIndex=സത്യം;

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
            // Find and copy skipped characters [prev..titleStart[
            int32_t titleStart=prev;
            int32_t titleLimit=prev;
            UChar32 c;
            U16_NEXT(src, titleLimit, index, c);
            എങ്കിൽ ((options&U_TITLECASE_NO_BREAK_ADJUSTMENT)==0) {
                // Adjust the titlecasing index to the next cased character,
                // or to the next letter/number/symbol/private use.
                // Stop with titleStart<titleLimit<=index
                // if there is a character to be titlecased,
                // or else stop with titleStart==titleLimit==index.
                തർക്കം toCased = (options&U_TITLECASE_ADJUST_TO_CASED) != 0;
                ആകുമ്പോൾ (toCased ? UCASE_NONE==ucase_getType(c) : !ustrcase_isLNS(c)) {
                    titleStart=titleLimit;
                    എങ്കിൽ(titleLimit==index) {
                        നിർത്തുക;
                    }
                    U16_NEXT(src, titleLimit, index, c);
                }
                എങ്കിൽ (prev < titleStart) {
                    destIndex=appendUnchanged(dest, destIndex, destCapacity,
                                              src+prev, titleStart-prev, options, edits);
                    എങ്കിൽ(destIndex<0) {
                        errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
                        തിരികെ 0;
                    }
                }
            }

            എങ്കിൽ(titleStart<titleLimit) {
                /* titlecase c which is from [titleStart..titleLimit[ */
                csc.cpStart=titleStart;
                csc.cpLimit=titleLimit;
                സ്ഥിരം char16_t *s;
                c=ucase_toFullTitle(c, utf16_caseContextIterator, &csc, &s, caseLocale);
                destIndex=appendResult(dest, destIndex, destCapacity, c, s,
                                       titleLimit-titleStart, options, edits);
                എങ്കിൽ(destIndex<0) {
                    errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
                    തിരികെ 0;
                }

                /* Special case Dutch IJ titlecasing */
                എങ്കിൽ (titleStart+1 < index &&
                        caseLocale == UCASE_LOC_DUTCH) {
                    എങ്കിൽ (c < 0) {
                        c = ~c;
                    }

                    എങ്കിൽ (c == u'I' || c == u'Í') {
                        titleLimit = maybeTitleDutchIJ(src, c, titleStart + 1, index,
                                                       dest, destIndex, destCapacity, options,
                                                       edits);
                    }
                }

                /* lowercase [titleLimit..index[ */
                എങ്കിൽ(titleLimit<index) {
                    എങ്കിൽ((options&U_TITLECASE_NO_LOWERCASE)==0) {
                        /* Normal operation: Lowercase the rest of the word. */
                        destIndex+=
                            toLower(
                                caseLocale, options,
                                (dest==nullptr) ? nullptr: dest+destIndex, destCapacity-destIndex,
                                src, &csc, titleLimit, index,
                                edits, errorCode);
                        എങ്കിൽ(errorCode==U_BUFFER_OVERFLOW_ERROR) {
                            errorCode=U_ZERO_ERROR;
                        }
                        എങ്കിൽ(U_FAILURE(errorCode)) {
                            തിരികെ destIndex;
                        }
                    } അല്ലെങ്കിൽ {
                        /* Optionally just copy the rest of the word unchanged. */
                        destIndex=appendUnchanged(dest, destIndex, destCapacity,
                                                  src+titleLimit, index-titleLimit, options, edits);
                        എങ്കിൽ(destIndex<0) {
                            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
                            തിരികെ 0;
                        }
                    }
                }
            }
        }

        prev=index;
    }

    തിരികെ checkOverflowAndEditsError(destIndex, destCapacity, edits, errorCode);
}

#endif  // !UCONFIG_NO_BREAK_ITERATION

U_NAMESPACE_BEGIN
നാമാകാശ GreekUpper {

// Data generated by prototype code, see
// https://icu.unicode.org/design/case/greek-upper
// TODO: Move this data into ucase.icu.
സ്ഥിതം സ്ഥിരം uint16_t data0370[] = {
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

സ്ഥിതം സ്ഥിരം uint16_t data1F00[] = {
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
സ്ഥിതം സ്ഥിരം uint16_t data2126 = 0x03A9 | HAS_VOWEL;

uint32_t getLetterData(UChar32 c) {
    എങ്കിൽ (c < 0x370 || 0x2126 < c || (0x3ff < c && c < 0x1f00)) {
        തിരികെ 0;
    } അല്ലെങ്കിൽ എങ്കിൽ (c <= 0x3ff) {
        തിരികെ data0370[c - 0x370];
    } അല്ലെങ്കിൽ എങ്കിൽ (c <= 0x1fff) {
        തിരികെ data1F00[c - 0x1f00];
    } അല്ലെങ്കിൽ എങ്കിൽ (c == 0x2126) {
        തിരികെ data2126;
    } അല്ലെങ്കിൽ {
        തിരികെ 0;
    }
}

uint32_t getDiacriticData(UChar32 c) {
    തിരഞ്ഞെടുപ്പ് (c) {
    അവസ്ഥ 0x0300:  // varia
    അവസ്ഥ 0x0301:  // tonos = oxia
    അവസ്ഥ 0x0342:  // perispomeni
    അവസ്ഥ 0x0302:  // circumflex can look like perispomeni
    അവസ്ഥ 0x0303:  // tilde can look like perispomeni
    അവസ്ഥ 0x0311:  // inverted breve can look like perispomeni
        തിരികെ HAS_ACCENT;
    അവസ്ഥ 0x0308:  // dialytika = diaeresis
        തിരികെ HAS_COMBINING_DIALYTIKA;
    അവസ്ഥ 0x0344:  // dialytika tonos
        തിരികെ HAS_COMBINING_DIALYTIKA | HAS_ACCENT;
    അവസ്ഥ 0x0345:  // ypogegrammeni = iota subscript
        തിരികെ HAS_YPOGEGRAMMENI;
    അവസ്ഥ 0x0304:  // macron
    അവസ്ഥ 0x0306:  // breve
    അവസ്ഥ 0x0313:  // comma above
    അവസ്ഥ 0x0314:  // reversed comma above
    അവസ്ഥ 0x0343:  // koronis
        തിരികെ HAS_OTHER_GREEK_DIACRITIC;
    സ്ഥിരസ്ഥിതി:
        തിരികെ 0;
    }
}

UBool isFollowedByCasedLetter(സ്ഥിരം char16_t *s, int32_t i, int32_t length) {
    ആകുമ്പോൾ (i < length) {
        UChar32 c;
        U16_NEXT(s, i, length, c);
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

/**
 * Greek string uppercasing with a state machine.
 * Probably simpler than a stateless function that has to figure out complex context-before
 * for each character.
 * TODO: Try to re-consolidate one way or another with the non-Greek function.
 */
int32_t toUpper(uint32_t options,
                char16_t *dest, int32_t destCapacity,
                സ്ഥിരം char16_t *src, int32_t srcLength,
                Edits *edits,
                UErrorCode &errorCode) {
    int32_t destIndex=0;
    uint32_t state = 0;
    വേണ്ടി (int32_t i = 0; i < srcLength;) {
        int32_t nextIndex = i;
        UChar32 c;
        U16_NEXT(src, nextIndex, srcLength, c);
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
                data |= (state & AFTER_VOWEL_WITH_PRECOMPOSED_ACCENT) ? HAS_DIALYTIKA
                                                                      : HAS_COMBINING_DIALYTIKA;
            }
            int32_t അക്കം_Ypogegrammeni = 0;  // Map each one to a trailing, spacing, capital iota.
            എങ്കിൽ ((data & HAS_YPOGEGRAMMENI) != 0) {
                അക്കം_Ypogegrammeni = 1;
            }
            സ്ഥിരം UBool hasPrecomposedAccent = (data & HAS_ACCENT) != 0;
            // Skip combining diacritics after this Greek letter.
            ആകുമ്പോൾ (nextIndex < srcLength) {
                uint32_t diacriticData = getDiacriticData(src[nextIndex]);
                എങ്കിൽ (diacriticData != 0) {
                    data |= diacriticData;
                    എങ്കിൽ ((diacriticData & HAS_YPOGEGRAMMENI) != 0) {
                        ++അക്കം_Ypogegrammeni;
                    }
                    ++nextIndex;
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
                change = src[i] != upper || അക്കം_Ypogegrammeni > 0;
                int32_t i2 = i + 1;
                എങ്കിൽ ((data & HAS_EITHER_DIALYTIKA) != 0) {
                    change |= i2 >= nextIndex || src[i2] != 0x308;
                    ++i2;
                }
                എങ്കിൽ (addTonos) {
                    change |= i2 >= nextIndex || src[i2] != 0x301;
                    ++i2;
                }
                int32_t oldLength = nextIndex - i;
                int32_t newLength = (i2 - i) + അക്കം_Ypogegrammeni;
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
                destIndex = appendUChar(dest, destIndex, destCapacity, static_cast<char16_t>(upper));
                എങ്കിൽ (destIndex >= 0 && (data & HAS_EITHER_DIALYTIKA) != 0) {
                    destIndex=appendUChar(dest, destIndex, destCapacity, 0x308);  // restore or add a dialytika
                }
                എങ്കിൽ (destIndex >= 0 && addTonos) {
                    destIndex=appendUChar(dest, destIndex, destCapacity, 0x301);
                }
                ആകുമ്പോൾ (destIndex >= 0 && അക്കം_Ypogegrammeni > 0) {
                    destIndex=appendUChar(dest, destIndex, destCapacity, 0x399);
                    --അക്കം_Ypogegrammeni;
                }
                എങ്കിൽ(destIndex<0) {
                    errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
                    തിരികെ 0;
                }
            }
        } അല്ലെങ്കിൽ {
            സ്ഥിരം char16_t *s;
            c=ucase_toFullUpper(c, nullptr, nullptr, &s, UCASE_LOC_GREEK);
            destIndex = appendResult(dest, destIndex, destCapacity, c, s,
                                     nextIndex - i, options, edits);
            എങ്കിൽ (destIndex < 0) {
                errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
                തിരികെ 0;
            }
        }
        i = nextIndex;
        state = nextState;
    }

    തിരികെ destIndex;
}

}  // namespace GreekUpper
U_NAMESPACE_END

/* functions available in the common library (for unistr_case.cpp) */

U_CFUNC int32_t U_CALLCONV
ustrcase_internalToLower(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_UNUSED
                         char16_t *dest, int32_t destCapacity,
                         സ്ഥിരം char16_t *src, int32_t srcLength,
                         icu::Edits *edits,
                         UErrorCode &errorCode) {
    UCaseContext csc=UCASECONTEXT_INITIALIZER;
    csc.p=(ശൂന്യം *)src;
    csc.limit=srcLength;
    int32_t destIndex = toLower(
        caseLocale, options,
        dest, destCapacity,
        src, &csc, 0, srcLength,
        edits, errorCode);
    തിരികെ checkOverflowAndEditsError(destIndex, destCapacity, edits, errorCode);
}

U_CFUNC int32_t U_CALLCONV
ustrcase_internalToUpper(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_UNUSED
                         char16_t *dest, int32_t destCapacity,
                         സ്ഥിരം char16_t *src, int32_t srcLength,
                         icu::Edits *edits,
                         UErrorCode &errorCode) {
    int32_t destIndex;
    എങ്കിൽ (caseLocale == UCASE_LOC_GREEK) {
        destIndex = GreekUpper::toUpper(options, dest, destCapacity,
                                        src, srcLength, edits, errorCode);
    } അല്ലെങ്കിൽ {
        UCaseContext csc=UCASECONTEXT_INITIALIZER;
        csc.p=(ശൂന്യം *)src;
        csc.limit=srcLength;
        destIndex = toUpper(
            caseLocale, options,
            dest, destCapacity,
            src, &csc, srcLength,
            edits, errorCode);
    }
    തിരികെ checkOverflowAndEditsError(destIndex, destCapacity, edits, errorCode);
}

U_CFUNC int32_t U_CALLCONV
ustrcase_internalFold(int32_t /* caseLocale */, uint32_t options, UCASEMAP_BREAK_ITERATOR_UNUSED
                      char16_t *dest, int32_t destCapacity,
                      സ്ഥിരം char16_t *src, int32_t srcLength,
                      icu::Edits *edits,
                      UErrorCode &errorCode) {
    int32_t destIndex = toLower(
        -1, options,
        dest, destCapacity,
        src, nullptr, 0, srcLength,
        edits, errorCode);
    തിരികെ checkOverflowAndEditsError(destIndex, destCapacity, edits, errorCode);
}

U_CFUNC int32_t
ustrcase_map(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_PARAM
             char16_t *dest, int32_t destCapacity,
             സ്ഥിരം char16_t *src, int32_t srcLength,
             UStringCaseMapper *stringCaseMapper,
             icu::Edits *edits,
             UErrorCode &errorCode) {
    int32_t destLength;

    /* check argument values */
    എങ്കിൽ(U_FAILURE(errorCode)) {
        തിരികെ 0;
    }
    എങ്കിൽ( destCapacity<0 ||
        (dest==nullptr && destCapacity>0) ||
        src==nullptr ||
        srcLength<-1
    ) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ 0;
    }

    /* get the string length */
    എങ്കിൽ(srcLength==-1) {
        srcLength=u_strlen(src);
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
    destLength=stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                                dest, destCapacity, src, srcLength, edits, errorCode);
    തിരികെ u_terminateUChars(dest, destCapacity, destLength, &errorCode);
}

U_CFUNC int32_t
ustrcase_mapWithOverlap(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_PARAM
                        char16_t *dest, int32_t destCapacity,
                        സ്ഥിരം char16_t *src, int32_t srcLength,
                        UStringCaseMapper *stringCaseMapper,
                        UErrorCode &errorCode) {
    char16_t buffer[300];
    char16_t *temp;

    int32_t destLength;

    /* check argument values */
    എങ്കിൽ(U_FAILURE(errorCode)) {
        തിരികെ 0;
    }
    എങ്കിൽ( destCapacity<0 ||
        (dest==nullptr && destCapacity>0) ||
        src==nullptr ||
        srcLength<-1
    ) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ 0;
    }

    /* get the string length */
    എങ്കിൽ(srcLength==-1) {
        srcLength=u_strlen(src);
    }

    /* check for overlapping source and destination */
    എങ്കിൽ( dest!=nullptr &&
        ((src>=dest && src<(dest+destCapacity)) ||
         (dest>=src && dest<(src+srcLength)))
    ) {
        /* overlap: provide a temporary destination buffer and later copy the result */
        എങ്കിൽ(destCapacity<=UPRV_LENGTHOF(buffer)) {
            /* the stack buffer is large enough */
            temp=buffer;
        } അല്ലെങ്കിൽ {
            /* allocate a buffer */
            temp=(char16_t *)uprv_malloc(destCapacity*U_SIZEOF_UCHAR);
            എങ്കിൽ(temp==nullptr) {
                errorCode=U_MEMORY_ALLOCATION_ERROR;
                തിരികെ 0;
            }
        }
    } അല്ലെങ്കിൽ {
        temp=dest;
    }

    destLength=stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                                temp, destCapacity, src, srcLength, nullptr, errorCode);
    എങ്കിൽ(temp!=dest) {
        /* copy the result string to the destination buffer */
        എങ്കിൽ (U_SUCCESS(errorCode) && 0 < destLength && destLength <= destCapacity) {
            u_memmove(dest, temp, destLength);
        }
        എങ്കിൽ(temp!=buffer) {
            uprv_free(temp);
        }
    }

    തിരികെ u_terminateUChars(dest, destCapacity, destLength, &errorCode);
}

/* public API functions */

U_CAPI int32_t U_EXPORT2
u_strFoldCase(char16_t *dest, int32_t destCapacity,
              സ്ഥിരം char16_t *src, int32_t srcLength,
              uint32_t options,
              UErrorCode *pErrorCode) {
    തിരികെ ustrcase_mapWithOverlap(
        UCASE_LOC_വേര്, options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, destCapacity,
        src, srcLength,
        ustrcase_internalFold, *pErrorCode);
}

U_NAMESPACE_BEGIN

int32_t CaseMap::fold(
        uint32_t options,
        സ്ഥിരം char16_t *src, int32_t srcLength,
        char16_t *dest, int32_t destCapacity, Edits *edits,
        UErrorCode &errorCode) {
    തിരികെ ustrcase_map(
        UCASE_LOC_വേര്, options, UCASEMAP_BREAK_ITERATOR_NULL
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
ഘടന CmpEquivLevel {
    സ്ഥിരം char16_t *start, *s, *limit;
};
തരനിർവചനം ഘടന CmpEquivLevel CmpEquivLevel;

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
സ്ഥിതം int32_t _cmpFold(
            സ്ഥിരം char16_t *s1, int32_t length1,
            സ്ഥിരം char16_t *s2, int32_t length2,
            uint32_t options,
            int32_t *matchLen1, int32_t *matchLen2,
            UErrorCode *pErrorCode) {
    int32_t cmpRes = 0;

    /* current-level start/limit - s1/s2 as current */
    സ്ഥിരം char16_t *start1, *start2, *limit1, *limit2;

    /* points to the original start address */
    സ്ഥിരം char16_t *org1, *org2;

    /* points to the end of match + 1 */
    സ്ഥിരം char16_t *m1, *m2;

    /* case folding variables */
    സ്ഥിരം char16_t *p;
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
    എങ്കിൽ(U_FAILURE(*pErrorCode)) {
        തിരികെ 0;
    }

    /* initialize */
    എങ്കിൽ(matchLen1) {
        U_ASSERT(matchLen2 !=nullptr);
        *matchLen1=0;
        *matchLen2=0;
    }

    start1=m1=org1=s1;
    എങ്കിൽ(length1==-1) {
        limit1=nullptr;
    } അല്ലെങ്കിൽ {
        limit1=s1+length1;
    }

    start2=m2=org2=s2;
    എങ്കിൽ(length2==-1) {
        limit2=nullptr;
    } അല്ലെങ്കിൽ {
        limit2=s2+length2;
    }

    level1=level2=0;
    c1=c2=-1;

    /* comparison loop */
    വേണ്ടി(;;) {
        /*
         * here a code unit value of -1 means "get another code unit"
         * below it will mean "this source is finished"
         */

        എങ്കിൽ(c1<0) {
            /* get next code unit from string 1, post-increment */
            വേണ്ടി(;;) {
                എങ്കിൽ(s1==limit1 || ((c1=*s1)==0 && (limit1==nullptr || (options&_STRNCMP_STYLE)))) {
                    എങ്കിൽ(level1==0) {
                        c1=-1;
                        നിർത്തുക;
                    }
                } അല്ലെങ്കിൽ {
                    ++s1;
                    നിർത്തുക;
                }

                /* reached end of level buffer, pop one level */
                ചെയ്യുക {
                    --level1;
                    start1=stack1[level1].start;    /*Not uninitialized*/
                } ആകുമ്പോൾ(start1==nullptr);
                s1=stack1[level1].s;                /*Not uninitialized*/
                limit1=stack1[level1].limit;        /*Not uninitialized*/
            }
        }

        എങ്കിൽ(c2<0) {
            /* get next code unit from string 2, post-increment */
            വേണ്ടി(;;) {
                എങ്കിൽ(s2==limit2 || ((c2=*s2)==0 && (limit2==nullptr || (options&_STRNCMP_STYLE)))) {
                    എങ്കിൽ(level2==0) {
                        c2=-1;
                        നിർത്തുക;
                    }
                } അല്ലെങ്കിൽ {
                    ++s2;
                    നിർത്തുക;
                }

                /* reached end of level buffer, pop one level */
                ചെയ്യുക {
                    --level2;
                    start2=stack2[level2].start;    /*Not uninitialized*/
                } ആകുമ്പോൾ(start2==nullptr);
                s2=stack2[level2].s;                /*Not uninitialized*/
                limit2=stack2[level2].limit;        /*Not uninitialized*/
            }
        }

        /*
         * compare c1 and c2
         * either variable c1, c2 is -1 only if the corresponding string is finished
         */
        എങ്കിൽ(c1==c2) {
            സ്ഥിരം char16_t *next1, *next2;

            എങ്കിൽ(c1<0) {
                cmpRes=0;   /* c1==c2==-1 indicating end of strings */
                നിർത്തുക;
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
            എങ്കിൽ(level1==0) {
                next1=s1;
            } അല്ലെങ്കിൽ എങ്കിൽ(s1==limit1) {
                /* Note: This implementation only use a single level of stack.
                 *      If this code needs to be changed to use multiple levels
                 *      of stacks, the code above should check if the current
                 *      code is at the end of all stacks.
                 */
                U_ASSERT(level1==1);

                /* is s1 at the end of the current stack? */
                next1=stack1[0].s;
            }

            എങ്കിൽ (next1!=nullptr) {
                എങ്കിൽ(level2==0) {
                    next2=s2;
                } അല്ലെങ്കിൽ എങ്കിൽ(s2==limit2) {
                    U_ASSERT(level2==1);

                    /* is s2 at the end of the current stack? */
                    next2=stack2[0].s;
                }
                എങ്കിൽ(next2!=nullptr) {
                    m1=next1;
                    m2=next2;
                }
            }
            c1=c2=-1;       /* make us fetch new code units */
            തുടരുക;
        } അല്ലെങ്കിൽ എങ്കിൽ(c1<0) {
            cmpRes=-1;      /* string 1 ends before string 2 */
            നിർത്തുക;
        } അല്ലെങ്കിൽ എങ്കിൽ(c2<0) {
            cmpRes=1;       /* string 2 ends before string 1 */
            നിർത്തുക;
        }
        /* c1!=c2 && c1>=0 && c2>=0 */

        /* get complete code points for c1, c2 for lookups if either is a surrogate */
        cp1=c1;
        എങ്കിൽ(U_IS_SURROGATE(c1)) {
            char16_t c;

            എങ്കിൽ(U_IS_SURROGATE_LEAD(c1)) {
                എങ്കിൽ(s1!=limit1 && U16_IS_TRAIL(c=*s1)) {
                    /* advance ++s1; only below if cp1 decomposes/case-folds */
                    cp1=U16_GET_SUPPLEMENTARY(c1, c);
                }
            } അല്ലെങ്കിൽ /* isTrail(c1) */ {
                എങ്കിൽ(start1<=(s1-2) && U16_IS_LEAD(c=*(s1-2))) {
                    cp1=U16_GET_SUPPLEMENTARY(c, c1);
                }
            }
        }

        cp2=c2;
        എങ്കിൽ(U_IS_SURROGATE(c2)) {
            char16_t c;

            എങ്കിൽ(U_IS_SURROGATE_LEAD(c2)) {
                എങ്കിൽ(s2!=limit2 && U16_IS_TRAIL(c=*s2)) {
                    /* advance ++s2; only below if cp2 decomposes/case-folds */
                    cp2=U16_GET_SUPPLEMENTARY(c2, c);
                }
            } അല്ലെങ്കിൽ /* isTrail(c2) */ {
                എങ്കിൽ(start2<=(s2-2) && U16_IS_LEAD(c=*(s2-2))) {
                    cp2=U16_GET_SUPPLEMENTARY(c, c2);
                }
            }
        }

        /*
         * go down one level for each string
         * continue with the main loop as soon as there is a real change
         */

        എങ്കിൽ( level1==0 &&
            (length = ucase_toFullFolding(cp1, &p, options)) >= 0
        ) {
            /* cp1 case-folds to the code point "length" or to p[length] */
            എങ്കിൽ(U_IS_SURROGATE(c1)) {
                എങ്കിൽ(U_IS_SURROGATE_LEAD(c1)) {
                    /* advance beyond source surrogate pair if it case-folds */
                    ++s1;
                } അല്ലെങ്കിൽ /* isTrail(c1) */ {
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
            എങ്കിൽ(length<=UCASE_MAX_STRING_LENGTH) {
                u_memcpy(fold1, p, length);
            } അല്ലെങ്കിൽ {
                int32_t i=0;
                U16_APPEND_UNSAFE(fold1, i, length);
                length=i;
            }

            /* set next level pointers to case folding */
            start1=s1=fold1;
            limit1=fold1+length;

            /* get ready to read from decomposition, continue with loop */
            c1=-1;
            തുടരുക;
        }

        എങ്കിൽ( level2==0 &&
            (length = ucase_toFullFolding(cp2, &p, options)) >= 0
        ) {
            /* cp2 case-folds to the code point "length" or to p[length] */
            എങ്കിൽ(U_IS_SURROGATE(c2)) {
                എങ്കിൽ(U_IS_SURROGATE_LEAD(c2)) {
                    /* advance beyond source surrogate pair if it case-folds */
                    ++s2;
                } അല്ലെങ്കിൽ /* isTrail(c2) */ {
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
            എങ്കിൽ(length<=UCASE_MAX_STRING_LENGTH) {
                u_memcpy(fold2, p, length);
            } അല്ലെങ്കിൽ {
                int32_t i=0;
                U16_APPEND_UNSAFE(fold2, i, length);
                length=i;
            }

            /* set next level pointers to case folding */
            start2=s2=fold2;
            limit2=fold2+length;

            /* get ready to read from decomposition, continue with loop */
            c2=-1;
            തുടരുക;
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

        എങ്കിൽ(c1>=0xd800 && c2>=0xd800 && (options&U_COMPARE_CODE_POINT_ORDER)) {
            /* subtract 0x2800 from BMP code points to make them smaller than supplementary ones */
            എങ്കിൽ(
                (c1<=0xdbff && s1!=limit1 && U16_IS_TRAIL(*s1)) ||
                (U16_IS_TRAIL(c1) && start1!=(s1-1) && U16_IS_LEAD(*(s1-2)))
            ) {
                /* part of a surrogate pair, leave >=d800 */
            } അല്ലെങ്കിൽ {
                /* BMP code point - may be surrogate code point - make <d800 */
                c1-=0x2800;
            }

            എങ്കിൽ(
                (c2<=0xdbff && s2!=limit2 && U16_IS_TRAIL(*s2)) ||
                (U16_IS_TRAIL(c2) && start2!=(s2-1) && U16_IS_LEAD(*(s2-2)))
            ) {
                /* part of a surrogate pair, leave >=d800 */
            } അല്ലെങ്കിൽ {
                /* BMP code point - may be surrogate code point - make <d800 */
                c2-=0x2800;
            }
        }

        cmpRes=c1-c2;
        നിർത്തുക;
    }

    എങ്കിൽ(matchLen1) {
        *matchLen1=static_cast<int32_t>(m1-org1);
        *matchLen2=static_cast<int32_t>(m2-org2);
    }
    തിരികെ cmpRes;
}

/* internal function */
U_CFUNC int32_t
u_strcmpFold(സ്ഥിരം char16_t *s1, int32_t length1,
             സ്ഥിരം char16_t *s2, int32_t length2,
             uint32_t options,
             UErrorCode *pErrorCode) {
    തിരികെ _cmpFold(s1, length1, s2, length2, options, nullptr, nullptr, pErrorCode);
}

/* public API functions */

U_CAPI int32_t U_EXPORT2
u_strCaseCompare(സ്ഥിരം char16_t *s1, int32_t length1,
                 സ്ഥിരം char16_t *s2, int32_t length2,
                 uint32_t options,
                 UErrorCode *pErrorCode) {
    /* argument checking */
    എങ്കിൽ (pErrorCode == nullptr || U_FAILURE(*pErrorCode)) {
        തിരികെ 0;
    }
    എങ്കിൽ(s1==nullptr || length1<-1 || s2==nullptr || length2<-1) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ 0;
    }
    തിരികെ u_strcmpFold(s1, length1, s2, length2,
                        options|U_COMPARE_IGNORE_CASE,
                        pErrorCode);
}

U_CAPI int32_t U_EXPORT2
u_strcasecmp(സ്ഥിരം char16_t *s1, സ്ഥിരം char16_t *s2, uint32_t options) {
    UErrorCode errorCode=U_ZERO_ERROR;
    തിരികെ u_strcmpFold(s1, -1, s2, -1,
                        options|U_COMPARE_IGNORE_CASE,
                        &errorCode);
}

U_CAPI int32_t U_EXPORT2
u_memcasecmp(സ്ഥിരം char16_t *s1, സ്ഥിരം char16_t *s2, int32_t length, uint32_t options) {
    UErrorCode errorCode=U_ZERO_ERROR;
    തിരികെ u_strcmpFold(s1, length, s2, length,
                        options|U_COMPARE_IGNORE_CASE,
                        &errorCode);
}

U_CAPI int32_t U_EXPORT2
u_strncasecmp(സ്ഥിരം char16_t *s1, സ്ഥിരം char16_t *s2, int32_t n, uint32_t options) {
    UErrorCode errorCode=U_ZERO_ERROR;
    തിരികെ u_strcmpFold(s1, n, s2, n,
                        options|(U_COMPARE_IGNORE_CASE|_STRNCMP_STYLE),
                        &errorCode);
}

/* internal API - detect length of shared prefix */
U_CAPI ശൂന്യം
u_caseInsensitivePrefixMatch(സ്ഥിരം char16_t *s1, int32_t length1,
                             സ്ഥിരം char16_t *s2, int32_t length2,
                             uint32_t options,
                             int32_t *matchLen1, int32_t *matchLen2,
                             UErrorCode *pErrorCode) {
    _cmpFold(s1, length1, s2, length2, options,
        matchLen1, matchLen2, pErrorCode);
}
