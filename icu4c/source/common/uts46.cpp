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

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಯದಿ !UCONFIG_NO_IDNA

#ಅನ್ತರ್ಭಾವ "unicode/bytestream.h"
#ಅನ್ತರ್ಭಾವ "unicode/idna.h"
#ಅನ್ತರ್ಭಾವ "unicode/normalizer2.h"
#ಅನ್ತರ್ಭಾವ "unicode/uscript.h"
#ಅನ್ತರ್ಭಾವ "unicode/ustring.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf16.h"
#ಅನ್ತರ್ಭಾವ "bytesinkutil.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"
#ಅನ್ತರ್ಭಾವ "punycode.h"
#ಅನ್ತರ್ಭಾವ "ubidi_props.h"

// Note about tests for UIDNA_ERROR_DOMAIN_NAME_TOO_LONG:
//
// The domain name length limit is 255 octets in an internal DNS representation
// where the last ("root") label is the empty label
// represented by length byte 0 alone.
// In a conventional string, this translates to 253 characters, or 254
// if there is a trailing dot for the root label.

U_NAMESPACE_BEGIN

// Severe errors which usually result in a U+FFFD replacement character in the result string.
ಧ್ರುವ uint32_t severeErrors=
    UIDNA_ERROR_LEADING_COMBINING_MARK|
    UIDNA_ERROR_DISALLOWED|
    UIDNA_ERROR_PUNYCODE|
    UIDNA_ERROR_LABEL_HAS_DOT|
    UIDNA_ERROR_INVALID_ACE_LABEL;

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ UBool
isASCIIString(ಧ್ರುವ UnicodeString &dest) {
    ಧ್ರುವ char16_t *s=dest.getBuffer();
    ಧ್ರುವ char16_t *limit=s+dest.length();
    ಯಾವತ್(s<limit) {
        ಯದಿ(*s++>0x7f) {
            ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
        }
    }
    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

ಸ್ಥಿರ UBool
isASCIIOkBiDi(ಧ್ರುವ char16_t *s, int32_t length);

ಸ್ಥಿರ UBool
isASCIIOkBiDi(ಧ್ರುವ ವರ್ಣ *s, int32_t length);

// IDNA class default implementations -------------------------------------- ***

IDNA::~IDNA() {}

ಶೂನ್ಯ
IDNA::labelToASCII_UTF8(StringPiece label, ByteSink &dest,
                        IDNAInfo &info, UErrorCode &errorCode) ಧ್ರುವ {
    ಯದಿ(U_SUCCESS(errorCode)) {
        UnicodeString destString;
        labelToASCII(UnicodeString::fromUTF8(label), destString,
                     info, errorCode).toUTF8(dest);
    }
}

ಶೂನ್ಯ
IDNA::labelToUnicodeUTF8(StringPiece label, ByteSink &dest,
                         IDNAInfo &info, UErrorCode &errorCode) ಧ್ರುವ {
    ಯದಿ(U_SUCCESS(errorCode)) {
        UnicodeString destString;
        labelToUnicode(UnicodeString::fromUTF8(label), destString,
                       info, errorCode).toUTF8(dest);
    }
}

ಶೂನ್ಯ
IDNA::nameToASCII_UTF8(StringPiece name, ByteSink &dest,
                       IDNAInfo &info, UErrorCode &errorCode) ಧ್ರುವ {
    ಯದಿ(U_SUCCESS(errorCode)) {
        UnicodeString destString;
        nameToASCII(UnicodeString::fromUTF8(name), destString,
                    info, errorCode).toUTF8(dest);
    }
}

ಶೂನ್ಯ
IDNA::nameToUnicodeUTF8(StringPiece name, ByteSink &dest,
                        IDNAInfo &info, UErrorCode &errorCode) ಧ್ರುವ {
    ಯದಿ(U_SUCCESS(errorCode)) {
        UnicodeString destString;
        nameToUnicode(UnicodeString::fromUTF8(name), destString,
                      info, errorCode).toUTF8(dest);
    }
}

// UTS46 class declaration ------------------------------------------------- ***

ವರ್ಗ UTS46 : ಸಾರ್ವಜನಿಕ IDNA {
ಸಾರ್ವಜನಿಕ:
    UTS46(uint32_t options, UErrorCode &errorCode);
    ಆಭಾಸ ~UTS46();

    ಆಭಾಸ UnicodeString&
    labelToASCII(ಧ್ರುವ UnicodeString& label,
                 UnicodeString& dest U_LIFETIME_BOUND,
                 IDNAInfo& info,
                 UErrorCode& errorCode) ಧ್ರುವ override;

    ಆಭಾಸ UnicodeString&
    labelToUnicode(ಧ್ರುವ UnicodeString& label,
                   UnicodeString& dest U_LIFETIME_BOUND,
                   IDNAInfo& info,
                   UErrorCode& errorCode) ಧ್ರುವ override;

    ಆಭಾಸ UnicodeString&
    nameToASCII(ಧ್ರುವ UnicodeString& name,
                UnicodeString& dest U_LIFETIME_BOUND,
                IDNAInfo& info,
                UErrorCode& errorCode) ಧ್ರುವ override;

    ಆಭಾಸ UnicodeString&
    nameToUnicode(ಧ್ರುವ UnicodeString& name,
                  UnicodeString& dest U_LIFETIME_BOUND,
                  IDNAInfo& info,
                  UErrorCode& errorCode) ಧ್ರುವ override;

    ಆಭಾಸ ಶೂನ್ಯ
    labelToASCII_UTF8(StringPiece label, ByteSink &dest,
                      IDNAInfo &info, UErrorCode &errorCode) ಧ್ರುವ override;

    ಆಭಾಸ ಶೂನ್ಯ
    labelToUnicodeUTF8(StringPiece label, ByteSink &dest,
                       IDNAInfo &info, UErrorCode &errorCode) ಧ್ರುವ override;

    ಆಭಾಸ ಶೂನ್ಯ
    nameToASCII_UTF8(StringPiece name, ByteSink &dest,
                     IDNAInfo &info, UErrorCode &errorCode) ಧ್ರುವ override;

    ಆಭಾಸ ಶೂನ್ಯ
    nameToUnicodeUTF8(StringPiece name, ByteSink &dest,
                      IDNAInfo &info, UErrorCode &errorCode) ಧ್ರುವ override;

ಗುಪ್ತ:
    UnicodeString&
    process(ಧ್ರುವ UnicodeString& src,
            UBool isLabel,
            UBool toASCII,
            UnicodeString& dest U_LIFETIME_BOUND,
            IDNAInfo& info,
            UErrorCode& errorCode) ಧ್ರುವ;

    ಶೂನ್ಯ
    processUTF8(StringPiece src,
                UBool isLabel, UBool toASCII,
                ByteSink &dest,
                IDNAInfo &info, UErrorCode &errorCode) ಧ್ರುವ;

    UnicodeString&
    processUnicode(ಧ್ರುವ UnicodeString& src,
                   int32_t labelStart,
                   int32_t mappingStart,
                   UBool isLabel,
                   UBool toASCII,
                   UnicodeString& dest U_LIFETIME_BOUND,
                   IDNAInfo& info,
                   UErrorCode& errorCode) ಧ್ರುವ;

    // returns the new dest.length()
    int32_t
    mapDevChars(UnicodeString &dest, int32_t labelStart, int32_t mappingStart,
                UErrorCode &errorCode) ಧ್ರುವ;

    // returns the new label length
    int32_t
    processLabel(UnicodeString &dest,
                 int32_t labelStart, int32_t labelLength,
                 UBool toASCII,
                 IDNAInfo &info, UErrorCode &errorCode) ಧ್ರುವ;
    int32_t
    markBadACELabel(UnicodeString &dest,
                    int32_t labelStart, int32_t labelLength,
                    UBool toASCII, IDNAInfo &info, UErrorCode &errorCode) ಧ್ರುವ;

    ಶೂನ್ಯ
    checkLabelBiDi(ಧ್ರುವ char16_t *label, int32_t labelLength, IDNAInfo &info) ಧ್ರುವ;

    UBool
    isLabelOkContextJ(ಧ್ರುವ char16_t *label, int32_t labelLength) ಧ್ರುವ;

    ಶೂನ್ಯ
    checkLabelContextO(ಧ್ರುವ char16_t *label, int32_t labelLength, IDNAInfo &info) ಧ್ರುವ;

    ಧ್ರುವ Normalizer2 &uts46Norm2;  // uts46.nrm
    uint32_t options;
};

IDNA *
IDNA::createUTS46Instance(uint32_t options, UErrorCode &errorCode) {
    ಯದಿ(U_SUCCESS(errorCode)) {
        IDNA *idna=ನೂತನ UTS46(options, errorCode);
        ಯದಿ(idna==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
        } ಅನ್ಯಥಾ ಯದಿ(U_FAILURE(errorCode)) {
            delete idna;
            idna=nullptr;
        }
        ಪ್ರತ್ಯಾಗಮ idna;
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
}

// UTS46 implementation ---------------------------------------------------- ***

UTS46::UTS46(uint32_t opt, UErrorCode &errorCode)
        : uts46Norm2(*Normalizer2::getInstance(nullptr, "uts46", UNORM2_COMPOSE, errorCode)),
          options(opt) {}

UTS46::~UTS46() {}

UnicodeString&
UTS46::labelToASCII(ಧ್ರುವ UnicodeString& label,
                    UnicodeString& dest U_LIFETIME_BOUND,
                    IDNAInfo& info,
                    UErrorCode& errorCode) ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ process(label, ಸತ್ಯ, ಸತ್ಯ, dest, info, errorCode);
}

UnicodeString&
UTS46::labelToUnicode(ಧ್ರುವ UnicodeString& label,
                      UnicodeString& dest U_LIFETIME_BOUND,
                      IDNAInfo& info,
                      UErrorCode& errorCode) ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ process(label, ಸತ್ಯ, ಅಸತ್ಯ, dest, info, errorCode);
}

UnicodeString&
UTS46::nameToASCII(ಧ್ರುವ UnicodeString& name,
                   UnicodeString& dest U_LIFETIME_BOUND,
                   IDNAInfo& info,
                   UErrorCode& errorCode) ಧ್ರುವ {
    process(name, ಅಸತ್ಯ, ಸತ್ಯ, dest, info, errorCode);
    ಯದಿ( dest.length()>=254 && (info.errors&UIDNA_ERROR_DOMAIN_NAME_TOO_LONG)==0 &&
        isASCIIString(dest) &&
        (dest.length()>254 || dest[253]!=0x2e)
    ) {
        info.errors|=UIDNA_ERROR_DOMAIN_NAME_TOO_LONG;
    }
    ಪ್ರತ್ಯಾಗಮ dest;
}

UnicodeString&
UTS46::nameToUnicode(ಧ್ರುವ UnicodeString& name,
                     UnicodeString& dest U_LIFETIME_BOUND,
                     IDNAInfo& info,
                     UErrorCode& errorCode) ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ process(name, ಅಸತ್ಯ, ಅಸತ್ಯ, dest, info, errorCode);
}

ಶೂನ್ಯ
UTS46::labelToASCII_UTF8(StringPiece label, ByteSink &dest,
                         IDNAInfo &info, UErrorCode &errorCode) ಧ್ರುವ {
    processUTF8(label, ಸತ್ಯ, ಸತ್ಯ, dest, info, errorCode);
}

ಶೂನ್ಯ
UTS46::labelToUnicodeUTF8(StringPiece label, ByteSink &dest,
                          IDNAInfo &info, UErrorCode &errorCode) ಧ್ರುವ {
    processUTF8(label, ಸತ್ಯ, ಅಸತ್ಯ, dest, info, errorCode);
}

ಶೂನ್ಯ
UTS46::nameToASCII_UTF8(StringPiece name, ByteSink &dest,
                        IDNAInfo &info, UErrorCode &errorCode) ಧ್ರುವ {
    processUTF8(name, ಅಸತ್ಯ, ಸತ್ಯ, dest, info, errorCode);
}

ಶೂನ್ಯ
UTS46::nameToUnicodeUTF8(StringPiece name, ByteSink &dest,
                         IDNAInfo &info, UErrorCode &errorCode) ಧ್ರುವ {
    processUTF8(name, ಅಸತ್ಯ, ಅಸತ್ಯ, dest, info, errorCode);
}

// UTS #46 data for ASCII characters.
// The normalizer (using uts46.nrm) maps uppercase ASCII letters to lowercase
// and passes through all other ASCII characters.
// If UIDNA_USE_STD3_RULES is set, then non-LDH characters are disallowed
// using this data.
// The ASCII fastpath also uses this data.
// Values: -1=disallowed  0==valid  1==mapped (lowercase)
ಸ್ಥಿರ ಧ್ರುವ int8_t asciiData[128]={
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
UTS46::process(ಧ್ರುವ UnicodeString& src,
               UBool isLabel,
               UBool toASCII,
               UnicodeString& dest U_LIFETIME_BOUND,
               IDNAInfo& info,
               UErrorCode& errorCode) ಧ್ರುವ {
    // uts46Norm2.normalize() would do all of this error checking and setup,
    // but with the ASCII fastpath we do not always call it, and do not
    // call it first.
    ಯದಿ(U_FAILURE(errorCode)) {
        dest.setToBogus();
        ಪ್ರತ್ಯಾಗಮ dest;
    }
    ಧ್ರುವ char16_t *srcArray=src.getBuffer();
    ಯದಿ(&dest==&src || srcArray==nullptr) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        dest.setToBogus();
        ಪ್ರತ್ಯಾಗಮ dest;
    }
    // Arguments are fine, reset output values.
    dest.remove();
    info.reset();
    int32_t srcLength=src.length();
    ಯದಿ(srcLength==0) {
        info.errors|=UIDNA_ERROR_EMPTY_LABEL;
        ಪ್ರತ್ಯಾಗಮ dest;
    }
    char16_t *destArray=dest.getBuffer(srcLength);
    ಯದಿ(destArray==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        ಪ್ರತ್ಯಾಗಮ dest;
    }
    // ASCII fastpath
    UBool disallowNonLDHDot=(options&UIDNA_USE_STD3_RULES)!=0;
    int32_t labelStart=0;
    int32_t i;
    ಪ್ರತಿ(i=0;; ++i) {
        ಯದಿ(i==srcLength) {
            ಯದಿ(toASCII) {
                ಯದಿ((i-labelStart)>63) {
                    info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
                }
                // There is a trailing dot if labelStart==i.
                ಯದಿ(!isLabel && i>=254 && (i>254 || labelStart<i)) {
                    info.errors|=UIDNA_ERROR_DOMAIN_NAME_TOO_LONG;
                }
            }
            info.errors|=info.labelErrors;
            dest.releaseBuffer(i);
            ಪ್ರತ್ಯಾಗಮ dest;
        }
        char16_t c=srcArray[i];
        ಯದಿ(c>0x7f) {
            ಭಙ್ಗ;
        }
        ಪೂರ್ಣಾಙ್ಕ cData=asciiData[c];
        ಯದಿ(cData>0) {
            destArray[i]=c+0x20;  // Lowercase an uppercase ASCII letter.
        } ಅನ್ಯಥಾ ಯದಿ(cData<0 && disallowNonLDHDot) {
            ಭಙ್ಗ;  // Replacing with U+FFFD can be complicated for toASCII.
        } ಅನ್ಯಥಾ {
            destArray[i]=c;
            ಯದಿ(c==0x2d) {  // hyphen
                ಯದಿ(i==(labelStart+3) && srcArray[i-1]==0x2d) {
                    // "??--..." is Punycode or forbidden.
                    ++i;  // '-' was copied to dest already
                    ಭಙ್ಗ;
                }
                ಯದಿ(i==labelStart) {
                    // label starts with "-"
                    info.labelErrors|=UIDNA_ERROR_LEADING_HYPHEN;
                }
                ಯದಿ((i+1)==srcLength || srcArray[i+1]==0x2e) {
                    // label ends with "-"
                    info.labelErrors|=UIDNA_ERROR_TRAILING_HYPHEN;
                }
            } ಅನ್ಯಥಾ ಯದಿ(c==0x2e) {  // dot
                ಯದಿ(isLabel) {
                    // Replacing with U+FFFD can be complicated for toASCII.
                    ++i;  // '.' was copied to dest already
                    ಭಙ್ಗ;
                }
                ಯದಿ(i==labelStart) {
                    info.labelErrors|=UIDNA_ERROR_EMPTY_LABEL;
                }
                ಯದಿ(toASCII && (i-labelStart)>63) {
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
    ಯದಿ( info.isBiDi && U_SUCCESS(errorCode) && (info.errors&severeErrors)==0 &&
        (!info.isOkBiDi || (labelStart>0 && !isASCIIOkBiDi(dest.getBuffer(), labelStart)))
    ) {
        info.errors|=UIDNA_ERROR_BIDI;
    }
    ಪ್ರತ್ಯಾಗಮ dest;
}

ಶೂನ್ಯ
UTS46::processUTF8(StringPiece src,
                   UBool isLabel, UBool toASCII,
                   ByteSink &dest,
                   IDNAInfo &info, UErrorCode &errorCode) ಧ್ರುವ {
    ಯದಿ(U_FAILURE(errorCode)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    ಧ್ರುವ ವರ್ಣ *srcArray=src.data();
    int32_t srcLength=src.length();
    ಯದಿ(srcArray==nullptr && srcLength!=0) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ;
    }
    // Arguments are fine, reset output values.
    info.reset();
    ಯದಿ(srcLength==0) {
        info.errors|=UIDNA_ERROR_EMPTY_LABEL;
        dest.Flush();
        ಪ್ರತ್ಯಾಗಮ;
    }
    UnicodeString destString;
    int32_t labelStart=0;
    ಯದಿ(srcLength<=256) {  // length of stackArray[]
        // ASCII fastpath
        ವರ್ಣ ರಾಶಿ_Array[256];
        int32_t destCapacity;
        ವರ್ಣ *destArray=dest.GetAppendBuffer(srcLength, srcLength+20,
                                             ರಾಶಿ_Array, UPRV_LENGTHOF(ರಾಶಿ_Array), &destCapacity);
        UBool disallowNonLDHDot=(options&UIDNA_USE_STD3_RULES)!=0;
        int32_t i;
        ಪ್ರತಿ(i=0;; ++i) {
            ಯದಿ(i==srcLength) {
                ಯದಿ(toASCII) {
                    ಯದಿ((i-labelStart)>63) {
                        info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
                    }
                    // There is a trailing dot if labelStart==i.
                    ಯದಿ(!isLabel && i>=254 && (i>254 || labelStart<i)) {
                        info.errors|=UIDNA_ERROR_DOMAIN_NAME_TOO_LONG;
                    }
                }
                info.errors|=info.labelErrors;
                dest.Append(destArray, i);
                dest.Flush();
                ಪ್ರತ್ಯಾಗಮ;
            }
            ವರ್ಣ c=srcArray[i];
            ಯದಿ (static_cast<int8_t>(c) < 0) { // (uint8_t)c>0x7f
                ಭಙ್ಗ;
            }
            ಪೂರ್ಣಾಙ್ಕ cData = asciiData[static_cast<ಪೂರ್ಣಾಙ್ಕ>(c)]; // Cast: gcc warns about indexing with a char.
            ಯದಿ(cData>0) {
                destArray[i]=c+0x20;  // Lowercase an uppercase ASCII letter.
            } ಅನ್ಯಥಾ ಯದಿ(cData<0 && disallowNonLDHDot) {
                ಭಙ್ಗ;  // Replacing with U+FFFD can be complicated for toASCII.
            } ಅನ್ಯಥಾ {
                destArray[i]=c;
                ಯದಿ(c==0x2d) {  // hyphen
                    ಯದಿ(i==(labelStart+3) && srcArray[i-1]==0x2d) {
                        // "??--..." is Punycode or forbidden.
                        ಭಙ್ಗ;
                    }
                    ಯದಿ(i==labelStart) {
                        // label starts with "-"
                        info.labelErrors|=UIDNA_ERROR_LEADING_HYPHEN;
                    }
                    ಯದಿ((i+1)==srcLength || srcArray[i+1]==0x2e) {
                        // label ends with "-"
                        info.labelErrors|=UIDNA_ERROR_TRAILING_HYPHEN;
                    }
                } ಅನ್ಯಥಾ ಯದಿ(c==0x2e) {  // dot
                    ಯದಿ(isLabel) {
                        ಭಙ್ಗ;  // Replacing with U+FFFD can be complicated for toASCII.
                    }
                    ಯದಿ(i==labelStart) {
                        info.labelErrors|=UIDNA_ERROR_EMPTY_LABEL;
                    }
                    ಯದಿ(toASCII && (i-labelStart)>63) {
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
    } ಅನ್ಯಥಾ {
        // src is too long for the ASCII fastpath implementation.
        processUnicode(UnicodeString::fromUTF8(src), 0, 0,
                       isLabel, toASCII,
                       destString, info, errorCode);
    }
    destString.toUTF8(dest);  // calls dest.Flush()
    ಯದಿ(toASCII && !isLabel) {
        // length==labelStart==254 means that there is a trailing dot (ok) and
        // destString is empty (do not index at 253-labelStart).
        int32_t length=labelStart+destString.length();
        ಯದಿ( length>=254 && isASCIIString(destString) &&
            (length>254 ||
             (labelStart<254 && destString[253-labelStart]!=0x2e))
        ) {
            info.errors|=UIDNA_ERROR_DOMAIN_NAME_TOO_LONG;
        }
    }
    ಯದಿ( info.isBiDi && U_SUCCESS(errorCode) && (info.errors&severeErrors)==0 &&
        (!info.isOkBiDi || (labelStart>0 && !isASCIIOkBiDi(srcArray, labelStart)))
    ) {
        info.errors|=UIDNA_ERROR_BIDI;
    }
}

UnicodeString&
UTS46::processUnicode(ಧ್ರುವ UnicodeString& src,
                      int32_t labelStart,
                      int32_t mappingStart,
                      UBool isLabel,
                      UBool toASCII,
                      UnicodeString& dest U_LIFETIME_BOUND,
                      IDNAInfo& info,
                      UErrorCode& errorCode) ಧ್ರುವ {
    ಯದಿ(mappingStart==0) {
        uts46Norm2.normalize(src, dest, errorCode);
    } ಅನ್ಯಥಾ {
        uts46Norm2.normalizeSecondAndAppend(dest, src.tempSubString(mappingStart), errorCode);
    }
    ಯದಿ(U_FAILURE(errorCode)) {
        ಪ್ರತ್ಯಾಗಮ dest;
    }
    UBool doMapDevChars=
        toASCII ? (options&UIDNA_NONTRANSITIONAL_TO_ASCII)==0 :
                  (options&UIDNA_NONTRANSITIONAL_TO_UNICODE)==0;
    ಧ್ರುವ char16_t *destArray=dest.getBuffer();
    int32_t destLength=dest.length();
    int32_t labelLimit=labelStart;
    ಯಾವತ್(labelLimit<destLength) {
        char16_t c=destArray[labelLimit];
        ಯದಿ(c==0x2e && !isLabel) {
            int32_t labelLength=labelLimit-labelStart;
            int32_t newLength=processLabel(dest, labelStart, labelLength,
                                            toASCII, info, errorCode);
            info.errors|=info.labelErrors;
            info.labelErrors=0;
            ಯದಿ(U_FAILURE(errorCode)) {
                ಪ್ರತ್ಯಾಗಮ dest;
            }
            destArray=dest.getBuffer();
            destLength+=newLength-labelLength;
            labelLimit=labelStart+=newLength+1;
            ಅನುವರ್ತ;
        } ಅನ್ಯಥಾ ಯದಿ(c<0xdf) {
            // pass
        } ಅನ್ಯಥಾ ಯದಿ(c<=0x200d && (c==0xdf || c==0x3c2 || c>=0x200c)) {
            info.isTransDiff=ಸತ್ಯ;
            ಯದಿ(doMapDevChars) {
                destLength=mapDevChars(dest, labelStart, labelLimit, errorCode);
                ಯದಿ(U_FAILURE(errorCode)) {
                    ಪ್ರತ್ಯಾಗಮ dest;
                }
                destArray=dest.getBuffer();
                // All deviation characters have been mapped, no need to check for them again.
                doMapDevChars=ಅಸತ್ಯ;
                // Do not increment labelLimit in case c was removed.
                ಅನುವರ್ತ;
            }
        } ಅನ್ಯಥಾ ಯದಿ(U16_IS_SURROGATE(c)) {
            ಯದಿ(U16_IS_SURROGATE_LEAD(c) ?
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
    ಯದಿ(0==labelStart || labelStart<labelLimit) {
        processLabel(dest, labelStart, labelLimit-labelStart,
                      toASCII, info, errorCode);
        info.errors|=info.labelErrors;
    }
    ಪ್ರತ್ಯಾಗಮ dest;
}

int32_t
UTS46::mapDevChars(UnicodeString &dest, int32_t labelStart, int32_t mappingStart,
                   UErrorCode &errorCode) ಧ್ರುವ {
    ಯದಿ(U_FAILURE(errorCode)) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    int32_t length=dest.length();
    char16_t *s=dest.getBuffer(dest[mappingStart]==0xdf ? length+1 : length);
    ಯದಿ(s==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        ಪ್ರತ್ಯಾಗಮ length;
    }
    int32_t capacity=dest.getCapacity();
    UBool didMapDevChars=ಅಸತ್ಯ;
    int32_t readIndex=mappingStart, writeIndex=mappingStart;
    ಕುರು {
        char16_t c=s[readIndex++];
        ವಿಕಲ್ಪ(c) {
        ಪಕ್ಷ 0xdf:
            // Map sharp s to ss.
            didMapDevChars=ಸತ್ಯ;
            s[writeIndex++]=0x73;  // Replace sharp s with first s.
            // Insert second s and account for possible buffer reallocation.
            ಯದಿ(writeIndex==readIndex) {
                ಯದಿ(length==capacity) {
                    dest.releaseBuffer(length);
                    s=dest.getBuffer(length+1);
                    ಯದಿ(s==nullptr) {
                        errorCode=U_MEMORY_ALLOCATION_ERROR;
                        ಪ್ರತ್ಯಾಗಮ length;
                    }
                    capacity=dest.getCapacity();
                }
                u_memmove(s+writeIndex+1, s+writeIndex, length-writeIndex);
                ++readIndex;
            }
            s[writeIndex++]=0x73;
            ++length;
            ಭಙ್ಗ;
        ಪಕ್ಷ 0x3c2:  // Map final sigma to nonfinal sigma.
            didMapDevChars=ಸತ್ಯ;
            s[writeIndex++]=0x3c3;
            ಭಙ್ಗ;
        ಪಕ್ಷ 0x200c:  // Ignore/remove ZWNJ.
        ಪಕ್ಷ 0x200d:  // Ignore/remove ZWJ.
            didMapDevChars=ಸತ್ಯ;
            --length;
            ಭಙ್ಗ;
        ಸ್ವಭಾವ:
            // Only really necessary if writeIndex was different from readIndex.
            s[writeIndex++]=c;
            ಭಙ್ಗ;
        }
    } ಯಾವತ್(writeIndex<length);
    dest.releaseBuffer(length);
    ಯದಿ(didMapDevChars) {
        // Mapping deviation characters might have resulted in an un-NFC string.
        // We could use either the NFC or the UTS #46 normalizer.
        // By using the UTS #46 normalizer again, we avoid having to load a second .nrm data file.
        UnicodeString normalized;
        uts46Norm2.normalize(dest.tempSubString(labelStart), normalized, errorCode);
        ಯದಿ(U_SUCCESS(errorCode)) {
            dest.replace(labelStart, 0x7fffffff, normalized);
            ಯದಿ(dest.isBogus()) {
                errorCode=U_MEMORY_ALLOCATION_ERROR;
            }
            ಪ್ರತ್ಯಾಗಮ dest.length();
        }
    }
    ಪ್ರತ್ಯಾಗಮ length;
}

// Replace the label in dest with the label string, if the label was modified.
// If &label==&dest then the label was modified in-place and labelLength
// is the new label length, different from label.length().
// If &label!=&dest then labelLength==label.length().
// Returns labelLength (= the new label length).
ಸ್ಥಿರ int32_t
replaceLabel(UnicodeString &dest, int32_t destLabelStart, int32_t destLabelLength,
             ಧ್ರುವ UnicodeString &label, int32_t labelLength, UErrorCode &errorCode) {
    ಯದಿ(U_FAILURE(errorCode)) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಯದಿ(&label!=&dest) {
        dest.replace(destLabelStart, destLabelLength, label);
        ಯದಿ(dest.isBogus()) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            ಪ್ರತ್ಯಾಗಮ 0;
        }
    }
    ಪ್ರತ್ಯಾಗಮ labelLength;
}

int32_t
UTS46::processLabel(UnicodeString &dest,
                    int32_t labelStart, int32_t labelLength,
                    UBool toASCII,
                    IDNAInfo &info, UErrorCode &errorCode) ಧ್ರುವ {
    ಯದಿ(U_FAILURE(errorCode)) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    UnicodeString fromPunycode;
    UnicodeString *labelString;
    ಧ್ರುವ char16_t *label=dest.getBuffer()+labelStart;
    int32_t destLabelStart=labelStart;
    int32_t destLabelLength=labelLength;
    UBool wasPunycode;
    ಯದಿ(labelLength>=4 && label[0]==0x78 && label[1]==0x6e && label[2]==0x2d && label[3]==0x2d) {
        // Label starts with "xn--", try to un-Punycode it.
        // In IDNA2008, labels like "xn--" (decodes to an empty string) and
        // "xn--ASCII-" (decodes to just "ASCII") fail the round-trip validation from
        // comparing the ToUnicode input with the back-to-ToASCII output.
        // They are alternate encodings of the respective ASCII labels.
        // Ignore "xn---" here: It will fail Punycode.decode() which logically comes before
        // the round-trip verification.
        ಯದಿ(labelLength==4 || (labelLength>5 && label[labelLength-1]==u'-')) {
            info.labelErrors|=UIDNA_ERROR_INVALID_ACE_LABEL;
            ಪ್ರತ್ಯಾಗಮ markBadACELabel(dest, labelStart, labelLength, toASCII, info, errorCode);
        }
        wasPunycode=ಸತ್ಯ;
        char16_t *unicodeBuffer=fromPunycode.getBuffer(-1);  // capacity==-1: most labels should fit
        ಯದಿ(unicodeBuffer==nullptr) {
            // Should never occur if we used capacity==-1 which uses the internal buffer.
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            ಪ್ರತ್ಯಾಗಮ labelLength;
        }
        UErrorCode punycodeErrorCode=U_ZERO_ERROR;
        int32_t unicodeLength=u_strFromPunycode(label+4, labelLength-4,
                                                unicodeBuffer, fromPunycode.getCapacity(),
                                                nullptr, &punycodeErrorCode);
        ಯದಿ(punycodeErrorCode==U_BUFFER_OVERFLOW_ERROR) {
            fromPunycode.releaseBuffer(0);
            unicodeBuffer=fromPunycode.getBuffer(unicodeLength);
            ಯದಿ(unicodeBuffer==nullptr) {
                errorCode=U_MEMORY_ALLOCATION_ERROR;
                ಪ್ರತ್ಯಾಗಮ labelLength;
            }
            punycodeErrorCode=U_ZERO_ERROR;
            unicodeLength=u_strFromPunycode(label+4, labelLength-4,
                                            unicodeBuffer, fromPunycode.getCapacity(),
                                            nullptr, &punycodeErrorCode);
        }
        fromPunycode.releaseBuffer(unicodeLength);
        ಯದಿ(U_FAILURE(punycodeErrorCode)) {
            info.labelErrors|=UIDNA_ERROR_PUNYCODE;
            ಪ್ರತ್ಯಾಗಮ markBadACELabel(dest, labelStart, labelLength, toASCII, info, errorCode);
        }
        // Check for NFC, and for characters that are not
        // valid or deviation characters according to the normalizer.
        // If there is something wrong, then the string will change.
        // Note that the normalizer passes through non-LDH ASCII and deviation characters.
        // Deviation characters are ok in Punycode even in transitional processing.
        // In the code further below, if we find non-LDH ASCII and we have UIDNA_USE_STD3_RULES
        // then we will set UIDNA_ERROR_INVALID_ACE_LABEL there too.
        UBool isValid=uts46Norm2.isNormalized(fromPunycode, errorCode);
        ಯದಿ(U_FAILURE(errorCode)) {
            ಪ್ರತ್ಯಾಗಮ labelLength;
        }
        // Unicode 15.1 UTS #46:
        // Added an additional condition in 4.1 Validity Criteria to
        // disallow labels such as xn--xn---epa., which do not round-trip.
        // --> Validity Criteria new criterion 4:
        // If not CheckHyphens, the label must not begin with “xn--”.
        ಯದಿ(!isValid || fromPunycode.startsWith(UnicodeString::readOnlyAlias(u"xn--"))) {
            info.labelErrors|=UIDNA_ERROR_INVALID_ACE_LABEL;
            ಪ್ರತ್ಯಾಗಮ markBadACELabel(dest, labelStart, labelLength, toASCII, info, errorCode);
        }
        labelString=&fromPunycode;
        label=fromPunycode.getBuffer();
        labelStart=0;
        labelLength=fromPunycode.length();
    } ಅನ್ಯಥಾ {
        wasPunycode=ಅಸತ್ಯ;
        labelString=&dest;
    }
    // Validity check
    ಯದಿ(labelLength==0) {
        info.labelErrors|=UIDNA_ERROR_EMPTY_LABEL;
        ಪ್ರತ್ಯಾಗಮ replaceLabel(dest, destLabelStart, destLabelLength,
                            *labelString, labelLength, errorCode);
    }
    // labelLength>0
    ಯದಿ(labelLength>=4 && label[2]==0x2d && label[3]==0x2d) {
        // label starts with "??--"
        info.labelErrors|=UIDNA_ERROR_HYPHEN_3_4;
    }
    ಯದಿ(label[0]==0x2d) {
        // label starts with "-"
        info.labelErrors|=UIDNA_ERROR_LEADING_HYPHEN;
    }
    ಯದಿ(label[labelLength-1]==0x2d) {
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
    ಧ್ರುವ char16_t *limit=label+labelLength;
    char16_t oredChars=0;
    // If we enforce STD3 rules, then ASCII characters other than LDH and dot are disallowed.
    UBool disallowNonLDHDot=(options&UIDNA_USE_STD3_RULES)!=0;
    ಕುರು {
        char16_t c=*s;
        ಯದಿ(c<=0x7f) {
            ಯದಿ(c==0x2e) {
                info.labelErrors|=UIDNA_ERROR_LABEL_HAS_DOT;
                *s=0xfffd;
            } ಅನ್ಯಥಾ ಯದಿ(disallowNonLDHDot && asciiData[c]<0) {
                info.labelErrors|=UIDNA_ERROR_DISALLOWED;
                *s=0xfffd;
            }
        } ಅನ್ಯಥಾ {
            oredChars|=c;
            ಯದಿ(c==0xfffd) {
                info.labelErrors|=UIDNA_ERROR_DISALLOWED;
            }
        }
        ++s;
    } ಯಾವತ್(s<limit);
    // Check for a leading combining mark after other validity checks
    // so that we don't report UIDNA_ERROR_DISALLOWED for the U+FFFD from here.
    UChar32 c;
    int32_t cpLength=0;
    // "Unsafe" is ok because unpaired surrogates were mapped to U+FFFD.
    U16_NEXT_UNSAFE(label, cpLength, c);
    ಯದಿ((U_GET_GC_MASK(c)&U_GC_M_MASK)!=0) {
        info.labelErrors|=UIDNA_ERROR_LEADING_COMBINING_MARK;
        labelString->replace(labelStart, cpLength, static_cast<char16_t>(0xfffd));
        label=labelString->getBuffer()+labelStart;
        labelLength+=1-cpLength;
        ಯದಿ(labelString==&dest) {
            destLabelLength=labelLength;
        }
    }
    ಯದಿ((info.labelErrors&severeErrors)==0) {
        // Do contextual checks only if we do not have U+FFFD from a severe error
        // because U+FFFD can make these checks fail.
        ಯದಿ((options&UIDNA_CHECK_BIDI)!=0 && (!info.isBiDi || info.isOkBiDi)) {
            checkLabelBiDi(label, labelLength, info);
        }
        ಯದಿ( (options&UIDNA_CHECK_CONTEXTJ)!=0 && (oredChars&0x200c)==0x200c &&
            !isLabelOkContextJ(label, labelLength)
        ) {
            info.labelErrors|=UIDNA_ERROR_CONTEXTJ;
        }
        ಯದಿ((options&UIDNA_CHECK_CONTEXTO)!=0 && oredChars>=0xb7) {
            checkLabelContextO(label, labelLength, info);
        }
        ಯದಿ(toASCII) {
            ಯದಿ(wasPunycode) {
                // Leave a Punycode label unchanged if it has no severe errors.
                ಯದಿ(destLabelLength>63) {
                    info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
                }
                ಪ್ರತ್ಯಾಗಮ destLabelLength;
            } ಅನ್ಯಥಾ ಯದಿ(oredChars>=0x80) {
                // Contains non-ASCII characters.
                UnicodeString punycode;
                char16_t *buffer=punycode.getBuffer(63);  // 63==maximum DNS label length
                ಯದಿ(buffer==nullptr) {
                    errorCode=U_MEMORY_ALLOCATION_ERROR;
                    ಪ್ರತ್ಯಾಗಮ destLabelLength;
                }
                buffer[0]=0x78;  // Write "xn--".
                buffer[1]=0x6e;
                buffer[2]=0x2d;
                buffer[3]=0x2d;
                UErrorCode punycodeErrorCode=U_ZERO_ERROR;
                int32_t punycodeLength=u_strToPunycode(label, labelLength,
                                                      buffer+4, punycode.getCapacity()-4,
                                                      nullptr, &punycodeErrorCode);
                ಯದಿ(punycodeErrorCode==U_BUFFER_OVERFLOW_ERROR) {
                    punycodeErrorCode=U_ZERO_ERROR;
                    punycode.releaseBuffer(4);
                    buffer=punycode.getBuffer(4+punycodeLength);
                    ಯದಿ(buffer==nullptr) {
                        errorCode=U_MEMORY_ALLOCATION_ERROR;
                        ಪ್ರತ್ಯಾಗಮ destLabelLength;
                    }
                    punycodeLength=u_strToPunycode(label, labelLength,
                                                  buffer+4, punycode.getCapacity()-4,
                                                  nullptr, &punycodeErrorCode);
                }
                punycodeLength+=4;
                punycode.releaseBuffer(punycodeLength);
                ಯದಿ(U_FAILURE(punycodeErrorCode)) {
                    errorCode = punycodeErrorCode;
                    ಪ್ರತ್ಯಾಗಮ destLabelLength;
                }
                ಯದಿ(punycodeLength>63) {
                    info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
                }
                ಪ್ರತ್ಯಾಗಮ replaceLabel(dest, destLabelStart, destLabelLength,
                                    punycode, punycodeLength, errorCode);
            } ಅನ್ಯಥಾ {
                // all-ASCII label
                ಯದಿ(labelLength>63) {
                    info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
                }
            }
        }
    } ಅನ್ಯಥಾ {
        // If a Punycode label has severe errors,
        // then leave it but make sure it does not look valid.
        ಯದಿ(wasPunycode) {
            info.labelErrors|=UIDNA_ERROR_INVALID_ACE_LABEL;
            ಪ್ರತ್ಯಾಗಮ markBadACELabel(dest, destLabelStart, destLabelLength, toASCII, info, errorCode);
        }
    }
    ಪ್ರತ್ಯಾಗಮ replaceLabel(dest, destLabelStart, destLabelLength,
                        *labelString, labelLength, errorCode);
}

// Make sure an ACE label does not look valid.
// Append U+FFFD if the label has only LDH characters.
// If UIDNA_USE_STD3_RULES, also replace disallowed ASCII characters with U+FFFD.
int32_t
UTS46::markBadACELabel(UnicodeString &dest,
                       int32_t labelStart, int32_t labelLength,
                       UBool toASCII, IDNAInfo &info, UErrorCode &errorCode) ಧ್ರುವ {
    ಯದಿ(U_FAILURE(errorCode)) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    UBool disallowNonLDHDot=(options&UIDNA_USE_STD3_RULES)!=0;
    UBool isASCII=ಸತ್ಯ;
    UBool onlyLDH=ಸತ್ಯ;
    ಧ್ರುವ char16_t *label=dest.getBuffer()+labelStart;
    ಧ್ರುವ char16_t *limit=label+labelLength;
    // Start after the initial "xn--".
    // Ok to cast away const because we own the UnicodeString.
    ಪ್ರತಿ(char16_t *s=const_cast<char16_t *>(label+4); s<limit; ++s) {
        char16_t c=*s;
        ಯದಿ(c<=0x7f) {
            ಯದಿ(c==0x2e) {
                info.labelErrors|=UIDNA_ERROR_LABEL_HAS_DOT;
                *s=0xfffd;
                isASCII=onlyLDH=ಅಸತ್ಯ;
            } ಅನ್ಯಥಾ ಯದಿ(asciiData[c]<0) {
                onlyLDH=ಅಸತ್ಯ;
                ಯದಿ(disallowNonLDHDot) {
                    *s=0xfffd;
                    isASCII=ಅಸತ್ಯ;
                }
            }
        } ಅನ್ಯಥಾ {
            isASCII=onlyLDH=ಅಸತ್ಯ;
        }
    }
    ಯದಿ(onlyLDH) {
        dest.insert(labelStart + labelLength, static_cast<char16_t>(0xfffd));
        ಯದಿ(dest.isBogus()) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            ಪ್ರತ್ಯಾಗಮ 0;
        }
        ++labelLength;
    } ಅನ್ಯಥಾ {
        ಯದಿ(toASCII && isASCII && labelLength>63) {
            info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
        }
    }
    ಪ್ರತ್ಯಾಗಮ labelLength;
}

ಧ್ರುವ uint32_t L_MASK=U_MASK(U_ಎಡ_TO_ಬಲ);
ಧ್ರುವ uint32_t R_AL_MASK=U_MASK(U_ಬಲ_TO_ಎಡ)|U_MASK(U_ಬಲ_TO_ಎಡ_ARABIC);
ಧ್ರುವ uint32_t L_R_AL_MASK=L_MASK|R_AL_MASK;

ಧ್ರುವ uint32_t R_AL_AN_MASK=R_AL_MASK|U_MASK(U_ARABIC_ಸಂಖ್ಯೆ);

ಧ್ರುವ uint32_t EN_AN_MASK=U_MASK(U_EUROPEAN_ಸಂಖ್ಯೆ)|U_MASK(U_ARABIC_ಸಂಖ್ಯೆ);
ಧ್ರುವ uint32_t R_AL_EN_AN_MASK=R_AL_MASK|EN_AN_MASK;
ಧ್ರುವ uint32_t L_EN_MASK=L_MASK|U_MASK(U_EUROPEAN_ಸಂಖ್ಯೆ);

ಧ್ರುವ uint32_t ES_CS_ET_ON_BN_NSM_MASK=
    U_MASK(U_EUROPEAN_ಸಂಖ್ಯೆ_SEPARATOR)|
    U_MASK(U_COMMON_ಸಂಖ್ಯೆ_SEPARATOR)|
    U_MASK(U_EUROPEAN_ಸಂಖ್ಯೆ_TERMINATOR)|
    U_MASK(U_OTHER_NEUTRAL)|
    U_MASK(U_BOUNDARY_NEUTRAL)|
    U_MASK(U_DIR_NON_SPACING_MARK);
ಧ್ರುವ uint32_t L_EN_ES_CS_ET_ON_BN_NSM_MASK=L_EN_MASK|ES_CS_ET_ON_BN_NSM_MASK;
ಧ್ರುವ uint32_t R_AL_AN_EN_ES_CS_ET_ON_BN_NSM_MASK=R_AL_MASK|EN_AN_MASK|ES_CS_ET_ON_BN_NSM_MASK;

// We scan the whole label and check both for whether it contains RTL characters
// and whether it passes the BiDi Rule.
// In a BiDi domain name, all labels must pass the BiDi Rule, but we might find
// that a domain name is a BiDi domain name (has an RTL label) only after
// processing several earlier labels.
ಶೂನ್ಯ
UTS46::checkLabelBiDi(ಧ್ರುವ char16_t *label, int32_t labelLength, IDNAInfo &info) ಧ್ರುವ {
    // IDNA2008 BiDi rule
    // Get the directionality of the first character.
    UChar32 c;
    int32_t i=0;
    U16_NEXT_UNSAFE(label, i, c);
    uint32_t firstMask=U_MASK(u_charDirection(c));
    // 1. The first character must be a character with BIDI property L, R
    // or AL.  If it has the R or AL property, it is an RTL label; if it
    // has the L property, it is an LTR label.
    ಯದಿ((firstMask&~L_R_AL_MASK)!=0) {
        info.isOkBiDi=ಅಸತ್ಯ;
    }
    // Get the directionality of the last non-NSM character.
    uint32_t lastMask;
    ಪ್ರತಿ(;;) {
        ಯದಿ(i>=labelLength) {
            lastMask=firstMask;
            ಭಙ್ಗ;
        }
        U16_PREV_UNSAFE(label, labelLength, c);
        UCharDirection dir=u_charDirection(c);
        ಯದಿ(dir!=U_DIR_NON_SPACING_MARK) {
            lastMask=U_MASK(dir);
            ಭಙ್ಗ;
        }
    }
    // 3. In an RTL label, the end of the label must be a character with
    // BIDI property R, AL, EN or AN, followed by zero or more
    // characters with BIDI property NSM.
    // 6. In an LTR label, the end of the label must be a character with
    // BIDI property L or EN, followed by zero or more characters with
    // BIDI property NSM.
    ಯದಿ( (firstMask&L_MASK)!=0 ?
            (lastMask&~L_EN_MASK)!=0 :
            (lastMask&~R_AL_EN_AN_MASK)!=0
    ) {
        info.isOkBiDi=ಅಸತ್ಯ;
    }
    // Add the directionalities of the intervening characters.
    uint32_t mask=firstMask|lastMask;
    ಯಾವತ್(i<labelLength) {
        U16_NEXT_UNSAFE(label, i, c);
        mask|=U_MASK(u_charDirection(c));
    }
    ಯದಿ(firstMask&L_MASK) {
        // 5. In an LTR label, only characters with the BIDI properties L, EN,
        // ES, CS, ET, ON, BN and NSM are allowed.
        ಯದಿ((mask&~L_EN_ES_CS_ET_ON_BN_NSM_MASK)!=0) {
            info.isOkBiDi=ಅಸತ್ಯ;
        }
    } ಅನ್ಯಥಾ {
        // 2. In an RTL label, only characters with the BIDI properties R, AL,
        // AN, EN, ES, CS, ET, ON, BN and NSM are allowed.
        ಯದಿ((mask&~R_AL_AN_EN_ES_CS_ET_ON_BN_NSM_MASK)!=0) {
            info.isOkBiDi=ಅಸತ್ಯ;
        }
        // 4. In an RTL label, if an EN is present, no AN may be present, and
        // vice versa.
        ಯದಿ((mask&EN_AN_MASK)==EN_AN_MASK) {
            info.isOkBiDi=ಅಸತ್ಯ;
        }
    }
    // An RTL label is a label that contains at least one character of type
    // R, AL or AN. [...]
    // A "BIDI domain name" is a domain name that contains at least one RTL
    // label. [...]
    // The following rule, consisting of six conditions, applies to labels
    // in BIDI domain names.
    ಯದಿ((mask&R_AL_AN_MASK)!=0) {
        info.isBiDi=ಸತ್ಯ;
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
ಸ್ಥಿರ UBool
isASCIIOkBiDi(ಧ್ರುವ char16_t *s, int32_t length) {
    int32_t labelStart=0;
    ಪ್ರತಿ(int32_t i=0; i<length; ++i) {
        char16_t c=s[i];
        ಯದಿ(c==0x2e) {  // dot
            ಯದಿ(i>labelStart) {
                c=s[i-1];
                ಯದಿ(!(0x61<=c && c<=0x7a) && !(0x30<=c && c<=0x39)) {
                    // Last character in the label is not an L or EN.
                    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
                }
            }
            labelStart=i+1;
        } ಅನ್ಯಥಾ ಯದಿ(i==labelStart) {
            ಯದಿ(!(0x61<=c && c<=0x7a)) {
                // First character in the label is not an L.
                ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
            }
        } ಅನ್ಯಥಾ {
            ಯದಿ(c<=0x20 && (c>=0x1c || (9<=c && c<=0xd))) {
                // Intermediate character in the label is a B, S or WS.
                ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
            }
        }
    }
    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

// UTF-8 version, called for source ASCII prefix.
// Can contain uppercase A-Z.
// s[length-1] must be the trailing dot.
ಸ್ಥಿರ UBool
isASCIIOkBiDi(ಧ್ರುವ ವರ್ಣ *s, int32_t length) {
    int32_t labelStart=0;
    ಪ್ರತಿ(int32_t i=0; i<length; ++i) {
        ವರ್ಣ c=s[i];
        ಯದಿ(c==0x2e) {  // dot
            ಯದಿ(i>labelStart) {
                c=s[i-1];
                ಯದಿ(!(0x61<=c && c<=0x7a) && !(0x41<=c && c<=0x5a) && !(0x30<=c && c<=0x39)) {
                    // Last character in the label is not an L or EN.
                    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
                }
            }
            labelStart=i+1;
        } ಅನ್ಯಥಾ ಯದಿ(i==labelStart) {
            ಯದಿ(!(0x61<=c && c<=0x7a) && !(0x41<=c && c<=0x5a)) {
                // First character in the label is not an L.
                ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
            }
        } ಅನ್ಯಥಾ {
            ಯದಿ(c<=0x20 && (c>=0x1c || (9<=c && c<=0xd))) {
                // Intermediate character in the label is a B, S or WS.
                ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
            }
        }
    }
    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

UBool
UTS46::isLabelOkContextJ(ಧ್ರುವ char16_t *label, int32_t labelLength) ಧ್ರುವ {
    // [IDNA2008-Tables]
    // 200C..200D  ; CONTEXTJ    # ZERO WIDTH NON-JOINER..ZERO WIDTH JOINER
    ಪ್ರತಿ(int32_t i=0; i<labelLength; ++i) {
        ಯದಿ(label[i]==0x200c) {
            // Appendix A.1. ZERO WIDTH NON-JOINER
            // Rule Set:
            //  False;
            //  If Canonical_Combining_Class(Before(cp)) .eq.  Virama Then True;
            //  If RegExpMatch((Joining_Type:{L,D})(Joining_Type:T)*\u200C
            //     (Joining_Type:T)*(Joining_Type:{R,D})) Then True;
            ಯದಿ(i==0) {
                ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
            }
            UChar32 c;
            int32_t j=i;
            U16_PREV_UNSAFE(label, j, c);
            ಯದಿ(uts46Norm2.getCombiningClass(c)==9) {
                ಅನುವರ್ತ;
            }
            // check precontext (Joining_Type:{L,D})(Joining_Type:T)*
            ಪ್ರತಿ(;;) {
                UJoiningType type=ubidi_getJoiningType(c);
                ಯದಿ(type==U_JT_TRANSPARENT) {
                    ಯದಿ(j==0) {
                        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
                    }
                    U16_PREV_UNSAFE(label, j, c);
                } ಅನ್ಯಥಾ ಯದಿ(type==U_JT_ಎಡ_JOINING || type==U_JT_DUAL_JOINING) {
                    ಭಙ್ಗ;  // precontext fulfilled
                } ಅನ್ಯಥಾ {
                    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
                }
            }
            // check postcontext (Joining_Type:T)*(Joining_Type:{R,D})
            ಪ್ರತಿ(j=i+1;;) {
                ಯದಿ(j==labelLength) {
                    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
                }
                U16_NEXT_UNSAFE(label, j, c);
                UJoiningType type=ubidi_getJoiningType(c);
                ಯದಿ(type==U_JT_TRANSPARENT) {
                    // just skip this character
                } ಅನ್ಯಥಾ ಯದಿ(type==U_JT_ಬಲ_JOINING || type==U_JT_DUAL_JOINING) {
                    ಭಙ್ಗ;  // postcontext fulfilled
                } ಅನ್ಯಥಾ {
                    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
                }
            }
        } ಅನ್ಯಥಾ ಯದಿ(label[i]==0x200d) {
            // Appendix A.2. ZERO WIDTH JOINER (U+200D)
            // Rule Set:
            //  False;
            //  If Canonical_Combining_Class(Before(cp)) .eq.  Virama Then True;
            ಯದಿ(i==0) {
                ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
            }
            UChar32 c;
            int32_t j=i;
            U16_PREV_UNSAFE(label, j, c);
            ಯದಿ(uts46Norm2.getCombiningClass(c)!=9) {
                ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
            }
        }
    }
    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

ಶೂನ್ಯ
UTS46::checkLabelContextO(ಧ್ರುವ char16_t *label, int32_t labelLength, IDNAInfo &info) ಧ್ರುವ {
    int32_t labelEnd=labelLength-1;  // inclusive
    int32_t arabicDigits=0;  // -1 for 066x, +1 for 06Fx
    ಪ್ರತಿ(int32_t i=0; i<=labelEnd; ++i) {
        UChar32 c=label[i];
        ಯದಿ(c<0xb7) {
            // ASCII fastpath
        } ಅನ್ಯಥಾ ಯದಿ(c<=0x6f9) {
            ಯದಿ(c==0xb7) {
                // Appendix A.3. MIDDLE DOT (U+00B7)
                // Rule Set:
                //  False;
                //  If Before(cp) .eq.  U+006C And
                //     After(cp) .eq.  U+006C Then True;
                ಯದಿ(!(0<i && label[i-1]==0x6c &&
                     i<labelEnd && label[i+1]==0x6c)) {
                    info.labelErrors|=UIDNA_ERROR_CONTEXTO_PUNCTUATION;
                }
            } ಅನ್ಯಥಾ ಯದಿ(c==0x375) {
                // Appendix A.4. GREEK LOWER NUMERAL SIGN (KERAIA) (U+0375)
                // Rule Set:
                //  False;
                //  If Script(After(cp)) .eq.  Greek Then True;
                UScriptCode script=USCRIPT_INVALID_CODE;
                ಯದಿ(i<labelEnd) {
                    UErrorCode errorCode=U_ZERO_ERROR;
                    int32_t j=i+1;
                    U16_NEXT(label, j, labelLength, c);
                    script=uscript_getScript(c, &errorCode);
                }
                ಯದಿ(script!=USCRIPT_GREEK) {
                    info.labelErrors|=UIDNA_ERROR_CONTEXTO_PUNCTUATION;
                }
            } ಅನ್ಯಥಾ ಯದಿ(c==0x5f3 || c==0x5f4) {
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
                ಯದಿ(0<i) {
                    UErrorCode errorCode=U_ZERO_ERROR;
                    int32_t j=i;
                    U16_PREV(label, 0, j, c);
                    script=uscript_getScript(c, &errorCode);
                }
                ಯದಿ(script!=USCRIPT_HEBREW) {
                    info.labelErrors|=UIDNA_ERROR_CONTEXTO_PUNCTUATION;
                }
            } ಅನ್ಯಥಾ ಯದಿ(0x660<=c /* && c<=0x6f9 */) {
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
                ಯದಿ(c<=0x669) {
                    ಯದಿ(arabicDigits>0) {
                        info.labelErrors|=UIDNA_ERROR_CONTEXTO_DIGITS;
                    }
                    arabicDigits=-1;
                } ಅನ್ಯಥಾ ಯದಿ(0x6f0<=c) {
                    ಯದಿ(arabicDigits<0) {
                        info.labelErrors|=UIDNA_ERROR_CONTEXTO_DIGITS;
                    }
                    arabicDigits=1;
                }
            }
        } ಅನ್ಯಥಾ ಯದಿ(c==0x30fb) {
            // Appendix A.7. KATAKANA MIDDLE DOT (U+30FB)
            // Rule Set:
            //  False;
            //  For All Characters:
            //    If Script(cp) .in. {Hiragana, Katakana, Han} Then True;
            //  End For;
            UErrorCode errorCode=U_ZERO_ERROR;
            ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ j=0;;) {
                ಯದಿ(j>labelEnd) {
                    info.labelErrors|=UIDNA_ERROR_CONTEXTO_PUNCTUATION;
                    ಭಙ್ಗ;
                }
                U16_NEXT(label, j, labelLength, c);
                UScriptCode script=uscript_getScript(c, &errorCode);
                ಯದಿ(script==USCRIPT_HIRAGANA || script==USCRIPT_KATAKANA || script==USCRIPT_HAN) {
                    ಭಙ್ಗ;
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
    ಪ್ರತ್ಯಾಗಮ reinterpret_cast<UIDNA *>(IDNA::createUTS46Instance(options, *pErrorCode));
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
uidna_close(UIDNA *idna) {
    delete reinterpret_cast<IDNA *>(idna);
}

ಸ್ಥಿರ UBool
checkArgs(ಧ್ರುವ ಶೂನ್ಯ *label, int32_t length,
          ಶೂನ್ಯ *dest, int32_t capacity,
          UIDNAInfo *pInfo, UErrorCode *pErrorCode) {
    ಯದಿ(U_FAILURE(*pErrorCode)) {
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    // sizeof(UIDNAInfo)=16 in the first API version.
    ಯದಿ(pInfo==nullptr || pInfo->size<16) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    ಯದಿ( (label==nullptr ? length!=0 : length<-1) ||
        (dest==nullptr ? capacity!=0 : capacity<0) ||
        (dest==label && label!=nullptr)
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    // Set all *pInfo bytes to 0 except for the size field itself.
    uprv_memset(&pInfo->size+1, 0, pInfo->size-ಪರಿಮಾಣ(pInfo->size));
    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

ಸ್ಥಿರ ಶೂನ್ಯ
idnaInfoToStruct(IDNAInfo &info, UIDNAInfo *pInfo) {
    pInfo->isTransitionalDifferent=info.isTransitionalDifferent();
    pInfo->errors=info.getErrors();
}

U_CAPI int32_t U_EXPORT2
uidna_labelToASCII(ಧ್ರುವ UIDNA *idna,
                   ಧ್ರುವ char16_t *label, int32_t length,
                   char16_t *dest, int32_t capacity,
                   UIDNAInfo *pInfo, UErrorCode *pErrorCode) {
    ಯದಿ(!checkArgs(label, length, dest, capacity, pInfo, pErrorCode)) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    UnicodeString src(length < 0, label, length);
    UnicodeString destString(dest, 0, capacity);
    IDNAInfo info;
    reinterpret_cast<ಧ್ರುವ IDNA *>(idna)->labelToASCII(src, destString, info, *pErrorCode);
    idnaInfoToStruct(info, pInfo);
    ಪ್ರತ್ಯಾಗಮ destString.extract(dest, capacity, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uidna_labelToUnicode(ಧ್ರುವ UIDNA *idna,
                     ಧ್ರುವ char16_t *label, int32_t length,
                     char16_t *dest, int32_t capacity,
                     UIDNAInfo *pInfo, UErrorCode *pErrorCode) {
    ಯದಿ(!checkArgs(label, length, dest, capacity, pInfo, pErrorCode)) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    UnicodeString src(length < 0, label, length);
    UnicodeString destString(dest, 0, capacity);
    IDNAInfo info;
    reinterpret_cast<ಧ್ರುವ IDNA *>(idna)->labelToUnicode(src, destString, info, *pErrorCode);
    idnaInfoToStruct(info, pInfo);
    ಪ್ರತ್ಯಾಗಮ destString.extract(dest, capacity, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uidna_nameToASCII(ಧ್ರುವ UIDNA *idna,
                  ಧ್ರುವ char16_t *name, int32_t length,
                  char16_t *dest, int32_t capacity,
                  UIDNAInfo *pInfo, UErrorCode *pErrorCode) {
    ಯದಿ(!checkArgs(name, length, dest, capacity, pInfo, pErrorCode)) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    UnicodeString src(length < 0, name, length);
    UnicodeString destString(dest, 0, capacity);
    IDNAInfo info;
    reinterpret_cast<ಧ್ರುವ IDNA *>(idna)->nameToASCII(src, destString, info, *pErrorCode);
    idnaInfoToStruct(info, pInfo);
    ಪ್ರತ್ಯಾಗಮ destString.extract(dest, capacity, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uidna_nameToUnicode(ಧ್ರುವ UIDNA *idna,
                    ಧ್ರುವ char16_t *name, int32_t length,
                    char16_t *dest, int32_t capacity,
                    UIDNAInfo *pInfo, UErrorCode *pErrorCode) {
    ಯದಿ(!checkArgs(name, length, dest, capacity, pInfo, pErrorCode)) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    UnicodeString src(length < 0, name, length);
    UnicodeString destString(dest, 0, capacity);
    IDNAInfo info;
    reinterpret_cast<ಧ್ರುವ IDNA *>(idna)->nameToUnicode(src, destString, info, *pErrorCode);
    idnaInfoToStruct(info, pInfo);
    ಪ್ರತ್ಯಾಗಮ destString.extract(dest, capacity, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uidna_labelToASCII_UTF8(ಧ್ರುವ UIDNA *idna,
                        ಧ್ರುವ ವರ್ಣ *label, int32_t length,
                        ವರ್ಣ *dest, int32_t capacity,
                        UIDNAInfo *pInfo, UErrorCode *pErrorCode) {
    ಯದಿ(!checkArgs(label, length, dest, capacity, pInfo, pErrorCode)) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    StringPiece src(label, length<0 ? static_cast<int32_t>(uprv_strlen(label)) : length);
    ಪ್ರತ್ಯಾಗಮ ByteSinkUtil::viaByteSinkToTerminatedChars(
        dest, capacity,
        [&](ByteSink& sink, UErrorCode& status) {
            IDNAInfo info;
            reinterpret_cast<ಧ್ರುವ IDNA *>(idna)->labelToASCII_UTF8(src, sink, info, status);
            idnaInfoToStruct(info, pInfo);
        },
        *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uidna_labelToUnicodeUTF8(ಧ್ರುವ UIDNA *idna,
                         ಧ್ರುವ ವರ್ಣ *label, int32_t length,
                         ವರ್ಣ *dest, int32_t capacity,
                         UIDNAInfo *pInfo, UErrorCode *pErrorCode) {
    ಯದಿ(!checkArgs(label, length, dest, capacity, pInfo, pErrorCode)) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    StringPiece src(label, length<0 ? static_cast<int32_t>(uprv_strlen(label)) : length);
    ಪ್ರತ್ಯಾಗಮ ByteSinkUtil::viaByteSinkToTerminatedChars(
        dest, capacity,
        [&](ByteSink& sink, UErrorCode& status) {
            IDNAInfo info;
            reinterpret_cast<ಧ್ರುವ IDNA *>(idna)->labelToUnicodeUTF8(src, sink, info, status);
            idnaInfoToStruct(info, pInfo);
        },
        *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uidna_nameToASCII_UTF8(ಧ್ರುವ UIDNA *idna,
                       ಧ್ರುವ ವರ್ಣ *name, int32_t length,
                       ವರ್ಣ *dest, int32_t capacity,
                       UIDNAInfo *pInfo, UErrorCode *pErrorCode) {
    ಯದಿ(!checkArgs(name, length, dest, capacity, pInfo, pErrorCode)) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    StringPiece src(name, length<0 ? static_cast<int32_t>(uprv_strlen(name)) : length);
    ಪ್ರತ್ಯಾಗಮ ByteSinkUtil::viaByteSinkToTerminatedChars(
        dest, capacity,
        [&](ByteSink& sink, UErrorCode& status) {
            IDNAInfo info;
            reinterpret_cast<ಧ್ರುವ IDNA *>(idna)->nameToASCII_UTF8(src, sink, info, status);
            idnaInfoToStruct(info, pInfo);
        },
        *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uidna_nameToUnicodeUTF8(ಧ್ರುವ UIDNA *idna,
                        ಧ್ರುವ ವರ್ಣ *name, int32_t length,
                        ವರ್ಣ *dest, int32_t capacity,
                        UIDNAInfo *pInfo, UErrorCode *pErrorCode) {
    ಯದಿ(!checkArgs(name, length, dest, capacity, pInfo, pErrorCode)) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    StringPiece src(name, length<0 ? static_cast<int32_t>(uprv_strlen(name)) : length);
    ಪ್ರತ್ಯಾಗಮ ByteSinkUtil::viaByteSinkToTerminatedChars(
        dest, capacity,
        [&](ByteSink& sink, UErrorCode& status) {
            IDNAInfo info;
            reinterpret_cast<ಧ್ರುವ IDNA *>(idna)->nameToUnicodeUTF8(src, sink, info, status);
            idnaInfoToStruct(info, pInfo);
        },
        *pErrorCode);
}

#endif  // UCONFIG_NO_IDNA
