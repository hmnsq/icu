// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
* Copyright (c) 2002-2016, International Business Machines
* Corporation and others.  All Rights Reserved.
**********************************************************************
*/

#অন্তর্ভাব "unicode/utypes.h"

#যদি !UCONFIG_NO_FORMATTING

#অন্তর্ভাব <utility>

#অন্তর্ভাব "unicode/ucurr.h"
#অন্তর্ভাব "unicode/locid.h"
#অন্তর্ভাব "unicode/ures.h"
#অন্তর্ভাব "unicode/ustring.h"
#অন্তর্ভাব "unicode/parsepos.h"
#অন্তর্ভাব "unicode/uniset.h"
#অন্তর্ভাব "unicode/usetiter.h"
#অন্তর্ভাব "unicode/utf16.h"
#অন্তর্ভাব "ustr_imp.h"
#অন্তর্ভাব "charstr.h"
#অন্তর্ভাব "cmemory.h"
#অন্তর্ভাব "cstring.h"
#অন্তর্ভাব "static_unicode_sets.h"
#অন্তর্ভাব "uassert.h"
#অন্তর্ভাব "umutex.h"
#অন্তর্ভাব "ucln_cmn.h"
#অন্তর্ভাব "uenumimp.h"
#অন্তর্ভাব "uhash.h"
#অন্তর্ভাব "hash.h"
#অন্তর্ভাব "uinvchar.h"
#অন্তর্ভাব "uresimp.h"
#অন্তর্ভাব "ulist.h"
#অন্তর্ভাব "uresimp.h"
#অন্তর্ভাব "ureslocs.h"
#অন্তর্ভাব "ulocimp.h"

using নামাকাশ icu;

//#define UCURR_DEBUG_EQUIV 1
#ifdef UCURR_DEBUG_EQUIV
#অন্তর্ভাব "stdio.h"
#endif
//#define UCURR_DEBUG 1
#ifdef UCURR_DEBUG
#অন্তর্ভাব "stdio.h"
#endif

প্রকারলেখ কাঠামো IsoCodeEntry {
    ধ্রুবক char16_t *isoCode; /* const because it's a reference to a resource bundle string. */
    UDate from;
    UDate to;
} IsoCodeEntry;

//------------------------------------------------------------
// Constants

// Default currency meta data of last resort.  We try to use the
// defaults encoded in the meta data resource bundle.  If there is a
// configuration/build error and these are not available, we use these
// hard-coded defaults (which should be identical).
স্থির ধ্রুবক int32_t LAST_RESORT_তথ্য[] = { 2, 0, 2, 0 };

// POW10[i] = 10^i, i=0..MAX_POW10
স্থির ধ্রুবক int32_t POW10[] = { 1, 10, 100, 1000, 10000, 100000,
                                 1000000, 10000000, 100000000, 1000000000 };

স্থির ধ্রুবক int32_t MAX_POW10 = UPRV_LENGTHOF(POW10) - 1;

#define ISO_CURRENCY_CODE_দৈর্ঘ্য 3

//------------------------------------------------------------
// Resource tags
//

স্থির ধ্রুবক অক্ষর CURRENCY_তথ্য[] = "supplementalData";
// Tag for meta-data, in root.
স্থির ধ্রুবক অক্ষর CURRENCY_META[] = "CurrencyMeta";

// Tag for map from countries to currencies, in root.
স্থির ধ্রুবক অক্ষর CURRENCY_MAP[] = "CurrencyMap";

// Tag for default meta-data, in CURRENCY_META
স্থির ধ্রুবক অক্ষর DEFAULT_META[] = "DEFAULT";

// Variant delimiter
স্থির ধ্রুবক অক্ষর VAR_DELIM = '_';

// Tag for localized display names (symbols) of currencies
স্থির ধ্রুবক অক্ষর CURRENCIES[] = "Currencies";
স্থির ধ্রুবক অক্ষর CURRENCIES_NARROW[] = "Currencies%narrow";
স্থির ধ্রুবক অক্ষর CURRENCIES_FORMAL[] = "Currencies%formal";
স্থির ধ্রুবক অক্ষর CURRENCIES_VARIANT[] = "Currencies%variant";
স্থির ধ্রুবক অক্ষর CURRENCYPLURALS[] = "CurrencyPlurals";

// ISO codes mapping table
স্থির ধ্রুবক UHashtable* gIsoCodes = nullptr;
স্থির icu::UInitOnce gIsoCodesInitOnce {};

// Currency symbol equivalances
স্থির ধ্রুবক icu::Hashtable* gCurrSymbolsEquiv = nullptr;
স্থির icu::UInitOnce gCurrSymbolsEquivInitOnce {};

U_NAMESPACE_BEGIN

// EquivIterator iterates over all strings that are equivalent to a given
// string, s. Note that EquivIterator will never yield s itself.
শ্রেণী EquivIterator : সর্বজনীন icu::UMemory {
সর্বজনীন:
    // Constructor. hash stores the equivalence relationships; s is the string
    // for which we find equivalent strings.
    অন্তঃরেখ EquivIterator(ধ্রুবক icu::Hashtable& hash, ধ্রুবক icu::UnicodeString& s)
        : _hash(hash) { 
        _start = _current = &s;
    }
    অন্তঃরেখ ~EquivIterator() { }

    // next returns the next equivalent string or nullptr if there are no more.
    // If s has no equivalent strings, next returns nullptr on the first call.
    ধ্রুবক icu::UnicodeString *next();
ব্যক্তিগত:
    ধ্রুবক icu::Hashtable& _hash;
    ধ্রুবক icu::UnicodeString* _start;
    ধ্রুবক icu::UnicodeString* _current;
};

ধ্রুবক icu::UnicodeString *
EquivIterator::next() {
    ধ্রুবক icu::UnicodeString* পরবর্তী_2 = static_cast<ধ্রুবক icu::UnicodeString*>(_hash.get(*_current));
    যদি (পরবর্তী_2 == nullptr) {
        U_ASSERT(_current == _start);
        ফেরত nullptr;
    }
    যদি (*পরবর্তী_2 == *_start) {
        ফেরত nullptr;
    }
    _current = পরবর্তী_2;
    ফেরত পরবর্তী_2;
}

U_NAMESPACE_END

// makeEquivalent makes lhs and rhs equivalent by updating the equivalence
// relations in hash accordingly.
স্থির শূন্য makeEquivalent(
    ধ্রুবক icu::UnicodeString &lhs,
    ধ্রুবক icu::UnicodeString &rhs,
    icu::Hashtable* hash, UErrorCode &status) {
    যদি (U_FAILURE(status)) {
        ফেরত;
    }
    যদি (lhs == rhs) {
        // already equivalent
        ফেরত;
    }
    icu::EquivIterator বাম_Iter(*hash, lhs);
    icu::EquivIterator ডান_Iter(*hash, rhs);
    ধ্রুবক icu::UnicodeString *first_বাম = বাম_Iter.next();
    ধ্রুবক icu::UnicodeString *first_ডান = ডান_Iter.next();
    ধ্রুবক icu::UnicodeString *পরবর্তী_বাম = first_বাম;
    ধ্রুবক icu::UnicodeString *পরবর্তী_ডান = first_ডান;
    যখন (পরবর্তী_বাম != nullptr && পরবর্তী_ডান != nullptr) {
        যদি (*পরবর্তী_বাম == rhs || *পরবর্তী_ডান == lhs) {
            // Already equivalent
            ফেরত;
        }
        পরবর্তী_বাম = বাম_Iter.next();
        পরবর্তী_ডান = ডান_Iter.next();
    }
    // Not equivalent. Must join.
    icu::UnicodeString *new_First_বাম;
    icu::UnicodeString *new_First_ডান;
    যদি (first_ডান == nullptr && first_বাম == nullptr) {
        // Neither lhs or rhs belong to an equivalence circle, so we form
        // a new equivalnce circle of just lhs and rhs.
        new_First_বাম = নতুন icu::UnicodeString(rhs);
        new_First_ডান = নতুন icu::UnicodeString(lhs);
    } অন্যথা যদি (first_ডান == nullptr) {
        // lhs belongs to an equivalence circle, but rhs does not, so we link
        // rhs into lhs' circle.
        new_First_বাম = নতুন icu::UnicodeString(rhs);
        new_First_ডান = নতুন icu::UnicodeString(*first_বাম);
    } অন্যথা যদি (first_বাম == nullptr) {
        // rhs belongs to an equivlance circle, but lhs does not, so we link
        // lhs into rhs' circle.
        new_First_বাম = নতুন icu::UnicodeString(*first_ডান);
        new_First_ডান = নতুন icu::UnicodeString(lhs);
    } অন্যথা {
        // Both lhs and rhs belong to different equivalnce circles. We link
        // them together to form one single, larger equivalnce circle.
        new_First_বাম = নতুন icu::UnicodeString(*first_ডান);
        new_First_ডান = নতুন icu::UnicodeString(*first_বাম);
    }
    যদি (new_First_বাম == nullptr || new_First_ডান == nullptr) {
        delete new_First_বাম;
        delete new_First_ডান;
        status = U_MEMORY_ALLOCATION_ERROR;
        ফেরত;
    }
    hash->put(lhs, (শূন্য *) new_First_বাম, status);
    hash->put(rhs, (শূন্য *) new_First_ডান, status);
}

// countEquivalent counts how many strings are equivalent to s.
// hash stores all the equivalnce relations.
// countEquivalent does not include s itself in the count.
স্থির int32_t গুনতি_Equivalent(ধ্রুবক icu::Hashtable &hash, ধ্রুবক icu::UnicodeString &s) {
    int32_t ফলাফল = 0;
    icu::EquivIterator iter(hash, s);
    যখন (iter.next() != nullptr) {
        ++ফলাফল;
    }
#ifdef UCURR_DEBUG_EQUIV
 {
   অক্ষর tmp[200];
   s.extract(0,s.length(),tmp, "UTF-8");
   printf("CountEquivalent('%s') = %d\n", tmp, ফলাফল);
 }
#endif
    ফেরত ফলাফল;
}

স্থির ধ্রুবক icu::Hashtable* getCurrSymbolsEquiv();

//------------------------------------------------------------
// Code

/**
 * Cleanup callback func
 */
স্থির UBool U_CALLCONV 
isoCodes_cleanup()
{
    যদি (gIsoCodes != nullptr) {
        uhash_close(const_cast<UHashtable *>(gIsoCodes));
        gIsoCodes = nullptr;
    }
    gIsoCodesInitOnce.reset();
    ফেরত সত্য;
}

/**
 * Cleanup callback func
 */
স্থির UBool U_CALLCONV 
currSymbolsEquiv_cleanup()
{
    delete const_cast<icu::Hashtable *>(gCurrSymbolsEquiv);
    gCurrSymbolsEquiv = nullptr;
    gCurrSymbolsEquivInitOnce.reset();
    ফেরত সত্য;
}

/**
 * Deleter for IsoCodeEntry
 */
স্থির শূন্য U_CALLCONV
deleteIsoCodeEntry(শূন্য *obj) {
    IsoCodeEntry* entry = static_cast<IsoCodeEntry*>(obj);
    uprv_free(entry);
}

/**
 * Deleter for gCurrSymbolsEquiv.
 */
স্থির শূন্য U_CALLCONV
deleteUnicode(শূন্য *obj) {
    icu::UnicodeString* entry = static_cast<icu::UnicodeString*>(obj);
    delete entry;
}

/**
 * Unfortunately, we have to convert the char16_t* currency code to char*
 * to use it as a resource key.
 */
স্থির অন্তঃরেখ অক্ষর*
myUCharsToChars(অক্ষর* ফলাফল_Of_Len4, ধ্রুবক char16_t* currency) {
    u_UCharsToChars(currency, ফলাফল_Of_Len4, ISO_CURRENCY_CODE_দৈর্ঘ্য);
    ফলাফল_Of_Len4[ISO_CURRENCY_CODE_দৈর্ঘ্য] = 0;
    ফেরত ফলাফল_Of_Len4;
}

/**
 * Internal function to look up currency data.  Result is an array of
 * four integers.  The first is the fraction digits.  The second is the
 * rounding increment, or 0 if none.  The rounding increment is in
 * units of 10^(-fraction_digits).  The third and fourth are the same
 * except that they are those used in cash transactions ( cashDigits
 * and cashRounding ).
 */
স্থির ধ্রুবক int32_t*
find_Meta_তথ্য(ধ্রুবক char16_t* currency, UErrorCode& ec) {

    যদি (currency == nullptr || *currency == 0) {
        যদি (U_SUCCESS(ec)) {
            ec = U_ILLEGAL_ARGUMENT_ERROR;
        }
        ফেরত LAST_RESORT_তথ্য;
    }

    // Get CurrencyMeta resource out of root locale file.  [This may
    // move out of the root locale file later; if it does, update this
    // code.]
    UResourceBundle* currency_তথ্য = ures_openDirect(U_ICUDATA_CURR, CURRENCY_তথ্য, &ec);
    LocalUResourceBundlePointer currencyMeta(ures_get_By_চাবি(currency_তথ্য, CURRENCY_META, currency_তথ্য, &ec));

    যদি (U_FAILURE(ec)) {
        // Config/build error; return hard-coded defaults
        ফেরত LAST_RESORT_তথ্য;
    }

    // Look up our currency, or if that's not available, then DEFAULT
    অক্ষর buf[ISO_CURRENCY_CODE_দৈর্ঘ্য+1];
    UErrorCode ec2 = U_ZERO_ERROR; // local error code: soft failure
    LocalUResourceBundlePointer rb(ures_get_By_চাবি(currencyMeta.getAlias(), myUCharsToChars(buf, currency), nullptr, &ec2));
      যদি (U_FAILURE(ec2)) {
        rb.adoptInstead(ures_get_By_চাবি(currencyMeta.getAlias(),DEFAULT_META, nullptr, &ec));
        যদি (U_FAILURE(ec)) {
            // Config/build error; return hard-coded defaults
            ফেরত LAST_RESORT_তথ্য;
        }
    }

    int32_t len;
    ধ্রুবক int32_t *তথ্য = ures_getIntVector(rb.getAlias(), &len, &ec);
    যদি (U_FAILURE(ec) || len != 4) {
        // Config/build error; return hard-coded defaults
        যদি (U_SUCCESS(ec)) {
            ec = U_INVALID_FORMAT_ERROR;
        }
        ফেরত LAST_RESORT_তথ্য;
    }

    ফেরত তথ্য;
}

// -------------------------------------

স্থির CharString
idForLocale(ধ্রুবক অক্ষর* locale, UErrorCode* ec)
{
    ফেরত ulocimp_get_Region_For_Supplemental_তথ্য(locale, মিথ্যা, *ec);
}

// ------------------------------------------
//
// Registration
//
//-------------------------------------------

// don't use ICUService since we don't need fallback

U_CDECL_BEGIN
স্থির UBool U_CALLCONV currency_cleanup();
U_CDECL_END

#যদি !UCONFIG_NO_SERVICE
কাঠামো CReg;

স্থির UMutex gCRegLock;
স্থির CReg* g_CReg_শীর্ষ = nullptr;

কাঠামো CReg : সর্বজনীন icu::UMemory {
    CReg *next;
    char16_t iso[ISO_CURRENCY_CODE_দৈর্ঘ্য+1];
    অক্ষর  id[ULOC_FULLNAME_CAPACITY];

    CReg(ধ্রুবক char16_t* _iso, ধ্রুবক অক্ষর* _id)
        : next(nullptr)
    {
        uprv_strncpy(id, _id, আকার(id)-1);
        id[আকার(id)-1] = 0;
        u_memcpy(iso, _iso, ISO_CURRENCY_CODE_দৈর্ঘ্য);
        iso[ISO_CURRENCY_CODE_দৈর্ঘ্য] = 0;
    }

    স্থির UCurr_Registry_চাবি reg(ধ্রুবক char16_t* _iso, ধ্রুবক অক্ষর* _id, UErrorCode* status)
    {
        যদি (status && U_SUCCESS(*status) && _iso && _id) {
            CReg* n = নতুন CReg(_iso, _id);
            যদি (n) {
                umtx_lock(&gCRegLock);
                যদি (!g_CReg_শীর্ষ) {
                    /* register for the first time */
                    ucln_common_registerCleanup(UCLN_COMMON_CURRENCY, currency_cleanup);
                }
                n->next = g_CReg_শীর্ষ;
                g_CReg_শীর্ষ = n;
                umtx_unlock(&gCRegLock);
                ফেরত n;
            }
            *status = U_MEMORY_ALLOCATION_ERROR;
        }
        ফেরত nullptr;
    }

    স্থির UBool unreg(UCurr_Registry_চাবি চাবি) {
        UBool found = মিথ্যা;
        umtx_lock(&gCRegLock);

        CReg** p = &g_CReg_শীর্ষ;
        যখন (*p) {
            যদি (*p == চাবি) {
                *p = ((CReg*)চাবি)->next;
                delete (CReg*)চাবি;
                found = সত্য;
                বিরতি;
            }
            p = &((*p)->next);
        }

        umtx_unlock(&gCRegLock);
        ফেরত found;
    }

    স্থির ধ্রুবক char16_t* get(ধ্রুবক অক্ষর* id) {
        ধ্রুবক char16_t* ফলাফল = nullptr;
        umtx_lock(&gCRegLock);
        CReg* p = g_CReg_শীর্ষ;

        /* register cleanup of the mutex */
        ucln_common_registerCleanup(UCLN_COMMON_CURRENCY, currency_cleanup);
        যখন (p) {
            যদি (uprv_strcmp(id, p->id) == 0) {
                ফলাফল = p->iso;
                বিরতি;
            }
            p = p->next;
        }
        umtx_unlock(&gCRegLock);
        ফেরত ফলাফল;
    }

    /* This doesn't need to be thread safe. It's for u_cleanup only. */
    স্থির শূন্য cleanup() {
        যখন (g_CReg_শীর্ষ) {
            CReg* n = g_CReg_শীর্ষ;
            g_CReg_শীর্ষ = g_CReg_শীর্ষ->next;
            delete n;
        }
    }
};

// -------------------------------------

U_CAPI UCurr_Registry_চাবি U_EXPORT2
ucurr_register(ধ্রুবক char16_t* isoCode, ধ্রুবক অক্ষর* locale, UErrorCode *status)
{
    যদি (status && U_SUCCESS(*status)) {
        CharString id = idForLocale(locale, status);
        ফেরত CReg::reg(isoCode, id.তথ্য(), status);
    }
    ফেরত nullptr;
}

// -------------------------------------

U_CAPI UBool U_EXPORT2
ucurr_unregister(UCurr_Registry_চাবি চাবি, UErrorCode* status)
{
    যদি (status && U_SUCCESS(*status)) {
        ফেরত CReg::unreg(চাবি);
    }
    ফেরত মিথ্যা;
}
#endif /* UCONFIG_NO_SERVICE */

// -------------------------------------

/**
 * Release all static memory held by currency.
 */
/*The declaration here is needed so currency_cleanup()
 * can call this function.
 */
স্থির UBool U_CALLCONV
currency_cache_cleanup();

U_CDECL_BEGIN
স্থির UBool U_CALLCONV currency_cleanup() {
#যদি !UCONFIG_NO_SERVICE
    CReg::cleanup();
#endif
    /*
     * There might be some cached currency data or isoCodes data.
     */
    currency_cache_cleanup();
    isoCodes_cleanup();
    currSymbolsEquiv_cleanup();

    ফেরত সত্য;
}
U_CDECL_END

// -------------------------------------

U_CAPI int32_t U_EXPORT2
ucurr_forLocale(ধ্রুবক অক্ষর* locale,
                char16_t* buff,
                int32_t buffCapacity,
                UErrorCode* ec) {
    যদি (U_FAILURE(*ec)) { ফেরত 0; }
    যদি (buffCapacity < 0 || (buff == nullptr && buffCapacity > 0)) {
        *ec = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত 0;
    }

    UErrorCode localStatus = U_ZERO_ERROR;
    CharString currency = ulocimp_get_Keyword_মান(locale, "currency", localStatus);
    int32_t resLen = currency.length();

    যদি (U_SUCCESS(localStatus) && resLen == 3 && uprv_isInvariantString(currency.তথ্য(), resLen)) {
        যদি (resLen < buffCapacity) {
            T_CString_toUpperCase(currency.তথ্য());
            u_charsToUChars(currency.তথ্য(), buff, resLen);
        }
        ফেরত u_terminateUChars(buff, buffCapacity, resLen, ec);
    }

    // get country or country_variant in `id'
    CharString id = idForLocale(locale, ec);
    যদি (U_FAILURE(*ec)) {
        ফেরত 0;
    }

#যদি !UCONFIG_NO_SERVICE
    ধ্রুবক char16_t* ফলাফল = CReg::get(id.তথ্য());
    যদি (ফলাফল) {
        যদি(buffCapacity > u_strlen(ফলাফল)) {
            u_strcpy(buff, ফলাফল);
        }
        resLen = u_strlen(ফলাফল);
        ফেরত u_terminateUChars(buff, buffCapacity, resLen, ec);
    }
#endif
    // Remove variants, which is only needed for registration.
    অক্ষর *idDelim = uprv_strchr(id.তথ্য(), VAR_DELIM);
    যদি (idDelim) {
        id.truncate(idDelim - id.তথ্য());
    }

    ধ্রুবক char16_t* s = nullptr;  // Currency code from data file.
    যদি (id.isEmpty()) {
        // No point looking in the data for an empty string.
        // This is what we would get.
        localStatus = U_MISSING_RESOURCE_ERROR;
    } অন্যথা {
        // Look up the CurrencyMap element in the root bundle.
        localStatus = U_ZERO_ERROR;
        UResourceBundle *rb = ures_openDirect(U_ICUDATA_CURR, CURRENCY_তথ্য, &localStatus);
        UResourceBundle *cm = ures_get_By_চাবি(rb, CURRENCY_MAP, rb, &localStatus);
        LocalUResourceBundlePointer countryArray(ures_get_By_চাবি(rb, id.তথ্য(), cm, &localStatus));
        // https://unicode-org.atlassian.net/browse/ICU-21997
        // Prefer to use currencies that are legal tender.
        যদি (U_SUCCESS(localStatus)) {
            int32_t array_দৈর্ঘ্য = ures_get_আকার(countryArray.getAlias());
            জন্য (int32_t i = 0; i < array_দৈর্ঘ্য; ++i) {
                LocalUResourceBundlePointer currencyReq(
                    ures_get_By_সূচক(countryArray.getAlias(), i, nullptr, &localStatus));
                // The currency is legal tender if it is *not* marked with tender{"false"}.
                UErrorCode tenderStatus = localStatus;
                ধ্রুবক char16_t *tender =
                    ures_get_String_By_চাবি(currencyReq.getAlias(), "tender", nullptr, &tenderStatus);
                যৌক্তিক isTender = U_FAILURE(tenderStatus) || u_strcmp(tender, u"false") != 0;
                যদি (!isTender && s != nullptr) {
                    // We already have a non-tender currency. Ignore all following non-tender ones.
                    চালু;
                }
                // Fetch the currency code.
                s = ures_get_String_By_চাবি(currencyReq.getAlias(), "id", &resLen, &localStatus);
                যদি (isTender) {
                    বিরতি;
                }
            }
            যদি (U_SUCCESS(localStatus) && s == nullptr) {
                localStatus = U_MISSING_RESOURCE_ERROR;
            }
        }
    }

    যদি ((U_FAILURE(localStatus)) && strchr(id.তথ্য(), '_') != nullptr) {
        // We don't know about it.  Check to see if we support the variant.
        CharString parent = ulocimp_getParent(locale, *ec);
        *ec = U_USING_FALLBACK_WARNING;
        // TODO: Loop over the parent rather than recursing and
        // looking again for a currency keyword.
        ফেরত ucurr_forLocale(parent.তথ্য(), buff, buffCapacity, ec);
    }
    যদি (*ec == U_ZERO_ERROR || localStatus != U_ZERO_ERROR) {
        // There is nothing to fallback to. Report the failure/warning if possible.
        *ec = localStatus;
    }
    যদি (U_SUCCESS(*ec)) {
        যদি(buffCapacity > resLen) {
            u_strcpy(buff, s);
        }
    }
    ফেরত u_terminateUChars(buff, buffCapacity, resLen, ec);
}

// end registration

/**
 * Modify the given locale name by removing the rightmost _-delimited
 * element.  If there is none, empty the string ("" == root).
 * NOTE: The string "root" is not recognized; do not use it.
 * @return true if the fallback happened; false if locale is already
 * root ("").
 */
স্থির UBool fallback(CharString& loc) {
    যদি (loc.isEmpty()) {
        ফেরত মিথ্যা;
    }
    UErrorCode status = U_ZERO_ERROR;
    যদি (loc == "en_GB") {
        // HACK: See #13368.  We need "en_GB" to fall back to "en_001" instead of "en"
        // in order to consume the correct data strings.  This hack will be removed
        // when proper data sink loading is implemented here.
        loc.truncate(3);
        loc.append("001", status);
    } অন্যথা {
        loc = ulocimp_getParent(loc.তথ্য(), status);
    }
 /*
    char *i = uprv_strrchr(loc, '_');
    if (i == nullptr) {
        i = loc;
    }
    *i = 0;
 */
    ফেরত সত্য;
}


U_CAPI ধ্রুবক char16_t* U_EXPORT2
ucurr_getName(ধ্রুবক char16_t* currency U_LIFETIME_BOUND,
              ধ্রুবক অক্ষর* locale,
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

    যদি (U_FAILURE(*ec)) {
        ফেরত nullptr;
    }

    int32_t choice = (int32_t) nameStyle;
    যদি (choice < 0 || choice > 4) {
        *ec = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত nullptr;
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

    যদি (locale == nullptr) {
        locale = uloc_getDefault();
    }
    CharString loc = ulocimp_getName(locale, ec2);
    যদি (U_FAILURE(ec2)) {
        *ec = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত nullptr;
    }

    অক্ষর buf[ISO_CURRENCY_CODE_দৈর্ঘ্য+1];
    myUCharsToChars(buf, currency);
    
    /* Normalize the keyword value to uppercase */
    T_CString_toUpperCase(buf);
    
    ধ্রুবক char16_t* s = nullptr;
    ec2 = U_ZERO_ERROR;
    LocalUResourceBundlePointer rb(ures_open(U_ICUDATA_CURR, loc.তথ্য(), &ec2));

    যদি (nameStyle == UCURR_NARROW_প্রতীক_NAME || nameStyle == UCURR_FORMAL_প্রতীক_NAME || nameStyle == UCURR_VARIANT_প্রতীক_NAME) {
        CharString চাবি;
        নির্বাচন (nameStyle) {
        ক্ষেত্র UCURR_NARROW_প্রতীক_NAME:
            চাবি.append(CURRENCIES_NARROW, ec2);
            বিরতি;
        ক্ষেত্র UCURR_FORMAL_প্রতীক_NAME:
            চাবি.append(CURRENCIES_FORMAL, ec2);
            বিরতি;
        ক্ষেত্র UCURR_VARIANT_প্রতীক_NAME:
            চাবি.append(CURRENCIES_VARIANT, ec2);
            বিরতি;
        পূর্বনির্ধারিত:
            *ec = U_UNSUPPORTED_ERROR;
            ফেরত nullptr;
        }
        চাবি.append("/", ec2);
        চাবি.append(buf, ec2);
        s = ures_get_String_By_চাবি_With_Fallback(rb.getAlias(), চাবি.তথ্য(), len, &ec2);
        যদি (ec2 == U_MISSING_RESOURCE_ERROR) {
            *ec = U_USING_FALLBACK_WARNING;
            ec2 = U_ZERO_ERROR;
            choice = UCURR_প্রতীক_NAME;
        }
    }
    যদি (s == nullptr) {
        ures_get_By_চাবি(rb.getAlias(), CURRENCIES, rb.getAlias(), &ec2);
        ures_get_By_চাবি_With_Fallback(rb.getAlias(), buf, rb.getAlias(), &ec2);
        s = ures_get_String_By_সূচক(rb.getAlias(), choice, len, &ec2);
    }

    // If we've succeeded we're done.  Otherwise, try to fallback.
    // If that fails (because we are already at root) then exit.
    যদি (U_SUCCESS(ec2)) {
        যদি (ec2 == U_USING_DEFAULT_WARNING
            || (ec2 == U_USING_FALLBACK_WARNING && *ec != U_USING_DEFAULT_WARNING)) {
            *ec = ec2;
        }
    }

    // We no longer support choice format data in names.  Data should not contain
    // choice patterns.
    যদি (isChoiceFormat != nullptr) {
        *isChoiceFormat = মিথ্যা;
    }
    যদি (U_SUCCESS(ec2)) {
        U_ASSERT(s != nullptr);
        ফেরত s;
    }

    // If we fail to find a match, use the ISO 4217 code
    *len = u_strlen(currency); // Should == ISO_CURRENCY_CODE_LENGTH, but maybe not...?
    *ec = U_USING_DEFAULT_WARNING;
    ফেরত currency;
}

U_CAPI ধ্রুবক char16_t* U_EXPORT2
ucurr_getPluralName(ধ্রুবক char16_t* currency U_LIFETIME_BOUND,
                    ধ্রুবক অক্ষর* locale,
                    UBool* isChoiceFormat,
                    ধ্রুবক অক্ষর* plural_গুনতি,
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

    যদি (U_FAILURE(*ec)) {
        ফেরত nullptr;
    }

    // Use a separate UErrorCode here that does not propagate out of
    // this function.
    UErrorCode ec2 = U_ZERO_ERROR;

    যদি (locale == nullptr) {
        locale = uloc_getDefault();
    }
    CharString loc = ulocimp_getName(locale, ec2);
    যদি (U_FAILURE(ec2)) {
        *ec = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত nullptr;
    }

    অক্ষর buf[ISO_CURRENCY_CODE_দৈর্ঘ্য+1];
    myUCharsToChars(buf, currency);

    ধ্রুবক char16_t* s = nullptr;
    ec2 = U_ZERO_ERROR;
    UResourceBundle* rb = ures_open(U_ICUDATA_CURR, loc.তথ্য(), &ec2);

    rb = ures_get_By_চাবি(rb, CURRENCYPLURALS, rb, &ec2);

    // Fetch resource with multi-level resource inheritance fallback
    LocalUResourceBundlePointer curr(ures_get_By_চাবি_With_Fallback(rb, buf, rb, &ec2));

    s = ures_get_String_By_চাবি_With_Fallback(curr.getAlias(), plural_গুনতি, len, &ec2);
    যদি (U_FAILURE(ec2)) {
        //  fall back to "other"
        ec2 = U_ZERO_ERROR;
        s = ures_get_String_By_চাবি_With_Fallback(curr.getAlias(), "other", len, &ec2);     
        যদি (U_FAILURE(ec2)) {
            // fall back to long name in Currencies
            ফেরত ucurr_getName(currency, locale, UCURR_LONG_NAME, 
                                 isChoiceFormat, len, ec);
        }
    }

    // If we've succeeded we're done.  Otherwise, try to fallback.
    // If that fails (because we are already at root) then exit.
    যদি (U_SUCCESS(ec2)) {
        যদি (ec2 == U_USING_DEFAULT_WARNING
            || (ec2 == U_USING_FALLBACK_WARNING && *ec != U_USING_DEFAULT_WARNING)) {
            *ec = ec2;
        }
        U_ASSERT(s != nullptr);
        ফেরত s;
    }

    // If we fail to find a match, use the ISO 4217 code
    *len = u_strlen(currency); // Should == ISO_CURRENCY_CODE_LENGTH, but maybe not...?
    *ec = U_USING_DEFAULT_WARNING;
    ফেরত currency;
}


//========================================================================
// Following are structure and function for parsing currency names

#define NEED_TO_BE_DELETED 0x1

// TODO: a better way to define this?
#define MAX_CURRENCY_NAME_LEN 100

প্রকারলেখ কাঠামো {
    ধ্রুবক অক্ষর* IsoCode;  //চাবি
    char16_t* currencyName;  //মান
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
স্থির পূর্ণসংখ্যা U_CALLCONV currencyNameComparator(ধ্রুবক শূন্য* a, ধ্রুবক শূন্য* b) {
    ধ্রুবক CurrencyNameStruct* currName_1 = static_cast<ধ্রুবক CurrencyNameStruct*>(a);
    ধ্রুবক CurrencyNameStruct* currName_2 = static_cast<ধ্রুবক CurrencyNameStruct*>(b);
    জন্য (int32_t i = 0; 
         i < MIN(currName_1->currencyNameLen, currName_2->currencyNameLen);
         ++i) {
        যদি (currName_1->currencyName[i] < currName_2->currencyName[i]) {
            ফেরত -1;
        }
        যদি (currName_1->currencyName[i] > currName_2->currencyName[i]) {
            ফেরত 1;
        }
    }
    যদি (currName_1->currencyNameLen < currName_2->currencyNameLen) {
        ফেরত -1;
    } অন্যথা যদি (currName_1->currencyNameLen > currName_2->currencyNameLen) {
        ফেরত 1;
    }
    ফেরত 0;
}


// Give a locale, return the maximum number of currency names associated with
// this locale.
// It gets currency names from resource bundles using fallback.
// It is the maximum number because in the fallback chain, some of the 
// currency names are duplicated.
// For example, given locale as "en_US", the currency names get from resource
// bundle in "en_US" and "en" are duplicated. The fallback mechanism will count
// all currency names in "en_US" and "en".
স্থির শূন্য
get_Currency_Name_গুনতি(ধ্রুবক অক্ষর* loc, int32_t* যোগ_currency_name_গুনতি, int32_t* যোগ_currency_প্রতীক_গুনতি) {
    U_NAMESPACE_USE
    *যোগ_currency_name_গুনতি = 0;
    *যোগ_currency_প্রতীক_গুনতি = 0;
    ধ্রুবক char16_t* s = nullptr;
    CharString locale;
    {
        UErrorCode status = U_ZERO_ERROR;
        locale.append(loc, status);
        যদি (U_FAILURE(status)) { ফেরত; }
    }
    ধ্রুবক icu::Hashtable *currencySymbolsEquiv = getCurrSymbolsEquiv();
    জন্য (;;) {
        UErrorCode ec2 = U_ZERO_ERROR;
        // TODO: ures_openDirect?
        LocalUResourceBundlePointer rb(ures_open(U_ICUDATA_CURR, locale.তথ্য(), &ec2));
        LocalUResourceBundlePointer curr(ures_get_By_চাবি(rb.getAlias(), CURRENCIES, nullptr, &ec2));
        int32_t n = ures_get_আকার(curr.getAlias());
        জন্য (int32_t i=0; i<n; ++i) {
            LocalUResourceBundlePointer names(ures_get_By_সূচক(curr.getAlias(), i, nullptr, &ec2));
            int32_t len;
            s = ures_get_String_By_সূচক(names.getAlias(), UCURR_প্রতীক_NAME, &len, &ec2);
            ++(*যোগ_currency_প্রতীক_গুনতি);  // currency symbol
            যদি (currencySymbolsEquiv != nullptr) {
                *যোগ_currency_প্রতীক_গুনতি += গুনতি_Equivalent(*currencySymbolsEquiv, UnicodeString(সত্য, s, len));
            }
            ++(*যোগ_currency_প্রতীক_গুনতি); // iso code
            ++(*যোগ_currency_name_গুনতি); // long name
        }

        // currency plurals
        UErrorCode ec3 = U_ZERO_ERROR;
        LocalUResourceBundlePointer curr_p(ures_get_By_চাবি(rb.getAlias(), CURRENCYPLURALS, nullptr, &ec3));
        n = ures_get_আকার(curr_p.getAlias());
        জন্য (int32_t i=0; i<n; ++i) {
            LocalUResourceBundlePointer names(ures_get_By_সূচক(curr_p.getAlias(), i, nullptr, &ec3));
            *যোগ_currency_name_গুনতি += ures_get_আকার(names.getAlias());
        }

        যদি (!fallback(locale)) {
            বিরতি;
        }
    }
}

স্থির char16_t*
toUpperCase(ধ্রুবক char16_t* source, int32_t len, ধ্রুবক অক্ষর* locale) {
    char16_t* dest = nullptr;
    UErrorCode ec = U_ZERO_ERROR;
    int32_t destLen = u_strToUpper(dest, 0, source, len, locale, &ec);

    ec = U_ZERO_ERROR;
    dest = static_cast<char16_t*>(uprv_malloc(আকার(char16_t) * MAX(destLen, len)));
    যদি (dest == nullptr) {
        ফেরত nullptr;
    }
    u_strToUpper(dest, destLen, source, len, locale, &ec);
    যদি (U_FAILURE(ec)) {
        u_memcpy(dest, source, len);
    } 
    ফেরত dest;
}


স্থির শূন্য deleteCurrencyNames(CurrencyNameStruct* currencyNames, int32_t count);
// Collect all available currency names associated with the given locale
// (enable fallback chain).
// Read currenc names defined in resource bundle "Currencies" and
// "CurrencyPlural", enable fallback chain.
// return the malloc-ed currency name arrays and the total number of currency
// names in the array.
স্থির শূন্য
collectCurrencyNames(ধ্রুবক অক্ষর* locale, 
                     CurrencyNameStruct** currencyNames, 
                     int32_t* যোগ_currency_name_গুনতি, 
                     CurrencyNameStruct** currencySymbols, 
                     int32_t* যোগ_currency_প্রতীক_গুনতি, 
                     UErrorCode& ec) {
    যদি (U_FAILURE(ec)) {
        *currencyNames = *currencySymbols = nullptr;
        *যোগ_currency_name_গুনতি = *যোগ_currency_প্রতীক_গুনতি = 0;
        ফেরত;
    }
    U_NAMESPACE_USE
    ধ্রুবক icu::Hashtable *currencySymbolsEquiv = getCurrSymbolsEquiv();
    // Look up the Currencies resource for the given locale.
    UErrorCode ec2 = U_ZERO_ERROR;

    যদি (locale == nullptr) {
        locale = uloc_getDefault();
    }
    CharString loc = ulocimp_getName(locale, ec2);
    যদি (U_FAILURE(ec2)) {
        ec = U_ILLEGAL_ARGUMENT_ERROR;
        *currencyNames = *currencySymbols = nullptr;
        *যোগ_currency_name_গুনতি = *যোগ_currency_প্রতীক_গুনতি = 0;
        ফেরত;
    }

    // Get maximum currency name count first.
    get_Currency_Name_গুনতি(loc.তথ্য(), যোগ_currency_name_গুনতি, যোগ_currency_প্রতীক_গুনতি);

    *currencyNames = static_cast<CurrencyNameStruct*>(
        uprv_malloc(আকার(CurrencyNameStruct) * (*যোগ_currency_name_গুনতি)));
    যদি(*currencyNames == nullptr) {
        *currencySymbols = nullptr;
        *যোগ_currency_name_গুনতি = *যোগ_currency_প্রতীক_গুনতি = 0;
        ec = U_MEMORY_ALLOCATION_ERROR;
        ফেরত;
    }
    *currencySymbols = static_cast<CurrencyNameStruct*>(
        uprv_malloc(আকার(CurrencyNameStruct) * (*যোগ_currency_প্রতীক_গুনতি)));

    যদি(*currencySymbols == nullptr) {
        uprv_free(*currencyNames);
        *currencyNames = nullptr;
        *যোগ_currency_name_গুনতি = *যোগ_currency_প্রতীক_গুনতি = 0;
        ec = U_MEMORY_ALLOCATION_ERROR;
        ফেরত;
    }

    ধ্রুবক char16_t* s = nullptr;  // currency name
    অক্ষর* iso = nullptr;  // currency ISO code

    *যোগ_currency_name_গুনতি = 0;
    *যোগ_currency_প্রতীক_গুনতি = 0;

    UErrorCode ec3 = U_ZERO_ERROR;
    UErrorCode ec4 = U_ZERO_ERROR;

    // Using hash to remove duplicates caused by locale fallback
    LocalUHashtablePointer currencyIsoCodes(uhash_open(uhash_hashChars, uhash_compareChars, nullptr, &ec3));
    LocalUHashtablePointer currencyPluralIsoCodes(uhash_open(uhash_hashChars, uhash_compareChars, nullptr, &ec4));
    জন্য (int32_t localeLevel = 0; ; ++localeLevel) {
        ec2 = U_ZERO_ERROR;
        // TODO: ures_openDirect
        LocalUResourceBundlePointer rb(ures_open(U_ICUDATA_CURR, loc.তথ্য(), &ec2));
        LocalUResourceBundlePointer curr(ures_get_By_চাবি(rb.getAlias(), CURRENCIES, nullptr, &ec2));
        int32_t n = ures_get_আকার(curr.getAlias());
        জন্য (int32_t i=0; i<n; ++i) {
            LocalUResourceBundlePointer names(ures_get_By_সূচক(curr.getAlias(), i, nullptr, &ec2));
            int32_t len;
            s = ures_get_String_By_সূচক(names.getAlias(), UCURR_প্রতীক_NAME, &len, &ec2);
            // TODO: uhash_put wont change key/value?
            iso = const_cast<অক্ষর*>(ures_get_চাবি(names.getAlias()));
            যদি (localeLevel != 0 && uhash_get(currencyIsoCodes.getAlias(), iso) != nullptr) {
                চালু;
            }
            uhash_put(currencyIsoCodes.getAlias(), iso, iso, &ec3); 
            // Add currency symbol.
            (*currencySymbols)[(*যোগ_currency_প্রতীক_গুনতি)++] = {iso, const_cast<char16_t*>(s), len, 0};

            // Add equivalent symbols
            যদি (currencySymbolsEquiv != nullptr) {
                UnicodeString str(সত্য, s, len);
                icu::EquivIterator iter(*currencySymbolsEquiv, str);
                ধ্রুবক UnicodeString *প্রতীক;
                যখন ((প্রতীক = iter.next()) != nullptr) {
                    (*currencySymbols)[(*যোগ_currency_প্রতীক_গুনতি)++]
                        = {iso, const_cast<char16_t*>(প্রতীক->get_বাফার()), প্রতীক->length(), 0};
                }
            }

            // Add currency long name.
            s = ures_get_String_By_সূচক(names.getAlias(), UCURR_LONG_NAME, &len, &ec2);
            char16_t* upperName = toUpperCase(s, len, locale);
            যদি (upperName == nullptr) {
                ec = U_MEMORY_ALLOCATION_ERROR;
                যাও error;
            }
            (*currencyNames)[(*যোগ_currency_name_গুনতি)++] = {iso, upperName, len, NEED_TO_BE_DELETED};

            // put (iso, 3, and iso) in to array
            // Add currency ISO code.
            char16_t* isoCode = static_cast<char16_t*>(uprv_malloc(আকার(char16_t) * 3));
            যদি (isoCode == nullptr) {
                ec = U_MEMORY_ALLOCATION_ERROR;
                যাও error;
            }
            // Must convert iso[] into Unicode
            u_charsToUChars(iso, isoCode, 3);
            (*currencySymbols)[(*যোগ_currency_প্রতীক_গুনতি)++] = {iso, isoCode, 3, NEED_TO_BE_DELETED};
        }

        // currency plurals
        UErrorCode ec5 = U_ZERO_ERROR;
        LocalUResourceBundlePointer curr_p(ures_get_By_চাবি(rb.getAlias(), CURRENCYPLURALS, nullptr, &ec5));
        n = ures_get_আকার(curr_p.getAlias());
        জন্য (int32_t i=0; i<n; ++i) {
            LocalUResourceBundlePointer names(ures_get_By_সূচক(curr_p.getAlias(), i, nullptr, &ec5));
            iso = const_cast<অক্ষর*>(ures_get_চাবি(names.getAlias()));
            // Using hash to remove duplicated ISO codes in fallback chain.
            যদি (localeLevel != 0 && uhash_get(currencyPluralIsoCodes.getAlias(), iso) != nullptr) {
                চালু;
            }
            uhash_put(currencyPluralIsoCodes.getAlias(), iso, iso, &ec4);
            int32_t num = ures_get_আকার(names.getAlias());
            int32_t len;
            জন্য (int32_t j = 0; j < num; ++j) {
                // TODO: remove duplicates between singular name and 
                // currency long name?
                s = ures_get_String_By_সূচক(names.getAlias(), j, &len, &ec5);
                char16_t* upperName = toUpperCase(s, len, locale);
                যদি (upperName == nullptr) {
                    ec = U_MEMORY_ALLOCATION_ERROR;
                    যাও error;
                }
                (*currencyNames)[(*যোগ_currency_name_গুনতি)++] = {iso, upperName, len, NEED_TO_BE_DELETED};
            }
        }

        যদি (!fallback(loc)) {
            বিরতি;
        }
    }

    // quick sort the struct
    qsort(*currencyNames, *যোগ_currency_name_গুনতি, 
          আকার(CurrencyNameStruct), currencyNameComparator);
    qsort(*currencySymbols, *যোগ_currency_প্রতীক_গুনতি, 
          আকার(CurrencyNameStruct), currencyNameComparator);

#ifdef UCURR_DEBUG
    printf("currency name count: %d\n", *যোগ_currency_name_গুনতি);
    জন্য (int32_t সূচক = 0; সূচক < *যোগ_currency_name_গুনতি; ++সূচক) {
        printf("index: %d\n", সূচক);
        printf("iso: %s\n", (*currencyNames)[সূচক].IsoCode);
        অক্ষর curNameBuf[1024];
        memset(curNameBuf, 0, 1024);
        u_austrncpy(curNameBuf, (*currencyNames)[সূচক].currencyName, (*currencyNames)[সূচক].currencyNameLen);
        printf("currencyName: %s\n", curNameBuf);
        printf("len: %d\n", (*currencyNames)[সূচক].currencyNameLen);
    }
    printf("currency symbol count: %d\n", *যোগ_currency_প্রতীক_গুনতি);
    জন্য (int32_t সূচক = 0; সূচক < *যোগ_currency_প্রতীক_গুনতি; ++সূচক) {
        printf("index: %d\n", সূচক);
        printf("iso: %s\n", (*currencySymbols)[সূচক].IsoCode);
        অক্ষর curNameBuf[1024];
        memset(curNameBuf, 0, 1024);
        u_austrncpy(curNameBuf, (*currencySymbols)[সূচক].currencyName, (*currencySymbols)[সূচক].currencyNameLen);
        printf("currencySymbol: %s\n", curNameBuf);
        printf("len: %d\n", (*currencySymbols)[সূচক].currencyNameLen);
    }
#endif
    // fail on hashtable errors
    যদি (U_FAILURE(ec3)) {
      ec = ec3;
    } অন্যথা যদি (U_FAILURE(ec4)) {
      ec = ec4;
    }

  error:
    // clean up if we got error
    যদি (U_FAILURE(ec)) {
        deleteCurrencyNames(*currencyNames, *যোগ_currency_name_গুনতি);
        deleteCurrencyNames(*currencySymbols, *যোগ_currency_প্রতীক_গুনতি);
        *currencyNames = *currencySymbols = nullptr;
        *যোগ_currency_name_গুনতি = *যোগ_currency_প্রতীক_গুনতি = 0;
    }
}

// @param  currencyNames: currency names array
// @param  indexInCurrencyNames: the index of the character in currency names 
//         array against which the comparison is done
// @param  key: input text char to compare against
// @param  begin(IN/OUT): the begin index of matching range in currency names array
// @param  end(IN/OUT): the end index of matching range in currency names array.
স্থির int32_t
দ্বিমিক_অনুসন্ধান(ধ্রুবক CurrencyNameStruct* currencyNames, 
             int32_t সূচক_In_Currency_Names,
             ধ্রুবক char16_t চাবি,
             int32_t* begin, int32_t* end) {
#ifdef UCURR_DEBUG
    printf("key = %x\n", চাবি);
#endif
   int32_t first = *begin;
   int32_t last = *end;
   যখন (first <= last) {
       int32_t mid = (first + last) / 2;  // compute mid point.
       যদি (সূচক_In_Currency_Names >= currencyNames[mid].currencyNameLen) {
           first = mid + 1;
       } অন্যথা {
           যদি (চাবি > currencyNames[mid].currencyName[সূচক_In_Currency_Names]) {
               first = mid + 1;
           }
           অন্যথা যদি (চাবি < currencyNames[mid].currencyName[সূচক_In_Currency_Names]) {
               last = mid - 1;
           }
           অন্যথা {
                // Find a match, and looking for ranges
                // Now do two more binary searches. First, on the left side for
                // the greatest L such that CurrencyNameStruct[L] < key.
                int32_t L = *begin;
                int32_t R = mid;

#ifdef UCURR_DEBUG
                printf("mid = %d\n", mid);
#endif
                যখন (L < R) {
                    int32_t M = (L + R) / 2;
#ifdef UCURR_DEBUG
                    printf("L = %d, R = %d, M = %d\n", L, R, M);
#endif
                    যদি (সূচক_In_Currency_Names >= currencyNames[M].currencyNameLen) {
                        L = M + 1;
                    } অন্যথা {
                        যদি (currencyNames[M].currencyName[সূচক_In_Currency_Names] < চাবি) {
                            L = M + 1;
                        } অন্যথা {
#ifdef UCURR_DEBUG
                            U_ASSERT(currencyNames[M].currencyName[সূচক_In_Currency_Names] == চাবি);
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
                U_ASSERT(currencyNames[*begin].currencyName[সূচক_In_Currency_Names] == চাবি);
#endif

                // Now for the second search, finding the least R such that
                // key < CurrencyNameStruct[R].
                L = mid;
                R = *end;
                যখন (L < R) {
                    int32_t M = (L + R) / 2;
#ifdef UCURR_DEBUG
                    printf("L = %d, R = %d, M = %d\n", L, R, M);
#endif
                    যদি (currencyNames[M].currencyNameLen < সূচক_In_Currency_Names) {
                        L = M + 1;
                    } অন্যথা {
                        যদি (currencyNames[M].currencyName[সূচক_In_Currency_Names] > চাবি) {
                            R = M;
                        } অন্যথা {
#ifdef UCURR_DEBUG
                            U_ASSERT(currencyNames[M].currencyName[সূচক_In_Currency_Names] == চাবি);
#endif
                            L = M + 1;
                        }
                    }
                }
#ifdef UCURR_DEBUG
                U_ASSERT(L == R);
#endif
                যদি (currencyNames[R].currencyName[সূচক_In_Currency_Names] > চাবি) {
                    *end = R - 1;
                } অন্যথা {
                    *end = R;
                }
#ifdef UCURR_DEBUG
                printf("end = %d\n", *end);
#endif

                // now, found the range. check whether there is exact match
                যদি (currencyNames[*begin].currencyNameLen == সূচক_In_Currency_Names + 1) {
                    ফেরত *begin;  // find range and exact match.
                }
                ফেরত -1;  // find range, but no exact match.
           }
       }
   }
   *begin = -1;
   *end = -1;
   ফেরত -1;    // failed to find range.
}


// Linear search "text" in "currencyNames".
// @param  begin, end: the begin and end index in currencyNames, within which
//         range should the search be performed.
// @param  textLen: the length of the text to be compared
// @param  maxMatchLen(IN/OUT): passing in the computed max matching length
//                              pass out the new max  matching length
// @param  maxMatchIndex: the index in currencyName which has the longest
//                        match with input text.
স্থির শূন্য
linear_অনুসন্ধান(ধ্রুবক CurrencyNameStruct* currencyNames, 
             int32_t begin, int32_t end,
             ধ্রুবক char16_t* text, int32_t textLen,
             int32_t *partialMatchLen,
             int32_t *maxMatchLen, int32_t* max_Match_সূচক) {
    int32_t initialPartialMatchLen = *partialMatchLen;
    জন্য (int32_t সূচক = begin; সূচক <= end; ++সূচক) {
        int32_t len = currencyNames[সূচক].currencyNameLen;
        যদি (len > *maxMatchLen && len <= textLen &&
            uprv_memcmp(currencyNames[সূচক].currencyName, text, len * আকার(char16_t)) == 0) {
            *partialMatchLen = MAX(*partialMatchLen, len);
            *max_Match_সূচক = সূচক;
            *maxMatchLen = len;
#ifdef UCURR_DEBUG
            printf("maxMatchIndex = %d, maxMatchLen = %d\n",
                   *max_Match_সূচক, *maxMatchLen);
#endif
        } অন্যথা {
            // Check for partial matches.
            জন্য (int32_t i=initialPartialMatchLen; i<MIN(len, textLen); i++) {
                যদি (currencyNames[সূচক].currencyName[i] != text[i]) {
                    বিরতি;
                }
                *partialMatchLen = MAX(*partialMatchLen, i + 1);
            }
        }
    }
}

#define LINEAR_অনুসন্ধান_THRESHOLD 10

// Find longest match between "text" and currency names in "currencyNames".
// @param  total_currency_count: total number of currency names in CurrencyNames.
// @param  textLen: the length of the text to be compared
// @param  maxMatchLen: passing in the computed max matching length
//                              pass out the new max  matching length
// @param  maxMatchIndex: the index in currencyName which has the longest
//                        match with input text.
স্থির শূন্য
অনুসন্ধান_Currency_Name(ধ্রুবক CurrencyNameStruct* currencyNames, 
                   int32_t যোগ_currency_গুনতি,
                   ধ্রুবক char16_t* text, int32_t textLen,
                   int32_t *partialMatchLen,
                   int32_t* maxMatchLen, int32_t* max_Match_সূচক) {
    *max_Match_সূচক = -1;
    *maxMatchLen = 0;
    int32_t match_সূচক = -1;
    int32_t দ্বিমিক_অনুসন্ধান_Begin = 0;
    int32_t দ্বিমিক_অনুসন্ধান_End = যোগ_currency_গুনতি - 1;
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
    জন্য (int32_t সূচক = 0; সূচক < textLen; ++সূচক) {
        // matchIndex saves the one with exact match till the current point.
        // [binarySearchBegin, binarySearchEnd] saves the matching range.
        match_সূচক = দ্বিমিক_অনুসন্ধান(currencyNames, সূচক,
                                  text[সূচক],
                                  &দ্বিমিক_অনুসন্ধান_Begin, &দ্বিমিক_অনুসন্ধান_End);
        যদি (দ্বিমিক_অনুসন্ধান_Begin == -1) { // did not find the range
            বিরতি;
        }
        *partialMatchLen = MAX(*partialMatchLen, সূচক + 1);
        যদি (match_সূচক != -1) { 
            // find an exact match for text from text[0] to text[index] 
            // in currencyNames array.
            *maxMatchLen = সূচক + 1;
            *max_Match_সূচক = match_সূচক;
        }
        যদি (দ্বিমিক_অনুসন্ধান_End - দ্বিমিক_অনুসন্ধান_Begin < LINEAR_অনুসন্ধান_THRESHOLD) {
            // linear search if within threshold.
            linear_অনুসন্ধান(currencyNames, দ্বিমিক_অনুসন্ধান_Begin, দ্বিমিক_অনুসন্ধান_End,
                         text, textLen,
                         partialMatchLen,
                         maxMatchLen, max_Match_সূচক);
            বিরতি;
        }
    }
}

//========================= currency name cache =====================
প্রকারলেখ কাঠামো {
    অক্ষর locale[ULOC_FULLNAME_CAPACITY];  //key
    // currency names, case insensitive
    CurrencyNameStruct* currencyNames;  //মান
    int32_t যোগ_Currency_Name_গুনতি;  // currency name count
    // currency symbols and ISO code, case sensitive
    CurrencyNameStruct* currencySymbols; //মান
    int32_t যোগ_Currency_প্রতীক_গুনতি;  //গুনতি
    // reference count.
    // reference count is set to 1 when an entry is put to cache.
    // it increases by 1 before accessing, and decreased by 1 after accessing.
    // The entry is deleted when ref count is zero, which means 
    // the entry is replaced out of cache and no process is accessing it.
    int32_t ref_গুনতি;
} CurrencyNameCacheEntry;


#define CURRENCY_NAME_CACHE_অঙ্ক 10

// Reserve 10 cache entries.
স্থির CurrencyNameCacheEntry* currCache[CURRENCY_NAME_CACHE_অঙ্ক] = {nullptr};
// Using an index to indicate which entry to be replaced when cache is full.
// It is a simple round-robin replacement strategy.
স্থির int8_t current_Cache_Entry_সূচক = 0;

স্থির UMutex gCurrencyCacheMutex;

// Cache deletion
স্থির শূন্য
deleteCurrencyNames(CurrencyNameStruct* currencyNames, int32_t count) {
    জন্য (int32_t সূচক = 0; সূচক < count; ++সূচক) {
        যদি ( (currencyNames[সূচক].flag & NEED_TO_BE_DELETED) ) {
            uprv_free(currencyNames[সূচক].currencyName);
        }
    }
    uprv_free(currencyNames);
}


স্থির শূন্য
deleteCacheEntry(CurrencyNameCacheEntry* entry) {
    deleteCurrencyNames(entry->currencyNames, entry->যোগ_Currency_Name_গুনতি);
    deleteCurrencyNames(entry->currencySymbols, entry->যোগ_Currency_প্রতীক_গুনতি);
    uprv_free(entry);
}


// Cache clean up
স্থির UBool U_CALLCONV
currency_cache_cleanup() {
    জন্য (int32_t i = 0; i < CURRENCY_NAME_CACHE_অঙ্ক; ++i) {
        যদি (currCache[i]) {
            deleteCacheEntry(currCache[i]);
            currCache[i] = nullptr;
        }
    }
    ফেরত সত্য;
}


/**
 * Loads the currency name data from the cache, or from resource bundles if necessary.
 * The refCount is automatically incremented.  It is the caller's responsibility
 * to decrement it when done!
 */
স্থির CurrencyNameCacheEntry*
getCacheEntry(ধ্রুবক অক্ষর* locale, UErrorCode& ec) {

    int32_t যোগ_currency_name_গুনতি = 0;
    CurrencyNameStruct* currencyNames = nullptr;
    int32_t যোগ_currency_প্রতীক_গুনতি = 0;
    CurrencyNameStruct* currencySymbols = nullptr;
    CurrencyNameCacheEntry* cacheEntry = nullptr;

    umtx_lock(&gCurrencyCacheMutex);
    // in order to handle racing correctly,
    // not putting 'search' in a separate function.
    int8_t found = -1;
    জন্য (int8_t i = 0; i < CURRENCY_NAME_CACHE_অঙ্ক; ++i) {
        যদি (currCache[i]!= nullptr &&
            uprv_strcmp(locale, currCache[i]->locale) == 0) {
            found = i;
            বিরতি;
        }
    }
    যদি (found != -1) {
        cacheEntry = currCache[found];
        ++(cacheEntry->ref_গুনতি);
    }
    umtx_unlock(&gCurrencyCacheMutex);
    যদি (found == -1) {
        collectCurrencyNames(locale, &currencyNames, &যোগ_currency_name_গুনতি, &currencySymbols, &যোগ_currency_প্রতীক_গুনতি, ec);
        যদি (U_FAILURE(ec)) {
            ফেরত nullptr;
        }
        umtx_lock(&gCurrencyCacheMutex);
        // check again.
        জন্য (int8_t i = 0; i < CURRENCY_NAME_CACHE_অঙ্ক; ++i) {
            যদি (currCache[i]!= nullptr &&
                uprv_strcmp(locale, currCache[i]->locale) == 0) {
                found = i;
                বিরতি;
            }
        }
        যদি (found == -1) {
            // insert new entry to 
            // currentCacheEntryIndex % CURRENCY_NAME_CACHE_NUM
            // and remove the existing entry 
            // currentCacheEntryIndex % CURRENCY_NAME_CACHE_NUM
            // from cache.
            cacheEntry = currCache[current_Cache_Entry_সূচক];
            যদি (cacheEntry) {
                --(cacheEntry->ref_গুনতি);
                // delete if the ref count is zero
                যদি (cacheEntry->ref_গুনতি == 0) {
                    deleteCacheEntry(cacheEntry);
                }
            }
            cacheEntry = static_cast<CurrencyNameCacheEntry*>(uprv_malloc(আকার(CurrencyNameCacheEntry)));
            যদি (cacheEntry == nullptr) {
                deleteCurrencyNames(currencyNames, যোগ_currency_name_গুনতি);
                deleteCurrencyNames(currencySymbols, যোগ_currency_প্রতীক_গুনতি);
                ec = U_MEMORY_ALLOCATION_ERROR;
                ফেরত nullptr;
            }
            currCache[current_Cache_Entry_সূচক] = cacheEntry;
            uprv_strcpy(cacheEntry->locale, locale);
            cacheEntry->currencyNames = currencyNames;
            cacheEntry->যোগ_Currency_Name_গুনতি = যোগ_currency_name_গুনতি;
            cacheEntry->currencySymbols = currencySymbols;
            cacheEntry->যোগ_Currency_প্রতীক_গুনতি = যোগ_currency_প্রতীক_গুনতি;
            cacheEntry->ref_গুনতি = 2; // one for cache, one for reference
            current_Cache_Entry_সূচক = (current_Cache_Entry_সূচক + 1) % CURRENCY_NAME_CACHE_অঙ্ক;
            ucln_common_registerCleanup(UCLN_COMMON_CURRENCY, currency_cleanup);
        } অন্যথা {
            deleteCurrencyNames(currencyNames, যোগ_currency_name_গুনতি);
            deleteCurrencyNames(currencySymbols, যোগ_currency_প্রতীক_গুনতি);
            cacheEntry = currCache[found];
            ++(cacheEntry->ref_গুনতি);
        }
        umtx_unlock(&gCurrencyCacheMutex);
    }

    ফেরত cacheEntry;
}

স্থির শূন্য releaseCacheEntry(CurrencyNameCacheEntry* cacheEntry) {
    umtx_lock(&gCurrencyCacheMutex);
    --(cacheEntry->ref_গুনতি);
    যদি (cacheEntry->ref_গুনতি == 0) {  // remove
        deleteCacheEntry(cacheEntry);
    }
    umtx_unlock(&gCurrencyCacheMutex);
}

U_CAPI শূন্য
uprv_parseCurrency(ধ্রুবক অক্ষর* locale,
                   ধ্রুবক icu::UnicodeString& text,
                   icu::ParsePosition& pos,
                   int8_t type,
                   int32_t* partialMatchLen,
                   char16_t* ফলাফল,
                   UErrorCode& ec) {
    U_NAMESPACE_USE
    যদি (U_FAILURE(ec)) {
        ফেরত;
    }
    CurrencyNameCacheEntry* cacheEntry = getCacheEntry(locale, ec);
    যদি (U_FAILURE(ec)) {
        ফেরত;
    }

    int32_t যোগ_currency_name_গুনতি = cacheEntry->যোগ_Currency_Name_গুনতি;
    CurrencyNameStruct* currencyNames = cacheEntry->currencyNames;
    int32_t যোগ_currency_প্রতীক_গুনতি = cacheEntry->যোগ_Currency_প্রতীক_গুনতি;
    CurrencyNameStruct* currencySymbols = cacheEntry->currencySymbols;

    int32_t start = pos.get_সূচক();

    char16_t প্রবেশ_Text[MAX_CURRENCY_NAME_LEN];
    char16_t upperText[MAX_CURRENCY_NAME_LEN];
    int32_t textLen = MIN(MAX_CURRENCY_NAME_LEN, text.length() - start);
    text.extract(start, textLen, প্রবেশ_Text);
    UErrorCode ec1 = U_ZERO_ERROR;
    textLen = u_strToUpper(upperText, MAX_CURRENCY_NAME_LEN, প্রবেশ_Text, textLen, locale, &ec1);

    // Make sure partialMatchLen is initialized
    *partialMatchLen = 0;

    int32_t max = 0;
    int32_t match_সূচক = -1;
    // case in-sensitive comparison against currency names
    অনুসন্ধান_Currency_Name(currencyNames, যোগ_currency_name_গুনতি, 
                       upperText, textLen, partialMatchLen, &max, &match_সূচক);

#ifdef UCURR_DEBUG
    printf("search in names, max = %d, matchIndex = %d\n", max, match_সূচক);
#endif

    int32_t max_In_প্রতীক = 0;
    int32_t match_সূচক_In_প্রতীক = -1;
    যদি (type != UCURR_LONG_NAME) {  // not name only
        // case sensitive comparison against currency symbols and ISO code.
        অনুসন্ধান_Currency_Name(currencySymbols, যোগ_currency_প্রতীক_গুনতি, 
                           প্রবেশ_Text, textLen,
                           partialMatchLen,
                           &max_In_প্রতীক, &match_সূচক_In_প্রতীক);
    }

#ifdef UCURR_DEBUG
    printf("search in symbols, maxInSymbol = %d, matchIndexInSymbol = %d\n", max_In_প্রতীক, match_সূচক_In_প্রতীক);
    যদি(match_সূচক_In_প্রতীক != -1) {
      printf("== ISO=%s\n", currencySymbols[match_সূচক_In_প্রতীক].IsoCode);
    }
#endif

    যদি (max >= max_In_প্রতীক && match_সূচক != -1) {
        u_charsToUChars(currencyNames[match_সূচক].IsoCode, ফলাফল, 4);
        pos.set_সূচক(start + max);
    } অন্যথা যদি (max_In_প্রতীক >= max && match_সূচক_In_প্রতীক != -1) {
        u_charsToUChars(currencySymbols[match_সূচক_In_প্রতীক].IsoCode, ফলাফল, 4);
        pos.set_সূচক(start + max_In_প্রতীক);
    }

    // decrease reference count
    releaseCacheEntry(cacheEntry);
}

শূন্য uprv_currencyLeads(ধ্রুবক অক্ষর* locale, icu::UnicodeSet& ফলাফল, UErrorCode& ec) {
    U_NAMESPACE_USE
    যদি (U_FAILURE(ec)) {
        ফেরত;
    }
    CurrencyNameCacheEntry* cacheEntry = getCacheEntry(locale, ec);
    যদি (U_FAILURE(ec)) {
        ফেরত;
    }

    জন্য (int32_t i=0; i<cacheEntry->যোগ_Currency_প্রতীক_গুনতি; i++) {
        ধ্রুবক CurrencyNameStruct& info = cacheEntry->currencySymbols[i];
        UChar32 cp;
        U16_GET(info.currencyName, 0, 0, info.currencyNameLen, cp);
        ফলাফল.add(cp);
    }

    জন্য (int32_t i=0; i<cacheEntry->যোগ_Currency_Name_গুনতি; i++) {
        ধ্রুবক CurrencyNameStruct& info = cacheEntry->currencyNames[i];
        UChar32 cp;
        U16_GET(info.currencyName, 0, 0, info.currencyNameLen, cp);
        ফলাফল.add(cp);
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
U_CAPI শূন্য
uprv_getStaticCurrencyName(ধ্রুবক char16_t* iso, ধ্রুবক অক্ষর* loc,
                           icu::UnicodeString& ফলাফল, UErrorCode& ec)
{
    U_NAMESPACE_USE

    int32_t len;
    ধ্রুবক char16_t* currname = ucurr_getName(iso, loc, UCURR_প্রতীক_NAME,
                                          nullptr /* isChoiceFormat */, &len, &ec);
    যদি (U_SUCCESS(ec)) {
        ফলাফল.setTo(currname, len);
    }
}

U_CAPI int32_t U_EXPORT2
ucurr_getDefaultFractionDigits(ধ্রুবক char16_t* currency, UErrorCode* ec) {
    ফেরত ucurr_getDefaultFractionDigitsForUsage(currency,UCURR_USAGE_STANDARD,ec);
}

U_CAPI int32_t U_EXPORT2
ucurr_getDefaultFractionDigitsForUsage(ধ্রুবক char16_t* currency, ধ্রুবক UCurrencyUsage usage, UErrorCode* ec) {
    int32_t fracDigits = 0;
    যদি (U_SUCCESS(*ec)) {
        নির্বাচন (usage) {
            ক্ষেত্র UCURR_USAGE_STANDARD:
                fracDigits = (find_Meta_তথ্য(currency, *ec))[0];
                বিরতি;
            ক্ষেত্র UCURR_USAGE_CASH:
                fracDigits = (find_Meta_তথ্য(currency, *ec))[2];
                বিরতি;
            পূর্বনির্ধারিত:
                *ec = U_UNSUPPORTED_ERROR;
        }
    }
    ফেরত fracDigits;
}

U_CAPI দ্বিগুণ U_EXPORT2
ucurr_getRoundingIncrement(ধ্রুবক char16_t* currency, UErrorCode* ec) {
    ফেরত ucurr_getRoundingIncrementForUsage(currency, UCURR_USAGE_STANDARD, ec);
}

U_CAPI দ্বিগুণ U_EXPORT2
ucurr_getRoundingIncrementForUsage(ধ্রুবক char16_t* currency, ধ্রুবক UCurrencyUsage usage, UErrorCode* ec) {
    দ্বিগুণ ফলাফল = 0.0;

    ধ্রুবক int32_t *তথ্য = find_Meta_তথ্য(currency, *ec);
    যদি (U_SUCCESS(*ec)) {
        int32_t fracDigits;
        int32_t increment;
        নির্বাচন (usage) {
            ক্ষেত্র UCURR_USAGE_STANDARD:
                fracDigits = তথ্য[0];
                increment = তথ্য[1];
                বিরতি;
            ক্ষেত্র UCURR_USAGE_CASH:
                fracDigits = তথ্য[2];
                increment = তথ্য[3];
                বিরতি;
            পূর্বনির্ধারিত:
                *ec = U_UNSUPPORTED_ERROR;
                ফেরত ফলাফল;
        }

        // If the meta data is invalid, return 0.0
        যদি (fracDigits < 0 || fracDigits > MAX_POW10) {
            *ec = U_INVALID_FORMAT_ERROR;
        } অন্যথা {
            // A rounding value of 0 or 1 indicates no rounding.
            যদি (increment >= 2) {
                // Return (increment) / 10^(fracDigits).  The only actual rounding data,
                // as of this writing, is CHF { 2, 5 }.
                ফলাফল = দ্বিগুণ(increment) / POW10[fracDigits];
            }
        }
    }

    ফেরত ফলাফল;
}

U_CDECL_BEGIN

প্রকারলেখ কাঠামো UCurrencyContext {
    uint32_t currType; /* UCurrCurrencyType */
    uint32_t listIdx;
} UCurrencyContext;

/*
Please keep this list in alphabetical order.
You can look at the CLDR supplemental data or ISO-4217 for the meaning of some
of these items.
ISO-4217: http://www.iso.org/iso/en/prods-services/popstds/currencycodeslist.html
*/
স্থির ধ্রুবক কাঠামো CurrencyList {
    ধ্রুবক অক্ষর *currency;
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

স্থির int32_t U_CALLCONV
ucurr_গুনতি_Currency_List(UEnumeration *enumerator, UErrorCode * /*pErrorCode*/) {
    UCurrencyContext *myContext = (UCurrencyContext *)(enumerator->context);
    uint32_t currType = myContext->currType;
    int32_t count = 0;

    /* Count the number of items matching the type we are looking for. */
    জন্য (int32_t idx = 0; gCurrencyList[idx].currency != nullptr; idx++) {
        যদি (UCURR_MATCHES_BITMASK(gCurrencyList[idx].currType, currType)) {
            count++;
        }
    }
    ফেরত count;
}

স্থির ধ্রুবক অক্ষর* U_CALLCONV
ucurr_পরবর্তী_Currency_List(UEnumeration *enumerator,
                        int32_t* ফলাফল_দৈর্ঘ্য,
                        UErrorCode * /*pErrorCode*/)
{
    UCurrencyContext *myContext = (UCurrencyContext *)(enumerator->context);

    /* Find the next in the list that matches the type we are looking for. */
    যখন (myContext->listIdx < UPRV_LENGTHOF(gCurrencyList)-1) {
        ধ্রুবক কাঠামো CurrencyList *currItem = &gCurrencyList[myContext->listIdx++];
        যদি (UCURR_MATCHES_BITMASK(currItem->currType, myContext->currType))
        {
            যদি (ফলাফল_দৈর্ঘ্য) {
                *ফলাফল_দৈর্ঘ্য = 3; /* Currency codes are only 3 chars long */
            }
            ফেরত currItem->currency;
        }
    }
    /* We enumerated too far. */
    যদি (ফলাফল_দৈর্ঘ্য) {
        *ফলাফল_দৈর্ঘ্য = 0;
    }
    ফেরত nullptr;
}

স্থির শূন্য U_CALLCONV
ucurr_resetCurrencyList(UEnumeration *enumerator, UErrorCode * /*pErrorCode*/) {
    ((UCurrencyContext *)(enumerator->context))->listIdx = 0;
}

স্থির শূন্য U_CALLCONV
ucurr_closeCurrencyList(UEnumeration *enumerator) {
    uprv_free(enumerator->context);
    uprv_free(enumerator);
}

স্থির শূন্য U_CALLCONV
ucurr_createCurrencyList(UHashtable *isoCodes, UErrorCode* status){
    UErrorCode localStatus = U_ZERO_ERROR;

    // Look up the CurrencyMap element in the root bundle.
    UResourceBundle *rb = ures_openDirect(U_ICUDATA_CURR, CURRENCY_তথ্য, &localStatus);
    LocalUResourceBundlePointer currencyMapArray(ures_get_By_চাবি(rb, CURRENCY_MAP, rb, &localStatus));

    যদি (U_SUCCESS(localStatus)) {
        // process each entry in currency map
        জন্য (int32_t i=0; i<ures_get_আকার(currencyMapArray.getAlias()); i++) {
            // get the currency resource
            LocalUResourceBundlePointer currencyArray(ures_get_By_সূচক(currencyMapArray.getAlias(), i, nullptr, &localStatus));
            // process each currency
            যদি (U_SUCCESS(localStatus)) {
                জন্য (int32_t j=0; j<ures_get_আকার(currencyArray.getAlias()); j++) {
                    // get the currency resource
                    LocalUResourceBundlePointer currencyRes(ures_get_By_সূচক(currencyArray.getAlias(), j, nullptr, &localStatus));
                    IsoCodeEntry *entry = (IsoCodeEntry*)uprv_malloc(আকার(IsoCodeEntry));
                    যদি (entry == nullptr) {
                        *status = U_MEMORY_ALLOCATION_ERROR;
                        ফেরত;
                    }

                    // get the ISO code
                    int32_t iso_দৈর্ঘ্য = 0;
                    LocalUResourceBundlePointer idRes(ures_get_By_চাবি(currencyRes.getAlias(), "id", nullptr, &localStatus));
                    যদি (idRes.isNull()) {
                        চালু;
                    }
                    ধ্রুবক char16_t *isoCode = ures_getString(idRes.getAlias(), &iso_দৈর্ঘ্য, &localStatus);

                    // get from date
                    UDate fromDate = U_DATE_MIN;
                    LocalUResourceBundlePointer fromRes(ures_get_By_চাবি(currencyRes.getAlias(), "from", nullptr, &localStatus));

                    যদি (U_SUCCESS(localStatus)) {
                        int32_t from_দৈর্ঘ্য = 0;
                        ধ্রুবক int32_t *fromArray = ures_getIntVector(fromRes.getAlias(), &from_দৈর্ঘ্য, &localStatus);
                        int64_t currDate64 = ((uint64_t)fromArray[0]) << 32;
                        currDate64 |= ((int64_t)fromArray[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                        fromDate = (UDate)currDate64;
                    }

                    // get to date
                    UDate toDate = U_DATE_MAX;
                    localStatus = U_ZERO_ERROR;
                    LocalUResourceBundlePointer toRes(ures_get_By_চাবি(currencyRes.getAlias(), "to", nullptr, &localStatus));

                    যদি (U_SUCCESS(localStatus)) {
                        int32_t to_দৈর্ঘ্য = 0;
                        ধ্রুবক int32_t *toArray = ures_getIntVector(toRes.getAlias(), &to_দৈর্ঘ্য, &localStatus);
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
            } অন্যথা {
                *status = localStatus;
            }
        }
    } অন্যথা {
        *status = localStatus;
    }
}

স্থির ধ্রুবক UEnumeration gEnumCurrencyList = {
    nullptr,
    nullptr,
    ucurr_closeCurrencyList,
    ucurr_গুনতি_Currency_List,
    uenum_unextDefault,
    ucurr_পরবর্তী_Currency_List,
    ucurr_resetCurrencyList
};
U_CDECL_END


স্থির শূন্য U_CALLCONV initIsoCodes(UErrorCode &status) {
    U_ASSERT(gIsoCodes == nullptr);
    ucln_common_registerCleanup(UCLN_COMMON_CURRENCY, currency_cleanup);

    LocalUHashtablePointer isoCodes(uhash_open(uhash_hashUChars, uhash_compareUChars, nullptr, &status));
    যদি (U_FAILURE(status)) {
        ফেরত;
    }
    uhash_set_মান_Deleter(isoCodes.getAlias(), deleteIsoCodeEntry);

    ucurr_createCurrencyList(isoCodes.getAlias(), &status);
    যদি (U_FAILURE(status)) {
        ফেরত;
    }
    gIsoCodes = isoCodes.orphan();  // Note: gIsoCodes is const. Once set up here it is never altered,
                                    //       and read only access is safe without synchronization.
}

স্থির শূন্য populateCurrSymbolsEquiv(icu::Hashtable *hash, UErrorCode &status) {
    যদি (U_FAILURE(status)) { ফেরত; }
    জন্য (ধ্রুবক auto& entry : unisets::kCurrencyEntries) {
        UnicodeString exemplar(entry.exemplar);
        ধ্রুবক UnicodeSet* set = unisets::get(entry.চাবি);
        যদি (set == nullptr) { ফেরত; }
        UnicodeSetIterator it(*set);
        যখন (it.next()) {
            UnicodeString value = it.getString();
            যদি (value == exemplar) {
                // No need to mark the exemplar character as an equivalent
                চালু;
            }
            makeEquivalent(exemplar, value, hash, status);
            যদি (U_FAILURE(status)) { ফেরত; }
        }
    }
}

স্থির শূন্য U_CALLCONV initCurrSymbolsEquiv() {
    U_ASSERT(gCurrSymbolsEquiv == nullptr);
    UErrorCode status = U_ZERO_ERROR;
    ucln_common_registerCleanup(UCLN_COMMON_CURRENCY, currency_cleanup);
    icu::Hashtable *অস্থায়ী = নতুন icu::Hashtable(status);
    যদি (অস্থায়ী == nullptr) {
        ফেরত;
    }
    যদি (U_FAILURE(status)) {
        delete অস্থায়ী;
        ফেরত;
    }
    অস্থায়ী->set_মান_Deleter(deleteUnicode);
    populateCurrSymbolsEquiv(অস্থায়ী, status);
    যদি (U_FAILURE(status)) {
        delete অস্থায়ী;
        ফেরত;
    }
    gCurrSymbolsEquiv = অস্থায়ী;
}

U_CAPI UBool U_EXPORT2
ucurr_isAvailable(ধ্রুবক char16_t* isoCode, UDate from, UDate to, UErrorCode* eErrorCode) {
    umtx_initOnce(gIsoCodesInitOnce, &initIsoCodes, *eErrorCode);
    যদি (U_FAILURE(*eErrorCode)) {
        ফেরত মিথ্যা;
    }

    IsoCodeEntry* ফলাফল = (IsoCodeEntry *) uhash_get(gIsoCodes, isoCode);
    যদি (ফলাফল == nullptr) {
        ফেরত মিথ্যা;
    } অন্যথা যদি (from > to) {
        *eErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত মিথ্যা;
    } অন্যথা যদি  ((from > ফলাফল->to) || (to < ফলাফল->from)) {
        ফেরত মিথ্যা;
    }
    ফেরত সত্য;
}

স্থির ধ্রুবক icu::Hashtable* getCurrSymbolsEquiv() {
    umtx_initOnce(gCurrSymbolsEquivInitOnce, &initCurrSymbolsEquiv);
    ফেরত gCurrSymbolsEquiv;
}

U_CAPI UEnumeration * U_EXPORT2
ucurr_openISOCurrencies(uint32_t currType, UErrorCode *pErrorCode) {
    UEnumeration *myEnum = nullptr;
    UCurrencyContext *myContext;

    myEnum = (UEnumeration*)uprv_malloc(আকার(UEnumeration));
    যদি (myEnum == nullptr) {
        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
        ফেরত nullptr;
    }
    uprv_memcpy(myEnum, &gEnumCurrencyList, আকার(UEnumeration));
    myContext = (UCurrencyContext*)uprv_malloc(আকার(UCurrencyContext));
    যদি (myContext == nullptr) {
        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
        uprv_free(myEnum);
        ফেরত nullptr;
    }
    myContext->currType = currType;
    myContext->listIdx = 0;
    myEnum->context = myContext;
    ফেরত myEnum;
}

U_CAPI int32_t U_EXPORT2
ucurr_গুনতি_Currencies(ধ্রুবক অক্ষর* locale, 
                 UDate date, 
                 UErrorCode* ec)
{
    int32_t curr_গুনতি = 0;

    যদি (ec != nullptr && U_SUCCESS(*ec)) 
    {
        // local variables
        UErrorCode localStatus = U_ZERO_ERROR;

        // get country or country_variant in `id'
        CharString id = idForLocale(locale, ec);

        যদি (U_FAILURE(*ec))
        {
            ফেরত 0;
        }

        // Remove variants, which is only needed for registration.
        অক্ষর *idDelim = strchr(id.তথ্য(), VAR_DELIM);
        যদি (idDelim)
        {
            id.truncate(idDelim - id.তথ্য());
        }

        // Look up the CurrencyMap element in the root bundle.
        UResourceBundle *rb = ures_openDirect(U_ICUDATA_CURR, CURRENCY_তথ্য, &localStatus);
        UResourceBundle *cm = ures_get_By_চাবি(rb, CURRENCY_MAP, rb, &localStatus);

        // Using the id derived from the local, get the currency data
        LocalUResourceBundlePointer countryArray(ures_get_By_চাবি(rb, id.তথ্য(), cm, &localStatus));

        // process each currency to see which one is valid for the given date
        যদি (U_SUCCESS(localStatus))
        {
            জন্য (int32_t i=0; i<ures_get_আকার(countryArray.getAlias()); i++)
            {
                // get the currency resource
                LocalUResourceBundlePointer currencyRes(ures_get_By_সূচক(countryArray.getAlias(), i, nullptr, &localStatus));

                // get the from date
                int32_t from_দৈর্ঘ্য = 0;
                LocalUResourceBundlePointer fromRes(ures_get_By_চাবি(currencyRes.getAlias(), "from", nullptr, &localStatus));
                ধ্রুবক int32_t *fromArray = ures_getIntVector(fromRes.getAlias(), &from_দৈর্ঘ্য, &localStatus);

                int64_t currDate64 = (int64_t)((uint64_t)(fromArray[0]) << 32);
                currDate64 |= ((int64_t)fromArray[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                UDate fromDate = (UDate)currDate64;

                যদি (ures_get_আকার(currencyRes.getAlias())> 2)
                {
                    int32_t to_দৈর্ঘ্য = 0;
                    LocalUResourceBundlePointer toRes(ures_get_By_চাবি(currencyRes.getAlias(), "to", nullptr, &localStatus));
                    ধ্রুবক int32_t *toArray = ures_getIntVector(toRes.getAlias(), &to_দৈর্ঘ্য, &localStatus);

                    currDate64 = (int64_t)toArray[0] << 32;
                    currDate64 |= ((int64_t)toArray[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                    UDate toDate = (UDate)currDate64;

                    যদি ((fromDate <= date) && (date < toDate))
                    {
                        curr_গুনতি++;
                    }
                }
                অন্যথা
                {
                    যদি (fromDate <= date)
                    {
                        curr_গুনতি++;
                    }
                }
            } // end For loop
        } // end if (U_SUCCESS(localStatus))

        // Check for errors
        যদি (*ec == U_ZERO_ERROR || localStatus != U_ZERO_ERROR)
        {
            // There is nothing to fallback to. 
            // Report the failure/warning if possible.
            *ec = localStatus;
        }

        যদি (U_SUCCESS(*ec))
        {
            // no errors
            ফেরত curr_গুনতি;
        }
    }

    // If we got here, either error code is invalid or
    // some argument passed is no good.
    ফেরত 0;
}

U_CAPI int32_t U_EXPORT2 
ucurr_forLocaleAndDate(ধ্রুবক অক্ষর* locale, 
                UDate date, 
                int32_t সূচক,
                char16_t* buff,
                int32_t buffCapacity, 
                UErrorCode* ec)
{
    int32_t resLen = 0;
	int32_t curr_সূচক = 0;
    ধ্রুবক char16_t* s = nullptr;

    যদি (ec != nullptr && U_SUCCESS(*ec))
    {
        // check the arguments passed
        যদি ((buff && buffCapacity) || !buffCapacity )
        {
            // local variables
            UErrorCode localStatus = U_ZERO_ERROR;

            // get country or country_variant in `id'
            CharString id = idForLocale(locale, ec);
            যদি (U_FAILURE(*ec))
            {
                ফেরত 0;
            }

            // Remove variants, which is only needed for registration.
            অক্ষর *idDelim = strchr(id.তথ্য(), VAR_DELIM);
            যদি (idDelim)
            {
                id.truncate(idDelim - id.তথ্য());
            }

            // Look up the CurrencyMap element in the root bundle.
            UResourceBundle *rb = ures_openDirect(U_ICUDATA_CURR, CURRENCY_তথ্য, &localStatus);
            UResourceBundle *cm = ures_get_By_চাবি(rb, CURRENCY_MAP, rb, &localStatus);

            // Using the id derived from the local, get the currency data
            LocalUResourceBundlePointer countryArray(ures_get_By_চাবি(rb, id.তথ্য(), cm, &localStatus));

            // process each currency to see which one is valid for the given date
            যৌক্তিক matchFound = মিথ্যা;
            যদি (U_SUCCESS(localStatus))
            {
                যদি ((সূচক <= 0) || (সূচক> ures_get_আকার(countryArray.getAlias())))
                {
                    // requested index is out of bounds
                    ফেরত 0;
                }

                জন্য (int32_t i=0; i<ures_get_আকার(countryArray.getAlias()); i++)
                {
                    // get the currency resource
                    LocalUResourceBundlePointer currencyRes(ures_get_By_সূচক(countryArray.getAlias(), i, nullptr, &localStatus));
                    s = ures_get_String_By_চাবি(currencyRes.getAlias(), "id", &resLen, &localStatus);

                    // get the from date
                    int32_t from_দৈর্ঘ্য = 0;
                    LocalUResourceBundlePointer fromRes(ures_get_By_চাবি(currencyRes.getAlias(), "from", nullptr, &localStatus));
                    ধ্রুবক int32_t *fromArray = ures_getIntVector(fromRes.getAlias(), &from_দৈর্ঘ্য, &localStatus);

                    int64_t currDate64 = (int64_t)((uint64_t)fromArray[0] << 32);
                    currDate64 |= ((int64_t)fromArray[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                    UDate fromDate = (UDate)currDate64;

                    যদি (ures_get_আকার(currencyRes.getAlias()) > 2)
                    {
                        int32_t to_দৈর্ঘ্য = 0;
                        LocalUResourceBundlePointer toRes(ures_get_By_চাবি(currencyRes.getAlias(), "to", nullptr, &localStatus));
                        ধ্রুবক int32_t *toArray = ures_getIntVector(toRes.getAlias(), &to_দৈর্ঘ্য, &localStatus);

                        currDate64 = (int64_t)toArray[0] << 32;
                        currDate64 |= ((int64_t)toArray[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                        UDate toDate = (UDate)currDate64;

                        যদি ((fromDate <= date) && (date < toDate))
                        {
                            curr_সূচক++;
                            যদি (curr_সূচক == সূচক)
                            {
                                matchFound = সত্য;
                            }
                        }
                    }
                    অন্যথা
                    {
                        যদি (fromDate <= date)
                        {
                            curr_সূচক++;
                            যদি (curr_সূচক == সূচক)
                            {
                                matchFound = সত্য;
                            }
                        }
                    }
                    // check for loop exit
                    যদি (matchFound)
                    {
                        বিরতি;
                    }

                } // end For loop
            }

            // Check for errors
            যদি (*ec == U_ZERO_ERROR || localStatus != U_ZERO_ERROR)
            {
                // There is nothing to fallback to. 
                // Report the failure/warning if possible.
                *ec = localStatus;
            }

            যদি (U_SUCCESS(*ec))
            {
                // no errors
                যদি((buffCapacity> resLen) && matchFound)
                {
                    // write out the currency value
                    u_strcpy(buff, s);
                }
                অন্যথা
                {
                    ফেরত 0;
                }
            }

            // return null terminated currency string
            ফেরত u_terminateUChars(buff, buffCapacity, resLen, ec);
        }
        অন্যথা
        {
            // illegal argument encountered
            *ec = U_ILLEGAL_ARGUMENT_ERROR;
        }

    }

    // If we got here, either error code is invalid or
    // some argument passed is no good.
    ফেরত resLen;
}

স্থির ধ্রুবক UEnumeration default_Keyword_মানসমূহ = {
    nullptr,
    nullptr,
    ulist_close_keyword_মানসমূহ_iterator,
    ulist_গুনতি_keyword_মানসমূহ,
    uenum_unextDefault,
    ulist_পরবর্তী_keyword_মান, 
    ulist_reset_keyword_মানসমূহ_iterator
};

U_CAPI UEnumeration *U_EXPORT2 ucurr_get_Keyword_মানসমূহ_For_Locale(ধ্রুবক অক্ষর *চাবি, ধ্রুবক অক্ষর *locale, UBool commonlyUsed, UErrorCode* status) {
    // Resolve region
    CharString prefRegion = ulocimp_get_Region_For_Supplemental_তথ্য(locale, সত্য, *status);

    // Read value from supplementalData
    UList *মানসমূহ = ulist_createEmptyList(status);
    UList *other_মানসমূহ = ulist_createEmptyList(status);
    UEnumeration *en = (UEnumeration *)uprv_malloc(আকার(UEnumeration));
    যদি (U_FAILURE(*status) || en == nullptr) {
        যদি (en == nullptr) {
            *status = U_MEMORY_ALLOCATION_ERROR;
        } অন্যথা {
            uprv_free(en);
        }
        ulist_deleteList(মানসমূহ);
        ulist_deleteList(other_মানসমূহ);
        ফেরত nullptr;
    }
    memcpy(en, &default_Keyword_মানসমূহ, আকার(UEnumeration));
    en->context = মানসমূহ;
    
    UResourceBundle* rb = ures_openDirect(U_ICUDATA_CURR, "supplementalData", status);
    LocalUResourceBundlePointer bundle(ures_get_By_চাবি(rb, "CurrencyMap", rb, status));
    স্তূপ_UResource_Bundle bundlekey, regbndl, curbndl, to;
    
    যখন (U_SUCCESS(*status) && ures_has_পরবর্তী(bundle.getAlias())) {
        ures_get_পরবর্তী_Resource(bundle.getAlias(), bundlekey.getAlias(), status);
        যদি (U_FAILURE(*status)) {
            বিরতি;
        }
        ধ্রুবক অক্ষর *region = ures_get_চাবি(bundlekey.getAlias());
        UBool isPrefRegion = prefRegion == region;
        যদি (!isPrefRegion && commonlyUsed) {
            // With commonlyUsed=true, we do not put
            // currencies for other regions in the
            // result list.
            চালু;
        }
        ures_get_By_চাবি(bundle.getAlias(), region, regbndl.getAlias(), status);
        যদি (U_FAILURE(*status)) {
            বিরতি;
        }
        যখন (U_SUCCESS(*status) && ures_has_পরবর্তী(regbndl.getAlias())) {
            ures_get_পরবর্তী_Resource(regbndl.getAlias(), curbndl.getAlias(), status);
            যদি (ures_getType(curbndl.getAlias()) != URES_TABLE) {
                // Currently, an empty ARRAY is mixed in.
                চালু;
            }
            অক্ষর *curID = (অক্ষর *)uprv_malloc(আকার(অক্ষর) * ULOC_KEYWORDS_CAPACITY);
            যদি (curID == nullptr) {
                *status = U_MEMORY_ALLOCATION_ERROR;
                বিরতি;
            }
            int32_t curIDLength = ULOC_KEYWORDS_CAPACITY;

#যদি U_CHARSET_FAMILY==U_ASCII_FAMILY
            ures_get_UTF8_String_By_চাবি(curbndl.getAlias(), "id", curID, &curIDLength, সত্য, status);
            /* optimize - use the utf-8 string */
#অন্যথা
            {
                       ধ্রুবক char16_t* defString = ures_get_String_By_চাবি(curbndl.getAlias(), "id", &curIDLength, status);
                       যদি(U_SUCCESS(*status)) {
			   যদি(curIDLength+1 > ULOC_KEYWORDS_CAPACITY) {
				*status = U_বাফার_OVERFLOW_ERROR;
			   } অন্যথা {
                           	u_UCharsToChars(defString, curID, curIDLength+1);
			   }
                       }
            }
#endif	

            যদি (U_FAILURE(*status)) {
                বিরতি;
            }
            UBool hasTo = মিথ্যা;
            ures_get_By_চাবি(curbndl.getAlias(), "to", to.getAlias(), status);
            যদি (U_FAILURE(*status)) {
                // Do nothing here...
                *status = U_ZERO_ERROR;
            } অন্যথা {
                hasTo = সত্য;
            }
            যদি (isPrefRegion && !hasTo && !ulist_containsString(মানসমূহ, curID, (int32_t)uprv_strlen(curID))) {
                // Currently active currency for the target country
                ulist_addItemEndList(মানসমূহ, curID, সত্য, status);
            } অন্যথা যদি (!ulist_containsString(other_মানসমূহ, curID, (int32_t)uprv_strlen(curID)) && !commonlyUsed) {
                ulist_addItemEndList(other_মানসমূহ, curID, সত্য, status);
            } অন্যথা {
                uprv_free(curID);
            }
        }
        
    }
    যদি (U_SUCCESS(*status)) {
        যদি (commonlyUsed) {
            যদি (ulist_get_List_আকার(মানসমূহ) == 0) {
                // This could happen if no valid region is supplied in the input
                // locale. In this case, we use the CLDR's default.
                uenum_close(en);
                en = ucurr_get_Keyword_মানসমূহ_For_Locale(চাবি, "und", সত্য, status);
            }
        } অন্যথা {
            // Consolidate the list
            অক্ষর *value = nullptr;
            ulist_resetList(other_মানসমূহ);
            যখন ((value = (অক্ষর *)ulist_get_পরবর্তী(other_মানসমূহ)) != nullptr) {
                যদি (!ulist_containsString(মানসমূহ, value, (int32_t)uprv_strlen(value))) {
                    অক্ষর *tmp_মান = (অক্ষর *)uprv_malloc(আকার(অক্ষর) * ULOC_KEYWORDS_CAPACITY);
                    যদি (tmp_মান == nullptr) {
                        *status = U_MEMORY_ALLOCATION_ERROR;
                        বিরতি;
                    }
                    uprv_memcpy(tmp_মান, value, uprv_strlen(value) + 1);
                    ulist_addItemEndList(মানসমূহ, tmp_মান, সত্য, status);
                    যদি (U_FAILURE(*status)) {
                        বিরতি;
                    }
                }
            }
        }
        ulist_resetList((UList *)(en->context));
    } অন্যথা {
        ulist_deleteList(মানসমূহ);
        uprv_free(en);
        মানসমূহ = nullptr;
        en = nullptr;
    }
    ulist_deleteList(other_মানসমূহ);
    ফেরত en;
}


U_CAPI int32_t U_EXPORT2
ucurr_getNumericCode(ধ্রুবক char16_t* currency) {
    int32_t code = 0;
    যদি (currency && u_strlen(currency) == ISO_CURRENCY_CODE_দৈর্ঘ্য) {
        UErrorCode status = U_ZERO_ERROR;

        UResourceBundle* bundle = ures_openDirect(nullptr, "currencyNumericCodes", &status);
        LocalUResourceBundlePointer codeMap(ures_get_By_চাবি(bundle, "codeMap", bundle, &status));
        যদি (U_SUCCESS(status)) {
            অক্ষর alphaCode[ISO_CURRENCY_CODE_দৈর্ঘ্য+1];
            myUCharsToChars(alphaCode, currency);
            T_CString_toUpperCase(alphaCode);
            ures_get_By_চাবি(codeMap.getAlias(), alphaCode, codeMap.getAlias(), &status);
            পূর্ণসংখ্যা tmpCode = ures_getInt(codeMap.getAlias(), &status);
            যদি (U_SUCCESS(status)) {
                code = tmpCode;
            }
        }
    }
    ফেরত code;
}
#endif /* #if !UCONFIG_NO_FORMATTING */

//eof
