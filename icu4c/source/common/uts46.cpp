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

#समावेश "unicode/utypes.h"

#अगर !UCONFIG_NO_IDNA

#समावेश "unicode/bytestream.h"
#समावेश "unicode/idna.h"
#समावेश "unicode/normalizer2.h"
#समावेश "unicode/uscript.h"
#समावेश "unicode/ustring.h"
#समावेश "unicode/utf16.h"
#समावेश "bytesinkutil.h"
#समावेश "cmemory.h"
#समावेश "cstring.h"
#समावेश "punycode.h"
#समावेश "ubidi_props.h"

// Note about tests for UIDNA_ERROR_DOMAIN_NAME_TOO_LONG:
//
// The domain name length limit is 255 octets in an internal DNS representation
// where the last ("root") label is the empty label
// represented by length byte 0 alone.
// In a conventional string, this translates to 253 characters, or 254
// if there is a trailing dot for the root label.

U_NAMESPACE_BEGIN

// Severe errors which usually result in a U+FFFD replacement character in the result string.
स्थिर uint32_t severeErrors=
    UIDNA_ERROR_LEADING_COMBINING_MARK|
    UIDNA_ERROR_DISALLOWED|
    UIDNA_ERROR_PUNYCODE|
    UIDNA_ERROR_LABEL_HAS_DOT|
    UIDNA_ERROR_INVALID_ACE_LABEL;

अटल अंतरभूत UBool
isASCIIString(स्थिर UnicodeString &dest) {
    स्थिर char16_t *s=dest.get_बफर();
    स्थिर char16_t *limit=s+dest.length();
    जबतक(s<limit) {
        अगर(*s++>0x7f) {
            वापस false;
        }
    }
    वापस true;
}

अटल UBool
isASCIIOkBiDi(स्थिर char16_t *s, int32_t length);

अटल UBool
isASCIIOkBiDi(स्थिर अक्षर *s, int32_t length);

// IDNA class default implementations -------------------------------------- ***

IDNA::~IDNA() {}

व्योम
IDNA::labelToASCII_UTF8(StringPiece label, ByteSink &dest,
                        IDNAInfo &info, UErrorCode &errorCode) स्थिर {
    अगर(U_SUCCESS(errorCode)) {
        UnicodeString destString;
        labelToASCII(UnicodeString::fromUTF8(label), destString,
                     info, errorCode).toUTF8(dest);
    }
}

व्योम
IDNA::labelToUnicodeUTF8(StringPiece label, ByteSink &dest,
                         IDNAInfo &info, UErrorCode &errorCode) स्थिर {
    अगर(U_SUCCESS(errorCode)) {
        UnicodeString destString;
        labelToUnicode(UnicodeString::fromUTF8(label), destString,
                       info, errorCode).toUTF8(dest);
    }
}

व्योम
IDNA::nameToASCII_UTF8(StringPiece name, ByteSink &dest,
                       IDNAInfo &info, UErrorCode &errorCode) स्थिर {
    अगर(U_SUCCESS(errorCode)) {
        UnicodeString destString;
        nameToASCII(UnicodeString::fromUTF8(name), destString,
                    info, errorCode).toUTF8(dest);
    }
}

व्योम
IDNA::nameToUnicodeUTF8(StringPiece name, ByteSink &dest,
                        IDNAInfo &info, UErrorCode &errorCode) स्थिर {
    अगर(U_SUCCESS(errorCode)) {
        UnicodeString destString;
        nameToUnicode(UnicodeString::fromUTF8(name), destString,
                      info, errorCode).toUTF8(dest);
    }
}

// UTS46 class declaration ------------------------------------------------- ***

श्रेणी UTS46 : खुला IDNA {
खुला:
    UTS46(uint32_t options, UErrorCode &errorCode);
    भव ~UTS46();

    भव UnicodeString&
    labelToASCII(स्थिर UnicodeString& label,
                 UnicodeString& dest U_LIFETIME_BOUND,
                 IDNAInfo& info,
                 UErrorCode& errorCode) स्थिर override;

    भव UnicodeString&
    labelToUnicode(स्थिर UnicodeString& label,
                   UnicodeString& dest U_LIFETIME_BOUND,
                   IDNAInfo& info,
                   UErrorCode& errorCode) स्थिर override;

    भव UnicodeString&
    nameToASCII(स्थिर UnicodeString& name,
                UnicodeString& dest U_LIFETIME_BOUND,
                IDNAInfo& info,
                UErrorCode& errorCode) स्थिर override;

    भव UnicodeString&
    nameToUnicode(स्थिर UnicodeString& name,
                  UnicodeString& dest U_LIFETIME_BOUND,
                  IDNAInfo& info,
                  UErrorCode& errorCode) स्थिर override;

    भव व्योम
    labelToASCII_UTF8(StringPiece label, ByteSink &dest,
                      IDNAInfo &info, UErrorCode &errorCode) स्थिर override;

    भव व्योम
    labelToUnicodeUTF8(StringPiece label, ByteSink &dest,
                       IDNAInfo &info, UErrorCode &errorCode) स्थिर override;

    भव व्योम
    nameToASCII_UTF8(StringPiece name, ByteSink &dest,
                     IDNAInfo &info, UErrorCode &errorCode) स्थिर override;

    भव व्योम
    nameToUnicodeUTF8(StringPiece name, ByteSink &dest,
                      IDNAInfo &info, UErrorCode &errorCode) स्थिर override;

निजी:
    UnicodeString&
    process(स्थिर UnicodeString& src,
            UBool isLabel,
            UBool toASCII,
            UnicodeString& dest U_LIFETIME_BOUND,
            IDNAInfo& info,
            UErrorCode& errorCode) स्थिर;

    व्योम
    processUTF8(StringPiece src,
                UBool isLabel, UBool toASCII,
                ByteSink &dest,
                IDNAInfo &info, UErrorCode &errorCode) स्थिर;

    UnicodeString&
    processUnicode(स्थिर UnicodeString& src,
                   int32_t labelStart,
                   int32_t mappingStart,
                   UBool isLabel,
                   UBool toASCII,
                   UnicodeString& dest U_LIFETIME_BOUND,
                   IDNAInfo& info,
                   UErrorCode& errorCode) स्थिर;

    // returns the new dest.length()
    int32_t
    mapDevChars(UnicodeString &dest, int32_t labelStart, int32_t mappingStart,
                UErrorCode &errorCode) स्थिर;

    // returns the new label length
    int32_t
    processLabel(UnicodeString &dest,
                 int32_t labelStart, int32_t label_लंबाई,
                 UBool toASCII,
                 IDNAInfo &info, UErrorCode &errorCode) स्थिर;
    int32_t
    markBadACELabel(UnicodeString &dest,
                    int32_t labelStart, int32_t label_लंबाई,
                    UBool toASCII, IDNAInfo &info, UErrorCode &errorCode) स्थिर;

    व्योम
    checkLabelBiDi(स्थिर char16_t *label, int32_t label_लंबाई, IDNAInfo &info) स्थिर;

    UBool
    isLabelOkContextJ(स्थिर char16_t *label, int32_t label_लंबाई) स्थिर;

    व्योम
    checkLabelContextO(स्थिर char16_t *label, int32_t label_लंबाई, IDNAInfo &info) स्थिर;

    स्थिर Normalizer2 &uts46Norm2;  // uts46.nrm
    uint32_t options;
};

IDNA *
IDNA::createUTS46Instance(uint32_t options, UErrorCode &errorCode) {
    अगर(U_SUCCESS(errorCode)) {
        IDNA *idna=नया UTS46(options, errorCode);
        अगर(idna==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
        } अन्यथा अगर(U_FAILURE(errorCode)) {
            मिटाओ idna;
            idna=nullptr;
        }
        वापस idna;
    } अन्यथा {
        वापस nullptr;
    }
}

// UTS46 implementation ---------------------------------------------------- ***

UTS46::UTS46(uint32_t opt, UErrorCode &errorCode)
        : uts46Norm2(*Normalizer2::getInstance(nullptr, "uts46", UNORM2_COMPOSE, errorCode)),
          options(opt) {}

UTS46::~UTS46() {}

UnicodeString&
UTS46::labelToASCII(स्थिर UnicodeString& label,
                    UnicodeString& dest U_LIFETIME_BOUND,
                    IDNAInfo& info,
                    UErrorCode& errorCode) स्थिर {
    वापस process(label, true, true, dest, info, errorCode);
}

UnicodeString&
UTS46::labelToUnicode(स्थिर UnicodeString& label,
                      UnicodeString& dest U_LIFETIME_BOUND,
                      IDNAInfo& info,
                      UErrorCode& errorCode) स्थिर {
    वापस process(label, true, false, dest, info, errorCode);
}

UnicodeString&
UTS46::nameToASCII(स्थिर UnicodeString& name,
                   UnicodeString& dest U_LIFETIME_BOUND,
                   IDNAInfo& info,
                   UErrorCode& errorCode) स्थिर {
    process(name, false, true, dest, info, errorCode);
    अगर( dest.length()>=254 && (info.errors&UIDNA_ERROR_DOMAIN_NAME_TOO_LONG)==0 &&
        isASCIIString(dest) &&
        (dest.length()>254 || dest[253]!=0x2e)
    ) {
        info.errors|=UIDNA_ERROR_DOMAIN_NAME_TOO_LONG;
    }
    वापस dest;
}

UnicodeString&
UTS46::nameToUnicode(स्थिर UnicodeString& name,
                     UnicodeString& dest U_LIFETIME_BOUND,
                     IDNAInfo& info,
                     UErrorCode& errorCode) स्थिर {
    वापस process(name, false, false, dest, info, errorCode);
}

व्योम
UTS46::labelToASCII_UTF8(StringPiece label, ByteSink &dest,
                         IDNAInfo &info, UErrorCode &errorCode) स्थिर {
    processUTF8(label, true, true, dest, info, errorCode);
}

व्योम
UTS46::labelToUnicodeUTF8(StringPiece label, ByteSink &dest,
                          IDNAInfo &info, UErrorCode &errorCode) स्थिर {
    processUTF8(label, true, false, dest, info, errorCode);
}

व्योम
UTS46::nameToASCII_UTF8(StringPiece name, ByteSink &dest,
                        IDNAInfo &info, UErrorCode &errorCode) स्थिर {
    processUTF8(name, false, true, dest, info, errorCode);
}

व्योम
UTS46::nameToUnicodeUTF8(StringPiece name, ByteSink &dest,
                         IDNAInfo &info, UErrorCode &errorCode) स्थिर {
    processUTF8(name, false, false, dest, info, errorCode);
}

// UTS #46 data for ASCII characters.
// The normalizer (using uts46.nrm) maps uppercase ASCII letters to lowercase
// and passes through all other ASCII characters.
// If UIDNA_USE_STD3_RULES is set, then non-LDH characters are disallowed
// using this data.
// The ASCII fastpath also uses this data.
// Values: -1=disallowed  0==valid  1==mapped (lowercase)
अटल स्थिर int8_t ascii_तथ्य[128]={
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
UTS46::process(स्थिर UnicodeString& src,
               UBool isLabel,
               UBool toASCII,
               UnicodeString& dest U_LIFETIME_BOUND,
               IDNAInfo& info,
               UErrorCode& errorCode) स्थिर {
    // uts46Norm2.normalize() would do all of this error checking and setup,
    // but with the ASCII fastpath we do not always call it, and do not
    // call it first.
    अगर(U_FAILURE(errorCode)) {
        dest.setToBogus();
        वापस dest;
    }
    स्थिर char16_t *srcArray=src.get_बफर();
    अगर(&dest==&src || srcArray==nullptr) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        dest.setToBogus();
        वापस dest;
    }
    // Arguments are fine, reset output values.
    dest.हटाओ();
    info.reset();
    int32_t src_लंबाई=src.length();
    अगर(src_लंबाई==0) {
        info.errors|=UIDNA_ERROR_EMPTY_LABEL;
        वापस dest;
    }
    char16_t *destArray=dest.get_बफर(src_लंबाई);
    अगर(destArray==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        वापस dest;
    }
    // ASCII fastpath
    UBool disallowNonLDHDot=(options&UIDNA_USE_STD3_RULES)!=0;
    int32_t labelStart=0;
    int32_t i;
    क्रम(i=0;; ++i) {
        अगर(i==src_लंबाई) {
            अगर(toASCII) {
                अगर((i-labelStart)>63) {
                    info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
                }
                // There is a trailing dot if labelStart==i.
                अगर(!isLabel && i>=254 && (i>254 || labelStart<i)) {
                    info.errors|=UIDNA_ERROR_DOMAIN_NAME_TOO_LONG;
                }
            }
            info.errors|=info.labelErrors;
            dest.release_बफर(i);
            वापस dest;
        }
        char16_t c=srcArray[i];
        अगर(c>0x7f) {
            अवरोध;
        }
        पूर्णांक c_तथ्य=ascii_तथ्य[c];
        अगर(c_तथ्य>0) {
            destArray[i]=c+0x20;  // Lowercase an uppercase ASCII letter.
        } अन्यथा अगर(c_तथ्य<0 && disallowNonLDHDot) {
            अवरोध;  // Replacing with U+FFFD can be complicated for toASCII.
        } अन्यथा {
            destArray[i]=c;
            अगर(c==0x2d) {  // hyphen
                अगर(i==(labelStart+3) && srcArray[i-1]==0x2d) {
                    // "??--..." is Punycode or forbidden.
                    ++i;  // '-' was copied to dest already
                    अवरोध;
                }
                अगर(i==labelStart) {
                    // label starts with "-"
                    info.labelErrors|=UIDNA_ERROR_LEADING_HYPHEN;
                }
                अगर((i+1)==src_लंबाई || srcArray[i+1]==0x2e) {
                    // label ends with "-"
                    info.labelErrors|=UIDNA_ERROR_TRAILING_HYPHEN;
                }
            } अन्यथा अगर(c==0x2e) {  // dot
                अगर(isLabel) {
                    // Replacing with U+FFFD can be complicated for toASCII.
                    ++i;  // '.' was copied to dest already
                    अवरोध;
                }
                अगर(i==labelStart) {
                    info.labelErrors|=UIDNA_ERROR_EMPTY_LABEL;
                }
                अगर(toASCII && (i-labelStart)>63) {
                    info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
                }
                info.errors|=info.labelErrors;
                info.labelErrors=0;
                labelStart=i+1;
            }
        }
    }
    info.errors|=info.labelErrors;
    dest.release_बफर(i);
    processUnicode(src, labelStart, i, isLabel, toASCII, dest, info, errorCode);
    अगर( info.isBiDi && U_SUCCESS(errorCode) && (info.errors&severeErrors)==0 &&
        (!info.isOkBiDi || (labelStart>0 && !isASCIIOkBiDi(dest.get_बफर(), labelStart)))
    ) {
        info.errors|=UIDNA_ERROR_BIDI;
    }
    वापस dest;
}

व्योम
UTS46::processUTF8(StringPiece src,
                   UBool isLabel, UBool toASCII,
                   ByteSink &dest,
                   IDNAInfo &info, UErrorCode &errorCode) स्थिर {
    अगर(U_FAILURE(errorCode)) {
        वापस;
    }
    स्थिर अक्षर *srcArray=src.तथ्य();
    int32_t src_लंबाई=src.length();
    अगर(srcArray==nullptr && src_लंबाई!=0) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        वापस;
    }
    // Arguments are fine, reset output values.
    info.reset();
    अगर(src_लंबाई==0) {
        info.errors|=UIDNA_ERROR_EMPTY_LABEL;
        dest.Flush();
        वापस;
    }
    UnicodeString destString;
    int32_t labelStart=0;
    अगर(src_लंबाई<=256) {  // length of stackArray[]
        // ASCII fastpath
        अक्षर ढेर_Array[256];
        int32_t destCapacity;
        अक्षर *destArray=dest.Get_Append_बफर(src_लंबाई, src_लंबाई+20,
                                             ढेर_Array, UPRV_LENGTHOF(ढेर_Array), &destCapacity);
        UBool disallowNonLDHDot=(options&UIDNA_USE_STD3_RULES)!=0;
        int32_t i;
        क्रम(i=0;; ++i) {
            अगर(i==src_लंबाई) {
                अगर(toASCII) {
                    अगर((i-labelStart)>63) {
                        info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
                    }
                    // There is a trailing dot if labelStart==i.
                    अगर(!isLabel && i>=254 && (i>254 || labelStart<i)) {
                        info.errors|=UIDNA_ERROR_DOMAIN_NAME_TOO_LONG;
                    }
                }
                info.errors|=info.labelErrors;
                dest.Append(destArray, i);
                dest.Flush();
                वापस;
            }
            अक्षर c=srcArray[i];
            अगर (static_cast<int8_t>(c) < 0) { // (uint8_t)c>0x7f
                अवरोध;
            }
            पूर्णांक c_तथ्य = ascii_तथ्य[static_cast<पूर्णांक>(c)]; // Cast: gcc warns about indexing with a char.
            अगर(c_तथ्य>0) {
                destArray[i]=c+0x20;  // Lowercase an uppercase ASCII letter.
            } अन्यथा अगर(c_तथ्य<0 && disallowNonLDHDot) {
                अवरोध;  // Replacing with U+FFFD can be complicated for toASCII.
            } अन्यथा {
                destArray[i]=c;
                अगर(c==0x2d) {  // hyphen
                    अगर(i==(labelStart+3) && srcArray[i-1]==0x2d) {
                        // "??--..." is Punycode or forbidden.
                        अवरोध;
                    }
                    अगर(i==labelStart) {
                        // label starts with "-"
                        info.labelErrors|=UIDNA_ERROR_LEADING_HYPHEN;
                    }
                    अगर((i+1)==src_लंबाई || srcArray[i+1]==0x2e) {
                        // label ends with "-"
                        info.labelErrors|=UIDNA_ERROR_TRAILING_HYPHEN;
                    }
                } अन्यथा अगर(c==0x2e) {  // dot
                    अगर(isLabel) {
                        अवरोध;  // Replacing with U+FFFD can be complicated for toASCII.
                    }
                    अगर(i==labelStart) {
                        info.labelErrors|=UIDNA_ERROR_EMPTY_LABEL;
                    }
                    अगर(toASCII && (i-labelStart)>63) {
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
    अगर(toASCII && !isLabel) {
        // length==labelStart==254 means that there is a trailing dot (ok) and
        // destString is empty (do not index at 253-labelStart).
        int32_t length=labelStart+destString.length();
        अगर( length>=254 && isASCIIString(destString) &&
            (length>254 ||
             (labelStart<254 && destString[253-labelStart]!=0x2e))
        ) {
            info.errors|=UIDNA_ERROR_DOMAIN_NAME_TOO_LONG;
        }
    }
    अगर( info.isBiDi && U_SUCCESS(errorCode) && (info.errors&severeErrors)==0 &&
        (!info.isOkBiDi || (labelStart>0 && !isASCIIOkBiDi(srcArray, labelStart)))
    ) {
        info.errors|=UIDNA_ERROR_BIDI;
    }
}

UnicodeString&
UTS46::processUnicode(स्थिर UnicodeString& src,
                      int32_t labelStart,
                      int32_t mappingStart,
                      UBool isLabel,
                      UBool toASCII,
                      UnicodeString& dest U_LIFETIME_BOUND,
                      IDNAInfo& info,
                      UErrorCode& errorCode) स्थिर {
    अगर(mappingStart==0) {
        uts46Norm2.normalize(src, dest, errorCode);
    } अन्यथा {
        uts46Norm2.normalizeSecondAndAppend(dest, src.अस्थायी_Sub_String(mappingStart), errorCode);
    }
    अगर(U_FAILURE(errorCode)) {
        वापस dest;
    }
    UBool doMapDevChars=
        toASCII ? (options&UIDNA_NONTRANSITIONAL_TO_ASCII)==0 :
                  (options&UIDNA_NONTRANSITIONAL_TO_UNICODE)==0;
    स्थिर char16_t *destArray=dest.get_बफर();
    int32_t dest_लंबाई=dest.length();
    int32_t labelLimit=labelStart;
    जबतक(labelLimit<dest_लंबाई) {
        char16_t c=destArray[labelLimit];
        अगर(c==0x2e && !isLabel) {
            int32_t label_लंबाई=labelLimit-labelStart;
            int32_t new_लंबाई=processLabel(dest, labelStart, label_लंबाई,
                                            toASCII, info, errorCode);
            info.errors|=info.labelErrors;
            info.labelErrors=0;
            अगर(U_FAILURE(errorCode)) {
                वापस dest;
            }
            destArray=dest.get_बफर();
            dest_लंबाई+=new_लंबाई-label_लंबाई;
            labelLimit=labelStart+=new_लंबाई+1;
            जारी;
        } अन्यथा अगर(c<0xdf) {
            // pass
        } अन्यथा अगर(c<=0x200d && (c==0xdf || c==0x3c2 || c>=0x200c)) {
            info.isTransDiff=true;
            अगर(doMapDevChars) {
                dest_लंबाई=mapDevChars(dest, labelStart, labelLimit, errorCode);
                अगर(U_FAILURE(errorCode)) {
                    वापस dest;
                }
                destArray=dest.get_बफर();
                // All deviation characters have been mapped, no need to check for them again.
                doMapDevChars=false;
                // Do not increment labelLimit in case c was removed.
                जारी;
            }
        } अन्यथा अगर(U16_IS_SURROGATE(c)) {
            अगर(U16_IS_SURROGATE_LEAD(c) ?
                    (labelLimit+1)==dest_लंबाई || !U16_IS_TRAIL(destArray[labelLimit+1]) :
                    labelLimit==labelStart || !U16_IS_LEAD(destArray[labelLimit-1])) {
                // Map an unpaired surrogate to U+FFFD before normalization so that when
                // that removes characters we do not turn two unpaired ones into a pair.
                info.labelErrors|=UIDNA_ERROR_DISALLOWED;
                dest.setCharAt(labelLimit, 0xfffd);
                destArray=dest.get_बफर();
            }
        }
        ++labelLimit;
    }
    // Permit an empty label at the end (0<labelStart==labelLimit==destLength is ok)
    // but not an empty label elsewhere nor a completely empty domain name.
    // processLabel() sets UIDNA_ERROR_EMPTY_LABEL when labelLength==0.
    अगर(0==labelStart || labelStart<labelLimit) {
        processLabel(dest, labelStart, labelLimit-labelStart,
                      toASCII, info, errorCode);
        info.errors|=info.labelErrors;
    }
    वापस dest;
}

int32_t
UTS46::mapDevChars(UnicodeString &dest, int32_t labelStart, int32_t mappingStart,
                   UErrorCode &errorCode) स्थिर {
    अगर(U_FAILURE(errorCode)) {
        वापस 0;
    }
    int32_t length=dest.length();
    char16_t *s=dest.get_बफर(dest[mappingStart]==0xdf ? length+1 : length);
    अगर(s==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        वापस length;
    }
    int32_t capacity=dest.getCapacity();
    UBool didMapDevChars=false;
    int32_t read_सूचकांक=mappingStart, write_सूचकांक=mappingStart;
    करो {
        char16_t c=s[read_सूचकांक++];
        चयन(c) {
        हाल 0xdf:
            // Map sharp s to ss.
            didMapDevChars=true;
            s[write_सूचकांक++]=0x73;  // Replace sharp s with first s.
            // Insert second s and account for possible buffer reallocation.
            अगर(write_सूचकांक==read_सूचकांक) {
                अगर(length==capacity) {
                    dest.release_बफर(length);
                    s=dest.get_बफर(length+1);
                    अगर(s==nullptr) {
                        errorCode=U_MEMORY_ALLOCATION_ERROR;
                        वापस length;
                    }
                    capacity=dest.getCapacity();
                }
                u_memmove(s+write_सूचकांक+1, s+write_सूचकांक, length-write_सूचकांक);
                ++read_सूचकांक;
            }
            s[write_सूचकांक++]=0x73;
            ++length;
            अवरोध;
        हाल 0x3c2:  // Map final sigma to nonfinal sigma.
            didMapDevChars=true;
            s[write_सूचकांक++]=0x3c3;
            अवरोध;
        हाल 0x200c:  // Ignore/remove ZWNJ.
        हाल 0x200d:  // Ignore/remove ZWJ.
            didMapDevChars=true;
            --length;
            अवरोध;
        शेष:
            // Only really necessary if writeIndex was different from readIndex.
            s[write_सूचकांक++]=c;
            अवरोध;
        }
    } जबतक(write_सूचकांक<length);
    dest.release_बफर(length);
    अगर(didMapDevChars) {
        // Mapping deviation characters might have resulted in an un-NFC string.
        // We could use either the NFC or the UTS #46 normalizer.
        // By using the UTS #46 normalizer again, we avoid having to load a second .nrm data file.
        UnicodeString normalized;
        uts46Norm2.normalize(dest.अस्थायी_Sub_String(labelStart), normalized, errorCode);
        अगर(U_SUCCESS(errorCode)) {
            dest.replace(labelStart, 0x7fffffff, normalized);
            अगर(dest.isBogus()) {
                errorCode=U_MEMORY_ALLOCATION_ERROR;
            }
            वापस dest.length();
        }
    }
    वापस length;
}

// Replace the label in dest with the label string, if the label was modified.
// If &label==&dest then the label was modified in-place and labelLength
// is the new label length, different from label.length().
// If &label!=&dest then labelLength==label.length().
// Returns labelLength (= the new label length).
अटल int32_t
replaceLabel(UnicodeString &dest, int32_t destLabelStart, int32_t dest_Label_लंबाई,
             स्थिर UnicodeString &label, int32_t label_लंबाई, UErrorCode &errorCode) {
    अगर(U_FAILURE(errorCode)) {
        वापस 0;
    }
    अगर(&label!=&dest) {
        dest.replace(destLabelStart, dest_Label_लंबाई, label);
        अगर(dest.isBogus()) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            वापस 0;
        }
    }
    वापस label_लंबाई;
}

int32_t
UTS46::processLabel(UnicodeString &dest,
                    int32_t labelStart, int32_t label_लंबाई,
                    UBool toASCII,
                    IDNAInfo &info, UErrorCode &errorCode) स्थिर {
    अगर(U_FAILURE(errorCode)) {
        वापस 0;
    }
    UnicodeString fromPunycode;
    UnicodeString *labelString;
    स्थिर char16_t *label=dest.get_बफर()+labelStart;
    int32_t destLabelStart=labelStart;
    int32_t dest_Label_लंबाई=label_लंबाई;
    UBool wasPunycode;
    अगर(label_लंबाई>=4 && label[0]==0x78 && label[1]==0x6e && label[2]==0x2d && label[3]==0x2d) {
        // Label starts with "xn--", try to un-Punycode it.
        // In IDNA2008, labels like "xn--" (decodes to an empty string) and
        // "xn--ASCII-" (decodes to just "ASCII") fail the round-trip validation from
        // comparing the ToUnicode input with the back-to-ToASCII output.
        // They are alternate encodings of the respective ASCII labels.
        // Ignore "xn---" here: It will fail Punycode.decode() which logically comes before
        // the round-trip verification.
        अगर(label_लंबाई==4 || (label_लंबाई>5 && label[label_लंबाई-1]==u'-')) {
            info.labelErrors|=UIDNA_ERROR_INVALID_ACE_LABEL;
            वापस markBadACELabel(dest, labelStart, label_लंबाई, toASCII, info, errorCode);
        }
        wasPunycode=true;
        char16_t *unicode_बफर=fromPunycode.get_बफर(-1);  // capacity==-1: most labels should fit
        अगर(unicode_बफर==nullptr) {
            // Should never occur if we used capacity==-1 which uses the internal buffer.
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            वापस label_लंबाई;
        }
        UErrorCode punycodeErrorCode=U_ZERO_ERROR;
        int32_t unicode_लंबाई=u_strFromPunycode(label+4, label_लंबाई-4,
                                                unicode_बफर, fromPunycode.getCapacity(),
                                                nullptr, &punycodeErrorCode);
        अगर(punycodeErrorCode==U_बफर_OVERFLOW_ERROR) {
            fromPunycode.release_बफर(0);
            unicode_बफर=fromPunycode.get_बफर(unicode_लंबाई);
            अगर(unicode_बफर==nullptr) {
                errorCode=U_MEMORY_ALLOCATION_ERROR;
                वापस label_लंबाई;
            }
            punycodeErrorCode=U_ZERO_ERROR;
            unicode_लंबाई=u_strFromPunycode(label+4, label_लंबाई-4,
                                            unicode_बफर, fromPunycode.getCapacity(),
                                            nullptr, &punycodeErrorCode);
        }
        fromPunycode.release_बफर(unicode_लंबाई);
        अगर(U_FAILURE(punycodeErrorCode)) {
            info.labelErrors|=UIDNA_ERROR_PUNYCODE;
            वापस markBadACELabel(dest, labelStart, label_लंबाई, toASCII, info, errorCode);
        }
        // Check for NFC, and for characters that are not
        // valid or deviation characters according to the normalizer.
        // If there is something wrong, then the string will change.
        // Note that the normalizer passes through non-LDH ASCII and deviation characters.
        // Deviation characters are ok in Punycode even in transitional processing.
        // In the code further below, if we find non-LDH ASCII and we have UIDNA_USE_STD3_RULES
        // then we will set UIDNA_ERROR_INVALID_ACE_LABEL there too.
        UBool isValid=uts46Norm2.isNormalized(fromPunycode, errorCode);
        अगर(U_FAILURE(errorCode)) {
            वापस label_लंबाई;
        }
        // Unicode 15.1 UTS #46:
        // Added an additional condition in 4.1 Validity Criteria to
        // disallow labels such as xn--xn---epa., which do not round-trip.
        // --> Validity Criteria new criterion 4:
        // If not CheckHyphens, the label must not begin with “xn--”.
        अगर(!isValid || fromPunycode.startsWith(UnicodeString::readOnlyAlias(u"xn--"))) {
            info.labelErrors|=UIDNA_ERROR_INVALID_ACE_LABEL;
            वापस markBadACELabel(dest, labelStart, label_लंबाई, toASCII, info, errorCode);
        }
        labelString=&fromPunycode;
        label=fromPunycode.get_बफर();
        labelStart=0;
        label_लंबाई=fromPunycode.length();
    } अन्यथा {
        wasPunycode=false;
        labelString=&dest;
    }
    // Validity check
    अगर(label_लंबाई==0) {
        info.labelErrors|=UIDNA_ERROR_EMPTY_LABEL;
        वापस replaceLabel(dest, destLabelStart, dest_Label_लंबाई,
                            *labelString, label_लंबाई, errorCode);
    }
    // labelLength>0
    अगर(label_लंबाई>=4 && label[2]==0x2d && label[3]==0x2d) {
        // label starts with "??--"
        info.labelErrors|=UIDNA_ERROR_HYPHEN_3_4;
    }
    अगर(label[0]==0x2d) {
        // label starts with "-"
        info.labelErrors|=UIDNA_ERROR_LEADING_HYPHEN;
    }
    अगर(label[label_लंबाई-1]==0x2d) {
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
    स्थिर char16_t *limit=label+label_लंबाई;
    char16_t oredChars=0;
    // If we enforce STD3 rules, then ASCII characters other than LDH and dot are disallowed.
    UBool disallowNonLDHDot=(options&UIDNA_USE_STD3_RULES)!=0;
    करो {
        char16_t c=*s;
        अगर(c<=0x7f) {
            अगर(c==0x2e) {
                info.labelErrors|=UIDNA_ERROR_LABEL_HAS_DOT;
                *s=0xfffd;
            } अन्यथा अगर(disallowNonLDHDot && ascii_तथ्य[c]<0) {
                info.labelErrors|=UIDNA_ERROR_DISALLOWED;
                *s=0xfffd;
            }
        } अन्यथा {
            oredChars|=c;
            अगर(c==0xfffd) {
                info.labelErrors|=UIDNA_ERROR_DISALLOWED;
            }
        }
        ++s;
    } जबतक(s<limit);
    // Check for a leading combining mark after other validity checks
    // so that we don't report UIDNA_ERROR_DISALLOWED for the U+FFFD from here.
    UChar32 c;
    int32_t cp_लंबाई=0;
    // "Unsafe" is ok because unpaired surrogates were mapped to U+FFFD.
    U16_अग्र_UNSAFE(label, cp_लंबाई, c);
    अगर((U_GET_GC_MASK(c)&U_GC_M_MASK)!=0) {
        info.labelErrors|=UIDNA_ERROR_LEADING_COMBINING_MARK;
        labelString->replace(labelStart, cp_लंबाई, static_cast<char16_t>(0xfffd));
        label=labelString->get_बफर()+labelStart;
        label_लंबाई+=1-cp_लंबाई;
        अगर(labelString==&dest) {
            dest_Label_लंबाई=label_लंबाई;
        }
    }
    अगर((info.labelErrors&severeErrors)==0) {
        // Do contextual checks only if we do not have U+FFFD from a severe error
        // because U+FFFD can make these checks fail.
        अगर((options&UIDNA_CHECK_BIDI)!=0 && (!info.isBiDi || info.isOkBiDi)) {
            checkLabelBiDi(label, label_लंबाई, info);
        }
        अगर( (options&UIDNA_CHECK_CONTEXTJ)!=0 && (oredChars&0x200c)==0x200c &&
            !isLabelOkContextJ(label, label_लंबाई)
        ) {
            info.labelErrors|=UIDNA_ERROR_CONTEXTJ;
        }
        अगर((options&UIDNA_CHECK_CONTEXTO)!=0 && oredChars>=0xb7) {
            checkLabelContextO(label, label_लंबाई, info);
        }
        अगर(toASCII) {
            अगर(wasPunycode) {
                // Leave a Punycode label unchanged if it has no severe errors.
                अगर(dest_Label_लंबाई>63) {
                    info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
                }
                वापस dest_Label_लंबाई;
            } अन्यथा अगर(oredChars>=0x80) {
                // Contains non-ASCII characters.
                UnicodeString punycode;
                char16_t *बफर=punycode.get_बफर(63);  // 63==maximum DNS label length
                अगर(बफर==nullptr) {
                    errorCode=U_MEMORY_ALLOCATION_ERROR;
                    वापस dest_Label_लंबाई;
                }
                बफर[0]=0x78;  // Write "xn--".
                बफर[1]=0x6e;
                बफर[2]=0x2d;
                बफर[3]=0x2d;
                UErrorCode punycodeErrorCode=U_ZERO_ERROR;
                int32_t punycode_लंबाई=u_strToPunycode(label, label_लंबाई,
                                                      बफर+4, punycode.getCapacity()-4,
                                                      nullptr, &punycodeErrorCode);
                अगर(punycodeErrorCode==U_बफर_OVERFLOW_ERROR) {
                    punycodeErrorCode=U_ZERO_ERROR;
                    punycode.release_बफर(4);
                    बफर=punycode.get_बफर(4+punycode_लंबाई);
                    अगर(बफर==nullptr) {
                        errorCode=U_MEMORY_ALLOCATION_ERROR;
                        वापस dest_Label_लंबाई;
                    }
                    punycode_लंबाई=u_strToPunycode(label, label_लंबाई,
                                                  बफर+4, punycode.getCapacity()-4,
                                                  nullptr, &punycodeErrorCode);
                }
                punycode_लंबाई+=4;
                punycode.release_बफर(punycode_लंबाई);
                अगर(U_FAILURE(punycodeErrorCode)) {
                    errorCode = punycodeErrorCode;
                    वापस dest_Label_लंबाई;
                }
                अगर(punycode_लंबाई>63) {
                    info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
                }
                वापस replaceLabel(dest, destLabelStart, dest_Label_लंबाई,
                                    punycode, punycode_लंबाई, errorCode);
            } अन्यथा {
                // all-ASCII label
                अगर(label_लंबाई>63) {
                    info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
                }
            }
        }
    } अन्यथा {
        // If a Punycode label has severe errors,
        // then leave it but make sure it does not look valid.
        अगर(wasPunycode) {
            info.labelErrors|=UIDNA_ERROR_INVALID_ACE_LABEL;
            वापस markBadACELabel(dest, destLabelStart, dest_Label_लंबाई, toASCII, info, errorCode);
        }
    }
    वापस replaceLabel(dest, destLabelStart, dest_Label_लंबाई,
                        *labelString, label_लंबाई, errorCode);
}

// Make sure an ACE label does not look valid.
// Append U+FFFD if the label has only LDH characters.
// If UIDNA_USE_STD3_RULES, also replace disallowed ASCII characters with U+FFFD.
int32_t
UTS46::markBadACELabel(UnicodeString &dest,
                       int32_t labelStart, int32_t label_लंबाई,
                       UBool toASCII, IDNAInfo &info, UErrorCode &errorCode) स्थिर {
    अगर(U_FAILURE(errorCode)) {
        वापस 0;
    }
    UBool disallowNonLDHDot=(options&UIDNA_USE_STD3_RULES)!=0;
    UBool isASCII=true;
    UBool onlyLDH=true;
    स्थिर char16_t *label=dest.get_बफर()+labelStart;
    स्थिर char16_t *limit=label+label_लंबाई;
    // Start after the initial "xn--".
    // Ok to cast away const because we own the UnicodeString.
    क्रम(char16_t *s=const_cast<char16_t *>(label+4); s<limit; ++s) {
        char16_t c=*s;
        अगर(c<=0x7f) {
            अगर(c==0x2e) {
                info.labelErrors|=UIDNA_ERROR_LABEL_HAS_DOT;
                *s=0xfffd;
                isASCII=onlyLDH=false;
            } अन्यथा अगर(ascii_तथ्य[c]<0) {
                onlyLDH=false;
                अगर(disallowNonLDHDot) {
                    *s=0xfffd;
                    isASCII=false;
                }
            }
        } अन्यथा {
            isASCII=onlyLDH=false;
        }
    }
    अगर(onlyLDH) {
        dest.insert(labelStart + label_लंबाई, static_cast<char16_t>(0xfffd));
        अगर(dest.isBogus()) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            वापस 0;
        }
        ++label_लंबाई;
    } अन्यथा {
        अगर(toASCII && isASCII && label_लंबाई>63) {
            info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
        }
    }
    वापस label_लंबाई;
}

स्थिर uint32_t L_MASK=U_MASK(U_बायाँ_TO_दायाँ);
स्थिर uint32_t R_AL_MASK=U_MASK(U_दायाँ_TO_बायाँ)|U_MASK(U_दायाँ_TO_बायाँ_ARABIC);
स्थिर uint32_t L_R_AL_MASK=L_MASK|R_AL_MASK;

स्थिर uint32_t R_AL_AN_MASK=R_AL_MASK|U_MASK(U_ARABIC_संख्या);

स्थिर uint32_t EN_AN_MASK=U_MASK(U_EUROPEAN_संख्या)|U_MASK(U_ARABIC_संख्या);
स्थिर uint32_t R_AL_EN_AN_MASK=R_AL_MASK|EN_AN_MASK;
स्थिर uint32_t L_EN_MASK=L_MASK|U_MASK(U_EUROPEAN_संख्या);

स्थिर uint32_t ES_CS_ET_ON_BN_NSM_MASK=
    U_MASK(U_EUROPEAN_संख्या_SEPARATOR)|
    U_MASK(U_COMMON_संख्या_SEPARATOR)|
    U_MASK(U_EUROPEAN_संख्या_TERMINATOR)|
    U_MASK(U_OTHER_NEUTRAL)|
    U_MASK(U_BOUNDARY_NEUTRAL)|
    U_MASK(U_DIR_NON_SPACING_MARK);
स्थिर uint32_t L_EN_ES_CS_ET_ON_BN_NSM_MASK=L_EN_MASK|ES_CS_ET_ON_BN_NSM_MASK;
स्थिर uint32_t R_AL_AN_EN_ES_CS_ET_ON_BN_NSM_MASK=R_AL_MASK|EN_AN_MASK|ES_CS_ET_ON_BN_NSM_MASK;

// We scan the whole label and check both for whether it contains RTL characters
// and whether it passes the BiDi Rule.
// In a BiDi domain name, all labels must pass the BiDi Rule, but we might find
// that a domain name is a BiDi domain name (has an RTL label) only after
// processing several earlier labels.
व्योम
UTS46::checkLabelBiDi(स्थिर char16_t *label, int32_t label_लंबाई, IDNAInfo &info) स्थिर {
    // IDNA2008 BiDi rule
    // Get the directionality of the first character.
    UChar32 c;
    int32_t i=0;
    U16_अग्र_UNSAFE(label, i, c);
    uint32_t firstMask=U_MASK(u_charDirection(c));
    // 1. The first character must be a character with BIDI property L, R
    // or AL.  If it has the R or AL property, it is an RTL label; if it
    // has the L property, it is an LTR label.
    अगर((firstMask&~L_R_AL_MASK)!=0) {
        info.isOkBiDi=false;
    }
    // Get the directionality of the last non-NSM character.
    uint32_t lastMask;
    क्रम(;;) {
        अगर(i>=label_लंबाई) {
            lastMask=firstMask;
            अवरोध;
        }
        U16_पूर्व_UNSAFE(label, label_लंबाई, c);
        UCharDirection dir=u_charDirection(c);
        अगर(dir!=U_DIR_NON_SPACING_MARK) {
            lastMask=U_MASK(dir);
            अवरोध;
        }
    }
    // 3. In an RTL label, the end of the label must be a character with
    // BIDI property R, AL, EN or AN, followed by zero or more
    // characters with BIDI property NSM.
    // 6. In an LTR label, the end of the label must be a character with
    // BIDI property L or EN, followed by zero or more characters with
    // BIDI property NSM.
    अगर( (firstMask&L_MASK)!=0 ?
            (lastMask&~L_EN_MASK)!=0 :
            (lastMask&~R_AL_EN_AN_MASK)!=0
    ) {
        info.isOkBiDi=false;
    }
    // Add the directionalities of the intervening characters.
    uint32_t mask=firstMask|lastMask;
    जबतक(i<label_लंबाई) {
        U16_अग्र_UNSAFE(label, i, c);
        mask|=U_MASK(u_charDirection(c));
    }
    अगर(firstMask&L_MASK) {
        // 5. In an LTR label, only characters with the BIDI properties L, EN,
        // ES, CS, ET, ON, BN and NSM are allowed.
        अगर((mask&~L_EN_ES_CS_ET_ON_BN_NSM_MASK)!=0) {
            info.isOkBiDi=false;
        }
    } अन्यथा {
        // 2. In an RTL label, only characters with the BIDI properties R, AL,
        // AN, EN, ES, CS, ET, ON, BN and NSM are allowed.
        अगर((mask&~R_AL_AN_EN_ES_CS_ET_ON_BN_NSM_MASK)!=0) {
            info.isOkBiDi=false;
        }
        // 4. In an RTL label, if an EN is present, no AN may be present, and
        // vice versa.
        अगर((mask&EN_AN_MASK)==EN_AN_MASK) {
            info.isOkBiDi=false;
        }
    }
    // An RTL label is a label that contains at least one character of type
    // R, AL or AN. [...]
    // A "BIDI domain name" is a domain name that contains at least one RTL
    // label. [...]
    // The following rule, consisting of six conditions, applies to labels
    // in BIDI domain names.
    अगर((mask&R_AL_AN_MASK)!=0) {
        info.isBiDi=true;
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
अटल UBool
isASCIIOkBiDi(स्थिर char16_t *s, int32_t length) {
    int32_t labelStart=0;
    क्रम(int32_t i=0; i<length; ++i) {
        char16_t c=s[i];
        अगर(c==0x2e) {  // dot
            अगर(i>labelStart) {
                c=s[i-1];
                अगर(!(0x61<=c && c<=0x7a) && !(0x30<=c && c<=0x39)) {
                    // Last character in the label is not an L or EN.
                    वापस false;
                }
            }
            labelStart=i+1;
        } अन्यथा अगर(i==labelStart) {
            अगर(!(0x61<=c && c<=0x7a)) {
                // First character in the label is not an L.
                वापस false;
            }
        } अन्यथा {
            अगर(c<=0x20 && (c>=0x1c || (9<=c && c<=0xd))) {
                // Intermediate character in the label is a B, S or WS.
                वापस false;
            }
        }
    }
    वापस true;
}

// UTF-8 version, called for source ASCII prefix.
// Can contain uppercase A-Z.
// s[length-1] must be the trailing dot.
अटल UBool
isASCIIOkBiDi(स्थिर अक्षर *s, int32_t length) {
    int32_t labelStart=0;
    क्रम(int32_t i=0; i<length; ++i) {
        अक्षर c=s[i];
        अगर(c==0x2e) {  // dot
            अगर(i>labelStart) {
                c=s[i-1];
                अगर(!(0x61<=c && c<=0x7a) && !(0x41<=c && c<=0x5a) && !(0x30<=c && c<=0x39)) {
                    // Last character in the label is not an L or EN.
                    वापस false;
                }
            }
            labelStart=i+1;
        } अन्यथा अगर(i==labelStart) {
            अगर(!(0x61<=c && c<=0x7a) && !(0x41<=c && c<=0x5a)) {
                // First character in the label is not an L.
                वापस false;
            }
        } अन्यथा {
            अगर(c<=0x20 && (c>=0x1c || (9<=c && c<=0xd))) {
                // Intermediate character in the label is a B, S or WS.
                वापस false;
            }
        }
    }
    वापस true;
}

UBool
UTS46::isLabelOkContextJ(स्थिर char16_t *label, int32_t label_लंबाई) स्थिर {
    // [IDNA2008-Tables]
    // 200C..200D  ; CONTEXTJ    # ZERO WIDTH NON-JOINER..ZERO WIDTH JOINER
    क्रम(int32_t i=0; i<label_लंबाई; ++i) {
        अगर(label[i]==0x200c) {
            // Appendix A.1. ZERO WIDTH NON-JOINER
            // Rule Set:
            //  False;
            //  If Canonical_Combining_Class(Before(cp)) .eq.  Virama Then True;
            //  If RegExpMatch((Joining_Type:{L,D})(Joining_Type:T)*\u200C
            //     (Joining_Type:T)*(Joining_Type:{R,D})) Then True;
            अगर(i==0) {
                वापस false;
            }
            UChar32 c;
            int32_t j=i;
            U16_पूर्व_UNSAFE(label, j, c);
            अगर(uts46Norm2.getCombiningClass(c)==9) {
                जारी;
            }
            // check precontext (Joining_Type:{L,D})(Joining_Type:T)*
            क्रम(;;) {
                UJoiningType type=ubidi_getJoiningType(c);
                अगर(type==U_JT_TRANSPARENT) {
                    अगर(j==0) {
                        वापस false;
                    }
                    U16_पूर्व_UNSAFE(label, j, c);
                } अन्यथा अगर(type==U_JT_बायाँ_JOINING || type==U_JT_DUAL_JOINING) {
                    अवरोध;  // precontext fulfilled
                } अन्यथा {
                    वापस false;
                }
            }
            // check postcontext (Joining_Type:T)*(Joining_Type:{R,D})
            क्रम(j=i+1;;) {
                अगर(j==label_लंबाई) {
                    वापस false;
                }
                U16_अग्र_UNSAFE(label, j, c);
                UJoiningType type=ubidi_getJoiningType(c);
                अगर(type==U_JT_TRANSPARENT) {
                    // just skip this character
                } अन्यथा अगर(type==U_JT_दायाँ_JOINING || type==U_JT_DUAL_JOINING) {
                    अवरोध;  // postcontext fulfilled
                } अन्यथा {
                    वापस false;
                }
            }
        } अन्यथा अगर(label[i]==0x200d) {
            // Appendix A.2. ZERO WIDTH JOINER (U+200D)
            // Rule Set:
            //  False;
            //  If Canonical_Combining_Class(Before(cp)) .eq.  Virama Then True;
            अगर(i==0) {
                वापस false;
            }
            UChar32 c;
            int32_t j=i;
            U16_पूर्व_UNSAFE(label, j, c);
            अगर(uts46Norm2.getCombiningClass(c)!=9) {
                वापस false;
            }
        }
    }
    वापस true;
}

व्योम
UTS46::checkLabelContextO(स्थिर char16_t *label, int32_t label_लंबाई, IDNAInfo &info) स्थिर {
    int32_t labelEnd=label_लंबाई-1;  // inclusive
    int32_t arabicDigits=0;  // -1 for 066x, +1 for 06Fx
    क्रम(int32_t i=0; i<=labelEnd; ++i) {
        UChar32 c=label[i];
        अगर(c<0xb7) {
            // ASCII fastpath
        } अन्यथा अगर(c<=0x6f9) {
            अगर(c==0xb7) {
                // Appendix A.3. MIDDLE DOT (U+00B7)
                // Rule Set:
                //  False;
                //  If Before(cp) .eq.  U+006C And
                //     After(cp) .eq.  U+006C Then True;
                अगर(!(0<i && label[i-1]==0x6c &&
                     i<labelEnd && label[i+1]==0x6c)) {
                    info.labelErrors|=UIDNA_ERROR_CONTEXTO_PUNCTUATION;
                }
            } अन्यथा अगर(c==0x375) {
                // Appendix A.4. GREEK LOWER NUMERAL SIGN (KERAIA) (U+0375)
                // Rule Set:
                //  False;
                //  If Script(After(cp)) .eq.  Greek Then True;
                UScriptCode script=USCRIPT_INVALID_CODE;
                अगर(i<labelEnd) {
                    UErrorCode errorCode=U_ZERO_ERROR;
                    int32_t j=i+1;
                    U16_अग्र(label, j, label_लंबाई, c);
                    script=uscript_getScript(c, &errorCode);
                }
                अगर(script!=USCRIPT_GREEK) {
                    info.labelErrors|=UIDNA_ERROR_CONTEXTO_PUNCTUATION;
                }
            } अन्यथा अगर(c==0x5f3 || c==0x5f4) {
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
                अगर(0<i) {
                    UErrorCode errorCode=U_ZERO_ERROR;
                    int32_t j=i;
                    U16_पूर्व(label, 0, j, c);
                    script=uscript_getScript(c, &errorCode);
                }
                अगर(script!=USCRIPT_HEBREW) {
                    info.labelErrors|=UIDNA_ERROR_CONTEXTO_PUNCTUATION;
                }
            } अन्यथा अगर(0x660<=c /* && c<=0x6f9 */) {
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
                अगर(c<=0x669) {
                    अगर(arabicDigits>0) {
                        info.labelErrors|=UIDNA_ERROR_CONTEXTO_DIGITS;
                    }
                    arabicDigits=-1;
                } अन्यथा अगर(0x6f0<=c) {
                    अगर(arabicDigits<0) {
                        info.labelErrors|=UIDNA_ERROR_CONTEXTO_DIGITS;
                    }
                    arabicDigits=1;
                }
            }
        } अन्यथा अगर(c==0x30fb) {
            // Appendix A.7. KATAKANA MIDDLE DOT (U+30FB)
            // Rule Set:
            //  False;
            //  For All Characters:
            //    If Script(cp) .in. {Hiragana, Katakana, Han} Then True;
            //  End For;
            UErrorCode errorCode=U_ZERO_ERROR;
            क्रम(पूर्णांक j=0;;) {
                अगर(j>labelEnd) {
                    info.labelErrors|=UIDNA_ERROR_CONTEXTO_PUNCTUATION;
                    अवरोध;
                }
                U16_अग्र(label, j, label_लंबाई, c);
                UScriptCode script=uscript_getScript(c, &errorCode);
                अगर(script==USCRIPT_HIRAGANA || script==USCRIPT_KATAKANA || script==USCRIPT_HAN) {
                    अवरोध;
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
    वापस reinterpret_cast<UIDNA *>(IDNA::createUTS46Instance(options, *pErrorCode));
}

U_CAPI व्योम U_EXPORT2
uidna_close(UIDNA *idna) {
    मिटाओ reinterpret_cast<IDNA *>(idna);
}

अटल UBool
checkArgs(स्थिर व्योम *label, int32_t length,
          व्योम *dest, int32_t capacity,
          UIDNAInfo *pInfo, UErrorCode *pErrorCode) {
    अगर(U_FAILURE(*pErrorCode)) {
        वापस false;
    }
    // sizeof(UIDNAInfo)=16 in the first API version.
    अगर(pInfo==nullptr || pInfo->size<16) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        वापस false;
    }
    अगर( (label==nullptr ? length!=0 : length<-1) ||
        (dest==nullptr ? capacity!=0 : capacity<0) ||
        (dest==label && label!=nullptr)
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        वापस false;
    }
    // Set all *pInfo bytes to 0 except for the size field itself.
    uprv_memset(&pInfo->size+1, 0, pInfo->size-माप(pInfo->size));
    वापस true;
}

अटल व्योम
idnaInfoToStruct(IDNAInfo &info, UIDNAInfo *pInfo) {
    pInfo->isTransitionalDifferent=info.isTransitionalDifferent();
    pInfo->errors=info.getErrors();
}

U_CAPI int32_t U_EXPORT2
uidna_labelToASCII(स्थिर UIDNA *idna,
                   स्थिर char16_t *label, int32_t length,
                   char16_t *dest, int32_t capacity,
                   UIDNAInfo *pInfo, UErrorCode *pErrorCode) {
    अगर(!checkArgs(label, length, dest, capacity, pInfo, pErrorCode)) {
        वापस 0;
    }
    UnicodeString src(length < 0, label, length);
    UnicodeString destString(dest, 0, capacity);
    IDNAInfo info;
    reinterpret_cast<स्थिर IDNA *>(idna)->labelToASCII(src, destString, info, *pErrorCode);
    idnaInfoToStruct(info, pInfo);
    वापस destString.extract(dest, capacity, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uidna_labelToUnicode(स्थिर UIDNA *idna,
                     स्थिर char16_t *label, int32_t length,
                     char16_t *dest, int32_t capacity,
                     UIDNAInfo *pInfo, UErrorCode *pErrorCode) {
    अगर(!checkArgs(label, length, dest, capacity, pInfo, pErrorCode)) {
        वापस 0;
    }
    UnicodeString src(length < 0, label, length);
    UnicodeString destString(dest, 0, capacity);
    IDNAInfo info;
    reinterpret_cast<स्थिर IDNA *>(idna)->labelToUnicode(src, destString, info, *pErrorCode);
    idnaInfoToStruct(info, pInfo);
    वापस destString.extract(dest, capacity, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uidna_nameToASCII(स्थिर UIDNA *idna,
                  स्थिर char16_t *name, int32_t length,
                  char16_t *dest, int32_t capacity,
                  UIDNAInfo *pInfo, UErrorCode *pErrorCode) {
    अगर(!checkArgs(name, length, dest, capacity, pInfo, pErrorCode)) {
        वापस 0;
    }
    UnicodeString src(length < 0, name, length);
    UnicodeString destString(dest, 0, capacity);
    IDNAInfo info;
    reinterpret_cast<स्थिर IDNA *>(idna)->nameToASCII(src, destString, info, *pErrorCode);
    idnaInfoToStruct(info, pInfo);
    वापस destString.extract(dest, capacity, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uidna_nameToUnicode(स्थिर UIDNA *idna,
                    स्थिर char16_t *name, int32_t length,
                    char16_t *dest, int32_t capacity,
                    UIDNAInfo *pInfo, UErrorCode *pErrorCode) {
    अगर(!checkArgs(name, length, dest, capacity, pInfo, pErrorCode)) {
        वापस 0;
    }
    UnicodeString src(length < 0, name, length);
    UnicodeString destString(dest, 0, capacity);
    IDNAInfo info;
    reinterpret_cast<स्थिर IDNA *>(idna)->nameToUnicode(src, destString, info, *pErrorCode);
    idnaInfoToStruct(info, pInfo);
    वापस destString.extract(dest, capacity, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uidna_labelToASCII_UTF8(स्थिर UIDNA *idna,
                        स्थिर अक्षर *label, int32_t length,
                        अक्षर *dest, int32_t capacity,
                        UIDNAInfo *pInfo, UErrorCode *pErrorCode) {
    अगर(!checkArgs(label, length, dest, capacity, pInfo, pErrorCode)) {
        वापस 0;
    }
    StringPiece src(label, length<0 ? static_cast<int32_t>(uprv_strlen(label)) : length);
    वापस ByteSinkUtil::viaByteSinkToTerminatedChars(
        dest, capacity,
        [&](ByteSink& sink, UErrorCode& status) {
            IDNAInfo info;
            reinterpret_cast<स्थिर IDNA *>(idna)->labelToASCII_UTF8(src, sink, info, status);
            idnaInfoToStruct(info, pInfo);
        },
        *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uidna_labelToUnicodeUTF8(स्थिर UIDNA *idna,
                         स्थिर अक्षर *label, int32_t length,
                         अक्षर *dest, int32_t capacity,
                         UIDNAInfo *pInfo, UErrorCode *pErrorCode) {
    अगर(!checkArgs(label, length, dest, capacity, pInfo, pErrorCode)) {
        वापस 0;
    }
    StringPiece src(label, length<0 ? static_cast<int32_t>(uprv_strlen(label)) : length);
    वापस ByteSinkUtil::viaByteSinkToTerminatedChars(
        dest, capacity,
        [&](ByteSink& sink, UErrorCode& status) {
            IDNAInfo info;
            reinterpret_cast<स्थिर IDNA *>(idna)->labelToUnicodeUTF8(src, sink, info, status);
            idnaInfoToStruct(info, pInfo);
        },
        *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uidna_nameToASCII_UTF8(स्थिर UIDNA *idna,
                       स्थिर अक्षर *name, int32_t length,
                       अक्षर *dest, int32_t capacity,
                       UIDNAInfo *pInfo, UErrorCode *pErrorCode) {
    अगर(!checkArgs(name, length, dest, capacity, pInfo, pErrorCode)) {
        वापस 0;
    }
    StringPiece src(name, length<0 ? static_cast<int32_t>(uprv_strlen(name)) : length);
    वापस ByteSinkUtil::viaByteSinkToTerminatedChars(
        dest, capacity,
        [&](ByteSink& sink, UErrorCode& status) {
            IDNAInfo info;
            reinterpret_cast<स्थिर IDNA *>(idna)->nameToASCII_UTF8(src, sink, info, status);
            idnaInfoToStruct(info, pInfo);
        },
        *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
uidna_nameToUnicodeUTF8(स्थिर UIDNA *idna,
                        स्थिर अक्षर *name, int32_t length,
                        अक्षर *dest, int32_t capacity,
                        UIDNAInfo *pInfo, UErrorCode *pErrorCode) {
    अगर(!checkArgs(name, length, dest, capacity, pInfo, pErrorCode)) {
        वापस 0;
    }
    StringPiece src(name, length<0 ? static_cast<int32_t>(uprv_strlen(name)) : length);
    वापस ByteSinkUtil::viaByteSinkToTerminatedChars(
        dest, capacity,
        [&](ByteSink& sink, UErrorCode& status) {
            IDNAInfo info;
            reinterpret_cast<स्थिर IDNA *>(idna)->nameToUnicodeUTF8(src, sink, info, status);
            idnaInfoToStruct(info, pInfo);
        },
        *pErrorCode);
}

#पूर्ण_अगर  // UCONFIG_NO_IDNA
