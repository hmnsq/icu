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

int32_t hashLocale(ಸ್ಥಿರಾಂಕ UHashTok token) {
    ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ* locale = static_cast<ಸ್ಥಿರಾಂಕ Locale*>(token.ಸೂಚಕ);
    ಹಿಂತಿರುಗಿ locale->hash_ಸಂಕೇತ();
}

UBool compareLocales(ಸ್ಥಿರಾಂಕ UHashTok t1, ಸ್ಥಿರಾಂಕ UHashTok t2) {
    ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ* l1 = static_cast<ಸ್ಥಿರಾಂಕ Locale*>(t1.ಸೂಚಕ);
    ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ* l2 = static_cast<ಸ್ಥಿರಾಂಕ Locale*>(t2.ಸೂಚಕ);
    ಹಿಂತಿರುಗಿ *l1 == *l2;
}

constexpr int32_t ತೂಕ_ONE = 1000;

ರಚನೆ Locale_And_ತೂಕ {
    Locale *locale;
    int32_t ತೂಕ;  // 0..1000 = 0.0..1.0
    int32_t ಸೂಚ್ಯಂಕ;  // force stable sort

    int32_t compare(ಸ್ಥಿರಾಂಕ Locale_And_ತೂಕ &other) ಸ್ಥಿರಾಂಕ {
        int32_t diff = other.ತೂಕ - ತೂಕ;  // descending: other-this
        ಆದರೆ (diff != 0) { ಹಿಂತಿರುಗಿ diff; }
        ಹಿಂತಿರುಗಿ ಸೂಚ್ಯಂಕ - other.ಸೂಚ್ಯಂಕ;
    }
};

int32_t U_CALLCONV
compare_Locale_And_ತೂಕ(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ * /*context*/, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಎಡ, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಬಲ) {
    ಹಿಂತಿರುಗಿ static_cast<ಸ್ಥಿರಾಂಕ Locale_And_ತೂಕ *>(ಎಡ)->
        compare(*static_cast<ಸ್ಥಿರಾಂಕ Locale_And_ತೂಕ *>(ಬಲ));
}

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *skipSpaces(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಎಲ್ಲೆ) {
    ಆಗ (p < ಎಲ್ಲೆ && *p == ' ') { ++p; }
    ಹಿಂತಿರುಗಿ p;
}

int32_t ಕಂಡುಹಿಡಿ_ಟ್ಯಾಗ್_ಉದ್ದಳತೆ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಎಲ್ಲೆ) {
    // Look for accept-language delimiters.
    // Leave other validation up to the Locale constructor.
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *q;
    ಗಾಗಿ (q = p; q < ಎಲ್ಲೆ; ++q) {
        ಅಕ್ಷರ c = *q;
        ಆದರೆ (c == ' ' || c == ',' || c == ';') { ನಿಲ್ಲಿಸು; }
    }
    ಹಿಂತಿರುಗಿ static_cast<int32_t>(q - p);
}

/**
 * Parses and returns a qvalue weight in millis.
 * Advances p to after the parsed substring.
 * Returns a negative value if parsing fails.
 */
int32_t parse_ತೂಕ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *&p, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಎಲ್ಲೆ) {
    p = skipSpaces(p, ಎಲ್ಲೆ);
    ಅಕ್ಷರ c;
    ಆದರೆ (p == ಎಲ್ಲೆ || ((c = *p) != '0' && c != '1')) { ಹಿಂತಿರುಗಿ -1; }
    int32_t ತೂಕ = (c - '0') * 1000;
    ಆದರೆ (++p == ಎಲ್ಲೆ || *p != '.') { ಹಿಂತಿರುಗಿ ತೂಕ; }
    int32_t multiplier = 100;
    ಆಗ (++p != ಎಲ್ಲೆ && '0' <= (c = *p) && c <= '9') {
        c -= '0';
        ಆದರೆ (multiplier > 0) {
            ತೂಕ += c * multiplier;
            multiplier /= 10;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (multiplier == 0) {
            // round up
            ಆದರೆ (c >= 5) { ++ತೂಕ; }
            multiplier = -1;
        }  // else ignore further fraction digits
    }
    ಹಿಂತಿರುಗಿ ತೂಕ <= ತೂಕ_ONE ? ತೂಕ : -1;  // bad if > 1.0
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
ರಚನೆ Locale_And_ತೂಕ_Array : ಸಾರ್ವಜನಿಕ UMemory {
    Maybe_ರಾಶಿ_Array<Locale_And_ತೂಕ, 20> array;
};

Locale_ಆದ್ಯತೆ_List::Locale_ಆದ್ಯತೆ_List(StringPiece s, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ; }
    list = ಹೊಸ Locale_And_ತೂಕ_Array();
    ಆದರೆ (list == ಶೂನ್ಯಸೂಚಕ) {
        error_ಸಂಕೇತ = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = s.ದತ್ತಾಂಶ();
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಎಲ್ಲೆ = p + s.length();
    ಆಗ ((p = skipSpaces(p, ಎಲ್ಲೆ)) != ಎಲ್ಲೆ) {
        ಆದರೆ (*p == ',') {  // empty range field
            ++p;
            ಮುಂದುವರಿ;
        }
        int32_t ಟ್ಯಾಗ್_ಉದ್ದಳತೆ = ಕಂಡುಹಿಡಿ_ಟ್ಯಾಗ್_ಉದ್ದಳತೆ(p, ಎಲ್ಲೆ);
        ಆದರೆ (ಟ್ಯಾಗ್_ಉದ್ದಳತೆ == 0) {
            error_ಸಂಕೇತ = U_ILLEGAL_ವಾದ_ERROR;
            ಹಿಂತಿರುಗಿ;
        }
        CharString ಟ್ಯಾಗ್(p, ಟ್ಯಾಗ್_ಉದ್ದಳತೆ, error_ಸಂಕೇತ);
        ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ; }
        Locale locale = Locale(ಟ್ಯಾಗ್.ದತ್ತಾಂಶ());
        ಆದರೆ (locale.isBogus()) {
            error_ಸಂಕೇತ = U_ILLEGAL_ವಾದ_ERROR;
            ಹಿಂತಿರುಗಿ;
        }
        int32_t ತೂಕ = ತೂಕ_ONE;
        ಆದರೆ ((p = skipSpaces(p + ಟ್ಯಾಗ್_ಉದ್ದಳತೆ, ಎಲ್ಲೆ)) != ಎಲ್ಲೆ && *p == ';') {
            ಆದರೆ ((p = skipSpaces(p + 1, ಎಲ್ಲೆ)) == ಎಲ್ಲೆ || *p != 'q' ||
                    (p = skipSpaces(p + 1, ಎಲ್ಲೆ)) == ಎಲ್ಲೆ || *p != '=' ||
                    (++p, (ತೂಕ = parse_ತೂಕ(p, ಎಲ್ಲೆ)) < 0)) {
                error_ಸಂಕೇತ = U_ILLEGAL_ವಾದ_ERROR;
                ಹಿಂತಿರುಗಿ;
            }
            p = skipSpaces(p, ಎಲ್ಲೆ);
        }
        ಆದರೆ (p != ಎಲ್ಲೆ && *p != ',') {  // trailing junk
            error_ಸಂಕೇತ = U_ILLEGAL_ವಾದ_ERROR;
            ಹಿಂತಿರುಗಿ;
        }
        ಕೂಡಿಸು(locale, ತೂಕ, error_ಸಂಕೇತ);
        ಆದರೆ (p == ಎಲ್ಲೆ) { ನಿಲ್ಲಿಸು; }
        ++p;
    }
    sort(error_ಸಂಕೇತ);
}

Locale_ಆದ್ಯತೆ_List::~Locale_ಆದ್ಯತೆ_List() {
    ಆದರೆ (list != ಶೂನ್ಯಸೂಚಕ) {
        ಗಾಗಿ (int32_t i = 0; i < list_ಉದ್ದಳತೆ; ++i) {
            ಅಳಿಸು list->array[i].locale;
        }
        ಅಳಿಸು list;
    }
    uhash_close(map);
}

ಸ್ಥಿರಾಂಕ Locale *Locale_ಆದ್ಯತೆ_List::localeAt(int32_t i) ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ list->array[i].locale;
}

Locale *Locale_ಆದ್ಯತೆ_List::orphanLocaleAt(int32_t i) {
    ಆದರೆ (list == ಶೂನ್ಯಸೂಚಕ) { ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ; }
    Locale_And_ತೂಕ &lw = list->array[i];
    Locale *l = lw.locale;
    lw.locale = ಶೂನ್ಯಸೂಚಕ;
    ಹಿಂತಿರುಗಿ l;
}

ತಾರ್ಕಿಕ Locale_ಆದ್ಯತೆ_List::ಕೂಡಿಸು(ಸ್ಥಿರಾಂಕ Locale &locale, int32_t ತೂಕ, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ ಸುಳ್ಳು; }
    ಆದರೆ (map == ಶೂನ್ಯಸೂಚಕ) {
        ಆದರೆ (ತೂಕ <= 0) { ಹಿಂತಿರುಗಿ ನಿಜ; }  // do not add q=0
        map = uhash_open(hashLocale, compareLocales, uhash_compareLong, &error_ಸಂಕೇತ);
        ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ ಸುಳ್ಳು; }
    }
    Local_ಸೂಚಕ<Locale> clone;
    UBool ಸಿಕ್ಕಿತು = ಸುಳ್ಳು;
    int32_t ಸೂಚ್ಯಂಕ = uhash_geti_And_ಸಿಕ್ಕಿತು(map, &locale, &ಸಿಕ್ಕಿತು);
    ಆದರೆ (ಸಿಕ್ಕಿತು) {
        // Duplicate: Remove the old item and append it anew.
        Locale_And_ತೂಕ &lw = list->array[ಸೂಚ್ಯಂಕ];
        clone.adoptInstead(lw.locale);
        lw.locale = ಶೂನ್ಯಸೂಚಕ;
        lw.ತೂಕ = 0;
        ++ಅಂಕೆ_Removed;
    }
    ಆದರೆ (ತೂಕ <= 0) {  // do not add q=0
        ಆದರೆ (ಸಿಕ್ಕಿತು) {
            // Not strictly necessary but cleaner.
            uhash_removei(map, &locale);
        }
        ಹಿಂತಿರುಗಿ ನಿಜ;
    }
    ಆದರೆ (clone.isNull()) {
        clone.adoptInstead(locale.clone());
        ಆದರೆ (clone.isNull() || (clone->isBogus() && !locale.isBogus())) {
            error_ಸಂಕೇತ = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
        }
    }
    ಆದರೆ (list_ಉದ್ದಳತೆ == list->array.get_ಸಾಮರ್ಥ್ಯ()) {
        int32_t new_ಸಾಮರ್ಥ್ಯ = list_ಉದ್ದಳತೆ < 50 ? 100 : 4 * list_ಉದ್ದಳತೆ;
        ಆದರೆ (list->array.resize(new_ಸಾಮರ್ಥ್ಯ, list_ಉದ್ದಳತೆ) == ಶೂನ್ಯಸೂಚಕ) {
            error_ಸಂಕೇತ = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
        }
    }
    uhash_putiAllowZero(map, clone.getAlias(), list_ಉದ್ದಳತೆ, &error_ಸಂಕೇತ);
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ ಸುಳ್ಳು; }
    Locale_And_ತೂಕ &lw = list->array[list_ಉದ್ದಳತೆ];
    lw.locale = clone.orphan();
    lw.ತೂಕ = ತೂಕ;
    lw.ಸೂಚ್ಯಂಕ = list_ಉದ್ದಳತೆ++;
    ಆದರೆ (ತೂಕ < ತೂಕ_ONE) { hasWeights = ನಿಜ; }
    U_ASSERT(uhash_ಎಣಿಕೆ(map) == get_ಉದ್ದಳತೆ());
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

ಶೂನ್ಯ Locale_ಆದ್ಯತೆ_List::sort(UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    // Sort by descending weights if there is a mix of weights.
    // The comparator forces a stable sort via the item index.
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ) || get_ಉದ್ದಳತೆ() <= 1 || !hasWeights) { ಹಿಂತಿರುಗಿ; }
    uprv_ವಿಂಗಡಿಸು_Array(list->array.getAlias(), list_ಉದ್ದಳತೆ, ಗಾತ್ರ(Locale_And_ತೂಕ),
                   compare_Locale_And_ತೂಕ, ಶೂನ್ಯಸೂಚಕ, ಸುಳ್ಳು, &error_ಸಂಕೇತ);
}

U_NAMESPACE_ಕೊನೆ
