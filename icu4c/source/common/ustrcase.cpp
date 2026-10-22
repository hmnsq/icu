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

#অন্তর্ভাব "unicode/utypes.h"
#অন্তর্ভাব "unicode/brkiter.h"
#অন্তর্ভাব "unicode/casemap.h"
#অন্তর্ভাব "unicode/edits.h"
#অন্তর্ভাব "unicode/stringoptions.h"
#অন্তর্ভাব "unicode/ustring.h"
#অন্তর্ভাব "unicode/ucasemap.h"
#অন্তর্ভাব "unicode/ubrk.h"
#অন্তর্ভাব "unicode/utf.h"
#অন্তর্ভাব "unicode/utf16.h"
#অন্তর্ভাব "cmemory.h"
#অন্তর্ভাব "ucase.h"
#অন্তর্ভাব "ucasemap_imp.h"
#অন্তর্ভাব "ustr_imp.h"
#অন্তর্ভাব "uassert.h"

/**
 * Code point for COMBINING ACUTE ACCENT
 * @internal
 */
#define ACUTE u'\u0301'

U_NAMESPACE_BEGIN

নামাকাশ {

int32_t checkOverflowAndEditsError(int32_t dest_সূচক, int32_t destCapacity,
                                   Edits *edits, UErrorCode &errorCode) {
    যদি (U_SUCCESS(errorCode)) {
        যদি (dest_সূচক > destCapacity) {
            errorCode = U_বাফার_OVERFLOW_ERROR;
        } অন্যথা যদি (edits != nullptr) {
            edits->copyErrorTo(errorCode);
        }
    }
    ফেরত dest_সূচক;
}

/* Appends a full case mapping result, see UCASE_MAX_STRING_LENGTH. */
অন্তঃরেখ int32_t
append_ফলাফল(char16_t *dest, int32_t dest_সূচক, int32_t destCapacity,
             int32_t ফলাফল, ধ্রুবক char16_t *s,
             int32_t cp_দৈর্ঘ্য, uint32_t options, icu::Edits *edits) {
    UChar32 c;
    int32_t length;

    /* decode the result */
    যদি(ফলাফল<0) {
        /* (not) original code point */
        যদি(edits!=nullptr) {
            edits->addUnchanged(cp_দৈর্ঘ্য);
        }
        যদি(options & U_OMIT_UNCHANGED_TEXT) {
            ফেরত dest_সূচক;
        }
        c=~ফলাফল;
        যদি(dest_সূচক<destCapacity && c<=0xffff) {  // BMP slightly-fastpath
            dest[dest_সূচক++] = static_cast<char16_t>(c);
            ফেরত dest_সূচক;
        }
        length=cp_দৈর্ঘ্য;
    } অন্যথা {
        যদি(ফলাফল<=UCASE_MAX_STRING_দৈর্ঘ্য) {
            c=U_SENTINEL;
            length=ফলাফল;
        } অন্যথা যদি(dest_সূচক<destCapacity && ফলাফল<=0xffff) {  // BMP slightly-fastpath
            dest[dest_সূচক++] = static_cast<char16_t>(ফলাফল);
            যদি(edits!=nullptr) {
                edits->addReplace(cp_দৈর্ঘ্য, 1);
            }
            ফেরত dest_সূচক;
        } অন্যথা {
            c=ফলাফল;
            length=U16_দৈর্ঘ্য(c);
        }
        যদি(edits!=nullptr) {
            edits->addReplace(cp_দৈর্ঘ্য, length);
        }
    }
    যদি(length>(INT32_MAX-dest_সূচক)) {
        ফেরত -1;  // integer overflow
    }

    যদি(dest_সূচক<destCapacity) {
        /* append the result */
        যদি(c>=0) {
            /* code point */
            UBool isError=মিথ্যা;
            U16_APPEND(dest, dest_সূচক, destCapacity, c, isError);
            যদি(isError) {
                /* overflow, nothing written */
                dest_সূচক+=length;
            }
        } অন্যথা {
            /* string */
            যদি((dest_সূচক+length)<=destCapacity) {
                যখন(length>0) {
                    dest[dest_সূচক++]=*s++;
                    --length;
                }
            } অন্যথা {
                /* overflow */
                dest_সূচক+=length;
            }
        }
    } অন্যথা {
        /* preflight */
        dest_সূচক+=length;
    }
    ফেরত dest_সূচক;
}

অন্তঃরেখ int32_t
appendUChar(char16_t *dest, int32_t dest_সূচক, int32_t destCapacity, char16_t c) {
    যদি(dest_সূচক<destCapacity) {
        dest[dest_সূচক]=c;
    } অন্যথা যদি(dest_সূচক==INT32_MAX) {
        ফেরত -1;  // integer overflow
    }
    ফেরত dest_সূচক+1;
}

int32_t
appendNonEmptyUnchanged(char16_t *dest, int32_t dest_সূচক, int32_t destCapacity,
                        ধ্রুবক char16_t *s, int32_t length, uint32_t options, icu::Edits *edits) {
    যদি(edits!=nullptr) {
        edits->addUnchanged(length);
    }
    যদি(options & U_OMIT_UNCHANGED_TEXT) {
        ফেরত dest_সূচক;
    }
    যদি(length>(INT32_MAX-dest_সূচক)) {
        ফেরত -1;  // integer overflow
    }
    যদি((dest_সূচক+length)<=destCapacity) {
        u_memcpy(dest+dest_সূচক, s, length);
    }
    ফেরত dest_সূচক + length;
}

অন্তঃরেখ int32_t
appendUnchanged(char16_t *dest, int32_t dest_সূচক, int32_t destCapacity,
                ধ্রুবক char16_t *s, int32_t length, uint32_t options, icu::Edits *edits) {
    যদি (length <= 0) {
        ফেরত dest_সূচক;
    }
    ফেরত appendNonEmptyUnchanged(dest, dest_সূচক, destCapacity, s, length, options, edits);
}

UChar32 U_CALLCONV
utf16_caseContextIterator(শূন্য *context, int8_t dir) {
    UCaseContext* csc = static_cast<UCaseContext*>(context);
    UChar32 c;

    যদি(dir<0) {
        /* reset for backward iteration */
        csc->সূচক=csc->cpStart;
        csc->dir=dir;
    } অন্যথা যদি(dir>0) {
        /* reset for forward iteration */
        csc->সূচক=csc->cpLimit;
        csc->dir=dir;
    } অন্যথা {
        /* continue current iteration direction */
        dir=csc->dir;
    }

    যদি(dir<0) {
        যদি(csc->start<csc->সূচক) {
            U16_পূর্ব((ধ্রুবক char16_t *)csc->p, csc->start, csc->সূচক, c);
            ফেরত c;
        }
    } অন্যথা {
        যদি(csc->সূচক<csc->limit) {
            U16_পরবর্তী((ধ্রুবক char16_t *)csc->p, csc->সূচক, csc->limit, c);
            ফেরত c;
        }
    }
    ফেরত U_SENTINEL;
}

/**
 * caseLocale >= 0: Lowercases [srcStart..srcLimit[ but takes context [0..srcLength[ into account.
 * caseLocale < 0: Case-folds [srcStart..srcLimit[.
 */
int32_t toLower(int32_t caseLocale, uint32_t options,
                char16_t *dest, int32_t destCapacity,
                ধ্রুবক char16_t *src, UCaseContext *csc, int32_t srcStart, int32_t srcLimit,
                icu::Edits *edits, UErrorCode &errorCode) {
    ধ্রুবক int8_t *latinToLower;
    যদি (caseLocale == UCASE_LOC_মূল ||
            (caseLocale >= 0 ?
                !(caseLocale == UCASE_LOC_TURKISH || caseLocale == UCASE_LOC_LITHUANIAN) :
                (options & _FOLD_CASE_OPTIONS_MASK) == U_FOLD_CASE_DEFAULT)) {
        latinToLower = LatinCase::TO_LOWER_NORMAL;
    } অন্যথা {
        latinToLower = LatinCase::TO_LOWER_TR_LT;
    }
    ধ্রুবক UTrie2 *trie = ucase_getTrie();
    int32_t dest_সূচক = 0;
    int32_t পূর্ব = srcStart;
    int32_t src_সূচক = srcStart;
    জন্য (;;) {
        // fast path for simple cases
        char16_t lead = 0;
        যখন (src_সূচক < srcLimit) {
            lead = src[src_সূচক];
            int32_t delta;
            যদি (lead < LatinCase::LONG_S) {
                int8_t d = latinToLower[lead];
                যদি (d == LatinCase::EXC) { বিরতি; }
                ++src_সূচক;
                যদি (d == 0) { চালু; }
                delta = d;
            } অন্যথা যদি (lead >= 0xd800) {
                বিরতি;  // surrogate or higher
            } অন্যথা {
                uint16_t props = UTRIE2_GET16_FROM_U16_SINGLE_LEAD(trie, lead);
                যদি (UCASE_HAS_EXCEPTION(props)) { বিরতি; }
                ++src_সূচক;
                যদি (!UCASE_IS_UPPER_OR_TITLE(props) || (delta = UCASE_GET_DELTA(props)) == 0) {
                    চালু;
                }
            }
            lead += static_cast<char16_t>(delta);
            dest_সূচক = appendUnchanged(dest, dest_সূচক, destCapacity,
                                        src + পূর্ব, src_সূচক - 1 - পূর্ব, options, edits);
            যদি (dest_সূচক >= 0) {
                dest_সূচক = appendUChar(dest, dest_সূচক, destCapacity, lead);
                যদি (edits != nullptr) {
                    edits->addReplace(1, 1);
                }
            }
            যদি (dest_সূচক < 0) {
                errorCode = U_সূচক_OUTOFBOUNDS_ERROR;
                ফেরত 0;
            }
            পূর্ব = src_সূচক;
        }
        যদি (src_সূচক >= srcLimit) {
            বিরতি;
        }
        // slow path
        int32_t cpStart = src_সূচক++;
        char16_t trail;
        UChar32 c;
        যদি (U16_IS_LEAD(lead) && src_সূচক < srcLimit && U16_IS_TRAIL(trail = src[src_সূচক])) {
            c = U16_GET_SUPPLEMENTARY(lead, trail);
            ++src_সূচক;
        } অন্যথা {
            c = lead;
        }
        ধ্রুবক char16_t *s = nullptr;
        যদি (caseLocale >= 0) {
            csc->cpStart = cpStart;
            csc->cpLimit = src_সূচক;
            c = ucase_toFullLower(c, utf16_caseContextIterator, csc, &s, caseLocale);
        } অন্যথা {
            c = ucase_toFullFolding(c, &s, options);
        }
        যদি (c >= 0) {
            dest_সূচক = appendUnchanged(dest, dest_সূচক, destCapacity,
                                        src + পূর্ব, cpStart - পূর্ব, options, edits);
            যদি (dest_সূচক >= 0) {
                dest_সূচক = append_ফলাফল(dest, dest_সূচক, destCapacity, c, s,
                                         src_সূচক - cpStart, options, edits);
            }
            যদি (dest_সূচক < 0) {
                errorCode = U_সূচক_OUTOFBOUNDS_ERROR;
                ফেরত 0;
            }
            পূর্ব = src_সূচক;
        }
    }
    dest_সূচক = appendUnchanged(dest, dest_সূচক, destCapacity,
                                src + পূর্ব, src_সূচক - পূর্ব, options, edits);
    যদি (dest_সূচক < 0) {
        errorCode = U_সূচক_OUTOFBOUNDS_ERROR;
        ফেরত 0;
    }
    ফেরত dest_সূচক;
}

int32_t toUpper(int32_t caseLocale, uint32_t options,
                char16_t *dest, int32_t destCapacity,
                ধ্রুবক char16_t *src, UCaseContext *csc, int32_t src_দৈর্ঘ্য,
                icu::Edits *edits, UErrorCode &errorCode) {
    ধ্রুবক int8_t *latinToUpper;
    যদি (caseLocale == UCASE_LOC_TURKISH) {
        latinToUpper = LatinCase::TO_UPPER_TR;
    } অন্যথা {
        latinToUpper = LatinCase::TO_UPPER_NORMAL;
    }
    ধ্রুবক UTrie2 *trie = ucase_getTrie();
    int32_t dest_সূচক = 0;
    int32_t পূর্ব = 0;
    int32_t src_সূচক = 0;
    জন্য (;;) {
        // fast path for simple cases
        char16_t lead = 0;
        যখন (src_সূচক < src_দৈর্ঘ্য) {
            lead = src[src_সূচক];
            int32_t delta;
            যদি (lead < LatinCase::LONG_S) {
                int8_t d = latinToUpper[lead];
                যদি (d == LatinCase::EXC) { বিরতি; }
                ++src_সূচক;
                যদি (d == 0) { চালু; }
                delta = d;
            } অন্যথা যদি (lead >= 0xd800) {
                বিরতি;  // surrogate or higher
            } অন্যথা {
                uint16_t props = UTRIE2_GET16_FROM_U16_SINGLE_LEAD(trie, lead);
                যদি (UCASE_HAS_EXCEPTION(props)) { বিরতি; }
                ++src_সূচক;
                যদি (UCASE_GET_TYPE(props) != UCASE_LOWER || (delta = UCASE_GET_DELTA(props)) == 0) {
                    চালু;
                }
            }
            lead += static_cast<char16_t>(delta);
            dest_সূচক = appendUnchanged(dest, dest_সূচক, destCapacity,
                                        src + পূর্ব, src_সূচক - 1 - পূর্ব, options, edits);
            যদি (dest_সূচক >= 0) {
                dest_সূচক = appendUChar(dest, dest_সূচক, destCapacity, lead);
                যদি (edits != nullptr) {
                    edits->addReplace(1, 1);
                }
            }
            যদি (dest_সূচক < 0) {
                errorCode = U_সূচক_OUTOFBOUNDS_ERROR;
                ফেরত 0;
            }
            পূর্ব = src_সূচক;
        }
        যদি (src_সূচক >= src_দৈর্ঘ্য) {
            বিরতি;
        }
        // slow path
        int32_t cpStart;
        csc->cpStart = cpStart = src_সূচক++;
        char16_t trail;
        UChar32 c;
        যদি (U16_IS_LEAD(lead) && src_সূচক < src_দৈর্ঘ্য && U16_IS_TRAIL(trail = src[src_সূচক])) {
            c = U16_GET_SUPPLEMENTARY(lead, trail);
            ++src_সূচক;
        } অন্যথা {
            c = lead;
        }
        csc->cpLimit = src_সূচক;
        ধ্রুবক char16_t *s = nullptr;
        c = ucase_toFullUpper(c, utf16_caseContextIterator, csc, &s, caseLocale);
        যদি (c >= 0) {
            dest_সূচক = appendUnchanged(dest, dest_সূচক, destCapacity,
                                        src + পূর্ব, cpStart - পূর্ব, options, edits);
            যদি (dest_সূচক >= 0) {
                dest_সূচক = append_ফলাফল(dest, dest_সূচক, destCapacity, c, s,
                                         src_সূচক - cpStart, options, edits);
            }
            যদি (dest_সূচক < 0) {
                errorCode = U_সূচক_OUTOFBOUNDS_ERROR;
                ফেরত 0;
            }
            পূর্ব = src_সূচক;
        }
    }
    dest_সূচক = appendUnchanged(dest, dest_সূচক, destCapacity,
                                src + পূর্ব, src_সূচক - পূর্ব, options, edits);
    যদি (dest_সূচক < 0) {
        errorCode = U_সূচক_OUTOFBOUNDS_ERROR;
        ফেরত 0;
    }
    ফেরত dest_সূচক;
}

}  // namespace

U_NAMESPACE_END

U_NAMESPACE_USE

#যদি !UCONFIG_NO_BREAK_ITERATION

নামাকাশ {

/**
 * Input: c is a letter I with or without acute accent.
 * start is the index in src after c, and is less than segmentLimit.
 * If a plain i/I is followed by a plain j/J,
 * or an i/I with acute (precomposed or decomposed) is followed by a j/J with acute,
 * then we output accordingly.
 *
 * @return the src index after the titlecased sequence, or the start index if no Dutch IJ
 */
int32_t maybeTitleDutchIJ(ধ্রুবক char16_t *src, UChar32 c, int32_t start, int32_t segmentLimit,
                          char16_t *dest, int32_t &dest_সূচক, int32_t destCapacity, uint32_t options,
                          icu::Edits *edits) {
    U_ASSERT(start < segmentLimit);

    int32_t সূচক = start;
    যৌক্তিক withAcute = মিথ্যা;

    // If the conditions are met, then the following variables tell us what to output.
    int32_t unchanged1 = 0;  // code units before the j, or the whole sequence (0..3)
    যৌক্তিক doTitleJ = মিথ্যা;  // true if the j needs to be titlecased
    int32_t unchanged2 = 0;  // after the j (0 or 1)

    // next character after the first letter
    char16_t c2 = src[সূচক++];

    // Is the first letter an i/I with accent?
    যদি (c == u'I') {
        যদি (c2 == ACUTE) {
            withAcute = সত্য;
            unchanged1 = 1;
            যদি (সূচক == segmentLimit) { ফেরত start; }
            c2 = src[সূচক++];
        }
    } অন্যথা {  // Í
        withAcute = সত্য;
    }

    // Is the next character a j/J?
    যদি (c2 == u'j') {
        doTitleJ = সত্য;
    } অন্যথা যদি (c2 == u'J') {
        ++unchanged1;
    } অন্যথা {
        ফেরত start;
    }

    // A plain i/I must be followed by a plain j/J.
    // An i/I with acute must be followed by a j/J with acute.
    যদি (withAcute) {
        যদি (সূচক == segmentLimit || src[সূচক++] != ACUTE) { ফেরত start; }
        যদি (doTitleJ) {
            unchanged2 = 1;
        } অন্যথা {
            ++unchanged1;
        }
    }

    // There must not be another combining mark.
    যদি (সূচক < segmentLimit) {
        int32_t cp;
        int32_t i = সূচক;
        U16_পরবর্তী(src, i, segmentLimit, cp);
        uint32_t typeMask = U_GET_GC_MASK(cp);
        যদি ((typeMask & U_GC_M_MASK) != 0) {
            ফেরত start;
        }
    }

    // Output the rest of the Dutch IJ.
    dest_সূচক = appendUnchanged(dest, dest_সূচক, destCapacity, src + start, unchanged1, options, edits);
    start += unchanged1;
    যদি (doTitleJ) {
        dest_সূচক = appendUChar(dest, dest_সূচক, destCapacity, u'J');
        যদি (edits != nullptr) {
            edits->addReplace(1, 1);
        }
        ++start;
    }
    dest_সূচক = appendUnchanged(dest, dest_সূচক, destCapacity, src + start, unchanged2, options, edits);

    U_ASSERT(start + unchanged2 == সূচক);
    ফেরত সূচক;
}

}  // namespace

U_CFUNC int32_t U_CALLCONV
ustrcase_internalToTitle(int32_t caseLocale, uint32_t options, BreakIterator *iter,
                         char16_t *dest, int32_t destCapacity,
                         ধ্রুবক char16_t *src, int32_t src_দৈর্ঘ্য,
                         icu::Edits *edits,
                         UErrorCode &errorCode) {
    যদি (!ustrcase_checkTitleAdjustmentOptions(options, errorCode)) {
        ফেরত 0;
    }

    /* set up local variables */
    UCaseContext csc=UCASECONTEXT_INITIALIZER;
    csc.p=(শূন্য *)src;
    csc.limit=src_দৈর্ঘ্য;
    int32_t dest_সূচক=0;
    int32_t পূর্ব=0;
    যৌক্তিক is_First_সূচক=সত্য;

    /* titlecasing loop */
    যখন(পূর্ব<src_দৈর্ঘ্য) {
        /* find next index where to titlecase */
        int32_t সূচক;
        যদি(is_First_সূচক) {
            is_First_সূচক=মিথ্যা;
            সূচক=iter->first();
        } অন্যথা {
            সূচক=iter->next();
        }
        যদি(সূচক==UBRK_DONE || সূচক>src_দৈর্ঘ্য) {
            সূচক=src_দৈর্ঘ্য;
        }

        /*
         * Segment [prev..index[ into 3 parts:
         * a) skipped characters (copy as-is) [prev..titleStart[
         * b) first letter (titlecase)              [titleStart..titleLimit[
         * c) subsequent characters (lowercase)                 [titleLimit..index[
         */
        যদি(পূর্ব<সূচক) {
            // Find and copy skipped characters [prev..titleStart[
            int32_t titleStart=পূর্ব;
            int32_t titleLimit=পূর্ব;
            UChar32 c;
            U16_পরবর্তী(src, titleLimit, সূচক, c);
            যদি ((options&U_TITLECASE_NO_BREAK_ADJUSTMENT)==0) {
                // Adjust the titlecasing index to the next cased character,
                // or to the next letter/number/symbol/private use.
                // Stop with titleStart<titleLimit<=index
                // if there is a character to be titlecased,
                // or else stop with titleStart==titleLimit==index.
                যৌক্তিক toCased = (options&U_TITLECASE_ADJUST_TO_CASED) != 0;
                যখন (toCased ? UCASE_NONE==ucase_getType(c) : !ustrcase_isLNS(c)) {
                    titleStart=titleLimit;
                    যদি(titleLimit==সূচক) {
                        বিরতি;
                    }
                    U16_পরবর্তী(src, titleLimit, সূচক, c);
                }
                যদি (পূর্ব < titleStart) {
                    dest_সূচক=appendUnchanged(dest, dest_সূচক, destCapacity,
                                              src+পূর্ব, titleStart-পূর্ব, options, edits);
                    যদি(dest_সূচক<0) {
                        errorCode=U_সূচক_OUTOFBOUNDS_ERROR;
                        ফেরত 0;
                    }
                }
            }

            যদি(titleStart<titleLimit) {
                /* titlecase c which is from [titleStart..titleLimit[ */
                csc.cpStart=titleStart;
                csc.cpLimit=titleLimit;
                ধ্রুবক char16_t *s;
                c=ucase_toFullTitle(c, utf16_caseContextIterator, &csc, &s, caseLocale);
                dest_সূচক=append_ফলাফল(dest, dest_সূচক, destCapacity, c, s,
                                       titleLimit-titleStart, options, edits);
                যদি(dest_সূচক<0) {
                    errorCode=U_সূচক_OUTOFBOUNDS_ERROR;
                    ফেরত 0;
                }

                /* Special case Dutch IJ titlecasing */
                যদি (titleStart+1 < সূচক &&
                        caseLocale == UCASE_LOC_DUTCH) {
                    যদি (c < 0) {
                        c = ~c;
                    }

                    যদি (c == u'I' || c == u'Í') {
                        titleLimit = maybeTitleDutchIJ(src, c, titleStart + 1, সূচক,
                                                       dest, dest_সূচক, destCapacity, options,
                                                       edits);
                    }
                }

                /* lowercase [titleLimit..index[ */
                যদি(titleLimit<সূচক) {
                    যদি((options&U_TITLECASE_NO_LOWERCASE)==0) {
                        /* Normal operation: Lowercase the rest of the word. */
                        dest_সূচক+=
                            toLower(
                                caseLocale, options,
                                (dest==nullptr) ? nullptr: dest+dest_সূচক, destCapacity-dest_সূচক,
                                src, &csc, titleLimit, সূচক,
                                edits, errorCode);
                        যদি(errorCode==U_বাফার_OVERFLOW_ERROR) {
                            errorCode=U_ZERO_ERROR;
                        }
                        যদি(U_FAILURE(errorCode)) {
                            ফেরত dest_সূচক;
                        }
                    } অন্যথা {
                        /* Optionally just copy the rest of the word unchanged. */
                        dest_সূচক=appendUnchanged(dest, dest_সূচক, destCapacity,
                                                  src+titleLimit, সূচক-titleLimit, options, edits);
                        যদি(dest_সূচক<0) {
                            errorCode=U_সূচক_OUTOFBOUNDS_ERROR;
                            ফেরত 0;
                        }
                    }
                }
            }
        }

        পূর্ব=সূচক;
    }

    ফেরত checkOverflowAndEditsError(dest_সূচক, destCapacity, edits, errorCode);
}

#endif  // !UCONFIG_NO_BREAK_ITERATION

U_NAMESPACE_BEGIN
নামাকাশ GreekUpper {

// Data generated by prototype code, see
// https://icu.unicode.org/design/case/greek-upper
// TODO: Move this data into ucase.icu.
স্থির ধ্রুবক uint16_t data0370[] = {
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

স্থির ধ্রুবক uint16_t data1F00[] = {
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
স্থির ধ্রুবক uint16_t data2126 = 0x03A9 | HAS_VOWEL;

uint32_t get_Letter_তথ্য(UChar32 c) {
    যদি (c < 0x370 || 0x2126 < c || (0x3ff < c && c < 0x1f00)) {
        ফেরত 0;
    } অন্যথা যদি (c <= 0x3ff) {
        ফেরত data0370[c - 0x370];
    } অন্যথা যদি (c <= 0x1fff) {
        ফেরত data1F00[c - 0x1f00];
    } অন্যথা যদি (c == 0x2126) {
        ফেরত data2126;
    } অন্যথা {
        ফেরত 0;
    }
}

uint32_t get_Diacritic_তথ্য(UChar32 c) {
    নির্বাচন (c) {
    ক্ষেত্র 0x0300:  // varia
    ক্ষেত্র 0x0301:  // tonos = oxia
    ক্ষেত্র 0x0342:  // perispomeni
    ক্ষেত্র 0x0302:  // circumflex can look like perispomeni
    ক্ষেত্র 0x0303:  // tilde can look like perispomeni
    ক্ষেত্র 0x0311:  // inverted breve can look like perispomeni
        ফেরত HAS_ACCENT;
    ক্ষেত্র 0x0308:  // dialytika = diaeresis
        ফেরত HAS_COMBINING_DIALYTIKA;
    ক্ষেত্র 0x0344:  // dialytika tonos
        ফেরত HAS_COMBINING_DIALYTIKA | HAS_ACCENT;
    ক্ষেত্র 0x0345:  // ypogegrammeni = iota subscript
        ফেরত HAS_YPOGEGRAMMENI;
    ক্ষেত্র 0x0304:  // macron
    ক্ষেত্র 0x0306:  // breve
    ক্ষেত্র 0x0313:  // comma above
    ক্ষেত্র 0x0314:  // reversed comma above
    ক্ষেত্র 0x0343:  // koronis
        ফেরত HAS_OTHER_GREEK_DIACRITIC;
    পূর্বনির্ধারিত:
        ফেরত 0;
    }
}

UBool isFollowedByCasedLetter(ধ্রুবক char16_t *s, int32_t i, int32_t length) {
    যখন (i < length) {
        UChar32 c;
        U16_পরবর্তী(s, i, length, c);
        int32_t type = ucase_getTypeOrIgnorable(c);
        যদি ((type & UCASE_IGNORABLE) != 0) {
            // Case-ignorable, continue with the loop.
        } অন্যথা যদি (type != UCASE_NONE) {
            ফেরত সত্য;  // Followed by cased letter.
        } অন্যথা {
            ফেরত মিথ্যা;  // Uncased and not case-ignorable.
        }
    }
    ফেরত মিথ্যা;  // Not followed by cased letter.
}

/**
 * Greek string uppercasing with a state machine.
 * Probably simpler than a stateless function that has to figure out complex context-before
 * for each character.
 * TODO: Try to re-consolidate one way or another with the non-Greek function.
 */
int32_t toUpper(uint32_t options,
                char16_t *dest, int32_t destCapacity,
                ধ্রুবক char16_t *src, int32_t src_দৈর্ঘ্য,
                Edits *edits,
                UErrorCode &errorCode) {
    int32_t dest_সূচক=0;
    uint32_t state = 0;
    জন্য (int32_t i = 0; i < src_দৈর্ঘ্য;) {
        int32_t পরবর্তী_সূচক = i;
        UChar32 c;
        U16_পরবর্তী(src, পরবর্তী_সূচক, src_দৈর্ঘ্য, c);
        uint32_t পরবর্তী_State = 0;
        int32_t type = ucase_getTypeOrIgnorable(c);
        যদি ((type & UCASE_IGNORABLE) != 0) {
            // c is case-ignorable
            পরবর্তী_State |= (state & AFTER_CASED);
        } অন্যথা যদি (type != UCASE_NONE) {
            // c is cased
            পরবর্তী_State |= AFTER_CASED;
        }
        uint32_t তথ্য = get_Letter_তথ্য(c);
        যদি (তথ্য > 0) {
            uint32_t upper = তথ্য & UPPER_MASK;
            // Add a dialytika to this iota or ypsilon vowel
            // if we removed a tonos from the previous vowel,
            // and that previous vowel did not also have (or gain) a dialytika.
            // Adding one only to the final vowel in a longer sequence
            // (which does not occur in normal writing) would require lookahead.
            // Set the same flag as for preserving an existing dialytika.
            যদি ((তথ্য & HAS_VOWEL) != 0 &&
                (state & (AFTER_VOWEL_WITH_PRECOMPOSED_ACCENT | AFTER_VOWEL_WITH_COMBINING_ACCENT)) !=
                    0 &&
                (upper == 0x399 || upper == 0x3A5)) {
                তথ্য |= (state & AFTER_VOWEL_WITH_PRECOMPOSED_ACCENT) ? HAS_DIALYTIKA
                                                                      : HAS_COMBINING_DIALYTIKA;
            }
            int32_t অঙ্ক_Ypogegrammeni = 0;  // Map each one to a trailing, spacing, capital iota.
            যদি ((তথ্য & HAS_YPOGEGRAMMENI) != 0) {
                অঙ্ক_Ypogegrammeni = 1;
            }
            ধ্রুবক UBool hasPrecomposedAccent = (তথ্য & HAS_ACCENT) != 0;
            // Skip combining diacritics after this Greek letter.
            যখন (পরবর্তী_সূচক < src_দৈর্ঘ্য) {
                uint32_t diacritic_তথ্য = get_Diacritic_তথ্য(src[পরবর্তী_সূচক]);
                যদি (diacritic_তথ্য != 0) {
                    তথ্য |= diacritic_তথ্য;
                    যদি ((diacritic_তথ্য & HAS_YPOGEGRAMMENI) != 0) {
                        ++অঙ্ক_Ypogegrammeni;
                    }
                    ++পরবর্তী_সূচক;
                } অন্যথা {
                    বিরতি;  // not a Greek diacritic
                }
            }
            যদি ((তথ্য & HAS_VOWEL_AND_ACCENT_AND_DIALYTIKA) == HAS_VOWEL_AND_ACCENT) {
                পরবর্তী_State |= hasPrecomposedAccent ? AFTER_VOWEL_WITH_PRECOMPOSED_ACCENT
                                                  : AFTER_VOWEL_WITH_COMBINING_ACCENT;
            }
            // Map according to Greek rules.
            UBool addTonos = মিথ্যা;
            যদি (upper == 0x397 &&
                    (তথ্য & HAS_ACCENT) != 0 &&
                    অঙ্ক_Ypogegrammeni == 0 &&
                    (state & AFTER_CASED) == 0 &&
                    !isFollowedByCasedLetter(src, পরবর্তী_সূচক, src_দৈর্ঘ্য)) {
                // Keep disjunctive "or" with (only) a tonos.
                // We use the same "word boundary" conditions as for the Final_Sigma test.
                যদি (hasPrecomposedAccent) {
                    upper = 0x389;  // Preserve the precomposed form.
                } অন্যথা {
                    addTonos = সত্য;
                }
            } অন্যথা যদি ((তথ্য & HAS_DIALYTIKA) != 0) {
                // Preserve a vowel with dialytika in precomposed form if it exists.
                যদি (upper == 0x399) {
                    upper = 0x3AA;
                    তথ্য &= ~HAS_EITHER_DIALYTIKA;
                } অন্যথা যদি (upper == 0x3A5) {
                    upper = 0x3AB;
                    তথ্য &= ~HAS_EITHER_DIALYTIKA;
                }
            }

            UBool change;
            যদি (edits == nullptr && (options & U_OMIT_UNCHANGED_TEXT) == 0) {
                change = সত্য;  // common, simple usage
            } অন্যথা {
                // Find out first whether we are changing the text.
                change = src[i] != upper || অঙ্ক_Ypogegrammeni > 0;
                int32_t i2 = i + 1;
                যদি ((তথ্য & HAS_EITHER_DIALYTIKA) != 0) {
                    change |= i2 >= পরবর্তী_সূচক || src[i2] != 0x308;
                    ++i2;
                }
                যদি (addTonos) {
                    change |= i2 >= পরবর্তী_সূচক || src[i2] != 0x301;
                    ++i2;
                }
                int32_t old_দৈর্ঘ্য = পরবর্তী_সূচক - i;
                int32_t new_দৈর্ঘ্য = (i2 - i) + অঙ্ক_Ypogegrammeni;
                change |= old_দৈর্ঘ্য != new_দৈর্ঘ্য;
                যদি (change) {
                    যদি (edits != nullptr) {
                        edits->addReplace(old_দৈর্ঘ্য, new_দৈর্ঘ্য);
                    }
                } অন্যথা {
                    যদি (edits != nullptr) {
                        edits->addUnchanged(old_দৈর্ঘ্য);
                    }
                    // Write unchanged text?
                    change = (options & U_OMIT_UNCHANGED_TEXT) == 0;
                }
            }

            যদি (change) {
                dest_সূচক = appendUChar(dest, dest_সূচক, destCapacity, static_cast<char16_t>(upper));
                যদি (dest_সূচক >= 0 && (তথ্য & HAS_EITHER_DIALYTIKA) != 0) {
                    dest_সূচক=appendUChar(dest, dest_সূচক, destCapacity, 0x308);  // restore or add a dialytika
                }
                যদি (dest_সূচক >= 0 && addTonos) {
                    dest_সূচক=appendUChar(dest, dest_সূচক, destCapacity, 0x301);
                }
                যখন (dest_সূচক >= 0 && অঙ্ক_Ypogegrammeni > 0) {
                    dest_সূচক=appendUChar(dest, dest_সূচক, destCapacity, 0x399);
                    --অঙ্ক_Ypogegrammeni;
                }
                যদি(dest_সূচক<0) {
                    errorCode=U_সূচক_OUTOFBOUNDS_ERROR;
                    ফেরত 0;
                }
            }
        } অন্যথা {
            ধ্রুবক char16_t *s;
            c=ucase_toFullUpper(c, nullptr, nullptr, &s, UCASE_LOC_GREEK);
            dest_সূচক = append_ফলাফল(dest, dest_সূচক, destCapacity, c, s,
                                     পরবর্তী_সূচক - i, options, edits);
            যদি (dest_সূচক < 0) {
                errorCode = U_সূচক_OUTOFBOUNDS_ERROR;
                ফেরত 0;
            }
        }
        i = পরবর্তী_সূচক;
        state = পরবর্তী_State;
    }

    ফেরত dest_সূচক;
}

}  // namespace GreekUpper
U_NAMESPACE_END

/* functions available in the common library (for unistr_case.cpp) */

U_CFUNC int32_t U_CALLCONV
ustrcase_internalToLower(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_UNUSED
                         char16_t *dest, int32_t destCapacity,
                         ধ্রুবক char16_t *src, int32_t src_দৈর্ঘ্য,
                         icu::Edits *edits,
                         UErrorCode &errorCode) {
    UCaseContext csc=UCASECONTEXT_INITIALIZER;
    csc.p=(শূন্য *)src;
    csc.limit=src_দৈর্ঘ্য;
    int32_t dest_সূচক = toLower(
        caseLocale, options,
        dest, destCapacity,
        src, &csc, 0, src_দৈর্ঘ্য,
        edits, errorCode);
    ফেরত checkOverflowAndEditsError(dest_সূচক, destCapacity, edits, errorCode);
}

U_CFUNC int32_t U_CALLCONV
ustrcase_internalToUpper(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_UNUSED
                         char16_t *dest, int32_t destCapacity,
                         ধ্রুবক char16_t *src, int32_t src_দৈর্ঘ্য,
                         icu::Edits *edits,
                         UErrorCode &errorCode) {
    int32_t dest_সূচক;
    যদি (caseLocale == UCASE_LOC_GREEK) {
        dest_সূচক = GreekUpper::toUpper(options, dest, destCapacity,
                                        src, src_দৈর্ঘ্য, edits, errorCode);
    } অন্যথা {
        UCaseContext csc=UCASECONTEXT_INITIALIZER;
        csc.p=(শূন্য *)src;
        csc.limit=src_দৈর্ঘ্য;
        dest_সূচক = toUpper(
            caseLocale, options,
            dest, destCapacity,
            src, &csc, src_দৈর্ঘ্য,
            edits, errorCode);
    }
    ফেরত checkOverflowAndEditsError(dest_সূচক, destCapacity, edits, errorCode);
}

U_CFUNC int32_t U_CALLCONV
ustrcase_internalFold(int32_t /* caseLocale */, uint32_t options, UCASEMAP_BREAK_ITERATOR_UNUSED
                      char16_t *dest, int32_t destCapacity,
                      ধ্রুবক char16_t *src, int32_t src_দৈর্ঘ্য,
                      icu::Edits *edits,
                      UErrorCode &errorCode) {
    int32_t dest_সূচক = toLower(
        -1, options,
        dest, destCapacity,
        src, nullptr, 0, src_দৈর্ঘ্য,
        edits, errorCode);
    ফেরত checkOverflowAndEditsError(dest_সূচক, destCapacity, edits, errorCode);
}

U_CFUNC int32_t
ustrcase_map(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_PARAM
             char16_t *dest, int32_t destCapacity,
             ধ্রুবক char16_t *src, int32_t src_দৈর্ঘ্য,
             UStringCaseMapper *stringCaseMapper,
             icu::Edits *edits,
             UErrorCode &errorCode) {
    int32_t dest_দৈর্ঘ্য;

    /* check argument values */
    যদি(U_FAILURE(errorCode)) {
        ফেরত 0;
    }
    যদি( destCapacity<0 ||
        (dest==nullptr && destCapacity>0) ||
        src==nullptr ||
        src_দৈর্ঘ্য<-1
    ) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত 0;
    }

    /* get the string length */
    যদি(src_দৈর্ঘ্য==-1) {
        src_দৈর্ঘ্য=u_strlen(src);
    }

    /* check for overlapping source and destination */
    যদি( dest!=nullptr &&
        ((src>=dest && src<(dest+destCapacity)) ||
         (dest>=src && dest<(src+src_দৈর্ঘ্য)))
    ) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত 0;
    }

    যদি (edits != nullptr && (options & U_EDITS_NO_RESET) == 0) {
        edits->reset();
    }
    dest_দৈর্ঘ্য=stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                                dest, destCapacity, src, src_দৈর্ঘ্য, edits, errorCode);
    ফেরত u_terminateUChars(dest, destCapacity, dest_দৈর্ঘ্য, &errorCode);
}

U_CFUNC int32_t
ustrcase_mapWithOverlap(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_PARAM
                        char16_t *dest, int32_t destCapacity,
                        ধ্রুবক char16_t *src, int32_t src_দৈর্ঘ্য,
                        UStringCaseMapper *stringCaseMapper,
                        UErrorCode &errorCode) {
    char16_t বাফার[300];
    char16_t *অস্থায়ী;

    int32_t dest_দৈর্ঘ্য;

    /* check argument values */
    যদি(U_FAILURE(errorCode)) {
        ফেরত 0;
    }
    যদি( destCapacity<0 ||
        (dest==nullptr && destCapacity>0) ||
        src==nullptr ||
        src_দৈর্ঘ্য<-1
    ) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত 0;
    }

    /* get the string length */
    যদি(src_দৈর্ঘ্য==-1) {
        src_দৈর্ঘ্য=u_strlen(src);
    }

    /* check for overlapping source and destination */
    যদি( dest!=nullptr &&
        ((src>=dest && src<(dest+destCapacity)) ||
         (dest>=src && dest<(src+src_দৈর্ঘ্য)))
    ) {
        /* overlap: provide a temporary destination buffer and later copy the result */
        যদি(destCapacity<=UPRV_LENGTHOF(বাফার)) {
            /* the stack buffer is large enough */
            অস্থায়ী=বাফার;
        } অন্যথা {
            /* allocate a buffer */
            অস্থায়ী=(char16_t *)uprv_malloc(destCapacity*U_SIZEOF_UCHAR);
            যদি(অস্থায়ী==nullptr) {
                errorCode=U_MEMORY_ALLOCATION_ERROR;
                ফেরত 0;
            }
        }
    } অন্যথা {
        অস্থায়ী=dest;
    }

    dest_দৈর্ঘ্য=stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                                অস্থায়ী, destCapacity, src, src_দৈর্ঘ্য, nullptr, errorCode);
    যদি(অস্থায়ী!=dest) {
        /* copy the result string to the destination buffer */
        যদি (U_SUCCESS(errorCode) && 0 < dest_দৈর্ঘ্য && dest_দৈর্ঘ্য <= destCapacity) {
            u_memmove(dest, অস্থায়ী, dest_দৈর্ঘ্য);
        }
        যদি(অস্থায়ী!=বাফার) {
            uprv_free(অস্থায়ী);
        }
    }

    ফেরত u_terminateUChars(dest, destCapacity, dest_দৈর্ঘ্য, &errorCode);
}

/* public API functions */

U_CAPI int32_t U_EXPORT2
u_strFoldCase(char16_t *dest, int32_t destCapacity,
              ধ্রুবক char16_t *src, int32_t src_দৈর্ঘ্য,
              uint32_t options,
              UErrorCode *pErrorCode) {
    ফেরত ustrcase_mapWithOverlap(
        UCASE_LOC_মূল, options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, destCapacity,
        src, src_দৈর্ঘ্য,
        ustrcase_internalFold, *pErrorCode);
}

U_NAMESPACE_BEGIN

int32_t CaseMap::fold(
        uint32_t options,
        ধ্রুবক char16_t *src, int32_t src_দৈর্ঘ্য,
        char16_t *dest, int32_t destCapacity, Edits *edits,
        UErrorCode &errorCode) {
    ফেরত ustrcase_map(
        UCASE_LOC_মূল, options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, destCapacity,
        src, src_দৈর্ঘ্য,
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
কাঠামো CmpEquivLevel {
    ধ্রুবক char16_t *start, *s, *limit;
};
প্রকারলেখ কাঠামো CmpEquivLevel CmpEquivLevel;

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
স্থির int32_t _cmpFold(
            ধ্রুবক char16_t *s1, int32_t length1,
            ধ্রুবক char16_t *s2, int32_t length2,
            uint32_t options,
            int32_t *matchLen1, int32_t *matchLen2,
            UErrorCode *pErrorCode) {
    int32_t cmpRes = 0;

    /* current-level start/limit - s1/s2 as current */
    ধ্রুবক char16_t *start1, *start2, *limit1, *limit2;

    /* points to the original start address */
    ধ্রুবক char16_t *org1, *org2;

    /* points to the end of match + 1 */
    ধ্রুবক char16_t *m1, *m2;

    /* case folding variables */
    ধ্রুবক char16_t *p;
    int32_t length;

    /* stacks of previous-level start/current/limit */
    CmpEquivLevel stack1[2], stack2[2];

    /* case folding buffers, only use current-level start/limit */
    char16_t fold1[UCASE_MAX_STRING_দৈর্ঘ্য+1], fold2[UCASE_MAX_STRING_দৈর্ঘ্য+1];

    /* track which is the current level per string */
    int32_t level1, level2;

    /* current code units, and code points for lookups */
    UChar32 c1, c2, cp1, cp2;

    /* no argument error checking because this itself is not an API */

    /*
     * assume that at least the option U_COMPARE_IGNORE_CASE is set
     * otherwise this function would have to behave exactly as uprv_strCompare()
     */
    যদি(U_FAILURE(*pErrorCode)) {
        ফেরত 0;
    }

    /* initialize */
    যদি(matchLen1) {
        U_ASSERT(matchLen2 !=nullptr);
        *matchLen1=0;
        *matchLen2=0;
    }

    start1=m1=org1=s1;
    যদি(length1==-1) {
        limit1=nullptr;
    } অন্যথা {
        limit1=s1+length1;
    }

    start2=m2=org2=s2;
    যদি(length2==-1) {
        limit2=nullptr;
    } অন্যথা {
        limit2=s2+length2;
    }

    level1=level2=0;
    c1=c2=-1;

    /* comparison loop */
    জন্য(;;) {
        /*
         * here a code unit value of -1 means "get another code unit"
         * below it will mean "this source is finished"
         */

        যদি(c1<0) {
            /* get next code unit from string 1, post-increment */
            জন্য(;;) {
                যদি(s1==limit1 || ((c1=*s1)==0 && (limit1==nullptr || (options&_STRNCMP_STYLE)))) {
                    যদি(level1==0) {
                        c1=-1;
                        বিরতি;
                    }
                } অন্যথা {
                    ++s1;
                    বিরতি;
                }

                /* reached end of level buffer, pop one level */
                করো {
                    --level1;
                    start1=stack1[level1].start;    /*Not uninitialized*/
                } যখন(start1==nullptr);
                s1=stack1[level1].s;                /*Not uninitialized*/
                limit1=stack1[level1].limit;        /*Not uninitialized*/
            }
        }

        যদি(c2<0) {
            /* get next code unit from string 2, post-increment */
            জন্য(;;) {
                যদি(s2==limit2 || ((c2=*s2)==0 && (limit2==nullptr || (options&_STRNCMP_STYLE)))) {
                    যদি(level2==0) {
                        c2=-1;
                        বিরতি;
                    }
                } অন্যথা {
                    ++s2;
                    বিরতি;
                }

                /* reached end of level buffer, pop one level */
                করো {
                    --level2;
                    start2=stack2[level2].start;    /*Not uninitialized*/
                } যখন(start2==nullptr);
                s2=stack2[level2].s;                /*Not uninitialized*/
                limit2=stack2[level2].limit;        /*Not uninitialized*/
            }
        }

        /*
         * compare c1 and c2
         * either variable c1, c2 is -1 only if the corresponding string is finished
         */
        যদি(c1==c2) {
            ধ্রুবক char16_t *next1, *next2;

            যদি(c1<0) {
                cmpRes=0;   /* c1==c2==-1 indicating end of strings */
                বিরতি;
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
            যদি(level1==0) {
                next1=s1;
            } অন্যথা যদি(s1==limit1) {
                /* Note: This implementation only use a single level of stack.
                 *      If this code needs to be changed to use multiple levels
                 *      of stacks, the code above should check if the current
                 *      code is at the end of all stacks.
                 */
                U_ASSERT(level1==1);

                /* is s1 at the end of the current stack? */
                next1=stack1[0].s;
            }

            যদি (next1!=nullptr) {
                যদি(level2==0) {
                    next2=s2;
                } অন্যথা যদি(s2==limit2) {
                    U_ASSERT(level2==1);

                    /* is s2 at the end of the current stack? */
                    next2=stack2[0].s;
                }
                যদি(next2!=nullptr) {
                    m1=next1;
                    m2=next2;
                }
            }
            c1=c2=-1;       /* make us fetch new code units */
            চালু;
        } অন্যথা যদি(c1<0) {
            cmpRes=-1;      /* string 1 ends before string 2 */
            বিরতি;
        } অন্যথা যদি(c2<0) {
            cmpRes=1;       /* string 2 ends before string 1 */
            বিরতি;
        }
        /* c1!=c2 && c1>=0 && c2>=0 */

        /* get complete code points for c1, c2 for lookups if either is a surrogate */
        cp1=c1;
        যদি(U_IS_SURROGATE(c1)) {
            char16_t c;

            যদি(U_IS_SURROGATE_LEAD(c1)) {
                যদি(s1!=limit1 && U16_IS_TRAIL(c=*s1)) {
                    /* advance ++s1; only below if cp1 decomposes/case-folds */
                    cp1=U16_GET_SUPPLEMENTARY(c1, c);
                }
            } অন্যথা /* isTrail(c1) */ {
                যদি(start1<=(s1-2) && U16_IS_LEAD(c=*(s1-2))) {
                    cp1=U16_GET_SUPPLEMENTARY(c, c1);
                }
            }
        }

        cp2=c2;
        যদি(U_IS_SURROGATE(c2)) {
            char16_t c;

            যদি(U_IS_SURROGATE_LEAD(c2)) {
                যদি(s2!=limit2 && U16_IS_TRAIL(c=*s2)) {
                    /* advance ++s2; only below if cp2 decomposes/case-folds */
                    cp2=U16_GET_SUPPLEMENTARY(c2, c);
                }
            } অন্যথা /* isTrail(c2) */ {
                যদি(start2<=(s2-2) && U16_IS_LEAD(c=*(s2-2))) {
                    cp2=U16_GET_SUPPLEMENTARY(c, c2);
                }
            }
        }

        /*
         * go down one level for each string
         * continue with the main loop as soon as there is a real change
         */

        যদি( level1==0 &&
            (length = ucase_toFullFolding(cp1, &p, options)) >= 0
        ) {
            /* cp1 case-folds to the code point "length" or to p[length] */
            যদি(U_IS_SURROGATE(c1)) {
                যদি(U_IS_SURROGATE_LEAD(c1)) {
                    /* advance beyond source surrogate pair if it case-folds */
                    ++s1;
                } অন্যথা /* isTrail(c1) */ {
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
            যদি(length<=UCASE_MAX_STRING_দৈর্ঘ্য) {
                u_memcpy(fold1, p, length);
            } অন্যথা {
                int32_t i=0;
                U16_APPEND_UNSAFE(fold1, i, length);
                length=i;
            }

            /* set next level pointers to case folding */
            start1=s1=fold1;
            limit1=fold1+length;

            /* get ready to read from decomposition, continue with loop */
            c1=-1;
            চালু;
        }

        যদি( level2==0 &&
            (length = ucase_toFullFolding(cp2, &p, options)) >= 0
        ) {
            /* cp2 case-folds to the code point "length" or to p[length] */
            যদি(U_IS_SURROGATE(c2)) {
                যদি(U_IS_SURROGATE_LEAD(c2)) {
                    /* advance beyond source surrogate pair if it case-folds */
                    ++s2;
                } অন্যথা /* isTrail(c2) */ {
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
            যদি(length<=UCASE_MAX_STRING_দৈর্ঘ্য) {
                u_memcpy(fold2, p, length);
            } অন্যথা {
                int32_t i=0;
                U16_APPEND_UNSAFE(fold2, i, length);
                length=i;
            }

            /* set next level pointers to case folding */
            start2=s2=fold2;
            limit2=fold2+length;

            /* get ready to read from decomposition, continue with loop */
            c2=-1;
            চালু;
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

        যদি(c1>=0xd800 && c2>=0xd800 && (options&U_COMPARE_CODE_POINT_ORDER)) {
            /* subtract 0x2800 from BMP code points to make them smaller than supplementary ones */
            যদি(
                (c1<=0xdbff && s1!=limit1 && U16_IS_TRAIL(*s1)) ||
                (U16_IS_TRAIL(c1) && start1!=(s1-1) && U16_IS_LEAD(*(s1-2)))
            ) {
                /* part of a surrogate pair, leave >=d800 */
            } অন্যথা {
                /* BMP code point - may be surrogate code point - make <d800 */
                c1-=0x2800;
            }

            যদি(
                (c2<=0xdbff && s2!=limit2 && U16_IS_TRAIL(*s2)) ||
                (U16_IS_TRAIL(c2) && start2!=(s2-1) && U16_IS_LEAD(*(s2-2)))
            ) {
                /* part of a surrogate pair, leave >=d800 */
            } অন্যথা {
                /* BMP code point - may be surrogate code point - make <d800 */
                c2-=0x2800;
            }
        }

        cmpRes=c1-c2;
        বিরতি;
    }

    যদি(matchLen1) {
        *matchLen1=static_cast<int32_t>(m1-org1);
        *matchLen2=static_cast<int32_t>(m2-org2);
    }
    ফেরত cmpRes;
}

/* internal function */
U_CFUNC int32_t
u_strcmpFold(ধ্রুবক char16_t *s1, int32_t length1,
             ধ্রুবক char16_t *s2, int32_t length2,
             uint32_t options,
             UErrorCode *pErrorCode) {
    ফেরত _cmpFold(s1, length1, s2, length2, options, nullptr, nullptr, pErrorCode);
}

/* public API functions */

U_CAPI int32_t U_EXPORT2
u_strCaseCompare(ধ্রুবক char16_t *s1, int32_t length1,
                 ধ্রুবক char16_t *s2, int32_t length2,
                 uint32_t options,
                 UErrorCode *pErrorCode) {
    /* argument checking */
    যদি (pErrorCode == nullptr || U_FAILURE(*pErrorCode)) {
        ফেরত 0;
    }
    যদি(s1==nullptr || length1<-1 || s2==nullptr || length2<-1) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত 0;
    }
    ফেরত u_strcmpFold(s1, length1, s2, length2,
                        options|U_COMPARE_IGNORE_CASE,
                        pErrorCode);
}

U_CAPI int32_t U_EXPORT2
u_strcasecmp(ধ্রুবক char16_t *s1, ধ্রুবক char16_t *s2, uint32_t options) {
    UErrorCode errorCode=U_ZERO_ERROR;
    ফেরত u_strcmpFold(s1, -1, s2, -1,
                        options|U_COMPARE_IGNORE_CASE,
                        &errorCode);
}

U_CAPI int32_t U_EXPORT2
u_memcasecmp(ধ্রুবক char16_t *s1, ধ্রুবক char16_t *s2, int32_t length, uint32_t options) {
    UErrorCode errorCode=U_ZERO_ERROR;
    ফেরত u_strcmpFold(s1, length, s2, length,
                        options|U_COMPARE_IGNORE_CASE,
                        &errorCode);
}

U_CAPI int32_t U_EXPORT2
u_strncasecmp(ধ্রুবক char16_t *s1, ধ্রুবক char16_t *s2, int32_t n, uint32_t options) {
    UErrorCode errorCode=U_ZERO_ERROR;
    ফেরত u_strcmpFold(s1, n, s2, n,
                        options|(U_COMPARE_IGNORE_CASE|_STRNCMP_STYLE),
                        &errorCode);
}

/* internal API - detect length of shared prefix */
U_CAPI শূন্য
u_caseInsensitivePrefixMatch(ধ্রুবক char16_t *s1, int32_t length1,
                             ধ্রুবক char16_t *s2, int32_t length2,
                             uint32_t options,
                             int32_t *matchLen1, int32_t *matchLen2,
                             UErrorCode *pErrorCode) {
    _cmpFold(s1, length1, s2, length2, options,
        matchLen1, matchLen2, pErrorCode);
}
