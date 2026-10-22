// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
* Copyright (c) 2002-2016, International Business Machines
* Corporation and others.  All Rights Reserved.
**********************************************************************
*/

#ضمن "unicode/utypes.h"

#إذا !UCONFIG_NO_FORMATTING

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

استعمال نطاق icu;

//#define UCURR_DEBUG_EQUIV 1
#جيڪڏهن_وصف_ٿيل UCURR_DEBUG_EQUIV
#ضمن "stdio.h"
#جيڪڏهن_پورو
//#define UCURR_DEBUG 1
#جيڪڏهن_وصف_ٿيل UCURR_DEBUG
#ضمن "stdio.h"
#جيڪڏهن_پورو

تعريفنوع بنية Iso_ڪوڊ_داخلا {
    ثابت char16_t *iso_ڪوڊ; /* const because it's a reference to a resource bundle string. */
    UDate from;
    UDate منزل;
} Iso_ڪوڊ_داخلا;

//------------------------------------------------------------
// Constants

// Default currency meta data of last resort.  We try to use the
// defaults encoded in the meta data resource bundle.  If there is a
// configuration/build error and these are not available, we use these
// hard-coded defaults (which should be identical).
ساكن ثابت int32_t آخري_RESORT_معلومات[] = { 2, 0, 2, 0 };

// POW10[i] = 10^i, i=0..MAX_POW10
ساكن ثابت int32_t POW10[] = { 1, 10, 100, 1000, 10000, 100000,
                                 1000000, 10000000, 100000000, 1000000000 };

ساكن ثابت int32_t MAX_POW10 = UPRV_LENGTHOF(POW10) - 1;

#وصف_ڪريو ISO_CURRENCY_ڪوڊ_ڊيگهائي 3

//------------------------------------------------------------
// Resource tags
//

ساكن ثابت حرف CURRENCY_معلومات[] = "supplementalData";
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
ساكن ثابت UHashtable* gIsoCodes = خالي_اشارو;
ساكن icu::UInitOnce gIsoCodesInitOnce {};

// Currency symbol equivalances
ساكن ثابت icu::Hashtable* gCurrSymbolsEquiv = خالي_اشارو;
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
        شروعات_2 = _current = &s;
    }
    مضمن ~EquivIterator() { }

    // next returns the next equivalent string or nullptr if there are no more.
    // If s has no equivalent strings, next returns nullptr on the first call.
    ثابت icu::UnicodeString *next();
خاص:
    ثابت icu::Hashtable& _hash;
    ثابت icu::UnicodeString* شروعات_2;
    ثابت icu::UnicodeString* _current;
};

ثابت icu::UnicodeString *
EquivIterator::next() {
    ثابت icu::UnicodeString* ايندڙ_2 = static_cast<ثابت icu::UnicodeString*>(_hash.get(*_current));
    إذا (ايندڙ_2 == خالي_اشارو) {
        U_ASSERT(_current == شروعات_2);
        أرجع خالي_اشارو;
    }
    إذا (*ايندڙ_2 == *شروعات_2) {
        أرجع خالي_اشارو;
    }
    _current = ايندڙ_2;
    أرجع ايندڙ_2;
}

U_NAMESPACE_پڇاڙي

// makeEquivalent makes lhs and rhs equivalent by updating the equivalence
// relations in hash accordingly.
ساكن فراغ makeEquivalent(
    ثابت icu::UnicodeString &lhs,
    ثابت icu::UnicodeString &rhs,
    icu::Hashtable* hash, UError_ڪوڊ &حالت) {
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    إذا (lhs == rhs) {
        // already equivalent
        أرجع;
    }
    icu::EquivIterator کاٻو_Iter(*hash, lhs);
    icu::EquivIterator ساڄو_Iter(*hash, rhs);
    ثابت icu::UnicodeString *پهريون_کاٻو = کاٻو_Iter.next();
    ثابت icu::UnicodeString *پهريون_ساڄو = ساڄو_Iter.next();
    ثابت icu::UnicodeString *ايندڙ_کاٻو = پهريون_کاٻو;
    ثابت icu::UnicodeString *ايندڙ_ساڄو = پهريون_ساڄو;
    طالما (ايندڙ_کاٻو != خالي_اشارو && ايندڙ_ساڄو != خالي_اشارو) {
        إذا (*ايندڙ_کاٻو == rhs || *ايندڙ_ساڄو == lhs) {
            // Already equivalent
            أرجع;
        }
        ايندڙ_کاٻو = کاٻو_Iter.next();
        ايندڙ_ساڄو = ساڄو_Iter.next();
    }
    // Not equivalent. Must join.
    icu::UnicodeString *new_پهريون_کاٻو;
    icu::UnicodeString *new_پهريون_ساڄو;
    إذا (پهريون_ساڄو == خالي_اشارو && پهريون_کاٻو == خالي_اشارو) {
        // Neither lhs or rhs belong to an equivalence circle, so we form
        // a new equivalnce circle of just lhs and rhs.
        new_پهريون_کاٻو = جديد icu::UnicodeString(rhs);
        new_پهريون_ساڄو = جديد icu::UnicodeString(lhs);
    } وإلا إذا (پهريون_ساڄو == خالي_اشارو) {
        // lhs belongs to an equivalence circle, but rhs does not, so we link
        // rhs into lhs' circle.
        new_پهريون_کاٻو = جديد icu::UnicodeString(rhs);
        new_پهريون_ساڄو = جديد icu::UnicodeString(*پهريون_کاٻو);
    } وإلا إذا (پهريون_کاٻو == خالي_اشارو) {
        // rhs belongs to an equivlance circle, but lhs does not, so we link
        // lhs into rhs' circle.
        new_پهريون_کاٻو = جديد icu::UnicodeString(*پهريون_ساڄو);
        new_پهريون_ساڄو = جديد icu::UnicodeString(lhs);
    } وإلا {
        // Both lhs and rhs belong to different equivalnce circles. We link
        // them together to form one single, larger equivalnce circle.
        new_پهريون_کاٻو = جديد icu::UnicodeString(*پهريون_ساڄو);
        new_پهريون_ساڄو = جديد icu::UnicodeString(*پهريون_کاٻو);
    }
    إذا (new_پهريون_کاٻو == خالي_اشارو || new_پهريون_ساڄو == خالي_اشارو) {
        ميٽيو new_پهريون_کاٻو;
        ميٽيو new_پهريون_ساڄو;
        حالت = U_يادگيري_ورهاست_ERROR;
        أرجع;
    }
    hash->put(lhs, (فراغ *) new_پهريون_کاٻو, حالت);
    hash->put(rhs, (فراغ *) new_پهريون_ساڄو, حالت);
}

// countEquivalent counts how many strings are equivalent to s.
// hash stores all the equivalnce relations.
// countEquivalent does not include s itself in the count.
ساكن int32_t ڳڻپ_Equivalent(ثابت icu::Hashtable &hash, ثابت icu::UnicodeString &s) {
    int32_t نتيجو = 0;
    icu::EquivIterator iter(hash, s);
    طالما (iter.next() != خالي_اشارو) {
        ++نتيجو;
    }
#جيڪڏهن_وصف_ٿيل UCURR_DEBUG_EQUIV
 {
   حرف tmp[200];
   s.extract(0,s.length(),tmp, "UTF-8");
   لکو("CountEquivalent('%s') = %d\n", tmp, نتيجو);
 }
#جيڪڏهن_پورو
    أرجع نتيجو;
}

ساكن ثابت icu::Hashtable* getCurrSymbolsEquiv();

//------------------------------------------------------------
//ڪوڊ

/**
 * Cleanup callback func
 */
ساكن UBool U_CALLCONV 
isoCodes_cleanup()
{
    إذا (gIsoCodes != خالي_اشارو) {
        uhash_close(const_cast<UHashtable *>(gIsoCodes));
        gIsoCodes = خالي_اشارو;
    }
    gIsoCodesInitOnce.reset();
    أرجع سچ;
}

/**
 * Cleanup callback func
 */
ساكن UBool U_CALLCONV 
currSymbolsEquiv_cleanup()
{
    ميٽيو const_cast<icu::Hashtable *>(gCurrSymbolsEquiv);
    gCurrSymbolsEquiv = خالي_اشارو;
    gCurrSymbolsEquivInitOnce.reset();
    أرجع سچ;
}

/**
 * Deleter for IsoCodeEntry
 */
ساكن فراغ U_CALLCONV
delete_Iso_ڪوڊ_داخلا(فراغ *obj) {
    Iso_ڪوڊ_داخلا* داخلا_2 = static_cast<Iso_ڪوڊ_داخلا*>(obj);
    uprv_free(داخلا_2);
}

/**
 * Deleter for gCurrSymbolsEquiv.
 */
ساكن فراغ U_CALLCONV
deleteUnicode(فراغ *obj) {
    icu::UnicodeString* داخلا_2 = static_cast<icu::UnicodeString*>(obj);
    ميٽيو داخلا_2;
}

/**
 * Unfortunately, we have to convert the char16_t* currency code to char*
 * to use it as a resource key.
 */
ساكن مضمن حرف*
my_UChars_منزل_Chars(حرف* نتيجو_جو_Len4, ثابت char16_t* currency) {
    u_UChars_منزل_Chars(currency, نتيجو_جو_Len4, ISO_CURRENCY_ڪوڊ_ڊيگهائي);
    نتيجو_جو_Len4[ISO_CURRENCY_ڪوڊ_ڊيگهائي] = 0;
    أرجع نتيجو_جو_Len4;
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
ڳوليو_Meta_معلومات(ثابت char16_t* currency, UError_ڪوڊ& ec) {

    إذا (currency == خالي_اشارو || *currency == 0) {
        إذا (U_SUCCESS(ec)) {
            ec = U_ILLEGAL_دليل_ERROR;
        }
        أرجع آخري_RESORT_معلومات;
    }

    // Get CurrencyMeta resource out of root locale file.  [This may
    // move out of the root locale file later; if it does, update this
    // code.]
    UResourceBundle* currency_معلومات = ures_open_سڌو(U_ICUDATA_CURR, CURRENCY_معلومات, &ec);
    Local_UResource_Bundle_اشارو currencyMeta(ures_get_By_ڪنجي(currency_معلومات, CURRENCY_META, currency_معلومات, &ec));

    إذا (U_FAILURE(ec)) {
        // Config/build error; return hard-coded defaults
        أرجع آخري_RESORT_معلومات;
    }

    // Look up our currency, or if that's not available, then DEFAULT
    حرف buf[ISO_CURRENCY_ڪوڊ_ڊيگهائي+1];
    UError_ڪوڊ ec2 = U_ZERO_ERROR; // local error code: soft failure
    Local_UResource_Bundle_اشارو rb(ures_get_By_ڪنجي(currencyMeta.getAlias(), my_UChars_منزل_Chars(buf, currency), خالي_اشارو, &ec2));
      إذا (U_FAILURE(ec2)) {
        rb.adoptInstead(ures_get_By_ڪنجي(currencyMeta.getAlias(),DEFAULT_META, خالي_اشارو, &ec));
        إذا (U_FAILURE(ec)) {
            // Config/build error; return hard-coded defaults
            أرجع آخري_RESORT_معلومات;
        }
    }

    int32_t len;
    ثابت int32_t *معلومات = ures_get_Int_ويڪٽر(rb.getAlias(), &len, &ec);
    إذا (U_FAILURE(ec) || len != 4) {
        // Config/build error; return hard-coded defaults
        إذا (U_SUCCESS(ec)) {
            ec = U_INVALID_FORMAT_ERROR;
        }
        أرجع آخري_RESORT_معلومات;
    }

    أرجع معلومات;
}

// -------------------------------------

ساكن CharString
idForLocale(ثابت حرف* locale, UError_ڪوڊ* ec)
{
    أرجع ulocimp_get_Region_For_Supplemental_معلومات(locale, خطأ, *ec);
}

// ------------------------------------------
//
// Registration
//
//-------------------------------------------

// don't use ICUService since we don't need fallback

U_CDECL_BEGIN
ساكن UBool U_CALLCONV currency_cleanup();
U_CDECL_پڇاڙي

#إذا !UCONFIG_NO_SERVICE
بنية CReg;

ساكن UMutex g_CReg_تالو;
ساكن CReg* g_CReg_سر = خالي_اشارو;

بنية CReg : عام icu::UMemory {
    CReg *next;
    char16_t iso[ISO_CURRENCY_ڪوڊ_ڊيگهائي+1];
    حرف  id[ULOC_FULLNAME_صلاحيت];

    CReg(ثابت char16_t* _iso, ثابت حرف* _id)
        : next(خالي_اشارو)
    {
        uprv_strncpy(id, _id, حجم(id)-1);
        id[حجم(id)-1] = 0;
        u_memcpy(iso, _iso, ISO_CURRENCY_ڪوڊ_ڊيگهائي);
        iso[ISO_CURRENCY_ڪوڊ_ڊيگهائي] = 0;
    }

    ساكن UCurr_Registry_ڪنجي reg(ثابت char16_t* _iso, ثابت حرف* _id, UError_ڪوڊ* حالت)
    {
        إذا (حالت && U_SUCCESS(*حالت) && _iso && _id) {
            CReg* n = جديد CReg(_iso, _id);
            إذا (n) {
                umtx_تالو(&g_CReg_تالو);
                إذا (!g_CReg_سر) {
                    /* register for the first time */
                    ucln_common_رجسٽر_Cleanup(UCLN_COMMON_CURRENCY, currency_cleanup);
                }
                n->next = g_CReg_سر;
                g_CReg_سر = n;
                umtx_unlock(&g_CReg_تالو);
                أرجع n;
            }
            *حالت = U_يادگيري_ورهاست_ERROR;
        }
        أرجع خالي_اشارو;
    }

    ساكن UBool unreg(UCurr_Registry_ڪنجي ڪنجي) {
        UBool مليو = خطأ;
        umtx_تالو(&g_CReg_تالو);

        CReg** p = &g_CReg_سر;
        طالما (*p) {
            إذا (*p == ڪنجي) {
                *p = ((CReg*)ڪنجي)->next;
                ميٽيو (CReg*)ڪنجي;
                مليو = سچ;
                اكسر;
            }
            p = &((*p)->next);
        }

        umtx_unlock(&g_CReg_تالو);
        أرجع مليو;
    }

    ساكن ثابت char16_t* get(ثابت حرف* id) {
        ثابت char16_t* نتيجو = خالي_اشارو;
        umtx_تالو(&g_CReg_تالو);
        CReg* p = g_CReg_سر;

        /* register cleanup of the mutex */
        ucln_common_رجسٽر_Cleanup(UCLN_COMMON_CURRENCY, currency_cleanup);
        طالما (p) {
            إذا (uprv_strcmp(id, p->id) == 0) {
                نتيجو = p->iso;
                اكسر;
            }
            p = p->next;
        }
        umtx_unlock(&g_CReg_تالو);
        أرجع نتيجو;
    }

    /* This doesn't need to be thread safe. It's for u_cleanup only. */
    ساكن فراغ cleanup() {
        طالما (g_CReg_سر) {
            CReg* n = g_CReg_سر;
            g_CReg_سر = g_CReg_سر->next;
            ميٽيو n;
        }
    }
};

// -------------------------------------

U_CAPI UCurr_Registry_ڪنجي U_EXPORT2
ucurr_رجسٽر(ثابت char16_t* iso_ڪوڊ, ثابت حرف* locale, UError_ڪوڊ *حالت)
{
    إذا (حالت && U_SUCCESS(*حالت)) {
        CharString id = idForLocale(locale, حالت);
        أرجع CReg::reg(iso_ڪوڊ, id.معلومات(), حالت);
    }
    أرجع خالي_اشارو;
}

// -------------------------------------

U_CAPI UBool U_EXPORT2
ucurr_unregister(UCurr_Registry_ڪنجي ڪنجي, UError_ڪوڊ* حالت)
{
    إذا (حالت && U_SUCCESS(*حالت)) {
        أرجع CReg::unreg(ڪنجي);
    }
    أرجع خطأ;
}
#جيڪڏهن_پورو /* UCONFIG_NO_SERVICE */

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
#إذا !UCONFIG_NO_SERVICE
    CReg::cleanup();
#جيڪڏهن_پورو
    /*
     * There might be some cached currency data or isoCodes data.
     */
    currency_cache_cleanup();
    isoCodes_cleanup();
    currSymbolsEquiv_cleanup();

    أرجع سچ;
}
U_CDECL_پڇاڙي

// -------------------------------------

U_CAPI int32_t U_EXPORT2
ucurr_forLocale(ثابت حرف* locale,
                char16_t* buff,
                int32_t buff_صلاحيت,
                UError_ڪوڊ* ec) {
    إذا (U_FAILURE(*ec)) { أرجع 0; }
    إذا (buff_صلاحيت < 0 || (buff == خالي_اشارو && buff_صلاحيت > 0)) {
        *ec = U_ILLEGAL_دليل_ERROR;
        أرجع 0;
    }

    UError_ڪوڊ local_حالت = U_ZERO_ERROR;
    CharString currency = ulocimp_get_Keyword_قدر(locale, "currency", local_حالت);
    int32_t resLen = currency.length();

    إذا (U_SUCCESS(local_حالت) && resLen == 3 && uprv_isInvariantString(currency.معلومات(), resLen)) {
        إذا (resLen < buff_صلاحيت) {
            T_CString_منزل_مٿانهون_Case(currency.معلومات());
            u_chars_منزل_UChars(currency.معلومات(), buff, resLen);
        }
        أرجع u_terminateUChars(buff, buff_صلاحيت, resLen, ec);
    }

    // get country or country_variant in `id'
    CharString id = idForLocale(locale, ec);
    إذا (U_FAILURE(*ec)) {
        أرجع 0;
    }

#إذا !UCONFIG_NO_SERVICE
    ثابت char16_t* نتيجو = CReg::get(id.معلومات());
    إذا (نتيجو) {
        إذا(buff_صلاحيت > u_strlen(نتيجو)) {
            u_strcpy(buff, نتيجو);
        }
        resLen = u_strlen(نتيجو);
        أرجع u_terminateUChars(buff, buff_صلاحيت, resLen, ec);
    }
#جيڪڏهن_پورو
    // Remove variants, which is only needed for registration.
    حرف *idDelim = uprv_strchr(id.معلومات(), VAR_DELIM);
    إذا (idDelim) {
        id.truncate(idDelim - id.معلومات());
    }

    ثابت char16_t* s = خالي_اشارو;  // Currency code from data file.
    إذا (id.is_خالي_جايون()) {
        // No point looking in the data for an empty string.
        // This is what we would get.
        local_حالت = U_MISSING_RESOURCE_ERROR;
    } وإلا {
        // Look up the CurrencyMap element in the root bundle.
        local_حالت = U_ZERO_ERROR;
        UResourceBundle *rb = ures_open_سڌو(U_ICUDATA_CURR, CURRENCY_معلومات, &local_حالت);
        UResourceBundle *cm = ures_get_By_ڪنجي(rb, CURRENCY_MAP, rb, &local_حالت);
        Local_UResource_Bundle_اشارو countryArray(ures_get_By_ڪنجي(rb, id.معلومات(), cm, &local_حالت));
        // https://unicode-org.atlassian.net/browse/ICU-21997
        // Prefer to use currencies that are legal tender.
        إذا (U_SUCCESS(local_حالت)) {
            int32_t array_ڊيگهائي = ures_get_سائيز(countryArray.getAlias());
            لكل (int32_t i = 0; i < array_ڊيگهائي; ++i) {
                Local_UResource_Bundle_اشارو currencyReq(
                    ures_get_By_اشاريو(countryArray.getAlias(), i, خالي_اشارو, &local_حالت));
                // The currency is legal tender if it is *not* marked with tender{"false"}.
                UError_ڪوڊ tender_حالت = local_حالت;
                ثابت char16_t *tender =
                    ures_get_String_By_ڪنجي(currencyReq.getAlias(), "tender", خالي_اشارو, &tender_حالت);
                منطقي isTender = U_FAILURE(tender_حالت) || u_strcmp(tender, u"false") != 0;
                إذا (!isTender && s != خالي_اشارو) {
                    // We already have a non-tender currency. Ignore all following non-tender ones.
                    استمر;
                }
                // Fetch the currency code.
                s = ures_get_String_By_ڪنجي(currencyReq.getAlias(), "id", &resLen, &local_حالت);
                إذا (isTender) {
                    اكسر;
                }
            }
            إذا (U_SUCCESS(local_حالت) && s == خالي_اشارو) {
                local_حالت = U_MISSING_RESOURCE_ERROR;
            }
        }
    }

    إذا ((U_FAILURE(local_حالت)) && strchr(id.معلومات(), '_') != خالي_اشارو) {
        // We don't know about it.  Check to see if we support the variant.
        CharString والد = ulocimp_get_والد(locale, *ec);
        *ec = U_USING_FALLBACK_WARNING;
        // TODO: Loop over the parent rather than recursing and
        // looking again for a currency keyword.
        أرجع ucurr_forLocale(والد.معلومات(), buff, buff_صلاحيت, ec);
    }
    إذا (*ec == U_ZERO_ERROR || local_حالت != U_ZERO_ERROR) {
        // There is nothing to fallback to. Report the failure/warning if possible.
        *ec = local_حالت;
    }
    إذا (U_SUCCESS(*ec)) {
        إذا(buff_صلاحيت > resLen) {
            u_strcpy(buff, s);
        }
    }
    أرجع u_terminateUChars(buff, buff_صلاحيت, resLen, ec);
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
    إذا (loc.is_خالي_جايون()) {
        أرجع خطأ;
    }
    UError_ڪوڊ حالت = U_ZERO_ERROR;
    إذا (loc == "en_GB") {
        // HACK: See #13368.  We need "en_GB" to fall back to "en_001" instead of "en"
        // in order to consume the correct data strings.  This hack will be removed
        // when proper data sink loading is implemented here.
        loc.truncate(3);
        loc.append("001", حالت);
    } وإلا {
        loc = ulocimp_get_والد(loc.معلومات(), حالت);
    }
 /*
    char *i = uprv_strrchr(loc, '_');
    if (i == nullptr) {
        i = loc;
    }
    *i = 0;
 */
    أرجع سچ;
}


U_CAPI ثابت char16_t* U_EXPORT2
ucurr_getName(ثابت char16_t* currency U_LIFETIME_BOUND,
              ثابت حرف* locale,
              UCurrNameStyle nameStyle,
              UBool* isChoiceFormat, // fillin
              int32_t* len, // fillin
              UError_ڪوڊ* ec) {

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

    إذا (U_FAILURE(*ec)) {
        أرجع خالي_اشارو;
    }

    int32_t choice = (int32_t) nameStyle;
    إذا (choice < 0 || choice > 4) {
        *ec = U_ILLEGAL_دليل_ERROR;
        أرجع خالي_اشارو;
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
    UError_ڪوڊ ec2 = U_ZERO_ERROR;

    إذا (locale == خالي_اشارو) {
        locale = uloc_getDefault();
    }
    CharString loc = ulocimp_getName(locale, ec2);
    إذا (U_FAILURE(ec2)) {
        *ec = U_ILLEGAL_دليل_ERROR;
        أرجع خالي_اشارو;
    }

    حرف buf[ISO_CURRENCY_ڪوڊ_ڊيگهائي+1];
    my_UChars_منزل_Chars(buf, currency);
    
    /* Normalize the keyword value to uppercase */
    T_CString_منزل_مٿانهون_Case(buf);
    
    ثابت char16_t* s = خالي_اشارو;
    ec2 = U_ZERO_ERROR;
    Local_UResource_Bundle_اشارو rb(ures_open(U_ICUDATA_CURR, loc.معلومات(), &ec2));

    إذا (nameStyle == UCURR_NARROW_نشان_NAME || nameStyle == UCURR_FORMAL_نشان_NAME || nameStyle == UCURR_VARIANT_نشان_NAME) {
        CharString ڪنجي;
        بدل (nameStyle) {
        حالة UCURR_NARROW_نشان_NAME:
            ڪنجي.append(CURRENCIES_NARROW, ec2);
            اكسر;
        حالة UCURR_FORMAL_نشان_NAME:
            ڪنجي.append(CURRENCIES_FORMAL, ec2);
            اكسر;
        حالة UCURR_VARIANT_نشان_NAME:
            ڪنجي.append(CURRENCIES_VARIANT, ec2);
            اكسر;
        افتراضي:
            *ec = U_UNSUPPORTED_ERROR;
            أرجع خالي_اشارو;
        }
        ڪنجي.append("/", ec2);
        ڪنجي.append(buf, ec2);
        s = ures_get_String_By_ڪنجي_With_Fallback(rb.getAlias(), ڪنجي.معلومات(), len, &ec2);
        إذا (ec2 == U_MISSING_RESOURCE_ERROR) {
            *ec = U_USING_FALLBACK_WARNING;
            ec2 = U_ZERO_ERROR;
            choice = UCURR_نشان_NAME;
        }
    }
    إذا (s == خالي_اشارو) {
        ures_get_By_ڪنجي(rb.getAlias(), CURRENCIES, rb.getAlias(), &ec2);
        ures_get_By_ڪنجي_With_Fallback(rb.getAlias(), buf, rb.getAlias(), &ec2);
        s = ures_get_String_By_اشاريو(rb.getAlias(), choice, len, &ec2);
    }

    // If we've succeeded we're done.  Otherwise, try to fallback.
    // If that fails (because we are already at root) then exit.
    إذا (U_SUCCESS(ec2)) {
        إذا (ec2 == U_USING_DEFAULT_WARNING
            || (ec2 == U_USING_FALLBACK_WARNING && *ec != U_USING_DEFAULT_WARNING)) {
            *ec = ec2;
        }
    }

    // We no longer support choice format data in names.  Data should not contain
    // choice patterns.
    إذا (isChoiceFormat != خالي_اشارو) {
        *isChoiceFormat = خطأ;
    }
    إذا (U_SUCCESS(ec2)) {
        U_ASSERT(s != خالي_اشارو);
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
                    ثابت حرف* plural_ڳڻپ,
                    int32_t* len, // fillin
                    UError_ڪوڊ* ec) {
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

    إذا (U_FAILURE(*ec)) {
        أرجع خالي_اشارو;
    }

    // Use a separate UErrorCode here that does not propagate out of
    // this function.
    UError_ڪوڊ ec2 = U_ZERO_ERROR;

    إذا (locale == خالي_اشارو) {
        locale = uloc_getDefault();
    }
    CharString loc = ulocimp_getName(locale, ec2);
    إذا (U_FAILURE(ec2)) {
        *ec = U_ILLEGAL_دليل_ERROR;
        أرجع خالي_اشارو;
    }

    حرف buf[ISO_CURRENCY_ڪوڊ_ڊيگهائي+1];
    my_UChars_منزل_Chars(buf, currency);

    ثابت char16_t* s = خالي_اشارو;
    ec2 = U_ZERO_ERROR;
    UResourceBundle* rb = ures_open(U_ICUDATA_CURR, loc.معلومات(), &ec2);

    rb = ures_get_By_ڪنجي(rb, CURRENCYPLURALS, rb, &ec2);

    // Fetch resource with multi-level resource inheritance fallback
    Local_UResource_Bundle_اشارو curr(ures_get_By_ڪنجي_With_Fallback(rb, buf, rb, &ec2));

    s = ures_get_String_By_ڪنجي_With_Fallback(curr.getAlias(), plural_ڳڻپ, len, &ec2);
    إذا (U_FAILURE(ec2)) {
        //  fall back to "other"
        ec2 = U_ZERO_ERROR;
        s = ures_get_String_By_ڪنجي_With_Fallback(curr.getAlias(), "other", len, &ec2);     
        إذا (U_FAILURE(ec2)) {
            // fall back to long name in Currencies
            أرجع ucurr_getName(currency, locale, UCURR_LONG_NAME, 
                                 isChoiceFormat, len, ec);
        }
    }

    // If we've succeeded we're done.  Otherwise, try to fallback.
    // If that fails (because we are already at root) then exit.
    إذا (U_SUCCESS(ec2)) {
        إذا (ec2 == U_USING_DEFAULT_WARNING
            || (ec2 == U_USING_FALLBACK_WARNING && *ec != U_USING_DEFAULT_WARNING)) {
            *ec = ec2;
        }
        U_ASSERT(s != خالي_اشارو);
        أرجع s;
    }

    // If we fail to find a match, use the ISO 4217 code
    *len = u_strlen(currency); // Should == ISO_CURRENCY_CODE_LENGTH, but maybe not...?
    *ec = U_USING_DEFAULT_WARNING;
    أرجع currency;
}


//========================================================================
// Following are structure and function for parsing currency names

#وصف_ڪريو NEED_منزل_BE_DELETED 0x1

// TODO: a better way to define this?
#وصف_ڪريو MAX_CURRENCY_NAME_LEN 100

تعريفنوع بنية {
    ثابت حرف* Iso_ڪوڊ;  //ڪنجي
    char16_t* currencyName;  //قدر
    int32_t currencyNameLen;  // value length
    int32_t flag;  // flags
} CurrencyNameStruct;


#جيڪڏهن_وصف_نه_ٿيل MIN
#وصف_ڪريو MIN(a,b) (((a)<(b)) ? (a) : (b))
#جيڪڏهن_پورو

#جيڪڏهن_وصف_نه_ٿيل MAX
#وصف_ڪريو MAX(a,b) (((a)<(b)) ? (b) : (a))
#جيڪڏهن_پورو


// Comparison function used in quick sort.
ساكن صحيح U_CALLCONV currencyNameComparator(ثابت فراغ* a, ثابت فراغ* b) {
    ثابت CurrencyNameStruct* currName_1 = static_cast<ثابت CurrencyNameStruct*>(a);
    ثابت CurrencyNameStruct* currName_2 = static_cast<ثابت CurrencyNameStruct*>(b);
    لكل (int32_t i = 0; 
         i < MIN(currName_1->currencyNameLen, currName_2->currencyNameLen);
         ++i) {
        إذا (currName_1->currencyName[i] < currName_2->currencyName[i]) {
            أرجع -1;
        }
        إذا (currName_1->currencyName[i] > currName_2->currencyName[i]) {
            أرجع 1;
        }
    }
    إذا (currName_1->currencyNameLen < currName_2->currencyNameLen) {
        أرجع -1;
    } وإلا إذا (currName_1->currencyNameLen > currName_2->currencyNameLen) {
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
get_Currency_Name_ڳڻپ(ثابت حرف* loc, int32_t* ڪل_currency_name_ڳڻپ, int32_t* ڪل_currency_نشان_ڳڻپ) {
    U_NAMESPACE_USE
    *ڪل_currency_name_ڳڻپ = 0;
    *ڪل_currency_نشان_ڳڻپ = 0;
    ثابت char16_t* s = خالي_اشارو;
    CharString locale;
    {
        UError_ڪوڊ حالت = U_ZERO_ERROR;
        locale.append(loc, حالت);
        إذا (U_FAILURE(حالت)) { أرجع; }
    }
    ثابت icu::Hashtable *currencySymbolsEquiv = getCurrSymbolsEquiv();
    لكل (;;) {
        UError_ڪوڊ ec2 = U_ZERO_ERROR;
        // TODO: ures_openDirect?
        Local_UResource_Bundle_اشارو rb(ures_open(U_ICUDATA_CURR, locale.معلومات(), &ec2));
        Local_UResource_Bundle_اشارو curr(ures_get_By_ڪنجي(rb.getAlias(), CURRENCIES, خالي_اشارو, &ec2));
        int32_t n = ures_get_سائيز(curr.getAlias());
        لكل (int32_t i=0; i<n; ++i) {
            Local_UResource_Bundle_اشارو نالا(ures_get_By_اشاريو(curr.getAlias(), i, خالي_اشارو, &ec2));
            int32_t len;
            s = ures_get_String_By_اشاريو(نالا.getAlias(), UCURR_نشان_NAME, &len, &ec2);
            ++(*ڪل_currency_نشان_ڳڻپ);  // currency symbol
            إذا (currencySymbolsEquiv != خالي_اشارو) {
                *ڪل_currency_نشان_ڳڻپ += ڳڻپ_Equivalent(*currencySymbolsEquiv, UnicodeString(سچ, s, len));
            }
            ++(*ڪل_currency_نشان_ڳڻپ); // iso code
            ++(*ڪل_currency_name_ڳڻپ); // long name
        }

        // currency plurals
        UError_ڪوڊ ec3 = U_ZERO_ERROR;
        Local_UResource_Bundle_اشارو curr_p(ures_get_By_ڪنجي(rb.getAlias(), CURRENCYPLURALS, خالي_اشارو, &ec3));
        n = ures_get_سائيز(curr_p.getAlias());
        لكل (int32_t i=0; i<n; ++i) {
            Local_UResource_Bundle_اشارو نالا(ures_get_By_اشاريو(curr_p.getAlias(), i, خالي_اشارو, &ec3));
            *ڪل_currency_name_ڳڻپ += ures_get_سائيز(نالا.getAlias());
        }

        إذا (!fallback(locale)) {
            اكسر;
        }
    }
}

ساكن char16_t*
منزل_مٿانهون_Case(ثابت char16_t* ذريعو, int32_t len, ثابت حرف* locale) {
    char16_t* dest = خالي_اشارو;
    UError_ڪوڊ ec = U_ZERO_ERROR;
    int32_t destLen = u_str_منزل_مٿانهون(dest, 0, ذريعو, len, locale, &ec);

    ec = U_ZERO_ERROR;
    dest = static_cast<char16_t*>(uprv_malloc(حجم(char16_t) * MAX(destLen, len)));
    إذا (dest == خالي_اشارو) {
        أرجع خالي_اشارو;
    }
    u_str_منزل_مٿانهون(dest, destLen, ذريعو, len, locale, &ec);
    إذا (U_FAILURE(ec)) {
        u_memcpy(dest, ذريعو, len);
    } 
    أرجع dest;
}


ساكن فراغ delete_Currency_نالا(CurrencyNameStruct* currency_نالا, int32_t count);
// Collect all available currency names associated with the given locale
// (enable fallback chain).
// Read currenc names defined in resource bundle "Currencies" and
// "CurrencyPlural", enable fallback chain.
// return the malloc-ed currency name arrays and the total number of currency
// names in the array.
ساكن فراغ
collect_Currency_نالا(ثابت حرف* locale, 
                     CurrencyNameStruct** currency_نالا, 
                     int32_t* ڪل_currency_name_ڳڻپ, 
                     CurrencyNameStruct** currencySymbols, 
                     int32_t* ڪل_currency_نشان_ڳڻپ, 
                     UError_ڪوڊ& ec) {
    إذا (U_FAILURE(ec)) {
        *currency_نالا = *currencySymbols = خالي_اشارو;
        *ڪل_currency_name_ڳڻپ = *ڪل_currency_نشان_ڳڻپ = 0;
        أرجع;
    }
    U_NAMESPACE_USE
    ثابت icu::Hashtable *currencySymbolsEquiv = getCurrSymbolsEquiv();
    // Look up the Currencies resource for the given locale.
    UError_ڪوڊ ec2 = U_ZERO_ERROR;

    إذا (locale == خالي_اشارو) {
        locale = uloc_getDefault();
    }
    CharString loc = ulocimp_getName(locale, ec2);
    إذا (U_FAILURE(ec2)) {
        ec = U_ILLEGAL_دليل_ERROR;
        *currency_نالا = *currencySymbols = خالي_اشارو;
        *ڪل_currency_name_ڳڻپ = *ڪل_currency_نشان_ڳڻپ = 0;
        أرجع;
    }

    // Get maximum currency name count first.
    get_Currency_Name_ڳڻپ(loc.معلومات(), ڪل_currency_name_ڳڻپ, ڪل_currency_نشان_ڳڻپ);

    *currency_نالا = static_cast<CurrencyNameStruct*>(
        uprv_malloc(حجم(CurrencyNameStruct) * (*ڪل_currency_name_ڳڻپ)));
    إذا(*currency_نالا == خالي_اشارو) {
        *currencySymbols = خالي_اشارو;
        *ڪل_currency_name_ڳڻپ = *ڪل_currency_نشان_ڳڻپ = 0;
        ec = U_يادگيري_ورهاست_ERROR;
        أرجع;
    }
    *currencySymbols = static_cast<CurrencyNameStruct*>(
        uprv_malloc(حجم(CurrencyNameStruct) * (*ڪل_currency_نشان_ڳڻپ)));

    إذا(*currencySymbols == خالي_اشارو) {
        uprv_free(*currency_نالا);
        *currency_نالا = خالي_اشارو;
        *ڪل_currency_name_ڳڻپ = *ڪل_currency_نشان_ڳڻپ = 0;
        ec = U_يادگيري_ورهاست_ERROR;
        أرجع;
    }

    ثابت char16_t* s = خالي_اشارو;  // currency name
    حرف* iso = خالي_اشارو;  // currency ISO code

    *ڪل_currency_name_ڳڻپ = 0;
    *ڪل_currency_نشان_ڳڻپ = 0;

    UError_ڪوڊ ec3 = U_ZERO_ERROR;
    UError_ڪوڊ ec4 = U_ZERO_ERROR;

    // Using hash to remove duplicates caused by locale fallback
    Local_UHashtable_اشارو currencyIsoCodes(uhash_open(uhash_hashChars, uhash_compareChars, خالي_اشارو, &ec3));
    Local_UHashtable_اشارو currencyPluralIsoCodes(uhash_open(uhash_hashChars, uhash_compareChars, خالي_اشارو, &ec4));
    لكل (int32_t localeLevel = 0; ; ++localeLevel) {
        ec2 = U_ZERO_ERROR;
        // TODO: ures_openDirect
        Local_UResource_Bundle_اشارو rb(ures_open(U_ICUDATA_CURR, loc.معلومات(), &ec2));
        Local_UResource_Bundle_اشارو curr(ures_get_By_ڪنجي(rb.getAlias(), CURRENCIES, خالي_اشارو, &ec2));
        int32_t n = ures_get_سائيز(curr.getAlias());
        لكل (int32_t i=0; i<n; ++i) {
            Local_UResource_Bundle_اشارو نالا(ures_get_By_اشاريو(curr.getAlias(), i, خالي_اشارو, &ec2));
            int32_t len;
            s = ures_get_String_By_اشاريو(نالا.getAlias(), UCURR_نشان_NAME, &len, &ec2);
            // TODO: uhash_put wont change key/value?
            iso = const_cast<حرف*>(ures_get_ڪنجي(نالا.getAlias()));
            إذا (localeLevel != 0 && uhash_get(currencyIsoCodes.getAlias(), iso) != خالي_اشارو) {
                استمر;
            }
            uhash_put(currencyIsoCodes.getAlias(), iso, iso, &ec3); 
            // Add currency symbol.
            (*currencySymbols)[(*ڪل_currency_نشان_ڳڻپ)++] = {iso, const_cast<char16_t*>(s), len, 0};

            // Add equivalent symbols
            إذا (currencySymbolsEquiv != خالي_اشارو) {
                UnicodeString str(سچ, s, len);
                icu::EquivIterator iter(*currencySymbolsEquiv, str);
                ثابت UnicodeString *نشان;
                طالما ((نشان = iter.next()) != خالي_اشارو) {
                    (*currencySymbols)[(*ڪل_currency_نشان_ڳڻپ)++]
                        = {iso, const_cast<char16_t*>(نشان->get_بفر()), نشان->length(), 0};
                }
            }

            // Add currency long name.
            s = ures_get_String_By_اشاريو(نالا.getAlias(), UCURR_LONG_NAME, &len, &ec2);
            char16_t* مٿانهون_Name = منزل_مٿانهون_Case(s, len, locale);
            إذا (مٿانهون_Name == خالي_اشارو) {
                ec = U_يادگيري_ورهاست_ERROR;
                اذهب error;
            }
            (*currency_نالا)[(*ڪل_currency_name_ڳڻپ)++] = {iso, مٿانهون_Name, len, NEED_منزل_BE_DELETED};

            // put (iso, 3, and iso) in to array
            // Add currency ISO code.
            char16_t* iso_ڪوڊ = static_cast<char16_t*>(uprv_malloc(حجم(char16_t) * 3));
            إذا (iso_ڪوڊ == خالي_اشارو) {
                ec = U_يادگيري_ورهاست_ERROR;
                اذهب error;
            }
            // Must convert iso[] into Unicode
            u_chars_منزل_UChars(iso, iso_ڪوڊ, 3);
            (*currencySymbols)[(*ڪل_currency_نشان_ڳڻپ)++] = {iso, iso_ڪوڊ, 3, NEED_منزل_BE_DELETED};
        }

        // currency plurals
        UError_ڪوڊ ec5 = U_ZERO_ERROR;
        Local_UResource_Bundle_اشارو curr_p(ures_get_By_ڪنجي(rb.getAlias(), CURRENCYPLURALS, خالي_اشارو, &ec5));
        n = ures_get_سائيز(curr_p.getAlias());
        لكل (int32_t i=0; i<n; ++i) {
            Local_UResource_Bundle_اشارو نالا(ures_get_By_اشاريو(curr_p.getAlias(), i, خالي_اشارو, &ec5));
            iso = const_cast<حرف*>(ures_get_ڪنجي(نالا.getAlias()));
            // Using hash to remove duplicated ISO codes in fallback chain.
            إذا (localeLevel != 0 && uhash_get(currencyPluralIsoCodes.getAlias(), iso) != خالي_اشارو) {
                استمر;
            }
            uhash_put(currencyPluralIsoCodes.getAlias(), iso, iso, &ec4);
            int32_t num = ures_get_سائيز(نالا.getAlias());
            int32_t len;
            لكل (int32_t j = 0; j < num; ++j) {
                // TODO: remove duplicates between singular name and 
                // currency long name?
                s = ures_get_String_By_اشاريو(نالا.getAlias(), j, &len, &ec5);
                char16_t* مٿانهون_Name = منزل_مٿانهون_Case(s, len, locale);
                إذا (مٿانهون_Name == خالي_اشارو) {
                    ec = U_يادگيري_ورهاست_ERROR;
                    اذهب error;
                }
                (*currency_نالا)[(*ڪل_currency_name_ڳڻپ)++] = {iso, مٿانهون_Name, len, NEED_منزل_BE_DELETED};
            }
        }

        إذا (!fallback(loc)) {
            اكسر;
        }
    }

    // quick sort the struct
    qsort(*currency_نالا, *ڪل_currency_name_ڳڻپ, 
          حجم(CurrencyNameStruct), currencyNameComparator);
    qsort(*currencySymbols, *ڪل_currency_نشان_ڳڻپ, 
          حجم(CurrencyNameStruct), currencyNameComparator);

#جيڪڏهن_وصف_ٿيل UCURR_DEBUG
    لکو("currency name count: %d\n", *ڪل_currency_name_ڳڻپ);
    لكل (int32_t اشاريو = 0; اشاريو < *ڪل_currency_name_ڳڻپ; ++اشاريو) {
        لکو("index: %d\n", اشاريو);
        لکو("iso: %s\n", (*currency_نالا)[اشاريو].Iso_ڪوڊ);
        حرف curNameBuf[1024];
        memset(curNameBuf, 0, 1024);
        u_austrncpy(curNameBuf, (*currency_نالا)[اشاريو].currencyName, (*currency_نالا)[اشاريو].currencyNameLen);
        لکو("currencyName: %s\n", curNameBuf);
        لکو("len: %d\n", (*currency_نالا)[اشاريو].currencyNameLen);
    }
    لکو("currency symbol count: %d\n", *ڪل_currency_نشان_ڳڻپ);
    لكل (int32_t اشاريو = 0; اشاريو < *ڪل_currency_نشان_ڳڻپ; ++اشاريو) {
        لکو("index: %d\n", اشاريو);
        لکو("iso: %s\n", (*currencySymbols)[اشاريو].Iso_ڪوڊ);
        حرف curNameBuf[1024];
        memset(curNameBuf, 0, 1024);
        u_austrncpy(curNameBuf, (*currencySymbols)[اشاريو].currencyName, (*currencySymbols)[اشاريو].currencyNameLen);
        لکو("currencySymbol: %s\n", curNameBuf);
        لکو("len: %d\n", (*currencySymbols)[اشاريو].currencyNameLen);
    }
#جيڪڏهن_پورو
    // fail on hashtable errors
    إذا (U_FAILURE(ec3)) {
      ec = ec3;
    } وإلا إذا (U_FAILURE(ec4)) {
      ec = ec4;
    }

  error:
    // clean up if we got error
    إذا (U_FAILURE(ec)) {
        delete_Currency_نالا(*currency_نالا, *ڪل_currency_name_ڳڻپ);
        delete_Currency_نالا(*currencySymbols, *ڪل_currency_نشان_ڳڻپ);
        *currency_نالا = *currencySymbols = خالي_اشارو;
        *ڪل_currency_name_ڳڻپ = *ڪل_currency_نشان_ڳڻپ = 0;
    }
}

// @param  currencyNames: currency names array
// @param  indexInCurrencyNames: the index of the character in currency names 
//         array against which the comparison is done
// @param  key: input text char to compare against
// @param  begin(IN/OUT): the begin index of matching range in currency names array
// @param  end(IN/OUT): the end index of matching range in currency names array.
ساكن int32_t
ٻٽي_ڳولا(ثابت CurrencyNameStruct* currency_نالا, 
             int32_t اشاريو_اندريون_Currency_نالا,
             ثابت char16_t ڪنجي,
             int32_t* begin, int32_t* پڇاڙي) {
#جيڪڏهن_وصف_ٿيل UCURR_DEBUG
    لکو("key = %x\n", ڪنجي);
#جيڪڏهن_پورو
   int32_t first = *begin;
   int32_t آخري = *پڇاڙي;
   طالما (first <= آخري) {
       int32_t mid = (first + آخري) / 2;  // compute mid point.
       إذا (اشاريو_اندريون_Currency_نالا >= currency_نالا[mid].currencyNameLen) {
           first = mid + 1;
       } وإلا {
           إذا (ڪنجي > currency_نالا[mid].currencyName[اشاريو_اندريون_Currency_نالا]) {
               first = mid + 1;
           }
           وإلا إذا (ڪنجي < currency_نالا[mid].currencyName[اشاريو_اندريون_Currency_نالا]) {
               آخري = mid - 1;
           }
           وإلا {
                // Find a match, and looking for ranges
                // Now do two more binary searches. First, on the left side for
                // the greatest L such that CurrencyNameStruct[L] < key.
                int32_t L = *begin;
                int32_t R = mid;

#جيڪڏهن_وصف_ٿيل UCURR_DEBUG
                لکو("mid = %d\n", mid);
#جيڪڏهن_پورو
                طالما (L < R) {
                    int32_t M = (L + R) / 2;
#جيڪڏهن_وصف_ٿيل UCURR_DEBUG
                    لکو("L = %d, R = %d, M = %d\n", L, R, M);
#جيڪڏهن_پورو
                    إذا (اشاريو_اندريون_Currency_نالا >= currency_نالا[M].currencyNameLen) {
                        L = M + 1;
                    } وإلا {
                        إذا (currency_نالا[M].currencyName[اشاريو_اندريون_Currency_نالا] < ڪنجي) {
                            L = M + 1;
                        } وإلا {
#جيڪڏهن_وصف_ٿيل UCURR_DEBUG
                            U_ASSERT(currency_نالا[M].currencyName[اشاريو_اندريون_Currency_نالا] == ڪنجي);
#جيڪڏهن_پورو
                            R = M;
                        }
                    }
                }
#جيڪڏهن_وصف_ٿيل UCURR_DEBUG
                U_ASSERT(L == R);
#جيڪڏهن_پورو
                *begin = L;
#جيڪڏهن_وصف_ٿيل UCURR_DEBUG
                لکو("begin = %d\n", *begin);
                U_ASSERT(currency_نالا[*begin].currencyName[اشاريو_اندريون_Currency_نالا] == ڪنجي);
#جيڪڏهن_پورو

                // Now for the second search, finding the least R such that
                // key < CurrencyNameStruct[R].
                L = mid;
                R = *پڇاڙي;
                طالما (L < R) {
                    int32_t M = (L + R) / 2;
#جيڪڏهن_وصف_ٿيل UCURR_DEBUG
                    لکو("L = %d, R = %d, M = %d\n", L, R, M);
#جيڪڏهن_پورو
                    إذا (currency_نالا[M].currencyNameLen < اشاريو_اندريون_Currency_نالا) {
                        L = M + 1;
                    } وإلا {
                        إذا (currency_نالا[M].currencyName[اشاريو_اندريون_Currency_نالا] > ڪنجي) {
                            R = M;
                        } وإلا {
#جيڪڏهن_وصف_ٿيل UCURR_DEBUG
                            U_ASSERT(currency_نالا[M].currencyName[اشاريو_اندريون_Currency_نالا] == ڪنجي);
#جيڪڏهن_پورو
                            L = M + 1;
                        }
                    }
                }
#جيڪڏهن_وصف_ٿيل UCURR_DEBUG
                U_ASSERT(L == R);
#جيڪڏهن_پورو
                إذا (currency_نالا[R].currencyName[اشاريو_اندريون_Currency_نالا] > ڪنجي) {
                    *پڇاڙي = R - 1;
                } وإلا {
                    *پڇاڙي = R;
                }
#جيڪڏهن_وصف_ٿيل UCURR_DEBUG
                لکو("end = %d\n", *پڇاڙي);
#جيڪڏهن_پورو

                // now, found the range. check whether there is exact match
                إذا (currency_نالا[*begin].currencyNameLen == اشاريو_اندريون_Currency_نالا + 1) {
                    أرجع *begin;  // find range and exact match.
                }
                أرجع -1;  // find range, but no exact match.
           }
       }
   }
   *begin = -1;
   *پڇاڙي = -1;
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
لڪيري_ڳولا(ثابت CurrencyNameStruct* currency_نالا, 
             int32_t begin, int32_t پڇاڙي,
             ثابت char16_t* متن, int32_t متن_Len,
             int32_t *partial_ميلاپ_Len,
             int32_t *max_ميلاپ_Len, int32_t* max_ميلاپ_اشاريو) {
    int32_t initial_Partial_ميلاپ_Len = *partial_ميلاپ_Len;
    لكل (int32_t اشاريو = begin; اشاريو <= پڇاڙي; ++اشاريو) {
        int32_t len = currency_نالا[اشاريو].currencyNameLen;
        إذا (len > *max_ميلاپ_Len && len <= متن_Len &&
            uprv_memcmp(currency_نالا[اشاريو].currencyName, متن, len * حجم(char16_t)) == 0) {
            *partial_ميلاپ_Len = MAX(*partial_ميلاپ_Len, len);
            *max_ميلاپ_اشاريو = اشاريو;
            *max_ميلاپ_Len = len;
#جيڪڏهن_وصف_ٿيل UCURR_DEBUG
            لکو("maxMatchIndex = %d, maxMatchLen = %d\n",
                   *max_ميلاپ_اشاريو, *max_ميلاپ_Len);
#جيڪڏهن_پورو
        } وإلا {
            // Check for partial matches.
            لكل (int32_t i=initial_Partial_ميلاپ_Len; i<MIN(len, متن_Len); i++) {
                إذا (currency_نالا[اشاريو].currencyName[i] != متن[i]) {
                    اكسر;
                }
                *partial_ميلاپ_Len = MAX(*partial_ميلاپ_Len, i + 1);
            }
        }
    }
}

#وصف_ڪريو لڪيري_ڳولا_حد 10

// Find longest match between "text" and currency names in "currencyNames".
// @param  total_currency_count: total number of currency names in CurrencyNames.
// @param  textLen: the length of the text to be compared
// @param  maxMatchLen: passing in the computed max matching length
//                              pass out the new max  matching length
// @param  maxMatchIndex: the index in currencyName which has the longest
//                        match with input text.
ساكن فراغ
ڳولا_Currency_Name(ثابت CurrencyNameStruct* currency_نالا, 
                   int32_t ڪل_currency_ڳڻپ,
                   ثابت char16_t* متن, int32_t متن_Len,
                   int32_t *partial_ميلاپ_Len,
                   int32_t* max_ميلاپ_Len, int32_t* max_ميلاپ_اشاريو) {
    *max_ميلاپ_اشاريو = -1;
    *max_ميلاپ_Len = 0;
    int32_t ميلاپ_اشاريو = -1;
    int32_t ٻٽو_ڳولا_Begin = 0;
    int32_t ٻٽو_ڳولا_پڇاڙي = ڪل_currency_ڳڻپ - 1;
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
    لكل (int32_t اشاريو = 0; اشاريو < متن_Len; ++اشاريو) {
        // matchIndex saves the one with exact match till the current point.
        // [binarySearchBegin, binarySearchEnd] saves the matching range.
        ميلاپ_اشاريو = ٻٽي_ڳولا(currency_نالا, اشاريو,
                                  متن[اشاريو],
                                  &ٻٽو_ڳولا_Begin, &ٻٽو_ڳولا_پڇاڙي);
        إذا (ٻٽو_ڳولا_Begin == -1) { // did not find the range
            اكسر;
        }
        *partial_ميلاپ_Len = MAX(*partial_ميلاپ_Len, اشاريو + 1);
        إذا (ميلاپ_اشاريو != -1) { 
            // find an exact match for text from text[0] to text[index] 
            // in currencyNames array.
            *max_ميلاپ_Len = اشاريو + 1;
            *max_ميلاپ_اشاريو = ميلاپ_اشاريو;
        }
        إذا (ٻٽو_ڳولا_پڇاڙي - ٻٽو_ڳولا_Begin < لڪيري_ڳولا_حد) {
            // linear search if within threshold.
            لڪيري_ڳولا(currency_نالا, ٻٽو_ڳولا_Begin, ٻٽو_ڳولا_پڇاڙي,
                         متن, متن_Len,
                         partial_ميلاپ_Len,
                         max_ميلاپ_Len, max_ميلاپ_اشاريو);
            اكسر;
        }
    }
}

//========================= currency name cache =====================
تعريفنوع بنية {
    حرف locale[ULOC_FULLNAME_صلاحيت];  //key
    // currency names, case insensitive
    CurrencyNameStruct* currency_نالا;  //قدر
    int32_t ڪل_Currency_Name_ڳڻپ;  // currency name count
    // currency symbols and ISO code, case sensitive
    CurrencyNameStruct* currencySymbols; //قدر
    int32_t ڪل_Currency_نشان_ڳڻپ;  //ڳڻپ
    // reference count.
    // reference count is set to 1 when an entry is put to cache.
    // it increases by 1 before accessing, and decreased by 1 after accessing.
    // The entry is deleted when ref count is zero, which means 
    // the entry is replaced out of cache and no process is accessing it.
    int32_t ref_ڳڻپ;
} Currency_Name_Cache_داخلا;


#وصف_ڪريو CURRENCY_NAME_CACHE_انگ 10

// Reserve 10 cache entries.
ساكن Currency_Name_Cache_داخلا* currCache[CURRENCY_NAME_CACHE_انگ] = {خالي_اشارو};
// Using an index to indicate which entry to be replaced when cache is full.
// It is a simple round-robin replacement strategy.
ساكن int8_t current_Cache_داخلا_اشاريو = 0;

ساكن UMutex gCurrencyCacheMutex;

// Cache deletion
ساكن فراغ
delete_Currency_نالا(CurrencyNameStruct* currency_نالا, int32_t count) {
    لكل (int32_t اشاريو = 0; اشاريو < count; ++اشاريو) {
        إذا ( (currency_نالا[اشاريو].flag & NEED_منزل_BE_DELETED) ) {
            uprv_free(currency_نالا[اشاريو].currencyName);
        }
    }
    uprv_free(currency_نالا);
}


ساكن فراغ
delete_Cache_داخلا(Currency_Name_Cache_داخلا* داخلا_2) {
    delete_Currency_نالا(داخلا_2->currency_نالا, داخلا_2->ڪل_Currency_Name_ڳڻپ);
    delete_Currency_نالا(داخلا_2->currencySymbols, داخلا_2->ڪل_Currency_نشان_ڳڻپ);
    uprv_free(داخلا_2);
}


// Cache clean up
ساكن UBool U_CALLCONV
currency_cache_cleanup() {
    لكل (int32_t i = 0; i < CURRENCY_NAME_CACHE_انگ; ++i) {
        إذا (currCache[i]) {
            delete_Cache_داخلا(currCache[i]);
            currCache[i] = خالي_اشارو;
        }
    }
    أرجع سچ;
}


/**
 * Loads the currency name data from the cache, or from resource bundles if necessary.
 * The refCount is automatically incremented.  It is the caller's responsibility
 * to decrement it when done!
 */
ساكن Currency_Name_Cache_داخلا*
get_Cache_داخلا(ثابت حرف* locale, UError_ڪوڊ& ec) {

    int32_t ڪل_currency_name_ڳڻپ = 0;
    CurrencyNameStruct* currency_نالا = خالي_اشارو;
    int32_t ڪل_currency_نشان_ڳڻپ = 0;
    CurrencyNameStruct* currencySymbols = خالي_اشارو;
    Currency_Name_Cache_داخلا* cache_داخلا = خالي_اشارو;

    umtx_تالو(&gCurrencyCacheMutex);
    // in order to handle racing correctly,
    // not putting 'search' in a separate function.
    int8_t مليو = -1;
    لكل (int8_t i = 0; i < CURRENCY_NAME_CACHE_انگ; ++i) {
        إذا (currCache[i]!= خالي_اشارو &&
            uprv_strcmp(locale, currCache[i]->locale) == 0) {
            مليو = i;
            اكسر;
        }
    }
    إذا (مليو != -1) {
        cache_داخلا = currCache[مليو];
        ++(cache_داخلا->ref_ڳڻپ);
    }
    umtx_unlock(&gCurrencyCacheMutex);
    إذا (مليو == -1) {
        collect_Currency_نالا(locale, &currency_نالا, &ڪل_currency_name_ڳڻپ, &currencySymbols, &ڪل_currency_نشان_ڳڻپ, ec);
        إذا (U_FAILURE(ec)) {
            أرجع خالي_اشارو;
        }
        umtx_تالو(&gCurrencyCacheMutex);
        // check again.
        لكل (int8_t i = 0; i < CURRENCY_NAME_CACHE_انگ; ++i) {
            إذا (currCache[i]!= خالي_اشارو &&
                uprv_strcmp(locale, currCache[i]->locale) == 0) {
                مليو = i;
                اكسر;
            }
        }
        إذا (مليو == -1) {
            // insert new entry to 
            // currentCacheEntryIndex % CURRENCY_NAME_CACHE_NUM
            // and remove the existing entry 
            // currentCacheEntryIndex % CURRENCY_NAME_CACHE_NUM
            // from cache.
            cache_داخلا = currCache[current_Cache_داخلا_اشاريو];
            إذا (cache_داخلا) {
                --(cache_داخلا->ref_ڳڻپ);
                // delete if the ref count is zero
                إذا (cache_داخلا->ref_ڳڻپ == 0) {
                    delete_Cache_داخلا(cache_داخلا);
                }
            }
            cache_داخلا = static_cast<Currency_Name_Cache_داخلا*>(uprv_malloc(حجم(Currency_Name_Cache_داخلا)));
            إذا (cache_داخلا == خالي_اشارو) {
                delete_Currency_نالا(currency_نالا, ڪل_currency_name_ڳڻپ);
                delete_Currency_نالا(currencySymbols, ڪل_currency_نشان_ڳڻپ);
                ec = U_يادگيري_ورهاست_ERROR;
                أرجع خالي_اشارو;
            }
            currCache[current_Cache_داخلا_اشاريو] = cache_داخلا;
            uprv_strcpy(cache_داخلا->locale, locale);
            cache_داخلا->currency_نالا = currency_نالا;
            cache_داخلا->ڪل_Currency_Name_ڳڻپ = ڪل_currency_name_ڳڻپ;
            cache_داخلا->currencySymbols = currencySymbols;
            cache_داخلا->ڪل_Currency_نشان_ڳڻپ = ڪل_currency_نشان_ڳڻپ;
            cache_داخلا->ref_ڳڻپ = 2; // one for cache, one for reference
            current_Cache_داخلا_اشاريو = (current_Cache_داخلا_اشاريو + 1) % CURRENCY_NAME_CACHE_انگ;
            ucln_common_رجسٽر_Cleanup(UCLN_COMMON_CURRENCY, currency_cleanup);
        } وإلا {
            delete_Currency_نالا(currency_نالا, ڪل_currency_name_ڳڻپ);
            delete_Currency_نالا(currencySymbols, ڪل_currency_نشان_ڳڻپ);
            cache_داخلا = currCache[مليو];
            ++(cache_داخلا->ref_ڳڻپ);
        }
        umtx_unlock(&gCurrencyCacheMutex);
    }

    أرجع cache_داخلا;
}

ساكن فراغ release_Cache_داخلا(Currency_Name_Cache_داخلا* cache_داخلا) {
    umtx_تالو(&gCurrencyCacheMutex);
    --(cache_داخلا->ref_ڳڻپ);
    إذا (cache_داخلا->ref_ڳڻپ == 0) {  // remove
        delete_Cache_داخلا(cache_داخلا);
    }
    umtx_unlock(&gCurrencyCacheMutex);
}

U_CAPI فراغ
uprv_parseCurrency(ثابت حرف* locale,
                   ثابت icu::UnicodeString& متن,
                   icu::ParsePosition& pos,
                   int8_t type,
                   int32_t* partial_ميلاپ_Len,
                   char16_t* نتيجو,
                   UError_ڪوڊ& ec) {
    U_NAMESPACE_USE
    إذا (U_FAILURE(ec)) {
        أرجع;
    }
    Currency_Name_Cache_داخلا* cache_داخلا = get_Cache_داخلا(locale, ec);
    إذا (U_FAILURE(ec)) {
        أرجع;
    }

    int32_t ڪل_currency_name_ڳڻپ = cache_داخلا->ڪل_Currency_Name_ڳڻپ;
    CurrencyNameStruct* currency_نالا = cache_داخلا->currency_نالا;
    int32_t ڪل_currency_نشان_ڳڻپ = cache_داخلا->ڪل_Currency_نشان_ڳڻپ;
    CurrencyNameStruct* currencySymbols = cache_داخلا->currencySymbols;

    int32_t شروعات = pos.get_اشاريو();

    char16_t داخلا_متن[MAX_CURRENCY_NAME_LEN];
    char16_t مٿانهون_متن[MAX_CURRENCY_NAME_LEN];
    int32_t متن_Len = MIN(MAX_CURRENCY_NAME_LEN, متن.length() - شروعات);
    متن.extract(شروعات, متن_Len, داخلا_متن);
    UError_ڪوڊ ec1 = U_ZERO_ERROR;
    متن_Len = u_str_منزل_مٿانهون(مٿانهون_متن, MAX_CURRENCY_NAME_LEN, داخلا_متن, متن_Len, locale, &ec1);

    // Make sure partialMatchLen is initialized
    *partial_ميلاپ_Len = 0;

    int32_t max = 0;
    int32_t ميلاپ_اشاريو = -1;
    // case in-sensitive comparison against currency names
    ڳولا_Currency_Name(currency_نالا, ڪل_currency_name_ڳڻپ, 
                       مٿانهون_متن, متن_Len, partial_ميلاپ_Len, &max, &ميلاپ_اشاريو);

#جيڪڏهن_وصف_ٿيل UCURR_DEBUG
    لکو("search in names, max = %d, matchIndex = %d\n", max, ميلاپ_اشاريو);
#جيڪڏهن_پورو

    int32_t max_اندريون_نشان = 0;
    int32_t ميلاپ_اشاريو_اندريون_نشان = -1;
    إذا (type != UCURR_LONG_NAME) {  // not name only
        // case sensitive comparison against currency symbols and ISO code.
        ڳولا_Currency_Name(currencySymbols, ڪل_currency_نشان_ڳڻپ, 
                           داخلا_متن, متن_Len,
                           partial_ميلاپ_Len,
                           &max_اندريون_نشان, &ميلاپ_اشاريو_اندريون_نشان);
    }

#جيڪڏهن_وصف_ٿيل UCURR_DEBUG
    لکو("search in symbols, maxInSymbol = %d, matchIndexInSymbol = %d\n", max_اندريون_نشان, ميلاپ_اشاريو_اندريون_نشان);
    إذا(ميلاپ_اشاريو_اندريون_نشان != -1) {
      لکو("== ISO=%s\n", currencySymbols[ميلاپ_اشاريو_اندريون_نشان].Iso_ڪوڊ);
    }
#جيڪڏهن_پورو

    إذا (max >= max_اندريون_نشان && ميلاپ_اشاريو != -1) {
        u_chars_منزل_UChars(currency_نالا[ميلاپ_اشاريو].Iso_ڪوڊ, نتيجو, 4);
        pos.سيٽ_اشاريو(شروعات + max);
    } وإلا إذا (max_اندريون_نشان >= max && ميلاپ_اشاريو_اندريون_نشان != -1) {
        u_chars_منزل_UChars(currencySymbols[ميلاپ_اشاريو_اندريون_نشان].Iso_ڪوڊ, نتيجو, 4);
        pos.سيٽ_اشاريو(شروعات + max_اندريون_نشان);
    }

    // decrease reference count
    release_Cache_داخلا(cache_داخلا);
}

فراغ uprv_currencyLeads(ثابت حرف* locale, icu::Unicode_سيٽ& نتيجو, UError_ڪوڊ& ec) {
    U_NAMESPACE_USE
    إذا (U_FAILURE(ec)) {
        أرجع;
    }
    Currency_Name_Cache_داخلا* cache_داخلا = get_Cache_داخلا(locale, ec);
    إذا (U_FAILURE(ec)) {
        أرجع;
    }

    لكل (int32_t i=0; i<cache_داخلا->ڪل_Currency_نشان_ڳڻپ; i++) {
        ثابت CurrencyNameStruct& info = cache_داخلا->currencySymbols[i];
        UChar32 cp;
        U16_GET(info.currencyName, 0, 0, info.currencyNameLen, cp);
        نتيجو.جوڙيو(cp);
    }

    لكل (int32_t i=0; i<cache_داخلا->ڪل_Currency_Name_ڳڻپ; i++) {
        ثابت CurrencyNameStruct& info = cache_داخلا->currency_نالا[i];
        UChar32 cp;
        U16_GET(info.currencyName, 0, 0, info.currencyNameLen, cp);
        نتيجو.جوڙيو(cp);
    }

    // decrease reference count
    release_Cache_داخلا(cache_داخلا);
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
                           icu::UnicodeString& نتيجو, UError_ڪوڊ& ec)
{
    U_NAMESPACE_USE

    int32_t len;
    ثابت char16_t* currname = ucurr_getName(iso, loc, UCURR_نشان_NAME,
                                          خالي_اشارو /* isChoiceFormat */, &len, &ec);
    إذا (U_SUCCESS(ec)) {
        نتيجو.سيٽ_منزل(currname, len);
    }
}

U_CAPI int32_t U_EXPORT2
ucurr_getDefaultFractionDigits(ثابت char16_t* currency, UError_ڪوڊ* ec) {
    أرجع ucurr_getDefaultFractionDigitsForUsage(currency,UCURR_USAGE_STANDARD,ec);
}

U_CAPI int32_t U_EXPORT2
ucurr_getDefaultFractionDigitsForUsage(ثابت char16_t* currency, ثابت UCurrencyUsage usage, UError_ڪوڊ* ec) {
    int32_t fracDigits = 0;
    إذا (U_SUCCESS(*ec)) {
        بدل (usage) {
            حالة UCURR_USAGE_STANDARD:
                fracDigits = (ڳوليو_Meta_معلومات(currency, *ec))[0];
                اكسر;
            حالة UCURR_USAGE_CASH:
                fracDigits = (ڳوليو_Meta_معلومات(currency, *ec))[2];
                اكسر;
            افتراضي:
                *ec = U_UNSUPPORTED_ERROR;
        }
    }
    أرجع fracDigits;
}

U_CAPI مزدوج U_EXPORT2
ucurr_getRoundingIncrement(ثابت char16_t* currency, UError_ڪوڊ* ec) {
    أرجع ucurr_getRoundingIncrementForUsage(currency, UCURR_USAGE_STANDARD, ec);
}

U_CAPI مزدوج U_EXPORT2
ucurr_getRoundingIncrementForUsage(ثابت char16_t* currency, ثابت UCurrencyUsage usage, UError_ڪوڊ* ec) {
    مزدوج نتيجو = 0.0;

    ثابت int32_t *معلومات = ڳوليو_Meta_معلومات(currency, *ec);
    إذا (U_SUCCESS(*ec)) {
        int32_t fracDigits;
        int32_t increment;
        بدل (usage) {
            حالة UCURR_USAGE_STANDARD:
                fracDigits = معلومات[0];
                increment = معلومات[1];
                اكسر;
            حالة UCURR_USAGE_CASH:
                fracDigits = معلومات[2];
                increment = معلومات[3];
                اكسر;
            افتراضي:
                *ec = U_UNSUPPORTED_ERROR;
                أرجع نتيجو;
        }

        // If the meta data is invalid, return 0.0
        إذا (fracDigits < 0 || fracDigits > MAX_POW10) {
            *ec = U_INVALID_FORMAT_ERROR;
        } وإلا {
            // A rounding value of 0 or 1 indicates no rounding.
            إذا (increment >= 2) {
                // Return (increment) / 10^(fracDigits).  The only actual rounding data,
                // as of this writing, is CHF { 2, 5 }.
                نتيجو = مزدوج(increment) / POW10[fracDigits];
            }
        }
    }

    أرجع نتيجو;
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
    { خالي_اشارو, 0 } // Leave here to denote the end of the list.
};

#وصف_ڪريو UCURR_MATCHES_BITMASK(variable, type_منزل_ميلاپ) \
    ((type_منزل_ميلاپ) == UCURR_ALL || ((variable) & (type_منزل_ميلاپ)) == (type_منزل_ميلاپ))

ساكن int32_t U_CALLCONV
ucurr_ڳڻپ_Currency_List(UEnumeration *enumerator, UError_ڪوڊ * /*pErrorCode*/) {
    UCurrencyContext *myContext = (UCurrencyContext *)(enumerator->context);
    uint32_t currType = myContext->currType;
    int32_t count = 0;

    /* Count the number of items matching the type we are looking for. */
    لكل (int32_t idx = 0; gCurrencyList[idx].currency != خالي_اشارو; idx++) {
        إذا (UCURR_MATCHES_BITMASK(gCurrencyList[idx].currType, currType)) {
            count++;
        }
    }
    أرجع count;
}

ساكن ثابت حرف* U_CALLCONV
ucurr_ايندڙ_Currency_List(UEnumeration *enumerator,
                        int32_t* نتيجو_ڊيگهائي,
                        UError_ڪوڊ * /*pErrorCode*/)
{
    UCurrencyContext *myContext = (UCurrencyContext *)(enumerator->context);

    /* Find the next in the list that matches the type we are looking for. */
    طالما (myContext->listIdx < UPRV_LENGTHOF(gCurrencyList)-1) {
        ثابت بنية CurrencyList *curr_شي = &gCurrencyList[myContext->listIdx++];
        إذا (UCURR_MATCHES_BITMASK(curr_شي->currType, myContext->currType))
        {
            إذا (نتيجو_ڊيگهائي) {
                *نتيجو_ڊيگهائي = 3; /* Currency codes are only 3 chars long */
            }
            أرجع curr_شي->currency;
        }
    }
    /* We enumerated too far. */
    إذا (نتيجو_ڊيگهائي) {
        *نتيجو_ڊيگهائي = 0;
    }
    أرجع خالي_اشارو;
}

ساكن فراغ U_CALLCONV
ucurr_resetCurrencyList(UEnumeration *enumerator, UError_ڪوڊ * /*pErrorCode*/) {
    ((UCurrencyContext *)(enumerator->context))->listIdx = 0;
}

ساكن فراغ U_CALLCONV
ucurr_closeCurrencyList(UEnumeration *enumerator) {
    uprv_free(enumerator->context);
    uprv_free(enumerator);
}

ساكن فراغ U_CALLCONV
ucurr_createCurrencyList(UHashtable *isoCodes, UError_ڪوڊ* حالت){
    UError_ڪوڊ local_حالت = U_ZERO_ERROR;

    // Look up the CurrencyMap element in the root bundle.
    UResourceBundle *rb = ures_open_سڌو(U_ICUDATA_CURR, CURRENCY_معلومات, &local_حالت);
    Local_UResource_Bundle_اشارو currencyMapArray(ures_get_By_ڪنجي(rb, CURRENCY_MAP, rb, &local_حالت));

    إذا (U_SUCCESS(local_حالت)) {
        // process each entry in currency map
        لكل (int32_t i=0; i<ures_get_سائيز(currencyMapArray.getAlias()); i++) {
            // get the currency resource
            Local_UResource_Bundle_اشارو currencyArray(ures_get_By_اشاريو(currencyMapArray.getAlias(), i, خالي_اشارو, &local_حالت));
            // process each currency
            إذا (U_SUCCESS(local_حالت)) {
                لكل (int32_t j=0; j<ures_get_سائيز(currencyArray.getAlias()); j++) {
                    // get the currency resource
                    Local_UResource_Bundle_اشارو currencyRes(ures_get_By_اشاريو(currencyArray.getAlias(), j, خالي_اشارو, &local_حالت));
                    Iso_ڪوڊ_داخلا *داخلا_2 = (Iso_ڪوڊ_داخلا*)uprv_malloc(حجم(Iso_ڪوڊ_داخلا));
                    إذا (داخلا_2 == خالي_اشارو) {
                        *حالت = U_يادگيري_ورهاست_ERROR;
                        أرجع;
                    }

                    // get the ISO code
                    int32_t iso_ڊيگهائي = 0;
                    Local_UResource_Bundle_اشارو idRes(ures_get_By_ڪنجي(currencyRes.getAlias(), "id", خالي_اشارو, &local_حالت));
                    إذا (idRes.isNull()) {
                        استمر;
                    }
                    ثابت char16_t *iso_ڪوڊ = ures_getString(idRes.getAlias(), &iso_ڊيگهائي, &local_حالت);

                    // get from date
                    UDate fromDate = U_DATE_MIN;
                    Local_UResource_Bundle_اشارو fromRes(ures_get_By_ڪنجي(currencyRes.getAlias(), "from", خالي_اشارو, &local_حالت));

                    إذا (U_SUCCESS(local_حالت)) {
                        int32_t from_ڊيگهائي = 0;
                        ثابت int32_t *fromArray = ures_get_Int_ويڪٽر(fromRes.getAlias(), &from_ڊيگهائي, &local_حالت);
                        int64_t currDate64 = ((uint64_t)fromArray[0]) << 32;
                        currDate64 |= ((int64_t)fromArray[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                        fromDate = (UDate)currDate64;
                    }

                    // get to date
                    UDate منزل_Date = U_DATE_MAX;
                    local_حالت = U_ZERO_ERROR;
                    Local_UResource_Bundle_اشارو منزل_Res(ures_get_By_ڪنجي(currencyRes.getAlias(), "to", خالي_اشارو, &local_حالت));

                    إذا (U_SUCCESS(local_حالت)) {
                        int32_t منزل_ڊيگهائي = 0;
                        ثابت int32_t *منزل_Array = ures_get_Int_ويڪٽر(منزل_Res.getAlias(), &منزل_ڊيگهائي, &local_حالت);
                        int64_t currDate64 = (uint64_t)منزل_Array[0] << 32;
                        currDate64 |= ((int64_t)منزل_Array[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                        منزل_Date = (UDate)currDate64;
                    }

                    داخلا_2->iso_ڪوڊ = iso_ڪوڊ;
                    داخلا_2->from = fromDate;
                    داخلا_2->منزل = منزل_Date;

                    local_حالت = U_ZERO_ERROR;
                    uhash_put(isoCodes, (char16_t *)iso_ڪوڊ, داخلا_2, &local_حالت);
                }
            } وإلا {
                *حالت = local_حالت;
            }
        }
    } وإلا {
        *حالت = local_حالت;
    }
}

ساكن ثابت UEnumeration gEnumCurrencyList = {
    خالي_اشارو,
    خالي_اشارو,
    ucurr_closeCurrencyList,
    ucurr_ڳڻپ_Currency_List,
    uenum_unextDefault,
    ucurr_ايندڙ_Currency_List,
    ucurr_resetCurrencyList
};
U_CDECL_پڇاڙي


ساكن فراغ U_CALLCONV initIsoCodes(UError_ڪوڊ &حالت) {
    U_ASSERT(gIsoCodes == خالي_اشارو);
    ucln_common_رجسٽر_Cleanup(UCLN_COMMON_CURRENCY, currency_cleanup);

    Local_UHashtable_اشارو isoCodes(uhash_open(uhash_hashUChars, uhash_compareUChars, خالي_اشارو, &حالت));
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    uhash_سيٽ_قدر_Deleter(isoCodes.getAlias(), delete_Iso_ڪوڊ_داخلا);

    ucurr_createCurrencyList(isoCodes.getAlias(), &حالت);
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    gIsoCodes = isoCodes.orphan();  // Note: gIsoCodes is const. Once set up here it is never altered,
                                    //       and read only access is safe without synchronization.
}

ساكن فراغ populateCurrSymbolsEquiv(icu::Hashtable *hash, UError_ڪوڊ &حالت) {
    إذا (U_FAILURE(حالت)) { أرجع; }
    لكل (ثابت خودڪار& داخلا_2 : unisets::kCurrencyEntries) {
        UnicodeString exemplar(داخلا_2.exemplar);
        ثابت Unicode_سيٽ* سيٽ = unisets::get(داخلا_2.ڪنجي);
        إذا (سيٽ == خالي_اشارو) { أرجع; }
        Unicode_سيٽ_Iterator it(*سيٽ);
        طالما (it.next()) {
            UnicodeString value = it.getString();
            إذا (value == exemplar) {
                // No need to mark the exemplar character as an equivalent
                استمر;
            }
            makeEquivalent(exemplar, value, hash, حالت);
            إذا (U_FAILURE(حالت)) { أرجع; }
        }
    }
}

ساكن فراغ U_CALLCONV initCurrSymbolsEquiv() {
    U_ASSERT(gCurrSymbolsEquiv == خالي_اشارو);
    UError_ڪوڊ حالت = U_ZERO_ERROR;
    ucln_common_رجسٽر_Cleanup(UCLN_COMMON_CURRENCY, currency_cleanup);
    icu::Hashtable *عارضي = جديد icu::Hashtable(حالت);
    إذا (عارضي == خالي_اشارو) {
        أرجع;
    }
    إذا (U_FAILURE(حالت)) {
        ميٽيو عارضي;
        أرجع;
    }
    عارضي->سيٽ_قدر_Deleter(deleteUnicode);
    populateCurrSymbolsEquiv(عارضي, حالت);
    إذا (U_FAILURE(حالت)) {
        ميٽيو عارضي;
        أرجع;
    }
    gCurrSymbolsEquiv = عارضي;
}

U_CAPI UBool U_EXPORT2
ucurr_is_موجود(ثابت char16_t* iso_ڪوڊ, UDate from, UDate منزل, UError_ڪوڊ* e_Error_ڪوڊ) {
    umtx_initOnce(gIsoCodesInitOnce, &initIsoCodes, *e_Error_ڪوڊ);
    إذا (U_FAILURE(*e_Error_ڪوڊ)) {
        أرجع خطأ;
    }

    Iso_ڪوڊ_داخلا* نتيجو = (Iso_ڪوڊ_داخلا *) uhash_get(gIsoCodes, iso_ڪوڊ);
    إذا (نتيجو == خالي_اشارو) {
        أرجع خطأ;
    } وإلا إذا (from > منزل) {
        *e_Error_ڪوڊ = U_ILLEGAL_دليل_ERROR;
        أرجع خطأ;
    } وإلا إذا  ((from > نتيجو->منزل) || (منزل < نتيجو->from)) {
        أرجع خطأ;
    }
    أرجع سچ;
}

ساكن ثابت icu::Hashtable* getCurrSymbolsEquiv() {
    umtx_initOnce(gCurrSymbolsEquivInitOnce, &initCurrSymbolsEquiv);
    أرجع gCurrSymbolsEquiv;
}

U_CAPI UEnumeration * U_EXPORT2
ucurr_openISOCurrencies(uint32_t currType, UError_ڪوڊ *p_Error_ڪوڊ) {
    UEnumeration *myEnum = خالي_اشارو;
    UCurrencyContext *myContext;

    myEnum = (UEnumeration*)uprv_malloc(حجم(UEnumeration));
    إذا (myEnum == خالي_اشارو) {
        *p_Error_ڪوڊ = U_يادگيري_ورهاست_ERROR;
        أرجع خالي_اشارو;
    }
    uprv_memcpy(myEnum, &gEnumCurrencyList, حجم(UEnumeration));
    myContext = (UCurrencyContext*)uprv_malloc(حجم(UCurrencyContext));
    إذا (myContext == خالي_اشارو) {
        *p_Error_ڪوڊ = U_يادگيري_ورهاست_ERROR;
        uprv_free(myEnum);
        أرجع خالي_اشارو;
    }
    myContext->currType = currType;
    myContext->listIdx = 0;
    myEnum->context = myContext;
    أرجع myEnum;
}

U_CAPI int32_t U_EXPORT2
ucurr_ڳڻپ_Currencies(ثابت حرف* locale, 
                 UDate date, 
                 UError_ڪوڊ* ec)
{
    int32_t curr_ڳڻپ = 0;

    إذا (ec != خالي_اشارو && U_SUCCESS(*ec)) 
    {
        // local variables
        UError_ڪوڊ local_حالت = U_ZERO_ERROR;

        // get country or country_variant in `id'
        CharString id = idForLocale(locale, ec);

        إذا (U_FAILURE(*ec))
        {
            أرجع 0;
        }

        // Remove variants, which is only needed for registration.
        حرف *idDelim = strchr(id.معلومات(), VAR_DELIM);
        إذا (idDelim)
        {
            id.truncate(idDelim - id.معلومات());
        }

        // Look up the CurrencyMap element in the root bundle.
        UResourceBundle *rb = ures_open_سڌو(U_ICUDATA_CURR, CURRENCY_معلومات, &local_حالت);
        UResourceBundle *cm = ures_get_By_ڪنجي(rb, CURRENCY_MAP, rb, &local_حالت);

        // Using the id derived from the local, get the currency data
        Local_UResource_Bundle_اشارو countryArray(ures_get_By_ڪنجي(rb, id.معلومات(), cm, &local_حالت));

        // process each currency to see which one is valid for the given date
        إذا (U_SUCCESS(local_حالت))
        {
            لكل (int32_t i=0; i<ures_get_سائيز(countryArray.getAlias()); i++)
            {
                // get the currency resource
                Local_UResource_Bundle_اشارو currencyRes(ures_get_By_اشاريو(countryArray.getAlias(), i, خالي_اشارو, &local_حالت));

                // get the from date
                int32_t from_ڊيگهائي = 0;
                Local_UResource_Bundle_اشارو fromRes(ures_get_By_ڪنجي(currencyRes.getAlias(), "from", خالي_اشارو, &local_حالت));
                ثابت int32_t *fromArray = ures_get_Int_ويڪٽر(fromRes.getAlias(), &from_ڊيگهائي, &local_حالت);

                int64_t currDate64 = (int64_t)((uint64_t)(fromArray[0]) << 32);
                currDate64 |= ((int64_t)fromArray[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                UDate fromDate = (UDate)currDate64;

                إذا (ures_get_سائيز(currencyRes.getAlias())> 2)
                {
                    int32_t منزل_ڊيگهائي = 0;
                    Local_UResource_Bundle_اشارو منزل_Res(ures_get_By_ڪنجي(currencyRes.getAlias(), "to", خالي_اشارو, &local_حالت));
                    ثابت int32_t *منزل_Array = ures_get_Int_ويڪٽر(منزل_Res.getAlias(), &منزل_ڊيگهائي, &local_حالت);

                    currDate64 = (int64_t)منزل_Array[0] << 32;
                    currDate64 |= ((int64_t)منزل_Array[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                    UDate منزل_Date = (UDate)currDate64;

                    إذا ((fromDate <= date) && (date < منزل_Date))
                    {
                        curr_ڳڻپ++;
                    }
                }
                وإلا
                {
                    إذا (fromDate <= date)
                    {
                        curr_ڳڻپ++;
                    }
                }
            } // end For loop
        } // end if (U_SUCCESS(localStatus))

        // Check for errors
        إذا (*ec == U_ZERO_ERROR || local_حالت != U_ZERO_ERROR)
        {
            // There is nothing to fallback to. 
            // Report the failure/warning if possible.
            *ec = local_حالت;
        }

        إذا (U_SUCCESS(*ec))
        {
            // no errors
            أرجع curr_ڳڻپ;
        }
    }

    // If we got here, either error code is invalid or
    // some argument passed is no good.
    أرجع 0;
}

U_CAPI int32_t U_EXPORT2 
ucurr_forLocaleAndDate(ثابت حرف* locale, 
                UDate date, 
                int32_t اشاريو,
                char16_t* buff,
                int32_t buff_صلاحيت, 
                UError_ڪوڊ* ec)
{
    int32_t resLen = 0;
	int32_t curr_اشاريو = 0;
    ثابت char16_t* s = خالي_اشارو;

    إذا (ec != خالي_اشارو && U_SUCCESS(*ec))
    {
        // check the arguments passed
        إذا ((buff && buff_صلاحيت) || !buff_صلاحيت )
        {
            // local variables
            UError_ڪوڊ local_حالت = U_ZERO_ERROR;

            // get country or country_variant in `id'
            CharString id = idForLocale(locale, ec);
            إذا (U_FAILURE(*ec))
            {
                أرجع 0;
            }

            // Remove variants, which is only needed for registration.
            حرف *idDelim = strchr(id.معلومات(), VAR_DELIM);
            إذا (idDelim)
            {
                id.truncate(idDelim - id.معلومات());
            }

            // Look up the CurrencyMap element in the root bundle.
            UResourceBundle *rb = ures_open_سڌو(U_ICUDATA_CURR, CURRENCY_معلومات, &local_حالت);
            UResourceBundle *cm = ures_get_By_ڪنجي(rb, CURRENCY_MAP, rb, &local_حالت);

            // Using the id derived from the local, get the currency data
            Local_UResource_Bundle_اشارو countryArray(ures_get_By_ڪنجي(rb, id.معلومات(), cm, &local_حالت));

            // process each currency to see which one is valid for the given date
            منطقي ميلاپ_مليو = خطأ;
            إذا (U_SUCCESS(local_حالت))
            {
                إذا ((اشاريو <= 0) || (اشاريو> ures_get_سائيز(countryArray.getAlias())))
                {
                    // requested index is out of bounds
                    أرجع 0;
                }

                لكل (int32_t i=0; i<ures_get_سائيز(countryArray.getAlias()); i++)
                {
                    // get the currency resource
                    Local_UResource_Bundle_اشارو currencyRes(ures_get_By_اشاريو(countryArray.getAlias(), i, خالي_اشارو, &local_حالت));
                    s = ures_get_String_By_ڪنجي(currencyRes.getAlias(), "id", &resLen, &local_حالت);

                    // get the from date
                    int32_t from_ڊيگهائي = 0;
                    Local_UResource_Bundle_اشارو fromRes(ures_get_By_ڪنجي(currencyRes.getAlias(), "from", خالي_اشارو, &local_حالت));
                    ثابت int32_t *fromArray = ures_get_Int_ويڪٽر(fromRes.getAlias(), &from_ڊيگهائي, &local_حالت);

                    int64_t currDate64 = (int64_t)((uint64_t)fromArray[0] << 32);
                    currDate64 |= ((int64_t)fromArray[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                    UDate fromDate = (UDate)currDate64;

                    إذا (ures_get_سائيز(currencyRes.getAlias()) > 2)
                    {
                        int32_t منزل_ڊيگهائي = 0;
                        Local_UResource_Bundle_اشارو منزل_Res(ures_get_By_ڪنجي(currencyRes.getAlias(), "to", خالي_اشارو, &local_حالت));
                        ثابت int32_t *منزل_Array = ures_get_Int_ويڪٽر(منزل_Res.getAlias(), &منزل_ڊيگهائي, &local_حالت);

                        currDate64 = (int64_t)منزل_Array[0] << 32;
                        currDate64 |= ((int64_t)منزل_Array[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                        UDate منزل_Date = (UDate)currDate64;

                        إذا ((fromDate <= date) && (date < منزل_Date))
                        {
                            curr_اشاريو++;
                            إذا (curr_اشاريو == اشاريو)
                            {
                                ميلاپ_مليو = سچ;
                            }
                        }
                    }
                    وإلا
                    {
                        إذا (fromDate <= date)
                        {
                            curr_اشاريو++;
                            إذا (curr_اشاريو == اشاريو)
                            {
                                ميلاپ_مليو = سچ;
                            }
                        }
                    }
                    // check for loop exit
                    إذا (ميلاپ_مليو)
                    {
                        اكسر;
                    }

                } // end For loop
            }

            // Check for errors
            إذا (*ec == U_ZERO_ERROR || local_حالت != U_ZERO_ERROR)
            {
                // There is nothing to fallback to. 
                // Report the failure/warning if possible.
                *ec = local_حالت;
            }

            إذا (U_SUCCESS(*ec))
            {
                // no errors
                إذا((buff_صلاحيت> resLen) && ميلاپ_مليو)
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
            أرجع u_terminateUChars(buff, buff_صلاحيت, resLen, ec);
        }
        وإلا
        {
            // illegal argument encountered
            *ec = U_ILLEGAL_دليل_ERROR;
        }

    }

    // If we got here, either error code is invalid or
    // some argument passed is no good.
    أرجع resLen;
}

ساكن ثابت UEnumeration default_Keyword_قدرون = {
    خالي_اشارو,
    خالي_اشارو,
    ulist_close_keyword_قدرون_iterator,
    ulist_ڳڻپ_keyword_قدرون,
    uenum_unextDefault,
    ulist_ايندڙ_keyword_قدر, 
    ulist_reset_keyword_قدرون_iterator
};

U_CAPI UEnumeration *U_EXPORT2 ucurr_get_Keyword_قدرون_For_Locale(ثابت حرف *ڪنجي, ثابت حرف *locale, UBool commonly_استعمال_ٿيل, UError_ڪوڊ* حالت) {
    // Resolve region
    CharString prefRegion = ulocimp_get_Region_For_Supplemental_معلومات(locale, سچ, *حالت);

    // Read value from supplementalData
    UList *قدرون = ulist_create_خالي_جايون_List(حالت);
    UList *other_قدرون = ulist_create_خالي_جايون_List(حالت);
    UEnumeration *en = (UEnumeration *)uprv_malloc(حجم(UEnumeration));
    إذا (U_FAILURE(*حالت) || en == خالي_اشارو) {
        إذا (en == خالي_اشارو) {
            *حالت = U_يادگيري_ورهاست_ERROR;
        } وإلا {
            uprv_free(en);
        }
        ulist_deleteList(قدرون);
        ulist_deleteList(other_قدرون);
        أرجع خالي_اشارو;
    }
    يادگيري_نقل(en, &default_Keyword_قدرون, حجم(UEnumeration));
    en->context = قدرون;
    
    UResourceBundle* rb = ures_open_سڌو(U_ICUDATA_CURR, "supplementalData", حالت);
    Local_UResource_Bundle_اشارو bundle(ures_get_By_ڪنجي(rb, "CurrencyMap", rb, حالت));
    ڍير_UResource_Bundle bundlekey, regbndl, curbndl, منزل;
    
    طالما (U_SUCCESS(*حالت) && ures_has_ايندڙ(bundle.getAlias())) {
        ures_get_ايندڙ_Resource(bundle.getAlias(), bundlekey.getAlias(), حالت);
        إذا (U_FAILURE(*حالت)) {
            اكسر;
        }
        ثابت حرف *region = ures_get_ڪنجي(bundlekey.getAlias());
        UBool isPrefRegion = prefRegion == region;
        إذا (!isPrefRegion && commonly_استعمال_ٿيل) {
            // With commonlyUsed=true, we do not put
            // currencies for other regions in the
            // result list.
            استمر;
        }
        ures_get_By_ڪنجي(bundle.getAlias(), region, regbndl.getAlias(), حالت);
        إذا (U_FAILURE(*حالت)) {
            اكسر;
        }
        طالما (U_SUCCESS(*حالت) && ures_has_ايندڙ(regbndl.getAlias())) {
            ures_get_ايندڙ_Resource(regbndl.getAlias(), curbndl.getAlias(), حالت);
            إذا (ures_getType(curbndl.getAlias()) != URES_جدول) {
                // Currently, an empty ARRAY is mixed in.
                استمر;
            }
            حرف *curID = (حرف *)uprv_malloc(حجم(حرف) * ULOC_KEYWORDS_صلاحيت);
            إذا (curID == خالي_اشارو) {
                *حالت = U_يادگيري_ورهاست_ERROR;
                اكسر;
            }
            int32_t curIDLength = ULOC_KEYWORDS_صلاحيت;

#إذا U_CHARSET_FAMILY==U_ASCII_FAMILY
            ures_get_UTF8_String_By_ڪنجي(curbndl.getAlias(), "id", curID, &curIDLength, سچ, حالت);
            /* optimize - use the utf-8 string */
#وإلا
            {
                       ثابت char16_t* defString = ures_get_String_By_ڪنجي(curbndl.getAlias(), "id", &curIDLength, حالت);
                       إذا(U_SUCCESS(*حالت)) {
			   إذا(curIDLength+1 > ULOC_KEYWORDS_صلاحيت) {
				*حالت = U_بفر_حد_کان_مٿي_ERROR;
			   } وإلا {
                           	u_UChars_منزل_Chars(defString, curID, curIDLength+1);
			   }
                       }
            }
#جيڪڏهن_پورو	

            إذا (U_FAILURE(*حالت)) {
                اكسر;
            }
            UBool has_منزل = خطأ;
            ures_get_By_ڪنجي(curbndl.getAlias(), "to", منزل.getAlias(), حالت);
            إذا (U_FAILURE(*حالت)) {
                // Do nothing here...
                *حالت = U_ZERO_ERROR;
            } وإلا {
                has_منزل = سچ;
            }
            إذا (isPrefRegion && !has_منزل && !ulist_شامل_آهي_String(قدرون, curID, (int32_t)uprv_strlen(curID))) {
                // Currently active currency for the target country
                ulist_جوڙيو_شي_پڇاڙي_List(قدرون, curID, سچ, حالت);
            } وإلا إذا (!ulist_شامل_آهي_String(other_قدرون, curID, (int32_t)uprv_strlen(curID)) && !commonly_استعمال_ٿيل) {
                ulist_جوڙيو_شي_پڇاڙي_List(other_قدرون, curID, سچ, حالت);
            } وإلا {
                uprv_free(curID);
            }
        }
        
    }
    إذا (U_SUCCESS(*حالت)) {
        إذا (commonly_استعمال_ٿيل) {
            إذا (ulist_get_List_سائيز(قدرون) == 0) {
                // This could happen if no valid region is supplied in the input
                // locale. In this case, we use the CLDR's default.
                uenum_close(en);
                en = ucurr_get_Keyword_قدرون_For_Locale(ڪنجي, "und", سچ, حالت);
            }
        } وإلا {
            // Consolidate the list
            حرف *value = خالي_اشارو;
            ulist_resetList(other_قدرون);
            طالما ((value = (حرف *)ulist_get_ايندڙ(other_قدرون)) != خالي_اشارو) {
                إذا (!ulist_شامل_آهي_String(قدرون, value, (int32_t)uprv_strlen(value))) {
                    حرف *tmp_قدر = (حرف *)uprv_malloc(حجم(حرف) * ULOC_KEYWORDS_صلاحيت);
                    إذا (tmp_قدر == خالي_اشارو) {
                        *حالت = U_يادگيري_ورهاست_ERROR;
                        اكسر;
                    }
                    uprv_memcpy(tmp_قدر, value, uprv_strlen(value) + 1);
                    ulist_جوڙيو_شي_پڇاڙي_List(قدرون, tmp_قدر, سچ, حالت);
                    إذا (U_FAILURE(*حالت)) {
                        اكسر;
                    }
                }
            }
        }
        ulist_resetList((UList *)(en->context));
    } وإلا {
        ulist_deleteList(قدرون);
        uprv_free(en);
        قدرون = خالي_اشارو;
        en = خالي_اشارو;
    }
    ulist_deleteList(other_قدرون);
    أرجع en;
}


U_CAPI int32_t U_EXPORT2
ucurr_get_Numeric_ڪوڊ(ثابت char16_t* currency) {
    int32_t ڪوڊ = 0;
    إذا (currency && u_strlen(currency) == ISO_CURRENCY_ڪوڊ_ڊيگهائي) {
        UError_ڪوڊ حالت = U_ZERO_ERROR;

        UResourceBundle* bundle = ures_open_سڌو(خالي_اشارو, "currencyNumericCodes", &حالت);
        Local_UResource_Bundle_اشارو ڪوڊ_Map(ures_get_By_ڪنجي(bundle, "codeMap", bundle, &حالت));
        إذا (U_SUCCESS(حالت)) {
            حرف alpha_ڪوڊ[ISO_CURRENCY_ڪوڊ_ڊيگهائي+1];
            my_UChars_منزل_Chars(alpha_ڪوڊ, currency);
            T_CString_منزل_مٿانهون_Case(alpha_ڪوڊ);
            ures_get_By_ڪنجي(ڪوڊ_Map.getAlias(), alpha_ڪوڊ, ڪوڊ_Map.getAlias(), &حالت);
            صحيح tmp_ڪوڊ = ures_getInt(ڪوڊ_Map.getAlias(), &حالت);
            إذا (U_SUCCESS(حالت)) {
                ڪوڊ = tmp_ڪوڊ;
            }
        }
    }
    أرجع ڪوڊ;
}
#جيڪڏهن_پورو /* #if !UCONFIG_NO_FORMATTING */

//eof
