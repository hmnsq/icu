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

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/brkiter.h"
#अन्तर्भाव "unicode/casemap.h"
#अन्तर्भाव "unicode/edits.h"
#अन्तर्भाव "unicode/stringoptions.h"
#अन्तर्भाव "unicode/ustring.h"
#अन्तर्भाव "unicode/ucasemap.h"
#अन्तर्भाव "unicode/ubrk.h"
#अन्तर्भाव "unicode/utf.h"
#अन्तर्भाव "unicode/utf16.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "ucase.h"
#अन्तर्भाव "ucasemap_imp.h"
#अन्तर्भाव "ustr_imp.h"
#अन्तर्भाव "uassert.h"

/**
 * Code point for COMBINING ACUTE ACCENT
 * @internal
 */
#परिभाषय ACUTE u'\u0301'

U_NAMESPACE_BEGIN

नामाकाश {

int32_t check_अतिप्रवाहः_And_Edits_Error(int32_t dest_सूचकांक, int32_t dest_क्षमता,
                                   Edits *edits, UError_संहिता &error_संहिता) {
    यदि (U_SUCCESS(error_संहिता)) {
        यदि (dest_सूचकांक > dest_क्षमता) {
            error_संहिता = U_बफर_अतिप्रवाहः_ERROR;
        } अन्यथा यदि (edits != शून्यसूचकः) {
            edits->copy_Error_गन्तव्यम्(error_संहिता);
        }
    }
    प्रत्यागम dest_सूचकांक;
}

/* Appends a full case mapping result, see UCASE_MAX_STRING_LENGTH. */
अन्तःस्थ int32_t
append_परिणाम(char16_t *dest, int32_t dest_सूचकांक, int32_t dest_क्षमता,
             int32_t परिणाम, ध्रुव char16_t *s,
             int32_t cp_दीर्घता, uint32_t options, icu::Edits *edits) {
    UChar32 c;
    int32_t length;

    /* decode the result */
    यदि(परिणाम<0) {
        /* (not) original code point */
        यदि(edits!=शून्यसूचकः) {
            edits->सङ्कलय_Unchanged(cp_दीर्घता);
        }
        यदि(options & U_OMIT_UNCHANGED_TEXT) {
            प्रत्यागम dest_सूचकांक;
        }
        c=~परिणाम;
        यदि(dest_सूचकांक<dest_क्षमता && c<=0xffff) {  // BMP slightly-fastpath
            dest[dest_सूचकांक++] = static_cast<char16_t>(c);
            प्रत्यागम dest_सूचकांक;
        }
        length=cp_दीर्घता;
    } अन्यथा {
        यदि(परिणाम<=UCASE_MAX_STRING_दीर्घता) {
            c=U_SENTINEL;
            length=परिणाम;
        } अन्यथा यदि(dest_सूचकांक<dest_क्षमता && परिणाम<=0xffff) {  // BMP slightly-fastpath
            dest[dest_सूचकांक++] = static_cast<char16_t>(परिणाम);
            यदि(edits!=शून्यसूचकः) {
                edits->सङ्कलय_Replace(cp_दीर्घता, 1);
            }
            प्रत्यागम dest_सूचकांक;
        } अन्यथा {
            c=परिणाम;
            length=U16_दीर्घता(c);
        }
        यदि(edits!=शून्यसूचकः) {
            edits->सङ्कलय_Replace(cp_दीर्घता, length);
        }
    }
    यदि(length>(INT32_MAX-dest_सूचकांक)) {
        प्रत्यागम -1;  // integer overflow
    }

    यदि(dest_सूचकांक<dest_क्षमता) {
        /* append the result */
        यदि(c>=0) {
            /* code point */
            UBool isError=असत्य;
            U16_APPEND(dest, dest_सूचकांक, dest_क्षमता, c, isError);
            यदि(isError) {
                /* overflow, nothing written */
                dest_सूचकांक+=length;
            }
        } अन्यथा {
            /* string */
            यदि((dest_सूचकांक+length)<=dest_क्षमता) {
                यावत्(length>0) {
                    dest[dest_सूचकांक++]=*s++;
                    --length;
                }
            } अन्यथा {
                /*अतिप्रवाहः*/
                dest_सूचकांक+=length;
            }
        }
    } अन्यथा {
        /* preflight */
        dest_सूचकांक+=length;
    }
    प्रत्यागम dest_सूचकांक;
}

अन्तःस्थ int32_t
appendUChar(char16_t *dest, int32_t dest_सूचकांक, int32_t dest_क्षमता, char16_t c) {
    यदि(dest_सूचकांक<dest_क्षमता) {
        dest[dest_सूचकांक]=c;
    } अन्यथा यदि(dest_सूचकांक==INT32_MAX) {
        प्रत्यागम -1;  // integer overflow
    }
    प्रत्यागम dest_सूचकांक+1;
}

int32_t
append_Non_रिक्तानि_Unchanged(char16_t *dest, int32_t dest_सूचकांक, int32_t dest_क्षमता,
                        ध्रुव char16_t *s, int32_t length, uint32_t options, icu::Edits *edits) {
    यदि(edits!=शून्यसूचकः) {
        edits->सङ्कलय_Unchanged(length);
    }
    यदि(options & U_OMIT_UNCHANGED_TEXT) {
        प्रत्यागम dest_सूचकांक;
    }
    यदि(length>(INT32_MAX-dest_सूचकांक)) {
        प्रत्यागम -1;  // integer overflow
    }
    यदि((dest_सूचकांक+length)<=dest_क्षमता) {
        u_memcpy(dest+dest_सूचकांक, s, length);
    }
    प्रत्यागम dest_सूचकांक + length;
}

अन्तःस्थ int32_t
appendUnchanged(char16_t *dest, int32_t dest_सूचकांक, int32_t dest_क्षमता,
                ध्रुव char16_t *s, int32_t length, uint32_t options, icu::Edits *edits) {
    यदि (length <= 0) {
        प्रत्यागम dest_सूचकांक;
    }
    प्रत्यागम append_Non_रिक्तानि_Unchanged(dest, dest_सूचकांक, dest_क्षमता, s, length, options, edits);
}

UChar32 U_CALLCONV
utf16_caseContextIterator(शून्य *context, int8_t dir) {
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
            U16_पूर्व((ध्रुव char16_t *)csc->p, csc->start, csc->सूचकांक, c);
            प्रत्यागम c;
        }
    } अन्यथा {
        यदि(csc->सूचकांक<csc->सीमा) {
            U16_अग्र((ध्रुव char16_t *)csc->p, csc->सूचकांक, csc->सीमा, c);
            प्रत्यागम c;
        }
    }
    प्रत्यागम U_SENTINEL;
}

/**
 * caseLocale >= 0: Lowercases [srcStart..srcLimit[ but takes context [0..srcLength[ into account.
 * caseLocale < 0: Case-folds [srcStart..srcLimit[.
 */
int32_t गन्तव्यम्_अधस्तनम्(int32_t caseLocale, uint32_t options,
                char16_t *dest, int32_t dest_क्षमता,
                ध्रुव char16_t *src, UCaseContext *csc, int32_t srcStart, int32_t src_सीमा,
                icu::Edits *edits, UError_संहिता &error_संहिता) {
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
    int32_t dest_सूचकांक = 0;
    int32_t पूर्व = srcStart;
    int32_t src_सूचकांक = srcStart;
    प्रति (;;) {
        // fast path for simple cases
        char16_t lead = 0;
        यावत् (src_सूचकांक < src_सीमा) {
            lead = src[src_सूचकांक];
            int32_t delta;
            यदि (lead < LatinCase::LONG_S) {
                int8_t d = latin_गन्तव्यम्_अधस्तनम्[lead];
                यदि (d == LatinCase::EXC) { भङ्ग; }
                ++src_सूचकांक;
                यदि (d == 0) { अनुवर्त; }
                delta = d;
            } अन्यथा यदि (lead >= 0xd800) {
                भङ्ग;  // surrogate or higher
            } अन्यथा {
                uint16_t props = UTRIE2_GET16_FROM_U16_SINGLE_LEAD(trie, lead);
                यदि (UCASE_HAS_EXCEPTION(props)) { भङ्ग; }
                ++src_सूचकांक;
                यदि (!UCASE_IS_ऊर्ध्वस्थम्_OR_TITLE(props) || (delta = UCASE_GET_DELTA(props)) == 0) {
                    अनुवर्त;
                }
            }
            lead += static_cast<char16_t>(delta);
            dest_सूचकांक = appendUnchanged(dest, dest_सूचकांक, dest_क्षमता,
                                        src + पूर्व, src_सूचकांक - 1 - पूर्व, options, edits);
            यदि (dest_सूचकांक >= 0) {
                dest_सूचकांक = appendUChar(dest, dest_सूचकांक, dest_क्षमता, lead);
                यदि (edits != शून्यसूचकः) {
                    edits->सङ्कलय_Replace(1, 1);
                }
            }
            यदि (dest_सूचकांक < 0) {
                error_संहिता = U_सूचकांक_OUTOFBOUNDS_ERROR;
                प्रत्यागम 0;
            }
            पूर्व = src_सूचकांक;
        }
        यदि (src_सूचकांक >= src_सीमा) {
            भङ्ग;
        }
        // slow path
        int32_t cpStart = src_सूचकांक++;
        char16_t trail;
        UChar32 c;
        यदि (U16_IS_LEAD(lead) && src_सूचकांक < src_सीमा && U16_IS_TRAIL(trail = src[src_सूचकांक])) {
            c = U16_GET_SUPPLEMENTARY(lead, trail);
            ++src_सूचकांक;
        } अन्यथा {
            c = lead;
        }
        ध्रुव char16_t *s = शून्यसूचकः;
        यदि (caseLocale >= 0) {
            csc->cpStart = cpStart;
            csc->cp_सीमा = src_सूचकांक;
            c = ucase_गन्तव्यम्_पूर्णम्_अधस्तनम्(c, utf16_caseContextIterator, csc, &s, caseLocale);
        } अन्यथा {
            c = ucase_गन्तव्यम्_पूर्णम्_Folding(c, &s, options);
        }
        यदि (c >= 0) {
            dest_सूचकांक = appendUnchanged(dest, dest_सूचकांक, dest_क्षमता,
                                        src + पूर्व, cpStart - पूर्व, options, edits);
            यदि (dest_सूचकांक >= 0) {
                dest_सूचकांक = append_परिणाम(dest, dest_सूचकांक, dest_क्षमता, c, s,
                                         src_सूचकांक - cpStart, options, edits);
            }
            यदि (dest_सूचकांक < 0) {
                error_संहिता = U_सूचकांक_OUTOFBOUNDS_ERROR;
                प्रत्यागम 0;
            }
            पूर्व = src_सूचकांक;
        }
    }
    dest_सूचकांक = appendUnchanged(dest, dest_सूचकांक, dest_क्षमता,
                                src + पूर्व, src_सूचकांक - पूर्व, options, edits);
    यदि (dest_सूचकांक < 0) {
        error_संहिता = U_सूचकांक_OUTOFBOUNDS_ERROR;
        प्रत्यागम 0;
    }
    प्रत्यागम dest_सूचकांक;
}

int32_t गन्तव्यम्_ऊर्ध्वस्थम्(int32_t caseLocale, uint32_t options,
                char16_t *dest, int32_t dest_क्षमता,
                ध्रुव char16_t *src, UCaseContext *csc, int32_t src_दीर्घता,
                icu::Edits *edits, UError_संहिता &error_संहिता) {
    ध्रुव int8_t *latin_गन्तव्यम्_ऊर्ध्वस्थम्;
    यदि (caseLocale == UCASE_LOC_TURKISH) {
        latin_गन्तव्यम्_ऊर्ध्वस्थम् = LatinCase::गन्तव्यम्_ऊर्ध्वस्थम्_TR;
    } अन्यथा {
        latin_गन्तव्यम्_ऊर्ध्वस्थम् = LatinCase::गन्तव्यम्_ऊर्ध्वस्थम्_प्रसामान्यम्;
    }
    ध्रुव UTrie2 *trie = ucase_getTrie();
    int32_t dest_सूचकांक = 0;
    int32_t पूर्व = 0;
    int32_t src_सूचकांक = 0;
    प्रति (;;) {
        // fast path for simple cases
        char16_t lead = 0;
        यावत् (src_सूचकांक < src_दीर्घता) {
            lead = src[src_सूचकांक];
            int32_t delta;
            यदि (lead < LatinCase::LONG_S) {
                int8_t d = latin_गन्तव्यम्_ऊर्ध्वस्थम्[lead];
                यदि (d == LatinCase::EXC) { भङ्ग; }
                ++src_सूचकांक;
                यदि (d == 0) { अनुवर्त; }
                delta = d;
            } अन्यथा यदि (lead >= 0xd800) {
                भङ्ग;  // surrogate or higher
            } अन्यथा {
                uint16_t props = UTRIE2_GET16_FROM_U16_SINGLE_LEAD(trie, lead);
                यदि (UCASE_HAS_EXCEPTION(props)) { भङ्ग; }
                ++src_सूचकांक;
                यदि (UCASE_GET_TYPE(props) != UCASE_अधस्तनम् || (delta = UCASE_GET_DELTA(props)) == 0) {
                    अनुवर्त;
                }
            }
            lead += static_cast<char16_t>(delta);
            dest_सूचकांक = appendUnchanged(dest, dest_सूचकांक, dest_क्षमता,
                                        src + पूर्व, src_सूचकांक - 1 - पूर्व, options, edits);
            यदि (dest_सूचकांक >= 0) {
                dest_सूचकांक = appendUChar(dest, dest_सूचकांक, dest_क्षमता, lead);
                यदि (edits != शून्यसूचकः) {
                    edits->सङ्कलय_Replace(1, 1);
                }
            }
            यदि (dest_सूचकांक < 0) {
                error_संहिता = U_सूचकांक_OUTOFBOUNDS_ERROR;
                प्रत्यागम 0;
            }
            पूर्व = src_सूचकांक;
        }
        यदि (src_सूचकांक >= src_दीर्घता) {
            भङ्ग;
        }
        // slow path
        int32_t cpStart;
        csc->cpStart = cpStart = src_सूचकांक++;
        char16_t trail;
        UChar32 c;
        यदि (U16_IS_LEAD(lead) && src_सूचकांक < src_दीर्घता && U16_IS_TRAIL(trail = src[src_सूचकांक])) {
            c = U16_GET_SUPPLEMENTARY(lead, trail);
            ++src_सूचकांक;
        } अन्यथा {
            c = lead;
        }
        csc->cp_सीमा = src_सूचकांक;
        ध्रुव char16_t *s = शून्यसूचकः;
        c = ucase_गन्तव्यम्_पूर्णम्_ऊर्ध्वस्थम्(c, utf16_caseContextIterator, csc, &s, caseLocale);
        यदि (c >= 0) {
            dest_सूचकांक = appendUnchanged(dest, dest_सूचकांक, dest_क्षमता,
                                        src + पूर्व, cpStart - पूर्व, options, edits);
            यदि (dest_सूचकांक >= 0) {
                dest_सूचकांक = append_परिणाम(dest, dest_सूचकांक, dest_क्षमता, c, s,
                                         src_सूचकांक - cpStart, options, edits);
            }
            यदि (dest_सूचकांक < 0) {
                error_संहिता = U_सूचकांक_OUTOFBOUNDS_ERROR;
                प्रत्यागम 0;
            }
            पूर्व = src_सूचकांक;
        }
    }
    dest_सूचकांक = appendUnchanged(dest, dest_सूचकांक, dest_क्षमता,
                                src + पूर्व, src_सूचकांक - पूर्व, options, edits);
    यदि (dest_सूचकांक < 0) {
        error_संहिता = U_सूचकांक_OUTOFBOUNDS_ERROR;
        प्रत्यागम 0;
    }
    प्रत्यागम dest_सूचकांक;
}

}  // namespace

U_NAMESPACE_अन्तः

U_NAMESPACE_USE

#यदि !UCONFIG_NO_BREAK_ITERATION

नामाकाश {

/**
 * Input: c is a letter I with or without acute accent.
 * start is the index in src after c, and is less than segmentLimit.
 * If a plain i/I is followed by a plain j/J,
 * or an i/I with acute (precomposed or decomposed) is followed by a j/J with acute,
 * then we output accordingly.
 *
 * @return the src index after the titlecased sequence, or the start index if no Dutch IJ
 */
int32_t maybeTitleDutchIJ(ध्रुव char16_t *src, UChar32 c, int32_t start, int32_t segment_सीमा,
                          char16_t *dest, int32_t &dest_सूचकांक, int32_t dest_क्षमता, uint32_t options,
                          icu::Edits *edits) {
    U_ASSERT(start < segment_सीमा);

    int32_t सूचकांक = start;
    तार्किक withAcute = असत्य;

    // If the conditions are met, then the following variables tell us what to output.
    int32_t unchanged1 = 0;  // code units before the j, or the whole sequence (0..3)
    तार्किक doTitleJ = असत्य;  // true if the j needs to be titlecased
    int32_t unchanged2 = 0;  // after the j (0 or 1)

    // next character after the first letter
    char16_t c2 = src[सूचकांक++];

    // Is the first letter an i/I with accent?
    यदि (c == u'I') {
        यदि (c2 == ACUTE) {
            withAcute = सत्य;
            unchanged1 = 1;
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
        यदि (सूचकांक == segment_सीमा || src[सूचकांक++] != ACUTE) { प्रत्यागम start; }
        यदि (doTitleJ) {
            unchanged2 = 1;
        } अन्यथा {
            ++unchanged1;
        }
    }

    // There must not be another combining mark.
    यदि (सूचकांक < segment_सीमा) {
        int32_t cp;
        int32_t i = सूचकांक;
        U16_अग्र(src, i, segment_सीमा, cp);
        uint32_t type_आवरणम् = U_GET_GC_आवरणम्(cp);
        यदि ((type_आवरणम् & U_GC_M_आवरणम्) != 0) {
            प्रत्यागम start;
        }
    }

    // Output the rest of the Dutch IJ.
    dest_सूचकांक = appendUnchanged(dest, dest_सूचकांक, dest_क्षमता, src + start, unchanged1, options, edits);
    start += unchanged1;
    यदि (doTitleJ) {
        dest_सूचकांक = appendUChar(dest, dest_सूचकांक, dest_क्षमता, u'J');
        यदि (edits != शून्यसूचकः) {
            edits->सङ्कलय_Replace(1, 1);
        }
        ++start;
    }
    dest_सूचकांक = appendUnchanged(dest, dest_सूचकांक, dest_क्षमता, src + start, unchanged2, options, edits);

    U_ASSERT(start + unchanged2 == सूचकांक);
    प्रत्यागम सूचकांक;
}

}  // namespace

U_CFUNC int32_t U_CALLCONV
ustrcase_आन्तरिकम्_गन्तव्यम्_Title(int32_t caseLocale, uint32_t options, BreakIterator *iter,
                         char16_t *dest, int32_t dest_क्षमता,
                         ध्रुव char16_t *src, int32_t src_दीर्घता,
                         icu::Edits *edits,
                         UError_संहिता &error_संहिता) {
    यदि (!ustrcase_checkTitleAdjustmentOptions(options, error_संहिता)) {
        प्रत्यागम 0;
    }

    /* set up local variables */
    UCaseContext csc=UCASECONTEXT_INITIALIZER;
    csc.p=(शून्य *)src;
    csc.सीमा=src_दीर्घता;
    int32_t dest_सूचकांक=0;
    int32_t पूर्व=0;
    तार्किक is_First_सूचकांक=सत्य;

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
            // Find and copy skipped characters [prev..titleStart[
            int32_t titleStart=पूर्व;
            int32_t title_सीमा=पूर्व;
            UChar32 c;
            U16_अग्र(src, title_सीमा, सूचकांक, c);
            यदि ((options&U_TITLECASE_NO_BREAK_ADJUSTMENT)==0) {
                // Adjust the titlecasing index to the next cased character,
                // or to the next letter/number/symbol/private use.
                // Stop with titleStart<titleLimit<=index
                // if there is a character to be titlecased,
                // or else stop with titleStart==titleLimit==index.
                तार्किक गन्तव्यम्_Cased = (options&U_TITLECASE_ADJUST_गन्तव्यम्_CASED) != 0;
                यावत् (गन्तव्यम्_Cased ? UCASE_NONE==ucase_getType(c) : !ustrcase_isLNS(c)) {
                    titleStart=title_सीमा;
                    यदि(title_सीमा==सूचकांक) {
                        भङ्ग;
                    }
                    U16_अग्र(src, title_सीमा, सूचकांक, c);
                }
                यदि (पूर्व < titleStart) {
                    dest_सूचकांक=appendUnchanged(dest, dest_सूचकांक, dest_क्षमता,
                                              src+पूर्व, titleStart-पूर्व, options, edits);
                    यदि(dest_सूचकांक<0) {
                        error_संहिता=U_सूचकांक_OUTOFBOUNDS_ERROR;
                        प्रत्यागम 0;
                    }
                }
            }

            यदि(titleStart<title_सीमा) {
                /* titlecase c which is from [titleStart..titleLimit[ */
                csc.cpStart=titleStart;
                csc.cp_सीमा=title_सीमा;
                ध्रुव char16_t *s;
                c=ucase_गन्तव्यम्_पूर्णम्_Title(c, utf16_caseContextIterator, &csc, &s, caseLocale);
                dest_सूचकांक=append_परिणाम(dest, dest_सूचकांक, dest_क्षमता, c, s,
                                       title_सीमा-titleStart, options, edits);
                यदि(dest_सूचकांक<0) {
                    error_संहिता=U_सूचकांक_OUTOFBOUNDS_ERROR;
                    प्रत्यागम 0;
                }

                /* Special case Dutch IJ titlecasing */
                यदि (titleStart+1 < सूचकांक &&
                        caseLocale == UCASE_LOC_DUTCH) {
                    यदि (c < 0) {
                        c = ~c;
                    }

                    यदि (c == u'I' || c == u'Í') {
                        title_सीमा = maybeTitleDutchIJ(src, c, titleStart + 1, सूचकांक,
                                                       dest, dest_सूचकांक, dest_क्षमता, options,
                                                       edits);
                    }
                }

                /* lowercase [titleLimit..index[ */
                यदि(title_सीमा<सूचकांक) {
                    यदि((options&U_TITLECASE_NO_LOWERCASE)==0) {
                        /* Normal operation: Lowercase the rest of the word. */
                        dest_सूचकांक+=
                            गन्तव्यम्_अधस्तनम्(
                                caseLocale, options,
                                (dest==शून्यसूचकः) ? शून्यसूचकः: dest+dest_सूचकांक, dest_क्षमता-dest_सूचकांक,
                                src, &csc, title_सीमा, सूचकांक,
                                edits, error_संहिता);
                        यदि(error_संहिता==U_बफर_अतिप्रवाहः_ERROR) {
                            error_संहिता=U_ZERO_ERROR;
                        }
                        यदि(U_FAILURE(error_संहिता)) {
                            प्रत्यागम dest_सूचकांक;
                        }
                    } अन्यथा {
                        /* Optionally just copy the rest of the word unchanged. */
                        dest_सूचकांक=appendUnchanged(dest, dest_सूचकांक, dest_क्षमता,
                                                  src+title_सीमा, सूचकांक-title_सीमा, options, edits);
                        यदि(dest_सूचकांक<0) {
                            error_संहिता=U_सूचकांक_OUTOFBOUNDS_ERROR;
                            प्रत्यागम 0;
                        }
                    }
                }
            }
        }

        पूर्व=सूचकांक;
    }

    प्रत्यागम check_अतिप्रवाहः_And_Edits_Error(dest_सूचकांक, dest_क्षमता, edits, error_संहिता);
}

#यदि_अन्तः  // !UCONFIG_NO_BREAK_ITERATION

U_NAMESPACE_BEGIN
नामाकाश Greek_ऊर्ध्वस्थम् {

// Data generated by prototype code, see
// https://icu.unicode.org/design/case/greek-upper
// TODO: Move this data into ucase.icu.
स्थिर ध्रुव uint16_t data0370[] = {
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

स्थिर ध्रुव uint16_t data1F00[] = {
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
स्थिर ध्रुव uint16_t data2126 = 0x03A9 | HAS_VOWEL;

uint32_t get_Letter_दत्त(UChar32 c) {
    यदि (c < 0x370 || 0x2126 < c || (0x3ff < c && c < 0x1f00)) {
        प्रत्यागम 0;
    } अन्यथा यदि (c <= 0x3ff) {
        प्रत्यागम data0370[c - 0x370];
    } अन्यथा यदि (c <= 0x1fff) {
        प्रत्यागम data1F00[c - 0x1f00];
    } अन्यथा यदि (c == 0x2126) {
        प्रत्यागम data2126;
    } अन्यथा {
        प्रत्यागम 0;
    }
}

uint32_t get_Diacritic_दत्त(UChar32 c) {
    विकल्प (c) {
    पक्ष 0x0300:  // varia
    पक्ष 0x0301:  // tonos = oxia
    पक्ष 0x0342:  // perispomeni
    पक्ष 0x0302:  // circumflex can look like perispomeni
    पक्ष 0x0303:  // tilde can look like perispomeni
    पक्ष 0x0311:  // inverted breve can look like perispomeni
        प्रत्यागम HAS_ACCENT;
    पक्ष 0x0308:  // dialytika = diaeresis
        प्रत्यागम HAS_COMBINING_DIALYTIKA;
    पक्ष 0x0344:  // dialytika tonos
        प्रत्यागम HAS_COMBINING_DIALYTIKA | HAS_ACCENT;
    पक्ष 0x0345:  // ypogegrammeni = iota subscript
        प्रत्यागम HAS_YPOGEGRAMMENI;
    पक्ष 0x0304:  // macron
    पक्ष 0x0306:  // breve
    पक्ष 0x0313:  // comma above
    पक्ष 0x0314:  // reversed comma above
    पक्ष 0x0343:  // koronis
        प्रत्यागम HAS_OTHER_GREEK_DIACRITIC;
    स्वभाव:
        प्रत्यागम 0;
    }
}

UBool isFollowedByCasedLetter(ध्रुव char16_t *s, int32_t i, int32_t length) {
    यावत् (i < length) {
        UChar32 c;
        U16_अग्र(s, i, length, c);
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

/**
 * Greek string uppercasing with a state machine.
 * Probably simpler than a stateless function that has to figure out complex context-before
 * for each character.
 * TODO: Try to re-consolidate one way or another with the non-Greek function.
 */
int32_t गन्तव्यम्_ऊर्ध्वस्थम्(uint32_t options,
                char16_t *dest, int32_t dest_क्षमता,
                ध्रुव char16_t *src, int32_t src_दीर्घता,
                Edits *edits,
                UError_संहिता &error_संहिता) {
    int32_t dest_सूचकांक=0;
    uint32_t state = 0;
    प्रति (int32_t i = 0; i < src_दीर्घता;) {
        int32_t अग्र_सूचकांक = i;
        UChar32 c;
        U16_अग्र(src, अग्र_सूचकांक, src_दीर्घता, c);
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
                दत्त |= (state & AFTER_VOWEL_WITH_PRECOMPOSED_ACCENT) ? HAS_DIALYTIKA
                                                                      : HAS_COMBINING_DIALYTIKA;
            }
            int32_t अङ्कः_Ypogegrammeni = 0;  // Map each one to a trailing, spacing, capital iota.
            यदि ((दत्त & HAS_YPOGEGRAMMENI) != 0) {
                अङ्कः_Ypogegrammeni = 1;
            }
            ध्रुव UBool hasPrecomposedAccent = (दत्त & HAS_ACCENT) != 0;
            // Skip combining diacritics after this Greek letter.
            यावत् (अग्र_सूचकांक < src_दीर्घता) {
                uint32_t diacritic_दत्त = get_Diacritic_दत्त(src[अग्र_सूचकांक]);
                यदि (diacritic_दत्त != 0) {
                    दत्त |= diacritic_दत्त;
                    यदि ((diacritic_दत्त & HAS_YPOGEGRAMMENI) != 0) {
                        ++अङ्कः_Ypogegrammeni;
                    }
                    ++अग्र_सूचकांक;
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
                change = src[i] != upper || अङ्कः_Ypogegrammeni > 0;
                int32_t i2 = i + 1;
                यदि ((दत्त & HAS_EITHER_DIALYTIKA) != 0) {
                    change |= i2 >= अग्र_सूचकांक || src[i2] != 0x308;
                    ++i2;
                }
                यदि (सङ्कलय_Tonos) {
                    change |= i2 >= अग्र_सूचकांक || src[i2] != 0x301;
                    ++i2;
                }
                int32_t old_दीर्घता = अग्र_सूचकांक - i;
                int32_t new_दीर्घता = (i2 - i) + अङ्कः_Ypogegrammeni;
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
                dest_सूचकांक = appendUChar(dest, dest_सूचकांक, dest_क्षमता, static_cast<char16_t>(upper));
                यदि (dest_सूचकांक >= 0 && (दत्त & HAS_EITHER_DIALYTIKA) != 0) {
                    dest_सूचकांक=appendUChar(dest, dest_सूचकांक, dest_क्षमता, 0x308);  // restore or add a dialytika
                }
                यदि (dest_सूचकांक >= 0 && सङ्कलय_Tonos) {
                    dest_सूचकांक=appendUChar(dest, dest_सूचकांक, dest_क्षमता, 0x301);
                }
                यावत् (dest_सूचकांक >= 0 && अङ्कः_Ypogegrammeni > 0) {
                    dest_सूचकांक=appendUChar(dest, dest_सूचकांक, dest_क्षमता, 0x399);
                    --अङ्कः_Ypogegrammeni;
                }
                यदि(dest_सूचकांक<0) {
                    error_संहिता=U_सूचकांक_OUTOFBOUNDS_ERROR;
                    प्रत्यागम 0;
                }
            }
        } अन्यथा {
            ध्रुव char16_t *s;
            c=ucase_गन्तव्यम्_पूर्णम्_ऊर्ध्वस्थम्(c, शून्यसूचकः, शून्यसूचकः, &s, UCASE_LOC_GREEK);
            dest_सूचकांक = append_परिणाम(dest, dest_सूचकांक, dest_क्षमता, c, s,
                                     अग्र_सूचकांक - i, options, edits);
            यदि (dest_सूचकांक < 0) {
                error_संहिता = U_सूचकांक_OUTOFBOUNDS_ERROR;
                प्रत्यागम 0;
            }
        }
        i = अग्र_सूचकांक;
        state = अग्र_State;
    }

    प्रत्यागम dest_सूचकांक;
}

}  // namespace GreekUpper
U_NAMESPACE_अन्तः

/* functions available in the common library (for unistr_case.cpp) */

U_CFUNC int32_t U_CALLCONV
ustrcase_आन्तरिकम्_गन्तव्यम्_अधस्तनम्(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_अप्रयुक्तम्
                         char16_t *dest, int32_t dest_क्षमता,
                         ध्रुव char16_t *src, int32_t src_दीर्घता,
                         icu::Edits *edits,
                         UError_संहिता &error_संहिता) {
    UCaseContext csc=UCASECONTEXT_INITIALIZER;
    csc.p=(शून्य *)src;
    csc.सीमा=src_दीर्घता;
    int32_t dest_सूचकांक = गन्तव्यम्_अधस्तनम्(
        caseLocale, options,
        dest, dest_क्षमता,
        src, &csc, 0, src_दीर्घता,
        edits, error_संहिता);
    प्रत्यागम check_अतिप्रवाहः_And_Edits_Error(dest_सूचकांक, dest_क्षमता, edits, error_संहिता);
}

U_CFUNC int32_t U_CALLCONV
ustrcase_आन्तरिकम्_गन्तव्यम्_ऊर्ध्वस्थम्(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_अप्रयुक्तम्
                         char16_t *dest, int32_t dest_क्षमता,
                         ध्रुव char16_t *src, int32_t src_दीर्घता,
                         icu::Edits *edits,
                         UError_संहिता &error_संहिता) {
    int32_t dest_सूचकांक;
    यदि (caseLocale == UCASE_LOC_GREEK) {
        dest_सूचकांक = Greek_ऊर्ध्वस्थम्::गन्तव्यम्_ऊर्ध्वस्थम्(options, dest, dest_क्षमता,
                                        src, src_दीर्घता, edits, error_संहिता);
    } अन्यथा {
        UCaseContext csc=UCASECONTEXT_INITIALIZER;
        csc.p=(शून्य *)src;
        csc.सीमा=src_दीर्घता;
        dest_सूचकांक = गन्तव्यम्_ऊर्ध्वस्थम्(
            caseLocale, options,
            dest, dest_क्षमता,
            src, &csc, src_दीर्घता,
            edits, error_संहिता);
    }
    प्रत्यागम check_अतिप्रवाहः_And_Edits_Error(dest_सूचकांक, dest_क्षमता, edits, error_संहिता);
}

U_CFUNC int32_t U_CALLCONV
ustrcase_आन्तरिकम्_Fold(int32_t /* caseLocale */, uint32_t options, UCASEMAP_BREAK_ITERATOR_अप्रयुक्तम्
                      char16_t *dest, int32_t dest_क्षमता,
                      ध्रुव char16_t *src, int32_t src_दीर्घता,
                      icu::Edits *edits,
                      UError_संहिता &error_संहिता) {
    int32_t dest_सूचकांक = गन्तव्यम्_अधस्तनम्(
        -1, options,
        dest, dest_क्षमता,
        src, शून्यसूचकः, 0, src_दीर्घता,
        edits, error_संहिता);
    प्रत्यागम check_अतिप्रवाहः_And_Edits_Error(dest_सूचकांक, dest_क्षमता, edits, error_संहिता);
}

U_CFUNC int32_t
ustrcase_map(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_PARAM
             char16_t *dest, int32_t dest_क्षमता,
             ध्रुव char16_t *src, int32_t src_दीर्घता,
             UStringCaseMapper *stringCaseMapper,
             icu::Edits *edits,
             UError_संहिता &error_संहिता) {
    int32_t dest_दीर्घता;

    /* check argument values */
    यदि(U_FAILURE(error_संहिता)) {
        प्रत्यागम 0;
    }
    यदि( dest_क्षमता<0 ||
        (dest==शून्यसूचकः && dest_क्षमता>0) ||
        src==शून्यसूचकः ||
        src_दीर्घता<-1
    ) {
        error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम 0;
    }

    /* get the string length */
    यदि(src_दीर्घता==-1) {
        src_दीर्घता=u_strlen(src);
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
    dest_दीर्घता=stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                                dest, dest_क्षमता, src, src_दीर्घता, edits, error_संहिता);
    प्रत्यागम u_terminateUChars(dest, dest_क्षमता, dest_दीर्घता, &error_संहिता);
}

U_CFUNC int32_t
ustrcase_mapWithOverlap(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_PARAM
                        char16_t *dest, int32_t dest_क्षमता,
                        ध्रुव char16_t *src, int32_t src_दीर्घता,
                        UStringCaseMapper *stringCaseMapper,
                        UError_संहिता &error_संहिता) {
    char16_t बफर[300];
    char16_t *अस्थायी;

    int32_t dest_दीर्घता;

    /* check argument values */
    यदि(U_FAILURE(error_संहिता)) {
        प्रत्यागम 0;
    }
    यदि( dest_क्षमता<0 ||
        (dest==शून्यसूचकः && dest_क्षमता>0) ||
        src==शून्यसूचकः ||
        src_दीर्घता<-1
    ) {
        error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम 0;
    }

    /* get the string length */
    यदि(src_दीर्घता==-1) {
        src_दीर्घता=u_strlen(src);
    }

    /* check for overlapping source and destination */
    यदि( dest!=शून्यसूचकः &&
        ((src>=dest && src<(dest+dest_क्षमता)) ||
         (dest>=src && dest<(src+src_दीर्घता)))
    ) {
        /* overlap: provide a temporary destination buffer and later copy the result */
        यदि(dest_क्षमता<=UPRV_LENGTHOF(बफर)) {
            /* the stack buffer is large enough */
            अस्थायी=बफर;
        } अन्यथा {
            /* allocate a buffer */
            अस्थायी=(char16_t *)uprv_malloc(dest_क्षमता*U_SIZEOF_UCHAR);
            यदि(अस्थायी==शून्यसूचकः) {
                error_संहिता=U_स्मृतिः_आवण्टनम्_ERROR;
                प्रत्यागम 0;
            }
        }
    } अन्यथा {
        अस्थायी=dest;
    }

    dest_दीर्घता=stringCaseMapper(caseLocale, options, UCASEMAP_BREAK_ITERATOR
                                अस्थायी, dest_क्षमता, src, src_दीर्घता, शून्यसूचकः, error_संहिता);
    यदि(अस्थायी!=dest) {
        /* copy the result string to the destination buffer */
        यदि (U_SUCCESS(error_संहिता) && 0 < dest_दीर्घता && dest_दीर्घता <= dest_क्षमता) {
            u_memmove(dest, अस्थायी, dest_दीर्घता);
        }
        यदि(अस्थायी!=बफर) {
            uprv_free(अस्थायी);
        }
    }

    प्रत्यागम u_terminateUChars(dest, dest_क्षमता, dest_दीर्घता, &error_संहिता);
}

/* public API functions */

U_CAPI int32_t U_EXPORT2
u_strFoldCase(char16_t *dest, int32_t dest_क्षमता,
              ध्रुव char16_t *src, int32_t src_दीर्घता,
              uint32_t options,
              UError_संहिता *p_Error_संहिता) {
    प्रत्यागम ustrcase_mapWithOverlap(
        UCASE_LOC_मूलम्, options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_क्षमता,
        src, src_दीर्घता,
        ustrcase_आन्तरिकम्_Fold, *p_Error_संहिता);
}

U_NAMESPACE_BEGIN

int32_t CaseMap::fold(
        uint32_t options,
        ध्रुव char16_t *src, int32_t src_दीर्घता,
        char16_t *dest, int32_t dest_क्षमता, Edits *edits,
        UError_संहिता &error_संहिता) {
    प्रत्यागम ustrcase_map(
        UCASE_LOC_मूलम्, options, UCASEMAP_BREAK_ITERATOR_NULL
        dest, dest_क्षमता,
        src, src_दीर्घता,
        ustrcase_आन्तरिकम्_Fold, edits, error_संहिता);
}

U_NAMESPACE_अन्तः

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
संरचना CmpEquivLevel {
    ध्रुव char16_t *start, *s, *सीमा;
};
प्रकारनिर्देश संरचना CmpEquivLevel CmpEquivLevel;

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
स्थिर int32_t _cmpFold(
            ध्रुव char16_t *s1, int32_t length1,
            ध्रुव char16_t *s2, int32_t length2,
            uint32_t options,
            int32_t *मेलनम्_Len1, int32_t *मेलनम्_Len2,
            UError_संहिता *p_Error_संहिता) {
    int32_t cmpRes = 0;

    /* current-level start/limit - s1/s2 as current */
    ध्रुव char16_t *start1, *start2, *limit1, *limit2;

    /* points to the original start address */
    ध्रुव char16_t *org1, *org2;

    /* points to the end of match + 1 */
    ध्रुव char16_t *m1, *m2;

    /* case folding variables */
    ध्रुव char16_t *p;
    int32_t length;

    /* stacks of previous-level start/current/limit */
    CmpEquivLevel stack1[2], stack2[2];

    /* case folding buffers, only use current-level start/limit */
    char16_t fold1[UCASE_MAX_STRING_दीर्घता+1], fold2[UCASE_MAX_STRING_दीर्घता+1];

    /* track which is the current level per string */
    int32_t level1, level2;

    /* current code units, and code points for lookups */
    UChar32 c1, c2, cp1, cp2;

    /* no argument error checking because this itself is not an API */

    /*
     * assume that at least the option U_COMPARE_IGNORE_CASE is set
     * otherwise this function would have to behave exactly as uprv_strCompare()
     */
    यदि(U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम 0;
    }

    /* initialize */
    यदि(मेलनम्_Len1) {
        U_ASSERT(मेलनम्_Len2 !=शून्यसूचकः);
        *मेलनम्_Len1=0;
        *मेलनम्_Len2=0;
    }

    start1=m1=org1=s1;
    यदि(length1==-1) {
        limit1=शून्यसूचकः;
    } अन्यथा {
        limit1=s1+length1;
    }

    start2=m2=org2=s2;
    यदि(length2==-1) {
        limit2=शून्यसूचकः;
    } अन्यथा {
        limit2=s2+length2;
    }

    level1=level2=0;
    c1=c2=-1;

    /* comparison loop */
    प्रति(;;) {
        /*
         * here a code unit value of -1 means "get another code unit"
         * below it will mean "this source is finished"
         */

        यदि(c1<0) {
            /* get next code unit from string 1, post-increment */
            प्रति(;;) {
                यदि(s1==limit1 || ((c1=*s1)==0 && (limit1==शून्यसूचकः || (options&_STRNCMP_STYLE)))) {
                    यदि(level1==0) {
                        c1=-1;
                        भङ्ग;
                    }
                } अन्यथा {
                    ++s1;
                    भङ्ग;
                }

                /* reached end of level buffer, pop one level */
                कुरु {
                    --level1;
                    start1=stack1[level1].start;    /*Not uninitialized*/
                } यावत्(start1==शून्यसूचकः);
                s1=stack1[level1].s;                /*Not uninitialized*/
                limit1=stack1[level1].सीमा;        /*Not uninitialized*/
            }
        }

        यदि(c2<0) {
            /* get next code unit from string 2, post-increment */
            प्रति(;;) {
                यदि(s2==limit2 || ((c2=*s2)==0 && (limit2==शून्यसूचकः || (options&_STRNCMP_STYLE)))) {
                    यदि(level2==0) {
                        c2=-1;
                        भङ्ग;
                    }
                } अन्यथा {
                    ++s2;
                    भङ्ग;
                }

                /* reached end of level buffer, pop one level */
                कुरु {
                    --level2;
                    start2=stack2[level2].start;    /*Not uninitialized*/
                } यावत्(start2==शून्यसूचकः);
                s2=stack2[level2].s;                /*Not uninitialized*/
                limit2=stack2[level2].सीमा;        /*Not uninitialized*/
            }
        }

        /*
         * compare c1 and c2
         * either variable c1, c2 is -1 only if the corresponding string is finished
         */
        यदि(c1==c2) {
            ध्रुव char16_t *next1, *next2;

            यदि(c1<0) {
                cmpRes=0;   /* c1==c2==-1 indicating end of strings */
                भङ्ग;
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
            next1=next2=शून्यसूचकः;
            यदि(level1==0) {
                next1=s1;
            } अन्यथा यदि(s1==limit1) {
                /* Note: This implementation only use a single level of stack.
                 *      If this code needs to be changed to use multiple levels
                 *      of stacks, the code above should check if the current
                 *      code is at the end of all stacks.
                 */
                U_ASSERT(level1==1);

                /* is s1 at the end of the current stack? */
                next1=stack1[0].s;
            }

            यदि (next1!=शून्यसूचकः) {
                यदि(level2==0) {
                    next2=s2;
                } अन्यथा यदि(s2==limit2) {
                    U_ASSERT(level2==1);

                    /* is s2 at the end of the current stack? */
                    next2=stack2[0].s;
                }
                यदि(next2!=शून्यसूचकः) {
                    m1=next1;
                    m2=next2;
                }
            }
            c1=c2=-1;       /* make us fetch new code units */
            अनुवर्त;
        } अन्यथा यदि(c1<0) {
            cmpRes=-1;      /* string 1 ends before string 2 */
            भङ्ग;
        } अन्यथा यदि(c2<0) {
            cmpRes=1;       /* string 2 ends before string 1 */
            भङ्ग;
        }
        /* c1!=c2 && c1>=0 && c2>=0 */

        /* get complete code points for c1, c2 for lookups if either is a surrogate */
        cp1=c1;
        यदि(U_IS_SURROGATE(c1)) {
            char16_t c;

            यदि(U_IS_SURROGATE_LEAD(c1)) {
                यदि(s1!=limit1 && U16_IS_TRAIL(c=*s1)) {
                    /* advance ++s1; only below if cp1 decomposes/case-folds */
                    cp1=U16_GET_SUPPLEMENTARY(c1, c);
                }
            } अन्यथा /* isTrail(c1) */ {
                यदि(start1<=(s1-2) && U16_IS_LEAD(c=*(s1-2))) {
                    cp1=U16_GET_SUPPLEMENTARY(c, c1);
                }
            }
        }

        cp2=c2;
        यदि(U_IS_SURROGATE(c2)) {
            char16_t c;

            यदि(U_IS_SURROGATE_LEAD(c2)) {
                यदि(s2!=limit2 && U16_IS_TRAIL(c=*s2)) {
                    /* advance ++s2; only below if cp2 decomposes/case-folds */
                    cp2=U16_GET_SUPPLEMENTARY(c2, c);
                }
            } अन्यथा /* isTrail(c2) */ {
                यदि(start2<=(s2-2) && U16_IS_LEAD(c=*(s2-2))) {
                    cp2=U16_GET_SUPPLEMENTARY(c, c2);
                }
            }
        }

        /*
         * go down one level for each string
         * continue with the main loop as soon as there is a real change
         */

        यदि( level1==0 &&
            (length = ucase_गन्तव्यम्_पूर्णम्_Folding(cp1, &p, options)) >= 0
        ) {
            /* cp1 case-folds to the code point "length" or to p[length] */
            यदि(U_IS_SURROGATE(c1)) {
                यदि(U_IS_SURROGATE_LEAD(c1)) {
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
            stack1[0].सीमा=limit1;
            ++level1;

            /* copy the folding result to fold1[] */
            यदि(length<=UCASE_MAX_STRING_दीर्घता) {
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
            अनुवर्त;
        }

        यदि( level2==0 &&
            (length = ucase_गन्तव्यम्_पूर्णम्_Folding(cp2, &p, options)) >= 0
        ) {
            /* cp2 case-folds to the code point "length" or to p[length] */
            यदि(U_IS_SURROGATE(c2)) {
                यदि(U_IS_SURROGATE_LEAD(c2)) {
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
            stack2[0].सीमा=limit2;
            ++level2;

            /* copy the folding result to fold2[] */
            यदि(length<=UCASE_MAX_STRING_दीर्घता) {
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
            अनुवर्त;
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

        यदि(c1>=0xd800 && c2>=0xd800 && (options&U_COMPARE_संहिता_POINT_क्रमः)) {
            /* subtract 0x2800 from BMP code points to make them smaller than supplementary ones */
            यदि(
                (c1<=0xdbff && s1!=limit1 && U16_IS_TRAIL(*s1)) ||
                (U16_IS_TRAIL(c1) && start1!=(s1-1) && U16_IS_LEAD(*(s1-2)))
            ) {
                /* part of a surrogate pair, leave >=d800 */
            } अन्यथा {
                /* BMP code point - may be surrogate code point - make <d800 */
                c1-=0x2800;
            }

            यदि(
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
        भङ्ग;
    }

    यदि(मेलनम्_Len1) {
        *मेलनम्_Len1=static_cast<int32_t>(m1-org1);
        *मेलनम्_Len2=static_cast<int32_t>(m2-org2);
    }
    प्रत्यागम cmpRes;
}

/* internal function */
U_CFUNC int32_t
u_strcmpFold(ध्रुव char16_t *s1, int32_t length1,
             ध्रुव char16_t *s2, int32_t length2,
             uint32_t options,
             UError_संहिता *p_Error_संहिता) {
    प्रत्यागम _cmpFold(s1, length1, s2, length2, options, शून्यसूचकः, शून्यसूचकः, p_Error_संहिता);
}

/* public API functions */

U_CAPI int32_t U_EXPORT2
u_strCaseCompare(ध्रुव char16_t *s1, int32_t length1,
                 ध्रुव char16_t *s2, int32_t length2,
                 uint32_t options,
                 UError_संहिता *p_Error_संहिता) {
    /* argument checking */
    यदि (p_Error_संहिता == शून्यसूचकः || U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम 0;
    }
    यदि(s1==शून्यसूचकः || length1<-1 || s2==शून्यसूचकः || length2<-1) {
        *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम 0;
    }
    प्रत्यागम u_strcmpFold(s1, length1, s2, length2,
                        options|U_COMPARE_IGNORE_CASE,
                        p_Error_संहिता);
}

U_CAPI int32_t U_EXPORT2
u_strcasecmp(ध्रुव char16_t *s1, ध्रुव char16_t *s2, uint32_t options) {
    UError_संहिता error_संहिता=U_ZERO_ERROR;
    प्रत्यागम u_strcmpFold(s1, -1, s2, -1,
                        options|U_COMPARE_IGNORE_CASE,
                        &error_संहिता);
}

U_CAPI int32_t U_EXPORT2
u_memcasecmp(ध्रुव char16_t *s1, ध्रुव char16_t *s2, int32_t length, uint32_t options) {
    UError_संहिता error_संहिता=U_ZERO_ERROR;
    प्रत्यागम u_strcmpFold(s1, length, s2, length,
                        options|U_COMPARE_IGNORE_CASE,
                        &error_संहिता);
}

U_CAPI int32_t U_EXPORT2
u_strncasecmp(ध्रुव char16_t *s1, ध्रुव char16_t *s2, int32_t n, uint32_t options) {
    UError_संहिता error_संहिता=U_ZERO_ERROR;
    प्रत्यागम u_strcmpFold(s1, n, s2, n,
                        options|(U_COMPARE_IGNORE_CASE|_STRNCMP_STYLE),
                        &error_संहिता);
}

/* internal API - detect length of shared prefix */
U_CAPI शून्य
u_case_Insensitive_उपसर्गः_मेलनम्(ध्रुव char16_t *s1, int32_t length1,
                             ध्रुव char16_t *s2, int32_t length2,
                             uint32_t options,
                             int32_t *मेलनम्_Len1, int32_t *मेलनम्_Len2,
                             UError_संहिता *p_Error_संहिता) {
    _cmpFold(s1, length1, s2, length2, options,
        मेलनम्_Len1, मेलनम्_Len2, p_Error_संहिता);
}
