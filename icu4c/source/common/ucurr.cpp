// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
* Copyright (c) 2002-2016, International Business Machines
* Corporation and others.  All Rights Reserved.
**********************************************************************
*/

#ضمن "unicode/utypes.h"

#𐤀𐤌 !UCONFIG_NO_FORMATTING

#ضمن <utility>

#ضمن "unicode/ucurr.h"
#ضمن "unicode/locid.h"
#ضمن "unicode/ures.h"
#ضمن "unicode/ustring.h"
#ضمن "unicode/parsepos.h"
#ضمن "unicode/uniset.h"
#ضمن "unicode/usetiter.h"
#ضمن "unicode/utf16.h"
#ضمن "ustr_imp.h"
#ضمن "charstr.h"
#ضمن "cmemory.h"
#ضمن "cstring.h"
#ضمن "static_unicode_sets.h"
#ضمن "uassert.h"
#ضمن "umutex.h"
#ضمن "ucln_cmn.h"
#ضمن "uenumimp.h"
#ضمن "uhash.h"
#ضمن "hash.h"
#ضمن "uinvchar.h"
#ضمن "uresimp.h"
#ضمن "ulist.h"
#ضمن "uresimp.h"
#ضمن "ureslocs.h"
#ضمن "ulocimp.h"

using نطاق icu;

//#define UCURR_DEBUG_EQUIV 1
#ifdef UCURR_DEBUG_EQUIV
#ضمن "stdio.h"
#endif
//#define UCURR_DEBUG 1
#ifdef UCURR_DEBUG
#ضمن "stdio.h"
#endif

تعريفنوع بنية IsoCodeEntry {
    ثابت char16_t *isoCode; /* const because it's a reference to a resource bundle string. */
    UDate from;
    UDate to;
} IsoCodeEntry;

//------------------------------------------------------------
// Constants

// Default currency meta data of last resort.  We try to use the
// defaults encoded in the meta data resource bundle.  If there is a
// configuration/build error and these are not available, we use these
// hard-coded defaults (which should be identical).
ساكن ثابت int32_t LAST_RESORT_DATA[] = { 2, 0, 2, 0 };

// POW10[i] = 10^i, i=0..MAX_POW10
ساكن ثابت int32_t POW10[] = { 1, 10, 100, 1000, 10000, 100000,
                                 1000000, 10000000, 100000000, 1000000000 };

ساكن ثابت int32_t MAX_POW10 = UPRV_LENGTHOF(POW10) - 1;

#define ISO_CURRENCY_CODE_LENGTH 3

//------------------------------------------------------------
// Resource tags
//

ساكن ثابت حرف CURRENCY_DATA[] = "supplementalData";
// Tag for meta-data, in root.
ساكن ثابت حرف CURRENCY_META[] = "CurrencyMeta";

// Tag for map from countries to currencies, in root.
ساكن ثابت حرف CURRENCY_MAP[] = "CurrencyMap";

// Tag for default meta-data, in CURRENCY_META
ساكن ثابت حرف DEFAULT_META[] = "DEFAULT";

// Variant delimiter
ساكن ثابت حرف VAR_DELIM = '_';

// Tag for localized display names (symbols) of currencies
ساكن ثابت حرف CURRENCIES[] = "Currencies";
ساكن ثابت حرف CURRENCIES_NARROW[] = "Currencies%narrow";
ساكن ثابت حرف CURRENCIES_FORMAL[] = "Currencies%formal";
ساكن ثابت حرف CURRENCIES_VARIANT[] = "Currencies%variant";
ساكن ثابت حرف CURRENCYPLURALS[] = "CurrencyPlurals";

// ISO codes mapping table
ساكن ثابت UHashtable* gIsoCodes = nullptr;
ساكن icu::UInitOnce gIsoCodesInitOnce {};

// Currency symbol equivalances
ساكن ثابت icu::Hashtable* gCurrSymbolsEquiv = nullptr;
ساكن icu::UInitOnce gCurrSymbolsEquivInitOnce {};

U_NAMESPACE_BEGIN

// EquivIterator iterates over all strings that are equivalent to a given
// string, s. Note that EquivIterator will never yield s itself.
صنف EquivIterator : عام icu::UMemory {
عام:
    // Constructor. hash stores the equivalence relationships; s is the string
    // for which we find equivalent strings.
    مضمن EquivIterator(ثابت icu::Hashtable& hash, ثابت icu::UnicodeString& s)
        : _hash(hash) { 
        _start = _current = &s;
    }
    مضمن ~EquivIterator() { }

    // next returns the next equivalent string or nullptr if there are no more.
    // If s has no equivalent strings, next returns nullptr on the first call.
    ثابت icu::UnicodeString *next();
خاص:
    ثابت icu::Hashtable& _hash;
    ثابت icu::UnicodeString* _start;
    ثابت icu::UnicodeString* _current;
};

ثابت icu::UnicodeString *
EquivIterator::next() {
    ثابت icu::UnicodeString* _next = static_cast<ثابت icu::UnicodeString*>(_hash.get(*_current));
    𐤀𐤌 (_next == nullptr) {
        U_ASSERT(_current == _start);
        أرجع nullptr;
    }
    𐤀𐤌 (*_next == *_start) {
        أرجع nullptr;
    }
    _current = _next;
    أرجع _next;
}

U_NAMESPACE_END

// makeEquivalent makes lhs and rhs equivalent by updating the equivalence
// relations in hash accordingly.
ساكن فراغ makeEquivalent(
    ثابت icu::UnicodeString &lhs,
    ثابت icu::UnicodeString &rhs,
    icu::Hashtable* hash, UErrorCode &status) {
    𐤀𐤌 (U_FAILURE(status)) {
        أرجع;
    }
    𐤀𐤌 (lhs == rhs) {
        // already equivalent
        أرجع;
    }
    icu::EquivIterator leftIter(*hash, lhs);
    icu::EquivIterator rightIter(*hash, rhs);
    ثابت icu::UnicodeString *firstLeft = leftIter.next();
    ثابت icu::UnicodeString *firstRight = rightIter.next();
    ثابت icu::UnicodeString *nextLeft = firstLeft;
    ثابت icu::UnicodeString *nextRight = firstRight;
    طالما (nextLeft != nullptr && nextRight != nullptr) {
        𐤀𐤌 (*nextLeft == rhs || *nextRight == lhs) {
            // Already equivalent
            أرجع;
        }
        nextLeft = leftIter.next();
        nextRight = rightIter.next();
    }
    // Not equivalent. Must join.
    icu::UnicodeString *newFirstLeft;
    icu::UnicodeString *newFirstRight;
    𐤀𐤌 (firstRight == nullptr && firstLeft == nullptr) {
        // Neither lhs or rhs belong to an equivalence circle, so we form
        // a new equivalnce circle of just lhs and rhs.
        newFirstLeft = جديد icu::UnicodeString(rhs);
        newFirstRight = جديد icu::UnicodeString(lhs);
    } وإلا 𐤀𐤌 (firstRight == nullptr) {
        // lhs belongs to an equivalence circle, but rhs does not, so we link
        // rhs into lhs' circle.
        newFirstLeft = جديد icu::UnicodeString(rhs);
        newFirstRight = جديد icu::UnicodeString(*firstLeft);
    } وإلا 𐤀𐤌 (firstLeft == nullptr) {
        // rhs belongs to an equivlance circle, but lhs does not, so we link
        // lhs into rhs' circle.
        newFirstLeft = جديد icu::UnicodeString(*firstRight);
        newFirstRight = جديد icu::UnicodeString(lhs);
    } وإلا {
        // Both lhs and rhs belong to different equivalnce circles. We link
        // them together to form one single, larger equivalnce circle.
        newFirstLeft = جديد icu::UnicodeString(*firstRight);
        newFirstRight = جديد icu::UnicodeString(*firstLeft);
    }
    𐤀𐤌 (newFirstLeft == nullptr || newFirstRight == nullptr) {
        delete newFirstLeft;
        delete newFirstRight;
        status = U_MEMORY_ALLOCATION_ERROR;
        أرجع;
    }
    hash->put(lhs, (فراغ *) newFirstLeft, status);
    hash->put(rhs, (فراغ *) newFirstRight, status);
}

// countEquivalent counts how many strings are equivalent to s.
// hash stores all the equivalnce relations.
// countEquivalent does not include s itself in the count.
ساكن int32_t countEquivalent(ثابت icu::Hashtable &hash, ثابت icu::UnicodeString &s) {
    int32_t result = 0;
    icu::EquivIterator iter(hash, s);
    طالما (iter.next() != nullptr) {
        ++result;
    }
#ifdef UCURR_DEBUG_EQUIV
 {
   حرف tmp[200];
   s.extract(0,s.length(),tmp, "UTF-8");
   printf("CountEquivalent('%s') = %d\n", tmp, result);
 }
#endif
    أرجع result;
}

ساكن ثابت icu::Hashtable* getCurrSymbolsEquiv();

//------------------------------------------------------------
// Code

/**
 * Cleanup callback func
 */
ساكن UBool U_CALLCONV 
isoCodes_cleanup()
{
    𐤀𐤌 (gIsoCodes != nullptr) {
        uhash_close(const_cast<UHashtable *>(gIsoCodes));
        gIsoCodes = nullptr;
    }
    gIsoCodesInitOnce.reset();
    أرجع true;
}

/**
 * Cleanup callback func
 */
ساكن UBool U_CALLCONV 
currSymbolsEquiv_cleanup()
{
    delete const_cast<icu::Hashtable *>(gCurrSymbolsEquiv);
    gCurrSymbolsEquiv = nullptr;
    gCurrSymbolsEquivInitOnce.reset();
    أرجع true;
}

/**
 * Deleter for IsoCodeEntry
 */
ساكن فراغ U_CALLCONV
deleteIsoCodeEntry(فراغ *obj) {
    IsoCodeEntry* entry = static_cast<IsoCodeEntry*>(obj);
    uprv_free(entry);
}

/**
 * Deleter for gCurrSymbolsEquiv.
 */
ساكن فراغ U_CALLCONV
deleteUnicode(فراغ *obj) {
    icu::UnicodeString* entry = static_cast<icu::UnicodeString*>(obj);
    delete entry;
}

/**
 * Unfortunately, we have to convert the char16_t* currency code to char*
 * to use it as a resource key.
 */
ساكن مضمن حرف*
myUCharsToChars(حرف* resultOfLen4, ثابت char16_t* currency) {
    u_UCharsToChars(currency, resultOfLen4, ISO_CURRENCY_CODE_LENGTH);
    resultOfLen4[ISO_CURRENCY_CODE_LENGTH] = 0;
    أرجع resultOfLen4;
}

/**
 * Internal function to look up currency data.  Result is an array of
 * four integers.  The first is the fraction digits.  The second is the
 * rounding increment, or 0 if none.  The rounding increment is in
 * units of 10^(-fraction_digits).  The third and fourth are the same
 * except that they are those used in cash transactions ( cashDigits
 * and cashRounding ).
 */
ساكن ثابت int32_t*
_findMetaData(ثابت char16_t* currency, UErrorCode& ec) {

    𐤀𐤌 (currency == nullptr || *currency == 0) {
        𐤀𐤌 (U_SUCCESS(ec)) {
            ec = U_ILLEGAL_ARGUMENT_ERROR;
        }
        أرجع LAST_RESORT_DATA;
    }

    // Get CurrencyMeta resource out of root locale file.  [This may
    // move out of the root locale file later; if it does, update this
    // code.]
    UResourceBundle* currencyData = ures_openDirect(U_ICUDATA_CURR, CURRENCY_DATA, &ec);
    LocalUResourceBundlePointer currencyMeta(ures_getByKey(currencyData, CURRENCY_META, currencyData, &ec));

    𐤀𐤌 (U_FAILURE(ec)) {
        // Config/build error; return hard-coded defaults
        أرجع LAST_RESORT_DATA;
    }

    // Look up our currency, or if that's not available, then DEFAULT
    حرف buf[ISO_CURRENCY_CODE_LENGTH+1];
    UErrorCode ec2 = U_ZERO_ERROR; // local error code: soft failure
    LocalUResourceBundlePointer rb(ures_getByKey(currencyMeta.getAlias(), myUCharsToChars(buf, currency), nullptr, &ec2));
      𐤀𐤌 (U_FAILURE(ec2)) {
        rb.adoptInstead(ures_getByKey(currencyMeta.getAlias(),DEFAULT_META, nullptr, &ec));
        𐤀𐤌 (U_FAILURE(ec)) {
            // Config/build error; return hard-coded defaults
            أرجع LAST_RESORT_DATA;
        }
    }

    int32_t len;
    ثابت int32_t *data = ures_getIntVector(rb.getAlias(), &len, &ec);
    𐤀𐤌 (U_FAILURE(ec) || len != 4) {
        // Config/build error; return hard-coded defaults
        𐤀𐤌 (U_SUCCESS(ec)) {
            ec = U_INVALID_FORMAT_ERROR;
        }
        أرجع LAST_RESORT_DATA;
    }

    أرجع data;
}

// -------------------------------------

ساكن CharString
idForLocale(ثابت حرف* locale, UErrorCode* ec)
{
    أرجع ulocimp_getRegionForSupplementalData(locale, خطأ, *ec);
}

// ------------------------------------------
//
// Registration
//
//-------------------------------------------

// don't use ICUService since we don't need fallback

U_CDECL_BEGIN
ساكن UBool U_CALLCONV currency_cleanup();
U_CDECL_END

#𐤀𐤌 !UCONFIG_NO_SERVICE
بنية CReg;

ساكن UMutex gCRegLock;
ساكن CReg* gCRegHead = nullptr;

بنية CReg : عام icu::UMemory {
    CReg *next;
    char16_t iso[ISO_CURRENCY_CODE_LENGTH+1];
    حرف  id[ULOC_FULLNAME_CAPACITY];

    CReg(ثابت char16_t* _iso, ثابت حرف* _id)
        : next(nullptr)
    {
        uprv_strncpy(id, _id, حجم(id)-1);
        id[حجم(id)-1] = 0;
        u_memcpy(iso, _iso, ISO_CURRENCY_CODE_LENGTH);
        iso[ISO_CURRENCY_CODE_LENGTH] = 0;
    }

    ساكن UCurrRegistryKey reg(ثابت char16_t* _iso, ثابت حرف* _id, UErrorCode* status)
    {
        𐤀𐤌 (status && U_SUCCESS(*status) && _iso && _id) {
            CReg* n = جديد CReg(_iso, _id);
            𐤀𐤌 (n) {
                umtx_lock(&gCRegLock);
                𐤀𐤌 (!gCRegHead) {
                    /* register for the first time */
                    ucln_common_registerCleanup(UCLN_COMMON_CURRENCY, currency_cleanup);
                }
                n->next = gCRegHead;
                gCRegHead = n;
                umtx_unlock(&gCRegLock);
                أرجع n;
            }
            *status = U_MEMORY_ALLOCATION_ERROR;
        }
        أرجع nullptr;
    }

    ساكن UBool unreg(UCurrRegistryKey key) {
        UBool found = خطأ;
        umtx_lock(&gCRegLock);

        CReg** p = &gCRegHead;
        طالما (*p) {
            𐤀𐤌 (*p == key) {
                *p = ((CReg*)key)->next;
                delete (CReg*)key;
                found = true;
                اكسر;
            }
            p = &((*p)->next);
        }

        umtx_unlock(&gCRegLock);
        أرجع found;
    }

    ساكن ثابت char16_t* get(ثابت حرف* id) {
        ثابت char16_t* result = nullptr;
        umtx_lock(&gCRegLock);
        CReg* p = gCRegHead;

        /* register cleanup of the mutex */
        ucln_common_registerCleanup(UCLN_COMMON_CURRENCY, currency_cleanup);
        طالما (p) {
            𐤀𐤌 (uprv_strcmp(id, p->id) == 0) {
                result = p->iso;
                اكسر;
            }
            p = p->next;
        }
        umtx_unlock(&gCRegLock);
        أرجع result;
    }

    /* This doesn't need to be thread safe. It's for u_cleanup only. */
    ساكن فراغ cleanup() {
        طالما (gCRegHead) {
            CReg* n = gCRegHead;
            gCRegHead = gCRegHead->next;
            delete n;
        }
    }
};

// -------------------------------------

U_CAPI UCurrRegistryKey U_EXPORT2
ucurr_register(ثابت char16_t* isoCode, ثابت حرف* locale, UErrorCode *status)
{
    𐤀𐤌 (status && U_SUCCESS(*status)) {
        CharString id = idForLocale(locale, status);
        أرجع CReg::reg(isoCode, id.data(), status);
    }
    أرجع nullptr;
}

// -------------------------------------

U_CAPI UBool U_EXPORT2
ucurr_unregister(UCurrRegistryKey key, UErrorCode* status)
{
    𐤀𐤌 (status && U_SUCCESS(*status)) {
        أرجع CReg::unreg(key);
    }
    أرجع خطأ;
}
#endif /* UCONFIG_NO_SERVICE */

// -------------------------------------

/**
 * Release all static memory held by currency.
 */
/*The declaration here is needed so currency_cleanup()
 * can call this function.
 */
ساكن UBool U_CALLCONV
currency_cache_cleanup();

U_CDECL_BEGIN
ساكن UBool U_CALLCONV currency_cleanup() {
#𐤀𐤌 !UCONFIG_NO_SERVICE
    CReg::cleanup();
#endif
    /*
     * There might be some cached currency data or isoCodes data.
     */
    currency_cache_cleanup();
    isoCodes_cleanup();
    currSymbolsEquiv_cleanup();

    أرجع true;
}
U_CDECL_END

// -------------------------------------

U_CAPI int32_t U_EXPORT2
ucurr_forLocale(ثابت حرف* locale,
                char16_t* buff,
                int32_t buffCapacity,
                UErrorCode* ec) {
    𐤀𐤌 (U_FAILURE(*ec)) { أرجع 0; }
    𐤀𐤌 (buffCapacity < 0 || (buff == nullptr && buffCapacity > 0)) {
        *ec = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع 0;
    }

    UErrorCode localStatus = U_ZERO_ERROR;
    CharString currency = ulocimp_getKeywordValue(locale, "currency", localStatus);
    int32_t resLen = currency.length();

    𐤀𐤌 (U_SUCCESS(localStatus) && resLen == 3 && uprv_isInvariantString(currency.data(), resLen)) {
        𐤀𐤌 (resLen < buffCapacity) {
            T_CString_toUpperCase(currency.data());
            u_charsToUChars(currency.data(), buff, resLen);
        }
        أرجع u_terminateUChars(buff, buffCapacity, resLen, ec);
    }

    // get country or country_variant in `id'
    CharString id = idForLocale(locale, ec);
    𐤀𐤌 (U_FAILURE(*ec)) {
        أرجع 0;
    }

#𐤀𐤌 !UCONFIG_NO_SERVICE
    ثابت char16_t* result = CReg::get(id.data());
    𐤀𐤌 (result) {
        𐤀𐤌(buffCapacity > u_strlen(result)) {
            u_strcpy(buff, result);
        }
        resLen = u_strlen(result);
        أرجع u_terminateUChars(buff, buffCapacity, resLen, ec);
    }
#endif
    // Remove variants, which is only needed for registration.
    حرف *idDelim = uprv_strchr(id.data(), VAR_DELIM);
    𐤀𐤌 (idDelim) {
        id.truncate(idDelim - id.data());
    }

    ثابت char16_t* s = nullptr;  // Currency code from data file.
    𐤀𐤌 (id.isEmpty()) {
        // No point looking in the data for an empty string.
        // This is what we would get.
        localStatus = U_MISSING_RESOURCE_ERROR;
    } وإلا {
        // Look up the CurrencyMap element in the root bundle.
        localStatus = U_ZERO_ERROR;
        UResourceBundle *rb = ures_openDirect(U_ICUDATA_CURR, CURRENCY_DATA, &localStatus);
        UResourceBundle *cm = ures_getByKey(rb, CURRENCY_MAP, rb, &localStatus);
        LocalUResourceBundlePointer countryArray(ures_getByKey(rb, id.data(), cm, &localStatus));
        // https://unicode-org.atlassian.net/browse/ICU-21997
        // Prefer to use currencies that are legal tender.
        𐤀𐤌 (U_SUCCESS(localStatus)) {
            int32_t arrayLength = ures_getSize(countryArray.getAlias());
            لكل (int32_t i = 0; i < arrayLength; ++i) {
                LocalUResourceBundlePointer currencyReq(
                    ures_getByIndex(countryArray.getAlias(), i, nullptr, &localStatus));
                // The currency is legal tender if it is *not* marked with tender{"false"}.
                UErrorCode tenderStatus = localStatus;
                ثابت char16_t *tender =
                    ures_getStringByKey(currencyReq.getAlias(), "tender", nullptr, &tenderStatus);
                منطقي isTender = U_FAILURE(tenderStatus) || u_strcmp(tender, u"false") != 0;
                𐤀𐤌 (!isTender && s != nullptr) {
                    // We already have a non-tender currency. Ignore all following non-tender ones.
                    استمر;
                }
                // Fetch the currency code.
                s = ures_getStringByKey(currencyReq.getAlias(), "id", &resLen, &localStatus);
                𐤀𐤌 (isTender) {
                    اكسر;
                }
            }
            𐤀𐤌 (U_SUCCESS(localStatus) && s == nullptr) {
                localStatus = U_MISSING_RESOURCE_ERROR;
            }
        }
    }

    𐤀𐤌 ((U_FAILURE(localStatus)) && strchr(id.data(), '_') != nullptr) {
        // We don't know about it.  Check to see if we support the variant.
        CharString parent = ulocimp_getParent(locale, *ec);
        *ec = U_USING_FALLBACK_WARNING;
        // TODO: Loop over the parent rather than recursing and
        // looking again for a currency keyword.
        أرجع ucurr_forLocale(parent.data(), buff, buffCapacity, ec);
    }
    𐤀𐤌 (*ec == U_ZERO_ERROR || localStatus != U_ZERO_ERROR) {
        // There is nothing to fallback to. Report the failure/warning if possible.
        *ec = localStatus;
    }
    𐤀𐤌 (U_SUCCESS(*ec)) {
        𐤀𐤌(buffCapacity > resLen) {
            u_strcpy(buff, s);
        }
    }
    أرجع u_terminateUChars(buff, buffCapacity, resLen, ec);
}

// end registration

/**
 * Modify the given locale name by removing the rightmost _-delimited
 * element.  If there is none, empty the string ("" == root).
 * NOTE: The string "root" is not recognized; do not use it.
 * @return true if the fallback happened; false if locale is already
 * root ("").
 */
ساكن UBool fallback(CharString& loc) {
    𐤀𐤌 (loc.isEmpty()) {
        أرجع خطأ;
    }
    UErrorCode status = U_ZERO_ERROR;
    𐤀𐤌 (loc == "en_GB") {
        // HACK: See #13368.  We need "en_GB" to fall back to "en_001" instead of "en"
        // in order to consume the correct data strings.  This hack will be removed
        // when proper data sink loading is implemented here.
        loc.truncate(3);
        loc.append("001", status);
    } وإلا {
        loc = ulocimp_getParent(loc.data(), status);
    }
 /*
    char *i = uprv_strrchr(loc, '_');
    if (i == nullptr) {
        i = loc;
    }
    *i = 0;
 */
    أرجع true;
}


U_CAPI ثابت char16_t* U_EXPORT2
ucurr_getName(ثابت char16_t* currency U_LIFETIME_BOUND,
              ثابت حرف* locale,
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

    𐤀𐤌 (U_FAILURE(*ec)) {
        أرجع nullptr;
    }

    int32_t choice = (int32_t) nameStyle;
    𐤀𐤌 (choice < 0 || choice > 4) {
        *ec = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع nullptr;
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

    𐤀𐤌 (locale == nullptr) {
        locale = uloc_getDefault();
    }
    CharString loc = ulocimp_getName(locale, ec2);
    𐤀𐤌 (U_FAILURE(ec2)) {
        *ec = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع nullptr;
    }

    حرف buf[ISO_CURRENCY_CODE_LENGTH+1];
    myUCharsToChars(buf, currency);
    
    /* Normalize the keyword value to uppercase */
    T_CString_toUpperCase(buf);
    
    ثابت char16_t* s = nullptr;
    ec2 = U_ZERO_ERROR;
    LocalUResourceBundlePointer rb(ures_open(U_ICUDATA_CURR, loc.data(), &ec2));

    𐤀𐤌 (nameStyle == UCURR_NARROW_SYMBOL_NAME || nameStyle == UCURR_FORMAL_SYMBOL_NAME || nameStyle == UCURR_VARIANT_SYMBOL_NAME) {
        CharString key;
        بدل (nameStyle) {
        حالة UCURR_NARROW_SYMBOL_NAME:
            key.append(CURRENCIES_NARROW, ec2);
            اكسر;
        حالة UCURR_FORMAL_SYMBOL_NAME:
            key.append(CURRENCIES_FORMAL, ec2);
            اكسر;
        حالة UCURR_VARIANT_SYMBOL_NAME:
            key.append(CURRENCIES_VARIANT, ec2);
            اكسر;
        افتراضي:
            *ec = U_UNSUPPORTED_ERROR;
            أرجع nullptr;
        }
        key.append("/", ec2);
        key.append(buf, ec2);
        s = ures_getStringByKeyWithFallback(rb.getAlias(), key.data(), len, &ec2);
        𐤀𐤌 (ec2 == U_MISSING_RESOURCE_ERROR) {
            *ec = U_USING_FALLBACK_WARNING;
            ec2 = U_ZERO_ERROR;
            choice = UCURR_SYMBOL_NAME;
        }
    }
    𐤀𐤌 (s == nullptr) {
        ures_getByKey(rb.getAlias(), CURRENCIES, rb.getAlias(), &ec2);
        ures_getByKeyWithFallback(rb.getAlias(), buf, rb.getAlias(), &ec2);
        s = ures_getStringByIndex(rb.getAlias(), choice, len, &ec2);
    }

    // If we've succeeded we're done.  Otherwise, try to fallback.
    // If that fails (because we are already at root) then exit.
    𐤀𐤌 (U_SUCCESS(ec2)) {
        𐤀𐤌 (ec2 == U_USING_DEFAULT_WARNING
            || (ec2 == U_USING_FALLBACK_WARNING && *ec != U_USING_DEFAULT_WARNING)) {
            *ec = ec2;
        }
    }

    // We no longer support choice format data in names.  Data should not contain
    // choice patterns.
    𐤀𐤌 (isChoiceFormat != nullptr) {
        *isChoiceFormat = خطأ;
    }
    𐤀𐤌 (U_SUCCESS(ec2)) {
        U_ASSERT(s != nullptr);
        أرجع s;
    }

    // If we fail to find a match, use the ISO 4217 code
    *len = u_strlen(currency); // Should == ISO_CURRENCY_CODE_LENGTH, but maybe not...?
    *ec = U_USING_DEFAULT_WARNING;
    أرجع currency;
}

U_CAPI ثابت char16_t* U_EXPORT2
ucurr_getPluralName(ثابت char16_t* currency U_LIFETIME_BOUND,
                    ثابت حرف* locale,
                    UBool* isChoiceFormat,
                    ثابت حرف* pluralCount,
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

    𐤀𐤌 (U_FAILURE(*ec)) {
        أرجع nullptr;
    }

    // Use a separate UErrorCode here that does not propagate out of
    // this function.
    UErrorCode ec2 = U_ZERO_ERROR;

    𐤀𐤌 (locale == nullptr) {
        locale = uloc_getDefault();
    }
    CharString loc = ulocimp_getName(locale, ec2);
    𐤀𐤌 (U_FAILURE(ec2)) {
        *ec = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع nullptr;
    }

    حرف buf[ISO_CURRENCY_CODE_LENGTH+1];
    myUCharsToChars(buf, currency);

    ثابت char16_t* s = nullptr;
    ec2 = U_ZERO_ERROR;
    UResourceBundle* rb = ures_open(U_ICUDATA_CURR, loc.data(), &ec2);

    rb = ures_getByKey(rb, CURRENCYPLURALS, rb, &ec2);

    // Fetch resource with multi-level resource inheritance fallback
    LocalUResourceBundlePointer curr(ures_getByKeyWithFallback(rb, buf, rb, &ec2));

    s = ures_getStringByKeyWithFallback(curr.getAlias(), pluralCount, len, &ec2);
    𐤀𐤌 (U_FAILURE(ec2)) {
        //  fall back to "other"
        ec2 = U_ZERO_ERROR;
        s = ures_getStringByKeyWithFallback(curr.getAlias(), "other", len, &ec2);     
        𐤀𐤌 (U_FAILURE(ec2)) {
            // fall back to long name in Currencies
            أرجع ucurr_getName(currency, locale, UCURR_LONG_NAME, 
                                 isChoiceFormat, len, ec);
        }
    }

    // If we've succeeded we're done.  Otherwise, try to fallback.
    // If that fails (because we are already at root) then exit.
    𐤀𐤌 (U_SUCCESS(ec2)) {
        𐤀𐤌 (ec2 == U_USING_DEFAULT_WARNING
            || (ec2 == U_USING_FALLBACK_WARNING && *ec != U_USING_DEFAULT_WARNING)) {
            *ec = ec2;
        }
        U_ASSERT(s != nullptr);
        أرجع s;
    }

    // If we fail to find a match, use the ISO 4217 code
    *len = u_strlen(currency); // Should == ISO_CURRENCY_CODE_LENGTH, but maybe not...?
    *ec = U_USING_DEFAULT_WARNING;
    أرجع currency;
}


//========================================================================
// Following are structure and function for parsing currency names

#define NEED_TO_BE_DELETED 0x1

// TODO: a better way to define this?
#define MAX_CURRENCY_NAME_LEN 100

تعريفنوع بنية {
    ثابت حرف* IsoCode;  // key
    char16_t* currencyName;  // value
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
ساكن صحيح U_CALLCONV currencyNameComparator(ثابت فراغ* a, ثابت فراغ* b) {
    ثابت CurrencyNameStruct* currName_1 = static_cast<ثابت CurrencyNameStruct*>(a);
    ثابت CurrencyNameStruct* currName_2 = static_cast<ثابت CurrencyNameStruct*>(b);
    لكل (int32_t i = 0; 
         i < MIN(currName_1->currencyNameLen, currName_2->currencyNameLen);
         ++i) {
        𐤀𐤌 (currName_1->currencyName[i] < currName_2->currencyName[i]) {
            أرجع -1;
        }
        𐤀𐤌 (currName_1->currencyName[i] > currName_2->currencyName[i]) {
            أرجع 1;
        }
    }
    𐤀𐤌 (currName_1->currencyNameLen < currName_2->currencyNameLen) {
        أرجع -1;
    } وإلا 𐤀𐤌 (currName_1->currencyNameLen > currName_2->currencyNameLen) {
        أرجع 1;
    }
    أرجع 0;
}


// Give a locale, return the maximum number of currency names associated with
// this locale.
// It gets currency names from resource bundles using fallback.
// It is the maximum number because in the fallback chain, some of the 
// currency names are duplicated.
// For example, given locale as "en_US", the currency names get from resource
// bundle in "en_US" and "en" are duplicated. The fallback mechanism will count
// all currency names in "en_US" and "en".
ساكن فراغ
getCurrencyNameCount(ثابت حرف* loc, int32_t* total_currency_name_count, int32_t* total_currency_symbol_count) {
    U_NAMESPACE_USE
    *total_currency_name_count = 0;
    *total_currency_symbol_count = 0;
    ثابت char16_t* s = nullptr;
    CharString locale;
    {
        UErrorCode status = U_ZERO_ERROR;
        locale.append(loc, status);
        𐤀𐤌 (U_FAILURE(status)) { أرجع; }
    }
    ثابت icu::Hashtable *currencySymbolsEquiv = getCurrSymbolsEquiv();
    لكل (;;) {
        UErrorCode ec2 = U_ZERO_ERROR;
        // TODO: ures_openDirect?
        LocalUResourceBundlePointer rb(ures_open(U_ICUDATA_CURR, locale.data(), &ec2));
        LocalUResourceBundlePointer curr(ures_getByKey(rb.getAlias(), CURRENCIES, nullptr, &ec2));
        int32_t n = ures_getSize(curr.getAlias());
        لكل (int32_t i=0; i<n; ++i) {
            LocalUResourceBundlePointer names(ures_getByIndex(curr.getAlias(), i, nullptr, &ec2));
            int32_t len;
            s = ures_getStringByIndex(names.getAlias(), UCURR_SYMBOL_NAME, &len, &ec2);
            ++(*total_currency_symbol_count);  // currency symbol
            𐤀𐤌 (currencySymbolsEquiv != nullptr) {
                *total_currency_symbol_count += countEquivalent(*currencySymbolsEquiv, UnicodeString(true, s, len));
            }
            ++(*total_currency_symbol_count); // iso code
            ++(*total_currency_name_count); // long name
        }

        // currency plurals
        UErrorCode ec3 = U_ZERO_ERROR;
        LocalUResourceBundlePointer curr_p(ures_getByKey(rb.getAlias(), CURRENCYPLURALS, nullptr, &ec3));
        n = ures_getSize(curr_p.getAlias());
        لكل (int32_t i=0; i<n; ++i) {
            LocalUResourceBundlePointer names(ures_getByIndex(curr_p.getAlias(), i, nullptr, &ec3));
            *total_currency_name_count += ures_getSize(names.getAlias());
        }

        𐤀𐤌 (!fallback(locale)) {
            اكسر;
        }
    }
}

ساكن char16_t*
toUpperCase(ثابت char16_t* source, int32_t len, ثابت حرف* locale) {
    char16_t* dest = nullptr;
    UErrorCode ec = U_ZERO_ERROR;
    int32_t destLen = u_strToUpper(dest, 0, source, len, locale, &ec);

    ec = U_ZERO_ERROR;
    dest = static_cast<char16_t*>(uprv_malloc(حجم(char16_t) * MAX(destLen, len)));
    𐤀𐤌 (dest == nullptr) {
        أرجع nullptr;
    }
    u_strToUpper(dest, destLen, source, len, locale, &ec);
    𐤀𐤌 (U_FAILURE(ec)) {
        u_memcpy(dest, source, len);
    } 
    أرجع dest;
}


ساكن فراغ deleteCurrencyNames(CurrencyNameStruct* currencyNames, int32_t count);
// Collect all available currency names associated with the given locale
// (enable fallback chain).
// Read currenc names defined in resource bundle "Currencies" and
// "CurrencyPlural", enable fallback chain.
// return the malloc-ed currency name arrays and the total number of currency
// names in the array.
ساكن فراغ
collectCurrencyNames(ثابت حرف* locale, 
                     CurrencyNameStruct** currencyNames, 
                     int32_t* total_currency_name_count, 
                     CurrencyNameStruct** currencySymbols, 
                     int32_t* total_currency_symbol_count, 
                     UErrorCode& ec) {
    𐤀𐤌 (U_FAILURE(ec)) {
        *currencyNames = *currencySymbols = nullptr;
        *total_currency_name_count = *total_currency_symbol_count = 0;
        أرجع;
    }
    U_NAMESPACE_USE
    ثابت icu::Hashtable *currencySymbolsEquiv = getCurrSymbolsEquiv();
    // Look up the Currencies resource for the given locale.
    UErrorCode ec2 = U_ZERO_ERROR;

    𐤀𐤌 (locale == nullptr) {
        locale = uloc_getDefault();
    }
    CharString loc = ulocimp_getName(locale, ec2);
    𐤀𐤌 (U_FAILURE(ec2)) {
        ec = U_ILLEGAL_ARGUMENT_ERROR;
        *currencyNames = *currencySymbols = nullptr;
        *total_currency_name_count = *total_currency_symbol_count = 0;
        أرجع;
    }

    // Get maximum currency name count first.
    getCurrencyNameCount(loc.data(), total_currency_name_count, total_currency_symbol_count);

    *currencyNames = static_cast<CurrencyNameStruct*>(
        uprv_malloc(حجم(CurrencyNameStruct) * (*total_currency_name_count)));
    𐤀𐤌(*currencyNames == nullptr) {
        *currencySymbols = nullptr;
        *total_currency_name_count = *total_currency_symbol_count = 0;
        ec = U_MEMORY_ALLOCATION_ERROR;
        أرجع;
    }
    *currencySymbols = static_cast<CurrencyNameStruct*>(
        uprv_malloc(حجم(CurrencyNameStruct) * (*total_currency_symbol_count)));

    𐤀𐤌(*currencySymbols == nullptr) {
        uprv_free(*currencyNames);
        *currencyNames = nullptr;
        *total_currency_name_count = *total_currency_symbol_count = 0;
        ec = U_MEMORY_ALLOCATION_ERROR;
        أرجع;
    }

    ثابت char16_t* s = nullptr;  // currency name
    حرف* iso = nullptr;  // currency ISO code

    *total_currency_name_count = 0;
    *total_currency_symbol_count = 0;

    UErrorCode ec3 = U_ZERO_ERROR;
    UErrorCode ec4 = U_ZERO_ERROR;

    // Using hash to remove duplicates caused by locale fallback
    LocalUHashtablePointer currencyIsoCodes(uhash_open(uhash_hashChars, uhash_compareChars, nullptr, &ec3));
    LocalUHashtablePointer currencyPluralIsoCodes(uhash_open(uhash_hashChars, uhash_compareChars, nullptr, &ec4));
    لكل (int32_t localeLevel = 0; ; ++localeLevel) {
        ec2 = U_ZERO_ERROR;
        // TODO: ures_openDirect
        LocalUResourceBundlePointer rb(ures_open(U_ICUDATA_CURR, loc.data(), &ec2));
        LocalUResourceBundlePointer curr(ures_getByKey(rb.getAlias(), CURRENCIES, nullptr, &ec2));
        int32_t n = ures_getSize(curr.getAlias());
        لكل (int32_t i=0; i<n; ++i) {
            LocalUResourceBundlePointer names(ures_getByIndex(curr.getAlias(), i, nullptr, &ec2));
            int32_t len;
            s = ures_getStringByIndex(names.getAlias(), UCURR_SYMBOL_NAME, &len, &ec2);
            // TODO: uhash_put wont change key/value?
            iso = const_cast<حرف*>(ures_getKey(names.getAlias()));
            𐤀𐤌 (localeLevel != 0 && uhash_get(currencyIsoCodes.getAlias(), iso) != nullptr) {
                استمر;
            }
            uhash_put(currencyIsoCodes.getAlias(), iso, iso, &ec3); 
            // Add currency symbol.
            (*currencySymbols)[(*total_currency_symbol_count)++] = {iso, const_cast<char16_t*>(s), len, 0};

            // Add equivalent symbols
            𐤀𐤌 (currencySymbolsEquiv != nullptr) {
                UnicodeString str(true, s, len);
                icu::EquivIterator iter(*currencySymbolsEquiv, str);
                ثابت UnicodeString *symbol;
                طالما ((symbol = iter.next()) != nullptr) {
                    (*currencySymbols)[(*total_currency_symbol_count)++]
                        = {iso, const_cast<char16_t*>(symbol->getBuffer()), symbol->length(), 0};
                }
            }

            // Add currency long name.
            s = ures_getStringByIndex(names.getAlias(), UCURR_LONG_NAME, &len, &ec2);
            char16_t* upperName = toUpperCase(s, len, locale);
            𐤀𐤌 (upperName == nullptr) {
                ec = U_MEMORY_ALLOCATION_ERROR;
                اذهب error;
            }
            (*currencyNames)[(*total_currency_name_count)++] = {iso, upperName, len, NEED_TO_BE_DELETED};

            // put (iso, 3, and iso) in to array
            // Add currency ISO code.
            char16_t* isoCode = static_cast<char16_t*>(uprv_malloc(حجم(char16_t) * 3));
            𐤀𐤌 (isoCode == nullptr) {
                ec = U_MEMORY_ALLOCATION_ERROR;
                اذهب error;
            }
            // Must convert iso[] into Unicode
            u_charsToUChars(iso, isoCode, 3);
            (*currencySymbols)[(*total_currency_symbol_count)++] = {iso, isoCode, 3, NEED_TO_BE_DELETED};
        }

        // currency plurals
        UErrorCode ec5 = U_ZERO_ERROR;
        LocalUResourceBundlePointer curr_p(ures_getByKey(rb.getAlias(), CURRENCYPLURALS, nullptr, &ec5));
        n = ures_getSize(curr_p.getAlias());
        لكل (int32_t i=0; i<n; ++i) {
            LocalUResourceBundlePointer names(ures_getByIndex(curr_p.getAlias(), i, nullptr, &ec5));
            iso = const_cast<حرف*>(ures_getKey(names.getAlias()));
            // Using hash to remove duplicated ISO codes in fallback chain.
            𐤀𐤌 (localeLevel != 0 && uhash_get(currencyPluralIsoCodes.getAlias(), iso) != nullptr) {
                استمر;
            }
            uhash_put(currencyPluralIsoCodes.getAlias(), iso, iso, &ec4);
            int32_t num = ures_getSize(names.getAlias());
            int32_t len;
            لكل (int32_t j = 0; j < num; ++j) {
                // TODO: remove duplicates between singular name and 
                // currency long name?
                s = ures_getStringByIndex(names.getAlias(), j, &len, &ec5);
                char16_t* upperName = toUpperCase(s, len, locale);
                𐤀𐤌 (upperName == nullptr) {
                    ec = U_MEMORY_ALLOCATION_ERROR;
                    اذهب error;
                }
                (*currencyNames)[(*total_currency_name_count)++] = {iso, upperName, len, NEED_TO_BE_DELETED};
            }
        }

        𐤀𐤌 (!fallback(loc)) {
            اكسر;
        }
    }

    // quick sort the struct
    qsort(*currencyNames, *total_currency_name_count, 
          حجم(CurrencyNameStruct), currencyNameComparator);
    qsort(*currencySymbols, *total_currency_symbol_count, 
          حجم(CurrencyNameStruct), currencyNameComparator);

#ifdef UCURR_DEBUG
    printf("currency name count: %d\n", *total_currency_name_count);
    لكل (int32_t index = 0; index < *total_currency_name_count; ++index) {
        printf("index: %d\n", index);
        printf("iso: %s\n", (*currencyNames)[index].IsoCode);
        حرف curNameBuf[1024];
        memset(curNameBuf, 0, 1024);
        u_austrncpy(curNameBuf, (*currencyNames)[index].currencyName, (*currencyNames)[index].currencyNameLen);
        printf("currencyName: %s\n", curNameBuf);
        printf("len: %d\n", (*currencyNames)[index].currencyNameLen);
    }
    printf("currency symbol count: %d\n", *total_currency_symbol_count);
    لكل (int32_t index = 0; index < *total_currency_symbol_count; ++index) {
        printf("index: %d\n", index);
        printf("iso: %s\n", (*currencySymbols)[index].IsoCode);
        حرف curNameBuf[1024];
        memset(curNameBuf, 0, 1024);
        u_austrncpy(curNameBuf, (*currencySymbols)[index].currencyName, (*currencySymbols)[index].currencyNameLen);
        printf("currencySymbol: %s\n", curNameBuf);
        printf("len: %d\n", (*currencySymbols)[index].currencyNameLen);
    }
#endif
    // fail on hashtable errors
    𐤀𐤌 (U_FAILURE(ec3)) {
      ec = ec3;
    } وإلا 𐤀𐤌 (U_FAILURE(ec4)) {
      ec = ec4;
    }

  error:
    // clean up if we got error
    𐤀𐤌 (U_FAILURE(ec)) {
        deleteCurrencyNames(*currencyNames, *total_currency_name_count);
        deleteCurrencyNames(*currencySymbols, *total_currency_symbol_count);
        *currencyNames = *currencySymbols = nullptr;
        *total_currency_name_count = *total_currency_symbol_count = 0;
    }
}

// @param  currencyNames: currency names array
// @param  indexInCurrencyNames: the index of the character in currency names 
//         array against which the comparison is done
// @param  key: input text char to compare against
// @param  begin(IN/OUT): the begin index of matching range in currency names array
// @param  end(IN/OUT): the end index of matching range in currency names array.
ساكن int32_t
binarySearch(ثابت CurrencyNameStruct* currencyNames, 
             int32_t indexInCurrencyNames,
             ثابت char16_t key,
             int32_t* begin, int32_t* end) {
#ifdef UCURR_DEBUG
    printf("key = %x\n", key);
#endif
   int32_t first = *begin;
   int32_t last = *end;
   طالما (first <= last) {
       int32_t mid = (first + last) / 2;  // compute mid point.
       𐤀𐤌 (indexInCurrencyNames >= currencyNames[mid].currencyNameLen) {
           first = mid + 1;
       } وإلا {
           𐤀𐤌 (key > currencyNames[mid].currencyName[indexInCurrencyNames]) {
               first = mid + 1;
           }
           وإلا 𐤀𐤌 (key < currencyNames[mid].currencyName[indexInCurrencyNames]) {
               last = mid - 1;
           }
           وإلا {
                // Find a match, and looking for ranges
                // Now do two more binary searches. First, on the left side for
                // the greatest L such that CurrencyNameStruct[L] < key.
                int32_t L = *begin;
                int32_t R = mid;

#ifdef UCURR_DEBUG
                printf("mid = %d\n", mid);
#endif
                طالما (L < R) {
                    int32_t M = (L + R) / 2;
#ifdef UCURR_DEBUG
                    printf("L = %d, R = %d, M = %d\n", L, R, M);
#endif
                    𐤀𐤌 (indexInCurrencyNames >= currencyNames[M].currencyNameLen) {
                        L = M + 1;
                    } وإلا {
                        𐤀𐤌 (currencyNames[M].currencyName[indexInCurrencyNames] < key) {
                            L = M + 1;
                        } وإلا {
#ifdef UCURR_DEBUG
                            U_ASSERT(currencyNames[M].currencyName[indexInCurrencyNames] == key);
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
                U_ASSERT(currencyNames[*begin].currencyName[indexInCurrencyNames] == key);
#endif

                // Now for the second search, finding the least R such that
                // key < CurrencyNameStruct[R].
                L = mid;
                R = *end;
                طالما (L < R) {
                    int32_t M = (L + R) / 2;
#ifdef UCURR_DEBUG
                    printf("L = %d, R = %d, M = %d\n", L, R, M);
#endif
                    𐤀𐤌 (currencyNames[M].currencyNameLen < indexInCurrencyNames) {
                        L = M + 1;
                    } وإلا {
                        𐤀𐤌 (currencyNames[M].currencyName[indexInCurrencyNames] > key) {
                            R = M;
                        } وإلا {
#ifdef UCURR_DEBUG
                            U_ASSERT(currencyNames[M].currencyName[indexInCurrencyNames] == key);
#endif
                            L = M + 1;
                        }
                    }
                }
#ifdef UCURR_DEBUG
                U_ASSERT(L == R);
#endif
                𐤀𐤌 (currencyNames[R].currencyName[indexInCurrencyNames] > key) {
                    *end = R - 1;
                } وإلا {
                    *end = R;
                }
#ifdef UCURR_DEBUG
                printf("end = %d\n", *end);
#endif

                // now, found the range. check whether there is exact match
                𐤀𐤌 (currencyNames[*begin].currencyNameLen == indexInCurrencyNames + 1) {
                    أرجع *begin;  // find range and exact match.
                }
                أرجع -1;  // find range, but no exact match.
           }
       }
   }
   *begin = -1;
   *end = -1;
   أرجع -1;    // failed to find range.
}


// Linear search "text" in "currencyNames".
// @param  begin, end: the begin and end index in currencyNames, within which
//         range should the search be performed.
// @param  textLen: the length of the text to be compared
// @param  maxMatchLen(IN/OUT): passing in the computed max matching length
//                              pass out the new max  matching length
// @param  maxMatchIndex: the index in currencyName which has the longest
//                        match with input text.
ساكن فراغ
linearSearch(ثابت CurrencyNameStruct* currencyNames, 
             int32_t begin, int32_t end,
             ثابت char16_t* text, int32_t textLen,
             int32_t *partialMatchLen,
             int32_t *maxMatchLen, int32_t* maxMatchIndex) {
    int32_t initialPartialMatchLen = *partialMatchLen;
    لكل (int32_t index = begin; index <= end; ++index) {
        int32_t len = currencyNames[index].currencyNameLen;
        𐤀𐤌 (len > *maxMatchLen && len <= textLen &&
            uprv_memcmp(currencyNames[index].currencyName, text, len * حجم(char16_t)) == 0) {
            *partialMatchLen = MAX(*partialMatchLen, len);
            *maxMatchIndex = index;
            *maxMatchLen = len;
#ifdef UCURR_DEBUG
            printf("maxMatchIndex = %d, maxMatchLen = %d\n",
                   *maxMatchIndex, *maxMatchLen);
#endif
        } وإلا {
            // Check for partial matches.
            لكل (int32_t i=initialPartialMatchLen; i<MIN(len, textLen); i++) {
                𐤀𐤌 (currencyNames[index].currencyName[i] != text[i]) {
                    اكسر;
                }
                *partialMatchLen = MAX(*partialMatchLen, i + 1);
            }
        }
    }
}

#define LINEAR_SEARCH_THRESHOLD 10

// Find longest match between "text" and currency names in "currencyNames".
// @param  total_currency_count: total number of currency names in CurrencyNames.
// @param  textLen: the length of the text to be compared
// @param  maxMatchLen: passing in the computed max matching length
//                              pass out the new max  matching length
// @param  maxMatchIndex: the index in currencyName which has the longest
//                        match with input text.
ساكن فراغ
searchCurrencyName(ثابت CurrencyNameStruct* currencyNames, 
                   int32_t total_currency_count,
                   ثابت char16_t* text, int32_t textLen,
                   int32_t *partialMatchLen,
                   int32_t* maxMatchLen, int32_t* maxMatchIndex) {
    *maxMatchIndex = -1;
    *maxMatchLen = 0;
    int32_t matchIndex = -1;
    int32_t binarySearchBegin = 0;
    int32_t binarySearchEnd = total_currency_count - 1;
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
    لكل (int32_t index = 0; index < textLen; ++index) {
        // matchIndex saves the one with exact match till the current point.
        // [binarySearchBegin, binarySearchEnd] saves the matching range.
        matchIndex = binarySearch(currencyNames, index,
                                  text[index],
                                  &binarySearchBegin, &binarySearchEnd);
        𐤀𐤌 (binarySearchBegin == -1) { // did not find the range
            اكسر;
        }
        *partialMatchLen = MAX(*partialMatchLen, index + 1);
        𐤀𐤌 (matchIndex != -1) { 
            // find an exact match for text from text[0] to text[index] 
            // in currencyNames array.
            *maxMatchLen = index + 1;
            *maxMatchIndex = matchIndex;
        }
        𐤀𐤌 (binarySearchEnd - binarySearchBegin < LINEAR_SEARCH_THRESHOLD) {
            // linear search if within threshold.
            linearSearch(currencyNames, binarySearchBegin, binarySearchEnd,
                         text, textLen,
                         partialMatchLen,
                         maxMatchLen, maxMatchIndex);
            اكسر;
        }
    }
}

//========================= currency name cache =====================
تعريفنوع بنية {
    حرف locale[ULOC_FULLNAME_CAPACITY];  //key
    // currency names, case insensitive
    CurrencyNameStruct* currencyNames;  // value
    int32_t totalCurrencyNameCount;  // currency name count
    // currency symbols and ISO code, case sensitive
    CurrencyNameStruct* currencySymbols; // value
    int32_t totalCurrencySymbolCount;  // count
    // reference count.
    // reference count is set to 1 when an entry is put to cache.
    // it increases by 1 before accessing, and decreased by 1 after accessing.
    // The entry is deleted when ref count is zero, which means 
    // the entry is replaced out of cache and no process is accessing it.
    int32_t refCount;
} CurrencyNameCacheEntry;


#define CURRENCY_NAME_CACHE_NUM 10

// Reserve 10 cache entries.
ساكن CurrencyNameCacheEntry* currCache[CURRENCY_NAME_CACHE_NUM] = {nullptr};
// Using an index to indicate which entry to be replaced when cache is full.
// It is a simple round-robin replacement strategy.
ساكن int8_t currentCacheEntryIndex = 0;

ساكن UMutex gCurrencyCacheMutex;

// Cache deletion
ساكن فراغ
deleteCurrencyNames(CurrencyNameStruct* currencyNames, int32_t count) {
    لكل (int32_t index = 0; index < count; ++index) {
        𐤀𐤌 ( (currencyNames[index].flag & NEED_TO_BE_DELETED) ) {
            uprv_free(currencyNames[index].currencyName);
        }
    }
    uprv_free(currencyNames);
}


ساكن فراغ
deleteCacheEntry(CurrencyNameCacheEntry* entry) {
    deleteCurrencyNames(entry->currencyNames, entry->totalCurrencyNameCount);
    deleteCurrencyNames(entry->currencySymbols, entry->totalCurrencySymbolCount);
    uprv_free(entry);
}


// Cache clean up
ساكن UBool U_CALLCONV
currency_cache_cleanup() {
    لكل (int32_t i = 0; i < CURRENCY_NAME_CACHE_NUM; ++i) {
        𐤀𐤌 (currCache[i]) {
            deleteCacheEntry(currCache[i]);
            currCache[i] = nullptr;
        }
    }
    أرجع true;
}


/**
 * Loads the currency name data from the cache, or from resource bundles if necessary.
 * The refCount is automatically incremented.  It is the caller's responsibility
 * to decrement it when done!
 */
ساكن CurrencyNameCacheEntry*
getCacheEntry(ثابت حرف* locale, UErrorCode& ec) {

    int32_t total_currency_name_count = 0;
    CurrencyNameStruct* currencyNames = nullptr;
    int32_t total_currency_symbol_count = 0;
    CurrencyNameStruct* currencySymbols = nullptr;
    CurrencyNameCacheEntry* cacheEntry = nullptr;

    umtx_lock(&gCurrencyCacheMutex);
    // in order to handle racing correctly,
    // not putting 'search' in a separate function.
    int8_t found = -1;
    لكل (int8_t i = 0; i < CURRENCY_NAME_CACHE_NUM; ++i) {
        𐤀𐤌 (currCache[i]!= nullptr &&
            uprv_strcmp(locale, currCache[i]->locale) == 0) {
            found = i;
            اكسر;
        }
    }
    𐤀𐤌 (found != -1) {
        cacheEntry = currCache[found];
        ++(cacheEntry->refCount);
    }
    umtx_unlock(&gCurrencyCacheMutex);
    𐤀𐤌 (found == -1) {
        collectCurrencyNames(locale, &currencyNames, &total_currency_name_count, &currencySymbols, &total_currency_symbol_count, ec);
        𐤀𐤌 (U_FAILURE(ec)) {
            أرجع nullptr;
        }
        umtx_lock(&gCurrencyCacheMutex);
        // check again.
        لكل (int8_t i = 0; i < CURRENCY_NAME_CACHE_NUM; ++i) {
            𐤀𐤌 (currCache[i]!= nullptr &&
                uprv_strcmp(locale, currCache[i]->locale) == 0) {
                found = i;
                اكسر;
            }
        }
        𐤀𐤌 (found == -1) {
            // insert new entry to 
            // currentCacheEntryIndex % CURRENCY_NAME_CACHE_NUM
            // and remove the existing entry 
            // currentCacheEntryIndex % CURRENCY_NAME_CACHE_NUM
            // from cache.
            cacheEntry = currCache[currentCacheEntryIndex];
            𐤀𐤌 (cacheEntry) {
                --(cacheEntry->refCount);
                // delete if the ref count is zero
                𐤀𐤌 (cacheEntry->refCount == 0) {
                    deleteCacheEntry(cacheEntry);
                }
            }
            cacheEntry = static_cast<CurrencyNameCacheEntry*>(uprv_malloc(حجم(CurrencyNameCacheEntry)));
            𐤀𐤌 (cacheEntry == nullptr) {
                deleteCurrencyNames(currencyNames, total_currency_name_count);
                deleteCurrencyNames(currencySymbols, total_currency_symbol_count);
                ec = U_MEMORY_ALLOCATION_ERROR;
                أرجع nullptr;
            }
            currCache[currentCacheEntryIndex] = cacheEntry;
            uprv_strcpy(cacheEntry->locale, locale);
            cacheEntry->currencyNames = currencyNames;
            cacheEntry->totalCurrencyNameCount = total_currency_name_count;
            cacheEntry->currencySymbols = currencySymbols;
            cacheEntry->totalCurrencySymbolCount = total_currency_symbol_count;
            cacheEntry->refCount = 2; // one for cache, one for reference
            currentCacheEntryIndex = (currentCacheEntryIndex + 1) % CURRENCY_NAME_CACHE_NUM;
            ucln_common_registerCleanup(UCLN_COMMON_CURRENCY, currency_cleanup);
        } وإلا {
            deleteCurrencyNames(currencyNames, total_currency_name_count);
            deleteCurrencyNames(currencySymbols, total_currency_symbol_count);
            cacheEntry = currCache[found];
            ++(cacheEntry->refCount);
        }
        umtx_unlock(&gCurrencyCacheMutex);
    }

    أرجع cacheEntry;
}

ساكن فراغ releaseCacheEntry(CurrencyNameCacheEntry* cacheEntry) {
    umtx_lock(&gCurrencyCacheMutex);
    --(cacheEntry->refCount);
    𐤀𐤌 (cacheEntry->refCount == 0) {  // remove
        deleteCacheEntry(cacheEntry);
    }
    umtx_unlock(&gCurrencyCacheMutex);
}

U_CAPI فراغ
uprv_parseCurrency(ثابت حرف* locale,
                   ثابت icu::UnicodeString& text,
                   icu::ParsePosition& pos,
                   int8_t type,
                   int32_t* partialMatchLen,
                   char16_t* result,
                   UErrorCode& ec) {
    U_NAMESPACE_USE
    𐤀𐤌 (U_FAILURE(ec)) {
        أرجع;
    }
    CurrencyNameCacheEntry* cacheEntry = getCacheEntry(locale, ec);
    𐤀𐤌 (U_FAILURE(ec)) {
        أرجع;
    }

    int32_t total_currency_name_count = cacheEntry->totalCurrencyNameCount;
    CurrencyNameStruct* currencyNames = cacheEntry->currencyNames;
    int32_t total_currency_symbol_count = cacheEntry->totalCurrencySymbolCount;
    CurrencyNameStruct* currencySymbols = cacheEntry->currencySymbols;

    int32_t start = pos.getIndex();

    char16_t inputText[MAX_CURRENCY_NAME_LEN];
    char16_t upperText[MAX_CURRENCY_NAME_LEN];
    int32_t textLen = MIN(MAX_CURRENCY_NAME_LEN, text.length() - start);
    text.extract(start, textLen, inputText);
    UErrorCode ec1 = U_ZERO_ERROR;
    textLen = u_strToUpper(upperText, MAX_CURRENCY_NAME_LEN, inputText, textLen, locale, &ec1);

    // Make sure partialMatchLen is initialized
    *partialMatchLen = 0;

    int32_t max = 0;
    int32_t matchIndex = -1;
    // case in-sensitive comparison against currency names
    searchCurrencyName(currencyNames, total_currency_name_count, 
                       upperText, textLen, partialMatchLen, &max, &matchIndex);

#ifdef UCURR_DEBUG
    printf("search in names, max = %d, matchIndex = %d\n", max, matchIndex);
#endif

    int32_t maxInSymbol = 0;
    int32_t matchIndexInSymbol = -1;
    𐤀𐤌 (type != UCURR_LONG_NAME) {  // not name only
        // case sensitive comparison against currency symbols and ISO code.
        searchCurrencyName(currencySymbols, total_currency_symbol_count, 
                           inputText, textLen,
                           partialMatchLen,
                           &maxInSymbol, &matchIndexInSymbol);
    }

#ifdef UCURR_DEBUG
    printf("search in symbols, maxInSymbol = %d, matchIndexInSymbol = %d\n", maxInSymbol, matchIndexInSymbol);
    𐤀𐤌(matchIndexInSymbol != -1) {
      printf("== ISO=%s\n", currencySymbols[matchIndexInSymbol].IsoCode);
    }
#endif

    𐤀𐤌 (max >= maxInSymbol && matchIndex != -1) {
        u_charsToUChars(currencyNames[matchIndex].IsoCode, result, 4);
        pos.setIndex(start + max);
    } وإلا 𐤀𐤌 (maxInSymbol >= max && matchIndexInSymbol != -1) {
        u_charsToUChars(currencySymbols[matchIndexInSymbol].IsoCode, result, 4);
        pos.setIndex(start + maxInSymbol);
    }

    // decrease reference count
    releaseCacheEntry(cacheEntry);
}

فراغ uprv_currencyLeads(ثابت حرف* locale, icu::UnicodeSet& result, UErrorCode& ec) {
    U_NAMESPACE_USE
    𐤀𐤌 (U_FAILURE(ec)) {
        أرجع;
    }
    CurrencyNameCacheEntry* cacheEntry = getCacheEntry(locale, ec);
    𐤀𐤌 (U_FAILURE(ec)) {
        أرجع;
    }

    لكل (int32_t i=0; i<cacheEntry->totalCurrencySymbolCount; i++) {
        ثابت CurrencyNameStruct& info = cacheEntry->currencySymbols[i];
        UChar32 cp;
        U16_GET(info.currencyName, 0, 0, info.currencyNameLen, cp);
        result.add(cp);
    }

    لكل (int32_t i=0; i<cacheEntry->totalCurrencyNameCount; i++) {
        ثابت CurrencyNameStruct& info = cacheEntry->currencyNames[i];
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
U_CAPI فراغ
uprv_getStaticCurrencyName(ثابت char16_t* iso, ثابت حرف* loc,
                           icu::UnicodeString& result, UErrorCode& ec)
{
    U_NAMESPACE_USE

    int32_t len;
    ثابت char16_t* currname = ucurr_getName(iso, loc, UCURR_SYMBOL_NAME,
                                          nullptr /* isChoiceFormat */, &len, &ec);
    𐤀𐤌 (U_SUCCESS(ec)) {
        result.setTo(currname, len);
    }
}

U_CAPI int32_t U_EXPORT2
ucurr_getDefaultFractionDigits(ثابت char16_t* currency, UErrorCode* ec) {
    أرجع ucurr_getDefaultFractionDigitsForUsage(currency,UCURR_USAGE_STANDARD,ec);
}

U_CAPI int32_t U_EXPORT2
ucurr_getDefaultFractionDigitsForUsage(ثابت char16_t* currency, ثابت UCurrencyUsage usage, UErrorCode* ec) {
    int32_t fracDigits = 0;
    𐤀𐤌 (U_SUCCESS(*ec)) {
        بدل (usage) {
            حالة UCURR_USAGE_STANDARD:
                fracDigits = (_findMetaData(currency, *ec))[0];
                اكسر;
            حالة UCURR_USAGE_CASH:
                fracDigits = (_findMetaData(currency, *ec))[2];
                اكسر;
            افتراضي:
                *ec = U_UNSUPPORTED_ERROR;
        }
    }
    أرجع fracDigits;
}

U_CAPI مزدوج U_EXPORT2
ucurr_getRoundingIncrement(ثابت char16_t* currency, UErrorCode* ec) {
    أرجع ucurr_getRoundingIncrementForUsage(currency, UCURR_USAGE_STANDARD, ec);
}

U_CAPI مزدوج U_EXPORT2
ucurr_getRoundingIncrementForUsage(ثابت char16_t* currency, ثابت UCurrencyUsage usage, UErrorCode* ec) {
    مزدوج result = 0.0;

    ثابت int32_t *data = _findMetaData(currency, *ec);
    𐤀𐤌 (U_SUCCESS(*ec)) {
        int32_t fracDigits;
        int32_t increment;
        بدل (usage) {
            حالة UCURR_USAGE_STANDARD:
                fracDigits = data[0];
                increment = data[1];
                اكسر;
            حالة UCURR_USAGE_CASH:
                fracDigits = data[2];
                increment = data[3];
                اكسر;
            افتراضي:
                *ec = U_UNSUPPORTED_ERROR;
                أرجع result;
        }

        // If the meta data is invalid, return 0.0
        𐤀𐤌 (fracDigits < 0 || fracDigits > MAX_POW10) {
            *ec = U_INVALID_FORMAT_ERROR;
        } وإلا {
            // A rounding value of 0 or 1 indicates no rounding.
            𐤀𐤌 (increment >= 2) {
                // Return (increment) / 10^(fracDigits).  The only actual rounding data,
                // as of this writing, is CHF { 2, 5 }.
                result = مزدوج(increment) / POW10[fracDigits];
            }
        }
    }

    أرجع result;
}

U_CDECL_BEGIN

تعريفنوع بنية UCurrencyContext {
    uint32_t currType; /* UCurrCurrencyType */
    uint32_t listIdx;
} UCurrencyContext;

/*
Please keep this list in alphabetical order.
You can look at the CLDR supplemental data or ISO-4217 for the meaning of some
of these items.
ISO-4217: http://www.iso.org/iso/en/prods-services/popstds/currencycodeslist.html
*/
ساكن ثابت بنية CurrencyList {
    ثابت حرف *currency;
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

ساكن int32_t U_CALLCONV
ucurr_countCurrencyList(UEnumeration *enumerator, UErrorCode * /*pErrorCode*/) {
    UCurrencyContext *myContext = (UCurrencyContext *)(enumerator->context);
    uint32_t currType = myContext->currType;
    int32_t count = 0;

    /* Count the number of items matching the type we are looking for. */
    لكل (int32_t idx = 0; gCurrencyList[idx].currency != nullptr; idx++) {
        𐤀𐤌 (UCURR_MATCHES_BITMASK(gCurrencyList[idx].currType, currType)) {
            count++;
        }
    }
    أرجع count;
}

ساكن ثابت حرف* U_CALLCONV
ucurr_nextCurrencyList(UEnumeration *enumerator,
                        int32_t* resultLength,
                        UErrorCode * /*pErrorCode*/)
{
    UCurrencyContext *myContext = (UCurrencyContext *)(enumerator->context);

    /* Find the next in the list that matches the type we are looking for. */
    طالما (myContext->listIdx < UPRV_LENGTHOF(gCurrencyList)-1) {
        ثابت بنية CurrencyList *currItem = &gCurrencyList[myContext->listIdx++];
        𐤀𐤌 (UCURR_MATCHES_BITMASK(currItem->currType, myContext->currType))
        {
            𐤀𐤌 (resultLength) {
                *resultLength = 3; /* Currency codes are only 3 chars long */
            }
            أرجع currItem->currency;
        }
    }
    /* We enumerated too far. */
    𐤀𐤌 (resultLength) {
        *resultLength = 0;
    }
    أرجع nullptr;
}

ساكن فراغ U_CALLCONV
ucurr_resetCurrencyList(UEnumeration *enumerator, UErrorCode * /*pErrorCode*/) {
    ((UCurrencyContext *)(enumerator->context))->listIdx = 0;
}

ساكن فراغ U_CALLCONV
ucurr_closeCurrencyList(UEnumeration *enumerator) {
    uprv_free(enumerator->context);
    uprv_free(enumerator);
}

ساكن فراغ U_CALLCONV
ucurr_createCurrencyList(UHashtable *isoCodes, UErrorCode* status){
    UErrorCode localStatus = U_ZERO_ERROR;

    // Look up the CurrencyMap element in the root bundle.
    UResourceBundle *rb = ures_openDirect(U_ICUDATA_CURR, CURRENCY_DATA, &localStatus);
    LocalUResourceBundlePointer currencyMapArray(ures_getByKey(rb, CURRENCY_MAP, rb, &localStatus));

    𐤀𐤌 (U_SUCCESS(localStatus)) {
        // process each entry in currency map
        لكل (int32_t i=0; i<ures_getSize(currencyMapArray.getAlias()); i++) {
            // get the currency resource
            LocalUResourceBundlePointer currencyArray(ures_getByIndex(currencyMapArray.getAlias(), i, nullptr, &localStatus));
            // process each currency
            𐤀𐤌 (U_SUCCESS(localStatus)) {
                لكل (int32_t j=0; j<ures_getSize(currencyArray.getAlias()); j++) {
                    // get the currency resource
                    LocalUResourceBundlePointer currencyRes(ures_getByIndex(currencyArray.getAlias(), j, nullptr, &localStatus));
                    IsoCodeEntry *entry = (IsoCodeEntry*)uprv_malloc(حجم(IsoCodeEntry));
                    𐤀𐤌 (entry == nullptr) {
                        *status = U_MEMORY_ALLOCATION_ERROR;
                        أرجع;
                    }

                    // get the ISO code
                    int32_t isoLength = 0;
                    LocalUResourceBundlePointer idRes(ures_getByKey(currencyRes.getAlias(), "id", nullptr, &localStatus));
                    𐤀𐤌 (idRes.isNull()) {
                        استمر;
                    }
                    ثابت char16_t *isoCode = ures_getString(idRes.getAlias(), &isoLength, &localStatus);

                    // get from date
                    UDate fromDate = U_DATE_MIN;
                    LocalUResourceBundlePointer fromRes(ures_getByKey(currencyRes.getAlias(), "from", nullptr, &localStatus));

                    𐤀𐤌 (U_SUCCESS(localStatus)) {
                        int32_t fromLength = 0;
                        ثابت int32_t *fromArray = ures_getIntVector(fromRes.getAlias(), &fromLength, &localStatus);
                        int64_t currDate64 = ((uint64_t)fromArray[0]) << 32;
                        currDate64 |= ((int64_t)fromArray[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                        fromDate = (UDate)currDate64;
                    }

                    // get to date
                    UDate toDate = U_DATE_MAX;
                    localStatus = U_ZERO_ERROR;
                    LocalUResourceBundlePointer toRes(ures_getByKey(currencyRes.getAlias(), "to", nullptr, &localStatus));

                    𐤀𐤌 (U_SUCCESS(localStatus)) {
                        int32_t toLength = 0;
                        ثابت int32_t *toArray = ures_getIntVector(toRes.getAlias(), &toLength, &localStatus);
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
            } وإلا {
                *status = localStatus;
            }
        }
    } وإلا {
        *status = localStatus;
    }
}

ساكن ثابت UEnumeration gEnumCurrencyList = {
    nullptr,
    nullptr,
    ucurr_closeCurrencyList,
    ucurr_countCurrencyList,
    uenum_unextDefault,
    ucurr_nextCurrencyList,
    ucurr_resetCurrencyList
};
U_CDECL_END


ساكن فراغ U_CALLCONV initIsoCodes(UErrorCode &status) {
    U_ASSERT(gIsoCodes == nullptr);
    ucln_common_registerCleanup(UCLN_COMMON_CURRENCY, currency_cleanup);

    LocalUHashtablePointer isoCodes(uhash_open(uhash_hashUChars, uhash_compareUChars, nullptr, &status));
    𐤀𐤌 (U_FAILURE(status)) {
        أرجع;
    }
    uhash_setValueDeleter(isoCodes.getAlias(), deleteIsoCodeEntry);

    ucurr_createCurrencyList(isoCodes.getAlias(), &status);
    𐤀𐤌 (U_FAILURE(status)) {
        أرجع;
    }
    gIsoCodes = isoCodes.orphan();  // Note: gIsoCodes is const. Once set up here it is never altered,
                                    //       and read only access is safe without synchronization.
}

ساكن فراغ populateCurrSymbolsEquiv(icu::Hashtable *hash, UErrorCode &status) {
    𐤀𐤌 (U_FAILURE(status)) { أرجع; }
    لكل (ثابت auto& entry : unisets::kCurrencyEntries) {
        UnicodeString exemplar(entry.exemplar);
        ثابت UnicodeSet* set = unisets::get(entry.key);
        𐤀𐤌 (set == nullptr) { أرجع; }
        UnicodeSetIterator it(*set);
        طالما (it.next()) {
            UnicodeString value = it.getString();
            𐤀𐤌 (value == exemplar) {
                // No need to mark the exemplar character as an equivalent
                استمر;
            }
            makeEquivalent(exemplar, value, hash, status);
            𐤀𐤌 (U_FAILURE(status)) { أرجع; }
        }
    }
}

ساكن فراغ U_CALLCONV initCurrSymbolsEquiv() {
    U_ASSERT(gCurrSymbolsEquiv == nullptr);
    UErrorCode status = U_ZERO_ERROR;
    ucln_common_registerCleanup(UCLN_COMMON_CURRENCY, currency_cleanup);
    icu::Hashtable *temp = جديد icu::Hashtable(status);
    𐤀𐤌 (temp == nullptr) {
        أرجع;
    }
    𐤀𐤌 (U_FAILURE(status)) {
        delete temp;
        أرجع;
    }
    temp->setValueDeleter(deleteUnicode);
    populateCurrSymbolsEquiv(temp, status);
    𐤀𐤌 (U_FAILURE(status)) {
        delete temp;
        أرجع;
    }
    gCurrSymbolsEquiv = temp;
}

U_CAPI UBool U_EXPORT2
ucurr_isAvailable(ثابت char16_t* isoCode, UDate from, UDate to, UErrorCode* eErrorCode) {
    umtx_initOnce(gIsoCodesInitOnce, &initIsoCodes, *eErrorCode);
    𐤀𐤌 (U_FAILURE(*eErrorCode)) {
        أرجع خطأ;
    }

    IsoCodeEntry* result = (IsoCodeEntry *) uhash_get(gIsoCodes, isoCode);
    𐤀𐤌 (result == nullptr) {
        أرجع خطأ;
    } وإلا 𐤀𐤌 (from > to) {
        *eErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع خطأ;
    } وإلا 𐤀𐤌  ((from > result->to) || (to < result->from)) {
        أرجع خطأ;
    }
    أرجع true;
}

ساكن ثابت icu::Hashtable* getCurrSymbolsEquiv() {
    umtx_initOnce(gCurrSymbolsEquivInitOnce, &initCurrSymbolsEquiv);
    أرجع gCurrSymbolsEquiv;
}

U_CAPI UEnumeration * U_EXPORT2
ucurr_openISOCurrencies(uint32_t currType, UErrorCode *pErrorCode) {
    UEnumeration *myEnum = nullptr;
    UCurrencyContext *myContext;

    myEnum = (UEnumeration*)uprv_malloc(حجم(UEnumeration));
    𐤀𐤌 (myEnum == nullptr) {
        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
        أرجع nullptr;
    }
    uprv_memcpy(myEnum, &gEnumCurrencyList, حجم(UEnumeration));
    myContext = (UCurrencyContext*)uprv_malloc(حجم(UCurrencyContext));
    𐤀𐤌 (myContext == nullptr) {
        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
        uprv_free(myEnum);
        أرجع nullptr;
    }
    myContext->currType = currType;
    myContext->listIdx = 0;
    myEnum->context = myContext;
    أرجع myEnum;
}

U_CAPI int32_t U_EXPORT2
ucurr_countCurrencies(ثابت حرف* locale, 
                 UDate date, 
                 UErrorCode* ec)
{
    int32_t currCount = 0;

    𐤀𐤌 (ec != nullptr && U_SUCCESS(*ec)) 
    {
        // local variables
        UErrorCode localStatus = U_ZERO_ERROR;

        // get country or country_variant in `id'
        CharString id = idForLocale(locale, ec);

        𐤀𐤌 (U_FAILURE(*ec))
        {
            أرجع 0;
        }

        // Remove variants, which is only needed for registration.
        حرف *idDelim = strchr(id.data(), VAR_DELIM);
        𐤀𐤌 (idDelim)
        {
            id.truncate(idDelim - id.data());
        }

        // Look up the CurrencyMap element in the root bundle.
        UResourceBundle *rb = ures_openDirect(U_ICUDATA_CURR, CURRENCY_DATA, &localStatus);
        UResourceBundle *cm = ures_getByKey(rb, CURRENCY_MAP, rb, &localStatus);

        // Using the id derived from the local, get the currency data
        LocalUResourceBundlePointer countryArray(ures_getByKey(rb, id.data(), cm, &localStatus));

        // process each currency to see which one is valid for the given date
        𐤀𐤌 (U_SUCCESS(localStatus))
        {
            لكل (int32_t i=0; i<ures_getSize(countryArray.getAlias()); i++)
            {
                // get the currency resource
                LocalUResourceBundlePointer currencyRes(ures_getByIndex(countryArray.getAlias(), i, nullptr, &localStatus));

                // get the from date
                int32_t fromLength = 0;
                LocalUResourceBundlePointer fromRes(ures_getByKey(currencyRes.getAlias(), "from", nullptr, &localStatus));
                ثابت int32_t *fromArray = ures_getIntVector(fromRes.getAlias(), &fromLength, &localStatus);

                int64_t currDate64 = (int64_t)((uint64_t)(fromArray[0]) << 32);
                currDate64 |= ((int64_t)fromArray[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                UDate fromDate = (UDate)currDate64;

                𐤀𐤌 (ures_getSize(currencyRes.getAlias())> 2)
                {
                    int32_t toLength = 0;
                    LocalUResourceBundlePointer toRes(ures_getByKey(currencyRes.getAlias(), "to", nullptr, &localStatus));
                    ثابت int32_t *toArray = ures_getIntVector(toRes.getAlias(), &toLength, &localStatus);

                    currDate64 = (int64_t)toArray[0] << 32;
                    currDate64 |= ((int64_t)toArray[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                    UDate toDate = (UDate)currDate64;

                    𐤀𐤌 ((fromDate <= date) && (date < toDate))
                    {
                        currCount++;
                    }
                }
                وإلا
                {
                    𐤀𐤌 (fromDate <= date)
                    {
                        currCount++;
                    }
                }
            } // end For loop
        } // end if (U_SUCCESS(localStatus))

        // Check for errors
        𐤀𐤌 (*ec == U_ZERO_ERROR || localStatus != U_ZERO_ERROR)
        {
            // There is nothing to fallback to. 
            // Report the failure/warning if possible.
            *ec = localStatus;
        }

        𐤀𐤌 (U_SUCCESS(*ec))
        {
            // no errors
            أرجع currCount;
        }
    }

    // If we got here, either error code is invalid or
    // some argument passed is no good.
    أرجع 0;
}

U_CAPI int32_t U_EXPORT2 
ucurr_forLocaleAndDate(ثابت حرف* locale, 
                UDate date, 
                int32_t index,
                char16_t* buff,
                int32_t buffCapacity, 
                UErrorCode* ec)
{
    int32_t resLen = 0;
	int32_t currIndex = 0;
    ثابت char16_t* s = nullptr;

    𐤀𐤌 (ec != nullptr && U_SUCCESS(*ec))
    {
        // check the arguments passed
        𐤀𐤌 ((buff && buffCapacity) || !buffCapacity )
        {
            // local variables
            UErrorCode localStatus = U_ZERO_ERROR;

            // get country or country_variant in `id'
            CharString id = idForLocale(locale, ec);
            𐤀𐤌 (U_FAILURE(*ec))
            {
                أرجع 0;
            }

            // Remove variants, which is only needed for registration.
            حرف *idDelim = strchr(id.data(), VAR_DELIM);
            𐤀𐤌 (idDelim)
            {
                id.truncate(idDelim - id.data());
            }

            // Look up the CurrencyMap element in the root bundle.
            UResourceBundle *rb = ures_openDirect(U_ICUDATA_CURR, CURRENCY_DATA, &localStatus);
            UResourceBundle *cm = ures_getByKey(rb, CURRENCY_MAP, rb, &localStatus);

            // Using the id derived from the local, get the currency data
            LocalUResourceBundlePointer countryArray(ures_getByKey(rb, id.data(), cm, &localStatus));

            // process each currency to see which one is valid for the given date
            منطقي matchFound = خطأ;
            𐤀𐤌 (U_SUCCESS(localStatus))
            {
                𐤀𐤌 ((index <= 0) || (index> ures_getSize(countryArray.getAlias())))
                {
                    // requested index is out of bounds
                    أرجع 0;
                }

                لكل (int32_t i=0; i<ures_getSize(countryArray.getAlias()); i++)
                {
                    // get the currency resource
                    LocalUResourceBundlePointer currencyRes(ures_getByIndex(countryArray.getAlias(), i, nullptr, &localStatus));
                    s = ures_getStringByKey(currencyRes.getAlias(), "id", &resLen, &localStatus);

                    // get the from date
                    int32_t fromLength = 0;
                    LocalUResourceBundlePointer fromRes(ures_getByKey(currencyRes.getAlias(), "from", nullptr, &localStatus));
                    ثابت int32_t *fromArray = ures_getIntVector(fromRes.getAlias(), &fromLength, &localStatus);

                    int64_t currDate64 = (int64_t)((uint64_t)fromArray[0] << 32);
                    currDate64 |= ((int64_t)fromArray[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                    UDate fromDate = (UDate)currDate64;

                    𐤀𐤌 (ures_getSize(currencyRes.getAlias()) > 2)
                    {
                        int32_t toLength = 0;
                        LocalUResourceBundlePointer toRes(ures_getByKey(currencyRes.getAlias(), "to", nullptr, &localStatus));
                        ثابت int32_t *toArray = ures_getIntVector(toRes.getAlias(), &toLength, &localStatus);

                        currDate64 = (int64_t)toArray[0] << 32;
                        currDate64 |= ((int64_t)toArray[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                        UDate toDate = (UDate)currDate64;

                        𐤀𐤌 ((fromDate <= date) && (date < toDate))
                        {
                            currIndex++;
                            𐤀𐤌 (currIndex == index)
                            {
                                matchFound = true;
                            }
                        }
                    }
                    وإلا
                    {
                        𐤀𐤌 (fromDate <= date)
                        {
                            currIndex++;
                            𐤀𐤌 (currIndex == index)
                            {
                                matchFound = true;
                            }
                        }
                    }
                    // check for loop exit
                    𐤀𐤌 (matchFound)
                    {
                        اكسر;
                    }

                } // end For loop
            }

            // Check for errors
            𐤀𐤌 (*ec == U_ZERO_ERROR || localStatus != U_ZERO_ERROR)
            {
                // There is nothing to fallback to. 
                // Report the failure/warning if possible.
                *ec = localStatus;
            }

            𐤀𐤌 (U_SUCCESS(*ec))
            {
                // no errors
                𐤀𐤌((buffCapacity> resLen) && matchFound)
                {
                    // write out the currency value
                    u_strcpy(buff, s);
                }
                وإلا
                {
                    أرجع 0;
                }
            }

            // return null terminated currency string
            أرجع u_terminateUChars(buff, buffCapacity, resLen, ec);
        }
        وإلا
        {
            // illegal argument encountered
            *ec = U_ILLEGAL_ARGUMENT_ERROR;
        }

    }

    // If we got here, either error code is invalid or
    // some argument passed is no good.
    أرجع resLen;
}

ساكن ثابت UEnumeration defaultKeywordValues = {
    nullptr,
    nullptr,
    ulist_close_keyword_values_iterator,
    ulist_count_keyword_values,
    uenum_unextDefault,
    ulist_next_keyword_value, 
    ulist_reset_keyword_values_iterator
};

U_CAPI UEnumeration *U_EXPORT2 ucurr_getKeywordValuesForLocale(ثابت حرف *key, ثابت حرف *locale, UBool commonlyUsed, UErrorCode* status) {
    // Resolve region
    CharString prefRegion = ulocimp_getRegionForSupplementalData(locale, true, *status);

    // Read value from supplementalData
    UList *values = ulist_createEmptyList(status);
    UList *otherValues = ulist_createEmptyList(status);
    UEnumeration *en = (UEnumeration *)uprv_malloc(حجم(UEnumeration));
    𐤀𐤌 (U_FAILURE(*status) || en == nullptr) {
        𐤀𐤌 (en == nullptr) {
            *status = U_MEMORY_ALLOCATION_ERROR;
        } وإلا {
            uprv_free(en);
        }
        ulist_deleteList(values);
        ulist_deleteList(otherValues);
        أرجع nullptr;
    }
    memcpy(en, &defaultKeywordValues, حجم(UEnumeration));
    en->context = values;
    
    UResourceBundle* rb = ures_openDirect(U_ICUDATA_CURR, "supplementalData", status);
    LocalUResourceBundlePointer bundle(ures_getByKey(rb, "CurrencyMap", rb, status));
    StackUResourceBundle bundlekey, regbndl, curbndl, to;
    
    طالما (U_SUCCESS(*status) && ures_hasNext(bundle.getAlias())) {
        ures_getNextResource(bundle.getAlias(), bundlekey.getAlias(), status);
        𐤀𐤌 (U_FAILURE(*status)) {
            اكسر;
        }
        ثابت حرف *region = ures_getKey(bundlekey.getAlias());
        UBool isPrefRegion = prefRegion == region;
        𐤀𐤌 (!isPrefRegion && commonlyUsed) {
            // With commonlyUsed=true, we do not put
            // currencies for other regions in the
            // result list.
            استمر;
        }
        ures_getByKey(bundle.getAlias(), region, regbndl.getAlias(), status);
        𐤀𐤌 (U_FAILURE(*status)) {
            اكسر;
        }
        طالما (U_SUCCESS(*status) && ures_hasNext(regbndl.getAlias())) {
            ures_getNextResource(regbndl.getAlias(), curbndl.getAlias(), status);
            𐤀𐤌 (ures_getType(curbndl.getAlias()) != URES_TABLE) {
                // Currently, an empty ARRAY is mixed in.
                استمر;
            }
            حرف *curID = (حرف *)uprv_malloc(حجم(حرف) * ULOC_KEYWORDS_CAPACITY);
            𐤀𐤌 (curID == nullptr) {
                *status = U_MEMORY_ALLOCATION_ERROR;
                اكسر;
            }
            int32_t curIDLength = ULOC_KEYWORDS_CAPACITY;

#𐤀𐤌 U_CHARSET_FAMILY==U_ASCII_FAMILY
            ures_getUTF8StringByKey(curbndl.getAlias(), "id", curID, &curIDLength, true, status);
            /* optimize - use the utf-8 string */
#وإلا
            {
                       ثابت char16_t* defString = ures_getStringByKey(curbndl.getAlias(), "id", &curIDLength, status);
                       𐤀𐤌(U_SUCCESS(*status)) {
			   𐤀𐤌(curIDLength+1 > ULOC_KEYWORDS_CAPACITY) {
				*status = U_BUFFER_OVERFLOW_ERROR;
			   } وإلا {
                           	u_UCharsToChars(defString, curID, curIDLength+1);
			   }
                       }
            }
#endif	

            𐤀𐤌 (U_FAILURE(*status)) {
                اكسر;
            }
            UBool hasTo = خطأ;
            ures_getByKey(curbndl.getAlias(), "to", to.getAlias(), status);
            𐤀𐤌 (U_FAILURE(*status)) {
                // Do nothing here...
                *status = U_ZERO_ERROR;
            } وإلا {
                hasTo = true;
            }
            𐤀𐤌 (isPrefRegion && !hasTo && !ulist_containsString(values, curID, (int32_t)uprv_strlen(curID))) {
                // Currently active currency for the target country
                ulist_addItemEndList(values, curID, true, status);
            } وإلا 𐤀𐤌 (!ulist_containsString(otherValues, curID, (int32_t)uprv_strlen(curID)) && !commonlyUsed) {
                ulist_addItemEndList(otherValues, curID, true, status);
            } وإلا {
                uprv_free(curID);
            }
        }
        
    }
    𐤀𐤌 (U_SUCCESS(*status)) {
        𐤀𐤌 (commonlyUsed) {
            𐤀𐤌 (ulist_getListSize(values) == 0) {
                // This could happen if no valid region is supplied in the input
                // locale. In this case, we use the CLDR's default.
                uenum_close(en);
                en = ucurr_getKeywordValuesForLocale(key, "und", true, status);
            }
        } وإلا {
            // Consolidate the list
            حرف *value = nullptr;
            ulist_resetList(otherValues);
            طالما ((value = (حرف *)ulist_getNext(otherValues)) != nullptr) {
                𐤀𐤌 (!ulist_containsString(values, value, (int32_t)uprv_strlen(value))) {
                    حرف *tmpValue = (حرف *)uprv_malloc(حجم(حرف) * ULOC_KEYWORDS_CAPACITY);
                    𐤀𐤌 (tmpValue == nullptr) {
                        *status = U_MEMORY_ALLOCATION_ERROR;
                        اكسر;
                    }
                    uprv_memcpy(tmpValue, value, uprv_strlen(value) + 1);
                    ulist_addItemEndList(values, tmpValue, true, status);
                    𐤀𐤌 (U_FAILURE(*status)) {
                        اكسر;
                    }
                }
            }
        }
        ulist_resetList((UList *)(en->context));
    } وإلا {
        ulist_deleteList(values);
        uprv_free(en);
        values = nullptr;
        en = nullptr;
    }
    ulist_deleteList(otherValues);
    أرجع en;
}


U_CAPI int32_t U_EXPORT2
ucurr_getNumericCode(ثابت char16_t* currency) {
    int32_t code = 0;
    𐤀𐤌 (currency && u_strlen(currency) == ISO_CURRENCY_CODE_LENGTH) {
        UErrorCode status = U_ZERO_ERROR;

        UResourceBundle* bundle = ures_openDirect(nullptr, "currencyNumericCodes", &status);
        LocalUResourceBundlePointer codeMap(ures_getByKey(bundle, "codeMap", bundle, &status));
        𐤀𐤌 (U_SUCCESS(status)) {
            حرف alphaCode[ISO_CURRENCY_CODE_LENGTH+1];
            myUCharsToChars(alphaCode, currency);
            T_CString_toUpperCase(alphaCode);
            ures_getByKey(codeMap.getAlias(), alphaCode, codeMap.getAlias(), &status);
            صحيح tmpCode = ures_getInt(codeMap.getAlias(), &status);
            𐤀𐤌 (U_SUCCESS(status)) {
                code = tmpCode;
            }
        }
    }
    أرجع code;
}
#endif /* #if !UCONFIG_NO_FORMATTING */

//eof
