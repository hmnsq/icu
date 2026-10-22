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

#ضمن "unicode/utypes.h"

#ܐܢ !UCONFIG_NO_IDNA

#ضمن "unicode/bytestream.h"
#ضمن "unicode/idna.h"
#ضمن "unicode/normalizer2.h"
#ضمن "unicode/uscript.h"
#ضمن "unicode/ustring.h"
#ضمن "unicode/utf16.h"
#ضمن "bytesinkutil.h"
#ضمن "cmemory.h"
#ضمن "cstring.h"
#ضمن "punycode.h"
#ضمن "ubidi_props.h"

// Note about tests for UIDNA_ERROR_DOMAIN_NAME_TOO_LONG:
//
// The domain name length limit is 255 octets in an internal DNS representation
// where the last ("root") label is the empty label
// represented by length byte 0 alone.
// In a conventional string, this translates to 253 characters, or 254
// if there is a trailing dot for the root label.

U_NAMESPACE_BEGIN

// Severe errors which usually result in a U+FFFD replacement character in the result string.
ثابت uint32_t severeErrors=
    UIDNA_ERROR_LEADING_COMBINING_MARK|
    UIDNA_ERROR_DISALLOWED|
    UIDNA_ERROR_PUNYCODE|
    UIDNA_ERROR_LABEL_HAS_DOT|
    UIDNA_ERROR_INVALID_ACE_LABEL;

ساكن مضمن UBool
isASCIIString(ثابت UnicodeString &dest) {
    ثابت char16_t *s=dest.getBuffer();
    ثابت char16_t *limit=s+dest.length();
    ܟܡܐ(s<limit) {
        ܐܢ(*s++>0x7f) {
            ܦܢܐ ܕܓܠܐ;
        }
    }
    ܦܢܐ ܫܪܪܐ;
}

ساكن UBool
isASCIIOkBiDi(ثابت char16_t *s, int32_t length);

ساكن UBool
isASCIIOkBiDi(ثابت حرف *s, int32_t length);

// IDNA class default implementations -------------------------------------- ***

IDNA::~IDNA() {}

فراغ
IDNA::labelToASCII_UTF8(StringPiece label, ByteSink &dest,
                        IDNAInfo &info, UErrorCode &errorCode) ثابت {
    ܐܢ(U_SUCCESS(errorCode)) {
        UnicodeString destString;
        labelToASCII(UnicodeString::fromUTF8(label), destString,
                     info, errorCode).toUTF8(dest);
    }
}

فراغ
IDNA::labelToUnicodeUTF8(StringPiece label, ByteSink &dest,
                         IDNAInfo &info, UErrorCode &errorCode) ثابت {
    ܐܢ(U_SUCCESS(errorCode)) {
        UnicodeString destString;
        labelToUnicode(UnicodeString::fromUTF8(label), destString,
                       info, errorCode).toUTF8(dest);
    }
}

فراغ
IDNA::nameToASCII_UTF8(StringPiece name, ByteSink &dest,
                       IDNAInfo &info, UErrorCode &errorCode) ثابت {
    ܐܢ(U_SUCCESS(errorCode)) {
        UnicodeString destString;
        nameToASCII(UnicodeString::fromUTF8(name), destString,
                    info, errorCode).toUTF8(dest);
    }
}

فراغ
IDNA::nameToUnicodeUTF8(StringPiece name, ByteSink &dest,
                        IDNAInfo &info, UErrorCode &errorCode) ثابت {
    ܐܢ(U_SUCCESS(errorCode)) {
        UnicodeString destString;
        nameToUnicode(UnicodeString::fromUTF8(name), destString,
                      info, errorCode).toUTF8(dest);
    }
}

// UTS46 class declaration ------------------------------------------------- ***

صنف UTS46 : عام IDNA {
عام:
    UTS46(uint32_t options, UErrorCode &errorCode);
    virtual ~UTS46();

    virtual UnicodeString&
    labelToASCII(ثابت UnicodeString& label,
                 UnicodeString& dest U_LIFETIME_BOUND,
                 IDNAInfo& info,
                 UErrorCode& errorCode) ثابت override;

    virtual UnicodeString&
    labelToUnicode(ثابت UnicodeString& label,
                   UnicodeString& dest U_LIFETIME_BOUND,
                   IDNAInfo& info,
                   UErrorCode& errorCode) ثابت override;

    virtual UnicodeString&
    nameToASCII(ثابت UnicodeString& name,
                UnicodeString& dest U_LIFETIME_BOUND,
                IDNAInfo& info,
                UErrorCode& errorCode) ثابت override;

    virtual UnicodeString&
    nameToUnicode(ثابت UnicodeString& name,
                  UnicodeString& dest U_LIFETIME_BOUND,
                  IDNAInfo& info,
                  UErrorCode& errorCode) ثابت override;

    virtual فراغ
    labelToASCII_UTF8(StringPiece label, ByteSink &dest,
                      IDNAInfo &info, UErrorCode &errorCode) ثابت override;

    virtual فراغ
    labelToUnicodeUTF8(StringPiece label, ByteSink &dest,
                       IDNAInfo &info, UErrorCode &errorCode) ثابت override;

    virtual فراغ
    nameToASCII_UTF8(StringPiece name, ByteSink &dest,
                     IDNAInfo &info, UErrorCode &errorCode) ثابت override;

    virtual فراغ
    nameToUnicodeUTF8(StringPiece name, ByteSink &dest,
                      IDNAInfo &info, UErrorCode &errorCode) ثابت override;

خاص:
    UnicodeString&
    process(ثابت UnicodeString& src,
            UBool isLabel,
            UBool toASCII,
            UnicodeString& dest U_LIFETIME_BOUND,
            IDNAInfo& info,
            UErrorCode& errorCode) ثابت;

    فراغ
    processUTF8(StringPiece src,
                UBool isLabel, UBool toASCII,
                ByteSink &dest,
                IDNAInfo &info, UErrorCode &errorCode) ثابت;

    UnicodeString&
    processUnicode(ثابت UnicodeString& src,
                   int32_t labelStart,
                   int32_t mappingStart,
                   UBool isLabel,
                   UBool toASCII,
                   UnicodeString& dest U_LIFETIME_BOUND,
                   IDNAInfo& info,
                   UErrorCode& errorCode) ثابت;

    // returns the new dest.length()
    int32_t
    mapDevChars(UnicodeString &dest, int32_t labelStart, int32_t mappingStart,
                UErrorCode &errorCode) ثابت;

    // returns the new label length
    int32_t
    processLabel(UnicodeString &dest,
                 int32_t labelStart, int32_t labelLength,
                 UBool toASCII,
                 IDNAInfo &info, UErrorCode &errorCode) ثابت;
    int32_t
    markBadACELabel(UnicodeString &dest,
                    int32_t labelStart, int32_t labelLength,
                    UBool toASCII, IDNAInfo &info, UErrorCode &errorCode) ثابت;

    فراغ
    checkLabelBiDi(ثابت char16_t *label, int32_t labelLength, IDNAInfo &info) ثابت;

    UBool
    isLabelOkContextJ(ثابت char16_t *label, int32_t labelLength) ثابت;

    فراغ
    checkLabelContextO(ثابت char16_t *label, int32_t labelLength, IDNAInfo &info) ثابت;

    ثابت Normalizer2 &uts46Norm2;  // uts46.nrm
    uint32_t options;
};

IDNA *
IDNA::createUTS46Instance(uint32_t options, UErrorCode &errorCode) {
    ܐܢ(U_SUCCESS(errorCode)) {
        IDNA *idna=جديد UTS46(options, errorCode);
        ܐܢ(idna==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
        } ܐܚܪܢܐ ܐܢ(U_FAILURE(errorCode)) {
            delete idna;
            idna=nullptr;
        }
        ܦܢܐ idna;
    } ܐܚܪܢܐ {
        ܦܢܐ nullptr;
    }
}

// UTS46 implementation ---------------------------------------------------- ***

UTS46::UTS46(uint32_t opt, UErrorCode &errorCode)
        : uts46Norm2(*Normalizer2::getInstance(nullptr, "uts46", UNORM2_COMPOSE, errorCode)),
          options(opt) {}

UTS46::~UTS46() {}

UnicodeString&
UTS46::labelToASCII(ثابت UnicodeString& label,
                    UnicodeString& dest U_LIFETIME_BOUND,
                    IDNAInfo& info,
                    UErrorCode& errorCode) ثابت {
    ܦܢܐ process(label, ܫܪܪܐ, ܫܪܪܐ, dest, info, errorCode);
}

UnicodeString&
UTS46::labelToUnicode(ثابت UnicodeString& label,
                      UnicodeString& dest U_LIFETIME_BOUND,
                      IDNAInfo& info,
                      UErrorCode& errorCode) ثابت {
    ܦܢܐ process(label, ܫܪܪܐ, ܕܓܠܐ, dest, info, errorCode);
}

UnicodeString&
UTS46::nameToASCII(ثابت UnicodeString& name,
                   UnicodeString& dest U_LIFETIME_BOUND,
                   IDNAInfo& info,
                   UErrorCode& errorCode) ثابت {
    process(name, ܕܓܠܐ, ܫܪܪܐ, dest, info, errorCode);
    ܐܢ( dest.length()>=254 && (info.errors&UIDNA_ERROR_DOMAIN_NAME_TOO_LONG)==0 &&
        isASCIIString(dest) &&
        (dest.length()>254 || dest[253]!=0x2e)
    ) {
        info.errors|=UIDNA_ERROR_DOMAIN_NAME_TOO_LONG;
    }
    ܦܢܐ dest;
}

UnicodeString&
UTS46::nameToUnicode(ثابت UnicodeString& name,
                     UnicodeString& dest U_LIFETIME_BOUND,
                     IDNAInfo& info,
                     UErrorCode& errorCode) ثابت {
    ܦܢܐ process(name, ܕܓܠܐ, ܕܓܠܐ, dest, info, errorCode);
}

فراغ
UTS46::labelToASCII_UTF8(StringPiece label, ByteSink &dest,
                         IDNAInfo &info, UErrorCode &errorCode) ثابت {
    processUTF8(label, ܫܪܪܐ, ܫܪܪܐ, dest, info, errorCode);
}

فراغ
UTS46::labelToUnicodeUTF8(StringPiece label, ByteSink &dest,
                          IDNAInfo &info, UErrorCode &errorCode) ثابت {
    processUTF8(label, ܫܪܪܐ, ܕܓܠܐ, dest, info, errorCode);
}

فراغ
UTS46::nameToASCII_UTF8(StringPiece name, ByteSink &dest,
                        IDNAInfo &info, UErrorCode &errorCode) ثابت {
    processUTF8(name, ܕܓܠܐ, ܫܪܪܐ, dest, info, errorCode);
}

فراغ
UTS46::nameToUnicodeUTF8(StringPiece name, ByteSink &dest,
                         IDNAInfo &info, UErrorCode &errorCode) ثابت {
    processUTF8(name, ܕܓܠܐ, ܕܓܠܐ, dest, info, errorCode);
}

// UTS #46 data for ASCII characters.
// The normalizer (using uts46.nrm) maps uppercase ASCII letters to lowercase
// and passes through all other ASCII characters.
// If UIDNA_USE_STD3_RULES is set, then non-LDH characters are disallowed
// using this data.
// The ASCII fastpath also uses this data.
// Values: -1=disallowed  0==valid  1==mapped (lowercase)
ساكن ثابت int8_t asciiData[128]={
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
UTS46::process(ثابت UnicodeString& src,
               UBool isLabel,
               UBool toASCII,
               UnicodeString& dest U_LIFETIME_BOUND,
               IDNAInfo& info,
               UErrorCode& errorCode) ثابت {
    // uts46Norm2.normalize() would do all of this error checking and setup,
    // but with the ASCII fastpath we do not always call it, and do not
    // call it first.
    ܐܢ(U_FAILURE(errorCode)) {
        dest.setToBogus();
        ܦܢܐ dest;
    }
    ثابت char16_t *srcArray=src.getBuffer();
    ܐܢ(&dest==&src || srcArray==nullptr) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        dest.setToBogus();
        ܦܢܐ dest;
    }
    // Arguments are fine, reset output values.
    dest.remove();
    info.reset();
    int32_t srcLength=src.length();
    ܐܢ(srcLength==0) {
        info.errors|=UIDNA_ERROR_EMPTY_LABEL;
        ܦܢܐ dest;
    }
    char16_t *destArray=dest.getBuffer(srcLength);
    ܐܢ(destArray==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        ܦܢܐ dest;
    }
    // ASCII fastpath
    UBool disallowNonLDHDot=(options&UIDNA_USE_STD3_RULES)!=0;
    int32_t labelStart=0;
    int32_t i;
    لكل(i=0;; ++i) {
        ܐܢ(i==srcLength) {
            ܐܢ(toASCII) {
                ܐܢ((i-labelStart)>63) {
                    info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
                }
                // There is a trailing dot if labelStart==i.
                ܐܢ(!isLabel && i>=254 && (i>254 || labelStart<i)) {
                    info.errors|=UIDNA_ERROR_DOMAIN_NAME_TOO_LONG;
                }
            }
            info.errors|=info.labelErrors;
            dest.releaseBuffer(i);
            ܦܢܐ dest;
        }
        char16_t c=srcArray[i];
        ܐܢ(c>0x7f) {
            اكسر;
        }
        صحيح cData=asciiData[c];
        ܐܢ(cData>0) {
            destArray[i]=c+0x20;  // Lowercase an uppercase ASCII letter.
        } ܐܚܪܢܐ ܐܢ(cData<0 && disallowNonLDHDot) {
            اكسر;  // Replacing with U+FFFD can be complicated for toASCII.
        } ܐܚܪܢܐ {
            destArray[i]=c;
            ܐܢ(c==0x2d) {  // hyphen
                ܐܢ(i==(labelStart+3) && srcArray[i-1]==0x2d) {
                    // "??--..." is Punycode or forbidden.
                    ++i;  // '-' was copied to dest already
                    اكسر;
                }
                ܐܢ(i==labelStart) {
                    // label starts with "-"
                    info.labelErrors|=UIDNA_ERROR_LEADING_HYPHEN;
                }
                ܐܢ((i+1)==srcLength || srcArray[i+1]==0x2e) {
                    // label ends with "-"
                    info.labelErrors|=UIDNA_ERROR_TRAILING_HYPHEN;
                }
            } ܐܚܪܢܐ ܐܢ(c==0x2e) {  // dot
                ܐܢ(isLabel) {
                    // Replacing with U+FFFD can be complicated for toASCII.
                    ++i;  // '.' was copied to dest already
                    اكسر;
                }
                ܐܢ(i==labelStart) {
                    info.labelErrors|=UIDNA_ERROR_EMPTY_LABEL;
                }
                ܐܢ(toASCII && (i-labelStart)>63) {
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
    ܐܢ( info.isBiDi && U_SUCCESS(errorCode) && (info.errors&severeErrors)==0 &&
        (!info.isOkBiDi || (labelStart>0 && !isASCIIOkBiDi(dest.getBuffer(), labelStart)))
    ) {
        info.errors|=UIDNA_ERROR_BIDI;
    }
    ܦܢܐ dest;
}

فراغ
UTS46::processUTF8(StringPiece src,
                   UBool isLabel, UBool toASCII,
                   ByteSink &dest,
                   IDNAInfo &info, UErrorCode &errorCode) ثابت {
    ܐܢ(U_FAILURE(errorCode)) {
        ܦܢܐ;
    }
    ثابت حرف *srcArray=src.data();
    int32_t srcLength=src.length();
    ܐܢ(srcArray==nullptr && srcLength!=0) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ܦܢܐ;
    }
    // Arguments are fine, reset output values.
    info.reset();
    ܐܢ(srcLength==0) {
        info.errors|=UIDNA_ERROR_EMPTY_LABEL;
        dest.Flush();
        ܦܢܐ;
    }
    UnicodeString destString;
    int32_t labelStart=0;
    ܐܢ(srcLength<=256) {  // length of stackArray[]
        // ASCII fastpath
        حرف stackArray[256];
        int32_t destCapacity;
        حرف *destArray=dest.GetAppendBuffer(srcLength, srcLength+20,
                                             stackArray, UPRV_LENGTHOF(stackArray), &destCapacity);
        UBool disallowNonLDHDot=(options&UIDNA_USE_STD3_RULES)!=0;
        int32_t i;
        لكل(i=0;; ++i) {
            ܐܢ(i==srcLength) {
                ܐܢ(toASCII) {
                    ܐܢ((i-labelStart)>63) {
                        info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
                    }
                    // There is a trailing dot if labelStart==i.
                    ܐܢ(!isLabel && i>=254 && (i>254 || labelStart<i)) {
                        info.errors|=UIDNA_ERROR_DOMAIN_NAME_TOO_LONG;
                    }
                }
                info.errors|=info.labelErrors;
                dest.Append(destArray, i);
                dest.Flush();
                ܦܢܐ;
            }
            حرف c=srcArray[i];
            ܐܢ (static_cast<int8_t>(c) < 0) { // (uint8_t)c>0x7f
                اكسر;
            }
            صحيح cData = asciiData[static_cast<صحيح>(c)]; // Cast: gcc warns about indexing with a char.
            ܐܢ(cData>0) {
                destArray[i]=c+0x20;  // Lowercase an uppercase ASCII letter.
            } ܐܚܪܢܐ ܐܢ(cData<0 && disallowNonLDHDot) {
                اكسر;  // Replacing with U+FFFD can be complicated for toASCII.
            } ܐܚܪܢܐ {
                destArray[i]=c;
                ܐܢ(c==0x2d) {  // hyphen
                    ܐܢ(i==(labelStart+3) && srcArray[i-1]==0x2d) {
                        // "??--..." is Punycode or forbidden.
                        اكسر;
                    }
                    ܐܢ(i==labelStart) {
                        // label starts with "-"
                        info.labelErrors|=UIDNA_ERROR_LEADING_HYPHEN;
                    }
                    ܐܢ((i+1)==srcLength || srcArray[i+1]==0x2e) {
                        // label ends with "-"
                        info.labelErrors|=UIDNA_ERROR_TRAILING_HYPHEN;
                    }
                } ܐܚܪܢܐ ܐܢ(c==0x2e) {  // dot
                    ܐܢ(isLabel) {
                        اكسر;  // Replacing with U+FFFD can be complicated for toASCII.
                    }
                    ܐܢ(i==labelStart) {
                        info.labelErrors|=UIDNA_ERROR_EMPTY_LABEL;
                    }
                    ܐܢ(toASCII && (i-labelStart)>63) {
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
    } ܐܚܪܢܐ {
        // src is too long for the ASCII fastpath implementation.
        processUnicode(UnicodeString::fromUTF8(src), 0, 0,
                       isLabel, toASCII,
                       destString, info, errorCode);
    }
    destString.toUTF8(dest);  // calls dest.Flush()
    ܐܢ(toASCII && !isLabel) {
        // length==labelStart==254 means that there is a trailing dot (ok) and
        // destString is empty (do not index at 253-labelStart).
        int32_t length=labelStart+destString.length();
        ܐܢ( length>=254 && isASCIIString(destString) &&
            (length>254 ||
             (labelStart<254 && destString[253-labelStart]!=0x2e))
        ) {
            info.errors|=UIDNA_ERROR_DOMAIN_NAME_TOO_LONG;
        }
    }
    ܐܢ( info.isBiDi && U_SUCCESS(errorCode) && (info.errors&severeErrors)==0 &&
        (!info.isOkBiDi || (labelStart>0 && !isASCIIOkBiDi(srcArray, labelStart)))
    ) {
        info.errors|=UIDNA_ERROR_BIDI;
    }
}

UnicodeString&
UTS46::processUnicode(ثابت UnicodeString& src,
                      int32_t labelStart,
                      int32_t mappingStart,
                      UBool isLabel,
                      UBool toASCII,
                      UnicodeString& dest U_LIFETIME_BOUND,
                      IDNAInfo& info,
                      UErrorCode& errorCode) ثابت {
    ܐܢ(mappingStart==0) {
        uts46Norm2.normalize(src, dest, errorCode);
    } ܐܚܪܢܐ {
        uts46Norm2.normalizeSecondAndAppend(dest, src.tempSubString(mappingStart), errorCode);
    }
    ܐܢ(U_FAILURE(errorCode)) {
        ܦܢܐ dest;
    }
    UBool doMapDevChars=
        toASCII ? (options&UIDNA_NONTRANSITIONAL_TO_ASCII)==0 :
                  (options&UIDNA_NONTRANSITIONAL_TO_UNICODE)==0;
    ثابت char16_t *destArray=dest.getBuffer();
    int32_t destLength=dest.length();
    int32_t labelLimit=labelStart;
    ܟܡܐ(labelLimit<destLength) {
        char16_t c=destArray[labelLimit];
        ܐܢ(c==0x2e && !isLabel) {
            int32_t labelLength=labelLimit-labelStart;
            int32_t newLength=processLabel(dest, labelStart, labelLength,
                                            toASCII, info, errorCode);
            info.errors|=info.labelErrors;
            info.labelErrors=0;
            ܐܢ(U_FAILURE(errorCode)) {
                ܦܢܐ dest;
            }
            destArray=dest.getBuffer();
            destLength+=newLength-labelLength;
            labelLimit=labelStart+=newLength+1;
            استمر;
        } ܐܚܪܢܐ ܐܢ(c<0xdf) {
            // pass
        } ܐܚܪܢܐ ܐܢ(c<=0x200d && (c==0xdf || c==0x3c2 || c>=0x200c)) {
            info.isTransDiff=ܫܪܪܐ;
            ܐܢ(doMapDevChars) {
                destLength=mapDevChars(dest, labelStart, labelLimit, errorCode);
                ܐܢ(U_FAILURE(errorCode)) {
                    ܦܢܐ dest;
                }
                destArray=dest.getBuffer();
                // All deviation characters have been mapped, no need to check for them again.
                doMapDevChars=ܕܓܠܐ;
                // Do not increment labelLimit in case c was removed.
                استمر;
            }
        } ܐܚܪܢܐ ܐܢ(U16_IS_SURROGATE(c)) {
            ܐܢ(U16_IS_SURROGATE_LEAD(c) ?
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
    ܐܢ(0==labelStart || labelStart<labelLimit) {
        processLabel(dest, labelStart, labelLimit-labelStart,
                      toASCII, info, errorCode);
        info.errors|=info.labelErrors;
    }
    ܦܢܐ dest;
}

int32_t
UTS46::mapDevChars(UnicodeString &dest, int32_t labelStart, int32_t mappingStart,
                   UErrorCode &errorCode) ثابت {
    ܐܢ(U_FAILURE(errorCode)) {
        ܦܢܐ 0;
    }
    int32_t length=dest.length();
    char16_t *s=dest.getBuffer(dest[mappingStart]==0xdf ? length+1 : length);
    ܐܢ(s==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        ܦܢܐ length;
    }
    int32_t capacity=dest.getCapacity();
    UBool didMapDevChars=ܕܓܠܐ;
    int32_t readIndex=mappingStart, writeIndex=mappingStart;
    ܥܒܕ {
        char16_t c=s[readIndex++];
        بدل(c) {
        حالة 0xdf:
            // Map sharp s to ss.
            didMapDevChars=ܫܪܪܐ;
            s[writeIndex++]=0x73;  // Replace sharp s with first s.
            // Insert second s and account for possible buffer reallocation.
            ܐܢ(writeIndex==readIndex) {
                ܐܢ(length==capacity) {
                    dest.releaseBuffer(length);
                    s=dest.getBuffer(length+1);
                    ܐܢ(s==nullptr) {
                        errorCode=U_MEMORY_ALLOCATION_ERROR;
                        ܦܢܐ length;
                    }
                    capacity=dest.getCapacity();
                }
                u_memmove(s+writeIndex+1, s+writeIndex, length-writeIndex);
                ++readIndex;
            }
            s[writeIndex++]=0x73;
            ++length;
            اكسر;
        حالة 0x3c2:  // Map final sigma to nonfinal sigma.
            didMapDevChars=ܫܪܪܐ;
            s[writeIndex++]=0x3c3;
            اكسر;
        حالة 0x200c:  // Ignore/remove ZWNJ.
        حالة 0x200d:  // Ignore/remove ZWJ.
            didMapDevChars=ܫܪܪܐ;
            --length;
            اكسر;
        افتراضي:
            // Only really necessary if writeIndex was different from readIndex.
            s[writeIndex++]=c;
            اكسر;
        }
    } ܟܡܐ(writeIndex<length);
    dest.releaseBuffer(length);
    ܐܢ(didMapDevChars) {
        // Mapping deviation characters might have resulted in an un-NFC string.
        // We could use either the NFC or the UTS #46 normalizer.
        // By using the UTS #46 normalizer again, we avoid having to load a second .nrm data file.
        UnicodeString normalized;
        uts46Norm2.normalize(dest.tempSubString(labelStart), normalized, errorCode);
        ܐܢ(U_SUCCESS(errorCode)) {
            dest.replace(labelStart, 0x7fffffff, normalized);
            ܐܢ(dest.isBogus()) {
                errorCode=U_MEMORY_ALLOCATION_ERROR;
            }
            ܦܢܐ dest.length();
        }
    }
    ܦܢܐ length;
}

// Replace the label in dest with the label string, if the label was modified.
// If &label==&dest then the label was modified in-place and labelLength
// is the new label length, different from label.length().
// If &label!=&dest then labelLength==label.length().
// Returns labelLength (= the new label length).
ساكن int32_t
replaceLabel(UnicodeString &dest, int32_t destLabelStart, int32_t destLabelLength,
             ثابت UnicodeString &label, int32_t labelLength, UErrorCode &errorCode) {
    ܐܢ(U_FAILURE(errorCode)) {
        ܦܢܐ 0;
    }
    ܐܢ(&label!=&dest) {
        dest.replace(destLabelStart, destLabelLength, label);
        ܐܢ(dest.isBogus()) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            ܦܢܐ 0;
        }
    }
    ܦܢܐ labelLength;
}

int32_t
UTS46::processLabel(UnicodeString &dest,
                    int32_t labelStart, int32_t labelLength,
                    UBool toASCII,
                    IDNAInfo &info, UErrorCode &errorCode) ثابت {
    ܐܢ(U_FAILURE(errorCode)) {
        ܦܢܐ 0;
    }
    UnicodeString fromPunycode;
    UnicodeString *labelString;
    ثابت char16_t *label=dest.getBuffer()+labelStart;
    int32_t destLabelStart=labelStart;
    int32_t destLabelLength=labelLength;
    UBool wasPunycode;
    ܐܢ(labelLength>=4 && label[0]==0x78 && label[1]==0x6e && label[2]==0x2d && label[3]==0x2d) {
        // Label starts with "xn--", try to un-Punycode it.
        // In IDNA2008, labels like "xn--" (decodes to an empty string) and
        // "xn--ASCII-" (decodes to just "ASCII") fail the round-trip validation from
        // comparing the ToUnicode input with the back-to-ToASCII output.
        // They are alternate encodings of the respective ASCII labels.
        // Ignore "xn---" here: It will fail Punycode.decode() which logically comes before
        // the round-trip verification.
        ܐܢ(labelLength==4 || (labelLength>5 && label[labelLength-1]==u'-')) {
            info.labelErrors|=UIDNA_ERROR_INVALID_ACE_LABEL;
            ܦܢܐ markBadACELabel(dest, labelStart, labelLength, toASCII, info, errorCode);
        }
        wasPunycode=ܫܪܪܐ;
        char16_t *unicodeBuffer=fromPunycode.getBuffer(-1);  // capacity==-1: most labels should fit
        ܐܢ(unicodeBuffer==nullptr) {
            // Should never occur if we used capacity==-1 which uses the internal buffer.
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            ܦܢܐ labelLength;
        }
        UErrorCode punycodeErrorCode=U_ZERO_ERROR;
        int32_t unicodeLength=u_strFromPunycode(label+4, labelLength-4,
                                                unicodeBuffer, fromPunycode.getCapacity(),
                                                nullptr, &punycodeErrorCode);
        ܐܢ(punycodeErrorCode==U_BUFFER_OVERFLOW_ERROR) {
            fromPunycode.releaseBuffer(0);
            unicodeBuffer=fromPunycode.getBuffer(unicodeLength);
            ܐܢ(unicodeBuffer==nullptr) {
                errorCode=U_MEMORY_ALLOCATION_ERROR;
                ܦܢܐ labelLength;
            }
            punycodeErrorCode=U_ZERO_ERROR;
            unicodeLength=u_strFromPunycode(label+4, labelLength-4,
                                            unicodeBuffer, fromPunycode.getCapacity(),
                                            nullptr, &punycodeErrorCode);
        }
        fromPunycode.releaseBuffer(unicodeLength);
        ܐܢ(U_FAILURE(punycodeErrorCode)) {
            info.labelErrors|=UIDNA_ERROR_PUNYCODE;
            ܦܢܐ markBadACELabel(dest, labelStart, labelLength, toASCII, info, errorCode);
        }
        // Check for NFC, and for characters that are not
        // valid or deviation characters according to the normalizer.
        // If there is something wrong, then the string will change.
        // Note that the normalizer passes through non-LDH ASCII and deviation characters.
        // Deviation characters are ok in Punycode even in transitional processing.
        // In the code further below, if we find non-LDH ASCII and we have UIDNA_USE_STD3_RULES
        // then we will set UIDNA_ERROR_INVALID_ACE_LABEL there too.
        UBool isValid=uts46Norm2.isNormalized(fromPunycode, errorCode);
        ܐܢ(U_FAILURE(errorCode)) {
            ܦܢܐ labelLength;
        }
        // Unicode 15.1 UTS #46:
        // Added an additional condition in 4.1 Validity Criteria to
        // disallow labels such as xn--xn---epa., which do not round-trip.
        // --> Validity Criteria new criterion 4:
        // If not CheckHyphens, the label must not begin with “xn--”.
        ܐܢ(!isValid || fromPunycode.startsWith(UnicodeString::readOnlyAlias(u"xn--"))) {
            info.labelErrors|=UIDNA_ERROR_INVALID_ACE_LABEL;
            ܦܢܐ markBadACELabel(dest, labelStart, labelLength, toASCII, info, errorCode);
        }
        labelString=&fromPunycode;
        label=fromPunycode.getBuffer();
        labelStart=0;
        labelLength=fromPunycode.length();
    } ܐܚܪܢܐ {
        wasPunycode=ܕܓܠܐ;
        labelString=&dest;
    }
    // Validity check
    ܐܢ(labelLength==0) {
        info.labelErrors|=UIDNA_ERROR_EMPTY_LABEL;
        ܦܢܐ replaceLabel(dest, destLabelStart, destLabelLength,
                            *labelString, labelLength, errorCode);
    }
    // labelLength>0
    ܐܢ(labelLength>=4 && label[2]==0x2d && label[3]==0x2d) {
        // label starts with "??--"
        info.labelErrors|=UIDNA_ERROR_HYPHEN_3_4;
    }
    ܐܢ(label[0]==0x2d) {
        // label starts with "-"
        info.labelErrors|=UIDNA_ERROR_LEADING_HYPHEN;
    }
    ܐܢ(label[labelLength-1]==0x2d) {
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
    ثابت char16_t *limit=label+labelLength;
    char16_t oredChars=0;
    // If we enforce STD3 rules, then ASCII characters other than LDH and dot are disallowed.
    UBool disallowNonLDHDot=(options&UIDNA_USE_STD3_RULES)!=0;
    ܥܒܕ {
        char16_t c=*s;
        ܐܢ(c<=0x7f) {
            ܐܢ(c==0x2e) {
                info.labelErrors|=UIDNA_ERROR_LABEL_HAS_DOT;
                *s=0xfffd;
            } ܐܚܪܢܐ ܐܢ(disallowNonLDHDot && asciiData[c]<0) {
                info.labelErrors|=UIDNA_ERROR_DISALLOWED;
                *s=0xfffd;
            }
        } ܐܚܪܢܐ {
            oredChars|=c;
            ܐܢ(c==0xfffd) {
                info.labelErrors|=UIDNA_ERROR_DISALLOWED;
            }
        }
        ++s;
    } ܟܡܐ(s<limit);
    // Check for a leading combining mark after other validity checks
    // so that we don't report UIDNA_ERROR_DISALLOWED for the U+FFFD from here.
    UChar32 c;
    int32_t cpLength=0;
    // "Unsafe" is ok because unpaired surrogates were mapped to U+FFFD.
    U16_NEXT_UNSAFE(label, cpLength, c);
    ܐܢ((U_GET_GC_MASK(c)&U_GC_M_MASK)!=0) {
        info.labelErrors|=UIDNA_ERROR_LEADING_COMBINING_MARK;
        labelString->replace(labelStart, cpLength, static_cast<char16_t>(0xfffd));
        label=labelString->getBuffer()+labelStart;
        labelLength+=1-cpLength;
        ܐܢ(labelString==&dest) {
            destLabelLength=labelLength;
        }
    }
    ܐܢ((info.labelErrors&severeErrors)==0) {
        // Do contextual checks only if we do not have U+FFFD from a severe error
        // because U+FFFD can make these checks fail.
        ܐܢ((options&UIDNA_CHECK_BIDI)!=0 && (!info.isBiDi || info.isOkBiDi)) {
            checkLabelBiDi(label, labelLength, info);
        }
        ܐܢ( (options&UIDNA_CHECK_CONTEXTJ)!=0 && (oredChars&0x200c)==0x200c &&
            !isLabelOkContextJ(label, labelLength)
        ) {
            info.labelErrors|=UIDNA_ERROR_CONTEXTJ;
        }
        ܐܢ((options&UIDNA_CHECK_CONTEXTO)!=0 && oredChars>=0xb7) {
            checkLabelContextO(label, labelLength, info);
        }
        ܐܢ(toASCII) {
            ܐܢ(wasPunycode) {
                // Leave a Punycode label unchanged if it has no severe errors.
                ܐܢ(destLabelLength>63) {
                    info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
                }
                ܦܢܐ destLabelLength;
            } ܐܚܪܢܐ ܐܢ(oredChars>=0x80) {
                // Contains non-ASCII characters.
                UnicodeString punycode;
                char16_t *buffer=punycode.getBuffer(63);  // 63==maximum DNS label length
                ܐܢ(buffer==nullptr) {
                    errorCode=U_MEMORY_ALLOCATION_ERROR;
                    ܦܢܐ destLabelLength;
                }
                buffer[0]=0x78;  // Write "xn--".
                buffer[1]=0x6e;
                buffer[2]=0x2d;
                buffer[3]=0x2d;
                UErrorCode punycodeErrorCode=U_ZERO_ERROR;
                int32_t punycodeLength=u_strToPunycode(label, labelLength,
                                                      buffer+4, punycode.getCapacity()-4,
                                                      nullptr, &punycodeErrorCode);
                ܐܢ(punycodeErrorCode==U_BUFFER_OVERFLOW_ERROR) {
                    punycodeErrorCode=U_ZERO_ERROR;
                    punycode.releaseBuffer(4);
                    buffer=punycode.getBuffer(4+punycodeLength);
                    ܐܢ(buffer==nullptr) {
                        errorCode=U_MEMORY_ALLOCATION_ERROR;
                        ܦܢܐ destLabelLength;
                    }
                    punycodeLength=u_strToPunycode(label, labelLength,
                                                  buffer+4, punycode.getCapacity()-4,
                                                  nullptr, &punycodeErrorCode);
                }
                punycodeLength+=4;
                punycode.releaseBuffer(punycodeLength);
                ܐܢ(U_FAILURE(punycodeErrorCode)) {
                    errorCode = punycodeErrorCode;
                    ܦܢܐ destLabelLength;
                }
                ܐܢ(punycodeLength>63) {
                    info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
                }
                ܦܢܐ replaceLabel(dest, destLabelStart, destLabelLength,
                                    punycode, punycodeLength, errorCode);
            } ܐܚܪܢܐ {
                // all-ASCII label
                ܐܢ(labelLength>63) {
                    info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
                }
            }
        }
    } ܐܚܪܢܐ {
        // If a Punycode label has severe errors,
        // then leave it but make sure it does not look valid.
        ܐܢ(wasPunycode) {
            info.labelErrors|=UIDNA_ERROR_INVALID_ACE_LABEL;
            ܦܢܐ markBadACELabel(dest, destLabelStart, destLabelLength, toASCII, info, errorCode);
        }
    }
    ܦܢܐ replaceLabel(dest, destLabelStart, destLabelLength,
                        *labelString, labelLength, errorCode);
}

// Make sure an ACE label does not look valid.
// Append U+FFFD if the label has only LDH characters.
// If UIDNA_USE_STD3_RULES, also replace disallowed ASCII characters with U+FFFD.
int32_t
UTS46::markBadACELabel(UnicodeString &dest,
                       int32_t labelStart, int32_t labelLength,
                       UBool toASCII, IDNAInfo &info, UErrorCode &errorCode) ثابت {
    ܐܢ(U_FAILURE(errorCode)) {
        ܦܢܐ 0;
    }
    UBool disallowNonLDHDot=(options&UIDNA_USE_STD3_RULES)!=0;
    UBool isASCII=ܫܪܪܐ;
    UBool onlyLDH=ܫܪܪܐ;
    ثابت char16_t *label=dest.getBuffer()+labelStart;
    ثابت char16_t *limit=label+labelLength;
    // Start after the initial "xn--".
    // Ok to cast away const because we own the UnicodeString.
    لكل(char16_t *s=const_cast<char16_t *>(label+4); s<limit; ++s) {
        char16_t c=*s;
        ܐܢ(c<=0x7f) {
            ܐܢ(c==0x2e) {
                info.labelErrors|=UIDNA_ERROR_LABEL_HAS_DOT;
                *s=0xfffd;
                isASCII=onlyLDH=ܕܓܠܐ;
            } ܐܚܪܢܐ ܐܢ(asciiData[c]<0) {
                onlyLDH=ܕܓܠܐ;
                ܐܢ(disallowNonLDHDot) {
                    *s=0xfffd;
                    isASCII=ܕܓܠܐ;
                }
            }
        } ܐܚܪܢܐ {
            isASCII=onlyLDH=ܕܓܠܐ;
        }
    }
    ܐܢ(onlyLDH) {
        dest.insert(labelStart + labelLength, static_cast<char16_t>(0xfffd));
        ܐܢ(dest.isBogus()) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            ܦܢܐ 0;
        }
        ++labelLength;
    } ܐܚܪܢܐ {
        ܐܢ(toASCII && isASCII && labelLength>63) {
            info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
        }
    }
    ܦܢܐ labelLength;
}

ثابت uint32_t L_MASK=U_MASK(U_LEFT_TO_RIGHT);
ثابت uint32_t R_AL_MASK=U_MASK(U_RIGHT_TO_LEFT)|U_MASK(U_RIGHT_TO_LEFT_ARABIC);
ثابت uint32_t L_R_AL_MASK=L_MASK|R_AL_MASK;

ثابت uint32_t R_AL_AN_MASK=R_AL_MASK|U_MASK(U_ARABIC_NUMBER);

ثابت uint32_t EN_AN_MASK=U_MASK(U_EUROPEAN_NUMBER)|U_MASK(U_ARABIC_NUMBER);
ثابت uint32_t R_AL_EN_AN_MASK=R_AL_MASK|EN_AN_MASK;
ثابت uint32_t L_EN_MASK=L_MASK|U_MASK(U_EUROPEAN_NUMBER);

ثابت uint32_t ES_CS_ET_ON_BN_NSM_MASK=
    U_MASK(U_EUROPEAN_NUMBER_SEPARATOR)|
    U_MASK(U_COMMON_NUMBER_SEPARATOR)|
    U_MASK(U_EUROPEAN_NUMBER_TERMINATOR)|
    U_MASK(U_OTHER_NEUTRAL)|
    U_MASK(U_BOUNDARY_NEUTRAL)|
    U_MASK(U_DIR_NON_SPACING_MARK);
ثابت uint32_t L_EN_ES_CS_ET_ON_BN_NSM_MASK=L_EN_MASK|ES_CS_ET_ON_BN_NSM_MASK;
ثابت uint32_t R_AL_AN_EN_ES_CS_ET_ON_BN_NSM_MASK=R_AL_MASK|EN_AN_MASK|ES_CS_ET_ON_BN_NSM_MASK;

// We scan the whole label and check both for whether it contains RTL characters
// and whether it passes the BiDi Rule.
// In a BiDi domain name, all labels must pass the BiDi Rule, but we might find
// that a domain name is a BiDi domain name (has an RTL label) only after
// processing several earlier labels.
فراغ
UTS46::checkLabelBiDi(ثابت char16_t *label, int32_t labelLength, IDNAInfo &info) ثابت {
    // IDNA2008 BiDi rule
    // Get the directionality of the first character.
    UChar32 c;
    int32_t i=0;
    U16_NEXT_UNSAFE(label, i, c);
    uint32_t firstMask=U_MASK(u_charDirection(c));
    // 1. The first character must be a character with BIDI property L, R
    // or AL.  If it has the R or AL property, it is an RTL label; if it
    // has the L property, it is an LTR label.
    ܐܢ((firstMask&~L_R_AL_MASK)!=0) {
        info.isOkBiDi=ܕܓܠܐ;
    }
    // Get the directionality of the last non-NSM character.
    uint32_t lastMask;
    لكل(;;) {
        ܐܢ(i>=labelLength) {
            lastMask=firstMask;
            اكسر;
        }
        U16_PREV_UNSAFE(label, labelLength, c);
        UCharDirection dir=u_charDirection(c);
        ܐܢ(dir!=U_DIR_NON_SPACING_MARK) {
            lastMask=U_MASK(dir);
            اكسر;
        }
    }
    // 3. In an RTL label, the end of the label must be a character with
    // BIDI property R, AL, EN or AN, followed by zero or more
    // characters with BIDI property NSM.
    // 6. In an LTR label, the end of the label must be a character with
    // BIDI property L or EN, followed by zero or more characters with
    // BIDI property NSM.
    ܐܢ( (firstMask&L_MASK)!=0 ?
            (lastMask&~L_EN_MASK)!=0 :
            (lastMask&~R_AL_EN_AN_MASK)!=0
    ) {
        info.isOkBiDi=ܕܓܠܐ;
    }
    // Add the directionalities of the intervening characters.
    uint32_t mask=firstMask|lastMask;
    ܟܡܐ(i<labelLength) {
        U16_NEXT_UNSAFE(label, i, c);
        mask|=U_MASK(u_charDirection(c));
    }
    ܐܢ(firstMask&L_MASK) {
        // 5. In an LTR label, only characters with the BIDI properties L, EN,
        // ES, CS, ET, ON, BN and NSM are allowed.
        ܐܢ((mask&~L_EN_ES_CS_ET_ON_BN_NSM_MASK)!=0) {
            info.isOkBiDi=ܕܓܠܐ;
        }
    } ܐܚܪܢܐ {
        // 2. In an RTL label, only characters with the BIDI properties R, AL,
        // AN, EN, ES, CS, ET, ON, BN and NSM are allowed.
        ܐܢ((mask&~R_AL_AN_EN_ES_CS_ET_ON_BN_NSM_MASK)!=0) {
            info.isOkBiDi=ܕܓܠܐ;
        }
        // 4. In an RTL label, if an EN is present, no AN may be present, and
        // vice versa.
        ܐܢ((mask&EN_AN_MASK)==EN_AN_MASK) {
            info.isOkBiDi=ܕܓܠܐ;
        }
    }
    // An RTL label is a label that contains at least one character of type
    // R, AL or AN. [...]
    // A "BIDI domain name" is a domain name that contains at least one RTL
    // label. [...]
    // The following rule, consisting of six conditions, applies to labels
    // in BIDI domain names.
    ܐܢ((mask&R_AL_AN_MASK)!=0) {
        info.isBiDi=ܫܪܪܐ;
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
ساكن UBool
isASCIIOkBiDi(ثابت char16_t *s, int32_t length) {
    int32_t labelStart=0;
    لكل(int32_t i=0; i<length; ++i) {
        char16_t c=s[i];
        ܐܢ(c==0x2e) {  // dot
            ܐܢ(i>labelStart) {
                c=s[i-1];
                ܐܢ(!(0x61<=c && c<=0x7a) && !(0x30<=c && c<=0x39)) {
                    // Last character in the label is not an L or EN.
                    ܦܢܐ ܕܓܠܐ;
                }
            }
            labelStart=i+1;
        } ܐܚܪܢܐ ܐܢ(i==labelStart) {
            ܐܢ(!(0x61<=c && c<=0x7a)) {
                // First character in the label is not an L.
                ܦܢܐ ܕܓܠܐ;
            }
        } ܐܚܪܢܐ {
            ܐܢ(c<=0x20 && (c>=0x1c || (9<=c && c<=0xd))) {
                // Intermediate character in the label is a B, S or WS.
                ܦܢܐ ܕܓܠܐ;
            }
        }
    }
    ܦܢܐ ܫܪܪܐ;
}

// UTF-8 version, called for source ASCII prefix.
// Can contain uppercase A-Z.
// s[length-1] must be the trailing dot.
ساكن UBool
isASCIIOkBiDi(ثابت حرف *s, int32_t length) {
    int32_t labelStart=0;
    لكل(int32_t i=0; i<length; ++i) {
        حرف c=s[i];
        ܐܢ(c==0x2e) {  // dot
            ܐܢ(i>labelStart) {
                c=s[i-1];
                ܐܢ(!(0x61<=c && c<=0x7a) && !(0x41<=c && c<=0x5a) && !(0x30<=c && c<=0x39)) {
                    // Last character in the label is not an L or EN.
                    ܦܢܐ ܕܓܠܐ;
                }
            }
            labelStart=i+1;
        } ܐܚܪܢܐ ܐܢ(i==labelStart) {
            ܐܢ(!(0x61<=c && c<=0x7a) && !(0x41<=c && c<=0x5a)) {
                // First character in the label is not an L.
                ܦܢܐ ܕܓܠܐ;
            }
        } ܐܚܪܢܐ {
            ܐܢ(c<=0x20 && (c>=0x1c || (9<=c && c<=0xd))) {
                // Intermediate character in the label is a B, S or WS.
                ܦܢܐ ܕܓܠܐ;
            }
        }
    }
    ܦܢܐ ܫܪܪܐ;
}

UBool
UTS46::isLabelOkContextJ(ثابت char16_t *label, int32_t labelLength) ثابت {
    // [IDNA2008-Tables]
    // 200C..200D  ; CONTEXTJ    # ZERO WIDTH NON-JOINER..ZERO WIDTH JOINER
    لكل(int32_t i=0; i<labelLength; ++i) {
        ܐܢ(label[i]==0x200c) {
            // Appendix A.1. ZERO WIDTH NON-JOINER
            // Rule Set:
            //  False;
            //  If Canonical_Combining_Class(Before(cp)) .eq.  Virama Then True;
            //  If RegExpMatch((Joining_Type:{L,D})(Joining_Type:T)*\u200C
            //     (Joining_Type:T)*(Joining_Type:{R,D})) Then True;
            ܐܢ(i==0) {
                ܦܢܐ ܕܓܠܐ;
            }
            UChar32 c;
            int32_t j=i;
            U16_PREV_UNSAFE(label, j, c);
            ܐܢ(uts46Norm2.getCombiningClass(c)==9) {
                استمر;
            }
            // check precontext (Joining_Type:{L,D})(Joining_Type:T)*
            لكل(;;) {
                UJoiningType type=ubidi_getJoiningType(c);
                ܐܢ(type==U_JT_TRANSPARENT) {
                    ܐܢ(j==0) {
                        ܦܢܐ ܕܓܠܐ;
                    }
                    U16_PREV_UNSAFE(label, j, c);
                } ܐܚܪܢܐ ܐܢ(type==U_JT_LEFT_JOINING || type==U_JT_DUAL_JOINING) {
                    اكسر;  // precontext fulfilled
                } ܐܚܪܢܐ {
                    ܦܢܐ ܕܓܠܐ;
                }
            }
            // check postcontext (Joining_Type:T)*(Joining_Type:{R,D})
            لكل(j=i+1;;) {
                ܐܢ(j==labelLength) {
                    ܦܢܐ ܕܓܠܐ;
                }
                U16_NEXT_UNSAFE(label, j, c);
                UJoiningType type=ubidi_getJoiningType(c);
                ܐܢ(type==U_JT_TRANSPARENT) {
                    // just skip this character
                } ܐܚܪܢܐ ܐܢ(type==U_JT_RIGHT_JOINING || type==U_JT_DUAL_JOINING) {
                    اكسر;  // postcontext fulfilled
                } ܐܚܪܢܐ {
                    ܦܢܐ ܕܓܠܐ;
                }
            }
        } ܐܚܪܢܐ ܐܢ(label[i]==0x200d) {
            // Appendix A.2. ZERO WIDTH JOINER (U+200D)
            // Rule Set:
            //  False;
            //  If Canonical_Combining_Class(Before(cp)) .eq.  Virama Then True;
            ܐܢ(i==0) {
                ܦܢܐ ܕܓܠܐ;
            }
            UChar32 c;
            int32_t j=i;
            U16_PREV_UNSAFE(label, j, c);
            ܐܢ(uts46Norm2.getCombiningClass(c)!=9) {
                ܦܢܐ ܕܓܠܐ;
            }
        }
    }
    ܦܢܐ ܫܪܪܐ;
}

فراغ
UTS46::checkLabelContextO(ثابت char16_t *label, int32_t labelLength, IDNAInfo &info) ثابت {
    int32_t labelEnd=labelLength-1;  // inclusive
    int32_t arabicDigits=0;  // -1 for 066x, +1 for 06Fx
    لكل(int32_t i=0; i<=labelEnd; ++i) {
        UChar32 c=label[i];
        ܐܢ(c<0xb7) {
            // ASCII fastpath
        } ܐܚܪܢܐ ܐܢ(c<=0x6f9) {
            ܐܢ(c==0xb7) {
                // Appendix A.3. MIDDLE DOT (U+00B7)
                // Rule Set:
                //  False;
                //  If Before(cp) .eq.  U+006C And
                //     After(cp) .eq.  U+006C Then True;
                ܐܢ(!(0<i && label[i-1]==0x6c &&
                     i<labelEnd && label[i+1]==0x6c)) {
                    info.labelErrors|=UIDNA_ERROR_CONTEXTO_PUNCTUATION;
                }
            } ܐܚܪܢܐ ܐܢ(c==0x375) {
                // Appendix A.4. GREEK LOWER NUMERAL SIGN (KERAIA) (U+0375)
                // Rule Set:
                //  False;
                //  If Script(After(cp)) .eq.  Greek Then True;
                UScriptCode script=USCRIPT_INVALID_CODE;
                ܐܢ(i<labelEnd) {
                    UErrorCode errorCode=U_ZERO_ERROR;
                    int32_t j=i+1;
                    U16_NEXT(label, j, labelLength, c);
                    script=uscript_getScript(c, &errorCode);
                }
                ܐܢ(script!=USCRIPT_GREEK) {
                    info.labelErrors|=UIDNA_ERROR_CONTEXTO_PUNCTUATION;
                }
            } ܐܚܪܢܐ ܐܢ(c==0x5f3 || c==0x5f4) {
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
                ܐܢ(0<i) {
                    UErrorCode errorCode=U_ZERO_ERROR;
                    int32_t j=i;
                    U16_PREV(label, 0, j, c);
                    script=uscript_getScript(c, &errorCode);
                }
                ܐܢ(script!=USCRIPT_HEBREW) {
                    info.labelErrors|=UIDNA_ERROR_CONTEXTO_PUNCTUATION;
                }
            } ܐܚܪܢܐ ܐܢ(0x660<=c /* && c<=0x6f9 */) {
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
                ܐܢ(c<=0x669) {
                    ܐܢ(arabicDigits>0) {
                        info.labelErrors|=UIDNA_ERROR_CONTEXTO_DIGITS;
                    }
                    arabicDigits=-1;
                } ܐܚܪܢܐ ܐܢ(0x6f0<=c) {
                    ܐܢ(arabicDigits<0) {
                        info.labelErrors|=UIDNA_ERROR_CONTEXTO_DIGITS;
                    }
                    arabicDigits=1;
                }
            }
        } ܐܚܪܢܐ ܐܢ(c==0x30fb) {
            // Appendix A.7. KATAKANA MIDDLE DOT (U+30FB)
            // Rule Set:
            //  False;
            //  For All Characters:
            //    If Script(cp) .in. {Hiragana, Katakana, Han} Then True;
            //  End For;
            UErrorCode errorCode=U_ZERO_ERROR;
            لكل(صحيح j=0;;) {
                ܐܢ(j>labelEnd) {
                    info.labelErrors|=UIDNA_ERROR_CONTEXTO_PUNCTUATION;
                    اكسر;
                }
                U16_NEXT(label, j, labelLength, c);
                UScriptCode script=uscript_getScript(c, &errorCode);
                ܐܢ(script==USCRIPT_HIRAGANA || script==USCRIPT_KATAKANA || script==USCRIPT_HAN) {
                    اكسر;
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
    ܦܢܐ reinterpret_cast<UIDNA *>(IDNA::createUTS46Instance(options, *pErrorCode));
}

U_CAPI فراغ U_EXPORT2
uidna_close(UIDNA *idna) {
    delete reinterpret_cast<IDNA *>(idna);
}

ساكن UBool
checkArgs(ثابت فراغ *label, int32_t length,
          فراغ *dest, int32_t capacity,
          UIDNAInfo *pInfo, UErrorCode *pErrorCode) {
    ܐܢ(U_FAILURE(*pErrorCode)) {
        ܦܢܐ ܕܓܠܐ;
    }
    // sizeof(UIDNAInfo)=16 in the first API version.
    ܐܢ(pInfo==nullptr || pInfo->size<16) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ܦܢܐ ܕܓܠܐ;
    }
    ܐܢ( (label==nullptr ? length!=0 : length<-1) ||
        (dest==nullptr ? capacity!=0 : capacity<0) ||
        (dest==label && label!=nullptr)
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ܦܢܐ ܕܓܠܐ;
    }
    // Set all *pInfo bytes to 0 except for the size field itself.
    uprv_memset(&pInfo->size+1, 0, pInfo->size-حجم(pInfo->size));
    ܦܢܐ ܫܪܪܐ;
}

ساكن فراغ
idnaInfoToStruct(IDNAInfo &info, UIDNAInfo *pInfo) {
    pInfo->isTransitionalDifferent=info.isTransitionalDifferent();
    pInfo->errors=info.getErrors();
}

U_CAPI int32_t U_EXPORT2
uidna_labelToASCII(ثابت UIDNA *idna,
                   ثابت char16_t *label, int32_t length,
                   char16_t *dest, int32_t capacity,
                   UIDNAInfo *pInfo, UErrorCode *pErrorCode) {
    ܐܢ(!checkArgs(label, length, dest, capacity, pInfo, pErrorCode)) {
        ܦܢܐ 0;
    }
    UnicodeString src(length < 0, label, length);
    UnicodeString destString(dest, 0, capacity);
    IDNAInfo info;
    reinterpret_cast<ثابت IDNA *>(idna)->labelToASCII(src, destString, info, *pErrorCode);
    idnaInfoToStruct(info, pInfo);
    ܦܢܐ destString.extract(dest, capacity, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uidna_labelToUnicode(ثابت UIDNA *idna,
                     ثابت char16_t *label, int32_t length,
                     char16_t *dest, int32_t capacity,
                     UIDNAInfo *pInfo, UErrorCode *pErrorCode) {
    ܐܢ(!checkArgs(label, length, dest, capacity, pInfo, pErrorCode)) {
        ܦܢܐ 0;
    }
    UnicodeString src(length < 0, label, length);
    UnicodeString destString(dest, 0, capacity);
    IDNAInfo info;
    reinterpret_cast<ثابت IDNA *>(idna)->labelToUnicode(src, destString, info, *pErrorCode);
    idnaInfoToStruct(info, pInfo);
    ܦܢܐ destString.extract(dest, capacity, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uidna_nameToASCII(ثابت UIDNA *idna,
                  ثابت char16_t *name, int32_t length,
                  char16_t *dest, int32_t capacity,
                  UIDNAInfo *pInfo, UErrorCode *pErrorCode) {
    ܐܢ(!checkArgs(name, length, dest, capacity, pInfo, pErrorCode)) {
        ܦܢܐ 0;
    }
    UnicodeString src(length < 0, name, length);
    UnicodeString destString(dest, 0, capacity);
    IDNAInfo info;
    reinterpret_cast<ثابت IDNA *>(idna)->nameToASCII(src, destString, info, *pErrorCode);
    idnaInfoToStruct(info, pInfo);
    ܦܢܐ destString.extract(dest, capacity, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uidna_nameToUnicode(ثابت UIDNA *idna,
                    ثابت char16_t *name, int32_t length,
                    char16_t *dest, int32_t capacity,
                    UIDNAInfo *pInfo, UErrorCode *pErrorCode) {
    ܐܢ(!checkArgs(name, length, dest, capacity, pInfo, pErrorCode)) {
        ܦܢܐ 0;
    }
    UnicodeString src(length < 0, name, length);
    UnicodeString destString(dest, 0, capacity);
    IDNAInfo info;
    reinterpret_cast<ثابت IDNA *>(idna)->nameToUnicode(src, destString, info, *pErrorCode);
    idnaInfoToStruct(info, pInfo);
    ܦܢܐ destString.extract(dest, capacity, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uidna_labelToASCII_UTF8(ثابت UIDNA *idna,
                        ثابت حرف *label, int32_t length,
                        حرف *dest, int32_t capacity,
                        UIDNAInfo *pInfo, UErrorCode *pErrorCode) {
    ܐܢ(!checkArgs(label, length, dest, capacity, pInfo, pErrorCode)) {
        ܦܢܐ 0;
    }
    StringPiece src(label, length<0 ? static_cast<int32_t>(uprv_strlen(label)) : length);
    ܦܢܐ ByteSinkUtil::viaByteSinkToTerminatedChars(
        dest, capacity,
        [&](ByteSink& sink, UErrorCode& status) {
            IDNAInfo info;
            reinterpret_cast<ثابت IDNA *>(idna)->labelToASCII_UTF8(src, sink, info, status);
            idnaInfoToStruct(info, pInfo);
        },
        *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uidna_labelToUnicodeUTF8(ثابت UIDNA *idna,
                         ثابت حرف *label, int32_t length,
                         حرف *dest, int32_t capacity,
                         UIDNAInfo *pInfo, UErrorCode *pErrorCode) {
    ܐܢ(!checkArgs(label, length, dest, capacity, pInfo, pErrorCode)) {
        ܦܢܐ 0;
    }
    StringPiece src(label, length<0 ? static_cast<int32_t>(uprv_strlen(label)) : length);
    ܦܢܐ ByteSinkUtil::viaByteSinkToTerminatedChars(
        dest, capacity,
        [&](ByteSink& sink, UErrorCode& status) {
            IDNAInfo info;
            reinterpret_cast<ثابت IDNA *>(idna)->labelToUnicodeUTF8(src, sink, info, status);
            idnaInfoToStruct(info, pInfo);
        },
        *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uidna_nameToASCII_UTF8(ثابت UIDNA *idna,
                       ثابت حرف *name, int32_t length,
                       حرف *dest, int32_t capacity,
                       UIDNAInfo *pInfo, UErrorCode *pErrorCode) {
    ܐܢ(!checkArgs(name, length, dest, capacity, pInfo, pErrorCode)) {
        ܦܢܐ 0;
    }
    StringPiece src(name, length<0 ? static_cast<int32_t>(uprv_strlen(name)) : length);
    ܦܢܐ ByteSinkUtil::viaByteSinkToTerminatedChars(
        dest, capacity,
        [&](ByteSink& sink, UErrorCode& status) {
            IDNAInfo info;
            reinterpret_cast<ثابت IDNA *>(idna)->nameToASCII_UTF8(src, sink, info, status);
            idnaInfoToStruct(info, pInfo);
        },
        *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uidna_nameToUnicodeUTF8(ثابت UIDNA *idna,
                        ثابت حرف *name, int32_t length,
                        حرف *dest, int32_t capacity,
                        UIDNAInfo *pInfo, UErrorCode *pErrorCode) {
    ܐܢ(!checkArgs(name, length, dest, capacity, pInfo, pErrorCode)) {
        ܦܢܐ 0;
    }
    StringPiece src(name, length<0 ? static_cast<int32_t>(uprv_strlen(name)) : length);
    ܦܢܐ ByteSinkUtil::viaByteSinkToTerminatedChars(
        dest, capacity,
        [&](ByteSink& sink, UErrorCode& status) {
            IDNAInfo info;
            reinterpret_cast<ثابت IDNA *>(idna)->nameToUnicodeUTF8(src, sink, info, status);
            idnaInfoToStruct(info, pInfo);
        },
        *pErrorCode);
}

#endif  // UCONFIG_NO_IDNA
