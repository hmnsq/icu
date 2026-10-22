// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// localeprioritylist.cpp
// created: 2019jul11 Markus W. Scherer

#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "unicode/localpointer.h"
#అన్తర్భావ "unicode/locid.h"
#అన్తర్భావ "unicode/stringpiece.h"
#అన్తర్భావ "unicode/uobject.h"
#అన్తర్భావ "charstr.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "localeprioritylist.h"
#అన్తర్భావ "uarrsort.h"
#అన్తర్భావ "uassert.h"
#అన్తర్భావ "uhash.h"

U_NAMESPACE_BEGIN

నామాకాశ {

int32_t hashLocale(స్థిరరాశి UHashTok token) {
    స్థిరరాశి స్వయంచాలక* locale = static_cast<స్థిరరాశి Locale*>(token.సూచిక);
    తిరిగి locale->hash_సంకేతం();
}

UBool compareLocales(స్థిరరాశి UHashTok t1, స్థిరరాశి UHashTok t2) {
    స్థిరరాశి స్వయంచాలక* l1 = static_cast<స్థిరరాశి Locale*>(t1.సూచిక);
    స్థిరరాశి స్వయంచాలక* l2 = static_cast<స్థిరరాశి Locale*>(t2.సూచిక);
    తిరిగి *l1 == *l2;
}

constexpr int32_t బరువు_ONE = 1000;

నిర్మాణం Locale_And_బరువు {
    Locale *locale;
    int32_t బరువు;  // 0..1000 = 0.0..1.0
    int32_t సూచీ;  // force stable sort

    int32_t compare(స్థిరరాశి Locale_And_బరువు &other) స్థిరరాశి {
        int32_t diff = other.బరువు - బరువు;  // descending: other-this
        ఒకవేళ (diff != 0) { తిరిగి diff; }
        తిరిగి సూచీ - other.సూచీ;
    }
};

int32_t U_CALLCONV
compare_Locale_And_బరువు(స్థిరరాశి శూన్య * /*context*/, స్థిరరాశి శూన్య *ఎడమ, స్థిరరాశి శూన్య *కుడి) {
    తిరిగి static_cast<స్థిరరాశి Locale_And_బరువు *>(ఎడమ)->
        compare(*static_cast<స్థిరరాశి Locale_And_బరువు *>(కుడి));
}

స్థిరరాశి అక్షరం *skipSpaces(స్థిరరాశి అక్షరం *p, స్థిరరాశి అక్షరం *హద్దు) {
    అయితే (p < హద్దు && *p == ' ') { ++p; }
    తిరిగి p;
}

int32_t కనుగొను_ట్యాగ్_నిడివి(స్థిరరాశి అక్షరం *p, స్థిరరాశి అక్షరం *హద్దు) {
    // Look for accept-language delimiters.
    // Leave other validation up to the Locale constructor.
    స్థిరరాశి అక్షరం *q;
    కొరకు (q = p; q < హద్దు; ++q) {
        అక్షరం c = *q;
        ఒకవేళ (c == ' ' || c == ',' || c == ';') { ఆపు; }
    }
    తిరిగి static_cast<int32_t>(q - p);
}

/**
 * Parses and returns a qvalue weight in millis.
 * Advances p to after the parsed substring.
 * Returns a negative value if parsing fails.
 */
int32_t parse_బరువు(స్థిరరాశి అక్షరం *&p, స్థిరరాశి అక్షరం *హద్దు) {
    p = skipSpaces(p, హద్దు);
    అక్షరం c;
    ఒకవేళ (p == హద్దు || ((c = *p) != '0' && c != '1')) { తిరిగి -1; }
    int32_t బరువు = (c - '0') * 1000;
    ఒకవేళ (++p == హద్దు || *p != '.') { తిరిగి బరువు; }
    int32_t multiplier = 100;
    అయితే (++p != హద్దు && '0' <= (c = *p) && c <= '9') {
        c -= '0';
        ఒకవేళ (multiplier > 0) {
            బరువు += c * multiplier;
            multiplier /= 10;
        } లేకపోతే ఒకవేళ (multiplier == 0) {
            // round up
            ఒకవేళ (c >= 5) { ++బరువు; }
            multiplier = -1;
        }  // else ignore further fraction digits
    }
    తిరిగి బరువు <= బరువు_ONE ? బరువు : -1;  // bad if > 1.0
}

}  // namespace

/**
 * Nothing but a wrapper over a MaybeStackArray of LocaleAndWeight.
 *
 * This wrapper exists (and is not in an anonymous namespace)
 * so that we can forward-declare it in the header file and
 * don't have to expose the MaybeStackArray specialization and
 * the LocaleAndWeight to code (like the test) that #includes localeprioritylist.h.
 * Also, otherwise we would have to do a platform-specific
 * template export declaration of some kind for the MaybeStackArray specialization
 * to be properly exported from the common DLL.
 */
నిర్మాణం Locale_And_బరువు_Array : బహిరంగ UMemory {
    Maybe_దొంతర_Array<Locale_And_బరువు, 20> array;
};

Locale_ప్రాధాన్యత_List::Locale_ప్రాధాన్యత_List(StringPiece s, UError_సంకేతం &error_సంకేతం) {
    ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి; }
    list = కొత్త Locale_And_బరువు_Array();
    ఒకవేళ (list == శూన్యసూచిక) {
        error_సంకేతం = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి;
    }
    స్థిరరాశి అక్షరం *p = s.దత్తాంశం();
    స్థిరరాశి అక్షరం *హద్దు = p + s.length();
    అయితే ((p = skipSpaces(p, హద్దు)) != హద్దు) {
        ఒకవేళ (*p == ',') {  // empty range field
            ++p;
            కొనసాగు;
        }
        int32_t ట్యాగ్_నిడివి = కనుగొను_ట్యాగ్_నిడివి(p, హద్దు);
        ఒకవేళ (ట్యాగ్_నిడివి == 0) {
            error_సంకేతం = U_ILLEGAL_వాదం_ERROR;
            తిరిగి;
        }
        CharString ట్యాగ్(p, ట్యాగ్_నిడివి, error_సంకేతం);
        ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి; }
        Locale locale = Locale(ట్యాగ్.దత్తాంశం());
        ఒకవేళ (locale.isBogus()) {
            error_సంకేతం = U_ILLEGAL_వాదం_ERROR;
            తిరిగి;
        }
        int32_t బరువు = బరువు_ONE;
        ఒకవేళ ((p = skipSpaces(p + ట్యాగ్_నిడివి, హద్దు)) != హద్దు && *p == ';') {
            ఒకవేళ ((p = skipSpaces(p + 1, హద్దు)) == హద్దు || *p != 'q' ||
                    (p = skipSpaces(p + 1, హద్దు)) == హద్దు || *p != '=' ||
                    (++p, (బరువు = parse_బరువు(p, హద్దు)) < 0)) {
                error_సంకేతం = U_ILLEGAL_వాదం_ERROR;
                తిరిగి;
            }
            p = skipSpaces(p, హద్దు);
        }
        ఒకవేళ (p != హద్దు && *p != ',') {  // trailing junk
            error_సంకేతం = U_ILLEGAL_వాదం_ERROR;
            తిరిగి;
        }
        కూడు(locale, బరువు, error_సంకేతం);
        ఒకవేళ (p == హద్దు) { ఆపు; }
        ++p;
    }
    sort(error_సంకేతం);
}

Locale_ప్రాధాన్యత_List::~Locale_ప్రాధాన్యత_List() {
    ఒకవేళ (list != శూన్యసూచిక) {
        కొరకు (int32_t i = 0; i < list_నిడివి; ++i) {
            తొలగించు list->array[i].locale;
        }
        తొలగించు list;
    }
    uhash_close(map);
}

స్థిరరాశి Locale *Locale_ప్రాధాన్యత_List::localeAt(int32_t i) స్థిరరాశి {
    తిరిగి list->array[i].locale;
}

Locale *Locale_ప్రాధాన్యత_List::orphanLocaleAt(int32_t i) {
    ఒకవేళ (list == శూన్యసూచిక) { తిరిగి శూన్యసూచిక; }
    Locale_And_బరువు &lw = list->array[i];
    Locale *l = lw.locale;
    lw.locale = శూన్యసూచిక;
    తిరిగి l;
}

తార్కిక Locale_ప్రాధాన్యత_List::కూడు(స్థిరరాశి Locale &locale, int32_t బరువు, UError_సంకేతం &error_సంకేతం) {
    ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి అబద్ధం; }
    ఒకవేళ (map == శూన్యసూచిక) {
        ఒకవేళ (బరువు <= 0) { తిరిగి నిజం; }  // do not add q=0
        map = uhash_open(hashLocale, compareLocales, uhash_compareLong, &error_సంకేతం);
        ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి అబద్ధం; }
    }
    Local_సూచిక<Locale> clone;
    UBool దొరికింది = అబద్ధం;
    int32_t సూచీ = uhash_geti_And_దొరికింది(map, &locale, &దొరికింది);
    ఒకవేళ (దొరికింది) {
        // Duplicate: Remove the old item and append it anew.
        Locale_And_బరువు &lw = list->array[సూచీ];
        clone.adoptInstead(lw.locale);
        lw.locale = శూన్యసూచిక;
        lw.బరువు = 0;
        ++అంకె_Removed;
    }
    ఒకవేళ (బరువు <= 0) {  // do not add q=0
        ఒకవేళ (దొరికింది) {
            // Not strictly necessary but cleaner.
            uhash_removei(map, &locale);
        }
        తిరిగి నిజం;
    }
    ఒకవేళ (clone.isNull()) {
        clone.adoptInstead(locale.clone());
        ఒకవేళ (clone.isNull() || (clone->isBogus() && !locale.isBogus())) {
            error_సంకేతం = U_జ్ఞాపకం_కేటాయింపు_ERROR;
            తిరిగి అబద్ధం;
        }
    }
    ఒకవేళ (list_నిడివి == list->array.get_సామర్థ్యం()) {
        int32_t new_సామర్థ్యం = list_నిడివి < 50 ? 100 : 4 * list_నిడివి;
        ఒకవేళ (list->array.resize(new_సామర్థ్యం, list_నిడివి) == శూన్యసూచిక) {
            error_సంకేతం = U_జ్ఞాపకం_కేటాయింపు_ERROR;
            తిరిగి అబద్ధం;
        }
    }
    uhash_putiAllowZero(map, clone.getAlias(), list_నిడివి, &error_సంకేతం);
    ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి అబద్ధం; }
    Locale_And_బరువు &lw = list->array[list_నిడివి];
    lw.locale = clone.orphan();
    lw.బరువు = బరువు;
    lw.సూచీ = list_నిడివి++;
    ఒకవేళ (బరువు < బరువు_ONE) { hasWeights = నిజం; }
    U_ASSERT(uhash_లెక్క(map) == get_నిడివి());
    తిరిగి నిజం;
}

శూన్య Locale_ప్రాధాన్యత_List::sort(UError_సంకేతం &error_సంకేతం) {
    // Sort by descending weights if there is a mix of weights.
    // The comparator forces a stable sort via the item index.
    ఒకవేళ (U_FAILURE(error_సంకేతం) || get_నిడివి() <= 1 || !hasWeights) { తిరిగి; }
    uprv_క్రమీకరించు_Array(list->array.getAlias(), list_నిడివి, పరిమాణం(Locale_And_బరువు),
                   compare_Locale_And_బరువు, శూన్యసూచిక, అబద్ధం, &error_సంకేతం);
}

U_NAMESPACE_చివర
