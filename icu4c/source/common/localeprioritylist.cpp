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

int32_t hashLocale(स्थिरांक UHashTok token) {
    स्थिरांक स्वतः* locale = static_cast<स्थिरांक Locale*>(token.pointer);
    परत locale->hashCode();
}

UBool compareLocales(स्थिरांक UHashTok t1, स्थिरांक UHashTok t2) {
    स्थिरांक स्वतः* l1 = static_cast<स्थिरांक Locale*>(t1.pointer);
    स्थिरांक स्वतः* l2 = static_cast<स्थिरांक Locale*>(t2.pointer);
    परत *l1 == *l2;
}

constexpr int32_t WEIGHT_ONE = 1000;

संरचना LocaleAndWeight {
    Locale *locale;
    int32_t weight;  // 0..1000 = 0.0..1.0
    int32_t index;  // force stable sort

    int32_t compare(स्थिरांक LocaleAndWeight &other) स्थिरांक {
        int32_t diff = other.weight - weight;  // descending: other-this
        जर (diff != 0) { परत diff; }
        परत index - other.index;
    }
};

int32_t U_CALLCONV
compareLocaleAndWeight(स्थिरांक शून्य * /*context*/, स्थिरांक शून्य *left, स्थिरांक शून्य *उजवा) {
    परत static_cast<स्थिरांक LocaleAndWeight *>(left)->
        compare(*static_cast<स्थिरांक LocaleAndWeight *>(उजवा));
}

स्थिरांक अक्षर *skipSpaces(स्थिरांक अक्षर *p, स्थिरांक अक्षर *limit) {
    पर्यंत (p < limit && *p == ' ') { ++p; }
    परत p;
}

int32_t findTagLength(स्थिरांक अक्षर *p, स्थिरांक अक्षर *limit) {
    // Look for accept-language delimiters.
    // Leave other validation up to the Locale constructor.
    स्थिरांक अक्षर *q;
    साठी (q = p; q < limit; ++q) {
        अक्षर c = *q;
        जर (c == ' ' || c == ',' || c == ';') { थांबा; }
    }
    परत static_cast<int32_t>(q - p);
}

/**
 * Parses and returns a qvalue weight in millis.
 * Advances p to after the parsed substring.
 * Returns a negative value if parsing fails.
 */
int32_t parseWeight(स्थिरांक अक्षर *&p, स्थिरांक अक्षर *limit) {
    p = skipSpaces(p, limit);
    अक्षर c;
    जर (p == limit || ((c = *p) != '0' && c != '1')) { परत -1; }
    int32_t weight = (c - '0') * 1000;
    जर (++p == limit || *p != '.') { परत weight; }
    int32_t multiplier = 100;
    पर्यंत (++p != limit && '0' <= (c = *p) && c <= '9') {
        c -= '0';
        जर (multiplier > 0) {
            weight += c * multiplier;
            multiplier /= 10;
        } अन्यथा जर (multiplier == 0) {
            // round up
            जर (c >= 5) { ++weight; }
            multiplier = -1;
        }  // else ignore further fraction digits
    }
    परत weight <= WEIGHT_ONE ? weight : -1;  // bad if > 1.0
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
    Maybe_चळत_Array<LocaleAndWeight, 20> array;
};

LocalePriorityList::LocalePriorityList(StringPiece s, UErrorCode &errorCode) {
    जर (U_FAILURE(errorCode)) { परत; }
    list = नवीन LocaleAndWeightArray();
    जर (list == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        परत;
    }
    स्थिरांक अक्षर *p = s.data();
    स्थिरांक अक्षर *limit = p + s.length();
    पर्यंत ((p = skipSpaces(p, limit)) != limit) {
        जर (*p == ',') {  // empty range field
            ++p;
            सुरूठेवा;
        }
        int32_t tagLength = findTagLength(p, limit);
        जर (tagLength == 0) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            परत;
        }
        CharString tag(p, tagLength, errorCode);
        जर (U_FAILURE(errorCode)) { परत; }
        Locale locale = Locale(tag.data());
        जर (locale.isBogus()) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            परत;
        }
        int32_t weight = WEIGHT_ONE;
        जर ((p = skipSpaces(p + tagLength, limit)) != limit && *p == ';') {
            जर ((p = skipSpaces(p + 1, limit)) == limit || *p != 'q' ||
                    (p = skipSpaces(p + 1, limit)) == limit || *p != '=' ||
                    (++p, (weight = parseWeight(p, limit)) < 0)) {
                errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                परत;
            }
            p = skipSpaces(p, limit);
        }
        जर (p != limit && *p != ',') {  // trailing junk
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            परत;
        }
        add(locale, weight, errorCode);
        जर (p == limit) { थांबा; }
        ++p;
    }
    sort(errorCode);
}

LocalePriorityList::~LocalePriorityList() {
    जर (list != nullptr) {
        साठी (int32_t i = 0; i < listLength; ++i) {
            delete list->array[i].locale;
        }
        delete list;
    }
    uhash_close(map);
}

स्थिरांक Locale *LocalePriorityList::localeAt(int32_t i) स्थिरांक {
    परत list->array[i].locale;
}

Locale *LocalePriorityList::orphanLocaleAt(int32_t i) {
    जर (list == nullptr) { परत nullptr; }
    LocaleAndWeight &lw = list->array[i];
    Locale *l = lw.locale;
    lw.locale = nullptr;
    परत l;
}

तार्किक LocalePriorityList::add(स्थिरांक Locale &locale, int32_t weight, UErrorCode &errorCode) {
    जर (U_FAILURE(errorCode)) { परत असत्य; }
    जर (map == nullptr) {
        जर (weight <= 0) { परत सत्य; }  // do not add q=0
        map = uhash_open(hashLocale, compareLocales, uhash_compareLong, &errorCode);
        जर (U_FAILURE(errorCode)) { परत असत्य; }
    }
    LocalPointer<Locale> clone;
    UBool found = असत्य;
    int32_t index = uhash_getiAndFound(map, &locale, &found);
    जर (found) {
        // Duplicate: Remove the old item and append it anew.
        LocaleAndWeight &lw = list->array[index];
        clone.adoptInstead(lw.locale);
        lw.locale = nullptr;
        lw.weight = 0;
        ++अंक_Removed;
    }
    जर (weight <= 0) {  // do not add q=0
        जर (found) {
            // Not strictly necessary but cleaner.
            uhash_removei(map, &locale);
        }
        परत सत्य;
    }
    जर (clone.isNull()) {
        clone.adoptInstead(locale.clone());
        जर (clone.isNull() || (clone->isBogus() && !locale.isBogus())) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            परत असत्य;
        }
    }
    जर (listLength == list->array.getCapacity()) {
        int32_t newCapacity = listLength < 50 ? 100 : 4 * listLength;
        जर (list->array.resize(newCapacity, listLength) == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            परत असत्य;
        }
    }
    uhash_putiAllowZero(map, clone.getAlias(), listLength, &errorCode);
    जर (U_FAILURE(errorCode)) { परत असत्य; }
    LocaleAndWeight &lw = list->array[listLength];
    lw.locale = clone.orphan();
    lw.weight = weight;
    lw.index = listLength++;
    जर (weight < WEIGHT_ONE) { hasWeights = सत्य; }
    U_ASSERT(uhash_मोजणी(map) == getLength());
    परत सत्य;
}

शून्य LocalePriorityList::sort(UErrorCode &errorCode) {
    // Sort by descending weights if there is a mix of weights.
    // The comparator forces a stable sort via the item index.
    जर (U_FAILURE(errorCode) || getLength() <= 1 || !hasWeights) { परत; }
    uprv_sortArray(list->array.getAlias(), listLength, आकार(LocaleAndWeight),
                   compareLocaleAndWeight, nullptr, असत्य, &errorCode);
}

U_NAMESPACE_END
