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

#அந்தர்பாவ "unicode/utypes.h"

#எனில் !UCONFIG_NO_IDNA

#அந்தர்பாவ "unicode/bytestream.h"
#அந்தர்பாவ "unicode/idna.h"
#அந்தர்பாவ "unicode/normalizer2.h"
#அந்தர்பாவ "unicode/uscript.h"
#அந்தர்பாவ "unicode/ustring.h"
#அந்தர்பாவ "unicode/utf16.h"
#அந்தர்பாவ "bytesinkutil.h"
#அந்தர்பாவ "cmemory.h"
#அந்தர்பாவ "cstring.h"
#அந்தர்பாவ "punycode.h"
#அந்தர்பாவ "ubidi_props.h"

// Note about tests for UIDNA_ERROR_DOMAIN_NAME_TOO_LONG:
//
// The domain name length limit is 255 octets in an internal DNS representation
// where the last ("root") label is the empty label
// represented by length byte 0 alone.
// In a conventional string, this translates to 253 characters, or 254
// if there is a trailing dot for the root label.

U_NAMESPACE_BEGIN

// Severe errors which usually result in a U+FFFD replacement character in the result string.
மாறிலி uint32_t severeErrors=
    UIDNA_ERROR_LEADING_COMBINING_MARK|
    UIDNA_ERROR_DISALLOWED|
    UIDNA_ERROR_PUNYCODE|
    UIDNA_ERROR_LABEL_HAS_DOT|
    UIDNA_ERROR_INVALID_ACE_LABEL;

நிலையான உள்வரி UBool
isASCIIString(மாறிலி UnicodeString &dest) {
    மாறிலி char16_t *s=dest.getBuffer();
    மாறிலி char16_t *limit=s+dest.length();
    போது(s<limit) {
        எனில்(*s++>0x7f) {
            திரும்பு பொய்;
        }
    }
    திரும்பு உண்மை;
}

நிலையான UBool
isASCIIOkBiDi(மாறிலி char16_t *s, int32_t length);

நிலையான UBool
isASCIIOkBiDi(மாறிலி எழுத்து *s, int32_t length);

// IDNA class default implementations -------------------------------------- ***

IDNA::~IDNA() {}

வெறுமை
IDNA::labelToASCII_UTF8(StringPiece label, ByteSink &dest,
                        IDNAInfo &info, UErrorCode &errorCode) மாறிலி {
    எனில்(U_SUCCESS(errorCode)) {
        UnicodeString destString;
        labelToASCII(UnicodeString::fromUTF8(label), destString,
                     info, errorCode).toUTF8(dest);
    }
}

வெறுமை
IDNA::labelToUnicodeUTF8(StringPiece label, ByteSink &dest,
                         IDNAInfo &info, UErrorCode &errorCode) மாறிலி {
    எனில்(U_SUCCESS(errorCode)) {
        UnicodeString destString;
        labelToUnicode(UnicodeString::fromUTF8(label), destString,
                       info, errorCode).toUTF8(dest);
    }
}

வெறுமை
IDNA::nameToASCII_UTF8(StringPiece name, ByteSink &dest,
                       IDNAInfo &info, UErrorCode &errorCode) மாறிலி {
    எனில்(U_SUCCESS(errorCode)) {
        UnicodeString destString;
        nameToASCII(UnicodeString::fromUTF8(name), destString,
                    info, errorCode).toUTF8(dest);
    }
}

வெறுமை
IDNA::nameToUnicodeUTF8(StringPiece name, ByteSink &dest,
                        IDNAInfo &info, UErrorCode &errorCode) மாறிலி {
    எனில்(U_SUCCESS(errorCode)) {
        UnicodeString destString;
        nameToUnicode(UnicodeString::fromUTF8(name), destString,
                      info, errorCode).toUTF8(dest);
    }
}

// UTS46 class declaration ------------------------------------------------- ***

வகுப்பு UTS46 : பொது IDNA {
பொது:
    UTS46(uint32_t options, UErrorCode &errorCode);
    மெய்நிகர் ~UTS46();

    மெய்நிகர் UnicodeString&
    labelToASCII(மாறிலி UnicodeString& label,
                 UnicodeString& dest U_LIFETIME_BOUND,
                 IDNAInfo& info,
                 UErrorCode& errorCode) மாறிலி override;

    மெய்நிகர் UnicodeString&
    labelToUnicode(மாறிலி UnicodeString& label,
                   UnicodeString& dest U_LIFETIME_BOUND,
                   IDNAInfo& info,
                   UErrorCode& errorCode) மாறிலி override;

    மெய்நிகர் UnicodeString&
    nameToASCII(மாறிலி UnicodeString& name,
                UnicodeString& dest U_LIFETIME_BOUND,
                IDNAInfo& info,
                UErrorCode& errorCode) மாறிலி override;

    மெய்நிகர் UnicodeString&
    nameToUnicode(மாறிலி UnicodeString& name,
                  UnicodeString& dest U_LIFETIME_BOUND,
                  IDNAInfo& info,
                  UErrorCode& errorCode) மாறிலி override;

    மெய்நிகர் வெறுமை
    labelToASCII_UTF8(StringPiece label, ByteSink &dest,
                      IDNAInfo &info, UErrorCode &errorCode) மாறிலி override;

    மெய்நிகர் வெறுமை
    labelToUnicodeUTF8(StringPiece label, ByteSink &dest,
                       IDNAInfo &info, UErrorCode &errorCode) மாறிலி override;

    மெய்நிகர் வெறுமை
    nameToASCII_UTF8(StringPiece name, ByteSink &dest,
                     IDNAInfo &info, UErrorCode &errorCode) மாறிலி override;

    மெய்நிகர் வெறுமை
    nameToUnicodeUTF8(StringPiece name, ByteSink &dest,
                      IDNAInfo &info, UErrorCode &errorCode) மாறிலி override;

தனிப்பட்ட:
    UnicodeString&
    process(மாறிலி UnicodeString& src,
            UBool isLabel,
            UBool toASCII,
            UnicodeString& dest U_LIFETIME_BOUND,
            IDNAInfo& info,
            UErrorCode& errorCode) மாறிலி;

    வெறுமை
    processUTF8(StringPiece src,
                UBool isLabel, UBool toASCII,
                ByteSink &dest,
                IDNAInfo &info, UErrorCode &errorCode) மாறிலி;

    UnicodeString&
    processUnicode(மாறிலி UnicodeString& src,
                   int32_t labelStart,
                   int32_t mappingStart,
                   UBool isLabel,
                   UBool toASCII,
                   UnicodeString& dest U_LIFETIME_BOUND,
                   IDNAInfo& info,
                   UErrorCode& errorCode) மாறிலி;

    // returns the new dest.length()
    int32_t
    mapDevChars(UnicodeString &dest, int32_t labelStart, int32_t mappingStart,
                UErrorCode &errorCode) மாறிலி;

    // returns the new label length
    int32_t
    processLabel(UnicodeString &dest,
                 int32_t labelStart, int32_t labelLength,
                 UBool toASCII,
                 IDNAInfo &info, UErrorCode &errorCode) மாறிலி;
    int32_t
    markBadACELabel(UnicodeString &dest,
                    int32_t labelStart, int32_t labelLength,
                    UBool toASCII, IDNAInfo &info, UErrorCode &errorCode) மாறிலி;

    வெறுமை
    checkLabelBiDi(மாறிலி char16_t *label, int32_t labelLength, IDNAInfo &info) மாறிலி;

    UBool
    isLabelOkContextJ(மாறிலி char16_t *label, int32_t labelLength) மாறிலி;

    வெறுமை
    checkLabelContextO(மாறிலி char16_t *label, int32_t labelLength, IDNAInfo &info) மாறிலி;

    மாறிலி Normalizer2 &uts46Norm2;  // uts46.nrm
    uint32_t options;
};

IDNA *
IDNA::createUTS46Instance(uint32_t options, UErrorCode &errorCode) {
    எனில்(U_SUCCESS(errorCode)) {
        IDNA *idna=புதிய UTS46(options, errorCode);
        எனில்(idna==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
        } இல்லையெனில் எனில்(U_FAILURE(errorCode)) {
            delete idna;
            idna=nullptr;
        }
        திரும்பு idna;
    } இல்லையெனில் {
        திரும்பு nullptr;
    }
}

// UTS46 implementation ---------------------------------------------------- ***

UTS46::UTS46(uint32_t opt, UErrorCode &errorCode)
        : uts46Norm2(*Normalizer2::getInstance(nullptr, "uts46", UNORM2_COMPOSE, errorCode)),
          options(opt) {}

UTS46::~UTS46() {}

UnicodeString&
UTS46::labelToASCII(மாறிலி UnicodeString& label,
                    UnicodeString& dest U_LIFETIME_BOUND,
                    IDNAInfo& info,
                    UErrorCode& errorCode) மாறிலி {
    திரும்பு process(label, உண்மை, உண்மை, dest, info, errorCode);
}

UnicodeString&
UTS46::labelToUnicode(மாறிலி UnicodeString& label,
                      UnicodeString& dest U_LIFETIME_BOUND,
                      IDNAInfo& info,
                      UErrorCode& errorCode) மாறிலி {
    திரும்பு process(label, உண்மை, பொய், dest, info, errorCode);
}

UnicodeString&
UTS46::nameToASCII(மாறிலி UnicodeString& name,
                   UnicodeString& dest U_LIFETIME_BOUND,
                   IDNAInfo& info,
                   UErrorCode& errorCode) மாறிலி {
    process(name, பொய், உண்மை, dest, info, errorCode);
    எனில்( dest.length()>=254 && (info.errors&UIDNA_ERROR_DOMAIN_NAME_TOO_LONG)==0 &&
        isASCIIString(dest) &&
        (dest.length()>254 || dest[253]!=0x2e)
    ) {
        info.errors|=UIDNA_ERROR_DOMAIN_NAME_TOO_LONG;
    }
    திரும்பு dest;
}

UnicodeString&
UTS46::nameToUnicode(மாறிலி UnicodeString& name,
                     UnicodeString& dest U_LIFETIME_BOUND,
                     IDNAInfo& info,
                     UErrorCode& errorCode) மாறிலி {
    திரும்பு process(name, பொய், பொய், dest, info, errorCode);
}

வெறுமை
UTS46::labelToASCII_UTF8(StringPiece label, ByteSink &dest,
                         IDNAInfo &info, UErrorCode &errorCode) மாறிலி {
    processUTF8(label, உண்மை, உண்மை, dest, info, errorCode);
}

வெறுமை
UTS46::labelToUnicodeUTF8(StringPiece label, ByteSink &dest,
                          IDNAInfo &info, UErrorCode &errorCode) மாறிலி {
    processUTF8(label, உண்மை, பொய், dest, info, errorCode);
}

வெறுமை
UTS46::nameToASCII_UTF8(StringPiece name, ByteSink &dest,
                        IDNAInfo &info, UErrorCode &errorCode) மாறிலி {
    processUTF8(name, பொய், உண்மை, dest, info, errorCode);
}

வெறுமை
UTS46::nameToUnicodeUTF8(StringPiece name, ByteSink &dest,
                         IDNAInfo &info, UErrorCode &errorCode) மாறிலி {
    processUTF8(name, பொய், பொய், dest, info, errorCode);
}

// UTS #46 data for ASCII characters.
// The normalizer (using uts46.nrm) maps uppercase ASCII letters to lowercase
// and passes through all other ASCII characters.
// If UIDNA_USE_STD3_RULES is set, then non-LDH characters are disallowed
// using this data.
// The ASCII fastpath also uses this data.
// Values: -1=disallowed  0==valid  1==mapped (lowercase)
நிலையான மாறிலி int8_t asciiData[128]={
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
UTS46::process(மாறிலி UnicodeString& src,
               UBool isLabel,
               UBool toASCII,
               UnicodeString& dest U_LIFETIME_BOUND,
               IDNAInfo& info,
               UErrorCode& errorCode) மாறிலி {
    // uts46Norm2.normalize() would do all of this error checking and setup,
    // but with the ASCII fastpath we do not always call it, and do not
    // call it first.
    எனில்(U_FAILURE(errorCode)) {
        dest.setToBogus();
        திரும்பு dest;
    }
    மாறிலி char16_t *srcArray=src.getBuffer();
    எனில்(&dest==&src || srcArray==nullptr) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        dest.setToBogus();
        திரும்பு dest;
    }
    // Arguments are fine, reset output values.
    dest.remove();
    info.reset();
    int32_t srcLength=src.length();
    எனில்(srcLength==0) {
        info.errors|=UIDNA_ERROR_EMPTY_LABEL;
        திரும்பு dest;
    }
    char16_t *destArray=dest.getBuffer(srcLength);
    எனில்(destArray==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        திரும்பு dest;
    }
    // ASCII fastpath
    UBool disallowNonLDHDot=(options&UIDNA_USE_STD3_RULES)!=0;
    int32_t labelStart=0;
    int32_t i;
    க்கு(i=0;; ++i) {
        எனில்(i==srcLength) {
            எனில்(toASCII) {
                எனில்((i-labelStart)>63) {
                    info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
                }
                // There is a trailing dot if labelStart==i.
                எனில்(!isLabel && i>=254 && (i>254 || labelStart<i)) {
                    info.errors|=UIDNA_ERROR_DOMAIN_NAME_TOO_LONG;
                }
            }
            info.errors|=info.labelErrors;
            dest.releaseBuffer(i);
            திரும்பு dest;
        }
        char16_t c=srcArray[i];
        எனில்(c>0x7f) {
            நிறுத்து;
        }
        முழு cData=asciiData[c];
        எனில்(cData>0) {
            destArray[i]=c+0x20;  // Lowercase an uppercase ASCII letter.
        } இல்லையெனில் எனில்(cData<0 && disallowNonLDHDot) {
            நிறுத்து;  // Replacing with U+FFFD can be complicated for toASCII.
        } இல்லையெனில் {
            destArray[i]=c;
            எனில்(c==0x2d) {  // hyphen
                எனில்(i==(labelStart+3) && srcArray[i-1]==0x2d) {
                    // "??--..." is Punycode or forbidden.
                    ++i;  // '-' was copied to dest already
                    நிறுத்து;
                }
                எனில்(i==labelStart) {
                    // label starts with "-"
                    info.labelErrors|=UIDNA_ERROR_LEADING_HYPHEN;
                }
                எனில்((i+1)==srcLength || srcArray[i+1]==0x2e) {
                    // label ends with "-"
                    info.labelErrors|=UIDNA_ERROR_TRAILING_HYPHEN;
                }
            } இல்லையெனில் எனில்(c==0x2e) {  // dot
                எனில்(isLabel) {
                    // Replacing with U+FFFD can be complicated for toASCII.
                    ++i;  // '.' was copied to dest already
                    நிறுத்து;
                }
                எனில்(i==labelStart) {
                    info.labelErrors|=UIDNA_ERROR_EMPTY_LABEL;
                }
                எனில்(toASCII && (i-labelStart)>63) {
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
    எனில்( info.isBiDi && U_SUCCESS(errorCode) && (info.errors&severeErrors)==0 &&
        (!info.isOkBiDi || (labelStart>0 && !isASCIIOkBiDi(dest.getBuffer(), labelStart)))
    ) {
        info.errors|=UIDNA_ERROR_BIDI;
    }
    திரும்பு dest;
}

வெறுமை
UTS46::processUTF8(StringPiece src,
                   UBool isLabel, UBool toASCII,
                   ByteSink &dest,
                   IDNAInfo &info, UErrorCode &errorCode) மாறிலி {
    எனில்(U_FAILURE(errorCode)) {
        திரும்பு;
    }
    மாறிலி எழுத்து *srcArray=src.data();
    int32_t srcLength=src.length();
    எனில்(srcArray==nullptr && srcLength!=0) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு;
    }
    // Arguments are fine, reset output values.
    info.reset();
    எனில்(srcLength==0) {
        info.errors|=UIDNA_ERROR_EMPTY_LABEL;
        dest.Flush();
        திரும்பு;
    }
    UnicodeString destString;
    int32_t labelStart=0;
    எனில்(srcLength<=256) {  // length of stackArray[]
        // ASCII fastpath
        எழுத்து அடுக்கு_Array[256];
        int32_t destCapacity;
        எழுத்து *destArray=dest.GetAppendBuffer(srcLength, srcLength+20,
                                             அடுக்கு_Array, UPRV_LENGTHOF(அடுக்கு_Array), &destCapacity);
        UBool disallowNonLDHDot=(options&UIDNA_USE_STD3_RULES)!=0;
        int32_t i;
        க்கு(i=0;; ++i) {
            எனில்(i==srcLength) {
                எனில்(toASCII) {
                    எனில்((i-labelStart)>63) {
                        info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
                    }
                    // There is a trailing dot if labelStart==i.
                    எனில்(!isLabel && i>=254 && (i>254 || labelStart<i)) {
                        info.errors|=UIDNA_ERROR_DOMAIN_NAME_TOO_LONG;
                    }
                }
                info.errors|=info.labelErrors;
                dest.Append(destArray, i);
                dest.Flush();
                திரும்பு;
            }
            எழுத்து c=srcArray[i];
            எனில் (static_cast<int8_t>(c) < 0) { // (uint8_t)c>0x7f
                நிறுத்து;
            }
            முழு cData = asciiData[static_cast<முழு>(c)]; // Cast: gcc warns about indexing with a char.
            எனில்(cData>0) {
                destArray[i]=c+0x20;  // Lowercase an uppercase ASCII letter.
            } இல்லையெனில் எனில்(cData<0 && disallowNonLDHDot) {
                நிறுத்து;  // Replacing with U+FFFD can be complicated for toASCII.
            } இல்லையெனில் {
                destArray[i]=c;
                எனில்(c==0x2d) {  // hyphen
                    எனில்(i==(labelStart+3) && srcArray[i-1]==0x2d) {
                        // "??--..." is Punycode or forbidden.
                        நிறுத்து;
                    }
                    எனில்(i==labelStart) {
                        // label starts with "-"
                        info.labelErrors|=UIDNA_ERROR_LEADING_HYPHEN;
                    }
                    எனில்((i+1)==srcLength || srcArray[i+1]==0x2e) {
                        // label ends with "-"
                        info.labelErrors|=UIDNA_ERROR_TRAILING_HYPHEN;
                    }
                } இல்லையெனில் எனில்(c==0x2e) {  // dot
                    எனில்(isLabel) {
                        நிறுத்து;  // Replacing with U+FFFD can be complicated for toASCII.
                    }
                    எனில்(i==labelStart) {
                        info.labelErrors|=UIDNA_ERROR_EMPTY_LABEL;
                    }
                    எனில்(toASCII && (i-labelStart)>63) {
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
    } இல்லையெனில் {
        // src is too long for the ASCII fastpath implementation.
        processUnicode(UnicodeString::fromUTF8(src), 0, 0,
                       isLabel, toASCII,
                       destString, info, errorCode);
    }
    destString.toUTF8(dest);  // calls dest.Flush()
    எனில்(toASCII && !isLabel) {
        // length==labelStart==254 means that there is a trailing dot (ok) and
        // destString is empty (do not index at 253-labelStart).
        int32_t length=labelStart+destString.length();
        எனில்( length>=254 && isASCIIString(destString) &&
            (length>254 ||
             (labelStart<254 && destString[253-labelStart]!=0x2e))
        ) {
            info.errors|=UIDNA_ERROR_DOMAIN_NAME_TOO_LONG;
        }
    }
    எனில்( info.isBiDi && U_SUCCESS(errorCode) && (info.errors&severeErrors)==0 &&
        (!info.isOkBiDi || (labelStart>0 && !isASCIIOkBiDi(srcArray, labelStart)))
    ) {
        info.errors|=UIDNA_ERROR_BIDI;
    }
}

UnicodeString&
UTS46::processUnicode(மாறிலி UnicodeString& src,
                      int32_t labelStart,
                      int32_t mappingStart,
                      UBool isLabel,
                      UBool toASCII,
                      UnicodeString& dest U_LIFETIME_BOUND,
                      IDNAInfo& info,
                      UErrorCode& errorCode) மாறிலி {
    எனில்(mappingStart==0) {
        uts46Norm2.normalize(src, dest, errorCode);
    } இல்லையெனில் {
        uts46Norm2.normalizeSecondAndAppend(dest, src.tempSubString(mappingStart), errorCode);
    }
    எனில்(U_FAILURE(errorCode)) {
        திரும்பு dest;
    }
    UBool doMapDevChars=
        toASCII ? (options&UIDNA_NONTRANSITIONAL_TO_ASCII)==0 :
                  (options&UIDNA_NONTRANSITIONAL_TO_UNICODE)==0;
    மாறிலி char16_t *destArray=dest.getBuffer();
    int32_t destLength=dest.length();
    int32_t labelLimit=labelStart;
    போது(labelLimit<destLength) {
        char16_t c=destArray[labelLimit];
        எனில்(c==0x2e && !isLabel) {
            int32_t labelLength=labelLimit-labelStart;
            int32_t newLength=processLabel(dest, labelStart, labelLength,
                                            toASCII, info, errorCode);
            info.errors|=info.labelErrors;
            info.labelErrors=0;
            எனில்(U_FAILURE(errorCode)) {
                திரும்பு dest;
            }
            destArray=dest.getBuffer();
            destLength+=newLength-labelLength;
            labelLimit=labelStart+=newLength+1;
            தொடர்;
        } இல்லையெனில் எனில்(c<0xdf) {
            // pass
        } இல்லையெனில் எனில்(c<=0x200d && (c==0xdf || c==0x3c2 || c>=0x200c)) {
            info.isTransDiff=உண்மை;
            எனில்(doMapDevChars) {
                destLength=mapDevChars(dest, labelStart, labelLimit, errorCode);
                எனில்(U_FAILURE(errorCode)) {
                    திரும்பு dest;
                }
                destArray=dest.getBuffer();
                // All deviation characters have been mapped, no need to check for them again.
                doMapDevChars=பொய்;
                // Do not increment labelLimit in case c was removed.
                தொடர்;
            }
        } இல்லையெனில் எனில்(U16_IS_SURROGATE(c)) {
            எனில்(U16_IS_SURROGATE_LEAD(c) ?
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
    எனில்(0==labelStart || labelStart<labelLimit) {
        processLabel(dest, labelStart, labelLimit-labelStart,
                      toASCII, info, errorCode);
        info.errors|=info.labelErrors;
    }
    திரும்பு dest;
}

int32_t
UTS46::mapDevChars(UnicodeString &dest, int32_t labelStart, int32_t mappingStart,
                   UErrorCode &errorCode) மாறிலி {
    எனில்(U_FAILURE(errorCode)) {
        திரும்பு 0;
    }
    int32_t length=dest.length();
    char16_t *s=dest.getBuffer(dest[mappingStart]==0xdf ? length+1 : length);
    எனில்(s==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        திரும்பு length;
    }
    int32_t capacity=dest.getCapacity();
    UBool didMapDevChars=பொய்;
    int32_t readIndex=mappingStart, writeIndex=mappingStart;
    செய் {
        char16_t c=s[readIndex++];
        தேர்வு(c) {
        நிலை 0xdf:
            // Map sharp s to ss.
            didMapDevChars=உண்மை;
            s[writeIndex++]=0x73;  // Replace sharp s with first s.
            // Insert second s and account for possible buffer reallocation.
            எனில்(writeIndex==readIndex) {
                எனில்(length==capacity) {
                    dest.releaseBuffer(length);
                    s=dest.getBuffer(length+1);
                    எனில்(s==nullptr) {
                        errorCode=U_MEMORY_ALLOCATION_ERROR;
                        திரும்பு length;
                    }
                    capacity=dest.getCapacity();
                }
                u_memmove(s+writeIndex+1, s+writeIndex, length-writeIndex);
                ++readIndex;
            }
            s[writeIndex++]=0x73;
            ++length;
            நிறுத்து;
        நிலை 0x3c2:  // Map final sigma to nonfinal sigma.
            didMapDevChars=உண்மை;
            s[writeIndex++]=0x3c3;
            நிறுத்து;
        நிலை 0x200c:  // Ignore/remove ZWNJ.
        நிலை 0x200d:  // Ignore/remove ZWJ.
            didMapDevChars=உண்மை;
            --length;
            நிறுத்து;
        இயல்பு:
            // Only really necessary if writeIndex was different from readIndex.
            s[writeIndex++]=c;
            நிறுத்து;
        }
    } போது(writeIndex<length);
    dest.releaseBuffer(length);
    எனில்(didMapDevChars) {
        // Mapping deviation characters might have resulted in an un-NFC string.
        // We could use either the NFC or the UTS #46 normalizer.
        // By using the UTS #46 normalizer again, we avoid having to load a second .nrm data file.
        UnicodeString normalized;
        uts46Norm2.normalize(dest.tempSubString(labelStart), normalized, errorCode);
        எனில்(U_SUCCESS(errorCode)) {
            dest.replace(labelStart, 0x7fffffff, normalized);
            எனில்(dest.isBogus()) {
                errorCode=U_MEMORY_ALLOCATION_ERROR;
            }
            திரும்பு dest.length();
        }
    }
    திரும்பு length;
}

// Replace the label in dest with the label string, if the label was modified.
// If &label==&dest then the label was modified in-place and labelLength
// is the new label length, different from label.length().
// If &label!=&dest then labelLength==label.length().
// Returns labelLength (= the new label length).
நிலையான int32_t
replaceLabel(UnicodeString &dest, int32_t destLabelStart, int32_t destLabelLength,
             மாறிலி UnicodeString &label, int32_t labelLength, UErrorCode &errorCode) {
    எனில்(U_FAILURE(errorCode)) {
        திரும்பு 0;
    }
    எனில்(&label!=&dest) {
        dest.replace(destLabelStart, destLabelLength, label);
        எனில்(dest.isBogus()) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            திரும்பு 0;
        }
    }
    திரும்பு labelLength;
}

int32_t
UTS46::processLabel(UnicodeString &dest,
                    int32_t labelStart, int32_t labelLength,
                    UBool toASCII,
                    IDNAInfo &info, UErrorCode &errorCode) மாறிலி {
    எனில்(U_FAILURE(errorCode)) {
        திரும்பு 0;
    }
    UnicodeString fromPunycode;
    UnicodeString *labelString;
    மாறிலி char16_t *label=dest.getBuffer()+labelStart;
    int32_t destLabelStart=labelStart;
    int32_t destLabelLength=labelLength;
    UBool wasPunycode;
    எனில்(labelLength>=4 && label[0]==0x78 && label[1]==0x6e && label[2]==0x2d && label[3]==0x2d) {
        // Label starts with "xn--", try to un-Punycode it.
        // In IDNA2008, labels like "xn--" (decodes to an empty string) and
        // "xn--ASCII-" (decodes to just "ASCII") fail the round-trip validation from
        // comparing the ToUnicode input with the back-to-ToASCII output.
        // They are alternate encodings of the respective ASCII labels.
        // Ignore "xn---" here: It will fail Punycode.decode() which logically comes before
        // the round-trip verification.
        எனில்(labelLength==4 || (labelLength>5 && label[labelLength-1]==u'-')) {
            info.labelErrors|=UIDNA_ERROR_INVALID_ACE_LABEL;
            திரும்பு markBadACELabel(dest, labelStart, labelLength, toASCII, info, errorCode);
        }
        wasPunycode=உண்மை;
        char16_t *unicodeBuffer=fromPunycode.getBuffer(-1);  // capacity==-1: most labels should fit
        எனில்(unicodeBuffer==nullptr) {
            // Should never occur if we used capacity==-1 which uses the internal buffer.
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            திரும்பு labelLength;
        }
        UErrorCode punycodeErrorCode=U_ZERO_ERROR;
        int32_t unicodeLength=u_strFromPunycode(label+4, labelLength-4,
                                                unicodeBuffer, fromPunycode.getCapacity(),
                                                nullptr, &punycodeErrorCode);
        எனில்(punycodeErrorCode==U_BUFFER_OVERFLOW_ERROR) {
            fromPunycode.releaseBuffer(0);
            unicodeBuffer=fromPunycode.getBuffer(unicodeLength);
            எனில்(unicodeBuffer==nullptr) {
                errorCode=U_MEMORY_ALLOCATION_ERROR;
                திரும்பு labelLength;
            }
            punycodeErrorCode=U_ZERO_ERROR;
            unicodeLength=u_strFromPunycode(label+4, labelLength-4,
                                            unicodeBuffer, fromPunycode.getCapacity(),
                                            nullptr, &punycodeErrorCode);
        }
        fromPunycode.releaseBuffer(unicodeLength);
        எனில்(U_FAILURE(punycodeErrorCode)) {
            info.labelErrors|=UIDNA_ERROR_PUNYCODE;
            திரும்பு markBadACELabel(dest, labelStart, labelLength, toASCII, info, errorCode);
        }
        // Check for NFC, and for characters that are not
        // valid or deviation characters according to the normalizer.
        // If there is something wrong, then the string will change.
        // Note that the normalizer passes through non-LDH ASCII and deviation characters.
        // Deviation characters are ok in Punycode even in transitional processing.
        // In the code further below, if we find non-LDH ASCII and we have UIDNA_USE_STD3_RULES
        // then we will set UIDNA_ERROR_INVALID_ACE_LABEL there too.
        UBool isValid=uts46Norm2.isNormalized(fromPunycode, errorCode);
        எனில்(U_FAILURE(errorCode)) {
            திரும்பு labelLength;
        }
        // Unicode 15.1 UTS #46:
        // Added an additional condition in 4.1 Validity Criteria to
        // disallow labels such as xn--xn---epa., which do not round-trip.
        // --> Validity Criteria new criterion 4:
        // If not CheckHyphens, the label must not begin with “xn--”.
        எனில்(!isValid || fromPunycode.startsWith(UnicodeString::readOnlyAlias(u"xn--"))) {
            info.labelErrors|=UIDNA_ERROR_INVALID_ACE_LABEL;
            திரும்பு markBadACELabel(dest, labelStart, labelLength, toASCII, info, errorCode);
        }
        labelString=&fromPunycode;
        label=fromPunycode.getBuffer();
        labelStart=0;
        labelLength=fromPunycode.length();
    } இல்லையெனில் {
        wasPunycode=பொய்;
        labelString=&dest;
    }
    // Validity check
    எனில்(labelLength==0) {
        info.labelErrors|=UIDNA_ERROR_EMPTY_LABEL;
        திரும்பு replaceLabel(dest, destLabelStart, destLabelLength,
                            *labelString, labelLength, errorCode);
    }
    // labelLength>0
    எனில்(labelLength>=4 && label[2]==0x2d && label[3]==0x2d) {
        // label starts with "??--"
        info.labelErrors|=UIDNA_ERROR_HYPHEN_3_4;
    }
    எனில்(label[0]==0x2d) {
        // label starts with "-"
        info.labelErrors|=UIDNA_ERROR_LEADING_HYPHEN;
    }
    எனில்(label[labelLength-1]==0x2d) {
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
    மாறிலி char16_t *limit=label+labelLength;
    char16_t oredChars=0;
    // If we enforce STD3 rules, then ASCII characters other than LDH and dot are disallowed.
    UBool disallowNonLDHDot=(options&UIDNA_USE_STD3_RULES)!=0;
    செய் {
        char16_t c=*s;
        எனில்(c<=0x7f) {
            எனில்(c==0x2e) {
                info.labelErrors|=UIDNA_ERROR_LABEL_HAS_DOT;
                *s=0xfffd;
            } இல்லையெனில் எனில்(disallowNonLDHDot && asciiData[c]<0) {
                info.labelErrors|=UIDNA_ERROR_DISALLOWED;
                *s=0xfffd;
            }
        } இல்லையெனில் {
            oredChars|=c;
            எனில்(c==0xfffd) {
                info.labelErrors|=UIDNA_ERROR_DISALLOWED;
            }
        }
        ++s;
    } போது(s<limit);
    // Check for a leading combining mark after other validity checks
    // so that we don't report UIDNA_ERROR_DISALLOWED for the U+FFFD from here.
    UChar32 c;
    int32_t cpLength=0;
    // "Unsafe" is ok because unpaired surrogates were mapped to U+FFFD.
    U16_NEXT_UNSAFE(label, cpLength, c);
    எனில்((U_GET_GC_MASK(c)&U_GC_M_MASK)!=0) {
        info.labelErrors|=UIDNA_ERROR_LEADING_COMBINING_MARK;
        labelString->replace(labelStart, cpLength, static_cast<char16_t>(0xfffd));
        label=labelString->getBuffer()+labelStart;
        labelLength+=1-cpLength;
        எனில்(labelString==&dest) {
            destLabelLength=labelLength;
        }
    }
    எனில்((info.labelErrors&severeErrors)==0) {
        // Do contextual checks only if we do not have U+FFFD from a severe error
        // because U+FFFD can make these checks fail.
        எனில்((options&UIDNA_CHECK_BIDI)!=0 && (!info.isBiDi || info.isOkBiDi)) {
            checkLabelBiDi(label, labelLength, info);
        }
        எனில்( (options&UIDNA_CHECK_CONTEXTJ)!=0 && (oredChars&0x200c)==0x200c &&
            !isLabelOkContextJ(label, labelLength)
        ) {
            info.labelErrors|=UIDNA_ERROR_CONTEXTJ;
        }
        எனில்((options&UIDNA_CHECK_CONTEXTO)!=0 && oredChars>=0xb7) {
            checkLabelContextO(label, labelLength, info);
        }
        எனில்(toASCII) {
            எனில்(wasPunycode) {
                // Leave a Punycode label unchanged if it has no severe errors.
                எனில்(destLabelLength>63) {
                    info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
                }
                திரும்பு destLabelLength;
            } இல்லையெனில் எனில்(oredChars>=0x80) {
                // Contains non-ASCII characters.
                UnicodeString punycode;
                char16_t *buffer=punycode.getBuffer(63);  // 63==maximum DNS label length
                எனில்(buffer==nullptr) {
                    errorCode=U_MEMORY_ALLOCATION_ERROR;
                    திரும்பு destLabelLength;
                }
                buffer[0]=0x78;  // Write "xn--".
                buffer[1]=0x6e;
                buffer[2]=0x2d;
                buffer[3]=0x2d;
                UErrorCode punycodeErrorCode=U_ZERO_ERROR;
                int32_t punycodeLength=u_strToPunycode(label, labelLength,
                                                      buffer+4, punycode.getCapacity()-4,
                                                      nullptr, &punycodeErrorCode);
                எனில்(punycodeErrorCode==U_BUFFER_OVERFLOW_ERROR) {
                    punycodeErrorCode=U_ZERO_ERROR;
                    punycode.releaseBuffer(4);
                    buffer=punycode.getBuffer(4+punycodeLength);
                    எனில்(buffer==nullptr) {
                        errorCode=U_MEMORY_ALLOCATION_ERROR;
                        திரும்பு destLabelLength;
                    }
                    punycodeLength=u_strToPunycode(label, labelLength,
                                                  buffer+4, punycode.getCapacity()-4,
                                                  nullptr, &punycodeErrorCode);
                }
                punycodeLength+=4;
                punycode.releaseBuffer(punycodeLength);
                எனில்(U_FAILURE(punycodeErrorCode)) {
                    errorCode = punycodeErrorCode;
                    திரும்பு destLabelLength;
                }
                எனில்(punycodeLength>63) {
                    info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
                }
                திரும்பு replaceLabel(dest, destLabelStart, destLabelLength,
                                    punycode, punycodeLength, errorCode);
            } இல்லையெனில் {
                // all-ASCII label
                எனில்(labelLength>63) {
                    info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
                }
            }
        }
    } இல்லையெனில் {
        // If a Punycode label has severe errors,
        // then leave it but make sure it does not look valid.
        எனில்(wasPunycode) {
            info.labelErrors|=UIDNA_ERROR_INVALID_ACE_LABEL;
            திரும்பு markBadACELabel(dest, destLabelStart, destLabelLength, toASCII, info, errorCode);
        }
    }
    திரும்பு replaceLabel(dest, destLabelStart, destLabelLength,
                        *labelString, labelLength, errorCode);
}

// Make sure an ACE label does not look valid.
// Append U+FFFD if the label has only LDH characters.
// If UIDNA_USE_STD3_RULES, also replace disallowed ASCII characters with U+FFFD.
int32_t
UTS46::markBadACELabel(UnicodeString &dest,
                       int32_t labelStart, int32_t labelLength,
                       UBool toASCII, IDNAInfo &info, UErrorCode &errorCode) மாறிலி {
    எனில்(U_FAILURE(errorCode)) {
        திரும்பு 0;
    }
    UBool disallowNonLDHDot=(options&UIDNA_USE_STD3_RULES)!=0;
    UBool isASCII=உண்மை;
    UBool onlyLDH=உண்மை;
    மாறிலி char16_t *label=dest.getBuffer()+labelStart;
    மாறிலி char16_t *limit=label+labelLength;
    // Start after the initial "xn--".
    // Ok to cast away const because we own the UnicodeString.
    க்கு(char16_t *s=const_cast<char16_t *>(label+4); s<limit; ++s) {
        char16_t c=*s;
        எனில்(c<=0x7f) {
            எனில்(c==0x2e) {
                info.labelErrors|=UIDNA_ERROR_LABEL_HAS_DOT;
                *s=0xfffd;
                isASCII=onlyLDH=பொய்;
            } இல்லையெனில் எனில்(asciiData[c]<0) {
                onlyLDH=பொய்;
                எனில்(disallowNonLDHDot) {
                    *s=0xfffd;
                    isASCII=பொய்;
                }
            }
        } இல்லையெனில் {
            isASCII=onlyLDH=பொய்;
        }
    }
    எனில்(onlyLDH) {
        dest.insert(labelStart + labelLength, static_cast<char16_t>(0xfffd));
        எனில்(dest.isBogus()) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            திரும்பு 0;
        }
        ++labelLength;
    } இல்லையெனில் {
        எனில்(toASCII && isASCII && labelLength>63) {
            info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
        }
    }
    திரும்பு labelLength;
}

மாறிலி uint32_t L_MASK=U_MASK(U_இடது_TO_வலது);
மாறிலி uint32_t R_AL_MASK=U_MASK(U_வலது_TO_இடது)|U_MASK(U_வலது_TO_இடது_ARABIC);
மாறிலி uint32_t L_R_AL_MASK=L_MASK|R_AL_MASK;

மாறிலி uint32_t R_AL_AN_MASK=R_AL_MASK|U_MASK(U_ARABIC_எண்);

மாறிலி uint32_t EN_AN_MASK=U_MASK(U_EUROPEAN_எண்)|U_MASK(U_ARABIC_எண்);
மாறிலி uint32_t R_AL_EN_AN_MASK=R_AL_MASK|EN_AN_MASK;
மாறிலி uint32_t L_EN_MASK=L_MASK|U_MASK(U_EUROPEAN_எண்);

மாறிலி uint32_t ES_CS_ET_ON_BN_NSM_MASK=
    U_MASK(U_EUROPEAN_எண்_SEPARATOR)|
    U_MASK(U_COMMON_எண்_SEPARATOR)|
    U_MASK(U_EUROPEAN_எண்_TERMINATOR)|
    U_MASK(U_OTHER_NEUTRAL)|
    U_MASK(U_BOUNDARY_NEUTRAL)|
    U_MASK(U_DIR_NON_SPACING_MARK);
மாறிலி uint32_t L_EN_ES_CS_ET_ON_BN_NSM_MASK=L_EN_MASK|ES_CS_ET_ON_BN_NSM_MASK;
மாறிலி uint32_t R_AL_AN_EN_ES_CS_ET_ON_BN_NSM_MASK=R_AL_MASK|EN_AN_MASK|ES_CS_ET_ON_BN_NSM_MASK;

// We scan the whole label and check both for whether it contains RTL characters
// and whether it passes the BiDi Rule.
// In a BiDi domain name, all labels must pass the BiDi Rule, but we might find
// that a domain name is a BiDi domain name (has an RTL label) only after
// processing several earlier labels.
வெறுமை
UTS46::checkLabelBiDi(மாறிலி char16_t *label, int32_t labelLength, IDNAInfo &info) மாறிலி {
    // IDNA2008 BiDi rule
    // Get the directionality of the first character.
    UChar32 c;
    int32_t i=0;
    U16_NEXT_UNSAFE(label, i, c);
    uint32_t firstMask=U_MASK(u_charDirection(c));
    // 1. The first character must be a character with BIDI property L, R
    // or AL.  If it has the R or AL property, it is an RTL label; if it
    // has the L property, it is an LTR label.
    எனில்((firstMask&~L_R_AL_MASK)!=0) {
        info.isOkBiDi=பொய்;
    }
    // Get the directionality of the last non-NSM character.
    uint32_t lastMask;
    க்கு(;;) {
        எனில்(i>=labelLength) {
            lastMask=firstMask;
            நிறுத்து;
        }
        U16_PREV_UNSAFE(label, labelLength, c);
        UCharDirection dir=u_charDirection(c);
        எனில்(dir!=U_DIR_NON_SPACING_MARK) {
            lastMask=U_MASK(dir);
            நிறுத்து;
        }
    }
    // 3. In an RTL label, the end of the label must be a character with
    // BIDI property R, AL, EN or AN, followed by zero or more
    // characters with BIDI property NSM.
    // 6. In an LTR label, the end of the label must be a character with
    // BIDI property L or EN, followed by zero or more characters with
    // BIDI property NSM.
    எனில்( (firstMask&L_MASK)!=0 ?
            (lastMask&~L_EN_MASK)!=0 :
            (lastMask&~R_AL_EN_AN_MASK)!=0
    ) {
        info.isOkBiDi=பொய்;
    }
    // Add the directionalities of the intervening characters.
    uint32_t mask=firstMask|lastMask;
    போது(i<labelLength) {
        U16_NEXT_UNSAFE(label, i, c);
        mask|=U_MASK(u_charDirection(c));
    }
    எனில்(firstMask&L_MASK) {
        // 5. In an LTR label, only characters with the BIDI properties L, EN,
        // ES, CS, ET, ON, BN and NSM are allowed.
        எனில்((mask&~L_EN_ES_CS_ET_ON_BN_NSM_MASK)!=0) {
            info.isOkBiDi=பொய்;
        }
    } இல்லையெனில் {
        // 2. In an RTL label, only characters with the BIDI properties R, AL,
        // AN, EN, ES, CS, ET, ON, BN and NSM are allowed.
        எனில்((mask&~R_AL_AN_EN_ES_CS_ET_ON_BN_NSM_MASK)!=0) {
            info.isOkBiDi=பொய்;
        }
        // 4. In an RTL label, if an EN is present, no AN may be present, and
        // vice versa.
        எனில்((mask&EN_AN_MASK)==EN_AN_MASK) {
            info.isOkBiDi=பொய்;
        }
    }
    // An RTL label is a label that contains at least one character of type
    // R, AL or AN. [...]
    // A "BIDI domain name" is a domain name that contains at least one RTL
    // label. [...]
    // The following rule, consisting of six conditions, applies to labels
    // in BIDI domain names.
    எனில்((mask&R_AL_AN_MASK)!=0) {
        info.isBiDi=உண்மை;
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
நிலையான UBool
isASCIIOkBiDi(மாறிலி char16_t *s, int32_t length) {
    int32_t labelStart=0;
    க்கு(int32_t i=0; i<length; ++i) {
        char16_t c=s[i];
        எனில்(c==0x2e) {  // dot
            எனில்(i>labelStart) {
                c=s[i-1];
                எனில்(!(0x61<=c && c<=0x7a) && !(0x30<=c && c<=0x39)) {
                    // Last character in the label is not an L or EN.
                    திரும்பு பொய்;
                }
            }
            labelStart=i+1;
        } இல்லையெனில் எனில்(i==labelStart) {
            எனில்(!(0x61<=c && c<=0x7a)) {
                // First character in the label is not an L.
                திரும்பு பொய்;
            }
        } இல்லையெனில் {
            எனில்(c<=0x20 && (c>=0x1c || (9<=c && c<=0xd))) {
                // Intermediate character in the label is a B, S or WS.
                திரும்பு பொய்;
            }
        }
    }
    திரும்பு உண்மை;
}

// UTF-8 version, called for source ASCII prefix.
// Can contain uppercase A-Z.
// s[length-1] must be the trailing dot.
நிலையான UBool
isASCIIOkBiDi(மாறிலி எழுத்து *s, int32_t length) {
    int32_t labelStart=0;
    க்கு(int32_t i=0; i<length; ++i) {
        எழுத்து c=s[i];
        எனில்(c==0x2e) {  // dot
            எனில்(i>labelStart) {
                c=s[i-1];
                எனில்(!(0x61<=c && c<=0x7a) && !(0x41<=c && c<=0x5a) && !(0x30<=c && c<=0x39)) {
                    // Last character in the label is not an L or EN.
                    திரும்பு பொய்;
                }
            }
            labelStart=i+1;
        } இல்லையெனில் எனில்(i==labelStart) {
            எனில்(!(0x61<=c && c<=0x7a) && !(0x41<=c && c<=0x5a)) {
                // First character in the label is not an L.
                திரும்பு பொய்;
            }
        } இல்லையெனில் {
            எனில்(c<=0x20 && (c>=0x1c || (9<=c && c<=0xd))) {
                // Intermediate character in the label is a B, S or WS.
                திரும்பு பொய்;
            }
        }
    }
    திரும்பு உண்மை;
}

UBool
UTS46::isLabelOkContextJ(மாறிலி char16_t *label, int32_t labelLength) மாறிலி {
    // [IDNA2008-Tables]
    // 200C..200D  ; CONTEXTJ    # ZERO WIDTH NON-JOINER..ZERO WIDTH JOINER
    க்கு(int32_t i=0; i<labelLength; ++i) {
        எனில்(label[i]==0x200c) {
            // Appendix A.1. ZERO WIDTH NON-JOINER
            // Rule Set:
            //  False;
            //  If Canonical_Combining_Class(Before(cp)) .eq.  Virama Then True;
            //  If RegExpMatch((Joining_Type:{L,D})(Joining_Type:T)*\u200C
            //     (Joining_Type:T)*(Joining_Type:{R,D})) Then True;
            எனில்(i==0) {
                திரும்பு பொய்;
            }
            UChar32 c;
            int32_t j=i;
            U16_PREV_UNSAFE(label, j, c);
            எனில்(uts46Norm2.getCombiningClass(c)==9) {
                தொடர்;
            }
            // check precontext (Joining_Type:{L,D})(Joining_Type:T)*
            க்கு(;;) {
                UJoiningType type=ubidi_getJoiningType(c);
                எனில்(type==U_JT_TRANSPARENT) {
                    எனில்(j==0) {
                        திரும்பு பொய்;
                    }
                    U16_PREV_UNSAFE(label, j, c);
                } இல்லையெனில் எனில்(type==U_JT_இடது_JOINING || type==U_JT_DUAL_JOINING) {
                    நிறுத்து;  // precontext fulfilled
                } இல்லையெனில் {
                    திரும்பு பொய்;
                }
            }
            // check postcontext (Joining_Type:T)*(Joining_Type:{R,D})
            க்கு(j=i+1;;) {
                எனில்(j==labelLength) {
                    திரும்பு பொய்;
                }
                U16_NEXT_UNSAFE(label, j, c);
                UJoiningType type=ubidi_getJoiningType(c);
                எனில்(type==U_JT_TRANSPARENT) {
                    // just skip this character
                } இல்லையெனில் எனில்(type==U_JT_வலது_JOINING || type==U_JT_DUAL_JOINING) {
                    நிறுத்து;  // postcontext fulfilled
                } இல்லையெனில் {
                    திரும்பு பொய்;
                }
            }
        } இல்லையெனில் எனில்(label[i]==0x200d) {
            // Appendix A.2. ZERO WIDTH JOINER (U+200D)
            // Rule Set:
            //  False;
            //  If Canonical_Combining_Class(Before(cp)) .eq.  Virama Then True;
            எனில்(i==0) {
                திரும்பு பொய்;
            }
            UChar32 c;
            int32_t j=i;
            U16_PREV_UNSAFE(label, j, c);
            எனில்(uts46Norm2.getCombiningClass(c)!=9) {
                திரும்பு பொய்;
            }
        }
    }
    திரும்பு உண்மை;
}

வெறுமை
UTS46::checkLabelContextO(மாறிலி char16_t *label, int32_t labelLength, IDNAInfo &info) மாறிலி {
    int32_t labelEnd=labelLength-1;  // inclusive
    int32_t arabicDigits=0;  // -1 for 066x, +1 for 06Fx
    க்கு(int32_t i=0; i<=labelEnd; ++i) {
        UChar32 c=label[i];
        எனில்(c<0xb7) {
            // ASCII fastpath
        } இல்லையெனில் எனில்(c<=0x6f9) {
            எனில்(c==0xb7) {
                // Appendix A.3. MIDDLE DOT (U+00B7)
                // Rule Set:
                //  False;
                //  If Before(cp) .eq.  U+006C And
                //     After(cp) .eq.  U+006C Then True;
                எனில்(!(0<i && label[i-1]==0x6c &&
                     i<labelEnd && label[i+1]==0x6c)) {
                    info.labelErrors|=UIDNA_ERROR_CONTEXTO_PUNCTUATION;
                }
            } இல்லையெனில் எனில்(c==0x375) {
                // Appendix A.4. GREEK LOWER NUMERAL SIGN (KERAIA) (U+0375)
                // Rule Set:
                //  False;
                //  If Script(After(cp)) .eq.  Greek Then True;
                UScriptCode script=USCRIPT_INVALID_CODE;
                எனில்(i<labelEnd) {
                    UErrorCode errorCode=U_ZERO_ERROR;
                    int32_t j=i+1;
                    U16_NEXT(label, j, labelLength, c);
                    script=uscript_getScript(c, &errorCode);
                }
                எனில்(script!=USCRIPT_GREEK) {
                    info.labelErrors|=UIDNA_ERROR_CONTEXTO_PUNCTUATION;
                }
            } இல்லையெனில் எனில்(c==0x5f3 || c==0x5f4) {
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
                எனில்(0<i) {
                    UErrorCode errorCode=U_ZERO_ERROR;
                    int32_t j=i;
                    U16_PREV(label, 0, j, c);
                    script=uscript_getScript(c, &errorCode);
                }
                எனில்(script!=USCRIPT_HEBREW) {
                    info.labelErrors|=UIDNA_ERROR_CONTEXTO_PUNCTUATION;
                }
            } இல்லையெனில் எனில்(0x660<=c /* && c<=0x6f9 */) {
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
                எனில்(c<=0x669) {
                    எனில்(arabicDigits>0) {
                        info.labelErrors|=UIDNA_ERROR_CONTEXTO_DIGITS;
                    }
                    arabicDigits=-1;
                } இல்லையெனில் எனில்(0x6f0<=c) {
                    எனில்(arabicDigits<0) {
                        info.labelErrors|=UIDNA_ERROR_CONTEXTO_DIGITS;
                    }
                    arabicDigits=1;
                }
            }
        } இல்லையெனில் எனில்(c==0x30fb) {
            // Appendix A.7. KATAKANA MIDDLE DOT (U+30FB)
            // Rule Set:
            //  False;
            //  For All Characters:
            //    If Script(cp) .in. {Hiragana, Katakana, Han} Then True;
            //  End For;
            UErrorCode errorCode=U_ZERO_ERROR;
            க்கு(முழு j=0;;) {
                எனில்(j>labelEnd) {
                    info.labelErrors|=UIDNA_ERROR_CONTEXTO_PUNCTUATION;
                    நிறுத்து;
                }
                U16_NEXT(label, j, labelLength, c);
                UScriptCode script=uscript_getScript(c, &errorCode);
                எனில்(script==USCRIPT_HIRAGANA || script==USCRIPT_KATAKANA || script==USCRIPT_HAN) {
                    நிறுத்து;
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
    திரும்பு reinterpret_cast<UIDNA *>(IDNA::createUTS46Instance(options, *pErrorCode));
}

U_CAPI வெறுமை U_EXPORT2
uidna_close(UIDNA *idna) {
    delete reinterpret_cast<IDNA *>(idna);
}

நிலையான UBool
checkArgs(மாறிலி வெறுமை *label, int32_t length,
          வெறுமை *dest, int32_t capacity,
          UIDNAInfo *pInfo, UErrorCode *pErrorCode) {
    எனில்(U_FAILURE(*pErrorCode)) {
        திரும்பு பொய்;
    }
    // sizeof(UIDNAInfo)=16 in the first API version.
    எனில்(pInfo==nullptr || pInfo->size<16) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு பொய்;
    }
    எனில்( (label==nullptr ? length!=0 : length<-1) ||
        (dest==nullptr ? capacity!=0 : capacity<0) ||
        (dest==label && label!=nullptr)
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு பொய்;
    }
    // Set all *pInfo bytes to 0 except for the size field itself.
    uprv_memset(&pInfo->size+1, 0, pInfo->size-அளவு(pInfo->size));
    திரும்பு உண்மை;
}

நிலையான வெறுமை
idnaInfoToStruct(IDNAInfo &info, UIDNAInfo *pInfo) {
    pInfo->isTransitionalDifferent=info.isTransitionalDifferent();
    pInfo->errors=info.getErrors();
}

U_CAPI int32_t U_EXPORT2
uidna_labelToASCII(மாறிலி UIDNA *idna,
                   மாறிலி char16_t *label, int32_t length,
                   char16_t *dest, int32_t capacity,
                   UIDNAInfo *pInfo, UErrorCode *pErrorCode) {
    எனில்(!checkArgs(label, length, dest, capacity, pInfo, pErrorCode)) {
        திரும்பு 0;
    }
    UnicodeString src(length < 0, label, length);
    UnicodeString destString(dest, 0, capacity);
    IDNAInfo info;
    reinterpret_cast<மாறிலி IDNA *>(idna)->labelToASCII(src, destString, info, *pErrorCode);
    idnaInfoToStruct(info, pInfo);
    திரும்பு destString.extract(dest, capacity, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uidna_labelToUnicode(மாறிலி UIDNA *idna,
                     மாறிலி char16_t *label, int32_t length,
                     char16_t *dest, int32_t capacity,
                     UIDNAInfo *pInfo, UErrorCode *pErrorCode) {
    எனில்(!checkArgs(label, length, dest, capacity, pInfo, pErrorCode)) {
        திரும்பு 0;
    }
    UnicodeString src(length < 0, label, length);
    UnicodeString destString(dest, 0, capacity);
    IDNAInfo info;
    reinterpret_cast<மாறிலி IDNA *>(idna)->labelToUnicode(src, destString, info, *pErrorCode);
    idnaInfoToStruct(info, pInfo);
    திரும்பு destString.extract(dest, capacity, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uidna_nameToASCII(மாறிலி UIDNA *idna,
                  மாறிலி char16_t *name, int32_t length,
                  char16_t *dest, int32_t capacity,
                  UIDNAInfo *pInfo, UErrorCode *pErrorCode) {
    எனில்(!checkArgs(name, length, dest, capacity, pInfo, pErrorCode)) {
        திரும்பு 0;
    }
    UnicodeString src(length < 0, name, length);
    UnicodeString destString(dest, 0, capacity);
    IDNAInfo info;
    reinterpret_cast<மாறிலி IDNA *>(idna)->nameToASCII(src, destString, info, *pErrorCode);
    idnaInfoToStruct(info, pInfo);
    திரும்பு destString.extract(dest, capacity, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uidna_nameToUnicode(மாறிலி UIDNA *idna,
                    மாறிலி char16_t *name, int32_t length,
                    char16_t *dest, int32_t capacity,
                    UIDNAInfo *pInfo, UErrorCode *pErrorCode) {
    எனில்(!checkArgs(name, length, dest, capacity, pInfo, pErrorCode)) {
        திரும்பு 0;
    }
    UnicodeString src(length < 0, name, length);
    UnicodeString destString(dest, 0, capacity);
    IDNAInfo info;
    reinterpret_cast<மாறிலி IDNA *>(idna)->nameToUnicode(src, destString, info, *pErrorCode);
    idnaInfoToStruct(info, pInfo);
    திரும்பு destString.extract(dest, capacity, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uidna_labelToASCII_UTF8(மாறிலி UIDNA *idna,
                        மாறிலி எழுத்து *label, int32_t length,
                        எழுத்து *dest, int32_t capacity,
                        UIDNAInfo *pInfo, UErrorCode *pErrorCode) {
    எனில்(!checkArgs(label, length, dest, capacity, pInfo, pErrorCode)) {
        திரும்பு 0;
    }
    StringPiece src(label, length<0 ? static_cast<int32_t>(uprv_strlen(label)) : length);
    திரும்பு ByteSinkUtil::viaByteSinkToTerminatedChars(
        dest, capacity,
        [&](ByteSink& sink, UErrorCode& status) {
            IDNAInfo info;
            reinterpret_cast<மாறிலி IDNA *>(idna)->labelToASCII_UTF8(src, sink, info, status);
            idnaInfoToStruct(info, pInfo);
        },
        *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uidna_labelToUnicodeUTF8(மாறிலி UIDNA *idna,
                         மாறிலி எழுத்து *label, int32_t length,
                         எழுத்து *dest, int32_t capacity,
                         UIDNAInfo *pInfo, UErrorCode *pErrorCode) {
    எனில்(!checkArgs(label, length, dest, capacity, pInfo, pErrorCode)) {
        திரும்பு 0;
    }
    StringPiece src(label, length<0 ? static_cast<int32_t>(uprv_strlen(label)) : length);
    திரும்பு ByteSinkUtil::viaByteSinkToTerminatedChars(
        dest, capacity,
        [&](ByteSink& sink, UErrorCode& status) {
            IDNAInfo info;
            reinterpret_cast<மாறிலி IDNA *>(idna)->labelToUnicodeUTF8(src, sink, info, status);
            idnaInfoToStruct(info, pInfo);
        },
        *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uidna_nameToASCII_UTF8(மாறிலி UIDNA *idna,
                       மாறிலி எழுத்து *name, int32_t length,
                       எழுத்து *dest, int32_t capacity,
                       UIDNAInfo *pInfo, UErrorCode *pErrorCode) {
    எனில்(!checkArgs(name, length, dest, capacity, pInfo, pErrorCode)) {
        திரும்பு 0;
    }
    StringPiece src(name, length<0 ? static_cast<int32_t>(uprv_strlen(name)) : length);
    திரும்பு ByteSinkUtil::viaByteSinkToTerminatedChars(
        dest, capacity,
        [&](ByteSink& sink, UErrorCode& status) {
            IDNAInfo info;
            reinterpret_cast<மாறிலி IDNA *>(idna)->nameToASCII_UTF8(src, sink, info, status);
            idnaInfoToStruct(info, pInfo);
        },
        *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uidna_nameToUnicodeUTF8(மாறிலி UIDNA *idna,
                        மாறிலி எழுத்து *name, int32_t length,
                        எழுத்து *dest, int32_t capacity,
                        UIDNAInfo *pInfo, UErrorCode *pErrorCode) {
    எனில்(!checkArgs(name, length, dest, capacity, pInfo, pErrorCode)) {
        திரும்பு 0;
    }
    StringPiece src(name, length<0 ? static_cast<int32_t>(uprv_strlen(name)) : length);
    திரும்பு ByteSinkUtil::viaByteSinkToTerminatedChars(
        dest, capacity,
        [&](ByteSink& sink, UErrorCode& status) {
            IDNAInfo info;
            reinterpret_cast<மாறிலி IDNA *>(idna)->nameToUnicodeUTF8(src, sink, info, status);
            idnaInfoToStruct(info, pInfo);
        },
        *pErrorCode);
}

#endif  // UCONFIG_NO_IDNA
