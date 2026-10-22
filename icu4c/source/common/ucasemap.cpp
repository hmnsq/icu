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

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/brkiter.h"
#अन्तर्भाव "unicode/bytestream.h"
#अन्तर्भाव "unicode/casemap.h"
#अन्तर्भाव "unicode/edits.h"
#अन्तर्भाव "unicode/stringoptions.h"
#अन्तर्भाव "unicode/stringpiece.h"
#अन्तर्भाव "unicode/ubrk.h"
#अन्तर्भाव "unicode/uloc.h"
#अन्तर्भाव "unicode/ustring.h"
#अन्तर्भाव "unicode/ucasemap.h"
#यदि !UCONFIG_NO_BREAK_ITERATION
#अन्तर्भाव "unicode/utext.h"
#यदि_अन्तः
#अन्तर्भाव "unicode/utf.h"
#अन्तर्भाव "unicode/utf8.h"
#अन्तर्भाव "unicode/utf16.h"
#अन्तर्भाव "bytesinkutil.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "cstring.h"
#अन्तर्भाव "uassert.h"
#अन्तर्भाव "ucase.h"
#अन्तर्भाव "ucasemap_imp.h"

U_NAMESPACE_USE

/* UCaseMap service object -------------------------------------------------- */

// The public type, unicode/ucasemap.h struct UCaseMap, is an opaque C type.
// We define a parallel C++ type in the ICU namespace for entry point renaming.

U_NAMESPACE_BEGIN

UCaseMapImpl::UCaseMapImpl(ध्रुव वर्ण *localeID, uint32_t opts, UError_संहिता *p_Error_संहिता) :
#यदि !UCONFIG_NO_BREAK_ITERATION
        iter(शून्यसूचकः),
#यदि_अन्तः
        caseLocale(UCASE_LOC_UNKNOWN), options(opts) {
    ucasemap_समुच्चयः_Locale(reinterpret_cast<UCaseMap *>(एतत्), localeID, p_Error_संहिता);
}

UCaseMapImpl::~UCaseMapImpl() {
#यदि !UCONFIG_NO_BREAK_ITERATION
    विलोपय iter;
#यदि_अन्तः
}

U_NAMESPACE_अन्तः

U_CAPI UCaseMap * U_EXPORT2
ucasemap_open(ध्रुव वर्ण *locale, uint32_t options, UError_संहिता *p_Error_संहिता) {
    यदि(U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम शून्यसूचकः;
    }
    UCaseMapImpl *csm = नूतन UCaseMapImpl(locale, options, p_Error_संहिता);
    यदि(csm==शून्यसूचकः) {
        *p_Error_संहिता = U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम शून्यसूचकः;
    } अन्यथा यदि (U_FAILURE(*p_Error_संहिता)) {
        विलोपय csm;
        प्रत्यागम शून्यसूचकः;
    }
    प्रत्यागम reinterpret_cast<UCaseMap *>(csm);
}

U_CAPI शून्य U_EXPORT2
ucasemap_close(UCaseMap *csm) {
    विलोपय reinterpret_cast<UCaseMapImpl *>(csm);
}

U_CAPI ध्रुव वर्ण* U_EXPORT2
ucasemap_getLocale(ध्रुव UCaseMap* csm U_LIFETIME_BOUND) {
    प्रत्यागम reinterpret_cast<ध्रुव UCaseMapImpl *>(csm)->locale;
}

U_CAPI uint32_t U_EXPORT2
ucasemap_getOptions(ध्रुव UCaseMap *csm) {
    प्रत्यागम reinterpret_cast<ध्रुव UCaseMapImpl *>(csm)->options;
}

U_CAPI शून्य U_EXPORT2
ucasemap_समुच्चयः_Locale(UCaseMap *csm, ध्रुव वर्ण *locale, UError_संहिता *p_Error_संहिता) {
    यदि(U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम;
    }
    स्वचालित *impl = reinterpret_cast<UCaseMapImpl *>(csm);
    यदि (locale != शून्यसूचकः && *locale == 0) {
        impl->locale[0] = 0;
        impl->caseLocale = UCASE_LOC_मूलम्;
        प्रत्यागम;
    }

    UError_संहिता बफर_स्थितिः = U_ZERO_ERROR;
    int32_t length=uloc_getName(locale, impl->locale, (int32_t)परिमाण(impl->locale), &बफर_स्थितिः);
    यदि(बफर_स्थितिः==U_बफर_अतिप्रवाहः_ERROR || (U_SUCCESS(बफर_स्थितिः) && length==परिमाण(impl->locale))) {
        बफर_स्थितिः = U_ZERO_ERROR;
        /* we only really need the language code for case mappings */
        length=uloc_getLanguage(locale, impl->locale, (int32_t)परिमाण(impl->locale), &बफर_स्थितिः);
    }
    यदि(U_FAILURE(बफर_स्थितिः)) {
        *p_Error_संहिता=बफर_स्थितिः;
    } अन्यथा यदि(length==परिमाण(impl->locale)) {
        *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
    }
    यदि(U_SUCCESS(*p_Error_संहिता)) {     
        impl->caseLocale = ucase_getCaseLocale(impl->locale);
    } अन्यथा {
        impl->locale[0]=0;
        impl->caseLocale = UCASE_LOC_मूलम्;
    }
}

U_CAPI शून्य U_EXPORT2
ucasemap_समुच्चयः_Options(UCaseMap *csm, uint32_t options, UError_संहिता *p_Error_संहिता) {
    यदि(U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम;
    }
    reinterpret_cast<UCaseMapImpl *>(csm)->options=options;
}

/* UTF-8 string case mappings ----------------------------------------------- */

/* TODO(markus): Move to a new, separate utf8case.cpp file. */

नामाकाश {

/* append a full case mapping result, see UCASE_MAX_STRING_LENGTH */
अन्तःस्थ UBool
append_परिणाम(int32_t cp_दीर्घता, int32_t परिणाम, ध्रुव char16_t *s,
             ByteSink &sink, uint32_t options, icu::Edits *edits, UError_संहिता &error_संहिता) {
    U_ASSERT(U_SUCCESS(error_संहिता));

    /* decode the result */
    यदि(परिणाम<0) {
        /* (not) original code point */
        यदि(edits!=शून्यसूचकः) {
            edits->सङ्कलय_Unchanged(cp_दीर्घता);
        }
        यदि((options & U_OMIT_UNCHANGED_TEXT) == 0) {
            ByteSinkUtil::append_संहिता_Point(cp_दीर्घता, ~परिणाम, sink);
        }
    } अन्यथा {
        यदि(परिणाम<=UCASE_MAX_STRING_दीर्घता) {
            // string: "result" is the UTF-16 length
            प्रत्यागम ByteSinkUtil::appendChange(cp_दीर्घता, s, परिणाम, sink, edits, error_संहिता);
        } अन्यथा {
            ByteSinkUtil::append_संहिता_Point(cp_दीर्घता, परिणाम, sink, edits);
        }
    }
    प्रत्यागम सत्य;
}

// See unicode/utf8.h U8_APPEND_UNSAFE().
अन्तःस्थ uint8_t getTwoByteLead(UChar32 c) { प्रत्यागम static_cast<uint8_t>((c >> 6) | 0xc0); }
अन्तःस्थ uint8_t getTwoByteTrail(UChar32 c) { प्रत्यागम static_cast<uint8_t>((c & 0x3f) | 0x80); }

UChar32 U_CALLCONV
utf8_caseContextIterator(शून्य *context, int8_t dir) {
    UCaseContext* csc = static_cast<UCaseContext*>(context);
    UChar32 c;

    यदि(dir<0) {
        /* reset for backward iteration */
        csc->सूचकांक=csc->cpStart;
        csc->dir=dir;
    } अन्यथा यदि(dir>0) {
        /* reset for forward iteration */
        csc->सूचकांक=csc->cp_सीमा;
        csc->dir=dir;
    } अन्यथा {
        /* continue current iteration direction */
        dir=csc->dir;
    }

    यदि(dir<0) {
        यदि(csc->start<csc->सूचकांक) {
            U8_पूर्व((ध्रुव uint8_t *)csc->p, csc->start, csc->सूचकांक, c);
            प्रत्यागम c;
        }
    } अन्यथा {
        यदि(csc->सूचकांक<csc->सीमा) {
            U8_अग्र((ध्रुव uint8_t *)csc->p, csc->सूचकांक, csc->सीमा, c);
            प्रत्यागम c;
        }
    }
    प्रत्यागम U_SENTINEL;
}

/**
 * caseLocale >= 0: Lowercases [srcStart..srcLimit[ but takes context [0..srcLength[ into account.
 * caseLocale < 0: Case-folds [srcStart..srcLimit[.
 */
शून्य गन्तव्यम्_अधस्तनम्(int32_t caseLocale, uint32_t options,
             ध्रुव uint8_t *src, UCaseContext *csc, int32_t srcStart, int32_t src_सीमा,
             icu::ByteSink &sink, icu::Edits *edits, UError_संहिता &error_संहिता) {
    ध्रुव int8_t *latin_गन्तव्यम्_अधस्तनम्;
    यदि (caseLocale == UCASE_LOC_मूलम् ||
            (caseLocale >= 0 ?
                !(caseLocale == UCASE_LOC_TURKISH || caseLocale == UCASE_LOC_LITHUANIAN) :
                (options & FOLD_CASE_OPTIONS_आवरणम्) == U_FOLD_CASE_DEFAULT)) {
        latin_गन्तव्यम्_अधस्तनम् = LatinCase::गन्तव्यम्_अधस्तनम्_प्रसामान्यम्;
    } अन्यथा {
        latin_गन्तव्यम्_अधस्तनम् = LatinCase::गन्तव्यम्_अधस्तनम्_TR_LT;
    }
    ध्रुव UTrie2 *trie = ucase_getTrie();
    int32_t पूर्व = srcStart;
    int32_t src_सूचकांक = srcStart;
    प्रति (;;) {
        // fast path for simple cases
        int32_t cpStart;
        UChar32 c;
        प्रति (;;) {
            यदि (U_FAILURE(error_संहिता) || src_सूचकांक >= src_सीमा) {
                c = U_SENTINEL;
                भङ्ग;
            }
            uint8_t lead = src[src_सूचकांक++];
            यदि (lead <= 0x7f) {
                int8_t d = latin_गन्तव्यम्_अधस्तनम्[lead];
                यदि (d == LatinCase::EXC) {
                    cpStart = src_सूचकांक - 1;
                    c = lead;
                    भङ्ग;
                }
                यदि (d == 0) { अनुवर्त; }
                ByteSinkUtil::appendUnchanged(src + पूर्व, src_सूचकांक - 1 - पूर्व,
                                              sink, options, edits, error_संहिता);
                वर्ण ascii = static_cast<वर्ण>(lead + d);
                sink.Append(&ascii, 1);
                यदि (edits != शून्यसूचकः) {
                    edits->सङ्कलय_Replace(1, 1);
                }
                पूर्व = src_सूचकांक;
                अनुवर्त;
            } अन्यथा यदि (lead < 0xe3) {
                uint8_t t;
                यदि (0xc2 <= lead && lead <= 0xc5 && src_सूचकांक < src_सीमा &&
                        (t = src[src_सूचकांक] - 0x80) <= 0x3f) {
                    // U+0080..U+017F
                    ++src_सूचकांक;
                    c = ((lead - 0xc0) << 6) | t;
                    int8_t d = latin_गन्तव्यम्_अधस्तनम्[c];
                    यदि (d == LatinCase::EXC) {
                        cpStart = src_सूचकांक - 2;
                        भङ्ग;
                    }
                    यदि (d == 0) { अनुवर्त; }
                    ByteSinkUtil::appendUnchanged(src + पूर्व, src_सूचकांक - 2 - पूर्व,
                                                  sink, options, edits, error_संहिता);
                    ByteSinkUtil::append_Two_अष्टकानि(c + d, sink);
                    यदि (edits != शून्यसूचकः) {
                        edits->सङ्कलय_Replace(2, 2);
                    }
                    पूर्व = src_सूचकांक;
                    अनुवर्त;
                }
            } अन्यथा यदि ((lead <= 0xe9 || lead == 0xeb || lead == 0xec) &&
                    (src_सूचकांक + 2) <= src_सीमा &&
                    U8_IS_TRAIL(src[src_सूचकांक]) && U8_IS_TRAIL(src[src_सूचकांक + 1])) {
                // most of CJK: no case mappings
                src_सूचकांक += 2;
                अनुवर्त;
            }
            cpStart = --src_सूचकांक;
            U8_अग्र(src, src_सूचकांक, src_सीमा, c);
            यदि (c < 0) {
                // ill-formed UTF-8
                अनुवर्त;
            }
            uint16_t props = UTRIE2_GET16(trie, c);
            यदि (UCASE_HAS_EXCEPTION(props)) { भङ्ग; }
            int32_t delta;
            यदि (!UCASE_IS_ऊर्ध्वस्थम्_OR_TITLE(props) || (delta = UCASE_GET_DELTA(props)) == 0) {
                अनुवर्त;
            }
            ByteSinkUtil::appendUnchanged(src + पूर्व, cpStart - पूर्व,
                                          sink, options, edits, error_संहिता);
            ByteSinkUtil::append_संहिता_Point(src_सूचकांक - cpStart, c + delta, sink, edits);
            पूर्व = src_सूचकांक;
        }
        यदि (c < 0) {
            भङ्ग;
        }
        // slow path
        ध्रुव char16_t *s;
        यदि (caseLocale >= 0) {
            csc->cpStart = cpStart;
            csc->cp_सीमा = src_सूचकांक;
            c = ucase_गन्तव्यम्_पूर्णम्_अधस्तनम्(c, utf8_caseContextIterator, csc, &s, caseLocale);
        } अन्यथा {
            c = ucase_गन्तव्यम्_पूर्णम्_Folding(c, &s, options);
        }
        यदि (c >= 0) {
            ByteSinkUtil::appendUnchanged(src + पूर्व, cpStart - पूर्व,
                                          sink, options, edits, error_संहिता);
            append_परिणाम(src_सूचकांक - cpStart, c, s, sink, options, edits, error_संहिता);
            पूर्व = src_सूचकांक;
        }
    }
    ByteSinkUtil::appendUnchanged(src + पूर्व, src_सूचकांक - पूर्व,
                                  sink, options, edits, error_संहिता);
}

शून्य गन्तव्यम्_ऊर्ध्वस्थम्(int32_t caseLocale, uint32_t options,
             ध्रुव uint8_t *src, UCaseContext *csc, int32_t src_दीर्घता,
             icu::ByteSink &sink, icu::Edits *edits, UError_संहिता &error_संहिता) {
    ध्रुव int8_t *latin_गन्तव्यम्_ऊर्ध्वस्थम्;
    यदि (caseLocale == UCASE_LOC_TURKISH) {
        latin_गन्तव्यम्_ऊर्ध्वस्थम् = LatinCase::गन्तव्यम्_ऊर्ध्वस्थम्_TR;
    } अन्यथा {
        latin_गन्तव्यम्_ऊर्ध्वस्थम् = LatinCase::गन्तव्यम्_ऊर्ध्वस्थम्_प्रसामान्यम्;
    }
    ध्रुव UTrie2 *trie = ucase_getTrie();
    int32_t पूर्व = 0;
    int32_t src_सूचकांक = 0;
    प्रति (;;) {
        // fast path for simple cases
        int32_t cpStart;
        UChar32 c;
        प्रति (;;) {
            यदि (U_FAILURE(error_संहिता) || src_सूचकांक >= src_दीर्घता) {
                c = U_SENTINEL;
                भङ्ग;
            }
            uint8_t lead = src[src_सूचकांक++];
            यदि (lead <= 0x7f) {
                int8_t d = latin_गन्तव्यम्_ऊर्ध्वस्थम्[lead];
                यदि (d == LatinCase::EXC) {
                    cpStart = src_सूचकांक - 1;
                    c = lead;
                    भङ्ग;
                }
                यदि (d == 0) { अनुवर्त; }
                ByteSinkUtil::appendUnchanged(src + पूर्व, src_सूचकांक - 1 - पूर्व,
                                              sink, options, edits, error_संहिता);
                वर्ण ascii = static_cast<वर्ण>(lead + d);
                sink.Append(&ascii, 1);
                यदि (edits != शून्यसूचकः) {
                    edits->सङ्कलय_Replace(1, 1);
                }
                पूर्व = src_सूचकांक;
                अनुवर्त;
            } अन्यथा यदि (lead < 0xe3) {
                uint8_t t;
                यदि (0xc2 <= lead && lead <= 0xc5 && src_सूचकांक < src_दीर्घता &&
                        (t = src[src_सूचकांक] - 0x80) <= 0x3f) {
                    // U+0080..U+017F
                    ++src_सूचकांक;
                    c = ((lead - 0xc0) << 6) | t;
                    int8_t d = latin_गन्तव्यम्_ऊर्ध्वस्थम्[c];
                    यदि (d == LatinCase::EXC) {
                        cpStart = src_सूचकांक - 2;
                        भङ्ग;
                    }
                    यदि (d == 0) { अनुवर्त; }
                    ByteSinkUtil::appendUnchanged(src + पूर्व, src_सूचकांक - 2 - पूर्व,
                                                  sink, options, edits, error_संहिता);
                    ByteSinkUtil::append_Two_अष्टकानि(c + d, sink);
                    यदि (edits != शून्यसूचकः) {
                        edits->सङ्कलय_Replace(2, 2);
                    }
                    पूर्व = src_सूचकांक;
                    अनुवर्त;
                }
            } अन्यथा यदि ((lead <= 0xe9 || lead == 0xeb || lead == 0xec) &&
                    (src_सूचकांक + 2) <= src_दीर्घता &&
                    U8_IS_TRAIL(src[src_सूचकांक]) && U8_IS_TRAIL(src[src_सूचकांक + 1])) {
                // most of CJK: no case mappings
                src_सूचकांक += 2;
                अनुवर्त;
            }
            cpStart = --src_सूचकांक;
            U8_अग्र(src, src_सूचकांक, src_दीर्घता, c);
            यदि (c < 0) {
                // ill-formed UTF-8
                अनुवर्त;
            }
            uint16_t props = UTRIE2_GET16(trie, c);
            यदि (UCASE_HAS_EXCEPTION(props)) { भङ्ग; }
            int32_t delta;
            यदि (UCASE_GET_TYPE(props) != UCASE_अधस्तनम् || (delta = UCASE_GET_DELTA(props)) == 0) {
                अनुवर्त;
            }
            ByteSinkUtil::appendUnchanged(src + पूर्व, cpStart - पूर्व,
                                          sink, options, edits, error_संहिता);
            ByteSinkUtil::append_संहिता_Point(src_सूचकांक - cpStart, c + delta, sink, edits);
            पूर्व = src_सूचकांक;
        }
        यदि (c < 0) {
            भङ्ग;
        }
        // slow path
        csc->cpStart = cpStart;
        csc->cp_सीमा = src_सूचकांक;
        ध्रुव char16_t *s;
        c = ucase_गन्तव्यम्_पूर्णम्_ऊर्ध्वस्थम्(c, utf8_caseContextIterator, csc, &s, caseLocale);
        यदि (c >= 0) {
            ByteSinkUtil::appendUnchanged(src + पूर्व, cpStart - पूर्व,
                                          sink, options, edits, error_संहिता);
            append_परिणाम(src_सूचकांक - cpStart, c, s, sink, options, edits, error_संहिता);
            पूर्व = src_सूचकांक;
        }
    }
    ByteSinkUtil::appendUnchanged(src + पूर्व, src_सूचकांक - पूर्व,
                                  sink, options, edits, error_संहिता);
}

}  // namespace

#यदि !UCONFIG_NO_BREAK_ITERATION

नामाकाश {

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
int32_t maybeTitleDutchIJ(ध्रुव uint8_t *src, UChar32 c, int32_t start, int32_t segment_सीमा,
                          ByteSink &sink, uint32_t options, icu::Edits *edits, UError_संहिता &error_संहिता) {
    U_ASSERT(start < segment_सीमा);

    int32_t सूचकांक = start;
    तार्किक withAcute = असत्य;

    // If the conditions are met, then the following variables tell us what to output.
    int32_t unchanged1 = 0;  // code units before the j, or the whole sequence (0..3)
    तार्किक doTitleJ = असत्य;  // true if the j needs to be titlecased
    int32_t unchanged2 = 0;  // after the j (0 or 1)

    // next character after the first letter
    UChar32 c2;
    c2 = src[सूचकांक++];

    // Is the first letter an i/I with accent?
    यदि (c == u'I') {
        यदि (c2 == ACUTE_BYTE0 && सूचकांक < segment_सीमा && src[सूचकांक++] == ACUTE_BYTE1) {
            withAcute = सत्य;
            unchanged1 = 2;  // ACUTE is 2 code units in UTF-8
            यदि (सूचकांक == segment_सीमा) { प्रत्यागम start; }
            c2 = src[सूचकांक++];
        }
    } अन्यथा {  // Í
        withAcute = सत्य;
    }

    // Is the next character a j/J?
    यदि (c2 == u'j') {
        doTitleJ = सत्य;
    } अन्यथा यदि (c2 == u'J') {
        ++unchanged1;
    } अन्यथा {
        प्रत्यागम start;
    }

    // A plain i/I must be followed by a plain j/J.
    // An i/I with acute must be followed by a j/J with acute.
    यदि (withAcute) {
        यदि ((सूचकांक + 1) >= segment_सीमा || src[सूचकांक++] != ACUTE_BYTE0 || src[सूचकांक++] != ACUTE_BYTE1) {
            प्रत्यागम start;
        }
        यदि (doTitleJ) {
            unchanged2 = 2;  // ACUTE is 2 code units in UTF-8
        } अन्यथा {
            unchanged1 = unchanged1 + 2;    // ACUTE is 2 code units in UTF-8
        }
    }

    // There must not be another combining mark.
    यदि (सूचकांक < segment_सीमा) {
        int32_t cp;
        int32_t i = सूचकांक;
        U8_अग्र(src, i, segment_सीमा, cp);
        uint32_t type_आवरणम् = U_GET_GC_आवरणम्(cp);
        यदि ((type_आवरणम् & U_GC_M_आवरणम्) != 0) {
            प्रत्यागम start;
        }
    }

    // Output the rest of the Dutch IJ.
    ByteSinkUtil::appendUnchanged(src + start, unchanged1, sink, options, edits, error_संहिता);
    start += unchanged1;
    यदि (doTitleJ) {
        ByteSinkUtil::append_संहिता_Point(1, u'J', sink, edits);
        ++start;
    }
    ByteSinkUtil::appendUnchanged(src + start, unchanged2, sink, options, edits, error_संहिता);

    U_ASSERT(start + unchanged2 == सूचकांक);
    प्रत्यागम सूचकांक;
}

}  // namespace

U_CFUNC शून्य U_CALLCONV
ucasemap_आन्तरिकम्_UTF8_गन्तव्यम्_Title(
        int32_t caseLocale, uint32_t options, BreakIterator *iter,
        ध्रुव uint8_t *src, int32_t src_दीर्घता,
        ByteSink &sink, icu::Edits *edits,
        UError_संहिता &error_संहिता) {
    यदि (!ustrcase_checkTitleAdjustmentOptions(options, error_संहिता)) {
        प्रत्यागम;
    }

    /* set up local variables */
    UCaseContext csc=UCASECONTEXT_INITIALIZER;
    csc.p=(शून्य *)src;
    csc.सीमा=src_दीर्घता;
    int32_t पूर्व=0;
    UBool is_First_सूचकांक=सत्य;

    /* titlecasing loop */
    यावत्(पूर्व<src_दीर्घता) {
        /* find next index where to titlecase */
        int32_t सूचकांक;
        यदि(is_First_सूचकांक) {
            is_First_सूचकांक=असत्य;
            सूचकांक=iter->first();
        } अन्यथा {
            सूचकांक=iter->next();
        }
        यदि(सूचकांक==UBRK_समाप्तम् || सूचकांक>src_दीर्घता) {
            सूचकांक=src_दीर्घता;
        }

        /*
         * Segment [prev..index[ into 3 parts:
         * a) skipped characters (copy as-is) [prev..titleStart[
         * b) first letter (titlecase)              [titleStart..titleLimit[
         * c) subsequent characters (lowercase)                 [titleLimit..index[
         */
        यदि(पूर्व<सूचकांक) {
            /* find and copy skipped characters [prev..titleStart[ */
            int32_t titleStart=पूर्व;
            int32_t title_सीमा=पूर्व;
            UChar32 c;
            U8_अग्र(src, title_सीमा, सूचकांक, c);
            यदि ((options&U_TITLECASE_NO_BREAK_ADJUSTMENT)==0) {
                // Adjust the titlecasing index to the next cased character,
                // or to the next letter/number/symbol/private use.
                // Stop with titleStart<titleLimit<=index
                // if there is a character to be titlecased,
                // or else stop with titleStart==titleLimit==index.
                UBool गन्तव्यम्_Cased = (options&U_TITLECASE_ADJUST_गन्तव्यम्_CASED) != 0;
                यावत् (गन्तव्यम्_Cased ? UCASE_NONE==ucase_getType(c) : !ustrcase_isLNS(c)) {
                    titleStart=title_सीमा;
                    यदि(title_सीमा==सूचकांक) {
                        भङ्ग;
                    }
                    U8_अग्र(src, title_सीमा, सूचकांक, c);
                }
                यदि (पूर्व < titleStart) {
                    यदि (!ByteSinkUtil::appendUnchanged(src+पूर्व, titleStart-पूर्व,
                                                       sink, options, edits, error_संहिता)) {
                        प्रत्यागम;
                    }
                }
            }

            यदि(titleStart<title_सीमा) {
                /* titlecase c which is from [titleStart..titleLimit[ */
                यदि(c>=0) {
                    csc.cpStart=titleStart;
                    csc.cp_सीमा=title_सीमा;
                    ध्रुव char16_t *s;
                    c=ucase_गन्तव्यम्_पूर्णम्_Title(c, utf8_caseContextIterator, &csc, &s, caseLocale);
                    यदि (!append_परिणाम(title_सीमा-titleStart, c, s, sink, options, edits, error_संहिता)) {
                        प्रत्यागम;
                    }
                } अन्यथा {
                    // Malformed UTF-8.
                    यदि (!ByteSinkUtil::appendUnchanged(src+titleStart, title_सीमा-titleStart,
                                                       sink, options, edits, error_संहिता)) {
                        प्रत्यागम;
                    }
                }

                /* Special case Dutch IJ titlecasing */
                यदि (title_सीमा < सूचकांक &&
                    caseLocale == UCASE_LOC_DUTCH) {
                    यदि (c < 0) {
                        c = ~c;
                    }

                    यदि (c == u'I' || c == u'Í') {
                        title_सीमा = maybeTitleDutchIJ(src, c, title_सीमा, सूचकांक, sink, options, edits, error_संहिता);
                    }
                }

                /* lowercase [titleLimit..index[ */
                यदि(title_सीमा<सूचकांक) {
                    यदि((options&U_TITLECASE_NO_LOWERCASE)==0) {
                        /* Normal operation: Lowercase the rest of the word. */
                        गन्तव्यम्_अधस्तनम्(caseLocale, options,
                                src, &csc, title_सीमा, सूचकांक,
                                sink, edits, error_संहिता);
                        यदि(U_FAILURE(error_संहिता)) {
                            प्रत्यागम;
                        }
                    } अन्यथा {
                        /* Optionally just copy the rest of the word unchanged. */
                        यदि (!ByteSinkUtil::appendUnchanged(src+title_सीमा, सूचकांक-title_सीमा,
                                                           sink, options, edits, error_संहिता)) {
                            प्रत्यागम;
                        }
                    }
                }
            }
        }

        पूर्व=सूचकांक;
    }
}

#यदि_अन्तः

U_NAMESPACE_BEGIN
नामाकाश Greek_ऊर्ध्वस्थम् {

UBool isFollowedByCasedLetter(ध्रुव uint8_t *s, int32_t i, int32_t length) {
    यावत् (i < length) {
        UChar32 c;
        U8_अग्र(s, i, length, c);
        int32_t type = ucase_getTypeOrIgnorable(c);
        यदि ((type & UCASE_IGNORABLE) != 0) {
            // Case-ignorable, continue with the loop.
        } अन्यथा यदि (type != UCASE_NONE) {
            प्रत्यागम सत्य;  // Followed by cased letter.
        } अन्यथा {
            प्रत्यागम असत्य;  // Uncased and not case-ignorable.
        }
    }
    प्रत्यागम असत्य;  // Not followed by cased letter.
}

// Keep this consistent with the UTF-16 version in ustrcase.cpp and the Java version in CaseMap.java.
शून्य गन्तव्यम्_ऊर्ध्वस्थम्(uint32_t options,
             ध्रुव uint8_t *src, int32_t src_दीर्घता,
             ByteSink &sink, Edits *edits,
             UError_संहिता &error_संहिता) {
    uint32_t state = 0;
    प्रति (int32_t i = 0; i < src_दीर्घता;) {
        int32_t अग्र_सूचकांक = i;
        UChar32 c;
        U8_अग्र(src, अग्र_सूचकांक, src_दीर्घता, c);
        uint32_t अग्र_State = 0;
        int32_t type = ucase_getTypeOrIgnorable(c);
        यदि ((type & UCASE_IGNORABLE) != 0) {
            // c is case-ignorable
            अग्र_State |= (state & AFTER_CASED);
        } अन्यथा यदि (type != UCASE_NONE) {
            // c is cased
            अग्र_State |= AFTER_CASED;
        }
        uint32_t दत्त = get_Letter_दत्त(c);
        यदि (दत्त > 0) {
            uint32_t upper = दत्त & ऊर्ध्वस्थम्_आवरणम्;
            // Add a dialytika to this iota or ypsilon vowel
            // if we removed a tonos from the previous vowel,
            // and that previous vowel did not also have (or gain) a dialytika.
            // Adding one only to the final vowel in a longer sequence
            // (which does not occur in normal writing) would require lookahead.
            // Set the same flag as for preserving an existing dialytika.
            यदि ((दत्त & HAS_VOWEL) != 0 &&
                (state & (AFTER_VOWEL_WITH_PRECOMPOSED_ACCENT | AFTER_VOWEL_WITH_COMBINING_ACCENT)) !=
                    0 &&
                (upper == 0x399 || upper == 0x3A5)) {
                दत्त |= (state & AFTER_VOWEL_WITH_PRECOMPOSED_ACCENT) != 0 ? HAS_DIALYTIKA
                                                                           : HAS_COMBINING_DIALYTIKA;
            }
            int32_t अङ्कः_Ypogegrammeni = 0;  // Map each one to a trailing, spacing, capital iota.
            यदि ((दत्त & HAS_YPOGEGRAMMENI) != 0) {
                अङ्कः_Ypogegrammeni = 1;
            }
            ध्रुव UBool hasPrecomposedAccent = (दत्त & HAS_ACCENT) != 0;
            // Skip combining diacritics after this Greek letter.
            int32_t अग्र_अग्र_सूचकांक = अग्र_सूचकांक;
            यावत् (अग्र_सूचकांक < src_दीर्घता) {
                UChar32 c2;
                U8_अग्र(src, अग्र_अग्र_सूचकांक, src_दीर्घता, c2);
                uint32_t diacritic_दत्त = get_Diacritic_दत्त(c2);
                यदि (diacritic_दत्त != 0) {
                    दत्त |= diacritic_दत्त;
                    यदि ((diacritic_दत्त & HAS_YPOGEGRAMMENI) != 0) {
                        ++अङ्कः_Ypogegrammeni;
                    }
                    अग्र_सूचकांक = अग्र_अग्र_सूचकांक;
                } अन्यथा {
                    भङ्ग;  // not a Greek diacritic
                }
            }
            यदि ((दत्त & HAS_VOWEL_AND_ACCENT_AND_DIALYTIKA) == HAS_VOWEL_AND_ACCENT) {
                अग्र_State |= hasPrecomposedAccent ? AFTER_VOWEL_WITH_PRECOMPOSED_ACCENT
                                                  : AFTER_VOWEL_WITH_COMBINING_ACCENT;
            }
            // Map according to Greek rules.
            UBool सङ्कलय_Tonos = असत्य;
            यदि (upper == 0x397 &&
                    (दत्त & HAS_ACCENT) != 0 &&
                    अङ्कः_Ypogegrammeni == 0 &&
                    (state & AFTER_CASED) == 0 &&
                    !isFollowedByCasedLetter(src, अग्र_सूचकांक, src_दीर्घता)) {
                // Keep disjunctive "or" with (only) a tonos.
                // We use the same "word boundary" conditions as for the Final_Sigma test.
                यदि (hasPrecomposedAccent) {
                    upper = 0x389;  // Preserve the precomposed form.
                } अन्यथा {
                    सङ्कलय_Tonos = सत्य;
                }
            } अन्यथा यदि ((दत्त & HAS_DIALYTIKA) != 0) {
                // Preserve a vowel with dialytika in precomposed form if it exists.
                यदि (upper == 0x399) {
                    upper = 0x3AA;
                    दत्त &= ~HAS_EITHER_DIALYTIKA;
                } अन्यथा यदि (upper == 0x3A5) {
                    upper = 0x3AB;
                    दत्त &= ~HAS_EITHER_DIALYTIKA;
                }
            }

            UBool change;
            यदि (edits == शून्यसूचकः && (options & U_OMIT_UNCHANGED_TEXT) == 0) {
                change = सत्य;  // common, simple usage
            } अन्यथा {
                // Find out first whether we are changing the text.
                U_ASSERT(0x370 <= upper && upper <= 0x3ff);  // 2-byte UTF-8, main Greek block
                change = (i + 2) > अग्र_सूचकांक ||
                        src[i] != getTwoByteLead(upper) || src[i + 1] != getTwoByteTrail(upper) ||
                        अङ्कः_Ypogegrammeni > 0;
                int32_t i2 = i + 2;
                यदि ((दत्त & HAS_EITHER_DIALYTIKA) != 0) {
                    change |= (i2 + 2) > अग्र_सूचकांक ||
                            src[i2] != static_cast<uint8_t>(u8"\u0308"[0]) ||
                            src[i2 + 1] != static_cast<uint8_t>(u8"\u0308"[1]);
                    i2 += 2;
                }
                यदि (सङ्कलय_Tonos) {
                    change |= (i2 + 2) > अग्र_सूचकांक ||
                            src[i2] != static_cast<uint8_t>(u8"\u0301"[0]) ||
                            src[i2 + 1] != static_cast<uint8_t>(u8"\u0301"[1]);
                    i2 += 2;
                }
                int32_t old_दीर्घता = अग्र_सूचकांक - i;
                int32_t new_दीर्घता = (i2 - i) + अङ्कः_Ypogegrammeni * 2;  // 2 bytes per U+0399
                change |= old_दीर्घता != new_दीर्घता;
                यदि (change) {
                    यदि (edits != शून्यसूचकः) {
                        edits->सङ्कलय_Replace(old_दीर्घता, new_दीर्घता);
                    }
                } अन्यथा {
                    यदि (edits != शून्यसूचकः) {
                        edits->सङ्कलय_Unchanged(old_दीर्घता);
                    }
                    // Write unchanged text?
                    change = (options & U_OMIT_UNCHANGED_TEXT) == 0;
                }
            }

            यदि (change) {
                ByteSinkUtil::append_Two_अष्टकानि(upper, sink);
                यदि ((दत्त & HAS_EITHER_DIALYTIKA) != 0) {
                    sink.AppendU8(u8"\u0308", 2);  // restore or add a dialytika
                }
                यदि (सङ्कलय_Tonos) {
                    sink.AppendU8(u8"\u0301", 2);
                }
                यावत् (अङ्कः_Ypogegrammeni > 0) {
                    sink.AppendU8(u8"\u0399", 2);
                    --अङ्कः_Ypogegrammeni;
                }
            }
        } अन्यथा यदि(c>=0) {
            ध्रुव char16_t *s;
            c=ucase_गन्तव्यम्_पूर्णम्_ऊर्ध्वस्थम्(c, शून्यसूचकः, शून्यसूचकः, &s, UCASE_LOC_GREEK);
            यदि (!append_परिणाम(अग्र_सूचकांक - i, c, s, sink, options, edits, error_संहिता)) {
                प्रत्यागम;
            }
        } अन्यथा {
            // Malformed UTF-8.
            यदि (!ByteSinkUtil::appendUnchanged(src+i, अग्र_सूचकांक-i,
                                               sink, options, edits, error_संहिता)) {
                प्रत्यागम;
            }
        }
        i = अग्र_सूचकांक;
        state = अग्र_State;
    }
}

}  // namespace GreekUpper
U_NAMESPACE_अन्तः

स्थिर शून्य U_CALLCONV
ucasemap_आन्तरिकम्_UTF8_गन्तव्यम्_अधस्तनम्(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_अप्रयुक्तम्
                             ध्रुव uint8_t *src, int32_t src_दीर्घता,
                             icu::ByteSink &sink, icu::Edits *edits,
                             UError_संहिता &error_संहिता) {
    UCaseContext csc=UCASECONTEXT_INITIALIZER;
    csc.p=(शून्य *)src;
    csc.सीमा=src_दीर्घता;
    गन्तव्यम्_अधस्तनम्(
        caseLocale, options,
        src, &csc, 0, src_दीर्घता,
        sink, edits, error_संहिता);
}

स्थिर शून्य U_CALLCONV
ucasemap_आन्तरिकम्_UTF8_गन्तव्यम्_ऊर्ध्वस्थम्(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_अप्रयुक्तम्
                             ध्रुव uint8_t *src, int32_t src_दीर्घता,
                             icu::ByteSink &sink, icu::Edits *edits,
                             UError_संहिता &error_संहिता) {
    यदि (caseLocale == UCASE_LOC_GREEK) {
        Greek_ऊर्ध्वस्थम्::गन्तव्यम्_ऊर्ध्वस्थम्(options, src, src_दीर्घता, sink, edits, error_संहिता);
    } अन्यथा {
        UCaseContext csc=UCASECONTEXT_INITIALIZER;
        csc.p=(शून्य *)src;
        csc.सीमा=src_दीर्घता;
        गन्तव्यम्_ऊर्ध्वस्थम्(
            caseLocale, options,
            src, &csc, src_दीर्घता,
            sink, edits, error_संहिता);
    }
}

स्थिर शून्य U_CALLCONV
ucasemap_आन्तरिकम्_UTF8_Fold(int32_t /* caseLocale */, uint32_t options, UCASEMAP_BREAK_ITERATOR_अप्रयुक्तम्
                          ध्रुव uint8_t *src, int32_t src_दीर्घता,
                          icu::ByteSink &sink, icu::Edits *edits,
                          UError_संहिता &error_संहिता) {
    गन्तव्यम्_अधस्तनम्(
        -1, options,
        src, शून्यसूचकः, 0, src_दीर्घता,
        sink, edits, error_संहिता);
}

शून्य
ucasemap_mapUTF8(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_PARAM
                 ध्रुव वर्ण *src, int32_t src_दीर्घता,
                 UTF8CaseMapper *stringCaseMapper,
                 icu::ByteSink &sink, icu::Edits *edits,
                 UError_संहिता &error_संहिता) {
    /* check argument values */
    यदि (U_FAILURE(error_संहिता)) {
        प्रत्यागम;
    }
    यदि ((src == शून्यसूचकः && src_दीर्घता != 0) || src_दीर्घता < -1) {
        error_संहिता = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम;
    }

    // Get the string length.
    यदि (src_दीर्घता == -1) {
        src_दीर्घता = static_cast<int32_t>(uprv_strlen(src));
    }

    यदि (edits != शून्यसूचकः && (options & U_EDITS_NO_RESET) == 0) {
        edits->reset();
    }
    stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                     reinterpret_cast<ध्रुव uint8_t*>(src), src_दीर्घता, sink, edits, error_संहिता);
    sink.Flush();
    यदि (U_SUCCESS(error_संहिता)) {
        यदि (edits != शून्यसूचकः) {
            edits->copy_Error_गन्तव्यम्(error_संहिता);
        }
    }
}

int32_t
ucasemap_mapUTF8(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_PARAM
                 वर्ण *dest, int32_t dest_क्षमता,
                 ध्रुव वर्ण *src, int32_t src_दीर्घता,
                 UTF8CaseMapper *stringCaseMapper,
                 icu::Edits *edits,
                 UError_संहिता &error_संहिता) {
    /* check argument values */
    यदि(U_FAILURE(error_संहिता)) {
        प्रत्यागम 0;
    }
    यदि( dest_क्षमता<0 ||
        (dest==शून्यसूचकः && dest_क्षमता>0) ||
        (src==शून्यसूचकः && src_दीर्घता!=0) || src_दीर्घता<-1
    ) {
        error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम 0;
    }

    /* get the string length */
    यदि(src_दीर्घता==-1) {
        src_दीर्घता = static_cast<int32_t>(uprv_strlen(src));
    }

    /* check for overlapping source and destination */
    यदि( dest!=शून्यसूचकः &&
        ((src>=dest && src<(dest+dest_क्षमता)) ||
         (dest>=src && dest<(src+src_दीर्घता)))
    ) {
        error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम 0;
    }

    यदि (edits != शून्यसूचकः && (options & U_EDITS_NO_RESET) == 0) {
        edits->reset();
    }
    int32_t reslen = ByteSinkUtil::द्वारा_Byte_Sink_गन्तव्यम्_Terminated_Chars(
        dest, dest_क्षमता,
        [&](ByteSink& sink, UError_संहिता& स्थितिः) {
            stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                             reinterpret_cast<ध्रुव uint8_t*>(src), src_दीर्घता, sink, edits, स्थितिः);
        },
        error_संहिता);
    यदि (U_SUCCESS(error_संहिता) && edits != शून्यसूचकः) {
        edits->copy_Error_गन्तव्यम्(error_संहिता);
    }
    प्रत्यागम reslen;
}

/* public API functions */

U_CAPI int32_t U_EXPORT2
ucasemap_utf8_गन्तव्यम्_अधस्तनम्(ध्रुव UCaseMap *csm,
                     वर्ण *dest, int32_t dest_क्षमता,
                     ध्रुव वर्ण *src, int32_t src_दीर्घता,
                     UError_संहिता *p_Error_संहिता) {
    स्वचालित *impl = reinterpret_cast<ध्रुव UCaseMapImpl *>(csm);
    प्रत्यागम ucasemap_mapUTF8(
        impl->caseLocale, impl->options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_क्षमता,
        src, src_दीर्घता,
        ucasemap_आन्तरिकम्_UTF8_गन्तव्यम्_अधस्तनम्, शून्यसूचकः, *p_Error_संहिता);
}

U_CAPI int32_t U_EXPORT2
ucasemap_utf8_गन्तव्यम्_ऊर्ध्वस्थम्(ध्रुव UCaseMap *csm,
                     वर्ण *dest, int32_t dest_क्षमता,
                     ध्रुव वर्ण *src, int32_t src_दीर्घता,
                     UError_संहिता *p_Error_संहिता) {
    स्वचालित *impl = reinterpret_cast<ध्रुव UCaseMapImpl *>(csm);
    प्रत्यागम ucasemap_mapUTF8(
        impl->caseLocale, impl->options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_क्षमता,
        src, src_दीर्घता,
        ucasemap_आन्तरिकम्_UTF8_गन्तव्यम्_ऊर्ध्वस्थम्, शून्यसूचकः, *p_Error_संहिता);
}

U_CAPI int32_t U_EXPORT2
ucasemap_utf8FoldCase(ध्रुव UCaseMap *csm,
                      वर्ण *dest, int32_t dest_क्षमता,
                      ध्रुव वर्ण *src, int32_t src_दीर्घता,
                      UError_संहिता *p_Error_संहिता) {
    स्वचालित *impl = reinterpret_cast<ध्रुव UCaseMapImpl *>(csm);
    प्रत्यागम ucasemap_mapUTF8(
        UCASE_LOC_मूलम्, impl->options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_क्षमता,
        src, src_दीर्घता,
        ucasemap_आन्तरिकम्_UTF8_Fold, शून्यसूचकः, *p_Error_संहिता);
}

U_NAMESPACE_BEGIN

शून्य CaseMap::utf8_गन्तव्यम्_अधस्तनम्(
        ध्रुव वर्ण *locale, uint32_t options,
        StringPiece src, ByteSink &sink, Edits *edits,
        UError_संहिता &error_संहिता) {
    ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        src.दत्त(), src.length(),
        ucasemap_आन्तरिकम्_UTF8_गन्तव्यम्_अधस्तनम्, sink, edits, error_संहिता);
}

शून्य CaseMap::utf8_गन्तव्यम्_ऊर्ध्वस्थम्(
        ध्रुव वर्ण *locale, uint32_t options,
        StringPiece src, ByteSink &sink, Edits *edits,
        UError_संहिता &error_संहिता) {
    ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        src.दत्त(), src.length(),
        ucasemap_आन्तरिकम्_UTF8_गन्तव्यम्_ऊर्ध्वस्थम्, sink, edits, error_संहिता);
}

शून्य CaseMap::utf8Fold(
        uint32_t options,
        StringPiece src, ByteSink &sink, Edits *edits,
        UError_संहिता &error_संहिता) {
    ucasemap_mapUTF8(
        UCASE_LOC_मूलम्, options, UCASEMAP_BREAK_ITERATOR_NULL
        src.दत्त(), src.length(),
        ucasemap_आन्तरिकम्_UTF8_Fold, sink, edits, error_संहिता);
}

int32_t CaseMap::utf8_गन्तव्यम्_अधस्तनम्(
        ध्रुव वर्ण *locale, uint32_t options,
        ध्रुव वर्ण *src, int32_t src_दीर्घता,
        वर्ण *dest, int32_t dest_क्षमता, Edits *edits,
        UError_संहिता &error_संहिता) {
    प्रत्यागम ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_क्षमता,
        src, src_दीर्घता,
        ucasemap_आन्तरिकम्_UTF8_गन्तव्यम्_अधस्तनम्, edits, error_संहिता);
}

int32_t CaseMap::utf8_गन्तव्यम्_ऊर्ध्वस्थम्(
        ध्रुव वर्ण *locale, uint32_t options,
        ध्रुव वर्ण *src, int32_t src_दीर्घता,
        वर्ण *dest, int32_t dest_क्षमता, Edits *edits,
        UError_संहिता &error_संहिता) {
    प्रत्यागम ucasemap_mapUTF8(
        ustrcase_getCaseLocale(locale), options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_क्षमता,
        src, src_दीर्घता,
        ucasemap_आन्तरिकम्_UTF8_गन्तव्यम्_ऊर्ध्वस्थम्, edits, error_संहिता);
}

int32_t CaseMap::utf8Fold(
        uint32_t options,
        ध्रुव वर्ण *src, int32_t src_दीर्घता,
        वर्ण *dest, int32_t dest_क्षमता, Edits *edits,
        UError_संहिता &error_संहिता) {
    प्रत्यागम ucasemap_mapUTF8(
        UCASE_LOC_मूलम्, options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_क्षमता,
        src, src_दीर्घता,
        ucasemap_आन्तरिकम्_UTF8_Fold, edits, error_संहिता);
}

U_NAMESPACE_अन्तः
