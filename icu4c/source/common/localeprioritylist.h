// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// localeprioritylist.h
// created: 2019jul11 Markus W. Scherer

#నిర్వచితం_కాకపోతే __LOCALEPRIORITYLIST_H__
#నిర్వచించు __LOCALEPRIORITYLIST_H__

#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "unicode/locid.h"
#అన్తర్భావ "unicode/stringpiece.h"
#అన్తర్భావ "unicode/uobject.h"

నిర్మాణం UHashtable;

U_NAMESPACE_BEGIN

నిర్మాణం Locale_And_బరువు_Array;

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
class U_COMMON_API Locale_ప్రాధాన్యత_List : బహిరంగ UMemory {
బహిరంగ:
    class Iterator : బహిరంగ Locale::Iterator {
    బహిరంగ:
        UBool has_తదుపరి() స్థిరరాశి override { తిరిగి లెక్క < నిడివి; }

        స్థిరరాశి Locale &తదుపరి() override {
            కొరకు(;;) {
                స్థిరరాశి Locale *locale = list.localeAt(సూచీ++);
                ఒకవేళ (locale != nullptr) {
                    ++లెక్క;
                    తిరిగి *locale;
                }
            }
        }

    అంతరంగ:
        మిత్ర class Locale_ప్రాధాన్యత_List;

        Iterator(స్థిరరాశి Locale_ప్రాధాన్యత_List &list) : list(list), నిడివి(list.get_నిడివి()) {}

        స్థిరరాశి Locale_ప్రాధాన్యత_List &list;
        int32_t సూచీ = 0;
        int32_t లెక్క = 0;
        స్థిరరాశి int32_t నిడివి;
    };

    Locale_ప్రాధాన్యత_List(StringPiece s, UError_సంకేతం &error_సంకేతం);

    ~Locale_ప్రాధాన్యత_List();

    int32_t get_నిడివి() స్థిరరాశి { తిరిగి list_నిడివి - అంకె_Removed; }

    int32_t get_నిడివి_Including_Removed() స్థిరరాశి { తిరిగి list_నిడివి; }

    Iterator iterator() స్థిరరాశి { తిరిగి Iterator(*this); }

    స్థిరరాశి Locale *localeAt(int32_t i) స్థిరరాశి;

    Locale *orphanLocaleAt(int32_t i);

అంతరంగ:
    Locale_ప్రాధాన్యత_List(స్థిరరాశి Locale_ప్రాధాన్యత_List &) = delete;
    Locale_ప్రాధాన్యత_List &క్రియాకారకం=(స్థిరరాశి Locale_ప్రాధాన్యత_List &) = delete;

    bool కూడు(స్థిరరాశి Locale &locale, int32_t బరువు, UError_సంకేతం &error_సంకేతం);

    శూన్య క్రమీకరించు(UError_సంకేతం &error_సంకేతం);

    Locale_And_బరువు_Array *list = nullptr;
    int32_t list_నిడివి = 0;
    int32_t అంకె_Removed = 0;
    bool hasWeights = false;  // other than 1.0
    UHashtable *map = nullptr;
};

U_NAMESPACE_చివర

#అయితే_ముగింపు  // __LOCALEPRIORITYLIST_H__
