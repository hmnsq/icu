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

#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ !UCONFIG_NO_IDNA

#అన్తర్భావ "unicode/bytestream.h"
#అన్తర్భావ "unicode/idna.h"
#అన్తర్భావ "unicode/normalizer2.h"
#అన్తర్భావ "unicode/uscript.h"
#అన్తర్భావ "unicode/ustring.h"
#అన్తర్భావ "unicode/utf16.h"
#అన్తర్భావ "bytesinkutil.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "cstring.h"
#అన్తర్భావ "punycode.h"
#అన్తర్భావ "ubidi_props.h"

// Note about tests for UIDNA_ERROR_DOMAIN_NAME_TOO_LONG:
//
// The domain name length limit is 255 octets in an internal DNS representation
// where the last ("root") label is the empty label
// represented by length byte 0 alone.
// In a conventional string, this translates to 253 characters, or 254
// if there is a trailing dot for the root label.

U_NAMESPACE_BEGIN

// Severe errors which usually result in a U+FFFD replacement character in the result string.
స్థిరరాశి uint32_t severeErrors=
    UIDNA_ERROR_LEADING_COMBINING_MARK|
    UIDNA_ERROR_DISALLOWED|
    UIDNA_ERROR_PUNYCODE|
    UIDNA_ERROR_LABEL_HAS_DOT|
    UIDNA_ERROR_INVALID_ACE_LABEL;

స్థిర అంతఃవరుస UBool
isASCIIString(స్థిరరాశి UnicodeString &dest) {
    స్థిరరాశి char16_t *s=dest.get_బఫరు();
    స్థిరరాశి char16_t *హద్దు=s+dest.length();
    అయితే(s<హద్దు) {
        ఒకవేళ(*s++>0x7f) {
            తిరిగి అబద్ధం;
        }
    }
    తిరిగి నిజం;
}

స్థిర UBool
isASCIIOkBiDi(స్థిరరాశి char16_t *s, int32_t length);

స్థిర UBool
isASCIIOkBiDi(స్థిరరాశి అక్షరం *s, int32_t length);

// IDNA class default implementations -------------------------------------- ***

IDNA::~IDNA() {}

శూన్య
IDNA::label_గమ్యం_ASCII_UTF8(StringPiece label, ByteSink &dest,
                        IDNAInfo &info, UError_సంకేతం &error_సంకేతం) స్థిరరాశి {
    ఒకవేళ(U_SUCCESS(error_సంకేతం)) {
        UnicodeString destString;
        label_గమ్యం_ASCII(UnicodeString::fromUTF8(label), destString,
                     info, error_సంకేతం).గమ్యం_UTF8(dest);
    }
}

శూన్య
IDNA::label_గమ్యం_Unicode_UTF8(StringPiece label, ByteSink &dest,
                         IDNAInfo &info, UError_సంకేతం &error_సంకేతం) స్థిరరాశి {
    ఒకవేళ(U_SUCCESS(error_సంకేతం)) {
        UnicodeString destString;
        label_గమ్యం_Unicode(UnicodeString::fromUTF8(label), destString,
                       info, error_సంకేతం).గమ్యం_UTF8(dest);
    }
}

శూన్య
IDNA::name_గమ్యం_ASCII_UTF8(StringPiece name, ByteSink &dest,
                       IDNAInfo &info, UError_సంకేతం &error_సంకేతం) స్థిరరాశి {
    ఒకవేళ(U_SUCCESS(error_సంకేతం)) {
        UnicodeString destString;
        name_గమ్యం_ASCII(UnicodeString::fromUTF8(name), destString,
                    info, error_సంకేతం).గమ్యం_UTF8(dest);
    }
}

శూన్య
IDNA::name_గమ్యం_Unicode_UTF8(StringPiece name, ByteSink &dest,
                        IDNAInfo &info, UError_సంకేతం &error_సంకేతం) స్థిరరాశి {
    ఒకవేళ(U_SUCCESS(error_సంకేతం)) {
        UnicodeString destString;
        name_గమ్యం_Unicode(UnicodeString::fromUTF8(name), destString,
                      info, error_సంకేతం).గమ్యం_UTF8(dest);
    }
}

// UTS46 class declaration ------------------------------------------------- ***

తరగతి UTS46 : బహిరంగ IDNA {
బహిరంగ:
    UTS46(uint32_t options, UError_సంకేతం &error_సంకేతం);
    మాయా ~UTS46();

    మాయా UnicodeString&
    label_గమ్యం_ASCII(స్థిరరాశి UnicodeString& label,
                 UnicodeString& dest U_LIFETIME_BOUND,
                 IDNAInfo& info,
                 UError_సంకేతం& error_సంకేతం) స్థిరరాశి override;

    మాయా UnicodeString&
    label_గమ్యం_Unicode(స్థిరరాశి UnicodeString& label,
                   UnicodeString& dest U_LIFETIME_BOUND,
                   IDNAInfo& info,
                   UError_సంకేతం& error_సంకేతం) స్థిరరాశి override;

    మాయా UnicodeString&
    name_గమ్యం_ASCII(స్థిరరాశి UnicodeString& name,
                UnicodeString& dest U_LIFETIME_BOUND,
                IDNAInfo& info,
                UError_సంకేతం& error_సంకేతం) స్థిరరాశి override;

    మాయా UnicodeString&
    name_గమ్యం_Unicode(స్థిరరాశి UnicodeString& name,
                  UnicodeString& dest U_LIFETIME_BOUND,
                  IDNAInfo& info,
                  UError_సంకేతం& error_సంకేతం) స్థిరరాశి override;

    మాయా శూన్య
    label_గమ్యం_ASCII_UTF8(StringPiece label, ByteSink &dest,
                      IDNAInfo &info, UError_సంకేతం &error_సంకేతం) స్థిరరాశి override;

    మాయా శూన్య
    label_గమ్యం_Unicode_UTF8(StringPiece label, ByteSink &dest,
                       IDNAInfo &info, UError_సంకేతం &error_సంకేతం) స్థిరరాశి override;

    మాయా శూన్య
    name_గమ్యం_ASCII_UTF8(StringPiece name, ByteSink &dest,
                     IDNAInfo &info, UError_సంకేతం &error_సంకేతం) స్థిరరాశి override;

    మాయా శూన్య
    name_గమ్యం_Unicode_UTF8(StringPiece name, ByteSink &dest,
                      IDNAInfo &info, UError_సంకేతం &error_సంకేతం) స్థిరరాశి override;

అంతరంగ:
    UnicodeString&
    process(స్థిరరాశి UnicodeString& src,
            UBool isLabel,
            UBool గమ్యం_ASCII,
            UnicodeString& dest U_LIFETIME_BOUND,
            IDNAInfo& info,
            UError_సంకేతం& error_సంకేతం) స్థిరరాశి;

    శూన్య
    processUTF8(StringPiece src,
                UBool isLabel, UBool గమ్యం_ASCII,
                ByteSink &dest,
                IDNAInfo &info, UError_సంకేతం &error_సంకేతం) స్థిరరాశి;

    UnicodeString&
    processUnicode(స్థిరరాశి UnicodeString& src,
                   int32_t label_ప్రారంభం,
                   int32_t mapping_ప్రారంభం,
                   UBool isLabel,
                   UBool గమ్యం_ASCII,
                   UnicodeString& dest U_LIFETIME_BOUND,
                   IDNAInfo& info,
                   UError_సంకేతం& error_సంకేతం) స్థిరరాశి;

    // returns the new dest.length()
    int32_t
    mapDevChars(UnicodeString &dest, int32_t label_ప్రారంభం, int32_t mapping_ప్రారంభం,
                UError_సంకేతం &error_సంకేతం) స్థిరరాశి;

    // returns the new label length
    int32_t
    processLabel(UnicodeString &dest,
                 int32_t label_ప్రారంభం, int32_t label_నిడివి,
                 UBool గమ్యం_ASCII,
                 IDNAInfo &info, UError_సంకేతం &error_సంకేతం) స్థిరరాశి;
    int32_t
    markBadACELabel(UnicodeString &dest,
                    int32_t label_ప్రారంభం, int32_t label_నిడివి,
                    UBool గమ్యం_ASCII, IDNAInfo &info, UError_సంకేతం &error_సంకేతం) స్థిరరాశి;

    శూన్య
    checkLabelBiDi(స్థిరరాశి char16_t *label, int32_t label_నిడివి, IDNAInfo &info) స్థిరరాశి;

    UBool
    isLabelOkContextJ(స్థిరరాశి char16_t *label, int32_t label_నిడివి) స్థిరరాశి;

    శూన్య
    checkLabelContextO(స్థిరరాశి char16_t *label, int32_t label_నిడివి, IDNAInfo &info) స్థిరరాశి;

    స్థిరరాశి Normalizer2 &uts46Norm2;  // uts46.nrm
    uint32_t options;
};

IDNA *
IDNA::createUTS46Instance(uint32_t options, UError_సంకేతం &error_సంకేతం) {
    ఒకవేళ(U_SUCCESS(error_సంకేతం)) {
        IDNA *idna=కొత్త UTS46(options, error_సంకేతం);
        ఒకవేళ(idna==శూన్యసూచిక) {
            error_సంకేతం=U_జ్ఞాపకం_కేటాయింపు_ERROR;
        } లేకపోతే ఒకవేళ(U_FAILURE(error_సంకేతం)) {
            తొలగించు idna;
            idna=శూన్యసూచిక;
        }
        తిరిగి idna;
    } లేకపోతే {
        తిరిగి శూన్యసూచిక;
    }
}

// UTS46 implementation ---------------------------------------------------- ***

UTS46::UTS46(uint32_t opt, UError_సంకేతం &error_సంకేతం)
        : uts46Norm2(*Normalizer2::getInstance(శూన్యసూచిక, "uts46", UNORM2_COMPOSE, error_సంకేతం)),
          options(opt) {}

UTS46::~UTS46() {}

UnicodeString&
UTS46::label_గమ్యం_ASCII(స్థిరరాశి UnicodeString& label,
                    UnicodeString& dest U_LIFETIME_BOUND,
                    IDNAInfo& info,
                    UError_సంకేతం& error_సంకేతం) స్థిరరాశి {
    తిరిగి process(label, నిజం, నిజం, dest, info, error_సంకేతం);
}

UnicodeString&
UTS46::label_గమ్యం_Unicode(స్థిరరాశి UnicodeString& label,
                      UnicodeString& dest U_LIFETIME_BOUND,
                      IDNAInfo& info,
                      UError_సంకేతం& error_సంకేతం) స్థిరరాశి {
    తిరిగి process(label, నిజం, అబద్ధం, dest, info, error_సంకేతం);
}

UnicodeString&
UTS46::name_గమ్యం_ASCII(స్థిరరాశి UnicodeString& name,
                   UnicodeString& dest U_LIFETIME_BOUND,
                   IDNAInfo& info,
                   UError_సంకేతం& error_సంకేతం) స్థిరరాశి {
    process(name, అబద్ధం, నిజం, dest, info, error_సంకేతం);
    ఒకవేళ( dest.length()>=254 && (info.errors&UIDNA_ERROR_DOMAIN_NAME_TOO_LONG)==0 &&
        isASCIIString(dest) &&
        (dest.length()>254 || dest[253]!=0x2e)
    ) {
        info.errors|=UIDNA_ERROR_DOMAIN_NAME_TOO_LONG;
    }
    తిరిగి dest;
}

UnicodeString&
UTS46::name_గమ్యం_Unicode(స్థిరరాశి UnicodeString& name,
                     UnicodeString& dest U_LIFETIME_BOUND,
                     IDNAInfo& info,
                     UError_సంకేతం& error_సంకేతం) స్థిరరాశి {
    తిరిగి process(name, అబద్ధం, అబద్ధం, dest, info, error_సంకేతం);
}

శూన్య
UTS46::label_గమ్యం_ASCII_UTF8(StringPiece label, ByteSink &dest,
                         IDNAInfo &info, UError_సంకేతం &error_సంకేతం) స్థిరరాశి {
    processUTF8(label, నిజం, నిజం, dest, info, error_సంకేతం);
}

శూన్య
UTS46::label_గమ్యం_Unicode_UTF8(StringPiece label, ByteSink &dest,
                          IDNAInfo &info, UError_సంకేతం &error_సంకేతం) స్థిరరాశి {
    processUTF8(label, నిజం, అబద్ధం, dest, info, error_సంకేతం);
}

శూన్య
UTS46::name_గమ్యం_ASCII_UTF8(StringPiece name, ByteSink &dest,
                        IDNAInfo &info, UError_సంకేతం &error_సంకేతం) స్థిరరాశి {
    processUTF8(name, అబద్ధం, నిజం, dest, info, error_సంకేతం);
}

శూన్య
UTS46::name_గమ్యం_Unicode_UTF8(StringPiece name, ByteSink &dest,
                         IDNAInfo &info, UError_సంకేతం &error_సంకేతం) స్థిరరాశి {
    processUTF8(name, అబద్ధం, అబద్ధం, dest, info, error_సంకేతం);
}

// UTS #46 data for ASCII characters.
// The normalizer (using uts46.nrm) maps uppercase ASCII letters to lowercase
// and passes through all other ASCII characters.
// If UIDNA_USE_STD3_RULES is set, then non-LDH characters are disallowed
// using this data.
// The ASCII fastpath also uses this data.
// Values: -1=disallowed  0==valid  1==mapped (lowercase)
స్థిర స్థిరరాశి int8_t ascii_దత్తాంశం[128]={
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
UTS46::process(స్థిరరాశి UnicodeString& src,
               UBool isLabel,
               UBool గమ్యం_ASCII,
               UnicodeString& dest U_LIFETIME_BOUND,
               IDNAInfo& info,
               UError_సంకేతం& error_సంకేతం) స్థిరరాశి {
    // uts46Norm2.normalize() would do all of this error checking and setup,
    // but with the ASCII fastpath we do not always call it, and do not
    // call it first.
    ఒకవేళ(U_FAILURE(error_సంకేతం)) {
        dest.సమితి_గమ్యం_Bogus();
        తిరిగి dest;
    }
    స్థిరరాశి char16_t *srcArray=src.get_బఫరు();
    ఒకవేళ(&dest==&src || srcArray==శూన్యసూచిక) {
        error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        dest.సమితి_గమ్యం_Bogus();
        తిరిగి dest;
    }
    // Arguments are fine, reset output values.
    dest.remove();
    info.reset();
    int32_t src_నిడివి=src.length();
    ఒకవేళ(src_నిడివి==0) {
        info.errors|=UIDNA_ERROR_ఖాళీలు_LABEL;
        తిరిగి dest;
    }
    char16_t *destArray=dest.get_బఫరు(src_నిడివి);
    ఒకవేళ(destArray==శూన్యసూచిక) {
        error_సంకేతం=U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి dest;
    }
    // ASCII fastpath
    UBool disallowNonLDHDot=(options&UIDNA_USE_STD3_నియమాలు)!=0;
    int32_t label_ప్రారంభం=0;
    int32_t i;
    కొరకు(i=0;; ++i) {
        ఒకవేళ(i==src_నిడివి) {
            ఒకవేళ(గమ్యం_ASCII) {
                ఒకవేళ((i-label_ప్రారంభం)>63) {
                    info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
                }
                // There is a trailing dot if labelStart==i.
                ఒకవేళ(!isLabel && i>=254 && (i>254 || label_ప్రారంభం<i)) {
                    info.errors|=UIDNA_ERROR_DOMAIN_NAME_TOO_LONG;
                }
            }
            info.errors|=info.labelErrors;
            dest.release_బఫరు(i);
            తిరిగి dest;
        }
        char16_t c=srcArray[i];
        ఒకవేళ(c>0x7f) {
            ఆపు;
        }
        పూర్ణాంకం c_దత్తాంశం=ascii_దత్తాంశం[c];
        ఒకవేళ(c_దత్తాంశం>0) {
            destArray[i]=c+0x20;  // Lowercase an uppercase ASCII letter.
        } లేకపోతే ఒకవేళ(c_దత్తాంశం<0 && disallowNonLDHDot) {
            ఆపు;  // Replacing with U+FFFD can be complicated for toASCII.
        } లేకపోతే {
            destArray[i]=c;
            ఒకవేళ(c==0x2d) {  // hyphen
                ఒకవేళ(i==(label_ప్రారంభం+3) && srcArray[i-1]==0x2d) {
                    // "??--..." is Punycode or forbidden.
                    ++i;  // '-' was copied to dest already
                    ఆపు;
                }
                ఒకవేళ(i==label_ప్రారంభం) {
                    // label starts with "-"
                    info.labelErrors|=UIDNA_ERROR_LEADING_HYPHEN;
                }
                ఒకవేళ((i+1)==src_నిడివి || srcArray[i+1]==0x2e) {
                    // label ends with "-"
                    info.labelErrors|=UIDNA_ERROR_TRAILING_HYPHEN;
                }
            } లేకపోతే ఒకవేళ(c==0x2e) {  // dot
                ఒకవేళ(isLabel) {
                    // Replacing with U+FFFD can be complicated for toASCII.
                    ++i;  // '.' was copied to dest already
                    ఆపు;
                }
                ఒకవేళ(i==label_ప్రారంభం) {
                    info.labelErrors|=UIDNA_ERROR_ఖాళీలు_LABEL;
                }
                ఒకవేళ(గమ్యం_ASCII && (i-label_ప్రారంభం)>63) {
                    info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
                }
                info.errors|=info.labelErrors;
                info.labelErrors=0;
                label_ప్రారంభం=i+1;
            }
        }
    }
    info.errors|=info.labelErrors;
    dest.release_బఫరు(i);
    processUnicode(src, label_ప్రారంభం, i, isLabel, గమ్యం_ASCII, dest, info, error_సంకేతం);
    ఒకవేళ( info.isBiDi && U_SUCCESS(error_సంకేతం) && (info.errors&severeErrors)==0 &&
        (!info.isOkBiDi || (label_ప్రారంభం>0 && !isASCIIOkBiDi(dest.get_బఫరు(), label_ప్రారంభం)))
    ) {
        info.errors|=UIDNA_ERROR_BIDI;
    }
    తిరిగి dest;
}

శూన్య
UTS46::processUTF8(StringPiece src,
                   UBool isLabel, UBool గమ్యం_ASCII,
                   ByteSink &dest,
                   IDNAInfo &info, UError_సంకేతం &error_సంకేతం) స్థిరరాశి {
    ఒకవేళ(U_FAILURE(error_సంకేతం)) {
        తిరిగి;
    }
    స్థిరరాశి అక్షరం *srcArray=src.దత్తాంశం();
    int32_t src_నిడివి=src.length();
    ఒకవేళ(srcArray==శూన్యసూచిక && src_నిడివి!=0) {
        error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి;
    }
    // Arguments are fine, reset output values.
    info.reset();
    ఒకవేళ(src_నిడివి==0) {
        info.errors|=UIDNA_ERROR_ఖాళీలు_LABEL;
        dest.Flush();
        తిరిగి;
    }
    UnicodeString destString;
    int32_t label_ప్రారంభం=0;
    ఒకవేళ(src_నిడివి<=256) {  // length of stackArray[]
        // ASCII fastpath
        అక్షరం దొంతర_Array[256];
        int32_t dest_సామర్థ్యం;
        అక్షరం *destArray=dest.Get_Append_బఫరు(src_నిడివి, src_నిడివి+20,
                                             దొంతర_Array, UPRV_LENGTHOF(దొంతర_Array), &dest_సామర్థ్యం);
        UBool disallowNonLDHDot=(options&UIDNA_USE_STD3_నియమాలు)!=0;
        int32_t i;
        కొరకు(i=0;; ++i) {
            ఒకవేళ(i==src_నిడివి) {
                ఒకవేళ(గమ్యం_ASCII) {
                    ఒకవేళ((i-label_ప్రారంభం)>63) {
                        info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
                    }
                    // There is a trailing dot if labelStart==i.
                    ఒకవేళ(!isLabel && i>=254 && (i>254 || label_ప్రారంభం<i)) {
                        info.errors|=UIDNA_ERROR_DOMAIN_NAME_TOO_LONG;
                    }
                }
                info.errors|=info.labelErrors;
                dest.Append(destArray, i);
                dest.Flush();
                తిరిగి;
            }
            అక్షరం c=srcArray[i];
            ఒకవేళ (static_cast<int8_t>(c) < 0) { // (uint8_t)c>0x7f
                ఆపు;
            }
            పూర్ణాంకం c_దత్తాంశం = ascii_దత్తాంశం[static_cast<పూర్ణాంకం>(c)]; // Cast: gcc warns about indexing with a char.
            ఒకవేళ(c_దత్తాంశం>0) {
                destArray[i]=c+0x20;  // Lowercase an uppercase ASCII letter.
            } లేకపోతే ఒకవేళ(c_దత్తాంశం<0 && disallowNonLDHDot) {
                ఆపు;  // Replacing with U+FFFD can be complicated for toASCII.
            } లేకపోతే {
                destArray[i]=c;
                ఒకవేళ(c==0x2d) {  // hyphen
                    ఒకవేళ(i==(label_ప్రారంభం+3) && srcArray[i-1]==0x2d) {
                        // "??--..." is Punycode or forbidden.
                        ఆపు;
                    }
                    ఒకవేళ(i==label_ప్రారంభం) {
                        // label starts with "-"
                        info.labelErrors|=UIDNA_ERROR_LEADING_HYPHEN;
                    }
                    ఒకవేళ((i+1)==src_నిడివి || srcArray[i+1]==0x2e) {
                        // label ends with "-"
                        info.labelErrors|=UIDNA_ERROR_TRAILING_HYPHEN;
                    }
                } లేకపోతే ఒకవేళ(c==0x2e) {  // dot
                    ఒకవేళ(isLabel) {
                        ఆపు;  // Replacing with U+FFFD can be complicated for toASCII.
                    }
                    ఒకవేళ(i==label_ప్రారంభం) {
                        info.labelErrors|=UIDNA_ERROR_ఖాళీలు_LABEL;
                    }
                    ఒకవేళ(గమ్యం_ASCII && (i-label_ప్రారంభం)>63) {
                        info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
                    }
                    info.errors|=info.labelErrors;
                    info.labelErrors=0;
                    label_ప్రారంభం=i+1;
                }
            }
        }
        info.errors|=info.labelErrors;
        // Convert the processed ASCII prefix of the current label to UTF-16.
        int32_t mapping_ప్రారంభం=i-label_ప్రారంభం;
        destString=UnicodeString::fromUTF8(StringPiece(destArray+label_ప్రారంభం, mapping_ప్రారంభం));
        // Output the previous ASCII labels and process the rest of src in UTF-16.
        dest.Append(destArray, label_ప్రారంభం);
        processUnicode(UnicodeString::fromUTF8(StringPiece(src, label_ప్రారంభం)), 0, mapping_ప్రారంభం,
                       isLabel, గమ్యం_ASCII,
                       destString, info, error_సంకేతం);
    } లేకపోతే {
        // src is too long for the ASCII fastpath implementation.
        processUnicode(UnicodeString::fromUTF8(src), 0, 0,
                       isLabel, గమ్యం_ASCII,
                       destString, info, error_సంకేతం);
    }
    destString.గమ్యం_UTF8(dest);  // calls dest.Flush()
    ఒకవేళ(గమ్యం_ASCII && !isLabel) {
        // length==labelStart==254 means that there is a trailing dot (ok) and
        // destString is empty (do not index at 253-labelStart).
        int32_t length=label_ప్రారంభం+destString.length();
        ఒకవేళ( length>=254 && isASCIIString(destString) &&
            (length>254 ||
             (label_ప్రారంభం<254 && destString[253-label_ప్రారంభం]!=0x2e))
        ) {
            info.errors|=UIDNA_ERROR_DOMAIN_NAME_TOO_LONG;
        }
    }
    ఒకవేళ( info.isBiDi && U_SUCCESS(error_సంకేతం) && (info.errors&severeErrors)==0 &&
        (!info.isOkBiDi || (label_ప్రారంభం>0 && !isASCIIOkBiDi(srcArray, label_ప్రారంభం)))
    ) {
        info.errors|=UIDNA_ERROR_BIDI;
    }
}

UnicodeString&
UTS46::processUnicode(స్థిరరాశి UnicodeString& src,
                      int32_t label_ప్రారంభం,
                      int32_t mapping_ప్రారంభం,
                      UBool isLabel,
                      UBool గమ్యం_ASCII,
                      UnicodeString& dest U_LIFETIME_BOUND,
                      IDNAInfo& info,
                      UError_సంకేతం& error_సంకేతం) స్థిరరాశి {
    ఒకవేళ(mapping_ప్రారంభం==0) {
        uts46Norm2.normalize(src, dest, error_సంకేతం);
    } లేకపోతే {
        uts46Norm2.normalize_రెండవ_And_Append(dest, src.తాత్కాలిక_Sub_String(mapping_ప్రారంభం), error_సంకేతం);
    }
    ఒకవేళ(U_FAILURE(error_సంకేతం)) {
        తిరిగి dest;
    }
    UBool doMapDevChars=
        గమ్యం_ASCII ? (options&UIDNA_NONTRANSITIONAL_గమ్యం_ASCII)==0 :
                  (options&UIDNA_NONTRANSITIONAL_గమ్యం_UNICODE)==0;
    స్థిరరాశి char16_t *destArray=dest.get_బఫరు();
    int32_t dest_నిడివి=dest.length();
    int32_t label_హద్దు=label_ప్రారంభం;
    అయితే(label_హద్దు<dest_నిడివి) {
        char16_t c=destArray[label_హద్దు];
        ఒకవేళ(c==0x2e && !isLabel) {
            int32_t label_నిడివి=label_హద్దు-label_ప్రారంభం;
            int32_t new_నిడివి=processLabel(dest, label_ప్రారంభం, label_నిడివి,
                                            గమ్యం_ASCII, info, error_సంకేతం);
            info.errors|=info.labelErrors;
            info.labelErrors=0;
            ఒకవేళ(U_FAILURE(error_సంకేతం)) {
                తిరిగి dest;
            }
            destArray=dest.get_బఫరు();
            dest_నిడివి+=new_నిడివి-label_నిడివి;
            label_హద్దు=label_ప్రారంభం+=new_నిడివి+1;
            కొనసాగు;
        } లేకపోతే ఒకవేళ(c<0xdf) {
            // pass
        } లేకపోతే ఒకవేళ(c<=0x200d && (c==0xdf || c==0x3c2 || c>=0x200c)) {
            info.isTransDiff=నిజం;
            ఒకవేళ(doMapDevChars) {
                dest_నిడివి=mapDevChars(dest, label_ప్రారంభం, label_హద్దు, error_సంకేతం);
                ఒకవేళ(U_FAILURE(error_సంకేతం)) {
                    తిరిగి dest;
                }
                destArray=dest.get_బఫరు();
                // All deviation characters have been mapped, no need to check for them again.
                doMapDevChars=అబద్ధం;
                // Do not increment labelLimit in case c was removed.
                కొనసాగు;
            }
        } లేకపోతే ఒకవేళ(U16_IS_SURROGATE(c)) {
            ఒకవేళ(U16_IS_SURROGATE_LEAD(c) ?
                    (label_హద్దు+1)==dest_నిడివి || !U16_IS_TRAIL(destArray[label_హద్దు+1]) :
                    label_హద్దు==label_ప్రారంభం || !U16_IS_LEAD(destArray[label_హద్దు-1])) {
                // Map an unpaired surrogate to U+FFFD before normalization so that when
                // that removes characters we do not turn two unpaired ones into a pair.
                info.labelErrors|=UIDNA_ERROR_DISALLOWED;
                dest.సమితి_Char_At(label_హద్దు, 0xfffd);
                destArray=dest.get_బఫరు();
            }
        }
        ++label_హద్దు;
    }
    // Permit an empty label at the end (0<labelStart==labelLimit==destLength is ok)
    // but not an empty label elsewhere nor a completely empty domain name.
    // processLabel() sets UIDNA_ERROR_EMPTY_LABEL when labelLength==0.
    ఒకవేళ(0==label_ప్రారంభం || label_ప్రారంభం<label_హద్దు) {
        processLabel(dest, label_ప్రారంభం, label_హద్దు-label_ప్రారంభం,
                      గమ్యం_ASCII, info, error_సంకేతం);
        info.errors|=info.labelErrors;
    }
    తిరిగి dest;
}

int32_t
UTS46::mapDevChars(UnicodeString &dest, int32_t label_ప్రారంభం, int32_t mapping_ప్రారంభం,
                   UError_సంకేతం &error_సంకేతం) స్థిరరాశి {
    ఒకవేళ(U_FAILURE(error_సంకేతం)) {
        తిరిగి 0;
    }
    int32_t length=dest.length();
    char16_t *s=dest.get_బఫరు(dest[mapping_ప్రారంభం]==0xdf ? length+1 : length);
    ఒకవేళ(s==శూన్యసూచిక) {
        error_సంకేతం=U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి length;
    }
    int32_t capacity=dest.get_సామర్థ్యం();
    UBool didMapDevChars=అబద్ధం;
    int32_t read_సూచీ=mapping_ప్రారంభం, write_సూచీ=mapping_ప్రారంభం;
    చేయి {
        char16_t c=s[read_సూచీ++];
        ఎంపిక(c) {
        స్థితి 0xdf:
            // Map sharp s to ss.
            didMapDevChars=నిజం;
            s[write_సూచీ++]=0x73;  // Replace sharp s with first s.
            // Insert second s and account for possible buffer reallocation.
            ఒకవేళ(write_సూచీ==read_సూచీ) {
                ఒకవేళ(length==capacity) {
                    dest.release_బఫరు(length);
                    s=dest.get_బఫరు(length+1);
                    ఒకవేళ(s==శూన్యసూచిక) {
                        error_సంకేతం=U_జ్ఞాపకం_కేటాయింపు_ERROR;
                        తిరిగి length;
                    }
                    capacity=dest.get_సామర్థ్యం();
                }
                u_memmove(s+write_సూచీ+1, s+write_సూచీ, length-write_సూచీ);
                ++read_సూచీ;
            }
            s[write_సూచీ++]=0x73;
            ++length;
            ఆపు;
        స్థితి 0x3c2:  // Map final sigma to nonfinal sigma.
            didMapDevChars=నిజం;
            s[write_సూచీ++]=0x3c3;
            ఆపు;
        స్థితి 0x200c:  // Ignore/remove ZWNJ.
        స్థితి 0x200d:  // Ignore/remove ZWJ.
            didMapDevChars=నిజం;
            --length;
            ఆపు;
        అప్రమేయ:
            // Only really necessary if writeIndex was different from readIndex.
            s[write_సూచీ++]=c;
            ఆపు;
        }
    } అయితే(write_సూచీ<length);
    dest.release_బఫరు(length);
    ఒకవేళ(didMapDevChars) {
        // Mapping deviation characters might have resulted in an un-NFC string.
        // We could use either the NFC or the UTS #46 normalizer.
        // By using the UTS #46 normalizer again, we avoid having to load a second .nrm data file.
        UnicodeString normalized;
        uts46Norm2.normalize(dest.తాత్కాలిక_Sub_String(label_ప్రారంభం), normalized, error_సంకేతం);
        ఒకవేళ(U_SUCCESS(error_సంకేతం)) {
            dest.replace(label_ప్రారంభం, 0x7fffffff, normalized);
            ఒకవేళ(dest.isBogus()) {
                error_సంకేతం=U_జ్ఞాపకం_కేటాయింపు_ERROR;
            }
            తిరిగి dest.length();
        }
    }
    తిరిగి length;
}

// Replace the label in dest with the label string, if the label was modified.
// If &label==&dest then the label was modified in-place and labelLength
// is the new label length, different from label.length().
// If &label!=&dest then labelLength==label.length().
// Returns labelLength (= the new label length).
స్థిర int32_t
replaceLabel(UnicodeString &dest, int32_t dest_Label_ప్రారంభం, int32_t dest_Label_నిడివి,
             స్థిరరాశి UnicodeString &label, int32_t label_నిడివి, UError_సంకేతం &error_సంకేతం) {
    ఒకవేళ(U_FAILURE(error_సంకేతం)) {
        తిరిగి 0;
    }
    ఒకవేళ(&label!=&dest) {
        dest.replace(dest_Label_ప్రారంభం, dest_Label_నిడివి, label);
        ఒకవేళ(dest.isBogus()) {
            error_సంకేతం=U_జ్ఞాపకం_కేటాయింపు_ERROR;
            తిరిగి 0;
        }
    }
    తిరిగి label_నిడివి;
}

int32_t
UTS46::processLabel(UnicodeString &dest,
                    int32_t label_ప్రారంభం, int32_t label_నిడివి,
                    UBool గమ్యం_ASCII,
                    IDNAInfo &info, UError_సంకేతం &error_సంకేతం) స్థిరరాశి {
    ఒకవేళ(U_FAILURE(error_సంకేతం)) {
        తిరిగి 0;
    }
    UnicodeString fromPunycode;
    UnicodeString *labelString;
    స్థిరరాశి char16_t *label=dest.get_బఫరు()+label_ప్రారంభం;
    int32_t dest_Label_ప్రారంభం=label_ప్రారంభం;
    int32_t dest_Label_నిడివి=label_నిడివి;
    UBool wasPunycode;
    ఒకవేళ(label_నిడివి>=4 && label[0]==0x78 && label[1]==0x6e && label[2]==0x2d && label[3]==0x2d) {
        // Label starts with "xn--", try to un-Punycode it.
        // In IDNA2008, labels like "xn--" (decodes to an empty string) and
        // "xn--ASCII-" (decodes to just "ASCII") fail the round-trip validation from
        // comparing the ToUnicode input with the back-to-ToASCII output.
        // They are alternate encodings of the respective ASCII labels.
        // Ignore "xn---" here: It will fail Punycode.decode() which logically comes before
        // the round-trip verification.
        ఒకవేళ(label_నిడివి==4 || (label_నిడివి>5 && label[label_నిడివి-1]==u'-')) {
            info.labelErrors|=UIDNA_ERROR_INVALID_ACE_LABEL;
            తిరిగి markBadACELabel(dest, label_ప్రారంభం, label_నిడివి, గమ్యం_ASCII, info, error_సంకేతం);
        }
        wasPunycode=నిజం;
        char16_t *unicode_బఫరు=fromPunycode.get_బఫరు(-1);  // capacity==-1: most labels should fit
        ఒకవేళ(unicode_బఫరు==శూన్యసూచిక) {
            // Should never occur if we used capacity==-1 which uses the internal buffer.
            error_సంకేతం=U_జ్ఞాపకం_కేటాయింపు_ERROR;
            తిరిగి label_నిడివి;
        }
        UError_సంకేతం punycode_Error_సంకేతం=U_ZERO_ERROR;
        int32_t unicode_నిడివి=u_strFromPunycode(label+4, label_నిడివి-4,
                                                unicode_బఫరు, fromPunycode.get_సామర్థ్యం(),
                                                శూన్యసూచిక, &punycode_Error_సంకేతం);
        ఒకవేళ(punycode_Error_సంకేతం==U_బఫరు_అధికప్రవాహం_ERROR) {
            fromPunycode.release_బఫరు(0);
            unicode_బఫరు=fromPunycode.get_బఫరు(unicode_నిడివి);
            ఒకవేళ(unicode_బఫరు==శూన్యసూచిక) {
                error_సంకేతం=U_జ్ఞాపకం_కేటాయింపు_ERROR;
                తిరిగి label_నిడివి;
            }
            punycode_Error_సంకేతం=U_ZERO_ERROR;
            unicode_నిడివి=u_strFromPunycode(label+4, label_నిడివి-4,
                                            unicode_బఫరు, fromPunycode.get_సామర్థ్యం(),
                                            శూన్యసూచిక, &punycode_Error_సంకేతం);
        }
        fromPunycode.release_బఫరు(unicode_నిడివి);
        ఒకవేళ(U_FAILURE(punycode_Error_సంకేతం)) {
            info.labelErrors|=UIDNA_ERROR_PUNYCODE;
            తిరిగి markBadACELabel(dest, label_ప్రారంభం, label_నిడివి, గమ్యం_ASCII, info, error_సంకేతం);
        }
        // Check for NFC, and for characters that are not
        // valid or deviation characters according to the normalizer.
        // If there is something wrong, then the string will change.
        // Note that the normalizer passes through non-LDH ASCII and deviation characters.
        // Deviation characters are ok in Punycode even in transitional processing.
        // In the code further below, if we find non-LDH ASCII and we have UIDNA_USE_STD3_RULES
        // then we will set UIDNA_ERROR_INVALID_ACE_LABEL there too.
        UBool isValid=uts46Norm2.isNormalized(fromPunycode, error_సంకేతం);
        ఒకవేళ(U_FAILURE(error_సంకేతం)) {
            తిరిగి label_నిడివి;
        }
        // Unicode 15.1 UTS #46:
        // Added an additional condition in 4.1 Validity Criteria to
        // disallow labels such as xn--xn---epa., which do not round-trip.
        // --> Validity Criteria new criterion 4:
        // If not CheckHyphens, the label must not begin with “xn--”.
        ఒకవేళ(!isValid || fromPunycode.startsWith(UnicodeString::readOnlyAlias(u"xn--"))) {
            info.labelErrors|=UIDNA_ERROR_INVALID_ACE_LABEL;
            తిరిగి markBadACELabel(dest, label_ప్రారంభం, label_నిడివి, గమ్యం_ASCII, info, error_సంకేతం);
        }
        labelString=&fromPunycode;
        label=fromPunycode.get_బఫరు();
        label_ప్రారంభం=0;
        label_నిడివి=fromPunycode.length();
    } లేకపోతే {
        wasPunycode=అబద్ధం;
        labelString=&dest;
    }
    // Validity check
    ఒకవేళ(label_నిడివి==0) {
        info.labelErrors|=UIDNA_ERROR_ఖాళీలు_LABEL;
        తిరిగి replaceLabel(dest, dest_Label_ప్రారంభం, dest_Label_నిడివి,
                            *labelString, label_నిడివి, error_సంకేతం);
    }
    // labelLength>0
    ఒకవేళ(label_నిడివి>=4 && label[2]==0x2d && label[3]==0x2d) {
        // label starts with "??--"
        info.labelErrors|=UIDNA_ERROR_HYPHEN_3_4;
    }
    ఒకవేళ(label[0]==0x2d) {
        // label starts with "-"
        info.labelErrors|=UIDNA_ERROR_LEADING_HYPHEN;
    }
    ఒకవేళ(label[label_నిడివి-1]==0x2d) {
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
    స్థిరరాశి char16_t *హద్దు=label+label_నిడివి;
    char16_t oredChars=0;
    // If we enforce STD3 rules, then ASCII characters other than LDH and dot are disallowed.
    UBool disallowNonLDHDot=(options&UIDNA_USE_STD3_నియమాలు)!=0;
    చేయి {
        char16_t c=*s;
        ఒకవేళ(c<=0x7f) {
            ఒకవేళ(c==0x2e) {
                info.labelErrors|=UIDNA_ERROR_LABEL_HAS_DOT;
                *s=0xfffd;
            } లేకపోతే ఒకవేళ(disallowNonLDHDot && ascii_దత్తాంశం[c]<0) {
                info.labelErrors|=UIDNA_ERROR_DISALLOWED;
                *s=0xfffd;
            }
        } లేకపోతే {
            oredChars|=c;
            ఒకవేళ(c==0xfffd) {
                info.labelErrors|=UIDNA_ERROR_DISALLOWED;
            }
        }
        ++s;
    } అయితే(s<హద్దు);
    // Check for a leading combining mark after other validity checks
    // so that we don't report UIDNA_ERROR_DISALLOWED for the U+FFFD from here.
    UChar32 c;
    int32_t cp_నిడివి=0;
    // "Unsafe" is ok because unpaired surrogates were mapped to U+FFFD.
    U16_తదుపరి_UNSAFE(label, cp_నిడివి, c);
    ఒకవేళ((U_GET_GC_ముసుగు(c)&U_GC_M_ముసుగు)!=0) {
        info.labelErrors|=UIDNA_ERROR_LEADING_COMBINING_MARK;
        labelString->replace(label_ప్రారంభం, cp_నిడివి, static_cast<char16_t>(0xfffd));
        label=labelString->get_బఫరు()+label_ప్రారంభం;
        label_నిడివి+=1-cp_నిడివి;
        ఒకవేళ(labelString==&dest) {
            dest_Label_నిడివి=label_నిడివి;
        }
    }
    ఒకవేళ((info.labelErrors&severeErrors)==0) {
        // Do contextual checks only if we do not have U+FFFD from a severe error
        // because U+FFFD can make these checks fail.
        ఒకవేళ((options&UIDNA_CHECK_BIDI)!=0 && (!info.isBiDi || info.isOkBiDi)) {
            checkLabelBiDi(label, label_నిడివి, info);
        }
        ఒకవేళ( (options&UIDNA_CHECK_CONTEXTJ)!=0 && (oredChars&0x200c)==0x200c &&
            !isLabelOkContextJ(label, label_నిడివి)
        ) {
            info.labelErrors|=UIDNA_ERROR_CONTEXTJ;
        }
        ఒకవేళ((options&UIDNA_CHECK_CONTEXTO)!=0 && oredChars>=0xb7) {
            checkLabelContextO(label, label_నిడివి, info);
        }
        ఒకవేళ(గమ్యం_ASCII) {
            ఒకవేళ(wasPunycode) {
                // Leave a Punycode label unchanged if it has no severe errors.
                ఒకవేళ(dest_Label_నిడివి>63) {
                    info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
                }
                తిరిగి dest_Label_నిడివి;
            } లేకపోతే ఒకవేళ(oredChars>=0x80) {
                // Contains non-ASCII characters.
                UnicodeString punycode;
                char16_t *బఫరు=punycode.get_బఫరు(63);  // 63==maximum DNS label length
                ఒకవేళ(బఫరు==శూన్యసూచిక) {
                    error_సంకేతం=U_జ్ఞాపకం_కేటాయింపు_ERROR;
                    తిరిగి dest_Label_నిడివి;
                }
                బఫరు[0]=0x78;  // Write "xn--".
                బఫరు[1]=0x6e;
                బఫరు[2]=0x2d;
                బఫరు[3]=0x2d;
                UError_సంకేతం punycode_Error_సంకేతం=U_ZERO_ERROR;
                int32_t punycode_నిడివి=u_str_గమ్యం_Punycode(label, label_నిడివి,
                                                      బఫరు+4, punycode.get_సామర్థ్యం()-4,
                                                      శూన్యసూచిక, &punycode_Error_సంకేతం);
                ఒకవేళ(punycode_Error_సంకేతం==U_బఫరు_అధికప్రవాహం_ERROR) {
                    punycode_Error_సంకేతం=U_ZERO_ERROR;
                    punycode.release_బఫరు(4);
                    బఫరు=punycode.get_బఫరు(4+punycode_నిడివి);
                    ఒకవేళ(బఫరు==శూన్యసూచిక) {
                        error_సంకేతం=U_జ్ఞాపకం_కేటాయింపు_ERROR;
                        తిరిగి dest_Label_నిడివి;
                    }
                    punycode_నిడివి=u_str_గమ్యం_Punycode(label, label_నిడివి,
                                                  బఫరు+4, punycode.get_సామర్థ్యం()-4,
                                                  శూన్యసూచిక, &punycode_Error_సంకేతం);
                }
                punycode_నిడివి+=4;
                punycode.release_బఫరు(punycode_నిడివి);
                ఒకవేళ(U_FAILURE(punycode_Error_సంకేతం)) {
                    error_సంకేతం = punycode_Error_సంకేతం;
                    తిరిగి dest_Label_నిడివి;
                }
                ఒకవేళ(punycode_నిడివి>63) {
                    info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
                }
                తిరిగి replaceLabel(dest, dest_Label_ప్రారంభం, dest_Label_నిడివి,
                                    punycode, punycode_నిడివి, error_సంకేతం);
            } లేకపోతే {
                // all-ASCII label
                ఒకవేళ(label_నిడివి>63) {
                    info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
                }
            }
        }
    } లేకపోతే {
        // If a Punycode label has severe errors,
        // then leave it but make sure it does not look valid.
        ఒకవేళ(wasPunycode) {
            info.labelErrors|=UIDNA_ERROR_INVALID_ACE_LABEL;
            తిరిగి markBadACELabel(dest, dest_Label_ప్రారంభం, dest_Label_నిడివి, గమ్యం_ASCII, info, error_సంకేతం);
        }
    }
    తిరిగి replaceLabel(dest, dest_Label_ప్రారంభం, dest_Label_నిడివి,
                        *labelString, label_నిడివి, error_సంకేతం);
}

// Make sure an ACE label does not look valid.
// Append U+FFFD if the label has only LDH characters.
// If UIDNA_USE_STD3_RULES, also replace disallowed ASCII characters with U+FFFD.
int32_t
UTS46::markBadACELabel(UnicodeString &dest,
                       int32_t label_ప్రారంభం, int32_t label_నిడివి,
                       UBool గమ్యం_ASCII, IDNAInfo &info, UError_సంకేతం &error_సంకేతం) స్థిరరాశి {
    ఒకవేళ(U_FAILURE(error_సంకేతం)) {
        తిరిగి 0;
    }
    UBool disallowNonLDHDot=(options&UIDNA_USE_STD3_నియమాలు)!=0;
    UBool isASCII=నిజం;
    UBool onlyLDH=నిజం;
    స్థిరరాశి char16_t *label=dest.get_బఫరు()+label_ప్రారంభం;
    స్థిరరాశి char16_t *హద్దు=label+label_నిడివి;
    // Start after the initial "xn--".
    // Ok to cast away const because we own the UnicodeString.
    కొరకు(char16_t *s=const_cast<char16_t *>(label+4); s<హద్దు; ++s) {
        char16_t c=*s;
        ఒకవేళ(c<=0x7f) {
            ఒకవేళ(c==0x2e) {
                info.labelErrors|=UIDNA_ERROR_LABEL_HAS_DOT;
                *s=0xfffd;
                isASCII=onlyLDH=అబద్ధం;
            } లేకపోతే ఒకవేళ(ascii_దత్తాంశం[c]<0) {
                onlyLDH=అబద్ధం;
                ఒకవేళ(disallowNonLDHDot) {
                    *s=0xfffd;
                    isASCII=అబద్ధం;
                }
            }
        } లేకపోతే {
            isASCII=onlyLDH=అబద్ధం;
        }
    }
    ఒకవేళ(onlyLDH) {
        dest.insert(label_ప్రారంభం + label_నిడివి, static_cast<char16_t>(0xfffd));
        ఒకవేళ(dest.isBogus()) {
            error_సంకేతం=U_జ్ఞాపకం_కేటాయింపు_ERROR;
            తిరిగి 0;
        }
        ++label_నిడివి;
    } లేకపోతే {
        ఒకవేళ(గమ్యం_ASCII && isASCII && label_నిడివి>63) {
            info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
        }
    }
    తిరిగి label_నిడివి;
}

స్థిరరాశి uint32_t L_ముసుగు=U_ముసుగు(U_ఎడమ_గమ్యం_కుడి);
స్థిరరాశి uint32_t R_AL_ముసుగు=U_ముసుగు(U_కుడి_గమ్యం_ఎడమ)|U_ముసుగు(U_కుడి_గమ్యం_ఎడమ_ARABIC);
స్థిరరాశి uint32_t L_R_AL_ముసుగు=L_ముసుగు|R_AL_ముసుగు;

స్థిరరాశి uint32_t R_AL_AN_ముసుగు=R_AL_ముసుగు|U_ముసుగు(U_ARABIC_సంఖ్య);

స్థిరరాశి uint32_t EN_AN_ముసుగు=U_ముసుగు(U_EUROPEAN_సంఖ్య)|U_ముసుగు(U_ARABIC_సంఖ్య);
స్థిరరాశి uint32_t R_AL_EN_AN_ముసుగు=R_AL_ముసుగు|EN_AN_ముసుగు;
స్థిరరాశి uint32_t L_EN_ముసుగు=L_ముసుగు|U_ముసుగు(U_EUROPEAN_సంఖ్య);

స్థిరరాశి uint32_t ES_CS_ET_ON_BN_NSM_ముసుగు=
    U_ముసుగు(U_EUROPEAN_సంఖ్య_SEPARATOR)|
    U_ముసుగు(U_COMMON_సంఖ్య_SEPARATOR)|
    U_ముసుగు(U_EUROPEAN_సంఖ్య_TERMINATOR)|
    U_ముసుగు(U_OTHER_NEUTRAL)|
    U_ముసుగు(U_BOUNDARY_NEUTRAL)|
    U_ముసుగు(U_DIR_NON_SPACING_MARK);
స్థిరరాశి uint32_t L_EN_ES_CS_ET_ON_BN_NSM_ముసుగు=L_EN_ముసుగు|ES_CS_ET_ON_BN_NSM_ముసుగు;
స్థిరరాశి uint32_t R_AL_AN_EN_ES_CS_ET_ON_BN_NSM_ముసుగు=R_AL_ముసుగు|EN_AN_ముసుగు|ES_CS_ET_ON_BN_NSM_ముసుగు;

// We scan the whole label and check both for whether it contains RTL characters
// and whether it passes the BiDi Rule.
// In a BiDi domain name, all labels must pass the BiDi Rule, but we might find
// that a domain name is a BiDi domain name (has an RTL label) only after
// processing several earlier labels.
శూన్య
UTS46::checkLabelBiDi(స్థిరరాశి char16_t *label, int32_t label_నిడివి, IDNAInfo &info) స్థిరరాశి {
    // IDNA2008 BiDi rule
    // Get the directionality of the first character.
    UChar32 c;
    int32_t i=0;
    U16_తదుపరి_UNSAFE(label, i, c);
    uint32_t మొదటి_ముసుగు=U_ముసుగు(u_charDirection(c));
    // 1. The first character must be a character with BIDI property L, R
    // or AL.  If it has the R or AL property, it is an RTL label; if it
    // has the L property, it is an LTR label.
    ఒకవేళ((మొదటి_ముసుగు&~L_R_AL_ముసుగు)!=0) {
        info.isOkBiDi=అబద్ధం;
    }
    // Get the directionality of the last non-NSM character.
    uint32_t చివరి_ముసుగు;
    కొరకు(;;) {
        ఒకవేళ(i>=label_నిడివి) {
            చివరి_ముసుగు=మొదటి_ముసుగు;
            ఆపు;
        }
        U16_PREV_UNSAFE(label, label_నిడివి, c);
        UCharDirection dir=u_charDirection(c);
        ఒకవేళ(dir!=U_DIR_NON_SPACING_MARK) {
            చివరి_ముసుగు=U_ముసుగు(dir);
            ఆపు;
        }
    }
    // 3. In an RTL label, the end of the label must be a character with
    // BIDI property R, AL, EN or AN, followed by zero or more
    // characters with BIDI property NSM.
    // 6. In an LTR label, the end of the label must be a character with
    // BIDI property L or EN, followed by zero or more characters with
    // BIDI property NSM.
    ఒకవేళ( (మొదటి_ముసుగు&L_ముసుగు)!=0 ?
            (చివరి_ముసుగు&~L_EN_ముసుగు)!=0 :
            (చివరి_ముసుగు&~R_AL_EN_AN_ముసుగు)!=0
    ) {
        info.isOkBiDi=అబద్ధం;
    }
    // Add the directionalities of the intervening characters.
    uint32_t mask=మొదటి_ముసుగు|చివరి_ముసుగు;
    అయితే(i<label_నిడివి) {
        U16_తదుపరి_UNSAFE(label, i, c);
        mask|=U_ముసుగు(u_charDirection(c));
    }
    ఒకవేళ(మొదటి_ముసుగు&L_ముసుగు) {
        // 5. In an LTR label, only characters with the BIDI properties L, EN,
        // ES, CS, ET, ON, BN and NSM are allowed.
        ఒకవేళ((mask&~L_EN_ES_CS_ET_ON_BN_NSM_ముసుగు)!=0) {
            info.isOkBiDi=అబద్ధం;
        }
    } లేకపోతే {
        // 2. In an RTL label, only characters with the BIDI properties R, AL,
        // AN, EN, ES, CS, ET, ON, BN and NSM are allowed.
        ఒకవేళ((mask&~R_AL_AN_EN_ES_CS_ET_ON_BN_NSM_ముసుగు)!=0) {
            info.isOkBiDi=అబద్ధం;
        }
        // 4. In an RTL label, if an EN is present, no AN may be present, and
        // vice versa.
        ఒకవేళ((mask&EN_AN_ముసుగు)==EN_AN_ముసుగు) {
            info.isOkBiDi=అబద్ధం;
        }
    }
    // An RTL label is a label that contains at least one character of type
    // R, AL or AN. [...]
    // A "BIDI domain name" is a domain name that contains at least one RTL
    // label. [...]
    // The following rule, consisting of six conditions, applies to labels
    // in BIDI domain names.
    ఒకవేళ((mask&R_AL_AN_ముసుగు)!=0) {
        info.isBiDi=నిజం;
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
స్థిర UBool
isASCIIOkBiDi(స్థిరరాశి char16_t *s, int32_t length) {
    int32_t label_ప్రారంభం=0;
    కొరకు(int32_t i=0; i<length; ++i) {
        char16_t c=s[i];
        ఒకవేళ(c==0x2e) {  // dot
            ఒకవేళ(i>label_ప్రారంభం) {
                c=s[i-1];
                ఒకవేళ(!(0x61<=c && c<=0x7a) && !(0x30<=c && c<=0x39)) {
                    // Last character in the label is not an L or EN.
                    తిరిగి అబద్ధం;
                }
            }
            label_ప్రారంభం=i+1;
        } లేకపోతే ఒకవేళ(i==label_ప్రారంభం) {
            ఒకవేళ(!(0x61<=c && c<=0x7a)) {
                // First character in the label is not an L.
                తిరిగి అబద్ధం;
            }
        } లేకపోతే {
            ఒకవేళ(c<=0x20 && (c>=0x1c || (9<=c && c<=0xd))) {
                // Intermediate character in the label is a B, S or WS.
                తిరిగి అబద్ధం;
            }
        }
    }
    తిరిగి నిజం;
}

// UTF-8 version, called for source ASCII prefix.
// Can contain uppercase A-Z.
// s[length-1] must be the trailing dot.
స్థిర UBool
isASCIIOkBiDi(స్థిరరాశి అక్షరం *s, int32_t length) {
    int32_t label_ప్రారంభం=0;
    కొరకు(int32_t i=0; i<length; ++i) {
        అక్షరం c=s[i];
        ఒకవేళ(c==0x2e) {  // dot
            ఒకవేళ(i>label_ప్రారంభం) {
                c=s[i-1];
                ఒకవేళ(!(0x61<=c && c<=0x7a) && !(0x41<=c && c<=0x5a) && !(0x30<=c && c<=0x39)) {
                    // Last character in the label is not an L or EN.
                    తిరిగి అబద్ధం;
                }
            }
            label_ప్రారంభం=i+1;
        } లేకపోతే ఒకవేళ(i==label_ప్రారంభం) {
            ఒకవేళ(!(0x61<=c && c<=0x7a) && !(0x41<=c && c<=0x5a)) {
                // First character in the label is not an L.
                తిరిగి అబద్ధం;
            }
        } లేకపోతే {
            ఒకవేళ(c<=0x20 && (c>=0x1c || (9<=c && c<=0xd))) {
                // Intermediate character in the label is a B, S or WS.
                తిరిగి అబద్ధం;
            }
        }
    }
    తిరిగి నిజం;
}

UBool
UTS46::isLabelOkContextJ(స్థిరరాశి char16_t *label, int32_t label_నిడివి) స్థిరరాశి {
    // [IDNA2008-Tables]
    // 200C..200D  ; CONTEXTJ    # ZERO WIDTH NON-JOINER..ZERO WIDTH JOINER
    కొరకు(int32_t i=0; i<label_నిడివి; ++i) {
        ఒకవేళ(label[i]==0x200c) {
            // Appendix A.1. ZERO WIDTH NON-JOINER
            // Rule Set:
            //  False;
            //  If Canonical_Combining_Class(Before(cp)) .eq.  Virama Then True;
            //  If RegExpMatch((Joining_Type:{L,D})(Joining_Type:T)*\u200C
            //     (Joining_Type:T)*(Joining_Type:{R,D})) Then True;
            ఒకవేళ(i==0) {
                తిరిగి అబద్ధం;
            }
            UChar32 c;
            int32_t j=i;
            U16_PREV_UNSAFE(label, j, c);
            ఒకవేళ(uts46Norm2.getCombiningClass(c)==9) {
                కొనసాగు;
            }
            // check precontext (Joining_Type:{L,D})(Joining_Type:T)*
            కొరకు(;;) {
                UJoiningType type=ubidi_getJoiningType(c);
                ఒకవేళ(type==U_JT_TRANSPARENT) {
                    ఒకవేళ(j==0) {
                        తిరిగి అబద్ధం;
                    }
                    U16_PREV_UNSAFE(label, j, c);
                } లేకపోతే ఒకవేళ(type==U_JT_ఎడమ_JOINING || type==U_JT_DUAL_JOINING) {
                    ఆపు;  // precontext fulfilled
                } లేకపోతే {
                    తిరిగి అబద్ధం;
                }
            }
            // check postcontext (Joining_Type:T)*(Joining_Type:{R,D})
            కొరకు(j=i+1;;) {
                ఒకవేళ(j==label_నిడివి) {
                    తిరిగి అబద్ధం;
                }
                U16_తదుపరి_UNSAFE(label, j, c);
                UJoiningType type=ubidi_getJoiningType(c);
                ఒకవేళ(type==U_JT_TRANSPARENT) {
                    // just skip this character
                } లేకపోతే ఒకవేళ(type==U_JT_కుడి_JOINING || type==U_JT_DUAL_JOINING) {
                    ఆపు;  // postcontext fulfilled
                } లేకపోతే {
                    తిరిగి అబద్ధం;
                }
            }
        } లేకపోతే ఒకవేళ(label[i]==0x200d) {
            // Appendix A.2. ZERO WIDTH JOINER (U+200D)
            // Rule Set:
            //  False;
            //  If Canonical_Combining_Class(Before(cp)) .eq.  Virama Then True;
            ఒకవేళ(i==0) {
                తిరిగి అబద్ధం;
            }
            UChar32 c;
            int32_t j=i;
            U16_PREV_UNSAFE(label, j, c);
            ఒకవేళ(uts46Norm2.getCombiningClass(c)!=9) {
                తిరిగి అబద్ధం;
            }
        }
    }
    తిరిగి నిజం;
}

శూన్య
UTS46::checkLabelContextO(స్థిరరాశి char16_t *label, int32_t label_నిడివి, IDNAInfo &info) స్థిరరాశి {
    int32_t label_చివర=label_నిడివి-1;  // inclusive
    int32_t arabicDigits=0;  // -1 for 066x, +1 for 06Fx
    కొరకు(int32_t i=0; i<=label_చివర; ++i) {
        UChar32 c=label[i];
        ఒకవేళ(c<0xb7) {
            // ASCII fastpath
        } లేకపోతే ఒకవేళ(c<=0x6f9) {
            ఒకవేళ(c==0xb7) {
                // Appendix A.3. MIDDLE DOT (U+00B7)
                // Rule Set:
                //  False;
                //  If Before(cp) .eq.  U+006C And
                //     After(cp) .eq.  U+006C Then True;
                ఒకవేళ(!(0<i && label[i-1]==0x6c &&
                     i<label_చివర && label[i+1]==0x6c)) {
                    info.labelErrors|=UIDNA_ERROR_CONTEXTO_PUNCTUATION;
                }
            } లేకపోతే ఒకవేళ(c==0x375) {
                // Appendix A.4. GREEK LOWER NUMERAL SIGN (KERAIA) (U+0375)
                // Rule Set:
                //  False;
                //  If Script(After(cp)) .eq.  Greek Then True;
                UScript_సంకేతం script=USCRIPT_INVALID_సంకేతం;
                ఒకవేళ(i<label_చివర) {
                    UError_సంకేతం error_సంకేతం=U_ZERO_ERROR;
                    int32_t j=i+1;
                    U16_తదుపరి(label, j, label_నిడివి, c);
                    script=uscript_getScript(c, &error_సంకేతం);
                }
                ఒకవేళ(script!=USCRIPT_GREEK) {
                    info.labelErrors|=UIDNA_ERROR_CONTEXTO_PUNCTUATION;
                }
            } లేకపోతే ఒకవేళ(c==0x5f3 || c==0x5f4) {
                // Appendix A.5. HEBREW PUNCTUATION GERESH (U+05F3)
                // Rule Set:
                //  False;
                //  If Script(Before(cp)) .eq.  Hebrew Then True;
                //
                // Appendix A.6. HEBREW PUNCTUATION GERSHAYIM (U+05F4)
                // Rule Set:
                //  False;
                //  If Script(Before(cp)) .eq.  Hebrew Then True;
                UScript_సంకేతం script=USCRIPT_INVALID_సంకేతం;
                ఒకవేళ(0<i) {
                    UError_సంకేతం error_సంకేతం=U_ZERO_ERROR;
                    int32_t j=i;
                    U16_PREV(label, 0, j, c);
                    script=uscript_getScript(c, &error_సంకేతం);
                }
                ఒకవేళ(script!=USCRIPT_HEBREW) {
                    info.labelErrors|=UIDNA_ERROR_CONTEXTO_PUNCTUATION;
                }
            } లేకపోతే ఒకవేళ(0x660<=c /* && c<=0x6f9 */) {
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
                ఒకవేళ(c<=0x669) {
                    ఒకవేళ(arabicDigits>0) {
                        info.labelErrors|=UIDNA_ERROR_CONTEXTO_DIGITS;
                    }
                    arabicDigits=-1;
                } లేకపోతే ఒకవేళ(0x6f0<=c) {
                    ఒకవేళ(arabicDigits<0) {
                        info.labelErrors|=UIDNA_ERROR_CONTEXTO_DIGITS;
                    }
                    arabicDigits=1;
                }
            }
        } లేకపోతే ఒకవేళ(c==0x30fb) {
            // Appendix A.7. KATAKANA MIDDLE DOT (U+30FB)
            // Rule Set:
            //  False;
            //  For All Characters:
            //    If Script(cp) .in. {Hiragana, Katakana, Han} Then True;
            //  End For;
            UError_సంకేతం error_సంకేతం=U_ZERO_ERROR;
            కొరకు(పూర్ణాంకం j=0;;) {
                ఒకవేళ(j>label_చివర) {
                    info.labelErrors|=UIDNA_ERROR_CONTEXTO_PUNCTUATION;
                    ఆపు;
                }
                U16_తదుపరి(label, j, label_నిడివి, c);
                UScript_సంకేతం script=uscript_getScript(c, &error_సంకేతం);
                ఒకవేళ(script==USCRIPT_HIRAGANA || script==USCRIPT_KATAKANA || script==USCRIPT_HAN) {
                    ఆపు;
                }
            }
        }
    }
}

U_NAMESPACE_చివర

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

U_CAPI UIDNA * U_EXPORT2
uidna_openUTS46(uint32_t options, UError_సంకేతం *p_Error_సంకేతం) {
    తిరిగి reinterpret_cast<UIDNA *>(IDNA::createUTS46Instance(options, *p_Error_సంకేతం));
}

U_CAPI శూన్య U_EXPORT2
uidna_close(UIDNA *idna) {
    తొలగించు reinterpret_cast<IDNA *>(idna);
}

స్థిర UBool
check_వాదనలు(స్థిరరాశి శూన్య *label, int32_t length,
          శూన్య *dest, int32_t capacity,
          UIDNAInfo *pInfo, UError_సంకేతం *p_Error_సంకేతం) {
    ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి అబద్ధం;
    }
    // sizeof(UIDNAInfo)=16 in the first API version.
    ఒకవేళ(pInfo==శూన్యసూచిక || pInfo->size<16) {
        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి అబద్ధం;
    }
    ఒకవేళ( (label==శూన్యసూచిక ? length!=0 : length<-1) ||
        (dest==శూన్యసూచిక ? capacity!=0 : capacity<0) ||
        (dest==label && label!=శూన్యసూచిక)
    ) {
        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి అబద్ధం;
    }
    // Set all *pInfo bytes to 0 except for the size field itself.
    uprv_memset(&pInfo->size+1, 0, pInfo->size-పరిమాణం(pInfo->size));
    తిరిగి నిజం;
}

స్థిర శూన్య
idna_Info_గమ్యం_Struct(IDNAInfo &info, UIDNAInfo *pInfo) {
    pInfo->isTransitionalDifferent=info.isTransitionalDifferent();
    pInfo->errors=info.getErrors();
}

U_CAPI int32_t U_EXPORT2
uidna_label_గమ్యం_ASCII(స్థిరరాశి UIDNA *idna,
                   స్థిరరాశి char16_t *label, int32_t length,
                   char16_t *dest, int32_t capacity,
                   UIDNAInfo *pInfo, UError_సంకేతం *p_Error_సంకేతం) {
    ఒకవేళ(!check_వాదనలు(label, length, dest, capacity, pInfo, p_Error_సంకేతం)) {
        తిరిగి 0;
    }
    UnicodeString src(length < 0, label, length);
    UnicodeString destString(dest, 0, capacity);
    IDNAInfo info;
    reinterpret_cast<స్థిరరాశి IDNA *>(idna)->label_గమ్యం_ASCII(src, destString, info, *p_Error_సంకేతం);
    idna_Info_గమ్యం_Struct(info, pInfo);
    తిరిగి destString.extract(dest, capacity, *p_Error_సంకేతం);
}

U_CAPI int32_t U_EXPORT2
uidna_label_గమ్యం_Unicode(స్థిరరాశి UIDNA *idna,
                     స్థిరరాశి char16_t *label, int32_t length,
                     char16_t *dest, int32_t capacity,
                     UIDNAInfo *pInfo, UError_సంకేతం *p_Error_సంకేతం) {
    ఒకవేళ(!check_వాదనలు(label, length, dest, capacity, pInfo, p_Error_సంకేతం)) {
        తిరిగి 0;
    }
    UnicodeString src(length < 0, label, length);
    UnicodeString destString(dest, 0, capacity);
    IDNAInfo info;
    reinterpret_cast<స్థిరరాశి IDNA *>(idna)->label_గమ్యం_Unicode(src, destString, info, *p_Error_సంకేతం);
    idna_Info_గమ్యం_Struct(info, pInfo);
    తిరిగి destString.extract(dest, capacity, *p_Error_సంకేతం);
}

U_CAPI int32_t U_EXPORT2
uidna_name_గమ్యం_ASCII(స్థిరరాశి UIDNA *idna,
                  స్థిరరాశి char16_t *name, int32_t length,
                  char16_t *dest, int32_t capacity,
                  UIDNAInfo *pInfo, UError_సంకేతం *p_Error_సంకేతం) {
    ఒకవేళ(!check_వాదనలు(name, length, dest, capacity, pInfo, p_Error_సంకేతం)) {
        తిరిగి 0;
    }
    UnicodeString src(length < 0, name, length);
    UnicodeString destString(dest, 0, capacity);
    IDNAInfo info;
    reinterpret_cast<స్థిరరాశి IDNA *>(idna)->name_గమ్యం_ASCII(src, destString, info, *p_Error_సంకేతం);
    idna_Info_గమ్యం_Struct(info, pInfo);
    తిరిగి destString.extract(dest, capacity, *p_Error_సంకేతం);
}

U_CAPI int32_t U_EXPORT2
uidna_name_గమ్యం_Unicode(స్థిరరాశి UIDNA *idna,
                    స్థిరరాశి char16_t *name, int32_t length,
                    char16_t *dest, int32_t capacity,
                    UIDNAInfo *pInfo, UError_సంకేతం *p_Error_సంకేతం) {
    ఒకవేళ(!check_వాదనలు(name, length, dest, capacity, pInfo, p_Error_సంకేతం)) {
        తిరిగి 0;
    }
    UnicodeString src(length < 0, name, length);
    UnicodeString destString(dest, 0, capacity);
    IDNAInfo info;
    reinterpret_cast<స్థిరరాశి IDNA *>(idna)->name_గమ్యం_Unicode(src, destString, info, *p_Error_సంకేతం);
    idna_Info_గమ్యం_Struct(info, pInfo);
    తిరిగి destString.extract(dest, capacity, *p_Error_సంకేతం);
}

U_CAPI int32_t U_EXPORT2
uidna_label_గమ్యం_ASCII_UTF8(స్థిరరాశి UIDNA *idna,
                        స్థిరరాశి అక్షరం *label, int32_t length,
                        అక్షరం *dest, int32_t capacity,
                        UIDNAInfo *pInfo, UError_సంకేతం *p_Error_సంకేతం) {
    ఒకవేళ(!check_వాదనలు(label, length, dest, capacity, pInfo, p_Error_సంకేతం)) {
        తిరిగి 0;
    }
    StringPiece src(label, length<0 ? static_cast<int32_t>(uprv_strlen(label)) : length);
    తిరిగి ByteSinkUtil::ద్వారా_Byte_Sink_గమ్యం_Terminated_Chars(
        dest, capacity,
        [&](ByteSink& sink, UError_సంకేతం& స్థితి_2) {
            IDNAInfo info;
            reinterpret_cast<స్థిరరాశి IDNA *>(idna)->label_గమ్యం_ASCII_UTF8(src, sink, info, స్థితి_2);
            idna_Info_గమ్యం_Struct(info, pInfo);
        },
        *p_Error_సంకేతం);
}

U_CAPI int32_t U_EXPORT2
uidna_label_గమ్యం_Unicode_UTF8(స్థిరరాశి UIDNA *idna,
                         స్థిరరాశి అక్షరం *label, int32_t length,
                         అక్షరం *dest, int32_t capacity,
                         UIDNAInfo *pInfo, UError_సంకేతం *p_Error_సంకేతం) {
    ఒకవేళ(!check_వాదనలు(label, length, dest, capacity, pInfo, p_Error_సంకేతం)) {
        తిరిగి 0;
    }
    StringPiece src(label, length<0 ? static_cast<int32_t>(uprv_strlen(label)) : length);
    తిరిగి ByteSinkUtil::ద్వారా_Byte_Sink_గమ్యం_Terminated_Chars(
        dest, capacity,
        [&](ByteSink& sink, UError_సంకేతం& స్థితి_2) {
            IDNAInfo info;
            reinterpret_cast<స్థిరరాశి IDNA *>(idna)->label_గమ్యం_Unicode_UTF8(src, sink, info, స్థితి_2);
            idna_Info_గమ్యం_Struct(info, pInfo);
        },
        *p_Error_సంకేతం);
}

U_CAPI int32_t U_EXPORT2
uidna_name_గమ్యం_ASCII_UTF8(స్థిరరాశి UIDNA *idna,
                       స్థిరరాశి అక్షరం *name, int32_t length,
                       అక్షరం *dest, int32_t capacity,
                       UIDNAInfo *pInfo, UError_సంకేతం *p_Error_సంకేతం) {
    ఒకవేళ(!check_వాదనలు(name, length, dest, capacity, pInfo, p_Error_సంకేతం)) {
        తిరిగి 0;
    }
    StringPiece src(name, length<0 ? static_cast<int32_t>(uprv_strlen(name)) : length);
    తిరిగి ByteSinkUtil::ద్వారా_Byte_Sink_గమ్యం_Terminated_Chars(
        dest, capacity,
        [&](ByteSink& sink, UError_సంకేతం& స్థితి_2) {
            IDNAInfo info;
            reinterpret_cast<స్థిరరాశి IDNA *>(idna)->name_గమ్యం_ASCII_UTF8(src, sink, info, స్థితి_2);
            idna_Info_గమ్యం_Struct(info, pInfo);
        },
        *p_Error_సంకేతం);
}

U_CAPI int32_t U_EXPORT2
uidna_name_గమ్యం_Unicode_UTF8(స్థిరరాశి UIDNA *idna,
                        స్థిరరాశి అక్షరం *name, int32_t length,
                        అక్షరం *dest, int32_t capacity,
                        UIDNAInfo *pInfo, UError_సంకేతం *p_Error_సంకేతం) {
    ఒకవేళ(!check_వాదనలు(name, length, dest, capacity, pInfo, p_Error_సంకేతం)) {
        తిరిగి 0;
    }
    StringPiece src(name, length<0 ? static_cast<int32_t>(uprv_strlen(name)) : length);
    తిరిగి ByteSinkUtil::ద్వారా_Byte_Sink_గమ్యం_Terminated_Chars(
        dest, capacity,
        [&](ByteSink& sink, UError_సంకేతం& స్థితి_2) {
            IDNAInfo info;
            reinterpret_cast<స్థిరరాశి IDNA *>(idna)->name_గమ్యం_Unicode_UTF8(src, sink, info, స్థితి_2);
            idna_Info_గమ్యం_Struct(info, pInfo);
        },
        *p_Error_సంకేతం);
}

#అయితే_ముగింపు  // UCONFIG_NO_IDNA
