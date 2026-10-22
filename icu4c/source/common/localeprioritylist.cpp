// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// localeprioritylist.cpp
// created: 2019jul11 Markus W. Scherer

#समावेश "unicode/utypes.h"
#समावेश "unicode/localpointer.h"
#समावेश "unicode/locid.h"
#समावेश "unicode/stringpiece.h"
#समावेश "unicode/uobject.h"
#समावेश "charstr.h"
#समावेश "cmemory.h"
#समावेश "localeprioritylist.h"
#समावेश "uarrsort.h"
#समावेश "uassert.h"
#समावेश "uhash.h"

U_NAMESPACE_BEGIN

namespace {

int32_t hashLocale(स्थिर UHashTok token) {
    स्थिर स्वतः* locale = static_cast<स्थिर Locale*>(token.pointer);
    वापस locale->hashCode();
}

UBool compareLocales(स्थिर UHashTok t1, स्थिर UHashTok t2) {
    स्थिर स्वतः* l1 = static_cast<स्थिर Locale*>(t1.pointer);
    स्थिर स्वतः* l2 = static_cast<स्थिर Locale*>(t2.pointer);
    वापस *l1 == *l2;
}

constexpr int32_t WEIGHT_ONE = 1000;

काष्ठा LocaleAndWeight {
    Locale *locale;
    int32_t weight;  // 0..1000 = 0.0..1.0
    int32_t सूचकांक;  // force stable sort

    int32_t compare(स्थिर LocaleAndWeight &other) स्थिर {
        int32_t diff = other.weight - weight;  // descending: other-this
        अगर (diff != 0) { वापस diff; }
        वापस सूचकांक - other.सूचकांक;
    }
};

int32_t U_CALLCONV
compareLocaleAndWeight(स्थिर व्योम * /*context*/, स्थिर व्योम *बाँए, स्थिर व्योम *दाँए) {
    वापस static_cast<स्थिर LocaleAndWeight *>(बाँए)->
        compare(*static_cast<स्थिर LocaleAndWeight *>(दाँए));
}

स्थिर अक्षर *skipSpaces(स्थिर अक्षर *p, स्थिर अक्षर *limit) {
    जबतक (p < limit && *p == ' ') { ++p; }
    वापस p;
}

int32_t find_Tag_लंबाई(स्थिर अक्षर *p, स्थिर अक्षर *limit) {
    // Look for accept-language delimiters.
    // Leave other validation up to the Locale constructor.
    स्थिर अक्षर *q;
    क्रम (q = p; q < limit; ++q) {
        अक्षर c = *q;
        अगर (c == ' ' || c == ',' || c == ';') { अवरोध; }
    }
    वापस static_cast<int32_t>(q - p);
}

/**
 * Parses and returns a qvalue weight in millis.
 * Advances p to after the parsed substring.
 * Returns a negative value if parsing fails.
 */
int32_t parseWeight(स्थिर अक्षर *&p, स्थिर अक्षर *limit) {
    p = skipSpaces(p, limit);
    अक्षर c;
    अगर (p == limit || ((c = *p) != '0' && c != '1')) { वापस -1; }
    int32_t weight = (c - '0') * 1000;
    अगर (++p == limit || *p != '.') { वापस weight; }
    int32_t multiplier = 100;
    जबतक (++p != limit && '0' <= (c = *p) && c <= '9') {
        c -= '0';
        अगर (multiplier > 0) {
            weight += c * multiplier;
            multiplier /= 10;
        } अन्यथा अगर (multiplier == 0) {
            // round up
            अगर (c >= 5) { ++weight; }
            multiplier = -1;
        }  // else ignore further fraction digits
    }
    वापस weight <= WEIGHT_ONE ? weight : -1;  // bad if > 1.0
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
काष्ठा LocaleAndWeightArray : खुला UMemory {
    Maybe_ढेर_Array<LocaleAndWeight, 20> array;
};

LocalePriorityList::LocalePriorityList(StringPiece s, UErrorCode &errorCode) {
    अगर (U_FAILURE(errorCode)) { वापस; }
    list = नया LocaleAndWeightArray();
    अगर (list == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        वापस;
    }
    स्थिर अक्षर *p = s.तथ्य();
    स्थिर अक्षर *limit = p + s.length();
    जबतक ((p = skipSpaces(p, limit)) != limit) {
        अगर (*p == ',') {  // empty range field
            ++p;
            जारी;
        }
        int32_t tag_लंबाई = find_Tag_लंबाई(p, limit);
        अगर (tag_लंबाई == 0) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            वापस;
        }
        CharString tag(p, tag_लंबाई, errorCode);
        अगर (U_FAILURE(errorCode)) { वापस; }
        Locale locale = Locale(tag.तथ्य());
        अगर (locale.isBogus()) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            वापस;
        }
        int32_t weight = WEIGHT_ONE;
        अगर ((p = skipSpaces(p + tag_लंबाई, limit)) != limit && *p == ';') {
            अगर ((p = skipSpaces(p + 1, limit)) == limit || *p != 'q' ||
                    (p = skipSpaces(p + 1, limit)) == limit || *p != '=' ||
                    (++p, (weight = parseWeight(p, limit)) < 0)) {
                errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                वापस;
            }
            p = skipSpaces(p, limit);
        }
        अगर (p != limit && *p != ',') {  // trailing junk
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            वापस;
        }
        add(locale, weight, errorCode);
        अगर (p == limit) { अवरोध; }
        ++p;
    }
    sort(errorCode);
}

LocalePriorityList::~LocalePriorityList() {
    अगर (list != nullptr) {
        क्रम (int32_t i = 0; i < list_लंबाई; ++i) {
            मिटाओ list->array[i].locale;
        }
        मिटाओ list;
    }
    uhash_close(map);
}

स्थिर Locale *LocalePriorityList::localeAt(int32_t i) स्थिर {
    वापस list->array[i].locale;
}

Locale *LocalePriorityList::orphanLocaleAt(int32_t i) {
    अगर (list == nullptr) { वापस nullptr; }
    LocaleAndWeight &lw = list->array[i];
    Locale *l = lw.locale;
    lw.locale = nullptr;
    वापस l;
}

bool LocalePriorityList::add(स्थिर Locale &locale, int32_t weight, UErrorCode &errorCode) {
    अगर (U_FAILURE(errorCode)) { वापस false; }
    अगर (map == nullptr) {
        अगर (weight <= 0) { वापस true; }  // do not add q=0
        map = uhash_open(hashLocale, compareLocales, uhash_compareLong, &errorCode);
        अगर (U_FAILURE(errorCode)) { वापस false; }
    }
    LocalPointer<Locale> clone;
    UBool found = false;
    int32_t सूचकांक = uhash_getiAndFound(map, &locale, &found);
    अगर (found) {
        // Duplicate: Remove the old item and append it anew.
        LocaleAndWeight &lw = list->array[सूचकांक];
        clone.adoptInstead(lw.locale);
        lw.locale = nullptr;
        lw.weight = 0;
        ++अंक_Removed;
    }
    अगर (weight <= 0) {  // do not add q=0
        अगर (found) {
            // Not strictly necessary but cleaner.
            uhash_removei(map, &locale);
        }
        वापस true;
    }
    अगर (clone.isNull()) {
        clone.adoptInstead(locale.clone());
        अगर (clone.isNull() || (clone->isBogus() && !locale.isBogus())) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            वापस false;
        }
    }
    अगर (list_लंबाई == list->array.getCapacity()) {
        int32_t newCapacity = list_लंबाई < 50 ? 100 : 4 * list_लंबाई;
        अगर (list->array.resize(newCapacity, list_लंबाई) == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            वापस false;
        }
    }
    uhash_putiAllowZero(map, clone.getAlias(), list_लंबाई, &errorCode);
    अगर (U_FAILURE(errorCode)) { वापस false; }
    LocaleAndWeight &lw = list->array[list_लंबाई];
    lw.locale = clone.orphan();
    lw.weight = weight;
    lw.सूचकांक = list_लंबाई++;
    अगर (weight < WEIGHT_ONE) { hasWeights = true; }
    U_ASSERT(uhash_गिनती(map) == get_लंबाई());
    वापस true;
}

व्योम LocalePriorityList::sort(UErrorCode &errorCode) {
    // Sort by descending weights if there is a mix of weights.
    // The comparator forces a stable sort via the item index.
    अगर (U_FAILURE(errorCode) || get_लंबाई() <= 1 || !hasWeights) { वापस; }
    uprv_sortArray(list->array.getAlias(), list_लंबाई, माप(LocaleAndWeight),
                   compareLocaleAndWeight, nullptr, false, &errorCode);
}

U_NAMESPACE_END
