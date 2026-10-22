// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// localeprioritylist.h
// created: 2019jul11 Markus W. Scherer

#ifndef __LOCALEPRIORITYLIST_H__
#define __LOCALEPRIORITYLIST_H__

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/locid.h"
#ಅನ್ತರ್ಭಾವ "unicode/stringpiece.h"
#ಅನ್ತರ್ಭಾವ "unicode/uobject.h"

ಸಂರಚನಾ UHashtable;

U_NAMESPACE_BEGIN

ಸಂರಚನಾ LocaleAndWeightArray;

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
class U_COMMON_API LocalePriorityList : ಸಾರ್ವಜನಿಕ UMemory {
ಸಾರ್ವಜನಿಕ:
    class Iterator : ಸಾರ್ವಜನಿಕ Locale::Iterator {
    ಸಾರ್ವಜನಿಕ:
        UBool hasNext() ಧ್ರುವ override { ಪ್ರತ್ಯಾಗಮ ಎಣಿಕೆ < length; }

        ಧ್ರುವ Locale &next() override {
            ಪ್ರತಿ(;;) {
                ಧ್ರುವ Locale *locale = list.localeAt(index++);
                ಯದಿ (locale != nullptr) {
                    ++ಎಣಿಕೆ;
                    ಪ್ರತ್ಯಾಗಮ *locale;
                }
            }
        }

    ಗುಪ್ತ:
        friend class LocalePriorityList;

        Iterator(ಧ್ರುವ LocalePriorityList &list) : list(list), length(list.getLength()) {}

        ಧ್ರುವ LocalePriorityList &list;
        int32_t index = 0;
        int32_t ಎಣಿಕೆ = 0;
        ಧ್ರುವ int32_t length;
    };

    LocalePriorityList(StringPiece s, UErrorCode &errorCode);

    ~LocalePriorityList();

    int32_t getLength() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ listLength - ಅಂಕೆ_Removed; }

    int32_t getLengthIncludingRemoved() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ listLength; }

    Iterator iterator() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ Iterator(*this); }

    ಧ್ರುವ Locale *localeAt(int32_t i) ಧ್ರುವ;

    Locale *orphanLocaleAt(int32_t i);

ಗುಪ್ತ:
    LocalePriorityList(ಧ್ರುವ LocalePriorityList &) = delete;
    LocalePriorityList &operator=(ಧ್ರುವ LocalePriorityList &) = delete;

    bool add(ಧ್ರುವ Locale &locale, int32_t weight, UErrorCode &errorCode);

    ಶೂನ್ಯ sort(UErrorCode &errorCode);

    LocaleAndWeightArray *list = nullptr;
    int32_t listLength = 0;
    int32_t ಅಂಕೆ_Removed = 0;
    bool hasWeights = false;  // other than 1.0
    UHashtable *map = nullptr;
};

U_NAMESPACE_END

#endif  // __LOCALEPRIORITYLIST_H__
