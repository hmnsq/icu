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

استفاده نطاق icu;

//#define UCURR_DEBUG_EQUIV 1
#اگر_تعریف_شده UCURR_DEBUG_EQUIV
#ضمن "stdio.h"
#پایان_اگر
//#define UCURR_DEBUG 1
#اگر_تعریف_شده UCURR_DEBUG
#ضمن "stdio.h"
#پایان_اگر

تعريفنوع بنية Iso_کد_مدخل {
    ثابت char16_t *iso_کد; /* const because it's a reference to a resource bundle string. */
    UDate from;
    UDate مقصد;
} Iso_کد_مدخل;

//------------------------------------------------------------
// Constants

// Default currency meta data of last resort.  We try to use the
// defaults encoded in the meta data resource bundle.  If there is a
// configuration/build error and these are not available, we use these
// hard-coded defaults (which should be identical).
ساكن ثابت int32_t آخرین_RESORT_داده[] = { 2, 0, 2, 0 };

// POW10[i] = 10^i, i=0..MAX_POW10
ساكن ثابت int32_t POW10[] = { 1, 10, 100, 1000, 10000, 100000,
                                 1000000, 10000000, 100000000, 1000000000 };

ساكن ثابت int32_t MAX_POW10 = UPRV_LENGTHOF(POW10) - 1;

#تعریف_کن ISO_CURRENCY_کد_درازی 3

//------------------------------------------------------------
// Resource tags
//

ساكن ثابت حرف CURRENCY_داده[] = "supplementalData";
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
ساكن ثابت UHashtable* gIsoCodes = اشاره_تهی;
ساكن icu::UInitOnce gIsoCodesInitOnce {};

// Currency symbol equivalances
ساكن ثابت icu::Hashtable* gCurrSymbolsEquiv = اشاره_تهی;
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
    ثابت icu::UnicodeString* بعدی_2 = static_cast<ثابت icu::UnicodeString*>(_hash.get(*_current));
    إذا (بعدی_2 == اشاره_تهی) {
        U_ASSERT(_current == شروع_2);
        أرجع اشاره_تهی;
    }
    إذا (*بعدی_2 == *شروع_2) {
        أرجع اشاره_تهی;
    }
    _current = بعدی_2;
    أرجع بعدی_2;
}

U_NAMESPACE_انتها

// makeEquivalent makes lhs and rhs equivalent by updating the equivalence
// relations in hash accordingly.
ساكن فراغ makeEquivalent(
    ثابت icu::UnicodeString &lhs,
    ثابت icu::UnicodeString &rhs,
    icu::Hashtable* hash, UError_کد &وضعیت) {
    إذا (U_FAILURE(وضعیت)) {
        أرجع;
    }
    إذا (lhs == rhs) {
        // already equivalent
        أرجع;
    }
    icu::EquivIterator چپ_Iter(*hash, lhs);
    icu::EquivIterator راست_Iter(*hash, rhs);
    ثابت icu::UnicodeString *اولی_چپ = چپ_Iter.next();
    ثابت icu::UnicodeString *اولی_راست = راست_Iter.next();
    ثابت icu::UnicodeString *بعدی_چپ = اولی_چپ;
    ثابت icu::UnicodeString *بعدی_راست = اولی_راست;
    طالما (بعدی_چپ != اشاره_تهی && بعدی_راست != اشاره_تهی) {
        إذا (*بعدی_چپ == rhs || *بعدی_راست == lhs) {
            // Already equivalent
            أرجع;
        }
        بعدی_چپ = چپ_Iter.next();
        بعدی_راست = راست_Iter.next();
    }
    // Not equivalent. Must join.
    icu::UnicodeString *new_اولی_چپ;
    icu::UnicodeString *new_اولی_راست;
    إذا (اولی_راست == اشاره_تهی && اولی_چپ == اشاره_تهی) {
        // Neither lhs or rhs belong to an equivalence circle, so we form
        // a new equivalnce circle of just lhs and rhs.
        new_اولی_چپ = جديد icu::UnicodeString(rhs);
        new_اولی_راست = جديد icu::UnicodeString(lhs);
    } وإلا إذا (اولی_راست == اشاره_تهی) {
        // lhs belongs to an equivalence circle, but rhs does not, so we link
        // rhs into lhs' circle.
        new_اولی_چپ = جديد icu::UnicodeString(rhs);
        new_اولی_راست = جديد icu::UnicodeString(*اولی_چپ);
    } وإلا إذا (اولی_چپ == اشاره_تهی) {
        // rhs belongs to an equivlance circle, but lhs does not, so we link
        // lhs into rhs' circle.
        new_اولی_چپ = جديد icu::UnicodeString(*اولی_راست);
        new_اولی_راست = جديد icu::UnicodeString(lhs);
    } وإلا {
        // Both lhs and rhs belong to different equivalnce circles. We link
        // them together to form one single, larger equivalnce circle.
        new_اولی_چپ = جديد icu::UnicodeString(*اولی_راست);
        new_اولی_راست = جديد icu::UnicodeString(*اولی_چپ);
    }
    إذا (new_اولی_چپ == اشاره_تهی || new_اولی_راست == اشاره_تهی) {
        حذف new_اولی_چپ;
        حذف new_اولی_راست;
        وضعیت = U_حافظه_تخصیص_ERROR;
        أرجع;
    }
    hash->put(lhs, (فراغ *) new_اولی_چپ, وضعیت);
    hash->put(rhs, (فراغ *) new_اولی_راست, وضعیت);
}

// countEquivalent counts how many strings are equivalent to s.
// hash stores all the equivalnce relations.
// countEquivalent does not include s itself in the count.
ساكن int32_t شمار_Equivalent(ثابت icu::Hashtable &hash, ثابت icu::UnicodeString &s) {
    int32_t نتیجه = 0;
    icu::EquivIterator iter(hash, s);
    طالما (iter.next() != اشاره_تهی) {
        ++نتیجه;
    }
#اگر_تعریف_شده UCURR_DEBUG_EQUIV
 {
   حرف tmp[200];
   s.extract(0,s.length(),tmp, "UTF-8");
   چاپ_کن("CountEquivalent('%s') = %d\n", tmp, نتیجه);
 }
#پایان_اگر
    أرجع نتیجه;
}

ساكن ثابت icu::Hashtable* getCurrSymbolsEquiv();

//------------------------------------------------------------
//کد

/**
 * Cleanup callback func
 */
ساكن UBool U_CALLCONV 
isoCodes_cleanup()
{
    إذا (gIsoCodes != اشاره_تهی) {
        uhash_close(const_cast<UHashtable *>(gIsoCodes));
        gIsoCodes = اشاره_تهی;
    }
    gIsoCodesInitOnce.reset();
    أرجع درست;
}

/**
 * Cleanup callback func
 */
ساكن UBool U_CALLCONV 
currSymbolsEquiv_cleanup()
{
    حذف const_cast<icu::Hashtable *>(gCurrSymbolsEquiv);
    gCurrSymbolsEquiv = اشاره_تهی;
    gCurrSymbolsEquivInitOnce.reset();
    أرجع درست;
}

/**
 * Deleter for IsoCodeEntry
 */
ساكن فراغ U_CALLCONV
delete_Iso_کد_مدخل(فراغ *obj) {
    Iso_کد_مدخل* مدخل = static_cast<Iso_کد_مدخل*>(obj);
    uprv_free(مدخل);
}

/**
 * Deleter for gCurrSymbolsEquiv.
 */
ساكن فراغ U_CALLCONV
deleteUnicode(فراغ *obj) {
    icu::UnicodeString* مدخل = static_cast<icu::UnicodeString*>(obj);
    حذف مدخل;
}

/**
 * Unfortunately, we have to convert the char16_t* currency code to char*
 * to use it as a resource key.
 */
ساكن مضمن حرف*
my_UChars_مقصد_Chars(حرف* نتیجه_از_Len4, ثابت char16_t* currency) {
    u_UChars_مقصد_Chars(currency, نتیجه_از_Len4, ISO_CURRENCY_کد_درازی);
    نتیجه_از_Len4[ISO_CURRENCY_کد_درازی] = 0;
    أرجع نتیجه_از_Len4;
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
پیدا_کن_Meta_داده(ثابت char16_t* currency, UError_کد& ec) {

    إذا (currency == اشاره_تهی || *currency == 0) {
        إذا (U_SUCCESS(ec)) {
            ec = U_ILLEGAL_آرگومان_ERROR;
        }
        أرجع آخرین_RESORT_داده;
    }

    // Get CurrencyMeta resource out of root locale file.  [This may
    // move out of the root locale file later; if it does, update this
    // code.]
    UResourceBundle* currency_داده = ures_open_مستقیم(U_ICUDATA_CURR, CURRENCY_داده, &ec);
    Local_UResource_Bundle_اشاره_گر currencyMeta(ures_get_By_کلید(currency_داده, CURRENCY_META, currency_داده, &ec));

    إذا (U_FAILURE(ec)) {
        // Config/build error; return hard-coded defaults
        أرجع آخرین_RESORT_داده;
    }

    // Look up our currency, or if that's not available, then DEFAULT
    حرف buf[ISO_CURRENCY_کد_درازی+1];
    UError_کد ec2 = U_ZERO_ERROR; // local error code: soft failure
    Local_UResource_Bundle_اشاره_گر rb(ures_get_By_کلید(currencyMeta.getAlias(), my_UChars_مقصد_Chars(buf, currency), اشاره_تهی, &ec2));
      إذا (U_FAILURE(ec2)) {
        rb.adoptInstead(ures_get_By_کلید(currencyMeta.getAlias(),DEFAULT_META, اشاره_تهی, &ec));
        إذا (U_FAILURE(ec)) {
            // Config/build error; return hard-coded defaults
            أرجع آخرین_RESORT_داده;
        }
    }

    int32_t len;
    ثابت int32_t *داده = ures_get_Int_وکتور(rb.getAlias(), &len, &ec);
    إذا (U_FAILURE(ec) || len != 4) {
        // Config/build error; return hard-coded defaults
        إذا (U_SUCCESS(ec)) {
            ec = U_INVALID_FORMAT_ERROR;
        }
        أرجع آخرین_RESORT_داده;
    }

    أرجع داده;
}

// -------------------------------------

ساكن CharString
idForLocale(ثابت حرف* locale, UError_کد* ec)
{
    أرجع ulocimp_get_Region_For_Supplemental_داده(locale, خطأ, *ec);
}

// ------------------------------------------
//
// Registration
//
//-------------------------------------------

// don't use ICUService since we don't need fallback

U_CDECL_BEGIN
ساكن UBool U_CALLCONV currency_cleanup();
U_CDECL_انتها

#إذا !UCONFIG_NO_SERVICE
بنية CReg;

ساكن UMutex g_CReg_قفل;
ساكن CReg* g_CReg_سر = اشاره_تهی;

بنية CReg : عام icu::UMemory {
    CReg *next;
    char16_t iso[ISO_CURRENCY_کد_درازی+1];
    حرف  id[ULOC_FULLNAME_ظرفیت];

    CReg(ثابت char16_t* _iso, ثابت حرف* _id)
        : next(اشاره_تهی)
    {
        uprv_strncpy(id, _id, حجم(id)-1);
        id[حجم(id)-1] = 0;
        u_memcpy(iso, _iso, ISO_CURRENCY_کد_درازی);
        iso[ISO_CURRENCY_کد_درازی] = 0;
    }

    ساكن UCurr_Registry_کلید reg(ثابت char16_t* _iso, ثابت حرف* _id, UError_کد* وضعیت)
    {
        إذا (وضعیت && U_SUCCESS(*وضعیت) && _iso && _id) {
            CReg* n = جديد CReg(_iso, _id);
            إذا (n) {
                umtx_قفل(&g_CReg_قفل);
                إذا (!g_CReg_سر) {
                    /* register for the first time */
                    ucln_common_ثبات_Cleanup(UCLN_COMMON_CURRENCY, currency_cleanup);
                }
                n->next = g_CReg_سر;
                g_CReg_سر = n;
                umtx_unlock(&g_CReg_قفل);
                أرجع n;
            }
            *وضعیت = U_حافظه_تخصیص_ERROR;
        }
        أرجع اشاره_تهی;
    }

    ساكن UBool unreg(UCurr_Registry_کلید کلید) {
        UBool یافت = خطأ;
        umtx_قفل(&g_CReg_قفل);

        CReg** p = &g_CReg_سر;
        طالما (*p) {
            إذا (*p == کلید) {
                *p = ((CReg*)کلید)->next;
                حذف (CReg*)کلید;
                یافت = درست;
                اكسر;
            }
            p = &((*p)->next);
        }

        umtx_unlock(&g_CReg_قفل);
        أرجع یافت;
    }

    ساكن ثابت char16_t* get(ثابت حرف* id) {
        ثابت char16_t* نتیجه = اشاره_تهی;
        umtx_قفل(&g_CReg_قفل);
        CReg* p = g_CReg_سر;

        /* register cleanup of the mutex */
        ucln_common_ثبات_Cleanup(UCLN_COMMON_CURRENCY, currency_cleanup);
        طالما (p) {
            إذا (uprv_strcmp(id, p->id) == 0) {
                نتیجه = p->iso;
                اكسر;
            }
            p = p->next;
        }
        umtx_unlock(&g_CReg_قفل);
        أرجع نتیجه;
    }

    /* This doesn't need to be thread safe. It's for u_cleanup only. */
    ساكن فراغ cleanup() {
        طالما (g_CReg_سر) {
            CReg* n = g_CReg_سر;
            g_CReg_سر = g_CReg_سر->next;
            حذف n;
        }
    }
};

// -------------------------------------

U_CAPI UCurr_Registry_کلید U_EXPORT2
ucurr_ثبات(ثابت char16_t* iso_کد, ثابت حرف* locale, UError_کد *وضعیت)
{
    إذا (وضعیت && U_SUCCESS(*وضعیت)) {
        CharString id = idForLocale(locale, وضعیت);
        أرجع CReg::reg(iso_کد, id.داده(), وضعیت);
    }
    أرجع اشاره_تهی;
}

// -------------------------------------

U_CAPI UBool U_EXPORT2
ucurr_unregister(UCurr_Registry_کلید کلید, UError_کد* وضعیت)
{
    إذا (وضعیت && U_SUCCESS(*وضعیت)) {
        أرجع CReg::unreg(کلید);
    }
    أرجع خطأ;
}
#پایان_اگر /* UCONFIG_NO_SERVICE */

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
#پایان_اگر
    /*
     * There might be some cached currency data or isoCodes data.
     */
    currency_cache_cleanup();
    isoCodes_cleanup();
    currSymbolsEquiv_cleanup();

    أرجع درست;
}
U_CDECL_انتها

// -------------------------------------

U_CAPI int32_t U_EXPORT2
ucurr_forLocale(ثابت حرف* locale,
                char16_t* buff,
                int32_t buff_ظرفیت,
                UError_کد* ec) {
    إذا (U_FAILURE(*ec)) { أرجع 0; }
    إذا (buff_ظرفیت < 0 || (buff == اشاره_تهی && buff_ظرفیت > 0)) {
        *ec = U_ILLEGAL_آرگومان_ERROR;
        أرجع 0;
    }

    UError_کد local_وضعیت = U_ZERO_ERROR;
    CharString currency = ulocimp_get_Keyword_مقدار(locale, "currency", local_وضعیت);
    int32_t resLen = currency.length();

    إذا (U_SUCCESS(local_وضعیت) && resLen == 3 && uprv_isInvariantString(currency.داده(), resLen)) {
        إذا (resLen < buff_ظرفیت) {
            T_CString_مقصد_فوقانی_Case(currency.داده());
            u_chars_مقصد_UChars(currency.داده(), buff, resLen);
        }
        أرجع u_terminateUChars(buff, buff_ظرفیت, resLen, ec);
    }

    // get country or country_variant in `id'
    CharString id = idForLocale(locale, ec);
    إذا (U_FAILURE(*ec)) {
        أرجع 0;
    }

#إذا !UCONFIG_NO_SERVICE
    ثابت char16_t* نتیجه = CReg::get(id.داده());
    إذا (نتیجه) {
        إذا(buff_ظرفیت > u_strlen(نتیجه)) {
            u_strcpy(buff, نتیجه);
        }
        resLen = u_strlen(نتیجه);
        أرجع u_terminateUChars(buff, buff_ظرفیت, resLen, ec);
    }
#پایان_اگر
    // Remove variants, which is only needed for registration.
    حرف *idDelim = uprv_strchr(id.داده(), VAR_DELIM);
    إذا (idDelim) {
        id.truncate(idDelim - id.داده());
    }

    ثابت char16_t* s = اشاره_تهی;  // Currency code from data file.
    إذا (id.is_خالی()) {
        // No point looking in the data for an empty string.
        // This is what we would get.
        local_وضعیت = U_MISSING_RESOURCE_ERROR;
    } وإلا {
        // Look up the CurrencyMap element in the root bundle.
        local_وضعیت = U_ZERO_ERROR;
        UResourceBundle *rb = ures_open_مستقیم(U_ICUDATA_CURR, CURRENCY_داده, &local_وضعیت);
        UResourceBundle *cm = ures_get_By_کلید(rb, CURRENCY_MAP, rb, &local_وضعیت);
        Local_UResource_Bundle_اشاره_گر countryArray(ures_get_By_کلید(rb, id.داده(), cm, &local_وضعیت));
        // https://unicode-org.atlassian.net/browse/ICU-21997
        // Prefer to use currencies that are legal tender.
        إذا (U_SUCCESS(local_وضعیت)) {
            int32_t array_درازی = ures_get_اندازه(countryArray.getAlias());
            لكل (int32_t i = 0; i < array_درازی; ++i) {
                Local_UResource_Bundle_اشاره_گر currencyReq(
                    ures_get_By_شاخص(countryArray.getAlias(), i, اشاره_تهی, &local_وضعیت));
                // The currency is legal tender if it is *not* marked with tender{"false"}.
                UError_کد tender_وضعیت = local_وضعیت;
                ثابت char16_t *tender =
                    ures_get_String_By_کلید(currencyReq.getAlias(), "tender", اشاره_تهی, &tender_وضعیت);
                منطقي isTender = U_FAILURE(tender_وضعیت) || u_strcmp(tender, u"false") != 0;
                إذا (!isTender && s != اشاره_تهی) {
                    // We already have a non-tender currency. Ignore all following non-tender ones.
                    استمر;
                }
                // Fetch the currency code.
                s = ures_get_String_By_کلید(currencyReq.getAlias(), "id", &resLen, &local_وضعیت);
                إذا (isTender) {
                    اكسر;
                }
            }
            إذا (U_SUCCESS(local_وضعیت) && s == اشاره_تهی) {
                local_وضعیت = U_MISSING_RESOURCE_ERROR;
            }
        }
    }

    إذا ((U_FAILURE(local_وضعیت)) && strchr(id.داده(), '_') != اشاره_تهی) {
        // We don't know about it.  Check to see if we support the variant.
        CharString والد = ulocimp_get_والد(locale, *ec);
        *ec = U_USING_FALLBACK_WARNING;
        // TODO: Loop over the parent rather than recursing and
        // looking again for a currency keyword.
        أرجع ucurr_forLocale(والد.داده(), buff, buff_ظرفیت, ec);
    }
    إذا (*ec == U_ZERO_ERROR || local_وضعیت != U_ZERO_ERROR) {
        // There is nothing to fallback to. Report the failure/warning if possible.
        *ec = local_وضعیت;
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
    إذا (loc.is_خالی()) {
        أرجع خطأ;
    }
    UError_کد وضعیت = U_ZERO_ERROR;
    إذا (loc == "en_GB") {
        // HACK: See #13368.  We need "en_GB" to fall back to "en_001" instead of "en"
        // in order to consume the correct data strings.  This hack will be removed
        // when proper data sink loading is implemented here.
        loc.truncate(3);
        loc.append("001", وضعیت);
    } وإلا {
        loc = ulocimp_get_والد(loc.داده(), وضعیت);
    }
 /*
    char *i = uprv_strrchr(loc, '_');
    if (i == nullptr) {
        i = loc;
    }
    *i = 0;
 */
    أرجع درست;
}


U_CAPI ثابت char16_t* U_EXPORT2
ucurr_getName(ثابت char16_t* currency U_LIFETIME_BOUND,
              ثابت حرف* locale,
              UCurrNameStyle nameStyle,
              UBool* isChoiceFormat, // fillin
              int32_t* len, // fillin
              UError_کد* ec) {

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
        أرجع اشاره_تهی;
    }

    int32_t choice = (int32_t) nameStyle;
    إذا (choice < 0 || choice > 4) {
        *ec = U_ILLEGAL_آرگومان_ERROR;
        أرجع اشاره_تهی;
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
    UError_کد ec2 = U_ZERO_ERROR;

    إذا (locale == اشاره_تهی) {
        locale = uloc_getDefault();
    }
    CharString loc = ulocimp_getName(locale, ec2);
    إذا (U_FAILURE(ec2)) {
        *ec = U_ILLEGAL_آرگومان_ERROR;
        أرجع اشاره_تهی;
    }

    حرف buf[ISO_CURRENCY_کد_درازی+1];
    my_UChars_مقصد_Chars(buf, currency);
    
    /* Normalize the keyword value to uppercase */
    T_CString_مقصد_فوقانی_Case(buf);
    
    ثابت char16_t* s = اشاره_تهی;
    ec2 = U_ZERO_ERROR;
    Local_UResource_Bundle_اشاره_گر rb(ures_open(U_ICUDATA_CURR, loc.داده(), &ec2));

    إذا (nameStyle == UCURR_NARROW_نماد_NAME || nameStyle == UCURR_FORMAL_نماد_NAME || nameStyle == UCURR_VARIANT_نماد_NAME) {
        CharString کلید;
        بدل (nameStyle) {
        حالة UCURR_NARROW_نماد_NAME:
            کلید.append(CURRENCIES_NARROW, ec2);
            اكسر;
        حالة UCURR_FORMAL_نماد_NAME:
            کلید.append(CURRENCIES_FORMAL, ec2);
            اكسر;
        حالة UCURR_VARIANT_نماد_NAME:
            کلید.append(CURRENCIES_VARIANT, ec2);
            اكسر;
        افتراضي:
            *ec = U_UNSUPPORTED_ERROR;
            أرجع اشاره_تهی;
        }
        کلید.append("/", ec2);
        کلید.append(buf, ec2);
        s = ures_get_String_By_کلید_With_Fallback(rb.getAlias(), کلید.داده(), len, &ec2);
        إذا (ec2 == U_MISSING_RESOURCE_ERROR) {
            *ec = U_USING_FALLBACK_WARNING;
            ec2 = U_ZERO_ERROR;
            choice = UCURR_نماد_NAME;
        }
    }
    إذا (s == اشاره_تهی) {
        ures_get_By_کلید(rb.getAlias(), CURRENCIES, rb.getAlias(), &ec2);
        ures_get_By_کلید_With_Fallback(rb.getAlias(), buf, rb.getAlias(), &ec2);
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
    إذا (isChoiceFormat != اشاره_تهی) {
        *isChoiceFormat = خطأ;
    }
    إذا (U_SUCCESS(ec2)) {
        U_ASSERT(s != اشاره_تهی);
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
                    ثابت حرف* plural_شمار,
                    int32_t* len, // fillin
                    UError_کد* ec) {
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
        أرجع اشاره_تهی;
    }

    // Use a separate UErrorCode here that does not propagate out of
    // this function.
    UError_کد ec2 = U_ZERO_ERROR;

    إذا (locale == اشاره_تهی) {
        locale = uloc_getDefault();
    }
    CharString loc = ulocimp_getName(locale, ec2);
    إذا (U_FAILURE(ec2)) {
        *ec = U_ILLEGAL_آرگومان_ERROR;
        أرجع اشاره_تهی;
    }

    حرف buf[ISO_CURRENCY_کد_درازی+1];
    my_UChars_مقصد_Chars(buf, currency);

    ثابت char16_t* s = اشاره_تهی;
    ec2 = U_ZERO_ERROR;
    UResourceBundle* rb = ures_open(U_ICUDATA_CURR, loc.داده(), &ec2);

    rb = ures_get_By_کلید(rb, CURRENCYPLURALS, rb, &ec2);

    // Fetch resource with multi-level resource inheritance fallback
    Local_UResource_Bundle_اشاره_گر curr(ures_get_By_کلید_With_Fallback(rb, buf, rb, &ec2));

    s = ures_get_String_By_کلید_With_Fallback(curr.getAlias(), plural_شمار, len, &ec2);
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
        U_ASSERT(s != اشاره_تهی);
        أرجع s;
    }

    // If we fail to find a match, use the ISO 4217 code
    *len = u_strlen(currency); // Should == ISO_CURRENCY_CODE_LENGTH, but maybe not...?
    *ec = U_USING_DEFAULT_WARNING;
    أرجع currency;
}


//========================================================================
// Following are structure and function for parsing currency names

#تعریف_کن NEED_مقصد_BE_DELETED 0x1

// TODO: a better way to define this?
#تعریف_کن MAX_CURRENCY_NAME_LEN 100

تعريفنوع بنية {
    ثابت حرف* Iso_کد;  //کلید
    char16_t* currencyName;  //مقدار
    int32_t currencyNameLen;  // value length
    int32_t flag;  // flags
} CurrencyNameStruct;


#اگر_تعریف_نشده MIN
#تعریف_کن MIN(a,b) (((a)<(b)) ? (a) : (b))
#پایان_اگر

#اگر_تعریف_نشده MAX
#تعریف_کن MAX(a,b) (((a)<(b)) ? (b) : (a))
#پایان_اگر


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
get_Currency_Name_شمار(ثابت حرف* loc, int32_t* مجموع_currency_name_شمار, int32_t* مجموع_currency_نماد_شمار) {
    U_NAMESPACE_USE
    *مجموع_currency_name_شمار = 0;
    *مجموع_currency_نماد_شمار = 0;
    ثابت char16_t* s = اشاره_تهی;
    CharString locale;
    {
        UError_کد وضعیت = U_ZERO_ERROR;
        locale.append(loc, وضعیت);
        إذا (U_FAILURE(وضعیت)) { أرجع; }
    }
    ثابت icu::Hashtable *currencySymbolsEquiv = getCurrSymbolsEquiv();
    لكل (;;) {
        UError_کد ec2 = U_ZERO_ERROR;
        // TODO: ures_openDirect?
        Local_UResource_Bundle_اشاره_گر rb(ures_open(U_ICUDATA_CURR, locale.داده(), &ec2));
        Local_UResource_Bundle_اشاره_گر curr(ures_get_By_کلید(rb.getAlias(), CURRENCIES, اشاره_تهی, &ec2));
        int32_t n = ures_get_اندازه(curr.getAlias());
        لكل (int32_t i=0; i<n; ++i) {
            Local_UResource_Bundle_اشاره_گر نامها(ures_get_By_شاخص(curr.getAlias(), i, اشاره_تهی, &ec2));
            int32_t len;
            s = ures_get_String_By_شاخص(نامها.getAlias(), UCURR_نماد_NAME, &len, &ec2);
            ++(*مجموع_currency_نماد_شمار);  // currency symbol
            إذا (currencySymbolsEquiv != اشاره_تهی) {
                *مجموع_currency_نماد_شمار += شمار_Equivalent(*currencySymbolsEquiv, UnicodeString(درست, s, len));
            }
            ++(*مجموع_currency_نماد_شمار); // iso code
            ++(*مجموع_currency_name_شمار); // long name
        }

        // currency plurals
        UError_کد ec3 = U_ZERO_ERROR;
        Local_UResource_Bundle_اشاره_گر curr_p(ures_get_By_کلید(rb.getAlias(), CURRENCYPLURALS, اشاره_تهی, &ec3));
        n = ures_get_اندازه(curr_p.getAlias());
        لكل (int32_t i=0; i<n; ++i) {
            Local_UResource_Bundle_اشاره_گر نامها(ures_get_By_شاخص(curr_p.getAlias(), i, اشاره_تهی, &ec3));
            *مجموع_currency_name_شمار += ures_get_اندازه(نامها.getAlias());
        }

        إذا (!fallback(locale)) {
            اكسر;
        }
    }
}

ساكن char16_t*
مقصد_فوقانی_Case(ثابت char16_t* منبع, int32_t len, ثابت حرف* locale) {
    char16_t* dest = اشاره_تهی;
    UError_کد ec = U_ZERO_ERROR;
    int32_t destLen = u_str_مقصد_فوقانی(dest, 0, منبع, len, locale, &ec);

    ec = U_ZERO_ERROR;
    dest = static_cast<char16_t*>(uprv_malloc(حجم(char16_t) * MAX(destLen, len)));
    إذا (dest == اشاره_تهی) {
        أرجع اشاره_تهی;
    }
    u_str_مقصد_فوقانی(dest, destLen, منبع, len, locale, &ec);
    إذا (U_FAILURE(ec)) {
        u_memcpy(dest, منبع, len);
    } 
    أرجع dest;
}


ساكن فراغ delete_Currency_نامها(CurrencyNameStruct* currency_نامها, int32_t count);
// Collect all available currency names associated with the given locale
// (enable fallback chain).
// Read currenc names defined in resource bundle "Currencies" and
// "CurrencyPlural", enable fallback chain.
// return the malloc-ed currency name arrays and the total number of currency
// names in the array.
ساكن فراغ
collect_Currency_نامها(ثابت حرف* locale, 
                     CurrencyNameStruct** currency_نامها, 
                     int32_t* مجموع_currency_name_شمار, 
                     CurrencyNameStruct** currencySymbols, 
                     int32_t* مجموع_currency_نماد_شمار, 
                     UError_کد& ec) {
    إذا (U_FAILURE(ec)) {
        *currency_نامها = *currencySymbols = اشاره_تهی;
        *مجموع_currency_name_شمار = *مجموع_currency_نماد_شمار = 0;
        أرجع;
    }
    U_NAMESPACE_USE
    ثابت icu::Hashtable *currencySymbolsEquiv = getCurrSymbolsEquiv();
    // Look up the Currencies resource for the given locale.
    UError_کد ec2 = U_ZERO_ERROR;

    إذا (locale == اشاره_تهی) {
        locale = uloc_getDefault();
    }
    CharString loc = ulocimp_getName(locale, ec2);
    إذا (U_FAILURE(ec2)) {
        ec = U_ILLEGAL_آرگومان_ERROR;
        *currency_نامها = *currencySymbols = اشاره_تهی;
        *مجموع_currency_name_شمار = *مجموع_currency_نماد_شمار = 0;
        أرجع;
    }

    // Get maximum currency name count first.
    get_Currency_Name_شمار(loc.داده(), مجموع_currency_name_شمار, مجموع_currency_نماد_شمار);

    *currency_نامها = static_cast<CurrencyNameStruct*>(
        uprv_malloc(حجم(CurrencyNameStruct) * (*مجموع_currency_name_شمار)));
    إذا(*currency_نامها == اشاره_تهی) {
        *currencySymbols = اشاره_تهی;
        *مجموع_currency_name_شمار = *مجموع_currency_نماد_شمار = 0;
        ec = U_حافظه_تخصیص_ERROR;
        أرجع;
    }
    *currencySymbols = static_cast<CurrencyNameStruct*>(
        uprv_malloc(حجم(CurrencyNameStruct) * (*مجموع_currency_نماد_شمار)));

    إذا(*currencySymbols == اشاره_تهی) {
        uprv_free(*currency_نامها);
        *currency_نامها = اشاره_تهی;
        *مجموع_currency_name_شمار = *مجموع_currency_نماد_شمار = 0;
        ec = U_حافظه_تخصیص_ERROR;
        أرجع;
    }

    ثابت char16_t* s = اشاره_تهی;  // currency name
    حرف* iso = اشاره_تهی;  // currency ISO code

    *مجموع_currency_name_شمار = 0;
    *مجموع_currency_نماد_شمار = 0;

    UError_کد ec3 = U_ZERO_ERROR;
    UError_کد ec4 = U_ZERO_ERROR;

    // Using hash to remove duplicates caused by locale fallback
    Local_UHashtable_اشاره_گر currencyIsoCodes(uhash_open(uhash_hashChars, uhash_compareChars, اشاره_تهی, &ec3));
    Local_UHashtable_اشاره_گر currencyPluralIsoCodes(uhash_open(uhash_hashChars, uhash_compareChars, اشاره_تهی, &ec4));
    لكل (int32_t localeLevel = 0; ; ++localeLevel) {
        ec2 = U_ZERO_ERROR;
        // TODO: ures_openDirect
        Local_UResource_Bundle_اشاره_گر rb(ures_open(U_ICUDATA_CURR, loc.داده(), &ec2));
        Local_UResource_Bundle_اشاره_گر curr(ures_get_By_کلید(rb.getAlias(), CURRENCIES, اشاره_تهی, &ec2));
        int32_t n = ures_get_اندازه(curr.getAlias());
        لكل (int32_t i=0; i<n; ++i) {
            Local_UResource_Bundle_اشاره_گر نامها(ures_get_By_شاخص(curr.getAlias(), i, اشاره_تهی, &ec2));
            int32_t len;
            s = ures_get_String_By_شاخص(نامها.getAlias(), UCURR_نماد_NAME, &len, &ec2);
            // TODO: uhash_put wont change key/value?
            iso = const_cast<حرف*>(ures_get_کلید(نامها.getAlias()));
            إذا (localeLevel != 0 && uhash_get(currencyIsoCodes.getAlias(), iso) != اشاره_تهی) {
                استمر;
            }
            uhash_put(currencyIsoCodes.getAlias(), iso, iso, &ec3); 
            // Add currency symbol.
            (*currencySymbols)[(*مجموع_currency_نماد_شمار)++] = {iso, const_cast<char16_t*>(s), len, 0};

            // Add equivalent symbols
            إذا (currencySymbolsEquiv != اشاره_تهی) {
                UnicodeString str(درست, s, len);
                icu::EquivIterator iter(*currencySymbolsEquiv, str);
                ثابت UnicodeString *نماد;
                طالما ((نماد = iter.next()) != اشاره_تهی) {
                    (*currencySymbols)[(*مجموع_currency_نماد_شمار)++]
                        = {iso, const_cast<char16_t*>(نماد->get_میانگیر()), نماد->length(), 0};
                }
            }

            // Add currency long name.
            s = ures_get_String_By_شاخص(نامها.getAlias(), UCURR_LONG_NAME, &len, &ec2);
            char16_t* فوقانی_Name = مقصد_فوقانی_Case(s, len, locale);
            إذا (فوقانی_Name == اشاره_تهی) {
                ec = U_حافظه_تخصیص_ERROR;
                اذهب error;
            }
            (*currency_نامها)[(*مجموع_currency_name_شمار)++] = {iso, فوقانی_Name, len, NEED_مقصد_BE_DELETED};

            // put (iso, 3, and iso) in to array
            // Add currency ISO code.
            char16_t* iso_کد = static_cast<char16_t*>(uprv_malloc(حجم(char16_t) * 3));
            إذا (iso_کد == اشاره_تهی) {
                ec = U_حافظه_تخصیص_ERROR;
                اذهب error;
            }
            // Must convert iso[] into Unicode
            u_chars_مقصد_UChars(iso, iso_کد, 3);
            (*currencySymbols)[(*مجموع_currency_نماد_شمار)++] = {iso, iso_کد, 3, NEED_مقصد_BE_DELETED};
        }

        // currency plurals
        UError_کد ec5 = U_ZERO_ERROR;
        Local_UResource_Bundle_اشاره_گر curr_p(ures_get_By_کلید(rb.getAlias(), CURRENCYPLURALS, اشاره_تهی, &ec5));
        n = ures_get_اندازه(curr_p.getAlias());
        لكل (int32_t i=0; i<n; ++i) {
            Local_UResource_Bundle_اشاره_گر نامها(ures_get_By_شاخص(curr_p.getAlias(), i, اشاره_تهی, &ec5));
            iso = const_cast<حرف*>(ures_get_کلید(نامها.getAlias()));
            // Using hash to remove duplicated ISO codes in fallback chain.
            إذا (localeLevel != 0 && uhash_get(currencyPluralIsoCodes.getAlias(), iso) != اشاره_تهی) {
                استمر;
            }
            uhash_put(currencyPluralIsoCodes.getAlias(), iso, iso, &ec4);
            int32_t num = ures_get_اندازه(نامها.getAlias());
            int32_t len;
            لكل (int32_t j = 0; j < num; ++j) {
                // TODO: remove duplicates between singular name and 
                // currency long name?
                s = ures_get_String_By_شاخص(نامها.getAlias(), j, &len, &ec5);
                char16_t* فوقانی_Name = مقصد_فوقانی_Case(s, len, locale);
                إذا (فوقانی_Name == اشاره_تهی) {
                    ec = U_حافظه_تخصیص_ERROR;
                    اذهب error;
                }
                (*currency_نامها)[(*مجموع_currency_name_شمار)++] = {iso, فوقانی_Name, len, NEED_مقصد_BE_DELETED};
            }
        }

        إذا (!fallback(loc)) {
            اكسر;
        }
    }

    // quick sort the struct
    qsort(*currency_نامها, *مجموع_currency_name_شمار, 
          حجم(CurrencyNameStruct), currencyNameComparator);
    qsort(*currencySymbols, *مجموع_currency_نماد_شمار, 
          حجم(CurrencyNameStruct), currencyNameComparator);

#اگر_تعریف_شده UCURR_DEBUG
    چاپ_کن("currency name count: %d\n", *مجموع_currency_name_شمار);
    لكل (int32_t شاخص = 0; شاخص < *مجموع_currency_name_شمار; ++شاخص) {
        چاپ_کن("index: %d\n", شاخص);
        چاپ_کن("iso: %s\n", (*currency_نامها)[شاخص].Iso_کد);
        حرف curNameBuf[1024];
        memset(curNameBuf, 0, 1024);
        u_austrncpy(curNameBuf, (*currency_نامها)[شاخص].currencyName, (*currency_نامها)[شاخص].currencyNameLen);
        چاپ_کن("currencyName: %s\n", curNameBuf);
        چاپ_کن("len: %d\n", (*currency_نامها)[شاخص].currencyNameLen);
    }
    چاپ_کن("currency symbol count: %d\n", *مجموع_currency_نماد_شمار);
    لكل (int32_t شاخص = 0; شاخص < *مجموع_currency_نماد_شمار; ++شاخص) {
        چاپ_کن("index: %d\n", شاخص);
        چاپ_کن("iso: %s\n", (*currencySymbols)[شاخص].Iso_کد);
        حرف curNameBuf[1024];
        memset(curNameBuf, 0, 1024);
        u_austrncpy(curNameBuf, (*currencySymbols)[شاخص].currencyName, (*currencySymbols)[شاخص].currencyNameLen);
        چاپ_کن("currencySymbol: %s\n", curNameBuf);
        چاپ_کن("len: %d\n", (*currencySymbols)[شاخص].currencyNameLen);
    }
#پایان_اگر
    // fail on hashtable errors
    إذا (U_FAILURE(ec3)) {
      ec = ec3;
    } وإلا إذا (U_FAILURE(ec4)) {
      ec = ec4;
    }

  error:
    // clean up if we got error
    إذا (U_FAILURE(ec)) {
        delete_Currency_نامها(*currency_نامها, *مجموع_currency_name_شمار);
        delete_Currency_نامها(*currencySymbols, *مجموع_currency_نماد_شمار);
        *currency_نامها = *currencySymbols = اشاره_تهی;
        *مجموع_currency_name_شمار = *مجموع_currency_نماد_شمار = 0;
    }
}

// @param  currencyNames: currency names array
// @param  indexInCurrencyNames: the index of the character in currency names 
//         array against which the comparison is done
// @param  key: input text char to compare against
// @param  begin(IN/OUT): the begin index of matching range in currency names array
// @param  end(IN/OUT): the end index of matching range in currency names array.
ساكن int32_t
جستجوی_دودویی(ثابت CurrencyNameStruct* currency_نامها, 
             int32_t شاخص_درون_Currency_نامها,
             ثابت char16_t کلید,
             int32_t* begin, int32_t* انتها) {
#اگر_تعریف_شده UCURR_DEBUG
    چاپ_کن("key = %x\n", کلید);
#پایان_اگر
   int32_t first = *begin;
   int32_t آخرین = *انتها;
   طالما (first <= آخرین) {
       int32_t mid = (first + آخرین) / 2;  // compute mid point.
       إذا (شاخص_درون_Currency_نامها >= currency_نامها[mid].currencyNameLen) {
           first = mid + 1;
       } وإلا {
           إذا (کلید > currency_نامها[mid].currencyName[شاخص_درون_Currency_نامها]) {
               first = mid + 1;
           }
           وإلا إذا (کلید < currency_نامها[mid].currencyName[شاخص_درون_Currency_نامها]) {
               آخرین = mid - 1;
           }
           وإلا {
                // Find a match, and looking for ranges
                // Now do two more binary searches. First, on the left side for
                // the greatest L such that CurrencyNameStruct[L] < key.
                int32_t L = *begin;
                int32_t R = mid;

#اگر_تعریف_شده UCURR_DEBUG
                چاپ_کن("mid = %d\n", mid);
#پایان_اگر
                طالما (L < R) {
                    int32_t M = (L + R) / 2;
#اگر_تعریف_شده UCURR_DEBUG
                    چاپ_کن("L = %d, R = %d, M = %d\n", L, R, M);
#پایان_اگر
                    إذا (شاخص_درون_Currency_نامها >= currency_نامها[M].currencyNameLen) {
                        L = M + 1;
                    } وإلا {
                        إذا (currency_نامها[M].currencyName[شاخص_درون_Currency_نامها] < کلید) {
                            L = M + 1;
                        } وإلا {
#اگر_تعریف_شده UCURR_DEBUG
                            U_ASSERT(currency_نامها[M].currencyName[شاخص_درون_Currency_نامها] == کلید);
#پایان_اگر
                            R = M;
                        }
                    }
                }
#اگر_تعریف_شده UCURR_DEBUG
                U_ASSERT(L == R);
#پایان_اگر
                *begin = L;
#اگر_تعریف_شده UCURR_DEBUG
                چاپ_کن("begin = %d\n", *begin);
                U_ASSERT(currency_نامها[*begin].currencyName[شاخص_درون_Currency_نامها] == کلید);
#پایان_اگر

                // Now for the second search, finding the least R such that
                // key < CurrencyNameStruct[R].
                L = mid;
                R = *انتها;
                طالما (L < R) {
                    int32_t M = (L + R) / 2;
#اگر_تعریف_شده UCURR_DEBUG
                    چاپ_کن("L = %d, R = %d, M = %d\n", L, R, M);
#پایان_اگر
                    إذا (currency_نامها[M].currencyNameLen < شاخص_درون_Currency_نامها) {
                        L = M + 1;
                    } وإلا {
                        إذا (currency_نامها[M].currencyName[شاخص_درون_Currency_نامها] > کلید) {
                            R = M;
                        } وإلا {
#اگر_تعریف_شده UCURR_DEBUG
                            U_ASSERT(currency_نامها[M].currencyName[شاخص_درون_Currency_نامها] == کلید);
#پایان_اگر
                            L = M + 1;
                        }
                    }
                }
#اگر_تعریف_شده UCURR_DEBUG
                U_ASSERT(L == R);
#پایان_اگر
                إذا (currency_نامها[R].currencyName[شاخص_درون_Currency_نامها] > کلید) {
                    *انتها = R - 1;
                } وإلا {
                    *انتها = R;
                }
#اگر_تعریف_شده UCURR_DEBUG
                چاپ_کن("end = %d\n", *انتها);
#پایان_اگر

                // now, found the range. check whether there is exact match
                إذا (currency_نامها[*begin].currencyNameLen == شاخص_درون_Currency_نامها + 1) {
                    أرجع *begin;  // find range and exact match.
                }
                أرجع -1;  // find range, but no exact match.
           }
       }
   }
   *begin = -1;
   *انتها = -1;
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
خطی_جستجو(ثابت CurrencyNameStruct* currency_نامها, 
             int32_t begin, int32_t انتها,
             ثابت char16_t* متن, int32_t متن_Len,
             int32_t *partial_تطابق_Len,
             int32_t *max_تطابق_Len, int32_t* max_تطابق_شاخص) {
    int32_t initial_Partial_تطابق_Len = *partial_تطابق_Len;
    لكل (int32_t شاخص = begin; شاخص <= انتها; ++شاخص) {
        int32_t len = currency_نامها[شاخص].currencyNameLen;
        إذا (len > *max_تطابق_Len && len <= متن_Len &&
            uprv_memcmp(currency_نامها[شاخص].currencyName, متن, len * حجم(char16_t)) == 0) {
            *partial_تطابق_Len = MAX(*partial_تطابق_Len, len);
            *max_تطابق_شاخص = شاخص;
            *max_تطابق_Len = len;
#اگر_تعریف_شده UCURR_DEBUG
            چاپ_کن("maxMatchIndex = %d, maxMatchLen = %d\n",
                   *max_تطابق_شاخص, *max_تطابق_Len);
#پایان_اگر
        } وإلا {
            // Check for partial matches.
            لكل (int32_t i=initial_Partial_تطابق_Len; i<MIN(len, متن_Len); i++) {
                إذا (currency_نامها[شاخص].currencyName[i] != متن[i]) {
                    اكسر;
                }
                *partial_تطابق_Len = MAX(*partial_تطابق_Len, i + 1);
            }
        }
    }
}

#تعریف_کن خطی_جستجو_آستانه 10

// Find longest match between "text" and currency names in "currencyNames".
// @param  total_currency_count: total number of currency names in CurrencyNames.
// @param  textLen: the length of the text to be compared
// @param  maxMatchLen: passing in the computed max matching length
//                              pass out the new max  matching length
// @param  maxMatchIndex: the index in currencyName which has the longest
//                        match with input text.
ساكن فراغ
جستجو_Currency_Name(ثابت CurrencyNameStruct* currency_نامها, 
                   int32_t مجموع_currency_شمار,
                   ثابت char16_t* متن, int32_t متن_Len,
                   int32_t *partial_تطابق_Len,
                   int32_t* max_تطابق_Len, int32_t* max_تطابق_شاخص) {
    *max_تطابق_شاخص = -1;
    *max_تطابق_Len = 0;
    int32_t تطابق_شاخص = -1;
    int32_t دودویی_جستجو_Begin = 0;
    int32_t دودویی_جستجو_انتها = مجموع_currency_شمار - 1;
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
        تطابق_شاخص = جستجوی_دودویی(currency_نامها, شاخص,
                                  متن[شاخص],
                                  &دودویی_جستجو_Begin, &دودویی_جستجو_انتها);
        إذا (دودویی_جستجو_Begin == -1) { // did not find the range
            اكسر;
        }
        *partial_تطابق_Len = MAX(*partial_تطابق_Len, شاخص + 1);
        إذا (تطابق_شاخص != -1) { 
            // find an exact match for text from text[0] to text[index] 
            // in currencyNames array.
            *max_تطابق_Len = شاخص + 1;
            *max_تطابق_شاخص = تطابق_شاخص;
        }
        إذا (دودویی_جستجو_انتها - دودویی_جستجو_Begin < خطی_جستجو_آستانه) {
            // linear search if within threshold.
            خطی_جستجو(currency_نامها, دودویی_جستجو_Begin, دودویی_جستجو_انتها,
                         متن, متن_Len,
                         partial_تطابق_Len,
                         max_تطابق_Len, max_تطابق_شاخص);
            اكسر;
        }
    }
}

//========================= currency name cache =====================
تعريفنوع بنية {
    حرف locale[ULOC_FULLNAME_ظرفیت];  //key
    // currency names, case insensitive
    CurrencyNameStruct* currency_نامها;  //مقدار
    int32_t مجموع_Currency_Name_شمار;  // currency name count
    // currency symbols and ISO code, case sensitive
    CurrencyNameStruct* currencySymbols; //مقدار
    int32_t مجموع_Currency_نماد_شمار;  //شمار
    // reference count.
    // reference count is set to 1 when an entry is put to cache.
    // it increases by 1 before accessing, and decreased by 1 after accessing.
    // The entry is deleted when ref count is zero, which means 
    // the entry is replaced out of cache and no process is accessing it.
    int32_t ref_شمار;
} Currency_Name_Cache_مدخل;


#تعریف_کن CURRENCY_NAME_CACHE_رقم 10

// Reserve 10 cache entries.
ساكن Currency_Name_Cache_مدخل* currCache[CURRENCY_NAME_CACHE_رقم] = {اشاره_تهی};
// Using an index to indicate which entry to be replaced when cache is full.
// It is a simple round-robin replacement strategy.
ساكن int8_t current_Cache_مدخل_شاخص = 0;

ساكن UMutex gCurrencyCacheMutex;

// Cache deletion
ساكن فراغ
delete_Currency_نامها(CurrencyNameStruct* currency_نامها, int32_t count) {
    لكل (int32_t شاخص = 0; شاخص < count; ++شاخص) {
        إذا ( (currency_نامها[شاخص].flag & NEED_مقصد_BE_DELETED) ) {
            uprv_free(currency_نامها[شاخص].currencyName);
        }
    }
    uprv_free(currency_نامها);
}


ساكن فراغ
delete_Cache_مدخل(Currency_Name_Cache_مدخل* مدخل) {
    delete_Currency_نامها(مدخل->currency_نامها, مدخل->مجموع_Currency_Name_شمار);
    delete_Currency_نامها(مدخل->currencySymbols, مدخل->مجموع_Currency_نماد_شمار);
    uprv_free(مدخل);
}


// Cache clean up
ساكن UBool U_CALLCONV
currency_cache_cleanup() {
    لكل (int32_t i = 0; i < CURRENCY_NAME_CACHE_رقم; ++i) {
        إذا (currCache[i]) {
            delete_Cache_مدخل(currCache[i]);
            currCache[i] = اشاره_تهی;
        }
    }
    أرجع درست;
}


/**
 * Loads the currency name data from the cache, or from resource bundles if necessary.
 * The refCount is automatically incremented.  It is the caller's responsibility
 * to decrement it when done!
 */
ساكن Currency_Name_Cache_مدخل*
get_Cache_مدخل(ثابت حرف* locale, UError_کد& ec) {

    int32_t مجموع_currency_name_شمار = 0;
    CurrencyNameStruct* currency_نامها = اشاره_تهی;
    int32_t مجموع_currency_نماد_شمار = 0;
    CurrencyNameStruct* currencySymbols = اشاره_تهی;
    Currency_Name_Cache_مدخل* cache_مدخل = اشاره_تهی;

    umtx_قفل(&gCurrencyCacheMutex);
    // in order to handle racing correctly,
    // not putting 'search' in a separate function.
    int8_t یافت = -1;
    لكل (int8_t i = 0; i < CURRENCY_NAME_CACHE_رقم; ++i) {
        إذا (currCache[i]!= اشاره_تهی &&
            uprv_strcmp(locale, currCache[i]->locale) == 0) {
            یافت = i;
            اكسر;
        }
    }
    إذا (یافت != -1) {
        cache_مدخل = currCache[یافت];
        ++(cache_مدخل->ref_شمار);
    }
    umtx_unlock(&gCurrencyCacheMutex);
    إذا (یافت == -1) {
        collect_Currency_نامها(locale, &currency_نامها, &مجموع_currency_name_شمار, &currencySymbols, &مجموع_currency_نماد_شمار, ec);
        إذا (U_FAILURE(ec)) {
            أرجع اشاره_تهی;
        }
        umtx_قفل(&gCurrencyCacheMutex);
        // check again.
        لكل (int8_t i = 0; i < CURRENCY_NAME_CACHE_رقم; ++i) {
            إذا (currCache[i]!= اشاره_تهی &&
                uprv_strcmp(locale, currCache[i]->locale) == 0) {
                یافت = i;
                اكسر;
            }
        }
        إذا (یافت == -1) {
            // insert new entry to 
            // currentCacheEntryIndex % CURRENCY_NAME_CACHE_NUM
            // and remove the existing entry 
            // currentCacheEntryIndex % CURRENCY_NAME_CACHE_NUM
            // from cache.
            cache_مدخل = currCache[current_Cache_مدخل_شاخص];
            إذا (cache_مدخل) {
                --(cache_مدخل->ref_شمار);
                // delete if the ref count is zero
                إذا (cache_مدخل->ref_شمار == 0) {
                    delete_Cache_مدخل(cache_مدخل);
                }
            }
            cache_مدخل = static_cast<Currency_Name_Cache_مدخل*>(uprv_malloc(حجم(Currency_Name_Cache_مدخل)));
            إذا (cache_مدخل == اشاره_تهی) {
                delete_Currency_نامها(currency_نامها, مجموع_currency_name_شمار);
                delete_Currency_نامها(currencySymbols, مجموع_currency_نماد_شمار);
                ec = U_حافظه_تخصیص_ERROR;
                أرجع اشاره_تهی;
            }
            currCache[current_Cache_مدخل_شاخص] = cache_مدخل;
            uprv_strcpy(cache_مدخل->locale, locale);
            cache_مدخل->currency_نامها = currency_نامها;
            cache_مدخل->مجموع_Currency_Name_شمار = مجموع_currency_name_شمار;
            cache_مدخل->currencySymbols = currencySymbols;
            cache_مدخل->مجموع_Currency_نماد_شمار = مجموع_currency_نماد_شمار;
            cache_مدخل->ref_شمار = 2; // one for cache, one for reference
            current_Cache_مدخل_شاخص = (current_Cache_مدخل_شاخص + 1) % CURRENCY_NAME_CACHE_رقم;
            ucln_common_ثبات_Cleanup(UCLN_COMMON_CURRENCY, currency_cleanup);
        } وإلا {
            delete_Currency_نامها(currency_نامها, مجموع_currency_name_شمار);
            delete_Currency_نامها(currencySymbols, مجموع_currency_نماد_شمار);
            cache_مدخل = currCache[یافت];
            ++(cache_مدخل->ref_شمار);
        }
        umtx_unlock(&gCurrencyCacheMutex);
    }

    أرجع cache_مدخل;
}

ساكن فراغ release_Cache_مدخل(Currency_Name_Cache_مدخل* cache_مدخل) {
    umtx_قفل(&gCurrencyCacheMutex);
    --(cache_مدخل->ref_شمار);
    إذا (cache_مدخل->ref_شمار == 0) {  // remove
        delete_Cache_مدخل(cache_مدخل);
    }
    umtx_unlock(&gCurrencyCacheMutex);
}

U_CAPI فراغ
uprv_parseCurrency(ثابت حرف* locale,
                   ثابت icu::UnicodeString& متن,
                   icu::ParsePosition& pos,
                   int8_t type,
                   int32_t* partial_تطابق_Len,
                   char16_t* نتیجه,
                   UError_کد& ec) {
    U_NAMESPACE_USE
    إذا (U_FAILURE(ec)) {
        أرجع;
    }
    Currency_Name_Cache_مدخل* cache_مدخل = get_Cache_مدخل(locale, ec);
    إذا (U_FAILURE(ec)) {
        أرجع;
    }

    int32_t مجموع_currency_name_شمار = cache_مدخل->مجموع_Currency_Name_شمار;
    CurrencyNameStruct* currency_نامها = cache_مدخل->currency_نامها;
    int32_t مجموع_currency_نماد_شمار = cache_مدخل->مجموع_Currency_نماد_شمار;
    CurrencyNameStruct* currencySymbols = cache_مدخل->currencySymbols;

    int32_t شروع = pos.get_شاخص();

    char16_t درون_داد_متن[MAX_CURRENCY_NAME_LEN];
    char16_t فوقانی_متن[MAX_CURRENCY_NAME_LEN];
    int32_t متن_Len = MIN(MAX_CURRENCY_NAME_LEN, متن.length() - شروع);
    متن.extract(شروع, متن_Len, درون_داد_متن);
    UError_کد ec1 = U_ZERO_ERROR;
    متن_Len = u_str_مقصد_فوقانی(فوقانی_متن, MAX_CURRENCY_NAME_LEN, درون_داد_متن, متن_Len, locale, &ec1);

    // Make sure partialMatchLen is initialized
    *partial_تطابق_Len = 0;

    int32_t max = 0;
    int32_t تطابق_شاخص = -1;
    // case in-sensitive comparison against currency names
    جستجو_Currency_Name(currency_نامها, مجموع_currency_name_شمار, 
                       فوقانی_متن, متن_Len, partial_تطابق_Len, &max, &تطابق_شاخص);

#اگر_تعریف_شده UCURR_DEBUG
    چاپ_کن("search in names, max = %d, matchIndex = %d\n", max, تطابق_شاخص);
#پایان_اگر

    int32_t max_درون_نماد = 0;
    int32_t تطابق_شاخص_درون_نماد = -1;
    إذا (type != UCURR_LONG_NAME) {  // not name only
        // case sensitive comparison against currency symbols and ISO code.
        جستجو_Currency_Name(currencySymbols, مجموع_currency_نماد_شمار, 
                           درون_داد_متن, متن_Len,
                           partial_تطابق_Len,
                           &max_درون_نماد, &تطابق_شاخص_درون_نماد);
    }

#اگر_تعریف_شده UCURR_DEBUG
    چاپ_کن("search in symbols, maxInSymbol = %d, matchIndexInSymbol = %d\n", max_درون_نماد, تطابق_شاخص_درون_نماد);
    إذا(تطابق_شاخص_درون_نماد != -1) {
      چاپ_کن("== ISO=%s\n", currencySymbols[تطابق_شاخص_درون_نماد].Iso_کد);
    }
#پایان_اگر

    إذا (max >= max_درون_نماد && تطابق_شاخص != -1) {
        u_chars_مقصد_UChars(currency_نامها[تطابق_شاخص].Iso_کد, نتیجه, 4);
        pos.مجموعه_شاخص(شروع + max);
    } وإلا إذا (max_درون_نماد >= max && تطابق_شاخص_درون_نماد != -1) {
        u_chars_مقصد_UChars(currencySymbols[تطابق_شاخص_درون_نماد].Iso_کد, نتیجه, 4);
        pos.مجموعه_شاخص(شروع + max_درون_نماد);
    }

    // decrease reference count
    release_Cache_مدخل(cache_مدخل);
}

فراغ uprv_currencyLeads(ثابت حرف* locale, icu::Unicode_مجموعه& نتیجه, UError_کد& ec) {
    U_NAMESPACE_USE
    إذا (U_FAILURE(ec)) {
        أرجع;
    }
    Currency_Name_Cache_مدخل* cache_مدخل = get_Cache_مدخل(locale, ec);
    إذا (U_FAILURE(ec)) {
        أرجع;
    }

    لكل (int32_t i=0; i<cache_مدخل->مجموع_Currency_نماد_شمار; i++) {
        ثابت CurrencyNameStruct& info = cache_مدخل->currencySymbols[i];
        UChar32 cp;
        U16_GET(info.currencyName, 0, 0, info.currencyNameLen, cp);
        نتیجه.جمع_کن(cp);
    }

    لكل (int32_t i=0; i<cache_مدخل->مجموع_Currency_Name_شمار; i++) {
        ثابت CurrencyNameStruct& info = cache_مدخل->currency_نامها[i];
        UChar32 cp;
        U16_GET(info.currencyName, 0, 0, info.currencyNameLen, cp);
        نتیجه.جمع_کن(cp);
    }

    // decrease reference count
    release_Cache_مدخل(cache_مدخل);
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
                           icu::UnicodeString& نتیجه, UError_کد& ec)
{
    U_NAMESPACE_USE

    int32_t len;
    ثابت char16_t* currname = ucurr_getName(iso, loc, UCURR_نماد_NAME,
                                          اشاره_تهی /* isChoiceFormat */, &len, &ec);
    إذا (U_SUCCESS(ec)) {
        نتیجه.مجموعه_مقصد(currname, len);
    }
}

U_CAPI int32_t U_EXPORT2
ucurr_getDefaultFractionDigits(ثابت char16_t* currency, UError_کد* ec) {
    أرجع ucurr_getDefaultFractionDigitsForUsage(currency,UCURR_USAGE_STANDARD,ec);
}

U_CAPI int32_t U_EXPORT2
ucurr_getDefaultFractionDigitsForUsage(ثابت char16_t* currency, ثابت UCurrencyUsage usage, UError_کد* ec) {
    int32_t fracDigits = 0;
    إذا (U_SUCCESS(*ec)) {
        بدل (usage) {
            حالة UCURR_USAGE_STANDARD:
                fracDigits = (پیدا_کن_Meta_داده(currency, *ec))[0];
                اكسر;
            حالة UCURR_USAGE_CASH:
                fracDigits = (پیدا_کن_Meta_داده(currency, *ec))[2];
                اكسر;
            افتراضي:
                *ec = U_UNSUPPORTED_ERROR;
        }
    }
    أرجع fracDigits;
}

U_CAPI مزدوج U_EXPORT2
ucurr_getRoundingIncrement(ثابت char16_t* currency, UError_کد* ec) {
    أرجع ucurr_getRoundingIncrementForUsage(currency, UCURR_USAGE_STANDARD, ec);
}

U_CAPI مزدوج U_EXPORT2
ucurr_getRoundingIncrementForUsage(ثابت char16_t* currency, ثابت UCurrencyUsage usage, UError_کد* ec) {
    مزدوج نتیجه = 0.0;

    ثابت int32_t *داده = پیدا_کن_Meta_داده(currency, *ec);
    إذا (U_SUCCESS(*ec)) {
        int32_t fracDigits;
        int32_t increment;
        بدل (usage) {
            حالة UCURR_USAGE_STANDARD:
                fracDigits = داده[0];
                increment = داده[1];
                اكسر;
            حالة UCURR_USAGE_CASH:
                fracDigits = داده[2];
                increment = داده[3];
                اكسر;
            افتراضي:
                *ec = U_UNSUPPORTED_ERROR;
                أرجع نتیجه;
        }

        // If the meta data is invalid, return 0.0
        إذا (fracDigits < 0 || fracDigits > MAX_POW10) {
            *ec = U_INVALID_FORMAT_ERROR;
        } وإلا {
            // A rounding value of 0 or 1 indicates no rounding.
            إذا (increment >= 2) {
                // Return (increment) / 10^(fracDigits).  The only actual rounding data,
                // as of this writing, is CHF { 2, 5 }.
                نتیجه = مزدوج(increment) / POW10[fracDigits];
            }
        }
    }

    أرجع نتیجه;
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
    { اشاره_تهی, 0 } // Leave here to denote the end of the list.
};

#تعریف_کن UCURR_MATCHES_BITMASK(variable, type_مقصد_تطابق) \
    ((type_مقصد_تطابق) == UCURR_ALL || ((variable) & (type_مقصد_تطابق)) == (type_مقصد_تطابق))

ساكن int32_t U_CALLCONV
ucurr_شمار_Currency_List(UEnumeration *enumerator, UError_کد * /*pErrorCode*/) {
    UCurrencyContext *myContext = (UCurrencyContext *)(enumerator->context);
    uint32_t currType = myContext->currType;
    int32_t count = 0;

    /* Count the number of items matching the type we are looking for. */
    لكل (int32_t idx = 0; gCurrencyList[idx].currency != اشاره_تهی; idx++) {
        إذا (UCURR_MATCHES_BITMASK(gCurrencyList[idx].currType, currType)) {
            count++;
        }
    }
    أرجع count;
}

ساكن ثابت حرف* U_CALLCONV
ucurr_بعدی_Currency_List(UEnumeration *enumerator,
                        int32_t* نتیجه_درازی,
                        UError_کد * /*pErrorCode*/)
{
    UCurrencyContext *myContext = (UCurrencyContext *)(enumerator->context);

    /* Find the next in the list that matches the type we are looking for. */
    طالما (myContext->listIdx < UPRV_LENGTHOF(gCurrencyList)-1) {
        ثابت بنية CurrencyList *curr_قلم = &gCurrencyList[myContext->listIdx++];
        إذا (UCURR_MATCHES_BITMASK(curr_قلم->currType, myContext->currType))
        {
            إذا (نتیجه_درازی) {
                *نتیجه_درازی = 3; /* Currency codes are only 3 chars long */
            }
            أرجع curr_قلم->currency;
        }
    }
    /* We enumerated too far. */
    إذا (نتیجه_درازی) {
        *نتیجه_درازی = 0;
    }
    أرجع اشاره_تهی;
}

ساكن فراغ U_CALLCONV
ucurr_resetCurrencyList(UEnumeration *enumerator, UError_کد * /*pErrorCode*/) {
    ((UCurrencyContext *)(enumerator->context))->listIdx = 0;
}

ساكن فراغ U_CALLCONV
ucurr_closeCurrencyList(UEnumeration *enumerator) {
    uprv_free(enumerator->context);
    uprv_free(enumerator);
}

ساكن فراغ U_CALLCONV
ucurr_createCurrencyList(UHashtable *isoCodes, UError_کد* وضعیت){
    UError_کد local_وضعیت = U_ZERO_ERROR;

    // Look up the CurrencyMap element in the root bundle.
    UResourceBundle *rb = ures_open_مستقیم(U_ICUDATA_CURR, CURRENCY_داده, &local_وضعیت);
    Local_UResource_Bundle_اشاره_گر currencyMapArray(ures_get_By_کلید(rb, CURRENCY_MAP, rb, &local_وضعیت));

    إذا (U_SUCCESS(local_وضعیت)) {
        // process each entry in currency map
        لكل (int32_t i=0; i<ures_get_اندازه(currencyMapArray.getAlias()); i++) {
            // get the currency resource
            Local_UResource_Bundle_اشاره_گر currencyArray(ures_get_By_شاخص(currencyMapArray.getAlias(), i, اشاره_تهی, &local_وضعیت));
            // process each currency
            إذا (U_SUCCESS(local_وضعیت)) {
                لكل (int32_t j=0; j<ures_get_اندازه(currencyArray.getAlias()); j++) {
                    // get the currency resource
                    Local_UResource_Bundle_اشاره_گر currencyRes(ures_get_By_شاخص(currencyArray.getAlias(), j, اشاره_تهی, &local_وضعیت));
                    Iso_کد_مدخل *مدخل = (Iso_کد_مدخل*)uprv_malloc(حجم(Iso_کد_مدخل));
                    إذا (مدخل == اشاره_تهی) {
                        *وضعیت = U_حافظه_تخصیص_ERROR;
                        أرجع;
                    }

                    // get the ISO code
                    int32_t iso_درازی = 0;
                    Local_UResource_Bundle_اشاره_گر idRes(ures_get_By_کلید(currencyRes.getAlias(), "id", اشاره_تهی, &local_وضعیت));
                    إذا (idRes.isNull()) {
                        استمر;
                    }
                    ثابت char16_t *iso_کد = ures_getString(idRes.getAlias(), &iso_درازی, &local_وضعیت);

                    // get from date
                    UDate fromDate = U_DATE_MIN;
                    Local_UResource_Bundle_اشاره_گر fromRes(ures_get_By_کلید(currencyRes.getAlias(), "from", اشاره_تهی, &local_وضعیت));

                    إذا (U_SUCCESS(local_وضعیت)) {
                        int32_t from_درازی = 0;
                        ثابت int32_t *fromArray = ures_get_Int_وکتور(fromRes.getAlias(), &from_درازی, &local_وضعیت);
                        int64_t currDate64 = ((uint64_t)fromArray[0]) << 32;
                        currDate64 |= ((int64_t)fromArray[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                        fromDate = (UDate)currDate64;
                    }

                    // get to date
                    UDate مقصد_Date = U_DATE_MAX;
                    local_وضعیت = U_ZERO_ERROR;
                    Local_UResource_Bundle_اشاره_گر مقصد_Res(ures_get_By_کلید(currencyRes.getAlias(), "to", اشاره_تهی, &local_وضعیت));

                    إذا (U_SUCCESS(local_وضعیت)) {
                        int32_t مقصد_درازی = 0;
                        ثابت int32_t *مقصد_Array = ures_get_Int_وکتور(مقصد_Res.getAlias(), &مقصد_درازی, &local_وضعیت);
                        int64_t currDate64 = (uint64_t)مقصد_Array[0] << 32;
                        currDate64 |= ((int64_t)مقصد_Array[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                        مقصد_Date = (UDate)currDate64;
                    }

                    مدخل->iso_کد = iso_کد;
                    مدخل->from = fromDate;
                    مدخل->مقصد = مقصد_Date;

                    local_وضعیت = U_ZERO_ERROR;
                    uhash_put(isoCodes, (char16_t *)iso_کد, مدخل, &local_وضعیت);
                }
            } وإلا {
                *وضعیت = local_وضعیت;
            }
        }
    } وإلا {
        *وضعیت = local_وضعیت;
    }
}

ساكن ثابت UEnumeration gEnumCurrencyList = {
    اشاره_تهی,
    اشاره_تهی,
    ucurr_closeCurrencyList,
    ucurr_شمار_Currency_List,
    uenum_unextDefault,
    ucurr_بعدی_Currency_List,
    ucurr_resetCurrencyList
};
U_CDECL_انتها


ساكن فراغ U_CALLCONV initIsoCodes(UError_کد &وضعیت) {
    U_ASSERT(gIsoCodes == اشاره_تهی);
    ucln_common_ثبات_Cleanup(UCLN_COMMON_CURRENCY, currency_cleanup);

    Local_UHashtable_اشاره_گر isoCodes(uhash_open(uhash_hashUChars, uhash_compareUChars, اشاره_تهی, &وضعیت));
    إذا (U_FAILURE(وضعیت)) {
        أرجع;
    }
    uhash_مجموعه_مقدار_Deleter(isoCodes.getAlias(), delete_Iso_کد_مدخل);

    ucurr_createCurrencyList(isoCodes.getAlias(), &وضعیت);
    إذا (U_FAILURE(وضعیت)) {
        أرجع;
    }
    gIsoCodes = isoCodes.orphan();  // Note: gIsoCodes is const. Once set up here it is never altered,
                                    //       and read only access is safe without synchronization.
}

ساكن فراغ populateCurrSymbolsEquiv(icu::Hashtable *hash, UError_کد &وضعیت) {
    إذا (U_FAILURE(وضعیت)) { أرجع; }
    لكل (ثابت خودکار& مدخل : unisets::kCurrencyEntries) {
        UnicodeString exemplar(مدخل.exemplar);
        ثابت Unicode_مجموعه* مجموعه = unisets::get(مدخل.کلید);
        إذا (مجموعه == اشاره_تهی) { أرجع; }
        Unicode_مجموعه_Iterator it(*مجموعه);
        طالما (it.next()) {
            UnicodeString value = it.getString();
            إذا (value == exemplar) {
                // No need to mark the exemplar character as an equivalent
                استمر;
            }
            makeEquivalent(exemplar, value, hash, وضعیت);
            إذا (U_FAILURE(وضعیت)) { أرجع; }
        }
    }
}

ساكن فراغ U_CALLCONV initCurrSymbolsEquiv() {
    U_ASSERT(gCurrSymbolsEquiv == اشاره_تهی);
    UError_کد وضعیت = U_ZERO_ERROR;
    ucln_common_ثبات_Cleanup(UCLN_COMMON_CURRENCY, currency_cleanup);
    icu::Hashtable *موقت = جديد icu::Hashtable(وضعیت);
    إذا (موقت == اشاره_تهی) {
        أرجع;
    }
    إذا (U_FAILURE(وضعیت)) {
        حذف موقت;
        أرجع;
    }
    موقت->مجموعه_مقدار_Deleter(deleteUnicode);
    populateCurrSymbolsEquiv(موقت, وضعیت);
    إذا (U_FAILURE(وضعیت)) {
        حذف موقت;
        أرجع;
    }
    gCurrSymbolsEquiv = موقت;
}

U_CAPI UBool U_EXPORT2
ucurr_is_موجود(ثابت char16_t* iso_کد, UDate from, UDate مقصد, UError_کد* e_Error_کد) {
    umtx_initOnce(gIsoCodesInitOnce, &initIsoCodes, *e_Error_کد);
    إذا (U_FAILURE(*e_Error_کد)) {
        أرجع خطأ;
    }

    Iso_کد_مدخل* نتیجه = (Iso_کد_مدخل *) uhash_get(gIsoCodes, iso_کد);
    إذا (نتیجه == اشاره_تهی) {
        أرجع خطأ;
    } وإلا إذا (from > مقصد) {
        *e_Error_کد = U_ILLEGAL_آرگومان_ERROR;
        أرجع خطأ;
    } وإلا إذا  ((from > نتیجه->مقصد) || (مقصد < نتیجه->from)) {
        أرجع خطأ;
    }
    أرجع درست;
}

ساكن ثابت icu::Hashtable* getCurrSymbolsEquiv() {
    umtx_initOnce(gCurrSymbolsEquivInitOnce, &initCurrSymbolsEquiv);
    أرجع gCurrSymbolsEquiv;
}

U_CAPI UEnumeration * U_EXPORT2
ucurr_openISOCurrencies(uint32_t currType, UError_کد *p_Error_کد) {
    UEnumeration *myEnum = اشاره_تهی;
    UCurrencyContext *myContext;

    myEnum = (UEnumeration*)uprv_malloc(حجم(UEnumeration));
    إذا (myEnum == اشاره_تهی) {
        *p_Error_کد = U_حافظه_تخصیص_ERROR;
        أرجع اشاره_تهی;
    }
    uprv_memcpy(myEnum, &gEnumCurrencyList, حجم(UEnumeration));
    myContext = (UCurrencyContext*)uprv_malloc(حجم(UCurrencyContext));
    إذا (myContext == اشاره_تهی) {
        *p_Error_کد = U_حافظه_تخصیص_ERROR;
        uprv_free(myEnum);
        أرجع اشاره_تهی;
    }
    myContext->currType = currType;
    myContext->listIdx = 0;
    myEnum->context = myContext;
    أرجع myEnum;
}

U_CAPI int32_t U_EXPORT2
ucurr_شمار_Currencies(ثابت حرف* locale, 
                 UDate date, 
                 UError_کد* ec)
{
    int32_t curr_شمار = 0;

    إذا (ec != اشاره_تهی && U_SUCCESS(*ec)) 
    {
        // local variables
        UError_کد local_وضعیت = U_ZERO_ERROR;

        // get country or country_variant in `id'
        CharString id = idForLocale(locale, ec);

        إذا (U_FAILURE(*ec))
        {
            أرجع 0;
        }

        // Remove variants, which is only needed for registration.
        حرف *idDelim = strchr(id.داده(), VAR_DELIM);
        إذا (idDelim)
        {
            id.truncate(idDelim - id.داده());
        }

        // Look up the CurrencyMap element in the root bundle.
        UResourceBundle *rb = ures_open_مستقیم(U_ICUDATA_CURR, CURRENCY_داده, &local_وضعیت);
        UResourceBundle *cm = ures_get_By_کلید(rb, CURRENCY_MAP, rb, &local_وضعیت);

        // Using the id derived from the local, get the currency data
        Local_UResource_Bundle_اشاره_گر countryArray(ures_get_By_کلید(rb, id.داده(), cm, &local_وضعیت));

        // process each currency to see which one is valid for the given date
        إذا (U_SUCCESS(local_وضعیت))
        {
            لكل (int32_t i=0; i<ures_get_اندازه(countryArray.getAlias()); i++)
            {
                // get the currency resource
                Local_UResource_Bundle_اشاره_گر currencyRes(ures_get_By_شاخص(countryArray.getAlias(), i, اشاره_تهی, &local_وضعیت));

                // get the from date
                int32_t from_درازی = 0;
                Local_UResource_Bundle_اشاره_گر fromRes(ures_get_By_کلید(currencyRes.getAlias(), "from", اشاره_تهی, &local_وضعیت));
                ثابت int32_t *fromArray = ures_get_Int_وکتور(fromRes.getAlias(), &from_درازی, &local_وضعیت);

                int64_t currDate64 = (int64_t)((uint64_t)(fromArray[0]) << 32);
                currDate64 |= ((int64_t)fromArray[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                UDate fromDate = (UDate)currDate64;

                إذا (ures_get_اندازه(currencyRes.getAlias())> 2)
                {
                    int32_t مقصد_درازی = 0;
                    Local_UResource_Bundle_اشاره_گر مقصد_Res(ures_get_By_کلید(currencyRes.getAlias(), "to", اشاره_تهی, &local_وضعیت));
                    ثابت int32_t *مقصد_Array = ures_get_Int_وکتور(مقصد_Res.getAlias(), &مقصد_درازی, &local_وضعیت);

                    currDate64 = (int64_t)مقصد_Array[0] << 32;
                    currDate64 |= ((int64_t)مقصد_Array[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                    UDate مقصد_Date = (UDate)currDate64;

                    إذا ((fromDate <= date) && (date < مقصد_Date))
                    {
                        curr_شمار++;
                    }
                }
                وإلا
                {
                    إذا (fromDate <= date)
                    {
                        curr_شمار++;
                    }
                }
            } // end For loop
        } // end if (U_SUCCESS(localStatus))

        // Check for errors
        إذا (*ec == U_ZERO_ERROR || local_وضعیت != U_ZERO_ERROR)
        {
            // There is nothing to fallback to. 
            // Report the failure/warning if possible.
            *ec = local_وضعیت;
        }

        إذا (U_SUCCESS(*ec))
        {
            // no errors
            أرجع curr_شمار;
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
                UError_کد* ec)
{
    int32_t resLen = 0;
	int32_t curr_شاخص = 0;
    ثابت char16_t* s = اشاره_تهی;

    إذا (ec != اشاره_تهی && U_SUCCESS(*ec))
    {
        // check the arguments passed
        إذا ((buff && buff_ظرفیت) || !buff_ظرفیت )
        {
            // local variables
            UError_کد local_وضعیت = U_ZERO_ERROR;

            // get country or country_variant in `id'
            CharString id = idForLocale(locale, ec);
            إذا (U_FAILURE(*ec))
            {
                أرجع 0;
            }

            // Remove variants, which is only needed for registration.
            حرف *idDelim = strchr(id.داده(), VAR_DELIM);
            إذا (idDelim)
            {
                id.truncate(idDelim - id.داده());
            }

            // Look up the CurrencyMap element in the root bundle.
            UResourceBundle *rb = ures_open_مستقیم(U_ICUDATA_CURR, CURRENCY_داده, &local_وضعیت);
            UResourceBundle *cm = ures_get_By_کلید(rb, CURRENCY_MAP, rb, &local_وضعیت);

            // Using the id derived from the local, get the currency data
            Local_UResource_Bundle_اشاره_گر countryArray(ures_get_By_کلید(rb, id.داده(), cm, &local_وضعیت));

            // process each currency to see which one is valid for the given date
            منطقي تطابق_یافت = خطأ;
            إذا (U_SUCCESS(local_وضعیت))
            {
                إذا ((شاخص <= 0) || (شاخص> ures_get_اندازه(countryArray.getAlias())))
                {
                    // requested index is out of bounds
                    أرجع 0;
                }

                لكل (int32_t i=0; i<ures_get_اندازه(countryArray.getAlias()); i++)
                {
                    // get the currency resource
                    Local_UResource_Bundle_اشاره_گر currencyRes(ures_get_By_شاخص(countryArray.getAlias(), i, اشاره_تهی, &local_وضعیت));
                    s = ures_get_String_By_کلید(currencyRes.getAlias(), "id", &resLen, &local_وضعیت);

                    // get the from date
                    int32_t from_درازی = 0;
                    Local_UResource_Bundle_اشاره_گر fromRes(ures_get_By_کلید(currencyRes.getAlias(), "from", اشاره_تهی, &local_وضعیت));
                    ثابت int32_t *fromArray = ures_get_Int_وکتور(fromRes.getAlias(), &from_درازی, &local_وضعیت);

                    int64_t currDate64 = (int64_t)((uint64_t)fromArray[0] << 32);
                    currDate64 |= ((int64_t)fromArray[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                    UDate fromDate = (UDate)currDate64;

                    إذا (ures_get_اندازه(currencyRes.getAlias()) > 2)
                    {
                        int32_t مقصد_درازی = 0;
                        Local_UResource_Bundle_اشاره_گر مقصد_Res(ures_get_By_کلید(currencyRes.getAlias(), "to", اشاره_تهی, &local_وضعیت));
                        ثابت int32_t *مقصد_Array = ures_get_Int_وکتور(مقصد_Res.getAlias(), &مقصد_درازی, &local_وضعیت);

                        currDate64 = (int64_t)مقصد_Array[0] << 32;
                        currDate64 |= ((int64_t)مقصد_Array[1] & (int64_t)INT64_C(0x00000000FFFFFFFF));
                        UDate مقصد_Date = (UDate)currDate64;

                        إذا ((fromDate <= date) && (date < مقصد_Date))
                        {
                            curr_شاخص++;
                            إذا (curr_شاخص == شاخص)
                            {
                                تطابق_یافت = درست;
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
                                تطابق_یافت = درست;
                            }
                        }
                    }
                    // check for loop exit
                    إذا (تطابق_یافت)
                    {
                        اكسر;
                    }

                } // end For loop
            }

            // Check for errors
            إذا (*ec == U_ZERO_ERROR || local_وضعیت != U_ZERO_ERROR)
            {
                // There is nothing to fallback to. 
                // Report the failure/warning if possible.
                *ec = local_وضعیت;
            }

            إذا (U_SUCCESS(*ec))
            {
                // no errors
                إذا((buff_ظرفیت> resLen) && تطابق_یافت)
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
            *ec = U_ILLEGAL_آرگومان_ERROR;
        }

    }

    // If we got here, either error code is invalid or
    // some argument passed is no good.
    أرجع resLen;
}

ساكن ثابت UEnumeration default_Keyword_مقادیر = {
    اشاره_تهی,
    اشاره_تهی,
    ulist_close_keyword_مقادیر_iterator,
    ulist_شمار_keyword_مقادیر,
    uenum_unextDefault,
    ulist_بعدی_keyword_مقدار, 
    ulist_reset_keyword_مقادیر_iterator
};

U_CAPI UEnumeration *U_EXPORT2 ucurr_get_Keyword_مقادیر_For_Locale(ثابت حرف *کلید, ثابت حرف *locale, UBool commonly_استفاده_شده, UError_کد* وضعیت) {
    // Resolve region
    CharString prefRegion = ulocimp_get_Region_For_Supplemental_داده(locale, درست, *وضعیت);

    // Read value from supplementalData
    UList *مقادیر = ulist_create_خالی_List(وضعیت);
    UList *other_مقادیر = ulist_create_خالی_List(وضعیت);
    UEnumeration *en = (UEnumeration *)uprv_malloc(حجم(UEnumeration));
    إذا (U_FAILURE(*وضعیت) || en == اشاره_تهی) {
        إذا (en == اشاره_تهی) {
            *وضعیت = U_حافظه_تخصیص_ERROR;
        } وإلا {
            uprv_free(en);
        }
        ulist_deleteList(مقادیر);
        ulist_deleteList(other_مقادیر);
        أرجع اشاره_تهی;
    }
    کپی_حافظه(en, &default_Keyword_مقادیر, حجم(UEnumeration));
    en->context = مقادیر;
    
    UResourceBundle* rb = ures_open_مستقیم(U_ICUDATA_CURR, "supplementalData", وضعیت);
    Local_UResource_Bundle_اشاره_گر bundle(ures_get_By_کلید(rb, "CurrencyMap", rb, وضعیت));
    پشته_UResource_Bundle bundlekey, regbndl, curbndl, مقصد;
    
    طالما (U_SUCCESS(*وضعیت) && ures_has_بعدی(bundle.getAlias())) {
        ures_get_بعدی_Resource(bundle.getAlias(), bundlekey.getAlias(), وضعیت);
        إذا (U_FAILURE(*وضعیت)) {
            اكسر;
        }
        ثابت حرف *region = ures_get_کلید(bundlekey.getAlias());
        UBool isPrefRegion = prefRegion == region;
        إذا (!isPrefRegion && commonly_استفاده_شده) {
            // With commonlyUsed=true, we do not put
            // currencies for other regions in the
            // result list.
            استمر;
        }
        ures_get_By_کلید(bundle.getAlias(), region, regbndl.getAlias(), وضعیت);
        إذا (U_FAILURE(*وضعیت)) {
            اكسر;
        }
        طالما (U_SUCCESS(*وضعیت) && ures_has_بعدی(regbndl.getAlias())) {
            ures_get_بعدی_Resource(regbndl.getAlias(), curbndl.getAlias(), وضعیت);
            إذا (ures_getType(curbndl.getAlias()) != URES_جدول) {
                // Currently, an empty ARRAY is mixed in.
                استمر;
            }
            حرف *curID = (حرف *)uprv_malloc(حجم(حرف) * ULOC_KEYWORDS_ظرفیت);
            إذا (curID == اشاره_تهی) {
                *وضعیت = U_حافظه_تخصیص_ERROR;
                اكسر;
            }
            int32_t curIDLength = ULOC_KEYWORDS_ظرفیت;

#إذا U_CHARSET_FAMILY==U_ASCII_FAMILY
            ures_get_UTF8_String_By_کلید(curbndl.getAlias(), "id", curID, &curIDLength, درست, وضعیت);
            /* optimize - use the utf-8 string */
#وإلا
            {
                       ثابت char16_t* defString = ures_get_String_By_کلید(curbndl.getAlias(), "id", &curIDLength, وضعیت);
                       إذا(U_SUCCESS(*وضعیت)) {
			   إذا(curIDLength+1 > ULOC_KEYWORDS_ظرفیت) {
				*وضعیت = U_میانگیر_سرریز_ERROR;
			   } وإلا {
                           	u_UChars_مقصد_Chars(defString, curID, curIDLength+1);
			   }
                       }
            }
#پایان_اگر	

            إذا (U_FAILURE(*وضعیت)) {
                اكسر;
            }
            UBool has_مقصد = خطأ;
            ures_get_By_کلید(curbndl.getAlias(), "to", مقصد.getAlias(), وضعیت);
            إذا (U_FAILURE(*وضعیت)) {
                // Do nothing here...
                *وضعیت = U_ZERO_ERROR;
            } وإلا {
                has_مقصد = درست;
            }
            إذا (isPrefRegion && !has_مقصد && !ulist_شامل_است_String(مقادیر, curID, (int32_t)uprv_strlen(curID))) {
                // Currently active currency for the target country
                ulist_جمع_کن_قلم_انتها_List(مقادیر, curID, درست, وضعیت);
            } وإلا إذا (!ulist_شامل_است_String(other_مقادیر, curID, (int32_t)uprv_strlen(curID)) && !commonly_استفاده_شده) {
                ulist_جمع_کن_قلم_انتها_List(other_مقادیر, curID, درست, وضعیت);
            } وإلا {
                uprv_free(curID);
            }
        }
        
    }
    إذا (U_SUCCESS(*وضعیت)) {
        إذا (commonly_استفاده_شده) {
            إذا (ulist_get_List_اندازه(مقادیر) == 0) {
                // This could happen if no valid region is supplied in the input
                // locale. In this case, we use the CLDR's default.
                uenum_close(en);
                en = ucurr_get_Keyword_مقادیر_For_Locale(کلید, "und", درست, وضعیت);
            }
        } وإلا {
            // Consolidate the list
            حرف *value = اشاره_تهی;
            ulist_resetList(other_مقادیر);
            طالما ((value = (حرف *)ulist_get_بعدی(other_مقادیر)) != اشاره_تهی) {
                إذا (!ulist_شامل_است_String(مقادیر, value, (int32_t)uprv_strlen(value))) {
                    حرف *tmp_مقدار = (حرف *)uprv_malloc(حجم(حرف) * ULOC_KEYWORDS_ظرفیت);
                    إذا (tmp_مقدار == اشاره_تهی) {
                        *وضعیت = U_حافظه_تخصیص_ERROR;
                        اكسر;
                    }
                    uprv_memcpy(tmp_مقدار, value, uprv_strlen(value) + 1);
                    ulist_جمع_کن_قلم_انتها_List(مقادیر, tmp_مقدار, درست, وضعیت);
                    إذا (U_FAILURE(*وضعیت)) {
                        اكسر;
                    }
                }
            }
        }
        ulist_resetList((UList *)(en->context));
    } وإلا {
        ulist_deleteList(مقادیر);
        uprv_free(en);
        مقادیر = اشاره_تهی;
        en = اشاره_تهی;
    }
    ulist_deleteList(other_مقادیر);
    أرجع en;
}


U_CAPI int32_t U_EXPORT2
ucurr_get_Numeric_کد(ثابت char16_t* currency) {
    int32_t کد = 0;
    إذا (currency && u_strlen(currency) == ISO_CURRENCY_کد_درازی) {
        UError_کد وضعیت = U_ZERO_ERROR;

        UResourceBundle* bundle = ures_open_مستقیم(اشاره_تهی, "currencyNumericCodes", &وضعیت);
        Local_UResource_Bundle_اشاره_گر کد_Map(ures_get_By_کلید(bundle, "codeMap", bundle, &وضعیت));
        إذا (U_SUCCESS(وضعیت)) {
            حرف alpha_کد[ISO_CURRENCY_کد_درازی+1];
            my_UChars_مقصد_Chars(alpha_کد, currency);
            T_CString_مقصد_فوقانی_Case(alpha_کد);
            ures_get_By_کلید(کد_Map.getAlias(), alpha_کد, کد_Map.getAlias(), &وضعیت);
            صحيح tmp_کد = ures_getInt(کد_Map.getAlias(), &وضعیت);
            إذا (U_SUCCESS(وضعیت)) {
                کد = tmp_کد;
            }
        }
    }
    أرجع کد;
}
#پایان_اگر /* #if !UCONFIG_NO_FORMATTING */

//eof
