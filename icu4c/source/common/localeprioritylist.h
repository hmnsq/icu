// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// localeprioritylist.h
// created: 2019jul11 Markus W. Scherer

#यदि_अपरिभाषितम् __LOCALEPRIORITYLIST_H__
#परिभाषय __LOCALEPRIORITYLIST_H__

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/locid.h"
#अन्तर्भाव "unicode/stringpiece.h"
#अन्तर्भाव "unicode/uobject.h"

संरचना UHashtable;

U_NAMESPACE_BEGIN

संरचना LocaleAndWeightArray;

/**
 * Parses a list of locales from an accept-language string.
 * We are a bit more lenient than the spec:
 * We accept extra whitespace in more places, empty range fields,
 * and any number of qvalue fraction digits.
 *
 * https://tools.ietf.org/html/rfc2616#section-14.4
 * 14.4 Accept-Language
 *
 *        Accept-Language = "Accept-Language" ":"
 *                          1#( language-range [ ";" "q" "=" qvalue ] )
 *        language-range  = ( ( 1*8ALPHA *( "-" 1*8ALPHA ) ) | "*" )
 *
 *    Each language-range MAY be given an associated quality value which
 *    represents an estimate of the user's preference for the languages
 *    specified by that range. The quality value defaults to "q=1". For
 *    example,
 *
 *        Accept-Language: da, en-gb;q=0.8, en;q=0.7
 *
 * https://tools.ietf.org/html/rfc2616#section-3.9
 * 3.9 Quality Values
 *
 *    HTTP content negotiation (section 12) uses short "floating point"
 *    numbers to indicate the relative importance ("weight") of various
 *    negotiable parameters.  A weight is normalized to a real number in
 *    the range 0 through 1, where 0 is the minimum and 1 the maximum
 *    value. If a parameter has a quality value of 0, then content with
 *    this parameter is `not acceptable' for the client. HTTP/1.1
 *    applications MUST NOT generate more than three digits after the
 *    decimal point. User configuration of these values SHOULD also be
 *    limited in this fashion.
 *
 *        qvalue         = ( "0" [ "." 0*3DIGIT ] )
 *                       | ( "1" [ "." 0*3("0") ] )
 */
class U_COMMON_API Locale_प्राथमिकता_List : सार्वजनिक UMemory {
सार्वजनिक:
    class Iterator : सार्वजनिक Locale::Iterator {
    सार्वजनिक:
        UBool has_अग्र() ध्रुव override { प्रत्यागम गणनम् < दीर्घता; }

        ध्रुव Locale &अग्र() override {
            प्रति(;;) {
                ध्रुव Locale *locale = list.localeAt(सूचकांक++);
                यदि (locale != nullptr) {
                    ++गणनम्;
                    प्रत्यागम *locale;
                }
            }
        }

    गुप्त:
        मित्रम् class Locale_प्राथमिकता_List;

        Iterator(ध्रुव Locale_प्राथमिकता_List &list) : list(list), दीर्घता(list.get_दीर्घता()) {}

        ध्रुव Locale_प्राथमिकता_List &list;
        int32_t सूचकांक = 0;
        int32_t गणनम् = 0;
        ध्रुव int32_t दीर्घता;
    };

    Locale_प्राथमिकता_List(StringPiece s, UError_संहिता &error_संहिता);

    ~Locale_प्राथमिकता_List();

    int32_t get_दीर्घता() ध्रुव { प्रत्यागम list_दीर्घता - अङ्कः_Removed; }

    int32_t get_दीर्घता_Including_Removed() ध्रुव { प्रत्यागम list_दीर्घता; }

    Iterator iterator() ध्रुव { प्रत्यागम Iterator(*this); }

    ध्रुव Locale *localeAt(int32_t i) ध्रुव;

    Locale *orphanLocaleAt(int32_t i);

गुप्त:
    Locale_प्राथमिकता_List(ध्रुव Locale_प्राथमिकता_List &) = delete;
    Locale_प्राथमिकता_List &संक्रियकः=(ध्रुव Locale_प्राथमिकता_List &) = delete;

    bool सङ्कलय(ध्रुव Locale &locale, int32_t weight, UError_संहिता &error_संहिता);

    शून्य क्रमय(UError_संहिता &error_संहिता);

    LocaleAndWeightArray *list = nullptr;
    int32_t list_दीर्घता = 0;
    int32_t अङ्कः_Removed = 0;
    bool hasWeights = false;  // other than 1.0
    UHashtable *map = nullptr;
};

U_NAMESPACE_अन्तः

#यदि_अन्तः  // __LOCALEPRIORITYLIST_H__
