// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// localeprioritylist.cpp
// created: 2019jul11 Markus W. Scherer

#অন্তর্ভাব "unicode/utypes.h"
#অন্তর্ভাব "unicode/localpointer.h"
#অন্তর্ভাব "unicode/locid.h"
#অন্তর্ভাব "unicode/stringpiece.h"
#অন্তর্ভাব "unicode/uobject.h"
#অন্তর্ভাব "charstr.h"
#অন্তর্ভাব "cmemory.h"
#অন্তর্ভাব "localeprioritylist.h"
#অন্তর্ভাব "uarrsort.h"
#অন্তর্ভাব "uassert.h"
#অন্তর্ভাব "uhash.h"

U_NAMESPACE_BEGIN

নামাকাশ {

int32_t hashLocale(ধ্রুব UHashTok token) {
    ধ্রুব auto* locale = static_cast<ধ্রুব Locale*>(token.pointer);
    প্রত্যাগম locale->hashCode();
}

UBool compareLocales(ধ্রুব UHashTok t1, ধ্রুব UHashTok t2) {
    ধ্রুব auto* l1 = static_cast<ধ্রুব Locale*>(t1.pointer);
    ধ্রুব auto* l2 = static_cast<ধ্রুব Locale*>(t2.pointer);
    প্রত্যাগম *l1 == *l2;
}

constexpr int32_t WEIGHT_ONE = 1000;

সংরচনা LocaleAndWeight {
    Locale *locale;
    int32_t weight;  // 0..1000 = 0.0..1.0
    int32_t index;  // force stable sort

    int32_t compare(ধ্রুব LocaleAndWeight &other) ধ্রুব {
        int32_t diff = other.weight - weight;  // descending: other-this
        যদি (diff != 0) { প্রত্যাগম diff; }
        প্রত্যাগম index - other.index;
    }
};

int32_t U_CALLCONV
compareLocaleAndWeight(ধ্রুব শূন্য * /*context*/, ধ্রুব শূন্য *left, ধ্রুব শূন্য *ডান) {
    প্রত্যাগম static_cast<ধ্রুব LocaleAndWeight *>(left)->
        compare(*static_cast<ধ্রুব LocaleAndWeight *>(ডান));
}

ধ্রুব বর্ণ *skipSpaces(ধ্রুব বর্ণ *p, ধ্রুব বর্ণ *limit) {
    যাবত্ (p < limit && *p == ' ') { ++p; }
    প্রত্যাগম p;
}

int32_t findTagLength(ধ্রুব বর্ণ *p, ধ্রুব বর্ণ *limit) {
    // Look for accept-language delimiters.
    // Leave other validation up to the Locale constructor.
    ধ্রুব বর্ণ *q;
    প্রতি (q = p; q < limit; ++q) {
        বর্ণ c = *q;
        যদি (c == ' ' || c == ',' || c == ';') { ভঙ্গ; }
    }
    প্রত্যাগম static_cast<int32_t>(q - p);
}

/**
 * Parses and returns a qvalue weight in millis.
 * Advances p to after the parsed substring.
 * Returns a negative value if parsing fails.
 */
int32_t parseWeight(ধ্রুব বর্ণ *&p, ধ্রুব বর্ণ *limit) {
    p = skipSpaces(p, limit);
    বর্ণ c;
    যদি (p == limit || ((c = *p) != '0' && c != '1')) { প্রত্যাগম -1; }
    int32_t weight = (c - '0') * 1000;
    যদি (++p == limit || *p != '.') { প্রত্যাগম weight; }
    int32_t multiplier = 100;
    যাবত্ (++p != limit && '0' <= (c = *p) && c <= '9') {
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
    প্রত্যাগম weight <= WEIGHT_ONE ? weight : -1;  // bad if > 1.0
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
সংরচনা LocaleAndWeightArray : সার্বজনিক UMemory {
    Maybe_স্তূপ_Array<LocaleAndWeight, 20> array;
};

LocalePriorityList::LocalePriorityList(StringPiece s, UErrorCode &errorCode) {
    যদি (U_FAILURE(errorCode)) { প্রত্যাগম; }
    list = নূতন LocaleAndWeightArray();
    যদি (list == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        প্রত্যাগম;
    }
    ধ্রুব বর্ণ *p = s.data();
    ধ্রুব বর্ণ *limit = p + s.length();
    যাবত্ ((p = skipSpaces(p, limit)) != limit) {
        যদি (*p == ',') {  // empty range field
            ++p;
            অনুবর্ত;
        }
        int32_t tagLength = findTagLength(p, limit);
        যদি (tagLength == 0) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            প্রত্যাগম;
        }
        CharString tag(p, tagLength, errorCode);
        যদি (U_FAILURE(errorCode)) { প্রত্যাগম; }
        Locale locale = Locale(tag.data());
        যদি (locale.isBogus()) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            প্রত্যাগম;
        }
        int32_t weight = WEIGHT_ONE;
        যদি ((p = skipSpaces(p + tagLength, limit)) != limit && *p == ';') {
            যদি ((p = skipSpaces(p + 1, limit)) == limit || *p != 'q' ||
                    (p = skipSpaces(p + 1, limit)) == limit || *p != '=' ||
                    (++p, (weight = parseWeight(p, limit)) < 0)) {
                errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                প্রত্যাগম;
            }
            p = skipSpaces(p, limit);
        }
        যদি (p != limit && *p != ',') {  // trailing junk
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            প্রত্যাগম;
        }
        add(locale, weight, errorCode);
        যদি (p == limit) { ভঙ্গ; }
        ++p;
    }
    sort(errorCode);
}

LocalePriorityList::~LocalePriorityList() {
    যদি (list != nullptr) {
        প্রতি (int32_t i = 0; i < listLength; ++i) {
            delete list->array[i].locale;
        }
        delete list;
    }
    uhash_close(map);
}

ধ্রুব Locale *LocalePriorityList::localeAt(int32_t i) ধ্রুব {
    প্রত্যাগম list->array[i].locale;
}

Locale *LocalePriorityList::orphanLocaleAt(int32_t i) {
    যদি (list == nullptr) { প্রত্যাগম nullptr; }
    LocaleAndWeight &lw = list->array[i];
    Locale *l = lw.locale;
    lw.locale = nullptr;
    প্রত্যাগম l;
}

তার্কিক LocalePriorityList::add(ধ্রুব Locale &locale, int32_t weight, UErrorCode &errorCode) {
    যদি (U_FAILURE(errorCode)) { প্রত্যাগম অসত্য; }
    যদি (map == nullptr) {
        যদি (weight <= 0) { প্রত্যাগম সত্য; }  // do not add q=0
        map = uhash_open(hashLocale, compareLocales, uhash_compareLong, &errorCode);
        যদি (U_FAILURE(errorCode)) { প্রত্যাগম অসত্য; }
    }
    LocalPointer<Locale> clone;
    UBool found = অসত্য;
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
        প্রত্যাগম সত্য;
    }
    যদি (clone.isNull()) {
        clone.adoptInstead(locale.clone());
        যদি (clone.isNull() || (clone->isBogus() && !locale.isBogus())) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            প্রত্যাগম অসত্য;
        }
    }
    যদি (listLength == list->array.getCapacity()) {
        int32_t newCapacity = listLength < 50 ? 100 : 4 * listLength;
        যদি (list->array.resize(newCapacity, listLength) == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            প্রত্যাগম অসত্য;
        }
    }
    uhash_putiAllowZero(map, clone.getAlias(), listLength, &errorCode);
    যদি (U_FAILURE(errorCode)) { প্রত্যাগম অসত্য; }
    LocaleAndWeight &lw = list->array[listLength];
    lw.locale = clone.orphan();
    lw.weight = weight;
    lw.index = listLength++;
    যদি (weight < WEIGHT_ONE) { hasWeights = সত্য; }
    U_ASSERT(uhash_গুনতি(map) == getLength());
    প্রত্যাগম সত্য;
}

শূন্য LocalePriorityList::sort(UErrorCode &errorCode) {
    // Sort by descending weights if there is a mix of weights.
    // The comparator forces a stable sort via the item index.
    যদি (U_FAILURE(errorCode) || getLength() <= 1 || !hasWeights) { প্রত্যাগম; }
    uprv_sortArray(list->array.getAlias(), listLength, পরিমাণ(LocaleAndWeight),
                   compareLocaleAndWeight, nullptr, অসত্য, &errorCode);
}

U_NAMESPACE_END
