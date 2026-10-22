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

#إذا !UCONFIG_NO_IDNA

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
    ثابت char16_t *s=dest.get_بفر();
    ثابت char16_t *حد=s+dest.length();
    طالما(s<حد) {
        إذا(*s++>0x7f) {
            أرجع خطأ;
        }
    }
    أرجع ریښتیا;
}

ساكن UBool
isASCIIOkBiDi(ثابت char16_t *s, int32_t length);

ساكن UBool
isASCIIOkBiDi(ثابت حرف *s, int32_t length);

// IDNA class default implementations -------------------------------------- ***

IDNA::~IDNA() {}

فراغ
IDNA::label_منزل_ASCII_UTF8(StringPiece label, ByteSink &dest,
                        IDNAInfo &info, UError_کوډ &error_کوډ) ثابت {
    إذا(U_SUCCESS(error_کوډ)) {
        UnicodeString destString;
        label_منزل_ASCII(UnicodeString::fromUTF8(label), destString,
                     info, error_کوډ).منزل_UTF8(dest);
    }
}

فراغ
IDNA::label_منزل_Unicode_UTF8(StringPiece label, ByteSink &dest,
                         IDNAInfo &info, UError_کوډ &error_کوډ) ثابت {
    إذا(U_SUCCESS(error_کوډ)) {
        UnicodeString destString;
        label_منزل_Unicode(UnicodeString::fromUTF8(label), destString,
                       info, error_کوډ).منزل_UTF8(dest);
    }
}

فراغ
IDNA::name_منزل_ASCII_UTF8(StringPiece name, ByteSink &dest,
                       IDNAInfo &info, UError_کوډ &error_کوډ) ثابت {
    إذا(U_SUCCESS(error_کوډ)) {
        UnicodeString destString;
        name_منزل_ASCII(UnicodeString::fromUTF8(name), destString,
                    info, error_کوډ).منزل_UTF8(dest);
    }
}

فراغ
IDNA::name_منزل_Unicode_UTF8(StringPiece name, ByteSink &dest,
                        IDNAInfo &info, UError_کوډ &error_کوډ) ثابت {
    إذا(U_SUCCESS(error_کوډ)) {
        UnicodeString destString;
        name_منزل_Unicode(UnicodeString::fromUTF8(name), destString,
                      info, error_کوډ).منزل_UTF8(dest);
    }
}

// UTS46 class declaration ------------------------------------------------- ***

صنف UTS46 : عام IDNA {
عام:
    UTS46(uint32_t options, UError_کوډ &error_کوډ);
    مجازي ~UTS46();

    مجازي UnicodeString&
    label_منزل_ASCII(ثابت UnicodeString& label,
                 UnicodeString& dest U_LIFETIME_BOUND,
                 IDNAInfo& info,
                 UError_کوډ& error_کوډ) ثابت override;

    مجازي UnicodeString&
    label_منزل_Unicode(ثابت UnicodeString& label,
                   UnicodeString& dest U_LIFETIME_BOUND,
                   IDNAInfo& info,
                   UError_کوډ& error_کوډ) ثابت override;

    مجازي UnicodeString&
    name_منزل_ASCII(ثابت UnicodeString& name,
                UnicodeString& dest U_LIFETIME_BOUND,
                IDNAInfo& info,
                UError_کوډ& error_کوډ) ثابت override;

    مجازي UnicodeString&
    name_منزل_Unicode(ثابت UnicodeString& name,
                  UnicodeString& dest U_LIFETIME_BOUND,
                  IDNAInfo& info,
                  UError_کوډ& error_کوډ) ثابت override;

    مجازي فراغ
    label_منزل_ASCII_UTF8(StringPiece label, ByteSink &dest,
                      IDNAInfo &info, UError_کوډ &error_کوډ) ثابت override;

    مجازي فراغ
    label_منزل_Unicode_UTF8(StringPiece label, ByteSink &dest,
                       IDNAInfo &info, UError_کوډ &error_کوډ) ثابت override;

    مجازي فراغ
    name_منزل_ASCII_UTF8(StringPiece name, ByteSink &dest,
                     IDNAInfo &info, UError_کوډ &error_کوډ) ثابت override;

    مجازي فراغ
    name_منزل_Unicode_UTF8(StringPiece name, ByteSink &dest,
                      IDNAInfo &info, UError_کوډ &error_کوډ) ثابت override;

خاص:
    UnicodeString&
    process(ثابت UnicodeString& src,
            UBool isLabel,
            UBool منزل_ASCII,
            UnicodeString& dest U_LIFETIME_BOUND,
            IDNAInfo& info,
            UError_کوډ& error_کوډ) ثابت;

    فراغ
    processUTF8(StringPiece src,
                UBool isLabel, UBool منزل_ASCII,
                ByteSink &dest,
                IDNAInfo &info, UError_کوډ &error_کوډ) ثابت;

    UnicodeString&
    processUnicode(ثابت UnicodeString& src,
                   int32_t label_پیل,
                   int32_t mapping_پیل,
                   UBool isLabel,
                   UBool منزل_ASCII,
                   UnicodeString& dest U_LIFETIME_BOUND,
                   IDNAInfo& info,
                   UError_کوډ& error_کوډ) ثابت;

    // returns the new dest.length()
    int32_t
    mapDevChars(UnicodeString &dest, int32_t label_پیل, int32_t mapping_پیل,
                UError_کوډ &error_کوډ) ثابت;

    // returns the new label length
    int32_t
    processLabel(UnicodeString &dest,
                 int32_t label_پیل, int32_t label_اوږدتیا,
                 UBool منزل_ASCII,
                 IDNAInfo &info, UError_کوډ &error_کوډ) ثابت;
    int32_t
    markBadACELabel(UnicodeString &dest,
                    int32_t label_پیل, int32_t label_اوږدتیا,
                    UBool منزل_ASCII, IDNAInfo &info, UError_کوډ &error_کوډ) ثابت;

    فراغ
    checkLabelBiDi(ثابت char16_t *label, int32_t label_اوږدتیا, IDNAInfo &info) ثابت;

    UBool
    isLabelOkContextJ(ثابت char16_t *label, int32_t label_اوږدتیا) ثابت;

    فراغ
    checkLabelContextO(ثابت char16_t *label, int32_t label_اوږدتیا, IDNAInfo &info) ثابت;

    ثابت Normalizer2 &uts46Norm2;  // uts46.nrm
    uint32_t options;
};

IDNA *
IDNA::createUTS46Instance(uint32_t options, UError_کوډ &error_کوډ) {
    إذا(U_SUCCESS(error_کوډ)) {
        IDNA *idna=جديد UTS46(options, error_کوډ);
        إذا(idna==تش_اشاره) {
            error_کوډ=U_حافظه_ویش_ERROR;
        } وإلا إذا(U_FAILURE(error_کوډ)) {
            ړنګول idna;
            idna=تش_اشاره;
        }
        أرجع idna;
    } وإلا {
        أرجع تش_اشاره;
    }
}

// UTS46 implementation ---------------------------------------------------- ***

UTS46::UTS46(uint32_t opt, UError_کوډ &error_کوډ)
        : uts46Norm2(*Normalizer2::getInstance(تش_اشاره, "uts46", UNORM2_COMPOSE, error_کوډ)),
          options(opt) {}

UTS46::~UTS46() {}

UnicodeString&
UTS46::label_منزل_ASCII(ثابت UnicodeString& label,
                    UnicodeString& dest U_LIFETIME_BOUND,
                    IDNAInfo& info,
                    UError_کوډ& error_کوډ) ثابت {
    أرجع process(label, ریښتیا, ریښتیا, dest, info, error_کوډ);
}

UnicodeString&
UTS46::label_منزل_Unicode(ثابت UnicodeString& label,
                      UnicodeString& dest U_LIFETIME_BOUND,
                      IDNAInfo& info,
                      UError_کوډ& error_کوډ) ثابت {
    أرجع process(label, ریښتیا, خطأ, dest, info, error_کوډ);
}

UnicodeString&
UTS46::name_منزل_ASCII(ثابت UnicodeString& name,
                   UnicodeString& dest U_LIFETIME_BOUND,
                   IDNAInfo& info,
                   UError_کوډ& error_کوډ) ثابت {
    process(name, خطأ, ریښتیا, dest, info, error_کوډ);
    إذا( dest.length()>=254 && (info.errors&UIDNA_ERROR_DOMAIN_NAME_TOO_LONG)==0 &&
        isASCIIString(dest) &&
        (dest.length()>254 || dest[253]!=0x2e)
    ) {
        info.errors|=UIDNA_ERROR_DOMAIN_NAME_TOO_LONG;
    }
    أرجع dest;
}

UnicodeString&
UTS46::name_منزل_Unicode(ثابت UnicodeString& name,
                     UnicodeString& dest U_LIFETIME_BOUND,
                     IDNAInfo& info,
                     UError_کوډ& error_کوډ) ثابت {
    أرجع process(name, خطأ, خطأ, dest, info, error_کوډ);
}

فراغ
UTS46::label_منزل_ASCII_UTF8(StringPiece label, ByteSink &dest,
                         IDNAInfo &info, UError_کوډ &error_کوډ) ثابت {
    processUTF8(label, ریښتیا, ریښتیا, dest, info, error_کوډ);
}

فراغ
UTS46::label_منزل_Unicode_UTF8(StringPiece label, ByteSink &dest,
                          IDNAInfo &info, UError_کوډ &error_کوډ) ثابت {
    processUTF8(label, ریښتیا, خطأ, dest, info, error_کوډ);
}

فراغ
UTS46::name_منزل_ASCII_UTF8(StringPiece name, ByteSink &dest,
                        IDNAInfo &info, UError_کوډ &error_کوډ) ثابت {
    processUTF8(name, خطأ, ریښتیا, dest, info, error_کوډ);
}

فراغ
UTS46::name_منزل_Unicode_UTF8(StringPiece name, ByteSink &dest,
                         IDNAInfo &info, UError_کوډ &error_کوډ) ثابت {
    processUTF8(name, خطأ, خطأ, dest, info, error_کوډ);
}

// UTS #46 data for ASCII characters.
// The normalizer (using uts46.nrm) maps uppercase ASCII letters to lowercase
// and passes through all other ASCII characters.
// If UIDNA_USE_STD3_RULES is set, then non-LDH characters are disallowed
// using this data.
// The ASCII fastpath also uses this data.
// Values: -1=disallowed  0==valid  1==mapped (lowercase)
ساكن ثابت int8_t ascii_معلومات[128]={
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
               UBool منزل_ASCII,
               UnicodeString& dest U_LIFETIME_BOUND,
               IDNAInfo& info,
               UError_کوډ& error_کوډ) ثابت {
    // uts46Norm2.normalize() would do all of this error checking and setup,
    // but with the ASCII fastpath we do not always call it, and do not
    // call it first.
    إذا(U_FAILURE(error_کوډ)) {
        dest.سېټ_منزل_Bogus();
        أرجع dest;
    }
    ثابت char16_t *srcArray=src.get_بفر();
    إذا(&dest==&src || srcArray==تش_اشاره) {
        error_کوډ=U_ILLEGAL_دلیل_ERROR;
        dest.سېټ_منزل_Bogus();
        أرجع dest;
    }
    // Arguments are fine, reset output values.
    dest.remove();
    info.reset();
    int32_t src_اوږدتیا=src.length();
    إذا(src_اوږدتیا==0) {
        info.errors|=UIDNA_ERROR_تشه_LABEL;
        أرجع dest;
    }
    char16_t *destArray=dest.get_بفر(src_اوږدتیا);
    إذا(destArray==تش_اشاره) {
        error_کوډ=U_حافظه_ویش_ERROR;
        أرجع dest;
    }
    // ASCII fastpath
    UBool disallowNonLDHDot=(options&UIDNA_USE_STD3_قواعد)!=0;
    int32_t label_پیل=0;
    int32_t i;
    لكل(i=0;; ++i) {
        إذا(i==src_اوږدتیا) {
            إذا(منزل_ASCII) {
                إذا((i-label_پیل)>63) {
                    info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
                }
                // There is a trailing dot if labelStart==i.
                إذا(!isLabel && i>=254 && (i>254 || label_پیل<i)) {
                    info.errors|=UIDNA_ERROR_DOMAIN_NAME_TOO_LONG;
                }
            }
            info.errors|=info.labelErrors;
            dest.release_بفر(i);
            أرجع dest;
        }
        char16_t c=srcArray[i];
        إذا(c>0x7f) {
            اكسر;
        }
        صحيح c_معلومات=ascii_معلومات[c];
        إذا(c_معلومات>0) {
            destArray[i]=c+0x20;  // Lowercase an uppercase ASCII letter.
        } وإلا إذا(c_معلومات<0 && disallowNonLDHDot) {
            اكسر;  // Replacing with U+FFFD can be complicated for toASCII.
        } وإلا {
            destArray[i]=c;
            إذا(c==0x2d) {  // hyphen
                إذا(i==(label_پیل+3) && srcArray[i-1]==0x2d) {
                    // "??--..." is Punycode or forbidden.
                    ++i;  // '-' was copied to dest already
                    اكسر;
                }
                إذا(i==label_پیل) {
                    // label starts with "-"
                    info.labelErrors|=UIDNA_ERROR_LEADING_HYPHEN;
                }
                إذا((i+1)==src_اوږدتیا || srcArray[i+1]==0x2e) {
                    // label ends with "-"
                    info.labelErrors|=UIDNA_ERROR_TRAILING_HYPHEN;
                }
            } وإلا إذا(c==0x2e) {  // dot
                إذا(isLabel) {
                    // Replacing with U+FFFD can be complicated for toASCII.
                    ++i;  // '.' was copied to dest already
                    اكسر;
                }
                إذا(i==label_پیل) {
                    info.labelErrors|=UIDNA_ERROR_تشه_LABEL;
                }
                إذا(منزل_ASCII && (i-label_پیل)>63) {
                    info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
                }
                info.errors|=info.labelErrors;
                info.labelErrors=0;
                label_پیل=i+1;
            }
        }
    }
    info.errors|=info.labelErrors;
    dest.release_بفر(i);
    processUnicode(src, label_پیل, i, isLabel, منزل_ASCII, dest, info, error_کوډ);
    إذا( info.isBiDi && U_SUCCESS(error_کوډ) && (info.errors&severeErrors)==0 &&
        (!info.isOkBiDi || (label_پیل>0 && !isASCIIOkBiDi(dest.get_بفر(), label_پیل)))
    ) {
        info.errors|=UIDNA_ERROR_BIDI;
    }
    أرجع dest;
}

فراغ
UTS46::processUTF8(StringPiece src,
                   UBool isLabel, UBool منزل_ASCII,
                   ByteSink &dest,
                   IDNAInfo &info, UError_کوډ &error_کوډ) ثابت {
    إذا(U_FAILURE(error_کوډ)) {
        أرجع;
    }
    ثابت حرف *srcArray=src.معلومات();
    int32_t src_اوږدتیا=src.length();
    إذا(srcArray==تش_اشاره && src_اوږدتیا!=0) {
        error_کوډ=U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }
    // Arguments are fine, reset output values.
    info.reset();
    إذا(src_اوږدتیا==0) {
        info.errors|=UIDNA_ERROR_تشه_LABEL;
        dest.Flush();
        أرجع;
    }
    UnicodeString destString;
    int32_t label_پیل=0;
    إذا(src_اوږدتیا<=256) {  // length of stackArray[]
        // ASCII fastpath
        حرف ډېری_Array[256];
        int32_t dest_ظرفیت;
        حرف *destArray=dest.Get_Append_بفر(src_اوږدتیا, src_اوږدتیا+20,
                                             ډېری_Array, UPRV_LENGTHOF(ډېری_Array), &dest_ظرفیت);
        UBool disallowNonLDHDot=(options&UIDNA_USE_STD3_قواعد)!=0;
        int32_t i;
        لكل(i=0;; ++i) {
            إذا(i==src_اوږدتیا) {
                إذا(منزل_ASCII) {
                    إذا((i-label_پیل)>63) {
                        info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
                    }
                    // There is a trailing dot if labelStart==i.
                    إذا(!isLabel && i>=254 && (i>254 || label_پیل<i)) {
                        info.errors|=UIDNA_ERROR_DOMAIN_NAME_TOO_LONG;
                    }
                }
                info.errors|=info.labelErrors;
                dest.Append(destArray, i);
                dest.Flush();
                أرجع;
            }
            حرف c=srcArray[i];
            إذا (static_cast<int8_t>(c) < 0) { // (uint8_t)c>0x7f
                اكسر;
            }
            صحيح c_معلومات = ascii_معلومات[static_cast<صحيح>(c)]; // Cast: gcc warns about indexing with a char.
            إذا(c_معلومات>0) {
                destArray[i]=c+0x20;  // Lowercase an uppercase ASCII letter.
            } وإلا إذا(c_معلومات<0 && disallowNonLDHDot) {
                اكسر;  // Replacing with U+FFFD can be complicated for toASCII.
            } وإلا {
                destArray[i]=c;
                إذا(c==0x2d) {  // hyphen
                    إذا(i==(label_پیل+3) && srcArray[i-1]==0x2d) {
                        // "??--..." is Punycode or forbidden.
                        اكسر;
                    }
                    إذا(i==label_پیل) {
                        // label starts with "-"
                        info.labelErrors|=UIDNA_ERROR_LEADING_HYPHEN;
                    }
                    إذا((i+1)==src_اوږدتیا || srcArray[i+1]==0x2e) {
                        // label ends with "-"
                        info.labelErrors|=UIDNA_ERROR_TRAILING_HYPHEN;
                    }
                } وإلا إذا(c==0x2e) {  // dot
                    إذا(isLabel) {
                        اكسر;  // Replacing with U+FFFD can be complicated for toASCII.
                    }
                    إذا(i==label_پیل) {
                        info.labelErrors|=UIDNA_ERROR_تشه_LABEL;
                    }
                    إذا(منزل_ASCII && (i-label_پیل)>63) {
                        info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
                    }
                    info.errors|=info.labelErrors;
                    info.labelErrors=0;
                    label_پیل=i+1;
                }
            }
        }
        info.errors|=info.labelErrors;
        // Convert the processed ASCII prefix of the current label to UTF-16.
        int32_t mapping_پیل=i-label_پیل;
        destString=UnicodeString::fromUTF8(StringPiece(destArray+label_پیل, mapping_پیل));
        // Output the previous ASCII labels and process the rest of src in UTF-16.
        dest.Append(destArray, label_پیل);
        processUnicode(UnicodeString::fromUTF8(StringPiece(src, label_پیل)), 0, mapping_پیل,
                       isLabel, منزل_ASCII,
                       destString, info, error_کوډ);
    } وإلا {
        // src is too long for the ASCII fastpath implementation.
        processUnicode(UnicodeString::fromUTF8(src), 0, 0,
                       isLabel, منزل_ASCII,
                       destString, info, error_کوډ);
    }
    destString.منزل_UTF8(dest);  // calls dest.Flush()
    إذا(منزل_ASCII && !isLabel) {
        // length==labelStart==254 means that there is a trailing dot (ok) and
        // destString is empty (do not index at 253-labelStart).
        int32_t length=label_پیل+destString.length();
        إذا( length>=254 && isASCIIString(destString) &&
            (length>254 ||
             (label_پیل<254 && destString[253-label_پیل]!=0x2e))
        ) {
            info.errors|=UIDNA_ERROR_DOMAIN_NAME_TOO_LONG;
        }
    }
    إذا( info.isBiDi && U_SUCCESS(error_کوډ) && (info.errors&severeErrors)==0 &&
        (!info.isOkBiDi || (label_پیل>0 && !isASCIIOkBiDi(srcArray, label_پیل)))
    ) {
        info.errors|=UIDNA_ERROR_BIDI;
    }
}

UnicodeString&
UTS46::processUnicode(ثابت UnicodeString& src,
                      int32_t label_پیل,
                      int32_t mapping_پیل,
                      UBool isLabel,
                      UBool منزل_ASCII,
                      UnicodeString& dest U_LIFETIME_BOUND,
                      IDNAInfo& info,
                      UError_کوډ& error_کوډ) ثابت {
    إذا(mapping_پیل==0) {
        uts46Norm2.normalize(src, dest, error_کوډ);
    } وإلا {
        uts46Norm2.normalize_دوهم_And_Append(dest, src.لنډمهاله_Sub_String(mapping_پیل), error_کوډ);
    }
    إذا(U_FAILURE(error_کوډ)) {
        أرجع dest;
    }
    UBool doMapDevChars=
        منزل_ASCII ? (options&UIDNA_NONTRANSITIONAL_منزل_ASCII)==0 :
                  (options&UIDNA_NONTRANSITIONAL_منزل_UNICODE)==0;
    ثابت char16_t *destArray=dest.get_بفر();
    int32_t dest_اوږدتیا=dest.length();
    int32_t label_حد=label_پیل;
    طالما(label_حد<dest_اوږدتیا) {
        char16_t c=destArray[label_حد];
        إذا(c==0x2e && !isLabel) {
            int32_t label_اوږدتیا=label_حد-label_پیل;
            int32_t new_اوږدتیا=processLabel(dest, label_پیل, label_اوږدتیا,
                                            منزل_ASCII, info, error_کوډ);
            info.errors|=info.labelErrors;
            info.labelErrors=0;
            إذا(U_FAILURE(error_کوډ)) {
                أرجع dest;
            }
            destArray=dest.get_بفر();
            dest_اوږدتیا+=new_اوږدتیا-label_اوږدتیا;
            label_حد=label_پیل+=new_اوږدتیا+1;
            استمر;
        } وإلا إذا(c<0xdf) {
            // pass
        } وإلا إذا(c<=0x200d && (c==0xdf || c==0x3c2 || c>=0x200c)) {
            info.isTransDiff=ریښتیا;
            إذا(doMapDevChars) {
                dest_اوږدتیا=mapDevChars(dest, label_پیل, label_حد, error_کوډ);
                إذا(U_FAILURE(error_کوډ)) {
                    أرجع dest;
                }
                destArray=dest.get_بفر();
                // All deviation characters have been mapped, no need to check for them again.
                doMapDevChars=خطأ;
                // Do not increment labelLimit in case c was removed.
                استمر;
            }
        } وإلا إذا(U16_IS_SURROGATE(c)) {
            إذا(U16_IS_SURROGATE_LEAD(c) ?
                    (label_حد+1)==dest_اوږدتیا || !U16_IS_TRAIL(destArray[label_حد+1]) :
                    label_حد==label_پیل || !U16_IS_LEAD(destArray[label_حد-1])) {
                // Map an unpaired surrogate to U+FFFD before normalization so that when
                // that removes characters we do not turn two unpaired ones into a pair.
                info.labelErrors|=UIDNA_ERROR_DISALLOWED;
                dest.سېټ_Char_At(label_حد, 0xfffd);
                destArray=dest.get_بفر();
            }
        }
        ++label_حد;
    }
    // Permit an empty label at the end (0<labelStart==labelLimit==destLength is ok)
    // but not an empty label elsewhere nor a completely empty domain name.
    // processLabel() sets UIDNA_ERROR_EMPTY_LABEL when labelLength==0.
    إذا(0==label_پیل || label_پیل<label_حد) {
        processLabel(dest, label_پیل, label_حد-label_پیل,
                      منزل_ASCII, info, error_کوډ);
        info.errors|=info.labelErrors;
    }
    أرجع dest;
}

int32_t
UTS46::mapDevChars(UnicodeString &dest, int32_t label_پیل, int32_t mapping_پیل,
                   UError_کوډ &error_کوډ) ثابت {
    إذا(U_FAILURE(error_کوډ)) {
        أرجع 0;
    }
    int32_t length=dest.length();
    char16_t *s=dest.get_بفر(dest[mapping_پیل]==0xdf ? length+1 : length);
    إذا(s==تش_اشاره) {
        error_کوډ=U_حافظه_ویش_ERROR;
        أرجع length;
    }
    int32_t capacity=dest.get_ظرفیت();
    UBool didMapDevChars=خطأ;
    int32_t read_شاخص=mapping_پیل, write_شاخص=mapping_پیل;
    افعل {
        char16_t c=s[read_شاخص++];
        بدل(c) {
        حالة 0xdf:
            // Map sharp s to ss.
            didMapDevChars=ریښتیا;
            s[write_شاخص++]=0x73;  // Replace sharp s with first s.
            // Insert second s and account for possible buffer reallocation.
            إذا(write_شاخص==read_شاخص) {
                إذا(length==capacity) {
                    dest.release_بفر(length);
                    s=dest.get_بفر(length+1);
                    إذا(s==تش_اشاره) {
                        error_کوډ=U_حافظه_ویش_ERROR;
                        أرجع length;
                    }
                    capacity=dest.get_ظرفیت();
                }
                u_memmove(s+write_شاخص+1, s+write_شاخص, length-write_شاخص);
                ++read_شاخص;
            }
            s[write_شاخص++]=0x73;
            ++length;
            اكسر;
        حالة 0x3c2:  // Map final sigma to nonfinal sigma.
            didMapDevChars=ریښتیا;
            s[write_شاخص++]=0x3c3;
            اكسر;
        حالة 0x200c:  // Ignore/remove ZWNJ.
        حالة 0x200d:  // Ignore/remove ZWJ.
            didMapDevChars=ریښتیا;
            --length;
            اكسر;
        افتراضي:
            // Only really necessary if writeIndex was different from readIndex.
            s[write_شاخص++]=c;
            اكسر;
        }
    } طالما(write_شاخص<length);
    dest.release_بفر(length);
    إذا(didMapDevChars) {
        // Mapping deviation characters might have resulted in an un-NFC string.
        // We could use either the NFC or the UTS #46 normalizer.
        // By using the UTS #46 normalizer again, we avoid having to load a second .nrm data file.
        UnicodeString normalized;
        uts46Norm2.normalize(dest.لنډمهاله_Sub_String(label_پیل), normalized, error_کوډ);
        إذا(U_SUCCESS(error_کوډ)) {
            dest.replace(label_پیل, 0x7fffffff, normalized);
            إذا(dest.isBogus()) {
                error_کوډ=U_حافظه_ویش_ERROR;
            }
            أرجع dest.length();
        }
    }
    أرجع length;
}

// Replace the label in dest with the label string, if the label was modified.
// If &label==&dest then the label was modified in-place and labelLength
// is the new label length, different from label.length().
// If &label!=&dest then labelLength==label.length().
// Returns labelLength (= the new label length).
ساكن int32_t
replaceLabel(UnicodeString &dest, int32_t dest_Label_پیل, int32_t dest_Label_اوږدتیا,
             ثابت UnicodeString &label, int32_t label_اوږدتیا, UError_کوډ &error_کوډ) {
    إذا(U_FAILURE(error_کوډ)) {
        أرجع 0;
    }
    إذا(&label!=&dest) {
        dest.replace(dest_Label_پیل, dest_Label_اوږدتیا, label);
        إذا(dest.isBogus()) {
            error_کوډ=U_حافظه_ویش_ERROR;
            أرجع 0;
        }
    }
    أرجع label_اوږدتیا;
}

int32_t
UTS46::processLabel(UnicodeString &dest,
                    int32_t label_پیل, int32_t label_اوږدتیا,
                    UBool منزل_ASCII,
                    IDNAInfo &info, UError_کوډ &error_کوډ) ثابت {
    إذا(U_FAILURE(error_کوډ)) {
        أرجع 0;
    }
    UnicodeString fromPunycode;
    UnicodeString *labelString;
    ثابت char16_t *label=dest.get_بفر()+label_پیل;
    int32_t dest_Label_پیل=label_پیل;
    int32_t dest_Label_اوږدتیا=label_اوږدتیا;
    UBool wasPunycode;
    إذا(label_اوږدتیا>=4 && label[0]==0x78 && label[1]==0x6e && label[2]==0x2d && label[3]==0x2d) {
        // Label starts with "xn--", try to un-Punycode it.
        // In IDNA2008, labels like "xn--" (decodes to an empty string) and
        // "xn--ASCII-" (decodes to just "ASCII") fail the round-trip validation from
        // comparing the ToUnicode input with the back-to-ToASCII output.
        // They are alternate encodings of the respective ASCII labels.
        // Ignore "xn---" here: It will fail Punycode.decode() which logically comes before
        // the round-trip verification.
        إذا(label_اوږدتیا==4 || (label_اوږدتیا>5 && label[label_اوږدتیا-1]==u'-')) {
            info.labelErrors|=UIDNA_ERROR_INVALID_ACE_LABEL;
            أرجع markBadACELabel(dest, label_پیل, label_اوږدتیا, منزل_ASCII, info, error_کوډ);
        }
        wasPunycode=ریښتیا;
        char16_t *unicode_بفر=fromPunycode.get_بفر(-1);  // capacity==-1: most labels should fit
        إذا(unicode_بفر==تش_اشاره) {
            // Should never occur if we used capacity==-1 which uses the internal buffer.
            error_کوډ=U_حافظه_ویش_ERROR;
            أرجع label_اوږدتیا;
        }
        UError_کوډ punycode_Error_کوډ=U_ZERO_ERROR;
        int32_t unicode_اوږدتیا=u_strFromPunycode(label+4, label_اوږدتیا-4,
                                                unicode_بفر, fromPunycode.get_ظرفیت(),
                                                تش_اشاره, &punycode_Error_کوډ);
        إذا(punycode_Error_کوډ==U_بفر_ډکېدنه_ERROR) {
            fromPunycode.release_بفر(0);
            unicode_بفر=fromPunycode.get_بفر(unicode_اوږدتیا);
            إذا(unicode_بفر==تش_اشاره) {
                error_کوډ=U_حافظه_ویش_ERROR;
                أرجع label_اوږدتیا;
            }
            punycode_Error_کوډ=U_ZERO_ERROR;
            unicode_اوږدتیا=u_strFromPunycode(label+4, label_اوږدتیا-4,
                                            unicode_بفر, fromPunycode.get_ظرفیت(),
                                            تش_اشاره, &punycode_Error_کوډ);
        }
        fromPunycode.release_بفر(unicode_اوږدتیا);
        إذا(U_FAILURE(punycode_Error_کوډ)) {
            info.labelErrors|=UIDNA_ERROR_PUNYCODE;
            أرجع markBadACELabel(dest, label_پیل, label_اوږدتیا, منزل_ASCII, info, error_کوډ);
        }
        // Check for NFC, and for characters that are not
        // valid or deviation characters according to the normalizer.
        // If there is something wrong, then the string will change.
        // Note that the normalizer passes through non-LDH ASCII and deviation characters.
        // Deviation characters are ok in Punycode even in transitional processing.
        // In the code further below, if we find non-LDH ASCII and we have UIDNA_USE_STD3_RULES
        // then we will set UIDNA_ERROR_INVALID_ACE_LABEL there too.
        UBool isValid=uts46Norm2.isNormalized(fromPunycode, error_کوډ);
        إذا(U_FAILURE(error_کوډ)) {
            أرجع label_اوږدتیا;
        }
        // Unicode 15.1 UTS #46:
        // Added an additional condition in 4.1 Validity Criteria to
        // disallow labels such as xn--xn---epa., which do not round-trip.
        // --> Validity Criteria new criterion 4:
        // If not CheckHyphens, the label must not begin with “xn--”.
        إذا(!isValid || fromPunycode.startsWith(UnicodeString::readOnlyAlias(u"xn--"))) {
            info.labelErrors|=UIDNA_ERROR_INVALID_ACE_LABEL;
            أرجع markBadACELabel(dest, label_پیل, label_اوږدتیا, منزل_ASCII, info, error_کوډ);
        }
        labelString=&fromPunycode;
        label=fromPunycode.get_بفر();
        label_پیل=0;
        label_اوږدتیا=fromPunycode.length();
    } وإلا {
        wasPunycode=خطأ;
        labelString=&dest;
    }
    // Validity check
    إذا(label_اوږدتیا==0) {
        info.labelErrors|=UIDNA_ERROR_تشه_LABEL;
        أرجع replaceLabel(dest, dest_Label_پیل, dest_Label_اوږدتیا,
                            *labelString, label_اوږدتیا, error_کوډ);
    }
    // labelLength>0
    إذا(label_اوږدتیا>=4 && label[2]==0x2d && label[3]==0x2d) {
        // label starts with "??--"
        info.labelErrors|=UIDNA_ERROR_HYPHEN_3_4;
    }
    إذا(label[0]==0x2d) {
        // label starts with "-"
        info.labelErrors|=UIDNA_ERROR_LEADING_HYPHEN;
    }
    إذا(label[label_اوږدتیا-1]==0x2d) {
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
    ثابت char16_t *حد=label+label_اوږدتیا;
    char16_t oredChars=0;
    // If we enforce STD3 rules, then ASCII characters other than LDH and dot are disallowed.
    UBool disallowNonLDHDot=(options&UIDNA_USE_STD3_قواعد)!=0;
    افعل {
        char16_t c=*s;
        إذا(c<=0x7f) {
            إذا(c==0x2e) {
                info.labelErrors|=UIDNA_ERROR_LABEL_HAS_DOT;
                *s=0xfffd;
            } وإلا إذا(disallowNonLDHDot && ascii_معلومات[c]<0) {
                info.labelErrors|=UIDNA_ERROR_DISALLOWED;
                *s=0xfffd;
            }
        } وإلا {
            oredChars|=c;
            إذا(c==0xfffd) {
                info.labelErrors|=UIDNA_ERROR_DISALLOWED;
            }
        }
        ++s;
    } طالما(s<حد);
    // Check for a leading combining mark after other validity checks
    // so that we don't report UIDNA_ERROR_DISALLOWED for the U+FFFD from here.
    UChar32 c;
    int32_t cp_اوږدتیا=0;
    // "Unsafe" is ok because unpaired surrogates were mapped to U+FFFD.
    U16_بل_UNSAFE(label, cp_اوږدتیا, c);
    إذا((U_GET_GC_ماسک(c)&U_GC_M_ماسک)!=0) {
        info.labelErrors|=UIDNA_ERROR_LEADING_COMBINING_MARK;
        labelString->replace(label_پیل, cp_اوږدتیا, static_cast<char16_t>(0xfffd));
        label=labelString->get_بفر()+label_پیل;
        label_اوږدتیا+=1-cp_اوږدتیا;
        إذا(labelString==&dest) {
            dest_Label_اوږدتیا=label_اوږدتیا;
        }
    }
    إذا((info.labelErrors&severeErrors)==0) {
        // Do contextual checks only if we do not have U+FFFD from a severe error
        // because U+FFFD can make these checks fail.
        إذا((options&UIDNA_CHECK_BIDI)!=0 && (!info.isBiDi || info.isOkBiDi)) {
            checkLabelBiDi(label, label_اوږدتیا, info);
        }
        إذا( (options&UIDNA_CHECK_CONTEXTJ)!=0 && (oredChars&0x200c)==0x200c &&
            !isLabelOkContextJ(label, label_اوږدتیا)
        ) {
            info.labelErrors|=UIDNA_ERROR_CONTEXTJ;
        }
        إذا((options&UIDNA_CHECK_CONTEXTO)!=0 && oredChars>=0xb7) {
            checkLabelContextO(label, label_اوږدتیا, info);
        }
        إذا(منزل_ASCII) {
            إذا(wasPunycode) {
                // Leave a Punycode label unchanged if it has no severe errors.
                إذا(dest_Label_اوږدتیا>63) {
                    info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
                }
                أرجع dest_Label_اوږدتیا;
            } وإلا إذا(oredChars>=0x80) {
                // Contains non-ASCII characters.
                UnicodeString punycode;
                char16_t *بفر=punycode.get_بفر(63);  // 63==maximum DNS label length
                إذا(بفر==تش_اشاره) {
                    error_کوډ=U_حافظه_ویش_ERROR;
                    أرجع dest_Label_اوږدتیا;
                }
                بفر[0]=0x78;  // Write "xn--".
                بفر[1]=0x6e;
                بفر[2]=0x2d;
                بفر[3]=0x2d;
                UError_کوډ punycode_Error_کوډ=U_ZERO_ERROR;
                int32_t punycode_اوږدتیا=u_str_منزل_Punycode(label, label_اوږدتیا,
                                                      بفر+4, punycode.get_ظرفیت()-4,
                                                      تش_اشاره, &punycode_Error_کوډ);
                إذا(punycode_Error_کوډ==U_بفر_ډکېدنه_ERROR) {
                    punycode_Error_کوډ=U_ZERO_ERROR;
                    punycode.release_بفر(4);
                    بفر=punycode.get_بفر(4+punycode_اوږدتیا);
                    إذا(بفر==تش_اشاره) {
                        error_کوډ=U_حافظه_ویش_ERROR;
                        أرجع dest_Label_اوږدتیا;
                    }
                    punycode_اوږدتیا=u_str_منزل_Punycode(label, label_اوږدتیا,
                                                  بفر+4, punycode.get_ظرفیت()-4,
                                                  تش_اشاره, &punycode_Error_کوډ);
                }
                punycode_اوږدتیا+=4;
                punycode.release_بفر(punycode_اوږدتیا);
                إذا(U_FAILURE(punycode_Error_کوډ)) {
                    error_کوډ = punycode_Error_کوډ;
                    أرجع dest_Label_اوږدتیا;
                }
                إذا(punycode_اوږدتیا>63) {
                    info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
                }
                أرجع replaceLabel(dest, dest_Label_پیل, dest_Label_اوږدتیا,
                                    punycode, punycode_اوږدتیا, error_کوډ);
            } وإلا {
                // all-ASCII label
                إذا(label_اوږدتیا>63) {
                    info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
                }
            }
        }
    } وإلا {
        // If a Punycode label has severe errors,
        // then leave it but make sure it does not look valid.
        إذا(wasPunycode) {
            info.labelErrors|=UIDNA_ERROR_INVALID_ACE_LABEL;
            أرجع markBadACELabel(dest, dest_Label_پیل, dest_Label_اوږدتیا, منزل_ASCII, info, error_کوډ);
        }
    }
    أرجع replaceLabel(dest, dest_Label_پیل, dest_Label_اوږدتیا,
                        *labelString, label_اوږدتیا, error_کوډ);
}

// Make sure an ACE label does not look valid.
// Append U+FFFD if the label has only LDH characters.
// If UIDNA_USE_STD3_RULES, also replace disallowed ASCII characters with U+FFFD.
int32_t
UTS46::markBadACELabel(UnicodeString &dest,
                       int32_t label_پیل, int32_t label_اوږدتیا,
                       UBool منزل_ASCII, IDNAInfo &info, UError_کوډ &error_کوډ) ثابت {
    إذا(U_FAILURE(error_کوډ)) {
        أرجع 0;
    }
    UBool disallowNonLDHDot=(options&UIDNA_USE_STD3_قواعد)!=0;
    UBool isASCII=ریښتیا;
    UBool onlyLDH=ریښتیا;
    ثابت char16_t *label=dest.get_بفر()+label_پیل;
    ثابت char16_t *حد=label+label_اوږدتیا;
    // Start after the initial "xn--".
    // Ok to cast away const because we own the UnicodeString.
    لكل(char16_t *s=const_cast<char16_t *>(label+4); s<حد; ++s) {
        char16_t c=*s;
        إذا(c<=0x7f) {
            إذا(c==0x2e) {
                info.labelErrors|=UIDNA_ERROR_LABEL_HAS_DOT;
                *s=0xfffd;
                isASCII=onlyLDH=خطأ;
            } وإلا إذا(ascii_معلومات[c]<0) {
                onlyLDH=خطأ;
                إذا(disallowNonLDHDot) {
                    *s=0xfffd;
                    isASCII=خطأ;
                }
            }
        } وإلا {
            isASCII=onlyLDH=خطأ;
        }
    }
    إذا(onlyLDH) {
        dest.insert(label_پیل + label_اوږدتیا, static_cast<char16_t>(0xfffd));
        إذا(dest.isBogus()) {
            error_کوډ=U_حافظه_ویش_ERROR;
            أرجع 0;
        }
        ++label_اوږدتیا;
    } وإلا {
        إذا(منزل_ASCII && isASCII && label_اوږدتیا>63) {
            info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
        }
    }
    أرجع label_اوږدتیا;
}

ثابت uint32_t L_ماسک=U_ماسک(U_کیڼ_منزل_ښی);
ثابت uint32_t R_AL_ماسک=U_ماسک(U_ښی_منزل_کیڼ)|U_ماسک(U_ښی_منزل_کیڼ_ARABIC);
ثابت uint32_t L_R_AL_ماسک=L_ماسک|R_AL_ماسک;

ثابت uint32_t R_AL_AN_ماسک=R_AL_ماسک|U_ماسک(U_ARABIC_شمېره);

ثابت uint32_t EN_AN_ماسک=U_ماسک(U_EUROPEAN_شمېره)|U_ماسک(U_ARABIC_شمېره);
ثابت uint32_t R_AL_EN_AN_ماسک=R_AL_ماسک|EN_AN_ماسک;
ثابت uint32_t L_EN_ماسک=L_ماسک|U_ماسک(U_EUROPEAN_شمېره);

ثابت uint32_t ES_CS_ET_ON_BN_NSM_ماسک=
    U_ماسک(U_EUROPEAN_شمېره_SEPARATOR)|
    U_ماسک(U_COMMON_شمېره_SEPARATOR)|
    U_ماسک(U_EUROPEAN_شمېره_TERMINATOR)|
    U_ماسک(U_OTHER_NEUTRAL)|
    U_ماسک(U_BOUNDARY_NEUTRAL)|
    U_ماسک(U_DIR_NON_SPACING_MARK);
ثابت uint32_t L_EN_ES_CS_ET_ON_BN_NSM_ماسک=L_EN_ماسک|ES_CS_ET_ON_BN_NSM_ماسک;
ثابت uint32_t R_AL_AN_EN_ES_CS_ET_ON_BN_NSM_ماسک=R_AL_ماسک|EN_AN_ماسک|ES_CS_ET_ON_BN_NSM_ماسک;

// We scan the whole label and check both for whether it contains RTL characters
// and whether it passes the BiDi Rule.
// In a BiDi domain name, all labels must pass the BiDi Rule, but we might find
// that a domain name is a BiDi domain name (has an RTL label) only after
// processing several earlier labels.
فراغ
UTS46::checkLabelBiDi(ثابت char16_t *label, int32_t label_اوږدتیا, IDNAInfo &info) ثابت {
    // IDNA2008 BiDi rule
    // Get the directionality of the first character.
    UChar32 c;
    int32_t i=0;
    U16_بل_UNSAFE(label, i, c);
    uint32_t لومړی_ماسک=U_ماسک(u_charDirection(c));
    // 1. The first character must be a character with BIDI property L, R
    // or AL.  If it has the R or AL property, it is an RTL label; if it
    // has the L property, it is an LTR label.
    إذا((لومړی_ماسک&~L_R_AL_ماسک)!=0) {
        info.isOkBiDi=خطأ;
    }
    // Get the directionality of the last non-NSM character.
    uint32_t وروستی_ماسک;
    لكل(;;) {
        إذا(i>=label_اوږدتیا) {
            وروستی_ماسک=لومړی_ماسک;
            اكسر;
        }
        U16_PREV_UNSAFE(label, label_اوږدتیا, c);
        UCharDirection dir=u_charDirection(c);
        إذا(dir!=U_DIR_NON_SPACING_MARK) {
            وروستی_ماسک=U_ماسک(dir);
            اكسر;
        }
    }
    // 3. In an RTL label, the end of the label must be a character with
    // BIDI property R, AL, EN or AN, followed by zero or more
    // characters with BIDI property NSM.
    // 6. In an LTR label, the end of the label must be a character with
    // BIDI property L or EN, followed by zero or more characters with
    // BIDI property NSM.
    إذا( (لومړی_ماسک&L_ماسک)!=0 ?
            (وروستی_ماسک&~L_EN_ماسک)!=0 :
            (وروستی_ماسک&~R_AL_EN_AN_ماسک)!=0
    ) {
        info.isOkBiDi=خطأ;
    }
    // Add the directionalities of the intervening characters.
    uint32_t mask=لومړی_ماسک|وروستی_ماسک;
    طالما(i<label_اوږدتیا) {
        U16_بل_UNSAFE(label, i, c);
        mask|=U_ماسک(u_charDirection(c));
    }
    إذا(لومړی_ماسک&L_ماسک) {
        // 5. In an LTR label, only characters with the BIDI properties L, EN,
        // ES, CS, ET, ON, BN and NSM are allowed.
        إذا((mask&~L_EN_ES_CS_ET_ON_BN_NSM_ماسک)!=0) {
            info.isOkBiDi=خطأ;
        }
    } وإلا {
        // 2. In an RTL label, only characters with the BIDI properties R, AL,
        // AN, EN, ES, CS, ET, ON, BN and NSM are allowed.
        إذا((mask&~R_AL_AN_EN_ES_CS_ET_ON_BN_NSM_ماسک)!=0) {
            info.isOkBiDi=خطأ;
        }
        // 4. In an RTL label, if an EN is present, no AN may be present, and
        // vice versa.
        إذا((mask&EN_AN_ماسک)==EN_AN_ماسک) {
            info.isOkBiDi=خطأ;
        }
    }
    // An RTL label is a label that contains at least one character of type
    // R, AL or AN. [...]
    // A "BIDI domain name" is a domain name that contains at least one RTL
    // label. [...]
    // The following rule, consisting of six conditions, applies to labels
    // in BIDI domain names.
    إذا((mask&R_AL_AN_ماسک)!=0) {
        info.isBiDi=ریښتیا;
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
    int32_t label_پیل=0;
    لكل(int32_t i=0; i<length; ++i) {
        char16_t c=s[i];
        إذا(c==0x2e) {  // dot
            إذا(i>label_پیل) {
                c=s[i-1];
                إذا(!(0x61<=c && c<=0x7a) && !(0x30<=c && c<=0x39)) {
                    // Last character in the label is not an L or EN.
                    أرجع خطأ;
                }
            }
            label_پیل=i+1;
        } وإلا إذا(i==label_پیل) {
            إذا(!(0x61<=c && c<=0x7a)) {
                // First character in the label is not an L.
                أرجع خطأ;
            }
        } وإلا {
            إذا(c<=0x20 && (c>=0x1c || (9<=c && c<=0xd))) {
                // Intermediate character in the label is a B, S or WS.
                أرجع خطأ;
            }
        }
    }
    أرجع ریښتیا;
}

// UTF-8 version, called for source ASCII prefix.
// Can contain uppercase A-Z.
// s[length-1] must be the trailing dot.
ساكن UBool
isASCIIOkBiDi(ثابت حرف *s, int32_t length) {
    int32_t label_پیل=0;
    لكل(int32_t i=0; i<length; ++i) {
        حرف c=s[i];
        إذا(c==0x2e) {  // dot
            إذا(i>label_پیل) {
                c=s[i-1];
                إذا(!(0x61<=c && c<=0x7a) && !(0x41<=c && c<=0x5a) && !(0x30<=c && c<=0x39)) {
                    // Last character in the label is not an L or EN.
                    أرجع خطأ;
                }
            }
            label_پیل=i+1;
        } وإلا إذا(i==label_پیل) {
            إذا(!(0x61<=c && c<=0x7a) && !(0x41<=c && c<=0x5a)) {
                // First character in the label is not an L.
                أرجع خطأ;
            }
        } وإلا {
            إذا(c<=0x20 && (c>=0x1c || (9<=c && c<=0xd))) {
                // Intermediate character in the label is a B, S or WS.
                أرجع خطأ;
            }
        }
    }
    أرجع ریښتیا;
}

UBool
UTS46::isLabelOkContextJ(ثابت char16_t *label, int32_t label_اوږدتیا) ثابت {
    // [IDNA2008-Tables]
    // 200C..200D  ; CONTEXTJ    # ZERO WIDTH NON-JOINER..ZERO WIDTH JOINER
    لكل(int32_t i=0; i<label_اوږدتیا; ++i) {
        إذا(label[i]==0x200c) {
            // Appendix A.1. ZERO WIDTH NON-JOINER
            // Rule Set:
            //  False;
            //  If Canonical_Combining_Class(Before(cp)) .eq.  Virama Then True;
            //  If RegExpMatch((Joining_Type:{L,D})(Joining_Type:T)*\u200C
            //     (Joining_Type:T)*(Joining_Type:{R,D})) Then True;
            إذا(i==0) {
                أرجع خطأ;
            }
            UChar32 c;
            int32_t j=i;
            U16_PREV_UNSAFE(label, j, c);
            إذا(uts46Norm2.getCombiningClass(c)==9) {
                استمر;
            }
            // check precontext (Joining_Type:{L,D})(Joining_Type:T)*
            لكل(;;) {
                UJoiningType type=ubidi_getJoiningType(c);
                إذا(type==U_JT_TRANSPARENT) {
                    إذا(j==0) {
                        أرجع خطأ;
                    }
                    U16_PREV_UNSAFE(label, j, c);
                } وإلا إذا(type==U_JT_کیڼ_JOINING || type==U_JT_DUAL_JOINING) {
                    اكسر;  // precontext fulfilled
                } وإلا {
                    أرجع خطأ;
                }
            }
            // check postcontext (Joining_Type:T)*(Joining_Type:{R,D})
            لكل(j=i+1;;) {
                إذا(j==label_اوږدتیا) {
                    أرجع خطأ;
                }
                U16_بل_UNSAFE(label, j, c);
                UJoiningType type=ubidi_getJoiningType(c);
                إذا(type==U_JT_TRANSPARENT) {
                    // just skip this character
                } وإلا إذا(type==U_JT_ښی_JOINING || type==U_JT_DUAL_JOINING) {
                    اكسر;  // postcontext fulfilled
                } وإلا {
                    أرجع خطأ;
                }
            }
        } وإلا إذا(label[i]==0x200d) {
            // Appendix A.2. ZERO WIDTH JOINER (U+200D)
            // Rule Set:
            //  False;
            //  If Canonical_Combining_Class(Before(cp)) .eq.  Virama Then True;
            إذا(i==0) {
                أرجع خطأ;
            }
            UChar32 c;
            int32_t j=i;
            U16_PREV_UNSAFE(label, j, c);
            إذا(uts46Norm2.getCombiningClass(c)!=9) {
                أرجع خطأ;
            }
        }
    }
    أرجع ریښتیا;
}

فراغ
UTS46::checkLabelContextO(ثابت char16_t *label, int32_t label_اوږدتیا, IDNAInfo &info) ثابت {
    int32_t label_پای_ټکی=label_اوږدتیا-1;  // inclusive
    int32_t arabicDigits=0;  // -1 for 066x, +1 for 06Fx
    لكل(int32_t i=0; i<=label_پای_ټکی; ++i) {
        UChar32 c=label[i];
        إذا(c<0xb7) {
            // ASCII fastpath
        } وإلا إذا(c<=0x6f9) {
            إذا(c==0xb7) {
                // Appendix A.3. MIDDLE DOT (U+00B7)
                // Rule Set:
                //  False;
                //  If Before(cp) .eq.  U+006C And
                //     After(cp) .eq.  U+006C Then True;
                إذا(!(0<i && label[i-1]==0x6c &&
                     i<label_پای_ټکی && label[i+1]==0x6c)) {
                    info.labelErrors|=UIDNA_ERROR_CONTEXTO_PUNCTUATION;
                }
            } وإلا إذا(c==0x375) {
                // Appendix A.4. GREEK LOWER NUMERAL SIGN (KERAIA) (U+0375)
                // Rule Set:
                //  False;
                //  If Script(After(cp)) .eq.  Greek Then True;
                UScript_کوډ script=USCRIPT_INVALID_کوډ;
                إذا(i<label_پای_ټکی) {
                    UError_کوډ error_کوډ=U_ZERO_ERROR;
                    int32_t j=i+1;
                    U16_بل(label, j, label_اوږدتیا, c);
                    script=uscript_getScript(c, &error_کوډ);
                }
                إذا(script!=USCRIPT_GREEK) {
                    info.labelErrors|=UIDNA_ERROR_CONTEXTO_PUNCTUATION;
                }
            } وإلا إذا(c==0x5f3 || c==0x5f4) {
                // Appendix A.5. HEBREW PUNCTUATION GERESH (U+05F3)
                // Rule Set:
                //  False;
                //  If Script(Before(cp)) .eq.  Hebrew Then True;
                //
                // Appendix A.6. HEBREW PUNCTUATION GERSHAYIM (U+05F4)
                // Rule Set:
                //  False;
                //  If Script(Before(cp)) .eq.  Hebrew Then True;
                UScript_کوډ script=USCRIPT_INVALID_کوډ;
                إذا(0<i) {
                    UError_کوډ error_کوډ=U_ZERO_ERROR;
                    int32_t j=i;
                    U16_PREV(label, 0, j, c);
                    script=uscript_getScript(c, &error_کوډ);
                }
                إذا(script!=USCRIPT_HEBREW) {
                    info.labelErrors|=UIDNA_ERROR_CONTEXTO_PUNCTUATION;
                }
            } وإلا إذا(0x660<=c /* && c<=0x6f9 */) {
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
                إذا(c<=0x669) {
                    إذا(arabicDigits>0) {
                        info.labelErrors|=UIDNA_ERROR_CONTEXTO_DIGITS;
                    }
                    arabicDigits=-1;
                } وإلا إذا(0x6f0<=c) {
                    إذا(arabicDigits<0) {
                        info.labelErrors|=UIDNA_ERROR_CONTEXTO_DIGITS;
                    }
                    arabicDigits=1;
                }
            }
        } وإلا إذا(c==0x30fb) {
            // Appendix A.7. KATAKANA MIDDLE DOT (U+30FB)
            // Rule Set:
            //  False;
            //  For All Characters:
            //    If Script(cp) .in. {Hiragana, Katakana, Han} Then True;
            //  End For;
            UError_کوډ error_کوډ=U_ZERO_ERROR;
            لكل(صحيح j=0;;) {
                إذا(j>label_پای_ټکی) {
                    info.labelErrors|=UIDNA_ERROR_CONTEXTO_PUNCTUATION;
                    اكسر;
                }
                U16_بل(label, j, label_اوږدتیا, c);
                UScript_کوډ script=uscript_getScript(c, &error_کوډ);
                إذا(script==USCRIPT_HIRAGANA || script==USCRIPT_KATAKANA || script==USCRIPT_HAN) {
                    اكسر;
                }
            }
        }
    }
}

U_NAMESPACE_پای_ټکی

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

U_CAPI UIDNA * U_EXPORT2
uidna_openUTS46(uint32_t options, UError_کوډ *p_Error_کوډ) {
    أرجع reinterpret_cast<UIDNA *>(IDNA::createUTS46Instance(options, *p_Error_کوډ));
}

U_CAPI فراغ U_EXPORT2
uidna_close(UIDNA *idna) {
    ړنګول reinterpret_cast<IDNA *>(idna);
}

ساكن UBool
check_دلیلونه(ثابت فراغ *label, int32_t length,
          فراغ *dest, int32_t capacity,
          UIDNAInfo *pInfo, UError_کوډ *p_Error_کوډ) {
    إذا(U_FAILURE(*p_Error_کوډ)) {
        أرجع خطأ;
    }
    // sizeof(UIDNAInfo)=16 in the first API version.
    إذا(pInfo==تش_اشاره || pInfo->size<16) {
        *p_Error_کوډ=U_ILLEGAL_دلیل_ERROR;
        أرجع خطأ;
    }
    إذا( (label==تش_اشاره ? length!=0 : length<-1) ||
        (dest==تش_اشاره ? capacity!=0 : capacity<0) ||
        (dest==label && label!=تش_اشاره)
    ) {
        *p_Error_کوډ=U_ILLEGAL_دلیل_ERROR;
        أرجع خطأ;
    }
    // Set all *pInfo bytes to 0 except for the size field itself.
    uprv_memset(&pInfo->size+1, 0, pInfo->size-حجم(pInfo->size));
    أرجع ریښتیا;
}

ساكن فراغ
idna_Info_منزل_Struct(IDNAInfo &info, UIDNAInfo *pInfo) {
    pInfo->isTransitionalDifferent=info.isTransitionalDifferent();
    pInfo->errors=info.getErrors();
}

U_CAPI int32_t U_EXPORT2
uidna_label_منزل_ASCII(ثابت UIDNA *idna,
                   ثابت char16_t *label, int32_t length,
                   char16_t *dest, int32_t capacity,
                   UIDNAInfo *pInfo, UError_کوډ *p_Error_کوډ) {
    إذا(!check_دلیلونه(label, length, dest, capacity, pInfo, p_Error_کوډ)) {
        أرجع 0;
    }
    UnicodeString src(length < 0, label, length);
    UnicodeString destString(dest, 0, capacity);
    IDNAInfo info;
    reinterpret_cast<ثابت IDNA *>(idna)->label_منزل_ASCII(src, destString, info, *p_Error_کوډ);
    idna_Info_منزل_Struct(info, pInfo);
    أرجع destString.extract(dest, capacity, *p_Error_کوډ);
}

U_CAPI int32_t U_EXPORT2
uidna_label_منزل_Unicode(ثابت UIDNA *idna,
                     ثابت char16_t *label, int32_t length,
                     char16_t *dest, int32_t capacity,
                     UIDNAInfo *pInfo, UError_کوډ *p_Error_کوډ) {
    إذا(!check_دلیلونه(label, length, dest, capacity, pInfo, p_Error_کوډ)) {
        أرجع 0;
    }
    UnicodeString src(length < 0, label, length);
    UnicodeString destString(dest, 0, capacity);
    IDNAInfo info;
    reinterpret_cast<ثابت IDNA *>(idna)->label_منزل_Unicode(src, destString, info, *p_Error_کوډ);
    idna_Info_منزل_Struct(info, pInfo);
    أرجع destString.extract(dest, capacity, *p_Error_کوډ);
}

U_CAPI int32_t U_EXPORT2
uidna_name_منزل_ASCII(ثابت UIDNA *idna,
                  ثابت char16_t *name, int32_t length,
                  char16_t *dest, int32_t capacity,
                  UIDNAInfo *pInfo, UError_کوډ *p_Error_کوډ) {
    إذا(!check_دلیلونه(name, length, dest, capacity, pInfo, p_Error_کوډ)) {
        أرجع 0;
    }
    UnicodeString src(length < 0, name, length);
    UnicodeString destString(dest, 0, capacity);
    IDNAInfo info;
    reinterpret_cast<ثابت IDNA *>(idna)->name_منزل_ASCII(src, destString, info, *p_Error_کوډ);
    idna_Info_منزل_Struct(info, pInfo);
    أرجع destString.extract(dest, capacity, *p_Error_کوډ);
}

U_CAPI int32_t U_EXPORT2
uidna_name_منزل_Unicode(ثابت UIDNA *idna,
                    ثابت char16_t *name, int32_t length,
                    char16_t *dest, int32_t capacity,
                    UIDNAInfo *pInfo, UError_کوډ *p_Error_کوډ) {
    إذا(!check_دلیلونه(name, length, dest, capacity, pInfo, p_Error_کوډ)) {
        أرجع 0;
    }
    UnicodeString src(length < 0, name, length);
    UnicodeString destString(dest, 0, capacity);
    IDNAInfo info;
    reinterpret_cast<ثابت IDNA *>(idna)->name_منزل_Unicode(src, destString, info, *p_Error_کوډ);
    idna_Info_منزل_Struct(info, pInfo);
    أرجع destString.extract(dest, capacity, *p_Error_کوډ);
}

U_CAPI int32_t U_EXPORT2
uidna_label_منزل_ASCII_UTF8(ثابت UIDNA *idna,
                        ثابت حرف *label, int32_t length,
                        حرف *dest, int32_t capacity,
                        UIDNAInfo *pInfo, UError_کوډ *p_Error_کوډ) {
    إذا(!check_دلیلونه(label, length, dest, capacity, pInfo, p_Error_کوډ)) {
        أرجع 0;
    }
    StringPiece src(label, length<0 ? static_cast<int32_t>(uprv_strlen(label)) : length);
    أرجع ByteSinkUtil::له_لارې_Byte_Sink_منزل_Terminated_Chars(
        dest, capacity,
        [&](ByteSink& sink, UError_کوډ& حالت) {
            IDNAInfo info;
            reinterpret_cast<ثابت IDNA *>(idna)->label_منزل_ASCII_UTF8(src, sink, info, حالت);
            idna_Info_منزل_Struct(info, pInfo);
        },
        *p_Error_کوډ);
}

U_CAPI int32_t U_EXPORT2
uidna_label_منزل_Unicode_UTF8(ثابت UIDNA *idna,
                         ثابت حرف *label, int32_t length,
                         حرف *dest, int32_t capacity,
                         UIDNAInfo *pInfo, UError_کوډ *p_Error_کوډ) {
    إذا(!check_دلیلونه(label, length, dest, capacity, pInfo, p_Error_کوډ)) {
        أرجع 0;
    }
    StringPiece src(label, length<0 ? static_cast<int32_t>(uprv_strlen(label)) : length);
    أرجع ByteSinkUtil::له_لارې_Byte_Sink_منزل_Terminated_Chars(
        dest, capacity,
        [&](ByteSink& sink, UError_کوډ& حالت) {
            IDNAInfo info;
            reinterpret_cast<ثابت IDNA *>(idna)->label_منزل_Unicode_UTF8(src, sink, info, حالت);
            idna_Info_منزل_Struct(info, pInfo);
        },
        *p_Error_کوډ);
}

U_CAPI int32_t U_EXPORT2
uidna_name_منزل_ASCII_UTF8(ثابت UIDNA *idna,
                       ثابت حرف *name, int32_t length,
                       حرف *dest, int32_t capacity,
                       UIDNAInfo *pInfo, UError_کوډ *p_Error_کوډ) {
    إذا(!check_دلیلونه(name, length, dest, capacity, pInfo, p_Error_کوډ)) {
        أرجع 0;
    }
    StringPiece src(name, length<0 ? static_cast<int32_t>(uprv_strlen(name)) : length);
    أرجع ByteSinkUtil::له_لارې_Byte_Sink_منزل_Terminated_Chars(
        dest, capacity,
        [&](ByteSink& sink, UError_کوډ& حالت) {
            IDNAInfo info;
            reinterpret_cast<ثابت IDNA *>(idna)->name_منزل_ASCII_UTF8(src, sink, info, حالت);
            idna_Info_منزل_Struct(info, pInfo);
        },
        *p_Error_کوډ);
}

U_CAPI int32_t U_EXPORT2
uidna_name_منزل_Unicode_UTF8(ثابت UIDNA *idna,
                        ثابت حرف *name, int32_t length,
                        حرف *dest, int32_t capacity,
                        UIDNAInfo *pInfo, UError_کوډ *p_Error_کوډ) {
    إذا(!check_دلیلونه(name, length, dest, capacity, pInfo, p_Error_کوډ)) {
        أرجع 0;
    }
    StringPiece src(name, length<0 ? static_cast<int32_t>(uprv_strlen(name)) : length);
    أرجع ByteSinkUtil::له_لارې_Byte_Sink_منزل_Terminated_Chars(
        dest, capacity,
        [&](ByteSink& sink, UError_کوډ& حالت) {
            IDNAInfo info;
            reinterpret_cast<ثابت IDNA *>(idna)->name_منزل_Unicode_UTF8(src, sink, info, حالت);
            idna_Info_منزل_Struct(info, pInfo);
        },
        *p_Error_کوډ);
}

#د_که_پای  // UCONFIG_NO_IDNA
