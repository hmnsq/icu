// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
* Copyright (c) 2002-2016, International Business Machines
* Corporation and others.  All Rights Reserved.
**********************************************************************
*/

#அந்தர்பாவ "unicode/utypes.h"

#எனில் !UCONFIG_NO_FORMATTING

#அந்தர்பாவ <utility>

#அந்தர்பாவ "unicode/ucurr.h"
#அந்தர்பாவ "unicode/locid.h"
#அந்தர்பாவ "unicode/ures.h"
#அந்தர்பாவ "unicode/ustring.h"
#அந்தர்பாவ "unicode/parsepos.h"
#அந்தர்பாவ "unicode/uniset.h"
#அந்தர்பாவ "unicode/usetiter.h"
#அந்தர்பாவ "unicode/utf16.h"
#அந்தர்பாவ "ustr_imp.h"
#அந்தர்பாவ "charstr.h"
#அந்தர்பாவ "cmemory.h"
#அந்தர்பாவ "cstring.h"
#அந்தர்பாவ "static_unicode_sets.h"
#அந்தர்பாவ "uassert.h"
#அந்தர்பாவ "umutex.h"
#அந்தர்பாவ "ucln_cmn.h"
#அந்தர்பாவ "uenumimp.h"
#அந்தர்பாவ "uhash.h"
#அந்தர்பாவ "hash.h"
#அந்தர்பாவ "uinvchar.h"
#அந்தர்பாவ "uresimp.h"
#அந்தர்பாவ "ulist.h"
#அந்தர்பாவ "uresimp.h"
#அந்தர்பாவ "ureslocs.h"
#அந்தர்பாவ "ulocimp.h"

using நாமாகாஶ icu;

//#define UCURR_DEBUG_EQUIV 1
#ifdef UCURR_DEBUG_EQUIV
#அந்தர்பாவ "stdio.h"
#endif
//#define UCURR_DEBUG 1
#ifdef UCURR_DEBUG
#அந்தர்பாவ "stdio.h"
#endif

வகைவரை கட்டமைப்பு IsoCodeEntry {
    மாறிலி char16_t *isoCode; /* const because it's a reference to a resource bundle string. */
    UDate from;
    UDate to;
} IsoCodeEntry;

//------------------------------------------------------------
// Constants

// Default currency meta data of last resort.  We try to use the
// defaults encoded in the meta data resource bundle.  If there is a
// configuration/build error and these are not available, we use these
// hard-coded defaults (which should be identical).
நிலையான மாறிலி int32_t LAST_RESORT_DATA[] = { 2, 0, 2, 0 };

// POW10[i] = 10^i, i=0..MAX_POW10
நிலையான மாறிலி int32_t POW10[] = { 1, 10, 100, 1000, 10000, 100000,
                                 1000000, 10000000, 100000000, 1000000000 };

நிலையான மாறிலி int32_t MAX_POW10 = UPRV_LENGTHOF(POW10) - 1;

#define ISO_CURRENCY_CODE_LENGTH 3

//------------------------------------------------------------
// Resource tags
//

நிலையான மாறிலி எழுத்து CURRENCY_DATA[] = "supplementalData";
// Tag for meta-data, in root.
நிலையான மாறிலி எழுத்து CURRENCY_META[] = "CurrencyMeta";

// Tag for map from countries to currencies, in root.
நிலையான மாறிலி எழுத்து CURRENCY_MAP[] = "CurrencyMap";

// Tag for default meta-data, in CURRENCY_META
நிலையான மாறிலி எழுத்து DEFAULT_META[] = "DEFAULT";

// Variant delimiter
நிலையான மாறிலி எழுத்து VAR_DELIM = '_';

// Tag for localized display names (symbols) of currencies
நிலையான மாறிலி எழுத்து CURRENCIES[] = "Currencies";
நிலையான மாறிலி எழுத்து CURRENCIES_NARROW[] = "Currencies%narrow";
நிலையான மாறிலி எழுத்து CURRENCIES_FORMAL[] = "Currencies%formal";
நிலையான மாறிலி எழுத்து CURRENCIES_VARIANT[] = "Currencies%variant";
நிலையான மாறிலி எழுத்து CURRENCYPLURALS[] = "CurrencyPlurals";

// ISO codes mapping table
நிலையான மாறிலி UHashtable* gIsoCodes = nullptr;
நிலையான icu::UInitOnce gIsoCodesInitOnce {};

// Currency symbol equivalances
நிலையான மாறிலி icu::Hashtable* gCurrSymbolsEquiv = nullptr;
நிலையான icu::UInitOnce gCurrSymbolsEquivInitOnce {};

U_NAMESPACE_BEGIN

// EquivIterator iterates over all strings that are equivalent to a given
// string, s. Note that EquivIterator will never yield s itself.
வகுப்பு EquivIterator : பொது icu::UMemory {
பொது:
    // Constructor. hash stores the equivalence relationships; s is the string
    // for which we find equivalent strings.
    உள்வரி EquivIterator(மாறிலி icu::Hashtable& hash, மாறிலி icu::UnicodeString& s)
        : _hash(hash) { 
        _start = _current = &s;
    }
    உள்வரி ~EquivIterator() { }

    // next returns the next equivalent string or nullptr if there are no more.
    // If s has no equivalent strings, next returns nullptr on the first call.
    மாறிலி icu::UnicodeString *next();
தனிப்பட்ட:
    மாறிலி icu::Hashtable& _hash;
    மாறிலி icu::UnicodeString* _start;
    மாறிலி icu::UnicodeString* _current;
};

மாறிலி icu::UnicodeString *
EquivIterator::next() {
    மாறிலி icu::UnicodeString* _next = static_cast<மாறிலி icu::UnicodeString*>(_hash.get(*_current));
    எனில் (_next == nullptr) {
        U_ASSERT(_current == _start);
        திரும்பு nullptr;
    }
    எனில் (*_next == *_start) {
        திரும்பு nullptr;
    }
    _current = _next;
    திரும்பு _next;
}

U_NAMESPACE_END

// makeEquivalent makes lhs and rhs equivalent by updating the equivalence
// relations in hash accordingly.
நிலையான வெறுமை makeEquivalent(
    மாறிலி icu::UnicodeString &lhs,
    மாறிலி icu::UnicodeString &rhs,
    icu::Hashtable* hash, UErrorCode &status) {
    எனில் (U_FAILURE(status)) {
        திரும்பு;
    }
    எனில் (lhs == rhs) {
        // already equivalent
        திரும்பு;
    }
    icu::EquivIterator இடது_Iter(*hash, lhs);
    icu::EquivIterator வலது_Iter(*hash, rhs);
    மாறிலி icu::UnicodeString *first_இடது = இடது_Iter.next();
    மாறிலி icu::UnicodeString *first_வலது = வலது_Iter.next();
    மாறிலி icu::UnicodeString *next_இடது = first_இடது;
    மாறிலி icu::UnicodeString *next_வலது = first_வலது;
    போது (next_இடது != nullptr && next_வலது != nullptr) {
        எனில் (*next_இடது == rhs || *next_வலது == lhs) {
            // Already equivalent
            திரும்பு;
        }
        next_இடது = இடது_Iter.next();
        next_வலது = வலது_Iter.next();
    }
    // Not equivalent. Must join.
    icu::UnicodeString *new_First_இடது;
    icu::UnicodeString *new_First_வலது;
    எனில் (first_வலது == nullptr && first_இடது == nullptr) {
        // Neither lhs or rhs belong to an equivalence circle, so we form
        // a new equivalnce circle of just lhs and rhs.
        new_First_இடது = புதிய icu::UnicodeString(rhs);
        new_First_வலது = புதிய icu::UnicodeString(lhs);
    } இல்லையெனில் எனில் (first_வலது == nullptr) {
        // lhs belongs to an equivalence circle, but rhs does not, so we link
        // rhs into lhs' circle.
        new_First_இடது = புதிய icu::UnicodeString(rhs);
        new_First_வலது = புதிய icu::UnicodeString(*first_இடது);
    } இல்லையெனில் எனில் (first_இடது == nullptr) {
        // rhs belongs to an equivlance circle, but lhs does not, so we link
        // lhs into rhs' circle.
        new_First_இடது = புதிய icu::UnicodeString(*first_வலது);
        new_First_வலது = புதிய icu::UnicodeString(lhs);
    } இல்லையெனில் {
        // Both lhs and rhs belong to different equivalnce circles. We link
        // them together to form one single, larger equivalnce circle.
        new_First_இடது = புதிய icu::UnicodeString(*first_வலது);
        new_First_வலது = புதிய icu::UnicodeString(*first_இடது);
    }
    எனில் (new_First_இடது == nullptr || new_First_வலது == nullptr) {
        delete new_First_இடது;
        delete new_First_வலது;
        status = U_MEMORY_ALLOCATION_ERROR;
        திரும்பு;
    }
    hash->put(lhs, (வெறுமை *) new_First_இடது, status);
    hash->put(rhs, (வெறுமை *) new_First_வலது, status);
}

// countEquivalent counts how many strings are equivalent to s.
// hash stores all the equivalnce relations.
// countEquivalent does not include s itself in the count.
நிலையான int32_t மொத்தம்_Equivalent(மாறிலி icu::Hashtable &hash, மாறிலி icu::UnicodeString &s) {
    int32_t result = 0;
    icu::EquivIterator iter(hash, s);
    போது (iter.next() != nullptr) {
        ++result;
    }
#ifdef UCURR_DEBUG_EQUIV
 {
   எழுத்து tmp[200];
   s.extract(0,s.length(),tmp, "UTF-8");
   printf("CountEquivalent('%s') = %d\n", tmp, result);
 }
#endif
    திரும்பு result;
}

நிலையான மாறிலி icu::Hashtable* getCurrSymbolsEquiv();

//------------------------------------------------------------
// Code

/**
 * Cleanup callback func
 */
நிலையான UBool U_CALLCONV 
isoCodes_cleanup()
{
    எனில் (gIsoCodes != nullptr) {
        uhash_close(const_cast<UHashtable *>(gIsoCodes));
        gIsoCodes = nullptr;
    }
    gIsoCodesInitOnce.reset();
    திரும்பு உண்மை;
}

/**
 * Cleanup callback func
 */
நிலையான UBool U_CALLCONV 
currSymbolsEquiv_cleanup()
{
    delete const_cast<icu::Hashtable *>(gCurrSymbolsEquiv);
    gCurrSymbolsEquiv = nullptr;
    gCurrSymbolsEquivInitOnce.reset();
    திரும்பு உண்மை;
}

/**
 * Deleter for IsoCodeEntry
 */
நிலையான வெறுமை U_CALLCONV
deleteIsoCodeEntry(வெறுமை *obj) {
    IsoCodeEntry* entry = static_cast<IsoCodeEntry*>(obj);
    uprv_free(entry);
}

/**
 * Deleter for gCurrSymbolsEquiv.
 */
நிலையான வெறுமை U_CALLCONV
deleteUnicode(வெறுமை *obj) {
    icu::UnicodeString* entry = static_cast<icu::UnicodeString*>(obj);
    delete entry;
}

/**
 * Unfortunately, we have to convert the char16_t* currency code to char*
 * to use it as a resource key.
 */
நிலையான உள்வரி எழுத்து*
myUCharsToChars(எழுத்து* resultOfLen4, மாறிலி char16_t* currency) {
    u_UCharsToChars(currency, resultOfLen4, ISO_CURRENCY_CODE_LENGTH);
    resultOfLen4[ISO_CURRENCY_CODE_LENGTH] = 0;
    திரும்பு resultOfLen4;
}

/**
 * Internal function to look up currency data.  Result is an array of
 * four integers.  The first is the fraction digits.  The second is the
 * rounding increment, or 0 if none.  The rounding increment is in
 * units of 10^(-fraction_digits).  The third and fourth are the same
 * except that they are those used in cash transactions ( cashDigits
 * and cashRounding ).
 */
நிலையான மாறிலி int32_t*
_findMetaData(மாறிலி char16_t* currency, UErrorCode& ec) {

    எனில் (currency == nullptr || *currency == 0) {
        எனில் (U_SUCCESS(ec)) {
            ec = U_ILLEGAL_ARGUMENT_ERROR;
        }
        திரும்பு LAST_RESORT_DATA;
    }

    // Get CurrencyMeta resource out of root locale file.  [This may
    // move out of the root locale file later; if it does, update this
    // code.]
    UResourceBundle* currencyData = ures_openDirect(U_ICUDATA_CURR, CURRENCY_DATA, &ec);
    LocalUResourceBundlePointer currencyMeta(ures_get_By_சாவி(currencyData, CURRENCY_META, currencyData, &ec));

    எனில் (U_FAILURE(ec)) {
        // Config/build error; return hard-coded defaults
        திரும்பு LAST_RESORT_DATA;
    }

    // Look up our currency, or if that's not available, then DEFAULT
    எழுத்து buf[ISO_CURRENCY_CODE_LENGTH+1];
    UErrorCode ec2 = U_ZERO_ERROR; // local error code: soft failure
    LocalUResourceBundlePointer rb(ures_get_By_சாவி(currencyMeta.getAlias(), myUCharsToChars(buf, currency), nullptr, &ec2));
      எனில் (U_FAILURE(ec2)) {
        rb.adoptInstead(ures_get_By_சாவி(currencyMeta.getAlias(),DEFAULT_META, nullptr, &ec));
        எனில் (U_FAILURE(ec)) {
            // Config/build error; return hard-coded defaults
            திரும்பு LAST_RESORT_DATA;
        }
    }

    int32_t len;
    மாறிலி int32_t *data = ures_getIntVector(rb.getAlias(), &len, &ec);
    எனில் (U_FAILURE(ec) || len != 4) {
        // Config/build error; return hard-coded defaults
        எனில் (U_SUCCESS(ec)) {
            ec = U_INVALID_FORMAT_ERROR;
        }
        திரும்பு LAST_RESORT_DATA;
    }

    திரும்பு data;
}

// -------------------------------------

நிலையான CharString
idForLocale(மாறிலி எழுத்து* locale, UErrorCode* ec)
{
    திரும்பு ulocimp_getRegionForSupplementalData(locale, பொய், *ec);
}

// ------------------------------------------
//
// Registration
//
//-------------------------------------------

// don't use ICUService since we don't need fallback

U_CDECL_BEGIN
நிலையான UBool U_CALLCONV currency_cleanup();
U_CDECL_END

#எனில் !UCONFIG_NO_SERVICE
கட்டமைப்பு CReg;

நிலையான UMutex gCRegLock;
நிலையான CReg* gCRegHead = nullptr;

கட்டமைப்பு CReg : பொது icu::UMemory {
    CReg *next;
    char16_t iso[ISO_CURRENCY_CODE_LENGTH+1];
    எழுத்து  id[ULOC_FULLNAME_CAPACITY];

    CReg(மாறிலி char16_t* _iso, மாறிலி எழுத்து* _id)
        : next(nullptr)
    {
        uprv_strncpy(id, _id, அளவு(id)-1);
        id[அளவு(id)-1] = 0;
        u_memcpy(iso, _iso, ISO_CURRENCY_CODE_LENGTH);
        iso[ISO_CURRENCY_CODE_LENGTH] = 0;
    }

    நிலையான UCurr_Registry_சாவி reg(மாறிலி char16_t* _iso, மாறிலி எழுத்து* _id, UErrorCode* status)
    {
        எனில் (status && U_SUCCESS(*status) && _iso && _id) {
            CReg* n = புதிய CReg(_iso, _id);
            எனில் (n) {
                umtx_lock(&gCRegLock);
                எனில் (!gCRegHead) {
                    /* register for the first time */
                    ucln_common_registerCleanup(UCLN_COMMON_CURRENCY, currency_cleanup);
                }
                n->next = gCRegHead;
                gCRegHead = n;
                umtx_unlock(&gCRegLock);
                திரும்பு n;
            }
            *status = U_MEMORY_ALLOCATION_ERROR;
        }
        திரும்பு nullptr;
    }

    நிலையான UBool unreg(UCurr_Registry_சாவி சாவி) {
        UBool found = பொய்;
        umtx_lock(&gCRegLock);

        CReg** p = &gCRegHead;
        போது (*p) {
            எனில் (*p == சாவி) {
                *p = ((CReg*)சாவி)->next;
                delete (CReg*)சாவி;
                found = உண்மை;
                நிறுத்து;
            }
            p = &((*p)->next);
        }

        umtx_unlock(&gCRegLock);
        திரும்பு found;
    }

    நிலையான மாறிலி char16_t* get(மாறிலி எழுத்து* id) {
        மாறிலி char16_t* result = nullptr;
        umtx_lock(&gCRegLock);
        CReg* p = gCRegHead;

        /* register cleanup of the mutex */
        ucln_common_registerCleanup(UCLN_COMMON_CURRENCY, currency_cleanup);
        போது (p) {
            எனில் (uprv_strcmp(id, p->id) == 0) {
                result = p->iso;
                நிறுத்து;
            }
            p = p->next;
        }
        umtx_unlock(&gCRegLock);
        திரும்பு result;
    }

    /* This doesn't need to be thread safe. It's for u_cleanup only. */
    நிலையான வெறுமை cleanup() {
        போது (gCRegHead) {
            CReg* n = gCRegHead;
            gCRegHead = gCRegHead->next;
            delete n;
        }
    }
};

// -------------------------------------

U_CAPI UCurr_Registry_சாவி U_EXPORT2
ucurr_register(மாறிலி char16_t* isoCode, மாறிலி எழுத்து* locale, UErrorCode *status)
{
    எனில் (status && U_SUCCESS(*status)) {
        CharString id = idForLocale(locale, status);
        திரும்பு CReg::reg(isoCode, id.data(), status);
    }
    திரும்பு nullptr;
}

// -------------------------------------

U_CAPI UBool U_EXPORT2
ucurr_unregister(UCurr_Registry_சாவி சாவி, UErrorCode* status)
{
    எனில் (status && U_SUCCESS(*status)) {
        திரும்பு CReg::unreg(சாவி);
    }
    திரும்பு பொய்;
}
#endif /* UCONFIG_NO_SERVICE */

// -------------------------------------

/**
 * Release all static memory held by currency.
 */
/*The declaration here is needed so currency_cleanup()
 * can call this function.
 */
நிலையான UBool U_CALLCONV
currency_cache_cleanup();

U_CDECL_BEGIN
நிலையான UBool U_CALLCONV currency_cleanup() {
#எனில் !UCONFIG_NO_SERVICE
    CReg::cleanup();
#endif
    /*
     * There might be some cached currency data or isoCodes data.
     */
    currency_cache_cleanup();
    isoCodes_cleanup();
    currSymbolsEquiv_cleanup();

    திரும்பு உண்மை;
}
U_CDECL_END

// -------------------------------------

U_CAPI int32_t U_EXPORT2
ucurr_forLocale(மாறிலி எழுத்து* locale,
                char16_t* buff,
                int32_t buffCapacity,
                UErrorCode* ec) {
    எனில் (U_FAILURE(*ec)) { திரும்பு 0; }
    எனில் (buffCapacity < 0 || (buff == nullptr && buffCapacity > 0)) {
        *ec = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு 0;
    }

    UErrorCode localStatus = U_ZERO_ERROR;
    CharString currency = ulocimp_get_Keyword_மதிப்பு(locale, "currency", localStatus);
    int32_t resLen = currency.length();

    எனில் (U_SUCCESS(localStatus) && resLen == 3 && uprv_isInvariantString(currency.data(), resLen)) {
        எனில் (resLen < buffCapacity) {
            T_CString_toUpperCase(currency.data());
            u_charsToUChars(currency.data(), buff, resLen);
        }
        திரும்பு u_terminateUChars(buff, buffCapacity, resLen, ec);
    }

    // get country or country_variant in `id'
    CharString id = idForLocale(locale, ec);
    எனில் (U_FAILURE(*ec)) {
        திரும்பு 0;
    }

#எனில் !UCONFIG_NO_SERVICE
    மாறிலி char16_t* result = CReg::get(id.data());
    எனில் (result) {
        எனில்(buffCapacity > u_strlen(result)) {
            u_strcpy(buff, result);
        }
        resLen = u_strlen(result);
        திரும்பு u_terminateUChars(buff, buffCapacity, resLen, ec);
    }
#endif
    // Remove variants, which is only needed for registration.
    எழுத்து *idDelim = uprv_strchr(id.data(), VAR_DELIM);
    எனில் (idDelim) {
        id.truncate(idDelim - id.data());
    }

    மாறிலி char16_t* s = nullptr;  // Currency code from data file.
    எனில் (id.isEmpty()) {
        // No point looking in the data for an empty string.
        // This is what we would get.
        localStatus = U_MISSING_RESOURCE_ERROR;
    } இல்லையெனில் {
        // Look up the CurrencyMap element in the root bundle.
        localStatus = U_ZERO_ERROR;
        UResourceBundle *rb = ures_openDirect(U_ICUDATA_CURR, CURRENCY_DATA, &localStatus);
        UResourceBundle *cm = ures_get_By_சாவி(rb, CURRENCY_MAP, rb, &localStatus);
        LocalUResourceBundlePointer countryArray(ures_get_By_சாவி(rb, id.data(), cm, &localStatus));
        // https://unicode-org.atlassian.net/browse/ICU-21997
        // Prefer to use currencies that are legal tender.
        எனில் (U_SUCCESS(localStatus)) {
            int32_t arrayLength = ures_getSize(countryArray.getAlias());
            க்கு (int32_t i = 0; i < arrayLength; ++i) {
                LocalUResourceBundlePointer currencyReq(
                    ures_getByIndex(countryArray.getAlias(), i, nullptr, &localStatus));
                // The currency is legal tender if it is *not* marked with tender{"false"}.
                UErrorCode tenderStatus = localStatus;
                மாறிலி char16_t *tender =
                    ures_get_String_By_சாவி(currencyReq.getAlias(), "tender", nullptr, &tenderStatus);
                தருக்க isTender = U_FAILURE(tenderStatus) || u_strcmp(tender, u"false") != 0;
                எனில் (!isTender && s != nullptr) {
                    // We already have a non-tender currency. Ignore all following non-tender ones.
                    தொடர்;
                }
                // Fetch the currency code.
                s = ures_get_String_By_சாவி(currencyReq.getAlias(), "id", &resLen, &localStatus);
                எனில் (isTender) {
                    நிறுத்து;
                }
            }
            எனில் (U_SUCCESS(localStatus) && s == nullptr) {
                localStatus = U_MISSING_RESOURCE_ERROR;
            }
        }
    }

    எனில் ((U_FAILURE(localStatus)) && strchr(id.data(), '_') != nullptr) {
        // We don't know about it.  Check to see if we support the variant.
        CharString parent = ulocimp_getParent(locale, *ec);
        *ec = U_USING_FALLBACK_WARNING;
        // TODO: Loop over the parent rather than recursing and
        // looking again for a currency keyword.
        திரும்பு ucurr_forLocale(parent.data(), buff, buffCapacity, ec);
    }
    எனில் (*ec == U_ZERO_ERROR || localStatus != U_ZERO_ERROR) {
        // There is nothing to fallback to. Report the failure/warning if possible.
        *ec = localStatus;
    }
    எனில் (U_SUCCESS(*ec)) {
        எனில்(buffCapacity > resLen) {
            u_strcpy(buff, s);
        }
    }
    திரும்பு u_terminateUChars(buff, buffCapacity, resLen, ec);
}

// end registration

/**
 * Modify the given locale name by removing the rightmost _-delimited
 * element.  If there is none, empty the string ("" == root).
 * NOTE: The string "root" is not recognized; do not use it.
 * @return true if the fallback happened; false if locale is already
 * root ("").
 */
நிலையான UBool fallback(CharString& loc) {
    எனில் (loc.isEmpty()) {
        திரும்பு பொய்;
    }
    UErrorCode status = U_ZERO_ERROR;
    எனில் (loc == "en_GB") {
        // HACK: See #13368.  We need "en_GB" to fall back to "en_001" instead of "en"
        // in order to consume the correct data strings.  This hack will be removed
        // when proper data sink loading is implemented here.
        loc.truncate(3);
        loc.append("001", status);
    } இல்லையெனில் {
        loc = ulocimp_getParent(loc.data(), status);
    }
 /*
    char *i = uprv_strrchr(loc, '_');
    if (i == nullptr) {
        i = loc;
    }
    *i = 0;
 */
    திரும்பு உண்மை;
}


U_CAPI மாறிலி char16_t* U_EXPORT2
ucurr_getName(மாறிலி char16_t* currency U_LIFETIME_BOUND,
              மாறிலி எழுத்து* locale,
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

    எனில் (U_FAILURE(*ec)) {
        திரும்பு nullptr;
    }

    int32_t choice = (int32_t) nameStyle;
    எனில் (choice < 0 || choice > 4) {
        *ec = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு nullptr;
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

    எனில் (locale == nullptr) {
        locale = uloc_getDefault();
    }
    CharString loc = ulocimp_getName(locale, ec2);
    எனில் (U_FAILURE(ec2)) {
        *ec = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு nullptr;
    }

    எழுத்து buf[ISO_CURRENCY_CODE_LENGTH+1];
    myUCharsToChars(buf, currency);
    
    /* Normalize the keyword value to uppercase */
    T_CString_toUpperCase(buf);
    
    மாறிலி char16_t* s = nullptr;
    ec2 = U_ZERO_ERROR;
    LocalUResourceBundlePointer rb(ures_open(U_ICUDATA_CURR, loc.data(), &ec2));

    எனில் (nameStyle == UCURR_NARROW_குறியீடு_NAME || nameStyle == UCURR_FORMAL_குறியீடு_NAME || nameStyle == UCURR_VARIANT_குறியீடு_NAME) {
        CharString சாவி;
        தேர்வு (nameStyle) {
        நிலை UCURR_NARROW_குறியீடு_NAME:
            சாவி.append(CURRENCIES_NARROW, ec2);
            நிறுத்து;
        நிலை UCURR_FORMAL_குறியீடு_NAME:
            சாவி.append(CURRENCIES_FORMAL, ec2);
            நிறுத்து;
        நிலை UCURR_VARIANT_குறியீடு_NAME:
            சாவி.append(CURRENCIES_VARIANT, ec2);
            நிறுத்து;
        இயல்பு:
            *ec = U_UNSUPPORTED_ERROR;
            திரும்பு nullptr;
        }
        சாவி.append("/", ec2);
        சாவி.append(buf, ec2);
        s = ures_get_String_By_சாவி_With_Fallback(rb.getAlias(), சாவி.data(), len, &ec2);
        எனில் (ec2 == U_MISSING_RESOURCE_ERROR) {
            *ec = U_USING_FALLBACK_WARNING;
            ec2 = U_ZERO_ERROR;
            choice = UCURR_குறியீடு_NAME;
        }
    }
    எனில் (s == nullptr) {
        ures_get_By_சாவி(rb.getAlias(), CURRENCIES, rb.getAlias(), &ec2);
        ures_get_By_சாவி_With_Fallback(rb.getAlias(), buf, rb.getAlias(), &ec2);
        s = ures_getStringByIndex(rb.getAlias(), choice, len, &ec2);
    }

    // If we've succeeded we're done.  Otherwise, try to fallback.
    // If that fails (because we are already at root) then exit.
    எனில் (U_SUCCESS(ec2)) {
        எனில் (ec2 == U_USING_DEFAULT_WARNING
            || (ec2 == U_USING_FALLBACK_WARNING && *ec != U_USING_DEFAULT_WARNING)) {
            *ec = ec2;
        }
    }

    // We no longer support choice format data in names.  Data should not contain
    // choice patterns.
    எனில் (isChoiceFormat != nullptr) {
        *isChoiceFormat = பொய்;
    }
    எனில் (U_SUCCESS(ec2)) {
        U_ASSERT(s != nullptr);
        திரும்பு s;
    }

    // If we fail to find a match, use the ISO 4217 code
    *len = u_strlen(currency); // Should == ISO_CURRENCY_CODE_LENGTH, but maybe not...?
    *ec = U_USING_DEFAULT_WARNING;
    திரும்பு currency;
}

U_CAPI மாறிலி char16_t* U_EXPORT2
ucurr_getPluralName(மாறிலி char16_t* currency U_LIFETIME_BOUND,
                    மாறிலி எழுத்து* locale,
                    UBool* isChoiceFormat,
                    மாறிலி எழுத்து* plural_மொத்தம்,
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

    எனில் (U_FAILURE(*ec)) {
        திரும்பு nullptr;
    }

    // Use a separate UErrorCode here that does not propagate out of
    // this function.
    UErrorCode ec2 = U_ZERO_ERROR;

    எனில் (locale == nullptr) {
        locale = uloc_getDefault();
    }
    CharString loc = ulocimp_getName(locale, ec2);
    எனில் (U_FAILURE(ec2)) {
        *ec = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு nullptr;
    }

    எழுத்து buf[ISO_CURRENCY_CODE_LENGTH+1];
    myUCharsToChars(buf, currency);

    மாறிலி char16_t* s = nullptr;
    ec2 = U_ZERO_ERROR;
    UResourceBundle* rb = ures_open(U_ICUDATA_CURR, loc.data(), &ec2);

    rb = ures_get_By_சாவி(rb, CURRENCYPLURALS, rb, &ec2);

    // Fetch resource with multi-level resource inheritance fallback
    LocalUResourceBundlePointer curr(ures_get_By_சாவி_With_Fallback(rb, buf, rb, &ec2));

    s = ures_get_String_By_சாவி_With_Fallback(curr.getAlias(), plural_மொத்தம், len, &ec2);
    எனில் (U_FAILURE(ec2)) {
        //  fall back to "other"
        ec2 = U_ZERO_ERROR;
        s = ures_get_String_By_சாவி_With_Fallback(curr.getAlias(), "other", len, &ec2);     
        எனில் (U_FAILURE(ec2)) {
            // fall back to long name in Currencies
            திரும்பு ucurr_getName(currency, locale, UCURR_LONG_NAME, 
                                 isChoiceFormat, len, ec);
        }
    }

    // If we've succeeded we're done.  Otherwise, try to fallback.
    // If that fails (because we are already at root) then exit.
    எனில் (U_SUCCESS(ec2)) {
        எனில் (ec2 == U_USING_DEFAULT_WARNING
            || (ec2 == U_USING_FALLBACK_WARNING && *ec != U_USING_DEFAULT_WARNING)) {
            *ec = ec2;
        }
        U_ASSERT(s != nullptr);
        திரும்பு s;
    }

    // If we fail to find a match, use the ISO 4217 code
    *len = u_strlen(currency); // Should == ISO_CURRENCY_CODE_LENGTH, but maybe not...?
    *ec = U_USING_DEFAULT_WARNING;
    திரும்பு currency;
}


//========================================================================
// Following are structure and function for parsing currency names

#define NEED_TO_BE_DELETED 0x1

// TODO: a better way to define this?
#define MAX_CURRENCY_NAME_LEN 100

வகைவரை கட்டமைப்பு {
    மாறிலி எழுத்து* IsoCode;  //சாவி
    char16_t* currencyName;  //மதிப்பு
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
நிலையான முழு U_CALLCONV currencyNameComparator(மாறிலி வெறுமை* a, மாறிலி வெறுமை* b) {
    மாறிலி CurrencyNameStruct* currName_1 = static_cast<மாறிலி CurrencyNameStruct*>(a);
    மாறிலி CurrencyNameStruct* currName_2 = static_cast<மாறிலி CurrencyNameStruct*>(b);
    க்கு (int32_t i = 0; 
         i < MIN(currName_1->currencyNameLen, currName_2->currencyNameLen);
         ++i) {
        எனில் (currName_1->currencyName[i] < currName_2->currencyName[i]) {
            திரும்பு -1;
        }
        எனில் (currName_1->currencyName[i] > currName_2->currencyName[i]) {
            திரும்பு 1;
        }
    }
    எனில் (currName_1->currencyNameLen < currName_2->currencyNameLen) {
        திரும்பு -1;
    } இல்லையெனில் எனில் (currName_1->currencyNameLen > currName_2->currencyNameLen) {
        திரும்பு 1;
    }
    திரும்பு 0;
}


// Give a locale, return the maximum number of currency names associated with
// this locale.
// It gets currency names from resource bundles using fallback.
// It is the maximum number because in the fallback chain, some of the 
// currency names are duplicated.
// For example, given locale as "en_US", the currency names get from resource
// bundle in "en_US" and "en" are duplicated. The fallback mechanism will count
// all currency names in "en_US" and "en".
நிலையான வெறுமை
get_Currency_Name_மொத்தம்(மாறிலி எழுத்து* loc, int32_t* total_currency_name_மொத்தம், int32_t* total_currency_குறியீடு_மொத்தம்) {
    U_NAMESPACE_USE
    *total_currency_name_மொத்தம் = 0;
    *total_currency_குறியீடு_மொத்தம் = 0;
    மாறிலி char16_t* s = nullptr;
    CharString locale;
    {
        UErrorCode status = U_ZERO_ERROR;
        locale.append(loc, status);
        எனில் (U_FAILURE(status)) { திரும்பு; }
    }
    மாறிலி icu::Hashtable *currencySymbolsEquiv = getCurrSymbolsEquiv();
    க்கு (;;) {
        UErrorCode ec2 = U_ZERO_ERROR;
        // TODO: ures_openDirect?
        LocalUResourceBundlePointer rb(ures_open(U_ICUDATA_CURR, locale.data(), &ec2));
        LocalUResourceBundlePointer curr(ures_get_By_சாவி(rb.getAlias(), CURRENCIES, nullptr, &ec2));
        int32_t n = ures_getSize(curr.getAlias());
        க்கு (int32_t i=0; i<n; ++i) {
            LocalUResourceBundlePointer names(ures_getByIndex(curr.getAlias(), i, nullptr, &ec2));
            int32_t len;
            s = ures_getStringByIndex(names.getAlias(), UCURR_குறியீடு_NAME, &len, &ec2);
            ++(*total_currency_குறியீடு_மொத்தம்);  // currency symbol
            எனில் (currencySymbolsEquiv != nullptr) {
                *total_currency_குறியீடு_மொத்தம் += மொத்தம்_Equivalent(*currencySymbolsEquiv, UnicodeString(உண்மை, s, len));
            }
            ++(*total_currency_குறியீடு_மொத்தம்); // iso code
            ++(*total_currency_name_மொத்தம்); // long name
        }

        // currency plurals
        UErrorCode ec3 = U_ZERO_ERROR;
        LocalUResourceBundlePointer curr_p(ures_get_By_சாவி(rb.getAlias(), CURRENCYPLURALS, nullptr, &ec3));
        n = ures_getSize(curr_p.getAlias());
        க்கு (int32_t i=0; i<n; ++i) {
            LocalUResourceBundlePointer names(ures_getByIndex(curr_p.getAlias(), i, nullptr, &ec3));
            *total_currency_name_மொத்தம் += ures_getSize(names.getAlias());
        }

        எனில் (!fallback(locale)) {
            நிறுத்து;
        }
    }
}

நிலையான char16_t*
toUpperCase(மாறிலி char16_t* source, int32_t len, மாறிலி எழுத்து* locale) {
    char16_t* dest = nullptr;
    UErrorCode ec = U_ZERO_ERROR;
    int32_t destLen = u_strToUpper(dest, 0, source, len, locale, &ec);

    ec = U_ZERO_ERROR;
    dest = static_cast<char16_t*>(uprv_malloc(அளவு(char16_t) * MAX(destLen, len)));
    எனில் (dest == nullptr) {
        திரும்பு nullptr;
    }
    u_strToUpper(dest, destLen, source, len, locale, &ec);
    எனில் (U_FAILURE(ec)) {
        u_memcpy(dest, source, len);
    } 
    திரும்பு dest;
}


நிலையான வெறுமை deleteCurrencyNames(CurrencyNameStruct* currencyNames, int32_t count);
// Collect all available currency names associated with the given locale
// (enable fallback chain).
// Read currenc names defined in resource bundle "Currencies" and
// "CurrencyPlural", enable fallback chain.
// return the malloc-ed currency name arrays and the total number of currency
// names in the array.
நிலையான வெறுமை
collectCurrencyNames(மாறிலி எழுத்து* locale, 
                     CurrencyNameStruct** currencyNames, 
                     int32_t* total_currency_name_மொத்தம், 
                     CurrencyNameStruct** currencySymbols, 
                     int32_t* total_currency_குறியீடு_மொத்தம், 
                     UErrorCode& ec) {
    எனில் (U_FAILURE(ec)) {
        *currencyNames = *currencySymbols = nullptr;
        *total_currency_name_மொத்தம் = *total_currency_குறியீடு_மொத்தம் = 0;
        திரும்பு;
    }
    U_NAMESPACE_USE
    மாறிலி icu::Hashtable *currencySymbolsEquiv = getCurrSymbolsEquiv();
    // Look up the Currencies resource for the given locale.
    UErrorCode ec2 = U_ZERO_ERROR;

    எனில் (locale == nullptr) {
        locale = uloc_getDefault();
    }
    CharString loc = ulocimp_getName(locale, ec2);
    எனில் (U_FAILURE(ec2)) {
        ec = U_ILLEGAL_ARGUMENT_ERROR;
        *currencyNames = *currencySymbols = nullptr;
        *total_currency_name_மொத்தம் = *total_currency_குறியீடு_மொத்தம் = 0;
        திரும்பு;
    }

    // Get maximum currency name count first.
    get_Currency_Name_மொத்தம்(loc.data(), total_currency_name_மொத்தம், total_currency_குறியீடு_மொத்தம்);

    *currencyNames = static_cast<CurrencyNameStruct*>(
        uprv_malloc(அளவு(CurrencyNameStruct) * (*total_currency_name_மொத்தம்)));
    எனில்(*currencyNames == nullptr) {
        *currencySymbols = nullptr;
        *total_currency_name_மொத்தம் = *total_currency_குறியீடு_மொத்தம் = 0;
        ec = U_MEMORY_ALLOCATION_ERROR;
        திரும்பு;
    }
    *currencySymbols = static_cast<CurrencyNameStruct*>(
        uprv_malloc(அளவு(CurrencyNameStruct) * (*total_currency_குறியீடு_மொத்தம்)));

    எனில்(*currencySymbols == nullptr) {
        uprv_free(*currencyNames);
        *currencyNames = nullptr;
        *total_currency_name_மொத்தம் = *total_currency_குறியீடு_மொத்தம் = 0;
        ec = U_MEMORY_ALLOCATION_ERROR;
        திரும்பு;
    }

    மாறிலி char16_t* s = nullptr;  // currency name
    எழுத்து* iso = nullptr;  // currency ISO code

    *total_currency_name_மொத்தம் = 0;
    *total_currency_குறியீடு_மொத்தம் = 0;

    UErrorCode ec3 = U_ZERO_ERROR;
    UErrorCode ec4 = U_ZERO_ERROR;

    // Using hash to remove duplicates caused by locale fallback
    LocalUHashtablePointer currencyIsoCodes(uhash_open(uhash_hashChars, uhash_compareChars, nullptr, &ec3));
    LocalUHashtablePointer currencyPluralIsoCodes(uhash_open(uhash_hashChars, uhash_compareChars, nullptr, &ec4));
    க்கு (int32_t localeLevel = 0; ; ++localeLevel) {
        ec2 = U_ZERO_ERROR;
        // TODO: ures_openDirect
        LocalUResourceBundlePointer rb(ures_open(U_ICUDATA_CURR, loc.data(), &ec2));
        LocalUResourceBundlePointer curr(ures_get_By_சாவி(rb.getAlias(), CURRENCIES, nullptr, &ec2));
        int32_t n = ures_getSize(curr.getAlias());
        க்கு (int32_t i=0; i<n; ++i) {
            LocalUResourceBundlePointer names(ures_getByIndex(curr.getAlias(), i, nullptr, &ec2));
            int32_t len;
            s = ures_getStringByIndex(names.getAlias(), UCURR_குறியீடு_NAME, &len, &ec2);
            // TODO: uhash_put wont change key/value?
            iso = const_cast<எழுத்து*>(ures_get_சாவி(names.getAlias()));
            எனில் (localeLevel != 0 && uhash_get(currencyIsoCodes.getAlias(), iso) != nullptr) {
                தொடர்;
            }
            uhash_put(currencyIsoCodes.getAlias(), iso, iso, &ec3); 
            // Add currency symbol.
            (*currencySymbols)[(*total_currency_குறியீடு_மொத்தம்)++] = {iso, const_cast<char16_t*>(s), len, 0};

            // Add equivalent symbols
            எனில் (currencySymbolsEquiv != nullptr) {
                UnicodeString str(உண்மை, s, len);
                icu::EquivIterator iter(*currencySymbolsEquiv, str);
                மாறிலி UnicodeString *குறியீடு;
                போது ((குறியீடு = iter.next()) != nullptr) {
                    (*currencySymbols)[(*total_currency_குறியீடு_மொத்தம்)++]
                        = {iso, const_cast<char16_t*>(குறியீடு->getBuffer()), குறியீடு->length(), 0};
                }
            }

            // Add currency long name.
            s = ures_getStringByIndex(names.getAlias(), UCURR_LONG_NAME, &len, &ec2);
            char16_t* upperName = toUpperCase(s, len, locale);
            எனில் (upperName == nullptr) {
                ec = U_MEMORY_ALLOCATION_ERROR;
                செல் error;
            }
            (*currencyNames)[(*total_currency_name_மொத்தம்)++] = {iso, upperName, len, NEED_TO_BE_DELETED};

            // put (iso, 3, and iso) in to array
            // Add currency ISO code.
            char16_t* isoCode = static_cast<char16_t*>(uprv_malloc(அளவு(char16_t) * 3));
            எனில் (isoCode == nullptr) {
                ec = U_MEMORY_ALLOCATION_ERROR;
                செல் error;
            }
            // Must convert iso[] into Unicode
            u_charsToUChars(iso, isoCode, 3);
            (*currencySymbols)[(*total_currency_குறியீடு_மொத்தம்)++] = {iso, isoCode, 3, NEED_TO_BE_DELETED};
        }

        // currency plurals
        UErrorCode ec5 = U_ZERO_ERROR;
        LocalUResourceBundlePointer curr_p(ures_get_By_சாவி(rb.getAlias(), CURRENCYPLURALS, nullptr, &ec5));
        n = ures_getSize(curr_p.getAlias());
        க்கு (int32_t i=0; i<n; ++i) {
            LocalUResourceBundlePointer names(ures_getByIndex(curr_p.getAlias(), i, nullptr, &ec5));
            iso = const_cast<எழுத்து*>(ures_get_சாவி(names.getAlias()));
            // Using hash to remove duplicated ISO codes in fallback chain.
            எனில் (localeLevel != 0 && uhash_get(currencyPluralIsoCodes.getAlias(), iso) != nullptr) {
                தொடர்;
            }
            uhash_put(currencyPluralIsoCodes.getAlias(), iso, iso, &ec4);
            int32_t num = ures_getSize(names.getAlias());
            int32_t len;
            க்கு (int32_t j = 0; j < num; ++j) {
                // TODO: remove duplicates between singular name and 
                // currency long name?
                s = ures_getStringByIndex(names.getAlias(), j, &len, &ec5);
                char16_t* upperName = toUpperCase(s, len, locale);
                எனில் (upperName == nullptr) {
                    ec = U_MEMORY_ALLOCATION_ERROR;
                    செல் error;
                }
                (*currencyNames)[(*total_currency_name_மொத்தம்)++] = {iso, upperName, len, NEED_TO_BE_DELETED};
            }
        }

        எனில் (!fallback(loc)) {
            நிறுத்து;
        }
    }

    // quick sort the struct
    qsort(*currencyNames, *total_currency_name_மொத்தம், 
          அளவு(CurrencyNameStruct), currencyNameComparator);
    qsort(*currencySymbols, *total_currency_குறியீடு_மொத்தம், 
          அளவு(CurrencyNameStruct), currencyNameComparator);

#ifdef UCURR_DEBUG
    printf("currency name count: %d\n", *total_currency_name_மொத்தம்);
    க்கு (int32_t index = 0; index < *total_currency_name_மொத்தம்; ++index) {
        printf("index: %d\n", index);
        printf("iso: %s\n", (*currencyNames)[index].IsoCode);
        எழுத்து curNameBuf[1024];
        memset(curNameBuf, 0, 1024);
        u_austrncpy(curNameBuf, (*currencyNames)[index].currencyName, (*currencyNames)[index].currencyNameLen);
        printf("currencyName: %s\n", curNameBuf);
        printf("len: %d\n", (*currencyNames)[index].currencyNameLen);
    }
    printf("currency symbol count: %d\n", *total_currency_குறியீடு_மொத்தம்);
    க்கு (int32_t index = 0; index < *total_currency_குறியீடு_மொத்தம்; ++index) {
        printf("index: %d\n", index);
        printf("iso: %s\n", (*currencySymbols)[index].IsoCode);
        எழுத்து curNameBuf[1024];
        memset(curNameBuf, 0, 1024);
        u_austrncpy(curNameBuf, (*currencySymbols)[index].currencyName, (*currencySymbols)[index].currencyNameLen);
        printf("currencySymbol: %s\n", curNameBuf);
        printf("len: %d\n", (*currencySymbols)[index].currencyNameLen);
    }
#endif
    // fail on hashtable errors
    எனில் (U_FAILURE(ec3)) {
      ec = ec3;
    } இல்லையெனில் எனில் (U_FAILURE(ec4)) {
      ec = ec4;
    }

  error:
    // clean up if we got error
    எனில் (U_FAILURE(ec)) {
        deleteCurrencyNames(*currencyNames, *total_currency_name_மொத்தம்);
        deleteCurrencyNames(*currencySymbols, *total_currency_குறியீடு_மொத்தம்);
        *currencyNames = *currencySymbols = nullptr;
        *total_currency_name_மொத்தம் = *total_currency_குறியீடு_மொத்தம் = 0;
    }
}

// @param  currencyNames: currency names array
// @param  indexInCurrencyNames: the index of the character in currency names 
//         array against which the comparison is done
// @param  key: input text char to compare against
// @param  begin(IN/OUT): the begin index of matching range in currency names array
// @param  end(IN/OUT): the end index of matching range in currency names array.
நிலையான int32_t
இருமத்_தேடல்(மாறிலி CurrencyNameStruct* currencyNames, 
             int32_t indexInCurrencyNames,
             மாறிலி char16_t சாவி,
             int32_t* begin, int32_t* end) {
#ifdef UCURR_DEBUG
    printf("key = %x\n", சாவி);
#endif
   int32_t first = *begin;
   int32_t last = *end;
   போது (first <= last) {
       int32_t mid = (first + last) / 2;  // compute mid point.
       எனில் (indexInCurrencyNames >= currencyNames[mid].currencyNameLen) {
           first = mid + 1;
       } இல்லையெனில் {
           எனில் (சாவி > currencyNames[mid].currencyName[indexInCurrencyNames]) {
               first = mid + 1;
           }
           இல்லையெனில் எனில் (சாவி < currencyNames[mid].currencyName[indexInCurrencyNames]) {
               last = mid - 1;
           }
           இல்லையெனில் {
                // Find a match, and looking for ranges
                // Now do two more binary searches. First, on the left side for
                // the greatest L such that CurrencyNameStruct[L] < key.
                int32_t L = *begin;
                int32_t R = mid;

#ifdef UCURR_DEBUG
                printf("mid = %d\n", mid);
#endif
                போது (L < R) {
                    int32_t M = (L + R) / 2;
#ifdef UCURR_DEBUG
                    printf("L = %d, R = %d, M = %d\n", L, R, M);
#endif
                    எனில் (indexInCurrencyNames >= currencyNames[M].currencyNameLen) {
                        L = M + 1;
                    } இல்லையெனில் {
                        எனில் (currencyNames[M].currencyName[indexInCurrencyNames] < சாவி) {
                            L = M + 1;
                        } இல்லையெனில் {
#ifdef UCURR_DEBUG
                            U_ASSERT(currencyNames[M].currencyName[indexInCurrencyNames] == சாவி);
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
                U_ASSERT(currencyNames[*begin].currencyName[indexInCurrencyNames] == சாவி);
#endif

                // Now for the second search, finding the least R such that
                // key < CurrencyNameStruct[R].
                L = mid;
                R = *end;
                போது (L < R) {
                    int32_t M = (L + R) / 2;
#ifdef UCURR_DEBUG
                    printf("L = %d, R = %d, M = %d\n", L, R, M);
#endif
                    எனில் (currencyNames[M].currencyNameLen < indexInCurrencyNames) {
                        L = M + 1;
                    } இல்லையெனில் {
                        எனில் (currencyNames[M].currencyName[indexInCurrencyNames] > சாவி) {
                            R = M;
                        } இல்லையெனில் {
#ifdef UCURR_DEBUG
                            U_ASSERT(currencyNames[M].currencyName[indexInCurrencyNames] == சாவி);
#endif
                            L = M + 1;
                        }
                    }
                }
#ifdef UCURR_DEBUG
                U_ASSERT(L == R);
#endif
                எனில் (currencyNames[R].currencyName[indexInCurrencyNames] > சாவி) {
                    *end = R - 1;
                } இல்லையெனில் {
                    *end = R;
                }
#ifdef UCURR_DEBUG
                printf("end = %d\n", *end);
#endif

                // now, found the range. check whether there is exact match
                எனில் (currencyNames[*begin].currencyNameLen == indexInCurrencyNames + 1) {
                    திரும்பு *begin;  // find range and exact match.
                }
                திரும்பு -1;  // find range, but no exact match.
           }
       }
   }
   *begin = -1;
   *end = -1;
   திரும்பு -1;    // failed to find range.
}


// Linear search "text" in "currencyNames".
// @param  begin, end: the begin and end index in currencyNames, within which
//         range should the search be performed.
// @param  textLen: the length of the text to be compared
// @param  maxMatchLen(IN/OUT): passing in the computed max matching length
//                              pass out the new max  matching length
// @param  maxMatchIndex: the index in currencyName which has the longest
//                        match with input text.
நிலையான வெறுமை
linear_தேடல்(மாறிலி CurrencyNameStruct* currencyNames, 
             int32_t begin, int32_t end,
             மாறிலி char16_t* text, int32_t textLen,
             int32_t *partialMatchLen,
             int32_t *maxMatchLen, int32_t* maxMatchIndex) {
    int32_t initialPartialMatchLen = *partialMatchLen;
    க்கு (int32_t index = begin; index <= end; ++index) {
        int32_t len = currencyNames[index].currencyNameLen;
        எனில் (len > *maxMatchLen && len <= textLen &&
            uprv_memcmp(currencyNames[index].currencyName, text, len * அளவு(char16_t)) == 0) {
            *partialMatchLen = MAX(*partialMatchLen, len);
            *maxMatchIndex = index;
            *maxMatchLen = len;
#ifdef UCURR_DEBUG
            printf("maxMatchIndex = %d, maxMatchLen = %d\n",
                   *maxMatchIndex, *maxMatchLen);
#endif
        } இல்லையெனில் {
            // Check for partial matches.
            க்கு (int32_t i=initialPartialMatchLen; i<MIN(len, textLen); i++) {
                எனில் (currencyNames[index].currencyName[i] != text[i]) {
                    நிறுத்து;
                }
                *partialMatchLen = MAX(*partialMatchLen, i + 1);
            }
        }
    }
}

#define LINEAR_தேடல்_THRESHOLD 10

// Find longest match between "text" and currency names in "currencyNames".
// @param  total_currency_count: total number of currency names in CurrencyNames.
// @param  textLen: the length of the text to be compared
// @param  maxMatchLen: passing in the computed max matching length
//                              pass out the new max  matching length
// @param  maxMatchIndex: the index in currencyName which has the longest
//                        match with input text.
நிலையான வெறுமை
தேடல்_Currency_Name(மாறிலி CurrencyNameStruct* currencyNames, 
                   int32_t total_currency_மொத்தம்,
                   மாறிலி char16_t* text, int32_t textLen,
                   int32_t *partialMatchLen,
                   int32_t* maxMatchLen, int32_t* maxMatchIndex) {
    *maxMatchIndex = -1;
    *maxMatchLen = 0;
    int32_t matchIndex = -1;
    int32_t இருமம்_தேடல்_Begin = 0;
    int32_t இருமம்_தேடல்_End = total_currency_மொத்தம் - 1;
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
    க்கு (int32_t index = 0; index < textLen; ++index) {
        // matchIndex saves the one with exact match till the current point.
        // [binarySearchBegin, binarySearchEnd] saves the matching range.
        matchIndex = இருமத்_தேடல்(currencyNames, index,
                                  text[index],
                                  &இருமம்_தேடல்_Begin, &இருமம்_தேடல்_End);
        எனில் (இருமம்_தேடல்_Begin == -1) { // did not find the range
            நிறுத்து;
        }
        *partialMatchLen = MAX(*partialMatchLen, index + 1);
        எனில் (matchIndex != -1) { 
            // find an exact match for text from text[0] to text[index] 
            // in currencyNames array.
            *maxMatchLen = index + 1;
            *maxMatchIndex = matchIndex;
        }
        எனில் (இருமம்_தேடல்_End - இருமம்_தேடல்_Begin < LINEAR_தேடல்_THRESHOLD) {
            // linear search if within threshold.
            linear_தேடல்(currencyNames, இருமம்_தேடல்_Begin, இருமம்_தேடல்_End,
                         text, textLen,
                         partialMatchLen,
                         maxMatchLen, maxMatchIndex);
            நிறுத்து;
        }
    }
}

//========================= currency name cache =====================
வகைவரை கட்டமைப்பு {
    எழுத்து locale[ULOC_FULLNAME_CAPACITY];  //key
    // currency names, case insensitive
    CurrencyNameStruct* currencyNames;  //மதிப்பு
    int32_t total_Currency_Name_மொத்தம்;  // currency name count
    // currency symbols and ISO code, case sensitive
    CurrencyNameStruct* currencySymbols; //மதிப்பு
    int32_t total_Currency_குறியீடு_மொத்தம்;  //மொத்தம்
    // reference count.
    // reference count is set to 1 when an entry is put to cache.
    // it increases by 1 before accessing, and decreased by 1 after accessing.
    // The entry is deleted when ref count is zero, which means 
    // the entry is replaced out of cache and no process is accessing it.
    int32_t ref_மொத்தம்;
} CurrencyNameCacheEntry;


#define CURRENCY_NAME_CACHE_இலக்கம் 10

// Reserve 10 cache entries.
நிலையான CurrencyNameCacheEntry* currCache[CURRENCY_NAME_CACHE_இலக்கம்] = {nullptr};
// Using an index to indicate which entry to be replaced when cache is full.
// It is a simple round-robin replacement strategy.
நிலையான int8_t currentCacheEntryIndex = 0;

நிலையான UMutex gCurrencyCacheMutex;

// Cache deletion
நிலையான வெறுமை
deleteCurrencyNames(CurrencyNameStruct* currencyNames, int32_t count) {
    க்கு (int32_t index = 0; index < count; ++index) {
        எனில் ( (currencyNames[index].flag & NEED_TO_BE_DELETED) ) {
            uprv_free(currencyNames[index].currencyName);
        }
    }
    uprv_free(currencyNames);
}


நிலையான வெறுமை
deleteCacheEntry(CurrencyNameCacheEntry* entry) {
    deleteCurrencyNames(entry->currencyNames, entry->total_Currency_Name_மொத்தம்);
    deleteCurrencyNames(entry->currencySymbols, entry->total_Currency_குறியீடு_மொத்தம்);
    uprv_free(entry);
}


// Cache clean up
நிலையான UBool U_CALLCONV
currency_cache_cleanup() {
    க்கு (int32_t i = 0; i < CURRENCY_NAME_CACHE_இலக்கம்; ++i) {
        எனில் (currCache[i]) {
            deleteCacheEntry(currCache[i]);
            currCache[i] = nullptr;
        }
    }
    திரும்பு உண்மை;
}


/**
 * Loads the currency name data from the cache, or from resource bundles if necessary.
 * The refCount is automatically incremented.  It is the caller's responsibility
 * to decrement it when done!
 */
நிலையான CurrencyNameCacheEntry*
getCacheEntry(மாறிலி எழுத்து* locale, UErrorCode& ec) {

    int32_t total_currency_name_மொத்தம் = 0;
    CurrencyNameStruct* currencyNames = nullptr;
    int32_t total_currency_குறியீடு_மொத்தம் = 0;
    CurrencyNameStruct* currencySymbols = nullptr;
    CurrencyNameCacheEntry* cacheEntry = nullptr;

    umtx_lock(&gCurrencyCacheMutex);
    // in order to handle racing correctly,
    // not putting 'search' in a separate function.
    int8_t found = -1;
    க்கு (int8_t i = 0; i < CURRENCY_NAME_CACHE_இலக்கம்; ++i) {
        எனில் (currCache[i]!= nullptr &&
            uprv_strcmp(locale, currCache[i]->locale) == 0) {
            found = i;
            நிறுத்து;
        }
    }
    எனில் (found != -1) {
        cacheEntry = currCache[found];
        ++(cacheEntry->ref_மொத்தம்);
    }
    umtx_unlock(&gCurrencyCacheMutex);
    எனில் (found == -1) {
        collectCurrencyNames(locale, &currencyNames, &total_currency_name_மொத்தம், &currencySymbols, &total_currency_குறியீடு_மொத்தம், ec);
        எனில் (U_FAILURE(ec)) {
            திரும்பு nullptr;
        }
        umtx_lock(&gCurrencyCacheMutex);
        // check again.
        க்கு (int8_t i = 0; i < CURRENCY_NAME_CACHE_இலக்கம்; ++i) {
            எனில் (currCache[i]!= nullptr &&
                uprv_strcmp(locale, currCache[i]->locale) == 0) {
                found = i;
                நிறுத்து;
            }
        }
        எனில் (found == -1) {
            // insert new entry to 
            // currentCacheEntryIndex % CURRENCY_NAME_CACHE_NUM
            // and remove the existing entry 
            // currentCacheEntryIndex % CURRENCY_NAME_CACHE_NUM
            // from cache.
            cacheEntry = currCache[currentCacheEntryIndex];
            எனில் (cacheEntry) {
                --(cacheEntry->ref_மொத்தம்);
                // delete if the ref count is zero
                எனில் (cacheEntry->ref_மொத்தம் == 0) {
                    deleteCacheEntry(cacheEntry);
                }
            }
            cacheEntry = static_cast<CurrencyNameCacheEntry*>(uprv_malloc(அளவு(CurrencyNameCacheEntry)));
            எனில் (cacheEntry == nullptr) {
                deleteCurrencyNames(currencyNames, total_currency_name_மொத்தம்);
                deleteCurrencyNames(currencySymbols, total_currency_குறியீடு_மொத்தம்);
                ec = U_MEMORY_ALLOCATION_ERROR;
                திரும்பு nullptr;
            }
            currCache[currentCacheEntryIndex] = cacheEntry;
            uprv_strcpy(cacheEntry->locale, locale);
            cacheEntry->currencyNames = currencyNames;
            cacheEntry->total_Currency_Name_மொத்தம் = total_currency_name_மொத்தம்;
            cacheEntry->currencySymbols = currencySymbols;
            cacheEntry->total_Currency_குறியீடு_மொத்தம் = total_currency_குறியீடு_மொத்தம்;
            cacheEntry->ref_மொத்தம் = 2; // one for cache, one for reference
            currentCacheEntryIndex = (currentCacheEntryIndex + 1) % CURRENCY_NAME_CACHE_இலக்கம்;
            ucln_common_registerCleanup(UCLN_COMMON_CURRENCY, currency_cleanup);
        } இல்லையெனில் {
            deleteCurrencyNames(currencyNames, total_currency_name_மொத்தம்);
            deleteCurrencyNames(currencySymbols, total_currency_குறியீடு_மொத்தம்);
            cacheEntry = currCache[found];
            ++(cacheEntry->ref_மொத்தம்);
        }
        umtx_unlock(&gCurrencyCacheMutex);
    }

    திரும்பு cacheEntry;
}

நிலையான வெறுமை releaseCacheEntry(CurrencyNameCacheEntry* cacheEntry) {
    umtx_lock(&gCurrencyCacheMutex);
    --(cacheEntry->ref_மொத்தம்);
    எனில் (cacheEntry->ref_மொத்தம் == 0) {  // remove
        deleteCacheEntry(cacheEntry);
    }
    umtx_unlock(&gCurrencyCacheMutex);
}

U_CAPI வெறுமை
uprv_parseCurrency(மாறிலி எழுத்து* locale,
                   மாறிலி icu::UnicodeString& text,
                   icu::ParsePosition& pos,
                   int8_t type,
                   int32_t* partialMatchLen,
                   char16_t* result,
                   UErrorCode& ec) {
    U_NAMESPACE_USE
    எனில் (U_FAILURE(ec)) {
        திரும்பு;
    }
    CurrencyNameCacheEntry* cacheEntry = getCacheEntry(locale, ec);
    எனில் (U_FAILURE(ec)) {
        திரும்பு;
    }

    int32_t total_currency_name_மொத்தம் = cacheEntry->total_Currency_Name_மொத்தம்;
    CurrencyNameStruct* currencyNames = cacheEntry->currencyNames;
    int32_t total_currency_குறியீடு_மொத்தம் = cacheEntry->total_Currency_குறியீடு_மொத்தம்;
    CurrencyNameStruct* currencySymbols = cacheEntry->currencySymbols;

    int32_t start = pos.getIndex();

    char16_t உள்ளீடு_Text[MAX_CURRENCY_NAME_LEN];
    char16_t upperText[MAX_CURRENCY_NAME_LEN];
    int32_t textLen = MIN(MAX_CURRENCY_NAME_LEN, text.length() - start);
    text.extract(start, textLen, உள்ளீடு_Text);
    UErrorCode ec1 = U_ZERO_ERROR;
    textLen = u_strToUpper(upperText, MAX_CURRENCY_NAME_LEN, உள்ளீடு_Text, textLen, locale, &ec1);

    // Make sure partialMatchLen is initialized
    *partialMatchLen = 0;

    int32_t max = 0;
    int32_t matchIndex = -1;
    // case in-sensitive comparison against currency names
    தேடல்_Currency_Name(currencyNames, total_currency_name_மொத்தம், 
                       upperText, textLen, partialMatchLen, &max, &matchIndex);

#ifdef UCURR_DEBUG
    printf("search in names, max = %d, matchIndex = %d\n", max, matchIndex);
#endif

    int32_t max_In_குறியீடு = 0;
    int32_t match_Index_In_குறியீடு = -1;
    எனில் (type != UCURR_LONG_NAME) {  // not name only
        // case sensitive comparison against currency symbols and ISO code.
        தேடல்_Currency_Name(currencySymbols, total_currency_குறியீடு_மொத்தம், 
                           உள்ளீடு_Text, textLen,
                           partialMatchLen,
                           &max_In_குறியீடு, &match_Index_In_குறியீடு);
    }

#ifdef UCURR_DEBUG
    printf("search in symbols, maxInSymbol = %d, matchIndexInSymbol = %d\n", max_In_குறியீடு, match_Index_In_குறியீடு);
    எனில்(match_Index_In_குறியீடு != -1) {
      printf("== ISO=%s\n", currencySymbols[match_Index_In_குறியீடு].IsoCode);
    }
#endif

    எனில் (max >= max_In_குறியீடு && matchIndex != -1) {
        u_charsToUChars(currencyNames[matchIndex].IsoCode, result, 4);
        pos.setIndex(start + max);
    } இல்லையெனில் எனில் (max_In_குறியீடு >= max && match_Index_In_குறியீடு != -1) {
        u_charsToUChars(currencySymbols[match_Index_In_குறியீடு].IsoCode, result, 4);
        pos.setIndex(start + max_In_குறியீடு);
    }

    // decrease reference count
    releaseCacheEntry(cacheEntry);
}

வெறுமை uprv_currencyLeads(மாறிலி எழுத்து* locale, icu::UnicodeSet& result, UErrorCode& ec) {
    U_NAMESPACE_USE
    எனில் (U_FAILURE(ec)) {
        திரும்பு;
    }
    CurrencyNameCacheEntry* cacheEntry = getCacheEntry(locale, ec);
    எனில் (U_FAILURE(ec)) {
        திரும்பு;
    }

    க்கு (int32_t i=0; i<cacheEntry->total_Currency_குறியீடு_மொத்தம்; i++) {
        மாறிலி CurrencyNameStruct& info = cacheEntry->currencySymbols[i];
        UChar32 cp;
        U16_GET(info.currencyName, 0, 0, info.currencyNameLen, cp);
        result.add(cp);
    }

    க்கு (int32_t i=0; i<cacheEntry->total_Currency_Name_மொத்தம்; i++) {
        மாறிலி CurrencyNameStruct& info = cacheEntry->currencyNames[i];
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
U_CAPI வெறுமை
uprv_getStaticCurrencyName(மாறிலி char16_t* iso, மாறிலி எழுத்து* loc,
                           icu::UnicodeString& result, UErrorCode& ec)
{
    U_NAMESPACE_USE

    int32_t len;
    மாறிலி char16_t* currname = ucurr_getName(iso, loc, UCURR_குறியீடு_NAME,
                                          nullptr /* isChoiceFormat */, &len, &ec);
    எனில் (U_SUCCESS(ec)) {
        result.setTo(currname, len);
    }
}

U_CAPI int32_t U_EXPORT2
ucurr_getDefaultFractionDigits(மாறிலி char16_t* currency, UErrorCode* ec) {
    திரும்பு ucurr_getDefaultFractionDigitsForUsage(currency,UCURR_USAGE_STANDARD,ec);
}

U_CAPI int32_t U_EXPORT2
ucurr_getDefaultFractionDigitsForUsage(மாறிலி char16_t* currency, மாறிலி UCurrencyUsage usage, UErrorCode* ec) {
    int32_t fracDigits = 0;
    எனில் (U_SUCCESS(*ec)) {
        தேர்வு (usage) {
            நிலை UCURR_USAGE_STANDARD:
                fracDigits = (_findMetaData(currency, *ec))[0];
                நிறுத்து;
            நிலை UCURR_USAGE_CASH:
                fracDigits = (_findMetaData(currency, *ec))[2];
                நிறுத்து;
            இயல்பு:
                *ec = U_UNSUPPORTED_ERROR;
        }
    }
    திரும்பு fracDigits;
}

U_CAPI இரட்டை U_EXPORT2
ucurr_getRoundingIncrement(மாறிலி char16_t* currency, UErrorCode* ec) {
    திரும்பு ucurr_getRoundingIncrementForUsage(currency, UCURR_USAGE_STANDARD, ec);
}

U_CAPI இரட்டை U_EXPORT2
ucurr_getRoundingIncrementForUsage(மாறிலி char16_t* currency, மாறிலி UCurrencyUsage usage, UErrorCode* ec) {
    இரட்டை result = 0.0;

    மாறிலி int32_t *data = _findMetaData(currency, *ec);
    எனில் (U_SUCCESS(*ec)) {
        int32_t fracDigits;
        int32_t increment;
        தேர்வு (usage) {
            நிலை UCURR_USAGE_STANDARD:
                fracDigits = data[0];
                increment = data[1];
                நிறுத்து;
            நிலை UCURR_USAGE_CASH:
                fracDigits = data[2];
                increment = data[3];
                நிறுத்து;
            இயல்பு:
                *ec = U_UNSUPPORTED_ERROR;
                திரும்பு result;
        }

        // If the meta data is invalid, return 0.0
        எனில் (fracDigits < 0 || fracDigits > MAX_POW10) {
            *ec = U_INVALID_FORMAT_ERROR;
        } இல்லையெனில் {
            // A rounding value of 0 or 1 indicates no rounding.
            எனில் (increment >= 2) {
                // Return (increment) / 10^(fracDigits).  The only actual rounding data,
                // as of this writing, is CHF { 2, 5 }.
                result = இரட்டை(increment) / POW10[fracDigits];
            }
        }
    }

    திரும்பு result;
}

U_CDECL_BEGIN

வகைவரை கட்டமைப்பு UCurrencyContext {
    uint32_t currType; /* UCurrCurrencyType */
    uint32_t listIdx;
} UCurrencyContext;

/*
Please keep this list in alphabetical order.
You can look at the CLDR supplemental data or ISO-4217 for the meaning of some
of these items.
ISO-4217: http://www.iso.org/iso/en/prods-services/popstds/currencycodeslist.html
*/
நிலையான மாறிலி கட்டமைப்பு CurrencyList {
    மாறிலி எழுத்து *currency;
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

நிலையான int32_t U_CALLCONV
ucurr_மொத்தம்_Currency_List(UEnumeration *enumerator, UErrorCode * /*pErrorCode*/) {
    UCurrencyContext *myContext = (UCurrencyContext *)(enumerator->context);
    uint32_t currType = myContext->currType;
    int32_t count = 0;

    /* Count the number of items matching the type we are looking for. */
    க்கு (int32_t idx = 0; gCurrencyList[idx].currency != nullptr; idx++) {
        எனில் (UCURR_MATCHES_BITMASK(gCurrencyList[idx].currType, currType)) {
            count++;
        }
    }
    திரும்பு count;
}

நிலையான மாறிலி எழுத்து* U_CALLCONV
ucurr_nextCurrencyList(UEnumeration *enumerator,
                        int32_t* resultLength,
                        UErrorCode * /*pErrorCode*/)
{
    UCurrencyContext *myContext = (UCurrencyContext *)(enumerator->context);

    /* Find the next in the list that matches the type we are looking for. */
    போது (myContext->listIdx < UPRV_LENGTHOF(gCurrencyList)-1) {
        மாறிலி கட்டமைப்பு CurrencyList *currItem = &gCurrencyList[myContext->listIdx++];
        எனில் (UCURR_MATCHES_BITMASK(currItem->currType, myContext->currType))
        {
            எனில் (resultLength) {
                *resultLength = 3; /* Currency codes are only 3 chars long */
            }
            திரும்பு currItem->currency;
        }
    }
    /* We enumerated too far. */
    எனில் (resultLength) {
        *resultLength = 0;
    }
    திரும்பு nullptr;
}

நிலையான வெறுமை U_CALLCONV
ucurr_resetCurrencyList(UEnumeration *enumerator, UErrorCode * /*pErrorCode*/) {
    ((UCurrencyContext *)(enumerator->context))->listIdx = 0;
}

நிலையான வெறுமை U_CALLCONV
ucurr_closeCurrencyList(UEnumeration *enumerator) {
    uprv_free(enumerator->context);
    uprv_free(enumerator);
}

நிலையான வெறுமை U_CALLCONV
ucurr_createCurrencyList(UHashtable *isoCodes, UErrorCode* status){
    UErrorCode localStatus = U_ZERO_ERROR;

    // Look up the CurrencyMap element in the root bundle.
    UResourceBundle *rb = ures_openDirect(U_ICUDATA_CURR, CURRENCY_DATA, &localStatus);
    LocalUResourceBundlePointer currencyMapArray(ures_get_By_சாவி(rb, CURRENCY_MAP, rb, &localStatus));

    எனில் (U_SUCCESS(localStatus)) {
        // process each entry in currency map
        க்கு (int32_t i=0; i<ures_getSize(currencyMapArray.getAlias()); i++) {
            // get the currency resource
            LocalUResourceBundlePointer currencyArray(ures_getByIndex(currencyMapArray.getAlias(), i, nullptr, &localStatus));
            // process each currency
            எனில் (U_SUCCESS(localStatus)) {
                க்கு (int32_t j=0; j<ures_getSize(currencyArray.getAlias()); j++) {
                    // get the currency resource
                    LocalUResourceBundlePointer currencyRes(ures_getByIndex(currencyArray.getAlias(), j, nullptr, &localStatus));
                    IsoCodeEntry *entry = (IsoCodeEntry*)uprv_malloc(அளவு(IsoCodeEntry));
                    எனில் (entry == nullptr) {
                        *status = U_MEMORY_ALLOCATION_ERROR;
                        திரும்பு;
                    }

                    // get the ISO code
                    int32_t isoLength = 0;
                    LocalUResourceBundlePointer idRes(ures_get_By_சாவி(currencyRes.getAlias(), "id", nullptr, &localStatus));
                    எனில் (idRes.isNull()) {
                        தொடர்;
                    }
                    மாறிலி char16_t *isoCode = ures_getString(idRes.getAlias(), &isoLength, &localStatus);

                    // get from date
                    UDate fromDate = U_DATE_MIN;
                    LocalUResourceBundlePointer fromRes(ures_get_By_சாவி(currencyRes.getAlias(), "from", nullptr, &localStatus));

                    எனில் (U_SUCCESS(localStatus)) {
                        int32_t fromLength = 0;
                        மாறிலி int32_t *fromArray = ures_getIntVector(fromRes.getAlias(), &fromLength, &localStatus);
                        int64_t currDate64 = ((uint64_t)fromArray[0]) << 32;
                        currDate64 |= ((int64_t)fromArray[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                        fromDate = (UDate)currDate64;
                    }

                    // get to date
                    UDate toDate = U_DATE_MAX;
                    localStatus = U_ZERO_ERROR;
                    LocalUResourceBundlePointer toRes(ures_get_By_சாவி(currencyRes.getAlias(), "to", nullptr, &localStatus));

                    எனில் (U_SUCCESS(localStatus)) {
                        int32_t toLength = 0;
                        மாறிலி int32_t *toArray = ures_getIntVector(toRes.getAlias(), &toLength, &localStatus);
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
            } இல்லையெனில் {
                *status = localStatus;
            }
        }
    } இல்லையெனில் {
        *status = localStatus;
    }
}

நிலையான மாறிலி UEnumeration gEnumCurrencyList = {
    nullptr,
    nullptr,
    ucurr_closeCurrencyList,
    ucurr_மொத்தம்_Currency_List,
    uenum_unextDefault,
    ucurr_nextCurrencyList,
    ucurr_resetCurrencyList
};
U_CDECL_END


நிலையான வெறுமை U_CALLCONV initIsoCodes(UErrorCode &status) {
    U_ASSERT(gIsoCodes == nullptr);
    ucln_common_registerCleanup(UCLN_COMMON_CURRENCY, currency_cleanup);

    LocalUHashtablePointer isoCodes(uhash_open(uhash_hashUChars, uhash_compareUChars, nullptr, &status));
    எனில் (U_FAILURE(status)) {
        திரும்பு;
    }
    uhash_set_மதிப்பு_Deleter(isoCodes.getAlias(), deleteIsoCodeEntry);

    ucurr_createCurrencyList(isoCodes.getAlias(), &status);
    எனில் (U_FAILURE(status)) {
        திரும்பு;
    }
    gIsoCodes = isoCodes.orphan();  // Note: gIsoCodes is const. Once set up here it is never altered,
                                    //       and read only access is safe without synchronization.
}

நிலையான வெறுமை populateCurrSymbolsEquiv(icu::Hashtable *hash, UErrorCode &status) {
    எனில் (U_FAILURE(status)) { திரும்பு; }
    க்கு (மாறிலி auto& entry : unisets::kCurrencyEntries) {
        UnicodeString exemplar(entry.exemplar);
        மாறிலி UnicodeSet* set = unisets::get(entry.சாவி);
        எனில் (set == nullptr) { திரும்பு; }
        UnicodeSetIterator it(*set);
        போது (it.next()) {
            UnicodeString value = it.getString();
            எனில் (value == exemplar) {
                // No need to mark the exemplar character as an equivalent
                தொடர்;
            }
            makeEquivalent(exemplar, value, hash, status);
            எனில் (U_FAILURE(status)) { திரும்பு; }
        }
    }
}

நிலையான வெறுமை U_CALLCONV initCurrSymbolsEquiv() {
    U_ASSERT(gCurrSymbolsEquiv == nullptr);
    UErrorCode status = U_ZERO_ERROR;
    ucln_common_registerCleanup(UCLN_COMMON_CURRENCY, currency_cleanup);
    icu::Hashtable *temp = புதிய icu::Hashtable(status);
    எனில் (temp == nullptr) {
        திரும்பு;
    }
    எனில் (U_FAILURE(status)) {
        delete temp;
        திரும்பு;
    }
    temp->set_மதிப்பு_Deleter(deleteUnicode);
    populateCurrSymbolsEquiv(temp, status);
    எனில் (U_FAILURE(status)) {
        delete temp;
        திரும்பு;
    }
    gCurrSymbolsEquiv = temp;
}

U_CAPI UBool U_EXPORT2
ucurr_isAvailable(மாறிலி char16_t* isoCode, UDate from, UDate to, UErrorCode* eErrorCode) {
    umtx_initOnce(gIsoCodesInitOnce, &initIsoCodes, *eErrorCode);
    எனில் (U_FAILURE(*eErrorCode)) {
        திரும்பு பொய்;
    }

    IsoCodeEntry* result = (IsoCodeEntry *) uhash_get(gIsoCodes, isoCode);
    எனில் (result == nullptr) {
        திரும்பு பொய்;
    } இல்லையெனில் எனில் (from > to) {
        *eErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு பொய்;
    } இல்லையெனில் எனில்  ((from > result->to) || (to < result->from)) {
        திரும்பு பொய்;
    }
    திரும்பு உண்மை;
}

நிலையான மாறிலி icu::Hashtable* getCurrSymbolsEquiv() {
    umtx_initOnce(gCurrSymbolsEquivInitOnce, &initCurrSymbolsEquiv);
    திரும்பு gCurrSymbolsEquiv;
}

U_CAPI UEnumeration * U_EXPORT2
ucurr_openISOCurrencies(uint32_t currType, UErrorCode *pErrorCode) {
    UEnumeration *myEnum = nullptr;
    UCurrencyContext *myContext;

    myEnum = (UEnumeration*)uprv_malloc(அளவு(UEnumeration));
    எனில் (myEnum == nullptr) {
        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
        திரும்பு nullptr;
    }
    uprv_memcpy(myEnum, &gEnumCurrencyList, அளவு(UEnumeration));
    myContext = (UCurrencyContext*)uprv_malloc(அளவு(UCurrencyContext));
    எனில் (myContext == nullptr) {
        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
        uprv_free(myEnum);
        திரும்பு nullptr;
    }
    myContext->currType = currType;
    myContext->listIdx = 0;
    myEnum->context = myContext;
    திரும்பு myEnum;
}

U_CAPI int32_t U_EXPORT2
ucurr_மொத்தம்_Currencies(மாறிலி எழுத்து* locale, 
                 UDate date, 
                 UErrorCode* ec)
{
    int32_t curr_மொத்தம் = 0;

    எனில் (ec != nullptr && U_SUCCESS(*ec)) 
    {
        // local variables
        UErrorCode localStatus = U_ZERO_ERROR;

        // get country or country_variant in `id'
        CharString id = idForLocale(locale, ec);

        எனில் (U_FAILURE(*ec))
        {
            திரும்பு 0;
        }

        // Remove variants, which is only needed for registration.
        எழுத்து *idDelim = strchr(id.data(), VAR_DELIM);
        எனில் (idDelim)
        {
            id.truncate(idDelim - id.data());
        }

        // Look up the CurrencyMap element in the root bundle.
        UResourceBundle *rb = ures_openDirect(U_ICUDATA_CURR, CURRENCY_DATA, &localStatus);
        UResourceBundle *cm = ures_get_By_சாவி(rb, CURRENCY_MAP, rb, &localStatus);

        // Using the id derived from the local, get the currency data
        LocalUResourceBundlePointer countryArray(ures_get_By_சாவி(rb, id.data(), cm, &localStatus));

        // process each currency to see which one is valid for the given date
        எனில் (U_SUCCESS(localStatus))
        {
            க்கு (int32_t i=0; i<ures_getSize(countryArray.getAlias()); i++)
            {
                // get the currency resource
                LocalUResourceBundlePointer currencyRes(ures_getByIndex(countryArray.getAlias(), i, nullptr, &localStatus));

                // get the from date
                int32_t fromLength = 0;
                LocalUResourceBundlePointer fromRes(ures_get_By_சாவி(currencyRes.getAlias(), "from", nullptr, &localStatus));
                மாறிலி int32_t *fromArray = ures_getIntVector(fromRes.getAlias(), &fromLength, &localStatus);

                int64_t currDate64 = (int64_t)((uint64_t)(fromArray[0]) << 32);
                currDate64 |= ((int64_t)fromArray[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                UDate fromDate = (UDate)currDate64;

                எனில் (ures_getSize(currencyRes.getAlias())> 2)
                {
                    int32_t toLength = 0;
                    LocalUResourceBundlePointer toRes(ures_get_By_சாவி(currencyRes.getAlias(), "to", nullptr, &localStatus));
                    மாறிலி int32_t *toArray = ures_getIntVector(toRes.getAlias(), &toLength, &localStatus);

                    currDate64 = (int64_t)toArray[0] << 32;
                    currDate64 |= ((int64_t)toArray[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                    UDate toDate = (UDate)currDate64;

                    எனில் ((fromDate <= date) && (date < toDate))
                    {
                        curr_மொத்தம்++;
                    }
                }
                இல்லையெனில்
                {
                    எனில் (fromDate <= date)
                    {
                        curr_மொத்தம்++;
                    }
                }
            } // end For loop
        } // end if (U_SUCCESS(localStatus))

        // Check for errors
        எனில் (*ec == U_ZERO_ERROR || localStatus != U_ZERO_ERROR)
        {
            // There is nothing to fallback to. 
            // Report the failure/warning if possible.
            *ec = localStatus;
        }

        எனில் (U_SUCCESS(*ec))
        {
            // no errors
            திரும்பு curr_மொத்தம்;
        }
    }

    // If we got here, either error code is invalid or
    // some argument passed is no good.
    திரும்பு 0;
}

U_CAPI int32_t U_EXPORT2 
ucurr_forLocaleAndDate(மாறிலி எழுத்து* locale, 
                UDate date, 
                int32_t index,
                char16_t* buff,
                int32_t buffCapacity, 
                UErrorCode* ec)
{
    int32_t resLen = 0;
	int32_t currIndex = 0;
    மாறிலி char16_t* s = nullptr;

    எனில் (ec != nullptr && U_SUCCESS(*ec))
    {
        // check the arguments passed
        எனில் ((buff && buffCapacity) || !buffCapacity )
        {
            // local variables
            UErrorCode localStatus = U_ZERO_ERROR;

            // get country or country_variant in `id'
            CharString id = idForLocale(locale, ec);
            எனில் (U_FAILURE(*ec))
            {
                திரும்பு 0;
            }

            // Remove variants, which is only needed for registration.
            எழுத்து *idDelim = strchr(id.data(), VAR_DELIM);
            எனில் (idDelim)
            {
                id.truncate(idDelim - id.data());
            }

            // Look up the CurrencyMap element in the root bundle.
            UResourceBundle *rb = ures_openDirect(U_ICUDATA_CURR, CURRENCY_DATA, &localStatus);
            UResourceBundle *cm = ures_get_By_சாவி(rb, CURRENCY_MAP, rb, &localStatus);

            // Using the id derived from the local, get the currency data
            LocalUResourceBundlePointer countryArray(ures_get_By_சாவி(rb, id.data(), cm, &localStatus));

            // process each currency to see which one is valid for the given date
            தருக்க matchFound = பொய்;
            எனில் (U_SUCCESS(localStatus))
            {
                எனில் ((index <= 0) || (index> ures_getSize(countryArray.getAlias())))
                {
                    // requested index is out of bounds
                    திரும்பு 0;
                }

                க்கு (int32_t i=0; i<ures_getSize(countryArray.getAlias()); i++)
                {
                    // get the currency resource
                    LocalUResourceBundlePointer currencyRes(ures_getByIndex(countryArray.getAlias(), i, nullptr, &localStatus));
                    s = ures_get_String_By_சாவி(currencyRes.getAlias(), "id", &resLen, &localStatus);

                    // get the from date
                    int32_t fromLength = 0;
                    LocalUResourceBundlePointer fromRes(ures_get_By_சாவி(currencyRes.getAlias(), "from", nullptr, &localStatus));
                    மாறிலி int32_t *fromArray = ures_getIntVector(fromRes.getAlias(), &fromLength, &localStatus);

                    int64_t currDate64 = (int64_t)((uint64_t)fromArray[0] << 32);
                    currDate64 |= ((int64_t)fromArray[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                    UDate fromDate = (UDate)currDate64;

                    எனில் (ures_getSize(currencyRes.getAlias()) > 2)
                    {
                        int32_t toLength = 0;
                        LocalUResourceBundlePointer toRes(ures_get_By_சாவி(currencyRes.getAlias(), "to", nullptr, &localStatus));
                        மாறிலி int32_t *toArray = ures_getIntVector(toRes.getAlias(), &toLength, &localStatus);

                        currDate64 = (int64_t)toArray[0] << 32;
                        currDate64 |= ((int64_t)toArray[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                        UDate toDate = (UDate)currDate64;

                        எனில் ((fromDate <= date) && (date < toDate))
                        {
                            currIndex++;
                            எனில் (currIndex == index)
                            {
                                matchFound = உண்மை;
                            }
                        }
                    }
                    இல்லையெனில்
                    {
                        எனில் (fromDate <= date)
                        {
                            currIndex++;
                            எனில் (currIndex == index)
                            {
                                matchFound = உண்மை;
                            }
                        }
                    }
                    // check for loop exit
                    எனில் (matchFound)
                    {
                        நிறுத்து;
                    }

                } // end For loop
            }

            // Check for errors
            எனில் (*ec == U_ZERO_ERROR || localStatus != U_ZERO_ERROR)
            {
                // There is nothing to fallback to. 
                // Report the failure/warning if possible.
                *ec = localStatus;
            }

            எனில் (U_SUCCESS(*ec))
            {
                // no errors
                எனில்((buffCapacity> resLen) && matchFound)
                {
                    // write out the currency value
                    u_strcpy(buff, s);
                }
                இல்லையெனில்
                {
                    திரும்பு 0;
                }
            }

            // return null terminated currency string
            திரும்பு u_terminateUChars(buff, buffCapacity, resLen, ec);
        }
        இல்லையெனில்
        {
            // illegal argument encountered
            *ec = U_ILLEGAL_ARGUMENT_ERROR;
        }

    }

    // If we got here, either error code is invalid or
    // some argument passed is no good.
    திரும்பு resLen;
}

நிலையான மாறிலி UEnumeration default_Keyword_மதிப்புகள் = {
    nullptr,
    nullptr,
    ulist_close_keyword_மதிப்புகள்_iterator,
    ulist_மொத்தம்_keyword_மதிப்புகள்,
    uenum_unextDefault,
    ulist_next_keyword_மதிப்பு, 
    ulist_reset_keyword_மதிப்புகள்_iterator
};

U_CAPI UEnumeration *U_EXPORT2 ucurr_get_Keyword_மதிப்புகள்_For_Locale(மாறிலி எழுத்து *சாவி, மாறிலி எழுத்து *locale, UBool commonlyUsed, UErrorCode* status) {
    // Resolve region
    CharString prefRegion = ulocimp_getRegionForSupplementalData(locale, உண்மை, *status);

    // Read value from supplementalData
    UList *மதிப்புகள் = ulist_createEmptyList(status);
    UList *other_மதிப்புகள் = ulist_createEmptyList(status);
    UEnumeration *en = (UEnumeration *)uprv_malloc(அளவு(UEnumeration));
    எனில் (U_FAILURE(*status) || en == nullptr) {
        எனில் (en == nullptr) {
            *status = U_MEMORY_ALLOCATION_ERROR;
        } இல்லையெனில் {
            uprv_free(en);
        }
        ulist_deleteList(மதிப்புகள்);
        ulist_deleteList(other_மதிப்புகள்);
        திரும்பு nullptr;
    }
    memcpy(en, &default_Keyword_மதிப்புகள், அளவு(UEnumeration));
    en->context = மதிப்புகள்;
    
    UResourceBundle* rb = ures_openDirect(U_ICUDATA_CURR, "supplementalData", status);
    LocalUResourceBundlePointer bundle(ures_get_By_சாவி(rb, "CurrencyMap", rb, status));
    அடுக்கு_UResource_Bundle bundlekey, regbndl, curbndl, to;
    
    போது (U_SUCCESS(*status) && ures_hasNext(bundle.getAlias())) {
        ures_getNextResource(bundle.getAlias(), bundlekey.getAlias(), status);
        எனில் (U_FAILURE(*status)) {
            நிறுத்து;
        }
        மாறிலி எழுத்து *region = ures_get_சாவி(bundlekey.getAlias());
        UBool isPrefRegion = prefRegion == region;
        எனில் (!isPrefRegion && commonlyUsed) {
            // With commonlyUsed=true, we do not put
            // currencies for other regions in the
            // result list.
            தொடர்;
        }
        ures_get_By_சாவி(bundle.getAlias(), region, regbndl.getAlias(), status);
        எனில் (U_FAILURE(*status)) {
            நிறுத்து;
        }
        போது (U_SUCCESS(*status) && ures_hasNext(regbndl.getAlias())) {
            ures_getNextResource(regbndl.getAlias(), curbndl.getAlias(), status);
            எனில் (ures_getType(curbndl.getAlias()) != URES_TABLE) {
                // Currently, an empty ARRAY is mixed in.
                தொடர்;
            }
            எழுத்து *curID = (எழுத்து *)uprv_malloc(அளவு(எழுத்து) * ULOC_KEYWORDS_CAPACITY);
            எனில் (curID == nullptr) {
                *status = U_MEMORY_ALLOCATION_ERROR;
                நிறுத்து;
            }
            int32_t curIDLength = ULOC_KEYWORDS_CAPACITY;

#எனில் U_CHARSET_FAMILY==U_ASCII_FAMILY
            ures_get_UTF8_String_By_சாவி(curbndl.getAlias(), "id", curID, &curIDLength, உண்மை, status);
            /* optimize - use the utf-8 string */
#இல்லையெனில்
            {
                       மாறிலி char16_t* defString = ures_get_String_By_சாவி(curbndl.getAlias(), "id", &curIDLength, status);
                       எனில்(U_SUCCESS(*status)) {
			   எனில்(curIDLength+1 > ULOC_KEYWORDS_CAPACITY) {
				*status = U_BUFFER_OVERFLOW_ERROR;
			   } இல்லையெனில் {
                           	u_UCharsToChars(defString, curID, curIDLength+1);
			   }
                       }
            }
#endif	

            எனில் (U_FAILURE(*status)) {
                நிறுத்து;
            }
            UBool hasTo = பொய்;
            ures_get_By_சாவி(curbndl.getAlias(), "to", to.getAlias(), status);
            எனில் (U_FAILURE(*status)) {
                // Do nothing here...
                *status = U_ZERO_ERROR;
            } இல்லையெனில் {
                hasTo = உண்மை;
            }
            எனில் (isPrefRegion && !hasTo && !ulist_containsString(மதிப்புகள், curID, (int32_t)uprv_strlen(curID))) {
                // Currently active currency for the target country
                ulist_addItemEndList(மதிப்புகள், curID, உண்மை, status);
            } இல்லையெனில் எனில் (!ulist_containsString(other_மதிப்புகள், curID, (int32_t)uprv_strlen(curID)) && !commonlyUsed) {
                ulist_addItemEndList(other_மதிப்புகள், curID, உண்மை, status);
            } இல்லையெனில் {
                uprv_free(curID);
            }
        }
        
    }
    எனில் (U_SUCCESS(*status)) {
        எனில் (commonlyUsed) {
            எனில் (ulist_getListSize(மதிப்புகள்) == 0) {
                // This could happen if no valid region is supplied in the input
                // locale. In this case, we use the CLDR's default.
                uenum_close(en);
                en = ucurr_get_Keyword_மதிப்புகள்_For_Locale(சாவி, "und", உண்மை, status);
            }
        } இல்லையெனில் {
            // Consolidate the list
            எழுத்து *value = nullptr;
            ulist_resetList(other_மதிப்புகள்);
            போது ((value = (எழுத்து *)ulist_getNext(other_மதிப்புகள்)) != nullptr) {
                எனில் (!ulist_containsString(மதிப்புகள், value, (int32_t)uprv_strlen(value))) {
                    எழுத்து *tmp_மதிப்பு = (எழுத்து *)uprv_malloc(அளவு(எழுத்து) * ULOC_KEYWORDS_CAPACITY);
                    எனில் (tmp_மதிப்பு == nullptr) {
                        *status = U_MEMORY_ALLOCATION_ERROR;
                        நிறுத்து;
                    }
                    uprv_memcpy(tmp_மதிப்பு, value, uprv_strlen(value) + 1);
                    ulist_addItemEndList(மதிப்புகள், tmp_மதிப்பு, உண்மை, status);
                    எனில் (U_FAILURE(*status)) {
                        நிறுத்து;
                    }
                }
            }
        }
        ulist_resetList((UList *)(en->context));
    } இல்லையெனில் {
        ulist_deleteList(மதிப்புகள்);
        uprv_free(en);
        மதிப்புகள் = nullptr;
        en = nullptr;
    }
    ulist_deleteList(other_மதிப்புகள்);
    திரும்பு en;
}


U_CAPI int32_t U_EXPORT2
ucurr_getNumericCode(மாறிலி char16_t* currency) {
    int32_t code = 0;
    எனில் (currency && u_strlen(currency) == ISO_CURRENCY_CODE_LENGTH) {
        UErrorCode status = U_ZERO_ERROR;

        UResourceBundle* bundle = ures_openDirect(nullptr, "currencyNumericCodes", &status);
        LocalUResourceBundlePointer codeMap(ures_get_By_சாவி(bundle, "codeMap", bundle, &status));
        எனில் (U_SUCCESS(status)) {
            எழுத்து alphaCode[ISO_CURRENCY_CODE_LENGTH+1];
            myUCharsToChars(alphaCode, currency);
            T_CString_toUpperCase(alphaCode);
            ures_get_By_சாவி(codeMap.getAlias(), alphaCode, codeMap.getAlias(), &status);
            முழு tmpCode = ures_getInt(codeMap.getAlias(), &status);
            எனில் (U_SUCCESS(status)) {
                code = tmpCode;
            }
        }
    }
    திரும்பு code;
}
#endif /* #if !UCONFIG_NO_FORMATTING */

//eof
