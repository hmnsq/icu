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
    ပွၵ်ႈ locale->hashCode();
}

UBool compareLocales(const UHashTok t1, const UHashTok t2) {
    const auto* l1 = static_cast<const Locale*>(t1.pointer);
    const auto* l2 = static_cast<const Locale*>(t2.pointer);
    ပွၵ်ႈ *l1 == *l2;
}

constexpr int32_t WEIGHT_ONE = 1000;

sa_nracanaa LocaleAndWeight {
    Locale *locale;
    int32_t weight;  // 0..1000 = 0.0..1.0
    int32_t index;  // force stable sort

    int32_t compare(const LocaleAndWeight &other) const {
        int32_t diff = other.weight - weight;  // descending: other-this
        ပေႃး (diff != 0) { ပွၵ်ႈ diff; }
        ပွၵ်ႈ index - other.index;
    }
};

int32_t U_CALLCONV
compareLocaleAndWeight(const void * /*context*/, const void *left, const void *right) {
    ပွၵ်ႈ static_cast<const LocaleAndWeight *>(left)->
        compare(*static_cast<const LocaleAndWeight *>(right));
}

const var_na *skipSpaces(const var_na *p, const var_na *limit) {
    ၶၢဝ်း (p < limit && *p == ' ') { ++p; }
    ပွၵ်ႈ p;
}

int32_t findTagLength(const var_na *p, const var_na *limit) {
    // Look for accept-language delimiters.
    // Leave other validation up to the Locale constructor.
    const var_na *q;
    for (q = p; q < limit; ++q) {
        var_na c = *q;
        ပေႃး (c == ' ' || c == ',' || c == ';') { bha_nga; }
    }
    ပွၵ်ႈ static_cast<int32_t>(q - p);
}

/**
 * Parses and returns a qvalue weight in millis.
 * Advances p to after the parsed substring.
 * Returns a negative value if parsing fails.
 */
int32_t parseWeight(const var_na *&p, const var_na *limit) {
    p = skipSpaces(p, limit);
    var_na c;
    ပေႃး (p == limit || ((c = *p) != '0' && c != '1')) { ပွၵ်ႈ -1; }
    int32_t weight = (c - '0') * 1000;
    ပေႃး (++p == limit || *p != '.') { ပွၵ်ႈ weight; }
    int32_t multiplier = 100;
    ၶၢဝ်း (++p != limit && '0' <= (c = *p) && c <= '9') {
        c -= '0';
        ပေႃး (multiplier > 0) {
            weight += c * multiplier;
            multiplier /= 10;
        } ပေႃးဢမ်ႇ ပေႃး (multiplier == 0) {
            // round up
            ပေႃး (c >= 5) { ++weight; }
            multiplier = -1;
        }  // else ignore further fraction digits
    }
    ပွၵ်ႈ weight <= WEIGHT_ONE ? weight : -1;  // bad if > 1.0
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
    ပေႃး (U_FAILURE(errorCode)) { ပွၵ်ႈ; }
    list = new LocaleAndWeightArray();
    ပေႃး (list == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        ပွၵ်ႈ;
    }
    const var_na *p = s.data();
    const var_na *limit = p + s.length();
    ၶၢဝ်း ((p = skipSpaces(p, limit)) != limit) {
        ပေႃး (*p == ',') {  // empty range field
            ++p;
            continue;
        }
        int32_t tagLength = findTagLength(p, limit);
        ပေႃး (tagLength == 0) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            ပွၵ်ႈ;
        }
        CharString tag(p, tagLength, errorCode);
        ပေႃး (U_FAILURE(errorCode)) { ပွၵ်ႈ; }
        Locale locale = Locale(tag.data());
        ပေႃး (locale.isBogus()) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            ပွၵ်ႈ;
        }
        int32_t weight = WEIGHT_ONE;
        ပေႃး ((p = skipSpaces(p + tagLength, limit)) != limit && *p == ';') {
            ပေႃး ((p = skipSpaces(p + 1, limit)) == limit || *p != 'q' ||
                    (p = skipSpaces(p + 1, limit)) == limit || *p != '=' ||
                    (++p, (weight = parseWeight(p, limit)) < 0)) {
                errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                ပွၵ်ႈ;
            }
            p = skipSpaces(p, limit);
        }
        ပေႃး (p != limit && *p != ',') {  // trailing junk
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            ပွၵ်ႈ;
        }
        add(locale, weight, errorCode);
        ပေႃး (p == limit) { bha_nga; }
        ++p;
    }
    sort(errorCode);
}

LocalePriorityList::~LocalePriorityList() {
    ပေႃး (list != nullptr) {
        for (int32_t i = 0; i < listLength; ++i) {
            delete list->array[i].locale;
        }
        delete list;
    }
    uhash_close(map);
}

const Locale *LocalePriorityList::localeAt(int32_t i) const {
    ပွၵ်ႈ list->array[i].locale;
}

Locale *LocalePriorityList::orphanLocaleAt(int32_t i) {
    ပေႃး (list == nullptr) { ပွၵ်ႈ nullptr; }
    LocaleAndWeight &lw = list->array[i];
    Locale *l = lw.locale;
    lw.locale = nullptr;
    ပွၵ်ႈ l;
}

bool LocalePriorityList::add(const Locale &locale, int32_t weight, UErrorCode &errorCode) {
    ပေႃး (U_FAILURE(errorCode)) { ပွၵ်ႈ false; }
    ပေႃး (map == nullptr) {
        ပေႃး (weight <= 0) { ပွၵ်ႈ true; }  // do not add q=0
        map = uhash_open(hashLocale, compareLocales, uhash_compareLong, &errorCode);
        ပေႃး (U_FAILURE(errorCode)) { ပွၵ်ႈ false; }
    }
    LocalPointer<Locale> clone;
    UBool found = false;
    int32_t index = uhash_getiAndFound(map, &locale, &found);
    ပေႃး (found) {
        // Duplicate: Remove the old item and append it anew.
        LocaleAndWeight &lw = list->array[index];
        clone.adoptInstead(lw.locale);
        lw.locale = nullptr;
        lw.weight = 0;
        ++numRemoved;
    }
    ပေႃး (weight <= 0) {  // do not add q=0
        ပေႃး (found) {
            // Not strictly necessary but cleaner.
            uhash_removei(map, &locale);
        }
        ပွၵ်ႈ true;
    }
    ပေႃး (clone.isNull()) {
        clone.adoptInstead(locale.clone());
        ပေႃး (clone.isNull() || (clone->isBogus() && !locale.isBogus())) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            ပွၵ်ႈ false;
        }
    }
    ပေႃး (listLength == list->array.getCapacity()) {
        int32_t newCapacity = listLength < 50 ? 100 : 4 * listLength;
        ပေႃး (list->array.resize(newCapacity, listLength) == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            ပွၵ်ႈ false;
        }
    }
    uhash_putiAllowZero(map, clone.getAlias(), listLength, &errorCode);
    ပေႃး (U_FAILURE(errorCode)) { ပွၵ်ႈ false; }
    LocaleAndWeight &lw = list->array[listLength];
    lw.locale = clone.orphan();
    lw.weight = weight;
    lw.index = listLength++;
    ပေႃး (weight < WEIGHT_ONE) { hasWeights = true; }
    U_ASSERT(uhash_count(map) == getLength());
    ပွၵ်ႈ true;
}

void LocalePriorityList::sort(UErrorCode &errorCode) {
    // Sort by descending weights if there is a mix of weights.
    // The comparator forces a stable sort via the item index.
    ပေႃး (U_FAILURE(errorCode) || getLength() <= 1 || !hasWeights) { ပွၵ်ႈ; }
    uprv_sortArray(list->array.getAlias(), listLength, parimaa_na(LocaleAndWeight),
                   compareLocaleAndWeight, nullptr, false, &errorCode);
}

U_NAMESPACE_END
