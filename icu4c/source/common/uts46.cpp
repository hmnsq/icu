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

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಆದರೆ !UCONFIG_NO_IDNA

#ಅನ್ತರ್ಭಾವ "unicode/bytestream.h"
#ಅನ್ತರ್ಭಾವ "unicode/idna.h"
#ಅನ್ತರ್ಭಾವ "unicode/normalizer2.h"
#ಅನ್ತರ್ಭಾವ "unicode/uscript.h"
#ಅನ್ತರ್ಭಾವ "unicode/ustring.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf16.h"
#ಅನ್ತರ್ಭಾವ "bytesinkutil.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"
#ಅನ್ತರ್ಭಾವ "punycode.h"
#ಅನ್ತರ್ಭಾವ "ubidi_props.h"

// Note about tests for UIDNA_ERROR_DOMAIN_NAME_TOO_LONG:
//
// The domain name length limit is 255 octets in an internal DNS representation
// where the last ("root") label is the empty label
// represented by length byte 0 alone.
// In a conventional string, this translates to 253 characters, or 254
// if there is a trailing dot for the root label.

U_NAMESPACE_BEGIN

// Severe errors which usually result in a U+FFFD replacement character in the result string.
ಸ್ಥಿರಾಂಕ uint32_t severeErrors=
    UIDNA_ERROR_LEADING_COMBINING_MARK|
    UIDNA_ERROR_DISALLOWED|
    UIDNA_ERROR_PUNYCODE|
    UIDNA_ERROR_LABEL_HAS_DOT|
    UIDNA_ERROR_INVALID_ACE_LABEL;

ಸ್ಥಿರ ಸಾಲೊಳಗೆ UBool
isASCIIString(ಸ್ಥಿರಾಂಕ UnicodeString &dest) {
    ಸ್ಥಿರಾಂಕ char16_t *s=dest.get_ಬಫರ್();
    ಸ್ಥಿರಾಂಕ char16_t *ಎಲ್ಲೆ=s+dest.length();
    ಆಗ(s<ಎಲ್ಲೆ) {
        ಆದರೆ(*s++>0x7f) {
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
        }
    }
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

ಸ್ಥಿರ UBool
isASCIIOkBiDi(ಸ್ಥಿರಾಂಕ char16_t *s, int32_t length);

ಸ್ಥಿರ UBool
isASCIIOkBiDi(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, int32_t length);

// IDNA class default implementations -------------------------------------- ***

IDNA::~IDNA() {}

ಶೂನ್ಯ
IDNA::label_ಗಮ್ಯ_ASCII_UTF8(StringPiece label, ByteSink &dest,
                        IDNAInfo &info, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ(U_SUCCESS(error_ಸಂಕೇತ)) {
        UnicodeString destString;
        label_ಗಮ್ಯ_ASCII(UnicodeString::fromUTF8(label), destString,
                     info, error_ಸಂಕೇತ).ಗಮ್ಯ_UTF8(dest);
    }
}

ಶೂನ್ಯ
IDNA::label_ಗಮ್ಯ_Unicode_UTF8(StringPiece label, ByteSink &dest,
                         IDNAInfo &info, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ(U_SUCCESS(error_ಸಂಕೇತ)) {
        UnicodeString destString;
        label_ಗಮ್ಯ_Unicode(UnicodeString::fromUTF8(label), destString,
                       info, error_ಸಂಕೇತ).ಗಮ್ಯ_UTF8(dest);
    }
}

ಶೂನ್ಯ
IDNA::name_ಗಮ್ಯ_ASCII_UTF8(StringPiece name, ByteSink &dest,
                       IDNAInfo &info, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ(U_SUCCESS(error_ಸಂಕೇತ)) {
        UnicodeString destString;
        name_ಗಮ್ಯ_ASCII(UnicodeString::fromUTF8(name), destString,
                    info, error_ಸಂಕೇತ).ಗಮ್ಯ_UTF8(dest);
    }
}

ಶೂನ್ಯ
IDNA::name_ಗಮ್ಯ_Unicode_UTF8(StringPiece name, ByteSink &dest,
                        IDNAInfo &info, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ(U_SUCCESS(error_ಸಂಕೇತ)) {
        UnicodeString destString;
        name_ಗಮ್ಯ_Unicode(UnicodeString::fromUTF8(name), destString,
                      info, error_ಸಂಕೇತ).ಗಮ್ಯ_UTF8(dest);
    }
}

// UTS46 class declaration ------------------------------------------------- ***

ವರ್ಗ UTS46 : ಸಾರ್ವಜನಿಕ IDNA {
ಸಾರ್ವಜನಿಕ:
    UTS46(uint32_t options, UError_ಸಂಕೇತ &error_ಸಂಕೇತ);
    ವಾಸ್ತವಿಕ ~UTS46();

    ವಾಸ್ತವಿಕ UnicodeString&
    label_ಗಮ್ಯ_ASCII(ಸ್ಥಿರಾಂಕ UnicodeString& label,
                 UnicodeString& dest U_LIFETIME_BOUND,
                 IDNAInfo& info,
                 UError_ಸಂಕೇತ& error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ override;

    ವಾಸ್ತವಿಕ UnicodeString&
    label_ಗಮ್ಯ_Unicode(ಸ್ಥಿರಾಂಕ UnicodeString& label,
                   UnicodeString& dest U_LIFETIME_BOUND,
                   IDNAInfo& info,
                   UError_ಸಂಕೇತ& error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ override;

    ವಾಸ್ತವಿಕ UnicodeString&
    name_ಗಮ್ಯ_ASCII(ಸ್ಥಿರಾಂಕ UnicodeString& name,
                UnicodeString& dest U_LIFETIME_BOUND,
                IDNAInfo& info,
                UError_ಸಂಕೇತ& error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ override;

    ವಾಸ್ತವಿಕ UnicodeString&
    name_ಗಮ್ಯ_Unicode(ಸ್ಥಿರಾಂಕ UnicodeString& name,
                  UnicodeString& dest U_LIFETIME_BOUND,
                  IDNAInfo& info,
                  UError_ಸಂಕೇತ& error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ override;

    ವಾಸ್ತವಿಕ ಶೂನ್ಯ
    label_ಗಮ್ಯ_ASCII_UTF8(StringPiece label, ByteSink &dest,
                      IDNAInfo &info, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ override;

    ವಾಸ್ತವಿಕ ಶೂನ್ಯ
    label_ಗಮ್ಯ_Unicode_UTF8(StringPiece label, ByteSink &dest,
                       IDNAInfo &info, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ override;

    ವಾಸ್ತವಿಕ ಶೂನ್ಯ
    name_ಗಮ್ಯ_ASCII_UTF8(StringPiece name, ByteSink &dest,
                     IDNAInfo &info, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ override;

    ವಾಸ್ತವಿಕ ಶೂನ್ಯ
    name_ಗಮ್ಯ_Unicode_UTF8(StringPiece name, ByteSink &dest,
                      IDNAInfo &info, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ override;

ಖಾಸಗಿ:
    UnicodeString&
    process(ಸ್ಥಿರಾಂಕ UnicodeString& src,
            UBool isLabel,
            UBool ಗಮ್ಯ_ASCII,
            UnicodeString& dest U_LIFETIME_BOUND,
            IDNAInfo& info,
            UError_ಸಂಕೇತ& error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ;

    ಶೂನ್ಯ
    processUTF8(StringPiece src,
                UBool isLabel, UBool ಗಮ್ಯ_ASCII,
                ByteSink &dest,
                IDNAInfo &info, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ;

    UnicodeString&
    processUnicode(ಸ್ಥಿರಾಂಕ UnicodeString& src,
                   int32_t label_ಆರಂಭ,
                   int32_t mapping_ಆರಂಭ,
                   UBool isLabel,
                   UBool ಗಮ್ಯ_ASCII,
                   UnicodeString& dest U_LIFETIME_BOUND,
                   IDNAInfo& info,
                   UError_ಸಂಕೇತ& error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ;

    // returns the new dest.length()
    int32_t
    mapDevChars(UnicodeString &dest, int32_t label_ಆರಂಭ, int32_t mapping_ಆರಂಭ,
                UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ;

    // returns the new label length
    int32_t
    processLabel(UnicodeString &dest,
                 int32_t label_ಆರಂಭ, int32_t label_ಉದ್ದಳತೆ,
                 UBool ಗಮ್ಯ_ASCII,
                 IDNAInfo &info, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ;
    int32_t
    markBadACELabel(UnicodeString &dest,
                    int32_t label_ಆರಂಭ, int32_t label_ಉದ್ದಳತೆ,
                    UBool ಗಮ್ಯ_ASCII, IDNAInfo &info, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ;

    ಶೂನ್ಯ
    checkLabelBiDi(ಸ್ಥಿರಾಂಕ char16_t *label, int32_t label_ಉದ್ದಳತೆ, IDNAInfo &info) ಸ್ಥಿರಾಂಕ;

    UBool
    isLabelOkContextJ(ಸ್ಥಿರಾಂಕ char16_t *label, int32_t label_ಉದ್ದಳತೆ) ಸ್ಥಿರಾಂಕ;

    ಶೂನ್ಯ
    checkLabelContextO(ಸ್ಥಿರಾಂಕ char16_t *label, int32_t label_ಉದ್ದಳತೆ, IDNAInfo &info) ಸ್ಥಿರಾಂಕ;

    ಸ್ಥಿರಾಂಕ Normalizer2 &uts46Norm2;  // uts46.nrm
    uint32_t options;
};

IDNA *
IDNA::createUTS46Instance(uint32_t options, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಆದರೆ(U_SUCCESS(error_ಸಂಕೇತ)) {
        IDNA *idna=ಹೊಸ UTS46(options, error_ಸಂಕೇತ);
        ಆದರೆ(idna==ಶೂನ್ಯಸೂಚಕ) {
            error_ಸಂಕೇತ=U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
            ಅಳಿಸು idna;
            idna=ಶೂನ್ಯಸೂಚಕ;
        }
        ಹಿಂತಿರುಗಿ idna;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
}

// UTS46 implementation ---------------------------------------------------- ***

UTS46::UTS46(uint32_t opt, UError_ಸಂಕೇತ &error_ಸಂಕೇತ)
        : uts46Norm2(*Normalizer2::getInstance(ಶೂನ್ಯಸೂಚಕ, "uts46", UNORM2_COMPOSE, error_ಸಂಕೇತ)),
          options(opt) {}

UTS46::~UTS46() {}

UnicodeString&
UTS46::label_ಗಮ್ಯ_ASCII(ಸ್ಥಿರಾಂಕ UnicodeString& label,
                    UnicodeString& dest U_LIFETIME_BOUND,
                    IDNAInfo& info,
                    UError_ಸಂಕೇತ& error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ process(label, ನಿಜ, ನಿಜ, dest, info, error_ಸಂಕೇತ);
}

UnicodeString&
UTS46::label_ಗಮ್ಯ_Unicode(ಸ್ಥಿರಾಂಕ UnicodeString& label,
                      UnicodeString& dest U_LIFETIME_BOUND,
                      IDNAInfo& info,
                      UError_ಸಂಕೇತ& error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ process(label, ನಿಜ, ಸುಳ್ಳು, dest, info, error_ಸಂಕೇತ);
}

UnicodeString&
UTS46::name_ಗಮ್ಯ_ASCII(ಸ್ಥಿರಾಂಕ UnicodeString& name,
                   UnicodeString& dest U_LIFETIME_BOUND,
                   IDNAInfo& info,
                   UError_ಸಂಕೇತ& error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ {
    process(name, ಸುಳ್ಳು, ನಿಜ, dest, info, error_ಸಂಕೇತ);
    ಆದರೆ( dest.length()>=254 && (info.errors&UIDNA_ERROR_DOMAIN_NAME_TOO_LONG)==0 &&
        isASCIIString(dest) &&
        (dest.length()>254 || dest[253]!=0x2e)
    ) {
        info.errors|=UIDNA_ERROR_DOMAIN_NAME_TOO_LONG;
    }
    ಹಿಂತಿರುಗಿ dest;
}

UnicodeString&
UTS46::name_ಗಮ್ಯ_Unicode(ಸ್ಥಿರಾಂಕ UnicodeString& name,
                     UnicodeString& dest U_LIFETIME_BOUND,
                     IDNAInfo& info,
                     UError_ಸಂಕೇತ& error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ process(name, ಸುಳ್ಳು, ಸುಳ್ಳು, dest, info, error_ಸಂಕೇತ);
}

ಶೂನ್ಯ
UTS46::label_ಗಮ್ಯ_ASCII_UTF8(StringPiece label, ByteSink &dest,
                         IDNAInfo &info, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ {
    processUTF8(label, ನಿಜ, ನಿಜ, dest, info, error_ಸಂಕೇತ);
}

ಶೂನ್ಯ
UTS46::label_ಗಮ್ಯ_Unicode_UTF8(StringPiece label, ByteSink &dest,
                          IDNAInfo &info, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ {
    processUTF8(label, ನಿಜ, ಸುಳ್ಳು, dest, info, error_ಸಂಕೇತ);
}

ಶೂನ್ಯ
UTS46::name_ಗಮ್ಯ_ASCII_UTF8(StringPiece name, ByteSink &dest,
                        IDNAInfo &info, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ {
    processUTF8(name, ಸುಳ್ಳು, ನಿಜ, dest, info, error_ಸಂಕೇತ);
}

ಶೂನ್ಯ
UTS46::name_ಗಮ್ಯ_Unicode_UTF8(StringPiece name, ByteSink &dest,
                         IDNAInfo &info, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ {
    processUTF8(name, ಸುಳ್ಳು, ಸುಳ್ಳು, dest, info, error_ಸಂಕೇತ);
}

// UTS #46 data for ASCII characters.
// The normalizer (using uts46.nrm) maps uppercase ASCII letters to lowercase
// and passes through all other ASCII characters.
// If UIDNA_USE_STD3_RULES is set, then non-LDH characters are disallowed
// using this data.
// The ASCII fastpath also uses this data.
// Values: -1=disallowed  0==valid  1==mapped (lowercase)
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ int8_t ascii_ದತ್ತಾಂಶ[128]={
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
UTS46::process(ಸ್ಥಿರಾಂಕ UnicodeString& src,
               UBool isLabel,
               UBool ಗಮ್ಯ_ASCII,
               UnicodeString& dest U_LIFETIME_BOUND,
               IDNAInfo& info,
               UError_ಸಂಕೇತ& error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ {
    // uts46Norm2.normalize() would do all of this error checking and setup,
    // but with the ASCII fastpath we do not always call it, and do not
    // call it first.
    ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
        dest.ಗಣ_ಗಮ್ಯ_Bogus();
        ಹಿಂತಿರುಗಿ dest;
    }
    ಸ್ಥಿರಾಂಕ char16_t *srcArray=src.get_ಬಫರ್();
    ಆದರೆ(&dest==&src || srcArray==ಶೂನ್ಯಸೂಚಕ) {
        error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        dest.ಗಣ_ಗಮ್ಯ_Bogus();
        ಹಿಂತಿರುಗಿ dest;
    }
    // Arguments are fine, reset output values.
    dest.remove();
    info.reset();
    int32_t src_ಉದ್ದಳತೆ=src.length();
    ಆದರೆ(src_ಉದ್ದಳತೆ==0) {
        info.errors|=UIDNA_ERROR_ಖಾಲಿಗಳು_LABEL;
        ಹಿಂತಿರುಗಿ dest;
    }
    char16_t *destArray=dest.get_ಬಫರ್(src_ಉದ್ದಳತೆ);
    ಆದರೆ(destArray==ಶೂನ್ಯಸೂಚಕ) {
        error_ಸಂಕೇತ=U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ dest;
    }
    // ASCII fastpath
    UBool disallowNonLDHDot=(options&UIDNA_USE_STD3_ನಿಯಮಗಳು)!=0;
    int32_t label_ಆರಂಭ=0;
    int32_t i;
    ಗಾಗಿ(i=0;; ++i) {
        ಆದರೆ(i==src_ಉದ್ದಳತೆ) {
            ಆದರೆ(ಗಮ್ಯ_ASCII) {
                ಆದರೆ((i-label_ಆರಂಭ)>63) {
                    info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
                }
                // There is a trailing dot if labelStart==i.
                ಆದರೆ(!isLabel && i>=254 && (i>254 || label_ಆರಂಭ<i)) {
                    info.errors|=UIDNA_ERROR_DOMAIN_NAME_TOO_LONG;
                }
            }
            info.errors|=info.labelErrors;
            dest.release_ಬಫರ್(i);
            ಹಿಂತಿರುಗಿ dest;
        }
        char16_t c=srcArray[i];
        ಆದರೆ(c>0x7f) {
            ನಿಲ್ಲಿಸು;
        }
        ಪೂರ್ಣಾಂಕ c_ದತ್ತಾಂಶ=ascii_ದತ್ತಾಂಶ[c];
        ಆದರೆ(c_ದತ್ತಾಂಶ>0) {
            destArray[i]=c+0x20;  // Lowercase an uppercase ASCII letter.
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c_ದತ್ತಾಂಶ<0 && disallowNonLDHDot) {
            ನಿಲ್ಲಿಸು;  // Replacing with U+FFFD can be complicated for toASCII.
        } ಇಲ್ಲದಿದ್ದರೆ {
            destArray[i]=c;
            ಆದರೆ(c==0x2d) {  // hyphen
                ಆದರೆ(i==(label_ಆರಂಭ+3) && srcArray[i-1]==0x2d) {
                    // "??--..." is Punycode or forbidden.
                    ++i;  // '-' was copied to dest already
                    ನಿಲ್ಲಿಸು;
                }
                ಆದರೆ(i==label_ಆರಂಭ) {
                    // label starts with "-"
                    info.labelErrors|=UIDNA_ERROR_LEADING_HYPHEN;
                }
                ಆದರೆ((i+1)==src_ಉದ್ದಳತೆ || srcArray[i+1]==0x2e) {
                    // label ends with "-"
                    info.labelErrors|=UIDNA_ERROR_TRAILING_HYPHEN;
                }
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c==0x2e) {  // dot
                ಆದರೆ(isLabel) {
                    // Replacing with U+FFFD can be complicated for toASCII.
                    ++i;  // '.' was copied to dest already
                    ನಿಲ್ಲಿಸು;
                }
                ಆದರೆ(i==label_ಆರಂಭ) {
                    info.labelErrors|=UIDNA_ERROR_ಖಾಲಿಗಳು_LABEL;
                }
                ಆದರೆ(ಗಮ್ಯ_ASCII && (i-label_ಆರಂಭ)>63) {
                    info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
                }
                info.errors|=info.labelErrors;
                info.labelErrors=0;
                label_ಆರಂಭ=i+1;
            }
        }
    }
    info.errors|=info.labelErrors;
    dest.release_ಬಫರ್(i);
    processUnicode(src, label_ಆರಂಭ, i, isLabel, ಗಮ್ಯ_ASCII, dest, info, error_ಸಂಕೇತ);
    ಆದರೆ( info.isBiDi && U_SUCCESS(error_ಸಂಕೇತ) && (info.errors&severeErrors)==0 &&
        (!info.isOkBiDi || (label_ಆರಂಭ>0 && !isASCIIOkBiDi(dest.get_ಬಫರ್(), label_ಆರಂಭ)))
    ) {
        info.errors|=UIDNA_ERROR_BIDI;
    }
    ಹಿಂತಿರುಗಿ dest;
}

ಶೂನ್ಯ
UTS46::processUTF8(StringPiece src,
                   UBool isLabel, UBool ಗಮ್ಯ_ASCII,
                   ByteSink &dest,
                   IDNAInfo &info, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ;
    }
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *srcArray=src.ದತ್ತಾಂಶ();
    int32_t src_ಉದ್ದಳತೆ=src.length();
    ಆದರೆ(srcArray==ಶೂನ್ಯಸೂಚಕ && src_ಉದ್ದಳತೆ!=0) {
        error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }
    // Arguments are fine, reset output values.
    info.reset();
    ಆದರೆ(src_ಉದ್ದಳತೆ==0) {
        info.errors|=UIDNA_ERROR_ಖಾಲಿಗಳು_LABEL;
        dest.Flush();
        ಹಿಂತಿರುಗಿ;
    }
    UnicodeString destString;
    int32_t label_ಆರಂಭ=0;
    ಆದರೆ(src_ಉದ್ದಳತೆ<=256) {  // length of stackArray[]
        // ASCII fastpath
        ಅಕ್ಷರ ರಾಶಿ_Array[256];
        int32_t dest_ಸಾಮರ್ಥ್ಯ;
        ಅಕ್ಷರ *destArray=dest.Get_Append_ಬಫರ್(src_ಉದ್ದಳತೆ, src_ಉದ್ದಳತೆ+20,
                                             ರಾಶಿ_Array, UPRV_LENGTHOF(ರಾಶಿ_Array), &dest_ಸಾಮರ್ಥ್ಯ);
        UBool disallowNonLDHDot=(options&UIDNA_USE_STD3_ನಿಯಮಗಳು)!=0;
        int32_t i;
        ಗಾಗಿ(i=0;; ++i) {
            ಆದರೆ(i==src_ಉದ್ದಳತೆ) {
                ಆದರೆ(ಗಮ್ಯ_ASCII) {
                    ಆದರೆ((i-label_ಆರಂಭ)>63) {
                        info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
                    }
                    // There is a trailing dot if labelStart==i.
                    ಆದರೆ(!isLabel && i>=254 && (i>254 || label_ಆರಂಭ<i)) {
                        info.errors|=UIDNA_ERROR_DOMAIN_NAME_TOO_LONG;
                    }
                }
                info.errors|=info.labelErrors;
                dest.Append(destArray, i);
                dest.Flush();
                ಹಿಂತಿರುಗಿ;
            }
            ಅಕ್ಷರ c=srcArray[i];
            ಆದರೆ (static_cast<int8_t>(c) < 0) { // (uint8_t)c>0x7f
                ನಿಲ್ಲಿಸು;
            }
            ಪೂರ್ಣಾಂಕ c_ದತ್ತಾಂಶ = ascii_ದತ್ತಾಂಶ[static_cast<ಪೂರ್ಣಾಂಕ>(c)]; // Cast: gcc warns about indexing with a char.
            ಆದರೆ(c_ದತ್ತಾಂಶ>0) {
                destArray[i]=c+0x20;  // Lowercase an uppercase ASCII letter.
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c_ದತ್ತಾಂಶ<0 && disallowNonLDHDot) {
                ನಿಲ್ಲಿಸು;  // Replacing with U+FFFD can be complicated for toASCII.
            } ಇಲ್ಲದಿದ್ದರೆ {
                destArray[i]=c;
                ಆದರೆ(c==0x2d) {  // hyphen
                    ಆದರೆ(i==(label_ಆರಂಭ+3) && srcArray[i-1]==0x2d) {
                        // "??--..." is Punycode or forbidden.
                        ನಿಲ್ಲಿಸು;
                    }
                    ಆದರೆ(i==label_ಆರಂಭ) {
                        // label starts with "-"
                        info.labelErrors|=UIDNA_ERROR_LEADING_HYPHEN;
                    }
                    ಆದರೆ((i+1)==src_ಉದ್ದಳತೆ || srcArray[i+1]==0x2e) {
                        // label ends with "-"
                        info.labelErrors|=UIDNA_ERROR_TRAILING_HYPHEN;
                    }
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c==0x2e) {  // dot
                    ಆದರೆ(isLabel) {
                        ನಿಲ್ಲಿಸು;  // Replacing with U+FFFD can be complicated for toASCII.
                    }
                    ಆದರೆ(i==label_ಆರಂಭ) {
                        info.labelErrors|=UIDNA_ERROR_ಖಾಲಿಗಳು_LABEL;
                    }
                    ಆದರೆ(ಗಮ್ಯ_ASCII && (i-label_ಆರಂಭ)>63) {
                        info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
                    }
                    info.errors|=info.labelErrors;
                    info.labelErrors=0;
                    label_ಆರಂಭ=i+1;
                }
            }
        }
        info.errors|=info.labelErrors;
        // Convert the processed ASCII prefix of the current label to UTF-16.
        int32_t mapping_ಆರಂಭ=i-label_ಆರಂಭ;
        destString=UnicodeString::fromUTF8(StringPiece(destArray+label_ಆರಂಭ, mapping_ಆರಂಭ));
        // Output the previous ASCII labels and process the rest of src in UTF-16.
        dest.Append(destArray, label_ಆರಂಭ);
        processUnicode(UnicodeString::fromUTF8(StringPiece(src, label_ಆರಂಭ)), 0, mapping_ಆರಂಭ,
                       isLabel, ಗಮ್ಯ_ASCII,
                       destString, info, error_ಸಂಕೇತ);
    } ಇಲ್ಲದಿದ್ದರೆ {
        // src is too long for the ASCII fastpath implementation.
        processUnicode(UnicodeString::fromUTF8(src), 0, 0,
                       isLabel, ಗಮ್ಯ_ASCII,
                       destString, info, error_ಸಂಕೇತ);
    }
    destString.ಗಮ್ಯ_UTF8(dest);  // calls dest.Flush()
    ಆದರೆ(ಗಮ್ಯ_ASCII && !isLabel) {
        // length==labelStart==254 means that there is a trailing dot (ok) and
        // destString is empty (do not index at 253-labelStart).
        int32_t length=label_ಆರಂಭ+destString.length();
        ಆದರೆ( length>=254 && isASCIIString(destString) &&
            (length>254 ||
             (label_ಆರಂಭ<254 && destString[253-label_ಆರಂಭ]!=0x2e))
        ) {
            info.errors|=UIDNA_ERROR_DOMAIN_NAME_TOO_LONG;
        }
    }
    ಆದರೆ( info.isBiDi && U_SUCCESS(error_ಸಂಕೇತ) && (info.errors&severeErrors)==0 &&
        (!info.isOkBiDi || (label_ಆರಂಭ>0 && !isASCIIOkBiDi(srcArray, label_ಆರಂಭ)))
    ) {
        info.errors|=UIDNA_ERROR_BIDI;
    }
}

UnicodeString&
UTS46::processUnicode(ಸ್ಥಿರಾಂಕ UnicodeString& src,
                      int32_t label_ಆರಂಭ,
                      int32_t mapping_ಆರಂಭ,
                      UBool isLabel,
                      UBool ಗಮ್ಯ_ASCII,
                      UnicodeString& dest U_LIFETIME_BOUND,
                      IDNAInfo& info,
                      UError_ಸಂಕೇತ& error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ(mapping_ಆರಂಭ==0) {
        uts46Norm2.normalize(src, dest, error_ಸಂಕೇತ);
    } ಇಲ್ಲದಿದ್ದರೆ {
        uts46Norm2.normalize_ಎರಡನೇ_And_Append(dest, src.ತಾತ್ಕಾಲಿಕ_Sub_String(mapping_ಆರಂಭ), error_ಸಂಕೇತ);
    }
    ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ dest;
    }
    UBool doMapDevChars=
        ಗಮ್ಯ_ASCII ? (options&UIDNA_NONTRANSITIONAL_ಗಮ್ಯ_ASCII)==0 :
                  (options&UIDNA_NONTRANSITIONAL_ಗಮ್ಯ_UNICODE)==0;
    ಸ್ಥಿರಾಂಕ char16_t *destArray=dest.get_ಬಫರ್();
    int32_t dest_ಉದ್ದಳತೆ=dest.length();
    int32_t label_ಎಲ್ಲೆ=label_ಆರಂಭ;
    ಆಗ(label_ಎಲ್ಲೆ<dest_ಉದ್ದಳತೆ) {
        char16_t c=destArray[label_ಎಲ್ಲೆ];
        ಆದರೆ(c==0x2e && !isLabel) {
            int32_t label_ಉದ್ದಳತೆ=label_ಎಲ್ಲೆ-label_ಆರಂಭ;
            int32_t new_ಉದ್ದಳತೆ=processLabel(dest, label_ಆರಂಭ, label_ಉದ್ದಳತೆ,
                                            ಗಮ್ಯ_ASCII, info, error_ಸಂಕೇತ);
            info.errors|=info.labelErrors;
            info.labelErrors=0;
            ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
                ಹಿಂತಿರುಗಿ dest;
            }
            destArray=dest.get_ಬಫರ್();
            dest_ಉದ್ದಳತೆ+=new_ಉದ್ದಳತೆ-label_ಉದ್ದಳತೆ;
            label_ಎಲ್ಲೆ=label_ಆರಂಭ+=new_ಉದ್ದಳತೆ+1;
            ಮುಂದುವರಿ;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c<0xdf) {
            // pass
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c<=0x200d && (c==0xdf || c==0x3c2 || c>=0x200c)) {
            info.isTransDiff=ನಿಜ;
            ಆದರೆ(doMapDevChars) {
                dest_ಉದ್ದಳತೆ=mapDevChars(dest, label_ಆರಂಭ, label_ಎಲ್ಲೆ, error_ಸಂಕೇತ);
                ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
                    ಹಿಂತಿರುಗಿ dest;
                }
                destArray=dest.get_ಬಫರ್();
                // All deviation characters have been mapped, no need to check for them again.
                doMapDevChars=ಸುಳ್ಳು;
                // Do not increment labelLimit in case c was removed.
                ಮುಂದುವರಿ;
            }
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(U16_IS_SURROGATE(c)) {
            ಆದರೆ(U16_IS_SURROGATE_LEAD(c) ?
                    (label_ಎಲ್ಲೆ+1)==dest_ಉದ್ದಳತೆ || !U16_IS_TRAIL(destArray[label_ಎಲ್ಲೆ+1]) :
                    label_ಎಲ್ಲೆ==label_ಆರಂಭ || !U16_IS_LEAD(destArray[label_ಎಲ್ಲೆ-1])) {
                // Map an unpaired surrogate to U+FFFD before normalization so that when
                // that removes characters we do not turn two unpaired ones into a pair.
                info.labelErrors|=UIDNA_ERROR_DISALLOWED;
                dest.ಗಣ_Char_At(label_ಎಲ್ಲೆ, 0xfffd);
                destArray=dest.get_ಬಫರ್();
            }
        }
        ++label_ಎಲ್ಲೆ;
    }
    // Permit an empty label at the end (0<labelStart==labelLimit==destLength is ok)
    // but not an empty label elsewhere nor a completely empty domain name.
    // processLabel() sets UIDNA_ERROR_EMPTY_LABEL when labelLength==0.
    ಆದರೆ(0==label_ಆರಂಭ || label_ಆರಂಭ<label_ಎಲ್ಲೆ) {
        processLabel(dest, label_ಆರಂಭ, label_ಎಲ್ಲೆ-label_ಆರಂಭ,
                      ಗಮ್ಯ_ASCII, info, error_ಸಂಕೇತ);
        info.errors|=info.labelErrors;
    }
    ಹಿಂತಿರುಗಿ dest;
}

int32_t
UTS46::mapDevChars(UnicodeString &dest, int32_t label_ಆರಂಭ, int32_t mapping_ಆರಂಭ,
                   UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ 0;
    }
    int32_t length=dest.length();
    char16_t *s=dest.get_ಬಫರ್(dest[mapping_ಆರಂಭ]==0xdf ? length+1 : length);
    ಆದರೆ(s==ಶೂನ್ಯಸೂಚಕ) {
        error_ಸಂಕೇತ=U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ length;
    }
    int32_t capacity=dest.get_ಸಾಮರ್ಥ್ಯ();
    UBool didMapDevChars=ಸುಳ್ಳು;
    int32_t read_ಸೂಚ್ಯಂಕ=mapping_ಆರಂಭ, write_ಸೂಚ್ಯಂಕ=mapping_ಆರಂಭ;
    ಮಾಡು {
        char16_t c=s[read_ಸೂಚ್ಯಂಕ++];
        ಆಯ್ಕೆ(c) {
        ಸ್ಥಿತಿ 0xdf:
            // Map sharp s to ss.
            didMapDevChars=ನಿಜ;
            s[write_ಸೂಚ್ಯಂಕ++]=0x73;  // Replace sharp s with first s.
            // Insert second s and account for possible buffer reallocation.
            ಆದರೆ(write_ಸೂಚ್ಯಂಕ==read_ಸೂಚ್ಯಂಕ) {
                ಆದರೆ(length==capacity) {
                    dest.release_ಬಫರ್(length);
                    s=dest.get_ಬಫರ್(length+1);
                    ಆದರೆ(s==ಶೂನ್ಯಸೂಚಕ) {
                        error_ಸಂಕೇತ=U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
                        ಹಿಂತಿರುಗಿ length;
                    }
                    capacity=dest.get_ಸಾಮರ್ಥ್ಯ();
                }
                u_memmove(s+write_ಸೂಚ್ಯಂಕ+1, s+write_ಸೂಚ್ಯಂಕ, length-write_ಸೂಚ್ಯಂಕ);
                ++read_ಸೂಚ್ಯಂಕ;
            }
            s[write_ಸೂಚ್ಯಂಕ++]=0x73;
            ++length;
            ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ 0x3c2:  // Map final sigma to nonfinal sigma.
            didMapDevChars=ನಿಜ;
            s[write_ಸೂಚ್ಯಂಕ++]=0x3c3;
            ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ 0x200c:  // Ignore/remove ZWNJ.
        ಸ್ಥಿತಿ 0x200d:  // Ignore/remove ZWJ.
            didMapDevChars=ನಿಜ;
            --length;
            ನಿಲ್ಲಿಸು;
        ಪೂರ್ವನಿಯೋಜಿತ:
            // Only really necessary if writeIndex was different from readIndex.
            s[write_ಸೂಚ್ಯಂಕ++]=c;
            ನಿಲ್ಲಿಸು;
        }
    } ಆಗ(write_ಸೂಚ್ಯಂಕ<length);
    dest.release_ಬಫರ್(length);
    ಆದರೆ(didMapDevChars) {
        // Mapping deviation characters might have resulted in an un-NFC string.
        // We could use either the NFC or the UTS #46 normalizer.
        // By using the UTS #46 normalizer again, we avoid having to load a second .nrm data file.
        UnicodeString normalized;
        uts46Norm2.normalize(dest.ತಾತ್ಕಾಲಿಕ_Sub_String(label_ಆರಂಭ), normalized, error_ಸಂಕೇತ);
        ಆದರೆ(U_SUCCESS(error_ಸಂಕೇತ)) {
            dest.replace(label_ಆರಂಭ, 0x7fffffff, normalized);
            ಆದರೆ(dest.isBogus()) {
                error_ಸಂಕೇತ=U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
            }
            ಹಿಂತಿರುಗಿ dest.length();
        }
    }
    ಹಿಂತಿರುಗಿ length;
}

// Replace the label in dest with the label string, if the label was modified.
// If &label==&dest then the label was modified in-place and labelLength
// is the new label length, different from label.length().
// If &label!=&dest then labelLength==label.length().
// Returns labelLength (= the new label length).
ಸ್ಥಿರ int32_t
replaceLabel(UnicodeString &dest, int32_t dest_Label_ಆರಂಭ, int32_t dest_Label_ಉದ್ದಳತೆ,
             ಸ್ಥಿರಾಂಕ UnicodeString &label, int32_t label_ಉದ್ದಳತೆ, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ 0;
    }
    ಆದರೆ(&label!=&dest) {
        dest.replace(dest_Label_ಆರಂಭ, dest_Label_ಉದ್ದಳತೆ, label);
        ಆದರೆ(dest.isBogus()) {
            error_ಸಂಕೇತ=U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
            ಹಿಂತಿರುಗಿ 0;
        }
    }
    ಹಿಂತಿರುಗಿ label_ಉದ್ದಳತೆ;
}

int32_t
UTS46::processLabel(UnicodeString &dest,
                    int32_t label_ಆರಂಭ, int32_t label_ಉದ್ದಳತೆ,
                    UBool ಗಮ್ಯ_ASCII,
                    IDNAInfo &info, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ 0;
    }
    UnicodeString fromPunycode;
    UnicodeString *labelString;
    ಸ್ಥಿರಾಂಕ char16_t *label=dest.get_ಬಫರ್()+label_ಆರಂಭ;
    int32_t dest_Label_ಆರಂಭ=label_ಆರಂಭ;
    int32_t dest_Label_ಉದ್ದಳತೆ=label_ಉದ್ದಳತೆ;
    UBool wasPunycode;
    ಆದರೆ(label_ಉದ್ದಳತೆ>=4 && label[0]==0x78 && label[1]==0x6e && label[2]==0x2d && label[3]==0x2d) {
        // Label starts with "xn--", try to un-Punycode it.
        // In IDNA2008, labels like "xn--" (decodes to an empty string) and
        // "xn--ASCII-" (decodes to just "ASCII") fail the round-trip validation from
        // comparing the ToUnicode input with the back-to-ToASCII output.
        // They are alternate encodings of the respective ASCII labels.
        // Ignore "xn---" here: It will fail Punycode.decode() which logically comes before
        // the round-trip verification.
        ಆದರೆ(label_ಉದ್ದಳತೆ==4 || (label_ಉದ್ದಳತೆ>5 && label[label_ಉದ್ದಳತೆ-1]==u'-')) {
            info.labelErrors|=UIDNA_ERROR_INVALID_ACE_LABEL;
            ಹಿಂತಿರುಗಿ markBadACELabel(dest, label_ಆರಂಭ, label_ಉದ್ದಳತೆ, ಗಮ್ಯ_ASCII, info, error_ಸಂಕೇತ);
        }
        wasPunycode=ನಿಜ;
        char16_t *unicode_ಬಫರ್=fromPunycode.get_ಬಫರ್(-1);  // capacity==-1: most labels should fit
        ಆದರೆ(unicode_ಬಫರ್==ಶೂನ್ಯಸೂಚಕ) {
            // Should never occur if we used capacity==-1 which uses the internal buffer.
            error_ಸಂಕೇತ=U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
            ಹಿಂತಿರುಗಿ label_ಉದ್ದಳತೆ;
        }
        UError_ಸಂಕೇತ punycode_Error_ಸಂಕೇತ=U_ZERO_ERROR;
        int32_t unicode_ಉದ್ದಳತೆ=u_strFromPunycode(label+4, label_ಉದ್ದಳತೆ-4,
                                                unicode_ಬಫರ್, fromPunycode.get_ಸಾಮರ್ಥ್ಯ(),
                                                ಶೂನ್ಯಸೂಚಕ, &punycode_Error_ಸಂಕೇತ);
        ಆದರೆ(punycode_Error_ಸಂಕೇತ==U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR) {
            fromPunycode.release_ಬಫರ್(0);
            unicode_ಬಫರ್=fromPunycode.get_ಬಫರ್(unicode_ಉದ್ದಳತೆ);
            ಆದರೆ(unicode_ಬಫರ್==ಶೂನ್ಯಸೂಚಕ) {
                error_ಸಂಕೇತ=U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
                ಹಿಂತಿರುಗಿ label_ಉದ್ದಳತೆ;
            }
            punycode_Error_ಸಂಕೇತ=U_ZERO_ERROR;
            unicode_ಉದ್ದಳತೆ=u_strFromPunycode(label+4, label_ಉದ್ದಳತೆ-4,
                                            unicode_ಬಫರ್, fromPunycode.get_ಸಾಮರ್ಥ್ಯ(),
                                            ಶೂನ್ಯಸೂಚಕ, &punycode_Error_ಸಂಕೇತ);
        }
        fromPunycode.release_ಬಫರ್(unicode_ಉದ್ದಳತೆ);
        ಆದರೆ(U_FAILURE(punycode_Error_ಸಂಕೇತ)) {
            info.labelErrors|=UIDNA_ERROR_PUNYCODE;
            ಹಿಂತಿರುಗಿ markBadACELabel(dest, label_ಆರಂಭ, label_ಉದ್ದಳತೆ, ಗಮ್ಯ_ASCII, info, error_ಸಂಕೇತ);
        }
        // Check for NFC, and for characters that are not
        // valid or deviation characters according to the normalizer.
        // If there is something wrong, then the string will change.
        // Note that the normalizer passes through non-LDH ASCII and deviation characters.
        // Deviation characters are ok in Punycode even in transitional processing.
        // In the code further below, if we find non-LDH ASCII and we have UIDNA_USE_STD3_RULES
        // then we will set UIDNA_ERROR_INVALID_ACE_LABEL there too.
        UBool isValid=uts46Norm2.isNormalized(fromPunycode, error_ಸಂಕೇತ);
        ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
            ಹಿಂತಿರುಗಿ label_ಉದ್ದಳತೆ;
        }
        // Unicode 15.1 UTS #46:
        // Added an additional condition in 4.1 Validity Criteria to
        // disallow labels such as xn--xn---epa., which do not round-trip.
        // --> Validity Criteria new criterion 4:
        // If not CheckHyphens, the label must not begin with “xn--”.
        ಆದರೆ(!isValid || fromPunycode.startsWith(UnicodeString::readOnlyAlias(u"xn--"))) {
            info.labelErrors|=UIDNA_ERROR_INVALID_ACE_LABEL;
            ಹಿಂತಿರುಗಿ markBadACELabel(dest, label_ಆರಂಭ, label_ಉದ್ದಳತೆ, ಗಮ್ಯ_ASCII, info, error_ಸಂಕೇತ);
        }
        labelString=&fromPunycode;
        label=fromPunycode.get_ಬಫರ್();
        label_ಆರಂಭ=0;
        label_ಉದ್ದಳತೆ=fromPunycode.length();
    } ಇಲ್ಲದಿದ್ದರೆ {
        wasPunycode=ಸುಳ್ಳು;
        labelString=&dest;
    }
    // Validity check
    ಆದರೆ(label_ಉದ್ದಳತೆ==0) {
        info.labelErrors|=UIDNA_ERROR_ಖಾಲಿಗಳು_LABEL;
        ಹಿಂತಿರುಗಿ replaceLabel(dest, dest_Label_ಆರಂಭ, dest_Label_ಉದ್ದಳತೆ,
                            *labelString, label_ಉದ್ದಳತೆ, error_ಸಂಕೇತ);
    }
    // labelLength>0
    ಆದರೆ(label_ಉದ್ದಳತೆ>=4 && label[2]==0x2d && label[3]==0x2d) {
        // label starts with "??--"
        info.labelErrors|=UIDNA_ERROR_HYPHEN_3_4;
    }
    ಆದರೆ(label[0]==0x2d) {
        // label starts with "-"
        info.labelErrors|=UIDNA_ERROR_LEADING_HYPHEN;
    }
    ಆದರೆ(label[label_ಉದ್ದಳತೆ-1]==0x2d) {
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
    ಸ್ಥಿರಾಂಕ char16_t *ಎಲ್ಲೆ=label+label_ಉದ್ದಳತೆ;
    char16_t oredChars=0;
    // If we enforce STD3 rules, then ASCII characters other than LDH and dot are disallowed.
    UBool disallowNonLDHDot=(options&UIDNA_USE_STD3_ನಿಯಮಗಳು)!=0;
    ಮಾಡು {
        char16_t c=*s;
        ಆದರೆ(c<=0x7f) {
            ಆದರೆ(c==0x2e) {
                info.labelErrors|=UIDNA_ERROR_LABEL_HAS_DOT;
                *s=0xfffd;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(disallowNonLDHDot && ascii_ದತ್ತಾಂಶ[c]<0) {
                info.labelErrors|=UIDNA_ERROR_DISALLOWED;
                *s=0xfffd;
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            oredChars|=c;
            ಆದರೆ(c==0xfffd) {
                info.labelErrors|=UIDNA_ERROR_DISALLOWED;
            }
        }
        ++s;
    } ಆಗ(s<ಎಲ್ಲೆ);
    // Check for a leading combining mark after other validity checks
    // so that we don't report UIDNA_ERROR_DISALLOWED for the U+FFFD from here.
    UChar32 c;
    int32_t cp_ಉದ್ದಳತೆ=0;
    // "Unsafe" is ok because unpaired surrogates were mapped to U+FFFD.
    U16_ಮುಂದಿನ_UNSAFE(label, cp_ಉದ್ದಳತೆ, c);
    ಆದರೆ((U_GET_GC_ಮುಖವಾಡ(c)&U_GC_M_ಮುಖವಾಡ)!=0) {
        info.labelErrors|=UIDNA_ERROR_LEADING_COMBINING_MARK;
        labelString->replace(label_ಆರಂಭ, cp_ಉದ್ದಳತೆ, static_cast<char16_t>(0xfffd));
        label=labelString->get_ಬಫರ್()+label_ಆರಂಭ;
        label_ಉದ್ದಳತೆ+=1-cp_ಉದ್ದಳತೆ;
        ಆದರೆ(labelString==&dest) {
            dest_Label_ಉದ್ದಳತೆ=label_ಉದ್ದಳತೆ;
        }
    }
    ಆದರೆ((info.labelErrors&severeErrors)==0) {
        // Do contextual checks only if we do not have U+FFFD from a severe error
        // because U+FFFD can make these checks fail.
        ಆದರೆ((options&UIDNA_CHECK_BIDI)!=0 && (!info.isBiDi || info.isOkBiDi)) {
            checkLabelBiDi(label, label_ಉದ್ದಳತೆ, info);
        }
        ಆದರೆ( (options&UIDNA_CHECK_CONTEXTJ)!=0 && (oredChars&0x200c)==0x200c &&
            !isLabelOkContextJ(label, label_ಉದ್ದಳತೆ)
        ) {
            info.labelErrors|=UIDNA_ERROR_CONTEXTJ;
        }
        ಆದರೆ((options&UIDNA_CHECK_CONTEXTO)!=0 && oredChars>=0xb7) {
            checkLabelContextO(label, label_ಉದ್ದಳತೆ, info);
        }
        ಆದರೆ(ಗಮ್ಯ_ASCII) {
            ಆದರೆ(wasPunycode) {
                // Leave a Punycode label unchanged if it has no severe errors.
                ಆದರೆ(dest_Label_ಉದ್ದಳತೆ>63) {
                    info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
                }
                ಹಿಂತಿರುಗಿ dest_Label_ಉದ್ದಳತೆ;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(oredChars>=0x80) {
                // Contains non-ASCII characters.
                UnicodeString punycode;
                char16_t *ಬಫರ್=punycode.get_ಬಫರ್(63);  // 63==maximum DNS label length
                ಆದರೆ(ಬಫರ್==ಶೂನ್ಯಸೂಚಕ) {
                    error_ಸಂಕೇತ=U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
                    ಹಿಂತಿರುಗಿ dest_Label_ಉದ್ದಳತೆ;
                }
                ಬಫರ್[0]=0x78;  // Write "xn--".
                ಬಫರ್[1]=0x6e;
                ಬಫರ್[2]=0x2d;
                ಬಫರ್[3]=0x2d;
                UError_ಸಂಕೇತ punycode_Error_ಸಂಕೇತ=U_ZERO_ERROR;
                int32_t punycode_ಉದ್ದಳತೆ=u_str_ಗಮ್ಯ_Punycode(label, label_ಉದ್ದಳತೆ,
                                                      ಬಫರ್+4, punycode.get_ಸಾಮರ್ಥ್ಯ()-4,
                                                      ಶೂನ್ಯಸೂಚಕ, &punycode_Error_ಸಂಕೇತ);
                ಆದರೆ(punycode_Error_ಸಂಕೇತ==U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR) {
                    punycode_Error_ಸಂಕೇತ=U_ZERO_ERROR;
                    punycode.release_ಬಫರ್(4);
                    ಬಫರ್=punycode.get_ಬಫರ್(4+punycode_ಉದ್ದಳತೆ);
                    ಆದರೆ(ಬಫರ್==ಶೂನ್ಯಸೂಚಕ) {
                        error_ಸಂಕೇತ=U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
                        ಹಿಂತಿರುಗಿ dest_Label_ಉದ್ದಳತೆ;
                    }
                    punycode_ಉದ್ದಳತೆ=u_str_ಗಮ್ಯ_Punycode(label, label_ಉದ್ದಳತೆ,
                                                  ಬಫರ್+4, punycode.get_ಸಾಮರ್ಥ್ಯ()-4,
                                                  ಶೂನ್ಯಸೂಚಕ, &punycode_Error_ಸಂಕೇತ);
                }
                punycode_ಉದ್ದಳತೆ+=4;
                punycode.release_ಬಫರ್(punycode_ಉದ್ದಳತೆ);
                ಆದರೆ(U_FAILURE(punycode_Error_ಸಂಕೇತ)) {
                    error_ಸಂಕೇತ = punycode_Error_ಸಂಕೇತ;
                    ಹಿಂತಿರುಗಿ dest_Label_ಉದ್ದಳತೆ;
                }
                ಆದರೆ(punycode_ಉದ್ದಳತೆ>63) {
                    info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
                }
                ಹಿಂತಿರುಗಿ replaceLabel(dest, dest_Label_ಆರಂಭ, dest_Label_ಉದ್ದಳತೆ,
                                    punycode, punycode_ಉದ್ದಳತೆ, error_ಸಂಕೇತ);
            } ಇಲ್ಲದಿದ್ದರೆ {
                // all-ASCII label
                ಆದರೆ(label_ಉದ್ದಳತೆ>63) {
                    info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
                }
            }
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        // If a Punycode label has severe errors,
        // then leave it but make sure it does not look valid.
        ಆದರೆ(wasPunycode) {
            info.labelErrors|=UIDNA_ERROR_INVALID_ACE_LABEL;
            ಹಿಂತಿರುಗಿ markBadACELabel(dest, dest_Label_ಆರಂಭ, dest_Label_ಉದ್ದಳತೆ, ಗಮ್ಯ_ASCII, info, error_ಸಂಕೇತ);
        }
    }
    ಹಿಂತಿರುಗಿ replaceLabel(dest, dest_Label_ಆರಂಭ, dest_Label_ಉದ್ದಳತೆ,
                        *labelString, label_ಉದ್ದಳತೆ, error_ಸಂಕೇತ);
}

// Make sure an ACE label does not look valid.
// Append U+FFFD if the label has only LDH characters.
// If UIDNA_USE_STD3_RULES, also replace disallowed ASCII characters with U+FFFD.
int32_t
UTS46::markBadACELabel(UnicodeString &dest,
                       int32_t label_ಆರಂಭ, int32_t label_ಉದ್ದಳತೆ,
                       UBool ಗಮ್ಯ_ASCII, IDNAInfo &info, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ 0;
    }
    UBool disallowNonLDHDot=(options&UIDNA_USE_STD3_ನಿಯಮಗಳು)!=0;
    UBool isASCII=ನಿಜ;
    UBool onlyLDH=ನಿಜ;
    ಸ್ಥಿರಾಂಕ char16_t *label=dest.get_ಬಫರ್()+label_ಆರಂಭ;
    ಸ್ಥಿರಾಂಕ char16_t *ಎಲ್ಲೆ=label+label_ಉದ್ದಳತೆ;
    // Start after the initial "xn--".
    // Ok to cast away const because we own the UnicodeString.
    ಗಾಗಿ(char16_t *s=const_cast<char16_t *>(label+4); s<ಎಲ್ಲೆ; ++s) {
        char16_t c=*s;
        ಆದರೆ(c<=0x7f) {
            ಆದರೆ(c==0x2e) {
                info.labelErrors|=UIDNA_ERROR_LABEL_HAS_DOT;
                *s=0xfffd;
                isASCII=onlyLDH=ಸುಳ್ಳು;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ascii_ದತ್ತಾಂಶ[c]<0) {
                onlyLDH=ಸುಳ್ಳು;
                ಆದರೆ(disallowNonLDHDot) {
                    *s=0xfffd;
                    isASCII=ಸುಳ್ಳು;
                }
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            isASCII=onlyLDH=ಸುಳ್ಳು;
        }
    }
    ಆದರೆ(onlyLDH) {
        dest.insert(label_ಆರಂಭ + label_ಉದ್ದಳತೆ, static_cast<char16_t>(0xfffd));
        ಆದರೆ(dest.isBogus()) {
            error_ಸಂಕೇತ=U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
            ಹಿಂತಿರುಗಿ 0;
        }
        ++label_ಉದ್ದಳತೆ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಆದರೆ(ಗಮ್ಯ_ASCII && isASCII && label_ಉದ್ದಳತೆ>63) {
            info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
        }
    }
    ಹಿಂತಿರುಗಿ label_ಉದ್ದಳತೆ;
}

ಸ್ಥಿರಾಂಕ uint32_t L_ಮುಖವಾಡ=U_ಮುಖವಾಡ(U_ಎಡ_ಗಮ್ಯ_ಬಲ);
ಸ್ಥಿರಾಂಕ uint32_t R_AL_ಮುಖವಾಡ=U_ಮುಖವಾಡ(U_ಬಲ_ಗಮ್ಯ_ಎಡ)|U_ಮುಖವಾಡ(U_ಬಲ_ಗಮ್ಯ_ಎಡ_ARABIC);
ಸ್ಥಿರಾಂಕ uint32_t L_R_AL_ಮುಖವಾಡ=L_ಮುಖವಾಡ|R_AL_ಮುಖವಾಡ;

ಸ್ಥಿರಾಂಕ uint32_t R_AL_AN_ಮುಖವಾಡ=R_AL_ಮುಖವಾಡ|U_ಮುಖವಾಡ(U_ARABIC_ಸಂಖ್ಯೆ);

ಸ್ಥಿರಾಂಕ uint32_t EN_AN_ಮುಖವಾಡ=U_ಮುಖವಾಡ(U_EUROPEAN_ಸಂಖ್ಯೆ)|U_ಮುಖವಾಡ(U_ARABIC_ಸಂಖ್ಯೆ);
ಸ್ಥಿರಾಂಕ uint32_t R_AL_EN_AN_ಮುಖವಾಡ=R_AL_ಮುಖವಾಡ|EN_AN_ಮುಖವಾಡ;
ಸ್ಥಿರಾಂಕ uint32_t L_EN_ಮುಖವಾಡ=L_ಮುಖವಾಡ|U_ಮುಖವಾಡ(U_EUROPEAN_ಸಂಖ್ಯೆ);

ಸ್ಥಿರಾಂಕ uint32_t ES_CS_ET_ON_BN_NSM_ಮುಖವಾಡ=
    U_ಮುಖವಾಡ(U_EUROPEAN_ಸಂಖ್ಯೆ_SEPARATOR)|
    U_ಮುಖವಾಡ(U_COMMON_ಸಂಖ್ಯೆ_SEPARATOR)|
    U_ಮುಖವಾಡ(U_EUROPEAN_ಸಂಖ್ಯೆ_TERMINATOR)|
    U_ಮುಖವಾಡ(U_OTHER_NEUTRAL)|
    U_ಮುಖವಾಡ(U_BOUNDARY_NEUTRAL)|
    U_ಮುಖವಾಡ(U_DIR_NON_SPACING_MARK);
ಸ್ಥಿರಾಂಕ uint32_t L_EN_ES_CS_ET_ON_BN_NSM_ಮುಖವಾಡ=L_EN_ಮುಖವಾಡ|ES_CS_ET_ON_BN_NSM_ಮುಖವಾಡ;
ಸ್ಥಿರಾಂಕ uint32_t R_AL_AN_EN_ES_CS_ET_ON_BN_NSM_ಮುಖವಾಡ=R_AL_ಮುಖವಾಡ|EN_AN_ಮುಖವಾಡ|ES_CS_ET_ON_BN_NSM_ಮುಖವಾಡ;

// We scan the whole label and check both for whether it contains RTL characters
// and whether it passes the BiDi Rule.
// In a BiDi domain name, all labels must pass the BiDi Rule, but we might find
// that a domain name is a BiDi domain name (has an RTL label) only after
// processing several earlier labels.
ಶೂನ್ಯ
UTS46::checkLabelBiDi(ಸ್ಥಿರಾಂಕ char16_t *label, int32_t label_ಉದ್ದಳತೆ, IDNAInfo &info) ಸ್ಥಿರಾಂಕ {
    // IDNA2008 BiDi rule
    // Get the directionality of the first character.
    UChar32 c;
    int32_t i=0;
    U16_ಮುಂದಿನ_UNSAFE(label, i, c);
    uint32_t ಮೊದಲ_ಮುಖವಾಡ=U_ಮುಖವಾಡ(u_charDirection(c));
    // 1. The first character must be a character with BIDI property L, R
    // or AL.  If it has the R or AL property, it is an RTL label; if it
    // has the L property, it is an LTR label.
    ಆದರೆ((ಮೊದಲ_ಮುಖವಾಡ&~L_R_AL_ಮುಖವಾಡ)!=0) {
        info.isOkBiDi=ಸುಳ್ಳು;
    }
    // Get the directionality of the last non-NSM character.
    uint32_t ಕೊನೆಯ_ಮುಖವಾಡ;
    ಗಾಗಿ(;;) {
        ಆದರೆ(i>=label_ಉದ್ದಳತೆ) {
            ಕೊನೆಯ_ಮುಖವಾಡ=ಮೊದಲ_ಮುಖವಾಡ;
            ನಿಲ್ಲಿಸು;
        }
        U16_PREV_UNSAFE(label, label_ಉದ್ದಳತೆ, c);
        UCharDirection dir=u_charDirection(c);
        ಆದರೆ(dir!=U_DIR_NON_SPACING_MARK) {
            ಕೊನೆಯ_ಮುಖವಾಡ=U_ಮುಖವಾಡ(dir);
            ನಿಲ್ಲಿಸು;
        }
    }
    // 3. In an RTL label, the end of the label must be a character with
    // BIDI property R, AL, EN or AN, followed by zero or more
    // characters with BIDI property NSM.
    // 6. In an LTR label, the end of the label must be a character with
    // BIDI property L or EN, followed by zero or more characters with
    // BIDI property NSM.
    ಆದರೆ( (ಮೊದಲ_ಮುಖವಾಡ&L_ಮುಖವಾಡ)!=0 ?
            (ಕೊನೆಯ_ಮುಖವಾಡ&~L_EN_ಮುಖವಾಡ)!=0 :
            (ಕೊನೆಯ_ಮುಖವಾಡ&~R_AL_EN_AN_ಮುಖವಾಡ)!=0
    ) {
        info.isOkBiDi=ಸುಳ್ಳು;
    }
    // Add the directionalities of the intervening characters.
    uint32_t mask=ಮೊದಲ_ಮುಖವಾಡ|ಕೊನೆಯ_ಮುಖವಾಡ;
    ಆಗ(i<label_ಉದ್ದಳತೆ) {
        U16_ಮುಂದಿನ_UNSAFE(label, i, c);
        mask|=U_ಮುಖವಾಡ(u_charDirection(c));
    }
    ಆದರೆ(ಮೊದಲ_ಮುಖವಾಡ&L_ಮುಖವಾಡ) {
        // 5. In an LTR label, only characters with the BIDI properties L, EN,
        // ES, CS, ET, ON, BN and NSM are allowed.
        ಆದರೆ((mask&~L_EN_ES_CS_ET_ON_BN_NSM_ಮುಖವಾಡ)!=0) {
            info.isOkBiDi=ಸುಳ್ಳು;
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        // 2. In an RTL label, only characters with the BIDI properties R, AL,
        // AN, EN, ES, CS, ET, ON, BN and NSM are allowed.
        ಆದರೆ((mask&~R_AL_AN_EN_ES_CS_ET_ON_BN_NSM_ಮುಖವಾಡ)!=0) {
            info.isOkBiDi=ಸುಳ್ಳು;
        }
        // 4. In an RTL label, if an EN is present, no AN may be present, and
        // vice versa.
        ಆದರೆ((mask&EN_AN_ಮುಖವಾಡ)==EN_AN_ಮುಖವಾಡ) {
            info.isOkBiDi=ಸುಳ್ಳು;
        }
    }
    // An RTL label is a label that contains at least one character of type
    // R, AL or AN. [...]
    // A "BIDI domain name" is a domain name that contains at least one RTL
    // label. [...]
    // The following rule, consisting of six conditions, applies to labels
    // in BIDI domain names.
    ಆದರೆ((mask&R_AL_AN_ಮುಖವಾಡ)!=0) {
        info.isBiDi=ನಿಜ;
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
ಸ್ಥಿರ UBool
isASCIIOkBiDi(ಸ್ಥಿರಾಂಕ char16_t *s, int32_t length) {
    int32_t label_ಆರಂಭ=0;
    ಗಾಗಿ(int32_t i=0; i<length; ++i) {
        char16_t c=s[i];
        ಆದರೆ(c==0x2e) {  // dot
            ಆದರೆ(i>label_ಆರಂಭ) {
                c=s[i-1];
                ಆದರೆ(!(0x61<=c && c<=0x7a) && !(0x30<=c && c<=0x39)) {
                    // Last character in the label is not an L or EN.
                    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
                }
            }
            label_ಆರಂಭ=i+1;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(i==label_ಆರಂಭ) {
            ಆದರೆ(!(0x61<=c && c<=0x7a)) {
                // First character in the label is not an L.
                ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಆದರೆ(c<=0x20 && (c>=0x1c || (9<=c && c<=0xd))) {
                // Intermediate character in the label is a B, S or WS.
                ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
            }
        }
    }
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

// UTF-8 version, called for source ASCII prefix.
// Can contain uppercase A-Z.
// s[length-1] must be the trailing dot.
ಸ್ಥಿರ UBool
isASCIIOkBiDi(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, int32_t length) {
    int32_t label_ಆರಂಭ=0;
    ಗಾಗಿ(int32_t i=0; i<length; ++i) {
        ಅಕ್ಷರ c=s[i];
        ಆದರೆ(c==0x2e) {  // dot
            ಆದರೆ(i>label_ಆರಂಭ) {
                c=s[i-1];
                ಆದರೆ(!(0x61<=c && c<=0x7a) && !(0x41<=c && c<=0x5a) && !(0x30<=c && c<=0x39)) {
                    // Last character in the label is not an L or EN.
                    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
                }
            }
            label_ಆರಂಭ=i+1;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(i==label_ಆರಂಭ) {
            ಆದರೆ(!(0x61<=c && c<=0x7a) && !(0x41<=c && c<=0x5a)) {
                // First character in the label is not an L.
                ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಆದರೆ(c<=0x20 && (c>=0x1c || (9<=c && c<=0xd))) {
                // Intermediate character in the label is a B, S or WS.
                ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
            }
        }
    }
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

UBool
UTS46::isLabelOkContextJ(ಸ್ಥಿರಾಂಕ char16_t *label, int32_t label_ಉದ್ದಳತೆ) ಸ್ಥಿರಾಂಕ {
    // [IDNA2008-Tables]
    // 200C..200D  ; CONTEXTJ    # ZERO WIDTH NON-JOINER..ZERO WIDTH JOINER
    ಗಾಗಿ(int32_t i=0; i<label_ಉದ್ದಳತೆ; ++i) {
        ಆದರೆ(label[i]==0x200c) {
            // Appendix A.1. ZERO WIDTH NON-JOINER
            // Rule Set:
            //  False;
            //  If Canonical_Combining_Class(Before(cp)) .eq.  Virama Then True;
            //  If RegExpMatch((Joining_Type:{L,D})(Joining_Type:T)*\u200C
            //     (Joining_Type:T)*(Joining_Type:{R,D})) Then True;
            ಆದರೆ(i==0) {
                ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
            }
            UChar32 c;
            int32_t j=i;
            U16_PREV_UNSAFE(label, j, c);
            ಆದರೆ(uts46Norm2.getCombiningClass(c)==9) {
                ಮುಂದುವರಿ;
            }
            // check precontext (Joining_Type:{L,D})(Joining_Type:T)*
            ಗಾಗಿ(;;) {
                UJoiningType type=ubidi_getJoiningType(c);
                ಆದರೆ(type==U_JT_TRANSPARENT) {
                    ಆದರೆ(j==0) {
                        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
                    }
                    U16_PREV_UNSAFE(label, j, c);
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(type==U_JT_ಎಡ_JOINING || type==U_JT_DUAL_JOINING) {
                    ನಿಲ್ಲಿಸು;  // precontext fulfilled
                } ಇಲ್ಲದಿದ್ದರೆ {
                    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
                }
            }
            // check postcontext (Joining_Type:T)*(Joining_Type:{R,D})
            ಗಾಗಿ(j=i+1;;) {
                ಆದರೆ(j==label_ಉದ್ದಳತೆ) {
                    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
                }
                U16_ಮುಂದಿನ_UNSAFE(label, j, c);
                UJoiningType type=ubidi_getJoiningType(c);
                ಆದರೆ(type==U_JT_TRANSPARENT) {
                    // just skip this character
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(type==U_JT_ಬಲ_JOINING || type==U_JT_DUAL_JOINING) {
                    ನಿಲ್ಲಿಸು;  // postcontext fulfilled
                } ಇಲ್ಲದಿದ್ದರೆ {
                    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
                }
            }
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(label[i]==0x200d) {
            // Appendix A.2. ZERO WIDTH JOINER (U+200D)
            // Rule Set:
            //  False;
            //  If Canonical_Combining_Class(Before(cp)) .eq.  Virama Then True;
            ಆದರೆ(i==0) {
                ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
            }
            UChar32 c;
            int32_t j=i;
            U16_PREV_UNSAFE(label, j, c);
            ಆದರೆ(uts46Norm2.getCombiningClass(c)!=9) {
                ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
            }
        }
    }
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

ಶೂನ್ಯ
UTS46::checkLabelContextO(ಸ್ಥಿರಾಂಕ char16_t *label, int32_t label_ಉದ್ದಳತೆ, IDNAInfo &info) ಸ್ಥಿರಾಂಕ {
    int32_t label_ಕೊನೆ=label_ಉದ್ದಳತೆ-1;  // inclusive
    int32_t arabicDigits=0;  // -1 for 066x, +1 for 06Fx
    ಗಾಗಿ(int32_t i=0; i<=label_ಕೊನೆ; ++i) {
        UChar32 c=label[i];
        ಆದರೆ(c<0xb7) {
            // ASCII fastpath
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c<=0x6f9) {
            ಆದರೆ(c==0xb7) {
                // Appendix A.3. MIDDLE DOT (U+00B7)
                // Rule Set:
                //  False;
                //  If Before(cp) .eq.  U+006C And
                //     After(cp) .eq.  U+006C Then True;
                ಆದರೆ(!(0<i && label[i-1]==0x6c &&
                     i<label_ಕೊನೆ && label[i+1]==0x6c)) {
                    info.labelErrors|=UIDNA_ERROR_CONTEXTO_PUNCTUATION;
                }
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c==0x375) {
                // Appendix A.4. GREEK LOWER NUMERAL SIGN (KERAIA) (U+0375)
                // Rule Set:
                //  False;
                //  If Script(After(cp)) .eq.  Greek Then True;
                UScript_ಸಂಕೇತ script=USCRIPT_INVALID_ಸಂಕೇತ;
                ಆದರೆ(i<label_ಕೊನೆ) {
                    UError_ಸಂಕೇತ error_ಸಂಕೇತ=U_ZERO_ERROR;
                    int32_t j=i+1;
                    U16_ಮುಂದಿನ(label, j, label_ಉದ್ದಳತೆ, c);
                    script=uscript_getScript(c, &error_ಸಂಕೇತ);
                }
                ಆದರೆ(script!=USCRIPT_GREEK) {
                    info.labelErrors|=UIDNA_ERROR_CONTEXTO_PUNCTUATION;
                }
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c==0x5f3 || c==0x5f4) {
                // Appendix A.5. HEBREW PUNCTUATION GERESH (U+05F3)
                // Rule Set:
                //  False;
                //  If Script(Before(cp)) .eq.  Hebrew Then True;
                //
                // Appendix A.6. HEBREW PUNCTUATION GERSHAYIM (U+05F4)
                // Rule Set:
                //  False;
                //  If Script(Before(cp)) .eq.  Hebrew Then True;
                UScript_ಸಂಕೇತ script=USCRIPT_INVALID_ಸಂಕೇತ;
                ಆದರೆ(0<i) {
                    UError_ಸಂಕೇತ error_ಸಂಕೇತ=U_ZERO_ERROR;
                    int32_t j=i;
                    U16_PREV(label, 0, j, c);
                    script=uscript_getScript(c, &error_ಸಂಕೇತ);
                }
                ಆದರೆ(script!=USCRIPT_HEBREW) {
                    info.labelErrors|=UIDNA_ERROR_CONTEXTO_PUNCTUATION;
                }
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(0x660<=c /* && c<=0x6f9 */) {
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
                ಆದರೆ(c<=0x669) {
                    ಆದರೆ(arabicDigits>0) {
                        info.labelErrors|=UIDNA_ERROR_CONTEXTO_DIGITS;
                    }
                    arabicDigits=-1;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(0x6f0<=c) {
                    ಆದರೆ(arabicDigits<0) {
                        info.labelErrors|=UIDNA_ERROR_CONTEXTO_DIGITS;
                    }
                    arabicDigits=1;
                }
            }
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c==0x30fb) {
            // Appendix A.7. KATAKANA MIDDLE DOT (U+30FB)
            // Rule Set:
            //  False;
            //  For All Characters:
            //    If Script(cp) .in. {Hiragana, Katakana, Han} Then True;
            //  End For;
            UError_ಸಂಕೇತ error_ಸಂಕೇತ=U_ZERO_ERROR;
            ಗಾಗಿ(ಪೂರ್ಣಾಂಕ j=0;;) {
                ಆದರೆ(j>label_ಕೊನೆ) {
                    info.labelErrors|=UIDNA_ERROR_CONTEXTO_PUNCTUATION;
                    ನಿಲ್ಲಿಸು;
                }
                U16_ಮುಂದಿನ(label, j, label_ಉದ್ದಳತೆ, c);
                UScript_ಸಂಕೇತ script=uscript_getScript(c, &error_ಸಂಕೇತ);
                ಆದರೆ(script==USCRIPT_HIRAGANA || script==USCRIPT_KATAKANA || script==USCRIPT_HAN) {
                    ನಿಲ್ಲಿಸು;
                }
            }
        }
    }
}

U_NAMESPACE_ಕೊನೆ

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

U_CAPI UIDNA * U_EXPORT2
uidna_openUTS46(uint32_t options, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಹಿಂತಿರುಗಿ reinterpret_cast<UIDNA *>(IDNA::createUTS46Instance(options, *p_Error_ಸಂಕೇತ));
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
uidna_close(UIDNA *idna) {
    ಅಳಿಸು reinterpret_cast<IDNA *>(idna);
}

ಸ್ಥಿರ UBool
check_ವಾದಗಳು(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *label, int32_t length,
          ಶೂನ್ಯ *dest, int32_t capacity,
          UIDNAInfo *pInfo, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    // sizeof(UIDNAInfo)=16 in the first API version.
    ಆದರೆ(pInfo==ಶೂನ್ಯಸೂಚಕ || pInfo->size<16) {
        *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    ಆದರೆ( (label==ಶೂನ್ಯಸೂಚಕ ? length!=0 : length<-1) ||
        (dest==ಶೂನ್ಯಸೂಚಕ ? capacity!=0 : capacity<0) ||
        (dest==label && label!=ಶೂನ್ಯಸೂಚಕ)
    ) {
        *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    // Set all *pInfo bytes to 0 except for the size field itself.
    uprv_memset(&pInfo->size+1, 0, pInfo->size-ಗಾತ್ರ(pInfo->size));
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

ಸ್ಥಿರ ಶೂನ್ಯ
idna_Info_ಗಮ್ಯ_Struct(IDNAInfo &info, UIDNAInfo *pInfo) {
    pInfo->isTransitionalDifferent=info.isTransitionalDifferent();
    pInfo->errors=info.getErrors();
}

U_CAPI int32_t U_EXPORT2
uidna_label_ಗಮ್ಯ_ASCII(ಸ್ಥಿರಾಂಕ UIDNA *idna,
                   ಸ್ಥಿರಾಂಕ char16_t *label, int32_t length,
                   char16_t *dest, int32_t capacity,
                   UIDNAInfo *pInfo, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಆದರೆ(!check_ವಾದಗಳು(label, length, dest, capacity, pInfo, p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ 0;
    }
    UnicodeString src(length < 0, label, length);
    UnicodeString destString(dest, 0, capacity);
    IDNAInfo info;
    reinterpret_cast<ಸ್ಥಿರಾಂಕ IDNA *>(idna)->label_ಗಮ್ಯ_ASCII(src, destString, info, *p_Error_ಸಂಕೇತ);
    idna_Info_ಗಮ್ಯ_Struct(info, pInfo);
    ಹಿಂತಿರುಗಿ destString.extract(dest, capacity, *p_Error_ಸಂಕೇತ);
}

U_CAPI int32_t U_EXPORT2
uidna_label_ಗಮ್ಯ_Unicode(ಸ್ಥಿರಾಂಕ UIDNA *idna,
                     ಸ್ಥಿರಾಂಕ char16_t *label, int32_t length,
                     char16_t *dest, int32_t capacity,
                     UIDNAInfo *pInfo, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಆದರೆ(!check_ವಾದಗಳು(label, length, dest, capacity, pInfo, p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ 0;
    }
    UnicodeString src(length < 0, label, length);
    UnicodeString destString(dest, 0, capacity);
    IDNAInfo info;
    reinterpret_cast<ಸ್ಥಿರಾಂಕ IDNA *>(idna)->label_ಗಮ್ಯ_Unicode(src, destString, info, *p_Error_ಸಂಕೇತ);
    idna_Info_ಗಮ್ಯ_Struct(info, pInfo);
    ಹಿಂತಿರುಗಿ destString.extract(dest, capacity, *p_Error_ಸಂಕೇತ);
}

U_CAPI int32_t U_EXPORT2
uidna_name_ಗಮ್ಯ_ASCII(ಸ್ಥಿರಾಂಕ UIDNA *idna,
                  ಸ್ಥಿರಾಂಕ char16_t *name, int32_t length,
                  char16_t *dest, int32_t capacity,
                  UIDNAInfo *pInfo, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಆದರೆ(!check_ವಾದಗಳು(name, length, dest, capacity, pInfo, p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ 0;
    }
    UnicodeString src(length < 0, name, length);
    UnicodeString destString(dest, 0, capacity);
    IDNAInfo info;
    reinterpret_cast<ಸ್ಥಿರಾಂಕ IDNA *>(idna)->name_ಗಮ್ಯ_ASCII(src, destString, info, *p_Error_ಸಂಕೇತ);
    idna_Info_ಗಮ್ಯ_Struct(info, pInfo);
    ಹಿಂತಿರುಗಿ destString.extract(dest, capacity, *p_Error_ಸಂಕೇತ);
}

U_CAPI int32_t U_EXPORT2
uidna_name_ಗಮ್ಯ_Unicode(ಸ್ಥಿರಾಂಕ UIDNA *idna,
                    ಸ್ಥಿರಾಂಕ char16_t *name, int32_t length,
                    char16_t *dest, int32_t capacity,
                    UIDNAInfo *pInfo, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಆದರೆ(!check_ವಾದಗಳು(name, length, dest, capacity, pInfo, p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ 0;
    }
    UnicodeString src(length < 0, name, length);
    UnicodeString destString(dest, 0, capacity);
    IDNAInfo info;
    reinterpret_cast<ಸ್ಥಿರಾಂಕ IDNA *>(idna)->name_ಗಮ್ಯ_Unicode(src, destString, info, *p_Error_ಸಂಕೇತ);
    idna_Info_ಗಮ್ಯ_Struct(info, pInfo);
    ಹಿಂತಿರುಗಿ destString.extract(dest, capacity, *p_Error_ಸಂಕೇತ);
}

U_CAPI int32_t U_EXPORT2
uidna_label_ಗಮ್ಯ_ASCII_UTF8(ಸ್ಥಿರಾಂಕ UIDNA *idna,
                        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *label, int32_t length,
                        ಅಕ್ಷರ *dest, int32_t capacity,
                        UIDNAInfo *pInfo, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಆದರೆ(!check_ವಾದಗಳು(label, length, dest, capacity, pInfo, p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ 0;
    }
    StringPiece src(label, length<0 ? static_cast<int32_t>(uprv_strlen(label)) : length);
    ಹಿಂತಿರುಗಿ ByteSinkUtil::ಮೂಲಕ_Byte_Sink_ಗಮ್ಯ_Terminated_Chars(
        dest, capacity,
        [&](ByteSink& sink, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) {
            IDNAInfo info;
            reinterpret_cast<ಸ್ಥಿರಾಂಕ IDNA *>(idna)->label_ಗಮ್ಯ_ASCII_UTF8(src, sink, info, ಸ್ಥಿತಿ_2);
            idna_Info_ಗಮ್ಯ_Struct(info, pInfo);
        },
        *p_Error_ಸಂಕೇತ);
}

U_CAPI int32_t U_EXPORT2
uidna_label_ಗಮ್ಯ_Unicode_UTF8(ಸ್ಥಿರಾಂಕ UIDNA *idna,
                         ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *label, int32_t length,
                         ಅಕ್ಷರ *dest, int32_t capacity,
                         UIDNAInfo *pInfo, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಆದರೆ(!check_ವಾದಗಳು(label, length, dest, capacity, pInfo, p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ 0;
    }
    StringPiece src(label, length<0 ? static_cast<int32_t>(uprv_strlen(label)) : length);
    ಹಿಂತಿರುಗಿ ByteSinkUtil::ಮೂಲಕ_Byte_Sink_ಗಮ್ಯ_Terminated_Chars(
        dest, capacity,
        [&](ByteSink& sink, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) {
            IDNAInfo info;
            reinterpret_cast<ಸ್ಥಿರಾಂಕ IDNA *>(idna)->label_ಗಮ್ಯ_Unicode_UTF8(src, sink, info, ಸ್ಥಿತಿ_2);
            idna_Info_ಗಮ್ಯ_Struct(info, pInfo);
        },
        *p_Error_ಸಂಕೇತ);
}

U_CAPI int32_t U_EXPORT2
uidna_name_ಗಮ್ಯ_ASCII_UTF8(ಸ್ಥಿರಾಂಕ UIDNA *idna,
                       ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, int32_t length,
                       ಅಕ್ಷರ *dest, int32_t capacity,
                       UIDNAInfo *pInfo, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಆದರೆ(!check_ವಾದಗಳು(name, length, dest, capacity, pInfo, p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ 0;
    }
    StringPiece src(name, length<0 ? static_cast<int32_t>(uprv_strlen(name)) : length);
    ಹಿಂತಿರುಗಿ ByteSinkUtil::ಮೂಲಕ_Byte_Sink_ಗಮ್ಯ_Terminated_Chars(
        dest, capacity,
        [&](ByteSink& sink, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) {
            IDNAInfo info;
            reinterpret_cast<ಸ್ಥಿರಾಂಕ IDNA *>(idna)->name_ಗಮ್ಯ_ASCII_UTF8(src, sink, info, ಸ್ಥಿತಿ_2);
            idna_Info_ಗಮ್ಯ_Struct(info, pInfo);
        },
        *p_Error_ಸಂಕೇತ);
}

U_CAPI int32_t U_EXPORT2
uidna_name_ಗಮ್ಯ_Unicode_UTF8(ಸ್ಥಿರಾಂಕ UIDNA *idna,
                        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, int32_t length,
                        ಅಕ್ಷರ *dest, int32_t capacity,
                        UIDNAInfo *pInfo, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಆದರೆ(!check_ವಾದಗಳು(name, length, dest, capacity, pInfo, p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ 0;
    }
    StringPiece src(name, length<0 ? static_cast<int32_t>(uprv_strlen(name)) : length);
    ಹಿಂತಿರುಗಿ ByteSinkUtil::ಮೂಲಕ_Byte_Sink_ಗಮ್ಯ_Terminated_Chars(
        dest, capacity,
        [&](ByteSink& sink, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) {
            IDNAInfo info;
            reinterpret_cast<ಸ್ಥಿರಾಂಕ IDNA *>(idna)->name_ಗಮ್ಯ_Unicode_UTF8(src, sink, info, ಸ್ಥಿತಿ_2);
            idna_Info_ಗಮ್ಯ_Struct(info, pInfo);
        },
        *p_Error_ಸಂಕೇತ);
}

#ಆದರೆ_ಅಂತ್ಯ  // UCONFIG_NO_IDNA
