// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// localeprioritylist.h
// created: 2019jul11 Markus W. Scherer

#ifndef __LOCALEPRIORITYLIST_H__
#define __LOCALEPRIORITYLIST_H__

#അന്തര്ഭാവ "unicode/utypes.h"
#അന്തര്ഭാവ "unicode/locid.h"
#അന്തര്ഭാവ "unicode/stringpiece.h"
#അന്തര്ഭാവ "unicode/uobject.h"

ഘടന UHashtable;

U_NAMESPACE_BEGIN

ഘടന LocaleAndWeightArray;

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
class U_COMMON_API LocalePriorityList : പൊതു UMemory {
പൊതു:
    class Iterator : പൊതു Locale::Iterator {
    പൊതു:
        UBool hasNext() സ്ഥിരം override { തിരികെ എണ്ണം < length; }

        സ്ഥിരം Locale &next() override {
            വേണ്ടി(;;) {
                സ്ഥിരം Locale *locale = list.localeAt(index++);
                എങ്കിൽ (locale != nullptr) {
                    ++എണ്ണം;
                    തിരികെ *locale;
                }
            }
        }

    സ്വകാര്യ:
        friend class LocalePriorityList;

        Iterator(സ്ഥിരം LocalePriorityList &list) : list(list), length(list.getLength()) {}

        സ്ഥിരം LocalePriorityList &list;
        int32_t index = 0;
        int32_t എണ്ണം = 0;
        സ്ഥിരം int32_t length;
    };

    LocalePriorityList(StringPiece s, UErrorCode &errorCode);

    ~LocalePriorityList();

    int32_t getLength() സ്ഥിരം { തിരികെ listLength - അക്കം_Removed; }

    int32_t getLengthIncludingRemoved() സ്ഥിരം { തിരികെ listLength; }

    Iterator iterator() സ്ഥിരം { തിരികെ Iterator(*this); }

    സ്ഥിരം Locale *localeAt(int32_t i) സ്ഥിരം;

    Locale *orphanLocaleAt(int32_t i);

സ്വകാര്യ:
    LocalePriorityList(സ്ഥിരം LocalePriorityList &) = delete;
    LocalePriorityList &operator=(സ്ഥിരം LocalePriorityList &) = delete;

    bool add(സ്ഥിരം Locale &locale, int32_t weight, UErrorCode &errorCode);

    ശൂന്യം sort(UErrorCode &errorCode);

    LocaleAndWeightArray *list = nullptr;
    int32_t listLength = 0;
    int32_t അക്കം_Removed = 0;
    bool hasWeights = false;  // other than 1.0
    UHashtable *map = nullptr;
};

U_NAMESPACE_END

#endif  // __LOCALEPRIORITYLIST_H__
