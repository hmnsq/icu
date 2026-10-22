// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// localeprioritylist.cpp
// created: 2019jul11 Markus W. Scherer

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/localpointer.h"
#अन्तर्भाव "unicode/locid.h"
#अन्तर्भाव "unicode/stringpiece.h"
#अन्तर्भाव "unicode/uobject.h"
#अन्तर्भाव "charstr.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "localeprioritylist.h"
#अन्तर्भाव "uarrsort.h"
#अन्तर्भाव "uassert.h"
#अन्तर्भाव "uhash.h"

U_NAMESPACE_BEGIN

नामाकाश {

int32_t hashLocale(ध्रुव UHashTok token) {
    ध्रुव स्वचालित* locale = static_cast<ध्रुव Locale*>(token.pointer);
    प्रत्यागम locale->hash_संहिता();
}

UBool compareLocales(ध्रुव UHashTok t1, ध्रुव UHashTok t2) {
    ध्रुव स्वचालित* l1 = static_cast<ध्रुव Locale*>(t1.pointer);
    ध्रुव स्वचालित* l2 = static_cast<ध्रुव Locale*>(t2.pointer);
    प्रत्यागम *l1 == *l2;
}

constexpr int32_t WEIGHT_ONE = 1000;

संरचना LocaleAndWeight {
    Locale *locale;
    int32_t weight;  // 0..1000 = 0.0..1.0
    int32_t सूचकांक;  // force stable sort

    int32_t compare(ध्रुव LocaleAndWeight &other) ध्रुव {
        int32_t diff = other.weight - weight;  // descending: other-this
        यदि (diff != 0) { प्रत्यागम diff; }
        प्रत्यागम सूचकांक - other.सूचकांक;
    }
};

int32_t U_CALLCONV
compareLocaleAndWeight(ध्रुव शून्य * /*context*/, ध्रुव शून्य *वामतः, ध्रुव शून्य *दक्षिणतः) {
    प्रत्यागम static_cast<ध्रुव LocaleAndWeight *>(वामतः)->
        compare(*static_cast<ध्रुव LocaleAndWeight *>(दक्षिणतः));
}

ध्रुव वर्ण *skipSpaces(ध्रुव वर्ण *p, ध्रुव वर्ण *सीमा) {
    यावत् (p < सीमा && *p == ' ') { ++p; }
    प्रत्यागम p;
}

int32_t find_चिह्नकम्_दीर्घता(ध्रुव वर्ण *p, ध्रुव वर्ण *सीमा) {
    // Look for accept-language delimiters.
    // Leave other validation up to the Locale constructor.
    ध्रुव वर्ण *q;
    प्रति (q = p; q < सीमा; ++q) {
        वर्ण c = *q;
        यदि (c == ' ' || c == ',' || c == ';') { भङ्ग; }
    }
    प्रत्यागम static_cast<int32_t>(q - p);
}

/**
 * Parses and returns a qvalue weight in millis.
 * Advances p to after the parsed substring.
 * Returns a negative value if parsing fails.
 */
int32_t parseWeight(ध्रुव वर्ण *&p, ध्रुव वर्ण *सीमा) {
    p = skipSpaces(p, सीमा);
    वर्ण c;
    यदि (p == सीमा || ((c = *p) != '0' && c != '1')) { प्रत्यागम -1; }
    int32_t weight = (c - '0') * 1000;
    यदि (++p == सीमा || *p != '.') { प्रत्यागम weight; }
    int32_t multiplier = 100;
    यावत् (++p != सीमा && '0' <= (c = *p) && c <= '9') {
        c -= '0';
        यदि (multiplier > 0) {
            weight += c * multiplier;
            multiplier /= 10;
        } अन्यथा यदि (multiplier == 0) {
            // round up
            यदि (c >= 5) { ++weight; }
            multiplier = -1;
        }  // else ignore further fraction digits
    }
    प्रत्यागम weight <= WEIGHT_ONE ? weight : -1;  // bad if > 1.0
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
संरचना LocaleAndWeightArray : सार्वजनिक UMemory {
    Maybe_राशिः_Array<LocaleAndWeight, 20> array;
};

Locale_प्राथमिकता_List::Locale_प्राथमिकता_List(StringPiece s, UError_संहिता &error_संहिता) {
    यदि (U_FAILURE(error_संहिता)) { प्रत्यागम; }
    list = नूतन LocaleAndWeightArray();
    यदि (list == शून्यसूचकः) {
        error_संहिता = U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम;
    }
    ध्रुव वर्ण *p = s.दत्त();
    ध्रुव वर्ण *सीमा = p + s.length();
    यावत् ((p = skipSpaces(p, सीमा)) != सीमा) {
        यदि (*p == ',') {  // empty range field
            ++p;
            अनुवर्त;
        }
        int32_t चिह्नकम्_दीर्घता = find_चिह्नकम्_दीर्घता(p, सीमा);
        यदि (चिह्नकम्_दीर्घता == 0) {
            error_संहिता = U_ILLEGAL_तर्कः_ERROR;
            प्रत्यागम;
        }
        CharString चिह्नकम्(p, चिह्नकम्_दीर्घता, error_संहिता);
        यदि (U_FAILURE(error_संहिता)) { प्रत्यागम; }
        Locale locale = Locale(चिह्नकम्.दत्त());
        यदि (locale.isBogus()) {
            error_संहिता = U_ILLEGAL_तर्कः_ERROR;
            प्रत्यागम;
        }
        int32_t weight = WEIGHT_ONE;
        यदि ((p = skipSpaces(p + चिह्नकम्_दीर्घता, सीमा)) != सीमा && *p == ';') {
            यदि ((p = skipSpaces(p + 1, सीमा)) == सीमा || *p != 'q' ||
                    (p = skipSpaces(p + 1, सीमा)) == सीमा || *p != '=' ||
                    (++p, (weight = parseWeight(p, सीमा)) < 0)) {
                error_संहिता = U_ILLEGAL_तर्कः_ERROR;
                प्रत्यागम;
            }
            p = skipSpaces(p, सीमा);
        }
        यदि (p != सीमा && *p != ',') {  // trailing junk
            error_संहिता = U_ILLEGAL_तर्कः_ERROR;
            प्रत्यागम;
        }
        सङ्कलय(locale, weight, error_संहिता);
        यदि (p == सीमा) { भङ्ग; }
        ++p;
    }
    sort(error_संहिता);
}

Locale_प्राथमिकता_List::~Locale_प्राथमिकता_List() {
    यदि (list != शून्यसूचकः) {
        प्रति (int32_t i = 0; i < list_दीर्घता; ++i) {
            विलोपय list->array[i].locale;
        }
        विलोपय list;
    }
    uhash_close(map);
}

ध्रुव Locale *Locale_प्राथमिकता_List::localeAt(int32_t i) ध्रुव {
    प्रत्यागम list->array[i].locale;
}

Locale *Locale_प्राथमिकता_List::orphanLocaleAt(int32_t i) {
    यदि (list == शून्यसूचकः) { प्रत्यागम शून्यसूचकः; }
    LocaleAndWeight &lw = list->array[i];
    Locale *l = lw.locale;
    lw.locale = शून्यसूचकः;
    प्रत्यागम l;
}

तार्किक Locale_प्राथमिकता_List::सङ्कलय(ध्रुव Locale &locale, int32_t weight, UError_संहिता &error_संहिता) {
    यदि (U_FAILURE(error_संहिता)) { प्रत्यागम असत्य; }
    यदि (map == शून्यसूचकः) {
        यदि (weight <= 0) { प्रत्यागम सत्य; }  // do not add q=0
        map = uhash_open(hashLocale, compareLocales, uhash_compareLong, &error_संहिता);
        यदि (U_FAILURE(error_संहिता)) { प्रत्यागम असत्य; }
    }
    LocalPointer<Locale> clone;
    UBool प्राप्तम् = असत्य;
    int32_t सूचकांक = uhash_geti_And_प्राप्तम्(map, &locale, &प्राप्तम्);
    यदि (प्राप्तम्) {
        // Duplicate: Remove the old item and append it anew.
        LocaleAndWeight &lw = list->array[सूचकांक];
        clone.adoptInstead(lw.locale);
        lw.locale = शून्यसूचकः;
        lw.weight = 0;
        ++अङ्कः_Removed;
    }
    यदि (weight <= 0) {  // do not add q=0
        यदि (प्राप्तम्) {
            // Not strictly necessary but cleaner.
            uhash_removei(map, &locale);
        }
        प्रत्यागम सत्य;
    }
    यदि (clone.isNull()) {
        clone.adoptInstead(locale.clone());
        यदि (clone.isNull() || (clone->isBogus() && !locale.isBogus())) {
            error_संहिता = U_स्मृतिः_आवण्टनम्_ERROR;
            प्रत्यागम असत्य;
        }
    }
    यदि (list_दीर्घता == list->array.get_क्षमता()) {
        int32_t new_क्षमता = list_दीर्घता < 50 ? 100 : 4 * list_दीर्घता;
        यदि (list->array.resize(new_क्षमता, list_दीर्घता) == शून्यसूचकः) {
            error_संहिता = U_स्मृतिः_आवण्टनम्_ERROR;
            प्रत्यागम असत्य;
        }
    }
    uhash_putiAllowZero(map, clone.getAlias(), list_दीर्घता, &error_संहिता);
    यदि (U_FAILURE(error_संहिता)) { प्रत्यागम असत्य; }
    LocaleAndWeight &lw = list->array[list_दीर्घता];
    lw.locale = clone.orphan();
    lw.weight = weight;
    lw.सूचकांक = list_दीर्घता++;
    यदि (weight < WEIGHT_ONE) { hasWeights = सत्य; }
    U_ASSERT(uhash_गणनम्(map) == get_दीर्घता());
    प्रत्यागम सत्य;
}

शून्य Locale_प्राथमिकता_List::sort(UError_संहिता &error_संहिता) {
    // Sort by descending weights if there is a mix of weights.
    // The comparator forces a stable sort via the item index.
    यदि (U_FAILURE(error_संहिता) || get_दीर्घता() <= 1 || !hasWeights) { प्रत्यागम; }
    uprv_क्रमय_Array(list->array.getAlias(), list_दीर्घता, परिमाण(LocaleAndWeight),
                   compareLocaleAndWeight, शून्यसूचकः, असत्य, &error_संहिता);
}

U_NAMESPACE_अन्तः
