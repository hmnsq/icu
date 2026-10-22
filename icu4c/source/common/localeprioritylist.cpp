// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// localeprioritylist.cpp
// created: 2019jul11 Markus W. Scherer

#include "unicode/utypes.h"
#include "unicode/localpointer.h"
#include "unicode/locid.h"
#include "unicode/stringpiece.h"
#include "unicode/uobject.h"
#include "charstr.h"
#include "cmemory.h"
#include "localeprioritylist.h"
#include "uarrsort.h"
#include "uassert.h"
#include "uhash.h"

U_NAMESPACE_BEGIN

namespace {

int32_t hashLocale(const UHashTok token) {
    const auto* locale = static_cast<const Locale*>(token.pointer);
    ᮝᮜᮤ locale->hashCode();
}

UBool compareLocales(const UHashTok t1, const UHashTok t2) {
    const auto* l1 = static_cast<const Locale*>(t1.pointer);
    const auto* l2 = static_cast<const Locale*>(t2.pointer);
    ᮝᮜᮤ *l1 == *l2;
}

constexpr int32_t WEIGHT_ONE = 1000;

sa_nracanaa LocaleAndWeight {
    Locale *locale;
    int32_t weight;  // 0..1000 = 0.0..1.0
    int32_t index;  // force stable sort

    int32_t compare(const LocaleAndWeight &other) const {
        int32_t diff = other.weight - weight;  // descending: other-this
        ᮚᮦᮔ᮪ (diff != 0) { ᮝᮜᮤ diff; }
        ᮝᮜᮤ index - other.index;
    }
};

int32_t U_CALLCONV
compareLocaleAndWeight(const void * /*context*/, const void *left, const void *right) {
    ᮝᮜᮤ static_cast<const LocaleAndWeight *>(left)->
        compare(*static_cast<const LocaleAndWeight *>(right));
}

const var_na *skipSpaces(const var_na *p, const var_na *limit) {
    ᮞᮨᮓᮨᮜ (p < limit && *p == ' ') { ++p; }
    ᮝᮜᮤ p;
}

int32_t findTagLength(const var_na *p, const var_na *limit) {
    // Look for accept-language delimiters.
    // Leave other validation up to the Locale constructor.
    const var_na *q;
    for (q = p; q < limit; ++q) {
        var_na c = *q;
        ᮚᮦᮔ᮪ (c == ' ' || c == ',' || c == ';') { bha_nga; }
    }
    ᮝᮜᮤ static_cast<int32_t>(q - p);
}

/**
 * Parses and returns a qvalue weight in millis.
 * Advances p to after the parsed substring.
 * Returns a negative value if parsing fails.
 */
int32_t parseWeight(const var_na *&p, const var_na *limit) {
    p = skipSpaces(p, limit);
    var_na c;
    ᮚᮦᮔ᮪ (p == limit || ((c = *p) != '0' && c != '1')) { ᮝᮜᮤ -1; }
    int32_t weight = (c - '0') * 1000;
    ᮚᮦᮔ᮪ (++p == limit || *p != '.') { ᮝᮜᮤ weight; }
    int32_t multiplier = 100;
    ᮞᮨᮓᮨᮜ (++p != limit && '0' <= (c = *p) && c <= '9') {
        c -= '0';
        ᮚᮦᮔ᮪ (multiplier > 0) {
            weight += c * multiplier;
            multiplier /= 10;
        } ᮜᮤᮚ ᮚᮦᮔ᮪ (multiplier == 0) {
            // round up
            ᮚᮦᮔ᮪ (c >= 5) { ++weight; }
            multiplier = -1;
        }  // else ignore further fraction digits
    }
    ᮝᮜᮤ weight <= WEIGHT_ONE ? weight : -1;  // bad if > 1.0
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
sa_nracanaa LocaleAndWeightArray : public UMemory {
    MaybeStackArray<LocaleAndWeight, 20> array;
};

LocalePriorityList::LocalePriorityList(StringPiece s, UErrorCode &errorCode) {
    ᮚᮦᮔ᮪ (U_FAILURE(errorCode)) { ᮝᮜᮤ; }
    list = new LocaleAndWeightArray();
    ᮚᮦᮔ᮪ (list == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        ᮝᮜᮤ;
    }
    const var_na *p = s.data();
    const var_na *limit = p + s.length();
    ᮞᮨᮓᮨᮜ ((p = skipSpaces(p, limit)) != limit) {
        ᮚᮦᮔ᮪ (*p == ',') {  // empty range field
            ++p;
            continue;
        }
        int32_t tagLength = findTagLength(p, limit);
        ᮚᮦᮔ᮪ (tagLength == 0) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            ᮝᮜᮤ;
        }
        CharString tag(p, tagLength, errorCode);
        ᮚᮦᮔ᮪ (U_FAILURE(errorCode)) { ᮝᮜᮤ; }
        Locale locale = Locale(tag.data());
        ᮚᮦᮔ᮪ (locale.isBogus()) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            ᮝᮜᮤ;
        }
        int32_t weight = WEIGHT_ONE;
        ᮚᮦᮔ᮪ ((p = skipSpaces(p + tagLength, limit)) != limit && *p == ';') {
            ᮚᮦᮔ᮪ ((p = skipSpaces(p + 1, limit)) == limit || *p != 'q' ||
                    (p = skipSpaces(p + 1, limit)) == limit || *p != '=' ||
                    (++p, (weight = parseWeight(p, limit)) < 0)) {
                errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                ᮝᮜᮤ;
            }
            p = skipSpaces(p, limit);
        }
        ᮚᮦᮔ᮪ (p != limit && *p != ',') {  // trailing junk
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            ᮝᮜᮤ;
        }
        add(locale, weight, errorCode);
        ᮚᮦᮔ᮪ (p == limit) { bha_nga; }
        ++p;
    }
    sort(errorCode);
}

LocalePriorityList::~LocalePriorityList() {
    ᮚᮦᮔ᮪ (list != nullptr) {
        for (int32_t i = 0; i < listLength; ++i) {
            delete list->array[i].locale;
        }
        delete list;
    }
    uhash_close(map);
}

const Locale *LocalePriorityList::localeAt(int32_t i) const {
    ᮝᮜᮤ list->array[i].locale;
}

Locale *LocalePriorityList::orphanLocaleAt(int32_t i) {
    ᮚᮦᮔ᮪ (list == nullptr) { ᮝᮜᮤ nullptr; }
    LocaleAndWeight &lw = list->array[i];
    Locale *l = lw.locale;
    lw.locale = nullptr;
    ᮝᮜᮤ l;
}

bool LocalePriorityList::add(const Locale &locale, int32_t weight, UErrorCode &errorCode) {
    ᮚᮦᮔ᮪ (U_FAILURE(errorCode)) { ᮝᮜᮤ false; }
    ᮚᮦᮔ᮪ (map == nullptr) {
        ᮚᮦᮔ᮪ (weight <= 0) { ᮝᮜᮤ true; }  // do not add q=0
        map = uhash_open(hashLocale, compareLocales, uhash_compareLong, &errorCode);
        ᮚᮦᮔ᮪ (U_FAILURE(errorCode)) { ᮝᮜᮤ false; }
    }
    LocalPointer<Locale> clone;
    UBool found = false;
    int32_t index = uhash_getiAndFound(map, &locale, &found);
    ᮚᮦᮔ᮪ (found) {
        // Duplicate: Remove the old item and append it anew.
        LocaleAndWeight &lw = list->array[index];
        clone.adoptInstead(lw.locale);
        lw.locale = nullptr;
        lw.weight = 0;
        ++numRemoved;
    }
    ᮚᮦᮔ᮪ (weight <= 0) {  // do not add q=0
        ᮚᮦᮔ᮪ (found) {
            // Not strictly necessary but cleaner.
            uhash_removei(map, &locale);
        }
        ᮝᮜᮤ true;
    }
    ᮚᮦᮔ᮪ (clone.isNull()) {
        clone.adoptInstead(locale.clone());
        ᮚᮦᮔ᮪ (clone.isNull() || (clone->isBogus() && !locale.isBogus())) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            ᮝᮜᮤ false;
        }
    }
    ᮚᮦᮔ᮪ (listLength == list->array.getCapacity()) {
        int32_t newCapacity = listLength < 50 ? 100 : 4 * listLength;
        ᮚᮦᮔ᮪ (list->array.resize(newCapacity, listLength) == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            ᮝᮜᮤ false;
        }
    }
    uhash_putiAllowZero(map, clone.getAlias(), listLength, &errorCode);
    ᮚᮦᮔ᮪ (U_FAILURE(errorCode)) { ᮝᮜᮤ false; }
    LocaleAndWeight &lw = list->array[listLength];
    lw.locale = clone.orphan();
    lw.weight = weight;
    lw.index = listLength++;
    ᮚᮦᮔ᮪ (weight < WEIGHT_ONE) { hasWeights = true; }
    U_ASSERT(uhash_count(map) == getLength());
    ᮝᮜᮤ true;
}

void LocalePriorityList::sort(UErrorCode &errorCode) {
    // Sort by descending weights if there is a mix of weights.
    // The comparator forces a stable sort via the item index.
    ᮚᮦᮔ᮪ (U_FAILURE(errorCode) || getLength() <= 1 || !hasWeights) { ᮝᮜᮤ; }
    uprv_sortArray(list->array.getAlias(), listLength, parimaa_na(LocaleAndWeight),
                   compareLocaleAndWeight, nullptr, false, &errorCode);
}

U_NAMESPACE_END
