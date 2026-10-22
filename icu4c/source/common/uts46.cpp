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

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"

#ଯଦି !UCONFIG_NO_IDNA

#ଅନ୍ତର୍ଭାଵ "unicode/bytestream.h"
#ଅନ୍ତର୍ଭାଵ "unicode/idna.h"
#ଅନ୍ତର୍ଭାଵ "unicode/normalizer2.h"
#ଅନ୍ତର୍ଭାଵ "unicode/uscript.h"
#ଅନ୍ତର୍ଭାଵ "unicode/ustring.h"
#ଅନ୍ତର୍ଭାଵ "unicode/utf16.h"
#ଅନ୍ତର୍ଭାଵ "bytesinkutil.h"
#ଅନ୍ତର୍ଭାଵ "cmemory.h"
#ଅନ୍ତର୍ଭାଵ "cstring.h"
#ଅନ୍ତର୍ଭାଵ "punycode.h"
#ଅନ୍ତର୍ଭାଵ "ubidi_props.h"

// Note about tests for UIDNA_ERROR_DOMAIN_NAME_TOO_LONG:
//
// The domain name length limit is 255 octets in an internal DNS representation
// where the last ("root") label is the empty label
// represented by length byte 0 alone.
// In a conventional string, this translates to 253 characters, or 254
// if there is a trailing dot for the root label.

U_NAMESPACE_BEGIN

// Severe errors which usually result in a U+FFFD replacement character in the result string.
ଅଚଳ uint32_t severeErrors=
    UIDNA_ERROR_LEADING_COMBINING_MARK|
    UIDNA_ERROR_DISALLOWED|
    UIDNA_ERROR_PUNYCODE|
    UIDNA_ERROR_LABEL_HAS_DOT|
    UIDNA_ERROR_INVALID_ACE_LABEL;

ସ୍ଥିର ଧାଡ଼ିଭିତରେ UBool
isASCIIString(ଅଚଳ UnicodeString &dest) {
    ଅଚଳ char16_t *s=dest.getBuffer();
    ଅଚଳ char16_t *limit=s+dest.length();
    ଯେତେବେଳେ(s<limit) {
        ଯଦି(*s++>0x7f) {
            ଫେରନ୍ତୁ ମିଥ୍ୟା;
        }
    }
    ଫେରନ୍ତୁ ସତ୍ୟ;
}

ସ୍ଥିର UBool
isASCIIOkBiDi(ଅଚଳ char16_t *s, int32_t length);

ସ୍ଥିର UBool
isASCIIOkBiDi(ଅଚଳ ଅକ୍ଷର *s, int32_t length);

// IDNA class default implementations -------------------------------------- ***

IDNA::~IDNA() {}

ଶୂନ୍ୟ
IDNA::labelToASCII_UTF8(StringPiece label, ByteSink &dest,
                        IDNAInfo &info, UErrorCode &errorCode) ଅଚଳ {
    ଯଦି(U_SUCCESS(errorCode)) {
        UnicodeString destString;
        labelToASCII(UnicodeString::fromUTF8(label), destString,
                     info, errorCode).toUTF8(dest);
    }
}

ଶୂନ୍ୟ
IDNA::labelToUnicodeUTF8(StringPiece label, ByteSink &dest,
                         IDNAInfo &info, UErrorCode &errorCode) ଅଚଳ {
    ଯଦି(U_SUCCESS(errorCode)) {
        UnicodeString destString;
        labelToUnicode(UnicodeString::fromUTF8(label), destString,
                       info, errorCode).toUTF8(dest);
    }
}

ଶୂନ୍ୟ
IDNA::nameToASCII_UTF8(StringPiece name, ByteSink &dest,
                       IDNAInfo &info, UErrorCode &errorCode) ଅଚଳ {
    ଯଦି(U_SUCCESS(errorCode)) {
        UnicodeString destString;
        nameToASCII(UnicodeString::fromUTF8(name), destString,
                    info, errorCode).toUTF8(dest);
    }
}

ଶୂନ୍ୟ
IDNA::nameToUnicodeUTF8(StringPiece name, ByteSink &dest,
                        IDNAInfo &info, UErrorCode &errorCode) ଅଚଳ {
    ଯଦି(U_SUCCESS(errorCode)) {
        UnicodeString destString;
        nameToUnicode(UnicodeString::fromUTF8(name), destString,
                      info, errorCode).toUTF8(dest);
    }
}

// UTS46 class declaration ------------------------------------------------- ***

ଶ୍ରେଣୀ UTS46 : ସର୍ବସାଧାରଣ IDNA {
ସର୍ବସାଧାରଣ:
    UTS46(uint32_t options, UErrorCode &errorCode);
    ଆଭାସୀ ~UTS46();

    ଆଭାସୀ UnicodeString&
    labelToASCII(ଅଚଳ UnicodeString& label,
                 UnicodeString& dest U_LIFETIME_BOUND,
                 IDNAInfo& info,
                 UErrorCode& errorCode) ଅଚଳ override;

    ଆଭାସୀ UnicodeString&
    labelToUnicode(ଅଚଳ UnicodeString& label,
                   UnicodeString& dest U_LIFETIME_BOUND,
                   IDNAInfo& info,
                   UErrorCode& errorCode) ଅଚଳ override;

    ଆଭାସୀ UnicodeString&
    nameToASCII(ଅଚଳ UnicodeString& name,
                UnicodeString& dest U_LIFETIME_BOUND,
                IDNAInfo& info,
                UErrorCode& errorCode) ଅଚଳ override;

    ଆଭାସୀ UnicodeString&
    nameToUnicode(ଅଚଳ UnicodeString& name,
                  UnicodeString& dest U_LIFETIME_BOUND,
                  IDNAInfo& info,
                  UErrorCode& errorCode) ଅଚଳ override;

    ଆଭାସୀ ଶୂନ୍ୟ
    labelToASCII_UTF8(StringPiece label, ByteSink &dest,
                      IDNAInfo &info, UErrorCode &errorCode) ଅଚଳ override;

    ଆଭାସୀ ଶୂନ୍ୟ
    labelToUnicodeUTF8(StringPiece label, ByteSink &dest,
                       IDNAInfo &info, UErrorCode &errorCode) ଅଚଳ override;

    ଆଭାସୀ ଶୂନ୍ୟ
    nameToASCII_UTF8(StringPiece name, ByteSink &dest,
                     IDNAInfo &info, UErrorCode &errorCode) ଅଚଳ override;

    ଆଭାସୀ ଶୂନ୍ୟ
    nameToUnicodeUTF8(StringPiece name, ByteSink &dest,
                      IDNAInfo &info, UErrorCode &errorCode) ଅଚଳ override;

ବ୍ୟକ୍ତିଗତ:
    UnicodeString&
    process(ଅଚଳ UnicodeString& src,
            UBool isLabel,
            UBool toASCII,
            UnicodeString& dest U_LIFETIME_BOUND,
            IDNAInfo& info,
            UErrorCode& errorCode) ଅଚଳ;

    ଶୂନ୍ୟ
    processUTF8(StringPiece src,
                UBool isLabel, UBool toASCII,
                ByteSink &dest,
                IDNAInfo &info, UErrorCode &errorCode) ଅଚଳ;

    UnicodeString&
    processUnicode(ଅଚଳ UnicodeString& src,
                   int32_t labelStart,
                   int32_t mappingStart,
                   UBool isLabel,
                   UBool toASCII,
                   UnicodeString& dest U_LIFETIME_BOUND,
                   IDNAInfo& info,
                   UErrorCode& errorCode) ଅଚଳ;

    // returns the new dest.length()
    int32_t
    mapDevChars(UnicodeString &dest, int32_t labelStart, int32_t mappingStart,
                UErrorCode &errorCode) ଅଚଳ;

    // returns the new label length
    int32_t
    processLabel(UnicodeString &dest,
                 int32_t labelStart, int32_t labelLength,
                 UBool toASCII,
                 IDNAInfo &info, UErrorCode &errorCode) ଅଚଳ;
    int32_t
    markBadACELabel(UnicodeString &dest,
                    int32_t labelStart, int32_t labelLength,
                    UBool toASCII, IDNAInfo &info, UErrorCode &errorCode) ଅଚଳ;

    ଶୂନ୍ୟ
    checkLabelBiDi(ଅଚଳ char16_t *label, int32_t labelLength, IDNAInfo &info) ଅଚଳ;

    UBool
    isLabelOkContextJ(ଅଚଳ char16_t *label, int32_t labelLength) ଅଚଳ;

    ଶୂନ୍ୟ
    checkLabelContextO(ଅଚଳ char16_t *label, int32_t labelLength, IDNAInfo &info) ଅଚଳ;

    ଅଚଳ Normalizer2 &uts46Norm2;  // uts46.nrm
    uint32_t options;
};

IDNA *
IDNA::createUTS46Instance(uint32_t options, UErrorCode &errorCode) {
    ଯଦି(U_SUCCESS(errorCode)) {
        IDNA *idna=ନୂତନ UTS46(options, errorCode);
        ଯଦି(idna==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
        } ଅନ୍ୟଥା ଯଦି(U_FAILURE(errorCode)) {
            delete idna;
            idna=nullptr;
        }
        ଫେରନ୍ତୁ idna;
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ nullptr;
    }
}

// UTS46 implementation ---------------------------------------------------- ***

UTS46::UTS46(uint32_t opt, UErrorCode &errorCode)
        : uts46Norm2(*Normalizer2::getInstance(nullptr, "uts46", UNORM2_COMPOSE, errorCode)),
          options(opt) {}

UTS46::~UTS46() {}

UnicodeString&
UTS46::labelToASCII(ଅଚଳ UnicodeString& label,
                    UnicodeString& dest U_LIFETIME_BOUND,
                    IDNAInfo& info,
                    UErrorCode& errorCode) ଅଚଳ {
    ଫେରନ୍ତୁ process(label, ସତ୍ୟ, ସତ୍ୟ, dest, info, errorCode);
}

UnicodeString&
UTS46::labelToUnicode(ଅଚଳ UnicodeString& label,
                      UnicodeString& dest U_LIFETIME_BOUND,
                      IDNAInfo& info,
                      UErrorCode& errorCode) ଅଚଳ {
    ଫେରନ୍ତୁ process(label, ସତ୍ୟ, ମିଥ୍ୟା, dest, info, errorCode);
}

UnicodeString&
UTS46::nameToASCII(ଅଚଳ UnicodeString& name,
                   UnicodeString& dest U_LIFETIME_BOUND,
                   IDNAInfo& info,
                   UErrorCode& errorCode) ଅଚଳ {
    process(name, ମିଥ୍ୟା, ସତ୍ୟ, dest, info, errorCode);
    ଯଦି( dest.length()>=254 && (info.errors&UIDNA_ERROR_DOMAIN_NAME_TOO_LONG)==0 &&
        isASCIIString(dest) &&
        (dest.length()>254 || dest[253]!=0x2e)
    ) {
        info.errors|=UIDNA_ERROR_DOMAIN_NAME_TOO_LONG;
    }
    ଫେରନ୍ତୁ dest;
}

UnicodeString&
UTS46::nameToUnicode(ଅଚଳ UnicodeString& name,
                     UnicodeString& dest U_LIFETIME_BOUND,
                     IDNAInfo& info,
                     UErrorCode& errorCode) ଅଚଳ {
    ଫେରନ୍ତୁ process(name, ମିଥ୍ୟା, ମିଥ୍ୟା, dest, info, errorCode);
}

ଶୂନ୍ୟ
UTS46::labelToASCII_UTF8(StringPiece label, ByteSink &dest,
                         IDNAInfo &info, UErrorCode &errorCode) ଅଚଳ {
    processUTF8(label, ସତ୍ୟ, ସତ୍ୟ, dest, info, errorCode);
}

ଶୂନ୍ୟ
UTS46::labelToUnicodeUTF8(StringPiece label, ByteSink &dest,
                          IDNAInfo &info, UErrorCode &errorCode) ଅଚଳ {
    processUTF8(label, ସତ୍ୟ, ମିଥ୍ୟା, dest, info, errorCode);
}

ଶୂନ୍ୟ
UTS46::nameToASCII_UTF8(StringPiece name, ByteSink &dest,
                        IDNAInfo &info, UErrorCode &errorCode) ଅଚଳ {
    processUTF8(name, ମିଥ୍ୟା, ସତ୍ୟ, dest, info, errorCode);
}

ଶୂନ୍ୟ
UTS46::nameToUnicodeUTF8(StringPiece name, ByteSink &dest,
                         IDNAInfo &info, UErrorCode &errorCode) ଅଚଳ {
    processUTF8(name, ମିଥ୍ୟା, ମିଥ୍ୟା, dest, info, errorCode);
}

// UTS #46 data for ASCII characters.
// The normalizer (using uts46.nrm) maps uppercase ASCII letters to lowercase
// and passes through all other ASCII characters.
// If UIDNA_USE_STD3_RULES is set, then non-LDH characters are disallowed
// using this data.
// The ASCII fastpath also uses this data.
// Values: -1=disallowed  0==valid  1==mapped (lowercase)
ସ୍ଥିର ଅଚଳ int8_t asciiData[128]={
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
UTS46::process(ଅଚଳ UnicodeString& src,
               UBool isLabel,
               UBool toASCII,
               UnicodeString& dest U_LIFETIME_BOUND,
               IDNAInfo& info,
               UErrorCode& errorCode) ଅଚଳ {
    // uts46Norm2.normalize() would do all of this error checking and setup,
    // but with the ASCII fastpath we do not always call it, and do not
    // call it first.
    ଯଦି(U_FAILURE(errorCode)) {
        dest.setToBogus();
        ଫେରନ୍ତୁ dest;
    }
    ଅଚଳ char16_t *srcArray=src.getBuffer();
    ଯଦି(&dest==&src || srcArray==nullptr) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        dest.setToBogus();
        ଫେରନ୍ତୁ dest;
    }
    // Arguments are fine, reset output values.
    dest.remove();
    info.reset();
    int32_t srcLength=src.length();
    ଯଦି(srcLength==0) {
        info.errors|=UIDNA_ERROR_EMPTY_LABEL;
        ଫେରନ୍ତୁ dest;
    }
    char16_t *destArray=dest.getBuffer(srcLength);
    ଯଦି(destArray==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        ଫେରନ୍ତୁ dest;
    }
    // ASCII fastpath
    UBool disallowNonLDHDot=(options&UIDNA_USE_STD3_RULES)!=0;
    int32_t labelStart=0;
    int32_t i;
    ପାଇଁ(i=0;; ++i) {
        ଯଦି(i==srcLength) {
            ଯଦି(toASCII) {
                ଯଦି((i-labelStart)>63) {
                    info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
                }
                // There is a trailing dot if labelStart==i.
                ଯଦି(!isLabel && i>=254 && (i>254 || labelStart<i)) {
                    info.errors|=UIDNA_ERROR_DOMAIN_NAME_TOO_LONG;
                }
            }
            info.errors|=info.labelErrors;
            dest.releaseBuffer(i);
            ଫେରନ୍ତୁ dest;
        }
        char16_t c=srcArray[i];
        ଯଦି(c>0x7f) {
            ଭାଙ୍ଗ;
        }
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା cData=asciiData[c];
        ଯଦି(cData>0) {
            destArray[i]=c+0x20;  // Lowercase an uppercase ASCII letter.
        } ଅନ୍ୟଥା ଯଦି(cData<0 && disallowNonLDHDot) {
            ଭାଙ୍ଗ;  // Replacing with U+FFFD can be complicated for toASCII.
        } ଅନ୍ୟଥା {
            destArray[i]=c;
            ଯଦି(c==0x2d) {  // hyphen
                ଯଦି(i==(labelStart+3) && srcArray[i-1]==0x2d) {
                    // "??--..." is Punycode or forbidden.
                    ++i;  // '-' was copied to dest already
                    ଭାଙ୍ଗ;
                }
                ଯଦି(i==labelStart) {
                    // label starts with "-"
                    info.labelErrors|=UIDNA_ERROR_LEADING_HYPHEN;
                }
                ଯଦି((i+1)==srcLength || srcArray[i+1]==0x2e) {
                    // label ends with "-"
                    info.labelErrors|=UIDNA_ERROR_TRAILING_HYPHEN;
                }
            } ଅନ୍ୟଥା ଯଦି(c==0x2e) {  // dot
                ଯଦି(isLabel) {
                    // Replacing with U+FFFD can be complicated for toASCII.
                    ++i;  // '.' was copied to dest already
                    ଭାଙ୍ଗ;
                }
                ଯଦି(i==labelStart) {
                    info.labelErrors|=UIDNA_ERROR_EMPTY_LABEL;
                }
                ଯଦି(toASCII && (i-labelStart)>63) {
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
    ଯଦି( info.isBiDi && U_SUCCESS(errorCode) && (info.errors&severeErrors)==0 &&
        (!info.isOkBiDi || (labelStart>0 && !isASCIIOkBiDi(dest.getBuffer(), labelStart)))
    ) {
        info.errors|=UIDNA_ERROR_BIDI;
    }
    ଫେରନ୍ତୁ dest;
}

ଶୂନ୍ୟ
UTS46::processUTF8(StringPiece src,
                   UBool isLabel, UBool toASCII,
                   ByteSink &dest,
                   IDNAInfo &info, UErrorCode &errorCode) ଅଚଳ {
    ଯଦି(U_FAILURE(errorCode)) {
        ଫେରନ୍ତୁ;
    }
    ଅଚଳ ଅକ୍ଷର *srcArray=src.data();
    int32_t srcLength=src.length();
    ଯଦି(srcArray==nullptr && srcLength!=0) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ;
    }
    // Arguments are fine, reset output values.
    info.reset();
    ଯଦି(srcLength==0) {
        info.errors|=UIDNA_ERROR_EMPTY_LABEL;
        dest.Flush();
        ଫେରନ୍ତୁ;
    }
    UnicodeString destString;
    int32_t labelStart=0;
    ଯଦି(srcLength<=256) {  // length of stackArray[]
        // ASCII fastpath
        ଅକ୍ଷର ଗଦା_Array[256];
        int32_t destCapacity;
        ଅକ୍ଷର *destArray=dest.GetAppendBuffer(srcLength, srcLength+20,
                                             ଗଦା_Array, UPRV_LENGTHOF(ଗଦା_Array), &destCapacity);
        UBool disallowNonLDHDot=(options&UIDNA_USE_STD3_RULES)!=0;
        int32_t i;
        ପାଇଁ(i=0;; ++i) {
            ଯଦି(i==srcLength) {
                ଯଦି(toASCII) {
                    ଯଦି((i-labelStart)>63) {
                        info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
                    }
                    // There is a trailing dot if labelStart==i.
                    ଯଦି(!isLabel && i>=254 && (i>254 || labelStart<i)) {
                        info.errors|=UIDNA_ERROR_DOMAIN_NAME_TOO_LONG;
                    }
                }
                info.errors|=info.labelErrors;
                dest.Append(destArray, i);
                dest.Flush();
                ଫେରନ୍ତୁ;
            }
            ଅକ୍ଷର c=srcArray[i];
            ଯଦି (static_cast<int8_t>(c) < 0) { // (uint8_t)c>0x7f
                ଭାଙ୍ଗ;
            }
            ପୂର୍ଣ୍ଣସଂଖ୍ୟା cData = asciiData[static_cast<ପୂର୍ଣ୍ଣସଂଖ୍ୟା>(c)]; // Cast: gcc warns about indexing with a char.
            ଯଦି(cData>0) {
                destArray[i]=c+0x20;  // Lowercase an uppercase ASCII letter.
            } ଅନ୍ୟଥା ଯଦି(cData<0 && disallowNonLDHDot) {
                ଭାଙ୍ଗ;  // Replacing with U+FFFD can be complicated for toASCII.
            } ଅନ୍ୟଥା {
                destArray[i]=c;
                ଯଦି(c==0x2d) {  // hyphen
                    ଯଦି(i==(labelStart+3) && srcArray[i-1]==0x2d) {
                        // "??--..." is Punycode or forbidden.
                        ଭାଙ୍ଗ;
                    }
                    ଯଦି(i==labelStart) {
                        // label starts with "-"
                        info.labelErrors|=UIDNA_ERROR_LEADING_HYPHEN;
                    }
                    ଯଦି((i+1)==srcLength || srcArray[i+1]==0x2e) {
                        // label ends with "-"
                        info.labelErrors|=UIDNA_ERROR_TRAILING_HYPHEN;
                    }
                } ଅନ୍ୟଥା ଯଦି(c==0x2e) {  // dot
                    ଯଦି(isLabel) {
                        ଭାଙ୍ଗ;  // Replacing with U+FFFD can be complicated for toASCII.
                    }
                    ଯଦି(i==labelStart) {
                        info.labelErrors|=UIDNA_ERROR_EMPTY_LABEL;
                    }
                    ଯଦି(toASCII && (i-labelStart)>63) {
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
    } ଅନ୍ୟଥା {
        // src is too long for the ASCII fastpath implementation.
        processUnicode(UnicodeString::fromUTF8(src), 0, 0,
                       isLabel, toASCII,
                       destString, info, errorCode);
    }
    destString.toUTF8(dest);  // calls dest.Flush()
    ଯଦି(toASCII && !isLabel) {
        // length==labelStart==254 means that there is a trailing dot (ok) and
        // destString is empty (do not index at 253-labelStart).
        int32_t length=labelStart+destString.length();
        ଯଦି( length>=254 && isASCIIString(destString) &&
            (length>254 ||
             (labelStart<254 && destString[253-labelStart]!=0x2e))
        ) {
            info.errors|=UIDNA_ERROR_DOMAIN_NAME_TOO_LONG;
        }
    }
    ଯଦି( info.isBiDi && U_SUCCESS(errorCode) && (info.errors&severeErrors)==0 &&
        (!info.isOkBiDi || (labelStart>0 && !isASCIIOkBiDi(srcArray, labelStart)))
    ) {
        info.errors|=UIDNA_ERROR_BIDI;
    }
}

UnicodeString&
UTS46::processUnicode(ଅଚଳ UnicodeString& src,
                      int32_t labelStart,
                      int32_t mappingStart,
                      UBool isLabel,
                      UBool toASCII,
                      UnicodeString& dest U_LIFETIME_BOUND,
                      IDNAInfo& info,
                      UErrorCode& errorCode) ଅଚଳ {
    ଯଦି(mappingStart==0) {
        uts46Norm2.normalize(src, dest, errorCode);
    } ଅନ୍ୟଥା {
        uts46Norm2.normalizeSecondAndAppend(dest, src.tempSubString(mappingStart), errorCode);
    }
    ଯଦି(U_FAILURE(errorCode)) {
        ଫେରନ୍ତୁ dest;
    }
    UBool doMapDevChars=
        toASCII ? (options&UIDNA_NONTRANSITIONAL_TO_ASCII)==0 :
                  (options&UIDNA_NONTRANSITIONAL_TO_UNICODE)==0;
    ଅଚଳ char16_t *destArray=dest.getBuffer();
    int32_t destLength=dest.length();
    int32_t labelLimit=labelStart;
    ଯେତେବେଳେ(labelLimit<destLength) {
        char16_t c=destArray[labelLimit];
        ଯଦି(c==0x2e && !isLabel) {
            int32_t labelLength=labelLimit-labelStart;
            int32_t newLength=processLabel(dest, labelStart, labelLength,
                                            toASCII, info, errorCode);
            info.errors|=info.labelErrors;
            info.labelErrors=0;
            ଯଦି(U_FAILURE(errorCode)) {
                ଫେରନ୍ତୁ dest;
            }
            destArray=dest.getBuffer();
            destLength+=newLength-labelLength;
            labelLimit=labelStart+=newLength+1;
            ଜାରି;
        } ଅନ୍ୟଥା ଯଦି(c<0xdf) {
            // pass
        } ଅନ୍ୟଥା ଯଦି(c<=0x200d && (c==0xdf || c==0x3c2 || c>=0x200c)) {
            info.isTransDiff=ସତ୍ୟ;
            ଯଦି(doMapDevChars) {
                destLength=mapDevChars(dest, labelStart, labelLimit, errorCode);
                ଯଦି(U_FAILURE(errorCode)) {
                    ଫେରନ୍ତୁ dest;
                }
                destArray=dest.getBuffer();
                // All deviation characters have been mapped, no need to check for them again.
                doMapDevChars=ମିଥ୍ୟା;
                // Do not increment labelLimit in case c was removed.
                ଜାରି;
            }
        } ଅନ୍ୟଥା ଯଦି(U16_IS_SURROGATE(c)) {
            ଯଦି(U16_IS_SURROGATE_LEAD(c) ?
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
    ଯଦି(0==labelStart || labelStart<labelLimit) {
        processLabel(dest, labelStart, labelLimit-labelStart,
                      toASCII, info, errorCode);
        info.errors|=info.labelErrors;
    }
    ଫେରନ୍ତୁ dest;
}

int32_t
UTS46::mapDevChars(UnicodeString &dest, int32_t labelStart, int32_t mappingStart,
                   UErrorCode &errorCode) ଅଚଳ {
    ଯଦି(U_FAILURE(errorCode)) {
        ଫେରନ୍ତୁ 0;
    }
    int32_t length=dest.length();
    char16_t *s=dest.getBuffer(dest[mappingStart]==0xdf ? length+1 : length);
    ଯଦି(s==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        ଫେରନ୍ତୁ length;
    }
    int32_t capacity=dest.getCapacity();
    UBool didMapDevChars=ମିଥ୍ୟା;
    int32_t readIndex=mappingStart, writeIndex=mappingStart;
    କର {
        char16_t c=s[readIndex++];
        ବାଛ(c) {
        ସ୍ଥିତି 0xdf:
            // Map sharp s to ss.
            didMapDevChars=ସତ୍ୟ;
            s[writeIndex++]=0x73;  // Replace sharp s with first s.
            // Insert second s and account for possible buffer reallocation.
            ଯଦି(writeIndex==readIndex) {
                ଯଦି(length==capacity) {
                    dest.releaseBuffer(length);
                    s=dest.getBuffer(length+1);
                    ଯଦି(s==nullptr) {
                        errorCode=U_MEMORY_ALLOCATION_ERROR;
                        ଫେରନ୍ତୁ length;
                    }
                    capacity=dest.getCapacity();
                }
                u_memmove(s+writeIndex+1, s+writeIndex, length-writeIndex);
                ++readIndex;
            }
            s[writeIndex++]=0x73;
            ++length;
            ଭାଙ୍ଗ;
        ସ୍ଥିତି 0x3c2:  // Map final sigma to nonfinal sigma.
            didMapDevChars=ସତ୍ୟ;
            s[writeIndex++]=0x3c3;
            ଭାଙ୍ଗ;
        ସ୍ଥିତି 0x200c:  // Ignore/remove ZWNJ.
        ସ୍ଥିତି 0x200d:  // Ignore/remove ZWJ.
            didMapDevChars=ସତ୍ୟ;
            --length;
            ଭାଙ୍ଗ;
        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
            // Only really necessary if writeIndex was different from readIndex.
            s[writeIndex++]=c;
            ଭାଙ୍ଗ;
        }
    } ଯେତେବେଳେ(writeIndex<length);
    dest.releaseBuffer(length);
    ଯଦି(didMapDevChars) {
        // Mapping deviation characters might have resulted in an un-NFC string.
        // We could use either the NFC or the UTS #46 normalizer.
        // By using the UTS #46 normalizer again, we avoid having to load a second .nrm data file.
        UnicodeString normalized;
        uts46Norm2.normalize(dest.tempSubString(labelStart), normalized, errorCode);
        ଯଦି(U_SUCCESS(errorCode)) {
            dest.replace(labelStart, 0x7fffffff, normalized);
            ଯଦି(dest.isBogus()) {
                errorCode=U_MEMORY_ALLOCATION_ERROR;
            }
            ଫେରନ୍ତୁ dest.length();
        }
    }
    ଫେରନ୍ତୁ length;
}

// Replace the label in dest with the label string, if the label was modified.
// If &label==&dest then the label was modified in-place and labelLength
// is the new label length, different from label.length().
// If &label!=&dest then labelLength==label.length().
// Returns labelLength (= the new label length).
ସ୍ଥିର int32_t
replaceLabel(UnicodeString &dest, int32_t destLabelStart, int32_t destLabelLength,
             ଅଚଳ UnicodeString &label, int32_t labelLength, UErrorCode &errorCode) {
    ଯଦି(U_FAILURE(errorCode)) {
        ଫେରନ୍ତୁ 0;
    }
    ଯଦି(&label!=&dest) {
        dest.replace(destLabelStart, destLabelLength, label);
        ଯଦି(dest.isBogus()) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            ଫେରନ୍ତୁ 0;
        }
    }
    ଫେରନ୍ତୁ labelLength;
}

int32_t
UTS46::processLabel(UnicodeString &dest,
                    int32_t labelStart, int32_t labelLength,
                    UBool toASCII,
                    IDNAInfo &info, UErrorCode &errorCode) ଅଚଳ {
    ଯଦି(U_FAILURE(errorCode)) {
        ଫେରନ୍ତୁ 0;
    }
    UnicodeString fromPunycode;
    UnicodeString *labelString;
    ଅଚଳ char16_t *label=dest.getBuffer()+labelStart;
    int32_t destLabelStart=labelStart;
    int32_t destLabelLength=labelLength;
    UBool wasPunycode;
    ଯଦି(labelLength>=4 && label[0]==0x78 && label[1]==0x6e && label[2]==0x2d && label[3]==0x2d) {
        // Label starts with "xn--", try to un-Punycode it.
        // In IDNA2008, labels like "xn--" (decodes to an empty string) and
        // "xn--ASCII-" (decodes to just "ASCII") fail the round-trip validation from
        // comparing the ToUnicode input with the back-to-ToASCII output.
        // They are alternate encodings of the respective ASCII labels.
        // Ignore "xn---" here: It will fail Punycode.decode() which logically comes before
        // the round-trip verification.
        ଯଦି(labelLength==4 || (labelLength>5 && label[labelLength-1]==u'-')) {
            info.labelErrors|=UIDNA_ERROR_INVALID_ACE_LABEL;
            ଫେରନ୍ତୁ markBadACELabel(dest, labelStart, labelLength, toASCII, info, errorCode);
        }
        wasPunycode=ସତ୍ୟ;
        char16_t *unicodeBuffer=fromPunycode.getBuffer(-1);  // capacity==-1: most labels should fit
        ଯଦି(unicodeBuffer==nullptr) {
            // Should never occur if we used capacity==-1 which uses the internal buffer.
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            ଫେରନ୍ତୁ labelLength;
        }
        UErrorCode punycodeErrorCode=U_ZERO_ERROR;
        int32_t unicodeLength=u_strFromPunycode(label+4, labelLength-4,
                                                unicodeBuffer, fromPunycode.getCapacity(),
                                                nullptr, &punycodeErrorCode);
        ଯଦି(punycodeErrorCode==U_BUFFER_OVERFLOW_ERROR) {
            fromPunycode.releaseBuffer(0);
            unicodeBuffer=fromPunycode.getBuffer(unicodeLength);
            ଯଦି(unicodeBuffer==nullptr) {
                errorCode=U_MEMORY_ALLOCATION_ERROR;
                ଫେରନ୍ତୁ labelLength;
            }
            punycodeErrorCode=U_ZERO_ERROR;
            unicodeLength=u_strFromPunycode(label+4, labelLength-4,
                                            unicodeBuffer, fromPunycode.getCapacity(),
                                            nullptr, &punycodeErrorCode);
        }
        fromPunycode.releaseBuffer(unicodeLength);
        ଯଦି(U_FAILURE(punycodeErrorCode)) {
            info.labelErrors|=UIDNA_ERROR_PUNYCODE;
            ଫେରନ୍ତୁ markBadACELabel(dest, labelStart, labelLength, toASCII, info, errorCode);
        }
        // Check for NFC, and for characters that are not
        // valid or deviation characters according to the normalizer.
        // If there is something wrong, then the string will change.
        // Note that the normalizer passes through non-LDH ASCII and deviation characters.
        // Deviation characters are ok in Punycode even in transitional processing.
        // In the code further below, if we find non-LDH ASCII and we have UIDNA_USE_STD3_RULES
        // then we will set UIDNA_ERROR_INVALID_ACE_LABEL there too.
        UBool isValid=uts46Norm2.isNormalized(fromPunycode, errorCode);
        ଯଦି(U_FAILURE(errorCode)) {
            ଫେରନ୍ତୁ labelLength;
        }
        // Unicode 15.1 UTS #46:
        // Added an additional condition in 4.1 Validity Criteria to
        // disallow labels such as xn--xn---epa., which do not round-trip.
        // --> Validity Criteria new criterion 4:
        // If not CheckHyphens, the label must not begin with “xn--”.
        ଯଦି(!isValid || fromPunycode.startsWith(UnicodeString::readOnlyAlias(u"xn--"))) {
            info.labelErrors|=UIDNA_ERROR_INVALID_ACE_LABEL;
            ଫେରନ୍ତୁ markBadACELabel(dest, labelStart, labelLength, toASCII, info, errorCode);
        }
        labelString=&fromPunycode;
        label=fromPunycode.getBuffer();
        labelStart=0;
        labelLength=fromPunycode.length();
    } ଅନ୍ୟଥା {
        wasPunycode=ମିଥ୍ୟା;
        labelString=&dest;
    }
    // Validity check
    ଯଦି(labelLength==0) {
        info.labelErrors|=UIDNA_ERROR_EMPTY_LABEL;
        ଫେରନ୍ତୁ replaceLabel(dest, destLabelStart, destLabelLength,
                            *labelString, labelLength, errorCode);
    }
    // labelLength>0
    ଯଦି(labelLength>=4 && label[2]==0x2d && label[3]==0x2d) {
        // label starts with "??--"
        info.labelErrors|=UIDNA_ERROR_HYPHEN_3_4;
    }
    ଯଦି(label[0]==0x2d) {
        // label starts with "-"
        info.labelErrors|=UIDNA_ERROR_LEADING_HYPHEN;
    }
    ଯଦି(label[labelLength-1]==0x2d) {
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
    ଅଚଳ char16_t *limit=label+labelLength;
    char16_t oredChars=0;
    // If we enforce STD3 rules, then ASCII characters other than LDH and dot are disallowed.
    UBool disallowNonLDHDot=(options&UIDNA_USE_STD3_RULES)!=0;
    କର {
        char16_t c=*s;
        ଯଦି(c<=0x7f) {
            ଯଦି(c==0x2e) {
                info.labelErrors|=UIDNA_ERROR_LABEL_HAS_DOT;
                *s=0xfffd;
            } ଅନ୍ୟଥା ଯଦି(disallowNonLDHDot && asciiData[c]<0) {
                info.labelErrors|=UIDNA_ERROR_DISALLOWED;
                *s=0xfffd;
            }
        } ଅନ୍ୟଥା {
            oredChars|=c;
            ଯଦି(c==0xfffd) {
                info.labelErrors|=UIDNA_ERROR_DISALLOWED;
            }
        }
        ++s;
    } ଯେତେବେଳେ(s<limit);
    // Check for a leading combining mark after other validity checks
    // so that we don't report UIDNA_ERROR_DISALLOWED for the U+FFFD from here.
    UChar32 c;
    int32_t cpLength=0;
    // "Unsafe" is ok because unpaired surrogates were mapped to U+FFFD.
    U16_NEXT_UNSAFE(label, cpLength, c);
    ଯଦି((U_GET_GC_MASK(c)&U_GC_M_MASK)!=0) {
        info.labelErrors|=UIDNA_ERROR_LEADING_COMBINING_MARK;
        labelString->replace(labelStart, cpLength, static_cast<char16_t>(0xfffd));
        label=labelString->getBuffer()+labelStart;
        labelLength+=1-cpLength;
        ଯଦି(labelString==&dest) {
            destLabelLength=labelLength;
        }
    }
    ଯଦି((info.labelErrors&severeErrors)==0) {
        // Do contextual checks only if we do not have U+FFFD from a severe error
        // because U+FFFD can make these checks fail.
        ଯଦି((options&UIDNA_CHECK_BIDI)!=0 && (!info.isBiDi || info.isOkBiDi)) {
            checkLabelBiDi(label, labelLength, info);
        }
        ଯଦି( (options&UIDNA_CHECK_CONTEXTJ)!=0 && (oredChars&0x200c)==0x200c &&
            !isLabelOkContextJ(label, labelLength)
        ) {
            info.labelErrors|=UIDNA_ERROR_CONTEXTJ;
        }
        ଯଦି((options&UIDNA_CHECK_CONTEXTO)!=0 && oredChars>=0xb7) {
            checkLabelContextO(label, labelLength, info);
        }
        ଯଦି(toASCII) {
            ଯଦି(wasPunycode) {
                // Leave a Punycode label unchanged if it has no severe errors.
                ଯଦି(destLabelLength>63) {
                    info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
                }
                ଫେରନ୍ତୁ destLabelLength;
            } ଅନ୍ୟଥା ଯଦି(oredChars>=0x80) {
                // Contains non-ASCII characters.
                UnicodeString punycode;
                char16_t *buffer=punycode.getBuffer(63);  // 63==maximum DNS label length
                ଯଦି(buffer==nullptr) {
                    errorCode=U_MEMORY_ALLOCATION_ERROR;
                    ଫେରନ୍ତୁ destLabelLength;
                }
                buffer[0]=0x78;  // Write "xn--".
                buffer[1]=0x6e;
                buffer[2]=0x2d;
                buffer[3]=0x2d;
                UErrorCode punycodeErrorCode=U_ZERO_ERROR;
                int32_t punycodeLength=u_strToPunycode(label, labelLength,
                                                      buffer+4, punycode.getCapacity()-4,
                                                      nullptr, &punycodeErrorCode);
                ଯଦି(punycodeErrorCode==U_BUFFER_OVERFLOW_ERROR) {
                    punycodeErrorCode=U_ZERO_ERROR;
                    punycode.releaseBuffer(4);
                    buffer=punycode.getBuffer(4+punycodeLength);
                    ଯଦି(buffer==nullptr) {
                        errorCode=U_MEMORY_ALLOCATION_ERROR;
                        ଫେରନ୍ତୁ destLabelLength;
                    }
                    punycodeLength=u_strToPunycode(label, labelLength,
                                                  buffer+4, punycode.getCapacity()-4,
                                                  nullptr, &punycodeErrorCode);
                }
                punycodeLength+=4;
                punycode.releaseBuffer(punycodeLength);
                ଯଦି(U_FAILURE(punycodeErrorCode)) {
                    errorCode = punycodeErrorCode;
                    ଫେରନ୍ତୁ destLabelLength;
                }
                ଯଦି(punycodeLength>63) {
                    info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
                }
                ଫେରନ୍ତୁ replaceLabel(dest, destLabelStart, destLabelLength,
                                    punycode, punycodeLength, errorCode);
            } ଅନ୍ୟଥା {
                // all-ASCII label
                ଯଦି(labelLength>63) {
                    info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
                }
            }
        }
    } ଅନ୍ୟଥା {
        // If a Punycode label has severe errors,
        // then leave it but make sure it does not look valid.
        ଯଦି(wasPunycode) {
            info.labelErrors|=UIDNA_ERROR_INVALID_ACE_LABEL;
            ଫେରନ୍ତୁ markBadACELabel(dest, destLabelStart, destLabelLength, toASCII, info, errorCode);
        }
    }
    ଫେରନ୍ତୁ replaceLabel(dest, destLabelStart, destLabelLength,
                        *labelString, labelLength, errorCode);
}

// Make sure an ACE label does not look valid.
// Append U+FFFD if the label has only LDH characters.
// If UIDNA_USE_STD3_RULES, also replace disallowed ASCII characters with U+FFFD.
int32_t
UTS46::markBadACELabel(UnicodeString &dest,
                       int32_t labelStart, int32_t labelLength,
                       UBool toASCII, IDNAInfo &info, UErrorCode &errorCode) ଅଚଳ {
    ଯଦି(U_FAILURE(errorCode)) {
        ଫେରନ୍ତୁ 0;
    }
    UBool disallowNonLDHDot=(options&UIDNA_USE_STD3_RULES)!=0;
    UBool isASCII=ସତ୍ୟ;
    UBool onlyLDH=ସତ୍ୟ;
    ଅଚଳ char16_t *label=dest.getBuffer()+labelStart;
    ଅଚଳ char16_t *limit=label+labelLength;
    // Start after the initial "xn--".
    // Ok to cast away const because we own the UnicodeString.
    ପାଇଁ(char16_t *s=const_cast<char16_t *>(label+4); s<limit; ++s) {
        char16_t c=*s;
        ଯଦି(c<=0x7f) {
            ଯଦି(c==0x2e) {
                info.labelErrors|=UIDNA_ERROR_LABEL_HAS_DOT;
                *s=0xfffd;
                isASCII=onlyLDH=ମିଥ୍ୟା;
            } ଅନ୍ୟଥା ଯଦି(asciiData[c]<0) {
                onlyLDH=ମିଥ୍ୟା;
                ଯଦି(disallowNonLDHDot) {
                    *s=0xfffd;
                    isASCII=ମିଥ୍ୟା;
                }
            }
        } ଅନ୍ୟଥା {
            isASCII=onlyLDH=ମିଥ୍ୟା;
        }
    }
    ଯଦି(onlyLDH) {
        dest.insert(labelStart + labelLength, static_cast<char16_t>(0xfffd));
        ଯଦି(dest.isBogus()) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            ଫେରନ୍ତୁ 0;
        }
        ++labelLength;
    } ଅନ୍ୟଥା {
        ଯଦି(toASCII && isASCII && labelLength>63) {
            info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
        }
    }
    ଫେରନ୍ତୁ labelLength;
}

ଅଚଳ uint32_t L_MASK=U_MASK(U_ବାମ_TO_ଡାହାଣ);
ଅଚଳ uint32_t R_AL_MASK=U_MASK(U_ଡାହାଣ_TO_ବାମ)|U_MASK(U_ଡାହାଣ_TO_ବାମ_ARABIC);
ଅଚଳ uint32_t L_R_AL_MASK=L_MASK|R_AL_MASK;

ଅଚଳ uint32_t R_AL_AN_MASK=R_AL_MASK|U_MASK(U_ARABIC_ସଂଖ୍ୟା);

ଅଚଳ uint32_t EN_AN_MASK=U_MASK(U_EUROPEAN_ସଂଖ୍ୟା)|U_MASK(U_ARABIC_ସଂଖ୍ୟା);
ଅଚଳ uint32_t R_AL_EN_AN_MASK=R_AL_MASK|EN_AN_MASK;
ଅଚଳ uint32_t L_EN_MASK=L_MASK|U_MASK(U_EUROPEAN_ସଂଖ୍ୟା);

ଅଚଳ uint32_t ES_CS_ET_ON_BN_NSM_MASK=
    U_MASK(U_EUROPEAN_ସଂଖ୍ୟା_SEPARATOR)|
    U_MASK(U_COMMON_ସଂଖ୍ୟା_SEPARATOR)|
    U_MASK(U_EUROPEAN_ସଂଖ୍ୟା_TERMINATOR)|
    U_MASK(U_OTHER_NEUTRAL)|
    U_MASK(U_BOUNDARY_NEUTRAL)|
    U_MASK(U_DIR_NON_SPACING_MARK);
ଅଚଳ uint32_t L_EN_ES_CS_ET_ON_BN_NSM_MASK=L_EN_MASK|ES_CS_ET_ON_BN_NSM_MASK;
ଅଚଳ uint32_t R_AL_AN_EN_ES_CS_ET_ON_BN_NSM_MASK=R_AL_MASK|EN_AN_MASK|ES_CS_ET_ON_BN_NSM_MASK;

// We scan the whole label and check both for whether it contains RTL characters
// and whether it passes the BiDi Rule.
// In a BiDi domain name, all labels must pass the BiDi Rule, but we might find
// that a domain name is a BiDi domain name (has an RTL label) only after
// processing several earlier labels.
ଶୂନ୍ୟ
UTS46::checkLabelBiDi(ଅଚଳ char16_t *label, int32_t labelLength, IDNAInfo &info) ଅଚଳ {
    // IDNA2008 BiDi rule
    // Get the directionality of the first character.
    UChar32 c;
    int32_t i=0;
    U16_NEXT_UNSAFE(label, i, c);
    uint32_t firstMask=U_MASK(u_charDirection(c));
    // 1. The first character must be a character with BIDI property L, R
    // or AL.  If it has the R or AL property, it is an RTL label; if it
    // has the L property, it is an LTR label.
    ଯଦି((firstMask&~L_R_AL_MASK)!=0) {
        info.isOkBiDi=ମିଥ୍ୟା;
    }
    // Get the directionality of the last non-NSM character.
    uint32_t lastMask;
    ପାଇଁ(;;) {
        ଯଦି(i>=labelLength) {
            lastMask=firstMask;
            ଭାଙ୍ଗ;
        }
        U16_PREV_UNSAFE(label, labelLength, c);
        UCharDirection dir=u_charDirection(c);
        ଯଦି(dir!=U_DIR_NON_SPACING_MARK) {
            lastMask=U_MASK(dir);
            ଭାଙ୍ଗ;
        }
    }
    // 3. In an RTL label, the end of the label must be a character with
    // BIDI property R, AL, EN or AN, followed by zero or more
    // characters with BIDI property NSM.
    // 6. In an LTR label, the end of the label must be a character with
    // BIDI property L or EN, followed by zero or more characters with
    // BIDI property NSM.
    ଯଦି( (firstMask&L_MASK)!=0 ?
            (lastMask&~L_EN_MASK)!=0 :
            (lastMask&~R_AL_EN_AN_MASK)!=0
    ) {
        info.isOkBiDi=ମିଥ୍ୟା;
    }
    // Add the directionalities of the intervening characters.
    uint32_t mask=firstMask|lastMask;
    ଯେତେବେଳେ(i<labelLength) {
        U16_NEXT_UNSAFE(label, i, c);
        mask|=U_MASK(u_charDirection(c));
    }
    ଯଦି(firstMask&L_MASK) {
        // 5. In an LTR label, only characters with the BIDI properties L, EN,
        // ES, CS, ET, ON, BN and NSM are allowed.
        ଯଦି((mask&~L_EN_ES_CS_ET_ON_BN_NSM_MASK)!=0) {
            info.isOkBiDi=ମିଥ୍ୟା;
        }
    } ଅନ୍ୟଥା {
        // 2. In an RTL label, only characters with the BIDI properties R, AL,
        // AN, EN, ES, CS, ET, ON, BN and NSM are allowed.
        ଯଦି((mask&~R_AL_AN_EN_ES_CS_ET_ON_BN_NSM_MASK)!=0) {
            info.isOkBiDi=ମିଥ୍ୟା;
        }
        // 4. In an RTL label, if an EN is present, no AN may be present, and
        // vice versa.
        ଯଦି((mask&EN_AN_MASK)==EN_AN_MASK) {
            info.isOkBiDi=ମିଥ୍ୟା;
        }
    }
    // An RTL label is a label that contains at least one character of type
    // R, AL or AN. [...]
    // A "BIDI domain name" is a domain name that contains at least one RTL
    // label. [...]
    // The following rule, consisting of six conditions, applies to labels
    // in BIDI domain names.
    ଯଦି((mask&R_AL_AN_MASK)!=0) {
        info.isBiDi=ସତ୍ୟ;
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
ସ୍ଥିର UBool
isASCIIOkBiDi(ଅଚଳ char16_t *s, int32_t length) {
    int32_t labelStart=0;
    ପାଇଁ(int32_t i=0; i<length; ++i) {
        char16_t c=s[i];
        ଯଦି(c==0x2e) {  // dot
            ଯଦି(i>labelStart) {
                c=s[i-1];
                ଯଦି(!(0x61<=c && c<=0x7a) && !(0x30<=c && c<=0x39)) {
                    // Last character in the label is not an L or EN.
                    ଫେରନ୍ତୁ ମିଥ୍ୟା;
                }
            }
            labelStart=i+1;
        } ଅନ୍ୟଥା ଯଦି(i==labelStart) {
            ଯଦି(!(0x61<=c && c<=0x7a)) {
                // First character in the label is not an L.
                ଫେରନ୍ତୁ ମିଥ୍ୟା;
            }
        } ଅନ୍ୟଥା {
            ଯଦି(c<=0x20 && (c>=0x1c || (9<=c && c<=0xd))) {
                // Intermediate character in the label is a B, S or WS.
                ଫେରନ୍ତୁ ମିଥ୍ୟା;
            }
        }
    }
    ଫେରନ୍ତୁ ସତ୍ୟ;
}

// UTF-8 version, called for source ASCII prefix.
// Can contain uppercase A-Z.
// s[length-1] must be the trailing dot.
ସ୍ଥିର UBool
isASCIIOkBiDi(ଅଚଳ ଅକ୍ଷର *s, int32_t length) {
    int32_t labelStart=0;
    ପାଇଁ(int32_t i=0; i<length; ++i) {
        ଅକ୍ଷର c=s[i];
        ଯଦି(c==0x2e) {  // dot
            ଯଦି(i>labelStart) {
                c=s[i-1];
                ଯଦି(!(0x61<=c && c<=0x7a) && !(0x41<=c && c<=0x5a) && !(0x30<=c && c<=0x39)) {
                    // Last character in the label is not an L or EN.
                    ଫେରନ୍ତୁ ମିଥ୍ୟା;
                }
            }
            labelStart=i+1;
        } ଅନ୍ୟଥା ଯଦି(i==labelStart) {
            ଯଦି(!(0x61<=c && c<=0x7a) && !(0x41<=c && c<=0x5a)) {
                // First character in the label is not an L.
                ଫେରନ୍ତୁ ମିଥ୍ୟା;
            }
        } ଅନ୍ୟଥା {
            ଯଦି(c<=0x20 && (c>=0x1c || (9<=c && c<=0xd))) {
                // Intermediate character in the label is a B, S or WS.
                ଫେରନ୍ତୁ ମିଥ୍ୟା;
            }
        }
    }
    ଫେରନ୍ତୁ ସତ୍ୟ;
}

UBool
UTS46::isLabelOkContextJ(ଅଚଳ char16_t *label, int32_t labelLength) ଅଚଳ {
    // [IDNA2008-Tables]
    // 200C..200D  ; CONTEXTJ    # ZERO WIDTH NON-JOINER..ZERO WIDTH JOINER
    ପାଇଁ(int32_t i=0; i<labelLength; ++i) {
        ଯଦି(label[i]==0x200c) {
            // Appendix A.1. ZERO WIDTH NON-JOINER
            // Rule Set:
            //  False;
            //  If Canonical_Combining_Class(Before(cp)) .eq.  Virama Then True;
            //  If RegExpMatch((Joining_Type:{L,D})(Joining_Type:T)*\u200C
            //     (Joining_Type:T)*(Joining_Type:{R,D})) Then True;
            ଯଦି(i==0) {
                ଫେରନ୍ତୁ ମିଥ୍ୟା;
            }
            UChar32 c;
            int32_t j=i;
            U16_PREV_UNSAFE(label, j, c);
            ଯଦି(uts46Norm2.getCombiningClass(c)==9) {
                ଜାରି;
            }
            // check precontext (Joining_Type:{L,D})(Joining_Type:T)*
            ପାଇଁ(;;) {
                UJoiningType type=ubidi_getJoiningType(c);
                ଯଦି(type==U_JT_TRANSPARENT) {
                    ଯଦି(j==0) {
                        ଫେରନ୍ତୁ ମିଥ୍ୟା;
                    }
                    U16_PREV_UNSAFE(label, j, c);
                } ଅନ୍ୟଥା ଯଦି(type==U_JT_ବାମ_JOINING || type==U_JT_DUAL_JOINING) {
                    ଭାଙ୍ଗ;  // precontext fulfilled
                } ଅନ୍ୟଥା {
                    ଫେରନ୍ତୁ ମିଥ୍ୟା;
                }
            }
            // check postcontext (Joining_Type:T)*(Joining_Type:{R,D})
            ପାଇଁ(j=i+1;;) {
                ଯଦି(j==labelLength) {
                    ଫେରନ୍ତୁ ମିଥ୍ୟା;
                }
                U16_NEXT_UNSAFE(label, j, c);
                UJoiningType type=ubidi_getJoiningType(c);
                ଯଦି(type==U_JT_TRANSPARENT) {
                    // just skip this character
                } ଅନ୍ୟଥା ଯଦି(type==U_JT_ଡାହାଣ_JOINING || type==U_JT_DUAL_JOINING) {
                    ଭାଙ୍ଗ;  // postcontext fulfilled
                } ଅନ୍ୟଥା {
                    ଫେରନ୍ତୁ ମିଥ୍ୟା;
                }
            }
        } ଅନ୍ୟଥା ଯଦି(label[i]==0x200d) {
            // Appendix A.2. ZERO WIDTH JOINER (U+200D)
            // Rule Set:
            //  False;
            //  If Canonical_Combining_Class(Before(cp)) .eq.  Virama Then True;
            ଯଦି(i==0) {
                ଫେରନ୍ତୁ ମିଥ୍ୟା;
            }
            UChar32 c;
            int32_t j=i;
            U16_PREV_UNSAFE(label, j, c);
            ଯଦି(uts46Norm2.getCombiningClass(c)!=9) {
                ଫେରନ୍ତୁ ମିଥ୍ୟା;
            }
        }
    }
    ଫେରନ୍ତୁ ସତ୍ୟ;
}

ଶୂନ୍ୟ
UTS46::checkLabelContextO(ଅଚଳ char16_t *label, int32_t labelLength, IDNAInfo &info) ଅଚଳ {
    int32_t labelEnd=labelLength-1;  // inclusive
    int32_t arabicDigits=0;  // -1 for 066x, +1 for 06Fx
    ପାଇଁ(int32_t i=0; i<=labelEnd; ++i) {
        UChar32 c=label[i];
        ଯଦି(c<0xb7) {
            // ASCII fastpath
        } ଅନ୍ୟଥା ଯଦି(c<=0x6f9) {
            ଯଦି(c==0xb7) {
                // Appendix A.3. MIDDLE DOT (U+00B7)
                // Rule Set:
                //  False;
                //  If Before(cp) .eq.  U+006C And
                //     After(cp) .eq.  U+006C Then True;
                ଯଦି(!(0<i && label[i-1]==0x6c &&
                     i<labelEnd && label[i+1]==0x6c)) {
                    info.labelErrors|=UIDNA_ERROR_CONTEXTO_PUNCTUATION;
                }
            } ଅନ୍ୟଥା ଯଦି(c==0x375) {
                // Appendix A.4. GREEK LOWER NUMERAL SIGN (KERAIA) (U+0375)
                // Rule Set:
                //  False;
                //  If Script(After(cp)) .eq.  Greek Then True;
                UScriptCode script=USCRIPT_INVALID_CODE;
                ଯଦି(i<labelEnd) {
                    UErrorCode errorCode=U_ZERO_ERROR;
                    int32_t j=i+1;
                    U16_NEXT(label, j, labelLength, c);
                    script=uscript_getScript(c, &errorCode);
                }
                ଯଦି(script!=USCRIPT_GREEK) {
                    info.labelErrors|=UIDNA_ERROR_CONTEXTO_PUNCTUATION;
                }
            } ଅନ୍ୟଥା ଯଦି(c==0x5f3 || c==0x5f4) {
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
                ଯଦି(0<i) {
                    UErrorCode errorCode=U_ZERO_ERROR;
                    int32_t j=i;
                    U16_PREV(label, 0, j, c);
                    script=uscript_getScript(c, &errorCode);
                }
                ଯଦି(script!=USCRIPT_HEBREW) {
                    info.labelErrors|=UIDNA_ERROR_CONTEXTO_PUNCTUATION;
                }
            } ଅନ୍ୟଥା ଯଦି(0x660<=c /* && c<=0x6f9 */) {
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
                ଯଦି(c<=0x669) {
                    ଯଦି(arabicDigits>0) {
                        info.labelErrors|=UIDNA_ERROR_CONTEXTO_DIGITS;
                    }
                    arabicDigits=-1;
                } ଅନ୍ୟଥା ଯଦି(0x6f0<=c) {
                    ଯଦି(arabicDigits<0) {
                        info.labelErrors|=UIDNA_ERROR_CONTEXTO_DIGITS;
                    }
                    arabicDigits=1;
                }
            }
        } ଅନ୍ୟଥା ଯଦି(c==0x30fb) {
            // Appendix A.7. KATAKANA MIDDLE DOT (U+30FB)
            // Rule Set:
            //  False;
            //  For All Characters:
            //    If Script(cp) .in. {Hiragana, Katakana, Han} Then True;
            //  End For;
            UErrorCode errorCode=U_ZERO_ERROR;
            ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା j=0;;) {
                ଯଦି(j>labelEnd) {
                    info.labelErrors|=UIDNA_ERROR_CONTEXTO_PUNCTUATION;
                    ଭାଙ୍ଗ;
                }
                U16_NEXT(label, j, labelLength, c);
                UScriptCode script=uscript_getScript(c, &errorCode);
                ଯଦି(script==USCRIPT_HIRAGANA || script==USCRIPT_KATAKANA || script==USCRIPT_HAN) {
                    ଭାଙ୍ଗ;
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
    ଫେରନ୍ତୁ reinterpret_cast<UIDNA *>(IDNA::createUTS46Instance(options, *pErrorCode));
}

U_CAPI ଶୂନ୍ୟ U_EXPORT2
uidna_close(UIDNA *idna) {
    delete reinterpret_cast<IDNA *>(idna);
}

ସ୍ଥିର UBool
checkArgs(ଅଚଳ ଶୂନ୍ୟ *label, int32_t length,
          ଶୂନ୍ୟ *dest, int32_t capacity,
          UIDNAInfo *pInfo, UErrorCode *pErrorCode) {
    ଯଦି(U_FAILURE(*pErrorCode)) {
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    // sizeof(UIDNAInfo)=16 in the first API version.
    ଯଦି(pInfo==nullptr || pInfo->size<16) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    ଯଦି( (label==nullptr ? length!=0 : length<-1) ||
        (dest==nullptr ? capacity!=0 : capacity<0) ||
        (dest==label && label!=nullptr)
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    // Set all *pInfo bytes to 0 except for the size field itself.
    uprv_memset(&pInfo->size+1, 0, pInfo->size-ଆକାର(pInfo->size));
    ଫେରନ୍ତୁ ସତ୍ୟ;
}

ସ୍ଥିର ଶୂନ୍ୟ
idnaInfoToStruct(IDNAInfo &info, UIDNAInfo *pInfo) {
    pInfo->isTransitionalDifferent=info.isTransitionalDifferent();
    pInfo->errors=info.getErrors();
}

U_CAPI int32_t U_EXPORT2
uidna_labelToASCII(ଅଚଳ UIDNA *idna,
                   ଅଚଳ char16_t *label, int32_t length,
                   char16_t *dest, int32_t capacity,
                   UIDNAInfo *pInfo, UErrorCode *pErrorCode) {
    ଯଦି(!checkArgs(label, length, dest, capacity, pInfo, pErrorCode)) {
        ଫେରନ୍ତୁ 0;
    }
    UnicodeString src(length < 0, label, length);
    UnicodeString destString(dest, 0, capacity);
    IDNAInfo info;
    reinterpret_cast<ଅଚଳ IDNA *>(idna)->labelToASCII(src, destString, info, *pErrorCode);
    idnaInfoToStruct(info, pInfo);
    ଫେରନ୍ତୁ destString.extract(dest, capacity, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uidna_labelToUnicode(ଅଚଳ UIDNA *idna,
                     ଅଚଳ char16_t *label, int32_t length,
                     char16_t *dest, int32_t capacity,
                     UIDNAInfo *pInfo, UErrorCode *pErrorCode) {
    ଯଦି(!checkArgs(label, length, dest, capacity, pInfo, pErrorCode)) {
        ଫେରନ୍ତୁ 0;
    }
    UnicodeString src(length < 0, label, length);
    UnicodeString destString(dest, 0, capacity);
    IDNAInfo info;
    reinterpret_cast<ଅଚଳ IDNA *>(idna)->labelToUnicode(src, destString, info, *pErrorCode);
    idnaInfoToStruct(info, pInfo);
    ଫେରନ୍ତୁ destString.extract(dest, capacity, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uidna_nameToASCII(ଅଚଳ UIDNA *idna,
                  ଅଚଳ char16_t *name, int32_t length,
                  char16_t *dest, int32_t capacity,
                  UIDNAInfo *pInfo, UErrorCode *pErrorCode) {
    ଯଦି(!checkArgs(name, length, dest, capacity, pInfo, pErrorCode)) {
        ଫେରନ୍ତୁ 0;
    }
    UnicodeString src(length < 0, name, length);
    UnicodeString destString(dest, 0, capacity);
    IDNAInfo info;
    reinterpret_cast<ଅଚଳ IDNA *>(idna)->nameToASCII(src, destString, info, *pErrorCode);
    idnaInfoToStruct(info, pInfo);
    ଫେରନ୍ତୁ destString.extract(dest, capacity, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uidna_nameToUnicode(ଅଚଳ UIDNA *idna,
                    ଅଚଳ char16_t *name, int32_t length,
                    char16_t *dest, int32_t capacity,
                    UIDNAInfo *pInfo, UErrorCode *pErrorCode) {
    ଯଦି(!checkArgs(name, length, dest, capacity, pInfo, pErrorCode)) {
        ଫେରନ୍ତୁ 0;
    }
    UnicodeString src(length < 0, name, length);
    UnicodeString destString(dest, 0, capacity);
    IDNAInfo info;
    reinterpret_cast<ଅଚଳ IDNA *>(idna)->nameToUnicode(src, destString, info, *pErrorCode);
    idnaInfoToStruct(info, pInfo);
    ଫେରନ୍ତୁ destString.extract(dest, capacity, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uidna_labelToASCII_UTF8(ଅଚଳ UIDNA *idna,
                        ଅଚଳ ଅକ୍ଷର *label, int32_t length,
                        ଅକ୍ଷର *dest, int32_t capacity,
                        UIDNAInfo *pInfo, UErrorCode *pErrorCode) {
    ଯଦି(!checkArgs(label, length, dest, capacity, pInfo, pErrorCode)) {
        ଫେରନ୍ତୁ 0;
    }
    StringPiece src(label, length<0 ? static_cast<int32_t>(uprv_strlen(label)) : length);
    ଫେରନ୍ତୁ ByteSinkUtil::viaByteSinkToTerminatedChars(
        dest, capacity,
        [&](ByteSink& sink, UErrorCode& status) {
            IDNAInfo info;
            reinterpret_cast<ଅଚଳ IDNA *>(idna)->labelToASCII_UTF8(src, sink, info, status);
            idnaInfoToStruct(info, pInfo);
        },
        *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uidna_labelToUnicodeUTF8(ଅଚଳ UIDNA *idna,
                         ଅଚଳ ଅକ୍ଷର *label, int32_t length,
                         ଅକ୍ଷର *dest, int32_t capacity,
                         UIDNAInfo *pInfo, UErrorCode *pErrorCode) {
    ଯଦି(!checkArgs(label, length, dest, capacity, pInfo, pErrorCode)) {
        ଫେରନ୍ତୁ 0;
    }
    StringPiece src(label, length<0 ? static_cast<int32_t>(uprv_strlen(label)) : length);
    ଫେରନ୍ତୁ ByteSinkUtil::viaByteSinkToTerminatedChars(
        dest, capacity,
        [&](ByteSink& sink, UErrorCode& status) {
            IDNAInfo info;
            reinterpret_cast<ଅଚଳ IDNA *>(idna)->labelToUnicodeUTF8(src, sink, info, status);
            idnaInfoToStruct(info, pInfo);
        },
        *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uidna_nameToASCII_UTF8(ଅଚଳ UIDNA *idna,
                       ଅଚଳ ଅକ୍ଷର *name, int32_t length,
                       ଅକ୍ଷର *dest, int32_t capacity,
                       UIDNAInfo *pInfo, UErrorCode *pErrorCode) {
    ଯଦି(!checkArgs(name, length, dest, capacity, pInfo, pErrorCode)) {
        ଫେରନ୍ତୁ 0;
    }
    StringPiece src(name, length<0 ? static_cast<int32_t>(uprv_strlen(name)) : length);
    ଫେରନ୍ତୁ ByteSinkUtil::viaByteSinkToTerminatedChars(
        dest, capacity,
        [&](ByteSink& sink, UErrorCode& status) {
            IDNAInfo info;
            reinterpret_cast<ଅଚଳ IDNA *>(idna)->nameToASCII_UTF8(src, sink, info, status);
            idnaInfoToStruct(info, pInfo);
        },
        *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uidna_nameToUnicodeUTF8(ଅଚଳ UIDNA *idna,
                        ଅଚଳ ଅକ୍ଷର *name, int32_t length,
                        ଅକ୍ଷର *dest, int32_t capacity,
                        UIDNAInfo *pInfo, UErrorCode *pErrorCode) {
    ଯଦି(!checkArgs(name, length, dest, capacity, pInfo, pErrorCode)) {
        ଫେରନ୍ତୁ 0;
    }
    StringPiece src(name, length<0 ? static_cast<int32_t>(uprv_strlen(name)) : length);
    ଫେରନ୍ତୁ ByteSinkUtil::viaByteSinkToTerminatedChars(
        dest, capacity,
        [&](ByteSink& sink, UErrorCode& status) {
            IDNAInfo info;
            reinterpret_cast<ଅଚଳ IDNA *>(idna)->nameToUnicodeUTF8(src, sink, info, status);
            idnaInfoToStruct(info, pInfo);
        },
        *pErrorCode);
}

#endif  // UCONFIG_NO_IDNA
