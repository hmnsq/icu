// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// localeprioritylist.h
// created: 2019jul11 Markus W. Scherer

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __LOCALEPRIORITYLIST_H__
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __LOCALEPRIORITYLIST_H__

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/locid.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/stringpiece.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uobject.h"

ਬਣਤਰ UHashtable;

U_NAMESPACE_BEGIN

ਬਣਤਰ Locale_And_ਭਾਰ_Array;

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
class U_COMMON_API Locale_ਤਰਜੀਹ_List : ਜਨਤਕ UMemory {
ਜਨਤਕ:
    class Iterator : ਜਨਤਕ Locale::Iterator {
    ਜਨਤਕ:
        UBool has_ਅਗਲਾ() ਅਚਲ override { ਵਾਪਸ ਗਿਣਤੀ_2 < ਲੰਮਾਈ; }

        ਅਚਲ Locale &ਅਗਲਾ() override {
            ਲਈ(;;) {
                ਅਚਲ Locale *locale = list.localeAt(ਸੂਚਕਾਂਕ++);
                ਜੇ (locale != nullptr) {
                    ++ਗਿਣਤੀ_2;
                    ਵਾਪਸ *locale;
                }
            }
        }

    ਨਿਜੀ:
        ਮਿੱਤਰ class Locale_ਤਰਜੀਹ_List;

        Iterator(ਅਚਲ Locale_ਤਰਜੀਹ_List &list) : list(list), ਲੰਮਾਈ(list.get_ਲੰਮਾਈ()) {}

        ਅਚਲ Locale_ਤਰਜੀਹ_List &list;
        int32_t ਸੂਚਕਾਂਕ = 0;
        int32_t ਗਿਣਤੀ_2 = 0;
        ਅਚਲ int32_t ਲੰਮਾਈ;
    };

    Locale_ਤਰਜੀਹ_List(StringPiece s, UError_ਕੋਡ &error_ਕੋਡ);

    ~Locale_ਤਰਜੀਹ_List();

    int32_t get_ਲੰਮਾਈ() ਅਚਲ { ਵਾਪਸ list_ਲੰਮਾਈ - ਅੰਕ_Removed; }

    int32_t get_ਲੰਮਾਈ_Including_Removed() ਅਚਲ { ਵਾਪਸ list_ਲੰਮਾਈ; }

    Iterator iterator() ਅਚਲ { ਵਾਪਸ Iterator(*this); }

    ਅਚਲ Locale *localeAt(int32_t i) ਅਚਲ;

    Locale *orphanLocaleAt(int32_t i);

ਨਿਜੀ:
    Locale_ਤਰਜੀਹ_List(ਅਚਲ Locale_ਤਰਜੀਹ_List &) = delete;
    Locale_ਤਰਜੀਹ_List &ਸੰਚਾਲਕ=(ਅਚਲ Locale_ਤਰਜੀਹ_List &) = delete;

    bool ਜੋੜੋ(ਅਚਲ Locale &locale, int32_t ਭਾਰ, UError_ਕੋਡ &error_ਕੋਡ);

    ਖਾਲੀ ਛਾਂਟੋ(UError_ਕੋਡ &error_ਕੋਡ);

    Locale_And_ਭਾਰ_Array *list = nullptr;
    int32_t list_ਲੰਮਾਈ = 0;
    int32_t ਅੰਕ_Removed = 0;
    bool hasWeights = false;  // other than 1.0
    UHashtable *map = nullptr;
};

U_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ  // __LOCALEPRIORITYLIST_H__
