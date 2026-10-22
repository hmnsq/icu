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

int32_t hashLocale(अचर UHashTok token) {
    अचर स्वतः* locale = static_cast<अचर Locale*>(token.pointer);
    फर्काऊ locale->hashCode();
}

UBool compareLocales(अचर UHashTok t1, अचर UHashTok t2) {
    अचर स्वतः* l1 = static_cast<अचर Locale*>(t1.pointer);
    अचर स्वतः* l2 = static_cast<अचर Locale*>(t2.pointer);
    फर्काऊ *l1 == *l2;
}

constexpr int32_t WEIGHT_ONE = 1000;

संरचना LocaleAndWeight {
    Locale *locale;
    int32_t weight;  // 0..1000 = 0.0..1.0
    int32_t index;  // force stable sort

    int32_t compare(अचर LocaleAndWeight &other) अचर {
        int32_t diff = other.weight - weight;  // descending: other-this
        यदि (diff != 0) { फर्काऊ diff; }
        फर्काऊ index - other.index;
    }
};

int32_t U_CALLCONV
compareLocaleAndWeight(अचर शून्य * /*context*/, अचर शून्य *left, अचर शून्य *दायाँ) {
    फर्काऊ static_cast<अचर LocaleAndWeight *>(left)->
        compare(*static_cast<अचर LocaleAndWeight *>(दायाँ));
}

अचर वर्ण *skipSpaces(अचर वर्ण *p, अचर वर्ण *limit) {
    जबसम्म (p < limit && *p == ' ') { ++p; }
    फर्काऊ p;
}

int32_t findTagLength(अचर वर्ण *p, अचर वर्ण *limit) {
    // Look for accept-language delimiters.
    // Leave other validation up to the Locale constructor.
    अचर वर्ण *q;
    लागि (q = p; q < limit; ++q) {
        वर्ण c = *q;
        यदि (c == ' ' || c == ',' || c == ';') { रोक; }
    }
    फर्काऊ static_cast<int32_t>(q - p);
}

/**
 * Parses and returns a qvalue weight in millis.
 * Advances p to after the parsed substring.
 * Returns a negative value if parsing fails.
 */
int32_t parseWeight(अचर वर्ण *&p, अचर वर्ण *limit) {
    p = skipSpaces(p, limit);
    वर्ण c;
    यदि (p == limit || ((c = *p) != '0' && c != '1')) { फर्काऊ -1; }
    int32_t weight = (c - '0') * 1000;
    यदि (++p == limit || *p != '.') { फर्काऊ weight; }
    int32_t multiplier = 100;
    जबसम्म (++p != limit && '0' <= (c = *p) && c <= '9') {
        c -= '0';
        यदि (multiplier > 0) {
            weight += c * multiplier;
            multiplier /= 10;
        } अन्यथा यदि (multiplier == 0) {
            // round up
            यदि (c >= 5) { ++weight; }
            multiplier = -1;
        }  // else ignore further fraction digits
    }
    फर्काऊ weight <= WEIGHT_ONE ? weight : -1;  // bad if > 1.0
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
    Maybe_थाक_Array<LocaleAndWeight, 20> array;
};

LocalePriorityList::LocalePriorityList(StringPiece s, UErrorCode &errorCode) {
    यदि (U_FAILURE(errorCode)) { फर्काऊ; }
    list = नूतन LocaleAndWeightArray();
    यदि (list == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        फर्काऊ;
    }
    अचर वर्ण *p = s.data();
    अचर वर्ण *limit = p + s.length();
    जबसम्म ((p = skipSpaces(p, limit)) != limit) {
        यदि (*p == ',') {  // empty range field
            ++p;
            जारी;
        }
        int32_t tagLength = findTagLength(p, limit);
        यदि (tagLength == 0) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            फर्काऊ;
        }
        CharString tag(p, tagLength, errorCode);
        यदि (U_FAILURE(errorCode)) { फर्काऊ; }
        Locale locale = Locale(tag.data());
        यदि (locale.isBogus()) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            फर्काऊ;
        }
        int32_t weight = WEIGHT_ONE;
        यदि ((p = skipSpaces(p + tagLength, limit)) != limit && *p == ';') {
            यदि ((p = skipSpaces(p + 1, limit)) == limit || *p != 'q' ||
                    (p = skipSpaces(p + 1, limit)) == limit || *p != '=' ||
                    (++p, (weight = parseWeight(p, limit)) < 0)) {
                errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                फर्काऊ;
            }
            p = skipSpaces(p, limit);
        }
        यदि (p != limit && *p != ',') {  // trailing junk
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            फर्काऊ;
        }
        add(locale, weight, errorCode);
        यदि (p == limit) { रोक; }
        ++p;
    }
    sort(errorCode);
}

LocalePriorityList::~LocalePriorityList() {
    यदि (list != nullptr) {
        लागि (int32_t i = 0; i < listLength; ++i) {
            delete list->array[i].locale;
        }
        delete list;
    }
    uhash_close(map);
}

अचर Locale *LocalePriorityList::localeAt(int32_t i) अचर {
    फर्काऊ list->array[i].locale;
}

Locale *LocalePriorityList::orphanLocaleAt(int32_t i) {
    यदि (list == nullptr) { फर्काऊ nullptr; }
    LocaleAndWeight &lw = list->array[i];
    Locale *l = lw.locale;
    lw.locale = nullptr;
    फर्काऊ l;
}

तार्किक LocalePriorityList::add(अचर Locale &locale, int32_t weight, UErrorCode &errorCode) {
    यदि (U_FAILURE(errorCode)) { फर्काऊ असत्य; }
    यदि (map == nullptr) {
        यदि (weight <= 0) { फर्काऊ सत्य; }  // do not add q=0
        map = uhash_open(hashLocale, compareLocales, uhash_compareLong, &errorCode);
        यदि (U_FAILURE(errorCode)) { फर्काऊ असत्य; }
    }
    LocalPointer<Locale> clone;
    UBool found = असत्य;
    int32_t index = uhash_getiAndFound(map, &locale, &found);
    यदि (found) {
        // Duplicate: Remove the old item and append it anew.
        LocaleAndWeight &lw = list->array[index];
        clone.adoptInstead(lw.locale);
        lw.locale = nullptr;
        lw.weight = 0;
        ++अङ्क_Removed;
    }
    यदि (weight <= 0) {  // do not add q=0
        यदि (found) {
            // Not strictly necessary but cleaner.
            uhash_removei(map, &locale);
        }
        फर्काऊ सत्य;
    }
    यदि (clone.isNull()) {
        clone.adoptInstead(locale.clone());
        यदि (clone.isNull() || (clone->isBogus() && !locale.isBogus())) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            फर्काऊ असत्य;
        }
    }
    यदि (listLength == list->array.getCapacity()) {
        int32_t newCapacity = listLength < 50 ? 100 : 4 * listLength;
        यदि (list->array.resize(newCapacity, listLength) == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            फर्काऊ असत्य;
        }
    }
    uhash_putiAllowZero(map, clone.getAlias(), listLength, &errorCode);
    यदि (U_FAILURE(errorCode)) { फर्काऊ असत्य; }
    LocaleAndWeight &lw = list->array[listLength];
    lw.locale = clone.orphan();
    lw.weight = weight;
    lw.index = listLength++;
    यदि (weight < WEIGHT_ONE) { hasWeights = सत्य; }
    U_ASSERT(uhash_गन्ती(map) == getLength());
    फर्काऊ सत्य;
}

शून्य LocalePriorityList::sort(UErrorCode &errorCode) {
    // Sort by descending weights if there is a mix of weights.
    // The comparator forces a stable sort via the item index.
    यदि (U_FAILURE(errorCode) || getLength() <= 1 || !hasWeights) { फर्काऊ; }
    uprv_sortArray(list->array.getAlias(), listLength, परिमाण(LocaleAndWeight),
                   compareLocaleAndWeight, nullptr, असत्य, &errorCode);
}

U_NAMESPACE_END
