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

ساکن اندرونی UBool
isASCIIString(ثابت UnicodeString &dest) {
    ثابت char16_t *s=dest.get_بفر();
    ثابت char16_t *حد_بندی=s+dest.length();
    جب(s<حد_بندی) {
        اگر(*s++>0x7f) {
            واپس جھوٹ;
        }
    }
    واپس سچ;
}

ساکن UBool
isASCIIOkBiDi(ثابت char16_t *s, int32_t length);

ساکن UBool
isASCIIOkBiDi(ثابت حرف *s, int32_t length);

// IDNA class default implementations -------------------------------------- ***

IDNA::~IDNA() {}

خالی
IDNA::label_مقصد_ASCII_UTF8(StringPiece label, ByteSink &dest,
                        IDNAInfo &info, UError_رمز &error_رمز) ثابت {
    اگر(U_SUCCESS(error_رمز)) {
        UnicodeString destString;
        label_مقصد_ASCII(UnicodeString::fromUTF8(label), destString,
                     info, error_رمز).مقصد_UTF8(dest);
    }
}

خالی
IDNA::label_مقصد_Unicode_UTF8(StringPiece label, ByteSink &dest,
                         IDNAInfo &info, UError_رمز &error_رمز) ثابت {
    اگر(U_SUCCESS(error_رمز)) {
        UnicodeString destString;
        label_مقصد_Unicode(UnicodeString::fromUTF8(label), destString,
                       info, error_رمز).مقصد_UTF8(dest);
    }
}

خالی
IDNA::name_مقصد_ASCII_UTF8(StringPiece name, ByteSink &dest,
                       IDNAInfo &info, UError_رمز &error_رمز) ثابت {
    اگر(U_SUCCESS(error_رمز)) {
        UnicodeString destString;
        name_مقصد_ASCII(UnicodeString::fromUTF8(name), destString,
                    info, error_رمز).مقصد_UTF8(dest);
    }
}

خالی
IDNA::name_مقصد_Unicode_UTF8(StringPiece name, ByteSink &dest,
                        IDNAInfo &info, UError_رمز &error_رمز) ثابت {
    اگر(U_SUCCESS(error_رمز)) {
        UnicodeString destString;
        name_مقصد_Unicode(UnicodeString::fromUTF8(name), destString,
                      info, error_رمز).مقصد_UTF8(dest);
    }
}

// UTS46 class declaration ------------------------------------------------- ***

جماعت UTS46 : عوامی IDNA {
عوامی:
    UTS46(uint32_t options, UError_رمز &error_رمز);
    مجازی ~UTS46();

    مجازی UnicodeString&
    label_مقصد_ASCII(ثابت UnicodeString& label,
                 UnicodeString& dest U_LIFETIME_BOUND,
                 IDNAInfo& info,
                 UError_رمز& error_رمز) ثابت override;

    مجازی UnicodeString&
    label_مقصد_Unicode(ثابت UnicodeString& label,
                   UnicodeString& dest U_LIFETIME_BOUND,
                   IDNAInfo& info,
                   UError_رمز& error_رمز) ثابت override;

    مجازی UnicodeString&
    name_مقصد_ASCII(ثابت UnicodeString& name,
                UnicodeString& dest U_LIFETIME_BOUND,
                IDNAInfo& info,
                UError_رمز& error_رمز) ثابت override;

    مجازی UnicodeString&
    name_مقصد_Unicode(ثابت UnicodeString& name,
                  UnicodeString& dest U_LIFETIME_BOUND,
                  IDNAInfo& info,
                  UError_رمز& error_رمز) ثابت override;

    مجازی خالی
    label_مقصد_ASCII_UTF8(StringPiece label, ByteSink &dest,
                      IDNAInfo &info, UError_رمز &error_رمز) ثابت override;

    مجازی خالی
    label_مقصد_Unicode_UTF8(StringPiece label, ByteSink &dest,
                       IDNAInfo &info, UError_رمز &error_رمز) ثابت override;

    مجازی خالی
    name_مقصد_ASCII_UTF8(StringPiece name, ByteSink &dest,
                     IDNAInfo &info, UError_رمز &error_رمز) ثابت override;

    مجازی خالی
    name_مقصد_Unicode_UTF8(StringPiece name, ByteSink &dest,
                      IDNAInfo &info, UError_رمز &error_رمز) ثابت override;

نجی:
    UnicodeString&
    process(ثابت UnicodeString& src,
            UBool isLabel,
            UBool مقصد_ASCII,
            UnicodeString& dest U_LIFETIME_BOUND,
            IDNAInfo& info,
            UError_رمز& error_رمز) ثابت;

    خالی
    processUTF8(StringPiece src,
                UBool isLabel, UBool مقصد_ASCII,
                ByteSink &dest,
                IDNAInfo &info, UError_رمز &error_رمز) ثابت;

    UnicodeString&
    processUnicode(ثابت UnicodeString& src,
                   int32_t label_آغاز,
                   int32_t mapping_آغاز,
                   UBool isLabel,
                   UBool مقصد_ASCII,
                   UnicodeString& dest U_LIFETIME_BOUND,
                   IDNAInfo& info,
                   UError_رمز& error_رمز) ثابت;

    // returns the new dest.length()
    int32_t
    mapDevChars(UnicodeString &dest, int32_t label_آغاز, int32_t mapping_آغاز,
                UError_رمز &error_رمز) ثابت;

    // returns the new label length
    int32_t
    processLabel(UnicodeString &dest,
                 int32_t label_آغاز, int32_t label_لمبائی,
                 UBool مقصد_ASCII,
                 IDNAInfo &info, UError_رمز &error_رمز) ثابت;
    int32_t
    markBadACELabel(UnicodeString &dest,
                    int32_t label_آغاز, int32_t label_لمبائی,
                    UBool مقصد_ASCII, IDNAInfo &info, UError_رمز &error_رمز) ثابت;

    خالی
    checkLabelBiDi(ثابت char16_t *label, int32_t label_لمبائی, IDNAInfo &info) ثابت;

    UBool
    isLabelOkContextJ(ثابت char16_t *label, int32_t label_لمبائی) ثابت;

    خالی
    checkLabelContextO(ثابت char16_t *label, int32_t label_لمبائی, IDNAInfo &info) ثابت;

    ثابت Normalizer2 &uts46Norm2;  // uts46.nrm
    uint32_t options;
};

IDNA *
IDNA::createUTS46Instance(uint32_t options, UError_رمز &error_رمز) {
    اگر(U_SUCCESS(error_رمز)) {
        IDNA *idna=نیا UTS46(options, error_رمز);
        اگر(idna==خالی_اشاریہ) {
            error_رمز=U_حافظہ_تخصیص_ERROR;
        } ورنہ اگر(U_FAILURE(error_رمز)) {
            حذف idna;
            idna=خالی_اشاریہ;
        }
        واپس idna;
    } ورنہ {
        واپس خالی_اشاریہ;
    }
}

// UTS46 implementation ---------------------------------------------------- ***

UTS46::UTS46(uint32_t opt, UError_رمز &error_رمز)
        : uts46Norm2(*Normalizer2::getInstance(خالی_اشاریہ, "uts46", UNORM2_COMPOSE, error_رمز)),
          options(opt) {}

UTS46::~UTS46() {}

UnicodeString&
UTS46::label_مقصد_ASCII(ثابت UnicodeString& label,
                    UnicodeString& dest U_LIFETIME_BOUND,
                    IDNAInfo& info,
                    UError_رمز& error_رمز) ثابت {
    واپس process(label, سچ, سچ, dest, info, error_رمز);
}

UnicodeString&
UTS46::label_مقصد_Unicode(ثابت UnicodeString& label,
                      UnicodeString& dest U_LIFETIME_BOUND,
                      IDNAInfo& info,
                      UError_رمز& error_رمز) ثابت {
    واپس process(label, سچ, جھوٹ, dest, info, error_رمز);
}

UnicodeString&
UTS46::name_مقصد_ASCII(ثابت UnicodeString& name,
                   UnicodeString& dest U_LIFETIME_BOUND,
                   IDNAInfo& info,
                   UError_رمز& error_رمز) ثابت {
    process(name, جھوٹ, سچ, dest, info, error_رمز);
    اگر( dest.length()>=254 && (info.errors&UIDNA_ERROR_DOMAIN_NAME_TOO_LONG)==0 &&
        isASCIIString(dest) &&
        (dest.length()>254 || dest[253]!=0x2e)
    ) {
        info.errors|=UIDNA_ERROR_DOMAIN_NAME_TOO_LONG;
    }
    واپس dest;
}

UnicodeString&
UTS46::name_مقصد_Unicode(ثابت UnicodeString& name,
                     UnicodeString& dest U_LIFETIME_BOUND,
                     IDNAInfo& info,
                     UError_رمز& error_رمز) ثابت {
    واپس process(name, جھوٹ, جھوٹ, dest, info, error_رمز);
}

خالی
UTS46::label_مقصد_ASCII_UTF8(StringPiece label, ByteSink &dest,
                         IDNAInfo &info, UError_رمز &error_رمز) ثابت {
    processUTF8(label, سچ, سچ, dest, info, error_رمز);
}

خالی
UTS46::label_مقصد_Unicode_UTF8(StringPiece label, ByteSink &dest,
                          IDNAInfo &info, UError_رمز &error_رمز) ثابت {
    processUTF8(label, سچ, جھوٹ, dest, info, error_رمز);
}

خالی
UTS46::name_مقصد_ASCII_UTF8(StringPiece name, ByteSink &dest,
                        IDNAInfo &info, UError_رمز &error_رمز) ثابت {
    processUTF8(name, جھوٹ, سچ, dest, info, error_رمز);
}

خالی
UTS46::name_مقصد_Unicode_UTF8(StringPiece name, ByteSink &dest,
                         IDNAInfo &info, UError_رمز &error_رمز) ثابت {
    processUTF8(name, جھوٹ, جھوٹ, dest, info, error_رمز);
}

// UTS #46 data for ASCII characters.
// The normalizer (using uts46.nrm) maps uppercase ASCII letters to lowercase
// and passes through all other ASCII characters.
// If UIDNA_USE_STD3_RULES is set, then non-LDH characters are disallowed
// using this data.
// The ASCII fastpath also uses this data.
// Values: -1=disallowed  0==valid  1==mapped (lowercase)
ساکن ثابت int8_t ascii_ڈیٹا[128]={
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
               UBool مقصد_ASCII,
               UnicodeString& dest U_LIFETIME_BOUND,
               IDNAInfo& info,
               UError_رمز& error_رمز) ثابت {
    // uts46Norm2.normalize() would do all of this error checking and setup,
    // but with the ASCII fastpath we do not always call it, and do not
    // call it first.
    اگر(U_FAILURE(error_رمز)) {
        dest.سیٹ_مقصد_Bogus();
        واپس dest;
    }
    ثابت char16_t *srcArray=src.get_بفر();
    اگر(&dest==&src || srcArray==خالی_اشاریہ) {
        error_رمز=U_ILLEGAL_دلیل_ERROR;
        dest.سیٹ_مقصد_Bogus();
        واپس dest;
    }
    // Arguments are fine, reset output values.
    dest.remove();
    info.reset();
    int32_t src_لمبائی=src.length();
    اگر(src_لمبائی==0) {
        info.errors|=UIDNA_ERROR_خالی_خانے_LABEL;
        واپس dest;
    }
    char16_t *destArray=dest.get_بفر(src_لمبائی);
    اگر(destArray==خالی_اشاریہ) {
        error_رمز=U_حافظہ_تخصیص_ERROR;
        واپس dest;
    }
    // ASCII fastpath
    UBool disallowNonLDHDot=(options&UIDNA_USE_STD3_قواعد)!=0;
    int32_t label_آغاز=0;
    int32_t i;
    برائے(i=0;; ++i) {
        اگر(i==src_لمبائی) {
            اگر(مقصد_ASCII) {
                اگر((i-label_آغاز)>63) {
                    info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
                }
                // There is a trailing dot if labelStart==i.
                اگر(!isLabel && i>=254 && (i>254 || label_آغاز<i)) {
                    info.errors|=UIDNA_ERROR_DOMAIN_NAME_TOO_LONG;
                }
            }
            info.errors|=info.labelErrors;
            dest.release_بفر(i);
            واپس dest;
        }
        char16_t c=srcArray[i];
        اگر(c>0x7f) {
            توڑو;
        }
        صحیح c_ڈیٹا=ascii_ڈیٹا[c];
        اگر(c_ڈیٹا>0) {
            destArray[i]=c+0x20;  // Lowercase an uppercase ASCII letter.
        } ورنہ اگر(c_ڈیٹا<0 && disallowNonLDHDot) {
            توڑو;  // Replacing with U+FFFD can be complicated for toASCII.
        } ورنہ {
            destArray[i]=c;
            اگر(c==0x2d) {  // hyphen
                اگر(i==(label_آغاز+3) && srcArray[i-1]==0x2d) {
                    // "??--..." is Punycode or forbidden.
                    ++i;  // '-' was copied to dest already
                    توڑو;
                }
                اگر(i==label_آغاز) {
                    // label starts with "-"
                    info.labelErrors|=UIDNA_ERROR_LEADING_HYPHEN;
                }
                اگر((i+1)==src_لمبائی || srcArray[i+1]==0x2e) {
                    // label ends with "-"
                    info.labelErrors|=UIDNA_ERROR_TRAILING_HYPHEN;
                }
            } ورنہ اگر(c==0x2e) {  // dot
                اگر(isLabel) {
                    // Replacing with U+FFFD can be complicated for toASCII.
                    ++i;  // '.' was copied to dest already
                    توڑو;
                }
                اگر(i==label_آغاز) {
                    info.labelErrors|=UIDNA_ERROR_خالی_خانے_LABEL;
                }
                اگر(مقصد_ASCII && (i-label_آغاز)>63) {
                    info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
                }
                info.errors|=info.labelErrors;
                info.labelErrors=0;
                label_آغاز=i+1;
            }
        }
    }
    info.errors|=info.labelErrors;
    dest.release_بفر(i);
    processUnicode(src, label_آغاز, i, isLabel, مقصد_ASCII, dest, info, error_رمز);
    اگر( info.isBiDi && U_SUCCESS(error_رمز) && (info.errors&severeErrors)==0 &&
        (!info.isOkBiDi || (label_آغاز>0 && !isASCIIOkBiDi(dest.get_بفر(), label_آغاز)))
    ) {
        info.errors|=UIDNA_ERROR_BIDI;
    }
    واپس dest;
}

خالی
UTS46::processUTF8(StringPiece src,
                   UBool isLabel, UBool مقصد_ASCII,
                   ByteSink &dest,
                   IDNAInfo &info, UError_رمز &error_رمز) ثابت {
    اگر(U_FAILURE(error_رمز)) {
        واپس;
    }
    ثابت حرف *srcArray=src.ڈیٹا();
    int32_t src_لمبائی=src.length();
    اگر(srcArray==خالی_اشاریہ && src_لمبائی!=0) {
        error_رمز=U_ILLEGAL_دلیل_ERROR;
        واپس;
    }
    // Arguments are fine, reset output values.
    info.reset();
    اگر(src_لمبائی==0) {
        info.errors|=UIDNA_ERROR_خالی_خانے_LABEL;
        dest.Flush();
        واپس;
    }
    UnicodeString destString;
    int32_t label_آغاز=0;
    اگر(src_لمبائی<=256) {  // length of stackArray[]
        // ASCII fastpath
        حرف ڈھیر_Array[256];
        int32_t dest_استعداد;
        حرف *destArray=dest.Get_Append_بفر(src_لمبائی, src_لمبائی+20,
                                             ڈھیر_Array, UPRV_LENGTHOF(ڈھیر_Array), &dest_استعداد);
        UBool disallowNonLDHDot=(options&UIDNA_USE_STD3_قواعد)!=0;
        int32_t i;
        برائے(i=0;; ++i) {
            اگر(i==src_لمبائی) {
                اگر(مقصد_ASCII) {
                    اگر((i-label_آغاز)>63) {
                        info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
                    }
                    // There is a trailing dot if labelStart==i.
                    اگر(!isLabel && i>=254 && (i>254 || label_آغاز<i)) {
                        info.errors|=UIDNA_ERROR_DOMAIN_NAME_TOO_LONG;
                    }
                }
                info.errors|=info.labelErrors;
                dest.Append(destArray, i);
                dest.Flush();
                واپس;
            }
            حرف c=srcArray[i];
            اگر (static_cast<int8_t>(c) < 0) { // (uint8_t)c>0x7f
                توڑو;
            }
            صحیح c_ڈیٹا = ascii_ڈیٹا[static_cast<صحیح>(c)]; // Cast: gcc warns about indexing with a char.
            اگر(c_ڈیٹا>0) {
                destArray[i]=c+0x20;  // Lowercase an uppercase ASCII letter.
            } ورنہ اگر(c_ڈیٹا<0 && disallowNonLDHDot) {
                توڑو;  // Replacing with U+FFFD can be complicated for toASCII.
            } ورنہ {
                destArray[i]=c;
                اگر(c==0x2d) {  // hyphen
                    اگر(i==(label_آغاز+3) && srcArray[i-1]==0x2d) {
                        // "??--..." is Punycode or forbidden.
                        توڑو;
                    }
                    اگر(i==label_آغاز) {
                        // label starts with "-"
                        info.labelErrors|=UIDNA_ERROR_LEADING_HYPHEN;
                    }
                    اگر((i+1)==src_لمبائی || srcArray[i+1]==0x2e) {
                        // label ends with "-"
                        info.labelErrors|=UIDNA_ERROR_TRAILING_HYPHEN;
                    }
                } ورنہ اگر(c==0x2e) {  // dot
                    اگر(isLabel) {
                        توڑو;  // Replacing with U+FFFD can be complicated for toASCII.
                    }
                    اگر(i==label_آغاز) {
                        info.labelErrors|=UIDNA_ERROR_خالی_خانے_LABEL;
                    }
                    اگر(مقصد_ASCII && (i-label_آغاز)>63) {
                        info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
                    }
                    info.errors|=info.labelErrors;
                    info.labelErrors=0;
                    label_آغاز=i+1;
                }
            }
        }
        info.errors|=info.labelErrors;
        // Convert the processed ASCII prefix of the current label to UTF-16.
        int32_t mapping_آغاز=i-label_آغاز;
        destString=UnicodeString::fromUTF8(StringPiece(destArray+label_آغاز, mapping_آغاز));
        // Output the previous ASCII labels and process the rest of src in UTF-16.
        dest.Append(destArray, label_آغاز);
        processUnicode(UnicodeString::fromUTF8(StringPiece(src, label_آغاز)), 0, mapping_آغاز,
                       isLabel, مقصد_ASCII,
                       destString, info, error_رمز);
    } ورنہ {
        // src is too long for the ASCII fastpath implementation.
        processUnicode(UnicodeString::fromUTF8(src), 0, 0,
                       isLabel, مقصد_ASCII,
                       destString, info, error_رمز);
    }
    destString.مقصد_UTF8(dest);  // calls dest.Flush()
    اگر(مقصد_ASCII && !isLabel) {
        // length==labelStart==254 means that there is a trailing dot (ok) and
        // destString is empty (do not index at 253-labelStart).
        int32_t length=label_آغاز+destString.length();
        اگر( length>=254 && isASCIIString(destString) &&
            (length>254 ||
             (label_آغاز<254 && destString[253-label_آغاز]!=0x2e))
        ) {
            info.errors|=UIDNA_ERROR_DOMAIN_NAME_TOO_LONG;
        }
    }
    اگر( info.isBiDi && U_SUCCESS(error_رمز) && (info.errors&severeErrors)==0 &&
        (!info.isOkBiDi || (label_آغاز>0 && !isASCIIOkBiDi(srcArray, label_آغاز)))
    ) {
        info.errors|=UIDNA_ERROR_BIDI;
    }
}

UnicodeString&
UTS46::processUnicode(ثابت UnicodeString& src,
                      int32_t label_آغاز,
                      int32_t mapping_آغاز,
                      UBool isLabel,
                      UBool مقصد_ASCII,
                      UnicodeString& dest U_LIFETIME_BOUND,
                      IDNAInfo& info,
                      UError_رمز& error_رمز) ثابت {
    اگر(mapping_آغاز==0) {
        uts46Norm2.normalize(src, dest, error_رمز);
    } ورنہ {
        uts46Norm2.normalize_دوسرا_And_Append(dest, src.عارضی_Sub_String(mapping_آغاز), error_رمز);
    }
    اگر(U_FAILURE(error_رمز)) {
        واپس dest;
    }
    UBool doMapDevChars=
        مقصد_ASCII ? (options&UIDNA_NONTRANSITIONAL_مقصد_ASCII)==0 :
                  (options&UIDNA_NONTRANSITIONAL_مقصد_UNICODE)==0;
    ثابت char16_t *destArray=dest.get_بفر();
    int32_t dest_لمبائی=dest.length();
    int32_t label_حد_بندی=label_آغاز;
    جب(label_حد_بندی<dest_لمبائی) {
        char16_t c=destArray[label_حد_بندی];
        اگر(c==0x2e && !isLabel) {
            int32_t label_لمبائی=label_حد_بندی-label_آغاز;
            int32_t new_لمبائی=processLabel(dest, label_آغاز, label_لمبائی,
                                            مقصد_ASCII, info, error_رمز);
            info.errors|=info.labelErrors;
            info.labelErrors=0;
            اگر(U_FAILURE(error_رمز)) {
                واپس dest;
            }
            destArray=dest.get_بفر();
            dest_لمبائی+=new_لمبائی-label_لمبائی;
            label_حد_بندی=label_آغاز+=new_لمبائی+1;
            جاری;
        } ورنہ اگر(c<0xdf) {
            // pass
        } ورنہ اگر(c<=0x200d && (c==0xdf || c==0x3c2 || c>=0x200c)) {
            info.isTransDiff=سچ;
            اگر(doMapDevChars) {
                dest_لمبائی=mapDevChars(dest, label_آغاز, label_حد_بندی, error_رمز);
                اگر(U_FAILURE(error_رمز)) {
                    واپس dest;
                }
                destArray=dest.get_بفر();
                // All deviation characters have been mapped, no need to check for them again.
                doMapDevChars=جھوٹ;
                // Do not increment labelLimit in case c was removed.
                جاری;
            }
        } ورنہ اگر(U16_IS_SURROGATE(c)) {
            اگر(U16_IS_SURROGATE_LEAD(c) ?
                    (label_حد_بندی+1)==dest_لمبائی || !U16_IS_TRAIL(destArray[label_حد_بندی+1]) :
                    label_حد_بندی==label_آغاز || !U16_IS_LEAD(destArray[label_حد_بندی-1])) {
                // Map an unpaired surrogate to U+FFFD before normalization so that when
                // that removes characters we do not turn two unpaired ones into a pair.
                info.labelErrors|=UIDNA_ERROR_DISALLOWED;
                dest.سیٹ_Char_At(label_حد_بندی, 0xfffd);
                destArray=dest.get_بفر();
            }
        }
        ++label_حد_بندی;
    }
    // Permit an empty label at the end (0<labelStart==labelLimit==destLength is ok)
    // but not an empty label elsewhere nor a completely empty domain name.
    // processLabel() sets UIDNA_ERROR_EMPTY_LABEL when labelLength==0.
    اگر(0==label_آغاز || label_آغاز<label_حد_بندی) {
        processLabel(dest, label_آغاز, label_حد_بندی-label_آغاز,
                      مقصد_ASCII, info, error_رمز);
        info.errors|=info.labelErrors;
    }
    واپس dest;
}

int32_t
UTS46::mapDevChars(UnicodeString &dest, int32_t label_آغاز, int32_t mapping_آغاز,
                   UError_رمز &error_رمز) ثابت {
    اگر(U_FAILURE(error_رمز)) {
        واپس 0;
    }
    int32_t length=dest.length();
    char16_t *s=dest.get_بفر(dest[mapping_آغاز]==0xdf ? length+1 : length);
    اگر(s==خالی_اشاریہ) {
        error_رمز=U_حافظہ_تخصیص_ERROR;
        واپس length;
    }
    int32_t capacity=dest.get_استعداد();
    UBool didMapDevChars=جھوٹ;
    int32_t read_اشاریہ=mapping_آغاز, write_اشاریہ=mapping_آغاز;
    کرو {
        char16_t c=s[read_اشاریہ++];
        انتخاب(c) {
        حالت 0xdf:
            // Map sharp s to ss.
            didMapDevChars=سچ;
            s[write_اشاریہ++]=0x73;  // Replace sharp s with first s.
            // Insert second s and account for possible buffer reallocation.
            اگر(write_اشاریہ==read_اشاریہ) {
                اگر(length==capacity) {
                    dest.release_بفر(length);
                    s=dest.get_بفر(length+1);
                    اگر(s==خالی_اشاریہ) {
                        error_رمز=U_حافظہ_تخصیص_ERROR;
                        واپس length;
                    }
                    capacity=dest.get_استعداد();
                }
                u_memmove(s+write_اشاریہ+1, s+write_اشاریہ, length-write_اشاریہ);
                ++read_اشاریہ;
            }
            s[write_اشاریہ++]=0x73;
            ++length;
            توڑو;
        حالت 0x3c2:  // Map final sigma to nonfinal sigma.
            didMapDevChars=سچ;
            s[write_اشاریہ++]=0x3c3;
            توڑو;
        حالت 0x200c:  // Ignore/remove ZWNJ.
        حالت 0x200d:  // Ignore/remove ZWJ.
            didMapDevChars=سچ;
            --length;
            توڑو;
        طےشدہ:
            // Only really necessary if writeIndex was different from readIndex.
            s[write_اشاریہ++]=c;
            توڑو;
        }
    } جب(write_اشاریہ<length);
    dest.release_بفر(length);
    اگر(didMapDevChars) {
        // Mapping deviation characters might have resulted in an un-NFC string.
        // We could use either the NFC or the UTS #46 normalizer.
        // By using the UTS #46 normalizer again, we avoid having to load a second .nrm data file.
        UnicodeString normalized;
        uts46Norm2.normalize(dest.عارضی_Sub_String(label_آغاز), normalized, error_رمز);
        اگر(U_SUCCESS(error_رمز)) {
            dest.replace(label_آغاز, 0x7fffffff, normalized);
            اگر(dest.isBogus()) {
                error_رمز=U_حافظہ_تخصیص_ERROR;
            }
            واپس dest.length();
        }
    }
    واپس length;
}

// Replace the label in dest with the label string, if the label was modified.
// If &label==&dest then the label was modified in-place and labelLength
// is the new label length, different from label.length().
// If &label!=&dest then labelLength==label.length().
// Returns labelLength (= the new label length).
ساکن int32_t
replaceLabel(UnicodeString &dest, int32_t dest_Label_آغاز, int32_t dest_Label_لمبائی,
             ثابت UnicodeString &label, int32_t label_لمبائی, UError_رمز &error_رمز) {
    اگر(U_FAILURE(error_رمز)) {
        واپس 0;
    }
    اگر(&label!=&dest) {
        dest.replace(dest_Label_آغاز, dest_Label_لمبائی, label);
        اگر(dest.isBogus()) {
            error_رمز=U_حافظہ_تخصیص_ERROR;
            واپس 0;
        }
    }
    واپس label_لمبائی;
}

int32_t
UTS46::processLabel(UnicodeString &dest,
                    int32_t label_آغاز, int32_t label_لمبائی,
                    UBool مقصد_ASCII,
                    IDNAInfo &info, UError_رمز &error_رمز) ثابت {
    اگر(U_FAILURE(error_رمز)) {
        واپس 0;
    }
    UnicodeString fromPunycode;
    UnicodeString *labelString;
    ثابت char16_t *label=dest.get_بفر()+label_آغاز;
    int32_t dest_Label_آغاز=label_آغاز;
    int32_t dest_Label_لمبائی=label_لمبائی;
    UBool wasPunycode;
    اگر(label_لمبائی>=4 && label[0]==0x78 && label[1]==0x6e && label[2]==0x2d && label[3]==0x2d) {
        // Label starts with "xn--", try to un-Punycode it.
        // In IDNA2008, labels like "xn--" (decodes to an empty string) and
        // "xn--ASCII-" (decodes to just "ASCII") fail the round-trip validation from
        // comparing the ToUnicode input with the back-to-ToASCII output.
        // They are alternate encodings of the respective ASCII labels.
        // Ignore "xn---" here: It will fail Punycode.decode() which logically comes before
        // the round-trip verification.
        اگر(label_لمبائی==4 || (label_لمبائی>5 && label[label_لمبائی-1]==u'-')) {
            info.labelErrors|=UIDNA_ERROR_INVALID_ACE_LABEL;
            واپس markBadACELabel(dest, label_آغاز, label_لمبائی, مقصد_ASCII, info, error_رمز);
        }
        wasPunycode=سچ;
        char16_t *unicode_بفر=fromPunycode.get_بفر(-1);  // capacity==-1: most labels should fit
        اگر(unicode_بفر==خالی_اشاریہ) {
            // Should never occur if we used capacity==-1 which uses the internal buffer.
            error_رمز=U_حافظہ_تخصیص_ERROR;
            واپس label_لمبائی;
        }
        UError_رمز punycode_Error_رمز=U_ZERO_ERROR;
        int32_t unicode_لمبائی=u_strFromPunycode(label+4, label_لمبائی-4,
                                                unicode_بفر, fromPunycode.get_استعداد(),
                                                خالی_اشاریہ, &punycode_Error_رمز);
        اگر(punycode_Error_رمز==U_بفر_حد_سے_تجاوز_ERROR) {
            fromPunycode.release_بفر(0);
            unicode_بفر=fromPunycode.get_بفر(unicode_لمبائی);
            اگر(unicode_بفر==خالی_اشاریہ) {
                error_رمز=U_حافظہ_تخصیص_ERROR;
                واپس label_لمبائی;
            }
            punycode_Error_رمز=U_ZERO_ERROR;
            unicode_لمبائی=u_strFromPunycode(label+4, label_لمبائی-4,
                                            unicode_بفر, fromPunycode.get_استعداد(),
                                            خالی_اشاریہ, &punycode_Error_رمز);
        }
        fromPunycode.release_بفر(unicode_لمبائی);
        اگر(U_FAILURE(punycode_Error_رمز)) {
            info.labelErrors|=UIDNA_ERROR_PUNYCODE;
            واپس markBadACELabel(dest, label_آغاز, label_لمبائی, مقصد_ASCII, info, error_رمز);
        }
        // Check for NFC, and for characters that are not
        // valid or deviation characters according to the normalizer.
        // If there is something wrong, then the string will change.
        // Note that the normalizer passes through non-LDH ASCII and deviation characters.
        // Deviation characters are ok in Punycode even in transitional processing.
        // In the code further below, if we find non-LDH ASCII and we have UIDNA_USE_STD3_RULES
        // then we will set UIDNA_ERROR_INVALID_ACE_LABEL there too.
        UBool isValid=uts46Norm2.isNormalized(fromPunycode, error_رمز);
        اگر(U_FAILURE(error_رمز)) {
            واپس label_لمبائی;
        }
        // Unicode 15.1 UTS #46:
        // Added an additional condition in 4.1 Validity Criteria to
        // disallow labels such as xn--xn---epa., which do not round-trip.
        // --> Validity Criteria new criterion 4:
        // If not CheckHyphens, the label must not begin with “xn--”.
        اگر(!isValid || fromPunycode.startsWith(UnicodeString::readOnlyAlias(u"xn--"))) {
            info.labelErrors|=UIDNA_ERROR_INVALID_ACE_LABEL;
            واپس markBadACELabel(dest, label_آغاز, label_لمبائی, مقصد_ASCII, info, error_رمز);
        }
        labelString=&fromPunycode;
        label=fromPunycode.get_بفر();
        label_آغاز=0;
        label_لمبائی=fromPunycode.length();
    } ورنہ {
        wasPunycode=جھوٹ;
        labelString=&dest;
    }
    // Validity check
    اگر(label_لمبائی==0) {
        info.labelErrors|=UIDNA_ERROR_خالی_خانے_LABEL;
        واپس replaceLabel(dest, dest_Label_آغاز, dest_Label_لمبائی,
                            *labelString, label_لمبائی, error_رمز);
    }
    // labelLength>0
    اگر(label_لمبائی>=4 && label[2]==0x2d && label[3]==0x2d) {
        // label starts with "??--"
        info.labelErrors|=UIDNA_ERROR_HYPHEN_3_4;
    }
    اگر(label[0]==0x2d) {
        // label starts with "-"
        info.labelErrors|=UIDNA_ERROR_LEADING_HYPHEN;
    }
    اگر(label[label_لمبائی-1]==0x2d) {
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
    ثابت char16_t *حد_بندی=label+label_لمبائی;
    char16_t oredChars=0;
    // If we enforce STD3 rules, then ASCII characters other than LDH and dot are disallowed.
    UBool disallowNonLDHDot=(options&UIDNA_USE_STD3_قواعد)!=0;
    کرو {
        char16_t c=*s;
        اگر(c<=0x7f) {
            اگر(c==0x2e) {
                info.labelErrors|=UIDNA_ERROR_LABEL_HAS_DOT;
                *s=0xfffd;
            } ورنہ اگر(disallowNonLDHDot && ascii_ڈیٹا[c]<0) {
                info.labelErrors|=UIDNA_ERROR_DISALLOWED;
                *s=0xfffd;
            }
        } ورنہ {
            oredChars|=c;
            اگر(c==0xfffd) {
                info.labelErrors|=UIDNA_ERROR_DISALLOWED;
            }
        }
        ++s;
    } جب(s<حد_بندی);
    // Check for a leading combining mark after other validity checks
    // so that we don't report UIDNA_ERROR_DISALLOWED for the U+FFFD from here.
    UChar32 c;
    int32_t cp_لمبائی=0;
    // "Unsafe" is ok because unpaired surrogates were mapped to U+FFFD.
    U16_اگلا_UNSAFE(label, cp_لمبائی, c);
    اگر((U_GET_GC_نقاب(c)&U_GC_M_نقاب)!=0) {
        info.labelErrors|=UIDNA_ERROR_LEADING_COMBINING_MARK;
        labelString->replace(label_آغاز, cp_لمبائی, static_cast<char16_t>(0xfffd));
        label=labelString->get_بفر()+label_آغاز;
        label_لمبائی+=1-cp_لمبائی;
        اگر(labelString==&dest) {
            dest_Label_لمبائی=label_لمبائی;
        }
    }
    اگر((info.labelErrors&severeErrors)==0) {
        // Do contextual checks only if we do not have U+FFFD from a severe error
        // because U+FFFD can make these checks fail.
        اگر((options&UIDNA_CHECK_BIDI)!=0 && (!info.isBiDi || info.isOkBiDi)) {
            checkLabelBiDi(label, label_لمبائی, info);
        }
        اگر( (options&UIDNA_CHECK_CONTEXTJ)!=0 && (oredChars&0x200c)==0x200c &&
            !isLabelOkContextJ(label, label_لمبائی)
        ) {
            info.labelErrors|=UIDNA_ERROR_CONTEXTJ;
        }
        اگر((options&UIDNA_CHECK_CONTEXTO)!=0 && oredChars>=0xb7) {
            checkLabelContextO(label, label_لمبائی, info);
        }
        اگر(مقصد_ASCII) {
            اگر(wasPunycode) {
                // Leave a Punycode label unchanged if it has no severe errors.
                اگر(dest_Label_لمبائی>63) {
                    info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
                }
                واپس dest_Label_لمبائی;
            } ورنہ اگر(oredChars>=0x80) {
                // Contains non-ASCII characters.
                UnicodeString punycode;
                char16_t *بفر=punycode.get_بفر(63);  // 63==maximum DNS label length
                اگر(بفر==خالی_اشاریہ) {
                    error_رمز=U_حافظہ_تخصیص_ERROR;
                    واپس dest_Label_لمبائی;
                }
                بفر[0]=0x78;  // Write "xn--".
                بفر[1]=0x6e;
                بفر[2]=0x2d;
                بفر[3]=0x2d;
                UError_رمز punycode_Error_رمز=U_ZERO_ERROR;
                int32_t punycode_لمبائی=u_str_مقصد_Punycode(label, label_لمبائی,
                                                      بفر+4, punycode.get_استعداد()-4,
                                                      خالی_اشاریہ, &punycode_Error_رمز);
                اگر(punycode_Error_رمز==U_بفر_حد_سے_تجاوز_ERROR) {
                    punycode_Error_رمز=U_ZERO_ERROR;
                    punycode.release_بفر(4);
                    بفر=punycode.get_بفر(4+punycode_لمبائی);
                    اگر(بفر==خالی_اشاریہ) {
                        error_رمز=U_حافظہ_تخصیص_ERROR;
                        واپس dest_Label_لمبائی;
                    }
                    punycode_لمبائی=u_str_مقصد_Punycode(label, label_لمبائی,
                                                  بفر+4, punycode.get_استعداد()-4,
                                                  خالی_اشاریہ, &punycode_Error_رمز);
                }
                punycode_لمبائی+=4;
                punycode.release_بفر(punycode_لمبائی);
                اگر(U_FAILURE(punycode_Error_رمز)) {
                    error_رمز = punycode_Error_رمز;
                    واپس dest_Label_لمبائی;
                }
                اگر(punycode_لمبائی>63) {
                    info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
                }
                واپس replaceLabel(dest, dest_Label_آغاز, dest_Label_لمبائی,
                                    punycode, punycode_لمبائی, error_رمز);
            } ورنہ {
                // all-ASCII label
                اگر(label_لمبائی>63) {
                    info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
                }
            }
        }
    } ورنہ {
        // If a Punycode label has severe errors,
        // then leave it but make sure it does not look valid.
        اگر(wasPunycode) {
            info.labelErrors|=UIDNA_ERROR_INVALID_ACE_LABEL;
            واپس markBadACELabel(dest, dest_Label_آغاز, dest_Label_لمبائی, مقصد_ASCII, info, error_رمز);
        }
    }
    واپس replaceLabel(dest, dest_Label_آغاز, dest_Label_لمبائی,
                        *labelString, label_لمبائی, error_رمز);
}

// Make sure an ACE label does not look valid.
// Append U+FFFD if the label has only LDH characters.
// If UIDNA_USE_STD3_RULES, also replace disallowed ASCII characters with U+FFFD.
int32_t
UTS46::markBadACELabel(UnicodeString &dest,
                       int32_t label_آغاز, int32_t label_لمبائی,
                       UBool مقصد_ASCII, IDNAInfo &info, UError_رمز &error_رمز) ثابت {
    اگر(U_FAILURE(error_رمز)) {
        واپس 0;
    }
    UBool disallowNonLDHDot=(options&UIDNA_USE_STD3_قواعد)!=0;
    UBool isASCII=سچ;
    UBool onlyLDH=سچ;
    ثابت char16_t *label=dest.get_بفر()+label_آغاز;
    ثابت char16_t *حد_بندی=label+label_لمبائی;
    // Start after the initial "xn--".
    // Ok to cast away const because we own the UnicodeString.
    برائے(char16_t *s=const_cast<char16_t *>(label+4); s<حد_بندی; ++s) {
        char16_t c=*s;
        اگر(c<=0x7f) {
            اگر(c==0x2e) {
                info.labelErrors|=UIDNA_ERROR_LABEL_HAS_DOT;
                *s=0xfffd;
                isASCII=onlyLDH=جھوٹ;
            } ورنہ اگر(ascii_ڈیٹا[c]<0) {
                onlyLDH=جھوٹ;
                اگر(disallowNonLDHDot) {
                    *s=0xfffd;
                    isASCII=جھوٹ;
                }
            }
        } ورنہ {
            isASCII=onlyLDH=جھوٹ;
        }
    }
    اگر(onlyLDH) {
        dest.insert(label_آغاز + label_لمبائی, static_cast<char16_t>(0xfffd));
        اگر(dest.isBogus()) {
            error_رمز=U_حافظہ_تخصیص_ERROR;
            واپس 0;
        }
        ++label_لمبائی;
    } ورنہ {
        اگر(مقصد_ASCII && isASCII && label_لمبائی>63) {
            info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
        }
    }
    واپس label_لمبائی;
}

ثابت uint32_t L_نقاب=U_نقاب(U_بایاں_مقصد_دایاں);
ثابت uint32_t R_AL_نقاب=U_نقاب(U_دایاں_مقصد_بایاں)|U_نقاب(U_دایاں_مقصد_بایاں_ARABIC);
ثابت uint32_t L_R_AL_نقاب=L_نقاب|R_AL_نقاب;

ثابت uint32_t R_AL_AN_نقاب=R_AL_نقاب|U_نقاب(U_ARABIC_نمبر);

ثابت uint32_t EN_AN_نقاب=U_نقاب(U_EUROPEAN_نمبر)|U_نقاب(U_ARABIC_نمبر);
ثابت uint32_t R_AL_EN_AN_نقاب=R_AL_نقاب|EN_AN_نقاب;
ثابت uint32_t L_EN_نقاب=L_نقاب|U_نقاب(U_EUROPEAN_نمبر);

ثابت uint32_t ES_CS_ET_ON_BN_NSM_نقاب=
    U_نقاب(U_EUROPEAN_نمبر_SEPARATOR)|
    U_نقاب(U_COMMON_نمبر_SEPARATOR)|
    U_نقاب(U_EUROPEAN_نمبر_TERMINATOR)|
    U_نقاب(U_OTHER_NEUTRAL)|
    U_نقاب(U_BOUNDARY_NEUTRAL)|
    U_نقاب(U_DIR_NON_SPACING_MARK);
ثابت uint32_t L_EN_ES_CS_ET_ON_BN_NSM_نقاب=L_EN_نقاب|ES_CS_ET_ON_BN_NSM_نقاب;
ثابت uint32_t R_AL_AN_EN_ES_CS_ET_ON_BN_NSM_نقاب=R_AL_نقاب|EN_AN_نقاب|ES_CS_ET_ON_BN_NSM_نقاب;

// We scan the whole label and check both for whether it contains RTL characters
// and whether it passes the BiDi Rule.
// In a BiDi domain name, all labels must pass the BiDi Rule, but we might find
// that a domain name is a BiDi domain name (has an RTL label) only after
// processing several earlier labels.
خالی
UTS46::checkLabelBiDi(ثابت char16_t *label, int32_t label_لمبائی, IDNAInfo &info) ثابت {
    // IDNA2008 BiDi rule
    // Get the directionality of the first character.
    UChar32 c;
    int32_t i=0;
    U16_اگلا_UNSAFE(label, i, c);
    uint32_t پہلا_نقاب=U_نقاب(u_charDirection(c));
    // 1. The first character must be a character with BIDI property L, R
    // or AL.  If it has the R or AL property, it is an RTL label; if it
    // has the L property, it is an LTR label.
    اگر((پہلا_نقاب&~L_R_AL_نقاب)!=0) {
        info.isOkBiDi=جھوٹ;
    }
    // Get the directionality of the last non-NSM character.
    uint32_t آخری_نقاب;
    برائے(;;) {
        اگر(i>=label_لمبائی) {
            آخری_نقاب=پہلا_نقاب;
            توڑو;
        }
        U16_پچھلا_UNSAFE(label, label_لمبائی, c);
        UCharDirection dir=u_charDirection(c);
        اگر(dir!=U_DIR_NON_SPACING_MARK) {
            آخری_نقاب=U_نقاب(dir);
            توڑو;
        }
    }
    // 3. In an RTL label, the end of the label must be a character with
    // BIDI property R, AL, EN or AN, followed by zero or more
    // characters with BIDI property NSM.
    // 6. In an LTR label, the end of the label must be a character with
    // BIDI property L or EN, followed by zero or more characters with
    // BIDI property NSM.
    اگر( (پہلا_نقاب&L_نقاب)!=0 ?
            (آخری_نقاب&~L_EN_نقاب)!=0 :
            (آخری_نقاب&~R_AL_EN_AN_نقاب)!=0
    ) {
        info.isOkBiDi=جھوٹ;
    }
    // Add the directionalities of the intervening characters.
    uint32_t mask=پہلا_نقاب|آخری_نقاب;
    جب(i<label_لمبائی) {
        U16_اگلا_UNSAFE(label, i, c);
        mask|=U_نقاب(u_charDirection(c));
    }
    اگر(پہلا_نقاب&L_نقاب) {
        // 5. In an LTR label, only characters with the BIDI properties L, EN,
        // ES, CS, ET, ON, BN and NSM are allowed.
        اگر((mask&~L_EN_ES_CS_ET_ON_BN_NSM_نقاب)!=0) {
            info.isOkBiDi=جھوٹ;
        }
    } ورنہ {
        // 2. In an RTL label, only characters with the BIDI properties R, AL,
        // AN, EN, ES, CS, ET, ON, BN and NSM are allowed.
        اگر((mask&~R_AL_AN_EN_ES_CS_ET_ON_BN_NSM_نقاب)!=0) {
            info.isOkBiDi=جھوٹ;
        }
        // 4. In an RTL label, if an EN is present, no AN may be present, and
        // vice versa.
        اگر((mask&EN_AN_نقاب)==EN_AN_نقاب) {
            info.isOkBiDi=جھوٹ;
        }
    }
    // An RTL label is a label that contains at least one character of type
    // R, AL or AN. [...]
    // A "BIDI domain name" is a domain name that contains at least one RTL
    // label. [...]
    // The following rule, consisting of six conditions, applies to labels
    // in BIDI domain names.
    اگر((mask&R_AL_AN_نقاب)!=0) {
        info.isBiDi=سچ;
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
ساکن UBool
isASCIIOkBiDi(ثابت char16_t *s, int32_t length) {
    int32_t label_آغاز=0;
    برائے(int32_t i=0; i<length; ++i) {
        char16_t c=s[i];
        اگر(c==0x2e) {  // dot
            اگر(i>label_آغاز) {
                c=s[i-1];
                اگر(!(0x61<=c && c<=0x7a) && !(0x30<=c && c<=0x39)) {
                    // Last character in the label is not an L or EN.
                    واپس جھوٹ;
                }
            }
            label_آغاز=i+1;
        } ورنہ اگر(i==label_آغاز) {
            اگر(!(0x61<=c && c<=0x7a)) {
                // First character in the label is not an L.
                واپس جھوٹ;
            }
        } ورنہ {
            اگر(c<=0x20 && (c>=0x1c || (9<=c && c<=0xd))) {
                // Intermediate character in the label is a B, S or WS.
                واپس جھوٹ;
            }
        }
    }
    واپس سچ;
}

// UTF-8 version, called for source ASCII prefix.
// Can contain uppercase A-Z.
// s[length-1] must be the trailing dot.
ساکن UBool
isASCIIOkBiDi(ثابت حرف *s, int32_t length) {
    int32_t label_آغاز=0;
    برائے(int32_t i=0; i<length; ++i) {
        حرف c=s[i];
        اگر(c==0x2e) {  // dot
            اگر(i>label_آغاز) {
                c=s[i-1];
                اگر(!(0x61<=c && c<=0x7a) && !(0x41<=c && c<=0x5a) && !(0x30<=c && c<=0x39)) {
                    // Last character in the label is not an L or EN.
                    واپس جھوٹ;
                }
            }
            label_آغاز=i+1;
        } ورنہ اگر(i==label_آغاز) {
            اگر(!(0x61<=c && c<=0x7a) && !(0x41<=c && c<=0x5a)) {
                // First character in the label is not an L.
                واپس جھوٹ;
            }
        } ورنہ {
            اگر(c<=0x20 && (c>=0x1c || (9<=c && c<=0xd))) {
                // Intermediate character in the label is a B, S or WS.
                واپس جھوٹ;
            }
        }
    }
    واپس سچ;
}

UBool
UTS46::isLabelOkContextJ(ثابت char16_t *label, int32_t label_لمبائی) ثابت {
    // [IDNA2008-Tables]
    // 200C..200D  ; CONTEXTJ    # ZERO WIDTH NON-JOINER..ZERO WIDTH JOINER
    برائے(int32_t i=0; i<label_لمبائی; ++i) {
        اگر(label[i]==0x200c) {
            // Appendix A.1. ZERO WIDTH NON-JOINER
            // Rule Set:
            //  False;
            //  If Canonical_Combining_Class(Before(cp)) .eq.  Virama Then True;
            //  If RegExpMatch((Joining_Type:{L,D})(Joining_Type:T)*\u200C
            //     (Joining_Type:T)*(Joining_Type:{R,D})) Then True;
            اگر(i==0) {
                واپس جھوٹ;
            }
            UChar32 c;
            int32_t j=i;
            U16_پچھلا_UNSAFE(label, j, c);
            اگر(uts46Norm2.getCombiningClass(c)==9) {
                جاری;
            }
            // check precontext (Joining_Type:{L,D})(Joining_Type:T)*
            برائے(;;) {
                UJoiningType type=ubidi_getJoiningType(c);
                اگر(type==U_JT_TRANSPARENT) {
                    اگر(j==0) {
                        واپس جھوٹ;
                    }
                    U16_پچھلا_UNSAFE(label, j, c);
                } ورنہ اگر(type==U_JT_بایاں_JOINING || type==U_JT_DUAL_JOINING) {
                    توڑو;  // precontext fulfilled
                } ورنہ {
                    واپس جھوٹ;
                }
            }
            // check postcontext (Joining_Type:T)*(Joining_Type:{R,D})
            برائے(j=i+1;;) {
                اگر(j==label_لمبائی) {
                    واپس جھوٹ;
                }
                U16_اگلا_UNSAFE(label, j, c);
                UJoiningType type=ubidi_getJoiningType(c);
                اگر(type==U_JT_TRANSPARENT) {
                    // just skip this character
                } ورنہ اگر(type==U_JT_دایاں_JOINING || type==U_JT_DUAL_JOINING) {
                    توڑو;  // postcontext fulfilled
                } ورنہ {
                    واپس جھوٹ;
                }
            }
        } ورنہ اگر(label[i]==0x200d) {
            // Appendix A.2. ZERO WIDTH JOINER (U+200D)
            // Rule Set:
            //  False;
            //  If Canonical_Combining_Class(Before(cp)) .eq.  Virama Then True;
            اگر(i==0) {
                واپس جھوٹ;
            }
            UChar32 c;
            int32_t j=i;
            U16_پچھلا_UNSAFE(label, j, c);
            اگر(uts46Norm2.getCombiningClass(c)!=9) {
                واپس جھوٹ;
            }
        }
    }
    واپس سچ;
}

خالی
UTS46::checkLabelContextO(ثابت char16_t *label, int32_t label_لمبائی, IDNAInfo &info) ثابت {
    int32_t label_انتہا=label_لمبائی-1;  // inclusive
    int32_t arabicDigits=0;  // -1 for 066x, +1 for 06Fx
    برائے(int32_t i=0; i<=label_انتہا; ++i) {
        UChar32 c=label[i];
        اگر(c<0xb7) {
            // ASCII fastpath
        } ورنہ اگر(c<=0x6f9) {
            اگر(c==0xb7) {
                // Appendix A.3. MIDDLE DOT (U+00B7)
                // Rule Set:
                //  False;
                //  If Before(cp) .eq.  U+006C And
                //     After(cp) .eq.  U+006C Then True;
                اگر(!(0<i && label[i-1]==0x6c &&
                     i<label_انتہا && label[i+1]==0x6c)) {
                    info.labelErrors|=UIDNA_ERROR_CONTEXTO_PUNCTUATION;
                }
            } ورنہ اگر(c==0x375) {
                // Appendix A.4. GREEK LOWER NUMERAL SIGN (KERAIA) (U+0375)
                // Rule Set:
                //  False;
                //  If Script(After(cp)) .eq.  Greek Then True;
                UScript_رمز script=USCRIPT_INVALID_رمز;
                اگر(i<label_انتہا) {
                    UError_رمز error_رمز=U_ZERO_ERROR;
                    int32_t j=i+1;
                    U16_اگلا(label, j, label_لمبائی, c);
                    script=uscript_getScript(c, &error_رمز);
                }
                اگر(script!=USCRIPT_GREEK) {
                    info.labelErrors|=UIDNA_ERROR_CONTEXTO_PUNCTUATION;
                }
            } ورنہ اگر(c==0x5f3 || c==0x5f4) {
                // Appendix A.5. HEBREW PUNCTUATION GERESH (U+05F3)
                // Rule Set:
                //  False;
                //  If Script(Before(cp)) .eq.  Hebrew Then True;
                //
                // Appendix A.6. HEBREW PUNCTUATION GERSHAYIM (U+05F4)
                // Rule Set:
                //  False;
                //  If Script(Before(cp)) .eq.  Hebrew Then True;
                UScript_رمز script=USCRIPT_INVALID_رمز;
                اگر(0<i) {
                    UError_رمز error_رمز=U_ZERO_ERROR;
                    int32_t j=i;
                    U16_پچھلا(label, 0, j, c);
                    script=uscript_getScript(c, &error_رمز);
                }
                اگر(script!=USCRIPT_HEBREW) {
                    info.labelErrors|=UIDNA_ERROR_CONTEXTO_PUNCTUATION;
                }
            } ورنہ اگر(0x660<=c /* && c<=0x6f9 */) {
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
                } ورنہ اگر(0x6f0<=c) {
                    اگر(arabicDigits<0) {
                        info.labelErrors|=UIDNA_ERROR_CONTEXTO_DIGITS;
                    }
                    arabicDigits=1;
                }
            }
        } ورنہ اگر(c==0x30fb) {
            // Appendix A.7. KATAKANA MIDDLE DOT (U+30FB)
            // Rule Set:
            //  False;
            //  For All Characters:
            //    If Script(cp) .in. {Hiragana, Katakana, Han} Then True;
            //  End For;
            UError_رمز error_رمز=U_ZERO_ERROR;
            برائے(صحیح j=0;;) {
                اگر(j>label_انتہا) {
                    info.labelErrors|=UIDNA_ERROR_CONTEXTO_PUNCTUATION;
                    توڑو;
                }
                U16_اگلا(label, j, label_لمبائی, c);
                UScript_رمز script=uscript_getScript(c, &error_رمز);
                اگر(script==USCRIPT_HIRAGANA || script==USCRIPT_KATAKANA || script==USCRIPT_HAN) {
                    توڑو;
                }
            }
        }
    }
}

U_NAMESPACE_انتہا

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

U_CAPI UIDNA * U_EXPORT2
uidna_openUTS46(uint32_t options, UError_رمز *p_Error_رمز) {
    واپس reinterpret_cast<UIDNA *>(IDNA::createUTS46Instance(options, *p_Error_رمز));
}

U_CAPI خالی U_EXPORT2
uidna_close(UIDNA *idna) {
    حذف reinterpret_cast<IDNA *>(idna);
}

ساکن UBool
check_دلائل(ثابت خالی *label, int32_t length,
          خالی *dest, int32_t capacity,
          UIDNAInfo *pInfo, UError_رمز *p_Error_رمز) {
    اگر(U_FAILURE(*p_Error_رمز)) {
        واپس جھوٹ;
    }
    // sizeof(UIDNAInfo)=16 in the first API version.
    اگر(pInfo==خالی_اشاریہ || pInfo->size<16) {
        *p_Error_رمز=U_ILLEGAL_دلیل_ERROR;
        واپس جھوٹ;
    }
    اگر( (label==خالی_اشاریہ ? length!=0 : length<-1) ||
        (dest==خالی_اشاریہ ? capacity!=0 : capacity<0) ||
        (dest==label && label!=خالی_اشاریہ)
    ) {
        *p_Error_رمز=U_ILLEGAL_دلیل_ERROR;
        واپس جھوٹ;
    }
    // Set all *pInfo bytes to 0 except for the size field itself.
    uprv_memset(&pInfo->size+1, 0, pInfo->size-حجم(pInfo->size));
    واپس سچ;
}

ساکن خالی
idna_Info_مقصد_Struct(IDNAInfo &info, UIDNAInfo *pInfo) {
    pInfo->isTransitionalDifferent=info.isTransitionalDifferent();
    pInfo->errors=info.getErrors();
}

U_CAPI int32_t U_EXPORT2
uidna_label_مقصد_ASCII(ثابت UIDNA *idna,
                   ثابت char16_t *label, int32_t length,
                   char16_t *dest, int32_t capacity,
                   UIDNAInfo *pInfo, UError_رمز *p_Error_رمز) {
    اگر(!check_دلائل(label, length, dest, capacity, pInfo, p_Error_رمز)) {
        واپس 0;
    }
    UnicodeString src(length < 0, label, length);
    UnicodeString destString(dest, 0, capacity);
    IDNAInfo info;
    reinterpret_cast<ثابت IDNA *>(idna)->label_مقصد_ASCII(src, destString, info, *p_Error_رمز);
    idna_Info_مقصد_Struct(info, pInfo);
    واپس destString.extract(dest, capacity, *p_Error_رمز);
}

U_CAPI int32_t U_EXPORT2
uidna_label_مقصد_Unicode(ثابت UIDNA *idna,
                     ثابت char16_t *label, int32_t length,
                     char16_t *dest, int32_t capacity,
                     UIDNAInfo *pInfo, UError_رمز *p_Error_رمز) {
    اگر(!check_دلائل(label, length, dest, capacity, pInfo, p_Error_رمز)) {
        واپس 0;
    }
    UnicodeString src(length < 0, label, length);
    UnicodeString destString(dest, 0, capacity);
    IDNAInfo info;
    reinterpret_cast<ثابت IDNA *>(idna)->label_مقصد_Unicode(src, destString, info, *p_Error_رمز);
    idna_Info_مقصد_Struct(info, pInfo);
    واپس destString.extract(dest, capacity, *p_Error_رمز);
}

U_CAPI int32_t U_EXPORT2
uidna_name_مقصد_ASCII(ثابت UIDNA *idna,
                  ثابت char16_t *name, int32_t length,
                  char16_t *dest, int32_t capacity,
                  UIDNAInfo *pInfo, UError_رمز *p_Error_رمز) {
    اگر(!check_دلائل(name, length, dest, capacity, pInfo, p_Error_رمز)) {
        واپس 0;
    }
    UnicodeString src(length < 0, name, length);
    UnicodeString destString(dest, 0, capacity);
    IDNAInfo info;
    reinterpret_cast<ثابت IDNA *>(idna)->name_مقصد_ASCII(src, destString, info, *p_Error_رمز);
    idna_Info_مقصد_Struct(info, pInfo);
    واپس destString.extract(dest, capacity, *p_Error_رمز);
}

U_CAPI int32_t U_EXPORT2
uidna_name_مقصد_Unicode(ثابت UIDNA *idna,
                    ثابت char16_t *name, int32_t length,
                    char16_t *dest, int32_t capacity,
                    UIDNAInfo *pInfo, UError_رمز *p_Error_رمز) {
    اگر(!check_دلائل(name, length, dest, capacity, pInfo, p_Error_رمز)) {
        واپس 0;
    }
    UnicodeString src(length < 0, name, length);
    UnicodeString destString(dest, 0, capacity);
    IDNAInfo info;
    reinterpret_cast<ثابت IDNA *>(idna)->name_مقصد_Unicode(src, destString, info, *p_Error_رمز);
    idna_Info_مقصد_Struct(info, pInfo);
    واپس destString.extract(dest, capacity, *p_Error_رمز);
}

U_CAPI int32_t U_EXPORT2
uidna_label_مقصد_ASCII_UTF8(ثابت UIDNA *idna,
                        ثابت حرف *label, int32_t length,
                        حرف *dest, int32_t capacity,
                        UIDNAInfo *pInfo, UError_رمز *p_Error_رمز) {
    اگر(!check_دلائل(label, length, dest, capacity, pInfo, p_Error_رمز)) {
        واپس 0;
    }
    StringPiece src(label, length<0 ? static_cast<int32_t>(uprv_strlen(label)) : length);
    واپس ByteSinkUtil::بذریعہ_Byte_Sink_مقصد_Terminated_Chars(
        dest, capacity,
        [&](ByteSink& sink, UError_رمز& حالت_2) {
            IDNAInfo info;
            reinterpret_cast<ثابت IDNA *>(idna)->label_مقصد_ASCII_UTF8(src, sink, info, حالت_2);
            idna_Info_مقصد_Struct(info, pInfo);
        },
        *p_Error_رمز);
}

U_CAPI int32_t U_EXPORT2
uidna_label_مقصد_Unicode_UTF8(ثابت UIDNA *idna,
                         ثابت حرف *label, int32_t length,
                         حرف *dest, int32_t capacity,
                         UIDNAInfo *pInfo, UError_رمز *p_Error_رمز) {
    اگر(!check_دلائل(label, length, dest, capacity, pInfo, p_Error_رمز)) {
        واپس 0;
    }
    StringPiece src(label, length<0 ? static_cast<int32_t>(uprv_strlen(label)) : length);
    واپس ByteSinkUtil::بذریعہ_Byte_Sink_مقصد_Terminated_Chars(
        dest, capacity,
        [&](ByteSink& sink, UError_رمز& حالت_2) {
            IDNAInfo info;
            reinterpret_cast<ثابت IDNA *>(idna)->label_مقصد_Unicode_UTF8(src, sink, info, حالت_2);
            idna_Info_مقصد_Struct(info, pInfo);
        },
        *p_Error_رمز);
}

U_CAPI int32_t U_EXPORT2
uidna_name_مقصد_ASCII_UTF8(ثابت UIDNA *idna,
                       ثابت حرف *name, int32_t length,
                       حرف *dest, int32_t capacity,
                       UIDNAInfo *pInfo, UError_رمز *p_Error_رمز) {
    اگر(!check_دلائل(name, length, dest, capacity, pInfo, p_Error_رمز)) {
        واپس 0;
    }
    StringPiece src(name, length<0 ? static_cast<int32_t>(uprv_strlen(name)) : length);
    واپس ByteSinkUtil::بذریعہ_Byte_Sink_مقصد_Terminated_Chars(
        dest, capacity,
        [&](ByteSink& sink, UError_رمز& حالت_2) {
            IDNAInfo info;
            reinterpret_cast<ثابت IDNA *>(idna)->name_مقصد_ASCII_UTF8(src, sink, info, حالت_2);
            idna_Info_مقصد_Struct(info, pInfo);
        },
        *p_Error_رمز);
}

U_CAPI int32_t U_EXPORT2
uidna_name_مقصد_Unicode_UTF8(ثابت UIDNA *idna,
                        ثابت حرف *name, int32_t length,
                        حرف *dest, int32_t capacity,
                        UIDNAInfo *pInfo, UError_رمز *p_Error_رمز) {
    اگر(!check_دلائل(name, length, dest, capacity, pInfo, p_Error_رمز)) {
        واپس 0;
    }
    StringPiece src(name, length<0 ? static_cast<int32_t>(uprv_strlen(name)) : length);
    واپس ByteSinkUtil::بذریعہ_Byte_Sink_مقصد_Terminated_Chars(
        dest, capacity,
        [&](ByteSink& sink, UError_رمز& حالت_2) {
            IDNAInfo info;
            reinterpret_cast<ثابت IDNA *>(idna)->name_مقصد_Unicode_UTF8(src, sink, info, حالت_2);
            idna_Info_مقصد_Struct(info, pInfo);
        },
        *p_Error_رمز);
}

#اگر_ختم  // UCONFIG_NO_IDNA
