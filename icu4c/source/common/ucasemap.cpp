// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*
*   Copyright (C) 2005-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  ucasemap.cpp
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2005may06
*   created by: Markus W. Scherer
*
*   Case mapping service object and functions using it.
*/

#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "unicode/brkiter.h"
#అన్తర్భావ "unicode/bytestream.h"
#అన్తర్భావ "unicode/casemap.h"
#అన్తర్భావ "unicode/edits.h"
#అన్తర్భావ "unicode/stringoptions.h"
#అన్తర్భావ "unicode/stringpiece.h"
#అన్తర్భావ "unicode/ubrk.h"
#అన్తర్భావ "unicode/uloc.h"
#అన్తర్భావ "unicode/ustring.h"
#అన్తర్భావ "unicode/ucasemap.h"
#ఒకవేళ !UCONFIG_NO_BREAK_ITERATION
#అన్తర్భావ "unicode/utext.h"
#అయితే_ముగింపు
#అన్తర్భావ "unicode/utf.h"
#అన్తర్భావ "unicode/utf8.h"
#అన్తర్భావ "unicode/utf16.h"
#అన్తర్భావ "bytesinkutil.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "cstring.h"
#అన్తర్భావ "uassert.h"
#అన్తర్భావ "ucase.h"
#అన్తర్భావ "ucasemap_imp.h"

U_NAMESPACE_USE

/* UCaseMap service object -------------------------------------------------- */

// The public type, unicode/ucasemap.h struct UCaseMap, is an opaque C type.
// We define a parallel C++ type in the ICU namespace for entry point renaming.

U_NAMESPACE_BEGIN

UCaseMapImpl::UCaseMapImpl(స్థిరరాశి అక్షరం *localeID, uint32_t opts, UError_సంకేతం *p_Error_సంకేతం) :
#ఒకవేళ !UCONFIG_NO_BREAK_ITERATION
        iter(శూన్యసూచిక),
#అయితే_ముగింపు
        caseLocale(UCASE_LOC_UNKNOWN), options(opts) {
    ucasemap_సమితి_Locale(reinterpret_cast<UCaseMap *>(ఇది), localeID, p_Error_సంకేతం);
}

UCaseMapImpl::~UCaseMapImpl() {
#ఒకవేళ !UCONFIG_NO_BREAK_ITERATION
    తొలగించు iter;
#అయితే_ముగింపు
}

U_NAMESPACE_చివర

U_CAPI UCaseMap * U_EXPORT2
ucasemap_open(స్థిరరాశి అక్షరం *locale, uint32_t options, UError_సంకేతం *p_Error_సంకేతం) {
    ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి శూన్యసూచిక;
    }
    UCaseMapImpl *csm = కొత్త UCaseMapImpl(locale, options, p_Error_సంకేతం);
    ఒకవేళ(csm==శూన్యసూచిక) {
        *p_Error_సంకేతం = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి శూన్యసూచిక;
    } లేకపోతే ఒకవేళ (U_FAILURE(*p_Error_సంకేతం)) {
        తొలగించు csm;
        తిరిగి శూన్యసూచిక;
    }
    తిరిగి reinterpret_cast<UCaseMap *>(csm);
}

U_CAPI శూన్య U_EXPORT2
ucasemap_close(UCaseMap *csm) {
    తొలగించు reinterpret_cast<UCaseMapImpl *>(csm);
}

U_CAPI స్థిరరాశి అక్షరం* U_EXPORT2
ucasemap_getLocale(స్థిరరాశి UCaseMap* csm U_LIFETIME_BOUND) {
    తిరిగి reinterpret_cast<స్థిరరాశి UCaseMapImpl *>(csm)->locale;
}

U_CAPI uint32_t U_EXPORT2
ucasemap_getOptions(స్థిరరాశి UCaseMap *csm) {
    తిరిగి reinterpret_cast<స్థిరరాశి UCaseMapImpl *>(csm)->options;
}

U_CAPI శూన్య U_EXPORT2
ucasemap_సమితి_Locale(UCaseMap *csm, స్థిరరాశి అక్షరం *locale, UError_సంకేతం *p_Error_సంకేతం) {
    ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి;
    }
    స్వయంచాలక *impl = reinterpret_cast<UCaseMapImpl *>(csm);
    ఒకవేళ (locale != శూన్యసూచిక && *locale == 0) {
        impl->locale[0] = 0;
        impl->caseLocale = UCASE_LOC_మూలం;
        తిరిగి;
    }

    UError_సంకేతం బఫరు_స్థితి = U_ZERO_ERROR;
    int32_t length=uloc_getName(locale, impl->locale, (int32_t)పరిమాణం(impl->locale), &బఫరు_స్థితి);
    ఒకవేళ(బఫరు_స్థితి==U_బఫరు_అధికప్రవాహం_ERROR || (U_SUCCESS(బఫరు_స్థితి) && length==పరిమాణం(impl->locale))) {
        బఫరు_స్థితి = U_ZERO_ERROR;
        /* we only really need the language code for case mappings */
        length=uloc_getLanguage(locale, impl->locale, (int32_t)పరిమాణం(impl->locale), &బఫరు_స్థితి);
    }
    ఒకవేళ(U_FAILURE(బఫరు_స్థితి)) {
        *p_Error_సంకేతం=బఫరు_స్థితి;
    } లేకపోతే ఒకవేళ(length==పరిమాణం(impl->locale)) {
        *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
    }
    ఒకవేళ(U_SUCCESS(*p_Error_సంకేతం)) {     
        impl->caseLocale = ucase_getCaseLocale(impl->locale);
    } లేకపోతే {
        impl->locale[0]=0;
        impl->caseLocale = UCASE_LOC_మూలం;
    }
}

U_CAPI శూన్య U_EXPORT2
ucasemap_సమితి_Options(UCaseMap *csm, uint32_t options, UError_సంకేతం *p_Error_సంకేతం) {
    ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి;
    }
    reinterpret_cast<UCaseMapImpl *>(csm)->options=options;
}

/* UTF-8 string case mappings ----------------------------------------------- */

/* TODO(markus): Move to a new, separate utf8case.cpp file. */

నామాకాశ {

/* append a full case mapping result, see UCASE_MAX_STRING_LENGTH */
అంతఃవరుస UBool
append_ఫలితం(int32_t cp_నిడివి, int32_t ఫలితం, స్థిరరాశి char16_t *s,
             ByteSink &sink, uint32_t options, icu::Edits *edits, UError_సంకేతం &error_సంకేతం) {
    U_ASSERT(U_SUCCESS(error_సంకేతం));

    /* decode the result */
    ఒకవేళ(ఫలితం<0) {
        /* (not) original code point */
        ఒకవేళ(edits!=శూన్యసూచిక) {
            edits->కూడు_Unchanged(cp_నిడివి);
        }
        ఒకవేళ((options & U_OMIT_UNCHANGED_పాఠ్యం) == 0) {
            ByteSinkUtil::append_సంకేతం_బిందువు(cp_నిడివి, ~ఫలితం, sink);
        }
    } లేకపోతే {
        ఒకవేళ(ఫలితం<=UCASE_MAX_STRING_నిడివి) {
            // string: "result" is the UTF-16 length
            తిరిగి ByteSinkUtil::appendChange(cp_నిడివి, s, ఫలితం, sink, edits, error_సంకేతం);
        } లేకపోతే {
            ByteSinkUtil::append_సంకేతం_బిందువు(cp_నిడివి, ఫలితం, sink, edits);
        }
    }
    తిరిగి నిజం;
}

// See unicode/utf8.h U8_APPEND_UNSAFE().
అంతఃవరుస uint8_t getTwoByteLead(UChar32 c) { తిరిగి static_cast<uint8_t>((c >> 6) | 0xc0); }
అంతఃవరుస uint8_t getTwoByteTrail(UChar32 c) { తిరిగి static_cast<uint8_t>((c & 0x3f) | 0x80); }

UChar32 U_CALLCONV
utf8_caseContextIterator(శూన్య *context, int8_t dir) {
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
            U8_PREV((స్థిరరాశి uint8_t *)csc->p, csc->ప్రారంభం, csc->సూచీ, c);
            తిరిగి c;
        }
    } లేకపోతే {
        ఒకవేళ(csc->సూచీ<csc->హద్దు) {
            U8_తదుపరి((స్థిరరాశి uint8_t *)csc->p, csc->సూచీ, csc->హద్దు, c);
            తిరిగి c;
        }
    }
    తిరిగి U_SENTINEL;
}

/**
 * caseLocale >= 0: Lowercases [srcStart..srcLimit[ but takes context [0..srcLength[ into account.
 * caseLocale < 0: Case-folds [srcStart..srcLimit[.
 */
శూన్య గమ్యం_క్రింది(int32_t caseLocale, uint32_t options,
             స్థిరరాశి uint8_t *src, UCaseContext *csc, int32_t src_ప్రారంభం, int32_t src_హద్దు,
             icu::ByteSink &sink, icu::Edits *edits, UError_సంకేతం &error_సంకేతం) {
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
    int32_t prev = src_ప్రారంభం;
    int32_t src_సూచీ = src_ప్రారంభం;
    కొరకు (;;) {
        // fast path for simple cases
        int32_t cp_ప్రారంభం;
        UChar32 c;
        కొరకు (;;) {
            ఒకవేళ (U_FAILURE(error_సంకేతం) || src_సూచీ >= src_హద్దు) {
                c = U_SENTINEL;
                ఆపు;
            }
            uint8_t lead = src[src_సూచీ++];
            ఒకవేళ (lead <= 0x7f) {
                int8_t d = latin_గమ్యం_క్రింది[lead];
                ఒకవేళ (d == LatinCase::EXC) {
                    cp_ప్రారంభం = src_సూచీ - 1;
                    c = lead;
                    ఆపు;
                }
                ఒకవేళ (d == 0) { కొనసాగు; }
                ByteSinkUtil::appendUnchanged(src + prev, src_సూచీ - 1 - prev,
                                              sink, options, edits, error_సంకేతం);
                అక్షరం ascii = static_cast<అక్షరం>(lead + d);
                sink.Append(&ascii, 1);
                ఒకవేళ (edits != శూన్యసూచిక) {
                    edits->కూడు_Replace(1, 1);
                }
                prev = src_సూచీ;
                కొనసాగు;
            } లేకపోతే ఒకవేళ (lead < 0xe3) {
                uint8_t t;
                ఒకవేళ (0xc2 <= lead && lead <= 0xc5 && src_సూచీ < src_హద్దు &&
                        (t = src[src_సూచీ] - 0x80) <= 0x3f) {
                    // U+0080..U+017F
                    ++src_సూచీ;
                    c = ((lead - 0xc0) << 6) | t;
                    int8_t d = latin_గమ్యం_క్రింది[c];
                    ఒకవేళ (d == LatinCase::EXC) {
                        cp_ప్రారంభం = src_సూచీ - 2;
                        ఆపు;
                    }
                    ఒకవేళ (d == 0) { కొనసాగు; }
                    ByteSinkUtil::appendUnchanged(src + prev, src_సూచీ - 2 - prev,
                                                  sink, options, edits, error_సంకేతం);
                    ByteSinkUtil::append_Two_బైట్లు(c + d, sink);
                    ఒకవేళ (edits != శూన్యసూచిక) {
                        edits->కూడు_Replace(2, 2);
                    }
                    prev = src_సూచీ;
                    కొనసాగు;
                }
            } లేకపోతే ఒకవేళ ((lead <= 0xe9 || lead == 0xeb || lead == 0xec) &&
                    (src_సూచీ + 2) <= src_హద్దు &&
                    U8_IS_TRAIL(src[src_సూచీ]) && U8_IS_TRAIL(src[src_సూచీ + 1])) {
                // most of CJK: no case mappings
                src_సూచీ += 2;
                కొనసాగు;
            }
            cp_ప్రారంభం = --src_సూచీ;
            U8_తదుపరి(src, src_సూచీ, src_హద్దు, c);
            ఒకవేళ (c < 0) {
                // ill-formed UTF-8
                కొనసాగు;
            }
            uint16_t props = UTRIE2_GET16(trie, c);
            ఒకవేళ (UCASE_HAS_EXCEPTION(props)) { ఆపు; }
            int32_t delta;
            ఒకవేళ (!UCASE_IS_పై_OR_TITLE(props) || (delta = UCASE_GET_DELTA(props)) == 0) {
                కొనసాగు;
            }
            ByteSinkUtil::appendUnchanged(src + prev, cp_ప్రారంభం - prev,
                                          sink, options, edits, error_సంకేతం);
            ByteSinkUtil::append_సంకేతం_బిందువు(src_సూచీ - cp_ప్రారంభం, c + delta, sink, edits);
            prev = src_సూచీ;
        }
        ఒకవేళ (c < 0) {
            ఆపు;
        }
        // slow path
        స్థిరరాశి char16_t *s;
        ఒకవేళ (caseLocale >= 0) {
            csc->cp_ప్రారంభం = cp_ప్రారంభం;
            csc->cp_హద్దు = src_సూచీ;
            c = ucase_గమ్యం_నిండిన_క్రింది(c, utf8_caseContextIterator, csc, &s, caseLocale);
        } లేకపోతే {
            c = ucase_గమ్యం_నిండిన_Folding(c, &s, options);
        }
        ఒకవేళ (c >= 0) {
            ByteSinkUtil::appendUnchanged(src + prev, cp_ప్రారంభం - prev,
                                          sink, options, edits, error_సంకేతం);
            append_ఫలితం(src_సూచీ - cp_ప్రారంభం, c, s, sink, options, edits, error_సంకేతం);
            prev = src_సూచీ;
        }
    }
    ByteSinkUtil::appendUnchanged(src + prev, src_సూచీ - prev,
                                  sink, options, edits, error_సంకేతం);
}

శూన్య గమ్యం_పై(int32_t caseLocale, uint32_t options,
             స్థిరరాశి uint8_t *src, UCaseContext *csc, int32_t src_నిడివి,
             icu::ByteSink &sink, icu::Edits *edits, UError_సంకేతం &error_సంకేతం) {
    స్థిరరాశి int8_t *latin_గమ్యం_పై;
    ఒకవేళ (caseLocale == UCASE_LOC_TURKISH) {
        latin_గమ్యం_పై = LatinCase::గమ్యం_పై_TR;
    } లేకపోతే {
        latin_గమ్యం_పై = LatinCase::గమ్యం_పై_సాధారణ;
    }
    స్థిరరాశి UTrie2 *trie = ucase_getTrie();
    int32_t prev = 0;
    int32_t src_సూచీ = 0;
    కొరకు (;;) {
        // fast path for simple cases
        int32_t cp_ప్రారంభం;
        UChar32 c;
        కొరకు (;;) {
            ఒకవేళ (U_FAILURE(error_సంకేతం) || src_సూచీ >= src_నిడివి) {
                c = U_SENTINEL;
                ఆపు;
            }
            uint8_t lead = src[src_సూచీ++];
            ఒకవేళ (lead <= 0x7f) {
                int8_t d = latin_గమ్యం_పై[lead];
                ఒకవేళ (d == LatinCase::EXC) {
                    cp_ప్రారంభం = src_సూచీ - 1;
                    c = lead;
                    ఆపు;
                }
                ఒకవేళ (d == 0) { కొనసాగు; }
                ByteSinkUtil::appendUnchanged(src + prev, src_సూచీ - 1 - prev,
                                              sink, options, edits, error_సంకేతం);
                అక్షరం ascii = static_cast<అక్షరం>(lead + d);
                sink.Append(&ascii, 1);
                ఒకవేళ (edits != శూన్యసూచిక) {
                    edits->కూడు_Replace(1, 1);
                }
                prev = src_సూచీ;
                కొనసాగు;
            } లేకపోతే ఒకవేళ (lead < 0xe3) {
                uint8_t t;
                ఒకవేళ (0xc2 <= lead && lead <= 0xc5 && src_సూచీ < src_నిడివి &&
                        (t = src[src_సూచీ] - 0x80) <= 0x3f) {
                    // U+0080..U+017F
                    ++src_సూచీ;
                    c = ((lead - 0xc0) << 6) | t;
                    int8_t d = latin_గమ్యం_పై[c];
                    ఒకవేళ (d == LatinCase::EXC) {
                        cp_ప్రారంభం = src_సూచీ - 2;
                        ఆపు;
                    }
                    ఒకవేళ (d == 0) { కొనసాగు; }
                    ByteSinkUtil::appendUnchanged(src + prev, src_సూచీ - 2 - prev,
                                                  sink, options, edits, error_సంకేతం);
                    ByteSinkUtil::append_Two_బైట్లు(c + d, sink);
                    ఒకవేళ (edits != శూన్యసూచిక) {
                        edits->కూడు_Replace(2, 2);
                    }
                    prev = src_సూచీ;
                    కొనసాగు;
                }
            } లేకపోతే ఒకవేళ ((lead <= 0xe9 || lead == 0xeb || lead == 0xec) &&
                    (src_సూచీ + 2) <= src_నిడివి &&
                    U8_IS_TRAIL(src[src_సూచీ]) && U8_IS_TRAIL(src[src_సూచీ + 1])) {
                // most of CJK: no case mappings
                src_సూచీ += 2;
                కొనసాగు;
            }
            cp_ప్రారంభం = --src_సూచీ;
            U8_తదుపరి(src, src_సూచీ, src_నిడివి, c);
            ఒకవేళ (c < 0) {
                // ill-formed UTF-8
                కొనసాగు;
            }
            uint16_t props = UTRIE2_GET16(trie, c);
            ఒకవేళ (UCASE_HAS_EXCEPTION(props)) { ఆపు; }
            int32_t delta;
            ఒకవేళ (UCASE_GET_TYPE(props) != UCASE_క్రింది || (delta = UCASE_GET_DELTA(props)) == 0) {
                కొనసాగు;
            }
            ByteSinkUtil::appendUnchanged(src + prev, cp_ప్రారంభం - prev,
                                          sink, options, edits, error_సంకేతం);
            ByteSinkUtil::append_సంకేతం_బిందువు(src_సూచీ - cp_ప్రారంభం, c + delta, sink, edits);
            prev = src_సూచీ;
        }
        ఒకవేళ (c < 0) {
            ఆపు;
        }
        // slow path
        csc->cp_ప్రారంభం = cp_ప్రారంభం;
        csc->cp_హద్దు = src_సూచీ;
        స్థిరరాశి char16_t *s;
        c = ucase_గమ్యం_నిండిన_పై(c, utf8_caseContextIterator, csc, &s, caseLocale);
        ఒకవేళ (c >= 0) {
            ByteSinkUtil::appendUnchanged(src + prev, cp_ప్రారంభం - prev,
                                          sink, options, edits, error_సంకేతం);
            append_ఫలితం(src_సూచీ - cp_ప్రారంభం, c, s, sink, options, edits, error_సంకేతం);
            prev = src_సూచీ;
        }
    }
    ByteSinkUtil::appendUnchanged(src + prev, src_సూచీ - prev,
                                  sink, options, edits, error_సంకేతం);
}

}  // namespace

#ఒకవేళ !UCONFIG_NO_BREAK_ITERATION

నామాకాశ {

constexpr uint8_t ACUTE_BYTE0 = u8"\u0301"[0];

constexpr uint8_t ACUTE_BYTE1 = u8"\u0301"[1];

/**
 * Input: c is a letter I with or without acute accent.
 * start is the index in src after c, and is less than segmentLimit.
 * If a plain i/I is followed by a plain j/J,
 * or an i/I with acute (precomposed or decomposed) is followed by a j/J with acute,
 * then we output accordingly.
 *
 * @return the src index after the titlecased sequence, or the start index if no Dutch IJ
 */
int32_t maybeTitleDutchIJ(స్థిరరాశి uint8_t *src, UChar32 c, int32_t ప్రారంభం, int32_t segment_హద్దు,
                          ByteSink &sink, uint32_t options, icu::Edits *edits, UError_సంకేతం &error_సంకేతం) {
    U_ASSERT(ప్రారంభం < segment_హద్దు);

    int32_t సూచీ = ప్రారంభం;
    తార్కిక withAcute = అబద్ధం;

    // If the conditions are met, then the following variables tell us what to output.
    int32_t unchanged1 = 0;  // code units before the j, or the whole sequence (0..3)
    తార్కిక doTitleJ = అబద్ధం;  // true if the j needs to be titlecased
    int32_t unchanged2 = 0;  // after the j (0 or 1)

    // next character after the first letter
    UChar32 c2;
    c2 = src[సూచీ++];

    // Is the first letter an i/I with accent?
    ఒకవేళ (c == u'I') {
        ఒకవేళ (c2 == ACUTE_BYTE0 && సూచీ < segment_హద్దు && src[సూచీ++] == ACUTE_BYTE1) {
            withAcute = నిజం;
            unchanged1 = 2;  // ACUTE is 2 code units in UTF-8
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
        ఒకవేళ ((సూచీ + 1) >= segment_హద్దు || src[సూచీ++] != ACUTE_BYTE0 || src[సూచీ++] != ACUTE_BYTE1) {
            తిరిగి ప్రారంభం;
        }
        ఒకవేళ (doTitleJ) {
            unchanged2 = 2;  // ACUTE is 2 code units in UTF-8
        } లేకపోతే {
            unchanged1 = unchanged1 + 2;    // ACUTE is 2 code units in UTF-8
        }
    }

    // There must not be another combining mark.
    ఒకవేళ (సూచీ < segment_హద్దు) {
        int32_t cp;
        int32_t i = సూచీ;
        U8_తదుపరి(src, i, segment_హద్దు, cp);
        uint32_t type_ముసుగు = U_GET_GC_ముసుగు(cp);
        ఒకవేళ ((type_ముసుగు & U_GC_M_ముసుగు) != 0) {
            తిరిగి ప్రారంభం;
        }
    }

    // Output the rest of the Dutch IJ.
    ByteSinkUtil::appendUnchanged(src + ప్రారంభం, unchanged1, sink, options, edits, error_సంకేతం);
    ప్రారంభం += unchanged1;
    ఒకవేళ (doTitleJ) {
        ByteSinkUtil::append_సంకేతం_బిందువు(1, u'J', sink, edits);
        ++ప్రారంభం;
    }
    ByteSinkUtil::appendUnchanged(src + ప్రారంభం, unchanged2, sink, options, edits, error_సంకేతం);

    U_ASSERT(ప్రారంభం + unchanged2 == సూచీ);
    తిరిగి సూచీ;
}

}  // namespace

U_CFUNC శూన్య U_CALLCONV
ucasemap_అంతర్గత_UTF8_గమ్యం_Title(
        int32_t caseLocale, uint32_t options, BreakIterator *iter,
        స్థిరరాశి uint8_t *src, int32_t src_నిడివి,
        ByteSink &sink, icu::Edits *edits,
        UError_సంకేతం &error_సంకేతం) {
    ఒకవేళ (!ustrcase_checkTitleAdjustmentOptions(options, error_సంకేతం)) {
        తిరిగి;
    }

    /* set up local variables */
    UCaseContext csc=UCASECONTEXT_INITIALIZER;
    csc.p=(శూన్య *)src;
    csc.హద్దు=src_నిడివి;
    int32_t prev=0;
    UBool is_మొదటి_సూచీ=నిజం;

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
            /* find and copy skipped characters [prev..titleStart[ */
            int32_t title_ప్రారంభం=prev;
            int32_t title_హద్దు=prev;
            UChar32 c;
            U8_తదుపరి(src, title_హద్దు, సూచీ, c);
            ఒకవేళ ((options&U_TITLECASE_NO_BREAK_ADJUSTMENT)==0) {
                // Adjust the titlecasing index to the next cased character,
                // or to the next letter/number/symbol/private use.
                // Stop with titleStart<titleLimit<=index
                // if there is a character to be titlecased,
                // or else stop with titleStart==titleLimit==index.
                UBool గమ్యం_Cased = (options&U_TITLECASE_ADJUST_గమ్యం_CASED) != 0;
                అయితే (గమ్యం_Cased ? UCASE_NONE==ucase_getType(c) : !ustrcase_isLNS(c)) {
                    title_ప్రారంభం=title_హద్దు;
                    ఒకవేళ(title_హద్దు==సూచీ) {
                        ఆపు;
                    }
                    U8_తదుపరి(src, title_హద్దు, సూచీ, c);
                }
                ఒకవేళ (prev < title_ప్రారంభం) {
                    ఒకవేళ (!ByteSinkUtil::appendUnchanged(src+prev, title_ప్రారంభం-prev,
                                                       sink, options, edits, error_సంకేతం)) {
                        తిరిగి;
                    }
                }
            }

            ఒకవేళ(title_ప్రారంభం<title_హద్దు) {
                /* titlecase c which is from [titleStart..titleLimit[ */
                ఒకవేళ(c>=0) {
                    csc.cp_ప్రారంభం=title_ప్రారంభం;
                    csc.cp_హద్దు=title_హద్దు;
                    స్థిరరాశి char16_t *s;
                    c=ucase_గమ్యం_నిండిన_Title(c, utf8_caseContextIterator, &csc, &s, caseLocale);
                    ఒకవేళ (!append_ఫలితం(title_హద్దు-title_ప్రారంభం, c, s, sink, options, edits, error_సంకేతం)) {
                        తిరిగి;
                    }
                } లేకపోతే {
                    // Malformed UTF-8.
                    ఒకవేళ (!ByteSinkUtil::appendUnchanged(src+title_ప్రారంభం, title_హద్దు-title_ప్రారంభం,
                                                       sink, options, edits, error_సంకేతం)) {
                        తిరిగి;
                    }
                }

                /* Special case Dutch IJ titlecasing */
                ఒకవేళ (title_హద్దు < సూచీ &&
                    caseLocale == UCASE_LOC_DUTCH) {
                    ఒకవేళ (c < 0) {
                        c = ~c;
                    }

                    ఒకవేళ (c == u'I' || c == u'Í') {
                        title_హద్దు = maybeTitleDutchIJ(src, c, title_హద్దు, సూచీ, sink, options, edits, error_సంకేతం);
                    }
                }

                /* lowercase [titleLimit..index[ */
                ఒకవేళ(title_హద్దు<సూచీ) {
                    ఒకవేళ((options&U_TITLECASE_NO_LOWERCASE)==0) {
                        /* Normal operation: Lowercase the rest of the word. */
                        గమ్యం_క్రింది(caseLocale, options,
                                src, &csc, title_హద్దు, సూచీ,
                                sink, edits, error_సంకేతం);
                        ఒకవేళ(U_FAILURE(error_సంకేతం)) {
                            తిరిగి;
                        }
                    } లేకపోతే {
                        /* Optionally just copy the rest of the word unchanged. */
                        ఒకవేళ (!ByteSinkUtil::appendUnchanged(src+title_హద్దు, సూచీ-title_హద్దు,
                                                           sink, options, edits, error_సంకేతం)) {
                            తిరిగి;
                        }
                    }
                }
            }
        }

        prev=సూచీ;
    }
}

#అయితే_ముగింపు

U_NAMESPACE_BEGIN
నామాకాశ Greek_పై {

UBool isFollowedByCasedLetter(స్థిరరాశి uint8_t *s, int32_t i, int32_t length) {
    అయితే (i < length) {
        UChar32 c;
        U8_తదుపరి(s, i, length, c);
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

// Keep this consistent with the UTF-16 version in ustrcase.cpp and the Java version in CaseMap.java.
శూన్య గమ్యం_పై(uint32_t options,
             స్థిరరాశి uint8_t *src, int32_t src_నిడివి,
             ByteSink &sink, Edits *edits,
             UError_సంకేతం &error_సంకేతం) {
    uint32_t state = 0;
    కొరకు (int32_t i = 0; i < src_నిడివి;) {
        int32_t తదుపరి_సూచీ = i;
        UChar32 c;
        U8_తదుపరి(src, తదుపరి_సూచీ, src_నిడివి, c);
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
                దత్తాంశం |= (state & AFTER_VOWEL_WITH_PRECOMPOSED_ACCENT) != 0 ? HAS_DIALYTIKA
                                                                           : HAS_COMBINING_DIALYTIKA;
            }
            int32_t అంకె_Ypogegrammeni = 0;  // Map each one to a trailing, spacing, capital iota.
            ఒకవేళ ((దత్తాంశం & HAS_YPOGEGRAMMENI) != 0) {
                అంకె_Ypogegrammeni = 1;
            }
            స్థిరరాశి UBool hasPrecomposedAccent = (దత్తాంశం & HAS_ACCENT) != 0;
            // Skip combining diacritics after this Greek letter.
            int32_t తదుపరి_తదుపరి_సూచీ = తదుపరి_సూచీ;
            అయితే (తదుపరి_సూచీ < src_నిడివి) {
                UChar32 c2;
                U8_తదుపరి(src, తదుపరి_తదుపరి_సూచీ, src_నిడివి, c2);
                uint32_t diacritic_దత్తాంశం = get_Diacritic_దత్తాంశం(c2);
                ఒకవేళ (diacritic_దత్తాంశం != 0) {
                    దత్తాంశం |= diacritic_దత్తాంశం;
                    ఒకవేళ ((diacritic_దత్తాంశం & HAS_YPOGEGRAMMENI) != 0) {
                        ++అంకె_Ypogegrammeni;
                    }
                    తదుపరి_సూచీ = తదుపరి_తదుపరి_సూచీ;
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
                U_ASSERT(0x370 <= upper && upper <= 0x3ff);  // 2-byte UTF-8, main Greek block
                change = (i + 2) > తదుపరి_సూచీ ||
                        src[i] != getTwoByteLead(upper) || src[i + 1] != getTwoByteTrail(upper) ||
                        అంకె_Ypogegrammeni > 0;
                int32_t i2 = i + 2;
                ఒకవేళ ((దత్తాంశం & HAS_EITHER_DIALYTIKA) != 0) {
                    change |= (i2 + 2) > తదుపరి_సూచీ ||
                            src[i2] != static_cast<uint8_t>(u8"\u0308"[0]) ||
                            src[i2 + 1] != static_cast<uint8_t>(u8"\u0308"[1]);
                    i2 += 2;
                }
                ఒకవేళ (కూడు_Tonos) {
                    change |= (i2 + 2) > తదుపరి_సూచీ ||
                            src[i2] != static_cast<uint8_t>(u8"\u0301"[0]) ||
                            src[i2 + 1] != static_cast<uint8_t>(u8"\u0301"[1]);
                    i2 += 2;
                }
                int32_t old_నిడివి = తదుపరి_సూచీ - i;
                int32_t new_నిడివి = (i2 - i) + అంకె_Ypogegrammeni * 2;  // 2 bytes per U+0399
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
                ByteSinkUtil::append_Two_బైట్లు(upper, sink);
                ఒకవేళ ((దత్తాంశం & HAS_EITHER_DIALYTIKA) != 0) {
                    sink.AppendU8(u8"\u0308", 2);  // restore or add a dialytika
                }
                ఒకవేళ (కూడు_Tonos) {
                    sink.AppendU8(u8"\u0301", 2);
                }
                అయితే (అంకె_Ypogegrammeni > 0) {
                    sink.AppendU8(u8"\u0399", 2);
                    --అంకె_Ypogegrammeni;
                }
            }
        } లేకపోతే ఒకవేళ(c>=0) {
            స్థిరరాశి char16_t *s;
            c=ucase_గమ్యం_నిండిన_పై(c, శూన్యసూచిక, శూన్యసూచిక, &s, UCASE_LOC_GREEK);
            ఒకవేళ (!append_ఫలితం(తదుపరి_సూచీ - i, c, s, sink, options, edits, error_సంకేతం)) {
                తిరిగి;
            }
        } లేకపోతే {
            // Malformed UTF-8.
            ఒకవేళ (!ByteSinkUtil::appendUnchanged(src+i, తదుపరి_సూచీ-i,
                                               sink, options, edits, error_సంకేతం)) {
                తిరిగి;
            }
        }
        i = తదుపరి_సూచీ;
        state = తదుపరి_State;
    }
}

}  // namespace GreekUpper
U_NAMESPACE_చివర

స్థిర శూన్య U_CALLCONV
ucasemap_అంతర్గత_UTF8_గమ్యం_క్రింది(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_వాడని
                             స్థిరరాశి uint8_t *src, int32_t src_నిడివి,
                             icu::ByteSink &sink, icu::Edits *edits,
                             UError_సంకేతం &error_సంకేతం) {
    UCaseContext csc=UCASECONTEXT_INITIALIZER;
    csc.p=(శూన్య *)src;
    csc.హద్దు=src_నిడివి;
    గమ్యం_క్రింది(
        caseLocale, options,
        src, &csc, 0, src_నిడివి,
        sink, edits, error_సంకేతం);
}

స్థిర శూన్య U_CALLCONV
ucasemap_అంతర్గత_UTF8_గమ్యం_పై(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_వాడని
                             స్థిరరాశి uint8_t *src, int32_t src_నిడివి,
                             icu::ByteSink &sink, icu::Edits *edits,
                             UError_సంకేతం &error_సంకేతం) {
    ఒకవేళ (caseLocale == UCASE_LOC_GREEK) {
        Greek_పై::గమ్యం_పై(options, src, src_నిడివి, sink, edits, error_సంకేతం);
    } లేకపోతే {
        UCaseContext csc=UCASECONTEXT_INITIALIZER;
        csc.p=(శూన్య *)src;
        csc.హద్దు=src_నిడివి;
        గమ్యం_పై(
            caseLocale, options,
            src, &csc, src_నిడివి,
            sink, edits, error_సంకేతం);
    }
}

స్థిర శూన్య U_CALLCONV
ucasemap_అంతర్గత_UTF8_Fold(int32_t /* caseLocale */, uint32_t options, UCASEMAP_BREAK_ITERATOR_వాడని
                          స్థిరరాశి uint8_t *src, int32_t src_నిడివి,
                          icu::ByteSink &sink, icu::Edits *edits,
                          UError_సంకేతం &error_సంకేతం) {
    గమ్యం_క్రింది(
        -1, options,
        src, శూన్యసూచిక, 0, src_నిడివి,
        sink, edits, error_సంకేతం);
}

శూన్య
ucasemap_mapUTF8(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_PARAM
                 స్థిరరాశి అక్షరం *src, int32_t src_నిడివి,
                 UTF8CaseMapper *stringCaseMapper,
                 icu::ByteSink &sink, icu::Edits *edits,
                 UError_సంకేతం &error_సంకేతం) {
    /* check argument values */
    ఒకవేళ (U_FAILURE(error_సంకేతం)) {
        తిరిగి;
    }
    ఒకవేళ ((src == శూన్యసూచిక && src_నిడివి != 0) || src_నిడివి < -1) {
        error_సంకేతం = U_ILLEGAL_వాదం_ERROR;
        తిరిగి;
    }

    // Get the string length.
    ఒకవేళ (src_నిడివి == -1) {
        src_నిడివి = static_cast<int32_t>(uprv_strlen(src));
    }

    ఒకవేళ (edits != శూన్యసూచిక && (options & U_EDITS_NO_RESET) == 0) {
        edits->reset();
    }
    stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                     reinterpret_cast<స్థిరరాశి uint8_t*>(src), src_నిడివి, sink, edits, error_సంకేతం);
    sink.Flush();
    ఒకవేళ (U_SUCCESS(error_సంకేతం)) {
        ఒకవేళ (edits != శూన్యసూచిక) {
            edits->copy_Error_గమ్యం(error_సంకేతం);
        }
    }
}

int32_t
ucasemap_mapUTF8(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_PARAM
                 అక్షరం *dest, int32_t dest_సామర్థ్యం,
                 స్థిరరాశి అక్షరం *src, int32_t src_నిడివి,
                 UTF8CaseMapper *stringCaseMapper,
                 icu::Edits *edits,
                 UError_సంకేతం &error_సంకేతం) {
    /* check argument values */
    ఒకవేళ(U_FAILURE(error_సంకేతం)) {
        తిరిగి 0;
    }
    ఒకవేళ( dest_సామర్థ్యం<0 ||
        (dest==శూన్యసూచిక && dest_సామర్థ్యం>0) ||
        (src==శూన్యసూచిక && src_నిడివి!=0) || src_నిడివి<-1
    ) {
        error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి 0;
    }

    /* get the string length */
    ఒకవేళ(src_నిడివి==-1) {
        src_నిడివి = static_cast<int32_t>(uprv_strlen(src));
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
    int32_t reslen = ByteSinkUtil::ద్వారా_Byte_Sink_గమ్యం_Terminated_Chars(
        dest, dest_సామర్థ్యం,
        [&](ByteSink& sink, UError_సంకేతం& స్థితి_2) {
            stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                             reinterpret_cast<స్థిరరాశి uint8_t*>(src), src_నిడివి, sink, edits, స్థితి_2);
        },
        error_సంకేతం);
    ఒకవేళ (U_SUCCESS(error_సంకేతం) && edits != శూన్యసూచిక) {
        edits->copy_Error_గమ్యం(error_సంకేతం);
    }
    తిరిగి reslen;
}

/* public API functions */

U_CAPI int32_t U_EXPORT2
ucasemap_utf8_గమ్యం_క్రింది(స్థిరరాశి UCaseMap *csm,
                     అక్షరం *dest, int32_t dest_సామర్థ్యం,
                     స్థిరరాశి అక్షరం *src, int32_t src_నిడివి,
                     UError_సంకేతం *p_Error_సంకేతం) {
    స్వయంచాలక *impl = reinterpret_cast<స్థిరరాశి UCaseMapImpl *>(csm);
    తిరిగి ucasemap_mapUTF8(
        impl->caseLocale, impl->options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_సామర్థ్యం,
        src, src_నిడివి,
        ucasemap_అంతర్గత_UTF8_గమ్యం_క్రింది, శూన్యసూచిక, *p_Error_సంకేతం);
}

U_CAPI int32_t U_EXPORT2
ucasemap_utf8_గమ్యం_పై(స్థిరరాశి UCaseMap *csm,
                     అక్షరం *dest, int32_t dest_సామర్థ్యం,
                     స్థిరరాశి అక్షరం *src, int32_t src_నిడివి,
                     UError_సంకేతం *p_Error_సంకేతం) {
    స్వయంచాలక *impl = reinterpret_cast<స్థిరరాశి UCaseMapImpl *>(csm);
    తిరిగి ucasemap_mapUTF8(
        impl->caseLocale, impl->options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_సామర్థ్యం,
        src, src_నిడివి,
        ucasemap_అంతర్గత_UTF8_గమ్యం_పై, శూన్యసూచిక, *p_Error_సంకేతం);
}

U_CAPI int32_t U_EXPORT2
ucasemap_utf8FoldCase(స్థిరరాశి UCaseMap *csm,
                      అక్షరం *dest, int32_t dest_సామర్థ్యం,
                      స్థిరరాశి అక్షరం *src, int32_t src_నిడివి,
                      UError_సంకేతం *p_Error_సంకేతం) {
    స్వయంచాలక *impl = reinterpret_cast<స్థిరరాశి UCaseMapImpl *>(csm);
    తిరిగి ucasemap_mapUTF8(
        UCASE_LOC_మూలం, impl->options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_సామర్థ్యం,
        src, src_నిడివి,
        ucasemap_అంతర్గత_UTF8_Fold, శూన్యసూచిక, *p_Error_సంకేతం);
}

U_NAMESPACE_BEGIN

శూన్య CaseMap::utf8_గమ్యం_క్రింది(
        స్థిరరాశి అక్షరం *locale, uint32_t options,
        StringPiece src, ByteSink &sink, Edits *edits,
        UError_సంకేతం &error_సంకేతం) {
    ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        src.దత్తాంశం(), src.length(),
        ucasemap_అంతర్గత_UTF8_గమ్యం_క్రింది, sink, edits, error_సంకేతం);
}

శూన్య CaseMap::utf8_గమ్యం_పై(
        స్థిరరాశి అక్షరం *locale, uint32_t options,
        StringPiece src, ByteSink &sink, Edits *edits,
        UError_సంకేతం &error_సంకేతం) {
    ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        src.దత్తాంశం(), src.length(),
        ucasemap_అంతర్గత_UTF8_గమ్యం_పై, sink, edits, error_సంకేతం);
}

శూన్య CaseMap::utf8Fold(
        uint32_t options,
        StringPiece src, ByteSink &sink, Edits *edits,
        UError_సంకేతం &error_సంకేతం) {
    ucasemap_mapUTF8(
        UCASE_LOC_మూలం, options, UCASEMAP_BREAK_ITERATOR_NULL
        src.దత్తాంశం(), src.length(),
        ucasemap_అంతర్గత_UTF8_Fold, sink, edits, error_సంకేతం);
}

int32_t CaseMap::utf8_గమ్యం_క్రింది(
        స్థిరరాశి అక్షరం *locale, uint32_t options,
        స్థిరరాశి అక్షరం *src, int32_t src_నిడివి,
        అక్షరం *dest, int32_t dest_సామర్థ్యం, Edits *edits,
        UError_సంకేతం &error_సంకేతం) {
    తిరిగి ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_సామర్థ్యం,
        src, src_నిడివి,
        ucasemap_అంతర్గత_UTF8_గమ్యం_క్రింది, edits, error_సంకేతం);
}

int32_t CaseMap::utf8_గమ్యం_పై(
        స్థిరరాశి అక్షరం *locale, uint32_t options,
        స్థిరరాశి అక్షరం *src, int32_t src_నిడివి,
        అక్షరం *dest, int32_t dest_సామర్థ్యం, Edits *edits,
        UError_సంకేతం &error_సంకేతం) {
    తిరిగి ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_సామర్థ్యం,
        src, src_నిడివి,
        ucasemap_అంతర్గత_UTF8_గమ్యం_పై, edits, error_సంకేతం);
}

int32_t CaseMap::utf8Fold(
        uint32_t options,
        స్థిరరాశి అక్షరం *src, int32_t src_నిడివి,
        అక్షరం *dest, int32_t dest_సామర్థ్యం, Edits *edits,
        UError_సంకేతం &error_సంకేతం) {
    తిరిగి ucasemap_mapUTF8(
        UCASE_LOC_మూలం, options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_సామర్థ్యం,
        src, src_నిడివి,
        ucasemap_అంతర్గత_UTF8_Fold, edits, error_సంకేతం);
}

U_NAMESPACE_చివర
