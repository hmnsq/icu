// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
* Copyright (c) 2002-2016, International Business Machines
* Corporation and others.  All Rights Reserved.
**********************************************************************
*/

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಆದರೆ !UCONFIG_NO_FORMATTING

#ಅನ್ತರ್ಭಾವ <utility>

#ಅನ್ತರ್ಭಾವ "unicode/ucurr.h"
#ಅನ್ತರ್ಭಾವ "unicode/locid.h"
#ಅನ್ತರ್ಭಾವ "unicode/ures.h"
#ಅನ್ತರ್ಭಾವ "unicode/ustring.h"
#ಅನ್ತರ್ಭಾವ "unicode/parsepos.h"
#ಅನ್ತರ್ಭಾವ "unicode/uniset.h"
#ಅನ್ತರ್ಭಾವ "unicode/usetiter.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf16.h"
#ಅನ್ತರ್ಭಾವ "ustr_imp.h"
#ಅನ್ತರ್ಭಾವ "charstr.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"
#ಅನ್ತರ್ಭಾವ "static_unicode_sets.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"
#ಅನ್ತರ್ಭಾವ "umutex.h"
#ಅನ್ತರ್ಭಾವ "ucln_cmn.h"
#ಅನ್ತರ್ಭಾವ "uenumimp.h"
#ಅನ್ತರ್ಭಾವ "uhash.h"
#ಅನ್ತರ್ಭಾವ "hash.h"
#ಅನ್ತರ್ಭಾವ "uinvchar.h"
#ಅನ್ತರ್ಭಾವ "uresimp.h"
#ಅನ್ತರ್ಭಾವ "ulist.h"
#ಅನ್ತರ್ಭಾವ "uresimp.h"
#ಅನ್ತರ್ಭಾವ "ureslocs.h"
#ಅನ್ತರ್ಭಾವ "ulocimp.h"

ಬಳಸಿ ನಾಮಾಕಾಶ icu;

//#define UCURR_DEBUG_EQUIV 1
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UCURR_DEBUG_EQUIV
#ಅನ್ತರ್ಭಾವ "stdio.h"
#ಆದರೆ_ಅಂತ್ಯ
//#define UCURR_DEBUG 1
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UCURR_DEBUG
#ಅನ್ತರ್ಭಾವ "stdio.h"
#ಆದರೆ_ಅಂತ್ಯ

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ Iso_ಸಂಕೇತ_ನಮೂದು {
    ಸ್ಥಿರಾಂಕ char16_t *iso_ಸಂಕೇತ; /* const because it's a reference to a resource bundle string. */
    UDate from;
    UDate ಗಮ್ಯ;
} Iso_ಸಂಕೇತ_ನಮೂದು;

//------------------------------------------------------------
// Constants

// Default currency meta data of last resort.  We try to use the
// defaults encoded in the meta data resource bundle.  If there is a
// configuration/build error and these are not available, we use these
// hard-coded defaults (which should be identical).
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ int32_t ಕೊನೆಯ_RESORT_ದತ್ತಾಂಶ[] = { 2, 0, 2, 0 };

// POW10[i] = 10^i, i=0..MAX_POW10
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ int32_t POW10[] = { 1, 10, 100, 1000, 10000, 100000,
                                 1000000, 10000000, 100000000, 1000000000 };

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ int32_t MAX_POW10 = UPRV_LENGTHOF(POW10) - 1;

#ವ್ಯಾಖ್ಯಾನಿಸು ISO_CURRENCY_ಸಂಕೇತ_ಉದ್ದಳತೆ 3

//------------------------------------------------------------
// Resource tags
//

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ CURRENCY_ದತ್ತಾಂಶ[] = "supplementalData";
// Tag for meta-data, in root.
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ CURRENCY_META[] = "CurrencyMeta";

// Tag for map from countries to currencies, in root.
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ CURRENCY_MAP[] = "CurrencyMap";

// Tag for default meta-data, in CURRENCY_META
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ DEFAULT_META[] = "DEFAULT";

// Variant delimiter
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ VAR_DELIM = '_';

// Tag for localized display names (symbols) of currencies
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ CURRENCIES[] = "Currencies";
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ CURRENCIES_NARROW[] = "Currencies%narrow";
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ CURRENCIES_FORMAL[] = "Currencies%formal";
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ CURRENCIES_VARIANT[] = "Currencies%variant";
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ CURRENCYPLURALS[] = "CurrencyPlurals";

// ISO codes mapping table
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ UHashtable* gIsoCodes = ಶೂನ್ಯಸೂಚಕ;
ಸ್ಥಿರ icu::UInitOnce gIsoCodesInitOnce {};

// Currency symbol equivalances
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ icu::Hashtable* gCurrSymbolsEquiv = ಶೂನ್ಯಸೂಚಕ;
ಸ್ಥಿರ icu::UInitOnce gCurrSymbolsEquivInitOnce {};

U_NAMESPACE_BEGIN

// EquivIterator iterates over all strings that are equivalent to a given
// string, s. Note that EquivIterator will never yield s itself.
ವರ್ಗ EquivIterator : ಸಾರ್ವಜನಿಕ icu::UMemory {
ಸಾರ್ವಜನಿಕ:
    // Constructor. hash stores the equivalence relationships; s is the string
    // for which we find equivalent strings.
    ಸಾಲೊಳಗೆ EquivIterator(ಸ್ಥಿರಾಂಕ icu::Hashtable& hash, ಸ್ಥಿರಾಂಕ icu::UnicodeString& s)
        : _hash(hash) { 
        ಆರಂಭ_2 = _current = &s;
    }
    ಸಾಲೊಳಗೆ ~EquivIterator() { }

    // next returns the next equivalent string or nullptr if there are no more.
    // If s has no equivalent strings, next returns nullptr on the first call.
    ಸ್ಥಿರಾಂಕ icu::UnicodeString *next();
ಖಾಸಗಿ:
    ಸ್ಥಿರಾಂಕ icu::Hashtable& _hash;
    ಸ್ಥಿರಾಂಕ icu::UnicodeString* ಆರಂಭ_2;
    ಸ್ಥಿರಾಂಕ icu::UnicodeString* _current;
};

ಸ್ಥಿರಾಂಕ icu::UnicodeString *
EquivIterator::next() {
    ಸ್ಥಿರಾಂಕ icu::UnicodeString* ಮುಂದಿನ_2 = static_cast<ಸ್ಥಿರಾಂಕ icu::UnicodeString*>(_hash.get(*_current));
    ಆದರೆ (ಮುಂದಿನ_2 == ಶೂನ್ಯಸೂಚಕ) {
        U_ASSERT(_current == ಆರಂಭ_2);
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಆದರೆ (*ಮುಂದಿನ_2 == *ಆರಂಭ_2) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    _current = ಮುಂದಿನ_2;
    ಹಿಂತಿರುಗಿ ಮುಂದಿನ_2;
}

U_NAMESPACE_ಕೊನೆ

// makeEquivalent makes lhs and rhs equivalent by updating the equivalence
// relations in hash accordingly.
ಸ್ಥಿರ ಶೂನ್ಯ makeEquivalent(
    ಸ್ಥಿರಾಂಕ icu::UnicodeString &lhs,
    ಸ್ಥಿರಾಂಕ icu::UnicodeString &rhs,
    icu::Hashtable* hash, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) {
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (lhs == rhs) {
        // already equivalent
        ಹಿಂತಿರುಗಿ;
    }
    icu::EquivIterator ಎಡ_Iter(*hash, lhs);
    icu::EquivIterator ಬಲ_Iter(*hash, rhs);
    ಸ್ಥಿರಾಂಕ icu::UnicodeString *ಮೊದಲ_ಎಡ = ಎಡ_Iter.next();
    ಸ್ಥಿರಾಂಕ icu::UnicodeString *ಮೊದಲ_ಬಲ = ಬಲ_Iter.next();
    ಸ್ಥಿರಾಂಕ icu::UnicodeString *ಮುಂದಿನ_ಎಡ = ಮೊದಲ_ಎಡ;
    ಸ್ಥಿರಾಂಕ icu::UnicodeString *ಮುಂದಿನ_ಬಲ = ಮೊದಲ_ಬಲ;
    ಆಗ (ಮುಂದಿನ_ಎಡ != ಶೂನ್ಯಸೂಚಕ && ಮುಂದಿನ_ಬಲ != ಶೂನ್ಯಸೂಚಕ) {
        ಆದರೆ (*ಮುಂದಿನ_ಎಡ == rhs || *ಮುಂದಿನ_ಬಲ == lhs) {
            // Already equivalent
            ಹಿಂತಿರುಗಿ;
        }
        ಮುಂದಿನ_ಎಡ = ಎಡ_Iter.next();
        ಮುಂದಿನ_ಬಲ = ಬಲ_Iter.next();
    }
    // Not equivalent. Must join.
    icu::UnicodeString *new_ಮೊದಲ_ಎಡ;
    icu::UnicodeString *new_ಮೊದಲ_ಬಲ;
    ಆದರೆ (ಮೊದಲ_ಬಲ == ಶೂನ್ಯಸೂಚಕ && ಮೊದಲ_ಎಡ == ಶೂನ್ಯಸೂಚಕ) {
        // Neither lhs or rhs belong to an equivalence circle, so we form
        // a new equivalnce circle of just lhs and rhs.
        new_ಮೊದಲ_ಎಡ = ಹೊಸ icu::UnicodeString(rhs);
        new_ಮೊದಲ_ಬಲ = ಹೊಸ icu::UnicodeString(lhs);
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಮೊದಲ_ಬಲ == ಶೂನ್ಯಸೂಚಕ) {
        // lhs belongs to an equivalence circle, but rhs does not, so we link
        // rhs into lhs' circle.
        new_ಮೊದಲ_ಎಡ = ಹೊಸ icu::UnicodeString(rhs);
        new_ಮೊದಲ_ಬಲ = ಹೊಸ icu::UnicodeString(*ಮೊದಲ_ಎಡ);
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಮೊದಲ_ಎಡ == ಶೂನ್ಯಸೂಚಕ) {
        // rhs belongs to an equivlance circle, but lhs does not, so we link
        // lhs into rhs' circle.
        new_ಮೊದಲ_ಎಡ = ಹೊಸ icu::UnicodeString(*ಮೊದಲ_ಬಲ);
        new_ಮೊದಲ_ಬಲ = ಹೊಸ icu::UnicodeString(lhs);
    } ಇಲ್ಲದಿದ್ದರೆ {
        // Both lhs and rhs belong to different equivalnce circles. We link
        // them together to form one single, larger equivalnce circle.
        new_ಮೊದಲ_ಎಡ = ಹೊಸ icu::UnicodeString(*ಮೊದಲ_ಬಲ);
        new_ಮೊದಲ_ಬಲ = ಹೊಸ icu::UnicodeString(*ಮೊದಲ_ಎಡ);
    }
    ಆದರೆ (new_ಮೊದಲ_ಎಡ == ಶೂನ್ಯಸೂಚಕ || new_ಮೊದಲ_ಬಲ == ಶೂನ್ಯಸೂಚಕ) {
        ಅಳಿಸು new_ಮೊದಲ_ಎಡ;
        ಅಳಿಸು new_ಮೊದಲ_ಬಲ;
        ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }
    hash->put(lhs, (ಶೂನ್ಯ *) new_ಮೊದಲ_ಎಡ, ಸ್ಥಿತಿ_2);
    hash->put(rhs, (ಶೂನ್ಯ *) new_ಮೊದಲ_ಬಲ, ಸ್ಥಿತಿ_2);
}

// countEquivalent counts how many strings are equivalent to s.
// hash stores all the equivalnce relations.
// countEquivalent does not include s itself in the count.
ಸ್ಥಿರ int32_t ಎಣಿಕೆ_Equivalent(ಸ್ಥಿರಾಂಕ icu::Hashtable &hash, ಸ್ಥಿರಾಂಕ icu::UnicodeString &s) {
    int32_t ಫಲಿತಾಂಶ = 0;
    icu::EquivIterator iter(hash, s);
    ಆಗ (iter.next() != ಶೂನ್ಯಸೂಚಕ) {
        ++ಫಲಿತಾಂಶ;
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UCURR_DEBUG_EQUIV
 {
   ಅಕ್ಷರ tmp[200];
   s.extract(0,s.length(),tmp, "UTF-8");
   ಬರೆ("CountEquivalent('%s') = %d\n", tmp, ಫಲಿತಾಂಶ);
 }
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ icu::Hashtable* getCurrSymbolsEquiv();

//------------------------------------------------------------
//ಸಂಕೇತ

/**
 * Cleanup callback func
 */
ಸ್ಥಿರ UBool U_CALLCONV 
isoCodes_cleanup()
{
    ಆದರೆ (gIsoCodes != ಶೂನ್ಯಸೂಚಕ) {
        uhash_close(const_cast<UHashtable *>(gIsoCodes));
        gIsoCodes = ಶೂನ್ಯಸೂಚಕ;
    }
    gIsoCodesInitOnce.reset();
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

/**
 * Cleanup callback func
 */
ಸ್ಥಿರ UBool U_CALLCONV 
currSymbolsEquiv_cleanup()
{
    ಅಳಿಸು const_cast<icu::Hashtable *>(gCurrSymbolsEquiv);
    gCurrSymbolsEquiv = ಶೂನ್ಯಸೂಚಕ;
    gCurrSymbolsEquivInitOnce.reset();
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

/**
 * Deleter for IsoCodeEntry
 */
ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
delete_Iso_ಸಂಕೇತ_ನಮೂದು(ಶೂನ್ಯ *obj) {
    Iso_ಸಂಕೇತ_ನಮೂದು* ನಮೂದು = static_cast<Iso_ಸಂಕೇತ_ನಮೂದು*>(obj);
    uprv_free(ನಮೂದು);
}

/**
 * Deleter for gCurrSymbolsEquiv.
 */
ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
deleteUnicode(ಶೂನ್ಯ *obj) {
    icu::UnicodeString* ನಮೂದು = static_cast<icu::UnicodeString*>(obj);
    ಅಳಿಸು ನಮೂದು;
}

/**
 * Unfortunately, we have to convert the char16_t* currency code to char*
 * to use it as a resource key.
 */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಅಕ್ಷರ*
my_UChars_ಗಮ್ಯ_Chars(ಅಕ್ಷರ* ಫಲಿತಾಂಶ_ದ_Len4, ಸ್ಥಿರಾಂಕ char16_t* currency) {
    u_UChars_ಗಮ್ಯ_Chars(currency, ಫಲಿತಾಂಶ_ದ_Len4, ISO_CURRENCY_ಸಂಕೇತ_ಉದ್ದಳತೆ);
    ಫಲಿತಾಂಶ_ದ_Len4[ISO_CURRENCY_ಸಂಕೇತ_ಉದ್ದಳತೆ] = 0;
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ_ದ_Len4;
}

/**
 * Internal function to look up currency data.  Result is an array of
 * four integers.  The first is the fraction digits.  The second is the
 * rounding increment, or 0 if none.  The rounding increment is in
 * units of 10^(-fraction_digits).  The third and fourth are the same
 * except that they are those used in cash transactions ( cashDigits
 * and cashRounding ).
 */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ int32_t*
ಕಂಡುಹಿಡಿ_Meta_ದತ್ತಾಂಶ(ಸ್ಥಿರಾಂಕ char16_t* currency, UError_ಸಂಕೇತ& ec) {

    ಆದರೆ (currency == ಶೂನ್ಯಸೂಚಕ || *currency == 0) {
        ಆದರೆ (U_SUCCESS(ec)) {
            ec = U_ILLEGAL_ವಾದ_ERROR;
        }
        ಹಿಂತಿರುಗಿ ಕೊನೆಯ_RESORT_ದತ್ತಾಂಶ;
    }

    // Get CurrencyMeta resource out of root locale file.  [This may
    // move out of the root locale file later; if it does, update this
    // code.]
    UResourceBundle* currency_ದತ್ತಾಂಶ = ures_open_ನೇರ(U_ICUDATA_CURR, CURRENCY_ದತ್ತಾಂಶ, &ec);
    Local_UResource_Bundle_ಸೂಚಕ currencyMeta(ures_get_By_ಕೀಲಿ(currency_ದತ್ತಾಂಶ, CURRENCY_META, currency_ದತ್ತಾಂಶ, &ec));

    ಆದರೆ (U_FAILURE(ec)) {
        // Config/build error; return hard-coded defaults
        ಹಿಂತಿರುಗಿ ಕೊನೆಯ_RESORT_ದತ್ತಾಂಶ;
    }

    // Look up our currency, or if that's not available, then DEFAULT
    ಅಕ್ಷರ buf[ISO_CURRENCY_ಸಂಕೇತ_ಉದ್ದಳತೆ+1];
    UError_ಸಂಕೇತ ec2 = U_ZERO_ERROR; // local error code: soft failure
    Local_UResource_Bundle_ಸೂಚಕ rb(ures_get_By_ಕೀಲಿ(currencyMeta.getAlias(), my_UChars_ಗಮ್ಯ_Chars(buf, currency), ಶೂನ್ಯಸೂಚಕ, &ec2));
      ಆದರೆ (U_FAILURE(ec2)) {
        rb.adoptInstead(ures_get_By_ಕೀಲಿ(currencyMeta.getAlias(),DEFAULT_META, ಶೂನ್ಯಸೂಚಕ, &ec));
        ಆದರೆ (U_FAILURE(ec)) {
            // Config/build error; return hard-coded defaults
            ಹಿಂತಿರುಗಿ ಕೊನೆಯ_RESORT_ದತ್ತಾಂಶ;
        }
    }

    int32_t len;
    ಸ್ಥಿರಾಂಕ int32_t *ದತ್ತಾಂಶ = ures_get_Int_ಸದಿಶ(rb.getAlias(), &len, &ec);
    ಆದರೆ (U_FAILURE(ec) || len != 4) {
        // Config/build error; return hard-coded defaults
        ಆದರೆ (U_SUCCESS(ec)) {
            ec = U_INVALID_FORMAT_ERROR;
        }
        ಹಿಂತಿರುಗಿ ಕೊನೆಯ_RESORT_ದತ್ತಾಂಶ;
    }

    ಹಿಂತಿರುಗಿ ದತ್ತಾಂಶ;
}

// -------------------------------------

ಸ್ಥಿರ CharString
idForLocale(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* locale, UError_ಸಂಕೇತ* ec)
{
    ಹಿಂತಿರುಗಿ ulocimp_get_Region_For_Supplemental_ದತ್ತಾಂಶ(locale, ಸುಳ್ಳು, *ec);
}

// ------------------------------------------
//
// Registration
//
//-------------------------------------------

// don't use ICUService since we don't need fallback

U_CDECL_BEGIN
ಸ್ಥಿರ UBool U_CALLCONV currency_cleanup();
U_CDECL_ಕೊನೆ

#ಆದರೆ !UCONFIG_NO_SERVICE
ರಚನೆ CReg;

ಸ್ಥಿರ UMutex g_CReg_ಬೀಗ;
ಸ್ಥಿರ CReg* g_CReg_ತಲೆ = ಶೂನ್ಯಸೂಚಕ;

ರಚನೆ CReg : ಸಾರ್ವಜನಿಕ icu::UMemory {
    CReg *next;
    char16_t iso[ISO_CURRENCY_ಸಂಕೇತ_ಉದ್ದಳತೆ+1];
    ಅಕ್ಷರ  id[ULOC_FULLNAME_ಸಾಮರ್ಥ್ಯ];

    CReg(ಸ್ಥಿರಾಂಕ char16_t* _iso, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* _id)
        : next(ಶೂನ್ಯಸೂಚಕ)
    {
        uprv_strncpy(id, _id, ಗಾತ್ರ(id)-1);
        id[ಗಾತ್ರ(id)-1] = 0;
        u_memcpy(iso, _iso, ISO_CURRENCY_ಸಂಕೇತ_ಉದ್ದಳತೆ);
        iso[ISO_CURRENCY_ಸಂಕೇತ_ಉದ್ದಳತೆ] = 0;
    }

    ಸ್ಥಿರ UCurr_Registry_ಕೀಲಿ reg(ಸ್ಥಿರಾಂಕ char16_t* _iso, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* _id, UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2)
    {
        ಆದರೆ (ಸ್ಥಿತಿ_2 && U_SUCCESS(*ಸ್ಥಿತಿ_2) && _iso && _id) {
            CReg* n = ಹೊಸ CReg(_iso, _id);
            ಆದರೆ (n) {
                umtx_ಬೀಗ(&g_CReg_ಬೀಗ);
                ಆದರೆ (!g_CReg_ತಲೆ) {
                    /* register for the first time */
                    ucln_common_ರಿಜಿಸ್ಟರ್_Cleanup(UCLN_COMMON_CURRENCY, currency_cleanup);
                }
                n->next = g_CReg_ತಲೆ;
                g_CReg_ತಲೆ = n;
                umtx_unlock(&g_CReg_ಬೀಗ);
                ಹಿಂತಿರುಗಿ n;
            }
            *ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        }
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    ಸ್ಥಿರ UBool unreg(UCurr_Registry_ಕೀಲಿ ಕೀಲಿ) {
        UBool ಸಿಕ್ಕಿತು = ಸುಳ್ಳು;
        umtx_ಬೀಗ(&g_CReg_ಬೀಗ);

        CReg** p = &g_CReg_ತಲೆ;
        ಆಗ (*p) {
            ಆದರೆ (*p == ಕೀಲಿ) {
                *p = ((CReg*)ಕೀಲಿ)->next;
                ಅಳಿಸು (CReg*)ಕೀಲಿ;
                ಸಿಕ್ಕಿತು = ನಿಜ;
                ನಿಲ್ಲಿಸು;
            }
            p = &((*p)->next);
        }

        umtx_unlock(&g_CReg_ಬೀಗ);
        ಹಿಂತಿರುಗಿ ಸಿಕ್ಕಿತು;
    }

    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ char16_t* get(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* id) {
        ಸ್ಥಿರಾಂಕ char16_t* ಫಲಿತಾಂಶ = ಶೂನ್ಯಸೂಚಕ;
        umtx_ಬೀಗ(&g_CReg_ಬೀಗ);
        CReg* p = g_CReg_ತಲೆ;

        /* register cleanup of the mutex */
        ucln_common_ರಿಜಿಸ್ಟರ್_Cleanup(UCLN_COMMON_CURRENCY, currency_cleanup);
        ಆಗ (p) {
            ಆದರೆ (uprv_strcmp(id, p->id) == 0) {
                ಫಲಿತಾಂಶ = p->iso;
                ನಿಲ್ಲಿಸು;
            }
            p = p->next;
        }
        umtx_unlock(&g_CReg_ಬೀಗ);
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }

    /* This doesn't need to be thread safe. It's for u_cleanup only. */
    ಸ್ಥಿರ ಶೂನ್ಯ cleanup() {
        ಆಗ (g_CReg_ತಲೆ) {
            CReg* n = g_CReg_ತಲೆ;
            g_CReg_ತಲೆ = g_CReg_ತಲೆ->next;
            ಅಳಿಸು n;
        }
    }
};

// -------------------------------------

U_CAPI UCurr_Registry_ಕೀಲಿ U_EXPORT2
ucurr_ರಿಜಿಸ್ಟರ್(ಸ್ಥಿರಾಂಕ char16_t* iso_ಸಂಕೇತ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* locale, UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2)
{
    ಆದರೆ (ಸ್ಥಿತಿ_2 && U_SUCCESS(*ಸ್ಥಿತಿ_2)) {
        CharString id = idForLocale(locale, ಸ್ಥಿತಿ_2);
        ಹಿಂತಿರುಗಿ CReg::reg(iso_ಸಂಕೇತ, id.ದತ್ತಾಂಶ(), ಸ್ಥಿತಿ_2);
    }
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
}

// -------------------------------------

U_CAPI UBool U_EXPORT2
ucurr_unregister(UCurr_Registry_ಕೀಲಿ ಕೀಲಿ, UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2)
{
    ಆದರೆ (ಸ್ಥಿತಿ_2 && U_SUCCESS(*ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ CReg::unreg(ಕೀಲಿ);
    }
    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}
#ಆದರೆ_ಅಂತ್ಯ /* UCONFIG_NO_SERVICE */

// -------------------------------------

/**
 * Release all static memory held by currency.
 */
/*The declaration here is needed so currency_cleanup()
 * can call this function.
 */
ಸ್ಥಿರ UBool U_CALLCONV
currency_cache_cleanup();

U_CDECL_BEGIN
ಸ್ಥಿರ UBool U_CALLCONV currency_cleanup() {
#ಆದರೆ !UCONFIG_NO_SERVICE
    CReg::cleanup();
#ಆದರೆ_ಅಂತ್ಯ
    /*
     * There might be some cached currency data or isoCodes data.
     */
    currency_cache_cleanup();
    isoCodes_cleanup();
    currSymbolsEquiv_cleanup();

    ಹಿಂತಿರುಗಿ ನಿಜ;
}
U_CDECL_ಕೊನೆ

// -------------------------------------

U_CAPI int32_t U_EXPORT2
ucurr_forLocale(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* locale,
                char16_t* buff,
                int32_t buff_ಸಾಮರ್ಥ್ಯ,
                UError_ಸಂಕೇತ* ec) {
    ಆದರೆ (U_FAILURE(*ec)) { ಹಿಂತಿರುಗಿ 0; }
    ಆದರೆ (buff_ಸಾಮರ್ಥ್ಯ < 0 || (buff == ಶೂನ್ಯಸೂಚಕ && buff_ಸಾಮರ್ಥ್ಯ > 0)) {
        *ec = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }

    UError_ಸಂಕೇತ local_ಸ್ಥಿತಿ = U_ZERO_ERROR;
    CharString currency = ulocimp_get_Keyword_ಮೌಲ್ಯ(locale, "currency", local_ಸ್ಥಿತಿ);
    int32_t resLen = currency.length();

    ಆದರೆ (U_SUCCESS(local_ಸ್ಥಿತಿ) && resLen == 3 && uprv_isInvariantString(currency.ದತ್ತಾಂಶ(), resLen)) {
        ಆದರೆ (resLen < buff_ಸಾಮರ್ಥ್ಯ) {
            T_CString_ಗಮ್ಯ_ಊರ್ಧ್ವ_Case(currency.ದತ್ತಾಂಶ());
            u_chars_ಗಮ್ಯ_UChars(currency.ದತ್ತಾಂಶ(), buff, resLen);
        }
        ಹಿಂತಿರುಗಿ u_terminateUChars(buff, buff_ಸಾಮರ್ಥ್ಯ, resLen, ec);
    }

    // get country or country_variant in `id'
    CharString id = idForLocale(locale, ec);
    ಆದರೆ (U_FAILURE(*ec)) {
        ಹಿಂತಿರುಗಿ 0;
    }

#ಆದರೆ !UCONFIG_NO_SERVICE
    ಸ್ಥಿರಾಂಕ char16_t* ಫಲಿತಾಂಶ = CReg::get(id.ದತ್ತಾಂಶ());
    ಆದರೆ (ಫಲಿತಾಂಶ) {
        ಆದರೆ(buff_ಸಾಮರ್ಥ್ಯ > u_strlen(ಫಲಿತಾಂಶ)) {
            u_strcpy(buff, ಫಲಿತಾಂಶ);
        }
        resLen = u_strlen(ಫಲಿತಾಂಶ);
        ಹಿಂತಿರುಗಿ u_terminateUChars(buff, buff_ಸಾಮರ್ಥ್ಯ, resLen, ec);
    }
#ಆದರೆ_ಅಂತ್ಯ
    // Remove variants, which is only needed for registration.
    ಅಕ್ಷರ *idDelim = uprv_strchr(id.ದತ್ತಾಂಶ(), VAR_DELIM);
    ಆದರೆ (idDelim) {
        id.truncate(idDelim - id.ದತ್ತಾಂಶ());
    }

    ಸ್ಥಿರಾಂಕ char16_t* s = ಶೂನ್ಯಸೂಚಕ;  // Currency code from data file.
    ಆದರೆ (id.is_ಖಾಲಿಗಳು()) {
        // No point looking in the data for an empty string.
        // This is what we would get.
        local_ಸ್ಥಿತಿ = U_MISSING_RESOURCE_ERROR;
    } ಇಲ್ಲದಿದ್ದರೆ {
        // Look up the CurrencyMap element in the root bundle.
        local_ಸ್ಥಿತಿ = U_ZERO_ERROR;
        UResourceBundle *rb = ures_open_ನೇರ(U_ICUDATA_CURR, CURRENCY_ದತ್ತಾಂಶ, &local_ಸ್ಥಿತಿ);
        UResourceBundle *cm = ures_get_By_ಕೀಲಿ(rb, CURRENCY_MAP, rb, &local_ಸ್ಥಿತಿ);
        Local_UResource_Bundle_ಸೂಚಕ countryArray(ures_get_By_ಕೀಲಿ(rb, id.ದತ್ತಾಂಶ(), cm, &local_ಸ್ಥಿತಿ));
        // https://unicode-org.atlassian.net/browse/ICU-21997
        // Prefer to use currencies that are legal tender.
        ಆದರೆ (U_SUCCESS(local_ಸ್ಥಿತಿ)) {
            int32_t array_ಉದ್ದಳತೆ = ures_get_ಗಾತ್ರ(countryArray.getAlias());
            ಗಾಗಿ (int32_t i = 0; i < array_ಉದ್ದಳತೆ; ++i) {
                Local_UResource_Bundle_ಸೂಚಕ currencyReq(
                    ures_get_By_ಸೂಚ್ಯಂಕ(countryArray.getAlias(), i, ಶೂನ್ಯಸೂಚಕ, &local_ಸ್ಥಿತಿ));
                // The currency is legal tender if it is *not* marked with tender{"false"}.
                UError_ಸಂಕೇತ tender_ಸ್ಥಿತಿ = local_ಸ್ಥಿತಿ;
                ಸ್ಥಿರಾಂಕ char16_t *tender =
                    ures_get_String_By_ಕೀಲಿ(currencyReq.getAlias(), "tender", ಶೂನ್ಯಸೂಚಕ, &tender_ಸ್ಥಿತಿ);
                ತಾರ್ಕಿಕ isTender = U_FAILURE(tender_ಸ್ಥಿತಿ) || u_strcmp(tender, u"false") != 0;
                ಆದರೆ (!isTender && s != ಶೂನ್ಯಸೂಚಕ) {
                    // We already have a non-tender currency. Ignore all following non-tender ones.
                    ಮುಂದುವರಿ;
                }
                // Fetch the currency code.
                s = ures_get_String_By_ಕೀಲಿ(currencyReq.getAlias(), "id", &resLen, &local_ಸ್ಥಿತಿ);
                ಆದರೆ (isTender) {
                    ನಿಲ್ಲಿಸು;
                }
            }
            ಆದರೆ (U_SUCCESS(local_ಸ್ಥಿತಿ) && s == ಶೂನ್ಯಸೂಚಕ) {
                local_ಸ್ಥಿತಿ = U_MISSING_RESOURCE_ERROR;
            }
        }
    }

    ಆದರೆ ((U_FAILURE(local_ಸ್ಥಿತಿ)) && strchr(id.ದತ್ತಾಂಶ(), '_') != ಶೂನ್ಯಸೂಚಕ) {
        // We don't know about it.  Check to see if we support the variant.
        CharString ಪೋಷಕ = ulocimp_get_ಪೋಷಕ(locale, *ec);
        *ec = U_USING_FALLBACK_WARNING;
        // TODO: Loop over the parent rather than recursing and
        // looking again for a currency keyword.
        ಹಿಂತಿರುಗಿ ucurr_forLocale(ಪೋಷಕ.ದತ್ತಾಂಶ(), buff, buff_ಸಾಮರ್ಥ್ಯ, ec);
    }
    ಆದರೆ (*ec == U_ZERO_ERROR || local_ಸ್ಥಿತಿ != U_ZERO_ERROR) {
        // There is nothing to fallback to. Report the failure/warning if possible.
        *ec = local_ಸ್ಥಿತಿ;
    }
    ಆದರೆ (U_SUCCESS(*ec)) {
        ಆದರೆ(buff_ಸಾಮರ್ಥ್ಯ > resLen) {
            u_strcpy(buff, s);
        }
    }
    ಹಿಂತಿರುಗಿ u_terminateUChars(buff, buff_ಸಾಮರ್ಥ್ಯ, resLen, ec);
}

// end registration

/**
 * Modify the given locale name by removing the rightmost _-delimited
 * element.  If there is none, empty the string ("" == root).
 * NOTE: The string "root" is not recognized; do not use it.
 * @return true if the fallback happened; false if locale is already
 * root ("").
 */
ಸ್ಥಿರ UBool fallback(CharString& loc) {
    ಆದರೆ (loc.is_ಖಾಲಿಗಳು()) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    UError_ಸಂಕೇತ ಸ್ಥಿತಿ_2 = U_ZERO_ERROR;
    ಆದರೆ (loc == "en_GB") {
        // HACK: See #13368.  We need "en_GB" to fall back to "en_001" instead of "en"
        // in order to consume the correct data strings.  This hack will be removed
        // when proper data sink loading is implemented here.
        loc.truncate(3);
        loc.append("001", ಸ್ಥಿತಿ_2);
    } ಇಲ್ಲದಿದ್ದರೆ {
        loc = ulocimp_get_ಪೋಷಕ(loc.ದತ್ತಾಂಶ(), ಸ್ಥಿತಿ_2);
    }
 /*
    char *i = uprv_strrchr(loc, '_');
    if (i == nullptr) {
        i = loc;
    }
    *i = 0;
 */
    ಹಿಂತಿರುಗಿ ನಿಜ;
}


U_CAPI ಸ್ಥಿರಾಂಕ char16_t* U_EXPORT2
ucurr_getName(ಸ್ಥಿರಾಂಕ char16_t* currency U_LIFETIME_BOUND,
              ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* locale,
              UCurrNameStyle nameStyle,
              UBool* isChoiceFormat, // fillin
              int32_t* len, // fillin
              UError_ಸಂಕೇತ* ec) {

    // Look up the Currencies resource for the given locale.  The
    // Currencies locale data looks like this:
    //|en {
    //|  Currencies {
    //|    USD { "US$", "US Dollar" }
    //|    CHF { "Sw F", "Swiss Franc" }
    //|    INR { "=0#Rs|1#Re|1<Rs", "=0#Rupees|1#Rupee|1<Rupees" }
    //|    //...
    //|  }
    //|}

    ಆದರೆ (U_FAILURE(*ec)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    int32_t choice = (int32_t) nameStyle;
    ಆದರೆ (choice < 0 || choice > 4) {
        *ec = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    // In the future, resource bundles may implement multi-level
    // fallback.  That is, if a currency is not found in the en_US
    // Currencies data, then the en Currencies data will be searched.
    // Currently, if a Currencies datum exists in en_US and en, the
    // en_US entry hides that in en.

    // We want multi-level fallback for this resource, so we implement
    // it manually.

    // Use a separate UErrorCode here that does not propagate out of
    // this function.
    UError_ಸಂಕೇತ ec2 = U_ZERO_ERROR;

    ಆದರೆ (locale == ಶೂನ್ಯಸೂಚಕ) {
        locale = uloc_getDefault();
    }
    CharString loc = ulocimp_getName(locale, ec2);
    ಆದರೆ (U_FAILURE(ec2)) {
        *ec = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    ಅಕ್ಷರ buf[ISO_CURRENCY_ಸಂಕೇತ_ಉದ್ದಳತೆ+1];
    my_UChars_ಗಮ್ಯ_Chars(buf, currency);
    
    /* Normalize the keyword value to uppercase */
    T_CString_ಗಮ್ಯ_ಊರ್ಧ್ವ_Case(buf);
    
    ಸ್ಥಿರಾಂಕ char16_t* s = ಶೂನ್ಯಸೂಚಕ;
    ec2 = U_ZERO_ERROR;
    Local_UResource_Bundle_ಸೂಚಕ rb(ures_open(U_ICUDATA_CURR, loc.ದತ್ತಾಂಶ(), &ec2));

    ಆದರೆ (nameStyle == UCURR_NARROW_ಚಿಹ್ನೆ_NAME || nameStyle == UCURR_FORMAL_ಚಿಹ್ನೆ_NAME || nameStyle == UCURR_VARIANT_ಚಿಹ್ನೆ_NAME) {
        CharString ಕೀಲಿ;
        ಆಯ್ಕೆ (nameStyle) {
        ಸ್ಥಿತಿ UCURR_NARROW_ಚಿಹ್ನೆ_NAME:
            ಕೀಲಿ.append(CURRENCIES_NARROW, ec2);
            ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ UCURR_FORMAL_ಚಿಹ್ನೆ_NAME:
            ಕೀಲಿ.append(CURRENCIES_FORMAL, ec2);
            ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ UCURR_VARIANT_ಚಿಹ್ನೆ_NAME:
            ಕೀಲಿ.append(CURRENCIES_VARIANT, ec2);
            ನಿಲ್ಲಿಸು;
        ಪೂರ್ವನಿಯೋಜಿತ:
            *ec = U_UNSUPPORTED_ERROR;
            ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
        }
        ಕೀಲಿ.append("/", ec2);
        ಕೀಲಿ.append(buf, ec2);
        s = ures_get_String_By_ಕೀಲಿ_With_Fallback(rb.getAlias(), ಕೀಲಿ.ದತ್ತಾಂಶ(), len, &ec2);
        ಆದರೆ (ec2 == U_MISSING_RESOURCE_ERROR) {
            *ec = U_USING_FALLBACK_WARNING;
            ec2 = U_ZERO_ERROR;
            choice = UCURR_ಚಿಹ್ನೆ_NAME;
        }
    }
    ಆದರೆ (s == ಶೂನ್ಯಸೂಚಕ) {
        ures_get_By_ಕೀಲಿ(rb.getAlias(), CURRENCIES, rb.getAlias(), &ec2);
        ures_get_By_ಕೀಲಿ_With_Fallback(rb.getAlias(), buf, rb.getAlias(), &ec2);
        s = ures_get_String_By_ಸೂಚ್ಯಂಕ(rb.getAlias(), choice, len, &ec2);
    }

    // If we've succeeded we're done.  Otherwise, try to fallback.
    // If that fails (because we are already at root) then exit.
    ಆದರೆ (U_SUCCESS(ec2)) {
        ಆದರೆ (ec2 == U_USING_DEFAULT_WARNING
            || (ec2 == U_USING_FALLBACK_WARNING && *ec != U_USING_DEFAULT_WARNING)) {
            *ec = ec2;
        }
    }

    // We no longer support choice format data in names.  Data should not contain
    // choice patterns.
    ಆದರೆ (isChoiceFormat != ಶೂನ್ಯಸೂಚಕ) {
        *isChoiceFormat = ಸುಳ್ಳು;
    }
    ಆದರೆ (U_SUCCESS(ec2)) {
        U_ASSERT(s != ಶೂನ್ಯಸೂಚಕ);
        ಹಿಂತಿರುಗಿ s;
    }

    // If we fail to find a match, use the ISO 4217 code
    *len = u_strlen(currency); // Should == ISO_CURRENCY_CODE_LENGTH, but maybe not...?
    *ec = U_USING_DEFAULT_WARNING;
    ಹಿಂತಿರುಗಿ currency;
}

U_CAPI ಸ್ಥಿರಾಂಕ char16_t* U_EXPORT2
ucurr_getPluralName(ಸ್ಥಿರಾಂಕ char16_t* currency U_LIFETIME_BOUND,
                    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* locale,
                    UBool* isChoiceFormat,
                    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* plural_ಎಣಿಕೆ,
                    int32_t* len, // fillin
                    UError_ಸಂಕೇತ* ec) {
    // Look up the Currencies resource for the given locale.  The
    // Currencies locale data looks like this:
    //|en {
    //|  CurrencyPlurals {
    //|    USD{
    //|      one{"US dollar"}
    //|      other{"US dollars"}
    //|    }
    //|  }
    //|}

    ಆದರೆ (U_FAILURE(*ec)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    // Use a separate UErrorCode here that does not propagate out of
    // this function.
    UError_ಸಂಕೇತ ec2 = U_ZERO_ERROR;

    ಆದರೆ (locale == ಶೂನ್ಯಸೂಚಕ) {
        locale = uloc_getDefault();
    }
    CharString loc = ulocimp_getName(locale, ec2);
    ಆದರೆ (U_FAILURE(ec2)) {
        *ec = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    ಅಕ್ಷರ buf[ISO_CURRENCY_ಸಂಕೇತ_ಉದ್ದಳತೆ+1];
    my_UChars_ಗಮ್ಯ_Chars(buf, currency);

    ಸ್ಥಿರಾಂಕ char16_t* s = ಶೂನ್ಯಸೂಚಕ;
    ec2 = U_ZERO_ERROR;
    UResourceBundle* rb = ures_open(U_ICUDATA_CURR, loc.ದತ್ತಾಂಶ(), &ec2);

    rb = ures_get_By_ಕೀಲಿ(rb, CURRENCYPLURALS, rb, &ec2);

    // Fetch resource with multi-level resource inheritance fallback
    Local_UResource_Bundle_ಸೂಚಕ curr(ures_get_By_ಕೀಲಿ_With_Fallback(rb, buf, rb, &ec2));

    s = ures_get_String_By_ಕೀಲಿ_With_Fallback(curr.getAlias(), plural_ಎಣಿಕೆ, len, &ec2);
    ಆದರೆ (U_FAILURE(ec2)) {
        //  fall back to "other"
        ec2 = U_ZERO_ERROR;
        s = ures_get_String_By_ಕೀಲಿ_With_Fallback(curr.getAlias(), "other", len, &ec2);     
        ಆದರೆ (U_FAILURE(ec2)) {
            // fall back to long name in Currencies
            ಹಿಂತಿರುಗಿ ucurr_getName(currency, locale, UCURR_LONG_NAME, 
                                 isChoiceFormat, len, ec);
        }
    }

    // If we've succeeded we're done.  Otherwise, try to fallback.
    // If that fails (because we are already at root) then exit.
    ಆದರೆ (U_SUCCESS(ec2)) {
        ಆದರೆ (ec2 == U_USING_DEFAULT_WARNING
            || (ec2 == U_USING_FALLBACK_WARNING && *ec != U_USING_DEFAULT_WARNING)) {
            *ec = ec2;
        }
        U_ASSERT(s != ಶೂನ್ಯಸೂಚಕ);
        ಹಿಂತಿರುಗಿ s;
    }

    // If we fail to find a match, use the ISO 4217 code
    *len = u_strlen(currency); // Should == ISO_CURRENCY_CODE_LENGTH, but maybe not...?
    *ec = U_USING_DEFAULT_WARNING;
    ಹಿಂತಿರುಗಿ currency;
}


//========================================================================
// Following are structure and function for parsing currency names

#ವ್ಯಾಖ್ಯಾನಿಸು NEED_ಗಮ್ಯ_BE_DELETED 0x1

// TODO: a better way to define this?
#ವ್ಯಾಖ್ಯಾನಿಸು MAX_CURRENCY_NAME_LEN 100

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* Iso_ಸಂಕೇತ;  //ಕೀಲಿ
    char16_t* currencyName;  //ಮೌಲ್ಯ
    int32_t currencyNameLen;  // value length
    int32_t flag;  // flags
} CurrencyNameStruct;


#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ MIN
#ವ್ಯಾಖ್ಯಾನಿಸು MIN(a,b) (((a)<(b)) ? (a) : (b))
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ MAX
#ವ್ಯಾಖ್ಯಾನಿಸು MAX(a,b) (((a)<(b)) ? (b) : (a))
#ಆದರೆ_ಅಂತ್ಯ


// Comparison function used in quick sort.
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ U_CALLCONV currencyNameComparator(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ* a, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ* b) {
    ಸ್ಥಿರಾಂಕ CurrencyNameStruct* currName_1 = static_cast<ಸ್ಥಿರಾಂಕ CurrencyNameStruct*>(a);
    ಸ್ಥಿರಾಂಕ CurrencyNameStruct* currName_2 = static_cast<ಸ್ಥಿರಾಂಕ CurrencyNameStruct*>(b);
    ಗಾಗಿ (int32_t i = 0; 
         i < MIN(currName_1->currencyNameLen, currName_2->currencyNameLen);
         ++i) {
        ಆದರೆ (currName_1->currencyName[i] < currName_2->currencyName[i]) {
            ಹಿಂತಿರುಗಿ -1;
        }
        ಆದರೆ (currName_1->currencyName[i] > currName_2->currencyName[i]) {
            ಹಿಂತಿರುಗಿ 1;
        }
    }
    ಆದರೆ (currName_1->currencyNameLen < currName_2->currencyNameLen) {
        ಹಿಂತಿರುಗಿ -1;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (currName_1->currencyNameLen > currName_2->currencyNameLen) {
        ಹಿಂತಿರುಗಿ 1;
    }
    ಹಿಂತಿರುಗಿ 0;
}


// Give a locale, return the maximum number of currency names associated with
// this locale.
// It gets currency names from resource bundles using fallback.
// It is the maximum number because in the fallback chain, some of the 
// currency names are duplicated.
// For example, given locale as "en_US", the currency names get from resource
// bundle in "en_US" and "en" are duplicated. The fallback mechanism will count
// all currency names in "en_US" and "en".
ಸ್ಥಿರ ಶೂನ್ಯ
get_Currency_Name_ಎಣಿಕೆ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* loc, int32_t* ಒಟ್ಟು_currency_name_ಎಣಿಕೆ, int32_t* ಒಟ್ಟು_currency_ಚಿಹ್ನೆ_ಎಣಿಕೆ) {
    U_NAMESPACE_USE
    *ಒಟ್ಟು_currency_name_ಎಣಿಕೆ = 0;
    *ಒಟ್ಟು_currency_ಚಿಹ್ನೆ_ಎಣಿಕೆ = 0;
    ಸ್ಥಿರಾಂಕ char16_t* s = ಶೂನ್ಯಸೂಚಕ;
    CharString locale;
    {
        UError_ಸಂಕೇತ ಸ್ಥಿತಿ_2 = U_ZERO_ERROR;
        locale.append(loc, ಸ್ಥಿತಿ_2);
        ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) { ಹಿಂತಿರುಗಿ; }
    }
    ಸ್ಥಿರಾಂಕ icu::Hashtable *currencySymbolsEquiv = getCurrSymbolsEquiv();
    ಗಾಗಿ (;;) {
        UError_ಸಂಕೇತ ec2 = U_ZERO_ERROR;
        // TODO: ures_openDirect?
        Local_UResource_Bundle_ಸೂಚಕ rb(ures_open(U_ICUDATA_CURR, locale.ದತ್ತಾಂಶ(), &ec2));
        Local_UResource_Bundle_ಸೂಚಕ curr(ures_get_By_ಕೀಲಿ(rb.getAlias(), CURRENCIES, ಶೂನ್ಯಸೂಚಕ, &ec2));
        int32_t n = ures_get_ಗಾತ್ರ(curr.getAlias());
        ಗಾಗಿ (int32_t i=0; i<n; ++i) {
            Local_UResource_Bundle_ಸೂಚಕ ಹೆಸರುಗಳು(ures_get_By_ಸೂಚ್ಯಂಕ(curr.getAlias(), i, ಶೂನ್ಯಸೂಚಕ, &ec2));
            int32_t len;
            s = ures_get_String_By_ಸೂಚ್ಯಂಕ(ಹೆಸರುಗಳು.getAlias(), UCURR_ಚಿಹ್ನೆ_NAME, &len, &ec2);
            ++(*ಒಟ್ಟು_currency_ಚಿಹ್ನೆ_ಎಣಿಕೆ);  // currency symbol
            ಆದರೆ (currencySymbolsEquiv != ಶೂನ್ಯಸೂಚಕ) {
                *ಒಟ್ಟು_currency_ಚಿಹ್ನೆ_ಎಣಿಕೆ += ಎಣಿಕೆ_Equivalent(*currencySymbolsEquiv, UnicodeString(ನಿಜ, s, len));
            }
            ++(*ಒಟ್ಟು_currency_ಚಿಹ್ನೆ_ಎಣಿಕೆ); // iso code
            ++(*ಒಟ್ಟು_currency_name_ಎಣಿಕೆ); // long name
        }

        // currency plurals
        UError_ಸಂಕೇತ ec3 = U_ZERO_ERROR;
        Local_UResource_Bundle_ಸೂಚಕ curr_p(ures_get_By_ಕೀಲಿ(rb.getAlias(), CURRENCYPLURALS, ಶೂನ್ಯಸೂಚಕ, &ec3));
        n = ures_get_ಗಾತ್ರ(curr_p.getAlias());
        ಗಾಗಿ (int32_t i=0; i<n; ++i) {
            Local_UResource_Bundle_ಸೂಚಕ ಹೆಸರುಗಳು(ures_get_By_ಸೂಚ್ಯಂಕ(curr_p.getAlias(), i, ಶೂನ್ಯಸೂಚಕ, &ec3));
            *ಒಟ್ಟು_currency_name_ಎಣಿಕೆ += ures_get_ಗಾತ್ರ(ಹೆಸರುಗಳು.getAlias());
        }

        ಆದರೆ (!fallback(locale)) {
            ನಿಲ್ಲಿಸು;
        }
    }
}

ಸ್ಥಿರ char16_t*
ಗಮ್ಯ_ಊರ್ಧ್ವ_Case(ಸ್ಥಿರಾಂಕ char16_t* ಮೂಲ, int32_t len, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* locale) {
    char16_t* dest = ಶೂನ್ಯಸೂಚಕ;
    UError_ಸಂಕೇತ ec = U_ZERO_ERROR;
    int32_t destLen = u_str_ಗಮ್ಯ_ಊರ್ಧ್ವ(dest, 0, ಮೂಲ, len, locale, &ec);

    ec = U_ZERO_ERROR;
    dest = static_cast<char16_t*>(uprv_malloc(ಗಾತ್ರ(char16_t) * MAX(destLen, len)));
    ಆದರೆ (dest == ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    u_str_ಗಮ್ಯ_ಊರ್ಧ್ವ(dest, destLen, ಮೂಲ, len, locale, &ec);
    ಆದರೆ (U_FAILURE(ec)) {
        u_memcpy(dest, ಮೂಲ, len);
    } 
    ಹಿಂತಿರುಗಿ dest;
}


ಸ್ಥಿರ ಶೂನ್ಯ delete_Currency_ಹೆಸರುಗಳು(CurrencyNameStruct* currency_ಹೆಸರುಗಳು, int32_t count);
// Collect all available currency names associated with the given locale
// (enable fallback chain).
// Read currenc names defined in resource bundle "Currencies" and
// "CurrencyPlural", enable fallback chain.
// return the malloc-ed currency name arrays and the total number of currency
// names in the array.
ಸ್ಥಿರ ಶೂನ್ಯ
collect_Currency_ಹೆಸರುಗಳು(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* locale, 
                     CurrencyNameStruct** currency_ಹೆಸರುಗಳು, 
                     int32_t* ಒಟ್ಟು_currency_name_ಎಣಿಕೆ, 
                     CurrencyNameStruct** currencySymbols, 
                     int32_t* ಒಟ್ಟು_currency_ಚಿಹ್ನೆ_ಎಣಿಕೆ, 
                     UError_ಸಂಕೇತ& ec) {
    ಆದರೆ (U_FAILURE(ec)) {
        *currency_ಹೆಸರುಗಳು = *currencySymbols = ಶೂನ್ಯಸೂಚಕ;
        *ಒಟ್ಟು_currency_name_ಎಣಿಕೆ = *ಒಟ್ಟು_currency_ಚಿಹ್ನೆ_ಎಣಿಕೆ = 0;
        ಹಿಂತಿರುಗಿ;
    }
    U_NAMESPACE_USE
    ಸ್ಥಿರಾಂಕ icu::Hashtable *currencySymbolsEquiv = getCurrSymbolsEquiv();
    // Look up the Currencies resource for the given locale.
    UError_ಸಂಕೇತ ec2 = U_ZERO_ERROR;

    ಆದರೆ (locale == ಶೂನ್ಯಸೂಚಕ) {
        locale = uloc_getDefault();
    }
    CharString loc = ulocimp_getName(locale, ec2);
    ಆದರೆ (U_FAILURE(ec2)) {
        ec = U_ILLEGAL_ವಾದ_ERROR;
        *currency_ಹೆಸರುಗಳು = *currencySymbols = ಶೂನ್ಯಸೂಚಕ;
        *ಒಟ್ಟು_currency_name_ಎಣಿಕೆ = *ಒಟ್ಟು_currency_ಚಿಹ್ನೆ_ಎಣಿಕೆ = 0;
        ಹಿಂತಿರುಗಿ;
    }

    // Get maximum currency name count first.
    get_Currency_Name_ಎಣಿಕೆ(loc.ದತ್ತಾಂಶ(), ಒಟ್ಟು_currency_name_ಎಣಿಕೆ, ಒಟ್ಟು_currency_ಚಿಹ್ನೆ_ಎಣಿಕೆ);

    *currency_ಹೆಸರುಗಳು = static_cast<CurrencyNameStruct*>(
        uprv_malloc(ಗಾತ್ರ(CurrencyNameStruct) * (*ಒಟ್ಟು_currency_name_ಎಣಿಕೆ)));
    ಆದರೆ(*currency_ಹೆಸರುಗಳು == ಶೂನ್ಯಸೂಚಕ) {
        *currencySymbols = ಶೂನ್ಯಸೂಚಕ;
        *ಒಟ್ಟು_currency_name_ಎಣಿಕೆ = *ಒಟ್ಟು_currency_ಚಿಹ್ನೆ_ಎಣಿಕೆ = 0;
        ec = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }
    *currencySymbols = static_cast<CurrencyNameStruct*>(
        uprv_malloc(ಗಾತ್ರ(CurrencyNameStruct) * (*ಒಟ್ಟು_currency_ಚಿಹ್ನೆ_ಎಣಿಕೆ)));

    ಆದರೆ(*currencySymbols == ಶೂನ್ಯಸೂಚಕ) {
        uprv_free(*currency_ಹೆಸರುಗಳು);
        *currency_ಹೆಸರುಗಳು = ಶೂನ್ಯಸೂಚಕ;
        *ಒಟ್ಟು_currency_name_ಎಣಿಕೆ = *ಒಟ್ಟು_currency_ಚಿಹ್ನೆ_ಎಣಿಕೆ = 0;
        ec = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }

    ಸ್ಥಿರಾಂಕ char16_t* s = ಶೂನ್ಯಸೂಚಕ;  // currency name
    ಅಕ್ಷರ* iso = ಶೂನ್ಯಸೂಚಕ;  // currency ISO code

    *ಒಟ್ಟು_currency_name_ಎಣಿಕೆ = 0;
    *ಒಟ್ಟು_currency_ಚಿಹ್ನೆ_ಎಣಿಕೆ = 0;

    UError_ಸಂಕೇತ ec3 = U_ZERO_ERROR;
    UError_ಸಂಕೇತ ec4 = U_ZERO_ERROR;

    // Using hash to remove duplicates caused by locale fallback
    Local_UHashtable_ಸೂಚಕ currencyIsoCodes(uhash_open(uhash_hashChars, uhash_compareChars, ಶೂನ್ಯಸೂಚಕ, &ec3));
    Local_UHashtable_ಸೂಚಕ currencyPluralIsoCodes(uhash_open(uhash_hashChars, uhash_compareChars, ಶೂನ್ಯಸೂಚಕ, &ec4));
    ಗಾಗಿ (int32_t localeLevel = 0; ; ++localeLevel) {
        ec2 = U_ZERO_ERROR;
        // TODO: ures_openDirect
        Local_UResource_Bundle_ಸೂಚಕ rb(ures_open(U_ICUDATA_CURR, loc.ದತ್ತಾಂಶ(), &ec2));
        Local_UResource_Bundle_ಸೂಚಕ curr(ures_get_By_ಕೀಲಿ(rb.getAlias(), CURRENCIES, ಶೂನ್ಯಸೂಚಕ, &ec2));
        int32_t n = ures_get_ಗಾತ್ರ(curr.getAlias());
        ಗಾಗಿ (int32_t i=0; i<n; ++i) {
            Local_UResource_Bundle_ಸೂಚಕ ಹೆಸರುಗಳು(ures_get_By_ಸೂಚ್ಯಂಕ(curr.getAlias(), i, ಶೂನ್ಯಸೂಚಕ, &ec2));
            int32_t len;
            s = ures_get_String_By_ಸೂಚ್ಯಂಕ(ಹೆಸರುಗಳು.getAlias(), UCURR_ಚಿಹ್ನೆ_NAME, &len, &ec2);
            // TODO: uhash_put wont change key/value?
            iso = const_cast<ಅಕ್ಷರ*>(ures_get_ಕೀಲಿ(ಹೆಸರುಗಳು.getAlias()));
            ಆದರೆ (localeLevel != 0 && uhash_get(currencyIsoCodes.getAlias(), iso) != ಶೂನ್ಯಸೂಚಕ) {
                ಮುಂದುವರಿ;
            }
            uhash_put(currencyIsoCodes.getAlias(), iso, iso, &ec3); 
            // Add currency symbol.
            (*currencySymbols)[(*ಒಟ್ಟು_currency_ಚಿಹ್ನೆ_ಎಣಿಕೆ)++] = {iso, const_cast<char16_t*>(s), len, 0};

            // Add equivalent symbols
            ಆದರೆ (currencySymbolsEquiv != ಶೂನ್ಯಸೂಚಕ) {
                UnicodeString str(ನಿಜ, s, len);
                icu::EquivIterator iter(*currencySymbolsEquiv, str);
                ಸ್ಥಿರಾಂಕ UnicodeString *ಚಿಹ್ನೆ;
                ಆಗ ((ಚಿಹ್ನೆ = iter.next()) != ಶೂನ್ಯಸೂಚಕ) {
                    (*currencySymbols)[(*ಒಟ್ಟು_currency_ಚಿಹ್ನೆ_ಎಣಿಕೆ)++]
                        = {iso, const_cast<char16_t*>(ಚಿಹ್ನೆ->get_ಬಫರ್()), ಚಿಹ್ನೆ->length(), 0};
                }
            }

            // Add currency long name.
            s = ures_get_String_By_ಸೂಚ್ಯಂಕ(ಹೆಸರುಗಳು.getAlias(), UCURR_LONG_NAME, &len, &ec2);
            char16_t* ಊರ್ಧ್ವ_Name = ಗಮ್ಯ_ಊರ್ಧ್ವ_Case(s, len, locale);
            ಆದರೆ (ಊರ್ಧ್ವ_Name == ಶೂನ್ಯಸೂಚಕ) {
                ec = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
                ಹೋಗು error;
            }
            (*currency_ಹೆಸರುಗಳು)[(*ಒಟ್ಟು_currency_name_ಎಣಿಕೆ)++] = {iso, ಊರ್ಧ್ವ_Name, len, NEED_ಗಮ್ಯ_BE_DELETED};

            // put (iso, 3, and iso) in to array
            // Add currency ISO code.
            char16_t* iso_ಸಂಕೇತ = static_cast<char16_t*>(uprv_malloc(ಗಾತ್ರ(char16_t) * 3));
            ಆದರೆ (iso_ಸಂಕೇತ == ಶೂನ್ಯಸೂಚಕ) {
                ec = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
                ಹೋಗು error;
            }
            // Must convert iso[] into Unicode
            u_chars_ಗಮ್ಯ_UChars(iso, iso_ಸಂಕೇತ, 3);
            (*currencySymbols)[(*ಒಟ್ಟು_currency_ಚಿಹ್ನೆ_ಎಣಿಕೆ)++] = {iso, iso_ಸಂಕೇತ, 3, NEED_ಗಮ್ಯ_BE_DELETED};
        }

        // currency plurals
        UError_ಸಂಕೇತ ec5 = U_ZERO_ERROR;
        Local_UResource_Bundle_ಸೂಚಕ curr_p(ures_get_By_ಕೀಲಿ(rb.getAlias(), CURRENCYPLURALS, ಶೂನ್ಯಸೂಚಕ, &ec5));
        n = ures_get_ಗಾತ್ರ(curr_p.getAlias());
        ಗಾಗಿ (int32_t i=0; i<n; ++i) {
            Local_UResource_Bundle_ಸೂಚಕ ಹೆಸರುಗಳು(ures_get_By_ಸೂಚ್ಯಂಕ(curr_p.getAlias(), i, ಶೂನ್ಯಸೂಚಕ, &ec5));
            iso = const_cast<ಅಕ್ಷರ*>(ures_get_ಕೀಲಿ(ಹೆಸರುಗಳು.getAlias()));
            // Using hash to remove duplicated ISO codes in fallback chain.
            ಆದರೆ (localeLevel != 0 && uhash_get(currencyPluralIsoCodes.getAlias(), iso) != ಶೂನ್ಯಸೂಚಕ) {
                ಮುಂದುವರಿ;
            }
            uhash_put(currencyPluralIsoCodes.getAlias(), iso, iso, &ec4);
            int32_t num = ures_get_ಗಾತ್ರ(ಹೆಸರುಗಳು.getAlias());
            int32_t len;
            ಗಾಗಿ (int32_t j = 0; j < num; ++j) {
                // TODO: remove duplicates between singular name and 
                // currency long name?
                s = ures_get_String_By_ಸೂಚ್ಯಂಕ(ಹೆಸರುಗಳು.getAlias(), j, &len, &ec5);
                char16_t* ಊರ್ಧ್ವ_Name = ಗಮ್ಯ_ಊರ್ಧ್ವ_Case(s, len, locale);
                ಆದರೆ (ಊರ್ಧ್ವ_Name == ಶೂನ್ಯಸೂಚಕ) {
                    ec = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
                    ಹೋಗು error;
                }
                (*currency_ಹೆಸರುಗಳು)[(*ಒಟ್ಟು_currency_name_ಎಣಿಕೆ)++] = {iso, ಊರ್ಧ್ವ_Name, len, NEED_ಗಮ್ಯ_BE_DELETED};
            }
        }

        ಆದರೆ (!fallback(loc)) {
            ನಿಲ್ಲಿಸು;
        }
    }

    // quick sort the struct
    qsort(*currency_ಹೆಸರುಗಳು, *ಒಟ್ಟು_currency_name_ಎಣಿಕೆ, 
          ಗಾತ್ರ(CurrencyNameStruct), currencyNameComparator);
    qsort(*currencySymbols, *ಒಟ್ಟು_currency_ಚಿಹ್ನೆ_ಎಣಿಕೆ, 
          ಗಾತ್ರ(CurrencyNameStruct), currencyNameComparator);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UCURR_DEBUG
    ಬರೆ("currency name count: %d\n", *ಒಟ್ಟು_currency_name_ಎಣಿಕೆ);
    ಗಾಗಿ (int32_t ಸೂಚ್ಯಂಕ = 0; ಸೂಚ್ಯಂಕ < *ಒಟ್ಟು_currency_name_ಎಣಿಕೆ; ++ಸೂಚ್ಯಂಕ) {
        ಬರೆ("index: %d\n", ಸೂಚ್ಯಂಕ);
        ಬರೆ("iso: %s\n", (*currency_ಹೆಸರುಗಳು)[ಸೂಚ್ಯಂಕ].Iso_ಸಂಕೇತ);
        ಅಕ್ಷರ curNameBuf[1024];
        memset(curNameBuf, 0, 1024);
        u_austrncpy(curNameBuf, (*currency_ಹೆಸರುಗಳು)[ಸೂಚ್ಯಂಕ].currencyName, (*currency_ಹೆಸರುಗಳು)[ಸೂಚ್ಯಂಕ].currencyNameLen);
        ಬರೆ("currencyName: %s\n", curNameBuf);
        ಬರೆ("len: %d\n", (*currency_ಹೆಸರುಗಳು)[ಸೂಚ್ಯಂಕ].currencyNameLen);
    }
    ಬರೆ("currency symbol count: %d\n", *ಒಟ್ಟು_currency_ಚಿಹ್ನೆ_ಎಣಿಕೆ);
    ಗಾಗಿ (int32_t ಸೂಚ್ಯಂಕ = 0; ಸೂಚ್ಯಂಕ < *ಒಟ್ಟು_currency_ಚಿಹ್ನೆ_ಎಣಿಕೆ; ++ಸೂಚ್ಯಂಕ) {
        ಬರೆ("index: %d\n", ಸೂಚ್ಯಂಕ);
        ಬರೆ("iso: %s\n", (*currencySymbols)[ಸೂಚ್ಯಂಕ].Iso_ಸಂಕೇತ);
        ಅಕ್ಷರ curNameBuf[1024];
        memset(curNameBuf, 0, 1024);
        u_austrncpy(curNameBuf, (*currencySymbols)[ಸೂಚ್ಯಂಕ].currencyName, (*currencySymbols)[ಸೂಚ್ಯಂಕ].currencyNameLen);
        ಬರೆ("currencySymbol: %s\n", curNameBuf);
        ಬರೆ("len: %d\n", (*currencySymbols)[ಸೂಚ್ಯಂಕ].currencyNameLen);
    }
#ಆದರೆ_ಅಂತ್ಯ
    // fail on hashtable errors
    ಆದರೆ (U_FAILURE(ec3)) {
      ec = ec3;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (U_FAILURE(ec4)) {
      ec = ec4;
    }

  error:
    // clean up if we got error
    ಆದರೆ (U_FAILURE(ec)) {
        delete_Currency_ಹೆಸರುಗಳು(*currency_ಹೆಸರುಗಳು, *ಒಟ್ಟು_currency_name_ಎಣಿಕೆ);
        delete_Currency_ಹೆಸರುಗಳು(*currencySymbols, *ಒಟ್ಟು_currency_ಚಿಹ್ನೆ_ಎಣಿಕೆ);
        *currency_ಹೆಸರುಗಳು = *currencySymbols = ಶೂನ್ಯಸೂಚಕ;
        *ಒಟ್ಟು_currency_name_ಎಣಿಕೆ = *ಒಟ್ಟು_currency_ಚಿಹ್ನೆ_ಎಣಿಕೆ = 0;
    }
}

// @param  currencyNames: currency names array
// @param  indexInCurrencyNames: the index of the character in currency names 
//         array against which the comparison is done
// @param  key: input text char to compare against
// @param  begin(IN/OUT): the begin index of matching range in currency names array
// @param  end(IN/OUT): the end index of matching range in currency names array.
ಸ್ಥಿರ int32_t
ದ್ವಿಮಾನ_ಹುಡುಕು(ಸ್ಥಿರಾಂಕ CurrencyNameStruct* currency_ಹೆಸರುಗಳು, 
             int32_t ಸೂಚ್ಯಂಕ_ಒಳ_Currency_ಹೆಸರುಗಳು,
             ಸ್ಥಿರಾಂಕ char16_t ಕೀಲಿ,
             int32_t* begin, int32_t* ಕೊನೆ) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UCURR_DEBUG
    ಬರೆ("key = %x\n", ಕೀಲಿ);
#ಆದರೆ_ಅಂತ್ಯ
   int32_t first = *begin;
   int32_t ಕೊನೆಯ = *ಕೊನೆ;
   ಆಗ (first <= ಕೊನೆಯ) {
       int32_t mid = (first + ಕೊನೆಯ) / 2;  // compute mid point.
       ಆದರೆ (ಸೂಚ್ಯಂಕ_ಒಳ_Currency_ಹೆಸರುಗಳು >= currency_ಹೆಸರುಗಳು[mid].currencyNameLen) {
           first = mid + 1;
       } ಇಲ್ಲದಿದ್ದರೆ {
           ಆದರೆ (ಕೀಲಿ > currency_ಹೆಸರುಗಳು[mid].currencyName[ಸೂಚ್ಯಂಕ_ಒಳ_Currency_ಹೆಸರುಗಳು]) {
               first = mid + 1;
           }
           ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಕೀಲಿ < currency_ಹೆಸರುಗಳು[mid].currencyName[ಸೂಚ್ಯಂಕ_ಒಳ_Currency_ಹೆಸರುಗಳು]) {
               ಕೊನೆಯ = mid - 1;
           }
           ಇಲ್ಲದಿದ್ದರೆ {
                // Find a match, and looking for ranges
                // Now do two more binary searches. First, on the left side for
                // the greatest L such that CurrencyNameStruct[L] < key.
                int32_t L = *begin;
                int32_t R = mid;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UCURR_DEBUG
                ಬರೆ("mid = %d\n", mid);
#ಆದರೆ_ಅಂತ್ಯ
                ಆಗ (L < R) {
                    int32_t M = (L + R) / 2;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UCURR_DEBUG
                    ಬರೆ("L = %d, R = %d, M = %d\n", L, R, M);
#ಆದರೆ_ಅಂತ್ಯ
                    ಆದರೆ (ಸೂಚ್ಯಂಕ_ಒಳ_Currency_ಹೆಸರುಗಳು >= currency_ಹೆಸರುಗಳು[M].currencyNameLen) {
                        L = M + 1;
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        ಆದರೆ (currency_ಹೆಸರುಗಳು[M].currencyName[ಸೂಚ್ಯಂಕ_ಒಳ_Currency_ಹೆಸರುಗಳು] < ಕೀಲಿ) {
                            L = M + 1;
                        } ಇಲ್ಲದಿದ್ದರೆ {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UCURR_DEBUG
                            U_ASSERT(currency_ಹೆಸರುಗಳು[M].currencyName[ಸೂಚ್ಯಂಕ_ಒಳ_Currency_ಹೆಸರುಗಳು] == ಕೀಲಿ);
#ಆದರೆ_ಅಂತ್ಯ
                            R = M;
                        }
                    }
                }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UCURR_DEBUG
                U_ASSERT(L == R);
#ಆದರೆ_ಅಂತ್ಯ
                *begin = L;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UCURR_DEBUG
                ಬರೆ("begin = %d\n", *begin);
                U_ASSERT(currency_ಹೆಸರುಗಳು[*begin].currencyName[ಸೂಚ್ಯಂಕ_ಒಳ_Currency_ಹೆಸರುಗಳು] == ಕೀಲಿ);
#ಆದರೆ_ಅಂತ್ಯ

                // Now for the second search, finding the least R such that
                // key < CurrencyNameStruct[R].
                L = mid;
                R = *ಕೊನೆ;
                ಆಗ (L < R) {
                    int32_t M = (L + R) / 2;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UCURR_DEBUG
                    ಬರೆ("L = %d, R = %d, M = %d\n", L, R, M);
#ಆದರೆ_ಅಂತ್ಯ
                    ಆದರೆ (currency_ಹೆಸರುಗಳು[M].currencyNameLen < ಸೂಚ್ಯಂಕ_ಒಳ_Currency_ಹೆಸರುಗಳು) {
                        L = M + 1;
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        ಆದರೆ (currency_ಹೆಸರುಗಳು[M].currencyName[ಸೂಚ್ಯಂಕ_ಒಳ_Currency_ಹೆಸರುಗಳು] > ಕೀಲಿ) {
                            R = M;
                        } ಇಲ್ಲದಿದ್ದರೆ {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UCURR_DEBUG
                            U_ASSERT(currency_ಹೆಸರುಗಳು[M].currencyName[ಸೂಚ್ಯಂಕ_ಒಳ_Currency_ಹೆಸರುಗಳು] == ಕೀಲಿ);
#ಆದರೆ_ಅಂತ್ಯ
                            L = M + 1;
                        }
                    }
                }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UCURR_DEBUG
                U_ASSERT(L == R);
#ಆದರೆ_ಅಂತ್ಯ
                ಆದರೆ (currency_ಹೆಸರುಗಳು[R].currencyName[ಸೂಚ್ಯಂಕ_ಒಳ_Currency_ಹೆಸರುಗಳು] > ಕೀಲಿ) {
                    *ಕೊನೆ = R - 1;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    *ಕೊನೆ = R;
                }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UCURR_DEBUG
                ಬರೆ("end = %d\n", *ಕೊನೆ);
#ಆದರೆ_ಅಂತ್ಯ

                // now, found the range. check whether there is exact match
                ಆದರೆ (currency_ಹೆಸರುಗಳು[*begin].currencyNameLen == ಸೂಚ್ಯಂಕ_ಒಳ_Currency_ಹೆಸರುಗಳು + 1) {
                    ಹಿಂತಿರುಗಿ *begin;  // find range and exact match.
                }
                ಹಿಂತಿರುಗಿ -1;  // find range, but no exact match.
           }
       }
   }
   *begin = -1;
   *ಕೊನೆ = -1;
   ಹಿಂತಿರುಗಿ -1;    // failed to find range.
}


// Linear search "text" in "currencyNames".
// @param  begin, end: the begin and end index in currencyNames, within which
//         range should the search be performed.
// @param  textLen: the length of the text to be compared
// @param  maxMatchLen(IN/OUT): passing in the computed max matching length
//                              pass out the new max  matching length
// @param  maxMatchIndex: the index in currencyName which has the longest
//                        match with input text.
ಸ್ಥಿರ ಶೂನ್ಯ
ರೇಖೀಯ_ಹುಡುಕು(ಸ್ಥಿರಾಂಕ CurrencyNameStruct* currency_ಹೆಸರುಗಳು, 
             int32_t begin, int32_t ಕೊನೆ,
             ಸ್ಥಿರಾಂಕ char16_t* ಪಠ್ಯ, int32_t ಪಠ್ಯ_Len,
             int32_t *partial_ಹೊಂದಿಕೆ_Len,
             int32_t *max_ಹೊಂದಿಕೆ_Len, int32_t* max_ಹೊಂದಿಕೆ_ಸೂಚ್ಯಂಕ) {
    int32_t initial_Partial_ಹೊಂದಿಕೆ_Len = *partial_ಹೊಂದಿಕೆ_Len;
    ಗಾಗಿ (int32_t ಸೂಚ್ಯಂಕ = begin; ಸೂಚ್ಯಂಕ <= ಕೊನೆ; ++ಸೂಚ್ಯಂಕ) {
        int32_t len = currency_ಹೆಸರುಗಳು[ಸೂಚ್ಯಂಕ].currencyNameLen;
        ಆದರೆ (len > *max_ಹೊಂದಿಕೆ_Len && len <= ಪಠ್ಯ_Len &&
            uprv_memcmp(currency_ಹೆಸರುಗಳು[ಸೂಚ್ಯಂಕ].currencyName, ಪಠ್ಯ, len * ಗಾತ್ರ(char16_t)) == 0) {
            *partial_ಹೊಂದಿಕೆ_Len = MAX(*partial_ಹೊಂದಿಕೆ_Len, len);
            *max_ಹೊಂದಿಕೆ_ಸೂಚ್ಯಂಕ = ಸೂಚ್ಯಂಕ;
            *max_ಹೊಂದಿಕೆ_Len = len;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UCURR_DEBUG
            ಬರೆ("maxMatchIndex = %d, maxMatchLen = %d\n",
                   *max_ಹೊಂದಿಕೆ_ಸೂಚ್ಯಂಕ, *max_ಹೊಂದಿಕೆ_Len);
#ಆದರೆ_ಅಂತ್ಯ
        } ಇಲ್ಲದಿದ್ದರೆ {
            // Check for partial matches.
            ಗಾಗಿ (int32_t i=initial_Partial_ಹೊಂದಿಕೆ_Len; i<MIN(len, ಪಠ್ಯ_Len); i++) {
                ಆದರೆ (currency_ಹೆಸರುಗಳು[ಸೂಚ್ಯಂಕ].currencyName[i] != ಪಠ್ಯ[i]) {
                    ನಿಲ್ಲಿಸು;
                }
                *partial_ಹೊಂದಿಕೆ_Len = MAX(*partial_ಹೊಂದಿಕೆ_Len, i + 1);
            }
        }
    }
}

#ವ್ಯಾಖ್ಯಾನಿಸು ರೇಖೀಯ_ಹುಡುಕು_ಮಿತಿ 10

// Find longest match between "text" and currency names in "currencyNames".
// @param  total_currency_count: total number of currency names in CurrencyNames.
// @param  textLen: the length of the text to be compared
// @param  maxMatchLen: passing in the computed max matching length
//                              pass out the new max  matching length
// @param  maxMatchIndex: the index in currencyName which has the longest
//                        match with input text.
ಸ್ಥಿರ ಶೂನ್ಯ
ಹುಡುಕು_Currency_Name(ಸ್ಥಿರಾಂಕ CurrencyNameStruct* currency_ಹೆಸರುಗಳು, 
                   int32_t ಒಟ್ಟು_currency_ಎಣಿಕೆ,
                   ಸ್ಥಿರಾಂಕ char16_t* ಪಠ್ಯ, int32_t ಪಠ್ಯ_Len,
                   int32_t *partial_ಹೊಂದಿಕೆ_Len,
                   int32_t* max_ಹೊಂದಿಕೆ_Len, int32_t* max_ಹೊಂದಿಕೆ_ಸೂಚ್ಯಂಕ) {
    *max_ಹೊಂದಿಕೆ_ಸೂಚ್ಯಂಕ = -1;
    *max_ಹೊಂದಿಕೆ_Len = 0;
    int32_t ಹೊಂದಿಕೆ_ಸೂಚ್ಯಂಕ = -1;
    int32_t ದ್ವಿಮಾನ_ಹುಡುಕು_Begin = 0;
    int32_t ದ್ವಿಮಾನ_ಹುಡುಕು_ಕೊನೆ = ಒಟ್ಟು_currency_ಎಣಿಕೆ - 1;
    // It is a variant of binary search.
    // For example, given the currency names in currencyNames array are:
    // A AB ABC AD AZ B BB BBEX BBEXYZ BS C D E....
    // and the input text is BBEXST
    // The first round binary search search "B" in the text against
    // the first char in currency names, and find the first char matching range
    // to be "B BB BBEX BBEXYZ BS" (and the maximum matching "B").
    // The 2nd round binary search search the second "B" in the text against
    // the 2nd char in currency names, and narrow the matching range to
    // "BB BBEX BBEXYZ" (and the maximum matching "BB").
    // The 3rd round returns the range as "BBEX BBEXYZ" (without changing
    // maximum matching).
    // The 4th round returns the same range (the maximum matching is "BBEX").
    // The 5th round returns no matching range.
    ಗಾಗಿ (int32_t ಸೂಚ್ಯಂಕ = 0; ಸೂಚ್ಯಂಕ < ಪಠ್ಯ_Len; ++ಸೂಚ್ಯಂಕ) {
        // matchIndex saves the one with exact match till the current point.
        // [binarySearchBegin, binarySearchEnd] saves the matching range.
        ಹೊಂದಿಕೆ_ಸೂಚ್ಯಂಕ = ದ್ವಿಮಾನ_ಹುಡುಕು(currency_ಹೆಸರುಗಳು, ಸೂಚ್ಯಂಕ,
                                  ಪಠ್ಯ[ಸೂಚ್ಯಂಕ],
                                  &ದ್ವಿಮಾನ_ಹುಡುಕು_Begin, &ದ್ವಿಮಾನ_ಹುಡುಕು_ಕೊನೆ);
        ಆದರೆ (ದ್ವಿಮಾನ_ಹುಡುಕು_Begin == -1) { // did not find the range
            ನಿಲ್ಲಿಸು;
        }
        *partial_ಹೊಂದಿಕೆ_Len = MAX(*partial_ಹೊಂದಿಕೆ_Len, ಸೂಚ್ಯಂಕ + 1);
        ಆದರೆ (ಹೊಂದಿಕೆ_ಸೂಚ್ಯಂಕ != -1) { 
            // find an exact match for text from text[0] to text[index] 
            // in currencyNames array.
            *max_ಹೊಂದಿಕೆ_Len = ಸೂಚ್ಯಂಕ + 1;
            *max_ಹೊಂದಿಕೆ_ಸೂಚ್ಯಂಕ = ಹೊಂದಿಕೆ_ಸೂಚ್ಯಂಕ;
        }
        ಆದರೆ (ದ್ವಿಮಾನ_ಹುಡುಕು_ಕೊನೆ - ದ್ವಿಮಾನ_ಹುಡುಕು_Begin < ರೇಖೀಯ_ಹುಡುಕು_ಮಿತಿ) {
            // linear search if within threshold.
            ರೇಖೀಯ_ಹುಡುಕು(currency_ಹೆಸರುಗಳು, ದ್ವಿಮಾನ_ಹುಡುಕು_Begin, ದ್ವಿಮಾನ_ಹುಡುಕು_ಕೊನೆ,
                         ಪಠ್ಯ, ಪಠ್ಯ_Len,
                         partial_ಹೊಂದಿಕೆ_Len,
                         max_ಹೊಂದಿಕೆ_Len, max_ಹೊಂದಿಕೆ_ಸೂಚ್ಯಂಕ);
            ನಿಲ್ಲಿಸು;
        }
    }
}

//========================= currency name cache =====================
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ಅಕ್ಷರ locale[ULOC_FULLNAME_ಸಾಮರ್ಥ್ಯ];  //key
    // currency names, case insensitive
    CurrencyNameStruct* currency_ಹೆಸರುಗಳು;  //ಮೌಲ್ಯ
    int32_t ಒಟ್ಟು_Currency_Name_ಎಣಿಕೆ;  // currency name count
    // currency symbols and ISO code, case sensitive
    CurrencyNameStruct* currencySymbols; //ಮೌಲ್ಯ
    int32_t ಒಟ್ಟು_Currency_ಚಿಹ್ನೆ_ಎಣಿಕೆ;  //ಎಣಿಕೆ
    // reference count.
    // reference count is set to 1 when an entry is put to cache.
    // it increases by 1 before accessing, and decreased by 1 after accessing.
    // The entry is deleted when ref count is zero, which means 
    // the entry is replaced out of cache and no process is accessing it.
    int32_t ref_ಎಣಿಕೆ;
} Currency_Name_Cache_ನಮೂದು;


#ವ್ಯಾಖ್ಯಾನಿಸು CURRENCY_NAME_CACHE_ಅಂಕೆ 10

// Reserve 10 cache entries.
ಸ್ಥಿರ Currency_Name_Cache_ನಮೂದು* currCache[CURRENCY_NAME_CACHE_ಅಂಕೆ] = {ಶೂನ್ಯಸೂಚಕ};
// Using an index to indicate which entry to be replaced when cache is full.
// It is a simple round-robin replacement strategy.
ಸ್ಥಿರ int8_t current_Cache_ನಮೂದು_ಸೂಚ್ಯಂಕ = 0;

ಸ್ಥಿರ UMutex gCurrencyCacheMutex;

// Cache deletion
ಸ್ಥಿರ ಶೂನ್ಯ
delete_Currency_ಹೆಸರುಗಳು(CurrencyNameStruct* currency_ಹೆಸರುಗಳು, int32_t count) {
    ಗಾಗಿ (int32_t ಸೂಚ್ಯಂಕ = 0; ಸೂಚ್ಯಂಕ < count; ++ಸೂಚ್ಯಂಕ) {
        ಆದರೆ ( (currency_ಹೆಸರುಗಳು[ಸೂಚ್ಯಂಕ].flag & NEED_ಗಮ್ಯ_BE_DELETED) ) {
            uprv_free(currency_ಹೆಸರುಗಳು[ಸೂಚ್ಯಂಕ].currencyName);
        }
    }
    uprv_free(currency_ಹೆಸರುಗಳು);
}


ಸ್ಥಿರ ಶೂನ್ಯ
delete_Cache_ನಮೂದು(Currency_Name_Cache_ನಮೂದು* ನಮೂದು) {
    delete_Currency_ಹೆಸರುಗಳು(ನಮೂದು->currency_ಹೆಸರುಗಳು, ನಮೂದು->ಒಟ್ಟು_Currency_Name_ಎಣಿಕೆ);
    delete_Currency_ಹೆಸರುಗಳು(ನಮೂದು->currencySymbols, ನಮೂದು->ಒಟ್ಟು_Currency_ಚಿಹ್ನೆ_ಎಣಿಕೆ);
    uprv_free(ನಮೂದು);
}


// Cache clean up
ಸ್ಥಿರ UBool U_CALLCONV
currency_cache_cleanup() {
    ಗಾಗಿ (int32_t i = 0; i < CURRENCY_NAME_CACHE_ಅಂಕೆ; ++i) {
        ಆದರೆ (currCache[i]) {
            delete_Cache_ನಮೂದು(currCache[i]);
            currCache[i] = ಶೂನ್ಯಸೂಚಕ;
        }
    }
    ಹಿಂತಿರುಗಿ ನಿಜ;
}


/**
 * Loads the currency name data from the cache, or from resource bundles if necessary.
 * The refCount is automatically incremented.  It is the caller's responsibility
 * to decrement it when done!
 */
ಸ್ಥಿರ Currency_Name_Cache_ನಮೂದು*
get_Cache_ನಮೂದು(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* locale, UError_ಸಂಕೇತ& ec) {

    int32_t ಒಟ್ಟು_currency_name_ಎಣಿಕೆ = 0;
    CurrencyNameStruct* currency_ಹೆಸರುಗಳು = ಶೂನ್ಯಸೂಚಕ;
    int32_t ಒಟ್ಟು_currency_ಚಿಹ್ನೆ_ಎಣಿಕೆ = 0;
    CurrencyNameStruct* currencySymbols = ಶೂನ್ಯಸೂಚಕ;
    Currency_Name_Cache_ನಮೂದು* cache_ನಮೂದು = ಶೂನ್ಯಸೂಚಕ;

    umtx_ಬೀಗ(&gCurrencyCacheMutex);
    // in order to handle racing correctly,
    // not putting 'search' in a separate function.
    int8_t ಸಿಕ್ಕಿತು = -1;
    ಗಾಗಿ (int8_t i = 0; i < CURRENCY_NAME_CACHE_ಅಂಕೆ; ++i) {
        ಆದರೆ (currCache[i]!= ಶೂನ್ಯಸೂಚಕ &&
            uprv_strcmp(locale, currCache[i]->locale) == 0) {
            ಸಿಕ್ಕಿತು = i;
            ನಿಲ್ಲಿಸು;
        }
    }
    ಆದರೆ (ಸಿಕ್ಕಿತು != -1) {
        cache_ನಮೂದು = currCache[ಸಿಕ್ಕಿತು];
        ++(cache_ನಮೂದು->ref_ಎಣಿಕೆ);
    }
    umtx_unlock(&gCurrencyCacheMutex);
    ಆದರೆ (ಸಿಕ್ಕಿತು == -1) {
        collect_Currency_ಹೆಸರುಗಳು(locale, &currency_ಹೆಸರುಗಳು, &ಒಟ್ಟು_currency_name_ಎಣಿಕೆ, &currencySymbols, &ಒಟ್ಟು_currency_ಚಿಹ್ನೆ_ಎಣಿಕೆ, ec);
        ಆದರೆ (U_FAILURE(ec)) {
            ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
        }
        umtx_ಬೀಗ(&gCurrencyCacheMutex);
        // check again.
        ಗಾಗಿ (int8_t i = 0; i < CURRENCY_NAME_CACHE_ಅಂಕೆ; ++i) {
            ಆದರೆ (currCache[i]!= ಶೂನ್ಯಸೂಚಕ &&
                uprv_strcmp(locale, currCache[i]->locale) == 0) {
                ಸಿಕ್ಕಿತು = i;
                ನಿಲ್ಲಿಸು;
            }
        }
        ಆದರೆ (ಸಿಕ್ಕಿತು == -1) {
            // insert new entry to 
            // currentCacheEntryIndex % CURRENCY_NAME_CACHE_NUM
            // and remove the existing entry 
            // currentCacheEntryIndex % CURRENCY_NAME_CACHE_NUM
            // from cache.
            cache_ನಮೂದು = currCache[current_Cache_ನಮೂದು_ಸೂಚ್ಯಂಕ];
            ಆದರೆ (cache_ನಮೂದು) {
                --(cache_ನಮೂದು->ref_ಎಣಿಕೆ);
                // delete if the ref count is zero
                ಆದರೆ (cache_ನಮೂದು->ref_ಎಣಿಕೆ == 0) {
                    delete_Cache_ನಮೂದು(cache_ನಮೂದು);
                }
            }
            cache_ನಮೂದು = static_cast<Currency_Name_Cache_ನಮೂದು*>(uprv_malloc(ಗಾತ್ರ(Currency_Name_Cache_ನಮೂದು)));
            ಆದರೆ (cache_ನಮೂದು == ಶೂನ್ಯಸೂಚಕ) {
                delete_Currency_ಹೆಸರುಗಳು(currency_ಹೆಸರುಗಳು, ಒಟ್ಟು_currency_name_ಎಣಿಕೆ);
                delete_Currency_ಹೆಸರುಗಳು(currencySymbols, ಒಟ್ಟು_currency_ಚಿಹ್ನೆ_ಎಣಿಕೆ);
                ec = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
                ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
            }
            currCache[current_Cache_ನಮೂದು_ಸೂಚ್ಯಂಕ] = cache_ನಮೂದು;
            uprv_strcpy(cache_ನಮೂದು->locale, locale);
            cache_ನಮೂದು->currency_ಹೆಸರುಗಳು = currency_ಹೆಸರುಗಳು;
            cache_ನಮೂದು->ಒಟ್ಟು_Currency_Name_ಎಣಿಕೆ = ಒಟ್ಟು_currency_name_ಎಣಿಕೆ;
            cache_ನಮೂದು->currencySymbols = currencySymbols;
            cache_ನಮೂದು->ಒಟ್ಟು_Currency_ಚಿಹ್ನೆ_ಎಣಿಕೆ = ಒಟ್ಟು_currency_ಚಿಹ್ನೆ_ಎಣಿಕೆ;
            cache_ನಮೂದು->ref_ಎಣಿಕೆ = 2; // one for cache, one for reference
            current_Cache_ನಮೂದು_ಸೂಚ್ಯಂಕ = (current_Cache_ನಮೂದು_ಸೂಚ್ಯಂಕ + 1) % CURRENCY_NAME_CACHE_ಅಂಕೆ;
            ucln_common_ರಿಜಿಸ್ಟರ್_Cleanup(UCLN_COMMON_CURRENCY, currency_cleanup);
        } ಇಲ್ಲದಿದ್ದರೆ {
            delete_Currency_ಹೆಸರುಗಳು(currency_ಹೆಸರುಗಳು, ಒಟ್ಟು_currency_name_ಎಣಿಕೆ);
            delete_Currency_ಹೆಸರುಗಳು(currencySymbols, ಒಟ್ಟು_currency_ಚಿಹ್ನೆ_ಎಣಿಕೆ);
            cache_ನಮೂದು = currCache[ಸಿಕ್ಕಿತು];
            ++(cache_ನಮೂದು->ref_ಎಣಿಕೆ);
        }
        umtx_unlock(&gCurrencyCacheMutex);
    }

    ಹಿಂತಿರುಗಿ cache_ನಮೂದು;
}

ಸ್ಥಿರ ಶೂನ್ಯ release_Cache_ನಮೂದು(Currency_Name_Cache_ನಮೂದು* cache_ನಮೂದು) {
    umtx_ಬೀಗ(&gCurrencyCacheMutex);
    --(cache_ನಮೂದು->ref_ಎಣಿಕೆ);
    ಆದರೆ (cache_ನಮೂದು->ref_ಎಣಿಕೆ == 0) {  // remove
        delete_Cache_ನಮೂದು(cache_ನಮೂದು);
    }
    umtx_unlock(&gCurrencyCacheMutex);
}

U_CAPI ಶೂನ್ಯ
uprv_parseCurrency(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* locale,
                   ಸ್ಥಿರಾಂಕ icu::UnicodeString& ಪಠ್ಯ,
                   icu::ParsePosition& pos,
                   int8_t type,
                   int32_t* partial_ಹೊಂದಿಕೆ_Len,
                   char16_t* ಫಲಿತಾಂಶ,
                   UError_ಸಂಕೇತ& ec) {
    U_NAMESPACE_USE
    ಆದರೆ (U_FAILURE(ec)) {
        ಹಿಂತಿರುಗಿ;
    }
    Currency_Name_Cache_ನಮೂದು* cache_ನಮೂದು = get_Cache_ನಮೂದು(locale, ec);
    ಆದರೆ (U_FAILURE(ec)) {
        ಹಿಂತಿರುಗಿ;
    }

    int32_t ಒಟ್ಟು_currency_name_ಎಣಿಕೆ = cache_ನಮೂದು->ಒಟ್ಟು_Currency_Name_ಎಣಿಕೆ;
    CurrencyNameStruct* currency_ಹೆಸರುಗಳು = cache_ನಮೂದು->currency_ಹೆಸರುಗಳು;
    int32_t ಒಟ್ಟು_currency_ಚಿಹ್ನೆ_ಎಣಿಕೆ = cache_ನಮೂದು->ಒಟ್ಟು_Currency_ಚಿಹ್ನೆ_ಎಣಿಕೆ;
    CurrencyNameStruct* currencySymbols = cache_ನಮೂದು->currencySymbols;

    int32_t ಆರಂಭ = pos.get_ಸೂಚ್ಯಂಕ();

    char16_t ಒಳಹರಿವು_ಪಠ್ಯ[MAX_CURRENCY_NAME_LEN];
    char16_t ಊರ್ಧ್ವ_ಪಠ್ಯ[MAX_CURRENCY_NAME_LEN];
    int32_t ಪಠ್ಯ_Len = MIN(MAX_CURRENCY_NAME_LEN, ಪಠ್ಯ.length() - ಆರಂಭ);
    ಪಠ್ಯ.extract(ಆರಂಭ, ಪಠ್ಯ_Len, ಒಳಹರಿವು_ಪಠ್ಯ);
    UError_ಸಂಕೇತ ec1 = U_ZERO_ERROR;
    ಪಠ್ಯ_Len = u_str_ಗಮ್ಯ_ಊರ್ಧ್ವ(ಊರ್ಧ್ವ_ಪಠ್ಯ, MAX_CURRENCY_NAME_LEN, ಒಳಹರಿವು_ಪಠ್ಯ, ಪಠ್ಯ_Len, locale, &ec1);

    // Make sure partialMatchLen is initialized
    *partial_ಹೊಂದಿಕೆ_Len = 0;

    int32_t max = 0;
    int32_t ಹೊಂದಿಕೆ_ಸೂಚ್ಯಂಕ = -1;
    // case in-sensitive comparison against currency names
    ಹುಡುಕು_Currency_Name(currency_ಹೆಸರುಗಳು, ಒಟ್ಟು_currency_name_ಎಣಿಕೆ, 
                       ಊರ್ಧ್ವ_ಪಠ್ಯ, ಪಠ್ಯ_Len, partial_ಹೊಂದಿಕೆ_Len, &max, &ಹೊಂದಿಕೆ_ಸೂಚ್ಯಂಕ);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UCURR_DEBUG
    ಬರೆ("search in names, max = %d, matchIndex = %d\n", max, ಹೊಂದಿಕೆ_ಸೂಚ್ಯಂಕ);
#ಆದರೆ_ಅಂತ್ಯ

    int32_t max_ಒಳ_ಚಿಹ್ನೆ = 0;
    int32_t ಹೊಂದಿಕೆ_ಸೂಚ್ಯಂಕ_ಒಳ_ಚಿಹ್ನೆ = -1;
    ಆದರೆ (type != UCURR_LONG_NAME) {  // not name only
        // case sensitive comparison against currency symbols and ISO code.
        ಹುಡುಕು_Currency_Name(currencySymbols, ಒಟ್ಟು_currency_ಚಿಹ್ನೆ_ಎಣಿಕೆ, 
                           ಒಳಹರಿವು_ಪಠ್ಯ, ಪಠ್ಯ_Len,
                           partial_ಹೊಂದಿಕೆ_Len,
                           &max_ಒಳ_ಚಿಹ್ನೆ, &ಹೊಂದಿಕೆ_ಸೂಚ್ಯಂಕ_ಒಳ_ಚಿಹ್ನೆ);
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UCURR_DEBUG
    ಬರೆ("search in symbols, maxInSymbol = %d, matchIndexInSymbol = %d\n", max_ಒಳ_ಚಿಹ್ನೆ, ಹೊಂದಿಕೆ_ಸೂಚ್ಯಂಕ_ಒಳ_ಚಿಹ್ನೆ);
    ಆದರೆ(ಹೊಂದಿಕೆ_ಸೂಚ್ಯಂಕ_ಒಳ_ಚಿಹ್ನೆ != -1) {
      ಬರೆ("== ISO=%s\n", currencySymbols[ಹೊಂದಿಕೆ_ಸೂಚ್ಯಂಕ_ಒಳ_ಚಿಹ್ನೆ].Iso_ಸಂಕೇತ);
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (max >= max_ಒಳ_ಚಿಹ್ನೆ && ಹೊಂದಿಕೆ_ಸೂಚ್ಯಂಕ != -1) {
        u_chars_ಗಮ್ಯ_UChars(currency_ಹೆಸರುಗಳು[ಹೊಂದಿಕೆ_ಸೂಚ್ಯಂಕ].Iso_ಸಂಕೇತ, ಫಲಿತಾಂಶ, 4);
        pos.ಗಣ_ಸೂಚ್ಯಂಕ(ಆರಂಭ + max);
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (max_ಒಳ_ಚಿಹ್ನೆ >= max && ಹೊಂದಿಕೆ_ಸೂಚ್ಯಂಕ_ಒಳ_ಚಿಹ್ನೆ != -1) {
        u_chars_ಗಮ್ಯ_UChars(currencySymbols[ಹೊಂದಿಕೆ_ಸೂಚ್ಯಂಕ_ಒಳ_ಚಿಹ್ನೆ].Iso_ಸಂಕೇತ, ಫಲಿತಾಂಶ, 4);
        pos.ಗಣ_ಸೂಚ್ಯಂಕ(ಆರಂಭ + max_ಒಳ_ಚಿಹ್ನೆ);
    }

    // decrease reference count
    release_Cache_ನಮೂದು(cache_ನಮೂದು);
}

ಶೂನ್ಯ uprv_currencyLeads(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* locale, icu::Unicode_ಗಣ& ಫಲಿತಾಂಶ, UError_ಸಂಕೇತ& ec) {
    U_NAMESPACE_USE
    ಆದರೆ (U_FAILURE(ec)) {
        ಹಿಂತಿರುಗಿ;
    }
    Currency_Name_Cache_ನಮೂದು* cache_ನಮೂದು = get_Cache_ನಮೂದು(locale, ec);
    ಆದರೆ (U_FAILURE(ec)) {
        ಹಿಂತಿರುಗಿ;
    }

    ಗಾಗಿ (int32_t i=0; i<cache_ನಮೂದು->ಒಟ್ಟು_Currency_ಚಿಹ್ನೆ_ಎಣಿಕೆ; i++) {
        ಸ್ಥಿರಾಂಕ CurrencyNameStruct& info = cache_ನಮೂದು->currencySymbols[i];
        UChar32 cp;
        U16_GET(info.currencyName, 0, 0, info.currencyNameLen, cp);
        ಫಲಿತಾಂಶ.ಕೂಡಿಸು(cp);
    }

    ಗಾಗಿ (int32_t i=0; i<cache_ನಮೂದು->ಒಟ್ಟು_Currency_Name_ಎಣಿಕೆ; i++) {
        ಸ್ಥಿರಾಂಕ CurrencyNameStruct& info = cache_ನಮೂದು->currency_ಹೆಸರುಗಳು[i];
        UChar32 cp;
        U16_GET(info.currencyName, 0, 0, info.currencyNameLen, cp);
        ಫಲಿತಾಂಶ.ಕೂಡಿಸು(cp);
    }

    // decrease reference count
    release_Cache_ನಮೂದು(cache_ನಮೂದು);
}


/**
 * Internal method.  Given a currency ISO code and a locale, return
 * the "static" currency name.  This is usually the same as the
 * UCURR_SYMBOL_NAME, but if the latter is a choice format, then the
 * format is applied to the number 2.0 (to yield the more common
 * plural) to return a static name.
 *
 * This is used for backward compatibility with old currency logic in
 * DecimalFormat and DecimalFormatSymbols.
 */
U_CAPI ಶೂನ್ಯ
uprv_getStaticCurrencyName(ಸ್ಥಿರಾಂಕ char16_t* iso, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* loc,
                           icu::UnicodeString& ಫಲಿತಾಂಶ, UError_ಸಂಕೇತ& ec)
{
    U_NAMESPACE_USE

    int32_t len;
    ಸ್ಥಿರಾಂಕ char16_t* currname = ucurr_getName(iso, loc, UCURR_ಚಿಹ್ನೆ_NAME,
                                          ಶೂನ್ಯಸೂಚಕ /* isChoiceFormat */, &len, &ec);
    ಆದರೆ (U_SUCCESS(ec)) {
        ಫಲಿತಾಂಶ.ಗಣ_ಗಮ್ಯ(currname, len);
    }
}

U_CAPI int32_t U_EXPORT2
ucurr_getDefaultFractionDigits(ಸ್ಥಿರಾಂಕ char16_t* currency, UError_ಸಂಕೇತ* ec) {
    ಹಿಂತಿರುಗಿ ucurr_getDefaultFractionDigitsForUsage(currency,UCURR_USAGE_STANDARD,ec);
}

U_CAPI int32_t U_EXPORT2
ucurr_getDefaultFractionDigitsForUsage(ಸ್ಥಿರಾಂಕ char16_t* currency, ಸ್ಥಿರಾಂಕ UCurrencyUsage usage, UError_ಸಂಕೇತ* ec) {
    int32_t fracDigits = 0;
    ಆದರೆ (U_SUCCESS(*ec)) {
        ಆಯ್ಕೆ (usage) {
            ಸ್ಥಿತಿ UCURR_USAGE_STANDARD:
                fracDigits = (ಕಂಡುಹಿಡಿ_Meta_ದತ್ತಾಂಶ(currency, *ec))[0];
                ನಿಲ್ಲಿಸು;
            ಸ್ಥಿತಿ UCURR_USAGE_CASH:
                fracDigits = (ಕಂಡುಹಿಡಿ_Meta_ದತ್ತಾಂಶ(currency, *ec))[2];
                ನಿಲ್ಲಿಸು;
            ಪೂರ್ವನಿಯೋಜಿತ:
                *ec = U_UNSUPPORTED_ERROR;
        }
    }
    ಹಿಂತಿರುಗಿ fracDigits;
}

U_CAPI ದ್ವಿಗುಣ U_EXPORT2
ucurr_getRoundingIncrement(ಸ್ಥಿರಾಂಕ char16_t* currency, UError_ಸಂಕೇತ* ec) {
    ಹಿಂತಿರುಗಿ ucurr_getRoundingIncrementForUsage(currency, UCURR_USAGE_STANDARD, ec);
}

U_CAPI ದ್ವಿಗುಣ U_EXPORT2
ucurr_getRoundingIncrementForUsage(ಸ್ಥಿರಾಂಕ char16_t* currency, ಸ್ಥಿರಾಂಕ UCurrencyUsage usage, UError_ಸಂಕೇತ* ec) {
    ದ್ವಿಗುಣ ಫಲಿತಾಂಶ = 0.0;

    ಸ್ಥಿರಾಂಕ int32_t *ದತ್ತಾಂಶ = ಕಂಡುಹಿಡಿ_Meta_ದತ್ತಾಂಶ(currency, *ec);
    ಆದರೆ (U_SUCCESS(*ec)) {
        int32_t fracDigits;
        int32_t increment;
        ಆಯ್ಕೆ (usage) {
            ಸ್ಥಿತಿ UCURR_USAGE_STANDARD:
                fracDigits = ದತ್ತಾಂಶ[0];
                increment = ದತ್ತಾಂಶ[1];
                ನಿಲ್ಲಿಸು;
            ಸ್ಥಿತಿ UCURR_USAGE_CASH:
                fracDigits = ದತ್ತಾಂಶ[2];
                increment = ದತ್ತಾಂಶ[3];
                ನಿಲ್ಲಿಸು;
            ಪೂರ್ವನಿಯೋಜಿತ:
                *ec = U_UNSUPPORTED_ERROR;
                ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
        }

        // If the meta data is invalid, return 0.0
        ಆದರೆ (fracDigits < 0 || fracDigits > MAX_POW10) {
            *ec = U_INVALID_FORMAT_ERROR;
        } ಇಲ್ಲದಿದ್ದರೆ {
            // A rounding value of 0 or 1 indicates no rounding.
            ಆದರೆ (increment >= 2) {
                // Return (increment) / 10^(fracDigits).  The only actual rounding data,
                // as of this writing, is CHF { 2, 5 }.
                ಫಲಿತಾಂಶ = ದ್ವಿಗುಣ(increment) / POW10[fracDigits];
            }
        }
    }

    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

U_CDECL_BEGIN

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ UCurrencyContext {
    uint32_t currType; /* UCurrCurrencyType */
    uint32_t listIdx;
} UCurrencyContext;

/*
Please keep this list in alphabetical order.
You can look at the CLDR supplemental data or ISO-4217 for the meaning of some
of these items.
ISO-4217: http://www.iso.org/iso/en/prods-services/popstds/currencycodeslist.html
*/
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ CurrencyList {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *currency;
    uint32_t currType;
} gCurrencyList[] = {
    {"ADP", UCURR_COMMON|UCURR_DEPRECATED},
    {"AED", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"AFA", UCURR_COMMON|UCURR_DEPRECATED},
    {"AFN", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"ALK", UCURR_COMMON|UCURR_DEPRECATED},
    {"ALL", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"AMD", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"ANG", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"AOA", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"AOK", UCURR_COMMON|UCURR_DEPRECATED},
    {"AON", UCURR_COMMON|UCURR_DEPRECATED},
    {"AOR", UCURR_COMMON|UCURR_DEPRECATED},
    {"ARA", UCURR_COMMON|UCURR_DEPRECATED},
    {"ARL", UCURR_COMMON|UCURR_DEPRECATED},
    {"ARM", UCURR_COMMON|UCURR_DEPRECATED},
    {"ARP", UCURR_COMMON|UCURR_DEPRECATED},
    {"ARS", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"ATS", UCURR_COMMON|UCURR_DEPRECATED},
    {"AUD", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"AWG", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"AZM", UCURR_COMMON|UCURR_DEPRECATED},
    {"AZN", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"BAD", UCURR_COMMON|UCURR_DEPRECATED},
    {"BAM", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"BAN", UCURR_COMMON|UCURR_DEPRECATED},
    {"BBD", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"BDT", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"BEC", UCURR_UNCOMMON|UCURR_DEPRECATED},
    {"BEF", UCURR_COMMON|UCURR_DEPRECATED},
    {"BEL", UCURR_UNCOMMON|UCURR_DEPRECATED},
    {"BGL", UCURR_COMMON|UCURR_DEPRECATED},
    {"BGM", UCURR_COMMON|UCURR_DEPRECATED},
    {"BGN", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"BGO", UCURR_COMMON|UCURR_DEPRECATED},
    {"BHD", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"BIF", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"BMD", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"BND", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"BOB", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"BOL", UCURR_COMMON|UCURR_DEPRECATED},
    {"BOP", UCURR_COMMON|UCURR_DEPRECATED},
    {"BOV", UCURR_UNCOMMON|UCURR_NON_DEPRECATED},
    {"BRB", UCURR_COMMON|UCURR_DEPRECATED},
    {"BRC", UCURR_COMMON|UCURR_DEPRECATED},
    {"BRE", UCURR_COMMON|UCURR_DEPRECATED},
    {"BRL", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"BRN", UCURR_COMMON|UCURR_DEPRECATED},
    {"BRR", UCURR_COMMON|UCURR_DEPRECATED},
    {"BRZ", UCURR_COMMON|UCURR_DEPRECATED},
    {"BSD", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"BTN", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"BUK", UCURR_COMMON|UCURR_DEPRECATED},
    {"BWP", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"BYB", UCURR_COMMON|UCURR_DEPRECATED},
    {"BYN", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"BYR", UCURR_COMMON|UCURR_DEPRECATED},
    {"BZD", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"CAD", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"CDF", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"CHE", UCURR_UNCOMMON|UCURR_NON_DEPRECATED},
    {"CHF", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"CHW", UCURR_UNCOMMON|UCURR_NON_DEPRECATED},
    {"CLE", UCURR_COMMON|UCURR_DEPRECATED},
    {"CLF", UCURR_UNCOMMON|UCURR_NON_DEPRECATED},
    {"CLP", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"CNH", UCURR_UNCOMMON|UCURR_NON_DEPRECATED},
    {"CNX", UCURR_UNCOMMON|UCURR_DEPRECATED},
    {"CNY", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"COP", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"COU", UCURR_UNCOMMON|UCURR_NON_DEPRECATED},
    {"CRC", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"CSD", UCURR_COMMON|UCURR_DEPRECATED},
    {"CSK", UCURR_COMMON|UCURR_DEPRECATED},
    {"CUC", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"CUP", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"CVE", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"CYP", UCURR_COMMON|UCURR_DEPRECATED},
    {"CZK", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"DDM", UCURR_COMMON|UCURR_DEPRECATED},
    {"DEM", UCURR_COMMON|UCURR_DEPRECATED},
    {"DJF", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"DKK", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"DOP", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"DZD", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"ECS", UCURR_COMMON|UCURR_DEPRECATED},
    {"ECV", UCURR_UNCOMMON|UCURR_DEPRECATED},
    {"EEK", UCURR_COMMON|UCURR_DEPRECATED},
    {"EGP", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"ERN", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"ESA", UCURR_UNCOMMON|UCURR_DEPRECATED},
    {"ESB", UCURR_UNCOMMON|UCURR_DEPRECATED},
    {"ESP", UCURR_COMMON|UCURR_DEPRECATED},
    {"ETB", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"EUR", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"FIM", UCURR_COMMON|UCURR_DEPRECATED},
    {"FJD", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"FKP", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"FRF", UCURR_COMMON|UCURR_DEPRECATED},
    {"GBP", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"GEK", UCURR_COMMON|UCURR_DEPRECATED},
    {"GEL", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"GHC", UCURR_COMMON|UCURR_DEPRECATED},
    {"GHS", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"GIP", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"GMD", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"GNF", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"GNS", UCURR_COMMON|UCURR_DEPRECATED},
    {"GQE", UCURR_COMMON|UCURR_DEPRECATED},
    {"GRD", UCURR_COMMON|UCURR_DEPRECATED},
    {"GTQ", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"GWE", UCURR_COMMON|UCURR_DEPRECATED},
    {"GWP", UCURR_COMMON|UCURR_DEPRECATED},
    {"GYD", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"HKD", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"HNL", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"HRD", UCURR_COMMON|UCURR_DEPRECATED},
    {"HRK", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"HTG", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"HUF", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"IDR", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"IEP", UCURR_COMMON|UCURR_DEPRECATED},
    {"ILP", UCURR_COMMON|UCURR_DEPRECATED},
    {"ILR", UCURR_COMMON|UCURR_DEPRECATED},
    {"ILS", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"INR", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"IQD", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"IRR", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"ISJ", UCURR_COMMON|UCURR_DEPRECATED},
    {"ISK", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"ITL", UCURR_COMMON|UCURR_DEPRECATED},
    {"JMD", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"JOD", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"JPY", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"KES", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"KGS", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"KHR", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"KMF", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"KPW", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"KRH", UCURR_COMMON|UCURR_DEPRECATED},
    {"KRO", UCURR_COMMON|UCURR_DEPRECATED},
    {"KRW", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"KWD", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"KYD", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"KZT", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"LAK", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"LBP", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"LKR", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"LRD", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"LSL", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"LSM", UCURR_COMMON|UCURR_DEPRECATED}, // questionable, remove?
    {"LTL", UCURR_COMMON|UCURR_DEPRECATED},
    {"LTT", UCURR_COMMON|UCURR_DEPRECATED},
    {"LUC", UCURR_UNCOMMON|UCURR_DEPRECATED},
    {"LUF", UCURR_COMMON|UCURR_DEPRECATED},
    {"LUL", UCURR_UNCOMMON|UCURR_DEPRECATED},
    {"LVL", UCURR_COMMON|UCURR_DEPRECATED},
    {"LVR", UCURR_COMMON|UCURR_DEPRECATED},
    {"LYD", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"MAD", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"MAF", UCURR_COMMON|UCURR_DEPRECATED},
    {"MCF", UCURR_COMMON|UCURR_DEPRECATED},
    {"MDC", UCURR_COMMON|UCURR_DEPRECATED},
    {"MDL", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"MGA", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"MGF", UCURR_COMMON|UCURR_DEPRECATED},
    {"MKD", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"MKN", UCURR_COMMON|UCURR_DEPRECATED},
    {"MLF", UCURR_COMMON|UCURR_DEPRECATED},
    {"MMK", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"MNT", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"MOP", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"MRO", UCURR_COMMON|UCURR_DEPRECATED},
    {"MRU", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"MTL", UCURR_COMMON|UCURR_DEPRECATED},
    {"MTP", UCURR_COMMON|UCURR_DEPRECATED},
    {"MUR", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"MVP", UCURR_COMMON|UCURR_DEPRECATED}, // questionable, remove?
    {"MVR", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"MWK", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"MXN", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"MXP", UCURR_COMMON|UCURR_DEPRECATED},
    {"MXV", UCURR_UNCOMMON|UCURR_NON_DEPRECATED},
    {"MYR", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"MZE", UCURR_COMMON|UCURR_DEPRECATED},
    {"MZM", UCURR_COMMON|UCURR_DEPRECATED},
    {"MZN", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"NAD", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"NGN", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"NIC", UCURR_COMMON|UCURR_DEPRECATED},
    {"NIO", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"NLG", UCURR_COMMON|UCURR_DEPRECATED},
    {"NOK", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"NPR", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"NZD", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"OMR", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"PAB", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"PEI", UCURR_COMMON|UCURR_DEPRECATED},
    {"PEN", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"PES", UCURR_COMMON|UCURR_DEPRECATED},
    {"PGK", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"PHP", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"PKR", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"PLN", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"PLZ", UCURR_COMMON|UCURR_DEPRECATED},
    {"PTE", UCURR_COMMON|UCURR_DEPRECATED},
    {"PYG", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"QAR", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"RHD", UCURR_COMMON|UCURR_DEPRECATED},
    {"ROL", UCURR_COMMON|UCURR_DEPRECATED},
    {"RON", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"RSD", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"RUB", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"RUR", UCURR_COMMON|UCURR_DEPRECATED},
    {"RWF", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"SAR", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"SBD", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"SCR", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"SDD", UCURR_COMMON|UCURR_DEPRECATED},
    {"SDG", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"SDP", UCURR_COMMON|UCURR_DEPRECATED},
    {"SEK", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"SGD", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"SHP", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"SIT", UCURR_COMMON|UCURR_DEPRECATED},
    {"SKK", UCURR_COMMON|UCURR_DEPRECATED},
    {"SLE", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"SLL", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"SOS", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"SRD", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"SRG", UCURR_COMMON|UCURR_DEPRECATED},
    {"SSP", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"STD", UCURR_COMMON|UCURR_DEPRECATED},
    {"STN", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"SUR", UCURR_COMMON|UCURR_DEPRECATED},
    {"SVC", UCURR_COMMON|UCURR_DEPRECATED},
    {"SYP", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"SZL", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"THB", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"TJR", UCURR_COMMON|UCURR_DEPRECATED},
    {"TJS", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"TMM", UCURR_COMMON|UCURR_DEPRECATED},
    {"TMT", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"TND", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"TOP", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"TPE", UCURR_COMMON|UCURR_DEPRECATED},
    {"TRL", UCURR_COMMON|UCURR_DEPRECATED},
    {"TRY", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"TTD", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"TWD", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"TZS", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"UAH", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"UAK", UCURR_COMMON|UCURR_DEPRECATED},
    {"UGS", UCURR_COMMON|UCURR_DEPRECATED},
    {"UGX", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"USD", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"USN", UCURR_UNCOMMON|UCURR_NON_DEPRECATED},
    {"USS", UCURR_UNCOMMON|UCURR_NON_DEPRECATED},
    {"UYI", UCURR_UNCOMMON|UCURR_NON_DEPRECATED},
    {"UYP", UCURR_COMMON|UCURR_DEPRECATED},
    {"UYU", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"UYW", UCURR_UNCOMMON|UCURR_NON_DEPRECATED},
    {"UZS", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"VEB", UCURR_COMMON|UCURR_DEPRECATED},
    {"VED", UCURR_UNCOMMON|UCURR_NON_DEPRECATED},
    {"VEF", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"VES", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"VND", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"VNN", UCURR_COMMON|UCURR_DEPRECATED},
    {"VUV", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"WST", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"XAF", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"XAG", UCURR_UNCOMMON|UCURR_NON_DEPRECATED},
    {"XAU", UCURR_UNCOMMON|UCURR_NON_DEPRECATED},
    {"XBA", UCURR_UNCOMMON|UCURR_NON_DEPRECATED},
    {"XBB", UCURR_UNCOMMON|UCURR_NON_DEPRECATED},
    {"XBC", UCURR_UNCOMMON|UCURR_NON_DEPRECATED},
    {"XBD", UCURR_UNCOMMON|UCURR_NON_DEPRECATED},
    {"XCD", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"XCG", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"XDR", UCURR_UNCOMMON|UCURR_NON_DEPRECATED},
    {"XEU", UCURR_UNCOMMON|UCURR_DEPRECATED},
    {"XFO", UCURR_UNCOMMON|UCURR_NON_DEPRECATED},
    {"XFU", UCURR_UNCOMMON|UCURR_NON_DEPRECATED},
    {"XOF", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"XPD", UCURR_UNCOMMON|UCURR_NON_DEPRECATED},
    {"XPF", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"XPT", UCURR_UNCOMMON|UCURR_NON_DEPRECATED},
    {"XRE", UCURR_UNCOMMON|UCURR_DEPRECATED},
    {"XSU", UCURR_UNCOMMON|UCURR_NON_DEPRECATED},
    {"XTS", UCURR_UNCOMMON|UCURR_NON_DEPRECATED},
    {"XUA", UCURR_UNCOMMON|UCURR_NON_DEPRECATED},
    {"XXX", UCURR_UNCOMMON|UCURR_NON_DEPRECATED},
    {"YDD", UCURR_COMMON|UCURR_DEPRECATED},
    {"YER", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"YUD", UCURR_COMMON|UCURR_DEPRECATED},
    {"YUM", UCURR_COMMON|UCURR_DEPRECATED},
    {"YUN", UCURR_COMMON|UCURR_DEPRECATED},
    {"YUR", UCURR_COMMON|UCURR_DEPRECATED},
    {"ZAL", UCURR_UNCOMMON|UCURR_DEPRECATED},
    {"ZAR", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"ZMK", UCURR_COMMON|UCURR_DEPRECATED},
    {"ZMW", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"ZRN", UCURR_COMMON|UCURR_DEPRECATED},
    {"ZRZ", UCURR_COMMON|UCURR_DEPRECATED},
    {"ZWD", UCURR_COMMON|UCURR_DEPRECATED},
    {"ZWG", UCURR_COMMON|UCURR_NON_DEPRECATED},
    {"ZWL", UCURR_COMMON|UCURR_DEPRECATED},
    {"ZWR", UCURR_COMMON|UCURR_DEPRECATED},
    { ಶೂನ್ಯಸೂಚಕ, 0 } // Leave here to denote the end of the list.
};

#ವ್ಯಾಖ್ಯಾನಿಸು UCURR_MATCHES_BITMASK(variable, type_ಗಮ್ಯ_ಹೊಂದಿಕೆ) \
    ((type_ಗಮ್ಯ_ಹೊಂದಿಕೆ) == UCURR_ALL || ((variable) & (type_ಗಮ್ಯ_ಹೊಂದಿಕೆ)) == (type_ಗಮ್ಯ_ಹೊಂದಿಕೆ))

ಸ್ಥಿರ int32_t U_CALLCONV
ucurr_ಎಣಿಕೆ_Currency_List(UEnumeration *enumerator, UError_ಸಂಕೇತ * /*pErrorCode*/) {
    UCurrencyContext *myContext = (UCurrencyContext *)(enumerator->context);
    uint32_t currType = myContext->currType;
    int32_t count = 0;

    /* Count the number of items matching the type we are looking for. */
    ಗಾಗಿ (int32_t idx = 0; gCurrencyList[idx].currency != ಶೂನ್ಯಸೂಚಕ; idx++) {
        ಆದರೆ (UCURR_MATCHES_BITMASK(gCurrencyList[idx].currType, currType)) {
            count++;
        }
    }
    ಹಿಂತಿರುಗಿ count;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* U_CALLCONV
ucurr_ಮುಂದಿನ_Currency_List(UEnumeration *enumerator,
                        int32_t* ಫಲಿತಾಂಶ_ಉದ್ದಳತೆ,
                        UError_ಸಂಕೇತ * /*pErrorCode*/)
{
    UCurrencyContext *myContext = (UCurrencyContext *)(enumerator->context);

    /* Find the next in the list that matches the type we are looking for. */
    ಆಗ (myContext->listIdx < UPRV_LENGTHOF(gCurrencyList)-1) {
        ಸ್ಥಿರಾಂಕ ರಚನೆ CurrencyList *curr_ಅಂಶ = &gCurrencyList[myContext->listIdx++];
        ಆದರೆ (UCURR_MATCHES_BITMASK(curr_ಅಂಶ->currType, myContext->currType))
        {
            ಆದರೆ (ಫಲಿತಾಂಶ_ಉದ್ದಳತೆ) {
                *ಫಲಿತಾಂಶ_ಉದ್ದಳತೆ = 3; /* Currency codes are only 3 chars long */
            }
            ಹಿಂತಿರುಗಿ curr_ಅಂಶ->currency;
        }
    }
    /* We enumerated too far. */
    ಆದರೆ (ಫಲಿತಾಂಶ_ಉದ್ದಳತೆ) {
        *ಫಲಿತಾಂಶ_ಉದ್ದಳತೆ = 0;
    }
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
ucurr_resetCurrencyList(UEnumeration *enumerator, UError_ಸಂಕೇತ * /*pErrorCode*/) {
    ((UCurrencyContext *)(enumerator->context))->listIdx = 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
ucurr_closeCurrencyList(UEnumeration *enumerator) {
    uprv_free(enumerator->context);
    uprv_free(enumerator);
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
ucurr_createCurrencyList(UHashtable *isoCodes, UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2){
    UError_ಸಂಕೇತ local_ಸ್ಥಿತಿ = U_ZERO_ERROR;

    // Look up the CurrencyMap element in the root bundle.
    UResourceBundle *rb = ures_open_ನೇರ(U_ICUDATA_CURR, CURRENCY_ದತ್ತಾಂಶ, &local_ಸ್ಥಿತಿ);
    Local_UResource_Bundle_ಸೂಚಕ currencyMapArray(ures_get_By_ಕೀಲಿ(rb, CURRENCY_MAP, rb, &local_ಸ್ಥಿತಿ));

    ಆದರೆ (U_SUCCESS(local_ಸ್ಥಿತಿ)) {
        // process each entry in currency map
        ಗಾಗಿ (int32_t i=0; i<ures_get_ಗಾತ್ರ(currencyMapArray.getAlias()); i++) {
            // get the currency resource
            Local_UResource_Bundle_ಸೂಚಕ currencyArray(ures_get_By_ಸೂಚ್ಯಂಕ(currencyMapArray.getAlias(), i, ಶೂನ್ಯಸೂಚಕ, &local_ಸ್ಥಿತಿ));
            // process each currency
            ಆದರೆ (U_SUCCESS(local_ಸ್ಥಿತಿ)) {
                ಗಾಗಿ (int32_t j=0; j<ures_get_ಗಾತ್ರ(currencyArray.getAlias()); j++) {
                    // get the currency resource
                    Local_UResource_Bundle_ಸೂಚಕ currencyRes(ures_get_By_ಸೂಚ್ಯಂಕ(currencyArray.getAlias(), j, ಶೂನ್ಯಸೂಚಕ, &local_ಸ್ಥಿತಿ));
                    Iso_ಸಂಕೇತ_ನಮೂದು *ನಮೂದು = (Iso_ಸಂಕೇತ_ನಮೂದು*)uprv_malloc(ಗಾತ್ರ(Iso_ಸಂಕೇತ_ನಮೂದು));
                    ಆದರೆ (ನಮೂದು == ಶೂನ್ಯಸೂಚಕ) {
                        *ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
                        ಹಿಂತಿರುಗಿ;
                    }

                    // get the ISO code
                    int32_t iso_ಉದ್ದಳತೆ = 0;
                    Local_UResource_Bundle_ಸೂಚಕ idRes(ures_get_By_ಕೀಲಿ(currencyRes.getAlias(), "id", ಶೂನ್ಯಸೂಚಕ, &local_ಸ್ಥಿತಿ));
                    ಆದರೆ (idRes.isNull()) {
                        ಮುಂದುವರಿ;
                    }
                    ಸ್ಥಿರಾಂಕ char16_t *iso_ಸಂಕೇತ = ures_getString(idRes.getAlias(), &iso_ಉದ್ದಳತೆ, &local_ಸ್ಥಿತಿ);

                    // get from date
                    UDate fromDate = U_DATE_MIN;
                    Local_UResource_Bundle_ಸೂಚಕ fromRes(ures_get_By_ಕೀಲಿ(currencyRes.getAlias(), "from", ಶೂನ್ಯಸೂಚಕ, &local_ಸ್ಥಿತಿ));

                    ಆದರೆ (U_SUCCESS(local_ಸ್ಥಿತಿ)) {
                        int32_t from_ಉದ್ದಳತೆ = 0;
                        ಸ್ಥಿರಾಂಕ int32_t *fromArray = ures_get_Int_ಸದಿಶ(fromRes.getAlias(), &from_ಉದ್ದಳತೆ, &local_ಸ್ಥಿತಿ);
                        int64_t currDate64 = ((uint64_t)fromArray[0]) << 32;
                        currDate64 |= ((int64_t)fromArray[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                        fromDate = (UDate)currDate64;
                    }

                    // get to date
                    UDate ಗಮ್ಯ_Date = U_DATE_MAX;
                    local_ಸ್ಥಿತಿ = U_ZERO_ERROR;
                    Local_UResource_Bundle_ಸೂಚಕ ಗಮ್ಯ_Res(ures_get_By_ಕೀಲಿ(currencyRes.getAlias(), "to", ಶೂನ್ಯಸೂಚಕ, &local_ಸ್ಥಿತಿ));

                    ಆದರೆ (U_SUCCESS(local_ಸ್ಥಿತಿ)) {
                        int32_t ಗಮ್ಯ_ಉದ್ದಳತೆ = 0;
                        ಸ್ಥಿರಾಂಕ int32_t *ಗಮ್ಯ_Array = ures_get_Int_ಸದಿಶ(ಗಮ್ಯ_Res.getAlias(), &ಗಮ್ಯ_ಉದ್ದಳತೆ, &local_ಸ್ಥಿತಿ);
                        int64_t currDate64 = (uint64_t)ಗಮ್ಯ_Array[0] << 32;
                        currDate64 |= ((int64_t)ಗಮ್ಯ_Array[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                        ಗಮ್ಯ_Date = (UDate)currDate64;
                    }

                    ನಮೂದು->iso_ಸಂಕೇತ = iso_ಸಂಕೇತ;
                    ನಮೂದು->from = fromDate;
                    ನಮೂದು->ಗಮ್ಯ = ಗಮ್ಯ_Date;

                    local_ಸ್ಥಿತಿ = U_ZERO_ERROR;
                    uhash_put(isoCodes, (char16_t *)iso_ಸಂಕೇತ, ನಮೂದು, &local_ಸ್ಥಿತಿ);
                }
            } ಇಲ್ಲದಿದ್ದರೆ {
                *ಸ್ಥಿತಿ_2 = local_ಸ್ಥಿತಿ;
            }
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        *ಸ್ಥಿತಿ_2 = local_ಸ್ಥಿತಿ;
    }
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ UEnumeration gEnumCurrencyList = {
    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ,
    ucurr_closeCurrencyList,
    ucurr_ಎಣಿಕೆ_Currency_List,
    uenum_unextDefault,
    ucurr_ಮುಂದಿನ_Currency_List,
    ucurr_resetCurrencyList
};
U_CDECL_ಕೊನೆ


ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV initIsoCodes(UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) {
    U_ASSERT(gIsoCodes == ಶೂನ್ಯಸೂಚಕ);
    ucln_common_ರಿಜಿಸ್ಟರ್_Cleanup(UCLN_COMMON_CURRENCY, currency_cleanup);

    Local_UHashtable_ಸೂಚಕ isoCodes(uhash_open(uhash_hashUChars, uhash_compareUChars, ಶೂನ್ಯಸೂಚಕ, &ಸ್ಥಿತಿ_2));
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ;
    }
    uhash_ಗಣ_ಮೌಲ್ಯ_Deleter(isoCodes.getAlias(), delete_Iso_ಸಂಕೇತ_ನಮೂದು);

    ucurr_createCurrencyList(isoCodes.getAlias(), &ಸ್ಥಿತಿ_2);
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ;
    }
    gIsoCodes = isoCodes.orphan();  // Note: gIsoCodes is const. Once set up here it is never altered,
                                    //       and read only access is safe without synchronization.
}

ಸ್ಥಿರ ಶೂನ್ಯ populateCurrSymbolsEquiv(icu::Hashtable *hash, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) {
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) { ಹಿಂತಿರುಗಿ; }
    ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ& ನಮೂದು : unisets::kCurrencyEntries) {
        UnicodeString exemplar(ನಮೂದು.exemplar);
        ಸ್ಥಿರಾಂಕ Unicode_ಗಣ* ಗಣ = unisets::get(ನಮೂದು.ಕೀಲಿ);
        ಆದರೆ (ಗಣ == ಶೂನ್ಯಸೂಚಕ) { ಹಿಂತಿರುಗಿ; }
        Unicode_ಗಣ_Iterator it(*ಗಣ);
        ಆಗ (it.next()) {
            UnicodeString value = it.getString();
            ಆದರೆ (value == exemplar) {
                // No need to mark the exemplar character as an equivalent
                ಮುಂದುವರಿ;
            }
            makeEquivalent(exemplar, value, hash, ಸ್ಥಿತಿ_2);
            ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) { ಹಿಂತಿರುಗಿ; }
        }
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV initCurrSymbolsEquiv() {
    U_ASSERT(gCurrSymbolsEquiv == ಶೂನ್ಯಸೂಚಕ);
    UError_ಸಂಕೇತ ಸ್ಥಿತಿ_2 = U_ZERO_ERROR;
    ucln_common_ರಿಜಿಸ್ಟರ್_Cleanup(UCLN_COMMON_CURRENCY, currency_cleanup);
    icu::Hashtable *ತಾತ್ಕಾಲಿಕ = ಹೊಸ icu::Hashtable(ಸ್ಥಿತಿ_2);
    ಆದರೆ (ತಾತ್ಕಾಲಿಕ == ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಅಳಿಸು ತಾತ್ಕಾಲಿಕ;
        ಹಿಂತಿರುಗಿ;
    }
    ತಾತ್ಕಾಲಿಕ->ಗಣ_ಮೌಲ್ಯ_Deleter(deleteUnicode);
    populateCurrSymbolsEquiv(ತಾತ್ಕಾಲಿಕ, ಸ್ಥಿತಿ_2);
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಅಳಿಸು ತಾತ್ಕಾಲಿಕ;
        ಹಿಂತಿರುಗಿ;
    }
    gCurrSymbolsEquiv = ತಾತ್ಕಾಲಿಕ;
}

U_CAPI UBool U_EXPORT2
ucurr_is_ಲಭ್ಯ(ಸ್ಥಿರಾಂಕ char16_t* iso_ಸಂಕೇತ, UDate from, UDate ಗಮ್ಯ, UError_ಸಂಕೇತ* e_Error_ಸಂಕೇತ) {
    umtx_initOnce(gIsoCodesInitOnce, &initIsoCodes, *e_Error_ಸಂಕೇತ);
    ಆದರೆ (U_FAILURE(*e_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }

    Iso_ಸಂಕೇತ_ನಮೂದು* ಫಲಿತಾಂಶ = (Iso_ಸಂಕೇತ_ನಮೂದು *) uhash_get(gIsoCodes, iso_ಸಂಕೇತ);
    ಆದರೆ (ಫಲಿತಾಂಶ == ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (from > ಗಮ್ಯ) {
        *e_Error_ಸಂಕೇತ = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ  ((from > ಫಲಿತಾಂಶ->ಗಮ್ಯ) || (ಗಮ್ಯ < ಫಲಿತಾಂಶ->from)) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ icu::Hashtable* getCurrSymbolsEquiv() {
    umtx_initOnce(gCurrSymbolsEquivInitOnce, &initCurrSymbolsEquiv);
    ಹಿಂತಿರುಗಿ gCurrSymbolsEquiv;
}

U_CAPI UEnumeration * U_EXPORT2
ucurr_openISOCurrencies(uint32_t currType, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    UEnumeration *myEnum = ಶೂನ್ಯಸೂಚಕ;
    UCurrencyContext *myContext;

    myEnum = (UEnumeration*)uprv_malloc(ಗಾತ್ರ(UEnumeration));
    ಆದರೆ (myEnum == ಶೂನ್ಯಸೂಚಕ) {
        *p_Error_ಸಂಕೇತ = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    uprv_memcpy(myEnum, &gEnumCurrencyList, ಗಾತ್ರ(UEnumeration));
    myContext = (UCurrencyContext*)uprv_malloc(ಗಾತ್ರ(UCurrencyContext));
    ಆದರೆ (myContext == ಶೂನ್ಯಸೂಚಕ) {
        *p_Error_ಸಂಕೇತ = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        uprv_free(myEnum);
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    myContext->currType = currType;
    myContext->listIdx = 0;
    myEnum->context = myContext;
    ಹಿಂತಿರುಗಿ myEnum;
}

U_CAPI int32_t U_EXPORT2
ucurr_ಎಣಿಕೆ_Currencies(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* locale, 
                 UDate date, 
                 UError_ಸಂಕೇತ* ec)
{
    int32_t curr_ಎಣಿಕೆ = 0;

    ಆದರೆ (ec != ಶೂನ್ಯಸೂಚಕ && U_SUCCESS(*ec)) 
    {
        // local variables
        UError_ಸಂಕೇತ local_ಸ್ಥಿತಿ = U_ZERO_ERROR;

        // get country or country_variant in `id'
        CharString id = idForLocale(locale, ec);

        ಆದರೆ (U_FAILURE(*ec))
        {
            ಹಿಂತಿರುಗಿ 0;
        }

        // Remove variants, which is only needed for registration.
        ಅಕ್ಷರ *idDelim = strchr(id.ದತ್ತಾಂಶ(), VAR_DELIM);
        ಆದರೆ (idDelim)
        {
            id.truncate(idDelim - id.ದತ್ತಾಂಶ());
        }

        // Look up the CurrencyMap element in the root bundle.
        UResourceBundle *rb = ures_open_ನೇರ(U_ICUDATA_CURR, CURRENCY_ದತ್ತಾಂಶ, &local_ಸ್ಥಿತಿ);
        UResourceBundle *cm = ures_get_By_ಕೀಲಿ(rb, CURRENCY_MAP, rb, &local_ಸ್ಥಿತಿ);

        // Using the id derived from the local, get the currency data
        Local_UResource_Bundle_ಸೂಚಕ countryArray(ures_get_By_ಕೀಲಿ(rb, id.ದತ್ತಾಂಶ(), cm, &local_ಸ್ಥಿತಿ));

        // process each currency to see which one is valid for the given date
        ಆದರೆ (U_SUCCESS(local_ಸ್ಥಿತಿ))
        {
            ಗಾಗಿ (int32_t i=0; i<ures_get_ಗಾತ್ರ(countryArray.getAlias()); i++)
            {
                // get the currency resource
                Local_UResource_Bundle_ಸೂಚಕ currencyRes(ures_get_By_ಸೂಚ್ಯಂಕ(countryArray.getAlias(), i, ಶೂನ್ಯಸೂಚಕ, &local_ಸ್ಥಿತಿ));

                // get the from date
                int32_t from_ಉದ್ದಳತೆ = 0;
                Local_UResource_Bundle_ಸೂಚಕ fromRes(ures_get_By_ಕೀಲಿ(currencyRes.getAlias(), "from", ಶೂನ್ಯಸೂಚಕ, &local_ಸ್ಥಿತಿ));
                ಸ್ಥಿರಾಂಕ int32_t *fromArray = ures_get_Int_ಸದಿಶ(fromRes.getAlias(), &from_ಉದ್ದಳತೆ, &local_ಸ್ಥಿತಿ);

                int64_t currDate64 = (int64_t)((uint64_t)(fromArray[0]) << 32);
                currDate64 |= ((int64_t)fromArray[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                UDate fromDate = (UDate)currDate64;

                ಆದರೆ (ures_get_ಗಾತ್ರ(currencyRes.getAlias())> 2)
                {
                    int32_t ಗಮ್ಯ_ಉದ್ದಳತೆ = 0;
                    Local_UResource_Bundle_ಸೂಚಕ ಗಮ್ಯ_Res(ures_get_By_ಕೀಲಿ(currencyRes.getAlias(), "to", ಶೂನ್ಯಸೂಚಕ, &local_ಸ್ಥಿತಿ));
                    ಸ್ಥಿರಾಂಕ int32_t *ಗಮ್ಯ_Array = ures_get_Int_ಸದಿಶ(ಗಮ್ಯ_Res.getAlias(), &ಗಮ್ಯ_ಉದ್ದಳತೆ, &local_ಸ್ಥಿತಿ);

                    currDate64 = (int64_t)ಗಮ್ಯ_Array[0] << 32;
                    currDate64 |= ((int64_t)ಗಮ್ಯ_Array[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                    UDate ಗಮ್ಯ_Date = (UDate)currDate64;

                    ಆದರೆ ((fromDate <= date) && (date < ಗಮ್ಯ_Date))
                    {
                        curr_ಎಣಿಕೆ++;
                    }
                }
                ಇಲ್ಲದಿದ್ದರೆ
                {
                    ಆದರೆ (fromDate <= date)
                    {
                        curr_ಎಣಿಕೆ++;
                    }
                }
            } // end For loop
        } // end if (U_SUCCESS(localStatus))

        // Check for errors
        ಆದರೆ (*ec == U_ZERO_ERROR || local_ಸ್ಥಿತಿ != U_ZERO_ERROR)
        {
            // There is nothing to fallback to. 
            // Report the failure/warning if possible.
            *ec = local_ಸ್ಥಿತಿ;
        }

        ಆದರೆ (U_SUCCESS(*ec))
        {
            // no errors
            ಹಿಂತಿರುಗಿ curr_ಎಣಿಕೆ;
        }
    }

    // If we got here, either error code is invalid or
    // some argument passed is no good.
    ಹಿಂತಿರುಗಿ 0;
}

U_CAPI int32_t U_EXPORT2 
ucurr_forLocaleAndDate(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* locale, 
                UDate date, 
                int32_t ಸೂಚ್ಯಂಕ,
                char16_t* buff,
                int32_t buff_ಸಾಮರ್ಥ್ಯ, 
                UError_ಸಂಕೇತ* ec)
{
    int32_t resLen = 0;
	int32_t curr_ಸೂಚ್ಯಂಕ = 0;
    ಸ್ಥಿರಾಂಕ char16_t* s = ಶೂನ್ಯಸೂಚಕ;

    ಆದರೆ (ec != ಶೂನ್ಯಸೂಚಕ && U_SUCCESS(*ec))
    {
        // check the arguments passed
        ಆದರೆ ((buff && buff_ಸಾಮರ್ಥ್ಯ) || !buff_ಸಾಮರ್ಥ್ಯ )
        {
            // local variables
            UError_ಸಂಕೇತ local_ಸ್ಥಿತಿ = U_ZERO_ERROR;

            // get country or country_variant in `id'
            CharString id = idForLocale(locale, ec);
            ಆದರೆ (U_FAILURE(*ec))
            {
                ಹಿಂತಿರುಗಿ 0;
            }

            // Remove variants, which is only needed for registration.
            ಅಕ್ಷರ *idDelim = strchr(id.ದತ್ತಾಂಶ(), VAR_DELIM);
            ಆದರೆ (idDelim)
            {
                id.truncate(idDelim - id.ದತ್ತಾಂಶ());
            }

            // Look up the CurrencyMap element in the root bundle.
            UResourceBundle *rb = ures_open_ನೇರ(U_ICUDATA_CURR, CURRENCY_ದತ್ತಾಂಶ, &local_ಸ್ಥಿತಿ);
            UResourceBundle *cm = ures_get_By_ಕೀಲಿ(rb, CURRENCY_MAP, rb, &local_ಸ್ಥಿತಿ);

            // Using the id derived from the local, get the currency data
            Local_UResource_Bundle_ಸೂಚಕ countryArray(ures_get_By_ಕೀಲಿ(rb, id.ದತ್ತಾಂಶ(), cm, &local_ಸ್ಥಿತಿ));

            // process each currency to see which one is valid for the given date
            ತಾರ್ಕಿಕ ಹೊಂದಿಕೆ_ಸಿಕ್ಕಿತು = ಸುಳ್ಳು;
            ಆದರೆ (U_SUCCESS(local_ಸ್ಥಿತಿ))
            {
                ಆದರೆ ((ಸೂಚ್ಯಂಕ <= 0) || (ಸೂಚ್ಯಂಕ> ures_get_ಗಾತ್ರ(countryArray.getAlias())))
                {
                    // requested index is out of bounds
                    ಹಿಂತಿರುಗಿ 0;
                }

                ಗಾಗಿ (int32_t i=0; i<ures_get_ಗಾತ್ರ(countryArray.getAlias()); i++)
                {
                    // get the currency resource
                    Local_UResource_Bundle_ಸೂಚಕ currencyRes(ures_get_By_ಸೂಚ್ಯಂಕ(countryArray.getAlias(), i, ಶೂನ್ಯಸೂಚಕ, &local_ಸ್ಥಿತಿ));
                    s = ures_get_String_By_ಕೀಲಿ(currencyRes.getAlias(), "id", &resLen, &local_ಸ್ಥಿತಿ);

                    // get the from date
                    int32_t from_ಉದ್ದಳತೆ = 0;
                    Local_UResource_Bundle_ಸೂಚಕ fromRes(ures_get_By_ಕೀಲಿ(currencyRes.getAlias(), "from", ಶೂನ್ಯಸೂಚಕ, &local_ಸ್ಥಿತಿ));
                    ಸ್ಥಿರಾಂಕ int32_t *fromArray = ures_get_Int_ಸದಿಶ(fromRes.getAlias(), &from_ಉದ್ದಳತೆ, &local_ಸ್ಥಿತಿ);

                    int64_t currDate64 = (int64_t)((uint64_t)fromArray[0] << 32);
                    currDate64 |= ((int64_t)fromArray[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                    UDate fromDate = (UDate)currDate64;

                    ಆದರೆ (ures_get_ಗಾತ್ರ(currencyRes.getAlias()) > 2)
                    {
                        int32_t ಗಮ್ಯ_ಉದ್ದಳತೆ = 0;
                        Local_UResource_Bundle_ಸೂಚಕ ಗಮ್ಯ_Res(ures_get_By_ಕೀಲಿ(currencyRes.getAlias(), "to", ಶೂನ್ಯಸೂಚಕ, &local_ಸ್ಥಿತಿ));
                        ಸ್ಥಿರಾಂಕ int32_t *ಗಮ್ಯ_Array = ures_get_Int_ಸದಿಶ(ಗಮ್ಯ_Res.getAlias(), &ಗಮ್ಯ_ಉದ್ದಳತೆ, &local_ಸ್ಥಿತಿ);

                        currDate64 = (int64_t)ಗಮ್ಯ_Array[0] << 32;
                        currDate64 |= ((int64_t)ಗಮ್ಯ_Array[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                        UDate ಗಮ್ಯ_Date = (UDate)currDate64;

                        ಆದರೆ ((fromDate <= date) && (date < ಗಮ್ಯ_Date))
                        {
                            curr_ಸೂಚ್ಯಂಕ++;
                            ಆದರೆ (curr_ಸೂಚ್ಯಂಕ == ಸೂಚ್ಯಂಕ)
                            {
                                ಹೊಂದಿಕೆ_ಸಿಕ್ಕಿತು = ನಿಜ;
                            }
                        }
                    }
                    ಇಲ್ಲದಿದ್ದರೆ
                    {
                        ಆದರೆ (fromDate <= date)
                        {
                            curr_ಸೂಚ್ಯಂಕ++;
                            ಆದರೆ (curr_ಸೂಚ್ಯಂಕ == ಸೂಚ್ಯಂಕ)
                            {
                                ಹೊಂದಿಕೆ_ಸಿಕ್ಕಿತು = ನಿಜ;
                            }
                        }
                    }
                    // check for loop exit
                    ಆದರೆ (ಹೊಂದಿಕೆ_ಸಿಕ್ಕಿತು)
                    {
                        ನಿಲ್ಲಿಸು;
                    }

                } // end For loop
            }

            // Check for errors
            ಆದರೆ (*ec == U_ZERO_ERROR || local_ಸ್ಥಿತಿ != U_ZERO_ERROR)
            {
                // There is nothing to fallback to. 
                // Report the failure/warning if possible.
                *ec = local_ಸ್ಥಿತಿ;
            }

            ಆದರೆ (U_SUCCESS(*ec))
            {
                // no errors
                ಆದರೆ((buff_ಸಾಮರ್ಥ್ಯ> resLen) && ಹೊಂದಿಕೆ_ಸಿಕ್ಕಿತು)
                {
                    // write out the currency value
                    u_strcpy(buff, s);
                }
                ಇಲ್ಲದಿದ್ದರೆ
                {
                    ಹಿಂತಿರುಗಿ 0;
                }
            }

            // return null terminated currency string
            ಹಿಂತಿರುಗಿ u_terminateUChars(buff, buff_ಸಾಮರ್ಥ್ಯ, resLen, ec);
        }
        ಇಲ್ಲದಿದ್ದರೆ
        {
            // illegal argument encountered
            *ec = U_ILLEGAL_ವಾದ_ERROR;
        }

    }

    // If we got here, either error code is invalid or
    // some argument passed is no good.
    ಹಿಂತಿರುಗಿ resLen;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ UEnumeration default_Keyword_ಮೌಲ್ಯಗಳು = {
    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ,
    ulist_close_keyword_ಮೌಲ್ಯಗಳು_iterator,
    ulist_ಎಣಿಕೆ_keyword_ಮೌಲ್ಯಗಳು,
    uenum_unextDefault,
    ulist_ಮುಂದಿನ_keyword_ಮೌಲ್ಯ, 
    ulist_reset_keyword_ಮೌಲ್ಯಗಳು_iterator
};

U_CAPI UEnumeration *U_EXPORT2 ucurr_get_Keyword_ಮೌಲ್ಯಗಳು_For_Locale(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *locale, UBool commonly_ಬಳಸಿದ, UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2) {
    // Resolve region
    CharString prefRegion = ulocimp_get_Region_For_Supplemental_ದತ್ತಾಂಶ(locale, ನಿಜ, *ಸ್ಥಿತಿ_2);

    // Read value from supplementalData
    UList *ಮೌಲ್ಯಗಳು = ulist_create_ಖಾಲಿಗಳು_List(ಸ್ಥಿತಿ_2);
    UList *other_ಮೌಲ್ಯಗಳು = ulist_create_ಖಾಲಿಗಳು_List(ಸ್ಥಿತಿ_2);
    UEnumeration *en = (UEnumeration *)uprv_malloc(ಗಾತ್ರ(UEnumeration));
    ಆದರೆ (U_FAILURE(*ಸ್ಥಿತಿ_2) || en == ಶೂನ್ಯಸೂಚಕ) {
        ಆದರೆ (en == ಶೂನ್ಯಸೂಚಕ) {
            *ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        } ಇಲ್ಲದಿದ್ದರೆ {
            uprv_free(en);
        }
        ulist_deleteList(ಮೌಲ್ಯಗಳು);
        ulist_deleteList(other_ಮೌಲ್ಯಗಳು);
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಸ್ಮರಣೆ_ನಕಲು(en, &default_Keyword_ಮೌಲ್ಯಗಳು, ಗಾತ್ರ(UEnumeration));
    en->context = ಮೌಲ್ಯಗಳು;
    
    UResourceBundle* rb = ures_open_ನೇರ(U_ICUDATA_CURR, "supplementalData", ಸ್ಥಿತಿ_2);
    Local_UResource_Bundle_ಸೂಚಕ bundle(ures_get_By_ಕೀಲಿ(rb, "CurrencyMap", rb, ಸ್ಥಿತಿ_2));
    ರಾಶಿ_UResource_Bundle bundlekey, regbndl, curbndl, ಗಮ್ಯ;
    
    ಆಗ (U_SUCCESS(*ಸ್ಥಿತಿ_2) && ures_has_ಮುಂದಿನ(bundle.getAlias())) {
        ures_get_ಮುಂದಿನ_Resource(bundle.getAlias(), bundlekey.getAlias(), ಸ್ಥಿತಿ_2);
        ಆದರೆ (U_FAILURE(*ಸ್ಥಿತಿ_2)) {
            ನಿಲ್ಲಿಸು;
        }
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *region = ures_get_ಕೀಲಿ(bundlekey.getAlias());
        UBool isPrefRegion = prefRegion == region;
        ಆದರೆ (!isPrefRegion && commonly_ಬಳಸಿದ) {
            // With commonlyUsed=true, we do not put
            // currencies for other regions in the
            // result list.
            ಮುಂದುವರಿ;
        }
        ures_get_By_ಕೀಲಿ(bundle.getAlias(), region, regbndl.getAlias(), ಸ್ಥಿತಿ_2);
        ಆದರೆ (U_FAILURE(*ಸ್ಥಿತಿ_2)) {
            ನಿಲ್ಲಿಸು;
        }
        ಆಗ (U_SUCCESS(*ಸ್ಥಿತಿ_2) && ures_has_ಮುಂದಿನ(regbndl.getAlias())) {
            ures_get_ಮುಂದಿನ_Resource(regbndl.getAlias(), curbndl.getAlias(), ಸ್ಥಿತಿ_2);
            ಆದರೆ (ures_getType(curbndl.getAlias()) != URES_ಕೋಷ್ಟಕ) {
                // Currently, an empty ARRAY is mixed in.
                ಮುಂದುವರಿ;
            }
            ಅಕ್ಷರ *curID = (ಅಕ್ಷರ *)uprv_malloc(ಗಾತ್ರ(ಅಕ್ಷರ) * ULOC_KEYWORDS_ಸಾಮರ್ಥ್ಯ);
            ಆದರೆ (curID == ಶೂನ್ಯಸೂಚಕ) {
                *ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
                ನಿಲ್ಲಿಸು;
            }
            int32_t curIDLength = ULOC_KEYWORDS_ಸಾಮರ್ಥ್ಯ;

#ಆದರೆ U_CHARSET_FAMILY==U_ASCII_FAMILY
            ures_get_UTF8_String_By_ಕೀಲಿ(curbndl.getAlias(), "id", curID, &curIDLength, ನಿಜ, ಸ್ಥಿತಿ_2);
            /* optimize - use the utf-8 string */
#ಇಲ್ಲದಿದ್ದರೆ
            {
                       ಸ್ಥಿರಾಂಕ char16_t* defString = ures_get_String_By_ಕೀಲಿ(curbndl.getAlias(), "id", &curIDLength, ಸ್ಥಿತಿ_2);
                       ಆದರೆ(U_SUCCESS(*ಸ್ಥಿತಿ_2)) {
			   ಆದರೆ(curIDLength+1 > ULOC_KEYWORDS_ಸಾಮರ್ಥ್ಯ) {
				*ಸ್ಥಿತಿ_2 = U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
			   } ಇಲ್ಲದಿದ್ದರೆ {
                           	u_UChars_ಗಮ್ಯ_Chars(defString, curID, curIDLength+1);
			   }
                       }
            }
#ಆದರೆ_ಅಂತ್ಯ	

            ಆದರೆ (U_FAILURE(*ಸ್ಥಿತಿ_2)) {
                ನಿಲ್ಲಿಸು;
            }
            UBool has_ಗಮ್ಯ = ಸುಳ್ಳು;
            ures_get_By_ಕೀಲಿ(curbndl.getAlias(), "to", ಗಮ್ಯ.getAlias(), ಸ್ಥಿತಿ_2);
            ಆದರೆ (U_FAILURE(*ಸ್ಥಿತಿ_2)) {
                // Do nothing here...
                *ಸ್ಥಿತಿ_2 = U_ZERO_ERROR;
            } ಇಲ್ಲದಿದ್ದರೆ {
                has_ಗಮ್ಯ = ನಿಜ;
            }
            ಆದರೆ (isPrefRegion && !has_ಗಮ್ಯ && !ulist_ಒಳಗೊಂಡಿದೆ_String(ಮೌಲ್ಯಗಳು, curID, (int32_t)uprv_strlen(curID))) {
                // Currently active currency for the target country
                ulist_ಕೂಡಿಸು_ಅಂಶ_ಕೊನೆ_List(ಮೌಲ್ಯಗಳು, curID, ನಿಜ, ಸ್ಥಿತಿ_2);
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!ulist_ಒಳಗೊಂಡಿದೆ_String(other_ಮೌಲ್ಯಗಳು, curID, (int32_t)uprv_strlen(curID)) && !commonly_ಬಳಸಿದ) {
                ulist_ಕೂಡಿಸು_ಅಂಶ_ಕೊನೆ_List(other_ಮೌಲ್ಯಗಳು, curID, ನಿಜ, ಸ್ಥಿತಿ_2);
            } ಇಲ್ಲದಿದ್ದರೆ {
                uprv_free(curID);
            }
        }
        
    }
    ಆದರೆ (U_SUCCESS(*ಸ್ಥಿತಿ_2)) {
        ಆದರೆ (commonly_ಬಳಸಿದ) {
            ಆದರೆ (ulist_get_List_ಗಾತ್ರ(ಮೌಲ್ಯಗಳು) == 0) {
                // This could happen if no valid region is supplied in the input
                // locale. In this case, we use the CLDR's default.
                uenum_close(en);
                en = ucurr_get_Keyword_ಮೌಲ್ಯಗಳು_For_Locale(ಕೀಲಿ, "und", ನಿಜ, ಸ್ಥಿತಿ_2);
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            // Consolidate the list
            ಅಕ್ಷರ *value = ಶೂನ್ಯಸೂಚಕ;
            ulist_resetList(other_ಮೌಲ್ಯಗಳು);
            ಆಗ ((value = (ಅಕ್ಷರ *)ulist_get_ಮುಂದಿನ(other_ಮೌಲ್ಯಗಳು)) != ಶೂನ್ಯಸೂಚಕ) {
                ಆದರೆ (!ulist_ಒಳಗೊಂಡಿದೆ_String(ಮೌಲ್ಯಗಳು, value, (int32_t)uprv_strlen(value))) {
                    ಅಕ್ಷರ *tmp_ಮೌಲ್ಯ = (ಅಕ್ಷರ *)uprv_malloc(ಗಾತ್ರ(ಅಕ್ಷರ) * ULOC_KEYWORDS_ಸಾಮರ್ಥ್ಯ);
                    ಆದರೆ (tmp_ಮೌಲ್ಯ == ಶೂನ್ಯಸೂಚಕ) {
                        *ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
                        ನಿಲ್ಲಿಸು;
                    }
                    uprv_memcpy(tmp_ಮೌಲ್ಯ, value, uprv_strlen(value) + 1);
                    ulist_ಕೂಡಿಸು_ಅಂಶ_ಕೊನೆ_List(ಮೌಲ್ಯಗಳು, tmp_ಮೌಲ್ಯ, ನಿಜ, ಸ್ಥಿತಿ_2);
                    ಆದರೆ (U_FAILURE(*ಸ್ಥಿತಿ_2)) {
                        ನಿಲ್ಲಿಸು;
                    }
                }
            }
        }
        ulist_resetList((UList *)(en->context));
    } ಇಲ್ಲದಿದ್ದರೆ {
        ulist_deleteList(ಮೌಲ್ಯಗಳು);
        uprv_free(en);
        ಮೌಲ್ಯಗಳು = ಶೂನ್ಯಸೂಚಕ;
        en = ಶೂನ್ಯಸೂಚಕ;
    }
    ulist_deleteList(other_ಮೌಲ್ಯಗಳು);
    ಹಿಂತಿರುಗಿ en;
}


U_CAPI int32_t U_EXPORT2
ucurr_get_Numeric_ಸಂಕೇತ(ಸ್ಥಿರಾಂಕ char16_t* currency) {
    int32_t ಸಂಕೇತ_2 = 0;
    ಆದರೆ (currency && u_strlen(currency) == ISO_CURRENCY_ಸಂಕೇತ_ಉದ್ದಳತೆ) {
        UError_ಸಂಕೇತ ಸ್ಥಿತಿ_2 = U_ZERO_ERROR;

        UResourceBundle* bundle = ures_open_ನೇರ(ಶೂನ್ಯಸೂಚಕ, "currencyNumericCodes", &ಸ್ಥಿತಿ_2);
        Local_UResource_Bundle_ಸೂಚಕ ಸಂಕೇತ_Map(ures_get_By_ಕೀಲಿ(bundle, "codeMap", bundle, &ಸ್ಥಿತಿ_2));
        ಆದರೆ (U_SUCCESS(ಸ್ಥಿತಿ_2)) {
            ಅಕ್ಷರ alpha_ಸಂಕೇತ[ISO_CURRENCY_ಸಂಕೇತ_ಉದ್ದಳತೆ+1];
            my_UChars_ಗಮ್ಯ_Chars(alpha_ಸಂಕೇತ, currency);
            T_CString_ಗಮ್ಯ_ಊರ್ಧ್ವ_Case(alpha_ಸಂಕೇತ);
            ures_get_By_ಕೀಲಿ(ಸಂಕೇತ_Map.getAlias(), alpha_ಸಂಕೇತ, ಸಂಕೇತ_Map.getAlias(), &ಸ್ಥಿತಿ_2);
            ಪೂರ್ಣಾಂಕ tmp_ಸಂಕೇತ = ures_getInt(ಸಂಕೇತ_Map.getAlias(), &ಸ್ಥಿತಿ_2);
            ಆದರೆ (U_SUCCESS(ಸ್ಥಿತಿ_2)) {
                ಸಂಕೇತ_2 = tmp_ಸಂಕೇತ;
            }
        }
    }
    ಹಿಂತಿರುಗಿ ಸಂಕೇತ_2;
}
#ಆದರೆ_ಅಂತ್ಯ /* #if !UCONFIG_NO_FORMATTING */

//eof
