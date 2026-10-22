// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// localeprioritylist.cpp
// created: 2019jul11 Markus W. Scherer

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/localpointer.h"
#ಅನ್ತರ್ಭಾವ "unicode/locid.h"
#ಅನ್ತರ್ಭಾವ "unicode/stringpiece.h"
#ಅನ್ತರ್ಭಾವ "unicode/uobject.h"
#ಅನ್ತರ್ಭಾವ "charstr.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "localeprioritylist.h"
#ಅನ್ತರ್ಭಾವ "uarrsort.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"
#ಅನ್ತರ್ಭಾವ "uhash.h"

U_NAMESPACE_BEGIN

ನಾಮಾಕಾಶ {

int32_t hashLocale(ಧ್ರುವ UHashTok token) {
    ಧ್ರುವ auto* locale = static_cast<ಧ್ರುವ Locale*>(token.pointer);
    ಪ್ರತ್ಯಾಗಮ locale->hashCode();
}

UBool compareLocales(ಧ್ರುವ UHashTok t1, ಧ್ರುವ UHashTok t2) {
    ಧ್ರುವ auto* l1 = static_cast<ಧ್ರುವ Locale*>(t1.pointer);
    ಧ್ರುವ auto* l2 = static_cast<ಧ್ರುವ Locale*>(t2.pointer);
    ಪ್ರತ್ಯಾಗಮ *l1 == *l2;
}

constexpr int32_t WEIGHT_ONE = 1000;

ಸಂರಚನಾ LocaleAndWeight {
    Locale *locale;
    int32_t weight;  // 0..1000 = 0.0..1.0
    int32_t index;  // force stable sort

    int32_t compare(ಧ್ರುವ LocaleAndWeight &other) ಧ್ರುವ {
        int32_t diff = other.weight - weight;  // descending: other-this
        ಯದಿ (diff != 0) { ಪ್ರತ್ಯಾಗಮ diff; }
        ಪ್ರತ್ಯಾಗಮ index - other.index;
    }
};

int32_t U_CALLCONV
compareLocaleAndWeight(ಧ್ರುವ ಶೂನ್ಯ * /*context*/, ಧ್ರುವ ಶೂನ್ಯ *left, ಧ್ರುವ ಶೂನ್ಯ *ಬಲ) {
    ಪ್ರತ್ಯಾಗಮ static_cast<ಧ್ರುವ LocaleAndWeight *>(left)->
        compare(*static_cast<ಧ್ರುವ LocaleAndWeight *>(ಬಲ));
}

ಧ್ರುವ ವರ್ಣ *skipSpaces(ಧ್ರುವ ವರ್ಣ *p, ಧ್ರುವ ವರ್ಣ *limit) {
    ಯಾವತ್ (p < limit && *p == ' ') { ++p; }
    ಪ್ರತ್ಯಾಗಮ p;
}

int32_t findTagLength(ಧ್ರುವ ವರ್ಣ *p, ಧ್ರುವ ವರ್ಣ *limit) {
    // Look for accept-language delimiters.
    // Leave other validation up to the Locale constructor.
    ಧ್ರುವ ವರ್ಣ *q;
    ಪ್ರತಿ (q = p; q < limit; ++q) {
        ವರ್ಣ c = *q;
        ಯದಿ (c == ' ' || c == ',' || c == ';') { ಭಙ್ಗ; }
    }
    ಪ್ರತ್ಯಾಗಮ static_cast<int32_t>(q - p);
}

/**
 * Parses and returns a qvalue weight in millis.
 * Advances p to after the parsed substring.
 * Returns a negative value if parsing fails.
 */
int32_t parseWeight(ಧ್ರುವ ವರ್ಣ *&p, ಧ್ರುವ ವರ್ಣ *limit) {
    p = skipSpaces(p, limit);
    ವರ್ಣ c;
    ಯದಿ (p == limit || ((c = *p) != '0' && c != '1')) { ಪ್ರತ್ಯಾಗಮ -1; }
    int32_t weight = (c - '0') * 1000;
    ಯದಿ (++p == limit || *p != '.') { ಪ್ರತ್ಯಾಗಮ weight; }
    int32_t multiplier = 100;
    ಯಾವತ್ (++p != limit && '0' <= (c = *p) && c <= '9') {
        c -= '0';
        ಯದಿ (multiplier > 0) {
            weight += c * multiplier;
            multiplier /= 10;
        } ಅನ್ಯಥಾ ಯದಿ (multiplier == 0) {
            // round up
            ಯದಿ (c >= 5) { ++weight; }
            multiplier = -1;
        }  // else ignore further fraction digits
    }
    ಪ್ರತ್ಯಾಗಮ weight <= WEIGHT_ONE ? weight : -1;  // bad if > 1.0
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
ಸಂರಚನಾ LocaleAndWeightArray : ಸಾರ್ವಜನಿಕ UMemory {
    Maybe_ರಾಶಿ_Array<LocaleAndWeight, 20> array;
};

LocalePriorityList::LocalePriorityList(StringPiece s, UErrorCode &errorCode) {
    ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ; }
    list = ನೂತನ LocaleAndWeightArray();
    ಯದಿ (list == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        ಪ್ರತ್ಯಾಗಮ;
    }
    ಧ್ರುವ ವರ್ಣ *p = s.data();
    ಧ್ರುವ ವರ್ಣ *limit = p + s.length();
    ಯಾವತ್ ((p = skipSpaces(p, limit)) != limit) {
        ಯದಿ (*p == ',') {  // empty range field
            ++p;
            ಅನುವರ್ತ;
        }
        int32_t tagLength = findTagLength(p, limit);
        ಯದಿ (tagLength == 0) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            ಪ್ರತ್ಯಾಗಮ;
        }
        CharString tag(p, tagLength, errorCode);
        ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ; }
        Locale locale = Locale(tag.data());
        ಯದಿ (locale.isBogus()) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            ಪ್ರತ್ಯಾಗಮ;
        }
        int32_t weight = WEIGHT_ONE;
        ಯದಿ ((p = skipSpaces(p + tagLength, limit)) != limit && *p == ';') {
            ಯದಿ ((p = skipSpaces(p + 1, limit)) == limit || *p != 'q' ||
                    (p = skipSpaces(p + 1, limit)) == limit || *p != '=' ||
                    (++p, (weight = parseWeight(p, limit)) < 0)) {
                errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                ಪ್ರತ್ಯಾಗಮ;
            }
            p = skipSpaces(p, limit);
        }
        ಯದಿ (p != limit && *p != ',') {  // trailing junk
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            ಪ್ರತ್ಯಾಗಮ;
        }
        add(locale, weight, errorCode);
        ಯದಿ (p == limit) { ಭಙ್ಗ; }
        ++p;
    }
    sort(errorCode);
}

LocalePriorityList::~LocalePriorityList() {
    ಯದಿ (list != nullptr) {
        ಪ್ರತಿ (int32_t i = 0; i < listLength; ++i) {
            delete list->array[i].locale;
        }
        delete list;
    }
    uhash_close(map);
}

ಧ್ರುವ Locale *LocalePriorityList::localeAt(int32_t i) ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ list->array[i].locale;
}

Locale *LocalePriorityList::orphanLocaleAt(int32_t i) {
    ಯದಿ (list == nullptr) { ಪ್ರತ್ಯಾಗಮ nullptr; }
    LocaleAndWeight &lw = list->array[i];
    Locale *l = lw.locale;
    lw.locale = nullptr;
    ಪ್ರತ್ಯಾಗಮ l;
}

ತಾರ್ಕಿಕ LocalePriorityList::add(ಧ್ರುವ Locale &locale, int32_t weight, UErrorCode &errorCode) {
    ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ; }
    ಯದಿ (map == nullptr) {
        ಯದಿ (weight <= 0) { ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ; }  // do not add q=0
        map = uhash_open(hashLocale, compareLocales, uhash_compareLong, &errorCode);
        ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ; }
    }
    LocalPointer<Locale> clone;
    UBool found = ಅಸತ್ಯ;
    int32_t index = uhash_getiAndFound(map, &locale, &found);
    ಯದಿ (found) {
        // Duplicate: Remove the old item and append it anew.
        LocaleAndWeight &lw = list->array[index];
        clone.adoptInstead(lw.locale);
        lw.locale = nullptr;
        lw.weight = 0;
        ++ಅಂಕೆ_Removed;
    }
    ಯದಿ (weight <= 0) {  // do not add q=0
        ಯದಿ (found) {
            // Not strictly necessary but cleaner.
            uhash_removei(map, &locale);
        }
        ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
    }
    ಯದಿ (clone.isNull()) {
        clone.adoptInstead(locale.clone());
        ಯದಿ (clone.isNull() || (clone->isBogus() && !locale.isBogus())) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
        }
    }
    ಯದಿ (listLength == list->array.getCapacity()) {
        int32_t newCapacity = listLength < 50 ? 100 : 4 * listLength;
        ಯದಿ (list->array.resize(newCapacity, listLength) == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
        }
    }
    uhash_putiAllowZero(map, clone.getAlias(), listLength, &errorCode);
    ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ; }
    LocaleAndWeight &lw = list->array[listLength];
    lw.locale = clone.orphan();
    lw.weight = weight;
    lw.index = listLength++;
    ಯದಿ (weight < WEIGHT_ONE) { hasWeights = ಸತ್ಯ; }
    U_ASSERT(uhash_ಎಣಿಕೆ(map) == getLength());
    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

ಶೂನ್ಯ LocalePriorityList::sort(UErrorCode &errorCode) {
    // Sort by descending weights if there is a mix of weights.
    // The comparator forces a stable sort via the item index.
    ಯದಿ (U_FAILURE(errorCode) || getLength() <= 1 || !hasWeights) { ಪ್ರತ್ಯಾಗಮ; }
    uprv_sortArray(list->array.getAlias(), listLength, ಪರಿಮಾಣ(LocaleAndWeight),
                   compareLocaleAndWeight, nullptr, ಅಸತ್ಯ, &errorCode);
}

U_NAMESPACE_END
