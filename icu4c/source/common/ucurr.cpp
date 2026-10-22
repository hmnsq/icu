// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
* Copyright (c) 2002-2016, International Business Machines
* Corporation and others.  All Rights Reserved.
**********************************************************************
*/

#અન્તર્ભાવ "unicode/utypes.h"

#જો !UCONFIG_NO_FORMATTING

#અન્તર્ભાવ <utility>

#અન્તર્ભાવ "unicode/ucurr.h"
#અન્તર્ભાવ "unicode/locid.h"
#અન્તર્ભાવ "unicode/ures.h"
#અન્તર્ભાવ "unicode/ustring.h"
#અન્તર્ભાવ "unicode/parsepos.h"
#અન્તર્ભાવ "unicode/uniset.h"
#અન્તર્ભાવ "unicode/usetiter.h"
#અન્તર્ભાવ "unicode/utf16.h"
#અન્તર્ભાવ "ustr_imp.h"
#અન્તર્ભાવ "charstr.h"
#અન્તર્ભાવ "cmemory.h"
#અન્તર્ભાવ "cstring.h"
#અન્તર્ભાવ "static_unicode_sets.h"
#અન્તર્ભાવ "uassert.h"
#અન્તર્ભાવ "umutex.h"
#અન્તર્ભાવ "ucln_cmn.h"
#અન્તર્ભાવ "uenumimp.h"
#અન્તર્ભાવ "uhash.h"
#અન્તર્ભાવ "hash.h"
#અન્તર્ભાવ "uinvchar.h"
#અન્તર્ભાવ "uresimp.h"
#અન્તર્ભાવ "ulist.h"
#અન્તર્ભાવ "uresimp.h"
#અન્તર્ભાવ "ureslocs.h"
#અન્તર્ભાવ "ulocimp.h"

using નામાકાશ icu;

//#define UCURR_DEBUG_EQUIV 1
#ifdef UCURR_DEBUG_EQUIV
#અન્તર્ભાવ "stdio.h"
#endif
//#define UCURR_DEBUG 1
#ifdef UCURR_DEBUG
#અન્તર્ભાવ "stdio.h"
#endif

પ્રકારવ્યાખ્યા સંરચના IsoCodeEntry {
    અચલ char16_t *isoCode; /* const because it's a reference to a resource bundle string. */
    UDate from;
    UDate to;
} IsoCodeEntry;

//------------------------------------------------------------
// Constants

// Default currency meta data of last resort.  We try to use the
// defaults encoded in the meta data resource bundle.  If there is a
// configuration/build error and these are not available, we use these
// hard-coded defaults (which should be identical).
સ્થિર અચલ int32_t LAST_RESORT_DATA[] = { 2, 0, 2, 0 };

// POW10[i] = 10^i, i=0..MAX_POW10
સ્થિર અચલ int32_t POW10[] = { 1, 10, 100, 1000, 10000, 100000,
                                 1000000, 10000000, 100000000, 1000000000 };

સ્થિર અચલ int32_t MAX_POW10 = UPRV_LENGTHOF(POW10) - 1;

#define ISO_CURRENCY_CODE_LENGTH 3

//------------------------------------------------------------
// Resource tags
//

સ્થિર અચલ અક્ષર CURRENCY_DATA[] = "supplementalData";
// Tag for meta-data, in root.
સ્થિર અચલ અક્ષર CURRENCY_META[] = "CurrencyMeta";

// Tag for map from countries to currencies, in root.
સ્થિર અચલ અક્ષર CURRENCY_MAP[] = "CurrencyMap";

// Tag for default meta-data, in CURRENCY_META
સ્થિર અચલ અક્ષર DEFAULT_META[] = "DEFAULT";

// Variant delimiter
સ્થિર અચલ અક્ષર VAR_DELIM = '_';

// Tag for localized display names (symbols) of currencies
સ્થિર અચલ અક્ષર CURRENCIES[] = "Currencies";
સ્થિર અચલ અક્ષર CURRENCIES_NARROW[] = "Currencies%narrow";
સ્થિર અચલ અક્ષર CURRENCIES_FORMAL[] = "Currencies%formal";
સ્થિર અચલ અક્ષર CURRENCIES_VARIANT[] = "Currencies%variant";
સ્થિર અચલ અક્ષર CURRENCYPLURALS[] = "CurrencyPlurals";

// ISO codes mapping table
સ્થિર અચલ UHashtable* gIsoCodes = nullptr;
સ્થિર icu::UInitOnce gIsoCodesInitOnce {};

// Currency symbol equivalances
સ્થિર અચલ icu::Hashtable* gCurrSymbolsEquiv = nullptr;
સ્થિર icu::UInitOnce gCurrSymbolsEquivInitOnce {};

U_NAMESPACE_BEGIN

// EquivIterator iterates over all strings that are equivalent to a given
// string, s. Note that EquivIterator will never yield s itself.
વર્ગ EquivIterator : જાહેર icu::UMemory {
જાહેર:
    // Constructor. hash stores the equivalence relationships; s is the string
    // for which we find equivalent strings.
    અંતઃરેખ EquivIterator(અચલ icu::Hashtable& hash, અચલ icu::UnicodeString& s)
        : _hash(hash) { 
        _start = _current = &s;
    }
    અંતઃરેખ ~EquivIterator() { }

    // next returns the next equivalent string or nullptr if there are no more.
    // If s has no equivalent strings, next returns nullptr on the first call.
    અચલ icu::UnicodeString *next();
ખાનગી:
    અચલ icu::Hashtable& _hash;
    અચલ icu::UnicodeString* _start;
    અચલ icu::UnicodeString* _current;
};

અચલ icu::UnicodeString *
EquivIterator::next() {
    અચલ icu::UnicodeString* _next = static_cast<અચલ icu::UnicodeString*>(_hash.get(*_current));
    જો (_next == nullptr) {
        U_ASSERT(_current == _start);
        પાછા nullptr;
    }
    જો (*_next == *_start) {
        પાછા nullptr;
    }
    _current = _next;
    પાછા _next;
}

U_NAMESPACE_END

// makeEquivalent makes lhs and rhs equivalent by updating the equivalence
// relations in hash accordingly.
સ્થિર શૂન્ય makeEquivalent(
    અચલ icu::UnicodeString &lhs,
    અચલ icu::UnicodeString &rhs,
    icu::Hashtable* hash, UErrorCode &status) {
    જો (U_FAILURE(status)) {
        પાછા;
    }
    જો (lhs == rhs) {
        // already equivalent
        પાછા;
    }
    icu::EquivIterator ડાબું_Iter(*hash, lhs);
    icu::EquivIterator જમણું_Iter(*hash, rhs);
    અચલ icu::UnicodeString *first_ડાબું = ડાબું_Iter.next();
    અચલ icu::UnicodeString *first_જમણું = જમણું_Iter.next();
    અચલ icu::UnicodeString *next_ડાબું = first_ડાબું;
    અચલ icu::UnicodeString *next_જમણું = first_જમણું;
    જ્યારે (next_ડાબું != nullptr && next_જમણું != nullptr) {
        જો (*next_ડાબું == rhs || *next_જમણું == lhs) {
            // Already equivalent
            પાછા;
        }
        next_ડાબું = ડાબું_Iter.next();
        next_જમણું = જમણું_Iter.next();
    }
    // Not equivalent. Must join.
    icu::UnicodeString *new_First_ડાબું;
    icu::UnicodeString *new_First_જમણું;
    જો (first_જમણું == nullptr && first_ડાબું == nullptr) {
        // Neither lhs or rhs belong to an equivalence circle, so we form
        // a new equivalnce circle of just lhs and rhs.
        new_First_ડાબું = નવું icu::UnicodeString(rhs);
        new_First_જમણું = નવું icu::UnicodeString(lhs);
    } નહીંતર જો (first_જમણું == nullptr) {
        // lhs belongs to an equivalence circle, but rhs does not, so we link
        // rhs into lhs' circle.
        new_First_ડાબું = નવું icu::UnicodeString(rhs);
        new_First_જમણું = નવું icu::UnicodeString(*first_ડાબું);
    } નહીંતર જો (first_ડાબું == nullptr) {
        // rhs belongs to an equivlance circle, but lhs does not, so we link
        // lhs into rhs' circle.
        new_First_ડાબું = નવું icu::UnicodeString(*first_જમણું);
        new_First_જમણું = નવું icu::UnicodeString(lhs);
    } નહીંતર {
        // Both lhs and rhs belong to different equivalnce circles. We link
        // them together to form one single, larger equivalnce circle.
        new_First_ડાબું = નવું icu::UnicodeString(*first_જમણું);
        new_First_જમણું = નવું icu::UnicodeString(*first_ડાબું);
    }
    જો (new_First_ડાબું == nullptr || new_First_જમણું == nullptr) {
        delete new_First_ડાબું;
        delete new_First_જમણું;
        status = U_MEMORY_ALLOCATION_ERROR;
        પાછા;
    }
    hash->put(lhs, (શૂન્ય *) new_First_ડાબું, status);
    hash->put(rhs, (શૂન્ય *) new_First_જમણું, status);
}

// countEquivalent counts how many strings are equivalent to s.
// hash stores all the equivalnce relations.
// countEquivalent does not include s itself in the count.
સ્થિર int32_t ગણતરી_Equivalent(અચલ icu::Hashtable &hash, અચલ icu::UnicodeString &s) {
    int32_t result = 0;
    icu::EquivIterator iter(hash, s);
    જ્યારે (iter.next() != nullptr) {
        ++result;
    }
#ifdef UCURR_DEBUG_EQUIV
 {
   અક્ષર tmp[200];
   s.extract(0,s.length(),tmp, "UTF-8");
   printf("CountEquivalent('%s') = %d\n", tmp, result);
 }
#endif
    પાછા result;
}

સ્થિર અચલ icu::Hashtable* getCurrSymbolsEquiv();

//------------------------------------------------------------
// Code

/**
 * Cleanup callback func
 */
સ્થિર UBool U_CALLCONV 
isoCodes_cleanup()
{
    જો (gIsoCodes != nullptr) {
        uhash_close(const_cast<UHashtable *>(gIsoCodes));
        gIsoCodes = nullptr;
    }
    gIsoCodesInitOnce.reset();
    પાછા સાચું;
}

/**
 * Cleanup callback func
 */
સ્થિર UBool U_CALLCONV 
currSymbolsEquiv_cleanup()
{
    delete const_cast<icu::Hashtable *>(gCurrSymbolsEquiv);
    gCurrSymbolsEquiv = nullptr;
    gCurrSymbolsEquivInitOnce.reset();
    પાછા સાચું;
}

/**
 * Deleter for IsoCodeEntry
 */
સ્થિર શૂન્ય U_CALLCONV
deleteIsoCodeEntry(શૂન્ય *obj) {
    IsoCodeEntry* entry = static_cast<IsoCodeEntry*>(obj);
    uprv_free(entry);
}

/**
 * Deleter for gCurrSymbolsEquiv.
 */
સ્થિર શૂન્ય U_CALLCONV
deleteUnicode(શૂન્ય *obj) {
    icu::UnicodeString* entry = static_cast<icu::UnicodeString*>(obj);
    delete entry;
}

/**
 * Unfortunately, we have to convert the char16_t* currency code to char*
 * to use it as a resource key.
 */
સ્થિર અંતઃરેખ અક્ષર*
myUCharsToChars(અક્ષર* resultOfLen4, અચલ char16_t* currency) {
    u_UCharsToChars(currency, resultOfLen4, ISO_CURRENCY_CODE_LENGTH);
    resultOfLen4[ISO_CURRENCY_CODE_LENGTH] = 0;
    પાછા resultOfLen4;
}

/**
 * Internal function to look up currency data.  Result is an array of
 * four integers.  The first is the fraction digits.  The second is the
 * rounding increment, or 0 if none.  The rounding increment is in
 * units of 10^(-fraction_digits).  The third and fourth are the same
 * except that they are those used in cash transactions ( cashDigits
 * and cashRounding ).
 */
સ્થિર અચલ int32_t*
_findMetaData(અચલ char16_t* currency, UErrorCode& ec) {

    જો (currency == nullptr || *currency == 0) {
        જો (U_SUCCESS(ec)) {
            ec = U_ILLEGAL_ARGUMENT_ERROR;
        }
        પાછા LAST_RESORT_DATA;
    }

    // Get CurrencyMeta resource out of root locale file.  [This may
    // move out of the root locale file later; if it does, update this
    // code.]
    UResourceBundle* currencyData = ures_openDirect(U_ICUDATA_CURR, CURRENCY_DATA, &ec);
    LocalUResourceBundlePointer currencyMeta(ures_get_By_ચાવી(currencyData, CURRENCY_META, currencyData, &ec));

    જો (U_FAILURE(ec)) {
        // Config/build error; return hard-coded defaults
        પાછા LAST_RESORT_DATA;
    }

    // Look up our currency, or if that's not available, then DEFAULT
    અક્ષર buf[ISO_CURRENCY_CODE_LENGTH+1];
    UErrorCode ec2 = U_ZERO_ERROR; // local error code: soft failure
    LocalUResourceBundlePointer rb(ures_get_By_ચાવી(currencyMeta.getAlias(), myUCharsToChars(buf, currency), nullptr, &ec2));
      જો (U_FAILURE(ec2)) {
        rb.adoptInstead(ures_get_By_ચાવી(currencyMeta.getAlias(),DEFAULT_META, nullptr, &ec));
        જો (U_FAILURE(ec)) {
            // Config/build error; return hard-coded defaults
            પાછા LAST_RESORT_DATA;
        }
    }

    int32_t len;
    અચલ int32_t *data = ures_getIntVector(rb.getAlias(), &len, &ec);
    જો (U_FAILURE(ec) || len != 4) {
        // Config/build error; return hard-coded defaults
        જો (U_SUCCESS(ec)) {
            ec = U_INVALID_FORMAT_ERROR;
        }
        પાછા LAST_RESORT_DATA;
    }

    પાછા data;
}

// -------------------------------------

સ્થિર CharString
idForLocale(અચલ અક્ષર* locale, UErrorCode* ec)
{
    પાછા ulocimp_getRegionForSupplementalData(locale, ખોટું, *ec);
}

// ------------------------------------------
//
// Registration
//
//-------------------------------------------

// don't use ICUService since we don't need fallback

U_CDECL_BEGIN
સ્થિર UBool U_CALLCONV currency_cleanup();
U_CDECL_END

#જો !UCONFIG_NO_SERVICE
સંરચના CReg;

સ્થિર UMutex gCRegLock;
સ્થિર CReg* gCRegHead = nullptr;

સંરચના CReg : જાહેર icu::UMemory {
    CReg *next;
    char16_t iso[ISO_CURRENCY_CODE_LENGTH+1];
    અક્ષર  id[ULOC_FULLNAME_CAPACITY];

    CReg(અચલ char16_t* _iso, અચલ અક્ષર* _id)
        : next(nullptr)
    {
        uprv_strncpy(id, _id, કદ(id)-1);
        id[કદ(id)-1] = 0;
        u_memcpy(iso, _iso, ISO_CURRENCY_CODE_LENGTH);
        iso[ISO_CURRENCY_CODE_LENGTH] = 0;
    }

    સ્થિર UCurr_Registry_ચાવી reg(અચલ char16_t* _iso, અચલ અક્ષર* _id, UErrorCode* status)
    {
        જો (status && U_SUCCESS(*status) && _iso && _id) {
            CReg* n = નવું CReg(_iso, _id);
            જો (n) {
                umtx_lock(&gCRegLock);
                જો (!gCRegHead) {
                    /* register for the first time */
                    ucln_common_registerCleanup(UCLN_COMMON_CURRENCY, currency_cleanup);
                }
                n->next = gCRegHead;
                gCRegHead = n;
                umtx_unlock(&gCRegLock);
                પાછા n;
            }
            *status = U_MEMORY_ALLOCATION_ERROR;
        }
        પાછા nullptr;
    }

    સ્થિર UBool unreg(UCurr_Registry_ચાવી ચાવી) {
        UBool found = ખોટું;
        umtx_lock(&gCRegLock);

        CReg** p = &gCRegHead;
        જ્યારે (*p) {
            જો (*p == ચાવી) {
                *p = ((CReg*)ચાવી)->next;
                delete (CReg*)ચાવી;
                found = સાચું;
                તોડો;
            }
            p = &((*p)->next);
        }

        umtx_unlock(&gCRegLock);
        પાછા found;
    }

    સ્થિર અચલ char16_t* get(અચલ અક્ષર* id) {
        અચલ char16_t* result = nullptr;
        umtx_lock(&gCRegLock);
        CReg* p = gCRegHead;

        /* register cleanup of the mutex */
        ucln_common_registerCleanup(UCLN_COMMON_CURRENCY, currency_cleanup);
        જ્યારે (p) {
            જો (uprv_strcmp(id, p->id) == 0) {
                result = p->iso;
                તોડો;
            }
            p = p->next;
        }
        umtx_unlock(&gCRegLock);
        પાછા result;
    }

    /* This doesn't need to be thread safe. It's for u_cleanup only. */
    સ્થિર શૂન્ય cleanup() {
        જ્યારે (gCRegHead) {
            CReg* n = gCRegHead;
            gCRegHead = gCRegHead->next;
            delete n;
        }
    }
};

// -------------------------------------

U_CAPI UCurr_Registry_ચાવી U_EXPORT2
ucurr_register(અચલ char16_t* isoCode, અચલ અક્ષર* locale, UErrorCode *status)
{
    જો (status && U_SUCCESS(*status)) {
        CharString id = idForLocale(locale, status);
        પાછા CReg::reg(isoCode, id.data(), status);
    }
    પાછા nullptr;
}

// -------------------------------------

U_CAPI UBool U_EXPORT2
ucurr_unregister(UCurr_Registry_ચાવી ચાવી, UErrorCode* status)
{
    જો (status && U_SUCCESS(*status)) {
        પાછા CReg::unreg(ચાવી);
    }
    પાછા ખોટું;
}
#endif /* UCONFIG_NO_SERVICE */

// -------------------------------------

/**
 * Release all static memory held by currency.
 */
/*The declaration here is needed so currency_cleanup()
 * can call this function.
 */
સ્થિર UBool U_CALLCONV
currency_cache_cleanup();

U_CDECL_BEGIN
સ્થિર UBool U_CALLCONV currency_cleanup() {
#જો !UCONFIG_NO_SERVICE
    CReg::cleanup();
#endif
    /*
     * There might be some cached currency data or isoCodes data.
     */
    currency_cache_cleanup();
    isoCodes_cleanup();
    currSymbolsEquiv_cleanup();

    પાછા સાચું;
}
U_CDECL_END

// -------------------------------------

U_CAPI int32_t U_EXPORT2
ucurr_forLocale(અચલ અક્ષર* locale,
                char16_t* buff,
                int32_t buffCapacity,
                UErrorCode* ec) {
    જો (U_FAILURE(*ec)) { પાછા 0; }
    જો (buffCapacity < 0 || (buff == nullptr && buffCapacity > 0)) {
        *ec = U_ILLEGAL_ARGUMENT_ERROR;
        પાછા 0;
    }

    UErrorCode localStatus = U_ZERO_ERROR;
    CharString currency = ulocimp_get_Keyword_કિંમત(locale, "currency", localStatus);
    int32_t resLen = currency.length();

    જો (U_SUCCESS(localStatus) && resLen == 3 && uprv_isInvariantString(currency.data(), resLen)) {
        જો (resLen < buffCapacity) {
            T_CString_toUpperCase(currency.data());
            u_charsToUChars(currency.data(), buff, resLen);
        }
        પાછા u_terminateUChars(buff, buffCapacity, resLen, ec);
    }

    // get country or country_variant in `id'
    CharString id = idForLocale(locale, ec);
    જો (U_FAILURE(*ec)) {
        પાછા 0;
    }

#જો !UCONFIG_NO_SERVICE
    અચલ char16_t* result = CReg::get(id.data());
    જો (result) {
        જો(buffCapacity > u_strlen(result)) {
            u_strcpy(buff, result);
        }
        resLen = u_strlen(result);
        પાછા u_terminateUChars(buff, buffCapacity, resLen, ec);
    }
#endif
    // Remove variants, which is only needed for registration.
    અક્ષર *idDelim = uprv_strchr(id.data(), VAR_DELIM);
    જો (idDelim) {
        id.truncate(idDelim - id.data());
    }

    અચલ char16_t* s = nullptr;  // Currency code from data file.
    જો (id.isEmpty()) {
        // No point looking in the data for an empty string.
        // This is what we would get.
        localStatus = U_MISSING_RESOURCE_ERROR;
    } નહીંતર {
        // Look up the CurrencyMap element in the root bundle.
        localStatus = U_ZERO_ERROR;
        UResourceBundle *rb = ures_openDirect(U_ICUDATA_CURR, CURRENCY_DATA, &localStatus);
        UResourceBundle *cm = ures_get_By_ચાવી(rb, CURRENCY_MAP, rb, &localStatus);
        LocalUResourceBundlePointer countryArray(ures_get_By_ચાવી(rb, id.data(), cm, &localStatus));
        // https://unicode-org.atlassian.net/browse/ICU-21997
        // Prefer to use currencies that are legal tender.
        જો (U_SUCCESS(localStatus)) {
            int32_t arrayLength = ures_getSize(countryArray.getAlias());
            માટે (int32_t i = 0; i < arrayLength; ++i) {
                LocalUResourceBundlePointer currencyReq(
                    ures_getByIndex(countryArray.getAlias(), i, nullptr, &localStatus));
                // The currency is legal tender if it is *not* marked with tender{"false"}.
                UErrorCode tenderStatus = localStatus;
                અચલ char16_t *tender =
                    ures_get_String_By_ચાવી(currencyReq.getAlias(), "tender", nullptr, &tenderStatus);
                તાર્કિક isTender = U_FAILURE(tenderStatus) || u_strcmp(tender, u"false") != 0;
                જો (!isTender && s != nullptr) {
                    // We already have a non-tender currency. Ignore all following non-tender ones.
                    ચાલુ;
                }
                // Fetch the currency code.
                s = ures_get_String_By_ચાવી(currencyReq.getAlias(), "id", &resLen, &localStatus);
                જો (isTender) {
                    તોડો;
                }
            }
            જો (U_SUCCESS(localStatus) && s == nullptr) {
                localStatus = U_MISSING_RESOURCE_ERROR;
            }
        }
    }

    જો ((U_FAILURE(localStatus)) && strchr(id.data(), '_') != nullptr) {
        // We don't know about it.  Check to see if we support the variant.
        CharString parent = ulocimp_getParent(locale, *ec);
        *ec = U_USING_FALLBACK_WARNING;
        // TODO: Loop over the parent rather than recursing and
        // looking again for a currency keyword.
        પાછા ucurr_forLocale(parent.data(), buff, buffCapacity, ec);
    }
    જો (*ec == U_ZERO_ERROR || localStatus != U_ZERO_ERROR) {
        // There is nothing to fallback to. Report the failure/warning if possible.
        *ec = localStatus;
    }
    જો (U_SUCCESS(*ec)) {
        જો(buffCapacity > resLen) {
            u_strcpy(buff, s);
        }
    }
    પાછા u_terminateUChars(buff, buffCapacity, resLen, ec);
}

// end registration

/**
 * Modify the given locale name by removing the rightmost _-delimited
 * element.  If there is none, empty the string ("" == root).
 * NOTE: The string "root" is not recognized; do not use it.
 * @return true if the fallback happened; false if locale is already
 * root ("").
 */
સ્થિર UBool fallback(CharString& loc) {
    જો (loc.isEmpty()) {
        પાછા ખોટું;
    }
    UErrorCode status = U_ZERO_ERROR;
    જો (loc == "en_GB") {
        // HACK: See #13368.  We need "en_GB" to fall back to "en_001" instead of "en"
        // in order to consume the correct data strings.  This hack will be removed
        // when proper data sink loading is implemented here.
        loc.truncate(3);
        loc.append("001", status);
    } નહીંતર {
        loc = ulocimp_getParent(loc.data(), status);
    }
 /*
    char *i = uprv_strrchr(loc, '_');
    if (i == nullptr) {
        i = loc;
    }
    *i = 0;
 */
    પાછા સાચું;
}


U_CAPI અચલ char16_t* U_EXPORT2
ucurr_getName(અચલ char16_t* currency U_LIFETIME_BOUND,
              અચલ અક્ષર* locale,
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

    જો (U_FAILURE(*ec)) {
        પાછા nullptr;
    }

    int32_t choice = (int32_t) nameStyle;
    જો (choice < 0 || choice > 4) {
        *ec = U_ILLEGAL_ARGUMENT_ERROR;
        પાછા nullptr;
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

    જો (locale == nullptr) {
        locale = uloc_getDefault();
    }
    CharString loc = ulocimp_getName(locale, ec2);
    જો (U_FAILURE(ec2)) {
        *ec = U_ILLEGAL_ARGUMENT_ERROR;
        પાછા nullptr;
    }

    અક્ષર buf[ISO_CURRENCY_CODE_LENGTH+1];
    myUCharsToChars(buf, currency);
    
    /* Normalize the keyword value to uppercase */
    T_CString_toUpperCase(buf);
    
    અચલ char16_t* s = nullptr;
    ec2 = U_ZERO_ERROR;
    LocalUResourceBundlePointer rb(ures_open(U_ICUDATA_CURR, loc.data(), &ec2));

    જો (nameStyle == UCURR_NARROW_પ્રતીક_NAME || nameStyle == UCURR_FORMAL_પ્રતીક_NAME || nameStyle == UCURR_VARIANT_પ્રતીક_NAME) {
        CharString ચાવી;
        પસંદગી (nameStyle) {
        સ્થિતિ UCURR_NARROW_પ્રતીક_NAME:
            ચાવી.append(CURRENCIES_NARROW, ec2);
            તોડો;
        સ્થિતિ UCURR_FORMAL_પ્રતીક_NAME:
            ચાવી.append(CURRENCIES_FORMAL, ec2);
            તોડો;
        સ્થિતિ UCURR_VARIANT_પ્રતીક_NAME:
            ચાવી.append(CURRENCIES_VARIANT, ec2);
            તોડો;
        મૂળભૂત:
            *ec = U_UNSUPPORTED_ERROR;
            પાછા nullptr;
        }
        ચાવી.append("/", ec2);
        ચાવી.append(buf, ec2);
        s = ures_get_String_By_ચાવી_With_Fallback(rb.getAlias(), ચાવી.data(), len, &ec2);
        જો (ec2 == U_MISSING_RESOURCE_ERROR) {
            *ec = U_USING_FALLBACK_WARNING;
            ec2 = U_ZERO_ERROR;
            choice = UCURR_પ્રતીક_NAME;
        }
    }
    જો (s == nullptr) {
        ures_get_By_ચાવી(rb.getAlias(), CURRENCIES, rb.getAlias(), &ec2);
        ures_get_By_ચાવી_With_Fallback(rb.getAlias(), buf, rb.getAlias(), &ec2);
        s = ures_getStringByIndex(rb.getAlias(), choice, len, &ec2);
    }

    // If we've succeeded we're done.  Otherwise, try to fallback.
    // If that fails (because we are already at root) then exit.
    જો (U_SUCCESS(ec2)) {
        જો (ec2 == U_USING_DEFAULT_WARNING
            || (ec2 == U_USING_FALLBACK_WARNING && *ec != U_USING_DEFAULT_WARNING)) {
            *ec = ec2;
        }
    }

    // We no longer support choice format data in names.  Data should not contain
    // choice patterns.
    જો (isChoiceFormat != nullptr) {
        *isChoiceFormat = ખોટું;
    }
    જો (U_SUCCESS(ec2)) {
        U_ASSERT(s != nullptr);
        પાછા s;
    }

    // If we fail to find a match, use the ISO 4217 code
    *len = u_strlen(currency); // Should == ISO_CURRENCY_CODE_LENGTH, but maybe not...?
    *ec = U_USING_DEFAULT_WARNING;
    પાછા currency;
}

U_CAPI અચલ char16_t* U_EXPORT2
ucurr_getPluralName(અચલ char16_t* currency U_LIFETIME_BOUND,
                    અચલ અક્ષર* locale,
                    UBool* isChoiceFormat,
                    અચલ અક્ષર* plural_ગણતરી,
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

    જો (U_FAILURE(*ec)) {
        પાછા nullptr;
    }

    // Use a separate UErrorCode here that does not propagate out of
    // this function.
    UErrorCode ec2 = U_ZERO_ERROR;

    જો (locale == nullptr) {
        locale = uloc_getDefault();
    }
    CharString loc = ulocimp_getName(locale, ec2);
    જો (U_FAILURE(ec2)) {
        *ec = U_ILLEGAL_ARGUMENT_ERROR;
        પાછા nullptr;
    }

    અક્ષર buf[ISO_CURRENCY_CODE_LENGTH+1];
    myUCharsToChars(buf, currency);

    અચલ char16_t* s = nullptr;
    ec2 = U_ZERO_ERROR;
    UResourceBundle* rb = ures_open(U_ICUDATA_CURR, loc.data(), &ec2);

    rb = ures_get_By_ચાવી(rb, CURRENCYPLURALS, rb, &ec2);

    // Fetch resource with multi-level resource inheritance fallback
    LocalUResourceBundlePointer curr(ures_get_By_ચાવી_With_Fallback(rb, buf, rb, &ec2));

    s = ures_get_String_By_ચાવી_With_Fallback(curr.getAlias(), plural_ગણતરી, len, &ec2);
    જો (U_FAILURE(ec2)) {
        //  fall back to "other"
        ec2 = U_ZERO_ERROR;
        s = ures_get_String_By_ચાવી_With_Fallback(curr.getAlias(), "other", len, &ec2);     
        જો (U_FAILURE(ec2)) {
            // fall back to long name in Currencies
            પાછા ucurr_getName(currency, locale, UCURR_LONG_NAME, 
                                 isChoiceFormat, len, ec);
        }
    }

    // If we've succeeded we're done.  Otherwise, try to fallback.
    // If that fails (because we are already at root) then exit.
    જો (U_SUCCESS(ec2)) {
        જો (ec2 == U_USING_DEFAULT_WARNING
            || (ec2 == U_USING_FALLBACK_WARNING && *ec != U_USING_DEFAULT_WARNING)) {
            *ec = ec2;
        }
        U_ASSERT(s != nullptr);
        પાછા s;
    }

    // If we fail to find a match, use the ISO 4217 code
    *len = u_strlen(currency); // Should == ISO_CURRENCY_CODE_LENGTH, but maybe not...?
    *ec = U_USING_DEFAULT_WARNING;
    પાછા currency;
}


//========================================================================
// Following are structure and function for parsing currency names

#define NEED_TO_BE_DELETED 0x1

// TODO: a better way to define this?
#define MAX_CURRENCY_NAME_LEN 100

પ્રકારવ્યાખ્યા સંરચના {
    અચલ અક્ષર* IsoCode;  //ચાવી
    char16_t* currencyName;  //કિંમત
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
સ્થિર પૂર્ણાંક U_CALLCONV currencyNameComparator(અચલ શૂન્ય* a, અચલ શૂન્ય* b) {
    અચલ CurrencyNameStruct* currName_1 = static_cast<અચલ CurrencyNameStruct*>(a);
    અચલ CurrencyNameStruct* currName_2 = static_cast<અચલ CurrencyNameStruct*>(b);
    માટે (int32_t i = 0; 
         i < MIN(currName_1->currencyNameLen, currName_2->currencyNameLen);
         ++i) {
        જો (currName_1->currencyName[i] < currName_2->currencyName[i]) {
            પાછા -1;
        }
        જો (currName_1->currencyName[i] > currName_2->currencyName[i]) {
            પાછા 1;
        }
    }
    જો (currName_1->currencyNameLen < currName_2->currencyNameLen) {
        પાછા -1;
    } નહીંતર જો (currName_1->currencyNameLen > currName_2->currencyNameLen) {
        પાછા 1;
    }
    પાછા 0;
}


// Give a locale, return the maximum number of currency names associated with
// this locale.
// It gets currency names from resource bundles using fallback.
// It is the maximum number because in the fallback chain, some of the 
// currency names are duplicated.
// For example, given locale as "en_US", the currency names get from resource
// bundle in "en_US" and "en" are duplicated. The fallback mechanism will count
// all currency names in "en_US" and "en".
સ્થિર શૂન્ય
get_Currency_Name_ગણતરી(અચલ અક્ષર* loc, int32_t* total_currency_name_ગણતરી, int32_t* total_currency_પ્રતીક_ગણતરી) {
    U_NAMESPACE_USE
    *total_currency_name_ગણતરી = 0;
    *total_currency_પ્રતીક_ગણતરી = 0;
    અચલ char16_t* s = nullptr;
    CharString locale;
    {
        UErrorCode status = U_ZERO_ERROR;
        locale.append(loc, status);
        જો (U_FAILURE(status)) { પાછા; }
    }
    અચલ icu::Hashtable *currencySymbolsEquiv = getCurrSymbolsEquiv();
    માટે (;;) {
        UErrorCode ec2 = U_ZERO_ERROR;
        // TODO: ures_openDirect?
        LocalUResourceBundlePointer rb(ures_open(U_ICUDATA_CURR, locale.data(), &ec2));
        LocalUResourceBundlePointer curr(ures_get_By_ચાવી(rb.getAlias(), CURRENCIES, nullptr, &ec2));
        int32_t n = ures_getSize(curr.getAlias());
        માટે (int32_t i=0; i<n; ++i) {
            LocalUResourceBundlePointer names(ures_getByIndex(curr.getAlias(), i, nullptr, &ec2));
            int32_t len;
            s = ures_getStringByIndex(names.getAlias(), UCURR_પ્રતીક_NAME, &len, &ec2);
            ++(*total_currency_પ્રતીક_ગણતરી);  // currency symbol
            જો (currencySymbolsEquiv != nullptr) {
                *total_currency_પ્રતીક_ગણતરી += ગણતરી_Equivalent(*currencySymbolsEquiv, UnicodeString(સાચું, s, len));
            }
            ++(*total_currency_પ્રતીક_ગણતરી); // iso code
            ++(*total_currency_name_ગણતરી); // long name
        }

        // currency plurals
        UErrorCode ec3 = U_ZERO_ERROR;
        LocalUResourceBundlePointer curr_p(ures_get_By_ચાવી(rb.getAlias(), CURRENCYPLURALS, nullptr, &ec3));
        n = ures_getSize(curr_p.getAlias());
        માટે (int32_t i=0; i<n; ++i) {
            LocalUResourceBundlePointer names(ures_getByIndex(curr_p.getAlias(), i, nullptr, &ec3));
            *total_currency_name_ગણતરી += ures_getSize(names.getAlias());
        }

        જો (!fallback(locale)) {
            તોડો;
        }
    }
}

સ્થિર char16_t*
toUpperCase(અચલ char16_t* source, int32_t len, અચલ અક્ષર* locale) {
    char16_t* dest = nullptr;
    UErrorCode ec = U_ZERO_ERROR;
    int32_t destLen = u_strToUpper(dest, 0, source, len, locale, &ec);

    ec = U_ZERO_ERROR;
    dest = static_cast<char16_t*>(uprv_malloc(કદ(char16_t) * MAX(destLen, len)));
    જો (dest == nullptr) {
        પાછા nullptr;
    }
    u_strToUpper(dest, destLen, source, len, locale, &ec);
    જો (U_FAILURE(ec)) {
        u_memcpy(dest, source, len);
    } 
    પાછા dest;
}


સ્થિર શૂન્ય deleteCurrencyNames(CurrencyNameStruct* currencyNames, int32_t count);
// Collect all available currency names associated with the given locale
// (enable fallback chain).
// Read currenc names defined in resource bundle "Currencies" and
// "CurrencyPlural", enable fallback chain.
// return the malloc-ed currency name arrays and the total number of currency
// names in the array.
સ્થિર શૂન્ય
collectCurrencyNames(અચલ અક્ષર* locale, 
                     CurrencyNameStruct** currencyNames, 
                     int32_t* total_currency_name_ગણતરી, 
                     CurrencyNameStruct** currencySymbols, 
                     int32_t* total_currency_પ્રતીક_ગણતરી, 
                     UErrorCode& ec) {
    જો (U_FAILURE(ec)) {
        *currencyNames = *currencySymbols = nullptr;
        *total_currency_name_ગણતરી = *total_currency_પ્રતીક_ગણતરી = 0;
        પાછા;
    }
    U_NAMESPACE_USE
    અચલ icu::Hashtable *currencySymbolsEquiv = getCurrSymbolsEquiv();
    // Look up the Currencies resource for the given locale.
    UErrorCode ec2 = U_ZERO_ERROR;

    જો (locale == nullptr) {
        locale = uloc_getDefault();
    }
    CharString loc = ulocimp_getName(locale, ec2);
    જો (U_FAILURE(ec2)) {
        ec = U_ILLEGAL_ARGUMENT_ERROR;
        *currencyNames = *currencySymbols = nullptr;
        *total_currency_name_ગણતરી = *total_currency_પ્રતીક_ગણતરી = 0;
        પાછા;
    }

    // Get maximum currency name count first.
    get_Currency_Name_ગણતરી(loc.data(), total_currency_name_ગણતરી, total_currency_પ્રતીક_ગણતરી);

    *currencyNames = static_cast<CurrencyNameStruct*>(
        uprv_malloc(કદ(CurrencyNameStruct) * (*total_currency_name_ગણતરી)));
    જો(*currencyNames == nullptr) {
        *currencySymbols = nullptr;
        *total_currency_name_ગણતરી = *total_currency_પ્રતીક_ગણતરી = 0;
        ec = U_MEMORY_ALLOCATION_ERROR;
        પાછા;
    }
    *currencySymbols = static_cast<CurrencyNameStruct*>(
        uprv_malloc(કદ(CurrencyNameStruct) * (*total_currency_પ્રતીક_ગણતરી)));

    જો(*currencySymbols == nullptr) {
        uprv_free(*currencyNames);
        *currencyNames = nullptr;
        *total_currency_name_ગણતરી = *total_currency_પ્રતીક_ગણતરી = 0;
        ec = U_MEMORY_ALLOCATION_ERROR;
        પાછા;
    }

    અચલ char16_t* s = nullptr;  // currency name
    અક્ષર* iso = nullptr;  // currency ISO code

    *total_currency_name_ગણતરી = 0;
    *total_currency_પ્રતીક_ગણતરી = 0;

    UErrorCode ec3 = U_ZERO_ERROR;
    UErrorCode ec4 = U_ZERO_ERROR;

    // Using hash to remove duplicates caused by locale fallback
    LocalUHashtablePointer currencyIsoCodes(uhash_open(uhash_hashChars, uhash_compareChars, nullptr, &ec3));
    LocalUHashtablePointer currencyPluralIsoCodes(uhash_open(uhash_hashChars, uhash_compareChars, nullptr, &ec4));
    માટે (int32_t localeLevel = 0; ; ++localeLevel) {
        ec2 = U_ZERO_ERROR;
        // TODO: ures_openDirect
        LocalUResourceBundlePointer rb(ures_open(U_ICUDATA_CURR, loc.data(), &ec2));
        LocalUResourceBundlePointer curr(ures_get_By_ચાવી(rb.getAlias(), CURRENCIES, nullptr, &ec2));
        int32_t n = ures_getSize(curr.getAlias());
        માટે (int32_t i=0; i<n; ++i) {
            LocalUResourceBundlePointer names(ures_getByIndex(curr.getAlias(), i, nullptr, &ec2));
            int32_t len;
            s = ures_getStringByIndex(names.getAlias(), UCURR_પ્રતીક_NAME, &len, &ec2);
            // TODO: uhash_put wont change key/value?
            iso = const_cast<અક્ષર*>(ures_get_ચાવી(names.getAlias()));
            જો (localeLevel != 0 && uhash_get(currencyIsoCodes.getAlias(), iso) != nullptr) {
                ચાલુ;
            }
            uhash_put(currencyIsoCodes.getAlias(), iso, iso, &ec3); 
            // Add currency symbol.
            (*currencySymbols)[(*total_currency_પ્રતીક_ગણતરી)++] = {iso, const_cast<char16_t*>(s), len, 0};

            // Add equivalent symbols
            જો (currencySymbolsEquiv != nullptr) {
                UnicodeString str(સાચું, s, len);
                icu::EquivIterator iter(*currencySymbolsEquiv, str);
                અચલ UnicodeString *પ્રતીક;
                જ્યારે ((પ્રતીક = iter.next()) != nullptr) {
                    (*currencySymbols)[(*total_currency_પ્રતીક_ગણતરી)++]
                        = {iso, const_cast<char16_t*>(પ્રતીક->getBuffer()), પ્રતીક->length(), 0};
                }
            }

            // Add currency long name.
            s = ures_getStringByIndex(names.getAlias(), UCURR_LONG_NAME, &len, &ec2);
            char16_t* upperName = toUpperCase(s, len, locale);
            જો (upperName == nullptr) {
                ec = U_MEMORY_ALLOCATION_ERROR;
                જાઓ error;
            }
            (*currencyNames)[(*total_currency_name_ગણતરી)++] = {iso, upperName, len, NEED_TO_BE_DELETED};

            // put (iso, 3, and iso) in to array
            // Add currency ISO code.
            char16_t* isoCode = static_cast<char16_t*>(uprv_malloc(કદ(char16_t) * 3));
            જો (isoCode == nullptr) {
                ec = U_MEMORY_ALLOCATION_ERROR;
                જાઓ error;
            }
            // Must convert iso[] into Unicode
            u_charsToUChars(iso, isoCode, 3);
            (*currencySymbols)[(*total_currency_પ્રતીક_ગણતરી)++] = {iso, isoCode, 3, NEED_TO_BE_DELETED};
        }

        // currency plurals
        UErrorCode ec5 = U_ZERO_ERROR;
        LocalUResourceBundlePointer curr_p(ures_get_By_ચાવી(rb.getAlias(), CURRENCYPLURALS, nullptr, &ec5));
        n = ures_getSize(curr_p.getAlias());
        માટે (int32_t i=0; i<n; ++i) {
            LocalUResourceBundlePointer names(ures_getByIndex(curr_p.getAlias(), i, nullptr, &ec5));
            iso = const_cast<અક્ષર*>(ures_get_ચાવી(names.getAlias()));
            // Using hash to remove duplicated ISO codes in fallback chain.
            જો (localeLevel != 0 && uhash_get(currencyPluralIsoCodes.getAlias(), iso) != nullptr) {
                ચાલુ;
            }
            uhash_put(currencyPluralIsoCodes.getAlias(), iso, iso, &ec4);
            int32_t num = ures_getSize(names.getAlias());
            int32_t len;
            માટે (int32_t j = 0; j < num; ++j) {
                // TODO: remove duplicates between singular name and 
                // currency long name?
                s = ures_getStringByIndex(names.getAlias(), j, &len, &ec5);
                char16_t* upperName = toUpperCase(s, len, locale);
                જો (upperName == nullptr) {
                    ec = U_MEMORY_ALLOCATION_ERROR;
                    જાઓ error;
                }
                (*currencyNames)[(*total_currency_name_ગણતરી)++] = {iso, upperName, len, NEED_TO_BE_DELETED};
            }
        }

        જો (!fallback(loc)) {
            તોડો;
        }
    }

    // quick sort the struct
    qsort(*currencyNames, *total_currency_name_ગણતરી, 
          કદ(CurrencyNameStruct), currencyNameComparator);
    qsort(*currencySymbols, *total_currency_પ્રતીક_ગણતરી, 
          કદ(CurrencyNameStruct), currencyNameComparator);

#ifdef UCURR_DEBUG
    printf("currency name count: %d\n", *total_currency_name_ગણતરી);
    માટે (int32_t index = 0; index < *total_currency_name_ગણતરી; ++index) {
        printf("index: %d\n", index);
        printf("iso: %s\n", (*currencyNames)[index].IsoCode);
        અક્ષર curNameBuf[1024];
        memset(curNameBuf, 0, 1024);
        u_austrncpy(curNameBuf, (*currencyNames)[index].currencyName, (*currencyNames)[index].currencyNameLen);
        printf("currencyName: %s\n", curNameBuf);
        printf("len: %d\n", (*currencyNames)[index].currencyNameLen);
    }
    printf("currency symbol count: %d\n", *total_currency_પ્રતીક_ગણતરી);
    માટે (int32_t index = 0; index < *total_currency_પ્રતીક_ગણતરી; ++index) {
        printf("index: %d\n", index);
        printf("iso: %s\n", (*currencySymbols)[index].IsoCode);
        અક્ષર curNameBuf[1024];
        memset(curNameBuf, 0, 1024);
        u_austrncpy(curNameBuf, (*currencySymbols)[index].currencyName, (*currencySymbols)[index].currencyNameLen);
        printf("currencySymbol: %s\n", curNameBuf);
        printf("len: %d\n", (*currencySymbols)[index].currencyNameLen);
    }
#endif
    // fail on hashtable errors
    જો (U_FAILURE(ec3)) {
      ec = ec3;
    } નહીંતર જો (U_FAILURE(ec4)) {
      ec = ec4;
    }

  error:
    // clean up if we got error
    જો (U_FAILURE(ec)) {
        deleteCurrencyNames(*currencyNames, *total_currency_name_ગણતરી);
        deleteCurrencyNames(*currencySymbols, *total_currency_પ્રતીક_ગણતરી);
        *currencyNames = *currencySymbols = nullptr;
        *total_currency_name_ગણતરી = *total_currency_પ્રતીક_ગણતરી = 0;
    }
}

// @param  currencyNames: currency names array
// @param  indexInCurrencyNames: the index of the character in currency names 
//         array against which the comparison is done
// @param  key: input text char to compare against
// @param  begin(IN/OUT): the begin index of matching range in currency names array
// @param  end(IN/OUT): the end index of matching range in currency names array.
સ્થિર int32_t
દ્વિઅંકી_શોધ(અચલ CurrencyNameStruct* currencyNames, 
             int32_t indexInCurrencyNames,
             અચલ char16_t ચાવી,
             int32_t* begin, int32_t* end) {
#ifdef UCURR_DEBUG
    printf("key = %x\n", ચાવી);
#endif
   int32_t first = *begin;
   int32_t last = *end;
   જ્યારે (first <= last) {
       int32_t mid = (first + last) / 2;  // compute mid point.
       જો (indexInCurrencyNames >= currencyNames[mid].currencyNameLen) {
           first = mid + 1;
       } નહીંતર {
           જો (ચાવી > currencyNames[mid].currencyName[indexInCurrencyNames]) {
               first = mid + 1;
           }
           નહીંતર જો (ચાવી < currencyNames[mid].currencyName[indexInCurrencyNames]) {
               last = mid - 1;
           }
           નહીંતર {
                // Find a match, and looking for ranges
                // Now do two more binary searches. First, on the left side for
                // the greatest L such that CurrencyNameStruct[L] < key.
                int32_t L = *begin;
                int32_t R = mid;

#ifdef UCURR_DEBUG
                printf("mid = %d\n", mid);
#endif
                જ્યારે (L < R) {
                    int32_t M = (L + R) / 2;
#ifdef UCURR_DEBUG
                    printf("L = %d, R = %d, M = %d\n", L, R, M);
#endif
                    જો (indexInCurrencyNames >= currencyNames[M].currencyNameLen) {
                        L = M + 1;
                    } નહીંતર {
                        જો (currencyNames[M].currencyName[indexInCurrencyNames] < ચાવી) {
                            L = M + 1;
                        } નહીંતર {
#ifdef UCURR_DEBUG
                            U_ASSERT(currencyNames[M].currencyName[indexInCurrencyNames] == ચાવી);
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
                U_ASSERT(currencyNames[*begin].currencyName[indexInCurrencyNames] == ચાવી);
#endif

                // Now for the second search, finding the least R such that
                // key < CurrencyNameStruct[R].
                L = mid;
                R = *end;
                જ્યારે (L < R) {
                    int32_t M = (L + R) / 2;
#ifdef UCURR_DEBUG
                    printf("L = %d, R = %d, M = %d\n", L, R, M);
#endif
                    જો (currencyNames[M].currencyNameLen < indexInCurrencyNames) {
                        L = M + 1;
                    } નહીંતર {
                        જો (currencyNames[M].currencyName[indexInCurrencyNames] > ચાવી) {
                            R = M;
                        } નહીંતર {
#ifdef UCURR_DEBUG
                            U_ASSERT(currencyNames[M].currencyName[indexInCurrencyNames] == ચાવી);
#endif
                            L = M + 1;
                        }
                    }
                }
#ifdef UCURR_DEBUG
                U_ASSERT(L == R);
#endif
                જો (currencyNames[R].currencyName[indexInCurrencyNames] > ચાવી) {
                    *end = R - 1;
                } નહીંતર {
                    *end = R;
                }
#ifdef UCURR_DEBUG
                printf("end = %d\n", *end);
#endif

                // now, found the range. check whether there is exact match
                જો (currencyNames[*begin].currencyNameLen == indexInCurrencyNames + 1) {
                    પાછા *begin;  // find range and exact match.
                }
                પાછા -1;  // find range, but no exact match.
           }
       }
   }
   *begin = -1;
   *end = -1;
   પાછા -1;    // failed to find range.
}


// Linear search "text" in "currencyNames".
// @param  begin, end: the begin and end index in currencyNames, within which
//         range should the search be performed.
// @param  textLen: the length of the text to be compared
// @param  maxMatchLen(IN/OUT): passing in the computed max matching length
//                              pass out the new max  matching length
// @param  maxMatchIndex: the index in currencyName which has the longest
//                        match with input text.
સ્થિર શૂન્ય
linear_શોધ(અચલ CurrencyNameStruct* currencyNames, 
             int32_t begin, int32_t end,
             અચલ char16_t* text, int32_t textLen,
             int32_t *partialMatchLen,
             int32_t *maxMatchLen, int32_t* maxMatchIndex) {
    int32_t initialPartialMatchLen = *partialMatchLen;
    માટે (int32_t index = begin; index <= end; ++index) {
        int32_t len = currencyNames[index].currencyNameLen;
        જો (len > *maxMatchLen && len <= textLen &&
            uprv_memcmp(currencyNames[index].currencyName, text, len * કદ(char16_t)) == 0) {
            *partialMatchLen = MAX(*partialMatchLen, len);
            *maxMatchIndex = index;
            *maxMatchLen = len;
#ifdef UCURR_DEBUG
            printf("maxMatchIndex = %d, maxMatchLen = %d\n",
                   *maxMatchIndex, *maxMatchLen);
#endif
        } નહીંતર {
            // Check for partial matches.
            માટે (int32_t i=initialPartialMatchLen; i<MIN(len, textLen); i++) {
                જો (currencyNames[index].currencyName[i] != text[i]) {
                    તોડો;
                }
                *partialMatchLen = MAX(*partialMatchLen, i + 1);
            }
        }
    }
}

#define LINEAR_શોધ_THRESHOLD 10

// Find longest match between "text" and currency names in "currencyNames".
// @param  total_currency_count: total number of currency names in CurrencyNames.
// @param  textLen: the length of the text to be compared
// @param  maxMatchLen: passing in the computed max matching length
//                              pass out the new max  matching length
// @param  maxMatchIndex: the index in currencyName which has the longest
//                        match with input text.
સ્થિર શૂન્ય
શોધ_Currency_Name(અચલ CurrencyNameStruct* currencyNames, 
                   int32_t total_currency_ગણતરી,
                   અચલ char16_t* text, int32_t textLen,
                   int32_t *partialMatchLen,
                   int32_t* maxMatchLen, int32_t* maxMatchIndex) {
    *maxMatchIndex = -1;
    *maxMatchLen = 0;
    int32_t matchIndex = -1;
    int32_t દ્વિઅંકી_શોધ_Begin = 0;
    int32_t દ્વિઅંકી_શોધ_End = total_currency_ગણતરી - 1;
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
    માટે (int32_t index = 0; index < textLen; ++index) {
        // matchIndex saves the one with exact match till the current point.
        // [binarySearchBegin, binarySearchEnd] saves the matching range.
        matchIndex = દ્વિઅંકી_શોધ(currencyNames, index,
                                  text[index],
                                  &દ્વિઅંકી_શોધ_Begin, &દ્વિઅંકી_શોધ_End);
        જો (દ્વિઅંકી_શોધ_Begin == -1) { // did not find the range
            તોડો;
        }
        *partialMatchLen = MAX(*partialMatchLen, index + 1);
        જો (matchIndex != -1) { 
            // find an exact match for text from text[0] to text[index] 
            // in currencyNames array.
            *maxMatchLen = index + 1;
            *maxMatchIndex = matchIndex;
        }
        જો (દ્વિઅંકી_શોધ_End - દ્વિઅંકી_શોધ_Begin < LINEAR_શોધ_THRESHOLD) {
            // linear search if within threshold.
            linear_શોધ(currencyNames, દ્વિઅંકી_શોધ_Begin, દ્વિઅંકી_શોધ_End,
                         text, textLen,
                         partialMatchLen,
                         maxMatchLen, maxMatchIndex);
            તોડો;
        }
    }
}

//========================= currency name cache =====================
પ્રકારવ્યાખ્યા સંરચના {
    અક્ષર locale[ULOC_FULLNAME_CAPACITY];  //key
    // currency names, case insensitive
    CurrencyNameStruct* currencyNames;  //કિંમત
    int32_t total_Currency_Name_ગણતરી;  // currency name count
    // currency symbols and ISO code, case sensitive
    CurrencyNameStruct* currencySymbols; //કિંમત
    int32_t total_Currency_પ્રતીક_ગણતરી;  //ગણતરી
    // reference count.
    // reference count is set to 1 when an entry is put to cache.
    // it increases by 1 before accessing, and decreased by 1 after accessing.
    // The entry is deleted when ref count is zero, which means 
    // the entry is replaced out of cache and no process is accessing it.
    int32_t ref_ગણતરી;
} CurrencyNameCacheEntry;


#define CURRENCY_NAME_CACHE_અંક 10

// Reserve 10 cache entries.
સ્થિર CurrencyNameCacheEntry* currCache[CURRENCY_NAME_CACHE_અંક] = {nullptr};
// Using an index to indicate which entry to be replaced when cache is full.
// It is a simple round-robin replacement strategy.
સ્થિર int8_t currentCacheEntryIndex = 0;

સ્થિર UMutex gCurrencyCacheMutex;

// Cache deletion
સ્થિર શૂન્ય
deleteCurrencyNames(CurrencyNameStruct* currencyNames, int32_t count) {
    માટે (int32_t index = 0; index < count; ++index) {
        જો ( (currencyNames[index].flag & NEED_TO_BE_DELETED) ) {
            uprv_free(currencyNames[index].currencyName);
        }
    }
    uprv_free(currencyNames);
}


સ્થિર શૂન્ય
deleteCacheEntry(CurrencyNameCacheEntry* entry) {
    deleteCurrencyNames(entry->currencyNames, entry->total_Currency_Name_ગણતરી);
    deleteCurrencyNames(entry->currencySymbols, entry->total_Currency_પ્રતીક_ગણતરી);
    uprv_free(entry);
}


// Cache clean up
સ્થિર UBool U_CALLCONV
currency_cache_cleanup() {
    માટે (int32_t i = 0; i < CURRENCY_NAME_CACHE_અંક; ++i) {
        જો (currCache[i]) {
            deleteCacheEntry(currCache[i]);
            currCache[i] = nullptr;
        }
    }
    પાછા સાચું;
}


/**
 * Loads the currency name data from the cache, or from resource bundles if necessary.
 * The refCount is automatically incremented.  It is the caller's responsibility
 * to decrement it when done!
 */
સ્થિર CurrencyNameCacheEntry*
getCacheEntry(અચલ અક્ષર* locale, UErrorCode& ec) {

    int32_t total_currency_name_ગણતરી = 0;
    CurrencyNameStruct* currencyNames = nullptr;
    int32_t total_currency_પ્રતીક_ગણતરી = 0;
    CurrencyNameStruct* currencySymbols = nullptr;
    CurrencyNameCacheEntry* cacheEntry = nullptr;

    umtx_lock(&gCurrencyCacheMutex);
    // in order to handle racing correctly,
    // not putting 'search' in a separate function.
    int8_t found = -1;
    માટે (int8_t i = 0; i < CURRENCY_NAME_CACHE_અંક; ++i) {
        જો (currCache[i]!= nullptr &&
            uprv_strcmp(locale, currCache[i]->locale) == 0) {
            found = i;
            તોડો;
        }
    }
    જો (found != -1) {
        cacheEntry = currCache[found];
        ++(cacheEntry->ref_ગણતરી);
    }
    umtx_unlock(&gCurrencyCacheMutex);
    જો (found == -1) {
        collectCurrencyNames(locale, &currencyNames, &total_currency_name_ગણતરી, &currencySymbols, &total_currency_પ્રતીક_ગણતરી, ec);
        જો (U_FAILURE(ec)) {
            પાછા nullptr;
        }
        umtx_lock(&gCurrencyCacheMutex);
        // check again.
        માટે (int8_t i = 0; i < CURRENCY_NAME_CACHE_અંક; ++i) {
            જો (currCache[i]!= nullptr &&
                uprv_strcmp(locale, currCache[i]->locale) == 0) {
                found = i;
                તોડો;
            }
        }
        જો (found == -1) {
            // insert new entry to 
            // currentCacheEntryIndex % CURRENCY_NAME_CACHE_NUM
            // and remove the existing entry 
            // currentCacheEntryIndex % CURRENCY_NAME_CACHE_NUM
            // from cache.
            cacheEntry = currCache[currentCacheEntryIndex];
            જો (cacheEntry) {
                --(cacheEntry->ref_ગણતરી);
                // delete if the ref count is zero
                જો (cacheEntry->ref_ગણતરી == 0) {
                    deleteCacheEntry(cacheEntry);
                }
            }
            cacheEntry = static_cast<CurrencyNameCacheEntry*>(uprv_malloc(કદ(CurrencyNameCacheEntry)));
            જો (cacheEntry == nullptr) {
                deleteCurrencyNames(currencyNames, total_currency_name_ગણતરી);
                deleteCurrencyNames(currencySymbols, total_currency_પ્રતીક_ગણતરી);
                ec = U_MEMORY_ALLOCATION_ERROR;
                પાછા nullptr;
            }
            currCache[currentCacheEntryIndex] = cacheEntry;
            uprv_strcpy(cacheEntry->locale, locale);
            cacheEntry->currencyNames = currencyNames;
            cacheEntry->total_Currency_Name_ગણતરી = total_currency_name_ગણતરી;
            cacheEntry->currencySymbols = currencySymbols;
            cacheEntry->total_Currency_પ્રતીક_ગણતરી = total_currency_પ્રતીક_ગણતરી;
            cacheEntry->ref_ગણતરી = 2; // one for cache, one for reference
            currentCacheEntryIndex = (currentCacheEntryIndex + 1) % CURRENCY_NAME_CACHE_અંક;
            ucln_common_registerCleanup(UCLN_COMMON_CURRENCY, currency_cleanup);
        } નહીંતર {
            deleteCurrencyNames(currencyNames, total_currency_name_ગણતરી);
            deleteCurrencyNames(currencySymbols, total_currency_પ્રતીક_ગણતરી);
            cacheEntry = currCache[found];
            ++(cacheEntry->ref_ગણતરી);
        }
        umtx_unlock(&gCurrencyCacheMutex);
    }

    પાછા cacheEntry;
}

સ્થિર શૂન્ય releaseCacheEntry(CurrencyNameCacheEntry* cacheEntry) {
    umtx_lock(&gCurrencyCacheMutex);
    --(cacheEntry->ref_ગણતરી);
    જો (cacheEntry->ref_ગણતરી == 0) {  // remove
        deleteCacheEntry(cacheEntry);
    }
    umtx_unlock(&gCurrencyCacheMutex);
}

U_CAPI શૂન્ય
uprv_parseCurrency(અચલ અક્ષર* locale,
                   અચલ icu::UnicodeString& text,
                   icu::ParsePosition& pos,
                   int8_t type,
                   int32_t* partialMatchLen,
                   char16_t* result,
                   UErrorCode& ec) {
    U_NAMESPACE_USE
    જો (U_FAILURE(ec)) {
        પાછા;
    }
    CurrencyNameCacheEntry* cacheEntry = getCacheEntry(locale, ec);
    જો (U_FAILURE(ec)) {
        પાછા;
    }

    int32_t total_currency_name_ગણતરી = cacheEntry->total_Currency_Name_ગણતરી;
    CurrencyNameStruct* currencyNames = cacheEntry->currencyNames;
    int32_t total_currency_પ્રતીક_ગણતરી = cacheEntry->total_Currency_પ્રતીક_ગણતરી;
    CurrencyNameStruct* currencySymbols = cacheEntry->currencySymbols;

    int32_t start = pos.getIndex();

    char16_t આગત_Text[MAX_CURRENCY_NAME_LEN];
    char16_t upperText[MAX_CURRENCY_NAME_LEN];
    int32_t textLen = MIN(MAX_CURRENCY_NAME_LEN, text.length() - start);
    text.extract(start, textLen, આગત_Text);
    UErrorCode ec1 = U_ZERO_ERROR;
    textLen = u_strToUpper(upperText, MAX_CURRENCY_NAME_LEN, આગત_Text, textLen, locale, &ec1);

    // Make sure partialMatchLen is initialized
    *partialMatchLen = 0;

    int32_t max = 0;
    int32_t matchIndex = -1;
    // case in-sensitive comparison against currency names
    શોધ_Currency_Name(currencyNames, total_currency_name_ગણતરી, 
                       upperText, textLen, partialMatchLen, &max, &matchIndex);

#ifdef UCURR_DEBUG
    printf("search in names, max = %d, matchIndex = %d\n", max, matchIndex);
#endif

    int32_t max_In_પ્રતીક = 0;
    int32_t match_Index_In_પ્રતીક = -1;
    જો (type != UCURR_LONG_NAME) {  // not name only
        // case sensitive comparison against currency symbols and ISO code.
        શોધ_Currency_Name(currencySymbols, total_currency_પ્રતીક_ગણતરી, 
                           આગત_Text, textLen,
                           partialMatchLen,
                           &max_In_પ્રતીક, &match_Index_In_પ્રતીક);
    }

#ifdef UCURR_DEBUG
    printf("search in symbols, maxInSymbol = %d, matchIndexInSymbol = %d\n", max_In_પ્રતીક, match_Index_In_પ્રતીક);
    જો(match_Index_In_પ્રતીક != -1) {
      printf("== ISO=%s\n", currencySymbols[match_Index_In_પ્રતીક].IsoCode);
    }
#endif

    જો (max >= max_In_પ્રતીક && matchIndex != -1) {
        u_charsToUChars(currencyNames[matchIndex].IsoCode, result, 4);
        pos.setIndex(start + max);
    } નહીંતર જો (max_In_પ્રતીક >= max && match_Index_In_પ્રતીક != -1) {
        u_charsToUChars(currencySymbols[match_Index_In_પ્રતીક].IsoCode, result, 4);
        pos.setIndex(start + max_In_પ્રતીક);
    }

    // decrease reference count
    releaseCacheEntry(cacheEntry);
}

શૂન્ય uprv_currencyLeads(અચલ અક્ષર* locale, icu::UnicodeSet& result, UErrorCode& ec) {
    U_NAMESPACE_USE
    જો (U_FAILURE(ec)) {
        પાછા;
    }
    CurrencyNameCacheEntry* cacheEntry = getCacheEntry(locale, ec);
    જો (U_FAILURE(ec)) {
        પાછા;
    }

    માટે (int32_t i=0; i<cacheEntry->total_Currency_પ્રતીક_ગણતરી; i++) {
        અચલ CurrencyNameStruct& info = cacheEntry->currencySymbols[i];
        UChar32 cp;
        U16_GET(info.currencyName, 0, 0, info.currencyNameLen, cp);
        result.add(cp);
    }

    માટે (int32_t i=0; i<cacheEntry->total_Currency_Name_ગણતરી; i++) {
        અચલ CurrencyNameStruct& info = cacheEntry->currencyNames[i];
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
U_CAPI શૂન્ય
uprv_getStaticCurrencyName(અચલ char16_t* iso, અચલ અક્ષર* loc,
                           icu::UnicodeString& result, UErrorCode& ec)
{
    U_NAMESPACE_USE

    int32_t len;
    અચલ char16_t* currname = ucurr_getName(iso, loc, UCURR_પ્રતીક_NAME,
                                          nullptr /* isChoiceFormat */, &len, &ec);
    જો (U_SUCCESS(ec)) {
        result.setTo(currname, len);
    }
}

U_CAPI int32_t U_EXPORT2
ucurr_getDefaultFractionDigits(અચલ char16_t* currency, UErrorCode* ec) {
    પાછા ucurr_getDefaultFractionDigitsForUsage(currency,UCURR_USAGE_STANDARD,ec);
}

U_CAPI int32_t U_EXPORT2
ucurr_getDefaultFractionDigitsForUsage(અચલ char16_t* currency, અચલ UCurrencyUsage usage, UErrorCode* ec) {
    int32_t fracDigits = 0;
    જો (U_SUCCESS(*ec)) {
        પસંદગી (usage) {
            સ્થિતિ UCURR_USAGE_STANDARD:
                fracDigits = (_findMetaData(currency, *ec))[0];
                તોડો;
            સ્થિતિ UCURR_USAGE_CASH:
                fracDigits = (_findMetaData(currency, *ec))[2];
                તોડો;
            મૂળભૂત:
                *ec = U_UNSUPPORTED_ERROR;
        }
    }
    પાછા fracDigits;
}

U_CAPI દ્વિગુણ U_EXPORT2
ucurr_getRoundingIncrement(અચલ char16_t* currency, UErrorCode* ec) {
    પાછા ucurr_getRoundingIncrementForUsage(currency, UCURR_USAGE_STANDARD, ec);
}

U_CAPI દ્વિગુણ U_EXPORT2
ucurr_getRoundingIncrementForUsage(અચલ char16_t* currency, અચલ UCurrencyUsage usage, UErrorCode* ec) {
    દ્વિગુણ result = 0.0;

    અચલ int32_t *data = _findMetaData(currency, *ec);
    જો (U_SUCCESS(*ec)) {
        int32_t fracDigits;
        int32_t increment;
        પસંદગી (usage) {
            સ્થિતિ UCURR_USAGE_STANDARD:
                fracDigits = data[0];
                increment = data[1];
                તોડો;
            સ્થિતિ UCURR_USAGE_CASH:
                fracDigits = data[2];
                increment = data[3];
                તોડો;
            મૂળભૂત:
                *ec = U_UNSUPPORTED_ERROR;
                પાછા result;
        }

        // If the meta data is invalid, return 0.0
        જો (fracDigits < 0 || fracDigits > MAX_POW10) {
            *ec = U_INVALID_FORMAT_ERROR;
        } નહીંતર {
            // A rounding value of 0 or 1 indicates no rounding.
            જો (increment >= 2) {
                // Return (increment) / 10^(fracDigits).  The only actual rounding data,
                // as of this writing, is CHF { 2, 5 }.
                result = દ્વિગુણ(increment) / POW10[fracDigits];
            }
        }
    }

    પાછા result;
}

U_CDECL_BEGIN

પ્રકારવ્યાખ્યા સંરચના UCurrencyContext {
    uint32_t currType; /* UCurrCurrencyType */
    uint32_t listIdx;
} UCurrencyContext;

/*
Please keep this list in alphabetical order.
You can look at the CLDR supplemental data or ISO-4217 for the meaning of some
of these items.
ISO-4217: http://www.iso.org/iso/en/prods-services/popstds/currencycodeslist.html
*/
સ્થિર અચલ સંરચના CurrencyList {
    અચલ અક્ષર *currency;
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

સ્થિર int32_t U_CALLCONV
ucurr_ગણતરી_Currency_List(UEnumeration *enumerator, UErrorCode * /*pErrorCode*/) {
    UCurrencyContext *myContext = (UCurrencyContext *)(enumerator->context);
    uint32_t currType = myContext->currType;
    int32_t count = 0;

    /* Count the number of items matching the type we are looking for. */
    માટે (int32_t idx = 0; gCurrencyList[idx].currency != nullptr; idx++) {
        જો (UCURR_MATCHES_BITMASK(gCurrencyList[idx].currType, currType)) {
            count++;
        }
    }
    પાછા count;
}

સ્થિર અચલ અક્ષર* U_CALLCONV
ucurr_nextCurrencyList(UEnumeration *enumerator,
                        int32_t* resultLength,
                        UErrorCode * /*pErrorCode*/)
{
    UCurrencyContext *myContext = (UCurrencyContext *)(enumerator->context);

    /* Find the next in the list that matches the type we are looking for. */
    જ્યારે (myContext->listIdx < UPRV_LENGTHOF(gCurrencyList)-1) {
        અચલ સંરચના CurrencyList *currItem = &gCurrencyList[myContext->listIdx++];
        જો (UCURR_MATCHES_BITMASK(currItem->currType, myContext->currType))
        {
            જો (resultLength) {
                *resultLength = 3; /* Currency codes are only 3 chars long */
            }
            પાછા currItem->currency;
        }
    }
    /* We enumerated too far. */
    જો (resultLength) {
        *resultLength = 0;
    }
    પાછા nullptr;
}

સ્થિર શૂન્ય U_CALLCONV
ucurr_resetCurrencyList(UEnumeration *enumerator, UErrorCode * /*pErrorCode*/) {
    ((UCurrencyContext *)(enumerator->context))->listIdx = 0;
}

સ્થિર શૂન્ય U_CALLCONV
ucurr_closeCurrencyList(UEnumeration *enumerator) {
    uprv_free(enumerator->context);
    uprv_free(enumerator);
}

સ્થિર શૂન્ય U_CALLCONV
ucurr_createCurrencyList(UHashtable *isoCodes, UErrorCode* status){
    UErrorCode localStatus = U_ZERO_ERROR;

    // Look up the CurrencyMap element in the root bundle.
    UResourceBundle *rb = ures_openDirect(U_ICUDATA_CURR, CURRENCY_DATA, &localStatus);
    LocalUResourceBundlePointer currencyMapArray(ures_get_By_ચાવી(rb, CURRENCY_MAP, rb, &localStatus));

    જો (U_SUCCESS(localStatus)) {
        // process each entry in currency map
        માટે (int32_t i=0; i<ures_getSize(currencyMapArray.getAlias()); i++) {
            // get the currency resource
            LocalUResourceBundlePointer currencyArray(ures_getByIndex(currencyMapArray.getAlias(), i, nullptr, &localStatus));
            // process each currency
            જો (U_SUCCESS(localStatus)) {
                માટે (int32_t j=0; j<ures_getSize(currencyArray.getAlias()); j++) {
                    // get the currency resource
                    LocalUResourceBundlePointer currencyRes(ures_getByIndex(currencyArray.getAlias(), j, nullptr, &localStatus));
                    IsoCodeEntry *entry = (IsoCodeEntry*)uprv_malloc(કદ(IsoCodeEntry));
                    જો (entry == nullptr) {
                        *status = U_MEMORY_ALLOCATION_ERROR;
                        પાછા;
                    }

                    // get the ISO code
                    int32_t isoLength = 0;
                    LocalUResourceBundlePointer idRes(ures_get_By_ચાવી(currencyRes.getAlias(), "id", nullptr, &localStatus));
                    જો (idRes.isNull()) {
                        ચાલુ;
                    }
                    અચલ char16_t *isoCode = ures_getString(idRes.getAlias(), &isoLength, &localStatus);

                    // get from date
                    UDate fromDate = U_DATE_MIN;
                    LocalUResourceBundlePointer fromRes(ures_get_By_ચાવી(currencyRes.getAlias(), "from", nullptr, &localStatus));

                    જો (U_SUCCESS(localStatus)) {
                        int32_t fromLength = 0;
                        અચલ int32_t *fromArray = ures_getIntVector(fromRes.getAlias(), &fromLength, &localStatus);
                        int64_t currDate64 = ((uint64_t)fromArray[0]) << 32;
                        currDate64 |= ((int64_t)fromArray[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                        fromDate = (UDate)currDate64;
                    }

                    // get to date
                    UDate toDate = U_DATE_MAX;
                    localStatus = U_ZERO_ERROR;
                    LocalUResourceBundlePointer toRes(ures_get_By_ચાવી(currencyRes.getAlias(), "to", nullptr, &localStatus));

                    જો (U_SUCCESS(localStatus)) {
                        int32_t toLength = 0;
                        અચલ int32_t *toArray = ures_getIntVector(toRes.getAlias(), &toLength, &localStatus);
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
            } નહીંતર {
                *status = localStatus;
            }
        }
    } નહીંતર {
        *status = localStatus;
    }
}

સ્થિર અચલ UEnumeration gEnumCurrencyList = {
    nullptr,
    nullptr,
    ucurr_closeCurrencyList,
    ucurr_ગણતરી_Currency_List,
    uenum_unextDefault,
    ucurr_nextCurrencyList,
    ucurr_resetCurrencyList
};
U_CDECL_END


સ્થિર શૂન્ય U_CALLCONV initIsoCodes(UErrorCode &status) {
    U_ASSERT(gIsoCodes == nullptr);
    ucln_common_registerCleanup(UCLN_COMMON_CURRENCY, currency_cleanup);

    LocalUHashtablePointer isoCodes(uhash_open(uhash_hashUChars, uhash_compareUChars, nullptr, &status));
    જો (U_FAILURE(status)) {
        પાછા;
    }
    uhash_set_કિંમત_Deleter(isoCodes.getAlias(), deleteIsoCodeEntry);

    ucurr_createCurrencyList(isoCodes.getAlias(), &status);
    જો (U_FAILURE(status)) {
        પાછા;
    }
    gIsoCodes = isoCodes.orphan();  // Note: gIsoCodes is const. Once set up here it is never altered,
                                    //       and read only access is safe without synchronization.
}

સ્થિર શૂન્ય populateCurrSymbolsEquiv(icu::Hashtable *hash, UErrorCode &status) {
    જો (U_FAILURE(status)) { પાછા; }
    માટે (અચલ auto& entry : unisets::kCurrencyEntries) {
        UnicodeString exemplar(entry.exemplar);
        અચલ UnicodeSet* set = unisets::get(entry.ચાવી);
        જો (set == nullptr) { પાછા; }
        UnicodeSetIterator it(*set);
        જ્યારે (it.next()) {
            UnicodeString value = it.getString();
            જો (value == exemplar) {
                // No need to mark the exemplar character as an equivalent
                ચાલુ;
            }
            makeEquivalent(exemplar, value, hash, status);
            જો (U_FAILURE(status)) { પાછા; }
        }
    }
}

સ્થિર શૂન્ય U_CALLCONV initCurrSymbolsEquiv() {
    U_ASSERT(gCurrSymbolsEquiv == nullptr);
    UErrorCode status = U_ZERO_ERROR;
    ucln_common_registerCleanup(UCLN_COMMON_CURRENCY, currency_cleanup);
    icu::Hashtable *temp = નવું icu::Hashtable(status);
    જો (temp == nullptr) {
        પાછા;
    }
    જો (U_FAILURE(status)) {
        delete temp;
        પાછા;
    }
    temp->set_કિંમત_Deleter(deleteUnicode);
    populateCurrSymbolsEquiv(temp, status);
    જો (U_FAILURE(status)) {
        delete temp;
        પાછા;
    }
    gCurrSymbolsEquiv = temp;
}

U_CAPI UBool U_EXPORT2
ucurr_isAvailable(અચલ char16_t* isoCode, UDate from, UDate to, UErrorCode* eErrorCode) {
    umtx_initOnce(gIsoCodesInitOnce, &initIsoCodes, *eErrorCode);
    જો (U_FAILURE(*eErrorCode)) {
        પાછા ખોટું;
    }

    IsoCodeEntry* result = (IsoCodeEntry *) uhash_get(gIsoCodes, isoCode);
    જો (result == nullptr) {
        પાછા ખોટું;
    } નહીંતર જો (from > to) {
        *eErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        પાછા ખોટું;
    } નહીંતર જો  ((from > result->to) || (to < result->from)) {
        પાછા ખોટું;
    }
    પાછા સાચું;
}

સ્થિર અચલ icu::Hashtable* getCurrSymbolsEquiv() {
    umtx_initOnce(gCurrSymbolsEquivInitOnce, &initCurrSymbolsEquiv);
    પાછા gCurrSymbolsEquiv;
}

U_CAPI UEnumeration * U_EXPORT2
ucurr_openISOCurrencies(uint32_t currType, UErrorCode *pErrorCode) {
    UEnumeration *myEnum = nullptr;
    UCurrencyContext *myContext;

    myEnum = (UEnumeration*)uprv_malloc(કદ(UEnumeration));
    જો (myEnum == nullptr) {
        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
        પાછા nullptr;
    }
    uprv_memcpy(myEnum, &gEnumCurrencyList, કદ(UEnumeration));
    myContext = (UCurrencyContext*)uprv_malloc(કદ(UCurrencyContext));
    જો (myContext == nullptr) {
        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
        uprv_free(myEnum);
        પાછા nullptr;
    }
    myContext->currType = currType;
    myContext->listIdx = 0;
    myEnum->context = myContext;
    પાછા myEnum;
}

U_CAPI int32_t U_EXPORT2
ucurr_ગણતરી_Currencies(અચલ અક્ષર* locale, 
                 UDate date, 
                 UErrorCode* ec)
{
    int32_t curr_ગણતરી = 0;

    જો (ec != nullptr && U_SUCCESS(*ec)) 
    {
        // local variables
        UErrorCode localStatus = U_ZERO_ERROR;

        // get country or country_variant in `id'
        CharString id = idForLocale(locale, ec);

        જો (U_FAILURE(*ec))
        {
            પાછા 0;
        }

        // Remove variants, which is only needed for registration.
        અક્ષર *idDelim = strchr(id.data(), VAR_DELIM);
        જો (idDelim)
        {
            id.truncate(idDelim - id.data());
        }

        // Look up the CurrencyMap element in the root bundle.
        UResourceBundle *rb = ures_openDirect(U_ICUDATA_CURR, CURRENCY_DATA, &localStatus);
        UResourceBundle *cm = ures_get_By_ચાવી(rb, CURRENCY_MAP, rb, &localStatus);

        // Using the id derived from the local, get the currency data
        LocalUResourceBundlePointer countryArray(ures_get_By_ચાવી(rb, id.data(), cm, &localStatus));

        // process each currency to see which one is valid for the given date
        જો (U_SUCCESS(localStatus))
        {
            માટે (int32_t i=0; i<ures_getSize(countryArray.getAlias()); i++)
            {
                // get the currency resource
                LocalUResourceBundlePointer currencyRes(ures_getByIndex(countryArray.getAlias(), i, nullptr, &localStatus));

                // get the from date
                int32_t fromLength = 0;
                LocalUResourceBundlePointer fromRes(ures_get_By_ચાવી(currencyRes.getAlias(), "from", nullptr, &localStatus));
                અચલ int32_t *fromArray = ures_getIntVector(fromRes.getAlias(), &fromLength, &localStatus);

                int64_t currDate64 = (int64_t)((uint64_t)(fromArray[0]) << 32);
                currDate64 |= ((int64_t)fromArray[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                UDate fromDate = (UDate)currDate64;

                જો (ures_getSize(currencyRes.getAlias())> 2)
                {
                    int32_t toLength = 0;
                    LocalUResourceBundlePointer toRes(ures_get_By_ચાવી(currencyRes.getAlias(), "to", nullptr, &localStatus));
                    અચલ int32_t *toArray = ures_getIntVector(toRes.getAlias(), &toLength, &localStatus);

                    currDate64 = (int64_t)toArray[0] << 32;
                    currDate64 |= ((int64_t)toArray[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                    UDate toDate = (UDate)currDate64;

                    જો ((fromDate <= date) && (date < toDate))
                    {
                        curr_ગણતરી++;
                    }
                }
                નહીંતર
                {
                    જો (fromDate <= date)
                    {
                        curr_ગણતરી++;
                    }
                }
            } // end For loop
        } // end if (U_SUCCESS(localStatus))

        // Check for errors
        જો (*ec == U_ZERO_ERROR || localStatus != U_ZERO_ERROR)
        {
            // There is nothing to fallback to. 
            // Report the failure/warning if possible.
            *ec = localStatus;
        }

        જો (U_SUCCESS(*ec))
        {
            // no errors
            પાછા curr_ગણતરી;
        }
    }

    // If we got here, either error code is invalid or
    // some argument passed is no good.
    પાછા 0;
}

U_CAPI int32_t U_EXPORT2 
ucurr_forLocaleAndDate(અચલ અક્ષર* locale, 
                UDate date, 
                int32_t index,
                char16_t* buff,
                int32_t buffCapacity, 
                UErrorCode* ec)
{
    int32_t resLen = 0;
	int32_t currIndex = 0;
    અચલ char16_t* s = nullptr;

    જો (ec != nullptr && U_SUCCESS(*ec))
    {
        // check the arguments passed
        જો ((buff && buffCapacity) || !buffCapacity )
        {
            // local variables
            UErrorCode localStatus = U_ZERO_ERROR;

            // get country or country_variant in `id'
            CharString id = idForLocale(locale, ec);
            જો (U_FAILURE(*ec))
            {
                પાછા 0;
            }

            // Remove variants, which is only needed for registration.
            અક્ષર *idDelim = strchr(id.data(), VAR_DELIM);
            જો (idDelim)
            {
                id.truncate(idDelim - id.data());
            }

            // Look up the CurrencyMap element in the root bundle.
            UResourceBundle *rb = ures_openDirect(U_ICUDATA_CURR, CURRENCY_DATA, &localStatus);
            UResourceBundle *cm = ures_get_By_ચાવી(rb, CURRENCY_MAP, rb, &localStatus);

            // Using the id derived from the local, get the currency data
            LocalUResourceBundlePointer countryArray(ures_get_By_ચાવી(rb, id.data(), cm, &localStatus));

            // process each currency to see which one is valid for the given date
            તાર્કિક matchFound = ખોટું;
            જો (U_SUCCESS(localStatus))
            {
                જો ((index <= 0) || (index> ures_getSize(countryArray.getAlias())))
                {
                    // requested index is out of bounds
                    પાછા 0;
                }

                માટે (int32_t i=0; i<ures_getSize(countryArray.getAlias()); i++)
                {
                    // get the currency resource
                    LocalUResourceBundlePointer currencyRes(ures_getByIndex(countryArray.getAlias(), i, nullptr, &localStatus));
                    s = ures_get_String_By_ચાવી(currencyRes.getAlias(), "id", &resLen, &localStatus);

                    // get the from date
                    int32_t fromLength = 0;
                    LocalUResourceBundlePointer fromRes(ures_get_By_ચાવી(currencyRes.getAlias(), "from", nullptr, &localStatus));
                    અચલ int32_t *fromArray = ures_getIntVector(fromRes.getAlias(), &fromLength, &localStatus);

                    int64_t currDate64 = (int64_t)((uint64_t)fromArray[0] << 32);
                    currDate64 |= ((int64_t)fromArray[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                    UDate fromDate = (UDate)currDate64;

                    જો (ures_getSize(currencyRes.getAlias()) > 2)
                    {
                        int32_t toLength = 0;
                        LocalUResourceBundlePointer toRes(ures_get_By_ચાવી(currencyRes.getAlias(), "to", nullptr, &localStatus));
                        અચલ int32_t *toArray = ures_getIntVector(toRes.getAlias(), &toLength, &localStatus);

                        currDate64 = (int64_t)toArray[0] << 32;
                        currDate64 |= ((int64_t)toArray[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                        UDate toDate = (UDate)currDate64;

                        જો ((fromDate <= date) && (date < toDate))
                        {
                            currIndex++;
                            જો (currIndex == index)
                            {
                                matchFound = સાચું;
                            }
                        }
                    }
                    નહીંતર
                    {
                        જો (fromDate <= date)
                        {
                            currIndex++;
                            જો (currIndex == index)
                            {
                                matchFound = સાચું;
                            }
                        }
                    }
                    // check for loop exit
                    જો (matchFound)
                    {
                        તોડો;
                    }

                } // end For loop
            }

            // Check for errors
            જો (*ec == U_ZERO_ERROR || localStatus != U_ZERO_ERROR)
            {
                // There is nothing to fallback to. 
                // Report the failure/warning if possible.
                *ec = localStatus;
            }

            જો (U_SUCCESS(*ec))
            {
                // no errors
                જો((buffCapacity> resLen) && matchFound)
                {
                    // write out the currency value
                    u_strcpy(buff, s);
                }
                નહીંતર
                {
                    પાછા 0;
                }
            }

            // return null terminated currency string
            પાછા u_terminateUChars(buff, buffCapacity, resLen, ec);
        }
        નહીંતર
        {
            // illegal argument encountered
            *ec = U_ILLEGAL_ARGUMENT_ERROR;
        }

    }

    // If we got here, either error code is invalid or
    // some argument passed is no good.
    પાછા resLen;
}

સ્થિર અચલ UEnumeration default_Keyword_કિંમતો = {
    nullptr,
    nullptr,
    ulist_close_keyword_કિંમતો_iterator,
    ulist_ગણતરી_keyword_કિંમતો,
    uenum_unextDefault,
    ulist_next_keyword_કિંમત, 
    ulist_reset_keyword_કિંમતો_iterator
};

U_CAPI UEnumeration *U_EXPORT2 ucurr_get_Keyword_કિંમતો_For_Locale(અચલ અક્ષર *ચાવી, અચલ અક્ષર *locale, UBool commonlyUsed, UErrorCode* status) {
    // Resolve region
    CharString prefRegion = ulocimp_getRegionForSupplementalData(locale, સાચું, *status);

    // Read value from supplementalData
    UList *કિંમતો = ulist_createEmptyList(status);
    UList *other_કિંમતો = ulist_createEmptyList(status);
    UEnumeration *en = (UEnumeration *)uprv_malloc(કદ(UEnumeration));
    જો (U_FAILURE(*status) || en == nullptr) {
        જો (en == nullptr) {
            *status = U_MEMORY_ALLOCATION_ERROR;
        } નહીંતર {
            uprv_free(en);
        }
        ulist_deleteList(કિંમતો);
        ulist_deleteList(other_કિંમતો);
        પાછા nullptr;
    }
    memcpy(en, &default_Keyword_કિંમતો, કદ(UEnumeration));
    en->context = કિંમતો;
    
    UResourceBundle* rb = ures_openDirect(U_ICUDATA_CURR, "supplementalData", status);
    LocalUResourceBundlePointer bundle(ures_get_By_ચાવી(rb, "CurrencyMap", rb, status));
    ઢગલો_UResource_Bundle bundlekey, regbndl, curbndl, to;
    
    જ્યારે (U_SUCCESS(*status) && ures_hasNext(bundle.getAlias())) {
        ures_getNextResource(bundle.getAlias(), bundlekey.getAlias(), status);
        જો (U_FAILURE(*status)) {
            તોડો;
        }
        અચલ અક્ષર *region = ures_get_ચાવી(bundlekey.getAlias());
        UBool isPrefRegion = prefRegion == region;
        જો (!isPrefRegion && commonlyUsed) {
            // With commonlyUsed=true, we do not put
            // currencies for other regions in the
            // result list.
            ચાલુ;
        }
        ures_get_By_ચાવી(bundle.getAlias(), region, regbndl.getAlias(), status);
        જો (U_FAILURE(*status)) {
            તોડો;
        }
        જ્યારે (U_SUCCESS(*status) && ures_hasNext(regbndl.getAlias())) {
            ures_getNextResource(regbndl.getAlias(), curbndl.getAlias(), status);
            જો (ures_getType(curbndl.getAlias()) != URES_TABLE) {
                // Currently, an empty ARRAY is mixed in.
                ચાલુ;
            }
            અક્ષર *curID = (અક્ષર *)uprv_malloc(કદ(અક્ષર) * ULOC_KEYWORDS_CAPACITY);
            જો (curID == nullptr) {
                *status = U_MEMORY_ALLOCATION_ERROR;
                તોડો;
            }
            int32_t curIDLength = ULOC_KEYWORDS_CAPACITY;

#જો U_CHARSET_FAMILY==U_ASCII_FAMILY
            ures_get_UTF8_String_By_ચાવી(curbndl.getAlias(), "id", curID, &curIDLength, સાચું, status);
            /* optimize - use the utf-8 string */
#નહીંતર
            {
                       અચલ char16_t* defString = ures_get_String_By_ચાવી(curbndl.getAlias(), "id", &curIDLength, status);
                       જો(U_SUCCESS(*status)) {
			   જો(curIDLength+1 > ULOC_KEYWORDS_CAPACITY) {
				*status = U_BUFFER_OVERFLOW_ERROR;
			   } નહીંતર {
                           	u_UCharsToChars(defString, curID, curIDLength+1);
			   }
                       }
            }
#endif	

            જો (U_FAILURE(*status)) {
                તોડો;
            }
            UBool hasTo = ખોટું;
            ures_get_By_ચાવી(curbndl.getAlias(), "to", to.getAlias(), status);
            જો (U_FAILURE(*status)) {
                // Do nothing here...
                *status = U_ZERO_ERROR;
            } નહીંતર {
                hasTo = સાચું;
            }
            જો (isPrefRegion && !hasTo && !ulist_containsString(કિંમતો, curID, (int32_t)uprv_strlen(curID))) {
                // Currently active currency for the target country
                ulist_addItemEndList(કિંમતો, curID, સાચું, status);
            } નહીંતર જો (!ulist_containsString(other_કિંમતો, curID, (int32_t)uprv_strlen(curID)) && !commonlyUsed) {
                ulist_addItemEndList(other_કિંમતો, curID, સાચું, status);
            } નહીંતર {
                uprv_free(curID);
            }
        }
        
    }
    જો (U_SUCCESS(*status)) {
        જો (commonlyUsed) {
            જો (ulist_getListSize(કિંમતો) == 0) {
                // This could happen if no valid region is supplied in the input
                // locale. In this case, we use the CLDR's default.
                uenum_close(en);
                en = ucurr_get_Keyword_કિંમતો_For_Locale(ચાવી, "und", સાચું, status);
            }
        } નહીંતર {
            // Consolidate the list
            અક્ષર *value = nullptr;
            ulist_resetList(other_કિંમતો);
            જ્યારે ((value = (અક્ષર *)ulist_getNext(other_કિંમતો)) != nullptr) {
                જો (!ulist_containsString(કિંમતો, value, (int32_t)uprv_strlen(value))) {
                    અક્ષર *tmp_કિંમત = (અક્ષર *)uprv_malloc(કદ(અક્ષર) * ULOC_KEYWORDS_CAPACITY);
                    જો (tmp_કિંમત == nullptr) {
                        *status = U_MEMORY_ALLOCATION_ERROR;
                        તોડો;
                    }
                    uprv_memcpy(tmp_કિંમત, value, uprv_strlen(value) + 1);
                    ulist_addItemEndList(કિંમતો, tmp_કિંમત, સાચું, status);
                    જો (U_FAILURE(*status)) {
                        તોડો;
                    }
                }
            }
        }
        ulist_resetList((UList *)(en->context));
    } નહીંતર {
        ulist_deleteList(કિંમતો);
        uprv_free(en);
        કિંમતો = nullptr;
        en = nullptr;
    }
    ulist_deleteList(other_કિંમતો);
    પાછા en;
}


U_CAPI int32_t U_EXPORT2
ucurr_getNumericCode(અચલ char16_t* currency) {
    int32_t code = 0;
    જો (currency && u_strlen(currency) == ISO_CURRENCY_CODE_LENGTH) {
        UErrorCode status = U_ZERO_ERROR;

        UResourceBundle* bundle = ures_openDirect(nullptr, "currencyNumericCodes", &status);
        LocalUResourceBundlePointer codeMap(ures_get_By_ચાવી(bundle, "codeMap", bundle, &status));
        જો (U_SUCCESS(status)) {
            અક્ષર alphaCode[ISO_CURRENCY_CODE_LENGTH+1];
            myUCharsToChars(alphaCode, currency);
            T_CString_toUpperCase(alphaCode);
            ures_get_By_ચાવી(codeMap.getAlias(), alphaCode, codeMap.getAlias(), &status);
            પૂર્ણાંક tmpCode = ures_getInt(codeMap.getAlias(), &status);
            જો (U_SUCCESS(status)) {
                code = tmpCode;
            }
        }
    }
    પાછા code;
}
#endif /* #if !UCONFIG_NO_FORMATTING */

//eof
