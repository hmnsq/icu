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

#அந்தர்பாவ "unicode/utypes.h"
#அந்தர்பாவ "unicode/brkiter.h"
#அந்தர்பாவ "unicode/casemap.h"
#அந்தர்பாவ "unicode/edits.h"
#அந்தர்பாவ "unicode/stringoptions.h"
#அந்தர்பாவ "unicode/ustring.h"
#அந்தர்பாவ "unicode/ucasemap.h"
#அந்தர்பாவ "unicode/ubrk.h"
#அந்தர்பாவ "unicode/utf.h"
#அந்தர்பாவ "unicode/utf16.h"
#அந்தர்பாவ "cmemory.h"
#அந்தர்பாவ "ucase.h"
#அந்தர்பாவ "ucasemap_imp.h"
#அந்தர்பாவ "ustr_imp.h"
#அந்தர்பாவ "uassert.h"

/**
 * Code point for COMBINING ACUTE ACCENT
 * @internal
 */
#define ACUTE u'\u0301'

U_NAMESPACE_BEGIN

நாமாகாஶ {

int32_t checkOverflowAndEditsError(int32_t destIndex, int32_t destCapacity,
                                   Edits *edits, UErrorCode &errorCode) {
    எனில் (U_SUCCESS(errorCode)) {
        எனில் (destIndex > destCapacity) {
            errorCode = U_BUFFER_OVERFLOW_ERROR;
        } இல்லையெனில் எனில் (edits != nullptr) {
            edits->copyErrorTo(errorCode);
        }
    }
    திரும்பு destIndex;
}

/* Appends a full case mapping result, see UCASE_MAX_STRING_LENGTH. */
உள்வரி int32_t
appendResult(char16_t *dest, int32_t destIndex, int32_t destCapacity,
             int32_t result, மாறிலி char16_t *s,
             int32_t cpLength, uint32_t options, icu::Edits *edits) {
    UChar32 c;
    int32_t length;

    /* decode the result */
    எனில்(result<0) {
        /* (not) original code point */
        எனில்(edits!=nullptr) {
            edits->addUnchanged(cpLength);
        }
        எனில்(options & U_OMIT_UNCHANGED_TEXT) {
            திரும்பு destIndex;
        }
        c=~result;
        எனில்(destIndex<destCapacity && c<=0xffff) {  // BMP slightly-fastpath
            dest[destIndex++] = static_cast<char16_t>(c);
            திரும்பு destIndex;
        }
        length=cpLength;
    } இல்லையெனில் {
        எனில்(result<=UCASE_MAX_STRING_LENGTH) {
            c=U_SENTINEL;
            length=result;
        } இல்லையெனில் எனில்(destIndex<destCapacity && result<=0xffff) {  // BMP slightly-fastpath
            dest[destIndex++] = static_cast<char16_t>(result);
            எனில்(edits!=nullptr) {
                edits->addReplace(cpLength, 1);
            }
            திரும்பு destIndex;
        } இல்லையெனில் {
            c=result;
            length=U16_LENGTH(c);
        }
        எனில்(edits!=nullptr) {
            edits->addReplace(cpLength, length);
        }
    }
    எனில்(length>(INT32_MAX-destIndex)) {
        திரும்பு -1;  // integer overflow
    }

    எனில்(destIndex<destCapacity) {
        /* append the result */
        எனில்(c>=0) {
            /* code point */
            UBool isError=பொய்;
            U16_APPEND(dest, destIndex, destCapacity, c, isError);
            எனில்(isError) {
                /* overflow, nothing written */
                destIndex+=length;
            }
        } இல்லையெனில் {
            /* string */
            எனில்((destIndex+length)<=destCapacity) {
                போது(length>0) {
                    dest[destIndex++]=*s++;
                    --length;
                }
            } இல்லையெனில் {
                /* overflow */
                destIndex+=length;
            }
        }
    } இல்லையெனில் {
        /* preflight */
        destIndex+=length;
    }
    திரும்பு destIndex;
}

உள்வரி int32_t
appendUChar(char16_t *dest, int32_t destIndex, int32_t destCapacity, char16_t c) {
    எனில்(destIndex<destCapacity) {
        dest[destIndex]=c;
    } இல்லையெனில் எனில்(destIndex==INT32_MAX) {
        திரும்பு -1;  // integer overflow
    }
    திரும்பு destIndex+1;
}

int32_t
appendNonEmptyUnchanged(char16_t *dest, int32_t destIndex, int32_t destCapacity,
                        மாறிலி char16_t *s, int32_t length, uint32_t options, icu::Edits *edits) {
    எனில்(edits!=nullptr) {
        edits->addUnchanged(length);
    }
    எனில்(options & U_OMIT_UNCHANGED_TEXT) {
        திரும்பு destIndex;
    }
    எனில்(length>(INT32_MAX-destIndex)) {
        திரும்பு -1;  // integer overflow
    }
    எனில்((destIndex+length)<=destCapacity) {
        u_memcpy(dest+destIndex, s, length);
    }
    திரும்பு destIndex + length;
}

உள்வரி int32_t
appendUnchanged(char16_t *dest, int32_t destIndex, int32_t destCapacity,
                மாறிலி char16_t *s, int32_t length, uint32_t options, icu::Edits *edits) {
    எனில் (length <= 0) {
        திரும்பு destIndex;
    }
    திரும்பு appendNonEmptyUnchanged(dest, destIndex, destCapacity, s, length, options, edits);
}

UChar32 U_CALLCONV
utf16_caseContextIterator(வெறுமை *context, int8_t dir) {
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
            U16_PREV((மாறிலி char16_t *)csc->p, csc->start, csc->index, c);
            திரும்பு c;
        }
    } இல்லையெனில் {
        எனில்(csc->index<csc->limit) {
            U16_NEXT((மாறிலி char16_t *)csc->p, csc->index, csc->limit, c);
            திரும்பு c;
        }
    }
    திரும்பு U_SENTINEL;
}

/**
 * caseLocale >= 0: Lowercases [srcStart..srcLimit[ but takes context [0..srcLength[ into account.
 * caseLocale < 0: Case-folds [srcStart..srcLimit[.
 */
int32_t toLower(int32_t caseLocale, uint32_t options,
                char16_t *dest, int32_t destCapacity,
                மாறிலி char16_t *src, UCaseContext *csc, int32_t srcStart, int32_t srcLimit,
                icu::Edits *edits, UErrorCode &errorCode) {
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
    int32_t destIndex = 0;
    int32_t prev = srcStart;
    int32_t srcIndex = srcStart;
    க்கு (;;) {
        // fast path for simple cases
        char16_t lead = 0;
        போது (srcIndex < srcLimit) {
            lead = src[srcIndex];
            int32_t delta;
            எனில் (lead < LatinCase::LONG_S) {
                int8_t d = latinToLower[lead];
                எனில் (d == LatinCase::EXC) { நிறுத்து; }
                ++srcIndex;
                எனில் (d == 0) { தொடர்; }
                delta = d;
            } இல்லையெனில் எனில் (lead >= 0xd800) {
                நிறுத்து;  // surrogate or higher
            } இல்லையெனில் {
                uint16_t props = UTRIE2_GET16_FROM_U16_SINGLE_LEAD(trie, lead);
                எனில் (UCASE_HAS_EXCEPTION(props)) { நிறுத்து; }
                ++srcIndex;
                எனில் (!UCASE_IS_UPPER_OR_TITLE(props) || (delta = UCASE_GET_DELTA(props)) == 0) {
                    தொடர்;
                }
            }
            lead += static_cast<char16_t>(delta);
            destIndex = appendUnchanged(dest, destIndex, destCapacity,
                                        src + prev, srcIndex - 1 - prev, options, edits);
            எனில் (destIndex >= 0) {
                destIndex = appendUChar(dest, destIndex, destCapacity, lead);
                எனில் (edits != nullptr) {
                    edits->addReplace(1, 1);
                }
            }
            எனில் (destIndex < 0) {
                errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
                திரும்பு 0;
            }
            prev = srcIndex;
        }
        எனில் (srcIndex >= srcLimit) {
            நிறுத்து;
        }
        // slow path
        int32_t cpStart = srcIndex++;
        char16_t trail;
        UChar32 c;
        எனில் (U16_IS_LEAD(lead) && srcIndex < srcLimit && U16_IS_TRAIL(trail = src[srcIndex])) {
            c = U16_GET_SUPPLEMENTARY(lead, trail);
            ++srcIndex;
        } இல்லையெனில் {
            c = lead;
        }
        மாறிலி char16_t *s = nullptr;
        எனில் (caseLocale >= 0) {
            csc->cpStart = cpStart;
            csc->cpLimit = srcIndex;
            c = ucase_toFullLower(c, utf16_caseContextIterator, csc, &s, caseLocale);
        } இல்லையெனில் {
            c = ucase_toFullFolding(c, &s, options);
        }
        எனில் (c >= 0) {
            destIndex = appendUnchanged(dest, destIndex, destCapacity,
                                        src + prev, cpStart - prev, options, edits);
            எனில் (destIndex >= 0) {
                destIndex = appendResult(dest, destIndex, destCapacity, c, s,
                                         srcIndex - cpStart, options, edits);
            }
            எனில் (destIndex < 0) {
                errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
                திரும்பு 0;
            }
            prev = srcIndex;
        }
    }
    destIndex = appendUnchanged(dest, destIndex, destCapacity,
                                src + prev, srcIndex - prev, options, edits);
    எனில் (destIndex < 0) {
        errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
        திரும்பு 0;
    }
    திரும்பு destIndex;
}

int32_t toUpper(int32_t caseLocale, uint32_t options,
                char16_t *dest, int32_t destCapacity,
                மாறிலி char16_t *src, UCaseContext *csc, int32_t srcLength,
                icu::Edits *edits, UErrorCode &errorCode) {
    மாறிலி int8_t *latinToUpper;
    எனில் (caseLocale == UCASE_LOC_TURKISH) {
        latinToUpper = LatinCase::TO_UPPER_TR;
    } இல்லையெனில் {
        latinToUpper = LatinCase::TO_UPPER_NORMAL;
    }
    மாறிலி UTrie2 *trie = ucase_getTrie();
    int32_t destIndex = 0;
    int32_t prev = 0;
    int32_t srcIndex = 0;
    க்கு (;;) {
        // fast path for simple cases
        char16_t lead = 0;
        போது (srcIndex < srcLength) {
            lead = src[srcIndex];
            int32_t delta;
            எனில் (lead < LatinCase::LONG_S) {
                int8_t d = latinToUpper[lead];
                எனில் (d == LatinCase::EXC) { நிறுத்து; }
                ++srcIndex;
                எனில் (d == 0) { தொடர்; }
                delta = d;
            } இல்லையெனில் எனில் (lead >= 0xd800) {
                நிறுத்து;  // surrogate or higher
            } இல்லையெனில் {
                uint16_t props = UTRIE2_GET16_FROM_U16_SINGLE_LEAD(trie, lead);
                எனில் (UCASE_HAS_EXCEPTION(props)) { நிறுத்து; }
                ++srcIndex;
                எனில் (UCASE_GET_TYPE(props) != UCASE_LOWER || (delta = UCASE_GET_DELTA(props)) == 0) {
                    தொடர்;
                }
            }
            lead += static_cast<char16_t>(delta);
            destIndex = appendUnchanged(dest, destIndex, destCapacity,
                                        src + prev, srcIndex - 1 - prev, options, edits);
            எனில் (destIndex >= 0) {
                destIndex = appendUChar(dest, destIndex, destCapacity, lead);
                எனில் (edits != nullptr) {
                    edits->addReplace(1, 1);
                }
            }
            எனில் (destIndex < 0) {
                errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
                திரும்பு 0;
            }
            prev = srcIndex;
        }
        எனில் (srcIndex >= srcLength) {
            நிறுத்து;
        }
        // slow path
        int32_t cpStart;
        csc->cpStart = cpStart = srcIndex++;
        char16_t trail;
        UChar32 c;
        எனில் (U16_IS_LEAD(lead) && srcIndex < srcLength && U16_IS_TRAIL(trail = src[srcIndex])) {
            c = U16_GET_SUPPLEMENTARY(lead, trail);
            ++srcIndex;
        } இல்லையெனில் {
            c = lead;
        }
        csc->cpLimit = srcIndex;
        மாறிலி char16_t *s = nullptr;
        c = ucase_toFullUpper(c, utf16_caseContextIterator, csc, &s, caseLocale);
        எனில் (c >= 0) {
            destIndex = appendUnchanged(dest, destIndex, destCapacity,
                                        src + prev, cpStart - prev, options, edits);
            எனில் (destIndex >= 0) {
                destIndex = appendResult(dest, destIndex, destCapacity, c, s,
                                         srcIndex - cpStart, options, edits);
            }
            எனில் (destIndex < 0) {
                errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
                திரும்பு 0;
            }
            prev = srcIndex;
        }
    }
    destIndex = appendUnchanged(dest, destIndex, destCapacity,
                                src + prev, srcIndex - prev, options, edits);
    எனில் (destIndex < 0) {
        errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
        திரும்பு 0;
    }
    திரும்பு destIndex;
}

}  // namespace

U_NAMESPACE_END

U_NAMESPACE_USE

#எனில் !UCONFIG_NO_BREAK_ITERATION

நாமாகாஶ {

/**
 * Input: c is a letter I with or without acute accent.
 * start is the index in src after c, and is less than segmentLimit.
 * If a plain i/I is followed by a plain j/J,
 * or an i/I with acute (precomposed or decomposed) is followed by a j/J with acute,
 * then we output accordingly.
 *
 * @return the src index after the titlecased sequence, or the start index if no Dutch IJ
 */
int32_t maybeTitleDutchIJ(மாறிலி char16_t *src, UChar32 c, int32_t start, int32_t segmentLimit,
                          char16_t *dest, int32_t &destIndex, int32_t destCapacity, uint32_t options,
                          icu::Edits *edits) {
    U_ASSERT(start < segmentLimit);

    int32_t index = start;
    தருக்க withAcute = பொய்;

    // If the conditions are met, then the following variables tell us what to output.
    int32_t unchanged1 = 0;  // code units before the j, or the whole sequence (0..3)
    தருக்க doTitleJ = பொய்;  // true if the j needs to be titlecased
    int32_t unchanged2 = 0;  // after the j (0 or 1)

    // next character after the first letter
    char16_t c2 = src[index++];

    // Is the first letter an i/I with accent?
    எனில் (c == u'I') {
        எனில் (c2 == ACUTE) {
            withAcute = உண்மை;
            unchanged1 = 1;
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
        எனில் (index == segmentLimit || src[index++] != ACUTE) { திரும்பு start; }
        எனில் (doTitleJ) {
            unchanged2 = 1;
        } இல்லையெனில் {
            ++unchanged1;
        }
    }

    // There must not be another combining mark.
    எனில் (index < segmentLimit) {
        int32_t cp;
        int32_t i = index;
        U16_NEXT(src, i, segmentLimit, cp);
        uint32_t typeMask = U_GET_GC_MASK(cp);
        எனில் ((typeMask & U_GC_M_MASK) != 0) {
            திரும்பு start;
        }
    }

    // Output the rest of the Dutch IJ.
    destIndex = appendUnchanged(dest, destIndex, destCapacity, src + start, unchanged1, options, edits);
    start += unchanged1;
    எனில் (doTitleJ) {
        destIndex = appendUChar(dest, destIndex, destCapacity, u'J');
        எனில் (edits != nullptr) {
            edits->addReplace(1, 1);
        }
        ++start;
    }
    destIndex = appendUnchanged(dest, destIndex, destCapacity, src + start, unchanged2, options, edits);

    U_ASSERT(start + unchanged2 == index);
    திரும்பு index;
}

}  // namespace

U_CFUNC int32_t U_CALLCONV
ustrcase_internalToTitle(int32_t caseLocale, uint32_t options, BreakIterator *iter,
                         char16_t *dest, int32_t destCapacity,
                         மாறிலி char16_t *src, int32_t srcLength,
                         icu::Edits *edits,
                         UErrorCode &errorCode) {
    எனில் (!ustrcase_checkTitleAdjustmentOptions(options, errorCode)) {
        திரும்பு 0;
    }

    /* set up local variables */
    UCaseContext csc=UCASECONTEXT_INITIALIZER;
    csc.p=(வெறுமை *)src;
    csc.limit=srcLength;
    int32_t destIndex=0;
    int32_t prev=0;
    தருக்க isFirstIndex=உண்மை;

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
            // Find and copy skipped characters [prev..titleStart[
            int32_t titleStart=prev;
            int32_t titleLimit=prev;
            UChar32 c;
            U16_NEXT(src, titleLimit, index, c);
            எனில் ((options&U_TITLECASE_NO_BREAK_ADJUSTMENT)==0) {
                // Adjust the titlecasing index to the next cased character,
                // or to the next letter/number/symbol/private use.
                // Stop with titleStart<titleLimit<=index
                // if there is a character to be titlecased,
                // or else stop with titleStart==titleLimit==index.
                தருக்க toCased = (options&U_TITLECASE_ADJUST_TO_CASED) != 0;
                போது (toCased ? UCASE_NONE==ucase_getType(c) : !ustrcase_isLNS(c)) {
                    titleStart=titleLimit;
                    எனில்(titleLimit==index) {
                        நிறுத்து;
                    }
                    U16_NEXT(src, titleLimit, index, c);
                }
                எனில் (prev < titleStart) {
                    destIndex=appendUnchanged(dest, destIndex, destCapacity,
                                              src+prev, titleStart-prev, options, edits);
                    எனில்(destIndex<0) {
                        errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
                        திரும்பு 0;
                    }
                }
            }

            எனில்(titleStart<titleLimit) {
                /* titlecase c which is from [titleStart..titleLimit[ */
                csc.cpStart=titleStart;
                csc.cpLimit=titleLimit;
                மாறிலி char16_t *s;
                c=ucase_toFullTitle(c, utf16_caseContextIterator, &csc, &s, caseLocale);
                destIndex=appendResult(dest, destIndex, destCapacity, c, s,
                                       titleLimit-titleStart, options, edits);
                எனில்(destIndex<0) {
                    errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
                    திரும்பு 0;
                }

                /* Special case Dutch IJ titlecasing */
                எனில் (titleStart+1 < index &&
                        caseLocale == UCASE_LOC_DUTCH) {
                    எனில் (c < 0) {
                        c = ~c;
                    }

                    எனில் (c == u'I' || c == u'Í') {
                        titleLimit = maybeTitleDutchIJ(src, c, titleStart + 1, index,
                                                       dest, destIndex, destCapacity, options,
                                                       edits);
                    }
                }

                /* lowercase [titleLimit..index[ */
                எனில்(titleLimit<index) {
                    எனில்((options&U_TITLECASE_NO_LOWERCASE)==0) {
                        /* Normal operation: Lowercase the rest of the word. */
                        destIndex+=
                            toLower(
                                caseLocale, options,
                                (dest==nullptr) ? nullptr: dest+destIndex, destCapacity-destIndex,
                                src, &csc, titleLimit, index,
                                edits, errorCode);
                        எனில்(errorCode==U_BUFFER_OVERFLOW_ERROR) {
                            errorCode=U_ZERO_ERROR;
                        }
                        எனில்(U_FAILURE(errorCode)) {
                            திரும்பு destIndex;
                        }
                    } இல்லையெனில் {
                        /* Optionally just copy the rest of the word unchanged. */
                        destIndex=appendUnchanged(dest, destIndex, destCapacity,
                                                  src+titleLimit, index-titleLimit, options, edits);
                        எனில்(destIndex<0) {
                            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
                            திரும்பு 0;
                        }
                    }
                }
            }
        }

        prev=index;
    }

    திரும்பு checkOverflowAndEditsError(destIndex, destCapacity, edits, errorCode);
}

#endif  // !UCONFIG_NO_BREAK_ITERATION

U_NAMESPACE_BEGIN
நாமாகாஶ GreekUpper {

// Data generated by prototype code, see
// https://icu.unicode.org/design/case/greek-upper
// TODO: Move this data into ucase.icu.
நிலையான மாறிலி uint16_t data0370[] = {
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

நிலையான மாறிலி uint16_t data1F00[] = {
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
நிலையான மாறிலி uint16_t data2126 = 0x03A9 | HAS_VOWEL;

uint32_t getLetterData(UChar32 c) {
    எனில் (c < 0x370 || 0x2126 < c || (0x3ff < c && c < 0x1f00)) {
        திரும்பு 0;
    } இல்லையெனில் எனில் (c <= 0x3ff) {
        திரும்பு data0370[c - 0x370];
    } இல்லையெனில் எனில் (c <= 0x1fff) {
        திரும்பு data1F00[c - 0x1f00];
    } இல்லையெனில் எனில் (c == 0x2126) {
        திரும்பு data2126;
    } இல்லையெனில் {
        திரும்பு 0;
    }
}

uint32_t getDiacriticData(UChar32 c) {
    தேர்வு (c) {
    நிலை 0x0300:  // varia
    நிலை 0x0301:  // tonos = oxia
    நிலை 0x0342:  // perispomeni
    நிலை 0x0302:  // circumflex can look like perispomeni
    நிலை 0x0303:  // tilde can look like perispomeni
    நிலை 0x0311:  // inverted breve can look like perispomeni
        திரும்பு HAS_ACCENT;
    நிலை 0x0308:  // dialytika = diaeresis
        திரும்பு HAS_COMBINING_DIALYTIKA;
    நிலை 0x0344:  // dialytika tonos
        திரும்பு HAS_COMBINING_DIALYTIKA | HAS_ACCENT;
    நிலை 0x0345:  // ypogegrammeni = iota subscript
        திரும்பு HAS_YPOGEGRAMMENI;
    நிலை 0x0304:  // macron
    நிலை 0x0306:  // breve
    நிலை 0x0313:  // comma above
    நிலை 0x0314:  // reversed comma above
    நிலை 0x0343:  // koronis
        திரும்பு HAS_OTHER_GREEK_DIACRITIC;
    இயல்பு:
        திரும்பு 0;
    }
}

UBool isFollowedByCasedLetter(மாறிலி char16_t *s, int32_t i, int32_t length) {
    போது (i < length) {
        UChar32 c;
        U16_NEXT(s, i, length, c);
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

/**
 * Greek string uppercasing with a state machine.
 * Probably simpler than a stateless function that has to figure out complex context-before
 * for each character.
 * TODO: Try to re-consolidate one way or another with the non-Greek function.
 */
int32_t toUpper(uint32_t options,
                char16_t *dest, int32_t destCapacity,
                மாறிலி char16_t *src, int32_t srcLength,
                Edits *edits,
                UErrorCode &errorCode) {
    int32_t destIndex=0;
    uint32_t state = 0;
    க்கு (int32_t i = 0; i < srcLength;) {
        int32_t nextIndex = i;
        UChar32 c;
        U16_NEXT(src, nextIndex, srcLength, c);
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
                data |= (state & AFTER_VOWEL_WITH_PRECOMPOSED_ACCENT) ? HAS_DIALYTIKA
                                                                      : HAS_COMBINING_DIALYTIKA;
            }
            int32_t இலக்கம்_Ypogegrammeni = 0;  // Map each one to a trailing, spacing, capital iota.
            எனில் ((data & HAS_YPOGEGRAMMENI) != 0) {
                இலக்கம்_Ypogegrammeni = 1;
            }
            மாறிலி UBool hasPrecomposedAccent = (data & HAS_ACCENT) != 0;
            // Skip combining diacritics after this Greek letter.
            போது (nextIndex < srcLength) {
                uint32_t diacriticData = getDiacriticData(src[nextIndex]);
                எனில் (diacriticData != 0) {
                    data |= diacriticData;
                    எனில் ((diacriticData & HAS_YPOGEGRAMMENI) != 0) {
                        ++இலக்கம்_Ypogegrammeni;
                    }
                    ++nextIndex;
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
                change = src[i] != upper || இலக்கம்_Ypogegrammeni > 0;
                int32_t i2 = i + 1;
                எனில் ((data & HAS_EITHER_DIALYTIKA) != 0) {
                    change |= i2 >= nextIndex || src[i2] != 0x308;
                    ++i2;
                }
                எனில் (addTonos) {
                    change |= i2 >= nextIndex || src[i2] != 0x301;
                    ++i2;
                }
                int32_t oldLength = nextIndex - i;
                int32_t newLength = (i2 - i) + இலக்கம்_Ypogegrammeni;
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
                destIndex = appendUChar(dest, destIndex, destCapacity, static_cast<char16_t>(upper));
                எனில் (destIndex >= 0 && (data & HAS_EITHER_DIALYTIKA) != 0) {
                    destIndex=appendUChar(dest, destIndex, destCapacity, 0x308);  // restore or add a dialytika
                }
                எனில் (destIndex >= 0 && addTonos) {
                    destIndex=appendUChar(dest, destIndex, destCapacity, 0x301);
                }
                போது (destIndex >= 0 && இலக்கம்_Ypogegrammeni > 0) {
                    destIndex=appendUChar(dest, destIndex, destCapacity, 0x399);
                    --இலக்கம்_Ypogegrammeni;
                }
                எனில்(destIndex<0) {
                    errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
                    திரும்பு 0;
                }
            }
        } இல்லையெனில் {
            மாறிலி char16_t *s;
            c=ucase_toFullUpper(c, nullptr, nullptr, &s, UCASE_LOC_GREEK);
            destIndex = appendResult(dest, destIndex, destCapacity, c, s,
                                     nextIndex - i, options, edits);
            எனில் (destIndex < 0) {
                errorCode = U_INDEX_OUTOFBOUNDS_ERROR;
                திரும்பு 0;
            }
        }
        i = nextIndex;
        state = nextState;
    }

    திரும்பு destIndex;
}

}  // namespace GreekUpper
U_NAMESPACE_END

/* functions available in the common library (for unistr_case.cpp) */

U_CFUNC int32_t U_CALLCONV
ustrcase_internalToLower(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_UNUSED
                         char16_t *dest, int32_t destCapacity,
                         மாறிலி char16_t *src, int32_t srcLength,
                         icu::Edits *edits,
                         UErrorCode &errorCode) {
    UCaseContext csc=UCASECONTEXT_INITIALIZER;
    csc.p=(வெறுமை *)src;
    csc.limit=srcLength;
    int32_t destIndex = toLower(
        caseLocale, options,
        dest, destCapacity,
        src, &csc, 0, srcLength,
        edits, errorCode);
    திரும்பு checkOverflowAndEditsError(destIndex, destCapacity, edits, errorCode);
}

U_CFUNC int32_t U_CALLCONV
ustrcase_internalToUpper(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_UNUSED
                         char16_t *dest, int32_t destCapacity,
                         மாறிலி char16_t *src, int32_t srcLength,
                         icu::Edits *edits,
                         UErrorCode &errorCode) {
    int32_t destIndex;
    எனில் (caseLocale == UCASE_LOC_GREEK) {
        destIndex = GreekUpper::toUpper(options, dest, destCapacity,
                                        src, srcLength, edits, errorCode);
    } இல்லையெனில் {
        UCaseContext csc=UCASECONTEXT_INITIALIZER;
        csc.p=(வெறுமை *)src;
        csc.limit=srcLength;
        destIndex = toUpper(
            caseLocale, options,
            dest, destCapacity,
            src, &csc, srcLength,
            edits, errorCode);
    }
    திரும்பு checkOverflowAndEditsError(destIndex, destCapacity, edits, errorCode);
}

U_CFUNC int32_t U_CALLCONV
ustrcase_internalFold(int32_t /* caseLocale */, uint32_t options, UCASEMAP_BREAK_ITERATOR_UNUSED
                      char16_t *dest, int32_t destCapacity,
                      மாறிலி char16_t *src, int32_t srcLength,
                      icu::Edits *edits,
                      UErrorCode &errorCode) {
    int32_t destIndex = toLower(
        -1, options,
        dest, destCapacity,
        src, nullptr, 0, srcLength,
        edits, errorCode);
    திரும்பு checkOverflowAndEditsError(destIndex, destCapacity, edits, errorCode);
}

U_CFUNC int32_t
ustrcase_map(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_PARAM
             char16_t *dest, int32_t destCapacity,
             மாறிலி char16_t *src, int32_t srcLength,
             UStringCaseMapper *stringCaseMapper,
             icu::Edits *edits,
             UErrorCode &errorCode) {
    int32_t destLength;

    /* check argument values */
    எனில்(U_FAILURE(errorCode)) {
        திரும்பு 0;
    }
    எனில்( destCapacity<0 ||
        (dest==nullptr && destCapacity>0) ||
        src==nullptr ||
        srcLength<-1
    ) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு 0;
    }

    /* get the string length */
    எனில்(srcLength==-1) {
        srcLength=u_strlen(src);
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
    destLength=stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                                dest, destCapacity, src, srcLength, edits, errorCode);
    திரும்பு u_terminateUChars(dest, destCapacity, destLength, &errorCode);
}

U_CFUNC int32_t
ustrcase_mapWithOverlap(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_PARAM
                        char16_t *dest, int32_t destCapacity,
                        மாறிலி char16_t *src, int32_t srcLength,
                        UStringCaseMapper *stringCaseMapper,
                        UErrorCode &errorCode) {
    char16_t buffer[300];
    char16_t *temp;

    int32_t destLength;

    /* check argument values */
    எனில்(U_FAILURE(errorCode)) {
        திரும்பு 0;
    }
    எனில்( destCapacity<0 ||
        (dest==nullptr && destCapacity>0) ||
        src==nullptr ||
        srcLength<-1
    ) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு 0;
    }

    /* get the string length */
    எனில்(srcLength==-1) {
        srcLength=u_strlen(src);
    }

    /* check for overlapping source and destination */
    எனில்( dest!=nullptr &&
        ((src>=dest && src<(dest+destCapacity)) ||
         (dest>=src && dest<(src+srcLength)))
    ) {
        /* overlap: provide a temporary destination buffer and later copy the result */
        எனில்(destCapacity<=UPRV_LENGTHOF(buffer)) {
            /* the stack buffer is large enough */
            temp=buffer;
        } இல்லையெனில் {
            /* allocate a buffer */
            temp=(char16_t *)uprv_malloc(destCapacity*U_SIZEOF_UCHAR);
            எனில்(temp==nullptr) {
                errorCode=U_MEMORY_ALLOCATION_ERROR;
                திரும்பு 0;
            }
        }
    } இல்லையெனில் {
        temp=dest;
    }

    destLength=stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                                temp, destCapacity, src, srcLength, nullptr, errorCode);
    எனில்(temp!=dest) {
        /* copy the result string to the destination buffer */
        எனில் (U_SUCCESS(errorCode) && 0 < destLength && destLength <= destCapacity) {
            u_memmove(dest, temp, destLength);
        }
        எனில்(temp!=buffer) {
            uprv_free(temp);
        }
    }

    திரும்பு u_terminateUChars(dest, destCapacity, destLength, &errorCode);
}

/* public API functions */

U_CAPI int32_t U_EXPORT2
u_strFoldCase(char16_t *dest, int32_t destCapacity,
              மாறிலி char16_t *src, int32_t srcLength,
              uint32_t options,
              UErrorCode *pErrorCode) {
    திரும்பு ustrcase_mapWithOverlap(
        UCASE_LOC_வேர், options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, destCapacity,
        src, srcLength,
        ustrcase_internalFold, *pErrorCode);
}

U_NAMESPACE_BEGIN

int32_t CaseMap::fold(
        uint32_t options,
        மாறிலி char16_t *src, int32_t srcLength,
        char16_t *dest, int32_t destCapacity, Edits *edits,
        UErrorCode &errorCode) {
    திரும்பு ustrcase_map(
        UCASE_LOC_வேர், options, UCASEMAP_BREAK_ITERATOR_NULL
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
கட்டமைப்பு CmpEquivLevel {
    மாறிலி char16_t *start, *s, *limit;
};
வகைவரை கட்டமைப்பு CmpEquivLevel CmpEquivLevel;

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
நிலையான int32_t _cmpFold(
            மாறிலி char16_t *s1, int32_t length1,
            மாறிலி char16_t *s2, int32_t length2,
            uint32_t options,
            int32_t *matchLen1, int32_t *matchLen2,
            UErrorCode *pErrorCode) {
    int32_t cmpRes = 0;

    /* current-level start/limit - s1/s2 as current */
    மாறிலி char16_t *start1, *start2, *limit1, *limit2;

    /* points to the original start address */
    மாறிலி char16_t *org1, *org2;

    /* points to the end of match + 1 */
    மாறிலி char16_t *m1, *m2;

    /* case folding variables */
    மாறிலி char16_t *p;
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
    எனில்(U_FAILURE(*pErrorCode)) {
        திரும்பு 0;
    }

    /* initialize */
    எனில்(matchLen1) {
        U_ASSERT(matchLen2 !=nullptr);
        *matchLen1=0;
        *matchLen2=0;
    }

    start1=m1=org1=s1;
    எனில்(length1==-1) {
        limit1=nullptr;
    } இல்லையெனில் {
        limit1=s1+length1;
    }

    start2=m2=org2=s2;
    எனில்(length2==-1) {
        limit2=nullptr;
    } இல்லையெனில் {
        limit2=s2+length2;
    }

    level1=level2=0;
    c1=c2=-1;

    /* comparison loop */
    க்கு(;;) {
        /*
         * here a code unit value of -1 means "get another code unit"
         * below it will mean "this source is finished"
         */

        எனில்(c1<0) {
            /* get next code unit from string 1, post-increment */
            க்கு(;;) {
                எனில்(s1==limit1 || ((c1=*s1)==0 && (limit1==nullptr || (options&_STRNCMP_STYLE)))) {
                    எனில்(level1==0) {
                        c1=-1;
                        நிறுத்து;
                    }
                } இல்லையெனில் {
                    ++s1;
                    நிறுத்து;
                }

                /* reached end of level buffer, pop one level */
                செய் {
                    --level1;
                    start1=stack1[level1].start;    /*Not uninitialized*/
                } போது(start1==nullptr);
                s1=stack1[level1].s;                /*Not uninitialized*/
                limit1=stack1[level1].limit;        /*Not uninitialized*/
            }
        }

        எனில்(c2<0) {
            /* get next code unit from string 2, post-increment */
            க்கு(;;) {
                எனில்(s2==limit2 || ((c2=*s2)==0 && (limit2==nullptr || (options&_STRNCMP_STYLE)))) {
                    எனில்(level2==0) {
                        c2=-1;
                        நிறுத்து;
                    }
                } இல்லையெனில் {
                    ++s2;
                    நிறுத்து;
                }

                /* reached end of level buffer, pop one level */
                செய் {
                    --level2;
                    start2=stack2[level2].start;    /*Not uninitialized*/
                } போது(start2==nullptr);
                s2=stack2[level2].s;                /*Not uninitialized*/
                limit2=stack2[level2].limit;        /*Not uninitialized*/
            }
        }

        /*
         * compare c1 and c2
         * either variable c1, c2 is -1 only if the corresponding string is finished
         */
        எனில்(c1==c2) {
            மாறிலி char16_t *next1, *next2;

            எனில்(c1<0) {
                cmpRes=0;   /* c1==c2==-1 indicating end of strings */
                நிறுத்து;
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
            எனில்(level1==0) {
                next1=s1;
            } இல்லையெனில் எனில்(s1==limit1) {
                /* Note: This implementation only use a single level of stack.
                 *      If this code needs to be changed to use multiple levels
                 *      of stacks, the code above should check if the current
                 *      code is at the end of all stacks.
                 */
                U_ASSERT(level1==1);

                /* is s1 at the end of the current stack? */
                next1=stack1[0].s;
            }

            எனில் (next1!=nullptr) {
                எனில்(level2==0) {
                    next2=s2;
                } இல்லையெனில் எனில்(s2==limit2) {
                    U_ASSERT(level2==1);

                    /* is s2 at the end of the current stack? */
                    next2=stack2[0].s;
                }
                எனில்(next2!=nullptr) {
                    m1=next1;
                    m2=next2;
                }
            }
            c1=c2=-1;       /* make us fetch new code units */
            தொடர்;
        } இல்லையெனில் எனில்(c1<0) {
            cmpRes=-1;      /* string 1 ends before string 2 */
            நிறுத்து;
        } இல்லையெனில் எனில்(c2<0) {
            cmpRes=1;       /* string 2 ends before string 1 */
            நிறுத்து;
        }
        /* c1!=c2 && c1>=0 && c2>=0 */

        /* get complete code points for c1, c2 for lookups if either is a surrogate */
        cp1=c1;
        எனில்(U_IS_SURROGATE(c1)) {
            char16_t c;

            எனில்(U_IS_SURROGATE_LEAD(c1)) {
                எனில்(s1!=limit1 && U16_IS_TRAIL(c=*s1)) {
                    /* advance ++s1; only below if cp1 decomposes/case-folds */
                    cp1=U16_GET_SUPPLEMENTARY(c1, c);
                }
            } இல்லையெனில் /* isTrail(c1) */ {
                எனில்(start1<=(s1-2) && U16_IS_LEAD(c=*(s1-2))) {
                    cp1=U16_GET_SUPPLEMENTARY(c, c1);
                }
            }
        }

        cp2=c2;
        எனில்(U_IS_SURROGATE(c2)) {
            char16_t c;

            எனில்(U_IS_SURROGATE_LEAD(c2)) {
                எனில்(s2!=limit2 && U16_IS_TRAIL(c=*s2)) {
                    /* advance ++s2; only below if cp2 decomposes/case-folds */
                    cp2=U16_GET_SUPPLEMENTARY(c2, c);
                }
            } இல்லையெனில் /* isTrail(c2) */ {
                எனில்(start2<=(s2-2) && U16_IS_LEAD(c=*(s2-2))) {
                    cp2=U16_GET_SUPPLEMENTARY(c, c2);
                }
            }
        }

        /*
         * go down one level for each string
         * continue with the main loop as soon as there is a real change
         */

        எனில்( level1==0 &&
            (length = ucase_toFullFolding(cp1, &p, options)) >= 0
        ) {
            /* cp1 case-folds to the code point "length" or to p[length] */
            எனில்(U_IS_SURROGATE(c1)) {
                எனில்(U_IS_SURROGATE_LEAD(c1)) {
                    /* advance beyond source surrogate pair if it case-folds */
                    ++s1;
                } இல்லையெனில் /* isTrail(c1) */ {
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
            எனில்(length<=UCASE_MAX_STRING_LENGTH) {
                u_memcpy(fold1, p, length);
            } இல்லையெனில் {
                int32_t i=0;
                U16_APPEND_UNSAFE(fold1, i, length);
                length=i;
            }

            /* set next level pointers to case folding */
            start1=s1=fold1;
            limit1=fold1+length;

            /* get ready to read from decomposition, continue with loop */
            c1=-1;
            தொடர்;
        }

        எனில்( level2==0 &&
            (length = ucase_toFullFolding(cp2, &p, options)) >= 0
        ) {
            /* cp2 case-folds to the code point "length" or to p[length] */
            எனில்(U_IS_SURROGATE(c2)) {
                எனில்(U_IS_SURROGATE_LEAD(c2)) {
                    /* advance beyond source surrogate pair if it case-folds */
                    ++s2;
                } இல்லையெனில் /* isTrail(c2) */ {
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
            எனில்(length<=UCASE_MAX_STRING_LENGTH) {
                u_memcpy(fold2, p, length);
            } இல்லையெனில் {
                int32_t i=0;
                U16_APPEND_UNSAFE(fold2, i, length);
                length=i;
            }

            /* set next level pointers to case folding */
            start2=s2=fold2;
            limit2=fold2+length;

            /* get ready to read from decomposition, continue with loop */
            c2=-1;
            தொடர்;
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

        எனில்(c1>=0xd800 && c2>=0xd800 && (options&U_COMPARE_CODE_POINT_ORDER)) {
            /* subtract 0x2800 from BMP code points to make them smaller than supplementary ones */
            எனில்(
                (c1<=0xdbff && s1!=limit1 && U16_IS_TRAIL(*s1)) ||
                (U16_IS_TRAIL(c1) && start1!=(s1-1) && U16_IS_LEAD(*(s1-2)))
            ) {
                /* part of a surrogate pair, leave >=d800 */
            } இல்லையெனில் {
                /* BMP code point - may be surrogate code point - make <d800 */
                c1-=0x2800;
            }

            எனில்(
                (c2<=0xdbff && s2!=limit2 && U16_IS_TRAIL(*s2)) ||
                (U16_IS_TRAIL(c2) && start2!=(s2-1) && U16_IS_LEAD(*(s2-2)))
            ) {
                /* part of a surrogate pair, leave >=d800 */
            } இல்லையெனில் {
                /* BMP code point - may be surrogate code point - make <d800 */
                c2-=0x2800;
            }
        }

        cmpRes=c1-c2;
        நிறுத்து;
    }

    எனில்(matchLen1) {
        *matchLen1=static_cast<int32_t>(m1-org1);
        *matchLen2=static_cast<int32_t>(m2-org2);
    }
    திரும்பு cmpRes;
}

/* internal function */
U_CFUNC int32_t
u_strcmpFold(மாறிலி char16_t *s1, int32_t length1,
             மாறிலி char16_t *s2, int32_t length2,
             uint32_t options,
             UErrorCode *pErrorCode) {
    திரும்பு _cmpFold(s1, length1, s2, length2, options, nullptr, nullptr, pErrorCode);
}

/* public API functions */

U_CAPI int32_t U_EXPORT2
u_strCaseCompare(மாறிலி char16_t *s1, int32_t length1,
                 மாறிலி char16_t *s2, int32_t length2,
                 uint32_t options,
                 UErrorCode *pErrorCode) {
    /* argument checking */
    எனில் (pErrorCode == nullptr || U_FAILURE(*pErrorCode)) {
        திரும்பு 0;
    }
    எனில்(s1==nullptr || length1<-1 || s2==nullptr || length2<-1) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு 0;
    }
    திரும்பு u_strcmpFold(s1, length1, s2, length2,
                        options|U_COMPARE_IGNORE_CASE,
                        pErrorCode);
}

U_CAPI int32_t U_EXPORT2
u_strcasecmp(மாறிலி char16_t *s1, மாறிலி char16_t *s2, uint32_t options) {
    UErrorCode errorCode=U_ZERO_ERROR;
    திரும்பு u_strcmpFold(s1, -1, s2, -1,
                        options|U_COMPARE_IGNORE_CASE,
                        &errorCode);
}

U_CAPI int32_t U_EXPORT2
u_memcasecmp(மாறிலி char16_t *s1, மாறிலி char16_t *s2, int32_t length, uint32_t options) {
    UErrorCode errorCode=U_ZERO_ERROR;
    திரும்பு u_strcmpFold(s1, length, s2, length,
                        options|U_COMPARE_IGNORE_CASE,
                        &errorCode);
}

U_CAPI int32_t U_EXPORT2
u_strncasecmp(மாறிலி char16_t *s1, மாறிலி char16_t *s2, int32_t n, uint32_t options) {
    UErrorCode errorCode=U_ZERO_ERROR;
    திரும்பு u_strcmpFold(s1, n, s2, n,
                        options|(U_COMPARE_IGNORE_CASE|_STRNCMP_STYLE),
                        &errorCode);
}

/* internal API - detect length of shared prefix */
U_CAPI வெறுமை
u_caseInsensitivePrefixMatch(மாறிலி char16_t *s1, int32_t length1,
                             மாறிலி char16_t *s2, int32_t length2,
                             uint32_t options,
                             int32_t *matchLen1, int32_t *matchLen2,
                             UErrorCode *pErrorCode) {
    _cmpFold(s1, length1, s2, length2, options,
        matchLen1, matchLen2, pErrorCode);
}
