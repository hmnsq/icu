// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
* Copyright (c) 2002-2016, International Business Machines
* Corporation and others.  All Rights Reserved.
**********************************************************************
*/

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

#ਜੇ !UCONFIG_NO_FORMATTING

#ਅਨ੍ਤਰ੍ਭਾਵ <utility>

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ucurr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/locid.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ures.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ustring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/parsepos.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uniset.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/usetiter.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utf16.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ustr_imp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "charstr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cstring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "static_unicode_sets.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uassert.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "umutex.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucln_cmn.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uenumimp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uhash.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hash.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uinvchar.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uresimp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ulist.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uresimp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ureslocs.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ulocimp.h"

ਵਰਤੋਂ ਨਾਮਾਕਾਸ਼ icu;

//#define UCURR_DEBUG_EQUIV 1
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UCURR_DEBUG_EQUIV
#ਅਨ੍ਤਰ੍ਭਾਵ "stdio.h"
#ਜੇ_ਅੰਤ
//#define UCURR_DEBUG 1
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UCURR_DEBUG
#ਅਨ੍ਤਰ੍ਭਾਵ "stdio.h"
#ਜੇ_ਅੰਤ

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ Iso_ਕੋਡ_ਇੰਦਰਾਜ {
    ਅਚਲ char16_t *iso_ਕੋਡ; /* const because it's a reference to a resource bundle string. */
    UDate from;
    UDate ਮੰਜ਼ਿਲ;
} Iso_ਕੋਡ_ਇੰਦਰਾਜ;

//------------------------------------------------------------
// Constants

// Default currency meta data of last resort.  We try to use the
// defaults encoded in the meta data resource bundle.  If there is a
// configuration/build error and these are not available, we use these
// hard-coded defaults (which should be identical).
ਸਥਿਰ ਅਚਲ int32_t ਆਖਰੀ_RESORT_ਅੰਕੜੇ[] = { 2, 0, 2, 0 };

// POW10[i] = 10^i, i=0..MAX_POW10
ਸਥਿਰ ਅਚਲ int32_t POW10[] = { 1, 10, 100, 1000, 10000, 100000,
                                 1000000, 10000000, 100000000, 1000000000 };

ਸਥਿਰ ਅਚਲ int32_t MAX_POW10 = UPRV_LENGTHOF(POW10) - 1;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ISO_CURRENCY_ਕੋਡ_ਲੰਮਾਈ 3

//------------------------------------------------------------
// Resource tags
//

ਸਥਿਰ ਅਚਲ ਅੱਖਰ CURRENCY_ਅੰਕੜੇ[] = "supplementalData";
// Tag for meta-data, in root.
ਸਥਿਰ ਅਚਲ ਅੱਖਰ CURRENCY_META[] = "CurrencyMeta";

// Tag for map from countries to currencies, in root.
ਸਥਿਰ ਅਚਲ ਅੱਖਰ CURRENCY_MAP[] = "CurrencyMap";

// Tag for default meta-data, in CURRENCY_META
ਸਥਿਰ ਅਚਲ ਅੱਖਰ DEFAULT_META[] = "DEFAULT";

// Variant delimiter
ਸਥਿਰ ਅਚਲ ਅੱਖਰ VAR_DELIM = '_';

// Tag for localized display names (symbols) of currencies
ਸਥਿਰ ਅਚਲ ਅੱਖਰ CURRENCIES[] = "Currencies";
ਸਥਿਰ ਅਚਲ ਅੱਖਰ CURRENCIES_NARROW[] = "Currencies%narrow";
ਸਥਿਰ ਅਚਲ ਅੱਖਰ CURRENCIES_FORMAL[] = "Currencies%formal";
ਸਥਿਰ ਅਚਲ ਅੱਖਰ CURRENCIES_VARIANT[] = "Currencies%variant";
ਸਥਿਰ ਅਚਲ ਅੱਖਰ CURRENCYPLURALS[] = "CurrencyPlurals";

// ISO codes mapping table
ਸਥਿਰ ਅਚਲ UHashtable* gIsoCodes = ਖਾਲੀ_ਸੂਚਕ;
ਸਥਿਰ icu::UInitOnce gIsoCodesInitOnce {};

// Currency symbol equivalances
ਸਥਿਰ ਅਚਲ icu::Hashtable* gCurrSymbolsEquiv = ਖਾਲੀ_ਸੂਚਕ;
ਸਥਿਰ icu::UInitOnce gCurrSymbolsEquivInitOnce {};

U_NAMESPACE_BEGIN

// EquivIterator iterates over all strings that are equivalent to a given
// string, s. Note that EquivIterator will never yield s itself.
ਵਰਗ EquivIterator : ਜਨਤਕ icu::UMemory {
ਜਨਤਕ:
    // Constructor. hash stores the equivalence relationships; s is the string
    // for which we find equivalent strings.
    ਸਤਰਅੰਦਰ EquivIterator(ਅਚਲ icu::Hashtable& hash, ਅਚਲ icu::UnicodeString& s)
        : _hash(hash) { 
        ਸ਼ੁਰੂ_2 = _current = &s;
    }
    ਸਤਰਅੰਦਰ ~EquivIterator() { }

    // next returns the next equivalent string or nullptr if there are no more.
    // If s has no equivalent strings, next returns nullptr on the first call.
    ਅਚਲ icu::UnicodeString *next();
ਨਿਜੀ:
    ਅਚਲ icu::Hashtable& _hash;
    ਅਚਲ icu::UnicodeString* ਸ਼ੁਰੂ_2;
    ਅਚਲ icu::UnicodeString* _current;
};

ਅਚਲ icu::UnicodeString *
EquivIterator::next() {
    ਅਚਲ icu::UnicodeString* ਅਗਲਾ_2 = static_cast<ਅਚਲ icu::UnicodeString*>(_hash.get(*_current));
    ਜੇ (ਅਗਲਾ_2 == ਖਾਲੀ_ਸੂਚਕ) {
        U_ASSERT(_current == ਸ਼ੁਰੂ_2);
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਜੇ (*ਅਗਲਾ_2 == *ਸ਼ੁਰੂ_2) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    _current = ਅਗਲਾ_2;
    ਵਾਪਸ ਅਗਲਾ_2;
}

U_NAMESPACE_ਸਿਰਾ

// makeEquivalent makes lhs and rhs equivalent by updating the equivalence
// relations in hash accordingly.
ਸਥਿਰ ਖਾਲੀ makeEquivalent(
    ਅਚਲ icu::UnicodeString &lhs,
    ਅਚਲ icu::UnicodeString &rhs,
    icu::Hashtable* hash, UError_ਕੋਡ &ਸਥਿਤੀ) {
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ;
    }
    ਜੇ (lhs == rhs) {
        // already equivalent
        ਵਾਪਸ;
    }
    icu::EquivIterator ਖੱਬਾ_Iter(*hash, lhs);
    icu::EquivIterator ਸੱਜਾ_Iter(*hash, rhs);
    ਅਚਲ icu::UnicodeString *ਪਹਿਲਾ_ਖੱਬਾ = ਖੱਬਾ_Iter.next();
    ਅਚਲ icu::UnicodeString *ਪਹਿਲਾ_ਸੱਜਾ = ਸੱਜਾ_Iter.next();
    ਅਚਲ icu::UnicodeString *ਅਗਲਾ_ਖੱਬਾ = ਪਹਿਲਾ_ਖੱਬਾ;
    ਅਚਲ icu::UnicodeString *ਅਗਲਾ_ਸੱਜਾ = ਪਹਿਲਾ_ਸੱਜਾ;
    ਜਦੋਂ (ਅਗਲਾ_ਖੱਬਾ != ਖਾਲੀ_ਸੂਚਕ && ਅਗਲਾ_ਸੱਜਾ != ਖਾਲੀ_ਸੂਚਕ) {
        ਜੇ (*ਅਗਲਾ_ਖੱਬਾ == rhs || *ਅਗਲਾ_ਸੱਜਾ == lhs) {
            // Already equivalent
            ਵਾਪਸ;
        }
        ਅਗਲਾ_ਖੱਬਾ = ਖੱਬਾ_Iter.next();
        ਅਗਲਾ_ਸੱਜਾ = ਸੱਜਾ_Iter.next();
    }
    // Not equivalent. Must join.
    icu::UnicodeString *new_ਪਹਿਲਾ_ਖੱਬਾ;
    icu::UnicodeString *new_ਪਹਿਲਾ_ਸੱਜਾ;
    ਜੇ (ਪਹਿਲਾ_ਸੱਜਾ == ਖਾਲੀ_ਸੂਚਕ && ਪਹਿਲਾ_ਖੱਬਾ == ਖਾਲੀ_ਸੂਚਕ) {
        // Neither lhs or rhs belong to an equivalence circle, so we form
        // a new equivalnce circle of just lhs and rhs.
        new_ਪਹਿਲਾ_ਖੱਬਾ = ਨਵਾਂ icu::UnicodeString(rhs);
        new_ਪਹਿਲਾ_ਸੱਜਾ = ਨਵਾਂ icu::UnicodeString(lhs);
    } ਨਹੀਂਤਾਂ ਜੇ (ਪਹਿਲਾ_ਸੱਜਾ == ਖਾਲੀ_ਸੂਚਕ) {
        // lhs belongs to an equivalence circle, but rhs does not, so we link
        // rhs into lhs' circle.
        new_ਪਹਿਲਾ_ਖੱਬਾ = ਨਵਾਂ icu::UnicodeString(rhs);
        new_ਪਹਿਲਾ_ਸੱਜਾ = ਨਵਾਂ icu::UnicodeString(*ਪਹਿਲਾ_ਖੱਬਾ);
    } ਨਹੀਂਤਾਂ ਜੇ (ਪਹਿਲਾ_ਖੱਬਾ == ਖਾਲੀ_ਸੂਚਕ) {
        // rhs belongs to an equivlance circle, but lhs does not, so we link
        // lhs into rhs' circle.
        new_ਪਹਿਲਾ_ਖੱਬਾ = ਨਵਾਂ icu::UnicodeString(*ਪਹਿਲਾ_ਸੱਜਾ);
        new_ਪਹਿਲਾ_ਸੱਜਾ = ਨਵਾਂ icu::UnicodeString(lhs);
    } ਨਹੀਂਤਾਂ {
        // Both lhs and rhs belong to different equivalnce circles. We link
        // them together to form one single, larger equivalnce circle.
        new_ਪਹਿਲਾ_ਖੱਬਾ = ਨਵਾਂ icu::UnicodeString(*ਪਹਿਲਾ_ਸੱਜਾ);
        new_ਪਹਿਲਾ_ਸੱਜਾ = ਨਵਾਂ icu::UnicodeString(*ਪਹਿਲਾ_ਖੱਬਾ);
    }
    ਜੇ (new_ਪਹਿਲਾ_ਖੱਬਾ == ਖਾਲੀ_ਸੂਚਕ || new_ਪਹਿਲਾ_ਸੱਜਾ == ਖਾਲੀ_ਸੂਚਕ) {
        ਮਿਟਾਓ new_ਪਹਿਲਾ_ਖੱਬਾ;
        ਮਿਟਾਓ new_ਪਹਿਲਾ_ਸੱਜਾ;
        ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ;
    }
    hash->put(lhs, (ਖਾਲੀ *) new_ਪਹਿਲਾ_ਖੱਬਾ, ਸਥਿਤੀ);
    hash->put(rhs, (ਖਾਲੀ *) new_ਪਹਿਲਾ_ਸੱਜਾ, ਸਥਿਤੀ);
}

// countEquivalent counts how many strings are equivalent to s.
// hash stores all the equivalnce relations.
// countEquivalent does not include s itself in the count.
ਸਥਿਰ int32_t ਗਿਣਤੀ_Equivalent(ਅਚਲ icu::Hashtable &hash, ਅਚਲ icu::UnicodeString &s) {
    int32_t ਨਤੀਜਾ = 0;
    icu::EquivIterator iter(hash, s);
    ਜਦੋਂ (iter.next() != ਖਾਲੀ_ਸੂਚਕ) {
        ++ਨਤੀਜਾ;
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UCURR_DEBUG_EQUIV
 {
   ਅੱਖਰ tmp[200];
   s.extract(0,s.length(),tmp, "UTF-8");
   ਲਿਖੋ("CountEquivalent('%s') = %d\n", tmp, ਨਤੀਜਾ);
 }
#ਜੇ_ਅੰਤ
    ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ ਅਚਲ icu::Hashtable* getCurrSymbolsEquiv();

//------------------------------------------------------------
//ਕੋਡ

/**
 * Cleanup callback func
 */
ਸਥਿਰ UBool U_CALLCONV 
isoCodes_cleanup()
{
    ਜੇ (gIsoCodes != ਖਾਲੀ_ਸੂਚਕ) {
        uhash_close(const_cast<UHashtable *>(gIsoCodes));
        gIsoCodes = ਖਾਲੀ_ਸੂਚਕ;
    }
    gIsoCodesInitOnce.reset();
    ਵਾਪਸ ਸੱਚ;
}

/**
 * Cleanup callback func
 */
ਸਥਿਰ UBool U_CALLCONV 
currSymbolsEquiv_cleanup()
{
    ਮਿਟਾਓ const_cast<icu::Hashtable *>(gCurrSymbolsEquiv);
    gCurrSymbolsEquiv = ਖਾਲੀ_ਸੂਚਕ;
    gCurrSymbolsEquivInitOnce.reset();
    ਵਾਪਸ ਸੱਚ;
}

/**
 * Deleter for IsoCodeEntry
 */
ਸਥਿਰ ਖਾਲੀ U_CALLCONV
delete_Iso_ਕੋਡ_ਇੰਦਰਾਜ(ਖਾਲੀ *obj) {
    Iso_ਕੋਡ_ਇੰਦਰਾਜ* ਇੰਦਰਾਜ = static_cast<Iso_ਕੋਡ_ਇੰਦਰਾਜ*>(obj);
    uprv_free(ਇੰਦਰਾਜ);
}

/**
 * Deleter for gCurrSymbolsEquiv.
 */
ਸਥਿਰ ਖਾਲੀ U_CALLCONV
deleteUnicode(ਖਾਲੀ *obj) {
    icu::UnicodeString* ਇੰਦਰਾਜ = static_cast<icu::UnicodeString*>(obj);
    ਮਿਟਾਓ ਇੰਦਰਾਜ;
}

/**
 * Unfortunately, we have to convert the char16_t* currency code to char*
 * to use it as a resource key.
 */
ਸਥਿਰ ਸਤਰਅੰਦਰ ਅੱਖਰ*
my_UChars_ਮੰਜ਼ਿਲ_Chars(ਅੱਖਰ* ਨਤੀਜਾ_ਦਾ_Len4, ਅਚਲ char16_t* currency) {
    u_UChars_ਮੰਜ਼ਿਲ_Chars(currency, ਨਤੀਜਾ_ਦਾ_Len4, ISO_CURRENCY_ਕੋਡ_ਲੰਮਾਈ);
    ਨਤੀਜਾ_ਦਾ_Len4[ISO_CURRENCY_ਕੋਡ_ਲੰਮਾਈ] = 0;
    ਵਾਪਸ ਨਤੀਜਾ_ਦਾ_Len4;
}

/**
 * Internal function to look up currency data.  Result is an array of
 * four integers.  The first is the fraction digits.  The second is the
 * rounding increment, or 0 if none.  The rounding increment is in
 * units of 10^(-fraction_digits).  The third and fourth are the same
 * except that they are those used in cash transactions ( cashDigits
 * and cashRounding ).
 */
ਸਥਿਰ ਅਚਲ int32_t*
ਲੱਭੋ_Meta_ਅੰਕੜੇ(ਅਚਲ char16_t* currency, UError_ਕੋਡ& ec) {

    ਜੇ (currency == ਖਾਲੀ_ਸੂਚਕ || *currency == 0) {
        ਜੇ (U_SUCCESS(ec)) {
            ec = U_ILLEGAL_ਦਲੀਲ_ERROR;
        }
        ਵਾਪਸ ਆਖਰੀ_RESORT_ਅੰਕੜੇ;
    }

    // Get CurrencyMeta resource out of root locale file.  [This may
    // move out of the root locale file later; if it does, update this
    // code.]
    UResourceBundle* currency_ਅੰਕੜੇ = ures_open_ਸਿੱਧਾ(U_ICUDATA_CURR, CURRENCY_ਅੰਕੜੇ, &ec);
    Local_UResource_Bundle_ਸੂਚਕ currencyMeta(ures_get_By_ਕੁੰਜੀ(currency_ਅੰਕੜੇ, CURRENCY_META, currency_ਅੰਕੜੇ, &ec));

    ਜੇ (U_FAILURE(ec)) {
        // Config/build error; return hard-coded defaults
        ਵਾਪਸ ਆਖਰੀ_RESORT_ਅੰਕੜੇ;
    }

    // Look up our currency, or if that's not available, then DEFAULT
    ਅੱਖਰ buf[ISO_CURRENCY_ਕੋਡ_ਲੰਮਾਈ+1];
    UError_ਕੋਡ ec2 = U_ZERO_ERROR; // local error code: soft failure
    Local_UResource_Bundle_ਸੂਚਕ rb(ures_get_By_ਕੁੰਜੀ(currencyMeta.getAlias(), my_UChars_ਮੰਜ਼ਿਲ_Chars(buf, currency), ਖਾਲੀ_ਸੂਚਕ, &ec2));
      ਜੇ (U_FAILURE(ec2)) {
        rb.adoptInstead(ures_get_By_ਕੁੰਜੀ(currencyMeta.getAlias(),DEFAULT_META, ਖਾਲੀ_ਸੂਚਕ, &ec));
        ਜੇ (U_FAILURE(ec)) {
            // Config/build error; return hard-coded defaults
            ਵਾਪਸ ਆਖਰੀ_RESORT_ਅੰਕੜੇ;
        }
    }

    int32_t len;
    ਅਚਲ int32_t *ਅੰਕੜੇ = ures_get_Int_ਸਦਿਸ਼(rb.getAlias(), &len, &ec);
    ਜੇ (U_FAILURE(ec) || len != 4) {
        // Config/build error; return hard-coded defaults
        ਜੇ (U_SUCCESS(ec)) {
            ec = U_INVALID_FORMAT_ERROR;
        }
        ਵਾਪਸ ਆਖਰੀ_RESORT_ਅੰਕੜੇ;
    }

    ਵਾਪਸ ਅੰਕੜੇ;
}

// -------------------------------------

ਸਥਿਰ CharString
idForLocale(ਅਚਲ ਅੱਖਰ* locale, UError_ਕੋਡ* ec)
{
    ਵਾਪਸ ulocimp_get_Region_For_Supplemental_ਅੰਕੜੇ(locale, ਝੂਠ, *ec);
}

// ------------------------------------------
//
// Registration
//
//-------------------------------------------

// don't use ICUService since we don't need fallback

U_CDECL_BEGIN
ਸਥਿਰ UBool U_CALLCONV currency_cleanup();
U_CDECL_ਸਿਰਾ

#ਜੇ !UCONFIG_NO_SERVICE
ਬਣਤਰ CReg;

ਸਥਿਰ UMutex g_CReg_ਤਾਲਾ;
ਸਥਿਰ CReg* g_CReg_ਸਿਰ = ਖਾਲੀ_ਸੂਚਕ;

ਬਣਤਰ CReg : ਜਨਤਕ icu::UMemory {
    CReg *next;
    char16_t iso[ISO_CURRENCY_ਕੋਡ_ਲੰਮਾਈ+1];
    ਅੱਖਰ  id[ULOC_FULLNAME_ਸਮਰੱਥਾ];

    CReg(ਅਚਲ char16_t* _iso, ਅਚਲ ਅੱਖਰ* _id)
        : next(ਖਾਲੀ_ਸੂਚਕ)
    {
        uprv_strncpy(id, _id, ਆਕਾਰ(id)-1);
        id[ਆਕਾਰ(id)-1] = 0;
        u_memcpy(iso, _iso, ISO_CURRENCY_ਕੋਡ_ਲੰਮਾਈ);
        iso[ISO_CURRENCY_ਕੋਡ_ਲੰਮਾਈ] = 0;
    }

    ਸਥਿਰ UCurr_Registry_ਕੁੰਜੀ reg(ਅਚਲ char16_t* _iso, ਅਚਲ ਅੱਖਰ* _id, UError_ਕੋਡ* ਸਥਿਤੀ)
    {
        ਜੇ (ਸਥਿਤੀ && U_SUCCESS(*ਸਥਿਤੀ) && _iso && _id) {
            CReg* n = ਨਵਾਂ CReg(_iso, _id);
            ਜੇ (n) {
                umtx_ਤਾਲਾ(&g_CReg_ਤਾਲਾ);
                ਜੇ (!g_CReg_ਸਿਰ) {
                    /* register for the first time */
                    ucln_common_ਰਜਿਸਟਰ_Cleanup(UCLN_COMMON_CURRENCY, currency_cleanup);
                }
                n->next = g_CReg_ਸਿਰ;
                g_CReg_ਸਿਰ = n;
                umtx_unlock(&g_CReg_ਤਾਲਾ);
                ਵਾਪਸ n;
            }
            *ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        }
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    ਸਥਿਰ UBool unreg(UCurr_Registry_ਕੁੰਜੀ ਕੁੰਜੀ) {
        UBool ਮਿਲਿਆ = ਝੂਠ;
        umtx_ਤਾਲਾ(&g_CReg_ਤਾਲਾ);

        CReg** p = &g_CReg_ਸਿਰ;
        ਜਦੋਂ (*p) {
            ਜੇ (*p == ਕੁੰਜੀ) {
                *p = ((CReg*)ਕੁੰਜੀ)->next;
                ਮਿਟਾਓ (CReg*)ਕੁੰਜੀ;
                ਮਿਲਿਆ = ਸੱਚ;
                ਰੋਕੋ;
            }
            p = &((*p)->next);
        }

        umtx_unlock(&g_CReg_ਤਾਲਾ);
        ਵਾਪਸ ਮਿਲਿਆ;
    }

    ਸਥਿਰ ਅਚਲ char16_t* get(ਅਚਲ ਅੱਖਰ* id) {
        ਅਚਲ char16_t* ਨਤੀਜਾ = ਖਾਲੀ_ਸੂਚਕ;
        umtx_ਤਾਲਾ(&g_CReg_ਤਾਲਾ);
        CReg* p = g_CReg_ਸਿਰ;

        /* register cleanup of the mutex */
        ucln_common_ਰਜਿਸਟਰ_Cleanup(UCLN_COMMON_CURRENCY, currency_cleanup);
        ਜਦੋਂ (p) {
            ਜੇ (uprv_strcmp(id, p->id) == 0) {
                ਨਤੀਜਾ = p->iso;
                ਰੋਕੋ;
            }
            p = p->next;
        }
        umtx_unlock(&g_CReg_ਤਾਲਾ);
        ਵਾਪਸ ਨਤੀਜਾ;
    }

    /* This doesn't need to be thread safe. It's for u_cleanup only. */
    ਸਥਿਰ ਖਾਲੀ cleanup() {
        ਜਦੋਂ (g_CReg_ਸਿਰ) {
            CReg* n = g_CReg_ਸਿਰ;
            g_CReg_ਸਿਰ = g_CReg_ਸਿਰ->next;
            ਮਿਟਾਓ n;
        }
    }
};

// -------------------------------------

U_CAPI UCurr_Registry_ਕੁੰਜੀ U_EXPORT2
ucurr_ਰਜਿਸਟਰ(ਅਚਲ char16_t* iso_ਕੋਡ, ਅਚਲ ਅੱਖਰ* locale, UError_ਕੋਡ *ਸਥਿਤੀ)
{
    ਜੇ (ਸਥਿਤੀ && U_SUCCESS(*ਸਥਿਤੀ)) {
        CharString id = idForLocale(locale, ਸਥਿਤੀ);
        ਵਾਪਸ CReg::reg(iso_ਕੋਡ, id.ਅੰਕੜੇ(), ਸਥਿਤੀ);
    }
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
}

// -------------------------------------

U_CAPI UBool U_EXPORT2
ucurr_unregister(UCurr_Registry_ਕੁੰਜੀ ਕੁੰਜੀ, UError_ਕੋਡ* ਸਥਿਤੀ)
{
    ਜੇ (ਸਥਿਤੀ && U_SUCCESS(*ਸਥਿਤੀ)) {
        ਵਾਪਸ CReg::unreg(ਕੁੰਜੀ);
    }
    ਵਾਪਸ ਝੂਠ;
}
#ਜੇ_ਅੰਤ /* UCONFIG_NO_SERVICE */

// -------------------------------------

/**
 * Release all static memory held by currency.
 */
/*The declaration here is needed so currency_cleanup()
 * can call this function.
 */
ਸਥਿਰ UBool U_CALLCONV
currency_cache_cleanup();

U_CDECL_BEGIN
ਸਥਿਰ UBool U_CALLCONV currency_cleanup() {
#ਜੇ !UCONFIG_NO_SERVICE
    CReg::cleanup();
#ਜੇ_ਅੰਤ
    /*
     * There might be some cached currency data or isoCodes data.
     */
    currency_cache_cleanup();
    isoCodes_cleanup();
    currSymbolsEquiv_cleanup();

    ਵਾਪਸ ਸੱਚ;
}
U_CDECL_ਸਿਰਾ

// -------------------------------------

U_CAPI int32_t U_EXPORT2
ucurr_forLocale(ਅਚਲ ਅੱਖਰ* locale,
                char16_t* buff,
                int32_t buff_ਸਮਰੱਥਾ,
                UError_ਕੋਡ* ec) {
    ਜੇ (U_FAILURE(*ec)) { ਵਾਪਸ 0; }
    ਜੇ (buff_ਸਮਰੱਥਾ < 0 || (buff == ਖਾਲੀ_ਸੂਚਕ && buff_ਸਮਰੱਥਾ > 0)) {
        *ec = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ 0;
    }

    UError_ਕੋਡ local_ਸਥਿਤੀ = U_ZERO_ERROR;
    CharString currency = ulocimp_get_Keyword_ਮੁੱਲ(locale, "currency", local_ਸਥਿਤੀ);
    int32_t resLen = currency.length();

    ਜੇ (U_SUCCESS(local_ਸਥਿਤੀ) && resLen == 3 && uprv_isInvariantString(currency.ਅੰਕੜੇ(), resLen)) {
        ਜੇ (resLen < buff_ਸਮਰੱਥਾ) {
            T_CString_ਮੰਜ਼ਿਲ_ਉੱਤਲਾ_Case(currency.ਅੰਕੜੇ());
            u_chars_ਮੰਜ਼ਿਲ_UChars(currency.ਅੰਕੜੇ(), buff, resLen);
        }
        ਵਾਪਸ u_terminateUChars(buff, buff_ਸਮਰੱਥਾ, resLen, ec);
    }

    // get country or country_variant in `id'
    CharString id = idForLocale(locale, ec);
    ਜੇ (U_FAILURE(*ec)) {
        ਵਾਪਸ 0;
    }

#ਜੇ !UCONFIG_NO_SERVICE
    ਅਚਲ char16_t* ਨਤੀਜਾ = CReg::get(id.ਅੰਕੜੇ());
    ਜੇ (ਨਤੀਜਾ) {
        ਜੇ(buff_ਸਮਰੱਥਾ > u_strlen(ਨਤੀਜਾ)) {
            u_strcpy(buff, ਨਤੀਜਾ);
        }
        resLen = u_strlen(ਨਤੀਜਾ);
        ਵਾਪਸ u_terminateUChars(buff, buff_ਸਮਰੱਥਾ, resLen, ec);
    }
#ਜੇ_ਅੰਤ
    // Remove variants, which is only needed for registration.
    ਅੱਖਰ *idDelim = uprv_strchr(id.ਅੰਕੜੇ(), VAR_DELIM);
    ਜੇ (idDelim) {
        id.truncate(idDelim - id.ਅੰਕੜੇ());
    }

    ਅਚਲ char16_t* s = ਖਾਲੀ_ਸੂਚਕ;  // Currency code from data file.
    ਜੇ (id.is_ਖਾਲੀ_ਥਾਵਾਂ()) {
        // No point looking in the data for an empty string.
        // This is what we would get.
        local_ਸਥਿਤੀ = U_MISSING_RESOURCE_ERROR;
    } ਨਹੀਂਤਾਂ {
        // Look up the CurrencyMap element in the root bundle.
        local_ਸਥਿਤੀ = U_ZERO_ERROR;
        UResourceBundle *rb = ures_open_ਸਿੱਧਾ(U_ICUDATA_CURR, CURRENCY_ਅੰਕੜੇ, &local_ਸਥਿਤੀ);
        UResourceBundle *cm = ures_get_By_ਕੁੰਜੀ(rb, CURRENCY_MAP, rb, &local_ਸਥਿਤੀ);
        Local_UResource_Bundle_ਸੂਚਕ countryArray(ures_get_By_ਕੁੰਜੀ(rb, id.ਅੰਕੜੇ(), cm, &local_ਸਥਿਤੀ));
        // https://unicode-org.atlassian.net/browse/ICU-21997
        // Prefer to use currencies that are legal tender.
        ਜੇ (U_SUCCESS(local_ਸਥਿਤੀ)) {
            int32_t array_ਲੰਮਾਈ = ures_get_ਮਾਪ(countryArray.getAlias());
            ਲਈ (int32_t i = 0; i < array_ਲੰਮਾਈ; ++i) {
                Local_UResource_Bundle_ਸੂਚਕ currencyReq(
                    ures_get_By_ਸੂਚਕਾਂਕ(countryArray.getAlias(), i, ਖਾਲੀ_ਸੂਚਕ, &local_ਸਥਿਤੀ));
                // The currency is legal tender if it is *not* marked with tender{"false"}.
                UError_ਕੋਡ tender_ਸਥਿਤੀ = local_ਸਥਿਤੀ;
                ਅਚਲ char16_t *tender =
                    ures_get_String_By_ਕੁੰਜੀ(currencyReq.getAlias(), "tender", ਖਾਲੀ_ਸੂਚਕ, &tender_ਸਥਿਤੀ);
                ਤਾਰਕਿਕ isTender = U_FAILURE(tender_ਸਥਿਤੀ) || u_strcmp(tender, u"false") != 0;
                ਜੇ (!isTender && s != ਖਾਲੀ_ਸੂਚਕ) {
                    // We already have a non-tender currency. Ignore all following non-tender ones.
                    ਜਾਰੀ;
                }
                // Fetch the currency code.
                s = ures_get_String_By_ਕੁੰਜੀ(currencyReq.getAlias(), "id", &resLen, &local_ਸਥਿਤੀ);
                ਜੇ (isTender) {
                    ਰੋਕੋ;
                }
            }
            ਜੇ (U_SUCCESS(local_ਸਥਿਤੀ) && s == ਖਾਲੀ_ਸੂਚਕ) {
                local_ਸਥਿਤੀ = U_MISSING_RESOURCE_ERROR;
            }
        }
    }

    ਜੇ ((U_FAILURE(local_ਸਥਿਤੀ)) && strchr(id.ਅੰਕੜੇ(), '_') != ਖਾਲੀ_ਸੂਚਕ) {
        // We don't know about it.  Check to see if we support the variant.
        CharString ਮਾਪਾ = ulocimp_get_ਮਾਪਾ(locale, *ec);
        *ec = U_USING_FALLBACK_WARNING;
        // TODO: Loop over the parent rather than recursing and
        // looking again for a currency keyword.
        ਵਾਪਸ ucurr_forLocale(ਮਾਪਾ.ਅੰਕੜੇ(), buff, buff_ਸਮਰੱਥਾ, ec);
    }
    ਜੇ (*ec == U_ZERO_ERROR || local_ਸਥਿਤੀ != U_ZERO_ERROR) {
        // There is nothing to fallback to. Report the failure/warning if possible.
        *ec = local_ਸਥਿਤੀ;
    }
    ਜੇ (U_SUCCESS(*ec)) {
        ਜੇ(buff_ਸਮਰੱਥਾ > resLen) {
            u_strcpy(buff, s);
        }
    }
    ਵਾਪਸ u_terminateUChars(buff, buff_ਸਮਰੱਥਾ, resLen, ec);
}

// end registration

/**
 * Modify the given locale name by removing the rightmost _-delimited
 * element.  If there is none, empty the string ("" == root).
 * NOTE: The string "root" is not recognized; do not use it.
 * @return true if the fallback happened; false if locale is already
 * root ("").
 */
ਸਥਿਰ UBool fallback(CharString& loc) {
    ਜੇ (loc.is_ਖਾਲੀ_ਥਾਵਾਂ()) {
        ਵਾਪਸ ਝੂਠ;
    }
    UError_ਕੋਡ ਸਥਿਤੀ = U_ZERO_ERROR;
    ਜੇ (loc == "en_GB") {
        // HACK: See #13368.  We need "en_GB" to fall back to "en_001" instead of "en"
        // in order to consume the correct data strings.  This hack will be removed
        // when proper data sink loading is implemented here.
        loc.truncate(3);
        loc.append("001", ਸਥਿਤੀ);
    } ਨਹੀਂਤਾਂ {
        loc = ulocimp_get_ਮਾਪਾ(loc.ਅੰਕੜੇ(), ਸਥਿਤੀ);
    }
 /*
    char *i = uprv_strrchr(loc, '_');
    if (i == nullptr) {
        i = loc;
    }
    *i = 0;
 */
    ਵਾਪਸ ਸੱਚ;
}


U_CAPI ਅਚਲ char16_t* U_EXPORT2
ucurr_getName(ਅਚਲ char16_t* currency U_LIFETIME_BOUND,
              ਅਚਲ ਅੱਖਰ* locale,
              UCurrNameStyle nameStyle,
              UBool* isChoiceFormat, // fillin
              int32_t* len, // fillin
              UError_ਕੋਡ* ec) {

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

    ਜੇ (U_FAILURE(*ec)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    int32_t choice = (int32_t) nameStyle;
    ਜੇ (choice < 0 || choice > 4) {
        *ec = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
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
    UError_ਕੋਡ ec2 = U_ZERO_ERROR;

    ਜੇ (locale == ਖਾਲੀ_ਸੂਚਕ) {
        locale = uloc_getDefault();
    }
    CharString loc = ulocimp_getName(locale, ec2);
    ਜੇ (U_FAILURE(ec2)) {
        *ec = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    ਅੱਖਰ buf[ISO_CURRENCY_ਕੋਡ_ਲੰਮਾਈ+1];
    my_UChars_ਮੰਜ਼ਿਲ_Chars(buf, currency);
    
    /* Normalize the keyword value to uppercase */
    T_CString_ਮੰਜ਼ਿਲ_ਉੱਤਲਾ_Case(buf);
    
    ਅਚਲ char16_t* s = ਖਾਲੀ_ਸੂਚਕ;
    ec2 = U_ZERO_ERROR;
    Local_UResource_Bundle_ਸੂਚਕ rb(ures_open(U_ICUDATA_CURR, loc.ਅੰਕੜੇ(), &ec2));

    ਜੇ (nameStyle == UCURR_NARROW_ਚਿੰਨ੍ਹ_NAME || nameStyle == UCURR_FORMAL_ਚਿੰਨ੍ਹ_NAME || nameStyle == UCURR_VARIANT_ਚਿੰਨ੍ਹ_NAME) {
        CharString ਕੁੰਜੀ;
        ਚੋਣ (nameStyle) {
        ਹਾਲਤ UCURR_NARROW_ਚਿੰਨ੍ਹ_NAME:
            ਕੁੰਜੀ.append(CURRENCIES_NARROW, ec2);
            ਰੋਕੋ;
        ਹਾਲਤ UCURR_FORMAL_ਚਿੰਨ੍ਹ_NAME:
            ਕੁੰਜੀ.append(CURRENCIES_FORMAL, ec2);
            ਰੋਕੋ;
        ਹਾਲਤ UCURR_VARIANT_ਚਿੰਨ੍ਹ_NAME:
            ਕੁੰਜੀ.append(CURRENCIES_VARIANT, ec2);
            ਰੋਕੋ;
        ਮੂਲ:
            *ec = U_UNSUPPORTED_ERROR;
            ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
        }
        ਕੁੰਜੀ.append("/", ec2);
        ਕੁੰਜੀ.append(buf, ec2);
        s = ures_get_String_By_ਕੁੰਜੀ_With_Fallback(rb.getAlias(), ਕੁੰਜੀ.ਅੰਕੜੇ(), len, &ec2);
        ਜੇ (ec2 == U_MISSING_RESOURCE_ERROR) {
            *ec = U_USING_FALLBACK_WARNING;
            ec2 = U_ZERO_ERROR;
            choice = UCURR_ਚਿੰਨ੍ਹ_NAME;
        }
    }
    ਜੇ (s == ਖਾਲੀ_ਸੂਚਕ) {
        ures_get_By_ਕੁੰਜੀ(rb.getAlias(), CURRENCIES, rb.getAlias(), &ec2);
        ures_get_By_ਕੁੰਜੀ_With_Fallback(rb.getAlias(), buf, rb.getAlias(), &ec2);
        s = ures_get_String_By_ਸੂਚਕਾਂਕ(rb.getAlias(), choice, len, &ec2);
    }

    // If we've succeeded we're done.  Otherwise, try to fallback.
    // If that fails (because we are already at root) then exit.
    ਜੇ (U_SUCCESS(ec2)) {
        ਜੇ (ec2 == U_USING_DEFAULT_WARNING
            || (ec2 == U_USING_FALLBACK_WARNING && *ec != U_USING_DEFAULT_WARNING)) {
            *ec = ec2;
        }
    }

    // We no longer support choice format data in names.  Data should not contain
    // choice patterns.
    ਜੇ (isChoiceFormat != ਖਾਲੀ_ਸੂਚਕ) {
        *isChoiceFormat = ਝੂਠ;
    }
    ਜੇ (U_SUCCESS(ec2)) {
        U_ASSERT(s != ਖਾਲੀ_ਸੂਚਕ);
        ਵਾਪਸ s;
    }

    // If we fail to find a match, use the ISO 4217 code
    *len = u_strlen(currency); // Should == ISO_CURRENCY_CODE_LENGTH, but maybe not...?
    *ec = U_USING_DEFAULT_WARNING;
    ਵਾਪਸ currency;
}

U_CAPI ਅਚਲ char16_t* U_EXPORT2
ucurr_getPluralName(ਅਚਲ char16_t* currency U_LIFETIME_BOUND,
                    ਅਚਲ ਅੱਖਰ* locale,
                    UBool* isChoiceFormat,
                    ਅਚਲ ਅੱਖਰ* plural_ਗਿਣਤੀ,
                    int32_t* len, // fillin
                    UError_ਕੋਡ* ec) {
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

    ਜੇ (U_FAILURE(*ec)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    // Use a separate UErrorCode here that does not propagate out of
    // this function.
    UError_ਕੋਡ ec2 = U_ZERO_ERROR;

    ਜੇ (locale == ਖਾਲੀ_ਸੂਚਕ) {
        locale = uloc_getDefault();
    }
    CharString loc = ulocimp_getName(locale, ec2);
    ਜੇ (U_FAILURE(ec2)) {
        *ec = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    ਅੱਖਰ buf[ISO_CURRENCY_ਕੋਡ_ਲੰਮਾਈ+1];
    my_UChars_ਮੰਜ਼ਿਲ_Chars(buf, currency);

    ਅਚਲ char16_t* s = ਖਾਲੀ_ਸੂਚਕ;
    ec2 = U_ZERO_ERROR;
    UResourceBundle* rb = ures_open(U_ICUDATA_CURR, loc.ਅੰਕੜੇ(), &ec2);

    rb = ures_get_By_ਕੁੰਜੀ(rb, CURRENCYPLURALS, rb, &ec2);

    // Fetch resource with multi-level resource inheritance fallback
    Local_UResource_Bundle_ਸੂਚਕ curr(ures_get_By_ਕੁੰਜੀ_With_Fallback(rb, buf, rb, &ec2));

    s = ures_get_String_By_ਕੁੰਜੀ_With_Fallback(curr.getAlias(), plural_ਗਿਣਤੀ, len, &ec2);
    ਜੇ (U_FAILURE(ec2)) {
        //  fall back to "other"
        ec2 = U_ZERO_ERROR;
        s = ures_get_String_By_ਕੁੰਜੀ_With_Fallback(curr.getAlias(), "other", len, &ec2);     
        ਜੇ (U_FAILURE(ec2)) {
            // fall back to long name in Currencies
            ਵਾਪਸ ucurr_getName(currency, locale, UCURR_LONG_NAME, 
                                 isChoiceFormat, len, ec);
        }
    }

    // If we've succeeded we're done.  Otherwise, try to fallback.
    // If that fails (because we are already at root) then exit.
    ਜੇ (U_SUCCESS(ec2)) {
        ਜੇ (ec2 == U_USING_DEFAULT_WARNING
            || (ec2 == U_USING_FALLBACK_WARNING && *ec != U_USING_DEFAULT_WARNING)) {
            *ec = ec2;
        }
        U_ASSERT(s != ਖਾਲੀ_ਸੂਚਕ);
        ਵਾਪਸ s;
    }

    // If we fail to find a match, use the ISO 4217 code
    *len = u_strlen(currency); // Should == ISO_CURRENCY_CODE_LENGTH, but maybe not...?
    *ec = U_USING_DEFAULT_WARNING;
    ਵਾਪਸ currency;
}


//========================================================================
// Following are structure and function for parsing currency names

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NEED_ਮੰਜ਼ਿਲ_BE_DELETED 0x1

// TODO: a better way to define this?
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_CURRENCY_NAME_LEN 100

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    ਅਚਲ ਅੱਖਰ* Iso_ਕੋਡ;  //ਕੁੰਜੀ
    char16_t* currencyName;  //ਮੁੱਲ
    int32_t currencyNameLen;  // value length
    int32_t flag;  // flags
} CurrencyNameStruct;


#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ MIN
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MIN(a,b) (((a)<(b)) ? (a) : (b))
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ MAX
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX(a,b) (((a)<(b)) ? (b) : (a))
#ਜੇ_ਅੰਤ


// Comparison function used in quick sort.
ਸਥਿਰ ਪੂਰਨ U_CALLCONV currencyNameComparator(ਅਚਲ ਖਾਲੀ* a, ਅਚਲ ਖਾਲੀ* b) {
    ਅਚਲ CurrencyNameStruct* currName_1 = static_cast<ਅਚਲ CurrencyNameStruct*>(a);
    ਅਚਲ CurrencyNameStruct* currName_2 = static_cast<ਅਚਲ CurrencyNameStruct*>(b);
    ਲਈ (int32_t i = 0; 
         i < MIN(currName_1->currencyNameLen, currName_2->currencyNameLen);
         ++i) {
        ਜੇ (currName_1->currencyName[i] < currName_2->currencyName[i]) {
            ਵਾਪਸ -1;
        }
        ਜੇ (currName_1->currencyName[i] > currName_2->currencyName[i]) {
            ਵਾਪਸ 1;
        }
    }
    ਜੇ (currName_1->currencyNameLen < currName_2->currencyNameLen) {
        ਵਾਪਸ -1;
    } ਨਹੀਂਤਾਂ ਜੇ (currName_1->currencyNameLen > currName_2->currencyNameLen) {
        ਵਾਪਸ 1;
    }
    ਵਾਪਸ 0;
}


// Give a locale, return the maximum number of currency names associated with
// this locale.
// It gets currency names from resource bundles using fallback.
// It is the maximum number because in the fallback chain, some of the 
// currency names are duplicated.
// For example, given locale as "en_US", the currency names get from resource
// bundle in "en_US" and "en" are duplicated. The fallback mechanism will count
// all currency names in "en_US" and "en".
ਸਥਿਰ ਖਾਲੀ
get_Currency_Name_ਗਿਣਤੀ(ਅਚਲ ਅੱਖਰ* loc, int32_t* ਕੁੱਲ_currency_name_ਗਿਣਤੀ, int32_t* ਕੁੱਲ_currency_ਚਿੰਨ੍ਹ_ਗਿਣਤੀ) {
    U_NAMESPACE_USE
    *ਕੁੱਲ_currency_name_ਗਿਣਤੀ = 0;
    *ਕੁੱਲ_currency_ਚਿੰਨ੍ਹ_ਗਿਣਤੀ = 0;
    ਅਚਲ char16_t* s = ਖਾਲੀ_ਸੂਚਕ;
    CharString locale;
    {
        UError_ਕੋਡ ਸਥਿਤੀ = U_ZERO_ERROR;
        locale.append(loc, ਸਥਿਤੀ);
        ਜੇ (U_FAILURE(ਸਥਿਤੀ)) { ਵਾਪਸ; }
    }
    ਅਚਲ icu::Hashtable *currencySymbolsEquiv = getCurrSymbolsEquiv();
    ਲਈ (;;) {
        UError_ਕੋਡ ec2 = U_ZERO_ERROR;
        // TODO: ures_openDirect?
        Local_UResource_Bundle_ਸੂਚਕ rb(ures_open(U_ICUDATA_CURR, locale.ਅੰਕੜੇ(), &ec2));
        Local_UResource_Bundle_ਸੂਚਕ curr(ures_get_By_ਕੁੰਜੀ(rb.getAlias(), CURRENCIES, ਖਾਲੀ_ਸੂਚਕ, &ec2));
        int32_t n = ures_get_ਮਾਪ(curr.getAlias());
        ਲਈ (int32_t i=0; i<n; ++i) {
            Local_UResource_Bundle_ਸੂਚਕ ਨਾਂ(ures_get_By_ਸੂਚਕਾਂਕ(curr.getAlias(), i, ਖਾਲੀ_ਸੂਚਕ, &ec2));
            int32_t len;
            s = ures_get_String_By_ਸੂਚਕਾਂਕ(ਨਾਂ.getAlias(), UCURR_ਚਿੰਨ੍ਹ_NAME, &len, &ec2);
            ++(*ਕੁੱਲ_currency_ਚਿੰਨ੍ਹ_ਗਿਣਤੀ);  // currency symbol
            ਜੇ (currencySymbolsEquiv != ਖਾਲੀ_ਸੂਚਕ) {
                *ਕੁੱਲ_currency_ਚਿੰਨ੍ਹ_ਗਿਣਤੀ += ਗਿਣਤੀ_Equivalent(*currencySymbolsEquiv, UnicodeString(ਸੱਚ, s, len));
            }
            ++(*ਕੁੱਲ_currency_ਚਿੰਨ੍ਹ_ਗਿਣਤੀ); // iso code
            ++(*ਕੁੱਲ_currency_name_ਗਿਣਤੀ); // long name
        }

        // currency plurals
        UError_ਕੋਡ ec3 = U_ZERO_ERROR;
        Local_UResource_Bundle_ਸੂਚਕ curr_p(ures_get_By_ਕੁੰਜੀ(rb.getAlias(), CURRENCYPLURALS, ਖਾਲੀ_ਸੂਚਕ, &ec3));
        n = ures_get_ਮਾਪ(curr_p.getAlias());
        ਲਈ (int32_t i=0; i<n; ++i) {
            Local_UResource_Bundle_ਸੂਚਕ ਨਾਂ(ures_get_By_ਸੂਚਕਾਂਕ(curr_p.getAlias(), i, ਖਾਲੀ_ਸੂਚਕ, &ec3));
            *ਕੁੱਲ_currency_name_ਗਿਣਤੀ += ures_get_ਮਾਪ(ਨਾਂ.getAlias());
        }

        ਜੇ (!fallback(locale)) {
            ਰੋਕੋ;
        }
    }
}

ਸਥਿਰ char16_t*
ਮੰਜ਼ਿਲ_ਉੱਤਲਾ_Case(ਅਚਲ char16_t* ਸਰੋਤ, int32_t len, ਅਚਲ ਅੱਖਰ* locale) {
    char16_t* dest = ਖਾਲੀ_ਸੂਚਕ;
    UError_ਕੋਡ ec = U_ZERO_ERROR;
    int32_t destLen = u_str_ਮੰਜ਼ਿਲ_ਉੱਤਲਾ(dest, 0, ਸਰੋਤ, len, locale, &ec);

    ec = U_ZERO_ERROR;
    dest = static_cast<char16_t*>(uprv_malloc(ਆਕਾਰ(char16_t) * MAX(destLen, len)));
    ਜੇ (dest == ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    u_str_ਮੰਜ਼ਿਲ_ਉੱਤਲਾ(dest, destLen, ਸਰੋਤ, len, locale, &ec);
    ਜੇ (U_FAILURE(ec)) {
        u_memcpy(dest, ਸਰੋਤ, len);
    } 
    ਵਾਪਸ dest;
}


ਸਥਿਰ ਖਾਲੀ delete_Currency_ਨਾਂ(CurrencyNameStruct* currency_ਨਾਂ, int32_t count);
// Collect all available currency names associated with the given locale
// (enable fallback chain).
// Read currenc names defined in resource bundle "Currencies" and
// "CurrencyPlural", enable fallback chain.
// return the malloc-ed currency name arrays and the total number of currency
// names in the array.
ਸਥਿਰ ਖਾਲੀ
collect_Currency_ਨਾਂ(ਅਚਲ ਅੱਖਰ* locale, 
                     CurrencyNameStruct** currency_ਨਾਂ, 
                     int32_t* ਕੁੱਲ_currency_name_ਗਿਣਤੀ, 
                     CurrencyNameStruct** currencySymbols, 
                     int32_t* ਕੁੱਲ_currency_ਚਿੰਨ੍ਹ_ਗਿਣਤੀ, 
                     UError_ਕੋਡ& ec) {
    ਜੇ (U_FAILURE(ec)) {
        *currency_ਨਾਂ = *currencySymbols = ਖਾਲੀ_ਸੂਚਕ;
        *ਕੁੱਲ_currency_name_ਗਿਣਤੀ = *ਕੁੱਲ_currency_ਚਿੰਨ੍ਹ_ਗਿਣਤੀ = 0;
        ਵਾਪਸ;
    }
    U_NAMESPACE_USE
    ਅਚਲ icu::Hashtable *currencySymbolsEquiv = getCurrSymbolsEquiv();
    // Look up the Currencies resource for the given locale.
    UError_ਕੋਡ ec2 = U_ZERO_ERROR;

    ਜੇ (locale == ਖਾਲੀ_ਸੂਚਕ) {
        locale = uloc_getDefault();
    }
    CharString loc = ulocimp_getName(locale, ec2);
    ਜੇ (U_FAILURE(ec2)) {
        ec = U_ILLEGAL_ਦਲੀਲ_ERROR;
        *currency_ਨਾਂ = *currencySymbols = ਖਾਲੀ_ਸੂਚਕ;
        *ਕੁੱਲ_currency_name_ਗਿਣਤੀ = *ਕੁੱਲ_currency_ਚਿੰਨ੍ਹ_ਗਿਣਤੀ = 0;
        ਵਾਪਸ;
    }

    // Get maximum currency name count first.
    get_Currency_Name_ਗਿਣਤੀ(loc.ਅੰਕੜੇ(), ਕੁੱਲ_currency_name_ਗਿਣਤੀ, ਕੁੱਲ_currency_ਚਿੰਨ੍ਹ_ਗਿਣਤੀ);

    *currency_ਨਾਂ = static_cast<CurrencyNameStruct*>(
        uprv_malloc(ਆਕਾਰ(CurrencyNameStruct) * (*ਕੁੱਲ_currency_name_ਗਿਣਤੀ)));
    ਜੇ(*currency_ਨਾਂ == ਖਾਲੀ_ਸੂਚਕ) {
        *currencySymbols = ਖਾਲੀ_ਸੂਚਕ;
        *ਕੁੱਲ_currency_name_ਗਿਣਤੀ = *ਕੁੱਲ_currency_ਚਿੰਨ੍ਹ_ਗਿਣਤੀ = 0;
        ec = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ;
    }
    *currencySymbols = static_cast<CurrencyNameStruct*>(
        uprv_malloc(ਆਕਾਰ(CurrencyNameStruct) * (*ਕੁੱਲ_currency_ਚਿੰਨ੍ਹ_ਗਿਣਤੀ)));

    ਜੇ(*currencySymbols == ਖਾਲੀ_ਸੂਚਕ) {
        uprv_free(*currency_ਨਾਂ);
        *currency_ਨਾਂ = ਖਾਲੀ_ਸੂਚਕ;
        *ਕੁੱਲ_currency_name_ਗਿਣਤੀ = *ਕੁੱਲ_currency_ਚਿੰਨ੍ਹ_ਗਿਣਤੀ = 0;
        ec = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ;
    }

    ਅਚਲ char16_t* s = ਖਾਲੀ_ਸੂਚਕ;  // currency name
    ਅੱਖਰ* iso = ਖਾਲੀ_ਸੂਚਕ;  // currency ISO code

    *ਕੁੱਲ_currency_name_ਗਿਣਤੀ = 0;
    *ਕੁੱਲ_currency_ਚਿੰਨ੍ਹ_ਗਿਣਤੀ = 0;

    UError_ਕੋਡ ec3 = U_ZERO_ERROR;
    UError_ਕੋਡ ec4 = U_ZERO_ERROR;

    // Using hash to remove duplicates caused by locale fallback
    Local_UHashtable_ਸੂਚਕ currencyIsoCodes(uhash_open(uhash_hashChars, uhash_compareChars, ਖਾਲੀ_ਸੂਚਕ, &ec3));
    Local_UHashtable_ਸੂਚਕ currencyPluralIsoCodes(uhash_open(uhash_hashChars, uhash_compareChars, ਖਾਲੀ_ਸੂਚਕ, &ec4));
    ਲਈ (int32_t localeLevel = 0; ; ++localeLevel) {
        ec2 = U_ZERO_ERROR;
        // TODO: ures_openDirect
        Local_UResource_Bundle_ਸੂਚਕ rb(ures_open(U_ICUDATA_CURR, loc.ਅੰਕੜੇ(), &ec2));
        Local_UResource_Bundle_ਸੂਚਕ curr(ures_get_By_ਕੁੰਜੀ(rb.getAlias(), CURRENCIES, ਖਾਲੀ_ਸੂਚਕ, &ec2));
        int32_t n = ures_get_ਮਾਪ(curr.getAlias());
        ਲਈ (int32_t i=0; i<n; ++i) {
            Local_UResource_Bundle_ਸੂਚਕ ਨਾਂ(ures_get_By_ਸੂਚਕਾਂਕ(curr.getAlias(), i, ਖਾਲੀ_ਸੂਚਕ, &ec2));
            int32_t len;
            s = ures_get_String_By_ਸੂਚਕਾਂਕ(ਨਾਂ.getAlias(), UCURR_ਚਿੰਨ੍ਹ_NAME, &len, &ec2);
            // TODO: uhash_put wont change key/value?
            iso = const_cast<ਅੱਖਰ*>(ures_get_ਕੁੰਜੀ(ਨਾਂ.getAlias()));
            ਜੇ (localeLevel != 0 && uhash_get(currencyIsoCodes.getAlias(), iso) != ਖਾਲੀ_ਸੂਚਕ) {
                ਜਾਰੀ;
            }
            uhash_put(currencyIsoCodes.getAlias(), iso, iso, &ec3); 
            // Add currency symbol.
            (*currencySymbols)[(*ਕੁੱਲ_currency_ਚਿੰਨ੍ਹ_ਗਿਣਤੀ)++] = {iso, const_cast<char16_t*>(s), len, 0};

            // Add equivalent symbols
            ਜੇ (currencySymbolsEquiv != ਖਾਲੀ_ਸੂਚਕ) {
                UnicodeString str(ਸੱਚ, s, len);
                icu::EquivIterator iter(*currencySymbolsEquiv, str);
                ਅਚਲ UnicodeString *ਚਿੰਨ੍ਹ;
                ਜਦੋਂ ((ਚਿੰਨ੍ਹ = iter.next()) != ਖਾਲੀ_ਸੂਚਕ) {
                    (*currencySymbols)[(*ਕੁੱਲ_currency_ਚਿੰਨ੍ਹ_ਗਿਣਤੀ)++]
                        = {iso, const_cast<char16_t*>(ਚਿੰਨ੍ਹ->get_ਬਫਰ()), ਚਿੰਨ੍ਹ->length(), 0};
                }
            }

            // Add currency long name.
            s = ures_get_String_By_ਸੂਚਕਾਂਕ(ਨਾਂ.getAlias(), UCURR_LONG_NAME, &len, &ec2);
            char16_t* ਉੱਤਲਾ_Name = ਮੰਜ਼ਿਲ_ਉੱਤਲਾ_Case(s, len, locale);
            ਜੇ (ਉੱਤਲਾ_Name == ਖਾਲੀ_ਸੂਚਕ) {
                ec = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
                ਜਾਓ error;
            }
            (*currency_ਨਾਂ)[(*ਕੁੱਲ_currency_name_ਗਿਣਤੀ)++] = {iso, ਉੱਤਲਾ_Name, len, NEED_ਮੰਜ਼ਿਲ_BE_DELETED};

            // put (iso, 3, and iso) in to array
            // Add currency ISO code.
            char16_t* iso_ਕੋਡ = static_cast<char16_t*>(uprv_malloc(ਆਕਾਰ(char16_t) * 3));
            ਜੇ (iso_ਕੋਡ == ਖਾਲੀ_ਸੂਚਕ) {
                ec = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
                ਜਾਓ error;
            }
            // Must convert iso[] into Unicode
            u_chars_ਮੰਜ਼ਿਲ_UChars(iso, iso_ਕੋਡ, 3);
            (*currencySymbols)[(*ਕੁੱਲ_currency_ਚਿੰਨ੍ਹ_ਗਿਣਤੀ)++] = {iso, iso_ਕੋਡ, 3, NEED_ਮੰਜ਼ਿਲ_BE_DELETED};
        }

        // currency plurals
        UError_ਕੋਡ ec5 = U_ZERO_ERROR;
        Local_UResource_Bundle_ਸੂਚਕ curr_p(ures_get_By_ਕੁੰਜੀ(rb.getAlias(), CURRENCYPLURALS, ਖਾਲੀ_ਸੂਚਕ, &ec5));
        n = ures_get_ਮਾਪ(curr_p.getAlias());
        ਲਈ (int32_t i=0; i<n; ++i) {
            Local_UResource_Bundle_ਸੂਚਕ ਨਾਂ(ures_get_By_ਸੂਚਕਾਂਕ(curr_p.getAlias(), i, ਖਾਲੀ_ਸੂਚਕ, &ec5));
            iso = const_cast<ਅੱਖਰ*>(ures_get_ਕੁੰਜੀ(ਨਾਂ.getAlias()));
            // Using hash to remove duplicated ISO codes in fallback chain.
            ਜੇ (localeLevel != 0 && uhash_get(currencyPluralIsoCodes.getAlias(), iso) != ਖਾਲੀ_ਸੂਚਕ) {
                ਜਾਰੀ;
            }
            uhash_put(currencyPluralIsoCodes.getAlias(), iso, iso, &ec4);
            int32_t num = ures_get_ਮਾਪ(ਨਾਂ.getAlias());
            int32_t len;
            ਲਈ (int32_t j = 0; j < num; ++j) {
                // TODO: remove duplicates between singular name and 
                // currency long name?
                s = ures_get_String_By_ਸੂਚਕਾਂਕ(ਨਾਂ.getAlias(), j, &len, &ec5);
                char16_t* ਉੱਤਲਾ_Name = ਮੰਜ਼ਿਲ_ਉੱਤਲਾ_Case(s, len, locale);
                ਜੇ (ਉੱਤਲਾ_Name == ਖਾਲੀ_ਸੂਚਕ) {
                    ec = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
                    ਜਾਓ error;
                }
                (*currency_ਨਾਂ)[(*ਕੁੱਲ_currency_name_ਗਿਣਤੀ)++] = {iso, ਉੱਤਲਾ_Name, len, NEED_ਮੰਜ਼ਿਲ_BE_DELETED};
            }
        }

        ਜੇ (!fallback(loc)) {
            ਰੋਕੋ;
        }
    }

    // quick sort the struct
    qsort(*currency_ਨਾਂ, *ਕੁੱਲ_currency_name_ਗਿਣਤੀ, 
          ਆਕਾਰ(CurrencyNameStruct), currencyNameComparator);
    qsort(*currencySymbols, *ਕੁੱਲ_currency_ਚਿੰਨ੍ਹ_ਗਿਣਤੀ, 
          ਆਕਾਰ(CurrencyNameStruct), currencyNameComparator);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UCURR_DEBUG
    ਲਿਖੋ("currency name count: %d\n", *ਕੁੱਲ_currency_name_ਗਿਣਤੀ);
    ਲਈ (int32_t ਸੂਚਕਾਂਕ = 0; ਸੂਚਕਾਂਕ < *ਕੁੱਲ_currency_name_ਗਿਣਤੀ; ++ਸੂਚਕਾਂਕ) {
        ਲਿਖੋ("index: %d\n", ਸੂਚਕਾਂਕ);
        ਲਿਖੋ("iso: %s\n", (*currency_ਨਾਂ)[ਸੂਚਕਾਂਕ].Iso_ਕੋਡ);
        ਅੱਖਰ curNameBuf[1024];
        memset(curNameBuf, 0, 1024);
        u_austrncpy(curNameBuf, (*currency_ਨਾਂ)[ਸੂਚਕਾਂਕ].currencyName, (*currency_ਨਾਂ)[ਸੂਚਕਾਂਕ].currencyNameLen);
        ਲਿਖੋ("currencyName: %s\n", curNameBuf);
        ਲਿਖੋ("len: %d\n", (*currency_ਨਾਂ)[ਸੂਚਕਾਂਕ].currencyNameLen);
    }
    ਲਿਖੋ("currency symbol count: %d\n", *ਕੁੱਲ_currency_ਚਿੰਨ੍ਹ_ਗਿਣਤੀ);
    ਲਈ (int32_t ਸੂਚਕਾਂਕ = 0; ਸੂਚਕਾਂਕ < *ਕੁੱਲ_currency_ਚਿੰਨ੍ਹ_ਗਿਣਤੀ; ++ਸੂਚਕਾਂਕ) {
        ਲਿਖੋ("index: %d\n", ਸੂਚਕਾਂਕ);
        ਲਿਖੋ("iso: %s\n", (*currencySymbols)[ਸੂਚਕਾਂਕ].Iso_ਕੋਡ);
        ਅੱਖਰ curNameBuf[1024];
        memset(curNameBuf, 0, 1024);
        u_austrncpy(curNameBuf, (*currencySymbols)[ਸੂਚਕਾਂਕ].currencyName, (*currencySymbols)[ਸੂਚਕਾਂਕ].currencyNameLen);
        ਲਿਖੋ("currencySymbol: %s\n", curNameBuf);
        ਲਿਖੋ("len: %d\n", (*currencySymbols)[ਸੂਚਕਾਂਕ].currencyNameLen);
    }
#ਜੇ_ਅੰਤ
    // fail on hashtable errors
    ਜੇ (U_FAILURE(ec3)) {
      ec = ec3;
    } ਨਹੀਂਤਾਂ ਜੇ (U_FAILURE(ec4)) {
      ec = ec4;
    }

  error:
    // clean up if we got error
    ਜੇ (U_FAILURE(ec)) {
        delete_Currency_ਨਾਂ(*currency_ਨਾਂ, *ਕੁੱਲ_currency_name_ਗਿਣਤੀ);
        delete_Currency_ਨਾਂ(*currencySymbols, *ਕੁੱਲ_currency_ਚਿੰਨ੍ਹ_ਗਿਣਤੀ);
        *currency_ਨਾਂ = *currencySymbols = ਖਾਲੀ_ਸੂਚਕ;
        *ਕੁੱਲ_currency_name_ਗਿਣਤੀ = *ਕੁੱਲ_currency_ਚਿੰਨ੍ਹ_ਗਿਣਤੀ = 0;
    }
}

// @param  currencyNames: currency names array
// @param  indexInCurrencyNames: the index of the character in currency names 
//         array against which the comparison is done
// @param  key: input text char to compare against
// @param  begin(IN/OUT): the begin index of matching range in currency names array
// @param  end(IN/OUT): the end index of matching range in currency names array.
ਸਥਿਰ int32_t
ਦੁਆਧਾਰੀ_ਖੋਜ(ਅਚਲ CurrencyNameStruct* currency_ਨਾਂ, 
             int32_t ਸੂਚਕਾਂਕ_ਵਿੱਚ_Currency_ਨਾਂ,
             ਅਚਲ char16_t ਕੁੰਜੀ,
             int32_t* begin, int32_t* ਸਿਰਾ) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UCURR_DEBUG
    ਲਿਖੋ("key = %x\n", ਕੁੰਜੀ);
#ਜੇ_ਅੰਤ
   int32_t first = *begin;
   int32_t ਆਖਰੀ = *ਸਿਰਾ;
   ਜਦੋਂ (first <= ਆਖਰੀ) {
       int32_t mid = (first + ਆਖਰੀ) / 2;  // compute mid point.
       ਜੇ (ਸੂਚਕਾਂਕ_ਵਿੱਚ_Currency_ਨਾਂ >= currency_ਨਾਂ[mid].currencyNameLen) {
           first = mid + 1;
       } ਨਹੀਂਤਾਂ {
           ਜੇ (ਕੁੰਜੀ > currency_ਨਾਂ[mid].currencyName[ਸੂਚਕਾਂਕ_ਵਿੱਚ_Currency_ਨਾਂ]) {
               first = mid + 1;
           }
           ਨਹੀਂਤਾਂ ਜੇ (ਕੁੰਜੀ < currency_ਨਾਂ[mid].currencyName[ਸੂਚਕਾਂਕ_ਵਿੱਚ_Currency_ਨਾਂ]) {
               ਆਖਰੀ = mid - 1;
           }
           ਨਹੀਂਤਾਂ {
                // Find a match, and looking for ranges
                // Now do two more binary searches. First, on the left side for
                // the greatest L such that CurrencyNameStruct[L] < key.
                int32_t L = *begin;
                int32_t R = mid;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UCURR_DEBUG
                ਲਿਖੋ("mid = %d\n", mid);
#ਜੇ_ਅੰਤ
                ਜਦੋਂ (L < R) {
                    int32_t M = (L + R) / 2;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UCURR_DEBUG
                    ਲਿਖੋ("L = %d, R = %d, M = %d\n", L, R, M);
#ਜੇ_ਅੰਤ
                    ਜੇ (ਸੂਚਕਾਂਕ_ਵਿੱਚ_Currency_ਨਾਂ >= currency_ਨਾਂ[M].currencyNameLen) {
                        L = M + 1;
                    } ਨਹੀਂਤਾਂ {
                        ਜੇ (currency_ਨਾਂ[M].currencyName[ਸੂਚਕਾਂਕ_ਵਿੱਚ_Currency_ਨਾਂ] < ਕੁੰਜੀ) {
                            L = M + 1;
                        } ਨਹੀਂਤਾਂ {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UCURR_DEBUG
                            U_ASSERT(currency_ਨਾਂ[M].currencyName[ਸੂਚਕਾਂਕ_ਵਿੱਚ_Currency_ਨਾਂ] == ਕੁੰਜੀ);
#ਜੇ_ਅੰਤ
                            R = M;
                        }
                    }
                }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UCURR_DEBUG
                U_ASSERT(L == R);
#ਜੇ_ਅੰਤ
                *begin = L;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UCURR_DEBUG
                ਲਿਖੋ("begin = %d\n", *begin);
                U_ASSERT(currency_ਨਾਂ[*begin].currencyName[ਸੂਚਕਾਂਕ_ਵਿੱਚ_Currency_ਨਾਂ] == ਕੁੰਜੀ);
#ਜੇ_ਅੰਤ

                // Now for the second search, finding the least R such that
                // key < CurrencyNameStruct[R].
                L = mid;
                R = *ਸਿਰਾ;
                ਜਦੋਂ (L < R) {
                    int32_t M = (L + R) / 2;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UCURR_DEBUG
                    ਲਿਖੋ("L = %d, R = %d, M = %d\n", L, R, M);
#ਜੇ_ਅੰਤ
                    ਜੇ (currency_ਨਾਂ[M].currencyNameLen < ਸੂਚਕਾਂਕ_ਵਿੱਚ_Currency_ਨਾਂ) {
                        L = M + 1;
                    } ਨਹੀਂਤਾਂ {
                        ਜੇ (currency_ਨਾਂ[M].currencyName[ਸੂਚਕਾਂਕ_ਵਿੱਚ_Currency_ਨਾਂ] > ਕੁੰਜੀ) {
                            R = M;
                        } ਨਹੀਂਤਾਂ {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UCURR_DEBUG
                            U_ASSERT(currency_ਨਾਂ[M].currencyName[ਸੂਚਕਾਂਕ_ਵਿੱਚ_Currency_ਨਾਂ] == ਕੁੰਜੀ);
#ਜੇ_ਅੰਤ
                            L = M + 1;
                        }
                    }
                }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UCURR_DEBUG
                U_ASSERT(L == R);
#ਜੇ_ਅੰਤ
                ਜੇ (currency_ਨਾਂ[R].currencyName[ਸੂਚਕਾਂਕ_ਵਿੱਚ_Currency_ਨਾਂ] > ਕੁੰਜੀ) {
                    *ਸਿਰਾ = R - 1;
                } ਨਹੀਂਤਾਂ {
                    *ਸਿਰਾ = R;
                }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UCURR_DEBUG
                ਲਿਖੋ("end = %d\n", *ਸਿਰਾ);
#ਜੇ_ਅੰਤ

                // now, found the range. check whether there is exact match
                ਜੇ (currency_ਨਾਂ[*begin].currencyNameLen == ਸੂਚਕਾਂਕ_ਵਿੱਚ_Currency_ਨਾਂ + 1) {
                    ਵਾਪਸ *begin;  // find range and exact match.
                }
                ਵਾਪਸ -1;  // find range, but no exact match.
           }
       }
   }
   *begin = -1;
   *ਸਿਰਾ = -1;
   ਵਾਪਸ -1;    // failed to find range.
}


// Linear search "text" in "currencyNames".
// @param  begin, end: the begin and end index in currencyNames, within which
//         range should the search be performed.
// @param  textLen: the length of the text to be compared
// @param  maxMatchLen(IN/OUT): passing in the computed max matching length
//                              pass out the new max  matching length
// @param  maxMatchIndex: the index in currencyName which has the longest
//                        match with input text.
ਸਥਿਰ ਖਾਲੀ
ਰੇਖਿਕ_ਖੋਜ(ਅਚਲ CurrencyNameStruct* currency_ਨਾਂ, 
             int32_t begin, int32_t ਸਿਰਾ,
             ਅਚਲ char16_t* ਪਾਠ, int32_t ਪਾਠ_Len,
             int32_t *partial_ਮੇਲ_Len,
             int32_t *max_ਮੇਲ_Len, int32_t* max_ਮੇਲ_ਸੂਚਕਾਂਕ) {
    int32_t initial_Partial_ਮੇਲ_Len = *partial_ਮੇਲ_Len;
    ਲਈ (int32_t ਸੂਚਕਾਂਕ = begin; ਸੂਚਕਾਂਕ <= ਸਿਰਾ; ++ਸੂਚਕਾਂਕ) {
        int32_t len = currency_ਨਾਂ[ਸੂਚਕਾਂਕ].currencyNameLen;
        ਜੇ (len > *max_ਮੇਲ_Len && len <= ਪਾਠ_Len &&
            uprv_memcmp(currency_ਨਾਂ[ਸੂਚਕਾਂਕ].currencyName, ਪਾਠ, len * ਆਕਾਰ(char16_t)) == 0) {
            *partial_ਮੇਲ_Len = MAX(*partial_ਮੇਲ_Len, len);
            *max_ਮੇਲ_ਸੂਚਕਾਂਕ = ਸੂਚਕਾਂਕ;
            *max_ਮੇਲ_Len = len;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UCURR_DEBUG
            ਲਿਖੋ("maxMatchIndex = %d, maxMatchLen = %d\n",
                   *max_ਮੇਲ_ਸੂਚਕਾਂਕ, *max_ਮੇਲ_Len);
#ਜੇ_ਅੰਤ
        } ਨਹੀਂਤਾਂ {
            // Check for partial matches.
            ਲਈ (int32_t i=initial_Partial_ਮੇਲ_Len; i<MIN(len, ਪਾਠ_Len); i++) {
                ਜੇ (currency_ਨਾਂ[ਸੂਚਕਾਂਕ].currencyName[i] != ਪਾਠ[i]) {
                    ਰੋਕੋ;
                }
                *partial_ਮੇਲ_Len = MAX(*partial_ਮੇਲ_Len, i + 1);
            }
        }
    }
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਰੇਖਿਕ_ਖੋਜ_ਹੱਦ 10

// Find longest match between "text" and currency names in "currencyNames".
// @param  total_currency_count: total number of currency names in CurrencyNames.
// @param  textLen: the length of the text to be compared
// @param  maxMatchLen: passing in the computed max matching length
//                              pass out the new max  matching length
// @param  maxMatchIndex: the index in currencyName which has the longest
//                        match with input text.
ਸਥਿਰ ਖਾਲੀ
ਖੋਜ_Currency_Name(ਅਚਲ CurrencyNameStruct* currency_ਨਾਂ, 
                   int32_t ਕੁੱਲ_currency_ਗਿਣਤੀ,
                   ਅਚਲ char16_t* ਪਾਠ, int32_t ਪਾਠ_Len,
                   int32_t *partial_ਮੇਲ_Len,
                   int32_t* max_ਮੇਲ_Len, int32_t* max_ਮੇਲ_ਸੂਚਕਾਂਕ) {
    *max_ਮੇਲ_ਸੂਚਕਾਂਕ = -1;
    *max_ਮੇਲ_Len = 0;
    int32_t ਮੇਲ_ਸੂਚਕਾਂਕ = -1;
    int32_t ਦੁਆਧਾਰੀ_ਖੋਜ_Begin = 0;
    int32_t ਦੁਆਧਾਰੀ_ਖੋਜ_ਸਿਰਾ = ਕੁੱਲ_currency_ਗਿਣਤੀ - 1;
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
    ਲਈ (int32_t ਸੂਚਕਾਂਕ = 0; ਸੂਚਕਾਂਕ < ਪਾਠ_Len; ++ਸੂਚਕਾਂਕ) {
        // matchIndex saves the one with exact match till the current point.
        // [binarySearchBegin, binarySearchEnd] saves the matching range.
        ਮੇਲ_ਸੂਚਕਾਂਕ = ਦੁਆਧਾਰੀ_ਖੋਜ(currency_ਨਾਂ, ਸੂਚਕਾਂਕ,
                                  ਪਾਠ[ਸੂਚਕਾਂਕ],
                                  &ਦੁਆਧਾਰੀ_ਖੋਜ_Begin, &ਦੁਆਧਾਰੀ_ਖੋਜ_ਸਿਰਾ);
        ਜੇ (ਦੁਆਧਾਰੀ_ਖੋਜ_Begin == -1) { // did not find the range
            ਰੋਕੋ;
        }
        *partial_ਮੇਲ_Len = MAX(*partial_ਮੇਲ_Len, ਸੂਚਕਾਂਕ + 1);
        ਜੇ (ਮੇਲ_ਸੂਚਕਾਂਕ != -1) { 
            // find an exact match for text from text[0] to text[index] 
            // in currencyNames array.
            *max_ਮੇਲ_Len = ਸੂਚਕਾਂਕ + 1;
            *max_ਮੇਲ_ਸੂਚਕਾਂਕ = ਮੇਲ_ਸੂਚਕਾਂਕ;
        }
        ਜੇ (ਦੁਆਧਾਰੀ_ਖੋਜ_ਸਿਰਾ - ਦੁਆਧਾਰੀ_ਖੋਜ_Begin < ਰੇਖਿਕ_ਖੋਜ_ਹੱਦ) {
            // linear search if within threshold.
            ਰੇਖਿਕ_ਖੋਜ(currency_ਨਾਂ, ਦੁਆਧਾਰੀ_ਖੋਜ_Begin, ਦੁਆਧਾਰੀ_ਖੋਜ_ਸਿਰਾ,
                         ਪਾਠ, ਪਾਠ_Len,
                         partial_ਮੇਲ_Len,
                         max_ਮੇਲ_Len, max_ਮੇਲ_ਸੂਚਕਾਂਕ);
            ਰੋਕੋ;
        }
    }
}

//========================= currency name cache =====================
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    ਅੱਖਰ locale[ULOC_FULLNAME_ਸਮਰੱਥਾ];  //key
    // currency names, case insensitive
    CurrencyNameStruct* currency_ਨਾਂ;  //ਮੁੱਲ
    int32_t ਕੁੱਲ_Currency_Name_ਗਿਣਤੀ;  // currency name count
    // currency symbols and ISO code, case sensitive
    CurrencyNameStruct* currencySymbols; //ਮੁੱਲ
    int32_t ਕੁੱਲ_Currency_ਚਿੰਨ੍ਹ_ਗਿਣਤੀ;  //ਗਿਣਤੀ
    // reference count.
    // reference count is set to 1 when an entry is put to cache.
    // it increases by 1 before accessing, and decreased by 1 after accessing.
    // The entry is deleted when ref count is zero, which means 
    // the entry is replaced out of cache and no process is accessing it.
    int32_t ref_ਗਿਣਤੀ;
} Currency_Name_Cache_ਇੰਦਰਾਜ;


#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURRENCY_NAME_CACHE_ਅੰਕ 10

// Reserve 10 cache entries.
ਸਥਿਰ Currency_Name_Cache_ਇੰਦਰਾਜ* currCache[CURRENCY_NAME_CACHE_ਅੰਕ] = {ਖਾਲੀ_ਸੂਚਕ};
// Using an index to indicate which entry to be replaced when cache is full.
// It is a simple round-robin replacement strategy.
ਸਥਿਰ int8_t current_Cache_ਇੰਦਰਾਜ_ਸੂਚਕਾਂਕ = 0;

ਸਥਿਰ UMutex gCurrencyCacheMutex;

// Cache deletion
ਸਥਿਰ ਖਾਲੀ
delete_Currency_ਨਾਂ(CurrencyNameStruct* currency_ਨਾਂ, int32_t count) {
    ਲਈ (int32_t ਸੂਚਕਾਂਕ = 0; ਸੂਚਕਾਂਕ < count; ++ਸੂਚਕਾਂਕ) {
        ਜੇ ( (currency_ਨਾਂ[ਸੂਚਕਾਂਕ].flag & NEED_ਮੰਜ਼ਿਲ_BE_DELETED) ) {
            uprv_free(currency_ਨਾਂ[ਸੂਚਕਾਂਕ].currencyName);
        }
    }
    uprv_free(currency_ਨਾਂ);
}


ਸਥਿਰ ਖਾਲੀ
delete_Cache_ਇੰਦਰਾਜ(Currency_Name_Cache_ਇੰਦਰਾਜ* ਇੰਦਰਾਜ) {
    delete_Currency_ਨਾਂ(ਇੰਦਰਾਜ->currency_ਨਾਂ, ਇੰਦਰਾਜ->ਕੁੱਲ_Currency_Name_ਗਿਣਤੀ);
    delete_Currency_ਨਾਂ(ਇੰਦਰਾਜ->currencySymbols, ਇੰਦਰਾਜ->ਕੁੱਲ_Currency_ਚਿੰਨ੍ਹ_ਗਿਣਤੀ);
    uprv_free(ਇੰਦਰਾਜ);
}


// Cache clean up
ਸਥਿਰ UBool U_CALLCONV
currency_cache_cleanup() {
    ਲਈ (int32_t i = 0; i < CURRENCY_NAME_CACHE_ਅੰਕ; ++i) {
        ਜੇ (currCache[i]) {
            delete_Cache_ਇੰਦਰਾਜ(currCache[i]);
            currCache[i] = ਖਾਲੀ_ਸੂਚਕ;
        }
    }
    ਵਾਪਸ ਸੱਚ;
}


/**
 * Loads the currency name data from the cache, or from resource bundles if necessary.
 * The refCount is automatically incremented.  It is the caller's responsibility
 * to decrement it when done!
 */
ਸਥਿਰ Currency_Name_Cache_ਇੰਦਰਾਜ*
get_Cache_ਇੰਦਰਾਜ(ਅਚਲ ਅੱਖਰ* locale, UError_ਕੋਡ& ec) {

    int32_t ਕੁੱਲ_currency_name_ਗਿਣਤੀ = 0;
    CurrencyNameStruct* currency_ਨਾਂ = ਖਾਲੀ_ਸੂਚਕ;
    int32_t ਕੁੱਲ_currency_ਚਿੰਨ੍ਹ_ਗਿਣਤੀ = 0;
    CurrencyNameStruct* currencySymbols = ਖਾਲੀ_ਸੂਚਕ;
    Currency_Name_Cache_ਇੰਦਰਾਜ* cache_ਇੰਦਰਾਜ = ਖਾਲੀ_ਸੂਚਕ;

    umtx_ਤਾਲਾ(&gCurrencyCacheMutex);
    // in order to handle racing correctly,
    // not putting 'search' in a separate function.
    int8_t ਮਿਲਿਆ = -1;
    ਲਈ (int8_t i = 0; i < CURRENCY_NAME_CACHE_ਅੰਕ; ++i) {
        ਜੇ (currCache[i]!= ਖਾਲੀ_ਸੂਚਕ &&
            uprv_strcmp(locale, currCache[i]->locale) == 0) {
            ਮਿਲਿਆ = i;
            ਰੋਕੋ;
        }
    }
    ਜੇ (ਮਿਲਿਆ != -1) {
        cache_ਇੰਦਰਾਜ = currCache[ਮਿਲਿਆ];
        ++(cache_ਇੰਦਰਾਜ->ref_ਗਿਣਤੀ);
    }
    umtx_unlock(&gCurrencyCacheMutex);
    ਜੇ (ਮਿਲਿਆ == -1) {
        collect_Currency_ਨਾਂ(locale, &currency_ਨਾਂ, &ਕੁੱਲ_currency_name_ਗਿਣਤੀ, &currencySymbols, &ਕੁੱਲ_currency_ਚਿੰਨ੍ਹ_ਗਿਣਤੀ, ec);
        ਜੇ (U_FAILURE(ec)) {
            ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
        }
        umtx_ਤਾਲਾ(&gCurrencyCacheMutex);
        // check again.
        ਲਈ (int8_t i = 0; i < CURRENCY_NAME_CACHE_ਅੰਕ; ++i) {
            ਜੇ (currCache[i]!= ਖਾਲੀ_ਸੂਚਕ &&
                uprv_strcmp(locale, currCache[i]->locale) == 0) {
                ਮਿਲਿਆ = i;
                ਰੋਕੋ;
            }
        }
        ਜੇ (ਮਿਲਿਆ == -1) {
            // insert new entry to 
            // currentCacheEntryIndex % CURRENCY_NAME_CACHE_NUM
            // and remove the existing entry 
            // currentCacheEntryIndex % CURRENCY_NAME_CACHE_NUM
            // from cache.
            cache_ਇੰਦਰਾਜ = currCache[current_Cache_ਇੰਦਰਾਜ_ਸੂਚਕਾਂਕ];
            ਜੇ (cache_ਇੰਦਰਾਜ) {
                --(cache_ਇੰਦਰਾਜ->ref_ਗਿਣਤੀ);
                // delete if the ref count is zero
                ਜੇ (cache_ਇੰਦਰਾਜ->ref_ਗਿਣਤੀ == 0) {
                    delete_Cache_ਇੰਦਰਾਜ(cache_ਇੰਦਰਾਜ);
                }
            }
            cache_ਇੰਦਰਾਜ = static_cast<Currency_Name_Cache_ਇੰਦਰਾਜ*>(uprv_malloc(ਆਕਾਰ(Currency_Name_Cache_ਇੰਦਰਾਜ)));
            ਜੇ (cache_ਇੰਦਰਾਜ == ਖਾਲੀ_ਸੂਚਕ) {
                delete_Currency_ਨਾਂ(currency_ਨਾਂ, ਕੁੱਲ_currency_name_ਗਿਣਤੀ);
                delete_Currency_ਨਾਂ(currencySymbols, ਕੁੱਲ_currency_ਚਿੰਨ੍ਹ_ਗਿਣਤੀ);
                ec = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
                ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
            }
            currCache[current_Cache_ਇੰਦਰਾਜ_ਸੂਚਕਾਂਕ] = cache_ਇੰਦਰਾਜ;
            uprv_strcpy(cache_ਇੰਦਰਾਜ->locale, locale);
            cache_ਇੰਦਰਾਜ->currency_ਨਾਂ = currency_ਨਾਂ;
            cache_ਇੰਦਰਾਜ->ਕੁੱਲ_Currency_Name_ਗਿਣਤੀ = ਕੁੱਲ_currency_name_ਗਿਣਤੀ;
            cache_ਇੰਦਰਾਜ->currencySymbols = currencySymbols;
            cache_ਇੰਦਰਾਜ->ਕੁੱਲ_Currency_ਚਿੰਨ੍ਹ_ਗਿਣਤੀ = ਕੁੱਲ_currency_ਚਿੰਨ੍ਹ_ਗਿਣਤੀ;
            cache_ਇੰਦਰਾਜ->ref_ਗਿਣਤੀ = 2; // one for cache, one for reference
            current_Cache_ਇੰਦਰਾਜ_ਸੂਚਕਾਂਕ = (current_Cache_ਇੰਦਰਾਜ_ਸੂਚਕਾਂਕ + 1) % CURRENCY_NAME_CACHE_ਅੰਕ;
            ucln_common_ਰਜਿਸਟਰ_Cleanup(UCLN_COMMON_CURRENCY, currency_cleanup);
        } ਨਹੀਂਤਾਂ {
            delete_Currency_ਨਾਂ(currency_ਨਾਂ, ਕੁੱਲ_currency_name_ਗਿਣਤੀ);
            delete_Currency_ਨਾਂ(currencySymbols, ਕੁੱਲ_currency_ਚਿੰਨ੍ਹ_ਗਿਣਤੀ);
            cache_ਇੰਦਰਾਜ = currCache[ਮਿਲਿਆ];
            ++(cache_ਇੰਦਰਾਜ->ref_ਗਿਣਤੀ);
        }
        umtx_unlock(&gCurrencyCacheMutex);
    }

    ਵਾਪਸ cache_ਇੰਦਰਾਜ;
}

ਸਥਿਰ ਖਾਲੀ release_Cache_ਇੰਦਰਾਜ(Currency_Name_Cache_ਇੰਦਰਾਜ* cache_ਇੰਦਰਾਜ) {
    umtx_ਤਾਲਾ(&gCurrencyCacheMutex);
    --(cache_ਇੰਦਰਾਜ->ref_ਗਿਣਤੀ);
    ਜੇ (cache_ਇੰਦਰਾਜ->ref_ਗਿਣਤੀ == 0) {  // remove
        delete_Cache_ਇੰਦਰਾਜ(cache_ਇੰਦਰਾਜ);
    }
    umtx_unlock(&gCurrencyCacheMutex);
}

U_CAPI ਖਾਲੀ
uprv_parseCurrency(ਅਚਲ ਅੱਖਰ* locale,
                   ਅਚਲ icu::UnicodeString& ਪਾਠ,
                   icu::ParsePosition& pos,
                   int8_t type,
                   int32_t* partial_ਮੇਲ_Len,
                   char16_t* ਨਤੀਜਾ,
                   UError_ਕੋਡ& ec) {
    U_NAMESPACE_USE
    ਜੇ (U_FAILURE(ec)) {
        ਵਾਪਸ;
    }
    Currency_Name_Cache_ਇੰਦਰਾਜ* cache_ਇੰਦਰਾਜ = get_Cache_ਇੰਦਰਾਜ(locale, ec);
    ਜੇ (U_FAILURE(ec)) {
        ਵਾਪਸ;
    }

    int32_t ਕੁੱਲ_currency_name_ਗਿਣਤੀ = cache_ਇੰਦਰਾਜ->ਕੁੱਲ_Currency_Name_ਗਿਣਤੀ;
    CurrencyNameStruct* currency_ਨਾਂ = cache_ਇੰਦਰਾਜ->currency_ਨਾਂ;
    int32_t ਕੁੱਲ_currency_ਚਿੰਨ੍ਹ_ਗਿਣਤੀ = cache_ਇੰਦਰਾਜ->ਕੁੱਲ_Currency_ਚਿੰਨ੍ਹ_ਗਿਣਤੀ;
    CurrencyNameStruct* currencySymbols = cache_ਇੰਦਰਾਜ->currencySymbols;

    int32_t ਸ਼ੁਰੂ = pos.get_ਸੂਚਕਾਂਕ();

    char16_t ਨਿਵੇਸ਼_ਪਾਠ[MAX_CURRENCY_NAME_LEN];
    char16_t ਉੱਤਲਾ_ਪਾਠ[MAX_CURRENCY_NAME_LEN];
    int32_t ਪਾਠ_Len = MIN(MAX_CURRENCY_NAME_LEN, ਪਾਠ.length() - ਸ਼ੁਰੂ);
    ਪਾਠ.extract(ਸ਼ੁਰੂ, ਪਾਠ_Len, ਨਿਵੇਸ਼_ਪਾਠ);
    UError_ਕੋਡ ec1 = U_ZERO_ERROR;
    ਪਾਠ_Len = u_str_ਮੰਜ਼ਿਲ_ਉੱਤਲਾ(ਉੱਤਲਾ_ਪਾਠ, MAX_CURRENCY_NAME_LEN, ਨਿਵੇਸ਼_ਪਾਠ, ਪਾਠ_Len, locale, &ec1);

    // Make sure partialMatchLen is initialized
    *partial_ਮੇਲ_Len = 0;

    int32_t max = 0;
    int32_t ਮੇਲ_ਸੂਚਕਾਂਕ = -1;
    // case in-sensitive comparison against currency names
    ਖੋਜ_Currency_Name(currency_ਨਾਂ, ਕੁੱਲ_currency_name_ਗਿਣਤੀ, 
                       ਉੱਤਲਾ_ਪਾਠ, ਪਾਠ_Len, partial_ਮੇਲ_Len, &max, &ਮੇਲ_ਸੂਚਕਾਂਕ);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UCURR_DEBUG
    ਲਿਖੋ("search in names, max = %d, matchIndex = %d\n", max, ਮੇਲ_ਸੂਚਕਾਂਕ);
#ਜੇ_ਅੰਤ

    int32_t max_ਵਿੱਚ_ਚਿੰਨ੍ਹ = 0;
    int32_t ਮੇਲ_ਸੂਚਕਾਂਕ_ਵਿੱਚ_ਚਿੰਨ੍ਹ = -1;
    ਜੇ (type != UCURR_LONG_NAME) {  // not name only
        // case sensitive comparison against currency symbols and ISO code.
        ਖੋਜ_Currency_Name(currencySymbols, ਕੁੱਲ_currency_ਚਿੰਨ੍ਹ_ਗਿਣਤੀ, 
                           ਨਿਵੇਸ਼_ਪਾਠ, ਪਾਠ_Len,
                           partial_ਮੇਲ_Len,
                           &max_ਵਿੱਚ_ਚਿੰਨ੍ਹ, &ਮੇਲ_ਸੂਚਕਾਂਕ_ਵਿੱਚ_ਚਿੰਨ੍ਹ);
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UCURR_DEBUG
    ਲਿਖੋ("search in symbols, maxInSymbol = %d, matchIndexInSymbol = %d\n", max_ਵਿੱਚ_ਚਿੰਨ੍ਹ, ਮੇਲ_ਸੂਚਕਾਂਕ_ਵਿੱਚ_ਚਿੰਨ੍ਹ);
    ਜੇ(ਮੇਲ_ਸੂਚਕਾਂਕ_ਵਿੱਚ_ਚਿੰਨ੍ਹ != -1) {
      ਲਿਖੋ("== ISO=%s\n", currencySymbols[ਮੇਲ_ਸੂਚਕਾਂਕ_ਵਿੱਚ_ਚਿੰਨ੍ਹ].Iso_ਕੋਡ);
    }
#ਜੇ_ਅੰਤ

    ਜੇ (max >= max_ਵਿੱਚ_ਚਿੰਨ੍ਹ && ਮੇਲ_ਸੂਚਕਾਂਕ != -1) {
        u_chars_ਮੰਜ਼ਿਲ_UChars(currency_ਨਾਂ[ਮੇਲ_ਸੂਚਕਾਂਕ].Iso_ਕੋਡ, ਨਤੀਜਾ, 4);
        pos.ਸੈੱਟ_ਸੂਚਕਾਂਕ(ਸ਼ੁਰੂ + max);
    } ਨਹੀਂਤਾਂ ਜੇ (max_ਵਿੱਚ_ਚਿੰਨ੍ਹ >= max && ਮੇਲ_ਸੂਚਕਾਂਕ_ਵਿੱਚ_ਚਿੰਨ੍ਹ != -1) {
        u_chars_ਮੰਜ਼ਿਲ_UChars(currencySymbols[ਮੇਲ_ਸੂਚਕਾਂਕ_ਵਿੱਚ_ਚਿੰਨ੍ਹ].Iso_ਕੋਡ, ਨਤੀਜਾ, 4);
        pos.ਸੈੱਟ_ਸੂਚਕਾਂਕ(ਸ਼ੁਰੂ + max_ਵਿੱਚ_ਚਿੰਨ੍ਹ);
    }

    // decrease reference count
    release_Cache_ਇੰਦਰਾਜ(cache_ਇੰਦਰਾਜ);
}

ਖਾਲੀ uprv_currencyLeads(ਅਚਲ ਅੱਖਰ* locale, icu::Unicode_ਸੈੱਟ& ਨਤੀਜਾ, UError_ਕੋਡ& ec) {
    U_NAMESPACE_USE
    ਜੇ (U_FAILURE(ec)) {
        ਵਾਪਸ;
    }
    Currency_Name_Cache_ਇੰਦਰਾਜ* cache_ਇੰਦਰਾਜ = get_Cache_ਇੰਦਰਾਜ(locale, ec);
    ਜੇ (U_FAILURE(ec)) {
        ਵਾਪਸ;
    }

    ਲਈ (int32_t i=0; i<cache_ਇੰਦਰਾਜ->ਕੁੱਲ_Currency_ਚਿੰਨ੍ਹ_ਗਿਣਤੀ; i++) {
        ਅਚਲ CurrencyNameStruct& info = cache_ਇੰਦਰਾਜ->currencySymbols[i];
        UChar32 cp;
        U16_GET(info.currencyName, 0, 0, info.currencyNameLen, cp);
        ਨਤੀਜਾ.ਜੋੜੋ(cp);
    }

    ਲਈ (int32_t i=0; i<cache_ਇੰਦਰਾਜ->ਕੁੱਲ_Currency_Name_ਗਿਣਤੀ; i++) {
        ਅਚਲ CurrencyNameStruct& info = cache_ਇੰਦਰਾਜ->currency_ਨਾਂ[i];
        UChar32 cp;
        U16_GET(info.currencyName, 0, 0, info.currencyNameLen, cp);
        ਨਤੀਜਾ.ਜੋੜੋ(cp);
    }

    // decrease reference count
    release_Cache_ਇੰਦਰਾਜ(cache_ਇੰਦਰਾਜ);
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
U_CAPI ਖਾਲੀ
uprv_getStaticCurrencyName(ਅਚਲ char16_t* iso, ਅਚਲ ਅੱਖਰ* loc,
                           icu::UnicodeString& ਨਤੀਜਾ, UError_ਕੋਡ& ec)
{
    U_NAMESPACE_USE

    int32_t len;
    ਅਚਲ char16_t* currname = ucurr_getName(iso, loc, UCURR_ਚਿੰਨ੍ਹ_NAME,
                                          ਖਾਲੀ_ਸੂਚਕ /* isChoiceFormat */, &len, &ec);
    ਜੇ (U_SUCCESS(ec)) {
        ਨਤੀਜਾ.ਸੈੱਟ_ਮੰਜ਼ਿਲ(currname, len);
    }
}

U_CAPI int32_t U_EXPORT2
ucurr_getDefaultFractionDigits(ਅਚਲ char16_t* currency, UError_ਕੋਡ* ec) {
    ਵਾਪਸ ucurr_getDefaultFractionDigitsForUsage(currency,UCURR_USAGE_STANDARD,ec);
}

U_CAPI int32_t U_EXPORT2
ucurr_getDefaultFractionDigitsForUsage(ਅਚਲ char16_t* currency, ਅਚਲ UCurrencyUsage usage, UError_ਕੋਡ* ec) {
    int32_t fracDigits = 0;
    ਜੇ (U_SUCCESS(*ec)) {
        ਚੋਣ (usage) {
            ਹਾਲਤ UCURR_USAGE_STANDARD:
                fracDigits = (ਲੱਭੋ_Meta_ਅੰਕੜੇ(currency, *ec))[0];
                ਰੋਕੋ;
            ਹਾਲਤ UCURR_USAGE_CASH:
                fracDigits = (ਲੱਭੋ_Meta_ਅੰਕੜੇ(currency, *ec))[2];
                ਰੋਕੋ;
            ਮੂਲ:
                *ec = U_UNSUPPORTED_ERROR;
        }
    }
    ਵਾਪਸ fracDigits;
}

U_CAPI ਦੁੱਗਣਾ U_EXPORT2
ucurr_getRoundingIncrement(ਅਚਲ char16_t* currency, UError_ਕੋਡ* ec) {
    ਵਾਪਸ ucurr_getRoundingIncrementForUsage(currency, UCURR_USAGE_STANDARD, ec);
}

U_CAPI ਦੁੱਗਣਾ U_EXPORT2
ucurr_getRoundingIncrementForUsage(ਅਚਲ char16_t* currency, ਅਚਲ UCurrencyUsage usage, UError_ਕੋਡ* ec) {
    ਦੁੱਗਣਾ ਨਤੀਜਾ = 0.0;

    ਅਚਲ int32_t *ਅੰਕੜੇ = ਲੱਭੋ_Meta_ਅੰਕੜੇ(currency, *ec);
    ਜੇ (U_SUCCESS(*ec)) {
        int32_t fracDigits;
        int32_t increment;
        ਚੋਣ (usage) {
            ਹਾਲਤ UCURR_USAGE_STANDARD:
                fracDigits = ਅੰਕੜੇ[0];
                increment = ਅੰਕੜੇ[1];
                ਰੋਕੋ;
            ਹਾਲਤ UCURR_USAGE_CASH:
                fracDigits = ਅੰਕੜੇ[2];
                increment = ਅੰਕੜੇ[3];
                ਰੋਕੋ;
            ਮੂਲ:
                *ec = U_UNSUPPORTED_ERROR;
                ਵਾਪਸ ਨਤੀਜਾ;
        }

        // If the meta data is invalid, return 0.0
        ਜੇ (fracDigits < 0 || fracDigits > MAX_POW10) {
            *ec = U_INVALID_FORMAT_ERROR;
        } ਨਹੀਂਤਾਂ {
            // A rounding value of 0 or 1 indicates no rounding.
            ਜੇ (increment >= 2) {
                // Return (increment) / 10^(fracDigits).  The only actual rounding data,
                // as of this writing, is CHF { 2, 5 }.
                ਨਤੀਜਾ = ਦੁੱਗਣਾ(increment) / POW10[fracDigits];
            }
        }
    }

    ਵਾਪਸ ਨਤੀਜਾ;
}

U_CDECL_BEGIN

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ UCurrencyContext {
    uint32_t currType; /* UCurrCurrencyType */
    uint32_t listIdx;
} UCurrencyContext;

/*
Please keep this list in alphabetical order.
You can look at the CLDR supplemental data or ISO-4217 for the meaning of some
of these items.
ISO-4217: http://www.iso.org/iso/en/prods-services/popstds/currencycodeslist.html
*/
ਸਥਿਰ ਅਚਲ ਬਣਤਰ CurrencyList {
    ਅਚਲ ਅੱਖਰ *currency;
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
    { ਖਾਲੀ_ਸੂਚਕ, 0 } // Leave here to denote the end of the list.
};

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UCURR_MATCHES_BITMASK(variable, type_ਮੰਜ਼ਿਲ_ਮੇਲ) \
    ((type_ਮੰਜ਼ਿਲ_ਮੇਲ) == UCURR_ALL || ((variable) & (type_ਮੰਜ਼ਿਲ_ਮੇਲ)) == (type_ਮੰਜ਼ਿਲ_ਮੇਲ))

ਸਥਿਰ int32_t U_CALLCONV
ucurr_ਗਿਣਤੀ_Currency_List(UEnumeration *enumerator, UError_ਕੋਡ * /*pErrorCode*/) {
    UCurrencyContext *myContext = (UCurrencyContext *)(enumerator->context);
    uint32_t currType = myContext->currType;
    int32_t count = 0;

    /* Count the number of items matching the type we are looking for. */
    ਲਈ (int32_t idx = 0; gCurrencyList[idx].currency != ਖਾਲੀ_ਸੂਚਕ; idx++) {
        ਜੇ (UCURR_MATCHES_BITMASK(gCurrencyList[idx].currType, currType)) {
            count++;
        }
    }
    ਵਾਪਸ count;
}

ਸਥਿਰ ਅਚਲ ਅੱਖਰ* U_CALLCONV
ucurr_ਅਗਲਾ_Currency_List(UEnumeration *enumerator,
                        int32_t* ਨਤੀਜਾ_ਲੰਮਾਈ,
                        UError_ਕੋਡ * /*pErrorCode*/)
{
    UCurrencyContext *myContext = (UCurrencyContext *)(enumerator->context);

    /* Find the next in the list that matches the type we are looking for. */
    ਜਦੋਂ (myContext->listIdx < UPRV_LENGTHOF(gCurrencyList)-1) {
        ਅਚਲ ਬਣਤਰ CurrencyList *curr_ਵਸਤੂ = &gCurrencyList[myContext->listIdx++];
        ਜੇ (UCURR_MATCHES_BITMASK(curr_ਵਸਤੂ->currType, myContext->currType))
        {
            ਜੇ (ਨਤੀਜਾ_ਲੰਮਾਈ) {
                *ਨਤੀਜਾ_ਲੰਮਾਈ = 3; /* Currency codes are only 3 chars long */
            }
            ਵਾਪਸ curr_ਵਸਤੂ->currency;
        }
    }
    /* We enumerated too far. */
    ਜੇ (ਨਤੀਜਾ_ਲੰਮਾਈ) {
        *ਨਤੀਜਾ_ਲੰਮਾਈ = 0;
    }
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
}

ਸਥਿਰ ਖਾਲੀ U_CALLCONV
ucurr_resetCurrencyList(UEnumeration *enumerator, UError_ਕੋਡ * /*pErrorCode*/) {
    ((UCurrencyContext *)(enumerator->context))->listIdx = 0;
}

ਸਥਿਰ ਖਾਲੀ U_CALLCONV
ucurr_closeCurrencyList(UEnumeration *enumerator) {
    uprv_free(enumerator->context);
    uprv_free(enumerator);
}

ਸਥਿਰ ਖਾਲੀ U_CALLCONV
ucurr_createCurrencyList(UHashtable *isoCodes, UError_ਕੋਡ* ਸਥਿਤੀ){
    UError_ਕੋਡ local_ਸਥਿਤੀ = U_ZERO_ERROR;

    // Look up the CurrencyMap element in the root bundle.
    UResourceBundle *rb = ures_open_ਸਿੱਧਾ(U_ICUDATA_CURR, CURRENCY_ਅੰਕੜੇ, &local_ਸਥਿਤੀ);
    Local_UResource_Bundle_ਸੂਚਕ currencyMapArray(ures_get_By_ਕੁੰਜੀ(rb, CURRENCY_MAP, rb, &local_ਸਥਿਤੀ));

    ਜੇ (U_SUCCESS(local_ਸਥਿਤੀ)) {
        // process each entry in currency map
        ਲਈ (int32_t i=0; i<ures_get_ਮਾਪ(currencyMapArray.getAlias()); i++) {
            // get the currency resource
            Local_UResource_Bundle_ਸੂਚਕ currencyArray(ures_get_By_ਸੂਚਕਾਂਕ(currencyMapArray.getAlias(), i, ਖਾਲੀ_ਸੂਚਕ, &local_ਸਥਿਤੀ));
            // process each currency
            ਜੇ (U_SUCCESS(local_ਸਥਿਤੀ)) {
                ਲਈ (int32_t j=0; j<ures_get_ਮਾਪ(currencyArray.getAlias()); j++) {
                    // get the currency resource
                    Local_UResource_Bundle_ਸੂਚਕ currencyRes(ures_get_By_ਸੂਚਕਾਂਕ(currencyArray.getAlias(), j, ਖਾਲੀ_ਸੂਚਕ, &local_ਸਥਿਤੀ));
                    Iso_ਕੋਡ_ਇੰਦਰਾਜ *ਇੰਦਰਾਜ = (Iso_ਕੋਡ_ਇੰਦਰਾਜ*)uprv_malloc(ਆਕਾਰ(Iso_ਕੋਡ_ਇੰਦਰਾਜ));
                    ਜੇ (ਇੰਦਰਾਜ == ਖਾਲੀ_ਸੂਚਕ) {
                        *ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
                        ਵਾਪਸ;
                    }

                    // get the ISO code
                    int32_t iso_ਲੰਮਾਈ = 0;
                    Local_UResource_Bundle_ਸੂਚਕ idRes(ures_get_By_ਕੁੰਜੀ(currencyRes.getAlias(), "id", ਖਾਲੀ_ਸੂਚਕ, &local_ਸਥਿਤੀ));
                    ਜੇ (idRes.isNull()) {
                        ਜਾਰੀ;
                    }
                    ਅਚਲ char16_t *iso_ਕੋਡ = ures_getString(idRes.getAlias(), &iso_ਲੰਮਾਈ, &local_ਸਥਿਤੀ);

                    // get from date
                    UDate fromDate = U_DATE_MIN;
                    Local_UResource_Bundle_ਸੂਚਕ fromRes(ures_get_By_ਕੁੰਜੀ(currencyRes.getAlias(), "from", ਖਾਲੀ_ਸੂਚਕ, &local_ਸਥਿਤੀ));

                    ਜੇ (U_SUCCESS(local_ਸਥਿਤੀ)) {
                        int32_t from_ਲੰਮਾਈ = 0;
                        ਅਚਲ int32_t *fromArray = ures_get_Int_ਸਦਿਸ਼(fromRes.getAlias(), &from_ਲੰਮਾਈ, &local_ਸਥਿਤੀ);
                        int64_t currDate64 = ((uint64_t)fromArray[0]) << 32;
                        currDate64 |= ((int64_t)fromArray[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                        fromDate = (UDate)currDate64;
                    }

                    // get to date
                    UDate ਮੰਜ਼ਿਲ_Date = U_DATE_MAX;
                    local_ਸਥਿਤੀ = U_ZERO_ERROR;
                    Local_UResource_Bundle_ਸੂਚਕ ਮੰਜ਼ਿਲ_Res(ures_get_By_ਕੁੰਜੀ(currencyRes.getAlias(), "to", ਖਾਲੀ_ਸੂਚਕ, &local_ਸਥਿਤੀ));

                    ਜੇ (U_SUCCESS(local_ਸਥਿਤੀ)) {
                        int32_t ਮੰਜ਼ਿਲ_ਲੰਮਾਈ = 0;
                        ਅਚਲ int32_t *ਮੰਜ਼ਿਲ_Array = ures_get_Int_ਸਦਿਸ਼(ਮੰਜ਼ਿਲ_Res.getAlias(), &ਮੰਜ਼ਿਲ_ਲੰਮਾਈ, &local_ਸਥਿਤੀ);
                        int64_t currDate64 = (uint64_t)ਮੰਜ਼ਿਲ_Array[0] << 32;
                        currDate64 |= ((int64_t)ਮੰਜ਼ਿਲ_Array[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                        ਮੰਜ਼ਿਲ_Date = (UDate)currDate64;
                    }

                    ਇੰਦਰਾਜ->iso_ਕੋਡ = iso_ਕੋਡ;
                    ਇੰਦਰਾਜ->from = fromDate;
                    ਇੰਦਰਾਜ->ਮੰਜ਼ਿਲ = ਮੰਜ਼ਿਲ_Date;

                    local_ਸਥਿਤੀ = U_ZERO_ERROR;
                    uhash_put(isoCodes, (char16_t *)iso_ਕੋਡ, ਇੰਦਰਾਜ, &local_ਸਥਿਤੀ);
                }
            } ਨਹੀਂਤਾਂ {
                *ਸਥਿਤੀ = local_ਸਥਿਤੀ;
            }
        }
    } ਨਹੀਂਤਾਂ {
        *ਸਥਿਤੀ = local_ਸਥਿਤੀ;
    }
}

ਸਥਿਰ ਅਚਲ UEnumeration gEnumCurrencyList = {
    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ,
    ucurr_closeCurrencyList,
    ucurr_ਗਿਣਤੀ_Currency_List,
    uenum_unextDefault,
    ucurr_ਅਗਲਾ_Currency_List,
    ucurr_resetCurrencyList
};
U_CDECL_ਸਿਰਾ


ਸਥਿਰ ਖਾਲੀ U_CALLCONV initIsoCodes(UError_ਕੋਡ &ਸਥਿਤੀ) {
    U_ASSERT(gIsoCodes == ਖਾਲੀ_ਸੂਚਕ);
    ucln_common_ਰਜਿਸਟਰ_Cleanup(UCLN_COMMON_CURRENCY, currency_cleanup);

    Local_UHashtable_ਸੂਚਕ isoCodes(uhash_open(uhash_hashUChars, uhash_compareUChars, ਖਾਲੀ_ਸੂਚਕ, &ਸਥਿਤੀ));
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ;
    }
    uhash_ਸੈੱਟ_ਮੁੱਲ_Deleter(isoCodes.getAlias(), delete_Iso_ਕੋਡ_ਇੰਦਰਾਜ);

    ucurr_createCurrencyList(isoCodes.getAlias(), &ਸਥਿਤੀ);
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ;
    }
    gIsoCodes = isoCodes.orphan();  // Note: gIsoCodes is const. Once set up here it is never altered,
                                    //       and read only access is safe without synchronization.
}

ਸਥਿਰ ਖਾਲੀ populateCurrSymbolsEquiv(icu::Hashtable *hash, UError_ਕੋਡ &ਸਥਿਤੀ) {
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) { ਵਾਪਸ; }
    ਲਈ (ਅਚਲ ਸਵੈਚਾਲਿਤ& ਇੰਦਰਾਜ : unisets::kCurrencyEntries) {
        UnicodeString exemplar(ਇੰਦਰਾਜ.exemplar);
        ਅਚਲ Unicode_ਸੈੱਟ* ਸੈੱਟ = unisets::get(ਇੰਦਰਾਜ.ਕੁੰਜੀ);
        ਜੇ (ਸੈੱਟ == ਖਾਲੀ_ਸੂਚਕ) { ਵਾਪਸ; }
        Unicode_ਸੈੱਟ_Iterator it(*ਸੈੱਟ);
        ਜਦੋਂ (it.next()) {
            UnicodeString value = it.getString();
            ਜੇ (value == exemplar) {
                // No need to mark the exemplar character as an equivalent
                ਜਾਰੀ;
            }
            makeEquivalent(exemplar, value, hash, ਸਥਿਤੀ);
            ਜੇ (U_FAILURE(ਸਥਿਤੀ)) { ਵਾਪਸ; }
        }
    }
}

ਸਥਿਰ ਖਾਲੀ U_CALLCONV initCurrSymbolsEquiv() {
    U_ASSERT(gCurrSymbolsEquiv == ਖਾਲੀ_ਸੂਚਕ);
    UError_ਕੋਡ ਸਥਿਤੀ = U_ZERO_ERROR;
    ucln_common_ਰਜਿਸਟਰ_Cleanup(UCLN_COMMON_CURRENCY, currency_cleanup);
    icu::Hashtable *ਆਰਜ਼ੀ = ਨਵਾਂ icu::Hashtable(ਸਥਿਤੀ);
    ਜੇ (ਆਰਜ਼ੀ == ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ;
    }
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਮਿਟਾਓ ਆਰਜ਼ੀ;
        ਵਾਪਸ;
    }
    ਆਰਜ਼ੀ->ਸੈੱਟ_ਮੁੱਲ_Deleter(deleteUnicode);
    populateCurrSymbolsEquiv(ਆਰਜ਼ੀ, ਸਥਿਤੀ);
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਮਿਟਾਓ ਆਰਜ਼ੀ;
        ਵਾਪਸ;
    }
    gCurrSymbolsEquiv = ਆਰਜ਼ੀ;
}

U_CAPI UBool U_EXPORT2
ucurr_is_ਉਪਲਬਧ(ਅਚਲ char16_t* iso_ਕੋਡ, UDate from, UDate ਮੰਜ਼ਿਲ, UError_ਕੋਡ* e_Error_ਕੋਡ) {
    umtx_initOnce(gIsoCodesInitOnce, &initIsoCodes, *e_Error_ਕੋਡ);
    ਜੇ (U_FAILURE(*e_Error_ਕੋਡ)) {
        ਵਾਪਸ ਝੂਠ;
    }

    Iso_ਕੋਡ_ਇੰਦਰਾਜ* ਨਤੀਜਾ = (Iso_ਕੋਡ_ਇੰਦਰਾਜ *) uhash_get(gIsoCodes, iso_ਕੋਡ);
    ਜੇ (ਨਤੀਜਾ == ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ ਝੂਠ;
    } ਨਹੀਂਤਾਂ ਜੇ (from > ਮੰਜ਼ਿਲ) {
        *e_Error_ਕੋਡ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ ਝੂਠ;
    } ਨਹੀਂਤਾਂ ਜੇ  ((from > ਨਤੀਜਾ->ਮੰਜ਼ਿਲ) || (ਮੰਜ਼ਿਲ < ਨਤੀਜਾ->from)) {
        ਵਾਪਸ ਝੂਠ;
    }
    ਵਾਪਸ ਸੱਚ;
}

ਸਥਿਰ ਅਚਲ icu::Hashtable* getCurrSymbolsEquiv() {
    umtx_initOnce(gCurrSymbolsEquivInitOnce, &initCurrSymbolsEquiv);
    ਵਾਪਸ gCurrSymbolsEquiv;
}

U_CAPI UEnumeration * U_EXPORT2
ucurr_openISOCurrencies(uint32_t currType, UError_ਕੋਡ *p_Error_ਕੋਡ) {
    UEnumeration *myEnum = ਖਾਲੀ_ਸੂਚਕ;
    UCurrencyContext *myContext;

    myEnum = (UEnumeration*)uprv_malloc(ਆਕਾਰ(UEnumeration));
    ਜੇ (myEnum == ਖਾਲੀ_ਸੂਚਕ) {
        *p_Error_ਕੋਡ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    uprv_memcpy(myEnum, &gEnumCurrencyList, ਆਕਾਰ(UEnumeration));
    myContext = (UCurrencyContext*)uprv_malloc(ਆਕਾਰ(UCurrencyContext));
    ਜੇ (myContext == ਖਾਲੀ_ਸੂਚਕ) {
        *p_Error_ਕੋਡ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        uprv_free(myEnum);
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    myContext->currType = currType;
    myContext->listIdx = 0;
    myEnum->context = myContext;
    ਵਾਪਸ myEnum;
}

U_CAPI int32_t U_EXPORT2
ucurr_ਗਿਣਤੀ_Currencies(ਅਚਲ ਅੱਖਰ* locale, 
                 UDate date, 
                 UError_ਕੋਡ* ec)
{
    int32_t curr_ਗਿਣਤੀ = 0;

    ਜੇ (ec != ਖਾਲੀ_ਸੂਚਕ && U_SUCCESS(*ec)) 
    {
        // local variables
        UError_ਕੋਡ local_ਸਥਿਤੀ = U_ZERO_ERROR;

        // get country or country_variant in `id'
        CharString id = idForLocale(locale, ec);

        ਜੇ (U_FAILURE(*ec))
        {
            ਵਾਪਸ 0;
        }

        // Remove variants, which is only needed for registration.
        ਅੱਖਰ *idDelim = strchr(id.ਅੰਕੜੇ(), VAR_DELIM);
        ਜੇ (idDelim)
        {
            id.truncate(idDelim - id.ਅੰਕੜੇ());
        }

        // Look up the CurrencyMap element in the root bundle.
        UResourceBundle *rb = ures_open_ਸਿੱਧਾ(U_ICUDATA_CURR, CURRENCY_ਅੰਕੜੇ, &local_ਸਥਿਤੀ);
        UResourceBundle *cm = ures_get_By_ਕੁੰਜੀ(rb, CURRENCY_MAP, rb, &local_ਸਥਿਤੀ);

        // Using the id derived from the local, get the currency data
        Local_UResource_Bundle_ਸੂਚਕ countryArray(ures_get_By_ਕੁੰਜੀ(rb, id.ਅੰਕੜੇ(), cm, &local_ਸਥਿਤੀ));

        // process each currency to see which one is valid for the given date
        ਜੇ (U_SUCCESS(local_ਸਥਿਤੀ))
        {
            ਲਈ (int32_t i=0; i<ures_get_ਮਾਪ(countryArray.getAlias()); i++)
            {
                // get the currency resource
                Local_UResource_Bundle_ਸੂਚਕ currencyRes(ures_get_By_ਸੂਚਕਾਂਕ(countryArray.getAlias(), i, ਖਾਲੀ_ਸੂਚਕ, &local_ਸਥਿਤੀ));

                // get the from date
                int32_t from_ਲੰਮਾਈ = 0;
                Local_UResource_Bundle_ਸੂਚਕ fromRes(ures_get_By_ਕੁੰਜੀ(currencyRes.getAlias(), "from", ਖਾਲੀ_ਸੂਚਕ, &local_ਸਥਿਤੀ));
                ਅਚਲ int32_t *fromArray = ures_get_Int_ਸਦਿਸ਼(fromRes.getAlias(), &from_ਲੰਮਾਈ, &local_ਸਥਿਤੀ);

                int64_t currDate64 = (int64_t)((uint64_t)(fromArray[0]) << 32);
                currDate64 |= ((int64_t)fromArray[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                UDate fromDate = (UDate)currDate64;

                ਜੇ (ures_get_ਮਾਪ(currencyRes.getAlias())> 2)
                {
                    int32_t ਮੰਜ਼ਿਲ_ਲੰਮਾਈ = 0;
                    Local_UResource_Bundle_ਸੂਚਕ ਮੰਜ਼ਿਲ_Res(ures_get_By_ਕੁੰਜੀ(currencyRes.getAlias(), "to", ਖਾਲੀ_ਸੂਚਕ, &local_ਸਥਿਤੀ));
                    ਅਚਲ int32_t *ਮੰਜ਼ਿਲ_Array = ures_get_Int_ਸਦਿਸ਼(ਮੰਜ਼ਿਲ_Res.getAlias(), &ਮੰਜ਼ਿਲ_ਲੰਮਾਈ, &local_ਸਥਿਤੀ);

                    currDate64 = (int64_t)ਮੰਜ਼ਿਲ_Array[0] << 32;
                    currDate64 |= ((int64_t)ਮੰਜ਼ਿਲ_Array[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                    UDate ਮੰਜ਼ਿਲ_Date = (UDate)currDate64;

                    ਜੇ ((fromDate <= date) && (date < ਮੰਜ਼ਿਲ_Date))
                    {
                        curr_ਗਿਣਤੀ++;
                    }
                }
                ਨਹੀਂਤਾਂ
                {
                    ਜੇ (fromDate <= date)
                    {
                        curr_ਗਿਣਤੀ++;
                    }
                }
            } // end For loop
        } // end if (U_SUCCESS(localStatus))

        // Check for errors
        ਜੇ (*ec == U_ZERO_ERROR || local_ਸਥਿਤੀ != U_ZERO_ERROR)
        {
            // There is nothing to fallback to. 
            // Report the failure/warning if possible.
            *ec = local_ਸਥਿਤੀ;
        }

        ਜੇ (U_SUCCESS(*ec))
        {
            // no errors
            ਵਾਪਸ curr_ਗਿਣਤੀ;
        }
    }

    // If we got here, either error code is invalid or
    // some argument passed is no good.
    ਵਾਪਸ 0;
}

U_CAPI int32_t U_EXPORT2 
ucurr_forLocaleAndDate(ਅਚਲ ਅੱਖਰ* locale, 
                UDate date, 
                int32_t ਸੂਚਕਾਂਕ,
                char16_t* buff,
                int32_t buff_ਸਮਰੱਥਾ, 
                UError_ਕੋਡ* ec)
{
    int32_t resLen = 0;
	int32_t curr_ਸੂਚਕਾਂਕ = 0;
    ਅਚਲ char16_t* s = ਖਾਲੀ_ਸੂਚਕ;

    ਜੇ (ec != ਖਾਲੀ_ਸੂਚਕ && U_SUCCESS(*ec))
    {
        // check the arguments passed
        ਜੇ ((buff && buff_ਸਮਰੱਥਾ) || !buff_ਸਮਰੱਥਾ )
        {
            // local variables
            UError_ਕੋਡ local_ਸਥਿਤੀ = U_ZERO_ERROR;

            // get country or country_variant in `id'
            CharString id = idForLocale(locale, ec);
            ਜੇ (U_FAILURE(*ec))
            {
                ਵਾਪਸ 0;
            }

            // Remove variants, which is only needed for registration.
            ਅੱਖਰ *idDelim = strchr(id.ਅੰਕੜੇ(), VAR_DELIM);
            ਜੇ (idDelim)
            {
                id.truncate(idDelim - id.ਅੰਕੜੇ());
            }

            // Look up the CurrencyMap element in the root bundle.
            UResourceBundle *rb = ures_open_ਸਿੱਧਾ(U_ICUDATA_CURR, CURRENCY_ਅੰਕੜੇ, &local_ਸਥਿਤੀ);
            UResourceBundle *cm = ures_get_By_ਕੁੰਜੀ(rb, CURRENCY_MAP, rb, &local_ਸਥਿਤੀ);

            // Using the id derived from the local, get the currency data
            Local_UResource_Bundle_ਸੂਚਕ countryArray(ures_get_By_ਕੁੰਜੀ(rb, id.ਅੰਕੜੇ(), cm, &local_ਸਥਿਤੀ));

            // process each currency to see which one is valid for the given date
            ਤਾਰਕਿਕ ਮੇਲ_ਮਿਲਿਆ = ਝੂਠ;
            ਜੇ (U_SUCCESS(local_ਸਥਿਤੀ))
            {
                ਜੇ ((ਸੂਚਕਾਂਕ <= 0) || (ਸੂਚਕਾਂਕ> ures_get_ਮਾਪ(countryArray.getAlias())))
                {
                    // requested index is out of bounds
                    ਵਾਪਸ 0;
                }

                ਲਈ (int32_t i=0; i<ures_get_ਮਾਪ(countryArray.getAlias()); i++)
                {
                    // get the currency resource
                    Local_UResource_Bundle_ਸੂਚਕ currencyRes(ures_get_By_ਸੂਚਕਾਂਕ(countryArray.getAlias(), i, ਖਾਲੀ_ਸੂਚਕ, &local_ਸਥਿਤੀ));
                    s = ures_get_String_By_ਕੁੰਜੀ(currencyRes.getAlias(), "id", &resLen, &local_ਸਥਿਤੀ);

                    // get the from date
                    int32_t from_ਲੰਮਾਈ = 0;
                    Local_UResource_Bundle_ਸੂਚਕ fromRes(ures_get_By_ਕੁੰਜੀ(currencyRes.getAlias(), "from", ਖਾਲੀ_ਸੂਚਕ, &local_ਸਥਿਤੀ));
                    ਅਚਲ int32_t *fromArray = ures_get_Int_ਸਦਿਸ਼(fromRes.getAlias(), &from_ਲੰਮਾਈ, &local_ਸਥਿਤੀ);

                    int64_t currDate64 = (int64_t)((uint64_t)fromArray[0] << 32);
                    currDate64 |= ((int64_t)fromArray[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                    UDate fromDate = (UDate)currDate64;

                    ਜੇ (ures_get_ਮਾਪ(currencyRes.getAlias()) > 2)
                    {
                        int32_t ਮੰਜ਼ਿਲ_ਲੰਮਾਈ = 0;
                        Local_UResource_Bundle_ਸੂਚਕ ਮੰਜ਼ਿਲ_Res(ures_get_By_ਕੁੰਜੀ(currencyRes.getAlias(), "to", ਖਾਲੀ_ਸੂਚਕ, &local_ਸਥਿਤੀ));
                        ਅਚਲ int32_t *ਮੰਜ਼ਿਲ_Array = ures_get_Int_ਸਦਿਸ਼(ਮੰਜ਼ਿਲ_Res.getAlias(), &ਮੰਜ਼ਿਲ_ਲੰਮਾਈ, &local_ਸਥਿਤੀ);

                        currDate64 = (int64_t)ਮੰਜ਼ਿਲ_Array[0] << 32;
                        currDate64 |= ((int64_t)ਮੰਜ਼ਿਲ_Array[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                        UDate ਮੰਜ਼ਿਲ_Date = (UDate)currDate64;

                        ਜੇ ((fromDate <= date) && (date < ਮੰਜ਼ਿਲ_Date))
                        {
                            curr_ਸੂਚਕਾਂਕ++;
                            ਜੇ (curr_ਸੂਚਕਾਂਕ == ਸੂਚਕਾਂਕ)
                            {
                                ਮੇਲ_ਮਿਲਿਆ = ਸੱਚ;
                            }
                        }
                    }
                    ਨਹੀਂਤਾਂ
                    {
                        ਜੇ (fromDate <= date)
                        {
                            curr_ਸੂਚਕਾਂਕ++;
                            ਜੇ (curr_ਸੂਚਕਾਂਕ == ਸੂਚਕਾਂਕ)
                            {
                                ਮੇਲ_ਮਿਲਿਆ = ਸੱਚ;
                            }
                        }
                    }
                    // check for loop exit
                    ਜੇ (ਮੇਲ_ਮਿਲਿਆ)
                    {
                        ਰੋਕੋ;
                    }

                } // end For loop
            }

            // Check for errors
            ਜੇ (*ec == U_ZERO_ERROR || local_ਸਥਿਤੀ != U_ZERO_ERROR)
            {
                // There is nothing to fallback to. 
                // Report the failure/warning if possible.
                *ec = local_ਸਥਿਤੀ;
            }

            ਜੇ (U_SUCCESS(*ec))
            {
                // no errors
                ਜੇ((buff_ਸਮਰੱਥਾ> resLen) && ਮੇਲ_ਮਿਲਿਆ)
                {
                    // write out the currency value
                    u_strcpy(buff, s);
                }
                ਨਹੀਂਤਾਂ
                {
                    ਵਾਪਸ 0;
                }
            }

            // return null terminated currency string
            ਵਾਪਸ u_terminateUChars(buff, buff_ਸਮਰੱਥਾ, resLen, ec);
        }
        ਨਹੀਂਤਾਂ
        {
            // illegal argument encountered
            *ec = U_ILLEGAL_ਦਲੀਲ_ERROR;
        }

    }

    // If we got here, either error code is invalid or
    // some argument passed is no good.
    ਵਾਪਸ resLen;
}

ਸਥਿਰ ਅਚਲ UEnumeration default_Keyword_ਮੁੱਲਾਂ = {
    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ,
    ulist_close_keyword_ਮੁੱਲਾਂ_iterator,
    ulist_ਗਿਣਤੀ_keyword_ਮੁੱਲਾਂ,
    uenum_unextDefault,
    ulist_ਅਗਲਾ_keyword_ਮੁੱਲ, 
    ulist_reset_keyword_ਮੁੱਲਾਂ_iterator
};

U_CAPI UEnumeration *U_EXPORT2 ucurr_get_Keyword_ਮੁੱਲਾਂ_For_Locale(ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ, ਅਚਲ ਅੱਖਰ *locale, UBool commonly_ਵਰਤਿਆ, UError_ਕੋਡ* ਸਥਿਤੀ) {
    // Resolve region
    CharString prefRegion = ulocimp_get_Region_For_Supplemental_ਅੰਕੜੇ(locale, ਸੱਚ, *ਸਥਿਤੀ);

    // Read value from supplementalData
    UList *ਮੁੱਲਾਂ = ulist_create_ਖਾਲੀ_ਥਾਵਾਂ_List(ਸਥਿਤੀ);
    UList *other_ਮੁੱਲਾਂ = ulist_create_ਖਾਲੀ_ਥਾਵਾਂ_List(ਸਥਿਤੀ);
    UEnumeration *en = (UEnumeration *)uprv_malloc(ਆਕਾਰ(UEnumeration));
    ਜੇ (U_FAILURE(*ਸਥਿਤੀ) || en == ਖਾਲੀ_ਸੂਚਕ) {
        ਜੇ (en == ਖਾਲੀ_ਸੂਚਕ) {
            *ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        } ਨਹੀਂਤਾਂ {
            uprv_free(en);
        }
        ulist_deleteList(ਮੁੱਲਾਂ);
        ulist_deleteList(other_ਮੁੱਲਾਂ);
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਸਮ੍ਰਿਤੀ_ਨਕਲ(en, &default_Keyword_ਮੁੱਲਾਂ, ਆਕਾਰ(UEnumeration));
    en->context = ਮੁੱਲਾਂ;
    
    UResourceBundle* rb = ures_open_ਸਿੱਧਾ(U_ICUDATA_CURR, "supplementalData", ਸਥਿਤੀ);
    Local_UResource_Bundle_ਸੂਚਕ bundle(ures_get_By_ਕੁੰਜੀ(rb, "CurrencyMap", rb, ਸਥਿਤੀ));
    ਢੇਰ_UResource_Bundle bundlekey, regbndl, curbndl, ਮੰਜ਼ਿਲ;
    
    ਜਦੋਂ (U_SUCCESS(*ਸਥਿਤੀ) && ures_has_ਅਗਲਾ(bundle.getAlias())) {
        ures_get_ਅਗਲਾ_Resource(bundle.getAlias(), bundlekey.getAlias(), ਸਥਿਤੀ);
        ਜੇ (U_FAILURE(*ਸਥਿਤੀ)) {
            ਰੋਕੋ;
        }
        ਅਚਲ ਅੱਖਰ *region = ures_get_ਕੁੰਜੀ(bundlekey.getAlias());
        UBool isPrefRegion = prefRegion == region;
        ਜੇ (!isPrefRegion && commonly_ਵਰਤਿਆ) {
            // With commonlyUsed=true, we do not put
            // currencies for other regions in the
            // result list.
            ਜਾਰੀ;
        }
        ures_get_By_ਕੁੰਜੀ(bundle.getAlias(), region, regbndl.getAlias(), ਸਥਿਤੀ);
        ਜੇ (U_FAILURE(*ਸਥਿਤੀ)) {
            ਰੋਕੋ;
        }
        ਜਦੋਂ (U_SUCCESS(*ਸਥਿਤੀ) && ures_has_ਅਗਲਾ(regbndl.getAlias())) {
            ures_get_ਅਗਲਾ_Resource(regbndl.getAlias(), curbndl.getAlias(), ਸਥਿਤੀ);
            ਜੇ (ures_getType(curbndl.getAlias()) != URES_ਸਾਰਣੀ) {
                // Currently, an empty ARRAY is mixed in.
                ਜਾਰੀ;
            }
            ਅੱਖਰ *curID = (ਅੱਖਰ *)uprv_malloc(ਆਕਾਰ(ਅੱਖਰ) * ULOC_KEYWORDS_ਸਮਰੱਥਾ);
            ਜੇ (curID == ਖਾਲੀ_ਸੂਚਕ) {
                *ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
                ਰੋਕੋ;
            }
            int32_t curIDLength = ULOC_KEYWORDS_ਸਮਰੱਥਾ;

#ਜੇ U_CHARSET_FAMILY==U_ASCII_FAMILY
            ures_get_UTF8_String_By_ਕੁੰਜੀ(curbndl.getAlias(), "id", curID, &curIDLength, ਸੱਚ, ਸਥਿਤੀ);
            /* optimize - use the utf-8 string */
#ਨਹੀਂਤਾਂ
            {
                       ਅਚਲ char16_t* defString = ures_get_String_By_ਕੁੰਜੀ(curbndl.getAlias(), "id", &curIDLength, ਸਥਿਤੀ);
                       ਜੇ(U_SUCCESS(*ਸਥਿਤੀ)) {
			   ਜੇ(curIDLength+1 > ULOC_KEYWORDS_ਸਮਰੱਥਾ) {
				*ਸਥਿਤੀ = U_ਬਫਰ_ਉਛਾਲ_ERROR;
			   } ਨਹੀਂਤਾਂ {
                           	u_UChars_ਮੰਜ਼ਿਲ_Chars(defString, curID, curIDLength+1);
			   }
                       }
            }
#ਜੇ_ਅੰਤ	

            ਜੇ (U_FAILURE(*ਸਥਿਤੀ)) {
                ਰੋਕੋ;
            }
            UBool has_ਮੰਜ਼ਿਲ = ਝੂਠ;
            ures_get_By_ਕੁੰਜੀ(curbndl.getAlias(), "to", ਮੰਜ਼ਿਲ.getAlias(), ਸਥਿਤੀ);
            ਜੇ (U_FAILURE(*ਸਥਿਤੀ)) {
                // Do nothing here...
                *ਸਥਿਤੀ = U_ZERO_ERROR;
            } ਨਹੀਂਤਾਂ {
                has_ਮੰਜ਼ਿਲ = ਸੱਚ;
            }
            ਜੇ (isPrefRegion && !has_ਮੰਜ਼ਿਲ && !ulist_ਸ਼ਾਮਲ_ਹੈ_String(ਮੁੱਲਾਂ, curID, (int32_t)uprv_strlen(curID))) {
                // Currently active currency for the target country
                ulist_ਜੋੜੋ_ਵਸਤੂ_ਸਿਰਾ_List(ਮੁੱਲਾਂ, curID, ਸੱਚ, ਸਥਿਤੀ);
            } ਨਹੀਂਤਾਂ ਜੇ (!ulist_ਸ਼ਾਮਲ_ਹੈ_String(other_ਮੁੱਲਾਂ, curID, (int32_t)uprv_strlen(curID)) && !commonly_ਵਰਤਿਆ) {
                ulist_ਜੋੜੋ_ਵਸਤੂ_ਸਿਰਾ_List(other_ਮੁੱਲਾਂ, curID, ਸੱਚ, ਸਥਿਤੀ);
            } ਨਹੀਂਤਾਂ {
                uprv_free(curID);
            }
        }
        
    }
    ਜੇ (U_SUCCESS(*ਸਥਿਤੀ)) {
        ਜੇ (commonly_ਵਰਤਿਆ) {
            ਜੇ (ulist_get_List_ਮਾਪ(ਮੁੱਲਾਂ) == 0) {
                // This could happen if no valid region is supplied in the input
                // locale. In this case, we use the CLDR's default.
                uenum_close(en);
                en = ucurr_get_Keyword_ਮੁੱਲਾਂ_For_Locale(ਕੁੰਜੀ, "und", ਸੱਚ, ਸਥਿਤੀ);
            }
        } ਨਹੀਂਤਾਂ {
            // Consolidate the list
            ਅੱਖਰ *value = ਖਾਲੀ_ਸੂਚਕ;
            ulist_resetList(other_ਮੁੱਲਾਂ);
            ਜਦੋਂ ((value = (ਅੱਖਰ *)ulist_get_ਅਗਲਾ(other_ਮੁੱਲਾਂ)) != ਖਾਲੀ_ਸੂਚਕ) {
                ਜੇ (!ulist_ਸ਼ਾਮਲ_ਹੈ_String(ਮੁੱਲਾਂ, value, (int32_t)uprv_strlen(value))) {
                    ਅੱਖਰ *tmp_ਮੁੱਲ = (ਅੱਖਰ *)uprv_malloc(ਆਕਾਰ(ਅੱਖਰ) * ULOC_KEYWORDS_ਸਮਰੱਥਾ);
                    ਜੇ (tmp_ਮੁੱਲ == ਖਾਲੀ_ਸੂਚਕ) {
                        *ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
                        ਰੋਕੋ;
                    }
                    uprv_memcpy(tmp_ਮੁੱਲ, value, uprv_strlen(value) + 1);
                    ulist_ਜੋੜੋ_ਵਸਤੂ_ਸਿਰਾ_List(ਮੁੱਲਾਂ, tmp_ਮੁੱਲ, ਸੱਚ, ਸਥਿਤੀ);
                    ਜੇ (U_FAILURE(*ਸਥਿਤੀ)) {
                        ਰੋਕੋ;
                    }
                }
            }
        }
        ulist_resetList((UList *)(en->context));
    } ਨਹੀਂਤਾਂ {
        ulist_deleteList(ਮੁੱਲਾਂ);
        uprv_free(en);
        ਮੁੱਲਾਂ = ਖਾਲੀ_ਸੂਚਕ;
        en = ਖਾਲੀ_ਸੂਚਕ;
    }
    ulist_deleteList(other_ਮੁੱਲਾਂ);
    ਵਾਪਸ en;
}


U_CAPI int32_t U_EXPORT2
ucurr_get_Numeric_ਕੋਡ(ਅਚਲ char16_t* currency) {
    int32_t ਕੋਡ = 0;
    ਜੇ (currency && u_strlen(currency) == ISO_CURRENCY_ਕੋਡ_ਲੰਮਾਈ) {
        UError_ਕੋਡ ਸਥਿਤੀ = U_ZERO_ERROR;

        UResourceBundle* bundle = ures_open_ਸਿੱਧਾ(ਖਾਲੀ_ਸੂਚਕ, "currencyNumericCodes", &ਸਥਿਤੀ);
        Local_UResource_Bundle_ਸੂਚਕ ਕੋਡ_Map(ures_get_By_ਕੁੰਜੀ(bundle, "codeMap", bundle, &ਸਥਿਤੀ));
        ਜੇ (U_SUCCESS(ਸਥਿਤੀ)) {
            ਅੱਖਰ alpha_ਕੋਡ[ISO_CURRENCY_ਕੋਡ_ਲੰਮਾਈ+1];
            my_UChars_ਮੰਜ਼ਿਲ_Chars(alpha_ਕੋਡ, currency);
            T_CString_ਮੰਜ਼ਿਲ_ਉੱਤਲਾ_Case(alpha_ਕੋਡ);
            ures_get_By_ਕੁੰਜੀ(ਕੋਡ_Map.getAlias(), alpha_ਕੋਡ, ਕੋਡ_Map.getAlias(), &ਸਥਿਤੀ);
            ਪੂਰਨ tmp_ਕੋਡ = ures_getInt(ਕੋਡ_Map.getAlias(), &ਸਥਿਤੀ);
            ਜੇ (U_SUCCESS(ਸਥਿਤੀ)) {
                ਕੋਡ = tmp_ਕੋਡ;
            }
        }
    }
    ਵਾਪਸ ਕੋਡ;
}
#ਜੇ_ਅੰਤ /* #if !UCONFIG_NO_FORMATTING */

//eof
