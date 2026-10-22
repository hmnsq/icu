// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// localeprioritylist.cpp
// created: 2019jul11 Markus W. Scherer

#ضمن "unicode/utypes.h"
#ضمن "unicode/localpointer.h"
#ضمن "unicode/locid.h"
#ضمن "unicode/stringpiece.h"
#ضمن "unicode/uobject.h"
#ضمن "charstr.h"
#ضمن "cmemory.h"
#ضمن "localeprioritylist.h"
#ضمن "uarrsort.h"
#ضمن "uassert.h"
#ضمن "uhash.h"

U_NAMESPACE_BEGIN

نطاق {

int32_t hashLocale(ثابت UHashTok token) {
    ثابت auto* locale = static_cast<ثابت Locale*>(token.pointer);
    ܦܢܐ locale->hashCode();
}

UBool compareLocales(ثابت UHashTok t1, ثابت UHashTok t2) {
    ثابت auto* l1 = static_cast<ثابت Locale*>(t1.pointer);
    ثابت auto* l2 = static_cast<ثابت Locale*>(t2.pointer);
    ܦܢܐ *l1 == *l2;
}

constexpr int32_t WEIGHT_ONE = 1000;

بنية LocaleAndWeight {
    Locale *locale;
    int32_t weight;  // 0..1000 = 0.0..1.0
    int32_t index;  // force stable sort

    int32_t compare(ثابت LocaleAndWeight &other) ثابت {
        int32_t diff = other.weight - weight;  // descending: other-this
        ܐܢ (diff != 0) { ܦܢܐ diff; }
        ܦܢܐ index - other.index;
    }
};

int32_t U_CALLCONV
compareLocaleAndWeight(ثابت فراغ * /*context*/, ثابت فراغ *left, ثابت فراغ *right) {
    ܦܢܐ static_cast<ثابت LocaleAndWeight *>(left)->
        compare(*static_cast<ثابت LocaleAndWeight *>(right));
}

ثابت حرف *skipSpaces(ثابت حرف *p, ثابت حرف *limit) {
    ܟܡܐ (p < limit && *p == ' ') { ++p; }
    ܦܢܐ p;
}

int32_t findTagLength(ثابت حرف *p, ثابت حرف *limit) {
    // Look for accept-language delimiters.
    // Leave other validation up to the Locale constructor.
    ثابت حرف *q;
    لكل (q = p; q < limit; ++q) {
        حرف c = *q;
        ܐܢ (c == ' ' || c == ',' || c == ';') { اكسر; }
    }
    ܦܢܐ static_cast<int32_t>(q - p);
}

/**
 * Parses and returns a qvalue weight in millis.
 * Advances p to after the parsed substring.
 * Returns a negative value if parsing fails.
 */
int32_t parseWeight(ثابت حرف *&p, ثابت حرف *limit) {
    p = skipSpaces(p, limit);
    حرف c;
    ܐܢ (p == limit || ((c = *p) != '0' && c != '1')) { ܦܢܐ -1; }
    int32_t weight = (c - '0') * 1000;
    ܐܢ (++p == limit || *p != '.') { ܦܢܐ weight; }
    int32_t multiplier = 100;
    ܟܡܐ (++p != limit && '0' <= (c = *p) && c <= '9') {
        c -= '0';
        ܐܢ (multiplier > 0) {
            weight += c * multiplier;
            multiplier /= 10;
        } ܐܚܪܢܐ ܐܢ (multiplier == 0) {
            // round up
            ܐܢ (c >= 5) { ++weight; }
            multiplier = -1;
        }  // else ignore further fraction digits
    }
    ܦܢܐ weight <= WEIGHT_ONE ? weight : -1;  // bad if > 1.0
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
بنية LocaleAndWeightArray : عام UMemory {
    MaybeStackArray<LocaleAndWeight, 20> array;
};

LocalePriorityList::LocalePriorityList(StringPiece s, UErrorCode &errorCode) {
    ܐܢ (U_FAILURE(errorCode)) { ܦܢܐ; }
    list = جديد LocaleAndWeightArray();
    ܐܢ (list == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        ܦܢܐ;
    }
    ثابت حرف *p = s.data();
    ثابت حرف *limit = p + s.length();
    ܟܡܐ ((p = skipSpaces(p, limit)) != limit) {
        ܐܢ (*p == ',') {  // empty range field
            ++p;
            استمر;
        }
        int32_t tagLength = findTagLength(p, limit);
        ܐܢ (tagLength == 0) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            ܦܢܐ;
        }
        CharString tag(p, tagLength, errorCode);
        ܐܢ (U_FAILURE(errorCode)) { ܦܢܐ; }
        Locale locale = Locale(tag.data());
        ܐܢ (locale.isBogus()) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            ܦܢܐ;
        }
        int32_t weight = WEIGHT_ONE;
        ܐܢ ((p = skipSpaces(p + tagLength, limit)) != limit && *p == ';') {
            ܐܢ ((p = skipSpaces(p + 1, limit)) == limit || *p != 'q' ||
                    (p = skipSpaces(p + 1, limit)) == limit || *p != '=' ||
                    (++p, (weight = parseWeight(p, limit)) < 0)) {
                errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                ܦܢܐ;
            }
            p = skipSpaces(p, limit);
        }
        ܐܢ (p != limit && *p != ',') {  // trailing junk
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            ܦܢܐ;
        }
        add(locale, weight, errorCode);
        ܐܢ (p == limit) { اكسر; }
        ++p;
    }
    sort(errorCode);
}

LocalePriorityList::~LocalePriorityList() {
    ܐܢ (list != nullptr) {
        لكل (int32_t i = 0; i < listLength; ++i) {
            delete list->array[i].locale;
        }
        delete list;
    }
    uhash_close(map);
}

ثابت Locale *LocalePriorityList::localeAt(int32_t i) ثابت {
    ܦܢܐ list->array[i].locale;
}

Locale *LocalePriorityList::orphanLocaleAt(int32_t i) {
    ܐܢ (list == nullptr) { ܦܢܐ nullptr; }
    LocaleAndWeight &lw = list->array[i];
    Locale *l = lw.locale;
    lw.locale = nullptr;
    ܦܢܐ l;
}

منطقي LocalePriorityList::add(ثابت Locale &locale, int32_t weight, UErrorCode &errorCode) {
    ܐܢ (U_FAILURE(errorCode)) { ܦܢܐ ܕܓܠܐ; }
    ܐܢ (map == nullptr) {
        ܐܢ (weight <= 0) { ܦܢܐ ܫܪܪܐ; }  // do not add q=0
        map = uhash_open(hashLocale, compareLocales, uhash_compareLong, &errorCode);
        ܐܢ (U_FAILURE(errorCode)) { ܦܢܐ ܕܓܠܐ; }
    }
    LocalPointer<Locale> clone;
    UBool found = ܕܓܠܐ;
    int32_t index = uhash_getiAndFound(map, &locale, &found);
    ܐܢ (found) {
        // Duplicate: Remove the old item and append it anew.
        LocaleAndWeight &lw = list->array[index];
        clone.adoptInstead(lw.locale);
        lw.locale = nullptr;
        lw.weight = 0;
        ++numRemoved;
    }
    ܐܢ (weight <= 0) {  // do not add q=0
        ܐܢ (found) {
            // Not strictly necessary but cleaner.
            uhash_removei(map, &locale);
        }
        ܦܢܐ ܫܪܪܐ;
    }
    ܐܢ (clone.isNull()) {
        clone.adoptInstead(locale.clone());
        ܐܢ (clone.isNull() || (clone->isBogus() && !locale.isBogus())) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            ܦܢܐ ܕܓܠܐ;
        }
    }
    ܐܢ (listLength == list->array.getCapacity()) {
        int32_t newCapacity = listLength < 50 ? 100 : 4 * listLength;
        ܐܢ (list->array.resize(newCapacity, listLength) == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            ܦܢܐ ܕܓܠܐ;
        }
    }
    uhash_putiAllowZero(map, clone.getAlias(), listLength, &errorCode);
    ܐܢ (U_FAILURE(errorCode)) { ܦܢܐ ܕܓܠܐ; }
    LocaleAndWeight &lw = list->array[listLength];
    lw.locale = clone.orphan();
    lw.weight = weight;
    lw.index = listLength++;
    ܐܢ (weight < WEIGHT_ONE) { hasWeights = ܫܪܪܐ; }
    U_ASSERT(uhash_count(map) == getLength());
    ܦܢܐ ܫܪܪܐ;
}

فراغ LocalePriorityList::sort(UErrorCode &errorCode) {
    // Sort by descending weights if there is a mix of weights.
    // The comparator forces a stable sort via the item index.
    ܐܢ (U_FAILURE(errorCode) || getLength() <= 1 || !hasWeights) { ܦܢܐ; }
    uprv_sortArray(list->array.getAlias(), listLength, حجم(LocaleAndWeight),
                   compareLocaleAndWeight, nullptr, ܕܓܠܐ, &errorCode);
}

U_NAMESPACE_END
