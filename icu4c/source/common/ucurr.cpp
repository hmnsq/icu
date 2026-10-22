// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
* Copyright (c) 2002-2016, International Business Machines
* Corporation and others.  All Rights Reserved.
**********************************************************************
*/

#अन्तर्भाव "unicode/utypes.h"

#जर !UCONFIG_NO_FORMATTING

#अन्तर्भाव <utility>

#अन्तर्भाव "unicode/ucurr.h"
#अन्तर्भाव "unicode/locid.h"
#अन्तर्भाव "unicode/ures.h"
#अन्तर्भाव "unicode/ustring.h"
#अन्तर्भाव "unicode/parsepos.h"
#अन्तर्भाव "unicode/uniset.h"
#अन्तर्भाव "unicode/usetiter.h"
#अन्तर्भाव "unicode/utf16.h"
#अन्तर्भाव "ustr_imp.h"
#अन्तर्भाव "charstr.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "cstring.h"
#अन्तर्भाव "static_unicode_sets.h"
#अन्तर्भाव "uassert.h"
#अन्तर्भाव "umutex.h"
#अन्तर्भाव "ucln_cmn.h"
#अन्तर्भाव "uenumimp.h"
#अन्तर्भाव "uhash.h"
#अन्तर्भाव "hash.h"
#अन्तर्भाव "uinvchar.h"
#अन्तर्भाव "uresimp.h"
#अन्तर्भाव "ulist.h"
#अन्तर्भाव "uresimp.h"
#अन्तर्भाव "ureslocs.h"
#अन्तर्भाव "ulocimp.h"

using नामाकाश icu;

//#define UCURR_DEBUG_EQUIV 1
#ifdef UCURR_DEBUG_EQUIV
#अन्तर्भाव "stdio.h"
#endif
//#define UCURR_DEBUG 1
#ifdef UCURR_DEBUG
#अन्तर्भाव "stdio.h"
#endif

प्रकारव्याख्या संरचना IsoCodeEntry {
    स्थिरांक char16_t *isoCode; /* const because it's a reference to a resource bundle string. */
    UDate from;
    UDate to;
} IsoCodeEntry;

//------------------------------------------------------------
// Constants

// Default currency meta data of last resort.  We try to use the
// defaults encoded in the meta data resource bundle.  If there is a
// configuration/build error and these are not available, we use these
// hard-coded defaults (which should be identical).
स्थिर स्थिरांक int32_t LAST_RESORT_DATA[] = { 2, 0, 2, 0 };

// POW10[i] = 10^i, i=0..MAX_POW10
स्थिर स्थिरांक int32_t POW10[] = { 1, 10, 100, 1000, 10000, 100000,
                                 1000000, 10000000, 100000000, 1000000000 };

स्थिर स्थिरांक int32_t MAX_POW10 = UPRV_LENGTHOF(POW10) - 1;

#define ISO_CURRENCY_CODE_LENGTH 3

//------------------------------------------------------------
// Resource tags
//

स्थिर स्थिरांक अक्षर CURRENCY_DATA[] = "supplementalData";
// Tag for meta-data, in root.
स्थिर स्थिरांक अक्षर CURRENCY_META[] = "CurrencyMeta";

// Tag for map from countries to currencies, in root.
स्थिर स्थिरांक अक्षर CURRENCY_MAP[] = "CurrencyMap";

// Tag for default meta-data, in CURRENCY_META
स्थिर स्थिरांक अक्षर DEFAULT_META[] = "DEFAULT";

// Variant delimiter
स्थिर स्थिरांक अक्षर VAR_DELIM = '_';

// Tag for localized display names (symbols) of currencies
स्थिर स्थिरांक अक्षर CURRENCIES[] = "Currencies";
स्थिर स्थिरांक अक्षर CURRENCIES_NARROW[] = "Currencies%narrow";
स्थिर स्थिरांक अक्षर CURRENCIES_FORMAL[] = "Currencies%formal";
स्थिर स्थिरांक अक्षर CURRENCIES_VARIANT[] = "Currencies%variant";
स्थिर स्थिरांक अक्षर CURRENCYPLURALS[] = "CurrencyPlurals";

// ISO codes mapping table
स्थिर स्थिरांक UHashtable* gIsoCodes = nullptr;
स्थिर icu::UInitOnce gIsoCodesInitOnce {};

// Currency symbol equivalances
स्थिर स्थिरांक icu::Hashtable* gCurrSymbolsEquiv = nullptr;
स्थिर icu::UInitOnce gCurrSymbolsEquivInitOnce {};

U_NAMESPACE_BEGIN

// EquivIterator iterates over all strings that are equivalent to a given
// string, s. Note that EquivIterator will never yield s itself.
वर्ग EquivIterator : सार्वजनिक icu::UMemory {
सार्वजनिक:
    // Constructor. hash stores the equivalence relationships; s is the string
    // for which we find equivalent strings.
    ओळीत EquivIterator(स्थिरांक icu::Hashtable& hash, स्थिरांक icu::UnicodeString& s)
        : _hash(hash) { 
        _start = _current = &s;
    }
    ओळीत ~EquivIterator() { }

    // next returns the next equivalent string or nullptr if there are no more.
    // If s has no equivalent strings, next returns nullptr on the first call.
    स्थिरांक icu::UnicodeString *next();
खाजगी:
    स्थिरांक icu::Hashtable& _hash;
    स्थिरांक icu::UnicodeString* _start;
    स्थिरांक icu::UnicodeString* _current;
};

स्थिरांक icu::UnicodeString *
EquivIterator::next() {
    स्थिरांक icu::UnicodeString* _next = static_cast<स्थिरांक icu::UnicodeString*>(_hash.get(*_current));
    जर (_next == nullptr) {
        U_ASSERT(_current == _start);
        परत nullptr;
    }
    जर (*_next == *_start) {
        परत nullptr;
    }
    _current = _next;
    परत _next;
}

U_NAMESPACE_END

// makeEquivalent makes lhs and rhs equivalent by updating the equivalence
// relations in hash accordingly.
स्थिर शून्य makeEquivalent(
    स्थिरांक icu::UnicodeString &lhs,
    स्थिरांक icu::UnicodeString &rhs,
    icu::Hashtable* hash, UErrorCode &status) {
    जर (U_FAILURE(status)) {
        परत;
    }
    जर (lhs == rhs) {
        // already equivalent
        परत;
    }
    icu::EquivIterator डावा_Iter(*hash, lhs);
    icu::EquivIterator उजवा_Iter(*hash, rhs);
    स्थिरांक icu::UnicodeString *first_डावा = डावा_Iter.next();
    स्थिरांक icu::UnicodeString *first_उजवा = उजवा_Iter.next();
    स्थिरांक icu::UnicodeString *next_डावा = first_डावा;
    स्थिरांक icu::UnicodeString *next_उजवा = first_उजवा;
    पर्यंत (next_डावा != nullptr && next_उजवा != nullptr) {
        जर (*next_डावा == rhs || *next_उजवा == lhs) {
            // Already equivalent
            परत;
        }
        next_डावा = डावा_Iter.next();
        next_उजवा = उजवा_Iter.next();
    }
    // Not equivalent. Must join.
    icu::UnicodeString *new_First_डावा;
    icu::UnicodeString *new_First_उजवा;
    जर (first_उजवा == nullptr && first_डावा == nullptr) {
        // Neither lhs or rhs belong to an equivalence circle, so we form
        // a new equivalnce circle of just lhs and rhs.
        new_First_डावा = नवीन icu::UnicodeString(rhs);
        new_First_उजवा = नवीन icu::UnicodeString(lhs);
    } अन्यथा जर (first_उजवा == nullptr) {
        // lhs belongs to an equivalence circle, but rhs does not, so we link
        // rhs into lhs' circle.
        new_First_डावा = नवीन icu::UnicodeString(rhs);
        new_First_उजवा = नवीन icu::UnicodeString(*first_डावा);
    } अन्यथा जर (first_डावा == nullptr) {
        // rhs belongs to an equivlance circle, but lhs does not, so we link
        // lhs into rhs' circle.
        new_First_डावा = नवीन icu::UnicodeString(*first_उजवा);
        new_First_उजवा = नवीन icu::UnicodeString(lhs);
    } अन्यथा {
        // Both lhs and rhs belong to different equivalnce circles. We link
        // them together to form one single, larger equivalnce circle.
        new_First_डावा = नवीन icu::UnicodeString(*first_उजवा);
        new_First_उजवा = नवीन icu::UnicodeString(*first_डावा);
    }
    जर (new_First_डावा == nullptr || new_First_उजवा == nullptr) {
        delete new_First_डावा;
        delete new_First_उजवा;
        status = U_MEMORY_ALLOCATION_ERROR;
        परत;
    }
    hash->put(lhs, (शून्य *) new_First_डावा, status);
    hash->put(rhs, (शून्य *) new_First_उजवा, status);
}

// countEquivalent counts how many strings are equivalent to s.
// hash stores all the equivalnce relations.
// countEquivalent does not include s itself in the count.
स्थिर int32_t मोजणी_Equivalent(स्थिरांक icu::Hashtable &hash, स्थिरांक icu::UnicodeString &s) {
    int32_t result = 0;
    icu::EquivIterator iter(hash, s);
    पर्यंत (iter.next() != nullptr) {
        ++result;
    }
#ifdef UCURR_DEBUG_EQUIV
 {
   अक्षर tmp[200];
   s.extract(0,s.length(),tmp, "UTF-8");
   म_लिखो("CountEquivalent('%s') = %d\n", tmp, result);
 }
#endif
    परत result;
}

स्थिर स्थिरांक icu::Hashtable* getCurrSymbolsEquiv();

//------------------------------------------------------------
// Code

/**
 * Cleanup callback func
 */
स्थिर UBool U_CALLCONV 
isoCodes_cleanup()
{
    जर (gIsoCodes != nullptr) {
        uhash_close(const_cast<UHashtable *>(gIsoCodes));
        gIsoCodes = nullptr;
    }
    gIsoCodesInitOnce.reset();
    परत सत्य;
}

/**
 * Cleanup callback func
 */
स्थिर UBool U_CALLCONV 
currSymbolsEquiv_cleanup()
{
    delete const_cast<icu::Hashtable *>(gCurrSymbolsEquiv);
    gCurrSymbolsEquiv = nullptr;
    gCurrSymbolsEquivInitOnce.reset();
    परत सत्य;
}

/**
 * Deleter for IsoCodeEntry
 */
स्थिर शून्य U_CALLCONV
deleteIsoCodeEntry(शून्य *obj) {
    IsoCodeEntry* entry = static_cast<IsoCodeEntry*>(obj);
    uprv_free(entry);
}

/**
 * Deleter for gCurrSymbolsEquiv.
 */
स्थिर शून्य U_CALLCONV
deleteUnicode(शून्य *obj) {
    icu::UnicodeString* entry = static_cast<icu::UnicodeString*>(obj);
    delete entry;
}

/**
 * Unfortunately, we have to convert the char16_t* currency code to char*
 * to use it as a resource key.
 */
स्थिर ओळीत अक्षर*
myUCharsToChars(अक्षर* resultOfLen4, स्थिरांक char16_t* currency) {
    u_UCharsToChars(currency, resultOfLen4, ISO_CURRENCY_CODE_LENGTH);
    resultOfLen4[ISO_CURRENCY_CODE_LENGTH] = 0;
    परत resultOfLen4;
}

/**
 * Internal function to look up currency data.  Result is an array of
 * four integers.  The first is the fraction digits.  The second is the
 * rounding increment, or 0 if none.  The rounding increment is in
 * units of 10^(-fraction_digits).  The third and fourth are the same
 * except that they are those used in cash transactions ( cashDigits
 * and cashRounding ).
 */
स्थिर स्थिरांक int32_t*
_findMetaData(स्थिरांक char16_t* currency, UErrorCode& ec) {

    जर (currency == nullptr || *currency == 0) {
        जर (U_SUCCESS(ec)) {
            ec = U_ILLEGAL_ARGUMENT_ERROR;
        }
        परत LAST_RESORT_DATA;
    }

    // Get CurrencyMeta resource out of root locale file.  [This may
    // move out of the root locale file later; if it does, update this
    // code.]
    UResourceBundle* currencyData = ures_openDirect(U_ICUDATA_CURR, CURRENCY_DATA, &ec);
    LocalUResourceBundlePointer currencyMeta(ures_get_By_किल्ली(currencyData, CURRENCY_META, currencyData, &ec));

    जर (U_FAILURE(ec)) {
        // Config/build error; return hard-coded defaults
        परत LAST_RESORT_DATA;
    }

    // Look up our currency, or if that's not available, then DEFAULT
    अक्षर buf[ISO_CURRENCY_CODE_LENGTH+1];
    UErrorCode ec2 = U_ZERO_ERROR; // local error code: soft failure
    LocalUResourceBundlePointer rb(ures_get_By_किल्ली(currencyMeta.getAlias(), myUCharsToChars(buf, currency), nullptr, &ec2));
      जर (U_FAILURE(ec2)) {
        rb.adoptInstead(ures_get_By_किल्ली(currencyMeta.getAlias(),DEFAULT_META, nullptr, &ec));
        जर (U_FAILURE(ec)) {
            // Config/build error; return hard-coded defaults
            परत LAST_RESORT_DATA;
        }
    }

    int32_t len;
    स्थिरांक int32_t *data = ures_getIntVector(rb.getAlias(), &len, &ec);
    जर (U_FAILURE(ec) || len != 4) {
        // Config/build error; return hard-coded defaults
        जर (U_SUCCESS(ec)) {
            ec = U_INVALID_FORMAT_ERROR;
        }
        परत LAST_RESORT_DATA;
    }

    परत data;
}

// -------------------------------------

स्थिर CharString
idForLocale(स्थिरांक अक्षर* locale, UErrorCode* ec)
{
    परत ulocimp_getRegionForSupplementalData(locale, असत्य, *ec);
}

// ------------------------------------------
//
// Registration
//
//-------------------------------------------

// don't use ICUService since we don't need fallback

U_CDECL_BEGIN
स्थिर UBool U_CALLCONV currency_cleanup();
U_CDECL_END

#जर !UCONFIG_NO_SERVICE
संरचना CReg;

स्थिर UMutex gCRegLock;
स्थिर CReg* gCRegHead = nullptr;

संरचना CReg : सार्वजनिक icu::UMemory {
    CReg *next;
    char16_t iso[ISO_CURRENCY_CODE_LENGTH+1];
    अक्षर  id[ULOC_FULLNAME_CAPACITY];

    CReg(स्थिरांक char16_t* _iso, स्थिरांक अक्षर* _id)
        : next(nullptr)
    {
        uprv_strncpy(id, _id, आकार(id)-1);
        id[आकार(id)-1] = 0;
        u_memcpy(iso, _iso, ISO_CURRENCY_CODE_LENGTH);
        iso[ISO_CURRENCY_CODE_LENGTH] = 0;
    }

    स्थिर UCurr_Registry_किल्ली reg(स्थिरांक char16_t* _iso, स्थिरांक अक्षर* _id, UErrorCode* status)
    {
        जर (status && U_SUCCESS(*status) && _iso && _id) {
            CReg* n = नवीन CReg(_iso, _id);
            जर (n) {
                umtx_lock(&gCRegLock);
                जर (!gCRegHead) {
                    /* register for the first time */
                    ucln_common_registerCleanup(UCLN_COMMON_CURRENCY, currency_cleanup);
                }
                n->next = gCRegHead;
                gCRegHead = n;
                umtx_unlock(&gCRegLock);
                परत n;
            }
            *status = U_MEMORY_ALLOCATION_ERROR;
        }
        परत nullptr;
    }

    स्थिर UBool unreg(UCurr_Registry_किल्ली किल्ली) {
        UBool found = असत्य;
        umtx_lock(&gCRegLock);

        CReg** p = &gCRegHead;
        पर्यंत (*p) {
            जर (*p == किल्ली) {
                *p = ((CReg*)किल्ली)->next;
                delete (CReg*)किल्ली;
                found = सत्य;
                थांबा;
            }
            p = &((*p)->next);
        }

        umtx_unlock(&gCRegLock);
        परत found;
    }

    स्थिर स्थिरांक char16_t* get(स्थिरांक अक्षर* id) {
        स्थिरांक char16_t* result = nullptr;
        umtx_lock(&gCRegLock);
        CReg* p = gCRegHead;

        /* register cleanup of the mutex */
        ucln_common_registerCleanup(UCLN_COMMON_CURRENCY, currency_cleanup);
        पर्यंत (p) {
            जर (uprv_strcmp(id, p->id) == 0) {
                result = p->iso;
                थांबा;
            }
            p = p->next;
        }
        umtx_unlock(&gCRegLock);
        परत result;
    }

    /* This doesn't need to be thread safe. It's for u_cleanup only. */
    स्थिर शून्य cleanup() {
        पर्यंत (gCRegHead) {
            CReg* n = gCRegHead;
            gCRegHead = gCRegHead->next;
            delete n;
        }
    }
};

// -------------------------------------

U_CAPI UCurr_Registry_किल्ली U_EXPORT2
ucurr_register(स्थिरांक char16_t* isoCode, स्थिरांक अक्षर* locale, UErrorCode *status)
{
    जर (status && U_SUCCESS(*status)) {
        CharString id = idForLocale(locale, status);
        परत CReg::reg(isoCode, id.data(), status);
    }
    परत nullptr;
}

// -------------------------------------

U_CAPI UBool U_EXPORT2
ucurr_unregister(UCurr_Registry_किल्ली किल्ली, UErrorCode* status)
{
    जर (status && U_SUCCESS(*status)) {
        परत CReg::unreg(किल्ली);
    }
    परत असत्य;
}
#endif /* UCONFIG_NO_SERVICE */

// -------------------------------------

/**
 * Release all static memory held by currency.
 */
/*The declaration here is needed so currency_cleanup()
 * can call this function.
 */
स्थिर UBool U_CALLCONV
currency_cache_cleanup();

U_CDECL_BEGIN
स्थिर UBool U_CALLCONV currency_cleanup() {
#जर !UCONFIG_NO_SERVICE
    CReg::cleanup();
#endif
    /*
     * There might be some cached currency data or isoCodes data.
     */
    currency_cache_cleanup();
    isoCodes_cleanup();
    currSymbolsEquiv_cleanup();

    परत सत्य;
}
U_CDECL_END

// -------------------------------------

U_CAPI int32_t U_EXPORT2
ucurr_forLocale(स्थिरांक अक्षर* locale,
                char16_t* buff,
                int32_t buffCapacity,
                UErrorCode* ec) {
    जर (U_FAILURE(*ec)) { परत 0; }
    जर (buffCapacity < 0 || (buff == nullptr && buffCapacity > 0)) {
        *ec = U_ILLEGAL_ARGUMENT_ERROR;
        परत 0;
    }

    UErrorCode localStatus = U_ZERO_ERROR;
    CharString currency = ulocimp_get_Keyword_मूल्य(locale, "currency", localStatus);
    int32_t resLen = currency.length();

    जर (U_SUCCESS(localStatus) && resLen == 3 && uprv_isInvariantString(currency.data(), resLen)) {
        जर (resLen < buffCapacity) {
            T_CString_toUpperCase(currency.data());
            u_charsToUChars(currency.data(), buff, resLen);
        }
        परत u_terminateUChars(buff, buffCapacity, resLen, ec);
    }

    // get country or country_variant in `id'
    CharString id = idForLocale(locale, ec);
    जर (U_FAILURE(*ec)) {
        परत 0;
    }

#जर !UCONFIG_NO_SERVICE
    स्थिरांक char16_t* result = CReg::get(id.data());
    जर (result) {
        जर(buffCapacity > u_strlen(result)) {
            u_strcpy(buff, result);
        }
        resLen = u_strlen(result);
        परत u_terminateUChars(buff, buffCapacity, resLen, ec);
    }
#endif
    // Remove variants, which is only needed for registration.
    अक्षर *idDelim = uprv_strchr(id.data(), VAR_DELIM);
    जर (idDelim) {
        id.truncate(idDelim - id.data());
    }

    स्थिरांक char16_t* s = nullptr;  // Currency code from data file.
    जर (id.isEmpty()) {
        // No point looking in the data for an empty string.
        // This is what we would get.
        localStatus = U_MISSING_RESOURCE_ERROR;
    } अन्यथा {
        // Look up the CurrencyMap element in the root bundle.
        localStatus = U_ZERO_ERROR;
        UResourceBundle *rb = ures_openDirect(U_ICUDATA_CURR, CURRENCY_DATA, &localStatus);
        UResourceBundle *cm = ures_get_By_किल्ली(rb, CURRENCY_MAP, rb, &localStatus);
        LocalUResourceBundlePointer countryArray(ures_get_By_किल्ली(rb, id.data(), cm, &localStatus));
        // https://unicode-org.atlassian.net/browse/ICU-21997
        // Prefer to use currencies that are legal tender.
        जर (U_SUCCESS(localStatus)) {
            int32_t arrayLength = ures_getSize(countryArray.getAlias());
            साठी (int32_t i = 0; i < arrayLength; ++i) {
                LocalUResourceBundlePointer currencyReq(
                    ures_getByIndex(countryArray.getAlias(), i, nullptr, &localStatus));
                // The currency is legal tender if it is *not* marked with tender{"false"}.
                UErrorCode tenderStatus = localStatus;
                स्थिरांक char16_t *tender =
                    ures_get_String_By_किल्ली(currencyReq.getAlias(), "tender", nullptr, &tenderStatus);
                तार्किक isTender = U_FAILURE(tenderStatus) || u_strcmp(tender, u"false") != 0;
                जर (!isTender && s != nullptr) {
                    // We already have a non-tender currency. Ignore all following non-tender ones.
                    सुरूठेवा;
                }
                // Fetch the currency code.
                s = ures_get_String_By_किल्ली(currencyReq.getAlias(), "id", &resLen, &localStatus);
                जर (isTender) {
                    थांबा;
                }
            }
            जर (U_SUCCESS(localStatus) && s == nullptr) {
                localStatus = U_MISSING_RESOURCE_ERROR;
            }
        }
    }

    जर ((U_FAILURE(localStatus)) && म_अक्षर(id.data(), '_') != nullptr) {
        // We don't know about it.  Check to see if we support the variant.
        CharString parent = ulocimp_getParent(locale, *ec);
        *ec = U_USING_FALLBACK_WARNING;
        // TODO: Loop over the parent rather than recursing and
        // looking again for a currency keyword.
        परत ucurr_forLocale(parent.data(), buff, buffCapacity, ec);
    }
    जर (*ec == U_ZERO_ERROR || localStatus != U_ZERO_ERROR) {
        // There is nothing to fallback to. Report the failure/warning if possible.
        *ec = localStatus;
    }
    जर (U_SUCCESS(*ec)) {
        जर(buffCapacity > resLen) {
            u_strcpy(buff, s);
        }
    }
    परत u_terminateUChars(buff, buffCapacity, resLen, ec);
}

// end registration

/**
 * Modify the given locale name by removing the rightmost _-delimited
 * element.  If there is none, empty the string ("" == root).
 * NOTE: The string "root" is not recognized; do not use it.
 * @return true if the fallback happened; false if locale is already
 * root ("").
 */
स्थिर UBool fallback(CharString& loc) {
    जर (loc.isEmpty()) {
        परत असत्य;
    }
    UErrorCode status = U_ZERO_ERROR;
    जर (loc == "en_GB") {
        // HACK: See #13368.  We need "en_GB" to fall back to "en_001" instead of "en"
        // in order to consume the correct data strings.  This hack will be removed
        // when proper data sink loading is implemented here.
        loc.truncate(3);
        loc.append("001", status);
    } अन्यथा {
        loc = ulocimp_getParent(loc.data(), status);
    }
 /*
    char *i = uprv_strrchr(loc, '_');
    if (i == nullptr) {
        i = loc;
    }
    *i = 0;
 */
    परत सत्य;
}


U_CAPI स्थिरांक char16_t* U_EXPORT2
ucurr_getName(स्थिरांक char16_t* currency U_LIFETIME_BOUND,
              स्थिरांक अक्षर* locale,
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

    जर (U_FAILURE(*ec)) {
        परत nullptr;
    }

    int32_t choice = (int32_t) nameStyle;
    जर (choice < 0 || choice > 4) {
        *ec = U_ILLEGAL_ARGUMENT_ERROR;
        परत nullptr;
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

    जर (locale == nullptr) {
        locale = uloc_getDefault();
    }
    CharString loc = ulocimp_getName(locale, ec2);
    जर (U_FAILURE(ec2)) {
        *ec = U_ILLEGAL_ARGUMENT_ERROR;
        परत nullptr;
    }

    अक्षर buf[ISO_CURRENCY_CODE_LENGTH+1];
    myUCharsToChars(buf, currency);
    
    /* Normalize the keyword value to uppercase */
    T_CString_toUpperCase(buf);
    
    स्थिरांक char16_t* s = nullptr;
    ec2 = U_ZERO_ERROR;
    LocalUResourceBundlePointer rb(ures_open(U_ICUDATA_CURR, loc.data(), &ec2));

    जर (nameStyle == UCURR_NARROW_चिन्ह_NAME || nameStyle == UCURR_FORMAL_चिन्ह_NAME || nameStyle == UCURR_VARIANT_चिन्ह_NAME) {
        CharString किल्ली;
        निवड (nameStyle) {
        स्थिती UCURR_NARROW_चिन्ह_NAME:
            किल्ली.append(CURRENCIES_NARROW, ec2);
            थांबा;
        स्थिती UCURR_FORMAL_चिन्ह_NAME:
            किल्ली.append(CURRENCIES_FORMAL, ec2);
            थांबा;
        स्थिती UCURR_VARIANT_चिन्ह_NAME:
            किल्ली.append(CURRENCIES_VARIANT, ec2);
            थांबा;
        पूर्वनिर्धारित:
            *ec = U_UNSUPPORTED_ERROR;
            परत nullptr;
        }
        किल्ली.append("/", ec2);
        किल्ली.append(buf, ec2);
        s = ures_get_String_By_किल्ली_With_Fallback(rb.getAlias(), किल्ली.data(), len, &ec2);
        जर (ec2 == U_MISSING_RESOURCE_ERROR) {
            *ec = U_USING_FALLBACK_WARNING;
            ec2 = U_ZERO_ERROR;
            choice = UCURR_चिन्ह_NAME;
        }
    }
    जर (s == nullptr) {
        ures_get_By_किल्ली(rb.getAlias(), CURRENCIES, rb.getAlias(), &ec2);
        ures_get_By_किल्ली_With_Fallback(rb.getAlias(), buf, rb.getAlias(), &ec2);
        s = ures_getStringByIndex(rb.getAlias(), choice, len, &ec2);
    }

    // If we've succeeded we're done.  Otherwise, try to fallback.
    // If that fails (because we are already at root) then exit.
    जर (U_SUCCESS(ec2)) {
        जर (ec2 == U_USING_DEFAULT_WARNING
            || (ec2 == U_USING_FALLBACK_WARNING && *ec != U_USING_DEFAULT_WARNING)) {
            *ec = ec2;
        }
    }

    // We no longer support choice format data in names.  Data should not contain
    // choice patterns.
    जर (isChoiceFormat != nullptr) {
        *isChoiceFormat = असत्य;
    }
    जर (U_SUCCESS(ec2)) {
        U_ASSERT(s != nullptr);
        परत s;
    }

    // If we fail to find a match, use the ISO 4217 code
    *len = u_strlen(currency); // Should == ISO_CURRENCY_CODE_LENGTH, but maybe not...?
    *ec = U_USING_DEFAULT_WARNING;
    परत currency;
}

U_CAPI स्थिरांक char16_t* U_EXPORT2
ucurr_getPluralName(स्थिरांक char16_t* currency U_LIFETIME_BOUND,
                    स्थिरांक अक्षर* locale,
                    UBool* isChoiceFormat,
                    स्थिरांक अक्षर* plural_मोजणी,
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

    जर (U_FAILURE(*ec)) {
        परत nullptr;
    }

    // Use a separate UErrorCode here that does not propagate out of
    // this function.
    UErrorCode ec2 = U_ZERO_ERROR;

    जर (locale == nullptr) {
        locale = uloc_getDefault();
    }
    CharString loc = ulocimp_getName(locale, ec2);
    जर (U_FAILURE(ec2)) {
        *ec = U_ILLEGAL_ARGUMENT_ERROR;
        परत nullptr;
    }

    अक्षर buf[ISO_CURRENCY_CODE_LENGTH+1];
    myUCharsToChars(buf, currency);

    स्थिरांक char16_t* s = nullptr;
    ec2 = U_ZERO_ERROR;
    UResourceBundle* rb = ures_open(U_ICUDATA_CURR, loc.data(), &ec2);

    rb = ures_get_By_किल्ली(rb, CURRENCYPLURALS, rb, &ec2);

    // Fetch resource with multi-level resource inheritance fallback
    LocalUResourceBundlePointer curr(ures_get_By_किल्ली_With_Fallback(rb, buf, rb, &ec2));

    s = ures_get_String_By_किल्ली_With_Fallback(curr.getAlias(), plural_मोजणी, len, &ec2);
    जर (U_FAILURE(ec2)) {
        //  fall back to "other"
        ec2 = U_ZERO_ERROR;
        s = ures_get_String_By_किल्ली_With_Fallback(curr.getAlias(), "other", len, &ec2);     
        जर (U_FAILURE(ec2)) {
            // fall back to long name in Currencies
            परत ucurr_getName(currency, locale, UCURR_LONG_NAME, 
                                 isChoiceFormat, len, ec);
        }
    }

    // If we've succeeded we're done.  Otherwise, try to fallback.
    // If that fails (because we are already at root) then exit.
    जर (U_SUCCESS(ec2)) {
        जर (ec2 == U_USING_DEFAULT_WARNING
            || (ec2 == U_USING_FALLBACK_WARNING && *ec != U_USING_DEFAULT_WARNING)) {
            *ec = ec2;
        }
        U_ASSERT(s != nullptr);
        परत s;
    }

    // If we fail to find a match, use the ISO 4217 code
    *len = u_strlen(currency); // Should == ISO_CURRENCY_CODE_LENGTH, but maybe not...?
    *ec = U_USING_DEFAULT_WARNING;
    परत currency;
}


//========================================================================
// Following are structure and function for parsing currency names

#define NEED_TO_BE_DELETED 0x1

// TODO: a better way to define this?
#define MAX_CURRENCY_NAME_LEN 100

प्रकारव्याख्या संरचना {
    स्थिरांक अक्षर* IsoCode;  //किल्ली
    char16_t* currencyName;  //मूल्य
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
स्थिर पूर्णांक U_CALLCONV currencyNameComparator(स्थिरांक शून्य* a, स्थिरांक शून्य* b) {
    स्थिरांक CurrencyNameStruct* currName_1 = static_cast<स्थिरांक CurrencyNameStruct*>(a);
    स्थिरांक CurrencyNameStruct* currName_2 = static_cast<स्थिरांक CurrencyNameStruct*>(b);
    साठी (int32_t i = 0; 
         i < MIN(currName_1->currencyNameLen, currName_2->currencyNameLen);
         ++i) {
        जर (currName_1->currencyName[i] < currName_2->currencyName[i]) {
            परत -1;
        }
        जर (currName_1->currencyName[i] > currName_2->currencyName[i]) {
            परत 1;
        }
    }
    जर (currName_1->currencyNameLen < currName_2->currencyNameLen) {
        परत -1;
    } अन्यथा जर (currName_1->currencyNameLen > currName_2->currencyNameLen) {
        परत 1;
    }
    परत 0;
}


// Give a locale, return the maximum number of currency names associated with
// this locale.
// It gets currency names from resource bundles using fallback.
// It is the maximum number because in the fallback chain, some of the 
// currency names are duplicated.
// For example, given locale as "en_US", the currency names get from resource
// bundle in "en_US" and "en" are duplicated. The fallback mechanism will count
// all currency names in "en_US" and "en".
स्थिर शून्य
get_Currency_Name_मोजणी(स्थिरांक अक्षर* loc, int32_t* total_currency_name_मोजणी, int32_t* total_currency_चिन्ह_मोजणी) {
    U_NAMESPACE_USE
    *total_currency_name_मोजणी = 0;
    *total_currency_चिन्ह_मोजणी = 0;
    स्थिरांक char16_t* s = nullptr;
    CharString locale;
    {
        UErrorCode status = U_ZERO_ERROR;
        locale.append(loc, status);
        जर (U_FAILURE(status)) { परत; }
    }
    स्थिरांक icu::Hashtable *currencySymbolsEquiv = getCurrSymbolsEquiv();
    साठी (;;) {
        UErrorCode ec2 = U_ZERO_ERROR;
        // TODO: ures_openDirect?
        LocalUResourceBundlePointer rb(ures_open(U_ICUDATA_CURR, locale.data(), &ec2));
        LocalUResourceBundlePointer curr(ures_get_By_किल्ली(rb.getAlias(), CURRENCIES, nullptr, &ec2));
        int32_t n = ures_getSize(curr.getAlias());
        साठी (int32_t i=0; i<n; ++i) {
            LocalUResourceBundlePointer names(ures_getByIndex(curr.getAlias(), i, nullptr, &ec2));
            int32_t len;
            s = ures_getStringByIndex(names.getAlias(), UCURR_चिन्ह_NAME, &len, &ec2);
            ++(*total_currency_चिन्ह_मोजणी);  // currency symbol
            जर (currencySymbolsEquiv != nullptr) {
                *total_currency_चिन्ह_मोजणी += मोजणी_Equivalent(*currencySymbolsEquiv, UnicodeString(सत्य, s, len));
            }
            ++(*total_currency_चिन्ह_मोजणी); // iso code
            ++(*total_currency_name_मोजणी); // long name
        }

        // currency plurals
        UErrorCode ec3 = U_ZERO_ERROR;
        LocalUResourceBundlePointer curr_p(ures_get_By_किल्ली(rb.getAlias(), CURRENCYPLURALS, nullptr, &ec3));
        n = ures_getSize(curr_p.getAlias());
        साठी (int32_t i=0; i<n; ++i) {
            LocalUResourceBundlePointer names(ures_getByIndex(curr_p.getAlias(), i, nullptr, &ec3));
            *total_currency_name_मोजणी += ures_getSize(names.getAlias());
        }

        जर (!fallback(locale)) {
            थांबा;
        }
    }
}

स्थिर char16_t*
toUpperCase(स्थिरांक char16_t* source, int32_t len, स्थिरांक अक्षर* locale) {
    char16_t* dest = nullptr;
    UErrorCode ec = U_ZERO_ERROR;
    int32_t destLen = u_strToUpper(dest, 0, source, len, locale, &ec);

    ec = U_ZERO_ERROR;
    dest = static_cast<char16_t*>(uprv_malloc(आकार(char16_t) * MAX(destLen, len)));
    जर (dest == nullptr) {
        परत nullptr;
    }
    u_strToUpper(dest, destLen, source, len, locale, &ec);
    जर (U_FAILURE(ec)) {
        u_memcpy(dest, source, len);
    } 
    परत dest;
}


स्थिर शून्य deleteCurrencyNames(CurrencyNameStruct* currencyNames, int32_t count);
// Collect all available currency names associated with the given locale
// (enable fallback chain).
// Read currenc names defined in resource bundle "Currencies" and
// "CurrencyPlural", enable fallback chain.
// return the malloc-ed currency name arrays and the total number of currency
// names in the array.
स्थिर शून्य
collectCurrencyNames(स्थिरांक अक्षर* locale, 
                     CurrencyNameStruct** currencyNames, 
                     int32_t* total_currency_name_मोजणी, 
                     CurrencyNameStruct** currencySymbols, 
                     int32_t* total_currency_चिन्ह_मोजणी, 
                     UErrorCode& ec) {
    जर (U_FAILURE(ec)) {
        *currencyNames = *currencySymbols = nullptr;
        *total_currency_name_मोजणी = *total_currency_चिन्ह_मोजणी = 0;
        परत;
    }
    U_NAMESPACE_USE
    स्थिरांक icu::Hashtable *currencySymbolsEquiv = getCurrSymbolsEquiv();
    // Look up the Currencies resource for the given locale.
    UErrorCode ec2 = U_ZERO_ERROR;

    जर (locale == nullptr) {
        locale = uloc_getDefault();
    }
    CharString loc = ulocimp_getName(locale, ec2);
    जर (U_FAILURE(ec2)) {
        ec = U_ILLEGAL_ARGUMENT_ERROR;
        *currencyNames = *currencySymbols = nullptr;
        *total_currency_name_मोजणी = *total_currency_चिन्ह_मोजणी = 0;
        परत;
    }

    // Get maximum currency name count first.
    get_Currency_Name_मोजणी(loc.data(), total_currency_name_मोजणी, total_currency_चिन्ह_मोजणी);

    *currencyNames = static_cast<CurrencyNameStruct*>(
        uprv_malloc(आकार(CurrencyNameStruct) * (*total_currency_name_मोजणी)));
    जर(*currencyNames == nullptr) {
        *currencySymbols = nullptr;
        *total_currency_name_मोजणी = *total_currency_चिन्ह_मोजणी = 0;
        ec = U_MEMORY_ALLOCATION_ERROR;
        परत;
    }
    *currencySymbols = static_cast<CurrencyNameStruct*>(
        uprv_malloc(आकार(CurrencyNameStruct) * (*total_currency_चिन्ह_मोजणी)));

    जर(*currencySymbols == nullptr) {
        uprv_free(*currencyNames);
        *currencyNames = nullptr;
        *total_currency_name_मोजणी = *total_currency_चिन्ह_मोजणी = 0;
        ec = U_MEMORY_ALLOCATION_ERROR;
        परत;
    }

    स्थिरांक char16_t* s = nullptr;  // currency name
    अक्षर* iso = nullptr;  // currency ISO code

    *total_currency_name_मोजणी = 0;
    *total_currency_चिन्ह_मोजणी = 0;

    UErrorCode ec3 = U_ZERO_ERROR;
    UErrorCode ec4 = U_ZERO_ERROR;

    // Using hash to remove duplicates caused by locale fallback
    LocalUHashtablePointer currencyIsoCodes(uhash_open(uhash_hashChars, uhash_compareChars, nullptr, &ec3));
    LocalUHashtablePointer currencyPluralIsoCodes(uhash_open(uhash_hashChars, uhash_compareChars, nullptr, &ec4));
    साठी (int32_t localeLevel = 0; ; ++localeLevel) {
        ec2 = U_ZERO_ERROR;
        // TODO: ures_openDirect
        LocalUResourceBundlePointer rb(ures_open(U_ICUDATA_CURR, loc.data(), &ec2));
        LocalUResourceBundlePointer curr(ures_get_By_किल्ली(rb.getAlias(), CURRENCIES, nullptr, &ec2));
        int32_t n = ures_getSize(curr.getAlias());
        साठी (int32_t i=0; i<n; ++i) {
            LocalUResourceBundlePointer names(ures_getByIndex(curr.getAlias(), i, nullptr, &ec2));
            int32_t len;
            s = ures_getStringByIndex(names.getAlias(), UCURR_चिन्ह_NAME, &len, &ec2);
            // TODO: uhash_put wont change key/value?
            iso = const_cast<अक्षर*>(ures_get_किल्ली(names.getAlias()));
            जर (localeLevel != 0 && uhash_get(currencyIsoCodes.getAlias(), iso) != nullptr) {
                सुरूठेवा;
            }
            uhash_put(currencyIsoCodes.getAlias(), iso, iso, &ec3); 
            // Add currency symbol.
            (*currencySymbols)[(*total_currency_चिन्ह_मोजणी)++] = {iso, const_cast<char16_t*>(s), len, 0};

            // Add equivalent symbols
            जर (currencySymbolsEquiv != nullptr) {
                UnicodeString str(सत्य, s, len);
                icu::EquivIterator iter(*currencySymbolsEquiv, str);
                स्थिरांक UnicodeString *चिन्ह;
                पर्यंत ((चिन्ह = iter.next()) != nullptr) {
                    (*currencySymbols)[(*total_currency_चिन्ह_मोजणी)++]
                        = {iso, const_cast<char16_t*>(चिन्ह->getBuffer()), चिन्ह->length(), 0};
                }
            }

            // Add currency long name.
            s = ures_getStringByIndex(names.getAlias(), UCURR_LONG_NAME, &len, &ec2);
            char16_t* upperName = toUpperCase(s, len, locale);
            जर (upperName == nullptr) {
                ec = U_MEMORY_ALLOCATION_ERROR;
                जा error;
            }
            (*currencyNames)[(*total_currency_name_मोजणी)++] = {iso, upperName, len, NEED_TO_BE_DELETED};

            // put (iso, 3, and iso) in to array
            // Add currency ISO code.
            char16_t* isoCode = static_cast<char16_t*>(uprv_malloc(आकार(char16_t) * 3));
            जर (isoCode == nullptr) {
                ec = U_MEMORY_ALLOCATION_ERROR;
                जा error;
            }
            // Must convert iso[] into Unicode
            u_charsToUChars(iso, isoCode, 3);
            (*currencySymbols)[(*total_currency_चिन्ह_मोजणी)++] = {iso, isoCode, 3, NEED_TO_BE_DELETED};
        }

        // currency plurals
        UErrorCode ec5 = U_ZERO_ERROR;
        LocalUResourceBundlePointer curr_p(ures_get_By_किल्ली(rb.getAlias(), CURRENCYPLURALS, nullptr, &ec5));
        n = ures_getSize(curr_p.getAlias());
        साठी (int32_t i=0; i<n; ++i) {
            LocalUResourceBundlePointer names(ures_getByIndex(curr_p.getAlias(), i, nullptr, &ec5));
            iso = const_cast<अक्षर*>(ures_get_किल्ली(names.getAlias()));
            // Using hash to remove duplicated ISO codes in fallback chain.
            जर (localeLevel != 0 && uhash_get(currencyPluralIsoCodes.getAlias(), iso) != nullptr) {
                सुरूठेवा;
            }
            uhash_put(currencyPluralIsoCodes.getAlias(), iso, iso, &ec4);
            int32_t num = ures_getSize(names.getAlias());
            int32_t len;
            साठी (int32_t j = 0; j < num; ++j) {
                // TODO: remove duplicates between singular name and 
                // currency long name?
                s = ures_getStringByIndex(names.getAlias(), j, &len, &ec5);
                char16_t* upperName = toUpperCase(s, len, locale);
                जर (upperName == nullptr) {
                    ec = U_MEMORY_ALLOCATION_ERROR;
                    जा error;
                }
                (*currencyNames)[(*total_currency_name_मोजणी)++] = {iso, upperName, len, NEED_TO_BE_DELETED};
            }
        }

        जर (!fallback(loc)) {
            थांबा;
        }
    }

    // quick sort the struct
    क्विक(*currencyNames, *total_currency_name_मोजणी, 
          आकार(CurrencyNameStruct), currencyNameComparator);
    क्विक(*currencySymbols, *total_currency_चिन्ह_मोजणी, 
          आकार(CurrencyNameStruct), currencyNameComparator);

#ifdef UCURR_DEBUG
    म_लिखो("currency name count: %d\n", *total_currency_name_मोजणी);
    साठी (int32_t index = 0; index < *total_currency_name_मोजणी; ++index) {
        म_लिखो("index: %d\n", index);
        म_लिखो("iso: %s\n", (*currencyNames)[index].IsoCode);
        अक्षर curNameBuf[1024];
        स_रखो(curNameBuf, 0, 1024);
        u_austrncpy(curNameBuf, (*currencyNames)[index].currencyName, (*currencyNames)[index].currencyNameLen);
        म_लिखो("currencyName: %s\n", curNameBuf);
        म_लिखो("len: %d\n", (*currencyNames)[index].currencyNameLen);
    }
    म_लिखो("currency symbol count: %d\n", *total_currency_चिन्ह_मोजणी);
    साठी (int32_t index = 0; index < *total_currency_चिन्ह_मोजणी; ++index) {
        म_लिखो("index: %d\n", index);
        म_लिखो("iso: %s\n", (*currencySymbols)[index].IsoCode);
        अक्षर curNameBuf[1024];
        स_रखो(curNameBuf, 0, 1024);
        u_austrncpy(curNameBuf, (*currencySymbols)[index].currencyName, (*currencySymbols)[index].currencyNameLen);
        म_लिखो("currencySymbol: %s\n", curNameBuf);
        म_लिखो("len: %d\n", (*currencySymbols)[index].currencyNameLen);
    }
#endif
    // fail on hashtable errors
    जर (U_FAILURE(ec3)) {
      ec = ec3;
    } अन्यथा जर (U_FAILURE(ec4)) {
      ec = ec4;
    }

  error:
    // clean up if we got error
    जर (U_FAILURE(ec)) {
        deleteCurrencyNames(*currencyNames, *total_currency_name_मोजणी);
        deleteCurrencyNames(*currencySymbols, *total_currency_चिन्ह_मोजणी);
        *currencyNames = *currencySymbols = nullptr;
        *total_currency_name_मोजणी = *total_currency_चिन्ह_मोजणी = 0;
    }
}

// @param  currencyNames: currency names array
// @param  indexInCurrencyNames: the index of the character in currency names 
//         array against which the comparison is done
// @param  key: input text char to compare against
// @param  begin(IN/OUT): the begin index of matching range in currency names array
// @param  end(IN/OUT): the end index of matching range in currency names array.
स्थिर int32_t
द्विमान_शोध(स्थिरांक CurrencyNameStruct* currencyNames, 
             int32_t indexInCurrencyNames,
             स्थिरांक char16_t किल्ली,
             int32_t* begin, int32_t* end) {
#ifdef UCURR_DEBUG
    म_लिखो("key = %x\n", किल्ली);
#endif
   int32_t first = *begin;
   int32_t last = *end;
   पर्यंत (first <= last) {
       int32_t mid = (first + last) / 2;  // compute mid point.
       जर (indexInCurrencyNames >= currencyNames[mid].currencyNameLen) {
           first = mid + 1;
       } अन्यथा {
           जर (किल्ली > currencyNames[mid].currencyName[indexInCurrencyNames]) {
               first = mid + 1;
           }
           अन्यथा जर (किल्ली < currencyNames[mid].currencyName[indexInCurrencyNames]) {
               last = mid - 1;
           }
           अन्यथा {
                // Find a match, and looking for ranges
                // Now do two more binary searches. First, on the left side for
                // the greatest L such that CurrencyNameStruct[L] < key.
                int32_t L = *begin;
                int32_t R = mid;

#ifdef UCURR_DEBUG
                म_लिखो("mid = %d\n", mid);
#endif
                पर्यंत (L < R) {
                    int32_t M = (L + R) / 2;
#ifdef UCURR_DEBUG
                    म_लिखो("L = %d, R = %d, M = %d\n", L, R, M);
#endif
                    जर (indexInCurrencyNames >= currencyNames[M].currencyNameLen) {
                        L = M + 1;
                    } अन्यथा {
                        जर (currencyNames[M].currencyName[indexInCurrencyNames] < किल्ली) {
                            L = M + 1;
                        } अन्यथा {
#ifdef UCURR_DEBUG
                            U_ASSERT(currencyNames[M].currencyName[indexInCurrencyNames] == किल्ली);
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
                म_लिखो("begin = %d\n", *begin);
                U_ASSERT(currencyNames[*begin].currencyName[indexInCurrencyNames] == किल्ली);
#endif

                // Now for the second search, finding the least R such that
                // key < CurrencyNameStruct[R].
                L = mid;
                R = *end;
                पर्यंत (L < R) {
                    int32_t M = (L + R) / 2;
#ifdef UCURR_DEBUG
                    म_लिखो("L = %d, R = %d, M = %d\n", L, R, M);
#endif
                    जर (currencyNames[M].currencyNameLen < indexInCurrencyNames) {
                        L = M + 1;
                    } अन्यथा {
                        जर (currencyNames[M].currencyName[indexInCurrencyNames] > किल्ली) {
                            R = M;
                        } अन्यथा {
#ifdef UCURR_DEBUG
                            U_ASSERT(currencyNames[M].currencyName[indexInCurrencyNames] == किल्ली);
#endif
                            L = M + 1;
                        }
                    }
                }
#ifdef UCURR_DEBUG
                U_ASSERT(L == R);
#endif
                जर (currencyNames[R].currencyName[indexInCurrencyNames] > किल्ली) {
                    *end = R - 1;
                } अन्यथा {
                    *end = R;
                }
#ifdef UCURR_DEBUG
                म_लिखो("end = %d\n", *end);
#endif

                // now, found the range. check whether there is exact match
                जर (currencyNames[*begin].currencyNameLen == indexInCurrencyNames + 1) {
                    परत *begin;  // find range and exact match.
                }
                परत -1;  // find range, but no exact match.
           }
       }
   }
   *begin = -1;
   *end = -1;
   परत -1;    // failed to find range.
}


// Linear search "text" in "currencyNames".
// @param  begin, end: the begin and end index in currencyNames, within which
//         range should the search be performed.
// @param  textLen: the length of the text to be compared
// @param  maxMatchLen(IN/OUT): passing in the computed max matching length
//                              pass out the new max  matching length
// @param  maxMatchIndex: the index in currencyName which has the longest
//                        match with input text.
स्थिर शून्य
linear_शोध(स्थिरांक CurrencyNameStruct* currencyNames, 
             int32_t begin, int32_t end,
             स्थिरांक char16_t* text, int32_t textLen,
             int32_t *partialMatchLen,
             int32_t *maxMatchLen, int32_t* maxMatchIndex) {
    int32_t initialPartialMatchLen = *partialMatchLen;
    साठी (int32_t index = begin; index <= end; ++index) {
        int32_t len = currencyNames[index].currencyNameLen;
        जर (len > *maxMatchLen && len <= textLen &&
            uprv_memcmp(currencyNames[index].currencyName, text, len * आकार(char16_t)) == 0) {
            *partialMatchLen = MAX(*partialMatchLen, len);
            *maxMatchIndex = index;
            *maxMatchLen = len;
#ifdef UCURR_DEBUG
            म_लिखो("maxMatchIndex = %d, maxMatchLen = %d\n",
                   *maxMatchIndex, *maxMatchLen);
#endif
        } अन्यथा {
            // Check for partial matches.
            साठी (int32_t i=initialPartialMatchLen; i<MIN(len, textLen); i++) {
                जर (currencyNames[index].currencyName[i] != text[i]) {
                    थांबा;
                }
                *partialMatchLen = MAX(*partialMatchLen, i + 1);
            }
        }
    }
}

#define LINEAR_शोध_THRESHOLD 10

// Find longest match between "text" and currency names in "currencyNames".
// @param  total_currency_count: total number of currency names in CurrencyNames.
// @param  textLen: the length of the text to be compared
// @param  maxMatchLen: passing in the computed max matching length
//                              pass out the new max  matching length
// @param  maxMatchIndex: the index in currencyName which has the longest
//                        match with input text.
स्थिर शून्य
शोध_Currency_Name(स्थिरांक CurrencyNameStruct* currencyNames, 
                   int32_t total_currency_मोजणी,
                   स्थिरांक char16_t* text, int32_t textLen,
                   int32_t *partialMatchLen,
                   int32_t* maxMatchLen, int32_t* maxMatchIndex) {
    *maxMatchIndex = -1;
    *maxMatchLen = 0;
    int32_t matchIndex = -1;
    int32_t द्विमान_शोध_Begin = 0;
    int32_t द्विमान_शोध_End = total_currency_मोजणी - 1;
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
    साठी (int32_t index = 0; index < textLen; ++index) {
        // matchIndex saves the one with exact match till the current point.
        // [binarySearchBegin, binarySearchEnd] saves the matching range.
        matchIndex = द्विमान_शोध(currencyNames, index,
                                  text[index],
                                  &द्विमान_शोध_Begin, &द्विमान_शोध_End);
        जर (द्विमान_शोध_Begin == -1) { // did not find the range
            थांबा;
        }
        *partialMatchLen = MAX(*partialMatchLen, index + 1);
        जर (matchIndex != -1) { 
            // find an exact match for text from text[0] to text[index] 
            // in currencyNames array.
            *maxMatchLen = index + 1;
            *maxMatchIndex = matchIndex;
        }
        जर (द्विमान_शोध_End - द्विमान_शोध_Begin < LINEAR_शोध_THRESHOLD) {
            // linear search if within threshold.
            linear_शोध(currencyNames, द्विमान_शोध_Begin, द्विमान_शोध_End,
                         text, textLen,
                         partialMatchLen,
                         maxMatchLen, maxMatchIndex);
            थांबा;
        }
    }
}

//========================= currency name cache =====================
प्रकारव्याख्या संरचना {
    अक्षर locale[ULOC_FULLNAME_CAPACITY];  //key
    // currency names, case insensitive
    CurrencyNameStruct* currencyNames;  //मूल्य
    int32_t total_Currency_Name_मोजणी;  // currency name count
    // currency symbols and ISO code, case sensitive
    CurrencyNameStruct* currencySymbols; //मूल्य
    int32_t total_Currency_चिन्ह_मोजणी;  //मोजणी
    // reference count.
    // reference count is set to 1 when an entry is put to cache.
    // it increases by 1 before accessing, and decreased by 1 after accessing.
    // The entry is deleted when ref count is zero, which means 
    // the entry is replaced out of cache and no process is accessing it.
    int32_t ref_मोजणी;
} CurrencyNameCacheEntry;


#define CURRENCY_NAME_CACHE_अंक 10

// Reserve 10 cache entries.
स्थिर CurrencyNameCacheEntry* currCache[CURRENCY_NAME_CACHE_अंक] = {nullptr};
// Using an index to indicate which entry to be replaced when cache is full.
// It is a simple round-robin replacement strategy.
स्थिर int8_t currentCacheEntryIndex = 0;

स्थिर UMutex gCurrencyCacheMutex;

// Cache deletion
स्थिर शून्य
deleteCurrencyNames(CurrencyNameStruct* currencyNames, int32_t count) {
    साठी (int32_t index = 0; index < count; ++index) {
        जर ( (currencyNames[index].flag & NEED_TO_BE_DELETED) ) {
            uprv_free(currencyNames[index].currencyName);
        }
    }
    uprv_free(currencyNames);
}


स्थिर शून्य
deleteCacheEntry(CurrencyNameCacheEntry* entry) {
    deleteCurrencyNames(entry->currencyNames, entry->total_Currency_Name_मोजणी);
    deleteCurrencyNames(entry->currencySymbols, entry->total_Currency_चिन्ह_मोजणी);
    uprv_free(entry);
}


// Cache clean up
स्थिर UBool U_CALLCONV
currency_cache_cleanup() {
    साठी (int32_t i = 0; i < CURRENCY_NAME_CACHE_अंक; ++i) {
        जर (currCache[i]) {
            deleteCacheEntry(currCache[i]);
            currCache[i] = nullptr;
        }
    }
    परत सत्य;
}


/**
 * Loads the currency name data from the cache, or from resource bundles if necessary.
 * The refCount is automatically incremented.  It is the caller's responsibility
 * to decrement it when done!
 */
स्थिर CurrencyNameCacheEntry*
getCacheEntry(स्थिरांक अक्षर* locale, UErrorCode& ec) {

    int32_t total_currency_name_मोजणी = 0;
    CurrencyNameStruct* currencyNames = nullptr;
    int32_t total_currency_चिन्ह_मोजणी = 0;
    CurrencyNameStruct* currencySymbols = nullptr;
    CurrencyNameCacheEntry* cacheEntry = nullptr;

    umtx_lock(&gCurrencyCacheMutex);
    // in order to handle racing correctly,
    // not putting 'search' in a separate function.
    int8_t found = -1;
    साठी (int8_t i = 0; i < CURRENCY_NAME_CACHE_अंक; ++i) {
        जर (currCache[i]!= nullptr &&
            uprv_strcmp(locale, currCache[i]->locale) == 0) {
            found = i;
            थांबा;
        }
    }
    जर (found != -1) {
        cacheEntry = currCache[found];
        ++(cacheEntry->ref_मोजणी);
    }
    umtx_unlock(&gCurrencyCacheMutex);
    जर (found == -1) {
        collectCurrencyNames(locale, &currencyNames, &total_currency_name_मोजणी, &currencySymbols, &total_currency_चिन्ह_मोजणी, ec);
        जर (U_FAILURE(ec)) {
            परत nullptr;
        }
        umtx_lock(&gCurrencyCacheMutex);
        // check again.
        साठी (int8_t i = 0; i < CURRENCY_NAME_CACHE_अंक; ++i) {
            जर (currCache[i]!= nullptr &&
                uprv_strcmp(locale, currCache[i]->locale) == 0) {
                found = i;
                थांबा;
            }
        }
        जर (found == -1) {
            // insert new entry to 
            // currentCacheEntryIndex % CURRENCY_NAME_CACHE_NUM
            // and remove the existing entry 
            // currentCacheEntryIndex % CURRENCY_NAME_CACHE_NUM
            // from cache.
            cacheEntry = currCache[currentCacheEntryIndex];
            जर (cacheEntry) {
                --(cacheEntry->ref_मोजणी);
                // delete if the ref count is zero
                जर (cacheEntry->ref_मोजणी == 0) {
                    deleteCacheEntry(cacheEntry);
                }
            }
            cacheEntry = static_cast<CurrencyNameCacheEntry*>(uprv_malloc(आकार(CurrencyNameCacheEntry)));
            जर (cacheEntry == nullptr) {
                deleteCurrencyNames(currencyNames, total_currency_name_मोजणी);
                deleteCurrencyNames(currencySymbols, total_currency_चिन्ह_मोजणी);
                ec = U_MEMORY_ALLOCATION_ERROR;
                परत nullptr;
            }
            currCache[currentCacheEntryIndex] = cacheEntry;
            uprv_strcpy(cacheEntry->locale, locale);
            cacheEntry->currencyNames = currencyNames;
            cacheEntry->total_Currency_Name_मोजणी = total_currency_name_मोजणी;
            cacheEntry->currencySymbols = currencySymbols;
            cacheEntry->total_Currency_चिन्ह_मोजणी = total_currency_चिन्ह_मोजणी;
            cacheEntry->ref_मोजणी = 2; // one for cache, one for reference
            currentCacheEntryIndex = (currentCacheEntryIndex + 1) % CURRENCY_NAME_CACHE_अंक;
            ucln_common_registerCleanup(UCLN_COMMON_CURRENCY, currency_cleanup);
        } अन्यथा {
            deleteCurrencyNames(currencyNames, total_currency_name_मोजणी);
            deleteCurrencyNames(currencySymbols, total_currency_चिन्ह_मोजणी);
            cacheEntry = currCache[found];
            ++(cacheEntry->ref_मोजणी);
        }
        umtx_unlock(&gCurrencyCacheMutex);
    }

    परत cacheEntry;
}

स्थिर शून्य releaseCacheEntry(CurrencyNameCacheEntry* cacheEntry) {
    umtx_lock(&gCurrencyCacheMutex);
    --(cacheEntry->ref_मोजणी);
    जर (cacheEntry->ref_मोजणी == 0) {  // remove
        deleteCacheEntry(cacheEntry);
    }
    umtx_unlock(&gCurrencyCacheMutex);
}

U_CAPI शून्य
uprv_parseCurrency(स्थिरांक अक्षर* locale,
                   स्थिरांक icu::UnicodeString& text,
                   icu::ParsePosition& pos,
                   int8_t type,
                   int32_t* partialMatchLen,
                   char16_t* result,
                   UErrorCode& ec) {
    U_NAMESPACE_USE
    जर (U_FAILURE(ec)) {
        परत;
    }
    CurrencyNameCacheEntry* cacheEntry = getCacheEntry(locale, ec);
    जर (U_FAILURE(ec)) {
        परत;
    }

    int32_t total_currency_name_मोजणी = cacheEntry->total_Currency_Name_मोजणी;
    CurrencyNameStruct* currencyNames = cacheEntry->currencyNames;
    int32_t total_currency_चिन्ह_मोजणी = cacheEntry->total_Currency_चिन्ह_मोजणी;
    CurrencyNameStruct* currencySymbols = cacheEntry->currencySymbols;

    int32_t start = pos.getIndex();

    char16_t आदान_Text[MAX_CURRENCY_NAME_LEN];
    char16_t upperText[MAX_CURRENCY_NAME_LEN];
    int32_t textLen = MIN(MAX_CURRENCY_NAME_LEN, text.length() - start);
    text.extract(start, textLen, आदान_Text);
    UErrorCode ec1 = U_ZERO_ERROR;
    textLen = u_strToUpper(upperText, MAX_CURRENCY_NAME_LEN, आदान_Text, textLen, locale, &ec1);

    // Make sure partialMatchLen is initialized
    *partialMatchLen = 0;

    int32_t max = 0;
    int32_t matchIndex = -1;
    // case in-sensitive comparison against currency names
    शोध_Currency_Name(currencyNames, total_currency_name_मोजणी, 
                       upperText, textLen, partialMatchLen, &max, &matchIndex);

#ifdef UCURR_DEBUG
    म_लिखो("search in names, max = %d, matchIndex = %d\n", max, matchIndex);
#endif

    int32_t max_In_चिन्ह = 0;
    int32_t match_Index_In_चिन्ह = -1;
    जर (type != UCURR_LONG_NAME) {  // not name only
        // case sensitive comparison against currency symbols and ISO code.
        शोध_Currency_Name(currencySymbols, total_currency_चिन्ह_मोजणी, 
                           आदान_Text, textLen,
                           partialMatchLen,
                           &max_In_चिन्ह, &match_Index_In_चिन्ह);
    }

#ifdef UCURR_DEBUG
    म_लिखो("search in symbols, maxInSymbol = %d, matchIndexInSymbol = %d\n", max_In_चिन्ह, match_Index_In_चिन्ह);
    जर(match_Index_In_चिन्ह != -1) {
      म_लिखो("== ISO=%s\n", currencySymbols[match_Index_In_चिन्ह].IsoCode);
    }
#endif

    जर (max >= max_In_चिन्ह && matchIndex != -1) {
        u_charsToUChars(currencyNames[matchIndex].IsoCode, result, 4);
        pos.setIndex(start + max);
    } अन्यथा जर (max_In_चिन्ह >= max && match_Index_In_चिन्ह != -1) {
        u_charsToUChars(currencySymbols[match_Index_In_चिन्ह].IsoCode, result, 4);
        pos.setIndex(start + max_In_चिन्ह);
    }

    // decrease reference count
    releaseCacheEntry(cacheEntry);
}

शून्य uprv_currencyLeads(स्थिरांक अक्षर* locale, icu::UnicodeSet& result, UErrorCode& ec) {
    U_NAMESPACE_USE
    जर (U_FAILURE(ec)) {
        परत;
    }
    CurrencyNameCacheEntry* cacheEntry = getCacheEntry(locale, ec);
    जर (U_FAILURE(ec)) {
        परत;
    }

    साठी (int32_t i=0; i<cacheEntry->total_Currency_चिन्ह_मोजणी; i++) {
        स्थिरांक CurrencyNameStruct& info = cacheEntry->currencySymbols[i];
        UChar32 cp;
        U16_GET(info.currencyName, 0, 0, info.currencyNameLen, cp);
        result.add(cp);
    }

    साठी (int32_t i=0; i<cacheEntry->total_Currency_Name_मोजणी; i++) {
        स्थिरांक CurrencyNameStruct& info = cacheEntry->currencyNames[i];
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
U_CAPI शून्य
uprv_getStaticCurrencyName(स्थिरांक char16_t* iso, स्थिरांक अक्षर* loc,
                           icu::UnicodeString& result, UErrorCode& ec)
{
    U_NAMESPACE_USE

    int32_t len;
    स्थिरांक char16_t* currname = ucurr_getName(iso, loc, UCURR_चिन्ह_NAME,
                                          nullptr /* isChoiceFormat */, &len, &ec);
    जर (U_SUCCESS(ec)) {
        result.setTo(currname, len);
    }
}

U_CAPI int32_t U_EXPORT2
ucurr_getDefaultFractionDigits(स्थिरांक char16_t* currency, UErrorCode* ec) {
    परत ucurr_getDefaultFractionDigitsForUsage(currency,UCURR_USAGE_STANDARD,ec);
}

U_CAPI int32_t U_EXPORT2
ucurr_getDefaultFractionDigitsForUsage(स्थिरांक char16_t* currency, स्थिरांक UCurrencyUsage usage, UErrorCode* ec) {
    int32_t fracDigits = 0;
    जर (U_SUCCESS(*ec)) {
        निवड (usage) {
            स्थिती UCURR_USAGE_STANDARD:
                fracDigits = (_findMetaData(currency, *ec))[0];
                थांबा;
            स्थिती UCURR_USAGE_CASH:
                fracDigits = (_findMetaData(currency, *ec))[2];
                थांबा;
            पूर्वनिर्धारित:
                *ec = U_UNSUPPORTED_ERROR;
        }
    }
    परत fracDigits;
}

U_CAPI द्विगुण U_EXPORT2
ucurr_getRoundingIncrement(स्थिरांक char16_t* currency, UErrorCode* ec) {
    परत ucurr_getRoundingIncrementForUsage(currency, UCURR_USAGE_STANDARD, ec);
}

U_CAPI द्विगुण U_EXPORT2
ucurr_getRoundingIncrementForUsage(स्थिरांक char16_t* currency, स्थिरांक UCurrencyUsage usage, UErrorCode* ec) {
    द्विगुण result = 0.0;

    स्थिरांक int32_t *data = _findMetaData(currency, *ec);
    जर (U_SUCCESS(*ec)) {
        int32_t fracDigits;
        int32_t increment;
        निवड (usage) {
            स्थिती UCURR_USAGE_STANDARD:
                fracDigits = data[0];
                increment = data[1];
                थांबा;
            स्थिती UCURR_USAGE_CASH:
                fracDigits = data[2];
                increment = data[3];
                थांबा;
            पूर्वनिर्धारित:
                *ec = U_UNSUPPORTED_ERROR;
                परत result;
        }

        // If the meta data is invalid, return 0.0
        जर (fracDigits < 0 || fracDigits > MAX_POW10) {
            *ec = U_INVALID_FORMAT_ERROR;
        } अन्यथा {
            // A rounding value of 0 or 1 indicates no rounding.
            जर (increment >= 2) {
                // Return (increment) / 10^(fracDigits).  The only actual rounding data,
                // as of this writing, is CHF { 2, 5 }.
                result = द्विगुण(increment) / POW10[fracDigits];
            }
        }
    }

    परत result;
}

U_CDECL_BEGIN

प्रकारव्याख्या संरचना UCurrencyContext {
    uint32_t currType; /* UCurrCurrencyType */
    uint32_t listIdx;
} UCurrencyContext;

/*
Please keep this list in alphabetical order.
You can look at the CLDR supplemental data or ISO-4217 for the meaning of some
of these items.
ISO-4217: http://www.iso.org/iso/en/prods-services/popstds/currencycodeslist.html
*/
स्थिर स्थिरांक संरचना CurrencyList {
    स्थिरांक अक्षर *currency;
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

स्थिर int32_t U_CALLCONV
ucurr_मोजणी_Currency_List(UEnumeration *enumerator, UErrorCode * /*pErrorCode*/) {
    UCurrencyContext *myContext = (UCurrencyContext *)(enumerator->context);
    uint32_t currType = myContext->currType;
    int32_t count = 0;

    /* Count the number of items matching the type we are looking for. */
    साठी (int32_t idx = 0; gCurrencyList[idx].currency != nullptr; idx++) {
        जर (UCURR_MATCHES_BITMASK(gCurrencyList[idx].currType, currType)) {
            count++;
        }
    }
    परत count;
}

स्थिर स्थिरांक अक्षर* U_CALLCONV
ucurr_nextCurrencyList(UEnumeration *enumerator,
                        int32_t* resultLength,
                        UErrorCode * /*pErrorCode*/)
{
    UCurrencyContext *myContext = (UCurrencyContext *)(enumerator->context);

    /* Find the next in the list that matches the type we are looking for. */
    पर्यंत (myContext->listIdx < UPRV_LENGTHOF(gCurrencyList)-1) {
        स्थिरांक संरचना CurrencyList *currItem = &gCurrencyList[myContext->listIdx++];
        जर (UCURR_MATCHES_BITMASK(currItem->currType, myContext->currType))
        {
            जर (resultLength) {
                *resultLength = 3; /* Currency codes are only 3 chars long */
            }
            परत currItem->currency;
        }
    }
    /* We enumerated too far. */
    जर (resultLength) {
        *resultLength = 0;
    }
    परत nullptr;
}

स्थिर शून्य U_CALLCONV
ucurr_resetCurrencyList(UEnumeration *enumerator, UErrorCode * /*pErrorCode*/) {
    ((UCurrencyContext *)(enumerator->context))->listIdx = 0;
}

स्थिर शून्य U_CALLCONV
ucurr_closeCurrencyList(UEnumeration *enumerator) {
    uprv_free(enumerator->context);
    uprv_free(enumerator);
}

स्थिर शून्य U_CALLCONV
ucurr_createCurrencyList(UHashtable *isoCodes, UErrorCode* status){
    UErrorCode localStatus = U_ZERO_ERROR;

    // Look up the CurrencyMap element in the root bundle.
    UResourceBundle *rb = ures_openDirect(U_ICUDATA_CURR, CURRENCY_DATA, &localStatus);
    LocalUResourceBundlePointer currencyMapArray(ures_get_By_किल्ली(rb, CURRENCY_MAP, rb, &localStatus));

    जर (U_SUCCESS(localStatus)) {
        // process each entry in currency map
        साठी (int32_t i=0; i<ures_getSize(currencyMapArray.getAlias()); i++) {
            // get the currency resource
            LocalUResourceBundlePointer currencyArray(ures_getByIndex(currencyMapArray.getAlias(), i, nullptr, &localStatus));
            // process each currency
            जर (U_SUCCESS(localStatus)) {
                साठी (int32_t j=0; j<ures_getSize(currencyArray.getAlias()); j++) {
                    // get the currency resource
                    LocalUResourceBundlePointer currencyRes(ures_getByIndex(currencyArray.getAlias(), j, nullptr, &localStatus));
                    IsoCodeEntry *entry = (IsoCodeEntry*)uprv_malloc(आकार(IsoCodeEntry));
                    जर (entry == nullptr) {
                        *status = U_MEMORY_ALLOCATION_ERROR;
                        परत;
                    }

                    // get the ISO code
                    int32_t isoLength = 0;
                    LocalUResourceBundlePointer idRes(ures_get_By_किल्ली(currencyRes.getAlias(), "id", nullptr, &localStatus));
                    जर (idRes.isNull()) {
                        सुरूठेवा;
                    }
                    स्थिरांक char16_t *isoCode = ures_getString(idRes.getAlias(), &isoLength, &localStatus);

                    // get from date
                    UDate fromDate = U_DATE_MIN;
                    LocalUResourceBundlePointer fromRes(ures_get_By_किल्ली(currencyRes.getAlias(), "from", nullptr, &localStatus));

                    जर (U_SUCCESS(localStatus)) {
                        int32_t fromLength = 0;
                        स्थिरांक int32_t *fromArray = ures_getIntVector(fromRes.getAlias(), &fromLength, &localStatus);
                        int64_t currDate64 = ((uint64_t)fromArray[0]) << 32;
                        currDate64 |= ((int64_t)fromArray[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                        fromDate = (UDate)currDate64;
                    }

                    // get to date
                    UDate toDate = U_DATE_MAX;
                    localStatus = U_ZERO_ERROR;
                    LocalUResourceBundlePointer toRes(ures_get_By_किल्ली(currencyRes.getAlias(), "to", nullptr, &localStatus));

                    जर (U_SUCCESS(localStatus)) {
                        int32_t toLength = 0;
                        स्थिरांक int32_t *toArray = ures_getIntVector(toRes.getAlias(), &toLength, &localStatus);
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
            } अन्यथा {
                *status = localStatus;
            }
        }
    } अन्यथा {
        *status = localStatus;
    }
}

स्थिर स्थिरांक UEnumeration gEnumCurrencyList = {
    nullptr,
    nullptr,
    ucurr_closeCurrencyList,
    ucurr_मोजणी_Currency_List,
    uenum_unextDefault,
    ucurr_nextCurrencyList,
    ucurr_resetCurrencyList
};
U_CDECL_END


स्थिर शून्य U_CALLCONV initIsoCodes(UErrorCode &status) {
    U_ASSERT(gIsoCodes == nullptr);
    ucln_common_registerCleanup(UCLN_COMMON_CURRENCY, currency_cleanup);

    LocalUHashtablePointer isoCodes(uhash_open(uhash_hashUChars, uhash_compareUChars, nullptr, &status));
    जर (U_FAILURE(status)) {
        परत;
    }
    uhash_set_मूल्य_Deleter(isoCodes.getAlias(), deleteIsoCodeEntry);

    ucurr_createCurrencyList(isoCodes.getAlias(), &status);
    जर (U_FAILURE(status)) {
        परत;
    }
    gIsoCodes = isoCodes.orphan();  // Note: gIsoCodes is const. Once set up here it is never altered,
                                    //       and read only access is safe without synchronization.
}

स्थिर शून्य populateCurrSymbolsEquiv(icu::Hashtable *hash, UErrorCode &status) {
    जर (U_FAILURE(status)) { परत; }
    साठी (स्थिरांक स्वतः& entry : unisets::kCurrencyEntries) {
        UnicodeString exemplar(entry.exemplar);
        स्थिरांक UnicodeSet* set = unisets::get(entry.किल्ली);
        जर (set == nullptr) { परत; }
        UnicodeSetIterator it(*set);
        पर्यंत (it.next()) {
            UnicodeString value = it.getString();
            जर (value == exemplar) {
                // No need to mark the exemplar character as an equivalent
                सुरूठेवा;
            }
            makeEquivalent(exemplar, value, hash, status);
            जर (U_FAILURE(status)) { परत; }
        }
    }
}

स्थिर शून्य U_CALLCONV initCurrSymbolsEquiv() {
    U_ASSERT(gCurrSymbolsEquiv == nullptr);
    UErrorCode status = U_ZERO_ERROR;
    ucln_common_registerCleanup(UCLN_COMMON_CURRENCY, currency_cleanup);
    icu::Hashtable *temp = नवीन icu::Hashtable(status);
    जर (temp == nullptr) {
        परत;
    }
    जर (U_FAILURE(status)) {
        delete temp;
        परत;
    }
    temp->set_मूल्य_Deleter(deleteUnicode);
    populateCurrSymbolsEquiv(temp, status);
    जर (U_FAILURE(status)) {
        delete temp;
        परत;
    }
    gCurrSymbolsEquiv = temp;
}

U_CAPI UBool U_EXPORT2
ucurr_isAvailable(स्थिरांक char16_t* isoCode, UDate from, UDate to, UErrorCode* eErrorCode) {
    umtx_initOnce(gIsoCodesInitOnce, &initIsoCodes, *eErrorCode);
    जर (U_FAILURE(*eErrorCode)) {
        परत असत्य;
    }

    IsoCodeEntry* result = (IsoCodeEntry *) uhash_get(gIsoCodes, isoCode);
    जर (result == nullptr) {
        परत असत्य;
    } अन्यथा जर (from > to) {
        *eErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        परत असत्य;
    } अन्यथा जर  ((from > result->to) || (to < result->from)) {
        परत असत्य;
    }
    परत सत्य;
}

स्थिर स्थिरांक icu::Hashtable* getCurrSymbolsEquiv() {
    umtx_initOnce(gCurrSymbolsEquivInitOnce, &initCurrSymbolsEquiv);
    परत gCurrSymbolsEquiv;
}

U_CAPI UEnumeration * U_EXPORT2
ucurr_openISOCurrencies(uint32_t currType, UErrorCode *pErrorCode) {
    UEnumeration *myEnum = nullptr;
    UCurrencyContext *myContext;

    myEnum = (UEnumeration*)uprv_malloc(आकार(UEnumeration));
    जर (myEnum == nullptr) {
        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
        परत nullptr;
    }
    uprv_memcpy(myEnum, &gEnumCurrencyList, आकार(UEnumeration));
    myContext = (UCurrencyContext*)uprv_malloc(आकार(UCurrencyContext));
    जर (myContext == nullptr) {
        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
        uprv_free(myEnum);
        परत nullptr;
    }
    myContext->currType = currType;
    myContext->listIdx = 0;
    myEnum->context = myContext;
    परत myEnum;
}

U_CAPI int32_t U_EXPORT2
ucurr_मोजणी_Currencies(स्थिरांक अक्षर* locale, 
                 UDate date, 
                 UErrorCode* ec)
{
    int32_t curr_मोजणी = 0;

    जर (ec != nullptr && U_SUCCESS(*ec)) 
    {
        // local variables
        UErrorCode localStatus = U_ZERO_ERROR;

        // get country or country_variant in `id'
        CharString id = idForLocale(locale, ec);

        जर (U_FAILURE(*ec))
        {
            परत 0;
        }

        // Remove variants, which is only needed for registration.
        अक्षर *idDelim = म_अक्षर(id.data(), VAR_DELIM);
        जर (idDelim)
        {
            id.truncate(idDelim - id.data());
        }

        // Look up the CurrencyMap element in the root bundle.
        UResourceBundle *rb = ures_openDirect(U_ICUDATA_CURR, CURRENCY_DATA, &localStatus);
        UResourceBundle *cm = ures_get_By_किल्ली(rb, CURRENCY_MAP, rb, &localStatus);

        // Using the id derived from the local, get the currency data
        LocalUResourceBundlePointer countryArray(ures_get_By_किल्ली(rb, id.data(), cm, &localStatus));

        // process each currency to see which one is valid for the given date
        जर (U_SUCCESS(localStatus))
        {
            साठी (int32_t i=0; i<ures_getSize(countryArray.getAlias()); i++)
            {
                // get the currency resource
                LocalUResourceBundlePointer currencyRes(ures_getByIndex(countryArray.getAlias(), i, nullptr, &localStatus));

                // get the from date
                int32_t fromLength = 0;
                LocalUResourceBundlePointer fromRes(ures_get_By_किल्ली(currencyRes.getAlias(), "from", nullptr, &localStatus));
                स्थिरांक int32_t *fromArray = ures_getIntVector(fromRes.getAlias(), &fromLength, &localStatus);

                int64_t currDate64 = (int64_t)((uint64_t)(fromArray[0]) << 32);
                currDate64 |= ((int64_t)fromArray[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                UDate fromDate = (UDate)currDate64;

                जर (ures_getSize(currencyRes.getAlias())> 2)
                {
                    int32_t toLength = 0;
                    LocalUResourceBundlePointer toRes(ures_get_By_किल्ली(currencyRes.getAlias(), "to", nullptr, &localStatus));
                    स्थिरांक int32_t *toArray = ures_getIntVector(toRes.getAlias(), &toLength, &localStatus);

                    currDate64 = (int64_t)toArray[0] << 32;
                    currDate64 |= ((int64_t)toArray[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                    UDate toDate = (UDate)currDate64;

                    जर ((fromDate <= date) && (date < toDate))
                    {
                        curr_मोजणी++;
                    }
                }
                अन्यथा
                {
                    जर (fromDate <= date)
                    {
                        curr_मोजणी++;
                    }
                }
            } // end For loop
        } // end if (U_SUCCESS(localStatus))

        // Check for errors
        जर (*ec == U_ZERO_ERROR || localStatus != U_ZERO_ERROR)
        {
            // There is nothing to fallback to. 
            // Report the failure/warning if possible.
            *ec = localStatus;
        }

        जर (U_SUCCESS(*ec))
        {
            // no errors
            परत curr_मोजणी;
        }
    }

    // If we got here, either error code is invalid or
    // some argument passed is no good.
    परत 0;
}

U_CAPI int32_t U_EXPORT2 
ucurr_forLocaleAndDate(स्थिरांक अक्षर* locale, 
                UDate date, 
                int32_t index,
                char16_t* buff,
                int32_t buffCapacity, 
                UErrorCode* ec)
{
    int32_t resLen = 0;
	int32_t currIndex = 0;
    स्थिरांक char16_t* s = nullptr;

    जर (ec != nullptr && U_SUCCESS(*ec))
    {
        // check the arguments passed
        जर ((buff && buffCapacity) || !buffCapacity )
        {
            // local variables
            UErrorCode localStatus = U_ZERO_ERROR;

            // get country or country_variant in `id'
            CharString id = idForLocale(locale, ec);
            जर (U_FAILURE(*ec))
            {
                परत 0;
            }

            // Remove variants, which is only needed for registration.
            अक्षर *idDelim = म_अक्षर(id.data(), VAR_DELIM);
            जर (idDelim)
            {
                id.truncate(idDelim - id.data());
            }

            // Look up the CurrencyMap element in the root bundle.
            UResourceBundle *rb = ures_openDirect(U_ICUDATA_CURR, CURRENCY_DATA, &localStatus);
            UResourceBundle *cm = ures_get_By_किल्ली(rb, CURRENCY_MAP, rb, &localStatus);

            // Using the id derived from the local, get the currency data
            LocalUResourceBundlePointer countryArray(ures_get_By_किल्ली(rb, id.data(), cm, &localStatus));

            // process each currency to see which one is valid for the given date
            तार्किक matchFound = असत्य;
            जर (U_SUCCESS(localStatus))
            {
                जर ((index <= 0) || (index> ures_getSize(countryArray.getAlias())))
                {
                    // requested index is out of bounds
                    परत 0;
                }

                साठी (int32_t i=0; i<ures_getSize(countryArray.getAlias()); i++)
                {
                    // get the currency resource
                    LocalUResourceBundlePointer currencyRes(ures_getByIndex(countryArray.getAlias(), i, nullptr, &localStatus));
                    s = ures_get_String_By_किल्ली(currencyRes.getAlias(), "id", &resLen, &localStatus);

                    // get the from date
                    int32_t fromLength = 0;
                    LocalUResourceBundlePointer fromRes(ures_get_By_किल्ली(currencyRes.getAlias(), "from", nullptr, &localStatus));
                    स्थिरांक int32_t *fromArray = ures_getIntVector(fromRes.getAlias(), &fromLength, &localStatus);

                    int64_t currDate64 = (int64_t)((uint64_t)fromArray[0] << 32);
                    currDate64 |= ((int64_t)fromArray[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                    UDate fromDate = (UDate)currDate64;

                    जर (ures_getSize(currencyRes.getAlias()) > 2)
                    {
                        int32_t toLength = 0;
                        LocalUResourceBundlePointer toRes(ures_get_By_किल्ली(currencyRes.getAlias(), "to", nullptr, &localStatus));
                        स्थिरांक int32_t *toArray = ures_getIntVector(toRes.getAlias(), &toLength, &localStatus);

                        currDate64 = (int64_t)toArray[0] << 32;
                        currDate64 |= ((int64_t)toArray[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                        UDate toDate = (UDate)currDate64;

                        जर ((fromDate <= date) && (date < toDate))
                        {
                            currIndex++;
                            जर (currIndex == index)
                            {
                                matchFound = सत्य;
                            }
                        }
                    }
                    अन्यथा
                    {
                        जर (fromDate <= date)
                        {
                            currIndex++;
                            जर (currIndex == index)
                            {
                                matchFound = सत्य;
                            }
                        }
                    }
                    // check for loop exit
                    जर (matchFound)
                    {
                        थांबा;
                    }

                } // end For loop
            }

            // Check for errors
            जर (*ec == U_ZERO_ERROR || localStatus != U_ZERO_ERROR)
            {
                // There is nothing to fallback to. 
                // Report the failure/warning if possible.
                *ec = localStatus;
            }

            जर (U_SUCCESS(*ec))
            {
                // no errors
                जर((buffCapacity> resLen) && matchFound)
                {
                    // write out the currency value
                    u_strcpy(buff, s);
                }
                अन्यथा
                {
                    परत 0;
                }
            }

            // return null terminated currency string
            परत u_terminateUChars(buff, buffCapacity, resLen, ec);
        }
        अन्यथा
        {
            // illegal argument encountered
            *ec = U_ILLEGAL_ARGUMENT_ERROR;
        }

    }

    // If we got here, either error code is invalid or
    // some argument passed is no good.
    परत resLen;
}

स्थिर स्थिरांक UEnumeration default_Keyword_मूल्ये = {
    nullptr,
    nullptr,
    ulist_close_keyword_मूल्ये_iterator,
    ulist_मोजणी_keyword_मूल्ये,
    uenum_unextDefault,
    ulist_next_keyword_मूल्य, 
    ulist_reset_keyword_मूल्ये_iterator
};

U_CAPI UEnumeration *U_EXPORT2 ucurr_get_Keyword_मूल्ये_For_Locale(स्थिरांक अक्षर *किल्ली, स्थिरांक अक्षर *locale, UBool commonlyUsed, UErrorCode* status) {
    // Resolve region
    CharString prefRegion = ulocimp_getRegionForSupplementalData(locale, सत्य, *status);

    // Read value from supplementalData
    UList *मूल्ये = ulist_createEmptyList(status);
    UList *other_मूल्ये = ulist_createEmptyList(status);
    UEnumeration *en = (UEnumeration *)uprv_malloc(आकार(UEnumeration));
    जर (U_FAILURE(*status) || en == nullptr) {
        जर (en == nullptr) {
            *status = U_MEMORY_ALLOCATION_ERROR;
        } अन्यथा {
            uprv_free(en);
        }
        ulist_deleteList(मूल्ये);
        ulist_deleteList(other_मूल्ये);
        परत nullptr;
    }
    स_नकल(en, &default_Keyword_मूल्ये, आकार(UEnumeration));
    en->context = मूल्ये;
    
    UResourceBundle* rb = ures_openDirect(U_ICUDATA_CURR, "supplementalData", status);
    LocalUResourceBundlePointer bundle(ures_get_By_किल्ली(rb, "CurrencyMap", rb, status));
    चळत_UResource_Bundle bundlekey, regbndl, curbndl, to;
    
    पर्यंत (U_SUCCESS(*status) && ures_hasNext(bundle.getAlias())) {
        ures_getNextResource(bundle.getAlias(), bundlekey.getAlias(), status);
        जर (U_FAILURE(*status)) {
            थांबा;
        }
        स्थिरांक अक्षर *region = ures_get_किल्ली(bundlekey.getAlias());
        UBool isPrefRegion = prefRegion == region;
        जर (!isPrefRegion && commonlyUsed) {
            // With commonlyUsed=true, we do not put
            // currencies for other regions in the
            // result list.
            सुरूठेवा;
        }
        ures_get_By_किल्ली(bundle.getAlias(), region, regbndl.getAlias(), status);
        जर (U_FAILURE(*status)) {
            थांबा;
        }
        पर्यंत (U_SUCCESS(*status) && ures_hasNext(regbndl.getAlias())) {
            ures_getNextResource(regbndl.getAlias(), curbndl.getAlias(), status);
            जर (ures_getType(curbndl.getAlias()) != URES_TABLE) {
                // Currently, an empty ARRAY is mixed in.
                सुरूठेवा;
            }
            अक्षर *curID = (अक्षर *)uprv_malloc(आकार(अक्षर) * ULOC_KEYWORDS_CAPACITY);
            जर (curID == nullptr) {
                *status = U_MEMORY_ALLOCATION_ERROR;
                थांबा;
            }
            int32_t curIDLength = ULOC_KEYWORDS_CAPACITY;

#जर U_CHARSET_FAMILY==U_ASCII_FAMILY
            ures_get_UTF8_String_By_किल्ली(curbndl.getAlias(), "id", curID, &curIDLength, सत्य, status);
            /* optimize - use the utf-8 string */
#अन्यथा
            {
                       स्थिरांक char16_t* defString = ures_get_String_By_किल्ली(curbndl.getAlias(), "id", &curIDLength, status);
                       जर(U_SUCCESS(*status)) {
			   जर(curIDLength+1 > ULOC_KEYWORDS_CAPACITY) {
				*status = U_BUFFER_OVERFLOW_ERROR;
			   } अन्यथा {
                           	u_UCharsToChars(defString, curID, curIDLength+1);
			   }
                       }
            }
#endif	

            जर (U_FAILURE(*status)) {
                थांबा;
            }
            UBool hasTo = असत्य;
            ures_get_By_किल्ली(curbndl.getAlias(), "to", to.getAlias(), status);
            जर (U_FAILURE(*status)) {
                // Do nothing here...
                *status = U_ZERO_ERROR;
            } अन्यथा {
                hasTo = सत्य;
            }
            जर (isPrefRegion && !hasTo && !ulist_containsString(मूल्ये, curID, (int32_t)uprv_strlen(curID))) {
                // Currently active currency for the target country
                ulist_addItemEndList(मूल्ये, curID, सत्य, status);
            } अन्यथा जर (!ulist_containsString(other_मूल्ये, curID, (int32_t)uprv_strlen(curID)) && !commonlyUsed) {
                ulist_addItemEndList(other_मूल्ये, curID, सत्य, status);
            } अन्यथा {
                uprv_free(curID);
            }
        }
        
    }
    जर (U_SUCCESS(*status)) {
        जर (commonlyUsed) {
            जर (ulist_getListSize(मूल्ये) == 0) {
                // This could happen if no valid region is supplied in the input
                // locale. In this case, we use the CLDR's default.
                uenum_close(en);
                en = ucurr_get_Keyword_मूल्ये_For_Locale(किल्ली, "und", सत्य, status);
            }
        } अन्यथा {
            // Consolidate the list
            अक्षर *value = nullptr;
            ulist_resetList(other_मूल्ये);
            पर्यंत ((value = (अक्षर *)ulist_getNext(other_मूल्ये)) != nullptr) {
                जर (!ulist_containsString(मूल्ये, value, (int32_t)uprv_strlen(value))) {
                    अक्षर *tmp_मूल्य = (अक्षर *)uprv_malloc(आकार(अक्षर) * ULOC_KEYWORDS_CAPACITY);
                    जर (tmp_मूल्य == nullptr) {
                        *status = U_MEMORY_ALLOCATION_ERROR;
                        थांबा;
                    }
                    uprv_memcpy(tmp_मूल्य, value, uprv_strlen(value) + 1);
                    ulist_addItemEndList(मूल्ये, tmp_मूल्य, सत्य, status);
                    जर (U_FAILURE(*status)) {
                        थांबा;
                    }
                }
            }
        }
        ulist_resetList((UList *)(en->context));
    } अन्यथा {
        ulist_deleteList(मूल्ये);
        uprv_free(en);
        मूल्ये = nullptr;
        en = nullptr;
    }
    ulist_deleteList(other_मूल्ये);
    परत en;
}


U_CAPI int32_t U_EXPORT2
ucurr_getNumericCode(स्थिरांक char16_t* currency) {
    int32_t code = 0;
    जर (currency && u_strlen(currency) == ISO_CURRENCY_CODE_LENGTH) {
        UErrorCode status = U_ZERO_ERROR;

        UResourceBundle* bundle = ures_openDirect(nullptr, "currencyNumericCodes", &status);
        LocalUResourceBundlePointer codeMap(ures_get_By_किल्ली(bundle, "codeMap", bundle, &status));
        जर (U_SUCCESS(status)) {
            अक्षर alphaCode[ISO_CURRENCY_CODE_LENGTH+1];
            myUCharsToChars(alphaCode, currency);
            T_CString_toUpperCase(alphaCode);
            ures_get_By_किल्ली(codeMap.getAlias(), alphaCode, codeMap.getAlias(), &status);
            पूर्णांक tmpCode = ures_getInt(codeMap.getAlias(), &status);
            जर (U_SUCCESS(status)) {
                code = tmpCode;
            }
        }
    }
    परत code;
}
#endif /* #if !UCONFIG_NO_FORMATTING */

//eof
