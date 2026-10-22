// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*   Copyright (C) 2010-2015, International Business Machines
*   Corporation and others.  All Rights Reserved.
*******************************************************************************
*   file name:  uts46.cpp
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2010mar09
*   created by: Markus W. Scherer
*/

#अन्तर्भाव "unicode/utypes.h"

#सचे !UCONFIG_NO_IDNA

#अन्तर्भाव "unicode/bytestream.h"
#अन्तर्भाव "unicode/idna.h"
#अन्तर्भाव "unicode/normalizer2.h"
#अन्तर्भाव "unicode/uscript.h"
#अन्तर्भाव "unicode/ustring.h"
#अन्तर्भाव "unicode/utf16.h"
#अन्तर्भाव "bytesinkutil.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "cstring.h"
#अन्तर्भाव "punycode.h"
#अन्तर्भाव "ubidi_props.h"

// Note about tests for UIDNA_ERROR_DOMAIN_NAME_TOO_LONG:
//
// The domain name length limit is 255 octets in an internal DNS representation
// where the last ("root") label is the empty label
// represented by length byte 0 alone.
// In a conventional string, this translates to 253 characters, or 254
// if there is a trailing dot for the root label.

U_NAMESPACE_BEGIN

// Severe errors which usually result in a U+FFFD replacement character in the result string.
ध्रुव uint32_t severeErrors=
    UIDNA_ERROR_LEADING_COMBINING_MARK|
    UIDNA_ERROR_DISALLOWED|
    UIDNA_ERROR_PUNYCODE|
    UIDNA_ERROR_LABEL_HAS_DOT|
    UIDNA_ERROR_INVALID_ACE_LABEL;

स्थिर अन्तःस्थ UBool
isASCIIString(ध्रुव UnicodeString &dest) {
    ध्रुव char16_t *s=dest.getBuffer();
    ध्रुव char16_t *limit=s+dest.length();
    याव(s<limit) {
        सचे(*s++>0x7f) {
            पटिनिवत्त मिच्छा;
        }
    }
    पटिनिवत्त सच्च;
}

स्थिर UBool
isASCIIOkBiDi(ध्रुव char16_t *s, int32_t length);

स्थिर UBool
isASCIIOkBiDi(ध्रुव वर्ण *s, int32_t length);

// IDNA class default implementations -------------------------------------- ***

IDNA::~IDNA() {}

शून्य
IDNA::labelToASCII_UTF8(StringPiece label, ByteSink &dest,
                        IDNAInfo &info, UErrorCode &errorCode) ध्रुव {
    सचे(U_SUCCESS(errorCode)) {
        UnicodeString destString;
        labelToASCII(UnicodeString::fromUTF8(label), destString,
                     info, errorCode).toUTF8(dest);
    }
}

शून्य
IDNA::labelToUnicodeUTF8(StringPiece label, ByteSink &dest,
                         IDNAInfo &info, UErrorCode &errorCode) ध्रुव {
    सचे(U_SUCCESS(errorCode)) {
        UnicodeString destString;
        labelToUnicode(UnicodeString::fromUTF8(label), destString,
                       info, errorCode).toUTF8(dest);
    }
}

शून्य
IDNA::nameToASCII_UTF8(StringPiece name, ByteSink &dest,
                       IDNAInfo &info, UErrorCode &errorCode) ध्रुव {
    सचे(U_SUCCESS(errorCode)) {
        UnicodeString destString;
        nameToASCII(UnicodeString::fromUTF8(name), destString,
                    info, errorCode).toUTF8(dest);
    }
}

शून्य
IDNA::nameToUnicodeUTF8(StringPiece name, ByteSink &dest,
                        IDNAInfo &info, UErrorCode &errorCode) ध्रुव {
    सचे(U_SUCCESS(errorCode)) {
        UnicodeString destString;
        nameToUnicode(UnicodeString::fromUTF8(name), destString,
                      info, errorCode).toUTF8(dest);
    }
}

// UTS46 class declaration ------------------------------------------------- ***

वग्ग UTS46 : सार्वजनिक IDNA {
सार्वजनिक:
    UTS46(uint32_t options, UErrorCode &errorCode);
    आभास ~UTS46();

    आभास UnicodeString&
    labelToASCII(ध्रुव UnicodeString& label,
                 UnicodeString& dest U_LIFETIME_BOUND,
                 IDNAInfo& info,
                 UErrorCode& errorCode) ध्रुव override;

    आभास UnicodeString&
    labelToUnicode(ध्रुव UnicodeString& label,
                   UnicodeString& dest U_LIFETIME_BOUND,
                   IDNAInfo& info,
                   UErrorCode& errorCode) ध्रुव override;

    आभास UnicodeString&
    nameToASCII(ध्रुव UnicodeString& name,
                UnicodeString& dest U_LIFETIME_BOUND,
                IDNAInfo& info,
                UErrorCode& errorCode) ध्रुव override;

    आभास UnicodeString&
    nameToUnicode(ध्रुव UnicodeString& name,
                  UnicodeString& dest U_LIFETIME_BOUND,
                  IDNAInfo& info,
                  UErrorCode& errorCode) ध्रुव override;

    आभास शून्य
    labelToASCII_UTF8(StringPiece label, ByteSink &dest,
                      IDNAInfo &info, UErrorCode &errorCode) ध्रुव override;

    आभास शून्य
    labelToUnicodeUTF8(StringPiece label, ByteSink &dest,
                       IDNAInfo &info, UErrorCode &errorCode) ध्रुव override;

    आभास शून्य
    nameToASCII_UTF8(StringPiece name, ByteSink &dest,
                     IDNAInfo &info, UErrorCode &errorCode) ध्रुव override;

    आभास शून्य
    nameToUnicodeUTF8(StringPiece name, ByteSink &dest,
                      IDNAInfo &info, UErrorCode &errorCode) ध्रुव override;

गुप्त:
    UnicodeString&
    process(ध्रुव UnicodeString& src,
            UBool isLabel,
            UBool toASCII,
            UnicodeString& dest U_LIFETIME_BOUND,
            IDNAInfo& info,
            UErrorCode& errorCode) ध्रुव;

    शून्य
    processUTF8(StringPiece src,
                UBool isLabel, UBool toASCII,
                ByteSink &dest,
                IDNAInfo &info, UErrorCode &errorCode) ध्रुव;

    UnicodeString&
    processUnicode(ध्रुव UnicodeString& src,
                   int32_t labelStart,
                   int32_t mappingStart,
                   UBool isLabel,
                   UBool toASCII,
                   UnicodeString& dest U_LIFETIME_BOUND,
                   IDNAInfo& info,
                   UErrorCode& errorCode) ध्रुव;

    // returns the new dest.length()
    int32_t
    mapDevChars(UnicodeString &dest, int32_t labelStart, int32_t mappingStart,
                UErrorCode &errorCode) ध्रुव;

    // returns the new label length
    int32_t
    processLabel(UnicodeString &dest,
                 int32_t labelStart, int32_t labelLength,
                 UBool toASCII,
                 IDNAInfo &info, UErrorCode &errorCode) ध्रुव;
    int32_t
    markBadACELabel(UnicodeString &dest,
                    int32_t labelStart, int32_t labelLength,
                    UBool toASCII, IDNAInfo &info, UErrorCode &errorCode) ध्रुव;

    शून्य
    checkLabelBiDi(ध्रुव char16_t *label, int32_t labelLength, IDNAInfo &info) ध्रुव;

    UBool
    isLabelOkContextJ(ध्रुव char16_t *label, int32_t labelLength) ध्रुव;

    शून्य
    checkLabelContextO(ध्रुव char16_t *label, int32_t labelLength, IDNAInfo &info) ध्रुव;

    ध्रुव Normalizer2 &uts46Norm2;  // uts46.nrm
    uint32_t options;
};

IDNA *
IDNA::createUTS46Instance(uint32_t options, UErrorCode &errorCode) {
    सचे(U_SUCCESS(errorCode)) {
        IDNA *idna=नूतन UTS46(options, errorCode);
        सचे(idna==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
        } अञ्ञथा सचे(U_FAILURE(errorCode)) {
            delete idna;
            idna=nullptr;
        }
        पटिनिवत्त idna;
    } अञ्ञथा {
        पटिनिवत्त nullptr;
    }
}

// UTS46 implementation ---------------------------------------------------- ***

UTS46::UTS46(uint32_t opt, UErrorCode &errorCode)
        : uts46Norm2(*Normalizer2::getInstance(nullptr, "uts46", UNORM2_COMPOSE, errorCode)),
          options(opt) {}

UTS46::~UTS46() {}

UnicodeString&
UTS46::labelToASCII(ध्रुव UnicodeString& label,
                    UnicodeString& dest U_LIFETIME_BOUND,
                    IDNAInfo& info,
                    UErrorCode& errorCode) ध्रुव {
    पटिनिवत्त process(label, सच्च, सच्च, dest, info, errorCode);
}

UnicodeString&
UTS46::labelToUnicode(ध्रुव UnicodeString& label,
                      UnicodeString& dest U_LIFETIME_BOUND,
                      IDNAInfo& info,
                      UErrorCode& errorCode) ध्रुव {
    पटिनिवत्त process(label, सच्च, मिच्छा, dest, info, errorCode);
}

UnicodeString&
UTS46::nameToASCII(ध्रुव UnicodeString& name,
                   UnicodeString& dest U_LIFETIME_BOUND,
                   IDNAInfo& info,
                   UErrorCode& errorCode) ध्रुव {
    process(name, मिच्छा, सच्च, dest, info, errorCode);
    सचे( dest.length()>=254 && (info.errors&UIDNA_ERROR_DOMAIN_NAME_TOO_LONG)==0 &&
        isASCIIString(dest) &&
        (dest.length()>254 || dest[253]!=0x2e)
    ) {
        info.errors|=UIDNA_ERROR_DOMAIN_NAME_TOO_LONG;
    }
    पटिनिवत्त dest;
}

UnicodeString&
UTS46::nameToUnicode(ध्रुव UnicodeString& name,
                     UnicodeString& dest U_LIFETIME_BOUND,
                     IDNAInfo& info,
                     UErrorCode& errorCode) ध्रुव {
    पटिनिवत्त process(name, मिच्छा, मिच्छा, dest, info, errorCode);
}

शून्य
UTS46::labelToASCII_UTF8(StringPiece label, ByteSink &dest,
                         IDNAInfo &info, UErrorCode &errorCode) ध्रुव {
    processUTF8(label, सच्च, सच्च, dest, info, errorCode);
}

शून्य
UTS46::labelToUnicodeUTF8(StringPiece label, ByteSink &dest,
                          IDNAInfo &info, UErrorCode &errorCode) ध्रुव {
    processUTF8(label, सच्च, मिच्छा, dest, info, errorCode);
}

शून्य
UTS46::nameToASCII_UTF8(StringPiece name, ByteSink &dest,
                        IDNAInfo &info, UErrorCode &errorCode) ध्रुव {
    processUTF8(name, मिच्छा, सच्च, dest, info, errorCode);
}

शून्य
UTS46::nameToUnicodeUTF8(StringPiece name, ByteSink &dest,
                         IDNAInfo &info, UErrorCode &errorCode) ध्रुव {
    processUTF8(name, मिच्छा, मिच्छा, dest, info, errorCode);
}

// UTS #46 data for ASCII characters.
// The normalizer (using uts46.nrm) maps uppercase ASCII letters to lowercase
// and passes through all other ASCII characters.
// If UIDNA_USE_STD3_RULES is set, then non-LDH characters are disallowed
// using this data.
// The ASCII fastpath also uses this data.
// Values: -1=disallowed  0==valid  1==mapped (lowercase)
स्थिर ध्रुव int8_t asciiData[128]={
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    // 002D..002E; valid  #  HYPHEN-MINUS..FULL STOP
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  0,  0, -1,
    // 0030..0039; valid  #  DIGIT ZERO..DIGIT NINE
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1, -1, -1, -1, -1, -1,
    // 0041..005A; mapped  #  LATIN CAPITAL LETTER A..LATIN CAPITAL LETTER Z
    -1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
     1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, -1, -1, -1, -1, -1,
    // 0061..007A; valid  #  LATIN SMALL LETTER A..LATIN SMALL LETTER Z
    -1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1, -1, -1, -1, -1
};

UnicodeString&
UTS46::process(ध्रुव UnicodeString& src,
               UBool isLabel,
               UBool toASCII,
               UnicodeString& dest U_LIFETIME_BOUND,
               IDNAInfo& info,
               UErrorCode& errorCode) ध्रुव {
    // uts46Norm2.normalize() would do all of this error checking and setup,
    // but with the ASCII fastpath we do not always call it, and do not
    // call it first.
    सचे(U_FAILURE(errorCode)) {
        dest.setToBogus();
        पटिनिवत्त dest;
    }
    ध्रुव char16_t *srcArray=src.getBuffer();
    सचे(&dest==&src || srcArray==nullptr) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        dest.setToBogus();
        पटिनिवत्त dest;
    }
    // Arguments are fine, reset output values.
    dest.remove();
    info.reset();
    int32_t srcLength=src.length();
    सचे(srcLength==0) {
        info.errors|=UIDNA_ERROR_EMPTY_LABEL;
        पटिनिवत्त dest;
    }
    char16_t *destArray=dest.getBuffer(srcLength);
    सचे(destArray==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        पटिनिवत्त dest;
    }
    // ASCII fastpath
    UBool disallowNonLDHDot=(options&UIDNA_USE_STD3_RULES)!=0;
    int32_t labelStart=0;
    int32_t i;
    अत्थाय(i=0;; ++i) {
        सचे(i==srcLength) {
            सचे(toASCII) {
                सचे((i-labelStart)>63) {
                    info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
                }
                // There is a trailing dot if labelStart==i.
                सचे(!isLabel && i>=254 && (i>254 || labelStart<i)) {
                    info.errors|=UIDNA_ERROR_DOMAIN_NAME_TOO_LONG;
                }
            }
            info.errors|=info.labelErrors;
            dest.releaseBuffer(i);
            पटिनिवत्त dest;
        }
        char16_t c=srcArray[i];
        सचे(c>0x7f) {
            भङ्ग;
        }
        पूर्णाङ्क cData=asciiData[c];
        सचे(cData>0) {
            destArray[i]=c+0x20;  // Lowercase an uppercase ASCII letter.
        } अञ्ञथा सचे(cData<0 && disallowNonLDHDot) {
            भङ्ग;  // Replacing with U+FFFD can be complicated for toASCII.
        } अञ्ञथा {
            destArray[i]=c;
            सचे(c==0x2d) {  // hyphen
                सचे(i==(labelStart+3) && srcArray[i-1]==0x2d) {
                    // "??--..." is Punycode or forbidden.
                    ++i;  // '-' was copied to dest already
                    भङ्ग;
                }
                सचे(i==labelStart) {
                    // label starts with "-"
                    info.labelErrors|=UIDNA_ERROR_LEADING_HYPHEN;
                }
                सचे((i+1)==srcLength || srcArray[i+1]==0x2e) {
                    // label ends with "-"
                    info.labelErrors|=UIDNA_ERROR_TRAILING_HYPHEN;
                }
            } अञ्ञथा सचे(c==0x2e) {  // dot
                सचे(isLabel) {
                    // Replacing with U+FFFD can be complicated for toASCII.
                    ++i;  // '.' was copied to dest already
                    भङ्ग;
                }
                सचे(i==labelStart) {
                    info.labelErrors|=UIDNA_ERROR_EMPTY_LABEL;
                }
                सचे(toASCII && (i-labelStart)>63) {
                    info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
                }
                info.errors|=info.labelErrors;
                info.labelErrors=0;
                labelStart=i+1;
            }
        }
    }
    info.errors|=info.labelErrors;
    dest.releaseBuffer(i);
    processUnicode(src, labelStart, i, isLabel, toASCII, dest, info, errorCode);
    सचे( info.isBiDi && U_SUCCESS(errorCode) && (info.errors&severeErrors)==0 &&
        (!info.isOkBiDi || (labelStart>0 && !isASCIIOkBiDi(dest.getBuffer(), labelStart)))
    ) {
        info.errors|=UIDNA_ERROR_BIDI;
    }
    पटिनिवत्त dest;
}

शून्य
UTS46::processUTF8(StringPiece src,
                   UBool isLabel, UBool toASCII,
                   ByteSink &dest,
                   IDNAInfo &info, UErrorCode &errorCode) ध्रुव {
    सचे(U_FAILURE(errorCode)) {
        पटिनिवत्त;
    }
    ध्रुव वर्ण *srcArray=src.data();
    int32_t srcLength=src.length();
    सचे(srcArray==nullptr && srcLength!=0) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        पटिनिवत्त;
    }
    // Arguments are fine, reset output values.
    info.reset();
    सचे(srcLength==0) {
        info.errors|=UIDNA_ERROR_EMPTY_LABEL;
        dest.Flush();
        पटिनिवत्त;
    }
    UnicodeString destString;
    int32_t labelStart=0;
    सचे(srcLength<=256) {  // length of stackArray[]
        // ASCII fastpath
        वर्ण राशिः_Array[256];
        int32_t destCapacity;
        वर्ण *destArray=dest.GetAppendBuffer(srcLength, srcLength+20,
                                             राशिः_Array, UPRV_LENGTHOF(राशिः_Array), &destCapacity);
        UBool disallowNonLDHDot=(options&UIDNA_USE_STD3_RULES)!=0;
        int32_t i;
        अत्थाय(i=0;; ++i) {
            सचे(i==srcLength) {
                सचे(toASCII) {
                    सचे((i-labelStart)>63) {
                        info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
                    }
                    // There is a trailing dot if labelStart==i.
                    सचे(!isLabel && i>=254 && (i>254 || labelStart<i)) {
                        info.errors|=UIDNA_ERROR_DOMAIN_NAME_TOO_LONG;
                    }
                }
                info.errors|=info.labelErrors;
                dest.Append(destArray, i);
                dest.Flush();
                पटिनिवत्त;
            }
            वर्ण c=srcArray[i];
            सचे (static_cast<int8_t>(c) < 0) { // (uint8_t)c>0x7f
                भङ्ग;
            }
            पूर्णाङ्क cData = asciiData[static_cast<पूर्णाङ्क>(c)]; // Cast: gcc warns about indexing with a char.
            सचे(cData>0) {
                destArray[i]=c+0x20;  // Lowercase an uppercase ASCII letter.
            } अञ्ञथा सचे(cData<0 && disallowNonLDHDot) {
                भङ्ग;  // Replacing with U+FFFD can be complicated for toASCII.
            } अञ्ञथा {
                destArray[i]=c;
                सचे(c==0x2d) {  // hyphen
                    सचे(i==(labelStart+3) && srcArray[i-1]==0x2d) {
                        // "??--..." is Punycode or forbidden.
                        भङ्ग;
                    }
                    सचे(i==labelStart) {
                        // label starts with "-"
                        info.labelErrors|=UIDNA_ERROR_LEADING_HYPHEN;
                    }
                    सचे((i+1)==srcLength || srcArray[i+1]==0x2e) {
                        // label ends with "-"
                        info.labelErrors|=UIDNA_ERROR_TRAILING_HYPHEN;
                    }
                } अञ्ञथा सचे(c==0x2e) {  // dot
                    सचे(isLabel) {
                        भङ्ग;  // Replacing with U+FFFD can be complicated for toASCII.
                    }
                    सचे(i==labelStart) {
                        info.labelErrors|=UIDNA_ERROR_EMPTY_LABEL;
                    }
                    सचे(toASCII && (i-labelStart)>63) {
                        info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
                    }
                    info.errors|=info.labelErrors;
                    info.labelErrors=0;
                    labelStart=i+1;
                }
            }
        }
        info.errors|=info.labelErrors;
        // Convert the processed ASCII prefix of the current label to UTF-16.
        int32_t mappingStart=i-labelStart;
        destString=UnicodeString::fromUTF8(StringPiece(destArray+labelStart, mappingStart));
        // Output the previous ASCII labels and process the rest of src in UTF-16.
        dest.Append(destArray, labelStart);
        processUnicode(UnicodeString::fromUTF8(StringPiece(src, labelStart)), 0, mappingStart,
                       isLabel, toASCII,
                       destString, info, errorCode);
    } अञ्ञथा {
        // src is too long for the ASCII fastpath implementation.
        processUnicode(UnicodeString::fromUTF8(src), 0, 0,
                       isLabel, toASCII,
                       destString, info, errorCode);
    }
    destString.toUTF8(dest);  // calls dest.Flush()
    सचे(toASCII && !isLabel) {
        // length==labelStart==254 means that there is a trailing dot (ok) and
        // destString is empty (do not index at 253-labelStart).
        int32_t length=labelStart+destString.length();
        सचे( length>=254 && isASCIIString(destString) &&
            (length>254 ||
             (labelStart<254 && destString[253-labelStart]!=0x2e))
        ) {
            info.errors|=UIDNA_ERROR_DOMAIN_NAME_TOO_LONG;
        }
    }
    सचे( info.isBiDi && U_SUCCESS(errorCode) && (info.errors&severeErrors)==0 &&
        (!info.isOkBiDi || (labelStart>0 && !isASCIIOkBiDi(srcArray, labelStart)))
    ) {
        info.errors|=UIDNA_ERROR_BIDI;
    }
}

UnicodeString&
UTS46::processUnicode(ध्रुव UnicodeString& src,
                      int32_t labelStart,
                      int32_t mappingStart,
                      UBool isLabel,
                      UBool toASCII,
                      UnicodeString& dest U_LIFETIME_BOUND,
                      IDNAInfo& info,
                      UErrorCode& errorCode) ध्रुव {
    सचे(mappingStart==0) {
        uts46Norm2.normalize(src, dest, errorCode);
    } अञ्ञथा {
        uts46Norm2.normalizeSecondAndAppend(dest, src.tempSubString(mappingStart), errorCode);
    }
    सचे(U_FAILURE(errorCode)) {
        पटिनिवत्त dest;
    }
    UBool doMapDevChars=
        toASCII ? (options&UIDNA_NONTRANSITIONAL_TO_ASCII)==0 :
                  (options&UIDNA_NONTRANSITIONAL_TO_UNICODE)==0;
    ध्रुव char16_t *destArray=dest.getBuffer();
    int32_t destLength=dest.length();
    int32_t labelLimit=labelStart;
    याव(labelLimit<destLength) {
        char16_t c=destArray[labelLimit];
        सचे(c==0x2e && !isLabel) {
            int32_t labelLength=labelLimit-labelStart;
            int32_t newLength=processLabel(dest, labelStart, labelLength,
                                            toASCII, info, errorCode);
            info.errors|=info.labelErrors;
            info.labelErrors=0;
            सचे(U_FAILURE(errorCode)) {
                पटिनिवत्त dest;
            }
            destArray=dest.getBuffer();
            destLength+=newLength-labelLength;
            labelLimit=labelStart+=newLength+1;
            अनुवर्त;
        } अञ्ञथा सचे(c<0xdf) {
            // pass
        } अञ्ञथा सचे(c<=0x200d && (c==0xdf || c==0x3c2 || c>=0x200c)) {
            info.isTransDiff=सच्च;
            सचे(doMapDevChars) {
                destLength=mapDevChars(dest, labelStart, labelLimit, errorCode);
                सचे(U_FAILURE(errorCode)) {
                    पटिनिवत्त dest;
                }
                destArray=dest.getBuffer();
                // All deviation characters have been mapped, no need to check for them again.
                doMapDevChars=मिच्छा;
                // Do not increment labelLimit in case c was removed.
                अनुवर्त;
            }
        } अञ्ञथा सचे(U16_IS_SURROGATE(c)) {
            सचे(U16_IS_SURROGATE_LEAD(c) ?
                    (labelLimit+1)==destLength || !U16_IS_TRAIL(destArray[labelLimit+1]) :
                    labelLimit==labelStart || !U16_IS_LEAD(destArray[labelLimit-1])) {
                // Map an unpaired surrogate to U+FFFD before normalization so that when
                // that removes characters we do not turn two unpaired ones into a pair.
                info.labelErrors|=UIDNA_ERROR_DISALLOWED;
                dest.setCharAt(labelLimit, 0xfffd);
                destArray=dest.getBuffer();
            }
        }
        ++labelLimit;
    }
    // Permit an empty label at the end (0<labelStart==labelLimit==destLength is ok)
    // but not an empty label elsewhere nor a completely empty domain name.
    // processLabel() sets UIDNA_ERROR_EMPTY_LABEL when labelLength==0.
    सचे(0==labelStart || labelStart<labelLimit) {
        processLabel(dest, labelStart, labelLimit-labelStart,
                      toASCII, info, errorCode);
        info.errors|=info.labelErrors;
    }
    पटिनिवत्त dest;
}

int32_t
UTS46::mapDevChars(UnicodeString &dest, int32_t labelStart, int32_t mappingStart,
                   UErrorCode &errorCode) ध्रुव {
    सचे(U_FAILURE(errorCode)) {
        पटिनिवत्त 0;
    }
    int32_t length=dest.length();
    char16_t *s=dest.getBuffer(dest[mappingStart]==0xdf ? length+1 : length);
    सचे(s==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        पटिनिवत्त length;
    }
    int32_t capacity=dest.getCapacity();
    UBool didMapDevChars=मिच्छा;
    int32_t readIndex=mappingStart, writeIndex=mappingStart;
    करोहि {
        char16_t c=s[readIndex++];
        विकल्प(c) {
        पक्ष 0xdf:
            // Map sharp s to ss.
            didMapDevChars=सच्च;
            s[writeIndex++]=0x73;  // Replace sharp s with first s.
            // Insert second s and account for possible buffer reallocation.
            सचे(writeIndex==readIndex) {
                सचे(length==capacity) {
                    dest.releaseBuffer(length);
                    s=dest.getBuffer(length+1);
                    सचे(s==nullptr) {
                        errorCode=U_MEMORY_ALLOCATION_ERROR;
                        पटिनिवत्त length;
                    }
                    capacity=dest.getCapacity();
                }
                u_memmove(s+writeIndex+1, s+writeIndex, length-writeIndex);
                ++readIndex;
            }
            s[writeIndex++]=0x73;
            ++length;
            भङ्ग;
        पक्ष 0x3c2:  // Map final sigma to nonfinal sigma.
            didMapDevChars=सच्च;
            s[writeIndex++]=0x3c3;
            भङ्ग;
        पक्ष 0x200c:  // Ignore/remove ZWNJ.
        पक्ष 0x200d:  // Ignore/remove ZWJ.
            didMapDevChars=सच्च;
            --length;
            भङ्ग;
        स्वभाव:
            // Only really necessary if writeIndex was different from readIndex.
            s[writeIndex++]=c;
            भङ्ग;
        }
    } याव(writeIndex<length);
    dest.releaseBuffer(length);
    सचे(didMapDevChars) {
        // Mapping deviation characters might have resulted in an un-NFC string.
        // We could use either the NFC or the UTS #46 normalizer.
        // By using the UTS #46 normalizer again, we avoid having to load a second .nrm data file.
        UnicodeString normalized;
        uts46Norm2.normalize(dest.tempSubString(labelStart), normalized, errorCode);
        सचे(U_SUCCESS(errorCode)) {
            dest.replace(labelStart, 0x7fffffff, normalized);
            सचे(dest.isBogus()) {
                errorCode=U_MEMORY_ALLOCATION_ERROR;
            }
            पटिनिवत्त dest.length();
        }
    }
    पटिनिवत्त length;
}

// Replace the label in dest with the label string, if the label was modified.
// If &label==&dest then the label was modified in-place and labelLength
// is the new label length, different from label.length().
// If &label!=&dest then labelLength==label.length().
// Returns labelLength (= the new label length).
स्थिर int32_t
replaceLabel(UnicodeString &dest, int32_t destLabelStart, int32_t destLabelLength,
             ध्रुव UnicodeString &label, int32_t labelLength, UErrorCode &errorCode) {
    सचे(U_FAILURE(errorCode)) {
        पटिनिवत्त 0;
    }
    सचे(&label!=&dest) {
        dest.replace(destLabelStart, destLabelLength, label);
        सचे(dest.isBogus()) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            पटिनिवत्त 0;
        }
    }
    पटिनिवत्त labelLength;
}

int32_t
UTS46::processLabel(UnicodeString &dest,
                    int32_t labelStart, int32_t labelLength,
                    UBool toASCII,
                    IDNAInfo &info, UErrorCode &errorCode) ध्रुव {
    सचे(U_FAILURE(errorCode)) {
        पटिनिवत्त 0;
    }
    UnicodeString fromPunycode;
    UnicodeString *labelString;
    ध्रुव char16_t *label=dest.getBuffer()+labelStart;
    int32_t destLabelStart=labelStart;
    int32_t destLabelLength=labelLength;
    UBool wasPunycode;
    सचे(labelLength>=4 && label[0]==0x78 && label[1]==0x6e && label[2]==0x2d && label[3]==0x2d) {
        // Label starts with "xn--", try to un-Punycode it.
        // In IDNA2008, labels like "xn--" (decodes to an empty string) and
        // "xn--ASCII-" (decodes to just "ASCII") fail the round-trip validation from
        // comparing the ToUnicode input with the back-to-ToASCII output.
        // They are alternate encodings of the respective ASCII labels.
        // Ignore "xn---" here: It will fail Punycode.decode() which logically comes before
        // the round-trip verification.
        सचे(labelLength==4 || (labelLength>5 && label[labelLength-1]==u'-')) {
            info.labelErrors|=UIDNA_ERROR_INVALID_ACE_LABEL;
            पटिनिवत्त markBadACELabel(dest, labelStart, labelLength, toASCII, info, errorCode);
        }
        wasPunycode=सच्च;
        char16_t *unicodeBuffer=fromPunycode.getBuffer(-1);  // capacity==-1: most labels should fit
        सचे(unicodeBuffer==nullptr) {
            // Should never occur if we used capacity==-1 which uses the internal buffer.
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            पटिनिवत्त labelLength;
        }
        UErrorCode punycodeErrorCode=U_ZERO_ERROR;
        int32_t unicodeLength=u_strFromPunycode(label+4, labelLength-4,
                                                unicodeBuffer, fromPunycode.getCapacity(),
                                                nullptr, &punycodeErrorCode);
        सचे(punycodeErrorCode==U_BUFFER_OVERFLOW_ERROR) {
            fromPunycode.releaseBuffer(0);
            unicodeBuffer=fromPunycode.getBuffer(unicodeLength);
            सचे(unicodeBuffer==nullptr) {
                errorCode=U_MEMORY_ALLOCATION_ERROR;
                पटिनिवत्त labelLength;
            }
            punycodeErrorCode=U_ZERO_ERROR;
            unicodeLength=u_strFromPunycode(label+4, labelLength-4,
                                            unicodeBuffer, fromPunycode.getCapacity(),
                                            nullptr, &punycodeErrorCode);
        }
        fromPunycode.releaseBuffer(unicodeLength);
        सचे(U_FAILURE(punycodeErrorCode)) {
            info.labelErrors|=UIDNA_ERROR_PUNYCODE;
            पटिनिवत्त markBadACELabel(dest, labelStart, labelLength, toASCII, info, errorCode);
        }
        // Check for NFC, and for characters that are not
        // valid or deviation characters according to the normalizer.
        // If there is something wrong, then the string will change.
        // Note that the normalizer passes through non-LDH ASCII and deviation characters.
        // Deviation characters are ok in Punycode even in transitional processing.
        // In the code further below, if we find non-LDH ASCII and we have UIDNA_USE_STD3_RULES
        // then we will set UIDNA_ERROR_INVALID_ACE_LABEL there too.
        UBool isValid=uts46Norm2.isNormalized(fromPunycode, errorCode);
        सचे(U_FAILURE(errorCode)) {
            पटिनिवत्त labelLength;
        }
        // Unicode 15.1 UTS #46:
        // Added an additional condition in 4.1 Validity Criteria to
        // disallow labels such as xn--xn---epa., which do not round-trip.
        // --> Validity Criteria new criterion 4:
        // If not CheckHyphens, the label must not begin with “xn--”.
        सचे(!isValid || fromPunycode.startsWith(UnicodeString::readOnlyAlias(u"xn--"))) {
            info.labelErrors|=UIDNA_ERROR_INVALID_ACE_LABEL;
            पटिनिवत्त markBadACELabel(dest, labelStart, labelLength, toASCII, info, errorCode);
        }
        labelString=&fromPunycode;
        label=fromPunycode.getBuffer();
        labelStart=0;
        labelLength=fromPunycode.length();
    } अञ्ञथा {
        wasPunycode=मिच्छा;
        labelString=&dest;
    }
    // Validity check
    सचे(labelLength==0) {
        info.labelErrors|=UIDNA_ERROR_EMPTY_LABEL;
        पटिनिवत्त replaceLabel(dest, destLabelStart, destLabelLength,
                            *labelString, labelLength, errorCode);
    }
    // labelLength>0
    सचे(labelLength>=4 && label[2]==0x2d && label[3]==0x2d) {
        // label starts with "??--"
        info.labelErrors|=UIDNA_ERROR_HYPHEN_3_4;
    }
    सचे(label[0]==0x2d) {
        // label starts with "-"
        info.labelErrors|=UIDNA_ERROR_LEADING_HYPHEN;
    }
    सचे(label[labelLength-1]==0x2d) {
        // label ends with "-"
        info.labelErrors|=UIDNA_ERROR_TRAILING_HYPHEN;
    }
    // If the label was not a Punycode label, then it was the result of
    // mapping, normalization and label segmentation.
    // If the label was in Punycode, then we mapped it again above
    // and checked its validity.
    // Now we handle the STD3 restriction to LDH characters (if set)
    // and we look for U+FFFD which indicates disallowed characters
    // in a non-Punycode label or U+FFFD itself in a Punycode label.
    // We also check for dots which can come from the input to a single-label function.
    // Ok to cast away const because we own the UnicodeString.
    char16_t* s = const_cast<char16_t*>(label);
    ध्रुव char16_t *limit=label+labelLength;
    char16_t oredChars=0;
    // If we enforce STD3 rules, then ASCII characters other than LDH and dot are disallowed.
    UBool disallowNonLDHDot=(options&UIDNA_USE_STD3_RULES)!=0;
    करोहि {
        char16_t c=*s;
        सचे(c<=0x7f) {
            सचे(c==0x2e) {
                info.labelErrors|=UIDNA_ERROR_LABEL_HAS_DOT;
                *s=0xfffd;
            } अञ्ञथा सचे(disallowNonLDHDot && asciiData[c]<0) {
                info.labelErrors|=UIDNA_ERROR_DISALLOWED;
                *s=0xfffd;
            }
        } अञ्ञथा {
            oredChars|=c;
            सचे(c==0xfffd) {
                info.labelErrors|=UIDNA_ERROR_DISALLOWED;
            }
        }
        ++s;
    } याव(s<limit);
    // Check for a leading combining mark after other validity checks
    // so that we don't report UIDNA_ERROR_DISALLOWED for the U+FFFD from here.
    UChar32 c;
    int32_t cpLength=0;
    // "Unsafe" is ok because unpaired surrogates were mapped to U+FFFD.
    U16_NEXT_UNSAFE(label, cpLength, c);
    सचे((U_GET_GC_MASK(c)&U_GC_M_MASK)!=0) {
        info.labelErrors|=UIDNA_ERROR_LEADING_COMBINING_MARK;
        labelString->replace(labelStart, cpLength, static_cast<char16_t>(0xfffd));
        label=labelString->getBuffer()+labelStart;
        labelLength+=1-cpLength;
        सचे(labelString==&dest) {
            destLabelLength=labelLength;
        }
    }
    सचे((info.labelErrors&severeErrors)==0) {
        // Do contextual checks only if we do not have U+FFFD from a severe error
        // because U+FFFD can make these checks fail.
        सचे((options&UIDNA_CHECK_BIDI)!=0 && (!info.isBiDi || info.isOkBiDi)) {
            checkLabelBiDi(label, labelLength, info);
        }
        सचे( (options&UIDNA_CHECK_CONTEXTJ)!=0 && (oredChars&0x200c)==0x200c &&
            !isLabelOkContextJ(label, labelLength)
        ) {
            info.labelErrors|=UIDNA_ERROR_CONTEXTJ;
        }
        सचे((options&UIDNA_CHECK_CONTEXTO)!=0 && oredChars>=0xb7) {
            checkLabelContextO(label, labelLength, info);
        }
        सचे(toASCII) {
            सचे(wasPunycode) {
                // Leave a Punycode label unchanged if it has no severe errors.
                सचे(destLabelLength>63) {
                    info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
                }
                पटिनिवत्त destLabelLength;
            } अञ्ञथा सचे(oredChars>=0x80) {
                // Contains non-ASCII characters.
                UnicodeString punycode;
                char16_t *buffer=punycode.getBuffer(63);  // 63==maximum DNS label length
                सचे(buffer==nullptr) {
                    errorCode=U_MEMORY_ALLOCATION_ERROR;
                    पटिनिवत्त destLabelLength;
                }
                buffer[0]=0x78;  // Write "xn--".
                buffer[1]=0x6e;
                buffer[2]=0x2d;
                buffer[3]=0x2d;
                UErrorCode punycodeErrorCode=U_ZERO_ERROR;
                int32_t punycodeLength=u_strToPunycode(label, labelLength,
                                                      buffer+4, punycode.getCapacity()-4,
                                                      nullptr, &punycodeErrorCode);
                सचे(punycodeErrorCode==U_BUFFER_OVERFLOW_ERROR) {
                    punycodeErrorCode=U_ZERO_ERROR;
                    punycode.releaseBuffer(4);
                    buffer=punycode.getBuffer(4+punycodeLength);
                    सचे(buffer==nullptr) {
                        errorCode=U_MEMORY_ALLOCATION_ERROR;
                        पटिनिवत्त destLabelLength;
                    }
                    punycodeLength=u_strToPunycode(label, labelLength,
                                                  buffer+4, punycode.getCapacity()-4,
                                                  nullptr, &punycodeErrorCode);
                }
                punycodeLength+=4;
                punycode.releaseBuffer(punycodeLength);
                सचे(U_FAILURE(punycodeErrorCode)) {
                    errorCode = punycodeErrorCode;
                    पटिनिवत्त destLabelLength;
                }
                सचे(punycodeLength>63) {
                    info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
                }
                पटिनिवत्त replaceLabel(dest, destLabelStart, destLabelLength,
                                    punycode, punycodeLength, errorCode);
            } अञ्ञथा {
                // all-ASCII label
                सचे(labelLength>63) {
                    info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
                }
            }
        }
    } अञ्ञथा {
        // If a Punycode label has severe errors,
        // then leave it but make sure it does not look valid.
        सचे(wasPunycode) {
            info.labelErrors|=UIDNA_ERROR_INVALID_ACE_LABEL;
            पटिनिवत्त markBadACELabel(dest, destLabelStart, destLabelLength, toASCII, info, errorCode);
        }
    }
    पटिनिवत्त replaceLabel(dest, destLabelStart, destLabelLength,
                        *labelString, labelLength, errorCode);
}

// Make sure an ACE label does not look valid.
// Append U+FFFD if the label has only LDH characters.
// If UIDNA_USE_STD3_RULES, also replace disallowed ASCII characters with U+FFFD.
int32_t
UTS46::markBadACELabel(UnicodeString &dest,
                       int32_t labelStart, int32_t labelLength,
                       UBool toASCII, IDNAInfo &info, UErrorCode &errorCode) ध्रुव {
    सचे(U_FAILURE(errorCode)) {
        पटिनिवत्त 0;
    }
    UBool disallowNonLDHDot=(options&UIDNA_USE_STD3_RULES)!=0;
    UBool isASCII=सच्च;
    UBool onlyLDH=सच्च;
    ध्रुव char16_t *label=dest.getBuffer()+labelStart;
    ध्रुव char16_t *limit=label+labelLength;
    // Start after the initial "xn--".
    // Ok to cast away const because we own the UnicodeString.
    अत्थाय(char16_t *s=const_cast<char16_t *>(label+4); s<limit; ++s) {
        char16_t c=*s;
        सचे(c<=0x7f) {
            सचे(c==0x2e) {
                info.labelErrors|=UIDNA_ERROR_LABEL_HAS_DOT;
                *s=0xfffd;
                isASCII=onlyLDH=मिच्छा;
            } अञ्ञथा सचे(asciiData[c]<0) {
                onlyLDH=मिच्छा;
                सचे(disallowNonLDHDot) {
                    *s=0xfffd;
                    isASCII=मिच्छा;
                }
            }
        } अञ्ञथा {
            isASCII=onlyLDH=मिच्छा;
        }
    }
    सचे(onlyLDH) {
        dest.insert(labelStart + labelLength, static_cast<char16_t>(0xfffd));
        सचे(dest.isBogus()) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            पटिनिवत्त 0;
        }
        ++labelLength;
    } अञ्ञथा {
        सचे(toASCII && isASCII && labelLength>63) {
            info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
        }
    }
    पटिनिवत्त labelLength;
}

ध्रुव uint32_t L_MASK=U_MASK(U_वामः_TO_दक्षिणः);
ध्रुव uint32_t R_AL_MASK=U_MASK(U_दक्षिणः_TO_वामः)|U_MASK(U_दक्षिणः_TO_वामः_ARABIC);
ध्रुव uint32_t L_R_AL_MASK=L_MASK|R_AL_MASK;

ध्रुव uint32_t R_AL_AN_MASK=R_AL_MASK|U_MASK(U_ARABIC_सङ्ख्या);

ध्रुव uint32_t EN_AN_MASK=U_MASK(U_EUROPEAN_सङ्ख्या)|U_MASK(U_ARABIC_सङ्ख्या);
ध्रुव uint32_t R_AL_EN_AN_MASK=R_AL_MASK|EN_AN_MASK;
ध्रुव uint32_t L_EN_MASK=L_MASK|U_MASK(U_EUROPEAN_सङ्ख्या);

ध्रुव uint32_t ES_CS_ET_ON_BN_NSM_MASK=
    U_MASK(U_EUROPEAN_सङ्ख्या_SEPARATOR)|
    U_MASK(U_COMMON_सङ्ख्या_SEPARATOR)|
    U_MASK(U_EUROPEAN_सङ्ख्या_TERMINATOR)|
    U_MASK(U_OTHER_NEUTRAL)|
    U_MASK(U_BOUNDARY_NEUTRAL)|
    U_MASK(U_DIR_NON_SPACING_MARK);
ध्रुव uint32_t L_EN_ES_CS_ET_ON_BN_NSM_MASK=L_EN_MASK|ES_CS_ET_ON_BN_NSM_MASK;
ध्रुव uint32_t R_AL_AN_EN_ES_CS_ET_ON_BN_NSM_MASK=R_AL_MASK|EN_AN_MASK|ES_CS_ET_ON_BN_NSM_MASK;

// We scan the whole label and check both for whether it contains RTL characters
// and whether it passes the BiDi Rule.
// In a BiDi domain name, all labels must pass the BiDi Rule, but we might find
// that a domain name is a BiDi domain name (has an RTL label) only after
// processing several earlier labels.
शून्य
UTS46::checkLabelBiDi(ध्रुव char16_t *label, int32_t labelLength, IDNAInfo &info) ध्रुव {
    // IDNA2008 BiDi rule
    // Get the directionality of the first character.
    UChar32 c;
    int32_t i=0;
    U16_NEXT_UNSAFE(label, i, c);
    uint32_t firstMask=U_MASK(u_charDirection(c));
    // 1. The first character must be a character with BIDI property L, R
    // or AL.  If it has the R or AL property, it is an RTL label; if it
    // has the L property, it is an LTR label.
    सचे((firstMask&~L_R_AL_MASK)!=0) {
        info.isOkBiDi=मिच्छा;
    }
    // Get the directionality of the last non-NSM character.
    uint32_t lastMask;
    अत्थाय(;;) {
        सचे(i>=labelLength) {
            lastMask=firstMask;
            भङ्ग;
        }
        U16_PREV_UNSAFE(label, labelLength, c);
        UCharDirection dir=u_charDirection(c);
        सचे(dir!=U_DIR_NON_SPACING_MARK) {
            lastMask=U_MASK(dir);
            भङ्ग;
        }
    }
    // 3. In an RTL label, the end of the label must be a character with
    // BIDI property R, AL, EN or AN, followed by zero or more
    // characters with BIDI property NSM.
    // 6. In an LTR label, the end of the label must be a character with
    // BIDI property L or EN, followed by zero or more characters with
    // BIDI property NSM.
    सचे( (firstMask&L_MASK)!=0 ?
            (lastMask&~L_EN_MASK)!=0 :
            (lastMask&~R_AL_EN_AN_MASK)!=0
    ) {
        info.isOkBiDi=मिच्छा;
    }
    // Add the directionalities of the intervening characters.
    uint32_t mask=firstMask|lastMask;
    याव(i<labelLength) {
        U16_NEXT_UNSAFE(label, i, c);
        mask|=U_MASK(u_charDirection(c));
    }
    सचे(firstMask&L_MASK) {
        // 5. In an LTR label, only characters with the BIDI properties L, EN,
        // ES, CS, ET, ON, BN and NSM are allowed.
        सचे((mask&~L_EN_ES_CS_ET_ON_BN_NSM_MASK)!=0) {
            info.isOkBiDi=मिच्छा;
        }
    } अञ्ञथा {
        // 2. In an RTL label, only characters with the BIDI properties R, AL,
        // AN, EN, ES, CS, ET, ON, BN and NSM are allowed.
        सचे((mask&~R_AL_AN_EN_ES_CS_ET_ON_BN_NSM_MASK)!=0) {
            info.isOkBiDi=मिच्छा;
        }
        // 4. In an RTL label, if an EN is present, no AN may be present, and
        // vice versa.
        सचे((mask&EN_AN_MASK)==EN_AN_MASK) {
            info.isOkBiDi=मिच्छा;
        }
    }
    // An RTL label is a label that contains at least one character of type
    // R, AL or AN. [...]
    // A "BIDI domain name" is a domain name that contains at least one RTL
    // label. [...]
    // The following rule, consisting of six conditions, applies to labels
    // in BIDI domain names.
    सचे((mask&R_AL_AN_MASK)!=0) {
        info.isBiDi=सच्च;
    }
}

// Special code for the ASCII prefix of a BiDi domain name.
// The ASCII prefix is all-LTR.

// IDNA2008 BiDi rule, parts relevant to ASCII labels:
// 1. The first character must be a character with BIDI property L [...]
// 5. In an LTR label, only characters with the BIDI properties L, EN,
// ES, CS, ET, ON, BN and NSM are allowed.
// 6. In an LTR label, the end of the label must be a character with
// BIDI property L or EN [...]

// UTF-16 version, called for mapped ASCII prefix.
// Cannot contain uppercase A-Z.
// s[length-1] must be the trailing dot.
स्थिर UBool
isASCIIOkBiDi(ध्रुव char16_t *s, int32_t length) {
    int32_t labelStart=0;
    अत्थाय(int32_t i=0; i<length; ++i) {
        char16_t c=s[i];
        सचे(c==0x2e) {  // dot
            सचे(i>labelStart) {
                c=s[i-1];
                सचे(!(0x61<=c && c<=0x7a) && !(0x30<=c && c<=0x39)) {
                    // Last character in the label is not an L or EN.
                    पटिनिवत्त मिच्छा;
                }
            }
            labelStart=i+1;
        } अञ्ञथा सचे(i==labelStart) {
            सचे(!(0x61<=c && c<=0x7a)) {
                // First character in the label is not an L.
                पटिनिवत्त मिच्छा;
            }
        } अञ्ञथा {
            सचे(c<=0x20 && (c>=0x1c || (9<=c && c<=0xd))) {
                // Intermediate character in the label is a B, S or WS.
                पटिनिवत्त मिच्छा;
            }
        }
    }
    पटिनिवत्त सच्च;
}

// UTF-8 version, called for source ASCII prefix.
// Can contain uppercase A-Z.
// s[length-1] must be the trailing dot.
स्थिर UBool
isASCIIOkBiDi(ध्रुव वर्ण *s, int32_t length) {
    int32_t labelStart=0;
    अत्थाय(int32_t i=0; i<length; ++i) {
        वर्ण c=s[i];
        सचे(c==0x2e) {  // dot
            सचे(i>labelStart) {
                c=s[i-1];
                सचे(!(0x61<=c && c<=0x7a) && !(0x41<=c && c<=0x5a) && !(0x30<=c && c<=0x39)) {
                    // Last character in the label is not an L or EN.
                    पटिनिवत्त मिच्छा;
                }
            }
            labelStart=i+1;
        } अञ्ञथा सचे(i==labelStart) {
            सचे(!(0x61<=c && c<=0x7a) && !(0x41<=c && c<=0x5a)) {
                // First character in the label is not an L.
                पटिनिवत्त मिच्छा;
            }
        } अञ्ञथा {
            सचे(c<=0x20 && (c>=0x1c || (9<=c && c<=0xd))) {
                // Intermediate character in the label is a B, S or WS.
                पटिनिवत्त मिच्छा;
            }
        }
    }
    पटिनिवत्त सच्च;
}

UBool
UTS46::isLabelOkContextJ(ध्रुव char16_t *label, int32_t labelLength) ध्रुव {
    // [IDNA2008-Tables]
    // 200C..200D  ; CONTEXTJ    # ZERO WIDTH NON-JOINER..ZERO WIDTH JOINER
    अत्थाय(int32_t i=0; i<labelLength; ++i) {
        सचे(label[i]==0x200c) {
            // Appendix A.1. ZERO WIDTH NON-JOINER
            // Rule Set:
            //  False;
            //  If Canonical_Combining_Class(Before(cp)) .eq.  Virama Then True;
            //  If RegExpMatch((Joining_Type:{L,D})(Joining_Type:T)*\u200C
            //     (Joining_Type:T)*(Joining_Type:{R,D})) Then True;
            सचे(i==0) {
                पटिनिवत्त मिच्छा;
            }
            UChar32 c;
            int32_t j=i;
            U16_PREV_UNSAFE(label, j, c);
            सचे(uts46Norm2.getCombiningClass(c)==9) {
                अनुवर्त;
            }
            // check precontext (Joining_Type:{L,D})(Joining_Type:T)*
            अत्थाय(;;) {
                UJoiningType type=ubidi_getJoiningType(c);
                सचे(type==U_JT_TRANSPARENT) {
                    सचे(j==0) {
                        पटिनिवत्त मिच्छा;
                    }
                    U16_PREV_UNSAFE(label, j, c);
                } अञ्ञथा सचे(type==U_JT_वामः_JOINING || type==U_JT_DUAL_JOINING) {
                    भङ्ग;  // precontext fulfilled
                } अञ्ञथा {
                    पटिनिवत्त मिच्छा;
                }
            }
            // check postcontext (Joining_Type:T)*(Joining_Type:{R,D})
            अत्थाय(j=i+1;;) {
                सचे(j==labelLength) {
                    पटिनिवत्त मिच्छा;
                }
                U16_NEXT_UNSAFE(label, j, c);
                UJoiningType type=ubidi_getJoiningType(c);
                सचे(type==U_JT_TRANSPARENT) {
                    // just skip this character
                } अञ्ञथा सचे(type==U_JT_दक्षिणः_JOINING || type==U_JT_DUAL_JOINING) {
                    भङ्ग;  // postcontext fulfilled
                } अञ्ञथा {
                    पटिनिवत्त मिच्छा;
                }
            }
        } अञ्ञथा सचे(label[i]==0x200d) {
            // Appendix A.2. ZERO WIDTH JOINER (U+200D)
            // Rule Set:
            //  False;
            //  If Canonical_Combining_Class(Before(cp)) .eq.  Virama Then True;
            सचे(i==0) {
                पटिनिवत्त मिच्छा;
            }
            UChar32 c;
            int32_t j=i;
            U16_PREV_UNSAFE(label, j, c);
            सचे(uts46Norm2.getCombiningClass(c)!=9) {
                पटिनिवत्त मिच्छा;
            }
        }
    }
    पटिनिवत्त सच्च;
}

शून्य
UTS46::checkLabelContextO(ध्रुव char16_t *label, int32_t labelLength, IDNAInfo &info) ध्रुव {
    int32_t labelEnd=labelLength-1;  // inclusive
    int32_t arabicDigits=0;  // -1 for 066x, +1 for 06Fx
    अत्थाय(int32_t i=0; i<=labelEnd; ++i) {
        UChar32 c=label[i];
        सचे(c<0xb7) {
            // ASCII fastpath
        } अञ्ञथा सचे(c<=0x6f9) {
            सचे(c==0xb7) {
                // Appendix A.3. MIDDLE DOT (U+00B7)
                // Rule Set:
                //  False;
                //  If Before(cp) .eq.  U+006C And
                //     After(cp) .eq.  U+006C Then True;
                सचे(!(0<i && label[i-1]==0x6c &&
                     i<labelEnd && label[i+1]==0x6c)) {
                    info.labelErrors|=UIDNA_ERROR_CONTEXTO_PUNCTUATION;
                }
            } अञ्ञथा सचे(c==0x375) {
                // Appendix A.4. GREEK LOWER NUMERAL SIGN (KERAIA) (U+0375)
                // Rule Set:
                //  False;
                //  If Script(After(cp)) .eq.  Greek Then True;
                UScriptCode script=USCRIPT_INVALID_CODE;
                सचे(i<labelEnd) {
                    UErrorCode errorCode=U_ZERO_ERROR;
                    int32_t j=i+1;
                    U16_NEXT(label, j, labelLength, c);
                    script=uscript_getScript(c, &errorCode);
                }
                सचे(script!=USCRIPT_GREEK) {
                    info.labelErrors|=UIDNA_ERROR_CONTEXTO_PUNCTUATION;
                }
            } अञ्ञथा सचे(c==0x5f3 || c==0x5f4) {
                // Appendix A.5. HEBREW PUNCTUATION GERESH (U+05F3)
                // Rule Set:
                //  False;
                //  If Script(Before(cp)) .eq.  Hebrew Then True;
                //
                // Appendix A.6. HEBREW PUNCTUATION GERSHAYIM (U+05F4)
                // Rule Set:
                //  False;
                //  If Script(Before(cp)) .eq.  Hebrew Then True;
                UScriptCode script=USCRIPT_INVALID_CODE;
                सचे(0<i) {
                    UErrorCode errorCode=U_ZERO_ERROR;
                    int32_t j=i;
                    U16_PREV(label, 0, j, c);
                    script=uscript_getScript(c, &errorCode);
                }
                सचे(script!=USCRIPT_HEBREW) {
                    info.labelErrors|=UIDNA_ERROR_CONTEXTO_PUNCTUATION;
                }
            } अञ्ञथा सचे(0x660<=c /* && c<=0x6f9 */) {
                // Appendix A.8. ARABIC-INDIC DIGITS (0660..0669)
                // Rule Set:
                //  True;
                //  For All Characters:
                //    If cp .in. 06F0..06F9 Then False;
                //  End For;
                //
                // Appendix A.9. EXTENDED ARABIC-INDIC DIGITS (06F0..06F9)
                // Rule Set:
                //  True;
                //  For All Characters:
                //    If cp .in. 0660..0669 Then False;
                //  End For;
                सचे(c<=0x669) {
                    सचे(arabicDigits>0) {
                        info.labelErrors|=UIDNA_ERROR_CONTEXTO_DIGITS;
                    }
                    arabicDigits=-1;
                } अञ्ञथा सचे(0x6f0<=c) {
                    सचे(arabicDigits<0) {
                        info.labelErrors|=UIDNA_ERROR_CONTEXTO_DIGITS;
                    }
                    arabicDigits=1;
                }
            }
        } अञ्ञथा सचे(c==0x30fb) {
            // Appendix A.7. KATAKANA MIDDLE DOT (U+30FB)
            // Rule Set:
            //  False;
            //  For All Characters:
            //    If Script(cp) .in. {Hiragana, Katakana, Han} Then True;
            //  End For;
            UErrorCode errorCode=U_ZERO_ERROR;
            अत्थाय(पूर्णाङ्क j=0;;) {
                सचे(j>labelEnd) {
                    info.labelErrors|=UIDNA_ERROR_CONTEXTO_PUNCTUATION;
                    भङ्ग;
                }
                U16_NEXT(label, j, labelLength, c);
                UScriptCode script=uscript_getScript(c, &errorCode);
                सचे(script==USCRIPT_HIRAGANA || script==USCRIPT_KATAKANA || script==USCRIPT_HAN) {
                    भङ्ग;
                }
            }
        }
    }
}

U_NAMESPACE_END

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

U_CAPI UIDNA * U_EXPORT2
uidna_openUTS46(uint32_t options, UErrorCode *pErrorCode) {
    पटिनिवत्त reinterpret_cast<UIDNA *>(IDNA::createUTS46Instance(options, *pErrorCode));
}

U_CAPI शून्य U_EXPORT2
uidna_close(UIDNA *idna) {
    delete reinterpret_cast<IDNA *>(idna);
}

स्थिर UBool
checkArgs(ध्रुव शून्य *label, int32_t length,
          शून्य *dest, int32_t capacity,
          UIDNAInfo *pInfo, UErrorCode *pErrorCode) {
    सचे(U_FAILURE(*pErrorCode)) {
        पटिनिवत्त मिच्छा;
    }
    // sizeof(UIDNAInfo)=16 in the first API version.
    सचे(pInfo==nullptr || pInfo->size<16) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        पटिनिवत्त मिच्छा;
    }
    सचे( (label==nullptr ? length!=0 : length<-1) ||
        (dest==nullptr ? capacity!=0 : capacity<0) ||
        (dest==label && label!=nullptr)
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        पटिनिवत्त मिच्छा;
    }
    // Set all *pInfo bytes to 0 except for the size field itself.
    uprv_memset(&pInfo->size+1, 0, pInfo->size-परिमाण(pInfo->size));
    पटिनिवत्त सच्च;
}

स्थिर शून्य
idnaInfoToStruct(IDNAInfo &info, UIDNAInfo *pInfo) {
    pInfo->isTransitionalDifferent=info.isTransitionalDifferent();
    pInfo->errors=info.getErrors();
}

U_CAPI int32_t U_EXPORT2
uidna_labelToASCII(ध्रुव UIDNA *idna,
                   ध्रुव char16_t *label, int32_t length,
                   char16_t *dest, int32_t capacity,
                   UIDNAInfo *pInfo, UErrorCode *pErrorCode) {
    सचे(!checkArgs(label, length, dest, capacity, pInfo, pErrorCode)) {
        पटिनिवत्त 0;
    }
    UnicodeString src(length < 0, label, length);
    UnicodeString destString(dest, 0, capacity);
    IDNAInfo info;
    reinterpret_cast<ध्रुव IDNA *>(idna)->labelToASCII(src, destString, info, *pErrorCode);
    idnaInfoToStruct(info, pInfo);
    पटिनिवत्त destString.extract(dest, capacity, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uidna_labelToUnicode(ध्रुव UIDNA *idna,
                     ध्रुव char16_t *label, int32_t length,
                     char16_t *dest, int32_t capacity,
                     UIDNAInfo *pInfo, UErrorCode *pErrorCode) {
    सचे(!checkArgs(label, length, dest, capacity, pInfo, pErrorCode)) {
        पटिनिवत्त 0;
    }
    UnicodeString src(length < 0, label, length);
    UnicodeString destString(dest, 0, capacity);
    IDNAInfo info;
    reinterpret_cast<ध्रुव IDNA *>(idna)->labelToUnicode(src, destString, info, *pErrorCode);
    idnaInfoToStruct(info, pInfo);
    पटिनिवत्त destString.extract(dest, capacity, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uidna_nameToASCII(ध्रुव UIDNA *idna,
                  ध्रुव char16_t *name, int32_t length,
                  char16_t *dest, int32_t capacity,
                  UIDNAInfo *pInfo, UErrorCode *pErrorCode) {
    सचे(!checkArgs(name, length, dest, capacity, pInfo, pErrorCode)) {
        पटिनिवत्त 0;
    }
    UnicodeString src(length < 0, name, length);
    UnicodeString destString(dest, 0, capacity);
    IDNAInfo info;
    reinterpret_cast<ध्रुव IDNA *>(idna)->nameToASCII(src, destString, info, *pErrorCode);
    idnaInfoToStruct(info, pInfo);
    पटिनिवत्त destString.extract(dest, capacity, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uidna_nameToUnicode(ध्रुव UIDNA *idna,
                    ध्रुव char16_t *name, int32_t length,
                    char16_t *dest, int32_t capacity,
                    UIDNAInfo *pInfo, UErrorCode *pErrorCode) {
    सचे(!checkArgs(name, length, dest, capacity, pInfo, pErrorCode)) {
        पटिनिवत्त 0;
    }
    UnicodeString src(length < 0, name, length);
    UnicodeString destString(dest, 0, capacity);
    IDNAInfo info;
    reinterpret_cast<ध्रुव IDNA *>(idna)->nameToUnicode(src, destString, info, *pErrorCode);
    idnaInfoToStruct(info, pInfo);
    पटिनिवत्त destString.extract(dest, capacity, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uidna_labelToASCII_UTF8(ध्रुव UIDNA *idna,
                        ध्रुव वर्ण *label, int32_t length,
                        वर्ण *dest, int32_t capacity,
                        UIDNAInfo *pInfo, UErrorCode *pErrorCode) {
    सचे(!checkArgs(label, length, dest, capacity, pInfo, pErrorCode)) {
        पटिनिवत्त 0;
    }
    StringPiece src(label, length<0 ? static_cast<int32_t>(uprv_strlen(label)) : length);
    पटिनिवत्त ByteSinkUtil::viaByteSinkToTerminatedChars(
        dest, capacity,
        [&](ByteSink& sink, UErrorCode& status) {
            IDNAInfo info;
            reinterpret_cast<ध्रुव IDNA *>(idna)->labelToASCII_UTF8(src, sink, info, status);
            idnaInfoToStruct(info, pInfo);
        },
        *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uidna_labelToUnicodeUTF8(ध्रुव UIDNA *idna,
                         ध्रुव वर्ण *label, int32_t length,
                         वर्ण *dest, int32_t capacity,
                         UIDNAInfo *pInfo, UErrorCode *pErrorCode) {
    सचे(!checkArgs(label, length, dest, capacity, pInfo, pErrorCode)) {
        पटिनिवत्त 0;
    }
    StringPiece src(label, length<0 ? static_cast<int32_t>(uprv_strlen(label)) : length);
    पटिनिवत्त ByteSinkUtil::viaByteSinkToTerminatedChars(
        dest, capacity,
        [&](ByteSink& sink, UErrorCode& status) {
            IDNAInfo info;
            reinterpret_cast<ध्रुव IDNA *>(idna)->labelToUnicodeUTF8(src, sink, info, status);
            idnaInfoToStruct(info, pInfo);
        },
        *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uidna_nameToASCII_UTF8(ध्रुव UIDNA *idna,
                       ध्रुव वर्ण *name, int32_t length,
                       वर्ण *dest, int32_t capacity,
                       UIDNAInfo *pInfo, UErrorCode *pErrorCode) {
    सचे(!checkArgs(name, length, dest, capacity, pInfo, pErrorCode)) {
        पटिनिवत्त 0;
    }
    StringPiece src(name, length<0 ? static_cast<int32_t>(uprv_strlen(name)) : length);
    पटिनिवत्त ByteSinkUtil::viaByteSinkToTerminatedChars(
        dest, capacity,
        [&](ByteSink& sink, UErrorCode& status) {
            IDNAInfo info;
            reinterpret_cast<ध्रुव IDNA *>(idna)->nameToASCII_UTF8(src, sink, info, status);
            idnaInfoToStruct(info, pInfo);
        },
        *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uidna_nameToUnicodeUTF8(ध्रुव UIDNA *idna,
                        ध्रुव वर्ण *name, int32_t length,
                        वर्ण *dest, int32_t capacity,
                        UIDNAInfo *pInfo, UErrorCode *pErrorCode) {
    सचे(!checkArgs(name, length, dest, capacity, pInfo, pErrorCode)) {
        पटिनिवत्त 0;
    }
    StringPiece src(name, length<0 ? static_cast<int32_t>(uprv_strlen(name)) : length);
    पटिनिवत्त ByteSinkUtil::viaByteSinkToTerminatedChars(
        dest, capacity,
        [&](ByteSink& sink, UErrorCode& status) {
            IDNAInfo info;
            reinterpret_cast<ध्रुव IDNA *>(idna)->nameToUnicodeUTF8(src, sink, info, status);
            idnaInfoToStruct(info, pInfo);
        },
        *pErrorCode);
}

#endif  // UCONFIG_NO_IDNA
