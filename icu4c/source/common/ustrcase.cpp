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
#وصف_ڪريو ACUTE u'\u0301'

U_NAMESPACE_BEGIN

نطاق {

int32_t check_حد_کان_مٿي_And_Edits_Error(int32_t dest_اشاريو, int32_t dest_صلاحيت,
                                   Edits *edits, UError_ڪوڊ &error_ڪوڊ) {
    إذا (U_SUCCESS(error_ڪوڊ)) {
        إذا (dest_اشاريو > dest_صلاحيت) {
            error_ڪوڊ = U_بفر_حد_کان_مٿي_ERROR;
        } وإلا إذا (edits != خالي_اشارو) {
            edits->copy_Error_منزل(error_ڪوڊ);
        }
    }
    أرجع dest_اشاريو;
}

/* Appends a full case mapping result, see UCASE_MAX_STRING_LENGTH. */
مضمن int32_t
append_نتيجو(char16_t *dest, int32_t dest_اشاريو, int32_t dest_صلاحيت,
             int32_t نتيجو, ثابت char16_t *s,
             int32_t cp_ڊيگهائي, uint32_t options, icu::Edits *edits) {
    UChar32 c;
    int32_t length;

    /* decode the result */
    إذا(نتيجو<0) {
        /* (not) original code point */
        إذا(edits!=خالي_اشارو) {
            edits->جوڙيو_Unchanged(cp_ڊيگهائي);
        }
        إذا(options & U_OMIT_UNCHANGED_متن) {
            أرجع dest_اشاريو;
        }
        c=~نتيجو;
        إذا(dest_اشاريو<dest_صلاحيت && c<=0xffff) {  // BMP slightly-fastpath
            dest[dest_اشاريو++] = static_cast<char16_t>(c);
            أرجع dest_اشاريو;
        }
        length=cp_ڊيگهائي;
    } وإلا {
        إذا(نتيجو<=UCASE_MAX_STRING_ڊيگهائي) {
            c=U_SENTINEL;
            length=نتيجو;
        } وإلا إذا(dest_اشاريو<dest_صلاحيت && نتيجو<=0xffff) {  // BMP slightly-fastpath
            dest[dest_اشاريو++] = static_cast<char16_t>(نتيجو);
            إذا(edits!=خالي_اشارو) {
                edits->جوڙيو_Replace(cp_ڊيگهائي, 1);
            }
            أرجع dest_اشاريو;
        } وإلا {
            c=نتيجو;
            length=U16_ڊيگهائي(c);
        }
        إذا(edits!=خالي_اشارو) {
            edits->جوڙيو_Replace(cp_ڊيگهائي, length);
        }
    }
    إذا(length>(INT32_MAX-dest_اشاريو)) {
        أرجع -1;  // integer overflow
    }

    إذا(dest_اشاريو<dest_صلاحيت) {
        /* append the result */
        إذا(c>=0) {
            /* code point */
            UBool isError=خطأ;
            U16_APPEND(dest, dest_اشاريو, dest_صلاحيت, c, isError);
            إذا(isError) {
                /* overflow, nothing written */
                dest_اشاريو+=length;
            }
        } وإلا {
            /* string */
            إذا((dest_اشاريو+length)<=dest_صلاحيت) {
                طالما(length>0) {
                    dest[dest_اشاريو++]=*s++;
                    --length;
                }
            } وإلا {
                /*حد_کان_مٿي*/
                dest_اشاريو+=length;
            }
        }
    } وإلا {
        /* preflight */
        dest_اشاريو+=length;
    }
    أرجع dest_اشاريو;
}

مضمن int32_t
appendUChar(char16_t *dest, int32_t dest_اشاريو, int32_t dest_صلاحيت, char16_t c) {
    إذا(dest_اشاريو<dest_صلاحيت) {
        dest[dest_اشاريو]=c;
    } وإلا إذا(dest_اشاريو==INT32_MAX) {
        أرجع -1;  // integer overflow
    }
    أرجع dest_اشاريو+1;
}

int32_t
append_Non_خالي_جايون_Unchanged(char16_t *dest, int32_t dest_اشاريو, int32_t dest_صلاحيت,
                        ثابت char16_t *s, int32_t length, uint32_t options, icu::Edits *edits) {
    إذا(edits!=خالي_اشارو) {
        edits->جوڙيو_Unchanged(length);
    }
    إذا(options & U_OMIT_UNCHANGED_متن) {
        أرجع dest_اشاريو;
    }
    إذا(length>(INT32_MAX-dest_اشاريو)) {
        أرجع -1;  // integer overflow
    }
    إذا((dest_اشاريو+length)<=dest_صلاحيت) {
        u_memcpy(dest+dest_اشاريو, s, length);
    }
    أرجع dest_اشاريو + length;
}

مضمن int32_t
appendUnchanged(char16_t *dest, int32_t dest_اشاريو, int32_t dest_صلاحيت,
                ثابت char16_t *s, int32_t length, uint32_t options, icu::Edits *edits) {
    إذا (length <= 0) {
        أرجع dest_اشاريو;
    }
    أرجع append_Non_خالي_جايون_Unchanged(dest, dest_اشاريو, dest_صلاحيت, s, length, options, edits);
}

UChar32 U_CALLCONV
utf16_caseContextIterator(فراغ *context, int8_t dir) {
    UCaseContext* csc = static_cast<UCaseContext*>(context);
    UChar32 c;

    إذا(dir<0) {
        /* reset for backward iteration */
        csc->اشاريو=csc->cp_شروعات;
        csc->dir=dir;
    } وإلا إذا(dir>0) {
        /* reset for forward iteration */
        csc->اشاريو=csc->cp_حد_بندي;
        csc->dir=dir;
    } وإلا {
        /* continue current iteration direction */
        dir=csc->dir;
    }

    إذا(dir<0) {
        إذا(csc->شروعات<csc->اشاريو) {
            U16_PREV((ثابت char16_t *)csc->p, csc->شروعات, csc->اشاريو, c);
            أرجع c;
        }
    } وإلا {
        إذا(csc->اشاريو<csc->حد_بندي) {
            U16_ايندڙ((ثابت char16_t *)csc->p, csc->اشاريو, csc->حد_بندي, c);
            أرجع c;
        }
    }
    أرجع U_SENTINEL;
}

/**
 * caseLocale >= 0: Lowercases [srcStart..srcLimit[ but takes context [0..srcLength[ into account.
 * caseLocale < 0: Case-folds [srcStart..srcLimit[.
 */
int32_t منزل_هيٺاهون(int32_t caseLocale, uint32_t options,
                char16_t *dest, int32_t dest_صلاحيت,
                ثابت char16_t *src, UCaseContext *csc, int32_t src_شروعات, int32_t src_حد_بندي,
                icu::Edits *edits, UError_ڪوڊ &error_ڪوڊ) {
    ثابت int8_t *latin_منزل_هيٺاهون;
    إذا (caseLocale == UCASE_LOC_پاڙ ||
            (caseLocale >= 0 ?
                !(caseLocale == UCASE_LOC_TURKISH || caseLocale == UCASE_LOC_LITHUANIAN) :
                (options & FOLD_CASE_OPTIONS_نقاب) == U_FOLD_CASE_DEFAULT)) {
        latin_منزل_هيٺاهون = LatinCase::منزل_هيٺاهون_عام;
    } وإلا {
        latin_منزل_هيٺاهون = LatinCase::منزل_هيٺاهون_TR_LT;
    }
    ثابت UTrie2 *trie = ucase_getTrie();
    int32_t dest_اشاريو = 0;
    int32_t prev = src_شروعات;
    int32_t src_اشاريو = src_شروعات;
    لكل (;;) {
        // fast path for simple cases
        char16_t lead = 0;
        طالما (src_اشاريو < src_حد_بندي) {
            lead = src[src_اشاريو];
            int32_t delta;
            إذا (lead < LatinCase::LONG_S) {
                int8_t d = latin_منزل_هيٺاهون[lead];
                إذا (d == LatinCase::EXC) { اكسر; }
                ++src_اشاريو;
                إذا (d == 0) { استمر; }
                delta = d;
            } وإلا إذا (lead >= 0xd800) {
                اكسر;  // surrogate or higher
            } وإلا {
                uint16_t props = UTRIE2_GET16_FROM_U16_SINGLE_LEAD(trie, lead);
                إذا (UCASE_HAS_EXCEPTION(props)) { اكسر; }
                ++src_اشاريو;
                إذا (!UCASE_IS_مٿانهون_OR_TITLE(props) || (delta = UCASE_GET_DELTA(props)) == 0) {
                    استمر;
                }
            }
            lead += static_cast<char16_t>(delta);
            dest_اشاريو = appendUnchanged(dest, dest_اشاريو, dest_صلاحيت,
                                        src + prev, src_اشاريو - 1 - prev, options, edits);
            إذا (dest_اشاريو >= 0) {
                dest_اشاريو = appendUChar(dest, dest_اشاريو, dest_صلاحيت, lead);
                إذا (edits != خالي_اشارو) {
                    edits->جوڙيو_Replace(1, 1);
                }
            }
            إذا (dest_اشاريو < 0) {
                error_ڪوڊ = U_اشاريو_OUTOFBOUNDS_ERROR;
                أرجع 0;
            }
            prev = src_اشاريو;
        }
        إذا (src_اشاريو >= src_حد_بندي) {
            اكسر;
        }
        // slow path
        int32_t cp_شروعات = src_اشاريو++;
        char16_t trail;
        UChar32 c;
        إذا (U16_IS_LEAD(lead) && src_اشاريو < src_حد_بندي && U16_IS_TRAIL(trail = src[src_اشاريو])) {
            c = U16_GET_SUPPLEMENTARY(lead, trail);
            ++src_اشاريو;
        } وإلا {
            c = lead;
        }
        ثابت char16_t *s = خالي_اشارو;
        إذا (caseLocale >= 0) {
            csc->cp_شروعات = cp_شروعات;
            csc->cp_حد_بندي = src_اشاريو;
            c = ucase_منزل_ڀريل_هيٺاهون(c, utf16_caseContextIterator, csc, &s, caseLocale);
        } وإلا {
            c = ucase_منزل_ڀريل_Folding(c, &s, options);
        }
        إذا (c >= 0) {
            dest_اشاريو = appendUnchanged(dest, dest_اشاريو, dest_صلاحيت,
                                        src + prev, cp_شروعات - prev, options, edits);
            إذا (dest_اشاريو >= 0) {
                dest_اشاريو = append_نتيجو(dest, dest_اشاريو, dest_صلاحيت, c, s,
                                         src_اشاريو - cp_شروعات, options, edits);
            }
            إذا (dest_اشاريو < 0) {
                error_ڪوڊ = U_اشاريو_OUTOFBOUNDS_ERROR;
                أرجع 0;
            }
            prev = src_اشاريو;
        }
    }
    dest_اشاريو = appendUnchanged(dest, dest_اشاريو, dest_صلاحيت,
                                src + prev, src_اشاريو - prev, options, edits);
    إذا (dest_اشاريو < 0) {
        error_ڪوڊ = U_اشاريو_OUTOFBOUNDS_ERROR;
        أرجع 0;
    }
    أرجع dest_اشاريو;
}

int32_t منزل_مٿانهون(int32_t caseLocale, uint32_t options,
                char16_t *dest, int32_t dest_صلاحيت,
                ثابت char16_t *src, UCaseContext *csc, int32_t src_ڊيگهائي,
                icu::Edits *edits, UError_ڪوڊ &error_ڪوڊ) {
    ثابت int8_t *latin_منزل_مٿانهون;
    إذا (caseLocale == UCASE_LOC_TURKISH) {
        latin_منزل_مٿانهون = LatinCase::منزل_مٿانهون_TR;
    } وإلا {
        latin_منزل_مٿانهون = LatinCase::منزل_مٿانهون_عام;
    }
    ثابت UTrie2 *trie = ucase_getTrie();
    int32_t dest_اشاريو = 0;
    int32_t prev = 0;
    int32_t src_اشاريو = 0;
    لكل (;;) {
        // fast path for simple cases
        char16_t lead = 0;
        طالما (src_اشاريو < src_ڊيگهائي) {
            lead = src[src_اشاريو];
            int32_t delta;
            إذا (lead < LatinCase::LONG_S) {
                int8_t d = latin_منزل_مٿانهون[lead];
                إذا (d == LatinCase::EXC) { اكسر; }
                ++src_اشاريو;
                إذا (d == 0) { استمر; }
                delta = d;
            } وإلا إذا (lead >= 0xd800) {
                اكسر;  // surrogate or higher
            } وإلا {
                uint16_t props = UTRIE2_GET16_FROM_U16_SINGLE_LEAD(trie, lead);
                إذا (UCASE_HAS_EXCEPTION(props)) { اكسر; }
                ++src_اشاريو;
                إذا (UCASE_GET_TYPE(props) != UCASE_هيٺاهون || (delta = UCASE_GET_DELTA(props)) == 0) {
                    استمر;
                }
            }
            lead += static_cast<char16_t>(delta);
            dest_اشاريو = appendUnchanged(dest, dest_اشاريو, dest_صلاحيت,
                                        src + prev, src_اشاريو - 1 - prev, options, edits);
            إذا (dest_اشاريو >= 0) {
                dest_اشاريو = appendUChar(dest, dest_اشاريو, dest_صلاحيت, lead);
                إذا (edits != خالي_اشارو) {
                    edits->جوڙيو_Replace(1, 1);
                }
            }
            إذا (dest_اشاريو < 0) {
                error_ڪوڊ = U_اشاريو_OUTOFBOUNDS_ERROR;
                أرجع 0;
            }
            prev = src_اشاريو;
        }
        إذا (src_اشاريو >= src_ڊيگهائي) {
            اكسر;
        }
        // slow path
        int32_t cp_شروعات;
        csc->cp_شروعات = cp_شروعات = src_اشاريو++;
        char16_t trail;
        UChar32 c;
        إذا (U16_IS_LEAD(lead) && src_اشاريو < src_ڊيگهائي && U16_IS_TRAIL(trail = src[src_اشاريو])) {
            c = U16_GET_SUPPLEMENTARY(lead, trail);
            ++src_اشاريو;
        } وإلا {
            c = lead;
        }
        csc->cp_حد_بندي = src_اشاريو;
        ثابت char16_t *s = خالي_اشارو;
        c = ucase_منزل_ڀريل_مٿانهون(c, utf16_caseContextIterator, csc, &s, caseLocale);
        إذا (c >= 0) {
            dest_اشاريو = appendUnchanged(dest, dest_اشاريو, dest_صلاحيت,
                                        src + prev, cp_شروعات - prev, options, edits);
            إذا (dest_اشاريو >= 0) {
                dest_اشاريو = append_نتيجو(dest, dest_اشاريو, dest_صلاحيت, c, s,
                                         src_اشاريو - cp_شروعات, options, edits);
            }
            إذا (dest_اشاريو < 0) {
                error_ڪوڊ = U_اشاريو_OUTOFBOUNDS_ERROR;
                أرجع 0;
            }
            prev = src_اشاريو;
        }
    }
    dest_اشاريو = appendUnchanged(dest, dest_اشاريو, dest_صلاحيت,
                                src + prev, src_اشاريو - prev, options, edits);
    إذا (dest_اشاريو < 0) {
        error_ڪوڊ = U_اشاريو_OUTOFBOUNDS_ERROR;
        أرجع 0;
    }
    أرجع dest_اشاريو;
}

}  // namespace

U_NAMESPACE_پڇاڙي

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
int32_t maybeTitleDutchIJ(ثابت char16_t *src, UChar32 c, int32_t شروعات, int32_t segment_حد_بندي,
                          char16_t *dest, int32_t &dest_اشاريو, int32_t dest_صلاحيت, uint32_t options,
                          icu::Edits *edits) {
    U_ASSERT(شروعات < segment_حد_بندي);

    int32_t اشاريو = شروعات;
    منطقي withAcute = خطأ;

    // If the conditions are met, then the following variables tell us what to output.
    int32_t unchanged1 = 0;  // code units before the j, or the whole sequence (0..3)
    منطقي doTitleJ = خطأ;  // true if the j needs to be titlecased
    int32_t unchanged2 = 0;  // after the j (0 or 1)

    // next character after the first letter
    char16_t c2 = src[اشاريو++];

    // Is the first letter an i/I with accent?
    إذا (c == u'I') {
        إذا (c2 == ACUTE) {
            withAcute = سچ;
            unchanged1 = 1;
            إذا (اشاريو == segment_حد_بندي) { أرجع شروعات; }
            c2 = src[اشاريو++];
        }
    } وإلا {  // Í
        withAcute = سچ;
    }

    // Is the next character a j/J?
    إذا (c2 == u'j') {
        doTitleJ = سچ;
    } وإلا إذا (c2 == u'J') {
        ++unchanged1;
    } وإلا {
        أرجع شروعات;
    }

    // A plain i/I must be followed by a plain j/J.
    // An i/I with acute must be followed by a j/J with acute.
    إذا (withAcute) {
        إذا (اشاريو == segment_حد_بندي || src[اشاريو++] != ACUTE) { أرجع شروعات; }
        إذا (doTitleJ) {
            unchanged2 = 1;
        } وإلا {
            ++unchanged1;
        }
    }

    // There must not be another combining mark.
    إذا (اشاريو < segment_حد_بندي) {
        int32_t cp;
        int32_t i = اشاريو;
        U16_ايندڙ(src, i, segment_حد_بندي, cp);
        uint32_t type_نقاب = U_GET_GC_نقاب(cp);
        إذا ((type_نقاب & U_GC_M_نقاب) != 0) {
            أرجع شروعات;
        }
    }

    // Output the rest of the Dutch IJ.
    dest_اشاريو = appendUnchanged(dest, dest_اشاريو, dest_صلاحيت, src + شروعات, unchanged1, options, edits);
    شروعات += unchanged1;
    إذا (doTitleJ) {
        dest_اشاريو = appendUChar(dest, dest_اشاريو, dest_صلاحيت, u'J');
        إذا (edits != خالي_اشارو) {
            edits->جوڙيو_Replace(1, 1);
        }
        ++شروعات;
    }
    dest_اشاريو = appendUnchanged(dest, dest_اشاريو, dest_صلاحيت, src + شروعات, unchanged2, options, edits);

    U_ASSERT(شروعات + unchanged2 == اشاريو);
    أرجع اشاريو;
}

}  // namespace

U_CFUNC int32_t U_CALLCONV
ustrcase_اندروني_منزل_Title(int32_t caseLocale, uint32_t options, BreakIterator *iter,
                         char16_t *dest, int32_t dest_صلاحيت,
                         ثابت char16_t *src, int32_t src_ڊيگهائي,
                         icu::Edits *edits,
                         UError_ڪوڊ &error_ڪوڊ) {
    إذا (!ustrcase_checkTitleAdjustmentOptions(options, error_ڪوڊ)) {
        أرجع 0;
    }

    /* set up local variables */
    UCaseContext csc=UCASECONTEXT_INITIALIZER;
    csc.p=(فراغ *)src;
    csc.حد_بندي=src_ڊيگهائي;
    int32_t dest_اشاريو=0;
    int32_t prev=0;
    منطقي is_پهريون_اشاريو=سچ;

    /* titlecasing loop */
    طالما(prev<src_ڊيگهائي) {
        /* find next index where to titlecase */
        int32_t اشاريو;
        إذا(is_پهريون_اشاريو) {
            is_پهريون_اشاريو=خطأ;
            اشاريو=iter->first();
        } وإلا {
            اشاريو=iter->next();
        }
        إذا(اشاريو==UBRK_پورو || اشاريو>src_ڊيگهائي) {
            اشاريو=src_ڊيگهائي;
        }

        /*
         * Segment [prev..index[ into 3 parts:
         * a) skipped characters (copy as-is) [prev..titleStart[
         * b) first letter (titlecase)              [titleStart..titleLimit[
         * c) subsequent characters (lowercase)                 [titleLimit..index[
         */
        إذا(prev<اشاريو) {
            // Find and copy skipped characters [prev..titleStart[
            int32_t title_شروعات=prev;
            int32_t title_حد_بندي=prev;
            UChar32 c;
            U16_ايندڙ(src, title_حد_بندي, اشاريو, c);
            إذا ((options&U_TITLECASE_NO_BREAK_ADJUSTMENT)==0) {
                // Adjust the titlecasing index to the next cased character,
                // or to the next letter/number/symbol/private use.
                // Stop with titleStart<titleLimit<=index
                // if there is a character to be titlecased,
                // or else stop with titleStart==titleLimit==index.
                منطقي منزل_Cased = (options&U_TITLECASE_ADJUST_منزل_CASED) != 0;
                طالما (منزل_Cased ? UCASE_NONE==ucase_getType(c) : !ustrcase_isLNS(c)) {
                    title_شروعات=title_حد_بندي;
                    إذا(title_حد_بندي==اشاريو) {
                        اكسر;
                    }
                    U16_ايندڙ(src, title_حد_بندي, اشاريو, c);
                }
                إذا (prev < title_شروعات) {
                    dest_اشاريو=appendUnchanged(dest, dest_اشاريو, dest_صلاحيت,
                                              src+prev, title_شروعات-prev, options, edits);
                    إذا(dest_اشاريو<0) {
                        error_ڪوڊ=U_اشاريو_OUTOFBOUNDS_ERROR;
                        أرجع 0;
                    }
                }
            }

            إذا(title_شروعات<title_حد_بندي) {
                /* titlecase c which is from [titleStart..titleLimit[ */
                csc.cp_شروعات=title_شروعات;
                csc.cp_حد_بندي=title_حد_بندي;
                ثابت char16_t *s;
                c=ucase_منزل_ڀريل_Title(c, utf16_caseContextIterator, &csc, &s, caseLocale);
                dest_اشاريو=append_نتيجو(dest, dest_اشاريو, dest_صلاحيت, c, s,
                                       title_حد_بندي-title_شروعات, options, edits);
                إذا(dest_اشاريو<0) {
                    error_ڪوڊ=U_اشاريو_OUTOFBOUNDS_ERROR;
                    أرجع 0;
                }

                /* Special case Dutch IJ titlecasing */
                إذا (title_شروعات+1 < اشاريو &&
                        caseLocale == UCASE_LOC_DUTCH) {
                    إذا (c < 0) {
                        c = ~c;
                    }

                    إذا (c == u'I' || c == u'Í') {
                        title_حد_بندي = maybeTitleDutchIJ(src, c, title_شروعات + 1, اشاريو,
                                                       dest, dest_اشاريو, dest_صلاحيت, options,
                                                       edits);
                    }
                }

                /* lowercase [titleLimit..index[ */
                إذا(title_حد_بندي<اشاريو) {
                    إذا((options&U_TITLECASE_NO_LOWERCASE)==0) {
                        /* Normal operation: Lowercase the rest of the word. */
                        dest_اشاريو+=
                            منزل_هيٺاهون(
                                caseLocale, options,
                                (dest==خالي_اشارو) ? خالي_اشارو: dest+dest_اشاريو, dest_صلاحيت-dest_اشاريو,
                                src, &csc, title_حد_بندي, اشاريو,
                                edits, error_ڪوڊ);
                        إذا(error_ڪوڊ==U_بفر_حد_کان_مٿي_ERROR) {
                            error_ڪوڊ=U_ZERO_ERROR;
                        }
                        إذا(U_FAILURE(error_ڪوڊ)) {
                            أرجع dest_اشاريو;
                        }
                    } وإلا {
                        /* Optionally just copy the rest of the word unchanged. */
                        dest_اشاريو=appendUnchanged(dest, dest_اشاريو, dest_صلاحيت,
                                                  src+title_حد_بندي, اشاريو-title_حد_بندي, options, edits);
                        إذا(dest_اشاريو<0) {
                            error_ڪوڊ=U_اشاريو_OUTOFBOUNDS_ERROR;
                            أرجع 0;
                        }
                    }
                }
            }
        }

        prev=اشاريو;
    }

    أرجع check_حد_کان_مٿي_And_Edits_Error(dest_اشاريو, dest_صلاحيت, edits, error_ڪوڊ);
}

#جيڪڏهن_پورو  // !UCONFIG_NO_BREAK_ITERATION

U_NAMESPACE_BEGIN
نطاق Greek_مٿانهون {

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
        U16_ايندڙ(s, i, length, c);
        int32_t type = ucase_getTypeOrIgnorable(c);
        إذا ((type & UCASE_IGNORABLE) != 0) {
            // Case-ignorable, continue with the loop.
        } وإلا إذا (type != UCASE_NONE) {
            أرجع سچ;  // Followed by cased letter.
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
int32_t منزل_مٿانهون(uint32_t options,
                char16_t *dest, int32_t dest_صلاحيت,
                ثابت char16_t *src, int32_t src_ڊيگهائي,
                Edits *edits,
                UError_ڪوڊ &error_ڪوڊ) {
    int32_t dest_اشاريو=0;
    uint32_t state = 0;
    لكل (int32_t i = 0; i < src_ڊيگهائي;) {
        int32_t ايندڙ_اشاريو = i;
        UChar32 c;
        U16_ايندڙ(src, ايندڙ_اشاريو, src_ڊيگهائي, c);
        uint32_t ايندڙ_State = 0;
        int32_t type = ucase_getTypeOrIgnorable(c);
        إذا ((type & UCASE_IGNORABLE) != 0) {
            // c is case-ignorable
            ايندڙ_State |= (state & AFTER_CASED);
        } وإلا إذا (type != UCASE_NONE) {
            // c is cased
            ايندڙ_State |= AFTER_CASED;
        }
        uint32_t معلومات = get_Letter_معلومات(c);
        إذا (معلومات > 0) {
            uint32_t upper = معلومات & مٿانهون_نقاب;
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
            int32_t انگ_Ypogegrammeni = 0;  // Map each one to a trailing, spacing, capital iota.
            إذا ((معلومات & HAS_YPOGEGRAMMENI) != 0) {
                انگ_Ypogegrammeni = 1;
            }
            ثابت UBool hasPrecomposedAccent = (معلومات & HAS_ACCENT) != 0;
            // Skip combining diacritics after this Greek letter.
            طالما (ايندڙ_اشاريو < src_ڊيگهائي) {
                uint32_t diacritic_معلومات = get_Diacritic_معلومات(src[ايندڙ_اشاريو]);
                إذا (diacritic_معلومات != 0) {
                    معلومات |= diacritic_معلومات;
                    إذا ((diacritic_معلومات & HAS_YPOGEGRAMMENI) != 0) {
                        ++انگ_Ypogegrammeni;
                    }
                    ++ايندڙ_اشاريو;
                } وإلا {
                    اكسر;  // not a Greek diacritic
                }
            }
            إذا ((معلومات & HAS_VOWEL_AND_ACCENT_AND_DIALYTIKA) == HAS_VOWEL_AND_ACCENT) {
                ايندڙ_State |= hasPrecomposedAccent ? AFTER_VOWEL_WITH_PRECOMPOSED_ACCENT
                                                  : AFTER_VOWEL_WITH_COMBINING_ACCENT;
            }
            // Map according to Greek rules.
            UBool جوڙيو_Tonos = خطأ;
            إذا (upper == 0x397 &&
                    (معلومات & HAS_ACCENT) != 0 &&
                    انگ_Ypogegrammeni == 0 &&
                    (state & AFTER_CASED) == 0 &&
                    !isFollowedByCasedLetter(src, ايندڙ_اشاريو, src_ڊيگهائي)) {
                // Keep disjunctive "or" with (only) a tonos.
                // We use the same "word boundary" conditions as for the Final_Sigma test.
                إذا (hasPrecomposedAccent) {
                    upper = 0x389;  // Preserve the precomposed form.
                } وإلا {
                    جوڙيو_Tonos = سچ;
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
            إذا (edits == خالي_اشارو && (options & U_OMIT_UNCHANGED_متن) == 0) {
                change = سچ;  // common, simple usage
            } وإلا {
                // Find out first whether we are changing the text.
                change = src[i] != upper || انگ_Ypogegrammeni > 0;
                int32_t i2 = i + 1;
                إذا ((معلومات & HAS_EITHER_DIALYTIKA) != 0) {
                    change |= i2 >= ايندڙ_اشاريو || src[i2] != 0x308;
                    ++i2;
                }
                إذا (جوڙيو_Tonos) {
                    change |= i2 >= ايندڙ_اشاريو || src[i2] != 0x301;
                    ++i2;
                }
                int32_t old_ڊيگهائي = ايندڙ_اشاريو - i;
                int32_t new_ڊيگهائي = (i2 - i) + انگ_Ypogegrammeni;
                change |= old_ڊيگهائي != new_ڊيگهائي;
                إذا (change) {
                    إذا (edits != خالي_اشارو) {
                        edits->جوڙيو_Replace(old_ڊيگهائي, new_ڊيگهائي);
                    }
                } وإلا {
                    إذا (edits != خالي_اشارو) {
                        edits->جوڙيو_Unchanged(old_ڊيگهائي);
                    }
                    // Write unchanged text?
                    change = (options & U_OMIT_UNCHANGED_متن) == 0;
                }
            }

            إذا (change) {
                dest_اشاريو = appendUChar(dest, dest_اشاريو, dest_صلاحيت, static_cast<char16_t>(upper));
                إذا (dest_اشاريو >= 0 && (معلومات & HAS_EITHER_DIALYTIKA) != 0) {
                    dest_اشاريو=appendUChar(dest, dest_اشاريو, dest_صلاحيت, 0x308);  // restore or add a dialytika
                }
                إذا (dest_اشاريو >= 0 && جوڙيو_Tonos) {
                    dest_اشاريو=appendUChar(dest, dest_اشاريو, dest_صلاحيت, 0x301);
                }
                طالما (dest_اشاريو >= 0 && انگ_Ypogegrammeni > 0) {
                    dest_اشاريو=appendUChar(dest, dest_اشاريو, dest_صلاحيت, 0x399);
                    --انگ_Ypogegrammeni;
                }
                إذا(dest_اشاريو<0) {
                    error_ڪوڊ=U_اشاريو_OUTOFBOUNDS_ERROR;
                    أرجع 0;
                }
            }
        } وإلا {
            ثابت char16_t *s;
            c=ucase_منزل_ڀريل_مٿانهون(c, خالي_اشارو, خالي_اشارو, &s, UCASE_LOC_GREEK);
            dest_اشاريو = append_نتيجو(dest, dest_اشاريو, dest_صلاحيت, c, s,
                                     ايندڙ_اشاريو - i, options, edits);
            إذا (dest_اشاريو < 0) {
                error_ڪوڊ = U_اشاريو_OUTOFBOUNDS_ERROR;
                أرجع 0;
            }
        }
        i = ايندڙ_اشاريو;
        state = ايندڙ_State;
    }

    أرجع dest_اشاريو;
}

}  // namespace GreekUpper
U_NAMESPACE_پڇاڙي

/* functions available in the common library (for unistr_case.cpp) */

U_CFUNC int32_t U_CALLCONV
ustrcase_اندروني_منزل_هيٺاهون(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_اڻ_استعمال_ٿيل
                         char16_t *dest, int32_t dest_صلاحيت,
                         ثابت char16_t *src, int32_t src_ڊيگهائي,
                         icu::Edits *edits,
                         UError_ڪوڊ &error_ڪوڊ) {
    UCaseContext csc=UCASECONTEXT_INITIALIZER;
    csc.p=(فراغ *)src;
    csc.حد_بندي=src_ڊيگهائي;
    int32_t dest_اشاريو = منزل_هيٺاهون(
        caseLocale, options,
        dest, dest_صلاحيت,
        src, &csc, 0, src_ڊيگهائي,
        edits, error_ڪوڊ);
    أرجع check_حد_کان_مٿي_And_Edits_Error(dest_اشاريو, dest_صلاحيت, edits, error_ڪوڊ);
}

U_CFUNC int32_t U_CALLCONV
ustrcase_اندروني_منزل_مٿانهون(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_اڻ_استعمال_ٿيل
                         char16_t *dest, int32_t dest_صلاحيت,
                         ثابت char16_t *src, int32_t src_ڊيگهائي,
                         icu::Edits *edits,
                         UError_ڪوڊ &error_ڪوڊ) {
    int32_t dest_اشاريو;
    إذا (caseLocale == UCASE_LOC_GREEK) {
        dest_اشاريو = Greek_مٿانهون::منزل_مٿانهون(options, dest, dest_صلاحيت,
                                        src, src_ڊيگهائي, edits, error_ڪوڊ);
    } وإلا {
        UCaseContext csc=UCASECONTEXT_INITIALIZER;
        csc.p=(فراغ *)src;
        csc.حد_بندي=src_ڊيگهائي;
        dest_اشاريو = منزل_مٿانهون(
            caseLocale, options,
            dest, dest_صلاحيت,
            src, &csc, src_ڊيگهائي,
            edits, error_ڪوڊ);
    }
    أرجع check_حد_کان_مٿي_And_Edits_Error(dest_اشاريو, dest_صلاحيت, edits, error_ڪوڊ);
}

U_CFUNC int32_t U_CALLCONV
ustrcase_اندروني_Fold(int32_t /* caseLocale */, uint32_t options, UCASEMAP_BREAK_ITERATOR_اڻ_استعمال_ٿيل
                      char16_t *dest, int32_t dest_صلاحيت,
                      ثابت char16_t *src, int32_t src_ڊيگهائي,
                      icu::Edits *edits,
                      UError_ڪوڊ &error_ڪوڊ) {
    int32_t dest_اشاريو = منزل_هيٺاهون(
        -1, options,
        dest, dest_صلاحيت,
        src, خالي_اشارو, 0, src_ڊيگهائي,
        edits, error_ڪوڊ);
    أرجع check_حد_کان_مٿي_And_Edits_Error(dest_اشاريو, dest_صلاحيت, edits, error_ڪوڊ);
}

U_CFUNC int32_t
ustrcase_map(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_PARAM
             char16_t *dest, int32_t dest_صلاحيت,
             ثابت char16_t *src, int32_t src_ڊيگهائي,
             UStringCaseMapper *stringCaseMapper,
             icu::Edits *edits,
             UError_ڪوڊ &error_ڪوڊ) {
    int32_t dest_ڊيگهائي;

    /* check argument values */
    إذا(U_FAILURE(error_ڪوڊ)) {
        أرجع 0;
    }
    إذا( dest_صلاحيت<0 ||
        (dest==خالي_اشارو && dest_صلاحيت>0) ||
        src==خالي_اشارو ||
        src_ڊيگهائي<-1
    ) {
        error_ڪوڊ=U_ILLEGAL_دليل_ERROR;
        أرجع 0;
    }

    /* get the string length */
    إذا(src_ڊيگهائي==-1) {
        src_ڊيگهائي=u_strlen(src);
    }

    /* check for overlapping source and destination */
    إذا( dest!=خالي_اشارو &&
        ((src>=dest && src<(dest+dest_صلاحيت)) ||
         (dest>=src && dest<(src+src_ڊيگهائي)))
    ) {
        error_ڪوڊ=U_ILLEGAL_دليل_ERROR;
        أرجع 0;
    }

    إذا (edits != خالي_اشارو && (options & U_EDITS_NO_RESET) == 0) {
        edits->reset();
    }
    dest_ڊيگهائي=stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                                dest, dest_صلاحيت, src, src_ڊيگهائي, edits, error_ڪوڊ);
    أرجع u_terminateUChars(dest, dest_صلاحيت, dest_ڊيگهائي, &error_ڪوڊ);
}

U_CFUNC int32_t
ustrcase_mapWithOverlap(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_PARAM
                        char16_t *dest, int32_t dest_صلاحيت,
                        ثابت char16_t *src, int32_t src_ڊيگهائي,
                        UStringCaseMapper *stringCaseMapper,
                        UError_ڪوڊ &error_ڪوڊ) {
    char16_t بفر[300];
    char16_t *عارضي;

    int32_t dest_ڊيگهائي;

    /* check argument values */
    إذا(U_FAILURE(error_ڪوڊ)) {
        أرجع 0;
    }
    إذا( dest_صلاحيت<0 ||
        (dest==خالي_اشارو && dest_صلاحيت>0) ||
        src==خالي_اشارو ||
        src_ڊيگهائي<-1
    ) {
        error_ڪوڊ=U_ILLEGAL_دليل_ERROR;
        أرجع 0;
    }

    /* get the string length */
    إذا(src_ڊيگهائي==-1) {
        src_ڊيگهائي=u_strlen(src);
    }

    /* check for overlapping source and destination */
    إذا( dest!=خالي_اشارو &&
        ((src>=dest && src<(dest+dest_صلاحيت)) ||
         (dest>=src && dest<(src+src_ڊيگهائي)))
    ) {
        /* overlap: provide a temporary destination buffer and later copy the result */
        إذا(dest_صلاحيت<=UPRV_LENGTHOF(بفر)) {
            /* the stack buffer is large enough */
            عارضي=بفر;
        } وإلا {
            /* allocate a buffer */
            عارضي=(char16_t *)uprv_malloc(dest_صلاحيت*U_SIZEOF_UCHAR);
            إذا(عارضي==خالي_اشارو) {
                error_ڪوڊ=U_يادگيري_ورهاست_ERROR;
                أرجع 0;
            }
        }
    } وإلا {
        عارضي=dest;
    }

    dest_ڊيگهائي=stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                                عارضي, dest_صلاحيت, src, src_ڊيگهائي, خالي_اشارو, error_ڪوڊ);
    إذا(عارضي!=dest) {
        /* copy the result string to the destination buffer */
        إذا (U_SUCCESS(error_ڪوڊ) && 0 < dest_ڊيگهائي && dest_ڊيگهائي <= dest_صلاحيت) {
            u_memmove(dest, عارضي, dest_ڊيگهائي);
        }
        إذا(عارضي!=بفر) {
            uprv_free(عارضي);
        }
    }

    أرجع u_terminateUChars(dest, dest_صلاحيت, dest_ڊيگهائي, &error_ڪوڊ);
}

/* public API functions */

U_CAPI int32_t U_EXPORT2
u_strFoldCase(char16_t *dest, int32_t dest_صلاحيت,
              ثابت char16_t *src, int32_t src_ڊيگهائي,
              uint32_t options,
              UError_ڪوڊ *p_Error_ڪوڊ) {
    أرجع ustrcase_mapWithOverlap(
        UCASE_LOC_پاڙ, options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_صلاحيت,
        src, src_ڊيگهائي,
        ustrcase_اندروني_Fold, *p_Error_ڪوڊ);
}

U_NAMESPACE_BEGIN

int32_t CaseMap::fold(
        uint32_t options,
        ثابت char16_t *src, int32_t src_ڊيگهائي,
        char16_t *dest, int32_t dest_صلاحيت, Edits *edits,
        UError_ڪوڊ &error_ڪوڊ) {
    أرجع ustrcase_map(
        UCASE_LOC_پاڙ, options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_صلاحيت,
        src, src_ڊيگهائي,
        ustrcase_اندروني_Fold, edits, error_ڪوڊ);
}

U_NAMESPACE_پڇاڙي

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
    ثابت char16_t *شروعات, *s, *حد_بندي;
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
            int32_t *ميلاپ_Len1, int32_t *ميلاپ_Len2,
            UError_ڪوڊ *p_Error_ڪوڊ) {
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
    char16_t fold1[UCASE_MAX_STRING_ڊيگهائي+1], fold2[UCASE_MAX_STRING_ڊيگهائي+1];

    /* track which is the current level per string */
    int32_t level1, level2;

    /* current code units, and code points for lookups */
    UChar32 c1, c2, cp1, cp2;

    /* no argument error checking because this itself is not an API */

    /*
     * assume that at least the option U_COMPARE_IGNORE_CASE is set
     * otherwise this function would have to behave exactly as uprv_strCompare()
     */
    إذا(U_FAILURE(*p_Error_ڪوڊ)) {
        أرجع 0;
    }

    /* initialize */
    إذا(ميلاپ_Len1) {
        U_ASSERT(ميلاپ_Len2 !=خالي_اشارو);
        *ميلاپ_Len1=0;
        *ميلاپ_Len2=0;
    }

    start1=m1=org1=s1;
    إذا(length1==-1) {
        limit1=خالي_اشارو;
    } وإلا {
        limit1=s1+length1;
    }

    start2=m2=org2=s2;
    إذا(length2==-1) {
        limit2=خالي_اشارو;
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
                إذا(s1==limit1 || ((c1=*s1)==0 && (limit1==خالي_اشارو || (options&_STRNCMP_STYLE)))) {
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
                    start1=stack1[level1].شروعات;    /*Not uninitialized*/
                } طالما(start1==خالي_اشارو);
                s1=stack1[level1].s;                /*Not uninitialized*/
                limit1=stack1[level1].حد_بندي;        /*Not uninitialized*/
            }
        }

        إذا(c2<0) {
            /* get next code unit from string 2, post-increment */
            لكل(;;) {
                إذا(s2==limit2 || ((c2=*s2)==0 && (limit2==خالي_اشارو || (options&_STRNCMP_STYLE)))) {
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
                    start2=stack2[level2].شروعات;    /*Not uninitialized*/
                } طالما(start2==خالي_اشارو);
                s2=stack2[level2].s;                /*Not uninitialized*/
                limit2=stack2[level2].حد_بندي;        /*Not uninitialized*/
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
            next1=next2=خالي_اشارو;
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

            إذا (next1!=خالي_اشارو) {
                إذا(level2==0) {
                    next2=s2;
                } وإلا إذا(s2==limit2) {
                    U_ASSERT(level2==1);

                    /* is s2 at the end of the current stack? */
                    next2=stack2[0].s;
                }
                إذا(next2!=خالي_اشارو) {
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
            (length = ucase_منزل_ڀريل_Folding(cp1, &p, options)) >= 0
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
            stack1[0].شروعات=start1;
            stack1[0].s=s1;
            stack1[0].حد_بندي=limit1;
            ++level1;

            /* copy the folding result to fold1[] */
            إذا(length<=UCASE_MAX_STRING_ڊيگهائي) {
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
            (length = ucase_منزل_ڀريل_Folding(cp2, &p, options)) >= 0
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
            stack2[0].شروعات=start2;
            stack2[0].s=s2;
            stack2[0].حد_بندي=limit2;
            ++level2;

            /* copy the folding result to fold2[] */
            إذا(length<=UCASE_MAX_STRING_ڊيگهائي) {
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

        إذا(c1>=0xd800 && c2>=0xd800 && (options&U_COMPARE_ڪوڊ_نقطو_ترتيب)) {
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

    إذا(ميلاپ_Len1) {
        *ميلاپ_Len1=static_cast<int32_t>(m1-org1);
        *ميلاپ_Len2=static_cast<int32_t>(m2-org2);
    }
    أرجع cmpRes;
}

/* internal function */
U_CFUNC int32_t
u_strcmpFold(ثابت char16_t *s1, int32_t length1,
             ثابت char16_t *s2, int32_t length2,
             uint32_t options,
             UError_ڪوڊ *p_Error_ڪوڊ) {
    أرجع _cmpFold(s1, length1, s2, length2, options, خالي_اشارو, خالي_اشارو, p_Error_ڪوڊ);
}

/* public API functions */

U_CAPI int32_t U_EXPORT2
u_strCaseCompare(ثابت char16_t *s1, int32_t length1,
                 ثابت char16_t *s2, int32_t length2,
                 uint32_t options,
                 UError_ڪوڊ *p_Error_ڪوڊ) {
    /* argument checking */
    إذا (p_Error_ڪوڊ == خالي_اشارو || U_FAILURE(*p_Error_ڪوڊ)) {
        أرجع 0;
    }
    إذا(s1==خالي_اشارو || length1<-1 || s2==خالي_اشارو || length2<-1) {
        *p_Error_ڪوڊ=U_ILLEGAL_دليل_ERROR;
        أرجع 0;
    }
    أرجع u_strcmpFold(s1, length1, s2, length2,
                        options|U_COMPARE_IGNORE_CASE,
                        p_Error_ڪوڊ);
}

U_CAPI int32_t U_EXPORT2
u_strcasecmp(ثابت char16_t *s1, ثابت char16_t *s2, uint32_t options) {
    UError_ڪوڊ error_ڪوڊ=U_ZERO_ERROR;
    أرجع u_strcmpFold(s1, -1, s2, -1,
                        options|U_COMPARE_IGNORE_CASE,
                        &error_ڪوڊ);
}

U_CAPI int32_t U_EXPORT2
u_memcasecmp(ثابت char16_t *s1, ثابت char16_t *s2, int32_t length, uint32_t options) {
    UError_ڪوڊ error_ڪوڊ=U_ZERO_ERROR;
    أرجع u_strcmpFold(s1, length, s2, length,
                        options|U_COMPARE_IGNORE_CASE,
                        &error_ڪوڊ);
}

U_CAPI int32_t U_EXPORT2
u_strncasecmp(ثابت char16_t *s1, ثابت char16_t *s2, int32_t n, uint32_t options) {
    UError_ڪوڊ error_ڪوڊ=U_ZERO_ERROR;
    أرجع u_strcmpFold(s1, n, s2, n,
                        options|(U_COMPARE_IGNORE_CASE|_STRNCMP_STYLE),
                        &error_ڪوڊ);
}

/* internal API - detect length of shared prefix */
U_CAPI فراغ
u_case_Insensitive_اڳياڙي_ميلاپ(ثابت char16_t *s1, int32_t length1,
                             ثابت char16_t *s2, int32_t length2,
                             uint32_t options,
                             int32_t *ميلاپ_Len1, int32_t *ميلاپ_Len2,
                             UError_ڪوڊ *p_Error_ڪوڊ) {
    _cmpFold(s1, length1, s2, length2, options,
        ميلاپ_Len1, ميلاپ_Len2, p_Error_ڪوڊ);
}
