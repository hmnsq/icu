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

#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "unicode/brkiter.h"
#అన్తర్భావ "unicode/casemap.h"
#అన్తర్భావ "unicode/edits.h"
#అన్తర్భావ "unicode/stringoptions.h"
#అన్తర్భావ "unicode/ustring.h"
#అన్తర్భావ "unicode/ucasemap.h"
#అన్తర్భావ "unicode/ubrk.h"
#అన్తర్భావ "unicode/utf.h"
#అన్తర్భావ "unicode/utf16.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "ucase.h"
#అన్తర్భావ "ucasemap_imp.h"
#అన్తర్భావ "ustr_imp.h"
#అన్తర్భావ "uassert.h"

/**
 * Code point for COMBINING ACUTE ACCENT
 * @internal
 */
#నిర్వచించు ACUTE u'\u0301'

U_NAMESPACE_BEGIN

నామాకాశ {

int32_t check_అధికప్రవాహం_And_Edits_Error(int32_t dest_సూచీ, int32_t dest_సామర్థ్యం,
                                   Edits *edits, UError_సంకేతం &error_సంకేతం) {
    ఒకవేళ (U_SUCCESS(error_సంకేతం)) {
        ఒకవేళ (dest_సూచీ > dest_సామర్థ్యం) {
            error_సంకేతం = U_బఫరు_అధికప్రవాహం_ERROR;
        } లేకపోతే ఒకవేళ (edits != శూన్యసూచిక) {
            edits->copy_Error_గమ్యం(error_సంకేతం);
        }
    }
    తిరిగి dest_సూచీ;
}

/* Appends a full case mapping result, see UCASE_MAX_STRING_LENGTH. */
అంతఃవరుస int32_t
append_ఫలితం(char16_t *dest, int32_t dest_సూచీ, int32_t dest_సామర్థ్యం,
             int32_t ఫలితం, స్థిరరాశి char16_t *s,
             int32_t cp_నిడివి, uint32_t options, icu::Edits *edits) {
    UChar32 c;
    int32_t length;

    /* decode the result */
    ఒకవేళ(ఫలితం<0) {
        /* (not) original code point */
        ఒకవేళ(edits!=శూన్యసూచిక) {
            edits->కూడు_Unchanged(cp_నిడివి);
        }
        ఒకవేళ(options & U_OMIT_UNCHANGED_పాఠ్యం) {
            తిరిగి dest_సూచీ;
        }
        c=~ఫలితం;
        ఒకవేళ(dest_సూచీ<dest_సామర్థ్యం && c<=0xffff) {  // BMP slightly-fastpath
            dest[dest_సూచీ++] = static_cast<char16_t>(c);
            తిరిగి dest_సూచీ;
        }
        length=cp_నిడివి;
    } లేకపోతే {
        ఒకవేళ(ఫలితం<=UCASE_MAX_STRING_నిడివి) {
            c=U_SENTINEL;
            length=ఫలితం;
        } లేకపోతే ఒకవేళ(dest_సూచీ<dest_సామర్థ్యం && ఫలితం<=0xffff) {  // BMP slightly-fastpath
            dest[dest_సూచీ++] = static_cast<char16_t>(ఫలితం);
            ఒకవేళ(edits!=శూన్యసూచిక) {
                edits->కూడు_Replace(cp_నిడివి, 1);
            }
            తిరిగి dest_సూచీ;
        } లేకపోతే {
            c=ఫలితం;
            length=U16_నిడివి(c);
        }
        ఒకవేళ(edits!=శూన్యసూచిక) {
            edits->కూడు_Replace(cp_నిడివి, length);
        }
    }
    ఒకవేళ(length>(INT32_MAX-dest_సూచీ)) {
        తిరిగి -1;  // integer overflow
    }

    ఒకవేళ(dest_సూచీ<dest_సామర్థ్యం) {
        /* append the result */
        ఒకవేళ(c>=0) {
            /* code point */
            UBool isError=అబద్ధం;
            U16_APPEND(dest, dest_సూచీ, dest_సామర్థ్యం, c, isError);
            ఒకవేళ(isError) {
                /* overflow, nothing written */
                dest_సూచీ+=length;
            }
        } లేకపోతే {
            /* string */
            ఒకవేళ((dest_సూచీ+length)<=dest_సామర్థ్యం) {
                అయితే(length>0) {
                    dest[dest_సూచీ++]=*s++;
                    --length;
                }
            } లేకపోతే {
                /*అధికప్రవాహం*/
                dest_సూచీ+=length;
            }
        }
    } లేకపోతే {
        /* preflight */
        dest_సూచీ+=length;
    }
    తిరిగి dest_సూచీ;
}

అంతఃవరుస int32_t
appendUChar(char16_t *dest, int32_t dest_సూచీ, int32_t dest_సామర్థ్యం, char16_t c) {
    ఒకవేళ(dest_సూచీ<dest_సామర్థ్యం) {
        dest[dest_సూచీ]=c;
    } లేకపోతే ఒకవేళ(dest_సూచీ==INT32_MAX) {
        తిరిగి -1;  // integer overflow
    }
    తిరిగి dest_సూచీ+1;
}

int32_t
append_Non_ఖాళీలు_Unchanged(char16_t *dest, int32_t dest_సూచీ, int32_t dest_సామర్థ్యం,
                        స్థిరరాశి char16_t *s, int32_t length, uint32_t options, icu::Edits *edits) {
    ఒకవేళ(edits!=శూన్యసూచిక) {
        edits->కూడు_Unchanged(length);
    }
    ఒకవేళ(options & U_OMIT_UNCHANGED_పాఠ్యం) {
        తిరిగి dest_సూచీ;
    }
    ఒకవేళ(length>(INT32_MAX-dest_సూచీ)) {
        తిరిగి -1;  // integer overflow
    }
    ఒకవేళ((dest_సూచీ+length)<=dest_సామర్థ్యం) {
        u_memcpy(dest+dest_సూచీ, s, length);
    }
    తిరిగి dest_సూచీ + length;
}

అంతఃవరుస int32_t
appendUnchanged(char16_t *dest, int32_t dest_సూచీ, int32_t dest_సామర్థ్యం,
                స్థిరరాశి char16_t *s, int32_t length, uint32_t options, icu::Edits *edits) {
    ఒకవేళ (length <= 0) {
        తిరిగి dest_సూచీ;
    }
    తిరిగి append_Non_ఖాళీలు_Unchanged(dest, dest_సూచీ, dest_సామర్థ్యం, s, length, options, edits);
}

UChar32 U_CALLCONV
utf16_caseContextIterator(శూన్య *context, int8_t dir) {
    UCaseContext* csc = static_cast<UCaseContext*>(context);
    UChar32 c;

    ఒకవేళ(dir<0) {
        /* reset for backward iteration */
        csc->సూచీ=csc->cp_ప్రారంభం;
        csc->dir=dir;
    } లేకపోతే ఒకవేళ(dir>0) {
        /* reset for forward iteration */
        csc->సూచీ=csc->cp_హద్దు;
        csc->dir=dir;
    } లేకపోతే {
        /* continue current iteration direction */
        dir=csc->dir;
    }

    ఒకవేళ(dir<0) {
        ఒకవేళ(csc->ప్రారంభం<csc->సూచీ) {
            U16_PREV((స్థిరరాశి char16_t *)csc->p, csc->ప్రారంభం, csc->సూచీ, c);
            తిరిగి c;
        }
    } లేకపోతే {
        ఒకవేళ(csc->సూచీ<csc->హద్దు) {
            U16_తదుపరి((స్థిరరాశి char16_t *)csc->p, csc->సూచీ, csc->హద్దు, c);
            తిరిగి c;
        }
    }
    తిరిగి U_SENTINEL;
}

/**
 * caseLocale >= 0: Lowercases [srcStart..srcLimit[ but takes context [0..srcLength[ into account.
 * caseLocale < 0: Case-folds [srcStart..srcLimit[.
 */
int32_t గమ్యం_క్రింది(int32_t caseLocale, uint32_t options,
                char16_t *dest, int32_t dest_సామర్థ్యం,
                స్థిరరాశి char16_t *src, UCaseContext *csc, int32_t src_ప్రారంభం, int32_t src_హద్దు,
                icu::Edits *edits, UError_సంకేతం &error_సంకేతం) {
    స్థిరరాశి int8_t *latin_గమ్యం_క్రింది;
    ఒకవేళ (caseLocale == UCASE_LOC_మూలం ||
            (caseLocale >= 0 ?
                !(caseLocale == UCASE_LOC_TURKISH || caseLocale == UCASE_LOC_LITHUANIAN) :
                (options & FOLD_CASE_OPTIONS_ముసుగు) == U_FOLD_CASE_DEFAULT)) {
        latin_గమ్యం_క్రింది = LatinCase::గమ్యం_క్రింది_సాధారణ;
    } లేకపోతే {
        latin_గమ్యం_క్రింది = LatinCase::గమ్యం_క్రింది_TR_LT;
    }
    స్థిరరాశి UTrie2 *trie = ucase_getTrie();
    int32_t dest_సూచీ = 0;
    int32_t prev = src_ప్రారంభం;
    int32_t src_సూచీ = src_ప్రారంభం;
    కొరకు (;;) {
        // fast path for simple cases
        char16_t lead = 0;
        అయితే (src_సూచీ < src_హద్దు) {
            lead = src[src_సూచీ];
            int32_t delta;
            ఒకవేళ (lead < LatinCase::LONG_S) {
                int8_t d = latin_గమ్యం_క్రింది[lead];
                ఒకవేళ (d == LatinCase::EXC) { ఆపు; }
                ++src_సూచీ;
                ఒకవేళ (d == 0) { కొనసాగు; }
                delta = d;
            } లేకపోతే ఒకవేళ (lead >= 0xd800) {
                ఆపు;  // surrogate or higher
            } లేకపోతే {
                uint16_t props = UTRIE2_GET16_FROM_U16_SINGLE_LEAD(trie, lead);
                ఒకవేళ (UCASE_HAS_EXCEPTION(props)) { ఆపు; }
                ++src_సూచీ;
                ఒకవేళ (!UCASE_IS_పై_OR_TITLE(props) || (delta = UCASE_GET_DELTA(props)) == 0) {
                    కొనసాగు;
                }
            }
            lead += static_cast<char16_t>(delta);
            dest_సూచీ = appendUnchanged(dest, dest_సూచీ, dest_సామర్థ్యం,
                                        src + prev, src_సూచీ - 1 - prev, options, edits);
            ఒకవేళ (dest_సూచీ >= 0) {
                dest_సూచీ = appendUChar(dest, dest_సూచీ, dest_సామర్థ్యం, lead);
                ఒకవేళ (edits != శూన్యసూచిక) {
                    edits->కూడు_Replace(1, 1);
                }
            }
            ఒకవేళ (dest_సూచీ < 0) {
                error_సంకేతం = U_సూచీ_OUTOFBOUNDS_ERROR;
                తిరిగి 0;
            }
            prev = src_సూచీ;
        }
        ఒకవేళ (src_సూచీ >= src_హద్దు) {
            ఆపు;
        }
        // slow path
        int32_t cp_ప్రారంభం = src_సూచీ++;
        char16_t trail;
        UChar32 c;
        ఒకవేళ (U16_IS_LEAD(lead) && src_సూచీ < src_హద్దు && U16_IS_TRAIL(trail = src[src_సూచీ])) {
            c = U16_GET_SUPPLEMENTARY(lead, trail);
            ++src_సూచీ;
        } లేకపోతే {
            c = lead;
        }
        స్థిరరాశి char16_t *s = శూన్యసూచిక;
        ఒకవేళ (caseLocale >= 0) {
            csc->cp_ప్రారంభం = cp_ప్రారంభం;
            csc->cp_హద్దు = src_సూచీ;
            c = ucase_గమ్యం_నిండిన_క్రింది(c, utf16_caseContextIterator, csc, &s, caseLocale);
        } లేకపోతే {
            c = ucase_గమ్యం_నిండిన_Folding(c, &s, options);
        }
        ఒకవేళ (c >= 0) {
            dest_సూచీ = appendUnchanged(dest, dest_సూచీ, dest_సామర్థ్యం,
                                        src + prev, cp_ప్రారంభం - prev, options, edits);
            ఒకవేళ (dest_సూచీ >= 0) {
                dest_సూచీ = append_ఫలితం(dest, dest_సూచీ, dest_సామర్థ్యం, c, s,
                                         src_సూచీ - cp_ప్రారంభం, options, edits);
            }
            ఒకవేళ (dest_సూచీ < 0) {
                error_సంకేతం = U_సూచీ_OUTOFBOUNDS_ERROR;
                తిరిగి 0;
            }
            prev = src_సూచీ;
        }
    }
    dest_సూచీ = appendUnchanged(dest, dest_సూచీ, dest_సామర్థ్యం,
                                src + prev, src_సూచీ - prev, options, edits);
    ఒకవేళ (dest_సూచీ < 0) {
        error_సంకేతం = U_సూచీ_OUTOFBOUNDS_ERROR;
        తిరిగి 0;
    }
    తిరిగి dest_సూచీ;
}

int32_t గమ్యం_పై(int32_t caseLocale, uint32_t options,
                char16_t *dest, int32_t dest_సామర్థ్యం,
                స్థిరరాశి char16_t *src, UCaseContext *csc, int32_t src_నిడివి,
                icu::Edits *edits, UError_సంకేతం &error_సంకేతం) {
    స్థిరరాశి int8_t *latin_గమ్యం_పై;
    ఒకవేళ (caseLocale == UCASE_LOC_TURKISH) {
        latin_గమ్యం_పై = LatinCase::గమ్యం_పై_TR;
    } లేకపోతే {
        latin_గమ్యం_పై = LatinCase::గమ్యం_పై_సాధారణ;
    }
    స్థిరరాశి UTrie2 *trie = ucase_getTrie();
    int32_t dest_సూచీ = 0;
    int32_t prev = 0;
    int32_t src_సూచీ = 0;
    కొరకు (;;) {
        // fast path for simple cases
        char16_t lead = 0;
        అయితే (src_సూచీ < src_నిడివి) {
            lead = src[src_సూచీ];
            int32_t delta;
            ఒకవేళ (lead < LatinCase::LONG_S) {
                int8_t d = latin_గమ్యం_పై[lead];
                ఒకవేళ (d == LatinCase::EXC) { ఆపు; }
                ++src_సూచీ;
                ఒకవేళ (d == 0) { కొనసాగు; }
                delta = d;
            } లేకపోతే ఒకవేళ (lead >= 0xd800) {
                ఆపు;  // surrogate or higher
            } లేకపోతే {
                uint16_t props = UTRIE2_GET16_FROM_U16_SINGLE_LEAD(trie, lead);
                ఒకవేళ (UCASE_HAS_EXCEPTION(props)) { ఆపు; }
                ++src_సూచీ;
                ఒకవేళ (UCASE_GET_TYPE(props) != UCASE_క్రింది || (delta = UCASE_GET_DELTA(props)) == 0) {
                    కొనసాగు;
                }
            }
            lead += static_cast<char16_t>(delta);
            dest_సూచీ = appendUnchanged(dest, dest_సూచీ, dest_సామర్థ్యం,
                                        src + prev, src_సూచీ - 1 - prev, options, edits);
            ఒకవేళ (dest_సూచీ >= 0) {
                dest_సూచీ = appendUChar(dest, dest_సూచీ, dest_సామర్థ్యం, lead);
                ఒకవేళ (edits != శూన్యసూచిక) {
                    edits->కూడు_Replace(1, 1);
                }
            }
            ఒకవేళ (dest_సూచీ < 0) {
                error_సంకేతం = U_సూచీ_OUTOFBOUNDS_ERROR;
                తిరిగి 0;
            }
            prev = src_సూచీ;
        }
        ఒకవేళ (src_సూచీ >= src_నిడివి) {
            ఆపు;
        }
        // slow path
        int32_t cp_ప్రారంభం;
        csc->cp_ప్రారంభం = cp_ప్రారంభం = src_సూచీ++;
        char16_t trail;
        UChar32 c;
        ఒకవేళ (U16_IS_LEAD(lead) && src_సూచీ < src_నిడివి && U16_IS_TRAIL(trail = src[src_సూచీ])) {
            c = U16_GET_SUPPLEMENTARY(lead, trail);
            ++src_సూచీ;
        } లేకపోతే {
            c = lead;
        }
        csc->cp_హద్దు = src_సూచీ;
        స్థిరరాశి char16_t *s = శూన్యసూచిక;
        c = ucase_గమ్యం_నిండిన_పై(c, utf16_caseContextIterator, csc, &s, caseLocale);
        ఒకవేళ (c >= 0) {
            dest_సూచీ = appendUnchanged(dest, dest_సూచీ, dest_సామర్థ్యం,
                                        src + prev, cp_ప్రారంభం - prev, options, edits);
            ఒకవేళ (dest_సూచీ >= 0) {
                dest_సూచీ = append_ఫలితం(dest, dest_సూచీ, dest_సామర్థ్యం, c, s,
                                         src_సూచీ - cp_ప్రారంభం, options, edits);
            }
            ఒకవేళ (dest_సూచీ < 0) {
                error_సంకేతం = U_సూచీ_OUTOFBOUNDS_ERROR;
                తిరిగి 0;
            }
            prev = src_సూచీ;
        }
    }
    dest_సూచీ = appendUnchanged(dest, dest_సూచీ, dest_సామర్థ్యం,
                                src + prev, src_సూచీ - prev, options, edits);
    ఒకవేళ (dest_సూచీ < 0) {
        error_సంకేతం = U_సూచీ_OUTOFBOUNDS_ERROR;
        తిరిగి 0;
    }
    తిరిగి dest_సూచీ;
}

}  // namespace

U_NAMESPACE_చివర

U_NAMESPACE_USE

#ఒకవేళ !UCONFIG_NO_BREAK_ITERATION

నామాకాశ {

/**
 * Input: c is a letter I with or without acute accent.
 * start is the index in src after c, and is less than segmentLimit.
 * If a plain i/I is followed by a plain j/J,
 * or an i/I with acute (precomposed or decomposed) is followed by a j/J with acute,
 * then we output accordingly.
 *
 * @return the src index after the titlecased sequence, or the start index if no Dutch IJ
 */
int32_t maybeTitleDutchIJ(స్థిరరాశి char16_t *src, UChar32 c, int32_t ప్రారంభం, int32_t segment_హద్దు,
                          char16_t *dest, int32_t &dest_సూచీ, int32_t dest_సామర్థ్యం, uint32_t options,
                          icu::Edits *edits) {
    U_ASSERT(ప్రారంభం < segment_హద్దు);

    int32_t సూచీ = ప్రారంభం;
    తార్కిక withAcute = అబద్ధం;

    // If the conditions are met, then the following variables tell us what to output.
    int32_t unchanged1 = 0;  // code units before the j, or the whole sequence (0..3)
    తార్కిక doTitleJ = అబద్ధం;  // true if the j needs to be titlecased
    int32_t unchanged2 = 0;  // after the j (0 or 1)

    // next character after the first letter
    char16_t c2 = src[సూచీ++];

    // Is the first letter an i/I with accent?
    ఒకవేళ (c == u'I') {
        ఒకవేళ (c2 == ACUTE) {
            withAcute = నిజం;
            unchanged1 = 1;
            ఒకవేళ (సూచీ == segment_హద్దు) { తిరిగి ప్రారంభం; }
            c2 = src[సూచీ++];
        }
    } లేకపోతే {  // Í
        withAcute = నిజం;
    }

    // Is the next character a j/J?
    ఒకవేళ (c2 == u'j') {
        doTitleJ = నిజం;
    } లేకపోతే ఒకవేళ (c2 == u'J') {
        ++unchanged1;
    } లేకపోతే {
        తిరిగి ప్రారంభం;
    }

    // A plain i/I must be followed by a plain j/J.
    // An i/I with acute must be followed by a j/J with acute.
    ఒకవేళ (withAcute) {
        ఒకవేళ (సూచీ == segment_హద్దు || src[సూచీ++] != ACUTE) { తిరిగి ప్రారంభం; }
        ఒకవేళ (doTitleJ) {
            unchanged2 = 1;
        } లేకపోతే {
            ++unchanged1;
        }
    }

    // There must not be another combining mark.
    ఒకవేళ (సూచీ < segment_హద్దు) {
        int32_t cp;
        int32_t i = సూచీ;
        U16_తదుపరి(src, i, segment_హద్దు, cp);
        uint32_t type_ముసుగు = U_GET_GC_ముసుగు(cp);
        ఒకవేళ ((type_ముసుగు & U_GC_M_ముసుగు) != 0) {
            తిరిగి ప్రారంభం;
        }
    }

    // Output the rest of the Dutch IJ.
    dest_సూచీ = appendUnchanged(dest, dest_సూచీ, dest_సామర్థ్యం, src + ప్రారంభం, unchanged1, options, edits);
    ప్రారంభం += unchanged1;
    ఒకవేళ (doTitleJ) {
        dest_సూచీ = appendUChar(dest, dest_సూచీ, dest_సామర్థ్యం, u'J');
        ఒకవేళ (edits != శూన్యసూచిక) {
            edits->కూడు_Replace(1, 1);
        }
        ++ప్రారంభం;
    }
    dest_సూచీ = appendUnchanged(dest, dest_సూచీ, dest_సామర్థ్యం, src + ప్రారంభం, unchanged2, options, edits);

    U_ASSERT(ప్రారంభం + unchanged2 == సూచీ);
    తిరిగి సూచీ;
}

}  // namespace

U_CFUNC int32_t U_CALLCONV
ustrcase_అంతర్గత_గమ్యం_Title(int32_t caseLocale, uint32_t options, BreakIterator *iter,
                         char16_t *dest, int32_t dest_సామర్థ్యం,
                         స్థిరరాశి char16_t *src, int32_t src_నిడివి,
                         icu::Edits *edits,
                         UError_సంకేతం &error_సంకేతం) {
    ఒకవేళ (!ustrcase_checkTitleAdjustmentOptions(options, error_సంకేతం)) {
        తిరిగి 0;
    }

    /* set up local variables */
    UCaseContext csc=UCASECONTEXT_INITIALIZER;
    csc.p=(శూన్య *)src;
    csc.హద్దు=src_నిడివి;
    int32_t dest_సూచీ=0;
    int32_t prev=0;
    తార్కిక is_మొదటి_సూచీ=నిజం;

    /* titlecasing loop */
    అయితే(prev<src_నిడివి) {
        /* find next index where to titlecase */
        int32_t సూచీ;
        ఒకవేళ(is_మొదటి_సూచీ) {
            is_మొదటి_సూచీ=అబద్ధం;
            సూచీ=iter->first();
        } లేకపోతే {
            సూచీ=iter->next();
        }
        ఒకవేళ(సూచీ==UBRK_పూర్తి || సూచీ>src_నిడివి) {
            సూచీ=src_నిడివి;
        }

        /*
         * Segment [prev..index[ into 3 parts:
         * a) skipped characters (copy as-is) [prev..titleStart[
         * b) first letter (titlecase)              [titleStart..titleLimit[
         * c) subsequent characters (lowercase)                 [titleLimit..index[
         */
        ఒకవేళ(prev<సూచీ) {
            // Find and copy skipped characters [prev..titleStart[
            int32_t title_ప్రారంభం=prev;
            int32_t title_హద్దు=prev;
            UChar32 c;
            U16_తదుపరి(src, title_హద్దు, సూచీ, c);
            ఒకవేళ ((options&U_TITLECASE_NO_BREAK_ADJUSTMENT)==0) {
                // Adjust the titlecasing index to the next cased character,
                // or to the next letter/number/symbol/private use.
                // Stop with titleStart<titleLimit<=index
                // if there is a character to be titlecased,
                // or else stop with titleStart==titleLimit==index.
                తార్కిక గమ్యం_Cased = (options&U_TITLECASE_ADJUST_గమ్యం_CASED) != 0;
                అయితే (గమ్యం_Cased ? UCASE_NONE==ucase_getType(c) : !ustrcase_isLNS(c)) {
                    title_ప్రారంభం=title_హద్దు;
                    ఒకవేళ(title_హద్దు==సూచీ) {
                        ఆపు;
                    }
                    U16_తదుపరి(src, title_హద్దు, సూచీ, c);
                }
                ఒకవేళ (prev < title_ప్రారంభం) {
                    dest_సూచీ=appendUnchanged(dest, dest_సూచీ, dest_సామర్థ్యం,
                                              src+prev, title_ప్రారంభం-prev, options, edits);
                    ఒకవేళ(dest_సూచీ<0) {
                        error_సంకేతం=U_సూచీ_OUTOFBOUNDS_ERROR;
                        తిరిగి 0;
                    }
                }
            }

            ఒకవేళ(title_ప్రారంభం<title_హద్దు) {
                /* titlecase c which is from [titleStart..titleLimit[ */
                csc.cp_ప్రారంభం=title_ప్రారంభం;
                csc.cp_హద్దు=title_హద్దు;
                స్థిరరాశి char16_t *s;
                c=ucase_గమ్యం_నిండిన_Title(c, utf16_caseContextIterator, &csc, &s, caseLocale);
                dest_సూచీ=append_ఫలితం(dest, dest_సూచీ, dest_సామర్థ్యం, c, s,
                                       title_హద్దు-title_ప్రారంభం, options, edits);
                ఒకవేళ(dest_సూచీ<0) {
                    error_సంకేతం=U_సూచీ_OUTOFBOUNDS_ERROR;
                    తిరిగి 0;
                }

                /* Special case Dutch IJ titlecasing */
                ఒకవేళ (title_ప్రారంభం+1 < సూచీ &&
                        caseLocale == UCASE_LOC_DUTCH) {
                    ఒకవేళ (c < 0) {
                        c = ~c;
                    }

                    ఒకవేళ (c == u'I' || c == u'Í') {
                        title_హద్దు = maybeTitleDutchIJ(src, c, title_ప్రారంభం + 1, సూచీ,
                                                       dest, dest_సూచీ, dest_సామర్థ్యం, options,
                                                       edits);
                    }
                }

                /* lowercase [titleLimit..index[ */
                ఒకవేళ(title_హద్దు<సూచీ) {
                    ఒకవేళ((options&U_TITLECASE_NO_LOWERCASE)==0) {
                        /* Normal operation: Lowercase the rest of the word. */
                        dest_సూచీ+=
                            గమ్యం_క్రింది(
                                caseLocale, options,
                                (dest==శూన్యసూచిక) ? శూన్యసూచిక: dest+dest_సూచీ, dest_సామర్థ్యం-dest_సూచీ,
                                src, &csc, title_హద్దు, సూచీ,
                                edits, error_సంకేతం);
                        ఒకవేళ(error_సంకేతం==U_బఫరు_అధికప్రవాహం_ERROR) {
                            error_సంకేతం=U_ZERO_ERROR;
                        }
                        ఒకవేళ(U_FAILURE(error_సంకేతం)) {
                            తిరిగి dest_సూచీ;
                        }
                    } లేకపోతే {
                        /* Optionally just copy the rest of the word unchanged. */
                        dest_సూచీ=appendUnchanged(dest, dest_సూచీ, dest_సామర్థ్యం,
                                                  src+title_హద్దు, సూచీ-title_హద్దు, options, edits);
                        ఒకవేళ(dest_సూచీ<0) {
                            error_సంకేతం=U_సూచీ_OUTOFBOUNDS_ERROR;
                            తిరిగి 0;
                        }
                    }
                }
            }
        }

        prev=సూచీ;
    }

    తిరిగి check_అధికప్రవాహం_And_Edits_Error(dest_సూచీ, dest_సామర్థ్యం, edits, error_సంకేతం);
}

#అయితే_ముగింపు  // !UCONFIG_NO_BREAK_ITERATION

U_NAMESPACE_BEGIN
నామాకాశ Greek_పై {

// Data generated by prototype code, see
// https://icu.unicode.org/design/case/greek-upper
// TODO: Move this data into ucase.icu.
స్థిర స్థిరరాశి uint16_t data0370[] = {
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

స్థిర స్థిరరాశి uint16_t data1F00[] = {
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
స్థిర స్థిరరాశి uint16_t data2126 = 0x03A9 | HAS_VOWEL;

uint32_t get_Letter_దత్తాంశం(UChar32 c) {
    ఒకవేళ (c < 0x370 || 0x2126 < c || (0x3ff < c && c < 0x1f00)) {
        తిరిగి 0;
    } లేకపోతే ఒకవేళ (c <= 0x3ff) {
        తిరిగి data0370[c - 0x370];
    } లేకపోతే ఒకవేళ (c <= 0x1fff) {
        తిరిగి data1F00[c - 0x1f00];
    } లేకపోతే ఒకవేళ (c == 0x2126) {
        తిరిగి data2126;
    } లేకపోతే {
        తిరిగి 0;
    }
}

uint32_t get_Diacritic_దత్తాంశం(UChar32 c) {
    ఎంపిక (c) {
    స్థితి 0x0300:  // varia
    స్థితి 0x0301:  // tonos = oxia
    స్థితి 0x0342:  // perispomeni
    స్థితి 0x0302:  // circumflex can look like perispomeni
    స్థితి 0x0303:  // tilde can look like perispomeni
    స్థితి 0x0311:  // inverted breve can look like perispomeni
        తిరిగి HAS_ACCENT;
    స్థితి 0x0308:  // dialytika = diaeresis
        తిరిగి HAS_COMBINING_DIALYTIKA;
    స్థితి 0x0344:  // dialytika tonos
        తిరిగి HAS_COMBINING_DIALYTIKA | HAS_ACCENT;
    స్థితి 0x0345:  // ypogegrammeni = iota subscript
        తిరిగి HAS_YPOGEGRAMMENI;
    స్థితి 0x0304:  // macron
    స్థితి 0x0306:  // breve
    స్థితి 0x0313:  // comma above
    స్థితి 0x0314:  // reversed comma above
    స్థితి 0x0343:  // koronis
        తిరిగి HAS_OTHER_GREEK_DIACRITIC;
    అప్రమేయ:
        తిరిగి 0;
    }
}

UBool isFollowedByCasedLetter(స్థిరరాశి char16_t *s, int32_t i, int32_t length) {
    అయితే (i < length) {
        UChar32 c;
        U16_తదుపరి(s, i, length, c);
        int32_t type = ucase_getTypeOrIgnorable(c);
        ఒకవేళ ((type & UCASE_IGNORABLE) != 0) {
            // Case-ignorable, continue with the loop.
        } లేకపోతే ఒకవేళ (type != UCASE_NONE) {
            తిరిగి నిజం;  // Followed by cased letter.
        } లేకపోతే {
            తిరిగి అబద్ధం;  // Uncased and not case-ignorable.
        }
    }
    తిరిగి అబద్ధం;  // Not followed by cased letter.
}

/**
 * Greek string uppercasing with a state machine.
 * Probably simpler than a stateless function that has to figure out complex context-before
 * for each character.
 * TODO: Try to re-consolidate one way or another with the non-Greek function.
 */
int32_t గమ్యం_పై(uint32_t options,
                char16_t *dest, int32_t dest_సామర్థ్యం,
                స్థిరరాశి char16_t *src, int32_t src_నిడివి,
                Edits *edits,
                UError_సంకేతం &error_సంకేతం) {
    int32_t dest_సూచీ=0;
    uint32_t state = 0;
    కొరకు (int32_t i = 0; i < src_నిడివి;) {
        int32_t తదుపరి_సూచీ = i;
        UChar32 c;
        U16_తదుపరి(src, తదుపరి_సూచీ, src_నిడివి, c);
        uint32_t తదుపరి_State = 0;
        int32_t type = ucase_getTypeOrIgnorable(c);
        ఒకవేళ ((type & UCASE_IGNORABLE) != 0) {
            // c is case-ignorable
            తదుపరి_State |= (state & AFTER_CASED);
        } లేకపోతే ఒకవేళ (type != UCASE_NONE) {
            // c is cased
            తదుపరి_State |= AFTER_CASED;
        }
        uint32_t దత్తాంశం = get_Letter_దత్తాంశం(c);
        ఒకవేళ (దత్తాంశం > 0) {
            uint32_t upper = దత్తాంశం & పై_ముసుగు;
            // Add a dialytika to this iota or ypsilon vowel
            // if we removed a tonos from the previous vowel,
            // and that previous vowel did not also have (or gain) a dialytika.
            // Adding one only to the final vowel in a longer sequence
            // (which does not occur in normal writing) would require lookahead.
            // Set the same flag as for preserving an existing dialytika.
            ఒకవేళ ((దత్తాంశం & HAS_VOWEL) != 0 &&
                (state & (AFTER_VOWEL_WITH_PRECOMPOSED_ACCENT | AFTER_VOWEL_WITH_COMBINING_ACCENT)) !=
                    0 &&
                (upper == 0x399 || upper == 0x3A5)) {
                దత్తాంశం |= (state & AFTER_VOWEL_WITH_PRECOMPOSED_ACCENT) ? HAS_DIALYTIKA
                                                                      : HAS_COMBINING_DIALYTIKA;
            }
            int32_t అంకె_Ypogegrammeni = 0;  // Map each one to a trailing, spacing, capital iota.
            ఒకవేళ ((దత్తాంశం & HAS_YPOGEGRAMMENI) != 0) {
                అంకె_Ypogegrammeni = 1;
            }
            స్థిరరాశి UBool hasPrecomposedAccent = (దత్తాంశం & HAS_ACCENT) != 0;
            // Skip combining diacritics after this Greek letter.
            అయితే (తదుపరి_సూచీ < src_నిడివి) {
                uint32_t diacritic_దత్తాంశం = get_Diacritic_దత్తాంశం(src[తదుపరి_సూచీ]);
                ఒకవేళ (diacritic_దత్తాంశం != 0) {
                    దత్తాంశం |= diacritic_దత్తాంశం;
                    ఒకవేళ ((diacritic_దత్తాంశం & HAS_YPOGEGRAMMENI) != 0) {
                        ++అంకె_Ypogegrammeni;
                    }
                    ++తదుపరి_సూచీ;
                } లేకపోతే {
                    ఆపు;  // not a Greek diacritic
                }
            }
            ఒకవేళ ((దత్తాంశం & HAS_VOWEL_AND_ACCENT_AND_DIALYTIKA) == HAS_VOWEL_AND_ACCENT) {
                తదుపరి_State |= hasPrecomposedAccent ? AFTER_VOWEL_WITH_PRECOMPOSED_ACCENT
                                                  : AFTER_VOWEL_WITH_COMBINING_ACCENT;
            }
            // Map according to Greek rules.
            UBool కూడు_Tonos = అబద్ధం;
            ఒకవేళ (upper == 0x397 &&
                    (దత్తాంశం & HAS_ACCENT) != 0 &&
                    అంకె_Ypogegrammeni == 0 &&
                    (state & AFTER_CASED) == 0 &&
                    !isFollowedByCasedLetter(src, తదుపరి_సూచీ, src_నిడివి)) {
                // Keep disjunctive "or" with (only) a tonos.
                // We use the same "word boundary" conditions as for the Final_Sigma test.
                ఒకవేళ (hasPrecomposedAccent) {
                    upper = 0x389;  // Preserve the precomposed form.
                } లేకపోతే {
                    కూడు_Tonos = నిజం;
                }
            } లేకపోతే ఒకవేళ ((దత్తాంశం & HAS_DIALYTIKA) != 0) {
                // Preserve a vowel with dialytika in precomposed form if it exists.
                ఒకవేళ (upper == 0x399) {
                    upper = 0x3AA;
                    దత్తాంశం &= ~HAS_EITHER_DIALYTIKA;
                } లేకపోతే ఒకవేళ (upper == 0x3A5) {
                    upper = 0x3AB;
                    దత్తాంశం &= ~HAS_EITHER_DIALYTIKA;
                }
            }

            UBool change;
            ఒకవేళ (edits == శూన్యసూచిక && (options & U_OMIT_UNCHANGED_పాఠ్యం) == 0) {
                change = నిజం;  // common, simple usage
            } లేకపోతే {
                // Find out first whether we are changing the text.
                change = src[i] != upper || అంకె_Ypogegrammeni > 0;
                int32_t i2 = i + 1;
                ఒకవేళ ((దత్తాంశం & HAS_EITHER_DIALYTIKA) != 0) {
                    change |= i2 >= తదుపరి_సూచీ || src[i2] != 0x308;
                    ++i2;
                }
                ఒకవేళ (కూడు_Tonos) {
                    change |= i2 >= తదుపరి_సూచీ || src[i2] != 0x301;
                    ++i2;
                }
                int32_t old_నిడివి = తదుపరి_సూచీ - i;
                int32_t new_నిడివి = (i2 - i) + అంకె_Ypogegrammeni;
                change |= old_నిడివి != new_నిడివి;
                ఒకవేళ (change) {
                    ఒకవేళ (edits != శూన్యసూచిక) {
                        edits->కూడు_Replace(old_నిడివి, new_నిడివి);
                    }
                } లేకపోతే {
                    ఒకవేళ (edits != శూన్యసూచిక) {
                        edits->కూడు_Unchanged(old_నిడివి);
                    }
                    // Write unchanged text?
                    change = (options & U_OMIT_UNCHANGED_పాఠ్యం) == 0;
                }
            }

            ఒకవేళ (change) {
                dest_సూచీ = appendUChar(dest, dest_సూచీ, dest_సామర్థ్యం, static_cast<char16_t>(upper));
                ఒకవేళ (dest_సూచీ >= 0 && (దత్తాంశం & HAS_EITHER_DIALYTIKA) != 0) {
                    dest_సూచీ=appendUChar(dest, dest_సూచీ, dest_సామర్థ్యం, 0x308);  // restore or add a dialytika
                }
                ఒకవేళ (dest_సూచీ >= 0 && కూడు_Tonos) {
                    dest_సూచీ=appendUChar(dest, dest_సూచీ, dest_సామర్థ్యం, 0x301);
                }
                అయితే (dest_సూచీ >= 0 && అంకె_Ypogegrammeni > 0) {
                    dest_సూచీ=appendUChar(dest, dest_సూచీ, dest_సామర్థ్యం, 0x399);
                    --అంకె_Ypogegrammeni;
                }
                ఒకవేళ(dest_సూచీ<0) {
                    error_సంకేతం=U_సూచీ_OUTOFBOUNDS_ERROR;
                    తిరిగి 0;
                }
            }
        } లేకపోతే {
            స్థిరరాశి char16_t *s;
            c=ucase_గమ్యం_నిండిన_పై(c, శూన్యసూచిక, శూన్యసూచిక, &s, UCASE_LOC_GREEK);
            dest_సూచీ = append_ఫలితం(dest, dest_సూచీ, dest_సామర్థ్యం, c, s,
                                     తదుపరి_సూచీ - i, options, edits);
            ఒకవేళ (dest_సూచీ < 0) {
                error_సంకేతం = U_సూచీ_OUTOFBOUNDS_ERROR;
                తిరిగి 0;
            }
        }
        i = తదుపరి_సూచీ;
        state = తదుపరి_State;
    }

    తిరిగి dest_సూచీ;
}

}  // namespace GreekUpper
U_NAMESPACE_చివర

/* functions available in the common library (for unistr_case.cpp) */

U_CFUNC int32_t U_CALLCONV
ustrcase_అంతర్గత_గమ్యం_క్రింది(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_వాడని
                         char16_t *dest, int32_t dest_సామర్థ్యం,
                         స్థిరరాశి char16_t *src, int32_t src_నిడివి,
                         icu::Edits *edits,
                         UError_సంకేతం &error_సంకేతం) {
    UCaseContext csc=UCASECONTEXT_INITIALIZER;
    csc.p=(శూన్య *)src;
    csc.హద్దు=src_నిడివి;
    int32_t dest_సూచీ = గమ్యం_క్రింది(
        caseLocale, options,
        dest, dest_సామర్థ్యం,
        src, &csc, 0, src_నిడివి,
        edits, error_సంకేతం);
    తిరిగి check_అధికప్రవాహం_And_Edits_Error(dest_సూచీ, dest_సామర్థ్యం, edits, error_సంకేతం);
}

U_CFUNC int32_t U_CALLCONV
ustrcase_అంతర్గత_గమ్యం_పై(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_వాడని
                         char16_t *dest, int32_t dest_సామర్థ్యం,
                         స్థిరరాశి char16_t *src, int32_t src_నిడివి,
                         icu::Edits *edits,
                         UError_సంకేతం &error_సంకేతం) {
    int32_t dest_సూచీ;
    ఒకవేళ (caseLocale == UCASE_LOC_GREEK) {
        dest_సూచీ = Greek_పై::గమ్యం_పై(options, dest, dest_సామర్థ్యం,
                                        src, src_నిడివి, edits, error_సంకేతం);
    } లేకపోతే {
        UCaseContext csc=UCASECONTEXT_INITIALIZER;
        csc.p=(శూన్య *)src;
        csc.హద్దు=src_నిడివి;
        dest_సూచీ = గమ్యం_పై(
            caseLocale, options,
            dest, dest_సామర్థ్యం,
            src, &csc, src_నిడివి,
            edits, error_సంకేతం);
    }
    తిరిగి check_అధికప్రవాహం_And_Edits_Error(dest_సూచీ, dest_సామర్థ్యం, edits, error_సంకేతం);
}

U_CFUNC int32_t U_CALLCONV
ustrcase_అంతర్గత_Fold(int32_t /* caseLocale */, uint32_t options, UCASEMAP_BREAK_ITERATOR_వాడని
                      char16_t *dest, int32_t dest_సామర్థ్యం,
                      స్థిరరాశి char16_t *src, int32_t src_నిడివి,
                      icu::Edits *edits,
                      UError_సంకేతం &error_సంకేతం) {
    int32_t dest_సూచీ = గమ్యం_క్రింది(
        -1, options,
        dest, dest_సామర్థ్యం,
        src, శూన్యసూచిక, 0, src_నిడివి,
        edits, error_సంకేతం);
    తిరిగి check_అధికప్రవాహం_And_Edits_Error(dest_సూచీ, dest_సామర్థ్యం, edits, error_సంకేతం);
}

U_CFUNC int32_t
ustrcase_map(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_PARAM
             char16_t *dest, int32_t dest_సామర్థ్యం,
             స్థిరరాశి char16_t *src, int32_t src_నిడివి,
             UStringCaseMapper *stringCaseMapper,
             icu::Edits *edits,
             UError_సంకేతం &error_సంకేతం) {
    int32_t dest_నిడివి;

    /* check argument values */
    ఒకవేళ(U_FAILURE(error_సంకేతం)) {
        తిరిగి 0;
    }
    ఒకవేళ( dest_సామర్థ్యం<0 ||
        (dest==శూన్యసూచిక && dest_సామర్థ్యం>0) ||
        src==శూన్యసూచిక ||
        src_నిడివి<-1
    ) {
        error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి 0;
    }

    /* get the string length */
    ఒకవేళ(src_నిడివి==-1) {
        src_నిడివి=u_strlen(src);
    }

    /* check for overlapping source and destination */
    ఒకవేళ( dest!=శూన్యసూచిక &&
        ((src>=dest && src<(dest+dest_సామర్థ్యం)) ||
         (dest>=src && dest<(src+src_నిడివి)))
    ) {
        error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి 0;
    }

    ఒకవేళ (edits != శూన్యసూచిక && (options & U_EDITS_NO_RESET) == 0) {
        edits->reset();
    }
    dest_నిడివి=stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                                dest, dest_సామర్థ్యం, src, src_నిడివి, edits, error_సంకేతం);
    తిరిగి u_terminateUChars(dest, dest_సామర్థ్యం, dest_నిడివి, &error_సంకేతం);
}

U_CFUNC int32_t
ustrcase_mapWithOverlap(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_PARAM
                        char16_t *dest, int32_t dest_సామర్థ్యం,
                        స్థిరరాశి char16_t *src, int32_t src_నిడివి,
                        UStringCaseMapper *stringCaseMapper,
                        UError_సంకేతం &error_సంకేతం) {
    char16_t బఫరు[300];
    char16_t *తాత్కాలిక;

    int32_t dest_నిడివి;

    /* check argument values */
    ఒకవేళ(U_FAILURE(error_సంకేతం)) {
        తిరిగి 0;
    }
    ఒకవేళ( dest_సామర్థ్యం<0 ||
        (dest==శూన్యసూచిక && dest_సామర్థ్యం>0) ||
        src==శూన్యసూచిక ||
        src_నిడివి<-1
    ) {
        error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి 0;
    }

    /* get the string length */
    ఒకవేళ(src_నిడివి==-1) {
        src_నిడివి=u_strlen(src);
    }

    /* check for overlapping source and destination */
    ఒకవేళ( dest!=శూన్యసూచిక &&
        ((src>=dest && src<(dest+dest_సామర్థ్యం)) ||
         (dest>=src && dest<(src+src_నిడివి)))
    ) {
        /* overlap: provide a temporary destination buffer and later copy the result */
        ఒకవేళ(dest_సామర్థ్యం<=UPRV_LENGTHOF(బఫరు)) {
            /* the stack buffer is large enough */
            తాత్కాలిక=బఫరు;
        } లేకపోతే {
            /* allocate a buffer */
            తాత్కాలిక=(char16_t *)uprv_malloc(dest_సామర్థ్యం*U_SIZEOF_UCHAR);
            ఒకవేళ(తాత్కాలిక==శూన్యసూచిక) {
                error_సంకేతం=U_జ్ఞాపకం_కేటాయింపు_ERROR;
                తిరిగి 0;
            }
        }
    } లేకపోతే {
        తాత్కాలిక=dest;
    }

    dest_నిడివి=stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                                తాత్కాలిక, dest_సామర్థ్యం, src, src_నిడివి, శూన్యసూచిక, error_సంకేతం);
    ఒకవేళ(తాత్కాలిక!=dest) {
        /* copy the result string to the destination buffer */
        ఒకవేళ (U_SUCCESS(error_సంకేతం) && 0 < dest_నిడివి && dest_నిడివి <= dest_సామర్థ్యం) {
            u_memmove(dest, తాత్కాలిక, dest_నిడివి);
        }
        ఒకవేళ(తాత్కాలిక!=బఫరు) {
            uprv_free(తాత్కాలిక);
        }
    }

    తిరిగి u_terminateUChars(dest, dest_సామర్థ్యం, dest_నిడివి, &error_సంకేతం);
}

/* public API functions */

U_CAPI int32_t U_EXPORT2
u_strFoldCase(char16_t *dest, int32_t dest_సామర్థ్యం,
              స్థిరరాశి char16_t *src, int32_t src_నిడివి,
              uint32_t options,
              UError_సంకేతం *p_Error_సంకేతం) {
    తిరిగి ustrcase_mapWithOverlap(
        UCASE_LOC_మూలం, options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_సామర్థ్యం,
        src, src_నిడివి,
        ustrcase_అంతర్గత_Fold, *p_Error_సంకేతం);
}

U_NAMESPACE_BEGIN

int32_t CaseMap::fold(
        uint32_t options,
        స్థిరరాశి char16_t *src, int32_t src_నిడివి,
        char16_t *dest, int32_t dest_సామర్థ్యం, Edits *edits,
        UError_సంకేతం &error_సంకేతం) {
    తిరిగి ustrcase_map(
        UCASE_LOC_మూలం, options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_సామర్థ్యం,
        src, src_నిడివి,
        ustrcase_అంతర్గత_Fold, edits, error_సంకేతం);
}

U_NAMESPACE_చివర

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
నిర్మాణం CmpEquivLevel {
    స్థిరరాశి char16_t *ప్రారంభం, *s, *హద్దు;
};
రకనిర్వచనం నిర్మాణం CmpEquivLevel CmpEquivLevel;

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
స్థిర int32_t _cmpFold(
            స్థిరరాశి char16_t *s1, int32_t length1,
            స్థిరరాశి char16_t *s2, int32_t length2,
            uint32_t options,
            int32_t *సరిపోలిక_Len1, int32_t *సరిపోలిక_Len2,
            UError_సంకేతం *p_Error_సంకేతం) {
    int32_t cmpRes = 0;

    /* current-level start/limit - s1/s2 as current */
    స్థిరరాశి char16_t *start1, *start2, *limit1, *limit2;

    /* points to the original start address */
    స్థిరరాశి char16_t *org1, *org2;

    /* points to the end of match + 1 */
    స్థిరరాశి char16_t *m1, *m2;

    /* case folding variables */
    స్థిరరాశి char16_t *p;
    int32_t length;

    /* stacks of previous-level start/current/limit */
    CmpEquivLevel stack1[2], stack2[2];

    /* case folding buffers, only use current-level start/limit */
    char16_t fold1[UCASE_MAX_STRING_నిడివి+1], fold2[UCASE_MAX_STRING_నిడివి+1];

    /* track which is the current level per string */
    int32_t level1, level2;

    /* current code units, and code points for lookups */
    UChar32 c1, c2, cp1, cp2;

    /* no argument error checking because this itself is not an API */

    /*
     * assume that at least the option U_COMPARE_IGNORE_CASE is set
     * otherwise this function would have to behave exactly as uprv_strCompare()
     */
    ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి 0;
    }

    /* initialize */
    ఒకవేళ(సరిపోలిక_Len1) {
        U_ASSERT(సరిపోలిక_Len2 !=శూన్యసూచిక);
        *సరిపోలిక_Len1=0;
        *సరిపోలిక_Len2=0;
    }

    start1=m1=org1=s1;
    ఒకవేళ(length1==-1) {
        limit1=శూన్యసూచిక;
    } లేకపోతే {
        limit1=s1+length1;
    }

    start2=m2=org2=s2;
    ఒకవేళ(length2==-1) {
        limit2=శూన్యసూచిక;
    } లేకపోతే {
        limit2=s2+length2;
    }

    level1=level2=0;
    c1=c2=-1;

    /* comparison loop */
    కొరకు(;;) {
        /*
         * here a code unit value of -1 means "get another code unit"
         * below it will mean "this source is finished"
         */

        ఒకవేళ(c1<0) {
            /* get next code unit from string 1, post-increment */
            కొరకు(;;) {
                ఒకవేళ(s1==limit1 || ((c1=*s1)==0 && (limit1==శూన్యసూచిక || (options&_STRNCMP_STYLE)))) {
                    ఒకవేళ(level1==0) {
                        c1=-1;
                        ఆపు;
                    }
                } లేకపోతే {
                    ++s1;
                    ఆపు;
                }

                /* reached end of level buffer, pop one level */
                చేయి {
                    --level1;
                    start1=stack1[level1].ప్రారంభం;    /*Not uninitialized*/
                } అయితే(start1==శూన్యసూచిక);
                s1=stack1[level1].s;                /*Not uninitialized*/
                limit1=stack1[level1].హద్దు;        /*Not uninitialized*/
            }
        }

        ఒకవేళ(c2<0) {
            /* get next code unit from string 2, post-increment */
            కొరకు(;;) {
                ఒకవేళ(s2==limit2 || ((c2=*s2)==0 && (limit2==శూన్యసూచిక || (options&_STRNCMP_STYLE)))) {
                    ఒకవేళ(level2==0) {
                        c2=-1;
                        ఆపు;
                    }
                } లేకపోతే {
                    ++s2;
                    ఆపు;
                }

                /* reached end of level buffer, pop one level */
                చేయి {
                    --level2;
                    start2=stack2[level2].ప్రారంభం;    /*Not uninitialized*/
                } అయితే(start2==శూన్యసూచిక);
                s2=stack2[level2].s;                /*Not uninitialized*/
                limit2=stack2[level2].హద్దు;        /*Not uninitialized*/
            }
        }

        /*
         * compare c1 and c2
         * either variable c1, c2 is -1 only if the corresponding string is finished
         */
        ఒకవేళ(c1==c2) {
            స్థిరరాశి char16_t *next1, *next2;

            ఒకవేళ(c1<0) {
                cmpRes=0;   /* c1==c2==-1 indicating end of strings */
                ఆపు;
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
            next1=next2=శూన్యసూచిక;
            ఒకవేళ(level1==0) {
                next1=s1;
            } లేకపోతే ఒకవేళ(s1==limit1) {
                /* Note: This implementation only use a single level of stack.
                 *      If this code needs to be changed to use multiple levels
                 *      of stacks, the code above should check if the current
                 *      code is at the end of all stacks.
                 */
                U_ASSERT(level1==1);

                /* is s1 at the end of the current stack? */
                next1=stack1[0].s;
            }

            ఒకవేళ (next1!=శూన్యసూచిక) {
                ఒకవేళ(level2==0) {
                    next2=s2;
                } లేకపోతే ఒకవేళ(s2==limit2) {
                    U_ASSERT(level2==1);

                    /* is s2 at the end of the current stack? */
                    next2=stack2[0].s;
                }
                ఒకవేళ(next2!=శూన్యసూచిక) {
                    m1=next1;
                    m2=next2;
                }
            }
            c1=c2=-1;       /* make us fetch new code units */
            కొనసాగు;
        } లేకపోతే ఒకవేళ(c1<0) {
            cmpRes=-1;      /* string 1 ends before string 2 */
            ఆపు;
        } లేకపోతే ఒకవేళ(c2<0) {
            cmpRes=1;       /* string 2 ends before string 1 */
            ఆపు;
        }
        /* c1!=c2 && c1>=0 && c2>=0 */

        /* get complete code points for c1, c2 for lookups if either is a surrogate */
        cp1=c1;
        ఒకవేళ(U_IS_SURROGATE(c1)) {
            char16_t c;

            ఒకవేళ(U_IS_SURROGATE_LEAD(c1)) {
                ఒకవేళ(s1!=limit1 && U16_IS_TRAIL(c=*s1)) {
                    /* advance ++s1; only below if cp1 decomposes/case-folds */
                    cp1=U16_GET_SUPPLEMENTARY(c1, c);
                }
            } లేకపోతే /* isTrail(c1) */ {
                ఒకవేళ(start1<=(s1-2) && U16_IS_LEAD(c=*(s1-2))) {
                    cp1=U16_GET_SUPPLEMENTARY(c, c1);
                }
            }
        }

        cp2=c2;
        ఒకవేళ(U_IS_SURROGATE(c2)) {
            char16_t c;

            ఒకవేళ(U_IS_SURROGATE_LEAD(c2)) {
                ఒకవేళ(s2!=limit2 && U16_IS_TRAIL(c=*s2)) {
                    /* advance ++s2; only below if cp2 decomposes/case-folds */
                    cp2=U16_GET_SUPPLEMENTARY(c2, c);
                }
            } లేకపోతే /* isTrail(c2) */ {
                ఒకవేళ(start2<=(s2-2) && U16_IS_LEAD(c=*(s2-2))) {
                    cp2=U16_GET_SUPPLEMENTARY(c, c2);
                }
            }
        }

        /*
         * go down one level for each string
         * continue with the main loop as soon as there is a real change
         */

        ఒకవేళ( level1==0 &&
            (length = ucase_గమ్యం_నిండిన_Folding(cp1, &p, options)) >= 0
        ) {
            /* cp1 case-folds to the code point "length" or to p[length] */
            ఒకవేళ(U_IS_SURROGATE(c1)) {
                ఒకవేళ(U_IS_SURROGATE_LEAD(c1)) {
                    /* advance beyond source surrogate pair if it case-folds */
                    ++s1;
                } లేకపోతే /* isTrail(c1) */ {
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
            stack1[0].ప్రారంభం=start1;
            stack1[0].s=s1;
            stack1[0].హద్దు=limit1;
            ++level1;

            /* copy the folding result to fold1[] */
            ఒకవేళ(length<=UCASE_MAX_STRING_నిడివి) {
                u_memcpy(fold1, p, length);
            } లేకపోతే {
                int32_t i=0;
                U16_APPEND_UNSAFE(fold1, i, length);
                length=i;
            }

            /* set next level pointers to case folding */
            start1=s1=fold1;
            limit1=fold1+length;

            /* get ready to read from decomposition, continue with loop */
            c1=-1;
            కొనసాగు;
        }

        ఒకవేళ( level2==0 &&
            (length = ucase_గమ్యం_నిండిన_Folding(cp2, &p, options)) >= 0
        ) {
            /* cp2 case-folds to the code point "length" or to p[length] */
            ఒకవేళ(U_IS_SURROGATE(c2)) {
                ఒకవేళ(U_IS_SURROGATE_LEAD(c2)) {
                    /* advance beyond source surrogate pair if it case-folds */
                    ++s2;
                } లేకపోతే /* isTrail(c2) */ {
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
            stack2[0].ప్రారంభం=start2;
            stack2[0].s=s2;
            stack2[0].హద్దు=limit2;
            ++level2;

            /* copy the folding result to fold2[] */
            ఒకవేళ(length<=UCASE_MAX_STRING_నిడివి) {
                u_memcpy(fold2, p, length);
            } లేకపోతే {
                int32_t i=0;
                U16_APPEND_UNSAFE(fold2, i, length);
                length=i;
            }

            /* set next level pointers to case folding */
            start2=s2=fold2;
            limit2=fold2+length;

            /* get ready to read from decomposition, continue with loop */
            c2=-1;
            కొనసాగు;
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

        ఒకవేళ(c1>=0xd800 && c2>=0xd800 && (options&U_COMPARE_సంకేతం_బిందువు_క్రమం)) {
            /* subtract 0x2800 from BMP code points to make them smaller than supplementary ones */
            ఒకవేళ(
                (c1<=0xdbff && s1!=limit1 && U16_IS_TRAIL(*s1)) ||
                (U16_IS_TRAIL(c1) && start1!=(s1-1) && U16_IS_LEAD(*(s1-2)))
            ) {
                /* part of a surrogate pair, leave >=d800 */
            } లేకపోతే {
                /* BMP code point - may be surrogate code point - make <d800 */
                c1-=0x2800;
            }

            ఒకవేళ(
                (c2<=0xdbff && s2!=limit2 && U16_IS_TRAIL(*s2)) ||
                (U16_IS_TRAIL(c2) && start2!=(s2-1) && U16_IS_LEAD(*(s2-2)))
            ) {
                /* part of a surrogate pair, leave >=d800 */
            } లేకపోతే {
                /* BMP code point - may be surrogate code point - make <d800 */
                c2-=0x2800;
            }
        }

        cmpRes=c1-c2;
        ఆపు;
    }

    ఒకవేళ(సరిపోలిక_Len1) {
        *సరిపోలిక_Len1=static_cast<int32_t>(m1-org1);
        *సరిపోలిక_Len2=static_cast<int32_t>(m2-org2);
    }
    తిరిగి cmpRes;
}

/* internal function */
U_CFUNC int32_t
u_strcmpFold(స్థిరరాశి char16_t *s1, int32_t length1,
             స్థిరరాశి char16_t *s2, int32_t length2,
             uint32_t options,
             UError_సంకేతం *p_Error_సంకేతం) {
    తిరిగి _cmpFold(s1, length1, s2, length2, options, శూన్యసూచిక, శూన్యసూచిక, p_Error_సంకేతం);
}

/* public API functions */

U_CAPI int32_t U_EXPORT2
u_strCaseCompare(స్థిరరాశి char16_t *s1, int32_t length1,
                 స్థిరరాశి char16_t *s2, int32_t length2,
                 uint32_t options,
                 UError_సంకేతం *p_Error_సంకేతం) {
    /* argument checking */
    ఒకవేళ (p_Error_సంకేతం == శూన్యసూచిక || U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి 0;
    }
    ఒకవేళ(s1==శూన్యసూచిక || length1<-1 || s2==శూన్యసూచిక || length2<-1) {
        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి 0;
    }
    తిరిగి u_strcmpFold(s1, length1, s2, length2,
                        options|U_COMPARE_IGNORE_CASE,
                        p_Error_సంకేతం);
}

U_CAPI int32_t U_EXPORT2
u_strcasecmp(స్థిరరాశి char16_t *s1, స్థిరరాశి char16_t *s2, uint32_t options) {
    UError_సంకేతం error_సంకేతం=U_ZERO_ERROR;
    తిరిగి u_strcmpFold(s1, -1, s2, -1,
                        options|U_COMPARE_IGNORE_CASE,
                        &error_సంకేతం);
}

U_CAPI int32_t U_EXPORT2
u_memcasecmp(స్థిరరాశి char16_t *s1, స్థిరరాశి char16_t *s2, int32_t length, uint32_t options) {
    UError_సంకేతం error_సంకేతం=U_ZERO_ERROR;
    తిరిగి u_strcmpFold(s1, length, s2, length,
                        options|U_COMPARE_IGNORE_CASE,
                        &error_సంకేతం);
}

U_CAPI int32_t U_EXPORT2
u_strncasecmp(స్థిరరాశి char16_t *s1, స్థిరరాశి char16_t *s2, int32_t n, uint32_t options) {
    UError_సంకేతం error_సంకేతం=U_ZERO_ERROR;
    తిరిగి u_strcmpFold(s1, n, s2, n,
                        options|(U_COMPARE_IGNORE_CASE|_STRNCMP_STYLE),
                        &error_సంకేతం);
}

/* internal API - detect length of shared prefix */
U_CAPI శూన్య
u_case_Insensitive_ఉపసర్గ_సరిపోలిక(స్థిరరాశి char16_t *s1, int32_t length1,
                             స్థిరరాశి char16_t *s2, int32_t length2,
                             uint32_t options,
                             int32_t *సరిపోలిక_Len1, int32_t *సరిపోలిక_Len2,
                             UError_సంకేతం *p_Error_సంకేతం) {
    _cmpFold(s1, length1, s2, length2, options,
        సరిపోలిక_Len1, సరిపోలిక_Len2, p_Error_సంకేతం);
}
