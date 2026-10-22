// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
* Copyright (c) 2002-2016, International Business Machines
* Corporation and others.  All Rights Reserved.
**********************************************************************
*/

#समावेश "unicode/utypes.h"

#अगर !UCONFIG_NO_FORMATTING

#समावेश <utility>

#समावेश "unicode/ucurr.h"
#समावेश "unicode/locid.h"
#समावेश "unicode/ures.h"
#समावेश "unicode/ustring.h"
#समावेश "unicode/parsepos.h"
#समावेश "unicode/uniset.h"
#समावेश "unicode/usetiter.h"
#समावेश "unicode/utf16.h"
#समावेश "ustr_imp.h"
#समावेश "charstr.h"
#समावेश "cmemory.h"
#समावेश "cstring.h"
#समावेश "static_unicode_sets.h"
#समावेश "uassert.h"
#समावेश "umutex.h"
#समावेश "ucln_cmn.h"
#समावेश "uenumimp.h"
#समावेश "uhash.h"
#समावेश "hash.h"
#समावेश "uinvchar.h"
#समावेश "uresimp.h"
#समावेश "ulist.h"
#समावेश "uresimp.h"
#समावेश "ureslocs.h"
#समावेश "ulocimp.h"

using namespace icu;

//#define UCURR_DEBUG_EQUIV 1
#अगर_घोषित UCURR_DEBUG_EQUIV
#समावेश "stdio.h"
#पूर्ण_अगर
//#define UCURR_DEBUG 1
#अगर_घोषित UCURR_DEBUG
#समावेश "stdio.h"
#पूर्ण_अगर

प्रकार काष्ठा IsoCodeEntry {
    स्थिर char16_t *isoCode; /* const because it's a reference to a resource bundle string. */
    UDate from;
    UDate to;
} IsoCodeEntry;

//------------------------------------------------------------
// Constants

// Default currency meta data of last resort.  We try to use the
// defaults encoded in the meta data resource bundle.  If there is a
// configuration/build error and these are not available, we use these
// hard-coded defaults (which should be identical).
अटल स्थिर int32_t LAST_RESORT_तथ्य[] = { 2, 0, 2, 0 };

// POW10[i] = 10^i, i=0..MAX_POW10
अटल स्थिर int32_t POW10[] = { 1, 10, 100, 1000, 10000, 100000,
                                 1000000, 10000000, 100000000, 1000000000 };

अटल स्थिर int32_t MAX_POW10 = UPRV_LENGTHOF(POW10) - 1;

#घोषणा ISO_CURRENCY_CODE_लंबाई 3

//------------------------------------------------------------
// Resource tags
//

अटल स्थिर अक्षर CURRENCY_तथ्य[] = "supplementalData";
// Tag for meta-data, in root.
अटल स्थिर अक्षर CURRENCY_META[] = "CurrencyMeta";

// Tag for map from countries to currencies, in root.
अटल स्थिर अक्षर CURRENCY_MAP[] = "CurrencyMap";

// Tag for default meta-data, in CURRENCY_META
अटल स्थिर अक्षर DEFAULT_META[] = "DEFAULT";

// Variant delimiter
अटल स्थिर अक्षर VAR_DELIM = '_';

// Tag for localized display names (symbols) of currencies
अटल स्थिर अक्षर CURRENCIES[] = "Currencies";
अटल स्थिर अक्षर CURRENCIES_NARROW[] = "Currencies%narrow";
अटल स्थिर अक्षर CURRENCIES_FORMAL[] = "Currencies%formal";
अटल स्थिर अक्षर CURRENCIES_VARIANT[] = "Currencies%variant";
अटल स्थिर अक्षर CURRENCYPLURALS[] = "CurrencyPlurals";

// ISO codes mapping table
अटल स्थिर UHashtable* gIsoCodes = nullptr;
अटल icu::UInitOnce gIsoCodesInitOnce {};

// Currency symbol equivalances
अटल स्थिर icu::Hashtable* gCurrSymbolsEquiv = nullptr;
अटल icu::UInitOnce gCurrSymbolsEquivInitOnce {};

U_NAMESPACE_BEGIN

// EquivIterator iterates over all strings that are equivalent to a given
// string, s. Note that EquivIterator will never yield s itself.
श्रेणी EquivIterator : खुला icu::UMemory {
खुला:
    // Constructor. hash stores the equivalence relationships; s is the string
    // for which we find equivalent strings.
    अंतरभूत EquivIterator(स्थिर icu::Hashtable& hash, स्थिर icu::UnicodeString& s)
        : _hash(hash) { 
        _start = _current = &s;
    }
    अंतरभूत ~EquivIterator() { }

    // next returns the next equivalent string or nullptr if there are no more.
    // If s has no equivalent strings, next returns nullptr on the first call.
    स्थिर icu::UnicodeString *next();
निजी:
    स्थिर icu::Hashtable& _hash;
    स्थिर icu::UnicodeString* _start;
    स्थिर icu::UnicodeString* _current;
};

स्थिर icu::UnicodeString *
EquivIterator::next() {
    स्थिर icu::UnicodeString* अग्र_2 = static_cast<स्थिर icu::UnicodeString*>(_hash.लो(*_current));
    अगर (अग्र_2 == nullptr) {
        U_ASSERT(_current == _start);
        वापस nullptr;
    }
    अगर (*अग्र_2 == *_start) {
        वापस nullptr;
    }
    _current = अग्र_2;
    वापस अग्र_2;
}

U_NAMESPACE_END

// makeEquivalent makes lhs and rhs equivalent by updating the equivalence
// relations in hash accordingly.
अटल व्योम makeEquivalent(
    स्थिर icu::UnicodeString &lhs,
    स्थिर icu::UnicodeString &rhs,
    icu::Hashtable* hash, UErrorCode &status) {
    अगर (U_FAILURE(status)) {
        वापस;
    }
    अगर (lhs == rhs) {
        // already equivalent
        वापस;
    }
    icu::EquivIterator बायाँ_Iter(*hash, lhs);
    icu::EquivIterator दायाँ_Iter(*hash, rhs);
    स्थिर icu::UnicodeString *first_बायाँ = बायाँ_Iter.next();
    स्थिर icu::UnicodeString *first_दायाँ = दायाँ_Iter.next();
    स्थिर icu::UnicodeString *अग्र_बायाँ = first_बायाँ;
    स्थिर icu::UnicodeString *अग्र_दायाँ = first_दायाँ;
    जबतक (अग्र_बायाँ != nullptr && अग्र_दायाँ != nullptr) {
        अगर (*अग्र_बायाँ == rhs || *अग्र_दायाँ == lhs) {
            // Already equivalent
            वापस;
        }
        अग्र_बायाँ = बायाँ_Iter.next();
        अग्र_दायाँ = दायाँ_Iter.next();
    }
    // Not equivalent. Must join.
    icu::UnicodeString *new_First_बायाँ;
    icu::UnicodeString *new_First_दायाँ;
    अगर (first_दायाँ == nullptr && first_बायाँ == nullptr) {
        // Neither lhs or rhs belong to an equivalence circle, so we form
        // a new equivalnce circle of just lhs and rhs.
        new_First_बायाँ = नया icu::UnicodeString(rhs);
        new_First_दायाँ = नया icu::UnicodeString(lhs);
    } अन्यथा अगर (first_दायाँ == nullptr) {
        // lhs belongs to an equivalence circle, but rhs does not, so we link
        // rhs into lhs' circle.
        new_First_बायाँ = नया icu::UnicodeString(rhs);
        new_First_दायाँ = नया icu::UnicodeString(*first_बायाँ);
    } अन्यथा अगर (first_बायाँ == nullptr) {
        // rhs belongs to an equivlance circle, but lhs does not, so we link
        // lhs into rhs' circle.
        new_First_बायाँ = नया icu::UnicodeString(*first_दायाँ);
        new_First_दायाँ = नया icu::UnicodeString(lhs);
    } अन्यथा {
        // Both lhs and rhs belong to different equivalnce circles. We link
        // them together to form one single, larger equivalnce circle.
        new_First_बायाँ = नया icu::UnicodeString(*first_दायाँ);
        new_First_दायाँ = नया icu::UnicodeString(*first_बायाँ);
    }
    अगर (new_First_बायाँ == nullptr || new_First_दायाँ == nullptr) {
        मिटाओ new_First_बायाँ;
        मिटाओ new_First_दायाँ;
        status = U_MEMORY_ALLOCATION_ERROR;
        वापस;
    }
    hash->दो(lhs, (व्योम *) new_First_बायाँ, status);
    hash->दो(rhs, (व्योम *) new_First_दायाँ, status);
}

// countEquivalent counts how many strings are equivalent to s.
// hash stores all the equivalnce relations.
// countEquivalent does not include s itself in the count.
अटल int32_t गिनती_Equivalent(स्थिर icu::Hashtable &hash, स्थिर icu::UnicodeString &s) {
    int32_t परिणाम = 0;
    icu::EquivIterator iter(hash, s);
    जबतक (iter.next() != nullptr) {
        ++परिणाम;
    }
#अगर_घोषित UCURR_DEBUG_EQUIV
 {
   अक्षर tmp[200];
   s.extract(0,s.length(),tmp, "UTF-8");
   म_लिखो("CountEquivalent('%s') = %d\n", tmp, परिणाम);
 }
#पूर्ण_अगर
    वापस परिणाम;
}

अटल स्थिर icu::Hashtable* getCurrSymbolsEquiv();

//------------------------------------------------------------
// Code

/**
 * Cleanup callback func
 */
अटल UBool U_CALLCONV 
isoCodes_cleanup()
{
    अगर (gIsoCodes != nullptr) {
        uhash_close(const_cast<UHashtable *>(gIsoCodes));
        gIsoCodes = nullptr;
    }
    gIsoCodesInitOnce.reset();
    वापस true;
}

/**
 * Cleanup callback func
 */
अटल UBool U_CALLCONV 
currSymbolsEquiv_cleanup()
{
    मिटाओ const_cast<icu::Hashtable *>(gCurrSymbolsEquiv);
    gCurrSymbolsEquiv = nullptr;
    gCurrSymbolsEquivInitOnce.reset();
    वापस true;
}

/**
 * Deleter for IsoCodeEntry
 */
अटल व्योम U_CALLCONV
deleteIsoCodeEntry(व्योम *obj) {
    IsoCodeEntry* entry = static_cast<IsoCodeEntry*>(obj);
    uprv_free(entry);
}

/**
 * Deleter for gCurrSymbolsEquiv.
 */
अटल व्योम U_CALLCONV
deleteUnicode(व्योम *obj) {
    icu::UnicodeString* entry = static_cast<icu::UnicodeString*>(obj);
    मिटाओ entry;
}

/**
 * Unfortunately, we have to convert the char16_t* currency code to char*
 * to use it as a resource key.
 */
अटल अंतरभूत अक्षर*
myUCharsToChars(अक्षर* परिणाम_Of_Len4, स्थिर char16_t* currency) {
    u_UCharsToChars(currency, परिणाम_Of_Len4, ISO_CURRENCY_CODE_लंबाई);
    परिणाम_Of_Len4[ISO_CURRENCY_CODE_लंबाई] = 0;
    वापस परिणाम_Of_Len4;
}

/**
 * Internal function to look up currency data.  Result is an array of
 * four integers.  The first is the fraction digits.  The second is the
 * rounding increment, or 0 if none.  The rounding increment is in
 * units of 10^(-fraction_digits).  The third and fourth are the same
 * except that they are those used in cash transactions ( cashDigits
 * and cashRounding ).
 */
अटल स्थिर int32_t*
find_Meta_तथ्य(स्थिर char16_t* currency, UErrorCode& ec) {

    अगर (currency == nullptr || *currency == 0) {
        अगर (U_SUCCESS(ec)) {
            ec = U_ILLEGAL_ARGUMENT_ERROR;
        }
        वापस LAST_RESORT_तथ्य;
    }

    // Get CurrencyMeta resource out of root locale file.  [This may
    // move out of the root locale file later; if it does, update this
    // code.]
    UResourceBundle* currency_तथ्य = ures_openDirect(U_ICUDATA_CURR, CURRENCY_तथ्य, &ec);
    LocalUResourceBundlePointer currencyMeta(ures_get_By_कुंजी(currency_तथ्य, CURRENCY_META, currency_तथ्य, &ec));

    अगर (U_FAILURE(ec)) {
        // Config/build error; return hard-coded defaults
        वापस LAST_RESORT_तथ्य;
    }

    // Look up our currency, or if that's not available, then DEFAULT
    अक्षर buf[ISO_CURRENCY_CODE_लंबाई+1];
    UErrorCode ec2 = U_ZERO_ERROR; // local error code: soft failure
    LocalUResourceBundlePointer rb(ures_get_By_कुंजी(currencyMeta.getAlias(), myUCharsToChars(buf, currency), nullptr, &ec2));
      अगर (U_FAILURE(ec2)) {
        rb.adoptInstead(ures_get_By_कुंजी(currencyMeta.getAlias(),DEFAULT_META, nullptr, &ec));
        अगर (U_FAILURE(ec)) {
            // Config/build error; return hard-coded defaults
            वापस LAST_RESORT_तथ्य;
        }
    }

    int32_t len;
    स्थिर int32_t *तथ्य = ures_getIntVector(rb.getAlias(), &len, &ec);
    अगर (U_FAILURE(ec) || len != 4) {
        // Config/build error; return hard-coded defaults
        अगर (U_SUCCESS(ec)) {
            ec = U_INVALID_FORMAT_ERROR;
        }
        वापस LAST_RESORT_तथ्य;
    }

    वापस तथ्य;
}

// -------------------------------------

अटल CharString
idForLocale(स्थिर अक्षर* locale, UErrorCode* ec)
{
    वापस ulocimp_get_Region_For_Supplemental_तथ्य(locale, false, *ec);
}

// ------------------------------------------
//
// Registration
//
//-------------------------------------------

// don't use ICUService since we don't need fallback

U_CDECL_BEGIN
अटल UBool U_CALLCONV currency_cleanup();
U_CDECL_END

#अगर !UCONFIG_NO_SERVICE
काष्ठा CReg;

अटल UMutex gCRegLock;
अटल CReg* g_CReg_शीर्ष = nullptr;

काष्ठा CReg : खुला icu::UMemory {
    CReg *next;
    char16_t iso[ISO_CURRENCY_CODE_लंबाई+1];
    अक्षर  id[ULOC_FULLNAME_CAPACITY];

    CReg(स्थिर char16_t* _iso, स्थिर अक्षर* _id)
        : next(nullptr)
    {
        uprv_strncpy(id, _id, माप(id)-1);
        id[माप(id)-1] = 0;
        u_memcpy(iso, _iso, ISO_CURRENCY_CODE_लंबाई);
        iso[ISO_CURRENCY_CODE_लंबाई] = 0;
    }

    अटल UCurr_Registry_कुंजी reg(स्थिर char16_t* _iso, स्थिर अक्षर* _id, UErrorCode* status)
    {
        अगर (status && U_SUCCESS(*status) && _iso && _id) {
            CReg* n = नया CReg(_iso, _id);
            अगर (n) {
                umtx_lock(&gCRegLock);
                अगर (!g_CReg_शीर्ष) {
                    /* register for the first time */
                    ucln_common_registerCleanup(UCLN_COMMON_CURRENCY, currency_cleanup);
                }
                n->next = g_CReg_शीर्ष;
                g_CReg_शीर्ष = n;
                umtx_unlock(&gCRegLock);
                वापस n;
            }
            *status = U_MEMORY_ALLOCATION_ERROR;
        }
        वापस nullptr;
    }

    अटल UBool unreg(UCurr_Registry_कुंजी कुंजी) {
        UBool found = false;
        umtx_lock(&gCRegLock);

        CReg** p = &g_CReg_शीर्ष;
        जबतक (*p) {
            अगर (*p == कुंजी) {
                *p = ((CReg*)कुंजी)->next;
                मिटाओ (CReg*)कुंजी;
                found = true;
                अवरोध;
            }
            p = &((*p)->next);
        }

        umtx_unlock(&gCRegLock);
        वापस found;
    }

    अटल स्थिर char16_t* लो(स्थिर अक्षर* id) {
        स्थिर char16_t* परिणाम = nullptr;
        umtx_lock(&gCRegLock);
        CReg* p = g_CReg_शीर्ष;

        /* register cleanup of the mutex */
        ucln_common_registerCleanup(UCLN_COMMON_CURRENCY, currency_cleanup);
        जबतक (p) {
            अगर (uprv_strcmp(id, p->id) == 0) {
                परिणाम = p->iso;
                अवरोध;
            }
            p = p->next;
        }
        umtx_unlock(&gCRegLock);
        वापस परिणाम;
    }

    /* This doesn't need to be thread safe. It's for u_cleanup only. */
    अटल व्योम cleanup() {
        जबतक (g_CReg_शीर्ष) {
            CReg* n = g_CReg_शीर्ष;
            g_CReg_शीर्ष = g_CReg_शीर्ष->next;
            मिटाओ n;
        }
    }
};

// -------------------------------------

U_CAPI UCurr_Registry_कुंजी U_EXPORT2
ucurr_register(स्थिर char16_t* isoCode, स्थिर अक्षर* locale, UErrorCode *status)
{
    अगर (status && U_SUCCESS(*status)) {
        CharString id = idForLocale(locale, status);
        वापस CReg::reg(isoCode, id.तथ्य(), status);
    }
    वापस nullptr;
}

// -------------------------------------

U_CAPI UBool U_EXPORT2
ucurr_unregister(UCurr_Registry_कुंजी कुंजी, UErrorCode* status)
{
    अगर (status && U_SUCCESS(*status)) {
        वापस CReg::unreg(कुंजी);
    }
    वापस false;
}
#पूर्ण_अगर /* UCONFIG_NO_SERVICE */

// -------------------------------------

/**
 * Release all static memory held by currency.
 */
/*The declaration here is needed so currency_cleanup()
 * can call this function.
 */
अटल UBool U_CALLCONV
currency_cache_cleanup();

U_CDECL_BEGIN
अटल UBool U_CALLCONV currency_cleanup() {
#अगर !UCONFIG_NO_SERVICE
    CReg::cleanup();
#पूर्ण_अगर
    /*
     * There might be some cached currency data or isoCodes data.
     */
    currency_cache_cleanup();
    isoCodes_cleanup();
    currSymbolsEquiv_cleanup();

    वापस true;
}
U_CDECL_END

// -------------------------------------

U_CAPI int32_t U_EXPORT2
ucurr_forLocale(स्थिर अक्षर* locale,
                char16_t* buff,
                int32_t buffCapacity,
                UErrorCode* ec) {
    अगर (U_FAILURE(*ec)) { वापस 0; }
    अगर (buffCapacity < 0 || (buff == nullptr && buffCapacity > 0)) {
        *ec = U_ILLEGAL_ARGUMENT_ERROR;
        वापस 0;
    }

    UErrorCode localStatus = U_ZERO_ERROR;
    CharString currency = ulocimp_get_Keyword_मूल्य(locale, "currency", localStatus);
    int32_t resLen = currency.length();

    अगर (U_SUCCESS(localStatus) && resLen == 3 && uprv_isInvariantString(currency.तथ्य(), resLen)) {
        अगर (resLen < buffCapacity) {
            T_CString_toUpperCase(currency.तथ्य());
            u_charsToUChars(currency.तथ्य(), buff, resLen);
        }
        वापस u_terminateUChars(buff, buffCapacity, resLen, ec);
    }

    // get country or country_variant in `id'
    CharString id = idForLocale(locale, ec);
    अगर (U_FAILURE(*ec)) {
        वापस 0;
    }

#अगर !UCONFIG_NO_SERVICE
    स्थिर char16_t* परिणाम = CReg::लो(id.तथ्य());
    अगर (परिणाम) {
        अगर(buffCapacity > u_strlen(परिणाम)) {
            u_strcpy(buff, परिणाम);
        }
        resLen = u_strlen(परिणाम);
        वापस u_terminateUChars(buff, buffCapacity, resLen, ec);
    }
#पूर्ण_अगर
    // Remove variants, which is only needed for registration.
    अक्षर *idDelim = uprv_strchr(id.तथ्य(), VAR_DELIM);
    अगर (idDelim) {
        id.truncate(idDelim - id.तथ्य());
    }

    स्थिर char16_t* s = nullptr;  // Currency code from data file.
    अगर (id.isEmpty()) {
        // No point looking in the data for an empty string.
        // This is what we would get.
        localStatus = U_MISSING_RESOURCE_ERROR;
    } अन्यथा {
        // Look up the CurrencyMap element in the root bundle.
        localStatus = U_ZERO_ERROR;
        UResourceBundle *rb = ures_openDirect(U_ICUDATA_CURR, CURRENCY_तथ्य, &localStatus);
        UResourceBundle *cm = ures_get_By_कुंजी(rb, CURRENCY_MAP, rb, &localStatus);
        LocalUResourceBundlePointer countryArray(ures_get_By_कुंजी(rb, id.तथ्य(), cm, &localStatus));
        // https://unicode-org.atlassian.net/browse/ICU-21997
        // Prefer to use currencies that are legal tender.
        अगर (U_SUCCESS(localStatus)) {
            int32_t array_लंबाई = ures_get_आकार(countryArray.getAlias());
            क्रम (int32_t i = 0; i < array_लंबाई; ++i) {
                LocalUResourceBundlePointer currencyReq(
                    ures_get_By_सूचकांक(countryArray.getAlias(), i, nullptr, &localStatus));
                // The currency is legal tender if it is *not* marked with tender{"false"}.
                UErrorCode tenderStatus = localStatus;
                स्थिर char16_t *tender =
                    ures_get_String_By_कुंजी(currencyReq.getAlias(), "tender", nullptr, &tenderStatus);
                bool isTender = U_FAILURE(tenderStatus) || u_strcmp(tender, u"false") != 0;
                अगर (!isTender && s != nullptr) {
                    // We already have a non-tender currency. Ignore all following non-tender ones.
                    जारी;
                }
                // Fetch the currency code.
                s = ures_get_String_By_कुंजी(currencyReq.getAlias(), "id", &resLen, &localStatus);
                अगर (isTender) {
                    अवरोध;
                }
            }
            अगर (U_SUCCESS(localStatus) && s == nullptr) {
                localStatus = U_MISSING_RESOURCE_ERROR;
            }
        }
    }

    अगर ((U_FAILURE(localStatus)) && म_अक्षर(id.तथ्य(), '_') != nullptr) {
        // We don't know about it.  Check to see if we support the variant.
        CharString parent = ulocimp_getParent(locale, *ec);
        *ec = U_USING_FALLBACK_WARNING;
        // TODO: Loop over the parent rather than recursing and
        // looking again for a currency keyword.
        वापस ucurr_forLocale(parent.तथ्य(), buff, buffCapacity, ec);
    }
    अगर (*ec == U_ZERO_ERROR || localStatus != U_ZERO_ERROR) {
        // There is nothing to fallback to. Report the failure/warning if possible.
        *ec = localStatus;
    }
    अगर (U_SUCCESS(*ec)) {
        अगर(buffCapacity > resLen) {
            u_strcpy(buff, s);
        }
    }
    वापस u_terminateUChars(buff, buffCapacity, resLen, ec);
}

// end registration

/**
 * Modify the given locale name by removing the rightmost _-delimited
 * element.  If there is none, empty the string ("" == root).
 * NOTE: The string "root" is not recognized; do not use it.
 * @return true if the fallback happened; false if locale is already
 * root ("").
 */
अटल UBool fallback(CharString& loc) {
    अगर (loc.isEmpty()) {
        वापस false;
    }
    UErrorCode status = U_ZERO_ERROR;
    अगर (loc == "en_GB") {
        // HACK: See #13368.  We need "en_GB" to fall back to "en_001" instead of "en"
        // in order to consume the correct data strings.  This hack will be removed
        // when proper data sink loading is implemented here.
        loc.truncate(3);
        loc.append("001", status);
    } अन्यथा {
        loc = ulocimp_getParent(loc.तथ्य(), status);
    }
 /*
    char *i = uprv_strrchr(loc, '_');
    if (i == nullptr) {
        i = loc;
    }
    *i = 0;
 */
    वापस true;
}


U_CAPI स्थिर char16_t* U_EXPORT2
ucurr_getName(स्थिर char16_t* currency U_LIFETIME_BOUND,
              स्थिर अक्षर* locale,
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

    अगर (U_FAILURE(*ec)) {
        वापस nullptr;
    }

    int32_t choice = (int32_t) nameStyle;
    अगर (choice < 0 || choice > 4) {
        *ec = U_ILLEGAL_ARGUMENT_ERROR;
        वापस nullptr;
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

    अगर (locale == nullptr) {
        locale = uloc_getDefault();
    }
    CharString loc = ulocimp_getName(locale, ec2);
    अगर (U_FAILURE(ec2)) {
        *ec = U_ILLEGAL_ARGUMENT_ERROR;
        वापस nullptr;
    }

    अक्षर buf[ISO_CURRENCY_CODE_लंबाई+1];
    myUCharsToChars(buf, currency);
    
    /* Normalize the keyword value to uppercase */
    T_CString_toUpperCase(buf);
    
    स्थिर char16_t* s = nullptr;
    ec2 = U_ZERO_ERROR;
    LocalUResourceBundlePointer rb(ures_open(U_ICUDATA_CURR, loc.तथ्य(), &ec2));

    अगर (nameStyle == UCURR_NARROW_प्रतीक_NAME || nameStyle == UCURR_FORMAL_प्रतीक_NAME || nameStyle == UCURR_VARIANT_प्रतीक_NAME) {
        CharString कुंजी;
        चयन (nameStyle) {
        हाल UCURR_NARROW_प्रतीक_NAME:
            कुंजी.append(CURRENCIES_NARROW, ec2);
            अवरोध;
        हाल UCURR_FORMAL_प्रतीक_NAME:
            कुंजी.append(CURRENCIES_FORMAL, ec2);
            अवरोध;
        हाल UCURR_VARIANT_प्रतीक_NAME:
            कुंजी.append(CURRENCIES_VARIANT, ec2);
            अवरोध;
        शेष:
            *ec = U_UNSUPPORTED_ERROR;
            वापस nullptr;
        }
        कुंजी.append("/", ec2);
        कुंजी.append(buf, ec2);
        s = ures_get_String_By_कुंजी_With_Fallback(rb.getAlias(), कुंजी.तथ्य(), len, &ec2);
        अगर (ec2 == U_MISSING_RESOURCE_ERROR) {
            *ec = U_USING_FALLBACK_WARNING;
            ec2 = U_ZERO_ERROR;
            choice = UCURR_प्रतीक_NAME;
        }
    }
    अगर (s == nullptr) {
        ures_get_By_कुंजी(rb.getAlias(), CURRENCIES, rb.getAlias(), &ec2);
        ures_get_By_कुंजी_With_Fallback(rb.getAlias(), buf, rb.getAlias(), &ec2);
        s = ures_get_String_By_सूचकांक(rb.getAlias(), choice, len, &ec2);
    }

    // If we've succeeded we're done.  Otherwise, try to fallback.
    // If that fails (because we are already at root) then exit.
    अगर (U_SUCCESS(ec2)) {
        अगर (ec2 == U_USING_DEFAULT_WARNING
            || (ec2 == U_USING_FALLBACK_WARNING && *ec != U_USING_DEFAULT_WARNING)) {
            *ec = ec2;
        }
    }

    // We no longer support choice format data in names.  Data should not contain
    // choice patterns.
    अगर (isChoiceFormat != nullptr) {
        *isChoiceFormat = false;
    }
    अगर (U_SUCCESS(ec2)) {
        U_ASSERT(s != nullptr);
        वापस s;
    }

    // If we fail to find a match, use the ISO 4217 code
    *len = u_strlen(currency); // Should == ISO_CURRENCY_CODE_LENGTH, but maybe not...?
    *ec = U_USING_DEFAULT_WARNING;
    वापस currency;
}

U_CAPI स्थिर char16_t* U_EXPORT2
ucurr_getPluralName(स्थिर char16_t* currency U_LIFETIME_BOUND,
                    स्थिर अक्षर* locale,
                    UBool* isChoiceFormat,
                    स्थिर अक्षर* plural_गिनती,
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

    अगर (U_FAILURE(*ec)) {
        वापस nullptr;
    }

    // Use a separate UErrorCode here that does not propagate out of
    // this function.
    UErrorCode ec2 = U_ZERO_ERROR;

    अगर (locale == nullptr) {
        locale = uloc_getDefault();
    }
    CharString loc = ulocimp_getName(locale, ec2);
    अगर (U_FAILURE(ec2)) {
        *ec = U_ILLEGAL_ARGUMENT_ERROR;
        वापस nullptr;
    }

    अक्षर buf[ISO_CURRENCY_CODE_लंबाई+1];
    myUCharsToChars(buf, currency);

    स्थिर char16_t* s = nullptr;
    ec2 = U_ZERO_ERROR;
    UResourceBundle* rb = ures_open(U_ICUDATA_CURR, loc.तथ्य(), &ec2);

    rb = ures_get_By_कुंजी(rb, CURRENCYPLURALS, rb, &ec2);

    // Fetch resource with multi-level resource inheritance fallback
    LocalUResourceBundlePointer curr(ures_get_By_कुंजी_With_Fallback(rb, buf, rb, &ec2));

    s = ures_get_String_By_कुंजी_With_Fallback(curr.getAlias(), plural_गिनती, len, &ec2);
    अगर (U_FAILURE(ec2)) {
        //  fall back to "other"
        ec2 = U_ZERO_ERROR;
        s = ures_get_String_By_कुंजी_With_Fallback(curr.getAlias(), "other", len, &ec2);     
        अगर (U_FAILURE(ec2)) {
            // fall back to long name in Currencies
            वापस ucurr_getName(currency, locale, UCURR_LONG_NAME, 
                                 isChoiceFormat, len, ec);
        }
    }

    // If we've succeeded we're done.  Otherwise, try to fallback.
    // If that fails (because we are already at root) then exit.
    अगर (U_SUCCESS(ec2)) {
        अगर (ec2 == U_USING_DEFAULT_WARNING
            || (ec2 == U_USING_FALLBACK_WARNING && *ec != U_USING_DEFAULT_WARNING)) {
            *ec = ec2;
        }
        U_ASSERT(s != nullptr);
        वापस s;
    }

    // If we fail to find a match, use the ISO 4217 code
    *len = u_strlen(currency); // Should == ISO_CURRENCY_CODE_LENGTH, but maybe not...?
    *ec = U_USING_DEFAULT_WARNING;
    वापस currency;
}


//========================================================================
// Following are structure and function for parsing currency names

#घोषणा NEED_TO_BE_DELETED 0x1

// TODO: a better way to define this?
#घोषणा MAX_CURRENCY_NAME_LEN 100

प्रकार काष्ठा {
    स्थिर अक्षर* IsoCode;  //कुंजी
    char16_t* currencyName;  //मूल्य
    int32_t currencyNameLen;  // value length
    int32_t flag;  // flags
} CurrencyNameStruct;


#अगर_अघोषित MIN
#घोषणा MIN(a,b) (((a)<(b)) ? (a) : (b))
#पूर्ण_अगर

#अगर_अघोषित MAX
#घोषणा MAX(a,b) (((a)<(b)) ? (b) : (a))
#पूर्ण_अगर


// Comparison function used in quick sort.
अटल पूर्णांक U_CALLCONV currencyNameComparator(स्थिर व्योम* a, स्थिर व्योम* b) {
    स्थिर CurrencyNameStruct* currName_1 = static_cast<स्थिर CurrencyNameStruct*>(a);
    स्थिर CurrencyNameStruct* currName_2 = static_cast<स्थिर CurrencyNameStruct*>(b);
    क्रम (int32_t i = 0; 
         i < MIN(currName_1->currencyNameLen, currName_2->currencyNameLen);
         ++i) {
        अगर (currName_1->currencyName[i] < currName_2->currencyName[i]) {
            वापस -1;
        }
        अगर (currName_1->currencyName[i] > currName_2->currencyName[i]) {
            वापस 1;
        }
    }
    अगर (currName_1->currencyNameLen < currName_2->currencyNameLen) {
        वापस -1;
    } अन्यथा अगर (currName_1->currencyNameLen > currName_2->currencyNameLen) {
        वापस 1;
    }
    वापस 0;
}


// Give a locale, return the maximum number of currency names associated with
// this locale.
// It gets currency names from resource bundles using fallback.
// It is the maximum number because in the fallback chain, some of the 
// currency names are duplicated.
// For example, given locale as "en_US", the currency names get from resource
// bundle in "en_US" and "en" are duplicated. The fallback mechanism will count
// all currency names in "en_US" and "en".
अटल व्योम
get_Currency_Name_गिनती(स्थिर अक्षर* loc, int32_t* योग_currency_name_गिनती, int32_t* योग_currency_प्रतीक_गिनती) {
    U_NAMESPACE_USE
    *योग_currency_name_गिनती = 0;
    *योग_currency_प्रतीक_गिनती = 0;
    स्थिर char16_t* s = nullptr;
    CharString locale;
    {
        UErrorCode status = U_ZERO_ERROR;
        locale.append(loc, status);
        अगर (U_FAILURE(status)) { वापस; }
    }
    स्थिर icu::Hashtable *currencySymbolsEquiv = getCurrSymbolsEquiv();
    क्रम (;;) {
        UErrorCode ec2 = U_ZERO_ERROR;
        // TODO: ures_openDirect?
        LocalUResourceBundlePointer rb(ures_open(U_ICUDATA_CURR, locale.तथ्य(), &ec2));
        LocalUResourceBundlePointer curr(ures_get_By_कुंजी(rb.getAlias(), CURRENCIES, nullptr, &ec2));
        int32_t n = ures_get_आकार(curr.getAlias());
        क्रम (int32_t i=0; i<n; ++i) {
            LocalUResourceBundlePointer names(ures_get_By_सूचकांक(curr.getAlias(), i, nullptr, &ec2));
            int32_t len;
            s = ures_get_String_By_सूचकांक(names.getAlias(), UCURR_प्रतीक_NAME, &len, &ec2);
            ++(*योग_currency_प्रतीक_गिनती);  // currency symbol
            अगर (currencySymbolsEquiv != nullptr) {
                *योग_currency_प्रतीक_गिनती += गिनती_Equivalent(*currencySymbolsEquiv, UnicodeString(true, s, len));
            }
            ++(*योग_currency_प्रतीक_गिनती); // iso code
            ++(*योग_currency_name_गिनती); // long name
        }

        // currency plurals
        UErrorCode ec3 = U_ZERO_ERROR;
        LocalUResourceBundlePointer curr_p(ures_get_By_कुंजी(rb.getAlias(), CURRENCYPLURALS, nullptr, &ec3));
        n = ures_get_आकार(curr_p.getAlias());
        क्रम (int32_t i=0; i<n; ++i) {
            LocalUResourceBundlePointer names(ures_get_By_सूचकांक(curr_p.getAlias(), i, nullptr, &ec3));
            *योग_currency_name_गिनती += ures_get_आकार(names.getAlias());
        }

        अगर (!fallback(locale)) {
            अवरोध;
        }
    }
}

अटल char16_t*
toUpperCase(स्थिर char16_t* source, int32_t len, स्थिर अक्षर* locale) {
    char16_t* dest = nullptr;
    UErrorCode ec = U_ZERO_ERROR;
    int32_t destLen = u_strToUpper(dest, 0, source, len, locale, &ec);

    ec = U_ZERO_ERROR;
    dest = static_cast<char16_t*>(uprv_malloc(माप(char16_t) * MAX(destLen, len)));
    अगर (dest == nullptr) {
        वापस nullptr;
    }
    u_strToUpper(dest, destLen, source, len, locale, &ec);
    अगर (U_FAILURE(ec)) {
        u_memcpy(dest, source, len);
    } 
    वापस dest;
}


अटल व्योम deleteCurrencyNames(CurrencyNameStruct* currencyNames, int32_t count);
// Collect all available currency names associated with the given locale
// (enable fallback chain).
// Read currenc names defined in resource bundle "Currencies" and
// "CurrencyPlural", enable fallback chain.
// return the malloc-ed currency name arrays and the total number of currency
// names in the array.
अटल व्योम
collectCurrencyNames(स्थिर अक्षर* locale, 
                     CurrencyNameStruct** currencyNames, 
                     int32_t* योग_currency_name_गिनती, 
                     CurrencyNameStruct** currencySymbols, 
                     int32_t* योग_currency_प्रतीक_गिनती, 
                     UErrorCode& ec) {
    अगर (U_FAILURE(ec)) {
        *currencyNames = *currencySymbols = nullptr;
        *योग_currency_name_गिनती = *योग_currency_प्रतीक_गिनती = 0;
        वापस;
    }
    U_NAMESPACE_USE
    स्थिर icu::Hashtable *currencySymbolsEquiv = getCurrSymbolsEquiv();
    // Look up the Currencies resource for the given locale.
    UErrorCode ec2 = U_ZERO_ERROR;

    अगर (locale == nullptr) {
        locale = uloc_getDefault();
    }
    CharString loc = ulocimp_getName(locale, ec2);
    अगर (U_FAILURE(ec2)) {
        ec = U_ILLEGAL_ARGUMENT_ERROR;
        *currencyNames = *currencySymbols = nullptr;
        *योग_currency_name_गिनती = *योग_currency_प्रतीक_गिनती = 0;
        वापस;
    }

    // Get maximum currency name count first.
    get_Currency_Name_गिनती(loc.तथ्य(), योग_currency_name_गिनती, योग_currency_प्रतीक_गिनती);

    *currencyNames = static_cast<CurrencyNameStruct*>(
        uprv_malloc(माप(CurrencyNameStruct) * (*योग_currency_name_गिनती)));
    अगर(*currencyNames == nullptr) {
        *currencySymbols = nullptr;
        *योग_currency_name_गिनती = *योग_currency_प्रतीक_गिनती = 0;
        ec = U_MEMORY_ALLOCATION_ERROR;
        वापस;
    }
    *currencySymbols = static_cast<CurrencyNameStruct*>(
        uprv_malloc(माप(CurrencyNameStruct) * (*योग_currency_प्रतीक_गिनती)));

    अगर(*currencySymbols == nullptr) {
        uprv_free(*currencyNames);
        *currencyNames = nullptr;
        *योग_currency_name_गिनती = *योग_currency_प्रतीक_गिनती = 0;
        ec = U_MEMORY_ALLOCATION_ERROR;
        वापस;
    }

    स्थिर char16_t* s = nullptr;  // currency name
    अक्षर* iso = nullptr;  // currency ISO code

    *योग_currency_name_गिनती = 0;
    *योग_currency_प्रतीक_गिनती = 0;

    UErrorCode ec3 = U_ZERO_ERROR;
    UErrorCode ec4 = U_ZERO_ERROR;

    // Using hash to remove duplicates caused by locale fallback
    LocalUHashtablePointer currencyIsoCodes(uhash_open(uhash_hashChars, uhash_compareChars, nullptr, &ec3));
    LocalUHashtablePointer currencyPluralIsoCodes(uhash_open(uhash_hashChars, uhash_compareChars, nullptr, &ec4));
    क्रम (int32_t localeLevel = 0; ; ++localeLevel) {
        ec2 = U_ZERO_ERROR;
        // TODO: ures_openDirect
        LocalUResourceBundlePointer rb(ures_open(U_ICUDATA_CURR, loc.तथ्य(), &ec2));
        LocalUResourceBundlePointer curr(ures_get_By_कुंजी(rb.getAlias(), CURRENCIES, nullptr, &ec2));
        int32_t n = ures_get_आकार(curr.getAlias());
        क्रम (int32_t i=0; i<n; ++i) {
            LocalUResourceBundlePointer names(ures_get_By_सूचकांक(curr.getAlias(), i, nullptr, &ec2));
            int32_t len;
            s = ures_get_String_By_सूचकांक(names.getAlias(), UCURR_प्रतीक_NAME, &len, &ec2);
            // TODO: uhash_put wont change key/value?
            iso = const_cast<अक्षर*>(ures_get_कुंजी(names.getAlias()));
            अगर (localeLevel != 0 && uhash_get(currencyIsoCodes.getAlias(), iso) != nullptr) {
                जारी;
            }
            uhash_put(currencyIsoCodes.getAlias(), iso, iso, &ec3); 
            // Add currency symbol.
            (*currencySymbols)[(*योग_currency_प्रतीक_गिनती)++] = {iso, const_cast<char16_t*>(s), len, 0};

            // Add equivalent symbols
            अगर (currencySymbolsEquiv != nullptr) {
                UnicodeString str(true, s, len);
                icu::EquivIterator iter(*currencySymbolsEquiv, str);
                स्थिर UnicodeString *प्रतीक;
                जबतक ((प्रतीक = iter.next()) != nullptr) {
                    (*currencySymbols)[(*योग_currency_प्रतीक_गिनती)++]
                        = {iso, const_cast<char16_t*>(प्रतीक->get_बफर()), प्रतीक->length(), 0};
                }
            }

            // Add currency long name.
            s = ures_get_String_By_सूचकांक(names.getAlias(), UCURR_LONG_NAME, &len, &ec2);
            char16_t* upperName = toUpperCase(s, len, locale);
            अगर (upperName == nullptr) {
                ec = U_MEMORY_ALLOCATION_ERROR;
                जाओ error;
            }
            (*currencyNames)[(*योग_currency_name_गिनती)++] = {iso, upperName, len, NEED_TO_BE_DELETED};

            // put (iso, 3, and iso) in to array
            // Add currency ISO code.
            char16_t* isoCode = static_cast<char16_t*>(uprv_malloc(माप(char16_t) * 3));
            अगर (isoCode == nullptr) {
                ec = U_MEMORY_ALLOCATION_ERROR;
                जाओ error;
            }
            // Must convert iso[] into Unicode
            u_charsToUChars(iso, isoCode, 3);
            (*currencySymbols)[(*योग_currency_प्रतीक_गिनती)++] = {iso, isoCode, 3, NEED_TO_BE_DELETED};
        }

        // currency plurals
        UErrorCode ec5 = U_ZERO_ERROR;
        LocalUResourceBundlePointer curr_p(ures_get_By_कुंजी(rb.getAlias(), CURRENCYPLURALS, nullptr, &ec5));
        n = ures_get_आकार(curr_p.getAlias());
        क्रम (int32_t i=0; i<n; ++i) {
            LocalUResourceBundlePointer names(ures_get_By_सूचकांक(curr_p.getAlias(), i, nullptr, &ec5));
            iso = const_cast<अक्षर*>(ures_get_कुंजी(names.getAlias()));
            // Using hash to remove duplicated ISO codes in fallback chain.
            अगर (localeLevel != 0 && uhash_get(currencyPluralIsoCodes.getAlias(), iso) != nullptr) {
                जारी;
            }
            uhash_put(currencyPluralIsoCodes.getAlias(), iso, iso, &ec4);
            int32_t num = ures_get_आकार(names.getAlias());
            int32_t len;
            क्रम (int32_t j = 0; j < num; ++j) {
                // TODO: remove duplicates between singular name and 
                // currency long name?
                s = ures_get_String_By_सूचकांक(names.getAlias(), j, &len, &ec5);
                char16_t* upperName = toUpperCase(s, len, locale);
                अगर (upperName == nullptr) {
                    ec = U_MEMORY_ALLOCATION_ERROR;
                    जाओ error;
                }
                (*currencyNames)[(*योग_currency_name_गिनती)++] = {iso, upperName, len, NEED_TO_BE_DELETED};
            }
        }

        अगर (!fallback(loc)) {
            अवरोध;
        }
    }

    // quick sort the struct
    क्विक(*currencyNames, *योग_currency_name_गिनती, 
          माप(CurrencyNameStruct), currencyNameComparator);
    क्विक(*currencySymbols, *योग_currency_प्रतीक_गिनती, 
          माप(CurrencyNameStruct), currencyNameComparator);

#अगर_घोषित UCURR_DEBUG
    म_लिखो("currency name count: %d\n", *योग_currency_name_गिनती);
    क्रम (int32_t सूचकांक = 0; सूचकांक < *योग_currency_name_गिनती; ++सूचकांक) {
        म_लिखो("index: %d\n", सूचकांक);
        म_लिखो("iso: %s\n", (*currencyNames)[सूचकांक].IsoCode);
        अक्षर curNameBuf[1024];
        स_रखो(curNameBuf, 0, 1024);
        u_austrncpy(curNameBuf, (*currencyNames)[सूचकांक].currencyName, (*currencyNames)[सूचकांक].currencyNameLen);
        म_लिखो("currencyName: %s\n", curNameBuf);
        म_लिखो("len: %d\n", (*currencyNames)[सूचकांक].currencyNameLen);
    }
    म_लिखो("currency symbol count: %d\n", *योग_currency_प्रतीक_गिनती);
    क्रम (int32_t सूचकांक = 0; सूचकांक < *योग_currency_प्रतीक_गिनती; ++सूचकांक) {
        म_लिखो("index: %d\n", सूचकांक);
        म_लिखो("iso: %s\n", (*currencySymbols)[सूचकांक].IsoCode);
        अक्षर curNameBuf[1024];
        स_रखो(curNameBuf, 0, 1024);
        u_austrncpy(curNameBuf, (*currencySymbols)[सूचकांक].currencyName, (*currencySymbols)[सूचकांक].currencyNameLen);
        म_लिखो("currencySymbol: %s\n", curNameBuf);
        म_लिखो("len: %d\n", (*currencySymbols)[सूचकांक].currencyNameLen);
    }
#पूर्ण_अगर
    // fail on hashtable errors
    अगर (U_FAILURE(ec3)) {
      ec = ec3;
    } अन्यथा अगर (U_FAILURE(ec4)) {
      ec = ec4;
    }

  error:
    // clean up if we got error
    अगर (U_FAILURE(ec)) {
        deleteCurrencyNames(*currencyNames, *योग_currency_name_गिनती);
        deleteCurrencyNames(*currencySymbols, *योग_currency_प्रतीक_गिनती);
        *currencyNames = *currencySymbols = nullptr;
        *योग_currency_name_गिनती = *योग_currency_प्रतीक_गिनती = 0;
    }
}

// @param  currencyNames: currency names array
// @param  indexInCurrencyNames: the index of the character in currency names 
//         array against which the comparison is done
// @param  key: input text char to compare against
// @param  begin(IN/OUT): the begin index of matching range in currency names array
// @param  end(IN/OUT): the end index of matching range in currency names array.
अटल int32_t
द्विआधारी_खोज(स्थिर CurrencyNameStruct* currencyNames, 
             int32_t सूचकांक_In_Currency_Names,
             स्थिर char16_t कुंजी,
             int32_t* begin, int32_t* end) {
#अगर_घोषित UCURR_DEBUG
    म_लिखो("key = %x\n", कुंजी);
#पूर्ण_अगर
   int32_t first = *begin;
   int32_t last = *end;
   जबतक (first <= last) {
       int32_t mid = (first + last) / 2;  // compute mid point.
       अगर (सूचकांक_In_Currency_Names >= currencyNames[mid].currencyNameLen) {
           first = mid + 1;
       } अन्यथा {
           अगर (कुंजी > currencyNames[mid].currencyName[सूचकांक_In_Currency_Names]) {
               first = mid + 1;
           }
           अन्यथा अगर (कुंजी < currencyNames[mid].currencyName[सूचकांक_In_Currency_Names]) {
               last = mid - 1;
           }
           अन्यथा {
                // Find a match, and looking for ranges
                // Now do two more binary searches. First, on the left side for
                // the greatest L such that CurrencyNameStruct[L] < key.
                int32_t L = *begin;
                int32_t R = mid;

#अगर_घोषित UCURR_DEBUG
                म_लिखो("mid = %d\n", mid);
#पूर्ण_अगर
                जबतक (L < R) {
                    int32_t M = (L + R) / 2;
#अगर_घोषित UCURR_DEBUG
                    म_लिखो("L = %d, R = %d, M = %d\n", L, R, M);
#पूर्ण_अगर
                    अगर (सूचकांक_In_Currency_Names >= currencyNames[M].currencyNameLen) {
                        L = M + 1;
                    } अन्यथा {
                        अगर (currencyNames[M].currencyName[सूचकांक_In_Currency_Names] < कुंजी) {
                            L = M + 1;
                        } अन्यथा {
#अगर_घोषित UCURR_DEBUG
                            U_ASSERT(currencyNames[M].currencyName[सूचकांक_In_Currency_Names] == कुंजी);
#पूर्ण_अगर
                            R = M;
                        }
                    }
                }
#अगर_घोषित UCURR_DEBUG
                U_ASSERT(L == R);
#पूर्ण_अगर
                *begin = L;
#अगर_घोषित UCURR_DEBUG
                म_लिखो("begin = %d\n", *begin);
                U_ASSERT(currencyNames[*begin].currencyName[सूचकांक_In_Currency_Names] == कुंजी);
#पूर्ण_अगर

                // Now for the second search, finding the least R such that
                // key < CurrencyNameStruct[R].
                L = mid;
                R = *end;
                जबतक (L < R) {
                    int32_t M = (L + R) / 2;
#अगर_घोषित UCURR_DEBUG
                    म_लिखो("L = %d, R = %d, M = %d\n", L, R, M);
#पूर्ण_अगर
                    अगर (currencyNames[M].currencyNameLen < सूचकांक_In_Currency_Names) {
                        L = M + 1;
                    } अन्यथा {
                        अगर (currencyNames[M].currencyName[सूचकांक_In_Currency_Names] > कुंजी) {
                            R = M;
                        } अन्यथा {
#अगर_घोषित UCURR_DEBUG
                            U_ASSERT(currencyNames[M].currencyName[सूचकांक_In_Currency_Names] == कुंजी);
#पूर्ण_अगर
                            L = M + 1;
                        }
                    }
                }
#अगर_घोषित UCURR_DEBUG
                U_ASSERT(L == R);
#पूर्ण_अगर
                अगर (currencyNames[R].currencyName[सूचकांक_In_Currency_Names] > कुंजी) {
                    *end = R - 1;
                } अन्यथा {
                    *end = R;
                }
#अगर_घोषित UCURR_DEBUG
                म_लिखो("end = %d\n", *end);
#पूर्ण_अगर

                // now, found the range. check whether there is exact match
                अगर (currencyNames[*begin].currencyNameLen == सूचकांक_In_Currency_Names + 1) {
                    वापस *begin;  // find range and exact match.
                }
                वापस -1;  // find range, but no exact match.
           }
       }
   }
   *begin = -1;
   *end = -1;
   वापस -1;    // failed to find range.
}


// Linear search "text" in "currencyNames".
// @param  begin, end: the begin and end index in currencyNames, within which
//         range should the search be performed.
// @param  textLen: the length of the text to be compared
// @param  maxMatchLen(IN/OUT): passing in the computed max matching length
//                              pass out the new max  matching length
// @param  maxMatchIndex: the index in currencyName which has the longest
//                        match with input text.
अटल व्योम
linear_खोज(स्थिर CurrencyNameStruct* currencyNames, 
             int32_t begin, int32_t end,
             स्थिर char16_t* text, int32_t textLen,
             int32_t *partialMatchLen,
             int32_t *maxMatchLen, int32_t* max_Match_सूचकांक) {
    int32_t initialPartialMatchLen = *partialMatchLen;
    क्रम (int32_t सूचकांक = begin; सूचकांक <= end; ++सूचकांक) {
        int32_t len = currencyNames[सूचकांक].currencyNameLen;
        अगर (len > *maxMatchLen && len <= textLen &&
            uprv_memcmp(currencyNames[सूचकांक].currencyName, text, len * माप(char16_t)) == 0) {
            *partialMatchLen = MAX(*partialMatchLen, len);
            *max_Match_सूचकांक = सूचकांक;
            *maxMatchLen = len;
#अगर_घोषित UCURR_DEBUG
            म_लिखो("maxMatchIndex = %d, maxMatchLen = %d\n",
                   *max_Match_सूचकांक, *maxMatchLen);
#पूर्ण_अगर
        } अन्यथा {
            // Check for partial matches.
            क्रम (int32_t i=initialPartialMatchLen; i<MIN(len, textLen); i++) {
                अगर (currencyNames[सूचकांक].currencyName[i] != text[i]) {
                    अवरोध;
                }
                *partialMatchLen = MAX(*partialMatchLen, i + 1);
            }
        }
    }
}

#घोषणा LINEAR_खोज_THRESHOLD 10

// Find longest match between "text" and currency names in "currencyNames".
// @param  total_currency_count: total number of currency names in CurrencyNames.
// @param  textLen: the length of the text to be compared
// @param  maxMatchLen: passing in the computed max matching length
//                              pass out the new max  matching length
// @param  maxMatchIndex: the index in currencyName which has the longest
//                        match with input text.
अटल व्योम
खोज_Currency_Name(स्थिर CurrencyNameStruct* currencyNames, 
                   int32_t योग_currency_गिनती,
                   स्थिर char16_t* text, int32_t textLen,
                   int32_t *partialMatchLen,
                   int32_t* maxMatchLen, int32_t* max_Match_सूचकांक) {
    *max_Match_सूचकांक = -1;
    *maxMatchLen = 0;
    int32_t match_सूचकांक = -1;
    int32_t द्विआधारी_खोज_Begin = 0;
    int32_t द्विआधारी_खोज_End = योग_currency_गिनती - 1;
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
    क्रम (int32_t सूचकांक = 0; सूचकांक < textLen; ++सूचकांक) {
        // matchIndex saves the one with exact match till the current point.
        // [binarySearchBegin, binarySearchEnd] saves the matching range.
        match_सूचकांक = द्विआधारी_खोज(currencyNames, सूचकांक,
                                  text[सूचकांक],
                                  &द्विआधारी_खोज_Begin, &द्विआधारी_खोज_End);
        अगर (द्विआधारी_खोज_Begin == -1) { // did not find the range
            अवरोध;
        }
        *partialMatchLen = MAX(*partialMatchLen, सूचकांक + 1);
        अगर (match_सूचकांक != -1) { 
            // find an exact match for text from text[0] to text[index] 
            // in currencyNames array.
            *maxMatchLen = सूचकांक + 1;
            *max_Match_सूचकांक = match_सूचकांक;
        }
        अगर (द्विआधारी_खोज_End - द्विआधारी_खोज_Begin < LINEAR_खोज_THRESHOLD) {
            // linear search if within threshold.
            linear_खोज(currencyNames, द्विआधारी_खोज_Begin, द्विआधारी_खोज_End,
                         text, textLen,
                         partialMatchLen,
                         maxMatchLen, max_Match_सूचकांक);
            अवरोध;
        }
    }
}

//========================= currency name cache =====================
प्रकार काष्ठा {
    अक्षर locale[ULOC_FULLNAME_CAPACITY];  //key
    // currency names, case insensitive
    CurrencyNameStruct* currencyNames;  //मूल्य
    int32_t योग_Currency_Name_गिनती;  // currency name count
    // currency symbols and ISO code, case sensitive
    CurrencyNameStruct* currencySymbols; //मूल्य
    int32_t योग_Currency_प्रतीक_गिनती;  //गिनती
    // reference count.
    // reference count is set to 1 when an entry is put to cache.
    // it increases by 1 before accessing, and decreased by 1 after accessing.
    // The entry is deleted when ref count is zero, which means 
    // the entry is replaced out of cache and no process is accessing it.
    int32_t ref_गिनती;
} CurrencyNameCacheEntry;


#घोषणा CURRENCY_NAME_CACHE_अंक 10

// Reserve 10 cache entries.
अटल CurrencyNameCacheEntry* currCache[CURRENCY_NAME_CACHE_अंक] = {nullptr};
// Using an index to indicate which entry to be replaced when cache is full.
// It is a simple round-robin replacement strategy.
अटल int8_t current_Cache_Entry_सूचकांक = 0;

अटल UMutex gCurrencyCacheMutex;

// Cache deletion
अटल व्योम
deleteCurrencyNames(CurrencyNameStruct* currencyNames, int32_t count) {
    क्रम (int32_t सूचकांक = 0; सूचकांक < count; ++सूचकांक) {
        अगर ( (currencyNames[सूचकांक].flag & NEED_TO_BE_DELETED) ) {
            uprv_free(currencyNames[सूचकांक].currencyName);
        }
    }
    uprv_free(currencyNames);
}


अटल व्योम
deleteCacheEntry(CurrencyNameCacheEntry* entry) {
    deleteCurrencyNames(entry->currencyNames, entry->योग_Currency_Name_गिनती);
    deleteCurrencyNames(entry->currencySymbols, entry->योग_Currency_प्रतीक_गिनती);
    uprv_free(entry);
}


// Cache clean up
अटल UBool U_CALLCONV
currency_cache_cleanup() {
    क्रम (int32_t i = 0; i < CURRENCY_NAME_CACHE_अंक; ++i) {
        अगर (currCache[i]) {
            deleteCacheEntry(currCache[i]);
            currCache[i] = nullptr;
        }
    }
    वापस true;
}


/**
 * Loads the currency name data from the cache, or from resource bundles if necessary.
 * The refCount is automatically incremented.  It is the caller's responsibility
 * to decrement it when done!
 */
अटल CurrencyNameCacheEntry*
getCacheEntry(स्थिर अक्षर* locale, UErrorCode& ec) {

    int32_t योग_currency_name_गिनती = 0;
    CurrencyNameStruct* currencyNames = nullptr;
    int32_t योग_currency_प्रतीक_गिनती = 0;
    CurrencyNameStruct* currencySymbols = nullptr;
    CurrencyNameCacheEntry* cacheEntry = nullptr;

    umtx_lock(&gCurrencyCacheMutex);
    // in order to handle racing correctly,
    // not putting 'search' in a separate function.
    int8_t found = -1;
    क्रम (int8_t i = 0; i < CURRENCY_NAME_CACHE_अंक; ++i) {
        अगर (currCache[i]!= nullptr &&
            uprv_strcmp(locale, currCache[i]->locale) == 0) {
            found = i;
            अवरोध;
        }
    }
    अगर (found != -1) {
        cacheEntry = currCache[found];
        ++(cacheEntry->ref_गिनती);
    }
    umtx_unlock(&gCurrencyCacheMutex);
    अगर (found == -1) {
        collectCurrencyNames(locale, &currencyNames, &योग_currency_name_गिनती, &currencySymbols, &योग_currency_प्रतीक_गिनती, ec);
        अगर (U_FAILURE(ec)) {
            वापस nullptr;
        }
        umtx_lock(&gCurrencyCacheMutex);
        // check again.
        क्रम (int8_t i = 0; i < CURRENCY_NAME_CACHE_अंक; ++i) {
            अगर (currCache[i]!= nullptr &&
                uprv_strcmp(locale, currCache[i]->locale) == 0) {
                found = i;
                अवरोध;
            }
        }
        अगर (found == -1) {
            // insert new entry to 
            // currentCacheEntryIndex % CURRENCY_NAME_CACHE_NUM
            // and remove the existing entry 
            // currentCacheEntryIndex % CURRENCY_NAME_CACHE_NUM
            // from cache.
            cacheEntry = currCache[current_Cache_Entry_सूचकांक];
            अगर (cacheEntry) {
                --(cacheEntry->ref_गिनती);
                // delete if the ref count is zero
                अगर (cacheEntry->ref_गिनती == 0) {
                    deleteCacheEntry(cacheEntry);
                }
            }
            cacheEntry = static_cast<CurrencyNameCacheEntry*>(uprv_malloc(माप(CurrencyNameCacheEntry)));
            अगर (cacheEntry == nullptr) {
                deleteCurrencyNames(currencyNames, योग_currency_name_गिनती);
                deleteCurrencyNames(currencySymbols, योग_currency_प्रतीक_गिनती);
                ec = U_MEMORY_ALLOCATION_ERROR;
                वापस nullptr;
            }
            currCache[current_Cache_Entry_सूचकांक] = cacheEntry;
            uprv_strcpy(cacheEntry->locale, locale);
            cacheEntry->currencyNames = currencyNames;
            cacheEntry->योग_Currency_Name_गिनती = योग_currency_name_गिनती;
            cacheEntry->currencySymbols = currencySymbols;
            cacheEntry->योग_Currency_प्रतीक_गिनती = योग_currency_प्रतीक_गिनती;
            cacheEntry->ref_गिनती = 2; // one for cache, one for reference
            current_Cache_Entry_सूचकांक = (current_Cache_Entry_सूचकांक + 1) % CURRENCY_NAME_CACHE_अंक;
            ucln_common_registerCleanup(UCLN_COMMON_CURRENCY, currency_cleanup);
        } अन्यथा {
            deleteCurrencyNames(currencyNames, योग_currency_name_गिनती);
            deleteCurrencyNames(currencySymbols, योग_currency_प्रतीक_गिनती);
            cacheEntry = currCache[found];
            ++(cacheEntry->ref_गिनती);
        }
        umtx_unlock(&gCurrencyCacheMutex);
    }

    वापस cacheEntry;
}

अटल व्योम releaseCacheEntry(CurrencyNameCacheEntry* cacheEntry) {
    umtx_lock(&gCurrencyCacheMutex);
    --(cacheEntry->ref_गिनती);
    अगर (cacheEntry->ref_गिनती == 0) {  // remove
        deleteCacheEntry(cacheEntry);
    }
    umtx_unlock(&gCurrencyCacheMutex);
}

U_CAPI व्योम
uprv_parseCurrency(स्थिर अक्षर* locale,
                   स्थिर icu::UnicodeString& text,
                   icu::ParsePosition& pos,
                   int8_t type,
                   int32_t* partialMatchLen,
                   char16_t* परिणाम,
                   UErrorCode& ec) {
    U_NAMESPACE_USE
    अगर (U_FAILURE(ec)) {
        वापस;
    }
    CurrencyNameCacheEntry* cacheEntry = getCacheEntry(locale, ec);
    अगर (U_FAILURE(ec)) {
        वापस;
    }

    int32_t योग_currency_name_गिनती = cacheEntry->योग_Currency_Name_गिनती;
    CurrencyNameStruct* currencyNames = cacheEntry->currencyNames;
    int32_t योग_currency_प्रतीक_गिनती = cacheEntry->योग_Currency_प्रतीक_गिनती;
    CurrencyNameStruct* currencySymbols = cacheEntry->currencySymbols;

    int32_t start = pos.get_सूचकांक();

    char16_t निवेश_Text[MAX_CURRENCY_NAME_LEN];
    char16_t upperText[MAX_CURRENCY_NAME_LEN];
    int32_t textLen = MIN(MAX_CURRENCY_NAME_LEN, text.length() - start);
    text.extract(start, textLen, निवेश_Text);
    UErrorCode ec1 = U_ZERO_ERROR;
    textLen = u_strToUpper(upperText, MAX_CURRENCY_NAME_LEN, निवेश_Text, textLen, locale, &ec1);

    // Make sure partialMatchLen is initialized
    *partialMatchLen = 0;

    int32_t max = 0;
    int32_t match_सूचकांक = -1;
    // case in-sensitive comparison against currency names
    खोज_Currency_Name(currencyNames, योग_currency_name_गिनती, 
                       upperText, textLen, partialMatchLen, &max, &match_सूचकांक);

#अगर_घोषित UCURR_DEBUG
    म_लिखो("search in names, max = %d, matchIndex = %d\n", max, match_सूचकांक);
#पूर्ण_अगर

    int32_t max_In_प्रतीक = 0;
    int32_t match_सूचकांक_In_प्रतीक = -1;
    अगर (type != UCURR_LONG_NAME) {  // not name only
        // case sensitive comparison against currency symbols and ISO code.
        खोज_Currency_Name(currencySymbols, योग_currency_प्रतीक_गिनती, 
                           निवेश_Text, textLen,
                           partialMatchLen,
                           &max_In_प्रतीक, &match_सूचकांक_In_प्रतीक);
    }

#अगर_घोषित UCURR_DEBUG
    म_लिखो("search in symbols, maxInSymbol = %d, matchIndexInSymbol = %d\n", max_In_प्रतीक, match_सूचकांक_In_प्रतीक);
    अगर(match_सूचकांक_In_प्रतीक != -1) {
      म_लिखो("== ISO=%s\n", currencySymbols[match_सूचकांक_In_प्रतीक].IsoCode);
    }
#पूर्ण_अगर

    अगर (max >= max_In_प्रतीक && match_सूचकांक != -1) {
        u_charsToUChars(currencyNames[match_सूचकांक].IsoCode, परिणाम, 4);
        pos.set_सूचकांक(start + max);
    } अन्यथा अगर (max_In_प्रतीक >= max && match_सूचकांक_In_प्रतीक != -1) {
        u_charsToUChars(currencySymbols[match_सूचकांक_In_प्रतीक].IsoCode, परिणाम, 4);
        pos.set_सूचकांक(start + max_In_प्रतीक);
    }

    // decrease reference count
    releaseCacheEntry(cacheEntry);
}

व्योम uprv_currencyLeads(स्थिर अक्षर* locale, icu::UnicodeSet& परिणाम, UErrorCode& ec) {
    U_NAMESPACE_USE
    अगर (U_FAILURE(ec)) {
        वापस;
    }
    CurrencyNameCacheEntry* cacheEntry = getCacheEntry(locale, ec);
    अगर (U_FAILURE(ec)) {
        वापस;
    }

    क्रम (int32_t i=0; i<cacheEntry->योग_Currency_प्रतीक_गिनती; i++) {
        स्थिर CurrencyNameStruct& info = cacheEntry->currencySymbols[i];
        UChar32 cp;
        U16_GET(info.currencyName, 0, 0, info.currencyNameLen, cp);
        परिणाम.add(cp);
    }

    क्रम (int32_t i=0; i<cacheEntry->योग_Currency_Name_गिनती; i++) {
        स्थिर CurrencyNameStruct& info = cacheEntry->currencyNames[i];
        UChar32 cp;
        U16_GET(info.currencyName, 0, 0, info.currencyNameLen, cp);
        परिणाम.add(cp);
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
U_CAPI व्योम
uprv_getStaticCurrencyName(स्थिर char16_t* iso, स्थिर अक्षर* loc,
                           icu::UnicodeString& परिणाम, UErrorCode& ec)
{
    U_NAMESPACE_USE

    int32_t len;
    स्थिर char16_t* currname = ucurr_getName(iso, loc, UCURR_प्रतीक_NAME,
                                          nullptr /* isChoiceFormat */, &len, &ec);
    अगर (U_SUCCESS(ec)) {
        परिणाम.setTo(currname, len);
    }
}

U_CAPI int32_t U_EXPORT2
ucurr_getDefaultFractionDigits(स्थिर char16_t* currency, UErrorCode* ec) {
    वापस ucurr_getDefaultFractionDigitsForUsage(currency,UCURR_USAGE_STANDARD,ec);
}

U_CAPI int32_t U_EXPORT2
ucurr_getDefaultFractionDigitsForUsage(स्थिर char16_t* currency, स्थिर UCurrencyUsage usage, UErrorCode* ec) {
    int32_t fracDigits = 0;
    अगर (U_SUCCESS(*ec)) {
        चयन (usage) {
            हाल UCURR_USAGE_STANDARD:
                fracDigits = (find_Meta_तथ्य(currency, *ec))[0];
                अवरोध;
            हाल UCURR_USAGE_CASH:
                fracDigits = (find_Meta_तथ्य(currency, *ec))[2];
                अवरोध;
            शेष:
                *ec = U_UNSUPPORTED_ERROR;
        }
    }
    वापस fracDigits;
}

U_CAPI द्विगुन U_EXPORT2
ucurr_getRoundingIncrement(स्थिर char16_t* currency, UErrorCode* ec) {
    वापस ucurr_getRoundingIncrementForUsage(currency, UCURR_USAGE_STANDARD, ec);
}

U_CAPI द्विगुन U_EXPORT2
ucurr_getRoundingIncrementForUsage(स्थिर char16_t* currency, स्थिर UCurrencyUsage usage, UErrorCode* ec) {
    द्विगुन परिणाम = 0.0;

    स्थिर int32_t *तथ्य = find_Meta_तथ्य(currency, *ec);
    अगर (U_SUCCESS(*ec)) {
        int32_t fracDigits;
        int32_t increment;
        चयन (usage) {
            हाल UCURR_USAGE_STANDARD:
                fracDigits = तथ्य[0];
                increment = तथ्य[1];
                अवरोध;
            हाल UCURR_USAGE_CASH:
                fracDigits = तथ्य[2];
                increment = तथ्य[3];
                अवरोध;
            शेष:
                *ec = U_UNSUPPORTED_ERROR;
                वापस परिणाम;
        }

        // If the meta data is invalid, return 0.0
        अगर (fracDigits < 0 || fracDigits > MAX_POW10) {
            *ec = U_INVALID_FORMAT_ERROR;
        } अन्यथा {
            // A rounding value of 0 or 1 indicates no rounding.
            अगर (increment >= 2) {
                // Return (increment) / 10^(fracDigits).  The only actual rounding data,
                // as of this writing, is CHF { 2, 5 }.
                परिणाम = द्विगुन(increment) / POW10[fracDigits];
            }
        }
    }

    वापस परिणाम;
}

U_CDECL_BEGIN

प्रकार काष्ठा UCurrencyContext {
    uint32_t currType; /* UCurrCurrencyType */
    uint32_t listIdx;
} UCurrencyContext;

/*
Please keep this list in alphabetical order.
You can look at the CLDR supplemental data or ISO-4217 for the meaning of some
of these items.
ISO-4217: http://www.iso.org/iso/en/prods-services/popstds/currencycodeslist.html
*/
अटल स्थिर काष्ठा CurrencyList {
    स्थिर अक्षर *currency;
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

#घोषणा UCURR_MATCHES_BITMASK(variable, typeToMatch) \
    ((typeToMatch) == UCURR_ALL || ((variable) & (typeToMatch)) == (typeToMatch))

अटल int32_t U_CALLCONV
ucurr_गिनती_Currency_List(UEnumeration *enumerator, UErrorCode * /*pErrorCode*/) {
    UCurrencyContext *myContext = (UCurrencyContext *)(enumerator->context);
    uint32_t currType = myContext->currType;
    int32_t count = 0;

    /* Count the number of items matching the type we are looking for. */
    क्रम (int32_t idx = 0; gCurrencyList[idx].currency != nullptr; idx++) {
        अगर (UCURR_MATCHES_BITMASK(gCurrencyList[idx].currType, currType)) {
            count++;
        }
    }
    वापस count;
}

अटल स्थिर अक्षर* U_CALLCONV
ucurr_अग्र_Currency_List(UEnumeration *enumerator,
                        int32_t* परिणाम_लंबाई,
                        UErrorCode * /*pErrorCode*/)
{
    UCurrencyContext *myContext = (UCurrencyContext *)(enumerator->context);

    /* Find the next in the list that matches the type we are looking for. */
    जबतक (myContext->listIdx < UPRV_LENGTHOF(gCurrencyList)-1) {
        स्थिर काष्ठा CurrencyList *currItem = &gCurrencyList[myContext->listIdx++];
        अगर (UCURR_MATCHES_BITMASK(currItem->currType, myContext->currType))
        {
            अगर (परिणाम_लंबाई) {
                *परिणाम_लंबाई = 3; /* Currency codes are only 3 chars long */
            }
            वापस currItem->currency;
        }
    }
    /* We enumerated too far. */
    अगर (परिणाम_लंबाई) {
        *परिणाम_लंबाई = 0;
    }
    वापस nullptr;
}

अटल व्योम U_CALLCONV
ucurr_resetCurrencyList(UEnumeration *enumerator, UErrorCode * /*pErrorCode*/) {
    ((UCurrencyContext *)(enumerator->context))->listIdx = 0;
}

अटल व्योम U_CALLCONV
ucurr_closeCurrencyList(UEnumeration *enumerator) {
    uprv_free(enumerator->context);
    uprv_free(enumerator);
}

अटल व्योम U_CALLCONV
ucurr_createCurrencyList(UHashtable *isoCodes, UErrorCode* status){
    UErrorCode localStatus = U_ZERO_ERROR;

    // Look up the CurrencyMap element in the root bundle.
    UResourceBundle *rb = ures_openDirect(U_ICUDATA_CURR, CURRENCY_तथ्य, &localStatus);
    LocalUResourceBundlePointer currencyMapArray(ures_get_By_कुंजी(rb, CURRENCY_MAP, rb, &localStatus));

    अगर (U_SUCCESS(localStatus)) {
        // process each entry in currency map
        क्रम (int32_t i=0; i<ures_get_आकार(currencyMapArray.getAlias()); i++) {
            // get the currency resource
            LocalUResourceBundlePointer currencyArray(ures_get_By_सूचकांक(currencyMapArray.getAlias(), i, nullptr, &localStatus));
            // process each currency
            अगर (U_SUCCESS(localStatus)) {
                क्रम (int32_t j=0; j<ures_get_आकार(currencyArray.getAlias()); j++) {
                    // get the currency resource
                    LocalUResourceBundlePointer currencyRes(ures_get_By_सूचकांक(currencyArray.getAlias(), j, nullptr, &localStatus));
                    IsoCodeEntry *entry = (IsoCodeEntry*)uprv_malloc(माप(IsoCodeEntry));
                    अगर (entry == nullptr) {
                        *status = U_MEMORY_ALLOCATION_ERROR;
                        वापस;
                    }

                    // get the ISO code
                    int32_t iso_लंबाई = 0;
                    LocalUResourceBundlePointer idRes(ures_get_By_कुंजी(currencyRes.getAlias(), "id", nullptr, &localStatus));
                    अगर (idRes.isNull()) {
                        जारी;
                    }
                    स्थिर char16_t *isoCode = ures_getString(idRes.getAlias(), &iso_लंबाई, &localStatus);

                    // get from date
                    UDate fromDate = U_DATE_MIN;
                    LocalUResourceBundlePointer fromRes(ures_get_By_कुंजी(currencyRes.getAlias(), "from", nullptr, &localStatus));

                    अगर (U_SUCCESS(localStatus)) {
                        int32_t from_लंबाई = 0;
                        स्थिर int32_t *fromArray = ures_getIntVector(fromRes.getAlias(), &from_लंबाई, &localStatus);
                        int64_t currDate64 = ((uint64_t)fromArray[0]) << 32;
                        currDate64 |= ((int64_t)fromArray[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                        fromDate = (UDate)currDate64;
                    }

                    // get to date
                    UDate toDate = U_DATE_MAX;
                    localStatus = U_ZERO_ERROR;
                    LocalUResourceBundlePointer toRes(ures_get_By_कुंजी(currencyRes.getAlias(), "to", nullptr, &localStatus));

                    अगर (U_SUCCESS(localStatus)) {
                        int32_t to_लंबाई = 0;
                        स्थिर int32_t *toArray = ures_getIntVector(toRes.getAlias(), &to_लंबाई, &localStatus);
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

अटल स्थिर UEnumeration gEnumCurrencyList = {
    nullptr,
    nullptr,
    ucurr_closeCurrencyList,
    ucurr_गिनती_Currency_List,
    uenum_unextDefault,
    ucurr_अग्र_Currency_List,
    ucurr_resetCurrencyList
};
U_CDECL_END


अटल व्योम U_CALLCONV initIsoCodes(UErrorCode &status) {
    U_ASSERT(gIsoCodes == nullptr);
    ucln_common_registerCleanup(UCLN_COMMON_CURRENCY, currency_cleanup);

    LocalUHashtablePointer isoCodes(uhash_open(uhash_hashUChars, uhash_compareUChars, nullptr, &status));
    अगर (U_FAILURE(status)) {
        वापस;
    }
    uhash_set_मूल्य_Deleter(isoCodes.getAlias(), deleteIsoCodeEntry);

    ucurr_createCurrencyList(isoCodes.getAlias(), &status);
    अगर (U_FAILURE(status)) {
        वापस;
    }
    gIsoCodes = isoCodes.orphan();  // Note: gIsoCodes is const. Once set up here it is never altered,
                                    //       and read only access is safe without synchronization.
}

अटल व्योम populateCurrSymbolsEquiv(icu::Hashtable *hash, UErrorCode &status) {
    अगर (U_FAILURE(status)) { वापस; }
    क्रम (स्थिर स्वतः& entry : unisets::kCurrencyEntries) {
        UnicodeString exemplar(entry.exemplar);
        स्थिर UnicodeSet* set = unisets::लो(entry.कुंजी);
        अगर (set == nullptr) { वापस; }
        UnicodeSetIterator it(*set);
        जबतक (it.next()) {
            UnicodeString value = it.getString();
            अगर (value == exemplar) {
                // No need to mark the exemplar character as an equivalent
                जारी;
            }
            makeEquivalent(exemplar, value, hash, status);
            अगर (U_FAILURE(status)) { वापस; }
        }
    }
}

अटल व्योम U_CALLCONV initCurrSymbolsEquiv() {
    U_ASSERT(gCurrSymbolsEquiv == nullptr);
    UErrorCode status = U_ZERO_ERROR;
    ucln_common_registerCleanup(UCLN_COMMON_CURRENCY, currency_cleanup);
    icu::Hashtable *अस्थायी = नया icu::Hashtable(status);
    अगर (अस्थायी == nullptr) {
        वापस;
    }
    अगर (U_FAILURE(status)) {
        मिटाओ अस्थायी;
        वापस;
    }
    अस्थायी->set_मूल्य_Deleter(deleteUnicode);
    populateCurrSymbolsEquiv(अस्थायी, status);
    अगर (U_FAILURE(status)) {
        मिटाओ अस्थायी;
        वापस;
    }
    gCurrSymbolsEquiv = अस्थायी;
}

U_CAPI UBool U_EXPORT2
ucurr_isAvailable(स्थिर char16_t* isoCode, UDate from, UDate to, UErrorCode* eErrorCode) {
    umtx_initOnce(gIsoCodesInitOnce, &initIsoCodes, *eErrorCode);
    अगर (U_FAILURE(*eErrorCode)) {
        वापस false;
    }

    IsoCodeEntry* परिणाम = (IsoCodeEntry *) uhash_get(gIsoCodes, isoCode);
    अगर (परिणाम == nullptr) {
        वापस false;
    } अन्यथा अगर (from > to) {
        *eErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        वापस false;
    } अन्यथा अगर  ((from > परिणाम->to) || (to < परिणाम->from)) {
        वापस false;
    }
    वापस true;
}

अटल स्थिर icu::Hashtable* getCurrSymbolsEquiv() {
    umtx_initOnce(gCurrSymbolsEquivInitOnce, &initCurrSymbolsEquiv);
    वापस gCurrSymbolsEquiv;
}

U_CAPI UEnumeration * U_EXPORT2
ucurr_openISOCurrencies(uint32_t currType, UErrorCode *pErrorCode) {
    UEnumeration *myEnum = nullptr;
    UCurrencyContext *myContext;

    myEnum = (UEnumeration*)uprv_malloc(माप(UEnumeration));
    अगर (myEnum == nullptr) {
        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
        वापस nullptr;
    }
    uprv_memcpy(myEnum, &gEnumCurrencyList, माप(UEnumeration));
    myContext = (UCurrencyContext*)uprv_malloc(माप(UCurrencyContext));
    अगर (myContext == nullptr) {
        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
        uprv_free(myEnum);
        वापस nullptr;
    }
    myContext->currType = currType;
    myContext->listIdx = 0;
    myEnum->context = myContext;
    वापस myEnum;
}

U_CAPI int32_t U_EXPORT2
ucurr_गिनती_Currencies(स्थिर अक्षर* locale, 
                 UDate date, 
                 UErrorCode* ec)
{
    int32_t curr_गिनती = 0;

    अगर (ec != nullptr && U_SUCCESS(*ec)) 
    {
        // local variables
        UErrorCode localStatus = U_ZERO_ERROR;

        // get country or country_variant in `id'
        CharString id = idForLocale(locale, ec);

        अगर (U_FAILURE(*ec))
        {
            वापस 0;
        }

        // Remove variants, which is only needed for registration.
        अक्षर *idDelim = म_अक्षर(id.तथ्य(), VAR_DELIM);
        अगर (idDelim)
        {
            id.truncate(idDelim - id.तथ्य());
        }

        // Look up the CurrencyMap element in the root bundle.
        UResourceBundle *rb = ures_openDirect(U_ICUDATA_CURR, CURRENCY_तथ्य, &localStatus);
        UResourceBundle *cm = ures_get_By_कुंजी(rb, CURRENCY_MAP, rb, &localStatus);

        // Using the id derived from the local, get the currency data
        LocalUResourceBundlePointer countryArray(ures_get_By_कुंजी(rb, id.तथ्य(), cm, &localStatus));

        // process each currency to see which one is valid for the given date
        अगर (U_SUCCESS(localStatus))
        {
            क्रम (int32_t i=0; i<ures_get_आकार(countryArray.getAlias()); i++)
            {
                // get the currency resource
                LocalUResourceBundlePointer currencyRes(ures_get_By_सूचकांक(countryArray.getAlias(), i, nullptr, &localStatus));

                // get the from date
                int32_t from_लंबाई = 0;
                LocalUResourceBundlePointer fromRes(ures_get_By_कुंजी(currencyRes.getAlias(), "from", nullptr, &localStatus));
                स्थिर int32_t *fromArray = ures_getIntVector(fromRes.getAlias(), &from_लंबाई, &localStatus);

                int64_t currDate64 = (int64_t)((uint64_t)(fromArray[0]) << 32);
                currDate64 |= ((int64_t)fromArray[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                UDate fromDate = (UDate)currDate64;

                अगर (ures_get_आकार(currencyRes.getAlias())> 2)
                {
                    int32_t to_लंबाई = 0;
                    LocalUResourceBundlePointer toRes(ures_get_By_कुंजी(currencyRes.getAlias(), "to", nullptr, &localStatus));
                    स्थिर int32_t *toArray = ures_getIntVector(toRes.getAlias(), &to_लंबाई, &localStatus);

                    currDate64 = (int64_t)toArray[0] << 32;
                    currDate64 |= ((int64_t)toArray[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                    UDate toDate = (UDate)currDate64;

                    अगर ((fromDate <= date) && (date < toDate))
                    {
                        curr_गिनती++;
                    }
                }
                अन्यथा
                {
                    अगर (fromDate <= date)
                    {
                        curr_गिनती++;
                    }
                }
            } // end For loop
        } // end if (U_SUCCESS(localStatus))

        // Check for errors
        अगर (*ec == U_ZERO_ERROR || localStatus != U_ZERO_ERROR)
        {
            // There is nothing to fallback to. 
            // Report the failure/warning if possible.
            *ec = localStatus;
        }

        अगर (U_SUCCESS(*ec))
        {
            // no errors
            वापस curr_गिनती;
        }
    }

    // If we got here, either error code is invalid or
    // some argument passed is no good.
    वापस 0;
}

U_CAPI int32_t U_EXPORT2 
ucurr_forLocaleAndDate(स्थिर अक्षर* locale, 
                UDate date, 
                int32_t सूचकांक,
                char16_t* buff,
                int32_t buffCapacity, 
                UErrorCode* ec)
{
    int32_t resLen = 0;
	int32_t curr_सूचकांक = 0;
    स्थिर char16_t* s = nullptr;

    अगर (ec != nullptr && U_SUCCESS(*ec))
    {
        // check the arguments passed
        अगर ((buff && buffCapacity) || !buffCapacity )
        {
            // local variables
            UErrorCode localStatus = U_ZERO_ERROR;

            // get country or country_variant in `id'
            CharString id = idForLocale(locale, ec);
            अगर (U_FAILURE(*ec))
            {
                वापस 0;
            }

            // Remove variants, which is only needed for registration.
            अक्षर *idDelim = म_अक्षर(id.तथ्य(), VAR_DELIM);
            अगर (idDelim)
            {
                id.truncate(idDelim - id.तथ्य());
            }

            // Look up the CurrencyMap element in the root bundle.
            UResourceBundle *rb = ures_openDirect(U_ICUDATA_CURR, CURRENCY_तथ्य, &localStatus);
            UResourceBundle *cm = ures_get_By_कुंजी(rb, CURRENCY_MAP, rb, &localStatus);

            // Using the id derived from the local, get the currency data
            LocalUResourceBundlePointer countryArray(ures_get_By_कुंजी(rb, id.तथ्य(), cm, &localStatus));

            // process each currency to see which one is valid for the given date
            bool matchFound = false;
            अगर (U_SUCCESS(localStatus))
            {
                अगर ((सूचकांक <= 0) || (सूचकांक> ures_get_आकार(countryArray.getAlias())))
                {
                    // requested index is out of bounds
                    वापस 0;
                }

                क्रम (int32_t i=0; i<ures_get_आकार(countryArray.getAlias()); i++)
                {
                    // get the currency resource
                    LocalUResourceBundlePointer currencyRes(ures_get_By_सूचकांक(countryArray.getAlias(), i, nullptr, &localStatus));
                    s = ures_get_String_By_कुंजी(currencyRes.getAlias(), "id", &resLen, &localStatus);

                    // get the from date
                    int32_t from_लंबाई = 0;
                    LocalUResourceBundlePointer fromRes(ures_get_By_कुंजी(currencyRes.getAlias(), "from", nullptr, &localStatus));
                    स्थिर int32_t *fromArray = ures_getIntVector(fromRes.getAlias(), &from_लंबाई, &localStatus);

                    int64_t currDate64 = (int64_t)((uint64_t)fromArray[0] << 32);
                    currDate64 |= ((int64_t)fromArray[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                    UDate fromDate = (UDate)currDate64;

                    अगर (ures_get_आकार(currencyRes.getAlias()) > 2)
                    {
                        int32_t to_लंबाई = 0;
                        LocalUResourceBundlePointer toRes(ures_get_By_कुंजी(currencyRes.getAlias(), "to", nullptr, &localStatus));
                        स्थिर int32_t *toArray = ures_getIntVector(toRes.getAlias(), &to_लंबाई, &localStatus);

                        currDate64 = (int64_t)toArray[0] << 32;
                        currDate64 |= ((int64_t)toArray[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                        UDate toDate = (UDate)currDate64;

                        अगर ((fromDate <= date) && (date < toDate))
                        {
                            curr_सूचकांक++;
                            अगर (curr_सूचकांक == सूचकांक)
                            {
                                matchFound = true;
                            }
                        }
                    }
                    अन्यथा
                    {
                        अगर (fromDate <= date)
                        {
                            curr_सूचकांक++;
                            अगर (curr_सूचकांक == सूचकांक)
                            {
                                matchFound = true;
                            }
                        }
                    }
                    // check for loop exit
                    अगर (matchFound)
                    {
                        अवरोध;
                    }

                } // end For loop
            }

            // Check for errors
            अगर (*ec == U_ZERO_ERROR || localStatus != U_ZERO_ERROR)
            {
                // There is nothing to fallback to. 
                // Report the failure/warning if possible.
                *ec = localStatus;
            }

            अगर (U_SUCCESS(*ec))
            {
                // no errors
                अगर((buffCapacity> resLen) && matchFound)
                {
                    // write out the currency value
                    u_strcpy(buff, s);
                }
                अन्यथा
                {
                    वापस 0;
                }
            }

            // return null terminated currency string
            वापस u_terminateUChars(buff, buffCapacity, resLen, ec);
        }
        अन्यथा
        {
            // illegal argument encountered
            *ec = U_ILLEGAL_ARGUMENT_ERROR;
        }

    }

    // If we got here, either error code is invalid or
    // some argument passed is no good.
    वापस resLen;
}

अटल स्थिर UEnumeration default_Keyword_मूल्यावली = {
    nullptr,
    nullptr,
    ulist_close_keyword_मूल्यावली_iterator,
    ulist_गिनती_keyword_मूल्यावली,
    uenum_unextDefault,
    ulist_अग्र_keyword_मूल्य, 
    ulist_reset_keyword_मूल्यावली_iterator
};

U_CAPI UEnumeration *U_EXPORT2 ucurr_get_Keyword_मूल्यावली_For_Locale(स्थिर अक्षर *कुंजी, स्थिर अक्षर *locale, UBool commonlyUsed, UErrorCode* status) {
    // Resolve region
    CharString prefRegion = ulocimp_get_Region_For_Supplemental_तथ्य(locale, true, *status);

    // Read value from supplementalData
    UList *मूल्यावली = ulist_createEmptyList(status);
    UList *other_मूल्यावली = ulist_createEmptyList(status);
    UEnumeration *en = (UEnumeration *)uprv_malloc(माप(UEnumeration));
    अगर (U_FAILURE(*status) || en == nullptr) {
        अगर (en == nullptr) {
            *status = U_MEMORY_ALLOCATION_ERROR;
        } अन्यथा {
            uprv_free(en);
        }
        ulist_deleteList(मूल्यावली);
        ulist_deleteList(other_मूल्यावली);
        वापस nullptr;
    }
    स_नकल(en, &default_Keyword_मूल्यावली, माप(UEnumeration));
    en->context = मूल्यावली;
    
    UResourceBundle* rb = ures_openDirect(U_ICUDATA_CURR, "supplementalData", status);
    LocalUResourceBundlePointer bundle(ures_get_By_कुंजी(rb, "CurrencyMap", rb, status));
    ढेर_UResource_Bundle bundlekey, regbndl, curbndl, to;
    
    जबतक (U_SUCCESS(*status) && ures_has_अग्र(bundle.getAlias())) {
        ures_get_अग्र_Resource(bundle.getAlias(), bundlekey.getAlias(), status);
        अगर (U_FAILURE(*status)) {
            अवरोध;
        }
        स्थिर अक्षर *region = ures_get_कुंजी(bundlekey.getAlias());
        UBool isPrefRegion = prefRegion == region;
        अगर (!isPrefRegion && commonlyUsed) {
            // With commonlyUsed=true, we do not put
            // currencies for other regions in the
            // result list.
            जारी;
        }
        ures_get_By_कुंजी(bundle.getAlias(), region, regbndl.getAlias(), status);
        अगर (U_FAILURE(*status)) {
            अवरोध;
        }
        जबतक (U_SUCCESS(*status) && ures_has_अग्र(regbndl.getAlias())) {
            ures_get_अग्र_Resource(regbndl.getAlias(), curbndl.getAlias(), status);
            अगर (ures_getType(curbndl.getAlias()) != URES_TABLE) {
                // Currently, an empty ARRAY is mixed in.
                जारी;
            }
            अक्षर *curID = (अक्षर *)uprv_malloc(माप(अक्षर) * ULOC_KEYWORDS_CAPACITY);
            अगर (curID == nullptr) {
                *status = U_MEMORY_ALLOCATION_ERROR;
                अवरोध;
            }
            int32_t curIDLength = ULOC_KEYWORDS_CAPACITY;

#अगर U_CHARSET_FAMILY==U_ASCII_FAMILY
            ures_get_UTF8_String_By_कुंजी(curbndl.getAlias(), "id", curID, &curIDLength, true, status);
            /* optimize - use the utf-8 string */
#अन्यथा
            {
                       स्थिर char16_t* defString = ures_get_String_By_कुंजी(curbndl.getAlias(), "id", &curIDLength, status);
                       अगर(U_SUCCESS(*status)) {
			   अगर(curIDLength+1 > ULOC_KEYWORDS_CAPACITY) {
				*status = U_बफर_OVERFLOW_ERROR;
			   } अन्यथा {
                           	u_UCharsToChars(defString, curID, curIDLength+1);
			   }
                       }
            }
#पूर्ण_अगर	

            अगर (U_FAILURE(*status)) {
                अवरोध;
            }
            UBool hasTo = false;
            ures_get_By_कुंजी(curbndl.getAlias(), "to", to.getAlias(), status);
            अगर (U_FAILURE(*status)) {
                // Do nothing here...
                *status = U_ZERO_ERROR;
            } अन्यथा {
                hasTo = true;
            }
            अगर (isPrefRegion && !hasTo && !ulist_containsString(मूल्यावली, curID, (int32_t)uprv_strlen(curID))) {
                // Currently active currency for the target country
                ulist_addItemEndList(मूल्यावली, curID, true, status);
            } अन्यथा अगर (!ulist_containsString(other_मूल्यावली, curID, (int32_t)uprv_strlen(curID)) && !commonlyUsed) {
                ulist_addItemEndList(other_मूल्यावली, curID, true, status);
            } अन्यथा {
                uprv_free(curID);
            }
        }
        
    }
    अगर (U_SUCCESS(*status)) {
        अगर (commonlyUsed) {
            अगर (ulist_get_List_आकार(मूल्यावली) == 0) {
                // This could happen if no valid region is supplied in the input
                // locale. In this case, we use the CLDR's default.
                uenum_close(en);
                en = ucurr_get_Keyword_मूल्यावली_For_Locale(कुंजी, "und", true, status);
            }
        } अन्यथा {
            // Consolidate the list
            अक्षर *value = nullptr;
            ulist_resetList(other_मूल्यावली);
            जबतक ((value = (अक्षर *)ulist_get_अग्र(other_मूल्यावली)) != nullptr) {
                अगर (!ulist_containsString(मूल्यावली, value, (int32_t)uprv_strlen(value))) {
                    अक्षर *tmp_मूल्य = (अक्षर *)uprv_malloc(माप(अक्षर) * ULOC_KEYWORDS_CAPACITY);
                    अगर (tmp_मूल्य == nullptr) {
                        *status = U_MEMORY_ALLOCATION_ERROR;
                        अवरोध;
                    }
                    uprv_memcpy(tmp_मूल्य, value, uprv_strlen(value) + 1);
                    ulist_addItemEndList(मूल्यावली, tmp_मूल्य, true, status);
                    अगर (U_FAILURE(*status)) {
                        अवरोध;
                    }
                }
            }
        }
        ulist_resetList((UList *)(en->context));
    } अन्यथा {
        ulist_deleteList(मूल्यावली);
        uprv_free(en);
        मूल्यावली = nullptr;
        en = nullptr;
    }
    ulist_deleteList(other_मूल्यावली);
    वापस en;
}


U_CAPI int32_t U_EXPORT2
ucurr_getNumericCode(स्थिर char16_t* currency) {
    int32_t code = 0;
    अगर (currency && u_strlen(currency) == ISO_CURRENCY_CODE_लंबाई) {
        UErrorCode status = U_ZERO_ERROR;

        UResourceBundle* bundle = ures_openDirect(nullptr, "currencyNumericCodes", &status);
        LocalUResourceBundlePointer codeMap(ures_get_By_कुंजी(bundle, "codeMap", bundle, &status));
        अगर (U_SUCCESS(status)) {
            अक्षर alphaCode[ISO_CURRENCY_CODE_लंबाई+1];
            myUCharsToChars(alphaCode, currency);
            T_CString_toUpperCase(alphaCode);
            ures_get_By_कुंजी(codeMap.getAlias(), alphaCode, codeMap.getAlias(), &status);
            पूर्णांक tmpCode = ures_getInt(codeMap.getAlias(), &status);
            अगर (U_SUCCESS(status)) {
                code = tmpCode;
            }
        }
    }
    वापस code;
}
#पूर्ण_अगर /* #if !UCONFIG_NO_FORMATTING */

//eof
