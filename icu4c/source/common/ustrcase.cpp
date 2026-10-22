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

#समावेश "unicode/utypes.h"
#समावेश "unicode/brkiter.h"
#समावेश "unicode/casemap.h"
#समावेश "unicode/edits.h"
#समावेश "unicode/stringoptions.h"
#समावेश "unicode/ustring.h"
#समावेश "unicode/ucasemap.h"
#समावेश "unicode/ubrk.h"
#समावेश "unicode/utf.h"
#समावेश "unicode/utf16.h"
#समावेश "cmemory.h"
#समावेश "ucase.h"
#समावेश "ucasemap_imp.h"
#समावेश "ustr_imp.h"
#समावेश "uassert.h"

/**
 * Code point for COMBINING ACUTE ACCENT
 * @internal
 */
#घोषणा ACUTE u'\u0301'

U_NAMESPACE_BEGIN

namespace {

int32_t checkOverflowAndEditsError(int32_t dest_सूचकांक, int32_t destCapacity,
                                   Edits *edits, UErrorCode &errorCode) {
    अगर (U_SUCCESS(errorCode)) {
        अगर (dest_सूचकांक > destCapacity) {
            errorCode = U_बफर_OVERFLOW_ERROR;
        } अन्यथा अगर (edits != nullptr) {
            edits->copyErrorTo(errorCode);
        }
    }
    वापस dest_सूचकांक;
}

/* Appends a full case mapping result, see UCASE_MAX_STRING_LENGTH. */
अंतरभूत int32_t
append_परिणाम(char16_t *dest, int32_t dest_सूचकांक, int32_t destCapacity,
             int32_t परिणाम, स्थिर char16_t *s,
             int32_t cp_लंबाई, uint32_t options, icu::Edits *edits) {
    UChar32 c;
    int32_t length;

    /* decode the result */
    अगर(परिणाम<0) {
        /* (not) original code point */
        अगर(edits!=nullptr) {
            edits->addUnchanged(cp_लंबाई);
        }
        अगर(options & U_OMIT_UNCHANGED_TEXT) {
            वापस dest_सूचकांक;
        }
        c=~परिणाम;
        अगर(dest_सूचकांक<destCapacity && c<=0xffff) {  // BMP slightly-fastpath
            dest[dest_सूचकांक++] = static_cast<char16_t>(c);
            वापस dest_सूचकांक;
        }
        length=cp_लंबाई;
    } अन्यथा {
        अगर(परिणाम<=UCASE_MAX_STRING_लंबाई) {
            c=U_SENTINEL;
            length=परिणाम;
        } अन्यथा अगर(dest_सूचकांक<destCapacity && परिणाम<=0xffff) {  // BMP slightly-fastpath
            dest[dest_सूचकांक++] = static_cast<char16_t>(परिणाम);
            अगर(edits!=nullptr) {
                edits->addReplace(cp_लंबाई, 1);
            }
            वापस dest_सूचकांक;
        } अन्यथा {
            c=परिणाम;
            length=U16_लंबाई(c);
        }
        अगर(edits!=nullptr) {
            edits->addReplace(cp_लंबाई, length);
        }
    }
    अगर(length>(INT32_MAX-dest_सूचकांक)) {
        वापस -1;  // integer overflow
    }

    अगर(dest_सूचकांक<destCapacity) {
        /* append the result */
        अगर(c>=0) {
            /* code point */
            UBool isError=false;
            U16_APPEND(dest, dest_सूचकांक, destCapacity, c, isError);
            अगर(isError) {
                /* overflow, nothing written */
                dest_सूचकांक+=length;
            }
        } अन्यथा {
            /* string */
            अगर((dest_सूचकांक+length)<=destCapacity) {
                जबतक(length>0) {
                    dest[dest_सूचकांक++]=*s++;
                    --length;
                }
            } अन्यथा {
                /* overflow */
                dest_सूचकांक+=length;
            }
        }
    } अन्यथा {
        /* preflight */
        dest_सूचकांक+=length;
    }
    वापस dest_सूचकांक;
}

अंतरभूत int32_t
appendUChar(char16_t *dest, int32_t dest_सूचकांक, int32_t destCapacity, char16_t c) {
    अगर(dest_सूचकांक<destCapacity) {
        dest[dest_सूचकांक]=c;
    } अन्यथा अगर(dest_सूचकांक==INT32_MAX) {
        वापस -1;  // integer overflow
    }
    वापस dest_सूचकांक+1;
}

int32_t
appendNonEmptyUnchanged(char16_t *dest, int32_t dest_सूचकांक, int32_t destCapacity,
                        स्थिर char16_t *s, int32_t length, uint32_t options, icu::Edits *edits) {
    अगर(edits!=nullptr) {
        edits->addUnchanged(length);
    }
    अगर(options & U_OMIT_UNCHANGED_TEXT) {
        वापस dest_सूचकांक;
    }
    अगर(length>(INT32_MAX-dest_सूचकांक)) {
        वापस -1;  // integer overflow
    }
    अगर((dest_सूचकांक+length)<=destCapacity) {
        u_memcpy(dest+dest_सूचकांक, s, length);
    }
    वापस dest_सूचकांक + length;
}

अंतरभूत int32_t
appendUnchanged(char16_t *dest, int32_t dest_सूचकांक, int32_t destCapacity,
                स्थिर char16_t *s, int32_t length, uint32_t options, icu::Edits *edits) {
    अगर (length <= 0) {
        वापस dest_सूचकांक;
    }
    वापस appendNonEmptyUnchanged(dest, dest_सूचकांक, destCapacity, s, length, options, edits);
}

UChar32 U_CALLCONV
utf16_caseContextIterator(व्योम *context, int8_t dir) {
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
            U16_पूर्व((स्थिर char16_t *)csc->p, csc->start, csc->सूचकांक, c);
            वापस c;
        }
    } अन्यथा {
        अगर(csc->सूचकांक<csc->limit) {
            U16_अग्र((स्थिर char16_t *)csc->p, csc->सूचकांक, csc->limit, c);
            वापस c;
        }
    }
    वापस U_SENTINEL;
}

/**
 * caseLocale >= 0: Lowercases [srcStart..srcLimit[ but takes context [0..srcLength[ into account.
 * caseLocale < 0: Case-folds [srcStart..srcLimit[.
 */
int32_t toLower(int32_t caseLocale, uint32_t options,
                char16_t *dest, int32_t destCapacity,
                स्थिर char16_t *src, UCaseContext *csc, int32_t srcStart, int32_t srcLimit,
                icu::Edits *edits, UErrorCode &errorCode) {
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
    int32_t dest_सूचकांक = 0;
    int32_t पूर्व = srcStart;
    int32_t src_सूचकांक = srcStart;
    क्रम (;;) {
        // fast path for simple cases
        char16_t lead = 0;
        जबतक (src_सूचकांक < srcLimit) {
            lead = src[src_सूचकांक];
            int32_t delta;
            अगर (lead < LatinCase::LONG_S) {
                int8_t d = latinToLower[lead];
                अगर (d == LatinCase::EXC) { अवरोध; }
                ++src_सूचकांक;
                अगर (d == 0) { जारी; }
                delta = d;
            } अन्यथा अगर (lead >= 0xd800) {
                अवरोध;  // surrogate or higher
            } अन्यथा {
                uint16_t props = UTRIE2_GET16_FROM_U16_SINGLE_LEAD(trie, lead);
                अगर (UCASE_HAS_EXCEPTION(props)) { अवरोध; }
                ++src_सूचकांक;
                अगर (!UCASE_IS_UPPER_OR_TITLE(props) || (delta = UCASE_GET_DELTA(props)) == 0) {
                    जारी;
                }
            }
            lead += static_cast<char16_t>(delta);
            dest_सूचकांक = appendUnchanged(dest, dest_सूचकांक, destCapacity,
                                        src + पूर्व, src_सूचकांक - 1 - पूर्व, options, edits);
            अगर (dest_सूचकांक >= 0) {
                dest_सूचकांक = appendUChar(dest, dest_सूचकांक, destCapacity, lead);
                अगर (edits != nullptr) {
                    edits->addReplace(1, 1);
                }
            }
            अगर (dest_सूचकांक < 0) {
                errorCode = U_सूचकांक_OUTOFBOUNDS_ERROR;
                वापस 0;
            }
            पूर्व = src_सूचकांक;
        }
        अगर (src_सूचकांक >= srcLimit) {
            अवरोध;
        }
        // slow path
        int32_t cpStart = src_सूचकांक++;
        char16_t trail;
        UChar32 c;
        अगर (U16_IS_LEAD(lead) && src_सूचकांक < srcLimit && U16_IS_TRAIL(trail = src[src_सूचकांक])) {
            c = U16_GET_SUPPLEMENTARY(lead, trail);
            ++src_सूचकांक;
        } अन्यथा {
            c = lead;
        }
        स्थिर char16_t *s = nullptr;
        अगर (caseLocale >= 0) {
            csc->cpStart = cpStart;
            csc->cpLimit = src_सूचकांक;
            c = ucase_toFullLower(c, utf16_caseContextIterator, csc, &s, caseLocale);
        } अन्यथा {
            c = ucase_toFullFolding(c, &s, options);
        }
        अगर (c >= 0) {
            dest_सूचकांक = appendUnchanged(dest, dest_सूचकांक, destCapacity,
                                        src + पूर्व, cpStart - पूर्व, options, edits);
            अगर (dest_सूचकांक >= 0) {
                dest_सूचकांक = append_परिणाम(dest, dest_सूचकांक, destCapacity, c, s,
                                         src_सूचकांक - cpStart, options, edits);
            }
            अगर (dest_सूचकांक < 0) {
                errorCode = U_सूचकांक_OUTOFBOUNDS_ERROR;
                वापस 0;
            }
            पूर्व = src_सूचकांक;
        }
    }
    dest_सूचकांक = appendUnchanged(dest, dest_सूचकांक, destCapacity,
                                src + पूर्व, src_सूचकांक - पूर्व, options, edits);
    अगर (dest_सूचकांक < 0) {
        errorCode = U_सूचकांक_OUTOFBOUNDS_ERROR;
        वापस 0;
    }
    वापस dest_सूचकांक;
}

int32_t toUpper(int32_t caseLocale, uint32_t options,
                char16_t *dest, int32_t destCapacity,
                स्थिर char16_t *src, UCaseContext *csc, int32_t src_लंबाई,
                icu::Edits *edits, UErrorCode &errorCode) {
    स्थिर int8_t *latinToUpper;
    अगर (caseLocale == UCASE_LOC_TURKISH) {
        latinToUpper = LatinCase::TO_UPPER_TR;
    } अन्यथा {
        latinToUpper = LatinCase::TO_UPPER_NORMAL;
    }
    स्थिर UTrie2 *trie = ucase_getTrie();
    int32_t dest_सूचकांक = 0;
    int32_t पूर्व = 0;
    int32_t src_सूचकांक = 0;
    क्रम (;;) {
        // fast path for simple cases
        char16_t lead = 0;
        जबतक (src_सूचकांक < src_लंबाई) {
            lead = src[src_सूचकांक];
            int32_t delta;
            अगर (lead < LatinCase::LONG_S) {
                int8_t d = latinToUpper[lead];
                अगर (d == LatinCase::EXC) { अवरोध; }
                ++src_सूचकांक;
                अगर (d == 0) { जारी; }
                delta = d;
            } अन्यथा अगर (lead >= 0xd800) {
                अवरोध;  // surrogate or higher
            } अन्यथा {
                uint16_t props = UTRIE2_GET16_FROM_U16_SINGLE_LEAD(trie, lead);
                अगर (UCASE_HAS_EXCEPTION(props)) { अवरोध; }
                ++src_सूचकांक;
                अगर (UCASE_GET_TYPE(props) != UCASE_LOWER || (delta = UCASE_GET_DELTA(props)) == 0) {
                    जारी;
                }
            }
            lead += static_cast<char16_t>(delta);
            dest_सूचकांक = appendUnchanged(dest, dest_सूचकांक, destCapacity,
                                        src + पूर्व, src_सूचकांक - 1 - पूर्व, options, edits);
            अगर (dest_सूचकांक >= 0) {
                dest_सूचकांक = appendUChar(dest, dest_सूचकांक, destCapacity, lead);
                अगर (edits != nullptr) {
                    edits->addReplace(1, 1);
                }
            }
            अगर (dest_सूचकांक < 0) {
                errorCode = U_सूचकांक_OUTOFBOUNDS_ERROR;
                वापस 0;
            }
            पूर्व = src_सूचकांक;
        }
        अगर (src_सूचकांक >= src_लंबाई) {
            अवरोध;
        }
        // slow path
        int32_t cpStart;
        csc->cpStart = cpStart = src_सूचकांक++;
        char16_t trail;
        UChar32 c;
        अगर (U16_IS_LEAD(lead) && src_सूचकांक < src_लंबाई && U16_IS_TRAIL(trail = src[src_सूचकांक])) {
            c = U16_GET_SUPPLEMENTARY(lead, trail);
            ++src_सूचकांक;
        } अन्यथा {
            c = lead;
        }
        csc->cpLimit = src_सूचकांक;
        स्थिर char16_t *s = nullptr;
        c = ucase_toFullUpper(c, utf16_caseContextIterator, csc, &s, caseLocale);
        अगर (c >= 0) {
            dest_सूचकांक = appendUnchanged(dest, dest_सूचकांक, destCapacity,
                                        src + पूर्व, cpStart - पूर्व, options, edits);
            अगर (dest_सूचकांक >= 0) {
                dest_सूचकांक = append_परिणाम(dest, dest_सूचकांक, destCapacity, c, s,
                                         src_सूचकांक - cpStart, options, edits);
            }
            अगर (dest_सूचकांक < 0) {
                errorCode = U_सूचकांक_OUTOFBOUNDS_ERROR;
                वापस 0;
            }
            पूर्व = src_सूचकांक;
        }
    }
    dest_सूचकांक = appendUnchanged(dest, dest_सूचकांक, destCapacity,
                                src + पूर्व, src_सूचकांक - पूर्व, options, edits);
    अगर (dest_सूचकांक < 0) {
        errorCode = U_सूचकांक_OUTOFBOUNDS_ERROR;
        वापस 0;
    }
    वापस dest_सूचकांक;
}

}  // namespace

U_NAMESPACE_END

U_NAMESPACE_USE

#अगर !UCONFIG_NO_BREAK_ITERATION

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
int32_t maybeTitleDutchIJ(स्थिर char16_t *src, UChar32 c, int32_t start, int32_t segmentLimit,
                          char16_t *dest, int32_t &dest_सूचकांक, int32_t destCapacity, uint32_t options,
                          icu::Edits *edits) {
    U_ASSERT(start < segmentLimit);

    int32_t सूचकांक = start;
    bool withAcute = false;

    // If the conditions are met, then the following variables tell us what to output.
    int32_t unchanged1 = 0;  // code units before the j, or the whole sequence (0..3)
    bool doTitleJ = false;  // true if the j needs to be titlecased
    int32_t unchanged2 = 0;  // after the j (0 or 1)

    // next character after the first letter
    char16_t c2 = src[सूचकांक++];

    // Is the first letter an i/I with accent?
    अगर (c == u'I') {
        अगर (c2 == ACUTE) {
            withAcute = true;
            unchanged1 = 1;
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
        अगर (सूचकांक == segmentLimit || src[सूचकांक++] != ACUTE) { वापस start; }
        अगर (doTitleJ) {
            unchanged2 = 1;
        } अन्यथा {
            ++unchanged1;
        }
    }

    // There must not be another combining mark.
    अगर (सूचकांक < segmentLimit) {
        int32_t cp;
        int32_t i = सूचकांक;
        U16_अग्र(src, i, segmentLimit, cp);
        uint32_t typeMask = U_GET_GC_MASK(cp);
        अगर ((typeMask & U_GC_M_MASK) != 0) {
            वापस start;
        }
    }

    // Output the rest of the Dutch IJ.
    dest_सूचकांक = appendUnchanged(dest, dest_सूचकांक, destCapacity, src + start, unchanged1, options, edits);
    start += unchanged1;
    अगर (doTitleJ) {
        dest_सूचकांक = appendUChar(dest, dest_सूचकांक, destCapacity, u'J');
        अगर (edits != nullptr) {
            edits->addReplace(1, 1);
        }
        ++start;
    }
    dest_सूचकांक = appendUnchanged(dest, dest_सूचकांक, destCapacity, src + start, unchanged2, options, edits);

    U_ASSERT(start + unchanged2 == सूचकांक);
    वापस सूचकांक;
}

}  // namespace

U_CFUNC int32_t U_CALLCONV
ustrcase_internalToTitle(int32_t caseLocale, uint32_t options, BreakIterator *iter,
                         char16_t *dest, int32_t destCapacity,
                         स्थिर char16_t *src, int32_t src_लंबाई,
                         icu::Edits *edits,
                         UErrorCode &errorCode) {
    अगर (!ustrcase_checkTitleAdjustmentOptions(options, errorCode)) {
        वापस 0;
    }

    /* set up local variables */
    UCaseContext csc=UCASECONTEXT_INITIALIZER;
    csc.p=(व्योम *)src;
    csc.limit=src_लंबाई;
    int32_t dest_सूचकांक=0;
    int32_t पूर्व=0;
    bool is_First_सूचकांक=true;

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
            // Find and copy skipped characters [prev..titleStart[
            int32_t titleStart=पूर्व;
            int32_t titleLimit=पूर्व;
            UChar32 c;
            U16_अग्र(src, titleLimit, सूचकांक, c);
            अगर ((options&U_TITLECASE_NO_BREAK_ADJUSTMENT)==0) {
                // Adjust the titlecasing index to the next cased character,
                // or to the next letter/number/symbol/private use.
                // Stop with titleStart<titleLimit<=index
                // if there is a character to be titlecased,
                // or else stop with titleStart==titleLimit==index.
                bool toCased = (options&U_TITLECASE_ADJUST_TO_CASED) != 0;
                जबतक (toCased ? UCASE_NONE==ucase_getType(c) : !ustrcase_isLNS(c)) {
                    titleStart=titleLimit;
                    अगर(titleLimit==सूचकांक) {
                        अवरोध;
                    }
                    U16_अग्र(src, titleLimit, सूचकांक, c);
                }
                अगर (पूर्व < titleStart) {
                    dest_सूचकांक=appendUnchanged(dest, dest_सूचकांक, destCapacity,
                                              src+पूर्व, titleStart-पूर्व, options, edits);
                    अगर(dest_सूचकांक<0) {
                        errorCode=U_सूचकांक_OUTOFBOUNDS_ERROR;
                        वापस 0;
                    }
                }
            }

            अगर(titleStart<titleLimit) {
                /* titlecase c which is from [titleStart..titleLimit[ */
                csc.cpStart=titleStart;
                csc.cpLimit=titleLimit;
                स्थिर char16_t *s;
                c=ucase_toFullTitle(c, utf16_caseContextIterator, &csc, &s, caseLocale);
                dest_सूचकांक=append_परिणाम(dest, dest_सूचकांक, destCapacity, c, s,
                                       titleLimit-titleStart, options, edits);
                अगर(dest_सूचकांक<0) {
                    errorCode=U_सूचकांक_OUTOFBOUNDS_ERROR;
                    वापस 0;
                }

                /* Special case Dutch IJ titlecasing */
                अगर (titleStart+1 < सूचकांक &&
                        caseLocale == UCASE_LOC_DUTCH) {
                    अगर (c < 0) {
                        c = ~c;
                    }

                    अगर (c == u'I' || c == u'Í') {
                        titleLimit = maybeTitleDutchIJ(src, c, titleStart + 1, सूचकांक,
                                                       dest, dest_सूचकांक, destCapacity, options,
                                                       edits);
                    }
                }

                /* lowercase [titleLimit..index[ */
                अगर(titleLimit<सूचकांक) {
                    अगर((options&U_TITLECASE_NO_LOWERCASE)==0) {
                        /* Normal operation: Lowercase the rest of the word. */
                        dest_सूचकांक+=
                            toLower(
                                caseLocale, options,
                                (dest==nullptr) ? nullptr: dest+dest_सूचकांक, destCapacity-dest_सूचकांक,
                                src, &csc, titleLimit, सूचकांक,
                                edits, errorCode);
                        अगर(errorCode==U_बफर_OVERFLOW_ERROR) {
                            errorCode=U_ZERO_ERROR;
                        }
                        अगर(U_FAILURE(errorCode)) {
                            वापस dest_सूचकांक;
                        }
                    } अन्यथा {
                        /* Optionally just copy the rest of the word unchanged. */
                        dest_सूचकांक=appendUnchanged(dest, dest_सूचकांक, destCapacity,
                                                  src+titleLimit, सूचकांक-titleLimit, options, edits);
                        अगर(dest_सूचकांक<0) {
                            errorCode=U_सूचकांक_OUTOFBOUNDS_ERROR;
                            वापस 0;
                        }
                    }
                }
            }
        }

        पूर्व=सूचकांक;
    }

    वापस checkOverflowAndEditsError(dest_सूचकांक, destCapacity, edits, errorCode);
}

#पूर्ण_अगर  // !UCONFIG_NO_BREAK_ITERATION

U_NAMESPACE_BEGIN
namespace GreekUpper {

// Data generated by prototype code, see
// https://icu.unicode.org/design/case/greek-upper
// TODO: Move this data into ucase.icu.
अटल स्थिर uint16_t data0370[] = {
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

अटल स्थिर uint16_t data1F00[] = {
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
अटल स्थिर uint16_t data2126 = 0x03A9 | HAS_VOWEL;

uint32_t get_Letter_तथ्य(UChar32 c) {
    अगर (c < 0x370 || 0x2126 < c || (0x3ff < c && c < 0x1f00)) {
        वापस 0;
    } अन्यथा अगर (c <= 0x3ff) {
        वापस data0370[c - 0x370];
    } अन्यथा अगर (c <= 0x1fff) {
        वापस data1F00[c - 0x1f00];
    } अन्यथा अगर (c == 0x2126) {
        वापस data2126;
    } अन्यथा {
        वापस 0;
    }
}

uint32_t get_Diacritic_तथ्य(UChar32 c) {
    चयन (c) {
    हाल 0x0300:  // varia
    हाल 0x0301:  // tonos = oxia
    हाल 0x0342:  // perispomeni
    हाल 0x0302:  // circumflex can look like perispomeni
    हाल 0x0303:  // tilde can look like perispomeni
    हाल 0x0311:  // inverted breve can look like perispomeni
        वापस HAS_ACCENT;
    हाल 0x0308:  // dialytika = diaeresis
        वापस HAS_COMBINING_DIALYTIKA;
    हाल 0x0344:  // dialytika tonos
        वापस HAS_COMBINING_DIALYTIKA | HAS_ACCENT;
    हाल 0x0345:  // ypogegrammeni = iota subscript
        वापस HAS_YPOGEGRAMMENI;
    हाल 0x0304:  // macron
    हाल 0x0306:  // breve
    हाल 0x0313:  // comma above
    हाल 0x0314:  // reversed comma above
    हाल 0x0343:  // koronis
        वापस HAS_OTHER_GREEK_DIACRITIC;
    शेष:
        वापस 0;
    }
}

UBool isFollowedByCasedLetter(स्थिर char16_t *s, int32_t i, int32_t length) {
    जबतक (i < length) {
        UChar32 c;
        U16_अग्र(s, i, length, c);
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

/**
 * Greek string uppercasing with a state machine.
 * Probably simpler than a stateless function that has to figure out complex context-before
 * for each character.
 * TODO: Try to re-consolidate one way or another with the non-Greek function.
 */
int32_t toUpper(uint32_t options,
                char16_t *dest, int32_t destCapacity,
                स्थिर char16_t *src, int32_t src_लंबाई,
                Edits *edits,
                UErrorCode &errorCode) {
    int32_t dest_सूचकांक=0;
    uint32_t state = 0;
    क्रम (int32_t i = 0; i < src_लंबाई;) {
        int32_t अग्र_सूचकांक = i;
        UChar32 c;
        U16_अग्र(src, अग्र_सूचकांक, src_लंबाई, c);
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
                तथ्य |= (state & AFTER_VOWEL_WITH_PRECOMPOSED_ACCENT) ? HAS_DIALYTIKA
                                                                      : HAS_COMBINING_DIALYTIKA;
            }
            int32_t अंक_Ypogegrammeni = 0;  // Map each one to a trailing, spacing, capital iota.
            अगर ((तथ्य & HAS_YPOGEGRAMMENI) != 0) {
                अंक_Ypogegrammeni = 1;
            }
            स्थिर UBool hasPrecomposedAccent = (तथ्य & HAS_ACCENT) != 0;
            // Skip combining diacritics after this Greek letter.
            जबतक (अग्र_सूचकांक < src_लंबाई) {
                uint32_t diacritic_तथ्य = get_Diacritic_तथ्य(src[अग्र_सूचकांक]);
                अगर (diacritic_तथ्य != 0) {
                    तथ्य |= diacritic_तथ्य;
                    अगर ((diacritic_तथ्य & HAS_YPOGEGRAMMENI) != 0) {
                        ++अंक_Ypogegrammeni;
                    }
                    ++अग्र_सूचकांक;
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
                change = src[i] != upper || अंक_Ypogegrammeni > 0;
                int32_t i2 = i + 1;
                अगर ((तथ्य & HAS_EITHER_DIALYTIKA) != 0) {
                    change |= i2 >= अग्र_सूचकांक || src[i2] != 0x308;
                    ++i2;
                }
                अगर (addTonos) {
                    change |= i2 >= अग्र_सूचकांक || src[i2] != 0x301;
                    ++i2;
                }
                int32_t old_लंबाई = अग्र_सूचकांक - i;
                int32_t new_लंबाई = (i2 - i) + अंक_Ypogegrammeni;
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
                dest_सूचकांक = appendUChar(dest, dest_सूचकांक, destCapacity, static_cast<char16_t>(upper));
                अगर (dest_सूचकांक >= 0 && (तथ्य & HAS_EITHER_DIALYTIKA) != 0) {
                    dest_सूचकांक=appendUChar(dest, dest_सूचकांक, destCapacity, 0x308);  // restore or add a dialytika
                }
                अगर (dest_सूचकांक >= 0 && addTonos) {
                    dest_सूचकांक=appendUChar(dest, dest_सूचकांक, destCapacity, 0x301);
                }
                जबतक (dest_सूचकांक >= 0 && अंक_Ypogegrammeni > 0) {
                    dest_सूचकांक=appendUChar(dest, dest_सूचकांक, destCapacity, 0x399);
                    --अंक_Ypogegrammeni;
                }
                अगर(dest_सूचकांक<0) {
                    errorCode=U_सूचकांक_OUTOFBOUNDS_ERROR;
                    वापस 0;
                }
            }
        } अन्यथा {
            स्थिर char16_t *s;
            c=ucase_toFullUpper(c, nullptr, nullptr, &s, UCASE_LOC_GREEK);
            dest_सूचकांक = append_परिणाम(dest, dest_सूचकांक, destCapacity, c, s,
                                     अग्र_सूचकांक - i, options, edits);
            अगर (dest_सूचकांक < 0) {
                errorCode = U_सूचकांक_OUTOFBOUNDS_ERROR;
                वापस 0;
            }
        }
        i = अग्र_सूचकांक;
        state = अग्र_State;
    }

    वापस dest_सूचकांक;
}

}  // namespace GreekUpper
U_NAMESPACE_END

/* functions available in the common library (for unistr_case.cpp) */

U_CFUNC int32_t U_CALLCONV
ustrcase_internalToLower(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_UNUSED
                         char16_t *dest, int32_t destCapacity,
                         स्थिर char16_t *src, int32_t src_लंबाई,
                         icu::Edits *edits,
                         UErrorCode &errorCode) {
    UCaseContext csc=UCASECONTEXT_INITIALIZER;
    csc.p=(व्योम *)src;
    csc.limit=src_लंबाई;
    int32_t dest_सूचकांक = toLower(
        caseLocale, options,
        dest, destCapacity,
        src, &csc, 0, src_लंबाई,
        edits, errorCode);
    वापस checkOverflowAndEditsError(dest_सूचकांक, destCapacity, edits, errorCode);
}

U_CFUNC int32_t U_CALLCONV
ustrcase_internalToUpper(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_UNUSED
                         char16_t *dest, int32_t destCapacity,
                         स्थिर char16_t *src, int32_t src_लंबाई,
                         icu::Edits *edits,
                         UErrorCode &errorCode) {
    int32_t dest_सूचकांक;
    अगर (caseLocale == UCASE_LOC_GREEK) {
        dest_सूचकांक = GreekUpper::toUpper(options, dest, destCapacity,
                                        src, src_लंबाई, edits, errorCode);
    } अन्यथा {
        UCaseContext csc=UCASECONTEXT_INITIALIZER;
        csc.p=(व्योम *)src;
        csc.limit=src_लंबाई;
        dest_सूचकांक = toUpper(
            caseLocale, options,
            dest, destCapacity,
            src, &csc, src_लंबाई,
            edits, errorCode);
    }
    वापस checkOverflowAndEditsError(dest_सूचकांक, destCapacity, edits, errorCode);
}

U_CFUNC int32_t U_CALLCONV
ustrcase_internalFold(int32_t /* caseLocale */, uint32_t options, UCASEMAP_BREAK_ITERATOR_UNUSED
                      char16_t *dest, int32_t destCapacity,
                      स्थिर char16_t *src, int32_t src_लंबाई,
                      icu::Edits *edits,
                      UErrorCode &errorCode) {
    int32_t dest_सूचकांक = toLower(
        -1, options,
        dest, destCapacity,
        src, nullptr, 0, src_लंबाई,
        edits, errorCode);
    वापस checkOverflowAndEditsError(dest_सूचकांक, destCapacity, edits, errorCode);
}

U_CFUNC int32_t
ustrcase_map(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_PARAM
             char16_t *dest, int32_t destCapacity,
             स्थिर char16_t *src, int32_t src_लंबाई,
             UStringCaseMapper *stringCaseMapper,
             icu::Edits *edits,
             UErrorCode &errorCode) {
    int32_t dest_लंबाई;

    /* check argument values */
    अगर(U_FAILURE(errorCode)) {
        वापस 0;
    }
    अगर( destCapacity<0 ||
        (dest==nullptr && destCapacity>0) ||
        src==nullptr ||
        src_लंबाई<-1
    ) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        वापस 0;
    }

    /* get the string length */
    अगर(src_लंबाई==-1) {
        src_लंबाई=u_strlen(src);
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
    dest_लंबाई=stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                                dest, destCapacity, src, src_लंबाई, edits, errorCode);
    वापस u_terminateUChars(dest, destCapacity, dest_लंबाई, &errorCode);
}

U_CFUNC int32_t
ustrcase_mapWithOverlap(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_PARAM
                        char16_t *dest, int32_t destCapacity,
                        स्थिर char16_t *src, int32_t src_लंबाई,
                        UStringCaseMapper *stringCaseMapper,
                        UErrorCode &errorCode) {
    char16_t बफर[300];
    char16_t *अस्थायी;

    int32_t dest_लंबाई;

    /* check argument values */
    अगर(U_FAILURE(errorCode)) {
        वापस 0;
    }
    अगर( destCapacity<0 ||
        (dest==nullptr && destCapacity>0) ||
        src==nullptr ||
        src_लंबाई<-1
    ) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        वापस 0;
    }

    /* get the string length */
    अगर(src_लंबाई==-1) {
        src_लंबाई=u_strlen(src);
    }

    /* check for overlapping source and destination */
    अगर( dest!=nullptr &&
        ((src>=dest && src<(dest+destCapacity)) ||
         (dest>=src && dest<(src+src_लंबाई)))
    ) {
        /* overlap: provide a temporary destination buffer and later copy the result */
        अगर(destCapacity<=UPRV_LENGTHOF(बफर)) {
            /* the stack buffer is large enough */
            अस्थायी=बफर;
        } अन्यथा {
            /* allocate a buffer */
            अस्थायी=(char16_t *)uprv_malloc(destCapacity*U_SIZEOF_UCHAR);
            अगर(अस्थायी==nullptr) {
                errorCode=U_MEMORY_ALLOCATION_ERROR;
                वापस 0;
            }
        }
    } अन्यथा {
        अस्थायी=dest;
    }

    dest_लंबाई=stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                                अस्थायी, destCapacity, src, src_लंबाई, nullptr, errorCode);
    अगर(अस्थायी!=dest) {
        /* copy the result string to the destination buffer */
        अगर (U_SUCCESS(errorCode) && 0 < dest_लंबाई && dest_लंबाई <= destCapacity) {
            u_memmove(dest, अस्थायी, dest_लंबाई);
        }
        अगर(अस्थायी!=बफर) {
            uprv_free(अस्थायी);
        }
    }

    वापस u_terminateUChars(dest, destCapacity, dest_लंबाई, &errorCode);
}

/* public API functions */

U_CAPI int32_t U_EXPORT2
u_strFoldCase(char16_t *dest, int32_t destCapacity,
              स्थिर char16_t *src, int32_t src_लंबाई,
              uint32_t options,
              UErrorCode *pErrorCode) {
    वापस ustrcase_mapWithOverlap(
        UCASE_LOC_मूल, options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, destCapacity,
        src, src_लंबाई,
        ustrcase_internalFold, *pErrorCode);
}

U_NAMESPACE_BEGIN

int32_t CaseMap::fold(
        uint32_t options,
        स्थिर char16_t *src, int32_t src_लंबाई,
        char16_t *dest, int32_t destCapacity, Edits *edits,
        UErrorCode &errorCode) {
    वापस ustrcase_map(
        UCASE_LOC_मूल, options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, destCapacity,
        src, src_लंबाई,
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
काष्ठा CmpEquivLevel {
    स्थिर char16_t *start, *s, *limit;
};
प्रकार काष्ठा CmpEquivLevel CmpEquivLevel;

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
अटल int32_t _cmpFold(
            स्थिर char16_t *s1, int32_t length1,
            स्थिर char16_t *s2, int32_t length2,
            uint32_t options,
            int32_t *matchLen1, int32_t *matchLen2,
            UErrorCode *pErrorCode) {
    int32_t cmpRes = 0;

    /* current-level start/limit - s1/s2 as current */
    स्थिर char16_t *start1, *start2, *limit1, *limit2;

    /* points to the original start address */
    स्थिर char16_t *org1, *org2;

    /* points to the end of match + 1 */
    स्थिर char16_t *m1, *m2;

    /* case folding variables */
    स्थिर char16_t *p;
    int32_t length;

    /* stacks of previous-level start/current/limit */
    CmpEquivLevel stack1[2], stack2[2];

    /* case folding buffers, only use current-level start/limit */
    char16_t fold1[UCASE_MAX_STRING_लंबाई+1], fold2[UCASE_MAX_STRING_लंबाई+1];

    /* track which is the current level per string */
    int32_t level1, level2;

    /* current code units, and code points for lookups */
    UChar32 c1, c2, cp1, cp2;

    /* no argument error checking because this itself is not an API */

    /*
     * assume that at least the option U_COMPARE_IGNORE_CASE is set
     * otherwise this function would have to behave exactly as uprv_strCompare()
     */
    अगर(U_FAILURE(*pErrorCode)) {
        वापस 0;
    }

    /* initialize */
    अगर(matchLen1) {
        U_ASSERT(matchLen2 !=nullptr);
        *matchLen1=0;
        *matchLen2=0;
    }

    start1=m1=org1=s1;
    अगर(length1==-1) {
        limit1=nullptr;
    } अन्यथा {
        limit1=s1+length1;
    }

    start2=m2=org2=s2;
    अगर(length2==-1) {
        limit2=nullptr;
    } अन्यथा {
        limit2=s2+length2;
    }

    level1=level2=0;
    c1=c2=-1;

    /* comparison loop */
    क्रम(;;) {
        /*
         * here a code unit value of -1 means "get another code unit"
         * below it will mean "this source is finished"
         */

        अगर(c1<0) {
            /* get next code unit from string 1, post-increment */
            क्रम(;;) {
                अगर(s1==limit1 || ((c1=*s1)==0 && (limit1==nullptr || (options&_STRNCMP_STYLE)))) {
                    अगर(level1==0) {
                        c1=-1;
                        अवरोध;
                    }
                } अन्यथा {
                    ++s1;
                    अवरोध;
                }

                /* reached end of level buffer, pop one level */
                करो {
                    --level1;
                    start1=stack1[level1].start;    /*Not uninitialized*/
                } जबतक(start1==nullptr);
                s1=stack1[level1].s;                /*Not uninitialized*/
                limit1=stack1[level1].limit;        /*Not uninitialized*/
            }
        }

        अगर(c2<0) {
            /* get next code unit from string 2, post-increment */
            क्रम(;;) {
                अगर(s2==limit2 || ((c2=*s2)==0 && (limit2==nullptr || (options&_STRNCMP_STYLE)))) {
                    अगर(level2==0) {
                        c2=-1;
                        अवरोध;
                    }
                } अन्यथा {
                    ++s2;
                    अवरोध;
                }

                /* reached end of level buffer, pop one level */
                करो {
                    --level2;
                    start2=stack2[level2].start;    /*Not uninitialized*/
                } जबतक(start2==nullptr);
                s2=stack2[level2].s;                /*Not uninitialized*/
                limit2=stack2[level2].limit;        /*Not uninitialized*/
            }
        }

        /*
         * compare c1 and c2
         * either variable c1, c2 is -1 only if the corresponding string is finished
         */
        अगर(c1==c2) {
            स्थिर char16_t *next1, *next2;

            अगर(c1<0) {
                cmpRes=0;   /* c1==c2==-1 indicating end of strings */
                अवरोध;
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
            अगर(level1==0) {
                next1=s1;
            } अन्यथा अगर(s1==limit1) {
                /* Note: This implementation only use a single level of stack.
                 *      If this code needs to be changed to use multiple levels
                 *      of stacks, the code above should check if the current
                 *      code is at the end of all stacks.
                 */
                U_ASSERT(level1==1);

                /* is s1 at the end of the current stack? */
                next1=stack1[0].s;
            }

            अगर (next1!=nullptr) {
                अगर(level2==0) {
                    next2=s2;
                } अन्यथा अगर(s2==limit2) {
                    U_ASSERT(level2==1);

                    /* is s2 at the end of the current stack? */
                    next2=stack2[0].s;
                }
                अगर(next2!=nullptr) {
                    m1=next1;
                    m2=next2;
                }
            }
            c1=c2=-1;       /* make us fetch new code units */
            जारी;
        } अन्यथा अगर(c1<0) {
            cmpRes=-1;      /* string 1 ends before string 2 */
            अवरोध;
        } अन्यथा अगर(c2<0) {
            cmpRes=1;       /* string 2 ends before string 1 */
            अवरोध;
        }
        /* c1!=c2 && c1>=0 && c2>=0 */

        /* get complete code points for c1, c2 for lookups if either is a surrogate */
        cp1=c1;
        अगर(U_IS_SURROGATE(c1)) {
            char16_t c;

            अगर(U_IS_SURROGATE_LEAD(c1)) {
                अगर(s1!=limit1 && U16_IS_TRAIL(c=*s1)) {
                    /* advance ++s1; only below if cp1 decomposes/case-folds */
                    cp1=U16_GET_SUPPLEMENTARY(c1, c);
                }
            } अन्यथा /* isTrail(c1) */ {
                अगर(start1<=(s1-2) && U16_IS_LEAD(c=*(s1-2))) {
                    cp1=U16_GET_SUPPLEMENTARY(c, c1);
                }
            }
        }

        cp2=c2;
        अगर(U_IS_SURROGATE(c2)) {
            char16_t c;

            अगर(U_IS_SURROGATE_LEAD(c2)) {
                अगर(s2!=limit2 && U16_IS_TRAIL(c=*s2)) {
                    /* advance ++s2; only below if cp2 decomposes/case-folds */
                    cp2=U16_GET_SUPPLEMENTARY(c2, c);
                }
            } अन्यथा /* isTrail(c2) */ {
                अगर(start2<=(s2-2) && U16_IS_LEAD(c=*(s2-2))) {
                    cp2=U16_GET_SUPPLEMENTARY(c, c2);
                }
            }
        }

        /*
         * go down one level for each string
         * continue with the main loop as soon as there is a real change
         */

        अगर( level1==0 &&
            (length = ucase_toFullFolding(cp1, &p, options)) >= 0
        ) {
            /* cp1 case-folds to the code point "length" or to p[length] */
            अगर(U_IS_SURROGATE(c1)) {
                अगर(U_IS_SURROGATE_LEAD(c1)) {
                    /* advance beyond source surrogate pair if it case-folds */
                    ++s1;
                } अन्यथा /* isTrail(c1) */ {
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
            अगर(length<=UCASE_MAX_STRING_लंबाई) {
                u_memcpy(fold1, p, length);
            } अन्यथा {
                int32_t i=0;
                U16_APPEND_UNSAFE(fold1, i, length);
                length=i;
            }

            /* set next level pointers to case folding */
            start1=s1=fold1;
            limit1=fold1+length;

            /* get ready to read from decomposition, continue with loop */
            c1=-1;
            जारी;
        }

        अगर( level2==0 &&
            (length = ucase_toFullFolding(cp2, &p, options)) >= 0
        ) {
            /* cp2 case-folds to the code point "length" or to p[length] */
            अगर(U_IS_SURROGATE(c2)) {
                अगर(U_IS_SURROGATE_LEAD(c2)) {
                    /* advance beyond source surrogate pair if it case-folds */
                    ++s2;
                } अन्यथा /* isTrail(c2) */ {
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
            अगर(length<=UCASE_MAX_STRING_लंबाई) {
                u_memcpy(fold2, p, length);
            } अन्यथा {
                int32_t i=0;
                U16_APPEND_UNSAFE(fold2, i, length);
                length=i;
            }

            /* set next level pointers to case folding */
            start2=s2=fold2;
            limit2=fold2+length;

            /* get ready to read from decomposition, continue with loop */
            c2=-1;
            जारी;
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

        अगर(c1>=0xd800 && c2>=0xd800 && (options&U_COMPARE_CODE_POINT_ORDER)) {
            /* subtract 0x2800 from BMP code points to make them smaller than supplementary ones */
            अगर(
                (c1<=0xdbff && s1!=limit1 && U16_IS_TRAIL(*s1)) ||
                (U16_IS_TRAIL(c1) && start1!=(s1-1) && U16_IS_LEAD(*(s1-2)))
            ) {
                /* part of a surrogate pair, leave >=d800 */
            } अन्यथा {
                /* BMP code point - may be surrogate code point - make <d800 */
                c1-=0x2800;
            }

            अगर(
                (c2<=0xdbff && s2!=limit2 && U16_IS_TRAIL(*s2)) ||
                (U16_IS_TRAIL(c2) && start2!=(s2-1) && U16_IS_LEAD(*(s2-2)))
            ) {
                /* part of a surrogate pair, leave >=d800 */
            } अन्यथा {
                /* BMP code point - may be surrogate code point - make <d800 */
                c2-=0x2800;
            }
        }

        cmpRes=c1-c2;
        अवरोध;
    }

    अगर(matchLen1) {
        *matchLen1=static_cast<int32_t>(m1-org1);
        *matchLen2=static_cast<int32_t>(m2-org2);
    }
    वापस cmpRes;
}

/* internal function */
U_CFUNC int32_t
u_strcmpFold(स्थिर char16_t *s1, int32_t length1,
             स्थिर char16_t *s2, int32_t length2,
             uint32_t options,
             UErrorCode *pErrorCode) {
    वापस _cmpFold(s1, length1, s2, length2, options, nullptr, nullptr, pErrorCode);
}

/* public API functions */

U_CAPI int32_t U_EXPORT2
u_strCaseCompare(स्थिर char16_t *s1, int32_t length1,
                 स्थिर char16_t *s2, int32_t length2,
                 uint32_t options,
                 UErrorCode *pErrorCode) {
    /* argument checking */
    अगर (pErrorCode == nullptr || U_FAILURE(*pErrorCode)) {
        वापस 0;
    }
    अगर(s1==nullptr || length1<-1 || s2==nullptr || length2<-1) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        वापस 0;
    }
    वापस u_strcmpFold(s1, length1, s2, length2,
                        options|U_COMPARE_IGNORE_CASE,
                        pErrorCode);
}

U_CAPI int32_t U_EXPORT2
u_strcasecmp(स्थिर char16_t *s1, स्थिर char16_t *s2, uint32_t options) {
    UErrorCode errorCode=U_ZERO_ERROR;
    वापस u_strcmpFold(s1, -1, s2, -1,
                        options|U_COMPARE_IGNORE_CASE,
                        &errorCode);
}

U_CAPI int32_t U_EXPORT2
u_memcasecmp(स्थिर char16_t *s1, स्थिर char16_t *s2, int32_t length, uint32_t options) {
    UErrorCode errorCode=U_ZERO_ERROR;
    वापस u_strcmpFold(s1, length, s2, length,
                        options|U_COMPARE_IGNORE_CASE,
                        &errorCode);
}

U_CAPI int32_t U_EXPORT2
u_strncasecmp(स्थिर char16_t *s1, स्थिर char16_t *s2, int32_t n, uint32_t options) {
    UErrorCode errorCode=U_ZERO_ERROR;
    वापस u_strcmpFold(s1, n, s2, n,
                        options|(U_COMPARE_IGNORE_CASE|_STRNCMP_STYLE),
                        &errorCode);
}

/* internal API - detect length of shared prefix */
U_CAPI व्योम
u_caseInsensitivePrefixMatch(स्थिर char16_t *s1, int32_t length1,
                             स्थिर char16_t *s2, int32_t length2,
                             uint32_t options,
                             int32_t *matchLen1, int32_t *matchLen2,
                             UErrorCode *pErrorCode) {
    _cmpFold(s1, length1, s2, length2, options,
        matchLen1, matchLen2, pErrorCode);
}
