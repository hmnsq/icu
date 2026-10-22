// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// localeprioritylist.cpp
// created: 2019jul11 Markus W. Scherer

#شامل "unicode/utypes.h"
#شامل "unicode/localpointer.h"
#شامل "unicode/locid.h"
#شامل "unicode/stringpiece.h"
#شامل "unicode/uobject.h"
#شامل "charstr.h"
#شامل "cmemory.h"
#شامل "localeprioritylist.h"
#شامل "uarrsort.h"
#شامل "uassert.h"
#شامل "uhash.h"

U_NAMESPACE_BEGIN

فضاینام {

int32_t hashLocale(ثابت UHashTok token) {
    ثابت auto* locale = static_cast<ثابت Locale*>(token.pointer);
    بازگشت locale->hashCode();
}

UBool compareLocales(ثابت UHashTok t1, ثابت UHashTok t2) {
    ثابت auto* l1 = static_cast<ثابت Locale*>(t1.pointer);
    ثابت auto* l2 = static_cast<ثابت Locale*>(t2.pointer);
    بازگشت *l1 == *l2;
}

constexpr int32_t WEIGHT_ONE = 1000;

ساختار LocaleAndWeight {
    Locale *locale;
    int32_t weight;  // 0..1000 = 0.0..1.0
    int32_t index;  // force stable sort

    int32_t compare(ثابت LocaleAndWeight &other) ثابت {
        int32_t diff = other.weight - weight;  // descending: other-this
        اگر (diff != 0) { بازگشت diff; }
        بازگشت index - other.index;
    }
};

int32_t U_CALLCONV
compareLocaleAndWeight(ثابت تهی * /*context*/, ثابت تهی *left, ثابت تهی *راست) {
    بازگشت static_cast<ثابت LocaleAndWeight *>(left)->
        compare(*static_cast<ثابت LocaleAndWeight *>(راست));
}

ثابت نویسه *skipSpaces(ثابت نویسه *p, ثابت نویسه *limit) {
    وقتی (p < limit && *p == ' ') { ++p; }
    بازگشت p;
}

int32_t findTagLength(ثابت نویسه *p, ثابت نویسه *limit) {
    // Look for accept-language delimiters.
    // Leave other validation up to the Locale constructor.
    ثابت نویسه *q;
    برای (q = p; q < limit; ++q) {
        نویسه c = *q;
        اگر (c == ' ' || c == ',' || c == ';') { بشکن; }
    }
    بازگشت static_cast<int32_t>(q - p);
}

/**
 * Parses and returns a qvalue weight in millis.
 * Advances p to after the parsed substring.
 * Returns a negative value if parsing fails.
 */
int32_t parseWeight(ثابت نویسه *&p, ثابت نویسه *limit) {
    p = skipSpaces(p, limit);
    نویسه c;
    اگر (p == limit || ((c = *p) != '0' && c != '1')) { بازگشت -1; }
    int32_t weight = (c - '0') * 1000;
    اگر (++p == limit || *p != '.') { بازگشت weight; }
    int32_t multiplier = 100;
    وقتی (++p != limit && '0' <= (c = *p) && c <= '9') {
        c -= '0';
        اگر (multiplier > 0) {
            weight += c * multiplier;
            multiplier /= 10;
        } وگرنه اگر (multiplier == 0) {
            // round up
            اگر (c >= 5) { ++weight; }
            multiplier = -1;
        }  // else ignore further fraction digits
    }
    بازگشت weight <= WEIGHT_ONE ? weight : -1;  // bad if > 1.0
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
ساختار LocaleAndWeightArray : عمومی UMemory {
    Maybe_پشته_Array<LocaleAndWeight, 20> array;
};

LocalePriorityList::LocalePriorityList(StringPiece s, UErrorCode &errorCode) {
    اگر (U_FAILURE(errorCode)) { بازگشت; }
    list = جدید LocaleAndWeightArray();
    اگر (list == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        بازگشت;
    }
    ثابت نویسه *p = s.data();
    ثابت نویسه *limit = p + s.length();
    وقتی ((p = skipSpaces(p, limit)) != limit) {
        اگر (*p == ',') {  // empty range field
            ++p;
            ادامه;
        }
        int32_t tagLength = findTagLength(p, limit);
        اگر (tagLength == 0) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            بازگشت;
        }
        CharString tag(p, tagLength, errorCode);
        اگر (U_FAILURE(errorCode)) { بازگشت; }
        Locale locale = Locale(tag.data());
        اگر (locale.isBogus()) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            بازگشت;
        }
        int32_t weight = WEIGHT_ONE;
        اگر ((p = skipSpaces(p + tagLength, limit)) != limit && *p == ';') {
            اگر ((p = skipSpaces(p + 1, limit)) == limit || *p != 'q' ||
                    (p = skipSpaces(p + 1, limit)) == limit || *p != '=' ||
                    (++p, (weight = parseWeight(p, limit)) < 0)) {
                errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                بازگشت;
            }
            p = skipSpaces(p, limit);
        }
        اگر (p != limit && *p != ',') {  // trailing junk
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            بازگشت;
        }
        add(locale, weight, errorCode);
        اگر (p == limit) { بشکن; }
        ++p;
    }
    sort(errorCode);
}

LocalePriorityList::~LocalePriorityList() {
    اگر (list != nullptr) {
        برای (int32_t i = 0; i < listLength; ++i) {
            delete list->array[i].locale;
        }
        delete list;
    }
    uhash_close(map);
}

ثابت Locale *LocalePriorityList::localeAt(int32_t i) ثابت {
    بازگشت list->array[i].locale;
}

Locale *LocalePriorityList::orphanLocaleAt(int32_t i) {
    اگر (list == nullptr) { بازگشت nullptr; }
    LocaleAndWeight &lw = list->array[i];
    Locale *l = lw.locale;
    lw.locale = nullptr;
    بازگشت l;
}

منطقی LocalePriorityList::add(ثابت Locale &locale, int32_t weight, UErrorCode &errorCode) {
    اگر (U_FAILURE(errorCode)) { بازگشت نادرست; }
    اگر (map == nullptr) {
        اگر (weight <= 0) { بازگشت درست; }  // do not add q=0
        map = uhash_open(hashLocale, compareLocales, uhash_compareLong, &errorCode);
        اگر (U_FAILURE(errorCode)) { بازگشت نادرست; }
    }
    LocalPointer<Locale> clone;
    UBool found = نادرست;
    int32_t index = uhash_getiAndFound(map, &locale, &found);
    اگر (found) {
        // Duplicate: Remove the old item and append it anew.
        LocaleAndWeight &lw = list->array[index];
        clone.adoptInstead(lw.locale);
        lw.locale = nullptr;
        lw.weight = 0;
        ++رقم_Removed;
    }
    اگر (weight <= 0) {  // do not add q=0
        اگر (found) {
            // Not strictly necessary but cleaner.
            uhash_removei(map, &locale);
        }
        بازگشت درست;
    }
    اگر (clone.isNull()) {
        clone.adoptInstead(locale.clone());
        اگر (clone.isNull() || (clone->isBogus() && !locale.isBogus())) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            بازگشت نادرست;
        }
    }
    اگر (listLength == list->array.getCapacity()) {
        int32_t newCapacity = listLength < 50 ? 100 : 4 * listLength;
        اگر (list->array.resize(newCapacity, listLength) == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            بازگشت نادرست;
        }
    }
    uhash_putiAllowZero(map, clone.getAlias(), listLength, &errorCode);
    اگر (U_FAILURE(errorCode)) { بازگشت نادرست; }
    LocaleAndWeight &lw = list->array[listLength];
    lw.locale = clone.orphan();
    lw.weight = weight;
    lw.index = listLength++;
    اگر (weight < WEIGHT_ONE) { hasWeights = درست; }
    U_ASSERT(uhash_شمار(map) == getLength());
    بازگشت درست;
}

تهی LocalePriorityList::sort(UErrorCode &errorCode) {
    // Sort by descending weights if there is a mix of weights.
    // The comparator forces a stable sort via the item index.
    اگر (U_FAILURE(errorCode) || getLength() <= 1 || !hasWeights) { بازگشت; }
    uprv_sortArray(list->array.getAlias(), listLength, اندازه(LocaleAndWeight),
                   compareLocaleAndWeight, nullptr, نادرست, &errorCode);
}

U_NAMESPACE_END
