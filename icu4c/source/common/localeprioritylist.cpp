// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// localeprioritylist.cpp
// created: 2019jul11 Markus W. Scherer

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/localpointer.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/locid.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/stringpiece.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uobject.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "charstr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "localeprioritylist.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uarrsort.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uassert.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uhash.h"

U_NAMESPACE_BEGIN

ਨਾਮਾਕਾਸ਼ {

int32_t hashLocale(ਅਚਲ UHashTok token) {
    ਅਚਲ ਸਵੈਚਾਲਿਤ* locale = static_cast<ਅਚਲ Locale*>(token.ਸੂਚਕ);
    ਵਾਪਸ locale->hash_ਕੋਡ();
}

UBool compareLocales(ਅਚਲ UHashTok t1, ਅਚਲ UHashTok t2) {
    ਅਚਲ ਸਵੈਚਾਲਿਤ* l1 = static_cast<ਅਚਲ Locale*>(t1.ਸੂਚਕ);
    ਅਚਲ ਸਵੈਚਾਲਿਤ* l2 = static_cast<ਅਚਲ Locale*>(t2.ਸੂਚਕ);
    ਵਾਪਸ *l1 == *l2;
}

constexpr int32_t ਭਾਰ_ONE = 1000;

ਬਣਤਰ Locale_And_ਭਾਰ {
    Locale *locale;
    int32_t ਭਾਰ;  // 0..1000 = 0.0..1.0
    int32_t ਸੂਚਕਾਂਕ;  // force stable sort

    int32_t compare(ਅਚਲ Locale_And_ਭਾਰ &other) ਅਚਲ {
        int32_t diff = other.ਭਾਰ - ਭਾਰ;  // descending: other-this
        ਜੇ (diff != 0) { ਵਾਪਸ diff; }
        ਵਾਪਸ ਸੂਚਕਾਂਕ - other.ਸੂਚਕਾਂਕ;
    }
};

int32_t U_CALLCONV
compare_Locale_And_ਭਾਰ(ਅਚਲ ਖਾਲੀ * /*context*/, ਅਚਲ ਖਾਲੀ *ਖੱਬੇ, ਅਚਲ ਖਾਲੀ *ਸੱਜੇ) {
    ਵਾਪਸ static_cast<ਅਚਲ Locale_And_ਭਾਰ *>(ਖੱਬੇ)->
        compare(*static_cast<ਅਚਲ Locale_And_ਭਾਰ *>(ਸੱਜੇ));
}

ਅਚਲ ਅੱਖਰ *skipSpaces(ਅਚਲ ਅੱਖਰ *p, ਅਚਲ ਅੱਖਰ *ਹੱਦਬੰਦੀ) {
    ਜਦੋਂ (p < ਹੱਦਬੰਦੀ && *p == ' ') { ++p; }
    ਵਾਪਸ p;
}

int32_t ਲੱਭੋ_ਟੈਗ_ਲੰਮਾਈ(ਅਚਲ ਅੱਖਰ *p, ਅਚਲ ਅੱਖਰ *ਹੱਦਬੰਦੀ) {
    // Look for accept-language delimiters.
    // Leave other validation up to the Locale constructor.
    ਅਚਲ ਅੱਖਰ *q;
    ਲਈ (q = p; q < ਹੱਦਬੰਦੀ; ++q) {
        ਅੱਖਰ c = *q;
        ਜੇ (c == ' ' || c == ',' || c == ';') { ਰੋਕੋ; }
    }
    ਵਾਪਸ static_cast<int32_t>(q - p);
}

/**
 * Parses and returns a qvalue weight in millis.
 * Advances p to after the parsed substring.
 * Returns a negative value if parsing fails.
 */
int32_t parse_ਭਾਰ(ਅਚਲ ਅੱਖਰ *&p, ਅਚਲ ਅੱਖਰ *ਹੱਦਬੰਦੀ) {
    p = skipSpaces(p, ਹੱਦਬੰਦੀ);
    ਅੱਖਰ c;
    ਜੇ (p == ਹੱਦਬੰਦੀ || ((c = *p) != '0' && c != '1')) { ਵਾਪਸ -1; }
    int32_t ਭਾਰ = (c - '0') * 1000;
    ਜੇ (++p == ਹੱਦਬੰਦੀ || *p != '.') { ਵਾਪਸ ਭਾਰ; }
    int32_t multiplier = 100;
    ਜਦੋਂ (++p != ਹੱਦਬੰਦੀ && '0' <= (c = *p) && c <= '9') {
        c -= '0';
        ਜੇ (multiplier > 0) {
            ਭਾਰ += c * multiplier;
            multiplier /= 10;
        } ਨਹੀਂਤਾਂ ਜੇ (multiplier == 0) {
            // round up
            ਜੇ (c >= 5) { ++ਭਾਰ; }
            multiplier = -1;
        }  // else ignore further fraction digits
    }
    ਵਾਪਸ ਭਾਰ <= ਭਾਰ_ONE ? ਭਾਰ : -1;  // bad if > 1.0
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
ਬਣਤਰ Locale_And_ਭਾਰ_Array : ਜਨਤਕ UMemory {
    Maybe_ਢੇਰ_Array<Locale_And_ਭਾਰ, 20> array;
};

Locale_ਤਰਜੀਹ_List::Locale_ਤਰਜੀਹ_List(StringPiece s, UError_ਕੋਡ &error_ਕੋਡ) {
    ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ; }
    list = ਨਵਾਂ Locale_And_ਭਾਰ_Array();
    ਜੇ (list == ਖਾਲੀ_ਸੂਚਕ) {
        error_ਕੋਡ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ;
    }
    ਅਚਲ ਅੱਖਰ *p = s.ਅੰਕੜੇ();
    ਅਚਲ ਅੱਖਰ *ਹੱਦਬੰਦੀ = p + s.length();
    ਜਦੋਂ ((p = skipSpaces(p, ਹੱਦਬੰਦੀ)) != ਹੱਦਬੰਦੀ) {
        ਜੇ (*p == ',') {  // empty range field
            ++p;
            ਜਾਰੀ;
        }
        int32_t ਟੈਗ_ਲੰਮਾਈ = ਲੱਭੋ_ਟੈਗ_ਲੰਮਾਈ(p, ਹੱਦਬੰਦੀ);
        ਜੇ (ਟੈਗ_ਲੰਮਾਈ == 0) {
            error_ਕੋਡ = U_ILLEGAL_ਦਲੀਲ_ERROR;
            ਵਾਪਸ;
        }
        CharString ਟੈਗ(p, ਟੈਗ_ਲੰਮਾਈ, error_ਕੋਡ);
        ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ; }
        Locale locale = Locale(ਟੈਗ.ਅੰਕੜੇ());
        ਜੇ (locale.isBogus()) {
            error_ਕੋਡ = U_ILLEGAL_ਦਲੀਲ_ERROR;
            ਵਾਪਸ;
        }
        int32_t ਭਾਰ = ਭਾਰ_ONE;
        ਜੇ ((p = skipSpaces(p + ਟੈਗ_ਲੰਮਾਈ, ਹੱਦਬੰਦੀ)) != ਹੱਦਬੰਦੀ && *p == ';') {
            ਜੇ ((p = skipSpaces(p + 1, ਹੱਦਬੰਦੀ)) == ਹੱਦਬੰਦੀ || *p != 'q' ||
                    (p = skipSpaces(p + 1, ਹੱਦਬੰਦੀ)) == ਹੱਦਬੰਦੀ || *p != '=' ||
                    (++p, (ਭਾਰ = parse_ਭਾਰ(p, ਹੱਦਬੰਦੀ)) < 0)) {
                error_ਕੋਡ = U_ILLEGAL_ਦਲੀਲ_ERROR;
                ਵਾਪਸ;
            }
            p = skipSpaces(p, ਹੱਦਬੰਦੀ);
        }
        ਜੇ (p != ਹੱਦਬੰਦੀ && *p != ',') {  // trailing junk
            error_ਕੋਡ = U_ILLEGAL_ਦਲੀਲ_ERROR;
            ਵਾਪਸ;
        }
        ਜੋੜੋ(locale, ਭਾਰ, error_ਕੋਡ);
        ਜੇ (p == ਹੱਦਬੰਦੀ) { ਰੋਕੋ; }
        ++p;
    }
    sort(error_ਕੋਡ);
}

Locale_ਤਰਜੀਹ_List::~Locale_ਤਰਜੀਹ_List() {
    ਜੇ (list != ਖਾਲੀ_ਸੂਚਕ) {
        ਲਈ (int32_t i = 0; i < list_ਲੰਮਾਈ; ++i) {
            ਮਿਟਾਓ list->array[i].locale;
        }
        ਮਿਟਾਓ list;
    }
    uhash_close(map);
}

ਅਚਲ Locale *Locale_ਤਰਜੀਹ_List::localeAt(int32_t i) ਅਚਲ {
    ਵਾਪਸ list->array[i].locale;
}

Locale *Locale_ਤਰਜੀਹ_List::orphanLocaleAt(int32_t i) {
    ਜੇ (list == ਖਾਲੀ_ਸੂਚਕ) { ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ; }
    Locale_And_ਭਾਰ &lw = list->array[i];
    Locale *l = lw.locale;
    lw.locale = ਖਾਲੀ_ਸੂਚਕ;
    ਵਾਪਸ l;
}

ਤਾਰਕਿਕ Locale_ਤਰਜੀਹ_List::ਜੋੜੋ(ਅਚਲ Locale &locale, int32_t ਭਾਰ, UError_ਕੋਡ &error_ਕੋਡ) {
    ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ ਝੂਠ; }
    ਜੇ (map == ਖਾਲੀ_ਸੂਚਕ) {
        ਜੇ (ਭਾਰ <= 0) { ਵਾਪਸ ਸੱਚ; }  // do not add q=0
        map = uhash_open(hashLocale, compareLocales, uhash_compareLong, &error_ਕੋਡ);
        ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ ਝੂਠ; }
    }
    Local_ਸੂਚਕ<Locale> clone;
    UBool ਮਿਲਿਆ = ਝੂਠ;
    int32_t ਸੂਚਕਾਂਕ = uhash_geti_And_ਮਿਲਿਆ(map, &locale, &ਮਿਲਿਆ);
    ਜੇ (ਮਿਲਿਆ) {
        // Duplicate: Remove the old item and append it anew.
        Locale_And_ਭਾਰ &lw = list->array[ਸੂਚਕਾਂਕ];
        clone.adoptInstead(lw.locale);
        lw.locale = ਖਾਲੀ_ਸੂਚਕ;
        lw.ਭਾਰ = 0;
        ++ਅੰਕ_Removed;
    }
    ਜੇ (ਭਾਰ <= 0) {  // do not add q=0
        ਜੇ (ਮਿਲਿਆ) {
            // Not strictly necessary but cleaner.
            uhash_removei(map, &locale);
        }
        ਵਾਪਸ ਸੱਚ;
    }
    ਜੇ (clone.isNull()) {
        clone.adoptInstead(locale.clone());
        ਜੇ (clone.isNull() || (clone->isBogus() && !locale.isBogus())) {
            error_ਕੋਡ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
            ਵਾਪਸ ਝੂਠ;
        }
    }
    ਜੇ (list_ਲੰਮਾਈ == list->array.get_ਸਮਰੱਥਾ()) {
        int32_t new_ਸਮਰੱਥਾ = list_ਲੰਮਾਈ < 50 ? 100 : 4 * list_ਲੰਮਾਈ;
        ਜੇ (list->array.resize(new_ਸਮਰੱਥਾ, list_ਲੰਮਾਈ) == ਖਾਲੀ_ਸੂਚਕ) {
            error_ਕੋਡ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
            ਵਾਪਸ ਝੂਠ;
        }
    }
    uhash_putiAllowZero(map, clone.getAlias(), list_ਲੰਮਾਈ, &error_ਕੋਡ);
    ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ ਝੂਠ; }
    Locale_And_ਭਾਰ &lw = list->array[list_ਲੰਮਾਈ];
    lw.locale = clone.orphan();
    lw.ਭਾਰ = ਭਾਰ;
    lw.ਸੂਚਕਾਂਕ = list_ਲੰਮਾਈ++;
    ਜੇ (ਭਾਰ < ਭਾਰ_ONE) { hasWeights = ਸੱਚ; }
    U_ASSERT(uhash_ਗਿਣਤੀ(map) == get_ਲੰਮਾਈ());
    ਵਾਪਸ ਸੱਚ;
}

ਖਾਲੀ Locale_ਤਰਜੀਹ_List::sort(UError_ਕੋਡ &error_ਕੋਡ) {
    // Sort by descending weights if there is a mix of weights.
    // The comparator forces a stable sort via the item index.
    ਜੇ (U_FAILURE(error_ਕੋਡ) || get_ਲੰਮਾਈ() <= 1 || !hasWeights) { ਵਾਪਸ; }
    uprv_ਛਾਂਟੋ_Array(list->array.getAlias(), list_ਲੰਮਾਈ, ਆਕਾਰ(Locale_And_ਭਾਰ),
                   compare_Locale_And_ਭਾਰ, ਖਾਲੀ_ਸੂਚਕ, ਝੂਠ, &error_ਕੋਡ);
}

U_NAMESPACE_ਸਿਰਾ
