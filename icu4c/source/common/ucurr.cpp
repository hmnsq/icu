// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
* Copyright (c) 2002-2016, International Business Machines
* Corporation and others.  All Rights Reserved.
**********************************************************************
*/

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಯದಿ !UCONFIG_NO_FORMATTING

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

using ನಾಮಾಕಾಶ icu;

//#define UCURR_DEBUG_EQUIV 1
#ifdef UCURR_DEBUG_EQUIV
#ಅನ್ತರ್ಭಾವ "stdio.h"
#endif
//#define UCURR_DEBUG 1
#ifdef UCURR_DEBUG
#ಅನ್ತರ್ಭಾವ "stdio.h"
#endif

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ IsoCodeEntry {
    ಧ್ರುವ char16_t *isoCode; /* const because it's a reference to a resource bundle string. */
    UDate from;
    UDate to;
} IsoCodeEntry;

//------------------------------------------------------------
// Constants

// Default currency meta data of last resort.  We try to use the
// defaults encoded in the meta data resource bundle.  If there is a
// configuration/build error and these are not available, we use these
// hard-coded defaults (which should be identical).
ಸ್ಥಿರ ಧ್ರುವ int32_t LAST_RESORT_DATA[] = { 2, 0, 2, 0 };

// POW10[i] = 10^i, i=0..MAX_POW10
ಸ್ಥಿರ ಧ್ರುವ int32_t POW10[] = { 1, 10, 100, 1000, 10000, 100000,
                                 1000000, 10000000, 100000000, 1000000000 };

ಸ್ಥಿರ ಧ್ರುವ int32_t MAX_POW10 = UPRV_LENGTHOF(POW10) - 1;

#define ISO_CURRENCY_CODE_LENGTH 3

//------------------------------------------------------------
// Resource tags
//

ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ CURRENCY_DATA[] = "supplementalData";
// Tag for meta-data, in root.
ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ CURRENCY_META[] = "CurrencyMeta";

// Tag for map from countries to currencies, in root.
ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ CURRENCY_MAP[] = "CurrencyMap";

// Tag for default meta-data, in CURRENCY_META
ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ DEFAULT_META[] = "DEFAULT";

// Variant delimiter
ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ VAR_DELIM = '_';

// Tag for localized display names (symbols) of currencies
ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ CURRENCIES[] = "Currencies";
ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ CURRENCIES_NARROW[] = "Currencies%narrow";
ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ CURRENCIES_FORMAL[] = "Currencies%formal";
ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ CURRENCIES_VARIANT[] = "Currencies%variant";
ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ CURRENCYPLURALS[] = "CurrencyPlurals";

// ISO codes mapping table
ಸ್ಥಿರ ಧ್ರುವ UHashtable* gIsoCodes = nullptr;
ಸ್ಥಿರ icu::UInitOnce gIsoCodesInitOnce {};

// Currency symbol equivalances
ಸ್ಥಿರ ಧ್ರುವ icu::Hashtable* gCurrSymbolsEquiv = nullptr;
ಸ್ಥಿರ icu::UInitOnce gCurrSymbolsEquivInitOnce {};

U_NAMESPACE_BEGIN

// EquivIterator iterates over all strings that are equivalent to a given
// string, s. Note that EquivIterator will never yield s itself.
ವರ್ಗ EquivIterator : ಸಾರ್ವಜನಿಕ icu::UMemory {
ಸಾರ್ವಜನಿಕ:
    // Constructor. hash stores the equivalence relationships; s is the string
    // for which we find equivalent strings.
    ಅನ್ತಃಸ್ಥ EquivIterator(ಧ್ರುವ icu::Hashtable& hash, ಧ್ರುವ icu::UnicodeString& s)
        : _hash(hash) { 
        _start = _current = &s;
    }
    ಅನ್ತಃಸ್ಥ ~EquivIterator() { }

    // next returns the next equivalent string or nullptr if there are no more.
    // If s has no equivalent strings, next returns nullptr on the first call.
    ಧ್ರುವ icu::UnicodeString *next();
ಗುಪ್ತ:
    ಧ್ರುವ icu::Hashtable& _hash;
    ಧ್ರುವ icu::UnicodeString* _start;
    ಧ್ರುವ icu::UnicodeString* _current;
};

ಧ್ರುವ icu::UnicodeString *
EquivIterator::next() {
    ಧ್ರುವ icu::UnicodeString* _next = static_cast<ಧ್ರುವ icu::UnicodeString*>(_hash.get(*_current));
    ಯದಿ (_next == nullptr) {
        U_ASSERT(_current == _start);
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    ಯದಿ (*_next == *_start) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    _current = _next;
    ಪ್ರತ್ಯಾಗಮ _next;
}

U_NAMESPACE_END

// makeEquivalent makes lhs and rhs equivalent by updating the equivalence
// relations in hash accordingly.
ಸ್ಥಿರ ಶೂನ್ಯ makeEquivalent(
    ಧ್ರುವ icu::UnicodeString &lhs,
    ಧ್ರುವ icu::UnicodeString &rhs,
    icu::Hashtable* hash, UErrorCode &status) {
    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (lhs == rhs) {
        // already equivalent
        ಪ್ರತ್ಯಾಗಮ;
    }
    icu::EquivIterator ಎಡ_Iter(*hash, lhs);
    icu::EquivIterator ಬಲ_Iter(*hash, rhs);
    ಧ್ರುವ icu::UnicodeString *first_ಎಡ = ಎಡ_Iter.next();
    ಧ್ರುವ icu::UnicodeString *first_ಬಲ = ಬಲ_Iter.next();
    ಧ್ರುವ icu::UnicodeString *next_ಎಡ = first_ಎಡ;
    ಧ್ರುವ icu::UnicodeString *next_ಬಲ = first_ಬಲ;
    ಯಾವತ್ (next_ಎಡ != nullptr && next_ಬಲ != nullptr) {
        ಯದಿ (*next_ಎಡ == rhs || *next_ಬಲ == lhs) {
            // Already equivalent
            ಪ್ರತ್ಯಾಗಮ;
        }
        next_ಎಡ = ಎಡ_Iter.next();
        next_ಬಲ = ಬಲ_Iter.next();
    }
    // Not equivalent. Must join.
    icu::UnicodeString *new_First_ಎಡ;
    icu::UnicodeString *new_First_ಬಲ;
    ಯದಿ (first_ಬಲ == nullptr && first_ಎಡ == nullptr) {
        // Neither lhs or rhs belong to an equivalence circle, so we form
        // a new equivalnce circle of just lhs and rhs.
        new_First_ಎಡ = ನೂತನ icu::UnicodeString(rhs);
        new_First_ಬಲ = ನೂತನ icu::UnicodeString(lhs);
    } ಅನ್ಯಥಾ ಯದಿ (first_ಬಲ == nullptr) {
        // lhs belongs to an equivalence circle, but rhs does not, so we link
        // rhs into lhs' circle.
        new_First_ಎಡ = ನೂತನ icu::UnicodeString(rhs);
        new_First_ಬಲ = ನೂತನ icu::UnicodeString(*first_ಎಡ);
    } ಅನ್ಯಥಾ ಯದಿ (first_ಎಡ == nullptr) {
        // rhs belongs to an equivlance circle, but lhs does not, so we link
        // lhs into rhs' circle.
        new_First_ಎಡ = ನೂತನ icu::UnicodeString(*first_ಬಲ);
        new_First_ಬಲ = ನೂತನ icu::UnicodeString(lhs);
    } ಅನ್ಯಥಾ {
        // Both lhs and rhs belong to different equivalnce circles. We link
        // them together to form one single, larger equivalnce circle.
        new_First_ಎಡ = ನೂತನ icu::UnicodeString(*first_ಬಲ);
        new_First_ಬಲ = ನೂತನ icu::UnicodeString(*first_ಎಡ);
    }
    ಯದಿ (new_First_ಎಡ == nullptr || new_First_ಬಲ == nullptr) {
        delete new_First_ಎಡ;
        delete new_First_ಬಲ;
        status = U_MEMORY_ALLOCATION_ERROR;
        ಪ್ರತ್ಯಾಗಮ;
    }
    hash->put(lhs, (ಶೂನ್ಯ *) new_First_ಎಡ, status);
    hash->put(rhs, (ಶೂನ್ಯ *) new_First_ಬಲ, status);
}

// countEquivalent counts how many strings are equivalent to s.
// hash stores all the equivalnce relations.
// countEquivalent does not include s itself in the count.
ಸ್ಥಿರ int32_t ಎಣಿಕೆ_Equivalent(ಧ್ರುವ icu::Hashtable &hash, ಧ್ರುವ icu::UnicodeString &s) {
    int32_t result = 0;
    icu::EquivIterator iter(hash, s);
    ಯಾವತ್ (iter.next() != nullptr) {
        ++result;
    }
#ifdef UCURR_DEBUG_EQUIV
 {
   ವರ್ಣ tmp[200];
   s.extract(0,s.length(),tmp, "UTF-8");
   printf("CountEquivalent('%s') = %d\n", tmp, result);
 }
#endif
    ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ ಧ್ರುವ icu::Hashtable* getCurrSymbolsEquiv();

//------------------------------------------------------------
// Code

/**
 * Cleanup callback func
 */
ಸ್ಥಿರ UBool U_CALLCONV 
isoCodes_cleanup()
{
    ಯದಿ (gIsoCodes != nullptr) {
        uhash_close(const_cast<UHashtable *>(gIsoCodes));
        gIsoCodes = nullptr;
    }
    gIsoCodesInitOnce.reset();
    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

/**
 * Cleanup callback func
 */
ಸ್ಥಿರ UBool U_CALLCONV 
currSymbolsEquiv_cleanup()
{
    delete const_cast<icu::Hashtable *>(gCurrSymbolsEquiv);
    gCurrSymbolsEquiv = nullptr;
    gCurrSymbolsEquivInitOnce.reset();
    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

/**
 * Deleter for IsoCodeEntry
 */
ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
deleteIsoCodeEntry(ಶೂನ್ಯ *obj) {
    IsoCodeEntry* entry = static_cast<IsoCodeEntry*>(obj);
    uprv_free(entry);
}

/**
 * Deleter for gCurrSymbolsEquiv.
 */
ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
deleteUnicode(ಶೂನ್ಯ *obj) {
    icu::UnicodeString* entry = static_cast<icu::UnicodeString*>(obj);
    delete entry;
}

/**
 * Unfortunately, we have to convert the char16_t* currency code to char*
 * to use it as a resource key.
 */
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ವರ್ಣ*
myUCharsToChars(ವರ್ಣ* resultOfLen4, ಧ್ರುವ char16_t* currency) {
    u_UCharsToChars(currency, resultOfLen4, ISO_CURRENCY_CODE_LENGTH);
    resultOfLen4[ISO_CURRENCY_CODE_LENGTH] = 0;
    ಪ್ರತ್ಯಾಗಮ resultOfLen4;
}

/**
 * Internal function to look up currency data.  Result is an array of
 * four integers.  The first is the fraction digits.  The second is the
 * rounding increment, or 0 if none.  The rounding increment is in
 * units of 10^(-fraction_digits).  The third and fourth are the same
 * except that they are those used in cash transactions ( cashDigits
 * and cashRounding ).
 */
ಸ್ಥಿರ ಧ್ರುವ int32_t*
_findMetaData(ಧ್ರುವ char16_t* currency, UErrorCode& ec) {

    ಯದಿ (currency == nullptr || *currency == 0) {
        ಯದಿ (U_SUCCESS(ec)) {
            ec = U_ILLEGAL_ARGUMENT_ERROR;
        }
        ಪ್ರತ್ಯಾಗಮ LAST_RESORT_DATA;
    }

    // Get CurrencyMeta resource out of root locale file.  [This may
    // move out of the root locale file later; if it does, update this
    // code.]
    UResourceBundle* currencyData = ures_openDirect(U_ICUDATA_CURR, CURRENCY_DATA, &ec);
    LocalUResourceBundlePointer currencyMeta(ures_get_By_ಕೀಲಿ(currencyData, CURRENCY_META, currencyData, &ec));

    ಯದಿ (U_FAILURE(ec)) {
        // Config/build error; return hard-coded defaults
        ಪ್ರತ್ಯಾಗಮ LAST_RESORT_DATA;
    }

    // Look up our currency, or if that's not available, then DEFAULT
    ವರ್ಣ buf[ISO_CURRENCY_CODE_LENGTH+1];
    UErrorCode ec2 = U_ZERO_ERROR; // local error code: soft failure
    LocalUResourceBundlePointer rb(ures_get_By_ಕೀಲಿ(currencyMeta.getAlias(), myUCharsToChars(buf, currency), nullptr, &ec2));
      ಯದಿ (U_FAILURE(ec2)) {
        rb.adoptInstead(ures_get_By_ಕೀಲಿ(currencyMeta.getAlias(),DEFAULT_META, nullptr, &ec));
        ಯದಿ (U_FAILURE(ec)) {
            // Config/build error; return hard-coded defaults
            ಪ್ರತ್ಯಾಗಮ LAST_RESORT_DATA;
        }
    }

    int32_t len;
    ಧ್ರುವ int32_t *data = ures_getIntVector(rb.getAlias(), &len, &ec);
    ಯದಿ (U_FAILURE(ec) || len != 4) {
        // Config/build error; return hard-coded defaults
        ಯದಿ (U_SUCCESS(ec)) {
            ec = U_INVALID_FORMAT_ERROR;
        }
        ಪ್ರತ್ಯಾಗಮ LAST_RESORT_DATA;
    }

    ಪ್ರತ್ಯಾಗಮ data;
}

// -------------------------------------

ಸ್ಥಿರ CharString
idForLocale(ಧ್ರುವ ವರ್ಣ* locale, UErrorCode* ec)
{
    ಪ್ರತ್ಯಾಗಮ ulocimp_getRegionForSupplementalData(locale, ಅಸತ್ಯ, *ec);
}

// ------------------------------------------
//
// Registration
//
//-------------------------------------------

// don't use ICUService since we don't need fallback

U_CDECL_BEGIN
ಸ್ಥಿರ UBool U_CALLCONV currency_cleanup();
U_CDECL_END

#ಯದಿ !UCONFIG_NO_SERVICE
ಸಂರಚನಾ CReg;

ಸ್ಥಿರ UMutex gCRegLock;
ಸ್ಥಿರ CReg* gCRegHead = nullptr;

ಸಂರಚನಾ CReg : ಸಾರ್ವಜನಿಕ icu::UMemory {
    CReg *next;
    char16_t iso[ISO_CURRENCY_CODE_LENGTH+1];
    ವರ್ಣ  id[ULOC_FULLNAME_CAPACITY];

    CReg(ಧ್ರುವ char16_t* _iso, ಧ್ರುವ ವರ್ಣ* _id)
        : next(nullptr)
    {
        uprv_strncpy(id, _id, ಪರಿಮಾಣ(id)-1);
        id[ಪರಿಮಾಣ(id)-1] = 0;
        u_memcpy(iso, _iso, ISO_CURRENCY_CODE_LENGTH);
        iso[ISO_CURRENCY_CODE_LENGTH] = 0;
    }

    ಸ್ಥಿರ UCurr_Registry_ಕೀಲಿ reg(ಧ್ರುವ char16_t* _iso, ಧ್ರುವ ವರ್ಣ* _id, UErrorCode* status)
    {
        ಯದಿ (status && U_SUCCESS(*status) && _iso && _id) {
            CReg* n = ನೂತನ CReg(_iso, _id);
            ಯದಿ (n) {
                umtx_lock(&gCRegLock);
                ಯದಿ (!gCRegHead) {
                    /* register for the first time */
                    ucln_common_registerCleanup(UCLN_COMMON_CURRENCY, currency_cleanup);
                }
                n->next = gCRegHead;
                gCRegHead = n;
                umtx_unlock(&gCRegLock);
                ಪ್ರತ್ಯಾಗಮ n;
            }
            *status = U_MEMORY_ALLOCATION_ERROR;
        }
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }

    ಸ್ಥಿರ UBool unreg(UCurr_Registry_ಕೀಲಿ ಕೀಲಿ) {
        UBool found = ಅಸತ್ಯ;
        umtx_lock(&gCRegLock);

        CReg** p = &gCRegHead;
        ಯಾವತ್ (*p) {
            ಯದಿ (*p == ಕೀಲಿ) {
                *p = ((CReg*)ಕೀಲಿ)->next;
                delete (CReg*)ಕೀಲಿ;
                found = ಸತ್ಯ;
                ಭಙ್ಗ;
            }
            p = &((*p)->next);
        }

        umtx_unlock(&gCRegLock);
        ಪ್ರತ್ಯಾಗಮ found;
    }

    ಸ್ಥಿರ ಧ್ರುವ char16_t* get(ಧ್ರುವ ವರ್ಣ* id) {
        ಧ್ರುವ char16_t* result = nullptr;
        umtx_lock(&gCRegLock);
        CReg* p = gCRegHead;

        /* register cleanup of the mutex */
        ucln_common_registerCleanup(UCLN_COMMON_CURRENCY, currency_cleanup);
        ಯಾವತ್ (p) {
            ಯದಿ (uprv_strcmp(id, p->id) == 0) {
                result = p->iso;
                ಭಙ್ಗ;
            }
            p = p->next;
        }
        umtx_unlock(&gCRegLock);
        ಪ್ರತ್ಯಾಗಮ result;
    }

    /* This doesn't need to be thread safe. It's for u_cleanup only. */
    ಸ್ಥಿರ ಶೂನ್ಯ cleanup() {
        ಯಾವತ್ (gCRegHead) {
            CReg* n = gCRegHead;
            gCRegHead = gCRegHead->next;
            delete n;
        }
    }
};

// -------------------------------------

U_CAPI UCurr_Registry_ಕೀಲಿ U_EXPORT2
ucurr_register(ಧ್ರುವ char16_t* isoCode, ಧ್ರುವ ವರ್ಣ* locale, UErrorCode *status)
{
    ಯದಿ (status && U_SUCCESS(*status)) {
        CharString id = idForLocale(locale, status);
        ಪ್ರತ್ಯಾಗಮ CReg::reg(isoCode, id.data(), status);
    }
    ಪ್ರತ್ಯಾಗಮ nullptr;
}

// -------------------------------------

U_CAPI UBool U_EXPORT2
ucurr_unregister(UCurr_Registry_ಕೀಲಿ ಕೀಲಿ, UErrorCode* status)
{
    ಯದಿ (status && U_SUCCESS(*status)) {
        ಪ್ರತ್ಯಾಗಮ CReg::unreg(ಕೀಲಿ);
    }
    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
}
#endif /* UCONFIG_NO_SERVICE */

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
#ಯದಿ !UCONFIG_NO_SERVICE
    CReg::cleanup();
#endif
    /*
     * There might be some cached currency data or isoCodes data.
     */
    currency_cache_cleanup();
    isoCodes_cleanup();
    currSymbolsEquiv_cleanup();

    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}
U_CDECL_END

// -------------------------------------

U_CAPI int32_t U_EXPORT2
ucurr_forLocale(ಧ್ರುವ ವರ್ಣ* locale,
                char16_t* buff,
                int32_t buffCapacity,
                UErrorCode* ec) {
    ಯದಿ (U_FAILURE(*ec)) { ಪ್ರತ್ಯಾಗಮ 0; }
    ಯದಿ (buffCapacity < 0 || (buff == nullptr && buffCapacity > 0)) {
        *ec = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ 0;
    }

    UErrorCode localStatus = U_ZERO_ERROR;
    CharString currency = ulocimp_get_Keyword_ಮೌಲ್ಯ(locale, "currency", localStatus);
    int32_t resLen = currency.length();

    ಯದಿ (U_SUCCESS(localStatus) && resLen == 3 && uprv_isInvariantString(currency.data(), resLen)) {
        ಯದಿ (resLen < buffCapacity) {
            T_CString_toUpperCase(currency.data());
            u_charsToUChars(currency.data(), buff, resLen);
        }
        ಪ್ರತ್ಯಾಗಮ u_terminateUChars(buff, buffCapacity, resLen, ec);
    }

    // get country or country_variant in `id'
    CharString id = idForLocale(locale, ec);
    ಯದಿ (U_FAILURE(*ec)) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }

#ಯದಿ !UCONFIG_NO_SERVICE
    ಧ್ರುವ char16_t* result = CReg::get(id.data());
    ಯದಿ (result) {
        ಯದಿ(buffCapacity > u_strlen(result)) {
            u_strcpy(buff, result);
        }
        resLen = u_strlen(result);
        ಪ್ರತ್ಯಾಗಮ u_terminateUChars(buff, buffCapacity, resLen, ec);
    }
#endif
    // Remove variants, which is only needed for registration.
    ವರ್ಣ *idDelim = uprv_strchr(id.data(), VAR_DELIM);
    ಯದಿ (idDelim) {
        id.truncate(idDelim - id.data());
    }

    ಧ್ರುವ char16_t* s = nullptr;  // Currency code from data file.
    ಯದಿ (id.isEmpty()) {
        // No point looking in the data for an empty string.
        // This is what we would get.
        localStatus = U_MISSING_RESOURCE_ERROR;
    } ಅನ್ಯಥಾ {
        // Look up the CurrencyMap element in the root bundle.
        localStatus = U_ZERO_ERROR;
        UResourceBundle *rb = ures_openDirect(U_ICUDATA_CURR, CURRENCY_DATA, &localStatus);
        UResourceBundle *cm = ures_get_By_ಕೀಲಿ(rb, CURRENCY_MAP, rb, &localStatus);
        LocalUResourceBundlePointer countryArray(ures_get_By_ಕೀಲಿ(rb, id.data(), cm, &localStatus));
        // https://unicode-org.atlassian.net/browse/ICU-21997
        // Prefer to use currencies that are legal tender.
        ಯದಿ (U_SUCCESS(localStatus)) {
            int32_t arrayLength = ures_getSize(countryArray.getAlias());
            ಪ್ರತಿ (int32_t i = 0; i < arrayLength; ++i) {
                LocalUResourceBundlePointer currencyReq(
                    ures_getByIndex(countryArray.getAlias(), i, nullptr, &localStatus));
                // The currency is legal tender if it is *not* marked with tender{"false"}.
                UErrorCode tenderStatus = localStatus;
                ಧ್ರುವ char16_t *tender =
                    ures_get_String_By_ಕೀಲಿ(currencyReq.getAlias(), "tender", nullptr, &tenderStatus);
                ತಾರ್ಕಿಕ isTender = U_FAILURE(tenderStatus) || u_strcmp(tender, u"false") != 0;
                ಯದಿ (!isTender && s != nullptr) {
                    // We already have a non-tender currency. Ignore all following non-tender ones.
                    ಅನುವರ್ತ;
                }
                // Fetch the currency code.
                s = ures_get_String_By_ಕೀಲಿ(currencyReq.getAlias(), "id", &resLen, &localStatus);
                ಯದಿ (isTender) {
                    ಭಙ್ಗ;
                }
            }
            ಯದಿ (U_SUCCESS(localStatus) && s == nullptr) {
                localStatus = U_MISSING_RESOURCE_ERROR;
            }
        }
    }

    ಯದಿ ((U_FAILURE(localStatus)) && strchr(id.data(), '_') != nullptr) {
        // We don't know about it.  Check to see if we support the variant.
        CharString parent = ulocimp_getParent(locale, *ec);
        *ec = U_USING_FALLBACK_WARNING;
        // TODO: Loop over the parent rather than recursing and
        // looking again for a currency keyword.
        ಪ್ರತ್ಯಾಗಮ ucurr_forLocale(parent.data(), buff, buffCapacity, ec);
    }
    ಯದಿ (*ec == U_ZERO_ERROR || localStatus != U_ZERO_ERROR) {
        // There is nothing to fallback to. Report the failure/warning if possible.
        *ec = localStatus;
    }
    ಯದಿ (U_SUCCESS(*ec)) {
        ಯದಿ(buffCapacity > resLen) {
            u_strcpy(buff, s);
        }
    }
    ಪ್ರತ್ಯಾಗಮ u_terminateUChars(buff, buffCapacity, resLen, ec);
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
    ಯದಿ (loc.isEmpty()) {
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    UErrorCode status = U_ZERO_ERROR;
    ಯದಿ (loc == "en_GB") {
        // HACK: See #13368.  We need "en_GB" to fall back to "en_001" instead of "en"
        // in order to consume the correct data strings.  This hack will be removed
        // when proper data sink loading is implemented here.
        loc.truncate(3);
        loc.append("001", status);
    } ಅನ್ಯಥಾ {
        loc = ulocimp_getParent(loc.data(), status);
    }
 /*
    char *i = uprv_strrchr(loc, '_');
    if (i == nullptr) {
        i = loc;
    }
    *i = 0;
 */
    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}


U_CAPI ಧ್ರುವ char16_t* U_EXPORT2
ucurr_getName(ಧ್ರುವ char16_t* currency U_LIFETIME_BOUND,
              ಧ್ರುವ ವರ್ಣ* locale,
              UCurrNameStyle nameStyle,
              UBool* isChoiceFormat, // fillin
              int32_t* len, // fillin
              UErrorCode* ec) {

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

    ಯದಿ (U_FAILURE(*ec)) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }

    int32_t choice = (int32_t) nameStyle;
    ಯದಿ (choice < 0 || choice > 4) {
        *ec = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ nullptr;
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
    UErrorCode ec2 = U_ZERO_ERROR;

    ಯದಿ (locale == nullptr) {
        locale = uloc_getDefault();
    }
    CharString loc = ulocimp_getName(locale, ec2);
    ಯದಿ (U_FAILURE(ec2)) {
        *ec = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }

    ವರ್ಣ buf[ISO_CURRENCY_CODE_LENGTH+1];
    myUCharsToChars(buf, currency);
    
    /* Normalize the keyword value to uppercase */
    T_CString_toUpperCase(buf);
    
    ಧ್ರುವ char16_t* s = nullptr;
    ec2 = U_ZERO_ERROR;
    LocalUResourceBundlePointer rb(ures_open(U_ICUDATA_CURR, loc.data(), &ec2));

    ಯದಿ (nameStyle == UCURR_NARROW_ಚಿಹ್ನೆ_NAME || nameStyle == UCURR_FORMAL_ಚಿಹ್ನೆ_NAME || nameStyle == UCURR_VARIANT_ಚಿಹ್ನೆ_NAME) {
        CharString ಕೀಲಿ;
        ವಿಕಲ್ಪ (nameStyle) {
        ಪಕ್ಷ UCURR_NARROW_ಚಿಹ್ನೆ_NAME:
            ಕೀಲಿ.append(CURRENCIES_NARROW, ec2);
            ಭಙ್ಗ;
        ಪಕ್ಷ UCURR_FORMAL_ಚಿಹ್ನೆ_NAME:
            ಕೀಲಿ.append(CURRENCIES_FORMAL, ec2);
            ಭಙ್ಗ;
        ಪಕ್ಷ UCURR_VARIANT_ಚಿಹ್ನೆ_NAME:
            ಕೀಲಿ.append(CURRENCIES_VARIANT, ec2);
            ಭಙ್ಗ;
        ಸ್ವಭಾವ:
            *ec = U_UNSUPPORTED_ERROR;
            ಪ್ರತ್ಯಾಗಮ nullptr;
        }
        ಕೀಲಿ.append("/", ec2);
        ಕೀಲಿ.append(buf, ec2);
        s = ures_get_String_By_ಕೀಲಿ_With_Fallback(rb.getAlias(), ಕೀಲಿ.data(), len, &ec2);
        ಯದಿ (ec2 == U_MISSING_RESOURCE_ERROR) {
            *ec = U_USING_FALLBACK_WARNING;
            ec2 = U_ZERO_ERROR;
            choice = UCURR_ಚಿಹ್ನೆ_NAME;
        }
    }
    ಯದಿ (s == nullptr) {
        ures_get_By_ಕೀಲಿ(rb.getAlias(), CURRENCIES, rb.getAlias(), &ec2);
        ures_get_By_ಕೀಲಿ_With_Fallback(rb.getAlias(), buf, rb.getAlias(), &ec2);
        s = ures_getStringByIndex(rb.getAlias(), choice, len, &ec2);
    }

    // If we've succeeded we're done.  Otherwise, try to fallback.
    // If that fails (because we are already at root) then exit.
    ಯದಿ (U_SUCCESS(ec2)) {
        ಯದಿ (ec2 == U_USING_DEFAULT_WARNING
            || (ec2 == U_USING_FALLBACK_WARNING && *ec != U_USING_DEFAULT_WARNING)) {
            *ec = ec2;
        }
    }

    // We no longer support choice format data in names.  Data should not contain
    // choice patterns.
    ಯದಿ (isChoiceFormat != nullptr) {
        *isChoiceFormat = ಅಸತ್ಯ;
    }
    ಯದಿ (U_SUCCESS(ec2)) {
        U_ASSERT(s != nullptr);
        ಪ್ರತ್ಯಾಗಮ s;
    }

    // If we fail to find a match, use the ISO 4217 code
    *len = u_strlen(currency); // Should == ISO_CURRENCY_CODE_LENGTH, but maybe not...?
    *ec = U_USING_DEFAULT_WARNING;
    ಪ್ರತ್ಯಾಗಮ currency;
}

U_CAPI ಧ್ರುವ char16_t* U_EXPORT2
ucurr_getPluralName(ಧ್ರುವ char16_t* currency U_LIFETIME_BOUND,
                    ಧ್ರುವ ವರ್ಣ* locale,
                    UBool* isChoiceFormat,
                    ಧ್ರುವ ವರ್ಣ* plural_ಎಣಿಕೆ,
                    int32_t* len, // fillin
                    UErrorCode* ec) {
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

    ಯದಿ (U_FAILURE(*ec)) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }

    // Use a separate UErrorCode here that does not propagate out of
    // this function.
    UErrorCode ec2 = U_ZERO_ERROR;

    ಯದಿ (locale == nullptr) {
        locale = uloc_getDefault();
    }
    CharString loc = ulocimp_getName(locale, ec2);
    ಯದಿ (U_FAILURE(ec2)) {
        *ec = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }

    ವರ್ಣ buf[ISO_CURRENCY_CODE_LENGTH+1];
    myUCharsToChars(buf, currency);

    ಧ್ರುವ char16_t* s = nullptr;
    ec2 = U_ZERO_ERROR;
    UResourceBundle* rb = ures_open(U_ICUDATA_CURR, loc.data(), &ec2);

    rb = ures_get_By_ಕೀಲಿ(rb, CURRENCYPLURALS, rb, &ec2);

    // Fetch resource with multi-level resource inheritance fallback
    LocalUResourceBundlePointer curr(ures_get_By_ಕೀಲಿ_With_Fallback(rb, buf, rb, &ec2));

    s = ures_get_String_By_ಕೀಲಿ_With_Fallback(curr.getAlias(), plural_ಎಣಿಕೆ, len, &ec2);
    ಯದಿ (U_FAILURE(ec2)) {
        //  fall back to "other"
        ec2 = U_ZERO_ERROR;
        s = ures_get_String_By_ಕೀಲಿ_With_Fallback(curr.getAlias(), "other", len, &ec2);     
        ಯದಿ (U_FAILURE(ec2)) {
            // fall back to long name in Currencies
            ಪ್ರತ್ಯಾಗಮ ucurr_getName(currency, locale, UCURR_LONG_NAME, 
                                 isChoiceFormat, len, ec);
        }
    }

    // If we've succeeded we're done.  Otherwise, try to fallback.
    // If that fails (because we are already at root) then exit.
    ಯದಿ (U_SUCCESS(ec2)) {
        ಯದಿ (ec2 == U_USING_DEFAULT_WARNING
            || (ec2 == U_USING_FALLBACK_WARNING && *ec != U_USING_DEFAULT_WARNING)) {
            *ec = ec2;
        }
        U_ASSERT(s != nullptr);
        ಪ್ರತ್ಯಾಗಮ s;
    }

    // If we fail to find a match, use the ISO 4217 code
    *len = u_strlen(currency); // Should == ISO_CURRENCY_CODE_LENGTH, but maybe not...?
    *ec = U_USING_DEFAULT_WARNING;
    ಪ್ರತ್ಯಾಗಮ currency;
}


//========================================================================
// Following are structure and function for parsing currency names

#define NEED_TO_BE_DELETED 0x1

// TODO: a better way to define this?
#define MAX_CURRENCY_NAME_LEN 100

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
    ಧ್ರುವ ವರ್ಣ* IsoCode;  //ಕೀಲಿ
    char16_t* currencyName;  //ಮೌಲ್ಯ
    int32_t currencyNameLen;  // value length
    int32_t flag;  // flags
} CurrencyNameStruct;


#ifndef MIN
#define MIN(a,b) (((a)<(b)) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a,b) (((a)<(b)) ? (b) : (a))
#endif


// Comparison function used in quick sort.
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ U_CALLCONV currencyNameComparator(ಧ್ರುವ ಶೂನ್ಯ* a, ಧ್ರುವ ಶೂನ್ಯ* b) {
    ಧ್ರುವ CurrencyNameStruct* currName_1 = static_cast<ಧ್ರುವ CurrencyNameStruct*>(a);
    ಧ್ರುವ CurrencyNameStruct* currName_2 = static_cast<ಧ್ರುವ CurrencyNameStruct*>(b);
    ಪ್ರತಿ (int32_t i = 0; 
         i < MIN(currName_1->currencyNameLen, currName_2->currencyNameLen);
         ++i) {
        ಯದಿ (currName_1->currencyName[i] < currName_2->currencyName[i]) {
            ಪ್ರತ್ಯಾಗಮ -1;
        }
        ಯದಿ (currName_1->currencyName[i] > currName_2->currencyName[i]) {
            ಪ್ರತ್ಯಾಗಮ 1;
        }
    }
    ಯದಿ (currName_1->currencyNameLen < currName_2->currencyNameLen) {
        ಪ್ರತ್ಯಾಗಮ -1;
    } ಅನ್ಯಥಾ ಯದಿ (currName_1->currencyNameLen > currName_2->currencyNameLen) {
        ಪ್ರತ್ಯಾಗಮ 1;
    }
    ಪ್ರತ್ಯಾಗಮ 0;
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
get_Currency_Name_ಎಣಿಕೆ(ಧ್ರುವ ವರ್ಣ* loc, int32_t* total_currency_name_ಎಣಿಕೆ, int32_t* total_currency_ಚಿಹ್ನೆ_ಎಣಿಕೆ) {
    U_NAMESPACE_USE
    *total_currency_name_ಎಣಿಕೆ = 0;
    *total_currency_ಚಿಹ್ನೆ_ಎಣಿಕೆ = 0;
    ಧ್ರುವ char16_t* s = nullptr;
    CharString locale;
    {
        UErrorCode status = U_ZERO_ERROR;
        locale.append(loc, status);
        ಯದಿ (U_FAILURE(status)) { ಪ್ರತ್ಯಾಗಮ; }
    }
    ಧ್ರುವ icu::Hashtable *currencySymbolsEquiv = getCurrSymbolsEquiv();
    ಪ್ರತಿ (;;) {
        UErrorCode ec2 = U_ZERO_ERROR;
        // TODO: ures_openDirect?
        LocalUResourceBundlePointer rb(ures_open(U_ICUDATA_CURR, locale.data(), &ec2));
        LocalUResourceBundlePointer curr(ures_get_By_ಕೀಲಿ(rb.getAlias(), CURRENCIES, nullptr, &ec2));
        int32_t n = ures_getSize(curr.getAlias());
        ಪ್ರತಿ (int32_t i=0; i<n; ++i) {
            LocalUResourceBundlePointer names(ures_getByIndex(curr.getAlias(), i, nullptr, &ec2));
            int32_t len;
            s = ures_getStringByIndex(names.getAlias(), UCURR_ಚಿಹ್ನೆ_NAME, &len, &ec2);
            ++(*total_currency_ಚಿಹ್ನೆ_ಎಣಿಕೆ);  // currency symbol
            ಯದಿ (currencySymbolsEquiv != nullptr) {
                *total_currency_ಚಿಹ್ನೆ_ಎಣಿಕೆ += ಎಣಿಕೆ_Equivalent(*currencySymbolsEquiv, UnicodeString(ಸತ್ಯ, s, len));
            }
            ++(*total_currency_ಚಿಹ್ನೆ_ಎಣಿಕೆ); // iso code
            ++(*total_currency_name_ಎಣಿಕೆ); // long name
        }

        // currency plurals
        UErrorCode ec3 = U_ZERO_ERROR;
        LocalUResourceBundlePointer curr_p(ures_get_By_ಕೀಲಿ(rb.getAlias(), CURRENCYPLURALS, nullptr, &ec3));
        n = ures_getSize(curr_p.getAlias());
        ಪ್ರತಿ (int32_t i=0; i<n; ++i) {
            LocalUResourceBundlePointer names(ures_getByIndex(curr_p.getAlias(), i, nullptr, &ec3));
            *total_currency_name_ಎಣಿಕೆ += ures_getSize(names.getAlias());
        }

        ಯದಿ (!fallback(locale)) {
            ಭಙ್ಗ;
        }
    }
}

ಸ್ಥಿರ char16_t*
toUpperCase(ಧ್ರುವ char16_t* source, int32_t len, ಧ್ರುವ ವರ್ಣ* locale) {
    char16_t* dest = nullptr;
    UErrorCode ec = U_ZERO_ERROR;
    int32_t destLen = u_strToUpper(dest, 0, source, len, locale, &ec);

    ec = U_ZERO_ERROR;
    dest = static_cast<char16_t*>(uprv_malloc(ಪರಿಮಾಣ(char16_t) * MAX(destLen, len)));
    ಯದಿ (dest == nullptr) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    u_strToUpper(dest, destLen, source, len, locale, &ec);
    ಯದಿ (U_FAILURE(ec)) {
        u_memcpy(dest, source, len);
    } 
    ಪ್ರತ್ಯಾಗಮ dest;
}


ಸ್ಥಿರ ಶೂನ್ಯ deleteCurrencyNames(CurrencyNameStruct* currencyNames, int32_t count);
// Collect all available currency names associated with the given locale
// (enable fallback chain).
// Read currenc names defined in resource bundle "Currencies" and
// "CurrencyPlural", enable fallback chain.
// return the malloc-ed currency name arrays and the total number of currency
// names in the array.
ಸ್ಥಿರ ಶೂನ್ಯ
collectCurrencyNames(ಧ್ರುವ ವರ್ಣ* locale, 
                     CurrencyNameStruct** currencyNames, 
                     int32_t* total_currency_name_ಎಣಿಕೆ, 
                     CurrencyNameStruct** currencySymbols, 
                     int32_t* total_currency_ಚಿಹ್ನೆ_ಎಣಿಕೆ, 
                     UErrorCode& ec) {
    ಯದಿ (U_FAILURE(ec)) {
        *currencyNames = *currencySymbols = nullptr;
        *total_currency_name_ಎಣಿಕೆ = *total_currency_ಚಿಹ್ನೆ_ಎಣಿಕೆ = 0;
        ಪ್ರತ್ಯಾಗಮ;
    }
    U_NAMESPACE_USE
    ಧ್ರುವ icu::Hashtable *currencySymbolsEquiv = getCurrSymbolsEquiv();
    // Look up the Currencies resource for the given locale.
    UErrorCode ec2 = U_ZERO_ERROR;

    ಯದಿ (locale == nullptr) {
        locale = uloc_getDefault();
    }
    CharString loc = ulocimp_getName(locale, ec2);
    ಯದಿ (U_FAILURE(ec2)) {
        ec = U_ILLEGAL_ARGUMENT_ERROR;
        *currencyNames = *currencySymbols = nullptr;
        *total_currency_name_ಎಣಿಕೆ = *total_currency_ಚಿಹ್ನೆ_ಎಣಿಕೆ = 0;
        ಪ್ರತ್ಯಾಗಮ;
    }

    // Get maximum currency name count first.
    get_Currency_Name_ಎಣಿಕೆ(loc.data(), total_currency_name_ಎಣಿಕೆ, total_currency_ಚಿಹ್ನೆ_ಎಣಿಕೆ);

    *currencyNames = static_cast<CurrencyNameStruct*>(
        uprv_malloc(ಪರಿಮಾಣ(CurrencyNameStruct) * (*total_currency_name_ಎಣಿಕೆ)));
    ಯದಿ(*currencyNames == nullptr) {
        *currencySymbols = nullptr;
        *total_currency_name_ಎಣಿಕೆ = *total_currency_ಚಿಹ್ನೆ_ಎಣಿಕೆ = 0;
        ec = U_MEMORY_ALLOCATION_ERROR;
        ಪ್ರತ್ಯಾಗಮ;
    }
    *currencySymbols = static_cast<CurrencyNameStruct*>(
        uprv_malloc(ಪರಿಮಾಣ(CurrencyNameStruct) * (*total_currency_ಚಿಹ್ನೆ_ಎಣಿಕೆ)));

    ಯದಿ(*currencySymbols == nullptr) {
        uprv_free(*currencyNames);
        *currencyNames = nullptr;
        *total_currency_name_ಎಣಿಕೆ = *total_currency_ಚಿಹ್ನೆ_ಎಣಿಕೆ = 0;
        ec = U_MEMORY_ALLOCATION_ERROR;
        ಪ್ರತ್ಯಾಗಮ;
    }

    ಧ್ರುವ char16_t* s = nullptr;  // currency name
    ವರ್ಣ* iso = nullptr;  // currency ISO code

    *total_currency_name_ಎಣಿಕೆ = 0;
    *total_currency_ಚಿಹ್ನೆ_ಎಣಿಕೆ = 0;

    UErrorCode ec3 = U_ZERO_ERROR;
    UErrorCode ec4 = U_ZERO_ERROR;

    // Using hash to remove duplicates caused by locale fallback
    LocalUHashtablePointer currencyIsoCodes(uhash_open(uhash_hashChars, uhash_compareChars, nullptr, &ec3));
    LocalUHashtablePointer currencyPluralIsoCodes(uhash_open(uhash_hashChars, uhash_compareChars, nullptr, &ec4));
    ಪ್ರತಿ (int32_t localeLevel = 0; ; ++localeLevel) {
        ec2 = U_ZERO_ERROR;
        // TODO: ures_openDirect
        LocalUResourceBundlePointer rb(ures_open(U_ICUDATA_CURR, loc.data(), &ec2));
        LocalUResourceBundlePointer curr(ures_get_By_ಕೀಲಿ(rb.getAlias(), CURRENCIES, nullptr, &ec2));
        int32_t n = ures_getSize(curr.getAlias());
        ಪ್ರತಿ (int32_t i=0; i<n; ++i) {
            LocalUResourceBundlePointer names(ures_getByIndex(curr.getAlias(), i, nullptr, &ec2));
            int32_t len;
            s = ures_getStringByIndex(names.getAlias(), UCURR_ಚಿಹ್ನೆ_NAME, &len, &ec2);
            // TODO: uhash_put wont change key/value?
            iso = const_cast<ವರ್ಣ*>(ures_get_ಕೀಲಿ(names.getAlias()));
            ಯದಿ (localeLevel != 0 && uhash_get(currencyIsoCodes.getAlias(), iso) != nullptr) {
                ಅನುವರ್ತ;
            }
            uhash_put(currencyIsoCodes.getAlias(), iso, iso, &ec3); 
            // Add currency symbol.
            (*currencySymbols)[(*total_currency_ಚಿಹ್ನೆ_ಎಣಿಕೆ)++] = {iso, const_cast<char16_t*>(s), len, 0};

            // Add equivalent symbols
            ಯದಿ (currencySymbolsEquiv != nullptr) {
                UnicodeString str(ಸತ್ಯ, s, len);
                icu::EquivIterator iter(*currencySymbolsEquiv, str);
                ಧ್ರುವ UnicodeString *ಚಿಹ್ನೆ;
                ಯಾವತ್ ((ಚಿಹ್ನೆ = iter.next()) != nullptr) {
                    (*currencySymbols)[(*total_currency_ಚಿಹ್ನೆ_ಎಣಿಕೆ)++]
                        = {iso, const_cast<char16_t*>(ಚಿಹ್ನೆ->getBuffer()), ಚಿಹ್ನೆ->length(), 0};
                }
            }

            // Add currency long name.
            s = ures_getStringByIndex(names.getAlias(), UCURR_LONG_NAME, &len, &ec2);
            char16_t* upperName = toUpperCase(s, len, locale);
            ಯದಿ (upperName == nullptr) {
                ec = U_MEMORY_ALLOCATION_ERROR;
                ಗಚ್ಛ error;
            }
            (*currencyNames)[(*total_currency_name_ಎಣಿಕೆ)++] = {iso, upperName, len, NEED_TO_BE_DELETED};

            // put (iso, 3, and iso) in to array
            // Add currency ISO code.
            char16_t* isoCode = static_cast<char16_t*>(uprv_malloc(ಪರಿಮಾಣ(char16_t) * 3));
            ಯದಿ (isoCode == nullptr) {
                ec = U_MEMORY_ALLOCATION_ERROR;
                ಗಚ್ಛ error;
            }
            // Must convert iso[] into Unicode
            u_charsToUChars(iso, isoCode, 3);
            (*currencySymbols)[(*total_currency_ಚಿಹ್ನೆ_ಎಣಿಕೆ)++] = {iso, isoCode, 3, NEED_TO_BE_DELETED};
        }

        // currency plurals
        UErrorCode ec5 = U_ZERO_ERROR;
        LocalUResourceBundlePointer curr_p(ures_get_By_ಕೀಲಿ(rb.getAlias(), CURRENCYPLURALS, nullptr, &ec5));
        n = ures_getSize(curr_p.getAlias());
        ಪ್ರತಿ (int32_t i=0; i<n; ++i) {
            LocalUResourceBundlePointer names(ures_getByIndex(curr_p.getAlias(), i, nullptr, &ec5));
            iso = const_cast<ವರ್ಣ*>(ures_get_ಕೀಲಿ(names.getAlias()));
            // Using hash to remove duplicated ISO codes in fallback chain.
            ಯದಿ (localeLevel != 0 && uhash_get(currencyPluralIsoCodes.getAlias(), iso) != nullptr) {
                ಅನುವರ್ತ;
            }
            uhash_put(currencyPluralIsoCodes.getAlias(), iso, iso, &ec4);
            int32_t num = ures_getSize(names.getAlias());
            int32_t len;
            ಪ್ರತಿ (int32_t j = 0; j < num; ++j) {
                // TODO: remove duplicates between singular name and 
                // currency long name?
                s = ures_getStringByIndex(names.getAlias(), j, &len, &ec5);
                char16_t* upperName = toUpperCase(s, len, locale);
                ಯದಿ (upperName == nullptr) {
                    ec = U_MEMORY_ALLOCATION_ERROR;
                    ಗಚ್ಛ error;
                }
                (*currencyNames)[(*total_currency_name_ಎಣಿಕೆ)++] = {iso, upperName, len, NEED_TO_BE_DELETED};
            }
        }

        ಯದಿ (!fallback(loc)) {
            ಭಙ್ಗ;
        }
    }

    // quick sort the struct
    qsort(*currencyNames, *total_currency_name_ಎಣಿಕೆ, 
          ಪರಿಮಾಣ(CurrencyNameStruct), currencyNameComparator);
    qsort(*currencySymbols, *total_currency_ಚಿಹ್ನೆ_ಎಣಿಕೆ, 
          ಪರಿಮಾಣ(CurrencyNameStruct), currencyNameComparator);

#ifdef UCURR_DEBUG
    printf("currency name count: %d\n", *total_currency_name_ಎಣಿಕೆ);
    ಪ್ರತಿ (int32_t index = 0; index < *total_currency_name_ಎಣಿಕೆ; ++index) {
        printf("index: %d\n", index);
        printf("iso: %s\n", (*currencyNames)[index].IsoCode);
        ವರ್ಣ curNameBuf[1024];
        memset(curNameBuf, 0, 1024);
        u_austrncpy(curNameBuf, (*currencyNames)[index].currencyName, (*currencyNames)[index].currencyNameLen);
        printf("currencyName: %s\n", curNameBuf);
        printf("len: %d\n", (*currencyNames)[index].currencyNameLen);
    }
    printf("currency symbol count: %d\n", *total_currency_ಚಿಹ್ನೆ_ಎಣಿಕೆ);
    ಪ್ರತಿ (int32_t index = 0; index < *total_currency_ಚಿಹ್ನೆ_ಎಣಿಕೆ; ++index) {
        printf("index: %d\n", index);
        printf("iso: %s\n", (*currencySymbols)[index].IsoCode);
        ವರ್ಣ curNameBuf[1024];
        memset(curNameBuf, 0, 1024);
        u_austrncpy(curNameBuf, (*currencySymbols)[index].currencyName, (*currencySymbols)[index].currencyNameLen);
        printf("currencySymbol: %s\n", curNameBuf);
        printf("len: %d\n", (*currencySymbols)[index].currencyNameLen);
    }
#endif
    // fail on hashtable errors
    ಯದಿ (U_FAILURE(ec3)) {
      ec = ec3;
    } ಅನ್ಯಥಾ ಯದಿ (U_FAILURE(ec4)) {
      ec = ec4;
    }

  error:
    // clean up if we got error
    ಯದಿ (U_FAILURE(ec)) {
        deleteCurrencyNames(*currencyNames, *total_currency_name_ಎಣಿಕೆ);
        deleteCurrencyNames(*currencySymbols, *total_currency_ಚಿಹ್ನೆ_ಎಣಿಕೆ);
        *currencyNames = *currencySymbols = nullptr;
        *total_currency_name_ಎಣಿಕೆ = *total_currency_ಚಿಹ್ನೆ_ಎಣಿಕೆ = 0;
    }
}

// @param  currencyNames: currency names array
// @param  indexInCurrencyNames: the index of the character in currency names 
//         array against which the comparison is done
// @param  key: input text char to compare against
// @param  begin(IN/OUT): the begin index of matching range in currency names array
// @param  end(IN/OUT): the end index of matching range in currency names array.
ಸ್ಥಿರ int32_t
ದ್ವಿಮಾನ_ಹುಡುಕು(ಧ್ರುವ CurrencyNameStruct* currencyNames, 
             int32_t indexInCurrencyNames,
             ಧ್ರುವ char16_t ಕೀಲಿ,
             int32_t* begin, int32_t* end) {
#ifdef UCURR_DEBUG
    printf("key = %x\n", ಕೀಲಿ);
#endif
   int32_t first = *begin;
   int32_t last = *end;
   ಯಾವತ್ (first <= last) {
       int32_t mid = (first + last) / 2;  // compute mid point.
       ಯದಿ (indexInCurrencyNames >= currencyNames[mid].currencyNameLen) {
           first = mid + 1;
       } ಅನ್ಯಥಾ {
           ಯದಿ (ಕೀಲಿ > currencyNames[mid].currencyName[indexInCurrencyNames]) {
               first = mid + 1;
           }
           ಅನ್ಯಥಾ ಯದಿ (ಕೀಲಿ < currencyNames[mid].currencyName[indexInCurrencyNames]) {
               last = mid - 1;
           }
           ಅನ್ಯಥಾ {
                // Find a match, and looking for ranges
                // Now do two more binary searches. First, on the left side for
                // the greatest L such that CurrencyNameStruct[L] < key.
                int32_t L = *begin;
                int32_t R = mid;

#ifdef UCURR_DEBUG
                printf("mid = %d\n", mid);
#endif
                ಯಾವತ್ (L < R) {
                    int32_t M = (L + R) / 2;
#ifdef UCURR_DEBUG
                    printf("L = %d, R = %d, M = %d\n", L, R, M);
#endif
                    ಯದಿ (indexInCurrencyNames >= currencyNames[M].currencyNameLen) {
                        L = M + 1;
                    } ಅನ್ಯಥಾ {
                        ಯದಿ (currencyNames[M].currencyName[indexInCurrencyNames] < ಕೀಲಿ) {
                            L = M + 1;
                        } ಅನ್ಯಥಾ {
#ifdef UCURR_DEBUG
                            U_ASSERT(currencyNames[M].currencyName[indexInCurrencyNames] == ಕೀಲಿ);
#endif
                            R = M;
                        }
                    }
                }
#ifdef UCURR_DEBUG
                U_ASSERT(L == R);
#endif
                *begin = L;
#ifdef UCURR_DEBUG
                printf("begin = %d\n", *begin);
                U_ASSERT(currencyNames[*begin].currencyName[indexInCurrencyNames] == ಕೀಲಿ);
#endif

                // Now for the second search, finding the least R such that
                // key < CurrencyNameStruct[R].
                L = mid;
                R = *end;
                ಯಾವತ್ (L < R) {
                    int32_t M = (L + R) / 2;
#ifdef UCURR_DEBUG
                    printf("L = %d, R = %d, M = %d\n", L, R, M);
#endif
                    ಯದಿ (currencyNames[M].currencyNameLen < indexInCurrencyNames) {
                        L = M + 1;
                    } ಅನ್ಯಥಾ {
                        ಯದಿ (currencyNames[M].currencyName[indexInCurrencyNames] > ಕೀಲಿ) {
                            R = M;
                        } ಅನ್ಯಥಾ {
#ifdef UCURR_DEBUG
                            U_ASSERT(currencyNames[M].currencyName[indexInCurrencyNames] == ಕೀಲಿ);
#endif
                            L = M + 1;
                        }
                    }
                }
#ifdef UCURR_DEBUG
                U_ASSERT(L == R);
#endif
                ಯದಿ (currencyNames[R].currencyName[indexInCurrencyNames] > ಕೀಲಿ) {
                    *end = R - 1;
                } ಅನ್ಯಥಾ {
                    *end = R;
                }
#ifdef UCURR_DEBUG
                printf("end = %d\n", *end);
#endif

                // now, found the range. check whether there is exact match
                ಯದಿ (currencyNames[*begin].currencyNameLen == indexInCurrencyNames + 1) {
                    ಪ್ರತ್ಯಾಗಮ *begin;  // find range and exact match.
                }
                ಪ್ರತ್ಯಾಗಮ -1;  // find range, but no exact match.
           }
       }
   }
   *begin = -1;
   *end = -1;
   ಪ್ರತ್ಯಾಗಮ -1;    // failed to find range.
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
linear_ಹುಡುಕು(ಧ್ರುವ CurrencyNameStruct* currencyNames, 
             int32_t begin, int32_t end,
             ಧ್ರುವ char16_t* text, int32_t textLen,
             int32_t *partialMatchLen,
             int32_t *maxMatchLen, int32_t* maxMatchIndex) {
    int32_t initialPartialMatchLen = *partialMatchLen;
    ಪ್ರತಿ (int32_t index = begin; index <= end; ++index) {
        int32_t len = currencyNames[index].currencyNameLen;
        ಯದಿ (len > *maxMatchLen && len <= textLen &&
            uprv_memcmp(currencyNames[index].currencyName, text, len * ಪರಿಮಾಣ(char16_t)) == 0) {
            *partialMatchLen = MAX(*partialMatchLen, len);
            *maxMatchIndex = index;
            *maxMatchLen = len;
#ifdef UCURR_DEBUG
            printf("maxMatchIndex = %d, maxMatchLen = %d\n",
                   *maxMatchIndex, *maxMatchLen);
#endif
        } ಅನ್ಯಥಾ {
            // Check for partial matches.
            ಪ್ರತಿ (int32_t i=initialPartialMatchLen; i<MIN(len, textLen); i++) {
                ಯದಿ (currencyNames[index].currencyName[i] != text[i]) {
                    ಭಙ್ಗ;
                }
                *partialMatchLen = MAX(*partialMatchLen, i + 1);
            }
        }
    }
}

#define LINEAR_ಹುಡುಕು_THRESHOLD 10

// Find longest match between "text" and currency names in "currencyNames".
// @param  total_currency_count: total number of currency names in CurrencyNames.
// @param  textLen: the length of the text to be compared
// @param  maxMatchLen: passing in the computed max matching length
//                              pass out the new max  matching length
// @param  maxMatchIndex: the index in currencyName which has the longest
//                        match with input text.
ಸ್ಥಿರ ಶೂನ್ಯ
ಹುಡುಕು_Currency_Name(ಧ್ರುವ CurrencyNameStruct* currencyNames, 
                   int32_t total_currency_ಎಣಿಕೆ,
                   ಧ್ರುವ char16_t* text, int32_t textLen,
                   int32_t *partialMatchLen,
                   int32_t* maxMatchLen, int32_t* maxMatchIndex) {
    *maxMatchIndex = -1;
    *maxMatchLen = 0;
    int32_t matchIndex = -1;
    int32_t ದ್ವಿಮಾನ_ಹುಡುಕು_Begin = 0;
    int32_t ದ್ವಿಮಾನ_ಹುಡುಕು_End = total_currency_ಎಣಿಕೆ - 1;
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
    ಪ್ರತಿ (int32_t index = 0; index < textLen; ++index) {
        // matchIndex saves the one with exact match till the current point.
        // [binarySearchBegin, binarySearchEnd] saves the matching range.
        matchIndex = ದ್ವಿಮಾನ_ಹುಡುಕು(currencyNames, index,
                                  text[index],
                                  &ದ್ವಿಮಾನ_ಹುಡುಕು_Begin, &ದ್ವಿಮಾನ_ಹುಡುಕು_End);
        ಯದಿ (ದ್ವಿಮಾನ_ಹುಡುಕು_Begin == -1) { // did not find the range
            ಭಙ್ಗ;
        }
        *partialMatchLen = MAX(*partialMatchLen, index + 1);
        ಯದಿ (matchIndex != -1) { 
            // find an exact match for text from text[0] to text[index] 
            // in currencyNames array.
            *maxMatchLen = index + 1;
            *maxMatchIndex = matchIndex;
        }
        ಯದಿ (ದ್ವಿಮಾನ_ಹುಡುಕು_End - ದ್ವಿಮಾನ_ಹುಡುಕು_Begin < LINEAR_ಹುಡುಕು_THRESHOLD) {
            // linear search if within threshold.
            linear_ಹುಡುಕು(currencyNames, ದ್ವಿಮಾನ_ಹುಡುಕು_Begin, ದ್ವಿಮಾನ_ಹುಡುಕು_End,
                         text, textLen,
                         partialMatchLen,
                         maxMatchLen, maxMatchIndex);
            ಭಙ್ಗ;
        }
    }
}

//========================= currency name cache =====================
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
    ವರ್ಣ locale[ULOC_FULLNAME_CAPACITY];  //key
    // currency names, case insensitive
    CurrencyNameStruct* currencyNames;  //ಮೌಲ್ಯ
    int32_t total_Currency_Name_ಎಣಿಕೆ;  // currency name count
    // currency symbols and ISO code, case sensitive
    CurrencyNameStruct* currencySymbols; //ಮೌಲ್ಯ
    int32_t total_Currency_ಚಿಹ್ನೆ_ಎಣಿಕೆ;  //ಎಣಿಕೆ
    // reference count.
    // reference count is set to 1 when an entry is put to cache.
    // it increases by 1 before accessing, and decreased by 1 after accessing.
    // The entry is deleted when ref count is zero, which means 
    // the entry is replaced out of cache and no process is accessing it.
    int32_t ref_ಎಣಿಕೆ;
} CurrencyNameCacheEntry;


#define CURRENCY_NAME_CACHE_ಅಂಕೆ 10

// Reserve 10 cache entries.
ಸ್ಥಿರ CurrencyNameCacheEntry* currCache[CURRENCY_NAME_CACHE_ಅಂಕೆ] = {nullptr};
// Using an index to indicate which entry to be replaced when cache is full.
// It is a simple round-robin replacement strategy.
ಸ್ಥಿರ int8_t currentCacheEntryIndex = 0;

ಸ್ಥಿರ UMutex gCurrencyCacheMutex;

// Cache deletion
ಸ್ಥಿರ ಶೂನ್ಯ
deleteCurrencyNames(CurrencyNameStruct* currencyNames, int32_t count) {
    ಪ್ರತಿ (int32_t index = 0; index < count; ++index) {
        ಯದಿ ( (currencyNames[index].flag & NEED_TO_BE_DELETED) ) {
            uprv_free(currencyNames[index].currencyName);
        }
    }
    uprv_free(currencyNames);
}


ಸ್ಥಿರ ಶೂನ್ಯ
deleteCacheEntry(CurrencyNameCacheEntry* entry) {
    deleteCurrencyNames(entry->currencyNames, entry->total_Currency_Name_ಎಣಿಕೆ);
    deleteCurrencyNames(entry->currencySymbols, entry->total_Currency_ಚಿಹ್ನೆ_ಎಣಿಕೆ);
    uprv_free(entry);
}


// Cache clean up
ಸ್ಥಿರ UBool U_CALLCONV
currency_cache_cleanup() {
    ಪ್ರತಿ (int32_t i = 0; i < CURRENCY_NAME_CACHE_ಅಂಕೆ; ++i) {
        ಯದಿ (currCache[i]) {
            deleteCacheEntry(currCache[i]);
            currCache[i] = nullptr;
        }
    }
    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}


/**
 * Loads the currency name data from the cache, or from resource bundles if necessary.
 * The refCount is automatically incremented.  It is the caller's responsibility
 * to decrement it when done!
 */
ಸ್ಥಿರ CurrencyNameCacheEntry*
getCacheEntry(ಧ್ರುವ ವರ್ಣ* locale, UErrorCode& ec) {

    int32_t total_currency_name_ಎಣಿಕೆ = 0;
    CurrencyNameStruct* currencyNames = nullptr;
    int32_t total_currency_ಚಿಹ್ನೆ_ಎಣಿಕೆ = 0;
    CurrencyNameStruct* currencySymbols = nullptr;
    CurrencyNameCacheEntry* cacheEntry = nullptr;

    umtx_lock(&gCurrencyCacheMutex);
    // in order to handle racing correctly,
    // not putting 'search' in a separate function.
    int8_t found = -1;
    ಪ್ರತಿ (int8_t i = 0; i < CURRENCY_NAME_CACHE_ಅಂಕೆ; ++i) {
        ಯದಿ (currCache[i]!= nullptr &&
            uprv_strcmp(locale, currCache[i]->locale) == 0) {
            found = i;
            ಭಙ್ಗ;
        }
    }
    ಯದಿ (found != -1) {
        cacheEntry = currCache[found];
        ++(cacheEntry->ref_ಎಣಿಕೆ);
    }
    umtx_unlock(&gCurrencyCacheMutex);
    ಯದಿ (found == -1) {
        collectCurrencyNames(locale, &currencyNames, &total_currency_name_ಎಣಿಕೆ, &currencySymbols, &total_currency_ಚಿಹ್ನೆ_ಎಣಿಕೆ, ec);
        ಯದಿ (U_FAILURE(ec)) {
            ಪ್ರತ್ಯಾಗಮ nullptr;
        }
        umtx_lock(&gCurrencyCacheMutex);
        // check again.
        ಪ್ರತಿ (int8_t i = 0; i < CURRENCY_NAME_CACHE_ಅಂಕೆ; ++i) {
            ಯದಿ (currCache[i]!= nullptr &&
                uprv_strcmp(locale, currCache[i]->locale) == 0) {
                found = i;
                ಭಙ್ಗ;
            }
        }
        ಯದಿ (found == -1) {
            // insert new entry to 
            // currentCacheEntryIndex % CURRENCY_NAME_CACHE_NUM
            // and remove the existing entry 
            // currentCacheEntryIndex % CURRENCY_NAME_CACHE_NUM
            // from cache.
            cacheEntry = currCache[currentCacheEntryIndex];
            ಯದಿ (cacheEntry) {
                --(cacheEntry->ref_ಎಣಿಕೆ);
                // delete if the ref count is zero
                ಯದಿ (cacheEntry->ref_ಎಣಿಕೆ == 0) {
                    deleteCacheEntry(cacheEntry);
                }
            }
            cacheEntry = static_cast<CurrencyNameCacheEntry*>(uprv_malloc(ಪರಿಮಾಣ(CurrencyNameCacheEntry)));
            ಯದಿ (cacheEntry == nullptr) {
                deleteCurrencyNames(currencyNames, total_currency_name_ಎಣಿಕೆ);
                deleteCurrencyNames(currencySymbols, total_currency_ಚಿಹ್ನೆ_ಎಣಿಕೆ);
                ec = U_MEMORY_ALLOCATION_ERROR;
                ಪ್ರತ್ಯಾಗಮ nullptr;
            }
            currCache[currentCacheEntryIndex] = cacheEntry;
            uprv_strcpy(cacheEntry->locale, locale);
            cacheEntry->currencyNames = currencyNames;
            cacheEntry->total_Currency_Name_ಎಣಿಕೆ = total_currency_name_ಎಣಿಕೆ;
            cacheEntry->currencySymbols = currencySymbols;
            cacheEntry->total_Currency_ಚಿಹ್ನೆ_ಎಣಿಕೆ = total_currency_ಚಿಹ್ನೆ_ಎಣಿಕೆ;
            cacheEntry->ref_ಎಣಿಕೆ = 2; // one for cache, one for reference
            currentCacheEntryIndex = (currentCacheEntryIndex + 1) % CURRENCY_NAME_CACHE_ಅಂಕೆ;
            ucln_common_registerCleanup(UCLN_COMMON_CURRENCY, currency_cleanup);
        } ಅನ್ಯಥಾ {
            deleteCurrencyNames(currencyNames, total_currency_name_ಎಣಿಕೆ);
            deleteCurrencyNames(currencySymbols, total_currency_ಚಿಹ್ನೆ_ಎಣಿಕೆ);
            cacheEntry = currCache[found];
            ++(cacheEntry->ref_ಎಣಿಕೆ);
        }
        umtx_unlock(&gCurrencyCacheMutex);
    }

    ಪ್ರತ್ಯಾಗಮ cacheEntry;
}

ಸ್ಥಿರ ಶೂನ್ಯ releaseCacheEntry(CurrencyNameCacheEntry* cacheEntry) {
    umtx_lock(&gCurrencyCacheMutex);
    --(cacheEntry->ref_ಎಣಿಕೆ);
    ಯದಿ (cacheEntry->ref_ಎಣಿಕೆ == 0) {  // remove
        deleteCacheEntry(cacheEntry);
    }
    umtx_unlock(&gCurrencyCacheMutex);
}

U_CAPI ಶೂನ್ಯ
uprv_parseCurrency(ಧ್ರುವ ವರ್ಣ* locale,
                   ಧ್ರುವ icu::UnicodeString& text,
                   icu::ParsePosition& pos,
                   int8_t type,
                   int32_t* partialMatchLen,
                   char16_t* result,
                   UErrorCode& ec) {
    U_NAMESPACE_USE
    ಯದಿ (U_FAILURE(ec)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    CurrencyNameCacheEntry* cacheEntry = getCacheEntry(locale, ec);
    ಯದಿ (U_FAILURE(ec)) {
        ಪ್ರತ್ಯಾಗಮ;
    }

    int32_t total_currency_name_ಎಣಿಕೆ = cacheEntry->total_Currency_Name_ಎಣಿಕೆ;
    CurrencyNameStruct* currencyNames = cacheEntry->currencyNames;
    int32_t total_currency_ಚಿಹ್ನೆ_ಎಣಿಕೆ = cacheEntry->total_Currency_ಚಿಹ್ನೆ_ಎಣಿಕೆ;
    CurrencyNameStruct* currencySymbols = cacheEntry->currencySymbols;

    int32_t start = pos.getIndex();

    char16_t ಒಳಹರಿವು_Text[MAX_CURRENCY_NAME_LEN];
    char16_t upperText[MAX_CURRENCY_NAME_LEN];
    int32_t textLen = MIN(MAX_CURRENCY_NAME_LEN, text.length() - start);
    text.extract(start, textLen, ಒಳಹರಿವು_Text);
    UErrorCode ec1 = U_ZERO_ERROR;
    textLen = u_strToUpper(upperText, MAX_CURRENCY_NAME_LEN, ಒಳಹರಿವು_Text, textLen, locale, &ec1);

    // Make sure partialMatchLen is initialized
    *partialMatchLen = 0;

    int32_t max = 0;
    int32_t matchIndex = -1;
    // case in-sensitive comparison against currency names
    ಹುಡುಕು_Currency_Name(currencyNames, total_currency_name_ಎಣಿಕೆ, 
                       upperText, textLen, partialMatchLen, &max, &matchIndex);

#ifdef UCURR_DEBUG
    printf("search in names, max = %d, matchIndex = %d\n", max, matchIndex);
#endif

    int32_t max_In_ಚಿಹ್ನೆ = 0;
    int32_t match_Index_In_ಚಿಹ್ನೆ = -1;
    ಯದಿ (type != UCURR_LONG_NAME) {  // not name only
        // case sensitive comparison against currency symbols and ISO code.
        ಹುಡುಕು_Currency_Name(currencySymbols, total_currency_ಚಿಹ್ನೆ_ಎಣಿಕೆ, 
                           ಒಳಹರಿವು_Text, textLen,
                           partialMatchLen,
                           &max_In_ಚಿಹ್ನೆ, &match_Index_In_ಚಿಹ್ನೆ);
    }

#ifdef UCURR_DEBUG
    printf("search in symbols, maxInSymbol = %d, matchIndexInSymbol = %d\n", max_In_ಚಿಹ್ನೆ, match_Index_In_ಚಿಹ್ನೆ);
    ಯದಿ(match_Index_In_ಚಿಹ್ನೆ != -1) {
      printf("== ISO=%s\n", currencySymbols[match_Index_In_ಚಿಹ್ನೆ].IsoCode);
    }
#endif

    ಯದಿ (max >= max_In_ಚಿಹ್ನೆ && matchIndex != -1) {
        u_charsToUChars(currencyNames[matchIndex].IsoCode, result, 4);
        pos.setIndex(start + max);
    } ಅನ್ಯಥಾ ಯದಿ (max_In_ಚಿಹ್ನೆ >= max && match_Index_In_ಚಿಹ್ನೆ != -1) {
        u_charsToUChars(currencySymbols[match_Index_In_ಚಿಹ್ನೆ].IsoCode, result, 4);
        pos.setIndex(start + max_In_ಚಿಹ್ನೆ);
    }

    // decrease reference count
    releaseCacheEntry(cacheEntry);
}

ಶೂನ್ಯ uprv_currencyLeads(ಧ್ರುವ ವರ್ಣ* locale, icu::UnicodeSet& result, UErrorCode& ec) {
    U_NAMESPACE_USE
    ಯದಿ (U_FAILURE(ec)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    CurrencyNameCacheEntry* cacheEntry = getCacheEntry(locale, ec);
    ಯದಿ (U_FAILURE(ec)) {
        ಪ್ರತ್ಯಾಗಮ;
    }

    ಪ್ರತಿ (int32_t i=0; i<cacheEntry->total_Currency_ಚಿಹ್ನೆ_ಎಣಿಕೆ; i++) {
        ಧ್ರುವ CurrencyNameStruct& info = cacheEntry->currencySymbols[i];
        UChar32 cp;
        U16_GET(info.currencyName, 0, 0, info.currencyNameLen, cp);
        result.add(cp);
    }

    ಪ್ರತಿ (int32_t i=0; i<cacheEntry->total_Currency_Name_ಎಣಿಕೆ; i++) {
        ಧ್ರುವ CurrencyNameStruct& info = cacheEntry->currencyNames[i];
        UChar32 cp;
        U16_GET(info.currencyName, 0, 0, info.currencyNameLen, cp);
        result.add(cp);
    }

    // decrease reference count
    releaseCacheEntry(cacheEntry);
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
uprv_getStaticCurrencyName(ಧ್ರುವ char16_t* iso, ಧ್ರುವ ವರ್ಣ* loc,
                           icu::UnicodeString& result, UErrorCode& ec)
{
    U_NAMESPACE_USE

    int32_t len;
    ಧ್ರುವ char16_t* currname = ucurr_getName(iso, loc, UCURR_ಚಿಹ್ನೆ_NAME,
                                          nullptr /* isChoiceFormat */, &len, &ec);
    ಯದಿ (U_SUCCESS(ec)) {
        result.setTo(currname, len);
    }
}

U_CAPI int32_t U_EXPORT2
ucurr_getDefaultFractionDigits(ಧ್ರುವ char16_t* currency, UErrorCode* ec) {
    ಪ್ರತ್ಯಾಗಮ ucurr_getDefaultFractionDigitsForUsage(currency,UCURR_USAGE_STANDARD,ec);
}

U_CAPI int32_t U_EXPORT2
ucurr_getDefaultFractionDigitsForUsage(ಧ್ರುವ char16_t* currency, ಧ್ರುವ UCurrencyUsage usage, UErrorCode* ec) {
    int32_t fracDigits = 0;
    ಯದಿ (U_SUCCESS(*ec)) {
        ವಿಕಲ್ಪ (usage) {
            ಪಕ್ಷ UCURR_USAGE_STANDARD:
                fracDigits = (_findMetaData(currency, *ec))[0];
                ಭಙ್ಗ;
            ಪಕ್ಷ UCURR_USAGE_CASH:
                fracDigits = (_findMetaData(currency, *ec))[2];
                ಭಙ್ಗ;
            ಸ್ವಭಾವ:
                *ec = U_UNSUPPORTED_ERROR;
        }
    }
    ಪ್ರತ್ಯಾಗಮ fracDigits;
}

U_CAPI ದ್ವಿಗುಣ U_EXPORT2
ucurr_getRoundingIncrement(ಧ್ರುವ char16_t* currency, UErrorCode* ec) {
    ಪ್ರತ್ಯಾಗಮ ucurr_getRoundingIncrementForUsage(currency, UCURR_USAGE_STANDARD, ec);
}

U_CAPI ದ್ವಿಗುಣ U_EXPORT2
ucurr_getRoundingIncrementForUsage(ಧ್ರುವ char16_t* currency, ಧ್ರುವ UCurrencyUsage usage, UErrorCode* ec) {
    ದ್ವಿಗುಣ result = 0.0;

    ಧ್ರುವ int32_t *data = _findMetaData(currency, *ec);
    ಯದಿ (U_SUCCESS(*ec)) {
        int32_t fracDigits;
        int32_t increment;
        ವಿಕಲ್ಪ (usage) {
            ಪಕ್ಷ UCURR_USAGE_STANDARD:
                fracDigits = data[0];
                increment = data[1];
                ಭಙ್ಗ;
            ಪಕ್ಷ UCURR_USAGE_CASH:
                fracDigits = data[2];
                increment = data[3];
                ಭಙ್ಗ;
            ಸ್ವಭಾವ:
                *ec = U_UNSUPPORTED_ERROR;
                ಪ್ರತ್ಯಾಗಮ result;
        }

        // If the meta data is invalid, return 0.0
        ಯದಿ (fracDigits < 0 || fracDigits > MAX_POW10) {
            *ec = U_INVALID_FORMAT_ERROR;
        } ಅನ್ಯಥಾ {
            // A rounding value of 0 or 1 indicates no rounding.
            ಯದಿ (increment >= 2) {
                // Return (increment) / 10^(fracDigits).  The only actual rounding data,
                // as of this writing, is CHF { 2, 5 }.
                result = ದ್ವಿಗುಣ(increment) / POW10[fracDigits];
            }
        }
    }

    ಪ್ರತ್ಯಾಗಮ result;
}

U_CDECL_BEGIN

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ UCurrencyContext {
    uint32_t currType; /* UCurrCurrencyType */
    uint32_t listIdx;
} UCurrencyContext;

/*
Please keep this list in alphabetical order.
You can look at the CLDR supplemental data or ISO-4217 for the meaning of some
of these items.
ISO-4217: http://www.iso.org/iso/en/prods-services/popstds/currencycodeslist.html
*/
ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ CurrencyList {
    ಧ್ರುವ ವರ್ಣ *currency;
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
    { nullptr, 0 } // Leave here to denote the end of the list.
};

#define UCURR_MATCHES_BITMASK(variable, typeToMatch) \
    ((typeToMatch) == UCURR_ALL || ((variable) & (typeToMatch)) == (typeToMatch))

ಸ್ಥಿರ int32_t U_CALLCONV
ucurr_ಎಣಿಕೆ_Currency_List(UEnumeration *enumerator, UErrorCode * /*pErrorCode*/) {
    UCurrencyContext *myContext = (UCurrencyContext *)(enumerator->context);
    uint32_t currType = myContext->currType;
    int32_t count = 0;

    /* Count the number of items matching the type we are looking for. */
    ಪ್ರತಿ (int32_t idx = 0; gCurrencyList[idx].currency != nullptr; idx++) {
        ಯದಿ (UCURR_MATCHES_BITMASK(gCurrencyList[idx].currType, currType)) {
            count++;
        }
    }
    ಪ್ರತ್ಯಾಗಮ count;
}

ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ* U_CALLCONV
ucurr_nextCurrencyList(UEnumeration *enumerator,
                        int32_t* resultLength,
                        UErrorCode * /*pErrorCode*/)
{
    UCurrencyContext *myContext = (UCurrencyContext *)(enumerator->context);

    /* Find the next in the list that matches the type we are looking for. */
    ಯಾವತ್ (myContext->listIdx < UPRV_LENGTHOF(gCurrencyList)-1) {
        ಧ್ರುವ ಸಂರಚನಾ CurrencyList *currItem = &gCurrencyList[myContext->listIdx++];
        ಯದಿ (UCURR_MATCHES_BITMASK(currItem->currType, myContext->currType))
        {
            ಯದಿ (resultLength) {
                *resultLength = 3; /* Currency codes are only 3 chars long */
            }
            ಪ್ರತ್ಯಾಗಮ currItem->currency;
        }
    }
    /* We enumerated too far. */
    ಯದಿ (resultLength) {
        *resultLength = 0;
    }
    ಪ್ರತ್ಯಾಗಮ nullptr;
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
ucurr_resetCurrencyList(UEnumeration *enumerator, UErrorCode * /*pErrorCode*/) {
    ((UCurrencyContext *)(enumerator->context))->listIdx = 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
ucurr_closeCurrencyList(UEnumeration *enumerator) {
    uprv_free(enumerator->context);
    uprv_free(enumerator);
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
ucurr_createCurrencyList(UHashtable *isoCodes, UErrorCode* status){
    UErrorCode localStatus = U_ZERO_ERROR;

    // Look up the CurrencyMap element in the root bundle.
    UResourceBundle *rb = ures_openDirect(U_ICUDATA_CURR, CURRENCY_DATA, &localStatus);
    LocalUResourceBundlePointer currencyMapArray(ures_get_By_ಕೀಲಿ(rb, CURRENCY_MAP, rb, &localStatus));

    ಯದಿ (U_SUCCESS(localStatus)) {
        // process each entry in currency map
        ಪ್ರತಿ (int32_t i=0; i<ures_getSize(currencyMapArray.getAlias()); i++) {
            // get the currency resource
            LocalUResourceBundlePointer currencyArray(ures_getByIndex(currencyMapArray.getAlias(), i, nullptr, &localStatus));
            // process each currency
            ಯದಿ (U_SUCCESS(localStatus)) {
                ಪ್ರತಿ (int32_t j=0; j<ures_getSize(currencyArray.getAlias()); j++) {
                    // get the currency resource
                    LocalUResourceBundlePointer currencyRes(ures_getByIndex(currencyArray.getAlias(), j, nullptr, &localStatus));
                    IsoCodeEntry *entry = (IsoCodeEntry*)uprv_malloc(ಪರಿಮಾಣ(IsoCodeEntry));
                    ಯದಿ (entry == nullptr) {
                        *status = U_MEMORY_ALLOCATION_ERROR;
                        ಪ್ರತ್ಯಾಗಮ;
                    }

                    // get the ISO code
                    int32_t isoLength = 0;
                    LocalUResourceBundlePointer idRes(ures_get_By_ಕೀಲಿ(currencyRes.getAlias(), "id", nullptr, &localStatus));
                    ಯದಿ (idRes.isNull()) {
                        ಅನುವರ್ತ;
                    }
                    ಧ್ರುವ char16_t *isoCode = ures_getString(idRes.getAlias(), &isoLength, &localStatus);

                    // get from date
                    UDate fromDate = U_DATE_MIN;
                    LocalUResourceBundlePointer fromRes(ures_get_By_ಕೀಲಿ(currencyRes.getAlias(), "from", nullptr, &localStatus));

                    ಯದಿ (U_SUCCESS(localStatus)) {
                        int32_t fromLength = 0;
                        ಧ್ರುವ int32_t *fromArray = ures_getIntVector(fromRes.getAlias(), &fromLength, &localStatus);
                        int64_t currDate64 = ((uint64_t)fromArray[0]) << 32;
                        currDate64 |= ((int64_t)fromArray[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                        fromDate = (UDate)currDate64;
                    }

                    // get to date
                    UDate toDate = U_DATE_MAX;
                    localStatus = U_ZERO_ERROR;
                    LocalUResourceBundlePointer toRes(ures_get_By_ಕೀಲಿ(currencyRes.getAlias(), "to", nullptr, &localStatus));

                    ಯದಿ (U_SUCCESS(localStatus)) {
                        int32_t toLength = 0;
                        ಧ್ರುವ int32_t *toArray = ures_getIntVector(toRes.getAlias(), &toLength, &localStatus);
                        int64_t currDate64 = (uint64_t)toArray[0] << 32;
                        currDate64 |= ((int64_t)toArray[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                        toDate = (UDate)currDate64;
                    }

                    entry->isoCode = isoCode;
                    entry->from = fromDate;
                    entry->to = toDate;

                    localStatus = U_ZERO_ERROR;
                    uhash_put(isoCodes, (char16_t *)isoCode, entry, &localStatus);
                }
            } ಅನ್ಯಥಾ {
                *status = localStatus;
            }
        }
    } ಅನ್ಯಥಾ {
        *status = localStatus;
    }
}

ಸ್ಥಿರ ಧ್ರುವ UEnumeration gEnumCurrencyList = {
    nullptr,
    nullptr,
    ucurr_closeCurrencyList,
    ucurr_ಎಣಿಕೆ_Currency_List,
    uenum_unextDefault,
    ucurr_nextCurrencyList,
    ucurr_resetCurrencyList
};
U_CDECL_END


ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV initIsoCodes(UErrorCode &status) {
    U_ASSERT(gIsoCodes == nullptr);
    ucln_common_registerCleanup(UCLN_COMMON_CURRENCY, currency_cleanup);

    LocalUHashtablePointer isoCodes(uhash_open(uhash_hashUChars, uhash_compareUChars, nullptr, &status));
    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    uhash_set_ಮೌಲ್ಯ_Deleter(isoCodes.getAlias(), deleteIsoCodeEntry);

    ucurr_createCurrencyList(isoCodes.getAlias(), &status);
    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    gIsoCodes = isoCodes.orphan();  // Note: gIsoCodes is const. Once set up here it is never altered,
                                    //       and read only access is safe without synchronization.
}

ಸ್ಥಿರ ಶೂನ್ಯ populateCurrSymbolsEquiv(icu::Hashtable *hash, UErrorCode &status) {
    ಯದಿ (U_FAILURE(status)) { ಪ್ರತ್ಯಾಗಮ; }
    ಪ್ರತಿ (ಧ್ರುವ auto& entry : unisets::kCurrencyEntries) {
        UnicodeString exemplar(entry.exemplar);
        ಧ್ರುವ UnicodeSet* set = unisets::get(entry.ಕೀಲಿ);
        ಯದಿ (set == nullptr) { ಪ್ರತ್ಯಾಗಮ; }
        UnicodeSetIterator it(*set);
        ಯಾವತ್ (it.next()) {
            UnicodeString value = it.getString();
            ಯದಿ (value == exemplar) {
                // No need to mark the exemplar character as an equivalent
                ಅನುವರ್ತ;
            }
            makeEquivalent(exemplar, value, hash, status);
            ಯದಿ (U_FAILURE(status)) { ಪ್ರತ್ಯಾಗಮ; }
        }
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV initCurrSymbolsEquiv() {
    U_ASSERT(gCurrSymbolsEquiv == nullptr);
    UErrorCode status = U_ZERO_ERROR;
    ucln_common_registerCleanup(UCLN_COMMON_CURRENCY, currency_cleanup);
    icu::Hashtable *temp = ನೂತನ icu::Hashtable(status);
    ಯದಿ (temp == nullptr) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (U_FAILURE(status)) {
        delete temp;
        ಪ್ರತ್ಯಾಗಮ;
    }
    temp->set_ಮೌಲ್ಯ_Deleter(deleteUnicode);
    populateCurrSymbolsEquiv(temp, status);
    ಯದಿ (U_FAILURE(status)) {
        delete temp;
        ಪ್ರತ್ಯಾಗಮ;
    }
    gCurrSymbolsEquiv = temp;
}

U_CAPI UBool U_EXPORT2
ucurr_isAvailable(ಧ್ರುವ char16_t* isoCode, UDate from, UDate to, UErrorCode* eErrorCode) {
    umtx_initOnce(gIsoCodesInitOnce, &initIsoCodes, *eErrorCode);
    ಯದಿ (U_FAILURE(*eErrorCode)) {
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }

    IsoCodeEntry* result = (IsoCodeEntry *) uhash_get(gIsoCodes, isoCode);
    ಯದಿ (result == nullptr) {
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    } ಅನ್ಯಥಾ ಯದಿ (from > to) {
        *eErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    } ಅನ್ಯಥಾ ಯದಿ  ((from > result->to) || (to < result->from)) {
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

ಸ್ಥಿರ ಧ್ರುವ icu::Hashtable* getCurrSymbolsEquiv() {
    umtx_initOnce(gCurrSymbolsEquivInitOnce, &initCurrSymbolsEquiv);
    ಪ್ರತ್ಯಾಗಮ gCurrSymbolsEquiv;
}

U_CAPI UEnumeration * U_EXPORT2
ucurr_openISOCurrencies(uint32_t currType, UErrorCode *pErrorCode) {
    UEnumeration *myEnum = nullptr;
    UCurrencyContext *myContext;

    myEnum = (UEnumeration*)uprv_malloc(ಪರಿಮಾಣ(UEnumeration));
    ಯದಿ (myEnum == nullptr) {
        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    uprv_memcpy(myEnum, &gEnumCurrencyList, ಪರಿಮಾಣ(UEnumeration));
    myContext = (UCurrencyContext*)uprv_malloc(ಪರಿಮಾಣ(UCurrencyContext));
    ಯದಿ (myContext == nullptr) {
        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
        uprv_free(myEnum);
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    myContext->currType = currType;
    myContext->listIdx = 0;
    myEnum->context = myContext;
    ಪ್ರತ್ಯಾಗಮ myEnum;
}

U_CAPI int32_t U_EXPORT2
ucurr_ಎಣಿಕೆ_Currencies(ಧ್ರುವ ವರ್ಣ* locale, 
                 UDate date, 
                 UErrorCode* ec)
{
    int32_t curr_ಎಣಿಕೆ = 0;

    ಯದಿ (ec != nullptr && U_SUCCESS(*ec)) 
    {
        // local variables
        UErrorCode localStatus = U_ZERO_ERROR;

        // get country or country_variant in `id'
        CharString id = idForLocale(locale, ec);

        ಯದಿ (U_FAILURE(*ec))
        {
            ಪ್ರತ್ಯಾಗಮ 0;
        }

        // Remove variants, which is only needed for registration.
        ವರ್ಣ *idDelim = strchr(id.data(), VAR_DELIM);
        ಯದಿ (idDelim)
        {
            id.truncate(idDelim - id.data());
        }

        // Look up the CurrencyMap element in the root bundle.
        UResourceBundle *rb = ures_openDirect(U_ICUDATA_CURR, CURRENCY_DATA, &localStatus);
        UResourceBundle *cm = ures_get_By_ಕೀಲಿ(rb, CURRENCY_MAP, rb, &localStatus);

        // Using the id derived from the local, get the currency data
        LocalUResourceBundlePointer countryArray(ures_get_By_ಕೀಲಿ(rb, id.data(), cm, &localStatus));

        // process each currency to see which one is valid for the given date
        ಯದಿ (U_SUCCESS(localStatus))
        {
            ಪ್ರತಿ (int32_t i=0; i<ures_getSize(countryArray.getAlias()); i++)
            {
                // get the currency resource
                LocalUResourceBundlePointer currencyRes(ures_getByIndex(countryArray.getAlias(), i, nullptr, &localStatus));

                // get the from date
                int32_t fromLength = 0;
                LocalUResourceBundlePointer fromRes(ures_get_By_ಕೀಲಿ(currencyRes.getAlias(), "from", nullptr, &localStatus));
                ಧ್ರುವ int32_t *fromArray = ures_getIntVector(fromRes.getAlias(), &fromLength, &localStatus);

                int64_t currDate64 = (int64_t)((uint64_t)(fromArray[0]) << 32);
                currDate64 |= ((int64_t)fromArray[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                UDate fromDate = (UDate)currDate64;

                ಯದಿ (ures_getSize(currencyRes.getAlias())> 2)
                {
                    int32_t toLength = 0;
                    LocalUResourceBundlePointer toRes(ures_get_By_ಕೀಲಿ(currencyRes.getAlias(), "to", nullptr, &localStatus));
                    ಧ್ರುವ int32_t *toArray = ures_getIntVector(toRes.getAlias(), &toLength, &localStatus);

                    currDate64 = (int64_t)toArray[0] << 32;
                    currDate64 |= ((int64_t)toArray[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                    UDate toDate = (UDate)currDate64;

                    ಯದಿ ((fromDate <= date) && (date < toDate))
                    {
                        curr_ಎಣಿಕೆ++;
                    }
                }
                ಅನ್ಯಥಾ
                {
                    ಯದಿ (fromDate <= date)
                    {
                        curr_ಎಣಿಕೆ++;
                    }
                }
            } // end For loop
        } // end if (U_SUCCESS(localStatus))

        // Check for errors
        ಯದಿ (*ec == U_ZERO_ERROR || localStatus != U_ZERO_ERROR)
        {
            // There is nothing to fallback to. 
            // Report the failure/warning if possible.
            *ec = localStatus;
        }

        ಯದಿ (U_SUCCESS(*ec))
        {
            // no errors
            ಪ್ರತ್ಯಾಗಮ curr_ಎಣಿಕೆ;
        }
    }

    // If we got here, either error code is invalid or
    // some argument passed is no good.
    ಪ್ರತ್ಯಾಗಮ 0;
}

U_CAPI int32_t U_EXPORT2 
ucurr_forLocaleAndDate(ಧ್ರುವ ವರ್ಣ* locale, 
                UDate date, 
                int32_t index,
                char16_t* buff,
                int32_t buffCapacity, 
                UErrorCode* ec)
{
    int32_t resLen = 0;
	int32_t currIndex = 0;
    ಧ್ರುವ char16_t* s = nullptr;

    ಯದಿ (ec != nullptr && U_SUCCESS(*ec))
    {
        // check the arguments passed
        ಯದಿ ((buff && buffCapacity) || !buffCapacity )
        {
            // local variables
            UErrorCode localStatus = U_ZERO_ERROR;

            // get country or country_variant in `id'
            CharString id = idForLocale(locale, ec);
            ಯದಿ (U_FAILURE(*ec))
            {
                ಪ್ರತ್ಯಾಗಮ 0;
            }

            // Remove variants, which is only needed for registration.
            ವರ್ಣ *idDelim = strchr(id.data(), VAR_DELIM);
            ಯದಿ (idDelim)
            {
                id.truncate(idDelim - id.data());
            }

            // Look up the CurrencyMap element in the root bundle.
            UResourceBundle *rb = ures_openDirect(U_ICUDATA_CURR, CURRENCY_DATA, &localStatus);
            UResourceBundle *cm = ures_get_By_ಕೀಲಿ(rb, CURRENCY_MAP, rb, &localStatus);

            // Using the id derived from the local, get the currency data
            LocalUResourceBundlePointer countryArray(ures_get_By_ಕೀಲಿ(rb, id.data(), cm, &localStatus));

            // process each currency to see which one is valid for the given date
            ತಾರ್ಕಿಕ matchFound = ಅಸತ್ಯ;
            ಯದಿ (U_SUCCESS(localStatus))
            {
                ಯದಿ ((index <= 0) || (index> ures_getSize(countryArray.getAlias())))
                {
                    // requested index is out of bounds
                    ಪ್ರತ್ಯಾಗಮ 0;
                }

                ಪ್ರತಿ (int32_t i=0; i<ures_getSize(countryArray.getAlias()); i++)
                {
                    // get the currency resource
                    LocalUResourceBundlePointer currencyRes(ures_getByIndex(countryArray.getAlias(), i, nullptr, &localStatus));
                    s = ures_get_String_By_ಕೀಲಿ(currencyRes.getAlias(), "id", &resLen, &localStatus);

                    // get the from date
                    int32_t fromLength = 0;
                    LocalUResourceBundlePointer fromRes(ures_get_By_ಕೀಲಿ(currencyRes.getAlias(), "from", nullptr, &localStatus));
                    ಧ್ರುವ int32_t *fromArray = ures_getIntVector(fromRes.getAlias(), &fromLength, &localStatus);

                    int64_t currDate64 = (int64_t)((uint64_t)fromArray[0] << 32);
                    currDate64 |= ((int64_t)fromArray[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                    UDate fromDate = (UDate)currDate64;

                    ಯದಿ (ures_getSize(currencyRes.getAlias()) > 2)
                    {
                        int32_t toLength = 0;
                        LocalUResourceBundlePointer toRes(ures_get_By_ಕೀಲಿ(currencyRes.getAlias(), "to", nullptr, &localStatus));
                        ಧ್ರುವ int32_t *toArray = ures_getIntVector(toRes.getAlias(), &toLength, &localStatus);

                        currDate64 = (int64_t)toArray[0] << 32;
                        currDate64 |= ((int64_t)toArray[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                        UDate toDate = (UDate)currDate64;

                        ಯದಿ ((fromDate <= date) && (date < toDate))
                        {
                            currIndex++;
                            ಯದಿ (currIndex == index)
                            {
                                matchFound = ಸತ್ಯ;
                            }
                        }
                    }
                    ಅನ್ಯಥಾ
                    {
                        ಯದಿ (fromDate <= date)
                        {
                            currIndex++;
                            ಯದಿ (currIndex == index)
                            {
                                matchFound = ಸತ್ಯ;
                            }
                        }
                    }
                    // check for loop exit
                    ಯದಿ (matchFound)
                    {
                        ಭಙ್ಗ;
                    }

                } // end For loop
            }

            // Check for errors
            ಯದಿ (*ec == U_ZERO_ERROR || localStatus != U_ZERO_ERROR)
            {
                // There is nothing to fallback to. 
                // Report the failure/warning if possible.
                *ec = localStatus;
            }

            ಯದಿ (U_SUCCESS(*ec))
            {
                // no errors
                ಯದಿ((buffCapacity> resLen) && matchFound)
                {
                    // write out the currency value
                    u_strcpy(buff, s);
                }
                ಅನ್ಯಥಾ
                {
                    ಪ್ರತ್ಯಾಗಮ 0;
                }
            }

            // return null terminated currency string
            ಪ್ರತ್ಯಾಗಮ u_terminateUChars(buff, buffCapacity, resLen, ec);
        }
        ಅನ್ಯಥಾ
        {
            // illegal argument encountered
            *ec = U_ILLEGAL_ARGUMENT_ERROR;
        }

    }

    // If we got here, either error code is invalid or
    // some argument passed is no good.
    ಪ್ರತ್ಯಾಗಮ resLen;
}

ಸ್ಥಿರ ಧ್ರುವ UEnumeration default_Keyword_ಮೌಲ್ಯಗಳು = {
    nullptr,
    nullptr,
    ulist_close_keyword_ಮೌಲ್ಯಗಳು_iterator,
    ulist_ಎಣಿಕೆ_keyword_ಮೌಲ್ಯಗಳು,
    uenum_unextDefault,
    ulist_next_keyword_ಮೌಲ್ಯ, 
    ulist_reset_keyword_ಮೌಲ್ಯಗಳು_iterator
};

U_CAPI UEnumeration *U_EXPORT2 ucurr_get_Keyword_ಮೌಲ್ಯಗಳು_For_Locale(ಧ್ರುವ ವರ್ಣ *ಕೀಲಿ, ಧ್ರುವ ವರ್ಣ *locale, UBool commonlyUsed, UErrorCode* status) {
    // Resolve region
    CharString prefRegion = ulocimp_getRegionForSupplementalData(locale, ಸತ್ಯ, *status);

    // Read value from supplementalData
    UList *ಮೌಲ್ಯಗಳು = ulist_createEmptyList(status);
    UList *other_ಮೌಲ್ಯಗಳು = ulist_createEmptyList(status);
    UEnumeration *en = (UEnumeration *)uprv_malloc(ಪರಿಮಾಣ(UEnumeration));
    ಯದಿ (U_FAILURE(*status) || en == nullptr) {
        ಯದಿ (en == nullptr) {
            *status = U_MEMORY_ALLOCATION_ERROR;
        } ಅನ್ಯಥಾ {
            uprv_free(en);
        }
        ulist_deleteList(ಮೌಲ್ಯಗಳು);
        ulist_deleteList(other_ಮೌಲ್ಯಗಳು);
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    memcpy(en, &default_Keyword_ಮೌಲ್ಯಗಳು, ಪರಿಮಾಣ(UEnumeration));
    en->context = ಮೌಲ್ಯಗಳು;
    
    UResourceBundle* rb = ures_openDirect(U_ICUDATA_CURR, "supplementalData", status);
    LocalUResourceBundlePointer bundle(ures_get_By_ಕೀಲಿ(rb, "CurrencyMap", rb, status));
    ರಾಶಿ_UResource_Bundle bundlekey, regbndl, curbndl, to;
    
    ಯಾವತ್ (U_SUCCESS(*status) && ures_hasNext(bundle.getAlias())) {
        ures_getNextResource(bundle.getAlias(), bundlekey.getAlias(), status);
        ಯದಿ (U_FAILURE(*status)) {
            ಭಙ್ಗ;
        }
        ಧ್ರುವ ವರ್ಣ *region = ures_get_ಕೀಲಿ(bundlekey.getAlias());
        UBool isPrefRegion = prefRegion == region;
        ಯದಿ (!isPrefRegion && commonlyUsed) {
            // With commonlyUsed=true, we do not put
            // currencies for other regions in the
            // result list.
            ಅನುವರ್ತ;
        }
        ures_get_By_ಕೀಲಿ(bundle.getAlias(), region, regbndl.getAlias(), status);
        ಯದಿ (U_FAILURE(*status)) {
            ಭಙ್ಗ;
        }
        ಯಾವತ್ (U_SUCCESS(*status) && ures_hasNext(regbndl.getAlias())) {
            ures_getNextResource(regbndl.getAlias(), curbndl.getAlias(), status);
            ಯದಿ (ures_getType(curbndl.getAlias()) != URES_TABLE) {
                // Currently, an empty ARRAY is mixed in.
                ಅನುವರ್ತ;
            }
            ವರ್ಣ *curID = (ವರ್ಣ *)uprv_malloc(ಪರಿಮಾಣ(ವರ್ಣ) * ULOC_KEYWORDS_CAPACITY);
            ಯದಿ (curID == nullptr) {
                *status = U_MEMORY_ALLOCATION_ERROR;
                ಭಙ್ಗ;
            }
            int32_t curIDLength = ULOC_KEYWORDS_CAPACITY;

#ಯದಿ U_CHARSET_FAMILY==U_ASCII_FAMILY
            ures_get_UTF8_String_By_ಕೀಲಿ(curbndl.getAlias(), "id", curID, &curIDLength, ಸತ್ಯ, status);
            /* optimize - use the utf-8 string */
#ಅನ್ಯಥಾ
            {
                       ಧ್ರುವ char16_t* defString = ures_get_String_By_ಕೀಲಿ(curbndl.getAlias(), "id", &curIDLength, status);
                       ಯದಿ(U_SUCCESS(*status)) {
			   ಯದಿ(curIDLength+1 > ULOC_KEYWORDS_CAPACITY) {
				*status = U_BUFFER_OVERFLOW_ERROR;
			   } ಅನ್ಯಥಾ {
                           	u_UCharsToChars(defString, curID, curIDLength+1);
			   }
                       }
            }
#endif	

            ಯದಿ (U_FAILURE(*status)) {
                ಭಙ್ಗ;
            }
            UBool hasTo = ಅಸತ್ಯ;
            ures_get_By_ಕೀಲಿ(curbndl.getAlias(), "to", to.getAlias(), status);
            ಯದಿ (U_FAILURE(*status)) {
                // Do nothing here...
                *status = U_ZERO_ERROR;
            } ಅನ್ಯಥಾ {
                hasTo = ಸತ್ಯ;
            }
            ಯದಿ (isPrefRegion && !hasTo && !ulist_containsString(ಮೌಲ್ಯಗಳು, curID, (int32_t)uprv_strlen(curID))) {
                // Currently active currency for the target country
                ulist_addItemEndList(ಮೌಲ್ಯಗಳು, curID, ಸತ್ಯ, status);
            } ಅನ್ಯಥಾ ಯದಿ (!ulist_containsString(other_ಮೌಲ್ಯಗಳು, curID, (int32_t)uprv_strlen(curID)) && !commonlyUsed) {
                ulist_addItemEndList(other_ಮೌಲ್ಯಗಳು, curID, ಸತ್ಯ, status);
            } ಅನ್ಯಥಾ {
                uprv_free(curID);
            }
        }
        
    }
    ಯದಿ (U_SUCCESS(*status)) {
        ಯದಿ (commonlyUsed) {
            ಯದಿ (ulist_getListSize(ಮೌಲ್ಯಗಳು) == 0) {
                // This could happen if no valid region is supplied in the input
                // locale. In this case, we use the CLDR's default.
                uenum_close(en);
                en = ucurr_get_Keyword_ಮೌಲ್ಯಗಳು_For_Locale(ಕೀಲಿ, "und", ಸತ್ಯ, status);
            }
        } ಅನ್ಯಥಾ {
            // Consolidate the list
            ವರ್ಣ *value = nullptr;
            ulist_resetList(other_ಮೌಲ್ಯಗಳು);
            ಯಾವತ್ ((value = (ವರ್ಣ *)ulist_getNext(other_ಮೌಲ್ಯಗಳು)) != nullptr) {
                ಯದಿ (!ulist_containsString(ಮೌಲ್ಯಗಳು, value, (int32_t)uprv_strlen(value))) {
                    ವರ್ಣ *tmp_ಮೌಲ್ಯ = (ವರ್ಣ *)uprv_malloc(ಪರಿಮಾಣ(ವರ್ಣ) * ULOC_KEYWORDS_CAPACITY);
                    ಯದಿ (tmp_ಮೌಲ್ಯ == nullptr) {
                        *status = U_MEMORY_ALLOCATION_ERROR;
                        ಭಙ್ಗ;
                    }
                    uprv_memcpy(tmp_ಮೌಲ್ಯ, value, uprv_strlen(value) + 1);
                    ulist_addItemEndList(ಮೌಲ್ಯಗಳು, tmp_ಮೌಲ್ಯ, ಸತ್ಯ, status);
                    ಯದಿ (U_FAILURE(*status)) {
                        ಭಙ್ಗ;
                    }
                }
            }
        }
        ulist_resetList((UList *)(en->context));
    } ಅನ್ಯಥಾ {
        ulist_deleteList(ಮೌಲ್ಯಗಳು);
        uprv_free(en);
        ಮೌಲ್ಯಗಳು = nullptr;
        en = nullptr;
    }
    ulist_deleteList(other_ಮೌಲ್ಯಗಳು);
    ಪ್ರತ್ಯಾಗಮ en;
}


U_CAPI int32_t U_EXPORT2
ucurr_getNumericCode(ಧ್ರುವ char16_t* currency) {
    int32_t code = 0;
    ಯದಿ (currency && u_strlen(currency) == ISO_CURRENCY_CODE_LENGTH) {
        UErrorCode status = U_ZERO_ERROR;

        UResourceBundle* bundle = ures_openDirect(nullptr, "currencyNumericCodes", &status);
        LocalUResourceBundlePointer codeMap(ures_get_By_ಕೀಲಿ(bundle, "codeMap", bundle, &status));
        ಯದಿ (U_SUCCESS(status)) {
            ವರ್ಣ alphaCode[ISO_CURRENCY_CODE_LENGTH+1];
            myUCharsToChars(alphaCode, currency);
            T_CString_toUpperCase(alphaCode);
            ures_get_By_ಕೀಲಿ(codeMap.getAlias(), alphaCode, codeMap.getAlias(), &status);
            ಪೂರ್ಣಾಙ್ಕ tmpCode = ures_getInt(codeMap.getAlias(), &status);
            ಯದಿ (U_SUCCESS(status)) {
                code = tmpCode;
            }
        }
    }
    ಪ್ರತ್ಯಾಗಮ code;
}
#endif /* #if !UCONFIG_NO_FORMATTING */

//eof
