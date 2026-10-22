// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// localeprioritylist.cpp
// created: 2019jul11 Markus W. Scherer

#অন্তর্ভাৱ "unicode/utypes.h"
#অন্তর্ভাৱ "unicode/localpointer.h"
#অন্তর্ভাৱ "unicode/locid.h"
#অন্তর্ভাৱ "unicode/stringpiece.h"
#অন্তর্ভাৱ "unicode/uobject.h"
#অন্তর্ভাৱ "charstr.h"
#অন্তর্ভাৱ "cmemory.h"
#অন্তর্ভাৱ "localeprioritylist.h"
#অন্তর্ভাৱ "uarrsort.h"
#অন্তর্ভাৱ "uassert.h"
#অন্তর্ভাৱ "uhash.h"

U_NAMESPACE_BEGIN

নামাকাশ {

int32_t hashLocale(ধ্রুৱ UHashTok token) {
    ধ্রুৱ auto* locale = static_cast<ধ্রুৱ Locale*>(token.pointer);
    ঘূৰাই locale->hashCode();
}

UBool compareLocales(ধ্রুৱ UHashTok t1, ধ্রুৱ UHashTok t2) {
    ধ্রুৱ auto* l1 = static_cast<ধ্রুৱ Locale*>(t1.pointer);
    ধ্রুৱ auto* l2 = static_cast<ধ্রুৱ Locale*>(t2.pointer);
    ঘূৰাই *l1 == *l2;
}

constexpr int32_t WEIGHT_ONE = 1000;

সংরচনা LocaleAndWeight {
    Locale *locale;
    int32_t weight;  // 0..1000 = 0.0..1.0
    int32_t index;  // force stable sort

    int32_t compare(ধ্রুৱ LocaleAndWeight &other) ধ্রুৱ {
        int32_t diff = other.weight - weight;  // descending: other-this
        যদি (diff != 0) { ঘূৰাই diff; }
        ঘূৰাই index - other.index;
    }
};

int32_t U_CALLCONV
compareLocaleAndWeight(ধ্রুৱ শূন্য * /*context*/, ধ্রুৱ শূন্য *left, ধ্রুৱ শূন্য *ডান) {
    ঘূৰাই static_cast<ধ্রুৱ LocaleAndWeight *>(left)->
        compare(*static_cast<ধ্রুৱ LocaleAndWeight *>(ডান));
}

ধ্রুৱ ৱর্ণ *skipSpaces(ধ্রুৱ ৱর্ণ *p, ধ্রুৱ ৱর্ণ *limit) {
    যেতিয়ালৈ (p < limit && *p == ' ') { ++p; }
    ঘূৰাই p;
}

int32_t findTagLength(ধ্রুৱ ৱর্ণ *p, ধ্রুৱ ৱর্ণ *limit) {
    // Look for accept-language delimiters.
    // Leave other validation up to the Locale constructor.
    ধ্রুৱ ৱর্ণ *q;
    বাবে (q = p; q < limit; ++q) {
        ৱর্ণ c = *q;
        যদি (c == ' ' || c == ',' || c == ';') { ভঙ্গ; }
    }
    ঘূৰাই static_cast<int32_t>(q - p);
}

/**
 * Parses and returns a qvalue weight in millis.
 * Advances p to after the parsed substring.
 * Returns a negative value if parsing fails.
 */
int32_t parseWeight(ধ্রুৱ ৱর্ণ *&p, ধ্রুৱ ৱর্ণ *limit) {
    p = skipSpaces(p, limit);
    ৱর্ণ c;
    যদি (p == limit || ((c = *p) != '0' && c != '1')) { ঘূৰাই -1; }
    int32_t weight = (c - '0') * 1000;
    যদি (++p == limit || *p != '.') { ঘূৰাই weight; }
    int32_t multiplier = 100;
    যেতিয়ালৈ (++p != limit && '0' <= (c = *p) && c <= '9') {
        c -= '0';
        যদি (multiplier > 0) {
            weight += c * multiplier;
            multiplier /= 10;
        } অন্যথা যদি (multiplier == 0) {
            // round up
            যদি (c >= 5) { ++weight; }
            multiplier = -1;
        }  // else ignore further fraction digits
    }
    ঘূৰাই weight <= WEIGHT_ONE ? weight : -1;  // bad if > 1.0
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
সংরচনা LocaleAndWeightArray : সার্ৱজনিক UMemory {
    Maybe_স্তূপ_Array<LocaleAndWeight, 20> array;
};

LocalePriorityList::LocalePriorityList(StringPiece s, UErrorCode &errorCode) {
    যদি (U_FAILURE(errorCode)) { ঘূৰাই; }
    list = নূতন LocaleAndWeightArray();
    যদি (list == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        ঘূৰাই;
    }
    ধ্রুৱ ৱর্ণ *p = s.data();
    ধ্রুৱ ৱর্ণ *limit = p + s.length();
    যেতিয়ালৈ ((p = skipSpaces(p, limit)) != limit) {
        যদি (*p == ',') {  // empty range field
            ++p;
            অনুৱর্ত;
        }
        int32_t tagLength = findTagLength(p, limit);
        যদি (tagLength == 0) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            ঘূৰাই;
        }
        CharString tag(p, tagLength, errorCode);
        যদি (U_FAILURE(errorCode)) { ঘূৰাই; }
        Locale locale = Locale(tag.data());
        যদি (locale.isBogus()) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            ঘূৰাই;
        }
        int32_t weight = WEIGHT_ONE;
        যদি ((p = skipSpaces(p + tagLength, limit)) != limit && *p == ';') {
            যদি ((p = skipSpaces(p + 1, limit)) == limit || *p != 'q' ||
                    (p = skipSpaces(p + 1, limit)) == limit || *p != '=' ||
                    (++p, (weight = parseWeight(p, limit)) < 0)) {
                errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                ঘূৰাই;
            }
            p = skipSpaces(p, limit);
        }
        যদি (p != limit && *p != ',') {  // trailing junk
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            ঘূৰাই;
        }
        add(locale, weight, errorCode);
        যদি (p == limit) { ভঙ্গ; }
        ++p;
    }
    sort(errorCode);
}

LocalePriorityList::~LocalePriorityList() {
    যদি (list != nullptr) {
        বাবে (int32_t i = 0; i < listLength; ++i) {
            delete list->array[i].locale;
        }
        delete list;
    }
    uhash_close(map);
}

ধ্রুৱ Locale *LocalePriorityList::localeAt(int32_t i) ধ্রুৱ {
    ঘূৰাই list->array[i].locale;
}

Locale *LocalePriorityList::orphanLocaleAt(int32_t i) {
    যদি (list == nullptr) { ঘূৰাই nullptr; }
    LocaleAndWeight &lw = list->array[i];
    Locale *l = lw.locale;
    lw.locale = nullptr;
    ঘূৰাই l;
}

তার্কিক LocalePriorityList::add(ধ্রুৱ Locale &locale, int32_t weight, UErrorCode &errorCode) {
    যদি (U_FAILURE(errorCode)) { ঘূৰাই মিছা; }
    যদি (map == nullptr) {
        যদি (weight <= 0) { ঘূৰাই সঁচা; }  // do not add q=0
        map = uhash_open(hashLocale, compareLocales, uhash_compareLong, &errorCode);
        যদি (U_FAILURE(errorCode)) { ঘূৰাই মিছা; }
    }
    LocalPointer<Locale> clone;
    UBool found = মিছা;
    int32_t index = uhash_getiAndFound(map, &locale, &found);
    যদি (found) {
        // Duplicate: Remove the old item and append it anew.
        LocaleAndWeight &lw = list->array[index];
        clone.adoptInstead(lw.locale);
        lw.locale = nullptr;
        lw.weight = 0;
        ++অঙ্ক_Removed;
    }
    যদি (weight <= 0) {  // do not add q=0
        যদি (found) {
            // Not strictly necessary but cleaner.
            uhash_removei(map, &locale);
        }
        ঘূৰাই সঁচা;
    }
    যদি (clone.isNull()) {
        clone.adoptInstead(locale.clone());
        যদি (clone.isNull() || (clone->isBogus() && !locale.isBogus())) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            ঘূৰাই মিছা;
        }
    }
    যদি (listLength == list->array.getCapacity()) {
        int32_t newCapacity = listLength < 50 ? 100 : 4 * listLength;
        যদি (list->array.resize(newCapacity, listLength) == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            ঘূৰাই মিছা;
        }
    }
    uhash_putiAllowZero(map, clone.getAlias(), listLength, &errorCode);
    যদি (U_FAILURE(errorCode)) { ঘূৰাই মিছা; }
    LocaleAndWeight &lw = list->array[listLength];
    lw.locale = clone.orphan();
    lw.weight = weight;
    lw.index = listLength++;
    যদি (weight < WEIGHT_ONE) { hasWeights = সঁচা; }
    U_ASSERT(uhash_গুনতি(map) == getLength());
    ঘূৰাই সঁচা;
}

শূন্য LocalePriorityList::sort(UErrorCode &errorCode) {
    // Sort by descending weights if there is a mix of weights.
    // The comparator forces a stable sort via the item index.
    যদি (U_FAILURE(errorCode) || getLength() <= 1 || !hasWeights) { ঘূৰাই; }
    uprv_sortArray(list->array.getAlias(), listLength, পরিমাণ(LocaleAndWeight),
                   compareLocaleAndWeight, nullptr, মিছা, &errorCode);
}

U_NAMESPACE_END
