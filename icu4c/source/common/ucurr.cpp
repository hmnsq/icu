// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
* Copyright (c) 2002-2016, International Business Machines
* Corporation and others.  All Rights Reserved.
**********************************************************************
*/

#അന്തര്ഭാവ "unicode/utypes.h"

#എങ്കിൽ !UCONFIG_NO_FORMATTING

#അന്തര്ഭാവ <utility>

#അന്തര്ഭാവ "unicode/ucurr.h"
#അന്തര്ഭാവ "unicode/locid.h"
#അന്തര്ഭാവ "unicode/ures.h"
#അന്തര്ഭാവ "unicode/ustring.h"
#അന്തര്ഭാവ "unicode/parsepos.h"
#അന്തര്ഭാവ "unicode/uniset.h"
#അന്തര്ഭാവ "unicode/usetiter.h"
#അന്തര്ഭാവ "unicode/utf16.h"
#അന്തര്ഭാവ "ustr_imp.h"
#അന്തര്ഭാവ "charstr.h"
#അന്തര്ഭാവ "cmemory.h"
#അന്തര്ഭാവ "cstring.h"
#അന്തര്ഭാവ "static_unicode_sets.h"
#അന്തര്ഭാവ "uassert.h"
#അന്തര്ഭാവ "umutex.h"
#അന്തര്ഭാവ "ucln_cmn.h"
#അന്തര്ഭാവ "uenumimp.h"
#അന്തര്ഭാവ "uhash.h"
#അന്തര്ഭാവ "hash.h"
#അന്തര്ഭാവ "uinvchar.h"
#അന്തര്ഭാവ "uresimp.h"
#അന്തര്ഭാവ "ulist.h"
#അന്തര്ഭാവ "uresimp.h"
#അന്തര്ഭാവ "ureslocs.h"
#അന്തര്ഭാവ "ulocimp.h"

using നാമാകാശ icu;

//#define UCURR_DEBUG_EQUIV 1
#ifdef UCURR_DEBUG_EQUIV
#അന്തര്ഭാവ "stdio.h"
#endif
//#define UCURR_DEBUG 1
#ifdef UCURR_DEBUG
#അന്തര്ഭാവ "stdio.h"
#endif

തരനിർവചനം ഘടന IsoCodeEntry {
    സ്ഥിരം char16_t *isoCode; /* const because it's a reference to a resource bundle string. */
    UDate from;
    UDate to;
} IsoCodeEntry;

//------------------------------------------------------------
// Constants

// Default currency meta data of last resort.  We try to use the
// defaults encoded in the meta data resource bundle.  If there is a
// configuration/build error and these are not available, we use these
// hard-coded defaults (which should be identical).
സ്ഥിതം സ്ഥിരം int32_t LAST_RESORT_DATA[] = { 2, 0, 2, 0 };

// POW10[i] = 10^i, i=0..MAX_POW10
സ്ഥിതം സ്ഥിരം int32_t POW10[] = { 1, 10, 100, 1000, 10000, 100000,
                                 1000000, 10000000, 100000000, 1000000000 };

സ്ഥിതം സ്ഥിരം int32_t MAX_POW10 = UPRV_LENGTHOF(POW10) - 1;

#define ISO_CURRENCY_CODE_LENGTH 3

//------------------------------------------------------------
// Resource tags
//

സ്ഥിതം സ്ഥിരം അക്ഷരം CURRENCY_DATA[] = "supplementalData";
// Tag for meta-data, in root.
സ്ഥിതം സ്ഥിരം അക്ഷരം CURRENCY_META[] = "CurrencyMeta";

// Tag for map from countries to currencies, in root.
സ്ഥിതം സ്ഥിരം അക്ഷരം CURRENCY_MAP[] = "CurrencyMap";

// Tag for default meta-data, in CURRENCY_META
സ്ഥിതം സ്ഥിരം അക്ഷരം DEFAULT_META[] = "DEFAULT";

// Variant delimiter
സ്ഥിതം സ്ഥിരം അക്ഷരം VAR_DELIM = '_';

// Tag for localized display names (symbols) of currencies
സ്ഥിതം സ്ഥിരം അക്ഷരം CURRENCIES[] = "Currencies";
സ്ഥിതം സ്ഥിരം അക്ഷരം CURRENCIES_NARROW[] = "Currencies%narrow";
സ്ഥിതം സ്ഥിരം അക്ഷരം CURRENCIES_FORMAL[] = "Currencies%formal";
സ്ഥിതം സ്ഥിരം അക്ഷരം CURRENCIES_VARIANT[] = "Currencies%variant";
സ്ഥിതം സ്ഥിരം അക്ഷരം CURRENCYPLURALS[] = "CurrencyPlurals";

// ISO codes mapping table
സ്ഥിതം സ്ഥിരം UHashtable* gIsoCodes = nullptr;
സ്ഥിതം icu::UInitOnce gIsoCodesInitOnce {};

// Currency symbol equivalances
സ്ഥിതം സ്ഥിരം icu::Hashtable* gCurrSymbolsEquiv = nullptr;
സ്ഥിതം icu::UInitOnce gCurrSymbolsEquivInitOnce {};

U_NAMESPACE_BEGIN

// EquivIterator iterates over all strings that are equivalent to a given
// string, s. Note that EquivIterator will never yield s itself.
വർഗ്ഗം EquivIterator : പൊതു icu::UMemory {
പൊതു:
    // Constructor. hash stores the equivalence relationships; s is the string
    // for which we find equivalent strings.
    വരിയിൽ EquivIterator(സ്ഥിരം icu::Hashtable& hash, സ്ഥിരം icu::UnicodeString& s)
        : _hash(hash) { 
        _start = _current = &s;
    }
    വരിയിൽ ~EquivIterator() { }

    // next returns the next equivalent string or nullptr if there are no more.
    // If s has no equivalent strings, next returns nullptr on the first call.
    സ്ഥിരം icu::UnicodeString *next();
സ്വകാര്യ:
    സ്ഥിരം icu::Hashtable& _hash;
    സ്ഥിരം icu::UnicodeString* _start;
    സ്ഥിരം icu::UnicodeString* _current;
};

സ്ഥിരം icu::UnicodeString *
EquivIterator::next() {
    സ്ഥിരം icu::UnicodeString* _next = static_cast<സ്ഥിരം icu::UnicodeString*>(_hash.get(*_current));
    എങ്കിൽ (_next == nullptr) {
        U_ASSERT(_current == _start);
        തിരികെ nullptr;
    }
    എങ്കിൽ (*_next == *_start) {
        തിരികെ nullptr;
    }
    _current = _next;
    തിരികെ _next;
}

U_NAMESPACE_END

// makeEquivalent makes lhs and rhs equivalent by updating the equivalence
// relations in hash accordingly.
സ്ഥിതം ശൂന്യം makeEquivalent(
    സ്ഥിരം icu::UnicodeString &lhs,
    സ്ഥിരം icu::UnicodeString &rhs,
    icu::Hashtable* hash, UErrorCode &status) {
    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ;
    }
    എങ്കിൽ (lhs == rhs) {
        // already equivalent
        തിരികെ;
    }
    icu::EquivIterator ഇടത്_Iter(*hash, lhs);
    icu::EquivIterator വലത്_Iter(*hash, rhs);
    സ്ഥിരം icu::UnicodeString *first_ഇടത് = ഇടത്_Iter.next();
    സ്ഥിരം icu::UnicodeString *first_വലത് = വലത്_Iter.next();
    സ്ഥിരം icu::UnicodeString *next_ഇടത് = first_ഇടത്;
    സ്ഥിരം icu::UnicodeString *next_വലത് = first_വലത്;
    ആകുമ്പോൾ (next_ഇടത് != nullptr && next_വലത് != nullptr) {
        എങ്കിൽ (*next_ഇടത് == rhs || *next_വലത് == lhs) {
            // Already equivalent
            തിരികെ;
        }
        next_ഇടത് = ഇടത്_Iter.next();
        next_വലത് = വലത്_Iter.next();
    }
    // Not equivalent. Must join.
    icu::UnicodeString *new_First_ഇടത്;
    icu::UnicodeString *new_First_വലത്;
    എങ്കിൽ (first_വലത് == nullptr && first_ഇടത് == nullptr) {
        // Neither lhs or rhs belong to an equivalence circle, so we form
        // a new equivalnce circle of just lhs and rhs.
        new_First_ഇടത് = പുതിയ icu::UnicodeString(rhs);
        new_First_വലത് = പുതിയ icu::UnicodeString(lhs);
    } അല്ലെങ്കിൽ എങ്കിൽ (first_വലത് == nullptr) {
        // lhs belongs to an equivalence circle, but rhs does not, so we link
        // rhs into lhs' circle.
        new_First_ഇടത് = പുതിയ icu::UnicodeString(rhs);
        new_First_വലത് = പുതിയ icu::UnicodeString(*first_ഇടത്);
    } അല്ലെങ്കിൽ എങ്കിൽ (first_ഇടത് == nullptr) {
        // rhs belongs to an equivlance circle, but lhs does not, so we link
        // lhs into rhs' circle.
        new_First_ഇടത് = പുതിയ icu::UnicodeString(*first_വലത്);
        new_First_വലത് = പുതിയ icu::UnicodeString(lhs);
    } അല്ലെങ്കിൽ {
        // Both lhs and rhs belong to different equivalnce circles. We link
        // them together to form one single, larger equivalnce circle.
        new_First_ഇടത് = പുതിയ icu::UnicodeString(*first_വലത്);
        new_First_വലത് = പുതിയ icu::UnicodeString(*first_ഇടത്);
    }
    എങ്കിൽ (new_First_ഇടത് == nullptr || new_First_വലത് == nullptr) {
        delete new_First_ഇടത്;
        delete new_First_വലത്;
        status = U_MEMORY_ALLOCATION_ERROR;
        തിരികെ;
    }
    hash->put(lhs, (ശൂന്യം *) new_First_ഇടത്, status);
    hash->put(rhs, (ശൂന്യം *) new_First_വലത്, status);
}

// countEquivalent counts how many strings are equivalent to s.
// hash stores all the equivalnce relations.
// countEquivalent does not include s itself in the count.
സ്ഥിതം int32_t എണ്ണം_Equivalent(സ്ഥിരം icu::Hashtable &hash, സ്ഥിരം icu::UnicodeString &s) {
    int32_t result = 0;
    icu::EquivIterator iter(hash, s);
    ആകുമ്പോൾ (iter.next() != nullptr) {
        ++result;
    }
#ifdef UCURR_DEBUG_EQUIV
 {
   അക്ഷരം tmp[200];
   s.extract(0,s.length(),tmp, "UTF-8");
   printf("CountEquivalent('%s') = %d\n", tmp, result);
 }
#endif
    തിരികെ result;
}

സ്ഥിതം സ്ഥിരം icu::Hashtable* getCurrSymbolsEquiv();

//------------------------------------------------------------
// Code

/**
 * Cleanup callback func
 */
സ്ഥിതം UBool U_CALLCONV 
isoCodes_cleanup()
{
    എങ്കിൽ (gIsoCodes != nullptr) {
        uhash_close(const_cast<UHashtable *>(gIsoCodes));
        gIsoCodes = nullptr;
    }
    gIsoCodesInitOnce.reset();
    തിരികെ സത്യം;
}

/**
 * Cleanup callback func
 */
സ്ഥിതം UBool U_CALLCONV 
currSymbolsEquiv_cleanup()
{
    delete const_cast<icu::Hashtable *>(gCurrSymbolsEquiv);
    gCurrSymbolsEquiv = nullptr;
    gCurrSymbolsEquivInitOnce.reset();
    തിരികെ സത്യം;
}

/**
 * Deleter for IsoCodeEntry
 */
സ്ഥിതം ശൂന്യം U_CALLCONV
deleteIsoCodeEntry(ശൂന്യം *obj) {
    IsoCodeEntry* entry = static_cast<IsoCodeEntry*>(obj);
    uprv_free(entry);
}

/**
 * Deleter for gCurrSymbolsEquiv.
 */
സ്ഥിതം ശൂന്യം U_CALLCONV
deleteUnicode(ശൂന്യം *obj) {
    icu::UnicodeString* entry = static_cast<icu::UnicodeString*>(obj);
    delete entry;
}

/**
 * Unfortunately, we have to convert the char16_t* currency code to char*
 * to use it as a resource key.
 */
സ്ഥിതം വരിയിൽ അക്ഷരം*
myUCharsToChars(അക്ഷരം* resultOfLen4, സ്ഥിരം char16_t* currency) {
    u_UCharsToChars(currency, resultOfLen4, ISO_CURRENCY_CODE_LENGTH);
    resultOfLen4[ISO_CURRENCY_CODE_LENGTH] = 0;
    തിരികെ resultOfLen4;
}

/**
 * Internal function to look up currency data.  Result is an array of
 * four integers.  The first is the fraction digits.  The second is the
 * rounding increment, or 0 if none.  The rounding increment is in
 * units of 10^(-fraction_digits).  The third and fourth are the same
 * except that they are those used in cash transactions ( cashDigits
 * and cashRounding ).
 */
സ്ഥിതം സ്ഥിരം int32_t*
_findMetaData(സ്ഥിരം char16_t* currency, UErrorCode& ec) {

    എങ്കിൽ (currency == nullptr || *currency == 0) {
        എങ്കിൽ (U_SUCCESS(ec)) {
            ec = U_ILLEGAL_ARGUMENT_ERROR;
        }
        തിരികെ LAST_RESORT_DATA;
    }

    // Get CurrencyMeta resource out of root locale file.  [This may
    // move out of the root locale file later; if it does, update this
    // code.]
    UResourceBundle* currencyData = ures_openDirect(U_ICUDATA_CURR, CURRENCY_DATA, &ec);
    LocalUResourceBundlePointer currencyMeta(ures_get_By_താക്കോൽ(currencyData, CURRENCY_META, currencyData, &ec));

    എങ്കിൽ (U_FAILURE(ec)) {
        // Config/build error; return hard-coded defaults
        തിരികെ LAST_RESORT_DATA;
    }

    // Look up our currency, or if that's not available, then DEFAULT
    അക്ഷരം buf[ISO_CURRENCY_CODE_LENGTH+1];
    UErrorCode ec2 = U_ZERO_ERROR; // local error code: soft failure
    LocalUResourceBundlePointer rb(ures_get_By_താക്കോൽ(currencyMeta.getAlias(), myUCharsToChars(buf, currency), nullptr, &ec2));
      എങ്കിൽ (U_FAILURE(ec2)) {
        rb.adoptInstead(ures_get_By_താക്കോൽ(currencyMeta.getAlias(),DEFAULT_META, nullptr, &ec));
        എങ്കിൽ (U_FAILURE(ec)) {
            // Config/build error; return hard-coded defaults
            തിരികെ LAST_RESORT_DATA;
        }
    }

    int32_t len;
    സ്ഥിരം int32_t *data = ures_getIntVector(rb.getAlias(), &len, &ec);
    എങ്കിൽ (U_FAILURE(ec) || len != 4) {
        // Config/build error; return hard-coded defaults
        എങ്കിൽ (U_SUCCESS(ec)) {
            ec = U_INVALID_FORMAT_ERROR;
        }
        തിരികെ LAST_RESORT_DATA;
    }

    തിരികെ data;
}

// -------------------------------------

സ്ഥിതം CharString
idForLocale(സ്ഥിരം അക്ഷരം* locale, UErrorCode* ec)
{
    തിരികെ ulocimp_getRegionForSupplementalData(locale, അസത്യം, *ec);
}

// ------------------------------------------
//
// Registration
//
//-------------------------------------------

// don't use ICUService since we don't need fallback

U_CDECL_BEGIN
സ്ഥിതം UBool U_CALLCONV currency_cleanup();
U_CDECL_END

#എങ്കിൽ !UCONFIG_NO_SERVICE
ഘടന CReg;

സ്ഥിതം UMutex gCRegLock;
സ്ഥിതം CReg* gCRegHead = nullptr;

ഘടന CReg : പൊതു icu::UMemory {
    CReg *next;
    char16_t iso[ISO_CURRENCY_CODE_LENGTH+1];
    അക്ഷരം  id[ULOC_FULLNAME_CAPACITY];

    CReg(സ്ഥിരം char16_t* _iso, സ്ഥിരം അക്ഷരം* _id)
        : next(nullptr)
    {
        uprv_strncpy(id, _id, വലുപ്പം(id)-1);
        id[വലുപ്പം(id)-1] = 0;
        u_memcpy(iso, _iso, ISO_CURRENCY_CODE_LENGTH);
        iso[ISO_CURRENCY_CODE_LENGTH] = 0;
    }

    സ്ഥിതം UCurr_Registry_താക്കോൽ reg(സ്ഥിരം char16_t* _iso, സ്ഥിരം അക്ഷരം* _id, UErrorCode* status)
    {
        എങ്കിൽ (status && U_SUCCESS(*status) && _iso && _id) {
            CReg* n = പുതിയ CReg(_iso, _id);
            എങ്കിൽ (n) {
                umtx_lock(&gCRegLock);
                എങ്കിൽ (!gCRegHead) {
                    /* register for the first time */
                    ucln_common_registerCleanup(UCLN_COMMON_CURRENCY, currency_cleanup);
                }
                n->next = gCRegHead;
                gCRegHead = n;
                umtx_unlock(&gCRegLock);
                തിരികെ n;
            }
            *status = U_MEMORY_ALLOCATION_ERROR;
        }
        തിരികെ nullptr;
    }

    സ്ഥിതം UBool unreg(UCurr_Registry_താക്കോൽ താക്കോൽ) {
        UBool found = അസത്യം;
        umtx_lock(&gCRegLock);

        CReg** p = &gCRegHead;
        ആകുമ്പോൾ (*p) {
            എങ്കിൽ (*p == താക്കോൽ) {
                *p = ((CReg*)താക്കോൽ)->next;
                delete (CReg*)താക്കോൽ;
                found = സത്യം;
                നിർത്തുക;
            }
            p = &((*p)->next);
        }

        umtx_unlock(&gCRegLock);
        തിരികെ found;
    }

    സ്ഥിതം സ്ഥിരം char16_t* get(സ്ഥിരം അക്ഷരം* id) {
        സ്ഥിരം char16_t* result = nullptr;
        umtx_lock(&gCRegLock);
        CReg* p = gCRegHead;

        /* register cleanup of the mutex */
        ucln_common_registerCleanup(UCLN_COMMON_CURRENCY, currency_cleanup);
        ആകുമ്പോൾ (p) {
            എങ്കിൽ (uprv_strcmp(id, p->id) == 0) {
                result = p->iso;
                നിർത്തുക;
            }
            p = p->next;
        }
        umtx_unlock(&gCRegLock);
        തിരികെ result;
    }

    /* This doesn't need to be thread safe. It's for u_cleanup only. */
    സ്ഥിതം ശൂന്യം cleanup() {
        ആകുമ്പോൾ (gCRegHead) {
            CReg* n = gCRegHead;
            gCRegHead = gCRegHead->next;
            delete n;
        }
    }
};

// -------------------------------------

U_CAPI UCurr_Registry_താക്കോൽ U_EXPORT2
ucurr_register(സ്ഥിരം char16_t* isoCode, സ്ഥിരം അക്ഷരം* locale, UErrorCode *status)
{
    എങ്കിൽ (status && U_SUCCESS(*status)) {
        CharString id = idForLocale(locale, status);
        തിരികെ CReg::reg(isoCode, id.data(), status);
    }
    തിരികെ nullptr;
}

// -------------------------------------

U_CAPI UBool U_EXPORT2
ucurr_unregister(UCurr_Registry_താക്കോൽ താക്കോൽ, UErrorCode* status)
{
    എങ്കിൽ (status && U_SUCCESS(*status)) {
        തിരികെ CReg::unreg(താക്കോൽ);
    }
    തിരികെ അസത്യം;
}
#endif /* UCONFIG_NO_SERVICE */

// -------------------------------------

/**
 * Release all static memory held by currency.
 */
/*The declaration here is needed so currency_cleanup()
 * can call this function.
 */
സ്ഥിതം UBool U_CALLCONV
currency_cache_cleanup();

U_CDECL_BEGIN
സ്ഥിതം UBool U_CALLCONV currency_cleanup() {
#എങ്കിൽ !UCONFIG_NO_SERVICE
    CReg::cleanup();
#endif
    /*
     * There might be some cached currency data or isoCodes data.
     */
    currency_cache_cleanup();
    isoCodes_cleanup();
    currSymbolsEquiv_cleanup();

    തിരികെ സത്യം;
}
U_CDECL_END

// -------------------------------------

U_CAPI int32_t U_EXPORT2
ucurr_forLocale(സ്ഥിരം അക്ഷരം* locale,
                char16_t* buff,
                int32_t buffCapacity,
                UErrorCode* ec) {
    എങ്കിൽ (U_FAILURE(*ec)) { തിരികെ 0; }
    എങ്കിൽ (buffCapacity < 0 || (buff == nullptr && buffCapacity > 0)) {
        *ec = U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ 0;
    }

    UErrorCode localStatus = U_ZERO_ERROR;
    CharString currency = ulocimp_get_Keyword_മൂല്യം(locale, "currency", localStatus);
    int32_t resLen = currency.length();

    എങ്കിൽ (U_SUCCESS(localStatus) && resLen == 3 && uprv_isInvariantString(currency.data(), resLen)) {
        എങ്കിൽ (resLen < buffCapacity) {
            T_CString_toUpperCase(currency.data());
            u_charsToUChars(currency.data(), buff, resLen);
        }
        തിരികെ u_terminateUChars(buff, buffCapacity, resLen, ec);
    }

    // get country or country_variant in `id'
    CharString id = idForLocale(locale, ec);
    എങ്കിൽ (U_FAILURE(*ec)) {
        തിരികെ 0;
    }

#എങ്കിൽ !UCONFIG_NO_SERVICE
    സ്ഥിരം char16_t* result = CReg::get(id.data());
    എങ്കിൽ (result) {
        എങ്കിൽ(buffCapacity > u_strlen(result)) {
            u_strcpy(buff, result);
        }
        resLen = u_strlen(result);
        തിരികെ u_terminateUChars(buff, buffCapacity, resLen, ec);
    }
#endif
    // Remove variants, which is only needed for registration.
    അക്ഷരം *idDelim = uprv_strchr(id.data(), VAR_DELIM);
    എങ്കിൽ (idDelim) {
        id.truncate(idDelim - id.data());
    }

    സ്ഥിരം char16_t* s = nullptr;  // Currency code from data file.
    എങ്കിൽ (id.isEmpty()) {
        // No point looking in the data for an empty string.
        // This is what we would get.
        localStatus = U_MISSING_RESOURCE_ERROR;
    } അല്ലെങ്കിൽ {
        // Look up the CurrencyMap element in the root bundle.
        localStatus = U_ZERO_ERROR;
        UResourceBundle *rb = ures_openDirect(U_ICUDATA_CURR, CURRENCY_DATA, &localStatus);
        UResourceBundle *cm = ures_get_By_താക്കോൽ(rb, CURRENCY_MAP, rb, &localStatus);
        LocalUResourceBundlePointer countryArray(ures_get_By_താക്കോൽ(rb, id.data(), cm, &localStatus));
        // https://unicode-org.atlassian.net/browse/ICU-21997
        // Prefer to use currencies that are legal tender.
        എങ്കിൽ (U_SUCCESS(localStatus)) {
            int32_t arrayLength = ures_getSize(countryArray.getAlias());
            വേണ്ടി (int32_t i = 0; i < arrayLength; ++i) {
                LocalUResourceBundlePointer currencyReq(
                    ures_getByIndex(countryArray.getAlias(), i, nullptr, &localStatus));
                // The currency is legal tender if it is *not* marked with tender{"false"}.
                UErrorCode tenderStatus = localStatus;
                സ്ഥിരം char16_t *tender =
                    ures_get_String_By_താക്കോൽ(currencyReq.getAlias(), "tender", nullptr, &tenderStatus);
                തർക്കം isTender = U_FAILURE(tenderStatus) || u_strcmp(tender, u"false") != 0;
                എങ്കിൽ (!isTender && s != nullptr) {
                    // We already have a non-tender currency. Ignore all following non-tender ones.
                    തുടരുക;
                }
                // Fetch the currency code.
                s = ures_get_String_By_താക്കോൽ(currencyReq.getAlias(), "id", &resLen, &localStatus);
                എങ്കിൽ (isTender) {
                    നിർത്തുക;
                }
            }
            എങ്കിൽ (U_SUCCESS(localStatus) && s == nullptr) {
                localStatus = U_MISSING_RESOURCE_ERROR;
            }
        }
    }

    എങ്കിൽ ((U_FAILURE(localStatus)) && strchr(id.data(), '_') != nullptr) {
        // We don't know about it.  Check to see if we support the variant.
        CharString parent = ulocimp_getParent(locale, *ec);
        *ec = U_USING_FALLBACK_WARNING;
        // TODO: Loop over the parent rather than recursing and
        // looking again for a currency keyword.
        തിരികെ ucurr_forLocale(parent.data(), buff, buffCapacity, ec);
    }
    എങ്കിൽ (*ec == U_ZERO_ERROR || localStatus != U_ZERO_ERROR) {
        // There is nothing to fallback to. Report the failure/warning if possible.
        *ec = localStatus;
    }
    എങ്കിൽ (U_SUCCESS(*ec)) {
        എങ്കിൽ(buffCapacity > resLen) {
            u_strcpy(buff, s);
        }
    }
    തിരികെ u_terminateUChars(buff, buffCapacity, resLen, ec);
}

// end registration

/**
 * Modify the given locale name by removing the rightmost _-delimited
 * element.  If there is none, empty the string ("" == root).
 * NOTE: The string "root" is not recognized; do not use it.
 * @return true if the fallback happened; false if locale is already
 * root ("").
 */
സ്ഥിതം UBool fallback(CharString& loc) {
    എങ്കിൽ (loc.isEmpty()) {
        തിരികെ അസത്യം;
    }
    UErrorCode status = U_ZERO_ERROR;
    എങ്കിൽ (loc == "en_GB") {
        // HACK: See #13368.  We need "en_GB" to fall back to "en_001" instead of "en"
        // in order to consume the correct data strings.  This hack will be removed
        // when proper data sink loading is implemented here.
        loc.truncate(3);
        loc.append("001", status);
    } അല്ലെങ്കിൽ {
        loc = ulocimp_getParent(loc.data(), status);
    }
 /*
    char *i = uprv_strrchr(loc, '_');
    if (i == nullptr) {
        i = loc;
    }
    *i = 0;
 */
    തിരികെ സത്യം;
}


U_CAPI സ്ഥിരം char16_t* U_EXPORT2
ucurr_getName(സ്ഥിരം char16_t* currency U_LIFETIME_BOUND,
              സ്ഥിരം അക്ഷരം* locale,
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

    എങ്കിൽ (U_FAILURE(*ec)) {
        തിരികെ nullptr;
    }

    int32_t choice = (int32_t) nameStyle;
    എങ്കിൽ (choice < 0 || choice > 4) {
        *ec = U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ nullptr;
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

    എങ്കിൽ (locale == nullptr) {
        locale = uloc_getDefault();
    }
    CharString loc = ulocimp_getName(locale, ec2);
    എങ്കിൽ (U_FAILURE(ec2)) {
        *ec = U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ nullptr;
    }

    അക്ഷരം buf[ISO_CURRENCY_CODE_LENGTH+1];
    myUCharsToChars(buf, currency);
    
    /* Normalize the keyword value to uppercase */
    T_CString_toUpperCase(buf);
    
    സ്ഥിരം char16_t* s = nullptr;
    ec2 = U_ZERO_ERROR;
    LocalUResourceBundlePointer rb(ures_open(U_ICUDATA_CURR, loc.data(), &ec2));

    എങ്കിൽ (nameStyle == UCURR_NARROW_ചിഹ്നം_NAME || nameStyle == UCURR_FORMAL_ചിഹ്നം_NAME || nameStyle == UCURR_VARIANT_ചിഹ്നം_NAME) {
        CharString താക്കോൽ;
        തിരഞ്ഞെടുപ്പ് (nameStyle) {
        അവസ്ഥ UCURR_NARROW_ചിഹ്നം_NAME:
            താക്കോൽ.append(CURRENCIES_NARROW, ec2);
            നിർത്തുക;
        അവസ്ഥ UCURR_FORMAL_ചിഹ്നം_NAME:
            താക്കോൽ.append(CURRENCIES_FORMAL, ec2);
            നിർത്തുക;
        അവസ്ഥ UCURR_VARIANT_ചിഹ്നം_NAME:
            താക്കോൽ.append(CURRENCIES_VARIANT, ec2);
            നിർത്തുക;
        സ്ഥിരസ്ഥിതി:
            *ec = U_UNSUPPORTED_ERROR;
            തിരികെ nullptr;
        }
        താക്കോൽ.append("/", ec2);
        താക്കോൽ.append(buf, ec2);
        s = ures_get_String_By_താക്കോൽ_With_Fallback(rb.getAlias(), താക്കോൽ.data(), len, &ec2);
        എങ്കിൽ (ec2 == U_MISSING_RESOURCE_ERROR) {
            *ec = U_USING_FALLBACK_WARNING;
            ec2 = U_ZERO_ERROR;
            choice = UCURR_ചിഹ്നം_NAME;
        }
    }
    എങ്കിൽ (s == nullptr) {
        ures_get_By_താക്കോൽ(rb.getAlias(), CURRENCIES, rb.getAlias(), &ec2);
        ures_get_By_താക്കോൽ_With_Fallback(rb.getAlias(), buf, rb.getAlias(), &ec2);
        s = ures_getStringByIndex(rb.getAlias(), choice, len, &ec2);
    }

    // If we've succeeded we're done.  Otherwise, try to fallback.
    // If that fails (because we are already at root) then exit.
    എങ്കിൽ (U_SUCCESS(ec2)) {
        എങ്കിൽ (ec2 == U_USING_DEFAULT_WARNING
            || (ec2 == U_USING_FALLBACK_WARNING && *ec != U_USING_DEFAULT_WARNING)) {
            *ec = ec2;
        }
    }

    // We no longer support choice format data in names.  Data should not contain
    // choice patterns.
    എങ്കിൽ (isChoiceFormat != nullptr) {
        *isChoiceFormat = അസത്യം;
    }
    എങ്കിൽ (U_SUCCESS(ec2)) {
        U_ASSERT(s != nullptr);
        തിരികെ s;
    }

    // If we fail to find a match, use the ISO 4217 code
    *len = u_strlen(currency); // Should == ISO_CURRENCY_CODE_LENGTH, but maybe not...?
    *ec = U_USING_DEFAULT_WARNING;
    തിരികെ currency;
}

U_CAPI സ്ഥിരം char16_t* U_EXPORT2
ucurr_getPluralName(സ്ഥിരം char16_t* currency U_LIFETIME_BOUND,
                    സ്ഥിരം അക്ഷരം* locale,
                    UBool* isChoiceFormat,
                    സ്ഥിരം അക്ഷരം* plural_എണ്ണം,
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

    എങ്കിൽ (U_FAILURE(*ec)) {
        തിരികെ nullptr;
    }

    // Use a separate UErrorCode here that does not propagate out of
    // this function.
    UErrorCode ec2 = U_ZERO_ERROR;

    എങ്കിൽ (locale == nullptr) {
        locale = uloc_getDefault();
    }
    CharString loc = ulocimp_getName(locale, ec2);
    എങ്കിൽ (U_FAILURE(ec2)) {
        *ec = U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ nullptr;
    }

    അക്ഷരം buf[ISO_CURRENCY_CODE_LENGTH+1];
    myUCharsToChars(buf, currency);

    സ്ഥിരം char16_t* s = nullptr;
    ec2 = U_ZERO_ERROR;
    UResourceBundle* rb = ures_open(U_ICUDATA_CURR, loc.data(), &ec2);

    rb = ures_get_By_താക്കോൽ(rb, CURRENCYPLURALS, rb, &ec2);

    // Fetch resource with multi-level resource inheritance fallback
    LocalUResourceBundlePointer curr(ures_get_By_താക്കോൽ_With_Fallback(rb, buf, rb, &ec2));

    s = ures_get_String_By_താക്കോൽ_With_Fallback(curr.getAlias(), plural_എണ്ണം, len, &ec2);
    എങ്കിൽ (U_FAILURE(ec2)) {
        //  fall back to "other"
        ec2 = U_ZERO_ERROR;
        s = ures_get_String_By_താക്കോൽ_With_Fallback(curr.getAlias(), "other", len, &ec2);     
        എങ്കിൽ (U_FAILURE(ec2)) {
            // fall back to long name in Currencies
            തിരികെ ucurr_getName(currency, locale, UCURR_LONG_NAME, 
                                 isChoiceFormat, len, ec);
        }
    }

    // If we've succeeded we're done.  Otherwise, try to fallback.
    // If that fails (because we are already at root) then exit.
    എങ്കിൽ (U_SUCCESS(ec2)) {
        എങ്കിൽ (ec2 == U_USING_DEFAULT_WARNING
            || (ec2 == U_USING_FALLBACK_WARNING && *ec != U_USING_DEFAULT_WARNING)) {
            *ec = ec2;
        }
        U_ASSERT(s != nullptr);
        തിരികെ s;
    }

    // If we fail to find a match, use the ISO 4217 code
    *len = u_strlen(currency); // Should == ISO_CURRENCY_CODE_LENGTH, but maybe not...?
    *ec = U_USING_DEFAULT_WARNING;
    തിരികെ currency;
}


//========================================================================
// Following are structure and function for parsing currency names

#define NEED_TO_BE_DELETED 0x1

// TODO: a better way to define this?
#define MAX_CURRENCY_NAME_LEN 100

തരനിർവചനം ഘടന {
    സ്ഥിരം അക്ഷരം* IsoCode;  //താക്കോൽ
    char16_t* currencyName;  //മൂല്യം
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
സ്ഥിതം പൂർണ്ണസംഖ്യ U_CALLCONV currencyNameComparator(സ്ഥിരം ശൂന്യം* a, സ്ഥിരം ശൂന്യം* b) {
    സ്ഥിരം CurrencyNameStruct* currName_1 = static_cast<സ്ഥിരം CurrencyNameStruct*>(a);
    സ്ഥിരം CurrencyNameStruct* currName_2 = static_cast<സ്ഥിരം CurrencyNameStruct*>(b);
    വേണ്ടി (int32_t i = 0; 
         i < MIN(currName_1->currencyNameLen, currName_2->currencyNameLen);
         ++i) {
        എങ്കിൽ (currName_1->currencyName[i] < currName_2->currencyName[i]) {
            തിരികെ -1;
        }
        എങ്കിൽ (currName_1->currencyName[i] > currName_2->currencyName[i]) {
            തിരികെ 1;
        }
    }
    എങ്കിൽ (currName_1->currencyNameLen < currName_2->currencyNameLen) {
        തിരികെ -1;
    } അല്ലെങ്കിൽ എങ്കിൽ (currName_1->currencyNameLen > currName_2->currencyNameLen) {
        തിരികെ 1;
    }
    തിരികെ 0;
}


// Give a locale, return the maximum number of currency names associated with
// this locale.
// It gets currency names from resource bundles using fallback.
// It is the maximum number because in the fallback chain, some of the 
// currency names are duplicated.
// For example, given locale as "en_US", the currency names get from resource
// bundle in "en_US" and "en" are duplicated. The fallback mechanism will count
// all currency names in "en_US" and "en".
സ്ഥിതം ശൂന്യം
get_Currency_Name_എണ്ണം(സ്ഥിരം അക്ഷരം* loc, int32_t* total_currency_name_എണ്ണം, int32_t* total_currency_ചിഹ്നം_എണ്ണം) {
    U_NAMESPACE_USE
    *total_currency_name_എണ്ണം = 0;
    *total_currency_ചിഹ്നം_എണ്ണം = 0;
    സ്ഥിരം char16_t* s = nullptr;
    CharString locale;
    {
        UErrorCode status = U_ZERO_ERROR;
        locale.append(loc, status);
        എങ്കിൽ (U_FAILURE(status)) { തിരികെ; }
    }
    സ്ഥിരം icu::Hashtable *currencySymbolsEquiv = getCurrSymbolsEquiv();
    വേണ്ടി (;;) {
        UErrorCode ec2 = U_ZERO_ERROR;
        // TODO: ures_openDirect?
        LocalUResourceBundlePointer rb(ures_open(U_ICUDATA_CURR, locale.data(), &ec2));
        LocalUResourceBundlePointer curr(ures_get_By_താക്കോൽ(rb.getAlias(), CURRENCIES, nullptr, &ec2));
        int32_t n = ures_getSize(curr.getAlias());
        വേണ്ടി (int32_t i=0; i<n; ++i) {
            LocalUResourceBundlePointer names(ures_getByIndex(curr.getAlias(), i, nullptr, &ec2));
            int32_t len;
            s = ures_getStringByIndex(names.getAlias(), UCURR_ചിഹ്നം_NAME, &len, &ec2);
            ++(*total_currency_ചിഹ്നം_എണ്ണം);  // currency symbol
            എങ്കിൽ (currencySymbolsEquiv != nullptr) {
                *total_currency_ചിഹ്നം_എണ്ണം += എണ്ണം_Equivalent(*currencySymbolsEquiv, UnicodeString(സത്യം, s, len));
            }
            ++(*total_currency_ചിഹ്നം_എണ്ണം); // iso code
            ++(*total_currency_name_എണ്ണം); // long name
        }

        // currency plurals
        UErrorCode ec3 = U_ZERO_ERROR;
        LocalUResourceBundlePointer curr_p(ures_get_By_താക്കോൽ(rb.getAlias(), CURRENCYPLURALS, nullptr, &ec3));
        n = ures_getSize(curr_p.getAlias());
        വേണ്ടി (int32_t i=0; i<n; ++i) {
            LocalUResourceBundlePointer names(ures_getByIndex(curr_p.getAlias(), i, nullptr, &ec3));
            *total_currency_name_എണ്ണം += ures_getSize(names.getAlias());
        }

        എങ്കിൽ (!fallback(locale)) {
            നിർത്തുക;
        }
    }
}

സ്ഥിതം char16_t*
toUpperCase(സ്ഥിരം char16_t* source, int32_t len, സ്ഥിരം അക്ഷരം* locale) {
    char16_t* dest = nullptr;
    UErrorCode ec = U_ZERO_ERROR;
    int32_t destLen = u_strToUpper(dest, 0, source, len, locale, &ec);

    ec = U_ZERO_ERROR;
    dest = static_cast<char16_t*>(uprv_malloc(വലുപ്പം(char16_t) * MAX(destLen, len)));
    എങ്കിൽ (dest == nullptr) {
        തിരികെ nullptr;
    }
    u_strToUpper(dest, destLen, source, len, locale, &ec);
    എങ്കിൽ (U_FAILURE(ec)) {
        u_memcpy(dest, source, len);
    } 
    തിരികെ dest;
}


സ്ഥിതം ശൂന്യം deleteCurrencyNames(CurrencyNameStruct* currencyNames, int32_t count);
// Collect all available currency names associated with the given locale
// (enable fallback chain).
// Read currenc names defined in resource bundle "Currencies" and
// "CurrencyPlural", enable fallback chain.
// return the malloc-ed currency name arrays and the total number of currency
// names in the array.
സ്ഥിതം ശൂന്യം
collectCurrencyNames(സ്ഥിരം അക്ഷരം* locale, 
                     CurrencyNameStruct** currencyNames, 
                     int32_t* total_currency_name_എണ്ണം, 
                     CurrencyNameStruct** currencySymbols, 
                     int32_t* total_currency_ചിഹ്നം_എണ്ണം, 
                     UErrorCode& ec) {
    എങ്കിൽ (U_FAILURE(ec)) {
        *currencyNames = *currencySymbols = nullptr;
        *total_currency_name_എണ്ണം = *total_currency_ചിഹ്നം_എണ്ണം = 0;
        തിരികെ;
    }
    U_NAMESPACE_USE
    സ്ഥിരം icu::Hashtable *currencySymbolsEquiv = getCurrSymbolsEquiv();
    // Look up the Currencies resource for the given locale.
    UErrorCode ec2 = U_ZERO_ERROR;

    എങ്കിൽ (locale == nullptr) {
        locale = uloc_getDefault();
    }
    CharString loc = ulocimp_getName(locale, ec2);
    എങ്കിൽ (U_FAILURE(ec2)) {
        ec = U_ILLEGAL_ARGUMENT_ERROR;
        *currencyNames = *currencySymbols = nullptr;
        *total_currency_name_എണ്ണം = *total_currency_ചിഹ്നം_എണ്ണം = 0;
        തിരികെ;
    }

    // Get maximum currency name count first.
    get_Currency_Name_എണ്ണം(loc.data(), total_currency_name_എണ്ണം, total_currency_ചിഹ്നം_എണ്ണം);

    *currencyNames = static_cast<CurrencyNameStruct*>(
        uprv_malloc(വലുപ്പം(CurrencyNameStruct) * (*total_currency_name_എണ്ണം)));
    എങ്കിൽ(*currencyNames == nullptr) {
        *currencySymbols = nullptr;
        *total_currency_name_എണ്ണം = *total_currency_ചിഹ്നം_എണ്ണം = 0;
        ec = U_MEMORY_ALLOCATION_ERROR;
        തിരികെ;
    }
    *currencySymbols = static_cast<CurrencyNameStruct*>(
        uprv_malloc(വലുപ്പം(CurrencyNameStruct) * (*total_currency_ചിഹ്നം_എണ്ണം)));

    എങ്കിൽ(*currencySymbols == nullptr) {
        uprv_free(*currencyNames);
        *currencyNames = nullptr;
        *total_currency_name_എണ്ണം = *total_currency_ചിഹ്നം_എണ്ണം = 0;
        ec = U_MEMORY_ALLOCATION_ERROR;
        തിരികെ;
    }

    സ്ഥിരം char16_t* s = nullptr;  // currency name
    അക്ഷരം* iso = nullptr;  // currency ISO code

    *total_currency_name_എണ്ണം = 0;
    *total_currency_ചിഹ്നം_എണ്ണം = 0;

    UErrorCode ec3 = U_ZERO_ERROR;
    UErrorCode ec4 = U_ZERO_ERROR;

    // Using hash to remove duplicates caused by locale fallback
    LocalUHashtablePointer currencyIsoCodes(uhash_open(uhash_hashChars, uhash_compareChars, nullptr, &ec3));
    LocalUHashtablePointer currencyPluralIsoCodes(uhash_open(uhash_hashChars, uhash_compareChars, nullptr, &ec4));
    വേണ്ടി (int32_t localeLevel = 0; ; ++localeLevel) {
        ec2 = U_ZERO_ERROR;
        // TODO: ures_openDirect
        LocalUResourceBundlePointer rb(ures_open(U_ICUDATA_CURR, loc.data(), &ec2));
        LocalUResourceBundlePointer curr(ures_get_By_താക്കോൽ(rb.getAlias(), CURRENCIES, nullptr, &ec2));
        int32_t n = ures_getSize(curr.getAlias());
        വേണ്ടി (int32_t i=0; i<n; ++i) {
            LocalUResourceBundlePointer names(ures_getByIndex(curr.getAlias(), i, nullptr, &ec2));
            int32_t len;
            s = ures_getStringByIndex(names.getAlias(), UCURR_ചിഹ്നം_NAME, &len, &ec2);
            // TODO: uhash_put wont change key/value?
            iso = const_cast<അക്ഷരം*>(ures_get_താക്കോൽ(names.getAlias()));
            എങ്കിൽ (localeLevel != 0 && uhash_get(currencyIsoCodes.getAlias(), iso) != nullptr) {
                തുടരുക;
            }
            uhash_put(currencyIsoCodes.getAlias(), iso, iso, &ec3); 
            // Add currency symbol.
            (*currencySymbols)[(*total_currency_ചിഹ്നം_എണ്ണം)++] = {iso, const_cast<char16_t*>(s), len, 0};

            // Add equivalent symbols
            എങ്കിൽ (currencySymbolsEquiv != nullptr) {
                UnicodeString str(സത്യം, s, len);
                icu::EquivIterator iter(*currencySymbolsEquiv, str);
                സ്ഥിരം UnicodeString *ചിഹ്നം;
                ആകുമ്പോൾ ((ചിഹ്നം = iter.next()) != nullptr) {
                    (*currencySymbols)[(*total_currency_ചിഹ്നം_എണ്ണം)++]
                        = {iso, const_cast<char16_t*>(ചിഹ്നം->getBuffer()), ചിഹ്നം->length(), 0};
                }
            }

            // Add currency long name.
            s = ures_getStringByIndex(names.getAlias(), UCURR_LONG_NAME, &len, &ec2);
            char16_t* upperName = toUpperCase(s, len, locale);
            എങ്കിൽ (upperName == nullptr) {
                ec = U_MEMORY_ALLOCATION_ERROR;
                പോകുക error;
            }
            (*currencyNames)[(*total_currency_name_എണ്ണം)++] = {iso, upperName, len, NEED_TO_BE_DELETED};

            // put (iso, 3, and iso) in to array
            // Add currency ISO code.
            char16_t* isoCode = static_cast<char16_t*>(uprv_malloc(വലുപ്പം(char16_t) * 3));
            എങ്കിൽ (isoCode == nullptr) {
                ec = U_MEMORY_ALLOCATION_ERROR;
                പോകുക error;
            }
            // Must convert iso[] into Unicode
            u_charsToUChars(iso, isoCode, 3);
            (*currencySymbols)[(*total_currency_ചിഹ്നം_എണ്ണം)++] = {iso, isoCode, 3, NEED_TO_BE_DELETED};
        }

        // currency plurals
        UErrorCode ec5 = U_ZERO_ERROR;
        LocalUResourceBundlePointer curr_p(ures_get_By_താക്കോൽ(rb.getAlias(), CURRENCYPLURALS, nullptr, &ec5));
        n = ures_getSize(curr_p.getAlias());
        വേണ്ടി (int32_t i=0; i<n; ++i) {
            LocalUResourceBundlePointer names(ures_getByIndex(curr_p.getAlias(), i, nullptr, &ec5));
            iso = const_cast<അക്ഷരം*>(ures_get_താക്കോൽ(names.getAlias()));
            // Using hash to remove duplicated ISO codes in fallback chain.
            എങ്കിൽ (localeLevel != 0 && uhash_get(currencyPluralIsoCodes.getAlias(), iso) != nullptr) {
                തുടരുക;
            }
            uhash_put(currencyPluralIsoCodes.getAlias(), iso, iso, &ec4);
            int32_t num = ures_getSize(names.getAlias());
            int32_t len;
            വേണ്ടി (int32_t j = 0; j < num; ++j) {
                // TODO: remove duplicates between singular name and 
                // currency long name?
                s = ures_getStringByIndex(names.getAlias(), j, &len, &ec5);
                char16_t* upperName = toUpperCase(s, len, locale);
                എങ്കിൽ (upperName == nullptr) {
                    ec = U_MEMORY_ALLOCATION_ERROR;
                    പോകുക error;
                }
                (*currencyNames)[(*total_currency_name_എണ്ണം)++] = {iso, upperName, len, NEED_TO_BE_DELETED};
            }
        }

        എങ്കിൽ (!fallback(loc)) {
            നിർത്തുക;
        }
    }

    // quick sort the struct
    qsort(*currencyNames, *total_currency_name_എണ്ണം, 
          വലുപ്പം(CurrencyNameStruct), currencyNameComparator);
    qsort(*currencySymbols, *total_currency_ചിഹ്നം_എണ്ണം, 
          വലുപ്പം(CurrencyNameStruct), currencyNameComparator);

#ifdef UCURR_DEBUG
    printf("currency name count: %d\n", *total_currency_name_എണ്ണം);
    വേണ്ടി (int32_t index = 0; index < *total_currency_name_എണ്ണം; ++index) {
        printf("index: %d\n", index);
        printf("iso: %s\n", (*currencyNames)[index].IsoCode);
        അക്ഷരം curNameBuf[1024];
        memset(curNameBuf, 0, 1024);
        u_austrncpy(curNameBuf, (*currencyNames)[index].currencyName, (*currencyNames)[index].currencyNameLen);
        printf("currencyName: %s\n", curNameBuf);
        printf("len: %d\n", (*currencyNames)[index].currencyNameLen);
    }
    printf("currency symbol count: %d\n", *total_currency_ചിഹ്നം_എണ്ണം);
    വേണ്ടി (int32_t index = 0; index < *total_currency_ചിഹ്നം_എണ്ണം; ++index) {
        printf("index: %d\n", index);
        printf("iso: %s\n", (*currencySymbols)[index].IsoCode);
        അക്ഷരം curNameBuf[1024];
        memset(curNameBuf, 0, 1024);
        u_austrncpy(curNameBuf, (*currencySymbols)[index].currencyName, (*currencySymbols)[index].currencyNameLen);
        printf("currencySymbol: %s\n", curNameBuf);
        printf("len: %d\n", (*currencySymbols)[index].currencyNameLen);
    }
#endif
    // fail on hashtable errors
    എങ്കിൽ (U_FAILURE(ec3)) {
      ec = ec3;
    } അല്ലെങ്കിൽ എങ്കിൽ (U_FAILURE(ec4)) {
      ec = ec4;
    }

  error:
    // clean up if we got error
    എങ്കിൽ (U_FAILURE(ec)) {
        deleteCurrencyNames(*currencyNames, *total_currency_name_എണ്ണം);
        deleteCurrencyNames(*currencySymbols, *total_currency_ചിഹ്നം_എണ്ണം);
        *currencyNames = *currencySymbols = nullptr;
        *total_currency_name_എണ്ണം = *total_currency_ചിഹ്നം_എണ്ണം = 0;
    }
}

// @param  currencyNames: currency names array
// @param  indexInCurrencyNames: the index of the character in currency names 
//         array against which the comparison is done
// @param  key: input text char to compare against
// @param  begin(IN/OUT): the begin index of matching range in currency names array
// @param  end(IN/OUT): the end index of matching range in currency names array.
സ്ഥിതം int32_t
ദ്വിമാന_തിരയൽ(സ്ഥിരം CurrencyNameStruct* currencyNames, 
             int32_t indexInCurrencyNames,
             സ്ഥിരം char16_t താക്കോൽ,
             int32_t* begin, int32_t* end) {
#ifdef UCURR_DEBUG
    printf("key = %x\n", താക്കോൽ);
#endif
   int32_t first = *begin;
   int32_t last = *end;
   ആകുമ്പോൾ (first <= last) {
       int32_t mid = (first + last) / 2;  // compute mid point.
       എങ്കിൽ (indexInCurrencyNames >= currencyNames[mid].currencyNameLen) {
           first = mid + 1;
       } അല്ലെങ്കിൽ {
           എങ്കിൽ (താക്കോൽ > currencyNames[mid].currencyName[indexInCurrencyNames]) {
               first = mid + 1;
           }
           അല്ലെങ്കിൽ എങ്കിൽ (താക്കോൽ < currencyNames[mid].currencyName[indexInCurrencyNames]) {
               last = mid - 1;
           }
           അല്ലെങ്കിൽ {
                // Find a match, and looking for ranges
                // Now do two more binary searches. First, on the left side for
                // the greatest L such that CurrencyNameStruct[L] < key.
                int32_t L = *begin;
                int32_t R = mid;

#ifdef UCURR_DEBUG
                printf("mid = %d\n", mid);
#endif
                ആകുമ്പോൾ (L < R) {
                    int32_t M = (L + R) / 2;
#ifdef UCURR_DEBUG
                    printf("L = %d, R = %d, M = %d\n", L, R, M);
#endif
                    എങ്കിൽ (indexInCurrencyNames >= currencyNames[M].currencyNameLen) {
                        L = M + 1;
                    } അല്ലെങ്കിൽ {
                        എങ്കിൽ (currencyNames[M].currencyName[indexInCurrencyNames] < താക്കോൽ) {
                            L = M + 1;
                        } അല്ലെങ്കിൽ {
#ifdef UCURR_DEBUG
                            U_ASSERT(currencyNames[M].currencyName[indexInCurrencyNames] == താക്കോൽ);
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
                U_ASSERT(currencyNames[*begin].currencyName[indexInCurrencyNames] == താക്കോൽ);
#endif

                // Now for the second search, finding the least R such that
                // key < CurrencyNameStruct[R].
                L = mid;
                R = *end;
                ആകുമ്പോൾ (L < R) {
                    int32_t M = (L + R) / 2;
#ifdef UCURR_DEBUG
                    printf("L = %d, R = %d, M = %d\n", L, R, M);
#endif
                    എങ്കിൽ (currencyNames[M].currencyNameLen < indexInCurrencyNames) {
                        L = M + 1;
                    } അല്ലെങ്കിൽ {
                        എങ്കിൽ (currencyNames[M].currencyName[indexInCurrencyNames] > താക്കോൽ) {
                            R = M;
                        } അല്ലെങ്കിൽ {
#ifdef UCURR_DEBUG
                            U_ASSERT(currencyNames[M].currencyName[indexInCurrencyNames] == താക്കോൽ);
#endif
                            L = M + 1;
                        }
                    }
                }
#ifdef UCURR_DEBUG
                U_ASSERT(L == R);
#endif
                എങ്കിൽ (currencyNames[R].currencyName[indexInCurrencyNames] > താക്കോൽ) {
                    *end = R - 1;
                } അല്ലെങ്കിൽ {
                    *end = R;
                }
#ifdef UCURR_DEBUG
                printf("end = %d\n", *end);
#endif

                // now, found the range. check whether there is exact match
                എങ്കിൽ (currencyNames[*begin].currencyNameLen == indexInCurrencyNames + 1) {
                    തിരികെ *begin;  // find range and exact match.
                }
                തിരികെ -1;  // find range, but no exact match.
           }
       }
   }
   *begin = -1;
   *end = -1;
   തിരികെ -1;    // failed to find range.
}


// Linear search "text" in "currencyNames".
// @param  begin, end: the begin and end index in currencyNames, within which
//         range should the search be performed.
// @param  textLen: the length of the text to be compared
// @param  maxMatchLen(IN/OUT): passing in the computed max matching length
//                              pass out the new max  matching length
// @param  maxMatchIndex: the index in currencyName which has the longest
//                        match with input text.
സ്ഥിതം ശൂന്യം
linear_തിരയൽ(സ്ഥിരം CurrencyNameStruct* currencyNames, 
             int32_t begin, int32_t end,
             സ്ഥിരം char16_t* text, int32_t textLen,
             int32_t *partialMatchLen,
             int32_t *maxMatchLen, int32_t* maxMatchIndex) {
    int32_t initialPartialMatchLen = *partialMatchLen;
    വേണ്ടി (int32_t index = begin; index <= end; ++index) {
        int32_t len = currencyNames[index].currencyNameLen;
        എങ്കിൽ (len > *maxMatchLen && len <= textLen &&
            uprv_memcmp(currencyNames[index].currencyName, text, len * വലുപ്പം(char16_t)) == 0) {
            *partialMatchLen = MAX(*partialMatchLen, len);
            *maxMatchIndex = index;
            *maxMatchLen = len;
#ifdef UCURR_DEBUG
            printf("maxMatchIndex = %d, maxMatchLen = %d\n",
                   *maxMatchIndex, *maxMatchLen);
#endif
        } അല്ലെങ്കിൽ {
            // Check for partial matches.
            വേണ്ടി (int32_t i=initialPartialMatchLen; i<MIN(len, textLen); i++) {
                എങ്കിൽ (currencyNames[index].currencyName[i] != text[i]) {
                    നിർത്തുക;
                }
                *partialMatchLen = MAX(*partialMatchLen, i + 1);
            }
        }
    }
}

#define LINEAR_തിരയൽ_THRESHOLD 10

// Find longest match between "text" and currency names in "currencyNames".
// @param  total_currency_count: total number of currency names in CurrencyNames.
// @param  textLen: the length of the text to be compared
// @param  maxMatchLen: passing in the computed max matching length
//                              pass out the new max  matching length
// @param  maxMatchIndex: the index in currencyName which has the longest
//                        match with input text.
സ്ഥിതം ശൂന്യം
തിരയൽ_Currency_Name(സ്ഥിരം CurrencyNameStruct* currencyNames, 
                   int32_t total_currency_എണ്ണം,
                   സ്ഥിരം char16_t* text, int32_t textLen,
                   int32_t *partialMatchLen,
                   int32_t* maxMatchLen, int32_t* maxMatchIndex) {
    *maxMatchIndex = -1;
    *maxMatchLen = 0;
    int32_t matchIndex = -1;
    int32_t ദ്വിമാന_തിരയൽ_Begin = 0;
    int32_t ദ്വിമാന_തിരയൽ_End = total_currency_എണ്ണം - 1;
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
    വേണ്ടി (int32_t index = 0; index < textLen; ++index) {
        // matchIndex saves the one with exact match till the current point.
        // [binarySearchBegin, binarySearchEnd] saves the matching range.
        matchIndex = ദ്വിമാന_തിരയൽ(currencyNames, index,
                                  text[index],
                                  &ദ്വിമാന_തിരയൽ_Begin, &ദ്വിമാന_തിരയൽ_End);
        എങ്കിൽ (ദ്വിമാന_തിരയൽ_Begin == -1) { // did not find the range
            നിർത്തുക;
        }
        *partialMatchLen = MAX(*partialMatchLen, index + 1);
        എങ്കിൽ (matchIndex != -1) { 
            // find an exact match for text from text[0] to text[index] 
            // in currencyNames array.
            *maxMatchLen = index + 1;
            *maxMatchIndex = matchIndex;
        }
        എങ്കിൽ (ദ്വിമാന_തിരയൽ_End - ദ്വിമാന_തിരയൽ_Begin < LINEAR_തിരയൽ_THRESHOLD) {
            // linear search if within threshold.
            linear_തിരയൽ(currencyNames, ദ്വിമാന_തിരയൽ_Begin, ദ്വിമാന_തിരയൽ_End,
                         text, textLen,
                         partialMatchLen,
                         maxMatchLen, maxMatchIndex);
            നിർത്തുക;
        }
    }
}

//========================= currency name cache =====================
തരനിർവചനം ഘടന {
    അക്ഷരം locale[ULOC_FULLNAME_CAPACITY];  //key
    // currency names, case insensitive
    CurrencyNameStruct* currencyNames;  //മൂല്യം
    int32_t total_Currency_Name_എണ്ണം;  // currency name count
    // currency symbols and ISO code, case sensitive
    CurrencyNameStruct* currencySymbols; //മൂല്യം
    int32_t total_Currency_ചിഹ്നം_എണ്ണം;  //എണ്ണം
    // reference count.
    // reference count is set to 1 when an entry is put to cache.
    // it increases by 1 before accessing, and decreased by 1 after accessing.
    // The entry is deleted when ref count is zero, which means 
    // the entry is replaced out of cache and no process is accessing it.
    int32_t ref_എണ്ണം;
} CurrencyNameCacheEntry;


#define CURRENCY_NAME_CACHE_അക്കം 10

// Reserve 10 cache entries.
സ്ഥിതം CurrencyNameCacheEntry* currCache[CURRENCY_NAME_CACHE_അക്കം] = {nullptr};
// Using an index to indicate which entry to be replaced when cache is full.
// It is a simple round-robin replacement strategy.
സ്ഥിതം int8_t currentCacheEntryIndex = 0;

സ്ഥിതം UMutex gCurrencyCacheMutex;

// Cache deletion
സ്ഥിതം ശൂന്യം
deleteCurrencyNames(CurrencyNameStruct* currencyNames, int32_t count) {
    വേണ്ടി (int32_t index = 0; index < count; ++index) {
        എങ്കിൽ ( (currencyNames[index].flag & NEED_TO_BE_DELETED) ) {
            uprv_free(currencyNames[index].currencyName);
        }
    }
    uprv_free(currencyNames);
}


സ്ഥിതം ശൂന്യം
deleteCacheEntry(CurrencyNameCacheEntry* entry) {
    deleteCurrencyNames(entry->currencyNames, entry->total_Currency_Name_എണ്ണം);
    deleteCurrencyNames(entry->currencySymbols, entry->total_Currency_ചിഹ്നം_എണ്ണം);
    uprv_free(entry);
}


// Cache clean up
സ്ഥിതം UBool U_CALLCONV
currency_cache_cleanup() {
    വേണ്ടി (int32_t i = 0; i < CURRENCY_NAME_CACHE_അക്കം; ++i) {
        എങ്കിൽ (currCache[i]) {
            deleteCacheEntry(currCache[i]);
            currCache[i] = nullptr;
        }
    }
    തിരികെ സത്യം;
}


/**
 * Loads the currency name data from the cache, or from resource bundles if necessary.
 * The refCount is automatically incremented.  It is the caller's responsibility
 * to decrement it when done!
 */
സ്ഥിതം CurrencyNameCacheEntry*
getCacheEntry(സ്ഥിരം അക്ഷരം* locale, UErrorCode& ec) {

    int32_t total_currency_name_എണ്ണം = 0;
    CurrencyNameStruct* currencyNames = nullptr;
    int32_t total_currency_ചിഹ്നം_എണ്ണം = 0;
    CurrencyNameStruct* currencySymbols = nullptr;
    CurrencyNameCacheEntry* cacheEntry = nullptr;

    umtx_lock(&gCurrencyCacheMutex);
    // in order to handle racing correctly,
    // not putting 'search' in a separate function.
    int8_t found = -1;
    വേണ്ടി (int8_t i = 0; i < CURRENCY_NAME_CACHE_അക്കം; ++i) {
        എങ്കിൽ (currCache[i]!= nullptr &&
            uprv_strcmp(locale, currCache[i]->locale) == 0) {
            found = i;
            നിർത്തുക;
        }
    }
    എങ്കിൽ (found != -1) {
        cacheEntry = currCache[found];
        ++(cacheEntry->ref_എണ്ണം);
    }
    umtx_unlock(&gCurrencyCacheMutex);
    എങ്കിൽ (found == -1) {
        collectCurrencyNames(locale, &currencyNames, &total_currency_name_എണ്ണം, &currencySymbols, &total_currency_ചിഹ്നം_എണ്ണം, ec);
        എങ്കിൽ (U_FAILURE(ec)) {
            തിരികെ nullptr;
        }
        umtx_lock(&gCurrencyCacheMutex);
        // check again.
        വേണ്ടി (int8_t i = 0; i < CURRENCY_NAME_CACHE_അക്കം; ++i) {
            എങ്കിൽ (currCache[i]!= nullptr &&
                uprv_strcmp(locale, currCache[i]->locale) == 0) {
                found = i;
                നിർത്തുക;
            }
        }
        എങ്കിൽ (found == -1) {
            // insert new entry to 
            // currentCacheEntryIndex % CURRENCY_NAME_CACHE_NUM
            // and remove the existing entry 
            // currentCacheEntryIndex % CURRENCY_NAME_CACHE_NUM
            // from cache.
            cacheEntry = currCache[currentCacheEntryIndex];
            എങ്കിൽ (cacheEntry) {
                --(cacheEntry->ref_എണ്ണം);
                // delete if the ref count is zero
                എങ്കിൽ (cacheEntry->ref_എണ്ണം == 0) {
                    deleteCacheEntry(cacheEntry);
                }
            }
            cacheEntry = static_cast<CurrencyNameCacheEntry*>(uprv_malloc(വലുപ്പം(CurrencyNameCacheEntry)));
            എങ്കിൽ (cacheEntry == nullptr) {
                deleteCurrencyNames(currencyNames, total_currency_name_എണ്ണം);
                deleteCurrencyNames(currencySymbols, total_currency_ചിഹ്നം_എണ്ണം);
                ec = U_MEMORY_ALLOCATION_ERROR;
                തിരികെ nullptr;
            }
            currCache[currentCacheEntryIndex] = cacheEntry;
            uprv_strcpy(cacheEntry->locale, locale);
            cacheEntry->currencyNames = currencyNames;
            cacheEntry->total_Currency_Name_എണ്ണം = total_currency_name_എണ്ണം;
            cacheEntry->currencySymbols = currencySymbols;
            cacheEntry->total_Currency_ചിഹ്നം_എണ്ണം = total_currency_ചിഹ്നം_എണ്ണം;
            cacheEntry->ref_എണ്ണം = 2; // one for cache, one for reference
            currentCacheEntryIndex = (currentCacheEntryIndex + 1) % CURRENCY_NAME_CACHE_അക്കം;
            ucln_common_registerCleanup(UCLN_COMMON_CURRENCY, currency_cleanup);
        } അല്ലെങ്കിൽ {
            deleteCurrencyNames(currencyNames, total_currency_name_എണ്ണം);
            deleteCurrencyNames(currencySymbols, total_currency_ചിഹ്നം_എണ്ണം);
            cacheEntry = currCache[found];
            ++(cacheEntry->ref_എണ്ണം);
        }
        umtx_unlock(&gCurrencyCacheMutex);
    }

    തിരികെ cacheEntry;
}

സ്ഥിതം ശൂന്യം releaseCacheEntry(CurrencyNameCacheEntry* cacheEntry) {
    umtx_lock(&gCurrencyCacheMutex);
    --(cacheEntry->ref_എണ്ണം);
    എങ്കിൽ (cacheEntry->ref_എണ്ണം == 0) {  // remove
        deleteCacheEntry(cacheEntry);
    }
    umtx_unlock(&gCurrencyCacheMutex);
}

U_CAPI ശൂന്യം
uprv_parseCurrency(സ്ഥിരം അക്ഷരം* locale,
                   സ്ഥിരം icu::UnicodeString& text,
                   icu::ParsePosition& pos,
                   int8_t type,
                   int32_t* partialMatchLen,
                   char16_t* result,
                   UErrorCode& ec) {
    U_NAMESPACE_USE
    എങ്കിൽ (U_FAILURE(ec)) {
        തിരികെ;
    }
    CurrencyNameCacheEntry* cacheEntry = getCacheEntry(locale, ec);
    എങ്കിൽ (U_FAILURE(ec)) {
        തിരികെ;
    }

    int32_t total_currency_name_എണ്ണം = cacheEntry->total_Currency_Name_എണ്ണം;
    CurrencyNameStruct* currencyNames = cacheEntry->currencyNames;
    int32_t total_currency_ചിഹ്നം_എണ്ണം = cacheEntry->total_Currency_ചിഹ്നം_എണ്ണം;
    CurrencyNameStruct* currencySymbols = cacheEntry->currencySymbols;

    int32_t start = pos.getIndex();

    char16_t നിവേശം_Text[MAX_CURRENCY_NAME_LEN];
    char16_t upperText[MAX_CURRENCY_NAME_LEN];
    int32_t textLen = MIN(MAX_CURRENCY_NAME_LEN, text.length() - start);
    text.extract(start, textLen, നിവേശം_Text);
    UErrorCode ec1 = U_ZERO_ERROR;
    textLen = u_strToUpper(upperText, MAX_CURRENCY_NAME_LEN, നിവേശം_Text, textLen, locale, &ec1);

    // Make sure partialMatchLen is initialized
    *partialMatchLen = 0;

    int32_t max = 0;
    int32_t matchIndex = -1;
    // case in-sensitive comparison against currency names
    തിരയൽ_Currency_Name(currencyNames, total_currency_name_എണ്ണം, 
                       upperText, textLen, partialMatchLen, &max, &matchIndex);

#ifdef UCURR_DEBUG
    printf("search in names, max = %d, matchIndex = %d\n", max, matchIndex);
#endif

    int32_t max_In_ചിഹ്നം = 0;
    int32_t match_Index_In_ചിഹ്നം = -1;
    എങ്കിൽ (type != UCURR_LONG_NAME) {  // not name only
        // case sensitive comparison against currency symbols and ISO code.
        തിരയൽ_Currency_Name(currencySymbols, total_currency_ചിഹ്നം_എണ്ണം, 
                           നിവേശം_Text, textLen,
                           partialMatchLen,
                           &max_In_ചിഹ്നം, &match_Index_In_ചിഹ്നം);
    }

#ifdef UCURR_DEBUG
    printf("search in symbols, maxInSymbol = %d, matchIndexInSymbol = %d\n", max_In_ചിഹ്നം, match_Index_In_ചിഹ്നം);
    എങ്കിൽ(match_Index_In_ചിഹ്നം != -1) {
      printf("== ISO=%s\n", currencySymbols[match_Index_In_ചിഹ്നം].IsoCode);
    }
#endif

    എങ്കിൽ (max >= max_In_ചിഹ്നം && matchIndex != -1) {
        u_charsToUChars(currencyNames[matchIndex].IsoCode, result, 4);
        pos.setIndex(start + max);
    } അല്ലെങ്കിൽ എങ്കിൽ (max_In_ചിഹ്നം >= max && match_Index_In_ചിഹ്നം != -1) {
        u_charsToUChars(currencySymbols[match_Index_In_ചിഹ്നം].IsoCode, result, 4);
        pos.setIndex(start + max_In_ചിഹ്നം);
    }

    // decrease reference count
    releaseCacheEntry(cacheEntry);
}

ശൂന്യം uprv_currencyLeads(സ്ഥിരം അക്ഷരം* locale, icu::UnicodeSet& result, UErrorCode& ec) {
    U_NAMESPACE_USE
    എങ്കിൽ (U_FAILURE(ec)) {
        തിരികെ;
    }
    CurrencyNameCacheEntry* cacheEntry = getCacheEntry(locale, ec);
    എങ്കിൽ (U_FAILURE(ec)) {
        തിരികെ;
    }

    വേണ്ടി (int32_t i=0; i<cacheEntry->total_Currency_ചിഹ്നം_എണ്ണം; i++) {
        സ്ഥിരം CurrencyNameStruct& info = cacheEntry->currencySymbols[i];
        UChar32 cp;
        U16_GET(info.currencyName, 0, 0, info.currencyNameLen, cp);
        result.add(cp);
    }

    വേണ്ടി (int32_t i=0; i<cacheEntry->total_Currency_Name_എണ്ണം; i++) {
        സ്ഥിരം CurrencyNameStruct& info = cacheEntry->currencyNames[i];
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
U_CAPI ശൂന്യം
uprv_getStaticCurrencyName(സ്ഥിരം char16_t* iso, സ്ഥിരം അക്ഷരം* loc,
                           icu::UnicodeString& result, UErrorCode& ec)
{
    U_NAMESPACE_USE

    int32_t len;
    സ്ഥിരം char16_t* currname = ucurr_getName(iso, loc, UCURR_ചിഹ്നം_NAME,
                                          nullptr /* isChoiceFormat */, &len, &ec);
    എങ്കിൽ (U_SUCCESS(ec)) {
        result.setTo(currname, len);
    }
}

U_CAPI int32_t U_EXPORT2
ucurr_getDefaultFractionDigits(സ്ഥിരം char16_t* currency, UErrorCode* ec) {
    തിരികെ ucurr_getDefaultFractionDigitsForUsage(currency,UCURR_USAGE_STANDARD,ec);
}

U_CAPI int32_t U_EXPORT2
ucurr_getDefaultFractionDigitsForUsage(സ്ഥിരം char16_t* currency, സ്ഥിരം UCurrencyUsage usage, UErrorCode* ec) {
    int32_t fracDigits = 0;
    എങ്കിൽ (U_SUCCESS(*ec)) {
        തിരഞ്ഞെടുപ്പ് (usage) {
            അവസ്ഥ UCURR_USAGE_STANDARD:
                fracDigits = (_findMetaData(currency, *ec))[0];
                നിർത്തുക;
            അവസ്ഥ UCURR_USAGE_CASH:
                fracDigits = (_findMetaData(currency, *ec))[2];
                നിർത്തുക;
            സ്ഥിരസ്ഥിതി:
                *ec = U_UNSUPPORTED_ERROR;
        }
    }
    തിരികെ fracDigits;
}

U_CAPI ഇരട്ട U_EXPORT2
ucurr_getRoundingIncrement(സ്ഥിരം char16_t* currency, UErrorCode* ec) {
    തിരികെ ucurr_getRoundingIncrementForUsage(currency, UCURR_USAGE_STANDARD, ec);
}

U_CAPI ഇരട്ട U_EXPORT2
ucurr_getRoundingIncrementForUsage(സ്ഥിരം char16_t* currency, സ്ഥിരം UCurrencyUsage usage, UErrorCode* ec) {
    ഇരട്ട result = 0.0;

    സ്ഥിരം int32_t *data = _findMetaData(currency, *ec);
    എങ്കിൽ (U_SUCCESS(*ec)) {
        int32_t fracDigits;
        int32_t increment;
        തിരഞ്ഞെടുപ്പ് (usage) {
            അവസ്ഥ UCURR_USAGE_STANDARD:
                fracDigits = data[0];
                increment = data[1];
                നിർത്തുക;
            അവസ്ഥ UCURR_USAGE_CASH:
                fracDigits = data[2];
                increment = data[3];
                നിർത്തുക;
            സ്ഥിരസ്ഥിതി:
                *ec = U_UNSUPPORTED_ERROR;
                തിരികെ result;
        }

        // If the meta data is invalid, return 0.0
        എങ്കിൽ (fracDigits < 0 || fracDigits > MAX_POW10) {
            *ec = U_INVALID_FORMAT_ERROR;
        } അല്ലെങ്കിൽ {
            // A rounding value of 0 or 1 indicates no rounding.
            എങ്കിൽ (increment >= 2) {
                // Return (increment) / 10^(fracDigits).  The only actual rounding data,
                // as of this writing, is CHF { 2, 5 }.
                result = ഇരട്ട(increment) / POW10[fracDigits];
            }
        }
    }

    തിരികെ result;
}

U_CDECL_BEGIN

തരനിർവചനം ഘടന UCurrencyContext {
    uint32_t currType; /* UCurrCurrencyType */
    uint32_t listIdx;
} UCurrencyContext;

/*
Please keep this list in alphabetical order.
You can look at the CLDR supplemental data or ISO-4217 for the meaning of some
of these items.
ISO-4217: http://www.iso.org/iso/en/prods-services/popstds/currencycodeslist.html
*/
സ്ഥിതം സ്ഥിരം ഘടന CurrencyList {
    സ്ഥിരം അക്ഷരം *currency;
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

സ്ഥിതം int32_t U_CALLCONV
ucurr_എണ്ണം_Currency_List(UEnumeration *enumerator, UErrorCode * /*pErrorCode*/) {
    UCurrencyContext *myContext = (UCurrencyContext *)(enumerator->context);
    uint32_t currType = myContext->currType;
    int32_t count = 0;

    /* Count the number of items matching the type we are looking for. */
    വേണ്ടി (int32_t idx = 0; gCurrencyList[idx].currency != nullptr; idx++) {
        എങ്കിൽ (UCURR_MATCHES_BITMASK(gCurrencyList[idx].currType, currType)) {
            count++;
        }
    }
    തിരികെ count;
}

സ്ഥിതം സ്ഥിരം അക്ഷരം* U_CALLCONV
ucurr_nextCurrencyList(UEnumeration *enumerator,
                        int32_t* resultLength,
                        UErrorCode * /*pErrorCode*/)
{
    UCurrencyContext *myContext = (UCurrencyContext *)(enumerator->context);

    /* Find the next in the list that matches the type we are looking for. */
    ആകുമ്പോൾ (myContext->listIdx < UPRV_LENGTHOF(gCurrencyList)-1) {
        സ്ഥിരം ഘടന CurrencyList *currItem = &gCurrencyList[myContext->listIdx++];
        എങ്കിൽ (UCURR_MATCHES_BITMASK(currItem->currType, myContext->currType))
        {
            എങ്കിൽ (resultLength) {
                *resultLength = 3; /* Currency codes are only 3 chars long */
            }
            തിരികെ currItem->currency;
        }
    }
    /* We enumerated too far. */
    എങ്കിൽ (resultLength) {
        *resultLength = 0;
    }
    തിരികെ nullptr;
}

സ്ഥിതം ശൂന്യം U_CALLCONV
ucurr_resetCurrencyList(UEnumeration *enumerator, UErrorCode * /*pErrorCode*/) {
    ((UCurrencyContext *)(enumerator->context))->listIdx = 0;
}

സ്ഥിതം ശൂന്യം U_CALLCONV
ucurr_closeCurrencyList(UEnumeration *enumerator) {
    uprv_free(enumerator->context);
    uprv_free(enumerator);
}

സ്ഥിതം ശൂന്യം U_CALLCONV
ucurr_createCurrencyList(UHashtable *isoCodes, UErrorCode* status){
    UErrorCode localStatus = U_ZERO_ERROR;

    // Look up the CurrencyMap element in the root bundle.
    UResourceBundle *rb = ures_openDirect(U_ICUDATA_CURR, CURRENCY_DATA, &localStatus);
    LocalUResourceBundlePointer currencyMapArray(ures_get_By_താക്കോൽ(rb, CURRENCY_MAP, rb, &localStatus));

    എങ്കിൽ (U_SUCCESS(localStatus)) {
        // process each entry in currency map
        വേണ്ടി (int32_t i=0; i<ures_getSize(currencyMapArray.getAlias()); i++) {
            // get the currency resource
            LocalUResourceBundlePointer currencyArray(ures_getByIndex(currencyMapArray.getAlias(), i, nullptr, &localStatus));
            // process each currency
            എങ്കിൽ (U_SUCCESS(localStatus)) {
                വേണ്ടി (int32_t j=0; j<ures_getSize(currencyArray.getAlias()); j++) {
                    // get the currency resource
                    LocalUResourceBundlePointer currencyRes(ures_getByIndex(currencyArray.getAlias(), j, nullptr, &localStatus));
                    IsoCodeEntry *entry = (IsoCodeEntry*)uprv_malloc(വലുപ്പം(IsoCodeEntry));
                    എങ്കിൽ (entry == nullptr) {
                        *status = U_MEMORY_ALLOCATION_ERROR;
                        തിരികെ;
                    }

                    // get the ISO code
                    int32_t isoLength = 0;
                    LocalUResourceBundlePointer idRes(ures_get_By_താക്കോൽ(currencyRes.getAlias(), "id", nullptr, &localStatus));
                    എങ്കിൽ (idRes.isNull()) {
                        തുടരുക;
                    }
                    സ്ഥിരം char16_t *isoCode = ures_getString(idRes.getAlias(), &isoLength, &localStatus);

                    // get from date
                    UDate fromDate = U_DATE_MIN;
                    LocalUResourceBundlePointer fromRes(ures_get_By_താക്കോൽ(currencyRes.getAlias(), "from", nullptr, &localStatus));

                    എങ്കിൽ (U_SUCCESS(localStatus)) {
                        int32_t fromLength = 0;
                        സ്ഥിരം int32_t *fromArray = ures_getIntVector(fromRes.getAlias(), &fromLength, &localStatus);
                        int64_t currDate64 = ((uint64_t)fromArray[0]) << 32;
                        currDate64 |= ((int64_t)fromArray[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                        fromDate = (UDate)currDate64;
                    }

                    // get to date
                    UDate toDate = U_DATE_MAX;
                    localStatus = U_ZERO_ERROR;
                    LocalUResourceBundlePointer toRes(ures_get_By_താക്കോൽ(currencyRes.getAlias(), "to", nullptr, &localStatus));

                    എങ്കിൽ (U_SUCCESS(localStatus)) {
                        int32_t toLength = 0;
                        സ്ഥിരം int32_t *toArray = ures_getIntVector(toRes.getAlias(), &toLength, &localStatus);
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
            } അല്ലെങ്കിൽ {
                *status = localStatus;
            }
        }
    } അല്ലെങ്കിൽ {
        *status = localStatus;
    }
}

സ്ഥിതം സ്ഥിരം UEnumeration gEnumCurrencyList = {
    nullptr,
    nullptr,
    ucurr_closeCurrencyList,
    ucurr_എണ്ണം_Currency_List,
    uenum_unextDefault,
    ucurr_nextCurrencyList,
    ucurr_resetCurrencyList
};
U_CDECL_END


സ്ഥിതം ശൂന്യം U_CALLCONV initIsoCodes(UErrorCode &status) {
    U_ASSERT(gIsoCodes == nullptr);
    ucln_common_registerCleanup(UCLN_COMMON_CURRENCY, currency_cleanup);

    LocalUHashtablePointer isoCodes(uhash_open(uhash_hashUChars, uhash_compareUChars, nullptr, &status));
    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ;
    }
    uhash_set_മൂല്യം_Deleter(isoCodes.getAlias(), deleteIsoCodeEntry);

    ucurr_createCurrencyList(isoCodes.getAlias(), &status);
    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ;
    }
    gIsoCodes = isoCodes.orphan();  // Note: gIsoCodes is const. Once set up here it is never altered,
                                    //       and read only access is safe without synchronization.
}

സ്ഥിതം ശൂന്യം populateCurrSymbolsEquiv(icu::Hashtable *hash, UErrorCode &status) {
    എങ്കിൽ (U_FAILURE(status)) { തിരികെ; }
    വേണ്ടി (സ്ഥിരം auto& entry : unisets::kCurrencyEntries) {
        UnicodeString exemplar(entry.exemplar);
        സ്ഥിരം UnicodeSet* set = unisets::get(entry.താക്കോൽ);
        എങ്കിൽ (set == nullptr) { തിരികെ; }
        UnicodeSetIterator it(*set);
        ആകുമ്പോൾ (it.next()) {
            UnicodeString value = it.getString();
            എങ്കിൽ (value == exemplar) {
                // No need to mark the exemplar character as an equivalent
                തുടരുക;
            }
            makeEquivalent(exemplar, value, hash, status);
            എങ്കിൽ (U_FAILURE(status)) { തിരികെ; }
        }
    }
}

സ്ഥിതം ശൂന്യം U_CALLCONV initCurrSymbolsEquiv() {
    U_ASSERT(gCurrSymbolsEquiv == nullptr);
    UErrorCode status = U_ZERO_ERROR;
    ucln_common_registerCleanup(UCLN_COMMON_CURRENCY, currency_cleanup);
    icu::Hashtable *temp = പുതിയ icu::Hashtable(status);
    എങ്കിൽ (temp == nullptr) {
        തിരികെ;
    }
    എങ്കിൽ (U_FAILURE(status)) {
        delete temp;
        തിരികെ;
    }
    temp->set_മൂല്യം_Deleter(deleteUnicode);
    populateCurrSymbolsEquiv(temp, status);
    എങ്കിൽ (U_FAILURE(status)) {
        delete temp;
        തിരികെ;
    }
    gCurrSymbolsEquiv = temp;
}

U_CAPI UBool U_EXPORT2
ucurr_isAvailable(സ്ഥിരം char16_t* isoCode, UDate from, UDate to, UErrorCode* eErrorCode) {
    umtx_initOnce(gIsoCodesInitOnce, &initIsoCodes, *eErrorCode);
    എങ്കിൽ (U_FAILURE(*eErrorCode)) {
        തിരികെ അസത്യം;
    }

    IsoCodeEntry* result = (IsoCodeEntry *) uhash_get(gIsoCodes, isoCode);
    എങ്കിൽ (result == nullptr) {
        തിരികെ അസത്യം;
    } അല്ലെങ്കിൽ എങ്കിൽ (from > to) {
        *eErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ അസത്യം;
    } അല്ലെങ്കിൽ എങ്കിൽ  ((from > result->to) || (to < result->from)) {
        തിരികെ അസത്യം;
    }
    തിരികെ സത്യം;
}

സ്ഥിതം സ്ഥിരം icu::Hashtable* getCurrSymbolsEquiv() {
    umtx_initOnce(gCurrSymbolsEquivInitOnce, &initCurrSymbolsEquiv);
    തിരികെ gCurrSymbolsEquiv;
}

U_CAPI UEnumeration * U_EXPORT2
ucurr_openISOCurrencies(uint32_t currType, UErrorCode *pErrorCode) {
    UEnumeration *myEnum = nullptr;
    UCurrencyContext *myContext;

    myEnum = (UEnumeration*)uprv_malloc(വലുപ്പം(UEnumeration));
    എങ്കിൽ (myEnum == nullptr) {
        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
        തിരികെ nullptr;
    }
    uprv_memcpy(myEnum, &gEnumCurrencyList, വലുപ്പം(UEnumeration));
    myContext = (UCurrencyContext*)uprv_malloc(വലുപ്പം(UCurrencyContext));
    എങ്കിൽ (myContext == nullptr) {
        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
        uprv_free(myEnum);
        തിരികെ nullptr;
    }
    myContext->currType = currType;
    myContext->listIdx = 0;
    myEnum->context = myContext;
    തിരികെ myEnum;
}

U_CAPI int32_t U_EXPORT2
ucurr_എണ്ണം_Currencies(സ്ഥിരം അക്ഷരം* locale, 
                 UDate date, 
                 UErrorCode* ec)
{
    int32_t curr_എണ്ണം = 0;

    എങ്കിൽ (ec != nullptr && U_SUCCESS(*ec)) 
    {
        // local variables
        UErrorCode localStatus = U_ZERO_ERROR;

        // get country or country_variant in `id'
        CharString id = idForLocale(locale, ec);

        എങ്കിൽ (U_FAILURE(*ec))
        {
            തിരികെ 0;
        }

        // Remove variants, which is only needed for registration.
        അക്ഷരം *idDelim = strchr(id.data(), VAR_DELIM);
        എങ്കിൽ (idDelim)
        {
            id.truncate(idDelim - id.data());
        }

        // Look up the CurrencyMap element in the root bundle.
        UResourceBundle *rb = ures_openDirect(U_ICUDATA_CURR, CURRENCY_DATA, &localStatus);
        UResourceBundle *cm = ures_get_By_താക്കോൽ(rb, CURRENCY_MAP, rb, &localStatus);

        // Using the id derived from the local, get the currency data
        LocalUResourceBundlePointer countryArray(ures_get_By_താക്കോൽ(rb, id.data(), cm, &localStatus));

        // process each currency to see which one is valid for the given date
        എങ്കിൽ (U_SUCCESS(localStatus))
        {
            വേണ്ടി (int32_t i=0; i<ures_getSize(countryArray.getAlias()); i++)
            {
                // get the currency resource
                LocalUResourceBundlePointer currencyRes(ures_getByIndex(countryArray.getAlias(), i, nullptr, &localStatus));

                // get the from date
                int32_t fromLength = 0;
                LocalUResourceBundlePointer fromRes(ures_get_By_താക്കോൽ(currencyRes.getAlias(), "from", nullptr, &localStatus));
                സ്ഥിരം int32_t *fromArray = ures_getIntVector(fromRes.getAlias(), &fromLength, &localStatus);

                int64_t currDate64 = (int64_t)((uint64_t)(fromArray[0]) << 32);
                currDate64 |= ((int64_t)fromArray[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                UDate fromDate = (UDate)currDate64;

                എങ്കിൽ (ures_getSize(currencyRes.getAlias())> 2)
                {
                    int32_t toLength = 0;
                    LocalUResourceBundlePointer toRes(ures_get_By_താക്കോൽ(currencyRes.getAlias(), "to", nullptr, &localStatus));
                    സ്ഥിരം int32_t *toArray = ures_getIntVector(toRes.getAlias(), &toLength, &localStatus);

                    currDate64 = (int64_t)toArray[0] << 32;
                    currDate64 |= ((int64_t)toArray[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                    UDate toDate = (UDate)currDate64;

                    എങ്കിൽ ((fromDate <= date) && (date < toDate))
                    {
                        curr_എണ്ണം++;
                    }
                }
                അല്ലെങ്കിൽ
                {
                    എങ്കിൽ (fromDate <= date)
                    {
                        curr_എണ്ണം++;
                    }
                }
            } // end For loop
        } // end if (U_SUCCESS(localStatus))

        // Check for errors
        എങ്കിൽ (*ec == U_ZERO_ERROR || localStatus != U_ZERO_ERROR)
        {
            // There is nothing to fallback to. 
            // Report the failure/warning if possible.
            *ec = localStatus;
        }

        എങ്കിൽ (U_SUCCESS(*ec))
        {
            // no errors
            തിരികെ curr_എണ്ണം;
        }
    }

    // If we got here, either error code is invalid or
    // some argument passed is no good.
    തിരികെ 0;
}

U_CAPI int32_t U_EXPORT2 
ucurr_forLocaleAndDate(സ്ഥിരം അക്ഷരം* locale, 
                UDate date, 
                int32_t index,
                char16_t* buff,
                int32_t buffCapacity, 
                UErrorCode* ec)
{
    int32_t resLen = 0;
	int32_t currIndex = 0;
    സ്ഥിരം char16_t* s = nullptr;

    എങ്കിൽ (ec != nullptr && U_SUCCESS(*ec))
    {
        // check the arguments passed
        എങ്കിൽ ((buff && buffCapacity) || !buffCapacity )
        {
            // local variables
            UErrorCode localStatus = U_ZERO_ERROR;

            // get country or country_variant in `id'
            CharString id = idForLocale(locale, ec);
            എങ്കിൽ (U_FAILURE(*ec))
            {
                തിരികെ 0;
            }

            // Remove variants, which is only needed for registration.
            അക്ഷരം *idDelim = strchr(id.data(), VAR_DELIM);
            എങ്കിൽ (idDelim)
            {
                id.truncate(idDelim - id.data());
            }

            // Look up the CurrencyMap element in the root bundle.
            UResourceBundle *rb = ures_openDirect(U_ICUDATA_CURR, CURRENCY_DATA, &localStatus);
            UResourceBundle *cm = ures_get_By_താക്കോൽ(rb, CURRENCY_MAP, rb, &localStatus);

            // Using the id derived from the local, get the currency data
            LocalUResourceBundlePointer countryArray(ures_get_By_താക്കോൽ(rb, id.data(), cm, &localStatus));

            // process each currency to see which one is valid for the given date
            തർക്കം matchFound = അസത്യം;
            എങ്കിൽ (U_SUCCESS(localStatus))
            {
                എങ്കിൽ ((index <= 0) || (index> ures_getSize(countryArray.getAlias())))
                {
                    // requested index is out of bounds
                    തിരികെ 0;
                }

                വേണ്ടി (int32_t i=0; i<ures_getSize(countryArray.getAlias()); i++)
                {
                    // get the currency resource
                    LocalUResourceBundlePointer currencyRes(ures_getByIndex(countryArray.getAlias(), i, nullptr, &localStatus));
                    s = ures_get_String_By_താക്കോൽ(currencyRes.getAlias(), "id", &resLen, &localStatus);

                    // get the from date
                    int32_t fromLength = 0;
                    LocalUResourceBundlePointer fromRes(ures_get_By_താക്കോൽ(currencyRes.getAlias(), "from", nullptr, &localStatus));
                    സ്ഥിരം int32_t *fromArray = ures_getIntVector(fromRes.getAlias(), &fromLength, &localStatus);

                    int64_t currDate64 = (int64_t)((uint64_t)fromArray[0] << 32);
                    currDate64 |= ((int64_t)fromArray[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                    UDate fromDate = (UDate)currDate64;

                    എങ്കിൽ (ures_getSize(currencyRes.getAlias()) > 2)
                    {
                        int32_t toLength = 0;
                        LocalUResourceBundlePointer toRes(ures_get_By_താക്കോൽ(currencyRes.getAlias(), "to", nullptr, &localStatus));
                        സ്ഥിരം int32_t *toArray = ures_getIntVector(toRes.getAlias(), &toLength, &localStatus);

                        currDate64 = (int64_t)toArray[0] << 32;
                        currDate64 |= ((int64_t)toArray[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                        UDate toDate = (UDate)currDate64;

                        എങ്കിൽ ((fromDate <= date) && (date < toDate))
                        {
                            currIndex++;
                            എങ്കിൽ (currIndex == index)
                            {
                                matchFound = സത്യം;
                            }
                        }
                    }
                    അല്ലെങ്കിൽ
                    {
                        എങ്കിൽ (fromDate <= date)
                        {
                            currIndex++;
                            എങ്കിൽ (currIndex == index)
                            {
                                matchFound = സത്യം;
                            }
                        }
                    }
                    // check for loop exit
                    എങ്കിൽ (matchFound)
                    {
                        നിർത്തുക;
                    }

                } // end For loop
            }

            // Check for errors
            എങ്കിൽ (*ec == U_ZERO_ERROR || localStatus != U_ZERO_ERROR)
            {
                // There is nothing to fallback to. 
                // Report the failure/warning if possible.
                *ec = localStatus;
            }

            എങ്കിൽ (U_SUCCESS(*ec))
            {
                // no errors
                എങ്കിൽ((buffCapacity> resLen) && matchFound)
                {
                    // write out the currency value
                    u_strcpy(buff, s);
                }
                അല്ലെങ്കിൽ
                {
                    തിരികെ 0;
                }
            }

            // return null terminated currency string
            തിരികെ u_terminateUChars(buff, buffCapacity, resLen, ec);
        }
        അല്ലെങ്കിൽ
        {
            // illegal argument encountered
            *ec = U_ILLEGAL_ARGUMENT_ERROR;
        }

    }

    // If we got here, either error code is invalid or
    // some argument passed is no good.
    തിരികെ resLen;
}

സ്ഥിതം സ്ഥിരം UEnumeration default_Keyword_മൂല്യങ്ങൾ = {
    nullptr,
    nullptr,
    ulist_close_keyword_മൂല്യങ്ങൾ_iterator,
    ulist_എണ്ണം_keyword_മൂല്യങ്ങൾ,
    uenum_unextDefault,
    ulist_next_keyword_മൂല്യം, 
    ulist_reset_keyword_മൂല്യങ്ങൾ_iterator
};

U_CAPI UEnumeration *U_EXPORT2 ucurr_get_Keyword_മൂല്യങ്ങൾ_For_Locale(സ്ഥിരം അക്ഷരം *താക്കോൽ, സ്ഥിരം അക്ഷരം *locale, UBool commonlyUsed, UErrorCode* status) {
    // Resolve region
    CharString prefRegion = ulocimp_getRegionForSupplementalData(locale, സത്യം, *status);

    // Read value from supplementalData
    UList *മൂല്യങ്ങൾ = ulist_createEmptyList(status);
    UList *other_മൂല്യങ്ങൾ = ulist_createEmptyList(status);
    UEnumeration *en = (UEnumeration *)uprv_malloc(വലുപ്പം(UEnumeration));
    എങ്കിൽ (U_FAILURE(*status) || en == nullptr) {
        എങ്കിൽ (en == nullptr) {
            *status = U_MEMORY_ALLOCATION_ERROR;
        } അല്ലെങ്കിൽ {
            uprv_free(en);
        }
        ulist_deleteList(മൂല്യങ്ങൾ);
        ulist_deleteList(other_മൂല്യങ്ങൾ);
        തിരികെ nullptr;
    }
    memcpy(en, &default_Keyword_മൂല്യങ്ങൾ, വലുപ്പം(UEnumeration));
    en->context = മൂല്യങ്ങൾ;
    
    UResourceBundle* rb = ures_openDirect(U_ICUDATA_CURR, "supplementalData", status);
    LocalUResourceBundlePointer bundle(ures_get_By_താക്കോൽ(rb, "CurrencyMap", rb, status));
    അട്ടി_UResource_Bundle bundlekey, regbndl, curbndl, to;
    
    ആകുമ്പോൾ (U_SUCCESS(*status) && ures_hasNext(bundle.getAlias())) {
        ures_getNextResource(bundle.getAlias(), bundlekey.getAlias(), status);
        എങ്കിൽ (U_FAILURE(*status)) {
            നിർത്തുക;
        }
        സ്ഥിരം അക്ഷരം *region = ures_get_താക്കോൽ(bundlekey.getAlias());
        UBool isPrefRegion = prefRegion == region;
        എങ്കിൽ (!isPrefRegion && commonlyUsed) {
            // With commonlyUsed=true, we do not put
            // currencies for other regions in the
            // result list.
            തുടരുക;
        }
        ures_get_By_താക്കോൽ(bundle.getAlias(), region, regbndl.getAlias(), status);
        എങ്കിൽ (U_FAILURE(*status)) {
            നിർത്തുക;
        }
        ആകുമ്പോൾ (U_SUCCESS(*status) && ures_hasNext(regbndl.getAlias())) {
            ures_getNextResource(regbndl.getAlias(), curbndl.getAlias(), status);
            എങ്കിൽ (ures_getType(curbndl.getAlias()) != URES_TABLE) {
                // Currently, an empty ARRAY is mixed in.
                തുടരുക;
            }
            അക്ഷരം *curID = (അക്ഷരം *)uprv_malloc(വലുപ്പം(അക്ഷരം) * ULOC_KEYWORDS_CAPACITY);
            എങ്കിൽ (curID == nullptr) {
                *status = U_MEMORY_ALLOCATION_ERROR;
                നിർത്തുക;
            }
            int32_t curIDLength = ULOC_KEYWORDS_CAPACITY;

#എങ്കിൽ U_CHARSET_FAMILY==U_ASCII_FAMILY
            ures_get_UTF8_String_By_താക്കോൽ(curbndl.getAlias(), "id", curID, &curIDLength, സത്യം, status);
            /* optimize - use the utf-8 string */
#അല്ലെങ്കിൽ
            {
                       സ്ഥിരം char16_t* defString = ures_get_String_By_താക്കോൽ(curbndl.getAlias(), "id", &curIDLength, status);
                       എങ്കിൽ(U_SUCCESS(*status)) {
			   എങ്കിൽ(curIDLength+1 > ULOC_KEYWORDS_CAPACITY) {
				*status = U_BUFFER_OVERFLOW_ERROR;
			   } അല്ലെങ്കിൽ {
                           	u_UCharsToChars(defString, curID, curIDLength+1);
			   }
                       }
            }
#endif	

            എങ്കിൽ (U_FAILURE(*status)) {
                നിർത്തുക;
            }
            UBool hasTo = അസത്യം;
            ures_get_By_താക്കോൽ(curbndl.getAlias(), "to", to.getAlias(), status);
            എങ്കിൽ (U_FAILURE(*status)) {
                // Do nothing here...
                *status = U_ZERO_ERROR;
            } അല്ലെങ്കിൽ {
                hasTo = സത്യം;
            }
            എങ്കിൽ (isPrefRegion && !hasTo && !ulist_containsString(മൂല്യങ്ങൾ, curID, (int32_t)uprv_strlen(curID))) {
                // Currently active currency for the target country
                ulist_addItemEndList(മൂല്യങ്ങൾ, curID, സത്യം, status);
            } അല്ലെങ്കിൽ എങ്കിൽ (!ulist_containsString(other_മൂല്യങ്ങൾ, curID, (int32_t)uprv_strlen(curID)) && !commonlyUsed) {
                ulist_addItemEndList(other_മൂല്യങ്ങൾ, curID, സത്യം, status);
            } അല്ലെങ്കിൽ {
                uprv_free(curID);
            }
        }
        
    }
    എങ്കിൽ (U_SUCCESS(*status)) {
        എങ്കിൽ (commonlyUsed) {
            എങ്കിൽ (ulist_getListSize(മൂല്യങ്ങൾ) == 0) {
                // This could happen if no valid region is supplied in the input
                // locale. In this case, we use the CLDR's default.
                uenum_close(en);
                en = ucurr_get_Keyword_മൂല്യങ്ങൾ_For_Locale(താക്കോൽ, "und", സത്യം, status);
            }
        } അല്ലെങ്കിൽ {
            // Consolidate the list
            അക്ഷരം *value = nullptr;
            ulist_resetList(other_മൂല്യങ്ങൾ);
            ആകുമ്പോൾ ((value = (അക്ഷരം *)ulist_getNext(other_മൂല്യങ്ങൾ)) != nullptr) {
                എങ്കിൽ (!ulist_containsString(മൂല്യങ്ങൾ, value, (int32_t)uprv_strlen(value))) {
                    അക്ഷരം *tmp_മൂല്യം = (അക്ഷരം *)uprv_malloc(വലുപ്പം(അക്ഷരം) * ULOC_KEYWORDS_CAPACITY);
                    എങ്കിൽ (tmp_മൂല്യം == nullptr) {
                        *status = U_MEMORY_ALLOCATION_ERROR;
                        നിർത്തുക;
                    }
                    uprv_memcpy(tmp_മൂല്യം, value, uprv_strlen(value) + 1);
                    ulist_addItemEndList(മൂല്യങ്ങൾ, tmp_മൂല്യം, സത്യം, status);
                    എങ്കിൽ (U_FAILURE(*status)) {
                        നിർത്തുക;
                    }
                }
            }
        }
        ulist_resetList((UList *)(en->context));
    } അല്ലെങ്കിൽ {
        ulist_deleteList(മൂല്യങ്ങൾ);
        uprv_free(en);
        മൂല്യങ്ങൾ = nullptr;
        en = nullptr;
    }
    ulist_deleteList(other_മൂല്യങ്ങൾ);
    തിരികെ en;
}


U_CAPI int32_t U_EXPORT2
ucurr_getNumericCode(സ്ഥിരം char16_t* currency) {
    int32_t code = 0;
    എങ്കിൽ (currency && u_strlen(currency) == ISO_CURRENCY_CODE_LENGTH) {
        UErrorCode status = U_ZERO_ERROR;

        UResourceBundle* bundle = ures_openDirect(nullptr, "currencyNumericCodes", &status);
        LocalUResourceBundlePointer codeMap(ures_get_By_താക്കോൽ(bundle, "codeMap", bundle, &status));
        എങ്കിൽ (U_SUCCESS(status)) {
            അക്ഷരം alphaCode[ISO_CURRENCY_CODE_LENGTH+1];
            myUCharsToChars(alphaCode, currency);
            T_CString_toUpperCase(alphaCode);
            ures_get_By_താക്കോൽ(codeMap.getAlias(), alphaCode, codeMap.getAlias(), &status);
            പൂർണ്ണസംഖ്യ tmpCode = ures_getInt(codeMap.getAlias(), &status);
            എങ്കിൽ (U_SUCCESS(status)) {
                code = tmpCode;
            }
        }
    }
    തിരികെ code;
}
#endif /* #if !UCONFIG_NO_FORMATTING */

//eof
