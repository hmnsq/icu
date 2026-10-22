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

int32_t hashLocale(ধ্রুবক UHashTok token) {
    ধ্রুবক auto* locale = static_cast<ধ্রুবক Locale*>(token.pointer);
    ফেরত locale->hashCode();
}

UBool compareLocales(ধ্রুবক UHashTok t1, ধ্রুবক UHashTok t2) {
    ধ্রুবক auto* l1 = static_cast<ধ্রুবক Locale*>(t1.pointer);
    ধ্রুবক auto* l2 = static_cast<ধ্রুবক Locale*>(t2.pointer);
    ফেরত *l1 == *l2;
}

constexpr int32_t WEIGHT_ONE = 1000;

কাঠামো LocaleAndWeight {
    Locale *locale;
    int32_t weight;  // 0..1000 = 0.0..1.0
    int32_t সূচক;  // force stable sort

    int32_t compare(ধ্রুবক LocaleAndWeight &other) ধ্রুবক {
        int32_t diff = other.weight - weight;  // descending: other-this
        যদি (diff != 0) { ফেরত diff; }
        ফেরত সূচক - other.সূচক;
    }
};

int32_t U_CALLCONV
compareLocaleAndWeight(ধ্রুবক শূন্য * /*context*/, ধ্রুবক শূন্য *left, ধ্রুবক শূন্য *ডান) {
    ফেরত static_cast<ধ্রুবক LocaleAndWeight *>(left)->
        compare(*static_cast<ধ্রুবক LocaleAndWeight *>(ডান));
}

ধ্রুবক অক্ষর *skipSpaces(ধ্রুবক অক্ষর *p, ধ্রুবক অক্ষর *limit) {
    যখন (p < limit && *p == ' ') { ++p; }
    ফেরত p;
}

int32_t find_Tag_দৈর্ঘ্য(ধ্রুবক অক্ষর *p, ধ্রুবক অক্ষর *limit) {
    // Look for accept-language delimiters.
    // Leave other validation up to the Locale constructor.
    ধ্রুবক অক্ষর *q;
    জন্য (q = p; q < limit; ++q) {
        অক্ষর c = *q;
        যদি (c == ' ' || c == ',' || c == ';') { বিরতি; }
    }
    ফেরত static_cast<int32_t>(q - p);
}

/**
 * Parses and returns a qvalue weight in millis.
 * Advances p to after the parsed substring.
 * Returns a negative value if parsing fails.
 */
int32_t parseWeight(ধ্রুবক অক্ষর *&p, ধ্রুবক অক্ষর *limit) {
    p = skipSpaces(p, limit);
    অক্ষর c;
    যদি (p == limit || ((c = *p) != '0' && c != '1')) { ফেরত -1; }
    int32_t weight = (c - '0') * 1000;
    যদি (++p == limit || *p != '.') { ফেরত weight; }
    int32_t multiplier = 100;
    যখন (++p != limit && '0' <= (c = *p) && c <= '9') {
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
    ফেরত weight <= WEIGHT_ONE ? weight : -1;  // bad if > 1.0
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
কাঠামো LocaleAndWeightArray : সর্বজনীন UMemory {
    Maybe_স্তূপ_Array<LocaleAndWeight, 20> array;
};

LocalePriorityList::LocalePriorityList(StringPiece s, UErrorCode &errorCode) {
    যদি (U_FAILURE(errorCode)) { ফেরত; }
    list = নতুন LocaleAndWeightArray();
    যদি (list == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        ফেরত;
    }
    ধ্রুবক অক্ষর *p = s.তথ্য();
    ধ্রুবক অক্ষর *limit = p + s.length();
    যখন ((p = skipSpaces(p, limit)) != limit) {
        যদি (*p == ',') {  // empty range field
            ++p;
            চালু;
        }
        int32_t tag_দৈর্ঘ্য = find_Tag_দৈর্ঘ্য(p, limit);
        যদি (tag_দৈর্ঘ্য == 0) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            ফেরত;
        }
        CharString tag(p, tag_দৈর্ঘ্য, errorCode);
        যদি (U_FAILURE(errorCode)) { ফেরত; }
        Locale locale = Locale(tag.তথ্য());
        যদি (locale.isBogus()) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            ফেরত;
        }
        int32_t weight = WEIGHT_ONE;
        যদি ((p = skipSpaces(p + tag_দৈর্ঘ্য, limit)) != limit && *p == ';') {
            যদি ((p = skipSpaces(p + 1, limit)) == limit || *p != 'q' ||
                    (p = skipSpaces(p + 1, limit)) == limit || *p != '=' ||
                    (++p, (weight = parseWeight(p, limit)) < 0)) {
                errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                ফেরত;
            }
            p = skipSpaces(p, limit);
        }
        যদি (p != limit && *p != ',') {  // trailing junk
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            ফেরত;
        }
        add(locale, weight, errorCode);
        যদি (p == limit) { বিরতি; }
        ++p;
    }
    sort(errorCode);
}

LocalePriorityList::~LocalePriorityList() {
    যদি (list != nullptr) {
        জন্য (int32_t i = 0; i < list_দৈর্ঘ্য; ++i) {
            delete list->array[i].locale;
        }
        delete list;
    }
    uhash_close(map);
}

ধ্রুবক Locale *LocalePriorityList::localeAt(int32_t i) ধ্রুবক {
    ফেরত list->array[i].locale;
}

Locale *LocalePriorityList::orphanLocaleAt(int32_t i) {
    যদি (list == nullptr) { ফেরত nullptr; }
    LocaleAndWeight &lw = list->array[i];
    Locale *l = lw.locale;
    lw.locale = nullptr;
    ফেরত l;
}

যৌক্তিক LocalePriorityList::add(ধ্রুবক Locale &locale, int32_t weight, UErrorCode &errorCode) {
    যদি (U_FAILURE(errorCode)) { ফেরত মিথ্যা; }
    যদি (map == nullptr) {
        যদি (weight <= 0) { ফেরত সত্য; }  // do not add q=0
        map = uhash_open(hashLocale, compareLocales, uhash_compareLong, &errorCode);
        যদি (U_FAILURE(errorCode)) { ফেরত মিথ্যা; }
    }
    LocalPointer<Locale> clone;
    UBool found = মিথ্যা;
    int32_t সূচক = uhash_getiAndFound(map, &locale, &found);
    যদি (found) {
        // Duplicate: Remove the old item and append it anew.
        LocaleAndWeight &lw = list->array[সূচক];
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
        ফেরত সত্য;
    }
    যদি (clone.isNull()) {
        clone.adoptInstead(locale.clone());
        যদি (clone.isNull() || (clone->isBogus() && !locale.isBogus())) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            ফেরত মিথ্যা;
        }
    }
    যদি (list_দৈর্ঘ্য == list->array.getCapacity()) {
        int32_t newCapacity = list_দৈর্ঘ্য < 50 ? 100 : 4 * list_দৈর্ঘ্য;
        যদি (list->array.resize(newCapacity, list_দৈর্ঘ্য) == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            ফেরত মিথ্যা;
        }
    }
    uhash_putiAllowZero(map, clone.getAlias(), list_দৈর্ঘ্য, &errorCode);
    যদি (U_FAILURE(errorCode)) { ফেরত মিথ্যা; }
    LocaleAndWeight &lw = list->array[list_দৈর্ঘ্য];
    lw.locale = clone.orphan();
    lw.weight = weight;
    lw.সূচক = list_দৈর্ঘ্য++;
    যদি (weight < WEIGHT_ONE) { hasWeights = সত্য; }
    U_ASSERT(uhash_গুনতি(map) == get_দৈর্ঘ্য());
    ফেরত সত্য;
}

শূন্য LocalePriorityList::sort(UErrorCode &errorCode) {
    // Sort by descending weights if there is a mix of weights.
    // The comparator forces a stable sort via the item index.
    যদি (U_FAILURE(errorCode) || get_দৈর্ঘ্য() <= 1 || !hasWeights) { ফেরত; }
    uprv_sortArray(list->array.getAlias(), list_দৈর্ঘ্য, আকার(LocaleAndWeight),
                   compareLocaleAndWeight, nullptr, মিথ্যা, &errorCode);
}

U_NAMESPACE_END
