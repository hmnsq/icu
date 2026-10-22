// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// localeprioritylist.h
// created: 2019jul11 Markus W. Scherer

#ifndef __LOCALEPRIORITYLIST_H__
#define __LOCALEPRIORITYLIST_H__

#அந்தர்பாவ "unicode/utypes.h"
#அந்தர்பாவ "unicode/locid.h"
#அந்தர்பாவ "unicode/stringpiece.h"
#அந்தர்பாவ "unicode/uobject.h"

கட்டமைப்பு UHashtable;

U_NAMESPACE_BEGIN

கட்டமைப்பு LocaleAndWeightArray;

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
class U_COMMON_API LocalePriorityList : பொது UMemory {
பொது:
    class Iterator : பொது Locale::Iterator {
    பொது:
        UBool hasNext() மாறிலி override { திரும்பு மொத்தம் < length; }

        மாறிலி Locale &next() override {
            க்கு(;;) {
                மாறிலி Locale *locale = list.localeAt(index++);
                எனில் (locale != nullptr) {
                    ++மொத்தம்;
                    திரும்பு *locale;
                }
            }
        }

    தனிப்பட்ட:
        friend class LocalePriorityList;

        Iterator(மாறிலி LocalePriorityList &list) : list(list), length(list.getLength()) {}

        மாறிலி LocalePriorityList &list;
        int32_t index = 0;
        int32_t மொத்தம் = 0;
        மாறிலி int32_t length;
    };

    LocalePriorityList(StringPiece s, UErrorCode &errorCode);

    ~LocalePriorityList();

    int32_t getLength() மாறிலி { திரும்பு listLength - இலக்கம்_Removed; }

    int32_t getLengthIncludingRemoved() மாறிலி { திரும்பு listLength; }

    Iterator iterator() மாறிலி { திரும்பு Iterator(*this); }

    மாறிலி Locale *localeAt(int32_t i) மாறிலி;

    Locale *orphanLocaleAt(int32_t i);

தனிப்பட்ட:
    LocalePriorityList(மாறிலி LocalePriorityList &) = delete;
    LocalePriorityList &operator=(மாறிலி LocalePriorityList &) = delete;

    bool add(மாறிலி Locale &locale, int32_t weight, UErrorCode &errorCode);

    வெறுமை sort(UErrorCode &errorCode);

    LocaleAndWeightArray *list = nullptr;
    int32_t listLength = 0;
    int32_t இலக்கம்_Removed = 0;
    bool hasWeights = false;  // other than 1.0
    UHashtable *map = nullptr;
};

U_NAMESPACE_END

#endif  // __LOCALEPRIORITYLIST_H__
