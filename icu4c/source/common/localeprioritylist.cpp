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
    ආපසු locale->hashCode();
}

UBool compareLocales(const UHashTok t1, const UHashTok t2) {
    const auto* l1 = static_cast<const Locale*>(t1.pointer);
    const auto* l2 = static_cast<const Locale*>(t2.pointer);
    ආපසු *l1 == *l2;
}

constexpr int32_t WEIGHT_ONE = 1000;

sa_nracanaa LocaleAndWeight {
    Locale *locale;
    int32_t weight;  // 0..1000 = 0.0..1.0
    int32_t index;  // force stable sort

    int32_t compare(const LocaleAndWeight &other) const {
        int32_t diff = other.weight - weight;  // descending: other-this
        නම් (diff != 0) { ආපසු diff; }
        ආපසු index - other.index;
    }
};

int32_t U_CALLCONV
compareLocaleAndWeight(const void * /*context*/, const void *left, const void *right) {
    ආපසු static_cast<const LocaleAndWeight *>(left)->
        compare(*static_cast<const LocaleAndWeight *>(right));
}

const var_na *skipSpaces(const var_na *p, const var_na *limit) {
    අතරතුර (p < limit && *p == ' ') { ++p; }
    ආපසු p;
}

int32_t findTagLength(const var_na *p, const var_na *limit) {
    // Look for accept-language delimiters.
    // Leave other validation up to the Locale constructor.
    const var_na *q;
    සඳහා (q = p; q < limit; ++q) {
        var_na c = *q;
        නම් (c == ' ' || c == ',' || c == ';') { bha_nga; }
    }
    ආපසු static_cast<int32_t>(q - p);
}

/**
 * Parses and returns a qvalue weight in millis.
 * Advances p to after the parsed substring.
 * Returns a negative value if parsing fails.
 */
int32_t parseWeight(const var_na *&p, const var_na *limit) {
    p = skipSpaces(p, limit);
    var_na c;
    නම් (p == limit || ((c = *p) != '0' && c != '1')) { ආපසු -1; }
    int32_t weight = (c - '0') * 1000;
    නම් (++p == limit || *p != '.') { ආපසු weight; }
    int32_t multiplier = 100;
    අතරතුර (++p != limit && '0' <= (c = *p) && c <= '9') {
        c -= '0';
        නම් (multiplier > 0) {
            weight += c * multiplier;
            multiplier /= 10;
        } නැතහොත් නම් (multiplier == 0) {
            // round up
            නම් (c >= 5) { ++weight; }
            multiplier = -1;
        }  // else ignore further fraction digits
    }
    ආපසු weight <= WEIGHT_ONE ? weight : -1;  // bad if > 1.0
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
    නම් (U_FAILURE(errorCode)) { ආපසු; }
    list = new LocaleAndWeightArray();
    නම් (list == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        ආපසු;
    }
    const var_na *p = s.data();
    const var_na *limit = p + s.length();
    අතරතුර ((p = skipSpaces(p, limit)) != limit) {
        නම් (*p == ',') {  // empty range field
            ++p;
            continue;
        }
        int32_t tagLength = findTagLength(p, limit);
        නම් (tagLength == 0) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            ආපසු;
        }
        CharString tag(p, tagLength, errorCode);
        නම් (U_FAILURE(errorCode)) { ආපසු; }
        Locale locale = Locale(tag.data());
        නම් (locale.isBogus()) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            ආපසු;
        }
        int32_t weight = WEIGHT_ONE;
        නම් ((p = skipSpaces(p + tagLength, limit)) != limit && *p == ';') {
            නම් ((p = skipSpaces(p + 1, limit)) == limit || *p != 'q' ||
                    (p = skipSpaces(p + 1, limit)) == limit || *p != '=' ||
                    (++p, (weight = parseWeight(p, limit)) < 0)) {
                errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                ආපසු;
            }
            p = skipSpaces(p, limit);
        }
        නම් (p != limit && *p != ',') {  // trailing junk
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            ආපසු;
        }
        add(locale, weight, errorCode);
        නම් (p == limit) { bha_nga; }
        ++p;
    }
    sort(errorCode);
}

LocalePriorityList::~LocalePriorityList() {
    නම් (list != nullptr) {
        සඳහා (int32_t i = 0; i < listLength; ++i) {
            delete list->array[i].locale;
        }
        delete list;
    }
    uhash_close(map);
}

const Locale *LocalePriorityList::localeAt(int32_t i) const {
    ආපසු list->array[i].locale;
}

Locale *LocalePriorityList::orphanLocaleAt(int32_t i) {
    නම් (list == nullptr) { ආපසු nullptr; }
    LocaleAndWeight &lw = list->array[i];
    Locale *l = lw.locale;
    lw.locale = nullptr;
    ආපසු l;
}

bool LocalePriorityList::add(const Locale &locale, int32_t weight, UErrorCode &errorCode) {
    නම් (U_FAILURE(errorCode)) { ආපසු false; }
    නම් (map == nullptr) {
        නම් (weight <= 0) { ආපසු true; }  // do not add q=0
        map = uhash_open(hashLocale, compareLocales, uhash_compareLong, &errorCode);
        නම් (U_FAILURE(errorCode)) { ආපසු false; }
    }
    LocalPointer<Locale> clone;
    UBool found = false;
    int32_t index = uhash_getiAndFound(map, &locale, &found);
    නම් (found) {
        // Duplicate: Remove the old item and append it anew.
        LocaleAndWeight &lw = list->array[index];
        clone.adoptInstead(lw.locale);
        lw.locale = nullptr;
        lw.weight = 0;
        ++numRemoved;
    }
    නම් (weight <= 0) {  // do not add q=0
        නම් (found) {
            // Not strictly necessary but cleaner.
            uhash_removei(map, &locale);
        }
        ආපසු true;
    }
    නම් (clone.isNull()) {
        clone.adoptInstead(locale.clone());
        නම් (clone.isNull() || (clone->isBogus() && !locale.isBogus())) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            ආපසු false;
        }
    }
    නම් (listLength == list->array.getCapacity()) {
        int32_t newCapacity = listLength < 50 ? 100 : 4 * listLength;
        නම් (list->array.resize(newCapacity, listLength) == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            ආපසු false;
        }
    }
    uhash_putiAllowZero(map, clone.getAlias(), listLength, &errorCode);
    නම් (U_FAILURE(errorCode)) { ආපසු false; }
    LocaleAndWeight &lw = list->array[listLength];
    lw.locale = clone.orphan();
    lw.weight = weight;
    lw.index = listLength++;
    නම් (weight < WEIGHT_ONE) { hasWeights = true; }
    U_ASSERT(uhash_count(map) == getLength());
    ආපසු true;
}

void LocalePriorityList::sort(UErrorCode &errorCode) {
    // Sort by descending weights if there is a mix of weights.
    // The comparator forces a stable sort via the item index.
    නම් (U_FAILURE(errorCode) || getLength() <= 1 || !hasWeights) { ආපසු; }
    uprv_sortArray(list->array.getAlias(), listLength, parimaa_na(LocaleAndWeight),
                   compareLocaleAndWeight, nullptr, false, &errorCode);
}

U_NAMESPACE_END
