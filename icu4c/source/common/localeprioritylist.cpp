// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// localeprioritylist.cpp
// created: 2019jul11 Markus W. Scherer

#അന്തര്ഭാവ "unicode/utypes.h"
#അന്തര്ഭാവ "unicode/localpointer.h"
#അന്തര്ഭാവ "unicode/locid.h"
#അന്തര്ഭാവ "unicode/stringpiece.h"
#അന്തര്ഭാവ "unicode/uobject.h"
#അന്തര്ഭാവ "charstr.h"
#അന്തര്ഭാവ "cmemory.h"
#അന്തര്ഭാവ "localeprioritylist.h"
#അന്തര്ഭാവ "uarrsort.h"
#അന്തര്ഭാവ "uassert.h"
#അന്തര്ഭാവ "uhash.h"

U_NAMESPACE_BEGIN

നാമാകാശ {

int32_t hashLocale(സ്ഥിരം UHashTok token) {
    സ്ഥിരം auto* locale = static_cast<സ്ഥിരം Locale*>(token.pointer);
    തിരികെ locale->hashCode();
}

UBool compareLocales(സ്ഥിരം UHashTok t1, സ്ഥിരം UHashTok t2) {
    സ്ഥിരം auto* l1 = static_cast<സ്ഥിരം Locale*>(t1.pointer);
    സ്ഥിരം auto* l2 = static_cast<സ്ഥിരം Locale*>(t2.pointer);
    തിരികെ *l1 == *l2;
}

constexpr int32_t WEIGHT_ONE = 1000;

ഘടന LocaleAndWeight {
    Locale *locale;
    int32_t weight;  // 0..1000 = 0.0..1.0
    int32_t index;  // force stable sort

    int32_t compare(സ്ഥിരം LocaleAndWeight &other) സ്ഥിരം {
        int32_t diff = other.weight - weight;  // descending: other-this
        എങ്കിൽ (diff != 0) { തിരികെ diff; }
        തിരികെ index - other.index;
    }
};

int32_t U_CALLCONV
compareLocaleAndWeight(സ്ഥിരം ശൂന്യം * /*context*/, സ്ഥിരം ശൂന്യം *left, സ്ഥിരം ശൂന്യം *വലത്) {
    തിരികെ static_cast<സ്ഥിരം LocaleAndWeight *>(left)->
        compare(*static_cast<സ്ഥിരം LocaleAndWeight *>(വലത്));
}

സ്ഥിരം അക്ഷരം *skipSpaces(സ്ഥിരം അക്ഷരം *p, സ്ഥിരം അക്ഷരം *limit) {
    ആകുമ്പോൾ (p < limit && *p == ' ') { ++p; }
    തിരികെ p;
}

int32_t findTagLength(സ്ഥിരം അക്ഷരം *p, സ്ഥിരം അക്ഷരം *limit) {
    // Look for accept-language delimiters.
    // Leave other validation up to the Locale constructor.
    സ്ഥിരം അക്ഷരം *q;
    വേണ്ടി (q = p; q < limit; ++q) {
        അക്ഷരം c = *q;
        എങ്കിൽ (c == ' ' || c == ',' || c == ';') { നിർത്തുക; }
    }
    തിരികെ static_cast<int32_t>(q - p);
}

/**
 * Parses and returns a qvalue weight in millis.
 * Advances p to after the parsed substring.
 * Returns a negative value if parsing fails.
 */
int32_t parseWeight(സ്ഥിരം അക്ഷരം *&p, സ്ഥിരം അക്ഷരം *limit) {
    p = skipSpaces(p, limit);
    അക്ഷരം c;
    എങ്കിൽ (p == limit || ((c = *p) != '0' && c != '1')) { തിരികെ -1; }
    int32_t weight = (c - '0') * 1000;
    എങ്കിൽ (++p == limit || *p != '.') { തിരികെ weight; }
    int32_t multiplier = 100;
    ആകുമ്പോൾ (++p != limit && '0' <= (c = *p) && c <= '9') {
        c -= '0';
        എങ്കിൽ (multiplier > 0) {
            weight += c * multiplier;
            multiplier /= 10;
        } അല്ലെങ്കിൽ എങ്കിൽ (multiplier == 0) {
            // round up
            എങ്കിൽ (c >= 5) { ++weight; }
            multiplier = -1;
        }  // else ignore further fraction digits
    }
    തിരികെ weight <= WEIGHT_ONE ? weight : -1;  // bad if > 1.0
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
ഘടന LocaleAndWeightArray : പൊതു UMemory {
    Maybe_അട്ടി_Array<LocaleAndWeight, 20> array;
};

LocalePriorityList::LocalePriorityList(StringPiece s, UErrorCode &errorCode) {
    എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ; }
    list = പുതിയ LocaleAndWeightArray();
    എങ്കിൽ (list == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        തിരികെ;
    }
    സ്ഥിരം അക്ഷരം *p = s.data();
    സ്ഥിരം അക്ഷരം *limit = p + s.length();
    ആകുമ്പോൾ ((p = skipSpaces(p, limit)) != limit) {
        എങ്കിൽ (*p == ',') {  // empty range field
            ++p;
            തുടരുക;
        }
        int32_t tagLength = findTagLength(p, limit);
        എങ്കിൽ (tagLength == 0) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            തിരികെ;
        }
        CharString tag(p, tagLength, errorCode);
        എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ; }
        Locale locale = Locale(tag.data());
        എങ്കിൽ (locale.isBogus()) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            തിരികെ;
        }
        int32_t weight = WEIGHT_ONE;
        എങ്കിൽ ((p = skipSpaces(p + tagLength, limit)) != limit && *p == ';') {
            എങ്കിൽ ((p = skipSpaces(p + 1, limit)) == limit || *p != 'q' ||
                    (p = skipSpaces(p + 1, limit)) == limit || *p != '=' ||
                    (++p, (weight = parseWeight(p, limit)) < 0)) {
                errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                തിരികെ;
            }
            p = skipSpaces(p, limit);
        }
        എങ്കിൽ (p != limit && *p != ',') {  // trailing junk
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            തിരികെ;
        }
        add(locale, weight, errorCode);
        എങ്കിൽ (p == limit) { നിർത്തുക; }
        ++p;
    }
    sort(errorCode);
}

LocalePriorityList::~LocalePriorityList() {
    എങ്കിൽ (list != nullptr) {
        വേണ്ടി (int32_t i = 0; i < listLength; ++i) {
            delete list->array[i].locale;
        }
        delete list;
    }
    uhash_close(map);
}

സ്ഥിരം Locale *LocalePriorityList::localeAt(int32_t i) സ്ഥിരം {
    തിരികെ list->array[i].locale;
}

Locale *LocalePriorityList::orphanLocaleAt(int32_t i) {
    എങ്കിൽ (list == nullptr) { തിരികെ nullptr; }
    LocaleAndWeight &lw = list->array[i];
    Locale *l = lw.locale;
    lw.locale = nullptr;
    തിരികെ l;
}

തർക്കം LocalePriorityList::add(സ്ഥിരം Locale &locale, int32_t weight, UErrorCode &errorCode) {
    എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ അസത്യം; }
    എങ്കിൽ (map == nullptr) {
        എങ്കിൽ (weight <= 0) { തിരികെ സത്യം; }  // do not add q=0
        map = uhash_open(hashLocale, compareLocales, uhash_compareLong, &errorCode);
        എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ അസത്യം; }
    }
    LocalPointer<Locale> clone;
    UBool found = അസത്യം;
    int32_t index = uhash_getiAndFound(map, &locale, &found);
    എങ്കിൽ (found) {
        // Duplicate: Remove the old item and append it anew.
        LocaleAndWeight &lw = list->array[index];
        clone.adoptInstead(lw.locale);
        lw.locale = nullptr;
        lw.weight = 0;
        ++അക്കം_Removed;
    }
    എങ്കിൽ (weight <= 0) {  // do not add q=0
        എങ്കിൽ (found) {
            // Not strictly necessary but cleaner.
            uhash_removei(map, &locale);
        }
        തിരികെ സത്യം;
    }
    എങ്കിൽ (clone.isNull()) {
        clone.adoptInstead(locale.clone());
        എങ്കിൽ (clone.isNull() || (clone->isBogus() && !locale.isBogus())) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            തിരികെ അസത്യം;
        }
    }
    എങ്കിൽ (listLength == list->array.getCapacity()) {
        int32_t newCapacity = listLength < 50 ? 100 : 4 * listLength;
        എങ്കിൽ (list->array.resize(newCapacity, listLength) == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            തിരികെ അസത്യം;
        }
    }
    uhash_putiAllowZero(map, clone.getAlias(), listLength, &errorCode);
    എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ അസത്യം; }
    LocaleAndWeight &lw = list->array[listLength];
    lw.locale = clone.orphan();
    lw.weight = weight;
    lw.index = listLength++;
    എങ്കിൽ (weight < WEIGHT_ONE) { hasWeights = സത്യം; }
    U_ASSERT(uhash_എണ്ണം(map) == getLength());
    തിരികെ സത്യം;
}

ശൂന്യം LocalePriorityList::sort(UErrorCode &errorCode) {
    // Sort by descending weights if there is a mix of weights.
    // The comparator forces a stable sort via the item index.
    എങ്കിൽ (U_FAILURE(errorCode) || getLength() <= 1 || !hasWeights) { തിരികെ; }
    uprv_sortArray(list->array.getAlias(), listLength, വലുപ്പം(LocaleAndWeight),
                   compareLocaleAndWeight, nullptr, അസത്യം, &errorCode);
}

U_NAMESPACE_END
