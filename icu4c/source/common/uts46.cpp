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

#شامل "unicode/utypes.h"

#اگر !UCONFIG_NO_IDNA

#شامل "unicode/bytestream.h"
#شامل "unicode/idna.h"
#شامل "unicode/normalizer2.h"
#شامل "unicode/uscript.h"
#شامل "unicode/ustring.h"
#شامل "unicode/utf16.h"
#شامل "bytesinkutil.h"
#شامل "cmemory.h"
#شامل "cstring.h"
#شامل "punycode.h"
#شامل "ubidi_props.h"

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

ایستا درونخطی UBool
isASCIIString(ثابت UnicodeString &dest) {
    ثابت char16_t *s=dest.getBuffer();
    ثابت char16_t *limit=s+dest.length();
    وقتی(s<limit) {
        اگر(*s++>0x7f) {
            بازگشت نادرست;
        }
    }
    بازگشت درست;
}

ایستا UBool
isASCIIOkBiDi(ثابت char16_t *s, int32_t length);

ایستا UBool
isASCIIOkBiDi(ثابت نویسه *s, int32_t length);

// IDNA class default implementations -------------------------------------- ***

IDNA::~IDNA() {}

تهی
IDNA::labelToASCII_UTF8(StringPiece label, ByteSink &dest,
                        IDNAInfo &info, UErrorCode &errorCode) ثابت {
    اگر(U_SUCCESS(errorCode)) {
        UnicodeString destString;
        labelToASCII(UnicodeString::fromUTF8(label), destString,
                     info, errorCode).toUTF8(dest);
    }
}

تهی
IDNA::labelToUnicodeUTF8(StringPiece label, ByteSink &dest,
                         IDNAInfo &info, UErrorCode &errorCode) ثابت {
    اگر(U_SUCCESS(errorCode)) {
        UnicodeString destString;
        labelToUnicode(UnicodeString::fromUTF8(label), destString,
                       info, errorCode).toUTF8(dest);
    }
}

تهی
IDNA::nameToASCII_UTF8(StringPiece name, ByteSink &dest,
                       IDNAInfo &info, UErrorCode &errorCode) ثابت {
    اگر(U_SUCCESS(errorCode)) {
        UnicodeString destString;
        nameToASCII(UnicodeString::fromUTF8(name), destString,
                    info, errorCode).toUTF8(dest);
    }
}

تهی
IDNA::nameToUnicodeUTF8(StringPiece name, ByteSink &dest,
                        IDNAInfo &info, UErrorCode &errorCode) ثابت {
    اگر(U_SUCCESS(errorCode)) {
        UnicodeString destString;
        nameToUnicode(UnicodeString::fromUTF8(name), destString,
                      info, errorCode).toUTF8(dest);
    }
}

// UTS46 class declaration ------------------------------------------------- ***

رده UTS46 : عمومی IDNA {
عمومی:
    UTS46(uint32_t options, UErrorCode &errorCode);
    مجازی ~UTS46();

    مجازی UnicodeString&
    labelToASCII(ثابت UnicodeString& label,
                 UnicodeString& dest U_LIFETIME_BOUND,
                 IDNAInfo& info,
                 UErrorCode& errorCode) ثابت override;

    مجازی UnicodeString&
    labelToUnicode(ثابت UnicodeString& label,
                   UnicodeString& dest U_LIFETIME_BOUND,
                   IDNAInfo& info,
                   UErrorCode& errorCode) ثابت override;

    مجازی UnicodeString&
    nameToASCII(ثابت UnicodeString& name,
                UnicodeString& dest U_LIFETIME_BOUND,
                IDNAInfo& info,
                UErrorCode& errorCode) ثابت override;

    مجازی UnicodeString&
    nameToUnicode(ثابت UnicodeString& name,
                  UnicodeString& dest U_LIFETIME_BOUND,
                  IDNAInfo& info,
                  UErrorCode& errorCode) ثابت override;

    مجازی تهی
    labelToASCII_UTF8(StringPiece label, ByteSink &dest,
                      IDNAInfo &info, UErrorCode &errorCode) ثابت override;

    مجازی تهی
    labelToUnicodeUTF8(StringPiece label, ByteSink &dest,
                       IDNAInfo &info, UErrorCode &errorCode) ثابت override;

    مجازی تهی
    nameToASCII_UTF8(StringPiece name, ByteSink &dest,
                     IDNAInfo &info, UErrorCode &errorCode) ثابت override;

    مجازی تهی
    nameToUnicodeUTF8(StringPiece name, ByteSink &dest,
                      IDNAInfo &info, UErrorCode &errorCode) ثابت override;

خصوصی:
    UnicodeString&
    process(ثابت UnicodeString& src,
            UBool isLabel,
            UBool toASCII,
            UnicodeString& dest U_LIFETIME_BOUND,
            IDNAInfo& info,
            UErrorCode& errorCode) ثابت;

    تهی
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

    تهی
    checkLabelBiDi(ثابت char16_t *label, int32_t labelLength, IDNAInfo &info) ثابت;

    UBool
    isLabelOkContextJ(ثابت char16_t *label, int32_t labelLength) ثابت;

    تهی
    checkLabelContextO(ثابت char16_t *label, int32_t labelLength, IDNAInfo &info) ثابت;

    ثابت Normalizer2 &uts46Norm2;  // uts46.nrm
    uint32_t options;
};

IDNA *
IDNA::createUTS46Instance(uint32_t options, UErrorCode &errorCode) {
    اگر(U_SUCCESS(errorCode)) {
        IDNA *idna=جدید UTS46(options, errorCode);
        اگر(idna==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
        } وگرنه اگر(U_FAILURE(errorCode)) {
            delete idna;
            idna=nullptr;
        }
        بازگشت idna;
    } وگرنه {
        بازگشت nullptr;
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
    بازگشت process(label, درست, درست, dest, info, errorCode);
}

UnicodeString&
UTS46::labelToUnicode(ثابت UnicodeString& label,
                      UnicodeString& dest U_LIFETIME_BOUND,
                      IDNAInfo& info,
                      UErrorCode& errorCode) ثابت {
    بازگشت process(label, درست, نادرست, dest, info, errorCode);
}

UnicodeString&
UTS46::nameToASCII(ثابت UnicodeString& name,
                   UnicodeString& dest U_LIFETIME_BOUND,
                   IDNAInfo& info,
                   UErrorCode& errorCode) ثابت {
    process(name, نادرست, درست, dest, info, errorCode);
    اگر( dest.length()>=254 && (info.errors&UIDNA_ERROR_DOMAIN_NAME_TOO_LONG)==0 &&
        isASCIIString(dest) &&
        (dest.length()>254 || dest[253]!=0x2e)
    ) {
        info.errors|=UIDNA_ERROR_DOMAIN_NAME_TOO_LONG;
    }
    بازگشت dest;
}

UnicodeString&
UTS46::nameToUnicode(ثابت UnicodeString& name,
                     UnicodeString& dest U_LIFETIME_BOUND,
                     IDNAInfo& info,
                     UErrorCode& errorCode) ثابت {
    بازگشت process(name, نادرست, نادرست, dest, info, errorCode);
}

تهی
UTS46::labelToASCII_UTF8(StringPiece label, ByteSink &dest,
                         IDNAInfo &info, UErrorCode &errorCode) ثابت {
    processUTF8(label, درست, درست, dest, info, errorCode);
}

تهی
UTS46::labelToUnicodeUTF8(StringPiece label, ByteSink &dest,
                          IDNAInfo &info, UErrorCode &errorCode) ثابت {
    processUTF8(label, درست, نادرست, dest, info, errorCode);
}

تهی
UTS46::nameToASCII_UTF8(StringPiece name, ByteSink &dest,
                        IDNAInfo &info, UErrorCode &errorCode) ثابت {
    processUTF8(name, نادرست, درست, dest, info, errorCode);
}

تهی
UTS46::nameToUnicodeUTF8(StringPiece name, ByteSink &dest,
                         IDNAInfo &info, UErrorCode &errorCode) ثابت {
    processUTF8(name, نادرست, نادرست, dest, info, errorCode);
}

// UTS #46 data for ASCII characters.
// The normalizer (using uts46.nrm) maps uppercase ASCII letters to lowercase
// and passes through all other ASCII characters.
// If UIDNA_USE_STD3_RULES is set, then non-LDH characters are disallowed
// using this data.
// The ASCII fastpath also uses this data.
// Values: -1=disallowed  0==valid  1==mapped (lowercase)
ایستا ثابت int8_t asciiData[128]={
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
    اگر(U_FAILURE(errorCode)) {
        dest.setToBogus();
        بازگشت dest;
    }
    ثابت char16_t *srcArray=src.getBuffer();
    اگر(&dest==&src || srcArray==nullptr) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        dest.setToBogus();
        بازگشت dest;
    }
    // Arguments are fine, reset output values.
    dest.remove();
    info.reset();
    int32_t srcLength=src.length();
    اگر(srcLength==0) {
        info.errors|=UIDNA_ERROR_EMPTY_LABEL;
        بازگشت dest;
    }
    char16_t *destArray=dest.getBuffer(srcLength);
    اگر(destArray==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        بازگشت dest;
    }
    // ASCII fastpath
    UBool disallowNonLDHDot=(options&UIDNA_USE_STD3_RULES)!=0;
    int32_t labelStart=0;
    int32_t i;
    برای(i=0;; ++i) {
        اگر(i==srcLength) {
            اگر(toASCII) {
                اگر((i-labelStart)>63) {
                    info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
                }
                // There is a trailing dot if labelStart==i.
                اگر(!isLabel && i>=254 && (i>254 || labelStart<i)) {
                    info.errors|=UIDNA_ERROR_DOMAIN_NAME_TOO_LONG;
                }
            }
            info.errors|=info.labelErrors;
            dest.releaseBuffer(i);
            بازگشت dest;
        }
        char16_t c=srcArray[i];
        اگر(c>0x7f) {
            بشکن;
        }
        صحیح cData=asciiData[c];
        اگر(cData>0) {
            destArray[i]=c+0x20;  // Lowercase an uppercase ASCII letter.
        } وگرنه اگر(cData<0 && disallowNonLDHDot) {
            بشکن;  // Replacing with U+FFFD can be complicated for toASCII.
        } وگرنه {
            destArray[i]=c;
            اگر(c==0x2d) {  // hyphen
                اگر(i==(labelStart+3) && srcArray[i-1]==0x2d) {
                    // "??--..." is Punycode or forbidden.
                    ++i;  // '-' was copied to dest already
                    بشکن;
                }
                اگر(i==labelStart) {
                    // label starts with "-"
                    info.labelErrors|=UIDNA_ERROR_LEADING_HYPHEN;
                }
                اگر((i+1)==srcLength || srcArray[i+1]==0x2e) {
                    // label ends with "-"
                    info.labelErrors|=UIDNA_ERROR_TRAILING_HYPHEN;
                }
            } وگرنه اگر(c==0x2e) {  // dot
                اگر(isLabel) {
                    // Replacing with U+FFFD can be complicated for toASCII.
                    ++i;  // '.' was copied to dest already
                    بشکن;
                }
                اگر(i==labelStart) {
                    info.labelErrors|=UIDNA_ERROR_EMPTY_LABEL;
                }
                اگر(toASCII && (i-labelStart)>63) {
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
    اگر( info.isBiDi && U_SUCCESS(errorCode) && (info.errors&severeErrors)==0 &&
        (!info.isOkBiDi || (labelStart>0 && !isASCIIOkBiDi(dest.getBuffer(), labelStart)))
    ) {
        info.errors|=UIDNA_ERROR_BIDI;
    }
    بازگشت dest;
}

تهی
UTS46::processUTF8(StringPiece src,
                   UBool isLabel, UBool toASCII,
                   ByteSink &dest,
                   IDNAInfo &info, UErrorCode &errorCode) ثابت {
    اگر(U_FAILURE(errorCode)) {
        بازگشت;
    }
    ثابت نویسه *srcArray=src.data();
    int32_t srcLength=src.length();
    اگر(srcArray==nullptr && srcLength!=0) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت;
    }
    // Arguments are fine, reset output values.
    info.reset();
    اگر(srcLength==0) {
        info.errors|=UIDNA_ERROR_EMPTY_LABEL;
        dest.Flush();
        بازگشت;
    }
    UnicodeString destString;
    int32_t labelStart=0;
    اگر(srcLength<=256) {  // length of stackArray[]
        // ASCII fastpath
        نویسه پشته_Array[256];
        int32_t destCapacity;
        نویسه *destArray=dest.GetAppendBuffer(srcLength, srcLength+20,
                                             پشته_Array, UPRV_LENGTHOF(پشته_Array), &destCapacity);
        UBool disallowNonLDHDot=(options&UIDNA_USE_STD3_RULES)!=0;
        int32_t i;
        برای(i=0;; ++i) {
            اگر(i==srcLength) {
                اگر(toASCII) {
                    اگر((i-labelStart)>63) {
                        info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
                    }
                    // There is a trailing dot if labelStart==i.
                    اگر(!isLabel && i>=254 && (i>254 || labelStart<i)) {
                        info.errors|=UIDNA_ERROR_DOMAIN_NAME_TOO_LONG;
                    }
                }
                info.errors|=info.labelErrors;
                dest.Append(destArray, i);
                dest.Flush();
                بازگشت;
            }
            نویسه c=srcArray[i];
            اگر (static_cast<int8_t>(c) < 0) { // (uint8_t)c>0x7f
                بشکن;
            }
            صحیح cData = asciiData[static_cast<صحیح>(c)]; // Cast: gcc warns about indexing with a char.
            اگر(cData>0) {
                destArray[i]=c+0x20;  // Lowercase an uppercase ASCII letter.
            } وگرنه اگر(cData<0 && disallowNonLDHDot) {
                بشکن;  // Replacing with U+FFFD can be complicated for toASCII.
            } وگرنه {
                destArray[i]=c;
                اگر(c==0x2d) {  // hyphen
                    اگر(i==(labelStart+3) && srcArray[i-1]==0x2d) {
                        // "??--..." is Punycode or forbidden.
                        بشکن;
                    }
                    اگر(i==labelStart) {
                        // label starts with "-"
                        info.labelErrors|=UIDNA_ERROR_LEADING_HYPHEN;
                    }
                    اگر((i+1)==srcLength || srcArray[i+1]==0x2e) {
                        // label ends with "-"
                        info.labelErrors|=UIDNA_ERROR_TRAILING_HYPHEN;
                    }
                } وگرنه اگر(c==0x2e) {  // dot
                    اگر(isLabel) {
                        بشکن;  // Replacing with U+FFFD can be complicated for toASCII.
                    }
                    اگر(i==labelStart) {
                        info.labelErrors|=UIDNA_ERROR_EMPTY_LABEL;
                    }
                    اگر(toASCII && (i-labelStart)>63) {
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
    } وگرنه {
        // src is too long for the ASCII fastpath implementation.
        processUnicode(UnicodeString::fromUTF8(src), 0, 0,
                       isLabel, toASCII,
                       destString, info, errorCode);
    }
    destString.toUTF8(dest);  // calls dest.Flush()
    اگر(toASCII && !isLabel) {
        // length==labelStart==254 means that there is a trailing dot (ok) and
        // destString is empty (do not index at 253-labelStart).
        int32_t length=labelStart+destString.length();
        اگر( length>=254 && isASCIIString(destString) &&
            (length>254 ||
             (labelStart<254 && destString[253-labelStart]!=0x2e))
        ) {
            info.errors|=UIDNA_ERROR_DOMAIN_NAME_TOO_LONG;
        }
    }
    اگر( info.isBiDi && U_SUCCESS(errorCode) && (info.errors&severeErrors)==0 &&
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
    اگر(mappingStart==0) {
        uts46Norm2.normalize(src, dest, errorCode);
    } وگرنه {
        uts46Norm2.normalizeSecondAndAppend(dest, src.tempSubString(mappingStart), errorCode);
    }
    اگر(U_FAILURE(errorCode)) {
        بازگشت dest;
    }
    UBool doMapDevChars=
        toASCII ? (options&UIDNA_NONTRANSITIONAL_TO_ASCII)==0 :
                  (options&UIDNA_NONTRANSITIONAL_TO_UNICODE)==0;
    ثابت char16_t *destArray=dest.getBuffer();
    int32_t destLength=dest.length();
    int32_t labelLimit=labelStart;
    وقتی(labelLimit<destLength) {
        char16_t c=destArray[labelLimit];
        اگر(c==0x2e && !isLabel) {
            int32_t labelLength=labelLimit-labelStart;
            int32_t newLength=processLabel(dest, labelStart, labelLength,
                                            toASCII, info, errorCode);
            info.errors|=info.labelErrors;
            info.labelErrors=0;
            اگر(U_FAILURE(errorCode)) {
                بازگشت dest;
            }
            destArray=dest.getBuffer();
            destLength+=newLength-labelLength;
            labelLimit=labelStart+=newLength+1;
            ادامه;
        } وگرنه اگر(c<0xdf) {
            // pass
        } وگرنه اگر(c<=0x200d && (c==0xdf || c==0x3c2 || c>=0x200c)) {
            info.isTransDiff=درست;
            اگر(doMapDevChars) {
                destLength=mapDevChars(dest, labelStart, labelLimit, errorCode);
                اگر(U_FAILURE(errorCode)) {
                    بازگشت dest;
                }
                destArray=dest.getBuffer();
                // All deviation characters have been mapped, no need to check for them again.
                doMapDevChars=نادرست;
                // Do not increment labelLimit in case c was removed.
                ادامه;
            }
        } وگرنه اگر(U16_IS_SURROGATE(c)) {
            اگر(U16_IS_SURROGATE_LEAD(c) ?
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
    اگر(0==labelStart || labelStart<labelLimit) {
        processLabel(dest, labelStart, labelLimit-labelStart,
                      toASCII, info, errorCode);
        info.errors|=info.labelErrors;
    }
    بازگشت dest;
}

int32_t
UTS46::mapDevChars(UnicodeString &dest, int32_t labelStart, int32_t mappingStart,
                   UErrorCode &errorCode) ثابت {
    اگر(U_FAILURE(errorCode)) {
        بازگشت 0;
    }
    int32_t length=dest.length();
    char16_t *s=dest.getBuffer(dest[mappingStart]==0xdf ? length+1 : length);
    اگر(s==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        بازگشت length;
    }
    int32_t capacity=dest.getCapacity();
    UBool didMapDevChars=نادرست;
    int32_t readIndex=mappingStart, writeIndex=mappingStart;
    انجامبده {
        char16_t c=s[readIndex++];
        انتخاب(c) {
        حالت 0xdf:
            // Map sharp s to ss.
            didMapDevChars=درست;
            s[writeIndex++]=0x73;  // Replace sharp s with first s.
            // Insert second s and account for possible buffer reallocation.
            اگر(writeIndex==readIndex) {
                اگر(length==capacity) {
                    dest.releaseBuffer(length);
                    s=dest.getBuffer(length+1);
                    اگر(s==nullptr) {
                        errorCode=U_MEMORY_ALLOCATION_ERROR;
                        بازگشت length;
                    }
                    capacity=dest.getCapacity();
                }
                u_memmove(s+writeIndex+1, s+writeIndex, length-writeIndex);
                ++readIndex;
            }
            s[writeIndex++]=0x73;
            ++length;
            بشکن;
        حالت 0x3c2:  // Map final sigma to nonfinal sigma.
            didMapDevChars=درست;
            s[writeIndex++]=0x3c3;
            بشکن;
        حالت 0x200c:  // Ignore/remove ZWNJ.
        حالت 0x200d:  // Ignore/remove ZWJ.
            didMapDevChars=درست;
            --length;
            بشکن;
        پیشفرض:
            // Only really necessary if writeIndex was different from readIndex.
            s[writeIndex++]=c;
            بشکن;
        }
    } وقتی(writeIndex<length);
    dest.releaseBuffer(length);
    اگر(didMapDevChars) {
        // Mapping deviation characters might have resulted in an un-NFC string.
        // We could use either the NFC or the UTS #46 normalizer.
        // By using the UTS #46 normalizer again, we avoid having to load a second .nrm data file.
        UnicodeString normalized;
        uts46Norm2.normalize(dest.tempSubString(labelStart), normalized, errorCode);
        اگر(U_SUCCESS(errorCode)) {
            dest.replace(labelStart, 0x7fffffff, normalized);
            اگر(dest.isBogus()) {
                errorCode=U_MEMORY_ALLOCATION_ERROR;
            }
            بازگشت dest.length();
        }
    }
    بازگشت length;
}

// Replace the label in dest with the label string, if the label was modified.
// If &label==&dest then the label was modified in-place and labelLength
// is the new label length, different from label.length().
// If &label!=&dest then labelLength==label.length().
// Returns labelLength (= the new label length).
ایستا int32_t
replaceLabel(UnicodeString &dest, int32_t destLabelStart, int32_t destLabelLength,
             ثابت UnicodeString &label, int32_t labelLength, UErrorCode &errorCode) {
    اگر(U_FAILURE(errorCode)) {
        بازگشت 0;
    }
    اگر(&label!=&dest) {
        dest.replace(destLabelStart, destLabelLength, label);
        اگر(dest.isBogus()) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            بازگشت 0;
        }
    }
    بازگشت labelLength;
}

int32_t
UTS46::processLabel(UnicodeString &dest,
                    int32_t labelStart, int32_t labelLength,
                    UBool toASCII,
                    IDNAInfo &info, UErrorCode &errorCode) ثابت {
    اگر(U_FAILURE(errorCode)) {
        بازگشت 0;
    }
    UnicodeString fromPunycode;
    UnicodeString *labelString;
    ثابت char16_t *label=dest.getBuffer()+labelStart;
    int32_t destLabelStart=labelStart;
    int32_t destLabelLength=labelLength;
    UBool wasPunycode;
    اگر(labelLength>=4 && label[0]==0x78 && label[1]==0x6e && label[2]==0x2d && label[3]==0x2d) {
        // Label starts with "xn--", try to un-Punycode it.
        // In IDNA2008, labels like "xn--" (decodes to an empty string) and
        // "xn--ASCII-" (decodes to just "ASCII") fail the round-trip validation from
        // comparing the ToUnicode input with the back-to-ToASCII output.
        // They are alternate encodings of the respective ASCII labels.
        // Ignore "xn---" here: It will fail Punycode.decode() which logically comes before
        // the round-trip verification.
        اگر(labelLength==4 || (labelLength>5 && label[labelLength-1]==u'-')) {
            info.labelErrors|=UIDNA_ERROR_INVALID_ACE_LABEL;
            بازگشت markBadACELabel(dest, labelStart, labelLength, toASCII, info, errorCode);
        }
        wasPunycode=درست;
        char16_t *unicodeBuffer=fromPunycode.getBuffer(-1);  // capacity==-1: most labels should fit
        اگر(unicodeBuffer==nullptr) {
            // Should never occur if we used capacity==-1 which uses the internal buffer.
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            بازگشت labelLength;
        }
        UErrorCode punycodeErrorCode=U_ZERO_ERROR;
        int32_t unicodeLength=u_strFromPunycode(label+4, labelLength-4,
                                                unicodeBuffer, fromPunycode.getCapacity(),
                                                nullptr, &punycodeErrorCode);
        اگر(punycodeErrorCode==U_BUFFER_OVERFLOW_ERROR) {
            fromPunycode.releaseBuffer(0);
            unicodeBuffer=fromPunycode.getBuffer(unicodeLength);
            اگر(unicodeBuffer==nullptr) {
                errorCode=U_MEMORY_ALLOCATION_ERROR;
                بازگشت labelLength;
            }
            punycodeErrorCode=U_ZERO_ERROR;
            unicodeLength=u_strFromPunycode(label+4, labelLength-4,
                                            unicodeBuffer, fromPunycode.getCapacity(),
                                            nullptr, &punycodeErrorCode);
        }
        fromPunycode.releaseBuffer(unicodeLength);
        اگر(U_FAILURE(punycodeErrorCode)) {
            info.labelErrors|=UIDNA_ERROR_PUNYCODE;
            بازگشت markBadACELabel(dest, labelStart, labelLength, toASCII, info, errorCode);
        }
        // Check for NFC, and for characters that are not
        // valid or deviation characters according to the normalizer.
        // If there is something wrong, then the string will change.
        // Note that the normalizer passes through non-LDH ASCII and deviation characters.
        // Deviation characters are ok in Punycode even in transitional processing.
        // In the code further below, if we find non-LDH ASCII and we have UIDNA_USE_STD3_RULES
        // then we will set UIDNA_ERROR_INVALID_ACE_LABEL there too.
        UBool isValid=uts46Norm2.isNormalized(fromPunycode, errorCode);
        اگر(U_FAILURE(errorCode)) {
            بازگشت labelLength;
        }
        // Unicode 15.1 UTS #46:
        // Added an additional condition in 4.1 Validity Criteria to
        // disallow labels such as xn--xn---epa., which do not round-trip.
        // --> Validity Criteria new criterion 4:
        // If not CheckHyphens, the label must not begin with “xn--”.
        اگر(!isValid || fromPunycode.startsWith(UnicodeString::readOnlyAlias(u"xn--"))) {
            info.labelErrors|=UIDNA_ERROR_INVALID_ACE_LABEL;
            بازگشت markBadACELabel(dest, labelStart, labelLength, toASCII, info, errorCode);
        }
        labelString=&fromPunycode;
        label=fromPunycode.getBuffer();
        labelStart=0;
        labelLength=fromPunycode.length();
    } وگرنه {
        wasPunycode=نادرست;
        labelString=&dest;
    }
    // Validity check
    اگر(labelLength==0) {
        info.labelErrors|=UIDNA_ERROR_EMPTY_LABEL;
        بازگشت replaceLabel(dest, destLabelStart, destLabelLength,
                            *labelString, labelLength, errorCode);
    }
    // labelLength>0
    اگر(labelLength>=4 && label[2]==0x2d && label[3]==0x2d) {
        // label starts with "??--"
        info.labelErrors|=UIDNA_ERROR_HYPHEN_3_4;
    }
    اگر(label[0]==0x2d) {
        // label starts with "-"
        info.labelErrors|=UIDNA_ERROR_LEADING_HYPHEN;
    }
    اگر(label[labelLength-1]==0x2d) {
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
    انجامبده {
        char16_t c=*s;
        اگر(c<=0x7f) {
            اگر(c==0x2e) {
                info.labelErrors|=UIDNA_ERROR_LABEL_HAS_DOT;
                *s=0xfffd;
            } وگرنه اگر(disallowNonLDHDot && asciiData[c]<0) {
                info.labelErrors|=UIDNA_ERROR_DISALLOWED;
                *s=0xfffd;
            }
        } وگرنه {
            oredChars|=c;
            اگر(c==0xfffd) {
                info.labelErrors|=UIDNA_ERROR_DISALLOWED;
            }
        }
        ++s;
    } وقتی(s<limit);
    // Check for a leading combining mark after other validity checks
    // so that we don't report UIDNA_ERROR_DISALLOWED for the U+FFFD from here.
    UChar32 c;
    int32_t cpLength=0;
    // "Unsafe" is ok because unpaired surrogates were mapped to U+FFFD.
    U16_NEXT_UNSAFE(label, cpLength, c);
    اگر((U_GET_GC_MASK(c)&U_GC_M_MASK)!=0) {
        info.labelErrors|=UIDNA_ERROR_LEADING_COMBINING_MARK;
        labelString->replace(labelStart, cpLength, static_cast<char16_t>(0xfffd));
        label=labelString->getBuffer()+labelStart;
        labelLength+=1-cpLength;
        اگر(labelString==&dest) {
            destLabelLength=labelLength;
        }
    }
    اگر((info.labelErrors&severeErrors)==0) {
        // Do contextual checks only if we do not have U+FFFD from a severe error
        // because U+FFFD can make these checks fail.
        اگر((options&UIDNA_CHECK_BIDI)!=0 && (!info.isBiDi || info.isOkBiDi)) {
            checkLabelBiDi(label, labelLength, info);
        }
        اگر( (options&UIDNA_CHECK_CONTEXTJ)!=0 && (oredChars&0x200c)==0x200c &&
            !isLabelOkContextJ(label, labelLength)
        ) {
            info.labelErrors|=UIDNA_ERROR_CONTEXTJ;
        }
        اگر((options&UIDNA_CHECK_CONTEXTO)!=0 && oredChars>=0xb7) {
            checkLabelContextO(label, labelLength, info);
        }
        اگر(toASCII) {
            اگر(wasPunycode) {
                // Leave a Punycode label unchanged if it has no severe errors.
                اگر(destLabelLength>63) {
                    info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
                }
                بازگشت destLabelLength;
            } وگرنه اگر(oredChars>=0x80) {
                // Contains non-ASCII characters.
                UnicodeString punycode;
                char16_t *buffer=punycode.getBuffer(63);  // 63==maximum DNS label length
                اگر(buffer==nullptr) {
                    errorCode=U_MEMORY_ALLOCATION_ERROR;
                    بازگشت destLabelLength;
                }
                buffer[0]=0x78;  // Write "xn--".
                buffer[1]=0x6e;
                buffer[2]=0x2d;
                buffer[3]=0x2d;
                UErrorCode punycodeErrorCode=U_ZERO_ERROR;
                int32_t punycodeLength=u_strToPunycode(label, labelLength,
                                                      buffer+4, punycode.getCapacity()-4,
                                                      nullptr, &punycodeErrorCode);
                اگر(punycodeErrorCode==U_BUFFER_OVERFLOW_ERROR) {
                    punycodeErrorCode=U_ZERO_ERROR;
                    punycode.releaseBuffer(4);
                    buffer=punycode.getBuffer(4+punycodeLength);
                    اگر(buffer==nullptr) {
                        errorCode=U_MEMORY_ALLOCATION_ERROR;
                        بازگشت destLabelLength;
                    }
                    punycodeLength=u_strToPunycode(label, labelLength,
                                                  buffer+4, punycode.getCapacity()-4,
                                                  nullptr, &punycodeErrorCode);
                }
                punycodeLength+=4;
                punycode.releaseBuffer(punycodeLength);
                اگر(U_FAILURE(punycodeErrorCode)) {
                    errorCode = punycodeErrorCode;
                    بازگشت destLabelLength;
                }
                اگر(punycodeLength>63) {
                    info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
                }
                بازگشت replaceLabel(dest, destLabelStart, destLabelLength,
                                    punycode, punycodeLength, errorCode);
            } وگرنه {
                // all-ASCII label
                اگر(labelLength>63) {
                    info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
                }
            }
        }
    } وگرنه {
        // If a Punycode label has severe errors,
        // then leave it but make sure it does not look valid.
        اگر(wasPunycode) {
            info.labelErrors|=UIDNA_ERROR_INVALID_ACE_LABEL;
            بازگشت markBadACELabel(dest, destLabelStart, destLabelLength, toASCII, info, errorCode);
        }
    }
    بازگشت replaceLabel(dest, destLabelStart, destLabelLength,
                        *labelString, labelLength, errorCode);
}

// Make sure an ACE label does not look valid.
// Append U+FFFD if the label has only LDH characters.
// If UIDNA_USE_STD3_RULES, also replace disallowed ASCII characters with U+FFFD.
int32_t
UTS46::markBadACELabel(UnicodeString &dest,
                       int32_t labelStart, int32_t labelLength,
                       UBool toASCII, IDNAInfo &info, UErrorCode &errorCode) ثابت {
    اگر(U_FAILURE(errorCode)) {
        بازگشت 0;
    }
    UBool disallowNonLDHDot=(options&UIDNA_USE_STD3_RULES)!=0;
    UBool isASCII=درست;
    UBool onlyLDH=درست;
    ثابت char16_t *label=dest.getBuffer()+labelStart;
    ثابت char16_t *limit=label+labelLength;
    // Start after the initial "xn--".
    // Ok to cast away const because we own the UnicodeString.
    برای(char16_t *s=const_cast<char16_t *>(label+4); s<limit; ++s) {
        char16_t c=*s;
        اگر(c<=0x7f) {
            اگر(c==0x2e) {
                info.labelErrors|=UIDNA_ERROR_LABEL_HAS_DOT;
                *s=0xfffd;
                isASCII=onlyLDH=نادرست;
            } وگرنه اگر(asciiData[c]<0) {
                onlyLDH=نادرست;
                اگر(disallowNonLDHDot) {
                    *s=0xfffd;
                    isASCII=نادرست;
                }
            }
        } وگرنه {
            isASCII=onlyLDH=نادرست;
        }
    }
    اگر(onlyLDH) {
        dest.insert(labelStart + labelLength, static_cast<char16_t>(0xfffd));
        اگر(dest.isBogus()) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            بازگشت 0;
        }
        ++labelLength;
    } وگرنه {
        اگر(toASCII && isASCII && labelLength>63) {
            info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
        }
    }
    بازگشت labelLength;
}

ثابت uint32_t L_MASK=U_MASK(U_چپ_TO_راست);
ثابت uint32_t R_AL_MASK=U_MASK(U_راست_TO_چپ)|U_MASK(U_راست_TO_چپ_ARABIC);
ثابت uint32_t L_R_AL_MASK=L_MASK|R_AL_MASK;

ثابت uint32_t R_AL_AN_MASK=R_AL_MASK|U_MASK(U_ARABIC_عدد);

ثابت uint32_t EN_AN_MASK=U_MASK(U_EUROPEAN_عدد)|U_MASK(U_ARABIC_عدد);
ثابت uint32_t R_AL_EN_AN_MASK=R_AL_MASK|EN_AN_MASK;
ثابت uint32_t L_EN_MASK=L_MASK|U_MASK(U_EUROPEAN_عدد);

ثابت uint32_t ES_CS_ET_ON_BN_NSM_MASK=
    U_MASK(U_EUROPEAN_عدد_SEPARATOR)|
    U_MASK(U_COMMON_عدد_SEPARATOR)|
    U_MASK(U_EUROPEAN_عدد_TERMINATOR)|
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
تهی
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
    اگر((firstMask&~L_R_AL_MASK)!=0) {
        info.isOkBiDi=نادرست;
    }
    // Get the directionality of the last non-NSM character.
    uint32_t lastMask;
    برای(;;) {
        اگر(i>=labelLength) {
            lastMask=firstMask;
            بشکن;
        }
        U16_PREV_UNSAFE(label, labelLength, c);
        UCharDirection dir=u_charDirection(c);
        اگر(dir!=U_DIR_NON_SPACING_MARK) {
            lastMask=U_MASK(dir);
            بشکن;
        }
    }
    // 3. In an RTL label, the end of the label must be a character with
    // BIDI property R, AL, EN or AN, followed by zero or more
    // characters with BIDI property NSM.
    // 6. In an LTR label, the end of the label must be a character with
    // BIDI property L or EN, followed by zero or more characters with
    // BIDI property NSM.
    اگر( (firstMask&L_MASK)!=0 ?
            (lastMask&~L_EN_MASK)!=0 :
            (lastMask&~R_AL_EN_AN_MASK)!=0
    ) {
        info.isOkBiDi=نادرست;
    }
    // Add the directionalities of the intervening characters.
    uint32_t mask=firstMask|lastMask;
    وقتی(i<labelLength) {
        U16_NEXT_UNSAFE(label, i, c);
        mask|=U_MASK(u_charDirection(c));
    }
    اگر(firstMask&L_MASK) {
        // 5. In an LTR label, only characters with the BIDI properties L, EN,
        // ES, CS, ET, ON, BN and NSM are allowed.
        اگر((mask&~L_EN_ES_CS_ET_ON_BN_NSM_MASK)!=0) {
            info.isOkBiDi=نادرست;
        }
    } وگرنه {
        // 2. In an RTL label, only characters with the BIDI properties R, AL,
        // AN, EN, ES, CS, ET, ON, BN and NSM are allowed.
        اگر((mask&~R_AL_AN_EN_ES_CS_ET_ON_BN_NSM_MASK)!=0) {
            info.isOkBiDi=نادرست;
        }
        // 4. In an RTL label, if an EN is present, no AN may be present, and
        // vice versa.
        اگر((mask&EN_AN_MASK)==EN_AN_MASK) {
            info.isOkBiDi=نادرست;
        }
    }
    // An RTL label is a label that contains at least one character of type
    // R, AL or AN. [...]
    // A "BIDI domain name" is a domain name that contains at least one RTL
    // label. [...]
    // The following rule, consisting of six conditions, applies to labels
    // in BIDI domain names.
    اگر((mask&R_AL_AN_MASK)!=0) {
        info.isBiDi=درست;
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
ایستا UBool
isASCIIOkBiDi(ثابت char16_t *s, int32_t length) {
    int32_t labelStart=0;
    برای(int32_t i=0; i<length; ++i) {
        char16_t c=s[i];
        اگر(c==0x2e) {  // dot
            اگر(i>labelStart) {
                c=s[i-1];
                اگر(!(0x61<=c && c<=0x7a) && !(0x30<=c && c<=0x39)) {
                    // Last character in the label is not an L or EN.
                    بازگشت نادرست;
                }
            }
            labelStart=i+1;
        } وگرنه اگر(i==labelStart) {
            اگر(!(0x61<=c && c<=0x7a)) {
                // First character in the label is not an L.
                بازگشت نادرست;
            }
        } وگرنه {
            اگر(c<=0x20 && (c>=0x1c || (9<=c && c<=0xd))) {
                // Intermediate character in the label is a B, S or WS.
                بازگشت نادرست;
            }
        }
    }
    بازگشت درست;
}

// UTF-8 version, called for source ASCII prefix.
// Can contain uppercase A-Z.
// s[length-1] must be the trailing dot.
ایستا UBool
isASCIIOkBiDi(ثابت نویسه *s, int32_t length) {
    int32_t labelStart=0;
    برای(int32_t i=0; i<length; ++i) {
        نویسه c=s[i];
        اگر(c==0x2e) {  // dot
            اگر(i>labelStart) {
                c=s[i-1];
                اگر(!(0x61<=c && c<=0x7a) && !(0x41<=c && c<=0x5a) && !(0x30<=c && c<=0x39)) {
                    // Last character in the label is not an L or EN.
                    بازگشت نادرست;
                }
            }
            labelStart=i+1;
        } وگرنه اگر(i==labelStart) {
            اگر(!(0x61<=c && c<=0x7a) && !(0x41<=c && c<=0x5a)) {
                // First character in the label is not an L.
                بازگشت نادرست;
            }
        } وگرنه {
            اگر(c<=0x20 && (c>=0x1c || (9<=c && c<=0xd))) {
                // Intermediate character in the label is a B, S or WS.
                بازگشت نادرست;
            }
        }
    }
    بازگشت درست;
}

UBool
UTS46::isLabelOkContextJ(ثابت char16_t *label, int32_t labelLength) ثابت {
    // [IDNA2008-Tables]
    // 200C..200D  ; CONTEXTJ    # ZERO WIDTH NON-JOINER..ZERO WIDTH JOINER
    برای(int32_t i=0; i<labelLength; ++i) {
        اگر(label[i]==0x200c) {
            // Appendix A.1. ZERO WIDTH NON-JOINER
            // Rule Set:
            //  False;
            //  If Canonical_Combining_Class(Before(cp)) .eq.  Virama Then True;
            //  If RegExpMatch((Joining_Type:{L,D})(Joining_Type:T)*\u200C
            //     (Joining_Type:T)*(Joining_Type:{R,D})) Then True;
            اگر(i==0) {
                بازگشت نادرست;
            }
            UChar32 c;
            int32_t j=i;
            U16_PREV_UNSAFE(label, j, c);
            اگر(uts46Norm2.getCombiningClass(c)==9) {
                ادامه;
            }
            // check precontext (Joining_Type:{L,D})(Joining_Type:T)*
            برای(;;) {
                UJoiningType type=ubidi_getJoiningType(c);
                اگر(type==U_JT_TRANSPARENT) {
                    اگر(j==0) {
                        بازگشت نادرست;
                    }
                    U16_PREV_UNSAFE(label, j, c);
                } وگرنه اگر(type==U_JT_چپ_JOINING || type==U_JT_DUAL_JOINING) {
                    بشکن;  // precontext fulfilled
                } وگرنه {
                    بازگشت نادرست;
                }
            }
            // check postcontext (Joining_Type:T)*(Joining_Type:{R,D})
            برای(j=i+1;;) {
                اگر(j==labelLength) {
                    بازگشت نادرست;
                }
                U16_NEXT_UNSAFE(label, j, c);
                UJoiningType type=ubidi_getJoiningType(c);
                اگر(type==U_JT_TRANSPARENT) {
                    // just skip this character
                } وگرنه اگر(type==U_JT_راست_JOINING || type==U_JT_DUAL_JOINING) {
                    بشکن;  // postcontext fulfilled
                } وگرنه {
                    بازگشت نادرست;
                }
            }
        } وگرنه اگر(label[i]==0x200d) {
            // Appendix A.2. ZERO WIDTH JOINER (U+200D)
            // Rule Set:
            //  False;
            //  If Canonical_Combining_Class(Before(cp)) .eq.  Virama Then True;
            اگر(i==0) {
                بازگشت نادرست;
            }
            UChar32 c;
            int32_t j=i;
            U16_PREV_UNSAFE(label, j, c);
            اگر(uts46Norm2.getCombiningClass(c)!=9) {
                بازگشت نادرست;
            }
        }
    }
    بازگشت درست;
}

تهی
UTS46::checkLabelContextO(ثابت char16_t *label, int32_t labelLength, IDNAInfo &info) ثابت {
    int32_t labelEnd=labelLength-1;  // inclusive
    int32_t arabicDigits=0;  // -1 for 066x, +1 for 06Fx
    برای(int32_t i=0; i<=labelEnd; ++i) {
        UChar32 c=label[i];
        اگر(c<0xb7) {
            // ASCII fastpath
        } وگرنه اگر(c<=0x6f9) {
            اگر(c==0xb7) {
                // Appendix A.3. MIDDLE DOT (U+00B7)
                // Rule Set:
                //  False;
                //  If Before(cp) .eq.  U+006C And
                //     After(cp) .eq.  U+006C Then True;
                اگر(!(0<i && label[i-1]==0x6c &&
                     i<labelEnd && label[i+1]==0x6c)) {
                    info.labelErrors|=UIDNA_ERROR_CONTEXTO_PUNCTUATION;
                }
            } وگرنه اگر(c==0x375) {
                // Appendix A.4. GREEK LOWER NUMERAL SIGN (KERAIA) (U+0375)
                // Rule Set:
                //  False;
                //  If Script(After(cp)) .eq.  Greek Then True;
                UScriptCode script=USCRIPT_INVALID_CODE;
                اگر(i<labelEnd) {
                    UErrorCode errorCode=U_ZERO_ERROR;
                    int32_t j=i+1;
                    U16_NEXT(label, j, labelLength, c);
                    script=uscript_getScript(c, &errorCode);
                }
                اگر(script!=USCRIPT_GREEK) {
                    info.labelErrors|=UIDNA_ERROR_CONTEXTO_PUNCTUATION;
                }
            } وگرنه اگر(c==0x5f3 || c==0x5f4) {
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
                اگر(0<i) {
                    UErrorCode errorCode=U_ZERO_ERROR;
                    int32_t j=i;
                    U16_PREV(label, 0, j, c);
                    script=uscript_getScript(c, &errorCode);
                }
                اگر(script!=USCRIPT_HEBREW) {
                    info.labelErrors|=UIDNA_ERROR_CONTEXTO_PUNCTUATION;
                }
            } وگرنه اگر(0x660<=c /* && c<=0x6f9 */) {
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
                اگر(c<=0x669) {
                    اگر(arabicDigits>0) {
                        info.labelErrors|=UIDNA_ERROR_CONTEXTO_DIGITS;
                    }
                    arabicDigits=-1;
                } وگرنه اگر(0x6f0<=c) {
                    اگر(arabicDigits<0) {
                        info.labelErrors|=UIDNA_ERROR_CONTEXTO_DIGITS;
                    }
                    arabicDigits=1;
                }
            }
        } وگرنه اگر(c==0x30fb) {
            // Appendix A.7. KATAKANA MIDDLE DOT (U+30FB)
            // Rule Set:
            //  False;
            //  For All Characters:
            //    If Script(cp) .in. {Hiragana, Katakana, Han} Then True;
            //  End For;
            UErrorCode errorCode=U_ZERO_ERROR;
            برای(صحیح j=0;;) {
                اگر(j>labelEnd) {
                    info.labelErrors|=UIDNA_ERROR_CONTEXTO_PUNCTUATION;
                    بشکن;
                }
                U16_NEXT(label, j, labelLength, c);
                UScriptCode script=uscript_getScript(c, &errorCode);
                اگر(script==USCRIPT_HIRAGANA || script==USCRIPT_KATAKANA || script==USCRIPT_HAN) {
                    بشکن;
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
    بازگشت reinterpret_cast<UIDNA *>(IDNA::createUTS46Instance(options, *pErrorCode));
}

U_CAPI تهی U_EXPORT2
uidna_close(UIDNA *idna) {
    delete reinterpret_cast<IDNA *>(idna);
}

ایستا UBool
checkArgs(ثابت تهی *label, int32_t length,
          تهی *dest, int32_t capacity,
          UIDNAInfo *pInfo, UErrorCode *pErrorCode) {
    اگر(U_FAILURE(*pErrorCode)) {
        بازگشت نادرست;
    }
    // sizeof(UIDNAInfo)=16 in the first API version.
    اگر(pInfo==nullptr || pInfo->size<16) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت نادرست;
    }
    اگر( (label==nullptr ? length!=0 : length<-1) ||
        (dest==nullptr ? capacity!=0 : capacity<0) ||
        (dest==label && label!=nullptr)
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت نادرست;
    }
    // Set all *pInfo bytes to 0 except for the size field itself.
    uprv_memset(&pInfo->size+1, 0, pInfo->size-اندازه(pInfo->size));
    بازگشت درست;
}

ایستا تهی
idnaInfoToStruct(IDNAInfo &info, UIDNAInfo *pInfo) {
    pInfo->isTransitionalDifferent=info.isTransitionalDifferent();
    pInfo->errors=info.getErrors();
}

U_CAPI int32_t U_EXPORT2
uidna_labelToASCII(ثابت UIDNA *idna,
                   ثابت char16_t *label, int32_t length,
                   char16_t *dest, int32_t capacity,
                   UIDNAInfo *pInfo, UErrorCode *pErrorCode) {
    اگر(!checkArgs(label, length, dest, capacity, pInfo, pErrorCode)) {
        بازگشت 0;
    }
    UnicodeString src(length < 0, label, length);
    UnicodeString destString(dest, 0, capacity);
    IDNAInfo info;
    reinterpret_cast<ثابت IDNA *>(idna)->labelToASCII(src, destString, info, *pErrorCode);
    idnaInfoToStruct(info, pInfo);
    بازگشت destString.extract(dest, capacity, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uidna_labelToUnicode(ثابت UIDNA *idna,
                     ثابت char16_t *label, int32_t length,
                     char16_t *dest, int32_t capacity,
                     UIDNAInfo *pInfo, UErrorCode *pErrorCode) {
    اگر(!checkArgs(label, length, dest, capacity, pInfo, pErrorCode)) {
        بازگشت 0;
    }
    UnicodeString src(length < 0, label, length);
    UnicodeString destString(dest, 0, capacity);
    IDNAInfo info;
    reinterpret_cast<ثابت IDNA *>(idna)->labelToUnicode(src, destString, info, *pErrorCode);
    idnaInfoToStruct(info, pInfo);
    بازگشت destString.extract(dest, capacity, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uidna_nameToASCII(ثابت UIDNA *idna,
                  ثابت char16_t *name, int32_t length,
                  char16_t *dest, int32_t capacity,
                  UIDNAInfo *pInfo, UErrorCode *pErrorCode) {
    اگر(!checkArgs(name, length, dest, capacity, pInfo, pErrorCode)) {
        بازگشت 0;
    }
    UnicodeString src(length < 0, name, length);
    UnicodeString destString(dest, 0, capacity);
    IDNAInfo info;
    reinterpret_cast<ثابت IDNA *>(idna)->nameToASCII(src, destString, info, *pErrorCode);
    idnaInfoToStruct(info, pInfo);
    بازگشت destString.extract(dest, capacity, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uidna_nameToUnicode(ثابت UIDNA *idna,
                    ثابت char16_t *name, int32_t length,
                    char16_t *dest, int32_t capacity,
                    UIDNAInfo *pInfo, UErrorCode *pErrorCode) {
    اگر(!checkArgs(name, length, dest, capacity, pInfo, pErrorCode)) {
        بازگشت 0;
    }
    UnicodeString src(length < 0, name, length);
    UnicodeString destString(dest, 0, capacity);
    IDNAInfo info;
    reinterpret_cast<ثابت IDNA *>(idna)->nameToUnicode(src, destString, info, *pErrorCode);
    idnaInfoToStruct(info, pInfo);
    بازگشت destString.extract(dest, capacity, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uidna_labelToASCII_UTF8(ثابت UIDNA *idna,
                        ثابت نویسه *label, int32_t length,
                        نویسه *dest, int32_t capacity,
                        UIDNAInfo *pInfo, UErrorCode *pErrorCode) {
    اگر(!checkArgs(label, length, dest, capacity, pInfo, pErrorCode)) {
        بازگشت 0;
    }
    StringPiece src(label, length<0 ? static_cast<int32_t>(uprv_strlen(label)) : length);
    بازگشت ByteSinkUtil::viaByteSinkToTerminatedChars(
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
                         ثابت نویسه *label, int32_t length,
                         نویسه *dest, int32_t capacity,
                         UIDNAInfo *pInfo, UErrorCode *pErrorCode) {
    اگر(!checkArgs(label, length, dest, capacity, pInfo, pErrorCode)) {
        بازگشت 0;
    }
    StringPiece src(label, length<0 ? static_cast<int32_t>(uprv_strlen(label)) : length);
    بازگشت ByteSinkUtil::viaByteSinkToTerminatedChars(
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
                       ثابت نویسه *name, int32_t length,
                       نویسه *dest, int32_t capacity,
                       UIDNAInfo *pInfo, UErrorCode *pErrorCode) {
    اگر(!checkArgs(name, length, dest, capacity, pInfo, pErrorCode)) {
        بازگشت 0;
    }
    StringPiece src(name, length<0 ? static_cast<int32_t>(uprv_strlen(name)) : length);
    بازگشت ByteSinkUtil::viaByteSinkToTerminatedChars(
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
                        ثابت نویسه *name, int32_t length,
                        نویسه *dest, int32_t capacity,
                        UIDNAInfo *pInfo, UErrorCode *pErrorCode) {
    اگر(!checkArgs(name, length, dest, capacity, pInfo, pErrorCode)) {
        بازگشت 0;
    }
    StringPiece src(name, length<0 ? static_cast<int32_t>(uprv_strlen(name)) : length);
    بازگشت ByteSinkUtil::viaByteSinkToTerminatedChars(
        dest, capacity,
        [&](ByteSink& sink, UErrorCode& status) {
            IDNAInfo info;
            reinterpret_cast<ثابت IDNA *>(idna)->nameToUnicodeUTF8(src, sink, info, status);
            idnaInfoToStruct(info, pInfo);
        },
        *pErrorCode);
}

#endif  // UCONFIG_NO_IDNA
