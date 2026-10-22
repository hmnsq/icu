// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// localeprioritylist.cpp
// created: 2019jul11 Markus W. Scherer

#அந்தர்பாவ "unicode/utypes.h"
#அந்தர்பாவ "unicode/localpointer.h"
#அந்தர்பாவ "unicode/locid.h"
#அந்தர்பாவ "unicode/stringpiece.h"
#அந்தர்பாவ "unicode/uobject.h"
#அந்தர்பாவ "charstr.h"
#அந்தர்பாவ "cmemory.h"
#அந்தர்பாவ "localeprioritylist.h"
#அந்தர்பாவ "uarrsort.h"
#அந்தர்பாவ "uassert.h"
#அந்தர்பாவ "uhash.h"

U_NAMESPACE_BEGIN

நாமாகாஶ {

int32_t hashLocale(மாறிலி UHashTok token) {
    மாறிலி auto* locale = static_cast<மாறிலி Locale*>(token.pointer);
    திரும்பு locale->hashCode();
}

UBool compareLocales(மாறிலி UHashTok t1, மாறிலி UHashTok t2) {
    மாறிலி auto* l1 = static_cast<மாறிலி Locale*>(t1.pointer);
    மாறிலி auto* l2 = static_cast<மாறிலி Locale*>(t2.pointer);
    திரும்பு *l1 == *l2;
}

constexpr int32_t WEIGHT_ONE = 1000;

கட்டமைப்பு LocaleAndWeight {
    Locale *locale;
    int32_t weight;  // 0..1000 = 0.0..1.0
    int32_t index;  // force stable sort

    int32_t compare(மாறிலி LocaleAndWeight &other) மாறிலி {
        int32_t diff = other.weight - weight;  // descending: other-this
        எனில் (diff != 0) { திரும்பு diff; }
        திரும்பு index - other.index;
    }
};

int32_t U_CALLCONV
compareLocaleAndWeight(மாறிலி வெறுமை * /*context*/, மாறிலி வெறுமை *left, மாறிலி வெறுமை *வலது) {
    திரும்பு static_cast<மாறிலி LocaleAndWeight *>(left)->
        compare(*static_cast<மாறிலி LocaleAndWeight *>(வலது));
}

மாறிலி எழுத்து *skipSpaces(மாறிலி எழுத்து *p, மாறிலி எழுத்து *limit) {
    போது (p < limit && *p == ' ') { ++p; }
    திரும்பு p;
}

int32_t findTagLength(மாறிலி எழுத்து *p, மாறிலி எழுத்து *limit) {
    // Look for accept-language delimiters.
    // Leave other validation up to the Locale constructor.
    மாறிலி எழுத்து *q;
    க்கு (q = p; q < limit; ++q) {
        எழுத்து c = *q;
        எனில் (c == ' ' || c == ',' || c == ';') { நிறுத்து; }
    }
    திரும்பு static_cast<int32_t>(q - p);
}

/**
 * Parses and returns a qvalue weight in millis.
 * Advances p to after the parsed substring.
 * Returns a negative value if parsing fails.
 */
int32_t parseWeight(மாறிலி எழுத்து *&p, மாறிலி எழுத்து *limit) {
    p = skipSpaces(p, limit);
    எழுத்து c;
    எனில் (p == limit || ((c = *p) != '0' && c != '1')) { திரும்பு -1; }
    int32_t weight = (c - '0') * 1000;
    எனில் (++p == limit || *p != '.') { திரும்பு weight; }
    int32_t multiplier = 100;
    போது (++p != limit && '0' <= (c = *p) && c <= '9') {
        c -= '0';
        எனில் (multiplier > 0) {
            weight += c * multiplier;
            multiplier /= 10;
        } இல்லையெனில் எனில் (multiplier == 0) {
            // round up
            எனில் (c >= 5) { ++weight; }
            multiplier = -1;
        }  // else ignore further fraction digits
    }
    திரும்பு weight <= WEIGHT_ONE ? weight : -1;  // bad if > 1.0
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
கட்டமைப்பு LocaleAndWeightArray : பொது UMemory {
    Maybe_அடுக்கு_Array<LocaleAndWeight, 20> array;
};

LocalePriorityList::LocalePriorityList(StringPiece s, UErrorCode &errorCode) {
    எனில் (U_FAILURE(errorCode)) { திரும்பு; }
    list = புதிய LocaleAndWeightArray();
    எனில் (list == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        திரும்பு;
    }
    மாறிலி எழுத்து *p = s.data();
    மாறிலி எழுத்து *limit = p + s.length();
    போது ((p = skipSpaces(p, limit)) != limit) {
        எனில் (*p == ',') {  // empty range field
            ++p;
            தொடர்;
        }
        int32_t tagLength = findTagLength(p, limit);
        எனில் (tagLength == 0) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            திரும்பு;
        }
        CharString tag(p, tagLength, errorCode);
        எனில் (U_FAILURE(errorCode)) { திரும்பு; }
        Locale locale = Locale(tag.data());
        எனில் (locale.isBogus()) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            திரும்பு;
        }
        int32_t weight = WEIGHT_ONE;
        எனில் ((p = skipSpaces(p + tagLength, limit)) != limit && *p == ';') {
            எனில் ((p = skipSpaces(p + 1, limit)) == limit || *p != 'q' ||
                    (p = skipSpaces(p + 1, limit)) == limit || *p != '=' ||
                    (++p, (weight = parseWeight(p, limit)) < 0)) {
                errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                திரும்பு;
            }
            p = skipSpaces(p, limit);
        }
        எனில் (p != limit && *p != ',') {  // trailing junk
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            திரும்பு;
        }
        add(locale, weight, errorCode);
        எனில் (p == limit) { நிறுத்து; }
        ++p;
    }
    sort(errorCode);
}

LocalePriorityList::~LocalePriorityList() {
    எனில் (list != nullptr) {
        க்கு (int32_t i = 0; i < listLength; ++i) {
            delete list->array[i].locale;
        }
        delete list;
    }
    uhash_close(map);
}

மாறிலி Locale *LocalePriorityList::localeAt(int32_t i) மாறிலி {
    திரும்பு list->array[i].locale;
}

Locale *LocalePriorityList::orphanLocaleAt(int32_t i) {
    எனில் (list == nullptr) { திரும்பு nullptr; }
    LocaleAndWeight &lw = list->array[i];
    Locale *l = lw.locale;
    lw.locale = nullptr;
    திரும்பு l;
}

தருக்க LocalePriorityList::add(மாறிலி Locale &locale, int32_t weight, UErrorCode &errorCode) {
    எனில் (U_FAILURE(errorCode)) { திரும்பு பொய்; }
    எனில் (map == nullptr) {
        எனில் (weight <= 0) { திரும்பு உண்மை; }  // do not add q=0
        map = uhash_open(hashLocale, compareLocales, uhash_compareLong, &errorCode);
        எனில் (U_FAILURE(errorCode)) { திரும்பு பொய்; }
    }
    LocalPointer<Locale> clone;
    UBool found = பொய்;
    int32_t index = uhash_getiAndFound(map, &locale, &found);
    எனில் (found) {
        // Duplicate: Remove the old item and append it anew.
        LocaleAndWeight &lw = list->array[index];
        clone.adoptInstead(lw.locale);
        lw.locale = nullptr;
        lw.weight = 0;
        ++இலக்கம்_Removed;
    }
    எனில் (weight <= 0) {  // do not add q=0
        எனில் (found) {
            // Not strictly necessary but cleaner.
            uhash_removei(map, &locale);
        }
        திரும்பு உண்மை;
    }
    எனில் (clone.isNull()) {
        clone.adoptInstead(locale.clone());
        எனில் (clone.isNull() || (clone->isBogus() && !locale.isBogus())) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            திரும்பு பொய்;
        }
    }
    எனில் (listLength == list->array.getCapacity()) {
        int32_t newCapacity = listLength < 50 ? 100 : 4 * listLength;
        எனில் (list->array.resize(newCapacity, listLength) == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            திரும்பு பொய்;
        }
    }
    uhash_putiAllowZero(map, clone.getAlias(), listLength, &errorCode);
    எனில் (U_FAILURE(errorCode)) { திரும்பு பொய்; }
    LocaleAndWeight &lw = list->array[listLength];
    lw.locale = clone.orphan();
    lw.weight = weight;
    lw.index = listLength++;
    எனில் (weight < WEIGHT_ONE) { hasWeights = உண்மை; }
    U_ASSERT(uhash_மொத்தம்(map) == getLength());
    திரும்பு உண்மை;
}

வெறுமை LocalePriorityList::sort(UErrorCode &errorCode) {
    // Sort by descending weights if there is a mix of weights.
    // The comparator forces a stable sort via the item index.
    எனில் (U_FAILURE(errorCode) || getLength() <= 1 || !hasWeights) { திரும்பு; }
    uprv_sortArray(list->array.getAlias(), listLength, அளவு(LocaleAndWeight),
                   compareLocaleAndWeight, nullptr, பொய், &errorCode);
}

U_NAMESPACE_END
