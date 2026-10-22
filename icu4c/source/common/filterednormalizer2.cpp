// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*
*   Copyright (C) 2009-2012, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  filterednormalizer2.cpp
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2009dec10
*   created by: Markus W. Scherer
*/

#अन्तर्भाव "unicode/utypes.h"

#यदि !UCONFIG_NO_NORMALIZATION

#अन्तर्भाव "unicode/edits.h"
#अन्तर्भाव "unicode/normalizer2.h"
#अन्तर्भाव "unicode/stringoptions.h"
#अन्तर्भाव "unicode/uniset.h"
#अन्तर्भाव "unicode/unistr.h"
#अन्तर्भाव "unicode/unorm.h"
#अन्तर्भाव "cpputils.h"

U_NAMESPACE_BEGIN

FilteredNormalizer2::~FilteredNormalizer2() {}

UnicodeString&
FilteredNormalizer2::normalize(ध्रुव UnicodeString& src,
                               UnicodeString& dest U_LIFETIME_BOUND,
                               UError_संहिता& error_संहिता) ध्रुव {
    uprv_check_Can_Get_बफर(src, error_संहिता);
    यदि(U_FAILURE(error_संहिता)) {
        dest.समुच्चयः_गन्तव्यम्_Bogus();
        प्रत्यागम dest;
    }
    यदि(&dest==&src) {
        error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम dest;
    }
    dest.remove();
    प्रत्यागम normalize(src, dest, USET_विस्तृतिः_SIMPLE, error_संहिता);
}

// Internal: No argument checking, and appends to dest.
// Pass as input spanCondition the one that is likely to yield a non-zero
// span length at the start of src.
// For set=[:age=3.2:], since almost all common characters were in Unicode 3.2,
// USET_SPAN_SIMPLE should be passed in for the start of src
// and USET_SPAN_NOT_CONTAINED should be passed in if we continue after
// an in-filter prefix.
UnicodeString&
FilteredNormalizer2::normalize(ध्रुव UnicodeString& src,
                               UnicodeString& dest U_LIFETIME_BOUND,
                               USet_विस्तृतिः_Condition विस्तृतिः_Condition,
                               UError_संहिता& error_संहिता) ध्रुव {
    UnicodeString अस्थायी_Dest;  // Don't throw away destination buffer between iterations.
    प्रति(int32_t पूर्व_विस्तृतिः_सीमा=0; पूर्व_विस्तृतिः_सीमा<src.length();) {
        int32_t विस्तृतिः_सीमा=समुच्चयः.विस्तृतिः(src, पूर्व_विस्तृतिः_सीमा, विस्तृतिः_Condition);
        int32_t विस्तृतिः_दीर्घता=विस्तृतिः_सीमा-पूर्व_विस्तृतिः_सीमा;
        यदि(विस्तृतिः_Condition==USET_विस्तृतिः_NOT_CONTAINED) {
            यदि(विस्तृतिः_दीर्घता!=0) {
                dest.append(src, पूर्व_विस्तृतिः_सीमा, विस्तृतिः_दीर्घता);
            }
            विस्तृतिः_Condition=USET_विस्तृतिः_SIMPLE;
        } अन्यथा {
            यदि(विस्तृतिः_दीर्घता!=0) {
                // Not norm2.normalizeSecondAndAppend() because we do not want
                // to modify the non-filter part of dest.
                dest.append(norm2.normalize(src.अस्थायी_Sub_String_Between(पूर्व_विस्तृतिः_सीमा, विस्तृतिः_सीमा),
                                            अस्थायी_Dest, error_संहिता));
                यदि(U_FAILURE(error_संहिता)) {
                    भङ्ग;
                }
            }
            विस्तृतिः_Condition=USET_विस्तृतिः_NOT_CONTAINED;
        }
        पूर्व_विस्तृतिः_सीमा=विस्तृतिः_सीमा;
    }
    प्रत्यागम dest;
}

शून्य
FilteredNormalizer2::normalizeUTF8(uint32_t options, StringPiece src, ByteSink &sink,
                                   Edits *edits, UError_संहिता &error_संहिता) ध्रुव {
    यदि (U_FAILURE(error_संहिता)) {
        प्रत्यागम;
    }
    यदि (edits != शून्यसूचकः && (options & U_EDITS_NO_RESET) == 0) {
        edits->reset();
    }
    options |= U_EDITS_NO_RESET;  // Do not reset for each span.
    normalizeUTF8(options, src.दत्त(), src.length(), sink, edits, USET_विस्तृतिः_SIMPLE, error_संहिता);
}

शून्य
FilteredNormalizer2::normalizeUTF8(uint32_t options, ध्रुव वर्ण *src, int32_t length,
                                   ByteSink &sink, Edits *edits,
                                   USet_विस्तृतिः_Condition विस्तृतिः_Condition,
                                   UError_संहिता &error_संहिता) ध्रुव {
    यावत् (length > 0) {
        int32_t विस्तृतिः_दीर्घता = समुच्चयः.विस्तृतिः_UTF8(src, length, विस्तृतिः_Condition);
        यदि (विस्तृतिः_Condition == USET_विस्तृतिः_NOT_CONTAINED) {
            यदि (विस्तृतिः_दीर्घता != 0) {
                यदि (edits != शून्यसूचकः) {
                    edits->सङ्कलय_Unchanged(विस्तृतिः_दीर्घता);
                }
                यदि ((options & U_OMIT_UNCHANGED_TEXT) == 0) {
                    sink.Append(src, विस्तृतिः_दीर्घता);
                }
            }
            विस्तृतिः_Condition = USET_विस्तृतिः_SIMPLE;
        } अन्यथा {
            यदि (विस्तृतिः_दीर्घता != 0) {
                // Not norm2.normalizeSecondAndAppend() because we do not want
                // to modify the non-filter part of dest.
                norm2.normalizeUTF8(options, StringPiece(src, विस्तृतिः_दीर्घता), sink, edits, error_संहिता);
                यदि (U_FAILURE(error_संहिता)) {
                    भङ्ग;
                }
            }
            विस्तृतिः_Condition = USET_विस्तृतिः_NOT_CONTAINED;
        }
        src += विस्तृतिः_दीर्घता;
        length -= विस्तृतिः_दीर्घता;
    }
}

UnicodeString&
FilteredNormalizer2::normalizeSecondAndAppend(UnicodeString& first U_LIFETIME_BOUND,
                                              ध्रुव UnicodeString& second,
                                              UError_संहिता& error_संहिता) ध्रुव {
    प्रत्यागम normalizeSecondAndAppend(first, second, सत्य, error_संहिता);
}

UnicodeString&
FilteredNormalizer2::append(UnicodeString& first U_LIFETIME_BOUND,
                            ध्रुव UnicodeString& second,
                            UError_संहिता& error_संहिता) ध्रुव {
    प्रत्यागम normalizeSecondAndAppend(first, second, असत्य, error_संहिता);
}

UnicodeString&
FilteredNormalizer2::normalizeSecondAndAppend(UnicodeString& first U_LIFETIME_BOUND,
                                              ध्रुव UnicodeString& second,
                                              UBool doNormalize,
                                              UError_संहिता& error_संहिता) ध्रुव {
    uprv_check_Can_Get_बफर(first, error_संहिता);
    uprv_check_Can_Get_बफर(second, error_संहिता);
    यदि(U_FAILURE(error_संहिता)) {
        प्रत्यागम first;
    }
    यदि(&first==&second) {
        error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम first;
    }
    यदि(first.is_रिक्तानि()) {
        यदि(doNormalize) {
            प्रत्यागम normalize(second, first, error_संहिता);
        } अन्यथा {
            प्रत्यागम first=second;
        }
    }
    // merge the in-filter suffix of the first string with the in-filter prefix of the second
    int32_t उपसर्गः_सीमा=समुच्चयः.विस्तृतिः(second, 0, USET_विस्तृतिः_SIMPLE);
    यदि(उपसर्गः_सीमा!=0) {
        UnicodeString उपसर्गः(second.अस्थायी_Sub_String(0, उपसर्गः_सीमा));
        int32_t suffixStart=समुच्चयः.विस्तृतिः_पृष्ठभागः(first, INT32_MAX, USET_विस्तृतिः_SIMPLE);
        यदि(suffixStart==0) {
            यदि(doNormalize) {
                norm2.normalizeSecondAndAppend(first, उपसर्गः, error_संहिता);
            } अन्यथा {
                norm2.append(first, उपसर्गः, error_संहिता);
            }
        } अन्यथा {
            UnicodeString मध्यः(first, suffixStart, INT32_MAX);
            यदि(doNormalize) {
                norm2.normalizeSecondAndAppend(मध्यः, उपसर्गः, error_संहिता);
            } अन्यथा {
                norm2.append(मध्यः, उपसर्गः, error_संहिता);
            }
            first.replace(suffixStart, INT32_MAX, मध्यः);
        }
    }
    यदि(उपसर्गः_सीमा<second.length()) {
        UnicodeString rest(second.अस्थायी_Sub_String(उपसर्गः_सीमा, INT32_MAX));
        यदि(doNormalize) {
            normalize(rest, first, USET_विस्तृतिः_NOT_CONTAINED, error_संहिता);
        } अन्यथा {
            first.append(rest);
        }
    }
    प्रत्यागम first;
}

UBool
FilteredNormalizer2::getDecomposition(UChar32 c, UnicodeString &decomposition) ध्रुव {
    प्रत्यागम समुच्चयः.अन्तर्भवति(c) && norm2.getDecomposition(c, decomposition);
}

UBool
FilteredNormalizer2::getRawDecomposition(UChar32 c, UnicodeString &decomposition) ध्रुव {
    प्रत्यागम समुच्चयः.अन्तर्भवति(c) && norm2.getRawDecomposition(c, decomposition);
}

UChar32
FilteredNormalizer2::composePair(UChar32 a, UChar32 b) ध्रुव {
    प्रत्यागम (समुच्चयः.अन्तर्भवति(a) && समुच्चयः.अन्तर्भवति(b)) ? norm2.composePair(a, b) : U_SENTINEL;
}

uint8_t
FilteredNormalizer2::getCombiningClass(UChar32 c) ध्रुव {
    प्रत्यागम समुच्चयः.अन्तर्भवति(c) ? norm2.getCombiningClass(c) : 0;
}

UBool
FilteredNormalizer2::isNormalized(ध्रुव UnicodeString &s, UError_संहिता &error_संहिता) ध्रुव {
    uprv_check_Can_Get_बफर(s, error_संहिता);
    यदि(U_FAILURE(error_संहिता)) {
        प्रत्यागम असत्य;
    }
    USet_विस्तृतिः_Condition विस्तृतिः_Condition=USET_विस्तृतिः_SIMPLE;
    प्रति(int32_t पूर्व_विस्तृतिः_सीमा=0; पूर्व_विस्तृतिः_सीमा<s.length();) {
        int32_t विस्तृतिः_सीमा=समुच्चयः.विस्तृतिः(s, पूर्व_विस्तृतिः_सीमा, विस्तृतिः_Condition);
        यदि(विस्तृतिः_Condition==USET_विस्तृतिः_NOT_CONTAINED) {
            विस्तृतिः_Condition=USET_विस्तृतिः_SIMPLE;
        } अन्यथा {
            यदि( !norm2.isNormalized(s.अस्थायी_Sub_String_Between(पूर्व_विस्तृतिः_सीमा, विस्तृतिः_सीमा), error_संहिता) ||
                U_FAILURE(error_संहिता)
            ) {
                प्रत्यागम असत्य;
            }
            विस्तृतिः_Condition=USET_विस्तृतिः_NOT_CONTAINED;
        }
        पूर्व_विस्तृतिः_सीमा=विस्तृतिः_सीमा;
    }
    प्रत्यागम सत्य;
}

UBool
FilteredNormalizer2::isNormalizedUTF8(StringPiece sp, UError_संहिता &error_संहिता) ध्रुव {
    यदि(U_FAILURE(error_संहिता)) {
        प्रत्यागम असत्य;
    }
    ध्रुव वर्ण *s = sp.दत्त();
    int32_t length = sp.length();
    USet_विस्तृतिः_Condition विस्तृतिः_Condition = USET_विस्तृतिः_SIMPLE;
    यावत् (length > 0) {
        int32_t विस्तृतिः_दीर्घता = समुच्चयः.विस्तृतिः_UTF8(s, length, विस्तृतिः_Condition);
        यदि (विस्तृतिः_Condition == USET_विस्तृतिः_NOT_CONTAINED) {
            विस्तृतिः_Condition = USET_विस्तृतिः_SIMPLE;
        } अन्यथा {
            यदि (!norm2.isNormalizedUTF8(StringPiece(s, विस्तृतिः_दीर्घता), error_संहिता) ||
                    U_FAILURE(error_संहिता)) {
                प्रत्यागम असत्य;
            }
            विस्तृतिः_Condition = USET_विस्तृतिः_NOT_CONTAINED;
        }
        s += विस्तृतिः_दीर्घता;
        length -= विस्तृतिः_दीर्घता;
    }
    प्रत्यागम सत्य;
}

UNormalization_Check_परिणाम
FilteredNormalizer2::शीघ्र_Check(ध्रुव UnicodeString &s, UError_संहिता &error_संहिता) ध्रुव {
    uprv_check_Can_Get_बफर(s, error_संहिता);
    यदि(U_FAILURE(error_संहिता)) {
        प्रत्यागम UNORM_MAYBE;
    }
    UNormalization_Check_परिणाम परिणाम=UNORM_YES;
    USet_विस्तृतिः_Condition विस्तृतिः_Condition=USET_विस्तृतिः_SIMPLE;
    प्रति(int32_t पूर्व_विस्तृतिः_सीमा=0; पूर्व_विस्तृतिः_सीमा<s.length();) {
        int32_t विस्तृतिः_सीमा=समुच्चयः.विस्तृतिः(s, पूर्व_विस्तृतिः_सीमा, विस्तृतिः_Condition);
        यदि(विस्तृतिः_Condition==USET_विस्तृतिः_NOT_CONTAINED) {
            विस्तृतिः_Condition=USET_विस्तृतिः_SIMPLE;
        } अन्यथा {
            UNormalization_Check_परिणाम qc_परिणाम=
                norm2.शीघ्र_Check(s.अस्थायी_Sub_String_Between(पूर्व_विस्तृतिः_सीमा, विस्तृतिः_सीमा), error_संहिता);
            यदि(U_FAILURE(error_संहिता) || qc_परिणाम==UNORM_NO) {
                प्रत्यागम qc_परिणाम;
            } अन्यथा यदि(qc_परिणाम==UNORM_MAYBE) {
                परिणाम=qc_परिणाम;
            }
            विस्तृतिः_Condition=USET_विस्तृतिः_NOT_CONTAINED;
        }
        पूर्व_विस्तृतिः_सीमा=विस्तृतिः_सीमा;
    }
    प्रत्यागम परिणाम;
}

int32_t
FilteredNormalizer2::विस्तृतिः_शीघ्र_Check_Yes(ध्रुव UnicodeString &s, UError_संहिता &error_संहिता) ध्रुव {
    uprv_check_Can_Get_बफर(s, error_संहिता);
    यदि(U_FAILURE(error_संहिता)) {
        प्रत्यागम 0;
    }
    USet_विस्तृतिः_Condition विस्तृतिः_Condition=USET_विस्तृतिः_SIMPLE;
    प्रति(int32_t पूर्व_विस्तृतिः_सीमा=0; पूर्व_विस्तृतिः_सीमा<s.length();) {
        int32_t विस्तृतिः_सीमा=समुच्चयः.विस्तृतिः(s, पूर्व_विस्तृतिः_सीमा, विस्तृतिः_Condition);
        यदि(विस्तृतिः_Condition==USET_विस्तृतिः_NOT_CONTAINED) {
            विस्तृतिः_Condition=USET_विस्तृतिः_SIMPLE;
        } अन्यथा {
            int32_t yes_सीमा=
                पूर्व_विस्तृतिः_सीमा+
                norm2.विस्तृतिः_शीघ्र_Check_Yes(
                    s.अस्थायी_Sub_String_Between(पूर्व_विस्तृतिः_सीमा, विस्तृतिः_सीमा), error_संहिता);
            यदि(U_FAILURE(error_संहिता) || yes_सीमा<विस्तृतिः_सीमा) {
                प्रत्यागम yes_सीमा;
            }
            विस्तृतिः_Condition=USET_विस्तृतिः_NOT_CONTAINED;
        }
        पूर्व_विस्तृतिः_सीमा=विस्तृतिः_सीमा;
    }
    प्रत्यागम s.length();
}

UBool
FilteredNormalizer2::hasBoundaryBefore(UChar32 c) ध्रुव {
    प्रत्यागम !समुच्चयः.अन्तर्भवति(c) || norm2.hasBoundaryBefore(c);
}

UBool
FilteredNormalizer2::hasBoundaryAfter(UChar32 c) ध्रुव {
    प्रत्यागम !समुच्चयः.अन्तर्भवति(c) || norm2.hasBoundaryAfter(c);
}

UBool
FilteredNormalizer2::isInert(UChar32 c) ध्रुव {
    प्रत्यागम !समुच्चयः.अन्तर्भवति(c) || norm2.isInert(c);
}

U_NAMESPACE_अन्तः

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

U_CAPI UNormalizer2* U_EXPORT2
unorm2_openFiltered(ध्रुव UNormalizer2* norm2 U_LIFETIME_BOUND,
                    ध्रुव USet* filter_समुच्चयः U_LIFETIME_BOUND,
                    UError_संहिता* p_Error_संहिता) {
    यदि(U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम शून्यसूचकः;
    }
    यदि(filter_समुच्चयः==शून्यसूचकः) {
        *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम शून्यसूचकः;
    }
    Normalizer2 *fn2=नूतन FilteredNormalizer2(*(Normalizer2 *)norm2,
                                             *Unicode_समुच्चयः::fromUSet(filter_समुच्चयः));
    यदि(fn2==शून्यसूचकः) {
        *p_Error_संहिता=U_स्मृतिः_आवण्टनम्_ERROR;
    }
    प्रत्यागम (UNormalizer2 *)fn2;
}

#यदि_अन्तः  // !UCONFIG_NO_NORMALIZATION
