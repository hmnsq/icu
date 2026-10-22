// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// localeprioritylist.cpp
// created: 2019jul11 Markus W. Scherer

#અન્તર્ભાવ "unicode/utypes.h"
#અન્તર્ભાવ "unicode/localpointer.h"
#અન્તર્ભાવ "unicode/locid.h"
#અન્તર્ભાવ "unicode/stringpiece.h"
#અન્તર્ભાવ "unicode/uobject.h"
#અન્તર્ભાવ "charstr.h"
#અન્તર્ભાવ "cmemory.h"
#અન્તર્ભાવ "localeprioritylist.h"
#અન્તર્ભાવ "uarrsort.h"
#અન્તર્ભાવ "uassert.h"
#અન્તર્ભાવ "uhash.h"

U_NAMESPACE_BEGIN

નામાકાશ {

int32_t hashLocale(અચલ UHashTok token) {
    અચલ auto* locale = static_cast<અચલ Locale*>(token.pointer);
    પાછા locale->hashCode();
}

UBool compareLocales(અચલ UHashTok t1, અચલ UHashTok t2) {
    અચલ auto* l1 = static_cast<અચલ Locale*>(t1.pointer);
    અચલ auto* l2 = static_cast<અચલ Locale*>(t2.pointer);
    પાછા *l1 == *l2;
}

constexpr int32_t WEIGHT_ONE = 1000;

સંરચના LocaleAndWeight {
    Locale *locale;
    int32_t weight;  // 0..1000 = 0.0..1.0
    int32_t index;  // force stable sort

    int32_t compare(અચલ LocaleAndWeight &other) અચલ {
        int32_t diff = other.weight - weight;  // descending: other-this
        જો (diff != 0) { પાછા diff; }
        પાછા index - other.index;
    }
};

int32_t U_CALLCONV
compareLocaleAndWeight(અચલ શૂન્ય * /*context*/, અચલ શૂન્ય *left, અચલ શૂન્ય *જમણું) {
    પાછા static_cast<અચલ LocaleAndWeight *>(left)->
        compare(*static_cast<અચલ LocaleAndWeight *>(જમણું));
}

અચલ અક્ષર *skipSpaces(અચલ અક્ષર *p, અચલ અક્ષર *limit) {
    જ્યારે (p < limit && *p == ' ') { ++p; }
    પાછા p;
}

int32_t findTagLength(અચલ અક્ષર *p, અચલ અક્ષર *limit) {
    // Look for accept-language delimiters.
    // Leave other validation up to the Locale constructor.
    અચલ અક્ષર *q;
    માટે (q = p; q < limit; ++q) {
        અક્ષર c = *q;
        જો (c == ' ' || c == ',' || c == ';') { તોડો; }
    }
    પાછા static_cast<int32_t>(q - p);
}

/**
 * Parses and returns a qvalue weight in millis.
 * Advances p to after the parsed substring.
 * Returns a negative value if parsing fails.
 */
int32_t parseWeight(અચલ અક્ષર *&p, અચલ અક્ષર *limit) {
    p = skipSpaces(p, limit);
    અક્ષર c;
    જો (p == limit || ((c = *p) != '0' && c != '1')) { પાછા -1; }
    int32_t weight = (c - '0') * 1000;
    જો (++p == limit || *p != '.') { પાછા weight; }
    int32_t multiplier = 100;
    જ્યારે (++p != limit && '0' <= (c = *p) && c <= '9') {
        c -= '0';
        જો (multiplier > 0) {
            weight += c * multiplier;
            multiplier /= 10;
        } નહીંતર જો (multiplier == 0) {
            // round up
            જો (c >= 5) { ++weight; }
            multiplier = -1;
        }  // else ignore further fraction digits
    }
    પાછા weight <= WEIGHT_ONE ? weight : -1;  // bad if > 1.0
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
સંરચના LocaleAndWeightArray : જાહેર UMemory {
    Maybe_ઢગલો_Array<LocaleAndWeight, 20> array;
};

LocalePriorityList::LocalePriorityList(StringPiece s, UErrorCode &errorCode) {
    જો (U_FAILURE(errorCode)) { પાછા; }
    list = નવું LocaleAndWeightArray();
    જો (list == nullptr) {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
        પાછા;
    }
    અચલ અક્ષર *p = s.data();
    અચલ અક્ષર *limit = p + s.length();
    જ્યારે ((p = skipSpaces(p, limit)) != limit) {
        જો (*p == ',') {  // empty range field
            ++p;
            ચાલુ;
        }
        int32_t tagLength = findTagLength(p, limit);
        જો (tagLength == 0) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            પાછા;
        }
        CharString tag(p, tagLength, errorCode);
        જો (U_FAILURE(errorCode)) { પાછા; }
        Locale locale = Locale(tag.data());
        જો (locale.isBogus()) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            પાછા;
        }
        int32_t weight = WEIGHT_ONE;
        જો ((p = skipSpaces(p + tagLength, limit)) != limit && *p == ';') {
            જો ((p = skipSpaces(p + 1, limit)) == limit || *p != 'q' ||
                    (p = skipSpaces(p + 1, limit)) == limit || *p != '=' ||
                    (++p, (weight = parseWeight(p, limit)) < 0)) {
                errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                પાછા;
            }
            p = skipSpaces(p, limit);
        }
        જો (p != limit && *p != ',') {  // trailing junk
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            પાછા;
        }
        add(locale, weight, errorCode);
        જો (p == limit) { તોડો; }
        ++p;
    }
    sort(errorCode);
}

LocalePriorityList::~LocalePriorityList() {
    જો (list != nullptr) {
        માટે (int32_t i = 0; i < listLength; ++i) {
            delete list->array[i].locale;
        }
        delete list;
    }
    uhash_close(map);
}

અચલ Locale *LocalePriorityList::localeAt(int32_t i) અચલ {
    પાછા list->array[i].locale;
}

Locale *LocalePriorityList::orphanLocaleAt(int32_t i) {
    જો (list == nullptr) { પાછા nullptr; }
    LocaleAndWeight &lw = list->array[i];
    Locale *l = lw.locale;
    lw.locale = nullptr;
    પાછા l;
}

તાર્કિક LocalePriorityList::add(અચલ Locale &locale, int32_t weight, UErrorCode &errorCode) {
    જો (U_FAILURE(errorCode)) { પાછા ખોટું; }
    જો (map == nullptr) {
        જો (weight <= 0) { પાછા સાચું; }  // do not add q=0
        map = uhash_open(hashLocale, compareLocales, uhash_compareLong, &errorCode);
        જો (U_FAILURE(errorCode)) { પાછા ખોટું; }
    }
    LocalPointer<Locale> clone;
    UBool found = ખોટું;
    int32_t index = uhash_getiAndFound(map, &locale, &found);
    જો (found) {
        // Duplicate: Remove the old item and append it anew.
        LocaleAndWeight &lw = list->array[index];
        clone.adoptInstead(lw.locale);
        lw.locale = nullptr;
        lw.weight = 0;
        ++અંક_Removed;
    }
    જો (weight <= 0) {  // do not add q=0
        જો (found) {
            // Not strictly necessary but cleaner.
            uhash_removei(map, &locale);
        }
        પાછા સાચું;
    }
    જો (clone.isNull()) {
        clone.adoptInstead(locale.clone());
        જો (clone.isNull() || (clone->isBogus() && !locale.isBogus())) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            પાછા ખોટું;
        }
    }
    જો (listLength == list->array.getCapacity()) {
        int32_t newCapacity = listLength < 50 ? 100 : 4 * listLength;
        જો (list->array.resize(newCapacity, listLength) == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            પાછા ખોટું;
        }
    }
    uhash_putiAllowZero(map, clone.getAlias(), listLength, &errorCode);
    જો (U_FAILURE(errorCode)) { પાછા ખોટું; }
    LocaleAndWeight &lw = list->array[listLength];
    lw.locale = clone.orphan();
    lw.weight = weight;
    lw.index = listLength++;
    જો (weight < WEIGHT_ONE) { hasWeights = સાચું; }
    U_ASSERT(uhash_ગણતરી(map) == getLength());
    પાછા સાચું;
}

શૂન્ય LocalePriorityList::sort(UErrorCode &errorCode) {
    // Sort by descending weights if there is a mix of weights.
    // The comparator forces a stable sort via the item index.
    જો (U_FAILURE(errorCode) || getLength() <= 1 || !hasWeights) { પાછા; }
    uprv_sortArray(list->array.getAlias(), listLength, કદ(LocaleAndWeight),
                   compareLocaleAndWeight, nullptr, ખોટું, &errorCode);
}

U_NAMESPACE_END
