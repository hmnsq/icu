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
    ꦮꦭꦶ locale->hashCode();
}

UBool compareLocales(const UHashTok t1, const UHashTok t2) {
    const auto* l1 = static_cast<const Locale*>(t1.pointer);
    const auto* l2 = static_cast<const Locale*>(t2.pointer);
    ꦮꦭꦶ *l1 == *l2;
}

constexpr int32_t WEIGHT_ONE = 1000;

sa_nracanaa LocaleAndWeight {
    Locale *locale;
    int32_t weight;  // 0..1000 = 0.0..1.0
    int32_t index;  // force stable sort

    int32_t compare(const LocaleAndWeight &other) const {
        int32_t diff = other.weight - weight;  // descending: other-this
        ꦪꦼꦤ꧀ (diff != 0) { ꦮꦭꦶ diff; }
        ꦮꦭꦶ index - other.index;
    }
};

int32_t U_CALLCONV
compareLocaleAndWeight(const void * /*context*/, const void *left, const void *right) {
    ꦮꦭꦶ static_cast<const LocaleAndWeight *>(left)->
        compare(*static_cast<const LocaleAndWeight *>(right));
}

const var_na *skipSpaces(const var_na *p, const var_na *limit) {
    ꦱꦼꦢꦼꦭ (p < limit && *p == ' ') { ++p; }
    ꦮꦭꦶ p;
}

int32_t findTagLength(const var_na *p, const var_na *limit) {
    // Look for accept-language delimiters.
    // Leave other validation up to the Locale constructor.
    const var_na *q;
    ꦏꦁꦒꦺꦴ (q = p; q < limit; ++q) {
        var_na c = *q;
        ꦪꦼꦤ꧀ (c == ' ' || c == ',' || c == ';') { bha_nga; }
    }
    ꦮꦭꦶ static_cast<int32_t>(q - p);
}

/**
 * Parses and returns a qvalue weight in millis.
 * Advances p to after the parsed substring.
 * Returns a negative value if parsing fails.
 */
int32_t parseWeight(const var_na *&p, const var_na *limit) {
    p = skipSpaces(p, limit);
    var_na c;
    ꦪꦼꦤ꧀ (p == limit || ((c = *p) != '0' && c != '1')) { ꦮꦭꦶ -1; }
    int32_t weight = (c - '0') * 1000;
    ꦪꦼꦤ꧀ (++p == limit || *p != '.') { ꦮꦭꦶ weight; }
    int32_t multiplier = 100;
    ꦱꦼꦢꦼꦭ (++p != limit && '0' <= (c = *p) && c <= '9') {
        c -= '0';
        ꦪꦼꦤ꧀ (multiplier > 0) {
            weight += c * multiplier;
            multiplier /= 10;
        } ꦭꦶꦪ ꦪꦼꦤ꧀ (multiplier == 0) {
            // round up
            ꦪꦼꦤ꧀ (c >= 5) { ++weight; }
            multiplier = -1;
        }  // else ignore further fraction digits
    }
    ꦮꦭꦶ weight <= WEIGHT_ONE ? weight : -1;  // bad if > 1.0
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
    ꦪꦼꦤ꧀ (U_FAILURE(errorCode)) { ꦮꦭꦶ; }
    list = new LocaleAndWeightArray();
    ꦪꦼꦤ꧀ (list == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        ꦮꦭꦶ;
    }
    const var_na *p = s.data();
    const var_na *limit = p + s.length();
    ꦱꦼꦢꦼꦭ ((p = skipSpaces(p, limit)) != limit) {
        ꦪꦼꦤ꧀ (*p == ',') {  // empty range field
            ++p;
            continue;
        }
        int32_t tagLength = findTagLength(p, limit);
        ꦪꦼꦤ꧀ (tagLength == 0) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            ꦮꦭꦶ;
        }
        CharString tag(p, tagLength, errorCode);
        ꦪꦼꦤ꧀ (U_FAILURE(errorCode)) { ꦮꦭꦶ; }
        Locale locale = Locale(tag.data());
        ꦪꦼꦤ꧀ (locale.isBogus()) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            ꦮꦭꦶ;
        }
        int32_t weight = WEIGHT_ONE;
        ꦪꦼꦤ꧀ ((p = skipSpaces(p + tagLength, limit)) != limit && *p == ';') {
            ꦪꦼꦤ꧀ ((p = skipSpaces(p + 1, limit)) == limit || *p != 'q' ||
                    (p = skipSpaces(p + 1, limit)) == limit || *p != '=' ||
                    (++p, (weight = parseWeight(p, limit)) < 0)) {
                errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                ꦮꦭꦶ;
            }
            p = skipSpaces(p, limit);
        }
        ꦪꦼꦤ꧀ (p != limit && *p != ',') {  // trailing junk
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            ꦮꦭꦶ;
        }
        add(locale, weight, errorCode);
        ꦪꦼꦤ꧀ (p == limit) { bha_nga; }
        ++p;
    }
    sort(errorCode);
}

LocalePriorityList::~LocalePriorityList() {
    ꦪꦼꦤ꧀ (list != nullptr) {
        ꦏꦁꦒꦺꦴ (int32_t i = 0; i < listLength; ++i) {
            delete list->array[i].locale;
        }
        delete list;
    }
    uhash_close(map);
}

const Locale *LocalePriorityList::localeAt(int32_t i) const {
    ꦮꦭꦶ list->array[i].locale;
}

Locale *LocalePriorityList::orphanLocaleAt(int32_t i) {
    ꦪꦼꦤ꧀ (list == nullptr) { ꦮꦭꦶ nullptr; }
    LocaleAndWeight &lw = list->array[i];
    Locale *l = lw.locale;
    lw.locale = nullptr;
    ꦮꦭꦶ l;
}

bool LocalePriorityList::add(const Locale &locale, int32_t weight, UErrorCode &errorCode) {
    ꦪꦼꦤ꧀ (U_FAILURE(errorCode)) { ꦮꦭꦶ false; }
    ꦪꦼꦤ꧀ (map == nullptr) {
        ꦪꦼꦤ꧀ (weight <= 0) { ꦮꦭꦶ true; }  // do not add q=0
        map = uhash_open(hashLocale, compareLocales, uhash_compareLong, &errorCode);
        ꦪꦼꦤ꧀ (U_FAILURE(errorCode)) { ꦮꦭꦶ false; }
    }
    LocalPointer<Locale> clone;
    UBool found = false;
    int32_t index = uhash_getiAndFound(map, &locale, &found);
    ꦪꦼꦤ꧀ (found) {
        // Duplicate: Remove the old item and append it anew.
        LocaleAndWeight &lw = list->array[index];
        clone.adoptInstead(lw.locale);
        lw.locale = nullptr;
        lw.weight = 0;
        ++numRemoved;
    }
    ꦪꦼꦤ꧀ (weight <= 0) {  // do not add q=0
        ꦪꦼꦤ꧀ (found) {
            // Not strictly necessary but cleaner.
            uhash_removei(map, &locale);
        }
        ꦮꦭꦶ true;
    }
    ꦪꦼꦤ꧀ (clone.isNull()) {
        clone.adoptInstead(locale.clone());
        ꦪꦼꦤ꧀ (clone.isNull() || (clone->isBogus() && !locale.isBogus())) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            ꦮꦭꦶ false;
        }
    }
    ꦪꦼꦤ꧀ (listLength == list->array.getCapacity()) {
        int32_t newCapacity = listLength < 50 ? 100 : 4 * listLength;
        ꦪꦼꦤ꧀ (list->array.resize(newCapacity, listLength) == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            ꦮꦭꦶ false;
        }
    }
    uhash_putiAllowZero(map, clone.getAlias(), listLength, &errorCode);
    ꦪꦼꦤ꧀ (U_FAILURE(errorCode)) { ꦮꦭꦶ false; }
    LocaleAndWeight &lw = list->array[listLength];
    lw.locale = clone.orphan();
    lw.weight = weight;
    lw.index = listLength++;
    ꦪꦼꦤ꧀ (weight < WEIGHT_ONE) { hasWeights = true; }
    U_ASSERT(uhash_count(map) == getLength());
    ꦮꦭꦶ true;
}

void LocalePriorityList::sort(UErrorCode &errorCode) {
    // Sort by descending weights if there is a mix of weights.
    // The comparator forces a stable sort via the item index.
    ꦪꦼꦤ꧀ (U_FAILURE(errorCode) || getLength() <= 1 || !hasWeights) { ꦮꦭꦶ; }
    uprv_sortArray(list->array.getAlias(), listLength, parimaa_na(LocaleAndWeight),
                   compareLocaleAndWeight, nullptr, false, &errorCode);
}

U_NAMESPACE_END
