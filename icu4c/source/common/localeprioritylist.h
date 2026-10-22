// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// localeprioritylist.h
// created: 2019jul11 Markus W. Scherer

#جے_بنا_تعریف __LOCALEPRIORITYLIST_H__
#تعریف_کرو __LOCALEPRIORITYLIST_H__

#ضمن "unicode/utypes.h"
#ضمن "unicode/locid.h"
#ضمن "unicode/stringpiece.h"
#ضمن "unicode/uobject.h"

بنية UHashtable;

U_NAMESPACE_BEGIN

بنية Locale_And_بھار_Array;

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
class U_COMMON_API Locale_ترجیح_List : عام UMemory {
عام:
    class Iterator : عام Locale::Iterator {
    عام:
        UBool has_اگلا() ثابت override { أرجع گنتی < لمائی; }

        ثابت Locale &اگلا() override {
            لكل(;;) {
                ثابت Locale *locale = list.localeAt(اشاریہ++);
                إذا (locale != nullptr) {
                    ++گنتی;
                    أرجع *locale;
                }
            }
        }

    خاص:
        یار class Locale_ترجیح_List;

        Iterator(ثابت Locale_ترجیح_List &list) : list(list), لمائی(list.get_لمائی()) {}

        ثابت Locale_ترجیح_List &list;
        int32_t اشاریہ = 0;
        int32_t گنتی = 0;
        ثابت int32_t لمائی;
    };

    Locale_ترجیح_List(StringPiece s, UError_کوڈ &error_کوڈ);

    ~Locale_ترجیح_List();

    int32_t get_لمائی() ثابت { أرجع list_لمائی - عدد_Removed; }

    int32_t get_لمائی_Including_Removed() ثابت { أرجع list_لمائی; }

    Iterator iterator() ثابت { أرجع Iterator(*this); }

    ثابت Locale *localeAt(int32_t i) ثابت;

    Locale *orphanLocaleAt(int32_t i);

خاص:
    Locale_ترجیح_List(ثابت Locale_ترجیح_List &) = delete;
    Locale_ترجیح_List &عامل=(ثابت Locale_ترجیح_List &) = delete;

    bool جوڑو(ثابت Locale &locale, int32_t بھار, UError_کوڈ &error_کوڈ);

    فراغ ترتیب_دیو(UError_کوڈ &error_کوڈ);

    Locale_And_بھار_Array *list = nullptr;
    int32_t list_لمائی = 0;
    int32_t عدد_Removed = 0;
    bool hasWeights = false;  // other than 1.0
    UHashtable *map = nullptr;
};

U_NAMESPACE_سرا

#جے_انت  // __LOCALEPRIORITYLIST_H__
