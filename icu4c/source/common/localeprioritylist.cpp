// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// localeprioritylist.cpp
// created: 2019jul11 Markus W. Scherer

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"
#ଅନ୍ତର୍ଭାଵ "unicode/localpointer.h"
#ଅନ୍ତର୍ଭାଵ "unicode/locid.h"
#ଅନ୍ତର୍ଭାଵ "unicode/stringpiece.h"
#ଅନ୍ତର୍ଭାଵ "unicode/uobject.h"
#ଅନ୍ତର୍ଭାଵ "charstr.h"
#ଅନ୍ତର୍ଭାଵ "cmemory.h"
#ଅନ୍ତର୍ଭାଵ "localeprioritylist.h"
#ଅନ୍ତର୍ଭାଵ "uarrsort.h"
#ଅନ୍ତର୍ଭାଵ "uassert.h"
#ଅନ୍ତର୍ଭାଵ "uhash.h"

U_NAMESPACE_BEGIN

ନାମାକାଶ {

int32_t hashLocale(ଅଚଳ UHashTok token) {
    ଅଚଳ auto* locale = static_cast<ଅଚଳ Locale*>(token.pointer);
    ଫେରନ୍ତୁ locale->hashCode();
}

UBool compareLocales(ଅଚଳ UHashTok t1, ଅଚଳ UHashTok t2) {
    ଅଚଳ auto* l1 = static_cast<ଅଚଳ Locale*>(t1.pointer);
    ଅଚଳ auto* l2 = static_cast<ଅଚଳ Locale*>(t2.pointer);
    ଫେରନ୍ତୁ *l1 == *l2;
}

constexpr int32_t WEIGHT_ONE = 1000;

ସଂରଚନା LocaleAndWeight {
    Locale *locale;
    int32_t weight;  // 0..1000 = 0.0..1.0
    int32_t index;  // force stable sort

    int32_t compare(ଅଚଳ LocaleAndWeight &other) ଅଚଳ {
        int32_t diff = other.weight - weight;  // descending: other-this
        ଯଦି (diff != 0) { ଫେରନ୍ତୁ diff; }
        ଫେରନ୍ତୁ index - other.index;
    }
};

int32_t U_CALLCONV
compareLocaleAndWeight(ଅଚଳ ଶୂନ୍ୟ * /*context*/, ଅଚଳ ଶୂନ୍ୟ *left, ଅଚଳ ଶୂନ୍ୟ *ଡାହାଣ) {
    ଫେରନ୍ତୁ static_cast<ଅଚଳ LocaleAndWeight *>(left)->
        compare(*static_cast<ଅଚଳ LocaleAndWeight *>(ଡାହାଣ));
}

ଅଚଳ ଅକ୍ଷର *skipSpaces(ଅଚଳ ଅକ୍ଷର *p, ଅଚଳ ଅକ୍ଷର *limit) {
    ଯେତେବେଳେ (p < limit && *p == ' ') { ++p; }
    ଫେରନ୍ତୁ p;
}

int32_t findTagLength(ଅଚଳ ଅକ୍ଷର *p, ଅଚଳ ଅକ୍ଷର *limit) {
    // Look for accept-language delimiters.
    // Leave other validation up to the Locale constructor.
    ଅଚଳ ଅକ୍ଷର *q;
    ପାଇଁ (q = p; q < limit; ++q) {
        ଅକ୍ଷର c = *q;
        ଯଦି (c == ' ' || c == ',' || c == ';') { ଭାଙ୍ଗ; }
    }
    ଫେରନ୍ତୁ static_cast<int32_t>(q - p);
}

/**
 * Parses and returns a qvalue weight in millis.
 * Advances p to after the parsed substring.
 * Returns a negative value if parsing fails.
 */
int32_t parseWeight(ଅଚଳ ଅକ୍ଷର *&p, ଅଚଳ ଅକ୍ଷର *limit) {
    p = skipSpaces(p, limit);
    ଅକ୍ଷର c;
    ଯଦି (p == limit || ((c = *p) != '0' && c != '1')) { ଫେରନ୍ତୁ -1; }
    int32_t weight = (c - '0') * 1000;
    ଯଦି (++p == limit || *p != '.') { ଫେରନ୍ତୁ weight; }
    int32_t multiplier = 100;
    ଯେତେବେଳେ (++p != limit && '0' <= (c = *p) && c <= '9') {
        c -= '0';
        ଯଦି (multiplier > 0) {
            weight += c * multiplier;
            multiplier /= 10;
        } ଅନ୍ୟଥା ଯଦି (multiplier == 0) {
            // round up
            ଯଦି (c >= 5) { ++weight; }
            multiplier = -1;
        }  // else ignore further fraction digits
    }
    ଫେରନ୍ତୁ weight <= WEIGHT_ONE ? weight : -1;  // bad if > 1.0
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
ସଂରଚନା LocaleAndWeightArray : ସର୍ବସାଧାରଣ UMemory {
    Maybe_ଗଦା_Array<LocaleAndWeight, 20> array;
};

LocalePriorityList::LocalePriorityList(StringPiece s, UErrorCode &errorCode) {
    ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ; }
    list = ନୂତନ LocaleAndWeightArray();
    ଯଦି (list == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        ଫେରନ୍ତୁ;
    }
    ଅଚଳ ଅକ୍ଷର *p = s.data();
    ଅଚଳ ଅକ୍ଷର *limit = p + s.length();
    ଯେତେବେଳେ ((p = skipSpaces(p, limit)) != limit) {
        ଯଦି (*p == ',') {  // empty range field
            ++p;
            ଜାରି;
        }
        int32_t tagLength = findTagLength(p, limit);
        ଯଦି (tagLength == 0) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            ଫେରନ୍ତୁ;
        }
        CharString tag(p, tagLength, errorCode);
        ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ; }
        Locale locale = Locale(tag.data());
        ଯଦି (locale.isBogus()) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            ଫେରନ୍ତୁ;
        }
        int32_t weight = WEIGHT_ONE;
        ଯଦି ((p = skipSpaces(p + tagLength, limit)) != limit && *p == ';') {
            ଯଦି ((p = skipSpaces(p + 1, limit)) == limit || *p != 'q' ||
                    (p = skipSpaces(p + 1, limit)) == limit || *p != '=' ||
                    (++p, (weight = parseWeight(p, limit)) < 0)) {
                errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                ଫେରନ୍ତୁ;
            }
            p = skipSpaces(p, limit);
        }
        ଯଦି (p != limit && *p != ',') {  // trailing junk
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            ଫେରନ୍ତୁ;
        }
        add(locale, weight, errorCode);
        ଯଦି (p == limit) { ଭାଙ୍ଗ; }
        ++p;
    }
    sort(errorCode);
}

LocalePriorityList::~LocalePriorityList() {
    ଯଦି (list != nullptr) {
        ପାଇଁ (int32_t i = 0; i < listLength; ++i) {
            delete list->array[i].locale;
        }
        delete list;
    }
    uhash_close(map);
}

ଅଚଳ Locale *LocalePriorityList::localeAt(int32_t i) ଅଚଳ {
    ଫେରନ୍ତୁ list->array[i].locale;
}

Locale *LocalePriorityList::orphanLocaleAt(int32_t i) {
    ଯଦି (list == nullptr) { ଫେରନ୍ତୁ nullptr; }
    LocaleAndWeight &lw = list->array[i];
    Locale *l = lw.locale;
    lw.locale = nullptr;
    ଫେରନ୍ତୁ l;
}

ତାର୍କିକ LocalePriorityList::add(ଅଚଳ Locale &locale, int32_t weight, UErrorCode &errorCode) {
    ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ ମିଥ୍ୟା; }
    ଯଦି (map == nullptr) {
        ଯଦି (weight <= 0) { ଫେରନ୍ତୁ ସତ୍ୟ; }  // do not add q=0
        map = uhash_open(hashLocale, compareLocales, uhash_compareLong, &errorCode);
        ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ ମିଥ୍ୟା; }
    }
    LocalPointer<Locale> clone;
    UBool found = ମିଥ୍ୟା;
    int32_t index = uhash_getiAndFound(map, &locale, &found);
    ଯଦି (found) {
        // Duplicate: Remove the old item and append it anew.
        LocaleAndWeight &lw = list->array[index];
        clone.adoptInstead(lw.locale);
        lw.locale = nullptr;
        lw.weight = 0;
        ++ଅଙ୍କ_Removed;
    }
    ଯଦି (weight <= 0) {  // do not add q=0
        ଯଦି (found) {
            // Not strictly necessary but cleaner.
            uhash_removei(map, &locale);
        }
        ଫେରନ୍ତୁ ସତ୍ୟ;
    }
    ଯଦି (clone.isNull()) {
        clone.adoptInstead(locale.clone());
        ଯଦି (clone.isNull() || (clone->isBogus() && !locale.isBogus())) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            ଫେରନ୍ତୁ ମିଥ୍ୟା;
        }
    }
    ଯଦି (listLength == list->array.getCapacity()) {
        int32_t newCapacity = listLength < 50 ? 100 : 4 * listLength;
        ଯଦି (list->array.resize(newCapacity, listLength) == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            ଫେରନ୍ତୁ ମିଥ୍ୟା;
        }
    }
    uhash_putiAllowZero(map, clone.getAlias(), listLength, &errorCode);
    ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ ମିଥ୍ୟା; }
    LocaleAndWeight &lw = list->array[listLength];
    lw.locale = clone.orphan();
    lw.weight = weight;
    lw.index = listLength++;
    ଯଦି (weight < WEIGHT_ONE) { hasWeights = ସତ୍ୟ; }
    U_ASSERT(uhash_ଗଣନା(map) == getLength());
    ଫେରନ୍ତୁ ସତ୍ୟ;
}

ଶୂନ୍ୟ LocalePriorityList::sort(UErrorCode &errorCode) {
    // Sort by descending weights if there is a mix of weights.
    // The comparator forces a stable sort via the item index.
    ଯଦି (U_FAILURE(errorCode) || getLength() <= 1 || !hasWeights) { ଫେରନ୍ତୁ; }
    uprv_sortArray(list->array.getAlias(), listLength, ଆକାର(LocaleAndWeight),
                   compareLocaleAndWeight, nullptr, ମିଥ୍ୟା, &errorCode);
}

U_NAMESPACE_END
