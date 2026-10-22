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
    вернуть locale->hashCode();
}

UBool compareLocales(const UHashTok t1, const UHashTok t2) {
    const auto* l1 = static_cast<const Locale*>(t1.pointer);
    const auto* l2 = static_cast<const Locale*>(t2.pointer);
    вернуть *l1 == *l2;
}

constexpr int32_t WEIGHT_ONE = 1000;

struct LocaleAndWeight {
    Locale *locale;
    int32_t weight;  // 0..1000 = 0.0..1.0
    int32_t index;  // force stable sort

    int32_t compare(const LocaleAndWeight &other) const {
        int32_t diff = other.weight - weight;  // descending: other-this
        если (diff != 0) { вернуть diff; }
        вернуть index - other.index;
    }
};

int32_t U_CALLCONV
compareLocaleAndWeight(const void * /*context*/, const void *left, const void *right) {
    вернуть static_cast<const LocaleAndWeight *>(left)->
        compare(*static_cast<const LocaleAndWeight *>(right));
}

const char *skipSpaces(const char *p, const char *limit) {
    пока (p < limit && *p == ' ') { ++p; }
    вернуть p;
}

int32_t findTagLength(const char *p, const char *limit) {
    // Look for accept-language delimiters.
    // Leave other validation up to the Locale constructor.
    const char *q;
    для (q = p; q < limit; ++q) {
        char c = *q;
        если (c == ' ' || c == ',' || c == ';') { break; }
    }
    вернуть static_cast<int32_t>(q - p);
}

/**
 * Parses and returns a qvalue weight in millis.
 * Advances p to after the parsed substring.
 * Returns a negative value if parsing fails.
 */
int32_t parseWeight(const char *&p, const char *limit) {
    p = skipSpaces(p, limit);
    char c;
    если (p == limit || ((c = *p) != '0' && c != '1')) { вернуть -1; }
    int32_t weight = (c - '0') * 1000;
    если (++p == limit || *p != '.') { вернуть weight; }
    int32_t multiplier = 100;
    пока (++p != limit && '0' <= (c = *p) && c <= '9') {
        c -= '0';
        если (multiplier > 0) {
            weight += c * multiplier;
            multiplier /= 10;
        } иначе если (multiplier == 0) {
            // round up
            если (c >= 5) { ++weight; }
            multiplier = -1;
        }  // else ignore further fraction digits
    }
    вернуть weight <= WEIGHT_ONE ? weight : -1;  // bad if > 1.0
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
struct LocaleAndWeightArray : public UMemory {
    MaybeStackArray<LocaleAndWeight, 20> array;
};

LocalePriorityList::LocalePriorityList(StringPiece s, UErrorCode &errorCode) {
    если (U_FAILURE(errorCode)) { вернуть; }
    list = new LocaleAndWeightArray();
    если (list == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        вернуть;
    }
    const char *p = s.data();
    const char *limit = p + s.length();
    пока ((p = skipSpaces(p, limit)) != limit) {
        если (*p == ',') {  // empty range field
            ++p;
            continue;
        }
        int32_t tagLength = findTagLength(p, limit);
        если (tagLength == 0) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            вернуть;
        }
        CharString tag(p, tagLength, errorCode);
        если (U_FAILURE(errorCode)) { вернуть; }
        Locale locale = Locale(tag.data());
        если (locale.isBogus()) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            вернуть;
        }
        int32_t weight = WEIGHT_ONE;
        если ((p = skipSpaces(p + tagLength, limit)) != limit && *p == ';') {
            если ((p = skipSpaces(p + 1, limit)) == limit || *p != 'q' ||
                    (p = skipSpaces(p + 1, limit)) == limit || *p != '=' ||
                    (++p, (weight = parseWeight(p, limit)) < 0)) {
                errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                вернуть;
            }
            p = skipSpaces(p, limit);
        }
        если (p != limit && *p != ',') {  // trailing junk
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            вернуть;
        }
        add(locale, weight, errorCode);
        если (p == limit) { break; }
        ++p;
    }
    sort(errorCode);
}

LocalePriorityList::~LocalePriorityList() {
    если (list != nullptr) {
        для (int32_t i = 0; i < listLength; ++i) {
            delete list->array[i].locale;
        }
        delete list;
    }
    uhash_close(map);
}

const Locale *LocalePriorityList::localeAt(int32_t i) const {
    вернуть list->array[i].locale;
}

Locale *LocalePriorityList::orphanLocaleAt(int32_t i) {
    если (list == nullptr) { вернуть nullptr; }
    LocaleAndWeight &lw = list->array[i];
    Locale *l = lw.locale;
    lw.locale = nullptr;
    вернуть l;
}

bool LocalePriorityList::add(const Locale &locale, int32_t weight, UErrorCode &errorCode) {
    если (U_FAILURE(errorCode)) { вернуть false; }
    если (map == nullptr) {
        если (weight <= 0) { вернуть true; }  // do not add q=0
        map = uhash_open(hashLocale, compareLocales, uhash_compareLong, &errorCode);
        если (U_FAILURE(errorCode)) { вернуть false; }
    }
    LocalPointer<Locale> clone;
    UBool found = false;
    int32_t index = uhash_getiAndFound(map, &locale, &found);
    если (found) {
        // Duplicate: Remove the old item and append it anew.
        LocaleAndWeight &lw = list->array[index];
        clone.adoptInstead(lw.locale);
        lw.locale = nullptr;
        lw.weight = 0;
        ++numRemoved;
    }
    если (weight <= 0) {  // do not add q=0
        если (found) {
            // Not strictly necessary but cleaner.
            uhash_removei(map, &locale);
        }
        вернуть true;
    }
    если (clone.isNull()) {
        clone.adoptInstead(locale.clone());
        если (clone.isNull() || (clone->isBogus() && !locale.isBogus())) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            вернуть false;
        }
    }
    если (listLength == list->array.getCapacity()) {
        int32_t newCapacity = listLength < 50 ? 100 : 4 * listLength;
        если (list->array.resize(newCapacity, listLength) == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            вернуть false;
        }
    }
    uhash_putiAllowZero(map, clone.getAlias(), listLength, &errorCode);
    если (U_FAILURE(errorCode)) { вернуть false; }
    LocaleAndWeight &lw = list->array[listLength];
    lw.locale = clone.orphan();
    lw.weight = weight;
    lw.index = listLength++;
    если (weight < WEIGHT_ONE) { hasWeights = true; }
    U_ASSERT(uhash_count(map) == getLength());
    вернуть true;
}

void LocalePriorityList::sort(UErrorCode &errorCode) {
    // Sort by descending weights if there is a mix of weights.
    // The comparator forces a stable sort via the item index.
    если (U_FAILURE(errorCode) || getLength() <= 1 || !hasWeights) { вернуть; }
    uprv_sortArray(list->array.getAlias(), listLength, sizeof(LocaleAndWeight),
                   compareLocaleAndWeight, nullptr, false, &errorCode);
}

U_NAMESPACE_END
