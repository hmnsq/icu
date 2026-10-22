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

کارول نطاق icu;

//#define UCURR_DEBUG_EQUIV 1
#که_تعریف_شوی UCURR_DEBUG_EQUIV
#ضمن "stdio.h"
#د_که_پای
//#define UCURR_DEBUG 1
#که_تعریف_شوی UCURR_DEBUG
#ضمن "stdio.h"
#د_که_پای

تعريفنوع بنية Iso_کوډ_ثبت {
    ثابت char16_t *iso_کوډ; /* const because it's a reference to a resource bundle string. */
    UDate from;
    UDate منزل;
} Iso_کوډ_ثبت;

//------------------------------------------------------------
// Constants

// Default currency meta data of last resort.  We try to use the
// defaults encoded in the meta data resource bundle.  If there is a
// configuration/build error and these are not available, we use these
// hard-coded defaults (which should be identical).
ساكن ثابت int32_t وروستی_RESORT_معلومات[] = { 2, 0, 2, 0 };

// POW10[i] = 10^i, i=0..MAX_POW10
ساكن ثابت int32_t POW10[] = { 1, 10, 100, 1000, 10000, 100000,
                                 1000000, 10000000, 100000000, 1000000000 };

ساكن ثابت int32_t MAX_POW10 = UPRV_LENGTHOF(POW10) - 1;

#تعریف_کړه ISO_CURRENCY_کوډ_اوږدتیا 3

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
ساكن ثابت UHashtable* gIsoCodes = تش_اشاره;
ساكن icu::UInitOnce gIsoCodesInitOnce {};

// Currency symbol equivalances
ساكن ثابت icu::Hashtable* gCurrSymbolsEquiv = تش_اشاره;
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
        پیل_2 = _current = &s;
    }
    مضمن ~EquivIterator() { }

    // next returns the next equivalent string or nullptr if there are no more.
    // If s has no equivalent strings, next returns nullptr on the first call.
    ثابت icu::UnicodeString *next();
خاص:
    ثابت icu::Hashtable& _hash;
    ثابت icu::UnicodeString* پیل_2;
    ثابت icu::UnicodeString* _current;
};

ثابت icu::UnicodeString *
EquivIterator::next() {
    ثابت icu::UnicodeString* بل_2 = static_cast<ثابت icu::UnicodeString*>(_hash.get(*_current));
    إذا (بل_2 == تش_اشاره) {
        U_ASSERT(_current == پیل_2);
        أرجع تش_اشاره;
    }
    إذا (*بل_2 == *پیل_2) {
        أرجع تش_اشاره;
    }
    _current = بل_2;
    أرجع بل_2;
}

U_NAMESPACE_پای_ټکی

// makeEquivalent makes lhs and rhs equivalent by updating the equivalence
// relations in hash accordingly.
ساكن فراغ makeEquivalent(
    ثابت icu::UnicodeString &lhs,
    ثابت icu::UnicodeString &rhs,
    icu::Hashtable* hash, UError_کوډ &حالت) {
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    إذا (lhs == rhs) {
        // already equivalent
        أرجع;
    }
    icu::EquivIterator کیڼ_Iter(*hash, lhs);
    icu::EquivIterator ښی_Iter(*hash, rhs);
    ثابت icu::UnicodeString *لومړی_کیڼ = کیڼ_Iter.next();
    ثابت icu::UnicodeString *لومړی_ښی = ښی_Iter.next();
    ثابت icu::UnicodeString *بل_کیڼ = لومړی_کیڼ;
    ثابت icu::UnicodeString *بل_ښی = لومړی_ښی;
    طالما (بل_کیڼ != تش_اشاره && بل_ښی != تش_اشاره) {
        إذا (*بل_کیڼ == rhs || *بل_ښی == lhs) {
            // Already equivalent
            أرجع;
        }
        بل_کیڼ = کیڼ_Iter.next();
        بل_ښی = ښی_Iter.next();
    }
    // Not equivalent. Must join.
    icu::UnicodeString *new_لومړی_کیڼ;
    icu::UnicodeString *new_لومړی_ښی;
    إذا (لومړی_ښی == تش_اشاره && لومړی_کیڼ == تش_اشاره) {
        // Neither lhs or rhs belong to an equivalence circle, so we form
        // a new equivalnce circle of just lhs and rhs.
        new_لومړی_کیڼ = جديد icu::UnicodeString(rhs);
        new_لومړی_ښی = جديد icu::UnicodeString(lhs);
    } وإلا إذا (لومړی_ښی == تش_اشاره) {
        // lhs belongs to an equivalence circle, but rhs does not, so we link
        // rhs into lhs' circle.
        new_لومړی_کیڼ = جديد icu::UnicodeString(rhs);
        new_لومړی_ښی = جديد icu::UnicodeString(*لومړی_کیڼ);
    } وإلا إذا (لومړی_کیڼ == تش_اشاره) {
        // rhs belongs to an equivlance circle, but lhs does not, so we link
        // lhs into rhs' circle.
        new_لومړی_کیڼ = جديد icu::UnicodeString(*لومړی_ښی);
        new_لومړی_ښی = جديد icu::UnicodeString(lhs);
    } وإلا {
        // Both lhs and rhs belong to different equivalnce circles. We link
        // them together to form one single, larger equivalnce circle.
        new_لومړی_کیڼ = جديد icu::UnicodeString(*لومړی_ښی);
        new_لومړی_ښی = جديد icu::UnicodeString(*لومړی_کیڼ);
    }
    إذا (new_لومړی_کیڼ == تش_اشاره || new_لومړی_ښی == تش_اشاره) {
        ړنګول new_لومړی_کیڼ;
        ړنګول new_لومړی_ښی;
        حالت = U_حافظه_ویش_ERROR;
        أرجع;
    }
    hash->put(lhs, (فراغ *) new_لومړی_کیڼ, حالت);
    hash->put(rhs, (فراغ *) new_لومړی_ښی, حالت);
}

// countEquivalent counts how many strings are equivalent to s.
// hash stores all the equivalnce relations.
// countEquivalent does not include s itself in the count.
ساكن int32_t شمېر_Equivalent(ثابت icu::Hashtable &hash, ثابت icu::UnicodeString &s) {
    int32_t پایله = 0;
    icu::EquivIterator iter(hash, s);
    طالما (iter.next() != تش_اشاره) {
        ++پایله;
    }
#که_تعریف_شوی UCURR_DEBUG_EQUIV
 {
   حرف tmp[200];
   s.extract(0,s.length(),tmp, "UTF-8");
   چاپ_کړه("CountEquivalent('%s') = %d\n", tmp, پایله);
 }
#د_که_پای
    أرجع پایله;
}

ساكن ثابت icu::Hashtable* getCurrSymbolsEquiv();

//------------------------------------------------------------
//کوډ

/**
 * Cleanup callback func
 */
ساكن UBool U_CALLCONV 
isoCodes_cleanup()
{
    إذا (gIsoCodes != تش_اشاره) {
        uhash_close(const_cast<UHashtable *>(gIsoCodes));
        gIsoCodes = تش_اشاره;
    }
    gIsoCodesInitOnce.reset();
    أرجع ریښتیا;
}

/**
 * Cleanup callback func
 */
ساكن UBool U_CALLCONV 
currSymbolsEquiv_cleanup()
{
    ړنګول const_cast<icu::Hashtable *>(gCurrSymbolsEquiv);
    gCurrSymbolsEquiv = تش_اشاره;
    gCurrSymbolsEquivInitOnce.reset();
    أرجع ریښتیا;
}

/**
 * Deleter for IsoCodeEntry
 */
ساكن فراغ U_CALLCONV
delete_Iso_کوډ_ثبت(فراغ *obj) {
    Iso_کوډ_ثبت* ثبت = static_cast<Iso_کوډ_ثبت*>(obj);
    uprv_free(ثبت);
}

/**
 * Deleter for gCurrSymbolsEquiv.
 */
ساكن فراغ U_CALLCONV
deleteUnicode(فراغ *obj) {
    icu::UnicodeString* ثبت = static_cast<icu::UnicodeString*>(obj);
    ړنګول ثبت;
}

/**
 * Unfortunately, we have to convert the char16_t* currency code to char*
 * to use it as a resource key.
 */
ساكن مضمن حرف*
my_UChars_منزل_Chars(حرف* پایله_د_Len4, ثابت char16_t* currency) {
    u_UChars_منزل_Chars(currency, پایله_د_Len4, ISO_CURRENCY_کوډ_اوږدتیا);
    پایله_د_Len4[ISO_CURRENCY_کوډ_اوږدتیا] = 0;
    أرجع پایله_د_Len4;
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
پیدا_کړه_Meta_معلومات(ثابت char16_t* currency, UError_کوډ& ec) {

    إذا (currency == تش_اشاره || *currency == 0) {
        إذا (U_SUCCESS(ec)) {
            ec = U_ILLEGAL_دلیل_ERROR;
        }
        أرجع وروستی_RESORT_معلومات;
    }

    // Get CurrencyMeta resource out of root locale file.  [This may
    // move out of the root locale file later; if it does, update this
    // code.]
    UResourceBundle* currency_معلومات = ures_open_مستقیم(U_ICUDATA_CURR, CURRENCY_معلومات, &ec);
    Local_UResource_Bundle_اشاره_ګر currencyMeta(ures_get_By_کلی(currency_معلومات, CURRENCY_META, currency_معلومات, &ec));

    إذا (U_FAILURE(ec)) {
        // Config/build error; return hard-coded defaults
        أرجع وروستی_RESORT_معلومات;
    }

    // Look up our currency, or if that's not available, then DEFAULT
    حرف buf[ISO_CURRENCY_کوډ_اوږدتیا+1];
    UError_کوډ ec2 = U_ZERO_ERROR; // local error code: soft failure
    Local_UResource_Bundle_اشاره_ګر rb(ures_get_By_کلی(currencyMeta.getAlias(), my_UChars_منزل_Chars(buf, currency), تش_اشاره, &ec2));
      إذا (U_FAILURE(ec2)) {
        rb.adoptInstead(ures_get_By_کلی(currencyMeta.getAlias(),DEFAULT_META, تش_اشاره, &ec));
        إذا (U_FAILURE(ec)) {
            // Config/build error; return hard-coded defaults
            أرجع وروستی_RESORT_معلومات;
        }
    }

    int32_t len;
    ثابت int32_t *معلومات = ures_get_Int_ویکټور(rb.getAlias(), &len, &ec);
    إذا (U_FAILURE(ec) || len != 4) {
        // Config/build error; return hard-coded defaults
        إذا (U_SUCCESS(ec)) {
            ec = U_INVALID_FORMAT_ERROR;
        }
        أرجع وروستی_RESORT_معلومات;
    }

    أرجع معلومات;
}

// -------------------------------------

ساكن CharString
idForLocale(ثابت حرف* locale, UError_کوډ* ec)
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
U_CDECL_پای_ټکی

#إذا !UCONFIG_NO_SERVICE
بنية CReg;

ساكن UMutex g_CReg_قلف;
ساكن CReg* g_CReg_سر = تش_اشاره;

بنية CReg : عام icu::UMemory {
    CReg *next;
    char16_t iso[ISO_CURRENCY_کوډ_اوږدتیا+1];
    حرف  id[ULOC_FULLNAME_ظرفیت];

    CReg(ثابت char16_t* _iso, ثابت حرف* _id)
        : next(تش_اشاره)
    {
        uprv_strncpy(id, _id, حجم(id)-1);
        id[حجم(id)-1] = 0;
        u_memcpy(iso, _iso, ISO_CURRENCY_کوډ_اوږدتیا);
        iso[ISO_CURRENCY_کوډ_اوږدتیا] = 0;
    }

    ساكن UCurr_Registry_کلی reg(ثابت char16_t* _iso, ثابت حرف* _id, UError_کوډ* حالت)
    {
        إذا (حالت && U_SUCCESS(*حالت) && _iso && _id) {
            CReg* n = جديد CReg(_iso, _id);
            إذا (n) {
                umtx_قلف(&g_CReg_قلف);
                إذا (!g_CReg_سر) {
                    /* register for the first time */
                    ucln_common_ثبتوونکی_Cleanup(UCLN_COMMON_CURRENCY, currency_cleanup);
                }
                n->next = g_CReg_سر;
                g_CReg_سر = n;
                umtx_unlock(&g_CReg_قلف);
                أرجع n;
            }
            *حالت = U_حافظه_ویش_ERROR;
        }
        أرجع تش_اشاره;
    }

    ساكن UBool unreg(UCurr_Registry_کلی کلی) {
        UBool وموندل = خطأ;
        umtx_قلف(&g_CReg_قلف);

        CReg** p = &g_CReg_سر;
        طالما (*p) {
            إذا (*p == کلی) {
                *p = ((CReg*)کلی)->next;
                ړنګول (CReg*)کلی;
                وموندل = ریښتیا;
                اكسر;
            }
            p = &((*p)->next);
        }

        umtx_unlock(&g_CReg_قلف);
        أرجع وموندل;
    }

    ساكن ثابت char16_t* get(ثابت حرف* id) {
        ثابت char16_t* پایله = تش_اشاره;
        umtx_قلف(&g_CReg_قلف);
        CReg* p = g_CReg_سر;

        /* register cleanup of the mutex */
        ucln_common_ثبتوونکی_Cleanup(UCLN_COMMON_CURRENCY, currency_cleanup);
        طالما (p) {
            إذا (uprv_strcmp(id, p->id) == 0) {
                پایله = p->iso;
                اكسر;
            }
            p = p->next;
        }
        umtx_unlock(&g_CReg_قلف);
        أرجع پایله;
    }

    /* This doesn't need to be thread safe. It's for u_cleanup only. */
    ساكن فراغ cleanup() {
        طالما (g_CReg_سر) {
            CReg* n = g_CReg_سر;
            g_CReg_سر = g_CReg_سر->next;
            ړنګول n;
        }
    }
};

// -------------------------------------

U_CAPI UCurr_Registry_کلی U_EXPORT2
ucurr_ثبتوونکی(ثابت char16_t* iso_کوډ, ثابت حرف* locale, UError_کوډ *حالت)
{
    إذا (حالت && U_SUCCESS(*حالت)) {
        CharString id = idForLocale(locale, حالت);
        أرجع CReg::reg(iso_کوډ, id.معلومات(), حالت);
    }
    أرجع تش_اشاره;
}

// -------------------------------------

U_CAPI UBool U_EXPORT2
ucurr_unregister(UCurr_Registry_کلی کلی, UError_کوډ* حالت)
{
    إذا (حالت && U_SUCCESS(*حالت)) {
        أرجع CReg::unreg(کلی);
    }
    أرجع خطأ;
}
#د_که_پای /* UCONFIG_NO_SERVICE */

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
#د_که_پای
    /*
     * There might be some cached currency data or isoCodes data.
     */
    currency_cache_cleanup();
    isoCodes_cleanup();
    currSymbolsEquiv_cleanup();

    أرجع ریښتیا;
}
U_CDECL_پای_ټکی

// -------------------------------------

U_CAPI int32_t U_EXPORT2
ucurr_forLocale(ثابت حرف* locale,
                char16_t* buff,
                int32_t buff_ظرفیت,
                UError_کوډ* ec) {
    إذا (U_FAILURE(*ec)) { أرجع 0; }
    إذا (buff_ظرفیت < 0 || (buff == تش_اشاره && buff_ظرفیت > 0)) {
        *ec = U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }

    UError_کوډ local_حالت = U_ZERO_ERROR;
    CharString currency = ulocimp_get_Keyword_ارزښت(locale, "currency", local_حالت);
    int32_t resLen = currency.length();

    إذا (U_SUCCESS(local_حالت) && resLen == 3 && uprv_isInvariantString(currency.معلومات(), resLen)) {
        إذا (resLen < buff_ظرفیت) {
            T_CString_منزل_پاسنی_Case(currency.معلومات());
            u_chars_منزل_UChars(currency.معلومات(), buff, resLen);
        }
        أرجع u_terminateUChars(buff, buff_ظرفیت, resLen, ec);
    }

    // get country or country_variant in `id'
    CharString id = idForLocale(locale, ec);
    إذا (U_FAILURE(*ec)) {
        أرجع 0;
    }

#إذا !UCONFIG_NO_SERVICE
    ثابت char16_t* پایله = CReg::get(id.معلومات());
    إذا (پایله) {
        إذا(buff_ظرفیت > u_strlen(پایله)) {
            u_strcpy(buff, پایله);
        }
        resLen = u_strlen(پایله);
        أرجع u_terminateUChars(buff, buff_ظرفیت, resLen, ec);
    }
#د_که_پای
    // Remove variants, which is only needed for registration.
    حرف *idDelim = uprv_strchr(id.معلومات(), VAR_DELIM);
    إذا (idDelim) {
        id.truncate(idDelim - id.معلومات());
    }

    ثابت char16_t* s = تش_اشاره;  // Currency code from data file.
    إذا (id.is_تشه()) {
        // No point looking in the data for an empty string.
        // This is what we would get.
        local_حالت = U_MISSING_RESOURCE_ERROR;
    } وإلا {
        // Look up the CurrencyMap element in the root bundle.
        local_حالت = U_ZERO_ERROR;
        UResourceBundle *rb = ures_open_مستقیم(U_ICUDATA_CURR, CURRENCY_معلومات, &local_حالت);
        UResourceBundle *cm = ures_get_By_کلی(rb, CURRENCY_MAP, rb, &local_حالت);
        Local_UResource_Bundle_اشاره_ګر countryArray(ures_get_By_کلی(rb, id.معلومات(), cm, &local_حالت));
        // https://unicode-org.atlassian.net/browse/ICU-21997
        // Prefer to use currencies that are legal tender.
        إذا (U_SUCCESS(local_حالت)) {
            int32_t array_اوږدتیا = ures_get_اندازه(countryArray.getAlias());
            لكل (int32_t i = 0; i < array_اوږدتیا; ++i) {
                Local_UResource_Bundle_اشاره_ګر currencyReq(
                    ures_get_By_شاخص(countryArray.getAlias(), i, تش_اشاره, &local_حالت));
                // The currency is legal tender if it is *not* marked with tender{"false"}.
                UError_کوډ tender_حالت = local_حالت;
                ثابت char16_t *tender =
                    ures_get_String_By_کلی(currencyReq.getAlias(), "tender", تش_اشاره, &tender_حالت);
                منطقي isTender = U_FAILURE(tender_حالت) || u_strcmp(tender, u"false") != 0;
                إذا (!isTender && s != تش_اشاره) {
                    // We already have a non-tender currency. Ignore all following non-tender ones.
                    استمر;
                }
                // Fetch the currency code.
                s = ures_get_String_By_کلی(currencyReq.getAlias(), "id", &resLen, &local_حالت);
                إذا (isTender) {
                    اكسر;
                }
            }
            إذا (U_SUCCESS(local_حالت) && s == تش_اشاره) {
                local_حالت = U_MISSING_RESOURCE_ERROR;
            }
        }
    }

    إذا ((U_FAILURE(local_حالت)) && strchr(id.معلومات(), '_') != تش_اشاره) {
        // We don't know about it.  Check to see if we support the variant.
        CharString پلار = ulocimp_get_پلار(locale, *ec);
        *ec = U_USING_FALLBACK_WARNING;
        // TODO: Loop over the parent rather than recursing and
        // looking again for a currency keyword.
        أرجع ucurr_forLocale(پلار.معلومات(), buff, buff_ظرفیت, ec);
    }
    إذا (*ec == U_ZERO_ERROR || local_حالت != U_ZERO_ERROR) {
        // There is nothing to fallback to. Report the failure/warning if possible.
        *ec = local_حالت;
    }
    إذا (U_SUCCESS(*ec)) {
        إذا(buff_ظرفیت > resLen) {
            u_strcpy(buff, s);
        }
    }
    أرجع u_terminateUChars(buff, buff_ظرفیت, resLen, ec);
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
    إذا (loc.is_تشه()) {
        أرجع خطأ;
    }
    UError_کوډ حالت = U_ZERO_ERROR;
    إذا (loc == "en_GB") {
        // HACK: See #13368.  We need "en_GB" to fall back to "en_001" instead of "en"
        // in order to consume the correct data strings.  This hack will be removed
        // when proper data sink loading is implemented here.
        loc.truncate(3);
        loc.append("001", حالت);
    } وإلا {
        loc = ulocimp_get_پلار(loc.معلومات(), حالت);
    }
 /*
    char *i = uprv_strrchr(loc, '_');
    if (i == nullptr) {
        i = loc;
    }
    *i = 0;
 */
    أرجع ریښتیا;
}


U_CAPI ثابت char16_t* U_EXPORT2
ucurr_getName(ثابت char16_t* currency U_LIFETIME_BOUND,
              ثابت حرف* locale,
              UCurrNameStyle nameStyle,
              UBool* isChoiceFormat, // fillin
              int32_t* len, // fillin
              UError_کوډ* ec) {

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
        أرجع تش_اشاره;
    }

    int32_t choice = (int32_t) nameStyle;
    إذا (choice < 0 || choice > 4) {
        *ec = U_ILLEGAL_دلیل_ERROR;
        أرجع تش_اشاره;
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
    UError_کوډ ec2 = U_ZERO_ERROR;

    إذا (locale == تش_اشاره) {
        locale = uloc_getDefault();
    }
    CharString loc = ulocimp_getName(locale, ec2);
    إذا (U_FAILURE(ec2)) {
        *ec = U_ILLEGAL_دلیل_ERROR;
        أرجع تش_اشاره;
    }

    حرف buf[ISO_CURRENCY_کوډ_اوږدتیا+1];
    my_UChars_منزل_Chars(buf, currency);
    
    /* Normalize the keyword value to uppercase */
    T_CString_منزل_پاسنی_Case(buf);
    
    ثابت char16_t* s = تش_اشاره;
    ec2 = U_ZERO_ERROR;
    Local_UResource_Bundle_اشاره_ګر rb(ures_open(U_ICUDATA_CURR, loc.معلومات(), &ec2));

    إذا (nameStyle == UCURR_NARROW_نښه_NAME || nameStyle == UCURR_FORMAL_نښه_NAME || nameStyle == UCURR_VARIANT_نښه_NAME) {
        CharString کلی;
        بدل (nameStyle) {
        حالة UCURR_NARROW_نښه_NAME:
            کلی.append(CURRENCIES_NARROW, ec2);
            اكسر;
        حالة UCURR_FORMAL_نښه_NAME:
            کلی.append(CURRENCIES_FORMAL, ec2);
            اكسر;
        حالة UCURR_VARIANT_نښه_NAME:
            کلی.append(CURRENCIES_VARIANT, ec2);
            اكسر;
        افتراضي:
            *ec = U_UNSUPPORTED_ERROR;
            أرجع تش_اشاره;
        }
        کلی.append("/", ec2);
        کلی.append(buf, ec2);
        s = ures_get_String_By_کلی_With_Fallback(rb.getAlias(), کلی.معلومات(), len, &ec2);
        إذا (ec2 == U_MISSING_RESOURCE_ERROR) {
            *ec = U_USING_FALLBACK_WARNING;
            ec2 = U_ZERO_ERROR;
            choice = UCURR_نښه_NAME;
        }
    }
    إذا (s == تش_اشاره) {
        ures_get_By_کلی(rb.getAlias(), CURRENCIES, rb.getAlias(), &ec2);
        ures_get_By_کلی_With_Fallback(rb.getAlias(), buf, rb.getAlias(), &ec2);
        s = ures_get_String_By_شاخص(rb.getAlias(), choice, len, &ec2);
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
    إذا (isChoiceFormat != تش_اشاره) {
        *isChoiceFormat = خطأ;
    }
    إذا (U_SUCCESS(ec2)) {
        U_ASSERT(s != تش_اشاره);
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
                    ثابت حرف* plural_شمېر,
                    int32_t* len, // fillin
                    UError_کوډ* ec) {
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
        أرجع تش_اشاره;
    }

    // Use a separate UErrorCode here that does not propagate out of
    // this function.
    UError_کوډ ec2 = U_ZERO_ERROR;

    إذا (locale == تش_اشاره) {
        locale = uloc_getDefault();
    }
    CharString loc = ulocimp_getName(locale, ec2);
    إذا (U_FAILURE(ec2)) {
        *ec = U_ILLEGAL_دلیل_ERROR;
        أرجع تش_اشاره;
    }

    حرف buf[ISO_CURRENCY_کوډ_اوږدتیا+1];
    my_UChars_منزل_Chars(buf, currency);

    ثابت char16_t* s = تش_اشاره;
    ec2 = U_ZERO_ERROR;
    UResourceBundle* rb = ures_open(U_ICUDATA_CURR, loc.معلومات(), &ec2);

    rb = ures_get_By_کلی(rb, CURRENCYPLURALS, rb, &ec2);

    // Fetch resource with multi-level resource inheritance fallback
    Local_UResource_Bundle_اشاره_ګر curr(ures_get_By_کلی_With_Fallback(rb, buf, rb, &ec2));

    s = ures_get_String_By_کلی_With_Fallback(curr.getAlias(), plural_شمېر, len, &ec2);
    إذا (U_FAILURE(ec2)) {
        //  fall back to "other"
        ec2 = U_ZERO_ERROR;
        s = ures_get_String_By_کلی_With_Fallback(curr.getAlias(), "other", len, &ec2);     
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
        U_ASSERT(s != تش_اشاره);
        أرجع s;
    }

    // If we fail to find a match, use the ISO 4217 code
    *len = u_strlen(currency); // Should == ISO_CURRENCY_CODE_LENGTH, but maybe not...?
    *ec = U_USING_DEFAULT_WARNING;
    أرجع currency;
}


//========================================================================
// Following are structure and function for parsing currency names

#تعریف_کړه NEED_منزل_BE_DELETED 0x1

// TODO: a better way to define this?
#تعریف_کړه MAX_CURRENCY_NAME_LEN 100

تعريفنوع بنية {
    ثابت حرف* Iso_کوډ;  //کلی
    char16_t* currencyName;  //ارزښت
    int32_t currencyNameLen;  // value length
    int32_t flag;  // flags
} CurrencyNameStruct;


#که_نه_تعریف_شوی MIN
#تعریف_کړه MIN(a,b) (((a)<(b)) ? (a) : (b))
#د_که_پای

#که_نه_تعریف_شوی MAX
#تعریف_کړه MAX(a,b) (((a)<(b)) ? (b) : (a))
#د_که_پای


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
get_Currency_Name_شمېر(ثابت حرف* loc, int32_t* ټول_currency_name_شمېر, int32_t* ټول_currency_نښه_شمېر) {
    U_NAMESPACE_USE
    *ټول_currency_name_شمېر = 0;
    *ټول_currency_نښه_شمېر = 0;
    ثابت char16_t* s = تش_اشاره;
    CharString locale;
    {
        UError_کوډ حالت = U_ZERO_ERROR;
        locale.append(loc, حالت);
        إذا (U_FAILURE(حالت)) { أرجع; }
    }
    ثابت icu::Hashtable *currencySymbolsEquiv = getCurrSymbolsEquiv();
    لكل (;;) {
        UError_کوډ ec2 = U_ZERO_ERROR;
        // TODO: ures_openDirect?
        Local_UResource_Bundle_اشاره_ګر rb(ures_open(U_ICUDATA_CURR, locale.معلومات(), &ec2));
        Local_UResource_Bundle_اشاره_ګر curr(ures_get_By_کلی(rb.getAlias(), CURRENCIES, تش_اشاره, &ec2));
        int32_t n = ures_get_اندازه(curr.getAlias());
        لكل (int32_t i=0; i<n; ++i) {
            Local_UResource_Bundle_اشاره_ګر نومونه(ures_get_By_شاخص(curr.getAlias(), i, تش_اشاره, &ec2));
            int32_t len;
            s = ures_get_String_By_شاخص(نومونه.getAlias(), UCURR_نښه_NAME, &len, &ec2);
            ++(*ټول_currency_نښه_شمېر);  // currency symbol
            إذا (currencySymbolsEquiv != تش_اشاره) {
                *ټول_currency_نښه_شمېر += شمېر_Equivalent(*currencySymbolsEquiv, UnicodeString(ریښتیا, s, len));
            }
            ++(*ټول_currency_نښه_شمېر); // iso code
            ++(*ټول_currency_name_شمېر); // long name
        }

        // currency plurals
        UError_کوډ ec3 = U_ZERO_ERROR;
        Local_UResource_Bundle_اشاره_ګر curr_p(ures_get_By_کلی(rb.getAlias(), CURRENCYPLURALS, تش_اشاره, &ec3));
        n = ures_get_اندازه(curr_p.getAlias());
        لكل (int32_t i=0; i<n; ++i) {
            Local_UResource_Bundle_اشاره_ګر نومونه(ures_get_By_شاخص(curr_p.getAlias(), i, تش_اشاره, &ec3));
            *ټول_currency_name_شمېر += ures_get_اندازه(نومونه.getAlias());
        }

        إذا (!fallback(locale)) {
            اكسر;
        }
    }
}

ساكن char16_t*
منزل_پاسنی_Case(ثابت char16_t* سرچینه, int32_t len, ثابت حرف* locale) {
    char16_t* dest = تش_اشاره;
    UError_کوډ ec = U_ZERO_ERROR;
    int32_t destLen = u_str_منزل_پاسنی(dest, 0, سرچینه, len, locale, &ec);

    ec = U_ZERO_ERROR;
    dest = static_cast<char16_t*>(uprv_malloc(حجم(char16_t) * MAX(destLen, len)));
    إذا (dest == تش_اشاره) {
        أرجع تش_اشاره;
    }
    u_str_منزل_پاسنی(dest, destLen, سرچینه, len, locale, &ec);
    إذا (U_FAILURE(ec)) {
        u_memcpy(dest, سرچینه, len);
    } 
    أرجع dest;
}


ساكن فراغ delete_Currency_نومونه(CurrencyNameStruct* currency_نومونه, int32_t count);
// Collect all available currency names associated with the given locale
// (enable fallback chain).
// Read currenc names defined in resource bundle "Currencies" and
// "CurrencyPlural", enable fallback chain.
// return the malloc-ed currency name arrays and the total number of currency
// names in the array.
ساكن فراغ
collect_Currency_نومونه(ثابت حرف* locale, 
                     CurrencyNameStruct** currency_نومونه, 
                     int32_t* ټول_currency_name_شمېر, 
                     CurrencyNameStruct** currencySymbols, 
                     int32_t* ټول_currency_نښه_شمېر, 
                     UError_کوډ& ec) {
    إذا (U_FAILURE(ec)) {
        *currency_نومونه = *currencySymbols = تش_اشاره;
        *ټول_currency_name_شمېر = *ټول_currency_نښه_شمېر = 0;
        أرجع;
    }
    U_NAMESPACE_USE
    ثابت icu::Hashtable *currencySymbolsEquiv = getCurrSymbolsEquiv();
    // Look up the Currencies resource for the given locale.
    UError_کوډ ec2 = U_ZERO_ERROR;

    إذا (locale == تش_اشاره) {
        locale = uloc_getDefault();
    }
    CharString loc = ulocimp_getName(locale, ec2);
    إذا (U_FAILURE(ec2)) {
        ec = U_ILLEGAL_دلیل_ERROR;
        *currency_نومونه = *currencySymbols = تش_اشاره;
        *ټول_currency_name_شمېر = *ټول_currency_نښه_شمېر = 0;
        أرجع;
    }

    // Get maximum currency name count first.
    get_Currency_Name_شمېر(loc.معلومات(), ټول_currency_name_شمېر, ټول_currency_نښه_شمېر);

    *currency_نومونه = static_cast<CurrencyNameStruct*>(
        uprv_malloc(حجم(CurrencyNameStruct) * (*ټول_currency_name_شمېر)));
    إذا(*currency_نومونه == تش_اشاره) {
        *currencySymbols = تش_اشاره;
        *ټول_currency_name_شمېر = *ټول_currency_نښه_شمېر = 0;
        ec = U_حافظه_ویش_ERROR;
        أرجع;
    }
    *currencySymbols = static_cast<CurrencyNameStruct*>(
        uprv_malloc(حجم(CurrencyNameStruct) * (*ټول_currency_نښه_شمېر)));

    إذا(*currencySymbols == تش_اشاره) {
        uprv_free(*currency_نومونه);
        *currency_نومونه = تش_اشاره;
        *ټول_currency_name_شمېر = *ټول_currency_نښه_شمېر = 0;
        ec = U_حافظه_ویش_ERROR;
        أرجع;
    }

    ثابت char16_t* s = تش_اشاره;  // currency name
    حرف* iso = تش_اشاره;  // currency ISO code

    *ټول_currency_name_شمېر = 0;
    *ټول_currency_نښه_شمېر = 0;

    UError_کوډ ec3 = U_ZERO_ERROR;
    UError_کوډ ec4 = U_ZERO_ERROR;

    // Using hash to remove duplicates caused by locale fallback
    Local_UHashtable_اشاره_ګر currencyIsoCodes(uhash_open(uhash_hashChars, uhash_compareChars, تش_اشاره, &ec3));
    Local_UHashtable_اشاره_ګر currencyPluralIsoCodes(uhash_open(uhash_hashChars, uhash_compareChars, تش_اشاره, &ec4));
    لكل (int32_t localeLevel = 0; ; ++localeLevel) {
        ec2 = U_ZERO_ERROR;
        // TODO: ures_openDirect
        Local_UResource_Bundle_اشاره_ګر rb(ures_open(U_ICUDATA_CURR, loc.معلومات(), &ec2));
        Local_UResource_Bundle_اشاره_ګر curr(ures_get_By_کلی(rb.getAlias(), CURRENCIES, تش_اشاره, &ec2));
        int32_t n = ures_get_اندازه(curr.getAlias());
        لكل (int32_t i=0; i<n; ++i) {
            Local_UResource_Bundle_اشاره_ګر نومونه(ures_get_By_شاخص(curr.getAlias(), i, تش_اشاره, &ec2));
            int32_t len;
            s = ures_get_String_By_شاخص(نومونه.getAlias(), UCURR_نښه_NAME, &len, &ec2);
            // TODO: uhash_put wont change key/value?
            iso = const_cast<حرف*>(ures_get_کلی(نومونه.getAlias()));
            إذا (localeLevel != 0 && uhash_get(currencyIsoCodes.getAlias(), iso) != تش_اشاره) {
                استمر;
            }
            uhash_put(currencyIsoCodes.getAlias(), iso, iso, &ec3); 
            // Add currency symbol.
            (*currencySymbols)[(*ټول_currency_نښه_شمېر)++] = {iso, const_cast<char16_t*>(s), len, 0};

            // Add equivalent symbols
            إذا (currencySymbolsEquiv != تش_اشاره) {
                UnicodeString str(ریښتیا, s, len);
                icu::EquivIterator iter(*currencySymbolsEquiv, str);
                ثابت UnicodeString *نښه;
                طالما ((نښه = iter.next()) != تش_اشاره) {
                    (*currencySymbols)[(*ټول_currency_نښه_شمېر)++]
                        = {iso, const_cast<char16_t*>(نښه->get_بفر()), نښه->length(), 0};
                }
            }

            // Add currency long name.
            s = ures_get_String_By_شاخص(نومونه.getAlias(), UCURR_LONG_NAME, &len, &ec2);
            char16_t* پاسنی_Name = منزل_پاسنی_Case(s, len, locale);
            إذا (پاسنی_Name == تش_اشاره) {
                ec = U_حافظه_ویش_ERROR;
                اذهب error;
            }
            (*currency_نومونه)[(*ټول_currency_name_شمېر)++] = {iso, پاسنی_Name, len, NEED_منزل_BE_DELETED};

            // put (iso, 3, and iso) in to array
            // Add currency ISO code.
            char16_t* iso_کوډ = static_cast<char16_t*>(uprv_malloc(حجم(char16_t) * 3));
            إذا (iso_کوډ == تش_اشاره) {
                ec = U_حافظه_ویش_ERROR;
                اذهب error;
            }
            // Must convert iso[] into Unicode
            u_chars_منزل_UChars(iso, iso_کوډ, 3);
            (*currencySymbols)[(*ټول_currency_نښه_شمېر)++] = {iso, iso_کوډ, 3, NEED_منزل_BE_DELETED};
        }

        // currency plurals
        UError_کوډ ec5 = U_ZERO_ERROR;
        Local_UResource_Bundle_اشاره_ګر curr_p(ures_get_By_کلی(rb.getAlias(), CURRENCYPLURALS, تش_اشاره, &ec5));
        n = ures_get_اندازه(curr_p.getAlias());
        لكل (int32_t i=0; i<n; ++i) {
            Local_UResource_Bundle_اشاره_ګر نومونه(ures_get_By_شاخص(curr_p.getAlias(), i, تش_اشاره, &ec5));
            iso = const_cast<حرف*>(ures_get_کلی(نومونه.getAlias()));
            // Using hash to remove duplicated ISO codes in fallback chain.
            إذا (localeLevel != 0 && uhash_get(currencyPluralIsoCodes.getAlias(), iso) != تش_اشاره) {
                استمر;
            }
            uhash_put(currencyPluralIsoCodes.getAlias(), iso, iso, &ec4);
            int32_t num = ures_get_اندازه(نومونه.getAlias());
            int32_t len;
            لكل (int32_t j = 0; j < num; ++j) {
                // TODO: remove duplicates between singular name and 
                // currency long name?
                s = ures_get_String_By_شاخص(نومونه.getAlias(), j, &len, &ec5);
                char16_t* پاسنی_Name = منزل_پاسنی_Case(s, len, locale);
                إذا (پاسنی_Name == تش_اشاره) {
                    ec = U_حافظه_ویش_ERROR;
                    اذهب error;
                }
                (*currency_نومونه)[(*ټول_currency_name_شمېر)++] = {iso, پاسنی_Name, len, NEED_منزل_BE_DELETED};
            }
        }

        إذا (!fallback(loc)) {
            اكسر;
        }
    }

    // quick sort the struct
    qsort(*currency_نومونه, *ټول_currency_name_شمېر, 
          حجم(CurrencyNameStruct), currencyNameComparator);
    qsort(*currencySymbols, *ټول_currency_نښه_شمېر, 
          حجم(CurrencyNameStruct), currencyNameComparator);

#که_تعریف_شوی UCURR_DEBUG
    چاپ_کړه("currency name count: %d\n", *ټول_currency_name_شمېر);
    لكل (int32_t شاخص = 0; شاخص < *ټول_currency_name_شمېر; ++شاخص) {
        چاپ_کړه("index: %d\n", شاخص);
        چاپ_کړه("iso: %s\n", (*currency_نومونه)[شاخص].Iso_کوډ);
        حرف curNameBuf[1024];
        memset(curNameBuf, 0, 1024);
        u_austrncpy(curNameBuf, (*currency_نومونه)[شاخص].currencyName, (*currency_نومونه)[شاخص].currencyNameLen);
        چاپ_کړه("currencyName: %s\n", curNameBuf);
        چاپ_کړه("len: %d\n", (*currency_نومونه)[شاخص].currencyNameLen);
    }
    چاپ_کړه("currency symbol count: %d\n", *ټول_currency_نښه_شمېر);
    لكل (int32_t شاخص = 0; شاخص < *ټول_currency_نښه_شمېر; ++شاخص) {
        چاپ_کړه("index: %d\n", شاخص);
        چاپ_کړه("iso: %s\n", (*currencySymbols)[شاخص].Iso_کوډ);
        حرف curNameBuf[1024];
        memset(curNameBuf, 0, 1024);
        u_austrncpy(curNameBuf, (*currencySymbols)[شاخص].currencyName, (*currencySymbols)[شاخص].currencyNameLen);
        چاپ_کړه("currencySymbol: %s\n", curNameBuf);
        چاپ_کړه("len: %d\n", (*currencySymbols)[شاخص].currencyNameLen);
    }
#د_که_پای
    // fail on hashtable errors
    إذا (U_FAILURE(ec3)) {
      ec = ec3;
    } وإلا إذا (U_FAILURE(ec4)) {
      ec = ec4;
    }

  error:
    // clean up if we got error
    إذا (U_FAILURE(ec)) {
        delete_Currency_نومونه(*currency_نومونه, *ټول_currency_name_شمېر);
        delete_Currency_نومونه(*currencySymbols, *ټول_currency_نښه_شمېر);
        *currency_نومونه = *currencySymbols = تش_اشاره;
        *ټول_currency_name_شمېر = *ټول_currency_نښه_شمېر = 0;
    }
}

// @param  currencyNames: currency names array
// @param  indexInCurrencyNames: the index of the character in currency names 
//         array against which the comparison is done
// @param  key: input text char to compare against
// @param  begin(IN/OUT): the begin index of matching range in currency names array
// @param  end(IN/OUT): the end index of matching range in currency names array.
ساكن int32_t
دوګونې_پلټنه(ثابت CurrencyNameStruct* currency_نومونه, 
             int32_t شاخص_دننه_Currency_نومونه,
             ثابت char16_t کلی,
             int32_t* begin, int32_t* پای_ټکی) {
#که_تعریف_شوی UCURR_DEBUG
    چاپ_کړه("key = %x\n", کلی);
#د_که_پای
   int32_t first = *begin;
   int32_t وروستی = *پای_ټکی;
   طالما (first <= وروستی) {
       int32_t mid = (first + وروستی) / 2;  // compute mid point.
       إذا (شاخص_دننه_Currency_نومونه >= currency_نومونه[mid].currencyNameLen) {
           first = mid + 1;
       } وإلا {
           إذا (کلی > currency_نومونه[mid].currencyName[شاخص_دننه_Currency_نومونه]) {
               first = mid + 1;
           }
           وإلا إذا (کلی < currency_نومونه[mid].currencyName[شاخص_دننه_Currency_نومونه]) {
               وروستی = mid - 1;
           }
           وإلا {
                // Find a match, and looking for ranges
                // Now do two more binary searches. First, on the left side for
                // the greatest L such that CurrencyNameStruct[L] < key.
                int32_t L = *begin;
                int32_t R = mid;

#که_تعریف_شوی UCURR_DEBUG
                چاپ_کړه("mid = %d\n", mid);
#د_که_پای
                طالما (L < R) {
                    int32_t M = (L + R) / 2;
#که_تعریف_شوی UCURR_DEBUG
                    چاپ_کړه("L = %d, R = %d, M = %d\n", L, R, M);
#د_که_پای
                    إذا (شاخص_دننه_Currency_نومونه >= currency_نومونه[M].currencyNameLen) {
                        L = M + 1;
                    } وإلا {
                        إذا (currency_نومونه[M].currencyName[شاخص_دننه_Currency_نومونه] < کلی) {
                            L = M + 1;
                        } وإلا {
#که_تعریف_شوی UCURR_DEBUG
                            U_ASSERT(currency_نومونه[M].currencyName[شاخص_دننه_Currency_نومونه] == کلی);
#د_که_پای
                            R = M;
                        }
                    }
                }
#که_تعریف_شوی UCURR_DEBUG
                U_ASSERT(L == R);
#د_که_پای
                *begin = L;
#که_تعریف_شوی UCURR_DEBUG
                چاپ_کړه("begin = %d\n", *begin);
                U_ASSERT(currency_نومونه[*begin].currencyName[شاخص_دننه_Currency_نومونه] == کلی);
#د_که_پای

                // Now for the second search, finding the least R such that
                // key < CurrencyNameStruct[R].
                L = mid;
                R = *پای_ټکی;
                طالما (L < R) {
                    int32_t M = (L + R) / 2;
#که_تعریف_شوی UCURR_DEBUG
                    چاپ_کړه("L = %d, R = %d, M = %d\n", L, R, M);
#د_که_پای
                    إذا (currency_نومونه[M].currencyNameLen < شاخص_دننه_Currency_نومونه) {
                        L = M + 1;
                    } وإلا {
                        إذا (currency_نومونه[M].currencyName[شاخص_دننه_Currency_نومونه] > کلی) {
                            R = M;
                        } وإلا {
#که_تعریف_شوی UCURR_DEBUG
                            U_ASSERT(currency_نومونه[M].currencyName[شاخص_دننه_Currency_نومونه] == کلی);
#د_که_پای
                            L = M + 1;
                        }
                    }
                }
#که_تعریف_شوی UCURR_DEBUG
                U_ASSERT(L == R);
#د_که_پای
                إذا (currency_نومونه[R].currencyName[شاخص_دننه_Currency_نومونه] > کلی) {
                    *پای_ټکی = R - 1;
                } وإلا {
                    *پای_ټکی = R;
                }
#که_تعریف_شوی UCURR_DEBUG
                چاپ_کړه("end = %d\n", *پای_ټکی);
#د_که_پای

                // now, found the range. check whether there is exact match
                إذا (currency_نومونه[*begin].currencyNameLen == شاخص_دننه_Currency_نومونه + 1) {
                    أرجع *begin;  // find range and exact match.
                }
                أرجع -1;  // find range, but no exact match.
           }
       }
   }
   *begin = -1;
   *پای_ټکی = -1;
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
خطي_پلټنه(ثابت CurrencyNameStruct* currency_نومونه, 
             int32_t begin, int32_t پای_ټکی,
             ثابت char16_t* متن, int32_t متن_Len,
             int32_t *partial_سمون_Len,
             int32_t *max_سمون_Len, int32_t* max_سمون_شاخص) {
    int32_t initial_Partial_سمون_Len = *partial_سمون_Len;
    لكل (int32_t شاخص = begin; شاخص <= پای_ټکی; ++شاخص) {
        int32_t len = currency_نومونه[شاخص].currencyNameLen;
        إذا (len > *max_سمون_Len && len <= متن_Len &&
            uprv_memcmp(currency_نومونه[شاخص].currencyName, متن, len * حجم(char16_t)) == 0) {
            *partial_سمون_Len = MAX(*partial_سمون_Len, len);
            *max_سمون_شاخص = شاخص;
            *max_سمون_Len = len;
#که_تعریف_شوی UCURR_DEBUG
            چاپ_کړه("maxMatchIndex = %d, maxMatchLen = %d\n",
                   *max_سمون_شاخص, *max_سمون_Len);
#د_که_پای
        } وإلا {
            // Check for partial matches.
            لكل (int32_t i=initial_Partial_سمون_Len; i<MIN(len, متن_Len); i++) {
                إذا (currency_نومونه[شاخص].currencyName[i] != متن[i]) {
                    اكسر;
                }
                *partial_سمون_Len = MAX(*partial_سمون_Len, i + 1);
            }
        }
    }
}

#تعریف_کړه خطي_پلټنه_پوله 10

// Find longest match between "text" and currency names in "currencyNames".
// @param  total_currency_count: total number of currency names in CurrencyNames.
// @param  textLen: the length of the text to be compared
// @param  maxMatchLen: passing in the computed max matching length
//                              pass out the new max  matching length
// @param  maxMatchIndex: the index in currencyName which has the longest
//                        match with input text.
ساكن فراغ
پلټنه_Currency_Name(ثابت CurrencyNameStruct* currency_نومونه, 
                   int32_t ټول_currency_شمېر,
                   ثابت char16_t* متن, int32_t متن_Len,
                   int32_t *partial_سمون_Len,
                   int32_t* max_سمون_Len, int32_t* max_سمون_شاخص) {
    *max_سمون_شاخص = -1;
    *max_سمون_Len = 0;
    int32_t سمون_شاخص = -1;
    int32_t دوګونی_پلټنه_Begin = 0;
    int32_t دوګونی_پلټنه_پای_ټکی = ټول_currency_شمېر - 1;
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
    لكل (int32_t شاخص = 0; شاخص < متن_Len; ++شاخص) {
        // matchIndex saves the one with exact match till the current point.
        // [binarySearchBegin, binarySearchEnd] saves the matching range.
        سمون_شاخص = دوګونې_پلټنه(currency_نومونه, شاخص,
                                  متن[شاخص],
                                  &دوګونی_پلټنه_Begin, &دوګونی_پلټنه_پای_ټکی);
        إذا (دوګونی_پلټنه_Begin == -1) { // did not find the range
            اكسر;
        }
        *partial_سمون_Len = MAX(*partial_سمون_Len, شاخص + 1);
        إذا (سمون_شاخص != -1) { 
            // find an exact match for text from text[0] to text[index] 
            // in currencyNames array.
            *max_سمون_Len = شاخص + 1;
            *max_سمون_شاخص = سمون_شاخص;
        }
        إذا (دوګونی_پلټنه_پای_ټکی - دوګونی_پلټنه_Begin < خطي_پلټنه_پوله) {
            // linear search if within threshold.
            خطي_پلټنه(currency_نومونه, دوګونی_پلټنه_Begin, دوګونی_پلټنه_پای_ټکی,
                         متن, متن_Len,
                         partial_سمون_Len,
                         max_سمون_Len, max_سمون_شاخص);
            اكسر;
        }
    }
}

//========================= currency name cache =====================
تعريفنوع بنية {
    حرف locale[ULOC_FULLNAME_ظرفیت];  //key
    // currency names, case insensitive
    CurrencyNameStruct* currency_نومونه;  //ارزښت
    int32_t ټول_Currency_Name_شمېر;  // currency name count
    // currency symbols and ISO code, case sensitive
    CurrencyNameStruct* currencySymbols; //ارزښت
    int32_t ټول_Currency_نښه_شمېر;  //شمېر
    // reference count.
    // reference count is set to 1 when an entry is put to cache.
    // it increases by 1 before accessing, and decreased by 1 after accessing.
    // The entry is deleted when ref count is zero, which means 
    // the entry is replaced out of cache and no process is accessing it.
    int32_t ref_شمېر;
} Currency_Name_Cache_ثبت;


#تعریف_کړه CURRENCY_NAME_CACHE_عدد 10

// Reserve 10 cache entries.
ساكن Currency_Name_Cache_ثبت* currCache[CURRENCY_NAME_CACHE_عدد] = {تش_اشاره};
// Using an index to indicate which entry to be replaced when cache is full.
// It is a simple round-robin replacement strategy.
ساكن int8_t current_Cache_ثبت_شاخص = 0;

ساكن UMutex gCurrencyCacheMutex;

// Cache deletion
ساكن فراغ
delete_Currency_نومونه(CurrencyNameStruct* currency_نومونه, int32_t count) {
    لكل (int32_t شاخص = 0; شاخص < count; ++شاخص) {
        إذا ( (currency_نومونه[شاخص].flag & NEED_منزل_BE_DELETED) ) {
            uprv_free(currency_نومونه[شاخص].currencyName);
        }
    }
    uprv_free(currency_نومونه);
}


ساكن فراغ
delete_Cache_ثبت(Currency_Name_Cache_ثبت* ثبت) {
    delete_Currency_نومونه(ثبت->currency_نومونه, ثبت->ټول_Currency_Name_شمېر);
    delete_Currency_نومونه(ثبت->currencySymbols, ثبت->ټول_Currency_نښه_شمېر);
    uprv_free(ثبت);
}


// Cache clean up
ساكن UBool U_CALLCONV
currency_cache_cleanup() {
    لكل (int32_t i = 0; i < CURRENCY_NAME_CACHE_عدد; ++i) {
        إذا (currCache[i]) {
            delete_Cache_ثبت(currCache[i]);
            currCache[i] = تش_اشاره;
        }
    }
    أرجع ریښتیا;
}


/**
 * Loads the currency name data from the cache, or from resource bundles if necessary.
 * The refCount is automatically incremented.  It is the caller's responsibility
 * to decrement it when done!
 */
ساكن Currency_Name_Cache_ثبت*
get_Cache_ثبت(ثابت حرف* locale, UError_کوډ& ec) {

    int32_t ټول_currency_name_شمېر = 0;
    CurrencyNameStruct* currency_نومونه = تش_اشاره;
    int32_t ټول_currency_نښه_شمېر = 0;
    CurrencyNameStruct* currencySymbols = تش_اشاره;
    Currency_Name_Cache_ثبت* cache_ثبت = تش_اشاره;

    umtx_قلف(&gCurrencyCacheMutex);
    // in order to handle racing correctly,
    // not putting 'search' in a separate function.
    int8_t وموندل = -1;
    لكل (int8_t i = 0; i < CURRENCY_NAME_CACHE_عدد; ++i) {
        إذا (currCache[i]!= تش_اشاره &&
            uprv_strcmp(locale, currCache[i]->locale) == 0) {
            وموندل = i;
            اكسر;
        }
    }
    إذا (وموندل != -1) {
        cache_ثبت = currCache[وموندل];
        ++(cache_ثبت->ref_شمېر);
    }
    umtx_unlock(&gCurrencyCacheMutex);
    إذا (وموندل == -1) {
        collect_Currency_نومونه(locale, &currency_نومونه, &ټول_currency_name_شمېر, &currencySymbols, &ټول_currency_نښه_شمېر, ec);
        إذا (U_FAILURE(ec)) {
            أرجع تش_اشاره;
        }
        umtx_قلف(&gCurrencyCacheMutex);
        // check again.
        لكل (int8_t i = 0; i < CURRENCY_NAME_CACHE_عدد; ++i) {
            إذا (currCache[i]!= تش_اشاره &&
                uprv_strcmp(locale, currCache[i]->locale) == 0) {
                وموندل = i;
                اكسر;
            }
        }
        إذا (وموندل == -1) {
            // insert new entry to 
            // currentCacheEntryIndex % CURRENCY_NAME_CACHE_NUM
            // and remove the existing entry 
            // currentCacheEntryIndex % CURRENCY_NAME_CACHE_NUM
            // from cache.
            cache_ثبت = currCache[current_Cache_ثبت_شاخص];
            إذا (cache_ثبت) {
                --(cache_ثبت->ref_شمېر);
                // delete if the ref count is zero
                إذا (cache_ثبت->ref_شمېر == 0) {
                    delete_Cache_ثبت(cache_ثبت);
                }
            }
            cache_ثبت = static_cast<Currency_Name_Cache_ثبت*>(uprv_malloc(حجم(Currency_Name_Cache_ثبت)));
            إذا (cache_ثبت == تش_اشاره) {
                delete_Currency_نومونه(currency_نومونه, ټول_currency_name_شمېر);
                delete_Currency_نومونه(currencySymbols, ټول_currency_نښه_شمېر);
                ec = U_حافظه_ویش_ERROR;
                أرجع تش_اشاره;
            }
            currCache[current_Cache_ثبت_شاخص] = cache_ثبت;
            uprv_strcpy(cache_ثبت->locale, locale);
            cache_ثبت->currency_نومونه = currency_نومونه;
            cache_ثبت->ټول_Currency_Name_شمېر = ټول_currency_name_شمېر;
            cache_ثبت->currencySymbols = currencySymbols;
            cache_ثبت->ټول_Currency_نښه_شمېر = ټول_currency_نښه_شمېر;
            cache_ثبت->ref_شمېر = 2; // one for cache, one for reference
            current_Cache_ثبت_شاخص = (current_Cache_ثبت_شاخص + 1) % CURRENCY_NAME_CACHE_عدد;
            ucln_common_ثبتوونکی_Cleanup(UCLN_COMMON_CURRENCY, currency_cleanup);
        } وإلا {
            delete_Currency_نومونه(currency_نومونه, ټول_currency_name_شمېر);
            delete_Currency_نومونه(currencySymbols, ټول_currency_نښه_شمېر);
            cache_ثبت = currCache[وموندل];
            ++(cache_ثبت->ref_شمېر);
        }
        umtx_unlock(&gCurrencyCacheMutex);
    }

    أرجع cache_ثبت;
}

ساكن فراغ release_Cache_ثبت(Currency_Name_Cache_ثبت* cache_ثبت) {
    umtx_قلف(&gCurrencyCacheMutex);
    --(cache_ثبت->ref_شمېر);
    إذا (cache_ثبت->ref_شمېر == 0) {  // remove
        delete_Cache_ثبت(cache_ثبت);
    }
    umtx_unlock(&gCurrencyCacheMutex);
}

U_CAPI فراغ
uprv_parseCurrency(ثابت حرف* locale,
                   ثابت icu::UnicodeString& متن,
                   icu::ParsePosition& pos,
                   int8_t type,
                   int32_t* partial_سمون_Len,
                   char16_t* پایله,
                   UError_کوډ& ec) {
    U_NAMESPACE_USE
    إذا (U_FAILURE(ec)) {
        أرجع;
    }
    Currency_Name_Cache_ثبت* cache_ثبت = get_Cache_ثبت(locale, ec);
    إذا (U_FAILURE(ec)) {
        أرجع;
    }

    int32_t ټول_currency_name_شمېر = cache_ثبت->ټول_Currency_Name_شمېر;
    CurrencyNameStruct* currency_نومونه = cache_ثبت->currency_نومونه;
    int32_t ټول_currency_نښه_شمېر = cache_ثبت->ټول_Currency_نښه_شمېر;
    CurrencyNameStruct* currencySymbols = cache_ثبت->currencySymbols;

    int32_t پیل = pos.get_شاخص();

    char16_t ننوت_متن[MAX_CURRENCY_NAME_LEN];
    char16_t پاسنی_متن[MAX_CURRENCY_NAME_LEN];
    int32_t متن_Len = MIN(MAX_CURRENCY_NAME_LEN, متن.length() - پیل);
    متن.extract(پیل, متن_Len, ننوت_متن);
    UError_کوډ ec1 = U_ZERO_ERROR;
    متن_Len = u_str_منزل_پاسنی(پاسنی_متن, MAX_CURRENCY_NAME_LEN, ننوت_متن, متن_Len, locale, &ec1);

    // Make sure partialMatchLen is initialized
    *partial_سمون_Len = 0;

    int32_t max = 0;
    int32_t سمون_شاخص = -1;
    // case in-sensitive comparison against currency names
    پلټنه_Currency_Name(currency_نومونه, ټول_currency_name_شمېر, 
                       پاسنی_متن, متن_Len, partial_سمون_Len, &max, &سمون_شاخص);

#که_تعریف_شوی UCURR_DEBUG
    چاپ_کړه("search in names, max = %d, matchIndex = %d\n", max, سمون_شاخص);
#د_که_پای

    int32_t max_دننه_نښه = 0;
    int32_t سمون_شاخص_دننه_نښه = -1;
    إذا (type != UCURR_LONG_NAME) {  // not name only
        // case sensitive comparison against currency symbols and ISO code.
        پلټنه_Currency_Name(currencySymbols, ټول_currency_نښه_شمېر, 
                           ننوت_متن, متن_Len,
                           partial_سمون_Len,
                           &max_دننه_نښه, &سمون_شاخص_دننه_نښه);
    }

#که_تعریف_شوی UCURR_DEBUG
    چاپ_کړه("search in symbols, maxInSymbol = %d, matchIndexInSymbol = %d\n", max_دننه_نښه, سمون_شاخص_دننه_نښه);
    إذا(سمون_شاخص_دننه_نښه != -1) {
      چاپ_کړه("== ISO=%s\n", currencySymbols[سمون_شاخص_دننه_نښه].Iso_کوډ);
    }
#د_که_پای

    إذا (max >= max_دننه_نښه && سمون_شاخص != -1) {
        u_chars_منزل_UChars(currency_نومونه[سمون_شاخص].Iso_کوډ, پایله, 4);
        pos.سېټ_شاخص(پیل + max);
    } وإلا إذا (max_دننه_نښه >= max && سمون_شاخص_دننه_نښه != -1) {
        u_chars_منزل_UChars(currencySymbols[سمون_شاخص_دننه_نښه].Iso_کوډ, پایله, 4);
        pos.سېټ_شاخص(پیل + max_دننه_نښه);
    }

    // decrease reference count
    release_Cache_ثبت(cache_ثبت);
}

فراغ uprv_currencyLeads(ثابت حرف* locale, icu::Unicode_سېټ& پایله, UError_کوډ& ec) {
    U_NAMESPACE_USE
    إذا (U_FAILURE(ec)) {
        أرجع;
    }
    Currency_Name_Cache_ثبت* cache_ثبت = get_Cache_ثبت(locale, ec);
    إذا (U_FAILURE(ec)) {
        أرجع;
    }

    لكل (int32_t i=0; i<cache_ثبت->ټول_Currency_نښه_شمېر; i++) {
        ثابت CurrencyNameStruct& info = cache_ثبت->currencySymbols[i];
        UChar32 cp;
        U16_GET(info.currencyName, 0, 0, info.currencyNameLen, cp);
        پایله.جمع_کړه(cp);
    }

    لكل (int32_t i=0; i<cache_ثبت->ټول_Currency_Name_شمېر; i++) {
        ثابت CurrencyNameStruct& info = cache_ثبت->currency_نومونه[i];
        UChar32 cp;
        U16_GET(info.currencyName, 0, 0, info.currencyNameLen, cp);
        پایله.جمع_کړه(cp);
    }

    // decrease reference count
    release_Cache_ثبت(cache_ثبت);
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
                           icu::UnicodeString& پایله, UError_کوډ& ec)
{
    U_NAMESPACE_USE

    int32_t len;
    ثابت char16_t* currname = ucurr_getName(iso, loc, UCURR_نښه_NAME,
                                          تش_اشاره /* isChoiceFormat */, &len, &ec);
    إذا (U_SUCCESS(ec)) {
        پایله.سېټ_منزل(currname, len);
    }
}

U_CAPI int32_t U_EXPORT2
ucurr_getDefaultFractionDigits(ثابت char16_t* currency, UError_کوډ* ec) {
    أرجع ucurr_getDefaultFractionDigitsForUsage(currency,UCURR_USAGE_STANDARD,ec);
}

U_CAPI int32_t U_EXPORT2
ucurr_getDefaultFractionDigitsForUsage(ثابت char16_t* currency, ثابت UCurrencyUsage usage, UError_کوډ* ec) {
    int32_t fracDigits = 0;
    إذا (U_SUCCESS(*ec)) {
        بدل (usage) {
            حالة UCURR_USAGE_STANDARD:
                fracDigits = (پیدا_کړه_Meta_معلومات(currency, *ec))[0];
                اكسر;
            حالة UCURR_USAGE_CASH:
                fracDigits = (پیدا_کړه_Meta_معلومات(currency, *ec))[2];
                اكسر;
            افتراضي:
                *ec = U_UNSUPPORTED_ERROR;
        }
    }
    أرجع fracDigits;
}

U_CAPI مزدوج U_EXPORT2
ucurr_getRoundingIncrement(ثابت char16_t* currency, UError_کوډ* ec) {
    أرجع ucurr_getRoundingIncrementForUsage(currency, UCURR_USAGE_STANDARD, ec);
}

U_CAPI مزدوج U_EXPORT2
ucurr_getRoundingIncrementForUsage(ثابت char16_t* currency, ثابت UCurrencyUsage usage, UError_کوډ* ec) {
    مزدوج پایله = 0.0;

    ثابت int32_t *معلومات = پیدا_کړه_Meta_معلومات(currency, *ec);
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
                أرجع پایله;
        }

        // If the meta data is invalid, return 0.0
        إذا (fracDigits < 0 || fracDigits > MAX_POW10) {
            *ec = U_INVALID_FORMAT_ERROR;
        } وإلا {
            // A rounding value of 0 or 1 indicates no rounding.
            إذا (increment >= 2) {
                // Return (increment) / 10^(fracDigits).  The only actual rounding data,
                // as of this writing, is CHF { 2, 5 }.
                پایله = مزدوج(increment) / POW10[fracDigits];
            }
        }
    }

    أرجع پایله;
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
    { تش_اشاره, 0 } // Leave here to denote the end of the list.
};

#تعریف_کړه UCURR_MATCHES_BITMASK(variable, type_منزل_سمون) \
    ((type_منزل_سمون) == UCURR_ALL || ((variable) & (type_منزل_سمون)) == (type_منزل_سمون))

ساكن int32_t U_CALLCONV
ucurr_شمېر_Currency_List(UEnumeration *enumerator, UError_کوډ * /*pErrorCode*/) {
    UCurrencyContext *myContext = (UCurrencyContext *)(enumerator->context);
    uint32_t currType = myContext->currType;
    int32_t count = 0;

    /* Count the number of items matching the type we are looking for. */
    لكل (int32_t idx = 0; gCurrencyList[idx].currency != تش_اشاره; idx++) {
        إذا (UCURR_MATCHES_BITMASK(gCurrencyList[idx].currType, currType)) {
            count++;
        }
    }
    أرجع count;
}

ساكن ثابت حرف* U_CALLCONV
ucurr_بل_Currency_List(UEnumeration *enumerator,
                        int32_t* پایله_اوږدتیا,
                        UError_کوډ * /*pErrorCode*/)
{
    UCurrencyContext *myContext = (UCurrencyContext *)(enumerator->context);

    /* Find the next in the list that matches the type we are looking for. */
    طالما (myContext->listIdx < UPRV_LENGTHOF(gCurrencyList)-1) {
        ثابت بنية CurrencyList *curr_توکی = &gCurrencyList[myContext->listIdx++];
        إذا (UCURR_MATCHES_BITMASK(curr_توکی->currType, myContext->currType))
        {
            إذا (پایله_اوږدتیا) {
                *پایله_اوږدتیا = 3; /* Currency codes are only 3 chars long */
            }
            أرجع curr_توکی->currency;
        }
    }
    /* We enumerated too far. */
    إذا (پایله_اوږدتیا) {
        *پایله_اوږدتیا = 0;
    }
    أرجع تش_اشاره;
}

ساكن فراغ U_CALLCONV
ucurr_resetCurrencyList(UEnumeration *enumerator, UError_کوډ * /*pErrorCode*/) {
    ((UCurrencyContext *)(enumerator->context))->listIdx = 0;
}

ساكن فراغ U_CALLCONV
ucurr_closeCurrencyList(UEnumeration *enumerator) {
    uprv_free(enumerator->context);
    uprv_free(enumerator);
}

ساكن فراغ U_CALLCONV
ucurr_createCurrencyList(UHashtable *isoCodes, UError_کوډ* حالت){
    UError_کوډ local_حالت = U_ZERO_ERROR;

    // Look up the CurrencyMap element in the root bundle.
    UResourceBundle *rb = ures_open_مستقیم(U_ICUDATA_CURR, CURRENCY_معلومات, &local_حالت);
    Local_UResource_Bundle_اشاره_ګر currencyMapArray(ures_get_By_کلی(rb, CURRENCY_MAP, rb, &local_حالت));

    إذا (U_SUCCESS(local_حالت)) {
        // process each entry in currency map
        لكل (int32_t i=0; i<ures_get_اندازه(currencyMapArray.getAlias()); i++) {
            // get the currency resource
            Local_UResource_Bundle_اشاره_ګر currencyArray(ures_get_By_شاخص(currencyMapArray.getAlias(), i, تش_اشاره, &local_حالت));
            // process each currency
            إذا (U_SUCCESS(local_حالت)) {
                لكل (int32_t j=0; j<ures_get_اندازه(currencyArray.getAlias()); j++) {
                    // get the currency resource
                    Local_UResource_Bundle_اشاره_ګر currencyRes(ures_get_By_شاخص(currencyArray.getAlias(), j, تش_اشاره, &local_حالت));
                    Iso_کوډ_ثبت *ثبت = (Iso_کوډ_ثبت*)uprv_malloc(حجم(Iso_کوډ_ثبت));
                    إذا (ثبت == تش_اشاره) {
                        *حالت = U_حافظه_ویش_ERROR;
                        أرجع;
                    }

                    // get the ISO code
                    int32_t iso_اوږدتیا = 0;
                    Local_UResource_Bundle_اشاره_ګر idRes(ures_get_By_کلی(currencyRes.getAlias(), "id", تش_اشاره, &local_حالت));
                    إذا (idRes.isNull()) {
                        استمر;
                    }
                    ثابت char16_t *iso_کوډ = ures_getString(idRes.getAlias(), &iso_اوږدتیا, &local_حالت);

                    // get from date
                    UDate fromDate = U_DATE_MIN;
                    Local_UResource_Bundle_اشاره_ګر fromRes(ures_get_By_کلی(currencyRes.getAlias(), "from", تش_اشاره, &local_حالت));

                    إذا (U_SUCCESS(local_حالت)) {
                        int32_t from_اوږدتیا = 0;
                        ثابت int32_t *fromArray = ures_get_Int_ویکټور(fromRes.getAlias(), &from_اوږدتیا, &local_حالت);
                        int64_t currDate64 = ((uint64_t)fromArray[0]) << 32;
                        currDate64 |= ((int64_t)fromArray[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                        fromDate = (UDate)currDate64;
                    }

                    // get to date
                    UDate منزل_Date = U_DATE_MAX;
                    local_حالت = U_ZERO_ERROR;
                    Local_UResource_Bundle_اشاره_ګر منزل_Res(ures_get_By_کلی(currencyRes.getAlias(), "to", تش_اشاره, &local_حالت));

                    إذا (U_SUCCESS(local_حالت)) {
                        int32_t منزل_اوږدتیا = 0;
                        ثابت int32_t *منزل_Array = ures_get_Int_ویکټور(منزل_Res.getAlias(), &منزل_اوږدتیا, &local_حالت);
                        int64_t currDate64 = (uint64_t)منزل_Array[0] << 32;
                        currDate64 |= ((int64_t)منزل_Array[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                        منزل_Date = (UDate)currDate64;
                    }

                    ثبت->iso_کوډ = iso_کوډ;
                    ثبت->from = fromDate;
                    ثبت->منزل = منزل_Date;

                    local_حالت = U_ZERO_ERROR;
                    uhash_put(isoCodes, (char16_t *)iso_کوډ, ثبت, &local_حالت);
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
    تش_اشاره,
    تش_اشاره,
    ucurr_closeCurrencyList,
    ucurr_شمېر_Currency_List,
    uenum_unextDefault,
    ucurr_بل_Currency_List,
    ucurr_resetCurrencyList
};
U_CDECL_پای_ټکی


ساكن فراغ U_CALLCONV initIsoCodes(UError_کوډ &حالت) {
    U_ASSERT(gIsoCodes == تش_اشاره);
    ucln_common_ثبتوونکی_Cleanup(UCLN_COMMON_CURRENCY, currency_cleanup);

    Local_UHashtable_اشاره_ګر isoCodes(uhash_open(uhash_hashUChars, uhash_compareUChars, تش_اشاره, &حالت));
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    uhash_سېټ_ارزښت_Deleter(isoCodes.getAlias(), delete_Iso_کوډ_ثبت);

    ucurr_createCurrencyList(isoCodes.getAlias(), &حالت);
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    gIsoCodes = isoCodes.orphan();  // Note: gIsoCodes is const. Once set up here it is never altered,
                                    //       and read only access is safe without synchronization.
}

ساكن فراغ populateCurrSymbolsEquiv(icu::Hashtable *hash, UError_کوډ &حالت) {
    إذا (U_FAILURE(حالت)) { أرجع; }
    لكل (ثابت خپلکاری& ثبت : unisets::kCurrencyEntries) {
        UnicodeString exemplar(ثبت.exemplar);
        ثابت Unicode_سېټ* سېټ = unisets::get(ثبت.کلی);
        إذا (سېټ == تش_اشاره) { أرجع; }
        Unicode_سېټ_Iterator it(*سېټ);
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
    U_ASSERT(gCurrSymbolsEquiv == تش_اشاره);
    UError_کوډ حالت = U_ZERO_ERROR;
    ucln_common_ثبتوونکی_Cleanup(UCLN_COMMON_CURRENCY, currency_cleanup);
    icu::Hashtable *لنډمهاله = جديد icu::Hashtable(حالت);
    إذا (لنډمهاله == تش_اشاره) {
        أرجع;
    }
    إذا (U_FAILURE(حالت)) {
        ړنګول لنډمهاله;
        أرجع;
    }
    لنډمهاله->سېټ_ارزښت_Deleter(deleteUnicode);
    populateCurrSymbolsEquiv(لنډمهاله, حالت);
    إذا (U_FAILURE(حالت)) {
        ړنګول لنډمهاله;
        أرجع;
    }
    gCurrSymbolsEquiv = لنډمهاله;
}

U_CAPI UBool U_EXPORT2
ucurr_is_شته(ثابت char16_t* iso_کوډ, UDate from, UDate منزل, UError_کوډ* e_Error_کوډ) {
    umtx_initOnce(gIsoCodesInitOnce, &initIsoCodes, *e_Error_کوډ);
    إذا (U_FAILURE(*e_Error_کوډ)) {
        أرجع خطأ;
    }

    Iso_کوډ_ثبت* پایله = (Iso_کوډ_ثبت *) uhash_get(gIsoCodes, iso_کوډ);
    إذا (پایله == تش_اشاره) {
        أرجع خطأ;
    } وإلا إذا (from > منزل) {
        *e_Error_کوډ = U_ILLEGAL_دلیل_ERROR;
        أرجع خطأ;
    } وإلا إذا  ((from > پایله->منزل) || (منزل < پایله->from)) {
        أرجع خطأ;
    }
    أرجع ریښتیا;
}

ساكن ثابت icu::Hashtable* getCurrSymbolsEquiv() {
    umtx_initOnce(gCurrSymbolsEquivInitOnce, &initCurrSymbolsEquiv);
    أرجع gCurrSymbolsEquiv;
}

U_CAPI UEnumeration * U_EXPORT2
ucurr_openISOCurrencies(uint32_t currType, UError_کوډ *p_Error_کوډ) {
    UEnumeration *myEnum = تش_اشاره;
    UCurrencyContext *myContext;

    myEnum = (UEnumeration*)uprv_malloc(حجم(UEnumeration));
    إذا (myEnum == تش_اشاره) {
        *p_Error_کوډ = U_حافظه_ویش_ERROR;
        أرجع تش_اشاره;
    }
    uprv_memcpy(myEnum, &gEnumCurrencyList, حجم(UEnumeration));
    myContext = (UCurrencyContext*)uprv_malloc(حجم(UCurrencyContext));
    إذا (myContext == تش_اشاره) {
        *p_Error_کوډ = U_حافظه_ویش_ERROR;
        uprv_free(myEnum);
        أرجع تش_اشاره;
    }
    myContext->currType = currType;
    myContext->listIdx = 0;
    myEnum->context = myContext;
    أرجع myEnum;
}

U_CAPI int32_t U_EXPORT2
ucurr_شمېر_Currencies(ثابت حرف* locale, 
                 UDate date, 
                 UError_کوډ* ec)
{
    int32_t curr_شمېر = 0;

    إذا (ec != تش_اشاره && U_SUCCESS(*ec)) 
    {
        // local variables
        UError_کوډ local_حالت = U_ZERO_ERROR;

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
        UResourceBundle *rb = ures_open_مستقیم(U_ICUDATA_CURR, CURRENCY_معلومات, &local_حالت);
        UResourceBundle *cm = ures_get_By_کلی(rb, CURRENCY_MAP, rb, &local_حالت);

        // Using the id derived from the local, get the currency data
        Local_UResource_Bundle_اشاره_ګر countryArray(ures_get_By_کلی(rb, id.معلومات(), cm, &local_حالت));

        // process each currency to see which one is valid for the given date
        إذا (U_SUCCESS(local_حالت))
        {
            لكل (int32_t i=0; i<ures_get_اندازه(countryArray.getAlias()); i++)
            {
                // get the currency resource
                Local_UResource_Bundle_اشاره_ګر currencyRes(ures_get_By_شاخص(countryArray.getAlias(), i, تش_اشاره, &local_حالت));

                // get the from date
                int32_t from_اوږدتیا = 0;
                Local_UResource_Bundle_اشاره_ګر fromRes(ures_get_By_کلی(currencyRes.getAlias(), "from", تش_اشاره, &local_حالت));
                ثابت int32_t *fromArray = ures_get_Int_ویکټور(fromRes.getAlias(), &from_اوږدتیا, &local_حالت);

                int64_t currDate64 = (int64_t)((uint64_t)(fromArray[0]) << 32);
                currDate64 |= ((int64_t)fromArray[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                UDate fromDate = (UDate)currDate64;

                إذا (ures_get_اندازه(currencyRes.getAlias())> 2)
                {
                    int32_t منزل_اوږدتیا = 0;
                    Local_UResource_Bundle_اشاره_ګر منزل_Res(ures_get_By_کلی(currencyRes.getAlias(), "to", تش_اشاره, &local_حالت));
                    ثابت int32_t *منزل_Array = ures_get_Int_ویکټور(منزل_Res.getAlias(), &منزل_اوږدتیا, &local_حالت);

                    currDate64 = (int64_t)منزل_Array[0] << 32;
                    currDate64 |= ((int64_t)منزل_Array[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                    UDate منزل_Date = (UDate)currDate64;

                    إذا ((fromDate <= date) && (date < منزل_Date))
                    {
                        curr_شمېر++;
                    }
                }
                وإلا
                {
                    إذا (fromDate <= date)
                    {
                        curr_شمېر++;
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
            أرجع curr_شمېر;
        }
    }

    // If we got here, either error code is invalid or
    // some argument passed is no good.
    أرجع 0;
}

U_CAPI int32_t U_EXPORT2 
ucurr_forLocaleAndDate(ثابت حرف* locale, 
                UDate date, 
                int32_t شاخص,
                char16_t* buff,
                int32_t buff_ظرفیت, 
                UError_کوډ* ec)
{
    int32_t resLen = 0;
	int32_t curr_شاخص = 0;
    ثابت char16_t* s = تش_اشاره;

    إذا (ec != تش_اشاره && U_SUCCESS(*ec))
    {
        // check the arguments passed
        إذا ((buff && buff_ظرفیت) || !buff_ظرفیت )
        {
            // local variables
            UError_کوډ local_حالت = U_ZERO_ERROR;

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
            UResourceBundle *rb = ures_open_مستقیم(U_ICUDATA_CURR, CURRENCY_معلومات, &local_حالت);
            UResourceBundle *cm = ures_get_By_کلی(rb, CURRENCY_MAP, rb, &local_حالت);

            // Using the id derived from the local, get the currency data
            Local_UResource_Bundle_اشاره_ګر countryArray(ures_get_By_کلی(rb, id.معلومات(), cm, &local_حالت));

            // process each currency to see which one is valid for the given date
            منطقي سمون_وموندل = خطأ;
            إذا (U_SUCCESS(local_حالت))
            {
                إذا ((شاخص <= 0) || (شاخص> ures_get_اندازه(countryArray.getAlias())))
                {
                    // requested index is out of bounds
                    أرجع 0;
                }

                لكل (int32_t i=0; i<ures_get_اندازه(countryArray.getAlias()); i++)
                {
                    // get the currency resource
                    Local_UResource_Bundle_اشاره_ګر currencyRes(ures_get_By_شاخص(countryArray.getAlias(), i, تش_اشاره, &local_حالت));
                    s = ures_get_String_By_کلی(currencyRes.getAlias(), "id", &resLen, &local_حالت);

                    // get the from date
                    int32_t from_اوږدتیا = 0;
                    Local_UResource_Bundle_اشاره_ګر fromRes(ures_get_By_کلی(currencyRes.getAlias(), "from", تش_اشاره, &local_حالت));
                    ثابت int32_t *fromArray = ures_get_Int_ویکټور(fromRes.getAlias(), &from_اوږدتیا, &local_حالت);

                    int64_t currDate64 = (int64_t)((uint64_t)fromArray[0] << 32);
                    currDate64 |= ((int64_t)fromArray[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                    UDate fromDate = (UDate)currDate64;

                    إذا (ures_get_اندازه(currencyRes.getAlias()) > 2)
                    {
                        int32_t منزل_اوږدتیا = 0;
                        Local_UResource_Bundle_اشاره_ګر منزل_Res(ures_get_By_کلی(currencyRes.getAlias(), "to", تش_اشاره, &local_حالت));
                        ثابت int32_t *منزل_Array = ures_get_Int_ویکټور(منزل_Res.getAlias(), &منزل_اوږدتیا, &local_حالت);

                        currDate64 = (int64_t)منزل_Array[0] << 32;
                        currDate64 |= ((int64_t)منزل_Array[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                        UDate منزل_Date = (UDate)currDate64;

                        إذا ((fromDate <= date) && (date < منزل_Date))
                        {
                            curr_شاخص++;
                            إذا (curr_شاخص == شاخص)
                            {
                                سمون_وموندل = ریښتیا;
                            }
                        }
                    }
                    وإلا
                    {
                        إذا (fromDate <= date)
                        {
                            curr_شاخص++;
                            إذا (curr_شاخص == شاخص)
                            {
                                سمون_وموندل = ریښتیا;
                            }
                        }
                    }
                    // check for loop exit
                    إذا (سمون_وموندل)
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
                إذا((buff_ظرفیت> resLen) && سمون_وموندل)
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
            أرجع u_terminateUChars(buff, buff_ظرفیت, resLen, ec);
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

ساكن ثابت UEnumeration default_Keyword_ارزښتونه = {
    تش_اشاره,
    تش_اشاره,
    ulist_close_keyword_ارزښتونه_iterator,
    ulist_شمېر_keyword_ارزښتونه,
    uenum_unextDefault,
    ulist_بل_keyword_ارزښت, 
    ulist_reset_keyword_ارزښتونه_iterator
};

U_CAPI UEnumeration *U_EXPORT2 ucurr_get_Keyword_ارزښتونه_For_Locale(ثابت حرف *کلی, ثابت حرف *locale, UBool commonly_کارول_شوی, UError_کوډ* حالت) {
    // Resolve region
    CharString prefRegion = ulocimp_get_Region_For_Supplemental_معلومات(locale, ریښتیا, *حالت);

    // Read value from supplementalData
    UList *ارزښتونه = ulist_create_تشه_List(حالت);
    UList *other_ارزښتونه = ulist_create_تشه_List(حالت);
    UEnumeration *en = (UEnumeration *)uprv_malloc(حجم(UEnumeration));
    إذا (U_FAILURE(*حالت) || en == تش_اشاره) {
        إذا (en == تش_اشاره) {
            *حالت = U_حافظه_ویش_ERROR;
        } وإلا {
            uprv_free(en);
        }
        ulist_deleteList(ارزښتونه);
        ulist_deleteList(other_ارزښتونه);
        أرجع تش_اشاره;
    }
    حافظه_نقل(en, &default_Keyword_ارزښتونه, حجم(UEnumeration));
    en->context = ارزښتونه;
    
    UResourceBundle* rb = ures_open_مستقیم(U_ICUDATA_CURR, "supplementalData", حالت);
    Local_UResource_Bundle_اشاره_ګر bundle(ures_get_By_کلی(rb, "CurrencyMap", rb, حالت));
    ډېری_UResource_Bundle bundlekey, regbndl, curbndl, منزل;
    
    طالما (U_SUCCESS(*حالت) && ures_has_بل(bundle.getAlias())) {
        ures_get_بل_Resource(bundle.getAlias(), bundlekey.getAlias(), حالت);
        إذا (U_FAILURE(*حالت)) {
            اكسر;
        }
        ثابت حرف *region = ures_get_کلی(bundlekey.getAlias());
        UBool isPrefRegion = prefRegion == region;
        إذا (!isPrefRegion && commonly_کارول_شوی) {
            // With commonlyUsed=true, we do not put
            // currencies for other regions in the
            // result list.
            استمر;
        }
        ures_get_By_کلی(bundle.getAlias(), region, regbndl.getAlias(), حالت);
        إذا (U_FAILURE(*حالت)) {
            اكسر;
        }
        طالما (U_SUCCESS(*حالت) && ures_has_بل(regbndl.getAlias())) {
            ures_get_بل_Resource(regbndl.getAlias(), curbndl.getAlias(), حالت);
            إذا (ures_getType(curbndl.getAlias()) != URES_جدول) {
                // Currently, an empty ARRAY is mixed in.
                استمر;
            }
            حرف *curID = (حرف *)uprv_malloc(حجم(حرف) * ULOC_KEYWORDS_ظرفیت);
            إذا (curID == تش_اشاره) {
                *حالت = U_حافظه_ویش_ERROR;
                اكسر;
            }
            int32_t curIDLength = ULOC_KEYWORDS_ظرفیت;

#إذا U_CHARSET_FAMILY==U_ASCII_FAMILY
            ures_get_UTF8_String_By_کلی(curbndl.getAlias(), "id", curID, &curIDLength, ریښتیا, حالت);
            /* optimize - use the utf-8 string */
#وإلا
            {
                       ثابت char16_t* defString = ures_get_String_By_کلی(curbndl.getAlias(), "id", &curIDLength, حالت);
                       إذا(U_SUCCESS(*حالت)) {
			   إذا(curIDLength+1 > ULOC_KEYWORDS_ظرفیت) {
				*حالت = U_بفر_ډکېدنه_ERROR;
			   } وإلا {
                           	u_UChars_منزل_Chars(defString, curID, curIDLength+1);
			   }
                       }
            }
#د_که_پای	

            إذا (U_FAILURE(*حالت)) {
                اكسر;
            }
            UBool has_منزل = خطأ;
            ures_get_By_کلی(curbndl.getAlias(), "to", منزل.getAlias(), حالت);
            إذا (U_FAILURE(*حالت)) {
                // Do nothing here...
                *حالت = U_ZERO_ERROR;
            } وإلا {
                has_منزل = ریښتیا;
            }
            إذا (isPrefRegion && !has_منزل && !ulist_لري_String(ارزښتونه, curID, (int32_t)uprv_strlen(curID))) {
                // Currently active currency for the target country
                ulist_جمع_کړه_توکی_پای_ټکی_List(ارزښتونه, curID, ریښتیا, حالت);
            } وإلا إذا (!ulist_لري_String(other_ارزښتونه, curID, (int32_t)uprv_strlen(curID)) && !commonly_کارول_شوی) {
                ulist_جمع_کړه_توکی_پای_ټکی_List(other_ارزښتونه, curID, ریښتیا, حالت);
            } وإلا {
                uprv_free(curID);
            }
        }
        
    }
    إذا (U_SUCCESS(*حالت)) {
        إذا (commonly_کارول_شوی) {
            إذا (ulist_get_List_اندازه(ارزښتونه) == 0) {
                // This could happen if no valid region is supplied in the input
                // locale. In this case, we use the CLDR's default.
                uenum_close(en);
                en = ucurr_get_Keyword_ارزښتونه_For_Locale(کلی, "und", ریښتیا, حالت);
            }
        } وإلا {
            // Consolidate the list
            حرف *value = تش_اشاره;
            ulist_resetList(other_ارزښتونه);
            طالما ((value = (حرف *)ulist_get_بل(other_ارزښتونه)) != تش_اشاره) {
                إذا (!ulist_لري_String(ارزښتونه, value, (int32_t)uprv_strlen(value))) {
                    حرف *tmp_ارزښت = (حرف *)uprv_malloc(حجم(حرف) * ULOC_KEYWORDS_ظرفیت);
                    إذا (tmp_ارزښت == تش_اشاره) {
                        *حالت = U_حافظه_ویش_ERROR;
                        اكسر;
                    }
                    uprv_memcpy(tmp_ارزښت, value, uprv_strlen(value) + 1);
                    ulist_جمع_کړه_توکی_پای_ټکی_List(ارزښتونه, tmp_ارزښت, ریښتیا, حالت);
                    إذا (U_FAILURE(*حالت)) {
                        اكسر;
                    }
                }
            }
        }
        ulist_resetList((UList *)(en->context));
    } وإلا {
        ulist_deleteList(ارزښتونه);
        uprv_free(en);
        ارزښتونه = تش_اشاره;
        en = تش_اشاره;
    }
    ulist_deleteList(other_ارزښتونه);
    أرجع en;
}


U_CAPI int32_t U_EXPORT2
ucurr_get_Numeric_کوډ(ثابت char16_t* currency) {
    int32_t کوډ = 0;
    إذا (currency && u_strlen(currency) == ISO_CURRENCY_کوډ_اوږدتیا) {
        UError_کوډ حالت = U_ZERO_ERROR;

        UResourceBundle* bundle = ures_open_مستقیم(تش_اشاره, "currencyNumericCodes", &حالت);
        Local_UResource_Bundle_اشاره_ګر کوډ_Map(ures_get_By_کلی(bundle, "codeMap", bundle, &حالت));
        إذا (U_SUCCESS(حالت)) {
            حرف alpha_کوډ[ISO_CURRENCY_کوډ_اوږدتیا+1];
            my_UChars_منزل_Chars(alpha_کوډ, currency);
            T_CString_منزل_پاسنی_Case(alpha_کوډ);
            ures_get_By_کلی(کوډ_Map.getAlias(), alpha_کوډ, کوډ_Map.getAlias(), &حالت);
            صحيح tmp_کوډ = ures_getInt(کوډ_Map.getAlias(), &حالت);
            إذا (U_SUCCESS(حالت)) {
                کوډ = tmp_کوډ;
            }
        }
    }
    أرجع کوډ;
}
#د_که_پای /* #if !UCONFIG_NO_FORMATTING */

//eof
