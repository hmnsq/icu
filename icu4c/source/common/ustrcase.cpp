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

#ضمن "unicode/utypes.h"
#ضمن "unicode/brkiter.h"
#ضمن "unicode/casemap.h"
#ضمن "unicode/edits.h"
#ضمن "unicode/stringoptions.h"
#ضمن "unicode/ustring.h"
#ضمن "unicode/ucasemap.h"
#ضمن "unicode/ubrk.h"
#ضمن "unicode/utf.h"
#ضمن "unicode/utf16.h"
#ضمن "cmemory.h"
#ضمن "ucase.h"
#ضمن "ucasemap_imp.h"
#ضمن "ustr_imp.h"
#ضمن "uassert.h"

/**
 * Code point for COMBINING ACUTE ACCENT
 * @internal
 */
#تعریف_کړه ACUTE u'\u0301'

U_NAMESPACE_BEGIN

نطاق {

int32_t check_ډکېدنه_And_Edits_Error(int32_t dest_شاخص, int32_t dest_ظرفیت,
                                   Edits *edits, UError_کوډ &error_کوډ) {
    إذا (U_SUCCESS(error_کوډ)) {
        إذا (dest_شاخص > dest_ظرفیت) {
            error_کوډ = U_بفر_ډکېدنه_ERROR;
        } وإلا إذا (edits != تش_اشاره) {
            edits->copy_Error_منزل(error_کوډ);
        }
    }
    أرجع dest_شاخص;
}

/* Appends a full case mapping result, see UCASE_MAX_STRING_LENGTH. */
مضمن int32_t
append_پایله(char16_t *dest, int32_t dest_شاخص, int32_t dest_ظرفیت,
             int32_t پایله, ثابت char16_t *s,
             int32_t cp_اوږدتیا, uint32_t options, icu::Edits *edits) {
    UChar32 c;
    int32_t length;

    /* decode the result */
    إذا(پایله<0) {
        /* (not) original code point */
        إذا(edits!=تش_اشاره) {
            edits->جمع_کړه_Unchanged(cp_اوږدتیا);
        }
        إذا(options & U_OMIT_UNCHANGED_متن) {
            أرجع dest_شاخص;
        }
        c=~پایله;
        إذا(dest_شاخص<dest_ظرفیت && c<=0xffff) {  // BMP slightly-fastpath
            dest[dest_شاخص++] = static_cast<char16_t>(c);
            أرجع dest_شاخص;
        }
        length=cp_اوږدتیا;
    } وإلا {
        إذا(پایله<=UCASE_MAX_STRING_اوږدتیا) {
            c=U_SENTINEL;
            length=پایله;
        } وإلا إذا(dest_شاخص<dest_ظرفیت && پایله<=0xffff) {  // BMP slightly-fastpath
            dest[dest_شاخص++] = static_cast<char16_t>(پایله);
            إذا(edits!=تش_اشاره) {
                edits->جمع_کړه_Replace(cp_اوږدتیا, 1);
            }
            أرجع dest_شاخص;
        } وإلا {
            c=پایله;
            length=U16_اوږدتیا(c);
        }
        إذا(edits!=تش_اشاره) {
            edits->جمع_کړه_Replace(cp_اوږدتیا, length);
        }
    }
    إذا(length>(INT32_MAX-dest_شاخص)) {
        أرجع -1;  // integer overflow
    }

    إذا(dest_شاخص<dest_ظرفیت) {
        /* append the result */
        إذا(c>=0) {
            /* code point */
            UBool isError=خطأ;
            U16_APPEND(dest, dest_شاخص, dest_ظرفیت, c, isError);
            إذا(isError) {
                /* overflow, nothing written */
                dest_شاخص+=length;
            }
        } وإلا {
            /* string */
            إذا((dest_شاخص+length)<=dest_ظرفیت) {
                طالما(length>0) {
                    dest[dest_شاخص++]=*s++;
                    --length;
                }
            } وإلا {
                /*ډکېدنه*/
                dest_شاخص+=length;
            }
        }
    } وإلا {
        /* preflight */
        dest_شاخص+=length;
    }
    أرجع dest_شاخص;
}

مضمن int32_t
appendUChar(char16_t *dest, int32_t dest_شاخص, int32_t dest_ظرفیت, char16_t c) {
    إذا(dest_شاخص<dest_ظرفیت) {
        dest[dest_شاخص]=c;
    } وإلا إذا(dest_شاخص==INT32_MAX) {
        أرجع -1;  // integer overflow
    }
    أرجع dest_شاخص+1;
}

int32_t
append_Non_تشه_Unchanged(char16_t *dest, int32_t dest_شاخص, int32_t dest_ظرفیت,
                        ثابت char16_t *s, int32_t length, uint32_t options, icu::Edits *edits) {
    إذا(edits!=تش_اشاره) {
        edits->جمع_کړه_Unchanged(length);
    }
    إذا(options & U_OMIT_UNCHANGED_متن) {
        أرجع dest_شاخص;
    }
    إذا(length>(INT32_MAX-dest_شاخص)) {
        أرجع -1;  // integer overflow
    }
    إذا((dest_شاخص+length)<=dest_ظرفیت) {
        u_memcpy(dest+dest_شاخص, s, length);
    }
    أرجع dest_شاخص + length;
}

مضمن int32_t
appendUnchanged(char16_t *dest, int32_t dest_شاخص, int32_t dest_ظرفیت,
                ثابت char16_t *s, int32_t length, uint32_t options, icu::Edits *edits) {
    إذا (length <= 0) {
        أرجع dest_شاخص;
    }
    أرجع append_Non_تشه_Unchanged(dest, dest_شاخص, dest_ظرفیت, s, length, options, edits);
}

UChar32 U_CALLCONV
utf16_caseContextIterator(فراغ *context, int8_t dir) {
    UCaseContext* csc = static_cast<UCaseContext*>(context);
    UChar32 c;

    إذا(dir<0) {
        /* reset for backward iteration */
        csc->شاخص=csc->cp_پیل;
        csc->dir=dir;
    } وإلا إذا(dir>0) {
        /* reset for forward iteration */
        csc->شاخص=csc->cp_حد;
        csc->dir=dir;
    } وإلا {
        /* continue current iteration direction */
        dir=csc->dir;
    }

    إذا(dir<0) {
        إذا(csc->پیل<csc->شاخص) {
            U16_PREV((ثابت char16_t *)csc->p, csc->پیل, csc->شاخص, c);
            أرجع c;
        }
    } وإلا {
        إذا(csc->شاخص<csc->حد) {
            U16_بل((ثابت char16_t *)csc->p, csc->شاخص, csc->حد, c);
            أرجع c;
        }
    }
    أرجع U_SENTINEL;
}

/**
 * caseLocale >= 0: Lowercases [srcStart..srcLimit[ but takes context [0..srcLength[ into account.
 * caseLocale < 0: Case-folds [srcStart..srcLimit[.
 */
int32_t منزل_ښکتنی(int32_t caseLocale, uint32_t options,
                char16_t *dest, int32_t dest_ظرفیت,
                ثابت char16_t *src, UCaseContext *csc, int32_t src_پیل, int32_t src_حد,
                icu::Edits *edits, UError_کوډ &error_کوډ) {
    ثابت int8_t *latin_منزل_ښکتنی;
    إذا (caseLocale == UCASE_LOC_ریښه ||
            (caseLocale >= 0 ?
                !(caseLocale == UCASE_LOC_TURKISH || caseLocale == UCASE_LOC_LITHUANIAN) :
                (options & FOLD_CASE_OPTIONS_ماسک) == U_FOLD_CASE_DEFAULT)) {
        latin_منزل_ښکتنی = LatinCase::منزل_ښکتنی_نورمال;
    } وإلا {
        latin_منزل_ښکتنی = LatinCase::منزل_ښکتنی_TR_LT;
    }
    ثابت UTrie2 *trie = ucase_getTrie();
    int32_t dest_شاخص = 0;
    int32_t prev = src_پیل;
    int32_t src_شاخص = src_پیل;
    لكل (;;) {
        // fast path for simple cases
        char16_t lead = 0;
        طالما (src_شاخص < src_حد) {
            lead = src[src_شاخص];
            int32_t delta;
            إذا (lead < LatinCase::LONG_S) {
                int8_t d = latin_منزل_ښکتنی[lead];
                إذا (d == LatinCase::EXC) { اكسر; }
                ++src_شاخص;
                إذا (d == 0) { استمر; }
                delta = d;
            } وإلا إذا (lead >= 0xd800) {
                اكسر;  // surrogate or higher
            } وإلا {
                uint16_t props = UTRIE2_GET16_FROM_U16_SINGLE_LEAD(trie, lead);
                إذا (UCASE_HAS_EXCEPTION(props)) { اكسر; }
                ++src_شاخص;
                إذا (!UCASE_IS_پاسنی_OR_TITLE(props) || (delta = UCASE_GET_DELTA(props)) == 0) {
                    استمر;
                }
            }
            lead += static_cast<char16_t>(delta);
            dest_شاخص = appendUnchanged(dest, dest_شاخص, dest_ظرفیت,
                                        src + prev, src_شاخص - 1 - prev, options, edits);
            إذا (dest_شاخص >= 0) {
                dest_شاخص = appendUChar(dest, dest_شاخص, dest_ظرفیت, lead);
                إذا (edits != تش_اشاره) {
                    edits->جمع_کړه_Replace(1, 1);
                }
            }
            إذا (dest_شاخص < 0) {
                error_کوډ = U_شاخص_OUTOFBOUNDS_ERROR;
                أرجع 0;
            }
            prev = src_شاخص;
        }
        إذا (src_شاخص >= src_حد) {
            اكسر;
        }
        // slow path
        int32_t cp_پیل = src_شاخص++;
        char16_t trail;
        UChar32 c;
        إذا (U16_IS_LEAD(lead) && src_شاخص < src_حد && U16_IS_TRAIL(trail = src[src_شاخص])) {
            c = U16_GET_SUPPLEMENTARY(lead, trail);
            ++src_شاخص;
        } وإلا {
            c = lead;
        }
        ثابت char16_t *s = تش_اشاره;
        إذا (caseLocale >= 0) {
            csc->cp_پیل = cp_پیل;
            csc->cp_حد = src_شاخص;
            c = ucase_منزل_ډک_ښکتنی(c, utf16_caseContextIterator, csc, &s, caseLocale);
        } وإلا {
            c = ucase_منزل_ډک_Folding(c, &s, options);
        }
        إذا (c >= 0) {
            dest_شاخص = appendUnchanged(dest, dest_شاخص, dest_ظرفیت,
                                        src + prev, cp_پیل - prev, options, edits);
            إذا (dest_شاخص >= 0) {
                dest_شاخص = append_پایله(dest, dest_شاخص, dest_ظرفیت, c, s,
                                         src_شاخص - cp_پیل, options, edits);
            }
            إذا (dest_شاخص < 0) {
                error_کوډ = U_شاخص_OUTOFBOUNDS_ERROR;
                أرجع 0;
            }
            prev = src_شاخص;
        }
    }
    dest_شاخص = appendUnchanged(dest, dest_شاخص, dest_ظرفیت,
                                src + prev, src_شاخص - prev, options, edits);
    إذا (dest_شاخص < 0) {
        error_کوډ = U_شاخص_OUTOFBOUNDS_ERROR;
        أرجع 0;
    }
    أرجع dest_شاخص;
}

int32_t منزل_پاسنی(int32_t caseLocale, uint32_t options,
                char16_t *dest, int32_t dest_ظرفیت,
                ثابت char16_t *src, UCaseContext *csc, int32_t src_اوږدتیا,
                icu::Edits *edits, UError_کوډ &error_کوډ) {
    ثابت int8_t *latin_منزل_پاسنی;
    إذا (caseLocale == UCASE_LOC_TURKISH) {
        latin_منزل_پاسنی = LatinCase::منزل_پاسنی_TR;
    } وإلا {
        latin_منزل_پاسنی = LatinCase::منزل_پاسنی_نورمال;
    }
    ثابت UTrie2 *trie = ucase_getTrie();
    int32_t dest_شاخص = 0;
    int32_t prev = 0;
    int32_t src_شاخص = 0;
    لكل (;;) {
        // fast path for simple cases
        char16_t lead = 0;
        طالما (src_شاخص < src_اوږدتیا) {
            lead = src[src_شاخص];
            int32_t delta;
            إذا (lead < LatinCase::LONG_S) {
                int8_t d = latin_منزل_پاسنی[lead];
                إذا (d == LatinCase::EXC) { اكسر; }
                ++src_شاخص;
                إذا (d == 0) { استمر; }
                delta = d;
            } وإلا إذا (lead >= 0xd800) {
                اكسر;  // surrogate or higher
            } وإلا {
                uint16_t props = UTRIE2_GET16_FROM_U16_SINGLE_LEAD(trie, lead);
                إذا (UCASE_HAS_EXCEPTION(props)) { اكسر; }
                ++src_شاخص;
                إذا (UCASE_GET_TYPE(props) != UCASE_ښکتنی || (delta = UCASE_GET_DELTA(props)) == 0) {
                    استمر;
                }
            }
            lead += static_cast<char16_t>(delta);
            dest_شاخص = appendUnchanged(dest, dest_شاخص, dest_ظرفیت,
                                        src + prev, src_شاخص - 1 - prev, options, edits);
            إذا (dest_شاخص >= 0) {
                dest_شاخص = appendUChar(dest, dest_شاخص, dest_ظرفیت, lead);
                إذا (edits != تش_اشاره) {
                    edits->جمع_کړه_Replace(1, 1);
                }
            }
            إذا (dest_شاخص < 0) {
                error_کوډ = U_شاخص_OUTOFBOUNDS_ERROR;
                أرجع 0;
            }
            prev = src_شاخص;
        }
        إذا (src_شاخص >= src_اوږدتیا) {
            اكسر;
        }
        // slow path
        int32_t cp_پیل;
        csc->cp_پیل = cp_پیل = src_شاخص++;
        char16_t trail;
        UChar32 c;
        إذا (U16_IS_LEAD(lead) && src_شاخص < src_اوږدتیا && U16_IS_TRAIL(trail = src[src_شاخص])) {
            c = U16_GET_SUPPLEMENTARY(lead, trail);
            ++src_شاخص;
        } وإلا {
            c = lead;
        }
        csc->cp_حد = src_شاخص;
        ثابت char16_t *s = تش_اشاره;
        c = ucase_منزل_ډک_پاسنی(c, utf16_caseContextIterator, csc, &s, caseLocale);
        إذا (c >= 0) {
            dest_شاخص = appendUnchanged(dest, dest_شاخص, dest_ظرفیت,
                                        src + prev, cp_پیل - prev, options, edits);
            إذا (dest_شاخص >= 0) {
                dest_شاخص = append_پایله(dest, dest_شاخص, dest_ظرفیت, c, s,
                                         src_شاخص - cp_پیل, options, edits);
            }
            إذا (dest_شاخص < 0) {
                error_کوډ = U_شاخص_OUTOFBOUNDS_ERROR;
                أرجع 0;
            }
            prev = src_شاخص;
        }
    }
    dest_شاخص = appendUnchanged(dest, dest_شاخص, dest_ظرفیت,
                                src + prev, src_شاخص - prev, options, edits);
    إذا (dest_شاخص < 0) {
        error_کوډ = U_شاخص_OUTOFBOUNDS_ERROR;
        أرجع 0;
    }
    أرجع dest_شاخص;
}

}  // namespace

U_NAMESPACE_پای_ټکی

U_NAMESPACE_USE

#إذا !UCONFIG_NO_BREAK_ITERATION

نطاق {

/**
 * Input: c is a letter I with or without acute accent.
 * start is the index in src after c, and is less than segmentLimit.
 * If a plain i/I is followed by a plain j/J,
 * or an i/I with acute (precomposed or decomposed) is followed by a j/J with acute,
 * then we output accordingly.
 *
 * @return the src index after the titlecased sequence, or the start index if no Dutch IJ
 */
int32_t maybeTitleDutchIJ(ثابت char16_t *src, UChar32 c, int32_t پیل, int32_t segment_حد,
                          char16_t *dest, int32_t &dest_شاخص, int32_t dest_ظرفیت, uint32_t options,
                          icu::Edits *edits) {
    U_ASSERT(پیل < segment_حد);

    int32_t شاخص = پیل;
    منطقي withAcute = خطأ;

    // If the conditions are met, then the following variables tell us what to output.
    int32_t unchanged1 = 0;  // code units before the j, or the whole sequence (0..3)
    منطقي doTitleJ = خطأ;  // true if the j needs to be titlecased
    int32_t unchanged2 = 0;  // after the j (0 or 1)

    // next character after the first letter
    char16_t c2 = src[شاخص++];

    // Is the first letter an i/I with accent?
    إذا (c == u'I') {
        إذا (c2 == ACUTE) {
            withAcute = ریښتیا;
            unchanged1 = 1;
            إذا (شاخص == segment_حد) { أرجع پیل; }
            c2 = src[شاخص++];
        }
    } وإلا {  // Í
        withAcute = ریښتیا;
    }

    // Is the next character a j/J?
    إذا (c2 == u'j') {
        doTitleJ = ریښتیا;
    } وإلا إذا (c2 == u'J') {
        ++unchanged1;
    } وإلا {
        أرجع پیل;
    }

    // A plain i/I must be followed by a plain j/J.
    // An i/I with acute must be followed by a j/J with acute.
    إذا (withAcute) {
        إذا (شاخص == segment_حد || src[شاخص++] != ACUTE) { أرجع پیل; }
        إذا (doTitleJ) {
            unchanged2 = 1;
        } وإلا {
            ++unchanged1;
        }
    }

    // There must not be another combining mark.
    إذا (شاخص < segment_حد) {
        int32_t cp;
        int32_t i = شاخص;
        U16_بل(src, i, segment_حد, cp);
        uint32_t type_ماسک = U_GET_GC_ماسک(cp);
        إذا ((type_ماسک & U_GC_M_ماسک) != 0) {
            أرجع پیل;
        }
    }

    // Output the rest of the Dutch IJ.
    dest_شاخص = appendUnchanged(dest, dest_شاخص, dest_ظرفیت, src + پیل, unchanged1, options, edits);
    پیل += unchanged1;
    إذا (doTitleJ) {
        dest_شاخص = appendUChar(dest, dest_شاخص, dest_ظرفیت, u'J');
        إذا (edits != تش_اشاره) {
            edits->جمع_کړه_Replace(1, 1);
        }
        ++پیل;
    }
    dest_شاخص = appendUnchanged(dest, dest_شاخص, dest_ظرفیت, src + پیل, unchanged2, options, edits);

    U_ASSERT(پیل + unchanged2 == شاخص);
    أرجع شاخص;
}

}  // namespace

U_CFUNC int32_t U_CALLCONV
ustrcase_دننی_منزل_Title(int32_t caseLocale, uint32_t options, BreakIterator *iter,
                         char16_t *dest, int32_t dest_ظرفیت,
                         ثابت char16_t *src, int32_t src_اوږدتیا,
                         icu::Edits *edits,
                         UError_کوډ &error_کوډ) {
    إذا (!ustrcase_checkTitleAdjustmentOptions(options, error_کوډ)) {
        أرجع 0;
    }

    /* set up local variables */
    UCaseContext csc=UCASECONTEXT_INITIALIZER;
    csc.p=(فراغ *)src;
    csc.حد=src_اوږدتیا;
    int32_t dest_شاخص=0;
    int32_t prev=0;
    منطقي is_لومړی_شاخص=ریښتیا;

    /* titlecasing loop */
    طالما(prev<src_اوږدتیا) {
        /* find next index where to titlecase */
        int32_t شاخص;
        إذا(is_لومړی_شاخص) {
            is_لومړی_شاخص=خطأ;
            شاخص=iter->first();
        } وإلا {
            شاخص=iter->next();
        }
        إذا(شاخص==UBRK_بشپړ || شاخص>src_اوږدتیا) {
            شاخص=src_اوږدتیا;
        }

        /*
         * Segment [prev..index[ into 3 parts:
         * a) skipped characters (copy as-is) [prev..titleStart[
         * b) first letter (titlecase)              [titleStart..titleLimit[
         * c) subsequent characters (lowercase)                 [titleLimit..index[
         */
        إذا(prev<شاخص) {
            // Find and copy skipped characters [prev..titleStart[
            int32_t title_پیل=prev;
            int32_t title_حد=prev;
            UChar32 c;
            U16_بل(src, title_حد, شاخص, c);
            إذا ((options&U_TITLECASE_NO_BREAK_ADJUSTMENT)==0) {
                // Adjust the titlecasing index to the next cased character,
                // or to the next letter/number/symbol/private use.
                // Stop with titleStart<titleLimit<=index
                // if there is a character to be titlecased,
                // or else stop with titleStart==titleLimit==index.
                منطقي منزل_Cased = (options&U_TITLECASE_ADJUST_منزل_CASED) != 0;
                طالما (منزل_Cased ? UCASE_NONE==ucase_getType(c) : !ustrcase_isLNS(c)) {
                    title_پیل=title_حد;
                    إذا(title_حد==شاخص) {
                        اكسر;
                    }
                    U16_بل(src, title_حد, شاخص, c);
                }
                إذا (prev < title_پیل) {
                    dest_شاخص=appendUnchanged(dest, dest_شاخص, dest_ظرفیت,
                                              src+prev, title_پیل-prev, options, edits);
                    إذا(dest_شاخص<0) {
                        error_کوډ=U_شاخص_OUTOFBOUNDS_ERROR;
                        أرجع 0;
                    }
                }
            }

            إذا(title_پیل<title_حد) {
                /* titlecase c which is from [titleStart..titleLimit[ */
                csc.cp_پیل=title_پیل;
                csc.cp_حد=title_حد;
                ثابت char16_t *s;
                c=ucase_منزل_ډک_Title(c, utf16_caseContextIterator, &csc, &s, caseLocale);
                dest_شاخص=append_پایله(dest, dest_شاخص, dest_ظرفیت, c, s,
                                       title_حد-title_پیل, options, edits);
                إذا(dest_شاخص<0) {
                    error_کوډ=U_شاخص_OUTOFBOUNDS_ERROR;
                    أرجع 0;
                }

                /* Special case Dutch IJ titlecasing */
                إذا (title_پیل+1 < شاخص &&
                        caseLocale == UCASE_LOC_DUTCH) {
                    إذا (c < 0) {
                        c = ~c;
                    }

                    إذا (c == u'I' || c == u'Í') {
                        title_حد = maybeTitleDutchIJ(src, c, title_پیل + 1, شاخص,
                                                       dest, dest_شاخص, dest_ظرفیت, options,
                                                       edits);
                    }
                }

                /* lowercase [titleLimit..index[ */
                إذا(title_حد<شاخص) {
                    إذا((options&U_TITLECASE_NO_LOWERCASE)==0) {
                        /* Normal operation: Lowercase the rest of the word. */
                        dest_شاخص+=
                            منزل_ښکتنی(
                                caseLocale, options,
                                (dest==تش_اشاره) ? تش_اشاره: dest+dest_شاخص, dest_ظرفیت-dest_شاخص,
                                src, &csc, title_حد, شاخص,
                                edits, error_کوډ);
                        إذا(error_کوډ==U_بفر_ډکېدنه_ERROR) {
                            error_کوډ=U_ZERO_ERROR;
                        }
                        إذا(U_FAILURE(error_کوډ)) {
                            أرجع dest_شاخص;
                        }
                    } وإلا {
                        /* Optionally just copy the rest of the word unchanged. */
                        dest_شاخص=appendUnchanged(dest, dest_شاخص, dest_ظرفیت,
                                                  src+title_حد, شاخص-title_حد, options, edits);
                        إذا(dest_شاخص<0) {
                            error_کوډ=U_شاخص_OUTOFBOUNDS_ERROR;
                            أرجع 0;
                        }
                    }
                }
            }
        }

        prev=شاخص;
    }

    أرجع check_ډکېدنه_And_Edits_Error(dest_شاخص, dest_ظرفیت, edits, error_کوډ);
}

#د_که_پای  // !UCONFIG_NO_BREAK_ITERATION

U_NAMESPACE_BEGIN
نطاق Greek_پاسنی {

// Data generated by prototype code, see
// https://icu.unicode.org/design/case/greek-upper
// TODO: Move this data into ucase.icu.
ساكن ثابت uint16_t data0370[] = {
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

ساكن ثابت uint16_t data1F00[] = {
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
ساكن ثابت uint16_t data2126 = 0x03A9 | HAS_VOWEL;

uint32_t get_Letter_معلومات(UChar32 c) {
    إذا (c < 0x370 || 0x2126 < c || (0x3ff < c && c < 0x1f00)) {
        أرجع 0;
    } وإلا إذا (c <= 0x3ff) {
        أرجع data0370[c - 0x370];
    } وإلا إذا (c <= 0x1fff) {
        أرجع data1F00[c - 0x1f00];
    } وإلا إذا (c == 0x2126) {
        أرجع data2126;
    } وإلا {
        أرجع 0;
    }
}

uint32_t get_Diacritic_معلومات(UChar32 c) {
    بدل (c) {
    حالة 0x0300:  // varia
    حالة 0x0301:  // tonos = oxia
    حالة 0x0342:  // perispomeni
    حالة 0x0302:  // circumflex can look like perispomeni
    حالة 0x0303:  // tilde can look like perispomeni
    حالة 0x0311:  // inverted breve can look like perispomeni
        أرجع HAS_ACCENT;
    حالة 0x0308:  // dialytika = diaeresis
        أرجع HAS_COMBINING_DIALYTIKA;
    حالة 0x0344:  // dialytika tonos
        أرجع HAS_COMBINING_DIALYTIKA | HAS_ACCENT;
    حالة 0x0345:  // ypogegrammeni = iota subscript
        أرجع HAS_YPOGEGRAMMENI;
    حالة 0x0304:  // macron
    حالة 0x0306:  // breve
    حالة 0x0313:  // comma above
    حالة 0x0314:  // reversed comma above
    حالة 0x0343:  // koronis
        أرجع HAS_OTHER_GREEK_DIACRITIC;
    افتراضي:
        أرجع 0;
    }
}

UBool isFollowedByCasedLetter(ثابت char16_t *s, int32_t i, int32_t length) {
    طالما (i < length) {
        UChar32 c;
        U16_بل(s, i, length, c);
        int32_t type = ucase_getTypeOrIgnorable(c);
        إذا ((type & UCASE_IGNORABLE) != 0) {
            // Case-ignorable, continue with the loop.
        } وإلا إذا (type != UCASE_NONE) {
            أرجع ریښتیا;  // Followed by cased letter.
        } وإلا {
            أرجع خطأ;  // Uncased and not case-ignorable.
        }
    }
    أرجع خطأ;  // Not followed by cased letter.
}

/**
 * Greek string uppercasing with a state machine.
 * Probably simpler than a stateless function that has to figure out complex context-before
 * for each character.
 * TODO: Try to re-consolidate one way or another with the non-Greek function.
 */
int32_t منزل_پاسنی(uint32_t options,
                char16_t *dest, int32_t dest_ظرفیت,
                ثابت char16_t *src, int32_t src_اوږدتیا,
                Edits *edits,
                UError_کوډ &error_کوډ) {
    int32_t dest_شاخص=0;
    uint32_t state = 0;
    لكل (int32_t i = 0; i < src_اوږدتیا;) {
        int32_t بل_شاخص = i;
        UChar32 c;
        U16_بل(src, بل_شاخص, src_اوږدتیا, c);
        uint32_t بل_State = 0;
        int32_t type = ucase_getTypeOrIgnorable(c);
        إذا ((type & UCASE_IGNORABLE) != 0) {
            // c is case-ignorable
            بل_State |= (state & AFTER_CASED);
        } وإلا إذا (type != UCASE_NONE) {
            // c is cased
            بل_State |= AFTER_CASED;
        }
        uint32_t معلومات = get_Letter_معلومات(c);
        إذا (معلومات > 0) {
            uint32_t upper = معلومات & پاسنی_ماسک;
            // Add a dialytika to this iota or ypsilon vowel
            // if we removed a tonos from the previous vowel,
            // and that previous vowel did not also have (or gain) a dialytika.
            // Adding one only to the final vowel in a longer sequence
            // (which does not occur in normal writing) would require lookahead.
            // Set the same flag as for preserving an existing dialytika.
            إذا ((معلومات & HAS_VOWEL) != 0 &&
                (state & (AFTER_VOWEL_WITH_PRECOMPOSED_ACCENT | AFTER_VOWEL_WITH_COMBINING_ACCENT)) !=
                    0 &&
                (upper == 0x399 || upper == 0x3A5)) {
                معلومات |= (state & AFTER_VOWEL_WITH_PRECOMPOSED_ACCENT) ? HAS_DIALYTIKA
                                                                      : HAS_COMBINING_DIALYTIKA;
            }
            int32_t عدد_Ypogegrammeni = 0;  // Map each one to a trailing, spacing, capital iota.
            إذا ((معلومات & HAS_YPOGEGRAMMENI) != 0) {
                عدد_Ypogegrammeni = 1;
            }
            ثابت UBool hasPrecomposedAccent = (معلومات & HAS_ACCENT) != 0;
            // Skip combining diacritics after this Greek letter.
            طالما (بل_شاخص < src_اوږدتیا) {
                uint32_t diacritic_معلومات = get_Diacritic_معلومات(src[بل_شاخص]);
                إذا (diacritic_معلومات != 0) {
                    معلومات |= diacritic_معلومات;
                    إذا ((diacritic_معلومات & HAS_YPOGEGRAMMENI) != 0) {
                        ++عدد_Ypogegrammeni;
                    }
                    ++بل_شاخص;
                } وإلا {
                    اكسر;  // not a Greek diacritic
                }
            }
            إذا ((معلومات & HAS_VOWEL_AND_ACCENT_AND_DIALYTIKA) == HAS_VOWEL_AND_ACCENT) {
                بل_State |= hasPrecomposedAccent ? AFTER_VOWEL_WITH_PRECOMPOSED_ACCENT
                                                  : AFTER_VOWEL_WITH_COMBINING_ACCENT;
            }
            // Map according to Greek rules.
            UBool جمع_کړه_Tonos = خطأ;
            إذا (upper == 0x397 &&
                    (معلومات & HAS_ACCENT) != 0 &&
                    عدد_Ypogegrammeni == 0 &&
                    (state & AFTER_CASED) == 0 &&
                    !isFollowedByCasedLetter(src, بل_شاخص, src_اوږدتیا)) {
                // Keep disjunctive "or" with (only) a tonos.
                // We use the same "word boundary" conditions as for the Final_Sigma test.
                إذا (hasPrecomposedAccent) {
                    upper = 0x389;  // Preserve the precomposed form.
                } وإلا {
                    جمع_کړه_Tonos = ریښتیا;
                }
            } وإلا إذا ((معلومات & HAS_DIALYTIKA) != 0) {
                // Preserve a vowel with dialytika in precomposed form if it exists.
                إذا (upper == 0x399) {
                    upper = 0x3AA;
                    معلومات &= ~HAS_EITHER_DIALYTIKA;
                } وإلا إذا (upper == 0x3A5) {
                    upper = 0x3AB;
                    معلومات &= ~HAS_EITHER_DIALYTIKA;
                }
            }

            UBool change;
            إذا (edits == تش_اشاره && (options & U_OMIT_UNCHANGED_متن) == 0) {
                change = ریښتیا;  // common, simple usage
            } وإلا {
                // Find out first whether we are changing the text.
                change = src[i] != upper || عدد_Ypogegrammeni > 0;
                int32_t i2 = i + 1;
                إذا ((معلومات & HAS_EITHER_DIALYTIKA) != 0) {
                    change |= i2 >= بل_شاخص || src[i2] != 0x308;
                    ++i2;
                }
                إذا (جمع_کړه_Tonos) {
                    change |= i2 >= بل_شاخص || src[i2] != 0x301;
                    ++i2;
                }
                int32_t old_اوږدتیا = بل_شاخص - i;
                int32_t new_اوږدتیا = (i2 - i) + عدد_Ypogegrammeni;
                change |= old_اوږدتیا != new_اوږدتیا;
                إذا (change) {
                    إذا (edits != تش_اشاره) {
                        edits->جمع_کړه_Replace(old_اوږدتیا, new_اوږدتیا);
                    }
                } وإلا {
                    إذا (edits != تش_اشاره) {
                        edits->جمع_کړه_Unchanged(old_اوږدتیا);
                    }
                    // Write unchanged text?
                    change = (options & U_OMIT_UNCHANGED_متن) == 0;
                }
            }

            إذا (change) {
                dest_شاخص = appendUChar(dest, dest_شاخص, dest_ظرفیت, static_cast<char16_t>(upper));
                إذا (dest_شاخص >= 0 && (معلومات & HAS_EITHER_DIALYTIKA) != 0) {
                    dest_شاخص=appendUChar(dest, dest_شاخص, dest_ظرفیت, 0x308);  // restore or add a dialytika
                }
                إذا (dest_شاخص >= 0 && جمع_کړه_Tonos) {
                    dest_شاخص=appendUChar(dest, dest_شاخص, dest_ظرفیت, 0x301);
                }
                طالما (dest_شاخص >= 0 && عدد_Ypogegrammeni > 0) {
                    dest_شاخص=appendUChar(dest, dest_شاخص, dest_ظرفیت, 0x399);
                    --عدد_Ypogegrammeni;
                }
                إذا(dest_شاخص<0) {
                    error_کوډ=U_شاخص_OUTOFBOUNDS_ERROR;
                    أرجع 0;
                }
            }
        } وإلا {
            ثابت char16_t *s;
            c=ucase_منزل_ډک_پاسنی(c, تش_اشاره, تش_اشاره, &s, UCASE_LOC_GREEK);
            dest_شاخص = append_پایله(dest, dest_شاخص, dest_ظرفیت, c, s,
                                     بل_شاخص - i, options, edits);
            إذا (dest_شاخص < 0) {
                error_کوډ = U_شاخص_OUTOFBOUNDS_ERROR;
                أرجع 0;
            }
        }
        i = بل_شاخص;
        state = بل_State;
    }

    أرجع dest_شاخص;
}

}  // namespace GreekUpper
U_NAMESPACE_پای_ټکی

/* functions available in the common library (for unistr_case.cpp) */

U_CFUNC int32_t U_CALLCONV
ustrcase_دننی_منزل_ښکتنی(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_ناکارول_شوی
                         char16_t *dest, int32_t dest_ظرفیت,
                         ثابت char16_t *src, int32_t src_اوږدتیا,
                         icu::Edits *edits,
                         UError_کوډ &error_کوډ) {
    UCaseContext csc=UCASECONTEXT_INITIALIZER;
    csc.p=(فراغ *)src;
    csc.حد=src_اوږدتیا;
    int32_t dest_شاخص = منزل_ښکتنی(
        caseLocale, options,
        dest, dest_ظرفیت,
        src, &csc, 0, src_اوږدتیا,
        edits, error_کوډ);
    أرجع check_ډکېدنه_And_Edits_Error(dest_شاخص, dest_ظرفیت, edits, error_کوډ);
}

U_CFUNC int32_t U_CALLCONV
ustrcase_دننی_منزل_پاسنی(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_ناکارول_شوی
                         char16_t *dest, int32_t dest_ظرفیت,
                         ثابت char16_t *src, int32_t src_اوږدتیا,
                         icu::Edits *edits,
                         UError_کوډ &error_کوډ) {
    int32_t dest_شاخص;
    إذا (caseLocale == UCASE_LOC_GREEK) {
        dest_شاخص = Greek_پاسنی::منزل_پاسنی(options, dest, dest_ظرفیت,
                                        src, src_اوږدتیا, edits, error_کوډ);
    } وإلا {
        UCaseContext csc=UCASECONTEXT_INITIALIZER;
        csc.p=(فراغ *)src;
        csc.حد=src_اوږدتیا;
        dest_شاخص = منزل_پاسنی(
            caseLocale, options,
            dest, dest_ظرفیت,
            src, &csc, src_اوږدتیا,
            edits, error_کوډ);
    }
    أرجع check_ډکېدنه_And_Edits_Error(dest_شاخص, dest_ظرفیت, edits, error_کوډ);
}

U_CFUNC int32_t U_CALLCONV
ustrcase_دننی_Fold(int32_t /* caseLocale */, uint32_t options, UCASEMAP_BREAK_ITERATOR_ناکارول_شوی
                      char16_t *dest, int32_t dest_ظرفیت,
                      ثابت char16_t *src, int32_t src_اوږدتیا,
                      icu::Edits *edits,
                      UError_کوډ &error_کوډ) {
    int32_t dest_شاخص = منزل_ښکتنی(
        -1, options,
        dest, dest_ظرفیت,
        src, تش_اشاره, 0, src_اوږدتیا,
        edits, error_کوډ);
    أرجع check_ډکېدنه_And_Edits_Error(dest_شاخص, dest_ظرفیت, edits, error_کوډ);
}

U_CFUNC int32_t
ustrcase_map(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_PARAM
             char16_t *dest, int32_t dest_ظرفیت,
             ثابت char16_t *src, int32_t src_اوږدتیا,
             UStringCaseMapper *stringCaseMapper,
             icu::Edits *edits,
             UError_کوډ &error_کوډ) {
    int32_t dest_اوږدتیا;

    /* check argument values */
    إذا(U_FAILURE(error_کوډ)) {
        أرجع 0;
    }
    إذا( dest_ظرفیت<0 ||
        (dest==تش_اشاره && dest_ظرفیت>0) ||
        src==تش_اشاره ||
        src_اوږدتیا<-1
    ) {
        error_کوډ=U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }

    /* get the string length */
    إذا(src_اوږدتیا==-1) {
        src_اوږدتیا=u_strlen(src);
    }

    /* check for overlapping source and destination */
    إذا( dest!=تش_اشاره &&
        ((src>=dest && src<(dest+dest_ظرفیت)) ||
         (dest>=src && dest<(src+src_اوږدتیا)))
    ) {
        error_کوډ=U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }

    إذا (edits != تش_اشاره && (options & U_EDITS_NO_RESET) == 0) {
        edits->reset();
    }
    dest_اوږدتیا=stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                                dest, dest_ظرفیت, src, src_اوږدتیا, edits, error_کوډ);
    أرجع u_terminateUChars(dest, dest_ظرفیت, dest_اوږدتیا, &error_کوډ);
}

U_CFUNC int32_t
ustrcase_mapWithOverlap(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_PARAM
                        char16_t *dest, int32_t dest_ظرفیت,
                        ثابت char16_t *src, int32_t src_اوږدتیا,
                        UStringCaseMapper *stringCaseMapper,
                        UError_کوډ &error_کوډ) {
    char16_t بفر[300];
    char16_t *لنډمهاله;

    int32_t dest_اوږدتیا;

    /* check argument values */
    إذا(U_FAILURE(error_کوډ)) {
        أرجع 0;
    }
    إذا( dest_ظرفیت<0 ||
        (dest==تش_اشاره && dest_ظرفیت>0) ||
        src==تش_اشاره ||
        src_اوږدتیا<-1
    ) {
        error_کوډ=U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }

    /* get the string length */
    إذا(src_اوږدتیا==-1) {
        src_اوږدتیا=u_strlen(src);
    }

    /* check for overlapping source and destination */
    إذا( dest!=تش_اشاره &&
        ((src>=dest && src<(dest+dest_ظرفیت)) ||
         (dest>=src && dest<(src+src_اوږدتیا)))
    ) {
        /* overlap: provide a temporary destination buffer and later copy the result */
        إذا(dest_ظرفیت<=UPRV_LENGTHOF(بفر)) {
            /* the stack buffer is large enough */
            لنډمهاله=بفر;
        } وإلا {
            /* allocate a buffer */
            لنډمهاله=(char16_t *)uprv_malloc(dest_ظرفیت*U_SIZEOF_UCHAR);
            إذا(لنډمهاله==تش_اشاره) {
                error_کوډ=U_حافظه_ویش_ERROR;
                أرجع 0;
            }
        }
    } وإلا {
        لنډمهاله=dest;
    }

    dest_اوږدتیا=stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                                لنډمهاله, dest_ظرفیت, src, src_اوږدتیا, تش_اشاره, error_کوډ);
    إذا(لنډمهاله!=dest) {
        /* copy the result string to the destination buffer */
        إذا (U_SUCCESS(error_کوډ) && 0 < dest_اوږدتیا && dest_اوږدتیا <= dest_ظرفیت) {
            u_memmove(dest, لنډمهاله, dest_اوږدتیا);
        }
        إذا(لنډمهاله!=بفر) {
            uprv_free(لنډمهاله);
        }
    }

    أرجع u_terminateUChars(dest, dest_ظرفیت, dest_اوږدتیا, &error_کوډ);
}

/* public API functions */

U_CAPI int32_t U_EXPORT2
u_strFoldCase(char16_t *dest, int32_t dest_ظرفیت,
              ثابت char16_t *src, int32_t src_اوږدتیا,
              uint32_t options,
              UError_کوډ *p_Error_کوډ) {
    أرجع ustrcase_mapWithOverlap(
        UCASE_LOC_ریښه, options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_ظرفیت,
        src, src_اوږدتیا,
        ustrcase_دننی_Fold, *p_Error_کوډ);
}

U_NAMESPACE_BEGIN

int32_t CaseMap::fold(
        uint32_t options,
        ثابت char16_t *src, int32_t src_اوږدتیا,
        char16_t *dest, int32_t dest_ظرفیت, Edits *edits,
        UError_کوډ &error_کوډ) {
    أرجع ustrcase_map(
        UCASE_LOC_ریښه, options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_ظرفیت,
        src, src_اوږدتیا,
        ustrcase_دننی_Fold, edits, error_کوډ);
}

U_NAMESPACE_پای_ټکی

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
بنية CmpEquivLevel {
    ثابت char16_t *پیل, *s, *حد;
};
تعريفنوع بنية CmpEquivLevel CmpEquivLevel;

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
ساكن int32_t _cmpFold(
            ثابت char16_t *s1, int32_t length1,
            ثابت char16_t *s2, int32_t length2,
            uint32_t options,
            int32_t *سمون_Len1, int32_t *سمون_Len2,
            UError_کوډ *p_Error_کوډ) {
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
    char16_t fold1[UCASE_MAX_STRING_اوږدتیا+1], fold2[UCASE_MAX_STRING_اوږدتیا+1];

    /* track which is the current level per string */
    int32_t level1, level2;

    /* current code units, and code points for lookups */
    UChar32 c1, c2, cp1, cp2;

    /* no argument error checking because this itself is not an API */

    /*
     * assume that at least the option U_COMPARE_IGNORE_CASE is set
     * otherwise this function would have to behave exactly as uprv_strCompare()
     */
    إذا(U_FAILURE(*p_Error_کوډ)) {
        أرجع 0;
    }

    /* initialize */
    إذا(سمون_Len1) {
        U_ASSERT(سمون_Len2 !=تش_اشاره);
        *سمون_Len1=0;
        *سمون_Len2=0;
    }

    start1=m1=org1=s1;
    إذا(length1==-1) {
        limit1=تش_اشاره;
    } وإلا {
        limit1=s1+length1;
    }

    start2=m2=org2=s2;
    إذا(length2==-1) {
        limit2=تش_اشاره;
    } وإلا {
        limit2=s2+length2;
    }

    level1=level2=0;
    c1=c2=-1;

    /* comparison loop */
    لكل(;;) {
        /*
         * here a code unit value of -1 means "get another code unit"
         * below it will mean "this source is finished"
         */

        إذا(c1<0) {
            /* get next code unit from string 1, post-increment */
            لكل(;;) {
                إذا(s1==limit1 || ((c1=*s1)==0 && (limit1==تش_اشاره || (options&_STRNCMP_STYLE)))) {
                    إذا(level1==0) {
                        c1=-1;
                        اكسر;
                    }
                } وإلا {
                    ++s1;
                    اكسر;
                }

                /* reached end of level buffer, pop one level */
                افعل {
                    --level1;
                    start1=stack1[level1].پیل;    /*Not uninitialized*/
                } طالما(start1==تش_اشاره);
                s1=stack1[level1].s;                /*Not uninitialized*/
                limit1=stack1[level1].حد;        /*Not uninitialized*/
            }
        }

        إذا(c2<0) {
            /* get next code unit from string 2, post-increment */
            لكل(;;) {
                إذا(s2==limit2 || ((c2=*s2)==0 && (limit2==تش_اشاره || (options&_STRNCMP_STYLE)))) {
                    إذا(level2==0) {
                        c2=-1;
                        اكسر;
                    }
                } وإلا {
                    ++s2;
                    اكسر;
                }

                /* reached end of level buffer, pop one level */
                افعل {
                    --level2;
                    start2=stack2[level2].پیل;    /*Not uninitialized*/
                } طالما(start2==تش_اشاره);
                s2=stack2[level2].s;                /*Not uninitialized*/
                limit2=stack2[level2].حد;        /*Not uninitialized*/
            }
        }

        /*
         * compare c1 and c2
         * either variable c1, c2 is -1 only if the corresponding string is finished
         */
        إذا(c1==c2) {
            ثابت char16_t *next1, *next2;

            إذا(c1<0) {
                cmpRes=0;   /* c1==c2==-1 indicating end of strings */
                اكسر;
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
            next1=next2=تش_اشاره;
            إذا(level1==0) {
                next1=s1;
            } وإلا إذا(s1==limit1) {
                /* Note: This implementation only use a single level of stack.
                 *      If this code needs to be changed to use multiple levels
                 *      of stacks, the code above should check if the current
                 *      code is at the end of all stacks.
                 */
                U_ASSERT(level1==1);

                /* is s1 at the end of the current stack? */
                next1=stack1[0].s;
            }

            إذا (next1!=تش_اشاره) {
                إذا(level2==0) {
                    next2=s2;
                } وإلا إذا(s2==limit2) {
                    U_ASSERT(level2==1);

                    /* is s2 at the end of the current stack? */
                    next2=stack2[0].s;
                }
                إذا(next2!=تش_اشاره) {
                    m1=next1;
                    m2=next2;
                }
            }
            c1=c2=-1;       /* make us fetch new code units */
            استمر;
        } وإلا إذا(c1<0) {
            cmpRes=-1;      /* string 1 ends before string 2 */
            اكسر;
        } وإلا إذا(c2<0) {
            cmpRes=1;       /* string 2 ends before string 1 */
            اكسر;
        }
        /* c1!=c2 && c1>=0 && c2>=0 */

        /* get complete code points for c1, c2 for lookups if either is a surrogate */
        cp1=c1;
        إذا(U_IS_SURROGATE(c1)) {
            char16_t c;

            إذا(U_IS_SURROGATE_LEAD(c1)) {
                إذا(s1!=limit1 && U16_IS_TRAIL(c=*s1)) {
                    /* advance ++s1; only below if cp1 decomposes/case-folds */
                    cp1=U16_GET_SUPPLEMENTARY(c1, c);
                }
            } وإلا /* isTrail(c1) */ {
                إذا(start1<=(s1-2) && U16_IS_LEAD(c=*(s1-2))) {
                    cp1=U16_GET_SUPPLEMENTARY(c, c1);
                }
            }
        }

        cp2=c2;
        إذا(U_IS_SURROGATE(c2)) {
            char16_t c;

            إذا(U_IS_SURROGATE_LEAD(c2)) {
                إذا(s2!=limit2 && U16_IS_TRAIL(c=*s2)) {
                    /* advance ++s2; only below if cp2 decomposes/case-folds */
                    cp2=U16_GET_SUPPLEMENTARY(c2, c);
                }
            } وإلا /* isTrail(c2) */ {
                إذا(start2<=(s2-2) && U16_IS_LEAD(c=*(s2-2))) {
                    cp2=U16_GET_SUPPLEMENTARY(c, c2);
                }
            }
        }

        /*
         * go down one level for each string
         * continue with the main loop as soon as there is a real change
         */

        إذا( level1==0 &&
            (length = ucase_منزل_ډک_Folding(cp1, &p, options)) >= 0
        ) {
            /* cp1 case-folds to the code point "length" or to p[length] */
            إذا(U_IS_SURROGATE(c1)) {
                إذا(U_IS_SURROGATE_LEAD(c1)) {
                    /* advance beyond source surrogate pair if it case-folds */
                    ++s1;
                } وإلا /* isTrail(c1) */ {
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
            stack1[0].پیل=start1;
            stack1[0].s=s1;
            stack1[0].حد=limit1;
            ++level1;

            /* copy the folding result to fold1[] */
            إذا(length<=UCASE_MAX_STRING_اوږدتیا) {
                u_memcpy(fold1, p, length);
            } وإلا {
                int32_t i=0;
                U16_APPEND_UNSAFE(fold1, i, length);
                length=i;
            }

            /* set next level pointers to case folding */
            start1=s1=fold1;
            limit1=fold1+length;

            /* get ready to read from decomposition, continue with loop */
            c1=-1;
            استمر;
        }

        إذا( level2==0 &&
            (length = ucase_منزل_ډک_Folding(cp2, &p, options)) >= 0
        ) {
            /* cp2 case-folds to the code point "length" or to p[length] */
            إذا(U_IS_SURROGATE(c2)) {
                إذا(U_IS_SURROGATE_LEAD(c2)) {
                    /* advance beyond source surrogate pair if it case-folds */
                    ++s2;
                } وإلا /* isTrail(c2) */ {
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
            stack2[0].پیل=start2;
            stack2[0].s=s2;
            stack2[0].حد=limit2;
            ++level2;

            /* copy the folding result to fold2[] */
            إذا(length<=UCASE_MAX_STRING_اوږدتیا) {
                u_memcpy(fold2, p, length);
            } وإلا {
                int32_t i=0;
                U16_APPEND_UNSAFE(fold2, i, length);
                length=i;
            }

            /* set next level pointers to case folding */
            start2=s2=fold2;
            limit2=fold2+length;

            /* get ready to read from decomposition, continue with loop */
            c2=-1;
            استمر;
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

        إذا(c1>=0xd800 && c2>=0xd800 && (options&U_COMPARE_کوډ_ټکی_ترتیب)) {
            /* subtract 0x2800 from BMP code points to make them smaller than supplementary ones */
            إذا(
                (c1<=0xdbff && s1!=limit1 && U16_IS_TRAIL(*s1)) ||
                (U16_IS_TRAIL(c1) && start1!=(s1-1) && U16_IS_LEAD(*(s1-2)))
            ) {
                /* part of a surrogate pair, leave >=d800 */
            } وإلا {
                /* BMP code point - may be surrogate code point - make <d800 */
                c1-=0x2800;
            }

            إذا(
                (c2<=0xdbff && s2!=limit2 && U16_IS_TRAIL(*s2)) ||
                (U16_IS_TRAIL(c2) && start2!=(s2-1) && U16_IS_LEAD(*(s2-2)))
            ) {
                /* part of a surrogate pair, leave >=d800 */
            } وإلا {
                /* BMP code point - may be surrogate code point - make <d800 */
                c2-=0x2800;
            }
        }

        cmpRes=c1-c2;
        اكسر;
    }

    إذا(سمون_Len1) {
        *سمون_Len1=static_cast<int32_t>(m1-org1);
        *سمون_Len2=static_cast<int32_t>(m2-org2);
    }
    أرجع cmpRes;
}

/* internal function */
U_CFUNC int32_t
u_strcmpFold(ثابت char16_t *s1, int32_t length1,
             ثابت char16_t *s2, int32_t length2,
             uint32_t options,
             UError_کوډ *p_Error_کوډ) {
    أرجع _cmpFold(s1, length1, s2, length2, options, تش_اشاره, تش_اشاره, p_Error_کوډ);
}

/* public API functions */

U_CAPI int32_t U_EXPORT2
u_strCaseCompare(ثابت char16_t *s1, int32_t length1,
                 ثابت char16_t *s2, int32_t length2,
                 uint32_t options,
                 UError_کوډ *p_Error_کوډ) {
    /* argument checking */
    إذا (p_Error_کوډ == تش_اشاره || U_FAILURE(*p_Error_کوډ)) {
        أرجع 0;
    }
    إذا(s1==تش_اشاره || length1<-1 || s2==تش_اشاره || length2<-1) {
        *p_Error_کوډ=U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }
    أرجع u_strcmpFold(s1, length1, s2, length2,
                        options|U_COMPARE_IGNORE_CASE,
                        p_Error_کوډ);
}

U_CAPI int32_t U_EXPORT2
u_strcasecmp(ثابت char16_t *s1, ثابت char16_t *s2, uint32_t options) {
    UError_کوډ error_کوډ=U_ZERO_ERROR;
    أرجع u_strcmpFold(s1, -1, s2, -1,
                        options|U_COMPARE_IGNORE_CASE,
                        &error_کوډ);
}

U_CAPI int32_t U_EXPORT2
u_memcasecmp(ثابت char16_t *s1, ثابت char16_t *s2, int32_t length, uint32_t options) {
    UError_کوډ error_کوډ=U_ZERO_ERROR;
    أرجع u_strcmpFold(s1, length, s2, length,
                        options|U_COMPARE_IGNORE_CASE,
                        &error_کوډ);
}

U_CAPI int32_t U_EXPORT2
u_strncasecmp(ثابت char16_t *s1, ثابت char16_t *s2, int32_t n, uint32_t options) {
    UError_کوډ error_کوډ=U_ZERO_ERROR;
    أرجع u_strcmpFold(s1, n, s2, n,
                        options|(U_COMPARE_IGNORE_CASE|_STRNCMP_STYLE),
                        &error_کوډ);
}

/* internal API - detect length of shared prefix */
U_CAPI فراغ
u_case_Insensitive_مختاړی_سمون(ثابت char16_t *s1, int32_t length1,
                             ثابت char16_t *s2, int32_t length2,
                             uint32_t options,
                             int32_t *سمون_Len1, int32_t *سمون_Len2,
                             UError_کوډ *p_Error_کوډ) {
    _cmpFold(s1, length1, s2, length2, options,
        سمون_Len1, سمون_Len2, p_Error_کوډ);
}
