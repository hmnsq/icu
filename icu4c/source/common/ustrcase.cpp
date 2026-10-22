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

#شامل "unicode/utypes.h"
#شامل "unicode/brkiter.h"
#شامل "unicode/casemap.h"
#شامل "unicode/edits.h"
#شامل "unicode/stringoptions.h"
#شامل "unicode/ustring.h"
#شامل "unicode/ucasemap.h"
#شامل "unicode/ubrk.h"
#شامل "unicode/utf.h"
#شامل "unicode/utf16.h"
#شامل "cmemory.h"
#شامل "ucase.h"
#شامل "ucasemap_imp.h"
#شامل "ustr_imp.h"
#شامل "uassert.h"

/**
 * Code point for COMBINING ACUTE ACCENT
 * @internal
 */
#تعریف_کرو ACUTE u'\u0301'

U_NAMESPACE_BEGIN

نامخلا {

int32_t check_حد_سے_تجاوز_And_Edits_Error(int32_t dest_اشاریہ, int32_t dest_استعداد,
                                   Edits *edits, UError_رمز &error_رمز) {
    اگر (U_SUCCESS(error_رمز)) {
        اگر (dest_اشاریہ > dest_استعداد) {
            error_رمز = U_بفر_حد_سے_تجاوز_ERROR;
        } ورنہ اگر (edits != خالی_اشاریہ) {
            edits->copy_Error_مقصد(error_رمز);
        }
    }
    واپس dest_اشاریہ;
}

/* Appends a full case mapping result, see UCASE_MAX_STRING_LENGTH. */
اندرونی int32_t
append_نتیجہ(char16_t *dest, int32_t dest_اشاریہ, int32_t dest_استعداد,
             int32_t نتیجہ, ثابت char16_t *s,
             int32_t cp_لمبائی, uint32_t options, icu::Edits *edits) {
    UChar32 c;
    int32_t length;

    /* decode the result */
    اگر(نتیجہ<0) {
        /* (not) original code point */
        اگر(edits!=خالی_اشاریہ) {
            edits->جوڑو_Unchanged(cp_لمبائی);
        }
        اگر(options & U_OMIT_UNCHANGED_متن) {
            واپس dest_اشاریہ;
        }
        c=~نتیجہ;
        اگر(dest_اشاریہ<dest_استعداد && c<=0xffff) {  // BMP slightly-fastpath
            dest[dest_اشاریہ++] = static_cast<char16_t>(c);
            واپس dest_اشاریہ;
        }
        length=cp_لمبائی;
    } ورنہ {
        اگر(نتیجہ<=UCASE_MAX_STRING_لمبائی) {
            c=U_SENTINEL;
            length=نتیجہ;
        } ورنہ اگر(dest_اشاریہ<dest_استعداد && نتیجہ<=0xffff) {  // BMP slightly-fastpath
            dest[dest_اشاریہ++] = static_cast<char16_t>(نتیجہ);
            اگر(edits!=خالی_اشاریہ) {
                edits->جوڑو_Replace(cp_لمبائی, 1);
            }
            واپس dest_اشاریہ;
        } ورنہ {
            c=نتیجہ;
            length=U16_لمبائی(c);
        }
        اگر(edits!=خالی_اشاریہ) {
            edits->جوڑو_Replace(cp_لمبائی, length);
        }
    }
    اگر(length>(INT32_MAX-dest_اشاریہ)) {
        واپس -1;  // integer overflow
    }

    اگر(dest_اشاریہ<dest_استعداد) {
        /* append the result */
        اگر(c>=0) {
            /* code point */
            UBool isError=جھوٹ;
            U16_APPEND(dest, dest_اشاریہ, dest_استعداد, c, isError);
            اگر(isError) {
                /* overflow, nothing written */
                dest_اشاریہ+=length;
            }
        } ورنہ {
            /* string */
            اگر((dest_اشاریہ+length)<=dest_استعداد) {
                جب(length>0) {
                    dest[dest_اشاریہ++]=*s++;
                    --length;
                }
            } ورنہ {
                /*حد_سے_تجاوز*/
                dest_اشاریہ+=length;
            }
        }
    } ورنہ {
        /* preflight */
        dest_اشاریہ+=length;
    }
    واپس dest_اشاریہ;
}

اندرونی int32_t
appendUChar(char16_t *dest, int32_t dest_اشاریہ, int32_t dest_استعداد, char16_t c) {
    اگر(dest_اشاریہ<dest_استعداد) {
        dest[dest_اشاریہ]=c;
    } ورنہ اگر(dest_اشاریہ==INT32_MAX) {
        واپس -1;  // integer overflow
    }
    واپس dest_اشاریہ+1;
}

int32_t
append_Non_خالی_خانے_Unchanged(char16_t *dest, int32_t dest_اشاریہ, int32_t dest_استعداد,
                        ثابت char16_t *s, int32_t length, uint32_t options, icu::Edits *edits) {
    اگر(edits!=خالی_اشاریہ) {
        edits->جوڑو_Unchanged(length);
    }
    اگر(options & U_OMIT_UNCHANGED_متن) {
        واپس dest_اشاریہ;
    }
    اگر(length>(INT32_MAX-dest_اشاریہ)) {
        واپس -1;  // integer overflow
    }
    اگر((dest_اشاریہ+length)<=dest_استعداد) {
        u_memcpy(dest+dest_اشاریہ, s, length);
    }
    واپس dest_اشاریہ + length;
}

اندرونی int32_t
appendUnchanged(char16_t *dest, int32_t dest_اشاریہ, int32_t dest_استعداد,
                ثابت char16_t *s, int32_t length, uint32_t options, icu::Edits *edits) {
    اگر (length <= 0) {
        واپس dest_اشاریہ;
    }
    واپس append_Non_خالی_خانے_Unchanged(dest, dest_اشاریہ, dest_استعداد, s, length, options, edits);
}

UChar32 U_CALLCONV
utf16_caseContextIterator(خالی *context, int8_t dir) {
    UCaseContext* csc = static_cast<UCaseContext*>(context);
    UChar32 c;

    اگر(dir<0) {
        /* reset for backward iteration */
        csc->اشاریہ=csc->cp_آغاز;
        csc->dir=dir;
    } ورنہ اگر(dir>0) {
        /* reset for forward iteration */
        csc->اشاریہ=csc->cp_حد_بندی;
        csc->dir=dir;
    } ورنہ {
        /* continue current iteration direction */
        dir=csc->dir;
    }

    اگر(dir<0) {
        اگر(csc->آغاز<csc->اشاریہ) {
            U16_پچھلا((ثابت char16_t *)csc->p, csc->آغاز, csc->اشاریہ, c);
            واپس c;
        }
    } ورنہ {
        اگر(csc->اشاریہ<csc->حد_بندی) {
            U16_اگلا((ثابت char16_t *)csc->p, csc->اشاریہ, csc->حد_بندی, c);
            واپس c;
        }
    }
    واپس U_SENTINEL;
}

/**
 * caseLocale >= 0: Lowercases [srcStart..srcLimit[ but takes context [0..srcLength[ into account.
 * caseLocale < 0: Case-folds [srcStart..srcLimit[.
 */
int32_t مقصد_زیریں(int32_t caseLocale, uint32_t options,
                char16_t *dest, int32_t dest_استعداد,
                ثابت char16_t *src, UCaseContext *csc, int32_t src_آغاز, int32_t src_حد_بندی,
                icu::Edits *edits, UError_رمز &error_رمز) {
    ثابت int8_t *latin_مقصد_زیریں;
    اگر (caseLocale == UCASE_LOC_جڑ ||
            (caseLocale >= 0 ?
                !(caseLocale == UCASE_LOC_TURKISH || caseLocale == UCASE_LOC_LITHUANIAN) :
                (options & FOLD_CASE_OPTIONS_نقاب) == U_FOLD_CASE_DEFAULT)) {
        latin_مقصد_زیریں = LatinCase::مقصد_زیریں_معمول;
    } ورنہ {
        latin_مقصد_زیریں = LatinCase::مقصد_زیریں_TR_LT;
    }
    ثابت UTrie2 *trie = ucase_getTrie();
    int32_t dest_اشاریہ = 0;
    int32_t پچھلا = src_آغاز;
    int32_t src_اشاریہ = src_آغاز;
    برائے (;;) {
        // fast path for simple cases
        char16_t lead = 0;
        جب (src_اشاریہ < src_حد_بندی) {
            lead = src[src_اشاریہ];
            int32_t delta;
            اگر (lead < LatinCase::LONG_S) {
                int8_t d = latin_مقصد_زیریں[lead];
                اگر (d == LatinCase::EXC) { توڑو; }
                ++src_اشاریہ;
                اگر (d == 0) { جاری; }
                delta = d;
            } ورنہ اگر (lead >= 0xd800) {
                توڑو;  // surrogate or higher
            } ورنہ {
                uint16_t props = UTRIE2_GET16_FROM_U16_SINGLE_LEAD(trie, lead);
                اگر (UCASE_HAS_EXCEPTION(props)) { توڑو; }
                ++src_اشاریہ;
                اگر (!UCASE_IS_بالائی_OR_TITLE(props) || (delta = UCASE_GET_DELTA(props)) == 0) {
                    جاری;
                }
            }
            lead += static_cast<char16_t>(delta);
            dest_اشاریہ = appendUnchanged(dest, dest_اشاریہ, dest_استعداد,
                                        src + پچھلا, src_اشاریہ - 1 - پچھلا, options, edits);
            اگر (dest_اشاریہ >= 0) {
                dest_اشاریہ = appendUChar(dest, dest_اشاریہ, dest_استعداد, lead);
                اگر (edits != خالی_اشاریہ) {
                    edits->جوڑو_Replace(1, 1);
                }
            }
            اگر (dest_اشاریہ < 0) {
                error_رمز = U_اشاریہ_OUTOFBOUNDS_ERROR;
                واپس 0;
            }
            پچھلا = src_اشاریہ;
        }
        اگر (src_اشاریہ >= src_حد_بندی) {
            توڑو;
        }
        // slow path
        int32_t cp_آغاز = src_اشاریہ++;
        char16_t trail;
        UChar32 c;
        اگر (U16_IS_LEAD(lead) && src_اشاریہ < src_حد_بندی && U16_IS_TRAIL(trail = src[src_اشاریہ])) {
            c = U16_GET_SUPPLEMENTARY(lead, trail);
            ++src_اشاریہ;
        } ورنہ {
            c = lead;
        }
        ثابت char16_t *s = خالی_اشاریہ;
        اگر (caseLocale >= 0) {
            csc->cp_آغاز = cp_آغاز;
            csc->cp_حد_بندی = src_اشاریہ;
            c = ucase_مقصد_بھرا_زیریں(c, utf16_caseContextIterator, csc, &s, caseLocale);
        } ورنہ {
            c = ucase_مقصد_بھرا_Folding(c, &s, options);
        }
        اگر (c >= 0) {
            dest_اشاریہ = appendUnchanged(dest, dest_اشاریہ, dest_استعداد,
                                        src + پچھلا, cp_آغاز - پچھلا, options, edits);
            اگر (dest_اشاریہ >= 0) {
                dest_اشاریہ = append_نتیجہ(dest, dest_اشاریہ, dest_استعداد, c, s,
                                         src_اشاریہ - cp_آغاز, options, edits);
            }
            اگر (dest_اشاریہ < 0) {
                error_رمز = U_اشاریہ_OUTOFBOUNDS_ERROR;
                واپس 0;
            }
            پچھلا = src_اشاریہ;
        }
    }
    dest_اشاریہ = appendUnchanged(dest, dest_اشاریہ, dest_استعداد,
                                src + پچھلا, src_اشاریہ - پچھلا, options, edits);
    اگر (dest_اشاریہ < 0) {
        error_رمز = U_اشاریہ_OUTOFBOUNDS_ERROR;
        واپس 0;
    }
    واپس dest_اشاریہ;
}

int32_t مقصد_بالائی(int32_t caseLocale, uint32_t options,
                char16_t *dest, int32_t dest_استعداد,
                ثابت char16_t *src, UCaseContext *csc, int32_t src_لمبائی,
                icu::Edits *edits, UError_رمز &error_رمز) {
    ثابت int8_t *latin_مقصد_بالائی;
    اگر (caseLocale == UCASE_LOC_TURKISH) {
        latin_مقصد_بالائی = LatinCase::مقصد_بالائی_TR;
    } ورنہ {
        latin_مقصد_بالائی = LatinCase::مقصد_بالائی_معمول;
    }
    ثابت UTrie2 *trie = ucase_getTrie();
    int32_t dest_اشاریہ = 0;
    int32_t پچھلا = 0;
    int32_t src_اشاریہ = 0;
    برائے (;;) {
        // fast path for simple cases
        char16_t lead = 0;
        جب (src_اشاریہ < src_لمبائی) {
            lead = src[src_اشاریہ];
            int32_t delta;
            اگر (lead < LatinCase::LONG_S) {
                int8_t d = latin_مقصد_بالائی[lead];
                اگر (d == LatinCase::EXC) { توڑو; }
                ++src_اشاریہ;
                اگر (d == 0) { جاری; }
                delta = d;
            } ورنہ اگر (lead >= 0xd800) {
                توڑو;  // surrogate or higher
            } ورنہ {
                uint16_t props = UTRIE2_GET16_FROM_U16_SINGLE_LEAD(trie, lead);
                اگر (UCASE_HAS_EXCEPTION(props)) { توڑو; }
                ++src_اشاریہ;
                اگر (UCASE_GET_TYPE(props) != UCASE_زیریں || (delta = UCASE_GET_DELTA(props)) == 0) {
                    جاری;
                }
            }
            lead += static_cast<char16_t>(delta);
            dest_اشاریہ = appendUnchanged(dest, dest_اشاریہ, dest_استعداد,
                                        src + پچھلا, src_اشاریہ - 1 - پچھلا, options, edits);
            اگر (dest_اشاریہ >= 0) {
                dest_اشاریہ = appendUChar(dest, dest_اشاریہ, dest_استعداد, lead);
                اگر (edits != خالی_اشاریہ) {
                    edits->جوڑو_Replace(1, 1);
                }
            }
            اگر (dest_اشاریہ < 0) {
                error_رمز = U_اشاریہ_OUTOFBOUNDS_ERROR;
                واپس 0;
            }
            پچھلا = src_اشاریہ;
        }
        اگر (src_اشاریہ >= src_لمبائی) {
            توڑو;
        }
        // slow path
        int32_t cp_آغاز;
        csc->cp_آغاز = cp_آغاز = src_اشاریہ++;
        char16_t trail;
        UChar32 c;
        اگر (U16_IS_LEAD(lead) && src_اشاریہ < src_لمبائی && U16_IS_TRAIL(trail = src[src_اشاریہ])) {
            c = U16_GET_SUPPLEMENTARY(lead, trail);
            ++src_اشاریہ;
        } ورنہ {
            c = lead;
        }
        csc->cp_حد_بندی = src_اشاریہ;
        ثابت char16_t *s = خالی_اشاریہ;
        c = ucase_مقصد_بھرا_بالائی(c, utf16_caseContextIterator, csc, &s, caseLocale);
        اگر (c >= 0) {
            dest_اشاریہ = appendUnchanged(dest, dest_اشاریہ, dest_استعداد,
                                        src + پچھلا, cp_آغاز - پچھلا, options, edits);
            اگر (dest_اشاریہ >= 0) {
                dest_اشاریہ = append_نتیجہ(dest, dest_اشاریہ, dest_استعداد, c, s,
                                         src_اشاریہ - cp_آغاز, options, edits);
            }
            اگر (dest_اشاریہ < 0) {
                error_رمز = U_اشاریہ_OUTOFBOUNDS_ERROR;
                واپس 0;
            }
            پچھلا = src_اشاریہ;
        }
    }
    dest_اشاریہ = appendUnchanged(dest, dest_اشاریہ, dest_استعداد,
                                src + پچھلا, src_اشاریہ - پچھلا, options, edits);
    اگر (dest_اشاریہ < 0) {
        error_رمز = U_اشاریہ_OUTOFBOUNDS_ERROR;
        واپس 0;
    }
    واپس dest_اشاریہ;
}

}  // namespace

U_NAMESPACE_انتہا

U_NAMESPACE_USE

#اگر !UCONFIG_NO_BREAK_ITERATION

نامخلا {

/**
 * Input: c is a letter I with or without acute accent.
 * start is the index in src after c, and is less than segmentLimit.
 * If a plain i/I is followed by a plain j/J,
 * or an i/I with acute (precomposed or decomposed) is followed by a j/J with acute,
 * then we output accordingly.
 *
 * @return the src index after the titlecased sequence, or the start index if no Dutch IJ
 */
int32_t maybeTitleDutchIJ(ثابت char16_t *src, UChar32 c, int32_t آغاز, int32_t segment_حد_بندی,
                          char16_t *dest, int32_t &dest_اشاریہ, int32_t dest_استعداد, uint32_t options,
                          icu::Edits *edits) {
    U_ASSERT(آغاز < segment_حد_بندی);

    int32_t اشاریہ = آغاز;
    منطقی withAcute = جھوٹ;

    // If the conditions are met, then the following variables tell us what to output.
    int32_t unchanged1 = 0;  // code units before the j, or the whole sequence (0..3)
    منطقی doTitleJ = جھوٹ;  // true if the j needs to be titlecased
    int32_t unchanged2 = 0;  // after the j (0 or 1)

    // next character after the first letter
    char16_t c2 = src[اشاریہ++];

    // Is the first letter an i/I with accent?
    اگر (c == u'I') {
        اگر (c2 == ACUTE) {
            withAcute = سچ;
            unchanged1 = 1;
            اگر (اشاریہ == segment_حد_بندی) { واپس آغاز; }
            c2 = src[اشاریہ++];
        }
    } ورنہ {  // Í
        withAcute = سچ;
    }

    // Is the next character a j/J?
    اگر (c2 == u'j') {
        doTitleJ = سچ;
    } ورنہ اگر (c2 == u'J') {
        ++unchanged1;
    } ورنہ {
        واپس آغاز;
    }

    // A plain i/I must be followed by a plain j/J.
    // An i/I with acute must be followed by a j/J with acute.
    اگر (withAcute) {
        اگر (اشاریہ == segment_حد_بندی || src[اشاریہ++] != ACUTE) { واپس آغاز; }
        اگر (doTitleJ) {
            unchanged2 = 1;
        } ورنہ {
            ++unchanged1;
        }
    }

    // There must not be another combining mark.
    اگر (اشاریہ < segment_حد_بندی) {
        int32_t cp;
        int32_t i = اشاریہ;
        U16_اگلا(src, i, segment_حد_بندی, cp);
        uint32_t type_نقاب = U_GET_GC_نقاب(cp);
        اگر ((type_نقاب & U_GC_M_نقاب) != 0) {
            واپس آغاز;
        }
    }

    // Output the rest of the Dutch IJ.
    dest_اشاریہ = appendUnchanged(dest, dest_اشاریہ, dest_استعداد, src + آغاز, unchanged1, options, edits);
    آغاز += unchanged1;
    اگر (doTitleJ) {
        dest_اشاریہ = appendUChar(dest, dest_اشاریہ, dest_استعداد, u'J');
        اگر (edits != خالی_اشاریہ) {
            edits->جوڑو_Replace(1, 1);
        }
        ++آغاز;
    }
    dest_اشاریہ = appendUnchanged(dest, dest_اشاریہ, dest_استعداد, src + آغاز, unchanged2, options, edits);

    U_ASSERT(آغاز + unchanged2 == اشاریہ);
    واپس اشاریہ;
}

}  // namespace

U_CFUNC int32_t U_CALLCONV
ustrcase_اندرونی_مقصد_Title(int32_t caseLocale, uint32_t options, BreakIterator *iter,
                         char16_t *dest, int32_t dest_استعداد,
                         ثابت char16_t *src, int32_t src_لمبائی,
                         icu::Edits *edits,
                         UError_رمز &error_رمز) {
    اگر (!ustrcase_checkTitleAdjustmentOptions(options, error_رمز)) {
        واپس 0;
    }

    /* set up local variables */
    UCaseContext csc=UCASECONTEXT_INITIALIZER;
    csc.p=(خالی *)src;
    csc.حد_بندی=src_لمبائی;
    int32_t dest_اشاریہ=0;
    int32_t پچھلا=0;
    منطقی is_پہلا_اشاریہ=سچ;

    /* titlecasing loop */
    جب(پچھلا<src_لمبائی) {
        /* find next index where to titlecase */
        int32_t اشاریہ;
        اگر(is_پہلا_اشاریہ) {
            is_پہلا_اشاریہ=جھوٹ;
            اشاریہ=iter->first();
        } ورنہ {
            اشاریہ=iter->next();
        }
        اگر(اشاریہ==UBRK_مکمل || اشاریہ>src_لمبائی) {
            اشاریہ=src_لمبائی;
        }

        /*
         * Segment [prev..index[ into 3 parts:
         * a) skipped characters (copy as-is) [prev..titleStart[
         * b) first letter (titlecase)              [titleStart..titleLimit[
         * c) subsequent characters (lowercase)                 [titleLimit..index[
         */
        اگر(پچھلا<اشاریہ) {
            // Find and copy skipped characters [prev..titleStart[
            int32_t title_آغاز=پچھلا;
            int32_t title_حد_بندی=پچھلا;
            UChar32 c;
            U16_اگلا(src, title_حد_بندی, اشاریہ, c);
            اگر ((options&U_TITLECASE_NO_BREAK_ADJUSTMENT)==0) {
                // Adjust the titlecasing index to the next cased character,
                // or to the next letter/number/symbol/private use.
                // Stop with titleStart<titleLimit<=index
                // if there is a character to be titlecased,
                // or else stop with titleStart==titleLimit==index.
                منطقی مقصد_Cased = (options&U_TITLECASE_ADJUST_مقصد_CASED) != 0;
                جب (مقصد_Cased ? UCASE_NONE==ucase_getType(c) : !ustrcase_isLNS(c)) {
                    title_آغاز=title_حد_بندی;
                    اگر(title_حد_بندی==اشاریہ) {
                        توڑو;
                    }
                    U16_اگلا(src, title_حد_بندی, اشاریہ, c);
                }
                اگر (پچھلا < title_آغاز) {
                    dest_اشاریہ=appendUnchanged(dest, dest_اشاریہ, dest_استعداد,
                                              src+پچھلا, title_آغاز-پچھلا, options, edits);
                    اگر(dest_اشاریہ<0) {
                        error_رمز=U_اشاریہ_OUTOFBOUNDS_ERROR;
                        واپس 0;
                    }
                }
            }

            اگر(title_آغاز<title_حد_بندی) {
                /* titlecase c which is from [titleStart..titleLimit[ */
                csc.cp_آغاز=title_آغاز;
                csc.cp_حد_بندی=title_حد_بندی;
                ثابت char16_t *s;
                c=ucase_مقصد_بھرا_Title(c, utf16_caseContextIterator, &csc, &s, caseLocale);
                dest_اشاریہ=append_نتیجہ(dest, dest_اشاریہ, dest_استعداد, c, s,
                                       title_حد_بندی-title_آغاز, options, edits);
                اگر(dest_اشاریہ<0) {
                    error_رمز=U_اشاریہ_OUTOFBOUNDS_ERROR;
                    واپس 0;
                }

                /* Special case Dutch IJ titlecasing */
                اگر (title_آغاز+1 < اشاریہ &&
                        caseLocale == UCASE_LOC_DUTCH) {
                    اگر (c < 0) {
                        c = ~c;
                    }

                    اگر (c == u'I' || c == u'Í') {
                        title_حد_بندی = maybeTitleDutchIJ(src, c, title_آغاز + 1, اشاریہ,
                                                       dest, dest_اشاریہ, dest_استعداد, options,
                                                       edits);
                    }
                }

                /* lowercase [titleLimit..index[ */
                اگر(title_حد_بندی<اشاریہ) {
                    اگر((options&U_TITLECASE_NO_LOWERCASE)==0) {
                        /* Normal operation: Lowercase the rest of the word. */
                        dest_اشاریہ+=
                            مقصد_زیریں(
                                caseLocale, options,
                                (dest==خالی_اشاریہ) ? خالی_اشاریہ: dest+dest_اشاریہ, dest_استعداد-dest_اشاریہ,
                                src, &csc, title_حد_بندی, اشاریہ,
                                edits, error_رمز);
                        اگر(error_رمز==U_بفر_حد_سے_تجاوز_ERROR) {
                            error_رمز=U_ZERO_ERROR;
                        }
                        اگر(U_FAILURE(error_رمز)) {
                            واپس dest_اشاریہ;
                        }
                    } ورنہ {
                        /* Optionally just copy the rest of the word unchanged. */
                        dest_اشاریہ=appendUnchanged(dest, dest_اشاریہ, dest_استعداد,
                                                  src+title_حد_بندی, اشاریہ-title_حد_بندی, options, edits);
                        اگر(dest_اشاریہ<0) {
                            error_رمز=U_اشاریہ_OUTOFBOUNDS_ERROR;
                            واپس 0;
                        }
                    }
                }
            }
        }

        پچھلا=اشاریہ;
    }

    واپس check_حد_سے_تجاوز_And_Edits_Error(dest_اشاریہ, dest_استعداد, edits, error_رمز);
}

#اگر_ختم  // !UCONFIG_NO_BREAK_ITERATION

U_NAMESPACE_BEGIN
نامخلا Greek_بالائی {

// Data generated by prototype code, see
// https://icu.unicode.org/design/case/greek-upper
// TODO: Move this data into ucase.icu.
ساکن ثابت uint16_t data0370[] = {
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

ساکن ثابت uint16_t data1F00[] = {
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
ساکن ثابت uint16_t data2126 = 0x03A9 | HAS_VOWEL;

uint32_t get_Letter_ڈیٹا(UChar32 c) {
    اگر (c < 0x370 || 0x2126 < c || (0x3ff < c && c < 0x1f00)) {
        واپس 0;
    } ورنہ اگر (c <= 0x3ff) {
        واپس data0370[c - 0x370];
    } ورنہ اگر (c <= 0x1fff) {
        واپس data1F00[c - 0x1f00];
    } ورنہ اگر (c == 0x2126) {
        واپس data2126;
    } ورنہ {
        واپس 0;
    }
}

uint32_t get_Diacritic_ڈیٹا(UChar32 c) {
    انتخاب (c) {
    حالت 0x0300:  // varia
    حالت 0x0301:  // tonos = oxia
    حالت 0x0342:  // perispomeni
    حالت 0x0302:  // circumflex can look like perispomeni
    حالت 0x0303:  // tilde can look like perispomeni
    حالت 0x0311:  // inverted breve can look like perispomeni
        واپس HAS_ACCENT;
    حالت 0x0308:  // dialytika = diaeresis
        واپس HAS_COMBINING_DIALYTIKA;
    حالت 0x0344:  // dialytika tonos
        واپس HAS_COMBINING_DIALYTIKA | HAS_ACCENT;
    حالت 0x0345:  // ypogegrammeni = iota subscript
        واپس HAS_YPOGEGRAMMENI;
    حالت 0x0304:  // macron
    حالت 0x0306:  // breve
    حالت 0x0313:  // comma above
    حالت 0x0314:  // reversed comma above
    حالت 0x0343:  // koronis
        واپس HAS_OTHER_GREEK_DIACRITIC;
    طےشدہ:
        واپس 0;
    }
}

UBool isFollowedByCasedLetter(ثابت char16_t *s, int32_t i, int32_t length) {
    جب (i < length) {
        UChar32 c;
        U16_اگلا(s, i, length, c);
        int32_t type = ucase_getTypeOrIgnorable(c);
        اگر ((type & UCASE_IGNORABLE) != 0) {
            // Case-ignorable, continue with the loop.
        } ورنہ اگر (type != UCASE_NONE) {
            واپس سچ;  // Followed by cased letter.
        } ورنہ {
            واپس جھوٹ;  // Uncased and not case-ignorable.
        }
    }
    واپس جھوٹ;  // Not followed by cased letter.
}

/**
 * Greek string uppercasing with a state machine.
 * Probably simpler than a stateless function that has to figure out complex context-before
 * for each character.
 * TODO: Try to re-consolidate one way or another with the non-Greek function.
 */
int32_t مقصد_بالائی(uint32_t options,
                char16_t *dest, int32_t dest_استعداد,
                ثابت char16_t *src, int32_t src_لمبائی,
                Edits *edits,
                UError_رمز &error_رمز) {
    int32_t dest_اشاریہ=0;
    uint32_t state = 0;
    برائے (int32_t i = 0; i < src_لمبائی;) {
        int32_t اگلا_اشاریہ = i;
        UChar32 c;
        U16_اگلا(src, اگلا_اشاریہ, src_لمبائی, c);
        uint32_t اگلا_State = 0;
        int32_t type = ucase_getTypeOrIgnorable(c);
        اگر ((type & UCASE_IGNORABLE) != 0) {
            // c is case-ignorable
            اگلا_State |= (state & AFTER_CASED);
        } ورنہ اگر (type != UCASE_NONE) {
            // c is cased
            اگلا_State |= AFTER_CASED;
        }
        uint32_t ڈیٹا = get_Letter_ڈیٹا(c);
        اگر (ڈیٹا > 0) {
            uint32_t upper = ڈیٹا & بالائی_نقاب;
            // Add a dialytika to this iota or ypsilon vowel
            // if we removed a tonos from the previous vowel,
            // and that previous vowel did not also have (or gain) a dialytika.
            // Adding one only to the final vowel in a longer sequence
            // (which does not occur in normal writing) would require lookahead.
            // Set the same flag as for preserving an existing dialytika.
            اگر ((ڈیٹا & HAS_VOWEL) != 0 &&
                (state & (AFTER_VOWEL_WITH_PRECOMPOSED_ACCENT | AFTER_VOWEL_WITH_COMBINING_ACCENT)) !=
                    0 &&
                (upper == 0x399 || upper == 0x3A5)) {
                ڈیٹا |= (state & AFTER_VOWEL_WITH_PRECOMPOSED_ACCENT) ? HAS_DIALYTIKA
                                                                      : HAS_COMBINING_DIALYTIKA;
            }
            int32_t عدد_Ypogegrammeni = 0;  // Map each one to a trailing, spacing, capital iota.
            اگر ((ڈیٹا & HAS_YPOGEGRAMMENI) != 0) {
                عدد_Ypogegrammeni = 1;
            }
            ثابت UBool hasPrecomposedAccent = (ڈیٹا & HAS_ACCENT) != 0;
            // Skip combining diacritics after this Greek letter.
            جب (اگلا_اشاریہ < src_لمبائی) {
                uint32_t diacritic_ڈیٹا = get_Diacritic_ڈیٹا(src[اگلا_اشاریہ]);
                اگر (diacritic_ڈیٹا != 0) {
                    ڈیٹا |= diacritic_ڈیٹا;
                    اگر ((diacritic_ڈیٹا & HAS_YPOGEGRAMMENI) != 0) {
                        ++عدد_Ypogegrammeni;
                    }
                    ++اگلا_اشاریہ;
                } ورنہ {
                    توڑو;  // not a Greek diacritic
                }
            }
            اگر ((ڈیٹا & HAS_VOWEL_AND_ACCENT_AND_DIALYTIKA) == HAS_VOWEL_AND_ACCENT) {
                اگلا_State |= hasPrecomposedAccent ? AFTER_VOWEL_WITH_PRECOMPOSED_ACCENT
                                                  : AFTER_VOWEL_WITH_COMBINING_ACCENT;
            }
            // Map according to Greek rules.
            UBool جوڑو_Tonos = جھوٹ;
            اگر (upper == 0x397 &&
                    (ڈیٹا & HAS_ACCENT) != 0 &&
                    عدد_Ypogegrammeni == 0 &&
                    (state & AFTER_CASED) == 0 &&
                    !isFollowedByCasedLetter(src, اگلا_اشاریہ, src_لمبائی)) {
                // Keep disjunctive "or" with (only) a tonos.
                // We use the same "word boundary" conditions as for the Final_Sigma test.
                اگر (hasPrecomposedAccent) {
                    upper = 0x389;  // Preserve the precomposed form.
                } ورنہ {
                    جوڑو_Tonos = سچ;
                }
            } ورنہ اگر ((ڈیٹا & HAS_DIALYTIKA) != 0) {
                // Preserve a vowel with dialytika in precomposed form if it exists.
                اگر (upper == 0x399) {
                    upper = 0x3AA;
                    ڈیٹا &= ~HAS_EITHER_DIALYTIKA;
                } ورنہ اگر (upper == 0x3A5) {
                    upper = 0x3AB;
                    ڈیٹا &= ~HAS_EITHER_DIALYTIKA;
                }
            }

            UBool change;
            اگر (edits == خالی_اشاریہ && (options & U_OMIT_UNCHANGED_متن) == 0) {
                change = سچ;  // common, simple usage
            } ورنہ {
                // Find out first whether we are changing the text.
                change = src[i] != upper || عدد_Ypogegrammeni > 0;
                int32_t i2 = i + 1;
                اگر ((ڈیٹا & HAS_EITHER_DIALYTIKA) != 0) {
                    change |= i2 >= اگلا_اشاریہ || src[i2] != 0x308;
                    ++i2;
                }
                اگر (جوڑو_Tonos) {
                    change |= i2 >= اگلا_اشاریہ || src[i2] != 0x301;
                    ++i2;
                }
                int32_t old_لمبائی = اگلا_اشاریہ - i;
                int32_t new_لمبائی = (i2 - i) + عدد_Ypogegrammeni;
                change |= old_لمبائی != new_لمبائی;
                اگر (change) {
                    اگر (edits != خالی_اشاریہ) {
                        edits->جوڑو_Replace(old_لمبائی, new_لمبائی);
                    }
                } ورنہ {
                    اگر (edits != خالی_اشاریہ) {
                        edits->جوڑو_Unchanged(old_لمبائی);
                    }
                    // Write unchanged text?
                    change = (options & U_OMIT_UNCHANGED_متن) == 0;
                }
            }

            اگر (change) {
                dest_اشاریہ = appendUChar(dest, dest_اشاریہ, dest_استعداد, static_cast<char16_t>(upper));
                اگر (dest_اشاریہ >= 0 && (ڈیٹا & HAS_EITHER_DIALYTIKA) != 0) {
                    dest_اشاریہ=appendUChar(dest, dest_اشاریہ, dest_استعداد, 0x308);  // restore or add a dialytika
                }
                اگر (dest_اشاریہ >= 0 && جوڑو_Tonos) {
                    dest_اشاریہ=appendUChar(dest, dest_اشاریہ, dest_استعداد, 0x301);
                }
                جب (dest_اشاریہ >= 0 && عدد_Ypogegrammeni > 0) {
                    dest_اشاریہ=appendUChar(dest, dest_اشاریہ, dest_استعداد, 0x399);
                    --عدد_Ypogegrammeni;
                }
                اگر(dest_اشاریہ<0) {
                    error_رمز=U_اشاریہ_OUTOFBOUNDS_ERROR;
                    واپس 0;
                }
            }
        } ورنہ {
            ثابت char16_t *s;
            c=ucase_مقصد_بھرا_بالائی(c, خالی_اشاریہ, خالی_اشاریہ, &s, UCASE_LOC_GREEK);
            dest_اشاریہ = append_نتیجہ(dest, dest_اشاریہ, dest_استعداد, c, s,
                                     اگلا_اشاریہ - i, options, edits);
            اگر (dest_اشاریہ < 0) {
                error_رمز = U_اشاریہ_OUTOFBOUNDS_ERROR;
                واپس 0;
            }
        }
        i = اگلا_اشاریہ;
        state = اگلا_State;
    }

    واپس dest_اشاریہ;
}

}  // namespace GreekUpper
U_NAMESPACE_انتہا

/* functions available in the common library (for unistr_case.cpp) */

U_CFUNC int32_t U_CALLCONV
ustrcase_اندرونی_مقصد_زیریں(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_غیر_مستعمل
                         char16_t *dest, int32_t dest_استعداد,
                         ثابت char16_t *src, int32_t src_لمبائی,
                         icu::Edits *edits,
                         UError_رمز &error_رمز) {
    UCaseContext csc=UCASECONTEXT_INITIALIZER;
    csc.p=(خالی *)src;
    csc.حد_بندی=src_لمبائی;
    int32_t dest_اشاریہ = مقصد_زیریں(
        caseLocale, options,
        dest, dest_استعداد,
        src, &csc, 0, src_لمبائی,
        edits, error_رمز);
    واپس check_حد_سے_تجاوز_And_Edits_Error(dest_اشاریہ, dest_استعداد, edits, error_رمز);
}

U_CFUNC int32_t U_CALLCONV
ustrcase_اندرونی_مقصد_بالائی(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_غیر_مستعمل
                         char16_t *dest, int32_t dest_استعداد,
                         ثابت char16_t *src, int32_t src_لمبائی,
                         icu::Edits *edits,
                         UError_رمز &error_رمز) {
    int32_t dest_اشاریہ;
    اگر (caseLocale == UCASE_LOC_GREEK) {
        dest_اشاریہ = Greek_بالائی::مقصد_بالائی(options, dest, dest_استعداد,
                                        src, src_لمبائی, edits, error_رمز);
    } ورنہ {
        UCaseContext csc=UCASECONTEXT_INITIALIZER;
        csc.p=(خالی *)src;
        csc.حد_بندی=src_لمبائی;
        dest_اشاریہ = مقصد_بالائی(
            caseLocale, options,
            dest, dest_استعداد,
            src, &csc, src_لمبائی,
            edits, error_رمز);
    }
    واپس check_حد_سے_تجاوز_And_Edits_Error(dest_اشاریہ, dest_استعداد, edits, error_رمز);
}

U_CFUNC int32_t U_CALLCONV
ustrcase_اندرونی_Fold(int32_t /* caseLocale */, uint32_t options, UCASEMAP_BREAK_ITERATOR_غیر_مستعمل
                      char16_t *dest, int32_t dest_استعداد,
                      ثابت char16_t *src, int32_t src_لمبائی,
                      icu::Edits *edits,
                      UError_رمز &error_رمز) {
    int32_t dest_اشاریہ = مقصد_زیریں(
        -1, options,
        dest, dest_استعداد,
        src, خالی_اشاریہ, 0, src_لمبائی,
        edits, error_رمز);
    واپس check_حد_سے_تجاوز_And_Edits_Error(dest_اشاریہ, dest_استعداد, edits, error_رمز);
}

U_CFUNC int32_t
ustrcase_map(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_PARAM
             char16_t *dest, int32_t dest_استعداد,
             ثابت char16_t *src, int32_t src_لمبائی,
             UStringCaseMapper *stringCaseMapper,
             icu::Edits *edits,
             UError_رمز &error_رمز) {
    int32_t dest_لمبائی;

    /* check argument values */
    اگر(U_FAILURE(error_رمز)) {
        واپس 0;
    }
    اگر( dest_استعداد<0 ||
        (dest==خالی_اشاریہ && dest_استعداد>0) ||
        src==خالی_اشاریہ ||
        src_لمبائی<-1
    ) {
        error_رمز=U_ILLEGAL_دلیل_ERROR;
        واپس 0;
    }

    /* get the string length */
    اگر(src_لمبائی==-1) {
        src_لمبائی=u_strlen(src);
    }

    /* check for overlapping source and destination */
    اگر( dest!=خالی_اشاریہ &&
        ((src>=dest && src<(dest+dest_استعداد)) ||
         (dest>=src && dest<(src+src_لمبائی)))
    ) {
        error_رمز=U_ILLEGAL_دلیل_ERROR;
        واپس 0;
    }

    اگر (edits != خالی_اشاریہ && (options & U_EDITS_NO_RESET) == 0) {
        edits->reset();
    }
    dest_لمبائی=stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                                dest, dest_استعداد, src, src_لمبائی, edits, error_رمز);
    واپس u_terminateUChars(dest, dest_استعداد, dest_لمبائی, &error_رمز);
}

U_CFUNC int32_t
ustrcase_mapWithOverlap(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_PARAM
                        char16_t *dest, int32_t dest_استعداد,
                        ثابت char16_t *src, int32_t src_لمبائی,
                        UStringCaseMapper *stringCaseMapper,
                        UError_رمز &error_رمز) {
    char16_t بفر[300];
    char16_t *عارضی;

    int32_t dest_لمبائی;

    /* check argument values */
    اگر(U_FAILURE(error_رمز)) {
        واپس 0;
    }
    اگر( dest_استعداد<0 ||
        (dest==خالی_اشاریہ && dest_استعداد>0) ||
        src==خالی_اشاریہ ||
        src_لمبائی<-1
    ) {
        error_رمز=U_ILLEGAL_دلیل_ERROR;
        واپس 0;
    }

    /* get the string length */
    اگر(src_لمبائی==-1) {
        src_لمبائی=u_strlen(src);
    }

    /* check for overlapping source and destination */
    اگر( dest!=خالی_اشاریہ &&
        ((src>=dest && src<(dest+dest_استعداد)) ||
         (dest>=src && dest<(src+src_لمبائی)))
    ) {
        /* overlap: provide a temporary destination buffer and later copy the result */
        اگر(dest_استعداد<=UPRV_LENGTHOF(بفر)) {
            /* the stack buffer is large enough */
            عارضی=بفر;
        } ورنہ {
            /* allocate a buffer */
            عارضی=(char16_t *)uprv_malloc(dest_استعداد*U_SIZEOF_UCHAR);
            اگر(عارضی==خالی_اشاریہ) {
                error_رمز=U_حافظہ_تخصیص_ERROR;
                واپس 0;
            }
        }
    } ورنہ {
        عارضی=dest;
    }

    dest_لمبائی=stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                                عارضی, dest_استعداد, src, src_لمبائی, خالی_اشاریہ, error_رمز);
    اگر(عارضی!=dest) {
        /* copy the result string to the destination buffer */
        اگر (U_SUCCESS(error_رمز) && 0 < dest_لمبائی && dest_لمبائی <= dest_استعداد) {
            u_memmove(dest, عارضی, dest_لمبائی);
        }
        اگر(عارضی!=بفر) {
            uprv_free(عارضی);
        }
    }

    واپس u_terminateUChars(dest, dest_استعداد, dest_لمبائی, &error_رمز);
}

/* public API functions */

U_CAPI int32_t U_EXPORT2
u_strFoldCase(char16_t *dest, int32_t dest_استعداد,
              ثابت char16_t *src, int32_t src_لمبائی,
              uint32_t options,
              UError_رمز *p_Error_رمز) {
    واپس ustrcase_mapWithOverlap(
        UCASE_LOC_جڑ, options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_استعداد,
        src, src_لمبائی,
        ustrcase_اندرونی_Fold, *p_Error_رمز);
}

U_NAMESPACE_BEGIN

int32_t CaseMap::fold(
        uint32_t options,
        ثابت char16_t *src, int32_t src_لمبائی,
        char16_t *dest, int32_t dest_استعداد, Edits *edits,
        UError_رمز &error_رمز) {
    واپس ustrcase_map(
        UCASE_LOC_جڑ, options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_استعداد,
        src, src_لمبائی,
        ustrcase_اندرونی_Fold, edits, error_رمز);
}

U_NAMESPACE_انتہا

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
ساخت CmpEquivLevel {
    ثابت char16_t *آغاز, *s, *حد_بندی;
};
قسمتعریف ساخت CmpEquivLevel CmpEquivLevel;

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
ساکن int32_t _cmpFold(
            ثابت char16_t *s1, int32_t length1,
            ثابت char16_t *s2, int32_t length2,
            uint32_t options,
            int32_t *مطابقت_Len1, int32_t *مطابقت_Len2,
            UError_رمز *p_Error_رمز) {
    int32_t cmpRes = 0;

    /* current-level start/limit - s1/s2 as current */
    ثابت char16_t *start1, *start2, *limit1, *limit2;

    /* points to the original start address */
    ثابت char16_t *org1, *org2;

    /* points to the end of match + 1 */
    ثابت char16_t *m1, *m2;

    /* case folding variables */
    ثابت char16_t *p;
    int32_t length;

    /* stacks of previous-level start/current/limit */
    CmpEquivLevel stack1[2], stack2[2];

    /* case folding buffers, only use current-level start/limit */
    char16_t fold1[UCASE_MAX_STRING_لمبائی+1], fold2[UCASE_MAX_STRING_لمبائی+1];

    /* track which is the current level per string */
    int32_t level1, level2;

    /* current code units, and code points for lookups */
    UChar32 c1, c2, cp1, cp2;

    /* no argument error checking because this itself is not an API */

    /*
     * assume that at least the option U_COMPARE_IGNORE_CASE is set
     * otherwise this function would have to behave exactly as uprv_strCompare()
     */
    اگر(U_FAILURE(*p_Error_رمز)) {
        واپس 0;
    }

    /* initialize */
    اگر(مطابقت_Len1) {
        U_ASSERT(مطابقت_Len2 !=خالی_اشاریہ);
        *مطابقت_Len1=0;
        *مطابقت_Len2=0;
    }

    start1=m1=org1=s1;
    اگر(length1==-1) {
        limit1=خالی_اشاریہ;
    } ورنہ {
        limit1=s1+length1;
    }

    start2=m2=org2=s2;
    اگر(length2==-1) {
        limit2=خالی_اشاریہ;
    } ورنہ {
        limit2=s2+length2;
    }

    level1=level2=0;
    c1=c2=-1;

    /* comparison loop */
    برائے(;;) {
        /*
         * here a code unit value of -1 means "get another code unit"
         * below it will mean "this source is finished"
         */

        اگر(c1<0) {
            /* get next code unit from string 1, post-increment */
            برائے(;;) {
                اگر(s1==limit1 || ((c1=*s1)==0 && (limit1==خالی_اشاریہ || (options&_STRNCMP_STYLE)))) {
                    اگر(level1==0) {
                        c1=-1;
                        توڑو;
                    }
                } ورنہ {
                    ++s1;
                    توڑو;
                }

                /* reached end of level buffer, pop one level */
                کرو {
                    --level1;
                    start1=stack1[level1].آغاز;    /*Not uninitialized*/
                } جب(start1==خالی_اشاریہ);
                s1=stack1[level1].s;                /*Not uninitialized*/
                limit1=stack1[level1].حد_بندی;        /*Not uninitialized*/
            }
        }

        اگر(c2<0) {
            /* get next code unit from string 2, post-increment */
            برائے(;;) {
                اگر(s2==limit2 || ((c2=*s2)==0 && (limit2==خالی_اشاریہ || (options&_STRNCMP_STYLE)))) {
                    اگر(level2==0) {
                        c2=-1;
                        توڑو;
                    }
                } ورنہ {
                    ++s2;
                    توڑو;
                }

                /* reached end of level buffer, pop one level */
                کرو {
                    --level2;
                    start2=stack2[level2].آغاز;    /*Not uninitialized*/
                } جب(start2==خالی_اشاریہ);
                s2=stack2[level2].s;                /*Not uninitialized*/
                limit2=stack2[level2].حد_بندی;        /*Not uninitialized*/
            }
        }

        /*
         * compare c1 and c2
         * either variable c1, c2 is -1 only if the corresponding string is finished
         */
        اگر(c1==c2) {
            ثابت char16_t *next1, *next2;

            اگر(c1<0) {
                cmpRes=0;   /* c1==c2==-1 indicating end of strings */
                توڑو;
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
            next1=next2=خالی_اشاریہ;
            اگر(level1==0) {
                next1=s1;
            } ورنہ اگر(s1==limit1) {
                /* Note: This implementation only use a single level of stack.
                 *      If this code needs to be changed to use multiple levels
                 *      of stacks, the code above should check if the current
                 *      code is at the end of all stacks.
                 */
                U_ASSERT(level1==1);

                /* is s1 at the end of the current stack? */
                next1=stack1[0].s;
            }

            اگر (next1!=خالی_اشاریہ) {
                اگر(level2==0) {
                    next2=s2;
                } ورنہ اگر(s2==limit2) {
                    U_ASSERT(level2==1);

                    /* is s2 at the end of the current stack? */
                    next2=stack2[0].s;
                }
                اگر(next2!=خالی_اشاریہ) {
                    m1=next1;
                    m2=next2;
                }
            }
            c1=c2=-1;       /* make us fetch new code units */
            جاری;
        } ورنہ اگر(c1<0) {
            cmpRes=-1;      /* string 1 ends before string 2 */
            توڑو;
        } ورنہ اگر(c2<0) {
            cmpRes=1;       /* string 2 ends before string 1 */
            توڑو;
        }
        /* c1!=c2 && c1>=0 && c2>=0 */

        /* get complete code points for c1, c2 for lookups if either is a surrogate */
        cp1=c1;
        اگر(U_IS_SURROGATE(c1)) {
            char16_t c;

            اگر(U_IS_SURROGATE_LEAD(c1)) {
                اگر(s1!=limit1 && U16_IS_TRAIL(c=*s1)) {
                    /* advance ++s1; only below if cp1 decomposes/case-folds */
                    cp1=U16_GET_SUPPLEMENTARY(c1, c);
                }
            } ورنہ /* isTrail(c1) */ {
                اگر(start1<=(s1-2) && U16_IS_LEAD(c=*(s1-2))) {
                    cp1=U16_GET_SUPPLEMENTARY(c, c1);
                }
            }
        }

        cp2=c2;
        اگر(U_IS_SURROGATE(c2)) {
            char16_t c;

            اگر(U_IS_SURROGATE_LEAD(c2)) {
                اگر(s2!=limit2 && U16_IS_TRAIL(c=*s2)) {
                    /* advance ++s2; only below if cp2 decomposes/case-folds */
                    cp2=U16_GET_SUPPLEMENTARY(c2, c);
                }
            } ورنہ /* isTrail(c2) */ {
                اگر(start2<=(s2-2) && U16_IS_LEAD(c=*(s2-2))) {
                    cp2=U16_GET_SUPPLEMENTARY(c, c2);
                }
            }
        }

        /*
         * go down one level for each string
         * continue with the main loop as soon as there is a real change
         */

        اگر( level1==0 &&
            (length = ucase_مقصد_بھرا_Folding(cp1, &p, options)) >= 0
        ) {
            /* cp1 case-folds to the code point "length" or to p[length] */
            اگر(U_IS_SURROGATE(c1)) {
                اگر(U_IS_SURROGATE_LEAD(c1)) {
                    /* advance beyond source surrogate pair if it case-folds */
                    ++s1;
                } ورنہ /* isTrail(c1) */ {
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
            stack1[0].آغاز=start1;
            stack1[0].s=s1;
            stack1[0].حد_بندی=limit1;
            ++level1;

            /* copy the folding result to fold1[] */
            اگر(length<=UCASE_MAX_STRING_لمبائی) {
                u_memcpy(fold1, p, length);
            } ورنہ {
                int32_t i=0;
                U16_APPEND_UNSAFE(fold1, i, length);
                length=i;
            }

            /* set next level pointers to case folding */
            start1=s1=fold1;
            limit1=fold1+length;

            /* get ready to read from decomposition, continue with loop */
            c1=-1;
            جاری;
        }

        اگر( level2==0 &&
            (length = ucase_مقصد_بھرا_Folding(cp2, &p, options)) >= 0
        ) {
            /* cp2 case-folds to the code point "length" or to p[length] */
            اگر(U_IS_SURROGATE(c2)) {
                اگر(U_IS_SURROGATE_LEAD(c2)) {
                    /* advance beyond source surrogate pair if it case-folds */
                    ++s2;
                } ورنہ /* isTrail(c2) */ {
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
            stack2[0].آغاز=start2;
            stack2[0].s=s2;
            stack2[0].حد_بندی=limit2;
            ++level2;

            /* copy the folding result to fold2[] */
            اگر(length<=UCASE_MAX_STRING_لمبائی) {
                u_memcpy(fold2, p, length);
            } ورنہ {
                int32_t i=0;
                U16_APPEND_UNSAFE(fold2, i, length);
                length=i;
            }

            /* set next level pointers to case folding */
            start2=s2=fold2;
            limit2=fold2+length;

            /* get ready to read from decomposition, continue with loop */
            c2=-1;
            جاری;
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

        اگر(c1>=0xd800 && c2>=0xd800 && (options&U_COMPARE_رمز_نقطہ_ترتیب)) {
            /* subtract 0x2800 from BMP code points to make them smaller than supplementary ones */
            اگر(
                (c1<=0xdbff && s1!=limit1 && U16_IS_TRAIL(*s1)) ||
                (U16_IS_TRAIL(c1) && start1!=(s1-1) && U16_IS_LEAD(*(s1-2)))
            ) {
                /* part of a surrogate pair, leave >=d800 */
            } ورنہ {
                /* BMP code point - may be surrogate code point - make <d800 */
                c1-=0x2800;
            }

            اگر(
                (c2<=0xdbff && s2!=limit2 && U16_IS_TRAIL(*s2)) ||
                (U16_IS_TRAIL(c2) && start2!=(s2-1) && U16_IS_LEAD(*(s2-2)))
            ) {
                /* part of a surrogate pair, leave >=d800 */
            } ورنہ {
                /* BMP code point - may be surrogate code point - make <d800 */
                c2-=0x2800;
            }
        }

        cmpRes=c1-c2;
        توڑو;
    }

    اگر(مطابقت_Len1) {
        *مطابقت_Len1=static_cast<int32_t>(m1-org1);
        *مطابقت_Len2=static_cast<int32_t>(m2-org2);
    }
    واپس cmpRes;
}

/* internal function */
U_CFUNC int32_t
u_strcmpFold(ثابت char16_t *s1, int32_t length1,
             ثابت char16_t *s2, int32_t length2,
             uint32_t options,
             UError_رمز *p_Error_رمز) {
    واپس _cmpFold(s1, length1, s2, length2, options, خالی_اشاریہ, خالی_اشاریہ, p_Error_رمز);
}

/* public API functions */

U_CAPI int32_t U_EXPORT2
u_strCaseCompare(ثابت char16_t *s1, int32_t length1,
                 ثابت char16_t *s2, int32_t length2,
                 uint32_t options,
                 UError_رمز *p_Error_رمز) {
    /* argument checking */
    اگر (p_Error_رمز == خالی_اشاریہ || U_FAILURE(*p_Error_رمز)) {
        واپس 0;
    }
    اگر(s1==خالی_اشاریہ || length1<-1 || s2==خالی_اشاریہ || length2<-1) {
        *p_Error_رمز=U_ILLEGAL_دلیل_ERROR;
        واپس 0;
    }
    واپس u_strcmpFold(s1, length1, s2, length2,
                        options|U_COMPARE_IGNORE_CASE,
                        p_Error_رمز);
}

U_CAPI int32_t U_EXPORT2
u_strcasecmp(ثابت char16_t *s1, ثابت char16_t *s2, uint32_t options) {
    UError_رمز error_رمز=U_ZERO_ERROR;
    واپس u_strcmpFold(s1, -1, s2, -1,
                        options|U_COMPARE_IGNORE_CASE,
                        &error_رمز);
}

U_CAPI int32_t U_EXPORT2
u_memcasecmp(ثابت char16_t *s1, ثابت char16_t *s2, int32_t length, uint32_t options) {
    UError_رمز error_رمز=U_ZERO_ERROR;
    واپس u_strcmpFold(s1, length, s2, length,
                        options|U_COMPARE_IGNORE_CASE,
                        &error_رمز);
}

U_CAPI int32_t U_EXPORT2
u_strncasecmp(ثابت char16_t *s1, ثابت char16_t *s2, int32_t n, uint32_t options) {
    UError_رمز error_رمز=U_ZERO_ERROR;
    واپس u_strcmpFold(s1, n, s2, n,
                        options|(U_COMPARE_IGNORE_CASE|_STRNCMP_STYLE),
                        &error_رمز);
}

/* internal API - detect length of shared prefix */
U_CAPI خالی
u_case_Insensitive_سابقہ_مطابقت(ثابت char16_t *s1, int32_t length1,
                             ثابت char16_t *s2, int32_t length2,
                             uint32_t options,
                             int32_t *مطابقت_Len1, int32_t *مطابقت_Len2,
                             UError_رمز *p_Error_رمز) {
    _cmpFold(s1, length1, s2, length2, options,
        مطابقت_Len1, مطابقت_Len2, p_Error_رمز);
}
