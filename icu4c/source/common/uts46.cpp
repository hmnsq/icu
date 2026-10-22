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

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

#ਜੇ !UCONFIG_NO_IDNA

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/bytestream.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/idna.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/normalizer2.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uscript.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ustring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utf16.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "bytesinkutil.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cstring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "punycode.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ubidi_props.h"

// Note about tests for UIDNA_ERROR_DOMAIN_NAME_TOO_LONG:
//
// The domain name length limit is 255 octets in an internal DNS representation
// where the last ("root") label is the empty label
// represented by length byte 0 alone.
// In a conventional string, this translates to 253 characters, or 254
// if there is a trailing dot for the root label.

U_NAMESPACE_BEGIN

// Severe errors which usually result in a U+FFFD replacement character in the result string.
ਅਚਲ uint32_t severeErrors=
    UIDNA_ERROR_LEADING_COMBINING_MARK|
    UIDNA_ERROR_DISALLOWED|
    UIDNA_ERROR_PUNYCODE|
    UIDNA_ERROR_LABEL_HAS_DOT|
    UIDNA_ERROR_INVALID_ACE_LABEL;

ਸਥਿਰ ਸਤਰਅੰਦਰ UBool
isASCIIString(ਅਚਲ UnicodeString &dest) {
    ਅਚਲ char16_t *s=dest.get_ਬਫਰ();
    ਅਚਲ char16_t *ਹੱਦਬੰਦੀ=s+dest.length();
    ਜਦੋਂ(s<ਹੱਦਬੰਦੀ) {
        ਜੇ(*s++>0x7f) {
            ਵਾਪਸ ਝੂਠ;
        }
    }
    ਵਾਪਸ ਸੱਚ;
}

ਸਥਿਰ UBool
isASCIIOkBiDi(ਅਚਲ char16_t *s, int32_t length);

ਸਥਿਰ UBool
isASCIIOkBiDi(ਅਚਲ ਅੱਖਰ *s, int32_t length);

// IDNA class default implementations -------------------------------------- ***

IDNA::~IDNA() {}

ਖਾਲੀ
IDNA::label_ਮੰਜ਼ਿਲ_ASCII_UTF8(StringPiece label, ByteSink &dest,
                        IDNAInfo &info, UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ {
    ਜੇ(U_SUCCESS(error_ਕੋਡ)) {
        UnicodeString destString;
        label_ਮੰਜ਼ਿਲ_ASCII(UnicodeString::fromUTF8(label), destString,
                     info, error_ਕੋਡ).ਮੰਜ਼ਿਲ_UTF8(dest);
    }
}

ਖਾਲੀ
IDNA::label_ਮੰਜ਼ਿਲ_Unicode_UTF8(StringPiece label, ByteSink &dest,
                         IDNAInfo &info, UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ {
    ਜੇ(U_SUCCESS(error_ਕੋਡ)) {
        UnicodeString destString;
        label_ਮੰਜ਼ਿਲ_Unicode(UnicodeString::fromUTF8(label), destString,
                       info, error_ਕੋਡ).ਮੰਜ਼ਿਲ_UTF8(dest);
    }
}

ਖਾਲੀ
IDNA::name_ਮੰਜ਼ਿਲ_ASCII_UTF8(StringPiece name, ByteSink &dest,
                       IDNAInfo &info, UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ {
    ਜੇ(U_SUCCESS(error_ਕੋਡ)) {
        UnicodeString destString;
        name_ਮੰਜ਼ਿਲ_ASCII(UnicodeString::fromUTF8(name), destString,
                    info, error_ਕੋਡ).ਮੰਜ਼ਿਲ_UTF8(dest);
    }
}

ਖਾਲੀ
IDNA::name_ਮੰਜ਼ਿਲ_Unicode_UTF8(StringPiece name, ByteSink &dest,
                        IDNAInfo &info, UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ {
    ਜੇ(U_SUCCESS(error_ਕੋਡ)) {
        UnicodeString destString;
        name_ਮੰਜ਼ਿਲ_Unicode(UnicodeString::fromUTF8(name), destString,
                      info, error_ਕੋਡ).ਮੰਜ਼ਿਲ_UTF8(dest);
    }
}

// UTS46 class declaration ------------------------------------------------- ***

ਵਰਗ UTS46 : ਜਨਤਕ IDNA {
ਜਨਤਕ:
    UTS46(uint32_t options, UError_ਕੋਡ &error_ਕੋਡ);
    ਆਭਾਸੀ ~UTS46();

    ਆਭਾਸੀ UnicodeString&
    label_ਮੰਜ਼ਿਲ_ASCII(ਅਚਲ UnicodeString& label,
                 UnicodeString& dest U_LIFETIME_BOUND,
                 IDNAInfo& info,
                 UError_ਕੋਡ& error_ਕੋਡ) ਅਚਲ override;

    ਆਭਾਸੀ UnicodeString&
    label_ਮੰਜ਼ਿਲ_Unicode(ਅਚਲ UnicodeString& label,
                   UnicodeString& dest U_LIFETIME_BOUND,
                   IDNAInfo& info,
                   UError_ਕੋਡ& error_ਕੋਡ) ਅਚਲ override;

    ਆਭਾਸੀ UnicodeString&
    name_ਮੰਜ਼ਿਲ_ASCII(ਅਚਲ UnicodeString& name,
                UnicodeString& dest U_LIFETIME_BOUND,
                IDNAInfo& info,
                UError_ਕੋਡ& error_ਕੋਡ) ਅਚਲ override;

    ਆਭਾਸੀ UnicodeString&
    name_ਮੰਜ਼ਿਲ_Unicode(ਅਚਲ UnicodeString& name,
                  UnicodeString& dest U_LIFETIME_BOUND,
                  IDNAInfo& info,
                  UError_ਕੋਡ& error_ਕੋਡ) ਅਚਲ override;

    ਆਭਾਸੀ ਖਾਲੀ
    label_ਮੰਜ਼ਿਲ_ASCII_UTF8(StringPiece label, ByteSink &dest,
                      IDNAInfo &info, UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ override;

    ਆਭਾਸੀ ਖਾਲੀ
    label_ਮੰਜ਼ਿਲ_Unicode_UTF8(StringPiece label, ByteSink &dest,
                       IDNAInfo &info, UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ override;

    ਆਭਾਸੀ ਖਾਲੀ
    name_ਮੰਜ਼ਿਲ_ASCII_UTF8(StringPiece name, ByteSink &dest,
                     IDNAInfo &info, UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ override;

    ਆਭਾਸੀ ਖਾਲੀ
    name_ਮੰਜ਼ਿਲ_Unicode_UTF8(StringPiece name, ByteSink &dest,
                      IDNAInfo &info, UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ override;

ਨਿਜੀ:
    UnicodeString&
    process(ਅਚਲ UnicodeString& src,
            UBool isLabel,
            UBool ਮੰਜ਼ਿਲ_ASCII,
            UnicodeString& dest U_LIFETIME_BOUND,
            IDNAInfo& info,
            UError_ਕੋਡ& error_ਕੋਡ) ਅਚਲ;

    ਖਾਲੀ
    processUTF8(StringPiece src,
                UBool isLabel, UBool ਮੰਜ਼ਿਲ_ASCII,
                ByteSink &dest,
                IDNAInfo &info, UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ;

    UnicodeString&
    processUnicode(ਅਚਲ UnicodeString& src,
                   int32_t label_ਸ਼ੁਰੂ,
                   int32_t mapping_ਸ਼ੁਰੂ,
                   UBool isLabel,
                   UBool ਮੰਜ਼ਿਲ_ASCII,
                   UnicodeString& dest U_LIFETIME_BOUND,
                   IDNAInfo& info,
                   UError_ਕੋਡ& error_ਕੋਡ) ਅਚਲ;

    // returns the new dest.length()
    int32_t
    mapDevChars(UnicodeString &dest, int32_t label_ਸ਼ੁਰੂ, int32_t mapping_ਸ਼ੁਰੂ,
                UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ;

    // returns the new label length
    int32_t
    processLabel(UnicodeString &dest,
                 int32_t label_ਸ਼ੁਰੂ, int32_t label_ਲੰਮਾਈ,
                 UBool ਮੰਜ਼ਿਲ_ASCII,
                 IDNAInfo &info, UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ;
    int32_t
    markBadACELabel(UnicodeString &dest,
                    int32_t label_ਸ਼ੁਰੂ, int32_t label_ਲੰਮਾਈ,
                    UBool ਮੰਜ਼ਿਲ_ASCII, IDNAInfo &info, UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ;

    ਖਾਲੀ
    checkLabelBiDi(ਅਚਲ char16_t *label, int32_t label_ਲੰਮਾਈ, IDNAInfo &info) ਅਚਲ;

    UBool
    isLabelOkContextJ(ਅਚਲ char16_t *label, int32_t label_ਲੰਮਾਈ) ਅਚਲ;

    ਖਾਲੀ
    checkLabelContextO(ਅਚਲ char16_t *label, int32_t label_ਲੰਮਾਈ, IDNAInfo &info) ਅਚਲ;

    ਅਚਲ Normalizer2 &uts46Norm2;  // uts46.nrm
    uint32_t options;
};

IDNA *
IDNA::createUTS46Instance(uint32_t options, UError_ਕੋਡ &error_ਕੋਡ) {
    ਜੇ(U_SUCCESS(error_ਕੋਡ)) {
        IDNA *idna=ਨਵਾਂ UTS46(options, error_ਕੋਡ);
        ਜੇ(idna==ਖਾਲੀ_ਸੂਚਕ) {
            error_ਕੋਡ=U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        } ਨਹੀਂਤਾਂ ਜੇ(U_FAILURE(error_ਕੋਡ)) {
            ਮਿਟਾਓ idna;
            idna=ਖਾਲੀ_ਸੂਚਕ;
        }
        ਵਾਪਸ idna;
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
}

// UTS46 implementation ---------------------------------------------------- ***

UTS46::UTS46(uint32_t opt, UError_ਕੋਡ &error_ਕੋਡ)
        : uts46Norm2(*Normalizer2::getInstance(ਖਾਲੀ_ਸੂਚਕ, "uts46", UNORM2_COMPOSE, error_ਕੋਡ)),
          options(opt) {}

UTS46::~UTS46() {}

UnicodeString&
UTS46::label_ਮੰਜ਼ਿਲ_ASCII(ਅਚਲ UnicodeString& label,
                    UnicodeString& dest U_LIFETIME_BOUND,
                    IDNAInfo& info,
                    UError_ਕੋਡ& error_ਕੋਡ) ਅਚਲ {
    ਵਾਪਸ process(label, ਸੱਚ, ਸੱਚ, dest, info, error_ਕੋਡ);
}

UnicodeString&
UTS46::label_ਮੰਜ਼ਿਲ_Unicode(ਅਚਲ UnicodeString& label,
                      UnicodeString& dest U_LIFETIME_BOUND,
                      IDNAInfo& info,
                      UError_ਕੋਡ& error_ਕੋਡ) ਅਚਲ {
    ਵਾਪਸ process(label, ਸੱਚ, ਝੂਠ, dest, info, error_ਕੋਡ);
}

UnicodeString&
UTS46::name_ਮੰਜ਼ਿਲ_ASCII(ਅਚਲ UnicodeString& name,
                   UnicodeString& dest U_LIFETIME_BOUND,
                   IDNAInfo& info,
                   UError_ਕੋਡ& error_ਕੋਡ) ਅਚਲ {
    process(name, ਝੂਠ, ਸੱਚ, dest, info, error_ਕੋਡ);
    ਜੇ( dest.length()>=254 && (info.errors&UIDNA_ERROR_DOMAIN_NAME_TOO_LONG)==0 &&
        isASCIIString(dest) &&
        (dest.length()>254 || dest[253]!=0x2e)
    ) {
        info.errors|=UIDNA_ERROR_DOMAIN_NAME_TOO_LONG;
    }
    ਵਾਪਸ dest;
}

UnicodeString&
UTS46::name_ਮੰਜ਼ਿਲ_Unicode(ਅਚਲ UnicodeString& name,
                     UnicodeString& dest U_LIFETIME_BOUND,
                     IDNAInfo& info,
                     UError_ਕੋਡ& error_ਕੋਡ) ਅਚਲ {
    ਵਾਪਸ process(name, ਝੂਠ, ਝੂਠ, dest, info, error_ਕੋਡ);
}

ਖਾਲੀ
UTS46::label_ਮੰਜ਼ਿਲ_ASCII_UTF8(StringPiece label, ByteSink &dest,
                         IDNAInfo &info, UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ {
    processUTF8(label, ਸੱਚ, ਸੱਚ, dest, info, error_ਕੋਡ);
}

ਖਾਲੀ
UTS46::label_ਮੰਜ਼ਿਲ_Unicode_UTF8(StringPiece label, ByteSink &dest,
                          IDNAInfo &info, UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ {
    processUTF8(label, ਸੱਚ, ਝੂਠ, dest, info, error_ਕੋਡ);
}

ਖਾਲੀ
UTS46::name_ਮੰਜ਼ਿਲ_ASCII_UTF8(StringPiece name, ByteSink &dest,
                        IDNAInfo &info, UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ {
    processUTF8(name, ਝੂਠ, ਸੱਚ, dest, info, error_ਕੋਡ);
}

ਖਾਲੀ
UTS46::name_ਮੰਜ਼ਿਲ_Unicode_UTF8(StringPiece name, ByteSink &dest,
                         IDNAInfo &info, UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ {
    processUTF8(name, ਝੂਠ, ਝੂਠ, dest, info, error_ਕੋਡ);
}

// UTS #46 data for ASCII characters.
// The normalizer (using uts46.nrm) maps uppercase ASCII letters to lowercase
// and passes through all other ASCII characters.
// If UIDNA_USE_STD3_RULES is set, then non-LDH characters are disallowed
// using this data.
// The ASCII fastpath also uses this data.
// Values: -1=disallowed  0==valid  1==mapped (lowercase)
ਸਥਿਰ ਅਚਲ int8_t ascii_ਅੰਕੜੇ[128]={
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
UTS46::process(ਅਚਲ UnicodeString& src,
               UBool isLabel,
               UBool ਮੰਜ਼ਿਲ_ASCII,
               UnicodeString& dest U_LIFETIME_BOUND,
               IDNAInfo& info,
               UError_ਕੋਡ& error_ਕੋਡ) ਅਚਲ {
    // uts46Norm2.normalize() would do all of this error checking and setup,
    // but with the ASCII fastpath we do not always call it, and do not
    // call it first.
    ਜੇ(U_FAILURE(error_ਕੋਡ)) {
        dest.ਸੈੱਟ_ਮੰਜ਼ਿਲ_Bogus();
        ਵਾਪਸ dest;
    }
    ਅਚਲ char16_t *srcArray=src.get_ਬਫਰ();
    ਜੇ(&dest==&src || srcArray==ਖਾਲੀ_ਸੂਚਕ) {
        error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        dest.ਸੈੱਟ_ਮੰਜ਼ਿਲ_Bogus();
        ਵਾਪਸ dest;
    }
    // Arguments are fine, reset output values.
    dest.remove();
    info.reset();
    int32_t src_ਲੰਮਾਈ=src.length();
    ਜੇ(src_ਲੰਮਾਈ==0) {
        info.errors|=UIDNA_ERROR_ਖਾਲੀ_ਥਾਵਾਂ_LABEL;
        ਵਾਪਸ dest;
    }
    char16_t *destArray=dest.get_ਬਫਰ(src_ਲੰਮਾਈ);
    ਜੇ(destArray==ਖਾਲੀ_ਸੂਚਕ) {
        error_ਕੋਡ=U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ dest;
    }
    // ASCII fastpath
    UBool disallowNonLDHDot=(options&UIDNA_USE_STD3_ਨਿਯਮਾਂ)!=0;
    int32_t label_ਸ਼ੁਰੂ=0;
    int32_t i;
    ਲਈ(i=0;; ++i) {
        ਜੇ(i==src_ਲੰਮਾਈ) {
            ਜੇ(ਮੰਜ਼ਿਲ_ASCII) {
                ਜੇ((i-label_ਸ਼ੁਰੂ)>63) {
                    info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
                }
                // There is a trailing dot if labelStart==i.
                ਜੇ(!isLabel && i>=254 && (i>254 || label_ਸ਼ੁਰੂ<i)) {
                    info.errors|=UIDNA_ERROR_DOMAIN_NAME_TOO_LONG;
                }
            }
            info.errors|=info.labelErrors;
            dest.release_ਬਫਰ(i);
            ਵਾਪਸ dest;
        }
        char16_t c=srcArray[i];
        ਜੇ(c>0x7f) {
            ਰੋਕੋ;
        }
        ਪੂਰਨ c_ਅੰਕੜੇ=ascii_ਅੰਕੜੇ[c];
        ਜੇ(c_ਅੰਕੜੇ>0) {
            destArray[i]=c+0x20;  // Lowercase an uppercase ASCII letter.
        } ਨਹੀਂਤਾਂ ਜੇ(c_ਅੰਕੜੇ<0 && disallowNonLDHDot) {
            ਰੋਕੋ;  // Replacing with U+FFFD can be complicated for toASCII.
        } ਨਹੀਂਤਾਂ {
            destArray[i]=c;
            ਜੇ(c==0x2d) {  // hyphen
                ਜੇ(i==(label_ਸ਼ੁਰੂ+3) && srcArray[i-1]==0x2d) {
                    // "??--..." is Punycode or forbidden.
                    ++i;  // '-' was copied to dest already
                    ਰੋਕੋ;
                }
                ਜੇ(i==label_ਸ਼ੁਰੂ) {
                    // label starts with "-"
                    info.labelErrors|=UIDNA_ERROR_LEADING_HYPHEN;
                }
                ਜੇ((i+1)==src_ਲੰਮਾਈ || srcArray[i+1]==0x2e) {
                    // label ends with "-"
                    info.labelErrors|=UIDNA_ERROR_TRAILING_HYPHEN;
                }
            } ਨਹੀਂਤਾਂ ਜੇ(c==0x2e) {  // dot
                ਜੇ(isLabel) {
                    // Replacing with U+FFFD can be complicated for toASCII.
                    ++i;  // '.' was copied to dest already
                    ਰੋਕੋ;
                }
                ਜੇ(i==label_ਸ਼ੁਰੂ) {
                    info.labelErrors|=UIDNA_ERROR_ਖਾਲੀ_ਥਾਵਾਂ_LABEL;
                }
                ਜੇ(ਮੰਜ਼ਿਲ_ASCII && (i-label_ਸ਼ੁਰੂ)>63) {
                    info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
                }
                info.errors|=info.labelErrors;
                info.labelErrors=0;
                label_ਸ਼ੁਰੂ=i+1;
            }
        }
    }
    info.errors|=info.labelErrors;
    dest.release_ਬਫਰ(i);
    processUnicode(src, label_ਸ਼ੁਰੂ, i, isLabel, ਮੰਜ਼ਿਲ_ASCII, dest, info, error_ਕੋਡ);
    ਜੇ( info.isBiDi && U_SUCCESS(error_ਕੋਡ) && (info.errors&severeErrors)==0 &&
        (!info.isOkBiDi || (label_ਸ਼ੁਰੂ>0 && !isASCIIOkBiDi(dest.get_ਬਫਰ(), label_ਸ਼ੁਰੂ)))
    ) {
        info.errors|=UIDNA_ERROR_BIDI;
    }
    ਵਾਪਸ dest;
}

ਖਾਲੀ
UTS46::processUTF8(StringPiece src,
                   UBool isLabel, UBool ਮੰਜ਼ਿਲ_ASCII,
                   ByteSink &dest,
                   IDNAInfo &info, UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ {
    ਜੇ(U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ;
    }
    ਅਚਲ ਅੱਖਰ *srcArray=src.ਅੰਕੜੇ();
    int32_t src_ਲੰਮਾਈ=src.length();
    ਜੇ(srcArray==ਖਾਲੀ_ਸੂਚਕ && src_ਲੰਮਾਈ!=0) {
        error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ;
    }
    // Arguments are fine, reset output values.
    info.reset();
    ਜੇ(src_ਲੰਮਾਈ==0) {
        info.errors|=UIDNA_ERROR_ਖਾਲੀ_ਥਾਵਾਂ_LABEL;
        dest.Flush();
        ਵਾਪਸ;
    }
    UnicodeString destString;
    int32_t label_ਸ਼ੁਰੂ=0;
    ਜੇ(src_ਲੰਮਾਈ<=256) {  // length of stackArray[]
        // ASCII fastpath
        ਅੱਖਰ ਢੇਰ_Array[256];
        int32_t dest_ਸਮਰੱਥਾ;
        ਅੱਖਰ *destArray=dest.Get_Append_ਬਫਰ(src_ਲੰਮਾਈ, src_ਲੰਮਾਈ+20,
                                             ਢੇਰ_Array, UPRV_LENGTHOF(ਢੇਰ_Array), &dest_ਸਮਰੱਥਾ);
        UBool disallowNonLDHDot=(options&UIDNA_USE_STD3_ਨਿਯਮਾਂ)!=0;
        int32_t i;
        ਲਈ(i=0;; ++i) {
            ਜੇ(i==src_ਲੰਮਾਈ) {
                ਜੇ(ਮੰਜ਼ਿਲ_ASCII) {
                    ਜੇ((i-label_ਸ਼ੁਰੂ)>63) {
                        info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
                    }
                    // There is a trailing dot if labelStart==i.
                    ਜੇ(!isLabel && i>=254 && (i>254 || label_ਸ਼ੁਰੂ<i)) {
                        info.errors|=UIDNA_ERROR_DOMAIN_NAME_TOO_LONG;
                    }
                }
                info.errors|=info.labelErrors;
                dest.Append(destArray, i);
                dest.Flush();
                ਵਾਪਸ;
            }
            ਅੱਖਰ c=srcArray[i];
            ਜੇ (static_cast<int8_t>(c) < 0) { // (uint8_t)c>0x7f
                ਰੋਕੋ;
            }
            ਪੂਰਨ c_ਅੰਕੜੇ = ascii_ਅੰਕੜੇ[static_cast<ਪੂਰਨ>(c)]; // Cast: gcc warns about indexing with a char.
            ਜੇ(c_ਅੰਕੜੇ>0) {
                destArray[i]=c+0x20;  // Lowercase an uppercase ASCII letter.
            } ਨਹੀਂਤਾਂ ਜੇ(c_ਅੰਕੜੇ<0 && disallowNonLDHDot) {
                ਰੋਕੋ;  // Replacing with U+FFFD can be complicated for toASCII.
            } ਨਹੀਂਤਾਂ {
                destArray[i]=c;
                ਜੇ(c==0x2d) {  // hyphen
                    ਜੇ(i==(label_ਸ਼ੁਰੂ+3) && srcArray[i-1]==0x2d) {
                        // "??--..." is Punycode or forbidden.
                        ਰੋਕੋ;
                    }
                    ਜੇ(i==label_ਸ਼ੁਰੂ) {
                        // label starts with "-"
                        info.labelErrors|=UIDNA_ERROR_LEADING_HYPHEN;
                    }
                    ਜੇ((i+1)==src_ਲੰਮਾਈ || srcArray[i+1]==0x2e) {
                        // label ends with "-"
                        info.labelErrors|=UIDNA_ERROR_TRAILING_HYPHEN;
                    }
                } ਨਹੀਂਤਾਂ ਜੇ(c==0x2e) {  // dot
                    ਜੇ(isLabel) {
                        ਰੋਕੋ;  // Replacing with U+FFFD can be complicated for toASCII.
                    }
                    ਜੇ(i==label_ਸ਼ੁਰੂ) {
                        info.labelErrors|=UIDNA_ERROR_ਖਾਲੀ_ਥਾਵਾਂ_LABEL;
                    }
                    ਜੇ(ਮੰਜ਼ਿਲ_ASCII && (i-label_ਸ਼ੁਰੂ)>63) {
                        info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
                    }
                    info.errors|=info.labelErrors;
                    info.labelErrors=0;
                    label_ਸ਼ੁਰੂ=i+1;
                }
            }
        }
        info.errors|=info.labelErrors;
        // Convert the processed ASCII prefix of the current label to UTF-16.
        int32_t mapping_ਸ਼ੁਰੂ=i-label_ਸ਼ੁਰੂ;
        destString=UnicodeString::fromUTF8(StringPiece(destArray+label_ਸ਼ੁਰੂ, mapping_ਸ਼ੁਰੂ));
        // Output the previous ASCII labels and process the rest of src in UTF-16.
        dest.Append(destArray, label_ਸ਼ੁਰੂ);
        processUnicode(UnicodeString::fromUTF8(StringPiece(src, label_ਸ਼ੁਰੂ)), 0, mapping_ਸ਼ੁਰੂ,
                       isLabel, ਮੰਜ਼ਿਲ_ASCII,
                       destString, info, error_ਕੋਡ);
    } ਨਹੀਂਤਾਂ {
        // src is too long for the ASCII fastpath implementation.
        processUnicode(UnicodeString::fromUTF8(src), 0, 0,
                       isLabel, ਮੰਜ਼ਿਲ_ASCII,
                       destString, info, error_ਕੋਡ);
    }
    destString.ਮੰਜ਼ਿਲ_UTF8(dest);  // calls dest.Flush()
    ਜੇ(ਮੰਜ਼ਿਲ_ASCII && !isLabel) {
        // length==labelStart==254 means that there is a trailing dot (ok) and
        // destString is empty (do not index at 253-labelStart).
        int32_t length=label_ਸ਼ੁਰੂ+destString.length();
        ਜੇ( length>=254 && isASCIIString(destString) &&
            (length>254 ||
             (label_ਸ਼ੁਰੂ<254 && destString[253-label_ਸ਼ੁਰੂ]!=0x2e))
        ) {
            info.errors|=UIDNA_ERROR_DOMAIN_NAME_TOO_LONG;
        }
    }
    ਜੇ( info.isBiDi && U_SUCCESS(error_ਕੋਡ) && (info.errors&severeErrors)==0 &&
        (!info.isOkBiDi || (label_ਸ਼ੁਰੂ>0 && !isASCIIOkBiDi(srcArray, label_ਸ਼ੁਰੂ)))
    ) {
        info.errors|=UIDNA_ERROR_BIDI;
    }
}

UnicodeString&
UTS46::processUnicode(ਅਚਲ UnicodeString& src,
                      int32_t label_ਸ਼ੁਰੂ,
                      int32_t mapping_ਸ਼ੁਰੂ,
                      UBool isLabel,
                      UBool ਮੰਜ਼ਿਲ_ASCII,
                      UnicodeString& dest U_LIFETIME_BOUND,
                      IDNAInfo& info,
                      UError_ਕੋਡ& error_ਕੋਡ) ਅਚਲ {
    ਜੇ(mapping_ਸ਼ੁਰੂ==0) {
        uts46Norm2.normalize(src, dest, error_ਕੋਡ);
    } ਨਹੀਂਤਾਂ {
        uts46Norm2.normalize_ਦੂਜਾ_And_Append(dest, src.ਆਰਜ਼ੀ_Sub_String(mapping_ਸ਼ੁਰੂ), error_ਕੋਡ);
    }
    ਜੇ(U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ dest;
    }
    UBool doMapDevChars=
        ਮੰਜ਼ਿਲ_ASCII ? (options&UIDNA_NONTRANSITIONAL_ਮੰਜ਼ਿਲ_ASCII)==0 :
                  (options&UIDNA_NONTRANSITIONAL_ਮੰਜ਼ਿਲ_UNICODE)==0;
    ਅਚਲ char16_t *destArray=dest.get_ਬਫਰ();
    int32_t dest_ਲੰਮਾਈ=dest.length();
    int32_t label_ਹੱਦਬੰਦੀ=label_ਸ਼ੁਰੂ;
    ਜਦੋਂ(label_ਹੱਦਬੰਦੀ<dest_ਲੰਮਾਈ) {
        char16_t c=destArray[label_ਹੱਦਬੰਦੀ];
        ਜੇ(c==0x2e && !isLabel) {
            int32_t label_ਲੰਮਾਈ=label_ਹੱਦਬੰਦੀ-label_ਸ਼ੁਰੂ;
            int32_t new_ਲੰਮਾਈ=processLabel(dest, label_ਸ਼ੁਰੂ, label_ਲੰਮਾਈ,
                                            ਮੰਜ਼ਿਲ_ASCII, info, error_ਕੋਡ);
            info.errors|=info.labelErrors;
            info.labelErrors=0;
            ਜੇ(U_FAILURE(error_ਕੋਡ)) {
                ਵਾਪਸ dest;
            }
            destArray=dest.get_ਬਫਰ();
            dest_ਲੰਮਾਈ+=new_ਲੰਮਾਈ-label_ਲੰਮਾਈ;
            label_ਹੱਦਬੰਦੀ=label_ਸ਼ੁਰੂ+=new_ਲੰਮਾਈ+1;
            ਜਾਰੀ;
        } ਨਹੀਂਤਾਂ ਜੇ(c<0xdf) {
            // pass
        } ਨਹੀਂਤਾਂ ਜੇ(c<=0x200d && (c==0xdf || c==0x3c2 || c>=0x200c)) {
            info.isTransDiff=ਸੱਚ;
            ਜੇ(doMapDevChars) {
                dest_ਲੰਮਾਈ=mapDevChars(dest, label_ਸ਼ੁਰੂ, label_ਹੱਦਬੰਦੀ, error_ਕੋਡ);
                ਜੇ(U_FAILURE(error_ਕੋਡ)) {
                    ਵਾਪਸ dest;
                }
                destArray=dest.get_ਬਫਰ();
                // All deviation characters have been mapped, no need to check for them again.
                doMapDevChars=ਝੂਠ;
                // Do not increment labelLimit in case c was removed.
                ਜਾਰੀ;
            }
        } ਨਹੀਂਤਾਂ ਜੇ(U16_IS_SURROGATE(c)) {
            ਜੇ(U16_IS_SURROGATE_LEAD(c) ?
                    (label_ਹੱਦਬੰਦੀ+1)==dest_ਲੰਮਾਈ || !U16_IS_TRAIL(destArray[label_ਹੱਦਬੰਦੀ+1]) :
                    label_ਹੱਦਬੰਦੀ==label_ਸ਼ੁਰੂ || !U16_IS_LEAD(destArray[label_ਹੱਦਬੰਦੀ-1])) {
                // Map an unpaired surrogate to U+FFFD before normalization so that when
                // that removes characters we do not turn two unpaired ones into a pair.
                info.labelErrors|=UIDNA_ERROR_DISALLOWED;
                dest.ਸੈੱਟ_Char_At(label_ਹੱਦਬੰਦੀ, 0xfffd);
                destArray=dest.get_ਬਫਰ();
            }
        }
        ++label_ਹੱਦਬੰਦੀ;
    }
    // Permit an empty label at the end (0<labelStart==labelLimit==destLength is ok)
    // but not an empty label elsewhere nor a completely empty domain name.
    // processLabel() sets UIDNA_ERROR_EMPTY_LABEL when labelLength==0.
    ਜੇ(0==label_ਸ਼ੁਰੂ || label_ਸ਼ੁਰੂ<label_ਹੱਦਬੰਦੀ) {
        processLabel(dest, label_ਸ਼ੁਰੂ, label_ਹੱਦਬੰਦੀ-label_ਸ਼ੁਰੂ,
                      ਮੰਜ਼ਿਲ_ASCII, info, error_ਕੋਡ);
        info.errors|=info.labelErrors;
    }
    ਵਾਪਸ dest;
}

int32_t
UTS46::mapDevChars(UnicodeString &dest, int32_t label_ਸ਼ੁਰੂ, int32_t mapping_ਸ਼ੁਰੂ,
                   UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ {
    ਜੇ(U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ 0;
    }
    int32_t length=dest.length();
    char16_t *s=dest.get_ਬਫਰ(dest[mapping_ਸ਼ੁਰੂ]==0xdf ? length+1 : length);
    ਜੇ(s==ਖਾਲੀ_ਸੂਚਕ) {
        error_ਕੋਡ=U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ length;
    }
    int32_t capacity=dest.get_ਸਮਰੱਥਾ();
    UBool didMapDevChars=ਝੂਠ;
    int32_t read_ਸੂਚਕਾਂਕ=mapping_ਸ਼ੁਰੂ, write_ਸੂਚਕਾਂਕ=mapping_ਸ਼ੁਰੂ;
    ਕਰੋ {
        char16_t c=s[read_ਸੂਚਕਾਂਕ++];
        ਚੋਣ(c) {
        ਹਾਲਤ 0xdf:
            // Map sharp s to ss.
            didMapDevChars=ਸੱਚ;
            s[write_ਸੂਚਕਾਂਕ++]=0x73;  // Replace sharp s with first s.
            // Insert second s and account for possible buffer reallocation.
            ਜੇ(write_ਸੂਚਕਾਂਕ==read_ਸੂਚਕਾਂਕ) {
                ਜੇ(length==capacity) {
                    dest.release_ਬਫਰ(length);
                    s=dest.get_ਬਫਰ(length+1);
                    ਜੇ(s==ਖਾਲੀ_ਸੂਚਕ) {
                        error_ਕੋਡ=U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
                        ਵਾਪਸ length;
                    }
                    capacity=dest.get_ਸਮਰੱਥਾ();
                }
                u_memmove(s+write_ਸੂਚਕਾਂਕ+1, s+write_ਸੂਚਕਾਂਕ, length-write_ਸੂਚਕਾਂਕ);
                ++read_ਸੂਚਕਾਂਕ;
            }
            s[write_ਸੂਚਕਾਂਕ++]=0x73;
            ++length;
            ਰੋਕੋ;
        ਹਾਲਤ 0x3c2:  // Map final sigma to nonfinal sigma.
            didMapDevChars=ਸੱਚ;
            s[write_ਸੂਚਕਾਂਕ++]=0x3c3;
            ਰੋਕੋ;
        ਹਾਲਤ 0x200c:  // Ignore/remove ZWNJ.
        ਹਾਲਤ 0x200d:  // Ignore/remove ZWJ.
            didMapDevChars=ਸੱਚ;
            --length;
            ਰੋਕੋ;
        ਮੂਲ:
            // Only really necessary if writeIndex was different from readIndex.
            s[write_ਸੂਚਕਾਂਕ++]=c;
            ਰੋਕੋ;
        }
    } ਜਦੋਂ(write_ਸੂਚਕਾਂਕ<length);
    dest.release_ਬਫਰ(length);
    ਜੇ(didMapDevChars) {
        // Mapping deviation characters might have resulted in an un-NFC string.
        // We could use either the NFC or the UTS #46 normalizer.
        // By using the UTS #46 normalizer again, we avoid having to load a second .nrm data file.
        UnicodeString normalized;
        uts46Norm2.normalize(dest.ਆਰਜ਼ੀ_Sub_String(label_ਸ਼ੁਰੂ), normalized, error_ਕੋਡ);
        ਜੇ(U_SUCCESS(error_ਕੋਡ)) {
            dest.replace(label_ਸ਼ੁਰੂ, 0x7fffffff, normalized);
            ਜੇ(dest.isBogus()) {
                error_ਕੋਡ=U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
            }
            ਵਾਪਸ dest.length();
        }
    }
    ਵਾਪਸ length;
}

// Replace the label in dest with the label string, if the label was modified.
// If &label==&dest then the label was modified in-place and labelLength
// is the new label length, different from label.length().
// If &label!=&dest then labelLength==label.length().
// Returns labelLength (= the new label length).
ਸਥਿਰ int32_t
replaceLabel(UnicodeString &dest, int32_t dest_Label_ਸ਼ੁਰੂ, int32_t dest_Label_ਲੰਮਾਈ,
             ਅਚਲ UnicodeString &label, int32_t label_ਲੰਮਾਈ, UError_ਕੋਡ &error_ਕੋਡ) {
    ਜੇ(U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ 0;
    }
    ਜੇ(&label!=&dest) {
        dest.replace(dest_Label_ਸ਼ੁਰੂ, dest_Label_ਲੰਮਾਈ, label);
        ਜੇ(dest.isBogus()) {
            error_ਕੋਡ=U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
            ਵਾਪਸ 0;
        }
    }
    ਵਾਪਸ label_ਲੰਮਾਈ;
}

int32_t
UTS46::processLabel(UnicodeString &dest,
                    int32_t label_ਸ਼ੁਰੂ, int32_t label_ਲੰਮਾਈ,
                    UBool ਮੰਜ਼ਿਲ_ASCII,
                    IDNAInfo &info, UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ {
    ਜੇ(U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ 0;
    }
    UnicodeString fromPunycode;
    UnicodeString *labelString;
    ਅਚਲ char16_t *label=dest.get_ਬਫਰ()+label_ਸ਼ੁਰੂ;
    int32_t dest_Label_ਸ਼ੁਰੂ=label_ਸ਼ੁਰੂ;
    int32_t dest_Label_ਲੰਮਾਈ=label_ਲੰਮਾਈ;
    UBool wasPunycode;
    ਜੇ(label_ਲੰਮਾਈ>=4 && label[0]==0x78 && label[1]==0x6e && label[2]==0x2d && label[3]==0x2d) {
        // Label starts with "xn--", try to un-Punycode it.
        // In IDNA2008, labels like "xn--" (decodes to an empty string) and
        // "xn--ASCII-" (decodes to just "ASCII") fail the round-trip validation from
        // comparing the ToUnicode input with the back-to-ToASCII output.
        // They are alternate encodings of the respective ASCII labels.
        // Ignore "xn---" here: It will fail Punycode.decode() which logically comes before
        // the round-trip verification.
        ਜੇ(label_ਲੰਮਾਈ==4 || (label_ਲੰਮਾਈ>5 && label[label_ਲੰਮਾਈ-1]==u'-')) {
            info.labelErrors|=UIDNA_ERROR_INVALID_ACE_LABEL;
            ਵਾਪਸ markBadACELabel(dest, label_ਸ਼ੁਰੂ, label_ਲੰਮਾਈ, ਮੰਜ਼ਿਲ_ASCII, info, error_ਕੋਡ);
        }
        wasPunycode=ਸੱਚ;
        char16_t *unicode_ਬਫਰ=fromPunycode.get_ਬਫਰ(-1);  // capacity==-1: most labels should fit
        ਜੇ(unicode_ਬਫਰ==ਖਾਲੀ_ਸੂਚਕ) {
            // Should never occur if we used capacity==-1 which uses the internal buffer.
            error_ਕੋਡ=U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
            ਵਾਪਸ label_ਲੰਮਾਈ;
        }
        UError_ਕੋਡ punycode_Error_ਕੋਡ=U_ZERO_ERROR;
        int32_t unicode_ਲੰਮਾਈ=u_strFromPunycode(label+4, label_ਲੰਮਾਈ-4,
                                                unicode_ਬਫਰ, fromPunycode.get_ਸਮਰੱਥਾ(),
                                                ਖਾਲੀ_ਸੂਚਕ, &punycode_Error_ਕੋਡ);
        ਜੇ(punycode_Error_ਕੋਡ==U_ਬਫਰ_ਉਛਾਲ_ERROR) {
            fromPunycode.release_ਬਫਰ(0);
            unicode_ਬਫਰ=fromPunycode.get_ਬਫਰ(unicode_ਲੰਮਾਈ);
            ਜੇ(unicode_ਬਫਰ==ਖਾਲੀ_ਸੂਚਕ) {
                error_ਕੋਡ=U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
                ਵਾਪਸ label_ਲੰਮਾਈ;
            }
            punycode_Error_ਕੋਡ=U_ZERO_ERROR;
            unicode_ਲੰਮਾਈ=u_strFromPunycode(label+4, label_ਲੰਮਾਈ-4,
                                            unicode_ਬਫਰ, fromPunycode.get_ਸਮਰੱਥਾ(),
                                            ਖਾਲੀ_ਸੂਚਕ, &punycode_Error_ਕੋਡ);
        }
        fromPunycode.release_ਬਫਰ(unicode_ਲੰਮਾਈ);
        ਜੇ(U_FAILURE(punycode_Error_ਕੋਡ)) {
            info.labelErrors|=UIDNA_ERROR_PUNYCODE;
            ਵਾਪਸ markBadACELabel(dest, label_ਸ਼ੁਰੂ, label_ਲੰਮਾਈ, ਮੰਜ਼ਿਲ_ASCII, info, error_ਕੋਡ);
        }
        // Check for NFC, and for characters that are not
        // valid or deviation characters according to the normalizer.
        // If there is something wrong, then the string will change.
        // Note that the normalizer passes through non-LDH ASCII and deviation characters.
        // Deviation characters are ok in Punycode even in transitional processing.
        // In the code further below, if we find non-LDH ASCII and we have UIDNA_USE_STD3_RULES
        // then we will set UIDNA_ERROR_INVALID_ACE_LABEL there too.
        UBool isValid=uts46Norm2.isNormalized(fromPunycode, error_ਕੋਡ);
        ਜੇ(U_FAILURE(error_ਕੋਡ)) {
            ਵਾਪਸ label_ਲੰਮਾਈ;
        }
        // Unicode 15.1 UTS #46:
        // Added an additional condition in 4.1 Validity Criteria to
        // disallow labels such as xn--xn---epa., which do not round-trip.
        // --> Validity Criteria new criterion 4:
        // If not CheckHyphens, the label must not begin with “xn--”.
        ਜੇ(!isValid || fromPunycode.startsWith(UnicodeString::readOnlyAlias(u"xn--"))) {
            info.labelErrors|=UIDNA_ERROR_INVALID_ACE_LABEL;
            ਵਾਪਸ markBadACELabel(dest, label_ਸ਼ੁਰੂ, label_ਲੰਮਾਈ, ਮੰਜ਼ਿਲ_ASCII, info, error_ਕੋਡ);
        }
        labelString=&fromPunycode;
        label=fromPunycode.get_ਬਫਰ();
        label_ਸ਼ੁਰੂ=0;
        label_ਲੰਮਾਈ=fromPunycode.length();
    } ਨਹੀਂਤਾਂ {
        wasPunycode=ਝੂਠ;
        labelString=&dest;
    }
    // Validity check
    ਜੇ(label_ਲੰਮਾਈ==0) {
        info.labelErrors|=UIDNA_ERROR_ਖਾਲੀ_ਥਾਵਾਂ_LABEL;
        ਵਾਪਸ replaceLabel(dest, dest_Label_ਸ਼ੁਰੂ, dest_Label_ਲੰਮਾਈ,
                            *labelString, label_ਲੰਮਾਈ, error_ਕੋਡ);
    }
    // labelLength>0
    ਜੇ(label_ਲੰਮਾਈ>=4 && label[2]==0x2d && label[3]==0x2d) {
        // label starts with "??--"
        info.labelErrors|=UIDNA_ERROR_HYPHEN_3_4;
    }
    ਜੇ(label[0]==0x2d) {
        // label starts with "-"
        info.labelErrors|=UIDNA_ERROR_LEADING_HYPHEN;
    }
    ਜੇ(label[label_ਲੰਮਾਈ-1]==0x2d) {
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
    ਅਚਲ char16_t *ਹੱਦਬੰਦੀ=label+label_ਲੰਮਾਈ;
    char16_t oredChars=0;
    // If we enforce STD3 rules, then ASCII characters other than LDH and dot are disallowed.
    UBool disallowNonLDHDot=(options&UIDNA_USE_STD3_ਨਿਯਮਾਂ)!=0;
    ਕਰੋ {
        char16_t c=*s;
        ਜੇ(c<=0x7f) {
            ਜੇ(c==0x2e) {
                info.labelErrors|=UIDNA_ERROR_LABEL_HAS_DOT;
                *s=0xfffd;
            } ਨਹੀਂਤਾਂ ਜੇ(disallowNonLDHDot && ascii_ਅੰਕੜੇ[c]<0) {
                info.labelErrors|=UIDNA_ERROR_DISALLOWED;
                *s=0xfffd;
            }
        } ਨਹੀਂਤਾਂ {
            oredChars|=c;
            ਜੇ(c==0xfffd) {
                info.labelErrors|=UIDNA_ERROR_DISALLOWED;
            }
        }
        ++s;
    } ਜਦੋਂ(s<ਹੱਦਬੰਦੀ);
    // Check for a leading combining mark after other validity checks
    // so that we don't report UIDNA_ERROR_DISALLOWED for the U+FFFD from here.
    UChar32 c;
    int32_t cp_ਲੰਮਾਈ=0;
    // "Unsafe" is ok because unpaired surrogates were mapped to U+FFFD.
    U16_ਅਗਲਾ_UNSAFE(label, cp_ਲੰਮਾਈ, c);
    ਜੇ((U_GET_GC_ਨਕਾਬ(c)&U_GC_M_ਨਕਾਬ)!=0) {
        info.labelErrors|=UIDNA_ERROR_LEADING_COMBINING_MARK;
        labelString->replace(label_ਸ਼ੁਰੂ, cp_ਲੰਮਾਈ, static_cast<char16_t>(0xfffd));
        label=labelString->get_ਬਫਰ()+label_ਸ਼ੁਰੂ;
        label_ਲੰਮਾਈ+=1-cp_ਲੰਮਾਈ;
        ਜੇ(labelString==&dest) {
            dest_Label_ਲੰਮਾਈ=label_ਲੰਮਾਈ;
        }
    }
    ਜੇ((info.labelErrors&severeErrors)==0) {
        // Do contextual checks only if we do not have U+FFFD from a severe error
        // because U+FFFD can make these checks fail.
        ਜੇ((options&UIDNA_CHECK_BIDI)!=0 && (!info.isBiDi || info.isOkBiDi)) {
            checkLabelBiDi(label, label_ਲੰਮਾਈ, info);
        }
        ਜੇ( (options&UIDNA_CHECK_CONTEXTJ)!=0 && (oredChars&0x200c)==0x200c &&
            !isLabelOkContextJ(label, label_ਲੰਮਾਈ)
        ) {
            info.labelErrors|=UIDNA_ERROR_CONTEXTJ;
        }
        ਜੇ((options&UIDNA_CHECK_CONTEXTO)!=0 && oredChars>=0xb7) {
            checkLabelContextO(label, label_ਲੰਮਾਈ, info);
        }
        ਜੇ(ਮੰਜ਼ਿਲ_ASCII) {
            ਜੇ(wasPunycode) {
                // Leave a Punycode label unchanged if it has no severe errors.
                ਜੇ(dest_Label_ਲੰਮਾਈ>63) {
                    info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
                }
                ਵਾਪਸ dest_Label_ਲੰਮਾਈ;
            } ਨਹੀਂਤਾਂ ਜੇ(oredChars>=0x80) {
                // Contains non-ASCII characters.
                UnicodeString punycode;
                char16_t *ਬਫਰ=punycode.get_ਬਫਰ(63);  // 63==maximum DNS label length
                ਜੇ(ਬਫਰ==ਖਾਲੀ_ਸੂਚਕ) {
                    error_ਕੋਡ=U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
                    ਵਾਪਸ dest_Label_ਲੰਮਾਈ;
                }
                ਬਫਰ[0]=0x78;  // Write "xn--".
                ਬਫਰ[1]=0x6e;
                ਬਫਰ[2]=0x2d;
                ਬਫਰ[3]=0x2d;
                UError_ਕੋਡ punycode_Error_ਕੋਡ=U_ZERO_ERROR;
                int32_t punycode_ਲੰਮਾਈ=u_str_ਮੰਜ਼ਿਲ_Punycode(label, label_ਲੰਮਾਈ,
                                                      ਬਫਰ+4, punycode.get_ਸਮਰੱਥਾ()-4,
                                                      ਖਾਲੀ_ਸੂਚਕ, &punycode_Error_ਕੋਡ);
                ਜੇ(punycode_Error_ਕੋਡ==U_ਬਫਰ_ਉਛਾਲ_ERROR) {
                    punycode_Error_ਕੋਡ=U_ZERO_ERROR;
                    punycode.release_ਬਫਰ(4);
                    ਬਫਰ=punycode.get_ਬਫਰ(4+punycode_ਲੰਮਾਈ);
                    ਜੇ(ਬਫਰ==ਖਾਲੀ_ਸੂਚਕ) {
                        error_ਕੋਡ=U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
                        ਵਾਪਸ dest_Label_ਲੰਮਾਈ;
                    }
                    punycode_ਲੰਮਾਈ=u_str_ਮੰਜ਼ਿਲ_Punycode(label, label_ਲੰਮਾਈ,
                                                  ਬਫਰ+4, punycode.get_ਸਮਰੱਥਾ()-4,
                                                  ਖਾਲੀ_ਸੂਚਕ, &punycode_Error_ਕੋਡ);
                }
                punycode_ਲੰਮਾਈ+=4;
                punycode.release_ਬਫਰ(punycode_ਲੰਮਾਈ);
                ਜੇ(U_FAILURE(punycode_Error_ਕੋਡ)) {
                    error_ਕੋਡ = punycode_Error_ਕੋਡ;
                    ਵਾਪਸ dest_Label_ਲੰਮਾਈ;
                }
                ਜੇ(punycode_ਲੰਮਾਈ>63) {
                    info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
                }
                ਵਾਪਸ replaceLabel(dest, dest_Label_ਸ਼ੁਰੂ, dest_Label_ਲੰਮਾਈ,
                                    punycode, punycode_ਲੰਮਾਈ, error_ਕੋਡ);
            } ਨਹੀਂਤਾਂ {
                // all-ASCII label
                ਜੇ(label_ਲੰਮਾਈ>63) {
                    info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
                }
            }
        }
    } ਨਹੀਂਤਾਂ {
        // If a Punycode label has severe errors,
        // then leave it but make sure it does not look valid.
        ਜੇ(wasPunycode) {
            info.labelErrors|=UIDNA_ERROR_INVALID_ACE_LABEL;
            ਵਾਪਸ markBadACELabel(dest, dest_Label_ਸ਼ੁਰੂ, dest_Label_ਲੰਮਾਈ, ਮੰਜ਼ਿਲ_ASCII, info, error_ਕੋਡ);
        }
    }
    ਵਾਪਸ replaceLabel(dest, dest_Label_ਸ਼ੁਰੂ, dest_Label_ਲੰਮਾਈ,
                        *labelString, label_ਲੰਮਾਈ, error_ਕੋਡ);
}

// Make sure an ACE label does not look valid.
// Append U+FFFD if the label has only LDH characters.
// If UIDNA_USE_STD3_RULES, also replace disallowed ASCII characters with U+FFFD.
int32_t
UTS46::markBadACELabel(UnicodeString &dest,
                       int32_t label_ਸ਼ੁਰੂ, int32_t label_ਲੰਮਾਈ,
                       UBool ਮੰਜ਼ਿਲ_ASCII, IDNAInfo &info, UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ {
    ਜੇ(U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ 0;
    }
    UBool disallowNonLDHDot=(options&UIDNA_USE_STD3_ਨਿਯਮਾਂ)!=0;
    UBool isASCII=ਸੱਚ;
    UBool onlyLDH=ਸੱਚ;
    ਅਚਲ char16_t *label=dest.get_ਬਫਰ()+label_ਸ਼ੁਰੂ;
    ਅਚਲ char16_t *ਹੱਦਬੰਦੀ=label+label_ਲੰਮਾਈ;
    // Start after the initial "xn--".
    // Ok to cast away const because we own the UnicodeString.
    ਲਈ(char16_t *s=const_cast<char16_t *>(label+4); s<ਹੱਦਬੰਦੀ; ++s) {
        char16_t c=*s;
        ਜੇ(c<=0x7f) {
            ਜੇ(c==0x2e) {
                info.labelErrors|=UIDNA_ERROR_LABEL_HAS_DOT;
                *s=0xfffd;
                isASCII=onlyLDH=ਝੂਠ;
            } ਨਹੀਂਤਾਂ ਜੇ(ascii_ਅੰਕੜੇ[c]<0) {
                onlyLDH=ਝੂਠ;
                ਜੇ(disallowNonLDHDot) {
                    *s=0xfffd;
                    isASCII=ਝੂਠ;
                }
            }
        } ਨਹੀਂਤਾਂ {
            isASCII=onlyLDH=ਝੂਠ;
        }
    }
    ਜੇ(onlyLDH) {
        dest.insert(label_ਸ਼ੁਰੂ + label_ਲੰਮਾਈ, static_cast<char16_t>(0xfffd));
        ਜੇ(dest.isBogus()) {
            error_ਕੋਡ=U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
            ਵਾਪਸ 0;
        }
        ++label_ਲੰਮਾਈ;
    } ਨਹੀਂਤਾਂ {
        ਜੇ(ਮੰਜ਼ਿਲ_ASCII && isASCII && label_ਲੰਮਾਈ>63) {
            info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
        }
    }
    ਵਾਪਸ label_ਲੰਮਾਈ;
}

ਅਚਲ uint32_t L_ਨਕਾਬ=U_ਨਕਾਬ(U_ਖੱਬਾ_ਮੰਜ਼ਿਲ_ਸੱਜਾ);
ਅਚਲ uint32_t R_AL_ਨਕਾਬ=U_ਨਕਾਬ(U_ਸੱਜਾ_ਮੰਜ਼ਿਲ_ਖੱਬਾ)|U_ਨਕਾਬ(U_ਸੱਜਾ_ਮੰਜ਼ਿਲ_ਖੱਬਾ_ARABIC);
ਅਚਲ uint32_t L_R_AL_ਨਕਾਬ=L_ਨਕਾਬ|R_AL_ਨਕਾਬ;

ਅਚਲ uint32_t R_AL_AN_ਨਕਾਬ=R_AL_ਨਕਾਬ|U_ਨਕਾਬ(U_ARABIC_ਸੰਖਿਆ);

ਅਚਲ uint32_t EN_AN_ਨਕਾਬ=U_ਨਕਾਬ(U_EUROPEAN_ਸੰਖਿਆ)|U_ਨਕਾਬ(U_ARABIC_ਸੰਖਿਆ);
ਅਚਲ uint32_t R_AL_EN_AN_ਨਕਾਬ=R_AL_ਨਕਾਬ|EN_AN_ਨਕਾਬ;
ਅਚਲ uint32_t L_EN_ਨਕਾਬ=L_ਨਕਾਬ|U_ਨਕਾਬ(U_EUROPEAN_ਸੰਖਿਆ);

ਅਚਲ uint32_t ES_CS_ET_ON_BN_NSM_ਨਕਾਬ=
    U_ਨਕਾਬ(U_EUROPEAN_ਸੰਖਿਆ_SEPARATOR)|
    U_ਨਕਾਬ(U_COMMON_ਸੰਖਿਆ_SEPARATOR)|
    U_ਨਕਾਬ(U_EUROPEAN_ਸੰਖਿਆ_TERMINATOR)|
    U_ਨਕਾਬ(U_OTHER_NEUTRAL)|
    U_ਨਕਾਬ(U_BOUNDARY_NEUTRAL)|
    U_ਨਕਾਬ(U_DIR_NON_SPACING_MARK);
ਅਚਲ uint32_t L_EN_ES_CS_ET_ON_BN_NSM_ਨਕਾਬ=L_EN_ਨਕਾਬ|ES_CS_ET_ON_BN_NSM_ਨਕਾਬ;
ਅਚਲ uint32_t R_AL_AN_EN_ES_CS_ET_ON_BN_NSM_ਨਕਾਬ=R_AL_ਨਕਾਬ|EN_AN_ਨਕਾਬ|ES_CS_ET_ON_BN_NSM_ਨਕਾਬ;

// We scan the whole label and check both for whether it contains RTL characters
// and whether it passes the BiDi Rule.
// In a BiDi domain name, all labels must pass the BiDi Rule, but we might find
// that a domain name is a BiDi domain name (has an RTL label) only after
// processing several earlier labels.
ਖਾਲੀ
UTS46::checkLabelBiDi(ਅਚਲ char16_t *label, int32_t label_ਲੰਮਾਈ, IDNAInfo &info) ਅਚਲ {
    // IDNA2008 BiDi rule
    // Get the directionality of the first character.
    UChar32 c;
    int32_t i=0;
    U16_ਅਗਲਾ_UNSAFE(label, i, c);
    uint32_t ਪਹਿਲਾ_ਨਕਾਬ=U_ਨਕਾਬ(u_charDirection(c));
    // 1. The first character must be a character with BIDI property L, R
    // or AL.  If it has the R or AL property, it is an RTL label; if it
    // has the L property, it is an LTR label.
    ਜੇ((ਪਹਿਲਾ_ਨਕਾਬ&~L_R_AL_ਨਕਾਬ)!=0) {
        info.isOkBiDi=ਝੂਠ;
    }
    // Get the directionality of the last non-NSM character.
    uint32_t ਆਖਰੀ_ਨਕਾਬ;
    ਲਈ(;;) {
        ਜੇ(i>=label_ਲੰਮਾਈ) {
            ਆਖਰੀ_ਨਕਾਬ=ਪਹਿਲਾ_ਨਕਾਬ;
            ਰੋਕੋ;
        }
        U16_PREV_UNSAFE(label, label_ਲੰਮਾਈ, c);
        UCharDirection dir=u_charDirection(c);
        ਜੇ(dir!=U_DIR_NON_SPACING_MARK) {
            ਆਖਰੀ_ਨਕਾਬ=U_ਨਕਾਬ(dir);
            ਰੋਕੋ;
        }
    }
    // 3. In an RTL label, the end of the label must be a character with
    // BIDI property R, AL, EN or AN, followed by zero or more
    // characters with BIDI property NSM.
    // 6. In an LTR label, the end of the label must be a character with
    // BIDI property L or EN, followed by zero or more characters with
    // BIDI property NSM.
    ਜੇ( (ਪਹਿਲਾ_ਨਕਾਬ&L_ਨਕਾਬ)!=0 ?
            (ਆਖਰੀ_ਨਕਾਬ&~L_EN_ਨਕਾਬ)!=0 :
            (ਆਖਰੀ_ਨਕਾਬ&~R_AL_EN_AN_ਨਕਾਬ)!=0
    ) {
        info.isOkBiDi=ਝੂਠ;
    }
    // Add the directionalities of the intervening characters.
    uint32_t mask=ਪਹਿਲਾ_ਨਕਾਬ|ਆਖਰੀ_ਨਕਾਬ;
    ਜਦੋਂ(i<label_ਲੰਮਾਈ) {
        U16_ਅਗਲਾ_UNSAFE(label, i, c);
        mask|=U_ਨਕਾਬ(u_charDirection(c));
    }
    ਜੇ(ਪਹਿਲਾ_ਨਕਾਬ&L_ਨਕਾਬ) {
        // 5. In an LTR label, only characters with the BIDI properties L, EN,
        // ES, CS, ET, ON, BN and NSM are allowed.
        ਜੇ((mask&~L_EN_ES_CS_ET_ON_BN_NSM_ਨਕਾਬ)!=0) {
            info.isOkBiDi=ਝੂਠ;
        }
    } ਨਹੀਂਤਾਂ {
        // 2. In an RTL label, only characters with the BIDI properties R, AL,
        // AN, EN, ES, CS, ET, ON, BN and NSM are allowed.
        ਜੇ((mask&~R_AL_AN_EN_ES_CS_ET_ON_BN_NSM_ਨਕਾਬ)!=0) {
            info.isOkBiDi=ਝੂਠ;
        }
        // 4. In an RTL label, if an EN is present, no AN may be present, and
        // vice versa.
        ਜੇ((mask&EN_AN_ਨਕਾਬ)==EN_AN_ਨਕਾਬ) {
            info.isOkBiDi=ਝੂਠ;
        }
    }
    // An RTL label is a label that contains at least one character of type
    // R, AL or AN. [...]
    // A "BIDI domain name" is a domain name that contains at least one RTL
    // label. [...]
    // The following rule, consisting of six conditions, applies to labels
    // in BIDI domain names.
    ਜੇ((mask&R_AL_AN_ਨਕਾਬ)!=0) {
        info.isBiDi=ਸੱਚ;
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
ਸਥਿਰ UBool
isASCIIOkBiDi(ਅਚਲ char16_t *s, int32_t length) {
    int32_t label_ਸ਼ੁਰੂ=0;
    ਲਈ(int32_t i=0; i<length; ++i) {
        char16_t c=s[i];
        ਜੇ(c==0x2e) {  // dot
            ਜੇ(i>label_ਸ਼ੁਰੂ) {
                c=s[i-1];
                ਜੇ(!(0x61<=c && c<=0x7a) && !(0x30<=c && c<=0x39)) {
                    // Last character in the label is not an L or EN.
                    ਵਾਪਸ ਝੂਠ;
                }
            }
            label_ਸ਼ੁਰੂ=i+1;
        } ਨਹੀਂਤਾਂ ਜੇ(i==label_ਸ਼ੁਰੂ) {
            ਜੇ(!(0x61<=c && c<=0x7a)) {
                // First character in the label is not an L.
                ਵਾਪਸ ਝੂਠ;
            }
        } ਨਹੀਂਤਾਂ {
            ਜੇ(c<=0x20 && (c>=0x1c || (9<=c && c<=0xd))) {
                // Intermediate character in the label is a B, S or WS.
                ਵਾਪਸ ਝੂਠ;
            }
        }
    }
    ਵਾਪਸ ਸੱਚ;
}

// UTF-8 version, called for source ASCII prefix.
// Can contain uppercase A-Z.
// s[length-1] must be the trailing dot.
ਸਥਿਰ UBool
isASCIIOkBiDi(ਅਚਲ ਅੱਖਰ *s, int32_t length) {
    int32_t label_ਸ਼ੁਰੂ=0;
    ਲਈ(int32_t i=0; i<length; ++i) {
        ਅੱਖਰ c=s[i];
        ਜੇ(c==0x2e) {  // dot
            ਜੇ(i>label_ਸ਼ੁਰੂ) {
                c=s[i-1];
                ਜੇ(!(0x61<=c && c<=0x7a) && !(0x41<=c && c<=0x5a) && !(0x30<=c && c<=0x39)) {
                    // Last character in the label is not an L or EN.
                    ਵਾਪਸ ਝੂਠ;
                }
            }
            label_ਸ਼ੁਰੂ=i+1;
        } ਨਹੀਂਤਾਂ ਜੇ(i==label_ਸ਼ੁਰੂ) {
            ਜੇ(!(0x61<=c && c<=0x7a) && !(0x41<=c && c<=0x5a)) {
                // First character in the label is not an L.
                ਵਾਪਸ ਝੂਠ;
            }
        } ਨਹੀਂਤਾਂ {
            ਜੇ(c<=0x20 && (c>=0x1c || (9<=c && c<=0xd))) {
                // Intermediate character in the label is a B, S or WS.
                ਵਾਪਸ ਝੂਠ;
            }
        }
    }
    ਵਾਪਸ ਸੱਚ;
}

UBool
UTS46::isLabelOkContextJ(ਅਚਲ char16_t *label, int32_t label_ਲੰਮਾਈ) ਅਚਲ {
    // [IDNA2008-Tables]
    // 200C..200D  ; CONTEXTJ    # ZERO WIDTH NON-JOINER..ZERO WIDTH JOINER
    ਲਈ(int32_t i=0; i<label_ਲੰਮਾਈ; ++i) {
        ਜੇ(label[i]==0x200c) {
            // Appendix A.1. ZERO WIDTH NON-JOINER
            // Rule Set:
            //  False;
            //  If Canonical_Combining_Class(Before(cp)) .eq.  Virama Then True;
            //  If RegExpMatch((Joining_Type:{L,D})(Joining_Type:T)*\u200C
            //     (Joining_Type:T)*(Joining_Type:{R,D})) Then True;
            ਜੇ(i==0) {
                ਵਾਪਸ ਝੂਠ;
            }
            UChar32 c;
            int32_t j=i;
            U16_PREV_UNSAFE(label, j, c);
            ਜੇ(uts46Norm2.getCombiningClass(c)==9) {
                ਜਾਰੀ;
            }
            // check precontext (Joining_Type:{L,D})(Joining_Type:T)*
            ਲਈ(;;) {
                UJoiningType type=ubidi_getJoiningType(c);
                ਜੇ(type==U_JT_TRANSPARENT) {
                    ਜੇ(j==0) {
                        ਵਾਪਸ ਝੂਠ;
                    }
                    U16_PREV_UNSAFE(label, j, c);
                } ਨਹੀਂਤਾਂ ਜੇ(type==U_JT_ਖੱਬਾ_JOINING || type==U_JT_DUAL_JOINING) {
                    ਰੋਕੋ;  // precontext fulfilled
                } ਨਹੀਂਤਾਂ {
                    ਵਾਪਸ ਝੂਠ;
                }
            }
            // check postcontext (Joining_Type:T)*(Joining_Type:{R,D})
            ਲਈ(j=i+1;;) {
                ਜੇ(j==label_ਲੰਮਾਈ) {
                    ਵਾਪਸ ਝੂਠ;
                }
                U16_ਅਗਲਾ_UNSAFE(label, j, c);
                UJoiningType type=ubidi_getJoiningType(c);
                ਜੇ(type==U_JT_TRANSPARENT) {
                    // just skip this character
                } ਨਹੀਂਤਾਂ ਜੇ(type==U_JT_ਸੱਜਾ_JOINING || type==U_JT_DUAL_JOINING) {
                    ਰੋਕੋ;  // postcontext fulfilled
                } ਨਹੀਂਤਾਂ {
                    ਵਾਪਸ ਝੂਠ;
                }
            }
        } ਨਹੀਂਤਾਂ ਜੇ(label[i]==0x200d) {
            // Appendix A.2. ZERO WIDTH JOINER (U+200D)
            // Rule Set:
            //  False;
            //  If Canonical_Combining_Class(Before(cp)) .eq.  Virama Then True;
            ਜੇ(i==0) {
                ਵਾਪਸ ਝੂਠ;
            }
            UChar32 c;
            int32_t j=i;
            U16_PREV_UNSAFE(label, j, c);
            ਜੇ(uts46Norm2.getCombiningClass(c)!=9) {
                ਵਾਪਸ ਝੂਠ;
            }
        }
    }
    ਵਾਪਸ ਸੱਚ;
}

ਖਾਲੀ
UTS46::checkLabelContextO(ਅਚਲ char16_t *label, int32_t label_ਲੰਮਾਈ, IDNAInfo &info) ਅਚਲ {
    int32_t label_ਸਿਰਾ=label_ਲੰਮਾਈ-1;  // inclusive
    int32_t arabicDigits=0;  // -1 for 066x, +1 for 06Fx
    ਲਈ(int32_t i=0; i<=label_ਸਿਰਾ; ++i) {
        UChar32 c=label[i];
        ਜੇ(c<0xb7) {
            // ASCII fastpath
        } ਨਹੀਂਤਾਂ ਜੇ(c<=0x6f9) {
            ਜੇ(c==0xb7) {
                // Appendix A.3. MIDDLE DOT (U+00B7)
                // Rule Set:
                //  False;
                //  If Before(cp) .eq.  U+006C And
                //     After(cp) .eq.  U+006C Then True;
                ਜੇ(!(0<i && label[i-1]==0x6c &&
                     i<label_ਸਿਰਾ && label[i+1]==0x6c)) {
                    info.labelErrors|=UIDNA_ERROR_CONTEXTO_PUNCTUATION;
                }
            } ਨਹੀਂਤਾਂ ਜੇ(c==0x375) {
                // Appendix A.4. GREEK LOWER NUMERAL SIGN (KERAIA) (U+0375)
                // Rule Set:
                //  False;
                //  If Script(After(cp)) .eq.  Greek Then True;
                UScript_ਕੋਡ script=USCRIPT_INVALID_ਕੋਡ;
                ਜੇ(i<label_ਸਿਰਾ) {
                    UError_ਕੋਡ error_ਕੋਡ=U_ZERO_ERROR;
                    int32_t j=i+1;
                    U16_ਅਗਲਾ(label, j, label_ਲੰਮਾਈ, c);
                    script=uscript_getScript(c, &error_ਕੋਡ);
                }
                ਜੇ(script!=USCRIPT_GREEK) {
                    info.labelErrors|=UIDNA_ERROR_CONTEXTO_PUNCTUATION;
                }
            } ਨਹੀਂਤਾਂ ਜੇ(c==0x5f3 || c==0x5f4) {
                // Appendix A.5. HEBREW PUNCTUATION GERESH (U+05F3)
                // Rule Set:
                //  False;
                //  If Script(Before(cp)) .eq.  Hebrew Then True;
                //
                // Appendix A.6. HEBREW PUNCTUATION GERSHAYIM (U+05F4)
                // Rule Set:
                //  False;
                //  If Script(Before(cp)) .eq.  Hebrew Then True;
                UScript_ਕੋਡ script=USCRIPT_INVALID_ਕੋਡ;
                ਜੇ(0<i) {
                    UError_ਕੋਡ error_ਕੋਡ=U_ZERO_ERROR;
                    int32_t j=i;
                    U16_PREV(label, 0, j, c);
                    script=uscript_getScript(c, &error_ਕੋਡ);
                }
                ਜੇ(script!=USCRIPT_HEBREW) {
                    info.labelErrors|=UIDNA_ERROR_CONTEXTO_PUNCTUATION;
                }
            } ਨਹੀਂਤਾਂ ਜੇ(0x660<=c /* && c<=0x6f9 */) {
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
                ਜੇ(c<=0x669) {
                    ਜੇ(arabicDigits>0) {
                        info.labelErrors|=UIDNA_ERROR_CONTEXTO_DIGITS;
                    }
                    arabicDigits=-1;
                } ਨਹੀਂਤਾਂ ਜੇ(0x6f0<=c) {
                    ਜੇ(arabicDigits<0) {
                        info.labelErrors|=UIDNA_ERROR_CONTEXTO_DIGITS;
                    }
                    arabicDigits=1;
                }
            }
        } ਨਹੀਂਤਾਂ ਜੇ(c==0x30fb) {
            // Appendix A.7. KATAKANA MIDDLE DOT (U+30FB)
            // Rule Set:
            //  False;
            //  For All Characters:
            //    If Script(cp) .in. {Hiragana, Katakana, Han} Then True;
            //  End For;
            UError_ਕੋਡ error_ਕੋਡ=U_ZERO_ERROR;
            ਲਈ(ਪੂਰਨ j=0;;) {
                ਜੇ(j>label_ਸਿਰਾ) {
                    info.labelErrors|=UIDNA_ERROR_CONTEXTO_PUNCTUATION;
                    ਰੋਕੋ;
                }
                U16_ਅਗਲਾ(label, j, label_ਲੰਮਾਈ, c);
                UScript_ਕੋਡ script=uscript_getScript(c, &error_ਕੋਡ);
                ਜੇ(script==USCRIPT_HIRAGANA || script==USCRIPT_KATAKANA || script==USCRIPT_HAN) {
                    ਰੋਕੋ;
                }
            }
        }
    }
}

U_NAMESPACE_ਸਿਰਾ

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

U_CAPI UIDNA * U_EXPORT2
uidna_openUTS46(uint32_t options, UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਵਾਪਸ reinterpret_cast<UIDNA *>(IDNA::createUTS46Instance(options, *p_Error_ਕੋਡ));
}

U_CAPI ਖਾਲੀ U_EXPORT2
uidna_close(UIDNA *idna) {
    ਮਿਟਾਓ reinterpret_cast<IDNA *>(idna);
}

ਸਥਿਰ UBool
check_ਦਲੀਲਾਂ(ਅਚਲ ਖਾਲੀ *label, int32_t length,
          ਖਾਲੀ *dest, int32_t capacity,
          UIDNAInfo *pInfo, UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ ਝੂਠ;
    }
    // sizeof(UIDNAInfo)=16 in the first API version.
    ਜੇ(pInfo==ਖਾਲੀ_ਸੂਚਕ || pInfo->size<16) {
        *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ ਝੂਠ;
    }
    ਜੇ( (label==ਖਾਲੀ_ਸੂਚਕ ? length!=0 : length<-1) ||
        (dest==ਖਾਲੀ_ਸੂਚਕ ? capacity!=0 : capacity<0) ||
        (dest==label && label!=ਖਾਲੀ_ਸੂਚਕ)
    ) {
        *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ ਝੂਠ;
    }
    // Set all *pInfo bytes to 0 except for the size field itself.
    uprv_memset(&pInfo->size+1, 0, pInfo->size-ਆਕਾਰ(pInfo->size));
    ਵਾਪਸ ਸੱਚ;
}

ਸਥਿਰ ਖਾਲੀ
idna_Info_ਮੰਜ਼ਿਲ_Struct(IDNAInfo &info, UIDNAInfo *pInfo) {
    pInfo->isTransitionalDifferent=info.isTransitionalDifferent();
    pInfo->errors=info.getErrors();
}

U_CAPI int32_t U_EXPORT2
uidna_label_ਮੰਜ਼ਿਲ_ASCII(ਅਚਲ UIDNA *idna,
                   ਅਚਲ char16_t *label, int32_t length,
                   char16_t *dest, int32_t capacity,
                   UIDNAInfo *pInfo, UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਜੇ(!check_ਦਲੀਲਾਂ(label, length, dest, capacity, pInfo, p_Error_ਕੋਡ)) {
        ਵਾਪਸ 0;
    }
    UnicodeString src(length < 0, label, length);
    UnicodeString destString(dest, 0, capacity);
    IDNAInfo info;
    reinterpret_cast<ਅਚਲ IDNA *>(idna)->label_ਮੰਜ਼ਿਲ_ASCII(src, destString, info, *p_Error_ਕੋਡ);
    idna_Info_ਮੰਜ਼ਿਲ_Struct(info, pInfo);
    ਵਾਪਸ destString.extract(dest, capacity, *p_Error_ਕੋਡ);
}

U_CAPI int32_t U_EXPORT2
uidna_label_ਮੰਜ਼ਿਲ_Unicode(ਅਚਲ UIDNA *idna,
                     ਅਚਲ char16_t *label, int32_t length,
                     char16_t *dest, int32_t capacity,
                     UIDNAInfo *pInfo, UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਜੇ(!check_ਦਲੀਲਾਂ(label, length, dest, capacity, pInfo, p_Error_ਕੋਡ)) {
        ਵਾਪਸ 0;
    }
    UnicodeString src(length < 0, label, length);
    UnicodeString destString(dest, 0, capacity);
    IDNAInfo info;
    reinterpret_cast<ਅਚਲ IDNA *>(idna)->label_ਮੰਜ਼ਿਲ_Unicode(src, destString, info, *p_Error_ਕੋਡ);
    idna_Info_ਮੰਜ਼ਿਲ_Struct(info, pInfo);
    ਵਾਪਸ destString.extract(dest, capacity, *p_Error_ਕੋਡ);
}

U_CAPI int32_t U_EXPORT2
uidna_name_ਮੰਜ਼ਿਲ_ASCII(ਅਚਲ UIDNA *idna,
                  ਅਚਲ char16_t *name, int32_t length,
                  char16_t *dest, int32_t capacity,
                  UIDNAInfo *pInfo, UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਜੇ(!check_ਦਲੀਲਾਂ(name, length, dest, capacity, pInfo, p_Error_ਕੋਡ)) {
        ਵਾਪਸ 0;
    }
    UnicodeString src(length < 0, name, length);
    UnicodeString destString(dest, 0, capacity);
    IDNAInfo info;
    reinterpret_cast<ਅਚਲ IDNA *>(idna)->name_ਮੰਜ਼ਿਲ_ASCII(src, destString, info, *p_Error_ਕੋਡ);
    idna_Info_ਮੰਜ਼ਿਲ_Struct(info, pInfo);
    ਵਾਪਸ destString.extract(dest, capacity, *p_Error_ਕੋਡ);
}

U_CAPI int32_t U_EXPORT2
uidna_name_ਮੰਜ਼ਿਲ_Unicode(ਅਚਲ UIDNA *idna,
                    ਅਚਲ char16_t *name, int32_t length,
                    char16_t *dest, int32_t capacity,
                    UIDNAInfo *pInfo, UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਜੇ(!check_ਦਲੀਲਾਂ(name, length, dest, capacity, pInfo, p_Error_ਕੋਡ)) {
        ਵਾਪਸ 0;
    }
    UnicodeString src(length < 0, name, length);
    UnicodeString destString(dest, 0, capacity);
    IDNAInfo info;
    reinterpret_cast<ਅਚਲ IDNA *>(idna)->name_ਮੰਜ਼ਿਲ_Unicode(src, destString, info, *p_Error_ਕੋਡ);
    idna_Info_ਮੰਜ਼ਿਲ_Struct(info, pInfo);
    ਵਾਪਸ destString.extract(dest, capacity, *p_Error_ਕੋਡ);
}

U_CAPI int32_t U_EXPORT2
uidna_label_ਮੰਜ਼ਿਲ_ASCII_UTF8(ਅਚਲ UIDNA *idna,
                        ਅਚਲ ਅੱਖਰ *label, int32_t length,
                        ਅੱਖਰ *dest, int32_t capacity,
                        UIDNAInfo *pInfo, UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਜੇ(!check_ਦਲੀਲਾਂ(label, length, dest, capacity, pInfo, p_Error_ਕੋਡ)) {
        ਵਾਪਸ 0;
    }
    StringPiece src(label, length<0 ? static_cast<int32_t>(uprv_strlen(label)) : length);
    ਵਾਪਸ ByteSinkUtil::ਰਾਹੀਂ_Byte_Sink_ਮੰਜ਼ਿਲ_Terminated_Chars(
        dest, capacity,
        [&](ByteSink& sink, UError_ਕੋਡ& ਸਥਿਤੀ) {
            IDNAInfo info;
            reinterpret_cast<ਅਚਲ IDNA *>(idna)->label_ਮੰਜ਼ਿਲ_ASCII_UTF8(src, sink, info, ਸਥਿਤੀ);
            idna_Info_ਮੰਜ਼ਿਲ_Struct(info, pInfo);
        },
        *p_Error_ਕੋਡ);
}

U_CAPI int32_t U_EXPORT2
uidna_label_ਮੰਜ਼ਿਲ_Unicode_UTF8(ਅਚਲ UIDNA *idna,
                         ਅਚਲ ਅੱਖਰ *label, int32_t length,
                         ਅੱਖਰ *dest, int32_t capacity,
                         UIDNAInfo *pInfo, UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਜੇ(!check_ਦਲੀਲਾਂ(label, length, dest, capacity, pInfo, p_Error_ਕੋਡ)) {
        ਵਾਪਸ 0;
    }
    StringPiece src(label, length<0 ? static_cast<int32_t>(uprv_strlen(label)) : length);
    ਵਾਪਸ ByteSinkUtil::ਰਾਹੀਂ_Byte_Sink_ਮੰਜ਼ਿਲ_Terminated_Chars(
        dest, capacity,
        [&](ByteSink& sink, UError_ਕੋਡ& ਸਥਿਤੀ) {
            IDNAInfo info;
            reinterpret_cast<ਅਚਲ IDNA *>(idna)->label_ਮੰਜ਼ਿਲ_Unicode_UTF8(src, sink, info, ਸਥਿਤੀ);
            idna_Info_ਮੰਜ਼ਿਲ_Struct(info, pInfo);
        },
        *p_Error_ਕੋਡ);
}

U_CAPI int32_t U_EXPORT2
uidna_name_ਮੰਜ਼ਿਲ_ASCII_UTF8(ਅਚਲ UIDNA *idna,
                       ਅਚਲ ਅੱਖਰ *name, int32_t length,
                       ਅੱਖਰ *dest, int32_t capacity,
                       UIDNAInfo *pInfo, UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਜੇ(!check_ਦਲੀਲਾਂ(name, length, dest, capacity, pInfo, p_Error_ਕੋਡ)) {
        ਵਾਪਸ 0;
    }
    StringPiece src(name, length<0 ? static_cast<int32_t>(uprv_strlen(name)) : length);
    ਵਾਪਸ ByteSinkUtil::ਰਾਹੀਂ_Byte_Sink_ਮੰਜ਼ਿਲ_Terminated_Chars(
        dest, capacity,
        [&](ByteSink& sink, UError_ਕੋਡ& ਸਥਿਤੀ) {
            IDNAInfo info;
            reinterpret_cast<ਅਚਲ IDNA *>(idna)->name_ਮੰਜ਼ਿਲ_ASCII_UTF8(src, sink, info, ਸਥਿਤੀ);
            idna_Info_ਮੰਜ਼ਿਲ_Struct(info, pInfo);
        },
        *p_Error_ਕੋਡ);
}

U_CAPI int32_t U_EXPORT2
uidna_name_ਮੰਜ਼ਿਲ_Unicode_UTF8(ਅਚਲ UIDNA *idna,
                        ਅਚਲ ਅੱਖਰ *name, int32_t length,
                        ਅੱਖਰ *dest, int32_t capacity,
                        UIDNAInfo *pInfo, UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਜੇ(!check_ਦਲੀਲਾਂ(name, length, dest, capacity, pInfo, p_Error_ਕੋਡ)) {
        ਵਾਪਸ 0;
    }
    StringPiece src(name, length<0 ? static_cast<int32_t>(uprv_strlen(name)) : length);
    ਵਾਪਸ ByteSinkUtil::ਰਾਹੀਂ_Byte_Sink_ਮੰਜ਼ਿਲ_Terminated_Chars(
        dest, capacity,
        [&](ByteSink& sink, UError_ਕੋਡ& ਸਥਿਤੀ) {
            IDNAInfo info;
            reinterpret_cast<ਅਚਲ IDNA *>(idna)->name_ਮੰਜ਼ਿਲ_Unicode_UTF8(src, sink, info, ਸਥਿਤੀ);
            idna_Info_ਮੰਜ਼ਿਲ_Struct(info, pInfo);
        },
        *p_Error_ਕੋਡ);
}

#ਜੇ_ਅੰਤ  // UCONFIG_NO_IDNA
