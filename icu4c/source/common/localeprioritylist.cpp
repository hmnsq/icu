// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// localeprioritylist.cpp
// created: 2019jul11 Markus W. Scherer

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/localpointer.h"
#अन्तर्भाव "unicode/locid.h"
#अन्तर्भाव "unicode/stringpiece.h"
#अन्तर्भाव "unicode/uobject.h"
#अन्तर्भाव "charstr.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "localeprioritylist.h"
#अन्तर्भाव "uarrsort.h"
#अन्तर्भाव "uassert.h"
#अन्तर्भाव "uhash.h"

U_NAMESPACE_BEGIN

नामाकाश {

int32_t hashLocale(ध्रुव UHashTok token) {
    ध्रुव auto* locale = static_cast<ध्रुव Locale*>(token.pointer);
    पटिनिवत्त locale->hashCode();
}

UBool compareLocales(ध्रुव UHashTok t1, ध्रुव UHashTok t2) {
    ध्रुव auto* l1 = static_cast<ध्रुव Locale*>(t1.pointer);
    ध्रुव auto* l2 = static_cast<ध्रुव Locale*>(t2.pointer);
    पटिनिवत्त *l1 == *l2;
}

constexpr int32_t WEIGHT_ONE = 1000;

संरचना LocaleAndWeight {
    Locale *locale;
    int32_t weight;  // 0..1000 = 0.0..1.0
    int32_t index;  // force stable sort

    int32_t compare(ध्रुव LocaleAndWeight &other) ध्रुव {
        int32_t diff = other.weight - weight;  // descending: other-this
        सचे (diff != 0) { पटिनिवत्त diff; }
        पटिनिवत्त index - other.index;
    }
};

int32_t U_CALLCONV
compareLocaleAndWeight(ध्रुव शून्य * /*context*/, ध्रुव शून्य *left, ध्रुव शून्य *दक्षिणः) {
    पटिनिवत्त static_cast<ध्रुव LocaleAndWeight *>(left)->
        compare(*static_cast<ध्रुव LocaleAndWeight *>(दक्षिणः));
}

ध्रुव वर्ण *skipSpaces(ध्रुव वर्ण *p, ध्रुव वर्ण *limit) {
    याव (p < limit && *p == ' ') { ++p; }
    पटिनिवत्त p;
}

int32_t findTagLength(ध्रुव वर्ण *p, ध्रुव वर्ण *limit) {
    // Look for accept-language delimiters.
    // Leave other validation up to the Locale constructor.
    ध्रुव वर्ण *q;
    अत्थाय (q = p; q < limit; ++q) {
        वर्ण c = *q;
        सचे (c == ' ' || c == ',' || c == ';') { भङ्ग; }
    }
    पटिनिवत्त static_cast<int32_t>(q - p);
}

/**
 * Parses and returns a qvalue weight in millis.
 * Advances p to after the parsed substring.
 * Returns a negative value if parsing fails.
 */
int32_t parseWeight(ध्रुव वर्ण *&p, ध्रुव वर्ण *limit) {
    p = skipSpaces(p, limit);
    वर्ण c;
    सचे (p == limit || ((c = *p) != '0' && c != '1')) { पटिनिवत्त -1; }
    int32_t weight = (c - '0') * 1000;
    सचे (++p == limit || *p != '.') { पटिनिवत्त weight; }
    int32_t multiplier = 100;
    याव (++p != limit && '0' <= (c = *p) && c <= '9') {
        c -= '0';
        सचे (multiplier > 0) {
            weight += c * multiplier;
            multiplier /= 10;
        } अञ्ञथा सचे (multiplier == 0) {
            // round up
            सचे (c >= 5) { ++weight; }
            multiplier = -1;
        }  // else ignore further fraction digits
    }
    पटिनिवत्त weight <= WEIGHT_ONE ? weight : -1;  // bad if > 1.0
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
संरचना LocaleAndWeightArray : सार्वजनिक UMemory {
    Maybe_राशिः_Array<LocaleAndWeight, 20> array;
};

LocalePriorityList::LocalePriorityList(StringPiece s, UErrorCode &errorCode) {
    सचे (U_FAILURE(errorCode)) { पटिनिवत्त; }
    list = नूतन LocaleAndWeightArray();
    सचे (list == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        पटिनिवत्त;
    }
    ध्रुव वर्ण *p = s.data();
    ध्रुव वर्ण *limit = p + s.length();
    याव ((p = skipSpaces(p, limit)) != limit) {
        सचे (*p == ',') {  // empty range field
            ++p;
            अनुवर्त;
        }
        int32_t tagLength = findTagLength(p, limit);
        सचे (tagLength == 0) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            पटिनिवत्त;
        }
        CharString tag(p, tagLength, errorCode);
        सचे (U_FAILURE(errorCode)) { पटिनिवत्त; }
        Locale locale = Locale(tag.data());
        सचे (locale.isBogus()) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            पटिनिवत्त;
        }
        int32_t weight = WEIGHT_ONE;
        सचे ((p = skipSpaces(p + tagLength, limit)) != limit && *p == ';') {
            सचे ((p = skipSpaces(p + 1, limit)) == limit || *p != 'q' ||
                    (p = skipSpaces(p + 1, limit)) == limit || *p != '=' ||
                    (++p, (weight = parseWeight(p, limit)) < 0)) {
                errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                पटिनिवत्त;
            }
            p = skipSpaces(p, limit);
        }
        सचे (p != limit && *p != ',') {  // trailing junk
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            पटिनिवत्त;
        }
        add(locale, weight, errorCode);
        सचे (p == limit) { भङ्ग; }
        ++p;
    }
    sort(errorCode);
}

LocalePriorityList::~LocalePriorityList() {
    सचे (list != nullptr) {
        अत्थाय (int32_t i = 0; i < listLength; ++i) {
            delete list->array[i].locale;
        }
        delete list;
    }
    uhash_close(map);
}

ध्रुव Locale *LocalePriorityList::localeAt(int32_t i) ध्रुव {
    पटिनिवत्त list->array[i].locale;
}

Locale *LocalePriorityList::orphanLocaleAt(int32_t i) {
    सचे (list == nullptr) { पटिनिवत्त nullptr; }
    LocaleAndWeight &lw = list->array[i];
    Locale *l = lw.locale;
    lw.locale = nullptr;
    पटिनिवत्त l;
}

तार्किक LocalePriorityList::add(ध्रुव Locale &locale, int32_t weight, UErrorCode &errorCode) {
    सचे (U_FAILURE(errorCode)) { पटिनिवत्त मिच्छा; }
    सचे (map == nullptr) {
        सचे (weight <= 0) { पटिनिवत्त सच्च; }  // do not add q=0
        map = uhash_open(hashLocale, compareLocales, uhash_compareLong, &errorCode);
        सचे (U_FAILURE(errorCode)) { पटिनिवत्त मिच्छा; }
    }
    LocalPointer<Locale> clone;
    UBool found = मिच्छा;
    int32_t index = uhash_getiAndFound(map, &locale, &found);
    सचे (found) {
        // Duplicate: Remove the old item and append it anew.
        LocaleAndWeight &lw = list->array[index];
        clone.adoptInstead(lw.locale);
        lw.locale = nullptr;
        lw.weight = 0;
        ++अङ्कः_Removed;
    }
    सचे (weight <= 0) {  // do not add q=0
        सचे (found) {
            // Not strictly necessary but cleaner.
            uhash_removei(map, &locale);
        }
        पटिनिवत्त सच्च;
    }
    सचे (clone.isNull()) {
        clone.adoptInstead(locale.clone());
        सचे (clone.isNull() || (clone->isBogus() && !locale.isBogus())) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            पटिनिवत्त मिच्छा;
        }
    }
    सचे (listLength == list->array.getCapacity()) {
        int32_t newCapacity = listLength < 50 ? 100 : 4 * listLength;
        सचे (list->array.resize(newCapacity, listLength) == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            पटिनिवत्त मिच्छा;
        }
    }
    uhash_putiAllowZero(map, clone.getAlias(), listLength, &errorCode);
    सचे (U_FAILURE(errorCode)) { पटिनिवत्त मिच्छा; }
    LocaleAndWeight &lw = list->array[listLength];
    lw.locale = clone.orphan();
    lw.weight = weight;
    lw.index = listLength++;
    सचे (weight < WEIGHT_ONE) { hasWeights = सच्च; }
    U_ASSERT(uhash_गणनम्(map) == getLength());
    पटिनिवत्त सच्च;
}

शून्य LocalePriorityList::sort(UErrorCode &errorCode) {
    // Sort by descending weights if there is a mix of weights.
    // The comparator forces a stable sort via the item index.
    सचे (U_FAILURE(errorCode) || getLength() <= 1 || !hasWeights) { पटिनिवत्त; }
    uprv_sortArray(list->array.getAlias(), listLength, परिमाण(LocaleAndWeight),
                   compareLocaleAndWeight, nullptr, मिच्छा, &errorCode);
}

U_NAMESPACE_END
