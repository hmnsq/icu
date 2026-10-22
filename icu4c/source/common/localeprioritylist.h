// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// localeprioritylist.h
// created: 2019jul11 Markus W. Scherer

#ifndef __LOCALEPRIORITYLIST_H__
#define __LOCALEPRIORITYLIST_H__

#অন্তর্ভাব "unicode/utypes.h"
#অন্তর্ভাব "unicode/locid.h"
#অন্তর্ভাব "unicode/stringpiece.h"
#অন্তর্ভাব "unicode/uobject.h"

কাঠামো UHashtable;

U_NAMESPACE_BEGIN

কাঠামো LocaleAndWeightArray;

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
class U_COMMON_API LocalePriorityList : সর্বজনীন UMemory {
সর্বজনীন:
    class Iterator : সর্বজনীন Locale::Iterator {
    সর্বজনীন:
        UBool has_পরবর্তী() ধ্রুবক override { ফেরত গুনতি < দৈর্ঘ্য; }

        ধ্রুবক Locale &পরবর্তী() override {
            জন্য(;;) {
                ধ্রুবক Locale *locale = list.localeAt(সূচক++);
                যদি (locale != nullptr) {
                    ++গুনতি;
                    ফেরত *locale;
                }
            }
        }

    ব্যক্তিগত:
        friend class LocalePriorityList;

        Iterator(ধ্রুবক LocalePriorityList &list) : list(list), দৈর্ঘ্য(list.get_দৈর্ঘ্য()) {}

        ধ্রুবক LocalePriorityList &list;
        int32_t সূচক = 0;
        int32_t গুনতি = 0;
        ধ্রুবক int32_t দৈর্ঘ্য;
    };

    LocalePriorityList(StringPiece s, UErrorCode &errorCode);

    ~LocalePriorityList();

    int32_t get_দৈর্ঘ্য() ধ্রুবক { ফেরত list_দৈর্ঘ্য - অঙ্ক_Removed; }

    int32_t get_দৈর্ঘ্য_Including_Removed() ধ্রুবক { ফেরত list_দৈর্ঘ্য; }

    Iterator iterator() ধ্রুবক { ফেরত Iterator(*this); }

    ধ্রুবক Locale *localeAt(int32_t i) ধ্রুবক;

    Locale *orphanLocaleAt(int32_t i);

ব্যক্তিগত:
    LocalePriorityList(ধ্রুবক LocalePriorityList &) = delete;
    LocalePriorityList &operator=(ধ্রুবক LocalePriorityList &) = delete;

    bool add(ধ্রুবক Locale &locale, int32_t weight, UErrorCode &errorCode);

    শূন্য sort(UErrorCode &errorCode);

    LocaleAndWeightArray *list = nullptr;
    int32_t list_দৈর্ঘ্য = 0;
    int32_t অঙ্ক_Removed = 0;
    bool hasWeights = false;  // other than 1.0
    UHashtable *map = nullptr;
};

U_NAMESPACE_END

#endif  // __LOCALEPRIORITYLIST_H__
