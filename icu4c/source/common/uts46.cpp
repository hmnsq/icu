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

#यदि !UCONFIG_NO_IDNA

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
ध्रुव uint32_t severeErrors=
    UIDNA_ERROR_LEADING_COMBINING_MARK|
    UIDNA_ERROR_DISALLOWED|
    UIDNA_ERROR_PUNYCODE|
    UIDNA_ERROR_LABEL_HAS_DOT|
    UIDNA_ERROR_INVALID_ACE_LABEL;

स्थिर अन्तःस्थ UBool
isASCIIString(ध्रुव UnicodeString &dest) {
    ध्रुव char16_t *s=dest.get_बफर();
    ध्रुव char16_t *सीमा=s+dest.length();
    यावत्(s<सीमा) {
        यदि(*s++>0x7f) {
            प्रत्यागम असत्य;
        }
    }
    प्रत्यागम सत्य;
}

स्थिर UBool
isASCIIOkBiDi(ध्रुव char16_t *s, int32_t length);

स्थिर UBool
isASCIIOkBiDi(ध्रुव वर्ण *s, int32_t length);

// IDNA class default implementations -------------------------------------- ***

IDNA::~IDNA() {}

शून्य
IDNA::label_गन्तव्यम्_ASCII_UTF8(StringPiece label, ByteSink &dest,
                        IDNAInfo &info, UError_संहिता &error_संहिता) ध्रुव {
    यदि(U_SUCCESS(error_संहिता)) {
        UnicodeString destString;
        label_गन्तव्यम्_ASCII(UnicodeString::fromUTF8(label), destString,
                     info, error_संहिता).गन्तव्यम्_UTF8(dest);
    }
}

शून्य
IDNA::label_गन्तव्यम्_Unicode_UTF8(StringPiece label, ByteSink &dest,
                         IDNAInfo &info, UError_संहिता &error_संहिता) ध्रुव {
    यदि(U_SUCCESS(error_संहिता)) {
        UnicodeString destString;
        label_गन्तव्यम्_Unicode(UnicodeString::fromUTF8(label), destString,
                       info, error_संहिता).गन्तव्यम्_UTF8(dest);
    }
}

शून्य
IDNA::name_गन्तव्यम्_ASCII_UTF8(StringPiece name, ByteSink &dest,
                       IDNAInfo &info, UError_संहिता &error_संहिता) ध्रुव {
    यदि(U_SUCCESS(error_संहिता)) {
        UnicodeString destString;
        name_गन्तव्यम्_ASCII(UnicodeString::fromUTF8(name), destString,
                    info, error_संहिता).गन्तव्यम्_UTF8(dest);
    }
}

शून्य
IDNA::name_गन्तव्यम्_Unicode_UTF8(StringPiece name, ByteSink &dest,
                        IDNAInfo &info, UError_संहिता &error_संहिता) ध्रुव {
    यदि(U_SUCCESS(error_संहिता)) {
        UnicodeString destString;
        name_गन्तव्यम्_Unicode(UnicodeString::fromUTF8(name), destString,
                      info, error_संहिता).गन्तव्यम्_UTF8(dest);
    }
}

// UTS46 class declaration ------------------------------------------------- ***

वर्ग UTS46 : सार्वजनिक IDNA {
सार्वजनिक:
    UTS46(uint32_t options, UError_संहिता &error_संहिता);
    आभास ~UTS46();

    आभास UnicodeString&
    label_गन्तव्यम्_ASCII(ध्रुव UnicodeString& label,
                 UnicodeString& dest U_LIFETIME_BOUND,
                 IDNAInfo& info,
                 UError_संहिता& error_संहिता) ध्रुव override;

    आभास UnicodeString&
    label_गन्तव्यम्_Unicode(ध्रुव UnicodeString& label,
                   UnicodeString& dest U_LIFETIME_BOUND,
                   IDNAInfo& info,
                   UError_संहिता& error_संहिता) ध्रुव override;

    आभास UnicodeString&
    name_गन्तव्यम्_ASCII(ध्रुव UnicodeString& name,
                UnicodeString& dest U_LIFETIME_BOUND,
                IDNAInfo& info,
                UError_संहिता& error_संहिता) ध्रुव override;

    आभास UnicodeString&
    name_गन्तव्यम्_Unicode(ध्रुव UnicodeString& name,
                  UnicodeString& dest U_LIFETIME_BOUND,
                  IDNAInfo& info,
                  UError_संहिता& error_संहिता) ध्रुव override;

    आभास शून्य
    label_गन्तव्यम्_ASCII_UTF8(StringPiece label, ByteSink &dest,
                      IDNAInfo &info, UError_संहिता &error_संहिता) ध्रुव override;

    आभास शून्य
    label_गन्तव्यम्_Unicode_UTF8(StringPiece label, ByteSink &dest,
                       IDNAInfo &info, UError_संहिता &error_संहिता) ध्रुव override;

    आभास शून्य
    name_गन्तव्यम्_ASCII_UTF8(StringPiece name, ByteSink &dest,
                     IDNAInfo &info, UError_संहिता &error_संहिता) ध्रुव override;

    आभास शून्य
    name_गन्तव्यम्_Unicode_UTF8(StringPiece name, ByteSink &dest,
                      IDNAInfo &info, UError_संहिता &error_संहिता) ध्रुव override;

गुप्त:
    UnicodeString&
    process(ध्रुव UnicodeString& src,
            UBool isLabel,
            UBool गन्तव्यम्_ASCII,
            UnicodeString& dest U_LIFETIME_BOUND,
            IDNAInfo& info,
            UError_संहिता& error_संहिता) ध्रुव;

    शून्य
    processUTF8(StringPiece src,
                UBool isLabel, UBool गन्तव्यम्_ASCII,
                ByteSink &dest,
                IDNAInfo &info, UError_संहिता &error_संहिता) ध्रुव;

    UnicodeString&
    processUnicode(ध्रुव UnicodeString& src,
                   int32_t labelStart,
                   int32_t mappingStart,
                   UBool isLabel,
                   UBool गन्तव्यम्_ASCII,
                   UnicodeString& dest U_LIFETIME_BOUND,
                   IDNAInfo& info,
                   UError_संहिता& error_संहिता) ध्रुव;

    // returns the new dest.length()
    int32_t
    mapDevChars(UnicodeString &dest, int32_t labelStart, int32_t mappingStart,
                UError_संहिता &error_संहिता) ध्रुव;

    // returns the new label length
    int32_t
    processLabel(UnicodeString &dest,
                 int32_t labelStart, int32_t label_दीर्घता,
                 UBool गन्तव्यम्_ASCII,
                 IDNAInfo &info, UError_संहिता &error_संहिता) ध्रुव;
    int32_t
    markBadACELabel(UnicodeString &dest,
                    int32_t labelStart, int32_t label_दीर्घता,
                    UBool गन्तव्यम्_ASCII, IDNAInfo &info, UError_संहिता &error_संहिता) ध्रुव;

    शून्य
    checkLabelBiDi(ध्रुव char16_t *label, int32_t label_दीर्घता, IDNAInfo &info) ध्रुव;

    UBool
    isLabelOkContextJ(ध्रुव char16_t *label, int32_t label_दीर्घता) ध्रुव;

    शून्य
    checkLabelContextO(ध्रुव char16_t *label, int32_t label_दीर्घता, IDNAInfo &info) ध्रुव;

    ध्रुव Normalizer2 &uts46Norm2;  // uts46.nrm
    uint32_t options;
};

IDNA *
IDNA::createUTS46Instance(uint32_t options, UError_संहिता &error_संहिता) {
    यदि(U_SUCCESS(error_संहिता)) {
        IDNA *idna=नूतन UTS46(options, error_संहिता);
        यदि(idna==शून्यसूचकः) {
            error_संहिता=U_स्मृतिः_आवण्टनम्_ERROR;
        } अन्यथा यदि(U_FAILURE(error_संहिता)) {
            विलोपय idna;
            idna=शून्यसूचकः;
        }
        प्रत्यागम idna;
    } अन्यथा {
        प्रत्यागम शून्यसूचकः;
    }
}

// UTS46 implementation ---------------------------------------------------- ***

UTS46::UTS46(uint32_t opt, UError_संहिता &error_संहिता)
        : uts46Norm2(*Normalizer2::getInstance(शून्यसूचकः, "uts46", UNORM2_COMPOSE, error_संहिता)),
          options(opt) {}

UTS46::~UTS46() {}

UnicodeString&
UTS46::label_गन्तव्यम्_ASCII(ध्रुव UnicodeString& label,
                    UnicodeString& dest U_LIFETIME_BOUND,
                    IDNAInfo& info,
                    UError_संहिता& error_संहिता) ध्रुव {
    प्रत्यागम process(label, सत्य, सत्य, dest, info, error_संहिता);
}

UnicodeString&
UTS46::label_गन्तव्यम्_Unicode(ध्रुव UnicodeString& label,
                      UnicodeString& dest U_LIFETIME_BOUND,
                      IDNAInfo& info,
                      UError_संहिता& error_संहिता) ध्रुव {
    प्रत्यागम process(label, सत्य, असत्य, dest, info, error_संहिता);
}

UnicodeString&
UTS46::name_गन्तव्यम्_ASCII(ध्रुव UnicodeString& name,
                   UnicodeString& dest U_LIFETIME_BOUND,
                   IDNAInfo& info,
                   UError_संहिता& error_संहिता) ध्रुव {
    process(name, असत्य, सत्य, dest, info, error_संहिता);
    यदि( dest.length()>=254 && (info.errors&UIDNA_ERROR_DOMAIN_NAME_TOO_LONG)==0 &&
        isASCIIString(dest) &&
        (dest.length()>254 || dest[253]!=0x2e)
    ) {
        info.errors|=UIDNA_ERROR_DOMAIN_NAME_TOO_LONG;
    }
    प्रत्यागम dest;
}

UnicodeString&
UTS46::name_गन्तव्यम्_Unicode(ध्रुव UnicodeString& name,
                     UnicodeString& dest U_LIFETIME_BOUND,
                     IDNAInfo& info,
                     UError_संहिता& error_संहिता) ध्रुव {
    प्रत्यागम process(name, असत्य, असत्य, dest, info, error_संहिता);
}

शून्य
UTS46::label_गन्तव्यम्_ASCII_UTF8(StringPiece label, ByteSink &dest,
                         IDNAInfo &info, UError_संहिता &error_संहिता) ध्रुव {
    processUTF8(label, सत्य, सत्य, dest, info, error_संहिता);
}

शून्य
UTS46::label_गन्तव्यम्_Unicode_UTF8(StringPiece label, ByteSink &dest,
                          IDNAInfo &info, UError_संहिता &error_संहिता) ध्रुव {
    processUTF8(label, सत्य, असत्य, dest, info, error_संहिता);
}

शून्य
UTS46::name_गन्तव्यम्_ASCII_UTF8(StringPiece name, ByteSink &dest,
                        IDNAInfo &info, UError_संहिता &error_संहिता) ध्रुव {
    processUTF8(name, असत्य, सत्य, dest, info, error_संहिता);
}

शून्य
UTS46::name_गन्तव्यम्_Unicode_UTF8(StringPiece name, ByteSink &dest,
                         IDNAInfo &info, UError_संहिता &error_संहिता) ध्रुव {
    processUTF8(name, असत्य, असत्य, dest, info, error_संहिता);
}

// UTS #46 data for ASCII characters.
// The normalizer (using uts46.nrm) maps uppercase ASCII letters to lowercase
// and passes through all other ASCII characters.
// If UIDNA_USE_STD3_RULES is set, then non-LDH characters are disallowed
// using this data.
// The ASCII fastpath also uses this data.
// Values: -1=disallowed  0==valid  1==mapped (lowercase)
स्थिर ध्रुव int8_t ascii_दत्त[128]={
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
UTS46::process(ध्रुव UnicodeString& src,
               UBool isLabel,
               UBool गन्तव्यम्_ASCII,
               UnicodeString& dest U_LIFETIME_BOUND,
               IDNAInfo& info,
               UError_संहिता& error_संहिता) ध्रुव {
    // uts46Norm2.normalize() would do all of this error checking and setup,
    // but with the ASCII fastpath we do not always call it, and do not
    // call it first.
    यदि(U_FAILURE(error_संहिता)) {
        dest.समुच्चयः_गन्तव्यम्_Bogus();
        प्रत्यागम dest;
    }
    ध्रुव char16_t *srcArray=src.get_बफर();
    यदि(&dest==&src || srcArray==शून्यसूचकः) {
        error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        dest.समुच्चयः_गन्तव्यम्_Bogus();
        प्रत्यागम dest;
    }
    // Arguments are fine, reset output values.
    dest.remove();
    info.reset();
    int32_t src_दीर्घता=src.length();
    यदि(src_दीर्घता==0) {
        info.errors|=UIDNA_ERROR_रिक्तानि_LABEL;
        प्रत्यागम dest;
    }
    char16_t *destArray=dest.get_बफर(src_दीर्घता);
    यदि(destArray==शून्यसूचकः) {
        error_संहिता=U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम dest;
    }
    // ASCII fastpath
    UBool disallowNonLDHDot=(options&UIDNA_USE_STD3_नियमाः)!=0;
    int32_t labelStart=0;
    int32_t i;
    प्रति(i=0;; ++i) {
        यदि(i==src_दीर्घता) {
            यदि(गन्तव्यम्_ASCII) {
                यदि((i-labelStart)>63) {
                    info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
                }
                // There is a trailing dot if labelStart==i.
                यदि(!isLabel && i>=254 && (i>254 || labelStart<i)) {
                    info.errors|=UIDNA_ERROR_DOMAIN_NAME_TOO_LONG;
                }
            }
            info.errors|=info.labelErrors;
            dest.release_बफर(i);
            प्रत्यागम dest;
        }
        char16_t c=srcArray[i];
        यदि(c>0x7f) {
            भङ्ग;
        }
        पूर्णाङ्क c_दत्त=ascii_दत्त[c];
        यदि(c_दत्त>0) {
            destArray[i]=c+0x20;  // Lowercase an uppercase ASCII letter.
        } अन्यथा यदि(c_दत्त<0 && disallowNonLDHDot) {
            भङ्ग;  // Replacing with U+FFFD can be complicated for toASCII.
        } अन्यथा {
            destArray[i]=c;
            यदि(c==0x2d) {  // hyphen
                यदि(i==(labelStart+3) && srcArray[i-1]==0x2d) {
                    // "??--..." is Punycode or forbidden.
                    ++i;  // '-' was copied to dest already
                    भङ्ग;
                }
                यदि(i==labelStart) {
                    // label starts with "-"
                    info.labelErrors|=UIDNA_ERROR_LEADING_HYPHEN;
                }
                यदि((i+1)==src_दीर्घता || srcArray[i+1]==0x2e) {
                    // label ends with "-"
                    info.labelErrors|=UIDNA_ERROR_TRAILING_HYPHEN;
                }
            } अन्यथा यदि(c==0x2e) {  // dot
                यदि(isLabel) {
                    // Replacing with U+FFFD can be complicated for toASCII.
                    ++i;  // '.' was copied to dest already
                    भङ्ग;
                }
                यदि(i==labelStart) {
                    info.labelErrors|=UIDNA_ERROR_रिक्तानि_LABEL;
                }
                यदि(गन्तव्यम्_ASCII && (i-labelStart)>63) {
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
    processUnicode(src, labelStart, i, isLabel, गन्तव्यम्_ASCII, dest, info, error_संहिता);
    यदि( info.isBiDi && U_SUCCESS(error_संहिता) && (info.errors&severeErrors)==0 &&
        (!info.isOkBiDi || (labelStart>0 && !isASCIIOkBiDi(dest.get_बफर(), labelStart)))
    ) {
        info.errors|=UIDNA_ERROR_BIDI;
    }
    प्रत्यागम dest;
}

शून्य
UTS46::processUTF8(StringPiece src,
                   UBool isLabel, UBool गन्तव्यम्_ASCII,
                   ByteSink &dest,
                   IDNAInfo &info, UError_संहिता &error_संहिता) ध्रुव {
    यदि(U_FAILURE(error_संहिता)) {
        प्रत्यागम;
    }
    ध्रुव वर्ण *srcArray=src.दत्त();
    int32_t src_दीर्घता=src.length();
    यदि(srcArray==शून्यसूचकः && src_दीर्घता!=0) {
        error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम;
    }
    // Arguments are fine, reset output values.
    info.reset();
    यदि(src_दीर्घता==0) {
        info.errors|=UIDNA_ERROR_रिक्तानि_LABEL;
        dest.Flush();
        प्रत्यागम;
    }
    UnicodeString destString;
    int32_t labelStart=0;
    यदि(src_दीर्घता<=256) {  // length of stackArray[]
        // ASCII fastpath
        वर्ण राशिः_Array[256];
        int32_t dest_क्षमता;
        वर्ण *destArray=dest.Get_Append_बफर(src_दीर्घता, src_दीर्घता+20,
                                             राशिः_Array, UPRV_LENGTHOF(राशिः_Array), &dest_क्षमता);
        UBool disallowNonLDHDot=(options&UIDNA_USE_STD3_नियमाः)!=0;
        int32_t i;
        प्रति(i=0;; ++i) {
            यदि(i==src_दीर्घता) {
                यदि(गन्तव्यम्_ASCII) {
                    यदि((i-labelStart)>63) {
                        info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
                    }
                    // There is a trailing dot if labelStart==i.
                    यदि(!isLabel && i>=254 && (i>254 || labelStart<i)) {
                        info.errors|=UIDNA_ERROR_DOMAIN_NAME_TOO_LONG;
                    }
                }
                info.errors|=info.labelErrors;
                dest.Append(destArray, i);
                dest.Flush();
                प्रत्यागम;
            }
            वर्ण c=srcArray[i];
            यदि (static_cast<int8_t>(c) < 0) { // (uint8_t)c>0x7f
                भङ्ग;
            }
            पूर्णाङ्क c_दत्त = ascii_दत्त[static_cast<पूर्णाङ्क>(c)]; // Cast: gcc warns about indexing with a char.
            यदि(c_दत्त>0) {
                destArray[i]=c+0x20;  // Lowercase an uppercase ASCII letter.
            } अन्यथा यदि(c_दत्त<0 && disallowNonLDHDot) {
                भङ्ग;  // Replacing with U+FFFD can be complicated for toASCII.
            } अन्यथा {
                destArray[i]=c;
                यदि(c==0x2d) {  // hyphen
                    यदि(i==(labelStart+3) && srcArray[i-1]==0x2d) {
                        // "??--..." is Punycode or forbidden.
                        भङ्ग;
                    }
                    यदि(i==labelStart) {
                        // label starts with "-"
                        info.labelErrors|=UIDNA_ERROR_LEADING_HYPHEN;
                    }
                    यदि((i+1)==src_दीर्घता || srcArray[i+1]==0x2e) {
                        // label ends with "-"
                        info.labelErrors|=UIDNA_ERROR_TRAILING_HYPHEN;
                    }
                } अन्यथा यदि(c==0x2e) {  // dot
                    यदि(isLabel) {
                        भङ्ग;  // Replacing with U+FFFD can be complicated for toASCII.
                    }
                    यदि(i==labelStart) {
                        info.labelErrors|=UIDNA_ERROR_रिक्तानि_LABEL;
                    }
                    यदि(गन्तव्यम्_ASCII && (i-labelStart)>63) {
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
                       isLabel, गन्तव्यम्_ASCII,
                       destString, info, error_संहिता);
    } अन्यथा {
        // src is too long for the ASCII fastpath implementation.
        processUnicode(UnicodeString::fromUTF8(src), 0, 0,
                       isLabel, गन्तव्यम्_ASCII,
                       destString, info, error_संहिता);
    }
    destString.गन्तव्यम्_UTF8(dest);  // calls dest.Flush()
    यदि(गन्तव्यम्_ASCII && !isLabel) {
        // length==labelStart==254 means that there is a trailing dot (ok) and
        // destString is empty (do not index at 253-labelStart).
        int32_t length=labelStart+destString.length();
        यदि( length>=254 && isASCIIString(destString) &&
            (length>254 ||
             (labelStart<254 && destString[253-labelStart]!=0x2e))
        ) {
            info.errors|=UIDNA_ERROR_DOMAIN_NAME_TOO_LONG;
        }
    }
    यदि( info.isBiDi && U_SUCCESS(error_संहिता) && (info.errors&severeErrors)==0 &&
        (!info.isOkBiDi || (labelStart>0 && !isASCIIOkBiDi(srcArray, labelStart)))
    ) {
        info.errors|=UIDNA_ERROR_BIDI;
    }
}

UnicodeString&
UTS46::processUnicode(ध्रुव UnicodeString& src,
                      int32_t labelStart,
                      int32_t mappingStart,
                      UBool isLabel,
                      UBool गन्तव्यम्_ASCII,
                      UnicodeString& dest U_LIFETIME_BOUND,
                      IDNAInfo& info,
                      UError_संहिता& error_संहिता) ध्रुव {
    यदि(mappingStart==0) {
        uts46Norm2.normalize(src, dest, error_संहिता);
    } अन्यथा {
        uts46Norm2.normalizeSecondAndAppend(dest, src.अस्थायी_Sub_String(mappingStart), error_संहिता);
    }
    यदि(U_FAILURE(error_संहिता)) {
        प्रत्यागम dest;
    }
    UBool doMapDevChars=
        गन्तव्यम्_ASCII ? (options&UIDNA_NONTRANSITIONAL_गन्तव्यम्_ASCII)==0 :
                  (options&UIDNA_NONTRANSITIONAL_गन्तव्यम्_UNICODE)==0;
    ध्रुव char16_t *destArray=dest.get_बफर();
    int32_t dest_दीर्घता=dest.length();
    int32_t label_सीमा=labelStart;
    यावत्(label_सीमा<dest_दीर्घता) {
        char16_t c=destArray[label_सीमा];
        यदि(c==0x2e && !isLabel) {
            int32_t label_दीर्घता=label_सीमा-labelStart;
            int32_t new_दीर्घता=processLabel(dest, labelStart, label_दीर्घता,
                                            गन्तव्यम्_ASCII, info, error_संहिता);
            info.errors|=info.labelErrors;
            info.labelErrors=0;
            यदि(U_FAILURE(error_संहिता)) {
                प्रत्यागम dest;
            }
            destArray=dest.get_बफर();
            dest_दीर्घता+=new_दीर्घता-label_दीर्घता;
            label_सीमा=labelStart+=new_दीर्घता+1;
            अनुवर्त;
        } अन्यथा यदि(c<0xdf) {
            // pass
        } अन्यथा यदि(c<=0x200d && (c==0xdf || c==0x3c2 || c>=0x200c)) {
            info.isTransDiff=सत्य;
            यदि(doMapDevChars) {
                dest_दीर्घता=mapDevChars(dest, labelStart, label_सीमा, error_संहिता);
                यदि(U_FAILURE(error_संहिता)) {
                    प्रत्यागम dest;
                }
                destArray=dest.get_बफर();
                // All deviation characters have been mapped, no need to check for them again.
                doMapDevChars=असत्य;
                // Do not increment labelLimit in case c was removed.
                अनुवर्त;
            }
        } अन्यथा यदि(U16_IS_SURROGATE(c)) {
            यदि(U16_IS_SURROGATE_LEAD(c) ?
                    (label_सीमा+1)==dest_दीर्घता || !U16_IS_TRAIL(destArray[label_सीमा+1]) :
                    label_सीमा==labelStart || !U16_IS_LEAD(destArray[label_सीमा-1])) {
                // Map an unpaired surrogate to U+FFFD before normalization so that when
                // that removes characters we do not turn two unpaired ones into a pair.
                info.labelErrors|=UIDNA_ERROR_DISALLOWED;
                dest.समुच्चयः_Char_At(label_सीमा, 0xfffd);
                destArray=dest.get_बफर();
            }
        }
        ++label_सीमा;
    }
    // Permit an empty label at the end (0<labelStart==labelLimit==destLength is ok)
    // but not an empty label elsewhere nor a completely empty domain name.
    // processLabel() sets UIDNA_ERROR_EMPTY_LABEL when labelLength==0.
    यदि(0==labelStart || labelStart<label_सीमा) {
        processLabel(dest, labelStart, label_सीमा-labelStart,
                      गन्तव्यम्_ASCII, info, error_संहिता);
        info.errors|=info.labelErrors;
    }
    प्रत्यागम dest;
}

int32_t
UTS46::mapDevChars(UnicodeString &dest, int32_t labelStart, int32_t mappingStart,
                   UError_संहिता &error_संहिता) ध्रुव {
    यदि(U_FAILURE(error_संहिता)) {
        प्रत्यागम 0;
    }
    int32_t length=dest.length();
    char16_t *s=dest.get_बफर(dest[mappingStart]==0xdf ? length+1 : length);
    यदि(s==शून्यसूचकः) {
        error_संहिता=U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम length;
    }
    int32_t capacity=dest.get_क्षमता();
    UBool didMapDevChars=असत्य;
    int32_t read_सूचकांक=mappingStart, write_सूचकांक=mappingStart;
    कुरु {
        char16_t c=s[read_सूचकांक++];
        विकल्प(c) {
        पक्ष 0xdf:
            // Map sharp s to ss.
            didMapDevChars=सत्य;
            s[write_सूचकांक++]=0x73;  // Replace sharp s with first s.
            // Insert second s and account for possible buffer reallocation.
            यदि(write_सूचकांक==read_सूचकांक) {
                यदि(length==capacity) {
                    dest.release_बफर(length);
                    s=dest.get_बफर(length+1);
                    यदि(s==शून्यसूचकः) {
                        error_संहिता=U_स्मृतिः_आवण्टनम्_ERROR;
                        प्रत्यागम length;
                    }
                    capacity=dest.get_क्षमता();
                }
                u_memmove(s+write_सूचकांक+1, s+write_सूचकांक, length-write_सूचकांक);
                ++read_सूचकांक;
            }
            s[write_सूचकांक++]=0x73;
            ++length;
            भङ्ग;
        पक्ष 0x3c2:  // Map final sigma to nonfinal sigma.
            didMapDevChars=सत्य;
            s[write_सूचकांक++]=0x3c3;
            भङ्ग;
        पक्ष 0x200c:  // Ignore/remove ZWNJ.
        पक्ष 0x200d:  // Ignore/remove ZWJ.
            didMapDevChars=सत्य;
            --length;
            भङ्ग;
        स्वभाव:
            // Only really necessary if writeIndex was different from readIndex.
            s[write_सूचकांक++]=c;
            भङ्ग;
        }
    } यावत्(write_सूचकांक<length);
    dest.release_बफर(length);
    यदि(didMapDevChars) {
        // Mapping deviation characters might have resulted in an un-NFC string.
        // We could use either the NFC or the UTS #46 normalizer.
        // By using the UTS #46 normalizer again, we avoid having to load a second .nrm data file.
        UnicodeString normalized;
        uts46Norm2.normalize(dest.अस्थायी_Sub_String(labelStart), normalized, error_संहिता);
        यदि(U_SUCCESS(error_संहिता)) {
            dest.replace(labelStart, 0x7fffffff, normalized);
            यदि(dest.isBogus()) {
                error_संहिता=U_स्मृतिः_आवण्टनम्_ERROR;
            }
            प्रत्यागम dest.length();
        }
    }
    प्रत्यागम length;
}

// Replace the label in dest with the label string, if the label was modified.
// If &label==&dest then the label was modified in-place and labelLength
// is the new label length, different from label.length().
// If &label!=&dest then labelLength==label.length().
// Returns labelLength (= the new label length).
स्थिर int32_t
replaceLabel(UnicodeString &dest, int32_t destLabelStart, int32_t dest_Label_दीर्घता,
             ध्रुव UnicodeString &label, int32_t label_दीर्घता, UError_संहिता &error_संहिता) {
    यदि(U_FAILURE(error_संहिता)) {
        प्रत्यागम 0;
    }
    यदि(&label!=&dest) {
        dest.replace(destLabelStart, dest_Label_दीर्घता, label);
        यदि(dest.isBogus()) {
            error_संहिता=U_स्मृतिः_आवण्टनम्_ERROR;
            प्रत्यागम 0;
        }
    }
    प्रत्यागम label_दीर्घता;
}

int32_t
UTS46::processLabel(UnicodeString &dest,
                    int32_t labelStart, int32_t label_दीर्घता,
                    UBool गन्तव्यम्_ASCII,
                    IDNAInfo &info, UError_संहिता &error_संहिता) ध्रुव {
    यदि(U_FAILURE(error_संहिता)) {
        प्रत्यागम 0;
    }
    UnicodeString fromPunycode;
    UnicodeString *labelString;
    ध्रुव char16_t *label=dest.get_बफर()+labelStart;
    int32_t destLabelStart=labelStart;
    int32_t dest_Label_दीर्घता=label_दीर्घता;
    UBool wasPunycode;
    यदि(label_दीर्घता>=4 && label[0]==0x78 && label[1]==0x6e && label[2]==0x2d && label[3]==0x2d) {
        // Label starts with "xn--", try to un-Punycode it.
        // In IDNA2008, labels like "xn--" (decodes to an empty string) and
        // "xn--ASCII-" (decodes to just "ASCII") fail the round-trip validation from
        // comparing the ToUnicode input with the back-to-ToASCII output.
        // They are alternate encodings of the respective ASCII labels.
        // Ignore "xn---" here: It will fail Punycode.decode() which logically comes before
        // the round-trip verification.
        यदि(label_दीर्घता==4 || (label_दीर्घता>5 && label[label_दीर्घता-1]==u'-')) {
            info.labelErrors|=UIDNA_ERROR_INVALID_ACE_LABEL;
            प्रत्यागम markBadACELabel(dest, labelStart, label_दीर्घता, गन्तव्यम्_ASCII, info, error_संहिता);
        }
        wasPunycode=सत्य;
        char16_t *unicode_बफर=fromPunycode.get_बफर(-1);  // capacity==-1: most labels should fit
        यदि(unicode_बफर==शून्यसूचकः) {
            // Should never occur if we used capacity==-1 which uses the internal buffer.
            error_संहिता=U_स्मृतिः_आवण्टनम्_ERROR;
            प्रत्यागम label_दीर्घता;
        }
        UError_संहिता punycode_Error_संहिता=U_ZERO_ERROR;
        int32_t unicode_दीर्घता=u_strFromPunycode(label+4, label_दीर्घता-4,
                                                unicode_बफर, fromPunycode.get_क्षमता(),
                                                शून्यसूचकः, &punycode_Error_संहिता);
        यदि(punycode_Error_संहिता==U_बफर_अतिप्रवाहः_ERROR) {
            fromPunycode.release_बफर(0);
            unicode_बफर=fromPunycode.get_बफर(unicode_दीर्घता);
            यदि(unicode_बफर==शून्यसूचकः) {
                error_संहिता=U_स्मृतिः_आवण्टनम्_ERROR;
                प्रत्यागम label_दीर्घता;
            }
            punycode_Error_संहिता=U_ZERO_ERROR;
            unicode_दीर्घता=u_strFromPunycode(label+4, label_दीर्घता-4,
                                            unicode_बफर, fromPunycode.get_क्षमता(),
                                            शून्यसूचकः, &punycode_Error_संहिता);
        }
        fromPunycode.release_बफर(unicode_दीर्घता);
        यदि(U_FAILURE(punycode_Error_संहिता)) {
            info.labelErrors|=UIDNA_ERROR_PUNYCODE;
            प्रत्यागम markBadACELabel(dest, labelStart, label_दीर्घता, गन्तव्यम्_ASCII, info, error_संहिता);
        }
        // Check for NFC, and for characters that are not
        // valid or deviation characters according to the normalizer.
        // If there is something wrong, then the string will change.
        // Note that the normalizer passes through non-LDH ASCII and deviation characters.
        // Deviation characters are ok in Punycode even in transitional processing.
        // In the code further below, if we find non-LDH ASCII and we have UIDNA_USE_STD3_RULES
        // then we will set UIDNA_ERROR_INVALID_ACE_LABEL there too.
        UBool isValid=uts46Norm2.isNormalized(fromPunycode, error_संहिता);
        यदि(U_FAILURE(error_संहिता)) {
            प्रत्यागम label_दीर्घता;
        }
        // Unicode 15.1 UTS #46:
        // Added an additional condition in 4.1 Validity Criteria to
        // disallow labels such as xn--xn---epa., which do not round-trip.
        // --> Validity Criteria new criterion 4:
        // If not CheckHyphens, the label must not begin with “xn--”.
        यदि(!isValid || fromPunycode.startsWith(UnicodeString::readOnlyAlias(u"xn--"))) {
            info.labelErrors|=UIDNA_ERROR_INVALID_ACE_LABEL;
            प्रत्यागम markBadACELabel(dest, labelStart, label_दीर्घता, गन्तव्यम्_ASCII, info, error_संहिता);
        }
        labelString=&fromPunycode;
        label=fromPunycode.get_बफर();
        labelStart=0;
        label_दीर्घता=fromPunycode.length();
    } अन्यथा {
        wasPunycode=असत्य;
        labelString=&dest;
    }
    // Validity check
    यदि(label_दीर्घता==0) {
        info.labelErrors|=UIDNA_ERROR_रिक्तानि_LABEL;
        प्रत्यागम replaceLabel(dest, destLabelStart, dest_Label_दीर्घता,
                            *labelString, label_दीर्घता, error_संहिता);
    }
    // labelLength>0
    यदि(label_दीर्घता>=4 && label[2]==0x2d && label[3]==0x2d) {
        // label starts with "??--"
        info.labelErrors|=UIDNA_ERROR_HYPHEN_3_4;
    }
    यदि(label[0]==0x2d) {
        // label starts with "-"
        info.labelErrors|=UIDNA_ERROR_LEADING_HYPHEN;
    }
    यदि(label[label_दीर्घता-1]==0x2d) {
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
    ध्रुव char16_t *सीमा=label+label_दीर्घता;
    char16_t oredChars=0;
    // If we enforce STD3 rules, then ASCII characters other than LDH and dot are disallowed.
    UBool disallowNonLDHDot=(options&UIDNA_USE_STD3_नियमाः)!=0;
    कुरु {
        char16_t c=*s;
        यदि(c<=0x7f) {
            यदि(c==0x2e) {
                info.labelErrors|=UIDNA_ERROR_LABEL_HAS_DOT;
                *s=0xfffd;
            } अन्यथा यदि(disallowNonLDHDot && ascii_दत्त[c]<0) {
                info.labelErrors|=UIDNA_ERROR_DISALLOWED;
                *s=0xfffd;
            }
        } अन्यथा {
            oredChars|=c;
            यदि(c==0xfffd) {
                info.labelErrors|=UIDNA_ERROR_DISALLOWED;
            }
        }
        ++s;
    } यावत्(s<सीमा);
    // Check for a leading combining mark after other validity checks
    // so that we don't report UIDNA_ERROR_DISALLOWED for the U+FFFD from here.
    UChar32 c;
    int32_t cp_दीर्घता=0;
    // "Unsafe" is ok because unpaired surrogates were mapped to U+FFFD.
    U16_अग्र_UNSAFE(label, cp_दीर्घता, c);
    यदि((U_GET_GC_आवरणम्(c)&U_GC_M_आवरणम्)!=0) {
        info.labelErrors|=UIDNA_ERROR_LEADING_COMBINING_MARK;
        labelString->replace(labelStart, cp_दीर्घता, static_cast<char16_t>(0xfffd));
        label=labelString->get_बफर()+labelStart;
        label_दीर्घता+=1-cp_दीर्घता;
        यदि(labelString==&dest) {
            dest_Label_दीर्घता=label_दीर्घता;
        }
    }
    यदि((info.labelErrors&severeErrors)==0) {
        // Do contextual checks only if we do not have U+FFFD from a severe error
        // because U+FFFD can make these checks fail.
        यदि((options&UIDNA_CHECK_BIDI)!=0 && (!info.isBiDi || info.isOkBiDi)) {
            checkLabelBiDi(label, label_दीर्घता, info);
        }
        यदि( (options&UIDNA_CHECK_CONTEXTJ)!=0 && (oredChars&0x200c)==0x200c &&
            !isLabelOkContextJ(label, label_दीर्घता)
        ) {
            info.labelErrors|=UIDNA_ERROR_CONTEXTJ;
        }
        यदि((options&UIDNA_CHECK_CONTEXTO)!=0 && oredChars>=0xb7) {
            checkLabelContextO(label, label_दीर्घता, info);
        }
        यदि(गन्तव्यम्_ASCII) {
            यदि(wasPunycode) {
                // Leave a Punycode label unchanged if it has no severe errors.
                यदि(dest_Label_दीर्घता>63) {
                    info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
                }
                प्रत्यागम dest_Label_दीर्घता;
            } अन्यथा यदि(oredChars>=0x80) {
                // Contains non-ASCII characters.
                UnicodeString punycode;
                char16_t *बफर=punycode.get_बफर(63);  // 63==maximum DNS label length
                यदि(बफर==शून्यसूचकः) {
                    error_संहिता=U_स्मृतिः_आवण्टनम्_ERROR;
                    प्रत्यागम dest_Label_दीर्घता;
                }
                बफर[0]=0x78;  // Write "xn--".
                बफर[1]=0x6e;
                बफर[2]=0x2d;
                बफर[3]=0x2d;
                UError_संहिता punycode_Error_संहिता=U_ZERO_ERROR;
                int32_t punycode_दीर्घता=u_str_गन्तव्यम्_Punycode(label, label_दीर्घता,
                                                      बफर+4, punycode.get_क्षमता()-4,
                                                      शून्यसूचकः, &punycode_Error_संहिता);
                यदि(punycode_Error_संहिता==U_बफर_अतिप्रवाहः_ERROR) {
                    punycode_Error_संहिता=U_ZERO_ERROR;
                    punycode.release_बफर(4);
                    बफर=punycode.get_बफर(4+punycode_दीर्घता);
                    यदि(बफर==शून्यसूचकः) {
                        error_संहिता=U_स्मृतिः_आवण्टनम्_ERROR;
                        प्रत्यागम dest_Label_दीर्घता;
                    }
                    punycode_दीर्घता=u_str_गन्तव्यम्_Punycode(label, label_दीर्घता,
                                                  बफर+4, punycode.get_क्षमता()-4,
                                                  शून्यसूचकः, &punycode_Error_संहिता);
                }
                punycode_दीर्घता+=4;
                punycode.release_बफर(punycode_दीर्घता);
                यदि(U_FAILURE(punycode_Error_संहिता)) {
                    error_संहिता = punycode_Error_संहिता;
                    प्रत्यागम dest_Label_दीर्घता;
                }
                यदि(punycode_दीर्घता>63) {
                    info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
                }
                प्रत्यागम replaceLabel(dest, destLabelStart, dest_Label_दीर्घता,
                                    punycode, punycode_दीर्घता, error_संहिता);
            } अन्यथा {
                // all-ASCII label
                यदि(label_दीर्घता>63) {
                    info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
                }
            }
        }
    } अन्यथा {
        // If a Punycode label has severe errors,
        // then leave it but make sure it does not look valid.
        यदि(wasPunycode) {
            info.labelErrors|=UIDNA_ERROR_INVALID_ACE_LABEL;
            प्रत्यागम markBadACELabel(dest, destLabelStart, dest_Label_दीर्घता, गन्तव्यम्_ASCII, info, error_संहिता);
        }
    }
    प्रत्यागम replaceLabel(dest, destLabelStart, dest_Label_दीर्घता,
                        *labelString, label_दीर्घता, error_संहिता);
}

// Make sure an ACE label does not look valid.
// Append U+FFFD if the label has only LDH characters.
// If UIDNA_USE_STD3_RULES, also replace disallowed ASCII characters with U+FFFD.
int32_t
UTS46::markBadACELabel(UnicodeString &dest,
                       int32_t labelStart, int32_t label_दीर्घता,
                       UBool गन्तव्यम्_ASCII, IDNAInfo &info, UError_संहिता &error_संहिता) ध्रुव {
    यदि(U_FAILURE(error_संहिता)) {
        प्रत्यागम 0;
    }
    UBool disallowNonLDHDot=(options&UIDNA_USE_STD3_नियमाः)!=0;
    UBool isASCII=सत्य;
    UBool onlyLDH=सत्य;
    ध्रुव char16_t *label=dest.get_बफर()+labelStart;
    ध्रुव char16_t *सीमा=label+label_दीर्घता;
    // Start after the initial "xn--".
    // Ok to cast away const because we own the UnicodeString.
    प्रति(char16_t *s=const_cast<char16_t *>(label+4); s<सीमा; ++s) {
        char16_t c=*s;
        यदि(c<=0x7f) {
            यदि(c==0x2e) {
                info.labelErrors|=UIDNA_ERROR_LABEL_HAS_DOT;
                *s=0xfffd;
                isASCII=onlyLDH=असत्य;
            } अन्यथा यदि(ascii_दत्त[c]<0) {
                onlyLDH=असत्य;
                यदि(disallowNonLDHDot) {
                    *s=0xfffd;
                    isASCII=असत्य;
                }
            }
        } अन्यथा {
            isASCII=onlyLDH=असत्य;
        }
    }
    यदि(onlyLDH) {
        dest.insert(labelStart + label_दीर्घता, static_cast<char16_t>(0xfffd));
        यदि(dest.isBogus()) {
            error_संहिता=U_स्मृतिः_आवण्टनम्_ERROR;
            प्रत्यागम 0;
        }
        ++label_दीर्घता;
    } अन्यथा {
        यदि(गन्तव्यम्_ASCII && isASCII && label_दीर्घता>63) {
            info.labelErrors|=UIDNA_ERROR_LABEL_TOO_LONG;
        }
    }
    प्रत्यागम label_दीर्घता;
}

ध्रुव uint32_t L_आवरणम्=U_आवरणम्(U_वामः_गन्तव्यम्_दक्षिणः);
ध्रुव uint32_t R_AL_आवरणम्=U_आवरणम्(U_दक्षिणः_गन्तव्यम्_वामः)|U_आवरणम्(U_दक्षिणः_गन्तव्यम्_वामः_ARABIC);
ध्रुव uint32_t L_R_AL_आवरणम्=L_आवरणम्|R_AL_आवरणम्;

ध्रुव uint32_t R_AL_AN_आवरणम्=R_AL_आवरणम्|U_आवरणम्(U_ARABIC_सङ्ख्या);

ध्रुव uint32_t EN_AN_आवरणम्=U_आवरणम्(U_EUROPEAN_सङ्ख्या)|U_आवरणम्(U_ARABIC_सङ्ख्या);
ध्रुव uint32_t R_AL_EN_AN_आवरणम्=R_AL_आवरणम्|EN_AN_आवरणम्;
ध्रुव uint32_t L_EN_आवरणम्=L_आवरणम्|U_आवरणम्(U_EUROPEAN_सङ्ख्या);

ध्रुव uint32_t ES_CS_ET_ON_BN_NSM_आवरणम्=
    U_आवरणम्(U_EUROPEAN_सङ्ख्या_SEPARATOR)|
    U_आवरणम्(U_COMMON_सङ्ख्या_SEPARATOR)|
    U_आवरणम्(U_EUROPEAN_सङ्ख्या_TERMINATOR)|
    U_आवरणम्(U_OTHER_NEUTRAL)|
    U_आवरणम्(U_BOUNDARY_NEUTRAL)|
    U_आवरणम्(U_DIR_NON_SPACING_MARK);
ध्रुव uint32_t L_EN_ES_CS_ET_ON_BN_NSM_आवरणम्=L_EN_आवरणम्|ES_CS_ET_ON_BN_NSM_आवरणम्;
ध्रुव uint32_t R_AL_AN_EN_ES_CS_ET_ON_BN_NSM_आवरणम्=R_AL_आवरणम्|EN_AN_आवरणम्|ES_CS_ET_ON_BN_NSM_आवरणम्;

// We scan the whole label and check both for whether it contains RTL characters
// and whether it passes the BiDi Rule.
// In a BiDi domain name, all labels must pass the BiDi Rule, but we might find
// that a domain name is a BiDi domain name (has an RTL label) only after
// processing several earlier labels.
शून्य
UTS46::checkLabelBiDi(ध्रुव char16_t *label, int32_t label_दीर्घता, IDNAInfo &info) ध्रुव {
    // IDNA2008 BiDi rule
    // Get the directionality of the first character.
    UChar32 c;
    int32_t i=0;
    U16_अग्र_UNSAFE(label, i, c);
    uint32_t first_आवरणम्=U_आवरणम्(u_charDirection(c));
    // 1. The first character must be a character with BIDI property L, R
    // or AL.  If it has the R or AL property, it is an RTL label; if it
    // has the L property, it is an LTR label.
    यदि((first_आवरणम्&~L_R_AL_आवरणम्)!=0) {
        info.isOkBiDi=असत्य;
    }
    // Get the directionality of the last non-NSM character.
    uint32_t चरमः_आवरणम्;
    प्रति(;;) {
        यदि(i>=label_दीर्घता) {
            चरमः_आवरणम्=first_आवरणम्;
            भङ्ग;
        }
        U16_पूर्व_UNSAFE(label, label_दीर्घता, c);
        UCharDirection dir=u_charDirection(c);
        यदि(dir!=U_DIR_NON_SPACING_MARK) {
            चरमः_आवरणम्=U_आवरणम्(dir);
            भङ्ग;
        }
    }
    // 3. In an RTL label, the end of the label must be a character with
    // BIDI property R, AL, EN or AN, followed by zero or more
    // characters with BIDI property NSM.
    // 6. In an LTR label, the end of the label must be a character with
    // BIDI property L or EN, followed by zero or more characters with
    // BIDI property NSM.
    यदि( (first_आवरणम्&L_आवरणम्)!=0 ?
            (चरमः_आवरणम्&~L_EN_आवरणम्)!=0 :
            (चरमः_आवरणम्&~R_AL_EN_AN_आवरणम्)!=0
    ) {
        info.isOkBiDi=असत्य;
    }
    // Add the directionalities of the intervening characters.
    uint32_t mask=first_आवरणम्|चरमः_आवरणम्;
    यावत्(i<label_दीर्घता) {
        U16_अग्र_UNSAFE(label, i, c);
        mask|=U_आवरणम्(u_charDirection(c));
    }
    यदि(first_आवरणम्&L_आवरणम्) {
        // 5. In an LTR label, only characters with the BIDI properties L, EN,
        // ES, CS, ET, ON, BN and NSM are allowed.
        यदि((mask&~L_EN_ES_CS_ET_ON_BN_NSM_आवरणम्)!=0) {
            info.isOkBiDi=असत्य;
        }
    } अन्यथा {
        // 2. In an RTL label, only characters with the BIDI properties R, AL,
        // AN, EN, ES, CS, ET, ON, BN and NSM are allowed.
        यदि((mask&~R_AL_AN_EN_ES_CS_ET_ON_BN_NSM_आवरणम्)!=0) {
            info.isOkBiDi=असत्य;
        }
        // 4. In an RTL label, if an EN is present, no AN may be present, and
        // vice versa.
        यदि((mask&EN_AN_आवरणम्)==EN_AN_आवरणम्) {
            info.isOkBiDi=असत्य;
        }
    }
    // An RTL label is a label that contains at least one character of type
    // R, AL or AN. [...]
    // A "BIDI domain name" is a domain name that contains at least one RTL
    // label. [...]
    // The following rule, consisting of six conditions, applies to labels
    // in BIDI domain names.
    यदि((mask&R_AL_AN_आवरणम्)!=0) {
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
isASCIIOkBiDi(ध्रुव char16_t *s, int32_t length) {
    int32_t labelStart=0;
    प्रति(int32_t i=0; i<length; ++i) {
        char16_t c=s[i];
        यदि(c==0x2e) {  // dot
            यदि(i>labelStart) {
                c=s[i-1];
                यदि(!(0x61<=c && c<=0x7a) && !(0x30<=c && c<=0x39)) {
                    // Last character in the label is not an L or EN.
                    प्रत्यागम असत्य;
                }
            }
            labelStart=i+1;
        } अन्यथा यदि(i==labelStart) {
            यदि(!(0x61<=c && c<=0x7a)) {
                // First character in the label is not an L.
                प्रत्यागम असत्य;
            }
        } अन्यथा {
            यदि(c<=0x20 && (c>=0x1c || (9<=c && c<=0xd))) {
                // Intermediate character in the label is a B, S or WS.
                प्रत्यागम असत्य;
            }
        }
    }
    प्रत्यागम सत्य;
}

// UTF-8 version, called for source ASCII prefix.
// Can contain uppercase A-Z.
// s[length-1] must be the trailing dot.
स्थिर UBool
isASCIIOkBiDi(ध्रुव वर्ण *s, int32_t length) {
    int32_t labelStart=0;
    प्रति(int32_t i=0; i<length; ++i) {
        वर्ण c=s[i];
        यदि(c==0x2e) {  // dot
            यदि(i>labelStart) {
                c=s[i-1];
                यदि(!(0x61<=c && c<=0x7a) && !(0x41<=c && c<=0x5a) && !(0x30<=c && c<=0x39)) {
                    // Last character in the label is not an L or EN.
                    प्रत्यागम असत्य;
                }
            }
            labelStart=i+1;
        } अन्यथा यदि(i==labelStart) {
            यदि(!(0x61<=c && c<=0x7a) && !(0x41<=c && c<=0x5a)) {
                // First character in the label is not an L.
                प्रत्यागम असत्य;
            }
        } अन्यथा {
            यदि(c<=0x20 && (c>=0x1c || (9<=c && c<=0xd))) {
                // Intermediate character in the label is a B, S or WS.
                प्रत्यागम असत्य;
            }
        }
    }
    प्रत्यागम सत्य;
}

UBool
UTS46::isLabelOkContextJ(ध्रुव char16_t *label, int32_t label_दीर्घता) ध्रुव {
    // [IDNA2008-Tables]
    // 200C..200D  ; CONTEXTJ    # ZERO WIDTH NON-JOINER..ZERO WIDTH JOINER
    प्रति(int32_t i=0; i<label_दीर्घता; ++i) {
        यदि(label[i]==0x200c) {
            // Appendix A.1. ZERO WIDTH NON-JOINER
            // Rule Set:
            //  False;
            //  If Canonical_Combining_Class(Before(cp)) .eq.  Virama Then True;
            //  If RegExpMatch((Joining_Type:{L,D})(Joining_Type:T)*\u200C
            //     (Joining_Type:T)*(Joining_Type:{R,D})) Then True;
            यदि(i==0) {
                प्रत्यागम असत्य;
            }
            UChar32 c;
            int32_t j=i;
            U16_पूर्व_UNSAFE(label, j, c);
            यदि(uts46Norm2.getCombiningClass(c)==9) {
                अनुवर्त;
            }
            // check precontext (Joining_Type:{L,D})(Joining_Type:T)*
            प्रति(;;) {
                UJoiningType type=ubidi_getJoiningType(c);
                यदि(type==U_JT_TRANSPARENT) {
                    यदि(j==0) {
                        प्रत्यागम असत्य;
                    }
                    U16_पूर्व_UNSAFE(label, j, c);
                } अन्यथा यदि(type==U_JT_वामः_JOINING || type==U_JT_DUAL_JOINING) {
                    भङ्ग;  // precontext fulfilled
                } अन्यथा {
                    प्रत्यागम असत्य;
                }
            }
            // check postcontext (Joining_Type:T)*(Joining_Type:{R,D})
            प्रति(j=i+1;;) {
                यदि(j==label_दीर्घता) {
                    प्रत्यागम असत्य;
                }
                U16_अग्र_UNSAFE(label, j, c);
                UJoiningType type=ubidi_getJoiningType(c);
                यदि(type==U_JT_TRANSPARENT) {
                    // just skip this character
                } अन्यथा यदि(type==U_JT_दक्षिणः_JOINING || type==U_JT_DUAL_JOINING) {
                    भङ्ग;  // postcontext fulfilled
                } अन्यथा {
                    प्रत्यागम असत्य;
                }
            }
        } अन्यथा यदि(label[i]==0x200d) {
            // Appendix A.2. ZERO WIDTH JOINER (U+200D)
            // Rule Set:
            //  False;
            //  If Canonical_Combining_Class(Before(cp)) .eq.  Virama Then True;
            यदि(i==0) {
                प्रत्यागम असत्य;
            }
            UChar32 c;
            int32_t j=i;
            U16_पूर्व_UNSAFE(label, j, c);
            यदि(uts46Norm2.getCombiningClass(c)!=9) {
                प्रत्यागम असत्य;
            }
        }
    }
    प्रत्यागम सत्य;
}

शून्य
UTS46::checkLabelContextO(ध्रुव char16_t *label, int32_t label_दीर्घता, IDNAInfo &info) ध्रुव {
    int32_t label_अन्तः=label_दीर्घता-1;  // inclusive
    int32_t arabicDigits=0;  // -1 for 066x, +1 for 06Fx
    प्रति(int32_t i=0; i<=label_अन्तः; ++i) {
        UChar32 c=label[i];
        यदि(c<0xb7) {
            // ASCII fastpath
        } अन्यथा यदि(c<=0x6f9) {
            यदि(c==0xb7) {
                // Appendix A.3. MIDDLE DOT (U+00B7)
                // Rule Set:
                //  False;
                //  If Before(cp) .eq.  U+006C And
                //     After(cp) .eq.  U+006C Then True;
                यदि(!(0<i && label[i-1]==0x6c &&
                     i<label_अन्तः && label[i+1]==0x6c)) {
                    info.labelErrors|=UIDNA_ERROR_CONTEXTO_PUNCTUATION;
                }
            } अन्यथा यदि(c==0x375) {
                // Appendix A.4. GREEK LOWER NUMERAL SIGN (KERAIA) (U+0375)
                // Rule Set:
                //  False;
                //  If Script(After(cp)) .eq.  Greek Then True;
                UScript_संहिता script=USCRIPT_INVALID_संहिता;
                यदि(i<label_अन्तः) {
                    UError_संहिता error_संहिता=U_ZERO_ERROR;
                    int32_t j=i+1;
                    U16_अग्र(label, j, label_दीर्घता, c);
                    script=uscript_getScript(c, &error_संहिता);
                }
                यदि(script!=USCRIPT_GREEK) {
                    info.labelErrors|=UIDNA_ERROR_CONTEXTO_PUNCTUATION;
                }
            } अन्यथा यदि(c==0x5f3 || c==0x5f4) {
                // Appendix A.5. HEBREW PUNCTUATION GERESH (U+05F3)
                // Rule Set:
                //  False;
                //  If Script(Before(cp)) .eq.  Hebrew Then True;
                //
                // Appendix A.6. HEBREW PUNCTUATION GERSHAYIM (U+05F4)
                // Rule Set:
                //  False;
                //  If Script(Before(cp)) .eq.  Hebrew Then True;
                UScript_संहिता script=USCRIPT_INVALID_संहिता;
                यदि(0<i) {
                    UError_संहिता error_संहिता=U_ZERO_ERROR;
                    int32_t j=i;
                    U16_पूर्व(label, 0, j, c);
                    script=uscript_getScript(c, &error_संहिता);
                }
                यदि(script!=USCRIPT_HEBREW) {
                    info.labelErrors|=UIDNA_ERROR_CONTEXTO_PUNCTUATION;
                }
            } अन्यथा यदि(0x660<=c /* && c<=0x6f9 */) {
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
                यदि(c<=0x669) {
                    यदि(arabicDigits>0) {
                        info.labelErrors|=UIDNA_ERROR_CONTEXTO_DIGITS;
                    }
                    arabicDigits=-1;
                } अन्यथा यदि(0x6f0<=c) {
                    यदि(arabicDigits<0) {
                        info.labelErrors|=UIDNA_ERROR_CONTEXTO_DIGITS;
                    }
                    arabicDigits=1;
                }
            }
        } अन्यथा यदि(c==0x30fb) {
            // Appendix A.7. KATAKANA MIDDLE DOT (U+30FB)
            // Rule Set:
            //  False;
            //  For All Characters:
            //    If Script(cp) .in. {Hiragana, Katakana, Han} Then True;
            //  End For;
            UError_संहिता error_संहिता=U_ZERO_ERROR;
            प्रति(पूर्णाङ्क j=0;;) {
                यदि(j>label_अन्तः) {
                    info.labelErrors|=UIDNA_ERROR_CONTEXTO_PUNCTUATION;
                    भङ्ग;
                }
                U16_अग्र(label, j, label_दीर्घता, c);
                UScript_संहिता script=uscript_getScript(c, &error_संहिता);
                यदि(script==USCRIPT_HIRAGANA || script==USCRIPT_KATAKANA || script==USCRIPT_HAN) {
                    भङ्ग;
                }
            }
        }
    }
}

U_NAMESPACE_अन्तः

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

U_CAPI UIDNA * U_EXPORT2
uidna_openUTS46(uint32_t options, UError_संहिता *p_Error_संहिता) {
    प्रत्यागम reinterpret_cast<UIDNA *>(IDNA::createUTS46Instance(options, *p_Error_संहिता));
}

U_CAPI शून्य U_EXPORT2
uidna_close(UIDNA *idna) {
    विलोपय reinterpret_cast<IDNA *>(idna);
}

स्थिर UBool
check_तर्काः(ध्रुव शून्य *label, int32_t length,
          शून्य *dest, int32_t capacity,
          UIDNAInfo *pInfo, UError_संहिता *p_Error_संहिता) {
    यदि(U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम असत्य;
    }
    // sizeof(UIDNAInfo)=16 in the first API version.
    यदि(pInfo==शून्यसूचकः || pInfo->size<16) {
        *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम असत्य;
    }
    यदि( (label==शून्यसूचकः ? length!=0 : length<-1) ||
        (dest==शून्यसूचकः ? capacity!=0 : capacity<0) ||
        (dest==label && label!=शून्यसूचकः)
    ) {
        *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम असत्य;
    }
    // Set all *pInfo bytes to 0 except for the size field itself.
    uprv_memset(&pInfo->size+1, 0, pInfo->size-परिमाण(pInfo->size));
    प्रत्यागम सत्य;
}

स्थिर शून्य
idna_Info_गन्तव्यम्_Struct(IDNAInfo &info, UIDNAInfo *pInfo) {
    pInfo->isTransitionalDifferent=info.isTransitionalDifferent();
    pInfo->errors=info.getErrors();
}

U_CAPI int32_t U_EXPORT2
uidna_label_गन्तव्यम्_ASCII(ध्रुव UIDNA *idna,
                   ध्रुव char16_t *label, int32_t length,
                   char16_t *dest, int32_t capacity,
                   UIDNAInfo *pInfo, UError_संहिता *p_Error_संहिता) {
    यदि(!check_तर्काः(label, length, dest, capacity, pInfo, p_Error_संहिता)) {
        प्रत्यागम 0;
    }
    UnicodeString src(length < 0, label, length);
    UnicodeString destString(dest, 0, capacity);
    IDNAInfo info;
    reinterpret_cast<ध्रुव IDNA *>(idna)->label_गन्तव्यम्_ASCII(src, destString, info, *p_Error_संहिता);
    idna_Info_गन्तव्यम्_Struct(info, pInfo);
    प्रत्यागम destString.extract(dest, capacity, *p_Error_संहिता);
}

U_CAPI int32_t U_EXPORT2
uidna_label_गन्तव्यम्_Unicode(ध्रुव UIDNA *idna,
                     ध्रुव char16_t *label, int32_t length,
                     char16_t *dest, int32_t capacity,
                     UIDNAInfo *pInfo, UError_संहिता *p_Error_संहिता) {
    यदि(!check_तर्काः(label, length, dest, capacity, pInfo, p_Error_संहिता)) {
        प्रत्यागम 0;
    }
    UnicodeString src(length < 0, label, length);
    UnicodeString destString(dest, 0, capacity);
    IDNAInfo info;
    reinterpret_cast<ध्रुव IDNA *>(idna)->label_गन्तव्यम्_Unicode(src, destString, info, *p_Error_संहिता);
    idna_Info_गन्तव्यम्_Struct(info, pInfo);
    प्रत्यागम destString.extract(dest, capacity, *p_Error_संहिता);
}

U_CAPI int32_t U_EXPORT2
uidna_name_गन्तव्यम्_ASCII(ध्रुव UIDNA *idna,
                  ध्रुव char16_t *name, int32_t length,
                  char16_t *dest, int32_t capacity,
                  UIDNAInfo *pInfo, UError_संहिता *p_Error_संहिता) {
    यदि(!check_तर्काः(name, length, dest, capacity, pInfo, p_Error_संहिता)) {
        प्रत्यागम 0;
    }
    UnicodeString src(length < 0, name, length);
    UnicodeString destString(dest, 0, capacity);
    IDNAInfo info;
    reinterpret_cast<ध्रुव IDNA *>(idna)->name_गन्तव्यम्_ASCII(src, destString, info, *p_Error_संहिता);
    idna_Info_गन्तव्यम्_Struct(info, pInfo);
    प्रत्यागम destString.extract(dest, capacity, *p_Error_संहिता);
}

U_CAPI int32_t U_EXPORT2
uidna_name_गन्तव्यम्_Unicode(ध्रुव UIDNA *idna,
                    ध्रुव char16_t *name, int32_t length,
                    char16_t *dest, int32_t capacity,
                    UIDNAInfo *pInfo, UError_संहिता *p_Error_संहिता) {
    यदि(!check_तर्काः(name, length, dest, capacity, pInfo, p_Error_संहिता)) {
        प्रत्यागम 0;
    }
    UnicodeString src(length < 0, name, length);
    UnicodeString destString(dest, 0, capacity);
    IDNAInfo info;
    reinterpret_cast<ध्रुव IDNA *>(idna)->name_गन्तव्यम्_Unicode(src, destString, info, *p_Error_संहिता);
    idna_Info_गन्तव्यम्_Struct(info, pInfo);
    प्रत्यागम destString.extract(dest, capacity, *p_Error_संहिता);
}

U_CAPI int32_t U_EXPORT2
uidna_label_गन्तव्यम्_ASCII_UTF8(ध्रुव UIDNA *idna,
                        ध्रुव वर्ण *label, int32_t length,
                        वर्ण *dest, int32_t capacity,
                        UIDNAInfo *pInfo, UError_संहिता *p_Error_संहिता) {
    यदि(!check_तर्काः(label, length, dest, capacity, pInfo, p_Error_संहिता)) {
        प्रत्यागम 0;
    }
    StringPiece src(label, length<0 ? static_cast<int32_t>(uprv_strlen(label)) : length);
    प्रत्यागम ByteSinkUtil::द्वारा_Byte_Sink_गन्तव्यम्_Terminated_Chars(
        dest, capacity,
        [&](ByteSink& sink, UError_संहिता& स्थितिः) {
            IDNAInfo info;
            reinterpret_cast<ध्रुव IDNA *>(idna)->label_गन्तव्यम्_ASCII_UTF8(src, sink, info, स्थितिः);
            idna_Info_गन्तव्यम्_Struct(info, pInfo);
        },
        *p_Error_संहिता);
}

U_CAPI int32_t U_EXPORT2
uidna_label_गन्तव्यम्_Unicode_UTF8(ध्रुव UIDNA *idna,
                         ध्रुव वर्ण *label, int32_t length,
                         वर्ण *dest, int32_t capacity,
                         UIDNAInfo *pInfo, UError_संहिता *p_Error_संहिता) {
    यदि(!check_तर्काः(label, length, dest, capacity, pInfo, p_Error_संहिता)) {
        प्रत्यागम 0;
    }
    StringPiece src(label, length<0 ? static_cast<int32_t>(uprv_strlen(label)) : length);
    प्रत्यागम ByteSinkUtil::द्वारा_Byte_Sink_गन्तव्यम्_Terminated_Chars(
        dest, capacity,
        [&](ByteSink& sink, UError_संहिता& स्थितिः) {
            IDNAInfo info;
            reinterpret_cast<ध्रुव IDNA *>(idna)->label_गन्तव्यम्_Unicode_UTF8(src, sink, info, स्थितिः);
            idna_Info_गन्तव्यम्_Struct(info, pInfo);
        },
        *p_Error_संहिता);
}

U_CAPI int32_t U_EXPORT2
uidna_name_गन्तव्यम्_ASCII_UTF8(ध्रुव UIDNA *idna,
                       ध्रुव वर्ण *name, int32_t length,
                       वर्ण *dest, int32_t capacity,
                       UIDNAInfo *pInfo, UError_संहिता *p_Error_संहिता) {
    यदि(!check_तर्काः(name, length, dest, capacity, pInfo, p_Error_संहिता)) {
        प्रत्यागम 0;
    }
    StringPiece src(name, length<0 ? static_cast<int32_t>(uprv_strlen(name)) : length);
    प्रत्यागम ByteSinkUtil::द्वारा_Byte_Sink_गन्तव्यम्_Terminated_Chars(
        dest, capacity,
        [&](ByteSink& sink, UError_संहिता& स्थितिः) {
            IDNAInfo info;
            reinterpret_cast<ध्रुव IDNA *>(idna)->name_गन्तव्यम्_ASCII_UTF8(src, sink, info, स्थितिः);
            idna_Info_गन्तव्यम्_Struct(info, pInfo);
        },
        *p_Error_संहिता);
}

U_CAPI int32_t U_EXPORT2
uidna_name_गन्तव्यम्_Unicode_UTF8(ध्रुव UIDNA *idna,
                        ध्रुव वर्ण *name, int32_t length,
                        वर्ण *dest, int32_t capacity,
                        UIDNAInfo *pInfo, UError_संहिता *p_Error_संहिता) {
    यदि(!check_तर्काः(name, length, dest, capacity, pInfo, p_Error_संहिता)) {
        प्रत्यागम 0;
    }
    StringPiece src(name, length<0 ? static_cast<int32_t>(uprv_strlen(name)) : length);
    प्रत्यागम ByteSinkUtil::द्वारा_Byte_Sink_गन्तव्यम्_Terminated_Chars(
        dest, capacity,
        [&](ByteSink& sink, UError_संहिता& स्थितिः) {
            IDNAInfo info;
            reinterpret_cast<ध्रुव IDNA *>(idna)->name_गन्तव्यम्_Unicode_UTF8(src, sink, info, स्थितिः);
            idna_Info_गन्तव्यम्_Struct(info, pInfo);
        },
        *p_Error_संहिता);
}

#यदि_अन्तः  // UCONFIG_NO_IDNA
