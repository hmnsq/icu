// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// localeprioritylist.h
// created: 2019jul11 Markus W. Scherer

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __LOCALEPRIORITYLIST_H__
#ವ್ಯಾಖ್ಯಾನಿಸು __LOCALEPRIORITYLIST_H__

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/locid.h"
#ಅನ್ತರ್ಭಾವ "unicode/stringpiece.h"
#ಅನ್ತರ್ಭಾವ "unicode/uobject.h"

ರಚನೆ UHashtable;

U_NAMESPACE_BEGIN

ರಚನೆ Locale_And_ತೂಕ_Array;

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
class U_COMMON_API Locale_ಆದ್ಯತೆ_List : ಸಾರ್ವಜನಿಕ UMemory {
ಸಾರ್ವಜನಿಕ:
    class Iterator : ಸಾರ್ವಜನಿಕ Locale::Iterator {
    ಸಾರ್ವಜನಿಕ:
        UBool has_ಮುಂದಿನ() ಸ್ಥಿರಾಂಕ override { ಹಿಂತಿರುಗಿ ಎಣಿಕೆ_2 < ಉದ್ದಳತೆ; }

        ಸ್ಥಿರಾಂಕ Locale &ಮುಂದಿನ() override {
            ಗಾಗಿ(;;) {
                ಸ್ಥಿರಾಂಕ Locale *locale = list.localeAt(ಸೂಚ್ಯಂಕ++);
                ಆದರೆ (locale != nullptr) {
                    ++ಎಣಿಕೆ_2;
                    ಹಿಂತಿರುಗಿ *locale;
                }
            }
        }

    ಖಾಸಗಿ:
        ಮಿತ್ರ class Locale_ಆದ್ಯತೆ_List;

        Iterator(ಸ್ಥಿರಾಂಕ Locale_ಆದ್ಯತೆ_List &list) : list(list), ಉದ್ದಳತೆ(list.get_ಉದ್ದಳತೆ()) {}

        ಸ್ಥಿರಾಂಕ Locale_ಆದ್ಯತೆ_List &list;
        int32_t ಸೂಚ್ಯಂಕ = 0;
        int32_t ಎಣಿಕೆ_2 = 0;
        ಸ್ಥಿರಾಂಕ int32_t ಉದ್ದಳತೆ;
    };

    Locale_ಆದ್ಯತೆ_List(StringPiece s, UError_ಸಂಕೇತ &error_ಸಂಕೇತ);

    ~Locale_ಆದ್ಯತೆ_List();

    int32_t get_ಉದ್ದಳತೆ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ list_ಉದ್ದಳತೆ - ಅಂಕೆ_Removed; }

    int32_t get_ಉದ್ದಳತೆ_Including_Removed() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ list_ಉದ್ದಳತೆ; }

    Iterator iterator() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ Iterator(*this); }

    ಸ್ಥಿರಾಂಕ Locale *localeAt(int32_t i) ಸ್ಥಿರಾಂಕ;

    Locale *orphanLocaleAt(int32_t i);

ಖಾಸಗಿ:
    Locale_ಆದ್ಯತೆ_List(ಸ್ಥಿರಾಂಕ Locale_ಆದ್ಯತೆ_List &) = delete;
    Locale_ಆದ್ಯತೆ_List &ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ Locale_ಆದ್ಯತೆ_List &) = delete;

    bool ಕೂಡಿಸು(ಸ್ಥಿರಾಂಕ Locale &locale, int32_t ತೂಕ, UError_ಸಂಕೇತ &error_ಸಂಕೇತ);

    ಶೂನ್ಯ ವಿಂಗಡಿಸು(UError_ಸಂಕೇತ &error_ಸಂಕೇತ);

    Locale_And_ತೂಕ_Array *list = nullptr;
    int32_t list_ಉದ್ದಳತೆ = 0;
    int32_t ಅಂಕೆ_Removed = 0;
    bool hasWeights = false;  // other than 1.0
    UHashtable *map = nullptr;
};

U_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ  // __LOCALEPRIORITYLIST_H__
