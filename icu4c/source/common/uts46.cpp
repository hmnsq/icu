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
    ثابت char16_t *s=dest.get_میانگیر();
    ثابت char16_t *حد=s+dest.length();
    طالما(s<حد) {
        إذا(*s++>0x7f) {
            أرجع خطأ;
        }
    }
    أرجع درست;
}

ساكن UBool
isASCIIOkBiDi(ثابت char16_t *s, int32_t length);

ساكن UBool
isASCIIOkBiDi(ثابت حرف *s, int32_t length);

// IDNA class default implementations -------------------------------------- ***

IDNA::~IDNA() {}

فراغ
IDNA::label_مقصد_ASCII_UTF8(StringPiece label, ByteSink &dest,
                        IDNAInfo &info, UError_کد &error_کد) ثابت {
    إذا(U_SUCCESS(error_کد)) {
        UnicodeString destString;
        label_مقصد_ASCII(UnicodeString::fromUTF8(label), destString,
                     info, error_کد).مقصد_UTF8(dest);
    }
}

فراغ
IDNA::label_مقصد_Unicode_UTF8(StringPiece label, ByteSink &dest,
                         IDNAInfo &info, UError_کد &error_کد) ثابت {
    إذا(U_SUCCESS(error_کد)) {
        UnicodeString destString;
        label_مقصد_Unicode(UnicodeString::fromUTF8(label), destString,
                       info, error_کد).مقصد_UTF8(dest);
    }
}

فراغ
IDNA::name_مقصد_ASCII_UTF8(StringPiece name, ByteSink &dest,
                       IDNAInfo &info, UError_کد &error_کد) ثابت {
    إذا(U_SUCCESS(error_کد)) {
        UnicodeString destString;
        name_مقصد_ASCII(UnicodeString::fromUTF8(name), destString,
                    info, error_کد).مقصد_UTF8(dest);
    }
}

فراغ
IDNA::name_مقصد_Unicode_UTF8(StringPiece name, ByteSink &dest,
                        IDNAInfo &info, UError_کد &error_کد) ثابت {
    إذا(U_SUCCESS(error_کد)) {
        UnicodeString destString;
        name_مقصد_Unicode(UnicodeString::fromUTF8(name), destString,
                      info, error_کد).مقصد_UTF8(dest);
    }
}

// UTS46 class declaration ------------------------------------------------- ***

صنف UTS46 : عام IDNA {
عام:
    UTS46(uint32_t options, UError_کد &error_کد);
    مجازی ~UTS46();

    مجازی UnicodeString&
    label_مقصد_ASCII(ثابت UnicodeString& label,
                 UnicodeString& dest U_LIFETIME_BOUND,
                 IDNAInfo& info,
                 UError_کد& error_کد) ثابت override;

    مجازی UnicodeString&
    label_مقصد_Unicode(ثابت UnicodeString& label,
                   UnicodeString& dest U_LIFETIME_BOUND,
                   IDNAInfo& info,
                   UError_کد& error_کد) ثابت override;

    مجازی UnicodeString&
    name_مقصد_ASCII(ثابت UnicodeString& name,
                UnicodeString& dest U_LIFETIME_BOUND,
                IDNAInfo& info,
                UError_کد& error_کد) ثابت override;

    مجازی UnicodeString&
    name_مقصد_Unicode(ثابت UnicodeString& name,
                  UnicodeString& dest U_LIFETIME_BOUND,
                  IDNAInfo& info,
                  UError_کد& error_کد) ثابت override;

    مجازی فراغ
    label_مقصد_ASCII_UTF8(StringPiece label, ByteSink &dest,
                      IDNAInfo &info, UError_کد &error_کد) ثابت override;

    مجازی فراغ
    label_مقصد_Unicode_UTF8(StringPiece label, ByteSink &dest,
                       IDNAInfo &info, UError_کد &error_کد) ثابت override;

    مجازی فراغ
    name_مقصد_ASCII_UTF8(StringPiece name, ByteSink &dest,
                     IDNAInfo &info, UError_کد &error_کد) ثابت override;

    مجازی فراغ
    name_مقصد_Unicode_UTF8(StringPiece name, ByteSink &dest,
                      IDNAInfo &info, UError_کد &error_کد) ثابت override;

خاص:
    UnicodeString&
    process(ثابت UnicodeString& src,
            UBool isLabel,
            UBool مقصد_ASCII,
            UnicodeString& dest U_LIFETIME_BOUND,
            IDNAInfo& info,
            UError_کد& error_کد) ثابت;

    فراغ
    processUTF8(StringPiece src,
                UBool isLabel, UBool مقصد_ASCII,
                ByteSink &dest,
                IDNAInfo &info, UError_کد &error_کد) ثابت;

    UnicodeString&
    processUnicode(ثابت UnicodeString& src,
                   int32_t label_شروع,
                   int32_t mapping_شروع,
                   UBool isLabel,
                   UBool مقصد_ASCII,
                   UnicodeString& dest U_LIFETIME_BOUND,
                   IDNAInfo& info,
                   UError_کد& error_کد) ثابت;

    // returns the new dest.length()
    int32_t
    mapDevChars(UnicodeString &dest, int32_t label_شروع, int32_t mapping_شروع,
                UError_کد &error_کد) ثابت;

    // returns the new label length
    int32_t
    processLabel(UnicodeString &dest,
                 int32_t label_شروع, int32_t label_درازی,
                 UBool مقصد_ASCII,
                 IDNAInfo &info, UError_کد &error_کد) ثابت;
    int32_t
    markBadACELabel(UnicodeString &dest,
                    int32_t label_شروع, int32_t label_درازی,
                    UBool مقصد_ASCII, IDNAInfo &info, UError_کد &error_کد) ثابت;

    فراغ
    checkLabelBiDi(ثابت char16_t *label, int32_t label_درازی, IDNAInfo &info) ثابت;

    UBool
    isLabelOkContextJ(ثابت char16_t *label, int32_t label_درازی) ثابت;

    فراغ
    checkLabelContextO(ثابت char16_t *label, int32_t label_درازی, IDNAInfo &info) ثابت;

    ثابت Normalizer2 &uts46Norm2;  // uts46.nrm
    uint32_t options;
};

IDNA *
IDNA::createUTS46Instance(uint32_t options, UError_کد &error_کد) {
    إذا(U_SUCCESS(error_کد)) {
        IDNA *idna=جديد UTS46(options, error_کد);
        إذا(idna==اشاره_تهی) {
            error_کد=U_حافظه_تخصیص_ERROR;
        } وإلا إذا(U_FAILURE(error_کد)) {
            حذف idna;
            idna=اشاره_تهی;
        }
        أرجع idna;
    } وإلا {
        أرجع اشاره_تهی;
    }
}

// UTS46 implementation ---------------------------------------------------- ***

UTS46::UTS46(uint32_t opt, UError_کد &error_کد)
        : uts46Norm2(*Normalizer2::getInstance(اشاره_تهی, "uts46", UNORM2_COMPOSE, error_کد)),
          options(opt) {}

UTS46::~UTS46() {}

UnicodeString&
UTS46::label_مقصد_ASCII(ثابت UnicodeString& label,
                    UnicodeString& dest U_LIFETIME_BOUND,
                    IDNAInfo& info,
                    UError_کد& error_کد) ثابت {
    أرجع process(label, درست, درست, dest, info, error_کد);
}

UnicodeString&
UTS46::label_مقصد_Unicode(ثابت UnicodeString& label,
                      UnicodeString& dest U_LIFETIME_BOUND,
                      IDNAInfo& info,
                      UError_کد& error_کد) ثابت {
    أرجع process(label, درست, خطأ, dest, info, error_کد);
}

UnicodeString&
UTS46::name_مقصد_ASCII(ثابت UnicodeString& name,
                   UnicodeString& dest U_LIFETIME_BOUND,
                   IDNAInfo& info,
                   UError_کد& error_کد) ثابت {
    process(name, خطأ, درست, dest, info, error_کد);
    إذا( dest.length()>=254 && (info.errors&UIDNA_ERROR_DOMAIN_NAME_TOO_LONG)==0 &&
        isASCIIString(dest) &&
        (dest.length()>254 || dest[253]!=0x2e)
    ) {
        info.errors|=UIDNA_ERROR_DOMAIN_NAME_TOO_LONG;
    }
    أرجع dest;
}

UnicodeString&
UTS46::name_مقصد_Unicode(ثابت UnicodeString& name,
                     UnicodeString& dest U_LIFETIME_BOUND,
                     IDNAInfo& info,
                     UError_کد& error_کد) ثابت {
    أرجع process(name, خطأ, خطأ, dest, info, error_کد);
}

فراغ
UTS46::label_مقصد_ASCII_UTF8(StringPiece label, ByteSink &dest,
                         IDNAInfo &info, UError_کد &error_کد) ثابت {
    processUTF8(label, درست, درست, dest, info, error_کد);
}

فراغ
UTS46::label_مقصد_Unicode_UTF8(StringPiece label, ByteSink &dest,
                          IDNAInfo &info, UError_کد &error_کد) ثابت {
    processUTF8(label, درست, خطأ, dest, info, error_کد);
}

فراغ
UTS46::name_مقصد_ASCII_UTF8(StringPiece name, ByteSink &dest,
                        IDNAInfo &info, UError_کد &error_کد) ثابت {
    processUTF8(name, خطأ, درست, dest, info, error_کد);
}

فراغ
UTS46::name_مقصد_Unicode_UTF8(StringPiece name, ByteSink &dest,
                         IDNAInfo &info, UError_کد &error_کد) ثابت {
    processUTF8(name, خطأ, خطأ, dest, info, error_کد);
}

// UTS #46 data for ASCII characters.
// The normalizer (using uts46.nrm) maps uppercase ASCII letters to lowercase
// and passes through all other ASCII characters.
// If UIDNA_USE_STD3_RULES is set, then non-LDH characters are disallowed
// using this data.
// The ASCII fastpath also uses this data.
// Values: -1=disallowed  0==valid  1==mapped (lowercase)
ساكن ثابت int8_t ascii_داده[128]={
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
               UError_کد& error_کد) ثابت {
    // uts46Norm2.normalize() would do all of this error checking and setup,
    // but with the ASCII fastpath we do not always call it, and do not
    // call it first.
    إذا(U_FAILURE(error_کد)) {
        dest.مجموعه_مقصد_Bogus();
        أرجع dest;
    }
    ثابت char16_t *srcArray=src.get_میانگیر();
    إذا(&dest==&src || srcArray==اشاره_تهی) {
        error_کد=U_ILLEGAL_آرگومان_ERROR;
        dest.مجموعه_مقصد_Bogus();
        أرجع dest;
    }
    // Arguments are fine, reset output values.
    dest.remove();
    info.reset();
    int32_t src_درازی=src.length();
    إذا(src_درازی==0) {
        info.errors|=UIDNA_ERROR_خالی_LABEL;
        أرجع dest;
    }
    char16_t *destArray=dest.get_میانگیر(src_درازی);
    إذا(destArray==اشاره_تهی) {
        error_کد=U_حافظه_تخصیص_ERROR;
        أرجع dest;
    }
    // ASCII fastpath
    UBool disallowNonLDHDot=(options&UIDNA_USE_STD3_قواعد)!=0;
    int32_t label_شروع=0;
    int32_t i;
    لكل(i=0;; ++i) {
        إذا(i==src_درازی) {
            إذا(مقصد_ASCII) {
                إذا((i-label_شروع)>63) {
                    info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
                }
                // There is a trailing dot if labelStart==i.
                إذا(!isLabel && i>=254 && (i>254 || label_شروع<i)) {
                    info.errors|=UIDNA_ERROR_DOMAIN_NAME_TOO_LONG;
                }
            }
            info.errors|=info.labelErrors;
            dest.release_میانگیر(i);
            أرجع dest;
        }
        char16_t c=srcArray[i];
        إذا(c>0x7f) {
            اكسر;
        }
        صحيح c_داده=ascii_داده[c];
        إذا(c_داده>0) {
            destArray[i]=c+0x20;  // Lowercase an uppercase ASCII letter.
        } وإلا إذا(c_داده<0 && disallowNonLDHDot) {
            اكسر;  // Replacing with U+FFFD can be complicated for toASCII.
        } وإلا {
            destArray[i]=c;
            إذا(c==0x2d) {  // hyphen
                إذا(i==(label_شروع+3) && srcArray[i-1]==0x2d) {
                    // "??--..." is Punycode or forbidden.
                    ++i;  // '-' was copied to dest already
                    اكسر;
                }
                إذا(i==label_شروع) {
                    // label starts with "-"
                    info.labelErrors|=UIDNA_ERROR_LEADING_HYPHEN;
                }
                إذا((i+1)==src_درازی || srcArray[i+1]==0x2e) {
                    // label ends with "-"
                    info.labelErrors|=UIDNA_ERROR_TRAILING_HYPHEN;
                }
            } وإلا إذا(c==0x2e) {  // dot
                إذا(isLabel) {
                    // Replacing with U+FFFD can be complicated for toASCII.
                    ++i;  // '.' was copied to dest already
                    اكسر;
                }
                إذا(i==label_شروع) {
                    info.labelErrors|=UIDNA_ERROR_خالی_LABEL;
                }
                إذا(مقصد_ASCII && (i-label_شروع)>63) {
                    info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
                }
                info.errors|=info.labelErrors;
                info.labelErrors=0;
                label_شروع=i+1;
            }
        }
    }
    info.errors|=info.labelErrors;
    dest.release_میانگیر(i);
    processUnicode(src, label_شروع, i, isLabel, مقصد_ASCII, dest, info, error_کد);
    إذا( info.isBiDi && U_SUCCESS(error_کد) && (info.errors&severeErrors)==0 &&
        (!info.isOkBiDi || (label_شروع>0 && !isASCIIOkBiDi(dest.get_میانگیر(), label_شروع)))
    ) {
        info.errors|=UIDNA_ERROR_BIDI;
    }
    أرجع dest;
}

فراغ
UTS46::processUTF8(StringPiece src,
                   UBool isLabel, UBool مقصد_ASCII,
                   ByteSink &dest,
                   IDNAInfo &info, UError_کد &error_کد) ثابت {
    إذا(U_FAILURE(error_کد)) {
        أرجع;
    }
    ثابت حرف *srcArray=src.داده();
    int32_t src_درازی=src.length();
    إذا(srcArray==اشاره_تهی && src_درازی!=0) {
        error_کد=U_ILLEGAL_آرگومان_ERROR;
        أرجع;
    }
    // Arguments are fine, reset output values.
    info.reset();
    إذا(src_درازی==0) {
        info.errors|=UIDNA_ERROR_خالی_LABEL;
        dest.Flush();
        أرجع;
    }
    UnicodeString destString;
    int32_t label_شروع=0;
    إذا(src_درازی<=256) {  // length of stackArray[]
        // ASCII fastpath
        حرف پشته_Array[256];
        int32_t dest_ظرفیت;
        حرف *destArray=dest.Get_Append_میانگیر(src_درازی, src_درازی+20,
                                             پشته_Array, UPRV_LENGTHOF(پشته_Array), &dest_ظرفیت);
        UBool disallowNonLDHDot=(options&UIDNA_USE_STD3_قواعد)!=0;
        int32_t i;
        لكل(i=0;; ++i) {
            إذا(i==src_درازی) {
                إذا(مقصد_ASCII) {
                    إذا((i-label_شروع)>63) {
                        info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
                    }
                    // There is a trailing dot if labelStart==i.
                    إذا(!isLabel && i>=254 && (i>254 || label_شروع<i)) {
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
            صحيح c_داده = ascii_داده[static_cast<صحيح>(c)]; // Cast: gcc warns about indexing with a char.
            إذا(c_داده>0) {
                destArray[i]=c+0x20;  // Lowercase an uppercase ASCII letter.
            } وإلا إذا(c_داده<0 && disallowNonLDHDot) {
                اكسر;  // Replacing with U+FFFD can be complicated for toASCII.
            } وإلا {
                destArray[i]=c;
                إذا(c==0x2d) {  // hyphen
                    إذا(i==(label_شروع+3) && srcArray[i-1]==0x2d) {
                        // "??--..." is Punycode or forbidden.
                        اكسر;
                    }
                    إذا(i==label_شروع) {
                        // label starts with "-"
                        info.labelErrors|=UIDNA_ERROR_LEADING_HYPHEN;
                    }
                    إذا((i+1)==src_درازی || srcArray[i+1]==0x2e) {
                        // label ends with "-"
                        info.labelErrors|=UIDNA_ERROR_TRAILING_HYPHEN;
                    }
                } وإلا إذا(c==0x2e) {  // dot
                    إذا(isLabel) {
                        اكسر;  // Replacing with U+FFFD can be complicated for toASCII.
                    }
                    إذا(i==label_شروع) {
                        info.labelErrors|=UIDNA_ERROR_خالی_LABEL;
                    }
                    إذا(مقصد_ASCII && (i-label_شروع)>63) {
                        info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
                    }
                    info.errors|=info.labelErrors;
                    info.labelErrors=0;
                    label_شروع=i+1;
                }
            }
        }
        info.errors|=info.labelErrors;
        // Convert the processed ASCII prefix of the current label to UTF-16.
        int32_t mapping_شروع=i-label_شروع;
        destString=UnicodeString::fromUTF8(StringPiece(destArray+label_شروع, mapping_شروع));
        // Output the previous ASCII labels and process the rest of src in UTF-16.
        dest.Append(destArray, label_شروع);
        processUnicode(UnicodeString::fromUTF8(StringPiece(src, label_شروع)), 0, mapping_شروع,
                       isLabel, مقصد_ASCII,
                       destString, info, error_کد);
    } وإلا {
        // src is too long for the ASCII fastpath implementation.
        processUnicode(UnicodeString::fromUTF8(src), 0, 0,
                       isLabel, مقصد_ASCII,
                       destString, info, error_کد);
    }
    destString.مقصد_UTF8(dest);  // calls dest.Flush()
    إذا(مقصد_ASCII && !isLabel) {
        // length==labelStart==254 means that there is a trailing dot (ok) and
        // destString is empty (do not index at 253-labelStart).
        int32_t length=label_شروع+destString.length();
        إذا( length>=254 && isASCIIString(destString) &&
            (length>254 ||
             (label_شروع<254 && destString[253-label_شروع]!=0x2e))
        ) {
            info.errors|=UIDNA_ERROR_DOMAIN_NAME_TOO_LONG;
        }
    }
    إذا( info.isBiDi && U_SUCCESS(error_کد) && (info.errors&severeErrors)==0 &&
        (!info.isOkBiDi || (label_شروع>0 && !isASCIIOkBiDi(srcArray, label_شروع)))
    ) {
        info.errors|=UIDNA_ERROR_BIDI;
    }
}

UnicodeString&
UTS46::processUnicode(ثابت UnicodeString& src,
                      int32_t label_شروع,
                      int32_t mapping_شروع,
                      UBool isLabel,
                      UBool مقصد_ASCII,
                      UnicodeString& dest U_LIFETIME_BOUND,
                      IDNAInfo& info,
                      UError_کد& error_کد) ثابت {
    إذا(mapping_شروع==0) {
        uts46Norm2.normalize(src, dest, error_کد);
    } وإلا {
        uts46Norm2.normalize_دومی_And_Append(dest, src.موقت_Sub_String(mapping_شروع), error_کد);
    }
    إذا(U_FAILURE(error_کد)) {
        أرجع dest;
    }
    UBool doMapDevChars=
        مقصد_ASCII ? (options&UIDNA_NONTRANSITIONAL_مقصد_ASCII)==0 :
                  (options&UIDNA_NONTRANSITIONAL_مقصد_UNICODE)==0;
    ثابت char16_t *destArray=dest.get_میانگیر();
    int32_t dest_درازی=dest.length();
    int32_t label_حد=label_شروع;
    طالما(label_حد<dest_درازی) {
        char16_t c=destArray[label_حد];
        إذا(c==0x2e && !isLabel) {
            int32_t label_درازی=label_حد-label_شروع;
            int32_t new_درازی=processLabel(dest, label_شروع, label_درازی,
                                            مقصد_ASCII, info, error_کد);
            info.errors|=info.labelErrors;
            info.labelErrors=0;
            إذا(U_FAILURE(error_کد)) {
                أرجع dest;
            }
            destArray=dest.get_میانگیر();
            dest_درازی+=new_درازی-label_درازی;
            label_حد=label_شروع+=new_درازی+1;
            استمر;
        } وإلا إذا(c<0xdf) {
            // pass
        } وإلا إذا(c<=0x200d && (c==0xdf || c==0x3c2 || c>=0x200c)) {
            info.isTransDiff=درست;
            إذا(doMapDevChars) {
                dest_درازی=mapDevChars(dest, label_شروع, label_حد, error_کد);
                إذا(U_FAILURE(error_کد)) {
                    أرجع dest;
                }
                destArray=dest.get_میانگیر();
                // All deviation characters have been mapped, no need to check for them again.
                doMapDevChars=خطأ;
                // Do not increment labelLimit in case c was removed.
                استمر;
            }
        } وإلا إذا(U16_IS_SURROGATE(c)) {
            إذا(U16_IS_SURROGATE_LEAD(c) ?
                    (label_حد+1)==dest_درازی || !U16_IS_TRAIL(destArray[label_حد+1]) :
                    label_حد==label_شروع || !U16_IS_LEAD(destArray[label_حد-1])) {
                // Map an unpaired surrogate to U+FFFD before normalization so that when
                // that removes characters we do not turn two unpaired ones into a pair.
                info.labelErrors|=UIDNA_ERROR_DISALLOWED;
                dest.مجموعه_Char_At(label_حد, 0xfffd);
                destArray=dest.get_میانگیر();
            }
        }
        ++label_حد;
    }
    // Permit an empty label at the end (0<labelStart==labelLimit==destLength is ok)
    // but not an empty label elsewhere nor a completely empty domain name.
    // processLabel() sets UIDNA_ERROR_EMPTY_LABEL when labelLength==0.
    إذا(0==label_شروع || label_شروع<label_حد) {
        processLabel(dest, label_شروع, label_حد-label_شروع,
                      مقصد_ASCII, info, error_کد);
        info.errors|=info.labelErrors;
    }
    أرجع dest;
}

int32_t
UTS46::mapDevChars(UnicodeString &dest, int32_t label_شروع, int32_t mapping_شروع,
                   UError_کد &error_کد) ثابت {
    إذا(U_FAILURE(error_کد)) {
        أرجع 0;
    }
    int32_t length=dest.length();
    char16_t *s=dest.get_میانگیر(dest[mapping_شروع]==0xdf ? length+1 : length);
    إذا(s==اشاره_تهی) {
        error_کد=U_حافظه_تخصیص_ERROR;
        أرجع length;
    }
    int32_t capacity=dest.get_ظرفیت();
    UBool didMapDevChars=خطأ;
    int32_t read_شاخص=mapping_شروع, write_شاخص=mapping_شروع;
    افعل {
        char16_t c=s[read_شاخص++];
        بدل(c) {
        حالة 0xdf:
            // Map sharp s to ss.
            didMapDevChars=درست;
            s[write_شاخص++]=0x73;  // Replace sharp s with first s.
            // Insert second s and account for possible buffer reallocation.
            إذا(write_شاخص==read_شاخص) {
                إذا(length==capacity) {
                    dest.release_میانگیر(length);
                    s=dest.get_میانگیر(length+1);
                    إذا(s==اشاره_تهی) {
                        error_کد=U_حافظه_تخصیص_ERROR;
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
            didMapDevChars=درست;
            s[write_شاخص++]=0x3c3;
            اكسر;
        حالة 0x200c:  // Ignore/remove ZWNJ.
        حالة 0x200d:  // Ignore/remove ZWJ.
            didMapDevChars=درست;
            --length;
            اكسر;
        افتراضي:
            // Only really necessary if writeIndex was different from readIndex.
            s[write_شاخص++]=c;
            اكسر;
        }
    } طالما(write_شاخص<length);
    dest.release_میانگیر(length);
    إذا(didMapDevChars) {
        // Mapping deviation characters might have resulted in an un-NFC string.
        // We could use either the NFC or the UTS #46 normalizer.
        // By using the UTS #46 normalizer again, we avoid having to load a second .nrm data file.
        UnicodeString normalized;
        uts46Norm2.normalize(dest.موقت_Sub_String(label_شروع), normalized, error_کد);
        إذا(U_SUCCESS(error_کد)) {
            dest.replace(label_شروع, 0x7fffffff, normalized);
            إذا(dest.isBogus()) {
                error_کد=U_حافظه_تخصیص_ERROR;
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
replaceLabel(UnicodeString &dest, int32_t dest_Label_شروع, int32_t dest_Label_درازی,
             ثابت UnicodeString &label, int32_t label_درازی, UError_کد &error_کد) {
    إذا(U_FAILURE(error_کد)) {
        أرجع 0;
    }
    إذا(&label!=&dest) {
        dest.replace(dest_Label_شروع, dest_Label_درازی, label);
        إذا(dest.isBogus()) {
            error_کد=U_حافظه_تخصیص_ERROR;
            أرجع 0;
        }
    }
    أرجع label_درازی;
}

int32_t
UTS46::processLabel(UnicodeString &dest,
                    int32_t label_شروع, int32_t label_درازی,
                    UBool مقصد_ASCII,
                    IDNAInfo &info, UError_کد &error_کد) ثابت {
    إذا(U_FAILURE(error_کد)) {
        أرجع 0;
    }
    UnicodeString fromPunycode;
    UnicodeString *labelString;
    ثابت char16_t *label=dest.get_میانگیر()+label_شروع;
    int32_t dest_Label_شروع=label_شروع;
    int32_t dest_Label_درازی=label_درازی;
    UBool wasPunycode;
    إذا(label_درازی>=4 && label[0]==0x78 && label[1]==0x6e && label[2]==0x2d && label[3]==0x2d) {
        // Label starts with "xn--", try to un-Punycode it.
        // In IDNA2008, labels like "xn--" (decodes to an empty string) and
        // "xn--ASCII-" (decodes to just "ASCII") fail the round-trip validation from
        // comparing the ToUnicode input with the back-to-ToASCII output.
        // They are alternate encodings of the respective ASCII labels.
        // Ignore "xn---" here: It will fail Punycode.decode() which logically comes before
        // the round-trip verification.
        إذا(label_درازی==4 || (label_درازی>5 && label[label_درازی-1]==u'-')) {
            info.labelErrors|=UIDNA_ERROR_INVALID_ACE_LABEL;
            أرجع markBadACELabel(dest, label_شروع, label_درازی, مقصد_ASCII, info, error_کد);
        }
        wasPunycode=درست;
        char16_t *unicode_میانگیر=fromPunycode.get_میانگیر(-1);  // capacity==-1: most labels should fit
        إذا(unicode_میانگیر==اشاره_تهی) {
            // Should never occur if we used capacity==-1 which uses the internal buffer.
            error_کد=U_حافظه_تخصیص_ERROR;
            أرجع label_درازی;
        }
        UError_کد punycode_Error_کد=U_ZERO_ERROR;
        int32_t unicode_درازی=u_strFromPunycode(label+4, label_درازی-4,
                                                unicode_میانگیر, fromPunycode.get_ظرفیت(),
                                                اشاره_تهی, &punycode_Error_کد);
        إذا(punycode_Error_کد==U_میانگیر_سرریز_ERROR) {
            fromPunycode.release_میانگیر(0);
            unicode_میانگیر=fromPunycode.get_میانگیر(unicode_درازی);
            إذا(unicode_میانگیر==اشاره_تهی) {
                error_کد=U_حافظه_تخصیص_ERROR;
                أرجع label_درازی;
            }
            punycode_Error_کد=U_ZERO_ERROR;
            unicode_درازی=u_strFromPunycode(label+4, label_درازی-4,
                                            unicode_میانگیر, fromPunycode.get_ظرفیت(),
                                            اشاره_تهی, &punycode_Error_کد);
        }
        fromPunycode.release_میانگیر(unicode_درازی);
        إذا(U_FAILURE(punycode_Error_کد)) {
            info.labelErrors|=UIDNA_ERROR_PUNYCODE;
            أرجع markBadACELabel(dest, label_شروع, label_درازی, مقصد_ASCII, info, error_کد);
        }
        // Check for NFC, and for characters that are not
        // valid or deviation characters according to the normalizer.
        // If there is something wrong, then the string will change.
        // Note that the normalizer passes through non-LDH ASCII and deviation characters.
        // Deviation characters are ok in Punycode even in transitional processing.
        // In the code further below, if we find non-LDH ASCII and we have UIDNA_USE_STD3_RULES
        // then we will set UIDNA_ERROR_INVALID_ACE_LABEL there too.
        UBool isValid=uts46Norm2.isNormalized(fromPunycode, error_کد);
        إذا(U_FAILURE(error_کد)) {
            أرجع label_درازی;
        }
        // Unicode 15.1 UTS #46:
        // Added an additional condition in 4.1 Validity Criteria to
        // disallow labels such as xn--xn---epa., which do not round-trip.
        // --> Validity Criteria new criterion 4:
        // If not CheckHyphens, the label must not begin with “xn--”.
        إذا(!isValid || fromPunycode.startsWith(UnicodeString::readOnlyAlias(u"xn--"))) {
            info.labelErrors|=UIDNA_ERROR_INVALID_ACE_LABEL;
            أرجع markBadACELabel(dest, label_شروع, label_درازی, مقصد_ASCII, info, error_کد);
        }
        labelString=&fromPunycode;
        label=fromPunycode.get_میانگیر();
        label_شروع=0;
        label_درازی=fromPunycode.length();
    } وإلا {
        wasPunycode=خطأ;
        labelString=&dest;
    }
    // Validity check
    إذا(label_درازی==0) {
        info.labelErrors|=UIDNA_ERROR_خالی_LABEL;
        أرجع replaceLabel(dest, dest_Label_شروع, dest_Label_درازی,
                            *labelString, label_درازی, error_کد);
    }
    // labelLength>0
    إذا(label_درازی>=4 && label[2]==0x2d && label[3]==0x2d) {
        // label starts with "??--"
        info.labelErrors|=UIDNA_ERROR_HYPHEN_3_4;
    }
    إذا(label[0]==0x2d) {
        // label starts with "-"
        info.labelErrors|=UIDNA_ERROR_LEADING_HYPHEN;
    }
    إذا(label[label_درازی-1]==0x2d) {
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
    ثابت char16_t *حد=label+label_درازی;
    char16_t oredChars=0;
    // If we enforce STD3 rules, then ASCII characters other than LDH and dot are disallowed.
    UBool disallowNonLDHDot=(options&UIDNA_USE_STD3_قواعد)!=0;
    افعل {
        char16_t c=*s;
        إذا(c<=0x7f) {
            إذا(c==0x2e) {
                info.labelErrors|=UIDNA_ERROR_LABEL_HAS_DOT;
                *s=0xfffd;
            } وإلا إذا(disallowNonLDHDot && ascii_داده[c]<0) {
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
    int32_t cp_درازی=0;
    // "Unsafe" is ok because unpaired surrogates were mapped to U+FFFD.
    U16_بعدی_UNSAFE(label, cp_درازی, c);
    إذا((U_GET_GC_ماسک(c)&U_GC_M_ماسک)!=0) {
        info.labelErrors|=UIDNA_ERROR_LEADING_COMBINING_MARK;
        labelString->replace(label_شروع, cp_درازی, static_cast<char16_t>(0xfffd));
        label=labelString->get_میانگیر()+label_شروع;
        label_درازی+=1-cp_درازی;
        إذا(labelString==&dest) {
            dest_Label_درازی=label_درازی;
        }
    }
    إذا((info.labelErrors&severeErrors)==0) {
        // Do contextual checks only if we do not have U+FFFD from a severe error
        // because U+FFFD can make these checks fail.
        إذا((options&UIDNA_CHECK_BIDI)!=0 && (!info.isBiDi || info.isOkBiDi)) {
            checkLabelBiDi(label, label_درازی, info);
        }
        إذا( (options&UIDNA_CHECK_CONTEXTJ)!=0 && (oredChars&0x200c)==0x200c &&
            !isLabelOkContextJ(label, label_درازی)
        ) {
            info.labelErrors|=UIDNA_ERROR_CONTEXTJ;
        }
        إذا((options&UIDNA_CHECK_CONTEXTO)!=0 && oredChars>=0xb7) {
            checkLabelContextO(label, label_درازی, info);
        }
        إذا(مقصد_ASCII) {
            إذا(wasPunycode) {
                // Leave a Punycode label unchanged if it has no severe errors.
                إذا(dest_Label_درازی>63) {
                    info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
                }
                أرجع dest_Label_درازی;
            } وإلا إذا(oredChars>=0x80) {
                // Contains non-ASCII characters.
                UnicodeString punycode;
                char16_t *میانگیر=punycode.get_میانگیر(63);  // 63==maximum DNS label length
                إذا(میانگیر==اشاره_تهی) {
                    error_کد=U_حافظه_تخصیص_ERROR;
                    أرجع dest_Label_درازی;
                }
                میانگیر[0]=0x78;  // Write "xn--".
                میانگیر[1]=0x6e;
                میانگیر[2]=0x2d;
                میانگیر[3]=0x2d;
                UError_کد punycode_Error_کد=U_ZERO_ERROR;
                int32_t punycode_درازی=u_str_مقصد_Punycode(label, label_درازی,
                                                      میانگیر+4, punycode.get_ظرفیت()-4,
                                                      اشاره_تهی, &punycode_Error_کد);
                إذا(punycode_Error_کد==U_میانگیر_سرریز_ERROR) {
                    punycode_Error_کد=U_ZERO_ERROR;
                    punycode.release_میانگیر(4);
                    میانگیر=punycode.get_میانگیر(4+punycode_درازی);
                    إذا(میانگیر==اشاره_تهی) {
                        error_کد=U_حافظه_تخصیص_ERROR;
                        أرجع dest_Label_درازی;
                    }
                    punycode_درازی=u_str_مقصد_Punycode(label, label_درازی,
                                                  میانگیر+4, punycode.get_ظرفیت()-4,
                                                  اشاره_تهی, &punycode_Error_کد);
                }
                punycode_درازی+=4;
                punycode.release_میانگیر(punycode_درازی);
                إذا(U_FAILURE(punycode_Error_کد)) {
                    error_کد = punycode_Error_کد;
                    أرجع dest_Label_درازی;
                }
                إذا(punycode_درازی>63) {
                    info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
                }
                أرجع replaceLabel(dest, dest_Label_شروع, dest_Label_درازی,
                                    punycode, punycode_درازی, error_کد);
            } وإلا {
                // all-ASCII label
                إذا(label_درازی>63) {
                    info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
                }
            }
        }
    } وإلا {
        // If a Punycode label has severe errors,
        // then leave it but make sure it does not look valid.
        إذا(wasPunycode) {
            info.labelErrors|=UIDNA_ERROR_INVALID_ACE_LABEL;
            أرجع markBadACELabel(dest, dest_Label_شروع, dest_Label_درازی, مقصد_ASCII, info, error_کد);
        }
    }
    أرجع replaceLabel(dest, dest_Label_شروع, dest_Label_درازی,
                        *labelString, label_درازی, error_کد);
}

// Make sure an ACE label does not look valid.
// Append U+FFFD if the label has only LDH characters.
// If UIDNA_USE_STD3_RULES, also replace disallowed ASCII characters with U+FFFD.
int32_t
UTS46::markBadACELabel(UnicodeString &dest,
                       int32_t label_شروع, int32_t label_درازی,
                       UBool مقصد_ASCII, IDNAInfo &info, UError_کد &error_کد) ثابت {
    إذا(U_FAILURE(error_کد)) {
        أرجع 0;
    }
    UBool disallowNonLDHDot=(options&UIDNA_USE_STD3_قواعد)!=0;
    UBool isASCII=درست;
    UBool onlyLDH=درست;
    ثابت char16_t *label=dest.get_میانگیر()+label_شروع;
    ثابت char16_t *حد=label+label_درازی;
    // Start after the initial "xn--".
    // Ok to cast away const because we own the UnicodeString.
    لكل(char16_t *s=const_cast<char16_t *>(label+4); s<حد; ++s) {
        char16_t c=*s;
        إذا(c<=0x7f) {
            إذا(c==0x2e) {
                info.labelErrors|=UIDNA_ERROR_LABEL_HAS_DOT;
                *s=0xfffd;
                isASCII=onlyLDH=خطأ;
            } وإلا إذا(ascii_داده[c]<0) {
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
        dest.insert(label_شروع + label_درازی, static_cast<char16_t>(0xfffd));
        إذا(dest.isBogus()) {
            error_کد=U_حافظه_تخصیص_ERROR;
            أرجع 0;
        }
        ++label_درازی;
    } وإلا {
        إذا(مقصد_ASCII && isASCII && label_درازی>63) {
            info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
        }
    }
    أرجع label_درازی;
}

ثابت uint32_t L_ماسک=U_ماسک(U_چپ_مقصد_راست);
ثابت uint32_t R_AL_ماسک=U_ماسک(U_راست_مقصد_چپ)|U_ماسک(U_راست_مقصد_چپ_ARABIC);
ثابت uint32_t L_R_AL_ماسک=L_ماسک|R_AL_ماسک;

ثابت uint32_t R_AL_AN_ماسک=R_AL_ماسک|U_ماسک(U_ARABIC_عدد);

ثابت uint32_t EN_AN_ماسک=U_ماسک(U_EUROPEAN_عدد)|U_ماسک(U_ARABIC_عدد);
ثابت uint32_t R_AL_EN_AN_ماسک=R_AL_ماسک|EN_AN_ماسک;
ثابت uint32_t L_EN_ماسک=L_ماسک|U_ماسک(U_EUROPEAN_عدد);

ثابت uint32_t ES_CS_ET_ON_BN_NSM_ماسک=
    U_ماسک(U_EUROPEAN_عدد_SEPARATOR)|
    U_ماسک(U_COMMON_عدد_SEPARATOR)|
    U_ماسک(U_EUROPEAN_عدد_TERMINATOR)|
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
UTS46::checkLabelBiDi(ثابت char16_t *label, int32_t label_درازی, IDNAInfo &info) ثابت {
    // IDNA2008 BiDi rule
    // Get the directionality of the first character.
    UChar32 c;
    int32_t i=0;
    U16_بعدی_UNSAFE(label, i, c);
    uint32_t اولی_ماسک=U_ماسک(u_charDirection(c));
    // 1. The first character must be a character with BIDI property L, R
    // or AL.  If it has the R or AL property, it is an RTL label; if it
    // has the L property, it is an LTR label.
    إذا((اولی_ماسک&~L_R_AL_ماسک)!=0) {
        info.isOkBiDi=خطأ;
    }
    // Get the directionality of the last non-NSM character.
    uint32_t آخرین_ماسک;
    لكل(;;) {
        إذا(i>=label_درازی) {
            آخرین_ماسک=اولی_ماسک;
            اكسر;
        }
        U16_PREV_UNSAFE(label, label_درازی, c);
        UCharDirection dir=u_charDirection(c);
        إذا(dir!=U_DIR_NON_SPACING_MARK) {
            آخرین_ماسک=U_ماسک(dir);
            اكسر;
        }
    }
    // 3. In an RTL label, the end of the label must be a character with
    // BIDI property R, AL, EN or AN, followed by zero or more
    // characters with BIDI property NSM.
    // 6. In an LTR label, the end of the label must be a character with
    // BIDI property L or EN, followed by zero or more characters with
    // BIDI property NSM.
    إذا( (اولی_ماسک&L_ماسک)!=0 ?
            (آخرین_ماسک&~L_EN_ماسک)!=0 :
            (آخرین_ماسک&~R_AL_EN_AN_ماسک)!=0
    ) {
        info.isOkBiDi=خطأ;
    }
    // Add the directionalities of the intervening characters.
    uint32_t mask=اولی_ماسک|آخرین_ماسک;
    طالما(i<label_درازی) {
        U16_بعدی_UNSAFE(label, i, c);
        mask|=U_ماسک(u_charDirection(c));
    }
    إذا(اولی_ماسک&L_ماسک) {
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
ساكن UBool
isASCIIOkBiDi(ثابت char16_t *s, int32_t length) {
    int32_t label_شروع=0;
    لكل(int32_t i=0; i<length; ++i) {
        char16_t c=s[i];
        إذا(c==0x2e) {  // dot
            إذا(i>label_شروع) {
                c=s[i-1];
                إذا(!(0x61<=c && c<=0x7a) && !(0x30<=c && c<=0x39)) {
                    // Last character in the label is not an L or EN.
                    أرجع خطأ;
                }
            }
            label_شروع=i+1;
        } وإلا إذا(i==label_شروع) {
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
    أرجع درست;
}

// UTF-8 version, called for source ASCII prefix.
// Can contain uppercase A-Z.
// s[length-1] must be the trailing dot.
ساكن UBool
isASCIIOkBiDi(ثابت حرف *s, int32_t length) {
    int32_t label_شروع=0;
    لكل(int32_t i=0; i<length; ++i) {
        حرف c=s[i];
        إذا(c==0x2e) {  // dot
            إذا(i>label_شروع) {
                c=s[i-1];
                إذا(!(0x61<=c && c<=0x7a) && !(0x41<=c && c<=0x5a) && !(0x30<=c && c<=0x39)) {
                    // Last character in the label is not an L or EN.
                    أرجع خطأ;
                }
            }
            label_شروع=i+1;
        } وإلا إذا(i==label_شروع) {
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
    أرجع درست;
}

UBool
UTS46::isLabelOkContextJ(ثابت char16_t *label, int32_t label_درازی) ثابت {
    // [IDNA2008-Tables]
    // 200C..200D  ; CONTEXTJ    # ZERO WIDTH NON-JOINER..ZERO WIDTH JOINER
    لكل(int32_t i=0; i<label_درازی; ++i) {
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
                } وإلا إذا(type==U_JT_چپ_JOINING || type==U_JT_DUAL_JOINING) {
                    اكسر;  // precontext fulfilled
                } وإلا {
                    أرجع خطأ;
                }
            }
            // check postcontext (Joining_Type:T)*(Joining_Type:{R,D})
            لكل(j=i+1;;) {
                إذا(j==label_درازی) {
                    أرجع خطأ;
                }
                U16_بعدی_UNSAFE(label, j, c);
                UJoiningType type=ubidi_getJoiningType(c);
                إذا(type==U_JT_TRANSPARENT) {
                    // just skip this character
                } وإلا إذا(type==U_JT_راست_JOINING || type==U_JT_DUAL_JOINING) {
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
    أرجع درست;
}

فراغ
UTS46::checkLabelContextO(ثابت char16_t *label, int32_t label_درازی, IDNAInfo &info) ثابت {
    int32_t label_انتها=label_درازی-1;  // inclusive
    int32_t arabicDigits=0;  // -1 for 066x, +1 for 06Fx
    لكل(int32_t i=0; i<=label_انتها; ++i) {
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
                     i<label_انتها && label[i+1]==0x6c)) {
                    info.labelErrors|=UIDNA_ERROR_CONTEXTO_PUNCTUATION;
                }
            } وإلا إذا(c==0x375) {
                // Appendix A.4. GREEK LOWER NUMERAL SIGN (KERAIA) (U+0375)
                // Rule Set:
                //  False;
                //  If Script(After(cp)) .eq.  Greek Then True;
                UScript_کد script=USCRIPT_INVALID_کد;
                إذا(i<label_انتها) {
                    UError_کد error_کد=U_ZERO_ERROR;
                    int32_t j=i+1;
                    U16_بعدی(label, j, label_درازی, c);
                    script=uscript_getScript(c, &error_کد);
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
                UScript_کد script=USCRIPT_INVALID_کد;
                إذا(0<i) {
                    UError_کد error_کد=U_ZERO_ERROR;
                    int32_t j=i;
                    U16_PREV(label, 0, j, c);
                    script=uscript_getScript(c, &error_کد);
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
            UError_کد error_کد=U_ZERO_ERROR;
            لكل(صحيح j=0;;) {
                إذا(j>label_انتها) {
                    info.labelErrors|=UIDNA_ERROR_CONTEXTO_PUNCTUATION;
                    اكسر;
                }
                U16_بعدی(label, j, label_درازی, c);
                UScript_کد script=uscript_getScript(c, &error_کد);
                إذا(script==USCRIPT_HIRAGANA || script==USCRIPT_KATAKANA || script==USCRIPT_HAN) {
                    اكسر;
                }
            }
        }
    }
}

U_NAMESPACE_انتها

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

U_CAPI UIDNA * U_EXPORT2
uidna_openUTS46(uint32_t options, UError_کد *p_Error_کد) {
    أرجع reinterpret_cast<UIDNA *>(IDNA::createUTS46Instance(options, *p_Error_کد));
}

U_CAPI فراغ U_EXPORT2
uidna_close(UIDNA *idna) {
    حذف reinterpret_cast<IDNA *>(idna);
}

ساكن UBool
check_آرگومانها(ثابت فراغ *label, int32_t length,
          فراغ *dest, int32_t capacity,
          UIDNAInfo *pInfo, UError_کد *p_Error_کد) {
    إذا(U_FAILURE(*p_Error_کد)) {
        أرجع خطأ;
    }
    // sizeof(UIDNAInfo)=16 in the first API version.
    إذا(pInfo==اشاره_تهی || pInfo->size<16) {
        *p_Error_کد=U_ILLEGAL_آرگومان_ERROR;
        أرجع خطأ;
    }
    إذا( (label==اشاره_تهی ? length!=0 : length<-1) ||
        (dest==اشاره_تهی ? capacity!=0 : capacity<0) ||
        (dest==label && label!=اشاره_تهی)
    ) {
        *p_Error_کد=U_ILLEGAL_آرگومان_ERROR;
        أرجع خطأ;
    }
    // Set all *pInfo bytes to 0 except for the size field itself.
    uprv_memset(&pInfo->size+1, 0, pInfo->size-حجم(pInfo->size));
    أرجع درست;
}

ساكن فراغ
idna_Info_مقصد_Struct(IDNAInfo &info, UIDNAInfo *pInfo) {
    pInfo->isTransitionalDifferent=info.isTransitionalDifferent();
    pInfo->errors=info.getErrors();
}

U_CAPI int32_t U_EXPORT2
uidna_label_مقصد_ASCII(ثابت UIDNA *idna,
                   ثابت char16_t *label, int32_t length,
                   char16_t *dest, int32_t capacity,
                   UIDNAInfo *pInfo, UError_کد *p_Error_کد) {
    إذا(!check_آرگومانها(label, length, dest, capacity, pInfo, p_Error_کد)) {
        أرجع 0;
    }
    UnicodeString src(length < 0, label, length);
    UnicodeString destString(dest, 0, capacity);
    IDNAInfo info;
    reinterpret_cast<ثابت IDNA *>(idna)->label_مقصد_ASCII(src, destString, info, *p_Error_کد);
    idna_Info_مقصد_Struct(info, pInfo);
    أرجع destString.extract(dest, capacity, *p_Error_کد);
}

U_CAPI int32_t U_EXPORT2
uidna_label_مقصد_Unicode(ثابت UIDNA *idna,
                     ثابت char16_t *label, int32_t length,
                     char16_t *dest, int32_t capacity,
                     UIDNAInfo *pInfo, UError_کد *p_Error_کد) {
    إذا(!check_آرگومانها(label, length, dest, capacity, pInfo, p_Error_کد)) {
        أرجع 0;
    }
    UnicodeString src(length < 0, label, length);
    UnicodeString destString(dest, 0, capacity);
    IDNAInfo info;
    reinterpret_cast<ثابت IDNA *>(idna)->label_مقصد_Unicode(src, destString, info, *p_Error_کد);
    idna_Info_مقصد_Struct(info, pInfo);
    أرجع destString.extract(dest, capacity, *p_Error_کد);
}

U_CAPI int32_t U_EXPORT2
uidna_name_مقصد_ASCII(ثابت UIDNA *idna,
                  ثابت char16_t *name, int32_t length,
                  char16_t *dest, int32_t capacity,
                  UIDNAInfo *pInfo, UError_کد *p_Error_کد) {
    إذا(!check_آرگومانها(name, length, dest, capacity, pInfo, p_Error_کد)) {
        أرجع 0;
    }
    UnicodeString src(length < 0, name, length);
    UnicodeString destString(dest, 0, capacity);
    IDNAInfo info;
    reinterpret_cast<ثابت IDNA *>(idna)->name_مقصد_ASCII(src, destString, info, *p_Error_کد);
    idna_Info_مقصد_Struct(info, pInfo);
    أرجع destString.extract(dest, capacity, *p_Error_کد);
}

U_CAPI int32_t U_EXPORT2
uidna_name_مقصد_Unicode(ثابت UIDNA *idna,
                    ثابت char16_t *name, int32_t length,
                    char16_t *dest, int32_t capacity,
                    UIDNAInfo *pInfo, UError_کد *p_Error_کد) {
    إذا(!check_آرگومانها(name, length, dest, capacity, pInfo, p_Error_کد)) {
        أرجع 0;
    }
    UnicodeString src(length < 0, name, length);
    UnicodeString destString(dest, 0, capacity);
    IDNAInfo info;
    reinterpret_cast<ثابت IDNA *>(idna)->name_مقصد_Unicode(src, destString, info, *p_Error_کد);
    idna_Info_مقصد_Struct(info, pInfo);
    أرجع destString.extract(dest, capacity, *p_Error_کد);
}

U_CAPI int32_t U_EXPORT2
uidna_label_مقصد_ASCII_UTF8(ثابت UIDNA *idna,
                        ثابت حرف *label, int32_t length,
                        حرف *dest, int32_t capacity,
                        UIDNAInfo *pInfo, UError_کد *p_Error_کد) {
    إذا(!check_آرگومانها(label, length, dest, capacity, pInfo, p_Error_کد)) {
        أرجع 0;
    }
    StringPiece src(label, length<0 ? static_cast<int32_t>(uprv_strlen(label)) : length);
    أرجع ByteSinkUtil::از_طریق_Byte_Sink_مقصد_Terminated_Chars(
        dest, capacity,
        [&](ByteSink& sink, UError_کد& وضعیت) {
            IDNAInfo info;
            reinterpret_cast<ثابت IDNA *>(idna)->label_مقصد_ASCII_UTF8(src, sink, info, وضعیت);
            idna_Info_مقصد_Struct(info, pInfo);
        },
        *p_Error_کد);
}

U_CAPI int32_t U_EXPORT2
uidna_label_مقصد_Unicode_UTF8(ثابت UIDNA *idna,
                         ثابت حرف *label, int32_t length,
                         حرف *dest, int32_t capacity,
                         UIDNAInfo *pInfo, UError_کد *p_Error_کد) {
    إذا(!check_آرگومانها(label, length, dest, capacity, pInfo, p_Error_کد)) {
        أرجع 0;
    }
    StringPiece src(label, length<0 ? static_cast<int32_t>(uprv_strlen(label)) : length);
    أرجع ByteSinkUtil::از_طریق_Byte_Sink_مقصد_Terminated_Chars(
        dest, capacity,
        [&](ByteSink& sink, UError_کد& وضعیت) {
            IDNAInfo info;
            reinterpret_cast<ثابت IDNA *>(idna)->label_مقصد_Unicode_UTF8(src, sink, info, وضعیت);
            idna_Info_مقصد_Struct(info, pInfo);
        },
        *p_Error_کد);
}

U_CAPI int32_t U_EXPORT2
uidna_name_مقصد_ASCII_UTF8(ثابت UIDNA *idna,
                       ثابت حرف *name, int32_t length,
                       حرف *dest, int32_t capacity,
                       UIDNAInfo *pInfo, UError_کد *p_Error_کد) {
    إذا(!check_آرگومانها(name, length, dest, capacity, pInfo, p_Error_کد)) {
        أرجع 0;
    }
    StringPiece src(name, length<0 ? static_cast<int32_t>(uprv_strlen(name)) : length);
    أرجع ByteSinkUtil::از_طریق_Byte_Sink_مقصد_Terminated_Chars(
        dest, capacity,
        [&](ByteSink& sink, UError_کد& وضعیت) {
            IDNAInfo info;
            reinterpret_cast<ثابت IDNA *>(idna)->name_مقصد_ASCII_UTF8(src, sink, info, وضعیت);
            idna_Info_مقصد_Struct(info, pInfo);
        },
        *p_Error_کد);
}

U_CAPI int32_t U_EXPORT2
uidna_name_مقصد_Unicode_UTF8(ثابت UIDNA *idna,
                        ثابت حرف *name, int32_t length,
                        حرف *dest, int32_t capacity,
                        UIDNAInfo *pInfo, UError_کد *p_Error_کد) {
    إذا(!check_آرگومانها(name, length, dest, capacity, pInfo, p_Error_کد)) {
        أرجع 0;
    }
    StringPiece src(name, length<0 ? static_cast<int32_t>(uprv_strlen(name)) : length);
    أرجع ByteSinkUtil::از_طریق_Byte_Sink_مقصد_Terminated_Chars(
        dest, capacity,
        [&](ByteSink& sink, UError_کد& وضعیت) {
            IDNAInfo info;
            reinterpret_cast<ثابت IDNA *>(idna)->name_مقصد_Unicode_UTF8(src, sink, info, وضعیت);
            idna_Info_مقصد_Struct(info, pInfo);
        },
        *p_Error_کد);
}

#پایان_اگر  // UCONFIG_NO_IDNA
