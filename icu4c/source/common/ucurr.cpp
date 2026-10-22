// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
* Copyright (c) 2002-2016, International Business Machines
* Corporation and others.  All Rights Reserved.
**********************************************************************
*/

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"

#ଯଦି !UCONFIG_NO_FORMATTING

#ଅନ୍ତର୍ଭାଵ <utility>

#ଅନ୍ତର୍ଭାଵ "unicode/ucurr.h"
#ଅନ୍ତର୍ଭାଵ "unicode/locid.h"
#ଅନ୍ତର୍ଭାଵ "unicode/ures.h"
#ଅନ୍ତର୍ଭାଵ "unicode/ustring.h"
#ଅନ୍ତର୍ଭାଵ "unicode/parsepos.h"
#ଅନ୍ତର୍ଭାଵ "unicode/uniset.h"
#ଅନ୍ତର୍ଭାଵ "unicode/usetiter.h"
#ଅନ୍ତର୍ଭାଵ "unicode/utf16.h"
#ଅନ୍ତର୍ଭାଵ "ustr_imp.h"
#ଅନ୍ତର୍ଭାଵ "charstr.h"
#ଅନ୍ତର୍ଭାଵ "cmemory.h"
#ଅନ୍ତର୍ଭାଵ "cstring.h"
#ଅନ୍ତର୍ଭାଵ "static_unicode_sets.h"
#ଅନ୍ତର୍ଭାଵ "uassert.h"
#ଅନ୍ତର୍ଭାଵ "umutex.h"
#ଅନ୍ତର୍ଭାଵ "ucln_cmn.h"
#ଅନ୍ତର୍ଭାଵ "uenumimp.h"
#ଅନ୍ତର୍ଭାଵ "uhash.h"
#ଅନ୍ତର୍ଭାଵ "hash.h"
#ଅନ୍ତର୍ଭାଵ "uinvchar.h"
#ଅନ୍ତର୍ଭାଵ "uresimp.h"
#ଅନ୍ତର୍ଭାଵ "ulist.h"
#ଅନ୍ତର୍ଭାଵ "uresimp.h"
#ଅନ୍ତର୍ଭାଵ "ureslocs.h"
#ଅନ୍ତର୍ଭାଵ "ulocimp.h"

using ନାମାକାଶ icu;

//#define UCURR_DEBUG_EQUIV 1
#ifdef UCURR_DEBUG_EQUIV
#ଅନ୍ତର୍ଭାଵ "stdio.h"
#endif
//#define UCURR_DEBUG 1
#ifdef UCURR_DEBUG
#ଅନ୍ତର୍ଭାଵ "stdio.h"
#endif

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା IsoCodeEntry {
    ଅଚଳ char16_t *isoCode; /* const because it's a reference to a resource bundle string. */
    UDate from;
    UDate to;
} IsoCodeEntry;

//------------------------------------------------------------
// Constants

// Default currency meta data of last resort.  We try to use the
// defaults encoded in the meta data resource bundle.  If there is a
// configuration/build error and these are not available, we use these
// hard-coded defaults (which should be identical).
ସ୍ଥିର ଅଚଳ int32_t LAST_RESORT_DATA[] = { 2, 0, 2, 0 };

// POW10[i] = 10^i, i=0..MAX_POW10
ସ୍ଥିର ଅଚଳ int32_t POW10[] = { 1, 10, 100, 1000, 10000, 100000,
                                 1000000, 10000000, 100000000, 1000000000 };

ସ୍ଥିର ଅଚଳ int32_t MAX_POW10 = UPRV_LENGTHOF(POW10) - 1;

#define ISO_CURRENCY_CODE_LENGTH 3

//------------------------------------------------------------
// Resource tags
//

ସ୍ଥିର ଅଚଳ ଅକ୍ଷର CURRENCY_DATA[] = "supplementalData";
// Tag for meta-data, in root.
ସ୍ଥିର ଅଚଳ ଅକ୍ଷର CURRENCY_META[] = "CurrencyMeta";

// Tag for map from countries to currencies, in root.
ସ୍ଥିର ଅଚଳ ଅକ୍ଷର CURRENCY_MAP[] = "CurrencyMap";

// Tag for default meta-data, in CURRENCY_META
ସ୍ଥିର ଅଚଳ ଅକ୍ଷର DEFAULT_META[] = "DEFAULT";

// Variant delimiter
ସ୍ଥିର ଅଚଳ ଅକ୍ଷର VAR_DELIM = '_';

// Tag for localized display names (symbols) of currencies
ସ୍ଥିର ଅଚଳ ଅକ୍ଷର CURRENCIES[] = "Currencies";
ସ୍ଥିର ଅଚଳ ଅକ୍ଷର CURRENCIES_NARROW[] = "Currencies%narrow";
ସ୍ଥିର ଅଚଳ ଅକ୍ଷର CURRENCIES_FORMAL[] = "Currencies%formal";
ସ୍ଥିର ଅଚଳ ଅକ୍ଷର CURRENCIES_VARIANT[] = "Currencies%variant";
ସ୍ଥିର ଅଚଳ ଅକ୍ଷର CURRENCYPLURALS[] = "CurrencyPlurals";

// ISO codes mapping table
ସ୍ଥିର ଅଚଳ UHashtable* gIsoCodes = nullptr;
ସ୍ଥିର icu::UInitOnce gIsoCodesInitOnce {};

// Currency symbol equivalances
ସ୍ଥିର ଅଚଳ icu::Hashtable* gCurrSymbolsEquiv = nullptr;
ସ୍ଥିର icu::UInitOnce gCurrSymbolsEquivInitOnce {};

U_NAMESPACE_BEGIN

// EquivIterator iterates over all strings that are equivalent to a given
// string, s. Note that EquivIterator will never yield s itself.
ଶ୍ରେଣୀ EquivIterator : ସର୍ବସାଧାରଣ icu::UMemory {
ସର୍ବସାଧାରଣ:
    // Constructor. hash stores the equivalence relationships; s is the string
    // for which we find equivalent strings.
    ଧାଡ଼ିଭିତରେ EquivIterator(ଅଚଳ icu::Hashtable& hash, ଅଚଳ icu::UnicodeString& s)
        : _hash(hash) { 
        _start = _current = &s;
    }
    ଧାଡ଼ିଭିତରେ ~EquivIterator() { }

    // next returns the next equivalent string or nullptr if there are no more.
    // If s has no equivalent strings, next returns nullptr on the first call.
    ଅଚଳ icu::UnicodeString *next();
ବ୍ୟକ୍ତିଗତ:
    ଅଚଳ icu::Hashtable& _hash;
    ଅଚଳ icu::UnicodeString* _start;
    ଅଚଳ icu::UnicodeString* _current;
};

ଅଚଳ icu::UnicodeString *
EquivIterator::next() {
    ଅଚଳ icu::UnicodeString* _next = static_cast<ଅଚଳ icu::UnicodeString*>(_hash.get(*_current));
    ଯଦି (_next == nullptr) {
        U_ASSERT(_current == _start);
        ଫେରନ୍ତୁ nullptr;
    }
    ଯଦି (*_next == *_start) {
        ଫେରନ୍ତୁ nullptr;
    }
    _current = _next;
    ଫେରନ୍ତୁ _next;
}

U_NAMESPACE_END

// makeEquivalent makes lhs and rhs equivalent by updating the equivalence
// relations in hash accordingly.
ସ୍ଥିର ଶୂନ୍ୟ makeEquivalent(
    ଅଚଳ icu::UnicodeString &lhs,
    ଅଚଳ icu::UnicodeString &rhs,
    icu::Hashtable* hash, UErrorCode &status) {
    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ;
    }
    ଯଦି (lhs == rhs) {
        // already equivalent
        ଫେରନ୍ତୁ;
    }
    icu::EquivIterator ବାମ_Iter(*hash, lhs);
    icu::EquivIterator ଡାହାଣ_Iter(*hash, rhs);
    ଅଚଳ icu::UnicodeString *first_ବାମ = ବାମ_Iter.next();
    ଅଚଳ icu::UnicodeString *first_ଡାହାଣ = ଡାହାଣ_Iter.next();
    ଅଚଳ icu::UnicodeString *next_ବାମ = first_ବାମ;
    ଅଚଳ icu::UnicodeString *next_ଡାହାଣ = first_ଡାହାଣ;
    ଯେତେବେଳେ (next_ବାମ != nullptr && next_ଡାହାଣ != nullptr) {
        ଯଦି (*next_ବାମ == rhs || *next_ଡାହାଣ == lhs) {
            // Already equivalent
            ଫେରନ୍ତୁ;
        }
        next_ବାମ = ବାମ_Iter.next();
        next_ଡାହାଣ = ଡାହାଣ_Iter.next();
    }
    // Not equivalent. Must join.
    icu::UnicodeString *new_First_ବାମ;
    icu::UnicodeString *new_First_ଡାହାଣ;
    ଯଦି (first_ଡାହାଣ == nullptr && first_ବାମ == nullptr) {
        // Neither lhs or rhs belong to an equivalence circle, so we form
        // a new equivalnce circle of just lhs and rhs.
        new_First_ବାମ = ନୂତନ icu::UnicodeString(rhs);
        new_First_ଡାହାଣ = ନୂତନ icu::UnicodeString(lhs);
    } ଅନ୍ୟଥା ଯଦି (first_ଡାହାଣ == nullptr) {
        // lhs belongs to an equivalence circle, but rhs does not, so we link
        // rhs into lhs' circle.
        new_First_ବାମ = ନୂତନ icu::UnicodeString(rhs);
        new_First_ଡାହାଣ = ନୂତନ icu::UnicodeString(*first_ବାମ);
    } ଅନ୍ୟଥା ଯଦି (first_ବାମ == nullptr) {
        // rhs belongs to an equivlance circle, but lhs does not, so we link
        // lhs into rhs' circle.
        new_First_ବାମ = ନୂତନ icu::UnicodeString(*first_ଡାହାଣ);
        new_First_ଡାହାଣ = ନୂତନ icu::UnicodeString(lhs);
    } ଅନ୍ୟଥା {
        // Both lhs and rhs belong to different equivalnce circles. We link
        // them together to form one single, larger equivalnce circle.
        new_First_ବାମ = ନୂତନ icu::UnicodeString(*first_ଡାହାଣ);
        new_First_ଡାହାଣ = ନୂତନ icu::UnicodeString(*first_ବାମ);
    }
    ଯଦି (new_First_ବାମ == nullptr || new_First_ଡାହାଣ == nullptr) {
        delete new_First_ବାମ;
        delete new_First_ଡାହାଣ;
        status = U_MEMORY_ALLOCATION_ERROR;
        ଫେରନ୍ତୁ;
    }
    hash->put(lhs, (ଶୂନ୍ୟ *) new_First_ବାମ, status);
    hash->put(rhs, (ଶୂନ୍ୟ *) new_First_ଡାହାଣ, status);
}

// countEquivalent counts how many strings are equivalent to s.
// hash stores all the equivalnce relations.
// countEquivalent does not include s itself in the count.
ସ୍ଥିର int32_t ଗଣନା_Equivalent(ଅଚଳ icu::Hashtable &hash, ଅଚଳ icu::UnicodeString &s) {
    int32_t result = 0;
    icu::EquivIterator iter(hash, s);
    ଯେତେବେଳେ (iter.next() != nullptr) {
        ++result;
    }
#ifdef UCURR_DEBUG_EQUIV
 {
   ଅକ୍ଷର tmp[200];
   s.extract(0,s.length(),tmp, "UTF-8");
   printf("CountEquivalent('%s') = %d\n", tmp, result);
 }
#endif
    ଫେରନ୍ତୁ result;
}

ସ୍ଥିର ଅଚଳ icu::Hashtable* getCurrSymbolsEquiv();

//------------------------------------------------------------
// Code

/**
 * Cleanup callback func
 */
ସ୍ଥିର UBool U_CALLCONV 
isoCodes_cleanup()
{
    ଯଦି (gIsoCodes != nullptr) {
        uhash_close(const_cast<UHashtable *>(gIsoCodes));
        gIsoCodes = nullptr;
    }
    gIsoCodesInitOnce.reset();
    ଫେରନ୍ତୁ ସତ୍ୟ;
}

/**
 * Cleanup callback func
 */
ସ୍ଥିର UBool U_CALLCONV 
currSymbolsEquiv_cleanup()
{
    delete const_cast<icu::Hashtable *>(gCurrSymbolsEquiv);
    gCurrSymbolsEquiv = nullptr;
    gCurrSymbolsEquivInitOnce.reset();
    ଫେରନ୍ତୁ ସତ୍ୟ;
}

/**
 * Deleter for IsoCodeEntry
 */
ସ୍ଥିର ଶୂନ୍ୟ U_CALLCONV
deleteIsoCodeEntry(ଶୂନ୍ୟ *obj) {
    IsoCodeEntry* entry = static_cast<IsoCodeEntry*>(obj);
    uprv_free(entry);
}

/**
 * Deleter for gCurrSymbolsEquiv.
 */
ସ୍ଥିର ଶୂନ୍ୟ U_CALLCONV
deleteUnicode(ଶୂନ୍ୟ *obj) {
    icu::UnicodeString* entry = static_cast<icu::UnicodeString*>(obj);
    delete entry;
}

/**
 * Unfortunately, we have to convert the char16_t* currency code to char*
 * to use it as a resource key.
 */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଅକ୍ଷର*
myUCharsToChars(ଅକ୍ଷର* resultOfLen4, ଅଚଳ char16_t* currency) {
    u_UCharsToChars(currency, resultOfLen4, ISO_CURRENCY_CODE_LENGTH);
    resultOfLen4[ISO_CURRENCY_CODE_LENGTH] = 0;
    ଫେରନ୍ତୁ resultOfLen4;
}

/**
 * Internal function to look up currency data.  Result is an array of
 * four integers.  The first is the fraction digits.  The second is the
 * rounding increment, or 0 if none.  The rounding increment is in
 * units of 10^(-fraction_digits).  The third and fourth are the same
 * except that they are those used in cash transactions ( cashDigits
 * and cashRounding ).
 */
ସ୍ଥିର ଅଚଳ int32_t*
_findMetaData(ଅଚଳ char16_t* currency, UErrorCode& ec) {

    ଯଦି (currency == nullptr || *currency == 0) {
        ଯଦି (U_SUCCESS(ec)) {
            ec = U_ILLEGAL_ARGUMENT_ERROR;
        }
        ଫେରନ୍ତୁ LAST_RESORT_DATA;
    }

    // Get CurrencyMeta resource out of root locale file.  [This may
    // move out of the root locale file later; if it does, update this
    // code.]
    UResourceBundle* currencyData = ures_openDirect(U_ICUDATA_CURR, CURRENCY_DATA, &ec);
    LocalUResourceBundlePointer currencyMeta(ures_get_By_ଚାବି(currencyData, CURRENCY_META, currencyData, &ec));

    ଯଦି (U_FAILURE(ec)) {
        // Config/build error; return hard-coded defaults
        ଫେରନ୍ତୁ LAST_RESORT_DATA;
    }

    // Look up our currency, or if that's not available, then DEFAULT
    ଅକ୍ଷର buf[ISO_CURRENCY_CODE_LENGTH+1];
    UErrorCode ec2 = U_ZERO_ERROR; // local error code: soft failure
    LocalUResourceBundlePointer rb(ures_get_By_ଚାବି(currencyMeta.getAlias(), myUCharsToChars(buf, currency), nullptr, &ec2));
      ଯଦି (U_FAILURE(ec2)) {
        rb.adoptInstead(ures_get_By_ଚାବି(currencyMeta.getAlias(),DEFAULT_META, nullptr, &ec));
        ଯଦି (U_FAILURE(ec)) {
            // Config/build error; return hard-coded defaults
            ଫେରନ୍ତୁ LAST_RESORT_DATA;
        }
    }

    int32_t len;
    ଅଚଳ int32_t *data = ures_getIntVector(rb.getAlias(), &len, &ec);
    ଯଦି (U_FAILURE(ec) || len != 4) {
        // Config/build error; return hard-coded defaults
        ଯଦି (U_SUCCESS(ec)) {
            ec = U_INVALID_FORMAT_ERROR;
        }
        ଫେରନ୍ତୁ LAST_RESORT_DATA;
    }

    ଫେରନ୍ତୁ data;
}

// -------------------------------------

ସ୍ଥିର CharString
idForLocale(ଅଚଳ ଅକ୍ଷର* locale, UErrorCode* ec)
{
    ଫେରନ୍ତୁ ulocimp_getRegionForSupplementalData(locale, ମିଥ୍ୟା, *ec);
}

// ------------------------------------------
//
// Registration
//
//-------------------------------------------

// don't use ICUService since we don't need fallback

U_CDECL_BEGIN
ସ୍ଥିର UBool U_CALLCONV currency_cleanup();
U_CDECL_END

#ଯଦି !UCONFIG_NO_SERVICE
ସଂରଚନା CReg;

ସ୍ଥିର UMutex gCRegLock;
ସ୍ଥିର CReg* gCRegHead = nullptr;

ସଂରଚନା CReg : ସର୍ବସାଧାରଣ icu::UMemory {
    CReg *next;
    char16_t iso[ISO_CURRENCY_CODE_LENGTH+1];
    ଅକ୍ଷର  id[ULOC_FULLNAME_CAPACITY];

    CReg(ଅଚଳ char16_t* _iso, ଅଚଳ ଅକ୍ଷର* _id)
        : next(nullptr)
    {
        uprv_strncpy(id, _id, ଆକାର(id)-1);
        id[ଆକାର(id)-1] = 0;
        u_memcpy(iso, _iso, ISO_CURRENCY_CODE_LENGTH);
        iso[ISO_CURRENCY_CODE_LENGTH] = 0;
    }

    ସ୍ଥିର UCurr_Registry_ଚାବି reg(ଅଚଳ char16_t* _iso, ଅଚଳ ଅକ୍ଷର* _id, UErrorCode* status)
    {
        ଯଦି (status && U_SUCCESS(*status) && _iso && _id) {
            CReg* n = ନୂତନ CReg(_iso, _id);
            ଯଦି (n) {
                umtx_lock(&gCRegLock);
                ଯଦି (!gCRegHead) {
                    /* register for the first time */
                    ucln_common_registerCleanup(UCLN_COMMON_CURRENCY, currency_cleanup);
                }
                n->next = gCRegHead;
                gCRegHead = n;
                umtx_unlock(&gCRegLock);
                ଫେରନ୍ତୁ n;
            }
            *status = U_MEMORY_ALLOCATION_ERROR;
        }
        ଫେରନ୍ତୁ nullptr;
    }

    ସ୍ଥିର UBool unreg(UCurr_Registry_ଚାବି ଚାବି) {
        UBool found = ମିଥ୍ୟା;
        umtx_lock(&gCRegLock);

        CReg** p = &gCRegHead;
        ଯେତେବେଳେ (*p) {
            ଯଦି (*p == ଚାବି) {
                *p = ((CReg*)ଚାବି)->next;
                delete (CReg*)ଚାବି;
                found = ସତ୍ୟ;
                ଭାଙ୍ଗ;
            }
            p = &((*p)->next);
        }

        umtx_unlock(&gCRegLock);
        ଫେରନ୍ତୁ found;
    }

    ସ୍ଥିର ଅଚଳ char16_t* get(ଅଚଳ ଅକ୍ଷର* id) {
        ଅଚଳ char16_t* result = nullptr;
        umtx_lock(&gCRegLock);
        CReg* p = gCRegHead;

        /* register cleanup of the mutex */
        ucln_common_registerCleanup(UCLN_COMMON_CURRENCY, currency_cleanup);
        ଯେତେବେଳେ (p) {
            ଯଦି (uprv_strcmp(id, p->id) == 0) {
                result = p->iso;
                ଭାଙ୍ଗ;
            }
            p = p->next;
        }
        umtx_unlock(&gCRegLock);
        ଫେରନ୍ତୁ result;
    }

    /* This doesn't need to be thread safe. It's for u_cleanup only. */
    ସ୍ଥିର ଶୂନ୍ୟ cleanup() {
        ଯେତେବେଳେ (gCRegHead) {
            CReg* n = gCRegHead;
            gCRegHead = gCRegHead->next;
            delete n;
        }
    }
};

// -------------------------------------

U_CAPI UCurr_Registry_ଚାବି U_EXPORT2
ucurr_register(ଅଚଳ char16_t* isoCode, ଅଚଳ ଅକ୍ଷର* locale, UErrorCode *status)
{
    ଯଦି (status && U_SUCCESS(*status)) {
        CharString id = idForLocale(locale, status);
        ଫେରନ୍ତୁ CReg::reg(isoCode, id.data(), status);
    }
    ଫେରନ୍ତୁ nullptr;
}

// -------------------------------------

U_CAPI UBool U_EXPORT2
ucurr_unregister(UCurr_Registry_ଚାବି ଚାବି, UErrorCode* status)
{
    ଯଦି (status && U_SUCCESS(*status)) {
        ଫେରନ୍ତୁ CReg::unreg(ଚାବି);
    }
    ଫେରନ୍ତୁ ମିଥ୍ୟା;
}
#endif /* UCONFIG_NO_SERVICE */

// -------------------------------------

/**
 * Release all static memory held by currency.
 */
/*The declaration here is needed so currency_cleanup()
 * can call this function.
 */
ସ୍ଥିର UBool U_CALLCONV
currency_cache_cleanup();

U_CDECL_BEGIN
ସ୍ଥିର UBool U_CALLCONV currency_cleanup() {
#ଯଦି !UCONFIG_NO_SERVICE
    CReg::cleanup();
#endif
    /*
     * There might be some cached currency data or isoCodes data.
     */
    currency_cache_cleanup();
    isoCodes_cleanup();
    currSymbolsEquiv_cleanup();

    ଫେରନ୍ତୁ ସତ୍ୟ;
}
U_CDECL_END

// -------------------------------------

U_CAPI int32_t U_EXPORT2
ucurr_forLocale(ଅଚଳ ଅକ୍ଷର* locale,
                char16_t* buff,
                int32_t buffCapacity,
                UErrorCode* ec) {
    ଯଦି (U_FAILURE(*ec)) { ଫେରନ୍ତୁ 0; }
    ଯଦି (buffCapacity < 0 || (buff == nullptr && buffCapacity > 0)) {
        *ec = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ 0;
    }

    UErrorCode localStatus = U_ZERO_ERROR;
    CharString currency = ulocimp_get_Keyword_ମୂଲ୍ୟ(locale, "currency", localStatus);
    int32_t resLen = currency.length();

    ଯଦି (U_SUCCESS(localStatus) && resLen == 3 && uprv_isInvariantString(currency.data(), resLen)) {
        ଯଦି (resLen < buffCapacity) {
            T_CString_toUpperCase(currency.data());
            u_charsToUChars(currency.data(), buff, resLen);
        }
        ଫେରନ୍ତୁ u_terminateUChars(buff, buffCapacity, resLen, ec);
    }

    // get country or country_variant in `id'
    CharString id = idForLocale(locale, ec);
    ଯଦି (U_FAILURE(*ec)) {
        ଫେରନ୍ତୁ 0;
    }

#ଯଦି !UCONFIG_NO_SERVICE
    ଅଚଳ char16_t* result = CReg::get(id.data());
    ଯଦି (result) {
        ଯଦି(buffCapacity > u_strlen(result)) {
            u_strcpy(buff, result);
        }
        resLen = u_strlen(result);
        ଫେରନ୍ତୁ u_terminateUChars(buff, buffCapacity, resLen, ec);
    }
#endif
    // Remove variants, which is only needed for registration.
    ଅକ୍ଷର *idDelim = uprv_strchr(id.data(), VAR_DELIM);
    ଯଦି (idDelim) {
        id.truncate(idDelim - id.data());
    }

    ଅଚଳ char16_t* s = nullptr;  // Currency code from data file.
    ଯଦି (id.isEmpty()) {
        // No point looking in the data for an empty string.
        // This is what we would get.
        localStatus = U_MISSING_RESOURCE_ERROR;
    } ଅନ୍ୟଥା {
        // Look up the CurrencyMap element in the root bundle.
        localStatus = U_ZERO_ERROR;
        UResourceBundle *rb = ures_openDirect(U_ICUDATA_CURR, CURRENCY_DATA, &localStatus);
        UResourceBundle *cm = ures_get_By_ଚାବି(rb, CURRENCY_MAP, rb, &localStatus);
        LocalUResourceBundlePointer countryArray(ures_get_By_ଚାବି(rb, id.data(), cm, &localStatus));
        // https://unicode-org.atlassian.net/browse/ICU-21997
        // Prefer to use currencies that are legal tender.
        ଯଦି (U_SUCCESS(localStatus)) {
            int32_t arrayLength = ures_getSize(countryArray.getAlias());
            ପାଇଁ (int32_t i = 0; i < arrayLength; ++i) {
                LocalUResourceBundlePointer currencyReq(
                    ures_getByIndex(countryArray.getAlias(), i, nullptr, &localStatus));
                // The currency is legal tender if it is *not* marked with tender{"false"}.
                UErrorCode tenderStatus = localStatus;
                ଅଚଳ char16_t *tender =
                    ures_get_String_By_ଚାବି(currencyReq.getAlias(), "tender", nullptr, &tenderStatus);
                ତାର୍କିକ isTender = U_FAILURE(tenderStatus) || u_strcmp(tender, u"false") != 0;
                ଯଦି (!isTender && s != nullptr) {
                    // We already have a non-tender currency. Ignore all following non-tender ones.
                    ଜାରି;
                }
                // Fetch the currency code.
                s = ures_get_String_By_ଚାବି(currencyReq.getAlias(), "id", &resLen, &localStatus);
                ଯଦି (isTender) {
                    ଭାଙ୍ଗ;
                }
            }
            ଯଦି (U_SUCCESS(localStatus) && s == nullptr) {
                localStatus = U_MISSING_RESOURCE_ERROR;
            }
        }
    }

    ଯଦି ((U_FAILURE(localStatus)) && strchr(id.data(), '_') != nullptr) {
        // We don't know about it.  Check to see if we support the variant.
        CharString parent = ulocimp_getParent(locale, *ec);
        *ec = U_USING_FALLBACK_WARNING;
        // TODO: Loop over the parent rather than recursing and
        // looking again for a currency keyword.
        ଫେରନ୍ତୁ ucurr_forLocale(parent.data(), buff, buffCapacity, ec);
    }
    ଯଦି (*ec == U_ZERO_ERROR || localStatus != U_ZERO_ERROR) {
        // There is nothing to fallback to. Report the failure/warning if possible.
        *ec = localStatus;
    }
    ଯଦି (U_SUCCESS(*ec)) {
        ଯଦି(buffCapacity > resLen) {
            u_strcpy(buff, s);
        }
    }
    ଫେରନ୍ତୁ u_terminateUChars(buff, buffCapacity, resLen, ec);
}

// end registration

/**
 * Modify the given locale name by removing the rightmost _-delimited
 * element.  If there is none, empty the string ("" == root).
 * NOTE: The string "root" is not recognized; do not use it.
 * @return true if the fallback happened; false if locale is already
 * root ("").
 */
ସ୍ଥିର UBool fallback(CharString& loc) {
    ଯଦି (loc.isEmpty()) {
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    UErrorCode status = U_ZERO_ERROR;
    ଯଦି (loc == "en_GB") {
        // HACK: See #13368.  We need "en_GB" to fall back to "en_001" instead of "en"
        // in order to consume the correct data strings.  This hack will be removed
        // when proper data sink loading is implemented here.
        loc.truncate(3);
        loc.append("001", status);
    } ଅନ୍ୟଥା {
        loc = ulocimp_getParent(loc.data(), status);
    }
 /*
    char *i = uprv_strrchr(loc, '_');
    if (i == nullptr) {
        i = loc;
    }
    *i = 0;
 */
    ଫେରନ୍ତୁ ସତ୍ୟ;
}


U_CAPI ଅଚଳ char16_t* U_EXPORT2
ucurr_getName(ଅଚଳ char16_t* currency U_LIFETIME_BOUND,
              ଅଚଳ ଅକ୍ଷର* locale,
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

    ଯଦି (U_FAILURE(*ec)) {
        ଫେରନ୍ତୁ nullptr;
    }

    int32_t choice = (int32_t) nameStyle;
    ଯଦି (choice < 0 || choice > 4) {
        *ec = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ nullptr;
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

    ଯଦି (locale == nullptr) {
        locale = uloc_getDefault();
    }
    CharString loc = ulocimp_getName(locale, ec2);
    ଯଦି (U_FAILURE(ec2)) {
        *ec = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ nullptr;
    }

    ଅକ୍ଷର buf[ISO_CURRENCY_CODE_LENGTH+1];
    myUCharsToChars(buf, currency);
    
    /* Normalize the keyword value to uppercase */
    T_CString_toUpperCase(buf);
    
    ଅଚଳ char16_t* s = nullptr;
    ec2 = U_ZERO_ERROR;
    LocalUResourceBundlePointer rb(ures_open(U_ICUDATA_CURR, loc.data(), &ec2));

    ଯଦି (nameStyle == UCURR_NARROW_ପ୍ରତୀକ_NAME || nameStyle == UCURR_FORMAL_ପ୍ରତୀକ_NAME || nameStyle == UCURR_VARIANT_ପ୍ରତୀକ_NAME) {
        CharString ଚାବି;
        ବାଛ (nameStyle) {
        ସ୍ଥିତି UCURR_NARROW_ପ୍ରତୀକ_NAME:
            ଚାବି.append(CURRENCIES_NARROW, ec2);
            ଭାଙ୍ଗ;
        ସ୍ଥିତି UCURR_FORMAL_ପ୍ରତୀକ_NAME:
            ଚାବି.append(CURRENCIES_FORMAL, ec2);
            ଭାଙ୍ଗ;
        ସ୍ଥିତି UCURR_VARIANT_ପ୍ରତୀକ_NAME:
            ଚାବି.append(CURRENCIES_VARIANT, ec2);
            ଭାଙ୍ଗ;
        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
            *ec = U_UNSUPPORTED_ERROR;
            ଫେରନ୍ତୁ nullptr;
        }
        ଚାବି.append("/", ec2);
        ଚାବି.append(buf, ec2);
        s = ures_get_String_By_ଚାବି_With_Fallback(rb.getAlias(), ଚାବି.data(), len, &ec2);
        ଯଦି (ec2 == U_MISSING_RESOURCE_ERROR) {
            *ec = U_USING_FALLBACK_WARNING;
            ec2 = U_ZERO_ERROR;
            choice = UCURR_ପ୍ରତୀକ_NAME;
        }
    }
    ଯଦି (s == nullptr) {
        ures_get_By_ଚାବି(rb.getAlias(), CURRENCIES, rb.getAlias(), &ec2);
        ures_get_By_ଚାବି_With_Fallback(rb.getAlias(), buf, rb.getAlias(), &ec2);
        s = ures_getStringByIndex(rb.getAlias(), choice, len, &ec2);
    }

    // If we've succeeded we're done.  Otherwise, try to fallback.
    // If that fails (because we are already at root) then exit.
    ଯଦି (U_SUCCESS(ec2)) {
        ଯଦି (ec2 == U_USING_DEFAULT_WARNING
            || (ec2 == U_USING_FALLBACK_WARNING && *ec != U_USING_DEFAULT_WARNING)) {
            *ec = ec2;
        }
    }

    // We no longer support choice format data in names.  Data should not contain
    // choice patterns.
    ଯଦି (isChoiceFormat != nullptr) {
        *isChoiceFormat = ମିଥ୍ୟା;
    }
    ଯଦି (U_SUCCESS(ec2)) {
        U_ASSERT(s != nullptr);
        ଫେରନ୍ତୁ s;
    }

    // If we fail to find a match, use the ISO 4217 code
    *len = u_strlen(currency); // Should == ISO_CURRENCY_CODE_LENGTH, but maybe not...?
    *ec = U_USING_DEFAULT_WARNING;
    ଫେରନ୍ତୁ currency;
}

U_CAPI ଅଚଳ char16_t* U_EXPORT2
ucurr_getPluralName(ଅଚଳ char16_t* currency U_LIFETIME_BOUND,
                    ଅଚଳ ଅକ୍ଷର* locale,
                    UBool* isChoiceFormat,
                    ଅଚଳ ଅକ୍ଷର* plural_ଗଣନା,
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

    ଯଦି (U_FAILURE(*ec)) {
        ଫେରନ୍ତୁ nullptr;
    }

    // Use a separate UErrorCode here that does not propagate out of
    // this function.
    UErrorCode ec2 = U_ZERO_ERROR;

    ଯଦି (locale == nullptr) {
        locale = uloc_getDefault();
    }
    CharString loc = ulocimp_getName(locale, ec2);
    ଯଦି (U_FAILURE(ec2)) {
        *ec = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ nullptr;
    }

    ଅକ୍ଷର buf[ISO_CURRENCY_CODE_LENGTH+1];
    myUCharsToChars(buf, currency);

    ଅଚଳ char16_t* s = nullptr;
    ec2 = U_ZERO_ERROR;
    UResourceBundle* rb = ures_open(U_ICUDATA_CURR, loc.data(), &ec2);

    rb = ures_get_By_ଚାବି(rb, CURRENCYPLURALS, rb, &ec2);

    // Fetch resource with multi-level resource inheritance fallback
    LocalUResourceBundlePointer curr(ures_get_By_ଚାବି_With_Fallback(rb, buf, rb, &ec2));

    s = ures_get_String_By_ଚାବି_With_Fallback(curr.getAlias(), plural_ଗଣନା, len, &ec2);
    ଯଦି (U_FAILURE(ec2)) {
        //  fall back to "other"
        ec2 = U_ZERO_ERROR;
        s = ures_get_String_By_ଚାବି_With_Fallback(curr.getAlias(), "other", len, &ec2);     
        ଯଦି (U_FAILURE(ec2)) {
            // fall back to long name in Currencies
            ଫେରନ୍ତୁ ucurr_getName(currency, locale, UCURR_LONG_NAME, 
                                 isChoiceFormat, len, ec);
        }
    }

    // If we've succeeded we're done.  Otherwise, try to fallback.
    // If that fails (because we are already at root) then exit.
    ଯଦି (U_SUCCESS(ec2)) {
        ଯଦି (ec2 == U_USING_DEFAULT_WARNING
            || (ec2 == U_USING_FALLBACK_WARNING && *ec != U_USING_DEFAULT_WARNING)) {
            *ec = ec2;
        }
        U_ASSERT(s != nullptr);
        ଫେରନ୍ତୁ s;
    }

    // If we fail to find a match, use the ISO 4217 code
    *len = u_strlen(currency); // Should == ISO_CURRENCY_CODE_LENGTH, but maybe not...?
    *ec = U_USING_DEFAULT_WARNING;
    ଫେରନ୍ତୁ currency;
}


//========================================================================
// Following are structure and function for parsing currency names

#define NEED_TO_BE_DELETED 0x1

// TODO: a better way to define this?
#define MAX_CURRENCY_NAME_LEN 100

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
    ଅଚଳ ଅକ୍ଷର* IsoCode;  //ଚାବି
    char16_t* currencyName;  //ମୂଲ୍ୟ
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
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା U_CALLCONV currencyNameComparator(ଅଚଳ ଶୂନ୍ୟ* a, ଅଚଳ ଶୂନ୍ୟ* b) {
    ଅଚଳ CurrencyNameStruct* currName_1 = static_cast<ଅଚଳ CurrencyNameStruct*>(a);
    ଅଚଳ CurrencyNameStruct* currName_2 = static_cast<ଅଚଳ CurrencyNameStruct*>(b);
    ପାଇଁ (int32_t i = 0; 
         i < MIN(currName_1->currencyNameLen, currName_2->currencyNameLen);
         ++i) {
        ଯଦି (currName_1->currencyName[i] < currName_2->currencyName[i]) {
            ଫେରନ୍ତୁ -1;
        }
        ଯଦି (currName_1->currencyName[i] > currName_2->currencyName[i]) {
            ଫେରନ୍ତୁ 1;
        }
    }
    ଯଦି (currName_1->currencyNameLen < currName_2->currencyNameLen) {
        ଫେରନ୍ତୁ -1;
    } ଅନ୍ୟଥା ଯଦି (currName_1->currencyNameLen > currName_2->currencyNameLen) {
        ଫେରନ୍ତୁ 1;
    }
    ଫେରନ୍ତୁ 0;
}


// Give a locale, return the maximum number of currency names associated with
// this locale.
// It gets currency names from resource bundles using fallback.
// It is the maximum number because in the fallback chain, some of the 
// currency names are duplicated.
// For example, given locale as "en_US", the currency names get from resource
// bundle in "en_US" and "en" are duplicated. The fallback mechanism will count
// all currency names in "en_US" and "en".
ସ୍ଥିର ଶୂନ୍ୟ
get_Currency_Name_ଗଣନା(ଅଚଳ ଅକ୍ଷର* loc, int32_t* total_currency_name_ଗଣନା, int32_t* total_currency_ପ୍ରତୀକ_ଗଣନା) {
    U_NAMESPACE_USE
    *total_currency_name_ଗଣନା = 0;
    *total_currency_ପ୍ରତୀକ_ଗଣନା = 0;
    ଅଚଳ char16_t* s = nullptr;
    CharString locale;
    {
        UErrorCode status = U_ZERO_ERROR;
        locale.append(loc, status);
        ଯଦି (U_FAILURE(status)) { ଫେରନ୍ତୁ; }
    }
    ଅଚଳ icu::Hashtable *currencySymbolsEquiv = getCurrSymbolsEquiv();
    ପାଇଁ (;;) {
        UErrorCode ec2 = U_ZERO_ERROR;
        // TODO: ures_openDirect?
        LocalUResourceBundlePointer rb(ures_open(U_ICUDATA_CURR, locale.data(), &ec2));
        LocalUResourceBundlePointer curr(ures_get_By_ଚାବି(rb.getAlias(), CURRENCIES, nullptr, &ec2));
        int32_t n = ures_getSize(curr.getAlias());
        ପାଇଁ (int32_t i=0; i<n; ++i) {
            LocalUResourceBundlePointer names(ures_getByIndex(curr.getAlias(), i, nullptr, &ec2));
            int32_t len;
            s = ures_getStringByIndex(names.getAlias(), UCURR_ପ୍ରତୀକ_NAME, &len, &ec2);
            ++(*total_currency_ପ୍ରତୀକ_ଗଣନା);  // currency symbol
            ଯଦି (currencySymbolsEquiv != nullptr) {
                *total_currency_ପ୍ରତୀକ_ଗଣନା += ଗଣନା_Equivalent(*currencySymbolsEquiv, UnicodeString(ସତ୍ୟ, s, len));
            }
            ++(*total_currency_ପ୍ରତୀକ_ଗଣନା); // iso code
            ++(*total_currency_name_ଗଣନା); // long name
        }

        // currency plurals
        UErrorCode ec3 = U_ZERO_ERROR;
        LocalUResourceBundlePointer curr_p(ures_get_By_ଚାବି(rb.getAlias(), CURRENCYPLURALS, nullptr, &ec3));
        n = ures_getSize(curr_p.getAlias());
        ପାଇଁ (int32_t i=0; i<n; ++i) {
            LocalUResourceBundlePointer names(ures_getByIndex(curr_p.getAlias(), i, nullptr, &ec3));
            *total_currency_name_ଗଣନା += ures_getSize(names.getAlias());
        }

        ଯଦି (!fallback(locale)) {
            ଭାଙ୍ଗ;
        }
    }
}

ସ୍ଥିର char16_t*
toUpperCase(ଅଚଳ char16_t* source, int32_t len, ଅଚଳ ଅକ୍ଷର* locale) {
    char16_t* dest = nullptr;
    UErrorCode ec = U_ZERO_ERROR;
    int32_t destLen = u_strToUpper(dest, 0, source, len, locale, &ec);

    ec = U_ZERO_ERROR;
    dest = static_cast<char16_t*>(uprv_malloc(ଆକାର(char16_t) * MAX(destLen, len)));
    ଯଦି (dest == nullptr) {
        ଫେରନ୍ତୁ nullptr;
    }
    u_strToUpper(dest, destLen, source, len, locale, &ec);
    ଯଦି (U_FAILURE(ec)) {
        u_memcpy(dest, source, len);
    } 
    ଫେରନ୍ତୁ dest;
}


ସ୍ଥିର ଶୂନ୍ୟ deleteCurrencyNames(CurrencyNameStruct* currencyNames, int32_t count);
// Collect all available currency names associated with the given locale
// (enable fallback chain).
// Read currenc names defined in resource bundle "Currencies" and
// "CurrencyPlural", enable fallback chain.
// return the malloc-ed currency name arrays and the total number of currency
// names in the array.
ସ୍ଥିର ଶୂନ୍ୟ
collectCurrencyNames(ଅଚଳ ଅକ୍ଷର* locale, 
                     CurrencyNameStruct** currencyNames, 
                     int32_t* total_currency_name_ଗଣନା, 
                     CurrencyNameStruct** currencySymbols, 
                     int32_t* total_currency_ପ୍ରତୀକ_ଗଣନା, 
                     UErrorCode& ec) {
    ଯଦି (U_FAILURE(ec)) {
        *currencyNames = *currencySymbols = nullptr;
        *total_currency_name_ଗଣନା = *total_currency_ପ୍ରତୀକ_ଗଣନା = 0;
        ଫେରନ୍ତୁ;
    }
    U_NAMESPACE_USE
    ଅଚଳ icu::Hashtable *currencySymbolsEquiv = getCurrSymbolsEquiv();
    // Look up the Currencies resource for the given locale.
    UErrorCode ec2 = U_ZERO_ERROR;

    ଯଦି (locale == nullptr) {
        locale = uloc_getDefault();
    }
    CharString loc = ulocimp_getName(locale, ec2);
    ଯଦି (U_FAILURE(ec2)) {
        ec = U_ILLEGAL_ARGUMENT_ERROR;
        *currencyNames = *currencySymbols = nullptr;
        *total_currency_name_ଗଣନା = *total_currency_ପ୍ରତୀକ_ଗଣନା = 0;
        ଫେରନ୍ତୁ;
    }

    // Get maximum currency name count first.
    get_Currency_Name_ଗଣନା(loc.data(), total_currency_name_ଗଣନା, total_currency_ପ୍ରତୀକ_ଗଣନା);

    *currencyNames = static_cast<CurrencyNameStruct*>(
        uprv_malloc(ଆକାର(CurrencyNameStruct) * (*total_currency_name_ଗଣନା)));
    ଯଦି(*currencyNames == nullptr) {
        *currencySymbols = nullptr;
        *total_currency_name_ଗଣନା = *total_currency_ପ୍ରତୀକ_ଗଣନା = 0;
        ec = U_MEMORY_ALLOCATION_ERROR;
        ଫେରନ୍ତୁ;
    }
    *currencySymbols = static_cast<CurrencyNameStruct*>(
        uprv_malloc(ଆକାର(CurrencyNameStruct) * (*total_currency_ପ୍ରତୀକ_ଗଣନା)));

    ଯଦି(*currencySymbols == nullptr) {
        uprv_free(*currencyNames);
        *currencyNames = nullptr;
        *total_currency_name_ଗଣନା = *total_currency_ପ୍ରତୀକ_ଗଣନା = 0;
        ec = U_MEMORY_ALLOCATION_ERROR;
        ଫେରନ୍ତୁ;
    }

    ଅଚଳ char16_t* s = nullptr;  // currency name
    ଅକ୍ଷର* iso = nullptr;  // currency ISO code

    *total_currency_name_ଗଣନା = 0;
    *total_currency_ପ୍ରତୀକ_ଗଣନା = 0;

    UErrorCode ec3 = U_ZERO_ERROR;
    UErrorCode ec4 = U_ZERO_ERROR;

    // Using hash to remove duplicates caused by locale fallback
    LocalUHashtablePointer currencyIsoCodes(uhash_open(uhash_hashChars, uhash_compareChars, nullptr, &ec3));
    LocalUHashtablePointer currencyPluralIsoCodes(uhash_open(uhash_hashChars, uhash_compareChars, nullptr, &ec4));
    ପାଇଁ (int32_t localeLevel = 0; ; ++localeLevel) {
        ec2 = U_ZERO_ERROR;
        // TODO: ures_openDirect
        LocalUResourceBundlePointer rb(ures_open(U_ICUDATA_CURR, loc.data(), &ec2));
        LocalUResourceBundlePointer curr(ures_get_By_ଚାବି(rb.getAlias(), CURRENCIES, nullptr, &ec2));
        int32_t n = ures_getSize(curr.getAlias());
        ପାଇଁ (int32_t i=0; i<n; ++i) {
            LocalUResourceBundlePointer names(ures_getByIndex(curr.getAlias(), i, nullptr, &ec2));
            int32_t len;
            s = ures_getStringByIndex(names.getAlias(), UCURR_ପ୍ରତୀକ_NAME, &len, &ec2);
            // TODO: uhash_put wont change key/value?
            iso = const_cast<ଅକ୍ଷର*>(ures_get_ଚାବି(names.getAlias()));
            ଯଦି (localeLevel != 0 && uhash_get(currencyIsoCodes.getAlias(), iso) != nullptr) {
                ଜାରି;
            }
            uhash_put(currencyIsoCodes.getAlias(), iso, iso, &ec3); 
            // Add currency symbol.
            (*currencySymbols)[(*total_currency_ପ୍ରତୀକ_ଗଣନା)++] = {iso, const_cast<char16_t*>(s), len, 0};

            // Add equivalent symbols
            ଯଦି (currencySymbolsEquiv != nullptr) {
                UnicodeString str(ସତ୍ୟ, s, len);
                icu::EquivIterator iter(*currencySymbolsEquiv, str);
                ଅଚଳ UnicodeString *ପ୍ରତୀକ;
                ଯେତେବେଳେ ((ପ୍ରତୀକ = iter.next()) != nullptr) {
                    (*currencySymbols)[(*total_currency_ପ୍ରତୀକ_ଗଣନା)++]
                        = {iso, const_cast<char16_t*>(ପ୍ରତୀକ->getBuffer()), ପ୍ରତୀକ->length(), 0};
                }
            }

            // Add currency long name.
            s = ures_getStringByIndex(names.getAlias(), UCURR_LONG_NAME, &len, &ec2);
            char16_t* upperName = toUpperCase(s, len, locale);
            ଯଦି (upperName == nullptr) {
                ec = U_MEMORY_ALLOCATION_ERROR;
                ଯାଅ error;
            }
            (*currencyNames)[(*total_currency_name_ଗଣନା)++] = {iso, upperName, len, NEED_TO_BE_DELETED};

            // put (iso, 3, and iso) in to array
            // Add currency ISO code.
            char16_t* isoCode = static_cast<char16_t*>(uprv_malloc(ଆକାର(char16_t) * 3));
            ଯଦି (isoCode == nullptr) {
                ec = U_MEMORY_ALLOCATION_ERROR;
                ଯାଅ error;
            }
            // Must convert iso[] into Unicode
            u_charsToUChars(iso, isoCode, 3);
            (*currencySymbols)[(*total_currency_ପ୍ରତୀକ_ଗଣନା)++] = {iso, isoCode, 3, NEED_TO_BE_DELETED};
        }

        // currency plurals
        UErrorCode ec5 = U_ZERO_ERROR;
        LocalUResourceBundlePointer curr_p(ures_get_By_ଚାବି(rb.getAlias(), CURRENCYPLURALS, nullptr, &ec5));
        n = ures_getSize(curr_p.getAlias());
        ପାଇଁ (int32_t i=0; i<n; ++i) {
            LocalUResourceBundlePointer names(ures_getByIndex(curr_p.getAlias(), i, nullptr, &ec5));
            iso = const_cast<ଅକ୍ଷର*>(ures_get_ଚାବି(names.getAlias()));
            // Using hash to remove duplicated ISO codes in fallback chain.
            ଯଦି (localeLevel != 0 && uhash_get(currencyPluralIsoCodes.getAlias(), iso) != nullptr) {
                ଜାରି;
            }
            uhash_put(currencyPluralIsoCodes.getAlias(), iso, iso, &ec4);
            int32_t num = ures_getSize(names.getAlias());
            int32_t len;
            ପାଇଁ (int32_t j = 0; j < num; ++j) {
                // TODO: remove duplicates between singular name and 
                // currency long name?
                s = ures_getStringByIndex(names.getAlias(), j, &len, &ec5);
                char16_t* upperName = toUpperCase(s, len, locale);
                ଯଦି (upperName == nullptr) {
                    ec = U_MEMORY_ALLOCATION_ERROR;
                    ଯାଅ error;
                }
                (*currencyNames)[(*total_currency_name_ଗଣନା)++] = {iso, upperName, len, NEED_TO_BE_DELETED};
            }
        }

        ଯଦି (!fallback(loc)) {
            ଭାଙ୍ଗ;
        }
    }

    // quick sort the struct
    qsort(*currencyNames, *total_currency_name_ଗଣନା, 
          ଆକାର(CurrencyNameStruct), currencyNameComparator);
    qsort(*currencySymbols, *total_currency_ପ୍ରତୀକ_ଗଣନା, 
          ଆକାର(CurrencyNameStruct), currencyNameComparator);

#ifdef UCURR_DEBUG
    printf("currency name count: %d\n", *total_currency_name_ଗଣନା);
    ପାଇଁ (int32_t index = 0; index < *total_currency_name_ଗଣନା; ++index) {
        printf("index: %d\n", index);
        printf("iso: %s\n", (*currencyNames)[index].IsoCode);
        ଅକ୍ଷର curNameBuf[1024];
        memset(curNameBuf, 0, 1024);
        u_austrncpy(curNameBuf, (*currencyNames)[index].currencyName, (*currencyNames)[index].currencyNameLen);
        printf("currencyName: %s\n", curNameBuf);
        printf("len: %d\n", (*currencyNames)[index].currencyNameLen);
    }
    printf("currency symbol count: %d\n", *total_currency_ପ୍ରତୀକ_ଗଣନା);
    ପାଇଁ (int32_t index = 0; index < *total_currency_ପ୍ରତୀକ_ଗଣନା; ++index) {
        printf("index: %d\n", index);
        printf("iso: %s\n", (*currencySymbols)[index].IsoCode);
        ଅକ୍ଷର curNameBuf[1024];
        memset(curNameBuf, 0, 1024);
        u_austrncpy(curNameBuf, (*currencySymbols)[index].currencyName, (*currencySymbols)[index].currencyNameLen);
        printf("currencySymbol: %s\n", curNameBuf);
        printf("len: %d\n", (*currencySymbols)[index].currencyNameLen);
    }
#endif
    // fail on hashtable errors
    ଯଦି (U_FAILURE(ec3)) {
      ec = ec3;
    } ଅନ୍ୟଥା ଯଦି (U_FAILURE(ec4)) {
      ec = ec4;
    }

  error:
    // clean up if we got error
    ଯଦି (U_FAILURE(ec)) {
        deleteCurrencyNames(*currencyNames, *total_currency_name_ଗଣନା);
        deleteCurrencyNames(*currencySymbols, *total_currency_ପ୍ରତୀକ_ଗଣନା);
        *currencyNames = *currencySymbols = nullptr;
        *total_currency_name_ଗଣନା = *total_currency_ପ୍ରତୀକ_ଗଣନା = 0;
    }
}

// @param  currencyNames: currency names array
// @param  indexInCurrencyNames: the index of the character in currency names 
//         array against which the comparison is done
// @param  key: input text char to compare against
// @param  begin(IN/OUT): the begin index of matching range in currency names array
// @param  end(IN/OUT): the end index of matching range in currency names array.
ସ୍ଥିର int32_t
ଦ୍ୱିମିକ_ସନ୍ଧାନ(ଅଚଳ CurrencyNameStruct* currencyNames, 
             int32_t indexInCurrencyNames,
             ଅଚଳ char16_t ଚାବି,
             int32_t* begin, int32_t* end) {
#ifdef UCURR_DEBUG
    printf("key = %x\n", ଚାବି);
#endif
   int32_t first = *begin;
   int32_t last = *end;
   ଯେତେବେଳେ (first <= last) {
       int32_t mid = (first + last) / 2;  // compute mid point.
       ଯଦି (indexInCurrencyNames >= currencyNames[mid].currencyNameLen) {
           first = mid + 1;
       } ଅନ୍ୟଥା {
           ଯଦି (ଚାବି > currencyNames[mid].currencyName[indexInCurrencyNames]) {
               first = mid + 1;
           }
           ଅନ୍ୟଥା ଯଦି (ଚାବି < currencyNames[mid].currencyName[indexInCurrencyNames]) {
               last = mid - 1;
           }
           ଅନ୍ୟଥା {
                // Find a match, and looking for ranges
                // Now do two more binary searches. First, on the left side for
                // the greatest L such that CurrencyNameStruct[L] < key.
                int32_t L = *begin;
                int32_t R = mid;

#ifdef UCURR_DEBUG
                printf("mid = %d\n", mid);
#endif
                ଯେତେବେଳେ (L < R) {
                    int32_t M = (L + R) / 2;
#ifdef UCURR_DEBUG
                    printf("L = %d, R = %d, M = %d\n", L, R, M);
#endif
                    ଯଦି (indexInCurrencyNames >= currencyNames[M].currencyNameLen) {
                        L = M + 1;
                    } ଅନ୍ୟଥା {
                        ଯଦି (currencyNames[M].currencyName[indexInCurrencyNames] < ଚାବି) {
                            L = M + 1;
                        } ଅନ୍ୟଥା {
#ifdef UCURR_DEBUG
                            U_ASSERT(currencyNames[M].currencyName[indexInCurrencyNames] == ଚାବି);
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
                U_ASSERT(currencyNames[*begin].currencyName[indexInCurrencyNames] == ଚାବି);
#endif

                // Now for the second search, finding the least R such that
                // key < CurrencyNameStruct[R].
                L = mid;
                R = *end;
                ଯେତେବେଳେ (L < R) {
                    int32_t M = (L + R) / 2;
#ifdef UCURR_DEBUG
                    printf("L = %d, R = %d, M = %d\n", L, R, M);
#endif
                    ଯଦି (currencyNames[M].currencyNameLen < indexInCurrencyNames) {
                        L = M + 1;
                    } ଅନ୍ୟଥା {
                        ଯଦି (currencyNames[M].currencyName[indexInCurrencyNames] > ଚାବି) {
                            R = M;
                        } ଅନ୍ୟଥା {
#ifdef UCURR_DEBUG
                            U_ASSERT(currencyNames[M].currencyName[indexInCurrencyNames] == ଚାବି);
#endif
                            L = M + 1;
                        }
                    }
                }
#ifdef UCURR_DEBUG
                U_ASSERT(L == R);
#endif
                ଯଦି (currencyNames[R].currencyName[indexInCurrencyNames] > ଚାବି) {
                    *end = R - 1;
                } ଅନ୍ୟଥା {
                    *end = R;
                }
#ifdef UCURR_DEBUG
                printf("end = %d\n", *end);
#endif

                // now, found the range. check whether there is exact match
                ଯଦି (currencyNames[*begin].currencyNameLen == indexInCurrencyNames + 1) {
                    ଫେରନ୍ତୁ *begin;  // find range and exact match.
                }
                ଫେରନ୍ତୁ -1;  // find range, but no exact match.
           }
       }
   }
   *begin = -1;
   *end = -1;
   ଫେରନ୍ତୁ -1;    // failed to find range.
}


// Linear search "text" in "currencyNames".
// @param  begin, end: the begin and end index in currencyNames, within which
//         range should the search be performed.
// @param  textLen: the length of the text to be compared
// @param  maxMatchLen(IN/OUT): passing in the computed max matching length
//                              pass out the new max  matching length
// @param  maxMatchIndex: the index in currencyName which has the longest
//                        match with input text.
ସ୍ଥିର ଶୂନ୍ୟ
linear_ସନ୍ଧାନ(ଅଚଳ CurrencyNameStruct* currencyNames, 
             int32_t begin, int32_t end,
             ଅଚଳ char16_t* text, int32_t textLen,
             int32_t *partialMatchLen,
             int32_t *maxMatchLen, int32_t* maxMatchIndex) {
    int32_t initialPartialMatchLen = *partialMatchLen;
    ପାଇଁ (int32_t index = begin; index <= end; ++index) {
        int32_t len = currencyNames[index].currencyNameLen;
        ଯଦି (len > *maxMatchLen && len <= textLen &&
            uprv_memcmp(currencyNames[index].currencyName, text, len * ଆକାର(char16_t)) == 0) {
            *partialMatchLen = MAX(*partialMatchLen, len);
            *maxMatchIndex = index;
            *maxMatchLen = len;
#ifdef UCURR_DEBUG
            printf("maxMatchIndex = %d, maxMatchLen = %d\n",
                   *maxMatchIndex, *maxMatchLen);
#endif
        } ଅନ୍ୟଥା {
            // Check for partial matches.
            ପାଇଁ (int32_t i=initialPartialMatchLen; i<MIN(len, textLen); i++) {
                ଯଦି (currencyNames[index].currencyName[i] != text[i]) {
                    ଭାଙ୍ଗ;
                }
                *partialMatchLen = MAX(*partialMatchLen, i + 1);
            }
        }
    }
}

#define LINEAR_ସନ୍ଧାନ_THRESHOLD 10

// Find longest match between "text" and currency names in "currencyNames".
// @param  total_currency_count: total number of currency names in CurrencyNames.
// @param  textLen: the length of the text to be compared
// @param  maxMatchLen: passing in the computed max matching length
//                              pass out the new max  matching length
// @param  maxMatchIndex: the index in currencyName which has the longest
//                        match with input text.
ସ୍ଥିର ଶୂନ୍ୟ
ସନ୍ଧାନ_Currency_Name(ଅଚଳ CurrencyNameStruct* currencyNames, 
                   int32_t total_currency_ଗଣନା,
                   ଅଚଳ char16_t* text, int32_t textLen,
                   int32_t *partialMatchLen,
                   int32_t* maxMatchLen, int32_t* maxMatchIndex) {
    *maxMatchIndex = -1;
    *maxMatchLen = 0;
    int32_t matchIndex = -1;
    int32_t ଦ୍ୱିମିକ_ସନ୍ଧାନ_Begin = 0;
    int32_t ଦ୍ୱିମିକ_ସନ୍ଧାନ_End = total_currency_ଗଣନା - 1;
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
    ପାଇଁ (int32_t index = 0; index < textLen; ++index) {
        // matchIndex saves the one with exact match till the current point.
        // [binarySearchBegin, binarySearchEnd] saves the matching range.
        matchIndex = ଦ୍ୱିମିକ_ସନ୍ଧାନ(currencyNames, index,
                                  text[index],
                                  &ଦ୍ୱିମିକ_ସନ୍ଧାନ_Begin, &ଦ୍ୱିମିକ_ସନ୍ଧାନ_End);
        ଯଦି (ଦ୍ୱିମିକ_ସନ୍ଧାନ_Begin == -1) { // did not find the range
            ଭାଙ୍ଗ;
        }
        *partialMatchLen = MAX(*partialMatchLen, index + 1);
        ଯଦି (matchIndex != -1) { 
            // find an exact match for text from text[0] to text[index] 
            // in currencyNames array.
            *maxMatchLen = index + 1;
            *maxMatchIndex = matchIndex;
        }
        ଯଦି (ଦ୍ୱିମିକ_ସନ୍ଧାନ_End - ଦ୍ୱିମିକ_ସନ୍ଧାନ_Begin < LINEAR_ସନ୍ଧାନ_THRESHOLD) {
            // linear search if within threshold.
            linear_ସନ୍ଧାନ(currencyNames, ଦ୍ୱିମିକ_ସନ୍ଧାନ_Begin, ଦ୍ୱିମିକ_ସନ୍ଧାନ_End,
                         text, textLen,
                         partialMatchLen,
                         maxMatchLen, maxMatchIndex);
            ଭାଙ୍ଗ;
        }
    }
}

//========================= currency name cache =====================
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
    ଅକ୍ଷର locale[ULOC_FULLNAME_CAPACITY];  //key
    // currency names, case insensitive
    CurrencyNameStruct* currencyNames;  //ମୂଲ୍ୟ
    int32_t total_Currency_Name_ଗଣନା;  // currency name count
    // currency symbols and ISO code, case sensitive
    CurrencyNameStruct* currencySymbols; //ମୂଲ୍ୟ
    int32_t total_Currency_ପ୍ରତୀକ_ଗଣନା;  //ଗଣନା
    // reference count.
    // reference count is set to 1 when an entry is put to cache.
    // it increases by 1 before accessing, and decreased by 1 after accessing.
    // The entry is deleted when ref count is zero, which means 
    // the entry is replaced out of cache and no process is accessing it.
    int32_t ref_ଗଣନା;
} CurrencyNameCacheEntry;


#define CURRENCY_NAME_CACHE_ଅଙ୍କ 10

// Reserve 10 cache entries.
ସ୍ଥିର CurrencyNameCacheEntry* currCache[CURRENCY_NAME_CACHE_ଅଙ୍କ] = {nullptr};
// Using an index to indicate which entry to be replaced when cache is full.
// It is a simple round-robin replacement strategy.
ସ୍ଥିର int8_t currentCacheEntryIndex = 0;

ସ୍ଥିର UMutex gCurrencyCacheMutex;

// Cache deletion
ସ୍ଥିର ଶୂନ୍ୟ
deleteCurrencyNames(CurrencyNameStruct* currencyNames, int32_t count) {
    ପାଇଁ (int32_t index = 0; index < count; ++index) {
        ଯଦି ( (currencyNames[index].flag & NEED_TO_BE_DELETED) ) {
            uprv_free(currencyNames[index].currencyName);
        }
    }
    uprv_free(currencyNames);
}


ସ୍ଥିର ଶୂନ୍ୟ
deleteCacheEntry(CurrencyNameCacheEntry* entry) {
    deleteCurrencyNames(entry->currencyNames, entry->total_Currency_Name_ଗଣନା);
    deleteCurrencyNames(entry->currencySymbols, entry->total_Currency_ପ୍ରତୀକ_ଗଣନା);
    uprv_free(entry);
}


// Cache clean up
ସ୍ଥିର UBool U_CALLCONV
currency_cache_cleanup() {
    ପାଇଁ (int32_t i = 0; i < CURRENCY_NAME_CACHE_ଅଙ୍କ; ++i) {
        ଯଦି (currCache[i]) {
            deleteCacheEntry(currCache[i]);
            currCache[i] = nullptr;
        }
    }
    ଫେରନ୍ତୁ ସତ୍ୟ;
}


/**
 * Loads the currency name data from the cache, or from resource bundles if necessary.
 * The refCount is automatically incremented.  It is the caller's responsibility
 * to decrement it when done!
 */
ସ୍ଥିର CurrencyNameCacheEntry*
getCacheEntry(ଅଚଳ ଅକ୍ଷର* locale, UErrorCode& ec) {

    int32_t total_currency_name_ଗଣନା = 0;
    CurrencyNameStruct* currencyNames = nullptr;
    int32_t total_currency_ପ୍ରତୀକ_ଗଣନା = 0;
    CurrencyNameStruct* currencySymbols = nullptr;
    CurrencyNameCacheEntry* cacheEntry = nullptr;

    umtx_lock(&gCurrencyCacheMutex);
    // in order to handle racing correctly,
    // not putting 'search' in a separate function.
    int8_t found = -1;
    ପାଇଁ (int8_t i = 0; i < CURRENCY_NAME_CACHE_ଅଙ୍କ; ++i) {
        ଯଦି (currCache[i]!= nullptr &&
            uprv_strcmp(locale, currCache[i]->locale) == 0) {
            found = i;
            ଭାଙ୍ଗ;
        }
    }
    ଯଦି (found != -1) {
        cacheEntry = currCache[found];
        ++(cacheEntry->ref_ଗଣନା);
    }
    umtx_unlock(&gCurrencyCacheMutex);
    ଯଦି (found == -1) {
        collectCurrencyNames(locale, &currencyNames, &total_currency_name_ଗଣନା, &currencySymbols, &total_currency_ପ୍ରତୀକ_ଗଣନା, ec);
        ଯଦି (U_FAILURE(ec)) {
            ଫେରନ୍ତୁ nullptr;
        }
        umtx_lock(&gCurrencyCacheMutex);
        // check again.
        ପାଇଁ (int8_t i = 0; i < CURRENCY_NAME_CACHE_ଅଙ୍କ; ++i) {
            ଯଦି (currCache[i]!= nullptr &&
                uprv_strcmp(locale, currCache[i]->locale) == 0) {
                found = i;
                ଭାଙ୍ଗ;
            }
        }
        ଯଦି (found == -1) {
            // insert new entry to 
            // currentCacheEntryIndex % CURRENCY_NAME_CACHE_NUM
            // and remove the existing entry 
            // currentCacheEntryIndex % CURRENCY_NAME_CACHE_NUM
            // from cache.
            cacheEntry = currCache[currentCacheEntryIndex];
            ଯଦି (cacheEntry) {
                --(cacheEntry->ref_ଗଣନା);
                // delete if the ref count is zero
                ଯଦି (cacheEntry->ref_ଗଣନା == 0) {
                    deleteCacheEntry(cacheEntry);
                }
            }
            cacheEntry = static_cast<CurrencyNameCacheEntry*>(uprv_malloc(ଆକାର(CurrencyNameCacheEntry)));
            ଯଦି (cacheEntry == nullptr) {
                deleteCurrencyNames(currencyNames, total_currency_name_ଗଣନା);
                deleteCurrencyNames(currencySymbols, total_currency_ପ୍ରତୀକ_ଗଣନା);
                ec = U_MEMORY_ALLOCATION_ERROR;
                ଫେରନ୍ତୁ nullptr;
            }
            currCache[currentCacheEntryIndex] = cacheEntry;
            uprv_strcpy(cacheEntry->locale, locale);
            cacheEntry->currencyNames = currencyNames;
            cacheEntry->total_Currency_Name_ଗଣନା = total_currency_name_ଗଣନା;
            cacheEntry->currencySymbols = currencySymbols;
            cacheEntry->total_Currency_ପ୍ରତୀକ_ଗଣନା = total_currency_ପ୍ରତୀକ_ଗଣନା;
            cacheEntry->ref_ଗଣନା = 2; // one for cache, one for reference
            currentCacheEntryIndex = (currentCacheEntryIndex + 1) % CURRENCY_NAME_CACHE_ଅଙ୍କ;
            ucln_common_registerCleanup(UCLN_COMMON_CURRENCY, currency_cleanup);
        } ଅନ୍ୟଥା {
            deleteCurrencyNames(currencyNames, total_currency_name_ଗଣନା);
            deleteCurrencyNames(currencySymbols, total_currency_ପ୍ରତୀକ_ଗଣନା);
            cacheEntry = currCache[found];
            ++(cacheEntry->ref_ଗଣନା);
        }
        umtx_unlock(&gCurrencyCacheMutex);
    }

    ଫେରନ୍ତୁ cacheEntry;
}

ସ୍ଥିର ଶୂନ୍ୟ releaseCacheEntry(CurrencyNameCacheEntry* cacheEntry) {
    umtx_lock(&gCurrencyCacheMutex);
    --(cacheEntry->ref_ଗଣନା);
    ଯଦି (cacheEntry->ref_ଗଣନା == 0) {  // remove
        deleteCacheEntry(cacheEntry);
    }
    umtx_unlock(&gCurrencyCacheMutex);
}

U_CAPI ଶୂନ୍ୟ
uprv_parseCurrency(ଅଚଳ ଅକ୍ଷର* locale,
                   ଅଚଳ icu::UnicodeString& text,
                   icu::ParsePosition& pos,
                   int8_t type,
                   int32_t* partialMatchLen,
                   char16_t* result,
                   UErrorCode& ec) {
    U_NAMESPACE_USE
    ଯଦି (U_FAILURE(ec)) {
        ଫେରନ୍ତୁ;
    }
    CurrencyNameCacheEntry* cacheEntry = getCacheEntry(locale, ec);
    ଯଦି (U_FAILURE(ec)) {
        ଫେରନ୍ତୁ;
    }

    int32_t total_currency_name_ଗଣନା = cacheEntry->total_Currency_Name_ଗଣନା;
    CurrencyNameStruct* currencyNames = cacheEntry->currencyNames;
    int32_t total_currency_ପ୍ରତୀକ_ଗଣନା = cacheEntry->total_Currency_ପ୍ରତୀକ_ଗଣନା;
    CurrencyNameStruct* currencySymbols = cacheEntry->currencySymbols;

    int32_t start = pos.getIndex();

    char16_t ନିବେଶ_Text[MAX_CURRENCY_NAME_LEN];
    char16_t upperText[MAX_CURRENCY_NAME_LEN];
    int32_t textLen = MIN(MAX_CURRENCY_NAME_LEN, text.length() - start);
    text.extract(start, textLen, ନିବେଶ_Text);
    UErrorCode ec1 = U_ZERO_ERROR;
    textLen = u_strToUpper(upperText, MAX_CURRENCY_NAME_LEN, ନିବେଶ_Text, textLen, locale, &ec1);

    // Make sure partialMatchLen is initialized
    *partialMatchLen = 0;

    int32_t max = 0;
    int32_t matchIndex = -1;
    // case in-sensitive comparison against currency names
    ସନ୍ଧାନ_Currency_Name(currencyNames, total_currency_name_ଗଣନା, 
                       upperText, textLen, partialMatchLen, &max, &matchIndex);

#ifdef UCURR_DEBUG
    printf("search in names, max = %d, matchIndex = %d\n", max, matchIndex);
#endif

    int32_t max_In_ପ୍ରତୀକ = 0;
    int32_t match_Index_In_ପ୍ରତୀକ = -1;
    ଯଦି (type != UCURR_LONG_NAME) {  // not name only
        // case sensitive comparison against currency symbols and ISO code.
        ସନ୍ଧାନ_Currency_Name(currencySymbols, total_currency_ପ୍ରତୀକ_ଗଣନା, 
                           ନିବେଶ_Text, textLen,
                           partialMatchLen,
                           &max_In_ପ୍ରତୀକ, &match_Index_In_ପ୍ରତୀକ);
    }

#ifdef UCURR_DEBUG
    printf("search in symbols, maxInSymbol = %d, matchIndexInSymbol = %d\n", max_In_ପ୍ରତୀକ, match_Index_In_ପ୍ରତୀକ);
    ଯଦି(match_Index_In_ପ୍ରତୀକ != -1) {
      printf("== ISO=%s\n", currencySymbols[match_Index_In_ପ୍ରତୀକ].IsoCode);
    }
#endif

    ଯଦି (max >= max_In_ପ୍ରତୀକ && matchIndex != -1) {
        u_charsToUChars(currencyNames[matchIndex].IsoCode, result, 4);
        pos.setIndex(start + max);
    } ଅନ୍ୟଥା ଯଦି (max_In_ପ୍ରତୀକ >= max && match_Index_In_ପ୍ରତୀକ != -1) {
        u_charsToUChars(currencySymbols[match_Index_In_ପ୍ରତୀକ].IsoCode, result, 4);
        pos.setIndex(start + max_In_ପ୍ରତୀକ);
    }

    // decrease reference count
    releaseCacheEntry(cacheEntry);
}

ଶୂନ୍ୟ uprv_currencyLeads(ଅଚଳ ଅକ୍ଷର* locale, icu::UnicodeSet& result, UErrorCode& ec) {
    U_NAMESPACE_USE
    ଯଦି (U_FAILURE(ec)) {
        ଫେରନ୍ତୁ;
    }
    CurrencyNameCacheEntry* cacheEntry = getCacheEntry(locale, ec);
    ଯଦି (U_FAILURE(ec)) {
        ଫେରନ୍ତୁ;
    }

    ପାଇଁ (int32_t i=0; i<cacheEntry->total_Currency_ପ୍ରତୀକ_ଗଣନା; i++) {
        ଅଚଳ CurrencyNameStruct& info = cacheEntry->currencySymbols[i];
        UChar32 cp;
        U16_GET(info.currencyName, 0, 0, info.currencyNameLen, cp);
        result.add(cp);
    }

    ପାଇଁ (int32_t i=0; i<cacheEntry->total_Currency_Name_ଗଣନା; i++) {
        ଅଚଳ CurrencyNameStruct& info = cacheEntry->currencyNames[i];
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
U_CAPI ଶୂନ୍ୟ
uprv_getStaticCurrencyName(ଅଚଳ char16_t* iso, ଅଚଳ ଅକ୍ଷର* loc,
                           icu::UnicodeString& result, UErrorCode& ec)
{
    U_NAMESPACE_USE

    int32_t len;
    ଅଚଳ char16_t* currname = ucurr_getName(iso, loc, UCURR_ପ୍ରତୀକ_NAME,
                                          nullptr /* isChoiceFormat */, &len, &ec);
    ଯଦି (U_SUCCESS(ec)) {
        result.setTo(currname, len);
    }
}

U_CAPI int32_t U_EXPORT2
ucurr_getDefaultFractionDigits(ଅଚଳ char16_t* currency, UErrorCode* ec) {
    ଫେରନ୍ତୁ ucurr_getDefaultFractionDigitsForUsage(currency,UCURR_USAGE_STANDARD,ec);
}

U_CAPI int32_t U_EXPORT2
ucurr_getDefaultFractionDigitsForUsage(ଅଚଳ char16_t* currency, ଅଚଳ UCurrencyUsage usage, UErrorCode* ec) {
    int32_t fracDigits = 0;
    ଯଦି (U_SUCCESS(*ec)) {
        ବାଛ (usage) {
            ସ୍ଥିତି UCURR_USAGE_STANDARD:
                fracDigits = (_findMetaData(currency, *ec))[0];
                ଭାଙ୍ଗ;
            ସ୍ଥିତି UCURR_USAGE_CASH:
                fracDigits = (_findMetaData(currency, *ec))[2];
                ଭାଙ୍ଗ;
            ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                *ec = U_UNSUPPORTED_ERROR;
        }
    }
    ଫେରନ୍ତୁ fracDigits;
}

U_CAPI ଦ୍ୱିଗୁଣ U_EXPORT2
ucurr_getRoundingIncrement(ଅଚଳ char16_t* currency, UErrorCode* ec) {
    ଫେରନ୍ତୁ ucurr_getRoundingIncrementForUsage(currency, UCURR_USAGE_STANDARD, ec);
}

U_CAPI ଦ୍ୱିଗୁଣ U_EXPORT2
ucurr_getRoundingIncrementForUsage(ଅଚଳ char16_t* currency, ଅଚଳ UCurrencyUsage usage, UErrorCode* ec) {
    ଦ୍ୱିଗୁଣ result = 0.0;

    ଅଚଳ int32_t *data = _findMetaData(currency, *ec);
    ଯଦି (U_SUCCESS(*ec)) {
        int32_t fracDigits;
        int32_t increment;
        ବାଛ (usage) {
            ସ୍ଥିତି UCURR_USAGE_STANDARD:
                fracDigits = data[0];
                increment = data[1];
                ଭାଙ୍ଗ;
            ସ୍ଥିତି UCURR_USAGE_CASH:
                fracDigits = data[2];
                increment = data[3];
                ଭାଙ୍ଗ;
            ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                *ec = U_UNSUPPORTED_ERROR;
                ଫେରନ୍ତୁ result;
        }

        // If the meta data is invalid, return 0.0
        ଯଦି (fracDigits < 0 || fracDigits > MAX_POW10) {
            *ec = U_INVALID_FORMAT_ERROR;
        } ଅନ୍ୟଥା {
            // A rounding value of 0 or 1 indicates no rounding.
            ଯଦି (increment >= 2) {
                // Return (increment) / 10^(fracDigits).  The only actual rounding data,
                // as of this writing, is CHF { 2, 5 }.
                result = ଦ୍ୱିଗୁଣ(increment) / POW10[fracDigits];
            }
        }
    }

    ଫେରନ୍ତୁ result;
}

U_CDECL_BEGIN

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା UCurrencyContext {
    uint32_t currType; /* UCurrCurrencyType */
    uint32_t listIdx;
} UCurrencyContext;

/*
Please keep this list in alphabetical order.
You can look at the CLDR supplemental data or ISO-4217 for the meaning of some
of these items.
ISO-4217: http://www.iso.org/iso/en/prods-services/popstds/currencycodeslist.html
*/
ସ୍ଥିର ଅଚଳ ସଂରଚନା CurrencyList {
    ଅଚଳ ଅକ୍ଷର *currency;
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

ସ୍ଥିର int32_t U_CALLCONV
ucurr_ଗଣନା_Currency_List(UEnumeration *enumerator, UErrorCode * /*pErrorCode*/) {
    UCurrencyContext *myContext = (UCurrencyContext *)(enumerator->context);
    uint32_t currType = myContext->currType;
    int32_t count = 0;

    /* Count the number of items matching the type we are looking for. */
    ପାଇଁ (int32_t idx = 0; gCurrencyList[idx].currency != nullptr; idx++) {
        ଯଦି (UCURR_MATCHES_BITMASK(gCurrencyList[idx].currType, currType)) {
            count++;
        }
    }
    ଫେରନ୍ତୁ count;
}

ସ୍ଥିର ଅଚଳ ଅକ୍ଷର* U_CALLCONV
ucurr_nextCurrencyList(UEnumeration *enumerator,
                        int32_t* resultLength,
                        UErrorCode * /*pErrorCode*/)
{
    UCurrencyContext *myContext = (UCurrencyContext *)(enumerator->context);

    /* Find the next in the list that matches the type we are looking for. */
    ଯେତେବେଳେ (myContext->listIdx < UPRV_LENGTHOF(gCurrencyList)-1) {
        ଅଚଳ ସଂରଚନା CurrencyList *currItem = &gCurrencyList[myContext->listIdx++];
        ଯଦି (UCURR_MATCHES_BITMASK(currItem->currType, myContext->currType))
        {
            ଯଦି (resultLength) {
                *resultLength = 3; /* Currency codes are only 3 chars long */
            }
            ଫେରନ୍ତୁ currItem->currency;
        }
    }
    /* We enumerated too far. */
    ଯଦି (resultLength) {
        *resultLength = 0;
    }
    ଫେରନ୍ତୁ nullptr;
}

ସ୍ଥିର ଶୂନ୍ୟ U_CALLCONV
ucurr_resetCurrencyList(UEnumeration *enumerator, UErrorCode * /*pErrorCode*/) {
    ((UCurrencyContext *)(enumerator->context))->listIdx = 0;
}

ସ୍ଥିର ଶୂନ୍ୟ U_CALLCONV
ucurr_closeCurrencyList(UEnumeration *enumerator) {
    uprv_free(enumerator->context);
    uprv_free(enumerator);
}

ସ୍ଥିର ଶୂନ୍ୟ U_CALLCONV
ucurr_createCurrencyList(UHashtable *isoCodes, UErrorCode* status){
    UErrorCode localStatus = U_ZERO_ERROR;

    // Look up the CurrencyMap element in the root bundle.
    UResourceBundle *rb = ures_openDirect(U_ICUDATA_CURR, CURRENCY_DATA, &localStatus);
    LocalUResourceBundlePointer currencyMapArray(ures_get_By_ଚାବି(rb, CURRENCY_MAP, rb, &localStatus));

    ଯଦି (U_SUCCESS(localStatus)) {
        // process each entry in currency map
        ପାଇଁ (int32_t i=0; i<ures_getSize(currencyMapArray.getAlias()); i++) {
            // get the currency resource
            LocalUResourceBundlePointer currencyArray(ures_getByIndex(currencyMapArray.getAlias(), i, nullptr, &localStatus));
            // process each currency
            ଯଦି (U_SUCCESS(localStatus)) {
                ପାଇଁ (int32_t j=0; j<ures_getSize(currencyArray.getAlias()); j++) {
                    // get the currency resource
                    LocalUResourceBundlePointer currencyRes(ures_getByIndex(currencyArray.getAlias(), j, nullptr, &localStatus));
                    IsoCodeEntry *entry = (IsoCodeEntry*)uprv_malloc(ଆକାର(IsoCodeEntry));
                    ଯଦି (entry == nullptr) {
                        *status = U_MEMORY_ALLOCATION_ERROR;
                        ଫେରନ୍ତୁ;
                    }

                    // get the ISO code
                    int32_t isoLength = 0;
                    LocalUResourceBundlePointer idRes(ures_get_By_ଚାବି(currencyRes.getAlias(), "id", nullptr, &localStatus));
                    ଯଦି (idRes.isNull()) {
                        ଜାରି;
                    }
                    ଅଚଳ char16_t *isoCode = ures_getString(idRes.getAlias(), &isoLength, &localStatus);

                    // get from date
                    UDate fromDate = U_DATE_MIN;
                    LocalUResourceBundlePointer fromRes(ures_get_By_ଚାବି(currencyRes.getAlias(), "from", nullptr, &localStatus));

                    ଯଦି (U_SUCCESS(localStatus)) {
                        int32_t fromLength = 0;
                        ଅଚଳ int32_t *fromArray = ures_getIntVector(fromRes.getAlias(), &fromLength, &localStatus);
                        int64_t currDate64 = ((uint64_t)fromArray[0]) << 32;
                        currDate64 |= ((int64_t)fromArray[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                        fromDate = (UDate)currDate64;
                    }

                    // get to date
                    UDate toDate = U_DATE_MAX;
                    localStatus = U_ZERO_ERROR;
                    LocalUResourceBundlePointer toRes(ures_get_By_ଚାବି(currencyRes.getAlias(), "to", nullptr, &localStatus));

                    ଯଦି (U_SUCCESS(localStatus)) {
                        int32_t toLength = 0;
                        ଅଚଳ int32_t *toArray = ures_getIntVector(toRes.getAlias(), &toLength, &localStatus);
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
            } ଅନ୍ୟଥା {
                *status = localStatus;
            }
        }
    } ଅନ୍ୟଥା {
        *status = localStatus;
    }
}

ସ୍ଥିର ଅଚଳ UEnumeration gEnumCurrencyList = {
    nullptr,
    nullptr,
    ucurr_closeCurrencyList,
    ucurr_ଗଣନା_Currency_List,
    uenum_unextDefault,
    ucurr_nextCurrencyList,
    ucurr_resetCurrencyList
};
U_CDECL_END


ସ୍ଥିର ଶୂନ୍ୟ U_CALLCONV initIsoCodes(UErrorCode &status) {
    U_ASSERT(gIsoCodes == nullptr);
    ucln_common_registerCleanup(UCLN_COMMON_CURRENCY, currency_cleanup);

    LocalUHashtablePointer isoCodes(uhash_open(uhash_hashUChars, uhash_compareUChars, nullptr, &status));
    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ;
    }
    uhash_set_ମୂଲ୍ୟ_Deleter(isoCodes.getAlias(), deleteIsoCodeEntry);

    ucurr_createCurrencyList(isoCodes.getAlias(), &status);
    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ;
    }
    gIsoCodes = isoCodes.orphan();  // Note: gIsoCodes is const. Once set up here it is never altered,
                                    //       and read only access is safe without synchronization.
}

ସ୍ଥିର ଶୂନ୍ୟ populateCurrSymbolsEquiv(icu::Hashtable *hash, UErrorCode &status) {
    ଯଦି (U_FAILURE(status)) { ଫେରନ୍ତୁ; }
    ପାଇଁ (ଅଚଳ auto& entry : unisets::kCurrencyEntries) {
        UnicodeString exemplar(entry.exemplar);
        ଅଚଳ UnicodeSet* set = unisets::get(entry.ଚାବି);
        ଯଦି (set == nullptr) { ଫେରନ୍ତୁ; }
        UnicodeSetIterator it(*set);
        ଯେତେବେଳେ (it.next()) {
            UnicodeString value = it.getString();
            ଯଦି (value == exemplar) {
                // No need to mark the exemplar character as an equivalent
                ଜାରି;
            }
            makeEquivalent(exemplar, value, hash, status);
            ଯଦି (U_FAILURE(status)) { ଫେରନ୍ତୁ; }
        }
    }
}

ସ୍ଥିର ଶୂନ୍ୟ U_CALLCONV initCurrSymbolsEquiv() {
    U_ASSERT(gCurrSymbolsEquiv == nullptr);
    UErrorCode status = U_ZERO_ERROR;
    ucln_common_registerCleanup(UCLN_COMMON_CURRENCY, currency_cleanup);
    icu::Hashtable *temp = ନୂତନ icu::Hashtable(status);
    ଯଦି (temp == nullptr) {
        ଫେରନ୍ତୁ;
    }
    ଯଦି (U_FAILURE(status)) {
        delete temp;
        ଫେରନ୍ତୁ;
    }
    temp->set_ମୂଲ୍ୟ_Deleter(deleteUnicode);
    populateCurrSymbolsEquiv(temp, status);
    ଯଦି (U_FAILURE(status)) {
        delete temp;
        ଫେରନ୍ତୁ;
    }
    gCurrSymbolsEquiv = temp;
}

U_CAPI UBool U_EXPORT2
ucurr_isAvailable(ଅଚଳ char16_t* isoCode, UDate from, UDate to, UErrorCode* eErrorCode) {
    umtx_initOnce(gIsoCodesInitOnce, &initIsoCodes, *eErrorCode);
    ଯଦି (U_FAILURE(*eErrorCode)) {
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }

    IsoCodeEntry* result = (IsoCodeEntry *) uhash_get(gIsoCodes, isoCode);
    ଯଦି (result == nullptr) {
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    } ଅନ୍ୟଥା ଯଦି (from > to) {
        *eErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    } ଅନ୍ୟଥା ଯଦି  ((from > result->to) || (to < result->from)) {
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    ଫେରନ୍ତୁ ସତ୍ୟ;
}

ସ୍ଥିର ଅଚଳ icu::Hashtable* getCurrSymbolsEquiv() {
    umtx_initOnce(gCurrSymbolsEquivInitOnce, &initCurrSymbolsEquiv);
    ଫେରନ୍ତୁ gCurrSymbolsEquiv;
}

U_CAPI UEnumeration * U_EXPORT2
ucurr_openISOCurrencies(uint32_t currType, UErrorCode *pErrorCode) {
    UEnumeration *myEnum = nullptr;
    UCurrencyContext *myContext;

    myEnum = (UEnumeration*)uprv_malloc(ଆକାର(UEnumeration));
    ଯଦି (myEnum == nullptr) {
        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
        ଫେରନ୍ତୁ nullptr;
    }
    uprv_memcpy(myEnum, &gEnumCurrencyList, ଆକାର(UEnumeration));
    myContext = (UCurrencyContext*)uprv_malloc(ଆକାର(UCurrencyContext));
    ଯଦି (myContext == nullptr) {
        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
        uprv_free(myEnum);
        ଫେରନ୍ତୁ nullptr;
    }
    myContext->currType = currType;
    myContext->listIdx = 0;
    myEnum->context = myContext;
    ଫେରନ୍ତୁ myEnum;
}

U_CAPI int32_t U_EXPORT2
ucurr_ଗଣନା_Currencies(ଅଚଳ ଅକ୍ଷର* locale, 
                 UDate date, 
                 UErrorCode* ec)
{
    int32_t curr_ଗଣନା = 0;

    ଯଦି (ec != nullptr && U_SUCCESS(*ec)) 
    {
        // local variables
        UErrorCode localStatus = U_ZERO_ERROR;

        // get country or country_variant in `id'
        CharString id = idForLocale(locale, ec);

        ଯଦି (U_FAILURE(*ec))
        {
            ଫେରନ୍ତୁ 0;
        }

        // Remove variants, which is only needed for registration.
        ଅକ୍ଷର *idDelim = strchr(id.data(), VAR_DELIM);
        ଯଦି (idDelim)
        {
            id.truncate(idDelim - id.data());
        }

        // Look up the CurrencyMap element in the root bundle.
        UResourceBundle *rb = ures_openDirect(U_ICUDATA_CURR, CURRENCY_DATA, &localStatus);
        UResourceBundle *cm = ures_get_By_ଚାବି(rb, CURRENCY_MAP, rb, &localStatus);

        // Using the id derived from the local, get the currency data
        LocalUResourceBundlePointer countryArray(ures_get_By_ଚାବି(rb, id.data(), cm, &localStatus));

        // process each currency to see which one is valid for the given date
        ଯଦି (U_SUCCESS(localStatus))
        {
            ପାଇଁ (int32_t i=0; i<ures_getSize(countryArray.getAlias()); i++)
            {
                // get the currency resource
                LocalUResourceBundlePointer currencyRes(ures_getByIndex(countryArray.getAlias(), i, nullptr, &localStatus));

                // get the from date
                int32_t fromLength = 0;
                LocalUResourceBundlePointer fromRes(ures_get_By_ଚାବି(currencyRes.getAlias(), "from", nullptr, &localStatus));
                ଅଚଳ int32_t *fromArray = ures_getIntVector(fromRes.getAlias(), &fromLength, &localStatus);

                int64_t currDate64 = (int64_t)((uint64_t)(fromArray[0]) << 32);
                currDate64 |= ((int64_t)fromArray[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                UDate fromDate = (UDate)currDate64;

                ଯଦି (ures_getSize(currencyRes.getAlias())> 2)
                {
                    int32_t toLength = 0;
                    LocalUResourceBundlePointer toRes(ures_get_By_ଚାବି(currencyRes.getAlias(), "to", nullptr, &localStatus));
                    ଅଚଳ int32_t *toArray = ures_getIntVector(toRes.getAlias(), &toLength, &localStatus);

                    currDate64 = (int64_t)toArray[0] << 32;
                    currDate64 |= ((int64_t)toArray[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                    UDate toDate = (UDate)currDate64;

                    ଯଦି ((fromDate <= date) && (date < toDate))
                    {
                        curr_ଗଣନା++;
                    }
                }
                ଅନ୍ୟଥା
                {
                    ଯଦି (fromDate <= date)
                    {
                        curr_ଗଣନା++;
                    }
                }
            } // end For loop
        } // end if (U_SUCCESS(localStatus))

        // Check for errors
        ଯଦି (*ec == U_ZERO_ERROR || localStatus != U_ZERO_ERROR)
        {
            // There is nothing to fallback to. 
            // Report the failure/warning if possible.
            *ec = localStatus;
        }

        ଯଦି (U_SUCCESS(*ec))
        {
            // no errors
            ଫେରନ୍ତୁ curr_ଗଣନା;
        }
    }

    // If we got here, either error code is invalid or
    // some argument passed is no good.
    ଫେରନ୍ତୁ 0;
}

U_CAPI int32_t U_EXPORT2 
ucurr_forLocaleAndDate(ଅଚଳ ଅକ୍ଷର* locale, 
                UDate date, 
                int32_t index,
                char16_t* buff,
                int32_t buffCapacity, 
                UErrorCode* ec)
{
    int32_t resLen = 0;
	int32_t currIndex = 0;
    ଅଚଳ char16_t* s = nullptr;

    ଯଦି (ec != nullptr && U_SUCCESS(*ec))
    {
        // check the arguments passed
        ଯଦି ((buff && buffCapacity) || !buffCapacity )
        {
            // local variables
            UErrorCode localStatus = U_ZERO_ERROR;

            // get country or country_variant in `id'
            CharString id = idForLocale(locale, ec);
            ଯଦି (U_FAILURE(*ec))
            {
                ଫେରନ୍ତୁ 0;
            }

            // Remove variants, which is only needed for registration.
            ଅକ୍ଷର *idDelim = strchr(id.data(), VAR_DELIM);
            ଯଦି (idDelim)
            {
                id.truncate(idDelim - id.data());
            }

            // Look up the CurrencyMap element in the root bundle.
            UResourceBundle *rb = ures_openDirect(U_ICUDATA_CURR, CURRENCY_DATA, &localStatus);
            UResourceBundle *cm = ures_get_By_ଚାବି(rb, CURRENCY_MAP, rb, &localStatus);

            // Using the id derived from the local, get the currency data
            LocalUResourceBundlePointer countryArray(ures_get_By_ଚାବି(rb, id.data(), cm, &localStatus));

            // process each currency to see which one is valid for the given date
            ତାର୍କିକ matchFound = ମିଥ୍ୟା;
            ଯଦି (U_SUCCESS(localStatus))
            {
                ଯଦି ((index <= 0) || (index> ures_getSize(countryArray.getAlias())))
                {
                    // requested index is out of bounds
                    ଫେରନ୍ତୁ 0;
                }

                ପାଇଁ (int32_t i=0; i<ures_getSize(countryArray.getAlias()); i++)
                {
                    // get the currency resource
                    LocalUResourceBundlePointer currencyRes(ures_getByIndex(countryArray.getAlias(), i, nullptr, &localStatus));
                    s = ures_get_String_By_ଚାବି(currencyRes.getAlias(), "id", &resLen, &localStatus);

                    // get the from date
                    int32_t fromLength = 0;
                    LocalUResourceBundlePointer fromRes(ures_get_By_ଚାବି(currencyRes.getAlias(), "from", nullptr, &localStatus));
                    ଅଚଳ int32_t *fromArray = ures_getIntVector(fromRes.getAlias(), &fromLength, &localStatus);

                    int64_t currDate64 = (int64_t)((uint64_t)fromArray[0] << 32);
                    currDate64 |= ((int64_t)fromArray[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                    UDate fromDate = (UDate)currDate64;

                    ଯଦି (ures_getSize(currencyRes.getAlias()) > 2)
                    {
                        int32_t toLength = 0;
                        LocalUResourceBundlePointer toRes(ures_get_By_ଚାବି(currencyRes.getAlias(), "to", nullptr, &localStatus));
                        ଅଚଳ int32_t *toArray = ures_getIntVector(toRes.getAlias(), &toLength, &localStatus);

                        currDate64 = (int64_t)toArray[0] << 32;
                        currDate64 |= ((int64_t)toArray[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                        UDate toDate = (UDate)currDate64;

                        ଯଦି ((fromDate <= date) && (date < toDate))
                        {
                            currIndex++;
                            ଯଦି (currIndex == index)
                            {
                                matchFound = ସତ୍ୟ;
                            }
                        }
                    }
                    ଅନ୍ୟଥା
                    {
                        ଯଦି (fromDate <= date)
                        {
                            currIndex++;
                            ଯଦି (currIndex == index)
                            {
                                matchFound = ସତ୍ୟ;
                            }
                        }
                    }
                    // check for loop exit
                    ଯଦି (matchFound)
                    {
                        ଭାଙ୍ଗ;
                    }

                } // end For loop
            }

            // Check for errors
            ଯଦି (*ec == U_ZERO_ERROR || localStatus != U_ZERO_ERROR)
            {
                // There is nothing to fallback to. 
                // Report the failure/warning if possible.
                *ec = localStatus;
            }

            ଯଦି (U_SUCCESS(*ec))
            {
                // no errors
                ଯଦି((buffCapacity> resLen) && matchFound)
                {
                    // write out the currency value
                    u_strcpy(buff, s);
                }
                ଅନ୍ୟଥା
                {
                    ଫେରନ୍ତୁ 0;
                }
            }

            // return null terminated currency string
            ଫେରନ୍ତୁ u_terminateUChars(buff, buffCapacity, resLen, ec);
        }
        ଅନ୍ୟଥା
        {
            // illegal argument encountered
            *ec = U_ILLEGAL_ARGUMENT_ERROR;
        }

    }

    // If we got here, either error code is invalid or
    // some argument passed is no good.
    ଫେରନ୍ତୁ resLen;
}

ସ୍ଥିର ଅଚଳ UEnumeration default_Keyword_ମୂଲ୍ୟଗୁଡ଼ିକ = {
    nullptr,
    nullptr,
    ulist_close_keyword_ମୂଲ୍ୟଗୁଡ଼ିକ_iterator,
    ulist_ଗଣନା_keyword_ମୂଲ୍ୟଗୁଡ଼ିକ,
    uenum_unextDefault,
    ulist_next_keyword_ମୂଲ୍ୟ, 
    ulist_reset_keyword_ମୂଲ୍ୟଗୁଡ଼ିକ_iterator
};

U_CAPI UEnumeration *U_EXPORT2 ucurr_get_Keyword_ମୂଲ୍ୟଗୁଡ଼ିକ_For_Locale(ଅଚଳ ଅକ୍ଷର *ଚାବି, ଅଚଳ ଅକ୍ଷର *locale, UBool commonlyUsed, UErrorCode* status) {
    // Resolve region
    CharString prefRegion = ulocimp_getRegionForSupplementalData(locale, ସତ୍ୟ, *status);

    // Read value from supplementalData
    UList *ମୂଲ୍ୟଗୁଡ଼ିକ = ulist_createEmptyList(status);
    UList *other_ମୂଲ୍ୟଗୁଡ଼ିକ = ulist_createEmptyList(status);
    UEnumeration *en = (UEnumeration *)uprv_malloc(ଆକାର(UEnumeration));
    ଯଦି (U_FAILURE(*status) || en == nullptr) {
        ଯଦି (en == nullptr) {
            *status = U_MEMORY_ALLOCATION_ERROR;
        } ଅନ୍ୟଥା {
            uprv_free(en);
        }
        ulist_deleteList(ମୂଲ୍ୟଗୁଡ଼ିକ);
        ulist_deleteList(other_ମୂଲ୍ୟଗୁଡ଼ିକ);
        ଫେରନ୍ତୁ nullptr;
    }
    memcpy(en, &default_Keyword_ମୂଲ୍ୟଗୁଡ଼ିକ, ଆକାର(UEnumeration));
    en->context = ମୂଲ୍ୟଗୁଡ଼ିକ;
    
    UResourceBundle* rb = ures_openDirect(U_ICUDATA_CURR, "supplementalData", status);
    LocalUResourceBundlePointer bundle(ures_get_By_ଚାବି(rb, "CurrencyMap", rb, status));
    ଗଦା_UResource_Bundle bundlekey, regbndl, curbndl, to;
    
    ଯେତେବେଳେ (U_SUCCESS(*status) && ures_hasNext(bundle.getAlias())) {
        ures_getNextResource(bundle.getAlias(), bundlekey.getAlias(), status);
        ଯଦି (U_FAILURE(*status)) {
            ଭାଙ୍ଗ;
        }
        ଅଚଳ ଅକ୍ଷର *region = ures_get_ଚାବି(bundlekey.getAlias());
        UBool isPrefRegion = prefRegion == region;
        ଯଦି (!isPrefRegion && commonlyUsed) {
            // With commonlyUsed=true, we do not put
            // currencies for other regions in the
            // result list.
            ଜାରି;
        }
        ures_get_By_ଚାବି(bundle.getAlias(), region, regbndl.getAlias(), status);
        ଯଦି (U_FAILURE(*status)) {
            ଭାଙ୍ଗ;
        }
        ଯେତେବେଳେ (U_SUCCESS(*status) && ures_hasNext(regbndl.getAlias())) {
            ures_getNextResource(regbndl.getAlias(), curbndl.getAlias(), status);
            ଯଦି (ures_getType(curbndl.getAlias()) != URES_TABLE) {
                // Currently, an empty ARRAY is mixed in.
                ଜାରି;
            }
            ଅକ୍ଷର *curID = (ଅକ୍ଷର *)uprv_malloc(ଆକାର(ଅକ୍ଷର) * ULOC_KEYWORDS_CAPACITY);
            ଯଦି (curID == nullptr) {
                *status = U_MEMORY_ALLOCATION_ERROR;
                ଭାଙ୍ଗ;
            }
            int32_t curIDLength = ULOC_KEYWORDS_CAPACITY;

#ଯଦି U_CHARSET_FAMILY==U_ASCII_FAMILY
            ures_get_UTF8_String_By_ଚାବି(curbndl.getAlias(), "id", curID, &curIDLength, ସତ୍ୟ, status);
            /* optimize - use the utf-8 string */
#ଅନ୍ୟଥା
            {
                       ଅଚଳ char16_t* defString = ures_get_String_By_ଚାବି(curbndl.getAlias(), "id", &curIDLength, status);
                       ଯଦି(U_SUCCESS(*status)) {
			   ଯଦି(curIDLength+1 > ULOC_KEYWORDS_CAPACITY) {
				*status = U_BUFFER_OVERFLOW_ERROR;
			   } ଅନ୍ୟଥା {
                           	u_UCharsToChars(defString, curID, curIDLength+1);
			   }
                       }
            }
#endif	

            ଯଦି (U_FAILURE(*status)) {
                ଭାଙ୍ଗ;
            }
            UBool hasTo = ମିଥ୍ୟା;
            ures_get_By_ଚାବି(curbndl.getAlias(), "to", to.getAlias(), status);
            ଯଦି (U_FAILURE(*status)) {
                // Do nothing here...
                *status = U_ZERO_ERROR;
            } ଅନ୍ୟଥା {
                hasTo = ସତ୍ୟ;
            }
            ଯଦି (isPrefRegion && !hasTo && !ulist_containsString(ମୂଲ୍ୟଗୁଡ଼ିକ, curID, (int32_t)uprv_strlen(curID))) {
                // Currently active currency for the target country
                ulist_addItemEndList(ମୂଲ୍ୟଗୁଡ଼ିକ, curID, ସତ୍ୟ, status);
            } ଅନ୍ୟଥା ଯଦି (!ulist_containsString(other_ମୂଲ୍ୟଗୁଡ଼ିକ, curID, (int32_t)uprv_strlen(curID)) && !commonlyUsed) {
                ulist_addItemEndList(other_ମୂଲ୍ୟଗୁଡ଼ିକ, curID, ସତ୍ୟ, status);
            } ଅନ୍ୟଥା {
                uprv_free(curID);
            }
        }
        
    }
    ଯଦି (U_SUCCESS(*status)) {
        ଯଦି (commonlyUsed) {
            ଯଦି (ulist_getListSize(ମୂଲ୍ୟଗୁଡ଼ିକ) == 0) {
                // This could happen if no valid region is supplied in the input
                // locale. In this case, we use the CLDR's default.
                uenum_close(en);
                en = ucurr_get_Keyword_ମୂଲ୍ୟଗୁଡ଼ିକ_For_Locale(ଚାବି, "und", ସତ୍ୟ, status);
            }
        } ଅନ୍ୟଥା {
            // Consolidate the list
            ଅକ୍ଷର *value = nullptr;
            ulist_resetList(other_ମୂଲ୍ୟଗୁଡ଼ିକ);
            ଯେତେବେଳେ ((value = (ଅକ୍ଷର *)ulist_getNext(other_ମୂଲ୍ୟଗୁଡ଼ିକ)) != nullptr) {
                ଯଦି (!ulist_containsString(ମୂଲ୍ୟଗୁଡ଼ିକ, value, (int32_t)uprv_strlen(value))) {
                    ଅକ୍ଷର *tmp_ମୂଲ୍ୟ = (ଅକ୍ଷର *)uprv_malloc(ଆକାର(ଅକ୍ଷର) * ULOC_KEYWORDS_CAPACITY);
                    ଯଦି (tmp_ମୂଲ୍ୟ == nullptr) {
                        *status = U_MEMORY_ALLOCATION_ERROR;
                        ଭାଙ୍ଗ;
                    }
                    uprv_memcpy(tmp_ମୂଲ୍ୟ, value, uprv_strlen(value) + 1);
                    ulist_addItemEndList(ମୂଲ୍ୟଗୁଡ଼ିକ, tmp_ମୂଲ୍ୟ, ସତ୍ୟ, status);
                    ଯଦି (U_FAILURE(*status)) {
                        ଭାଙ୍ଗ;
                    }
                }
            }
        }
        ulist_resetList((UList *)(en->context));
    } ଅନ୍ୟଥା {
        ulist_deleteList(ମୂଲ୍ୟଗୁଡ଼ିକ);
        uprv_free(en);
        ମୂଲ୍ୟଗୁଡ଼ିକ = nullptr;
        en = nullptr;
    }
    ulist_deleteList(other_ମୂଲ୍ୟଗୁଡ଼ିକ);
    ଫେରନ୍ତୁ en;
}


U_CAPI int32_t U_EXPORT2
ucurr_getNumericCode(ଅଚଳ char16_t* currency) {
    int32_t code = 0;
    ଯଦି (currency && u_strlen(currency) == ISO_CURRENCY_CODE_LENGTH) {
        UErrorCode status = U_ZERO_ERROR;

        UResourceBundle* bundle = ures_openDirect(nullptr, "currencyNumericCodes", &status);
        LocalUResourceBundlePointer codeMap(ures_get_By_ଚାବି(bundle, "codeMap", bundle, &status));
        ଯଦି (U_SUCCESS(status)) {
            ଅକ୍ଷର alphaCode[ISO_CURRENCY_CODE_LENGTH+1];
            myUCharsToChars(alphaCode, currency);
            T_CString_toUpperCase(alphaCode);
            ures_get_By_ଚାବି(codeMap.getAlias(), alphaCode, codeMap.getAlias(), &status);
            ପୂର୍ଣ୍ଣସଂଖ୍ୟା tmpCode = ures_getInt(codeMap.getAlias(), &status);
            ଯଦି (U_SUCCESS(status)) {
                code = tmpCode;
            }
        }
    }
    ଫେରନ୍ତୁ code;
}
#endif /* #if !UCONFIG_NO_FORMATTING */

//eof
