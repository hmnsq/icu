// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// localeprioritylist.h
// created: 2019jul11 Markus W. Scherer

#که_نه_تعریف_شوی __LOCALEPRIORITYLIST_H__
#تعریف_کړه __LOCALEPRIORITYLIST_H__

#ضمن "unicode/utypes.h"
#ضمن "unicode/locid.h"
#ضمن "unicode/stringpiece.h"
#ضمن "unicode/uobject.h"

بنية UHashtable;

U_NAMESPACE_BEGIN

بنية Locale_And_وزن_Array;

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
class U_COMMON_API Locale_لومړیتوب_List : عام UMemory {
عام:
    class Iterator : عام Locale::Iterator {
    عام:
        UBool has_بل() ثابت override { أرجع شمېر < اوږدتیا; }

        ثابت Locale &بل() override {
            لكل(;;) {
                ثابت Locale *locale = list.localeAt(شاخص++);
                إذا (locale != nullptr) {
                    ++شمېر;
                    أرجع *locale;
                }
            }
        }

    خاص:
        ملګری class Locale_لومړیتوب_List;

        Iterator(ثابت Locale_لومړیتوب_List &list) : list(list), اوږدتیا(list.get_اوږدتیا()) {}

        ثابت Locale_لومړیتوب_List &list;
        int32_t شاخص = 0;
        int32_t شمېر = 0;
        ثابت int32_t اوږدتیا;
    };

    Locale_لومړیتوب_List(StringPiece s, UError_کوډ &error_کوډ);

    ~Locale_لومړیتوب_List();

    int32_t get_اوږدتیا() ثابت { أرجع list_اوږدتیا - عدد_Removed; }

    int32_t get_اوږدتیا_Including_Removed() ثابت { أرجع list_اوږدتیا; }

    Iterator iterator() ثابت { أرجع Iterator(*this); }

    ثابت Locale *localeAt(int32_t i) ثابت;

    Locale *orphanLocaleAt(int32_t i);

خاص:
    Locale_لومړیتوب_List(ثابت Locale_لومړیتوب_List &) = delete;
    Locale_لومړیتوب_List &چلوونکی=(ثابت Locale_لومړیتوب_List &) = delete;

    bool جمع_کړه(ثابت Locale &locale, int32_t وزن, UError_کوډ &error_کوډ);

    فراغ ترتیبول(UError_کوډ &error_کوډ);

    Locale_And_وزن_Array *list = nullptr;
    int32_t list_اوږدتیا = 0;
    int32_t عدد_Removed = 0;
    bool hasWeights = false;  // other than 1.0
    UHashtable *map = nullptr;
};

U_NAMESPACE_پای_ټکی

#د_که_پای  // __LOCALEPRIORITYLIST_H__
