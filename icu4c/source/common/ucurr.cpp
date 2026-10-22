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

ورتوں نطاق icu;

//#define UCURR_DEBUG_EQUIV 1
#جے_تعریف_شدہ UCURR_DEBUG_EQUIV
#ضمن "stdio.h"
#جے_انت
//#define UCURR_DEBUG 1
#جے_تعریف_شدہ UCURR_DEBUG
#ضمن "stdio.h"
#جے_انت

تعريفنوع بنية Iso_کوڈ_اندراج {
    ثابت char16_t *iso_کوڈ; /* const because it's a reference to a resource bundle string. */
    UDate from;
    UDate منزل;
} Iso_کوڈ_اندراج;

//------------------------------------------------------------
// Constants

// Default currency meta data of last resort.  We try to use the
// defaults encoded in the meta data resource bundle.  If there is a
// configuration/build error and these are not available, we use these
// hard-coded defaults (which should be identical).
ساكن ثابت int32_t آخری_RESORT_انکڑے[] = { 2, 0, 2, 0 };

// POW10[i] = 10^i, i=0..MAX_POW10
ساكن ثابت int32_t POW10[] = { 1, 10, 100, 1000, 10000, 100000,
                                 1000000, 10000000, 100000000, 1000000000 };

ساكن ثابت int32_t MAX_POW10 = UPRV_LENGTHOF(POW10) - 1;

#تعریف_کرو ISO_CURRENCY_کوڈ_لمائی 3

//------------------------------------------------------------
// Resource tags
//

ساكن ثابت حرف CURRENCY_انکڑے[] = "supplementalData";
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
ساكن ثابت UHashtable* gIsoCodes = خالی_سوچک;
ساكن icu::UInitOnce gIsoCodesInitOnce {};

// Currency symbol equivalances
ساكن ثابت icu::Hashtable* gCurrSymbolsEquiv = خالی_سوچک;
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
        شروع_2 = _current = &s;
    }
    مضمن ~EquivIterator() { }

    // next returns the next equivalent string or nullptr if there are no more.
    // If s has no equivalent strings, next returns nullptr on the first call.
    ثابت icu::UnicodeString *next();
خاص:
    ثابت icu::Hashtable& _hash;
    ثابت icu::UnicodeString* شروع_2;
    ثابت icu::UnicodeString* _current;
};

ثابت icu::UnicodeString *
EquivIterator::next() {
    ثابت icu::UnicodeString* اگلا_2 = static_cast<ثابت icu::UnicodeString*>(_hash.get(*_current));
    إذا (اگلا_2 == خالی_سوچک) {
        U_ASSERT(_current == شروع_2);
        أرجع خالی_سوچک;
    }
    إذا (*اگلا_2 == *شروع_2) {
        أرجع خالی_سوچک;
    }
    _current = اگلا_2;
    أرجع اگلا_2;
}

U_NAMESPACE_سرا

// makeEquivalent makes lhs and rhs equivalent by updating the equivalence
// relations in hash accordingly.
ساكن فراغ makeEquivalent(
    ثابت icu::UnicodeString &lhs,
    ثابت icu::UnicodeString &rhs,
    icu::Hashtable* hash, UError_کوڈ &حالت) {
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    إذا (lhs == rhs) {
        // already equivalent
        أرجع;
    }
    icu::EquivIterator بایاں_Iter(*hash, lhs);
    icu::EquivIterator دایاں_Iter(*hash, rhs);
    ثابت icu::UnicodeString *پہلا_بایاں = بایاں_Iter.next();
    ثابت icu::UnicodeString *پہلا_دایاں = دایاں_Iter.next();
    ثابت icu::UnicodeString *اگلا_بایاں = پہلا_بایاں;
    ثابت icu::UnicodeString *اگلا_دایاں = پہلا_دایاں;
    طالما (اگلا_بایاں != خالی_سوچک && اگلا_دایاں != خالی_سوچک) {
        إذا (*اگلا_بایاں == rhs || *اگلا_دایاں == lhs) {
            // Already equivalent
            أرجع;
        }
        اگلا_بایاں = بایاں_Iter.next();
        اگلا_دایاں = دایاں_Iter.next();
    }
    // Not equivalent. Must join.
    icu::UnicodeString *new_پہلا_بایاں;
    icu::UnicodeString *new_پہلا_دایاں;
    إذا (پہلا_دایاں == خالی_سوچک && پہلا_بایاں == خالی_سوچک) {
        // Neither lhs or rhs belong to an equivalence circle, so we form
        // a new equivalnce circle of just lhs and rhs.
        new_پہلا_بایاں = جديد icu::UnicodeString(rhs);
        new_پہلا_دایاں = جديد icu::UnicodeString(lhs);
    } وإلا إذا (پہلا_دایاں == خالی_سوچک) {
        // lhs belongs to an equivalence circle, but rhs does not, so we link
        // rhs into lhs' circle.
        new_پہلا_بایاں = جديد icu::UnicodeString(rhs);
        new_پہلا_دایاں = جديد icu::UnicodeString(*پہلا_بایاں);
    } وإلا إذا (پہلا_بایاں == خالی_سوچک) {
        // rhs belongs to an equivlance circle, but lhs does not, so we link
        // lhs into rhs' circle.
        new_پہلا_بایاں = جديد icu::UnicodeString(*پہلا_دایاں);
        new_پہلا_دایاں = جديد icu::UnicodeString(lhs);
    } وإلا {
        // Both lhs and rhs belong to different equivalnce circles. We link
        // them together to form one single, larger equivalnce circle.
        new_پہلا_بایاں = جديد icu::UnicodeString(*پہلا_دایاں);
        new_پہلا_دایاں = جديد icu::UnicodeString(*پہلا_بایاں);
    }
    إذا (new_پہلا_بایاں == خالی_سوچک || new_پہلا_دایاں == خالی_سوچک) {
        مٹاؤ new_پہلا_بایاں;
        مٹاؤ new_پہلا_دایاں;
        حالت = U_حافظہ_ونڈ_ERROR;
        أرجع;
    }
    hash->put(lhs, (فراغ *) new_پہلا_بایاں, حالت);
    hash->put(rhs, (فراغ *) new_پہلا_دایاں, حالت);
}

// countEquivalent counts how many strings are equivalent to s.
// hash stores all the equivalnce relations.
// countEquivalent does not include s itself in the count.
ساكن int32_t گنتی_Equivalent(ثابت icu::Hashtable &hash, ثابت icu::UnicodeString &s) {
    int32_t نتیجہ = 0;
    icu::EquivIterator iter(hash, s);
    طالما (iter.next() != خالی_سوچک) {
        ++نتیجہ;
    }
#جے_تعریف_شدہ UCURR_DEBUG_EQUIV
 {
   حرف tmp[200];
   s.extract(0,s.length(),tmp, "UTF-8");
   لکھو("CountEquivalent('%s') = %d\n", tmp, نتیجہ);
 }
#جے_انت
    أرجع نتیجہ;
}

ساكن ثابت icu::Hashtable* getCurrSymbolsEquiv();

//------------------------------------------------------------
//کوڈ

/**
 * Cleanup callback func
 */
ساكن UBool U_CALLCONV 
isoCodes_cleanup()
{
    إذا (gIsoCodes != خالی_سوچک) {
        uhash_close(const_cast<UHashtable *>(gIsoCodes));
        gIsoCodes = خالی_سوچک;
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
    مٹاؤ const_cast<icu::Hashtable *>(gCurrSymbolsEquiv);
    gCurrSymbolsEquiv = خالی_سوچک;
    gCurrSymbolsEquivInitOnce.reset();
    أرجع سچ;
}

/**
 * Deleter for IsoCodeEntry
 */
ساكن فراغ U_CALLCONV
delete_Iso_کوڈ_اندراج(فراغ *obj) {
    Iso_کوڈ_اندراج* اندراج = static_cast<Iso_کوڈ_اندراج*>(obj);
    uprv_free(اندراج);
}

/**
 * Deleter for gCurrSymbolsEquiv.
 */
ساكن فراغ U_CALLCONV
deleteUnicode(فراغ *obj) {
    icu::UnicodeString* اندراج = static_cast<icu::UnicodeString*>(obj);
    مٹاؤ اندراج;
}

/**
 * Unfortunately, we have to convert the char16_t* currency code to char*
 * to use it as a resource key.
 */
ساكن مضمن حرف*
my_UChars_منزل_Chars(حرف* نتیجہ_دا_Len4, ثابت char16_t* currency) {
    u_UChars_منزل_Chars(currency, نتیجہ_دا_Len4, ISO_CURRENCY_کوڈ_لمائی);
    نتیجہ_دا_Len4[ISO_CURRENCY_کوڈ_لمائی] = 0;
    أرجع نتیجہ_دا_Len4;
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
لبھو_Meta_انکڑے(ثابت char16_t* currency, UError_کوڈ& ec) {

    إذا (currency == خالی_سوچک || *currency == 0) {
        إذا (U_SUCCESS(ec)) {
            ec = U_ILLEGAL_دلیل_ERROR;
        }
        أرجع آخری_RESORT_انکڑے;
    }

    // Get CurrencyMeta resource out of root locale file.  [This may
    // move out of the root locale file later; if it does, update this
    // code.]
    UResourceBundle* currency_انکڑے = ures_open_سدھا(U_ICUDATA_CURR, CURRENCY_انکڑے, &ec);
    Local_UResource_Bundle_اشارہ_گر currencyMeta(ures_get_By_کلید(currency_انکڑے, CURRENCY_META, currency_انکڑے, &ec));

    إذا (U_FAILURE(ec)) {
        // Config/build error; return hard-coded defaults
        أرجع آخری_RESORT_انکڑے;
    }

    // Look up our currency, or if that's not available, then DEFAULT
    حرف buf[ISO_CURRENCY_کوڈ_لمائی+1];
    UError_کوڈ ec2 = U_ZERO_ERROR; // local error code: soft failure
    Local_UResource_Bundle_اشارہ_گر rb(ures_get_By_کلید(currencyMeta.getAlias(), my_UChars_منزل_Chars(buf, currency), خالی_سوچک, &ec2));
      إذا (U_FAILURE(ec2)) {
        rb.adoptInstead(ures_get_By_کلید(currencyMeta.getAlias(),DEFAULT_META, خالی_سوچک, &ec));
        إذا (U_FAILURE(ec)) {
            // Config/build error; return hard-coded defaults
            أرجع آخری_RESORT_انکڑے;
        }
    }

    int32_t len;
    ثابت int32_t *انکڑے = ures_get_Int_سمتیہ(rb.getAlias(), &len, &ec);
    إذا (U_FAILURE(ec) || len != 4) {
        // Config/build error; return hard-coded defaults
        إذا (U_SUCCESS(ec)) {
            ec = U_INVALID_FORMAT_ERROR;
        }
        أرجع آخری_RESORT_انکڑے;
    }

    أرجع انکڑے;
}

// -------------------------------------

ساكن CharString
idForLocale(ثابت حرف* locale, UError_کوڈ* ec)
{
    أرجع ulocimp_get_Region_For_Supplemental_انکڑے(locale, خطأ, *ec);
}

// ------------------------------------------
//
// Registration
//
//-------------------------------------------

// don't use ICUService since we don't need fallback

U_CDECL_BEGIN
ساكن UBool U_CALLCONV currency_cleanup();
U_CDECL_سرا

#إذا !UCONFIG_NO_SERVICE
بنية CReg;

ساكن UMutex g_CReg_جندرا;
ساكن CReg* g_CReg_سر = خالی_سوچک;

بنية CReg : عام icu::UMemory {
    CReg *next;
    char16_t iso[ISO_CURRENCY_کوڈ_لمائی+1];
    حرف  id[ULOC_FULLNAME_سمرتھا];

    CReg(ثابت char16_t* _iso, ثابت حرف* _id)
        : next(خالی_سوچک)
    {
        uprv_strncpy(id, _id, حجم(id)-1);
        id[حجم(id)-1] = 0;
        u_memcpy(iso, _iso, ISO_CURRENCY_کوڈ_لمائی);
        iso[ISO_CURRENCY_کوڈ_لمائی] = 0;
    }

    ساكن UCurr_Registry_کلید reg(ثابت char16_t* _iso, ثابت حرف* _id, UError_کوڈ* حالت)
    {
        إذا (حالت && U_SUCCESS(*حالت) && _iso && _id) {
            CReg* n = جديد CReg(_iso, _id);
            إذا (n) {
                umtx_جندرا(&g_CReg_جندرا);
                إذا (!g_CReg_سر) {
                    /* register for the first time */
                    ucln_common_رجسٹر_Cleanup(UCLN_COMMON_CURRENCY, currency_cleanup);
                }
                n->next = g_CReg_سر;
                g_CReg_سر = n;
                umtx_unlock(&g_CReg_جندرا);
                أرجع n;
            }
            *حالت = U_حافظہ_ونڈ_ERROR;
        }
        أرجع خالی_سوچک;
    }

    ساكن UBool unreg(UCurr_Registry_کلید کلید) {
        UBool لبھیا = خطأ;
        umtx_جندرا(&g_CReg_جندرا);

        CReg** p = &g_CReg_سر;
        طالما (*p) {
            إذا (*p == کلید) {
                *p = ((CReg*)کلید)->next;
                مٹاؤ (CReg*)کلید;
                لبھیا = سچ;
                اكسر;
            }
            p = &((*p)->next);
        }

        umtx_unlock(&g_CReg_جندرا);
        أرجع لبھیا;
    }

    ساكن ثابت char16_t* get(ثابت حرف* id) {
        ثابت char16_t* نتیجہ = خالی_سوچک;
        umtx_جندرا(&g_CReg_جندرا);
        CReg* p = g_CReg_سر;

        /* register cleanup of the mutex */
        ucln_common_رجسٹر_Cleanup(UCLN_COMMON_CURRENCY, currency_cleanup);
        طالما (p) {
            إذا (uprv_strcmp(id, p->id) == 0) {
                نتیجہ = p->iso;
                اكسر;
            }
            p = p->next;
        }
        umtx_unlock(&g_CReg_جندرا);
        أرجع نتیجہ;
    }

    /* This doesn't need to be thread safe. It's for u_cleanup only. */
    ساكن فراغ cleanup() {
        طالما (g_CReg_سر) {
            CReg* n = g_CReg_سر;
            g_CReg_سر = g_CReg_سر->next;
            مٹاؤ n;
        }
    }
};

// -------------------------------------

U_CAPI UCurr_Registry_کلید U_EXPORT2
ucurr_رجسٹر(ثابت char16_t* iso_کوڈ, ثابت حرف* locale, UError_کوڈ *حالت)
{
    إذا (حالت && U_SUCCESS(*حالت)) {
        CharString id = idForLocale(locale, حالت);
        أرجع CReg::reg(iso_کوڈ, id.انکڑے(), حالت);
    }
    أرجع خالی_سوچک;
}

// -------------------------------------

U_CAPI UBool U_EXPORT2
ucurr_unregister(UCurr_Registry_کلید کلید, UError_کوڈ* حالت)
{
    إذا (حالت && U_SUCCESS(*حالت)) {
        أرجع CReg::unreg(کلید);
    }
    أرجع خطأ;
}
#جے_انت /* UCONFIG_NO_SERVICE */

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
#جے_انت
    /*
     * There might be some cached currency data or isoCodes data.
     */
    currency_cache_cleanup();
    isoCodes_cleanup();
    currSymbolsEquiv_cleanup();

    أرجع سچ;
}
U_CDECL_سرا

// -------------------------------------

U_CAPI int32_t U_EXPORT2
ucurr_forLocale(ثابت حرف* locale,
                char16_t* buff,
                int32_t buff_سمرتھا,
                UError_کوڈ* ec) {
    إذا (U_FAILURE(*ec)) { أرجع 0; }
    إذا (buff_سمرتھا < 0 || (buff == خالی_سوچک && buff_سمرتھا > 0)) {
        *ec = U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }

    UError_کوڈ local_حالت = U_ZERO_ERROR;
    CharString currency = ulocimp_get_Keyword_قدر(locale, "currency", local_حالت);
    int32_t resLen = currency.length();

    إذا (U_SUCCESS(local_حالت) && resLen == 3 && uprv_isInvariantString(currency.انکڑے(), resLen)) {
        إذا (resLen < buff_سمرتھا) {
            T_CString_منزل_اتلا_Case(currency.انکڑے());
            u_chars_منزل_UChars(currency.انکڑے(), buff, resLen);
        }
        أرجع u_terminateUChars(buff, buff_سمرتھا, resLen, ec);
    }

    // get country or country_variant in `id'
    CharString id = idForLocale(locale, ec);
    إذا (U_FAILURE(*ec)) {
        أرجع 0;
    }

#إذا !UCONFIG_NO_SERVICE
    ثابت char16_t* نتیجہ = CReg::get(id.انکڑے());
    إذا (نتیجہ) {
        إذا(buff_سمرتھا > u_strlen(نتیجہ)) {
            u_strcpy(buff, نتیجہ);
        }
        resLen = u_strlen(نتیجہ);
        أرجع u_terminateUChars(buff, buff_سمرتھا, resLen, ec);
    }
#جے_انت
    // Remove variants, which is only needed for registration.
    حرف *idDelim = uprv_strchr(id.انکڑے(), VAR_DELIM);
    إذا (idDelim) {
        id.truncate(idDelim - id.انکڑے());
    }

    ثابت char16_t* s = خالی_سوچک;  // Currency code from data file.
    إذا (id.is_خالی_تھاواں()) {
        // No point looking in the data for an empty string.
        // This is what we would get.
        local_حالت = U_MISSING_RESOURCE_ERROR;
    } وإلا {
        // Look up the CurrencyMap element in the root bundle.
        local_حالت = U_ZERO_ERROR;
        UResourceBundle *rb = ures_open_سدھا(U_ICUDATA_CURR, CURRENCY_انکڑے, &local_حالت);
        UResourceBundle *cm = ures_get_By_کلید(rb, CURRENCY_MAP, rb, &local_حالت);
        Local_UResource_Bundle_اشارہ_گر countryArray(ures_get_By_کلید(rb, id.انکڑے(), cm, &local_حالت));
        // https://unicode-org.atlassian.net/browse/ICU-21997
        // Prefer to use currencies that are legal tender.
        إذا (U_SUCCESS(local_حالت)) {
            int32_t array_لمائی = ures_get_ناپ(countryArray.getAlias());
            لكل (int32_t i = 0; i < array_لمائی; ++i) {
                Local_UResource_Bundle_اشارہ_گر currencyReq(
                    ures_get_By_اشاریہ(countryArray.getAlias(), i, خالی_سوچک, &local_حالت));
                // The currency is legal tender if it is *not* marked with tender{"false"}.
                UError_کوڈ tender_حالت = local_حالت;
                ثابت char16_t *tender =
                    ures_get_String_By_کلید(currencyReq.getAlias(), "tender", خالی_سوچک, &tender_حالت);
                منطقي isTender = U_FAILURE(tender_حالت) || u_strcmp(tender, u"false") != 0;
                إذا (!isTender && s != خالی_سوچک) {
                    // We already have a non-tender currency. Ignore all following non-tender ones.
                    استمر;
                }
                // Fetch the currency code.
                s = ures_get_String_By_کلید(currencyReq.getAlias(), "id", &resLen, &local_حالت);
                إذا (isTender) {
                    اكسر;
                }
            }
            إذا (U_SUCCESS(local_حالت) && s == خالی_سوچک) {
                local_حالت = U_MISSING_RESOURCE_ERROR;
            }
        }
    }

    إذا ((U_FAILURE(local_حالت)) && strchr(id.انکڑے(), '_') != خالی_سوچک) {
        // We don't know about it.  Check to see if we support the variant.
        CharString ماپا = ulocimp_get_ماپا(locale, *ec);
        *ec = U_USING_FALLBACK_WARNING;
        // TODO: Loop over the parent rather than recursing and
        // looking again for a currency keyword.
        أرجع ucurr_forLocale(ماپا.انکڑے(), buff, buff_سمرتھا, ec);
    }
    إذا (*ec == U_ZERO_ERROR || local_حالت != U_ZERO_ERROR) {
        // There is nothing to fallback to. Report the failure/warning if possible.
        *ec = local_حالت;
    }
    إذا (U_SUCCESS(*ec)) {
        إذا(buff_سمرتھا > resLen) {
            u_strcpy(buff, s);
        }
    }
    أرجع u_terminateUChars(buff, buff_سمرتھا, resLen, ec);
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
    إذا (loc.is_خالی_تھاواں()) {
        أرجع خطأ;
    }
    UError_کوڈ حالت = U_ZERO_ERROR;
    إذا (loc == "en_GB") {
        // HACK: See #13368.  We need "en_GB" to fall back to "en_001" instead of "en"
        // in order to consume the correct data strings.  This hack will be removed
        // when proper data sink loading is implemented here.
        loc.truncate(3);
        loc.append("001", حالت);
    } وإلا {
        loc = ulocimp_get_ماپا(loc.انکڑے(), حالت);
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
              UError_کوڈ* ec) {

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
        أرجع خالی_سوچک;
    }

    int32_t choice = (int32_t) nameStyle;
    إذا (choice < 0 || choice > 4) {
        *ec = U_ILLEGAL_دلیل_ERROR;
        أرجع خالی_سوچک;
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
    UError_کوڈ ec2 = U_ZERO_ERROR;

    إذا (locale == خالی_سوچک) {
        locale = uloc_getDefault();
    }
    CharString loc = ulocimp_getName(locale, ec2);
    إذا (U_FAILURE(ec2)) {
        *ec = U_ILLEGAL_دلیل_ERROR;
        أرجع خالی_سوچک;
    }

    حرف buf[ISO_CURRENCY_کوڈ_لمائی+1];
    my_UChars_منزل_Chars(buf, currency);
    
    /* Normalize the keyword value to uppercase */
    T_CString_منزل_اتلا_Case(buf);
    
    ثابت char16_t* s = خالی_سوچک;
    ec2 = U_ZERO_ERROR;
    Local_UResource_Bundle_اشارہ_گر rb(ures_open(U_ICUDATA_CURR, loc.انکڑے(), &ec2));

    إذا (nameStyle == UCURR_NARROW_علامت_NAME || nameStyle == UCURR_FORMAL_علامت_NAME || nameStyle == UCURR_VARIANT_علامت_NAME) {
        CharString کلید;
        بدل (nameStyle) {
        حالة UCURR_NARROW_علامت_NAME:
            کلید.append(CURRENCIES_NARROW, ec2);
            اكسر;
        حالة UCURR_FORMAL_علامت_NAME:
            کلید.append(CURRENCIES_FORMAL, ec2);
            اكسر;
        حالة UCURR_VARIANT_علامت_NAME:
            کلید.append(CURRENCIES_VARIANT, ec2);
            اكسر;
        افتراضي:
            *ec = U_UNSUPPORTED_ERROR;
            أرجع خالی_سوچک;
        }
        کلید.append("/", ec2);
        کلید.append(buf, ec2);
        s = ures_get_String_By_کلید_With_Fallback(rb.getAlias(), کلید.انکڑے(), len, &ec2);
        إذا (ec2 == U_MISSING_RESOURCE_ERROR) {
            *ec = U_USING_FALLBACK_WARNING;
            ec2 = U_ZERO_ERROR;
            choice = UCURR_علامت_NAME;
        }
    }
    إذا (s == خالی_سوچک) {
        ures_get_By_کلید(rb.getAlias(), CURRENCIES, rb.getAlias(), &ec2);
        ures_get_By_کلید_With_Fallback(rb.getAlias(), buf, rb.getAlias(), &ec2);
        s = ures_get_String_By_اشاریہ(rb.getAlias(), choice, len, &ec2);
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
    إذا (isChoiceFormat != خالی_سوچک) {
        *isChoiceFormat = خطأ;
    }
    إذا (U_SUCCESS(ec2)) {
        U_ASSERT(s != خالی_سوچک);
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
                    ثابت حرف* plural_گنتی,
                    int32_t* len, // fillin
                    UError_کوڈ* ec) {
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
        أرجع خالی_سوچک;
    }

    // Use a separate UErrorCode here that does not propagate out of
    // this function.
    UError_کوڈ ec2 = U_ZERO_ERROR;

    إذا (locale == خالی_سوچک) {
        locale = uloc_getDefault();
    }
    CharString loc = ulocimp_getName(locale, ec2);
    إذا (U_FAILURE(ec2)) {
        *ec = U_ILLEGAL_دلیل_ERROR;
        أرجع خالی_سوچک;
    }

    حرف buf[ISO_CURRENCY_کوڈ_لمائی+1];
    my_UChars_منزل_Chars(buf, currency);

    ثابت char16_t* s = خالی_سوچک;
    ec2 = U_ZERO_ERROR;
    UResourceBundle* rb = ures_open(U_ICUDATA_CURR, loc.انکڑے(), &ec2);

    rb = ures_get_By_کلید(rb, CURRENCYPLURALS, rb, &ec2);

    // Fetch resource with multi-level resource inheritance fallback
    Local_UResource_Bundle_اشارہ_گر curr(ures_get_By_کلید_With_Fallback(rb, buf, rb, &ec2));

    s = ures_get_String_By_کلید_With_Fallback(curr.getAlias(), plural_گنتی, len, &ec2);
    إذا (U_FAILURE(ec2)) {
        //  fall back to "other"
        ec2 = U_ZERO_ERROR;
        s = ures_get_String_By_کلید_With_Fallback(curr.getAlias(), "other", len, &ec2);     
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
        U_ASSERT(s != خالی_سوچک);
        أرجع s;
    }

    // If we fail to find a match, use the ISO 4217 code
    *len = u_strlen(currency); // Should == ISO_CURRENCY_CODE_LENGTH, but maybe not...?
    *ec = U_USING_DEFAULT_WARNING;
    أرجع currency;
}


//========================================================================
// Following are structure and function for parsing currency names

#تعریف_کرو NEED_منزل_BE_DELETED 0x1

// TODO: a better way to define this?
#تعریف_کرو MAX_CURRENCY_NAME_LEN 100

تعريفنوع بنية {
    ثابت حرف* Iso_کوڈ;  //کلید
    char16_t* currencyName;  //قدر
    int32_t currencyNameLen;  // value length
    int32_t flag;  // flags
} CurrencyNameStruct;


#جے_بنا_تعریف MIN
#تعریف_کرو MIN(a,b) (((a)<(b)) ? (a) : (b))
#جے_انت

#جے_بنا_تعریف MAX
#تعریف_کرو MAX(a,b) (((a)<(b)) ? (b) : (a))
#جے_انت


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
get_Currency_Name_گنتی(ثابت حرف* loc, int32_t* کل_currency_name_گنتی, int32_t* کل_currency_علامت_گنتی) {
    U_NAMESPACE_USE
    *کل_currency_name_گنتی = 0;
    *کل_currency_علامت_گنتی = 0;
    ثابت char16_t* s = خالی_سوچک;
    CharString locale;
    {
        UError_کوڈ حالت = U_ZERO_ERROR;
        locale.append(loc, حالت);
        إذا (U_FAILURE(حالت)) { أرجع; }
    }
    ثابت icu::Hashtable *currencySymbolsEquiv = getCurrSymbolsEquiv();
    لكل (;;) {
        UError_کوڈ ec2 = U_ZERO_ERROR;
        // TODO: ures_openDirect?
        Local_UResource_Bundle_اشارہ_گر rb(ures_open(U_ICUDATA_CURR, locale.انکڑے(), &ec2));
        Local_UResource_Bundle_اشارہ_گر curr(ures_get_By_کلید(rb.getAlias(), CURRENCIES, خالی_سوچک, &ec2));
        int32_t n = ures_get_ناپ(curr.getAlias());
        لكل (int32_t i=0; i<n; ++i) {
            Local_UResource_Bundle_اشارہ_گر ناں(ures_get_By_اشاریہ(curr.getAlias(), i, خالی_سوچک, &ec2));
            int32_t len;
            s = ures_get_String_By_اشاریہ(ناں.getAlias(), UCURR_علامت_NAME, &len, &ec2);
            ++(*کل_currency_علامت_گنتی);  // currency symbol
            إذا (currencySymbolsEquiv != خالی_سوچک) {
                *کل_currency_علامت_گنتی += گنتی_Equivalent(*currencySymbolsEquiv, UnicodeString(سچ, s, len));
            }
            ++(*کل_currency_علامت_گنتی); // iso code
            ++(*کل_currency_name_گنتی); // long name
        }

        // currency plurals
        UError_کوڈ ec3 = U_ZERO_ERROR;
        Local_UResource_Bundle_اشارہ_گر curr_p(ures_get_By_کلید(rb.getAlias(), CURRENCYPLURALS, خالی_سوچک, &ec3));
        n = ures_get_ناپ(curr_p.getAlias());
        لكل (int32_t i=0; i<n; ++i) {
            Local_UResource_Bundle_اشارہ_گر ناں(ures_get_By_اشاریہ(curr_p.getAlias(), i, خالی_سوچک, &ec3));
            *کل_currency_name_گنتی += ures_get_ناپ(ناں.getAlias());
        }

        إذا (!fallback(locale)) {
            اكسر;
        }
    }
}

ساكن char16_t*
منزل_اتلا_Case(ثابت char16_t* سروت, int32_t len, ثابت حرف* locale) {
    char16_t* dest = خالی_سوچک;
    UError_کوڈ ec = U_ZERO_ERROR;
    int32_t destLen = u_str_منزل_اتلا(dest, 0, سروت, len, locale, &ec);

    ec = U_ZERO_ERROR;
    dest = static_cast<char16_t*>(uprv_malloc(حجم(char16_t) * MAX(destLen, len)));
    إذا (dest == خالی_سوچک) {
        أرجع خالی_سوچک;
    }
    u_str_منزل_اتلا(dest, destLen, سروت, len, locale, &ec);
    إذا (U_FAILURE(ec)) {
        u_memcpy(dest, سروت, len);
    } 
    أرجع dest;
}


ساكن فراغ delete_Currency_ناں(CurrencyNameStruct* currency_ناں, int32_t count);
// Collect all available currency names associated with the given locale
// (enable fallback chain).
// Read currenc names defined in resource bundle "Currencies" and
// "CurrencyPlural", enable fallback chain.
// return the malloc-ed currency name arrays and the total number of currency
// names in the array.
ساكن فراغ
collect_Currency_ناں(ثابت حرف* locale, 
                     CurrencyNameStruct** currency_ناں, 
                     int32_t* کل_currency_name_گنتی, 
                     CurrencyNameStruct** currencySymbols, 
                     int32_t* کل_currency_علامت_گنتی, 
                     UError_کوڈ& ec) {
    إذا (U_FAILURE(ec)) {
        *currency_ناں = *currencySymbols = خالی_سوچک;
        *کل_currency_name_گنتی = *کل_currency_علامت_گنتی = 0;
        أرجع;
    }
    U_NAMESPACE_USE
    ثابت icu::Hashtable *currencySymbolsEquiv = getCurrSymbolsEquiv();
    // Look up the Currencies resource for the given locale.
    UError_کوڈ ec2 = U_ZERO_ERROR;

    إذا (locale == خالی_سوچک) {
        locale = uloc_getDefault();
    }
    CharString loc = ulocimp_getName(locale, ec2);
    إذا (U_FAILURE(ec2)) {
        ec = U_ILLEGAL_دلیل_ERROR;
        *currency_ناں = *currencySymbols = خالی_سوچک;
        *کل_currency_name_گنتی = *کل_currency_علامت_گنتی = 0;
        أرجع;
    }

    // Get maximum currency name count first.
    get_Currency_Name_گنتی(loc.انکڑے(), کل_currency_name_گنتی, کل_currency_علامت_گنتی);

    *currency_ناں = static_cast<CurrencyNameStruct*>(
        uprv_malloc(حجم(CurrencyNameStruct) * (*کل_currency_name_گنتی)));
    إذا(*currency_ناں == خالی_سوچک) {
        *currencySymbols = خالی_سوچک;
        *کل_currency_name_گنتی = *کل_currency_علامت_گنتی = 0;
        ec = U_حافظہ_ونڈ_ERROR;
        أرجع;
    }
    *currencySymbols = static_cast<CurrencyNameStruct*>(
        uprv_malloc(حجم(CurrencyNameStruct) * (*کل_currency_علامت_گنتی)));

    إذا(*currencySymbols == خالی_سوچک) {
        uprv_free(*currency_ناں);
        *currency_ناں = خالی_سوچک;
        *کل_currency_name_گنتی = *کل_currency_علامت_گنتی = 0;
        ec = U_حافظہ_ونڈ_ERROR;
        أرجع;
    }

    ثابت char16_t* s = خالی_سوچک;  // currency name
    حرف* iso = خالی_سوچک;  // currency ISO code

    *کل_currency_name_گنتی = 0;
    *کل_currency_علامت_گنتی = 0;

    UError_کوڈ ec3 = U_ZERO_ERROR;
    UError_کوڈ ec4 = U_ZERO_ERROR;

    // Using hash to remove duplicates caused by locale fallback
    Local_UHashtable_اشارہ_گر currencyIsoCodes(uhash_open(uhash_hashChars, uhash_compareChars, خالی_سوچک, &ec3));
    Local_UHashtable_اشارہ_گر currencyPluralIsoCodes(uhash_open(uhash_hashChars, uhash_compareChars, خالی_سوچک, &ec4));
    لكل (int32_t localeLevel = 0; ; ++localeLevel) {
        ec2 = U_ZERO_ERROR;
        // TODO: ures_openDirect
        Local_UResource_Bundle_اشارہ_گر rb(ures_open(U_ICUDATA_CURR, loc.انکڑے(), &ec2));
        Local_UResource_Bundle_اشارہ_گر curr(ures_get_By_کلید(rb.getAlias(), CURRENCIES, خالی_سوچک, &ec2));
        int32_t n = ures_get_ناپ(curr.getAlias());
        لكل (int32_t i=0; i<n; ++i) {
            Local_UResource_Bundle_اشارہ_گر ناں(ures_get_By_اشاریہ(curr.getAlias(), i, خالی_سوچک, &ec2));
            int32_t len;
            s = ures_get_String_By_اشاریہ(ناں.getAlias(), UCURR_علامت_NAME, &len, &ec2);
            // TODO: uhash_put wont change key/value?
            iso = const_cast<حرف*>(ures_get_کلید(ناں.getAlias()));
            إذا (localeLevel != 0 && uhash_get(currencyIsoCodes.getAlias(), iso) != خالی_سوچک) {
                استمر;
            }
            uhash_put(currencyIsoCodes.getAlias(), iso, iso, &ec3); 
            // Add currency symbol.
            (*currencySymbols)[(*کل_currency_علامت_گنتی)++] = {iso, const_cast<char16_t*>(s), len, 0};

            // Add equivalent symbols
            إذا (currencySymbolsEquiv != خالی_سوچک) {
                UnicodeString str(سچ, s, len);
                icu::EquivIterator iter(*currencySymbolsEquiv, str);
                ثابت UnicodeString *علامت;
                طالما ((علامت = iter.next()) != خالی_سوچک) {
                    (*currencySymbols)[(*کل_currency_علامت_گنتی)++]
                        = {iso, const_cast<char16_t*>(علامت->get_بفر()), علامت->length(), 0};
                }
            }

            // Add currency long name.
            s = ures_get_String_By_اشاریہ(ناں.getAlias(), UCURR_LONG_NAME, &len, &ec2);
            char16_t* اتلا_Name = منزل_اتلا_Case(s, len, locale);
            إذا (اتلا_Name == خالی_سوچک) {
                ec = U_حافظہ_ونڈ_ERROR;
                اذهب error;
            }
            (*currency_ناں)[(*کل_currency_name_گنتی)++] = {iso, اتلا_Name, len, NEED_منزل_BE_DELETED};

            // put (iso, 3, and iso) in to array
            // Add currency ISO code.
            char16_t* iso_کوڈ = static_cast<char16_t*>(uprv_malloc(حجم(char16_t) * 3));
            إذا (iso_کوڈ == خالی_سوچک) {
                ec = U_حافظہ_ونڈ_ERROR;
                اذهب error;
            }
            // Must convert iso[] into Unicode
            u_chars_منزل_UChars(iso, iso_کوڈ, 3);
            (*currencySymbols)[(*کل_currency_علامت_گنتی)++] = {iso, iso_کوڈ, 3, NEED_منزل_BE_DELETED};
        }

        // currency plurals
        UError_کوڈ ec5 = U_ZERO_ERROR;
        Local_UResource_Bundle_اشارہ_گر curr_p(ures_get_By_کلید(rb.getAlias(), CURRENCYPLURALS, خالی_سوچک, &ec5));
        n = ures_get_ناپ(curr_p.getAlias());
        لكل (int32_t i=0; i<n; ++i) {
            Local_UResource_Bundle_اشارہ_گر ناں(ures_get_By_اشاریہ(curr_p.getAlias(), i, خالی_سوچک, &ec5));
            iso = const_cast<حرف*>(ures_get_کلید(ناں.getAlias()));
            // Using hash to remove duplicated ISO codes in fallback chain.
            إذا (localeLevel != 0 && uhash_get(currencyPluralIsoCodes.getAlias(), iso) != خالی_سوچک) {
                استمر;
            }
            uhash_put(currencyPluralIsoCodes.getAlias(), iso, iso, &ec4);
            int32_t num = ures_get_ناپ(ناں.getAlias());
            int32_t len;
            لكل (int32_t j = 0; j < num; ++j) {
                // TODO: remove duplicates between singular name and 
                // currency long name?
                s = ures_get_String_By_اشاریہ(ناں.getAlias(), j, &len, &ec5);
                char16_t* اتلا_Name = منزل_اتلا_Case(s, len, locale);
                إذا (اتلا_Name == خالی_سوچک) {
                    ec = U_حافظہ_ونڈ_ERROR;
                    اذهب error;
                }
                (*currency_ناں)[(*کل_currency_name_گنتی)++] = {iso, اتلا_Name, len, NEED_منزل_BE_DELETED};
            }
        }

        إذا (!fallback(loc)) {
            اكسر;
        }
    }

    // quick sort the struct
    qsort(*currency_ناں, *کل_currency_name_گنتی, 
          حجم(CurrencyNameStruct), currencyNameComparator);
    qsort(*currencySymbols, *کل_currency_علامت_گنتی, 
          حجم(CurrencyNameStruct), currencyNameComparator);

#جے_تعریف_شدہ UCURR_DEBUG
    لکھو("currency name count: %d\n", *کل_currency_name_گنتی);
    لكل (int32_t اشاریہ = 0; اشاریہ < *کل_currency_name_گنتی; ++اشاریہ) {
        لکھو("index: %d\n", اشاریہ);
        لکھو("iso: %s\n", (*currency_ناں)[اشاریہ].Iso_کوڈ);
        حرف curNameBuf[1024];
        memset(curNameBuf, 0, 1024);
        u_austrncpy(curNameBuf, (*currency_ناں)[اشاریہ].currencyName, (*currency_ناں)[اشاریہ].currencyNameLen);
        لکھو("currencyName: %s\n", curNameBuf);
        لکھو("len: %d\n", (*currency_ناں)[اشاریہ].currencyNameLen);
    }
    لکھو("currency symbol count: %d\n", *کل_currency_علامت_گنتی);
    لكل (int32_t اشاریہ = 0; اشاریہ < *کل_currency_علامت_گنتی; ++اشاریہ) {
        لکھو("index: %d\n", اشاریہ);
        لکھو("iso: %s\n", (*currencySymbols)[اشاریہ].Iso_کوڈ);
        حرف curNameBuf[1024];
        memset(curNameBuf, 0, 1024);
        u_austrncpy(curNameBuf, (*currencySymbols)[اشاریہ].currencyName, (*currencySymbols)[اشاریہ].currencyNameLen);
        لکھو("currencySymbol: %s\n", curNameBuf);
        لکھو("len: %d\n", (*currencySymbols)[اشاریہ].currencyNameLen);
    }
#جے_انت
    // fail on hashtable errors
    إذا (U_FAILURE(ec3)) {
      ec = ec3;
    } وإلا إذا (U_FAILURE(ec4)) {
      ec = ec4;
    }

  error:
    // clean up if we got error
    إذا (U_FAILURE(ec)) {
        delete_Currency_ناں(*currency_ناں, *کل_currency_name_گنتی);
        delete_Currency_ناں(*currencySymbols, *کل_currency_علامت_گنتی);
        *currency_ناں = *currencySymbols = خالی_سوچک;
        *کل_currency_name_گنتی = *کل_currency_علامت_گنتی = 0;
    }
}

// @param  currencyNames: currency names array
// @param  indexInCurrencyNames: the index of the character in currency names 
//         array against which the comparison is done
// @param  key: input text char to compare against
// @param  begin(IN/OUT): the begin index of matching range in currency names array
// @param  end(IN/OUT): the end index of matching range in currency names array.
ساكن int32_t
ثنائی_تلاش(ثابت CurrencyNameStruct* currency_ناں, 
             int32_t اشاریہ_وچ_Currency_ناں,
             ثابت char16_t کلید,
             int32_t* begin, int32_t* سرا) {
#جے_تعریف_شدہ UCURR_DEBUG
    لکھو("key = %x\n", کلید);
#جے_انت
   int32_t first = *begin;
   int32_t آخری = *سرا;
   طالما (first <= آخری) {
       int32_t mid = (first + آخری) / 2;  // compute mid point.
       إذا (اشاریہ_وچ_Currency_ناں >= currency_ناں[mid].currencyNameLen) {
           first = mid + 1;
       } وإلا {
           إذا (کلید > currency_ناں[mid].currencyName[اشاریہ_وچ_Currency_ناں]) {
               first = mid + 1;
           }
           وإلا إذا (کلید < currency_ناں[mid].currencyName[اشاریہ_وچ_Currency_ناں]) {
               آخری = mid - 1;
           }
           وإلا {
                // Find a match, and looking for ranges
                // Now do two more binary searches. First, on the left side for
                // the greatest L such that CurrencyNameStruct[L] < key.
                int32_t L = *begin;
                int32_t R = mid;

#جے_تعریف_شدہ UCURR_DEBUG
                لکھو("mid = %d\n", mid);
#جے_انت
                طالما (L < R) {
                    int32_t M = (L + R) / 2;
#جے_تعریف_شدہ UCURR_DEBUG
                    لکھو("L = %d, R = %d, M = %d\n", L, R, M);
#جے_انت
                    إذا (اشاریہ_وچ_Currency_ناں >= currency_ناں[M].currencyNameLen) {
                        L = M + 1;
                    } وإلا {
                        إذا (currency_ناں[M].currencyName[اشاریہ_وچ_Currency_ناں] < کلید) {
                            L = M + 1;
                        } وإلا {
#جے_تعریف_شدہ UCURR_DEBUG
                            U_ASSERT(currency_ناں[M].currencyName[اشاریہ_وچ_Currency_ناں] == کلید);
#جے_انت
                            R = M;
                        }
                    }
                }
#جے_تعریف_شدہ UCURR_DEBUG
                U_ASSERT(L == R);
#جے_انت
                *begin = L;
#جے_تعریف_شدہ UCURR_DEBUG
                لکھو("begin = %d\n", *begin);
                U_ASSERT(currency_ناں[*begin].currencyName[اشاریہ_وچ_Currency_ناں] == کلید);
#جے_انت

                // Now for the second search, finding the least R such that
                // key < CurrencyNameStruct[R].
                L = mid;
                R = *سرا;
                طالما (L < R) {
                    int32_t M = (L + R) / 2;
#جے_تعریف_شدہ UCURR_DEBUG
                    لکھو("L = %d, R = %d, M = %d\n", L, R, M);
#جے_انت
                    إذا (currency_ناں[M].currencyNameLen < اشاریہ_وچ_Currency_ناں) {
                        L = M + 1;
                    } وإلا {
                        إذا (currency_ناں[M].currencyName[اشاریہ_وچ_Currency_ناں] > کلید) {
                            R = M;
                        } وإلا {
#جے_تعریف_شدہ UCURR_DEBUG
                            U_ASSERT(currency_ناں[M].currencyName[اشاریہ_وچ_Currency_ناں] == کلید);
#جے_انت
                            L = M + 1;
                        }
                    }
                }
#جے_تعریف_شدہ UCURR_DEBUG
                U_ASSERT(L == R);
#جے_انت
                إذا (currency_ناں[R].currencyName[اشاریہ_وچ_Currency_ناں] > کلید) {
                    *سرا = R - 1;
                } وإلا {
                    *سرا = R;
                }
#جے_تعریف_شدہ UCURR_DEBUG
                لکھو("end = %d\n", *سرا);
#جے_انت

                // now, found the range. check whether there is exact match
                إذا (currency_ناں[*begin].currencyNameLen == اشاریہ_وچ_Currency_ناں + 1) {
                    أرجع *begin;  // find range and exact match.
                }
                أرجع -1;  // find range, but no exact match.
           }
       }
   }
   *begin = -1;
   *سرا = -1;
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
خطی_تلاش(ثابت CurrencyNameStruct* currency_ناں, 
             int32_t begin, int32_t سرا,
             ثابت char16_t* متن, int32_t متن_Len,
             int32_t *partial_میل_Len,
             int32_t *max_میل_Len, int32_t* max_میل_اشاریہ) {
    int32_t initial_Partial_میل_Len = *partial_میل_Len;
    لكل (int32_t اشاریہ = begin; اشاریہ <= سرا; ++اشاریہ) {
        int32_t len = currency_ناں[اشاریہ].currencyNameLen;
        إذا (len > *max_میل_Len && len <= متن_Len &&
            uprv_memcmp(currency_ناں[اشاریہ].currencyName, متن, len * حجم(char16_t)) == 0) {
            *partial_میل_Len = MAX(*partial_میل_Len, len);
            *max_میل_اشاریہ = اشاریہ;
            *max_میل_Len = len;
#جے_تعریف_شدہ UCURR_DEBUG
            لکھو("maxMatchIndex = %d, maxMatchLen = %d\n",
                   *max_میل_اشاریہ, *max_میل_Len);
#جے_انت
        } وإلا {
            // Check for partial matches.
            لكل (int32_t i=initial_Partial_میل_Len; i<MIN(len, متن_Len); i++) {
                إذا (currency_ناں[اشاریہ].currencyName[i] != متن[i]) {
                    اكسر;
                }
                *partial_میل_Len = MAX(*partial_میل_Len, i + 1);
            }
        }
    }
}

#تعریف_کرو خطی_تلاش_حد 10

// Find longest match between "text" and currency names in "currencyNames".
// @param  total_currency_count: total number of currency names in CurrencyNames.
// @param  textLen: the length of the text to be compared
// @param  maxMatchLen: passing in the computed max matching length
//                              pass out the new max  matching length
// @param  maxMatchIndex: the index in currencyName which has the longest
//                        match with input text.
ساكن فراغ
تلاش_Currency_Name(ثابت CurrencyNameStruct* currency_ناں, 
                   int32_t کل_currency_گنتی,
                   ثابت char16_t* متن, int32_t متن_Len,
                   int32_t *partial_میل_Len,
                   int32_t* max_میل_Len, int32_t* max_میل_اشاریہ) {
    *max_میل_اشاریہ = -1;
    *max_میل_Len = 0;
    int32_t میل_اشاریہ = -1;
    int32_t ثنائی_تلاش_Begin = 0;
    int32_t ثنائی_تلاش_سرا = کل_currency_گنتی - 1;
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
    لكل (int32_t اشاریہ = 0; اشاریہ < متن_Len; ++اشاریہ) {
        // matchIndex saves the one with exact match till the current point.
        // [binarySearchBegin, binarySearchEnd] saves the matching range.
        میل_اشاریہ = ثنائی_تلاش(currency_ناں, اشاریہ,
                                  متن[اشاریہ],
                                  &ثنائی_تلاش_Begin, &ثنائی_تلاش_سرا);
        إذا (ثنائی_تلاش_Begin == -1) { // did not find the range
            اكسر;
        }
        *partial_میل_Len = MAX(*partial_میل_Len, اشاریہ + 1);
        إذا (میل_اشاریہ != -1) { 
            // find an exact match for text from text[0] to text[index] 
            // in currencyNames array.
            *max_میل_Len = اشاریہ + 1;
            *max_میل_اشاریہ = میل_اشاریہ;
        }
        إذا (ثنائی_تلاش_سرا - ثنائی_تلاش_Begin < خطی_تلاش_حد) {
            // linear search if within threshold.
            خطی_تلاش(currency_ناں, ثنائی_تلاش_Begin, ثنائی_تلاش_سرا,
                         متن, متن_Len,
                         partial_میل_Len,
                         max_میل_Len, max_میل_اشاریہ);
            اكسر;
        }
    }
}

//========================= currency name cache =====================
تعريفنوع بنية {
    حرف locale[ULOC_FULLNAME_سمرتھا];  //key
    // currency names, case insensitive
    CurrencyNameStruct* currency_ناں;  //قدر
    int32_t کل_Currency_Name_گنتی;  // currency name count
    // currency symbols and ISO code, case sensitive
    CurrencyNameStruct* currencySymbols; //قدر
    int32_t کل_Currency_علامت_گنتی;  //گنتی
    // reference count.
    // reference count is set to 1 when an entry is put to cache.
    // it increases by 1 before accessing, and decreased by 1 after accessing.
    // The entry is deleted when ref count is zero, which means 
    // the entry is replaced out of cache and no process is accessing it.
    int32_t ref_گنتی;
} Currency_Name_Cache_اندراج;


#تعریف_کرو CURRENCY_NAME_CACHE_عدد 10

// Reserve 10 cache entries.
ساكن Currency_Name_Cache_اندراج* currCache[CURRENCY_NAME_CACHE_عدد] = {خالی_سوچک};
// Using an index to indicate which entry to be replaced when cache is full.
// It is a simple round-robin replacement strategy.
ساكن int8_t current_Cache_اندراج_اشاریہ = 0;

ساكن UMutex gCurrencyCacheMutex;

// Cache deletion
ساكن فراغ
delete_Currency_ناں(CurrencyNameStruct* currency_ناں, int32_t count) {
    لكل (int32_t اشاریہ = 0; اشاریہ < count; ++اشاریہ) {
        إذا ( (currency_ناں[اشاریہ].flag & NEED_منزل_BE_DELETED) ) {
            uprv_free(currency_ناں[اشاریہ].currencyName);
        }
    }
    uprv_free(currency_ناں);
}


ساكن فراغ
delete_Cache_اندراج(Currency_Name_Cache_اندراج* اندراج) {
    delete_Currency_ناں(اندراج->currency_ناں, اندراج->کل_Currency_Name_گنتی);
    delete_Currency_ناں(اندراج->currencySymbols, اندراج->کل_Currency_علامت_گنتی);
    uprv_free(اندراج);
}


// Cache clean up
ساكن UBool U_CALLCONV
currency_cache_cleanup() {
    لكل (int32_t i = 0; i < CURRENCY_NAME_CACHE_عدد; ++i) {
        إذا (currCache[i]) {
            delete_Cache_اندراج(currCache[i]);
            currCache[i] = خالی_سوچک;
        }
    }
    أرجع سچ;
}


/**
 * Loads the currency name data from the cache, or from resource bundles if necessary.
 * The refCount is automatically incremented.  It is the caller's responsibility
 * to decrement it when done!
 */
ساكن Currency_Name_Cache_اندراج*
get_Cache_اندراج(ثابت حرف* locale, UError_کوڈ& ec) {

    int32_t کل_currency_name_گنتی = 0;
    CurrencyNameStruct* currency_ناں = خالی_سوچک;
    int32_t کل_currency_علامت_گنتی = 0;
    CurrencyNameStruct* currencySymbols = خالی_سوچک;
    Currency_Name_Cache_اندراج* cache_اندراج = خالی_سوچک;

    umtx_جندرا(&gCurrencyCacheMutex);
    // in order to handle racing correctly,
    // not putting 'search' in a separate function.
    int8_t لبھیا = -1;
    لكل (int8_t i = 0; i < CURRENCY_NAME_CACHE_عدد; ++i) {
        إذا (currCache[i]!= خالی_سوچک &&
            uprv_strcmp(locale, currCache[i]->locale) == 0) {
            لبھیا = i;
            اكسر;
        }
    }
    إذا (لبھیا != -1) {
        cache_اندراج = currCache[لبھیا];
        ++(cache_اندراج->ref_گنتی);
    }
    umtx_unlock(&gCurrencyCacheMutex);
    إذا (لبھیا == -1) {
        collect_Currency_ناں(locale, &currency_ناں, &کل_currency_name_گنتی, &currencySymbols, &کل_currency_علامت_گنتی, ec);
        إذا (U_FAILURE(ec)) {
            أرجع خالی_سوچک;
        }
        umtx_جندرا(&gCurrencyCacheMutex);
        // check again.
        لكل (int8_t i = 0; i < CURRENCY_NAME_CACHE_عدد; ++i) {
            إذا (currCache[i]!= خالی_سوچک &&
                uprv_strcmp(locale, currCache[i]->locale) == 0) {
                لبھیا = i;
                اكسر;
            }
        }
        إذا (لبھیا == -1) {
            // insert new entry to 
            // currentCacheEntryIndex % CURRENCY_NAME_CACHE_NUM
            // and remove the existing entry 
            // currentCacheEntryIndex % CURRENCY_NAME_CACHE_NUM
            // from cache.
            cache_اندراج = currCache[current_Cache_اندراج_اشاریہ];
            إذا (cache_اندراج) {
                --(cache_اندراج->ref_گنتی);
                // delete if the ref count is zero
                إذا (cache_اندراج->ref_گنتی == 0) {
                    delete_Cache_اندراج(cache_اندراج);
                }
            }
            cache_اندراج = static_cast<Currency_Name_Cache_اندراج*>(uprv_malloc(حجم(Currency_Name_Cache_اندراج)));
            إذا (cache_اندراج == خالی_سوچک) {
                delete_Currency_ناں(currency_ناں, کل_currency_name_گنتی);
                delete_Currency_ناں(currencySymbols, کل_currency_علامت_گنتی);
                ec = U_حافظہ_ونڈ_ERROR;
                أرجع خالی_سوچک;
            }
            currCache[current_Cache_اندراج_اشاریہ] = cache_اندراج;
            uprv_strcpy(cache_اندراج->locale, locale);
            cache_اندراج->currency_ناں = currency_ناں;
            cache_اندراج->کل_Currency_Name_گنتی = کل_currency_name_گنتی;
            cache_اندراج->currencySymbols = currencySymbols;
            cache_اندراج->کل_Currency_علامت_گنتی = کل_currency_علامت_گنتی;
            cache_اندراج->ref_گنتی = 2; // one for cache, one for reference
            current_Cache_اندراج_اشاریہ = (current_Cache_اندراج_اشاریہ + 1) % CURRENCY_NAME_CACHE_عدد;
            ucln_common_رجسٹر_Cleanup(UCLN_COMMON_CURRENCY, currency_cleanup);
        } وإلا {
            delete_Currency_ناں(currency_ناں, کل_currency_name_گنتی);
            delete_Currency_ناں(currencySymbols, کل_currency_علامت_گنتی);
            cache_اندراج = currCache[لبھیا];
            ++(cache_اندراج->ref_گنتی);
        }
        umtx_unlock(&gCurrencyCacheMutex);
    }

    أرجع cache_اندراج;
}

ساكن فراغ release_Cache_اندراج(Currency_Name_Cache_اندراج* cache_اندراج) {
    umtx_جندرا(&gCurrencyCacheMutex);
    --(cache_اندراج->ref_گنتی);
    إذا (cache_اندراج->ref_گنتی == 0) {  // remove
        delete_Cache_اندراج(cache_اندراج);
    }
    umtx_unlock(&gCurrencyCacheMutex);
}

U_CAPI فراغ
uprv_parseCurrency(ثابت حرف* locale,
                   ثابت icu::UnicodeString& متن,
                   icu::ParsePosition& pos,
                   int8_t type,
                   int32_t* partial_میل_Len,
                   char16_t* نتیجہ,
                   UError_کوڈ& ec) {
    U_NAMESPACE_USE
    إذا (U_FAILURE(ec)) {
        أرجع;
    }
    Currency_Name_Cache_اندراج* cache_اندراج = get_Cache_اندراج(locale, ec);
    إذا (U_FAILURE(ec)) {
        أرجع;
    }

    int32_t کل_currency_name_گنتی = cache_اندراج->کل_Currency_Name_گنتی;
    CurrencyNameStruct* currency_ناں = cache_اندراج->currency_ناں;
    int32_t کل_currency_علامت_گنتی = cache_اندراج->کل_Currency_علامت_گنتی;
    CurrencyNameStruct* currencySymbols = cache_اندراج->currencySymbols;

    int32_t شروع = pos.get_اشاریہ();

    char16_t داخلہ_متن[MAX_CURRENCY_NAME_LEN];
    char16_t اتلا_متن[MAX_CURRENCY_NAME_LEN];
    int32_t متن_Len = MIN(MAX_CURRENCY_NAME_LEN, متن.length() - شروع);
    متن.extract(شروع, متن_Len, داخلہ_متن);
    UError_کوڈ ec1 = U_ZERO_ERROR;
    متن_Len = u_str_منزل_اتلا(اتلا_متن, MAX_CURRENCY_NAME_LEN, داخلہ_متن, متن_Len, locale, &ec1);

    // Make sure partialMatchLen is initialized
    *partial_میل_Len = 0;

    int32_t max = 0;
    int32_t میل_اشاریہ = -1;
    // case in-sensitive comparison against currency names
    تلاش_Currency_Name(currency_ناں, کل_currency_name_گنتی, 
                       اتلا_متن, متن_Len, partial_میل_Len, &max, &میل_اشاریہ);

#جے_تعریف_شدہ UCURR_DEBUG
    لکھو("search in names, max = %d, matchIndex = %d\n", max, میل_اشاریہ);
#جے_انت

    int32_t max_وچ_علامت = 0;
    int32_t میل_اشاریہ_وچ_علامت = -1;
    إذا (type != UCURR_LONG_NAME) {  // not name only
        // case sensitive comparison against currency symbols and ISO code.
        تلاش_Currency_Name(currencySymbols, کل_currency_علامت_گنتی, 
                           داخلہ_متن, متن_Len,
                           partial_میل_Len,
                           &max_وچ_علامت, &میل_اشاریہ_وچ_علامت);
    }

#جے_تعریف_شدہ UCURR_DEBUG
    لکھو("search in symbols, maxInSymbol = %d, matchIndexInSymbol = %d\n", max_وچ_علامت, میل_اشاریہ_وچ_علامت);
    إذا(میل_اشاریہ_وچ_علامت != -1) {
      لکھو("== ISO=%s\n", currencySymbols[میل_اشاریہ_وچ_علامت].Iso_کوڈ);
    }
#جے_انت

    إذا (max >= max_وچ_علامت && میل_اشاریہ != -1) {
        u_chars_منزل_UChars(currency_ناں[میل_اشاریہ].Iso_کوڈ, نتیجہ, 4);
        pos.سیٹ_اشاریہ(شروع + max);
    } وإلا إذا (max_وچ_علامت >= max && میل_اشاریہ_وچ_علامت != -1) {
        u_chars_منزل_UChars(currencySymbols[میل_اشاریہ_وچ_علامت].Iso_کوڈ, نتیجہ, 4);
        pos.سیٹ_اشاریہ(شروع + max_وچ_علامت);
    }

    // decrease reference count
    release_Cache_اندراج(cache_اندراج);
}

فراغ uprv_currencyLeads(ثابت حرف* locale, icu::Unicode_سیٹ& نتیجہ, UError_کوڈ& ec) {
    U_NAMESPACE_USE
    إذا (U_FAILURE(ec)) {
        أرجع;
    }
    Currency_Name_Cache_اندراج* cache_اندراج = get_Cache_اندراج(locale, ec);
    إذا (U_FAILURE(ec)) {
        أرجع;
    }

    لكل (int32_t i=0; i<cache_اندراج->کل_Currency_علامت_گنتی; i++) {
        ثابت CurrencyNameStruct& info = cache_اندراج->currencySymbols[i];
        UChar32 cp;
        U16_GET(info.currencyName, 0, 0, info.currencyNameLen, cp);
        نتیجہ.جوڑو(cp);
    }

    لكل (int32_t i=0; i<cache_اندراج->کل_Currency_Name_گنتی; i++) {
        ثابت CurrencyNameStruct& info = cache_اندراج->currency_ناں[i];
        UChar32 cp;
        U16_GET(info.currencyName, 0, 0, info.currencyNameLen, cp);
        نتیجہ.جوڑو(cp);
    }

    // decrease reference count
    release_Cache_اندراج(cache_اندراج);
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
                           icu::UnicodeString& نتیجہ, UError_کوڈ& ec)
{
    U_NAMESPACE_USE

    int32_t len;
    ثابت char16_t* currname = ucurr_getName(iso, loc, UCURR_علامت_NAME,
                                          خالی_سوچک /* isChoiceFormat */, &len, &ec);
    إذا (U_SUCCESS(ec)) {
        نتیجہ.سیٹ_منزل(currname, len);
    }
}

U_CAPI int32_t U_EXPORT2
ucurr_getDefaultFractionDigits(ثابت char16_t* currency, UError_کوڈ* ec) {
    أرجع ucurr_getDefaultFractionDigitsForUsage(currency,UCURR_USAGE_STANDARD,ec);
}

U_CAPI int32_t U_EXPORT2
ucurr_getDefaultFractionDigitsForUsage(ثابت char16_t* currency, ثابت UCurrencyUsage usage, UError_کوڈ* ec) {
    int32_t fracDigits = 0;
    إذا (U_SUCCESS(*ec)) {
        بدل (usage) {
            حالة UCURR_USAGE_STANDARD:
                fracDigits = (لبھو_Meta_انکڑے(currency, *ec))[0];
                اكسر;
            حالة UCURR_USAGE_CASH:
                fracDigits = (لبھو_Meta_انکڑے(currency, *ec))[2];
                اكسر;
            افتراضي:
                *ec = U_UNSUPPORTED_ERROR;
        }
    }
    أرجع fracDigits;
}

U_CAPI مزدوج U_EXPORT2
ucurr_getRoundingIncrement(ثابت char16_t* currency, UError_کوڈ* ec) {
    أرجع ucurr_getRoundingIncrementForUsage(currency, UCURR_USAGE_STANDARD, ec);
}

U_CAPI مزدوج U_EXPORT2
ucurr_getRoundingIncrementForUsage(ثابت char16_t* currency, ثابت UCurrencyUsage usage, UError_کوڈ* ec) {
    مزدوج نتیجہ = 0.0;

    ثابت int32_t *انکڑے = لبھو_Meta_انکڑے(currency, *ec);
    إذا (U_SUCCESS(*ec)) {
        int32_t fracDigits;
        int32_t increment;
        بدل (usage) {
            حالة UCURR_USAGE_STANDARD:
                fracDigits = انکڑے[0];
                increment = انکڑے[1];
                اكسر;
            حالة UCURR_USAGE_CASH:
                fracDigits = انکڑے[2];
                increment = انکڑے[3];
                اكسر;
            افتراضي:
                *ec = U_UNSUPPORTED_ERROR;
                أرجع نتیجہ;
        }

        // If the meta data is invalid, return 0.0
        إذا (fracDigits < 0 || fracDigits > MAX_POW10) {
            *ec = U_INVALID_FORMAT_ERROR;
        } وإلا {
            // A rounding value of 0 or 1 indicates no rounding.
            إذا (increment >= 2) {
                // Return (increment) / 10^(fracDigits).  The only actual rounding data,
                // as of this writing, is CHF { 2, 5 }.
                نتیجہ = مزدوج(increment) / POW10[fracDigits];
            }
        }
    }

    أرجع نتیجہ;
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
    { خالی_سوچک, 0 } // Leave here to denote the end of the list.
};

#تعریف_کرو UCURR_MATCHES_BITMASK(variable, type_منزل_میل) \
    ((type_منزل_میل) == UCURR_ALL || ((variable) & (type_منزل_میل)) == (type_منزل_میل))

ساكن int32_t U_CALLCONV
ucurr_گنتی_Currency_List(UEnumeration *enumerator, UError_کوڈ * /*pErrorCode*/) {
    UCurrencyContext *myContext = (UCurrencyContext *)(enumerator->context);
    uint32_t currType = myContext->currType;
    int32_t count = 0;

    /* Count the number of items matching the type we are looking for. */
    لكل (int32_t idx = 0; gCurrencyList[idx].currency != خالی_سوچک; idx++) {
        إذا (UCURR_MATCHES_BITMASK(gCurrencyList[idx].currType, currType)) {
            count++;
        }
    }
    أرجع count;
}

ساكن ثابت حرف* U_CALLCONV
ucurr_اگلا_Currency_List(UEnumeration *enumerator,
                        int32_t* نتیجہ_لمائی,
                        UError_کوڈ * /*pErrorCode*/)
{
    UCurrencyContext *myContext = (UCurrencyContext *)(enumerator->context);

    /* Find the next in the list that matches the type we are looking for. */
    طالما (myContext->listIdx < UPRV_LENGTHOF(gCurrencyList)-1) {
        ثابت بنية CurrencyList *curr_شے = &gCurrencyList[myContext->listIdx++];
        إذا (UCURR_MATCHES_BITMASK(curr_شے->currType, myContext->currType))
        {
            إذا (نتیجہ_لمائی) {
                *نتیجہ_لمائی = 3; /* Currency codes are only 3 chars long */
            }
            أرجع curr_شے->currency;
        }
    }
    /* We enumerated too far. */
    إذا (نتیجہ_لمائی) {
        *نتیجہ_لمائی = 0;
    }
    أرجع خالی_سوچک;
}

ساكن فراغ U_CALLCONV
ucurr_resetCurrencyList(UEnumeration *enumerator, UError_کوڈ * /*pErrorCode*/) {
    ((UCurrencyContext *)(enumerator->context))->listIdx = 0;
}

ساكن فراغ U_CALLCONV
ucurr_closeCurrencyList(UEnumeration *enumerator) {
    uprv_free(enumerator->context);
    uprv_free(enumerator);
}

ساكن فراغ U_CALLCONV
ucurr_createCurrencyList(UHashtable *isoCodes, UError_کوڈ* حالت){
    UError_کوڈ local_حالت = U_ZERO_ERROR;

    // Look up the CurrencyMap element in the root bundle.
    UResourceBundle *rb = ures_open_سدھا(U_ICUDATA_CURR, CURRENCY_انکڑے, &local_حالت);
    Local_UResource_Bundle_اشارہ_گر currencyMapArray(ures_get_By_کلید(rb, CURRENCY_MAP, rb, &local_حالت));

    إذا (U_SUCCESS(local_حالت)) {
        // process each entry in currency map
        لكل (int32_t i=0; i<ures_get_ناپ(currencyMapArray.getAlias()); i++) {
            // get the currency resource
            Local_UResource_Bundle_اشارہ_گر currencyArray(ures_get_By_اشاریہ(currencyMapArray.getAlias(), i, خالی_سوچک, &local_حالت));
            // process each currency
            إذا (U_SUCCESS(local_حالت)) {
                لكل (int32_t j=0; j<ures_get_ناپ(currencyArray.getAlias()); j++) {
                    // get the currency resource
                    Local_UResource_Bundle_اشارہ_گر currencyRes(ures_get_By_اشاریہ(currencyArray.getAlias(), j, خالی_سوچک, &local_حالت));
                    Iso_کوڈ_اندراج *اندراج = (Iso_کوڈ_اندراج*)uprv_malloc(حجم(Iso_کوڈ_اندراج));
                    إذا (اندراج == خالی_سوچک) {
                        *حالت = U_حافظہ_ونڈ_ERROR;
                        أرجع;
                    }

                    // get the ISO code
                    int32_t iso_لمائی = 0;
                    Local_UResource_Bundle_اشارہ_گر idRes(ures_get_By_کلید(currencyRes.getAlias(), "id", خالی_سوچک, &local_حالت));
                    إذا (idRes.isNull()) {
                        استمر;
                    }
                    ثابت char16_t *iso_کوڈ = ures_getString(idRes.getAlias(), &iso_لمائی, &local_حالت);

                    // get from date
                    UDate fromDate = U_DATE_MIN;
                    Local_UResource_Bundle_اشارہ_گر fromRes(ures_get_By_کلید(currencyRes.getAlias(), "from", خالی_سوچک, &local_حالت));

                    إذا (U_SUCCESS(local_حالت)) {
                        int32_t from_لمائی = 0;
                        ثابت int32_t *fromArray = ures_get_Int_سمتیہ(fromRes.getAlias(), &from_لمائی, &local_حالت);
                        int64_t currDate64 = ((uint64_t)fromArray[0]) << 32;
                        currDate64 |= ((int64_t)fromArray[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                        fromDate = (UDate)currDate64;
                    }

                    // get to date
                    UDate منزل_Date = U_DATE_MAX;
                    local_حالت = U_ZERO_ERROR;
                    Local_UResource_Bundle_اشارہ_گر منزل_Res(ures_get_By_کلید(currencyRes.getAlias(), "to", خالی_سوچک, &local_حالت));

                    إذا (U_SUCCESS(local_حالت)) {
                        int32_t منزل_لمائی = 0;
                        ثابت int32_t *منزل_Array = ures_get_Int_سمتیہ(منزل_Res.getAlias(), &منزل_لمائی, &local_حالت);
                        int64_t currDate64 = (uint64_t)منزل_Array[0] << 32;
                        currDate64 |= ((int64_t)منزل_Array[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                        منزل_Date = (UDate)currDate64;
                    }

                    اندراج->iso_کوڈ = iso_کوڈ;
                    اندراج->from = fromDate;
                    اندراج->منزل = منزل_Date;

                    local_حالت = U_ZERO_ERROR;
                    uhash_put(isoCodes, (char16_t *)iso_کوڈ, اندراج, &local_حالت);
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
    خالی_سوچک,
    خالی_سوچک,
    ucurr_closeCurrencyList,
    ucurr_گنتی_Currency_List,
    uenum_unextDefault,
    ucurr_اگلا_Currency_List,
    ucurr_resetCurrencyList
};
U_CDECL_سرا


ساكن فراغ U_CALLCONV initIsoCodes(UError_کوڈ &حالت) {
    U_ASSERT(gIsoCodes == خالی_سوچک);
    ucln_common_رجسٹر_Cleanup(UCLN_COMMON_CURRENCY, currency_cleanup);

    Local_UHashtable_اشارہ_گر isoCodes(uhash_open(uhash_hashUChars, uhash_compareUChars, خالی_سوچک, &حالت));
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    uhash_سیٹ_قدر_Deleter(isoCodes.getAlias(), delete_Iso_کوڈ_اندراج);

    ucurr_createCurrencyList(isoCodes.getAlias(), &حالت);
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    gIsoCodes = isoCodes.orphan();  // Note: gIsoCodes is const. Once set up here it is never altered,
                                    //       and read only access is safe without synchronization.
}

ساكن فراغ populateCurrSymbolsEquiv(icu::Hashtable *hash, UError_کوڈ &حالت) {
    إذا (U_FAILURE(حالت)) { أرجع; }
    لكل (ثابت خودکار& اندراج : unisets::kCurrencyEntries) {
        UnicodeString exemplar(اندراج.exemplar);
        ثابت Unicode_سیٹ* سیٹ = unisets::get(اندراج.کلید);
        إذا (سیٹ == خالی_سوچک) { أرجع; }
        Unicode_سیٹ_Iterator it(*سیٹ);
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
    U_ASSERT(gCurrSymbolsEquiv == خالی_سوچک);
    UError_کوڈ حالت = U_ZERO_ERROR;
    ucln_common_رجسٹر_Cleanup(UCLN_COMMON_CURRENCY, currency_cleanup);
    icu::Hashtable *عارضی = جديد icu::Hashtable(حالت);
    إذا (عارضی == خالی_سوچک) {
        أرجع;
    }
    إذا (U_FAILURE(حالت)) {
        مٹاؤ عارضی;
        أرجع;
    }
    عارضی->سیٹ_قدر_Deleter(deleteUnicode);
    populateCurrSymbolsEquiv(عارضی, حالت);
    إذا (U_FAILURE(حالت)) {
        مٹاؤ عارضی;
        أرجع;
    }
    gCurrSymbolsEquiv = عارضی;
}

U_CAPI UBool U_EXPORT2
ucurr_is_دستیاب(ثابت char16_t* iso_کوڈ, UDate from, UDate منزل, UError_کوڈ* e_Error_کوڈ) {
    umtx_initOnce(gIsoCodesInitOnce, &initIsoCodes, *e_Error_کوڈ);
    إذا (U_FAILURE(*e_Error_کوڈ)) {
        أرجع خطأ;
    }

    Iso_کوڈ_اندراج* نتیجہ = (Iso_کوڈ_اندراج *) uhash_get(gIsoCodes, iso_کوڈ);
    إذا (نتیجہ == خالی_سوچک) {
        أرجع خطأ;
    } وإلا إذا (from > منزل) {
        *e_Error_کوڈ = U_ILLEGAL_دلیل_ERROR;
        أرجع خطأ;
    } وإلا إذا  ((from > نتیجہ->منزل) || (منزل < نتیجہ->from)) {
        أرجع خطأ;
    }
    أرجع سچ;
}

ساكن ثابت icu::Hashtable* getCurrSymbolsEquiv() {
    umtx_initOnce(gCurrSymbolsEquivInitOnce, &initCurrSymbolsEquiv);
    أرجع gCurrSymbolsEquiv;
}

U_CAPI UEnumeration * U_EXPORT2
ucurr_openISOCurrencies(uint32_t currType, UError_کوڈ *p_Error_کوڈ) {
    UEnumeration *myEnum = خالی_سوچک;
    UCurrencyContext *myContext;

    myEnum = (UEnumeration*)uprv_malloc(حجم(UEnumeration));
    إذا (myEnum == خالی_سوچک) {
        *p_Error_کوڈ = U_حافظہ_ونڈ_ERROR;
        أرجع خالی_سوچک;
    }
    uprv_memcpy(myEnum, &gEnumCurrencyList, حجم(UEnumeration));
    myContext = (UCurrencyContext*)uprv_malloc(حجم(UCurrencyContext));
    إذا (myContext == خالی_سوچک) {
        *p_Error_کوڈ = U_حافظہ_ونڈ_ERROR;
        uprv_free(myEnum);
        أرجع خالی_سوچک;
    }
    myContext->currType = currType;
    myContext->listIdx = 0;
    myEnum->context = myContext;
    أرجع myEnum;
}

U_CAPI int32_t U_EXPORT2
ucurr_گنتی_Currencies(ثابت حرف* locale, 
                 UDate date, 
                 UError_کوڈ* ec)
{
    int32_t curr_گنتی = 0;

    إذا (ec != خالی_سوچک && U_SUCCESS(*ec)) 
    {
        // local variables
        UError_کوڈ local_حالت = U_ZERO_ERROR;

        // get country or country_variant in `id'
        CharString id = idForLocale(locale, ec);

        إذا (U_FAILURE(*ec))
        {
            أرجع 0;
        }

        // Remove variants, which is only needed for registration.
        حرف *idDelim = strchr(id.انکڑے(), VAR_DELIM);
        إذا (idDelim)
        {
            id.truncate(idDelim - id.انکڑے());
        }

        // Look up the CurrencyMap element in the root bundle.
        UResourceBundle *rb = ures_open_سدھا(U_ICUDATA_CURR, CURRENCY_انکڑے, &local_حالت);
        UResourceBundle *cm = ures_get_By_کلید(rb, CURRENCY_MAP, rb, &local_حالت);

        // Using the id derived from the local, get the currency data
        Local_UResource_Bundle_اشارہ_گر countryArray(ures_get_By_کلید(rb, id.انکڑے(), cm, &local_حالت));

        // process each currency to see which one is valid for the given date
        إذا (U_SUCCESS(local_حالت))
        {
            لكل (int32_t i=0; i<ures_get_ناپ(countryArray.getAlias()); i++)
            {
                // get the currency resource
                Local_UResource_Bundle_اشارہ_گر currencyRes(ures_get_By_اشاریہ(countryArray.getAlias(), i, خالی_سوچک, &local_حالت));

                // get the from date
                int32_t from_لمائی = 0;
                Local_UResource_Bundle_اشارہ_گر fromRes(ures_get_By_کلید(currencyRes.getAlias(), "from", خالی_سوچک, &local_حالت));
                ثابت int32_t *fromArray = ures_get_Int_سمتیہ(fromRes.getAlias(), &from_لمائی, &local_حالت);

                int64_t currDate64 = (int64_t)((uint64_t)(fromArray[0]) << 32);
                currDate64 |= ((int64_t)fromArray[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                UDate fromDate = (UDate)currDate64;

                إذا (ures_get_ناپ(currencyRes.getAlias())> 2)
                {
                    int32_t منزل_لمائی = 0;
                    Local_UResource_Bundle_اشارہ_گر منزل_Res(ures_get_By_کلید(currencyRes.getAlias(), "to", خالی_سوچک, &local_حالت));
                    ثابت int32_t *منزل_Array = ures_get_Int_سمتیہ(منزل_Res.getAlias(), &منزل_لمائی, &local_حالت);

                    currDate64 = (int64_t)منزل_Array[0] << 32;
                    currDate64 |= ((int64_t)منزل_Array[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                    UDate منزل_Date = (UDate)currDate64;

                    إذا ((fromDate <= date) && (date < منزل_Date))
                    {
                        curr_گنتی++;
                    }
                }
                وإلا
                {
                    إذا (fromDate <= date)
                    {
                        curr_گنتی++;
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
            أرجع curr_گنتی;
        }
    }

    // If we got here, either error code is invalid or
    // some argument passed is no good.
    أرجع 0;
}

U_CAPI int32_t U_EXPORT2 
ucurr_forLocaleAndDate(ثابت حرف* locale, 
                UDate date, 
                int32_t اشاریہ,
                char16_t* buff,
                int32_t buff_سمرتھا, 
                UError_کوڈ* ec)
{
    int32_t resLen = 0;
	int32_t curr_اشاریہ = 0;
    ثابت char16_t* s = خالی_سوچک;

    إذا (ec != خالی_سوچک && U_SUCCESS(*ec))
    {
        // check the arguments passed
        إذا ((buff && buff_سمرتھا) || !buff_سمرتھا )
        {
            // local variables
            UError_کوڈ local_حالت = U_ZERO_ERROR;

            // get country or country_variant in `id'
            CharString id = idForLocale(locale, ec);
            إذا (U_FAILURE(*ec))
            {
                أرجع 0;
            }

            // Remove variants, which is only needed for registration.
            حرف *idDelim = strchr(id.انکڑے(), VAR_DELIM);
            إذا (idDelim)
            {
                id.truncate(idDelim - id.انکڑے());
            }

            // Look up the CurrencyMap element in the root bundle.
            UResourceBundle *rb = ures_open_سدھا(U_ICUDATA_CURR, CURRENCY_انکڑے, &local_حالت);
            UResourceBundle *cm = ures_get_By_کلید(rb, CURRENCY_MAP, rb, &local_حالت);

            // Using the id derived from the local, get the currency data
            Local_UResource_Bundle_اشارہ_گر countryArray(ures_get_By_کلید(rb, id.انکڑے(), cm, &local_حالت));

            // process each currency to see which one is valid for the given date
            منطقي میل_لبھیا = خطأ;
            إذا (U_SUCCESS(local_حالت))
            {
                إذا ((اشاریہ <= 0) || (اشاریہ> ures_get_ناپ(countryArray.getAlias())))
                {
                    // requested index is out of bounds
                    أرجع 0;
                }

                لكل (int32_t i=0; i<ures_get_ناپ(countryArray.getAlias()); i++)
                {
                    // get the currency resource
                    Local_UResource_Bundle_اشارہ_گر currencyRes(ures_get_By_اشاریہ(countryArray.getAlias(), i, خالی_سوچک, &local_حالت));
                    s = ures_get_String_By_کلید(currencyRes.getAlias(), "id", &resLen, &local_حالت);

                    // get the from date
                    int32_t from_لمائی = 0;
                    Local_UResource_Bundle_اشارہ_گر fromRes(ures_get_By_کلید(currencyRes.getAlias(), "from", خالی_سوچک, &local_حالت));
                    ثابت int32_t *fromArray = ures_get_Int_سمتیہ(fromRes.getAlias(), &from_لمائی, &local_حالت);

                    int64_t currDate64 = (int64_t)((uint64_t)fromArray[0] << 32);
                    currDate64 |= ((int64_t)fromArray[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                    UDate fromDate = (UDate)currDate64;

                    إذا (ures_get_ناپ(currencyRes.getAlias()) > 2)
                    {
                        int32_t منزل_لمائی = 0;
                        Local_UResource_Bundle_اشارہ_گر منزل_Res(ures_get_By_کلید(currencyRes.getAlias(), "to", خالی_سوچک, &local_حالت));
                        ثابت int32_t *منزل_Array = ures_get_Int_سمتیہ(منزل_Res.getAlias(), &منزل_لمائی, &local_حالت);

                        currDate64 = (int64_t)منزل_Array[0] << 32;
                        currDate64 |= ((int64_t)منزل_Array[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                        UDate منزل_Date = (UDate)currDate64;

                        إذا ((fromDate <= date) && (date < منزل_Date))
                        {
                            curr_اشاریہ++;
                            إذا (curr_اشاریہ == اشاریہ)
                            {
                                میل_لبھیا = سچ;
                            }
                        }
                    }
                    وإلا
                    {
                        إذا (fromDate <= date)
                        {
                            curr_اشاریہ++;
                            إذا (curr_اشاریہ == اشاریہ)
                            {
                                میل_لبھیا = سچ;
                            }
                        }
                    }
                    // check for loop exit
                    إذا (میل_لبھیا)
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
                إذا((buff_سمرتھا> resLen) && میل_لبھیا)
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
            أرجع u_terminateUChars(buff, buff_سمرتھا, resLen, ec);
        }
        وإلا
        {
            // illegal argument encountered
            *ec = U_ILLEGAL_دلیل_ERROR;
        }

    }

    // If we got here, either error code is invalid or
    // some argument passed is no good.
    أرجع resLen;
}

ساكن ثابت UEnumeration default_Keyword_اقدار = {
    خالی_سوچک,
    خالی_سوچک,
    ulist_close_keyword_اقدار_iterator,
    ulist_گنتی_keyword_اقدار,
    uenum_unextDefault,
    ulist_اگلا_keyword_قدر, 
    ulist_reset_keyword_اقدار_iterator
};

U_CAPI UEnumeration *U_EXPORT2 ucurr_get_Keyword_اقدار_For_Locale(ثابت حرف *کلید, ثابت حرف *locale, UBool commonly_ورتیا, UError_کوڈ* حالت) {
    // Resolve region
    CharString prefRegion = ulocimp_get_Region_For_Supplemental_انکڑے(locale, سچ, *حالت);

    // Read value from supplementalData
    UList *اقدار = ulist_create_خالی_تھاواں_List(حالت);
    UList *other_اقدار = ulist_create_خالی_تھاواں_List(حالت);
    UEnumeration *en = (UEnumeration *)uprv_malloc(حجم(UEnumeration));
    إذا (U_FAILURE(*حالت) || en == خالی_سوچک) {
        إذا (en == خالی_سوچک) {
            *حالت = U_حافظہ_ونڈ_ERROR;
        } وإلا {
            uprv_free(en);
        }
        ulist_deleteList(اقدار);
        ulist_deleteList(other_اقدار);
        أرجع خالی_سوچک;
    }
    حافظہ_نقل(en, &default_Keyword_اقدار, حجم(UEnumeration));
    en->context = اقدار;
    
    UResourceBundle* rb = ures_open_سدھا(U_ICUDATA_CURR, "supplementalData", حالت);
    Local_UResource_Bundle_اشارہ_گر bundle(ures_get_By_کلید(rb, "CurrencyMap", rb, حالت));
    ڈھیر_UResource_Bundle bundlekey, regbndl, curbndl, منزل;
    
    طالما (U_SUCCESS(*حالت) && ures_has_اگلا(bundle.getAlias())) {
        ures_get_اگلا_Resource(bundle.getAlias(), bundlekey.getAlias(), حالت);
        إذا (U_FAILURE(*حالت)) {
            اكسر;
        }
        ثابت حرف *region = ures_get_کلید(bundlekey.getAlias());
        UBool isPrefRegion = prefRegion == region;
        إذا (!isPrefRegion && commonly_ورتیا) {
            // With commonlyUsed=true, we do not put
            // currencies for other regions in the
            // result list.
            استمر;
        }
        ures_get_By_کلید(bundle.getAlias(), region, regbndl.getAlias(), حالت);
        إذا (U_FAILURE(*حالت)) {
            اكسر;
        }
        طالما (U_SUCCESS(*حالت) && ures_has_اگلا(regbndl.getAlias())) {
            ures_get_اگلا_Resource(regbndl.getAlias(), curbndl.getAlias(), حالت);
            إذا (ures_getType(curbndl.getAlias()) != URES_جدول) {
                // Currently, an empty ARRAY is mixed in.
                استمر;
            }
            حرف *curID = (حرف *)uprv_malloc(حجم(حرف) * ULOC_KEYWORDS_سمرتھا);
            إذا (curID == خالی_سوچک) {
                *حالت = U_حافظہ_ونڈ_ERROR;
                اكسر;
            }
            int32_t curIDLength = ULOC_KEYWORDS_سمرتھا;

#إذا U_CHARSET_FAMILY==U_ASCII_FAMILY
            ures_get_UTF8_String_By_کلید(curbndl.getAlias(), "id", curID, &curIDLength, سچ, حالت);
            /* optimize - use the utf-8 string */
#وإلا
            {
                       ثابت char16_t* defString = ures_get_String_By_کلید(curbndl.getAlias(), "id", &curIDLength, حالت);
                       إذا(U_SUCCESS(*حالت)) {
			   إذا(curIDLength+1 > ULOC_KEYWORDS_سمرتھا) {
				*حالت = U_بفر_حد_توں_ودھ_ERROR;
			   } وإلا {
                           	u_UChars_منزل_Chars(defString, curID, curIDLength+1);
			   }
                       }
            }
#جے_انت	

            إذا (U_FAILURE(*حالت)) {
                اكسر;
            }
            UBool has_منزل = خطأ;
            ures_get_By_کلید(curbndl.getAlias(), "to", منزل.getAlias(), حالت);
            إذا (U_FAILURE(*حالت)) {
                // Do nothing here...
                *حالت = U_ZERO_ERROR;
            } وإلا {
                has_منزل = سچ;
            }
            إذا (isPrefRegion && !has_منزل && !ulist_شامل_اے_String(اقدار, curID, (int32_t)uprv_strlen(curID))) {
                // Currently active currency for the target country
                ulist_جوڑو_شے_سرا_List(اقدار, curID, سچ, حالت);
            } وإلا إذا (!ulist_شامل_اے_String(other_اقدار, curID, (int32_t)uprv_strlen(curID)) && !commonly_ورتیا) {
                ulist_جوڑو_شے_سرا_List(other_اقدار, curID, سچ, حالت);
            } وإلا {
                uprv_free(curID);
            }
        }
        
    }
    إذا (U_SUCCESS(*حالت)) {
        إذا (commonly_ورتیا) {
            إذا (ulist_get_List_ناپ(اقدار) == 0) {
                // This could happen if no valid region is supplied in the input
                // locale. In this case, we use the CLDR's default.
                uenum_close(en);
                en = ucurr_get_Keyword_اقدار_For_Locale(کلید, "und", سچ, حالت);
            }
        } وإلا {
            // Consolidate the list
            حرف *value = خالی_سوچک;
            ulist_resetList(other_اقدار);
            طالما ((value = (حرف *)ulist_get_اگلا(other_اقدار)) != خالی_سوچک) {
                إذا (!ulist_شامل_اے_String(اقدار, value, (int32_t)uprv_strlen(value))) {
                    حرف *tmp_قدر = (حرف *)uprv_malloc(حجم(حرف) * ULOC_KEYWORDS_سمرتھا);
                    إذا (tmp_قدر == خالی_سوچک) {
                        *حالت = U_حافظہ_ونڈ_ERROR;
                        اكسر;
                    }
                    uprv_memcpy(tmp_قدر, value, uprv_strlen(value) + 1);
                    ulist_جوڑو_شے_سرا_List(اقدار, tmp_قدر, سچ, حالت);
                    إذا (U_FAILURE(*حالت)) {
                        اكسر;
                    }
                }
            }
        }
        ulist_resetList((UList *)(en->context));
    } وإلا {
        ulist_deleteList(اقدار);
        uprv_free(en);
        اقدار = خالی_سوچک;
        en = خالی_سوچک;
    }
    ulist_deleteList(other_اقدار);
    أرجع en;
}


U_CAPI int32_t U_EXPORT2
ucurr_get_Numeric_کوڈ(ثابت char16_t* currency) {
    int32_t کوڈ = 0;
    إذا (currency && u_strlen(currency) == ISO_CURRENCY_کوڈ_لمائی) {
        UError_کوڈ حالت = U_ZERO_ERROR;

        UResourceBundle* bundle = ures_open_سدھا(خالی_سوچک, "currencyNumericCodes", &حالت);
        Local_UResource_Bundle_اشارہ_گر کوڈ_Map(ures_get_By_کلید(bundle, "codeMap", bundle, &حالت));
        إذا (U_SUCCESS(حالت)) {
            حرف alpha_کوڈ[ISO_CURRENCY_کوڈ_لمائی+1];
            my_UChars_منزل_Chars(alpha_کوڈ, currency);
            T_CString_منزل_اتلا_Case(alpha_کوڈ);
            ures_get_By_کلید(کوڈ_Map.getAlias(), alpha_کوڈ, کوڈ_Map.getAlias(), &حالت);
            صحيح tmp_کوڈ = ures_getInt(کوڈ_Map.getAlias(), &حالت);
            إذا (U_SUCCESS(حالت)) {
                کوڈ = tmp_کوڈ;
            }
        }
    }
    أرجع کوڈ;
}
#جے_انت /* #if !UCONFIG_NO_FORMATTING */

//eof
