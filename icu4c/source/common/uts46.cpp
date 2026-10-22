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

#जर !UCONFIG_NO_IDNA

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
स्थिरांक uint32_t severeErrors=
    UIDNA_ERROR_LEADING_COMBINING_MARK|
    UIDNA_ERROR_DISALLOWED|
    UIDNA_ERROR_PUNYCODE|
    UIDNA_ERROR_LABEL_HAS_DOT|
    UIDNA_ERROR_INVALID_ACE_LABEL;

स्थिर ओळीत UBool
isASCIIString(स्थिरांक UnicodeString &dest) {
    स्थिरांक char16_t *s=dest.getBuffer();
    स्थिरांक char16_t *limit=s+dest.length();
    पर्यंत(s<limit) {
        जर(*s++>0x7f) {
            परत असत्य;
        }
    }
    परत सत्य;
}

स्थिर UBool
isASCIIOkBiDi(स्थिरांक char16_t *s, int32_t length);

स्थिर UBool
isASCIIOkBiDi(स्थिरांक अक्षर *s, int32_t length);

// IDNA class default implementations -------------------------------------- ***

IDNA::~IDNA() {}

शून्य
IDNA::labelToASCII_UTF8(StringPiece label, ByteSink &dest,
                        IDNAInfo &info, UErrorCode &errorCode) स्थिरांक {
    जर(U_SUCCESS(errorCode)) {
        UnicodeString destString;
        labelToASCII(UnicodeString::fromUTF8(label), destString,
                     info, errorCode).toUTF8(dest);
    }
}

शून्य
IDNA::labelToUnicodeUTF8(StringPiece label, ByteSink &dest,
                         IDNAInfo &info, UErrorCode &errorCode) स्थिरांक {
    जर(U_SUCCESS(errorCode)) {
        UnicodeString destString;
        labelToUnicode(UnicodeString::fromUTF8(label), destString,
                       info, errorCode).toUTF8(dest);
    }
}

शून्य
IDNA::nameToASCII_UTF8(StringPiece name, ByteSink &dest,
                       IDNAInfo &info, UErrorCode &errorCode) स्थिरांक {
    जर(U_SUCCESS(errorCode)) {
        UnicodeString destString;
        nameToASCII(UnicodeString::fromUTF8(name), destString,
                    info, errorCode).toUTF8(dest);
    }
}

शून्य
IDNA::nameToUnicodeUTF8(StringPiece name, ByteSink &dest,
                        IDNAInfo &info, UErrorCode &errorCode) स्थिरांक {
    जर(U_SUCCESS(errorCode)) {
        UnicodeString destString;
        nameToUnicode(UnicodeString::fromUTF8(name), destString,
                      info, errorCode).toUTF8(dest);
    }
}

// UTS46 class declaration ------------------------------------------------- ***

वर्ग UTS46 : सार्वजनिक IDNA {
सार्वजनिक:
    UTS46(uint32_t options, UErrorCode &errorCode);
    आभासी ~UTS46();

    आभासी UnicodeString&
    labelToASCII(स्थिरांक UnicodeString& label,
                 UnicodeString& dest U_LIFETIME_BOUND,
                 IDNAInfo& info,
                 UErrorCode& errorCode) स्थिरांक override;

    आभासी UnicodeString&
    labelToUnicode(स्थिरांक UnicodeString& label,
                   UnicodeString& dest U_LIFETIME_BOUND,
                   IDNAInfo& info,
                   UErrorCode& errorCode) स्थिरांक override;

    आभासी UnicodeString&
    nameToASCII(स्थिरांक UnicodeString& name,
                UnicodeString& dest U_LIFETIME_BOUND,
                IDNAInfo& info,
                UErrorCode& errorCode) स्थिरांक override;

    आभासी UnicodeString&
    nameToUnicode(स्थिरांक UnicodeString& name,
                  UnicodeString& dest U_LIFETIME_BOUND,
                  IDNAInfo& info,
                  UErrorCode& errorCode) स्थिरांक override;

    आभासी शून्य
    labelToASCII_UTF8(StringPiece label, ByteSink &dest,
                      IDNAInfo &info, UErrorCode &errorCode) स्थिरांक override;

    आभासी शून्य
    labelToUnicodeUTF8(StringPiece label, ByteSink &dest,
                       IDNAInfo &info, UErrorCode &errorCode) स्थिरांक override;

    आभासी शून्य
    nameToASCII_UTF8(StringPiece name, ByteSink &dest,
                     IDNAInfo &info, UErrorCode &errorCode) स्थिरांक override;

    आभासी शून्य
    nameToUnicodeUTF8(StringPiece name, ByteSink &dest,
                      IDNAInfo &info, UErrorCode &errorCode) स्थिरांक override;

खाजगी:
    UnicodeString&
    process(स्थिरांक UnicodeString& src,
            UBool isLabel,
            UBool toASCII,
            UnicodeString& dest U_LIFETIME_BOUND,
            IDNAInfo& info,
            UErrorCode& errorCode) स्थिरांक;

    शून्य
    processUTF8(StringPiece src,
                UBool isLabel, UBool toASCII,
                ByteSink &dest,
                IDNAInfo &info, UErrorCode &errorCode) स्थिरांक;

    UnicodeString&
    processUnicode(स्थिरांक UnicodeString& src,
                   int32_t labelStart,
                   int32_t mappingStart,
                   UBool isLabel,
                   UBool toASCII,
                   UnicodeString& dest U_LIFETIME_BOUND,
                   IDNAInfo& info,
                   UErrorCode& errorCode) स्थिरांक;

    // returns the new dest.length()
    int32_t
    mapDevChars(UnicodeString &dest, int32_t labelStart, int32_t mappingStart,
                UErrorCode &errorCode) स्थिरांक;

    // returns the new label length
    int32_t
    processLabel(UnicodeString &dest,
                 int32_t labelStart, int32_t labelLength,
                 UBool toASCII,
                 IDNAInfo &info, UErrorCode &errorCode) स्थिरांक;
    int32_t
    markBadACELabel(UnicodeString &dest,
                    int32_t labelStart, int32_t labelLength,
                    UBool toASCII, IDNAInfo &info, UErrorCode &errorCode) स्थिरांक;

    शून्य
    checkLabelBiDi(स्थिरांक char16_t *label, int32_t labelLength, IDNAInfo &info) स्थिरांक;

    UBool
    isLabelOkContextJ(स्थिरांक char16_t *label, int32_t labelLength) स्थिरांक;

    शून्य
    checkLabelContextO(स्थिरांक char16_t *label, int32_t labelLength, IDNAInfo &info) स्थिरांक;

    स्थिरांक Normalizer2 &uts46Norm2;  // uts46.nrm
    uint32_t options;
};

IDNA *
IDNA::createUTS46Instance(uint32_t options, UErrorCode &errorCode) {
    जर(U_SUCCESS(errorCode)) {
        IDNA *idna=नवीन UTS46(options, errorCode);
        जर(idna==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
        } अन्यथा जर(U_FAILURE(errorCode)) {
            delete idna;
            idna=nullptr;
        }
        परत idna;
    } अन्यथा {
        परत nullptr;
    }
}

// UTS46 implementation ---------------------------------------------------- ***

UTS46::UTS46(uint32_t opt, UErrorCode &errorCode)
        : uts46Norm2(*Normalizer2::getInstance(nullptr, "uts46", UNORM2_COMPOSE, errorCode)),
          options(opt) {}

UTS46::~UTS46() {}

UnicodeString&
UTS46::labelToASCII(स्थिरांक UnicodeString& label,
                    UnicodeString& dest U_LIFETIME_BOUND,
                    IDNAInfo& info,
                    UErrorCode& errorCode) स्थिरांक {
    परत process(label, सत्य, सत्य, dest, info, errorCode);
}

UnicodeString&
UTS46::labelToUnicode(स्थिरांक UnicodeString& label,
                      UnicodeString& dest U_LIFETIME_BOUND,
                      IDNAInfo& info,
                      UErrorCode& errorCode) स्थिरांक {
    परत process(label, सत्य, असत्य, dest, info, errorCode);
}

UnicodeString&
UTS46::nameToASCII(स्थिरांक UnicodeString& name,
                   UnicodeString& dest U_LIFETIME_BOUND,
                   IDNAInfo& info,
                   UErrorCode& errorCode) स्थिरांक {
    process(name, असत्य, सत्य, dest, info, errorCode);
    जर( dest.length()>=254 && (info.errors&UIDNA_ERROR_DOMAIN_NAME_TOO_LONG)==0 &&
        isASCIIString(dest) &&
        (dest.length()>254 || dest[253]!=0x2e)
    ) {
        info.errors|=UIDNA_ERROR_DOMAIN_NAME_TOO_LONG;
    }
    परत dest;
}

UnicodeString&
UTS46::nameToUnicode(स्थिरांक UnicodeString& name,
                     UnicodeString& dest U_LIFETIME_BOUND,
                     IDNAInfo& info,
                     UErrorCode& errorCode) स्थिरांक {
    परत process(name, असत्य, असत्य, dest, info, errorCode);
}

शून्य
UTS46::labelToASCII_UTF8(StringPiece label, ByteSink &dest,
                         IDNAInfo &info, UErrorCode &errorCode) स्थिरांक {
    processUTF8(label, सत्य, सत्य, dest, info, errorCode);
}

शून्य
UTS46::labelToUnicodeUTF8(StringPiece label, ByteSink &dest,
                          IDNAInfo &info, UErrorCode &errorCode) स्थिरांक {
    processUTF8(label, सत्य, असत्य, dest, info, errorCode);
}

शून्य
UTS46::nameToASCII_UTF8(StringPiece name, ByteSink &dest,
                        IDNAInfo &info, UErrorCode &errorCode) स्थिरांक {
    processUTF8(name, असत्य, सत्य, dest, info, errorCode);
}

शून्य
UTS46::nameToUnicodeUTF8(StringPiece name, ByteSink &dest,
                         IDNAInfo &info, UErrorCode &errorCode) स्थिरांक {
    processUTF8(name, असत्य, असत्य, dest, info, errorCode);
}

// UTS #46 data for ASCII characters.
// The normalizer (using uts46.nrm) maps uppercase ASCII letters to lowercase
// and passes through all other ASCII characters.
// If UIDNA_USE_STD3_RULES is set, then non-LDH characters are disallowed
// using this data.
// The ASCII fastpath also uses this data.
// Values: -1=disallowed  0==valid  1==mapped (lowercase)
स्थिर स्थिरांक int8_t asciiData[128]={
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
UTS46::process(स्थिरांक UnicodeString& src,
               UBool isLabel,
               UBool toASCII,
               UnicodeString& dest U_LIFETIME_BOUND,
               IDNAInfo& info,
               UErrorCode& errorCode) स्थिरांक {
    // uts46Norm2.normalize() would do all of this error checking and setup,
    // but with the ASCII fastpath we do not always call it, and do not
    // call it first.
    जर(U_FAILURE(errorCode)) {
        dest.setToBogus();
        परत dest;
    }
    स्थिरांक char16_t *srcArray=src.getBuffer();
    जर(&dest==&src || srcArray==nullptr) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        dest.setToBogus();
        परत dest;
    }
    // Arguments are fine, reset output values.
    dest.हटाओ();
    info.reset();
    int32_t srcLength=src.length();
    जर(srcLength==0) {
        info.errors|=UIDNA_ERROR_EMPTY_LABEL;
        परत dest;
    }
    char16_t *destArray=dest.getBuffer(srcLength);
    जर(destArray==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        परत dest;
    }
    // ASCII fastpath
    UBool disallowNonLDHDot=(options&UIDNA_USE_STD3_RULES)!=0;
    int32_t labelStart=0;
    int32_t i;
    साठी(i=0;; ++i) {
        जर(i==srcLength) {
            जर(toASCII) {
                जर((i-labelStart)>63) {
                    info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
                }
                // There is a trailing dot if labelStart==i.
                जर(!isLabel && i>=254 && (i>254 || labelStart<i)) {
                    info.errors|=UIDNA_ERROR_DOMAIN_NAME_TOO_LONG;
                }
            }
            info.errors|=info.labelErrors;
            dest.releaseBuffer(i);
            परत dest;
        }
        char16_t c=srcArray[i];
        जर(c>0x7f) {
            थांबा;
        }
        पूर्णांक cData=asciiData[c];
        जर(cData>0) {
            destArray[i]=c+0x20;  // Lowercase an uppercase ASCII letter.
        } अन्यथा जर(cData<0 && disallowNonLDHDot) {
            थांबा;  // Replacing with U+FFFD can be complicated for toASCII.
        } अन्यथा {
            destArray[i]=c;
            जर(c==0x2d) {  // hyphen
                जर(i==(labelStart+3) && srcArray[i-1]==0x2d) {
                    // "??--..." is Punycode or forbidden.
                    ++i;  // '-' was copied to dest already
                    थांबा;
                }
                जर(i==labelStart) {
                    // label starts with "-"
                    info.labelErrors|=UIDNA_ERROR_LEADING_HYPHEN;
                }
                जर((i+1)==srcLength || srcArray[i+1]==0x2e) {
                    // label ends with "-"
                    info.labelErrors|=UIDNA_ERROR_TRAILING_HYPHEN;
                }
            } अन्यथा जर(c==0x2e) {  // dot
                जर(isLabel) {
                    // Replacing with U+FFFD can be complicated for toASCII.
                    ++i;  // '.' was copied to dest already
                    थांबा;
                }
                जर(i==labelStart) {
                    info.labelErrors|=UIDNA_ERROR_EMPTY_LABEL;
                }
                जर(toASCII && (i-labelStart)>63) {
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
    जर( info.isBiDi && U_SUCCESS(errorCode) && (info.errors&severeErrors)==0 &&
        (!info.isOkBiDi || (labelStart>0 && !isASCIIOkBiDi(dest.getBuffer(), labelStart)))
    ) {
        info.errors|=UIDNA_ERROR_BIDI;
    }
    परत dest;
}

शून्य
UTS46::processUTF8(StringPiece src,
                   UBool isLabel, UBool toASCII,
                   ByteSink &dest,
                   IDNAInfo &info, UErrorCode &errorCode) स्थिरांक {
    जर(U_FAILURE(errorCode)) {
        परत;
    }
    स्थिरांक अक्षर *srcArray=src.data();
    int32_t srcLength=src.length();
    जर(srcArray==nullptr && srcLength!=0) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        परत;
    }
    // Arguments are fine, reset output values.
    info.reset();
    जर(srcLength==0) {
        info.errors|=UIDNA_ERROR_EMPTY_LABEL;
        dest.Flush();
        परत;
    }
    UnicodeString destString;
    int32_t labelStart=0;
    जर(srcLength<=256) {  // length of stackArray[]
        // ASCII fastpath
        अक्षर चळत_Array[256];
        int32_t destCapacity;
        अक्षर *destArray=dest.GetAppendBuffer(srcLength, srcLength+20,
                                             चळत_Array, UPRV_LENGTHOF(चळत_Array), &destCapacity);
        UBool disallowNonLDHDot=(options&UIDNA_USE_STD3_RULES)!=0;
        int32_t i;
        साठी(i=0;; ++i) {
            जर(i==srcLength) {
                जर(toASCII) {
                    जर((i-labelStart)>63) {
                        info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
                    }
                    // There is a trailing dot if labelStart==i.
                    जर(!isLabel && i>=254 && (i>254 || labelStart<i)) {
                        info.errors|=UIDNA_ERROR_DOMAIN_NAME_TOO_LONG;
                    }
                }
                info.errors|=info.labelErrors;
                dest.Append(destArray, i);
                dest.Flush();
                परत;
            }
            अक्षर c=srcArray[i];
            जर (static_cast<int8_t>(c) < 0) { // (uint8_t)c>0x7f
                थांबा;
            }
            पूर्णांक cData = asciiData[static_cast<पूर्णांक>(c)]; // Cast: gcc warns about indexing with a char.
            जर(cData>0) {
                destArray[i]=c+0x20;  // Lowercase an uppercase ASCII letter.
            } अन्यथा जर(cData<0 && disallowNonLDHDot) {
                थांबा;  // Replacing with U+FFFD can be complicated for toASCII.
            } अन्यथा {
                destArray[i]=c;
                जर(c==0x2d) {  // hyphen
                    जर(i==(labelStart+3) && srcArray[i-1]==0x2d) {
                        // "??--..." is Punycode or forbidden.
                        थांबा;
                    }
                    जर(i==labelStart) {
                        // label starts with "-"
                        info.labelErrors|=UIDNA_ERROR_LEADING_HYPHEN;
                    }
                    जर((i+1)==srcLength || srcArray[i+1]==0x2e) {
                        // label ends with "-"
                        info.labelErrors|=UIDNA_ERROR_TRAILING_HYPHEN;
                    }
                } अन्यथा जर(c==0x2e) {  // dot
                    जर(isLabel) {
                        थांबा;  // Replacing with U+FFFD can be complicated for toASCII.
                    }
                    जर(i==labelStart) {
                        info.labelErrors|=UIDNA_ERROR_EMPTY_LABEL;
                    }
                    जर(toASCII && (i-labelStart)>63) {
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
    } अन्यथा {
        // src is too long for the ASCII fastpath implementation.
        processUnicode(UnicodeString::fromUTF8(src), 0, 0,
                       isLabel, toASCII,
                       destString, info, errorCode);
    }
    destString.toUTF8(dest);  // calls dest.Flush()
    जर(toASCII && !isLabel) {
        // length==labelStart==254 means that there is a trailing dot (ok) and
        // destString is empty (do not index at 253-labelStart).
        int32_t length=labelStart+destString.length();
        जर( length>=254 && isASCIIString(destString) &&
            (length>254 ||
             (labelStart<254 && destString[253-labelStart]!=0x2e))
        ) {
            info.errors|=UIDNA_ERROR_DOMAIN_NAME_TOO_LONG;
        }
    }
    जर( info.isBiDi && U_SUCCESS(errorCode) && (info.errors&severeErrors)==0 &&
        (!info.isOkBiDi || (labelStart>0 && !isASCIIOkBiDi(srcArray, labelStart)))
    ) {
        info.errors|=UIDNA_ERROR_BIDI;
    }
}

UnicodeString&
UTS46::processUnicode(स्थिरांक UnicodeString& src,
                      int32_t labelStart,
                      int32_t mappingStart,
                      UBool isLabel,
                      UBool toASCII,
                      UnicodeString& dest U_LIFETIME_BOUND,
                      IDNAInfo& info,
                      UErrorCode& errorCode) स्थिरांक {
    जर(mappingStart==0) {
        uts46Norm2.normalize(src, dest, errorCode);
    } अन्यथा {
        uts46Norm2.normalizeSecondAndAppend(dest, src.tempSubString(mappingStart), errorCode);
    }
    जर(U_FAILURE(errorCode)) {
        परत dest;
    }
    UBool doMapDevChars=
        toASCII ? (options&UIDNA_NONTRANSITIONAL_TO_ASCII)==0 :
                  (options&UIDNA_NONTRANSITIONAL_TO_UNICODE)==0;
    स्थिरांक char16_t *destArray=dest.getBuffer();
    int32_t destLength=dest.length();
    int32_t labelLimit=labelStart;
    पर्यंत(labelLimit<destLength) {
        char16_t c=destArray[labelLimit];
        जर(c==0x2e && !isLabel) {
            int32_t labelLength=labelLimit-labelStart;
            int32_t newLength=processLabel(dest, labelStart, labelLength,
                                            toASCII, info, errorCode);
            info.errors|=info.labelErrors;
            info.labelErrors=0;
            जर(U_FAILURE(errorCode)) {
                परत dest;
            }
            destArray=dest.getBuffer();
            destLength+=newLength-labelLength;
            labelLimit=labelStart+=newLength+1;
            सुरूठेवा;
        } अन्यथा जर(c<0xdf) {
            // pass
        } अन्यथा जर(c<=0x200d && (c==0xdf || c==0x3c2 || c>=0x200c)) {
            info.isTransDiff=सत्य;
            जर(doMapDevChars) {
                destLength=mapDevChars(dest, labelStart, labelLimit, errorCode);
                जर(U_FAILURE(errorCode)) {
                    परत dest;
                }
                destArray=dest.getBuffer();
                // All deviation characters have been mapped, no need to check for them again.
                doMapDevChars=असत्य;
                // Do not increment labelLimit in case c was removed.
                सुरूठेवा;
            }
        } अन्यथा जर(U16_IS_SURROGATE(c)) {
            जर(U16_IS_SURROGATE_LEAD(c) ?
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
    जर(0==labelStart || labelStart<labelLimit) {
        processLabel(dest, labelStart, labelLimit-labelStart,
                      toASCII, info, errorCode);
        info.errors|=info.labelErrors;
    }
    परत dest;
}

int32_t
UTS46::mapDevChars(UnicodeString &dest, int32_t labelStart, int32_t mappingStart,
                   UErrorCode &errorCode) स्थिरांक {
    जर(U_FAILURE(errorCode)) {
        परत 0;
    }
    int32_t length=dest.length();
    char16_t *s=dest.getBuffer(dest[mappingStart]==0xdf ? length+1 : length);
    जर(s==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        परत length;
    }
    int32_t capacity=dest.getCapacity();
    UBool didMapDevChars=असत्य;
    int32_t readIndex=mappingStart, writeIndex=mappingStart;
    करा {
        char16_t c=s[readIndex++];
        निवड(c) {
        स्थिती 0xdf:
            // Map sharp s to ss.
            didMapDevChars=सत्य;
            s[writeIndex++]=0x73;  // Replace sharp s with first s.
            // Insert second s and account for possible buffer reallocation.
            जर(writeIndex==readIndex) {
                जर(length==capacity) {
                    dest.releaseBuffer(length);
                    s=dest.getBuffer(length+1);
                    जर(s==nullptr) {
                        errorCode=U_MEMORY_ALLOCATION_ERROR;
                        परत length;
                    }
                    capacity=dest.getCapacity();
                }
                u_memmove(s+writeIndex+1, s+writeIndex, length-writeIndex);
                ++readIndex;
            }
            s[writeIndex++]=0x73;
            ++length;
            थांबा;
        स्थिती 0x3c2:  // Map final sigma to nonfinal sigma.
            didMapDevChars=सत्य;
            s[writeIndex++]=0x3c3;
            थांबा;
        स्थिती 0x200c:  // Ignore/remove ZWNJ.
        स्थिती 0x200d:  // Ignore/remove ZWJ.
            didMapDevChars=सत्य;
            --length;
            थांबा;
        पूर्वनिर्धारित:
            // Only really necessary if writeIndex was different from readIndex.
            s[writeIndex++]=c;
            थांबा;
        }
    } पर्यंत(writeIndex<length);
    dest.releaseBuffer(length);
    जर(didMapDevChars) {
        // Mapping deviation characters might have resulted in an un-NFC string.
        // We could use either the NFC or the UTS #46 normalizer.
        // By using the UTS #46 normalizer again, we avoid having to load a second .nrm data file.
        UnicodeString normalized;
        uts46Norm2.normalize(dest.tempSubString(labelStart), normalized, errorCode);
        जर(U_SUCCESS(errorCode)) {
            dest.replace(labelStart, 0x7fffffff, normalized);
            जर(dest.isBogus()) {
                errorCode=U_MEMORY_ALLOCATION_ERROR;
            }
            परत dest.length();
        }
    }
    परत length;
}

// Replace the label in dest with the label string, if the label was modified.
// If &label==&dest then the label was modified in-place and labelLength
// is the new label length, different from label.length().
// If &label!=&dest then labelLength==label.length().
// Returns labelLength (= the new label length).
स्थिर int32_t
replaceLabel(UnicodeString &dest, int32_t destLabelStart, int32_t destLabelLength,
             स्थिरांक UnicodeString &label, int32_t labelLength, UErrorCode &errorCode) {
    जर(U_FAILURE(errorCode)) {
        परत 0;
    }
    जर(&label!=&dest) {
        dest.replace(destLabelStart, destLabelLength, label);
        जर(dest.isBogus()) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            परत 0;
        }
    }
    परत labelLength;
}

int32_t
UTS46::processLabel(UnicodeString &dest,
                    int32_t labelStart, int32_t labelLength,
                    UBool toASCII,
                    IDNAInfo &info, UErrorCode &errorCode) स्थिरांक {
    जर(U_FAILURE(errorCode)) {
        परत 0;
    }
    UnicodeString fromPunycode;
    UnicodeString *labelString;
    स्थिरांक char16_t *label=dest.getBuffer()+labelStart;
    int32_t destLabelStart=labelStart;
    int32_t destLabelLength=labelLength;
    UBool wasPunycode;
    जर(labelLength>=4 && label[0]==0x78 && label[1]==0x6e && label[2]==0x2d && label[3]==0x2d) {
        // Label starts with "xn--", try to un-Punycode it.
        // In IDNA2008, labels like "xn--" (decodes to an empty string) and
        // "xn--ASCII-" (decodes to just "ASCII") fail the round-trip validation from
        // comparing the ToUnicode input with the back-to-ToASCII output.
        // They are alternate encodings of the respective ASCII labels.
        // Ignore "xn---" here: It will fail Punycode.decode() which logically comes before
        // the round-trip verification.
        जर(labelLength==4 || (labelLength>5 && label[labelLength-1]==u'-')) {
            info.labelErrors|=UIDNA_ERROR_INVALID_ACE_LABEL;
            परत markBadACELabel(dest, labelStart, labelLength, toASCII, info, errorCode);
        }
        wasPunycode=सत्य;
        char16_t *unicodeBuffer=fromPunycode.getBuffer(-1);  // capacity==-1: most labels should fit
        जर(unicodeBuffer==nullptr) {
            // Should never occur if we used capacity==-1 which uses the internal buffer.
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            परत labelLength;
        }
        UErrorCode punycodeErrorCode=U_ZERO_ERROR;
        int32_t unicodeLength=u_strFromPunycode(label+4, labelLength-4,
                                                unicodeBuffer, fromPunycode.getCapacity(),
                                                nullptr, &punycodeErrorCode);
        जर(punycodeErrorCode==U_BUFFER_OVERFLOW_ERROR) {
            fromPunycode.releaseBuffer(0);
            unicodeBuffer=fromPunycode.getBuffer(unicodeLength);
            जर(unicodeBuffer==nullptr) {
                errorCode=U_MEMORY_ALLOCATION_ERROR;
                परत labelLength;
            }
            punycodeErrorCode=U_ZERO_ERROR;
            unicodeLength=u_strFromPunycode(label+4, labelLength-4,
                                            unicodeBuffer, fromPunycode.getCapacity(),
                                            nullptr, &punycodeErrorCode);
        }
        fromPunycode.releaseBuffer(unicodeLength);
        जर(U_FAILURE(punycodeErrorCode)) {
            info.labelErrors|=UIDNA_ERROR_PUNYCODE;
            परत markBadACELabel(dest, labelStart, labelLength, toASCII, info, errorCode);
        }
        // Check for NFC, and for characters that are not
        // valid or deviation characters according to the normalizer.
        // If there is something wrong, then the string will change.
        // Note that the normalizer passes through non-LDH ASCII and deviation characters.
        // Deviation characters are ok in Punycode even in transitional processing.
        // In the code further below, if we find non-LDH ASCII and we have UIDNA_USE_STD3_RULES
        // then we will set UIDNA_ERROR_INVALID_ACE_LABEL there too.
        UBool isValid=uts46Norm2.isNormalized(fromPunycode, errorCode);
        जर(U_FAILURE(errorCode)) {
            परत labelLength;
        }
        // Unicode 15.1 UTS #46:
        // Added an additional condition in 4.1 Validity Criteria to
        // disallow labels such as xn--xn---epa., which do not round-trip.
        // --> Validity Criteria new criterion 4:
        // If not CheckHyphens, the label must not begin with “xn--”.
        जर(!isValid || fromPunycode.startsWith(UnicodeString::readOnlyAlias(u"xn--"))) {
            info.labelErrors|=UIDNA_ERROR_INVALID_ACE_LABEL;
            परत markBadACELabel(dest, labelStart, labelLength, toASCII, info, errorCode);
        }
        labelString=&fromPunycode;
        label=fromPunycode.getBuffer();
        labelStart=0;
        labelLength=fromPunycode.length();
    } अन्यथा {
        wasPunycode=असत्य;
        labelString=&dest;
    }
    // Validity check
    जर(labelLength==0) {
        info.labelErrors|=UIDNA_ERROR_EMPTY_LABEL;
        परत replaceLabel(dest, destLabelStart, destLabelLength,
                            *labelString, labelLength, errorCode);
    }
    // labelLength>0
    जर(labelLength>=4 && label[2]==0x2d && label[3]==0x2d) {
        // label starts with "??--"
        info.labelErrors|=UIDNA_ERROR_HYPHEN_3_4;
    }
    जर(label[0]==0x2d) {
        // label starts with "-"
        info.labelErrors|=UIDNA_ERROR_LEADING_HYPHEN;
    }
    जर(label[labelLength-1]==0x2d) {
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
    स्थिरांक char16_t *limit=label+labelLength;
    char16_t oredChars=0;
    // If we enforce STD3 rules, then ASCII characters other than LDH and dot are disallowed.
    UBool disallowNonLDHDot=(options&UIDNA_USE_STD3_RULES)!=0;
    करा {
        char16_t c=*s;
        जर(c<=0x7f) {
            जर(c==0x2e) {
                info.labelErrors|=UIDNA_ERROR_LABEL_HAS_DOT;
                *s=0xfffd;
            } अन्यथा जर(disallowNonLDHDot && asciiData[c]<0) {
                info.labelErrors|=UIDNA_ERROR_DISALLOWED;
                *s=0xfffd;
            }
        } अन्यथा {
            oredChars|=c;
            जर(c==0xfffd) {
                info.labelErrors|=UIDNA_ERROR_DISALLOWED;
            }
        }
        ++s;
    } पर्यंत(s<limit);
    // Check for a leading combining mark after other validity checks
    // so that we don't report UIDNA_ERROR_DISALLOWED for the U+FFFD from here.
    UChar32 c;
    int32_t cpLength=0;
    // "Unsafe" is ok because unpaired surrogates were mapped to U+FFFD.
    U16_NEXT_UNSAFE(label, cpLength, c);
    जर((U_GET_GC_MASK(c)&U_GC_M_MASK)!=0) {
        info.labelErrors|=UIDNA_ERROR_LEADING_COMBINING_MARK;
        labelString->replace(labelStart, cpLength, static_cast<char16_t>(0xfffd));
        label=labelString->getBuffer()+labelStart;
        labelLength+=1-cpLength;
        जर(labelString==&dest) {
            destLabelLength=labelLength;
        }
    }
    जर((info.labelErrors&severeErrors)==0) {
        // Do contextual checks only if we do not have U+FFFD from a severe error
        // because U+FFFD can make these checks fail.
        जर((options&UIDNA_CHECK_BIDI)!=0 && (!info.isBiDi || info.isOkBiDi)) {
            checkLabelBiDi(label, labelLength, info);
        }
        जर( (options&UIDNA_CHECK_CONTEXTJ)!=0 && (oredChars&0x200c)==0x200c &&
            !isLabelOkContextJ(label, labelLength)
        ) {
            info.labelErrors|=UIDNA_ERROR_CONTEXTJ;
        }
        जर((options&UIDNA_CHECK_CONTEXTO)!=0 && oredChars>=0xb7) {
            checkLabelContextO(label, labelLength, info);
        }
        जर(toASCII) {
            जर(wasPunycode) {
                // Leave a Punycode label unchanged if it has no severe errors.
                जर(destLabelLength>63) {
                    info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
                }
                परत destLabelLength;
            } अन्यथा जर(oredChars>=0x80) {
                // Contains non-ASCII characters.
                UnicodeString punycode;
                char16_t *buffer=punycode.getBuffer(63);  // 63==maximum DNS label length
                जर(buffer==nullptr) {
                    errorCode=U_MEMORY_ALLOCATION_ERROR;
                    परत destLabelLength;
                }
                buffer[0]=0x78;  // Write "xn--".
                buffer[1]=0x6e;
                buffer[2]=0x2d;
                buffer[3]=0x2d;
                UErrorCode punycodeErrorCode=U_ZERO_ERROR;
                int32_t punycodeLength=u_strToPunycode(label, labelLength,
                                                      buffer+4, punycode.getCapacity()-4,
                                                      nullptr, &punycodeErrorCode);
                जर(punycodeErrorCode==U_BUFFER_OVERFLOW_ERROR) {
                    punycodeErrorCode=U_ZERO_ERROR;
                    punycode.releaseBuffer(4);
                    buffer=punycode.getBuffer(4+punycodeLength);
                    जर(buffer==nullptr) {
                        errorCode=U_MEMORY_ALLOCATION_ERROR;
                        परत destLabelLength;
                    }
                    punycodeLength=u_strToPunycode(label, labelLength,
                                                  buffer+4, punycode.getCapacity()-4,
                                                  nullptr, &punycodeErrorCode);
                }
                punycodeLength+=4;
                punycode.releaseBuffer(punycodeLength);
                जर(U_FAILURE(punycodeErrorCode)) {
                    errorCode = punycodeErrorCode;
                    परत destLabelLength;
                }
                जर(punycodeLength>63) {
                    info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
                }
                परत replaceLabel(dest, destLabelStart, destLabelLength,
                                    punycode, punycodeLength, errorCode);
            } अन्यथा {
                // all-ASCII label
                जर(labelLength>63) {
                    info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
                }
            }
        }
    } अन्यथा {
        // If a Punycode label has severe errors,
        // then leave it but make sure it does not look valid.
        जर(wasPunycode) {
            info.labelErrors|=UIDNA_ERROR_INVALID_ACE_LABEL;
            परत markBadACELabel(dest, destLabelStart, destLabelLength, toASCII, info, errorCode);
        }
    }
    परत replaceLabel(dest, destLabelStart, destLabelLength,
                        *labelString, labelLength, errorCode);
}

// Make sure an ACE label does not look valid.
// Append U+FFFD if the label has only LDH characters.
// If UIDNA_USE_STD3_RULES, also replace disallowed ASCII characters with U+FFFD.
int32_t
UTS46::markBadACELabel(UnicodeString &dest,
                       int32_t labelStart, int32_t labelLength,
                       UBool toASCII, IDNAInfo &info, UErrorCode &errorCode) स्थिरांक {
    जर(U_FAILURE(errorCode)) {
        परत 0;
    }
    UBool disallowNonLDHDot=(options&UIDNA_USE_STD3_RULES)!=0;
    UBool isASCII=सत्य;
    UBool onlyLDH=सत्य;
    स्थिरांक char16_t *label=dest.getBuffer()+labelStart;
    स्थिरांक char16_t *limit=label+labelLength;
    // Start after the initial "xn--".
    // Ok to cast away const because we own the UnicodeString.
    साठी(char16_t *s=const_cast<char16_t *>(label+4); s<limit; ++s) {
        char16_t c=*s;
        जर(c<=0x7f) {
            जर(c==0x2e) {
                info.labelErrors|=UIDNA_ERROR_LABEL_HAS_DOT;
                *s=0xfffd;
                isASCII=onlyLDH=असत्य;
            } अन्यथा जर(asciiData[c]<0) {
                onlyLDH=असत्य;
                जर(disallowNonLDHDot) {
                    *s=0xfffd;
                    isASCII=असत्य;
                }
            }
        } अन्यथा {
            isASCII=onlyLDH=असत्य;
        }
    }
    जर(onlyLDH) {
        dest.insert(labelStart + labelLength, static_cast<char16_t>(0xfffd));
        जर(dest.isBogus()) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            परत 0;
        }
        ++labelLength;
    } अन्यथा {
        जर(toASCII && isASCII && labelLength>63) {
            info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
        }
    }
    परत labelLength;
}

स्थिरांक uint32_t L_MASK=U_MASK(U_डावा_TO_उजवा);
स्थिरांक uint32_t R_AL_MASK=U_MASK(U_उजवा_TO_डावा)|U_MASK(U_उजवा_TO_डावा_ARABIC);
स्थिरांक uint32_t L_R_AL_MASK=L_MASK|R_AL_MASK;

स्थिरांक uint32_t R_AL_AN_MASK=R_AL_MASK|U_MASK(U_ARABIC_संख्या);

स्थिरांक uint32_t EN_AN_MASK=U_MASK(U_EUROPEAN_संख्या)|U_MASK(U_ARABIC_संख्या);
स्थिरांक uint32_t R_AL_EN_AN_MASK=R_AL_MASK|EN_AN_MASK;
स्थिरांक uint32_t L_EN_MASK=L_MASK|U_MASK(U_EUROPEAN_संख्या);

स्थिरांक uint32_t ES_CS_ET_ON_BN_NSM_MASK=
    U_MASK(U_EUROPEAN_संख्या_SEPARATOR)|
    U_MASK(U_COMMON_संख्या_SEPARATOR)|
    U_MASK(U_EUROPEAN_संख्या_TERMINATOR)|
    U_MASK(U_OTHER_NEUTRAL)|
    U_MASK(U_BOUNDARY_NEUTRAL)|
    U_MASK(U_DIR_NON_SPACING_MARK);
स्थिरांक uint32_t L_EN_ES_CS_ET_ON_BN_NSM_MASK=L_EN_MASK|ES_CS_ET_ON_BN_NSM_MASK;
स्थिरांक uint32_t R_AL_AN_EN_ES_CS_ET_ON_BN_NSM_MASK=R_AL_MASK|EN_AN_MASK|ES_CS_ET_ON_BN_NSM_MASK;

// We scan the whole label and check both for whether it contains RTL characters
// and whether it passes the BiDi Rule.
// In a BiDi domain name, all labels must pass the BiDi Rule, but we might find
// that a domain name is a BiDi domain name (has an RTL label) only after
// processing several earlier labels.
शून्य
UTS46::checkLabelBiDi(स्थिरांक char16_t *label, int32_t labelLength, IDNAInfo &info) स्थिरांक {
    // IDNA2008 BiDi rule
    // Get the directionality of the first character.
    UChar32 c;
    int32_t i=0;
    U16_NEXT_UNSAFE(label, i, c);
    uint32_t firstMask=U_MASK(u_charDirection(c));
    // 1. The first character must be a character with BIDI property L, R
    // or AL.  If it has the R or AL property, it is an RTL label; if it
    // has the L property, it is an LTR label.
    जर((firstMask&~L_R_AL_MASK)!=0) {
        info.isOkBiDi=असत्य;
    }
    // Get the directionality of the last non-NSM character.
    uint32_t lastMask;
    साठी(;;) {
        जर(i>=labelLength) {
            lastMask=firstMask;
            थांबा;
        }
        U16_PREV_UNSAFE(label, labelLength, c);
        UCharDirection dir=u_charDirection(c);
        जर(dir!=U_DIR_NON_SPACING_MARK) {
            lastMask=U_MASK(dir);
            थांबा;
        }
    }
    // 3. In an RTL label, the end of the label must be a character with
    // BIDI property R, AL, EN or AN, followed by zero or more
    // characters with BIDI property NSM.
    // 6. In an LTR label, the end of the label must be a character with
    // BIDI property L or EN, followed by zero or more characters with
    // BIDI property NSM.
    जर( (firstMask&L_MASK)!=0 ?
            (lastMask&~L_EN_MASK)!=0 :
            (lastMask&~R_AL_EN_AN_MASK)!=0
    ) {
        info.isOkBiDi=असत्य;
    }
    // Add the directionalities of the intervening characters.
    uint32_t mask=firstMask|lastMask;
    पर्यंत(i<labelLength) {
        U16_NEXT_UNSAFE(label, i, c);
        mask|=U_MASK(u_charDirection(c));
    }
    जर(firstMask&L_MASK) {
        // 5. In an LTR label, only characters with the BIDI properties L, EN,
        // ES, CS, ET, ON, BN and NSM are allowed.
        जर((mask&~L_EN_ES_CS_ET_ON_BN_NSM_MASK)!=0) {
            info.isOkBiDi=असत्य;
        }
    } अन्यथा {
        // 2. In an RTL label, only characters with the BIDI properties R, AL,
        // AN, EN, ES, CS, ET, ON, BN and NSM are allowed.
        जर((mask&~R_AL_AN_EN_ES_CS_ET_ON_BN_NSM_MASK)!=0) {
            info.isOkBiDi=असत्य;
        }
        // 4. In an RTL label, if an EN is present, no AN may be present, and
        // vice versa.
        जर((mask&EN_AN_MASK)==EN_AN_MASK) {
            info.isOkBiDi=असत्य;
        }
    }
    // An RTL label is a label that contains at least one character of type
    // R, AL or AN. [...]
    // A "BIDI domain name" is a domain name that contains at least one RTL
    // label. [...]
    // The following rule, consisting of six conditions, applies to labels
    // in BIDI domain names.
    जर((mask&R_AL_AN_MASK)!=0) {
        info.isBiDi=सत्य;
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
isASCIIOkBiDi(स्थिरांक char16_t *s, int32_t length) {
    int32_t labelStart=0;
    साठी(int32_t i=0; i<length; ++i) {
        char16_t c=s[i];
        जर(c==0x2e) {  // dot
            जर(i>labelStart) {
                c=s[i-1];
                जर(!(0x61<=c && c<=0x7a) && !(0x30<=c && c<=0x39)) {
                    // Last character in the label is not an L or EN.
                    परत असत्य;
                }
            }
            labelStart=i+1;
        } अन्यथा जर(i==labelStart) {
            जर(!(0x61<=c && c<=0x7a)) {
                // First character in the label is not an L.
                परत असत्य;
            }
        } अन्यथा {
            जर(c<=0x20 && (c>=0x1c || (9<=c && c<=0xd))) {
                // Intermediate character in the label is a B, S or WS.
                परत असत्य;
            }
        }
    }
    परत सत्य;
}

// UTF-8 version, called for source ASCII prefix.
// Can contain uppercase A-Z.
// s[length-1] must be the trailing dot.
स्थिर UBool
isASCIIOkBiDi(स्थिरांक अक्षर *s, int32_t length) {
    int32_t labelStart=0;
    साठी(int32_t i=0; i<length; ++i) {
        अक्षर c=s[i];
        जर(c==0x2e) {  // dot
            जर(i>labelStart) {
                c=s[i-1];
                जर(!(0x61<=c && c<=0x7a) && !(0x41<=c && c<=0x5a) && !(0x30<=c && c<=0x39)) {
                    // Last character in the label is not an L or EN.
                    परत असत्य;
                }
            }
            labelStart=i+1;
        } अन्यथा जर(i==labelStart) {
            जर(!(0x61<=c && c<=0x7a) && !(0x41<=c && c<=0x5a)) {
                // First character in the label is not an L.
                परत असत्य;
            }
        } अन्यथा {
            जर(c<=0x20 && (c>=0x1c || (9<=c && c<=0xd))) {
                // Intermediate character in the label is a B, S or WS.
                परत असत्य;
            }
        }
    }
    परत सत्य;
}

UBool
UTS46::isLabelOkContextJ(स्थिरांक char16_t *label, int32_t labelLength) स्थिरांक {
    // [IDNA2008-Tables]
    // 200C..200D  ; CONTEXTJ    # ZERO WIDTH NON-JOINER..ZERO WIDTH JOINER
    साठी(int32_t i=0; i<labelLength; ++i) {
        जर(label[i]==0x200c) {
            // Appendix A.1. ZERO WIDTH NON-JOINER
            // Rule Set:
            //  False;
            //  If Canonical_Combining_Class(Before(cp)) .eq.  Virama Then True;
            //  If RegExpMatch((Joining_Type:{L,D})(Joining_Type:T)*\u200C
            //     (Joining_Type:T)*(Joining_Type:{R,D})) Then True;
            जर(i==0) {
                परत असत्य;
            }
            UChar32 c;
            int32_t j=i;
            U16_PREV_UNSAFE(label, j, c);
            जर(uts46Norm2.getCombiningClass(c)==9) {
                सुरूठेवा;
            }
            // check precontext (Joining_Type:{L,D})(Joining_Type:T)*
            साठी(;;) {
                UJoiningType type=ubidi_getJoiningType(c);
                जर(type==U_JT_TRANSPARENT) {
                    जर(j==0) {
                        परत असत्य;
                    }
                    U16_PREV_UNSAFE(label, j, c);
                } अन्यथा जर(type==U_JT_डावा_JOINING || type==U_JT_DUAL_JOINING) {
                    थांबा;  // precontext fulfilled
                } अन्यथा {
                    परत असत्य;
                }
            }
            // check postcontext (Joining_Type:T)*(Joining_Type:{R,D})
            साठी(j=i+1;;) {
                जर(j==labelLength) {
                    परत असत्य;
                }
                U16_NEXT_UNSAFE(label, j, c);
                UJoiningType type=ubidi_getJoiningType(c);
                जर(type==U_JT_TRANSPARENT) {
                    // just skip this character
                } अन्यथा जर(type==U_JT_उजवा_JOINING || type==U_JT_DUAL_JOINING) {
                    थांबा;  // postcontext fulfilled
                } अन्यथा {
                    परत असत्य;
                }
            }
        } अन्यथा जर(label[i]==0x200d) {
            // Appendix A.2. ZERO WIDTH JOINER (U+200D)
            // Rule Set:
            //  False;
            //  If Canonical_Combining_Class(Before(cp)) .eq.  Virama Then True;
            जर(i==0) {
                परत असत्य;
            }
            UChar32 c;
            int32_t j=i;
            U16_PREV_UNSAFE(label, j, c);
            जर(uts46Norm2.getCombiningClass(c)!=9) {
                परत असत्य;
            }
        }
    }
    परत सत्य;
}

शून्य
UTS46::checkLabelContextO(स्थिरांक char16_t *label, int32_t labelLength, IDNAInfo &info) स्थिरांक {
    int32_t labelEnd=labelLength-1;  // inclusive
    int32_t arabicDigits=0;  // -1 for 066x, +1 for 06Fx
    साठी(int32_t i=0; i<=labelEnd; ++i) {
        UChar32 c=label[i];
        जर(c<0xb7) {
            // ASCII fastpath
        } अन्यथा जर(c<=0x6f9) {
            जर(c==0xb7) {
                // Appendix A.3. MIDDLE DOT (U+00B7)
                // Rule Set:
                //  False;
                //  If Before(cp) .eq.  U+006C And
                //     After(cp) .eq.  U+006C Then True;
                जर(!(0<i && label[i-1]==0x6c &&
                     i<labelEnd && label[i+1]==0x6c)) {
                    info.labelErrors|=UIDNA_ERROR_CONTEXTO_PUNCTUATION;
                }
            } अन्यथा जर(c==0x375) {
                // Appendix A.4. GREEK LOWER NUMERAL SIGN (KERAIA) (U+0375)
                // Rule Set:
                //  False;
                //  If Script(After(cp)) .eq.  Greek Then True;
                UScriptCode script=USCRIPT_INVALID_CODE;
                जर(i<labelEnd) {
                    UErrorCode errorCode=U_ZERO_ERROR;
                    int32_t j=i+1;
                    U16_NEXT(label, j, labelLength, c);
                    script=uscript_getScript(c, &errorCode);
                }
                जर(script!=USCRIPT_GREEK) {
                    info.labelErrors|=UIDNA_ERROR_CONTEXTO_PUNCTUATION;
                }
            } अन्यथा जर(c==0x5f3 || c==0x5f4) {
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
                जर(0<i) {
                    UErrorCode errorCode=U_ZERO_ERROR;
                    int32_t j=i;
                    U16_PREV(label, 0, j, c);
                    script=uscript_getScript(c, &errorCode);
                }
                जर(script!=USCRIPT_HEBREW) {
                    info.labelErrors|=UIDNA_ERROR_CONTEXTO_PUNCTUATION;
                }
            } अन्यथा जर(0x660<=c /* && c<=0x6f9 */) {
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
                जर(c<=0x669) {
                    जर(arabicDigits>0) {
                        info.labelErrors|=UIDNA_ERROR_CONTEXTO_DIGITS;
                    }
                    arabicDigits=-1;
                } अन्यथा जर(0x6f0<=c) {
                    जर(arabicDigits<0) {
                        info.labelErrors|=UIDNA_ERROR_CONTEXTO_DIGITS;
                    }
                    arabicDigits=1;
                }
            }
        } अन्यथा जर(c==0x30fb) {
            // Appendix A.7. KATAKANA MIDDLE DOT (U+30FB)
            // Rule Set:
            //  False;
            //  For All Characters:
            //    If Script(cp) .in. {Hiragana, Katakana, Han} Then True;
            //  End For;
            UErrorCode errorCode=U_ZERO_ERROR;
            साठी(पूर्णांक j=0;;) {
                जर(j>labelEnd) {
                    info.labelErrors|=UIDNA_ERROR_CONTEXTO_PUNCTUATION;
                    थांबा;
                }
                U16_NEXT(label, j, labelLength, c);
                UScriptCode script=uscript_getScript(c, &errorCode);
                जर(script==USCRIPT_HIRAGANA || script==USCRIPT_KATAKANA || script==USCRIPT_HAN) {
                    थांबा;
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
    परत reinterpret_cast<UIDNA *>(IDNA::createUTS46Instance(options, *pErrorCode));
}

U_CAPI शून्य U_EXPORT2
uidna_close(UIDNA *idna) {
    delete reinterpret_cast<IDNA *>(idna);
}

स्थिर UBool
checkArgs(स्थिरांक शून्य *label, int32_t length,
          शून्य *dest, int32_t capacity,
          UIDNAInfo *pInfo, UErrorCode *pErrorCode) {
    जर(U_FAILURE(*pErrorCode)) {
        परत असत्य;
    }
    // sizeof(UIDNAInfo)=16 in the first API version.
    जर(pInfo==nullptr || pInfo->size<16) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        परत असत्य;
    }
    जर( (label==nullptr ? length!=0 : length<-1) ||
        (dest==nullptr ? capacity!=0 : capacity<0) ||
        (dest==label && label!=nullptr)
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        परत असत्य;
    }
    // Set all *pInfo bytes to 0 except for the size field itself.
    uprv_memset(&pInfo->size+1, 0, pInfo->size-आकार(pInfo->size));
    परत सत्य;
}

स्थिर शून्य
idnaInfoToStruct(IDNAInfo &info, UIDNAInfo *pInfo) {
    pInfo->isTransitionalDifferent=info.isTransitionalDifferent();
    pInfo->errors=info.getErrors();
}

U_CAPI int32_t U_EXPORT2
uidna_labelToASCII(स्थिरांक UIDNA *idna,
                   स्थिरांक char16_t *label, int32_t length,
                   char16_t *dest, int32_t capacity,
                   UIDNAInfo *pInfo, UErrorCode *pErrorCode) {
    जर(!checkArgs(label, length, dest, capacity, pInfo, pErrorCode)) {
        परत 0;
    }
    UnicodeString src(length < 0, label, length);
    UnicodeString destString(dest, 0, capacity);
    IDNAInfo info;
    reinterpret_cast<स्थिरांक IDNA *>(idna)->labelToASCII(src, destString, info, *pErrorCode);
    idnaInfoToStruct(info, pInfo);
    परत destString.extract(dest, capacity, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uidna_labelToUnicode(स्थिरांक UIDNA *idna,
                     स्थिरांक char16_t *label, int32_t length,
                     char16_t *dest, int32_t capacity,
                     UIDNAInfo *pInfo, UErrorCode *pErrorCode) {
    जर(!checkArgs(label, length, dest, capacity, pInfo, pErrorCode)) {
        परत 0;
    }
    UnicodeString src(length < 0, label, length);
    UnicodeString destString(dest, 0, capacity);
    IDNAInfo info;
    reinterpret_cast<स्थिरांक IDNA *>(idna)->labelToUnicode(src, destString, info, *pErrorCode);
    idnaInfoToStruct(info, pInfo);
    परत destString.extract(dest, capacity, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uidna_nameToASCII(स्थिरांक UIDNA *idna,
                  स्थिरांक char16_t *name, int32_t length,
                  char16_t *dest, int32_t capacity,
                  UIDNAInfo *pInfo, UErrorCode *pErrorCode) {
    जर(!checkArgs(name, length, dest, capacity, pInfo, pErrorCode)) {
        परत 0;
    }
    UnicodeString src(length < 0, name, length);
    UnicodeString destString(dest, 0, capacity);
    IDNAInfo info;
    reinterpret_cast<स्थिरांक IDNA *>(idna)->nameToASCII(src, destString, info, *pErrorCode);
    idnaInfoToStruct(info, pInfo);
    परत destString.extract(dest, capacity, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uidna_nameToUnicode(स्थिरांक UIDNA *idna,
                    स्थिरांक char16_t *name, int32_t length,
                    char16_t *dest, int32_t capacity,
                    UIDNAInfo *pInfo, UErrorCode *pErrorCode) {
    जर(!checkArgs(name, length, dest, capacity, pInfo, pErrorCode)) {
        परत 0;
    }
    UnicodeString src(length < 0, name, length);
    UnicodeString destString(dest, 0, capacity);
    IDNAInfo info;
    reinterpret_cast<स्थिरांक IDNA *>(idna)->nameToUnicode(src, destString, info, *pErrorCode);
    idnaInfoToStruct(info, pInfo);
    परत destString.extract(dest, capacity, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uidna_labelToASCII_UTF8(स्थिरांक UIDNA *idna,
                        स्थिरांक अक्षर *label, int32_t length,
                        अक्षर *dest, int32_t capacity,
                        UIDNAInfo *pInfo, UErrorCode *pErrorCode) {
    जर(!checkArgs(label, length, dest, capacity, pInfo, pErrorCode)) {
        परत 0;
    }
    StringPiece src(label, length<0 ? static_cast<int32_t>(uprv_strlen(label)) : length);
    परत ByteSinkUtil::viaByteSinkToTerminatedChars(
        dest, capacity,
        [&](ByteSink& sink, UErrorCode& status) {
            IDNAInfo info;
            reinterpret_cast<स्थिरांक IDNA *>(idna)->labelToASCII_UTF8(src, sink, info, status);
            idnaInfoToStruct(info, pInfo);
        },
        *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uidna_labelToUnicodeUTF8(स्थिरांक UIDNA *idna,
                         स्थिरांक अक्षर *label, int32_t length,
                         अक्षर *dest, int32_t capacity,
                         UIDNAInfo *pInfo, UErrorCode *pErrorCode) {
    जर(!checkArgs(label, length, dest, capacity, pInfo, pErrorCode)) {
        परत 0;
    }
    StringPiece src(label, length<0 ? static_cast<int32_t>(uprv_strlen(label)) : length);
    परत ByteSinkUtil::viaByteSinkToTerminatedChars(
        dest, capacity,
        [&](ByteSink& sink, UErrorCode& status) {
            IDNAInfo info;
            reinterpret_cast<स्थिरांक IDNA *>(idna)->labelToUnicodeUTF8(src, sink, info, status);
            idnaInfoToStruct(info, pInfo);
        },
        *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uidna_nameToASCII_UTF8(स्थिरांक UIDNA *idna,
                       स्थिरांक अक्षर *name, int32_t length,
                       अक्षर *dest, int32_t capacity,
                       UIDNAInfo *pInfo, UErrorCode *pErrorCode) {
    जर(!checkArgs(name, length, dest, capacity, pInfo, pErrorCode)) {
        परत 0;
    }
    StringPiece src(name, length<0 ? static_cast<int32_t>(uprv_strlen(name)) : length);
    परत ByteSinkUtil::viaByteSinkToTerminatedChars(
        dest, capacity,
        [&](ByteSink& sink, UErrorCode& status) {
            IDNAInfo info;
            reinterpret_cast<स्थिरांक IDNA *>(idna)->nameToASCII_UTF8(src, sink, info, status);
            idnaInfoToStruct(info, pInfo);
        },
        *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uidna_nameToUnicodeUTF8(स्थिरांक UIDNA *idna,
                        स्थिरांक अक्षर *name, int32_t length,
                        अक्षर *dest, int32_t capacity,
                        UIDNAInfo *pInfo, UErrorCode *pErrorCode) {
    जर(!checkArgs(name, length, dest, capacity, pInfo, pErrorCode)) {
        परत 0;
    }
    StringPiece src(name, length<0 ? static_cast<int32_t>(uprv_strlen(name)) : length);
    परत ByteSinkUtil::viaByteSinkToTerminatedChars(
        dest, capacity,
        [&](ByteSink& sink, UErrorCode& status) {
            IDNAInfo info;
            reinterpret_cast<स्थिरांक IDNA *>(idna)->nameToUnicodeUTF8(src, sink, info, status);
            idnaInfoToStruct(info, pInfo);
        },
        *pErrorCode);
}

#endif  // UCONFIG_NO_IDNA
