// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// localeprioritylist.h
// created: 2019jul11 Markus W. Scherer

#اگر_غیر_تعریف_شدہ __LOCALEPRIORITYLIST_H__
#تعریف_کرو __LOCALEPRIORITYLIST_H__

#شامل "unicode/utypes.h"
#شامل "unicode/locid.h"
#شامل "unicode/stringpiece.h"
#شامل "unicode/uobject.h"

ساخت UHashtable;

U_NAMESPACE_BEGIN

ساخت Locale_And_وزن_Array;

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
class U_COMMON_API Locale_ترجیح_List : عوامی UMemory {
عوامی:
    class Iterator : عوامی Locale::Iterator {
    عوامی:
        UBool has_اگلا() ثابت override { واپس گنتی < لمبائی; }

        ثابت Locale &اگلا() override {
            برائے(;;) {
                ثابت Locale *locale = list.localeAt(اشاریہ++);
                اگر (locale != nullptr) {
                    ++گنتی;
                    واپس *locale;
                }
            }
        }

    نجی:
        دوست class Locale_ترجیح_List;

        Iterator(ثابت Locale_ترجیح_List &list) : list(list), لمبائی(list.get_لمبائی()) {}

        ثابت Locale_ترجیح_List &list;
        int32_t اشاریہ = 0;
        int32_t گنتی = 0;
        ثابت int32_t لمبائی;
    };

    Locale_ترجیح_List(StringPiece s, UError_رمز &error_رمز);

    ~Locale_ترجیح_List();

    int32_t get_لمبائی() ثابت { واپس list_لمبائی - عدد_Removed; }

    int32_t get_لمبائی_Including_Removed() ثابت { واپس list_لمبائی; }

    Iterator iterator() ثابت { واپس Iterator(*this); }

    ثابت Locale *localeAt(int32_t i) ثابت;

    Locale *orphanLocaleAt(int32_t i);

نجی:
    Locale_ترجیح_List(ثابت Locale_ترجیح_List &) = delete;
    Locale_ترجیح_List &عامل=(ثابت Locale_ترجیح_List &) = delete;

    bool جوڑو(ثابت Locale &locale, int32_t وزن, UError_رمز &error_رمز);

    خالی ترتیب_دو(UError_رمز &error_رمز);

    Locale_And_وزن_Array *list = nullptr;
    int32_t list_لمبائی = 0;
    int32_t عدد_Removed = 0;
    bool hasWeights = false;  // other than 1.0
    UHashtable *map = nullptr;
};

U_NAMESPACE_انتہا

#اگر_ختم  // __LOCALEPRIORITYLIST_H__
