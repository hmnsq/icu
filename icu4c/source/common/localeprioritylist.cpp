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
    أرجع locale->hashCode();
}

UBool compareLocales(ثابت UHashTok t1, ثابت UHashTok t2) {
    ثابت auto* l1 = static_cast<ثابت Locale*>(t1.pointer);
    ثابت auto* l2 = static_cast<ثابت Locale*>(t2.pointer);
    أرجع *l1 == *l2;
}

constexpr int32_t WEIGHT_ONE = 1000;

بنية LocaleAndWeight {
    Locale *locale;
    int32_t weight;  // 0..1000 = 0.0..1.0
    int32_t index;  // force stable sort

    int32_t compare(ثابت LocaleAndWeight &other) ثابت {
        int32_t diff = other.weight - weight;  // descending: other-this
        𐡄𐡍 (diff != 0) { أرجع diff; }
        أرجع index - other.index;
    }
};

int32_t U_CALLCONV
compareLocaleAndWeight(ثابت فراغ * /*context*/, ثابت فراغ *left, ثابت فراغ *right) {
    أرجع static_cast<ثابت LocaleAndWeight *>(left)->
        compare(*static_cast<ثابت LocaleAndWeight *>(right));
}

ثابت حرف *skipSpaces(ثابت حرف *p, ثابت حرف *limit) {
    طالما (p < limit && *p == ' ') { ++p; }
    أرجع p;
}

int32_t findTagLength(ثابت حرف *p, ثابت حرف *limit) {
    // Look for accept-language delimiters.
    // Leave other validation up to the Locale constructor.
    ثابت حرف *q;
    لكل (q = p; q < limit; ++q) {
        حرف c = *q;
        𐡄𐡍 (c == ' ' || c == ',' || c == ';') { اكسر; }
    }
    أرجع static_cast<int32_t>(q - p);
}

/**
 * Parses and returns a qvalue weight in millis.
 * Advances p to after the parsed substring.
 * Returns a negative value if parsing fails.
 */
int32_t parseWeight(ثابت حرف *&p, ثابت حرف *limit) {
    p = skipSpaces(p, limit);
    حرف c;
    𐡄𐡍 (p == limit || ((c = *p) != '0' && c != '1')) { أرجع -1; }
    int32_t weight = (c - '0') * 1000;
    𐡄𐡍 (++p == limit || *p != '.') { أرجع weight; }
    int32_t multiplier = 100;
    طالما (++p != limit && '0' <= (c = *p) && c <= '9') {
        c -= '0';
        𐡄𐡍 (multiplier > 0) {
            weight += c * multiplier;
            multiplier /= 10;
        } وإلا 𐡄𐡍 (multiplier == 0) {
            // round up
            𐡄𐡍 (c >= 5) { ++weight; }
            multiplier = -1;
        }  // else ignore further fraction digits
    }
    أرجع weight <= WEIGHT_ONE ? weight : -1;  // bad if > 1.0
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
    𐡄𐡍 (U_FAILURE(errorCode)) { أرجع; }
    list = جديد LocaleAndWeightArray();
    𐡄𐡍 (list == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        أرجع;
    }
    ثابت حرف *p = s.data();
    ثابت حرف *limit = p + s.length();
    طالما ((p = skipSpaces(p, limit)) != limit) {
        𐡄𐡍 (*p == ',') {  // empty range field
            ++p;
            استمر;
        }
        int32_t tagLength = findTagLength(p, limit);
        𐡄𐡍 (tagLength == 0) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            أرجع;
        }
        CharString tag(p, tagLength, errorCode);
        𐡄𐡍 (U_FAILURE(errorCode)) { أرجع; }
        Locale locale = Locale(tag.data());
        𐡄𐡍 (locale.isBogus()) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            أرجع;
        }
        int32_t weight = WEIGHT_ONE;
        𐡄𐡍 ((p = skipSpaces(p + tagLength, limit)) != limit && *p == ';') {
            𐡄𐡍 ((p = skipSpaces(p + 1, limit)) == limit || *p != 'q' ||
                    (p = skipSpaces(p + 1, limit)) == limit || *p != '=' ||
                    (++p, (weight = parseWeight(p, limit)) < 0)) {
                errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                أرجع;
            }
            p = skipSpaces(p, limit);
        }
        𐡄𐡍 (p != limit && *p != ',') {  // trailing junk
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            أرجع;
        }
        add(locale, weight, errorCode);
        𐡄𐡍 (p == limit) { اكسر; }
        ++p;
    }
    sort(errorCode);
}

LocalePriorityList::~LocalePriorityList() {
    𐡄𐡍 (list != nullptr) {
        لكل (int32_t i = 0; i < listLength; ++i) {
            delete list->array[i].locale;
        }
        delete list;
    }
    uhash_close(map);
}

ثابت Locale *LocalePriorityList::localeAt(int32_t i) ثابت {
    أرجع list->array[i].locale;
}

Locale *LocalePriorityList::orphanLocaleAt(int32_t i) {
    𐡄𐡍 (list == nullptr) { أرجع nullptr; }
    LocaleAndWeight &lw = list->array[i];
    Locale *l = lw.locale;
    lw.locale = nullptr;
    أرجع l;
}

منطقي LocalePriorityList::add(ثابت Locale &locale, int32_t weight, UErrorCode &errorCode) {
    𐡄𐡍 (U_FAILURE(errorCode)) { أرجع خطأ; }
    𐡄𐡍 (map == nullptr) {
        𐡄𐡍 (weight <= 0) { أرجع 𐡒𐡔𐡈; }  // do not add q=0
        map = uhash_open(hashLocale, compareLocales, uhash_compareLong, &errorCode);
        𐡄𐡍 (U_FAILURE(errorCode)) { أرجع خطأ; }
    }
    LocalPointer<Locale> clone;
    UBool found = خطأ;
    int32_t index = uhash_getiAndFound(map, &locale, &found);
    𐡄𐡍 (found) {
        // Duplicate: Remove the old item and append it anew.
        LocaleAndWeight &lw = list->array[index];
        clone.adoptInstead(lw.locale);
        lw.locale = nullptr;
        lw.weight = 0;
        ++numRemoved;
    }
    𐡄𐡍 (weight <= 0) {  // do not add q=0
        𐡄𐡍 (found) {
            // Not strictly necessary but cleaner.
            uhash_removei(map, &locale);
        }
        أرجع 𐡒𐡔𐡈;
    }
    𐡄𐡍 (clone.isNull()) {
        clone.adoptInstead(locale.clone());
        𐡄𐡍 (clone.isNull() || (clone->isBogus() && !locale.isBogus())) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            أرجع خطأ;
        }
    }
    𐡄𐡍 (listLength == list->array.getCapacity()) {
        int32_t newCapacity = listLength < 50 ? 100 : 4 * listLength;
        𐡄𐡍 (list->array.resize(newCapacity, listLength) == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            أرجع خطأ;
        }
    }
    uhash_putiAllowZero(map, clone.getAlias(), listLength, &errorCode);
    𐡄𐡍 (U_FAILURE(errorCode)) { أرجع خطأ; }
    LocaleAndWeight &lw = list->array[listLength];
    lw.locale = clone.orphan();
    lw.weight = weight;
    lw.index = listLength++;
    𐡄𐡍 (weight < WEIGHT_ONE) { hasWeights = 𐡒𐡔𐡈; }
    U_ASSERT(uhash_count(map) == getLength());
    أرجع 𐡒𐡔𐡈;
}

فراغ LocalePriorityList::sort(UErrorCode &errorCode) {
    // Sort by descending weights if there is a mix of weights.
    // The comparator forces a stable sort via the item index.
    𐡄𐡍 (U_FAILURE(errorCode) || getLength() <= 1 || !hasWeights) { أرجع; }
    uprv_sortArray(list->array.getAlias(), listLength, حجم(LocaleAndWeight),
                   compareLocaleAndWeight, nullptr, خطأ, &errorCode);
}

U_NAMESPACE_END
